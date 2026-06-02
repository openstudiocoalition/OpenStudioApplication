#!/usr/bin/env python3
"""
For each of the 18 language .ts files:
1. Use OpenStudioApp_es.ts as the master template (complete source string coverage)
2. Overlay any existing translations from the target language file
3. Leave everything else as empty unfinished stubs
4. Submit all batch translation jobs simultaneously
5. Poll until all complete, then apply results

This ensures every language has identical source string coverage to Spanish.
Spanish is also included so its new strings are machine-translated alongside others.
"""

import argparse
import re
import time
import sys
import anthropic
from anthropic.types.message_create_params import MessageCreateParamsNonStreaming
from anthropic.types.messages.batch_create_params import Request

LANGUAGES = {
    "ar": "Arabic",
    "ca": "Catalan",
    "es": "Spanish",
    "de": "German",
    "el": "Greek",
    "fa": "Persian (Farsi)",
    "fr": "French",
    "he": "Hebrew",
    "hi": "Hindi",
    "id": "Indonesian",
    "it": "Italian",
    "ja": "Japanese",
    "ko": "Korean",
    "pl": "Polish",
    "pt": "Portuguese",
    "tr": "Turkish",
    "vi": "Vietnamese",
    "zh_CN": "Simplified Chinese",
}

SPANISH_TS = "translations/OpenStudioApp_es.ts"
TS_TEMPLATE = "translations/OpenStudioApp_{lang}.ts"
MODEL = "claude-haiku-4-5"
KEY_FILE = r"C:\Users\ml\OneDrive\ClaudeAPIkey.txt"


def make_system_prompt(language_name: str) -> str:
    return (
        f"You are a professional technical translator for building energy modeling software "
        f"(OpenStudio / EnergyPlus). "
        f"The user will give you an English word or phrase inside <translate>...</translate> tags. "
        f"Return ONLY the {language_name} translation of that text — nothing else. "
        f"No explanations, no notes, no questions. Just the translation.\n"
        f"Rules:\n"
        f"- If the text is a UI label, button, or short phrase, translate it naturally.\n"
        f"- If the text is a technical IDD field name, use standard engineering terminology.\n"
        f"- Preserve newlines (\\n) and HTML entities (&amp; &lt; &gt; &quot; &apos;) exactly.\n"
        f"- Do NOT translate acronyms or brand names (HVAC, EnergyPlus, OSM, IDF, kWh, W/m², COP, ASHRAE, AWS).\n"
        f"- Keep abbreviations in parentheses where present (e.g. 'Outdoor Air (OA)').\n"
        f"- Short words like 'yes', 'no', 'cancel' should be translated as common UI button labels."
    )


def build_translation_lookup(ts_content: str) -> dict[str, dict]:
    """
    Build {source_text: {'translation': ..., 'translatorcomment': ...}}
    for all non-empty translations. Preserves human-added translatorcomments.
    """
    pattern = re.compile(
        r'<source>([^<]+)</source>\s*'
        r'(?:<comment>[^<]*</comment>\s*)?'
        r'(?:<translatorcomment>(.*?)</translatorcomment>\s*)?'
        r'<translation[^>]*>(.*?)</translation>',
        re.DOTALL,
    )
    lookup = {}
    for m in pattern.finditer(ts_content):
        translation_text = m.group(3).strip()
        if translation_text:
            lookup[m.group(1).strip()] = {
                'translation': translation_text,
                'translatorcomment': m.group(2).strip() if m.group(2) else None,
            }
    return lookup


def build_merged_file(es_content: str, existing_lookup: dict[str, dict], lang_code: str) -> str:
    """
    Start from Spanish as template. For each message:
    - Replace translation with existing target-language translation if available
    - Preserve any human-added translatorcomment alongside the translation
    - Otherwise replace with empty unfinished stub
    Also update the <TS language="..."> attribute.
    """
    content = re.sub(
        r'(<TS[^>]*language=")[^"]*(")',
        rf'\g<1>{lang_code}\2',
        es_content,
    )

    def replace_translation(m):
        source = m.group(1).strip()
        existing = existing_lookup.get(source)
        if existing:
            parts = [f'<source>{m.group(1)}</source>']
            if existing.get('translatorcomment'):
                parts.append(f'        <translatorcomment>{existing["translatorcomment"]}</translatorcomment>')
            parts.append(f'        <translation>{existing["translation"]}</translation>')
            return '\n'.join(parts)
        return (
            f'<source>{m.group(1)}</source>\n'
            f'        <translation type="unfinished"></translation>'
        )

    content = re.sub(
        r'<source>([^<]+)</source>\s*<translation[^>]*>.*?</translation>',
        replace_translation,
        content,
        flags=re.DOTALL,
    )
    return content


def extract_unfinished(ts_content: str, skip_contexts: set[str] | None = None) -> list[dict]:
    """Return all unfinished entries in document order with their context name.

    skip_contexts: if provided, entries whose context name is in this set are
    still indexed (so apply_translations IDs stay stable) but marked
    skip=True so the caller can omit them from the batch request.
    """
    context_pattern = re.compile(
        r'<context>\s*<name>([^<]+)</name>(.*?)</context>',
        re.DOTALL,
    )
    message_pattern = re.compile(
        r'<message>\s*(?:(?:<!--.*?-->|<location[^>]*/?>)\s*)*<source>([^<]+)</source>\s*'
        r'<translation type="unfinished"></translation>\s*</message>',
        re.DOTALL,
    )
    entries = []
    i = 0
    for ctx_m in context_pattern.finditer(ts_content):
        ctx_name = ctx_m.group(1).strip()
        skip = bool(skip_contexts and ctx_name in skip_contexts)
        for msg_m in message_pattern.finditer(ctx_m.group(2)):
            entries.append({
                "id": f"entry-{i}",
                "source": msg_m.group(1).strip(),
                "context": ctx_name,
                "skip": skip,
            })
            i += 1
    return entries


def build_batch_requests(entries: list[dict], language_name: str) -> list[Request]:
    system_prompt = make_system_prompt(language_name)
    return [
        Request(
            custom_id=entry["id"],
            params=MessageCreateParamsNonStreaming(
                model=MODEL,
                max_tokens=256,
                system=system_prompt,
                messages=[{"role": "user", "content": f"<translate>{entry['source']}</translate>"}],
            ),
        )
        for entry in entries
    ]


def apply_translations(ts_content: str, id_to_translation: dict[str, str]) -> str:
    # Use the SAME pattern as extract_unfinished so XML-commented entries don't
    # cause a counter shift and map translations to the wrong source strings.
    find_pattern = re.compile(
        r'<message>\s*(?:(?:<!--.*?-->|<location[^>]*/?>)\s*)*<source>([^<]+)</source>\s*'
        r'<translation type="unfinished"></translation>\s*</message>',
        re.DOTALL,
    )
    matches = list(find_pattern.finditer(ts_content))
    result = ts_content
    for i, m in reversed(list(enumerate(matches))):
        translation = id_to_translation.get(f"entry-{i}", "")
        if translation:
            # Strip XML tags the model may have included
            translation = re.sub(r'<[^>]+>', '', translation).strip()
            source = m.group(1)
            replacement = (
                f"<message>\n        <source>{source}</source>\n"
                f"        <translation>{translation}</translation>\n    </message>"
            )
            result = result[:m.start()] + replacement + result[m.end():]
    return result


def main():
    parser = argparse.ArgumentParser(description="Batch-translate OpenStudioApp .ts files via Claude API")
    parser.add_argument(
        "--lang", nargs="+", metavar="CODE",
        help="Only process these language codes (e.g. --lang da fr). Default: all languages.",
    )
    parser.add_argument(
        "--skip-contexts", nargs="+", metavar="CTX", default=[],
        help="Skip translation for these .ts context names (e.g. --skip-contexts IDD OutputVariables).",
    )
    args = parser.parse_args()

    languages = {k: v for k, v in LANGUAGES.items() if not args.lang or k in args.lang}
    if not languages:
        sys.exit(f"ERROR: No matching languages for --lang {args.lang}. Valid codes: {sorted(LANGUAGES)}")
    skip_contexts = set(args.skip_contexts)
    if skip_contexts:
        print(f"Skipping contexts: {sorted(skip_contexts)}")

    key_text = open(KEY_FILE, encoding="utf-8").read()
    key_match = re.search(r"sk-ant-[A-Za-z0-9_\-]+", key_text)
    if not key_match:
        sys.exit("ERROR: No API key found in key file.")
    client = anthropic.Anthropic(api_key=key_match.group(0))

    print(f"Reading Spanish master: {SPANISH_TS}")
    es_content = open(SPANISH_TS, encoding="utf-8").read()
    print(f"  {es_content.count('<message>')} total source strings in Spanish template")

    # Phase 1: Build merged files and submit all batches
    batch_jobs: dict[str, dict] = {}

    for lang_code, lang_name in languages.items():
        ts_file = TS_TEMPLATE.format(lang=lang_code)
        print(f"\n[{lang_code}] {lang_name}")

        existing_content = open(ts_file, encoding="utf-8").read()
        existing_lookup = build_translation_lookup(existing_content)
        print(f"  {len(existing_lookup)} existing translations to preserve")

        merged = build_merged_file(es_content, existing_lookup, lang_code)
        all_entries = extract_unfinished(merged, skip_contexts=skip_contexts)
        entries = [e for e in all_entries if not e["skip"]]
        skipped = len(all_entries) - len(entries)
        if skipped:
            print(f"  {skipped} entries skipped (excluded contexts)")
        print(f"  {len(entries)} entries need translation")

        # Write merged file immediately so it's correct even if batch fails
        with open(ts_file, "w", encoding="utf-8") as f:
            f.write(merged)

        if not entries:
            print(f"  Nothing to translate")
            continue

        requests = build_batch_requests(entries, lang_name)
        batch = client.messages.batches.create(requests=requests)
        print(f"  Batch submitted: {batch.id}")
        batch_jobs[lang_code] = {
            "batch_id": batch.id,
            "ts_file": ts_file,
            "lang_name": lang_name,
        }

    if not batch_jobs:
        print("\nAll languages already complete.")
        return

    print(f"\nAll {len(batch_jobs)} batches submitted. Polling...")

    # Phase 2: Poll all batches in one loop
    pending = set(batch_jobs.keys())
    while pending:
        time.sleep(15)
        still_pending = set()
        for lang_code in sorted(pending):
            batch = client.messages.batches.retrieve(batch_jobs[lang_code]["batch_id"])
            if batch.processing_status == "ended":
                c = batch.request_counts
                print(f"  [{lang_code}] done — succeeded={c.succeeded}, errored={c.errored}")
            else:
                still_pending.add(lang_code)
        pending = still_pending
        if pending:
            print(f"  Still running: {', '.join(sorted(pending))}")

    # Phase 3: Apply translations
    print("\nApplying translations...")
    for lang_code, job in batch_jobs.items():
        id_to_translation: dict[str, str] = {}
        for result in client.messages.batches.results(job["batch_id"]):
            if result.result.type == "succeeded":
                msg = result.result.message
                text = next((b.text for b in msg.content if b.type == "text"), "").strip()
                id_to_translation[result.custom_id] = text
            else:
                print(f"  [{lang_code}] WARN: {result.custom_id} -> {result.result.type}")

        content = open(job["ts_file"], encoding="utf-8").read()
        new_content = apply_translations(content, id_to_translation)
        with open(job["ts_file"], "w", encoding="utf-8") as f:
            f.write(new_content)

        remaining = [e for e in extract_unfinished(new_content, skip_contexts=skip_contexts) if not e["skip"]]
        print(f"  [{lang_code}] {len(id_to_translation)} applied, {len(remaining)} empty remaining")

    print("\nAll done. Run lrelease on each .ts file to compile .qm files.")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""
Re-translate remaining empty entries using XML-delimited source text so the
model cannot interpret short strings like 'yes'/'no' as conversational replies.
"""

import re
import time
import sys
import anthropic
from anthropic.types.message_create_params import MessageCreateParamsNonStreaming
from anthropic.types.messages.batch_create_params import Request

LANGUAGES = {
    "ar": "Arabic",
    "ca": "Catalan",
    "de": "German",
    "el": "Greek",
    "es": "Spanish",
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

TS_TEMPLATE = "translations/OpenStudioApp_{lang}.ts"
MODEL = "claude-haiku-4-5"
KEY_FILE = r"C:\Users\ml\OneDrive\ClaudeAPIkey.txt"


def make_system_prompt(language_name: str) -> str:
    return (
        f"You are a translator for OpenStudio building energy modeling software. "
        f"The user will give you an English word or phrase inside <translate>...</translate> tags. "
        f"Return ONLY the {language_name} translation — nothing else. "
        f"No explanations, no notes, no questions. Just the translation.\n"
        f"Rules:\n"
        f"- ALWAYS translate. Never return the English source text unchanged.\n"
        f"- For technical building energy terms, use standard {language_name} engineering terminology.\n"
        f"- For named thermal comfort models (Fanger, Pierce, KSU, ASHRAE 55, CEN 15251), "
        f"keep the model name as a proper noun but translate the surrounding words.\n"
        f"- Do NOT translate acronyms that are units or standards (kWh, W/m², COP, PMV, PPD).\n"
        f"- Keep brand/product names unchanged (EnergyPlus, OpenStudio, HVAC, IDF, OSM).\n"
        f"- Short UI words like 'yes', 'no', 'cancel' should be translated naturally.\n"
        f"- Preserve newlines (\\n) and HTML entities (&amp; &lt; &gt; &quot; &apos;) exactly."
    )


def extract_unfinished(ts_content: str) -> list[dict]:
    pattern = re.compile(
        r'<message>\s*(?:(?:<!--.*?-->|<location[^>]*/?>)\s*)*<source>([^<]+)</source>\s*'
        r'<translation type="unfinished"></translation>\s*</message>',
        re.DOTALL,
    )
    return [
        {"id": f"entry-{i}", "source": m.group(1).strip()}
        for i, m in enumerate(pattern.finditer(ts_content))
    ]


def build_batch_requests(entries: list[dict], language_name: str) -> list[Request]:
    system_prompt = make_system_prompt(language_name)
    return [
        Request(
            custom_id=entry["id"],
            params=MessageCreateParamsNonStreaming(
                model=MODEL,
                max_tokens=512,
                system=system_prompt,
                messages=[{"role": "user", "content": f"<translate>{entry['source']}</translate>"}],
            ),
        )
        for entry in entries
    ]


def apply_translations(ts_content: str, id_to_translation: dict[str, str]) -> str:
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
            translation = re.sub(r'<[^>]+>', '', translation).strip()
            source = m.group(1)
            replacement = (
                f"<message>\n        <source>{source}</source>\n"
                f"        <translation>{translation}</translation>\n    </message>"
            )
            result = result[:m.start()] + replacement + result[m.end():]
    return result


def main():
    key_text = open(KEY_FILE, encoding="utf-8").read()
    key_match = re.search(r"sk-ant-[A-Za-z0-9_\-]+", key_text)
    if not key_match:
        sys.exit("ERROR: No API key found in key file.")
    client = anthropic.Anthropic(api_key=key_match.group(0))

    batch_jobs: dict[str, dict] = {}

    for lang_code, lang_name in LANGUAGES.items():
        ts_file = TS_TEMPLATE.format(lang=lang_code)
        content = open(ts_file, encoding="utf-8").read()
        entries = extract_unfinished(content)

        if not entries:
            print(f"[{lang_code}] nothing to translate")
            continue

        print(f"[{lang_code}] {len(entries)} entries")
        requests = build_batch_requests(entries, lang_name)
        batch = client.messages.batches.create(requests=requests)
        print(f"[{lang_code}] batch: {batch.id}")
        batch_jobs[lang_code] = {"batch_id": batch.id, "ts_file": ts_file}

    if not batch_jobs:
        print("Nothing to do.")
        return

    print(f"\nPolling {len(batch_jobs)} batches...")
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

    print("\nApplying...")
    for lang_code, job in batch_jobs.items():
        id_to_translation: dict[str, str] = {}
        for result in client.messages.batches.results(job["batch_id"]):
            if result.result.type == "succeeded":
                msg = result.result.message
                text = next((b.text for b in msg.content if b.type == "text"), "").strip()
                # Strip any XML tags the model might have included in its response
                text = re.sub(r'<[^>]+>', '', text).strip()
                id_to_translation[result.custom_id] = text
            else:
                print(f"  [{lang_code}] WARN: {result.custom_id} -> {result.result.type}")

        content = open(job["ts_file"], encoding="utf-8").read()
        new_content = apply_translations(content, id_to_translation)
        with open(job["ts_file"], "w", encoding="utf-8") as f:
            f.write(new_content)

        remaining = extract_unfinished(new_content)
        print(f"  [{lang_code}] {len(id_to_translation)} applied, {len(remaining)} remaining")

    print("\nDone.")


if __name__ == "__main__":
    main()

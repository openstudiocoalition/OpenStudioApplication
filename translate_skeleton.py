#!/usr/bin/env python3
"""
Translate unfinished entries in the IDD context of every OpenStudioApp_*.ts
file via the Claude API batch system.

Run this after add_idd_skeleton.py has inserted new skeleton stubs.
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
        f"You are a professional technical translator for building energy modeling software "
        f"(OpenStudio / EnergyPlus). "
        f"The user will give you an English IDD field name inside <translate>...</translate> tags. "
        f"Return ONLY the {language_name} translation — nothing else. "
        f"No explanations, no notes, no questions. Just the translation.\n"
        f"Rules:\n"
        f"- Use standard building engineering / HVAC terminology in {language_name}.\n"
        f"- Do NOT translate acronyms or units (HVAC, EnergyPlus, kWh, W/m², COP, ASHRAE, IDF, OSM).\n"
        f"- Keep abbreviations in parentheses where present (e.g. 'Outdoor Air (OA)').\n"
        f"- Preserve any trailing colon or punctuation from the source string."
    )


def extract_idd_unfinished(ts_content: str) -> list[dict]:
    """Return unfinished entries from the IDD context block only."""
    m = re.search(r'<name>IDD</name>(.*?)(?=<context>|\Z)', ts_content, re.DOTALL)
    if not m:
        return []
    idd_block = m.group(1)
    pattern = re.compile(
        r'<message>\s*<source>([^<]+)</source>\s*'
        r'<translation type="unfinished"></translation>\s*</message>',
        re.DOTALL,
    )
    return [
        {"id": f"entry-{i}", "source": mo.group(1).strip()}
        for i, mo in enumerate(pattern.finditer(idd_block))
    ]


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


def apply_idd_translations(ts_content: str, id_to_translation: dict[str, str]) -> str:
    """Apply translations to unfinished entries in the IDD context block only."""
    m = re.search(r'(<name>IDD</name>)(.*?)(?=(<context>)|\Z)', ts_content, re.DOTALL)
    if not m:
        return ts_content

    idd_block = m.group(2)
    pattern = re.compile(
        r'<message>\s*<source>([^<]+)</source>\s*'
        r'<translation type="unfinished"></translation>\s*</message>',
        re.DOTALL,
    )
    matches = list(pattern.finditer(idd_block))
    for i, mo in reversed(list(enumerate(matches))):
        translation = id_to_translation.get(f"entry-{i}", "")
        if translation:
            translation = re.sub(r'<[^>]+>', '', translation).strip()
            source = mo.group(1)
            replacement = (
                f"<message>\n        <source>{source}</source>\n"
                f"        <translation>{translation}</translation>\n    </message>"
            )
            idd_block = idd_block[:mo.start()] + replacement + idd_block[mo.end():]

    return ts_content[:m.start(2)] + idd_block + ts_content[m.end(2):]


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
        entries = extract_idd_unfinished(content)

        if not entries:
            print(f"[{lang_code}] nothing to translate")
            continue

        print(f"[{lang_code}] {len(entries)} IDD entries to translate")
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
        new_content = apply_idd_translations(content, id_to_translation)
        with open(job["ts_file"], "w", encoding="utf-8") as f:
            f.write(new_content)

        remaining = extract_idd_unfinished(new_content)
        print(f"  [{lang_code}] {len(id_to_translation)} applied, {len(remaining)} remaining")

    print("\nDone. Run fix_and_unvanish.py to clean up.")


if __name__ == "__main__":
    main()

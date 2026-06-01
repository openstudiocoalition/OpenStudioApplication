#!/usr/bin/env python3
"""Recover from a polling failure by re-polling known batch IDs and applying results."""

import re
import time
import sys
import anthropic

KEY_FILE = r"C:\Users\ml\OneDrive\ClaudeAPIkey.txt"
TS_TEMPLATE = "translations/OpenStudioApp_{lang}.ts"

BATCH_JOBS = {
    "ar": "msgbatch_01Gods89Yzcpt7W15YWLksjD",
    "ca": "msgbatch_01LpcrUG7iCoFiGpGAiSct5U",
    "de": "msgbatch_015nGnnkd2K1VkTGNJwLDwxi",
    "el": "msgbatch_017QtAxWbGSSCgbVsJzb95ts",
    "fa": "msgbatch_012zpSKZqnu14eiXnCMyucuR",
    "fr": "msgbatch_01BxyQPVCzVkRfipx334gwff",
    "he": "msgbatch_01RsjwrhA3XKAgUEh51rn7DQ",
    "hi": "msgbatch_01WJYfi16v6cGYzHWcdKpgvh",
    "it": "msgbatch_019gFwYAf2KrntU2xm73pPKG",
    "ja": "msgbatch_01NxGqXvjAoGyAKyJorReP3K",
    "pl": "msgbatch_01FHgrVJigUSjrsGose67ymm",
    "vi": "msgbatch_01C7y8zZzPV5JPhgw5wUu1NU",
    "zh_CN": "msgbatch_01BxwKcJf6NhDmgwXSJkZ6X7",
}


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

    print(f"Polling {len(BATCH_JOBS)} batches...")
    pending = set(BATCH_JOBS.keys())
    while pending:
        time.sleep(15)
        still_pending = set()
        for lang_code in sorted(pending):
            batch = client.messages.batches.retrieve(BATCH_JOBS[lang_code])
            if batch.processing_status == "ended":
                c = batch.request_counts
                print(f"  [{lang_code}] done — succeeded={c.succeeded}, errored={c.errored}")
            else:
                still_pending.add(lang_code)
        pending = still_pending
        if pending:
            print(f"  Still running: {', '.join(sorted(pending))}")

    print("\nApplying translations...")
    for lang_code, batch_id in BATCH_JOBS.items():
        id_to_translation: dict[str, str] = {}
        for result in client.messages.batches.results(batch_id):
            if result.result.type == "succeeded":
                msg = result.result.message
                text = next((b.text for b in msg.content if b.type == "text"), "").strip()
                id_to_translation[result.custom_id] = text
            else:
                print(f"  [{lang_code}] WARN: {result.custom_id} -> {result.result.type}")

        ts_file = TS_TEMPLATE.format(lang=lang_code)
        content = open(ts_file, encoding="utf-8").read()
        new_content = apply_translations(content, id_to_translation)
        with open(ts_file, "w", encoding="utf-8") as f:
            f.write(new_content)

        remaining = extract_unfinished(new_content)
        print(f"  [{lang_code}] {len(id_to_translation)} applied, {len(remaining)} empty remaining")

    print("\nDone.")


if __name__ == "__main__":
    main()

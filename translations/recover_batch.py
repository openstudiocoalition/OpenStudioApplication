#!/usr/bin/env python3
"""
Recover and apply results from a batch translation job that
translate_all_languages.py submitted but never got around to applying
(e.g. its 2-hour polling loop timed out, or result download failed with
a transient SSL/connection error).

translate_all_languages.py prints the batch ID for every language it
submits, and reprints it for any batch still pending when the polling
loop times out. Take that batch ID and run:

    python recover_batch.py --lang ca --batch-id msgbatch_01FTviWQERZjajdeYLUi1b8m

Multiple languages can be recovered in one run by repeating both flags:

    python recover_batch.py --lang fa --batch-id msgbatch_xxx --lang vi --batch-id msgbatch_yyy

IMPORTANT: this only works if OpenStudioApp_<lang>.ts has not been
rewritten since the batch was submitted (i.e. don't re-run
translate_all_languages.py for that language first) -- the script
reconstructs the batch's custom_id -> source mapping from the current
file's unfinished entries, in the same order Phase 1 would have produced
them.

Always follow with:
    python fix_and_unvanish.py
"""

import argparse
import json
import re
import sys
import time

import requests

from translate_all_languages import KEY_FILE, TS_TEMPLATE, extract_unfinished, apply_translations

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.stderr.reconfigure(encoding="utf-8", errors="replace")

ANTHROPIC_VERSION = "2023-06-01"
HTTP_TIMEOUT = (10, 120)  # (connect, read) seconds


def download_results(api_key: str, batch_id: str, retries: int = 5) -> list[dict]:
    headers = {"x-api-key": api_key, "anthropic-version": ANTHROPIC_VERSION}
    status_url = f"https://api.anthropic.com/v1/messages/batches/{batch_id}"

    last_err = None
    for attempt in range(1, retries + 1):
        try:
            status = requests.get(status_url, headers=headers, timeout=HTTP_TIMEOUT).json()
            if status.get("processing_status") != "ended":
                raise RuntimeError(f"batch {batch_id} not ended: {status.get('processing_status')}")
            results_url = status["results_url"]

            results = []
            with requests.get(results_url, headers=headers, stream=True, timeout=HTTP_TIMEOUT) as resp:
                resp.raise_for_status()
                for line in resp.iter_lines(decode_unicode=True):
                    if line:
                        results.append(json.loads(line))
            return results
        except (requests.exceptions.RequestException, json.JSONDecodeError) as e:
            last_err = e
            wait = 5 * (2 ** (attempt - 1))
            print(f"    [retry {attempt}/{retries}] download error: {e!r} -- sleeping {wait}s", flush=True)
            time.sleep(wait)
    raise RuntimeError(f"download failed after {retries} retries: {last_err!r}")


def recover(api_key: str, lang_code: str, batch_id: str) -> None:
    ts_file = TS_TEMPLATE.format(lang=lang_code)
    content = open(ts_file, encoding="utf-8").read()

    first_only = [e for e in extract_unfinished(content) if e["is_first"]]
    idx_to_source = {f"t-{i}": e["source"] for i, e in enumerate(first_only)}
    print(f"[{lang_code}] {len(first_only)} unique unfinished stubs in current file")

    results = download_results(api_key, batch_id)

    source_to_translation: dict[str, str] = {}
    errored = 0
    for result in results:
        cid = result.get("custom_id", "")
        res = result.get("result", {})
        if res.get("type") == "succeeded":
            text = next(
                (b.get("text", "") for b in res["message"]["content"] if b.get("type") == "text"), ""
            ).strip()
            src = idx_to_source.get(cid, "")
            if text and src:
                source_to_translation[src] = text
        else:
            errored += 1

    sample = list(source_to_translation.items())[:3]
    for src, t in sample:
        print(f"    sample: {src!r} -> {t!r}")

    new_content = apply_translations(content, source_to_translation)
    with open(ts_file, "w", encoding="utf-8") as f:
        f.write(new_content)

    remaining = new_content.count('<translation type="unfinished"></translation>')
    print(
        f"[{lang_code}] {len(source_to_translation)}/{len(first_only)} translated "
        f"({remaining} stubs remaining, {errored} errored)"
    )


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--lang", action="append", required=True, metavar="CODE",
                         help="Language code (e.g. ca). Repeat --lang/--batch-id pairs for multiple languages.")
    parser.add_argument("--batch-id", action="append", required=True, metavar="ID",
                         help="Batch ID printed by translate_all_languages.py for the matching --lang.")
    args = parser.parse_args()

    if len(args.lang) != len(args.batch_id):
        sys.exit("ERROR: --lang and --batch-id must be given the same number of times, paired in order.")

    key_text = open(KEY_FILE, encoding="utf-8").read()
    key_match = re.search(r"sk-ant-[A-Za-z0-9_\-]+", key_text)
    if not key_match:
        sys.exit("ERROR: No API key found in key file.")
    api_key = key_match.group(0)

    for lang_code, batch_id in zip(args.lang, args.batch_id):
        recover(api_key, lang_code, batch_id)

    print("\nDone. Run:")
    print("  python fix_and_unvanish.py")


if __name__ == "__main__":
    main()

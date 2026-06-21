#!/usr/bin/env python3
"""
Three-part comparison CSV updater.

Part 1 — gui_comparison.csv
  Reads all non-IDD/non-OutputVariables contexts from ES and FR .ts files,
  fills New ES / New FR, and sets ES Changed / FR Changed flags.

Part 2 — output_vars_comparison.csv
  Reads the OutputVariables context from ES and FR .ts files and updates
  New ES / New FR / Changed flags.

Part 3 — idd_fields_comparison.csv
  Reads the IDD context from ES and FR .ts files and updates
  New ES / New FR / Changed flags.

Run after retranslate_*.py + fix_and_unvanish.py to capture the latest
translations in all three comparison spreadsheets.

Usage:
    python update_comparison_csvs.py           # all three parts
    python update_comparison_csvs.py --part 1  # gui only
    python update_comparison_csvs.py --part 2  # output vars only
    python update_comparison_csvs.py --part 3  # idd fields only
"""

import argparse
import csv
from html import unescape
import xml.etree.ElementTree as ET


# ---------------------------------------------------------------------------
# .ts parsing helpers
# ---------------------------------------------------------------------------

def parse_ts_all_contexts(ts_path: str) -> dict[str, dict[str, str]]:
    """Return {context_name: {source_string: translation_string}} for all contexts."""
    tree = ET.parse(ts_path)
    root = tree.getroot()
    result: dict[str, dict[str, str]] = {}
    for ctx in root.findall("context"):
        name_el = ctx.find("name")
        if name_el is None:
            continue
        ctx_name = name_el.text or ""
        trans_map: dict[str, str] = {}
        for msg in ctx.findall("message"):
            src = msg.find("source")
            tr = msg.find("translation")
            if src is not None and src.text and tr is not None and tr.text:
                trans_map[src.text.strip()] = unescape(tr.text.strip())
        if trans_map:
            result[ctx_name] = trans_map
    return result


def parse_ts_context(ts_path: str, context_name: str) -> dict[str, str]:
    """Return {source_string: translation_string} for one named context."""
    return parse_ts_all_contexts(ts_path).get(context_name, {})


# ---------------------------------------------------------------------------
# Part 1 — gui_comparison.csv  (multi-context, keyed by Context + English)
# ---------------------------------------------------------------------------

def update_gui(csv_path: str, es_ts: str, fr_ts: str) -> None:
    print(f"\n=== Part 1: {csv_path} ===")

    print(f"  Parsing {es_ts} ...")
    es_data = parse_ts_all_contexts(es_ts)
    print(f"  Parsing {fr_ts} ...")
    fr_data = parse_ts_all_contexts(fr_ts)

    rows: list[dict] = []
    fieldnames: list[str] = []

    with open(csv_path, newline="", encoding="utf-8-sig") as f:
        reader = csv.DictReader(f)
        fieldnames = list(reader.fieldnames or [])
        for row in reader:
            rows.append(dict(row))

    filled_es = filled_fr = changed_es = changed_fr = 0

    for row in rows:
        ctx    = row.get("Context", "")
        eng    = row.get("English", "")
        old_es = row.get("Old ES", "")
        old_fr = row.get("Old FR", "")

        new_es = es_data.get(ctx, {}).get(eng, "")
        new_fr = fr_data.get(ctx, {}).get(eng, "")

        row["New ES"] = new_es
        row["New FR"] = new_fr

        es_flag = "Y" if new_es and new_es != old_es else ""
        fr_flag = "Y" if new_fr and new_fr != old_fr else ""
        row["ES Changed"] = es_flag
        row["FR Changed"] = fr_flag

        if new_es: filled_es += 1
        if new_fr: filled_fr += 1
        if es_flag: changed_es += 1
        if fr_flag: changed_fr += 1

    with open(csv_path, "w", newline="", encoding="utf-8-sig") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)

    total = len(rows)
    print(f"  {total} rows total")
    print(f"  New ES filled: {filled_es}/{total}  ({changed_es} changed from Old ES)")
    print(f"  New FR filled: {filled_fr}/{total}  ({changed_fr} changed from Old FR)")
    print(f"  Wrote {csv_path}")


# ---------------------------------------------------------------------------
# Parts 2 & 3 — single-context CSVs keyed by English string only
# ---------------------------------------------------------------------------

def update_single_context(
    part: str,
    csv_path: str,
    context_name: str,
    es_ts: str,
    fr_ts: str,
) -> None:
    print(f"\n=== Part {part}: {csv_path} ({context_name} context) ===")

    print(f"  Parsing {es_ts} ...")
    es_map = parse_ts_context(es_ts, context_name)
    print(f"  {len(es_map)} {context_name} strings found in ES")

    print(f"  Parsing {fr_ts} ...")
    fr_map = parse_ts_context(fr_ts, context_name)
    print(f"  {len(fr_map)} {context_name} strings found in FR")

    rows: list[dict] = []
    fieldnames: list[str] = []

    with open(csv_path, newline="", encoding="utf-8-sig") as f:
        reader = csv.DictReader(f)
        fieldnames = list(reader.fieldnames or [])
        for row in reader:
            rows.append(dict(row))

    stats = dict(es_changed=0, es_unchanged=0, es_missing=0,
                 fr_changed=0, fr_unchanged=0, fr_missing=0)

    for row in rows:
        eng    = row.get("English", "")
        old_es = unescape(row.get("Old ES", ""))
        old_fr = unescape(row.get("Old FR", ""))

        new_es = es_map.get(eng, "")
        new_fr = fr_map.get(eng, "")

        if not new_es:              stats["es_missing"]   += 1
        elif new_es != old_es:      stats["es_changed"]   += 1
        else:                       stats["es_unchanged"] += 1

        if not new_fr:              stats["fr_missing"]   += 1
        elif new_fr != old_fr:      stats["fr_changed"]   += 1
        else:                       stats["fr_unchanged"] += 1

        row["New ES"] = new_es
        row["New FR"] = new_fr
        row["ES Changed"] = "Y" if new_es and new_es != old_es else ""
        row["FR Changed"] = "Y" if new_fr and new_fr != old_fr else ""

    with open(csv_path, "w", newline="", encoding="utf-8-sig") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)

    total = len(rows)
    print(f"  {total} rows total")
    print(f"  ES — changed: {stats['es_changed']}  unchanged: {stats['es_unchanged']}  missing: {stats['es_missing']}")
    print(f"  FR — changed: {stats['fr_changed']}  unchanged: {stats['fr_unchanged']}  missing: {stats['fr_missing']}")
    print(f"  Wrote {csv_path}")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Update all three comparison CSVs from current .ts files."
    )
    parser.add_argument(
        "--part", choices=["1", "2", "3"],
        help="Run only one part (default: all three).",
    )
    args = parser.parse_args()

    es_ts = "OpenStudioApp_es.ts"
    fr_ts = "OpenStudioApp_fr.ts"

    run_all = args.part is None

    if run_all or args.part == "1":
        update_gui(
            csv_path="gui_comparison.csv",
            es_ts=es_ts,
            fr_ts=fr_ts,
        )

    if run_all or args.part == "2":
        update_single_context(
            part="2",
            csv_path="output_vars_comparison.csv",
            context_name="OutputVariables",
            es_ts=es_ts,
            fr_ts=fr_ts,
        )

    if run_all or args.part == "3":
        update_single_context(
            part="3",
            csv_path="idd_fields_comparison.csv",
            context_name="IDD",
            es_ts=es_ts,
            fr_ts=fr_ts,
        )

    print("\nDone.")


if __name__ == "__main__":
    main()

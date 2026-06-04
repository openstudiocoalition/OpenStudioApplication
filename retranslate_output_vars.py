#!/usr/bin/env python3
"""
Re-translates the OutputVariables context in .ts files using EnergyPlus I/O Reference
definitions as context for each variable.

Requires output_var_definitions.json (produced by scrape_output_var_definitions.py).

Usage:
    python retranslate_output_vars.py                      # es + fr
    python retranslate_output_vars.py --lang es            # Spanish only
    python retranslate_output_vars.py --lang es fr --model claude-sonnet-4-6
    python retranslate_output_vars.py --all                # all languages
"""

import argparse
import json
import re
import time
import sys
from collections import defaultdict
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
    "it": "Italian",
    "ja": "Japanese",
    "pl": "Polish",
    "vi": "Vietnamese",
    "zh_CN": "Simplified Chinese",
}

DEFAULT_LANGS = ["es", "fr"]
DEFAULT_MODEL = "claude-haiku-4-5-20251001"
TS_TEMPLATE = "translations/OpenStudioApp_{lang}.ts"
DEFINITIONS_FILE = "output_var_definitions.json"
KEY_FILE = r"C:\Users\ml\OneDrive\ClaudeAPIkey.txt"


# ---------------------------------------------------------------------------
# Prompt
# ---------------------------------------------------------------------------

def make_system_prompt(language_name: str) -> str:
    return (
        f"You are a professional technical translator for building energy simulation software "
        f"(EnergyPlus / OpenStudio). "
        f"You are translating EnergyPlus output variable names into {language_name}.\n\n"
        f"Output variable names describe physical quantities reported during a building energy "
        f"simulation: temperatures, flow rates, power, energy, humidity ratios, control signals, "
        f"pressures, efficiencies, and similar.\n\n"
        f"Each variable name is presented with a colon separating two parts:\n"
        f"  [Category]: [Measurement]\n"
        f"The Category identifies the equipment or system (e.g. 'Zone Ideal Loads', 'Air System', "
        f"'Boiler'). The Measurement is the specific physical quantity being reported.\n"
        f"Translate BOTH parts, keeping the colon separator in the output. "
        f"The same Category must translate identically every time it appears.\n\n"
        f"When a Definition and Units are provided before <translate>, use them to understand "
        f"the precise physical meaning and choose accurate {language_name} HVAC / building-physics "
        f"terminology.\n\n"
        f"Return ONLY the {language_name} translation — nothing else. "
        f"No explanations, no notes, no questions.\n\n"
        f"Rules:\n"
        f"- Use standard HVAC and building-physics terminology in {language_name}.\n"
        f"- Keep SI unit abbreviations (kg/s, W, kWh, °C, m³/s, Pa, J, m²) unchanged.\n"
        f"- Keep acronyms as-is: HVAC, COP, EIR, PLR, EMS, VRF, DOAS, AHU, UPS.\n"
        f"- 'Rate' = rate of a physical process (not a tariff or price).\n"
        f"- 'Energy' = physical energy quantity (not economic value).\n"
        f"- 'Fraction' = dimensionless ratio between 0 and 1.\n"
        f"- Keep proper-noun equipment names standard in the {language_name}-speaking HVAC industry.\n"
        f"- The translation must be concise but technically precise."
    )


# ---------------------------------------------------------------------------
# Category detection
# ---------------------------------------------------------------------------

# Canonical top-level category prefixes. Sorted longest-first so the most
# specific match wins when multiple entries share a common prefix.
CANONICAL_CATEGORIES = sorted([
    "Air System",
    "Air Terminal",
    "Chilled Water Thermal Storage Tank",
    "Chiller",
    "Cooling Coil",
    "Cooling Panel",
    "Cooling Tower",
    "Daylighting",
    "Electric Load Center",
    "Exterior Windows",
    "Fluid Heat Exchanger",
    "Generator",
    "Heat Exchanger",
    "Heating Coil",
    "ITE",
    "Ice Thermal Storage",
    "Ideal Loads",
    "Infiltration",
    "Interior Windows",
    "Lights",
    "Mechanical Ventilation",
    "Plant",
    "Predicted",
    "Refrigeration Air Chiller System",
    "Refrigeration System",
    "Refrigeration Walk In",
    "Solar Collector",
    "System Node",
    "Thermal Comfort",
    "Unit Ventilator",
    "Unitary System",
    "VRF Air Terminal",
    "VRF Heat Pump",
    "Ventilation",
    "Ventilator",
    "Water Heater",
    "Water Use Connections",
    "Windows",
    "Zone",
], key=len, reverse=True)

CANONICAL_NO_ZONE = [c for c in CANONICAL_CATEGORIES if c != "Zone"]

# Zone sub-categories that should NOT be split out — keep them in the measurement.
ZONE_SUBCAT_EXCLUSIONS = {"Total Internal"}

MIN_CATEGORY_COUNT = 3  # prefix must appear in >=N variables to qualify


def _build_raw_category_map(sources: list[str]) -> dict[str, str]:
    """Return longest shared word-prefix (>=MIN_CATEGORY_COUNT occurrences) per source."""
    prefix_count: dict[str, int] = defaultdict(int)
    for name in sources:
        words = name.split()
        for n in range(1, len(words)):
            prefix_count[" ".join(words[:n])] += 1

    categories: dict[str, str] = {}
    for name in sources:
        words = name.split()
        best = words[0]
        for n in range(2, len(words)):
            prefix = " ".join(words[:n])
            if prefix_count[prefix] >= MIN_CATEGORY_COUNT:
                best = prefix
        categories[name] = best
    return categories


def _consolidate(cat: str, canon: list[str]) -> str:
    for c in canon:
        if cat == c or cat.startswith(c + " "):
            return c
    return cat


def build_category_map(sources: list[str]) -> tuple[dict[str, str], dict[str, str]]:
    """
    Returns (top_level_map, zone_subcat_map).
    top_level_map : source -> canonical top-level category
    zone_subcat_map: source -> canonical Zone sub-category (only for Zone variables)
    """
    raw = _build_raw_category_map(sources)
    top = {src: _consolidate(cat, CANONICAL_CATEGORIES) for src, cat in raw.items()}

    # Zone sub-categories: strip "Zone " prefix, re-run detection within that set
    zone_stripped = {
        src: src[5:]
        for src in sources
        if top.get(src) == "Zone" and src.startswith("Zone ")
    }
    sub_raw = _build_raw_category_map(list(zone_stripped.values()))
    zone_sub = {}
    for src, stripped in zone_stripped.items():
        subcat = _consolidate(sub_raw.get(stripped, stripped.split()[0]), CANONICAL_NO_ZONE)
        if subcat not in ZONE_SUBCAT_EXCLUSIONS:
            zone_sub[src] = subcat

    return top, zone_sub


def format_with_category(
    var_name: str,
    top: dict[str, str],
    zone_sub: dict[str, str],
) -> str:
    """
    Return the category-split string the API will translate.
    Zone variables use three levels: 'Zone: SubCat: Measurement'
    All others use two levels:       'Category: Measurement'
    """
    cat = top.get(var_name, var_name.split()[0])

    if cat == "Zone" and var_name in zone_sub:
        subcat = zone_sub[var_name]
        remainder = var_name[5:][len(subcat):].strip()  # strip "Zone " then subcat
        if remainder:
            return f"Zone: {subcat}: {remainder}"
        return f"Zone: {subcat}"

    measurement = var_name[len(cat):].strip()
    return f"{cat}: {measurement}" if measurement else var_name


def build_template_index(definitions: dict) -> list[tuple]:
    """
    Build a list of (compiled_regex, entry) for definitions whose keys contain
    <placeholder> tokens.  Used as a fallback when exact lookup fails.
    """
    index = []
    for key, entry in definitions.items():
        if "<" not in key:
            continue
        # Escape everything, then replace escaped <...> tokens with a greedy match
        pattern = re.escape(key)
        pattern = re.sub(r"\\<[^>]+\\>", ".+", pattern)
        index.append((re.compile(f"^{pattern}$", re.IGNORECASE), entry))
    return index


def lookup_definition(
    var_name: str,
    definitions: dict,
    template_index: list[tuple],
) -> dict | None:
    """Exact match first; fall back to template pattern match."""
    entry = definitions.get(var_name)
    if entry:
        return entry
    for pattern, entry in template_index:
        if pattern.match(var_name):
            return entry
    return None


def build_user_message(
    var_name: str,
    formatted: str,
    defn: dict | None,
) -> str:
    if defn and defn.get("definition"):
        units_str = f" [{defn['units']}]" if defn.get("units") else ""
        return (
            f"Definition: {defn['definition']}\n"
            f"Units:{units_str if units_str else ' dimensionless'}\n\n"
            f"<translate>{formatted}</translate>"
        )
    return f"<translate>{formatted}</translate>"


# ---------------------------------------------------------------------------
# .ts file helpers
# ---------------------------------------------------------------------------

def extract_output_var_sources(ts_content: str) -> list[str]:
    """Return all source strings in the OutputVariables context, in order."""
    ctx_m = re.search(
        r'<context>\s*<name>OutputVariables</name>(.*?)</context>',
        ts_content,
        re.DOTALL,
    )
    if not ctx_m:
        return []
    return [s.strip() for s in re.findall(r'<source>([^<]+)</source>', ctx_m.group(1))]


def apply_output_var_translations(ts_content: str, source_to_translation: dict[str, str]) -> str:
    """Replace <translation> values inside the OutputVariables context only."""

    def process_context(m):
        prefix = m.group(1)
        block = m.group(2)
        suffix = m.group(3)

        def replace_msg(msg_m):
            source = msg_m.group(1).strip()
            new_t = source_to_translation.get(source)
            if new_t:
                return (
                    f"<source>{msg_m.group(1)}</source>\n"
                    f"        <translation>{new_t}</translation>"
                )
            return msg_m.group(0)

        block = re.sub(
            r'<source>([^<]+)</source>\s*<translation[^>]*>.*?</translation>',
            replace_msg,
            block,
            flags=re.DOTALL,
        )
        return prefix + block + suffix

    return re.sub(
        r'(<context>\s*<name>OutputVariables</name>)(.*?)(</context>)',
        process_context,
        ts_content,
        flags=re.DOTALL,
    )


# ---------------------------------------------------------------------------
# Batch helpers
# ---------------------------------------------------------------------------

def build_batch_requests(
    sources: list[str],
    definitions: dict,
    template_index: list[tuple],
    top: dict[str, str],
    zone_sub: dict[str, str],
    language_name: str,
    model: str,
) -> list[Request]:
    system = make_system_prompt(language_name)
    requests_list = []
    for i, var_name in enumerate(sources):
        defn = lookup_definition(var_name, definitions, template_index)
        formatted = format_with_category(var_name, top, zone_sub)
        requests_list.append(
            Request(
                custom_id=f"ov-{i}",
                params=MessageCreateParamsNonStreaming(
                    model=model,
                    max_tokens=256,
                    system=system,
                    messages=[{"role": "user", "content": build_user_message(var_name, formatted, defn)}],
                ),
            )
        )
    return requests_list


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Re-translate OutputVariables context using I/O Reference definitions."
    )
    parser.add_argument(
        "--lang", nargs="+", metavar="CODE",
        help=f"Language codes to process (default: {DEFAULT_LANGS}). E.g. --lang es fr",
    )
    parser.add_argument(
        "--all", action="store_true",
        help="Process all supported languages.",
    )
    parser.add_argument(
        "--model", default=DEFAULT_MODEL,
        help=f"Claude model to use (default: {DEFAULT_MODEL}).",
    )
    args = parser.parse_args()

    if args.all:
        lang_map = LANGUAGES
    elif args.lang:
        lang_map = {k: v for k, v in LANGUAGES.items() if k in args.lang}
        if not lang_map:
            sys.exit(f"ERROR: No matching languages. Valid codes: {sorted(LANGUAGES)}")
    else:
        lang_map = {k: LANGUAGES[k] for k in DEFAULT_LANGS}

    # Load definitions
    try:
        with open(DEFINITIONS_FILE, encoding="utf-8") as f:
            definitions = json.load(f)
    except FileNotFoundError:
        sys.exit(f"ERROR: {DEFINITIONS_FILE} not found. Run scrape_output_var_definitions.py first.")

    print(f"Loaded {len(definitions)} output variable definitions from {DEFINITIONS_FILE}")
    template_index = build_template_index(definitions)
    print(f"Built template index: {len(template_index)} patterns")

    # Load API key
    key_text = open(KEY_FILE, encoding="utf-8").read()
    key_match = re.search(r"sk-ant-[A-Za-z0-9_\-]+", key_text)
    if not key_match:
        sys.exit("ERROR: No API key found in key file.")
    client = anthropic.Anthropic(api_key=key_match.group(0))

    # Phase 1: Submit batches
    batch_jobs: dict[str, dict] = {}

    for lang_code, lang_name in lang_map.items():
        ts_file = TS_TEMPLATE.format(lang=lang_code)
        print(f"\n[{lang_code}] {lang_name}")

        try:
            content = open(ts_file, encoding="utf-8").read()
        except FileNotFoundError:
            print(f"  SKIP: {ts_file} not found")
            continue

        sources = extract_output_var_sources(content)
        if not sources:
            print(f"  SKIP: no OutputVariables context found")
            continue

        top, zone_sub = build_category_map(sources)
        unique_cats = len(set(top.values()))
        zone_count = sum(1 for v in top.values() if v == "Zone")
        print(f"  {unique_cats} top-level categories, {zone_count} Zone variables with {len(set(zone_sub.values()))} sub-categories")

        with_def = sum(
            1 for s in sources
            if (lookup_definition(s, definitions, template_index) or {}).get("definition")
        )
        without_def = len(sources) - with_def
        print(f"  {len(sources)} output variables: {with_def} with definition ({100*with_def//len(sources)}%), {without_def} without")

        requests_list = build_batch_requests(sources, definitions, template_index, top, zone_sub, lang_name, args.model)
        batch = client.messages.batches.create(requests=requests_list)
        print(f"  Batch submitted: {batch.id} ({len(requests_list)} requests)")

        batch_jobs[lang_code] = {
            "batch_id": batch.id,
            "ts_file": ts_file,
            "lang_name": lang_name,
            "sources": sources,
        }

    if not batch_jobs:
        print("\nNothing to process.")
        return

    print(f"\n{len(batch_jobs)} batch(es) submitted. Polling every 15s...")

    # Phase 2: Poll
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

    # Phase 3: Apply
    print("\nApplying translations...")
    for lang_code, job in batch_jobs.items():
        id_to_translation: dict[str, str] = {}
        for result in client.messages.batches.results(job["batch_id"]):
            if result.result.type == "succeeded":
                msg = result.result.message
                text = next((b.text for b in msg.content if b.type == "text"), "").strip()
                # Strip any XML tags the model may have echoed back
                text = re.sub(r"<[^>]+>", "", text).strip()
                if text:
                    id_to_translation[result.custom_id] = text
            else:
                print(f"  [{lang_code}] WARN: {result.custom_id} → {result.result.type}")

        # Map custom_id back to source string
        source_to_translation: dict[str, str] = {}
        for i, src in enumerate(job["sources"]):
            t = id_to_translation.get(f"ov-{i}")
            if t:
                source_to_translation[src] = t

        content = open(job["ts_file"], encoding="utf-8").read()
        new_content = apply_output_var_translations(content, source_to_translation)

        with open(job["ts_file"], "w", encoding="utf-8") as f:
            f.write(new_content)

        applied = len(source_to_translation)
        total = len(job["sources"])
        print(f"  [{lang_code}] {applied}/{total} translations applied -> {job['ts_file']}")

    print("\nDone. Run fix_and_unvanish.py then lrelease to compile .qm files.")


if __name__ == "__main__":
    main()

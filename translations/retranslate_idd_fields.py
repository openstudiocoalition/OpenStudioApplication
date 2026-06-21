#!/usr/bin/env python3
"""
Re-translates the IDD context in .ts files using EnergyPlus I/O Reference
field definitions as context for each field label.

Requires idd_field_definitions.json (produced by scrape_idd_field_definitions.py).

Usage:
    python retranslate_idd_fields.py                      # es + fr
    python retranslate_idd_fields.py --lang es            # Spanish only
    python retranslate_idd_fields.py --lang es fr --model claude-sonnet-4-6
    python retranslate_idd_fields.py --all                # all languages
"""

import argparse
import json
import re
import time
import sys
from html import unescape
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
MAX_POLL_SECONDS = 1800  # 30 min — bail out if a batch stalls on Anthropic's side
TS_TEMPLATE = "OpenStudioApp_{lang}.ts"
DEFINITIONS_FILE = "idd_field_definitions.json"
KEY_FILE = r"C:\Users\ml\OneDrive\ClaudeAPIkey.txt"


# ---------------------------------------------------------------------------
# Prompt
# ---------------------------------------------------------------------------

def make_system_prompt(language_name: str) -> str:
    return (
        f"You are a professional technical translator for building energy simulation software "
        f"(EnergyPlus / OpenStudio). "
        f"You are translating EnergyPlus IDD input field labels into {language_name}.\n\n"
        f"These labels appear as field names in a building energy modeling GUI — they are short, "
        f"precise technical labels that identify input parameters for HVAC components, building "
        f"envelope elements, schedules, controls, and other simulation objects.\n\n"
        f"When an Object type and Definition are provided before <translate>, use them to "
        f"understand the precise engineering meaning and choose accurate {language_name} "
        f"HVAC / building-physics terminology.\n\n"
        f"Return ONLY the {language_name} translation of the field label — nothing else. "
        f"No explanations, no notes, no questions, no parenthetical clarifications.\n\n"
        f"Rules:\n"
        f"- Use standard HVAC and building-physics terminology in {language_name}.\n"
        f"- Keep the translation concise — these are UI labels, not descriptions.\n"
        f"- Keep SI unit abbreviations unchanged if they appear in the label.\n"
        f"- Keep the following acronyms unchanged in translations "
        f"(their expansions are provided here for context only):\n"
        f"  HVAC (Heating, Ventilation, and Air Conditioning), "
        f"COP (Coefficient of Performance), "
        f"EIR (Energy Input Ratio), "
        f"SHR (Sensible Heat Ratio), "
        f"PLR (Part Load Ratio), "
        f"VRF (Variable Refrigerant Flow), "
        f"DOAS (Dedicated Outdoor Air System), "
        f"VAV (Variable Air Volume), "
        f"CAV (Constant Air Volume), "
        f"EMS (Energy Management System), "
        f"AHU (Air Handling Unit), "
        f"UPS (Uninterruptible Power Supply), "
        f"DX (Direct Expansion refrigeration cycle), "
        f"VFD (Variable Frequency Drive), "
        f"ITE (Information Technology Equipment), "
        f"AHRI (Air-Conditioning, Heating, and Refrigeration Institute), "
        f"ARI (Air-Conditioning and Refrigeration Institute), "
        f"DCV (Demand Controlled Ventilation), "
        f"ERV (Energy Recovery Ventilator), "
        f"PTAC (Packaged Terminal Air Conditioner), "
        f"PTHP (Packaged Terminal Heat Pump), "
        f"ASHRAE (American Society of Heating, Refrigerating and Air-Conditioning Engineers).\n"
        f"- 'Schedule' translates to the standard {language_name} term used in energy modeling software.\n"
        f"- 'Fraction' = dimensionless ratio between 0 and 1.\n"
        f"- 'Coefficient' = technical coefficient in an equation.\n"
        f"- Keep proper-noun equipment names standard in the {language_name}-speaking HVAC industry.\n\n"
        f"EnergyPlus-specific terms that may appear in the Definition context — "
        f"use for understanding only, do not translate these object names:\n"
        f"- AirLoopHVAC / AirLoop / air loop: the central forced-air HVAC system "
        f"(air-handling unit and supply/return duct network) serving one or more thermal zones.\n"
        f"- PlantLoop / plant loop / condenser loop: a closed hydronic loop connecting a heat "
        f"source or sink (boiler, chiller, condenser, etc.) to loads via circulating water or other fluid.\n"
        f"- ZoneHVAC / zone HVAC / zone equipment: terminal HVAC units that condition a single "
        f"thermal zone (fan coil units, baseboard heaters, packaged terminals), distinct from "
        f"central air systems.\n"
        f"- SetpointManager / setpoint manager: a controller that calculates and assigns a target "
        f"setpoint (temperature, humidity, flow rate, etc.) at nodes in the air or water network.\n"
        f"- AvailabilityManager / availability manager: a controller that determines when an HVAC "
        f"system or component is permitted to operate.\n"
        f"- Branch / BranchList: a series of components connected in sequence along a fluid or "
        f"air loop.\n"
        f"- NodeList / node list: a named group of fluid or air network nodes to which a common "
        f"property (e.g. a setpoint) is applied simultaneously.\n"
        f"- DesignSpecification / design specification (e.g. DesignSpecification:OutdoorAir): "
        f"an object that specifies minimum outdoor air ventilation requirements for a zone or system."
    )


# ---------------------------------------------------------------------------
# .ts file helpers
# ---------------------------------------------------------------------------

def extract_idd_sources(ts_content: str, mode: str = "unfinished") -> list[str]:
    """Return source strings in the IDD context.

    mode='unfinished': only entries with an empty or type="unfinished" translation.
    mode='all':        every entry regardless of existing translation.
    """
    ctx_m = re.search(
        r'<context>\s*<name>IDD</name>(.*?)</context>',
        ts_content,
        re.DOTALL,
    )
    if not ctx_m:
        return []
    if mode == "all":
        raw = re.findall(r'<source>(.*?)</source>', ctx_m.group(1))
        return [unescape(s.strip()) for s in raw]
    pattern = re.compile(
        r'<source>([^<]+)</source>\s*<translation(?:\s+type="unfinished")?>\s*</translation>',
        re.DOTALL,
    )
    return [unescape(m.group(1).strip()) for m in pattern.finditer(ctx_m.group(1))]


def apply_idd_translations(
    ts_content: str,
    source_to_translation: dict[str, str],
    mode: str = "unfinished",
) -> str:
    """Replace <translation> values inside the IDD context only."""
    trans_pattern = (
        r'<source>([^<]+)</source>\s*<translation[^>]*>.*?</translation>'
        if mode == "all" else
        r'<source>([^<]+)</source>\s*<translation(?:\s+type="unfinished")?>\s*</translation>'
    )

    def process_context(m):
        prefix = m.group(1)
        block = m.group(2)
        suffix = m.group(3)

        def replace_msg(msg_m):
            source = unescape(msg_m.group(1).strip())
            new_t = source_to_translation.get(source)
            if new_t:
                esc = new_t.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
                return (
                    f"<source>{msg_m.group(1)}</source>\n"
                    f"        <translation>{esc}</translation>"
                )
            return msg_m.group(0)

        block = re.sub(trans_pattern, replace_msg, block, flags=re.DOTALL)
        return prefix + block + suffix

    return re.sub(
        r'(<context>\s*<name>IDD</name>)(.*?)(</context>)',
        process_context,
        ts_content,
        flags=re.DOTALL,
    )


# ---------------------------------------------------------------------------
# Batch helpers
# ---------------------------------------------------------------------------

def build_user_message(field_name: str, defn: dict | None) -> str:
    """Build the user-turn message for a single field label."""
    if defn and defn.get("definition"):
        obj = defn.get("object", "")
        parts = []
        if obj:
            parts.append(f"Object: {obj}")
        parts.append(f"Definition: {defn['definition']}")
        context = "\n".join(parts)
        return f"{context}\n\n<translate>{field_name}</translate>"
    return f"<translate>{field_name}</translate>"


def build_batch_requests(
    unique_sources: list[str],
    definitions: dict,
    language_name: str,
    model: str,
) -> list[Request]:
    system = make_system_prompt(language_name)
    requests_list = []
    for i, field_name in enumerate(unique_sources):
        defn = definitions.get(field_name)
        requests_list.append(
            Request(
                custom_id=f"idd-{i}",
                params=MessageCreateParamsNonStreaming(
                    model=model,
                    max_tokens=128,
                    system=system,
                    messages=[{"role": "user", "content": build_user_message(field_name, defn)}],
                ),
            )
        )
    return requests_list


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Re-translate IDD context using I/O Reference field definitions."
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
    parser.add_argument(
        "--mode", choices=["unfinished", "all"], default="unfinished",
        help="'unfinished' (default): only translate empty/unfinished entries. "
             "'all': retranslate every entry, overwriting existing translations.",
    )
    args = parser.parse_args()

    if args.mode == "all":
        print(
            "WARNING: --mode all generates a fresh machine translation for every IDD entry,\n"
            "overwriting any human-provided translations in the .ts files.\n"
            "Intended use: populate idd_fields_comparison.csv New ES/FR columns so machine\n"
            "translations can be compared side-by-side with human translations.\n"
            "Press Ctrl+C within 5 s to abort."
        )
        import time as _time; _time.sleep(5)

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
        sys.exit(f"ERROR: {DEFINITIONS_FILE} not found. Run scrape_idd_field_definitions.py first.")

    print(f"Loaded {len(definitions)} IDD field definitions from {DEFINITIONS_FILE}")

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

        all_sources = extract_idd_sources(content, mode=args.mode)
        if not all_sources:
            print(f"  SKIP: no IDD context found")
            continue

        # Deduplicate: translate each unique field label once
        seen: set[str] = set()
        unique_sources: list[str] = []
        for s in all_sources:
            if s not in seen:
                seen.add(s)
                unique_sources.append(s)

        with_def = sum(1 for s in unique_sources if definitions.get(s, {}).get("definition"))
        without_def = len(unique_sources) - with_def
        print(f"  {len(all_sources)} total strings, {len(unique_sources)} unique")
        print(f"  {with_def} with definition ({100*with_def//len(unique_sources)}%), {without_def} without")

        requests_list = build_batch_requests(unique_sources, definitions, lang_name, args.model)
        batch = client.messages.batches.create(requests=requests_list)
        print(f"  Batch submitted: {batch.id} ({len(requests_list)} requests)")

        batch_jobs[lang_code] = {
            "batch_id": batch.id,
            "ts_file": ts_file,
            "lang_name": lang_name,
            "all_sources": all_sources,
            "unique_sources": unique_sources,
        }

    if not batch_jobs:
        print("\nNothing to process.")
        return

    print(f"\n{len(batch_jobs)} batch(es) submitted. Polling every 15 s (timeout {MAX_POLL_SECONDS//60} min) ...", flush=True)

    # Phase 2: Poll
    pending = set(batch_jobs.keys())
    deadline = time.time() + MAX_POLL_SECONDS
    while pending:
        time.sleep(15)
        still_pending = set()
        for lang_code in sorted(pending):
            batch = client.messages.batches.retrieve(batch_jobs[lang_code]["batch_id"])
            if batch.processing_status == "ended":
                c = batch.request_counts
                print(f"  [{lang_code}] done - succeeded={c.succeeded} errored={c.errored}", flush=True)
            else:
                still_pending.add(lang_code)
        pending = still_pending
        if pending:
            if time.time() > deadline:
                print(f"\nWARN: polling timeout ({MAX_POLL_SECONDS//60} min). Batches still running:", flush=True)
                for lc in sorted(pending):
                    print(f"  [{lc}] {batch_jobs[lc]['batch_id']} -- use recover_batches.py when complete", flush=True)
                for lc in pending:
                    batch_jobs.pop(lc)
                break
            print(f"  Still running: {', '.join(sorted(pending))}", flush=True)

    # Phase 3: Apply
    print("\nApplying translations...")
    for lang_code, job in batch_jobs.items():
        id_to_translation: dict[str, str] = {}
        for result in client.messages.batches.results(job["batch_id"]):
            if result.result.type == "succeeded":
                msg = result.result.message
                text = next((b.text for b in msg.content if b.type == "text"), "").strip()
                text = re.sub(r"<[^>]+>", "", text).strip()
                if text:
                    id_to_translation[result.custom_id] = text
            else:
                print(f"  [{lang_code}] WARN: {result.custom_id} - {result.result.type}")

        # Map custom_id back to source string (unique index)
        source_to_translation: dict[str, str] = {}
        for i, src in enumerate(job["unique_sources"]):
            t = id_to_translation.get(f"idd-{i}")
            if t:
                source_to_translation[src] = t

        content = open(job["ts_file"], encoding="utf-8").read()
        new_content = apply_idd_translations(content, source_to_translation, mode=args.mode)

        with open(job["ts_file"], "w", encoding="utf-8") as f:
            f.write(new_content)

        applied = len(source_to_translation)
        total = len(job["unique_sources"])
        print(f"  [{lang_code}] {applied}/{total} unique labels translated -> {job['ts_file']}")

    print("\nDone. Run fix_and_unvanish.py then lrelease to compile .qm files.")


if __name__ == "__main__":
    main()

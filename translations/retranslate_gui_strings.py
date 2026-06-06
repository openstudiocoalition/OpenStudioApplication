#!/usr/bin/env python3
"""
Re-translates all non-IDD, non-OutputVariables contexts in .ts files using
gui_string_definitions.json (produced by build_gui_definitions.py) for context.

Usage:
    python retranslate_gui_strings.py                        # es + fr (default)
    python retranslate_gui_strings.py --lang es              # Spanish only
    python retranslate_gui_strings.py --lang es fr de
    python retranslate_gui_strings.py --all                  # all 18 languages
    python retranslate_gui_strings.py --model claude-haiku-4-5-20251001

Always follow with:
    python fix_and_unvanish.py
    cmake --build . --target OpenStudioApplication_lrelease
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

DEFAULT_LANGS = ["es", "fr"]
DEFAULT_MODEL = "claude-haiku-4-5-20251001"
MAX_POLL_SECONDS = 1800  # 30 min — bail out if a batch stalls on Anthropic's side
TS_TEMPLATE = "OpenStudioApp_{lang}.ts"
DEFINITIONS_FILE = "gui_string_definitions.json"
KEY_FILE = r"C:\Users\ml\OneDrive\ClaudeAPIkey.txt"
SKIP_CONTEXTS = {"IDD", "OutputVariables"}


# ---------------------------------------------------------------------------
# System prompt
# ---------------------------------------------------------------------------

def make_system_prompt(language_name: str) -> str:
    return (
        f"You are a professional technical translator for the OpenStudio Application, "
        f"a building energy modelling GUI built on EnergyPlus / OpenStudio.\n\n"
        f"You translate short UI strings — labels, button text, menu items, dialog titles, "
        f"status and error messages — into {language_name}.\n\n"
        f"Each string is tagged with one of three categories:\n"
        f"  general_software    — common software UI terms (Save, Cancel, Name:, Apply, …)\n"
        f"  hvac_terminology    — HVAC and building-physics terms\n"
        f"  openstudio_specific — OpenStudio / EnergyPlus workflow terms\n"
        f"When a Context note is provided, use it to understand the precise meaning.\n\n"
        f"Return ONLY the {language_name} translation — nothing else. "
        f"No explanations, no parenthetical notes, no questions.\n\n"
        f"=== Rules ===\n"
        f"Qt keyboard accelerators: keep the & marker. "
        f"  '&File' → '&Archivo' (Spanish example). Place & before the appropriate letter.\n"
        f"Format placeholders: keep %1, %2, %3, … exactly as-is in the same relative position.\n"
        f"HTML markup: preserve all HTML tags (<b>, <br/>, <strong>, <p>, <a href=...>, etc.) "
        f"  unchanged and in place; translate only the visible text between tags.\n"
        f"Section-separator strings (=====, ───, etc.): keep separators as-is; translate any "
        f"  enclosed label text.\n"
        f"File extension patterns like (*.osm), (*.idf), (*.xml), ;;CSV Files(*.csv): "
        f"  return them untranslated.\n"
        f"Keep these acronyms/identifiers unchanged "
        f"(their expansions below are for context only, do not include them in the translation):\n"
        f"  HVAC (Heating, Ventilation, Air Conditioning), "
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
        f"DX (Direct Expansion), "
        f"VFD (Variable Frequency Drive), "
        f"PTAC (Packaged Terminal Air Conditioner), "
        f"PTHP (Packaged Terminal Heat Pump), "
        f"ASHRAE (American Society of Heating, Refrigerating and Air-Conditioning Engineers), "
        f"BCL (Building Component Library), "
        f"IDD, OSM, IDF, gbXML, SDD, FloorspaceJS, BIMserver, QAQC.\n"
        f"Program / brand names that stay untranslated: "
        f"OpenStudio, EnergyPlus, FloorspaceJS, BIMserver, Radiance.\n"
        f"Language names (Arabic, Chinese, Spanish, …): translate to their {language_name} equivalents.\n"
        f"Month names and time/date units: translate normally.\n"
        f"'Schedule' → use the standard {language_name} term in energy modelling software.\n"
        f"'Measure' (OpenStudio) → a Ruby/Python parametric script; use the standard "
        f"{language_name} translation of 'measure' as used in building energy software.\n"
        f"'Space Type' (OpenStudio) → a reusable zone template; translate as the "
        f"standard {language_name} term.\n"
        f"Keep SI unit abbreviations unchanged (W, kWh, m², °C, kg/s, Pa, etc.).\n"
        f"Keep proper-noun HVAC equipment names standard in {language_name}-speaking markets.\n"
    )


# ---------------------------------------------------------------------------
# .ts file helpers
# ---------------------------------------------------------------------------

def extract_gui_sources(ts_content: str, mode: str = "unfinished") -> list[tuple[str, str]]:
    """Return [(context_name, source_string), ...] for all non-skipped contexts.

    mode='unfinished': only entries with an empty or type="unfinished" translation.
    mode='all':        every entry regardless of existing translation.
    """
    ctx_pattern = re.compile(
        r"<context>\s*<name>(.*?)</name>(.*?)</context>",
        re.DOTALL,
    )
    result: list[tuple[str, str]] = []
    if mode == "all":
        src_pattern = re.compile(r"<source>([\s\S]+?)</source>", re.DOTALL)
        for m in ctx_pattern.finditer(ts_content):
            ctx_name = unescape(m.group(1).strip())
            if ctx_name in SKIP_CONTEXTS:
                continue
            for sm in src_pattern.finditer(m.group(2)):
                result.append((ctx_name, unescape(sm.group(1).strip())))
    else:
        msg_pattern = re.compile(
            r"<source>([\s\S]+?)</source>\s*"
            r'<translation(?:\s+type="unfinished")?>\s*</translation>',
            re.DOTALL,
        )
        for m in ctx_pattern.finditer(ts_content):
            ctx_name = unescape(m.group(1).strip())
            if ctx_name in SKIP_CONTEXTS:
                continue
            for sm in msg_pattern.finditer(m.group(2)):
                result.append((ctx_name, unescape(sm.group(1).strip())))
    return result


def apply_gui_translations(
    ts_content: str,
    source_to_translation: dict[str, str],
    mode: str = "unfinished",
) -> str:
    """Replace <translation> values in all non-IDD/non-OutputVariables contexts."""
    trans_pattern = (
        r"<source>([\s\S]+?)</source>\s*<translation[^>]*>[\s\S]*?</translation>"
        if mode == "all" else
        r'<source>([\s\S]+?)</source>\s*<translation(?:\s+type="unfinished")?>\s*</translation>'
    )

    def process_context(m: re.Match) -> str:
        full = m.group(0)
        name_m = re.match(r"<context>\s*<name>(.*?)</name>", full, re.DOTALL)
        if name_m:
            ctx_name = unescape(name_m.group(1).strip())
            if ctx_name in SKIP_CONTEXTS:
                return full

        def replace_msg(msg_m: re.Match) -> str:
            source = unescape(msg_m.group(1).strip())
            new_t = source_to_translation.get(source)
            if new_t is None:
                return msg_m.group(0)
            esc = (
                new_t
                .replace("&", "&amp;")
                .replace("<", "&lt;")
                .replace(">", "&gt;")
            )
            return (
                f"<source>{msg_m.group(1)}</source>\n"
                f"        <translation>{esc}</translation>"
            )

        return re.sub(trans_pattern, replace_msg, full, flags=re.DOTALL)

    return re.sub(
        r"<context>[\s\S]+?</context>",
        process_context,
        ts_content,
        flags=re.DOTALL,
    )


# ---------------------------------------------------------------------------
# Batch helpers
# ---------------------------------------------------------------------------

def build_user_message(source: str, defn: dict | None) -> str:
    """Build user-turn content for a single GUI string."""
    if defn:
        cat = defn.get("category", "general_software")
        definition = defn.get("definition", "")
        parts = [f"Category: {cat}"]
        if definition:
            parts.append(f"Context: {definition}")
        return "\n".join(parts) + f"\n\n<translate>{source}</translate>"
    return f"<translate>{source}</translate>"


def build_batch_requests(
    unique_sources: list[str],
    definitions: dict,
    language_name: str,
    model: str,
) -> list[Request]:
    system = make_system_prompt(language_name)
    requests_list: list[Request] = []
    for i, source in enumerate(unique_sources):
        defn = definitions.get(source)
        requests_list.append(
            Request(
                custom_id=f"gui-{i}",
                params=MessageCreateParamsNonStreaming(
                    model=model,
                    max_tokens=256,
                    system=system,
                    messages=[{"role": "user", "content": build_user_message(source, defn)}],
                ),
            )
        )
    return requests_list


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Re-translate GUI contexts using gui_string_definitions.json."
    )
    parser.add_argument(
        "--lang", nargs="+", metavar="CODE",
        help=f"Language codes to process (default: {DEFAULT_LANGS}).",
    )
    parser.add_argument(
        "--all", action="store_true",
        help="Process all 18 supported languages.",
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
            "WARNING: --mode all generates a fresh machine translation for every GUI entry,\n"
            "overwriting any human-provided translations in the .ts files.\n"
            "Intended use: populate gui_comparison.csv New ES/FR columns so machine\n"
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
        sys.exit(
            f"ERROR: {DEFINITIONS_FILE} not found. "
            "Run build_gui_definitions.py first."
        )
    print(f"Loaded {len(definitions)} GUI string definitions from {DEFINITIONS_FILE}")

    # Category stats
    from collections import Counter
    cats = Counter(v.get("category", "?") for v in definitions.values())
    for cat, count in sorted(cats.items()):
        print(f"  {cat:25s}: {count}")

    # Load API key
    key_text = open(KEY_FILE, encoding="utf-8").read()
    key_match = re.search(r"sk-ant-[A-Za-z0-9_\-]+", key_text)
    if not key_match:
        sys.exit("ERROR: No API key found in key file.")
    client = anthropic.Anthropic(api_key=key_match.group(0))

    # Phase 1: Submit one batch per language
    batch_jobs: dict[str, dict] = {}

    for lang_code, lang_name in lang_map.items():
        ts_file = TS_TEMPLATE.format(lang=lang_code)
        print(f"\n[{lang_code}] {lang_name}")

        try:
            content = open(ts_file, encoding="utf-8").read()
        except FileNotFoundError:
            print(f"  SKIP: {ts_file} not found")
            continue

        all_pairs = extract_gui_sources(content, mode=args.mode)
        if not all_pairs:
            print(f"  SKIP: no translatable contexts found")
            continue

        # Deduplicate: translate each unique source string once
        seen: set[str] = set()
        unique_sources: list[str] = []
        for _ctx, src in all_pairs:
            if src not in seen:
                seen.add(src)
                unique_sources.append(src)

        with_def = sum(1 for s in unique_sources if definitions.get(s, {}).get("definition"))
        print(
            f"  {len(all_pairs)} total strings, {len(unique_sources)} unique  "
            f"({with_def} with definition, "
            f"{len(unique_sources) - with_def} without)"
        )

        requests_list = build_batch_requests(unique_sources, definitions, lang_name, args.model)
        batch = client.messages.batches.create(requests=requests_list)
        print(f"  Batch submitted: {batch.id}  ({len(requests_list)} requests)")

        batch_jobs[lang_code] = {
            "batch_id": batch.id,
            "ts_file": ts_file,
            "lang_name": lang_name,
            "unique_sources": unique_sources,
        }

    if not batch_jobs:
        print("\nNothing to process.")
        return

    # Phase 2: Poll until all batches are done
    print(f"\n{len(batch_jobs)} batch(es) submitted. Polling every 15 s (timeout {MAX_POLL_SECONDS//60} min) ...", flush=True)
    pending = set(batch_jobs.keys())
    deadline = time.time() + MAX_POLL_SECONDS
    while pending:
        time.sleep(15)
        still_pending: set[str] = set()
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

    # Phase 3: Apply translations to .ts files
    print("\nApplying translations ...")
    for lang_code, job in batch_jobs.items():
        id_to_translation: dict[str, str] = {}
        for result in client.messages.batches.results(job["batch_id"]):
            if result.result.type == "succeeded":
                msg = result.result.message
                text = next((b.text for b in msg.content if b.type == "text"), "").strip()
                # Strip residual XML wrapper if the model accidentally included it
                text = re.sub(r"<[^>]+>", "", text).strip()
                if text:
                    id_to_translation[result.custom_id] = text
            else:
                print(f"  [{lang_code}] WARN: {result.custom_id} - {result.result.type}")

        # Map custom_id index back to source string
        source_to_translation: dict[str, str] = {}
        for i, src in enumerate(job["unique_sources"]):
            t = id_to_translation.get(f"gui-{i}")
            if t:
                source_to_translation[src] = t

        content = open(job["ts_file"], encoding="utf-8").read()
        new_content = apply_gui_translations(content, source_to_translation, mode=args.mode)

        with open(job["ts_file"], "w", encoding="utf-8") as f:
            f.write(new_content)

        applied = len(source_to_translation)
        total = len(job["unique_sources"])
        print(f"  [{lang_code}] {applied}/{total} unique strings translated -> {job['ts_file']}")

    print("\nDone. Run:")
    print("  python fix_and_unvanish.py")
    print("  cmake --build . --target OpenStudioApplication_lrelease")


if __name__ == "__main__":
    main()

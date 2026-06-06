#!/usr/bin/env python3
"""
Translates all unfinished strings in every OpenStudioApp .ts file using
definition-aware prompts for each of the three string contexts:

  IDD            → idd_field_definitions.json  (EnergyPlus I/O Reference field docs)
  OutputVariables→ output_var_definitions.json  (EnergyPlus I/O Reference variable docs)
  All other GUI  → gui_string_definitions.json  (category + UI-role definitions)

Workflow:
  1. Uses OpenStudioApp_es.ts as the master source-string template.
  2. For each target language, overlays existing translations and marks the
     rest as unfinished stubs.
  3. Deduplicates: one API request per unique source string (not per occurrence),
     so "Name:" appearing in 90 inspector views costs one request, not ninety.
  4. Dispatches each string to the correct system prompt based on its .ts context.
  5. Submits one batch per language simultaneously; polls until all complete.
  6. Applies translations back to every occurrence of each source string.

Usage:
    python translate_all_languages.py                    # all 18 languages
    python translate_all_languages.py --lang ko pt tr    # specific languages
    python translate_all_languages.py --model claude-haiku-4-5-20251001
"""

import argparse
import json
import re
import time
import sys
from collections import defaultdict
from html import unescape
import anthropic
from anthropic.types.message_create_params import MessageCreateParamsNonStreaming
from anthropic.types.messages.batch_create_params import Request

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

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

DEFAULT_MODEL    = "claude-haiku-4-5-20251001"
MAX_POLL_SECONDS = 1800  # 30 min — bail out if a batch stalls on Anthropic's side
SPANISH_TS    = "OpenStudioApp_es.ts"
TS_TEMPLATE   = "OpenStudioApp_{lang}.ts"
KEY_FILE      = r"C:\Users\ml\OneDrive\ClaudeAPIkey.txt"

IDD_DEFS_FILE = "idd_field_definitions.json"
OV_DEFS_FILE  = "output_var_definitions.json"
GUI_DEFS_FILE = "gui_string_definitions.json"

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def load_json_optional(path: str) -> dict:
    try:
        with open(path, encoding="utf-8") as f:
            return json.load(f)
    except FileNotFoundError:
        return {}


def xml_escape(text: str) -> str:
    return text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


# ===========================================================================
# IDD CONTEXT — system prompt + user message
# Derived from retranslate_idd_fields.py
# ===========================================================================

def _idd_system_prompt(language_name: str) -> str:
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
        f"- Keep the following acronyms unchanged: "
        f"HVAC, COP, EIR, SHR, PLR, VRF, DOAS, VAV, CAV, EMS, AHU, DX, VFD, "
        f"PTAC, PTHP, ASHRAE, DCV, ERV, ITE, AHRI, ARI.\n"
        f"- 'Schedule' translates to the standard {language_name} term used in energy modeling.\n"
        f"- 'Fraction' = dimensionless ratio between 0 and 1.\n"
        f"- 'Coefficient' = technical coefficient in an equation.\n"
    )


def _idd_user_message(field_name: str, defn: dict | None) -> str:
    if defn and defn.get("definition"):
        obj = defn.get("object", "")
        parts = []
        if obj:
            parts.append(f"Object: {obj}")
        parts.append(f"Definition: {defn['definition']}")
        return "\n".join(parts) + f"\n\n<translate>{field_name}</translate>"
    return f"<translate>{field_name}</translate>"


# ===========================================================================
# OUTPUT VARIABLES CONTEXT — system prompt + category detection + user message
# Derived from retranslate_output_vars.py
# ===========================================================================

CANONICAL_CATEGORIES = sorted([
    "Air System", "Air Terminal", "Chilled Water Thermal Storage Tank", "Chiller",
    "Cooling Coil", "Cooling Panel", "Cooling Tower", "Daylighting",
    "Electric Load Center", "Exterior Windows", "Fluid Heat Exchanger", "Generator",
    "Heat Exchanger", "Heating Coil", "ITE", "Ice Thermal Storage", "Ideal Loads",
    "Infiltration", "Interior Windows", "Lights", "Mechanical Ventilation", "Plant",
    "Predicted", "Refrigeration Air Chiller System", "Refrigeration System",
    "Refrigeration Walk In", "Solar Collector", "System Node", "Thermal Comfort",
    "Unit Ventilator", "Unitary System", "VRF Air Terminal", "VRF Heat Pump",
    "Ventilation", "Ventilator", "Water Heater", "Water Use Connections", "Windows", "Zone",
], key=len, reverse=True)

_CANONICAL_NO_ZONE = [c for c in CANONICAL_CATEGORIES if c != "Zone"]
_ZONE_SUBCAT_EXCL  = {"Total Internal"}
_MIN_CAT_COUNT     = 3


def _build_raw_cat_map(sources: list[str]) -> dict[str, str]:
    prefix_count: dict[str, int] = defaultdict(int)
    for name in sources:
        words = name.split()
        for n in range(1, len(words)):
            prefix_count[" ".join(words[:n])] += 1
    cats: dict[str, str] = {}
    for name in sources:
        words = name.split()
        best = words[0]
        for n in range(2, len(words)):
            prefix = " ".join(words[:n])
            if prefix_count[prefix] >= _MIN_CAT_COUNT:
                best = prefix
        cats[name] = best
    return cats


def _consolidate(cat: str, canon: list[str]) -> str:
    for c in canon:
        if cat == c or cat.startswith(c + " "):
            return c
    return cat


def build_category_map(sources: list[str]) -> tuple[dict[str, str], dict[str, str]]:
    raw = _build_raw_cat_map(sources)
    top = {s: _consolidate(c, CANONICAL_CATEGORIES) for s, c in raw.items()}
    zone_stripped = {s: s[5:] for s in sources if top.get(s) == "Zone" and s.startswith("Zone ")}
    sub_raw = _build_raw_cat_map(list(zone_stripped.values()))
    zone_sub: dict[str, str] = {}
    for src, stripped in zone_stripped.items():
        subcat = _consolidate(sub_raw.get(stripped, stripped.split()[0]), _CANONICAL_NO_ZONE)
        if subcat not in _ZONE_SUBCAT_EXCL:
            zone_sub[src] = subcat
    return top, zone_sub


def _format_ov_with_category(var_name: str, top: dict[str, str], zone_sub: dict[str, str]) -> str:
    cat = top.get(var_name, var_name.split()[0])
    if cat == "Zone" and var_name in zone_sub:
        subcat = zone_sub[var_name]
        remainder = var_name[5:][len(subcat):].strip()
        return f"Zone: {subcat}: {remainder}" if remainder else f"Zone: {subcat}"
    measurement = var_name[len(cat):].strip()
    return f"{cat}: {measurement}" if measurement else var_name


def build_ov_template_index(definitions: dict) -> list[tuple]:
    index = []
    for key, entry in definitions.items():
        if "<" not in key:
            continue
        pattern = re.escape(key)
        pattern = re.sub(r"\\<[^>]+\\>", ".+", pattern)
        index.append((re.compile(f"^{pattern}$", re.IGNORECASE), entry))
    return index


def _lookup_ov_defn(var_name: str, definitions: dict, template_index: list[tuple]) -> dict | None:
    entry = definitions.get(var_name)
    if entry:
        return entry
    for pattern, entry in template_index:
        if pattern.match(var_name):
            return entry
    return None


def _ov_system_prompt(language_name: str) -> str:
    return (
        f"You are a professional technical translator for building energy simulation software "
        f"(EnergyPlus / OpenStudio). "
        f"You are translating EnergyPlus output variable names into {language_name}.\n\n"
        f"Each variable name is presented with a colon separating two parts:\n"
        f"  [Category]: [Measurement]\n"
        f"The Category identifies the equipment or system. The Measurement is the specific "
        f"physical quantity being reported.\n"
        f"Translate BOTH parts, keeping the colon separator. "
        f"The same Category must translate identically every time it appears.\n\n"
        f"When a Definition and Units are provided before <translate>, use them to understand "
        f"the precise physical meaning and choose accurate {language_name} HVAC / building-physics "
        f"terminology.\n\n"
        f"Return ONLY the {language_name} translation — nothing else.\n\n"
        f"Rules:\n"
        f"- Use standard HVAC and building-physics terminology in {language_name}.\n"
        f"- Keep SI unit abbreviations unchanged (kg/s, W, kWh, °C, m³/s, Pa, J, m²).\n"
        f"- Keep the following acronyms unchanged: "
        f"HVAC, COP, EIR, SHR, PLR, VRF, DOAS, VAV, CAV, EMS, AHU, DX, VFD, "
        f"PTAC, PTHP, ASHRAE, DCV, ERV, ITE.\n"
        f"- 'Rate' = rate of a physical process (not a tariff or price).\n"
        f"- 'Energy' = physical energy quantity (not economic value).\n"
        f"- 'Fraction' = dimensionless ratio between 0 and 1.\n"
    )


def _ov_user_message(var_name: str, formatted: str, defn: dict | None) -> str:
    if defn and defn.get("definition"):
        units_str = f" [{defn['units']}]" if defn.get("units") else " dimensionless"
        return (
            f"Definition: {defn['definition']}\n"
            f"Units:{units_str}\n\n"
            f"<translate>{formatted}</translate>"
        )
    return f"<translate>{formatted}</translate>"


# ===========================================================================
# GUI CONTEXT — system prompt + context roles + user message
# Derived from retranslate_gui_strings.py and build_gui_definitions.py
# ===========================================================================

_GUI_CONTEXT_EXACT: dict[str, str] = {
    "openstudio::MainMenu":           "item in the application menu bar",
    "openstudio::MainWindow":         "main application window element",
    "openstudio::OpenStudioApp":      "application-level dialog or status message",
    "openstudio::OSDocument":         "document operation (save, open, import, export)",
    "openstudio::OSDropZone":         "drag-and-drop zone where users drop library objects into the model",
    "openstudio::OSItemSelectorButtons": "item selector buttons (Add, Copy, Remove)",
    "openstudio::OSGridController":   "spreadsheet grid column header or cell tooltip",
    "openstudio::RunView":            "simulation run panel (start/stop, progress log)",
    "openstudio::ResultsView":        "simulation results browser",
    "openstudio::VariablesList":      "output variable selection list",
    "openstudio::VariablesTabView":   "Output Variables tab heading",
    "openstudio::LifeCycleCostsView": "life cycle cost analysis input form",
    "openstudio::MeasureManager":     "Measures update/sync status message",
    "openstudio::BuildingComponentDialog":
        "BCL (Building Component Library) browser dialog",
    "openstudio::ApplyMeasureNowDialog":
        "dialog to apply an OpenStudio Measure immediately to the current model",
    "openstudio::EditorWebView":      "FloorspaceJS 3D geometry editor",
    "TaxonomyCategories":
        "category label in the BCL Measures taxonomy browser",
    "UtilityBillsView":
        "utility bill type heading (electric, gas, water, etc.)",
}

_GUI_SUFFIX_ROLES: list[tuple[str, str]] = [
    ("GridController",  "column header or tooltip in a spreadsheet grid view"),
    ("GridView",        "heading or filter label in a spreadsheet grid view"),
    ("InspectorView",   "field label in the Inspector panel (object property editor)"),
    ("TabController",   "navigation tab label"),
    ("TabView",         "navigation tab label or section heading"),
    ("DropZoneView",    "drag-and-drop target where users drop objects from the library panel"),
    ("DropZoneItem",    "label on an item inside a drag-and-drop zone"),
    ("DropZone",        "drag-and-drop target label"),
    ("ControlsView",    "HVAC controls configuration form field"),
    ("MiniView",        "compact thumbnail card in the HVAC system overview"),
    ("ItemDelegate",    "item label in a list or table widget"),
    ("Dialog",          "dialog window title or field label"),
]


def _get_context_role(ctx_name: str) -> str:
    if ctx_name in _GUI_CONTEXT_EXACT:
        return _GUI_CONTEXT_EXACT[ctx_name]
    short = ctx_name.replace("openstudio::", "")
    for suffix, role in _GUI_SUFFIX_ROLES:
        if short.endswith(suffix):
            prefix = short[: -len(suffix)]
            return f"{role} ({prefix})" if prefix else role
    return f"UI label in the {short} view"


def _gui_system_prompt(language_name: str) -> str:
    return (
        f"You are a professional technical translator for the OpenStudio Application, "
        f"a building energy modelling GUI built on EnergyPlus / OpenStudio.\n\n"
        f"You translate short UI strings — labels, button text, menu items, dialog titles, "
        f"status messages, error messages — into {language_name}.\n\n"
        f"Each string may be tagged with a Category and Context note. Use them.\n\n"
        f"Return ONLY the {language_name} translation — nothing else.\n\n"
        f"=== Rules ===\n"
        f"Qt accelerators: keep the & marker in place — '&File' -> '&Archivo'.\n"
        f"Format placeholders: keep %1, %2, %3 exactly as-is.\n"
        f"HTML markup: preserve all tags (<b>, <br/>, <strong>, <a href=...>); "
        f"translate only visible text between tags.\n"
        f"File extension patterns like (*.osm), (*.idf): return untranslated.\n"
        f"Keep these acronyms/identifiers unchanged: "
        f"HVAC, COP, EIR, SHR, PLR, VRF, DOAS, VAV, CAV, EMS, AHU, DX, VFD, "
        f"PTAC, PTHP, ASHRAE, BCL, IDD, OSM, IDF, gbXML, SDD, FloorspaceJS, "
        f"BIMserver, QAQC.\n"
        f"Program names that stay untranslated: OpenStudio, EnergyPlus, FloorspaceJS, "
        f"BIMserver, Radiance.\n"
        f"Language names (Arabic, Chinese, Spanish, …): translate to their "
        f"{language_name} equivalents.\n"
        f"Month names and date/time units: translate normally.\n"
        f"'Schedule' -> standard {language_name} term in energy modelling software.\n"
        f"'Measure' (OpenStudio) -> Ruby/Python parametric script; use the standard "
        f"{language_name} translation used in building energy software.\n"
        f"Keep SI unit abbreviations unchanged (W, kWh, m², °C, etc.).\n"
    )


def _gui_user_message(source: str, ctx_name: str, gui_defs: dict) -> str:
    role = _get_context_role(ctx_name)
    defn = gui_defs.get(source) or gui_defs.get(source.rstrip(": "))
    parts = [f"UI Role: {role}"]
    if defn:
        cat = defn.get("category", "")
        definition = defn.get("definition", "")
        if cat:
            parts.append(f"Category: {cat}")
        if definition:
            parts.append(f"Context: {definition}")
    return "\n".join(parts) + f"\n\n<translate>{source}</translate>"


# ===========================================================================
# .ts file helpers (from existing translate_all_languages.py)
# ===========================================================================

def build_translation_lookup(ts_content: str) -> dict[str, str]:
    """Return {source_text: translation_text} for all non-empty translations."""
    pattern = re.compile(
        r'<source>([\s\S]+?)</source>\s*'
        r'(?:<comment>[\s\S]*?</comment>\s*)?'
        r'(?:<translatorcomment>[\s\S]*?</translatorcomment>\s*)?'
        r'<translation[^>]*>([\s\S]*?)</translation>',
        re.DOTALL,
    )
    lookup: dict[str, str] = {}
    for m in pattern.finditer(ts_content):
        translation_text = m.group(2).strip()
        if translation_text:
            lookup[m.group(1).strip()] = translation_text
    return lookup


def build_merged_file(es_content: str, existing_lookup: dict[str, str], lang_code: str) -> str:
    """
    Start from the Spanish master. For each message:
    - Use existing target-language translation if available.
    - Otherwise insert an empty unfinished stub.
    Update the <TS language="..."> attribute.
    """
    content = re.sub(
        r'(<TS[^>]*language=")[^"]*(")',
        rf'\g<1>{lang_code}\2',
        es_content,
    )

    def replace_translation(m: re.Match) -> str:
        source = m.group(1).strip()
        existing = existing_lookup.get(source, "")
        if existing:
            return (
                f"<source>{m.group(1)}</source>\n"
                f"        <translation>{existing}</translation>"
            )
        return (
            f"<source>{m.group(1)}</source>\n"
            f"        <translation type=\"unfinished\"></translation>"
        )

    return re.sub(
        r'<source>([\s\S]+?)</source>\s*(?:<comment>[\s\S]*?</comment>\s*)?'
        r'(?:<translatorcomment>[\s\S]*?</translatorcomment>\s*)?'
        r'<translation[^>]*>[\s\S]*?</translation>',
        replace_translation,
        content,
        flags=re.DOTALL,
    )


def extract_unfinished(ts_content: str) -> list[dict]:
    """
    Return all unfinished entries in document order with context names.
    Each entry: {source, context, first_occurrence}
    """
    ctx_pattern = re.compile(
        r'<context>\s*<name>([\s\S]+?)</name>([\s\S]*?)</context>',
        re.DOTALL,
    )
    unfinished_pattern = re.compile(
        r'<source>([\s\S]+?)</source>[\s\S]*?'
        r'<translation type="unfinished"></translation>',
        re.DOTALL,
    )
    entries: list[dict] = []
    seen_sources: set[str] = set()
    for ctx_m in ctx_pattern.finditer(ts_content):
        ctx_name = unescape(ctx_m.group(1).strip())
        for msg_m in unfinished_pattern.finditer(ctx_m.group(2)):
            source = unescape(msg_m.group(1).strip())
            entries.append({
                "source":  source,
                "context": ctx_name,
                "is_first": source not in seen_sources,
            })
            seen_sources.add(source)
    return entries


def apply_translations(ts_content: str, source_to_translation: dict[str, str]) -> str:
    """Replace all unfinished stubs whose source is in source_to_translation."""
    def replace_msg(m: re.Match) -> str:
        source = unescape(m.group(1).strip())
        t = source_to_translation.get(source)
        if t is None:
            return m.group(0)
        # Strip any XML tags the model may have accidentally included
        t = re.sub(r"<[^>]+>", "", t).strip()
        if not t:
            return m.group(0)
        return (
            f"<source>{m.group(1)}</source>\n"
            f"        <translation>{xml_escape(t)}</translation>"
        )

    return re.sub(
        r'<source>([\s\S]+?)</source>\s*<translation type="unfinished"></translation>',
        replace_msg,
        ts_content,
        flags=re.DOTALL,
    )


# ===========================================================================
# Batch request builder — dispatches to the right prompt per context
# ===========================================================================

def build_request(
    source: str,
    ctx_name: str,
    lang_name: str,
    model: str,
    idd_defs: dict,
    ov_defs: dict,
    ov_top: dict,
    ov_zone_sub: dict,
    ov_template_index: list,
    gui_defs: dict,
    request_id: str,
) -> Request:
    if ctx_name == "IDD":
        system    = _idd_system_prompt(lang_name)
        user      = _idd_user_message(source, idd_defs.get(source))
        max_tok   = 128
    elif ctx_name == "OutputVariables":
        system    = _ov_system_prompt(lang_name)
        defn      = _lookup_ov_defn(source, ov_defs, ov_template_index)
        formatted = _format_ov_with_category(source, ov_top, ov_zone_sub)
        user      = _ov_user_message(source, formatted, defn)
        max_tok   = 256
    else:
        system    = _gui_system_prompt(lang_name)
        user      = _gui_user_message(source, ctx_name, gui_defs)
        max_tok   = 256

    return Request(
        custom_id=request_id,
        params=MessageCreateParamsNonStreaming(
            model=model,
            max_tokens=max_tok,
            system=system,
            messages=[{"role": "user", "content": user}],
        ),
    )


# ===========================================================================
# Main
# ===========================================================================

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Translate unfinished strings in OpenStudioApp .ts files (definition-aware)."
    )
    parser.add_argument(
        "--lang", nargs="+", metavar="CODE",
        help="Only process these language codes. Default: all 18 languages.",
    )
    parser.add_argument(
        "--model", default=DEFAULT_MODEL,
        help=f"Claude model to use (default: {DEFAULT_MODEL}).",
    )
    args = parser.parse_args()

    languages = {k: v for k, v in LANGUAGES.items() if not args.lang or k in args.lang}
    if not languages:
        sys.exit(f"ERROR: No matching languages. Valid codes: {sorted(LANGUAGES)}")

    # Load API key
    key_text  = open(KEY_FILE, encoding="utf-8").read()
    key_match = re.search(r"sk-ant-[A-Za-z0-9_\-]+", key_text)
    if not key_match:
        sys.exit("ERROR: No API key found in key file.")
    client = anthropic.Anthropic(api_key=key_match.group(0))

    # Load definition files (missing files → empty dicts, handled gracefully)
    idd_defs = load_json_optional(IDD_DEFS_FILE)
    ov_defs  = load_json_optional(OV_DEFS_FILE)
    gui_defs = load_json_optional(GUI_DEFS_FILE)

    print(f"Definitions loaded:  IDD={len(idd_defs)}  OV={len(ov_defs)}  GUI={len(gui_defs)}")
    if not idd_defs:
        print(f"  (run scrape_idd_field_definitions.py to populate {IDD_DEFS_FILE})")
    if not ov_defs:
        print(f"  (run scrape_output_var_definitions.py to populate {OV_DEFS_FILE})")
    if not gui_defs:
        print(f"  (run build_gui_definitions.py to populate {GUI_DEFS_FILE})")

    # Pre-compute OutputVariables category map from the Spanish master
    print(f"\nReading Spanish master: {SPANISH_TS}")
    es_content = open(SPANISH_TS, encoding="utf-8").read()
    ov_sources_all = re.findall(
        r'<context>\s*<name>OutputVariables</name>[\s\S]*?</context>',
        es_content,
    )
    ov_all_names: list[str] = []
    if ov_sources_all:
        ov_all_names = [
            unescape(s.strip())
            for s in re.findall(r'<source>([^<]+)</source>', ov_sources_all[0])
        ]
    ov_top, ov_zone_sub = build_category_map(ov_all_names)
    ov_template_index   = build_ov_template_index(ov_defs)
    print(f"  {es_content.count('<message>')} total messages in Spanish master")
    print(f"  {len(ov_all_names)} OutputVariables  |  "
          f"{len([c for c in ov_top.values() if c == 'Zone'])} Zone variables")

    # Phase 1: Merge + submit batches
    batch_jobs: dict[str, dict] = {}

    for lang_code, lang_name in languages.items():
        ts_file = TS_TEMPLATE.format(lang=lang_code)
        print(f"\n[{lang_code}] {lang_name}")

        try:
            existing_content = open(ts_file, encoding="utf-8").read()
        except FileNotFoundError:
            print(f"  NOTE: {ts_file} not found — creating from Spanish master")
            existing_content = ""

        existing_lookup = build_translation_lookup(existing_content)
        print(f"  {len(existing_lookup)} existing translations preserved")

        merged = build_merged_file(es_content, existing_lookup, lang_code)
        with open(ts_file, "w", encoding="utf-8") as f:
            f.write(merged)

        all_entries  = extract_unfinished(merged)
        first_only   = [e for e in all_entries if e["is_first"]]  # deduplicated
        total_stubs  = len(all_entries)
        unique_stubs = len(first_only)
        print(f"  {total_stubs} unfinished stubs  ({unique_stubs} unique source strings)")

        if not first_only:
            print(f"  Nothing to translate")
            continue

        # Count by context type
        idd_count = sum(1 for e in first_only if e["context"] == "IDD")
        ov_count  = sum(1 for e in first_only if e["context"] == "OutputVariables")
        gui_count = unique_stubs - idd_count - ov_count
        print(f"  IDD={idd_count}  OutputVars={ov_count}  GUI={gui_count}")

        requests_list = [
            build_request(
                source       = e["source"],
                ctx_name     = e["context"],
                lang_name    = lang_name,
                model        = args.model,
                idd_defs     = idd_defs,
                ov_defs      = ov_defs,
                ov_top       = ov_top,
                ov_zone_sub  = ov_zone_sub,
                ov_template_index = ov_template_index,
                gui_defs     = gui_defs,
                request_id   = f"t-{i}",
            )
            for i, e in enumerate(first_only)
        ]

        batch = client.messages.batches.create(requests=requests_list)
        print(f"  Batch submitted: {batch.id}  ({len(requests_list)} requests)")

        batch_jobs[lang_code] = {
            "batch_id":    batch.id,
            "ts_file":     ts_file,
            "lang_name":   lang_name,
            "first_only":  first_only,
        }

    if not batch_jobs:
        print("\nAll languages already complete.")
        return

    # Phase 2: Poll until all batches complete
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

    # Phase 3: Apply translations
    print("\nApplying translations ...")
    for lang_code, job in batch_jobs.items():
        # Map request index back to source string
        idx_to_source = {f"t-{i}": e["source"] for i, e in enumerate(job["first_only"])}

        source_to_translation: dict[str, str] = {}
        for result in client.messages.batches.results(job["batch_id"]):
            if result.result.type == "succeeded":
                text = next(
                    (b.text for b in result.result.message.content if b.type == "text"), ""
                ).strip()
                src = idx_to_source.get(result.custom_id, "")
                if text and src:
                    source_to_translation[src] = text
            else:
                src = idx_to_source.get(result.custom_id, result.custom_id)
                print(f"  [{lang_code}] WARN: {src[:60]} - {result.result.type}")

        content     = open(job["ts_file"], encoding="utf-8").read()
        new_content = apply_translations(content, source_to_translation)
        with open(job["ts_file"], "w", encoding="utf-8") as f:
            f.write(new_content)

        remaining = len(extract_unfinished(new_content))
        print(
            f"  [{lang_code}] {len(source_to_translation)}/{len(job['first_only'])} "
            f"unique strings translated  ({remaining} stubs remaining)"
        )

    print(
        "\nDone. Run:\n"
        "  python fix_and_unvanish.py\n"
        "  cmake --build . --target OpenStudioApplication_lrelease"
    )


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""
Classifies and defines all OpenStudio GUI strings (non-IDD, non-OutputVariables
contexts) into three categories for use by retranslate_gui_strings.py.

Three information sources are layered for each string:
  1. Qt context name  → UI role description (grid column header, drop zone, etc.)
  2. idd_field_definitions.json → cross-reference if the string is an IDD field label
  3. sdk_doc_definitions.json  → cross-reference if the string has an OpenStudio SDK entry
     (run scrape_sdk_docs.py first to produce this file)
Claude API is used to generate the final category + translator note, enriched by
whichever of the three sources are available.

Output: gui_string_definitions.json  {source: {"category": "...", "definition": "..."}}

Usage:
    python build_gui_definitions.py
    python build_gui_definitions.py --model claude-haiku-4-5-20251001
    python build_gui_definitions.py --ts-file OpenStudioApp_es.ts
"""

import argparse
import json
import re
import time
import sys
import xml.etree.ElementTree as ET
from html import unescape
import anthropic
from anthropic.types.message_create_params import MessageCreateParamsNonStreaming
from anthropic.types.messages.batch_create_params import Request

DEFAULT_MODEL = "claude-haiku-4-5-20251001"
DEFAULT_TS_FILE = "OpenStudioApp_es.ts"
OUTPUT_FILE = "gui_string_definitions.json"
KEY_FILE = r"C:\Users\ml\OneDrive\ClaudeAPIkey.txt"
SKIP_CONTEXTS = {"IDD", "OutputVariables"}

# ---------------------------------------------------------------------------
# Context → UI role description
# ---------------------------------------------------------------------------
# Each string Claude sees will include one of these descriptions so it knows
# WHERE in the application the string appears, which dramatically improves
# the category assignment and definition quality.

_CONTEXT_EXACT: dict[str, str] = {
    "openstudio::MainMenu":
        "item in the application menu bar (File, Edit, Preferences, Help, …)",
    "openstudio::MainWindow":
        "main application window element (title bar, status bar, toolbar buttons)",
    "openstudio::OpenStudioApp":
        "application-level dialog, startup message, or update notification",
    "openstudio::OSDocument":
        "document-level operation: save, open, import, export, revert-to-saved",
    "openstudio::OSDropZone":
        "generic drag-and-drop zone; the user drags an object from the library panel "
        "and drops it here to add it to the model",
    "openstudio::OSItemSelectorButtons":
        "buttons on the left-column item selector (Add, Copy, Remove, etc.)",
    "openstudio::OSGridController":
        "generic spreadsheet grid — column header or cell tooltip",
    "openstudio::RunView":
        "simulation run panel (Start/Stop buttons, progress log, status messages)",
    "openstudio::ResultsView":
        "simulation results and report browser",
    "openstudio::VariablesList":
        "list of EnergyPlus output variables the user can request for reporting",
    "openstudio::VariablesTabView":
        "Output Variables tab heading or section label",
    "openstudio::LifeCycleCostsView":
        "Life Cycle Cost Analysis input form field label or section heading",
    "openstudio::MeasureManager":
        "status message from the OpenStudio Measures update/sync manager",
    "openstudio::LibraryDialog":
        "Library file management dialog (add / remove / browse library files)",
    "openstudio::LibraryItemDelegate":
        "item label in the local component library panel",
    "openstudio::LibraryItemView":
        "item thumbnail in the local component library panel",
    "openstudio::LostCloudConnectionDialog":
        "dialog shown when the cloud connection is dropped",
    "openstudio::ExternalToolsDialog":
        "dialog for configuring paths to external tools (EnergyPlus, Radiance, etc.)",
    "openstudio::SyncMeasuresDialog":
        "dialog for synchronising Measures from the BCL (Building Component Library)",
    "openstudio::SyncMeasuresDialogCentralWidget":
        "central widget inside the Measures sync dialog",
    "openstudio::BuildingComponentDialog":
        "BCL (Building Component Library) browser dialog — search, download, and manage "
        "components and Measures from the online library",
    "openstudio::BuildingComponentDialogCentralWidget":
        "central widget inside the BCL browser (sort, filter, download controls)",
    "openstudio::ApplyMeasureNowDialog":
        "dialog for applying an OpenStudio Measure immediately to the current model",
    "openstudio::EditController":
        "Measures workflow sidebar — lists Measures applied to the model",
    "openstudio::EditRubyMeasureView":
        "inspector panel for a single OpenStudio Measure: name, description, and input arguments",
    "openstudio::EditorWebView":
        "FloorspaceJS 3D geometry editor embedded in the Geometry tab",
    "openstudio::PreviewWebView":
        "3D geometry preview panel (read-only render)",
    "openstudio::ScriptsTabView":
        "Scripts/Measures tab navigation heading",
    "openstudio::GeometryTabController":
        "Geometry tab navigation label",
    "openstudio::FacilityTabController":
        "Facility tab navigation label or sub-tab category",
    "openstudio::FacilityTabView":
        "Facility tab heading",
    "openstudio::ConstructionsTabController":
        "Constructions tab navigation label or sub-category (Constructions, Materials, Construction Sets)",
    "openstudio::ConstructionsView":
        "Constructions library list heading or filter label",
    "openstudio::MaterialsView":
        "Materials library list heading or filter label",
    "openstudio::LoadsView":
        "Loads library list heading",
    "openstudio::SpaceTypesTabView":
        "Space Types tab heading",
    "openstudio::ThermalZonesTabView":
        "Thermal Zones tab heading",
    "openstudio::HVACSystemsTabView":
        "HVAC Systems tab heading",
    "openstudio::HVACSystemsController":
        "HVAC Systems layout view — manages air loops and plant loops",
    "openstudio::HVACToolbarView":
        "toolbar in the HVAC Systems tab (zoom, add system, etc.)",
    "openstudio::LocationTabController":
        "Location/Site tab navigation label",
    "openstudio::LocationTabView":
        "Location tab heading",
    "openstudio::LocationView":
        "Location tab form: weather file, site coordinates, design days",
    "openstudio::SchedulesTabController":
        "Schedules tab navigation label or sub-tab category",
    "openstudio::SchedulesTabView":
        "Schedules tab heading",
    "openstudio::SimSettingsTabController":
        "Simulation Settings tab navigation label",
    "openstudio::ResultsTabController":
        "Results tab navigation label",
    "openstudio::bimserver::ProjectImporter":
        "BIMserver project import dialog",
    "TaxonomyCategories":
        "category label in the BCL Measures taxonomy browser "
        "(used to filter/browse Measures by discipline)",
    "UtilityBillsView":
        "utility bill type heading in the Utility Bills panel (electric, gas, water, etc.)",
    "ScheduleTypeLimitItem":
        "schedule type limit item name in the Schedules panel",
    "ScheduleOthersView":
        "schedule type selector label in the Schedules panel (Compact, Constant, File, etc.)",
}

_CONTEXT_SUFFIX_ROLES: list[tuple[str, str]] = [
    ("GridController",
     "column header, tooltip, or cell label in a spreadsheet-style grid view"),
    ("GridView",
     "heading, filter label, or drag-and-drop target label in a spreadsheet-style grid view"),
    ("InspectorView",
     "field label or section heading in the Inspector panel (object property editor sidebar)"),
    ("TabController",
     "navigation tab label or sub-panel category label in the left-side navigation"),
    ("TabView",
     "navigation tab label or section heading"),
    ("DropZoneView",
     "drag-and-drop target area; the user drags an HVAC or library object here to add it to the model"),
    ("DropZoneItem",
     "label on a drag-and-drop item that sits inside a drop zone"),
    ("DropZone",
     "drag-and-drop target label"),
    ("ControlsView",
     "HVAC system controls configuration form field (setpoint, control type, etc.)"),
    ("MiniView",
     "compact thumbnail card label in the HVAC system overview diagram"),
    ("ItemDelegate",
     "item label rendered inside a list or table widget"),
    ("Dialog",
     "dialog window title or field label"),
]


def get_context_role(ctx_name: str) -> str:
    """Return a human-readable UI role description for a Qt context name."""
    if ctx_name in _CONTEXT_EXACT:
        return _CONTEXT_EXACT[ctx_name]
    short = ctx_name.replace("openstudio::", "")
    for suffix, role in _CONTEXT_SUFFIX_ROLES:
        if short.endswith(suffix):
            # Include the class prefix for specificity, e.g.
            # "SpacesSubsurfacesGridController → column header in SpacesSubsurfaces grid"
            prefix = short[: -len(suffix)]
            if prefix:
                return f"{role} ({prefix})"
            return role
    return f"UI label in the {short} view"


# ---------------------------------------------------------------------------
# Load cross-reference files
# ---------------------------------------------------------------------------

def load_json_optional(path: str) -> dict:
    try:
        with open(path, encoding="utf-8") as f:
            return json.load(f)
    except FileNotFoundError:
        return {}


# ---------------------------------------------------------------------------
# Extract unique strings from .ts file
# ---------------------------------------------------------------------------

def extract_unique_strings(ts_path: str) -> list[tuple[str, str]]:
    """Return [(first_context_name, source_string), ...] deduped by source string."""
    tree = ET.parse(ts_path)
    root = tree.getroot()
    seen: set[str] = set()
    result: list[tuple[str, str]] = []
    for context in root.findall("context"):
        ctx_el = context.find("name")
        if ctx_el is None:
            continue
        ctx_name = ctx_el.text or ""
        if ctx_name in SKIP_CONTEXTS:
            continue
        for msg in context.findall("message"):
            src = msg.find("source")
            if src is not None and src.text:
                key = src.text.strip()
                if key not in seen:
                    seen.add(key)
                    result.append((ctx_name, key))
    return result


# ---------------------------------------------------------------------------
# System prompt
# ---------------------------------------------------------------------------

SYSTEM_PROMPT = """\
You classify OpenStudio Application GUI string labels for use by a technical translator.

Assign one of three categories:
  general_software    — common software UI terms found in any application
                        (Save, Cancel, OK, Name:, Date, File, Edit, Apply, Download, Author, …)
  hvac_terminology    — HVAC, mechanical engineering, or building-physics terms
                        (Chiller, Boiler, Coil, Heat Pump, Design Day, ASHRAE, Air Terminal,
                         DX, VAV, Economizer, Plenum, Baseboard, …)
  openstudio_specific — terms specific to the OpenStudio / EnergyPlus modelling workflow
                        (Measure, BCL, Space Type, Run Period, FloorspaceJS, gbXML, OSM, IDD,
                         Standards Building Type, Thermal Zone, Availability Manager, …)

Return ONLY valid JSON with exactly two keys: "category" and "definition".
  category   : one of the three values above (exact string)
  definition : 1–2 sentence translator note explaining the term in context.
               For general_software a single short sentence is fine.
               For hvac and openstudio terms explain what the component/concept is.
               If an IDD definition or SDK description is provided, use it as the basis
               for your definition but rewrite it to be translator-friendly (no jargon, clear).

Examples:
  Input  → UI Role: application menu bar item | Qt Context: MainMenu | String: "Cancel"
  Output → {"category": "general_software", "definition": "Standard button to dismiss a dialog without saving."}

  Input  → UI Role: column header in SpacesSubsurfaces grid | Qt Context: SpacesSubsurfacesGridController
           IDD definition: WindowProperty:FrameAndDivider object defines frame/divider thermal properties.
           String: "Frame and Divider"
  Output → {"category": "hvac_terminology", "definition": "Window frame and divider assembly that affects thermal and solar performance of the fenestration."}

  Input  → UI Role: drag-and-drop target area (VRFSystemDropZone) | Qt Context: VRFSystemDropZoneView
           String: "Drop VRF System"
  Output → {"category": "openstudio_specific", "definition": "Label on a drag-and-drop zone; the user drags a Variable Refrigerant Flow (VRF) system from the library and drops it here to add it to the model."}

  Input  → UI Role: field label in the Inspector panel (BuildingInspectorView)
           SDK description: Returns the standards building type for energy code compliance.
           String: "Standards Building Type:"
  Output → {"category": "openstudio_specific", "definition": "Building use category (e.g. LargeOffice, SmallHotel) used by the openstudio-standards gem for energy code compliance checking."}
"""


# ---------------------------------------------------------------------------
# Build user message
# ---------------------------------------------------------------------------

def build_user_message(
    source: str,
    ctx_name: str,
    idd_defs: dict,
    sdk_defs: dict,
) -> str:
    """Construct the user-turn message for Claude, layering all available context."""
    role = get_context_role(ctx_name)
    short_ctx = ctx_name.replace("openstudio::", "")

    lines = [f"UI Role: {role}"]
    lines.append(f"Qt Context: {short_ctx}")

    # IDD cross-reference: check exact match and colon-stripped match
    idd_entry = idd_defs.get(source) or idd_defs.get(source.rstrip(": "))
    if idd_entry and idd_entry.get("definition"):
        obj = idd_entry.get("object", "")
        defn = idd_entry["definition"][:250]
        context_note = f"IDD field in {obj}: {defn}" if obj else f"IDD field: {defn}"
        lines.append(f"IDD definition: {context_note}")

    # SDK cross-reference: check exact match and colon-stripped match
    sdk_entry = sdk_defs.get(source) or sdk_defs.get(source.rstrip(": "))
    if sdk_entry and sdk_entry.get("description"):
        desc = sdk_entry["description"][:250]
        cls = sdk_entry.get("class", "")
        context_note = f"OpenStudio SDK ({cls}): {desc}" if cls else f"OpenStudio SDK: {desc}"
        lines.append(f"SDK description: {context_note}")

    lines.append(f'\nString: {json.dumps(source, ensure_ascii=False)}')
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Batch requests
# ---------------------------------------------------------------------------

def build_requests(
    strings: list[tuple[str, str]],
    idd_defs: dict,
    sdk_defs: dict,
    model: str,
) -> list[Request]:
    requests_list: list[Request] = []
    for i, (ctx_name, source) in enumerate(strings):
        user_msg = build_user_message(source, ctx_name, idd_defs, sdk_defs)
        requests_list.append(
            Request(
                custom_id=f"gui-{i}",
                params=MessageCreateParamsNonStreaming(
                    model=model,
                    max_tokens=256,
                    system=SYSTEM_PROMPT,
                    messages=[{"role": "user", "content": user_msg}],
                ),
            )
        )
    return requests_list


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Build gui_string_definitions.json by classifying OpenStudio GUI strings."
    )
    parser.add_argument("--model", default=DEFAULT_MODEL, help=f"Claude model (default: {DEFAULT_MODEL})")
    parser.add_argument("--ts-file", default=DEFAULT_TS_FILE, help="Source .ts file to extract strings from")
    args = parser.parse_args()

    # API key
    key_text = open(KEY_FILE, encoding="utf-8").read()
    key_match = re.search(r"sk-ant-[A-Za-z0-9_\-]+", key_text)
    if not key_match:
        sys.exit("ERROR: No API key found in key file.")
    client = anthropic.Anthropic(api_key=key_match.group(0))

    # Load cross-reference sources
    idd_defs = load_json_optional("idd_field_definitions.json")
    sdk_defs = load_json_optional("sdk_doc_definitions.json")
    print(f"Loaded {len(idd_defs)} IDD field definitions")
    print(f"Loaded {len(sdk_defs)} SDK doc definitions")
    if not sdk_defs:
        print("  (run scrape_sdk_docs.py to add OpenStudio SDK context for Standards* fields)")

    # Count how many strings will get cross-reference context
    print(f"\nExtracting strings from {args.ts_file} ...")
    strings = extract_unique_strings(args.ts_file)
    print(f"Found {len(strings)} unique strings across non-IDD/non-OutputVariables contexts")

    idd_hits = sum(
        1 for _, s in strings
        if idd_defs.get(s) or idd_defs.get(s.rstrip(": "))
    )
    sdk_hits = sum(
        1 for _, s in strings
        if sdk_defs.get(s) or sdk_defs.get(s.rstrip(": "))
    )
    print(f"  {idd_hits} strings have IDD definitions")
    print(f"  {sdk_hits} strings have SDK definitions")
    print(f"  {len(strings) - idd_hits - sdk_hits} strings rely on UI role + Claude only")

    requests_list = build_requests(strings, idd_defs, sdk_defs, args.model)
    batch = client.messages.batches.create(requests=requests_list)
    print(f"\nBatch submitted: {batch.id}  ({len(requests_list)} requests, model={args.model})")

    # Poll
    while True:
        time.sleep(15)
        batch = client.messages.batches.retrieve(batch.id)
        c = batch.request_counts
        print(f"  {batch.processing_status}: succeeded={c.succeeded} errored={c.errored} processing={c.processing}")
        if batch.processing_status == "ended":
            break

    # Collect raw results
    id_to_raw: dict[str, str] = {}
    for result in client.messages.batches.results(batch.id):
        if result.result.type == "succeeded":
            text = next(
                (b.text for b in result.result.message.content if b.type == "text"), ""
            ).strip()
            id_to_raw[result.custom_id] = text
        else:
            print(f"  WARN: {result.custom_id} → {result.result.type}")

    # Parse JSON from each result
    definitions: dict[str, dict] = {}
    errors = 0
    for i, (ctx_name, source) in enumerate(strings):
        raw = id_to_raw.get(f"gui-{i}", "")
        if not raw:
            errors += 1
            continue
        json_match = re.search(r"\{[^{}]+\}", raw, re.DOTALL)
        if not json_match:
            errors += 1
            print(f"  WARN: no JSON for: {repr(source[:70])}")
            continue
        try:
            parsed = json.loads(json_match.group(0))
        except json.JSONDecodeError:
            errors += 1
            print(f"  WARN: JSON decode error for: {repr(source[:70])}")
            continue
        cat = parsed.get("category", "general_software")
        if cat not in ("general_software", "hvac_terminology", "openstudio_specific"):
            cat = "general_software"
        defn = parsed.get("definition", "")
        definitions[source] = {"category": cat, "definition": defn}

    # Summary
    from collections import Counter
    cats = Counter(v["category"] for v in definitions.values())
    print(f"\n{len(definitions)} definitions built, {errors} parse errors")
    for cat, count in sorted(cats.items()):
        pct = 100 * count // max(len(definitions), 1)
        print(f"  {cat:25s}: {count:4d}  ({pct}%)")

    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        json.dump(definitions, f, ensure_ascii=False, indent=2)
    print(f"\nWrote {OUTPUT_FILE}")
    print("Next step: python retranslate_gui_strings.py")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""
Scrapes OpenStudio model SDK documentation pages (Doxygen HTML) and produces
sdk_doc_definitions.json: {gui_label: {"description": "...", "class": "...", "method": "..."}}

The GUI labels are derived from method names via camelCase → "Title Case" conversion.

Usage:
    python scrape_sdk_docs.py
    python scrape_sdk_docs.py --version 3.10.0
    python scrape_sdk_docs.py --dry-run        # show what would be scraped without writing

Re-run when:
  - A new OpenStudio major version is published to the SDK docs S3 bucket
  - New Standards* properties are added to model classes

Output is committed to the repo (like idd_field_definitions.json) so the build
script can run offline.
"""

import argparse
import json
import re
import sys
import time
import requests
from bs4 import BeautifulSoup

SDK_DOCS_BASE = "https://openstudio-sdk-documentation.s3.amazonaws.com/cpp/OpenStudio-{version}-doc/model/html/{page}"
DEFAULT_VERSION = "3.10.0"
OUTPUT_FILE = "sdk_doc_definitions.json"
DELAY = 0.5  # seconds between requests

# ---------------------------------------------------------------------------
# Curated list of model class pages and the specific method name prefixes
# we care about. Empty means "take all methods with descriptions".
# ---------------------------------------------------------------------------
PAGES = [
    {
        "page":    "classopenstudio_1_1model_1_1_building.html",
        "class":   "Building",
        "methods": [
            "northAxis", "nominalFloortoFloorHeight", "nominalFloortoCeilingHeight",
            "relocatable", "spaceType", "defaultConstructionSet", "defaultScheduleSet",
            "standardsTemplate", "standardsBuildingType",
            "standardsNumberOfStories", "standardsNumberOfAboveGroundStories",
            "standardsNumberOfLivingUnits",
        ],
    },
    {
        "page":    "classopenstudio_1_1model_1_1_space_type.html",
        "class":   "SpaceType",
        "methods": [
            "standardsTemplate", "standardsBuildingType", "standardsSpaceType",
            "designSpecificationOutdoorAir",
            "peoplePerFloorArea", "lightingPowerPerFloorArea",
            "electricEquipmentPowerPerFloorArea", "gasEquipmentPowerPerFloorArea",
        ],
    },
    {
        "page":    "classopenstudio_1_1model_1_1_standards_information_construction.html",
        "class":   "StandardsInformationConstruction",
        "methods": [
            "standardsConstructionType", "intendedSurfaceType", "fenestrationFrameType",
            "constructionStandard", "constructionStandardSource",
        ],
    },
    {
        "page":    "classopenstudio_1_1model_1_1_standards_information_material.html",
        "class":   "StandardsInformationMaterial",
        "methods": [],  # take all
    },
    {
        "page":    "classopenstudio_1_1model_1_1_space.html",
        "class":   "Space",
        "methods": [
            "defaultConstructionSet", "defaultScheduleSet", "spaceType",
            "thermalZone", "buildingStory", "partofTotalFloorArea",
        ],
    },
]

# ---------------------------------------------------------------------------
# Hard-coded overrides for GUI labels that don't derive cleanly from
# the method name alone. Key = GUI string (exact, without trailing colon).
# ---------------------------------------------------------------------------
MANUAL_DEFINITIONS = {
    "Standards Template": {
        "description": (
            "An energy code standard identifier from the openstudio-standards gem, "
            "e.g. 'ASHRAE 90.1-2019' or 'DOE Ref Pre-1980'. "
            "Sets the applicable standard for automated lighting, equipment, and HVAC sizing."
        ),
        "class": "Building / SpaceType",
        "method": "standardsTemplate",
    },
    "Standards Building Type": {
        "description": (
            "Building use category for energy code compliance, drawn from the openstudio-standards "
            "database, e.g. 'LargeOffice', 'SmallHotel', 'PrimarySchool'. "
            "Must match a value recognised by the selected Standards Template."
        ),
        "class": "Building / SpaceType",
        "method": "standardsBuildingType",
    },
    "Standards Space Type": {
        "description": (
            "Space occupancy category for energy code compliance, e.g. 'Office - Open Plan', "
            "'Corridor/Transition', 'Classroom'. "
            "Used by openstudio-standards to assign lighting power densities and schedules."
        ),
        "class": "SpaceType",
        "method": "standardsSpaceType",
    },
    "Standards Number of Stories": {
        "description": (
            "Total number of above- and below-grade building stories. "
            "Used by openstudio-standards for code-compliance checks."
        ),
        "class": "Building",
        "method": "standardsNumberOfStories",
    },
    "Standards Number of Above Ground Stories": {
        "description": (
            "Number of stories above grade level. "
            "Used by openstudio-standards for code-compliance checks."
        ),
        "class": "Building",
        "method": "standardsNumberOfAboveGroundStories",
    },
    "Standards Number of Living Units": {
        "description": (
            "Number of residential dwelling units in the building. "
            "Used by openstudio-standards for residential code-compliance checks."
        ),
        "class": "Building",
        "method": "standardsNumberOfLivingUnits",
    },
    "Standard": {
        "description": (
            "The energy or building standard (e.g., ASHRAE 90.1, Title 24, ISO 6946) "
            "that specifies this construction assembly's thermal properties."
        ),
        "class": "StandardsInformationConstruction / StandardsInformationMaterial",
        "method": "constructionStandard",
    },
    "Standard Source": {
        "description": (
            "The table, section, or reference within the selected Standard that specifies "
            "this construction or material (e.g., 'Table A2.3', 'Section 5.5.3')."
        ),
        "class": "StandardsInformationConstruction / StandardsInformationMaterial",
        "method": "constructionStandardSource",
    },
    "Standards Construction Type": {
        "description": (
            "Freeform string identifying the construction type per the selected standard, "
            "e.g. 'Mass', 'Metal Building', 'SteelFramed'. "
            "Must match the enumeration used by openstudio-standards."
        ),
        "class": "StandardsInformationConstruction",
        "method": "standardsConstructionType",
    },
}

# ---------------------------------------------------------------------------
# camelCase → GUI label conversion
# ---------------------------------------------------------------------------

# Connective words that are lowercase within camelCase (e.g., "floorTo" → "Floor to")
_CONNECTIVES = {"of", "to", "per", "at", "in", "for", "by", "from", "and", "with"}


def camel_to_gui_label(method_name: str) -> str:
    """Convert a camelCase method name to a GUI-style label.

    Examples:
        standardsBuildingType        → Standards Building Type
        nominalFloortoFloorHeight    → Nominal Floor to Floor Height
        standardsNumberOfStories     → Standards Number of Stories
        northAxis                    → North Axis
        defaultConstructionSet       → Default Construction Set
    """
    # Strip getter/setter prefixes
    name = re.sub(r"^(get|set|reset|is|has|populate|suggested|edit|check)", "", method_name)
    if not name:
        return ""
    # Lowercase the first character before splitting (it may have been uppercase)
    name = name[0].lower() + name[1:]

    # Insert space before each uppercase letter preceded by a lowercase letter
    spaced = re.sub(r"([a-z])([A-Z])", r"\1 \2", name)
    # Insert space before a run of uppercase letters followed by a lowercase letter
    spaced = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1 \2", spaced)

    # Split into tokens and look for embedded connective words (e.g. "floorto" → "floor to")
    tokens: list[str] = []
    for token in spaced.split():
        split_done = False
        token_lower = token.lower()
        # Check whether this token ends with a known connective (min 3-char prefix to avoid "into")
        for conn in sorted(_CONNECTIVES, key=len, reverse=True):
            if token_lower.endswith(conn) and len(token) > len(conn) + 2:
                prefix = token[: -len(conn)]
                tokens.append(prefix)
                tokens.append(conn)
                split_done = True
                break
        if not split_done:
            tokens.append(token)

    # Build the final label: capitalize first token, lowercase connectives elsewhere
    result: list[str] = []
    for i, tok in enumerate(tokens):
        if i == 0:
            result.append(tok.capitalize())
        elif tok.lower() in _CONNECTIVES:
            result.append(tok.lower())
        else:
            result.append(tok.capitalize())

    return " ".join(result)


# ---------------------------------------------------------------------------
# Doxygen HTML parsing
# ---------------------------------------------------------------------------

def scrape_page(url: str, class_name: str, wanted_methods: list[str]) -> dict[str, dict]:
    """Scrape method descriptions from a Doxygen-generated SDK class page.

    Returns {method_name: {"description": str, "class": str}}.
    Only methods in wanted_methods are included (all methods if list is empty).
    """
    resp = requests.get(url, timeout=30)
    resp.raise_for_status()
    soup = BeautifulSoup(resp.text, "html.parser")

    results: dict[str, dict] = {}

    # Doxygen summary table: rows with class "memitem:ANCHOR" hold the method signature;
    # the immediately following row with class "memdesc:ANCHOR" holds the short description.
    for row in soup.find_all("tr"):
        row_classes = " ".join(row.get("class", []))
        if not row_classes.startswith("memitem:"):
            continue

        # Extract method name from the right cell (bold element or first anchor in name cell)
        right_cell = row.find("td", class_="memItemRight")
        if not right_cell:
            continue
        method_name: str | None = None
        for candidate in right_cell.find_all(["b", "a"]):
            text = candidate.get_text(strip=True)
            # A valid method name: starts lowercase, no spaces, no parens
            if text and re.match(r"^[a-z][a-zA-Z0-9]+$", text):
                method_name = text
                break
        if not method_name:
            continue
        if wanted_methods and method_name not in wanted_methods:
            continue
        if method_name in results:
            continue  # first occurrence wins

        # Find the matching memdesc row
        desc_row = row.find_next_sibling("tr")
        if not desc_row:
            continue
        desc_classes = " ".join(desc_row.get("class", []))
        if not desc_classes.startswith("memdesc:"):
            continue
        desc_cell = desc_row.find("td", class_="mdescRight")
        if not desc_cell:
            continue
        description = desc_cell.get_text(" ", strip=True)
        # Skip generic "More..." links and empty descriptions
        if not description or description.lower() in ("more...", ""):
            continue

        results[method_name] = {
            "description": description.rstrip("."),
            "class": class_name,
        }

    return results


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Scrape OpenStudio SDK model docs → sdk_doc_definitions.json"
    )
    parser.add_argument(
        "--version", default=DEFAULT_VERSION,
        help=f"SDK docs version to scrape (default: {DEFAULT_VERSION}). "
             "Check https://openstudio-sdk-documentation.s3.amazonaws.com/index.html for available versions.",
    )
    parser.add_argument(
        "--dry-run", action="store_true",
        help="Print what would be scraped without writing the output file.",
    )
    args = parser.parse_args()

    all_methods: dict[str, dict] = {}

    for page_spec in PAGES:
        url = SDK_DOCS_BASE.format(version=args.version, page=page_spec["page"])
        class_name = page_spec["class"]
        wanted = page_spec["methods"]
        print(f"Scraping {class_name} ... ({url})")
        try:
            scraped = scrape_page(url, class_name, wanted)
        except requests.HTTPError as exc:
            print(f"  WARNING: {exc} — skipping")
            continue
        except Exception as exc:
            print(f"  ERROR: {exc} — skipping")
            continue
        print(f"  {len(scraped)} methods with descriptions")
        all_methods.update(scraped)
        time.sleep(DELAY)

    # Convert method names → GUI labels
    definitions: dict[str, dict] = {}

    for method_name, info in all_methods.items():
        gui_label = camel_to_gui_label(method_name)
        if not gui_label:
            continue
        definitions[gui_label] = {
            "description": info["description"],
            "class": info["class"],
            "method": method_name,
        }

    # Merge manual definitions (they take precedence, richer descriptions)
    for gui_label, defn in MANUAL_DEFINITIONS.items():
        definitions[gui_label] = defn

    print(f"\n{len(definitions)} total SDK label definitions")
    for label, defn in sorted(definitions.items()):
        print(f"  {label!r:55s}  [{defn['class']}]")

    if args.dry_run:
        print("\n--dry-run: not writing output file.")
        return

    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        json.dump(definitions, f, ensure_ascii=False, indent=2)
    print(f"\nWrote {OUTPUT_FILE}")
    print("Next step: python build_gui_definitions.py")


if __name__ == "__main__":
    main()

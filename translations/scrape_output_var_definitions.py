#!/usr/bin/env python3
"""
Scrapes EnergyPlus Output Variable definitions from the BigLadder I/O Reference.
Produces output_var_definitions.json: {var_name: {definition, units, page}}

Usage:
    python scrape_output_var_definitions.py
"""

import json
import time
import re
import sys
import requests
from bs4 import BeautifulSoup

BASE_URL = "https://bigladdersoftware.com/epx/docs/25-1/input-output-reference/"
OUTPUT_FILE = "output_var_definitions.json"
DELAY = 0.5  # seconds between requests — be polite to the server

PAGES = [
    "group-simulation-parameters.html",
    "group-compliance-objects.html",
    "group-location-climate-weather-file-access.html",
    "group-schedules.html",
    "group-surface-construction-elements.html",
    "group-thermal-zone-description-geometry.html",
    "group-advanced-surface-concepts.html",
    "group-detailed-ground-heat-transfer.html",
    "group-room-air-models.html",
    "group-internal-gains-people-lights-other.html",
    "group-daylighting.html",
    "group-exterior-energy-use-equipment.html",
    "group-airflow.html",
    "group-design-objects.html",
    "group-node-branch-management.html",
    "group-plant-condenser-loops.html",
    "group-plant-condenser-control.html",
    "group-plant-equipment.html",
    "group-water-heaters.html",
    "group-condenser-equipment.html",
    "group-air-distribution.html",
    "group-airflow-network.html",
    "group-zone-equipment.html",
    "group-air-distribution-equipment.html",
    "group-zone-forced-air-units.html",
    "group-unitary-equipment.html",
    "group-variable-refrigerant-flow-equipment.html",
    "group-radiative-convective-units.html",
    "group-refrigeration.html",
    "group-zone-controls-thermostats.html",
    "group-air-path.html",
    "group-non-zone-equipment.html",
    "group-solar-collectors.html",
    "group-pumps.html",
    "group-plant-condenser-flow-control.html",
    "group-heating-and-cooling-coils.html",
    "group-coil-cooling-dx.html",
    "group-fans.html",
    "group-humidifiers-and-dehumidifiers.html",
    "group-energy-management-system-ems.html",
    "group-python-plugins.html",
    "group-externalinterface.html",
    "group-user-defined-hvac-and-plant-component.html",
    "group-system-availability-managers.html",
    "group-setpoint-managers.html",
    "group-controllers.html",
    "group-evaporative-coolers.html",
    "group-heat-recovery.html",
    "group-demand-limiting-controls.html",
    "group-electric-load-center-generator.html",
    "group-water-systems.html",
    "group-operational-faults.html",
]


def scrape_page(page_name: str) -> dict:
    url = BASE_URL + page_name
    resp = requests.get(url, timeout=30)
    resp.raise_for_status()
    soup = BeautifulSoup(resp.text, "html.parser")

    definitions = {}
    # Output variable headings appear as h3, h4, or h5 depending on the page.
    # Process in document order across all levels; first occurrence of a name wins.
    for heading in soup.find_all(["h3", "h4", "h5"]):
        # Strip anchor link so we get only the heading text
        for a in heading.find_all("a"):
            a.decompose()
        text = heading.get_text(strip=True)

        # Output variable headings end with [unit] or [] for dimensionless, e.g. "Zone Mean Air Temperature [C]"
        m = re.match(r'^(.*?)\s*\[([^\]]*)\]\s*$', text)
        if not m:
            continue

        var_name = m.group(1).strip()
        units = m.group(2).strip()
        if not var_name:
            continue

        # Already seen this variable name on this page — skip
        if var_name in definitions:
            continue

        # Definition is the first paragraph immediately following the heading.
        # Skip NavigableString (whitespace) nodes — their .name is None.
        definition = ""
        for sibling in heading.next_siblings:
            if getattr(sibling, "name", None) is None:
                continue
            if sibling.name == "p":
                definition = sibling.get_text(" ", strip=True)
            break  # stop at first real element regardless of tag

        definitions[var_name] = {
            "definition": definition,
            "units": units,
            "page": page_name,
        }

    return definitions


def main():
    all_definitions: dict = {}
    total = len(PAGES)

    for i, page in enumerate(PAGES, 1):
        print(f"[{i:2d}/{total}] {page} ... ", end="", flush=True)
        try:
            defs = scrape_page(page)
            # Don't overwrite — first occurrence of a variable name wins
            new_entries = {k: v for k, v in defs.items() if k not in all_definitions}
            all_definitions.update(new_entries)
            print(f"{len(defs)} variables ({len(new_entries)} new)")
        except Exception as exc:
            print(f"ERROR: {exc}", file=sys.stderr)
        if i < total:
            time.sleep(DELAY)

    print(f"\nTotal unique definitions: {len(all_definitions)}")
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        json.dump(all_definitions, f, ensure_ascii=False, indent=2)
    print(f"Saved -> {OUTPUT_FILE}")


if __name__ == "__main__":
    main()

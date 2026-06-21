#!/usr/bin/env python3
"""
Scrapes EnergyPlus I/O Reference input field definitions for IDD objects.
Reads the page list from IddObjectDocUrl.hpp and fetches all pages.
Produces idd_field_definitions.json: {field_name: {definition, object, page}}

Usage:
    python scrape_idd_field_definitions.py
"""

import json
import os
import re
import subprocess
import sys
import time
import requests
from bs4 import BeautifulSoup

BASE_URL = "https://bigladdersoftware.com/epx/docs/25-1/input-output-reference/"
# IddObjectDocUrl.hpp lives on feat/doc-links-inspector; fall back to git show if absent.
URL_MAP_FILE = "../src/model_editor/IddObjectDocUrl.hpp"
URL_MAP_BRANCH = "feat/doc-links-inspector"
OUTPUT_FILE = "idd_field_definitions.json"
DELAY = 0.5  # seconds between requests


def parse_url_map() -> list[str]:
    """Return sorted list of unique page filenames from IddObjectDocUrl.hpp.

    Reads the file from disk if present, otherwise fetches it from git.
    """
    if os.path.exists(URL_MAP_FILE):
        with open(URL_MAP_FILE, encoding="utf-8") as f:
            text = f.read()
    else:
        result = subprocess.run(
            ["git", "show", f"{URL_MAP_BRANCH}:{URL_MAP_FILE}"],
            capture_output=True, text=True, encoding="utf-8"
        )
        if result.returncode != 0:
            sys.exit(f"Cannot read {URL_MAP_FILE}: {result.stderr.strip()}")
        text = result.stdout
    urls = re.findall(r'"((?:group|input|output|lifecycle)[^"]+\.html[^"]*)"', text)
    pages = sorted({u.split("#")[0] for u in urls})
    return pages


# Section subheadings that are NOT object names — skip these when looking for the parent object.
_SECTION_HEADINGS = {"Inputs", "Outputs", "Input", "Output", "Notes", "References",
                     "Example", "Examples", "Overview", "Description", "Summary"}


def current_object_name(soup_element) -> str:
    """Walk backwards from element to find the nearest h2 object heading.

    Skips h3 section subheadings like 'Inputs' and 'Outputs'.
    """
    for prev in soup_element.find_all_previous(["h2", "h3"]):
        for a in prev.find_all("a"):
            a.decompose()
        text = prev.get_text(strip=True)
        if not text or len(text) <= 2:
            continue
        if prev.name == "h3" and text in _SECTION_HEADINGS:
            continue
        return text
    return ""


def scrape_page(page_name: str) -> dict:
    """Scrape all field definitions from a BigLadder I/O Reference page.

    Returns {field_name: {definition, object, page}} — first occurrence wins.
    """
    url = BASE_URL + page_name
    resp = requests.get(url, timeout=30)
    resp.raise_for_status()
    soup = BeautifulSoup(resp.text, "html.parser")

    definitions = {}

    # Field headings have id="field-..." (h4 is standard; some pages use h3/h5)
    for heading in soup.find_all(["h3", "h4", "h5"]):
        hid = heading.get("id", "")
        if not hid.startswith("field-"):
            continue

        # Strip the anchor link to get clean text, e.g. "Field: Minimum Outdoor Air Flow Rate"
        for a in heading.find_all("a"):
            a.decompose()
        text = heading.get_text(strip=True)

        # Remove leading "Field: " prefix (case-insensitive, optional colon/space variants)
        field_name = re.sub(r"^field[:\s]+", "", text, flags=re.IGNORECASE).strip()
        if not field_name:
            continue

        # First occurrence of this field name wins across all pages
        if field_name in definitions:
            continue

        # Definition = all content between this heading and the next heading.
        # Collect <p>, <ul>/<ol>, <dl> text; stop at the next h2–h5.
        parts = []
        for sibling in heading.next_siblings:
            tag = getattr(sibling, "name", None)
            if tag is None:
                continue
            if tag in ("h2", "h3", "h4", "h5"):
                break
            if tag == "p":
                t = sibling.get_text(" ", strip=True)
                if t:
                    parts.append(t)
            elif tag in ("ul", "ol"):
                items = [li.get_text(" ", strip=True) for li in sibling.find_all("li")]
                if items:
                    parts.append(", ".join(items))
            elif tag == "dl":
                terms = [dt.get_text(" ", strip=True) for dt in sibling.find_all("dt")]
                if terms:
                    parts.append(", ".join(terms))
        definition = " ".join(parts)

        # Determine which EnergyPlus object this field belongs to
        obj_name = current_object_name(heading)

        definitions[field_name] = {
            "definition": definition,
            "object": obj_name,
            "page": page_name,
        }

    return definitions


def main():
    pages = parse_url_map()
    total = len(pages)
    print(f"Pages to scrape: {total}")

    all_definitions: dict = {}

    for i, page in enumerate(pages, 1):
        print(f"[{i:2d}/{total}] {page} ... ", end="", flush=True)
        try:
            defs = scrape_page(page)
            new = {k: v for k, v in defs.items() if k not in all_definitions}
            all_definitions.update(new)
            print(f"{len(defs)} fields ({len(new)} new)")
        except Exception as exc:
            print(f"ERROR: {exc}", file=sys.stderr)
        if i < total:
            time.sleep(DELAY)

    print(f"\nTotal unique field definitions: {len(all_definitions)}")
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        json.dump(all_definitions, f, ensure_ascii=False, indent=2)
    print(f"Saved -> {OUTPUT_FILE}")


if __name__ == "__main__":
    main()

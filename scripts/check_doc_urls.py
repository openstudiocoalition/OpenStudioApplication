#!/usr/bin/env python3
"""
check_doc_urls.py - Verify BigLadder EnergyPlus I/O Reference URLs in OpenStudioApp source.

Scans source files for doc URL strings, fetches each unique page once, checks that
every anchor referenced actually exists in the page HTML, and reports failures.

Usage:
    python scripts/check_doc_urls.py [--repo-root PATH] [--delay SEC] [--openstudio-hxx PATH]

The BigLadder doc version (eg "25-2") is auto-detected from the OpenStudio SDK's
OpenStudio.hxx (energyPlusVersionMajor()/energyPlusVersionMinor()), the same values
openstudio::bigladdersoftwareDocBaseUrl() uses at runtime. The SDK is located by
searching for OpenStudio-*/*/include/openstudio/OpenStudio.hxx under --repo-root
(covers both in-source and build/ out-of-source SDK downloads), or pass
--openstudio-hxx to point at it directly.

Exit codes:
    0  All URLs valid
    1  One or more broken/missing anchors found
    2  Usage / dependency error

Why a Python script rather than a GTest network test
-----------------------------------------------------
BigLadder returns HTTP 200 for *any* URL on an existing page, regardless of whether
the anchor exists. A plain HTTP HEAD or GET check would silently pass even when an
anchor has been renamed or removed. Verifying anchor IDs requires fetching the full
page HTML and scanning for id="..." attributes — straightforward in Python with
html.parser, but awkward in C++/Qt without a full HTML parser dependency.

GTest network tests were also considered but ruled out because:
  - They are slow and flaky in CI (network dependency).
  - QNetworkAccessManager requires a running event loop and async handling.
  - GTest provides no natural mechanism to fetch-and-parse HTML for anchor checks.

This script runs standalone (no build step), can be invoked as a pre-commit hook or
CI job, and completes in roughly one second per unique page fetched.
"""

import argparse
import re
import sys
import time
from collections import defaultdict
from html.parser import HTMLParser
from pathlib import Path
from urllib.request import urlopen
from urllib.error import URLError

# ---------------------------------------------------------------------------
# Files to scan and the regex patterns that extract URL fragments from them
# ---------------------------------------------------------------------------

# Matches values in the IddObjectDocUrl.hpp urlMap and groupMap:
#   {"OS:Something",  "group-foo.html#anchor"},
#   {"OpenStudio Group Name",  "group-foo.html"},
IDDOBJECTDOCURL_PATTERN = re.compile(
    r'"(?:OS:|OpenStudio |Solar |Electric |Energy |User |Python |Airflow)[^"]*"\s*,\s*"([^"]+\.html(?:#[^"]*)?)"'
)

# Matches QString URL constructions in .cpp files:
#   QString::fromStdString(openstudio::bigladdersoftwareDocBaseUrl()) + "page.html#anchor"
#   or the old QStringLiteral(BASE "page.html#anchor") form
CPP_URL_FRAGMENT_PATTERN = re.compile(
    r'(?:bigladdersoftwareDocBaseUrl\(\)\s*\+\s*"([^"]+\.html(?:#[^"]*)?)"'
    r'|QStringLiteral\(BIGLADDERSOFTWARE_DOC_BASE_URL\s*"([^"]+\.html(?:#[^"]*)?)"'
    r'|\+\s*"(group-[^"]+\.html(?:#[^"]*)?)"'
    r'|\+\s*"(lifecyclecost-[^"]+\.html(?:#[^"]*)?)")'
)

SOURCE_FILES = [
    "src/model_editor/IddObjectDocUrl.hpp",
    "src/openstudio_lib/SimSettingsView.cpp",
    "src/openstudio_lib/LoadsView.cpp",
    "src/openstudio_lib/ConstructionsView.cpp",
    "src/openstudio_lib/MaterialsView.cpp",
    "src/openstudio_lib/ScheduleOthersView.cpp",
    "src/openstudio_lib/LocationTabView.cpp",
    "src/openstudio_lib/YearSettingsWidget.cpp",
    "src/openstudio_lib/GroundTemperatureView.cpp",
    "src/openstudio_lib/LifeCycleCostsTabView.cpp",
    "src/openstudio_lib/BuildingInspectorView.cpp",
]

# Where to look for the downloaded OpenStudio SDK's OpenStudio.hxx, relative to repo root.
SDK_HXX_GLOBS = [
    "OpenStudio-*/*/include/openstudio/OpenStudio.hxx",
    "build*/OpenStudio-*/*/include/openstudio/OpenStudio.hxx",
]

ENERGYPLUS_VERSION_RE = {
    "major": re.compile(r"energyPlusVersionMajor\(\)\s*\{\s*return\s*(\d+);"),
    "minor": re.compile(r"energyPlusVersionMinor\(\)\s*\{\s*return\s*(\d+);"),
}


# ---------------------------------------------------------------------------
# HTML parser that collects all id= attributes
# ---------------------------------------------------------------------------

class AnchorCollector(HTMLParser):
    def __init__(self):
        super().__init__()
        self.ids = set()

    def handle_starttag(self, tag, attrs):
        for name, value in attrs:
            if name == "id" and value:
                self.ids.add(value)


# ---------------------------------------------------------------------------
# EnergyPlus version detection (mirrors openstudio::bigladdersoftwareDocBaseUrl())
# ---------------------------------------------------------------------------

def find_openstudio_hxx(repo_root: Path) -> Path | None:
    for pattern in SDK_HXX_GLOBS:
        matches = sorted(repo_root.glob(pattern))
        if matches:
            return matches[0]
    return None


def detect_bigladdersoftware_base(hxx_path: Path) -> str:
    """
    Parse energyPlusVersionMajor()/energyPlusVersionMinor() from the SDK's
    OpenStudio.hxx and build the BigLadder I/O Reference base URL the same way
    openstudio::bigladdersoftwareDocBaseUrl() does at runtime.
    """
    text = hxx_path.read_text(encoding="utf-8")
    versions = {}
    for part, pattern in ENERGYPLUS_VERSION_RE.items():
        m = pattern.search(text)
        if not m:
            raise ValueError(f"Could not find energyPlusVersion{part.capitalize()}() in {hxx_path}")
        versions[part] = m.group(1)

    return f"https://bigladdersoftware.com/epx/docs/{versions['major']}-{versions['minor']}/input-output-reference/"


# ---------------------------------------------------------------------------
# URL extraction
# ---------------------------------------------------------------------------

def extract_fragments(repo_root: Path, base_url: str):
    """
    Returns a dict: page_url -> list of (anchor_or_None, source_file, line_no)
    """
    results = defaultdict(list)

    for rel_path in SOURCE_FILES:
        src = repo_root / rel_path
        if not src.exists():
            print(f"  WARNING: {rel_path} not found, skipping", file=sys.stderr)
            continue

        text = src.read_text(encoding="utf-8")

        if rel_path.endswith("IddObjectDocUrl.hpp"):
            pattern = IDDOBJECTDOCURL_PATTERN
        else:
            pattern = CPP_URL_FRAGMENT_PATTERN

        for lineno, line in enumerate(text.splitlines(), 1):
            for m in pattern.finditer(line):
                fragment = next((g for g in m.groups() if g), None)
                if not fragment:
                    continue
                if "#" in fragment:
                    page_part, anchor = fragment.split("#", 1)
                else:
                    page_part, anchor = fragment, None
                page_url = base_url + page_part
                results[page_url].append((anchor, rel_path, lineno))

    return results


# ---------------------------------------------------------------------------
# Page fetching with simple cache
# ---------------------------------------------------------------------------

def fetch_anchors(url: str, delay: float = 0.5) -> set | None:
    """Fetch a page and return the set of id= values, or None on error."""
    try:
        time.sleep(delay)
        with urlopen(url, timeout=15) as resp:
            html = resp.read().decode("utf-8", errors="replace")
        parser = AnchorCollector()
        parser.feed(html)
        return parser.ids
    except URLError as e:
        print(f"  ERROR fetching {url}: {e}", file=sys.stderr)
        return None


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--repo-root",
        default=".",
        help="Path to the OpenStudioApplication repo root (default: current directory)",
    )
    parser.add_argument(
        "--delay",
        type=float,
        default=0.5,
        help="Seconds to wait between page fetches (default: 0.5)",
    )
    parser.add_argument(
        "--openstudio-hxx",
        help="Path to the OpenStudio SDK's OpenStudio.hxx (default: auto-detect under --repo-root)",
    )
    args = parser.parse_args()

    repo_root = Path(args.repo_root).resolve()
    print(f"Scanning repo: {repo_root}")

    hxx_path = Path(args.openstudio_hxx).resolve() if args.openstudio_hxx else find_openstudio_hxx(repo_root)
    if hxx_path is None or not hxx_path.exists():
        print(
            "ERROR: Could not find the OpenStudio SDK's OpenStudio.hxx "
            f"(looked for {SDK_HXX_GLOBS} under {repo_root}).\n"
            "Build/configure the project first, or pass --openstudio-hxx explicitly.",
            file=sys.stderr,
        )
        sys.exit(2)

    base_url = detect_bigladdersoftware_base(hxx_path)
    print(f"Using EnergyPlus doc base URL: {base_url}\n  (from {hxx_path})")

    fragments = extract_fragments(repo_root, base_url)
    if not fragments:
        print("No URLs found — check SOURCE_FILES list.", file=sys.stderr)
        sys.exit(2)

    print(f"\nFound {sum(len(v) for v in fragments.values())} URL references across {len(fragments)} unique pages.\n")

    failures = []
    page_cache = {}

    for page_url in sorted(fragments):
        print(f"Checking: {page_url}")
        if page_url not in page_cache:
            page_cache[page_url] = fetch_anchors(page_url, delay=args.delay)

        page_ids = page_cache[page_url]

        for anchor, src_file, lineno in fragments[page_url]:
            if page_ids is None:
                failures.append((src_file, lineno, page_url, anchor, "page fetch failed"))
            elif anchor and anchor not in page_ids:
                failures.append((src_file, lineno, page_url, anchor, "anchor not found in page"))
            else:
                status = "OK" if anchor else "OK (no anchor)"
                print(f"  {status}: #{anchor or ''}")

    print()
    if failures:
        print(f"FAILURES ({len(failures)}):")
        for src_file, lineno, page_url, anchor, reason in failures:
            print(f"  {src_file}:{lineno}  #{anchor}  -> {reason}")
            print(f"    {page_url}#{anchor or ''}")
        sys.exit(1)
    else:
        print("All URLs OK.")
        sys.exit(0)


if __name__ == "__main__":
    main()

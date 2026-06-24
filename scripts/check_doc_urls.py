#!/usr/bin/env python3
"""
check_doc_urls.py - Verify the local EnergyPlus Input Output Reference doc links in OpenStudioApp source.

Scans IddObjectDocUrl.hpp for page#anchor references, and checks that every referenced page
exists and every anchor referenced actually has a matching id= in that page's HTML.

Usage:
    python scripts/check_doc_urls.py [--repo-root PATH] [--docs-dir PATH]

The documentation directory is auto-detected by searching for EnergyPlus/doc/input-output-reference
under --repo-root (covers build-tree layouts), or pass --docs-dir to point at it directly
(this is the same directory openstudio::energyPlusDocDirectory() resolves to at runtime).

Exit codes:
    0  All URLs valid
    1  One or more broken/missing pages or anchors found
    2  Usage / dependency error
"""

import argparse
import re
import sys
from collections import defaultdict
from html.parser import HTMLParser
from pathlib import Path

# ---------------------------------------------------------------------------
# Files to scan and the regex pattern that extracts page#anchor fragments from them
# ---------------------------------------------------------------------------

# Matches values in the IddObjectDocUrl.hpp urlMap and groupMap:
#   {"OS:Something",  "1.5-group-foo.html#anchor"},
#   {"OpenStudio Group Name",  "1.5-group-foo.html"},
IDDOBJECTDOCURL_PATTERN = re.compile(
    r'"(?:OS:|OpenStudio |Solar |Electric |Energy |User |Python |Airflow)[^"]*"\s*,\s*"([^"]+\.html(?:#[^"]*)?)"'
)

SOURCE_FILES = [
    "src/model_editor/IddObjectDocUrl.hpp",
]

# Where to look for the extracted documentation, relative to repo root.
DOCS_DIR_GLOBS = [
    "EnergyPlus/doc/input-output-reference",
    "build*/EnergyPlus/doc/input-output-reference",
]


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
# Docs directory detection
# ---------------------------------------------------------------------------

def find_docs_dir(repo_root: Path) -> Path | None:
    for pattern in DOCS_DIR_GLOBS:
        matches = sorted(repo_root.glob(pattern))
        if matches:
            return matches[0]
    return None


# ---------------------------------------------------------------------------
# URL extraction
# ---------------------------------------------------------------------------

def extract_fragments(repo_root: Path):
    """
    Returns a dict: page_name -> list of (anchor_or_None, source_file, line_no)
    """
    results = defaultdict(list)

    for rel_path in SOURCE_FILES:
        src = repo_root / rel_path
        if not src.exists():
            print(f"  WARNING: {rel_path} not found, skipping", file=sys.stderr)
            continue

        text = src.read_text(encoding="utf-8")

        for lineno, line in enumerate(text.splitlines(), 1):
            for m in IDDOBJECTDOCURL_PATTERN.finditer(line):
                fragment = m.group(1)
                if "#" in fragment:
                    page_name, anchor = fragment.split("#", 1)
                else:
                    page_name, anchor = fragment, None
                results[page_name].append((anchor, rel_path, lineno))

    return results


# ---------------------------------------------------------------------------
# Page reading with simple cache
# ---------------------------------------------------------------------------

def read_anchors(path: Path) -> set | None:
    """Read a local HTML file and return the set of id= values, or None on error."""
    try:
        html = path.read_text(encoding="utf-8", errors="replace")
        parser = AnchorCollector()
        parser.feed(html)
        return parser.ids
    except OSError as e:
        print(f"  ERROR reading {path}: {e}", file=sys.stderr)
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
        "--docs-dir",
        help="Path to the extracted EnergyPlus Input Output Reference HTML (default: auto-detect under --repo-root)",
    )
    args = parser.parse_args()

    repo_root = Path(args.repo_root).resolve()
    print(f"Scanning repo: {repo_root}")

    docs_dir = Path(args.docs_dir).resolve() if args.docs_dir else find_docs_dir(repo_root)
    if docs_dir is None or not docs_dir.is_dir():
        print(
            "ERROR: Could not find the EnergyPlus docs directory "
            f"(looked for {DOCS_DIR_GLOBS} under {repo_root}).\n"
            "Configure/build the project first, or pass --docs-dir explicitly.",
            file=sys.stderr,
        )
        sys.exit(2)

    print(f"Using EnergyPlus docs directory: {docs_dir}")

    fragments = extract_fragments(repo_root)
    if not fragments:
        print("No URLs found — check SOURCE_FILES list.", file=sys.stderr)
        sys.exit(2)

    print(f"\nFound {sum(len(v) for v in fragments.values())} URL references across {len(fragments)} unique pages.\n")

    failures = []
    page_cache = {}

    for page_name in sorted(fragments):
        page_path = docs_dir / page_name
        print(f"Checking: {page_path}")
        if page_name not in page_cache:
            if not page_path.exists():
                page_cache[page_name] = None
            else:
                page_cache[page_name] = read_anchors(page_path)

        page_ids = page_cache[page_name]

        for anchor, src_file, lineno in fragments[page_name]:
            if page_ids is None:
                failures.append((src_file, lineno, page_name, anchor, "page not found"))
            elif anchor and anchor not in page_ids:
                failures.append((src_file, lineno, page_name, anchor, "anchor not found in page"))
            else:
                status = "OK" if anchor else "OK (no anchor)"
                print(f"  {status}: #{anchor or ''}")

    print()
    if failures:
        print(f"FAILURES ({len(failures)}):")
        for src_file, lineno, page_name, anchor, reason in failures:
            print(f"  {src_file}:{lineno}  #{anchor}  -> {reason}")
            print(f"    {page_name}#{anchor or ''}")
        sys.exit(1)
    else:
        print("All URLs OK.")
        sys.exit(0)


if __name__ == "__main__":
    main()

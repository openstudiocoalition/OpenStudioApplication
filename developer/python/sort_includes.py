#!/usr/bin/env python3
"""
sort_includes.py
================
Applies the include-order rule from
  .github/instructions/cpp-refactoring.instructions.md
to every .cpp and .hpp file under src/.

Groups (blank line between non-empty groups, alphabetical within group):
  1. Own header    (.cpp only) — #include "ThisClass.hpp" matching the file stem
  2. Same-dir                  — #include "Sibling.hpp"   (quoted, no path separator)
  3. Cross-dir                 — #include "../other/Foo.hpp" (quoted, contains '/')
  4. OpenStudio SDK            — #include <openstudio/...>
  5. Qt / Boost / system       — #include <...>  (all other angle-bracket includes)

Scope: only the contiguous top-level include block is touched.
  - Includes inside #if / #ifdef / #ifndef / #else / #elif / #endif are never moved.
  - Any line that is not '#include ...' or blank terminates the block.

Usage (run from the repo root, i.e. the directory that contains src/):
  python developer/python/sort_includes.py            # rewrite files in-place
  python developer/python/sort_includes.py --dry-run  # report what would change, no writes
  python developer/python/sort_includes.py --check    # exit 1 if any file is out of order (CI mode)
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Classification helpers
# ---------------------------------------------------------------------------

INCLUDE_RE = re.compile(r'^#include\s*([<"])(.*?)([>"])\s*(?://.*)?$')


def classify(line: str, stem: str, is_cpp: bool) -> int:
    """Return group number 1-5, or 0 if not a recognisable #include line."""
    m = INCLUDE_RE.match(line.strip())
    if not m:
        return 0
    bracket, path = m.group(1), m.group(2)

    if bracket == '"':
        # Group 1: own header (.cpp only) — stem of included file matches this file's stem
        if is_cpp and Path(path).stem == stem:
            return 1
        # Group 3: cross-directory — path contains a directory separator
        if '/' in path or '\\' in path:
            return 3
        # Group 2: same-directory
        return 2
    else:  # angle-bracket
        # Group 4: OpenStudio SDK
        if path.startswith('openstudio/'):
            return 4
        # Group 5: Qt, Boost, system, gtest, etc.
        return 5


def sort_key(line: str) -> str:
    """Alphabetical sort key: lowercase basename of the included path."""
    m = INCLUDE_RE.match(line.strip())
    if not m:
        return line.lower()
    return Path(m.group(2)).name.lower()


# ---------------------------------------------------------------------------
# Reorder a flat list of #include lines into the canonical group order
# ---------------------------------------------------------------------------

def reorder_includes(includes: list[str], stem: str, is_cpp: bool) -> list[str]:
    """
    Given a flat list of #include lines (no blank lines between them),
    return them reordered into groups separated by single blank lines.
    """
    groups: dict[int, list[str]] = {1: [], 2: [], 3: [], 4: [], 5: []}
    unclassified: list[str] = []

    for line in includes:
        g = classify(line, stem, is_cpp)
        if g:
            groups[g].append(line)
        else:
            # Should not happen in a well-formed include block; keep as-is in group 5
            unclassified.append(line)

    if unclassified:
        groups[5].extend(unclassified)

    for g in groups:
        groups[g].sort(key=sort_key)

    result: list[str] = []
    first_group = True
    for g in (1, 2, 3, 4, 5):
        if groups[g]:
            if not first_group:
                result.append('')
            result.extend(groups[g])
            first_group = False

    return result


# ---------------------------------------------------------------------------
# File transformation
# ---------------------------------------------------------------------------

def transform(filepath: Path) -> tuple[bool, str]:
    """
    Parse *filepath*, reorder its top-level include block according to the
    five-group rule, and return (changed, new_content).

    Returns (False, original_text) if no change is needed or the file has no
    top-level include block.
    """
    text = filepath.read_text(encoding='utf-8', errors='replace')
    original = text
    lines = text.splitlines()
    n = len(lines)

    stem = filepath.stem
    is_cpp = filepath.suffix == '.cpp'

    # ── Step 1: skip to the first non-blank line ─────────────────────────────
    i = 0
    while i < n and lines[i].strip() == '':
        i += 1

    # ── Step 2: skip the file-top block comment  /* ... */ ───────────────────
    if i < n and lines[i].strip().startswith('/*'):
        while i < n and '*/' not in lines[i]:
            i += 1
        i += 1  # skip the closing '*/' line

    # skip blank lines after the license comment
    while i < n and lines[i].strip() == '':
        i += 1

    # ── Step 3: skip include guards or #pragma once ──────────────────────────
    # Pattern A: #pragma once
    if i < n and lines[i].strip().startswith('#pragma once'):
        i += 1
    # Pattern B: #ifndef FOO_HPP  /  #define FOO_HPP
    elif i < n and lines[i].strip().startswith('#ifndef'):
        if i + 1 < n and lines[i + 1].strip().startswith('#define'):
            i += 2

    # skip blank lines after guard
    while i < n and lines[i].strip() == '':
        i += 1

    include_start = i

    # ── Step 4: collect the contiguous top-level include block ───────────────
    # Rules:
    #   - '#include ...' lines and blank lines between them are included.
    #   - The first line that is neither '#include' nor blank terminates the block.
    #   - Lines starting with '#if', '#ifdef', etc. also terminate the block
    #     (conditional includes are never reordered).
    include_lines_raw: list[str] = []
    pending_blanks = 0
    j = include_start

    while j < n:
        stripped = lines[j].strip()

        if stripped == '':
            pending_blanks += 1
            j += 1
            continue

        if stripped.startswith('#include'):
            # Commit any pending blanks only if we already have some includes
            if include_lines_raw:
                include_lines_raw.extend([''] * pending_blanks)
            pending_blanks = 0
            include_lines_raw.append(lines[j])
            j += 1
            continue

        # Anything else (code, comment, preprocessor conditional) → stop
        break

    include_end = j  # first line *after* the block (trailing blanks not consumed)

    if not include_lines_raw:
        return False, original

    # ── Step 5: reorder ───────────────────────────────────────────────────────
    flat_includes = [ln for ln in include_lines_raw if ln.strip() != '']
    reordered = reorder_includes(flat_includes, stem, is_cpp)

    # ── Step 6: rebuild file ──────────────────────────────────────────────────
    new_lines = lines[:include_start] + reordered + lines[include_end:]
    new_text = '\n'.join(new_lines)
    if text.endswith('\n'):
        new_text += '\n'

    changed = new_text != original
    return changed, new_text


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main() -> int:
    dry_run = '--dry-run' in sys.argv
    check_mode = '--check' in sys.argv  # CI: exit 1 if any file needs changes

    root = Path(__file__).resolve().parent.parent.parent / 'src'
    if not root.is_dir():
        print(f"ERROR: src/ directory not found at {root}", file=sys.stderr)
        return 1

    files = sorted(root.rglob('*.cpp')) + sorted(root.rglob('*.hpp'))

    changed_files: list[Path] = []
    error_files: list[tuple[Path, str]] = []

    for fp in files:
        try:
            changed, new_content = transform(fp)
        except Exception as exc:
            error_files.append((fp, str(exc)))
            continue

        if changed:
            changed_files.append(fp)
            rel = fp.relative_to(root.parent)
            if check_mode or dry_run:
                print(f"{'[CHECK]' if check_mode else '[DRY-RUN]'} needs reorder: {rel}")
            else:
                fp.write_text(new_content, encoding='utf-8')
                print(f"reordered: {rel}")

    for fp, err in error_files:
        print(f"ERROR: {fp.relative_to(root.parent)}: {err}", file=sys.stderr)

    mode_label = 'would change' if (dry_run or check_mode) else 'reordered'
    print(f"\nDone. {len(changed_files)} file(s) {mode_label}, {len(error_files)} error(s).")

    if check_mode and (changed_files or error_files):
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())

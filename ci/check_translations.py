#!/usr/bin/env python3
"""
Called by translation_check.yml after lupdate has run.

Checks whether lupdate changed any OpenStudioApp_*.ts file by running
`git diff translations/`. If there are changes it means:

  - type="unfinished" stubs were ADDED  → new tr() calls in source with no
    translation yet. Fix: run translate_skeleton.py / translate_all_languages.py.

  - type="obsolete" entries were ADDED  → tr() calls removed from source but
    still present as dead entries in the .ts files. Fix: run
    `lupdate -no-obsolete` and commit the cleaned-up .ts files.

If the diff is empty, all .ts files are in sync with the C++ source.
Exits with code 1 if drift is detected so CI fails.
"""

import re
import subprocess
import sys


def run(cmd: list[str]) -> str:
    return subprocess.check_output(cmd, text=True)


def main() -> int:
    # Produce a diff of everything lupdate may have touched.
    diff = run(["git", "diff", "translations/"])

    if not diff:
        print("All .ts files are in sync with the C++ source. ✓")
        return 0

    # Save patch as artifact for inspection.
    with open("translation_check.patch", "w") as f:
        f.write(diff)

    # Classify what changed so the error message is actionable.
    added_unfinished = len(re.findall(r'^\+.*type="unfinished"', diff, re.MULTILINE))
    added_obsolete   = len(re.findall(r'^\+.*type="obsolete"',   diff, re.MULTILINE))

    # Summarise changed files.
    stat = run(["git", "diff", "--stat", "translations/"])
    print("lupdate changed the following .ts files:")
    print(stat)

    if added_unfinished:
        print(f"  {added_unfinished} new empty unfinished string(s) detected.")
        print("  These are new tr() calls in C++ source that have no translation yet.")
        print("  Fix: run translate_skeleton.py (Spanish) or translate_all_languages.py")
        print("       to batch-translate the new strings, then commit the updated .ts files.")
        print()

    if added_obsolete:
        print(f"  {added_obsolete} new obsolete string(s) detected.")
        print("  These are tr() calls that were removed from C++ source.")
        print("  Fix: run `lupdate src/ -no-obsolete -ts translations/OpenStudioApp_*.ts`")
        print("       then commit the cleaned-up .ts files.")
        print()

    if not added_unfinished and not added_obsolete:
        # Location comments or other minor changes — still fail so the dev is aware.
        print("  .ts files changed in an unexpected way (not unfinished/obsolete).")
        print("  Review translation_check.patch for details.")

    print("Translation check FAILED. Commit the updated .ts files to fix CI.")
    return 1


if __name__ == "__main__":
    sys.exit(main())

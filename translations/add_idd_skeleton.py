#!/usr/bin/env python3
"""
Scan the OpenStudio IDD for field names and add any missing entries to the
IDD context of every OpenStudioApp_*.ts file as unfinished skeleton stubs.

Run this after an SDK update that introduces new IDD objects or fields, then
run translate_all_languages.py to fill in translations for the new entries.
"""

import re
import glob
import os
import sys

SDK_PYTHON = r"C:\Users\ml\openstudioapplication\OpenStudio-3.11.0\OpenStudio-3.11.0+241b8abb4d-Windows\Python"
SDK_BIN    = r"C:\Users\ml\openstudioapplication\OpenStudio-3.11.0\OpenStudio-3.11.0+241b8abb4d-Windows\bin"

sys.path.insert(0, SDK_PYTHON)
os.add_dll_directory(SDK_BIN)

import openstudio


def get_idd_field_names() -> list[str]:
    """Return sorted unique non-Handle field names from the OpenStudio IDD."""
    idd = openstudio.IddFactory.instance().getIddFile(openstudio.IddFileType("OpenStudio"))
    names: set[str] = set()
    for obj in idd.objects():
        nf = obj.numFields()
        for i in range(nf):
            fld = obj.getField(i)
            if fld.is_initialized():
                name = fld.get().name()
                if name and name != "Handle":
                    names.add(name)
        ext_size = len(obj.extensibleGroup())
        for i in range(ext_size):
            fld = obj.getField(nf + i)
            if fld.is_initialized():
                name = fld.get().name()
                if name and name != "Handle":
                    names.add(name)
    return sorted(names)


def get_idd_sources(ts_content: str) -> set[str]:
    """Return the set of <source> strings already in the IDD context."""
    m = re.search(r'<name>IDD</name>(.*?)(?=<context>|\Z)', ts_content, re.DOTALL)
    if not m:
        return set()
    return set(re.findall(r'<source>([^<]+)</source>', m.group(1)))


def add_missing_entries(ts_content: str, missing: list[str]) -> str:
    """Append skeleton messages for missing names before </context> in the IDD block."""
    m = re.search(r'(<name>IDD</name>.*?)(</context>)', ts_content, re.DOTALL)
    if not m:
        return ts_content

    new_messages = "\n".join(
        f'    <message>\n        <source>{name}</source>\n'
        f'        <translation type="unfinished"></translation>\n    </message>'
        for name in missing
    )
    insert_pos = m.start(2)
    return ts_content[:insert_pos] + new_messages + "\n" + ts_content[insert_pos:]


def main():
    print("Fetching IDD field names from OpenStudio SDK...")
    idd_fields = get_idd_field_names()
    print(f"  {len(idd_fields)} unique IDD field names")

    ts_files = sorted(glob.glob("OpenStudioApp_*.ts"))
    print(f"Processing {len(ts_files)} .ts files...\n")

    for ts_file in ts_files:
        lang = os.path.basename(ts_file).replace("OpenStudioApp_", "").replace(".ts", "")
        with open(ts_file, encoding="utf-8") as f:
            content = f.read()

        existing = get_idd_sources(content)
        missing = [n for n in idd_fields if n not in existing]

        if not missing:
            print(f"  [{lang}] no missing entries")
            continue

        new_content = add_missing_entries(content, missing)
        with open(ts_file, "w", encoding="utf-8") as f:
            f.write(new_content)
        print(f"  [{lang}] added {len(missing)} skeleton entries")

    print("\nDone. Run translate_all_languages.py to fill in translations.")


if __name__ == "__main__":
    main()

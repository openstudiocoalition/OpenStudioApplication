#!/usr/bin/env python3
"""
Add a new language entry to MainMenu.cpp and MainMenu.hpp.

Usage:
    python add_language_to_menu.py --lang da --name Danish

Run this before the CMake/lupdate steps when adding a new language.
After running, rebuild the application to see the language in
Preferences > Language.

Touch points handled automatically:
  - MainMenu.hpp: QAction* member variable + slot declaration
  - MainMenu.cpp: action registration block, setChecked(false) in all
    existing language slots, new langXxxClicked() implementation
"""

import argparse
import re
import sys

MAIN_MENU_HPP = "../src/openstudio_lib/MainMenu.hpp"
MAIN_MENU_CPP = "../src/openstudio_lib/MainMenu.cpp"


def main():
    parser = argparse.ArgumentParser(description="Add a language to the MainMenu")
    parser.add_argument("--lang", required=True, help="Language code, e.g. da")
    parser.add_argument("--name", required=True, help="Display name, e.g. Danish")
    args = parser.parse_args()

    lang_code = args.lang
    lang_name = args.name
    camel  = lang_name.replace(" ", "")
    member = f"m_lang{camel}Action"
    slot   = f"lang{camel}Clicked"

    with open(MAIN_MENU_HPP, encoding="utf-8") as f:
        hpp = f.read()
    with open(MAIN_MENU_CPP, encoding="utf-8") as f:
        cpp = f.read()

    if member in hpp:
        sys.exit(f"ERROR: {member} already exists in MainMenu.hpp — already added?")

    # Extract ordered member list from an existing slot to use as the unchecked list
    slot_body_m = re.search(
        r'void MainMenu::langIndonesianClicked\(\)\s*\{(.*?)\}',
        cpp, re.DOTALL
    )
    if not slot_body_m:
        sys.exit("ERROR: Could not find langIndonesianClicked() in MainMenu.cpp")
    existing_members = re.findall(r'(m_lang\w+Action)->setChecked', slot_body_m.group(1))

    # --- MainMenu.hpp ---

    # Member variable: insert after last m_lang...Action;
    last_member_m = list(re.finditer(r'  QAction\* m_lang\w+Action;', hpp))
    if not last_member_m:
        sys.exit("ERROR: Could not find m_lang...Action members in MainMenu.hpp")
    pos = last_member_m[-1].end()
    hpp = hpp[:pos] + f"\n  QAction* {member};" + hpp[pos:]

    # Slot declaration: insert after last lang...Clicked(); (before addingNewLanguageClicked)
    last_slot_m = list(re.finditer(r'  void lang\w+Clicked\(\);', hpp))
    if not last_slot_m:
        sys.exit("ERROR: Could not find lang...Clicked() slots in MainMenu.hpp")
    pos = last_slot_m[-1].end()
    hpp = hpp[:pos] + f"\n  void {slot}();" + hpp[pos:]

    # --- MainMenu.cpp ---

    # Action registration block: insert before "Add a new language"
    add_new_pos = cpp.find('  action = new QAction(tr("Add a new language")')
    if add_new_pos == -1:
        sys.exit('ERROR: Could not find "Add a new language" action in MainMenu.cpp')
    block = (
        f'  {member} = new QAction(tr("{lang_name}"), this);\n'
        f'  m_preferencesActions.push_back({member});\n'
        f'  {member}->setCheckable(true);\n'
        f'  langMenu->addAction({member});\n'
        f'  connect({member}, &QAction::triggered, this, &MainMenu::{slot}, Qt::QueuedConnection);\n\n'
    )
    cpp = cpp[:add_new_pos] + block + cpp[add_new_pos:]

    # setChecked(false) in all existing language clicked slots only.
    # We match the Indonesian line that appears inside a langXxxClicked() function body,
    # i.e. where Indonesian is set to false (all slots except langIndonesianClicked itself).
    # We also handle langIndonesianClicked (where it's set to true) separately.
    # Simplest safe approach: replace only inside function bodies, not the constructor chain.
    # We locate all langXxxClicked() bodies and insert after the Indonesian line there.
    def insert_unchecked_in_slots(text):
        # Match each langXxxClicked function body and insert the new setChecked(false) line
        func_pattern = re.compile(
            r'(void MainMenu::lang\w+Clicked\(\)\s*\{)(.*?)(^\})',
            re.DOTALL | re.MULTILINE
        )
        def patch_body(m):
            body = m.group(2)
            if f'm_langIndonesianAction->setChecked' not in body:
                return m.group(0)
            patched = re.sub(
                r'(  m_langIndonesianAction->setChecked\([^)]+\);)',
                lambda mm: mm.group(0) + f"\n  {member}->setChecked(false);",
                body, count=1
            )
            return m.group(1) + patched + m.group(3)
        return func_pattern.sub(patch_body, text)

    cpp = insert_unchecked_in_slots(cpp)

    # Initializer if/else chain: insert before the final } else { (English default)
    # Pattern: the last } else if ... id ... branch, then the } else { default
    cpp = re.sub(
        r'(} else if \(m_currLang == "id"\) \{[^}]+\})\s*(\} else \{)',
        lambda m: m.group(1) + f'\n  }} else if (m_currLang == "{lang_code}") {{\n    {member}->setChecked(true);\n  ' + m.group(2),
        cpp, count=1
    )

    # New slot implementation: insert before addingNewLanguageClicked()
    adding_pos = cpp.find("void MainMenu::addingNewLanguageClicked()")
    if adding_pos == -1:
        sys.exit("ERROR: Could not find addingNewLanguageClicked() in MainMenu.cpp")
    unchecked = "".join(f"  {m}->setChecked(false);\n" for m in existing_members)
    new_slot = (
        f"void MainMenu::{slot}() {{\n"
        f"{unchecked}"
        f"  {member}->setChecked(true);\n\n"
        f'  emit changeLanguageClicked("{lang_code}");\n'
        f"}}\n\n"
    )
    cpp = cpp[:adding_pos] + new_slot + cpp[adding_pos:]

    with open(MAIN_MENU_HPP, "w", encoding="utf-8") as f:
        f.write(hpp)
    with open(MAIN_MENU_CPP, "w", encoding="utf-8") as f:
        f.write(cpp)

    print(f"Done. Added '{lang_name}' (--lang {lang_code}) to the language menu.")
    print(f"  Member : {member}")
    print(f"  Slot   : MainMenu::{slot}()")
    print("Rebuild the application to see it in Preferences > Language.")


if __name__ == "__main__":
    main()

# OpenStudio Application — Translation Workflow

> **Note:** This document describes the AI-assisted pipeline used to bootstrap
> and maintain machine translations. The preferred approach is still to have a
> bilingual contributor translate directly using Qt Linguist — see the wiki:
> [Internationalization: Translating the OpenStudioApplication to another target language](https://github.com/openstudiocoalition/OpenStudioApplication/wiki/Internationalization:-Translating-the-OpenStudioApplication-to-another-target-language)

This document describes the AI-assisted pipeline for adding or updating translations in
`OpenStudioApp_<lang>.ts` files.

---

## Quick-reference pipeline

Python translation scripts live in `translations/` and must be run from that directory.
CMake targets must be run from the repo root.

```bash
# From repo root:
cmake --build . --target OpenStudioApplication_lupdate

# From translations/ directory:
cd translations
python fix_and_unvanish.py
python translate_all_languages.py       # only when adding new strings or a new language
python fix_and_unvanish.py              # always run again after translate script
cd ..

# From repo root:
cmake --build . --target OpenStudioApplication_lrelease
cmake --build . --config Release
```

`translate_all_languages.py` dispatches each string to one of three definition-aware
prompts (IDD / OutputVariables / GUI) and deduplicates source strings before batching,
so "Name:" appearing in 90 inspector views costs one API request, not ninety.

When new `tr()` strings are added to the C++ source, update the appropriate definitions
file **before** running `translate_all_languages.py` (from within `translations/`):
- GUI strings → `python build_gui_definitions.py`
- IDD fields → `python scrape_idd_field_definitions.py`
- Output variables → `python scrape_output_var_definitions.py`

> **Rule:** `fix_and_unvanish.py` must be run after **every** lupdate and after
> every run of `translate_all_languages.py`. Skipping it leaves strings marked
> `type="vanished"` or `type="unfinished"` that the app will silently fall back
> to English for.

---

## Step-by-step

### 1. Wrap new strings with `tr()`

- In Q_OBJECT member functions: `tr("My string")`
- In static / non-Q_OBJECT functions: `QCoreApplication::translate("ContextName", "My string")`
- **Never** wrap via a local lambda — lupdate cannot follow the indirection and
  will silently miss those strings.
- Model-bound combo boxes: `addItem(tr("Display text"), QStringLiteral("EnglishEnumValue"))`
  and read back with `currentData()`, **not** `currentText()`.

### 2. Run lupdate

```bash
# From repo root:
cmake --build . --target OpenStudioApplication_lupdate
```

- Always use the CMake target — never run `lupdate` directly on individual files.
  Doing so marks every string not in that file as `type="vanished"`.
- lupdate adds new `<message>` entries as `type="unfinished"` and marks removed
  or renamed source strings as `type="vanished"`.

### 3. Run fix_and_unvanish.py  ← **do not skip**

```bash
# From translations/ directory:
python fix_and_unvanish.py
```

What it does:
- **Un-vanishes** any `type="vanished"` entry that already has a non-empty
  translation (IDD runtime strings get re-vanished by lupdate every time).
- Promotes `type="unfinished"` entries that already have translation text to
  finished (removes the attribute).
- Copies the `" Value: "` vanished translation to the new `"Value: "` entry in
  `ScheduleConstantInspectorView`.

Expected output: every `.ts` file shows `Vanished=0, Unfinished=0`.

### 4. Update definitions JSON for new strings (when needed)

`translate_all_languages.py` uses three definition files to give the translator context.
When you add **new `tr()` strings** to the C++ source, update the appropriate file before translating:

#### GUI strings (any context that is not `IDD` or `OutputVariables`)

```bash
# From translations/ directory:
python build_gui_definitions.py
```

This reads all `.ts` files, finds source strings not yet in `gui_string_definitions.json`,
classifies each one (`general_software` / `hvac_terminology` / `openstudio_specific`) via the
Claude API, and writes a definition note that the translator will receive as context.
Output: `gui_string_definitions.json`.

For a small number of new strings you can also add entries manually:
```json
"My New Label": {
    "category": "openstudio_specific",
    "definition": "Short description of what this label means in the UI."
}
```

#### IDD field labels (`IDD` context — new EnergyPlus objects or fields)

```bash
# From translations/ directory:
python scrape_idd_field_definitions.py
```

Re-scrapes the BigLadder I/O Reference pages listed in `IddObjectDocUrl.hpp` and
regenerates `idd_field_definitions.json`. Re-run when the EnergyPlus version changes
or when new objects are added to `IddObjectDocUrl.hpp`.

#### Output variables (`OutputVariables` context — new EnergyPlus version)

```bash
# From translations/ directory:
python scrape_output_var_definitions.py
```

Re-scrapes 52 BigLadder I/O Reference pages and regenerates `output_var_definitions.json`.
Re-run when the EnergyPlus version changes or new output variables are added.

> **Skip this step** if you haven't added any new source strings — only the three JSON
> definition files need updating, and they don't change when you're only adding a new
> language or re-running translations.

### 5. Translate new strings (when needed)

```bash
# From translations/ directory:
python translate_all_languages.py
```

Then immediately run fix_and_unvanish.py again:

```bash
python fix_and_unvanish.py
```

### 6. Validate with lrelease

```bash
# From repo root:
cmake --build . --target OpenStudioApplication_lrelease
```

Any XML parse error (e.g. unescaped `&` in an accelerator mnemonic like
`종료(&E)` instead of `종료(&amp;E)`) will surface here. Fix and re-run.

### 7. Rebuild

```bash
# From repo root:
cmake --build . --config Release
```

---

## Machine translation quality improvement

`translate_all_languages.py` uses definition-aware prompts for all three context types — it
dispatches each string internally to the appropriate IDD, OutputVariables, or GUI prompt, so
every language (including newly added ones) benefits from definition context from the first run.

The three standalone retranslate scripts (`retranslate_output_vars.py`,
`retranslate_idd_fields.py`, `retranslate_gui_strings.py`) are for **targeted re-translation of
a specific context** after its definition file has been updated or improved — for example, after
scraping a new EnergyPlus version or manually improving definitions. They retranslate only their
context in selected languages without touching the rest of the `.ts` files.

### Definition files

Three JSON files provide the translator with context. They are committed to the repo so scripts
can run without a network connection.

| File | Context | Source |
|------|---------|--------|
| `output_var_definitions.json` | `OutputVariables` | Scraped from 52 BigLadder I/O Reference pages |
| `idd_field_definitions.json` | `IDD` | Scraped from 47 BigLadder I/O Reference pages |
| `gui_string_definitions.json` | all other contexts | Claude-generated via `build_gui_definitions.py` |

**Coverage:**
- Output variables: ~51% have a scraped definition; the remainder receive category context only
- IDD field labels: ~52% of 3,843 unique labels have a scraped definition
- GUI strings (~1,520 unique): 100% receive category + definition context (AI-generated)

### Output Variables — re-translation

Re-translates the `OutputVariables` context with definitions and a category-split structure
(`Category: Measurement` or `Zone: SubCat: Measurement` for zone variables).

```bash
# From translations/ directory:

# Update definitions when EnergyPlus version changes (produces output_var_definitions.json)
python scrape_output_var_definitions.py

# Re-translate unfinished/empty entries only — safe for human-provided translations (default)
python retranslate_output_vars.py

# Re-translate ALL entries — use this to generate a fresh machine translation for every string
# so it can be compared side-by-side with human-provided translations in output_vars_comparison.csv.
# Prints a warning and waits 5 s before submitting; press Ctrl+C to abort.
# After running, update the CSV with: python update_comparison_csvs.py --part 2
python retranslate_output_vars.py --mode all

# Other options
python retranslate_output_vars.py --lang es
python retranslate_output_vars.py --all
python retranslate_output_vars.py --lang es fr --model claude-sonnet-4-6

# Always follow with:
python fix_and_unvanish.py
# Then from repo root:
cmake --build .. --target OpenStudioApplication_lrelease
```

### IDD input field labels — re-translation

Re-translates the `IDD` context with field definitions (including full enum option descriptions)
and the EnergyPlus object type as context. Reads the page list from `IddObjectDocUrl.hpp`.

```bash
# From translations/ directory:

# Update definitions when EnergyPlus version changes or new objects are added to IddObjectDocUrl.hpp
python scrape_idd_field_definitions.py

# Re-translate unfinished/empty entries only — safe for human-provided translations (default)
python retranslate_idd_fields.py

# Re-translate ALL entries — use this to generate a fresh machine translation for every string
# so it can be compared side-by-side with human-provided translations in idd_fields_comparison.csv.
# Prints a warning and waits 5 s before submitting; press Ctrl+C to abort.
# After running, update the CSV with: python update_comparison_csvs.py --part 3
python retranslate_idd_fields.py --mode all

# Other options
python retranslate_idd_fields.py --lang es
python retranslate_idd_fields.py --all
python retranslate_idd_fields.py --lang es fr --model claude-sonnet-4-6

# Always follow with:
python fix_and_unvanish.py
# Then from repo root:
cmake --build .. --target OpenStudioApplication_lrelease
```

### GUI strings — re-translation

Re-translates the ~1,970 strings in the remaining 193 Qt contexts — menu items, button labels,
dialog titles, error messages, inspector field labels, HVAC component names, and similar.

Strings are classified into three categories:
- **general_software** — common UI terms found in any application (Save, Cancel, Name:, Apply, …)
- **hvac_terminology** — HVAC and building-physics terms (Chiller, Boiler, Design Day, ASHRAE, …)
- **openstudio_specific** — OpenStudio/EnergyPlus workflow terms (Measure, BCL, Space Type, …)

`build_gui_definitions.py` layers three information sources per string: Qt context role (e.g.
"drag-and-drop target area"), IDD cross-reference (216 strings), and OpenStudio SDK cross-reference
(26 Standards* strings via `sdk_doc_definitions.json`).

```bash
# From translations/ directory:

# Update SDK definitions when OpenStudio SDK version changes (produces sdk_doc_definitions.json)
# Check https://openstudio-sdk-documentation.s3.amazonaws.com/index.html for the latest version.
python scrape_sdk_docs.py --version 3.10.0

# Rebuild GUI definitions after new tr() strings are added or SDK defs are updated
python build_gui_definitions.py

# Re-translate unfinished/empty entries only — safe for human-provided translations (default)
python retranslate_gui_strings.py

# Re-translate ALL entries — use this to generate a fresh machine translation for every string
# so it can be compared side-by-side with human-provided translations in gui_comparison.csv.
# Prints a warning and waits 5 s before submitting; press Ctrl+C to abort.
# After running, update the CSV with: python update_comparison_csvs.py --part 1
python retranslate_gui_strings.py --mode all

# Other options
python retranslate_gui_strings.py --lang es
python retranslate_gui_strings.py --all
python retranslate_gui_strings.py --lang es fr --model claude-sonnet-4-6

# Always follow with:
python fix_and_unvanish.py
# Then from repo root:
cmake --build .. --target OpenStudioApplication_lrelease
```

### When to re-run each script

| Trigger | Action |
|---------|--------|
| EnergyPlus version bump | Re-run `scrape_output_var_definitions.py` and `scrape_idd_field_definitions.py`, then the matching retranslate scripts |
| New IDD objects added to `IddObjectDocUrl.hpp` | Re-run `scrape_idd_field_definitions.py`, then `retranslate_idd_fields.py` |
| New `tr()` strings added to C++ source | Re-run `build_gui_definitions.py`, then `retranslate_gui_strings.py` |
| OpenStudio SDK version bump | Re-run `scrape_sdk_docs.py --version X.Y.Z`, then `build_gui_definitions.py`, then `retranslate_gui_strings.py` |
| Definition text manually improved | Re-run the matching retranslate script for the affected context |

---

## Supported languages (18)

| Code | Language |
|------|----------|
| `ar` | Arabic |
| `ca` | Catalan |
| `de` | German |
| `el` | Greek |
| `es` | Spanish |
| `fa` | Persian / Farsi |
| `fr` | French |
| `he` | Hebrew |
| `hi` | Hindi |
| `id` | Indonesian |
| `it` | Italian |
| `ja` | Japanese |
| `ko` | Korean |
| `pl` | Polish |
| `pt` | Portuguese |
| `tr` | Turkish |
| `vi` | Vietnamese |
| `zh_CN` | Simplified Chinese |

---

## Adding a new language

1. Wire up the C++ language menu:
   ```bash
   python add_language_to_menu.py --lang <code> --name "Language Name"
   ```
   This script edits `src/openstudio_lib/MainMenu.hpp` and `MainMenu.cpp` in one
   step, handling all five touch points:
   - `MainMenu.hpp`: `QAction*` member variable + slot declaration
   - `MainMenu.cpp`: action registration block (before "Add a new language")
   - `MainMenu.cpp`: `setChecked(false)` inserted in all existing language slots
   - `MainMenu.cpp`: new `langXxxClicked()` implementation
   - `MainMenu.cpp`: `else if (m_currLang == "<code>")` branch in the constructor's
     startup checkmark initializer (so the menu reflects the saved language on launch)

2. Add the `.ts` file entry to `translations/CMakeLists.txt` `TS_FILES`:
   ```cmake
   OpenStudioApp_<lang>.ts  # Language name
   ```

3. Add the corresponding post-build copy commands to the same file (follow the existing pattern; check Qt's `translations/` directory to confirm which `.qm` files exist for the locale):
   ```cmake
   # Language name
   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qt_<lang>.qm ...
   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_INSTALL_DIR}/translations/qtbase_<lang>.qm ...
   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEBENGINE_PAK_FOLDER}/<lang>.pak ...
   ```
   If Qt does not ship `qt_<lang>.qm` / `qtbase_<lang>.qm` for the locale (as with Greek, Hindi, Vietnamese, Indonesian), comment those two lines out and only copy the WebEngine pak.

4. Add the language to the `LANGUAGES` dict in `translations/translate_all_languages.py`:
   ```python
   "<lang>": "Language Name",
   ```

5. Run the standard pipeline:
   ```bash
   # From repo root:
   cmake --build . --target OpenStudioApplication_lupdate

   # From translations/ directory:
   python fix_and_unvanish.py
   python translate_all_languages.py
   python fix_and_unvanish.py

   # From repo root:
   cmake --build . --target OpenStudioApplication_lrelease
   ```
   lupdate will create the new `.ts` file automatically on the first run.

6. Add the language to the **Supported languages** table in this document.

---

## Helper scripts (`translations/`)

All helper scripts live in `translations/` and should be run from that directory.

| Script | Purpose |
|--------|---------|
| `fix_and_unvanish.py` | Un-vanish translated entries; promote finished unfinished entries |
| `translate_all_languages.py` | Batch-translate new strings into all languages via Claude API (re-run to pick up any entries that came back empty) |
| `add_idd_skeleton.py` | Scan the OpenStudio IDD via Python bindings and insert unfinished stubs for any field names missing from the `.ts` files; follow with `translate_all_languages.py` |
| `add_language_to_menu.py` | Wire up a new language in `MainMenu.cpp/.hpp` (member, slot, action block, setChecked lines) |
| `scrape_output_var_definitions.py` | Scrape EnergyPlus I/O Reference output variable definitions → `output_var_definitions.json` |
| `retranslate_output_vars.py` | Re-translate `OutputVariables` context with definition-aware, category-split prompts |
| `scrape_idd_field_definitions.py` | Scrape EnergyPlus I/O Reference field definitions for IDD objects → `idd_field_definitions.json` |
| `retranslate_idd_fields.py` | Re-translate `IDD` context with field definition and object-type context |
| `scrape_sdk_docs.py` | Scrape OpenStudio model SDK class pages → `sdk_doc_definitions.json` (Standards* fields, model properties) |
| `build_gui_definitions.py` | Classify and define all ~1,520 unique GUI strings → `gui_string_definitions.json` (layers IDD + SDK + UI role context) |
| `retranslate_gui_strings.py` | Re-translate all non-IDD/non-OutputVariables contexts with category + definition context |

---

## Common mistakes

| Mistake | Consequence | Fix |
|---------|-------------|-----|
| Skipping `fix_and_unvanish.py` after lupdate | IDD/runtime strings silently fall back to English | Run the script |
| Running lupdate on a single file directly | All strings not in that file marked `type="vanished"` | Always use the CMake target |
| Running Python scripts from the repo root instead of `translations/` | Scripts cannot find `.ts` files or JSON definitions | `cd translations` before running any Python script |
| Using `currentText().toStdString()` with a model setter | Model receives translated string instead of English enum value | Use `currentData()` with English data role |
| Lambda wrapper around `QCoreApplication::translate()` | lupdate misses the strings entirely | Use string literal inline |
| Unescaped `&` in translated accelerator mnemonics | lrelease XML parse error | Use `&amp;` |

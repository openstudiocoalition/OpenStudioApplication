# PR 873 — Review Response Drafts

Status of each review comment thread. Update this file as items change.

---

## macumber — General review ("found anti-patterns, fix cppcheck and Clang Format")

> All addressed:
> - Removed all `#define MACRO tr(...)` patterns across 13 grid view files — all column header strings are now inline `tr()` calls throughout
> - Changed return types in `MaterialsView`, `ScheduleOthersView`, and `ConstructionsView` to `std::vector<std::pair<IddObjectType, QString>>` with direct `tr()` calls
> - Reverted HTML geometry files — agreed to skip the in-HTML JavaScript approach for now and pursue the proper Qt injection approach in a follow-up PR
> - Copilot bugs fixed: signal connection order (`VariablesTabView`), dead pre-bind `addItem()` calls (`ScheduleFileInspectorView`), string concat → `tr("Custom Report %1").arg(n)` (`ResultsTabView`), leading space removed (`ScheduleConstantInspectorView`)
> - clang-format run on all 35 modified C++ files

---

## macumber — "did you get Claude to do the string translations?"

> Yes — Claude did both the `tr()` wrapping and the translations. The batch API approach scales well: all 18 languages are now in this PR, generated via Claude Haiku Batches API at roughly $3–4 total cost. Each language gets ~7,000 translations covering UI strings, IDD field names, and OutputVariable display names. Adding a new language in the future is a one-command operation (`translate_new_languages.py`).
>
> **Languages added:** Arabic, Catalan, German, Greek, Persian/Farsi, French, Hebrew, Hindi, Indonesian, Italian, Japanese, Korean, Polish, Portuguese, Spanish, Turkish, Vietnamese, Simplified Chinese.

---

## macumber — `InspectorGadget.cpp` (maintenance concern + OutputVariables coverage)

> Good call. The OutputVariable gap is fixed — the original entries only had `Zone X` prefixed names, but `outputVariableNames()` returns bare names (e.g. `Lights Electricity Energy`). Added 359 bare-name entries auto-derived from the Zone translations, plus specific `Site`/`Surface`/`People`/`Daylighting` entries.
>
> For IDD fields: all fields now have translations across all 18 language `.ts` files, generated via Claude API batch translation. Two complementary tools maintain coverage going forward:
>
> - **`add_idd_skeleton.py`** — developer maintenance tool. Run after an SDK update to add skeleton entries for any new IDD fields to all `.ts` files, then run `translate_skeleton.py` to batch-translate them.
> - **`Translation_ts.IddCoverageAllLanguages`** GTest — CI verification. Uses `IddFactory::instance()` to enumerate every IddObject and its fields, then checks that each unique field name has a source entry in the IDD context of **every** `OpenStudioApp_*.ts` file. Fails with a per-language list of missing field names if any `.ts` file is out of sync with the SDK.
>
> These are complementary: the test detects drift, the script fixes it. The test does not call the script.

---

## macumber — `.ts` file comment ("need github actions to detect vanished or unfinished strings")

> Agreed — will open a separate PR for this.

---

## macumber — `geometry_editor_start.html` + `geometry_preview.html`

> Reverted. We'll implement the proper Qt approach (injecting locale via `QWebEnginePage::runJavaScript()` and moving the strings into the `.ts` file) in a follow-up PR.

---

## macumber — All `#define` macro comments

*(SpacesInteriorPartitionsGridView, SpacesLoadsGridView, SpacesShading, SpacesSpaces, SpacesSubsurfaces, SpacesSubtab, SpacesSurfaces, SpaceTypes, ThermalZones, FacilityShading, FacilityStoriesGridView, FacilityExteriorEquipmentGridView)*

> Done — all `#define MACRO tr(...)` patterns removed from all 13 grid view files. Column header strings are now inline `tr()` calls.

---

## macumber — Return type comments (MaterialsView, ScheduleOthersView, ConstructionsView)

> Done.

---

## macumber — `SchedulesView.cpp` month names suggestion

> The current implementation already uses the correct pattern: `const QStringList monthNames = {tr("January"), ...}` with each string individually wrapped, so lupdate can extract them. The alternative (`tr(variable)`) wouldn't be extractable by lupdate. Leaving as-is.

---

## Copilot — `VariablesTabView.cpp` signal connection order

> Fixed — moved `connect()` to after `populateFrequencyComboBox()` and after the initial index is set, so populating the list doesn't fire `indexChanged()` during construction.

---

## Copilot — `ScheduleFileInspectorView.cpp` dead `addItem()` calls

> Fixed — removed the pre-bind `addItem(tr(...))` calls. The `bind()` call supplies its own choices list and overwrites them anyway.

---

## Copilot — `ResultsTabView.cpp` string concatenation

> Fixed — changed to `tr("Custom Report %1").arg(num)`.

---

## Copilot — `ScheduleConstantInspectorView.cpp` leading space

> Fixed — changed to `tr("Value: ")`.

---

## Copilot — BOM in inspector view files

> Done — removed the UTF-8 BOM from 27 inspector view `.cpp` files (commit d2396584). The BOM was introduced by the editor when files were modified during i18n work; it has no effect at runtime but is incorrect for UTF-8 source files.

---

## Copilot — `SpaceTypesGridView.cpp` tooltip, `GeometryEditorView.cpp` Debug button + dialog title, `GeometryPreviewView.cpp` checkbox tooltip

> Done — all C++ hardcoded strings are now wrapped with `tr()`:
> - `SpaceTypesGridView.cpp` tooltip was already `tr()`-wrapped earlier in the PR
> - `GeometryEditorView.cpp` "Debug" button label wrapped (commit d2396584)
> - `GeometryPreviewView.cpp` Geometry Diagnostics checkbox tooltip wrapped (commit d2396584)
>
> The HTML geometry files (`geometry_editor_start.html`, `geometry_preview.html`) still contain untranslated JavaScript strings, but these require the `QWebEnginePage::runJavaScript()` injection approach — deferred to a follow-up PR by agreement with macumber.

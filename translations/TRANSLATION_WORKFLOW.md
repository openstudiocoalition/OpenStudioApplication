# OpenStudio Application — Translation Workflow

> **Note:** This document describes the AI-assisted pipeline used to bootstrap
> and maintain machine translations. The preferred approach is still to have a
> bilingual contributor translate directly using Qt Linguist — see the wiki:
> [Internationalization: Translating the OpenStudioApplication to another target language](https://github.com/openstudiocoalition/OpenStudioApplication/wiki/Internationalization:-Translating-the-OpenStudioApplication-to-another-target-language)

This document describes the AI-assisted pipeline for adding or updating translations in
`OpenStudioApp_<lang>.ts` files.

---

## Quick-reference pipeline

```
cmake --build . --target OpenStudioApplication_lupdate
python fix_and_unvanish.py
python translate_all_languages.py       # only when adding new strings
python fix_and_unvanish.py              # always run again after translate script
cmake --build . --target OpenStudioApplication_lrelease
cmake --build . --config Release
```

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
cmake --build . --target OpenStudioApplication_lupdate
```

- Always use the CMake target — never run `lupdate` directly on individual files.
  Doing so marks every string not in that file as `type="vanished"`.
- lupdate adds new `<message>` entries as `type="unfinished"` and marks removed
  or renamed source strings as `type="vanished"`.

### 3. Run fix_and_unvanish.py  ← **do not skip**

```bash
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

### 4. Translate new strings (when needed)

```bash
python translate_all_languages.py
```

Then immediately run fix_and_unvanish.py again:

```bash
python fix_and_unvanish.py
```

### 5. Validate with lrelease

```bash
cmake --build . --target OpenStudioApplication_lrelease
```

Any XML parse error (e.g. unescaped `&` in an accelerator mnemonic like
`종료(&E)` instead of `종료(&amp;E)`) will surface here. Fix and re-run.

### 6. Rebuild

```bash
cmake --build . --config Release
```

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

## Helper scripts (repo root)

| Script | Purpose |
|--------|---------|
| `fix_and_unvanish.py` | Un-vanish translated entries; promote finished unfinished entries |
| `translate_all_languages.py` | Batch-translate new strings into all 17 non-Spanish languages via Claude API |
| `retranslate_stubborn.py` | Re-translate any entries that came back empty from the batch run |
| `recover_batches.py` | Re-poll known batch IDs after a network failure |

---

## Common mistakes

| Mistake | Consequence | Fix |
|---------|-------------|-----|
| Skipping `fix_and_unvanish.py` after lupdate | IDD/runtime strings silently fall back to English | Run the script |
| Running lupdate on a single file directly | All strings not in that file marked `type="vanished"` | Always use the CMake target |
| Using `currentText().toStdString()` with a model setter | Model receives translated string instead of English enum value | Use `currentData()` with English data role |
| Lambda wrapper around `QCoreApplication::translate()` | lupdate misses the strings entirely | Use string literal inline |
| Unescaped `&` in translated accelerator mnemonics | lrelease XML parse error | Use `&amp;` |

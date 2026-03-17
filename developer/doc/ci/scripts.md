# CI: Helper Scripts

> **Back to:** [CI/CD Overview](overview.md)

## Overview

The `ci/` directory contains five helper scripts used by GitHub Actions workflows and the local development pre-commit hook.

---

## `ci/clang-format.sh`

**Used by:** [`clangformat.yml`](workflows/clangformat.md), [`ci/pre-commit.sh`](#cipre-commitsh)

Runs `clang-format` on only the C/C++ files that differ between two git refs, then produces a unified diff patch.

### Arguments

```bash
ci/clang-format.sh <ref1> <ref2>
# e.g.: ci/clang-format.sh HEAD remotes/origin/develop
```

### Behavior

1. Computes `git diff --name-only ref1 ref2` filtered to `*.cpp`, `*.hpp`, `*.cxx`, `*.h`
2. Runs `clang-format -style=file -i` on each changed file (uses the `.clang-format` in the repo root)
3. Runs `git diff` to detect any formatting changes
4. If changes exist: writes `clang_format.patch` and exits with code 1
5. If no changes: exits with code 0

### Exit Codes

| Code | Meaning |
|---|---|
| 0 | All changed files are correctly formatted |
| 1 | Formatting changes needed; `clang_format.patch` produced |

---

## `ci/pre-commit.sh`

**Used by:** Developer local setup (git pre-commit hook)

A git pre-commit hook that runs `clang-format.sh` on staged changes before each commit. Prevents unformatted code from being committed.

### Installation

```bash
cd .git/hooks
ln -s ../../ci/pre-commit.sh pre-commit
```

### Behavior

1. Stashes unstaged changes (`git stash -u`) to isolate staged content
2. Runs `ci/clang-format.sh HEAD origin/develop`
3. If exit code is 1: prints guidance and aborts the commit (exit 1)
4. Restores stashed changes (`git stash pop`)

---

## `ci/colorize_cppcheck_results.py`

**Used by:** [`cppcheck.yml`](workflows/cppcheck.md)

Reads cppcheck output from stdin, applies ANSI colors by severity, and prints a summary counter.

### Input Format

Expects lines matching:
```
[file.cpp:42]:(error),[missingOverride],Override has no matching virtual function
```

### Color Mapping

| Severity | ANSI Color |
|---|---|
| `error` | Red |
| `warning` | Yellow |
| `style` | Blue |
| `performance` | Cyan |
| `portability` | Magenta |
| `information` | Green |

### Usage

```bash
cppcheck ... 2>&1 | python3 ci/colorize_cppcheck_results.py
```

---

## `ci/install_script_qtifw.qs`

**Used by:** [`app_build.yml`](workflows/app_build.md) (Windows and macOS, silent install verification)

A QtIFW (Qt Installer Framework) controller script for **unattended / silent installation** in CI environments.

### Behavior

Implements all QtIFW `Controller` callbacks:
- `WelcomePageCallback` — auto-clicks Next
- `LicenseAgreementPageCallback` — auto-accepts
- `ComponentSelectionPageCallback` — accepts defaults
- `TargetDirectoryPageCallback` — accepts default install path
- `ReadyForInstallationPageCallback` — clicks Install
- `FinishedPageCallback` — disables "Launch after install" checkbox, clicks Finish

This allows automated installation of the produced `.exe`/`.dmg` installer as part of post-build verification.

---

## `ci/parse_cmake_versions.py`

**Used by:** [`app_build.yml`](workflows/app_build.md), [`export_standards_data.yml`](workflows/export_standards_data.md)

Parses version numbers from `CMakeLists.txt` and `FindOpenStudioSDK.cmake` and writes them to `GITHUB_ENV` and `GITHUB_OUTPUT` for use by downstream workflow steps.

### Variables Extracted

From `CMakeLists.txt`:
- `OS_APP_VERSION` — e.g., `1.11.0`
- `OS_APP_VERSION_MAJOR`, `OS_APP_VERSION_MINOR`, `OS_APP_VERSION_PATCH`

From `FindOpenStudioSDK.cmake`:
- `OS_SDK_VERSION` — assembled from major/minor/patch/prerelease components
- `OS_SDK_VERSION_MAJOR`, `OS_SDK_VERSION_MINOR`, `OS_SDK_VERSION_PATCH`
- `OS_SDK_VERSION_PRERELEASE`, `OS_SDK_SHA` (git SHA of the SDK)

### Usage

```bash
python3 ci/parse_cmake_versions.py
# Writes to $GITHUB_ENV and $GITHUB_OUTPUT automatically
```

After this step, any subsequent GitHub Actions step can use the variables:
```yaml
- name: Use version
  run: echo "Building version ${{ env.OS_APP_VERSION }}"
```

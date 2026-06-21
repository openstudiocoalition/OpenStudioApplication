# OpenStudio Application Development

## Project Overview

Cross-platform (Windows, Mac, Linux) graphical interface for OpenStudio energy models. Built on the [OpenStudio SDK](https://github.com/NatLabRockies/OpenStudio) using Qt 6.11.0 and C++20.

## Quick Start

See [BUILDING.md](BUILDING.md) for comprehensive build instructions.

**Docker-based local builds** (for validation, not production):
```bash
# First time setup
make image && make configure && make build && make test

# After source file edits
make build && make test

# After CMakeLists.txt or conanfile.py changes
make configure && make build && make test
```

**Windows users:** Run `make` commands via WSL (Ubuntu 22.04), not PowerShell. See [docker-build instructions](.github/instructions/docker-build.instructions.md) for setup details.

**Production builds** use native conan + cmake (see [BUILDING.md](BUILDING.md)).

## Code Standards

- **C++20** standard, compile with `-Werror` (warnings are errors)
- **Qt 6.11.0** for all GUI components
- **Unit tests required** with 90%+ coverage (see [CONTRIBUTING.md](CONTRIBUTING.md))
- Run static analysis: `make cppcheck`

## Testing

**Known baseline failures** in Docker environment (expected, not regressions):
- `ModelEditorFixture.MorePath_Conversions` — Windows path test always fails on Linux
- `OpenStudioLibFixture.AnalyticsHelperSecrets` — Requires CI-injected secrets

All other tests must pass. Zero warnings, zero regressions.

## Architecture

- **src/model_editor** — Core model editing functionality and Qt UI components
- **src/openstudio_lib** — Application library layer, main UI screens
- **src/openstudio_app** — Qt application entry point and lifecycle
- **src/shared_gui_components** — Reusable widgets and workflow controllers

Supports OpenStudio Measures (Ruby/Python scripts for model transformations).

## Key Conventions

- Docker builds are for **local validation only**; CI uses native builds on ubuntu-22.04, macos-12, and windows-2022
- Build artifacts live in a Docker volume (`osapp-build`), not on the host filesystem — use `make check-build` to inspect
- On Windows, Makefile targets require WSL bash; they will not work in PowerShell or cmd.exe

## Resources

- [Documentation](https://openstudiocoalition.org/)
- [Contributing Guidelines](CONTRIBUTING.md)
- [Code of Conduct](CODE_OF_CONDUCT.md)
- [Building Instructions](BUILDING.md)

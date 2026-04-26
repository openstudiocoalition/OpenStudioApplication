# Library: `utilities` — Path Helpers

> **Source:** `src/utilities/`  
> **CMake target:** `openstudioapp_utilities` (static library)  
> **Back to:** [Architecture Overview](../architecture.md)

## Purpose

The `utilities` module is a minimal helper library that resolves runtime filesystem paths to key components: the OpenStudio CLI, EnergyPlus, Radiance, and the application's own resources directory.

Because the application is packaged differently on each platform (macOS bundle, Windows installer tree, Linux deb), path resolution must be centralised and aware of the deployment layout. This module provides that abstraction.

---

## Files

| File | Description |
|---|---|
| `OpenStudioApplicationPathHelpers.hpp` | Declares free functions for application version strings and runtime path resolution; no DLL export macro (static library) |
| `OpenStudioApplicationPathHelpers.cxx.in` | CMake template — `configure_file` substitutes the actual install-time paths at build time, producing `OpenStudioApplicationPathHelpers.cxx` |

---

## Key API

```cpp
namespace openstudio {
  // Version strings
  std::string openStudioApplicationVersion();             // MAJOR.MINOR.PATCH[-prerelease+sha]
  std::string openStudioApplicationVersionWithPrerelease(); // MAJOR.MINOR.PATCH[-prerelease]

  // Path resolution
  path getOpenStudioApplicationPath();        // Path to the running executable
  path getOpenStudioApplicationDirectory();   // Directory containing the executable
  path getOpenStudioApplicationSourceDirectory(); // Resources/source directory (build or install)
  path getOpenStudioApplicationBuildDirectory();  // Build directory (empty in installed builds)
  path getOpenStudioApplicationModule();      // Path to the OpenStudio SDK shared library
  path getOpenStudioApplicationModuleDirectory(); // Directory of the SDK shared library
  path getOpenStudioCoreCLI();                // Path to the OpenStudio CLI executable

  // Build-tree detection
  bool isOpenStudioApplicationRunningFromBuildDirectory();
  bool isOpenStudioApplicationModuleRunningFromBuildDirectory();
}
```

All path functions resolve relative to the running executable, so they work correctly in both development build trees and installed packages on all platforms.

---

## External Dependencies

| Dependency | Usage |
|---|---|
| **OpenStudio SDK** (`openstudio/utilities/core/Path.hpp`) | `openstudio::path` type alias for `std::filesystem::path` |
| **Qt 6** (`QCoreApplication`) | `applicationDirPath()` for relative path resolution |

---

## Internal Dependencies

None — this is the lowest-dependency module in the application.

---

## Patterns & Conventions

- **CMake `configure_file`** — the `.cxx.in` file contains `@VARIABLE@` placeholders that CMake fills in at configure time based on install prefix, SDK path, and platform. This is how install-time paths are baked in without hardcoding.
- **No DLL export macro** — the target is explicitly `STATIC`; all symbols are directly linked into the executable and require no `__declspec(dllexport)` decoration.

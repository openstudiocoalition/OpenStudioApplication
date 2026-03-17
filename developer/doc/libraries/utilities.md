# Library: `utilities` — Path Helpers

> **Source:** `src/utilities/`  
> **CMake target:** `openstudio_apputils` (header/static)  
> **Back to:** [Architecture Overview](../architecture.md)

## Purpose

The `utilities` module is a minimal helper library that resolves runtime filesystem paths to key components: the OpenStudio CLI, EnergyPlus, Radiance, and the application's own resources directory.

Because the application is packaged differently on each platform (macOS bundle, Windows installer tree, Linux deb), path resolution must be centralised and aware of the deployment layout. This module provides that abstraction.

---

## Files

| File | Description |
|---|---|
| `OpenStudioApplicationPathHelpers.hpp` | Public API — declares free functions for locating SDK CLI, EnergyPlus, Radiance, and resource paths |
| `OpenStudioApplicationPathHelpers.cxx.in` | CMake template — `configure_file` substitutes the actual install-time paths at build time, producing `OpenStudioApplicationPathHelpers.cxx` |
| `OpenStudioApplicationUtilitiesAPI.hpp` | DLL export macro (`OPENSDTUDIOAPPLICATIONUTILITIES_API`) for Windows shared library symbol visibility |

---

## Key API

```cpp
namespace openstudio {
  // Returns the path to the OpenStudio CLI executable
  path getOpenStudioCoreCLI();

  // Returns the path to the application's shared resources directory
  path getOpenStudioApplicationSourceDirectory();

  // Returns the path to the EnergyPlus executable (from SDK install layout)
  boost::optional<path> findEnergyPlus();

  // Returns the path to the Radiance installation (from SDK install layout)
  boost::optional<path> findRadiance();
}
```

Paths are resolved relative to the running executable (`QCoreApplication::applicationDirPath()`), making them work correctly whether the app is run from a development build tree or an installed package.

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
- **`boost::optional` returns** — EnergyPlus and Radiance are optional; functions return `boost::none` if the expected path does not exist on disk, allowing callers to gracefully degrade.

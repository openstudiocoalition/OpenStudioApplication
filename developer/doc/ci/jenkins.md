# CI: Jenkins Pipelines

> **Back to:** [CI/CD Overview](overview.md)

## Overview

Three Jenkinsfiles provide integration with NREL's internal Jenkins infrastructure (`cbci`). Each file is minimal — it simply selects a function from the shared library `cbci_shared_libs` and calls it. All build logic lives in that external library.

---

## Files

| Jenkinsfile | Library Branch | Library Function |
|---|---|---|
| `Jenkinsfile_linux` | `@updateLibs` branch of `cbci_shared_libs` | `openstudio_app_incr_linux()` |
| `Jenkinsfile_osx` | default (`@cbci_shared_libs`) | `openstudio_app_incr_osx()` |
| `Jenkinsfile_windows` | default (`@cbci_shared_libs`) | `openstudio_app_incr_windows()` |

---

## Trigger Condition

All three pipelines share the same guard:

```groovy
if (env.CHANGE_ID && env.CHANGE_TARGET) {
    // Run — this is a PR build
} else {
    // Skip — this is a branch push; handled by GitHub Actions
}
```

`CHANGE_ID` is set by the Jenkins GitHub plugin only when building a Pull Request. This means the Jenkins pipelines run **exclusively on PRs** and do not duplicate the push-triggered GitHub Actions builds.

---

## Relationship to GitHub Actions

| Metric | GitHub Actions (`app_build.yml`) | Jenkins |
|---|---|---|
| Trigger | PR + push + tags | PR only |
| Platforms | 5 (Ubuntu 22/24, Windows, macOS Intel/ARM) | Internal NREL runners |
| Code signing | Yes (on tags) | No |
| Release upload | Yes | No |
| Purpose | Official artifacts + release | Supplemental incremental check |

Jenkins builds serve as an additional quality gate for NREL contributors without duplicating the official release workflow.

---

## Shared Library

The `cbci_shared_libs` library is hosted at `github.com/NREL/cbci_jenkins_libs` (internal). The `openstudio_app_incr_*` functions typically:
1. Check out the PR branch
2. Set up the build environment (Conan, CMake, compiler)
3. Run a build (`cmake --build`)
4. Run tests (`ctest`)
5. Report pass/fail back to the PR status check

The exact steps are defined in the external shared library, not in this repository.

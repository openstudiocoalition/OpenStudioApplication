---
description: "Use when reviewing pull requests, conducting code review, or checking a diff for correctness. Covers documentation accuracy checks for developer/doc/."
---

# Pull Request Review Guidelines

## Documentation Accuracy Check

On every pull request review, verify that `developer/doc/` documentation is consistent with the code changes:

1. **Library docs** (`developer/doc/libraries/*.md`): If any `CMakeLists.txt` under `src/` changed (new dependencies, renamed targets, moved files), check the corresponding library doc for stale dependency tables, incorrect CMake target names, or missing/extra class entries.

2. **Class docs** (`developer/doc/classes/**/*.md`): If a documented class's `.hpp` changed (new methods, changed signals/slots, updated inheritance), check that the class diagram, public API table, and Signals/Slots table still match the header.

3. **Architecture overview** (`developer/doc/architecture.md`): If the top-level `CMakeLists.txt` or any module `CMakeLists.txt` changes the dependency graph, verify the Module Dependency Graph flowchart and Module Index table are still accurate.

4. **Class moved between modules**: If a class moved from one `src/` subdirectory to another, check that:
   - The class doc's `Module:` header and `Header:` path are updated
   - The class doc's `Library doc:` backlink points to the new library
   - The old library doc no longer lists the class in its Class Documentation index
   - The new library doc adds the class to its Class Documentation index

5. **Class deleted**: If a class is removed, its class doc should be deleted and its entry removed from the library doc index.

6. **New class added**: If a new class matching the documentation heuristic (see `developer/doc/` prompt) is introduced, flag it for documentation.

> For the full documentation maintenance procedure, see [`.github/prompts/update-docs.prompt.md`](../prompts/update-docs.prompt.md).

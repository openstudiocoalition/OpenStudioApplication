---
description: "Use when reviewing pull requests, conducting code review, or checking a diff for correctness. Covers documentation accuracy checks for developer/doc/."
---

# Pull Request Review Guidelines

On every PR review, verify documentation and comments are consistent with the code changes:

**`developer/doc/` docs**
- `CMakeLists.txt` changed → check the corresponding `libraries/*.md` for stale dependency tables or target names; check `architecture.md` dependency graph.
- Class added/moved/deleted → check library doc's Key Classes section is accurate and the header has a `/** */` doc comment.

**Inline comments**
- Significant class added or refactored (`src/**/*.hpp`) → `/** */` comment above the class must exist, describe current responsibility, and contain no stale references.
- Workflow added or modified (`.github/workflows/*.yml`) → `#` comment block at top must accurately reflect purpose, triggers, and required secrets.
- CI script added or modified (`ci/*.sh`, `ci/*.py`, `ci/*.qs`) → header comment must accurately describe usage, arguments, and exit codes.

> Full standards: [`.github/prompts/update-docs.prompt.md`](../prompts/update-docs.prompt.md)

---
description: "Use when building, testing, or validating C++ changes. Covers how to compile the project inside the Docker build container using Makefile targets."
applyTo: "src/**/*.cpp,src/**/*.hpp,src/**/CMakeLists.txt,docker/**,Makefile"
---

# Building and Testing with the Docker Build Container

All compilation and testing happens inside the Docker container.  
Use `make` targets from the project root to build, test, and validate changes.  The Makefile handles all Docker invocation.

Real builds are still made on native host machines directly.  The docker builds are used for testing and validation, not for production releases.  The Docker image is a convenient, consistent environment for all developers to run builds and tests without needing to set up Qt, Conan, or the OpenStudio SDK on their local machines.

---

## Prerequisites

Docker must be running.  On a fresh checkout, build the image once:

```bash
make image
```

This is slow (~20 min) because it bakes Qt 6.11.0 and the OpenStudio SDK into
the image.  Re-run only when `docker/Dockerfile` changes.

---

## Typical workflow

### First time after checkout (or after `make clean`)

```bash
make configure   # Conan install + cmake configure
make build       # Compile
make test        # Run all CTest tests
```

### Incremental build after editing source files

```bash
make build       # Recompiles only changed translation units (ccache accelerated)
make test        # Re-runs tests
```

### After changing `conanfile.py` or `CMakeLists.txt`

```bash
make configure   # Re-run conan + cmake before building
make build
make test
```

---

## All available targets

| Target | When to use |
|--------|-------------|
| `make image` | Rebuild the Docker image (Dockerfile changed) |
| `make configure` | After checkout, after `conanfile.py`/`CMakeLists.txt` changes, or after `make clean` |
| `make build` | After any source file edit |
| `make test` | Validate correctness; starts Xvfb at `:99`, runs `ctest -j4 --test-timeout 120 --exclude-regex GithubRelease` |
| `make cppcheck` | Static analysis; output written to `build/cppcheck-results.txt` |
| `make run-app` | Launch the compiled OpenStudioApp GUI (requires WSLg/Linux/macOS+XQuartz) |
| `make check-build` | Bash shell with all volumes mounted — inspect build artifacts, run commands against your actual source and build output |
| `make attach` | `/bin/sh` with **no volumes mounted** — inspect the image itself (e.g. verify Qt at `/opt/Qt`, check `/opt/openstudio-sdk`) |
| `make clean` | Wipe the build volume (keeps Conan + ccache volumes); alias for `build-clean` |
| `make build-clean` | Destroy and recreate the build volume (empty slate) |
| `make image-clean` | Remove the Docker image |
| `make volumes-clean` | Destroy all named volumes — build, Conan, and ccache (forces complete rebuild) |

---

## Running make commands on Windows

All `make` targets require a Linux-compatible shell because they invoke Docker
with bash process substitution and Unix-style paths.  On Windows, always run
`make` commands inside **WSL** (Windows Subsystem for Linux), not in
PowerShell or Command Prompt.

When the user is on Windows and asks you to run a `make` target, use a WSL
shell.  For example:

```powershell
wsl make configure
wsl make build
wsl make test
```

Or open a persistent WSL session and run the commands there.  Never run `make` targets directly in a PowerShell or `cmd.exe` terminal on Windows.

`make build` and `make test` can run for many minutes.  Follow these rules to avoid getting stuck not knowing whether a command finished.

**Always capture the exit code explicitly** — append `; echo "EXIT: $?"` so the final line of output is unambiguous even when stdout is truncated:

```bash
wsl bash -c 'cd /mnt/c/repos/osapp3 && make build; echo "EXIT: $?"'
```

**Never pipe Docker commands** — piping (e.g. `| tail -50`, `| grep`) causes SIGPIPE (exit 141) when the consumer exits before Docker finishes.  Redirect stderr into stdout with `2>&1` instead:

```bash
wsl bash -c 'cd /mnt/c/repos/osapp3 && make build 2>&1; echo "EXIT: $?"'
```

**Completion markers to look for in output:**

| Command | Success indicator |
|---------|-------------------|
| `make configure` | `Install finished successfully` + `conan install exit code: 0` |
| `make build` | `cmake --build` exits and `EXIT: 0` is printed |
| `make test` | `100% tests passed` or `EXIT: 0` printed after ctest |

**If output is truncated** and the completion marker is not visible, call `get_terminal_output` again — do not assume the command is still running until at least 5 minutes have elapsed without new output.

---

## Validating a refactoring step

After every logical change (e.g. one item from `developer/doc/refactoring-ideas.md`):

1. **Determine which targets to run** based on what changed:
   - Source files only (`.cpp`/`.hpp`) → `make build && make test`
   - Added or removed source files from a `CMakeLists.txt` → `make configure && make build && make test`
   - Changed `conanfile.py`, `CMakeLists.txt` contents, or any `*.cmake` file → `make configure && make build && make test`

2. **`make build` must exit 0 with zero warnings.** The project builds with `-Werror`; any compiler warning is a build failure.  Fix all warnings before proceeding.

3. **`make test` must exit 0 with no regressions.** All tests that passed before the change must still pass.  The following tests are known baseline failures in the Docker environment and are **not** regressions:

   | Test | Reason |
   |------|--------|
   | `ModelEditorFixture.MorePath_Conversions` | Tests Windows-style backslash path conversion; always fails on Linux |
   | `OpenStudioLibFixture.AnalyticsHelperSecrets` | Requires analytics API secrets injected by CI; always empty in local builds |

   `GithubRelease*` tests are excluded entirely by the Makefile (`--exclude-regex GithubRelease`) and are not part of the baseline.  If `make test` fails, read `build/Testing/Temporary/LastTest.log` to identify which test failed and why before attempting a fix.

4. **Optionally run `make cppcheck`** after any non-trivial structural change (new class, moved logic, changed ownership patterns).  Review `build/cppcheck-results.txt` for new issues introduced by the change.

Do not proceed to the next refactoring step until steps 2 and 3 succeed.

---

## Named Docker volumes

Three named volumes persist data between runs:

| Volume | Mounted at | Contents |
|--------|-----------|----------|
| `osapp-build` | `/workspace/build` | CMake build tree, compiled objects, Ninja database |
| `osapp-conan-cache` | `/conan-cache` | Downloaded Conan packages (`CONAN_HOME`) |
| `osapp-ccache` | `/ccache` | ccache object cache (`CCACHE_DIR`) |

The build volume shadows the host `build/` directory — build output never lands on the host filesystem directly.  Use `make check-build` to inspect volume contents interactively.  `make clean` / `make build-clean` wipes only `osapp-build`; Conan and ccache volumes are preserved.  `make volumes-clean` destroys all three.

---

## Reading build output

Build artifacts and logs land in `build/` (git-ignored).  Key paths inside the container (mapped to the same path on the host):

| Path | Contents |
|------|----------|
| `build/compile_commands.json` | Compilation database (used by cppcheck and clangd) |
| `build/Testing/Temporary/LastTest.log` | CTest output from the most recent run |
| `build/cppcheck-results.txt` | Static analysis output from `make cppcheck` |

---

## Troubleshooting

**`make configure` fails with a Conan error about a missing package**
Run `make volumes-clean` then `make configure` again to rebuild the Conan cache from scratch.

**`make image` fails to pull `ubuntu:22.04`**
Docker Desktop cannot reach `docker.io`.  Restart Docker Desktop or check proxy/VPN settings.  Run `docker pull ubuntu:22.04` to confirm connectivity before retrying.

**Tests fail due to a missing display**
The container includes `xvfb`; `make test` starts a dedicated Xvfb process at `DISPLAY=:99` before invoking ctest, then kills it afterward.  If running ctest manually inside `make check-build`, start Xvfb first:
```bash
Xvfb :99 -screen 0 1280x1024x24 -ac &
export DISPLAY=:99
cd build && ctest -j4 --output-on-failure --test-timeout 120 --exclude-regex GithubRelease
```

---
description: "Use when building, testing, or validating C++ changes. Covers how to compile the project inside the Docker build container using Makefile targets."
applyTo: "src/**/*.cpp,src/**/*.hpp,src/**/CMakeLists.txt,docker/**,Makefile"
---

# Building and Testing with the Docker Build Container

Production release builds are made on native CI machines.  The Docker container is used for local development validation — it provides a consistent environment without needing to install Qt, Conan, or the OpenStudio SDK locally.

When using the Docker container builds, use `make` targets from the project root to build, test, and validate changes.  The Makefile handles all further Docker invocations.

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
| `make check-build` | Bash shell with all volumes mounted - inspect build artifacts, run commands against your actual source and build output |
| `make attach` | `/bin/sh` with **no volumes mounted** - inspect the image itself (e.g. verify Qt at `/opt/Qt`, check `/opt/openstudio-sdk`) |
| `make clean` | Wipe the build volume (keeps Conan + ccache volumes); alias for `build-clean` |
| `make build-clean` | Destroy and recreate the build volume (empty slate) |
| `make image-clean` | Remove the Docker image |
| `make volumes-clean` | Destroy all named volumes - build, Conan, and ccache (forces complete rebuild) |

---

## Agent Instructions

The sections below contain behavioral rules for the agent. Follow "Platform-specific command execution" to choose the right syntax, then refer to "Common build validation rules" and "Validating a refactoring step" for testing requirements. All remaining sections are reference documentation.

---

## Platform-specific command execution

**Always determine the user's OS before generating `make` commands.** Look for these indicators:

| OS | Indicators |
|----|-----------|
| **Windows** | PowerShell prompts, Windows paths like `C:\`, explicit mention of Windows |
| **Linux** | Unix paths like `/home/`, bash prompts, explicit mention of Linux/Ubuntu/Debian/Fedora |
| **macOS** | Unix paths like `/Users/`, explicit mention of macOS/Mac |

**If the OS is unknown**, ask the user before generating commands.

---

## Common build validation rules

`make build` and `make test` can run for many minutes. Follow these rules to avoid getting stuck not knowing whether a command finished.

**Always capture the exit code explicitly** by appending `; echo "EXIT: $?"` (bash/Linux/macOS) or ``; echo 'EXIT: '`$?`` (PowerShell/Windows) so the final line of output is unambiguous even when stdout is truncated.

**Never pipe Docker commands** - piping (e.g. `| tail -50`, `| grep`) causes SIGPIPE (exit 141) when the consumer exits before Docker finishes. Redirect stderr into stdout with `2>&1` instead.

**Completion markers to look for in output:**

| Command | Success indicator |
|---------|-------------------|
| `make configure` | `Install finished successfully` + `conan install exit code: 0` |
| `make build` | `cmake --build` exits and `EXIT: 0` is printed |
| `make test` | `100% tests passed` or `EXIT: 0` printed after ctest |

**If output is truncated** and the completion marker is not visible, call `get_terminal_output` again immediately. Continue polling every 30 seconds. Only conclude the command is still running (and stop polling) if no new output has appeared for 5 consecutive minutes.

---

## Running make commands on Windows

All `make` targets require a Linux-compatible shell because they invoke Docker
with bash process substitution and Unix-style paths.  On Windows, always run
`make` commands inside **WSL** (Windows Subsystem for Linux), not in
PowerShell or Command Prompt.

### Command syntax

Wrap all `make` commands in `wsl bash -lc`. Set `PROJECT_ROOT` to the WSL path before running commands:

```powershell
$env:PROJECT_ROOT = '/mnt/c/repos/osapp'
wsl bash -lc "cd $env:PROJECT_ROOT && make configure"
wsl bash -lc "cd $env:PROJECT_ROOT && make build"
wsl bash -lc "cd $env:PROJECT_ROOT && make test"
```

**Path conversion:** Windows path `c:\repos\osapp` maps to `/mnt/c/repos/osapp` in WSL. To convert any Windows path to WSL, replace the drive letter and colon with `/mnt/<drive>` and flip backslashes to forward slashes (e.g. `D:\projects\osapp` → `/mnt/d/projects/osapp`).

**Windows command template** combining exit-code capture and stderr redirect:

```powershell
$env:PROJECT_ROOT = '/mnt/c/repos/osapp'
wsl bash -lc "cd $env:PROJECT_ROOT && make <target> 2>&1; echo 'EXIT: '`$?"
```

Alternatively, open a persistent WSL session and run commands directly there. Never run `make` targets in PowerShell or `cmd.exe`.

### Prerequisites: Install Ubuntu 22.04 in WSL

This project's local Windows workflow should match the Linux CI environment (`ubuntu-22.04`).

1. In PowerShell, install Ubuntu 22.04 in WSL:
   ```powershell
   wsl --install -d Ubuntu-22.04
   ```
1. Confirm it is installed:
   ```powershell
   wsl -l -v
   ```
1. Set Ubuntu-22.04 as the default distro:
   ```powershell
   wsl --set-default Ubuntu-22.04
   ```
1. Start Docker Desktop on Windows.
1. In Docker Desktop, enable WSL integration for Ubuntu-22.04:
   - Open **Settings > Resources > WSL Integration**.
   - Turn on **Enable integration with additional distros**.
   - Enable **Ubuntu-22.04**.
1. In PowerShell, verify Docker Desktop is reachable from WSL:
   ```powershell
   wsl bash -lc 'docker version'
   ```
1. In PowerShell, run a container pull test from WSL:
   ```powershell
   wsl bash -lc 'docker pull ubuntu:22.04'
   ```
1. Open the distro:
   ```powershell
   wsl
   ```
1. In Ubuntu, update package indexes:
   ```bash
   sudo apt update
   ```
1. Install Make:
   ```bash
   sudo apt install -y make
   ```
1. Verify installation:
   ```bash
   make --version
   ```
1. Verify Docker access in Ubuntu:
   ```bash
   docker info
   ```

If `docker info` fails with a permission error on `/var/run/docker.sock`, run:

```bash
sudo usermod -aG docker $USER
newgrp docker
docker info
```

---

## Running make commands on Linux/macOS

On native Linux or macOS, run `make` targets directly in your terminal without any WSL wrapper.

### Command syntax

Navigate to the project root and run commands directly:

```bash
cd /path/to/osapp
make configure
make build
make test
```

**Command template with exit code capture:**

```bash
cd /path/to/osapp && make <target>; echo "EXIT: $?"
```

### Prerequisites

**Linux (Ubuntu/Debian):**
- Docker must be installed and running
- Install `make`: `sudo apt install make`
- Add user to docker group: `sudo usermod -aG docker $USER && newgrp docker`

**macOS:**
- Docker Desktop must be installed and running
- `make` is pre-installed via Xcode Command Line Tools

**macOS GUI testing (optional):**

To use `make run-app` on macOS:
1. Install XQuartz: `brew install --cask xquartz`
2. Open XQuartz preferences → enable "Allow connections from network clients"
3. Restart XQuartz
4. Set `DISPLAY=host.docker.internal:0` before running `make run-app`

---

## Validating a refactoring step

After every logical change (e.g. one item from `developer/doc/refactoring-ideas.md`):

1. **Determine which targets to run** based on what changed:
   - Source files only (`.cpp`/`.hpp`) → `make build && make test`
   - Added or removed source files from a `CMakeLists.txt` → `make configure && make build && make test`
   - Changed `conanfile.py`, `CMakeLists.txt` contents, or any `*.cmake` file → `make configure && make build && make test`

   If changes fall into multiple categories, use the highest-priority category that applies, where priority order is: **(1)** `conanfile.py`/`CMakeLists.txt`/`*.cmake` changes → **(2)** `CMakeLists.txt` source additions/removals → **(3)** source-only.  When in doubt, run `make configure && make build && make test`.

1. **`make build` must exit 0 with zero warnings.** The project builds with `-Werror`; any compiler warning is a build failure.  Fix all warnings before proceeding.

1. **`make test` must exit 0 with no regressions.** All tests that passed before the change must still pass.  The following tests are known baseline failures in the Docker environment and are **not** regressions:

   | Test | Reason |
   |------|--------|
   | `ModelEditorFixture.MorePath_Conversions` | Tests Windows-style backslash path conversion; always fails on Linux |
   | `OpenStudioLibFixture.AnalyticsHelperSecrets` | Requires analytics API secrets injected by CI; always empty in local builds |

   `GithubRelease*` tests are excluded entirely by the Makefile (`--exclude-regex GithubRelease`) and are not part of the baseline.  If `make test` fails, read `build/Testing/Temporary/LastTest.log` to identify which test failed and why before attempting a fix.

   If a test fails that is not in the known-baseline table and does not appear related to your change, re-run `make test` once to rule out flakiness.  If it fails again, check `git log` on that test file.  If the test was already failing on `main` before your change, document it and continue; otherwise treat it as a regression to fix.

1. **Optionally run `make cppcheck`** after any non-trivial structural change (new class, moved logic, changed ownership patterns).  Review `build/cppcheck-results.txt` for new issues introduced by the change.

Do not proceed to the next refactoring step until steps 2 and 3 succeed.

---

## Named Docker volumes

Three named volumes persist data between runs:

| Volume | Mounted at | Contents |
|--------|-----------|----------|
| `osapp-build` | `/workspace/build` | CMake build tree, compiled objects, Ninja database |
| `osapp-conan-cache` | `/conan-cache` | Downloaded Conan packages (`CONAN_HOME`) |
| `osapp-ccache` | `/ccache` | ccache object cache (`CCACHE_DIR`) |

The build volume shadows the host `build/` directory - build output never lands on the host filesystem directly.  Use `make check-build` to inspect volume contents interactively.  `make clean` / `make build-clean` wipes only `osapp-build`; Conan and ccache volumes are preserved.  `make volumes-clean` destroys all three.

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

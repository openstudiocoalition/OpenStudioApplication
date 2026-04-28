---
description: "Use when building, testing, or validating C++ changes. Covers how to compile the project inside the Docker build container using Makefile targets."
applyTo: "src/**/*.cpp,src/**/*.hpp,src/**/CMakeLists.txt,docker/**,Makefile"
---

# Building and Testing with the Docker Build Container

All compilation and testing happens inside the Docker container.  Never run
`cmake`, `conan`, `ninja`, or `ctest` directly on the host.  Use `make`
targets from the project root instead.

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
| `make test` | Validate correctness; runs `xvfb-run ctest -j` for headless Qt tests |
| `make cppcheck` | Static analysis; output written to `build/cppcheck-results.txt` |
| `make shell` | Interactive bash shell inside the container for debugging |
| `make clean` | Delete `build/` (keeps Conan + ccache volumes) |
| `make volumes-clean` | Destroy Conan and ccache named volumes (forces full dependency rebuild) |

---

## Validating a refactoring step

After every logical change (e.g. one item from `developer/doc/refactoring-ideas.md`):

1. `make build` — must exit 0 with no new warnings.
2. `make test` — all previously passing tests must still pass.
3. If a header or CMakeLists.txt was modified: `make configure && make build && make test`.

Do not proceed to the next refactoring step until both commands succeed.

---

## Reading build output

Build artefacts and logs land in `build/` (git-ignored).  Key paths inside
the container (mapped to the same path on the host):

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
Docker Desktop cannot reach `docker.io`.  Restart Docker Desktop or check
proxy/VPN settings.  Run `docker pull ubuntu:22.04` to confirm connectivity
before retrying.

**Tests fail due to a missing display**
The container includes `xvfb`; `make test` already wraps ctest with
`xvfb-run`.  If running commands manually inside `make shell`, prefix with
`xvfb-run` for any test that opens a Qt window.

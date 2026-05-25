# =============================================================================
# OpenStudio Application - Docker-based build system
#
# Prerequisites:  Docker (with BuildKit enabled) + GNU make (or WSL/Git Bash
#                 on Windows).
#
# Quick start:
#   make image        # Build the Docker image (once, ~20 min)
#   make configure    # Install Conan deps + CMake configure
#   make build        # Compile
#   make test         # Run CTest
#   make check-build  # Drop into an interactive container shell
# =============================================================================

# ---------------------------------------------------------------------------
# Git Bash/MSYS2: Prevent unwanted path conversion for Docker
# If running in Git Bash/MSYS2, export MSYS_NO_PATHCONV=1 to avoid issues with
# Docker volume and working directory paths. This is a no-op in other shells.
ifeq ($(shell uname -s | grep -iE 'mingw|msys|cygwin'),)
else
export MSYS_NO_PATHCONV=1
endif

IMAGE     := osapp-build
TAG       := latest
BUILD_DIR := build

# Named volumes - persist Conan packages, ccache, and build artifacts between runs.
# The build volume is mounted at /workspace/build inside the container, shadowing
# any host build/ directory.  This gives Linux-native filesystem performance for
# incremental Ninja builds and avoids file-ownership noise on Windows hosts.
CONAN_VOL  := osapp-conan-cache
CCACHE_VOL := osapp-ccache
BUILD_VOL  := osapp-build

# Qt install dir inside the image (matches Dockerfile ENV).
QT_INSTALL_DIR := /opt/Qt/6.11.0/gcc_64

# Extra commands after `make attach` are treated as a command to run in the image.
# Example: make attach env
ATTACH_ARGS := $(filter-out attach,$(MAKECMDGOALS))
CHECK_BUILD_ARGS := $(filter-out check-build,$(MAKECMDGOALS))

# When `attach` or `check-build` is invoked with extra commands (eg:
# `make attach env` or `make check-build env`), prevent make from treating
# those commands as unknown targets.
ifneq (,$(filter attach,$(MAKECMDGOALS)))
$(foreach goal,$(ATTACH_ARGS),$(eval .PHONY: $(goal)))
$(foreach goal,$(ATTACH_ARGS),$(eval $(goal): ; @:))
endif
ifneq (,$(filter check-build,$(MAKECMDGOALS)))
$(foreach goal,$(CHECK_BUILD_ARGS),$(eval .PHONY: $(goal)))
$(foreach goal,$(CHECK_BUILD_ARGS),$(eval $(goal): ; @:))
endif

# ---------------------------------------------------------------------------
# Base docker run command (non-interactive, workspace mounted as /workspace).
# Runs as root so build artifacts have consistent ownership.
# The build volume is mounted over /workspace/build so the host never sees
# raw build output; use 'make check-build' to inspect artifacts interactively.
# ---------------------------------------------------------------------------
DOCKER_RUN := docker run --rm \
	-v "$(CURDIR):/workspace" \
	-v "$(BUILD_VOL):/workspace/build" \
	-v "$(CONAN_VOL):/conan-cache" \
	-v "$(CCACHE_VOL):/ccache" \
	-e CONAN_HOME=/conan-cache \
	-e CCACHE_DIR=/ccache \
	-e QT_INSTALL_DIR=$(QT_INSTALL_DIR) \
	-w /workspace \
	$(IMAGE):$(TAG)

.PHONY: all image volumes configure build test cppcheck run-app check-build attach \
        clean image-clean volumes-clean build-clean help

all: help

# ---------------------------------------------------------------------------
# image - Build the Docker image (slow; only needed when Dockerfile changes).
# ---------------------------------------------------------------------------
image:
	docker build -t $(IMAGE):$(TAG) docker/

# ---------------------------------------------------------------------------
# volumes - Ensure all named volumes exist.
# ---------------------------------------------------------------------------
volumes:
	docker volume inspect $(CONAN_VOL)  > /dev/null 2>&1 || docker volume create $(CONAN_VOL)
	docker volume inspect $(CCACHE_VOL) > /dev/null 2>&1 || docker volume create $(CCACHE_VOL)
	docker volume inspect $(BUILD_VOL)  > /dev/null 2>&1 || docker volume create $(BUILD_VOL)

# ---------------------------------------------------------------------------
# configure - Bootstrap Conan, symlink SDK, run conan install + cmake.
#             Re-run whenever conanfile.py or CMakeLists.txt changes.
# ---------------------------------------------------------------------------
configure: volumes
	$(DOCKER_RUN) bash /workspace/docker/configure.sh

# ---------------------------------------------------------------------------
# build - Compile (uses Ninja + ccache; incremental).
# ---------------------------------------------------------------------------
build: volumes
	$(DOCKER_RUN) cmake --build --preset conan-docker -j

# ---------------------------------------------------------------------------
# test - Run CTest inside the build directory.
#         Xvfb is started manually so all parallel test processes share the
#         same display (xvfb-run only wraps a single process, which is
#         insufficient when ctest spawns many children in parallel).
#
#         Per-test timeout notes:
#           All tests have TIMEOUT 660 set in CTestTestfile.cmake.
#           ctest --timeout only sets the *default* for tests that have no
#           timeout property, so it does not override 660.
#           cmake 3.27+ ctest --test-timeout DOES override per-test timeouts.
#           We use --test-timeout 120 so no single test can block the suite.
#
#         Excluded tests:
#           GithubRelease* - make live HTTP calls to the GitHub releases API
#           which hang or fail in a network-sandboxed container.
# ---------------------------------------------------------------------------
test: build
	$(DOCKER_RUN) bash -c "\
	  Xvfb :99 -screen 0 1280x1024x24 -ac &\
	  XVFB_PID=$$! ;\
	  export DISPLAY=:99 ;\
	  export QT_QPA_PLATFORM=xcb ;\
	  sleep 1 ;\
	  cd build && ctest -j4 \
	    --output-on-failure \
	    --test-timeout 120 \
	    --exclude-regex 'GithubRelease' \
	  ; CTEST_EXIT=$$? ;\
	  kill $$XVFB_PID 2>/dev/null || true ;\
	  exit $$CTEST_EXIT"

# ---------------------------------------------------------------------------
# cppcheck - Static analysis (matches CI cppcheck.yml flags).
#            Requires build/ to exist for compile_commands.json.
#            Output written to build/cppcheck-results.txt.
# ---------------------------------------------------------------------------
cppcheck: build
	$(DOCKER_RUN) bash -c \
	  "cppcheck \
	    --std=c++20 \
	    --suppress=useStlAlgorithm \
	    --inline-suppr \
	    --inconclusive \
	    --enable=all \
	    --library=qt \
	    --project=build/compile_commands.json \
	    2>&1 | tee build/cppcheck-results.txt"

# ---------------------------------------------------------------------------
# run-app - Launch the compiled OpenStudioApp with GUI forwarded to the host.
#
#   Automatically detects the host platform:
#
#   WSL2/WSLg (Windows 11):
#     Uses WSLg's X11 socket (/tmp/.X11-unix) and Wayland runtime
#     (/mnt/wslg).  No extra software needed.
#     Override display: make run-app DISPLAY=:1
#
#   Linux (native):
#     Mounts /tmp/.X11-unix and uses the host DISPLAY.  Runs xhost +local:docker
#     first to grant the container access.
#     Override display: make run-app DISPLAY=:1
#
#   macOS:
#     Requires XQuartz (https://xquartz.org).  Start XQuartz and run:
#       xhost + 127.0.0.1
#     Then: make run-app
#     DISPLAY is set to host.docker.internal:0 automatically.
#
#   If on native Linux and GPU passthrough is unavailable (--device /dev/dri
#   fails), add: make run-app LIBGL_ALWAYS_SOFTWARE=1
# ---------------------------------------------------------------------------
APP_BIN := /workspace/build/Products/OpenStudioApp

# Detect host OS.  'uname -s' is available on Linux, macOS, and WSL.
# On native Windows (no WSL) this will be empty - unsupported directly.
_UNAME := $(shell uname -s 2>/dev/null)

# Detect WSL2: /mnt/wslg exists only inside the WSL2 VM.
_IS_WSL := $(shell test -d /mnt/wslg && echo 1 || echo 0)

ifeq ($(_UNAME),Darwin)
  # macOS: XQuartz listens on host.docker.internal:0
  DISPLAY ?= host.docker.internal:0
  _X11_MOUNTS  :=
  _WSLG_MOUNTS :=
  _DRI_DEVICE  :=
  _DISPLAY_ENV := -e DISPLAY=$(DISPLAY)
else ifeq ($(_IS_WSL),1)
  # WSL2 + WSLg: GPU is handled by the WSLg Wayland compositor via /dev/dxg;
  # no --device /dev/dri needed (that device doesn't exist in the WSL2 VM).
  DISPLAY ?= :0
  _X11_MOUNTS  := -v /tmp/.X11-unix:/tmp/.X11-unix
  _WSLG_MOUNTS := -v /mnt/wslg:/mnt/wslg \
                  -e WAYLAND_DISPLAY=wayland-0 \
                  -e XDG_RUNTIME_DIR=/mnt/wslg/runtime-dir \
                  -e PULSE_SERVER=/mnt/wslg/PulseServer
  _DRI_DEVICE  :=
  _DISPLAY_ENV := -e DISPLAY=$(DISPLAY)
else
  # Native Linux
  DISPLAY ?= $(shell echo $$DISPLAY)
  _X11_MOUNTS  := -v /tmp/.X11-unix:/tmp/.X11-unix
  _WSLG_MOUNTS :=
  _DRI_DEVICE  := --device /dev/dri
  _DISPLAY_ENV := -e DISPLAY=$(DISPLAY)
endif

run-app: volumes
ifeq ($(_UNAME),Linux)
ifneq ($(_IS_WSL),1)
	xhost +local:docker 2>/dev/null || true
endif
endif
	docker run --rm -it \
		-v "$(CURDIR):/workspace" \
		-v "$(BUILD_VOL):/workspace/build" \
		-v "$(CONAN_VOL):/conan-cache" \
		-v "$(CCACHE_VOL):/ccache" \
		$(_X11_MOUNTS) \
		$(_WSLG_MOUNTS) \
		$(_DISPLAY_ENV) \
		$(_DRI_DEVICE) \
		-e CONAN_HOME=/conan-cache \
		-e CCACHE_DIR=/ccache \
		-e QT_INSTALL_DIR=$(QT_INSTALL_DIR) \
		-e QT_QPA_PLATFORM=xcb \
		-w /workspace \
		$(IMAGE):$(TAG) \
		$(APP_BIN)

# ---------------------------------------------------------------------------
# check-build - Interactive bash shell inside the container (all volumes mounted).
# ---------------------------------------------------------------------------
check-build: volumes
	@if [ -n "$(strip $(CHECK_BUILD_ARGS))" ]; then \
		docker run --rm -i \
			-v "$(CURDIR):/workspace" \
			-v "$(BUILD_VOL):/workspace/build" \
			-v "$(CONAN_VOL):/conan-cache" \
			-v "$(CCACHE_VOL):/ccache" \
			-e CONAN_HOME=/conan-cache \
			-e CCACHE_DIR=/ccache \
			-e QT_INSTALL_DIR=$(QT_INSTALL_DIR) \
			-w /workspace \
			$(IMAGE):$(TAG) /bin/bash -lc "$(CHECK_BUILD_ARGS)" ; \
	else \
		docker run --rm -it \
			-v "$(CURDIR):/workspace" \
			-v "$(BUILD_VOL):/workspace/build" \
			-v "$(CONAN_VOL):/conan-cache" \
			-v "$(CCACHE_VOL):/ccache" \
			-e CONAN_HOME=/conan-cache \
			-e CCACHE_DIR=/ccache \
			-e QT_INSTALL_DIR=$(QT_INSTALL_DIR) \
			-w /workspace \
			$(IMAGE):$(TAG) /bin/bash ; \
	fi

# ---------------------------------------------------------------------------
# attach - /bin/bash inside the image with NO volume mounts.
#          Use this to debug the image itself (inspect /opt/Qt, /opt/openstudio-sdk, etc.)
# ---------------------------------------------------------------------------
attach:
	@if [ -n "$(strip $(ATTACH_ARGS))" ]; then \
		docker run --rm -i $(IMAGE):$(TAG) /bin/bash -lc "$(ATTACH_ARGS)" ; \
	else \
		docker run --rm -it $(IMAGE):$(TAG) /bin/bash ; \
	fi

# ---------------------------------------------------------------------------
# clean - Wipe the build volume (keeps Conan, ccache, and source).
#          Equivalent to the old 'rm -rf build/'.
#          Also removes any stale host-side build/ directory if present.
# ---------------------------------------------------------------------------
clean: build-clean

# ---------------------------------------------------------------------------
# build-clean - Destroy and recreate the build volume (empty slate).
# ---------------------------------------------------------------------------
build-clean:
	docker volume rm $(BUILD_VOL) || true
	docker volume create $(BUILD_VOL)
	rm -rf $(BUILD_DIR)

# ---------------------------------------------------------------------------
# image-clean - Remove the Docker image.
# ---------------------------------------------------------------------------
image-clean:
	docker rmi $(IMAGE):$(TAG) || true

# ---------------------------------------------------------------------------
# volumes-clean - Destroy all named volumes (forces complete rebuild).
# ---------------------------------------------------------------------------
volumes-clean:
	docker volume rm $(BUILD_VOL) $(CONAN_VOL) $(CCACHE_VOL) || true
	rm -rf $(BUILD_DIR)

# ---------------------------------------------------------------------------
# help - List all targets.
# ---------------------------------------------------------------------------
help:
	@echo ""
	@echo "Available targets:"
	@echo "  image         Build the Docker image (run once after Dockerfile changes)"
	@echo "  configure     Bootstrap Conan + run cmake configure"
	@echo "  build         Compile the project (incremental)"
	@echo "  test          Run CTest"
	@echo "  cppcheck      Static analysis (output -> build/cppcheck-results.txt)"
	@echo "  run-app       Launch OpenStudioApp GUI (WSLg/Linux/macOS+XQuartz)"
	@echo "  check-build   Mounted build shell; run command with: make check-build env"
	@echo "  attach        Image-only /bin/bash (no mounts); run command with: make attach env"
	@echo "  clean         Wipe the build volume (equivalent to rm -rf build/)"
	@echo "  build-clean   Alias for clean"
	@echo "  image-clean   Remove the Docker image"
	@echo "  volumes-clean Destroy all named volumes (build + Conan + ccache)"
	@echo ""
	@echo "Typical first-time workflow:"
	@echo "  make image && make configure && make build && make test"
	@echo ""
	@echo "Note: build artifacts live in the '$(BUILD_VOL)' Docker volume."
	@echo "      Use 'make check-build' to inspect them interactively."
	@echo "      'make clean' wipes the build volume; Conan/ccache are preserved."
	@echo ""

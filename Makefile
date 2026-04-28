# =============================================================================
# OpenStudio Application — Docker-based build system
#
# Prerequisites:  Docker (with BuildKit enabled) + GNU make (or WSL/Git Bash
#                 on Windows).
#
# Quick start:
#   make image        # Build the Docker image (once, ~20 min)
#   make configure    # Install Conan deps + CMake configure
#   make build        # Compile
#   make test         # Run CTest
#   make shell        # Drop into an interactive container shell
# =============================================================================

IMAGE     := osapp2-build
TAG       := latest
BUILD_DIR := build

# Named volumes – persist the Conan package cache and ccache between runs.
CONAN_VOL  := osapp2-conan-cache
CCACHE_VOL := osapp2-ccache

# TODO: consider making build dir a volume instead of a host mount for performance
# TODO: figure out how to launch the app from inside the volume (not sure this is possible on Windows)
# TODO: increase debug and warning verbosity of configure and build steps

# Qt install dir inside the image (matches Dockerfile ENV).
QT_INSTALL_DIR := /opt/Qt/6.11.0/gcc_64

# ---------------------------------------------------------------------------
# Base docker run command (non-interactive, workspace mounted as /workspace).
# Runs as root so build artefacts in build/ have consistent ownership.
# ---------------------------------------------------------------------------
DOCKER_RUN := docker run --rm \
	-v "$(CURDIR):/workspace" \
	-v "$(CONAN_VOL):/conan-cache" \
	-v "$(CCACHE_VOL):/ccache" \
	-e CONAN_HOME=/conan-cache \
	-e CCACHE_DIR=/ccache \
	-e QT_INSTALL_DIR=$(QT_INSTALL_DIR) \
	-w /workspace \
	$(IMAGE):$(TAG)

.PHONY: all image volumes configure build test cppcheck shell attach \
        clean image-clean volumes-clean help

all: help

# ---------------------------------------------------------------------------
# image — Build the Docker image (slow; only needed when Dockerfile changes).
# ---------------------------------------------------------------------------
image:
	docker build -t $(IMAGE):$(TAG) docker/

# ---------------------------------------------------------------------------
# volumes — Ensure named cache volumes exist.
# ---------------------------------------------------------------------------
volumes:
	docker volume inspect $(CONAN_VOL)  > /dev/null 2>&1 || docker volume create $(CONAN_VOL)
	docker volume inspect $(CCACHE_VOL) > /dev/null 2>&1 || docker volume create $(CCACHE_VOL)

# ---------------------------------------------------------------------------
# configure — Bootstrap Conan, symlink SDK, run conan install + cmake.
#             Re-run whenever conanfile.py or CMakeLists.txt changes.
# ---------------------------------------------------------------------------
configure: volumes
	$(DOCKER_RUN) bash /workspace/docker/configure.sh

# ---------------------------------------------------------------------------
# build — Compile (uses Ninja + ccache; incremental).
# ---------------------------------------------------------------------------
build: volumes
	$(DOCKER_RUN) cmake --build --preset conan-release -j

# ---------------------------------------------------------------------------
# test — Run CTest inside the build directory (xvfb-run provides a virtual
#         display for headless Qt tests).
#         Depends on build so an empty build/ gives a clear error message.
# ---------------------------------------------------------------------------
test: build
	$(DOCKER_RUN) bash -c "cd build && xvfb-run ctest -j --output-on-failure --timeout 120"

# ---------------------------------------------------------------------------
# cppcheck — Static analysis (matches CI cppcheck.yml flags).
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
# shell — Interactive bash shell inside the container (workspace mounted).
# ---------------------------------------------------------------------------
shell: volumes
	docker run --rm -it \
		-v "$(CURDIR):/workspace" \
		-v "$(CONAN_VOL):/conan-cache" \
		-v "$(CCACHE_VOL):/ccache" \
		-e CONAN_HOME=/conan-cache \
		-e CCACHE_DIR=/ccache \
		-e QT_INSTALL_DIR=$(QT_INSTALL_DIR) \
		-w /workspace \
		$(IMAGE):$(TAG) bash

# ---------------------------------------------------------------------------
# attach — /bin/sh inside the image with NO volume mounts.
#          Use this to debug the image itself (inspect /opt/Qt, /opt/openstudio-sdk, etc.)
# ---------------------------------------------------------------------------
attach:
	docker run --rm -it $(IMAGE):$(TAG) /bin/sh

# ---------------------------------------------------------------------------
# clean — Remove the build directory (keeps Conan + ccache volumes).
# ---------------------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR)

# ---------------------------------------------------------------------------
# image-clean — Remove the Docker image.
# ---------------------------------------------------------------------------
image-clean:
	docker rmi $(IMAGE):$(TAG) || true

# ---------------------------------------------------------------------------
# volumes-clean — Destroy the Conan and ccache volumes (forces full rebuild).
# ---------------------------------------------------------------------------
volumes-clean:
	docker volume rm $(CONAN_VOL) $(CCACHE_VOL) || true

# ---------------------------------------------------------------------------
# help — List all targets.
# ---------------------------------------------------------------------------
help:
	@echo ""
	@echo "Available targets:"
	@echo "  image         Build the Docker image (run once after Dockerfile changes)"
	@echo "  configure     Bootstrap Conan + run cmake configure"
	@echo "  build         Compile the project (incremental)"
	@echo "  test          Run CTest"
	@echo "  cppcheck      Static analysis (output -> build/cppcheck-results.txt)"
	@echo "  shell         Interactive bash shell inside the build container"
	@echo "  attach        /bin/sh in the image with no mounts (debug image contents)"
	@echo "  clean         Remove build/ directory"
	@echo "  image-clean   Remove the Docker image"
	@echo "  volumes-clean Destroy Conan + ccache named volumes"
	@echo ""
	@echo "Typical first-time workflow:"
	@echo "  make image && make configure && make build && make test"
	@echo ""

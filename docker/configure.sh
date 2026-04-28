#!/usr/bin/env bash
# configure.sh — runs inside the build container.
# Called by:  make configure
# Purpose:    1. Download the OpenStudio SDK into build/ if not already present
#             2. Bootstrap Conan home (first run only)
#             3. Run `conan install` to fetch/build dependencies and generate
#                the CMake toolchain + CMakeUserPresets.json.
#             4. Run `cmake --preset conan-release` to configure the project.
set -euo pipefail

# ── SDK paths (must match FindOpenStudioSDK.cmake) ───────────────────────────
SDK_VERSION="3.11.0"
SDK_SHA="+241b8abb4d"
SDK_PLATFORM="Ubuntu-22.04-x86_64"   # matches FindOpenStudioSDK.cmake: ${LSB_RELEASE_ID_SHORT}-${LSB_RELEASE_VERSION_SHORT}-${ARCH}
SDK_BASENAME="OpenStudio-${SDK_VERSION}${SDK_SHA}-${SDK_PLATFORM}"
SDK_DIR="build/OpenStudio-${SDK_VERSION}"            # created in workspace
SDK_DEST="${SDK_DIR}/${SDK_BASENAME}"                # where CMake looks
SDK_URL="https://github.com/NREL/OpenStudio/releases/download/v${SDK_VERSION}/${SDK_BASENAME}.tar.gz"

# ── Qt ───────────────────────────────────────────────────────────────────────
QT_INSTALL_DIR="${QT_INSTALL_DIR:-/opt/Qt/6.11.0/linux_gcc_64}"

echo "==> [1/4] Checking OpenStudio SDK ..."
if [ -d "${SDK_DEST}" ]; then
    echo "    SDK already present at ${SDK_DEST}"
else
    mkdir -p "${SDK_DIR}"
    echo "    Downloading ${SDK_URL} ..."
    curl -fSL --retry 5 --retry-delay 10 --retry-connrefused "${SDK_URL}" -o "${SDK_DIR}/${SDK_BASENAME}.tar.gz"
    echo "    Extracting ..."
    tar xzf "${SDK_DIR}/${SDK_BASENAME}.tar.gz" -C "${SDK_DIR}"
    echo "    SDK extracted to ${SDK_DEST}"
fi

# ── Conan first-run bootstrap ─────────────────────────────────────────────────
echo "==> [2/4] Bootstrapping Conan ..."
CONAN_HOME="${CONAN_HOME:-${HOME}/.conan2}"
if [ ! -f "${CONAN_HOME}/profiles/default" ]; then
    conan profile detect --force
    # Enforce C++20 and Release build type in the default profile
    sed -i 's/cppstd=.*$/cppstd=20/'    "${CONAN_HOME}/profiles/default"
    sed -i 's/build_type=.*$/build_type=Release/' "${CONAN_HOME}/profiles/default"
    # NREL custom remote (hosts ruby/3.2.2 and other project packages)
    conan remote add --force nrel-v2 \
        https://conan.openstudio.net/artifactory/api/conan/conan-v2
    echo "    Conan profile created."
else
    echo "    Conan profile already exists."
fi

# Always (re)write global.conf so stale values from prior runs are corrected.
mkdir -p "${CONAN_HOME}"
{
    echo "core:non_interactive = True"
    echo "core.download:parallel = 4"
    echo "core.sources:download_cache = ${CONAN_HOME}/.conan-download-cache"
} > "${CONAN_HOME}/global.conf"

# ── ccache setup ──────────────────────────────────────────────────────────────
if command -v ccache &>/dev/null; then
    ccache --max-size=500M
    ccache --set-config=compression=true
fi

# ── Conan install ─────────────────────────────────────────────────────────────
echo "==> [3/4] Running conan install ..."
conan install . \
    --output-folder=./build \
    --build=missing \
    -c tools.cmake.cmaketoolchain:generator=Ninja \
    -s compiler.cppstd=20 \
    -s build_type=Release

# ── CMake configure ───────────────────────────────────────────────────────────
echo "==> [4/4] Running cmake configure ..."
cmake --preset conan-release \
    -DQT_INSTALL_DIR:PATH="${QT_INSTALL_DIR}" \
    -DBUILD_DOCUMENTATION:BOOL=OFF \
    -DBUILD_PACKAGE:BOOL=OFF \
    -DBUILD_TESTING:BOOL=ON \
    -DBUILD_BENCHMARK:BOOL=ON

echo ""
echo "Configure complete. Run 'make build' to compile."

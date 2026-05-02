#!/usr/bin/env bash
# configure.sh — runs inside the build container.
# Called by:  make configure
# Purpose:    1. Download the OpenStudio SDK into build/ if not already present
#             2. Bootstrap Conan home (first run only)
#             3. Run `conan install` to fetch/build dependencies and generate
#                the CMake toolchain + CMakeUserPresets.json.
#             4. Run `cmake --preset conan-release` to configure the project.
set -euo pipefail

# Tee all output to a log file inside the build volume (osapp-build), not the
# host filesystem. Ensures the full log survives PowerShell Tee-Object SIGPIPE.
mkdir -p /workspace/build
exec > >(tee -a /workspace/build/configure-docker.log) 2>&1

# ── Verbose debug header ──────────────────────────────────────────────────────
echo "================================================================"
echo "  configure.sh  —  $(date -u '+%Y-%m-%d %H:%M:%S UTC')"
echo "  Host:     $(uname -a)"
echo "  User:     $(id)"
echo "  Workdir:  $(pwd)"
echo "  CONAN_HOME: ${CONAN_HOME:-<unset, will default>}"
echo "  CCACHE_DIR: ${CCACHE_DIR:-<unset>}"
echo "  QT_INSTALL_DIR: ${QT_INSTALL_DIR:-<unset>}"
echo "  PATH: ${PATH}"
echo "================================================================"

echo "--- Tool versions ---"
echo "  bash:   $(bash --version | head -1)"
echo "  cmake:  $(cmake --version | head -1)"
echo "  conan:  $(conan --version 2>&1 | head -1)"
echo "  ninja:  $(ninja --version 2>&1 || echo 'not found')"
echo "  ccache: $(ccache --version 2>&1 | head -1 || echo 'not found')"
echo "  curl:   $(curl --version | head -1)"
echo "---------------------"

# ── SDK paths (must match FindOpenStudioSDK.cmake) ───────────────────────────
SDK_VERSION="3.11.0"
SDK_SHA="+241b8abb4d"
SDK_PLATFORM="Ubuntu-22.04-x86_64"   # matches FindOpenStudioSDK.cmake: ${LSB_RELEASE_ID_SHORT}-${LSB_RELEASE_VERSION_SHORT}-${ARCH}
SDK_BASENAME="OpenStudio-${SDK_VERSION}${SDK_SHA}-${SDK_PLATFORM}"
SDK_DIR="build/OpenStudio-${SDK_VERSION}"            # created in workspace
SDK_DEST="${SDK_DIR}/${SDK_BASENAME}"                # where CMake looks
SDK_URL="https://github.com/NREL/OpenStudio/releases/download/v${SDK_VERSION}/${SDK_BASENAME}.tar.gz"

echo "  SDK_DEST: ${SDK_DEST}"
echo "  SDK_URL:  ${SDK_URL}"

# ── Qt ───────────────────────────────────────────────────────────────────────
QT_INSTALL_DIR="${QT_INSTALL_DIR:-/opt/Qt/6.11.0/linux_gcc_64}"
echo "  Qt:       ${QT_INSTALL_DIR}"
if [ -d "${QT_INSTALL_DIR}" ]; then
    echo "  Qt dir exists: OK"
else
    echo "  WARNING: Qt dir not found at ${QT_INSTALL_DIR}"
fi

echo "==> [1/4] Checking OpenStudio SDK ..."
if [ -d "${SDK_DEST}" ]; then
    echo "    SDK already present at ${SDK_DEST}"
    echo "    SDK contents (top-level):"
    ls -lh "${SDK_DEST}" | head -20
else
    echo "    SDK not found — downloading ..."
    mkdir -p "${SDK_DIR}"
    echo "    Downloading ${SDK_URL} ..."
    curl -fSL --retry 5 --retry-delay 10 --retry-connrefused \
        --progress-bar \
        "${SDK_URL}" -o "${SDK_DIR}/${SDK_BASENAME}.tar.gz"
    echo "    Download complete. Archive size: $(du -sh "${SDK_DIR}/${SDK_BASENAME}.tar.gz" | cut -f1)"
    echo "    Extracting ..."
    tar xzf "${SDK_DIR}/${SDK_BASENAME}.tar.gz" -C "${SDK_DIR}" --verbose 2>&1 | tail -5
    echo "    SDK extracted to ${SDK_DEST}"
fi

# ── Conan first-run bootstrap ─────────────────────────────────────────────────
echo "==> [2/4] Bootstrapping Conan ..."
CONAN_HOME="${CONAN_HOME:-${HOME}/.conan2}"
echo "    CONAN_HOME resolved to: ${CONAN_HOME}"
if [ ! -f "${CONAN_HOME}/profiles/default" ]; then
    echo "    No default profile found — running 'conan profile detect' ..."
    conan profile detect --force
    # Enforce C++20 and Release build type in the default profile
    sed -i 's/cppstd=.*$/cppstd=20/'    "${CONAN_HOME}/profiles/default"
    sed -i 's/build_type=.*$/build_type=Release/' "${CONAN_HOME}/profiles/default"
    echo "    Profile after edits:"
    cat "${CONAN_HOME}/profiles/default"
    # NREL custom remote (hosts ruby/3.2.2 and other project packages).
    # --insecure disables TLS certificate verification — needed while
    # conan.openstudio.net has an expired certificate.  Remove --insecure once
    # the certificate is renewed.
    conan remote add --force --insecure nrel-v2 \
        https://conan.openstudio.net/artifactory/api/conan/conan-v2
    echo "    Conan profile created."
else
    echo "    Conan profile already exists:"
    cat "${CONAN_HOME}/profiles/default"
fi

# ── Ensure nrel-v2 remote is registered and insecure ─────────────────────────
# conan.openstudio.net currently has an expired TLS certificate; --insecure
# disables cert verification so the build is not blocked.  Remove the
# --insecure flag (and this comment) once the certificate is renewed.
echo "    Checking for nrel-v2 remote ..."
if conan remote list 2>/dev/null | grep -q 'nrel-v2'; then
    echo "    nrel-v2 remote found — ensuring it is enabled and insecure ..."
    conan remote enable nrel-v2
    conan remote update nrel-v2 \
        --url https://conan.openstudio.net/artifactory/api/conan/conan-v2 \
        --insecure
else
    echo "    nrel-v2 remote not registered — adding with --insecure ..."
    conan remote add --insecure nrel-v2 \
        https://conan.openstudio.net/artifactory/api/conan/conan-v2
fi
echo "    Active Conan remotes:"
conan remote list

# Always (re)write global.conf so stale values from prior runs are corrected.
mkdir -p "${CONAN_HOME}"
echo "    Writing ${CONAN_HOME}/global.conf ..."
{
    echo "core:non_interactive = True"
    echo "core.download:parallel = 4"
    echo "core.sources:download_cache = ${CONAN_HOME}/.conan-download-cache"
} > "${CONAN_HOME}/global.conf"
echo "    global.conf written:"
cat "${CONAN_HOME}/global.conf"

# ── ccache setup ──────────────────────────────────────────────────────────────
if command -v ccache &>/dev/null; then
    echo "    ccache found — configuring ..."
    ccache --max-size=500M
    ccache --set-config=compression=true
    echo "    ccache stats before build:"
    ccache --show-stats
else
    echo "    WARNING: ccache not found — builds will not be cached"
fi

# ── Conan install ─────────────────────────────────────────────────────────────
echo "==> [3/4] Running conan install ..."
echo "    conanfile.py: $(head -5 conanfile.py 2>/dev/null || echo 'not found')"
conan install . \
    --output-folder=./build \
    --build=missing \
    -c tools.cmake.cmaketoolchain:generator=Ninja \
    -s compiler.cppstd=20 \
    -s build_type=Release
echo "    conan install exit code: $?"
echo "    build/ contents after conan install:"
ls -lh build/ | grep -v "^total" | head -30

# ── CMake configure ───────────────────────────────────────────────────────────
echo "==> [4/4] Running cmake configure ..."
echo "    Preset: conan-release"
echo "    CMakeUserPresets.json:"
cat CMakeUserPresets.json 2>/dev/null || echo "    (not found)"
cmake --preset conan-release \
    -DQT_INSTALL_DIR:PATH="${QT_INSTALL_DIR}" \
    -DBUILD_DOCUMENTATION:BOOL=OFF \
    -DBUILD_PACKAGE:BOOL=OFF \
    -DBUILD_TESTING:BOOL=ON \
    -DBUILD_BENCHMARK:BOOL=ON \
    --log-level=STATUS
echo "    cmake configure exit code: $?"
echo "    CMakeCache.txt key values:"
grep -E "^(CMAKE_BUILD_TYPE|CMAKE_CXX_COMPILER|CMAKE_MAKE_PROGRAM|QT_INSTALL_DIR|BUILD_TESTING|BUILD_BENCHMARK)" \
    build/CMakeCache.txt 2>/dev/null | sort || echo "    (CMakeCache.txt not found)"

echo ""
echo "================================================================"
echo "Configure complete. Run 'make build' to compile."
echo "================================================================"

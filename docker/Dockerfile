FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

# -- Versions (change here when bumping) -------------------------------------
ARG QT_VERSION=6.11.0
ARG QT_ARCH=linux_gcc_64
# -----------------------------------------------------------------------------

# Layer 1: system packages
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential git curl wget ninja-build ccache \
        cmake python3 python3-pip python3-dev \
        mesa-common-dev libglu1-mesa-dev xvfb \
        libxkbcommon-x11-dev libgl1-mesa-dev chrpath \
        libxcb-icccm4 libxcb-keysyms1 libxcb-xkb1 libxcb-randr0 \
        libxcb-shape0 libxkbcommon-x11-0 libxcb-cursor0 \
        patchelf cppcheck ca-certificates lsb-release libglib2.0-0 \
        libfontconfig1-dev libdbus-1-dev \
        libasound2-dev libnss3-dev libnspr4-dev libxdamage-dev libxcomposite-dev libxrandr-dev libxtst-dev \
    && rm -rf /var/lib/apt/lists/*

# Layer 2: Python tools + Qt 6.11.0
#   Use the PyPI release of aqtinstall (not git master) - the git+https
#   workaround in app_build.yml is only required for Windows Qt 6.11.0 due to
#   a directory-structure change (github.com/miurahr/aqtinstall/issues/1007).
#   Linux is unaffected, so the stable PyPI release works here.
RUN pip3 install --no-cache-dir setuptools --upgrade \
    && pip3 install --no-cache-dir 'conan>2' \
    && pip3 install --no-cache-dir aqtinstall \
    && aqt install-qt \
        --outputdir /opt/Qt \
        linux desktop ${QT_VERSION} ${QT_ARCH} \
        -m qtwebchannel qtwebengine qtwebview qtpositioning qtcharts \
    && rm -rf ~/.cache/pip

# NOTE: The OpenStudio SDK is NOT baked into the image.
#   It is downloaded once by configure.sh (make configure) into the host-side
#   build/ directory, which is mounted at /workspace/build at runtime.
#   FindOpenStudioSDK.cmake finds it there automatically.

# Layer 3: Newer CMake
#   Ubuntu 22.04 ships cmake 3.22, but Conan 2 generates CMakePresets.json
#   version 4 which requires cmake >= 3.23.  Install via pip (small layer,
#   keeps the Qt layer above cached).
RUN pip3 install --no-cache-dir cmake

ENV QT_INSTALL_DIR=/opt/Qt/${QT_VERSION}/gcc_64
ENV PATH="/opt/Qt/${QT_VERSION}/gcc_64/bin:${PATH}"

WORKDIR /workspace

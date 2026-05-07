#!/usr/bin/env python3
"""Download OpenStudio CI builds and optionally update FindOpenStudioSDK.cmake."""

import argparse
import hashlib
import re
import subprocess
import sys
import urllib.request
from dataclasses import dataclass
from pathlib import Path

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

S3_URL = "http://openstudio-ci-builds.s3-website-us-west-2.amazonaws.com"


@dataclass(frozen=True)
class Platform:
    suffix: str  # used in tarball filename, e.g. "Darwin-arm64"
    signed: bool  # True → fetched from /signed/ subdirectory on S3
    cmake_key: tuple  # locates the hash line in FindOpenStudioSDK.cmake:
    #   ("apple", "arm64") | ("apple", "x86_64")
    #   ("unix", "<distro>", "arm64"|"x86_64")
    #   ("win32",)


PLATFORMS = [
    Platform("Ubuntu-22.04-x86_64", signed=False, cmake_key=("unix", "22.04", "x86_64")),
    Platform("Ubuntu-22.04-arm64", signed=False, cmake_key=("unix", "22.04", "arm64")),
    Platform("Ubuntu-24.04-x86_64", signed=False, cmake_key=("unix", "24.04", "x86_64")),
    Platform("Ubuntu-24.04-arm64", signed=False, cmake_key=("unix", "24.04", "arm64")),
    Platform("AlmaLinux-9.7-x86_64", signed=False, cmake_key=("unix", "almalinux")),
    Platform("Darwin-arm64", signed=True, cmake_key=("apple", "arm64")),
    Platform("Darwin-x86_64", signed=True, cmake_key=("apple", "x86_64")),
    Platform("Windows", signed=True, cmake_key=("win32",)),
]

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

SCRIPT_DIR = Path(__file__).resolve().parent
DOWNLOAD_DIR = SCRIPT_DIR.parent / "tmp_download_openstudio"
CMAKE_FILE = SCRIPT_DIR.parent.parent / "FindOpenStudioSDK.cmake"


def build_urls(folder: str, base_name: str) -> list[tuple[Platform, str, str]]:
    """Return (platform, url, local_filename) triples for all platforms."""
    triples = []
    for p in PLATFORMS:
        name = f"{base_name}-{p.suffix}.tar.gz"
        subpath = "signed/" if p.signed else ""
        triples.append((p, f"{S3_URL}/{folder}/{subpath}{name}", name))
    return triples


def http_exists(url: str) -> bool:
    req = urllib.request.Request(url, method="HEAD")
    try:
        with urllib.request.urlopen(req) as resp:
            return resp.status == 200
    except urllib.error.HTTPError:
        return False


def download_file(url: str, filename: str) -> Path:
    dest = DOWNLOAD_DIR / filename
    if dest.exists():
        print(f"Already on disk, skipping: {filename}")
        return dest
    if not http_exists(url):
        print(f"WARNING: not found on S3, skipping: {url}", file=sys.stderr)
        return dest
    print(f"Downloading: {url}")
    subprocess.run(["aria2c", f"--dir={DOWNLOAD_DIR}", url], check=True)
    return dest


def md5(path: Path) -> str:
    h = hashlib.md5()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


# ---------------------------------------------------------------------------
# FindOpenStudioSDK.cmake update
# ---------------------------------------------------------------------------

# FindOpenStudioSDK.cmake block structure (simplified):
#
#   if(APPLE)
#     if(ARCH MATCHES "arm64")
#       set(OPENSTUDIO_EXPECTED_HASH ...)    ← ("apple", "arm64")
#     else()
#       set(OPENSTUDIO_EXPECTED_HASH ...)    ← ("apple", "x86_64")
#   elseif(UNIX)
#     if(... "24.04")
#       if(ARCH MATCHES "arm64")
#         set(OPENSTUDIO_EXPECTED_HASH ...)  ← ("unix", "24.04", "arm64")
#       else()
#         set(OPENSTUDIO_EXPECTED_HASH ...)  ← ("unix", "24.04", "x86_64")
#     elseif(... "22.04")
#       ...                                  ← ("unix", "22.04", ...)
#     elseif(... "AlmaLinux")
#       set(OPENSTUDIO_EXPECTED_HASH ...)    ← ("unix", "almalinux")
#   elseif(WIN32)
#     set(OPENSTUDIO_EXPECTED_HASH ...)      ← ("win32",)

HASH_RE = re.compile(r"(\s*set\(OPENSTUDIO_EXPECTED_HASH\s+)[0-9a-f]+(\))[^\n]*")


def _replace_hash_line(line: str, new_hash: str, suffix: str) -> str:
    return HASH_RE.sub(rf"\g<1>{new_hash}\g<2>  # {suffix}", line)


def update_cmake(hashes: dict[str, str]) -> None:
    """hashes: {platform_suffix: md5_hex}, e.g. {"Darwin-arm64": "abc123..."}"""
    cmake_key_to_suffix = {p.cmake_key: p.suffix for p in PLATFORMS}

    text = CMAKE_FILE.read_text()
    lines = text.splitlines(keepends=True)

    os_block: str | None = None  # "apple" | "unix" | "win32"
    distro: str | None = None  # "24.04" | "22.04" | "almalinux"
    arch: str | None = None  # "arm64" | "x86_64"
    updated: set[str] = set()

    result = []
    for line in lines:
        s = line.strip()

        # ── top-level OS block ───────────────────────────────────────────
        if re.match(r"if\s*\(\s*APPLE\s*\)", s):
            os_block, distro, arch = "apple", None, None
        elif re.match(r"elseif\s*\(\s*UNIX\s*\)", s):
            os_block, distro, arch = "unix", None, None
        elif re.match(r"elseif\s*\(\s*WIN32\s*\)", s):
            os_block, distro, arch = "win32", None, None

        # ── distro sub-block (inside UNIX, if/elseif lines only) ────────
        if os_block == "unix" and re.match(r"(?:elseif|if)\s*\(", s):
            if re.search(r'"24\.04"', s):
                distro, arch = "24.04", None
            elif re.search(r'"22\.04"', s):
                distro, arch = "22.04", None
            elif re.search(r'"AlmaLinux"', s):
                distro, arch = "almalinux", None

        # ── arch sub-block (if/elseif lines only) ───────────────────────
        if re.match(r"(?:elseif|if)\s*\(", s) and re.search(r'ARCH\s+MATCHES\s+"arm64"', s):
            arch = "arm64"
        elif re.match(r"else\s*\(\s*\)", s) and arch == "arm64":
            arch = "x86_64"
        elif re.match(r"endif\s*\(\s*\)", s):
            arch = None  # reset between arch sub-blocks; os_block/distro updated by their own if/elseif

        # ── hash replacement ─────────────────────────────────────────────
        if HASH_RE.search(line):
            cmake_key: tuple | None = None
            if os_block == "win32":
                cmake_key = ("win32",)
            elif os_block == "apple" and arch:
                cmake_key = ("apple", arch)
            elif os_block == "unix" and distro == "almalinux":
                cmake_key = ("unix", "almalinux")
            elif os_block == "unix" and distro and arch:
                cmake_key = ("unix", distro, arch)

            suffix = cmake_key_to_suffix.get(cmake_key) if cmake_key else None
            if suffix and suffix in hashes:
                print(f"  Updating hash for {suffix}: {hashes[suffix]}")
                line = _replace_hash_line(line, hashes[suffix], suffix)
                updated.add(suffix)

        result.append(line)

    missing = set(hashes) - updated
    if missing:
        raise RuntimeError(f"No cmake hash line found for: {', '.join(sorted(missing))}")

    CMAKE_FILE.write_text("".join(result))
    print(f"Updated {CMAKE_FILE}")


def update_cmake_version(version: str, prerelease: str, sha: str) -> None:
    """Update VERSION_MAJOR/MINOR/PATCH, PRERELEASE, and SHA in FindOpenStudioSDK.cmake."""
    major, minor, patch = version.split(".")
    text = CMAKE_FILE.read_text()

    replacements = [
        (r"(set\(OPENSTUDIO_VERSION_MAJOR\s+)\S+(\))", rf"\g<1>{major}\2"),
        (r"(set\(OPENSTUDIO_VERSION_MINOR\s+)\S+(\))", rf"\g<1>{minor}\2"),
        (r"(set\(OPENSTUDIO_VERSION_PATCH\s+)\S+(\))", rf"\g<1>{patch}\2"),
        (r'(set\(OPENSTUDIO_VERSION_PRERELEASE\s+")[^"]*(")', rf"\g<1>{prerelease}\2"),
        (r'(set\(OPENSTUDIO_VERSION_SHA\s+")[^"]*(")', rf"\g<1>+{sha}\2"),
    ]
    for pattern, repl in replacements:
        text = re.sub(pattern, repl, text)

    CMAKE_FILE.write_text(text)
    print(f"Updated version/SHA in {CMAKE_FILE}")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main() -> None:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=(
            "Examples:\n"
            "  # Pre-release build:\n"
            "  %(prog)s --version 3.11.1 --prerelease alpha --sha 32d4f461a0 --folder develop\n"
            "\n"
            "  # Official release (no prerelease tag):\n"
            "  %(prog)s --version 3.11.0 --sha 241b8abb4d --folder 3.11.0\n"
            "\n"
            "  # Download and update FindOpenStudioSDK.cmake:\n"
            "  %(prog)s --version 3.11.1 --prerelease alpha --sha 32d4f461a0 --folder develop --update-cmake\n"
        ),
    )
    parser.add_argument("--version", required=True, help="OpenStudio version, e.g. 3.11.1")
    parser.add_argument(
        "--prerelease",
        default=None,
        help='Pre-release tag without leading "-", e.g. "alpha" or "rc1". Omit for official releases.',
    )
    parser.add_argument("--sha", required=True, help="Commit SHA without leading '+', e.g. 32d4f461a0")
    parser.add_argument("--folder", required=True, help="S3 folder, e.g. develop or 3.11.0")
    parser.add_argument(
        "--update-cmake",
        action="store_true",
        help="After downloading, update FindOpenStudioSDK.cmake with new hashes and version",
    )
    args = parser.parse_args()

    prerelease_tag = f"-{args.prerelease}" if args.prerelease else ""
    base_name = f"OpenStudio-{args.version}{prerelease_tag}+{args.sha}"

    DOWNLOAD_DIR.mkdir(parents=True, exist_ok=True)

    downloaded: dict[str, Path] = {}  # platform suffix → local path
    for platform, url, filename in build_urls(args.folder, base_name):
        path = download_file(url, filename)
        if path.exists():
            downloaded[platform.suffix] = path

    if args.update_cmake:
        print("\nComputing MD5 hashes...")
        hashes = {platform: md5(path) for platform, path in downloaded.items()}
        for platform, h in hashes.items():
            print(f"  {platform}: {h}")
        update_cmake_version(args.version, prerelease_tag, args.sha)
        update_cmake(hashes)


if __name__ == "__main__":
    main()

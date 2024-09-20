import os
import re
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parent.parent


def parse_os_app_version(cmakelists_path: Path):
    content = cmakelists_path.read_text()
    m = re.search("project\(OpenStudioApplication VERSION (\d+\.\d+\.\d+)\)", content)
    version = "X.Y.Z"
    if m:
        version = m.groups()[0]
        print(f"OS_APP_VERSION={version}")

    if "GITHUB_ENV" not in os.environ:
        return
    for out in ["GITHUB_ENV", "GITHUB_OUTPUT"]:
        with open(os.environ[out], "a") as f:
            f.write(f"\nOS_APP_VERSION={version}")


def parse_os_sdk_version(find_sdk_path: Path):
    with open(find_sdk_path, "r") as f:
        content = f.read()

    no_comments_lines = []
    for line in content.splitlines():
        stripped_line = line.strip().split("#")[0]
        if stripped_line:
            no_comments_lines.append(stripped_line)
    content = "\n".join(no_comments_lines)

    m_major = re.search(r"set\(OPENSTUDIO_VERSION_MAJOR (\d+)\)", content)
    m_minor = re.search(r"set\(OPENSTUDIO_VERSION_MINOR (\d+)\)", content)
    m_patch = re.search(r"set\(OPENSTUDIO_VERSION_PATCH (\d+)\)", content)
    m_prerelease = re.search(r'set\(OPENSTUDIO_VERSION_PRERELEASE "(.*)"\)', content)
    m_sha = re.search(r'set\(OPENSTUDIO_VERSION_SHA "(.*?)"\)', content)

    assert m_major, "Unable to find OPENSTUDIO_VERSION_MAJOR"
    assert m_minor, "Unable to find OPENSTUDIO_VERSION_MINOR"
    assert m_patch, "Unable to find OPENSTUDIO_VERSION_PATCH"
    assert m_prerelease, "Unable to find OPENSTUDIO_VERSION_PRERELEASE"
    assert m_sha, "Unable to find OPENSTUDIO_VERSION_SHA"

    OS_SDK_VERSION_MAJOR = m_major.groups()[0]
    OS_SDK_VERSION_MINOR = m_minor.groups()[0]
    OS_SDK_VERSION_PATCH = m_patch.groups()[0]
    OS_SDK_VERSION_PRERELEASE = m_prerelease.groups()[0]
    OS_SDK_VERSION_SHA = m_sha.groups()[0]

    OS_SDK_VERSION = f"{OS_SDK_VERSION_MAJOR}.{OS_SDK_VERSION_MINOR}.{OS_SDK_VERSION_PATCH}"
    OS_SDK_INSTALLER_NAME = f"OpenStudio-{OS_SDK_VERSION}{OS_SDK_VERSION_PRERELEASE}{OS_SDK_VERSION_SHA}"

    print(f"{OS_SDK_VERSION_MAJOR=}")
    print(f"{OS_SDK_VERSION_MINOR=}")
    print(f"{OS_SDK_VERSION_PATCH=}")
    print(f"{OS_SDK_VERSION_PRERELEASE=}")
    print(f"{OS_SDK_VERSION_SHA=}")
    print(f"{OS_SDK_VERSION=}")
    print(f"{OS_SDK_INSTALLER_NAME=}")

    if "GITHUB_ENV" not in os.environ:
        return

    for out in ["GITHUB_ENV", "GITHUB_OUTPUT"]:
        with open(os.environ[out], "a") as f:
            f.write(f"\nOS_SDK_VERSION_MAJOR={OS_SDK_VERSION_MAJOR}")
            f.write(f"\nOS_SDK_VERSION_MINOR={OS_SDK_VERSION_MINOR}")
            f.write(f"\nOS_SDK_VERSION_PATCH={OS_SDK_VERSION_PATCH}")
            f.write(f"\nOS_SDK_VERSION={OS_SDK_VERSION}")
            f.write(f"\nOS_SDK_VERSION={OS_SDK_VERSION}")
            f.write(f"\nOS_SDK_VERSION_SHA={OS_SDK_VERSION_SHA}")
            f.write(f"\nOS_SDK_INSTALLER_NAME={OS_SDK_INSTALLER_NAME}")


if __name__ == "__main__":
    parse_os_app_version(cmakelists_path=ROOT_DIR / "CMakeLists.txt")
    parse_os_sdk_version(find_sdk_path=ROOT_DIR / "FindOpenStudioSDK.cmake")

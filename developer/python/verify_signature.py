import argparse
import json
import os
import platform
import re
import subprocess
import warnings
from enum import Enum
from pathlib import Path
from typing import List


class Generator(Enum):
    IFW = 1
    TGZ = 2

BUNDLED_APPS = ['OpenStudioApp.app']
# BUNDLED_APPS = []


def get_cmake_install_prefix_for_generator(build_dir: Path, generator: Generator, component: str = None) -> Path:
    cpack_dir = build_dir / f"_CPack_Packages/Darwin/{generator.name}"
    if not cpack_dir.exists():
        print(f"Could not find a _CPack_Packages directory for {generator.name}")
        return None
    cmake_install_root = next(x for x in cpack_dir.glob('*') if x.is_dir() and x.suffix != '.app')
    if generator == Generator.TGZ:
        return cmake_install_root

    p = cmake_install_root / 'packages'
    if component:
        p = p / component / 'data'
    assert p.is_dir()
    return p


def find_executable_files(root_dir: Path) -> List[Path]:
    bundled_apps = [root_dir / x for x in BUNDLED_APPS]

    files = list(
        [
            x
            for x in root_dir.glob("**/*")
            if x.is_file()
            and not x.is_symlink()
            and os.access(x, os.X_OK)
            and not any([x.is_relative_to(bundled_p) for bundled_p in bundled_apps])
        ]
    )

    dylibs = list(
        [
            x
            for x in root_dir.glob("**/*.dylib")
            if x.is_file()
            and not x.is_symlink()
            and not any([x.is_relative_to(bundled_p) for bundled_p in bundled_apps])
        ]
    )

    sos = list([
        x for x in root_dir.glob('**/*.so')
        if x.is_file()
        and not x.is_symlink()
        and not any([x.is_relative_to(bundled_p) for bundled_p in bundled_apps])
    ])

    print(f"In {root_dir} found {len(files)} executable files and {len(dylibs)} dylibs and {len(sos)} SOs")

    files = list(set(files + dylibs + sos))

    files = sorted(sorted(files), key=lambda x: len(x.parts))

    return files


def verify_signature(p, verbose=False, root_dir=None, raise_on_error=False):
    info = subprocess.run(["codesign", "-dvvv", "--strict", str(p)], stderr=subprocess.PIPE, encoding="utf-8").stderr
    if "UG9S5ZLM34" not in info:
        msg = f"{p} is not codesigned properly"
        if raise_on_error:
            raise ValueError(msg)
        print(msg)
    if verbose:
        if root_dir is not None:
            p = p.relative_to(root_dir)
        print(f"- {p}: signature OK")


def compare_executables():
    cmake_ifw_prefix = get_cmake_install_prefix_for_generator(generator=Generator.IFW)
    ifw_files = find_executable_files(cmake_ifw_prefix)
    ifw_files_rel = [x.relative_to(cmake_ifw_prefix) for x in ifw_files]
    cmake_tgz_prefix = get_cmake_install_prefix_for_generator(generator=Generator.TGZ)
    tgz_files = find_executable_files(cmake_tgz_prefix)
    tgz_files_rel = [x.relative_to(cmake_tgz_prefix) for x in tgz_files]
    extra_ifw_files = set(ifw_files_rel) - set(tgz_files_rel)
    if extra_ifw_files:
        print(f"Extra IFW files: {extra_ifw_files}")

    extra_tgz_files = set(tgz_files_rel) - set(ifw_files_rel)
    if extra_tgz_files:
        print(f"Extra TGZ files: {extra_tgz_files}")


_OTOOL_ARCHITECTURE_RE = re.compile(r"^(?P<name>.*?)(?: \(architecture (?P<architecture>\w+)\))?:$")

LINKED_RE = re.compile(
    r"(?P<libname>.*) \(compatibility version (?P<compat_version>\d+\.\d+\.\d+), "
    r"current version (?P<current_version>\d+\.\d+\.\d+)(?:, \w+)?\)"
)

RPATH_RE = re.compile(r"path (?P<rpath>.*) \(offset \d+\)")

RESOLVED_PATHS = ["@loader_path", "@executable_path", "@rpath"]


def get_linked_libraries(p):
    lines = subprocess.check_output(["otool", "-L", str(p)], encoding="utf-8", universal_newlines=True).splitlines()
    if "is not an object file" in lines[0]:
        return None

    lines = [x.strip() for x in lines]

    m = _OTOOL_ARCHITECTURE_RE.match(lines[0])
    assert m
    arch = m.groupdict()['architecture']
    if arch is None:
        lines = lines[1:]
    else:
        arches = {}
        arches[arch] = {'start': 1}
        for i, line in enumerate(lines):
            if i == 0:
                continue
            if m := _OTOOL_ARCHITECTURE_RE.match(line):
                arches[arch]['end'] = i
                arch = m.groupdict()['architecture']
                arches[arch] = {'start': i + 1}
        arches[arch]['end'] = len(lines) - 1

        in_preference_order = ['arm64', 'x86_64', 'i386']
        for pref_arch in in_preference_order:
            if pref_arch in arches:
                arch = pref_arch
                lines = lines[arches[pref_arch]['start']:arches[pref_arch]['end']]
                print(f"Found multiple architectures, will select {pref_arch}, candidates were {arches.keys()}")
                break

    linked_libs = []
    for line in lines:
        if m := LINKED_RE.match(line):
            linked_libs.append(m.groupdict())
        else:
            raise ValueError(f"For {p}, cannot parse line: '{line}'")
    return linked_libs


def get_rpath(p):
    rpaths = []
    lines = subprocess.check_output(["otool", "-l", str(p)], encoding="utf-8", universal_newlines=True).splitlines()
    lines = [x.strip() for x in lines]
    for i, line in enumerate(lines):
        if line.startswith("cmd LC_RPATH"):
            assert lines[i + 1].startswith("cmdsize")
            rpath_line = lines[i + 2]
            m = RPATH_RE.match(rpath_line)
            assert m
            rpaths.append(m["rpath"])
    return rpaths


def otool(p, verify_resolve=False, verbose=False):
    linked_libs = get_linked_libraries(p)
    if linked_libs is None:
        return None

    rpaths = get_rpath(p)

    if verbose:
        print(f"- Otooling {p}")

    first_time = True
    rpath_infos = [{"actual": "implicit", "resolved": str(p.parent)}]
    for rpath in rpaths:
        rpath_dict = {}
        rpath_dict["actual"] = rpath
        if not rpath.startswith("@"):
            rpath_infos.append(rpath_dict)
            continue

        if verbose:
            if first_time:
                print("  * Resolving Rpaths")
                first_time = False
            print(f"    * Trying to resolve rpath '{rpath}'")
        rpath_dict["resolved"] = None
        resolved_path = rpath
        for resolv in RESOLVED_PATHS:
            resolved_path = resolved_path.replace(resolv, str(p.parent))
        resolved_path = Path(resolved_path).resolve()
        if resolved_path.exists():
            if verbose:
                print(f"      - Found {resolved_path}")
            rpath_dict["resolved"] = str(resolved_path)
        elif verify_resolve:
            warnings.warn(f"Could not resolve rpath '{rpath}' for '{p}'")
        rpath_infos.append(rpath_dict)

    first_time = True
    for linked_lib in linked_libs:
        libname = linked_lib["libname"]
        if not libname.startswith("@"):
            continue

        if verbose:
            if first_time:
                print("  * Resolving Libraries")
                first_time = False
            print(f"    * Trying to resolve library '{libname}'")
        linked_lib["resolved"] = None
        found = False
        for rpath_info in rpath_infos:
            rpath = rpath_info.get("resolved", rpath_info["actual"])
            resolved_path = libname
            for resolv in RESOLVED_PATHS:
                resolved_path = resolved_path.replace(resolv, str(rpath))

            resolved_path = Path(resolved_path)
            if resolved_path.exists():
                if verbose:
                    print(f"      - Found '{resolved_path}'")
                linked_lib["resolved"] = str(resolved_path)
                found = True
                break
        if not found and verify_resolve:
            msg = f"Could not resolve '{libname}' for '{p}'"
            if 'Radiance' in p.parts:
                print(msg)
            else:
                raise ValueError(msg)

    info = {
        "linked_libraries": linked_libs,
        "rpaths": rpath_infos,
    }

    if verbose:
        print("-" * 80)

    return info


if __name__ == "__main__":

    if platform.system() != "Darwin":
        raise OSError("Only supported on Darwin")

    parser = argparse.ArgumentParser(description="Verify codesigning on macOS")
    parser.add_argument(
        "build_dir",
        type=Path,
        help="Root of the Build directory where CMakeCache.txt can be found (or the install dir if --install is passed)",
    )
    parser.add_argument(
        "--install", action="store_true", default=False, help="This is an install dir, not the build_dir"
    )
    parser.add_argument(
        "--verbose", action="store_true", default=False, help="Enable verbose output"
    )
    parser.add_argument(
        "--only-generator", type=str, help="Only run for the given generator", choices=[i.name for i in Generator]
    )
    parser.add_argument("--otool", action="store_true", default=False, help="Output OTOOL info on all executables")
    parser.add_argument(
        "--otool-out-file", metavar="JSON_FILE", required=False, type=Path, help="Output OTOOL info to JSON file"
    )

    args = parser.parse_args()

    if args.otool_out_file and not args.otool:
        raise ValueError("--otool-out-file requires --otool")

    build_dir = args.build_dir.resolve()
    if not (build_dir.exists() and build_dir.is_dir()):
        raise NotADirectoryError(f"{build_dir} is not a valid directory")

    top_level_otool_infos = {}
    if args.install:
        if not (build_dir / "EnergyPlus/energyplus").is_file():
            raise ValueError(f"{build_dir} does not contain energyplus exe")
        print(f"Checking Install dir {build_dir}")
        executable_files = find_executable_files(root_dir=build_dir)
        excludes = ["runenergyplus", "runreadvars", "runepmacro", "maintenancetool"]
        executable_files = [x for x in executable_files if not any([n in x.name for n in excludes])]
        otool_infos = {}
        for p in executable_files:
            verify_signature(p, verbose=args.verbose, root_dir=build_dir)
            if args.otool:
                otool_info = otool(p=p, verify_resolve=True, verbose=args.verbose)
                if otool_info is not None:
                    otool_infos[str(p)] = otool_info
        print("Everything is signed correctly")
        top_level_otool_infos["install_dir"] = otool_infos
    else:
        if not (build_dir / "CMakeCache.txt").is_file():
            raise ValueError(f"{build_dir} does not contain CMakeCache.txt, did you forget to build?")

        if not (build_dir / "_CPack_Packages").is_dir():
            raise ValueError(
                f"{build_dir} does not contain a _CPack_Packages subfolder, did you forget to build the package?"
            )

        generators = [x for x in Generator]
        if args.only_generator is not None:
            generators = [Generator[args.only_generator]]

        for generator in generators:
            print(f"Checking {generator.name} Generator")
            cmake_install_prefix = get_cmake_install_prefix_for_generator(build_dir=build_dir, generator=generator)
            if cmake_install_prefix is None:
                continue
            executable_files = find_executable_files(root_dir=cmake_install_prefix)
            otool_infos = {}
            for p in executable_files:
                verify_signature(p, verbose=args.verbose, root_dir=cmake_install_prefix)
                if args.otool:
                    otool_info = otool(p=p, verify_resolve=True, verbose=args.verbose)
                    if otool_info is not None:
                        otool_infos[str(p)] = otool_info
            print("Everything is signed correctly")
            top_level_otool_infos[generator.name] = otool_infos
            print("=" * 80)

    if args.otool:
        short_otool_infos = {}
        for k, otool_infos in top_level_otool_infos.items():
            short_otool_infos[k] = {}
            for libpath, otool_info in otool_infos.items():
                short_otool_infos[k][libpath] = []
                for linked_libs in otool_info["linked_libraries"]:
                    short_otool_infos[k][libpath].append(linked_libs.get("resolved", linked_libs["libname"]))

        print("Shortened & resolved otool info:")
        print(json.dumps(short_otool_infos, indent=2))

        if args.otool_out_file:
            otool_out_file = args.otool_out_file.resolve()
            otool_out_file.parent.mkdir(parents=True, exist_ok=True)
            otool_out_file_short = otool_out_file.parent / f"{otool_out_file.stem}_short.json"
            print(f"Saving full otool infos to {otool_out_file} and short version to {otool_out_file_short}")
            otool_out_file.unlink(missing_ok=True)
            otool_out_file.write_text(json.dumps(top_level_otool_infos, indent=2))
            otool_out_file_short.unlink(missing_ok=True)
            otool_out_file_short.write_text(json.dumps(short_otool_infos, indent=2))

#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GITE01",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.4"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# ==== helper-msl cflags (only helper-msl edits this block) ====
# MSL_C / Runtime.PPCEABI.H / TRK_MINNOW_DOLPHIN: GC/2.6 (TP's TRK/Runtime/MSL math sources match with
# GC/2.5-2.7; NMWException's __construct_array only with 2.5+, the linker rules out 2.7). Flags follow
# zeldaret/tp's cflags_trk / cflags_runtime.
cflags_msl_base = [
    *cflags_base,
    "-i include/MSL_C",
    "-i include/Runtime.PPCEABI.H",
    "-i include/TRK_MINNOW_DOLPHIN",
    "-D__GEKKO__",
]
cflags_trk = [
    *cflags_msl_base,
    "-use_lmw_stmw on",
    "-rostr",
    "-str reuse",
    "-gccinc",
    "-common off",
    "-inline deferred,auto",
    "-char signed",
    "-sdata 0",
    "-sdata2 0",
]
cflags_rt = [
    *cflags_msl_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-common off",
    "-char signed",
    "-func_align 4",
    "-inline deferred,auto",
]
# MSL_C common sources: Geist's (older) MSL is built without the global optimizer (strlen, rand, time match
# only with "-opt level=0,peephole,schedule", not levels 1-4); the math library is -O4,p like the runtime.
# MSL_C common: "-O4,p" followed by "-opt level=0,peephole,schedule" (the -O4 sub-options stay set;
# dropping -O4,p changes register saves, e.g. stmw for two registers).
cflags_msl = [*cflags_rt, "-opt level=0,peephole,schedule", "-inline off"]  # no inlining at all (mem.c, alloc.c, buffer_io.c)
cflags_msl_math = cflags_rt
# ==== end helper-msl cflags ====
# ==== helper-musyx cflags (only helper-musyx edits this block) ====
# MusyX 2.0.3 (Factor 5), source layout from AxioDL/musyx (MIT) at the commit Prime's decomp pins.
# Evidence (helper-musyx): compiling all MusyX sources and searching main.dol, GC/1.3.2 + MusyX 2.0.3
# matched the most functions (368/613; 2.0.2: 350, 2.6: fewer, e.g. snd_math 1/4 vs 4/4; 1.2.5n: ~25).
cflags_musyx = [
    "-proc gekko",
    "-nodefaults",
    "-nosyspath",
    "-i include/musyx/compat",
    "-i include",
    "-i include/libc",
    f"-i build/{config.version}/include",
    "-inline auto,depth=4",
    "-O4,p",
    "-fp hard",
    "-enum int",
    "-Cpp_exceptions off",
    "-str reuse,pool,readonly",
    "-fp_contract off",
    "-DMUSY_TARGET=1",
    "-DMUSY_VERSION_MAJOR=2",
    "-DMUSY_VERSION_MINOR=0",
    "-DMUSY_VERSION_PATCH=3",
]
# zlib 1.1.4 (linked into game code, 0x80197E50-0x8019FD58). Evidence (helper-musyx): Geist compiled zlib
# as C++ (comparisons are byte-sized bools, deflateInit2_'s local static is guard-initialized, internal
# functions would be mangled); sources are upstream 1.1.4 with K&R definitions converted to ANSI.
# Registers are saved with stmw, functions come out in reverse source order (-inline deferred), strings
# are pooled. GC/1.3.2 and 2.0 differ on deflate_*; 2.5/2.6/2.7 match, 2.6 as for the rest of the game.
# -lang must come first: options before it are reset.
cflags_zlib = [
    "-lang=c++",
    "-proc gekko",
    "-nodefaults",
    "-nosyspath",
    "-i include/zlib/compat",
    "-i include/zlib",
    "-i src/zlib",
    "-i include",
    "-i include/libc",
    f"-i build/{config.version}/include",
    "-fp hard",
    "-enum int",
    "-Cpp_exceptions off",
    "-RTTI off",
    "-O4,p",
    "-inline auto,deferred",
    "-use_lmw_stmw on",
    "-str pool,readonly",
]
# ==== end helper-musyx cflags ====

# Game code (G1): GC/1.3.x, C++; -O4,p, -fp_contract on (from cflags_base). Unsettled: stmw, deferred inlining.
cflags_game = [
    *cflags_base,
    "-lang=c++",
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-i include/libc",
]

# Game files that were compiled as C (evidence per file, STATUS.md G4): same flags with -lang=c.
cflags_game_c = [f if f != "-lang=c++" else "-lang=c" for f in cflags_game]

# REL flags
cflags_rel = [
    *cflags_base,
    "-sdata 0",
    "-sdata2 0",
]

# Evidence (STATUS.md, 2026-10-10): linkers GC/1.2.5n-2.6 reproduce the retail DOL; GC/2.7 warns that the
# game's runtime is older than 2.7 and changes 67 bytes; 3.0 crashes. Game code matches GC/1.3.2-2.7
# (not 1.2.5n/3.0). 2.5 and 2.6 generate identical code on all SDK sources tested; 2.6 is the working choice.
config.linker_version = "GC/2.6"


# Dolphin SDK flags, as used by doldecomp/dolsdk2004 (whose sources are in src/dolphin).
cflags_dolphin = [
    "-char unsigned",
    "-nodefaults",
    "-proc gekko",
    "-fp hard",
    "-Cpp_exceptions off",
    "-enum int",
    "-warn pragmas",
    "-requireprotos",
    '-pragma "cats off"',
    "-O4,p",
    "-inline auto",
    "-D__GEKKO__",
    "-DSDK_REVISION=2",
    "-DRELEASE",
    "-I-",
    "-i include",
    "-i include/libc",
    "-ir src/dolphin",
    f"-i build/{config.version}/include",
]


# EXIBios.c is built at -O3,p in the SDK (dolsdk2004 does the same); a later -O3,p does not override -O4,p.
cflags_dolphin_O3 = ["-O3,p" if f == "-O4,p" else f for f in cflags_dolphin]


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_dolphin,
        "progress_category": "sdk",
        "objects": objects,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    # Dolphin SDK (GC/1.2.5n). Units found by matching dolsdk2004 sources against main.dol; see STATUS.md.
    DolphinLib(
        "ar",
        [
            Object(Matching, "dolphin/ar/ar.c"),
            Object(Matching, "dolphin/ar/arq.c"),
        ],
    ),
    DolphinLib(
        "base",
        [
            Object(Matching, "dolphin/base/PPCArch.c"),
        ],
    ),
    DolphinLib(
        "card",
        [
            Object(Matching, "dolphin/card/CARDBios.c"),
            Object(Matching, "dolphin/card/CARDUnlock.c"),
            Object(Matching, "dolphin/card/CARDRdwr.c"),
            Object(Matching, "dolphin/card/CARDBlock.c"),
            Object(Matching, "dolphin/card/CARDDir.c"),
            Object(Matching, "dolphin/card/CARDCheck.c"),
            Object(Matching, "dolphin/card/CARDMount.c"),
            Object(Matching, "dolphin/card/CARDFormat.c"),
            Object(Matching, "dolphin/card/CARDOpen.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/card/CARDCreate.c"),
            Object(Matching, "dolphin/card/CARDRead.c"),
            Object(Matching, "dolphin/card/CARDWrite.c"),
            Object(Matching, "dolphin/card/CARDDelete.c"),
            Object(Matching, "dolphin/card/CARDStat.c"),
            Object(Matching, "dolphin/card/CARDRename.c", extra_cflags=["-char signed"]),
        ],
    ),
    DolphinLib(
        "db",
        [
            Object(Matching, "dolphin/db/db.c"),
        ],
    ),
    DolphinLib(
        "dsp",
        [
            Object(Matching, "dolphin/dsp/dsp.c"),
            Object(Matching, "dolphin/dsp/dsp_task.c"),
        ],
    ),
    DolphinLib(
        "dvd",
        [
            Object(Matching, "dolphin/dvd/dvdlow.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/dvdfs.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/dvd.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/dvdqueue.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/dvderror.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/dvdidutils.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/fstload.c", extra_cflags=["-char signed"]),
        ],
    ),
    DolphinLib(
        "gx",
        [
            Object(Matching, "dolphin/gx/GXInit.c"),
            Object(Matching, "dolphin/gx/GXFifo.c"),
            Object(Matching, "dolphin/gx/GXAttr.c"),
            Object(Matching, "dolphin/gx/GXMisc.c"),
            Object(Matching, "dolphin/gx/GXGeometry.c"),
            Object(Matching, "dolphin/gx/GXFrameBuf.c"),
            Object(Matching, "dolphin/gx/GXLight.c"),
            Object(Matching, "dolphin/gx/GXTexture.c"),
            Object(Matching, "dolphin/gx/GXBump.c"),
            Object(Matching, "dolphin/gx/GXTev.c"),
            Object(Matching, "dolphin/gx/GXPixel.c"),
            Object(Matching, "dolphin/gx/GXDisplayList.c"),
            Object(Matching, "dolphin/gx/GXTransform.c"),
            Object(Matching, "dolphin/gx/GXPerf.c"),
        ],
    ),
    DolphinLib(
        "mtx",
        [
            Object(Matching, "dolphin/mtx/mtx.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/mtx/mtxvec.c"),
            Object(Matching, "dolphin/mtx/mtx44vec.c"),
            Object(Matching, "dolphin/mtx/mtx44.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/mtx/vec.c"),
            Object(Matching, "dolphin/mtx/quat.c"),
        ],
    ),
    DolphinLib(
        "os",
        [
            Object(Matching, "dolphin/os/OS.c"),
            Object(Matching, "dolphin/os/OSAlarm.c"),
            Object(Matching, "dolphin/os/OSAlloc.c"),
            Object(Matching, "dolphin/os/OSArena.c"),
            Object(Matching, "dolphin/os/OSAudioSystem.c"),
            Object(Matching, "dolphin/os/OSCache.c"),
            Object(Matching, "dolphin/os/OSContext.c"),
            Object(Matching, "dolphin/os/OSError.c"),
            Object(Matching, "dolphin/os/OSExec.c"),
            Object(Matching, "dolphin/os/OSFont.c"),
            Object(Matching, "dolphin/os/OSInterrupt.c"),
            Object(Matching, "dolphin/os/OSLink.c"),
            Object(Matching, "dolphin/os/OSMemory.c"),
            Object(Matching, "dolphin/os/OSMutex.c"),
            Object(Matching, "dolphin/os/OSReboot.c"),
            Object(Matching, "dolphin/os/OSReset.c"),
            Object(Matching, "dolphin/os/OSResetSW.c"),
            Object(Matching, "dolphin/os/OSRtc.c"),
            Object(Matching, "dolphin/os/OSSync.c"),
            Object(Matching, "dolphin/os/OSTime.c"),
            Object(Matching, "dolphin/os/__ppc_eabi_init.c"),
        ],
    ),
    DolphinLib(
        "pad",
        [
            Object(Matching, "dolphin/pad/Padclamp.c"),
            Object(Matching, "dolphin/pad/Pad.c"),
        ],
    ),
    DolphinLib(
        "vi",
        [
            Object(Matching, "dolphin/vi/vi.c"),
        ],
    ),
    DolphinLib(
        "ai",
        [
            Object(Matching, "dolphin/ai/ai.c"),
        ],
    ),
    DolphinLib(
        "exi",
        [
            Object(Matching, "dolphin/exi/EXIBios.c", cflags=cflags_dolphin_O3),
            Object(Matching, "dolphin/exi/EXIUart.c"),
        ],
    ),
    DolphinLib(
        "si",
        [
            Object(Matching, "dolphin/si/SIBios.c"),
            Object(Matching, "dolphin/si/SISamplingRate.c"),
        ],
    ),
    # ==== game units (generated by tools/gen_game_splits.py; provisional boundaries) ====
    {
        "lib": "game",
        "mw_version": "GC/1.3.2",
        "cflags": cflags_game,
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "game/Cube.cpp"),  # start
            Object(NonMatching, "game/main.cpp"),  # guess
            Object(NonMatching, "game/u_8000F2F0.cpp"),  # guess
            Object(NonMatching, "game/Turf.cpp"),  # guess
            Object(Matching, "game/u_8001598C.cpp"),  # frame timer, hook stubs
            Object(Matching, "game/u_80015A44.cpp"),  # path splitting
            Object(NonMatching, "game/u_80015B64.cpp"),  # split from u_8001598C
            Object(Matching, "game/File.cpp"),
            Object(NonMatching, "game/DbRoom.c"),  # guess
            Object(NonMatching, "game/u_80022B10.cpp"),  # guess
            Object(NonMatching, "game/ModInfo.cpp"),  # guess
            Object(NonMatching, "game/u_8003B98C.cpp"),  # guess
            Object(NonMatching, "game/u_8003CC90.cpp"),  # guess
            Object(NonMatching, "game/u_800448D0.cpp"),  # meshes, split from Store.c
            Object(NonMatching, "game/Store.c"),  # guess
            Object(NonMatching, "game/FearActor.c"),  # guess
            Object(NonMatching, "game/u_80064FD4.cpp"),  # guess
            Object(Matching, "game/u_80067FF8.cpp"),  # known
            Object(NonMatching, "game/u_80068004.cpp"),  # guess
            Object(NonMatching, "game/u_8006C604.cpp"),  # guess
            Object(NonMatching, "game/u_800776FC.cpp"),  # guess
            Object(NonMatching, "game/Arena.c"),  # guess
            Object(Matching, "game/Audio.c", cflags=cflags_game_c),  # C: unprototyped 3/4-arg calls (STATUS G4)
            Object(Matching, "game/u_8007DAC8.cpp"),  # timers
            Object(Matching, "game/u_8007E1E8.cpp"),  # split-screen text helpers
            Object(Matching, "game/u_8007E8F8.cpp"),  # object lookup tables
            Object(NonMatching, "game/u_8007EE68.cpp"),  # guess
            Object(NonMatching, "game/u_80080714.cpp"),  # ring-buffer queues
            Object(NonMatching, "game/u_80080F88.cpp"),  # guess
            Object(NonMatching, "game/Script.c"),  # guess
            Object(Matching, "game/u_80085CDC.cpp"),  # guess
            Object(Matching, "game/u_80085E50.cpp"),  # string table
            Object(NonMatching, "game/AppMain.c"),  # guess
            Object(NonMatching, "game/u_800886C8.cpp"),  # guess
            Object(NonMatching, "game/u_8008DCDC.cpp"),  # guess
            Object(NonMatching, "game/Shadow2.cpp"),  # guess
            Object(Matching, "game/Memory.cpp"),  # guess
            Object(NonMatching, "game/u_8009DE94.cpp"),  # known
            Object(NonMatching, "game/u_800A6F18.cpp"),  # guess
            Object(NonMatching, "game/u_800B1834.cpp"),  # guess
            Object(NonMatching, "game/u_800BAA14.cpp"),  # guess
            Object(NonMatching, "game/u_800C5028.cpp"),  # guess
            Object(NonMatching, "game/Progress.cpp"),  # guess
            Object(NonMatching, "game/u_800D0BF4.cpp"),  # guess
            Object(NonMatching, "game/u_800D32E8.cpp"),  # guess
            Object(Matching, "game/u_800DD92C.cpp"),  # NPC talk
            Object(NonMatching, "game/u_800DE0C4.cpp"),  # pinned
            Object(Matching, "game/u_800DED18.cpp"),  # level variables DB
            Object(Matching, "game/u_800DFE3C.cpp"),  # music streams, split from u_800DEC54
            Object(NonMatching, "game/SAM.cpp"),  # guess
            Object(NonMatching, "game/u_800E0CB8.cpp"),  # Vec3 helpers
            Object(Matching, "game/asyncio.cpp"),  # guess
            Object(NonMatching, "game/u_800E1B5C.cpp"),  # guess
            Object(NonMatching, "game/u_800E9418.cpp"),  # guess
            Object(NonMatching, "game/u_800EB1B8.cpp"),  # guess
            Object(NonMatching, "game/SpiritGun.cpp"),  # "SpiritGun" effect type
            Object(NonMatching, "game/Homing.cpp"),  # "Homing" effect type
            Object(Matching, "game/DeferPriority.cpp"),  # guess
            Object(Matching, "game/u_800F14A8.cpp"),  # known
            Object(Matching, "game/Image.cpp"),
            Object(NonMatching, "game/u_800F396C.cpp"),  # pinned
            Object(NonMatching, "game/u_800F73C8.cpp"),  # guess
            Object(NonMatching, "game/u_800FDDE4.cpp"),  # guess
            Object(NonMatching, "game/u_80100EB0.cpp"),  # guess
            Object(NonMatching, "game/CubeTev.cpp"),  # guess
            Object(NonMatching, "game/u_80112308.cpp"),  # guess
            Object(NonMatching, "game/u_80116B28.cpp"),  # guess
            Object(NonMatching, "game/u_8011818C.cpp"),  # guess
            Object(NonMatching, "game/u_8012071C.cpp", mw_version="GC/2.0"),  # stat table; only GC/2.0 matches its qsort comparators (STATUS G4)
            Object(NonMatching, "game/u_80120D98.cpp"),  # guess
            Object(NonMatching, "game/u_80125DE4.cpp"),  # guess
            Object(NonMatching, "game/u_80127D80.cpp"),  # guess
            Object(NonMatching, "game/u_80136884.cpp"),  # guess
            Object(NonMatching, "game/u_8013A3D4.cpp"),  # guess
            Object(NonMatching, "game/u_8013CA38.cpp"),  # known
            Object(NonMatching, "game/u_80140A30.cpp"),  # guess
            Object(NonMatching, "game/u_80142E88.cpp"),  # guess
            Object(Matching, "game/Skybox.cpp"),  # known (helper-game-b)
            Object(NonMatching, "game/u_80150300.cpp"),  # guess
            Object(NonMatching, "game/u_80153C10.cpp"),  # guess
            Object(NonMatching, "game/u_80157730.cpp"),  # guess
            Object(Matching, "game/Hazard.cpp"),  # (helper-game-b)
            Object(Matching, "game/EvacPoint.cpp"),  # (helper-game-b)
            Object(NonMatching, "game/u_8015C79C.cpp"),  # split from u_80157730
            Object(Matching, "game/u_80160188.cpp"),  # guess
            Object(NonMatching, "game/GrenadeFx.cpp"),  # starts 0x801608A4 (helper-game-b)
            Object(NonMatching, "game/u_80160DB0.cpp"),  # guess
            Object(Matching, "game/u_80161F40.cpp"),  # behaviour registry
            Object(NonMatching, "game/u_80161FF4.cpp"),  # guess
            Object(NonMatching, "game/u_8016A774.cpp"),  # pinned
            Object(NonMatching, "game/u_8016C244.cpp"),  # guess
            Object(NonMatching, "game/Msx.cpp"),  # guess
            Object(NonMatching, "game/u_80175D60.cpp"),  # known
            Object(Matching, "game/DataStream.cpp"),
            Object(NonMatching, "game/u_8017D794.cpp"),  # known
            Object(NonMatching, "game/u_8018C358.cpp"),  # guess
            Object(NonMatching, "game/u_8018D2D4.cpp"),  # pinned
            Object(NonMatching, "game/u_8018FAE8.cpp"),  # guess
            Object(Matching, "game/Repel.cpp"),  # starts 0x80191C6C (helper-game-b)
            Object(NonMatching, "game/u_801920B8.cpp"),  # known
            Object(NonMatching, "game/u_801965FC.cpp"),  # guess
            Object(NonMatching, "game/u_8019FDD0.cpp"),  # rendering after zutil (mirror/reflection)
            Object(NonMatching, "game/u_801A0F2C.cpp"),  # guess
            Object(NonMatching, "game/u_801A1D24.cpp"),  # guess
            Object(NonMatching, "game/u_801A28A4.cpp"),  # pinned
            Object(NonMatching, "game/u_801A6094.cpp"),  # pinned
            Object(NonMatching, "game/u_801AADA8.cpp"),  # pinned
            Object(NonMatching, "game/u_801AE50C.cpp"),  # pinned
            Object(NonMatching, "game/VMem.cpp"),  # known start; end 0x801BF578 (helper-game-b)
            Object(NonMatching, "game/u_801BF578.cpp"),  # split from VMem
            Object(NonMatching, "game/u_801D9A70.cpp"),  # guess
            Object(NonMatching, "game/u_801E0EDC.cpp"),  # guess
            Object(NonMatching, "game/u_801E241C.cpp"),  # pinned
            Object(NonMatching, "game/u_801E7DE0.cpp"),  # guess
            Object(NonMatching, "game/u_801EAACC.cpp"),  # guess
            Object(NonMatching, "game/u_801EDE74.cpp"),  # guess
            Object(NonMatching, "game/u_801F1798.cpp"),  # guess
            Object(NonMatching, "game/u_801F2DA4.cpp"),  # pinned
            Object(NonMatching, "game/u_801F676C.cpp"),  # pinned
            Object(NonMatching, "game/u_801F7F48.cpp"),  # pinned
            Object(NonMatching, "game/u_801FC634.cpp"),  # known
            Object(NonMatching, "game/FearMap2.c"),  # guess
            Object(NonMatching, "game/u_80202AB8.cpp"),  # guess
            Object(NonMatching, "game/u_802052B4.cpp"),  # guess
            Object(NonMatching, "game/u_80208238.cpp"),  # pinned
            Object(NonMatching, "game/u_8020A650.cpp"),  # guess
            Object(NonMatching, "game/u_8020B77C.cpp"),  # pinned
            Object(NonMatching, "game/u_8020D47C.cpp"),  # pinned
            Object(NonMatching, "game/u_80210D80.cpp"),  # guess
            Object(NonMatching, "game/linkObjSys.c"),  # guess
            Object(NonMatching, "game/u_8021427C.cpp"),  # guess
            Object(NonMatching, "game/u_802154B0.cpp"),  # guess
            Object(NonMatching, "game/u_80221100.cpp"),  # known
            Object(NonMatching, "game/u_80222C08.cpp"),  # guess
            Object(NonMatching, "game/u_80226DE8.cpp"),  # guess
            Object(Matching, "game/Streamer.cpp"),  # (helper-game-b)
            Object(NonMatching, "game/u_8022ED88.cpp"),  # split from u_80226DE8
            Object(Matching, "game/PadRecord.cpp"),  # 0x802301E0 (helper-game-b)
            Object(Matching, "game/ExtraData.cpp"),  # split from u_80230224
            Object(NonMatching, "game/u_80230504.cpp"),  # guess
            Object(NonMatching, "game/u_80231550.cpp"),  # guess
            Object(NonMatching, "game/Corona.cpp"),  # guess
            Object(NonMatching, "game/u_8023824C.cpp"),  # guess
            Object(NonMatching, "game/u_8023B96C.cpp"),  # guess
            Object(NonMatching, "game/u_8023E7E0.cpp"),  # pinned
            Object(NonMatching, "game/u_8023FA5C.cpp"),  # guess
            Object(NonMatching, "game/u_80243B0C.cpp"),  # guess
            Object(NonMatching, "game/u_80244728.cpp"),  # pinned
            Object(NonMatching, "game/u_80247EB0.cpp"),  # guess
            Object(NonMatching, "game/u_80248FC8.cpp"),  # pinned
            Object(NonMatching, "game/u_8024CAA0.cpp"),  # known
            Object(Matching, "game/Scannable.cpp"),  # (helper-game-b)
            Object(NonMatching, "game/u_8024E5D0.cpp"),  # guess
            Object(NonMatching, "game/u_80251CB8.cpp"),  # pinned
            Object(NonMatching, "game/u_802541D4.cpp"),  # guess
            Object(NonMatching, "game/u_8025643C.cpp"),  # guess
            Object(NonMatching, "game/LaserScope.cpp"),  # (helper-game-b)
            Object(NonMatching, "game/u_80259AB0.cpp"),  # pinned
            Object(NonMatching, "game/u_8025ECF8.cpp"),  # pinned
            Object(Matching, "game/ThermoBeh.cpp"),  # thermostat behaviour (split from Thermostat.cpp)
            Object(Matching, "game/Thermostat.cpp"),  # thermostat gauge actor
            Object(NonMatching, "game/Retinal.cpp"),  # (helper-game-b)
            Object(NonMatching, "game/u_80268528.cpp"),  # split from u_80267558
            Object(NonMatching, "game/u_80268780.cpp"),  # guess
            Object(NonMatching, "game/u_8026A158.cpp"),  # guess
            Object(NonMatching, "game/u_8026A37C.cpp"),  # guess
            Object(NonMatching, "game/u_8026C200.cpp"),  # guess
            Object(NonMatching, "game/u_802766F0.cpp"),  # guess
            Object(NonMatching, "game/u_80277600.cpp"),  # guess
            Object(NonMatching, "game/u_8027D3EC.cpp"),  # guess
            Object(NonMatching, "game/u_8028009C.cpp"),  # guess
            Object(NonMatching, "game/u_80285014.cpp"),  # known
            Object(Matching, "game/Mirror.cpp"),  # (helper-game-b)
            Object(NonMatching, "game/u_80285ACC.cpp"),  # split from u_80285014
            Object(NonMatching, "game/VM.c"),  # guess
            Object(NonMatching, "game/u_8028703C.cpp"),  # guess
            Object(Matching, "game/Clock.cpp"),  # (helper-game-b)
            Object(NonMatching, "game/u_8028865C.cpp"),  # split from u_8028703C
            Object(NonMatching, "game/u_8028A77C.cpp"),  # guess
            Object(NonMatching, "game/u_8028C54C.cpp"),  # guess
            Object(NonMatching, "game/u_8028DA1C.cpp"),  # guess
            Object(NonMatching, "game/u_80290C58.cpp"),  # guess
            Object(NonMatching, "game/u_80294548.cpp"),  # guess
            Object(NonMatching, "game/u_8029C0AC.cpp"),  # guess
            Object(NonMatching, "game/u_8029E714.cpp"),  # pinned
            Object(Matching, "game/u_802A770C.cpp"),  # host types
            Object(NonMatching, "game/Fuel.cpp"),  # (helper-game-b)
            Object(NonMatching, "game/u_802A815C.cpp"),  # split from u_802A770C
            Object(NonMatching, "game/u_802A9824.cpp"),  # known
            Object(NonMatching, "game/u_802AE27C.cpp"),  # pinned
            Object(Matching, "game/u_802B2680.cpp"),  # pinned
            Object(NonMatching, "game/u_802B3430.cpp"),  # guess
            Object(NonMatching, "game/u_802B4600.cpp"),  # pinned
            Object(NonMatching, "game/u_802B7120.cpp"),  # pinned
            Object(NonMatching, "game/u_802B9088.cpp"),  # guess
            Object(NonMatching, "game/u_802BA078.cpp"),  # pinned
            Object(NonMatching, "game/u_802BB1F4.cpp"),  # guess
            Object(NonMatching, "game/u_802BF114.cpp"),  # pinned
            Object(NonMatching, "game/u_802C01E8.cpp"),  # guess
            Object(NonMatching, "game/u_802C30A4.cpp"),  # guess
            Object(NonMatching, "game/u_802C8900.cpp"),  # guess
            Object(NonMatching, "game/u_802C9818.cpp"),  # pinned
            Object(NonMatching, "game/u_802CBB44.cpp"),  # pinned
            Object(NonMatching, "game/u_802CED5C.cpp"),  # guess
            Object(NonMatching, "game/u_802D439C.cpp"),  # guess
            Object(NonMatching, "game/u_802D4D18.cpp"),  # pinned
            Object(NonMatching, "game/u_802D77AC.cpp"),  # pinned
            Object(NonMatching, "game/u_802D8C7C.cpp"),  # guess
            Object(NonMatching, "game/u_802DB9D0.cpp"),  # pinned
            Object(NonMatching, "game/u_802DC82C.cpp"),  # guess
            Object(NonMatching, "game/u_802DDEDC.cpp"),  # guess
            Object(Matching, "game/CollectionHost.cpp"),
            Object(Matching, "game/CollectionGhost.cpp"),
            Object(Matching, "game/Credits.cpp"),  # credits screen
            Object(Matching, "game/CreditsTip.cpp"),  # tip messages (split from Credits.cpp)
            Object(Matching, "game/NoBot.cpp"),  # "No Bot" behaviour (split from Credits.cpp)
        ],
    },
    # ==== end game units ====
    # ==== helper-msl: MSL_C / Runtime.PPCEABI.H / TRK_MINNOW_DOLPHIN libs (only helper-msl edits this block) ====
    {
        "lib": "TRK_MINNOW_DOLPHIN",
        "mw_version": "GC/2.6",
        "cflags": cflags_trk,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/mainloop.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/nubevent.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/nubinit.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/msg.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/msgbuf.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/serpoll.c", extra_cflags=['-sdata 8']),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/usr_put.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/dispatch.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/msghndlr.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/support.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/mutex_TRK.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/notify.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/flush_cache.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/mem_TRK.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/mpc_7xx_603e.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/main_TRK.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk_glue.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/targcont.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/target_options.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Export/mslsupp.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/UDP_Stubs.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/gamedev/cust_connection/cc/exi2/GCN/EXI2_DDH_GCN/main.c", extra_cflags=['-sdata 8']),
            Object(Matching, "TRK_MINNOW_DOLPHIN/gamedev/cust_connection/utils/common/CircleBuffer.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN/gamedev/cust_connection/cc/exi2/GCN/EXI2_GDEV_GCN/main.c", extra_cflags=['-sdata 8']),
            Object(Matching, "TRK_MINNOW_DOLPHIN/gamedev/cust_connection/utils/common/MWTrace.c"),
            Object(Matching, "TRK_MINNOW_DOLPHIN/gamedev/cust_connection/utils/gc/MWCriticalSection_gc.c"),
        ],
    },
    {
        "lib": "Runtime.PPCEABI.H",
        "mw_version": "GC/2.6",
        "cflags": cflags_rt,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "Runtime.PPCEABI.H/__va_arg.c"),
            Object(Matching, "Runtime.PPCEABI.H/global_destructor_chain.c"),
            Object(Matching, "Runtime.PPCEABI.H/exception.cp", extra_cflags=['-Cpp_exceptions on', '-RTTI on']),
            Object(Matching, "Runtime.PPCEABI.H/NMWException.cp", extra_cflags=['-Cpp_exceptions on']),
            Object(Matching, "Runtime.PPCEABI.H/runtime.c"),
            Object(Matching, "Runtime.PPCEABI.H/__init_cpp_exceptions.cpp"),
            Object(Matching, "Runtime.PPCEABI.H/Gecko_ExceptionPPC.cp", extra_cflags=['-Cpp_exceptions on', '-RTTI on', '-str nopool'], extab_padding=[0x02, 0x55]),
            Object(Matching, "Runtime.PPCEABI.H/GCN_mem_alloc.c", extra_cflags=['-str reuse,nopool,readonly']),
        ],
    },
    {
        "lib": "MSL_C",
        "mw_version": "GC/2.6",
        "cflags": cflags_msl,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "MSL_C/MSL_Common/Src/abort_exit.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/alloc.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/ansi_files.c", extra_cflags=['-use_lmw_stmw off'], cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Src/ansi_fp.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/buffer_io.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/PPC_EABI/Src/critical_regions.gamecube.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/ctype.c", extra_cflags=["-DMSL_USE_INLINES=1"], cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/direct_io.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/file_io.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/FILE_POS.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/mbstring.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/mem.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/mem_funcs.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/misc_io.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/printf.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/qsort.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/rand.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/signal.c", extra_cflags=['-use_lmw_stmw off'], cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/string.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/time.c", extra_cflags=['-use_lmw_stmw off'], cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/wchar_io.c", extra_cflags=['-use_lmw_stmw off'], cflags=cflags_msl),
            Object(Matching, "MSL_C/PPC_EABI/Src/time.gamecube.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/PPC_EABI/Src/uart_console_io_gcn.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_acos.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_atan2.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_exp.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_fmod.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_pow.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_rem_pio2.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_cos.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_rem_pio2.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_sin.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/k_tan.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_atan.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_copysign.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_cos.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_floor.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_frexp.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_ldexp.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_modf.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_sin.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/s_tan.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_acos.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_atan2.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_exp.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_fmod.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_pow.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/e_sqrt.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/PPC_EABI/Src/math_ppc.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common/Src/math_api.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common_Embedded/Math/Double_precision/w_sqrt.c", cflags=cflags_msl_math),
            Object(Matching, "MSL_C/MSL_Common/Src/extras.c", extra_cflags=["-DMSL_USE_INLINES=1"], cflags=cflags_rt),
        ],
    },
    # ==== end helper-msl ====
    # ==== helper-musyx: MusyX / zlib libs (only helper-musyx edits this block) ====
    {
        "lib": "musyx",
        "mw_version": "GC/1.3.2",
        "cflags": cflags_musyx,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "musyx/runtime/seq.c"),
            Object(Matching, "musyx/runtime/synth.c"),
            Object(Matching, "musyx/runtime/seq_api.c"),
            Object(Matching, "musyx/runtime/snd_synthapi.c"),
            Object(Matching, "musyx/runtime/stream.c"),
            Object(Matching, "musyx/runtime/synthdata.c"),
            Object(Matching, "musyx/runtime/synthmacros.c"),
            Object(Matching, "musyx/runtime/synthvoice.c"),
            Object(Matching, "musyx/runtime/synth_ac.c"),
            Object(Matching, "musyx/runtime/synth_adsr.c"),
            Object(Matching, "musyx/runtime/synth_vsamples.c"),
            Object(Matching, "musyx/runtime/s_data.c"),
            Object(Matching, "musyx/runtime/hw_dspctrl.c"),
            Object(Matching, "musyx/runtime/hw_volconv.c"),
            Object(Matching, "musyx/runtime/snd3d.c"),
            Object(Matching, "musyx/runtime/snd_init.c"),
            Object(Matching, "musyx/runtime/snd_math.c"),
            Object(Matching, "musyx/runtime/snd_midictrl.c"),
            Object(Matching, "musyx/runtime/snd_service.c"),
            Object(Matching, "musyx/runtime/hardware.c"),
            Object(Matching, "musyx/runtime/hw_aramdma.c"),
            Object(Matching, "musyx/runtime/dsp_import.c"),
            Object(Matching, "musyx/runtime/hw_dolphin.c"),
            Object(Matching, "musyx/runtime/hw_memory.c"),
            Object(Matching, "musyx/runtime/CheapReverb/creverb_fx.c"),
            Object(Matching, "musyx/runtime/CheapReverb/creverb.c"),
        ],
    },
    {
        "lib": "zlib",
        "mw_version": "GC/2.6",
        "cflags": cflags_zlib,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "zlib/adler32.c"),
            Object(Matching, "zlib/compress.c"),
            Object(Matching, "zlib/deflate.c"),
            Object(Matching, "zlib/infblock.c"),
            Object(Matching, "zlib/infcodes.c"),
            Object(Matching, "zlib/inffast.c"),
            Object(Matching, "zlib/inflate.c"),
            Object(Matching, "zlib/inftrees.c"),
            Object(Matching, "zlib/infutil.c"),
            Object(Matching, "zlib/trees.c"),
            Object(Matching, "zlib/uncompr.c"),
            Object(Matching, "zlib/zutil.c"),
        ],
    },
    # ==== end helper-musyx ====
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)

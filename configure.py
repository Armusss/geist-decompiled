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
cflags_msl = [f for f in cflags_rt if f != "-O4,p"] + ["-opt level=0,peephole,schedule"]
cflags_msl_math = cflags_rt
# ==== end helper-msl cflags ====
# ==== helper-musyx cflags (only helper-musyx edits this block) ====
# ==== end helper-musyx cflags ====

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
            Object(NonMatching, "dolphin/ar/arq.c"),
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
            Object(NonMatching, "dolphin/dvd/dvdfs.c", extra_cflags=["-char signed"]),
            Object(NonMatching, "dolphin/dvd/dvd.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/dvdqueue.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/dvderror.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/dvdidutils.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/dvd/fstload.c", extra_cflags=["-char signed"]),
        ],
    ),
    DolphinLib(
        "gx",
        [
            Object(NonMatching, "dolphin/gx/GXInit.c"),
            Object(NonMatching, "dolphin/gx/GXFifo.c"),
            Object(NonMatching, "dolphin/gx/GXAttr.c"),
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
            Object(NonMatching, "dolphin/mtx/mtx.c", extra_cflags=["-char signed"]),
            Object(Matching, "dolphin/mtx/mtxvec.c"),
            Object(Matching, "dolphin/mtx/mtx44vec.c"),
            Object(NonMatching, "dolphin/mtx/vec.c"),
            Object(NonMatching, "dolphin/mtx/quat.c"),
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
            Object(NonMatching, "dolphin/os/OSFont.c"),
            Object(Matching, "dolphin/os/OSInterrupt.c"),
            Object(Matching, "dolphin/os/OSLink.c"),
            Object(Matching, "dolphin/os/OSMemory.c"),
            Object(Matching, "dolphin/os/OSMutex.c"),
            Object(Matching, "dolphin/os/OSReboot.c"),
            Object(Matching, "dolphin/os/OSReset.c"),
            Object(Matching, "dolphin/os/OSResetSW.c"),
            Object(NonMatching, "dolphin/os/OSRtc.c"),
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
            Object(NonMatching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/msghndlr.c"),
            Object(NonMatching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Portable/support.c"),
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
            Object(NonMatching, "TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Export/mslsupp.c"),
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
            Object(NonMatching, "Runtime.PPCEABI.H/global_destructor_chain.c"),
            Object(NonMatching, "Runtime.PPCEABI.H/NMWException.cp", extra_cflags=['-Cpp_exceptions on']),
            Object(NonMatching, "Runtime.PPCEABI.H/ptmf.c"),
            Object(NonMatching, "Runtime.PPCEABI.H/runtime.c"),
            Object(NonMatching, "Runtime.PPCEABI.H/__init_cpp_exceptions.cpp"),
            Object(NonMatching, "Runtime.PPCEABI.H/Gecko_ExceptionPPC.cp", extra_cflags=['-Cpp_exceptions on']),
            Object(NonMatching, "Runtime.PPCEABI.H/GCN_mem_alloc.c", extra_cflags=['-str reuse,nopool,readonly']),
        ],
    },
    {
        "lib": "MSL_C",
        "mw_version": "GC/2.6",
        "cflags": cflags_msl,
        "progress_category": "sdk",
        "objects": [
            Object(NonMatching, "MSL_C/MSL_Common/Src/abort_exit.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/alloc.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/ansi_files.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common_Embedded/Src/ansi_fp.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/buffer_io.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/PPC_EABI/Src/critical_regions.gamecube.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/ctype.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/direct_io.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/file_io.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/FILE_POS.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/mbstring.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/mem.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/mem_funcs.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/MSL_Common/Src/misc_io.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/printf.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/scanf.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/rand.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/signal.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/string.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/time.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/MSL_Common/Src/wchar_io.c", cflags=cflags_msl),
            Object(Matching, "MSL_C/PPC_EABI/Src/time.gamecube.c", cflags=cflags_msl),
            Object(NonMatching, "MSL_C/PPC_EABI/Src/uart_console_io_gcn.c", cflags=cflags_msl),
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
            Object(NonMatching, "MSL_C/MSL_Common/Src/extras.c", cflags=cflags_msl_math),
        ],
    },
    # ==== end helper-msl ====
    # ==== helper-musyx: MusyX / zlib libs (only helper-musyx edits this block) ====
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

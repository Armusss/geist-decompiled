# STATUS (2026-10-10)

Handoff file for the matching decompilation. Newest section first. See README.md for setup/build.

## Chunk 4 — SDK data placement (33 / 75 files matched)

**Result:** 33 SDK files build from C with a byte-identical main.dol
(`SDK Code: 95.56% fuzzy, 92.59% matched, 20.14% linked (33 / 75 files)`). Added this chunk:
dvderror, dvdqueue, fstload, GXTev, GXBump, GXPerf, GXFrameBuf, GXDisplayList, GXLight, GXPixel, mtxvec,
mtx44vec, OSAlloc, OSArena, OSAudioSystem, OSError, OSLink, OSMemory, OSReboot, OSResetSW, EXIUart,
Padclamp, SISamplingRate.

**What made data placement work (all in `~/geist-work/mwprobe/auto_match.py` + `obj_layout.py`):**
- The linker strips unused **data** too (constants/strings used only by stripped functions), so each
  data object is located individually from relocations, not by section base + offset.
- A section's split runs from its base to the end of its last *referenced* object (rounded to 4).
  Sections with no referenced object at all were stripped entirely and get no split.
- Only referenced objects are written to symbols.txt; dtk's guessed data symbols inside those ranges
  are removed first (dtk had guessed wrong sizes, e.g. `lbl_803C7872` spanning three strings).
- A section never starts before the end of an earlier split of the same section.

**Still assembly although the code matches (5):**
- `mtx/mtx.c`: unit extended to 0x8030D06C (now 15 functions, full code match), but its `.sdata2`
  constants interleave with vec.c/quat.c references (undefined lbl_804DB858..lbl_804DB864). Fix the
  three together.
- `mtx/vec.c`, `mtx/quat.c`: same constant-pool tangle (quat is referenced from the 0x8030D114 gap,
  probably mtx44.c functions).
- `gx/GXAttr.c`: dtk's `GXTexRegionAddrTable` (0x803C7A4C, guessed size 0xC0) crosses the boundary;
  set its real size in symbols.txt.
- `ar/arq.c`: its .sbss objects overlap a variable used by ar.c (lbl_804D58FC), so the placement is
  wrong; check against ar.c's own data.

## Chunk 3 — first SDK files matched in C (17 / 75)

**Result:** 17 Dolphin SDK files now build from C source and link into a byte-identical main.dol
(`SDK Code: 93.04% matched, 9.35% linked (17 / 75 files)`):
PPCArch, CARDFormat, CARDOpen, CARDCreate, CARDRead, CARDWrite, CARDDelete, CARDStat, CARDRename,
OSMutex, dvderror, OSArena, OSAudioSystem, OSMemory, OSReboot, OSResetSW, EXIUart.

**Source:** `src/dolphin/` and `include/` are copied from doldecomp/dolsdk2004 (commit 2328b416,
2025-02-03), at the user's request. **That repository has no licence**, so this project must stay
private. Flags are dolsdk2004's (`cflags_dolphin` in configure.py: GC/1.2.5n, `-char unsigned`, with
`-char signed` for CARDOpen/CARDRename/dvd/mtx/mtx44 and `-O3,p` for EXIBios).

**Tools (in `~/geist-work/mwprobe/`):**
- `tu_check.py`: compiles each split SDK file and checks whether its functions, in order, cover the
  unit exactly (functions Geist never calls are allowed to be missing: the linker strips them).
  38 of 75 pass. The other 37 have at least one function that differs (Geist's SDK builds are
  Apr 5 / Nov 10 2004; the reference is 4/20/2004).
- `data_locate.py`: finds each data section of a compiled file in main.dol from the DOL's own
  instructions at relocation sites (r13/r2 small-data loads, lis/addi pairs).
- `names_from_relocs.py --apply`: gives every symbol a C file defines or references its real name in
  symbols.txt (e.g. memcmp, strlen, __shr2i, __CARDBlock), so asm and C units link together.
- `auto_match.py`: tries each candidate file (adds data splits, marks Matching, renames), rebuilds,
  and keeps it only if the SHA-1 still matches.

**Why the other fully-matching files were reverted:**
- dtk's guessed data-symbol sizes conflict with the compiled section sizes ("split ends within symbol",
  e.g. fstload .data ends inside lbl_803C7872). Fix: adjust that symbol's size in symbols.txt, or the
  split end, per file.
- Data sections only referenced from stripped functions can't be located by relocations (shown as
  "not located confidently"): GXFrameBuf, GXDisplayList, mtxvec, mtx44vec, quat, OSLink, Padclamp, arq.
- Shared constant pools: GXPixel and vec both reference lbl_804DB85C in .sdata2, so at least one base is
  wrong. mtx.c: an asm unit still calls a function inside it by its old name.

**Verify (current state, after chunk 4):**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 95.56% fuzzy, 92.59% matched, 20.14% linked (33 / 75 files)
```

**Next:** fix the reverted files one by one (data sizes, constant pools), then adapt the 37 differing
files to Geist's SDK build (diff each function with `probe.py --diff`).

## Chunk 2 — Dolphin SDK splits (done for .text)

**Result:** 75 SDK translation units across 14 libraries (ai, am, ar, base, card, db, dsp, dvd, exi, gx,
mtx, os, pad, si, vi) in `config/GITE01/splits.txt` (`.text` ranges only), 674 SDK functions named in
`symbols.txt`, and matching `DolphinLib` entries (all `NonMatching`, no source yet) in `configure.py`.
The relinked DOL still matches retail.

**How:** `~/geist-work/mwprobe/sdk_scan.py` compiles all 182 dolsdk2004 sources with GC/1.2.5n
(`EXIBios.c` with `-O3,p`, as dolsdk2004 does) and searches main.dol: 702 functions match at exactly one
address in the SDK range. `gen_sdk_splits.py` turns contiguous runs into units, drops files that only
"match" duplicated helper code (CARDProgram, CARDErase, SISteeringAuto, axfx/reverb_std), extends units
with neighbouring functions whose names appear in the same source file, and writes splits + renames.

**Gotchas found:**
- **Linker hang:** any unit boundary at 0x80314318 (start of OSSync.c's system-call stub) makes the
  GC/2.6 linker spin forever. Workaround: the `dolphin/os/OSRtc.c` unit also covers OSSync.c
  (0x80314318-0x803143A0), with a TODO in splits.txt. Making the label the function name instead gives
  a bare "Link failed". Revisit when OSSync.c gets real source.
- Statics with the same name in several files (OnReset, AlarmHandler, WriteCallback, EraseCallback,
  BootInfo, ...) must be `scope:local`, otherwise the linker reports multiply-defined symbols.
- `__OSSetExceptionHandler` rename skipped (name already used by dtk elsewhere).
- Gaps not yet assigned: mtx (0x8030CD80-0x8030D06C, 0x8030D114-0x8030D2B0, probably mtx44/psmtx
  variants), small 1-function gaps between CARD files, and dvd 0x8030427C-0x803045BC.

**Not yet split:** MSL C / Runtime / TRK (≈0x802E0000-0x802F6BE8), MusyX + others (0x8031DE10-0x8033F5B8,
137 KB, 382 functions), zlib 1.1.4 (location not yet confirmed), and `.data`/`.bss`/`.sdata` for all units.

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && ninja | grep "SDK Code"   # SDK Code: 0.00% ... (0 / 75 files)
sha1sum build/GITE01/main.dol                     # 6612f52ce6c50bb831ffddd5264b22973304f288
```

## Chunk 1 — compiler version (done as far as the evidence goes)

### Verify the baseline
```sh
export PATH=$HOME/.local/bin:$PATH
python3 configure.py && ninja
sha1sum orig/GITE01/sys/main.dol build/GITE01/main.dol   # both 6612f52ce6c50bb831ffddd5264b22973304f288
```

### Tool
`~/geist-work/mwprobe/probe.py` (outside this repo): compiles a C file with a chosen CodeWarrior
version and searches main.dol for each function, masking relocated fields. `--diff <addr>` prints a
side-by-side disassembly. Reference SDK source: `~/geist-work/refs/dolsdk2004` (public matching
decomp of the 4/20/2004 SDK; read-only, none of its scripts are run).

```sh
cd ~/geist-work/mwprobe
python3 probe.py 1.2.5n ~/geist-work/refs/dolsdk2004/src/pad/Pad.c        # expect 22/33 MATCH
python3 probe.py --diff 80031FAC 2.6 game/v1.c -O4,p                       # expect no "!!" lines
```

### Findings
1. **SDK libraries: GC/1.2.5n, `-O4,p -inline auto`** (confirmed). Compiling dolsdk2004 sources and
   searching main.dol:

   | version | OSAlarm.c | Pad.c | dvdfs.c |
   |---|---|---|---|
   | 1.2.5 | 6/14 | 13/33 | 14/31 |
   | **1.2.5n** | **9/14** | **22/33** | **18/31** |
   | 1.3 – 2.7 | 3/14 | 4/33 | 7/31 |
   | 3.0a3 | 3/14 | 2/33 | 3/31 |

   Misses are mostly functions Geist never calls (dead-stripped by the linker) or small differences
   between the 4/20/2004 reference and Geist's builds (Apr 5 2004 for AR/CARD/DVD/PAD, Nov 10 2004
   for GX/OS). `configure.py`'s `DolphinLib` already uses GC/1.2.5n, so no change needed.
2. **Game code: GC/1.3.2, 2.0, 2.5 or 2.6; working choice GC/2.6.**
   - A hand-written C version of fn_80031FAC (104 bytes) matches byte-for-byte with 1.3.2-2.7 and with
     none of 1.2.5n / 3.0a3 / 3.0a5. Flags: `-O4,p`.
   - **Linker test** (swap `config.linker_version`, relink the baseline): GC/1.2.5n, 1.3.2, 2.0, 2.5 and
     2.6 all reproduce the retail SHA-1. GC/2.7 warns "runtime sources 'global_destructor_chain.c' and
     '__init_cpp_exceptions.cpp' both need to be updated to latest version" and changes 67 bytes (also
     with comment version 11). GC/3.0a3/3.0a5 crash. So Geist predates CW 2.7.
   - 2.5 and 2.6 generate identical code for all 78 SDK sources compiled (os, gx, mtx, card). 2.0 differs
     on 7 (float loops: 2.5/2.6 unroll by two and use paired-single saves), 1.3.2 on 17.
   - Not settled: 1.3.2 vs 2.0 vs 2.5/2.6. Two probes (paired-single FPR saves: present from 1.3.2 on;
     a FIFO-write loop: identical in all) didn't separate them. Game code has 6 compiler-unrolled loops
     (e.g. fn_8000761C); reproducing one of those float loops is the next discriminator.
   - `mw_comment_version: 10` is correct for 1.3.2-2.6. configure.py comment updated with this evidence.
3. **Game code uses pooled strings** (`-str pool` or similar): literals are addressed as
   `pool_base + offset`. Will need adding to the game cflags.
4. n-Space's chunk loaders use a bounds-check macro (need = count*size; ok = left>0 && need>0;
   ptr = ok ? cursor : 0; then advance by need plus 32-byte padding). fn_80043270 (model loader)
   reproduces except 13 instructions of register numbering (same for all five compilers).
5. Geist's C library (MSL) is a different source release from Twilight Princess's (TP builds MSL with
   GC/2.7): TP's memmove can't match Geist's under any version/optimisation tried. Pikmin 2's repo
   moved (not found at projectPiklopedia/pikmin2 or doldecomp/pikmin2); worth finding later.

### Next steps
- (Optional) Separate 1.3.2 / 2.0 / 2.6 by matching one of the six compiler-unrolled game loops.
- Chunk 2: done (see above).

### Verify chunk 1
```sh
cd ~/geist-work/mwprobe && python3 codegen_cmp.py
# expect: 80 files: identical code under all four = 61, differs = 17, compile errors = 2
```

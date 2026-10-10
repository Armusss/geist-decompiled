# STATUS (2026-10-10)

Handoff file for the matching decompilation. Newest section first. See README.md for setup/build.

## Team board (Decomp Lead + helpers) — 2026-10-10

| Who | Worktree / branch | Assignment | State |
|---|---|---|---|
| Lead (main session) | `geist-matching` / `main` | SDK done (77/77 linked); now small unknown SDK units, tail stubs 0x8033F5B8-0x80341200, merging | working |
| helper-msl | `.claude/worktrees/optimistic-khayyam-7b9401` / `claude/optimistic-khayyam-7b9401` | **MSL_C + Runtime.PPCEABI.H + TRK_MINNOW_DOLPHIN**: `.text` from the first TRK/Runtime function (about 0x802E4800, find the exact start) up to 0x802F6B9C (`AMC_IsStub` and everything from 0x802F6BA4 is SDK, not yours) plus their data | working (Remote Control on) |
| helper-musyx | `.claude/worktrees/goofy-clarke-759020` / `claude/goofy-clarke-759020` | **MusyX + zlib 1.1.4**: `.text` 0x8031DE10-0x8033F5B8 (382 functions, none named yet) plus their data; also find where zlib's inflate lives (it may be elsewhere) | working (Remote Control on) |

**Rules for helpers:**
1. Work only in your own worktree and branch. Never edit `main`, never push, never touch `orig/`, game
   installs or other folders. Kill processes only by exact PID.
2. File ownership, so merges stay clean:
   - `configure.py`: only between your own `# ==== helper-<name> ... ====` / `# ==== end helper-<name> ...`
     markers (one libs block and one cflags block each).
   - `config/GITE01/splits.txt` / `symbols.txt`: only entries whose address is inside your range (and
     the data those units own). Don't reorder or rewrite other lines.
   - Sources only under your own dirs: helper-msl -> `src/MSL_C/`, `src/Runtime.PPCEABI.H/`,
     `src/TRK_MINNOW_DOLPHIN/`, `include/MSL_C/`... ; helper-musyx -> `src/musyx/`, `src/zlib/`,
     `include/musyx/`, `include/zlib/`. Don't edit `src/dolphin/`, `include/dolphin/` or `STATUS.md`
     (report to the Lead instead; the Lead keeps this board).
   - Tools: copy what you need from `~/geist-work/mwprobe/` into your own folder
     (`~/geist-work/helper-msl/` or `~/geist-work/helper-musyx/`); don't write into `mwprobe/`.
     `probe.py`, `auto_match.py` and `dol_diff.py` take the repo path from the `GM` env var.
3. Setup in a fresh worktree: `orig/GITE01/` is git-ignored, so symlink it:
   `ln -s "$HOME/Documents/Claude code/geist-matching/orig/GITE01/sys" orig/GITE01/sys` (same for `files`).
4. Every step must keep `build/GITE01/main.dol: OK` (SHA-1 6612f52ce6c50bb831ffddd5264b22973304f288).
   Check units with `objdiff-cli` (`ninja` builds the report). Commit on your branch after each green
   batch; message ends with the Co-Authored-By line used in this repo.
5. Prefer real library source you can reconstruct (MSL/TRK/Runtime from the CodeWarrior GC versions,
   MusyX from public decomps' structure, zlib 1.1.4 is public). Don't run scripts from cloned
   reference repos; read them only. Record the compiler version and flags with evidence.
6. When done (or blocked), send the Lead a short report: units matched, compiler/flags, anything odd,
   exact verify command + expected output. Then stop.

## Chunk 7 — SDK: all 77 units build from C (Lead, 2026-10-10)

**Result:** `SDK Code: 99.97% fuzzy, 99.96% matched, 100.00% linked (77 / 77 files)`, DOL byte-identical.
New: GXInit, GXAttr, GXFifo, dvd, arq, dvdfs, OSFont, mtx, mtx44 (new unit), vec, quat, OSSync (new
unit), OSRtc.

**Findings:**
- **The "linker hang at 0x80314318" was a duplicate label**, not a boundary problem: symbols.txt had
  `SystemCallVector` both as a label and as a function there. Renaming the label to
  `__OSSystemCallVectorStart` lets OSSync.c be its own unit; nothing hangs.
- Geist's SDK `GXData` is 0x5B8 bytes (dolsdk2004: 0x5B0); `__gx.h` pads it with 8 bytes at the end.
- GXFifo: `CPUFifo`/`GPFifo` are statics declared before `__GXCurrentThread` (retail .sbss order).
  0x803069DC is `GXGetCPUFifo`, not `GXGetCurrentGXThread` (that one is stripped).
- Video-mode tables in GXFrameBuf .data: GXNtsc480IntDf 0x803C7CE8, GXNtsc480Prog 0x803C7D24 (the game
  picks it for progressive scan), GXMpal480IntDf, GXPal528IntDf, GXEurgb60Hz480IntDf.
- dvd.c's .text runs to 0x803045BC (DVDCancelAllAsync, DVDGetCurrentDiskID, DVDCheckDisk,
  __DVDPrepareResetAsync, __DVDTestAlarm); its .sbss starts at `executing` 0x804D59C0 (the two
  objects before it are dvdfs.c's). arq.c keeps `ARQGetChunkSize` (0x802F8A94).
- mtx44.c (C_MTXPerspective, C_MTXOrtho, PSMTX44Identity) sits at 0x8030D114-0x8030D2B0 after
  mtx44vec.c; sdata2: mtx 0x804DB868, mtx44 0x804DB87C, vec 0x804DB898, quat 0x804DB8A0-0x804DB8B8.
- objdiff `gap_... size out of section bounds` = alignment padding after the *previous* unit's
  section; extend that unit's end over it (dsp_task .data, OSError .data/.sdata, fstload .sdata).
- Tool bugs fixed: `auto_match.py` ninja timeout 180 s -> 600 s (it was reverting good attempts);
  `names_from_relocs.py` only renames placeholder names (fn_/lbl_/jumptable_/@) now.

**Left in the SDK area (Lead):** unknown small units with no dolsdk2004 source: dvdfatal.c
(`__DVDPrintFatalMessage` 0x80304A44 + `FatalFunc` .sbss 0x804D5A10), the 0x50-byte function
0x802FFA44 between dsp.c and dsp_task.c, `fn_8031439C` (4 bytes, after OSSync), and the stub libs
0x8033F5B8-0x80341200 (`Hu_IsStub` etc.). objdiff still scores OSExec `Run` at 50% (asm; DOL is
identical), so check the reloc naming.

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 99.97% fuzzy, 99.96% matched, 100.00% linked (77 / 77 files)
```

## Chunk 6 — SDK: 64 / 75 files matched

**Result:** `SDK Code: 97.86% fuzzy, 97.85% matched, 76.91% linked (64 / 75 files)`, DOL byte-identical.
New: dsp, dsp_task, dvdlow, OS, Pad, OSCache, SIBios, vi, CARDBios, __ppc_eabi_init, EXIBios.

**Fixes that mattered:**
- **Pooled sections are force-active** (`...data.0` / `...bss.0` markers): nothing inside them is
  stripped, so their split covers the full compiled size (e.g. dsp.c keeps "DSPInit(): Build Date"
  strings that no code uses in release builds).
- **Alignment padding** between a section and the next split (< 32 zero bytes) is absorbed, otherwise
  objdiff-cli's report fails on a tiny gap unit.
- **vi.c reproduces a bug in Geist's SDK build:** `VISetPreRetraceCallback` reads and writes `PostCB`.
- **EXIBios** must be built with `-O3,p` *instead of* `-O4,p` (`cflags_dolphin_O3`); appending
  `-O3,p` after `-O4,p` does not override it.
- CARDBios.c starts at 0x802F8BB0 (`__CARDDefaultApiCallback`); __ppc_eabi_init.c also has `.init`
  0x800054EC-0x80005544 (`__init_hardware`, `__flush_cache`).

**Remaining (11):**
- `dvd/dvdfs.c`, `os/OSFont.c`: the DOL links identically, but objdiff-cli's report refuses
  (compiled `.data` is 4 bytes longer than the split because the linker strips a trailing object).
  Needs a symbols/splits workaround that objdiff accepts.
- `gx/GXInit.c` + `gx/GXFrameBuf.c`: GXInit uses the video-mode tables (`GXNtsc480IntDf` at
  0x803C7CE8 etc.) defined in GXFrameBuf.c, whose `.data` isn't split yet; a first attempt pulled in
  GXAttr's jump tables. Give GXFrameBuf a .data split exactly covering the four tables, then GXInit.
- `gx/GXFifo.c` (undefined `lbl_804D5A48` from GXInit), `dvd/dvd.c` (`__DVDThreadQueue`),
  `ar/arq.c` (overlaps ar.c data): symbol naming/placement between neighbours.
- `gx/GXAttr.c`: dtk's `GXTexRegionAddrTable` size; `mtx/mtx.c`, `vec.c`, `quat.c`: shared .sdata2
  constant tangle; `os/OSRtc.c`: waits on the OSSync linker hang.

**Safety:** commit after every successful batch. `KEEP=1` leaves a failed attempt in place for
`dol_diff.py`; afterwards restore with `git checkout -- config/GITE01/splits.txt
config/GITE01/symbols.txt configure.py` (only safe because everything good is committed).

## Chunk 5 — SDK: 53 / 75 files matched

**Result:** `SDK Code: 95.99% fuzzy, 93.02% matched, 48.70% linked (53 / 75 files)`, DOL byte-identical.

**Correction to chunk 3:** "37 files have a differing function" was wrong. My probe listed compiled
functions in symbol-table order instead of code order. With that fixed (and the project's real flags,
e.g. `-char unsigned`, EXIBios `-O3,p` without `-O4,p` before it), **74 of 75 SDK units match their
source completely**; the 75th, OSRtc.c, only fails because it deliberately also covers OSSync.c.

**Changes this chunk:**
- Unit edges fixed: mtx.c extended to 0x8030D06C; dsp.c ends 0x802FFA44 and dsp_task.c starts 0x802FFA94
  (the 0x50-byte function between belongs to a file not in dolsdk2004); dvd.c starts 0x80301A50 (takes
  the 4-byte function); dvdidutils.c ends 0x80304A44 and fstload.c starts 0x80304A74 (0x30-byte unknown
  function between); ar.c starts 0x802F6BA4 and ai.c 0x80319D50 (their preceding functions use the
  files' own statics, so they belong to them).
- `obj_layout.py` follows pointers stored in data (e.g. `__ARVersion` -> the SDK version banner) to
  place objects that only other data references. This unblocked ar.c and ai.c.
- Globals not used by their own file's code but already named in symbols.txt keep that address.
- `auto_match.py` needs two passes (some files only link once their neighbours are C).
- Process note: I once ran `git checkout` on splits/symbols/configure.py, which discarded uncommitted
  progress; it was regenerated by re-running the scripts. Commit after each chunk.

**Still assembly (22):** ar/arq.c, card/CARDBios.c, dsp/dsp.c, dsp/dsp_task.c, dvd/dvdlow.c, dvd/dvdfs.c, dvd/dvd.c, gx/GXInit.c, gx/GXFifo.c, gx/GXAttr.c, mtx/mtx.c, mtx/vec.c, mtx/quat.c, os/OS.c, os/OSCache.c, os/OSFont.c, os/__ppc_eabi_init.c, pad/Pad.c, vi/vi.c, exi/EXIBios.c, si/SIBios.c, os/OSRtc.c.
Typical blockers (`~/geist-work/mwprobe/pending_errors.json`, re-run `ONLY=<file> KEEP=1 python3
auto_match.py` then `python3 dol_diff.py` to see the first differing bytes):
- links but DOL differs: dsp, dvdlow, dvdfs, mtx, OS, OSCache, OSFont, Pad, vi, EXIBios, SIBios
  (probably data placement details: stripped/kept objects, or strings in pools);
- duplicate/undefined symbols between a C file and neighbouring asm (CARDBios `...bss.0`, GXInit
  `GXNtsc480IntDf`, GXFifo, dsp_task, dvd `__DVDThreadQueue`, __ppc_eabi_init `__init_hardware`);
- GXAttr: dtk symbol `GXTexRegionAddrTable` has the wrong size; vec/quat: constant pool tangle.

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 95.99% fuzzy, 93.02% matched, 48.70% linked (53 / 75 files)
```

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

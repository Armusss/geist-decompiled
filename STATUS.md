# STATUS (2026-10-10)

Handoff file for the matching decompilation. Newest section first. See README.md for setup/build.

## Team board (Decomp Lead + helpers) — game code, from 2026-10-11

| Who | Worktree / branch | Assignment | State |
|---|---|---|---|
| Lead (main session) | `geist-matching` / `main` | Coordinating: verifies and merges helper branches when they report; owns game units **0x802D0000-0x802E4858** (Credits.cpp WIP 23/30) and takes small files only | coordinating |
| helper-game-a | `.claude/worktrees/optimistic-khayyam-7b9401` / `claude/intelligent-babbage-a49862` | **Game units 0x80005800-0x80150000** except the Lead's files above | working (Remote Control on) |
| helper-game-b | `.claude/worktrees/goofy-clarke-759020` / `claude/frosty-gauss-da470f` | **Game units 0x80150000-0x802D0000** (the Lead took 0x802D0000-0x802E4858 on 2026-10-11) except zlib (done), game/zutil.c (zlib, GC/2.6) and the Lead's files above | working (Remote Control on) |

Earlier helpers (2026-10-10): helper-msl (MSL/Runtime/TRK) and helper-musyx (MusyX/zlib), both done and merged.

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
6. **Game code specifics:** read "G3 — first game file" and "G1"/"G2" above first; `src/game/Memory.cpp` is the
   worked example. Work file by file in address order inside your range. Fix a unit's boundaries in
   splits.txt by hand when matching proves them wrong (only between units inside your range; if a boundary
   with a neighbour outside your range must move, ask the Lead). Do NOT run `tools/gen_game_splits.py`.
   You may give names to functions/objects outside your range only if they are still `fn_`/`lbl_`
   placeholders (callees you need, with mangled names); list them in your report.
7. When done (or blocked), send the Lead a short report: units matched, compiler/flags, anything odd,
   exact verify command + expected output. Then stop.

## G4 — game compiler is GC/2.0 (Lead, 2026-10-13)

helper-a found two qsort comparators in u_8012071C (0x80120A78/0x80120A9C) that match only with GC/2.0 or 2.6:
retail schedules lfs from parameter pointers above the `stwu`, which 1.3.2 never does. Test results:
- with all game units on GC/2.0, main.dol is still OK and all 23 matched game files still match;
- per-source `mtry.py` match counts under 1.3.2 vs 2.0 are identical for every game file, except u_8012071C
  (6 vs 8 functions).

So the game block in configure.py now uses **GC/2.0**. GC/2.6 is still ruled out (plane/timing functions). The
late parameter-copy pattern (see G3) is not fixed by 2.0 either.

## G3 — first game file: Memory.cpp matched (Lead, 2026-10-11)

**Result:** `Game Code: ... (1 / 190 files)`, DOL OK. `src/game/Memory.cpp` (0x8009D710-0x8009DE94): heap
wrapper with "Out of Memory <file> <line>" reports, two scratch ring buffers (0x160000/0x40000), and the
"VMem" pool object (0x6C0000 block) whose methods live at 0x801BEC4C-0x801BF5xx.
**Settled for game code:**
- GC/1.3.2 with cflags_game works (`-lang=c++ -O4,p -inline auto -fp_contract on`, string pooling).
  **No deferred inlining:** functions come out in source order (unlike the GC/2.6 libraries), and small
  functions defined earlier in the file are auto-inlined (MemAlloc/MemFree inlined into MemInit/MemShutdown).
- A C++ file ends after its `__sinit_*`: CodeWarrior then emits static-destructor helpers and weak inline
  functions (ctors/dtors, e.g. `__dt__4VMemFv`), so those belong to the file.
- String pools keep strings of dead-stripped functions; add an unused function using them, in pool order
  (Memory.cpp: `MemOutOfMemory`).
- symbols.txt needs the **mangled** names (e.g. `MemAlloc__FiPCci`, `Free__4VMemFPv`) for C++ functions,
  including external ones the file calls.
- Typical fixes: the order of declarations and temporaries decides registers; `if (x > 0) {...}` vs
  `if (x < 1)` changes the compare; early `return`s vs a result variable; ternaries in call arguments.
- Tool: `~/geist-work/mwprobe/mdiff.py UNIT FUNC ADDR [SIZE]` diffs one function of a unit against the DOL.
- **asyncio.cpp matched** (0x800E1144-0x800E1B5C; the provisional unit started too early: Vec3 helpers
  0x800E0CB8-0x800E1144 and a ring copy at 0x800E0B54 are separate files, rumble code follows at 0x800E1B5C).
  Buffered async DVD streams with zlib "BILZ" files. Lessons: **`-use_lmw_stmw on`** is required (now in
  cflags_game); repeated blocks are usually **inline helper functions** (AsyncCheckHeader, AsyncQueueNext,
  AsyncDumpBuffers): writing them out by hand gives different registers; `switch` vs if/else matters.
- **Image.cpp matched** (0x800F2B08-0x800F396C, .rodata 0x80345320-58, .sdata2 0x804D7348-90; the stub at
  0x800F2B00 returning 1.0f belongs to the previous file: a file never holds the same float constant twice,
  and 1.0f appears again at 0x804D735C). TPL loading with a local copy of the SDK's `TPLBind` (assert line 337),
  plus screen quads. Lessons: game files can `#include <dolphin/gx.h>` etc. (cflags_game now has
  `-i include/libc`); SDK struct typedefs mangle by their tag (`GXTexObj*` -> `P9_GXTexObj`); quad corners
  passed as float **locals** stay in registers while literals reload; an early `u8 a = alpha;` explains a
  `clrlwi` at entry; `/ 2.0f` compiles to `* 0.5f` but allocates differently; statement order (x0, x1, y0,
  y1) decides float registers. Graphics wrappers named in symbols.txt: GfxLoadTexObj 0x8000F944,
  GfxSetVtxDesc 0x80008DAC, GfxSetCullMode 0x80008B44, GfxFrameEnd/Swap/Begin 0x8000ADDC/0x8000A028/0x80009EE0,
  GfxSetMaterial 0x80112AB0, GfxResetTev 0x8010D130, GfxSetBlendMode 0x801D8A10; data gDVDMutex 0x803D7114,
  gWideScreen 0x804D4008.
- **DataStream.cpp matched** (.text 0x8017BE38-0x8017D794: the provisional end 0x8017D6C4 was too early, the
  StreamQueue dtor, the sMem array-dtor helper and the out-of-line VMem ctor follow `__sinit`; .rodata
  0x80346EF0-40, .sdata 0x804D3008-10, .bss 0x80491880-0x80494A80, .sbss 0x804D5058-A8; next unit renamed
  u_8017D794). Disc streaming: two request queues (binary trees by disc offset, flattened before reading), two
  VMem pools, a 0x2000 bounce buffer. Lessons: **this file needs `#pragma auto_inline off`** (retail calls tiny
  StreamFlushQueues instead of inlining it) while Memory.cpp needs auto-inlining (MemAlloc inlined into
  MemInit), so keep `-inline auto` in cflags_game and use the pragma per file. Explicit `inline` recursive
  functions are expanded 2-3 levels deep into themselves; a tiny `inline` callee defined first is inlined at
  the top levels and called at the deepest one. Field reloads after a branch mean `volatile` (here the current
  request pointer, sPendingBytes, sFileOpen). All the file's .bss is reached from one base register (the first
  static), which confirms which statics belong to the file. A run of string literals in .rodata that nothing
  references (POSSESS, GHOSTACTION, ...) repeats in several files: a header-defined static table.
- **File.cpp matched** (.text 0x800190D4-0x80019A04; .rodata 0x80341E98-0x80341F30, .bss 0x803DAA88-0x803DAE88,
  .sbss 0x804D41A0-B0, .sdata2 0x804D60A0-A8). The provisional unit started at 0x8001598C; everything before
  0x800190D4 touches other data (sdata2 0x804D6030-609C, sbss 0x804D4198) and is now u_8001598C (still several
  files: module hooks called from 0x80087BB0/0x80087FA8 tables etc.). OLC level loader: chunk handler table,
  per-type disc offsets (ArchiveFindOffset), whole-file readers. Lessons: `x = f(); if (x != y)` vs
  `if (f() != y)` swaps cmpw operands; error paths placed after the main path come from `if (ok) {...} else
  {error}` shapes; declaration order decides callee-saved registers, so search it with
  `~/geist-work/mwprobe/perm.py SRC FUNC ADDR SIZE` (declaration lines on stdin). Also new:
  `udiff.py SRC START END` (all functions of a unit) and `adiff.py MDIFF_OUT` (aligned diff that tolerates
  length differences).
- **CollectionHost.cpp matched** (was u_802E2F08: .text 0x802E2F08-0x802E32D0, .rodata 0x8034DC10-88, .data
  0x803C6388-B8, .sdata 0x804D3DC8-D0, .sdata2 0x804DB2E0-F0). Lessons: call arguments are evaluated **right
  to left** (`sprintf(buf, fmt, F2(), G(), F1())` calls F1 first); message structs in different `case`s get
  their own stack slots when declared at function scope (first declared = highest address); a variable used
  in several cases cannot reuse a parameter's register, so give each case its own block-scoped variable.
- **CollectionGhost.cpp matched** ("Collection Ghost Health", .text 0x802E32D0-0x802E3674; the two functions
  at 0x802E3674/0x802E36A4 belong to the next file, which is now u_802E3674: its table at 0x803C63E8 lists them).
  Lesson: calls nested in an argument list run before simple loads of the other arguments, rightmost call
  first, so `F(G(p), H(&x), x)` calls H, then G, then loads x.
- **Credits.cpp WIP 23/30** (was u_802E3674, 0x802E3674-0x802E4858 = end of game code; credits screen, tip
  messages, "No Bot" behaviour; .data ends at 0x803C6468, the jump tables after it are MetroTRK's). Open
  problem shared with helper-b: **`cmplwi r4,0` then parameter copies after the branch** (retail) vs our
  `mr. r31,r4` with copies at entry. Not fixed by any compiler version 1.3-3.0a5, -O2/-O3/-O4,s, -opt
  noschedule/nopropagation/nolifetimes/nodeadcode, inline bodies, block-scoped copies, early return, member
  functions, C mode, -g/-sym, -opt nocse/noloop/noalias, -inline deferred, compilers 1.0-1.2.5n (3.0a3 tests
  first but still copies before the branch). It hits non-leaf `void f(Owner*, Beh*) { if (b) {...} }`
  behaviour callbacks; the int-returning `if (!b) return 0;` ones fuse normally (`mr.`) and match. Other lessons: a query whose String must die before later code goes in its own inline function;
  `if (a == 0 || f() == -1) return 0; return 1;` gives retail's li 0/li 1 shape where `return a && f() != -1`
  gives a bool (clrlwi); a hash written `h = h % 0xFFFFFFFF; return h;` keeps h in r3.

## G2 — game code split into provisional units (Lead, 2026-10-11)

**Result:** `Game Code: 0/192 files`, `SDK Code ... (199 / 202 files)`, DOL OK.
- `tools/gen_game_splits.py` writes `build/tu_proposal.json` into splits.txt (.text only, address order
  next to zlib) and into configure.py between `# ==== game units` markers (lib "game", GC/1.3.2,
  `cflags_game` = cflags_base + `-lang=c++ -str reuse,pool,readonly`; `.c` files get `.c` names). Re-running
  regenerates only the game units. 27 units carry assert-derived names (game/File.cpp, game/DataStream.cpp,
  game/Script.c, game/VM.c, ...); the rest are `game/u_<address>.cpp`.
- Boundaries are provisional: when matching a unit, confirm or move its edges (edit splits.txt by hand,
  and stop regenerating once units are being matched).
- Progress map (`tools/progress_svg.py`) now shows one square per game file.

## G1 — game compiler (Lead, 2026-10-11)

- **Game code is NOT GC/2.6.** zlib (GC/2.6) is a prebuilt library; the game's own code matches only
  **GC/1.3 or GC/1.3.2** (working choice 1.3.2, the same as MusyX). Evidence (`tools/g1/`, probe with
  `-lang=c++ -O4,p -inline auto -fp_contract on`):
  - 0x8001734C plane-from-normal (`g1e.cpp`): matches with 1.2.5n/1.3/1.3.2 only (2.0+ hoist loads over stores).
  - 0x8001598C frame-time from OSGetTick (`g1f.cpp`): matches with 1.3/1.3.2, not 1.2.5n.
  - 0x800185F8 wrapper (`g1.cpp`): matches with all.
  1.3 vs 1.3.2 and the `-use_lmw_stmw` / `-inline deferred` settings are not settled yet (no test function
  tells them apart so far). C++ name mangling is used; `.c` files exist too (assert names).
- `-fp_contract on` (fmadds) and `-O4,p` confirmed.

## G0 progress — game file boundaries (Lead, 2026-10-11)

- `tools/tu_mono.py`: each file's private data (`.sdata2` constants, `.rodata`, jump tables in `.data`) is
  laid out in code order, so a boundary between two functions is only possible if everything private used
  before it lies below everything used after it. Real ranges: `.rodata` 0x80341280-0x8034EE00, `.data` from
  0x8034EE00, `.sdata2` 0x804D5E80-0x804DB338 (above that are weak merged constants such as sqrtf's
  `_half`/`_three`). Addresses used by far-apart functions and 2 trend outliers are ignored.
  Result: **2,414 allowed boundaries** (upper bound); largest forced-together span 130 KB (138 functions).
  Writes `build/tu_mono.json`.
- `tools/tu_pools.py`: **203 string pools** (lis/addi bases into pooled strings). All functions between a
  pool's first and last user are one file. Of 193 gaps between consecutive pools, 37 have exactly one
  allowed boundary (pinned), 21 have none: probably files with both a `.rodata` and a `.data` pool, so pools
  must be paired before "different pool = different file" holds. Writes `build/tu_pools.json`.
- No zero padding between game functions (4-byte alignment), so padding gives no boundary signal.
- Refinements (validated on zlib, whose 9 real boundaries are known): only *compiler-generated* private
  data counts: `.rodata` strings (not named tables like zlib's `_length_code`), `.sdata2` constants, jump
  tables. "Shared" is decided per symbol object (per raw address in `.sdata2`). Vtables are NOT usable
  (weak, shared between files). Now **2,566 allowed boundaries**, all 9 zlib boundaries allowed; pool
  contradictions down to 8 (one area 0x801B4xxx-0x801D7xxx, probably built without string pooling).
- `tools/tu_propose.py` -> `build/tu_proposal.json`: **194 proposed game units** (35 pinned by pools,
  12 known from zlib, 146 guesses at the first allowed boundary; median 0x2694 bytes, largest 0x29A6C).
  Run: `python3 tools/tu_mono.py && python3 tools/tu_pools.py && python3 tools/tu_propose.py`.
- **Validation on library code** (0x802E4858-0x8033F990, all 189 real boundaries known; run with
  `LO=0x802E4858 HI=0x8033F990 NOKNOWN=1`): the order constraint keeps **100%** of real boundaries (safe,
  but in library C code it rules out little). String-pool spans now never cross a real boundary (0/189,
  after fixing a bug that treated `.bss` addresses as strings), so **"same pool = same file" is reliable**.
  The reverse is not: "different pools = boundary" was right in only 2 of 5 pinned cases (some files have
  two string pools), and call-graph placement of the other cuts was right 13/36.
- Also added: static initializers (16/17 agree with the constraint; all 17 used as known file ends), file
  names from assert strings (27 units named, e.g. main.cpp, File.cpp, DataStream.cpp, Image.cpp,
  asyncio.cpp, Memory.cpp, Script.c, VM.c, FearMap2.c, Msx.cpp), and **zutil.c** (zlib) found unsplit at
  0x8019FD58. Static variables (.bss/.sbss/.sdata) were tried and rejected (globals used by neighbouring
  files make them unreliable; `STATICS=1` re-enables).
- Current proposal: **205 units** (29 known, 35 pinned, 140 guessed). Treat cut positions as provisional:
  a cut is only confirmed when the whole file matches. Hard facts usable now: allowed-boundary set (upper
  bound), pool spans (lower bound: never split inside one), static-initializer ends, zlib.
- **Next:** G1 (confirm game flags on small functions), then G2: write provisional ranges as asm units
  (DOL must stay OK; more units than real files is harmless for asm) and fix cuts while matching.

## Game code plan (Lead, 2026-10-11)

**Size:** 7,163 functions, 2.9 MB of `.text` (0x80005800-0x802E4858, zlib already split out inside it).
Mixed C and C++ (assert strings name e.g. main.cpp, AppMain.c, Script.c, VM.c, FearActor.c, FearMap2.c,
Turf.cpp, Cube.cpp, CubeTev.cpp, Image.cpp, File.cpp, DataStream.cpp, asyncio.cpp, Memory.cpp, Msx.cpp,
Shadow2.cpp, SAM.cpp, Progress.cpp, MemCard.c, Store.c, Audio.c, Arena.c, ...). 17 static initializers in
`.ctors` (each ends a C++ file). Compiler evidence from zlib (linked inside game code): GC/2.6, `-lang=c++`,
`-O4,p -inline auto,deferred -use_lmw_stmw on -str pool,readonly`.

**Phases:**
- **G0 file boundaries:** `~/geist-work/game/tu_cluster.py` groups functions sharing private constants
  (sdata2/rodata/data objects used only within a 64 KB window): 1,784 candidate boundaries (upper bound; many
  functions have no private constants). Combine with string pools (one pool per file), `.ctors` entries,
  jump tables, vtables/RTTI and assert file names to settle real files.
- **G1 flags:** confirm GC/2.6 + the flags above on a few small game functions (C vs C++ files differ).
- **G2 split all game code into asm units** with those boundaries (DOL must stay OK). Then units can be
  matched independently, and helpers can own disjoint address ranges.
- **G3 match,** starting with code the Rust port needs most: file/stream/asset loading (File.cpp,
  DataStream.cpp, asyncio.cpp, Image.cpp, Memory.cpp, chunk loaders), then engine systems, then gameplay.
- **Helpers:** worth it after G2 (two helpers on separate address ranges). Not before, because splitting
  touches the whole splits/symbols files.
- **Library leftovers (3):** file_io.c (fflush), Gecko_ExceptionPPC.cp, EXI2_GDEV main.c; revisit later.

## Chunk 12 — MSL_C common almost done (Lead, 2026-10-11)

**Result:** `SDK Code: 98.70% fuzzy, 98.52% matched, 98.14% linked (199 / 202 files)`, DOL OK.
New: direct_io, buffer_io, mem, ansi_files, extras, ctype, uart_console_io_gcn, abort_exit, FILE_POS,
mbstring, qsort (the unit split as "scanf.c"), printf (+clearerr in misc_io), ansi_fp, alloc.
- **MSL_C common flags:** `-O4,p -opt level=0,peephole,schedule -inline off` (cflags_msl). extras.c is
  different: cflags_rt (-O4,p, inlining) + `-DMSL_USE_INLINES=1`; ctype.c also needs `-DMSL_USE_INLINES=1`
  (weak inline ctype functions; only tolower/isupper/isdigit survive).
- Even with -inline off, files are emitted in **reverse source order**.
- **Comma expressions** (e.g. macros like `((a)->x = b, (a)->y = c)`) make the compiler reserve saved
  registers for temporaries; that explains "reserved but unused" registers (alloc's FixSubBlock_construct is
  such a macro; Block_empty too).
- Older-revision details: printf's `va_start(ap, fmt)` evaluates `(void)fmt` (Prime's stdarg); no locale radix
  (`'.'`); fprintf/vfprintf check `fwide(file, -1) >= 0`; abort -> exit -> __exit; UART init split into
  __init_uart_console; critical-region ids differ from TP's enum (setvbuf uses 2).
- Wrong dtk names fixed: 0x802EF06C is __flush_buffer and 0x802EF138 __load_buffer (were swapped);
  "strnicmp" 0x802F6ACC is strupr; the "scanf.c" unit is qsort; 0x80316480 is _ExitProcess (now in
  __ppc_eabi_init.c's split).
- Tool: `~/geist-work/mwprobe/mtry.py UNIT_SRC CANDIDATE [flags]` compiles a candidate with the unit's exact
  ninja flags and reports per-function matches inside the unit's range (it does not check order; the DOL
  check does). `INC=build/scratch/plibc` swaps in Prime's libc headers (copied there with a minimal types.h).
- **Left (3):** file_io.c (fflush: retail reserves r29 but compares r3 directly; everything else matches),
  Gecko_ExceptionPPC.cp (full C++ EH runtime, no reference source), EXI2_GDEV main.c (gdev_cc_read
  register allocation).

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 98.70% fuzzy, 98.52% matched, 98.14% linked (199 / 202 files)
```

## Chunk 11 — MSL flags fix + Prime reference (Lead, 2026-10-11)

**Result:** `SDK Code: 95.02% fuzzy, 93.87% matched, 92.44% linked (185 / 202 files)`, DOL OK. New: string.c.
- **Flag fix:** MSL_C common files are `-O4,p` *followed by* `-opt level=0,peephole,schedule`
  (`cflags_msl = [*cflags_rt, "-opt level=0,peephole,schedule"]`). The helper's probes always had -O4,p first;
  configure had dropped it, which changes register saves (stmw for 2 registers). All matched MSL units
  still match with the fix.
- **Reference:** `~/geist-work/refs/prime` (sparse clone of PrimeDecomp/prime, read-only:
  extern/sdk/{runtime,libc,include}) has an *older* MSL revision. string.c: strlen..strchr from Prime match;
  strcat/strrchr from TP; strncat/strstr written from the MSL algorithm (all 10 match).
  Prime's Gecko_ExceptionPPC.cpp is only a 40-line stub (no help).
- Probe Prime files with: `cd ~/geist-work/refs/prime/extern/sdk && GM=... probe.py 2.6 runtime/<f>.c
  -opt level=0,peephole,schedule -gccinc -i include -i libc -i runtime -use_lmw_stmw on
  -str reuse,pool,readonly -common off -char signed -func_align 4 -inline deferred,auto` (probe adds -O4,p
  itself). First results: alloc 3/15, uart_console_io 1/2, mem 0/4 (mem matches neither TP nor Prime yet:
  retail memchr keeps a dead copy of `n` in r29). Others failed to compile only for lack of `-gccinc`.
- **Next:** per MSL file, pick per function the TP or Prime version that matches; files still left:
  abort_exit, alloc, ansi_files, ansi_fp, buffer_io, ctype, direct_io, extras, file_io, FILE_POS,
  mbstring, mem, printf, scanf, uart_console_io_gcn (+ Gecko_ExceptionPPC, gdev main.c).

## Chunk 10 — library leftovers (Lead, 2026-10-11)

**Result:** `SDK Code: 94.76% fuzzy, 93.37% matched, 92.11% linked (184 / 202 files)`, DOL OK.
New: runtime.c, __init_cpp_exceptions.cpp, global_destructor_chain.c, NMWException.cp, **exception.cp
(new unit)**, TRK msghndlr.c, support.c, mslsupp.c. "ptmf.c" was not a real unit (ptmf is stripped);
its function is NMWException's dthandler, so it was folded in.
- Unreferenced .ctors/.dtors entries in C: `#pragma force_active on` + `__declspec(section ".ctors")`
  (plain section names; dtk's ldscript does not merge `.ctors$10`).
- Geist's older C++ runtime: std::exception (dtor, what, RTTI on) and a `throw()` operator delete form
  their own unit 0x802EABBC-0x802EAC88 before NMWException; duhandler inlines std::terminate (handler
  declarations inside `namespace std`); GetR2 is a called `asm char* GetR2(void)`.
- 0x802ECF9C is MSL `abort` (overrides the SDK's weak abort); 0x802EBF6C is `__unexpected`.
- MetroTRK v2.0 differences: Read/WriteMemory use a plain `u8 buf[0x800]` (no ARAM, no 32-byte offset);
  TRKSuppAccessFile flags EOF on short transfers; mslsupp has __open_file/__close_file/__position_file/
  __close_console (targsupp stubs TRKOpenFile 0x802E8DC8, TRKCloseFile, TRKPositionFile).
- `~/geist-work/mwprobe/rdis.py ADDR SIZE` disassembles retail with symbol names.
- **Left (18):** 16 MSL_C common files (older MSL revision: e.g. memchr keeps a dead copy of `n`; no -opt
  setting reproduces it, so they need that revision's source), Gecko_ExceptionPPC.cp (full C++ EH
  runtime), EXI2_GDEV main.c (`gdev_cc_read` register allocation: retail keeps p1/p2 apart from size).

## Chunk 9 — merged helper-msl: MSL_C / Runtime / TRK (2026-10-10)

**Result after merge:** `SDK Code: 94.32% fuzzy, 92.52% matched, 89.34% linked (176 / 202 files)`, DOL OK.
- All GC/2.6. TRK 24/28 (TP's cflags_trk; serpoll and DDH/GDEV main.c add `-sdata 8`). Runtime 2/8
  (`-O4,p`; GCN_mem_alloc adds `-str reuse,nopool,readonly`). MSL_C 36/52: math with cflags_runtime;
  common files use `-opt level=0,peephole,schedule` (strlen/rand/time only match with it); signal, time,
  wchar_io and ansi_files also need `-use_lmw_stmw off`.
- Geist's MSL common source is an older revision than TP's; the 16 left need hand reconstruction.
- **Left (asm):** TRK msghndlr, support, mslsupp, EXI2_GDEV main.c; Runtime global_destructor_chain
  (C strips the last .dtors entry 0x80341268), NMWException, ptmf, runtime, __init_cpp_exceptions,
  Gecko_ExceptionPPC; 16 MSL_C common files; targsupp.s/exception.s not split; 0x802F4B38 (0xAC) unknown.
- `-inline deferred` emits functions in reverse order, so check names of look-alike functions by call sites.
  dtk's `AMGetZeroBuffer` 0x802E6964 is really `GetTRKConnected`.
- .init: mem_TRK 0x80003100-0x80003154, dolphin_trk 0x80005088-0x800050B4.
- Merge fix: GXInit's pseudo-global `time` renamed `__GXInitTime` (clashed with MSL `time()`).
- The linker sometimes fails once after a config change and passes on a rerun.

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 94.32% fuzzy, 92.52% matched, 89.34% linked (176 / 202 files)
```

## Chunk 8 — merged helper-musyx: MusyX + zlib (2026-10-10)

**Result after merge:** `SDK Code: 99.99% fuzzy, 99.98% matched, 100.00% linked (114 / 114 files)`, DOL OK.
- MusyX 2.0.3, GC/1.3.2, 0x8031EAF4-0x8033F990 (25 code units + dsp_import.c data). A compat math.h maps
  MSL inline fmodf/powf/floorf to double versions.
- zlib 1.1.4 at 0x80197E50-0x8019FD58 (inside game code; 11 units incl. deflate), **compiled as C++ with
  GC/2.6**: `-lang=c++` (must come first, it resets earlier options) `-O4,p -inline auto,deferred
  -use_lmw_stmw on -str pool,readonly`. **Strong hint for the game code: GC/2.5+, C++, stmw, pooled
  readonly strings, deferred inlining.**
- objdiff scores some data sections < 100% (hw_dspctrl, infblock, inflate, infcodes, hardware) though the
  DOL is identical: data-symbol pairing artefacts.
- symbols.txt merges: `~/geist-work/mwprobe/sym_merge3.py BASE OURS THEIRS OUT` (keyed by section+address).

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
0x8033F5B8-0x80341200 (odemuexi2 DebuggerDriver: "Can't select EXI2 port!" string; no reference
source here; plus a 0x3B4-byte float function at 0x8033F5B8 with a 0x24 wrapper). The 8-byte
`return 0` stubs dtk named `AMC_IsStub` (0x802F6B9C) and `Hu_IsStub` (0x8033F990) don't match dolsdk2004's
amcstubs (which returns 1), so their real names/units are unconfirmed. objdiff still scores OSExec `Run` at 50% (asm; DOL is
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

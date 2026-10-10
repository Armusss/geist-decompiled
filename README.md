# Geist matching decompilation (GITE01)

A matching decompilation of **Geist** (GameCube, n-Space 2005, NTSC-U `GITE01` rev 0).
The goal is C/C++ source that recompiles, with the original Metrowerks CodeWarrior compiler,
into a `main.dol` that is **byte-identical** to the retail one. Once that's complete, the matched
source becomes the reference for the Rust rewrite in `../geist-rs`.

Built on [encounter/dtk-template](https://github.com/encounter/dtk-template) (git remote `template`).
Template documentation is in [`docs/`](docs/).

## Credits
- Dolphin SDK source in `src/dolphin/` and headers in `include/` come from
  [doldecomp/dolsdk2004](https://github.com/doldecomp/dolsdk2004) (commit 2328b416). That repository has
  **no licence**, so keep this project private.

## Never distribute game data
`orig/`, `build/` and all game binaries are gitignored. Never commit or upload the DOL, REL,
disc image, or anything extracted from them.

## Setup (already done on this machine)
- `orig/GITE01/sys/main.dol`: SHA-1 `6612f52ce6c50bb831ffddd5264b22973304f288`
- `orig/GITE01/files/loader.rel`: SHA-1 `20cc25827a68208dd48e0929e901ef6a83feda42` (not yet configured, see below)
- `ninja` is in `~/.local/bin` (add it to `PATH`). `configure.py` downloads the rest into `build/`:
  decomp-toolkit (dtk v1.8.4), objdiff-cli, wibo, the CodeWarrior compilers and gc-wii-binutils.

## Build
```sh
export PATH=$HOME/.local/bin:$PATH
python3 configure.py
ninja            # rebuilds main.dol and checks it against config/GITE01/build.sha1
```
The baseline (all code still as split assembly) builds and verifies: `build/GITE01/main.dol: OK`.

## State as of 2026-10-10
See **STATUS.md** for the detailed handoff (compiler findings, SDK splits, next steps).
- 75 Dolphin SDK translation units split; 53 of them now build from C and match.
- dtk analysis found 8,706 functions in main.dol and split it into 70 units, all still assembly (0% matched).
- `config/GITE01/symbols.txt` and `splits.txt` were auto-generated. There's no symbol map on the disc,
  so names are `fn_`/`lbl_` except for SDK/runtime functions dtk recognised by signature.

## Open questions / TODO
1. **Compiler version: mostly settled** (see STATUS.md). SDK = GC/1.2.5n (confirmed by matching).
   Game = GC/1.3.2-2.6 (2.7's linker rejects the runtime; 3.0 doesn't match); GC/2.6 is the working
   choice and the linker setting. `mw_comment_version: 10` is right for that range.
2. **Splits are auto-generated** and need refining into real translation units (SDK libraries,
   MusyX, MSL/Runtime, TRK, zlib 1.1.4, then game code). Ghidra pseudocode in `../geist-decomp` and
   the Ghidra projects in `~/.claude/ghidra-workspace/projects/` help find unit boundaries.
3. **loader.rel is left out.** Its imports from main.dol use ELF-style section indices (2, 7) with
   absolute addresses, so dtk v1.8.4 creates `lbl_800144A4` inside `extab` and fails. It's only
   10 functions; re-add it later by fixing/rewriting its relocations or patching dtk.
4. The objdiff GUI isn't installed (objdiff-cli is). Optional, from
   https://github.com/encounter/objdiff/releases, for visual diffing.

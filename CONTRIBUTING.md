# Contributing

Thanks for helping. This is a **matching** decompilation: a change is only useful if the rebuilt `main.dol`
stays byte-identical to the retail one. Everything below serves that goal. Using an AI agent? See
[docs/contributing-with-ai.md](docs/contributing-with-ai.md) for what to provide, how to set it up and prompts to
paste; the agent itself should read [AGENTS.md](AGENTS.md).

## Before you start
- Follow the setup in the [README](README.md) with your **own** copy of the game. Never commit or upload game
  data (DOL, REL, disc images, extracted files) or decompiler pseudocode (Ghidra/IDA output).
- Build once: `python3 configure.py && ninja` must end with `build/GITE01/main.dol: OK`.

## Picking work
[STATUS.md](STATUS.md) is the work queue. Its **Opus queue** section lists partly matched units, the
functions still left in each, their diff counts, and the source shapes already tried, so you don't repeat them.
Good first targets are functions with small diff counts in units that are nearly done (orange squares on the
progress map). Untouched units (grey squares) need drafting from scratch.
To avoid duplicate work, open an issue (or comment on an existing one) saying which unit you are taking.

## How a unit is organised
- `config/GITE01/splits.txt` assigns address ranges (.text, .rodata, .data, .bss, .sdata, .sdata2, ...) to each
  source file. Game files are provisional `u_<address>.cpp` units until their real boundary is known.
- `config/GITE01/symbols.txt` names every function and object. Placeholders are `fn_<addr>` / `lbl_<addr>`.
- `configure.py` lists every unit as `Matching` (linked from your compiled C) or `NonMatching` (linked from the
  original assembly; your C is still compiled, so you can diff it).
- Game code is C++ built with CodeWarrior **GC/1.3.2**; the SDK uses GC/1.2.5n and the runtime GC/2.6.

## The matching loop
```sh
# per-function diff against retail (`!!` marks differing instructions; ADDR/SIZE from symbols.txt)
python3 tools/matching/mdiff.py src/game/SpiritGun.cpp SpiritGunCreate 0x800F09D0 0x140
# every function of a unit (START/END from the unit's .text line in splits.txt)
python3 tools/matching/udiff.py src/game/u_800DE0C4.cpp 0x800DE0C4 0x800DED18
# brute-force ordinary source variants of one function (see the script's header)
python3 tools/matching/variants.py SRC FUNC ADDR SIZE my_variants.py
```
When **all** functions of a unit match, flip it to `Matching` in `configure.py`. If it then fails to link, the
unit probably needs its data in splits.txt (its string pool, type descriptor, floats in .sdata2), or other files
still refer to its functions by old placeholder names. Then run the audit:
```sh
sh tools/matching/audit.sh SpiritGun        # names of the units you flipped
```
It must print `AUDIT OK`. Regenerate the progress map with `ninja build/GITE01/report.json && python3
tools/progress_svg.py`.

[docs/matching-tricks.md](docs/matching-tricks.md) collects the CodeWarrior 1.3.2 behaviours that decide a
match: register ranking, when it copies a zero through a register, the lazy name-hash recipe, inline helpers,
constant-pool order and more. Read it before brute-forcing.

## Rules (a pull request that breaks one is not merged)
1. **The built main.dol must keep SHA-1 `6612f52ce6c50bb831ffddd5264b22973304f288`.** Every merge is audited:
   full rebuild, SHA-1 check, each claimed unit linked from its own source object.
2. **No fake matches.** No inline `asm` for ordinary code (real hardware ops like GQR setup are fine), no calling
   functions through `extern "C"` declarations of mangled names, no hand-built byte or string blobs standing in
   for code or data, no `lbl_X + offset` address tricks, no Ghidra artifacts (`FUN_`, `DAT_`, `undefined4`).
   `tools/matching/scan_fakes.sh` checks most of these.
3. **Never pick a compiler or flags just to make code match.** Game files are GC/1.3.2. A different version or
   flag for one file needs evidence that n-Space built it that way. The only per-file pragma accepted is a
   file-level `#pragma auto_inline off`, and only with evidence (retail calls a small same-file function that ours
   inlines); say what the evidence is in the pull request.
4. **Write real source.** Brute force is welcome, but every variant must be ordinary C/C++ the original
   programmer could have written.
5. **Unit boundaries and data splits** (splits.txt) need a reason: say which evidence (a duplicated constant, a
   padding word, a string pool, an inlining difference) shows the boundary.
6. **Credit borrowed code.** Code adapted from other decompilations or public libraries (ODE, zlib, MusyX, ...)
   needs a comment naming the source and its licence, and a line in the README credits.
7. Renamed symbols must be unique (`grep -c "^NAME" config/GITE01/symbols.txt` is 0 before you add it), and the
   mangled name in symbols.txt must match your declaration (`u32` is `unsigned long`, so it mangles as `Ul`).

## Pull requests
- One unit or one idiom per pull request when possible. Say which functions now match and paste the
  `udiff.py` summary and the `AUDIT OK` line.
- Partial progress is welcome: improving a NonMatching unit (fewer diffs, more functions at 0) is useful even if
  the unit isn't finished; say so in the description.
- If you found a new compiler behaviour, add it to docs/matching-tricks.md and note what failed to STATUS.md.
- Nothing is merged without the maintainer's audit. CI can't check matching here because it would need the
  game's DOL, so audits are run locally before merging.

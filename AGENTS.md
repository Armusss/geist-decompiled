# Instructions for AI agents

You are helping with a **byte-for-byte matching decompilation** of Geist (GameCube, NTSC-U GITE01). Your output
is judged by one thing: after your change, `python3 configure.py && ninja` must still end with
`build/GITE01/main.dol: OK`, and the functions you claim must compile to exactly the retail instructions.
Read [CONTRIBUTING.md](CONTRIBUTING.md) for setup and the full rules; this file is the short version.

## Ground truth
- `orig/GITE01/sys/main.dol` (the user's own copy, SHA-1 `6612f52c...`) is the source of truth. **Never modify
  it** or anything under `orig/`.
- Compare with `python3 tools/matching/mdiff.py SRC FUNC ADDR SIZE` (one function, `!!` marks diffs) and
  `tools/matching/udiff.py SRC START END` (a whole unit). If mdiff prints `!!BUILDFAIL`, your source does not
  compile: fix that before reading any diff.
- Decompiler pseudocode (Ghidra etc.) is a research aid only. Never paste it into the repository; write
  ordinary C/C++ from it, with no `FUN_`, `DAT_`, `undefinedN`, `param_N`, `local_N` leftovers.

## Hard rules
1. No fake matches: no inline asm for ordinary code, no calls through `extern "C"` declarations of mangled
   names, no byte/string blobs for code or data, no `lbl_X + offset` address tricks, no `#pragma` other than a
   file-level `#pragma auto_inline off` backed by evidence, no `__declspec(section)`. Run
   `sh tools/matching/scan_fakes.sh` before you finish.
2. Never change compilers, compiler flags or `mw_version` to make code match. Game code is CodeWarrior GC/1.3.2.
   If you find evidence that a file needs something else, report the evidence; don't act on it.
3. Don't change `config/GITE01/splits.txt` (unit boundaries) unless that is your task and you can name the
   evidence; otherwise report what the unit needs.
4. Only rename `fn_`/`lbl_` placeholders, with unique names and the correct C++ mangling; list your renames.
5. Don't fabricate code to satisfy a constant pool or padding (for example an invented unused function).
   Report it instead; STATUS.md records such cases.
6. Report honestly: give diff counts as measured, and say what you tried that failed.

## How to work efficiently
- Pick a target from STATUS.md's Opus queue (functions left per unit, with what was already tried).
- Draft every function of a unit first, then fix diffs. Write source early and edit it as you go; don't read
  large amounts of assembly before writing anything.
- Most remaining diffs are **register allocation**: every opcode matches but register numbers differ. Use the
  known CodeWarrior 1.3.2 idioms in [docs/matching-tricks.md](docs/matching-tricks.md) before brute force:
  - Callee-saved ranking: values produced inside inlines first, then locals in declaration order, then
    parameters last-first; a same-type copy of a parameter merges into it.
  - `li rA,0; mr rB,rA` (a zero copied through a register) appears only for zero-initialised locals of an
    **inline** helper. Move that block into a `static inline` function.
  - An inline's locals rank in reverse declaration order and get ascending stack slots.
  - The lazy type-name hash needs all three: `h *= 31; h += c;` as two statements, a local copy of the name,
    and a local pointer to the type in the caller.
  - Behaviour/state callbacks take `void*` parameters and cast them to typed locals.
- Brute force ordinary variants with `tools/matching/variants.py` (declaration orders, statement orders, inline
  helpers, types, loop forms). Variant file names must differ by more than letter case: the compiler runs under
  wibo, whose file names are case-insensitive.
- Time-box each function. If it doesn't move after a reasonable number of honest attempts, write down what you
  tried and move on: the notes are as valuable as a match.

## Finishing a unit
Flip it to `Matching` in configure.py, then run `sh tools/matching/audit.sh UNIT`. It must print `AUDIT OK`
(the full build matches and the unit is linked from its own source). Fix link errors caused by other files that
still use old placeholder names. Add anything new you learned to docs/matching-tricks.md.

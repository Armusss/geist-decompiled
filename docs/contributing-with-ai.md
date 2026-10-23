# Contributing with an AI model

This page is for people who want to point an AI coding agent (Claude Code, Codex, Cursor, Aider, Cline, a
local model with a coding harness, ...) at this repository to help decompile Geist. It covers what you must
provide yourself, how to set the agent up, and prompts you can paste into its chat.

Your agent's work is merged only after the maintainer audits it locally (full rebuild, the main.dol SHA-1 check,
each claimed unit linked from its own source, and a fake-match scan). The prompts below ask for exactly the kind
of pull request that passes that audit.

## 1. What you must provide yourself
- **Your own copy of the game.** This repository contains no game data and never will. You need Geist,
  NTSC-U, game ID `GITE01` (revision 0), dumped from a disc you own (for example with a Wii and CleanRip).
  Do not download it from the internet, and never upload game files anywhere.
- **The DOL**, put in place in one of two ways:
  - Place your disc image (ISO, GCM, RVZ, CISO, GCZ, ...) directly in `orig/GITE01/`; decomp-toolkit reads
    `sys/main.dol` from it on the first build.
  - Or extract it with [Dolphin](https://dolphin-emu.org/): right-click the game, Properties, Filesystem,
    right-click the disc, "Extract Entire Disc", and copy `sys/main.dol` to `orig/GITE01/sys/main.dol`.
  Check it: `sha1sum orig/GITE01/sys/main.dol` must print `6612f52ce6c50bb831ffddd5264b22973304f288`.
  A different SHA-1 means a different revision or a bad dump, and nothing will match.
- **Python 3 and ninja.** On Linux and macOS, `python3 configure.py` downloads everything else into `build/`
  (decomp-toolkit, objdiff-cli, the CodeWarrior compilers, binutils, and wibo, which runs the Windows
  compilers). On Windows the compilers run natively. See [getting_started.md](getting_started.md) and
  [dependencies.md](dependencies.md).
- **Optional: a decompiler.** [Ghidra](https://ghidra-sre.org/) with a GameCube loader, or another
  decompiler, gives pseudocode to start from. Generate it from your own copy. Pseudocode is a reading aid
  only: it must **never** be committed, and the agent must write ordinary C/C++ from it.

Before involving any agent, check the setup by hand:
```sh
python3 configure.py
ninja        # must end with: build/GITE01/main.dol: OK
```

## 2. Setting up the agent
- **Give it the repository as its working directory** (a fork of
  [Armusss/geist-decompiled](https://github.com/Armusss/geist-decompiled), on a branch).
- **Make sure it reads [AGENTS.md](../AGENTS.md).** Many harnesses load `AGENTS.md` automatically (Codex,
  Cursor, Aider with a config, and others). For Claude Code, the first prompt below tells it to read the file;
  you can also copy AGENTS.md's contents into a `CLAUDE.md` in your fork.
- **Permissions it needs:** reading and editing files under `src/`, `include/`, `config/GITE01/` and
  `configure.py`; running `python3`, `ninja` and the scripts in `tools/matching/`. It does **not** need network
  access after `configure.py` has downloaded the tools, and it must never touch `orig/`.
- **Pick a capable model.** Matching is precise, tedious work. Strong coding models do well on drafting and
  on the documented idioms. Weaker models tend to stall on register-allocation puzzles or "fix" them with
  forbidden tricks, so keep their tasks to drafting and small functions, and check their diffs.
- **Budget.** Give it one unit, or one idiom, per session. A time or turn limit helps: an agent that is stuck
  should report and stop, not loop.

## 3. Prompts to paste into the agent's chat
Replace the parts in `<angle brackets>`. Send the first prompt at the start of every new session.

### Start of a session (always first)
```text
You are working on a byte-for-byte matching decompilation of Geist (GameCube, GITE01) in this repository.
Read AGENTS.md and CONTRIBUTING.md fully before doing anything, then docs/matching-tricks.md. Confirm the
setup works by running `python3 configure.py && ninja`; it must end with `build/GITE01/main.dol: OK`.
Never modify anything under orig/. Follow AGENTS.md's hard rules exactly: no fake matches, no compiler or
flag changes, no pragmas except an evidence-backed file-level `#pragma auto_inline off`, no Ghidra artifacts
in source. Reply with a short summary of the rules and the result of the build, then wait for my next message.
```

### Pick a target for me
```text
Open STATUS.md and read the "Opus queue" section. Propose three targets that suit one session: a partly
matched unit with few functions left or small diff counts, and nothing someone already marked as failed many
times. For each, give the unit, the functions left, their diff counts from
`python3 tools/matching/udiff.py SRC START END` (START/END from the unit's .text line in
config/GITE01/splits.txt), and which idiom from docs/matching-tricks.md you would try first.
Don't edit anything yet.
```

### Finish a partly matched unit
```text
Work on <src/game/UNIT.cpp>. Measure it first with tools/matching/udiff.py. For each function that does
not match, look at `python3 tools/matching/mdiff.py SRC FUNC ADDR SIZE` and fix it with ordinary C/C++,
trying the idioms in docs/matching-tricks.md before brute force. Brute force is allowed through
tools/matching/variants.py, but every variant must be source the original programmer could have written.
Spend at most about 20 minutes per function; if one doesn't move, note what you tried and continue.
When every function matches, flip the unit to Matching in configure.py and run
`sh tools/matching/audit.sh <UNIT>`; it must print AUDIT OK. Commit after each improvement with a message
that says which functions now match. At the end, report per function: matched or not, the change that
fixed it, and what failed.
```

### Draft an untouched unit
```text
Draft <src/game/UNIT.cpp> from scratch (it has no source yet; its .text range is in
config/GITE01/splits.txt and its assembly is in build/GITE01/asm/game/<UNIT>.s). Work top to bottom: for each
function, read its assembly (and my decompiler output if I give you some), write ordinary C++ for it into the
file immediately, then go to the next function. Do not read more than two functions ahead without writing
source. Rename each fn_ placeholder you define in config/GITE01/symbols.txt to a descriptive unique name with
the correct C++ mangling (u32 is unsigned long, which mangles as Ul). Keep the file compiling. Leave the unit
NonMatching. When every function has a first version, run tools/matching/udiff.py and report the result.
Don't try to fix diffs in this session.
```

### Crack a recurring idiom
```text
Several functions in STATUS.md's Opus queue fail the same way: <describe it, e.g. "retail keeps X in r5,
ours in r29">. Find the ordinary source shape that makes CodeWarrior GC/1.3.2 produce retail's code. First
build small isolated probe files (outside src/) and compile shapes there; then apply what works to <list the
functions>. Report what you learned about the compiler's behaviour even if nothing matches, and add it to
docs/matching-tricks.md.
```

### Before opening the pull request
```text
Before I open a pull request: run `sh tools/matching/scan_fakes.sh` and `sh tools/matching/audit.sh <units you
flipped>`; both must pass. Check `git diff` for anything under orig/, any pasted decompiler pseudocode, any
changed compiler flags, and any splits.txt change without a stated reason. Then write a pull request
description listing the functions that now match, the udiff summary per unit, the AUDIT OK line, any symbol
renames, and anything new you learned about the compiler.
```

## 4. What gets a pull request rejected
- The built main.dol no longer has SHA-1 `6612f52c...`, or a claimed unit isn't linked from its own source.
- Fake matches: inline asm for ordinary code, calls through `extern "C"` declarations of mangled names, byte or
  string blobs standing in for code or data, `lbl_X + offset` tricks, extra pragmas, `__declspec(section)`.
- Changed compilers or compiler flags without evidence that n-Space built the file that way.
- Invented code that exists only to fix a constant pool or padding.
- Committed game data or decompiler pseudocode.
- Borrowed code without credit and licence in a comment and in the README.

#!/usr/bin/env python3
"""List game functions whose retail code shows a GC/2.0+-only compiler fingerprint.

Fingerprint: a load through a parameter register (r3-r10) scheduled *before* the `stwu r1` that sets up the
stack frame. GC/1.3.2 and older never schedule instructions above the stwu (0 of ~1100 functions in the
1.2.5n Dolphin SDK and 1.3.2 MusyX units do), while GC/2.0+ can (reproduced on u_8012071C's qsort
comparators). A game unit that contains such a function cannot be matched with GC/1.3.2, and is a candidate
for `mw_version="GC/2.0"` (see STATUS.md G4 for the rule).

Usage: python3 tools/compiler_fingerprint.py   (run from the repo root after `python3 configure.py && ninja`)"""
import glob
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LOAD = re.compile(r"(lfs|lfd|lwz|lhz|lha|lbz) \S+, -?0x[0-9a-f]+\(r([3-9]|10)\)")
PREFIX = re.compile(r"^/\* [0-9A-F]+ [0-9A-F]+  [0-9A-F ]+ \*/\t")


def scan(path):
    hits, cur, pre, done = [], None, [], True
    for line in open(path):
        m = re.match(r"\.fn (\S+),", line)
        if m:
            cur, pre, done = m[1], [], False
            continue
        if done or not cur:
            continue
        ins = PREFIX.sub("", line).strip()
        if not ins or ins.startswith((".", "#")):
            continue
        if ins.startswith("stwu r1"):
            if any(LOAD.match(p) for p in pre):
                hits.append(cur)
            done = True
            continue
        pre.append(ins)
        if len(pre) > 6:
            done = True
    return hits


def main():
    total = 0
    for path in sorted(glob.glob(os.path.join(ROOT, "build/GITE01/asm/game/*.s"))):
        hits = scan(path)
        if hits:
            total += len(hits)
            print(f"game/{os.path.basename(path)[:-2]}: {', '.join(hits)}")
    print(f"{total} function(s) with the GC/2.0+ fingerprint")


if __name__ == "__main__":
    main()

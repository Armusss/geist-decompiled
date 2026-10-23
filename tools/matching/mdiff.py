#!/usr/bin/env python3
"""mdiff.py SRC FUNC ADDR [SIZE]: build SRC's object with ninja (project flags), then print the compiled function
whose name contains FUNC next to the original main.dol at ADDR (hex). Differing instructions are marked `!!`;
relocated fields (call targets, addresses) are masked. ADDR and SIZE come from config/GITE01/symbols.txt.
Exits 2 with !!BUILDFAIL if the build fails (it never diffs a stale object).
Example: python3 tools/matching/mdiff.py src/game/SpiritGun.cpp SpiritGunCreate 0x800F09D0 0x140"""
import os, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import GM, dol_text, elf_funcs, disasm

src, fn, addr = sys.argv[1], sys.argv[2], int(sys.argv[3], 16)
os.chdir(GM)
obj = 'build/GITE01/' + os.path.splitext(src)[0] + '.o'
b = subprocess.run(['ninja', obj], capture_output=True, text=True)
if b.returncode != 0:
    sys.stdout.write(b.stdout[-3000:] + b.stderr[-1000:])
    print('!!BUILDFAIL: ninja failed for ' + obj + '; not diffing the old object')
    sys.exit(2)
name, code, mask = next(f for f in elf_funcs(obj) if fn in f[0])
size = int(sys.argv[4], 16) if len(sys.argv) > 4 else len(code)
tgt = next(t[addr - base:addr - base + size] for base, t in dol_text() if base <= addr < base + len(t))
mine, theirs = disasm(code, addr), disasm(tgt, addr)
for k in range(max(len(code), size) // 4):
    same = k < len(code) // 4 and k < size // 4 and all(
        (code[4 * k + j] & mask[4 * k + j]) == (tgt[4 * k + j] & mask[4 * k + j]) for j in range(4))
    print(f'{addr + 4 * k:08x} {"  " if same else "!!"} {mine[k] if k < len(mine) else "":38s} | '
          f'{theirs[k] if k < len(theirs) else ""}')

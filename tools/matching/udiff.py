#!/usr/bin/env python3
"""udiff.py SRC START END: build SRC's object and diff every symbols.txt function in [START, END) (hex) against
the original main.dol. Prints the count of differing instructions per non-matching function and "M/N match".
The unit's range is its .text line in config/GITE01/splits.txt.
Example: python3 tools/matching/udiff.py src/game/u_800DE0C4.cpp 0x800DE0C4 0x800DED18"""
import os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from common import GM

src, lo, hi = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
os.chdir(GM)
obj = 'build/GITE01/' + os.path.splitext(src)[0] + '.o'
b = subprocess.run(['ninja', obj], capture_output=True, text=True)
if b.returncode:
    print('BUILD FAIL\n' + b.stdout[-1500:]); sys.exit(1)
fs = []
for l in open('config/GITE01/symbols.txt'):
    m = re.match(r'(\S+) = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)', l)
    if m and lo <= int(m[2], 16) < hi:
        fs.append((m[1], m[2], m[3]))
bad = 0
for n, a, z in fs:
    r = subprocess.run([sys.executable, os.path.join(HERE, 'mdiff.py'), src, n, a, z], capture_output=True, text=True)
    c = r.stdout.count('!!')
    if c or r.returncode:
        bad += 1
        print(f'{n}: {c}' + (' ERR (name in source differs from symbols.txt?)' if r.returncode else ''))
print(f'{len(fs) - bad}/{len(fs)} match')

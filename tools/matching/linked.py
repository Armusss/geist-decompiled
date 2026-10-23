#!/usr/bin/env python3
"""linked.py UNIT...: exit 0 only if every UNIT (e.g. SpiritGun, u_800DE0C4) is linked into main.elf from its
compiled source object (build/GITE01/src/game/UNIT.o on the main.elf link line in build.ninja), not from asm."""
import os, sys
root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
lines = open(os.path.join(root, 'build.ninja')).read().split('\n')
i = next(k for k, l in enumerate(lines) if l.startswith('build build/GITE01/main.elf: link'))
buf = ''
while True:
    buf += ' ' + lines[i].rstrip('$')
    if not lines[i].endswith('$'):
        break
    i += 1
objs = set(buf.split('|')[0].split())
bad = [u for u in sys.argv[1:] if f'build/GITE01/src/game/{u}.o' not in objs]
for u in bad:
    print(f'NOT LINKED FROM SOURCE: {u}')
sys.exit(1 if bad else 0)

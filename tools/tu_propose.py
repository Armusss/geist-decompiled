#!/usr/bin/env python3
"""Combine tu_mono.json (allowed boundaries) and tu_pools.json (string-pool cores) into a first proposal of
game file ranges: build/tu_proposal.json = [[start, end, confidence], ...]. confidence: 'pinned' when the
gap between two pool cores has exactly one allowed boundary, 'guess' otherwise (first allowed boundary)."""
import json, os, re
GM = os.environ.get('GM', os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
mono = json.load(open(GM + '/build/tu_mono.json')); pools = json.load(open(GM + '/build/tu_pools.json'))
allowed = sorted(mono['ok'])
LO, HI = 0x80005800, 0x802E4858
cuts = {}
for last, first, cands in pools['gaps']:
    if len(cands) == 1: cuts[cands[0]] = 'pinned'
    elif cands: cuts.setdefault(cands[0], 'guess')
# known library units inside the game range (zlib) are fixed boundaries
for l in open(GM + '/config/GITE01/splits.txt'):
    pass
spl = open(GM + '/config/GITE01/splits.txt').read()
for name, s, e in re.findall(r'^(\S+):\n(?:\t[^\n]*\n)*?\t\.text\s+start:0x([0-9A-F]+) end:0x([0-9A-F]+)', spl, re.M):
    s, e = int(s, 16), int(e, 16)
    if LO <= s < HI: cuts[s] = 'known'; cuts[e] = 'known'
pts = sorted(set([LO] + list(cuts) + [HI]))
units = [[a, b, cuts.get(a, 'start')] for a, b in zip(pts, pts[1:]) if b > a]
json.dump(units, open(GM + '/build/tu_proposal.json', 'w'))
from collections import Counter
print('proposed units', len(units), dict(Counter(u[2] for u in units)))
sizes = sorted(b - a for a, b, _ in units)
print('size median', hex(sizes[len(sizes) // 2]), 'largest', hex(sizes[-1]))

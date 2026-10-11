#!/usr/bin/env python3
"""Combine tu_mono.json (allowed boundaries) and tu_pools.json (string-pool cores) into a first proposal of
game file ranges: build/tu_proposal.json = [[start, end, confidence], ...]. confidence: 'pinned' when the
gap between two pool cores has exactly one allowed boundary, 'guess' otherwise (first allowed boundary)."""
import json, os, re
GM = os.environ.get('GM', os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
mono = json.load(open(GM + '/build/tu_mono.json')); pools = json.load(open(GM + '/build/tu_pools.json'))
allowed = sorted(mono['ok'])
LO, HI = int(os.environ.get('LO', '0x80005800'), 16), int(os.environ.get('HI', '0x802E4858'), 16)
# call graph (bl targets) to place boundaries inside a gap: keep callers and callees on the same side
import struct as _st, bisect as _bi
_d = open(GM + '/orig/GITE01/sys/main.dol', 'rb').read()
_o = _st.unpack_from('>18I', _d, 0); _a = _st.unpack_from('>18I', _d, 0x48); _z = _st.unpack_from('>18I', _d, 0x90)
_f = []
for l in open(GM + '/config/GITE01/symbols.txt'):
    m = re.match(r'\S+ = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)', l)
    if m and LO <= int(m[1], 16) < HI: _f.append((int(m[1], 16), int(m[2], 16)))
_f.sort(); _fs = [x[0] for x in _f]
def _fi(a):
    i = _bi.bisect_right(_fs, a) - 1
    return i if i >= 0 and a < _f[i][0] + _f[i][1] else None
edges = [set() for _ in _f]
for i, (a, n) in enumerate(_f):
    for o, b, z in zip(_o, _a, _z):
        if z and b <= a < b + z: code = _d[o + a - b:o + a - b + n]; break
    for k in range(0, n - 3, 4):
        w = _st.unpack_from('>I', code, k)[0]
        if w >> 26 == 18 and w & 3 == 1:
            li = w & 0x03FFFFFC; li -= 0x4000000 if li & 0x2000000 else 0
            j = _fi(a + k + li)
            if j is not None and j != i: edges[i].add(j); edges[j].add(i)
def place(last, first, cands):
    # functions strictly between the two cores; score each candidate cut
    L, R = _fs.index(last), _fs.index(first)
    best = None
    for c in cands:
        ci = _fs.index(c)
        score = 0
        for f in range(L + 1, R):
            side_left = f < ci
            for g in edges[f]:
                if g <= L: score += 1 if side_left else -1
                elif g >= R: score += -1 if side_left else 1
        if best is None or score > best[0]: best = (score, c)
    return best[1]
cuts = {}
for last, first, cands in pools['gaps']:
    if len(cands) == 1: cuts[cands[0]] = 'pinned'
    elif cands: cuts.setdefault(place(last, first, cands), 'guess')
# known library units inside the game range (zlib) are fixed boundaries
for l in open(GM + '/config/GITE01/splits.txt'):
    pass
spl = open(GM + '/config/GITE01/splits.txt').read()
for name, s, e in re.findall(r'^(\S+):\n(?:\t[^\n]*\n)*?\t\.text\s+start:0x([0-9A-F]+) end:0x([0-9A-F]+)', spl, re.M):
    s, e = int(s, 16), int(e, 16)
    if LO <= s < HI and not os.environ.get('NOKNOWN'): cuts[s] = 'known'; cuts[e] = 'known'
# static initializers end their C++ file: the next function starts a new one
import struct
d = open(GM + '/orig/GITE01/sys/main.dol', 'rb').read()
offs = struct.unpack_from('>18I', d, 0); adr = struct.unpack_from('>18I', d, 0x48); sz = struct.unpack_from('>18I', d, 0x90)
def rd(a, n):
    for o, b, s_ in zip(offs, adr, sz):
        if s_ and b <= a < b + s_: return d[o + a - b:o + a - b + n]
fsize = {}
for l in open(GM + '/config/GITE01/symbols.txt'):
    m = re.match(r'(\S+) = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)', l)
    if m: fsize[int(m[2], 16)] = int(m[3], 16)
c = 0x80341204
while True:
    v = struct.unpack('>I', rd(c, 4))[0]
    if v == 0: break
    if LO <= v < HI and v in fsize: cuts[v + fsize[v]] = 'known'
    c += 4
pts = sorted(set([LO] + list(cuts) + [HI]))
units = [[a, b, cuts.get(a, 'start'), ''] for a, b in zip(pts, pts[1:]) if b > a]
# file names from assert strings: the string sits in some pool; that pool's users are in the named file
names = {}
for o, b, s_ in zip(offs, adr, sz):
    if not s_: continue
    for m in re.finditer(rb'(?<=\x00)[A-Za-z0-9_]{2,40}\.(cpp|c)\x00', d[o:o + s_]):
        names[b + m.start()] = m.group()[:-1].decode()
pool_list = sorted((int(k, 16), v) for k, v in pools['pools'].items())
for na, nm in names.items():
    best = None
    for pb, (lo, hi) in pool_list:
        if pb <= na < pb + 0x800: best = (pb, lo, hi)
    if best:
        for u in units:
            if u[0] <= best[1] < u[1]: u[3] = (u[3] + ',' if u[3] else '') + nm
units_named = sum(1 for u in units if u[3])
from collections import Counter
print('proposed units', len(units), dict(Counter(u[2] for u in units)), 'named', units_named)
print([ (hex(u[0]), u[3]) for u in units if u[3]])
sizes = sorted(u[1] - u[0] for u in units)
print('size median', hex(sizes[len(sizes) // 2]), 'largest', hex(sizes[-1]))
json.dump(units, open(GM + '/build/tu_proposal.json', 'w'))

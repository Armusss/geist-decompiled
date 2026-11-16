#!/usr/bin/env python3
# Usage: python3 tools/boundary_scan.py  (after ninja; reads build/GITE01/asm and report.json)
"""From duplicate .sdata2 constants, derive required .text cut intervals per unit, and per-piece match counts."""
import re, struct, glob, os, json
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
d = open(f'{ROOT}/orig/GITE01/sys/main.dol', 'rb').read()
offs = struct.unpack('>18I', d[0:72]); addr = struct.unpack('>18I', d[72:144]); sz = struct.unpack('>18I', d[144:216])
def rd(a, n):
    for o, s, l in zip(offs, addr, sz):
        if l and s <= a < s + l: return d[o + a - s:o + a - s + n]
sd2 = {}
for line in open(f'{ROOT}/config/GITE01/symbols.txt'):
    m = re.match(r'(\S+) = \.sdata2:0x([0-9A-F]+); // type:\w+(?: size:0x([0-9A-F]+))?', line)
    if m: sd2[m[1]] = (int(m[2], 16), int(m[3] or '4', 16))
cfg = open(f'{ROOT}/configure.py').read()
status = dict((p, k) for k, p in re.findall(r'Object\((Matching|NonMatching|Equivalent), "game/([^"]+)\.c(?:pp)?"', cfg))
rep = {u['name'].split('/', 1)[1]: {f['name']: f.get('fuzzy_match_percent', 0) for f in u.get('functions', [])}
       for u in json.load(open(f'{ROOT}/build/GITE01/report.json'))['units']}
def val(a, size):
    n = 8 if size >= 8 and a % 8 == 0 else 4
    return rd(a, n)
res = []
for unit, st in status.items():
    if st != 'NonMatching': continue
    f = f'{ROOT}/build/GITE01/asm/game/{unit}.s'
    if not os.path.exists(f): continue
    fns = []; cur = None
    for line in open(f):
        m = re.match(r'\.fn (\S+),', line)
        if m:
            cur = [m[1], set()]; fns.append(cur); continue
        if cur:
            for lab in re.findall(r'(lbl_[0-9A-F]{8})@sda21', line):
                if lab in sd2: cur[1].add(sd2[lab])
    byv = {}
    for i, (n, refs) in enumerate(fns):
        for a, s in refs:
            byv.setdefault(val(a, s), {}).setdefault(a, []).append(i)
    cons = []; bad = []
    for v, m in byv.items():
        if len(m) < 2: continue
        al = sorted(m)
        for a1, a2 in zip(al, al[1:]):
            lo, hi = max(m[a1]), min(m[a2])
            if lo < hi: cons.append((lo, hi))
            else: bad.append((hex(a1), hex(a2)))
    if not cons and not bad: continue
    cons.sort(key=lambda c: c[1]); cuts = []
    for lo, hi in cons:
        if not any(lo < c <= hi for c in cuts): cuts.append(hi)
    cuts.sort()
    # report intervals for each chosen cut: tightest lo among constraints it satisfies
    pieces = []; prev = 0
    for c in cuts + [len(fns)]:
        names = [fns[i][0] for i in range(prev, c)]
        mt = sum(1 for n in names if rep.get(f'game/{unit}', {}).get(n, 0) >= 100)
        pieces.append((names[0] if names else '-', len(names), mt)); prev = c
    ivs = []
    for c in cuts:
        lo = max(l for l, h in cons if l < c <= h)
        ivs.append((fns[lo + 1][0], fns[c][0]))
    res.append((unit, len(fns), ivs, pieces, bad))
for unit, n, ivs, pieces, bad in sorted(res, key=lambda r: -max(p[2] / p[1] if p[1] else 0 for p in r[3])):
    full = [p for p in pieces if p[1] and p[1] == p[2]]
    print(f'== {unit}: {n} fns, {len(ivs)} cut(s); pieces (first fn, fns, matched): {pieces}' + ('  <-- FULLY MATCHED PIECE' if full else ''))
    for a, b in ivs: print(f'     cut somewhere in [{a} .. {b}]')
    if bad: print(f'     interleaved dup pairs (no clean cut): {bad}')

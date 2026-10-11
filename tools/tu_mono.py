# Game TU boundaries from ordering constraints: per-TU private data (sdata2 constants, rodata pools,
# jump tables) is laid out in the same order as the code. A boundary between functions i and i+1 is only
# possible if every private address used by functions <= i lies below every one used by functions > i.
import struct, re, collections, bisect, json, os, sys
GM = os.environ.get('GM', os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
d = open(GM + '/orig/GITE01/sys/main.dol', 'rb').read()
offs = struct.unpack_from('>18I', d, 0); adr = struct.unpack_from('>18I', d, 0x48); sz = struct.unpack_from('>18I', d, 0x90)
secs = [(o, b, s) for o, b, s in zip(offs, adr, sz) if s]
R13, R2 = 0x804DA520, 0x804DDE80
LO, HI = 0x80005800, 0x802E4858
funcs = []; objs = {}
for l in open(GM + '/config/GITE01/symbols.txt'):
    m = re.match(r'(\S+) = (\S+):0x([0-9A-F]+); // type:(\w+)(?: size:0x([0-9A-F]+))?(.*)', l)
    if not m: continue
    a = int(m[3], 16); size = int(m[5] or '0', 16)
    if m[4] == 'function' and LO <= a < HI: funcs.append((a, size, m[1]))
    elif m[4] == 'object': objs[a] = (m[2], m[1], size, m[6])
funcs.sort()
SEC = {'.sdata2': (0x804D5E80, 0x804DE000), '.rodata': (0x80341280, 0x8034EE00), '.data': (0x8034EE00, 0x803D0000)}
def text(a, n):
    for o, b, s in secs:
        if b <= a < b + s: return d[o + a - b:o + a - b + n]
# vtable-like tables: runs of >= 3 pointers to game functions inside .data (referenced by constructors)
import bisect as _b
_fs = sorted(f[0] for f in funcs); _fset = set(_fs)
_do, _db, _ds = next((o, b, s) for o, b, s in secs if b <= 0x8034EE00 < b + s or b == 0x8034EE00)
VT = {}                        # table start -> list of target functions
_cur = []
for _i in range(0, _ds, 4):
    _w = struct.unpack_from('>I', d, _do + _i)[0]
    if _w in _fset: _cur.append((_db + _i, _w)); continue
    if len(_cur) >= 3: VT[_cur[0][0]] = [w for _, w in _cur]
    _cur = []
_vt_starts = sorted(VT)
def in_vtable(t):
    i = _b.bisect_right(_vt_starts, t) - 1
    return i >= 0 and t < _vt_starts[i] + 4 * len(VT[_vt_starts[i]])
def private(t):
    # sdata2 / rodata anything; .data only jump tables (per function, compiler generated)
    for name, (lo, hi) in SEC.items():
        if lo <= t < hi:
            if name == '.rodata':
                b = text(t, 3) or b''
                return name if len(b) == 3 and all(32 <= c < 127 or c in (9, 10) for c in b) else None
            if name == '.data':
                o = objs.get(t)
                return name if (o and o[1].startswith('jumptable_')) else None  # vtables: weak, shared between files
            return name
ostarts = sorted(objs)
def obj_key(t):
    i = bisect.bisect_right(ostarts, t) - 1
    return ostarts[i] if i >= 0 else t
refs = []
for a, size, n in funcs:
    code = text(a, size) or b''; hi = {}; r = collections.defaultdict(list)
    for i in range(0, len(code) - 3, 4):
        w = struct.unpack_from('>I', code, i)[0]; op = w >> 26; rA = (w >> 16) & 31
        imm = w & 0xffff; simm = imm - 0x10000 if imm & 0x8000 else imm; t = None
        if op == 15 and rA == 0: hi[(w >> 21) & 31] = imm << 16; continue
        if op in (14, 32, 34, 36, 40, 48, 50, 52, 54) and rA in hi: t = hi[rA] + simm
        elif op in (14, 32, 34, 40, 48, 50) and rA == 2: t = R2 + simm
        if t is not None:
            s = private(t)
            if s: r[s].append(t)
    refs.append(r)
N = len(funcs)
WIN = int(os.environ.get('WIN', '0x10000'), 16)
users = collections.defaultdict(list)
for i, r in enumerate(refs):
    for sname, ts in r.items():
        for t in ts: users[(sname, t if sname == '.sdata2' else obj_key(t))].append(i)
shared = {k for k, us in users.items() if funcs[max(us)][0] - funcs[min(us)][0] > WIN}
for i, r in enumerate(refs):
    for sname in list(r):
        r[sname] = [t for t in r[sname] if (sname, t if sname == ".sdata2" else obj_key(t)) not in shared and not (sname == ".sdata2" and t >= 0x804DB338)]
print('shared objects dropped', len(shared))
# outlier pass: drop references far from the local trend (neighbouring functions' median address)
import statistics
THR = {'.sdata2': int(os.environ.get('THR2', '0x400'), 16), '.rodata': int(os.environ.get('THRR', '0x2000'), 16), '.data': 0x4000}
K = int(os.environ.get('K', '40'))
dropped = 0
for sname in SEC:
    k = K if sname != '.data' else 4 * K
    meds = []
    for i in range(N):
        pool = [t for j in range(max(0, i - k), min(N, i + k + 1)) for t in refs[j][sname]]
        meds.append(statistics.median(pool) if len(pool) >= 5 else None)
    for i in range(N):
        if meds[i] is None:
            keep = []                     # too little context to trust these references
        else:
            keep = [t for t in refs[i][sname] if abs(t - meds[i]) <= THR[sname]]
        dropped += len(refs[i][sname]) - len(keep); refs[i][sname] = keep
print('outlier references dropped', dropped)
ok = [True] * N            # ok[i]: boundary allowed before function i
for s in SEC:
    pre = [0] * N; m = 0
    for i in range(N):
        if refs[i][s]: m = max(m, max(refs[i][s]))
        pre[i] = m
    suf = [1 << 40] * (N + 1); m = 1 << 40
    for i in range(N - 1, -1, -1):
        if refs[i][s]: m = min(m, min(refs[i][s]))
        suf[i] = m
    for i in range(1, N):
        if pre[i - 1] >= suf[i]: ok[i] = False
bounds = [i for i in range(1, N) if ok[i]]
gaps = sum(1 for i in range(1, N) if funcs[i][0] != funcs[i - 1][0] + funcs[i - 1][1])
gap_bounds = [i for i in range(1, N) if funcs[i][0] != funcs[i - 1][0] + funcs[i - 1][1]]
print('functions', N, 'allowed boundaries', len(bounds), 'alignment gaps', gaps,
      'gaps that are allowed boundaries', sum(ok[i] for i in gap_bounds))
# definite segments = runs between forced merges: a "forced" region is where ok is False
segs = []; start = 0
for i in range(1, N):
    if ok[i]: segs.append((start, i)); start = i
segs.append((start, N))
big = sorted(((funcs[e - 1][0] + funcs[e - 1][1] - funcs[b][0], b, e) for b, e in segs), reverse=True)[:5]
print('largest forced-together spans (bytes, functions):', [(hex(x), e - b) for x, b, e in big])
json.dump({'ok': [funcs[i][0] for i in bounds], 'gaps': [funcs[i][0] for i in gap_bounds]},
          open(GM + '/build/tu_mono.json', 'w'))

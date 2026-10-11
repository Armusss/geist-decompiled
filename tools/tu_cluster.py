# Estimate game translation-unit boundaries: functions that reference the same private constant
# (.sdata2 / .rodata / .data object that is only used within a narrow address window) belong to one TU.
import struct, re, collections, bisect, json, os
GM = os.environ.get('GM', os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
d = open(GM + '/orig/GITE01/sys/main.dol', 'rb').read()
offs = struct.unpack_from('>18I', d, 0); adr = struct.unpack_from('>18I', d, 0x48); sz = struct.unpack_from('>18I', d, 0x90)
secs = [(o, b, s) for o, b, s in zip(offs, adr, sz) if s]
R13, R2 = 0x804DA520, 0x804DDE80
LO, HI = 0x80005800, 0x802E4858
funcs = []; objs = []
for l in open(GM + '/config/GITE01/symbols.txt'):
    m = re.match(r'(\S+) = (\S+):0x([0-9A-F]+); // type:(\w+)(?: size:0x([0-9A-F]+))?', l)
    if not m: continue
    a = int(m[3], 16); size = int(m[5] or '0', 16)
    if m[4] == 'function' and LO <= a < HI: funcs.append((a, size, m[1]))
    elif m[4] == 'object' and m[2] in ('.sdata2', '.rodata', '.data', '.sdata'): objs.append((a, size, m[2]))
funcs.sort(); objs.sort(); ostart = [o[0] for o in objs]
def obj_of(t):
    i = bisect.bisect_right(ostart, t) - 1
    if i >= 0 and objs[i][0] <= t < objs[i][0] + max(objs[i][1], 1): return objs[i][0]
def text(a, n):
    for o, b, s in secs:
        if b <= a < b + s: return d[o + a - b:o + a - b + n]
users = collections.defaultdict(set)
for fi, (a, size, n) in enumerate(funcs):
    code = text(a, size) or b''; hi = {}
    for i in range(0, len(code) - 3, 4):
        w = struct.unpack_from('>I', code, i)[0]; op = w >> 26; rA = (w >> 16) & 31
        imm = w & 0xffff; simm = imm - 0x10000 if imm & 0x8000 else imm
        t = None
        if op == 15 and rA == 0: hi[(w >> 21) & 31] = imm << 16
        elif op in (14, 32, 34, 36, 40, 48, 50, 52, 54) and rA in hi: t = hi[rA] + simm
        elif op in (14, 32, 34, 40, 48, 50) and rA == 2: t = R2 + simm
        elif op in (14, 32, 34, 40, 48, 50) and rA == 13: t = R13 + simm
        if t:
            o = obj_of(t)
            if o: users[o].add(fi)
# private objects: used by functions spanning < 64 KB
links = []
for o, fs in users.items():
    if len(fs) < 2: continue
    lo, hi_ = min(fs), max(fs)
    if funcs[hi_][0] - funcs[lo][0] < 0x10000: links.append((lo, hi_))
# union contiguous ranges: TU spans
cover = [0] * (len(funcs) + 1)
for lo, hi_ in links: cover[lo] += 1; cover[hi_] -= 1
run = 0; bounds = [0]
for i in range(len(funcs) - 1):
    run += cover[i]
    if run == 0: bounds.append(i + 1)   # no private object spans i..i+1: candidate boundary
print('functions', len(funcs), 'candidate TU boundaries', len(bounds))
json.dump([funcs[i][0] for i in bounds], open(os.path.join(GM, 'build/tu_bounds.json'), 'w'))
spans = [funcs[bounds[k + 1] - 1][0] + funcs[bounds[k + 1] - 1][1] - funcs[bounds[k]][0] for k in range(len(bounds) - 1)]
print('span sizes: median', sorted(spans)[len(spans) // 2], 'max', max(spans), '>=16KB', sum(s >= 0x4000 for s in spans))

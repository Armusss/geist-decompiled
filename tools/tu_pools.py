# Positive TU evidence from string pools: functions that address the same pooled-string base (lis/addi
# target used as a base for further offsets) are in one TU; different pools mark a boundary between them.
import os, re, struct, collections, json, sys
src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'tu_mono.py')).read()
exec(src.split("refs = []\n")[0])          # loads funcs, objs, text(), SEC
GM = os.environ.get('GM', os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
def is_stringy(t):
    b = text(t, 4) or b''
    return all(32 <= c < 127 or c in (9, 10, 13) for c in b[:3]) and b[:1] != b'\0'
pool_of = []      # per function: set of pool bases
for a, size, n in funcs:
    code = text(a, size) or b''; hi = {}; bases = set()
    for i in range(0, len(code) - 3, 4):
        w = struct.unpack_from('>I', code, i)[0]; op = w >> 26; rD = (w >> 21) & 31; rA = (w >> 16) & 31
        imm = w & 0xffff; simm = imm - 0x10000 if imm & 0x8000 else imm
        if op == 15 and rA == 0: hi[rD] = imm << 16; continue
        if op == 14 and rA in hi:
            t = hi[rA] + simm
            if (0x80341280 <= t < 0x8034EE00 or 0x8034EE00 <= t < 0x803D0000) and is_stringy(t): bases.add(t)
    pool_of.append(bases)
users = collections.defaultdict(list)
for i, b in enumerate(pool_of):
    for t in b: users[t].append(i)
N = len(funcs)
# a pool shared by functions far apart is a global table, not a TU pool
pools = {t: us for t, us in users.items() if funcs[max(us)][0] - funcs[min(us)][0] < 0x40000}
# merges: all functions between first and last user of a pool are in one TU
merge = [False] * N            # merge[i]: i-1 and i in same TU
for t, us in pools.items():
    for i in range(min(us) + 1, max(us) + 1): merge[i] = True
# forced boundaries: consecutive pool-user functions with different pools and no merge between
mono = json.load(open(GM + '/build/tu_mono.json'))
allowed = set(mono['ok'])
forced = []
pairs = []
for sec_lo, sec_hi in ((0x80341280, 0x8034EE00), (0x8034EE00, 0x803D0000)):   # .rodata pools, .data pools
    first = sorted((min(us), t) for t, us in pools.items() if sec_lo <= t < sec_hi)
    # consecutive pools in address order are consecutive files (same section, same order as code)
    first = sorted(first, key=lambda x: x[1])
    pairs += list(zip(first, first[1:]))
for (i1, t1), (i2, t2) in pairs:
    if i2 <= i1:
        continue                      # pool order disagrees with code order: not a clean pair
    last1 = max(pools[t1])
    if last1 < min(pools[t2]):
        i2 = min(pools[t2])
        cands = [k for k in range(last1 + 1, i2 + 1) if funcs[k][0] in allowed and not merge[k]]
        forced.append((last1, i2, cands))
print('string pools', len(pools), 'pool-separated gaps', len(forced),
      'with exactly one allowed boundary', sum(len(c) == 1 for _, _, c in forced),
      'with none', sum(len(c) == 0 for _, _, c in forced))
json.dump({'pools': {hex(t): [funcs[min(u)][0], funcs[max(u)][0]] for t, u in pools.items()},
           'gaps': [[funcs[a][0], funcs[b][0], [funcs[k][0] for k in c]] for a, b, c in forced]},
          open(GM + '/build/tu_pools.json', 'w'))

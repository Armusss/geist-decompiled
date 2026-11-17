# ps_rsqrte / ps_res sites in retail game code (asm-normalize investigation)

opus-asm, 2026-11-17. Investigation only: nothing in src/ was changed. Scratch files, the site catalogue and
the scripts are in ~/geist-work/ds/opus-asm-tools/ (catalog.py, classify.py, sites.tsv, vecnorm.h, b3.cpp, v6.bak).

## 1. Catalogue

The count was taken from a disassembly of the retail main.dol .text, not from build/GITE01/asm. It finds
**336** `ps_rsqrte` and 336 `ps_res`, in **163 functions** and **69 units**. A `grep -c` over asm/game gives 342 lines;
the "375 / 75" figure in the brief does not reproduce. Every site falls into one of two shapes, plus one outlier:

| shape | sites | functions | units | what it is |
|---|---|---|---|---|
| NORM3 | 243 | 117 | 57 | inline 3-vector normalize (zero test, then an asm block holding the whole computation) |
| FASTSQRT | 92 | 59 | 32 | the already-approved `x <= 0 ? 0 : asm{lfs; ps_rsqrte; ps_res; stfs}` |
| SQRT-reg | 1 | 1 | 1 | fn_800A109C (u_8009DE94): `ps_rsqrte t,t ... ps_res t,t` on a register value (1-x), with no stack traffic (acos-style polynomial) |

How each NORM3 site looks depends on what feeds it (sub-shapes are listed in sites.tsv):
- **asm-lfs (126):** the source is an unknown pointer. The asm reloads `lfs x,0(rS)` after the C-side fabs loads (DbRoom, fn_8009F7A8 first call).
- **frsp-fwd (113):** the source is a local just written by C. CW forwards the asm `lfs` from the `stfs` and emits `frsp x,x` (ExtraTrailApply, SwarmUpdate 0x8022F3A8, Actor62UpdatePath 0x802A91C4, BuildModel 0x8020B524).
- **regs (4):** the inputs are forwarded from registers, and the scaled outputs go to new registers (fn_800D72CC x2, fn_80176044, ShotFire).
- **fn_8003247C (ModInfo):** one site is fully spilled (`stfs/frsp` around every op). The same asm block is presumably used, but the register variables did not get registers there. This is unverified.

FASTSQRT sub-forms: 91 use the if/else form (`bne; b; ps_rsqrte; ps_res; frsp; stfs`). One (AimPacketBuild, u_802DDEDC) uses the
if-only form (`beq skip`, no frsp, result dead). The variants "rsqrte reads another reg" and "ps_res writes a new reg" come from
CW splitting the live range of the `register float` temp. They are not different source.

### The NORM3 instruction window (common to all 243)
```
  lfs/fabs x3, fcmpo x3 vs FLT_EPSILON through mfcr/rlwinm, add, add.   ; (|x|>e)+(|y|>e)+(|z|>e) == 0
  bne L ; lfs 0.0 ; stfs dst.x/y/z ; b end
L: [lfs x,0(rS); lfs y,4(rS); lfs z,8(rS)]  or  [frsp x,x; frsp y,y; frsp z,z]
  fmul sq,x,x ; fmul t,y,y ; fmul w,z,z ; fadd sq,sq,t ; fadd sq,sq,w    <- DOUBLE fmul/fadd on floats
  ps_rsqrte sq,sq ; fmul x,x,sq ; fmul y,y,sq ; fmul z,z,sq ; ps_res t,sq
  stfs x,dst.x ; stfs y,dst.y ; stfs z,dst.z ; stfs t,len(r1)            <- no frsp before the stfs
```
Before the block, retail stores `src` and `dst` (pointer values, often `&local`) to two low stack slots, usually
8(r1)/12(r1), and keeps `len` in the next slot. Those slots are never read back.

**Why the arithmetic must be asm:** `fmul`/`fadd` (double) on float operands, followed by `stfs` with no `frsp`, cannot
come from C. Float C gives `fmuls`/`fadds`. Double C needs an `frsp` (or a double store) before a float store. The whole
computation was written inside the asm block in the original source, not just the two paired-single instructions.

## 2. Original source: one shared inline (VecNormalize)

All 243 NORM3 sites fit **one** inline, shown below. Each piece of its structure was tested against retail:

```cpp
inline float VecNormalize(Vec3* dst, const Vec3* src) {
    float len;
    float az = __fabsf(src->z);
    float ay = __fabsf(src->y);
    float ax = __fabsf(src->x);
    register float x, y, z, sq, t, w;
    register const Vec3* s;
    register Vec3* d;
    if ((ax > 1.1920929e-7f) + (ay > 1.1920929e-7f) + (az > 1.1920929e-7f) == 0) {
        dst->x = 0.0f;
        dst->y = 0.0f;
        dst->z = 0.0f;
    } else {
        asm {
            lwz s, src
            lwz d, dst
            lfs x, 0(s)
            lfs y, 4(s)
            lfs z, 8(s)
            fmul sq, x, x
            fmul t, y, y
            fmul w, z, z
            fadd sq, sq, t
            fadd sq, sq, w
            ps_rsqrte sq, sq
            fmul x, x, sq
            fmul y, y, sq
            fmul z, z, sq
            ps_res t, sq
            stfs x, 0(d)
            stfs y, 4(d)
            stfs z, 8(d)
            stfs t, len
        }
    }
    return len;
}
```

What each detail produces (all confirmed by scratch compiles with GC/1.3.2 and the game cflags):
- **Pointer stores at 8/12(r1): yes, this is the "inline taking Vec3* args that CW spills" idea, and it is confirmed.** It comes from
  `lwz s, src` / `lwz d, dst`. An asm operand naming a non-register variable forces it into a stack home, so CW
  stores the pointer there. It then forwards the load, so `lfs x,0(s)` turns into `lfs x,0(r4)` or into `frsp` when src is `&local`.
  With `register` params, the stores disappear. Naming the param directly as `0(src)` is a compile error ("src was not
  assigned to a register").
- **Parameter order `(dst, src)`:** with `(src, dst)` the two stack slots come out swapped compared with retail.
- **The asm stores `stfs ... 0(d)` and `stfs t, len`:** these give the dst stores and the dead `len` slot at the 3rd low slot.
- **`w` is a register variable, not a hard-coded f0.** CW happens to give it f0. Naming f0 reserves f0, which moves eps/0.0 off f0 (+16 diffs).
- **The zero test is three `__fabsf` locals declared z, y, x, with the sum `== 0`.** This gives each fabs its own register,
  issued right after its load, in retail's f6/f7/f8 order. A single expression shares one fabs register (+30 diffs). `||`/`&&` forms are
  different code.
- **Callers declare `const Vec3*` for an untouched source param** (e.g. `fn_8009F7A8(Mtx34*, const Vec3*)`). CW can then
  substitute the inline's `src` with the caller's parameter, which gives retail's slot layout. A `Vec3*` caller param adds a copy.
- In-place calls (`VecNormalize(&v, &v)`) explain the sites where both slots hold the same address (ExtraTrailApply,
  Actor62UpdatePath, AimPacketBuild, fn_8009F7A8 second call).

FASTSQRT stays the 4-line block already approved (u_800E0CB8, u_8016D170 and u_80253614 match with it). It needs no new helper.
For the SQRT-reg site (fn_800A109C, an acos polynomial), the source is `register float t = 1.0f - x; asm { ps_rsqrte t, t; ps_res t, t }`.
Scratch q2.cpp gives the same 35 instructions with the same opcode sequence, so the structure is reproduced. 14 FPR numbers in the polynomial are
still different (the order of its constant loads), and this is not tested further.

## 3. Proofs (scratch, ~/geist-work/ds/opus-asm-tools/)

| function | result | remaining |
|---|---|---|
| fn_80184A70 (u_8017D794), v6.bak | **all 49 instructions of the inlined helper (0x80184A70-0x80184B34) byte-identical**, including stack frame, the pointer stores, frsp-forwarding, register numbers, the len slot | 10 diffs, all FPR numbers in the caller's tail (dot/reflect code after the helper). Brute force of all declaration orders of the caller's locals (about 5k variants) does not go below 10 |
| fn_8009F7A8 (u_8009DE94), b3.cpp, 2 calls (asm-lfs + in-place frsp-fwd) | same instruction count (133/133), every opcode, stack slot and pointer store identical | 62 diffs, all FPR numbers: retail colors sq,t -> f2,f1 and x,y,z -> f5,f4,f3, while ours gives y,z -> f2,f1 and sq,t -> f4,f3 |
| ExtraTrailApply (u_80230504), Actor62UpdatePath (u_802A815C) | the helper window is opcode-identical, with registers shifted by one | the existing drafts differ elsewhere (frame size, control flow), so they are not a test of the helper |

None of the 720 orders of the register declarations, the scope placements of the register vars, the asm instruction orders, or the forms of the zero test
change fn_8009F7A8's coloring. GC/2.0 gives the same result (checked only for information; not proposed). So
**no real function has reached 0 diffs yet.** The helper is proven down to the instruction and stack-layout
level. Register coloring around it depends on the surrounding caller source, which is ordinary per-function brute
force (the same situation as any other register swap).

## 4. Recommendation (what the user would approve)

1. **Approve one more asm form: the `VecNormalize` inline above**, as a single shared header helper (e.g.
   include/game/vecnormalize.h). It holds a 19-instruction asm block that includes ordinary `fmul`/`fadd` and `lwz`/`lfs`/`stfs`.
   This breaks hard rule 2 ("never fmul/fadd in asm"), so it needs the user's explicit approval. The case for it:
   (a) retail's double fmul/fadd followed by stfs with no frsp is impossible from C; (b) one helper explains all 243 sites
   in 57 units, including the stack-pointer stores, which no C form produces; (c) the helper part of fn_80184A70
   matches byte for byte. Suggested condition, the same as for FastSqrt: allowed only verbatim, from the shared header, and only when the unit then fully
   matches with AUDIT OK.
2. FastSqrt: no change. 92 sites / 32 units are already covered by the approved 4-line block (also put it in the shared header).
3. SQRT-reg (fn_800A109C): defer. It is a 2-instruction register-form asm; ask when that unit is otherwise done.
4. Not reproduced: fn_8003247C's spilled variant (1 site, untested), and the 0-diff caller coloring in fn_8009F7A8.

Units unblocked by (1) (all NonMatching; counts are NORM3 sites): ModInfo 19, u_8006C604 21, u_800B5028 12,
u_80015B64 11, u_80068004 11, u_8009DE94 11, u_800D32E8 11, u_80176044 9, u_80112308 8, u_801CC6D0 8, u_800886C8 7,
u_80226DE8 7, u_8025ECF8 6, and 44 more units with 1-5 sites each. These include the four the lead named: u_80230504
(ExtraTrailApply), u_8022ED88 (SwarmUpdate x2), u_802A815C (Actor62UpdatePath), u_8020A650 (BuildModel x2). In every case the drafts
currently written as `1.0f / sqrtf(...)` or `__frsqrte` must be replaced by `VecNormalize(&out, &in)`, and a function taking an
untouched source pointer should declare it `const Vec3*`.

## Appendix A: per-unit counts

| unit | status | NORM3 | FASTSQRT | other | functions |
|---|---|---|---|---|---|
| Corona.cpp | NonMatching | 1 | 0 | 0 | 1 |
| Cube.cpp | NonMatching | 2 | 0 | 0 | 1 |
| CubeTev.cpp | NonMatching | 0 | 1 | 0 | 1 |
| DbRoom.c | NonMatching | 3 | 0 | 0 | 1 |
| FearActor.c | NonMatching | 3 | 3 | 0 | 5 |
| ModInfo.cpp | NonMatching | 19 | 31 | 0 | 18 |
| Msx.cpp | NonMatching | 1 | 0 | 0 | 1 |
| u_80015B64.cpp | NonMatching | 11 | 0 | 0 | 3 |
| u_80022B10.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_8003CC90.cpp | NonMatching | 5 | 0 | 0 | 4 |
| u_80068004.cpp | NonMatching | 11 | 1 | 0 | 3 |
| u_8006C604.cpp | NonMatching | 21 | 2 | 0 | 6 |
| u_800886C8.cpp | NonMatching | 7 | 0 | 0 | 5 |
| u_8008DCDC.cpp | NonMatching | 0 | 4 | 0 | 1 |
| u_8009DE94.cpp | NonMatching | 11 | 5 | 1 | 5 |
| u_800A6F18.cpp | NonMatching | 5 | 5 | 0 | 9 |
| u_800B1834.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_800B5028.cpp | NonMatching | 12 | 2 | 0 | 5 |
| u_800BAA14.cpp | NonMatching | 4 | 2 | 0 | 2 |
| u_800C5028.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_800D32E8.cpp | NonMatching | 11 | 1 | 0 | 4 |
| u_800E0CB8.cpp | Matching | 0 | 5 | 0 | 5 |
| u_800EB1B8.cpp | NonMatching | 2 | 3 | 0 | 4 |
| u_80112308.cpp | NonMatching | 8 | 0 | 0 | 7 |
| u_80120D98.cpp | NonMatching | 0 | 4 | 0 | 3 |
| u_80127D80.cpp | NonMatching | 1 | 2 | 0 | 2 |
| u_8013CA38.cpp | NonMatching | 0 | 1 | 0 | 1 |
| u_80153C10.cpp | NonMatching | 1 | 0 | 0 | 1 |
| u_80157730.cpp | NonMatching | 0 | 1 | 0 | 1 |
| u_80161FF4.cpp | NonMatching | 5 | 1 | 0 | 3 |
| u_8016D170.cpp | Matching | 0 | 1 | 0 | 1 |
| u_80176044.cpp | NonMatching | 9 | 1 | 0 | 5 |
| u_8017D794.cpp | NonMatching | 2 | 0 | 0 | 2 |
| u_8018D2D4.cpp | NonMatching | 0 | 1 | 0 | 1 |
| u_801A28A4.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_801AE50C.cpp | NonMatching | 0 | 1 | 0 | 1 |
| u_801CC6D0.cpp | NonMatching | 8 | 0 | 0 | 3 |
| u_801CDFBC.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_801D25CC.cpp | NonMatching | 1 | 1 | 0 | 2 |
| u_801D8FD4.cpp | NonMatching | 1 | 0 | 0 | 1 |
| u_801D9A70.cpp | NonMatching | 1 | 0 | 0 | 1 |
| u_801E241C.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_801F7F48.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_801FC634.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_80202AB8.cpp | NonMatching | 3 | 0 | 0 | 1 |
| u_802052B4.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_8020A650.cpp | NonMatching | 2 | 2 | 0 | 1 |
| u_8020B77C.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_8020D47C.cpp | NonMatching | 4 | 0 | 0 | 1 |
| u_80210D80.cpp | NonMatching | 3 | 0 | 0 | 2 |
| u_80221100.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_80222C08.cpp | NonMatching | 0 | 1 | 0 | 1 |
| u_80226DE8.cpp | NonMatching | 7 | 4 | 0 | 3 |
| u_8022ED88.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_80230504.cpp | NonMatching | 1 | 0 | 0 | 1 |
| u_80231550.cpp | NonMatching | 5 | 1 | 0 | 1 |
| u_80244728.cpp | NonMatching | 1 | 0 | 0 | 1 |
| u_80253614.cpp | Matching | 0 | 1 | 0 | 1 |
| u_80259AB0.cpp | NonMatching | 4 | 0 | 0 | 2 |
| u_8025ECF8.cpp | NonMatching | 6 | 0 | 0 | 2 |
| u_802690C8.cpp | NonMatching | 2 | 0 | 0 | 1 |
| u_8028865C.cpp | NonMatching | 3 | 0 | 0 | 2 |
| u_8029E714.cpp | NonMatching | 3 | 1 | 0 | 3 |
| u_802A815C.cpp | NonMatching | 1 | 0 | 0 | 1 |
| u_802A9824.cpp | NonMatching | 4 | 1 | 0 | 3 |
| u_802B7120.cpp | NonMatching | 0 | 1 | 0 | 1 |
| u_802C30A4.cpp | NonMatching | 1 | 0 | 0 | 1 |
| u_802CED5C.cpp | NonMatching | 1 | 0 | 0 | 1 |
| u_802DDEDC.cpp | NonMatching | 2 | 1 | 0 | 2 |

## Appendix B: all 336 sites (addr, shape, sub-shape, unit, function)

```
0x80009ca4	NORM3	frsp-fwd	game/Cube.cpp	fn_800098B8
0x80009d54	NORM3	asm-lfs	game/Cube.cpp	fn_800098B8
0x80015f98	NORM3	frsp-fwd	game/u_80015B64.cpp	ViewFrustumBuild__FPfPC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3ff
0x800160dc	NORM3	frsp-fwd	game/u_80015B64.cpp	ViewFrustumBuild__FPfPC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3ff
0x80016220	NORM3	frsp-fwd	game/u_80015B64.cpp	ViewFrustumBuild__FPfPC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3ff
0x80016364	NORM3	frsp-fwd	game/u_80015B64.cpp	ViewFrustumBuild__FPfPC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3ff
0x80016454	NORM3	asm-lfs	game/u_80015B64.cpp	ViewFrustumBuild__FPfPC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3PC4Vec3ff
0x8001669c	NORM3	asm-lfs	game/u_80015B64.cpp	ViewFrustumUpdate__FPfPC4Vec3PC4Vec3PC4Vec3ffff
0x8001699c	NORM3	frsp-fwd	game/u_80015B64.cpp	fn_800167EC__FPfPC4Vec3PC4Vec3PC4Vec3ff
0x80016bb4	NORM3	frsp-fwd	game/u_80015B64.cpp	fn_800167EC__FPfPC4Vec3PC4Vec3PC4Vec3ff
0x80016cf8	NORM3	frsp-fwd	game/u_80015B64.cpp	fn_800167EC__FPfPC4Vec3PC4Vec3PC4Vec3ff
0x80016e3c	NORM3	frsp-fwd	game/u_80015B64.cpp	fn_800167EC__FPfPC4Vec3PC4Vec3PC4Vec3ff
0x80016f80	NORM3	frsp-fwd	game/u_80015B64.cpp	fn_800167EC__FPfPC4Vec3PC4Vec3PC4Vec3ff
0x8001b6b8	NORM3	asm-lfs	game/DbRoom.c	fn_8001B5E8
0x8001b798	NORM3	asm-lfs	game/DbRoom.c	fn_8001B5E8
0x8001b878	NORM3	asm-lfs	game/DbRoom.c	fn_8001B5E8
0x80026398	NORM3	frsp-fwd	game/u_80022B10.cpp	AuxAimAngle__FPi
0x80026448	NORM3	asm-lfs	game/u_80022B10.cpp	AuxAimAngle__FPi
0x80027040	NORM3	asm-lfs	game/ModInfo.cpp	fn_80026B50
0x80027e08	NORM3	frsp-fwd	game/ModInfo.cpp	fn_80027CD8
0x8002a06c	NORM3	asm-lfs	game/ModInfo.cpp	fn_80029F4C
0x8002a168	NORM3	frsp-fwd	game/ModInfo.cpp	fn_80029F4C
0x8002a3e4	NORM3	asm-lfs	game/ModInfo.cpp	fn_8002A274
0x8002a494	NORM3	asm-lfs	game/ModInfo.cpp	fn_8002A274
0x8002a4fc	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002A274
0x8002a558	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002A274
0x8002a71c	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002A274
0x8002a860	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002A274
0x8002c144	NORM3	asm-lfs	game/ModInfo.cpp	fn_8002C014
0x8002c37c	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c3d0	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c48c	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c4dc	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c598	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c5e8	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c6a4	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c6f4	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c7b0	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c800	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c8c4	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002c914	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002C014
0x8002cc24	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002CBE4
0x8002cc74	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002CBE4
0x8002d1ec	NORM3	asm-lfs	game/ModInfo.cpp	fn_8002D11C
0x8002d2c0	NORM3	asm-lfs	game/ModInfo.cpp	fn_8002D11C
0x8002d394	NORM3	asm-lfs	game/ModInfo.cpp	fn_8002D11C
0x8002e580	NORM3	asm-lfs	game/ModInfo.cpp	fn_8002E438
0x8002e76c	NORM3	asm-lfs	game/ModInfo.cpp	fn_8002E438
0x8002ec1c	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002E438
0x8002ec70	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002E438
0x8002ef5c	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002E438
0x8002efb0	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002E438
0x8002f204	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002F108
0x8002f260	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002F108
0x8002f320	NORM3	asm-lfs	game/ModInfo.cpp	fn_8002F108
0x8002f538	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002F3F8
0x8002f6b0	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8002F3F8
0x80032758	NORM3	frsp-fwd	game/ModInfo.cpp	fn_8003247C
0x80032b5c	NORM3	frsp-fwd	game/ModInfo.cpp	fn_8003247C
0x800354fc	NORM3	frsp-fwd	game/ModInfo.cpp	fn_800353F8
0x8003568c	NORM3	asm-lfs	game/ModInfo.cpp	fn_800353F8
0x80035988	NORM3	frsp-fwd	game/ModInfo.cpp	fn_800357E0
0x8003645c	NORM3	asm-lfs	game/ModInfo.cpp	fn_800362E0
0x8003a390	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8003A1F0
0x8003a594	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8003A3F0
0x8003acac	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	ModelAnimate__FiPci
0x8003af7c	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8003AE54
0x8003b138	FASTSQRT	ifelse/frsp	game/ModInfo.cpp	fn_8003AE54
0x8003f85c	NORM3	frsp-fwd	game/u_8003CC90.cpp	fn_8003F15C
0x8003fce0	NORM3	frsp-fwd	game/u_8003CC90.cpp	fn_8003FB58
0x8003fe6c	NORM3	frsp-fwd	game/u_8003CC90.cpp	fn_8003FB58
0x80041ac4	NORM3	frsp-fwd	game/u_8003CC90.cpp	fn_8004189C
0x80041cf0	NORM3	frsp-fwd	game/u_8003CC90.cpp	fn_80041B64
0x80049390	NORM3	frsp-fwd	game/FearActor.c	fn_80048BA8
0x8004ad7c	FASTSQRT	ifelse/frsp	game/FearActor.c	fn_8004A370
0x8005a07c	NORM3	frsp-fwd	game/FearActor.c	fn_80059EA4
0x8005a12c	NORM3	asm-lfs	game/FearActor.c	fn_80059EA4
0x80063d24	FASTSQRT	ifelse/frsp	game/FearActor.c	fn_8006353C
0x800643b4	FASTSQRT	ifelse/frsp	game/FearActor.c	fn_80064038
0x80069fc4	NORM3	asm-lfs	game/u_80068004.cpp	PlayerAimUpdate__FP6Playerf
0x8006a074	NORM3	asm-lfs	game/u_80068004.cpp	PlayerAimUpdate__FP6Playerf
0x8006a8c8	FASTSQRT	ifelse/frsp	game/u_80068004.cpp	PlayerAimUpdate__FP6Playerf
0x8006b784	NORM3	frsp-fwd	game/u_80068004.cpp	AimInputFromPad__FPfP6PlayerP5Actor
0x8006b868	NORM3	frsp-fwd	game/u_80068004.cpp	AimInputFromPad__FPfP6PlayerP5Actor
0x8006bac0	NORM3	asm-lfs	game/u_80068004.cpp	AimInputFromPad__FPfP6PlayerP5Actor
0x8006bfac	NORM3	frsp-fwd	game/u_80068004.cpp	AimAssistUpdate__FPfP6PlayerP5Actor
0x8006c060	NORM3	asm-lfs	game/u_80068004.cpp	AimAssistUpdate__FPfP6PlayerP5Actor
0x8006c198	NORM3	frsp-fwd	game/u_80068004.cpp	AimAssistUpdate__FPfP6PlayerP5Actor
0x8006c368	NORM3	asm-lfs	game/u_80068004.cpp	AimAssistUpdate__FPfP6PlayerP5Actor
0x8006c41c	NORM3	asm-lfs	game/u_80068004.cpp	AimAssistUpdate__FPfP6PlayerP5Actor
0x8006c500	NORM3	asm-lfs	game/u_80068004.cpp	AimAssistUpdate__FPfP6PlayerP5Actor
0x8006f34c	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionMove__FPvPf
0x8006f518	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionMove__FPvPf
0x8006f920	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionMove__FPvPf
0x8006ffc4	FASTSQRT	ifelse/frsp	game/u_8006C604.cpp	PlayerCollisionSlideVector__FiPf
0x80070020	FASTSQRT	ifelse/frsp	game/u_8006C604.cpp	PlayerCollisionSlideVector__FiPf
0x800700e0	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionSlideVector__FiPf
0x800702e0	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x80070440	NORM3	frsp-fwd	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x80070768	NORM3	frsp-fwd	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x80070bdc	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x80070d5c	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x80070e68	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x800710a4	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x80071774	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x80071b80	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x80071ddc	NORM3	asm-lfs	game/u_8006C604.cpp	PlayerCollisionCallback__FPiPf
0x80074b10	NORM3	asm-lfs	game/u_8006C604.cpp	fn_80074018
0x80074ff8	NORM3	asm-lfs	game/u_8006C604.cpp	fn_80074D68
0x800750ac	NORM3	asm-lfs	game/u_8006C604.cpp	fn_80074D68
0x8007621c	NORM3	frsp-fwd	game/u_8006C604.cpp	fn_80075F94
0x800762cc	NORM3	asm-lfs	game/u_8006C604.cpp	fn_80075F94
0x80076414	NORM3	frsp-fwd	game/u_8006C604.cpp	fn_80075F94
0x800764c8	NORM3	asm-lfs	game/u_8006C604.cpp	fn_80075F94
0x800896c8	NORM3	asm-lfs	game/u_800886C8.cpp	FxBuildScrollItemTransform__FPiPiPi
0x8008a24c	NORM3	frsp-fwd	game/u_800886C8.cpp	FxBuildScreenScrollItemWave__FPiPiPi
0x8008a3c4	NORM3	asm-lfs	game/u_800886C8.cpp	FxBuildScreenScrollItemWave__FPiPiPi
0x8008ab44	NORM3	frsp-fwd	game/u_800886C8.cpp	FxBuildScreenScrollItemsTex__FPiPiPi
0x8008b0b4	NORM3	frsp-fwd	game/u_800886C8.cpp	FxBuildScrollItemAlongSegment__FPiPiPi
0x8008b4bc	NORM3	frsp-fwd	game/u_800886C8.cpp	FxBuildScrollItemWaveAtClock__FPiPiPi
0x8008b634	NORM3	asm-lfs	game/u_800886C8.cpp	FxBuildScrollItemWaveAtClock__FPiPiPi
0x80092b24	FASTSQRT	ifelse/frsp	game/u_8008DCDC.cpp	fn_800929C0
0x80092c38	FASTSQRT	ifelse/frsp	game/u_8008DCDC.cpp	fn_800929C0
0x80092d20	FASTSQRT	ifelse/frsp	game/u_8008DCDC.cpp	fn_800929C0
0x80092dec	FASTSQRT	ifelse/frsp	game/u_8008DCDC.cpp	fn_800929C0
0x8009e1fc	NORM3	frsp-fwd	game/u_8009DE94.cpp	BasisFromVecs__FPfP4Vec3P4Vec3P4Vec3
0x8009e2c0	NORM3	frsp-fwd	game/u_8009DE94.cpp	BasisFromVecs__FPfP4Vec3P4Vec3P4Vec3
0x8009e3e8	NORM3	frsp-fwd	game/u_8009DE94.cpp	BasisFromVecs__FPfP4Vec3P4Vec3P4Vec3
0x8009e570	NORM3	asm-lfs	game/u_8009DE94.cpp	BasisFromVecs__FPfP4Vec3P4Vec3P4Vec3
0x8009e698	NORM3	frsp-fwd	game/u_8009DE94.cpp	BasisFromVecs__FPfP4Vec3P4Vec3P4Vec3
0x8009e75c	NORM3	frsp-fwd	game/u_8009DE94.cpp	BasisFromVecs__FPfP4Vec3P4Vec3P4Vec3
0x8009e8e4	NORM3	asm-lfs	game/u_8009DE94.cpp	BasisFromVecs__FPfP4Vec3P4Vec3P4Vec3
0x8009ea0c	NORM3	frsp-fwd	game/u_8009DE94.cpp	BasisFromVecs__FPfP4Vec3P4Vec3P4Vec3
0x8009eb30	NORM3	asm-lfs	game/u_8009DE94.cpp	BasisFromVecs__FPfP4Vec3P4Vec3P4Vec3
0x8009f838	NORM3	asm-lfs	game/u_8009DE94.cpp	fn_8009F7A8
0x8009f928	NORM3	frsp-fwd	game/u_8009DE94.cpp	fn_8009F7A8
0x800a0b0c	FASTSQRT	ifelse/frsp	game/u_8009DE94.cpp	BoneSetMatrix__FPvP5Mtx34
0x800a0be4	FASTSQRT	ifelse/frsp	game/u_8009DE94.cpp	BoneSetMatrix__FPvP5Mtx34
0x800a0c6c	FASTSQRT	ifelse/frsp	game/u_8009DE94.cpp	BoneSetMatrix__FPvP5Mtx34
0x800a0cf4	FASTSQRT	ifelse/frsp	game/u_8009DE94.cpp	BoneSetMatrix__FPvP5Mtx34
0x800a1100	SQRT-reg	inline-lfs	game/u_8009DE94.cpp	fn_800A109C
0x800a5af4	FASTSQRT	ifelse/frsp	game/u_8009DE94.cpp	fn_800A59B0
0x800a822c	FASTSQRT	ifelse/frsp	game/u_800A6F18.cpp	ZoneFindNearestVisit__FPvPvPv
0x800a841c	NORM3	asm-lfs	game/u_800A6F18.cpp	ZoneClassifyDirection__FPvP4Vec3
0x800a9118	NORM3	frsp-fwd	game/u_800A6F18.cpp	FollowLineCheckActorDist__FPvPvf
0x800aa148	FASTSQRT	ifelse/frsp	game/u_800A6F18.cpp	OwnerValue__FP5OwnerPv
0x800aa210	FASTSQRT	ifelse/frsp	game/u_800A6F18.cpp	OwnerValue__FP5OwnerP4Vec3
0x800aa840	NORM3	frsp-fwd	game/u_800A6F18.cpp	ActorAimAngleToPos__FPvP4Vec3
0x800aa964	NORM3	frsp-fwd	game/u_800A6F18.cpp	ActorAimAngleToPos__FPvP4Vec3
0x800aab54	NORM3	frsp-fwd	game/u_800A6F18.cpp	ActorAngleToPos__FPvP4Vec3
0x800aac18	FASTSQRT	ifelse/frsp	game/u_800A6F18.cpp	Distance2D__FPfPf
0x800aac7c	FASTSQRT	ifelse/frsp	game/u_800A6F18.cpp	Distance3D__FPfPf
0x800b3fec	NORM3	frsp-fwd	game/u_800B1834.cpp	OwnerFindSightTarget__FP5OwnerPiPfPiPUlii
0x800b409c	NORM3	asm-lfs	game/u_800B1834.cpp	OwnerFindSightTarget__FP5OwnerPiPfPiPUlii
0x800b5d48	FASTSQRT	ifelse/frsp	game/u_800B5028.cpp	fn_800B5BF8
0x800b68e0	FASTSQRT	ifelse/frsp	game/u_800B5028.cpp	fn_800B5E10
0x800b74a0	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B7098
0x800b75f4	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B7098
0x800b7788	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B7098
0x800b78dc	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B7098
0x800b7a70	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B7098
0x800b7bc4	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B7098
0x800b7f1c	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B7CE0
0x800b8090	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B7CE0
0x800b81b8	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B7CE0
0x800b8374	NORM3	asm-lfs	game/u_800B5028.cpp	fn_800B7CE0
0x800b872c	NORM3	frsp-fwd	game/u_800B5028.cpp	fn_800B84E4
0x800b8938	NORM3	asm-lfs	game/u_800B5028.cpp	fn_800B84E4
0x800be768	NORM3	frsp-fwd	game/u_800BAA14.cpp	NavRouteCost__FiiiPvP11SearchState
0x800be818	NORM3	asm-lfs	game/u_800BAA14.cpp	NavRouteCost__FiiiPvP11SearchState
0x800be9a0	NORM3	asm-lfs	game/u_800BAA14.cpp	NavRouteCost__FiiiPvP11SearchState
0x800bea50	NORM3	asm-lfs	game/u_800BAA14.cpp	NavRouteCost__FiiiPvP11SearchState
0x800c3830	FASTSQRT	ifelse/frsp	game/u_800BAA14.cpp	fn_800C30FC
0x800c3900	FASTSQRT	ifelse/frsp	game/u_800BAA14.cpp	fn_800C30FC
0x800cb5c8	NORM3	asm-lfs	game/u_800C5028.cpp	fn_800CADE0
0x800cb678	NORM3	asm-lfs	game/u_800C5028.cpp	fn_800CADE0
0x800d5384	FASTSQRT	ifelse/frsp	game/u_800D32E8.cpp	EmitterSpawnSample__FfP7EmitterP11EmitterBankP11EmitterDescP12EmitterGroupP20EmitterParticleStateP15EmitterSlotDataP5Mtx34
0x800d55a4	NORM3	asm-lfs	game/u_800D32E8.cpp	EmitterSpawnSample__FfP7EmitterP11EmitterBankP11EmitterDescP12EmitterGroupP20EmitterParticleStateP15EmitterSlotDataP5Mtx34
0x800d5b6c	NORM3	frsp-fwd	game/u_800D32E8.cpp	EmitterSpawnAccumForce
0x800d65a0	NORM3	asm-lfs	game/u_800D32E8.cpp	EmitterSpawnTracePick__FfP16EmitterSpawnDataP4Vec3P4Vec3PfP7EmitterPf
0x800d6744	NORM3	asm-lfs	game/u_800D32E8.cpp	EmitterSpawnTracePick__FfP16EmitterSpawnDataP4Vec3P4Vec3PfP7EmitterPf
0x800d6898	NORM3	asm-lfs	game/u_800D32E8.cpp	EmitterSpawnTracePick__FfP16EmitterSpawnDataP4Vec3P4Vec3PfP7EmitterPf
0x800d7600	NORM3	asm-lfs	game/u_800D32E8.cpp	fn_800D72CC
0x800d76f8	NORM3	asm-lfs	game/u_800D32E8.cpp	fn_800D72CC
0x800d7800	NORM3	regs	game/u_800D32E8.cpp	fn_800D72CC
0x800d7cf4	NORM3	asm-lfs	game/u_800D32E8.cpp	fn_800D72CC
0x800d7dec	NORM3	asm-lfs	game/u_800D32E8.cpp	fn_800D72CC
0x800d7ef4	NORM3	regs	game/u_800D32E8.cpp	fn_800D72CC
0x800e0d54	FASTSQRT	ifelse/frsp	game/u_800E0CB8.cpp	fn_800E0CB8
0x800e0e78	FASTSQRT	ifelse/frsp	game/u_800E0CB8.cpp	fn_800E0DBC
0x800e0f64	FASTSQRT	ifelse/frsp	game/u_800E0CB8.cpp	fn_800E0E9C
0x800e1080	FASTSQRT	ifelse/frsp	game/u_800E0CB8.cpp	fn_800E1020
0x800e1104	FASTSQRT	ifelse/frsp	game/u_800E0CB8.cpp	fn_800E1098
0x800eccd4	FASTSQRT	ifelse/frsp	game/u_800EB1B8.cpp	fn_800ECB60
0x800ed010	FASTSQRT	ifelse/frsp	game/u_800EB1B8.cpp	fn_800ECE14
0x800eeed8	FASTSQRT	ifelse/frsp	game/u_800EB1B8.cpp	fn_800EEADC
0x800ef5e4	NORM3	frsp-fwd	game/u_800EB1B8.cpp	fn_800EF334
0x800ef700	NORM3	frsp-fwd	game/u_800EB1B8.cpp	fn_800EF334
0x8010f41c	FASTSQRT	ifelse/frsp	game/CubeTev.cpp	fn_8010F110
0x801131b0	NORM3	asm-lfs	game/u_80112308.cpp	fn_80112D84
0x80113d30	NORM3	asm-lfs	game/u_80112308.cpp	fn_80113AF4
0x80113fc0	NORM3	asm-lfs	game/u_80112308.cpp	fn_80113DBC
0x8011431c	NORM3	asm-lfs	game/u_80112308.cpp	fn_801140E8
0x80114408	NORM3	asm-lfs	game/u_80112308.cpp	fn_801140E8
0x80114730	NORM3	asm-lfs	game/u_80112308.cpp	fn_801145FC
0x80114bc0	NORM3	asm-lfs	game/u_80112308.cpp	fn_8011498C
0x80114e48	NORM3	asm-lfs	game/u_80112308.cpp	fn_80114C4C
0x80122acc	FASTSQRT	ifelse/frsp	game/u_80120D98.cpp	HazardSplineProximityFx__FP17HazardSplineQueryP16HazardSplineZone
0x80122bdc	FASTSQRT	ifelse/frsp	game/u_80120D98.cpp	HazardSplineZoneCheck__FfP17HazardSplineActorP16HazardSplineZone
0x80123528	FASTSQRT	ifelse/frsp	game/u_80120D98.cpp	HazardTrackTarget__FfiP13HazardMsgPortP16HazardTrackActor
0x801235a0	FASTSQRT	ifelse/frsp	game/u_80120D98.cpp	HazardTrackTarget__FfiP13HazardMsgPortP16HazardTrackActor
0x8012e3c0	NORM3	frsp-fwd	game/u_80127D80.cpp	fn_8012E11C
0x8012eb14	FASTSQRT	ifelse/frsp	game/u_80127D80.cpp	fn_8012E9C0
0x8012eb8c	FASTSQRT	ifelse/frsp	game/u_80127D80.cpp	fn_8012E9C0
0x8013ce8c	FASTSQRT	ifelse/frsp	game/u_8013CA38.cpp	fn_8013CACC
0x80156fd0	NORM3	asm-lfs	game/u_80153C10.cpp	GhostFlashFindSpot__FP5Mtx34P9GhostSlotf
0x8015a328	FASTSQRT	ifelse/frsp	game/u_80157730.cpp	fn_8015A1A0
0x8016227c	FASTSQRT	ifelse/frsp	game/u_80161FF4.cpp	Fn80161FF4__FP5OwnerPvf
0x80162318	NORM3	asm-lfs	game/u_80161FF4.cpp	Fn80161FF4__FP5OwnerPvf
0x80162c18	NORM3	asm-lfs	game/u_80161FF4.cpp	Fn8016295C__FP5OwnerP3Beh
0x80162cc8	NORM3	asm-lfs	game/u_80161FF4.cpp	Fn8016295C__FP5OwnerP3Beh
0x801696e0	NORM3	frsp-fwd	game/u_80161FF4.cpp	fn_80168BB8
0x80169790	NORM3	asm-lfs	game/u_80161FF4.cpp	fn_80168BB8
0x8016d490	FASTSQRT	ifelse/frsp	game/u_8016D170.cpp	fn_8016D440
0x80175c78	NORM3	asm-lfs	game/Msx.cpp	fn_80175A90
0x80176234	NORM3	regs	game/u_80176044.cpp	fn_80176044
0x80176b84	NORM3	frsp-fwd	game/u_80176044.cpp	GunRefreshAimBasis__FPv
0x80176c64	NORM3	frsp-fwd	game/u_80176044.cpp	GunRefreshAimBasis__FPv
0x801770ec	NORM3	asm-lfs	game/u_80176044.cpp	GunAimTargetUpdate80176F14__FPvPv
0x8017719c	NORM3	asm-lfs	game/u_80176044.cpp	GunAimTargetUpdate80176F14__FPvPv
0x801794b4	NORM3	frsp-fwd	game/u_80176044.cpp	GunAimMatrixUpdate80179268__FfP5OwnerP3Beh
0x8017957c	FASTSQRT	ifelse/frsp	game/u_80176044.cpp	GunAimMatrixUpdate80179268__FfP5OwnerP3Beh
0x80179618	NORM3	asm-lfs	game/u_80176044.cpp	GunAimMatrixUpdate80179268__FfP5OwnerP3Beh
0x8017a100	NORM3	asm-lfs	game/u_80176044.cpp	GunAimTargetUpdate80179F08__FPvPv
0x8017a1b0	NORM3	asm-lfs	game/u_80176044.cpp	GunAimTargetUpdate80179F08__FPvPv
0x8017e020	NORM3	frsp-fwd	game/u_8017D794.cpp	fn_8017DF34
0x80184b10	NORM3	frsp-fwd	game/u_8017D794.cpp	fn_80184A70
0x8018dfa4	FASTSQRT	ifelse/frsp	game/u_8018D2D4.cpp	GhostShotUpdateAll__Ff
0x801a5774	NORM3	frsp-fwd	game/u_801A28A4.cpp	GeistA28A4FetchAimCheck__Fii
0x801a589c	NORM3	frsp-fwd	game/u_801A28A4.cpp	GeistA28A4FetchAimCheck__Fii
0x801b444c	FASTSQRT	ifelse/frsp	game/u_801AE50C.cpp	fn_801B3DE8
0x801cc9d4	NORM3	asm-lfs	game/u_801CC6D0.cpp	fn_801CC7D4
0x801ccb84	NORM3	asm-lfs	game/u_801CC6D0.cpp	fn_801CC7D4
0x801ccc80	NORM3	asm-lfs	game/u_801CC6D0.cpp	fn_801CC7D4
0x801ccf94	NORM3	asm-lfs	game/u_801CC6D0.cpp	fn_801CCCE0
0x801cd0e8	NORM3	asm-lfs	game/u_801CC6D0.cpp	fn_801CCCE0
0x801cd6d8	NORM3	asm-lfs	game/u_801CC6D0.cpp	fn_801CD484
0x801cd78c	NORM3	asm-lfs	game/u_801CC6D0.cpp	fn_801CD484
0x801cdc54	NORM3	asm-lfs	game/u_801CC6D0.cpp	fn_801CD484
0x801ce7b8	NORM3	asm-lfs	game/u_801CDFBC.cpp	fn_801CE5D4
0x801ce868	NORM3	asm-lfs	game/u_801CDFBC.cpp	fn_801CE5D4
0x801d2a2c	FASTSQRT	ifelse/frsp	game/u_801D25CC.cpp	RoomGeomCapsuleCollide__FP4GeomP4GeomiP1
0x801d38d4	NORM3	frsp-fwd	game/u_801D25CC.cpp	RoomGeomBoxCollide__FP4GeomP4GeomiP11Con
0x801d9978	NORM3	frsp-fwd	game/u_801D8FD4.cpp	fn_801D96C4
0x801dfd34	NORM3	asm-lfs	game/u_801D9A70.cpp	fn_801DF948
0x801e6c74	NORM3	asm-lfs	game/u_801E241C.cpp	fn_801E69AC
0x801e6f60	NORM3	asm-lfs	game/u_801E241C.cpp	fn_801E69AC
0x801fc1c4	NORM3	frsp-fwd	game/u_801F7F48.cpp	fn_801FC08C
0x801fc298	NORM3	asm-lfs	game/u_801F7F48.cpp	fn_801FC08C
0x801fce54	NORM3	frsp-fwd	game/u_801FC634.cpp	FearRivetUpdate__FP9FearRivetP5Actor
0x801fcf14	NORM3	asm-lfs	game/u_801FC634.cpp	FearRivetUpdate__FP9FearRivetP5Actor
0x80203474	NORM3	frsp-fwd	game/u_80202AB8.cpp	HostActorAimTimerCheck__FP5ActorP5Actorf
0x802035a0	NORM3	frsp-fwd	game/u_80202AB8.cpp	HostActorAimTimerCheck__FP5ActorP5Actorf
0x8020376c	NORM3	frsp-fwd	game/u_80202AB8.cpp	HostActorAimTimerCheck__FP5ActorP5Actorf
0x802058bc	NORM3	frsp-fwd	game/u_802052B4.cpp	TurretFireUpdate__FPvf
0x802059e0	NORM3	frsp-fwd	game/u_802052B4.cpp	TurretFireUpdate__FPvf
0x8020ae0c	NORM3	asm-lfs	game/u_8020A650.cpp	u8020A650BuildModel__FP14u8020A650OwneriPUcP13u8020A650AnimP5Mtx34
0x8020ae74	FASTSQRT	ifelse/frsp	game/u_8020A650.cpp	u8020A650BuildModel__FP14u8020A650OwneriPUcP13u8020A650AnimP5Mtx34
0x8020b524	NORM3	frsp-fwd	game/u_8020A650.cpp	u8020A650BuildModel__FP14u8020A650OwneriPUcP13u8020A650AnimP5Mtx34
0x8020b58c	FASTSQRT	ifelse/frsp	game/u_8020A650.cpp	u8020A650BuildModel__FP14u8020A650OwneriPUcP13u8020A650AnimP5Mtx34
0x8020d0f8	NORM3	frsp-fwd	game/u_8020B77C.cpp	FxEmitterAimUpdate__FfP7FxActor
0x8020d2ac	NORM3	frsp-fwd	game/u_8020B77C.cpp	FxEmitterAimUpdate__FfP7FxActor
0x8020e500	NORM3	frsp-fwd	game/u_8020D47C.cpp	PossessShotFire__FPvii
0x8020e628	NORM3	asm-lfs	game/u_8020D47C.cpp	PossessShotFire__FPvii
0x8020e9cc	NORM3	frsp-fwd	game/u_8020D47C.cpp	PossessShotFire__FPvii
0x8020eac8	NORM3	asm-lfs	game/u_8020D47C.cpp	PossessShotFire__FPvii
0x80211920	NORM3	frsp-fwd	game/u_80210D80.cpp	PerturbDirectionRandom__FPfPff
0x80211a24	NORM3	asm-lfs	game/u_80210D80.cpp	PerturbDirectionRandom__FPfPff
0x80211d80	NORM3	frsp-fwd	game/u_80210D80.cpp	Fn80211AA4__FP5Actorf
0x80221bbc	NORM3	frsp-fwd	game/u_80221100.cpp	BeesUpdate__FP5Actor
0x80221c5c	NORM3	asm-lfs	game/u_80221100.cpp	BeesUpdate__FP5Actor
0x80224314	FASTSQRT	ifelse/frsp	game/u_80222C08.cpp	RivetSelect__FP9RivetDataP10RivetActor
0x80228010	FASTSQRT	ifelse/frsp	game/u_80226DE8.cpp	fn_80227DD8
0x802280ac	NORM3	asm-lfs	game/u_80226DE8.cpp	fn_80227DD8
0x80228bd0	FASTSQRT	ifelse/frsp	game/u_80226DE8.cpp	fn_80228AE4
0x8022a408	FASTSQRT	ifelse/frsp	game/u_80226DE8.cpp	fn_8022A180
0x8022a4a4	NORM3	asm-lfs	game/u_80226DE8.cpp	fn_8022A180
0x8022a6a0	NORM3	frsp-fwd	game/u_80226DE8.cpp	fn_8022A180
0x8022a768	FASTSQRT	ifelse/frsp	game/u_80226DE8.cpp	fn_8022A180
0x8022a804	NORM3	asm-lfs	game/u_80226DE8.cpp	fn_8022A180
0x8022a8bc	NORM3	frsp-fwd	game/u_80226DE8.cpp	fn_8022A180
0x8022aa28	NORM3	frsp-fwd	game/u_80226DE8.cpp	fn_8022A180
0x8022aad8	NORM3	asm-lfs	game/u_80226DE8.cpp	fn_8022A180
0x8022f3c8	NORM3	frsp-fwd	game/u_8022ED88.cpp	SwarmUpdate__FP5Actorf
0x8022f468	NORM3	asm-lfs	game/u_8022ED88.cpp	SwarmUpdate__FP5Actorf
0x802307a4	NORM3	frsp-fwd	game/u_80230504.cpp	ExtraTrailApply__FP11ExtraHolderP8ExtraMsg
0x80231d30	NORM3	asm-lfs	game/u_80231550.cpp	ShotFire__FP5ActoriiffP4Vec3P5Mtx34P4Vec3P4Vec3
0x80232110	NORM3	frsp-fwd	game/u_80231550.cpp	ShotFire__FP5ActoriiffP4Vec3P5Mtx34P4Vec3P4Vec3
0x8023268c	NORM3	regs	game/u_80231550.cpp	ShotFire__FP5ActoriiffP4Vec3P5Mtx34P4Vec3P4Vec3
0x80232740	NORM3	frsp-fwd	game/u_80231550.cpp	ShotFire__FP5ActoriiffP4Vec3P5Mtx34P4Vec3P4Vec3
0x80232990	FASTSQRT	ifelse/frsp	game/u_80231550.cpp	ShotFire__FP5ActoriiffP4Vec3P5Mtx34P4Vec3P4Vec3
0x80232ad8	NORM3	frsp-fwd	game/u_80231550.cpp	ShotFire__FP5ActoriiffP4Vec3P5Mtx34P4Vec3P4Vec3
0x80237ce0	NORM3	frsp-fwd	game/Corona.cpp	fn_80237B84
0x80246d74	NORM3	frsp-fwd	game/u_80244728.cpp	FoeUpdateChase__FP5Actor
0x802538ac	FASTSQRT	ifelse/frsp	game/u_80253614.cpp	ImpFireUpdate__FP9EffectObjf
0x8025d1c0	NORM3	frsp-fwd	game/u_80259AB0.cpp	fn_8025CCAC
0x8025d270	NORM3	asm-lfs	game/u_80259AB0.cpp	fn_8025CCAC
0x8025dcac	NORM3	frsp-fwd	game/u_80259AB0.cpp	fn_8025D9E8
0x8025dd5c	NORM3	asm-lfs	game/u_80259AB0.cpp	fn_8025D9E8
0x80261928	NORM3	frsp-fwd	game/u_8025ECF8.cpp	fn_80261480
0x802619d8	NORM3	asm-lfs	game/u_8025ECF8.cpp	fn_80261480
0x8026221c	NORM3	frsp-fwd	game/u_8025ECF8.cpp	fn_80261D58
0x802622cc	NORM3	asm-lfs	game/u_8025ECF8.cpp	fn_80261D58
0x80262550	NORM3	frsp-fwd	game/u_8025ECF8.cpp	fn_80261D58
0x8026260c	NORM3	frsp-fwd	game/u_8025ECF8.cpp	fn_80261D58
0x8026964c	NORM3	frsp-fwd	game/u_802690C8.cpp	HermitSpitAmmoFire__FiiPvi
0x80269774	NORM3	frsp-fwd	game/u_802690C8.cpp	HermitSpitAmmoFire__FiiPvi
0x80289550	NORM3	asm-lfs	game/u_8028865C.cpp	ClockComputeViewTransform__FPvPvPv
0x80289660	NORM3	asm-lfs	game/u_8028865C.cpp	ClockComputeViewTransform__FPvPvPv
0x80289858	NORM3	asm-lfs	game/u_8028865C.cpp	ClockSetupPageBasis__FPvPvPv
0x802a1944	FASTSQRT	ifelse/frsp	game/u_8029E714.cpp	fn_802A163C
0x802a318c	NORM3	frsp-fwd	game/u_8029E714.cpp	fn_802A2C94
0x802a4568	NORM3	frsp-fwd	game/u_8029E714.cpp	fn_802A42C4
0x802a4684	NORM3	asm-lfs	game/u_8029E714.cpp	fn_802A42C4
0x802a91e4	NORM3	frsp-fwd	game/u_802A815C.cpp	Actor62UpdatePath__FUlPii
0x802ab4ac	NORM3	frsp-fwd	game/u_802A9824.cpp	AllyUpdateState1__FP5Actor
0x802acc28	FASTSQRT	ifelse/frsp	game/u_802A9824.cpp	AllyUpdate__FP5Actor
0x802ad100	NORM3	asm-lfs	game/u_802A9824.cpp	AllyTargetFilter__FiiP5Actor
0x802ad1b0	NORM3	asm-lfs	game/u_802A9824.cpp	AllyTargetFilter__FiiP5Actor
0x802ad3b4	NORM3	frsp-fwd	game/u_802A9824.cpp	AllyTargetFilter__FiiP5Actor
0x802b8eec	FASTSQRT	ifelse/frsp	game/u_802B7120.cpp	ImpThrowFireball802B8A24__FiiPiPvf
0x802c3e6c	NORM3	frsp-fwd	game/u_802C30A4.cpp	BossHitFeedback__FP7BossMsg
0x802d0224	NORM3	asm-lfs	game/u_802CED5C.cpp	fn_802CFEA4
0x802e1f64	FASTSQRT	if-skip	game/u_802DDEDC.cpp	AimPacketBuild__FfP5ActorP9AimPacket
0x802e1ffc	NORM3	asm-lfs	game/u_802DDEDC.cpp	AimPacketBuild__FfP5ActorP9AimPacket
0x802e2790	NORM3	frsp-fwd	game/u_802DDEDC.cpp	AimBehPick__FP5ActorP3Beh
```

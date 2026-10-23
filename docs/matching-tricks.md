# Game-code matching tricks (CodeWarrior GC/1.3.2, cflags_game)

Collected by helper-game-a and helper-game-b (2026-10-11 to 10-15). Try these first on stuck diffs.
Tools named here live in ~/geist-work/ (helper-game-a/var2.py + cdiff.py brute-force declaration orders
without touching src; helper-game-b/stackperm.py brute-forces stack slot orders).

## helper-game-b

Geist game-code matching (CodeWarrior GC/1.3.2, cflags_game). Non-obvious source shapes that reproduced retail:
- Actor files: descriptor {0, name, msg, update, init, 0, destroy, ...} in .data; msg handlers take 4 args (actor, msg, data, arg).
- Each actor file's string pool starts with the header table "POSSESS..SCAN"; recreate with an unused `static const char* sActionNames[]`.
- Name hash: inline with seed param `StrHash(s, h)` and index loop `for (i=0; s[i]; i++) h = h*31 + (u8)toupper(s[i])` gives the retail two-pointer loop; `h %= 0xFFFFFFFF; return h;` vs `return h % ...` changes the result register.
- Inline message-case helpers returning int (result in r0, then `mr r3,r0`); a single-case `switch` gives `cmpw; beq; b`.
- `cond ? 1 : 0` gives neg/or/srwi; `(int)(x >> 24) == 2` for cmpwi.
- String class: Buffer base (virtual dtor 0x8028B9C0 out of line), String : Buffer; String wrapped as first member of a struct inside a msg gives the retail double null-check in the inlined dtor.
- Locals in different case blocks share stack slots; retail often declares msg structs at function scope (set frame size accordingly).
- `a + (b->size | 0) + 32` keeps the a + (b+32) add order (CW otherwise reassociates).
- Stack slots of aggregate locals depend on declaration order in a non-obvious way; ~/geist-work/helper-game-b/stackperm.py brute-forces orders (and object sizes, from gaps) to hit retail offsets.
- Reusing a variable (`bone *= 28`) or splitting a call result into a temp (`int name = f(); g(name)`) fixes register choices.
- Unsolved pattern: `cmplwi r4,0` then parameter copies inside the if (instead of `mr.`).


## helper-game-a

Learned while matching game code 0x80005800-0x80150000 (helper-game-a, 2026-10-11/12):
- A file's static arrays are addressed from one anchor symbol (asm shows `lbl_X+0x28C`): write separate `static` arrays, not one struct.
- Volatile/callee register ranking changes with: copying a param into a local declared first (`u8* p = data; int n = size;`), declaring a loop var in its own `else` block (`int j`), or moving a loop into an `inline` helper (CRC table builder).
- Inline walk helpers matched only as `{ int i; T* n; n = list; i = index; while (...) }`.
- Retail repeated tails come from nested if/else with a copy of the tail in each branch, not `continue`.
- Single-case `switch` gives `cmpw; beq; b` (used for TPL format checks).
- Brute force declaration orders: ~/geist-work/helper-game-a/var2.py + cdiff.py (compile-only, never touches src; safe in background). mwprobe/var.py-style runners that write src get clobbered by their final restore.
- When renaming symbols from `objdump -t`, sort by offset first; scrambled names showed up as "dead-stripped" DOL size mismatches.
- Callee-saved ranking off in a caller of an out-param inline: wrap the lookup and type check in a second inline (`return VarFloat(0, GameEvent(..), var)`), declaring `def` before `obj`.
- `return group->objs[i].values[x]` swapped regs; `VarObj* obj = &group->objs[i]; return obj->values[x];` matched.
- Switch on the assignment (`switch (sMode = mode)`) gave the unfolded `li r0,0; cmpwi r0,1` after inlining a constant.
- A second loop over the same range needed its own counter variable (`m`), not a reused `j`.
- Retail reloading globals just stored (no forwarding): read them with `*(T* volatile*)&g`. Declaring the globals volatile instead reorders stores in other functions.
- cdiff/var2 show sda21 relocs as `0(0)`, so they can't see WHICH global is accessed. Always confirm with the DOL check (or `objdump -dr` relocs).
- Flat `a*16 + b*16 + c*4` matched; parenthesizing let CW fold the sizeof terms differently.
- Random-range floats `lo + (hi-lo)*rand()/32767` whose loads come after the rand() call matched as `inline float Lerp(float lo, float hi, float t) { float d = hi - lo; return lo + d * t; }`, called with rand last (args are evaluated right to left). A macro or plain inline hoisted the loads into f29-f31.
- MSL string hash: reuse StrHash from CollectionGhost.cpp. When a `li rX,0` null result shares a register with the hash, use the `if (s == 0) h = 0; else { h = key; ... }` form.
- ps_rsqrte/ps_res sqrt with retail `stfs x,0xc / frsp / stfs y,0x8`: statement asm on MEMORY operands (`lfs f0,x; ps_rsqrte f0,f0; ps_res f0,f0; stfs f0,y`); CW forwards the store/load. Register-qualified asm vars give no stack traffic.
- A loop that matched only inside an inline helper (`StrUpper(dst, src)`) after declaration orders failed; LogoShow-style repeated wait loops must reuse the function's `i` (an inline helper adds a new counter register).
- An aggregate literal declared inside a loop (`Vec3 pos = {0,0,0};`) gives retail's per-iteration lwz copy from .rodata; a file-scope const struct gets hoisted lfs.
- Pre-stwu param loads (G4 2.0 fingerprint) did NOT reproduce on 2.0 for branchy functions (u_800E0CB8), only on branch-free ones.
- Field loads in the wrong order before an inlined call: CW evaluates call arguments right to left, so permute the inline helper's PARAMETER order (brute force 5! with var2) until the loads line up.
- Index vs i*size offset register swap (`lhax rX,base,off`) or a loop counter/pointer swap: move that code into an inline helper that takes the index as a parameter (TimerFire(int i), TimerReset()). This fixed what declaration orders couldn't.
- Store forms: `sTimers[i].x = ...` vs `Timer* t = &sTimers[i]; t->x = ...` changes which register holds the element address.
- Calls to a function with fewer args than another call site (r6 left unset) = unprototyped C call; C++ can't produce it (Audio.c, asked the Lead).


## DeepSeek workers (2026-10-18)
- "[INVENT]"-style string hashes in pickup code need a one-argument `StrHash(s)` with a local `h = 0`. That gives
  the null path and the loop path their own `li 0`. The two-argument (seeded) form merges them and makes the
  function 4 bytes short (u_801A1D24, ds-3).
- Spin angles handled by the 0x8009F190/0x8009F028 helpers are 12-bit fixed point. Use an IVec3 and
  `angle.z += (int)dt` (an integer add on the raw word), not floats (u_801A1D24 PickupUpdate).
- Address-taken result vectors (`float r[3]`) and function-scope message structs set the stack layout. Declaring a
  struct in a block instead of at function scope moves it to a different slot (u_80243B0C DashAim).
- Direct expressions like `gTable[w->state].fn(w)` vs `T* n = &gTable[w->state]; n->fn(w)` pick different
  addressing modes. Try both (u_80160188, ds-2).

## opus-1 (2026-10-18): register ranking of parameters, locals and inline values
CW 1.3.2 colours callee-saved registers in a rank order (first ranked gets r31). Observed ranks:
values produced inside an inline (its locals and its return value) rank first, then the caller's
locals in declaration order, then the parameters (last parameter first). A local that is just a copy
of a parameter of the same type is merged into the parameter and keeps the parameter's rank.
- A pointer parameter that retail keeps in r31 ahead of the locals: declare the parameter `void*` and
  cast it to a typed local (`int F(int action, void* p) { Actor* actor = (Actor*)p; ...`). The cast
  makes a real local that ranks with the locals (HostAction, WeaponMsg, Credits NoBotUpdate/State0Update).
- An inline's returned value (`int r1 = Query(...)`) outranks everything. If retail ranks it after a
  local, return it through an out pointer instead: `Query(&r1, ...)` (HostAction, WeaponMsg).
- The reverse: a plain caller local that retail ranks first can come from an inline's return value
  (PickupLoad: `tag = (u32*)StreamTake(s, 4)`, a helper that advances the stream).
- The lazy type-name hash (ToolCreate etc.) is solved; see "The lazy type-name hash" below.

## Loop counter vs. pointer walker register swap: move the loop into an inline helper
When retail keeps a loop's counter and its pointer walker in each other's registers (or adds/drops a
`mr` copy of the base), moving the loop into a small `static inline` helper that takes the array and the
count as parameters often fixes it, because inline-produced values rank first for callee-saved registers.
Fixed GrenadeRemoveAll (`EmitterSlotsClear(gGrenadeEmitters, 8)`) and the scan loop of PickupScanMsg.
Related: an `inline float fabsf(float x) { return __fabsf(x); }` wrapper ranks float registers differently
from a direct `__fabsf` call (RideActionAnimate), and a `struct { float v[4]; }` copies word-wise with
`lwzu` where four named floats copy with `lfs` (GrenadeUpdate).

## More register fixes from opus-1 (2026-10-21)
- A block moved into its own `static inline` helper fixed a register again (PickupScanMsg: the host setup
  became `HostSetup(target)`).
- A ternary min `n = (m < k - n) ? m : k - n;` instead of an if-min fixed a register reuse (ToolMsg).
- Reusing an existing counter instead of a new local fixed one (QueueForEach: `e = q.write; e -= q.size;`).
- Actor units whose split lists only .text may need their type descriptor in a `.data` split before they
  link as Matching (u_801A1D24's gPickupType).
- Variant runners: wibo's file names are case-insensitive, so never use variant names that differ only in case.

## The zero-copy idiom (`li rA,0; mr rB,rA`): solved by opus-1, 2026-10-21
CW 1.3.2 merges zero-initialised locals into a register that already holds zero (`li rA,0; mr rB,rA`) only
when those locals belong to an **inline**; a caller's own locals always get separate `li` instructions,
however they are written. So retail li/mr means both zero-initialised locals live in an inline helper.
Evidence: FileReadStream (an inline in File.cpp, `void* buf = 0; u32 size = 0;`) and GrenadeRemoveAll's
hoisted zero; isolated probes give li/li in a plain function and li/mr in an `inline`.
Side effect: an inline's locals take callee-saved registers in reverse declaration order, so permute the
declarations after wrapping code in an inline. SaveRestoreBlocks matched this way
(`static inline void SaveSendBlocks(SaveSlot*)`, `int* hdr; u8* blocks; int i; int offset;`, `offset = 0; i = 0;`).
- Refinement (opus-1 scan): a plain `for (i = 0; ...) a[i]` loop inside an inline already gives `li i,0;
  mr off,i` (the compiler-made offset copies i); use plain indexing in the inline, not an explicit offset.
- Outside inlines the opposite ranking holds: declaring `int i;` before `int words` matched seven Bitset
  helpers in u_80022B10.

## The lazy type-name hash: solved by opus-1, 2026-10-21
Retail (ToolCreate, SpiritGunCreate, DogMeleeCreate, HomingMsg and ~22 more effect Create/Msg
functions) keeps the type pointer and the name pointer ahead of the hash (`s r4, type r5, h r6, c r7`);
our shapes always gave `h r4`. Three things are needed together, none works alone:
```
for (i = 0; s[i] != 0; i++) {
    h *= 31;                                    // two statements, not h = h * 31 + ...
    h += (unsigned char)toupper(s[i]);
}
inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;          // name copied into a local first
        type->hash = StrHash(name);
    }
    return type->hash;
}
void* ToolCreate(int a, int id) {
    EffectType* type = &gToolType;              // a local type pointer, not &gToolType in the call
    Tool* t;
    EffectTypeHash(type);
```
The StrHash null-check form (return 0 / shared h) does not matter. Findings on the way:
- `h = h * 31 + x` makes a temp for `h * 31` that is ranked like an inline value and pulls h to the
  front; `h *= 31; h += x` keeps h a plain variable.
- Volatile ranking seen in 1.3.2: an inline's locals rank before its parameters, inline parameters
  rank outer-first, the CSE'd `*s` byte ranks after them, and caller locals rank last (unless they
  are temps). The walked pointer is never ranked by declaration order.
- DashOnEvent and RideMsg (u_802B9088) use the seeded `StrHash(s, seed)` with the result in a
  callee-saved register. Retail accumulates directly in that register (the caller's variable), which
  `inline void StrHash(u32& h, const char* s)` + `h *= 31` reproduces; the callee-saved ranking is
  still off there.
- HomingMsg: the 0x400 query result is a 32-byte struct (`MsgResult`, as in ThermoBeh), and the
  target velocity add is `float x = v->x; ... x *= 1000.0f; ... info.vel.x += x;`.

## State-name String blocks and actor loads in message handlers (opus-1, 2026-10-21)
- A message handler that builds String locals for a state-name reply, with the String stack slots in
  ascending order (name 12, newline 32, result 52), needs that block as a `static inline` helper
  (`HostBehStateName(Buffer* out, const char* s)`): inline locals get ascending stack slots. Fixed HostBehMsg
  and HostRideCMsg, then HostRideDMsg. Only when retail's String slots ascend: PossessHostMsg's descend,
  and there the block stays in the outer function.
- If a handler keeps the actor in r6 with an `mr r3` copy, make that case an inline that loads the actor
  from `data` itself (HostBehMsg's 0x41E query).
- A `cond == 0 ? 1 : x` ternary gave retail's branch polarity where if/else shapes did not (HostBehState2Update).
- Buffer/String: matching files declare `virtual ~Buffer()` so the vptr is at 0.

## Effect Create functions (opus-1, 2026-10-22)
The lazy name-hash recipe matched 18 more `XCreate(int, int)` functions (Pistol, Sniper, Rpg, ...). The extern
type descriptor must be declared with the full 0x30-byte EffectType: an 8-byte struct gets addressed as small
data and breaks the match. Generator: ~/geist-work/ds/opus-1-tools/cgen.py.

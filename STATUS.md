# STATUS

Working log and work queue for the matching decompilation, newest section first. See README.md for setup and
CONTRIBUTING.md for how to help. **The Opus queue section below is the list of partly matched units, the
functions still left in each, and what was already tried.**

Note for contributors: this log was written by the maintainer's Lead session and its helper agents. Paths under
`~/geist-work/...` are the maintainer's private scripts and notes, not part of this repository. Their public
equivalents are in `tools/matching/`: `mdiff.py`, `udiff.py`, `variants.py` (variant runner), `audit.sh`,
`linked.py` and `scan_fakes.sh`. "Ghidra pseudocode" means the maintainer's local study copy, which is never
published; generate your own from your copy of the game if you want it.

## Team board (Decomp Lead + DeepSeek workers) — game code, from 2026-10-18

| Who | Worktree / branch | Assignment | State |
|---|---|---|---|
| Lead (Opus, main session) | `geist-matching` / `main` | Coordinates; audits and merges every worker result; compiler/flag questions, unit-boundary and data-split changes, anything needing Dolphin; Credits.cpp; the **Opus queue** below | coordinating |
| ds-1 (DeepSeek Flash) | `~/geist-work/ds/ds-1` / `ds/ds-1` | retired 2026-10-27 (user): Sonnet drafts untouched units instead | retired |
| ds-2 (DeepSeek Flash) | `~/geist-work/ds/ds-2` / `ds/ds-2` | retired 2026-10-27 (user): Sonnet drafts untouched units instead | retired |
| ds-3 (DeepSeek Flash) | `~/geist-work/ds/ds-3` / `ds/ds-3` | retired 2026-10-27 (user): Sonnet drafts untouched units instead | retired |
| opus-1 (Opus 5.5 agent) | `~/geist-work/ds/opus-1` / `ds/opus-1` | near-miss sweep 4 done 2026-10-27: +22 functions (1,724 -> 1,746) | idle |
| sonnet-1 (Sonnet 5.5 agent) | `~/geist-work/ds/sonnet-1` / `ds/sonnet-1` | batch 3 done: six more untouched units drafted (+82 functions, 1,871 -> 1,953); batch 4 running | running |

DeepSeek budget: **$10/day** (user, 2026-10-20). `run_worker.sh` runs a 30-turn draft pass first when the unit has no source yet.
Workers are headless Claude Code on **deepseek-flash** (DeepSeek-V4.1-Flash-1017, thinking on; `~/.local/bin/claude-deepseek`).
They are started with `~/geist-work/ds/run_worker.sh NAME [MAX_TURNS]` (allowlist + 150-turn cap), at most 3 at once.
You can watch one live with `python3 ~/geist-work/ds/ds_watch.py NAME -f`; `ds-stop` stops all DeepSeek workers. Before
every merge commit, `~/geist-work/ds/lead/audit.sh [UNITS]` must print AUDIT OK.
**Round 1 (2026-10-18, V4 Pro, no turn cap):** about $6 for three workers, roughly 300-400 turns each.
- ds-1: u_8022ED88 matched and merged; u_80285014 reached 4/6.
- ds-2: u_80161F40 was byte-identical, but only through a hand-built string blob and mangled-name calls. Rejected;
  the Lead rewrote it as real source (the behaviour registry).
- ds-3: u_802A770C reached 4/6.
Lesson: do data splits before handing out a unit, and reject hacks.
**From 2026-10-18 the workers triage first:** draft every function, then skip register-only diffs at once and
anything stuck after 8 tries. Those go to the Opus agent, which may brute-force source shapes (never compilers
or flags). Its results get the same audit. They have read-only Ghidra, WebSearch and WebFetch only (no REA, no Dolphin, no memory). Each gets
a self-contained prompt from `~/geist-work/ds/task_template.md`, with the rules, commands and a skip-and-hand-off
rule. Their results are in `~/geist-work/ds/<name>/worker-result.json`. A worker never commits; the Lead audits
each result (rebuild, DOL SHA-1 = build.sha1, unit Matching and linked from its own src object), then commits and
merges it.
Borrowed code that workers report goes into the README credits.

**Late parameter copy and parameter-last rotation: solved (2026-10-18/19, opus-1 + Lead).**
CW 1.3.2's callee-saved ranking (rank 1 gets r31), worked out by opus-1:
1. values produced inside an inline (its locals and its return value);
2. the caller's locals, in declaration order;
3. the parameters, last parameter first.
A local that is only a same-type copy of a parameter is merged into it and keeps the parameter's rank.

Retail's behaviour and message callbacks take **`void*` parameters** and cast them to typed locals, inside
`if (beh) {...}` or after `if (!beh) return 0;`. Calls through state tables pass `(owner, beh)` straight on.
Queries use out-parameter inlines: `Query(&r, ...)`.
- `~/geist-work/ds/lead/voidify.py FILE REGEX` rewrites callbacks into this form.
- Results:
  - Credits was **three files**, all Matching (NoBotState4Update solved by opus-1 with an inline QuerySend):
    - **Credits.cpp**: 0x802E3674-0x802E3AE4, the string pool, gCreditsScreen, sCredits, 3 .sdata words.
    - **CreditsTip.cpp**: 0x802E3AE4-0x802E3EF8, the gTip* .sdata, .sbss, its own 0.0f.
    - **NoBot.cpp**: 0x802E3EF8-0x802E4858, gNoBotStates, its own 0.0f and the conversion double.
    - Clues: the same constant twice in .sdata2, and zero words in .sdata, which are file-boundary padding.
      CW puts `= 0` globals in .sbss and merges named float constants with literals (tested), so neither explains them.
  - Thermostat was **two files**, now both Matching:
    - **ThermoBeh.cpp**: 0x802665A8-0x80266FAC, .data gThermoStates, .sdata2 0x804DA0A8-B8.
    - **Thermostat.cpp**: the gauge actor, 0x80266FAC-0x80267558, its pool, types + jump table, .sdata2 0x804DA0B8-D8.
    - Clue: two 0.0 constants with a 4-byte alignment gap between them. A file never holds the same constant twice,
      so a repeated constant marks the boundary between two files.
  - u_802A770C, u_80160188, PickupLoad (u_801A1D24): Matching.
- The callbacks in u_80161F40's gBehTypes are typed `(void*, void*)`.

**Opus queue** (handed-off functions, plus anything a worker failed twice; the Lead finishes these):
u_80175D60 58/137: GunAimStateSwitch80176D4C 1, fn_80175EE4/fn_80178D4C/fn_8017AB10 2
u_800886C8 23/70 (ds-1 draft, screen FX): FxScreenRecordCopyMode 6
u_801A6094 12/49 (ds-2 draft, AI state machine): ActorGetFleeState 3, DogActorCancelFollow 11; fn_801AA570 (0x838) TODO
u_802BB1F4 8/35 (ds-1 draft, FshSve states): ZoneCheckOwner 2, FollowTargetReset 17, ClearHeld 19
u_802AE27C 4/32 (ds-2 draft, BehParams/ghost actor): GhostStateCheck 5, StateChangeMsg 8, ResetActorVelocityAndCondition 11
u_801EAACC 6/33 (ds-3 draft, text rendering/player mode): PlayerSetupGunAction 2, PlayerModeUpdate 3, TextDrawBeginFrame 6
u_8020D47C 16/37 (ds-2 draft, possession select): EnterPossessed 1, ResetGun 2, CollisionUpdate 3, SelectSlotUpdate 4, GunUpdate 6
u_802C01E8 6/25 (ds-1 draft, possessed-actor status): fn_802C301C 1
u_801AADA8 7/29 (ds-3 draft, FSHSVE actor): TargetBlocksFollow 3, ResetRangeState 5, StateTwoEnter 5, FollowAbortState 6
u_8023FA5C 20/52 (ds-2 draft, large actor): LargeActorResetFlags 3
u_801A28A4 15/44 (ds-1 draft, possession state bits/follow slots): EnableStateBits 8, FollowSlotEnter 8
u_802CBB44 6/32 (ds-3 draft, Stone Men): CollisionSplashFx 6, StateUpdate 8, SetPendingState 11
u_8027D3EC 4/15 (ds-1 draft, possessed-actor effects): PossessedEffectQuery 16, PossessedEffectCreate 98
u_80125DE4 1/9 (ds-2 draft, pickup key actor): ProbeHit 1, UpdatePickupKey 42; weak QueryMsg/QueryResult/String dtors are emitted between Update and Message
u_8024E5D0 9/33 (ds-3 draft, host-ride behaviour states): EnterRideState 2, UpdateTypeCheckedState 10
u_801EDE74 9/41 (ds-1 draft, held-slot/gun aim/item): HeldSlotField44 4, HeldSlotCallback0 5, HeldSlotDispatchCallback 7
u_80136884 23/53 (ds-2 draft, player gun): HeldTypeValue 4, HeldTypeCallback 4, HeldTypeBegin 8
u_802D4D18 4/19 (ds-3 draft, host ride): RideStartHandler 1, RefreshStreamState 7; RideHostUpdate (0x924) TODO, the DeepSeek draft did not compile
u_80290C58 8/43 (ds-1 draft, host follow/carrier): OffsetResetEnter 2, FollowBeginState 3, OffsetStateEnter 4, CarrierReleaseState 5, AccumLinkDirection 8
u_8023824C 4/22 (ds-2 draft, follow marker/behaviour): BehaviourRelease 8, StateTimerUpdate 35
u_802D8C7C 8/27 (ds-3 draft, owner follow states): FollowStateUpdate 1, HasFollowCondition 1, FollowStateEnterClear 2
u_802052B4 1/14 (ds-1 draft, turret actor): TurretDestroy 14, GunStateMsg 68
u_80202AB8 0/13 (ds-3 draft, host actor possession): HostActorInit 5, BeginTimerDone 47, HoldTimerDone 49, TimerDone 62
u_801E7DE0 7/35 (ds-2 draft, player weapon/aim states): Ea620StateEnter 1, WeaponStateId 4, WeaponStateNotify 4, CollisionViewUpdate/StateUpdate 6, AimStateUpdate 25
u_8028865C 1/19 (ds-1 draft, clock pages/particles): EmitParticlePageFromInts 7, PageBufferDestroy 9 (hand-written deleting dtor; should become a real class dtor), DrawParticleSlots 11, fn_8028A728 15
u_802C9818 untouched: DeepSeek draft stalled on its first two functions (0x504, 0x7EC); left for Opus
u_8018FAE8 2/12 (ds-3 draft, ghost hint/scoreboard): Reset 55, Input 62; float-constant order and register diffs
u_8029C0AC 7/33 (ds-2 draft, NPC Sve follow): ResetEvent 8, BeginTargetFollow 9, ClearEntryRange 11, BeginPathFollow 13, StopFollow/CancelFollow 14 diffs
u_8020B77C 7/15 (ds-3 draft, FX emitter): Start 64, StartTextOut 64, SetAsset 86; StateUpdate 553
u_8024CAA0 0/8 (ds-1 draft, camera rail/scannable beam): RailFireloopDestroy 22, ScannableBeamSetParam 44; RailMove 601
u_8028A77C 17/47 (ds-1 draft, Buffer/String/ScreenEffectSlot): ScreenEffectSlotQueueCopy 3, GeistCommMessage 10, fn_8028B2D4 (slot Flush) 15 diffs; fn_8028BAE0 not named yet
u_802A815C 3/10 (ds-2 draft, Actor62 path actor): Dispatch 10, CopyState 38, DrawPathTrail 45; UpdatePath 615
u_802B7120 5/17 (ds-3 draft): ActorShouldCollect802B84C8 1, ActorGetSelectedId802B85D0 5 diffs; MessageHandler (0x5F0) and DamageHandler (0x2CC) large
u_80208238 9/38 (ds-2 draft, Batsqueeks host): near misses SoundListStopAll 2, SoundListTeardown 2, GetModeMotionId 4, RunModeCallback 4, ApplySourceMode 4, RunModeHook04 6
u_80116B28 3/13 (ds-3 draft): QueryTypeId 3, fn_80117FB4 14, GetTypeName 14 diffs; fn_801174B0 521, fn_80116E30 384
u_802B3430 1/5 (ds-1 draft, clock actor): Msg 88, Init 115, UpdateHands 184, Tick 619 diffs
u_801E0EDC 3/17 (ds-1 draft): near misses fn_801E1354 1, fn_801E13A0 1, fn_801E10D8 2, fn_801E0FD0 3, fn_801E1150/11C0 6, fn_801E1420 8; Barrier actor functions 88-430
u_80210D80 4/21 (ds-3 draft): closest MarkLookupEntryFlag 7, GetLookupEntryValue 18, SetParticleDrawTransform 21, fn_80211E38 23 diffs
u_8003B98C 0/11 (ds-2 draft): closest fn_8003BA70 8, SetHandleEntryValue 13, StreamHandleFree 17 diffs
u_8028703C 8/22 (ds-1 draft): 5 TODO functions incl. fn_8028787C (0x608), fn_80287EB8 (0x32C); fn_8028757C should inline the sprintf/halt body
u_800B1834 38/94 (ds-3 draft, 3 runs): 19 TODO; register-rotation and branch-layout diffs noted in commit history
u_8020A650 1/4 (ds-2 draft): BuildModel 814 diffs (0xCDC bytes), DestroyVtObject 24, InitNameTables
u_802C8900 0/5 (ds-1 draft): all five functions 80-396 diffs; ActorFxEmitterUpdate's String/SendMsg(0x41D) else-branch layout is approximated
u_800A6F18 12/64 (ds-2 draft): 14 TODO functions
u_8009DE94 14/90 (ds-1 draft): 4 TODO; 42 shared math/AI helpers keep fn_ names (descriptive names in comments) until callers are relinked
u_8006C604 19/61 (ds-3 draft): 9 TODO functions incl. fn_80074018 (0xD50), fn_80074D68 (0x7F8); PlayerUpdateGunMotion stack-buffer layout for ActorGetMatrix needs review
u_80120D98 12/49 (ds-1 draft): HazardTargetAimDir TODO; HazardTrackTarget 155, TrackListUpdate 137; Clear vtable slot vs base class; lbl_8039EED8 needs a struct type (0x14C entries)
u_8011818C 15/49 (ds-2 draft): fn_8011F66C (0x660) and fn_8011FCEC (0x674) not drafted; ItemSetupNearest/StatusEvent etc. have large diffs
- u_800D32E8 16/50 (emitters/pads, ds-3 3 runs): 34 left.
- sonnet-1 batch 3: u_802DDEDC CardGuide/aim 10/30 (card code to 0x802E14B0, aim behaviour from 0x802E1D84: candidate split; defines gCardMsgs .data 0x803C57F0 0xAF0, CRC table .bss 0x804A4AA8 0x400, statics .sbss 0x804D5828-0x804D587C), u_802A9824 Ally 17/34 (gAllyStates 0x803C40B8; AllyEnterFollow/AllyEnterState3 r30/r31), u_80231550 shards 5/32 (14 PtrArray template members not drafted: need the container base's real virtual layout, vtables 0x803BD9E8-0x803BDA54), u_8028009C Extras menu 10/35 (gLevelGates .rodata 0x8034B0D8; unexplained .ctors entry 0x80341240), u_80222C08 Rivet 11/37 (starts with the Bees unit's BeeBase deleting dtor fn_80222C08: boundary to move), u_800F73C8 29/107 (really several files: streams, emitters, cameras, team pads, minigame; 12 array members + 0x800FB630, 0x800FBE88 not drafted). Cheap leads: StreamFinished/Stop/Prepared 1 each, RivetPlayerBusy 2, PlayerHangEnter 2, CameraSetFov 8.
- sonnet-1 batch 2 (2026-10-27): u_80248FC8 Cord 38/52, u_801F2DA4 Fear 9/28, u_80244728 Foe 14/26, u_80112308 Gfx lighting 9/46 (large functions not drafted: 0x80112AB0 GfxSetMaterial, 0x80112D84 and ~25 more), u_801F7F48 Crouch 12/39 (0x801FAEE8, 0x801FB320, 0x801FC08C not drafted), u_800F396C Weather 14/35 (0x800F39E0, 0x800F4F0C and ~10 more not drafted; WeatherLatchInit falls through into the next function). Cheap leads: FoeUpdateState1 (r30/r31), FoeLeaveFire 1 (bool return), CordHandlerBasic / CordEnterLedge (one mr), Fear/Crouch Enter* 6-8 (lfs/lwz order). Data for the Lead: Gfx colour globals 0x8039ECA8-0x8039ECD4 via one anchor (r31 = 0x8039ECCC); state tables Cord 0x803BE7D0, Fear 0x803B9B30, Crouch 0x803B9CE8, Foe 0x803BE740 have no .data split. Kept fn_ names (callers elsewhere): fn_800F6CFC, fn_800F6D44, fn_800F6D4C, fn_800F6F34, fn_80115EE0.
- sonnet-1 drafts (2026-10-27, ~22 min): u_8023B96C (Rules*) 1/12: retail addresses .data/.bss through single anchors (r31 = 0x803BE3A8 / 0x803BE310 / 0x804A28A0); the block 0x803BE310-0x803BE540 (two 0x44 var blocks, mode-hash table 0x803BE398, nine 0x18 menu entries 0x803BE3A8, callback table 0x803BE480) must be defined in the unit (Lead data split) before its functions can match. u_8028DA1C (HostDrive*) 11/23: register-only HostDriveWalkA 9 / WalkB 7 (r30/r31), AnimEvent 2, fn_80290B80 2; large: Animate 806, Sweep 502, Msg 370, Update 304. u_802B4600 (Crab*) 12/21: CrabAnimEventA 12 / B 10 (r30/r31), CrabRender 22 (anchor 0x803C42D0, tables 0x803C43A8/4438/444C/4484/4498), UpdateStrafe 1021, Animate 365; HitMsg's offset comments disagree with its layout (line ~181); DeferAdd is called with 3 arguments in retail but its name takes 4.
- sonnet-1 trial (2026-10-27): untouched u_80221100 (Bees) drafted to 5/8 (BeesSetTimeMsg, BeesGetTimeMsg, BeesDestroy, BeesInit, BeeList dtor). Left: fn_80221100 42 (clamp shape), BeesSpawn 23 (TrailDesc/SpawnDesc 4 bytes low; SPIN at pool +71 vs +94), BeesUpdate 1110 (raw draft). Boundary: fn_80222C08 (BeeBase deleting dtor, 0x68) belongs to this file; flipping needs .data (vtables) and .sdata2 splits. Notes: ~/geist-work/ds/sonnet-1-tools/u_80221100.notes.md.
- opus-1 near-miss sweep 4 (2026-10-27, 70-99.9%): 22 functions in 13 commits. AnimBankGetSetCount now takes ints (user approved; __Fiii): u_801A6094 2,896 -> 1,940 diff lines, u_8023FA5C 3,106 -> 3,036, nothing worse; the GeistA9 states' last 5 diffs are the 100.0f/1.0f constants (unit-wide literals made 4 functions +1, so do it per function). Skipped: u_8009DE94 as a whole (unit-wide load/store interleaving, 75/90 differ), HostAction, DashOnEvent, HuntBotMsg, HuntBotPickable, PlayerGunZoomSetupUpdate/UpdateAlt, CollectUpdate/Enter/Msg.
- opus-1 near-miss sweep 3 (2026-10-27, 80-99.9%): 61 functions in 29 commits across 23 units (u_80175D60 +16, u_80290C58 +12). Signature fixes applied by the Lead (user approved): PlayerModelAnimUpdate/PlayerSubModelAnimUpdate take (float dt, void*), fn_800913EC is (int handle, float dt, void* out) in u_8006C604 (20 -> 24/61), FSHSVEFollowAbort takes Actor* (matches). BehPairUpdate80178AF8 with (void*, void*) gave 16 diffs, so it keeps the typed signature and void* copies. Skipped: HostDestroyBehaviour, fn_800A3990, RideInit/RideUpdate, FSHSVEFollowSelectTarget, HostTypeEnter0A/0B, EmitterResolveHandle, VecToAngles, PoolFree, fn_8003BA00, fn_8003C308, u_8016C244 DogFetch/Possess (stack layout).
- opus-1 near-miss sweep 2 (2026-10-27): 33 functions newly matched in 22 commits (patterns appended to docs/matching-tricks.md). Actor62Dispatch matched after the user approved giving Actor62Msg a 4th argument (r6 = 0; symbol now Actor62Msg__FUliPii). Skipped: GhostFlashNotify (stack slot order, 720 orders), ClockEmitParticlePageFromInts (r7/r6), ClockPageBufferDestroy/fn_8028A728 (derived-destructor class model), HostDestroyBehaviour/RailFireloopDestroy (lwz r4 ... mr r31,r4), ModelBoneNodeChain, FollowStateEnterClear, FSHSVEFollowSelectTarget, ItemColorDrawQuad, DrawUpdateScreenScale, HudTimerReset, RangeSyncWords, HydraOnBehaviourHitMessage, PlayerInitGhostFx (needs data).
- opus-1 near-miss sweep (2026-10-27): 56 functions newly matched across 22 units (list: ~/geist-work/ds/nearmiss.txt; patterns added to docs/matching-tricks.md). Skipped (register/scheduling or need real classes): BufferObjectRelease, fn_800B8D00 (derived destructors), HostTypeEnter0A, HostTypeUpdateG, TargetSlotUsable, fn_801E0EDC, FxScreenRecordCopyMode, SplineHermiteEval, GhostSlotCanPossess, PadButtonState, HazardDestroy, SltFileHandler, PlayerGunSetupActionSlot1, PlayerBatsqueekUpdate (string pool missing earlier strings), cb04/hook04 variants (retail keeps r4 live), u_8026A37C virtual calls.
- opus-1 two-left pass (2026-10-27): no unit flipped. Retinal 6/8: RetinalTrigger 430 -> 73 (4c382fb: h/l declared before u1/u2; hi/lo clamps on the -320/gAspectScale expression; left: FPR numbering and a 16-byte-short frame), RetinalMsg 7. HostRideBMsg 15 with the v2 patch (temp coalesces into r4 with fn_80019F88's argument; ~300 variants), HostRideBUpdate 37 (li r4,9 before addi r3; dot-product conversion slot order), HostRideMsg 15 / HostRideTick 187. Not reached: u_80268780, u_8022ED88. Scripts: ~/geist-work/ds/opus-1-tools/h2/.
- opus-1 one-left pass (2026-10-27): **u_802541D4** 7/7 (ImpFireBallMsg: typed `d` at the top, inline takes void*), Lead split .rodata 0x8034A718-0x8034A978, .data 0x803BEE18-0x803BF068, .sdata 0x804D3608-0x804D3620, .sdata2 0x804D9E50-0x804D9E98 and named the shared table gImpFireAssets (was lbl_803BEFB8, also used by u_80251CB8): flipped, AUDIT OK. **u_8007EE68** 14/14 (FontResetNodes inline) but cannot flip yet: its .data is 8-aligned, so sFontDefs lands at 0x80397E90 instead of 0x80397E8C. In retail the font glyph bitmaps before it (lbl_80395278..lbl_80397DE4, about 0x2C14 bytes) belong to this unit; matching needs that data in the object (needs a user decision: it is game data). Still stuck: MirrorObjRegister 24 (~290 variants), SaveCommand 32 (~136), ImpFireUpdate 43, VMem::Init 9.
- opus-1 cheapest-first pass (2026-10-27): flipped **u_8021427C** (fabsf max recipe, switch on node hash), **u_80285ACC** (own .sdata/.sbss variables defined; VMAllocPage goto-done), **u_8023E7E0** (ghost-action name table as real strings with a trailing "": that is retail's extra 0 byte after SCAN; the same fix matched ImpMsg/ImpInit). Still open: u_802541D4 6/7 (ImpFireBallMsg 12: data passed from r5 while a copy lives in r31), u_801A0F2C 22/23 (MirrorObjRegister 24, r29/r30 swap), u_8007EE68 13/14 (fn_8007F0D4 86, zero/node r3/r4), u_80251CB8 34/35 (ImpFireUpdate 43, float scheduling), u_8016C244 11/18 (VecToAngles 9), Retinal 6/8 (RetinalMsg 7; RetinalTrigger ~430, 5 vs 4 saved FPRs), SaveCommand 32 (register-only), HostRideBMsg/HostRideMsg (patch ~/geist-work/ds/opus-1-tools/nd/hrb_temp_split_v2.patch), u_8026A37C (needs real virtual classes, vptr+0x48), u_80268780 (HermitSpitAmmoFire 474, SensorSeeActor 25).
- opus-1 easiest-partials pass (2026-10-27): **u_80080714** (QueuePush: copy loop written out in the caller with a typed `p` local), **u_8026A158** (AmmoRefillMsg: switch + seeded StrHash inline, QueryMsg/AnimMsg structs, gAmmoRefillType descriptor), **u_80268528** (HermitSpitQuery: case blocks in retail order) matched; the Lead split their data (.bss 0x8042D890-0x8042DA90 + .sbss 0x804D4610-0x804D4618; .rodata 0x8034ADD8-0x8034ADF0 + .data 0x803C0360-0x803C0390; .sdata2 0x804DA138-0x804DA13C) and flipped all three (AUDIT OK). Still register-only: StatGetDebug 6 (sStats base r5 vs format r4; 48 print-inline orders tried), VMem::Init 9, SAMReadCallback r28/r29, GhostGunUpdate 9, GunHostState13Enter 21 (a slot inline fixes r29 but addresses a+9924). GunNodeFn1 matches with an extra int parameter passed through to fn1 (user approved the symbol rename to GunNodeFn1__FP5Actorif): **u_800E9418** flipped to Matching (AUDIT OK). SaveCommand (32) and the two-left units not reached.
- opus-1 close1 (2026-10-26): u_800E9418 18/19 (GunNodeFn1 4: retail keeps r4 free, maybe a pass-through int argument on the fn1 type); u_800E0CB8 Vec3 helpers 0/5 exact, all within 5-14 (the 5 pre-stwu loads look out of reach for 1.3.2; GC/2.0 also puts stwu first); fn_800E1098 is declared with the wrong argument order in u_80153C10, u_802D439C, u_80064FD4, u_80243B0C (real: Vec3* out, Vec3* in, float length). u_80251CB8 32/35: ImpMsg 3 and ImpInit 2 are a one-byte string-pool offset (retail has an extra 0 byte after SCAN at lbl_8034A660), ImpFireUpdate 43.
- u_8013A3D4 21/28 (trail buffers / 3D lines, ds-1 2 runs): 7 left.
- u_80153C10 8/22 (ghost possession, ds-2 2 runs): GhostPossessEnd/Tick/Notify, fn_801554E0, GhostPossessAttempt (0x5A4-0xB50 bytes) barely started; frame-size differences.
- opus-1 effects4 (2026-10-26): u_80251CB8 31/35 (ImpMsg 3 and ImpInit 2 only string-pool offsets: define the state table lbl_803BECD8 in source after the data split; ImpAttackUpdate 12; ImpFireUpdate 43, rand() scheduling). Boundaries: actor .rodata 0x8034A660-0x8034A6C8, IMP FIRE pool from 0x8034A6C8 ('IMP FIRE' 0x8034A70F) and .sdata2 0x804D9E20-48; actor .sdata2 0x804D9DD0-0x804D9E18; attack behaviour 0x80252F2C-0x80253614 may be its own file (own 0.0f at 0x804D9E18). FastSqrt with a register temp takes u_800E0CB8 IVecLength 55 -> 7. GunHostAimUpdate 4, SensorSeeActor 25, HermitSpitAmmoFire not attempted.
- u_800D0BF4 16/31 (zones, ds-3 2 runs + resume): 15 left.
- u_80100EB0 6/24 (ds-1 2 runs): 18 left.
- u_800FDDE4 10/26 (HUD notifications, ds-2 2 runs): 16 left (HudNotifySet 202, HudNotifyBar 193, ...).
- opus-1 near-done pass (2026-10-23): HostRideBMsg/HostRideMsg 15 each (the temp now coalesces into r4 with fn_80019F88's argument in case 0x41D where retail keeps mr r4,r5; patch ~/geist-work/ds/opus-1-tools/nd/hrb_temp_split.patch), VecSwapXY/VecSwapXY2 2 at best (retail loads y before x), StatGetDebug 6, VMem::Init 9, SAMReadCallback (r28/r29), GhostGunUpdate 9, GunUpdateState3 4 (~80 max/fabs shapes), GunNodeFn1 4 (maybe an extra parameter on the fn1 pointer type), QueuePush 6. Not reached: Store, SaveCommand, u_80285ACC, CollectMsg.
- opus-1 idioms pass (2026-10-22): HostRideBMsg and HostRideMsg 15 each (retail keeps the actor's CSE temp in r5 and reads f904 through r31; ours r29), CollectMsg 31 (msg/p callee-saved swap). New one-function sources for 18 effect Create functions (u_800E1B5C, u_800EB1B8, u_801920B8, u_801965FC, u_801BF578, u_801E241C, u_80251CB8, u_8025643C, u_80268780, u_80294548, u_802CED5C, u_8013CA38, u_8017D794, u_8015C79C, u_80226DE8, Corona, u_8018C358): they need the rest of each unit drafted, plus data/sdata2 splits, before they can link. Not done: ShotgunCreate and ProxMineCreate (u_80127D80, static-init guard), RocketCreate (u_8015C79C).
- opus-1 partials pass (2026-10-21): u_802766F0 has 21/21 functions matching (ODE solver shapes) but will not link as Matching: its .sdata2 literal pool order differs (retail [1.0f, 0.0f, 0.5, 3.0, 0.0, u32-cvt, 2^32, s32-cvt]; CW pools literals in first-use order, so retail probably had earlier code using 1.0f and an unsigned-to-double conversion first): needs a Lead decision. Left: u_801F1798 HostRideCHandle40A 11 (seeded StrHash, r29), GunUpdateState3 4, GunNodeFn1 4 (maybe a pass-through 2nd argument), ArenaRelease 5 (and. operand order), VMAllocPages 5, ReflectEnd 6, MirrorObjState4/5Update 7. Scripts: ~/geist-work/ds/opus-1-tools/o5/ (allu.sh prints udiff summaries for every NonMatching unit).
- opus-1, name-hash idiom (2026-10-21): CRACKED (docs/matching-tricks.md): needs all three of h *= 31 / h += ... as two statements, a local name copy in EffectTypeHash, and a local type pointer in the caller. About 22 more retail lazy-hash Create/Msg functions should take the recipe (u_800E1B5C, u_800EB1B8, u_80127D80, u_8013CA38, u_8015C79C, u_8017D794, u_8018C358, u_801920B8, u_801965FC, u_801BF578, u_801E241C, u_80226DE8, Corona, u_80251CB8, u_8025643C, u_80268780, u_80294548, u_802CED5C; ~/geist-work/ds/opus-1-tools/o4/lazyscan.py). DashOnEvent/RideMsg are a callee-saved variant (inline void StrHash(u32& h, const char* s) gives the loop shape; r29-r31 ranking still wrong).
- opus-1 zero-copy scan (2026-10-21): 13 functions matched (u_801FC634 FearObjRelease/FearRegionSetup, u_80140A30 HuntBotExit, u_8016A774 SltLoad/SltMsgHandler, u_80022B10 8 Bitset helpers). Hits not yet tried: BitsetAllocIndices 43, HuntBotUpdate 43, SltCreate 130, SolverCholeskyFactor 93, ModelBoneHitSweep 145, PickupBehMsg 177, AnimTrackUpdateScene 183, CollisionInit 201, and larger ones (list in ~/geist-work/ds/opus-1-tools/zscan.out). GeomRegisterClass 14 (20-byte struct copy load order). Notes: SAMNop__Fv (0x800E05A4) is passed (id, arg) by retail, so its signature is probably wrong; u_80022B10 fn_800234A4/fn_800234A8 are missing from our build of the unit. mdiff.py now refuses to diff a stale object after a failed build.
- opus-1, zero-copy pass: SaveCommand 32 diffs, register-only (arg/name r30 vs retail r28, slot r24 vs r27; loop region now exact); Progress LoadingSetFile 1 (wrapping the fade loop in an inline gives the li/mr copy but in r29/r28 instead of r27/r26; something in the image loop decides). Scripts: ~/geist-work/ds/opus-1-tools/o3/.
- opus-1, 2026-10-21 (two-left batch): GhostGunUpdate 9 (scheduling: retail stores msg.actor/actorCC before the param loads); ToolCreate 11 (lazy hash, ~35 more shapes; same idiom unmatched in Homing, SpiritGun, u_80230504, u_802B9088, u_80285ACC); QueuePush 6 (n/src r28/r30; retail ranks src > i > n); StoreSave/StoreInit 7 each (dst vs i*8 offset r27/r29; retail increments i before the offset); SaveRestoreBlocks 1 and SaveCommand 159 share one idiom: retail copies a zero through a register (li r31,0; mr r30,r31) where ours constant-folds. Tooling: wibo is case-insensitive, so variant names differing only in case overwrite each other's .o (older mixed-case variant results may be wrong).
- opus-1, 2026-10-21: u_801A1D24 PickupScanMsg 14 diffs (host block pointer h shares r31 with i; retail ranks h between d and target), u_802D439C GhostGunUpdate 21 (0x407 hit-message field load/store scheduling; idea: build it through an inline with by-value args). New trick: moving a loop into an inline helper fixed the loop counter vs. pointer walker swap (GrenadeRemoveAll, PickupScanMsg).
- Lead, 2026-10-21 (two-left pass): Progress LoadingSetFile (1 diff: retail li/li for j and alpha, ours mr; 15 shapes/types + all 720 local orders: no change); Store StoreSave/StoreInit share the StoreLoadBanner inline swap (dst/i/offset ranking; 24 decl orders, 7 loop shapes: no change); QueueForEach/QueuePush register-only. Fixed two wrong manglings (LoadingActive takes const char*, HostRideAState1Leave void*): Progress and u_801F676C are now one function left each.
- Lead, 2026-10-21 (one-function-left pass): SpiritGunCreate (11, lazy hash; 7 more EffectTypeHash shapes incl. hash inlined by hand: no change), VMem::Init (9, size/zero r3-r4 swap in the inlined VMemList ctor; 7 shapes), StatGetDebug (6, format-string vs table address scheduling; 4 shapes), SAMReadCallback (9, r28/r29 swap between the header pointer and an sdata address; all 24 decl orders): all register/scheduling only. Variant runner: ~/geist-work/ds/lead/var/try.py.
- u_80015B64 21/36 (view frustum, bezier, model bone hit tests; ds-2 2 runs): 15 left; close: ModelBoneNodeChain (8), ViewFrustumBoxTest (20).
- u_80064FD4 11/17 (player movement/pose, ds-3 2 runs): PlayerPoseUpdate (0xEA4 bytes, still an empty body), PlayerGroundMove (459), PlayerModelUpdate (445), PlayerAirMove (360), PlayerStepMove (198), Pose48Copy (9) left.
- u_80157730 43/67 (ds-1 3 runs): 24 left; close ones fn_80159A4C, fn_8015A97C, fn_8015BFD8 (4 each), fn_80158BE0 (8), fn_8015A10C (11); biggest fn_8015779C (599).
- u_80150300 46/60 (possession host-type tables, ds-3 2 runs): HostTypeEnter0A (2), HostTypeUpdateG (3), HostTypeUpdateI (6), HostTypeUpdateH (9) are close; PossessHostMsg (86), HostTypeInit (150), HostTypeMsgA/B, HostTypeTrails, HostTypeUpdateF/J/K, HostTypeEnter0B left.
- u_80022B10 27/57 (actor support: links, bitsets, debug draw, CRC32, pools; ds-2 3 runs; file-level auto_inline off on evidence): 30 functions left.
- u_8026A37C 30/41 (ODE collision kernel, ds-1 2 runs; file-level auto_inline off on evidence): six 2-diff vtable wrappers (GeomGetAabb, GeomDestroyGeom, SpaceCollideSelf, SpaceGetNumGeoms, SpaceRemoveGeom, SpaceAddGeom: retail loads the vtbl straight into r12) and SpaceDestroy (2), GeomUserTest (7), GeomRegisterClass (25), SpaceCollidePair (24), CollisionInit (201) left. Public ODE source (collision_kernel.cpp) may help as reference.
- u_800E9418 17/19 (gun actor, ds-1 2 runs): GunUpdateState3 (4, f0/f1 swap on max of two fabsf; tried 6 shapes) and GunNodeFn1 (4, table base r4 vs retail r5; local node pointer makes it 6) left.
- u_8016C244 9/18 (collectible + IVec3 helpers, ds-2 2 runs): CollectUpdate (17), CollectEnter (17), CollectMsg (205), CollectInit (16), VecSwapXY/VecSwapXY2 (4 each), FixedToRad (25), RadToFixed (11), VecToAngles (9) left.
- u_80140A30 1/12 (Fear hunt screen, ds-3 2 runs): everything except one; HuntBotDraw (446), HuntBotReset (425), HuntBotScore (303), HuntBotEnter (226) are the big ones.
- u_8018D2D4 6/10 (ghost shot, ds-1 2 runs): GhostShotUpdateAll (0x1304 bytes, 1210), GhostShotDrawAll (60), GhostShotDrawBeam (189), GhostShotDrawSprite (105) left.
- u_80285ACC 14/19 (dog melee + VM pages, ds-2 2 runs): DogMeleeUpdate (47), DogMeleeMsg (285), DogMeleeCreate (15, lazy type-hash swap), VMAllocPage (87), fn_80286980 (6) left.
- u_801A0F2C 20/23 (mirror object, ds-3 1 run): MirrorObjRegister (24, actor/objId r30/r29 swap), MirrorObjState5Update and State4Update (7 each, f1/f2 swap) left; all register-only.
- u_80160DB0 16/22 (pickup behaviour, ds-1 2 runs + Lead void* fix): PickupBehMsgId, PickupBehGivePickup, PickupBehState7/6/5/4Update, PickupBehMsg (177), PickupBehUpdate (119) left.
- u_8016A774 6/17 (SLT loader, ds-2 2 runs): SltLoad, SltUpdate, SltEffectUpdate, SltCreate, SltLoadData not attempted much; SltMsgHandler, SltUnload, SltRelease, SltFileHandler, SltInit, SltDrawEntry register/scheduling-only. Notes in the file header comments.
- u_802766F0 19/21 (matrix solver, ds-3 2 runs): SolverInvertSymmetric 0x8027678C (182), SolverCholeskyFactor left.
- u_8007EE68 11/14 (font, ds-2 2 runs): fn_8007F0D4, FontCreate, FontRender are left. Notes: `~/geist-work/ds/ds-2.notes.u_8007EE68.md`.
- u_801FC634 4/9 (Fear rivet/region item, ds-1 2 runs): FearRivetSpawn, FearRivetUpdate, FearRivetDraw, FearRegionSetup, FearObjRelease are left (the big msg handler 0xB9C is among them). Notes: `~/geist-work/ds/ds-1.notes.u_801FC634.md`.
- u_8023E7E0 15/19 (host ride D, ds-3 2 runs): HostRideDMsg, HostRideDState3Enter, HostRideDState2Update, HostRideDState2Enter are left.
- u_802DB9D0 15/17 (host behaviour, ds-2 2 runs): HostBehMsg, HostBehState2Update are left. Notes: `~/geist-work/ds/ds-2.notes.u_802DB9D0.md`.
- u_8019FDD0 4/7 (mirror reflection, ds-3 2 runs): ReflectEnd, ReflectRender, ReflectUpdateCamera are left. Notes: `~/geist-work/ds/ds-3.notes.u_8019FDD0.md`.
- u_801F676C 15/16 (host ride A, ds-2 2 runs): HostRideABehavior 0x801F7BA8 is left (12 diffs).
  - At entry, retail loads `m->actor` into r3, uses it (`a->beh`, the store into q, the OwnerId argument), and
    only then copies it to r31. Ours loads straight into r31. This is the same "load then late copy" pattern as
    HostRideMsg; making the body an inline (m, a) helper made it worse (48).
  - Plus a StrHash loop register rotation.
- u_801F1798 12/15 (host ride C, ds-1 2 runs): HostRideCMsg, HostRideCState2Update, HostRideCHandle40A are left. Notes: `~/geist-work/ds/ds-1.notes.u_801F1798.md`.
- u_802DC82C 6/12 (host actor with emitters, "host_die", ds-3 2 runs): HostCreateEmitters, HostUpdate, HostAction,
  HostDie, HostDeathCallback, HostReset are left. Notes: `~/geist-work/ds/ds-3.notes.u_802DC82C.md`.
- u_802D77AC 12/14 and u_8028C54C 12/14 (two sibling "host ride" units, ds-1/ds-2): HostRideMsg in both, plus
  HostRideTick (u_802D77AC) and HostRideUpdate (u_8028C54C). One fix may cover both.
- u_8022ED88 5/7 (Ghost Bee spin actors, ds-1 2 runs): SwarmUpdate, SwarmInit are left. Notes: `~/geist-work/ds/ds-1.notes.u_8022ED88.md`.
- u_80230504 4/7 (extra trail, ds-2 2 runs):
  - ExtraTrailApply 0x80230504 (386 diffs): retail saves f28-f31 (frame 0xB0), ours only f30/f31 (0x90). The
    velocity normalize is probably an inline helper.
  - ExtraTrailUpdate (177): a 2-iteration loop.
  - ExtraTrailAttach.
  - Notes: `~/geist-work/ds/ds-2.notes.u_80230504.md`.
- u_80247EB0 7/10 (Grenade, ds-3, 2 runs):
  - GrenadeSpawn: 4 diffs, register-only.
  - GrenadeUpdate and GrenadeMsg: not attempted by the worker.
  - Notes: `~/geist-work/ds/ds-3.notes.u_80247EB0.md`.
- u_8021427C 10/14 (ghost action, ds-2 run 1): UpdateGhostAction, SetupGhostAction, UpdateGhostActionIdle,
  GhostActionSetActive are left. Notes: `~/geist-work/ds/ds-2.notes.u_8021427C.md`.
- u_802BF114 9/11 (ride action 2, ds-2): RideAction2Update (23 diffs, r30/r31 swap) and RideAction2Animate
  (33 diffs, FP register renumbering). Both are register-only and were sent to opus-1 with their u_802BA078 siblings.
- **u_802B2680 Matching** (opus-1 batch 3). Shapes that worked:
  - single-case switches;
  - a bone pointer taken into a local and re-taken at a later use;
  - extern prototypes with the float argument in the 4th position. fn_8003F15C is declared that way in
    u_802B2680/u_802D439C, but with the float last in u_80085CDC. Both link, since floats go in FPRs either way,
    but the declarations should be reconciled.
- u_802B9088 5/11 (ride actor):

  | Function | Diffs | What's left |
  |---|---|---|
  | RideUpdate | 5 | switch case-block order |
  | RideInit | 5 | the same switch |
  | RideState0Update | 2 | cond register r0 vs r4 |
  | RideState1Update | 2 | same |
  | RideTick | 287 | many small diffs |
  | RideMsg | 235 | String stack slots reversed | Notes in `~/geist-work/ds/ds-1.notes.u_802B9088.md`. u_802BA078 8/11 (ride action):
  RideActionMsg, RideActionUpdate, RideActionAnimate (sent to opus-1).
- u_801A1D24 5/7 (pickup actor, ds-3, 2 runs). Both leftovers are **register-only**:
  - PickupLoad 0x801A1D24 (8 diffs): tag/string/end pointers rotate, ours r8/r6/r7 vs retail r6/r7/r8.
  - PickupScanMsg 0x801A24BC (29 diffs).
  Partial source in main; notes in `~/geist-work/ds/ds-3/worker-notes.md`.
- u_802D439C 2/4 (ghost gun, ds-2; both leftovers are **register-only**):
  - GhostGunUpdate 0x802D443C: 113 diffs. Same instruction multiset as retail; a/r29-r30 roles swap and the
    store schedule follows.
  - GhostGunDraw 0x802D4B78: 72 diffs. A 4-register rotation, plus retail coalesces `fade = alpha = 0`
    (`mr r27,r28`), so ours is 4 bytes longer.
- u_802B2680 6/10 (host-action actor, ds-1 run 1 with the old prompt):
  - Left: HostActionMsg 0x802B2680, HostActionEvent, HostActionEnter, HostActionAnimate (0x6CC).
  - **Boundary:** the descriptor at 0x803C42A0 points at HostActionFree 0x802B33F0 (in this unit) and at
    0x802B3430..0x802B36B4 (in u_802B3430). The two units are probably one file; the Lead fixes that.
- **Done:** u_8022ED88 and u_8022EEEC merged into u_8022ED88.cpp (0x8022ED88-0x802301E0). It has one pool
  (0x80349D88-F0) and two descriptors (Ghost Bee 0x803BD988, Ghost Bee Swarm 0x803BD9B8). It is 3/7 and NonMatching
  until a worker does the four ex-u_8022EEEC functions.
- u_80243B0C 1/4 (Dash actor; failed twice, ds-1 on Flash). DashSendAnim matches; the rest is open:

  | Function | Address | Diffs | What's left |
  |---|---|---|---|
  | DashOnEvent | 0x80243B0C | 21 | Register pair around an inlined StrHash (Animator* r29 / hash r31 in retail); possibly the lazy-hash pattern again |
  | DashUpdate | 0x80243DC8 | 323 | Stack slots: frame 0xF0 vs 0x100 |
  | DashAim | 0x802443C0 | 214 | Callee-saved roles and fctiwz temp slots |

  The partial source is in main, and ds-1's notes are in `~/geist-work/ds/ds-1/worker-notes.md`.
- u_80285014 4/6 (failed twice, ds-1 on V4 Pro then on Flash): ToolMsg 0x80285140 at 9 diffs, ToolCreate
  0x80285564 at 11. Partial source is in main.
  - ToolCreate is the **lazy type-name hash** pattern, also open in SpiritGun and Homing. Retail keeps the
    name pointer in r4 and the hash in r6; ours swaps them.
  - Lead 2026-10-18 tried 9 StrHash shapes (index vs pointer walk, if/else with a shared h, a seed parameter,
    while loop, Credits' StrHashKey) and 4 EffectTypeHash wrappers (a local h, a local name, void, `!hash`).
    The count stayed at 11 for all of them, so the cause is outside the helper.
- **u_802A770C 5/6** and **u_80160188 4/5** hit the same open pattern, the **"parameter-last register
  rotation"**. HostAction (0x802A770C, 61 diffs) and WeaponMsg (0x80160188, 60 diffs) are sibling message
  handlers `(int action, Actor* a)` that make two Query calls. Every opcode, offset and branch matches; only
  the callee-saved registers rotate:

  | | actor/weapon | query 1 | query 2 | action |
  |---|---|---|---|---|
  | retail | r31 | r30 | r29 | r28 |
  | ours | r29 | r31 | r30 | r28 |

  Tried with no effect:
  - local declaration orders, split declarations, perm.py;
  - a parameter copy at the top, middle or end;
  - all 6 Query parameter orders;
  - the switch body as an inline helper with params (actor, r1, r2, action);
  - a reference parameter, a void* alias, a const pointer.

  The Lead fixed HostTypeUpdate (it takes a second argument) and the Actor padding. Partial sources are in main.
- Audio.c AudioMsgHandler (helper-a): matches only when a 3-argument call is made to a function that another
  site calls with 4 (r6 left unset). That's an unprototyped C call, so it's a candidate C-only fingerprint
  under the user's rule.
- Earlier helper leftovers (from helper-game-a and helper-game-b, both archived 2026-10-18):
  - main.cpp 9/11 (LevelSelect, ProgressiveSelect)
  - SpiritGun 5/6, Homing 1/2 (type-name hash register swap)
  - u_800E0CB8 0/5 (pre-stwu loads; no compiler reproduces them)
  - VMem 8/9 (Init)
  - Thermostat 15/20 (late param-copy pattern)
  - GrenadeFx 6/7
  - LaserScope 3/4
  - Retinal 5/8 (the file extends to about 0x8026A0BC; boundary work)
  - Fuel 2/3
  - SAM 12/13 (SAMReadCallback), Store.c 13/15 (StoreSave/StoreInit icon loop), Progress 6/7, Shadow2 10/12,
    u_800448D0 5/8, Arena 2/5, u_800DE0C4 14/16
  - Detail from helper-a's final report: LevelSelect has 4 diffs (loop counter r19 vs r29); ProgressiveSelect has 7;
    QueueForEach has 15 and QueuePush has 6; HomingMsg's frame is also 32 bytes too small (Vec3 temporaries)
- Unstarted lead from helper-a: a camera module inside u_800F73C8 (0x800F9458-0x800FA004: 16 cameras of 0x124
  bytes at 0x8045C770, a shake table at 0x8039CF58). It needs a boundary split first (Lead), then it can go to a worker.

Matching tricks from both helpers: `docs/matching-tricks.md`.

Earlier helpers: helper-msl (MSL/Runtime/TRK) and helper-musyx (MusyX/zlib), both done and merged (2026-10-10); helper-game-a
(0x80005800-0x80150000) and helper-game-b (0x80150000-0x802D0000), wound down and archived 2026-10-18, all work merged.
The rules below were written for those helpers; workers get the same rules in their task prompt.

**Rules for helpers:**
1. Work only in your own worktree and branch. Never edit `main`, never push, never touch `orig/`, game
   installs or other folders. Kill processes only by exact PID.
2. File ownership, so merges stay clean:
   - `configure.py`: only between your own `# ==== helper-<name> ... ====` / `# ==== end helper-<name> ...`
     markers (one libs block and one cflags block each).
   - `config/GITE01/splits.txt` / `symbols.txt`: only entries whose address is inside your range (and
     the data those units own). Don't reorder or rewrite other lines.
   - Sources only under your own dirs: helper-msl -> `src/MSL_C/`, `src/Runtime.PPCEABI.H/`,
     `src/TRK_MINNOW_DOLPHIN/`, `include/MSL_C/`... ; helper-musyx -> `src/musyx/`, `src/zlib/`,
     `include/musyx/`, `include/zlib/`. Don't edit `src/dolphin/`, `include/dolphin/` or `STATUS.md`
     (report to the Lead instead; the Lead keeps this board).
   - Tools: copy what you need from `~/geist-work/mwprobe/` into your own folder
     (`~/geist-work/helper-msl/` or `~/geist-work/helper-musyx/`); don't write into `mwprobe/`.
     `probe.py`, `auto_match.py` and `dol_diff.py` take the repo path from the `GM` env var.
3. Setup in a fresh worktree: `orig/GITE01/` is git-ignored, so symlink it:
   `ln -s "$HOME/Documents/Claude code/geist-matching/orig/GITE01/sys" orig/GITE01/sys` (same for `files`).
4. Every step must keep `build/GITE01/main.dol: OK` (SHA-1 6612f52ce6c50bb831ffddd5264b22973304f288).
   Check units with `objdiff-cli` (`ninja` builds the report). Commit on your branch after each green
   batch; message ends with the Co-Authored-By line used in this repo.
5. Prefer real library source you can reconstruct (MSL/TRK/Runtime from the CodeWarrior GC versions,
   MusyX from public decomps' structure, zlib 1.1.4 is public). Don't run scripts from cloned
   reference repos; read them only. Record the compiler version and flags with evidence.
6. **Game code specifics:** read "G3 — first game file" and "G1"/"G2" above first; `src/game/Memory.cpp` is the
   worked example. Work file by file in address order inside your range. Fix a unit's boundaries in
   splits.txt by hand when matching proves them wrong (only between units inside your range; if a boundary
   with a neighbour outside your range must move, ask the Lead). Do NOT run `tools/gen_game_splits.py`.
   You may give names to functions/objects outside your range only if they are still `fn_`/`lbl_`
   placeholders (callees you need, with mangled names); list them in your report.
7. When done (or blocked), send the Lead a short report: units matched, compiler/flags, anything odd,
   exact verify command + expected output. Then stop.

## G4 — game compiler: GC/1.3.2 default, GC/2.0 per file on evidence (Lead, 2026-10-13; rule approved by the user)

Geist was not built with one compiler for every file. Retail game code has fingerprints of two generations:
- **needs <= 1.3.2:** 0x8001734C plane-from-normal (`tools/g1/g1e.cpp`). Retail reloads n->y/n->z after the
  store to out->x. Every 2.x compiler reuses the earlier loads, including 2.0 with -O2/-O3, -opt
  nocse/nopropagation/noalias/nolifetimes/nodeadstore/noregpromotion.
- **needs >= 2.0:** loads through parameter registers scheduled *before* the `stwu r1` prologue. GC/2.0
  reproduces it on u_8012071C's qsort comparators (0x80120A78/0x80120A9C); 1.3.2 cannot. Control: 0 of ~1100
  functions in the 1.2.5n Dolphin SDK and 1.3.2 MusyX units show it. `python3 tools/compiler_fingerprint.py`
  lists the game functions with it (2026-10-13): u_8008DCDC, u_8009DE94, u_800D32E8, u_800E0CB8 (Vec3
  helpers), u_8012071C, u_8016C244, Msx.cpp, u_80277600. (u_8009DE4C in the output is a stale asm file.)
No single compiler version and flag set produces both, so the original build mixed compilers.

**Rule (approved by the user):** the original main.dol is never changed. A compiler is never chosen just
because it makes a function line up. Each file uses the compiler its retail code shows:
1. The default for game files is **GC/1.3.2** (configure.py game block).
2. A file may use `mw_version="GC/2.0"` on its Object only if
   - its retail code has a fingerprint 1.3.2 cannot produce (today: the pre-stwu load from
     tools/compiler_fingerprint.py; add new fingerprints here only with a control test like the one above);
   - and GC/2.0 then matches the **whole** file.
   Record each such file in the list below with its evidence.
3. Never use 2.0 for a file that matches with 1.3.2, or for a function that is merely hard to match. Try
   source shapes first.
4. GC/2.6 stays ruled out for game code (it also fails the plane function, and nothing needs it over 2.0).

**Fingerprint caveat (helper-a, 2026-10-14):** the pre-stwu load is necessary evidence but not sufficient.
In u_800E0CB8 (Vec3 helpers) every compiler 1.3.2-2.7 puts stwu first, even a minimal zero-vector-test
version. Those functions branch early, unlike the qsort comparators. So u_800E0CB8 stays on 1.3.2,
NonMatching. Something else (flag or source shape) drives that scheduling. A flagged unit only moves to 2.0
when 2.0 actually reproduces it. helper-a also found that u_800E0CB8's sqrt body matches as statement asm
on memory operands (`lfs; ps_rsqrte; ps_res; stfs`); CW's store/load forwarding gives retail's stfs/frsp.

**Library leftovers (Lead, 2026-10-14):** 201/203 SDK files match. Remaining:
- MSL file_io.c fflush: 1 diff. Retail saves r29 but never uses it, testing the `__flush_buffer` result
  directly in r3 (`cmpwi r3,0`); ours keeps the copy (`mr. r29,r3`). Tried every 2.x/3.0 compiler and five
  source shapes (direct test, `!= 0`, assignment in the if, a block-scoped temp, declaration order,
  `register`). Probably a trait of the compiler build Metrowerks used for the prebuilt MSL. Parked.
- EXI2 GDEV main.c gdev_cc_read: down from 45 to 2 diffs. Retail keeps `size`, the loop count and the
  read count in three registers: the loop count is `u32` (the unsigned compare), and the loop uses the
  first copy (`p1 = size; p2 = p1;`). The last 2 diffs are the order of the two `mr` copies. Parked.
- Gecko_ExceptionPPC.cp: **matched** (see below).

**Gecko_ExceptionPPC.cp matched (Lead, 2026-10-14):** the source is pikmin2's
(`~/geist-work/helper-msl/refs/pikmin2`, a sparse read-only clone; nothing in it is run). It compiles to
Geist's code with GC/2.6 plus evidence-based per-file flags:
- `-str nopool`: retail's rodata holds the RTTI names and the "bad_exception"/"exception" strings as
  separate 4-aligned objects (0x8034E5F8-0x8034E690). With pooling, `what()` gets an extra `addi`.
- `-RTTI on`: retail's bad_exception vtable starts with an `__RTTI__` pointer, and the unit owns local
  copies of `__RTTI__` for std::exception and std::bad_exception (.sdata 0x804D3E20).
- `include/Runtime.PPCEABI.H/exception.h`: std::exception is inline in this unit's view. Retail's
  ~bad_exception stores the exception vtable with no call, and the "exception" string at 0x8034E680
  comes from the inline what() (exception.cp still defines both out of line).
- `extab_padding=[0x02, 0x55]` (dtk `extab clean`): one catch(...) record has two padding bytes that
  MWCC leaves uninitialized. This is the standard dtk fix for compiler memory garbage; no code or data
  values are changed.
Static ExPPC_* helpers inlined everywhere are emitted in the object and dead-stripped at link.

**C vs C++ for the game's `.c` files (Lead, 2026-10-14):** the user's rule is that genuine `.c` files
are compiled as C. Tested Audio.c, Store.c and Arena.c both ways (scratch C conversion: `typedef`s for
struct tags, no `extern "C"`). The two languages give identical code for every function, except
`result = x != 0;` in StoreLoad. There C++'s bool temp matches retail, but `x ? 1 : 0` and four other C
forms also match in C. So the game code doesn't discriminate. Counter-evidence: zlib (also `.c`, inside
game code, but a separate GC/2.6 build) **must** be C++. Its deflateCopy uses an out-of-line compiler-generated
`internal_state::operator=` (`__as__14internal_stateFRC14internal_state`) that is in retail, and
deflate/inftrees/trees/infblock compile differently as C. Switching game `.c` units to C also needs the
shared functions they call (SendMsg, MemAlloc/MemFree, FxStart, FileFlushBuffer, Music*) to get C
linkage. That renames those symbols in symbols.txt and every C++ caller. **User decision (2026-10-14): keep `.c`
files on C++ until a file shows code only C can produce; then switch that file to C.**

**zlib/zutil.c matched (Lead, 2026-10-14):** 0x8019FD58-0x8019FDD0 (zcfree, zcalloc), .rodata
0x80348270-0x803482F8, .data 0x803AA950-0x803AA978 (z_errmsg); trees.c .data now ends 0x803AA950 (padding).
It is zlib 1.1.4's zutil.c (from ~/geist-work/helper-musyx/refs) with calloc/free replaced by the game's
MemAlloc/MemFree (`__FILE__`, `__LINE__` 213/220) and K&R definitions made ANSI for the C++ build.
zlibVersion/zError are dead-stripped. The rest of the old game/zutil.c unit (mirror/reflection rendering)
is now game/u_8019FDD0.cpp.

**Files compiled as C (`cflags_game_c`, `-lang=c`; same compiler GC/1.3.2):**
- **Audio.c** (2026-10-18, user approved).
  - Evidence: AudioMsgHandler calls fn_80084540 with 3 arguments (r6 left unset), and SoundSetMsg calls it with 4,
    in the same file. Only an unprototyped C call does both; in C++ it would take a function-pointer cast.
  - The whole file (15/15) matches as C.
  - Counter-evidence, recorded honestly: CodeWarrior lays out .sbss in reverse declaration order in C and in
    forward order in C++. Retail's layout matches C++ with the natural order, or C with the 12 small statics
    declared in reverse. Declaration order is free source shape, so this decides nothing.
  - The shared functions it calls or defines now have **C linkage** everywhere: extern "C" in 37 game files and
    zutil.c, and plain names in symbols.txt. That covers SendMsg, MemAlloc, MemFree, FileFlushBuffer, FxStart,
    Music* (12), Fn8007CEA4 and Audio's own functions.
  - `~/geist-work/ds/lead/clinkage.py NAME...` applies extern "C" across src/game when another .c file needs it.
  - Data splits added: .rodata 0x80343610-18, .bss 0x80426910-0x80426B10, .sbss 0x804D45B0-E0,
    .sdata2 0x804D6770-88. MusicFadeTo is a static helper defined before AudioUpdate, so its 0.0f/1.0f come
    first in .sdata2; the linker strips its unused copy.

- **Store.c stays C++** (checked 2026-10-20). opus-1 reported 15/15 as C, but its test compiled a file that still had
  an `extern "C" {` block, which is an error in C mode, and so it compared a stale object. Converted properly (`~/geist-work/ds/lead/c_ify.py`)
  and compiled as C, it gets StoreSave 144, StoreLoad 30 and StoreInit 88 diffs, all functions shorter than retail.
  As C++ those are 12, 0 and 7. No C-only evidence.

Files on GC/2.0: **u_8012071C** (qsort comparators 0x80120A78/0x80120A9C, pre-stwu loads; 1.3.2 cannot).
History: on 2026-10-13 the Lead briefly switched the whole game block to 2.0 after checking only matched
files. That missed the plane function, so it was reverted.

## G3 — first game file: Memory.cpp matched (Lead, 2026-10-11)

**Result:** `Game Code: ... (1 / 190 files)`, DOL OK. `src/game/Memory.cpp` (0x8009D710-0x8009DE94): heap
wrapper with "Out of Memory <file> <line>" reports, two scratch ring buffers (0x160000/0x40000), and the
"VMem" pool object (0x6C0000 block) whose methods live at 0x801BEC4C-0x801BF5xx.
**Settled for game code:**
- GC/1.3.2 with cflags_game works (`-lang=c++ -O4,p -inline auto -fp_contract on`, string pooling).
  **No deferred inlining:** functions come out in source order (unlike the GC/2.6 libraries), and small
  functions defined earlier in the file are auto-inlined (MemAlloc/MemFree inlined into MemInit/MemShutdown).
- A C++ file ends after its `__sinit_*`: CodeWarrior then emits static-destructor helpers and weak inline
  functions (ctors/dtors, e.g. `__dt__4VMemFv`), so those belong to the file.
- String pools keep strings of dead-stripped functions; add an unused function using them, in pool order
  (Memory.cpp: `MemOutOfMemory`).
- symbols.txt needs the **mangled** names (e.g. `MemAlloc__FiPCci`, `Free__4VMemFPv`) for C++ functions,
  including external ones the file calls.
- Typical fixes: the order of declarations and temporaries decides registers; `if (x > 0) {...}` vs
  `if (x < 1)` changes the compare; early `return`s vs a result variable; ternaries in call arguments.
- Tool: `~/geist-work/mwprobe/mdiff.py UNIT FUNC ADDR [SIZE]` diffs one function of a unit against the DOL.
- **asyncio.cpp matched** (0x800E1144-0x800E1B5C; the provisional unit started too early: Vec3 helpers
  0x800E0CB8-0x800E1144 and a ring copy at 0x800E0B54 are separate files, rumble code follows at 0x800E1B5C).
  Buffered async DVD streams with zlib "BILZ" files. Lessons: **`-use_lmw_stmw on`** is required (now in
  cflags_game); repeated blocks are usually **inline helper functions** (AsyncCheckHeader, AsyncQueueNext,
  AsyncDumpBuffers): writing them out by hand gives different registers; `switch` vs if/else matters.
- **Image.cpp matched** (0x800F2B08-0x800F396C, .rodata 0x80345320-58, .sdata2 0x804D7348-90; the stub at
  0x800F2B00 returning 1.0f belongs to the previous file: a file never holds the same float constant twice,
  and 1.0f appears again at 0x804D735C). TPL loading with a local copy of the SDK's `TPLBind` (assert line 337),
  plus screen quads. Lessons: game files can `#include <dolphin/gx.h>` etc. (cflags_game now has
  `-i include/libc`); SDK struct typedefs mangle by their tag (`GXTexObj*` -> `P9_GXTexObj`); quad corners
  passed as float **locals** stay in registers while literals reload; an early `u8 a = alpha;` explains a
  `clrlwi` at entry; `/ 2.0f` compiles to `* 0.5f` but allocates differently; statement order (x0, x1, y0,
  y1) decides float registers. Graphics wrappers named in symbols.txt: GfxLoadTexObj 0x8000F944,
  GfxSetVtxDesc 0x80008DAC, GfxSetCullMode 0x80008B44, GfxFrameEnd/Swap/Begin 0x8000ADDC/0x8000A028/0x80009EE0,
  GfxSetMaterial 0x80112AB0, GfxResetTev 0x8010D130, GfxSetBlendMode 0x801D8A10; data gDVDMutex 0x803D7114,
  gWideScreen 0x804D4008.
- **DataStream.cpp matched** (.text 0x8017BE38-0x8017D794: the provisional end 0x8017D6C4 was too early, the
  StreamQueue dtor, the sMem array-dtor helper and the out-of-line VMem ctor follow `__sinit`; .rodata
  0x80346EF0-40, .sdata 0x804D3008-10, .bss 0x80491880-0x80494A80, .sbss 0x804D5058-A8; next unit renamed
  u_8017D794). Disc streaming: two request queues (binary trees by disc offset, flattened before reading), two
  VMem pools, a 0x2000 bounce buffer. Lessons: **this file needs `#pragma auto_inline off`** (retail calls tiny
  StreamFlushQueues instead of inlining it) while Memory.cpp needs auto-inlining (MemAlloc inlined into
  MemInit), so keep `-inline auto` in cflags_game and use the pragma per file. Explicit `inline` recursive
  functions are expanded 2-3 levels deep into themselves; a tiny `inline` callee defined first is inlined at
  the top levels and called at the deepest one. Field reloads after a branch mean `volatile` (here the current
  request pointer, sPendingBytes, sFileOpen). All the file's .bss is reached from one base register (the first
  static), which confirms which statics belong to the file. A run of string literals in .rodata that nothing
  references (POSSESS, GHOSTACTION, ...) repeats in several files: a header-defined static table.
- **File.cpp matched** (.text 0x800190D4-0x80019A04; .rodata 0x80341E98-0x80341F30, .bss 0x803DAA88-0x803DAE88,
  .sbss 0x804D41A0-B0, .sdata2 0x804D60A0-A8). The provisional unit started at 0x8001598C; everything before
  0x800190D4 touches other data (sdata2 0x804D6030-609C, sbss 0x804D4198) and is now u_8001598C (still several
  files: module hooks called from 0x80087BB0/0x80087FA8 tables etc.). OLC level loader: chunk handler table,
  per-type disc offsets (ArchiveFindOffset), whole-file readers. Lessons: `x = f(); if (x != y)` vs
  `if (f() != y)` swaps cmpw operands; error paths placed after the main path come from `if (ok) {...} else
  {error}` shapes; declaration order decides callee-saved registers, so search it with
  `~/geist-work/mwprobe/perm.py SRC FUNC ADDR SIZE` (declaration lines on stdin). Also new:
  `udiff.py SRC START END` (all functions of a unit) and `adiff.py MDIFF_OUT` (aligned diff that tolerates
  length differences).
- **CollectionHost.cpp matched** (was u_802E2F08: .text 0x802E2F08-0x802E32D0, .rodata 0x8034DC10-88, .data
  0x803C6388-B8, .sdata 0x804D3DC8-D0, .sdata2 0x804DB2E0-F0). Lessons: call arguments are evaluated **right
  to left** (`sprintf(buf, fmt, F2(), G(), F1())` calls F1 first); message structs in different `case`s get
  their own stack slots when declared at function scope (first declared = highest address); a variable used
  in several cases cannot reuse a parameter's register, so give each case its own block-scoped variable.
- **CollectionGhost.cpp matched** ("Collection Ghost Health", .text 0x802E32D0-0x802E3674; the two functions
  at 0x802E3674/0x802E36A4 belong to the next file, which is now u_802E3674: its table at 0x803C63E8 lists them).
  Lesson: calls nested in an argument list run before simple loads of the other arguments, rightmost call
  first, so `F(G(p), H(&x), x)` calls H, then G, then loads x.
- **Credits.cpp WIP 23/30** (was u_802E3674, 0x802E3674-0x802E4858 = end of game code; credits screen, tip
  messages, "No Bot" behaviour; .data ends at 0x803C6468, the jump tables after it are MetroTRK's). Open
  problem shared with helper-b: **`cmplwi r4,0` then parameter copies after the branch** (retail) vs our
  `mr. r31,r4` with copies at entry. Not fixed by any compiler version 1.3-3.0a5, -O2/-O3/-O4,s, -opt
  noschedule/nopropagation/nolifetimes/nodeadcode, inline bodies, block-scoped copies, early return, member
  functions, C mode, -g/-sym, -opt nocse/noloop/noalias, -inline deferred, compilers 1.0-1.2.5n (3.0a3 tests
  first but still copies before the branch). It hits non-leaf `void f(Owner*, Beh*) { if (b) {...} }`
  behaviour callbacks; the int-returning `if (!b) return 0;` ones fuse normally (`mr.`) and match. Other lessons: a query whose String must die before later code goes in its own inline function;
  `if (a == 0 || f() == -1) return 0; return 1;` gives retail's li 0/li 1 shape where `return a && f() != -1`
  gives a bool (clrlwi); a hash written `h = h % 0xFFFFFFFF; return h;` keeps h in r3.

## G2 — game code split into provisional units (Lead, 2026-10-11)

**Result:** `Game Code: 0/192 files`, `SDK Code ... (199 / 202 files)`, DOL OK.
- `tools/gen_game_splits.py` writes `build/tu_proposal.json` into splits.txt (.text only, address order
  next to zlib) and into configure.py between `# ==== game units` markers (lib "game", GC/1.3.2,
  `cflags_game` = cflags_base + `-lang=c++ -str reuse,pool,readonly`; `.c` files get `.c` names). Re-running
  regenerates only the game units. 27 units carry assert-derived names (game/File.cpp, game/DataStream.cpp,
  game/Script.c, game/VM.c, ...); the rest are `game/u_<address>.cpp`.
- Boundaries are provisional: when matching a unit, confirm or move its edges (edit splits.txt by hand,
  and stop regenerating once units are being matched).
- Progress map (`tools/progress_svg.py`) now shows one square per game file.

## G1 — game compiler (Lead, 2026-10-11)

- **Game code is NOT GC/2.6.** zlib (GC/2.6) is a prebuilt library; the game's own code matches only
  **GC/1.3 or GC/1.3.2** (working choice 1.3.2, the same as MusyX). Evidence (`tools/g1/`, probe with
  `-lang=c++ -O4,p -inline auto -fp_contract on`):
  - 0x8001734C plane-from-normal (`g1e.cpp`): matches with 1.2.5n/1.3/1.3.2 only (2.0+ hoist loads over stores).
  - 0x8001598C frame-time from OSGetTick (`g1f.cpp`): matches with 1.3/1.3.2, not 1.2.5n.
  - 0x800185F8 wrapper (`g1.cpp`): matches with all.
  1.3 vs 1.3.2 and the `-use_lmw_stmw` / `-inline deferred` settings are not settled yet (no test function
  tells them apart so far). C++ name mangling is used; `.c` files exist too (assert names).
- `-fp_contract on` (fmadds) and `-O4,p` confirmed.

## G0 progress — game file boundaries (Lead, 2026-10-11)

- `tools/tu_mono.py`: each file's private data (`.sdata2` constants, `.rodata`, jump tables in `.data`) is
  laid out in code order, so a boundary between two functions is only possible if everything private used
  before it lies below everything used after it. Real ranges: `.rodata` 0x80341280-0x8034EE00, `.data` from
  0x8034EE00, `.sdata2` 0x804D5E80-0x804DB338 (above that are weak merged constants such as sqrtf's
  `_half`/`_three`). Addresses used by far-apart functions and 2 trend outliers are ignored.
  Result: **2,414 allowed boundaries** (upper bound); largest forced-together span 130 KB (138 functions).
  Writes `build/tu_mono.json`.
- `tools/tu_pools.py`: **203 string pools** (lis/addi bases into pooled strings). All functions between a
  pool's first and last user are one file. Of 193 gaps between consecutive pools, 37 have exactly one
  allowed boundary (pinned), 21 have none: probably files with both a `.rodata` and a `.data` pool, so pools
  must be paired before "different pool = different file" holds. Writes `build/tu_pools.json`.
- No zero padding between game functions (4-byte alignment), so padding gives no boundary signal.
- Refinements (validated on zlib, whose 9 real boundaries are known): only *compiler-generated* private
  data counts: `.rodata` strings (not named tables like zlib's `_length_code`), `.sdata2` constants, jump
  tables. "Shared" is decided per symbol object (per raw address in `.sdata2`). Vtables are NOT usable
  (weak, shared between files). Now **2,566 allowed boundaries**, all 9 zlib boundaries allowed; pool
  contradictions down to 8 (one area 0x801B4xxx-0x801D7xxx, probably built without string pooling).
- `tools/tu_propose.py` -> `build/tu_proposal.json`: **194 proposed game units** (35 pinned by pools,
  12 known from zlib, 146 guesses at the first allowed boundary; median 0x2694 bytes, largest 0x29A6C).
  Run: `python3 tools/tu_mono.py && python3 tools/tu_pools.py && python3 tools/tu_propose.py`.
- **Validation on library code** (0x802E4858-0x8033F990, all 189 real boundaries known; run with
  `LO=0x802E4858 HI=0x8033F990 NOKNOWN=1`): the order constraint keeps **100%** of real boundaries (safe,
  but in library C code it rules out little). String-pool spans now never cross a real boundary (0/189,
  after fixing a bug that treated `.bss` addresses as strings), so **"same pool = same file" is reliable**.
  The reverse is not: "different pools = boundary" was right in only 2 of 5 pinned cases (some files have
  two string pools), and call-graph placement of the other cuts was right 13/36.
- Also added: static initializers (16/17 agree with the constraint; all 17 used as known file ends), file
  names from assert strings (27 units named, e.g. main.cpp, File.cpp, DataStream.cpp, Image.cpp,
  asyncio.cpp, Memory.cpp, Script.c, VM.c, FearMap2.c, Msx.cpp), and **zutil.c** (zlib) found unsplit at
  0x8019FD58. Static variables (.bss/.sbss/.sdata) were tried and rejected (globals used by neighbouring
  files make them unreliable; `STATICS=1` re-enables).
- Current proposal: **205 units** (29 known, 35 pinned, 140 guessed). Treat cut positions as provisional:
  a cut is only confirmed when the whole file matches. Hard facts usable now: allowed-boundary set (upper
  bound), pool spans (lower bound: never split inside one), static-initializer ends, zlib.
- **Next:** G1 (confirm game flags on small functions), then G2: write provisional ranges as asm units
  (DOL must stay OK; more units than real files is harmless for asm) and fix cuts while matching.

## Game code plan (Lead, 2026-10-11)

**Size:** 7,163 functions, 2.9 MB of `.text` (0x80005800-0x802E4858, zlib already split out inside it).
Mixed C and C++ (assert strings name e.g. main.cpp, AppMain.c, Script.c, VM.c, FearActor.c, FearMap2.c,
Turf.cpp, Cube.cpp, CubeTev.cpp, Image.cpp, File.cpp, DataStream.cpp, asyncio.cpp, Memory.cpp, Msx.cpp,
Shadow2.cpp, SAM.cpp, Progress.cpp, MemCard.c, Store.c, Audio.c, Arena.c, ...). 17 static initializers in
`.ctors` (each ends a C++ file). Compiler evidence from zlib (linked inside game code): GC/2.6, `-lang=c++`,
`-O4,p -inline auto,deferred -use_lmw_stmw on -str pool,readonly`.

**Phases:**
- **G0 file boundaries:** `~/geist-work/game/tu_cluster.py` groups functions sharing private constants
  (sdata2/rodata/data objects used only within a 64 KB window): 1,784 candidate boundaries (upper bound; many
  functions have no private constants). Combine with string pools (one pool per file), `.ctors` entries,
  jump tables, vtables/RTTI and assert file names to settle real files.
- **G1 flags:** confirm GC/2.6 + the flags above on a few small game functions (C vs C++ files differ).
- **G2 split all game code into asm units** with those boundaries (DOL must stay OK). Then units can be
  matched independently, and helpers can own disjoint address ranges.
- **G3 match,** starting with code the Rust port needs most: file/stream/asset loading (File.cpp,
  DataStream.cpp, asyncio.cpp, Image.cpp, Memory.cpp, chunk loaders), then engine systems, then gameplay.
- **Helpers:** worth it after G2 (two helpers on separate address ranges). Not before, because splitting
  touches the whole splits/symbols files.
- **Library leftovers (3):** file_io.c (fflush), Gecko_ExceptionPPC.cp, EXI2_GDEV main.c; revisit later.

## Chunk 12 — MSL_C common almost done (Lead, 2026-10-11)

**Result:** `SDK Code: 98.70% fuzzy, 98.52% matched, 98.14% linked (199 / 202 files)`, DOL OK.
New: direct_io, buffer_io, mem, ansi_files, extras, ctype, uart_console_io_gcn, abort_exit, FILE_POS,
mbstring, qsort (the unit split as "scanf.c"), printf (+clearerr in misc_io), ansi_fp, alloc.
- **MSL_C common flags:** `-O4,p -opt level=0,peephole,schedule -inline off` (cflags_msl). extras.c is
  different: cflags_rt (-O4,p, inlining) + `-DMSL_USE_INLINES=1`; ctype.c also needs `-DMSL_USE_INLINES=1`
  (weak inline ctype functions; only tolower/isupper/isdigit survive).
- Even with -inline off, files are emitted in **reverse source order**.
- **Comma expressions** (e.g. macros like `((a)->x = b, (a)->y = c)`) make the compiler reserve saved
  registers for temporaries; that explains "reserved but unused" registers (alloc's FixSubBlock_construct is
  such a macro; Block_empty too).
- Older-revision details: printf's `va_start(ap, fmt)` evaluates `(void)fmt` (Prime's stdarg); no locale radix
  (`'.'`); fprintf/vfprintf check `fwide(file, -1) >= 0`; abort -> exit -> __exit; UART init split into
  __init_uart_console; critical-region ids differ from TP's enum (setvbuf uses 2).
- Wrong dtk names fixed: 0x802EF06C is __flush_buffer and 0x802EF138 __load_buffer (were swapped);
  "strnicmp" 0x802F6ACC is strupr; the "scanf.c" unit is qsort; 0x80316480 is _ExitProcess (now in
  __ppc_eabi_init.c's split).
- Tool: `~/geist-work/mwprobe/mtry.py UNIT_SRC CANDIDATE [flags]` compiles a candidate with the unit's exact
  ninja flags and reports per-function matches inside the unit's range (it does not check order; the DOL
  check does). `INC=build/scratch/plibc` swaps in Prime's libc headers (copied there with a minimal types.h).
- **Left (3):** file_io.c (fflush: retail reserves r29 but compares r3 directly; everything else matches),
  Gecko_ExceptionPPC.cp (full C++ EH runtime, no reference source), EXI2_GDEV main.c (gdev_cc_read
  register allocation).

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 98.70% fuzzy, 98.52% matched, 98.14% linked (199 / 202 files)
```

## Chunk 11 — MSL flags fix + Prime reference (Lead, 2026-10-11)

**Result:** `SDK Code: 95.02% fuzzy, 93.87% matched, 92.44% linked (185 / 202 files)`, DOL OK. New: string.c.
- **Flag fix:** MSL_C common files are `-O4,p` *followed by* `-opt level=0,peephole,schedule`
  (`cflags_msl = [*cflags_rt, "-opt level=0,peephole,schedule"]`). The helper's probes always had -O4,p first;
  configure had dropped it, which changes register saves (stmw for 2 registers). All matched MSL units
  still match with the fix.
- **Reference:** `~/geist-work/refs/prime` (sparse clone of PrimeDecomp/prime, read-only:
  extern/sdk/{runtime,libc,include}) has an *older* MSL revision. string.c: strlen..strchr from Prime match;
  strcat/strrchr from TP; strncat/strstr written from the MSL algorithm (all 10 match).
  Prime's Gecko_ExceptionPPC.cpp is only a 40-line stub (no help).
- Probe Prime files with: `cd ~/geist-work/refs/prime/extern/sdk && GM=... probe.py 2.6 runtime/<f>.c
  -opt level=0,peephole,schedule -gccinc -i include -i libc -i runtime -use_lmw_stmw on
  -str reuse,pool,readonly -common off -char signed -func_align 4 -inline deferred,auto` (probe adds -O4,p
  itself). First results: alloc 3/15, uart_console_io 1/2, mem 0/4 (mem matches neither TP nor Prime yet:
  retail memchr keeps a dead copy of `n` in r29). Others failed to compile only for lack of `-gccinc`.
- **Next:** per MSL file, pick per function the TP or Prime version that matches; files still left:
  abort_exit, alloc, ansi_files, ansi_fp, buffer_io, ctype, direct_io, extras, file_io, FILE_POS,
  mbstring, mem, printf, scanf, uart_console_io_gcn (+ Gecko_ExceptionPPC, gdev main.c).

## Chunk 10 — library leftovers (Lead, 2026-10-11)

**Result:** `SDK Code: 94.76% fuzzy, 93.37% matched, 92.11% linked (184 / 202 files)`, DOL OK.
New: runtime.c, __init_cpp_exceptions.cpp, global_destructor_chain.c, NMWException.cp, **exception.cp
(new unit)**, TRK msghndlr.c, support.c, mslsupp.c. "ptmf.c" was not a real unit (ptmf is stripped);
its function is NMWException's dthandler, so it was folded in.
- Unreferenced .ctors/.dtors entries in C: `#pragma force_active on` + `__declspec(section ".ctors")`
  (plain section names; dtk's ldscript does not merge `.ctors$10`).
- Geist's older C++ runtime: std::exception (dtor, what, RTTI on) and a `throw()` operator delete form
  their own unit 0x802EABBC-0x802EAC88 before NMWException; duhandler inlines std::terminate (handler
  declarations inside `namespace std`); GetR2 is a called `asm char* GetR2(void)`.
- 0x802ECF9C is MSL `abort` (overrides the SDK's weak abort); 0x802EBF6C is `__unexpected`.
- MetroTRK v2.0 differences: Read/WriteMemory use a plain `u8 buf[0x800]` (no ARAM, no 32-byte offset);
  TRKSuppAccessFile flags EOF on short transfers; mslsupp has __open_file/__close_file/__position_file/
  __close_console (targsupp stubs TRKOpenFile 0x802E8DC8, TRKCloseFile, TRKPositionFile).
- `~/geist-work/mwprobe/rdis.py ADDR SIZE` disassembles retail with symbol names.
- **Left (18):** 16 MSL_C common files (older MSL revision: e.g. memchr keeps a dead copy of `n`; no -opt
  setting reproduces it, so they need that revision's source), Gecko_ExceptionPPC.cp (full C++ EH
  runtime), EXI2_GDEV main.c (`gdev_cc_read` register allocation: retail keeps p1/p2 apart from size).

## Chunk 9 — merged helper-msl: MSL_C / Runtime / TRK (2026-10-10)

**Result after merge:** `SDK Code: 94.32% fuzzy, 92.52% matched, 89.34% linked (176 / 202 files)`, DOL OK.
- All GC/2.6. TRK 24/28 (TP's cflags_trk; serpoll and DDH/GDEV main.c add `-sdata 8`). Runtime 2/8
  (`-O4,p`; GCN_mem_alloc adds `-str reuse,nopool,readonly`). MSL_C 36/52: math with cflags_runtime;
  common files use `-opt level=0,peephole,schedule` (strlen/rand/time only match with it); signal, time,
  wchar_io and ansi_files also need `-use_lmw_stmw off`.
- Geist's MSL common source is an older revision than TP's; the 16 left need hand reconstruction.
- **Left (asm):** TRK msghndlr, support, mslsupp, EXI2_GDEV main.c; Runtime global_destructor_chain
  (C strips the last .dtors entry 0x80341268), NMWException, ptmf, runtime, __init_cpp_exceptions,
  Gecko_ExceptionPPC; 16 MSL_C common files; targsupp.s/exception.s not split; 0x802F4B38 (0xAC) unknown.
- `-inline deferred` emits functions in reverse order, so check names of look-alike functions by call sites.
  dtk's `AMGetZeroBuffer` 0x802E6964 is really `GetTRKConnected`.
- .init: mem_TRK 0x80003100-0x80003154, dolphin_trk 0x80005088-0x800050B4.
- Merge fix: GXInit's pseudo-global `time` renamed `__GXInitTime` (clashed with MSL `time()`).
- The linker sometimes fails once after a config change and passes on a rerun.

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 94.32% fuzzy, 92.52% matched, 89.34% linked (176 / 202 files)
```

## Chunk 8 — merged helper-musyx: MusyX + zlib (2026-10-10)

**Result after merge:** `SDK Code: 99.99% fuzzy, 99.98% matched, 100.00% linked (114 / 114 files)`, DOL OK.
- MusyX 2.0.3, GC/1.3.2, 0x8031EAF4-0x8033F990 (25 code units + dsp_import.c data). A compat math.h maps
  MSL inline fmodf/powf/floorf to double versions.
- zlib 1.1.4 at 0x80197E50-0x8019FD58 (inside game code; 11 units incl. deflate), **compiled as C++ with
  GC/2.6**: `-lang=c++` (must come first, it resets earlier options) `-O4,p -inline auto,deferred
  -use_lmw_stmw on -str pool,readonly`. **Strong hint for the game code: GC/2.5+, C++, stmw, pooled
  readonly strings, deferred inlining.**
- objdiff scores some data sections < 100% (hw_dspctrl, infblock, inflate, infcodes, hardware) though the
  DOL is identical: data-symbol pairing artefacts.
- symbols.txt merges: `~/geist-work/mwprobe/sym_merge3.py BASE OURS THEIRS OUT` (keyed by section+address).

## Chunk 7 — SDK: all 77 units build from C (Lead, 2026-10-10)

**Result:** `SDK Code: 99.97% fuzzy, 99.96% matched, 100.00% linked (77 / 77 files)`, DOL byte-identical.
New: GXInit, GXAttr, GXFifo, dvd, arq, dvdfs, OSFont, mtx, mtx44 (new unit), vec, quat, OSSync (new
unit), OSRtc.

**Findings:**
- **The "linker hang at 0x80314318" was a duplicate label**, not a boundary problem: symbols.txt had
  `SystemCallVector` both as a label and as a function there. Renaming the label to
  `__OSSystemCallVectorStart` lets OSSync.c be its own unit; nothing hangs.
- Geist's SDK `GXData` is 0x5B8 bytes (dolsdk2004: 0x5B0); `__gx.h` pads it with 8 bytes at the end.
- GXFifo: `CPUFifo`/`GPFifo` are statics declared before `__GXCurrentThread` (retail .sbss order).
  0x803069DC is `GXGetCPUFifo`, not `GXGetCurrentGXThread` (that one is stripped).
- Video-mode tables in GXFrameBuf .data: GXNtsc480IntDf 0x803C7CE8, GXNtsc480Prog 0x803C7D24 (the game
  picks it for progressive scan), GXMpal480IntDf, GXPal528IntDf, GXEurgb60Hz480IntDf.
- dvd.c's .text runs to 0x803045BC (DVDCancelAllAsync, DVDGetCurrentDiskID, DVDCheckDisk,
  __DVDPrepareResetAsync, __DVDTestAlarm); its .sbss starts at `executing` 0x804D59C0 (the two
  objects before it are dvdfs.c's). arq.c keeps `ARQGetChunkSize` (0x802F8A94).
- mtx44.c (C_MTXPerspective, C_MTXOrtho, PSMTX44Identity) sits at 0x8030D114-0x8030D2B0 after
  mtx44vec.c; sdata2: mtx 0x804DB868, mtx44 0x804DB87C, vec 0x804DB898, quat 0x804DB8A0-0x804DB8B8.
- objdiff `gap_... size out of section bounds` = alignment padding after the *previous* unit's
  section; extend that unit's end over it (dsp_task .data, OSError .data/.sdata, fstload .sdata).
- Tool bugs fixed: `auto_match.py` ninja timeout 180 s -> 600 s (it was reverting good attempts);
  `names_from_relocs.py` only renames placeholder names (fn_/lbl_/jumptable_/@) now.

**Left in the SDK area (Lead):** unknown small units with no dolsdk2004 source: dvdfatal.c
(`__DVDPrintFatalMessage` 0x80304A44 + `FatalFunc` .sbss 0x804D5A10), the 0x50-byte function
0x802FFA44 between dsp.c and dsp_task.c, `fn_8031439C` (4 bytes, after OSSync), and the stub libs
0x8033F5B8-0x80341200 (odemuexi2 DebuggerDriver: "Can't select EXI2 port!" string; no reference
source here; plus a 0x3B4-byte float function at 0x8033F5B8 with a 0x24 wrapper). The 8-byte
`return 0` stubs dtk named `AMC_IsStub` (0x802F6B9C) and `Hu_IsStub` (0x8033F990) don't match dolsdk2004's
amcstubs (which returns 1), so their real names/units are unconfirmed. objdiff still scores OSExec `Run` at 50% (asm; DOL is
identical), so check the reloc naming.

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 99.97% fuzzy, 99.96% matched, 100.00% linked (77 / 77 files)
```

## Chunk 6 — SDK: 64 / 75 files matched

**Result:** `SDK Code: 97.86% fuzzy, 97.85% matched, 76.91% linked (64 / 75 files)`, DOL byte-identical.
New: dsp, dsp_task, dvdlow, OS, Pad, OSCache, SIBios, vi, CARDBios, __ppc_eabi_init, EXIBios.

**Fixes that mattered:**
- **Pooled sections are force-active** (`...data.0` / `...bss.0` markers): nothing inside them is
  stripped, so their split covers the full compiled size (e.g. dsp.c keeps "DSPInit(): Build Date"
  strings that no code uses in release builds).
- **Alignment padding** between a section and the next split (< 32 zero bytes) is absorbed, otherwise
  objdiff-cli's report fails on a tiny gap unit.
- **vi.c reproduces a bug in Geist's SDK build:** `VISetPreRetraceCallback` reads and writes `PostCB`.
- **EXIBios** must be built with `-O3,p` *instead of* `-O4,p` (`cflags_dolphin_O3`); appending
  `-O3,p` after `-O4,p` does not override it.
- CARDBios.c starts at 0x802F8BB0 (`__CARDDefaultApiCallback`); __ppc_eabi_init.c also has `.init`
  0x800054EC-0x80005544 (`__init_hardware`, `__flush_cache`).

**Remaining (11):**
- `dvd/dvdfs.c`, `os/OSFont.c`: the DOL links identically, but objdiff-cli's report refuses
  (compiled `.data` is 4 bytes longer than the split because the linker strips a trailing object).
  Needs a symbols/splits workaround that objdiff accepts.
- `gx/GXInit.c` + `gx/GXFrameBuf.c`: GXInit uses the video-mode tables (`GXNtsc480IntDf` at
  0x803C7CE8 etc.) defined in GXFrameBuf.c, whose `.data` isn't split yet; a first attempt pulled in
  GXAttr's jump tables. Give GXFrameBuf a .data split exactly covering the four tables, then GXInit.
- `gx/GXFifo.c` (undefined `lbl_804D5A48` from GXInit), `dvd/dvd.c` (`__DVDThreadQueue`),
  `ar/arq.c` (overlaps ar.c data): symbol naming/placement between neighbours.
- `gx/GXAttr.c`: dtk's `GXTexRegionAddrTable` size; `mtx/mtx.c`, `vec.c`, `quat.c`: shared .sdata2
  constant tangle; `os/OSRtc.c`: waits on the OSSync linker hang.

**Safety:** commit after every successful batch. `KEEP=1` leaves a failed attempt in place for
`dol_diff.py`; afterwards restore with `git checkout -- config/GITE01/splits.txt
config/GITE01/symbols.txt configure.py` (only safe because everything good is committed).

## Chunk 5 — SDK: 53 / 75 files matched

**Result:** `SDK Code: 95.99% fuzzy, 93.02% matched, 48.70% linked (53 / 75 files)`, DOL byte-identical.

**Correction to chunk 3:** "37 files have a differing function" was wrong. My probe listed compiled
functions in symbol-table order instead of code order. With that fixed (and the project's real flags,
e.g. `-char unsigned`, EXIBios `-O3,p` without `-O4,p` before it), **74 of 75 SDK units match their
source completely**; the 75th, OSRtc.c, only fails because it deliberately also covers OSSync.c.

**Changes this chunk:**
- Unit edges fixed: mtx.c extended to 0x8030D06C; dsp.c ends 0x802FFA44 and dsp_task.c starts 0x802FFA94
  (the 0x50-byte function between belongs to a file not in dolsdk2004); dvd.c starts 0x80301A50 (takes
  the 4-byte function); dvdidutils.c ends 0x80304A44 and fstload.c starts 0x80304A74 (0x30-byte unknown
  function between); ar.c starts 0x802F6BA4 and ai.c 0x80319D50 (their preceding functions use the
  files' own statics, so they belong to them).
- `obj_layout.py` follows pointers stored in data (e.g. `__ARVersion` -> the SDK version banner) to
  place objects that only other data references. This unblocked ar.c and ai.c.
- Globals not used by their own file's code but already named in symbols.txt keep that address.
- `auto_match.py` needs two passes (some files only link once their neighbours are C).
- Process note: I once ran `git checkout` on splits/symbols/configure.py, which discarded uncommitted
  progress; it was regenerated by re-running the scripts. Commit after each chunk.

**Still assembly (22):** ar/arq.c, card/CARDBios.c, dsp/dsp.c, dsp/dsp_task.c, dvd/dvdlow.c, dvd/dvdfs.c, dvd/dvd.c, gx/GXInit.c, gx/GXFifo.c, gx/GXAttr.c, mtx/mtx.c, mtx/vec.c, mtx/quat.c, os/OS.c, os/OSCache.c, os/OSFont.c, os/__ppc_eabi_init.c, pad/Pad.c, vi/vi.c, exi/EXIBios.c, si/SIBios.c, os/OSRtc.c.
Typical blockers (`~/geist-work/mwprobe/pending_errors.json`, re-run `ONLY=<file> KEEP=1 python3
auto_match.py` then `python3 dol_diff.py` to see the first differing bytes):
- links but DOL differs: dsp, dvdlow, dvdfs, mtx, OS, OSCache, OSFont, Pad, vi, EXIBios, SIBios
  (probably data placement details: stripped/kept objects, or strings in pools);
- duplicate/undefined symbols between a C file and neighbouring asm (CARDBios `...bss.0`, GXInit
  `GXNtsc480IntDf`, GXFifo, dsp_task, dvd `__DVDThreadQueue`, __ppc_eabi_init `__init_hardware`);
- GXAttr: dtk symbol `GXTexRegionAddrTable` has the wrong size; vec/quat: constant pool tangle.

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 95.99% fuzzy, 93.02% matched, 48.70% linked (53 / 75 files)
```

## Chunk 4 — SDK data placement (33 / 75 files matched)

**Result:** 33 SDK files build from C with a byte-identical main.dol
(`SDK Code: 95.56% fuzzy, 92.59% matched, 20.14% linked (33 / 75 files)`). Added this chunk:
dvderror, dvdqueue, fstload, GXTev, GXBump, GXPerf, GXFrameBuf, GXDisplayList, GXLight, GXPixel, mtxvec,
mtx44vec, OSAlloc, OSArena, OSAudioSystem, OSError, OSLink, OSMemory, OSReboot, OSResetSW, EXIUart,
Padclamp, SISamplingRate.

**What made data placement work (all in `~/geist-work/mwprobe/auto_match.py` + `obj_layout.py`):**
- The linker strips unused **data** too (constants/strings used only by stripped functions), so each
  data object is located individually from relocations, not by section base + offset.
- A section's split runs from its base to the end of its last *referenced* object (rounded to 4).
  Sections with no referenced object at all were stripped entirely and get no split.
- Only referenced objects are written to symbols.txt; dtk's guessed data symbols inside those ranges
  are removed first (dtk had guessed wrong sizes, e.g. `lbl_803C7872` spanning three strings).
- A section never starts before the end of an earlier split of the same section.

**Still assembly although the code matches (5):**
- `mtx/mtx.c`: unit extended to 0x8030D06C (now 15 functions, full code match), but its `.sdata2`
  constants interleave with vec.c/quat.c references (undefined lbl_804DB858..lbl_804DB864). Fix the
  three together.
- `mtx/vec.c`, `mtx/quat.c`: same constant-pool tangle (quat is referenced from the 0x8030D114 gap,
  probably mtx44.c functions).
- `gx/GXAttr.c`: dtk's `GXTexRegionAddrTable` (0x803C7A4C, guessed size 0xC0) crosses the boundary;
  set its real size in symbols.txt.
- `ar/arq.c`: its .sbss objects overlap a variable used by ar.c (lbl_804D58FC), so the placement is
  wrong; check against ar.c's own data.

## Chunk 3 — first SDK files matched in C (17 / 75)

**Result:** 17 Dolphin SDK files now build from C source and link into a byte-identical main.dol
(`SDK Code: 93.04% matched, 9.35% linked (17 / 75 files)`):
PPCArch, CARDFormat, CARDOpen, CARDCreate, CARDRead, CARDWrite, CARDDelete, CARDStat, CARDRename,
OSMutex, dvderror, OSArena, OSAudioSystem, OSMemory, OSReboot, OSResetSW, EXIUart.

**Source:** `src/dolphin/` and `include/` are copied from doldecomp/dolsdk2004 (commit 2328b416,
2025-02-03), at the user's request. **That repository has no licence**, so this project must stay
private. Flags are dolsdk2004's (`cflags_dolphin` in configure.py: GC/1.2.5n, `-char unsigned`, with
`-char signed` for CARDOpen/CARDRename/dvd/mtx/mtx44 and `-O3,p` for EXIBios).

**Tools (in `~/geist-work/mwprobe/`):**
- `tu_check.py`: compiles each split SDK file and checks whether its functions, in order, cover the
  unit exactly (functions Geist never calls are allowed to be missing: the linker strips them).
  38 of 75 pass. The other 37 have at least one function that differs (Geist's SDK builds are
  Apr 5 / Nov 10 2004; the reference is 4/20/2004).
- `data_locate.py`: finds each data section of a compiled file in main.dol from the DOL's own
  instructions at relocation sites (r13/r2 small-data loads, lis/addi pairs).
- `names_from_relocs.py --apply`: gives every symbol a C file defines or references its real name in
  symbols.txt (e.g. memcmp, strlen, __shr2i, __CARDBlock), so asm and C units link together.
- `auto_match.py`: tries each candidate file (adds data splits, marks Matching, renames), rebuilds,
  and keeps it only if the SHA-1 still matches.

**Why the other fully-matching files were reverted:**
- dtk's guessed data-symbol sizes conflict with the compiled section sizes ("split ends within symbol",
  e.g. fstload .data ends inside lbl_803C7872). Fix: adjust that symbol's size in symbols.txt, or the
  split end, per file.
- Data sections only referenced from stripped functions can't be located by relocations (shown as
  "not located confidently"): GXFrameBuf, GXDisplayList, mtxvec, mtx44vec, quat, OSLink, Padclamp, arq.
- Shared constant pools: GXPixel and vec both reference lbl_804DB85C in .sdata2, so at least one base is
  wrong. mtx.c: an asm unit still calls a function inside it by its old name.

**Verify (current state, after chunk 4):**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && rm -f build/GITE01/main.dol && ninja | grep -E "main.dol|SDK Code"
# build/GITE01/main.dol: OK
# SDK Code: 95.56% fuzzy, 92.59% matched, 20.14% linked (33 / 75 files)
```

**Next:** fix the reverted files one by one (data sizes, constant pools), then adapt the 37 differing
files to Geist's SDK build (diff each function with `probe.py --diff`).

## Chunk 2 — Dolphin SDK splits (done for .text)

**Result:** 75 SDK translation units across 14 libraries (ai, am, ar, base, card, db, dsp, dvd, exi, gx,
mtx, os, pad, si, vi) in `config/GITE01/splits.txt` (`.text` ranges only), 674 SDK functions named in
`symbols.txt`, and matching `DolphinLib` entries (all `NonMatching`, no source yet) in `configure.py`.
The relinked DOL still matches retail.

**How:** `~/geist-work/mwprobe/sdk_scan.py` compiles all 182 dolsdk2004 sources with GC/1.2.5n
(`EXIBios.c` with `-O3,p`, as dolsdk2004 does) and searches main.dol: 702 functions match at exactly one
address in the SDK range. `gen_sdk_splits.py` turns contiguous runs into units, drops files that only
"match" duplicated helper code (CARDProgram, CARDErase, SISteeringAuto, axfx/reverb_std), extends units
with neighbouring functions whose names appear in the same source file, and writes splits + renames.

**Gotchas found:**
- **Linker hang:** any unit boundary at 0x80314318 (start of OSSync.c's system-call stub) makes the
  GC/2.6 linker spin forever. Workaround: the `dolphin/os/OSRtc.c` unit also covers OSSync.c
  (0x80314318-0x803143A0), with a TODO in splits.txt. Making the label the function name instead gives
  a bare "Link failed". Revisit when OSSync.c gets real source.
- Statics with the same name in several files (OnReset, AlarmHandler, WriteCallback, EraseCallback,
  BootInfo, ...) must be `scope:local`, otherwise the linker reports multiply-defined symbols.
- `__OSSetExceptionHandler` rename skipped (name already used by dtk elsewhere).
- Gaps not yet assigned: mtx (0x8030CD80-0x8030D06C, 0x8030D114-0x8030D2B0, probably mtx44/psmtx
  variants), small 1-function gaps between CARD files, and dvd 0x8030427C-0x803045BC.

**Not yet split:** MSL C / Runtime / TRK (≈0x802E0000-0x802F6BE8), MusyX + others (0x8031DE10-0x8033F5B8,
137 KB, 382 functions), zlib 1.1.4 (location not yet confirmed), and `.data`/`.bss`/`.sdata` for all units.

**Verify:**
```sh
cd ~/Documents/"Claude code"/geist-matching && export PATH=$HOME/.local/bin:$PATH
python3 configure.py && ninja | grep "SDK Code"   # SDK Code: 0.00% ... (0 / 75 files)
sha1sum build/GITE01/main.dol                     # 6612f52ce6c50bb831ffddd5264b22973304f288
```

## Chunk 1 — compiler version (done as far as the evidence goes)

### Verify the baseline
```sh
export PATH=$HOME/.local/bin:$PATH
python3 configure.py && ninja
sha1sum orig/GITE01/sys/main.dol build/GITE01/main.dol   # both 6612f52ce6c50bb831ffddd5264b22973304f288
```

### Tool
`~/geist-work/mwprobe/probe.py` (outside this repo): compiles a C file with a chosen CodeWarrior
version and searches main.dol for each function, masking relocated fields. `--diff <addr>` prints a
side-by-side disassembly. Reference SDK source: `~/geist-work/refs/dolsdk2004` (public matching
decomp of the 4/20/2004 SDK; read-only, none of its scripts are run).

```sh
cd ~/geist-work/mwprobe
python3 probe.py 1.2.5n ~/geist-work/refs/dolsdk2004/src/pad/Pad.c        # expect 22/33 MATCH
python3 probe.py --diff 80031FAC 2.6 game/v1.c -O4,p                       # expect no "!!" lines
```

### Findings
1. **SDK libraries: GC/1.2.5n, `-O4,p -inline auto`** (confirmed). Compiling dolsdk2004 sources and
   searching main.dol:

   | version | OSAlarm.c | Pad.c | dvdfs.c |
   |---|---|---|---|
   | 1.2.5 | 6/14 | 13/33 | 14/31 |
   | **1.2.5n** | **9/14** | **22/33** | **18/31** |
   | 1.3 – 2.7 | 3/14 | 4/33 | 7/31 |
   | 3.0a3 | 3/14 | 2/33 | 3/31 |

   Misses are mostly functions Geist never calls (dead-stripped by the linker) or small differences
   between the 4/20/2004 reference and Geist's builds (Apr 5 2004 for AR/CARD/DVD/PAD, Nov 10 2004
   for GX/OS). `configure.py`'s `DolphinLib` already uses GC/1.2.5n, so no change needed.
2. **Game code: GC/1.3.2, 2.0, 2.5 or 2.6; working choice GC/2.6.**
   - A hand-written C version of fn_80031FAC (104 bytes) matches byte-for-byte with 1.3.2-2.7 and with
     none of 1.2.5n / 3.0a3 / 3.0a5. Flags: `-O4,p`.
   - **Linker test** (swap `config.linker_version`, relink the baseline): GC/1.2.5n, 1.3.2, 2.0, 2.5 and
     2.6 all reproduce the retail SHA-1. GC/2.7 warns "runtime sources 'global_destructor_chain.c' and
     '__init_cpp_exceptions.cpp' both need to be updated to latest version" and changes 67 bytes (also
     with comment version 11). GC/3.0a3/3.0a5 crash. So Geist predates CW 2.7.
   - 2.5 and 2.6 generate identical code for all 78 SDK sources compiled (os, gx, mtx, card). 2.0 differs
     on 7 (float loops: 2.5/2.6 unroll by two and use paired-single saves), 1.3.2 on 17.
   - Not settled: 1.3.2 vs 2.0 vs 2.5/2.6. Two probes (paired-single FPR saves: present from 1.3.2 on;
     a FIFO-write loop: identical in all) didn't separate them. Game code has 6 compiler-unrolled loops
     (e.g. fn_8000761C); reproducing one of those float loops is the next discriminator.
   - `mw_comment_version: 10` is correct for 1.3.2-2.6. configure.py comment updated with this evidence.
3. **Game code uses pooled strings** (`-str pool` or similar): literals are addressed as
   `pool_base + offset`. Will need adding to the game cflags.
4. n-Space's chunk loaders use a bounds-check macro (need = count*size; ok = left>0 && need>0;
   ptr = ok ? cursor : 0; then advance by need plus 32-byte padding). fn_80043270 (model loader)
   reproduces except 13 instructions of register numbering (same for all five compilers).
5. Geist's C library (MSL) is a different source release from Twilight Princess's (TP builds MSL with
   GC/2.7): TP's memmove can't match Geist's under any version/optimisation tried. Pikmin 2's repo
   moved (not found at projectPiklopedia/pikmin2 or doldecomp/pikmin2); worth finding later.

### Next steps
- (Optional) Separate 1.3.2 / 2.0 / 2.6 by matching one of the six compiler-unrolled game loops.
- Chunk 2: done (see above).

### Verify chunk 1
```sh
cd ~/geist-work/mwprobe && python3 codegen_cmp.py
# expect: 80 files: identical code under all four = 61, differs = 17, compile errors = 2
```

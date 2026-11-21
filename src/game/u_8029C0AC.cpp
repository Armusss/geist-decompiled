// u_8029C0AC.cpp: NPC / path-follow actor ("Sve") message handlers and follow states (0x8029C0AC-0x8029E714).
// Names are descriptive guesses.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned char u8;
typedef unsigned long u32;
typedef long s32;

// Empties a range of 12-byte elements: exactly the counted loop CodeWarrior
// emits for a "for each element, call the (empty) destructor" loop.
struct Entry12 {
    u32 a;
    u32 b;
    u32 c;
};

void ClearEntryRange(Entry12* begin, Entry12* end) {
    for (; begin < end; begin++) {
    }
}

// ---------------------------------------------------------------------------
// Shared declarations for the NPC/"SVE" actor message handlers below.
// ---------------------------------------------------------------------------

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    int i;
    u32 h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    for (i = 0; s[i] != 0; i++) {
        h *= 31;
        h += (unsigned char)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
}

inline u32 StrHash(const char* s, u32 h) {
    int i;

    if (s == 0) {
        return 0;
    }
    for (i = 0; s[i] != 0; i++) {
        h *= 31;
        h += (unsigned char)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
}

struct Owner;

// 16-byte per-state descriptor: enter, may-leave check, per-frame update, name.
typedef int (*TypeFunc)(Owner*);

struct TypeInfo {
    TypeFunc fn0;
    TypeFunc fn4;
    TypeFunc fn8;
    const char* name;
};

// Per-actor event/stream block owned through Owner::evt.
struct PathEvent {
    int state;                            // 0x00: current type index
    float unk04;                          // 0x04
    int flags;                            // 0x08
    float timer;                          // 0x0c
};

// Type descriptor reached through Owner::unkC4.
struct SveTypeData {
    u32 type;                             // 0x000: actor type hash
    u8 unk004[0x160 - 0x004];
    u32 anim160;                          // 0x160: animation bank
    u32 anim164;                          // 0x164
    u32 anim168;                          // 0x168
};

struct Owner {
    u8 unk000[0x24];
    int i24;                              // 0x24
    int i28;                              // 0x28
    int i2c;                              // 0x2c
    u8 unk030[0x38 - 0x30];
    int unk38;                            // 0x38: actor body id
    u8 unk03c[0x84 - 0x3c];
    Vec3 pos84;                           // 0x84: owner world position
    u8 unk90[0xc4 - 0x90];
    SveTypeData* unkC4;                   // 0xc4: actor type descriptor
    u8 unkC8[0xd4 - 0xc8];
    void* unkD4;                          // 0xd4: agent handle
    u8 unkD8[0xfc - 0xd8];
    void (*fn0fc)(void*);                 // 0xfc
    void* unk100;                         // 0x100
    int (*unk104)(Owner*);                // 0x104: per-frame update handler
    u8 unk108[0x10c - 0x108];
    void (*fn10c)(void*);                 // 0x10c: follow-behaviour handler
    u8 unk110[0x128 - 0x110];
    int unk128;                           // 0x128: actor class id
    u8 unk12c[0x130 - 0x12c];
    int* evt;                             // 0x130: event/stream block
    u8 unk134[0x138 - 0x134];
    float f138;                           // 0x138: timer increment
    u8 unk13c[0x21c - 0x13c];
    int unk21c;                           // 0x21c: pending follow target
    int unk220;                           // 0x220: follow sub-state
    u8 unk224[0x22c - 0x224];
    int unk22c;                           // 0x22c: follow target object
    u8 unk230[0x268 - 0x230];
    int unk268;                           // 0x268: lowest seen timer
    u8 unk26c[0x270 - 0x26c];
    u8 cond270[0x288 - 0x270];            // 0x270: condition block
    int unk288;                           // 0x288
    u8 unk28c[0x2c4 - 0x28c];
    u32 unk2c4;                           // 0x2c4
    u8 unk2c8[0x5f4 - 0x2c8];
    void* unk5f4;                         // 0x5f4
    u8 unk5f8[0x5fc - 0x5f8];
    void (*fn5fc)(void*);                 // 0x5fc
    void (*fn600)(void*);                 // 0x600
    void (*fn604)(void*);                 // 0x604
    u8 unk608[0x894 - 0x608];
    u8 unk894[0x904 - 0x894];
    float f904;                           // 0x904
    int unk908;                           // 0x908: actor id
};


int OwnerId(Owner* owner);
int ActorPlayAnim(int id, u32 anim, int arg);
void FollowSetTargetObj(void* self, void* target);
// One-argument form: the state-switch helpers below call 0x800A78A0 with a
// single argument (r4 is never set before the bl), so they bind to this one.
void FollowSetTargetObj(void* self);

extern "C" String fn_800225C8(const String& a, const String& b);
extern "C" void fn_8028C188(void* out, const char* s, int length);
extern "C" void* memcpy(void* dst, const void* src, u32 n);

int NpcSveHandleMsg(int msg, int* p);
int NpcSveUpdateActor(Owner* owner);
int NpcSveCheckTarget(Owner* owner);
int NpcSveStopFollow(Owner* o);
int NpcSveBeginFollow(Owner* owner);
int NpcSveCheckFollowReady(Owner* owner);
int NpcSveCancelFollow(Owner* o);
int NpcSveStartFollowTarget(Owner* owner);
int NpcSveUpdateFollowTarget(Owner* owner);
int NpcSveResetFollowCondition(Owner* o);
int NpcSveStartFollowState(Owner* owner);
int NpcSveUpdateFollow(Owner* owner);
int NpcSveExitFollowTarget(Owner* o);
int NpcSveStartTargetFollow(Owner* o);
int NpcSveUpdateTargetFollow(Owner* owner);
int NpcSveFinishTargetFollow(Owner* o);
int NpcSveBeginTargetFollow(Owner* o);
int NpcSveRefreshFollowTarget(Owner* owner);
int NpcSveEnterStateThree(Owner* o);
int NpcSveBeginPathFollow(Owner* o);
int NpcSveUpdatePathFollow(Owner* owner);
int NpcFollowOffsetSetup(void* p);
int NpcFollowClearState(void* p);
int NpcSveUpdateFollowAim(void* p);
int NpcSveClearTargetFollow(void* p);
extern "C" int NpcSveResetEvent(int owner);
extern "C" int NpcSvePlayAnimMsg(Owner* a, Msg40A* m);

// The asset found by AssetFind for the 0x41e query reply, cached at init.
int gNpcSveAsset[2] = { -1, 0 };

TypeInfo gNpcSveTypes[8] = {
    { (TypeFunc)NpcSveResetEvent, (TypeFunc)NpcSveClearTargetFollow, (TypeFunc)NpcSveUpdateFollowAim, "" },
    { NpcSveBeginPathFollow, NpcSveEnterStateThree, NpcSveRefreshFollowTarget, "" },
    { (TypeFunc)NpcFollowClearState, (TypeFunc)NpcFollowOffsetSetup, NpcSveUpdatePathFollow, "" },
    { NpcSveBeginTargetFollow, NpcSveFinishTargetFollow, NpcSveUpdateTargetFollow, "" },
    { NpcSveStartFollowTarget, NpcSveCancelFollow, NpcSveCheckFollowReady, "" },
    { NpcSveBeginFollow, NpcSveStopFollow, NpcSveCheckTarget, "" },
    { NpcSveStartTargetFollow, NpcSveExitFollowTarget, NpcSveUpdateFollow, "" },
    { NpcSveStartFollowState, NpcSveResetFollowCondition, NpcSveUpdateFollowTarget, "" },
};

// Save/load request (0x7102 save, 0x7103 load).
struct MsgSave {
    Owner* actor;   // 0x00
    int size;       // 0x04: room left
    u8* buf;        // 0x08
    int used;       // 0x0C
    int ok;         // 0x10
};

// Reads a saved block's tag word and checks it against the hash of the block's name.
inline int SaveCheckTag(MsgSave* m, const char* name) {
    if (m->used < 4) {
        return 0;
    } else {
        u32* p = (u32*)m->buf;

        m->used += 4;
        m->size -= 4;
        m->buf += 4;
        if (*p == StrHash(name)) {
            return 1;
        } else {
            return 0;
        }
    }
}

static inline void NpcSveStateName(Buffer* out, const char* s) {
    String name(s);
    String newline("\n");
    const String& text = fn_800225C8(name, newline);

    fn_8028C188(out, text.mData, text.mLength);
}

int NpcSveHandleMsg(int msg, int* p) {
    Owner* a = *(Owner**)p;
    u32 tag;

    switch (msg) {
    case 0x400: {
        Msg400* m = (Msg400*)p;

        m->ownerId = OwnerId(a);
        m->state = (a->f904 > 0.0f) ? 1 : 0;
        m->f10 = 1.0f;
        m->f14 = 1.0f;
        m->flags = 2;
        m->unk1C = 0;
        return 1;
    }
    case 0x40A:
        return NpcSvePlayAnimMsg(a, (Msg40A*)p);
    case 0x409: {
        Msg409* m = (Msg409*)p;
        PathEvent* b = (PathEvent*)a->evt;

        if (ActorPlayAnim(OwnerId(a), m->arg, m->anim) != 0) {
            return 1;
        }
        switch (m->arg) {
        case 0x272CAC:
            if (gNpcSveTypes[b->state].fn4(a)) {
                FollowSetTargetObj(a->cond270, (void*)m->unk04);
                if (gNpcSveTypes[3].fn0(a) == 0) {
                    gNpcSveTypes[b->state].fn0(a);
                }
            }
            return 1;
        }
        return 0;
    }
    case 0x7102: {
        MsgSave* m = (MsgSave*)p;
        PathEvent* b = (PathEvent*)a->evt;

        if (b != 0) {
            if (m->size < 0x14) {
                m->ok = 0;
                return 1;
            }
            tag = StrHash("[NPCSVE]");
            memcpy(m->buf, &tag, 4);
            m->used += 4;
            m->size -= 4;
            m->buf += 4;
            memcpy(m->buf, b, 0x10);
            m->used += 0x10;
            m->size -= 0x10;
            m->buf += 0x10;
            return 1;
        }
        return 1;
    }
    case 0x7103: {
        MsgSave* m = (MsgSave*)p;
        PathEvent* b = (PathEvent*)a->evt;

        if (b != 0) {
            if (m->used < 0x14) {
                m->ok = 0;
                return 1;
            }
            if (!SaveCheckTag(m, "[NPCSVE]")) {
                return 1;
            }
            memcpy(b, m->buf, 0x10);
            m->used -= 0x10;
            m->buf += 0x10;
        }
        return 1;
    }
    case 0x41E: {
        Msg41E* m = (Msg41E*)p;
        PathEvent* b = (PathEvent*)a->evt;

        switch (m->cmd) {
        case 0x832DF50A:
            *(int*)&m->result = gNpcSveAsset[0];
            return 1;
        case 0xAD8576FA: {
            NpcSveStateName(&m->result, gNpcSveTypes[b->state].name);
            return 0;
        }
        }
        return 0;
    }
    }
    return 0;
}

int HasCondition(void* cond, int id);
void VFree(void* p);

extern "C" int fn_80162658(void* owner, void* arg);
extern "C" void fn_801BDC3C(u32 hash, void* owner);
extern "C" int fn_800BEF9C(u32 handle);
void ConditionSetBit(void* cond, int id, int value);
extern "C" int NpcSveChooseState(void* owner);

// Per-frame update for the path-follow actor: runs the current type's handler,
// advances the event timer and switches to a new type when requested.
int NpcSveUpdateActor(Owner* owner) {
    Owner* o = (Owner*)owner;
    if (o == 0) {
        return 0;
    }
    PathEvent* evt = (PathEvent*)o->evt;
    if (HasCondition(o->cond270, 0xa) != 0 ||
        HasCondition(o->cond270, 0xb) != 0) {
        fn_80162658(o, o->unk894);
        return 1;
    }
    if (o->unk268 > fn_800BEF9C(o->unk2c4)) {
        o->unk268 = fn_800BEF9C(o->unk2c4);
    }
    int ret = gNpcSveTypes[evt->state].fn8(o);
    evt->timer = evt->timer + o->f138;
    fn_80162658(o, o->unk894);
    if (ret != 0 || (evt->flags & 2) != 0) {
        evt->flags &= ~2;
        int next = NpcSveChooseState(o);
        if (next == -1) {
            if (o->unk908 == (int)0x8cd18ee1) {
                fn_801BDC3C(0xc8422a5b, o);
            } else {
                fn_801BDC3C(0xb1c30d62, o);
            }
            return 1;
        }
        if (next != evt->state &&
            gNpcSveTypes[evt->state].fn4(o) != 0) {
            gNpcSveTypes[next].fn0(o);
            evt->timer = 0.0f;
            ConditionSetBit(o->cond270, 0x40, 0);
            ConditionSetBit(o->cond270, 0x41, 0);
            ConditionSetBit(o->cond270, 0x43, 0);
            ConditionSetBit(o->cond270, 0x42, 0);
        }
    }
    return 1;
}

// Releases the event block and clears the actor's stream state.
void NpcSveFreeEvent(Owner* o) {
    VFree(o->evt);
    o->unk104 = 0;
    o->evt = 0;
}

// ---- declarations used by the create/check helpers below -------------------

void* VAlloc(int size);
int AssetFind(u32 id, u32 type);
void AnimBankActivate(u32 bank, u32 group);
void* FollowGetTargetObj(void* self);

extern "C" void fn_800CE96C(void*);
extern "C" void fn_800CF064(void*);
extern "C" void fn_800CC984(void*);
extern "C" void fn_801D55CC(void*);
extern "C" int fn_800B26B8(void* owner);
extern "C" void fn_800B261C(void* owner, int mode, void* out);
extern "C" int fn_800B22F8(void* owner, int mode);
extern "C" void fn_800C748C(void* self);
extern "C" float fn_80115F68(int id);

// Initialises the SVE path-follow actor: installs the per-frame update handler,
// allocates the 16-byte event block and re-activates the type's animation banks.
void NpcSveInitActor(Owner* o) {
    o->unk104 = NpcSveUpdateActor;
    PathEvent* ev = (PathEvent*)VAlloc(0x10);
    if (ev == 0) {
        o->evt = 0;
        return;
    }
    o->evt = (int*)ev;
    o->fn0fc = fn_800CE96C;
    o->fn604 = fn_800CF064;
    o->fn5fc = fn_800CC984;
    o->fn600 = fn_800CC984;
    o->unk288 = 0;
    ev->unk04 = 0.0f;
    ev->state = 0;
    ev->flags = 0;
    ev->timer = 0.0f;
    gNpcSveAsset[0] = AssetFind(0xf1754bf2, 0x21c5e0);
    if (o->unkC4->type == 0x17c92eb2 || o->unkC4->type == 0xd06b7dcc ||
        o->unkC4->type == 0x23d7f0f0 || o->unkC4->type == 0x956698c2) {
        if (o->unk5f4 != 0) {
            fn_801D55CC(o->unk5f4);
            o->unk5f4 = 0;
        }
        AnimBankActivate(0xb089184c, o->unkC4->anim160);
        AnimBankActivate(0xb089184c, o->unkC4->anim164);
        AnimBankActivate(0xb0b21bb1, o->unkC4->anim168);
    }
}

// Tells whether the actor must stay on its current follow target: the movement
// mode changed, the followed object changed, or the path timer is still running.
int NpcSveCheckTarget(Owner* owner) {
    Owner* o = (Owner*)owner;
    if (o == 0) {
        return 0;
    }
    void* target;
    int mode;
    PathEvent* ev;
    ev = (PathEvent*)o->evt;
    mode = fn_800B26B8(o);
    if (mode != 4) {
        fn_800B261C(o, mode, &target);
        if (target != FollowGetTargetObj(o->cond270)) {
            return 1;
        }
        mode = fn_800B22F8(o, mode);
        if (mode <= 2) {
            return 1;
        }
    }
    if (ev->timer > 3000.0f) {
        return 1;
    }
    return 0;
}

void ConditionUpdateBit(void* cond, int id, int value);

// Tells whether the owner is a live actor, after clearing the follow-condition
// bits. Used as the "stop following" step of the path-follow state machine.
int NpcSveStopFollow(Owner* o) {
    if (o == 0) {
        return 0;
    }
    ConditionUpdateBit(o->cond270, 4, 0);
    ConditionUpdateBit(o->cond270, 9, 0);
    return 1;
}

extern "C" void SendMsg(int to, int msg, void* data);
extern "C" void fn_800B1F20(void* owner, int mode);

// Sends the actor itself message 0x41d (key 0xf9d0235b, argument 1).
static inline void NpcSveSend41D(Owner* o) {
    int id = o->unk38;
    QueryMsg q;

    q.hash = 0xf9d0235b;
    *(int*)&q.result = 1;
    q.unk8 = 0;
    SendMsg(id, 0x41d, &q);
}

void NpcSveAdvanceToFollowState(Owner* o);
void OwnerSlotAdd(Owner* o, int mode, u32 handler, u32 target, u32 size);

// Picks up the object reported by the movement query, plays its follow
// animation and tells the host that the follow behaviour has started.
int NpcSveBeginFollow(Owner* owner) {
    Owner* o;
    PathEvent* ev;
    int mode;
    void* target;

    if (owner == 0) {
        return 0;
    }
    o = (Owner*)owner;
    ev = (PathEvent*)o->evt;
    mode = fn_800B26B8(owner);
    if (mode == 4) {
        return 0;
    }
    fn_800B261C(owner, mode, &target);
    if (target == 0) {
        return 0;
    }
    if (ActorPlayAnim(o->unk38, 0xb1c7633c, ((Owner*)target)->unk38) != 0) {
        return 0;
    }
    fn_800B1F20(owner, mode);
    FollowSetTargetObj(o->cond270, target);
    o->unk288 = 0;
    o->unk268 = 3;
    ev->state = 5;
    ev->unk04 = 0.0f;
    ConditionUpdateBit(o->cond270, 4, 1);
    ConditionUpdateBit(o->cond270, 9, 1);
    FollowSetTargetObj(o->cond270, target);
    NpcSveSend41D(o);
    OwnerSlotAdd(o, mode, (u32)NpcSveAdvanceToFollowState, (u32)target, 0x3c);
    return 1;
}

// Variant of NpcSveCheckTarget used by the follow state machine: also requires
// the movement query to report an active follow mode and a running path timer.
int NpcSveCheckFollowReady(Owner* owner) {
    Owner* o = (Owner*)owner;
    int mode;
    PathEvent* ev;
    void* target;

    if (o == 0) {
        return 0;
    }
    ev = (PathEvent*)o->evt;
    mode = fn_800B26B8(o);
    if (mode != 4) {
        fn_800B261C(o, mode, &target);
        if (target != FollowGetTargetObj(o->cond270)) {
            return 1;
        }
        if (fn_800B22F8(o, mode) <= 2) {
            return 1;
        }
    } else {
        return 1;
    }
    if (ev->timer > 3000.0f) {
        return 1;
    }
    return 0;
}

// Clears the follow-condition bits and reports whether the owner was valid;
// used as the "cancel following" step of the path-follow state machine.
int NpcSveCancelFollow(Owner* o) {
    if (o == 0) {
        return 0;
    }
    ConditionUpdateBit(o->cond270, 4, 0);
    ConditionUpdateBit(o->cond270, 9, 0);
    return 1;
}

void NpcSveAdvanceToTargetFollowState(Owner* o);
float OwnerValue(Owner* o, void* arg);
int MemFlagIsSet(void* cond, int id);
extern "C" void fn_800BBCA8(void* owner);
extern "C" int fn_800BAF40(void* owner);

// Starts following the object reported by the movement query: plays the follow
// animation, retargets the follow condition block and queues the follow update.
int NpcSveStartFollowTarget(Owner* owner) {
    Owner* o = (Owner*)owner;
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    int mode = fn_800B26B8(o);
    if (mode == 4) {
        return 0;
    }
    void* target;
    fn_800B261C(o, mode, &target);
    if (target == 0) {
        return 0;
    }
    if (ActorPlayAnim(o->unk38, 0x7c794ab6, ((Owner*)target)->unk38) != 0) {
        return 0;
    }
    fn_800B1F20(o, mode);
    FollowSetTargetObj(o->cond270, target);
    o->unk288 = 0;
    o->unk268 = 3;
    ev->state = 4;
    ev->unk04 = 0.0f;
    ConditionUpdateBit(o->cond270, 4, 1);
    ConditionUpdateBit(o->cond270, 9, 1);
    FollowSetTargetObj(o->cond270, target);
    OwnerSlotAdd(o, mode, (u32)NpcSveAdvanceToTargetFollowState, (u32)target,
                 0x28);
    return 1;
}

// Second half of the follow state machine: validates the reported follow target,
// watches the path timer and drives the follow animation / host callback.
int NpcSveUpdateFollowTarget(Owner* owner) {
    Owner* o = (Owner*)owner;
    int mode;
    PathEvent* ev;
    void* target;

    if (o == 0) {
        return 0;
    }
    ev = (PathEvent*)o->evt;
    mode = fn_800B26B8(o);
    if (mode != 4) {
        FollowGetTargetObj(o->cond270);
        fn_800B261C(o, mode, &target);
        if (target != 0 && target != FollowGetTargetObj(o->cond270)) {
            return 1;
        }
        if (fn_800B22F8(o, mode) <= 2) {
            return 1;
        }
    }
    target = FollowGetTargetObj(o->cond270);
    if (target == 0) {
        return 1;
    }
    if (OwnerValue(o, target) < 600.0f || (ev->flags & 1) != 0) {
        if (o->unk288 != 0) {
            fn_800BBCA8(o);
            o->unk288 = 0;
        } else if ((ev->flags & 1) == 0) {
            ConditionUpdateBit(o->cond270, 4, 0);
            if (ActorPlayAnim(o->unk38, 0xfa658dd0, ((Owner*)target)->unk38) != 0) {
                return 1;
            }
            ConditionUpdateBit(o->cond270, 0x14, 1);
            ev->flags |= 1;
        } else if (MemFlagIsSet(o->cond270, 0x14) == 0) {
            return 1;
        }
    } else if (fn_800BAF40(o) == -1) {
        o->unk288 = 0;
    } else {
        o->unk288 = 2;
    }
    return 0;
}

void RangeResetPending(void* cond);
extern "C" void fn_800BC56C(void* owner, void* target);

// Clears the follow condition state: releases the pending range, resets the
// follow bits and drops the actor's stored follow status.
int NpcSveResetFollowCondition(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    fn_800BBCA8(o);
    RangeResetPending(o->cond270);
    ConditionUpdateBit(o->cond270, 4, 0);
    o->unk288 = 0;
    ev->flags &= ~1;
    return 1;
}

// Starts the follow state machine: switches the event block to the "following"
// state and retargets the follow condition block at the reported object.
int NpcSveStartFollowState(Owner* owner) {
    if (owner == 0) {
        return 0;
    }
    Owner* o = (Owner*)owner;
    PathEvent* ev = (PathEvent*)o->evt;
    void* target = FollowGetTargetObj(o->cond270);
    if (target == 0) {
        return 0;
    }
    o->unk288 = 0;
    ev->state = 7;
    ev->flags &= ~1;
    ConditionUpdateBit(o->cond270, 4, 1);
    fn_800BC56C(o, target);
    return 1;
}

// Message-0x400 reply payload: the host's follow state word.
struct SveFollowState {
    int unk00[6];
    u32 flags;                            // 0x18
};

extern "C" int fn_800B1ED4(void* owner, int mode);
extern "C" void fn_800E1098(float scale, Vec3* io, Vec3* in);
extern "C" void fn_800BC5A8(void* owner, Vec3* pos);
struct Actor;
int ActorAim(int* out, Actor* actor, int arg);
float ActorAimAngleToPos(void* actor, Vec3* pos);
float ActorAngleToPos(void* actor, Vec3* pos);
inline float fabsf(float x) { return __fabsf(x); }

// Per-frame step of the follow-target state machine: validates the reported
// target, keeps the follow timer and drives the follow animation.
int NpcSveUpdateFollow(Owner* owner) {
    Owner* o = (Owner*)owner;
    int mode;
    PathEvent* ev;
    Owner* target;
    Msg400 q;

    if (o == 0) {
        return 0;
    }
    ev = (PathEvent*)o->evt;
    mode = fn_800B26B8(o);
    if (mode != 4) {
        FollowGetTargetObj(o->cond270);
        fn_800B261C(o, mode, &target);
        if (target != 0 && target != FollowGetTargetObj(o->cond270)) {
            return 1;
        }
        if (fn_800B22F8(o, mode) <= 2) {
            return 1;
        }
        if ((fn_800B1ED4(o, mode) & 8) != 0) {
            return 1;
        }
    }
    target = (Owner*)FollowGetTargetObj(o->cond270);
    if (target == 0) {
        return 1;
    }
    SendMsg(target->unk38, 0x400, &q);
    if ((q.flags & 2) == 0) {
        return 1;
    }
    if ((OwnerValue(o, target) < 1500.0f && fabsf(ActorAimAngleToPos(target, &o->pos84) < 10.0f)) ||
        (ev->flags & 1) != 0) {
        if (o->unk288 != 0) {
            fn_800BBCA8(o);
            o->unk288 = 0;
        } else if ((ev->flags & 1) == 0) {
            ConditionUpdateBit(o->cond270, 4, 0);
            if (ActorPlayAnim(o->unk38, 0x1ce948a2, target->unk38) != 0) {
                return 1;
            }
            ConditionUpdateBit(o->cond270, 0x12, 1);
            ev->flags |= 1;
        } else if (MemFlagIsSet(o->cond270, 0x12) == 0) {
            return 1;
        }
    } else {
        if (ev->timer > 200.0f) {
            int aim[3];
            Vec3 to;
            Vec3 d;

            to.x = target->i24;
            to.y = target->i28;
            to.z = target->i2c;
            ActorAim(aim, (Actor*)target, 1);
            aim[2] = 0;
            d.x = aim[0];
            d.y = aim[1];
            d.z = aim[2];
            fn_800E1098(1350.0f, &d, &d);
            to.x += d.x;
            to.y += d.y;
            to.z += d.z;
            fn_800BC5A8(o, &to);
            ev->timer = 0.0f;
        }
        if (fn_800BAF40(o) == -1) {
            o->unk288 = 0;
        } else {
            o->unk288 = 2;
        }
    }
    return 0;
}

// Exit step of the follow state machine: releases the pending range, clears the
// follow bits and drops the stored follow status.
int NpcSveExitFollowTarget(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    fn_800BBCA8(o);
    RangeResetPending(o->cond270);
    ConditionUpdateBit(o->cond270, 4, 0);
    ConditionUpdateBit(o->cond270, 0x16, 0);
    o->unk288 = 0;
    ev->flags &= ~1;
    return 1;
}

extern "C" int fn_8020B97C(void* obj);
Owner* GetPlayer();

// Target object reported by FollowGetTargetObj: fixed-point position at
// 0x24..0x2c and the packed actor id at 0x38.
struct FollowTarget {
    u8 unk00[0x24];
    int x24;
    int y28;
    int z2c;
    u8 unk30[0x38 - 0x30];
    u32 unk38;
};

// Payload of the 0xc14 "target follow" message: animation result, the actor
// that produced it and the message tag.
struct SveTargetMsg {
    int unk00;
    int anim;
    int owner;
    u32 tag;
};

// Starts the target-follow phase once the follow query reports an object:
// resets the follow state and re-arms the condition bits used by the
// path-follow state machine.
int NpcSveStartTargetFollow(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    if (FollowGetTargetObj(o->cond270) == 0) {
        return 0;
    }
    o->unk288 = 0;
    ev->state = 6;
    ev->flags &= ~1;
    ConditionUpdateBit(o->cond270, 4, 1);
    ConditionUpdateBit(o->cond270, 3, 0);
    ConditionUpdateBit(o->cond270, 0x16, 1);
    return 1;
}

// Per-frame handler of the target-follow phase: if the followed object is
// still valid it plays the follow animation / checks the facing angle and
// timer, and reports whether the followed actor is still alive.
int NpcSveUpdateTargetFollow(Owner* owner) {
    Owner* o;
    PathEvent* ev;
    FollowTarget* target;
    Vec3 pos;

    if (owner == 0) {
        return 0;
    }
    o = (Owner*)owner;
    ev = (PathEvent*)o->evt;
    target = (FollowTarget*)FollowGetTargetObj(o->cond270);
    if (target == 0 || (int)(target->unk38 >> 24) != 2) {
        SveTargetMsg msg;
        int ownId = o->unk38;
        Owner* player = GetPlayer();

        if (player->unk38 != -1) {
            msg.owner = ownId;
            msg.tag = 0xaf699ed0;
            msg.anim = 0;
            SendMsg(player->unk38, 0xc14, &msg);
        }
        ev->flags |= 4;
        return 1;
    }
    if (MemFlagIsSet(o->cond270, 0x1f) != 0) {
        if ((ev->flags & 4) == 0) {
            SveTargetMsg msg;
            int anim;
            int id;

            ev->flags |= 4;
            anim = ActorPlayAnim(o->unk38, 0xdc35206a, target->unk38);
            id = o->unk38;
            if ((int)target->unk38 != -1) {
                msg.anim = anim;
                msg.owner = id;
                msg.tag = 0xaf699ed0;
                SendMsg(target->unk38, 0xc14, &msg);
            }
        }
    } else if (MemFlagIsSet(o->cond270, 4) == 1 && HasCondition(o->cond270, 0xd) == 0 &&
               HasCondition(o->cond270, 0xc) == 0) {
        pos.x = target->x24;
        pos.y = target->y28;
        pos.z = target->z2c;
        if (fabsf(ActorAngleToPos(o, &pos)) < 10.0f || ev->timer > 5000.0f) {
            ConditionUpdateBit(o->cond270, 4, 0);
            ev->flags |= 1;
            ev->timer = 0.0f;
        }
    } else if ((ev->flags & 4) == 0) {
        SveTargetMsg msg;
        int anim;
        int id;

        ev->flags |= 4;
        anim = ActorPlayAnim(o->unk38, 0xdc35206a, target->unk38);
        id = o->unk38;
        if ((int)target->unk38 != -1) {
            msg.anim = anim;
            msg.owner = id;
            msg.tag = 0xaf699ed0;
            SendMsg(target->unk38, 0xc14, &msg);
        }
        if (HasCondition(o->cond270, 0xd) == 0) {
            ConditionSetBit(o->cond270, 0x12, 1);
        }
    }
    return fn_8020B97C(target) == 0 ? 1 : 0;
}

// Clears the "target reached" event flag and releases the pending follow range;
// when the follow condition is no longer active the follow bits are dropped.
int NpcSveFinishTargetFollow(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    if ((ev->flags & 4) == 0) {
        return 0;
    }
    RangeResetPending(o->cond270);
    ev->flags &= ~4;
    if (MemFlagIsSet(o->cond270, 0x1f) == 0) {
        ConditionUpdateBit(o->cond270, 4, 0);
        ConditionUpdateBit(o->cond270, 0x16, 0);
    }
    return 1;
}

// Starts the follow-return phase: switches the event block to state 3, clears
// the follow flags and re-arms the follow conditions.
int NpcSveBeginTargetFollow(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    if (FollowGetTargetObj(o->cond270) == 0) {
        return 0;
    }
    o->unk288 = 0;
    ev->state = 3;
    ev->flags &= ~1;
    ev->flags &= ~4;
    ev->timer = 0.0f;
    if (MemFlagIsSet(o->cond270, 0x1f) == 0) {
        ConditionUpdateBit(o->cond270, 4, 1);
        ConditionUpdateBit(o->cond270, 3, 0);
        ConditionUpdateBit(o->cond270, 0x16, 1);
        if (o->unk220 == 4) {
            o->unk220 = 0;
        }
    }
    return 1;
}

extern "C" int fn_800B1EF4(void* owner, int mode);

// Per-frame step of the path-follow state machine: refreshes the reported
// target, restarts the follow timer while the follower is still close, and
// reports success once the timer runs out (switching the event to state 3).
int NpcSveRefreshFollowTarget(Owner* owner) {
    Owner* o = (Owner*)owner;
    if (o == 0) {
        return 0;
    }
    void* target;
    int mode;
    PathEvent* ev;
    ev = (PathEvent*)o->evt;
    mode = fn_800B26B8(o);
    if (mode != 4) {
        if (fn_800B1EF4(o, mode) == 0) {
            fn_800B261C(o, mode, &target);
            fn_800B22F8(o, mode);
            fn_800B1F20(o, mode);
        }
        if ((fn_800B1ED4(o, mode) & 0x10) == 0 || fn_800B22F8(o, mode) < 3) {
            ev->timer = 0.0f;
        }
    }
    if (ev->timer > 3000.0f) {
        o->unk268 = 3;
        return 1;
    }
    return 0;
}

// Switches the path-follow actor to state 3 and re-arms its follow conditions.
int NpcSveEnterStateThree(Owner* o) {
    if (o == 0) {
        return 0;
    }
    o->unk268 = 3;
    RangeResetPending(o->cond270);
    ConditionUpdateBit(o->cond270, 4, 0);
    ConditionSetBit(o->cond270, 0x1e, 0);
    return 1;
}

// Enters the first path-follow state: resets the event block, raises the follow
// condition bits and reports whether the movement query found a target object.
int NpcSveBeginPathFollow(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    o->unk288 = 0;
    ev->state = 1;
    ev->unk04 = 0.0f;
    ConditionUpdateBit(o->cond270, 4, 1);
    ConditionSetBit(o->cond270, 0x1e, 1);
    int mode = fn_800B26B8(o);
    if (mode == 4) {
        return 0;
    }
    u32 target[3];
    fn_800B261C(o, mode, target);
    return target[0] != 0;
}

// Per-frame update of the path-follow behaviour: tracks the movement query's
// target, drives the follow handler and keeps the path-timer condition bit.
int NpcSveUpdatePathFollow(Owner* owner) {
    Owner* o = (Owner*)owner;
    if (o == 0) {
        return 0;
    }
    int mode = fn_800B26B8(o);
    if (mode != 4) {
        int lowest = fn_800B22F8(o, mode);
        void* target;
        fn_800B261C(o, mode, &target);
        if (lowest != 4) {
            o->unk268 = lowest;
            if (MemFlagIsSet(o->cond270, 5) == 0) {
                return 1;
            }
        }
    }
    if (o->unk220 == 0 && MemFlagIsSet(o->cond270, 5) == 0) {
        return 1;
    }
    if (o->unk220 == 2) {
        o->fn10c = fn_800C748C;
        if (FollowGetTargetObj(o->cond270) != (void*)o->unk22c) {
            FollowSetTargetObj(o->cond270, (void*)o->unk22c);
            if (o->unk21c != 0) {
                o->unk288 = o->unk21c;
                fn_800BC56C(o, (void*)o->unk22c);
            }
        }
        if (fn_800BAF40(o) == -1 && o->unk21c != 0) {
            FollowSetTargetObj(o->cond270, 0);
            o->unk288 = 0;
        }
    } else {
        RangeResetPending(o->cond270);
        o->unk288 = 0;
    }
    float pathTime = fn_80115F68(o->unk38);
    if (pathTime < 1.0f) {
        ConditionUpdateBit(o->cond270, 9, 1);
    } else {
        ConditionUpdateBit(o->cond270, 9, 0);
    }
    return 0;
}

void FollowOffsetUpdate(void* self);
extern "C" u32 fn_8008DCDC(void* agent, int a, int b, int c);

// Behaviour handler: makes the follow-offset update the actor's per-frame
// handler; drops the follow sub-state when the agent reports flag 0x10, and
// releases condition 13 if it is set.
int NpcFollowOffsetSetup(void* p) {
    Owner* o = (Owner*)p;
    if (o == 0) {
        return 0;
    }
    o->fn10c = FollowOffsetUpdate;
    if (o->unk220 == 4 && (fn_8008DCDC(o->unkD4, 0, 0, 0) & 0x10) != 0) {
        o->unk220 = 0;
    }
    if (HasCondition(o->cond270, 0xd) != 0) {
        ConditionSetBit(o->cond270, 0xd, 0);
    }
    return 1;
}

// Behaviour handler: resets the actor's event/stream state to 2.
int NpcFollowClearState(void* p) {
    Owner* o = (Owner*)p;
    if (o == 0) {
        return 0;
    }
    *o->evt = 2;
    return 1;
}

void ActorAimOffset3D(void* owner, float* pos, float* dist, float* height);

// Per-frame update of the follow-offset query: advances the follow timer, aims
// at the object reported by the movement query and re-targets or drops the
// follow range depending on how far the object is. Condition bit 9 mirrors the
// path timer.
int NpcSveUpdateFollowAim(void* p) {
    Owner* o = (Owner*)p;
    PathEvent* ev;
    int mode;
    FollowTarget* target;
    float yaw;
    float y;
    float dist;
    Vec3 pos;

    if (o == 0) {
        return 0;
    }
    ev = (PathEvent*)o->evt;
    if (o->unk220 != 0) {
        return 1;
    }
    mode = fn_800B26B8(o);
    ev->unk04 -= o->f138;
    if (mode != 4) {
        if (fn_800B1EF4(o, mode) == 0) {
            return 1;
        }
        yaw = 3.4028235e38f;
        dist = 3.4028235e38f;
        fn_800B261C(o, mode, &target);
        if (target != 0) {
            pos.x = target->x24;
            pos.y = target->y28;
            pos.z = target->z2c;
            ActorAimOffset3D(o, &pos.x, &yaw, &dist);
        }
        y = yaw;
        if (fabsf(y) < 45.0f && dist < 3000.0f) {
            FollowSetTargetObj(o->cond270, target);
        } else if (fabsf(y) > 70.0f || dist > 6000.0f) {
            RangeResetPending(o->cond270);
        }
    }
    fn_800BBCA8(o);
    if (fn_80115F68(o->unk38) < 1.0f) {
        ConditionUpdateBit(o->cond270, 9, 1);
    } else {
        ConditionUpdateBit(o->cond270, 9, 0);
    }
    return 0;
}

// Clears the target-follow condition bit; returns whether the owner is valid.
int NpcSveClearTargetFollow(void* p) {
    Owner* o = (Owner*)p;
    if (o == 0) {
        return 0;
    }
    ConditionUpdateBit(o->cond270, 0x16, 0);
    return 1;
}

void ConditionUpdateBit(void* cond, int id, int value);

// Clears the event block and arms one of its condition bits; returns whether
// the actor pointer was non-null.
extern "C" int NpcSveResetEvent(int owner) {
    if (owner == 0) {
        return 0;
    }
    PathEvent* evt = *(PathEvent**)(owner + 0x130);
    *(int*)(owner + 0x288) = 0;
    evt->state = 0;
    evt->unk04 = 0.0f;
    ConditionUpdateBit((void*)(owner + 0x270), 0x16, 1);
    return 1;
}

extern "C" int fn_800F7A90(void* handle);
extern "C" void fn_800B1AB8(float value, void* owner, int mode, int arg);

// Chooses the actor's next state from its follow mode, the condition block and
// the distance to the follow target; -1/1/0/2/4/5 are the state codes.
extern "C" int fn_8029E058(void* actor) {
    Owner* o;
    int mode;
    u32 flags;
    int n;
    float dist;
    void* target;

    if (actor == 0) {
        return 0;
    }
    o = (Owner*)actor;
    if (MemFlagIsSet(o->cond270, 5)) {
        return 2;
    }
    mode = fn_800B26B8(o);
    fn_800B261C(o, mode, &target);
    flags = fn_800B1ED4(o, mode);
    if (mode != 4 && target != 0) {
        dist = OwnerValue(o, target);
        n = fn_800B22F8(o, mode);
        if (n <= 2) {
            if (fn_800F7A90(*(void**)((u8*)o + 0x8f0)) == -0x679f49e4) {
                return -1;
            }
            return 1;
        }
        if ((flags & 8) && dist < 5000.0f) {
            fn_800B1AB8(-1.0f, o, mode, 3);
            return 5;
        }
        if ((flags & 4) && dist < 5000.0f && MemFlagIsSet(o->cond270, 0x27) == 0) {
            return 4;
        }
        if (!(flags & 0x10) && !(flags & 0x40) && !(flags & 0x20) && dist < 1500.0f &&
            fabsf(ActorAimAngleToPos(target, &o->pos84)) < 10.0f) {
            o->unk268 = 1;
            return 1;
        }
        if (n == 3 && fn_800B1EF4(o, mode) == 0) {
            fn_800B1F20(o, mode);
            return 0;
        }
        fn_800B1F20(o, mode);
    }
    return o->unk220 != 0 ? 2 : 0;
}

// Switches the path-follow actor to follow state 7: asks the current state's
// exit handler whether the actor may leave and then runs the follow state's
// entry handler. Only actors of type 0x12ec1 own this state machine.
void NpcSveAdvanceToFollowState(Owner* owner) {
    PathEvent* ev;
    if (owner == 0) {
        return;
    }
    Owner* o = (Owner*)owner;
    ev = (PathEvent*)o->evt;
    if (ev == 0) {
        return;
    }
    if (o->unk128 != 0x12ec1) {
        return;
    }
    FollowSetTargetObj(o->cond270);
    int state = ev->state;
    if (state != 7 && gNpcSveTypes[state].fn4(o) != 0) {
        gNpcSveTypes[7].fn0(o);
    }
}

// Switches the path-follow actor to target-follow state 6, with the same
// leave/enter handshake as NpcSveAdvanceToFollowState.
void NpcSveAdvanceToTargetFollowState(Owner* owner) {
    PathEvent* ev;
    if (owner == 0) {
        return;
    }
    Owner* o = (Owner*)owner;
    ev = (PathEvent*)o->evt;
    if (ev == 0) {
        return;
    }
    if (o->unk128 != 0x12ec1) {
        return;
    }
    FollowSetTargetObj(o->cond270);
    int state = ev->state;
    if (state != 6 && gNpcSveTypes[state].fn4(o) != 0) {
        gNpcSveTypes[6].fn0(o);
    }
}

// ---------------------------------------------------------------------------
// 0x40a: replies to a state-animation query. Sends the 0x400 "who am I" query
// back to the requesting object, then plays the animation named by the type
// hash and reports the new follow state.
// ---------------------------------------------------------------------------

// Object reached through the message's first word: its 0x30 entry is the
// "send query" handler.
struct QuerySink {
    u8 unk00[0x30];
    void (*query)(int id, void* args);    // 0x30
};

// Payload of the 0x400 query: the requesting object and the actor id.
struct QueryArgs {
    int object;                           // 0x00
    int id;                               // 0x04
};

// Reply payload posted back when the state animation starts. The receiver does
// not look at unk00, so it is left as it was on the stack.
struct SveAnimReply {
    int unk00;                            // 0x00
    int zero;                             // 0x04
    int state;                            // 0x08
    int minusOne;                         // 0x0c
    int unk10;                            // 0x10
};

// Per-host slot of the local player: the hosted object's type tag.
struct PlayerHostSlot {
    u32 type;                             // 0x00
};

struct PlayerMode {
    u8 unk00[0x38];
    u32 flags;                            // 0x38
};

// The local player, seen through the fields this query reads.
struct PlayerActor {
    u8 unk000[0x26c4];
    PlayerHostSlot* hosts[8];             // 0x26c4: per-host slot table
    PlayerMode* mode;                     // 0x26e4
};

extern "C" int fn_80049940();
int ActorAnimState(int id, u32 state);
int PlayerInHost(Actor* actor);

extern "C" int NpcSvePlayAnimMsg(Owner* a, Msg40A* m) {
    Msg400 q;
    SveAnimReply r;
    u32 h;
    PlayerActor* pl;

    q.actor = m->actor;
    q.ownerId = OwnerId(a);
    ((QuerySink*)m->actor)->query(0x400, &q);
    h = StrHash("QUERY", m->anim);
    if (ActorPlayAnim(OwnerId(a), h, m->unk04) != 0) {
        m->result = 0;
        return 1;
    }
    if (HasCondition(a->cond270, 0xA) || HasCondition(a->cond270, 9)) {
        return 0;
    }
    if (fn_80049940()) {
        if (m->anim == 0x7C44EC3D) {
            m->result = 0xA53F7E23;
            return 1;
        }
        return 0;
    }
    if (MemFlagIsSet(a->cond270, 0xB)) {
        return 1;
    }
    if (m->anim == 0x3CA34BD6 && MemFlagIsSet(a->cond270, 0x20) == 0) {
        m->result = 0x1379C466;
        return 1;
    }
    if (HasCondition(a->cond270, 0xB)) {
        return 0;
    }
    if (ActorAnimState(a->unk38, 0xDC35206A) == 0) {
        return 0;
    }
    pl = (PlayerActor*)GetPlayer();
    if (pl == 0) {
        return 0;
    }
    if (pl->mode != 0 && (int)(pl->mode->flags >> 24) == 1) {
        return 0;
    }
    if (pl->hosts[PlayerInHost((Actor*)pl)]->type == 0x04624485 ||
        pl->hosts[PlayerInHost((Actor*)pl)]->type == 0x13C05 ||
        pl->hosts[PlayerInHost((Actor*)pl)]->type == 0xE0AB9B4F ||
        pl->hosts[PlayerInHost((Actor*)pl)]->type == 0xFFF5) {
        return 0;
    }
    if (MemFlagIsSet(a->cond270, 0x1E)) {
        return 0;
    }
    if (m->anim == 0x7C44EC3D) {
        int kind = fn_800B26B8(a);

        if (kind != 4 && fn_800B22F8(a, kind) <= 1) {
            return 0;
        }
    }
    {
        r.zero = 0;
        r.minusOne = -1;
        m->result = 0x272CAC;
        r.state = m->result;
        SendMsg(m->unk04, 0x40B, &r);
    }
    return 1;
}

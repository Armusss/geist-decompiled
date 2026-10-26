// u_8029C0AC.cpp: NPC / path-follow actor ("Sve") message handlers and follow states (0x8029C0AC-0x8029E714).
// Names are descriptive guesses.

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
    u32 count = (u32)((u8*)end + 11 - (u8*)begin) / 12;
    if (begin >= end) {
        return;
    }
    do {
        count--;
    } while (count != 0);
}

// ---------------------------------------------------------------------------
// Shared declarations for the NPC/"SVE" actor message handlers below.
// ---------------------------------------------------------------------------

extern u8 __upper_map[];
extern char lbl_8034B818[];
extern float lbl_804DA778;
extern float lbl_804DA77C;
extern float lbl_804DA780;
extern float lbl_804DA784;
extern int lbl_804D3848;

struct Owner;

// 16-byte per-type descriptor entry; the handler table lives at lbl_803C3188.
struct TypeInfo {
    int (*fn0)(Owner*);
    int (*fn4)(Owner*);
    int (*fn8)(Owner*);
    const char* name;
};

extern TypeInfo lbl_803C3188[];

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

struct Vec3 {
    float x;
    float y;
    float z;
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

class Buffer {
public:
    virtual ~Buffer();
    char* data;                           // 0x04
    int length;                           // 0x08
    int capacity;                         // 0x0c
    int flags;                            // 0x10
};

class String : public Buffer {
public:
    String(const char* s);
    String();
};

int OwnerId(Owner* owner);
int ActorPlayAnim(int id, u32 anim, int arg);
void FollowSetTargetObj(void* self, void* target);
// One-argument form: the state-switch helpers below call 0x800A78A0 with a
// single argument (r4 is never set before the bl), so they bind to this one.
void FollowSetTargetObj(void* self);

extern "C" int fn_8029E3B0(void* actor);
extern "C" String fn_800225C8(const String& a, const String& b);
extern "C" void fn_8028C188(void* out, const char* s, int length);
extern "C" void* memcpy(void* dst, const void* src, u32 n);

// Character mapping used by the type-name hash (0xff maps to itself).
static inline u32 SveHashChar(char c) {
    if (c == -1) {
        return 0xffffffff;
    }
    return __upper_map[(u8)c];
}

// Lazy type-name hash: h = h * 31 + upper(c), modulo 0xffffffff.
static inline u32 SveStrHash(const char* name) {
    u32 h = 0;
    if (name != 0) {
        for (int i = 0; name[i] != 0; i++) {
            h *= 31;
            h += SveHashChar(name[i]) & 0xff;
        }
    }
    return h % 0xffffffff;
}

// Builds the state-name reply String (type name + separator) into out.
static inline void SveStateName(int* out, const char* name) {
    String part(name);
    String sep(lbl_8034B818 + 0x51);
    String full = fn_800225C8(part, sep);
    fn_8028C188(out, full.data, full.length);
}

int NpcSveHandleMsg(int msg, int* p) {
    Owner* o = (Owner*)*p;

    switch (msg) {
    case 0x400:
        p[1] = OwnerId(o);
        p[3] = o->f904 > lbl_804DA778;
        *(float*)&p[4] = lbl_804DA77C;
        *(float*)&p[5] = lbl_804DA77C;
        p[6] = 2;
        p[7] = 0;
        return 1;

    case 0x409: {
        int* type = o->evt;
        int id = OwnerId(o);
        if (ActorPlayAnim(id, p[3], p[2]) != 0) {
            return 1;
        }
        if (p[3] != 0x272cac) {
            return 0;
        }
        if (lbl_803C3188[*type].fn4(o) != 0) {
            FollowSetTargetObj((u8*)o + 0x270, (void*)p[1]);
            if (lbl_803C3188[3].fn0(o) == 0) {
                lbl_803C3188[*type].fn0(o);
            }
        }
        return 1;
    }

    case 0x40a:
        return fn_8029E3B0(o);

    case 0x41e:
        switch (p[1]) {
        case -0x527a8906:                 // 0xad8576fa
            SveStateName(&p[3], lbl_803C3188[*o->evt].name);
            return 0;
        case -0x7cd20af6:                 // 0x832ef50a
            p[3] = lbl_804D3848;
            return 1;
        }
        return 0;

    case 0x7102: {
        int* evt = o->evt;
        if (evt == 0) {
            return 1;
        }
        if (p[1] < 0x14) {
            p[4] = 0;
            return 1;
        }
        u32 h = SveStrHash(lbl_8034B818 + 0x48);
        memcpy((void*)p[2], &h, 4);
        p[3] += 4;
        p[1] -= 4;
        p[2] += 4;
        memcpy((void*)p[2], evt, 0x10);
        p[3] += 0x10;
        p[1] -= 0x10;
        p[2] += 0x10;
        return 1;
    }

    case 0x7103: {
        int* evt = o->evt;
        if (evt == 0) {
            return 1;
        }
        if (p[3] < 0x14) {
            p[4] = 0;
            return 1;
        }
        int ok;
        if (p[3] < 4) {
            ok = 0;
        } else {
            u32* data = (u32*)p[2];
            p[3] += 4;
            p[1] -= 4;
            p[2] += 4;
            ok = (*data == SveStrHash(lbl_8034B818 + 0x48));
        }
        if (!ok) {
            return 1;
        }
        memcpy(evt, (void*)p[2], 0x10);
        p[3] -= 0x10;
        p[2] += 0x10;
        return 1;
    }

    default:
        return 0;
    }
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
int NpcSveUpdateActor(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* evt = (PathEvent*)o->evt;
    if (HasCondition(o->cond270, 0xa) != 0 ||
        HasCondition(o->cond270, 0xb) != 0) {
        fn_80162658(o, o->unk894);
        return 1;
    }
    if (fn_800BEF9C(o->unk2c4) < o->unk268) {
        o->unk268 = fn_800BEF9C(o->unk2c4);
    }
    int ret = lbl_803C3188[evt->state].fn8(o);
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
            lbl_803C3188[evt->state].fn4(o) != 0) {
            lbl_803C3188[next].fn0(o);
            evt->timer = lbl_804DA778;
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
    ev->unk04 = lbl_804DA778;
    ev->state = 0;
    ev->flags = 0;
    ev->timer = lbl_804DA778;
    lbl_804D3848 = AssetFind(0xf1754bf2, 0x21c5e0);
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
int NpcSveCheckTarget(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    int mode = fn_800B26B8(o);
    if (mode != 4) {
        u32 target[3];
        fn_800B261C(o, mode, target);
        if (target[0] != (u32)FollowGetTargetObj(o->cond270)) {
            return 1;
        }
        mode = fn_800B22F8(o, mode);
        if (mode <= 2) {
            return 1;
        }
    }
    return lbl_804DA780 < ev->timer;
}

void ConditionUpdateBit(void* cond, int id, int value);

// Tells whether the owner is a live actor, after clearing the follow-condition
// bits. Used as the "stop following" step of the path-follow state machine.
bool NpcSveStopFollow(Owner* o) {
    if (o != 0) {
        ConditionUpdateBit(o->cond270, 4, 0);
        ConditionUpdateBit(o->cond270, 9, 0);
    }
    return o != 0;
}

extern "C" void SendMsg(int to, int msg, void* data);
extern "C" void fn_800B1F20(void* owner, int mode);

// Payload of message 0x41d: the follow-behaviour type hash and the entity name.
struct SveBeginMsg {
    int unk00;
    u32 hash;
    int unk08;
    String text;
};

// Picks up the object reported by the movement query, plays its follow
// animation and tells the host that the follow behaviour has started.
int NpcSveBeginFollow(Owner* o) {
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
    if (ActorPlayAnim(o->unk38, 0xb1c7633c, ((Owner*)target)->unk38) != 0) {
        return 0;
    }
    fn_800B1F20(o, mode);
    FollowSetTargetObj(o->cond270, target);
    o->unk288 = 0;
    o->unk268 = 3;
    ev->state = 5;
    ev->unk04 = lbl_804DA778;
    ConditionUpdateBit(o->cond270, 4, 1);
    ConditionUpdateBit(o->cond270, 9, 1);
    FollowSetTargetObj(o->cond270, target);
    SveBeginMsg msg;
    msg.hash = 0xf9d0235b;
    msg.unk08 = 0;
    SendMsg(o->unk38, 0x41d, &msg);
    return 1;
}

// Variant of NpcSveCheckTarget used by the follow state machine: also requires
// the movement query to report an active follow mode and a running path timer.
int NpcSveCheckFollowReady(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    int mode = fn_800B26B8(o);
    if (mode == 4) {
        return 1;
    }
    u32 target[3];
    fn_800B261C(o, mode, target);
    if (target[0] != (u32)FollowGetTargetObj(o->cond270)) {
        return 1;
    }
    mode = fn_800B22F8(o, mode);
    if (mode <= 2) {
        return 1;
    }
    return ev->timer > lbl_804DA780;
}

// Clears the follow-condition bits and reports whether the owner was valid;
// used as the "cancel following" step of the path-follow state machine.
bool NpcSveCancelFollow(Owner* o) {
    if (o != 0) {
        ConditionUpdateBit(o->cond270, 4, 0);
        ConditionUpdateBit(o->cond270, 9, 0);
    }
    return o != 0;
}

void NpcSveAdvanceToTargetFollowState(Owner* o);
void OwnerSlotAdd(Owner* o, int mode, u32 handler, u32 target, u32 size);
float OwnerValue(Owner* o, void* arg);
int MemFlagIsSet(void* cond, int id);
extern "C" void fn_800BBCA8(void* owner);
extern "C" int fn_800BAF40(void* owner);

// Starts following the object reported by the movement query: plays the follow
// animation, retargets the follow condition block and queues the follow update.
int NpcSveStartFollowTarget(Owner* o) {
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
    ev->unk04 = lbl_804DA778;
    ConditionUpdateBit(o->cond270, 4, 1);
    ConditionUpdateBit(o->cond270, 9, 1);
    FollowSetTargetObj(o->cond270, target);
    OwnerSlotAdd(o, mode, (u32)NpcSveAdvanceToTargetFollowState, (u32)target,
                 0x28);
    return 1;
}

// Second half of the follow state machine: validates the reported follow target,
// watches the path timer and drives the follow animation / host callback.
int NpcSveUpdateFollowTarget(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    int mode = fn_800B26B8(o);
    if (mode != 4) {
        FollowGetTargetObj(o->cond270);
        void* reported;
        fn_800B261C(o, mode, &reported);
        if (reported != 0 && reported != FollowGetTargetObj(o->cond270)) {
            return 1;
        }
        if (fn_800B22F8(o, mode) <= 2) {
            return 1;
        }
    }
    void* target = FollowGetTargetObj(o->cond270);
    if (target == 0) {
        return 1;
    }
    if (OwnerValue(o, 0) < lbl_804DA784 || (ev->flags & 1) != 0) {
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
        } else {
            if (MemFlagIsSet(o->cond270, 0x14) == 0) {
                return 1;
            }
        }
    } else {
        if (fn_800BAF40(o) == -1) {
            o->unk288 = 0;
        } else {
            o->unk288 = 2;
        }
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
int NpcSveStartFollowState(Owner* o) {
    if (o == 0) {
        return 0;
    }
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

extern float lbl_804DA788;
extern float lbl_804DA78C;
extern float lbl_804DA790;
extern float lbl_804DA794;
extern double lbl_804DA798;
extern double lbl_804DA7A0;

extern "C" int fn_800B1ED4(void* owner, int mode);
extern "C" void fn_800E1098(float scale, Vec3* io, Vec3* in);
extern "C" void fn_800BC5A8(void* owner, Vec3* pos);
struct Actor;
int ActorAim(int* out, Actor* actor, int arg);
float ActorAimAngleToPos(void* actor, Vec3* pos);
float ActorAngleToPos(void* actor, Vec3* pos);
float fabsf(float x);

// Per-frame step of the follow-target state machine: validates the reported
// target, keeps the follow timer and drives the follow animation.
int NpcSveUpdateFollow(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    int active = 0;
    int mode = fn_800B26B8(o);
    if (mode != 4) {
        void* reported;
        FollowGetTargetObj(o->cond270);
        fn_800B261C(o, mode, &reported);
        if (reported != 0 && reported != FollowGetTargetObj(o->cond270)) {
            return 1;
        }
        if (fn_800B22F8(o, mode) <= 2) {
            return 1;
        }
        if ((fn_800B1ED4(o, mode) & 8) != 0) {
            return 1;
        }
    }
    Owner* target = (Owner*)FollowGetTargetObj(o->cond270);
    if (target == 0) {
        return 1;
    }
    SveFollowState st;
    SendMsg(target->unk38, 0x400, &st);
    if ((st.flags & 2) == 0) {
        return 1;
    }
    if (OwnerValue(o, target) < lbl_804DA788) {
        float angle = ActorAimAngleToPos(target, &o->pos84);
        active = fabsf((float)(angle < lbl_804DA78C)) != lbl_804DA778;
    }
    if (active == 0) {
        if ((ev->flags & 1) == 0) {
            if (lbl_804DA790 < ev->timer) {
                Vec3 pos;
                pos.x = (float)target->i24;
                pos.y = (float)target->i28;
                pos.z = (float)target->i2c;
                int dir[2];
                ActorAim(dir, (Actor*)target, 1);
                Vec3 off;
                off.x = (float)dir[0];
                off.y = (float)dir[1];
                off.z = 0.0f;
                fn_800E1098(lbl_804DA794, &off, &off);
                pos.x += off.x;
                pos.y += off.y;
                pos.z += off.z;
                fn_800BC5A8(o, &pos);
                ev->timer = lbl_804DA778;
            }
            if (fn_800BAF40(o) == -1) {
                o->unk288 = 0;
                return 0;
            }
            o->unk288 = 2;
            return 0;
        }
    }
    if (o->unk288 == 0) {
        if ((ev->flags & 1) == 0) {
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
        fn_800BBCA8(o);
        o->unk288 = 0;
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
extern float lbl_804DA7A8;
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
int NpcSveUpdateTargetFollow(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    FollowTarget* target = (FollowTarget*)FollowGetTargetObj(o->cond270);
    if (target == 0 || (target->unk38 >> 24) != 2) {
        int ownId = o->unk38;
        Owner* player = GetPlayer();
        if (player->unk38 != -1) {
            SveTargetMsg msg;
            msg.anim = 0;
            msg.owner = ownId;
            msg.tag = 0xaf699ed0;
            SendMsg(player->unk38, 0xc14, &msg);
        }
        ev->flags |= 4;
        return 1;
    }
    if (MemFlagIsSet(o->cond270, 0x1f) != 0) {
        if ((ev->flags & 4) == 0) {
            ev->flags |= 4;
            int anim = ActorPlayAnim(o->unk38, 0xdc35206a, target->unk38);
            if (target->unk38 != (u32)-1) {
                SveTargetMsg msg;
                msg.anim = anim;
                msg.owner = o->unk38;
                msg.tag = 0xaf699ed0;
                SendMsg(target->unk38, 0xc14, &msg);
            }
        }
    } else if (MemFlagIsSet(o->cond270, 4) == 1 &&
               HasCondition(o->cond270, 0xd) == 0 &&
               HasCondition(o->cond270, 0xc) == 0) {
        Vec3 pos;
        pos.x = (float)target->x24;
        pos.y = (float)target->y28;
        pos.z = (float)target->z2c;
        if (fabsf(ActorAngleToPos(o, &pos)) < lbl_804DA78C ||
            ev->timer > lbl_804DA7A8) {
            ConditionUpdateBit(o->cond270, 4, 0);
            ev->flags |= 1;
            ev->timer = lbl_804DA778;
        }
    } else if ((ev->flags & 4) == 0) {
        ev->flags |= 4;
        int anim = ActorPlayAnim(o->unk38, 0xdc35206a, target->unk38);
        if (target->unk38 != (u32)-1) {
            SveTargetMsg msg;
            msg.anim = anim;
            msg.owner = o->unk38;
            msg.tag = 0xaf699ed0;
            SendMsg(target->unk38, 0xc14, &msg);
        }
        if (HasCondition(o->cond270, 0xd) == 0) {
            ConditionSetBit(o->cond270, 0x12, 1);
        }
    }
    return fn_8020B97C(target) == 0;
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
    ev->timer = lbl_804DA778;
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
int NpcSveRefreshFollowTarget(Owner* o) {
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    int mode = fn_800B26B8(o);
    if (mode != 4) {
        u32 target[3];
        if (fn_800B1EF4(o, mode) == 0) {
            fn_800B261C(o, mode, target);
            fn_800B22F8(o, mode);
            fn_800B1F20(o, mode);
        }
        if ((fn_800B1ED4(o, mode) & 0x10) == 0 || fn_800B22F8(o, mode) < 3) {
            ev->timer = lbl_804DA778;
        }
    }
    if (ev->timer <= lbl_804DA780) {
        return 0;
    }
    o->unk268 = 3;
    return 1;
}

// Switches the path-follow actor to state 3 and re-arms its follow conditions.
bool NpcSveEnterStateThree(Owner* o) {
    if (o != 0) {
        o->unk268 = 3;
        RangeResetPending(o->cond270);
        ConditionUpdateBit(o->cond270, 4, 0);
        ConditionSetBit(o->cond270, 0x1e, 0);
    }
    return o != 0;
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
    ev->unk04 = lbl_804DA778;
    ConditionUpdateBit(o->cond270, 4, 1);
    ConditionSetBit(o->cond270, 0x1e, 1);
    int mode = fn_800B26B8(o);
    if (mode == 4) {
        return 0;
    }
    u32 target[5];
    fn_800B261C(o, mode, target);
    return target[0] != 0;
}

// Per-frame update of the path-follow behaviour: tracks the movement query's
// target, drives the follow handler and keeps the path-timer condition bit.
int NpcSveUpdatePathFollow(Owner* o) {
    if (o == 0) {
        return 0;
    }
    int mode = fn_800B26B8(o);
    if (mode != 4) {
        int lowest = fn_800B22F8(o, mode);
        u8 target[12];
        fn_800B261C(o, mode, target);
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
    if (pathTime < lbl_804DA77C) {
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

extern float lbl_804DA7AC;
extern float lbl_804DA7B0;
extern float lbl_804DA7B4;
extern float lbl_804DA7B8;
void ActorAimOffset3D(void* owner, float* pos, float* dist, float* height);

// Per-frame update of the follow-offset query: advances the follow timer, aims
// at the object reported by the movement query and re-targets or drops the
// follow range depending on how far the object is. Condition bit 9 mirrors the
// path timer.
int NpcSveUpdateFollowAim(void* p) {
    Owner* o = (Owner*)p;
    if (o == 0) {
        return 0;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    if (o->unk220 != 0) {
        return 1;
    }
    int mode = fn_800B26B8(o);
    ev->timer = ev->timer - o->f138;
    if (mode != 4) {
        if (fn_800B1EF4(o, mode) == 0) {
            return 1;
        }
        float height = lbl_804DA7AC;
        float dist = lbl_804DA7AC;
        FollowTarget* target = 0;
        fn_800B261C(o, mode, &target);
        Vec3 aimPos;
        if (target != 0) {
            aimPos.x = (float)target->x24;
            aimPos.y = (float)target->y28;
            aimPos.z = (float)target->z2c;
            ActorAimOffset3D(o, &aimPos.x, &dist, &height);
        }
        if (fabsf(dist) < lbl_804DA7B0 && height < lbl_804DA780) {
            FollowSetTargetObj(o->cond270, target);
        } else if (fabsf(dist) > lbl_804DA7B4 || height > lbl_804DA7B8) {
            RangeResetPending(o->cond270);
        }
    }
    fn_800BBCA8(o);
    float pathTime = fn_80115F68(o->unk38);
    if (pathTime < lbl_804DA77C) {
        ConditionUpdateBit(o->cond270, 9, 1);
    } else {
        ConditionUpdateBit(o->cond270, 9, 0);
    }
    return 0;
}

// Clears the target-follow condition bit; returns whether the owner is valid.
int NpcSveClearTargetFollow(void* p) {
    Owner* o = (Owner*)p;
    if (o != 0) {
        ConditionUpdateBit(o->cond270, 0x16, 0);
    }
    return o != 0;
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
    evt->unk04 = lbl_804DA778;
    ConditionUpdateBit((void*)(owner + 0x270), 0x16, 1);
    return 1;
}

extern "C" int fn_800F7A90(void* handle);
extern "C" void fn_800B1AB8(float value, void* owner, int mode, int arg);
extern float lbl_804DA7BC;

// Chooses the actor's next state from its follow mode, the condition block and
// the distance to the follow target; -1/1/0/2/4/5 are the state codes.
extern "C" int fn_8029E058(void* actor) {
    Owner* o = (Owner*)actor;
    if (o == 0) {
        return 0;
    }
    if (MemFlagIsSet(o->cond270, 5) != 0) {
        return 2;
    }
    int mode = fn_800B26B8(o);
    int target[2];
    fn_800B261C(o, mode, target);
    int flags = fn_800B1ED4(o, mode);
    if (mode != 4 && target[0] != 0) {
        float dist = OwnerValue(o, (void*)target[0]);
        int rank = fn_800B22F8(o, mode);
        if (rank <= 2) {
            if (fn_800F7A90(*(void**)((u8*)o + 0x8f0)) == -0x679f49e4) {
                return -1;
            }
            return 1;
        }
        if ((flags & 8) != 0 && dist < lbl_804DA7A8) {
            fn_800B1AB8(lbl_804DA7BC, o, mode, 3);
            return 5;
        }
        if ((flags & 4) != 0 && dist < lbl_804DA7A8 &&
            MemFlagIsSet(o->cond270, 0x27) == 0) {
            return 4;
        }
        if ((flags & 0x10) == 0 && (flags & 0x40) == 0 &&
            (flags & 0x20) == 0 && dist < lbl_804DA788 &&
            fabsf(ActorAimAngleToPos((void*)target[0], &o->pos84)) <
                lbl_804DA78C) {
            o->unk268 = 1;
            return 1;
        }
        if (rank == 3 && fn_800B1EF4(o, mode) == 0) {
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
void NpcSveAdvanceToFollowState(Owner* o) {
    if (o == 0) {
        return;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    if (ev == 0) {
        return;
    }
    if (o->unk128 != 0x12ec1) {
        return;
    }
    FollowSetTargetObj(o->cond270);
    int state = ev->state;
    if (state != 7 && lbl_803C3188[state].fn4(o) != 0) {
        lbl_803C3188[7].fn0(o);
    }
}

// Switches the path-follow actor to target-follow state 6, with the same
// leave/enter handshake as NpcSveAdvanceToFollowState.
void NpcSveAdvanceToTargetFollowState(Owner* o) {
    if (o == 0) {
        return;
    }
    PathEvent* ev = (PathEvent*)o->evt;
    if (ev == 0) {
        return;
    }
    if (o->unk128 != 0x12ec1) {
        return;
    }
    FollowSetTargetObj(o->cond270);
    int state = ev->state;
    if (state != 6 && lbl_803C3188[state].fn4(o) != 0) {
        lbl_803C3188[6].fn0(o);
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
    PlayerHostSlot** hosts;               // 0x26c4: per-host slot table
    u8 unk26c8[0x26e4 - 0x26c8];
    PlayerMode* mode;                     // 0x26e4
};

// Seeded form of the type-name hash: the message carries a partial hash that
// the name characters are folded into.
static inline u32 SveStrHashSeeded(const char* name, u32 h) {
    if (name == 0) {
        return 0;
    }
    for (int i = 0; name[i] != 0; i++) {
        h *= 31;
        h += SveHashChar(name[i]) & 0xff;
    }
    return h % 0xffffffff;
}

extern "C" int fn_80049940();
int ActorAnimState(int id, u32 state);
int PlayerInHost(Actor* actor);

extern "C" int NpcSvePlayAnimMsg(void* owner, int* msg) {
    Owner* o = (Owner*)owner;

    QueryArgs args;
    args.object = *msg;
    args.id = OwnerId(o);
    ((QuerySink*)*msg)->query(0x400, &args);

    u32 hash = SveStrHashSeeded(lbl_8034B818 + 0x53, (u32)msg[2]);
    if (ActorPlayAnim(OwnerId(o), hash, msg[1]) != 0) {
        msg[3] = 0;
        return 1;
    }
    if (HasCondition(o->cond270, 0xa) != 0 ||
        HasCondition(o->cond270, 0x9) != 0) {
        return 0;
    }
    if (fn_80049940() != 0) {
        if (msg[2] == 0x7c44ec3d) {
            msg[3] = -0x5ac081dd;
            return 1;
        }
        return 0;
    }
    if (MemFlagIsSet(o->cond270, 0xb) != 0) {
        return 1;
    }
    if (msg[2] == (int)0x3ca34bd6 && MemFlagIsSet(o->cond270, 0x20) == 0) {
        msg[3] = 0x1379c466;
        return 1;
    }
    if (HasCondition(o->cond270, 0xb) != 0) {
        return 0;
    }
    if (ActorAnimState(o->unk38, 0xdc35206a) == 0) {
        return 0;
    }
    PlayerActor* pl = (PlayerActor*)GetPlayer();
    if (pl == 0) {
        return 0;
    }
    if (pl->mode != 0 && (pl->mode->flags >> 24) == 1) {
        return 0;
    }
    if (pl->hosts[PlayerInHost((Actor*)pl)]->type == 0x4624485 ||
        pl->hosts[PlayerInHost((Actor*)pl)]->type == 0x13c05 ||
        pl->hosts[PlayerInHost((Actor*)pl)]->type == (u32)-0x1f5464b1 ||
        pl->hosts[PlayerInHost((Actor*)pl)]->type == 0xfff5) {
        return 0;
    }
    if (MemFlagIsSet(o->cond270, 0x1e) != 0) {
        return 0;
    }
    if (msg[2] == 0x7c44ec3d) {
        int mode = fn_800B26B8(o);
        if (mode != 4) {
            if (fn_800B22F8(o, mode) <= 1) {
                return 0;
            }
        }
    }
    SveAnimReply reply;
    reply.zero = 0;
    reply.minusOne = -1;
    msg[3] = 0x272cac;
    reply.state = msg[3];
    SendMsg(msg[1], 0x40b, &reply);
    return 1;
}

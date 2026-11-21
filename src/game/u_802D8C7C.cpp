// u_802D8C7C.cpp: owner follow states and conditions (0x802D8C7C-0x802DB9D0).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

// u_802D8C7C.cpp: actor query message handler and per-frame state update.
// Function, struct and field names are descriptive guesses.

typedef unsigned char u8;
typedef unsigned long u32;
typedef int s32;
typedef float f32;

struct Owner;

// State block owned by the actor, reached through Owner::unk130.
struct StateData {
    int unk00;          // 0x00 current state id
    int unk04;          // 0x04
    int unk08;          // 0x08
    f32 unk0c;          // 0x0c state timer
    f32 unk10;          // 0x10
    f32 unk14;          // 0x14
    f32 unk18;          // 0x18
    f32 unk1c;          // 0x1c
};

// Per-state dispatch table entry (shared table at 0x803C5730).
struct StateEntry {
    int (*fn0)(Owner*);     // 0x00
    int (*fn1)(Owner*);     // 0x04
    int (*fn2)(Owner*);     // 0x08
    const char* name;       // 0x0c
};

struct Owner {
    u8   unk000[0x24];              // 0x000
    u8   unk024[0x0c];              // 0x024
    void (*unk030)(int, void*);     // 0x030
    u8   unk034[0x038 - 0x034];     // 0x034
    int  unk038;                    // 0x038 owner id
    u8   unk03c[0x084 - 0x03c];     // 0x03c
    Vec3 unk084;                    // 0x084 world position
    u8   unk090[0x0c4 - 0x090];     // 0x090
    u8*  unk0c4;                    // 0x0c4 animation bank block
    u8   unk0c8[0x0fc - 0x0c8];     // 0x0c8
    void (*unk0fc)(Owner*);         // 0x0fc
    u8   unk100[0x104 - 0x100];     // 0x100
    int  (*unk104)(Owner*);         // 0x104 per-frame state updater
    u8   unk108[0x10c - 0x108];     // 0x108
    void (*unk10c)(Owner*);         // 0x10c
    u8   unk110[0x130 - 0x110];     // 0x110
    StateData* unk130;              // 0x130
    u8   unk134[0x04];              // 0x134
    f32  unk138;                    // 0x138
    u8   unk13c[0x140 - 0x13c];     // 0x13c
    f32  unk140;                    // 0x140
    u8   unk144[0x16c - 0x144];     // 0x144
    u32  unk16c;                    // 0x16c
    u8   unk170[0x174 - 0x170];     // 0x170
    f32  unk174;                    // 0x174
    int  unk178;                    // 0x178
    int  unk17c;                    // 0x17c
    u8   unk180[0x184 - 0x180];     // 0x180
    u32  unk184;                    // 0x184
    u8   unk188[0x21c - 0x188];     // 0x188
    int  unk21c;                    // 0x21c
    int  unk220;                    // 0x220
    u8   unk224[0x22c - 0x224];     // 0x224
    u32  unk22c;                    // 0x22c
    u8   unk230[0x240 - 0x230];     // 0x230
    f32  unk240;                    // 0x240
    f32  unk244;                    // 0x244
    f32  unk248;                    // 0x248
    u8   unk24c[0x268 - 0x24c];     // 0x24c
    int  unk268;                    // 0x268
    int  unk26c;                    // 0x26c
    u8   unk270[0x288 - 0x270];     // 0x270
    int  unk288;                    // 0x288
    u8   unk28c[0x2c4 - 0x28c];     // 0x28c
    int  unk2c4;                    // 0x2c4
    u8   unk2c8[0x5b4 - 0x2c8];     // 0x2c8
    f32  unk5b4;                    // 0x5b4 follow range scale
    f32  unk5b8;                    // 0x5b8 follow-range ratio limit
    u8   unk5bc[0x5d8 - 0x5bc];     // 0x5bc
    f32  unk5d8;                    // 0x5d8 follow range
    f32  unk5dc;                    // 0x5dc follow range inner limit
    u8   unk5e0[0x5e8 - 0x5e0];     // 0x5e0
    int  unk5e8;                    // 0x5e8 interaction partner owner id
    u8   unk5ec[0x5fc - 0x5ec];     // 0x5ec
    void (*unk5fc)(Owner*);         // 0x5fc
    void (*unk600)(Owner*);         // 0x600
    void (*unk604)(Owner*);         // 0x604
    u8   unk608[0x894 - 0x608];     // 0x608
    u8   unk894[0x8ec - 0x894];     // 0x894
    u32  unk8ec;                    // 0x8ec
    u32  unk8f0;                    // 0x8f0
    u8   unk8f4[0x900 - 0x8f4];     // 0x8f4
    f32  unk900;                    // 0x900
    f32  unk904;                    // 0x904
    void* unk908;                   // 0x908
};

// Reply block filled by the query message.
struct ScanReply {
    u8  unk00[0x0c];    // 0x00
    int unk0c;          // 0x0c
};

// Info block returned by OwnerGetInfo for a live target.
struct OwnerInfo {
    u8  unk00[0x24];    // 0x00
    s32 unk24;          // 0x24
    s32 unk28;          // 0x28
    s32 unk2c;          // 0x2c
};

struct TargetRef {
    u8  unk00[0x38];    // 0x00
    int unk38;          // 0x38
};

union MsgVal {
    f32 f;
    u32 u;
};

// Command block passed with every message.
struct MsgData {
    Owner* owner;       // 0x00
    int    cmd;         // 0x04
    u8*    buffer;      // 0x08
    int    pos;         // 0x0c
    MsgVal val10;       // 0x10
    MsgVal val14;       // 0x14
    int    unk18;       // 0x18
    int    unk1c;       // 0x1c
};

extern u8 __upper_map[256];

extern "C" {
void* memcpy(void*, const void*, unsigned long);
void* memset(void*, int, unsigned long);
void fn_8001EAF8(void* obj, int time, void (*cb)(void), void* arg, void* data);
int  fn_80049940(void);
int  fn_800BAF40(Owner* self);
int  fn_800BEF9C(int handle);
void fn_800C9ED0(Owner* self);
void fn_800C9FC0(Owner* self, void* arg, f32 value);
void fn_80162658(Owner* self, void* arg);
void fn_801BD7D4(Owner* self);
void fn_8028C188(void* out, const char* s, int length);
String fn_800225C8(const String& a, const String& b);
int  SendMsg(int to, int msg, void* data);
int  rand(void);
void fn_800B1AB8(Owner* self, int a, f32 delay, int b);
int  fn_800B26B8(Owner* self);
int  fn_800B261C(Owner* self, int value, int* out);
int  fn_800B1EF4(Owner* self, int index = 0);
void fn_800B1F20(Owner* self, int index);
void fn_800B1ED4(Owner* self, int index);
void fn_800BC56C(Owner* self, int target);
void fn_800BBCA8(Owner* self);
void fn_800CE96C(Owner* self);
void fn_800CF064(Owner* self);
void fn_800CC984(Owner* self);
void fn_800C748C(Owner* self);
int  fn_800B7098(int a, int b, int c, int d);
int  fn_800B84E4(int a, int b, void* c);
void fn_80162610(Owner* self, void* arg, int value);
void fn_8029B070(Owner* self, u32 id);
int  fn_8029B3B0(Owner* self, u32 id);
int  fn_800B7CE0(int handle);
u32  fn_8016325C(void* mem);
int  fn_800E0DBC(void* vec);
void fn_800E1098(void* vec);
f32  fn_800E1020(void* vec);
int  fn_800BF2D0(void* vec);
void fn_800BC5A8(Owner* self, void* vec);
int  fn_800BB218(Owner* self, int model, void* from, void* to, Owner* owner,
                 f32* scale, int* a, int* b, f32 radius);
void fn_800BAFF8(Owner* self, void* a, int b, int c, void* d, f32 e, f32 f);
}

void FollowOffsetUpdate(void* self);

int FollowStateDecide(Owner* self);

int OwnerId(Owner*);
int OwnerGetInfo(u32 id);
Owner* GetPlayer(void);
f32 OwnerValue(Owner*, void* target);
int ActorScanTargetCallback(Owner* self, Owner* other, f32 dist);
int ConditionUpdateBit(void* mem, int bit, int value);
int  FollowLineCheckActorDist(void* self, void* target, f32 dist);
int  fn_80162940(void* mem, int value);
void VFree(void* ptr);
int  fn_8029B140(Owner* self, u32 id);
int  fn_800B22F8(Owner* self, int index);
void fn_800B2418(f32 delay, Owner* self, int index);
int  fn_800B2454(Owner* self, int index);
u32  Fn800F7DEC(int a, int b, int c, int d, void* e);
void BitClear(u32* bits, int bit);
int MemFlagIsSet(void* mem, int flag);
int HasCondition(void* mem, int cond);
void ConditionSetBit(void* mem, int bit, int value);
int ActorPlayAnim(int id, u32 name, int arg);
u32 AssetFind(u32 id, u32 sig);
void* FollowGetTargetObj(void* mem);
void RangeResetPending(void* mem);
void FollowSetTargetObj(void* mem, void* target);
void* VAlloc(int size);
void AnimBankActivate(u32 bank, u32 anim);

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

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

int FollowIdleStateEnter(Owner* self);
int FollowStateEnterClear(Owner* self);
u32 FollowApproachStateUpdate(Owner* self);
int ActorFollowTargetDetach(Owner* self);
int FollowOffsetUpdateBegin(Owner* self);
int FollowTargetStateUpdate(Owner* self);
int ActorFollowDetachTarget(void* p);
int ActorFollowOffsetReset(Owner* self);
int FollowStateUpdate(Owner* self);
int FollowStateResetToIdle(Owner* self);
int ActorStateEnterFollow(Owner* self);
u32 ActorStateCheckTarget(Owner* self);
int ActorFollowEnd(Owner* self);
int ActorFollowBegin(Owner* self);
int ActorStateCanContinue(Owner* self);
u32 FollowSearchTimerArm(Owner* self);
u32 FollowOffsetTimerReset(Owner* self);
u32 FollowSearchStateUpdate(Owner* self);
int FollowWaitStateEnter(Owner* self);
int FollowOffsetStateLeave(Owner* self);
bool HasFollowCondition(Owner* self);

typedef int (*StateFunc)(Owner*);

StateEntry gFsgSveStates[8] = {
    { FollowIdleStateEnter, FollowStateEnterClear, (StateFunc)FollowApproachStateUpdate, "" },
    { ActorFollowTargetDetach, FollowOffsetUpdateBegin, FollowTargetStateUpdate, "" },
    { (StateFunc)ActorFollowDetachTarget, ActorFollowOffsetReset, FollowStateUpdate, "" },
    { FollowStateResetToIdle, ActorStateEnterFollow, (StateFunc)ActorStateCheckTarget, "" },
    { ActorFollowEnd, ActorFollowBegin, ActorStateCanContinue, "" },
    { (StateFunc)FollowSearchTimerArm, (StateFunc)FollowOffsetTimerReset, (StateFunc)FollowSearchStateUpdate, "" },
    { FollowWaitStateEnter, FollowOffsetStateLeave, (StateFunc)HasFollowCondition, "" },
    { 0, 0, 0, "" },
};

// The asset found by AssetFind for the 0x41e query reply.
int gFsgSveAsset = -1;
// Target-kind mask for the scan timer, built on first use.
u32 gFsgSveScanMask = 0;

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

static inline void FsgSveStateName(Buffer* out, const char* s) {
    String name(s);
    String newline("\n");
    const String& text = fn_800225C8(name, newline);

    fn_8028C188(out, text.mData, text.mLength);
}

struct Msg407 {
    Owner* actor;   // 0x00
    int unk04;      // 0x04
    void* arg;      // 0x08
    u8 pad0c[8];    // 0x0c
    u32 key;        // 0x14
};

static inline int FsgSveQuery(MsgData* data) {
    Owner* a = data->owner;
    Msg41E* m = (Msg41E*)data;
    StateData* b = a->unk130;

    switch (m->cmd) {
    case 0x63F94A69: {
        TargetRef* t = (TargetRef*)FollowGetTargetObj(a->unk270);

        if (t != 0) {
            *(int*)&m->result = t->unk38;
        } else {
            *(int*)&m->result = -1;
        }
        return 1;
    }
    case 0xBDC9494E:
        *(int*)&m->result = 0;
        return 1;
    case 0x832DF50A:
        if (gFsgSveAsset == -1) {
            gFsgSveAsset = AssetFind(0xF1754BF2, 0x21C5E0);
        }
        *(int*)&m->result = gFsgSveAsset;
        return 1;
    case 0xAD8576FA:
        FsgSveStateName(&m->result, gFsgSveStates[b->unk00].name);
        return 0;
    }
    return 0;
}

int HandleQueryActorMsg(int msg, MsgData* data)
{
    Owner* a = data->owner;
    u32 tag;
    Msg400 q;

    switch (msg) {
    case 0x400: {
        Msg400* m = (Msg400*)data;

        m->ownerId = OwnerId(data->owner);
        m->state = (a->unk904 > 0.0f) ? 1 : 0;
        m->f10 = 1.0f;
        m->f14 = 1.0f;
        m->flags = 2;
        m->unk1C = 0;
        return 1;
    }
    case 0x40A: {
        Msg40A* m = (Msg40A*)data;
        u32 h;

        q.actor = (Actor*)data->owner;
        q.ownerId = OwnerId(data->owner);
        ((Owner*)m->actor)->unk030(0x400, &q);
        h = StrHash("QUERY", m->anim);
        if (ActorPlayAnim(OwnerId(a), h, m->unk04) != 0) {
            m->result = 0;
            return 1;
        }
        if (HasCondition(a->unk270, 0xA) || HasCondition(a->unk270, 9)) {
            return 0;
        }
        if (fn_80049940()) {
            if (m->anim == 0x7C44EC3D) {
                m->result = 0xA53F7E23;
                return 1;
            }
            return 0;
        }
        if (MemFlagIsSet(a->unk270, 0xB)) {
            return 1;
        }
        if (m->anim == 0x3CA34BD6 && MemFlagIsSet(a->unk270, 0x20) == 0) {
            m->result = 0x1379C466;
            return 1;
        }
        return 0;
    }
    case 0x407: {
        Msg407* m = (Msg407*)data;

        if (m->key != 0x6CA5F172) {
            fn_800C9FC0(data->owner, m->arg, 5000.0f);
        }
        return 0;
    }
    case 0x7102: {
        MsgSave* m = (MsgSave*)data;
        StateData* b = data->owner->unk130;

        if (b != 0) {
            if (m->size < 0x24) {
                m->ok = 0;
                return 1;
            }
            tag = StrHash("[FSGSVE]");
            memcpy(m->buf, &tag, 4);
            m->used += 4;
            m->size -= 4;
            m->buf += 4;
            memcpy(m->buf, b, 0x20);
            m->used += 0x20;
            m->size -= 0x20;
            m->buf += 0x20;
        }
        return 1;
    }
    case 0x7103: {
        MsgSave* m = (MsgSave*)data;
        StateData* b = data->owner->unk130;

        if (b != 0) {
            if (m->used < 0x24) {
                m->ok = 0;
                return 1;
            }
            if (!SaveCheckTag(m, "[FSGSVE]")) {
                return 1;
            }
            memcpy(b, m->buf, 0x20);
            m->used -= 0x20;
            m->buf += 0x20;
        }
        return 1;
    }
    case 0x41E:
        return FsgSveQuery(data);
    }
    return 0;
}

int UpdateActorState(Owner* owner)
{
    Owner* self = (Owner*)owner;
    if (self == 0)
        return 0;

    StateData* st = self->unk130;

    if (!MemFlagIsSet(self->unk270, 5)) {
        if (gFsgSveScanMask == 0) {
            memset(&gFsgSveScanMask, 0xff, 4);
            BitClear((u32*)&gFsgSveScanMask, 0x13);
        }
        fn_8001EAF8(self->unk024, 0x5dc, (void (*)(void))ActorScanTargetCallback, self, &gFsgSveScanMask);
    }

    st->unk10 += self->unk138;
    st->unk14 += self->unk138;
    st->unk18 += self->unk138;

    if (FollowGetTargetObj(self->unk270) != 0)
        st->unk1c = 0.0f;
    else
        st->unk1c += self->unk138;

    if (HasCondition(self->unk270, 9) || HasCondition(self->unk270, 10) ||
        HasCondition(self->unk270, 0xb)) {
        fn_80162658(self, self->unk894);
        return 1;
    }

    int limit = fn_800BEF9C(self->unk2c4);
    if (self->unk268 > limit)
        self->unk268 = fn_800BEF9C(self->unk2c4);

    int res = gFsgSveStates[st->unk00].fn2(self);
    if (MemFlagIsSet(self->unk270, 5))
        res = 1;

    int stat = fn_800BAF40(self);
    if (stat == -1) {
        self->unk288 = 0;
        self->unk26c = 0;
    } else if (self->unk288 == 0) {
        self->unk288 = 3;
        self->unk26c = 3;
    }

    fn_80162658(self, self->unk894);

    if (res != 0) {
        int next = FollowStateDecide(self);
        if (next == 7) {
            fn_801BD7D4(self);
            return 1;
        }
        if ((next != st->unk00 || next == 1) &&
            gFsgSveStates[st->unk00].fn1(self) != 0) {
            if (gFsgSveStates[next].fn0(self) == 0)
                gFsgSveStates[st->unk00].fn0(self);
            ConditionSetBit(self->unk270, 0x40, 0);
            ConditionSetBit(self->unk270, 0x41, 0);
            ConditionSetBit(self->unk270, 0x43, 0);
            ConditionSetBit(self->unk270, 0x42, 0);
        }
    }
    return 1;
}

// Timer callback run once per scan interval: if the owner's current target is
// alive and no other actor has claimed it, arm the pending state change on
// both actors so they walk into each other's interaction range.
// Arms a pending state change on `a` towards `b`.
static inline void ScanArm(Owner* a, Owner* b) {
    a->unk17c = 2;
    a->unk184 = b->unk038;
    a->unk174 = 1001.0f;
    a->unk16c = 0;
    a->unk16c |= 2;
    a->unk16c |= 1;
    a->unk178 = 1;
    fn_800B1AB8(a, 1, -1.0f, 1);
}

// Links the two actors unless they already belong to the same group: arms `a`
// unless it is already heading for an actor, then arms `b` unless it is
// already heading for a target within `dist`.
static inline int ScanLink(Owner* a, Owner* b, f32 dist) {
    Owner* s = (Owner*)(void*)a;
    if (b->unk908 != 0 && b->unk908 == a->unk908) {
        return 1;
    }
    if ((s->unk16c & 1) == 0 || (s->unk16c & 4) != 0 ||
        (int)(s->unk184 >> 24) != 2) {
        ScanArm(s, b);
    }
    if ((b->unk16c & 1) != 0 && (b->unk16c & 4) == 0) {
        OwnerInfo* info = (OwnerInfo*)OwnerGetInfo(b->unk184);
        if (info != 0) {
            f32 x = (f32)info->unk24;
            f32 y = (f32)info->unk28;
            f32 z = (f32)info->unk2c;
            if (x * x + y * y + z * z < dist)
                return 1;
        }
    }
    ScanArm(b, a);
    return 1;
}

int ActorScanTargetCallback(void* p, Owner* other, f32 dist)
{
    Owner* self = (Owner*)p;
    Msg400 reply;

    if (self == 0 || SendMsg(self->unk038, 0x400, &reply) == 0 ||
        (int)((u32)self->unk038 >> 24) != 0x13 || reply.state == 0) {
        return 1;
    }
    return ScanLink(self, other, dist);
}

// Drops every condition bit this actor set up for its target interaction and
// releases the state block.
void ActorReleaseState(Owner* self)
{
    self->unk104 = 0;
    ConditionUpdateBit(self->unk270, 4, 0);
    ConditionUpdateBit(self->unk270, 3, 0);
    ConditionUpdateBit(self->unk270, 0x17, 0);
    ConditionSetBit(self->unk270, 0x2a, 0);
    ConditionSetBit(self->unk270, 0x2b, 0);
    VFree(self->unk130);
    self->unk130 = 0;
}

// Allocates the actor's state block, initialises it from the current state and
// arms its animation banks and follow-target tracking.
void ActorStateInit(Owner* self)
{
    StateData* st;
    u32 local;
    int cur;

    self->unk104 = UpdateActorState;
    void* block = VAlloc(0x20);
    if (block == 0) {
        self->unk130 = 0;
        return;
    }

    self->unk130 = (StateData*)block;
    self->unk0fc = fn_800CE96C;
    self->unk604 = fn_800CF064;
    self->unk5fc = fn_800CC984;
    self->unk600 = fn_800CC984;

    st = (StateData*)self->unk130;
    st->unk00 = FollowStateDecide(self);
    st->unk04 = 0;
    st->unk14 = st->unk10 = 0.0f;
    if (st->unk00 == 7)
        st->unk00 = 0;

    ConditionUpdateBit(self->unk270, 6, 1);
    ConditionUpdateBit(self->unk270, 0x17, 1);

    cur = fn_800B26B8(self);
    if (cur != 4) {
        if (fn_800B261C(self, cur, (int*)&local) != 0 && local != 0)
            FollowSetTargetObj(self->unk270, (void*)local);
        else
            RangeResetPending(self->unk270);
    }

    gFsgSveStates[st->unk00].fn0(self);

    gFsgSveAsset = AssetFind(0xf1754bf2u, 0x21c5e0u);
    AnimBankActivate(0xb089184cu, *(u32*)(self->unk0c4 + 0x160));
    AnimBankActivate(0xb089184cu, *(u32*)(self->unk0c4 + 0x164));
    AnimBankActivate(0xb0b21bb1u, *(u32*)(self->unk0c4 + 0x168));
}

// Returns 1 while the actor may keep running its current state; once the state
// has been told to leave and the timer has expired it finishes the transition.
int ActorStateCanContinue(Owner* self)
{
    void* target;
    int index;

    if (self == 0)
        return 0;

    index = fn_800B26B8(self);
    if (index != 4 && fn_800B1EF4(self, index) == 0)
        return 1;

    if (self->unk220 == 2) {
        self->unk10c = fn_800C748C;
        target = FollowGetTargetObj(self->unk270);
        if ((target != (void*)self->unk22c || self->unk288 != self->unk21c) &&
            self->unk21c != 0) {
            self->unk288 = self->unk21c;
            fn_800BC56C(self, self->unk22c);
            ConditionUpdateBit(self->unk270, 0x17, 1);
        }
        if (fn_800BAF40(self) == -1 && self->unk21c != 0)
            self->unk288 = 0;
        if (self->unk140 >= 500.0f) {
            fn_800BBCA8(self);
            self->unk220 = 0;
            return 1;
        }
    } else if (self->unk220 == 0) {
        return 1;
    }
    return 0;
}

// Arms the follow-offset update: installs the offset hinge updater on the actor,
// clears the pending follow fields and resets the offset timer.
int ActorFollowBegin(Owner* self)
{
    if (self == 0)
        return 0;

    self->unk10c = (void (*)(Owner*))FollowOffsetUpdate;
    self->unk130->unk14 = 0.0f;
    self->unk288 = 0;
    self->unk21c = 0;
    self->unk220 = 0;
    fn_800BBCA8(self);
    self->unk10c = (void (*)(Owner*))FollowOffsetUpdate;
    ConditionUpdateBit(self->unk270, 0x16, 0);
    fn_8029B070(self, 0x73b1e248);
    fn_8029B070(self, 0x13e6b);
    return 1;
}

// Leaves the follow state: switches the state block to the follow state, arms
// the follow conditions and, when a follow target exists, tells it to detach.
int ActorFollowEnd(Owner* self)
{
    struct FollowMsg {
        u8  unk00[4];               // 0x00
        int unk04;                  // 0x04 follow target handle
        int unk08;                  // 0x08 message id
        u8  unk0c[0x20 - 0x0c];     // 0x0c
    };

    if (self == 0)
        return 0;

    self->unk130->unk00 = 4;
    fn_8029B3B0(self, 0x13e6b);
    ConditionUpdateBit(self->unk270, 4, 1);
    ConditionUpdateBit(self->unk270, 0x16, 1);
    ConditionUpdateBit(self->unk270, 0x17, 1);

    int target = fn_800B7CE0(self->unk038);
    if (target != -1) {
        FollowMsg msg;
        memset(&msg, 0, sizeof(msg));
        msg.unk04 = target;
        msg.unk08 = 0x13e6b;
        SendMsg(self->unk038, 0x1800, &msg);
    }
    return 1;
}

// True while the actor still has something to follow: clears the follow bit and
// reports success when the target vanished, otherwise reports whether the
// follow-offset timer has not yet reached its limit.
u32 ActorStateCheckTarget(Owner* self)
{
    void* target;
    u32 value;

    if (self == 0)
        return 0;

    target = FollowGetTargetObj(self->unk270);
    fn_800B26B8(self);
    if (target == 0) {
        ConditionUpdateBit(self->unk270, 4, 0);
        return 1;
    }
    value = fn_8016325C(self->unk894);
    return ((6U - value) | (value - 6U)) >> 31;
}

// Sets the pending follow bit and re-evaluates the actor; entry step of the
// follow state.
int ActorStateEnterFollow(Owner* self)
{
    if (self == 0)
        return 0;

    ConditionSetBit(self->unk270, 0x2a, 0);
    fn_800BBCA8(self);
    return 1;
}

// Puts the follow state machine back to its idle state (3): clears the
// conditions that drove the search, drops the tracked target and resets the
// pending follow fields.
int FollowStateResetToIdle(Owner* self)
{
    StateData* st;
    void* target;

    if (self == 0)
        return 0;

    st = self->unk130;
    if (st->unk00 == 5)
        ConditionSetBit(self->unk270, 0x2a, 1);
    st->unk00 = 3;
    ConditionUpdateBit(self->unk270, 3, 0);
    ConditionUpdateBit(self->unk270, 4, 0);

    target = FollowGetTargetObj(self->unk270);
    if (target != 0) {
        FollowSetTargetObj(self->unk270, target);
        self->unk288 = 0;
    }

    if (self->unk220 == 4)
        self->unk220 = 0;

    fn_80162610(self, self->unk894, 3);
    return 1;
}

// Per-frame check of the follow state: finishes immediately when there is no
// target or the actor's target filter rejects it, otherwise keeps the state
// alive and leaves it once the lost-target case has resolved.
int FollowStateUpdate(Owner* self)
{
    Owner* target;

    if (self == 0)
        return 0;

    target = (Owner*)FollowGetTargetObj(self->unk270);
    if (target == 0 ||
        (self->unk604 != 0 &&
         ((int (*)(Owner*, int))self->unk604)(self, target->unk038) == 0)) {
        RangeResetPending(self->unk270);
        return 1;
    }

    if (self->unk22c == 0 ||
        fn_800B7098(((Owner*)self->unk22c)->unk038, self->unk038, target->unk038, 0) == 0)
        return 1;

    if (self->unk220 == 2) {
        self->unk10c = fn_800C748C;
        if ((FollowGetTargetObj(self->unk270) != (void*)self->unk22c ||
             self->unk288 != self->unk21c) &&
            self->unk21c != 0) {
            self->unk288 = self->unk21c;
            fn_800BC56C(self, self->unk22c);
        }
        if (fn_800BAF40(self) == -1 && self->unk21c != 0)
            self->unk288 = 0;
        if (self->unk140 >= 500.0f) {
            fn_800BBCA8(self);
            self->unk220 = 0;
            return 1;
        }
    } else if (self->unk220 == 0) {
        return 1;
    }
    return 0;
}

// Arms the follow-offset update: installs the offset hinge updater on the actor,
// clears the pending follow fields and re-arms the follow conditions.
int ActorFollowOffsetReset(Owner* self)
{
    if (self == 0)
        return 0;

    self->unk10c = (void (*)(Owner*))FollowOffsetUpdate;
    self->unk130->unk14 = 0.0f;
    self->unk288 = 0;
    self->unk21c = 0;
    self->unk220 = 0;
    fn_800BBCA8(self);
    self->unk10c = (void (*)(Owner*))FollowOffsetUpdate;
    fn_8029B070(self, 0x73b1e248);
    fn_8029B070(self, 0x13e6b);
    return 1;
}

// Leaves the follow-offset state: switches the state block back to state 2,
// clears the follow conditions and, when the tracked target is still valid,
// tells its actor to detach.
int ActorFollowDetachTarget(void* p)
{
    struct DetachMsg {
        u8  unk00[4];               // 0x00
        int unk04;                  // 0x04 follow target handle
        int unk08;                  // 0x08 message id
        u8  unk0c[0x20 - 0x0c];     // 0x0c
    };

    Owner* self = (Owner*)p;

    if (self == 0)
        return 0;

    self->unk130->unk00 = 2;
    fn_8029B3B0(self, 0x13e6b);
    ConditionUpdateBit(self->unk270, 4, 0);
    ConditionUpdateBit(self->unk270, 0x16, 0);
    ConditionUpdateBit(self->unk270, 0x17, 0);

    TargetRef* target = (TargetRef*)FollowGetTargetObj(self->unk270);
    if (target != 0) {
        int handle = fn_800B84E4(self->unk038, target->unk38, 0);
        if (handle != -1) {
            DetachMsg msg;
            memset(&msg, 0, sizeof(msg));
            msg.unk04 = handle;
            msg.unk08 = 0x13e6b;
            SendMsg(self->unk038, 0x1800, &msg);
        }
    }
    return 1;
}

// Per-frame update of the follow-target state: finishes at once when the
// tracked target is gone or rejected by the target filter, otherwise refreshes
// the follow condition bits and runs the pending-state transition.
int FollowTargetStateUpdate(Owner* self)
{
    Owner* target;

    if (self == 0)
        return 0;

    target = (Owner*)FollowGetTargetObj(self->unk270);
    if (target == 0 ||
        (self->unk604 != 0 &&
         ((int (*)(Owner*, int))self->unk604)(self, target->unk038) == 0)) {
        RangeResetPending(self->unk270);
        return 1;
    }

    if (self->unk22c == 0 ||
        fn_800B7098(((Owner*)self->unk22c)->unk038, self->unk038,
                    target->unk038, 0) == 0)
        return 1;

    if (FollowLineCheckActorDist(self, FollowGetTargetObj(self->unk270), 0.8f) != 0 &&
        fn_8029B3B0(self, 0x73b1e248u) != 0) {
        ConditionUpdateBit(self->unk270, 0, 1);
        ConditionUpdateBit(self->unk270, 3, 1);
        ConditionUpdateBit(self->unk270, 4, 1);
        if (fn_80162940(self->unk894, 2) != 0 && fn_80162940(self->unk894, 8) != 0 &&
            self->unk5b8 > (f32)rand() / 32767.0f) {
            fn_80162610(self, self->unk894, 2);
        } else if (fn_80162940(self->unk894, 1) != 0 && fn_80162940(self->unk894, 4) != 0) {
            fn_80162610(self, self->unk894, 1);
        }
    } else {
        fn_80162610(self, self->unk894, 0);
        ConditionUpdateBit(self->unk270, 0, 0);
    }

    if (self->unk220 == 2) {
        self->unk10c = fn_800C748C;
        if ((FollowGetTargetObj(self->unk270) != (void*)self->unk22c ||
             self->unk288 != self->unk21c) &&
            self->unk21c != 0) {
            self->unk288 = self->unk21c;
            fn_800BC56C(self, self->unk22c);
            ConditionUpdateBit(self->unk270, 0x17, 1);
        }
        if (fn_800BAF40(self) == -1 && self->unk21c != 0)
            self->unk288 = 0;
        if (self->unk140 >= 500.0f) {
            fn_800BBCA8(self);
            self->unk220 = 0;
            return 1;
        }
    } else if (self->unk220 == 0) {
        return 1;
    }
    return 0;
}

// Entry of the follow-offset state: installs the offset hinge updater on the
// actor, clears the pending follow fields, resets the offset timer and re-arms
// the follow conditions.
int FollowOffsetUpdateBegin(Owner* self)
{
    if (self == 0)
        return 0;

    self->unk10c = (void (*)(Owner*))FollowOffsetUpdate;
    self->unk130->unk14 = 0.0f;
    self->unk288 = 0;
    self->unk21c = 0;
    self->unk220 = 0;
    fn_800BBCA8(self);
    self->unk10c = (void (*)(Owner*))FollowOffsetUpdate;
    ConditionUpdateBit(self->unk270, 0x16, 0);
    fn_8029B070(self, 0x73b1e248u);
    fn_8029B070(self, 0x13e6bu);
    return 1;
}

// Leaves the follow-target state: switches the state block to state 1, arms the
// follow conditions and, when the tracked target is still valid, tells its
// actor to detach.
int ActorFollowTargetDetach(Owner* self)
{
    struct DetachMsg {
        u8  unk00[4];               // 0x00
        int unk04;                  // 0x04 follow target handle
        int unk08;                  // 0x08 message id
        u8  unk0c[0x20 - 0x0c];     // 0x0c
    };

    if (self == 0)
        return 0;

    self->unk130->unk00 = 1;
    fn_8029B3B0(self, 0x13e6b);
    ConditionUpdateBit(self->unk270, 4, 1);
    ConditionUpdateBit(self->unk270, 0x16, 1);
    ConditionUpdateBit(self->unk270, 0x17, 1);

    TargetRef* target = (TargetRef*)FollowGetTargetObj(self->unk270);
    if (target != 0) {
        int handle = fn_800B84E4(self->unk038, target->unk38, 0);
        if (handle != -1) {
            DetachMsg msg;
            memset(&msg, 0, sizeof(msg));
            msg.unk04 = handle;
            msg.unk08 = 0x13e6b;
            SendMsg(self->unk038, 0x1800, &msg);
        }
    }
    return 1;
}

// True while the actor has no follow-offset condition pending: the follow
// target scan runs every frame regardless of the offset state.
bool HasFollowCondition(Owner* self)
{
    if (self == 0)
        return 0;

    fn_800C9ED0(self);
    return HasCondition(self->unk270, 0x2c) == 0;
}

// Leaves the follow-offset state: clears the offset timer, releases follow
// condition bit 4, resets the follow state block and signals the actor.
int FollowOffsetStateLeave(Owner* self)
{
    if (self == 0)
        return 0;

    self->unk130->unk10 = 0.0f;
    ConditionUpdateBit(self->unk270, 4, 1);
    fn_80162610(self, self->unk894, 0);
    fn_8029B070(self, 0x73b1e248);
    return 1;
}

// Enters the follow wait state: switches the state block to state 6, clears the
// search conditions, resets any pending follow target and re-arms the follow
// timer for the actor's current follow target.
int FollowWaitStateEnter(Owner* self)
{
    StateData* st;
    int index;
    int target[4];

    if (self == 0)
        return 0;

    st = self->unk130;
    self->unk288 = 0;
    if (FollowGetTargetObj(self->unk270) != 0) {
        self->unk240 = 0.0f;
        self->unk244 = 0.0f;
        self->unk248 = 0.0f;
        fn_800BBCA8(self);
    }

    st->unk00 = 5;
    st->unk00 = 6;
    st->unk08 = 0;
    st->unk10 = 0.0f;

    ConditionUpdateBit(self->unk270, 4, 0);
    ConditionUpdateBit(self->unk270, 3, 0);
    ConditionUpdateBit(self->unk270, 0x17, 1);
    ConditionSetBit(self->unk270, 0x2b, 1);
    ConditionSetBit(self->unk270, 0x2c, 1);
    fn_80162610(self, self->unk894, 0);
    if (self->unk220 == 4)
        self->unk220 = 0;

    index = fn_800B26B8(self);
    if (index < 4 && fn_800B1EF4(self, index) == 0) {
        fn_800B1F20(self, index);
        fn_800B1AB8(self, index, -1.0f, 1);
        fn_800B261C(self, index, target);
        if (target[0] != 0)
            fn_800C9FC0(self, *(void**)(target[0] + 0x38), 10000.0f);
    }
    return 1;
}

// Per-frame update of the follow-search state: keeps the tracked target while
// the target filter accepts it, refreshes the follow timer and condition bits
// and reports whether the actor should leave the state.
u32 FollowSearchStateUpdate(Owner* self)
{
    StateData* st;
    Owner* target;
    int index;

    if (self == 0)
        return 0;

    st = self->unk130;
    if (fn_8029B140(self, 0x73b1e248u) == 0)
        return 1;

    target = (Owner*)FollowGetTargetObj(self->unk270);
    if (target == 0 ||
        (self->unk604 != 0 &&
         ((int (*)(Owner*, int))self->unk604)(self, target->unk038) == 0)) {
        RangeResetPending(self->unk270);
        return 1;
    }

    index = fn_800B26B8(self);
    if (index != 4) {
        if (fn_800B1EF4(self, index) == 0) {
            fn_800B261C(self, index, (int*)&target);
            fn_800B22F8(self, index);
            if (target != 0 && fn_800B22F8(self, index) < 2) {
                fn_800B2418(5000.0f, self, index);
                fn_800B1F20(self, index);
                fn_800B1AB8(self, index, -1.0f, 1);
                FollowSetTargetObj(self->unk270, target);
                if (st->unk10 > 3000.0f)
                    return 1;
            }
        } else {
            if (fn_800B2454(self, index) != 0 && st->unk10 > 3000.0f)
                return 1;
            fn_800B2418(5000.0f, self, index);
        }
    }

    fn_800C9ED0(self);
    if (FollowLineCheckActorDist(self, target, 0.8f) == 0)
        return 3000.0f < st->unk10;

    if (fn_80162940(self->unk894, 2) != 0 &&
        fn_80162940(self->unk894, 8) != 0) {
        if ((f32)rand() / 32767.0f < self->unk5b8) {
            fn_80162610(self, self->unk894, 2);
            goto CheckConditionBits;
        }
    }

    if (fn_80162940(self->unk894, 1) != 0 &&
        fn_80162940(self->unk894, 4) != 0) {
        fn_80162610(self, self->unk894, 1);
    } else {
        if (Fn800F7DEC(self->unk8ec, self->unk8f0, 0x20ee36, OwnerId(self), 0) ==
            0xc0becf7eu)
            return 1;
    }

CheckConditionBits:
    if ((fn_80162940(self->unk894, 0x800) == 0 &&
         fn_80162940(self->unk894, 0x400) == 0) ||
        st->unk10 <= 3000.0f)
        return 0;
    return 1;
}

// Resets the follow-offset timer: clears the offset timer, releases follow
// condition bit 4 and the offset condition, and re-arms the follow loop.
u32 FollowOffsetTimerReset(Owner* self)
{
    if (self == 0)
        return 0;

    self->unk130->unk10 = 0.0f;
    ConditionUpdateBit(self->unk270, 4, 0);
    ConditionSetBit(self->unk270, 0x2a, 0);
    fn_80162610(self, self->unk894, 0);
    fn_8029B070(self, 0x73b1e248u);
    return 1;
}

// Arms the follow-search timer: puts the state block into state 5, clears the
// pending follow offset, raises the follow conditions and re-arms the follow
// chain for the actor's current follow target.
u32 FollowSearchTimerArm(Owner* self)
{
    StateData* st;
    int index;
    TargetRef* target;

    if (self == 0)
        return 0;

    st = self->unk130;
    self->unk288 = 0;
    st->unk00 = 5;
    st->unk08 = 0;
    st->unk10 = 0.0f;
    ConditionUpdateBit(self->unk270, 4, 1);
    ConditionUpdateBit(self->unk270, 3, 1);
    ConditionUpdateBit(self->unk270, 0x17, 1);
    ConditionSetBit(self->unk270, 0x2a, 1);
    fn_8029B3B0(self, 0x73b1e248u);
    if (self->unk220 == 4)
        self->unk220 = 0;

    index = fn_800B26B8(self);
    if (index < 4 && fn_800B1EF4(self, index) == 0) {
        fn_800B1F20(self, index);
        fn_800B1AB8(self, index, -1.0f, 1);
        fn_800B261C(self, index, (int*)&target);
        if (target != 0)
            fn_800C9FC0(self, (void*)target->unk38, 10000.0f);
    }
    return 1;
}

// Per-frame update of the follow-approach state: validates the tracked target,
// keeps the follow condition bits in sync with the distance to the target and
// to the player, and leaves the state when the actor has closed in or slipped
// out of range.
u32 FollowApproachStateUpdate(Owner* self)
{
    StateData* st;
    Owner* player;
    TargetRef* target;
    int index;
    int inRange;
    f32 value;

    if (self == 0)
        return 0;

    st = self->unk130;
    player = GetPlayer();
    target = (TargetRef*)FollowGetTargetObj(self->unk270);
    if (target != 0 && self->unk604 != 0 &&
        ((int (*)(Owner*, int))self->unk604)(self, target->unk38) == 0) {
        RangeResetPending(self->unk270);
    }

    if (MemFlagIsSet(self->unk270, 0x24) && st->unk18 > 6000.0f)
        return 1;

    index = fn_800B26B8(self);
    if (index != 4) {
        if (fn_800B1EF4(self, index) == 0) {
            fn_800B261C(self, index, (int*)&target);
            fn_800B22F8(self, index);
            if (target != 0 && fn_800B22F8(self, index) < 2) {
                fn_800B2418(5000.0f, self, index);
                fn_800B1F20(self, index);
                fn_800B1AB8(self, index, -1.0f, 1);
                FollowSetTargetObj(self->unk270, target);
                fn_800C9FC0(self, (void*)target->unk38, 10000.0f);
            }
        } else {
            if (fn_800B2454(self, index) != 0) {
                ConditionUpdateBit(self->unk270, 4, 0);
            } else {
                fn_800B2418(5000.0f, self, index);
                ConditionUpdateBit(self->unk270, 4, 1);
            }
        }
    }

    fn_800C9ED0(self);
    if (Fn800F7DEC(self->unk8ec, self->unk8f0, 0x20ee36, OwnerId(self), 0) ==
        0xc0becf7eu)
        return 1;

    if (target != 0) {
        inRange = FollowLineCheckActorDist(self, target, 0.8f);
        value = OwnerValue(self, target);
        ConditionUpdateBit(self->unk270, 4, inRange != 0);

        if (inRange != 0 && fn_8029B3B0(self, 0x73b1e248u) != 0) {
            if (fn_80162940(self->unk894, 2) != 0 &&
                fn_80162940(self->unk894, 8) != 0 &&
                self->unk5b8 > (f32)rand() / 32767.0f) {
                fn_80162610(self, self->unk894, 2);
            } else if (fn_80162940(self->unk894, 1) != 0 &&
                       fn_80162940(self->unk894, 4) != 0) {
                fn_80162610(self, self->unk894, 1);
            }
            goto check_player;
        }

        if (fn_800BAF40(self) == -1)
            st->unk04 &= ~1;

        if ((st->unk04 & 1) == 0) {
            OwnerInfo* info;
            Vec3 toPlayer;
            Vec3 toSelf;
            int distSelf;
            int distPlayer;

            if (fn_800B84E4(self->unk038, target->unk38, 0) != -1)
                return 1;

            if (self->unk5e8 == -1)
                goto check_player;
            info = (OwnerInfo*)OwnerGetInfo(self->unk5e8);
            if (info == 0 || (inRange != 0 && value <= 1200.0f))
                goto check_player;

            toPlayer.x = info->unk24 - ((OwnerInfo*)player)->unk24;
            toPlayer.y = info->unk28 - ((OwnerInfo*)player)->unk28;
            toPlayer.z = info->unk2c - ((OwnerInfo*)player)->unk2c;
            toSelf.x = info->unk24 - ((OwnerInfo*)self)->unk24;
            toSelf.y = info->unk28 - ((OwnerInfo*)self)->unk28;
            toSelf.z = info->unk2c - ((OwnerInfo*)self)->unk2c;
            distSelf = fn_800E0DBC(&toSelf);
            distPlayer = fn_800E0DBC(&toPlayer);
            if ((f32)distPlayer <
                0.7f * (self->unk5d8 * self->unk5b4) + (f32)distSelf) {
                fn_800BC56C(self, (int)target);
                st->unk04 |= 1;
            }
        }
    } else {
        value = 3.4028235e38f;
        inRange = 0;
        if (fn_800B7CE0(self->unk038) != -1)
            return 1;
    }

check_player:
    if (player != 0) {
        f32 playerValue = OwnerValue(self, player);

        if (fn_800BAF40(self) != -1) {
            if (st->unk04 & 1) {
                if (playerValue > self->unk5d8) {
                    fn_800BBCA8(self);
                    st->unk04 &= ~1;
                } else if (inRange != 0 && value < 1200.0f) {
                    fn_800BBCA8(self);
                    st->unk04 &= ~1;
                } else if (self->unk5e8 != -1) {
                    OwnerInfo* info = (OwnerInfo*)OwnerGetInfo(self->unk5e8);
                    if (info != 0) {
                        Vec3 toPlayer;
                        Vec3 toSelf;
                        int distSelf;
                        int distPlayer;

                        toPlayer.x = info->unk24 - ((OwnerInfo*)player)->unk24;
                        toPlayer.y = info->unk28 - ((OwnerInfo*)player)->unk28;
                        toPlayer.z = info->unk2c - ((OwnerInfo*)player)->unk2c;
                        toSelf.x = info->unk24 - ((OwnerInfo*)self)->unk24;
                        toSelf.y = info->unk28 - ((OwnerInfo*)self)->unk28;
                        toSelf.z = info->unk2c - ((OwnerInfo*)self)->unk2c;
                        distSelf = fn_800E0DBC(&toSelf);
                        distPlayer = fn_800E0DBC(&toPlayer);
                        if (distPlayer > distSelf &&
                            playerValue > 0.7f * self->unk5d8) {
                            fn_800BBCA8(self);
                            st->unk04 &= ~1;
                        }
                    }
                }
            } else {
                if (playerValue < 0.9f * self->unk5d8 &&
                    playerValue > 1.1f * self->unk5dc) {
                    ConditionUpdateBit(self->unk270, 0x1d, 0);
                    fn_800BBCA8(self);
                }
            }
        } else {
            if (playerValue > self->unk5d8) {
                ConditionUpdateBit(self->unk270, 0x1d, 1);
                fn_800BC56C(self, (int)player);
                ConditionUpdateBit(self->unk270, 0x17, 1);
                self->unk288 = 3;
            } else if (playerValue < self->unk5dc) {
                Vec3 v;
                Vec3 p;
                f32 scale;
                int ia;
                int ib;

                v.x = (f32)((OwnerInfo*)player)->unk24;
                v.y = (f32)((OwnerInfo*)player)->unk28;
                v.z = (f32)((OwnerInfo*)player)->unk2c;
                v.x = self->unk084.x - v.x;
                v.y = self->unk084.y - v.y;
                v.z = self->unk084.z - v.z;
                v.z = 0.0f;
                fn_800E1098(&v);
                p.x = v.x + self->unk084.x;
                p.y = v.y + self->unk084.y;
                p.z = v.z + self->unk084.z;
                if (fn_800BB218(self, self->unk2c4, &self->unk084, &p, self,
                                &scale, &ia, &ib, self->unk900) != 0) {
                    f32 t = fn_800E1020(&v);
                    fn_800BAFF8(self, &v, ia, ib, &p, scale,
                                t * (1.0f - scale));
                }
                if (fn_800BF2D0(&self->unk084) == fn_800BF2D0(&p)) {
                    ConditionUpdateBit(self->unk270, 0x1d, 1);
                    fn_800BC5A8(self, &p);
                    ConditionUpdateBit(self->unk270, 0x17, 1);
                    self->unk288 = 3;
                }
            }
        }
    }
    return 0;
}

// Clears the follow condition bits, resets the actor's follow state to 0 and
// re-arms the follow timer.
int FollowStateEnterClear(Owner* self)
{
    if (self == 0)
        return 0;

    ConditionUpdateBit(self->unk270, 4, 0);
    fn_80162610(self, self->unk894, 0);
    fn_8029B070(self, 0x73b1e248u);
    return 1;
}

// Enters the follow idle state: resets the state block, picks a random state
// timer, arms the follow conditions and refreshes the actor's follow target.
int FollowIdleStateEnter(Owner* self)
{
    StateData* st;
    int index;
    int target[2];

    if (self == 0)
        return 0;

    st = self->unk130;
    self->unk288 = 3;
    st->unk00 = 0;
    st->unk08 = 0;
    st->unk0c = 500.0f * ((f32)(u32)rand() / 32767.0f) + 300.0f;
    st->unk04 &= ~1;
    ConditionUpdateBit(self->unk270, 4, 1);
    ConditionUpdateBit(self->unk270, 3, 1);
    ConditionUpdateBit(self->unk270, 0x17, 1);
    if (self->unk220 == 4)
        self->unk220 = 0;

    index = fn_800B26B8(self);
    if (index < 4 && fn_800B1EF4(self, index) == 0) {
        fn_800B1F20(self, index);
        fn_800B1AB8(self, index, -1.0f, 1);
        fn_800B261C(self, index, target);
        if (target[0] != 0)
            fn_800C9FC0(self, *(void**)(target[0] + 0x38), 10000.0f);
    }
    return 1;
}

// Chooses the next follow-state id for the actor from the current state block,
// the tracked follow target and the actor's follow condition flags.
int FollowStateDecide(Owner* self)
{
    StateData* st = self->unk130;
    void* target = FollowGetTargetObj(self->unk270);
    int index = fn_800B26B8(self);
    int alive;
    void* found;
    bool pending;
    void* track;
    int status;

    if (index != 4) {
        if (fn_800B1EF4(self, index) == 0) {
            fn_800B1F20(self, index);
            fn_800B261C(self, index, (int*)&found);
            alive = fn_800B22F8(self, index);
            fn_800B1ED4(self, index);
            if (found != 0 && alive < 2) {
                FollowSetTargetObj(self->unk270, found);
                target = found;
            }
        }
    }

    if (target == 0 ||
        (self->unk604 != 0 &&
         ((int (*)(Owner*, int))self->unk604)(self, ((TargetRef*)target)->unk38) == 0)) {
        RangeResetPending(self->unk270);
        pending = true;
    } else {
        pending = false;
    }

    fn_800B26B8(self);
    track = FollowGetTargetObj(self->unk270);

    if (MemFlagIsSet(self->unk270, 5))
        return 7;

    if (pending) {
        if (st->unk1c > 60000.0f || MemFlagIsSet(self->unk270, 5))
            return 7;
        if (self->unk5e8 == -1 || st->unk00 == 4)
            return 0;
        return 4;
    }

    if (MemFlagIsSet(self->unk270, 0x24) && st->unk18 > 6000.0f) {
        ConditionUpdateBit(self->unk270, 0x24, 0);
        st->unk18 = 0.0f;

        if ((f32)rand() / 32767.0f < self->unk5b4 ||
            st->unk14 < 1000.0f ||
            fn_8029B3B0(self, 0x13e6b) == 0 ||
            self->unk140 >= 500.0f) {
            if ((f32)rand() / 32767.0f >= 0.142857f ||
                st->unk10 <= 3000.0f ||
                fn_8029B3B0(self, 0x73b1e248) == 0)
                return 6;
            return 5;
        }
        return 1;
    }

    if (Fn800F7DEC(self->unk8ec, self->unk8f0, 0x20ee36, OwnerId(self), 0) ==
        0xc0becf7eu) {
        if (st->unk00 == 2)
            return 3;
        if (fn_8029B3B0(self, 0x13e6b) == 0 || track == 0 ||
            fn_800B84E4(self->unk038, ((TargetRef*)track)->unk38, &status) == -1)
            return 3;
        return 2;
    }

    if (FollowLineCheckActorDist(self, track, 0.8f) != 0 &&
        fn_8029B3B0(self, 0x73b1e248) != 0)
        return 0;
    if (fn_8029B3B0(self, 0x13e6b) != 0 && self->unk140 < 500.0f &&
        fn_800B84E4(self->unk038, ((TargetRef*)track)->unk38, 0) != -1)
        return 1;
    return 0;
}

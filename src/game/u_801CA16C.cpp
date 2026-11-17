// u_801CA16C.cpp: 0x801CA16C-0x801CC6D0, 33 functions. Rideable-host behaviour state machine J (8 states).
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   string pool 0x80348818 (POSSESS..SCAN, "", "QUERY", "[FMESVE]", "\n")
//   state table 0x803ACB38 (8 entries of 0x10 bytes, callbacks HostRideJState5Update-HostRideJState0Enter)
//   .sdata2 0x804D8B00-0x804D8B48 (0.0 at 0x804D8B00 read from 0x801CA16C to 0x801CC34C), .sdata 0x804D3220
//   HostRideJSlotTrue (return 1) is referenced by HostRideJState1Enter, so it stays here
// Like u_801F1798, the host's per-actor block (VAlloc'd, 0x10 bytes) holds the state index and two timers; the
// message handler answers the rider's queries and saves/loads that block (0x7102/0x7103, tagged "[FMESVE]").
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;
typedef unsigned char u8;

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

inline float fabsf(float x) { return __fabsf(x); }

struct Actor;
struct Owner;

// Per-actor ride state (VAlloc'd, 0x10 bytes; saved and loaded whole).
struct Beh {
    int state;      // 0x0
    float timer;    // 0x4
    u32 flags;      // 0x8
    float time;     // 0xC: time spent in the current state
};

// Position and handle of an owner (a target).
struct Target {
    char pad0[0x24];
    int x;          // 0x24
    int y;          // 0x28
    int z;          // 0x2C
    char pad30[8];
    u32 id;         // 0x38
};

// The player's host-entry array at 0x26C4; each entry starts with a kind word.
struct HostEntry {
    u32 kind;       // 0x0
};

struct Actor {
    char pad00[0x24];
    int x;                              // 0x24
    int y;                              // 0x28
    int z;                              // 0x2C
    int (*msgFn)(int msg, void* data);  // 0x30
    char pad34[4];
    int id;                             // 0x38
    char pad3C[0x48];
    Vec3 pos;                           // 0x84
    char pad90[0x44];
    int unkD4;                          // 0xD4
    char padD8[0x24];
    void* unkFC;                        // 0xFC
    char pad100[4];
    void* unk104;                       // 0x104
    char pad108[4];
    void* unk10C;                       // 0x10C
    char pad110[0x20];
    Beh* beh;                           // 0x130
    char pad134[4];
    float dt;                           // 0x138
    char pad13C[0xE0];
    int unk21C;                         // 0x21C
    int unk220;                         // 0x220
    char pad224[8];
    Target* unk22C;                     // 0x22C
    char pad230[0x38];
    int unk268;                         // 0x268
    char pad26C[4];
    char st[0x18];                      // 0x270: condition block
    int unk288;                         // 0x288
    char pad28C[0x38];
    int unk2C4;                         // 0x2C4
    char pad2C8[0x334];
    void* unk5FC;                       // 0x5FC
    void* unk600;                       // 0x600
    void* unk604;                       // 0x604
    char pad608[0x28C];
    char unk894[0x70];                  // 0x894
    float f904;                         // 0x904
    char pad908[0x1DBC];
    HostEntry* hosts[8];                // 0x26C4
    Target* unk26E4;                    // 0x26E4
};

// One entry of the state table (0x10 bytes).
typedef int (*HostRideJFunc)(void* actor);

struct HostRideJType {
    HostRideJFunc enter;    // 0x0
    HostRideJFunc check;    // 0x4: may the state be left?
    HostRideJFunc update;   // 0x8
    const char* name;       // 0xC
};

struct Msg407 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int unk08;      // 0x08
    char pad0C[8];
    u32 type;       // 0x14
};

struct Msg40B {
    int unk0;       // 0x00
    int unk4;       // 0x04
    int unk8;       // 0x08
    int unk0C;      // 0x0C
};

struct MsgC14 {
    int unk0;       // 0x00
    int unk4;       // 0x04
    int handle;     // 0x08
    u32 key;        // 0x0C
};

// Save/load request (0x7102 save, 0x7103 load).
struct MsgSave {
    Actor* actor;   // 0x00
    int size;       // 0x04: room left
    u8* buf;        // 0x08
    int used;       // 0x0C
    int ok;         // 0x10
};

// The data of the slot query set up by state 1.
struct SlotQuery {
    Actor* actor;   // 0x0
    void* target;   // 0x4
};

int OwnerId(Owner* o);                                          // 0x800A3A40
void* VAlloc(int size);                                         // 0x8009DA68
void VFree(void* p);                                            // 0x8009DA3C
int HasCondition(void* conditions, int which);                  // 0x800A7AC4
int AssetFind(u32 bank, u32 name);                              // 0x8007EB34
float OwnerValue(Owner* o, void* value);                        // 0x800AA050
Actor* GetPlayer(void);                                         // 0x80049948
int PlayerInHost(Actor* player);                                // 0x80049750
int ActorPlayAnim(int owner, u32 anim, int arg);                // 0x800C5654
int ActorAnimState(int owner, u32 anim);                        // 0x800C5028
void OwnerSlotAdd(Owner* owner, int index, u32 a, u32 b, u32 c);  // 0x800B1DCC
float ActorAimAngleToPos(void* self, Vec3* pos);                // 0x800AA6C4
float ActorAngleToPos(void* self, Vec3* pos);                   // 0x800AA9FC
void ActorAimOffset3D(void* self, float* pos, float* yaw, float* dist);  // 0x800AAE5C
void ActorAim(int* out, Actor* target, int a);                  // 0x8009F700
float GfxOwnerBrightness(u32 id);                               // 0x80115F68
int MemFlagIsSet(void* bits, int index);
int ConditionUpdateBit(void* cond, int bit, int set);
int ConditionSetBit(void* cond, int bit, int set);
void* FollowGetTargetObj(void* range);
void FollowSetTargetObj(void* range, void* obj);
int FollowOffsetUpdate(void* self);
void RangeResetPending(void* range);

extern "C" {
void* memcpy(void* dst, const void* src, u32 n);
int SendMsg(int to, int msg, void* data);                       // 0x800265D4
void fn_8001EAF8(void* pos, int radius, int (*fn)(), void* data, int a);  // 0x8001EAF8
int fn_80019F88(void* data, Actor* actor);                      // 0x80019F88
String fn_800225C8(const String& a, const String& b);           // 0x800225C8
int fn_80049940();                                              // 0x80049940
u32 fn_8008DCDC(int a, int b, int c, int d);                    // 0x8008DCDC
void fn_800B1AB8(Actor* actor, int kind, float f, int a);       // 0x800B1AB8
u32 fn_800B1ED4(Actor* actor, int kind);                        // 0x800B1ED4
int fn_800B1EF4(Actor* actor, int kind);                        // 0x800B1EF4
void fn_800B1F20(Actor* actor, int kind);                       // 0x800B1F20
int fn_800B22F8(Actor* actor, int kind);                        // 0x800B22F8
void fn_800B261C(Actor* actor, int kind, void* out);            // 0x800B261C
int fn_800B26B8(Actor* actor);                                  // 0x800B26B8
int fn_800BAF40(Actor* actor);                                  // 0x800BAF40
void fn_800BBCA8(Actor* actor);                                 // 0x800BBCA8
void fn_800BC56C(Actor* actor, void* target);                   // 0x800BC56C
void fn_800BC5A8(Actor* actor, Vec3* to);                       // 0x800BC5A8
int fn_800BEF9C(int obj);                                       // 0x800BEF9C
void fn_800C748C();                                             // 0x800C748C
void fn_800CC984();                                             // 0x800CC984
void fn_800CE96C();                                             // 0x800CE96C
void fn_800CF064();                                             // 0x800CF064
void fn_800E1098(void* out, void* in, float length);            // 0x800E1098
void fn_80162658(Actor* actor, void* p);                        // 0x80162658
int fn_8020B97C(void* p);                                       // 0x8020B97C
void fn_8028C188(Buffer* b, char* data, int len);               // 0x8028C188
}

int HostRideJMsg(int msg, void* data);
int HostRideJTick(void* owner);
void HostRideJFree(Actor* actor);
void HostRideJInit(Actor* actor);
int HostRideJState5Update(void* owner);
int HostRideJState5Exit(Actor* actor);
int HostRideJState5Enter(void* owner);
int HostRideJState4Update(void* owner);
int HostRideJState4Exit(Actor* actor);
int HostRideJState4Enter(void* owner);
int HostRideJState7Update(void* owner);
int HostRideJState7Exit(Actor* actor);
int HostRideJState7Enter(void* owner);
int HostRideJState6Update(void* owner);
int HostRideJState6Exit(Actor* actor);
int HostRideJState6Enter(Actor* actor);
int HostRideJState3Update(void* owner);
int HostRideJState3Exit(Actor* actor);
int HostRideJState3Enter(Actor* actor);
int HostRideJState1Update(void* owner);
int HostRideJState1Exit(Actor* actor);
int HostRideJState1Enter(void* owner);
int HostRideJState2Update(void* owner);
int HostRideJState2Exit(Actor* actor);
int HostRideJState2Enter(Actor* actor);
int HostRideJState0Update(void* owner);
int HostRideJState0Exit(Actor* actor);
int HostRideJState0Enter(Actor* actor);
int HostRideJNextState(void* owner);
void HostRideJSlot7(void* owner, void* target);
void HostRideJSlot6(void* owner, void* target);
int HostRideJSlotTrue();

// The asset found by AssetFind for the host query reply, cached on first use.
int gHostRideJAsset[2] = { -1, 0 };

HostRideJType gHostRideJTypes[8] = {
    { (HostRideJFunc)HostRideJState0Enter, (HostRideJFunc)HostRideJState0Exit, HostRideJState0Update, "" },
    { HostRideJState1Enter, (HostRideJFunc)HostRideJState1Exit, HostRideJState1Update, "" },
    { (HostRideJFunc)HostRideJState2Enter, (HostRideJFunc)HostRideJState2Exit, HostRideJState2Update, "" },
    { (HostRideJFunc)HostRideJState3Enter, (HostRideJFunc)HostRideJState3Exit, HostRideJState3Update, "" },
    { HostRideJState4Enter, (HostRideJFunc)HostRideJState4Exit, HostRideJState4Update, "" },
    { HostRideJState5Enter, (HostRideJFunc)HostRideJState5Exit, HostRideJState5Update, "" },
    { (HostRideJFunc)HostRideJState6Enter, (HostRideJFunc)HostRideJState6Exit, HostRideJState6Update, "" },
    { HostRideJState7Enter, (HostRideJFunc)HostRideJState7Exit, HostRideJState7Update, "" },
};

int HostRideJMsg(int msg, void* data) {
    Actor* a = *(Actor**)data;
    Beh* b = a->beh;

    switch (msg) {
    case 0x400: {
        Msg400* m = (Msg400*)data;

        m->ownerId = OwnerId((Owner*)a);
        m->state = (a->f904 > 0.0f) ? 1 : 0;
        m->f10 = 1.0f;
        m->f14 = 1.0f;
        m->flags = 2;
        m->unk1C = 0;
        return 1;
    }
    case 0x407: {
        Msg407* m = (Msg407*)data;

        if (m->type == 0x11A6B && m->unk04 != 0) {
            if (b->state == 3) {
                return 1;
            }
            ActorPlayAnim(a->id, m->type, m->unk08);
            return 1;
        }
        return 0;
    }
    case 0x40A: {
        Msg40A* m = (Msg40A*)data;
        Msg400 q;
        u32 h;
        Actor* p;

        q.actor = a;
        q.ownerId = OwnerId((Owner*)a);
        m->actor->msgFn(0x400, &q);
        h = StrHash("QUERY", m->anim);
        if (ActorPlayAnim(OwnerId((Owner*)a), h, m->unk04) != 0) {
            m->result = 0;
            return 1;
        }
        if (HasCondition(a->st, 0xA) || HasCondition(a->st, 9) || HasCondition(a->st, 0xB)) {
            return 0;
        }
        if (fn_80049940()) {
            if (m->anim == 0x7C44EC3D) {
                m->result = 0xA53F7E23;
                return 1;
            }
            return 0;
        }
        if (MemFlagIsSet(a->st, 0xB)) {
            return 1;
        }
        if (m->anim == 0x3CA34BD6 && MemFlagIsSet(a->st, 0x20) == 0) {
            m->result = 0x1379C466;
            return 1;
        }
        if (ActorAnimState(a->id, 0xDC35206A) == 0) {
            return 0;
        }
        p = GetPlayer();
        if (p == 0) {
            return 0;
        }
        if (p->unk26E4 != 0 && (int)(p->unk26E4->id >> 24) == 1) {
            return 0;
        }
        if (p->hosts[PlayerInHost(p)]->kind == 0x04624485 ||
            p->hosts[PlayerInHost(p)]->kind == 0x13C05 ||
            p->hosts[PlayerInHost(p)]->kind == 0xE0AB9B4F ||
            p->hosts[PlayerInHost(p)]->kind == 0xFFF5) {
            return 0;
        }
        if (MemFlagIsSet(a->st, 0x1E)) {
            return 0;
        }
        if (m->anim == 0x7C44EC3D) {
            int kind = fn_800B26B8(a);

            if (kind != 4 && fn_800B22F8(a, kind) <= 1) {
                return 0;
            }
        }
        {
            Msg40B r;

            r.unk4 = 0;
            r.unk0C = -1;
            m->result = 0x272CAC;
            r.unk8 = m->result;
            SendMsg(m->unk04, 0x40B, &r);
        }
        return 1;
    }
    case 0x409: {
        Msg409* m = (Msg409*)data;

        if (ActorPlayAnim(OwnerId((Owner*)a), m->arg, m->anim) != 0) {
            return 1;
        }
        switch (m->arg) {
        case 0x272CAC:
            if (MemFlagIsSet(a->st, 0x1F) == 0 && gHostRideJTypes[b->state].check(a)) {
                FollowSetTargetObj(a->st, (void*)m->unk04);
                if (gHostRideJTypes[3].enter(a) == 0) {
                    gHostRideJTypes[b->state].enter(a);
                }
            } else {
                MsgC14 c;
                int r;
                int id;

                r = ActorPlayAnim(a->id, 0xDC35206A, m->anim);
                id = a->id;
                if (m->anim != -1) {
                    c.unk4 = r;
                    c.handle = id;
                    c.key = 0xAF699ED0;
                    SendMsg(m->anim, 0xC14, &c);
                }
            }
            return 1;
        }
        return 0;
    }
    case 0x41D:
        return fn_80019F88(data, a) ? 1 : 0;
    case 0x41E: {
        Msg41E* m = (Msg41E*)data;

        switch (m->cmd) {
        case 0x832DF50A:
            *(int*)&m->result = gHostRideJAsset[0];
            return 1;
        case 0xAD8576FA: {
            String name(gHostRideJTypes[b->state].name);
            String newline("\n");
            const String& text = fn_800225C8(name, newline);

            fn_8028C188(&m->result, text.mData, text.mLength);
            return 0;
        }
        }
        return 0;
    }
    case 0x7102: {
        MsgSave* m = (MsgSave*)data;

        if (b != 0) {
            u32 tag;

            if (m->size < 0x14) {
                m->ok = 0;
                return 1;
            }
            tag = StrHash("[FMESVE]");
            memcpy(m->buf, &tag, 4);
            m->used += 4;
            m->size -= 4;
            m->buf += 4;
            memcpy(m->buf, b, 0x10);
            m->used += 0x10;
            m->size -= 0x10;
            m->buf += 0x10;
        }
        return 1;
    }
    case 0x7103: {
        MsgSave* m = (MsgSave*)data;

        if (b != 0) {
            bool ok;

            if (m->used < 0x14) {
                m->ok = 0;
                return 1;
            }
            if (m->used < 4) {
                ok = false;
            } else {
                u32* p = (u32*)m->buf;

                m->used += 4;
                m->size -= 4;
                m->buf += 4;
                ok = *p == StrHash("[FMESVE]");
            }
            if (!ok) {
                return 1;
            }
            memcpy(b, m->buf, 0x10);
            m->used -= 0x10;
            m->buf += 0x10;
        }
        return 1;
    }
    }
    return 0;
}

int HostRideJTick(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b;
    int r;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (HasCondition(a->st, 0xA) || HasCondition(a->st, 0xB)) {
        fn_80162658(a, a->unk894);
        return 1;
    }
    if (a->unk268 > fn_800BEF9C(a->unk2C4)) {
        a->unk268 = fn_800BEF9C(a->unk2C4);
    }
    r = gHostRideJTypes[b->state].update(a);
    b->time += a->dt;
    fn_80162658(a, a->unk894);
    if (r != 0 || (b->flags & 2)) {
        int next;

        b->flags &= ~2;
        next = HostRideJNextState(a);
        if (next != b->state && gHostRideJTypes[b->state].check(a)) {
            gHostRideJTypes[next].enter(a);
            b->time = 0.0f;
            ConditionSetBit(a->st, 0x40, 0);
            ConditionSetBit(a->st, 0x41, 0);
            ConditionSetBit(a->st, 0x43, 0);
            ConditionSetBit(a->st, 0x42, 0);
        }
    }
    return 1;
}

void HostRideJFree(Actor* a) {
    VFree(a->beh);
    a->unk104 = 0;
    a->beh = 0;
}

void HostRideJInit(Actor* a) {
    Beh* b;

    a->unk104 = (void*)HostRideJTick;
    b = (Beh*)VAlloc(0x10);
    if (b == 0) {
        a->beh = 0;
        return;
    }
    a->beh = b;
    a->unkFC = (void*)fn_800CE96C;
    a->unk604 = (void*)fn_800CF064;
    a->unk5FC = (void*)fn_800CC984;
    a->unk600 = (void*)fn_800CC984;
    b->timer = 0.0f;
    b->state = 0;
    b->flags = 0;
    b->time = 0.0f;
    gHostRideJAsset[0] = AssetFind(0xF1754BF2, 0x21C5E0);
}

int HostRideJState5Update(void* owner) {
    Actor* a = (Actor*)owner;
    int kind;
    Beh* b;
    Target* t;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    kind = fn_800B26B8(a);
    if (kind != 4) {
        fn_800B261C(a, kind, &t);
        if (t != FollowGetTargetObj(a->st)) {
            return 1;
        }
        if (fn_800B22F8(a, kind) <= 2) {
            return 1;
        }
    }
    if (b->time > 3000.0f) {
        return 1;
    }
    return 0;
}

int HostRideJState5Exit(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ConditionUpdateBit(a->st, 4, 0);
    ConditionUpdateBit(a->st, 9, 0);
    return 1;
}

int HostRideJState5Enter(void* owner) {
    Actor* a;
    int kind;
    Beh* b;
    Target* t;

    if (owner == 0) {
        return 0;
    }
    a = (Actor*)owner;
    b = a->beh;
    kind = fn_800B26B8((Actor*)owner);
    if (kind == 4) {
        return 0;
    }
    fn_800B261C((Actor*)owner, kind, &t);
    if (t == 0) {
        return 0;
    }
    if (ActorPlayAnim(a->id, 0xB1C7633C, t->id) != 0) {
        return 0;
    }
    fn_800B1F20((Actor*)owner, kind);
    FollowSetTargetObj(a->st, t);
    a->unk288 = 0;
    a->unk268 = 3;
    b->state = 5;
    b->timer = 0.0f;
    ConditionUpdateBit(a->st, 4, 1);
    ConditionUpdateBit(a->st, 9, 1);
    FollowSetTargetObj(a->st, t);
    {
        int id = a->id;
        QueryMsg q;

        q.hash = 0xF9D0235B;
        *(int*)&q.result = 1;
        q.unk8 = 0;
        SendMsg(id, 0x41D, &q);
    }
    OwnerSlotAdd((Owner*)a, kind, (u32)HostRideJSlot7, (u32)t, 0x3C);
    return 1;
}

int HostRideJState4Update(void* owner) {
    Actor* a = (Actor*)owner;
    int kind;
    Beh* b;
    Target* t;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    kind = fn_800B26B8(a);
    if (kind != 4) {
        fn_800B261C(a, kind, &t);
        if (t != FollowGetTargetObj(a->st)) {
            return 1;
        }
        if (fn_800B22F8(a, kind) <= 2) {
            return 1;
        }
    } else {
        return 1;
    }
    if (b->time > 3000.0f) {
        return 1;
    }
    return 0;
}

int HostRideJState4Exit(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ConditionUpdateBit(a->st, 4, 0);
    ConditionUpdateBit(a->st, 9, 0);
    return 1;
}

int HostRideJState4Enter(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b;
    int kind;
    Target* t;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    kind = fn_800B26B8(a);
    if (kind == 4) {
        return 0;
    }
    fn_800B261C(a, kind, &t);
    if (t == 0) {
        return 0;
    }
    if (ActorPlayAnim(a->id, 0x7C794AB6, t->id) != 0) {
        return 0;
    }
    fn_800B1F20(a, kind);
    FollowSetTargetObj(a->st, t);
    a->unk288 = 0;
    a->unk268 = 3;
    b->state = 4;
    b->timer = 0.0f;
    ConditionUpdateBit(a->st, 4, 1);
    ConditionUpdateBit(a->st, 9, 1);
    FollowSetTargetObj(a->st, t);
    OwnerSlotAdd((Owner*)a, kind, (u32)HostRideJSlot6, (u32)t, 0x28);
    return 1;
}

int HostRideJState7Update(void* owner) {
    Actor* a = (Actor*)owner;
    int kind;
    Beh* b;
    Target* t;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    kind = fn_800B26B8(a);
    if (kind != 4) {
        FollowGetTargetObj(a->st);
        fn_800B261C(a, kind, &t);
        if (t != 0 && t != FollowGetTargetObj(a->st)) {
            return 1;
        }
        if (fn_800B22F8(a, kind) <= 2) {
            return 1;
        }
    }
    t = (Target*)FollowGetTargetObj(a->st);
    if (t == 0) {
        return 1;
    }
    if (OwnerValue((Owner*)a, t) < 600.0f || (b->flags & 1)) {
        if (a->unk288 != 0) {
            fn_800BBCA8(a);
            a->unk288 = 0;
        } else if (!(b->flags & 1)) {
            ConditionUpdateBit(a->st, 4, 0);
            if (ActorPlayAnim(a->id, 0xFA658DD0, t->id) != 0) {
                return 1;
            }
            ConditionUpdateBit(a->st, 0x14, 1);
            b->flags |= 1;
        } else if (MemFlagIsSet(a->st, 0x14) == 0) {
            return 1;
        }
    } else if (fn_800BAF40(a) == -1) {
        a->unk288 = 0;
    } else {
        a->unk288 = 2;
    }
    return 0;
}

int HostRideJState7Exit(Actor* a) {
    Beh* b;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    fn_800BBCA8(a);
    RangeResetPending(a->st);
    ConditionUpdateBit(a->st, 4, 0);
    a->unk288 = 0;
    b->flags &= ~1;
    return 1;
}

int HostRideJState7Enter(void* owner) {
    Actor* a;
    Beh* b;
    void* t;

    if (owner == 0) {
        return 0;
    }
    a = (Actor*)owner;
    b = a->beh;
    t = FollowGetTargetObj(a->st);
    if (t == 0) {
        return 0;
    }
    a->unk288 = 0;
    b->state = 7;
    b->flags &= ~1;
    ConditionUpdateBit(a->st, 4, 1);
    fn_800BC56C(a, t);
    return 1;
}

int HostRideJState6Update(void* owner) {
    Actor* a = (Actor*)owner;
    int kind;
    Beh* b;
    Target* t;
    Msg400 q;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    kind = fn_800B26B8(a);
    if (kind != 4) {
        FollowGetTargetObj(a->st);
        fn_800B261C(a, kind, &t);
        if (t != 0 && t != FollowGetTargetObj(a->st)) {
            return 1;
        }
        if (fn_800B22F8(a, kind) <= 2) {
            return 1;
        }
        if (fn_800B1ED4(a, kind) & 8) {
            return 1;
        }
    }
    t = (Target*)FollowGetTargetObj(a->st);
    if (t == 0) {
        return 1;
    }
    SendMsg(t->id, 0x400, &q);
    if (!(q.flags & 2)) {
        return 1;
    }
    if ((OwnerValue((Owner*)a, t) < 1500.0f && fabsf(ActorAimAngleToPos(t, &a->pos) < 10.0f)) || (b->flags & 1)) {
        if (a->unk288 != 0) {
            fn_800BBCA8(a);
            a->unk288 = 0;
        } else if (!(b->flags & 1)) {
            ConditionUpdateBit(a->st, 4, 0);
            if (ActorPlayAnim(a->id, 0x1CE948A2, t->id) != 0) {
                return 1;
            }
            ConditionUpdateBit(a->st, 0x12, 1);
            b->flags |= 1;
        } else if (MemFlagIsSet(a->st, 0x12) == 0) {
            return 1;
        }
    } else {
        if (b->time > 200.0f) {
            int aim[3];
            Vec3 to;
            Vec3 d;

            to.x = t->x;
            to.y = t->y;
            to.z = t->z;
            ActorAim(aim, (Actor*)t, 1);
            aim[2] = 0;
            d.x = aim[0];
            d.y = aim[1];
            d.z = aim[2];
            fn_800E1098(&d, &d, 1350.0f);
            to.x += d.x;
            to.y += d.y;
            to.z += d.z;
            fn_800BC5A8(a, &to);
            b->time = 0.0f;
        }
        if (fn_800BAF40(a) == -1) {
            a->unk288 = 0;
        } else {
            a->unk288 = 2;
        }
    }
    return 0;
}

int HostRideJState6Exit(Actor* a) {
    Beh* b;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    fn_800BBCA8(a);
    RangeResetPending(a->st);
    ConditionUpdateBit(a->st, 4, 0);
    ConditionUpdateBit(a->st, 0x16, 0);
    a->unk288 = 0;
    b->flags &= ~1;
    return 1;
}

int HostRideJState6Enter(Actor* a) {
    Beh* b;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (FollowGetTargetObj(a->st) == 0) {
        return 0;
    }
    a->unk288 = 0;
    b->state = 6;
    b->flags &= ~1;
    ConditionUpdateBit(a->st, 4, 1);
    ConditionUpdateBit(a->st, 3, 0);
    ConditionUpdateBit(a->st, 0x16, 1);
    return 1;
}

int HostRideJState3Update(void* owner) {
    Actor* a;
    Beh* b;
    Target* t;
    Vec3 pos;

    if (owner == 0) {
        return 0;
    }
    a = (Actor*)owner;
    b = a->beh;
    t = (Target*)FollowGetTargetObj(a->st);
    if (t == 0 || (int)(t->id >> 24) != 2) {
        MsgC14 m;
        int id = a->id;
        Actor* p = GetPlayer();

        if (p->id != -1) {
            m.handle = id;
            m.key = 0xAF699ED0;
            m.unk4 = 0;
            SendMsg(p->id, 0xC14, &m);
        }
        b->flags |= 4;
        return 1;
    }
    if (MemFlagIsSet(a->st, 4) == 1 && HasCondition(a->st, 0xD) == 0 && HasCondition(a->st, 0xC) == 0) {
        pos.x = t->x;
        pos.y = t->y;
        pos.z = t->z;
        if (fabsf(ActorAngleToPos(a, &pos)) < 10.0f || b->time > 5000.0f) {
            ConditionUpdateBit(a->st, 4, 0);
            b->flags |= 1;
            b->time = 0.0f;
        }
    } else if (!(b->flags & 4)) {
        MsgC14 m;
        int r;
        int id;

        b->flags |= 4;
        r = ActorPlayAnim(a->id, 0xDC35206A, t->id);
        id = a->id;
        if ((int)t->id != -1) {
            m.unk4 = r;
            m.handle = id;
            m.key = 0xAF699ED0;
            SendMsg(t->id, 0xC14, &m);
        }
        if (HasCondition(a->st, 0xD) == 0) {
            ConditionSetBit(a->st, 0x12, 1);
        }
    }
    return fn_8020B97C(t) == 0 ? 1 : 0;
}

int HostRideJState3Exit(Actor* a) {
    Beh* b;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (!(b->flags & 4)) {
        return 0;
    }
    RangeResetPending(a->st);
    b->flags &= ~4;
    ConditionUpdateBit(a->st, 4, 0);
    ConditionUpdateBit(a->st, 0x16, 0);
    return 1;
}

int HostRideJState3Enter(Actor* a) {
    Beh* b;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (FollowGetTargetObj(a->st) == 0) {
        return 0;
    }
    a->unk288 = 0;
    b->state = 3;
    b->flags &= ~1;
    b->flags &= ~4;
    b->time = 0.0f;
    ConditionUpdateBit(a->st, 4, 1);
    ConditionUpdateBit(a->st, 3, 0);
    ConditionUpdateBit(a->st, 0x16, 1);
    if (a->unk220 == 4) {
        a->unk220 = 0;
    }
    return 1;
}

int HostRideJState1Update(void* owner) {
    Actor* a = (Actor*)owner;
    int kind;
    Beh* b;
    Target* t;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    kind = fn_800B26B8(a);
    if (kind != 4) {
        if (fn_800B1EF4(a, kind) == 0) {
            fn_800B261C(a, kind, &t);
            fn_800B22F8(a, kind);
            fn_800B1F20(a, kind);
        }
        if (!(fn_800B1ED4(a, kind) & 0x10) || fn_800B22F8(a, kind) < 3) {
            b->time = 0.0f;
        }
    }
    if (b->time > 3000.0f) {
        a->unk268 = 3;
        return 1;
    }
    return 0;
}

int HostRideJState1Exit(Actor* a) {
    if (a == 0) {
        return 0;
    }
    a->unk268 = 3;
    RangeResetPending(a->st);
    ConditionUpdateBit(a->st, 4, 0);
    ConditionSetBit(a->st, 0x1E, 0);
    return 1;
}

int HostRideJState1Enter(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b;
    int kind;
    Target* t;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    a->unk288 = 0;
    b->state = 1;
    b->timer = 0.0f;
    ConditionUpdateBit(a->st, 4, 1);
    ConditionSetBit(a->st, 0x1E, 1);
    kind = fn_800B26B8(a);
    if (kind == 4) {
        return 0;
    }
    fn_800B261C(a, kind, &t);
    if (t == 0) {
        return 0;
    }
    if (t != 0) {
        SlotQuery q;

        q.target = t;
        q.actor = a;
        fn_8001EAF8(&a->x, 10000, HostRideJSlotTrue, &q, 0);
    }
    return 1;
}

int HostRideJState2Update(void* owner) {
    Actor* a = (Actor*)owner;
    int kind;
    int n;
    Target* t;

    if (a == 0) {
        return 0;
    }
    kind = fn_800B26B8(a);
    if (kind != 4) {
        n = fn_800B22F8(a, kind);
        fn_800B261C(a, kind, &t);
        if (n != 4) {
            a->unk268 = n;
            if (MemFlagIsSet(a->st, 5) == 0) {
                return 1;
            }
        }
    }
    if (a->unk220 == 0 && MemFlagIsSet(a->st, 5) == 0) {
        return 1;
    }
    if (a->unk220 == 2) {
        a->unk10C = (void*)fn_800C748C;
        if (FollowGetTargetObj(a->st) != a->unk22C) {
            FollowSetTargetObj(a->st, a->unk22C);
            if (a->unk21C != 0) {
                a->unk288 = a->unk21C;
                fn_800BC56C(a, a->unk22C);
            }
        }
        if (fn_800BAF40(a) == -1 && a->unk21C != 0) {
            FollowSetTargetObj(a->st, 0);
            a->unk288 = 0;
        }
    } else {
        RangeResetPending(a->st);
        a->unk288 = 0;
    }
    if (GfxOwnerBrightness(a->id) < 1.0f) {
        ConditionUpdateBit(a->st, 9, 1);
    } else {
        ConditionUpdateBit(a->st, 9, 0);
    }
    return 0;
}

int HostRideJState2Exit(Actor* a) {
    if (a == 0) {
        return 0;
    }
    a->unk10C = (void*)FollowOffsetUpdate;
    if (a->unk220 == 4 && (fn_8008DCDC(a->unkD4, 0, 0, 0) & 0x10)) {
        a->unk220 = 0;
    }
    if (HasCondition(a->st, 0xD)) {
        ConditionSetBit(a->st, 0xD, 0);
    }
    return 1;
}

int HostRideJState2Enter(Actor* a) {
    if (a == 0) {
        return 0;
    }
    a->beh->state = 2;
    return 1;
}

int HostRideJState0Update(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b;
    int kind;
    Target* t;
    float yaw;
    float y;
    float dist;
    Vec3 pos;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (a->unk220 != 0) {
        return 1;
    }
    kind = fn_800B26B8(a);
    b->timer -= a->dt;
    if (kind != 4) {
        if (fn_800B1EF4(a, kind) == 0) {
            return 1;
        }
        yaw = 3.4028235e38f;
        dist = 3.4028235e38f;
        fn_800B261C(a, kind, &t);
        if (t != 0) {
            pos.x = t->x;
            pos.y = t->y;
            pos.z = t->z;
            ActorAimOffset3D(a, &pos.x, &yaw, &dist);
        }
        y = yaw;
        if (fabsf(y) < 45.0f && dist < 3000.0f) {
            FollowSetTargetObj(a->st, t);
        } else if (fabsf(y) > 70.0f || dist > 6000.0f) {
            RangeResetPending(a->st);
        }
    }
    fn_800BBCA8(a);
    if (GfxOwnerBrightness(a->id) < 1.0f) {
        ConditionUpdateBit(a->st, 9, 1);
    } else {
        ConditionUpdateBit(a->st, 9, 0);
    }
    return 0;
}

int HostRideJState0Exit(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ConditionUpdateBit(a->st, 0x16, 0);
    return 1;
}

int HostRideJState0Enter(Actor* a) {
    Beh* b;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    a->unk288 = 0;
    b->state = 0;
    b->timer = 0.0f;
    ConditionUpdateBit(a->st, 0x16, 1);
    return 1;
}

int HostRideJNextState(void* owner) {
    Actor* a;
    int kind;
    u32 flags;
    int n;
    Target* t;

    if (owner == 0) {
        return 0;
    }
    a = (Actor*)owner;
    if (MemFlagIsSet(a->st, 5)) {
        return 2;
    }
    kind = fn_800B26B8(a);
    fn_800B261C(a, kind, &t);
    flags = fn_800B1ED4(a, kind);
    if (kind != 4 && t != 0) {
        OwnerValue((Owner*)a, t);
        n = fn_800B22F8(a, kind);
        if (flags & 8) {
            fn_800B1AB8(a, kind, -1.0f, 3);
            return 5;
        }
        if (n <= 1) {
            return 1;
        }
        if ((flags & 4) && MemFlagIsSet(a->st, 0x27) == 0) {
            return 4;
        }
        if (!(flags & 0x10) && !(flags & 0x40) && !(flags & 0x20) &&
            OwnerValue((Owner*)a, t) < 1500.0f && fabsf(ActorAimAngleToPos(t, &a->pos)) < 10.0f) {
            a->unk268 = 1;
            return 1;
        }
        if (n == 2 && fn_800B1EF4(a, kind) == 0) {
            return 1;
        }
        if (n == 3 && fn_800B1EF4(a, kind) == 0) {
            fn_800B1F20(a, kind);
            return 0;
        }
        fn_800B1F20(a, kind);
    }
    return a->unk220 != 0 ? 2 : 0;
}

void HostRideJSlot7(void* owner, void* target) {
    Beh* b;
    Actor* a;

    if (owner != 0) {
        a = (Actor*)owner;
        b = a->beh;
        if (b != 0) {
            FollowSetTargetObj(a->st, target);
            if (b->state != 7 && gHostRideJTypes[b->state].check(a)) {
                gHostRideJTypes[7].enter(a);
            }
        }
    }
}

void HostRideJSlot6(void* owner, void* target) {
    Beh* b;
    Actor* a;

    if (owner != 0) {
        a = (Actor*)owner;
        b = a->beh;
        if (b != 0) {
            FollowSetTargetObj(a->st, target);
            if (b->state != 6 && gHostRideJTypes[b->state].check(a)) {
                gHostRideJTypes[6].enter(a);
            }
        }
    }
}

int HostRideJSlotTrue() {
    return 1;
}

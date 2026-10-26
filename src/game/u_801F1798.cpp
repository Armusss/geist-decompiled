// u_801F1798.cpp: rideable-host behaviour state machine C (0x801F1798-0x801F2DA4).
// A host that can be ridden runs a three-state enter/exit/update machine (state block VAlloc'd,
// 8 bytes: timer, state index, flags).  The message handler answers the riding ghost's queries
// (0x400, 0x407, 0x409, 0x40A, 0x41D, 0x41E); the 0x40A and 0x41D payloads are handled by their
// own functions.  Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

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

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

struct Actor;
struct Owner;
struct Target;

// Per-actor ride state (VAlloc'd, 8 bytes).
struct Beh {
    float timer;    // 0x0
    u8 state;       // 0x4
    u8 flags;       // 0x5
};

// Condition block at actor+0x270 (0x18 bytes).
struct HostState {
    char pad[0x18];
};

// The actor's "current puppet" record at 0xC4: its kind lives at 0xC.
struct ActorC4 {
    char pad0[0xC];
    int type;       // 0xC
};

// The player's host-entry array at 0x26C4; each entry starts with a kind word.
struct HostEntry {
    u32 kind;       // 0x0
};

// Position and handle of an owner (the OwnerGetInfo result, and the current target).
struct Target {
    char pad0[0x24];
    int x;          // 0x24
    int y;          // 0x28
    int z;          // 0x2C
    char pad30[8];
    int id;         // 0x38
};

// Scratch area at actor+0x894 (0x70 bytes).
struct Unk894 {
    char pad0[0x58];
    int f58;        // 0x58
    int f5C;        // 0x5C
    char pad60[0x10];
};

struct Actor {
    char pad00[0x24];
    int x;                              // 0x24
    int y;                              // 0x28
    int z;                              // 0x2C
    int (*msgFn)(int msg, void* data);  // 0x30
    char pad34[4];
    int id;                             // 0x38
    char pad3C[0x88];
    ActorC4* unkC4;                     // 0xC4
    char padC8[0x3C];
    int unk104;                         // 0x104
    char pad108[0x28];
    Beh* beh;                           // 0x130
    char pad134[4];
    float dt;                           // 0x138
    char pad13C[0x30];
    u32 flags16C;                       // 0x16C
    char pad170[4];
    float f174;                         // 0x174
    int unk178;                         // 0x178
    int unk17C;                         // 0x17C
    char pad180[4];
    int unk184;                         // 0x184
    char pad188[0xE8];
    HostState st;                       // 0x270
    int unk288;                         // 0x288
    char pad28C[0x328];
    float f5B4;                         // 0x5B4
    char pad5B8[0x20];
    float f5D8;                         // 0x5D8
    char pad5DC[4];
    int unk5E0;                         // 0x5E0
    char pad5E4[4];
    int unk5E8;                         // 0x5E8
    char pad5EC[0x1C];
    int unk608;                         // 0x608
    char pad60C[0x288];
    Unk894 unk894;                      // 0x894
    float f904;                         // 0x904
    int unk908;                         // 0x908
    char pad90C[0x1DB8];
    HostEntry* hosts[8];                // 0x26C4
    Target* unk26E4;                    // 0x26E4
};

// One entry of the ride dispatch table (0x10 bytes).
typedef int (*HostRideCFunc)(void* actor);

struct HostRideCType {
    HostRideCFunc enter;    // 0x0
    HostRideCFunc check;    // 0x4
    HostRideCFunc update;   // 0x8
    const char* name;       // 0xC
};

// Message layouts. The offsets are the same across message types; each case uses its own view.
struct Msg400 {
    Actor* actor;   // 0x00
    int ownerId;    // 0x04
    int unk08;      // 0x08
    int state;      // 0x0C
    float f10;      // 0x10
    float f14;      // 0x14
    int flags;      // 0x18
    int unk1C;      // 0x1C
};

struct Msg407 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int unk08;      // 0x08
    int unk0C;      // 0x0C
};

struct Msg409 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int anim;       // 0x08
    int arg;        // 0x0C
};

struct Msg40A {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    u32 anim;       // 0x08
    int result;     // 0x0C
};

struct Msg40B {
    int unk0;       // 0x00
    int unk4;       // 0x04
    u32 anim;       // 0x08
    int arg;        // 0x0C
    int unk10;      // 0x10
};

struct Msg41D {
    Actor* actor;   // 0x00
    u32 cmd;        // 0x04
    int unk08;      // 0x08
    int arg;        // 0x0C
};

struct Msg41E {
    Actor* actor;   // 0x00
    u32 cmd;        // 0x04
    int unk08;      // 0x08
    Buffer result;  // 0x0C
};

struct MsgC14 {
    int unk0;       // 0x00
    int unk4;       // 0x04
    int handle;     // 0x08
    u32 key;        // 0x0C
};

// Mangled C++ callees.
int OwnerId(Owner* o);                              // 0x800A3A40
void* VAlloc(int size);                             // 0x8009DA68
void VFree(void* p);                                // 0x8009DA3C
int HasCondition(void* conditions, int which);      // 0x800A7AC4
int AssetFind(u32 bank, u32 name);                  // 0x8007EB34
float OwnerValue(Owner* o, void* value);            // 0x800AA050
Target* OwnerGetInfo(u32 handle);                   // 0x8002394C
int PlayerInHost(Actor* player);                    // 0x80049750
Actor* GetPlayer(void);                             // 0x80049948
int ActorPlayAnim(int owner, u32 anim, int arg);    // 0x800C5654
int Fn800F7DEC(int a, int b, int c, int d, void* e);  // 0x800F7DEC

// Still-unnamed helpers.
extern "C" {
int rand(void);
int SendMsg(int to, int msg, void* data);               // 0x800265D4
int fn_80019F88(void* data, Actor* actor);              // 0x80019F88
String fn_800225C8(const String& a, const String& b);   // 0x800225C8
int fn_80049940();                                      // 0x80049940
void fn_800B1AB8(Actor* actor, int state, float f, int a);  // 0x800B1AB8
int fn_800B1EF4(Actor* actor, int state);               // 0x800B1EF4
void fn_800B1F20(Actor* actor, int state);              // 0x800B1F20
int fn_800B22F8(Actor* actor, int state);               // 0x800B22F8
void fn_800B2418(Actor* actor, int state, float f);     // 0x800B2418
int fn_800B2454(Actor* actor, int state);               // 0x800B2454
void fn_800B261C(Actor* actor, int state, void** out);  // 0x800B261C
int fn_800B26B8(Actor* actor);                          // 0x800B26B8
void fn_800B35D0(Actor* actor);                         // 0x800B35D0
int fn_800B7098(int a, int b, int c, int d);            // 0x800B7098
void fn_800C9ED0(Actor* actor);                         // 0x800C9ED0
void fn_800C9FC0(Actor* actor, int target, float f);    // 0x800C9FC0
int fn_800E0DBC(void* v);                               // 0x800E0DBC
void fn_80162610(Actor* actor, void* p, int a);         // 0x80162610
void fn_80162658(Actor* actor, void* p);                // 0x80162658
int fn_80162940(void* p, int a);                        // 0x80162940
int fn_8020B97C(void* p);                               // 0x8020B97C
void fn_8028C188(Buffer* b, char* data, int len);       // 0x8028C188
int fn_8029B070(Actor* actor, u32 key);                 // 0x8029B070
int fn_8029B140(Actor* actor, u32 key);                 // 0x8029B140
int fn_8029B3B0(Actor* actor, int key);                 // 0x8029B3B0
}
int FollowLineCheckActorDist(void* self, void* target, float budget);
int MemFlagIsSet(void* bits, int index);
int ConditionUpdateBit(void* cond, int bit, int set);
int ConditionSetBit(void* cond, int bit, int set);
void* FollowGetTargetObj(void* range);
void FollowSetTargetObj(void* range, void* obj);
void RangeResetPending(void* range);

int HostRideCMsg(int msg, void* data);
int HostRideCTick(void* owner);
void HostRideCFree(Actor* actor);
void HostRideCInit(void* owner);
int HostRideCState2Update(Actor* actor);
int HostRideCState2Exit(Actor* actor);
int HostRideCState2Enter(Actor* actor);
int HostRideCState1Exit(void* owner);
int HostRideCState1Update(void* owner);
int HostRideCState1Enter(Actor* actor);
int HostRideCState0Check(Actor* actor);
int HostRideCState0Update(void* owner);
int HostRideCState0Enter(Actor* actor);
int HostRideCHandle41D(void* data);
int HostRideCHandle40A(void* data);

// The asset found by AssetFind for the host query reply, cached on first use.
int gHostRideCAsset[2] = { -1, 0 };

HostRideCType gHostRideCTypes[] = {
    { (HostRideCFunc)HostRideCState0Enter, (HostRideCFunc)HostRideCState0Check, HostRideCState0Update, "" },
    { (HostRideCFunc)HostRideCState1Enter, HostRideCState1Exit, HostRideCState1Update, "" },
    { (HostRideCFunc)HostRideCState2Enter, (HostRideCFunc)HostRideCState2Exit, (HostRideCFunc)HostRideCState2Update, "" },
};

static inline void HostRideCStateName(Buffer* out, const char* s) {
    String name(s);
    String newline("\n");
    const String& text = fn_800225C8(name, newline);

    fn_8028C188(out, text.mData, text.mLength);
}

int HostRideCMsg(int msg, void* data) {
    switch (msg) {
    case 0x400: {
        Msg400* m = (Msg400*)data;
        Actor* a = m->actor;

        m->ownerId = OwnerId((Owner*)a);
        m->state = (a->f904 > 0.0f) ? 1 : 0;
        m->f10 = 1.0f;
        m->f14 = 1.0f;
        m->flags = 2;
        m->unk1C = 0;
        return 1;
    }
    case 0x40A:
        return HostRideCHandle40A(data);
    case 0x409: {
        Msg409* m = (Msg409*)data;
        Beh* b;
        Actor* a = m->actor;

        b = a->beh;

        if (ActorPlayAnim(OwnerId((Owner*)a), m->arg, m->anim) != 0) {
            return 1;
        }
        switch (m->arg) {
        case 0x272CAC:
            if (gHostRideCTypes[b->state].check(a)) {
                FollowSetTargetObj(&a->st, (void*)m->unk04);
                if (gHostRideCTypes[2].enter(a) == 0) {
                    gHostRideCTypes[b->state].enter(a);
                }
            }
            return 1;
        }
        return 0;
    }
    case 0x41E: {
        Msg41E* m = (Msg41E*)data;
        Beh* b = m->actor->beh;

        switch (m->cmd) {
        case 0x832DF50A:
            *(int*)&m->result = (int)gHostRideCAsset[0];
            return 1;
        case 0xAD8576FA:
            HostRideCStateName(&m->result, gHostRideCTypes[b->state].name);
            return 0;
        }
        return 0;
    }
    case 0x41D:
        return HostRideCHandle41D(data);
    case 0x407: {
        Msg407* m = (Msg407*)data;
        Beh* b;
        Actor* a = m->actor;
        int type = a->unkC4->type;

        b = a->beh;

        if ((type == 0x12EC1 || type == 0x906E8474) &&
            MemFlagIsSet(&a->st, 0x28) == 0 &&
            (float)rand() / 32767.0f > a->f5B4 &&
            a->unk908 != 0x8CD18EE1 &&
            fn_8029B3B0(a, 0x13E6B) != 0 &&
            a->id != -1) {
            MsgC14 q;

            q.handle = a->id;
            q.key = 0x93F5146E;
            q.unk4 = 0;
            SendMsg(a->id, 0xC14, &q);
        }
        if (b->state == 1 && (float)rand() / 32767.0f > a->f5B4) {
            b->timer = 0.0f;
        }
        return 0;
    }
    }
    return 0;
}

int HostRideCTick(void* owner) {
    int next;
    Actor* o;
    Beh* b;

    o = (Actor*)owner;
    b = o->beh;

    if (HasCondition(&o->st, 0xA) || HasCondition(&o->st, 0xB)) {
        fn_80162658(o, &o->unk894);
        return 1;
    }
    next = gHostRideCTypes[b->state].update(o);
    if (next != b->state && !(b->flags & 1)) {
        if (gHostRideCTypes[b->state].check(o)) {
            gHostRideCTypes[next].enter(o);
            ConditionSetBit(&o->st, 0x40, 0);
            ConditionSetBit(&o->st, 0x41, 0);
            ConditionSetBit(&o->st, 0x43, 0);
            ConditionSetBit(&o->st, 0x42, 0);
        }
    }
    fn_80162658(o, &o->unk894);
    return 1;
}

void HostRideCFree(Actor* o) {
    VFree(o->beh);
    o->unk104 = 0;
    o->beh = 0;
}

void HostRideCInit(void* owner) {
    Actor* o = (Actor*)owner;
    Beh* b;

    b = (Beh*)VAlloc(8);
    if (!b) {
        o->beh = 0;
        return;
    }
    o->unk5E0 = o->unk608;
    o->beh = b;
    b->timer = 0.0f;
    b->state = 0;
    b->flags = 0;
    gHostRideCAsset[0] = AssetFind(0xF1754BF2, 0x21C5E0);
    gHostRideCTypes[b->state].enter(o);
}

int HostRideCState2Update(Actor* o) {
    Target* target;
    Beh* b = o->beh;

    target = (Target*)FollowGetTargetObj(&o->st);

    if (target == 0 || (int)((u32)target->id >> 24) != 2) {
        MsgC14 m;
        int id = o->id;
        Actor* p = GetPlayer();

        if (p->id != -1) {
            m.handle = id;
            m.key = 0xAF699ED0;
            m.unk4 = 0;
            SendMsg(p->id, 0xC14, &m);
        }
        b->flags |= 2;
        return 1;
    }
    if (!(b->flags & 2)) {
        MsgC14 m;
        int r;
        int id;

        b->flags |= 2;
        r = ActorPlayAnim(o->id, 0xDC35206A, target->id);
        id = o->id;
        if (target->id != -1) {
            m.unk4 = r;
            m.handle = id;
            m.key = 0xAF699ED0;
            SendMsg(target->id, 0xC14, &m);
        }
    }
    return fn_8020B97C(target) == 0 ? 1 : 0;
}

int HostRideCState2Exit(Actor* o) {
    Beh* b = o->beh;

    if (!(b->flags & 2)) {
        return 0;
    }
    b->flags &= ~2;
    RangeResetPending(&o->st);
    ConditionUpdateBit(&o->st, 0x16, 0);
    return 1;
}

int HostRideCState2Enter(Actor* o) {
    Beh* b = o->beh;

    if (FollowGetTargetObj(&o->st) == 0) {
        return 0;
    }
    o->unk288 = 0;
    b->state = 2;
    b->flags &= ~2;
    ConditionUpdateBit(&o->st, 0x16, 1);
    return 1;
}

int HostRideCState1Exit(void* owner) {
    Actor* o;
    Beh* b;

    o = (Actor*)owner;
    b = o->beh;
    b->timer = 1500.0f * ((float)rand() / 32767.0f) + 1500.0f;
    ConditionUpdateBit(&o->st, 0x16, 0);
    fn_8029B070(o, 0x73B1E248);
    if (o->unkC4->type == 0x12EC1 || o->unkC4->type == 0x906E8474) {
        if (MemFlagIsSet(&o->st, 0x28) == 0 &&
            (float)rand() / 32767.0f < o->f5B4 &&
            o->unk908 != 0x8CD18EE1 &&
            fn_8029B3B0(o, 0x13E6B) != 0 &&
            o->id != -1) {
            MsgC14 m;

            m.handle = o->id;
            m.key = 0x93F5146E;
            m.unk4 = 0;
            SendMsg(o->id, 0xC14, &m);
        }
    }
    return 1;
}

int HostRideCState1Update(void* owner) {
    Actor* o = (Actor*)owner;
    Beh* b = o->beh;
    int kind;
    void* target;

    kind = fn_800B26B8(o);
    if (kind != 4) {
        if (fn_800B1EF4(o, kind) == 0) {
            fn_800B261C(o, kind, &target);
            fn_800B22F8(o, kind);
            if (target == 0 || fn_800B22F8(o, kind) > 1) {
                return 0;
            }
            fn_800B2418(o, kind, 15000.0f);
            fn_800B1F20(o, kind);
            fn_800B1AB8(o, kind, -1.0f, 1);
            {
                void* t;

                fn_800B261C(o, kind, &t);
                FollowSetTargetObj(&o->st, t);
                if (t != 0) {
                    fn_800C9FC0(o, ((Target*)t)->id, 10000.0f);
                }
            }
        } else {
            fn_800B2418(o, kind, 15000.0f);
        }
    } else {
        return 0;
    }
    target = FollowGetTargetObj(&o->st);
    if (target == 0) {
        return 0;
    }
    if (fn_8029B140(o, 0x73B1E248) == 0) {
        return 0;
    }
    if (!HasCondition(&o->st, 8) && FollowLineCheckActorDist(o, target, 0.0f) == 0) {
        return 0;
    }
    if (Fn800F7DEC(o->unk894.f58, o->unk894.f5C, 0x20EE36, o->id, 0) == 0xC0BECF7E) {
        return 0;
    }
    fn_800C9ED0(o);
    if (MemFlagIsSet(&o->st, 0x28) == 0 &&
        fn_800B7098(o->unk608, o->id, ((Target*)target)->id, 1) == 0 &&
        o->id != -1) {
        MsgC14 m;

        m.handle = o->id;
        m.key = 0x93F5146E;
        m.unk4 = 0;
        SendMsg(o->id, 0xC14, &m);
    }
    if (fn_80162940(&o->unk894, 1) && fn_80162940(&o->unk894, 4)) {
        fn_80162610(o, &o->unk894, 1);
    }
    if (!(b->flags & 4)) {
        b->timer -= o->dt;
        if (b->timer <= 0.0f) {
            return 0;
        }
    }
    return b->state;
}

int HostRideCState1Enter(Actor* o) {
    Beh* b = o->beh;
    int kind = fn_800B26B8(o);

    if (kind != 4 && fn_800B22F8(o, kind) <= 1) {
        void* target = 0;

        fn_800B261C(o, kind, &target);
        FollowSetTargetObj(&o->st, target);
    }
    b->state = 1;
    b->timer = 2000.0f * ((float)rand() / 32767.0f) + 4000.0f;
    ConditionUpdateBit(&o->st, 3, 1);
    ConditionUpdateBit(&o->st, 0x16, 1);
    ConditionSetBit(&o->st, 8, 1);
    return 1;
}

int HostRideCState0Check(Actor* o) {
    return 1;
}

int HostRideCState0Update(void* owner) {
    Actor* o = (Actor*)owner;
    int ret;
    Beh* b = o->beh;
    int kind;
    void* target;
    int a[3];
    int c[3];

    ret = b->state;
    kind = fn_800B26B8(o);
    if (kind != 4 && fn_800B22F8(o, kind) <= 1) {
        target = 0;
        fn_800B261C(o, kind, &target);
        ConditionSetBit(&o->st, 0x1A, 1);
        if (target != 0) {
            FollowSetTargetObj(&o->st, target);
        }
        if (fn_800B2454(o, kind) == 0) {
            fn_800B2418(o, kind, 15000.0f);
        }
        if (b->flags & 4) {
            if (fn_8029B3B0(o, 0x73B1E248) != 0) {
                ret = 1;
            }
        } else {
            b->timer -= o->dt;
            if (b->timer <= 0.0f && fn_8029B3B0(o, 0x73B1E248) != 0) {
                ret = 1;
            }
        }
    } else {
        b->timer = 0.0f;
        if (!(b->flags & 1)) {
            ConditionSetBit(&o->st, 0x1A, 0);
        }
    }
    if (MemFlagIsSet(&o->st, 0x28) == 0) {
        Target* t = (Target*)FollowGetTargetObj(&o->st);
        if (t != 0 && fn_800B7098(o->unk608, o->id, t->id, 1) == 0) {
            if (o->id != -1) {
                MsgC14 m;

                m.handle = o->id;
                m.key = 0x93F5146E;
                m.unk4 = 0;
                SendMsg(o->id, 0xC14, &m);
            }
            ret = 0;
        }
        if (o->unk908 == 0x8CD18EE1) {
            Actor* player = GetPlayer();

            if (player != 0 && OwnerValue((Owner*)o, player) > o->f5D8) {
                Target* info;
                int d1;
                int d2;

                if (o->unk5E8 == -1 || (info = OwnerGetInfo(o->unk5E8)) == 0) {
                    if (o->id != -1) {
                        MsgC14 m;

                        m.handle = o->id;
                        m.key = 0x93F5146E;
                        m.unk4 = 0;
                        SendMsg(o->id, 0xC14, &m);
                    }
                    ret = 0;
                } else {
                    a[0] = info->x - player->x;
                    a[1] = info->y - player->y;
                    a[2] = info->z - player->z;
                    c[0] = info->x - o->x;
                    c[1] = info->y - o->y;
                    c[2] = info->z - o->z;
                    d1 = fn_800E0DBC(c);
                    d2 = fn_800E0DBC(a);
                    if (d2 < d1) {
                        if (o->id != -1) {
                            MsgC14 m;

                            m.handle = o->id;
                            m.key = 0x93F5146E;
                            m.unk4 = 0;
                            SendMsg(o->id, 0xC14, &m);
                        }
                        ret = 0;
                    }
                }
            }
        }
    }
    return ret;
}

int HostRideCState0Enter(Actor* o) {
    Beh* b = o->beh;

    b->state = 0;
    ConditionUpdateBit(&o->st, 3, 0);
    o->unk288 = 0;
    return 1;
}

int HostRideCHandle41D(void* data) {
    Msg41D* m = (Msg41D*)data;
    Actor* o = m->actor;
    Beh* b;

    if (fn_80019F88(data, o) != 0) {
        return 1;
    }
    b = o->beh;
    switch (m->cmd) {
    case 0x63F94A69:
        if (m->arg != -1) {
            o->unk17C = 2;
            o->unk184 = m->arg;
            o->f174 = 3.4028235e38f;
            o->flags16C |= 2;
            o->flags16C |= 1;
            o->flags16C |= 0x10;
            o->unk178 = 1;
        } else {
            fn_800B35D0(o);
            RangeResetPending(&o->st);
        }
        return 1;
    case 0x21D542:
        if (m->arg != 0) {
            b->flags |= 1;
            ConditionSetBit(&o->st, 0x1A, 1);
            if (b->state != 0) {
                gHostRideCTypes[b->state].check(o);
                gHostRideCTypes[0].enter(o);
            }
        } else {
            b->flags &= ~1;
            ConditionSetBit(&o->st, 0x1A, 0);
        }
        return 1;
    case 0x7A1C0EF3:
        if (m->arg != 0) {
            b->flags |= 1;
            if (b->state != 1) {
                gHostRideCTypes[b->state].check(o);
                gHostRideCTypes[1].enter(o);
            }
        } else {
            b->flags &= ~1;
            ConditionSetBit(&o->st, 0x1A, 0);
        }
        return 1;
    case 0xF1C3E114:
        if (m->arg != 0) {
            b->flags &= ~4;
        } else {
            b->flags |= 4;
        }
        return 1;
    }
    return 0;
}

int HostRideCHandle40A(void* data) {
    Msg40A* m = (Msg40A*)data;
    Actor* o = m->actor;
    Beh* b = o->beh;
    Msg400 q;
    int kind;
    Actor* player;

    q.actor = m->actor;
    q.ownerId = OwnerId((Owner*)o);
    m->actor->msgFn(0x400, &q);
    if (ActorPlayAnim(OwnerId((Owner*)o), StrHash("QUERY", m->anim), m->unk04) != 0) {
        m->result = 0;
        return 1;
    }
    if (MemFlagIsSet(&o->st, 0xB) != 0) {
        return 1;
    }
    if (o->f904 > 0.0f) {
        if (HasCondition(&o->st, 0xA) == 0) {
            if (q.flags & 2) {
                if (m->anim == 0x3CA34BD6) {
                    if (MemFlagIsSet(&o->st, 0x20) != 0) {
                        Msg40B mm;

                        if (GetPlayer() == 0) {
                            return 0;
                        }
                        if (HasCondition(&o->st, 0xB) != 0) {
                            return 0;
                        }
                        mm.unk4 = 0;
                        mm.arg = -1;
                        m->result = 0x272CAC;
                        mm.anim = (u32)m->result;
                        SendMsg(m->unk04, 0x40B, &mm);
                        return 1;
                    } else {
                        m->result = 0x1379C466;
                        return 1;
                    }
                } else if (m->anim == 0x7C44EC3D) {
                    Msg40B mm;

                    if (fn_80049940() != 0) {
                        m->result = 0xA53F7E23;
                        return 1;
                    }
                    kind = fn_800B26B8(o);
                    player = GetPlayer();
                    if (player == 0) {
                        return 0;
                    }
                    if (kind != 4 && fn_800B22F8(o, kind) <= 1) {
                        return 0;
                    }
                    if (player->unk26E4 != 0 &&
                        (int)((u32)player->unk26E4->id >> 24) == 1) {
                        return 0;
                    }
                    if (player->hosts[PlayerInHost(player)]->kind == 0x4624485 ||
                        player->hosts[PlayerInHost(player)]->kind == 0x13C05 ||
                        player->hosts[PlayerInHost(player)]->kind == 0xE0AB9B4F ||
                        player->hosts[PlayerInHost(player)]->kind == 0xFFF5) {
                        return 0;
                    }
                    if (HasCondition(&o->st, 0xB) != 0) {
                        return 0;
                    }
                    if (MemFlagIsSet(&o->st, 0x1E) != 0) {
                        return 0;
                    }
                    if (b->state == 0) {
                        mm.unk4 = 0;
                        mm.arg = -1;
                        m->result = 0x272CAC;
                        mm.anim = (u32)m->result;
                        SendMsg(m->unk04, 0x40B, &mm);
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

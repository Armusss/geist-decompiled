// u_8028C54C.cpp: rideable-host behaviour state machine (0x8028C54C-0x8028DA1C).
// A host that can be ridden moves through three states; the table at 0x803C2A48
// holds each state's enter/leave/update callbacks.  Names are descriptive guesses;
// the code is matched against the retail DOL.

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
        h = h * 31 + (u8)toupper(s[i]);
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
        h = h * 31 + (unsigned char)toupper(s[i]);
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

// A ride target, as returned by the condition system.
struct Target {
    char pad0[0x24];
    int x;              // 0x24
    int y;              // 0x28
    int z;              // 0x2C
    char pad30[8];
    int id;             // 0x38
};

// Per-actor ride state (VAlloc'd, 0x1C bytes).
struct RideData {
    int state;          // 0x00
    int unk04;          // 0x04
    int unk08;          // 0x08
    float f0C;          // 0x0C
    float f10;          // 0x10
    float f14;          // 0x14
    float f18;          // 0x18
};

struct Actor {
    char pad00[0x30];
    int (*msgFn)(int msg, void* data);          // 0x30
    char pad34[0x50];
    float f84;                                  // 0x84
    float f88;                                  // 0x88
    float f8C;                                  // 0x8C
    char pad90[0x3C];
    int iCC;                                    // 0xCC
    char padD0[4];
    int iD4;                                    // 0xD4
    char padD8[0x24];
    int cbFC;                                   // 0xFC
    char pad100[4];
    int i104;                                   // 0x104
    char pad108[4];
    int cb10C;                                  // 0x10C
    char pad110[0x20];
    RideData* beh;                              // 0x130
    char pad134[0xE8];
    int i21C;                                   // 0x21C
    int i220;                                   // 0x220
    char pad224[8];
    Target* pTarget;                            // 0x22C
    char pad230[0x38];
    int i268;                                   // 0x268
    int i26C;                                   // 0x26C
    char st[0x18];                              // 0x270
    int i288;                                   // 0x288
    char pad28C[0x38];
    int unk2C4;                                 // 0x2C4
    char pad2C8[0x334];
    int cb5FC;                                  // 0x5FC
    int cb600;                                  // 0x600
    int cb604;                                  // 0x604
    char pad608[0x28C];
    u8 unk894[0x70];                            // 0x894
    float f904;                                 // 0x904
    char pad908[0x14];
    float f91C;                                 // 0x91C
    short s920;                                 // 0x920
    short s922;                                 // 0x922
};

// Enter/leave/update/name for one ride state.
struct RideType {
    int (*enter)(Actor* actor);
    int (*leave)(Actor* actor);
    int (*update)(void* actor);
    const char* name;
};

// Save-game stream handed to the pack/unpack callbacks.
struct HostStream {
    Actor* actor;
    int remain;
    u8* ptr;
    int count;
    int status;
};

// Payload of message 0x400 (rider query).
struct Msg400 {
    Actor* actor;       // 0x00
    int ownerId;        // 0x04
    int unk08;          // 0x08
    int state;          // 0x0C
    float f10;          // 0x10
    float f14;          // 0x14
    int flags;          // 0x18
    int unk1C;          // 0x1C
};

// Payload of messages 0x40A/0x409.
struct Msg40A {
    Actor* actor;       // 0x00
    int unk04;          // 0x04
    u32 unk08;          // 0x08
    int unk0C;          // 0x0C
};

// Payload of message 0x41D.
struct Msg41D {
    Actor* actor;       // 0x00
    int key;            // 0x04
    int unk08;          // 0x08
    int value;          // 0x0C
};

// Payload of message 0x41E.  The answer word doubles as an output buffer
// for the name query.
struct Msg41E {
    Actor* actor;       // 0x00
    int key;            // 0x04
    int unk08;          // 0x08
    Buffer result;      // 0x0C
};

// Payload of messages 0xC04/0xC0D.
struct MsgC04 {
    Actor* actor;       // 0x00
    int unk04;          // 0x04
    int unk08;          // 0x08
    int unk0C;          // 0x0C
    int unk10;          // 0x10
    int unk14;          // 0x14
    int unk18;          // 0x18
};

extern "C" {
int SendMsg(int target, int msg, void* data);                // 0x800265D4
void* memcpy(void* dst, const void* src, u32 n);
String fn_800225C8(const String& a, const String& b);        // 0x800225C8
void fn_8028C188(Buffer* b, char* data, int len);            // 0x8028C188
int fn_80019F88(void* data, Actor* actor);                   // 0x80019F88
int fn_80049940(void);                                       // 0x80049940
void fn_8008F230(int anim, float f, int a, int b);           // 0x8008F230
void fn_8008F378(int anim);                                  // 0x8008F378
void fn_8008F4E8(int anim, int a);                           // 0x8008F4E8
void fn_8008F4EC(int anim, int a);                           // 0x8008F4EC
void fn_8008F4F0(int anim, int a, float f);                  // 0x8008F4F0
void fn_80090778(int anim, float f, int a, int b, int c, int d, int e); // 0x80090778
int fn_80090EA4(int owner, int a);                           // 0x80090EA4
void fn_800A7028(void* conditions);                          // 0x800A7028
int fn_800A70D0(Actor* actor);                               // 0x800A70D0
void fn_800A78A0(void* conditions, void* target);            // 0x800A78A0
Target* fn_800A7A18(void* conditions);                       // 0x800A7A18
void fn_800A7A54(void* conditions, int which, int value);    // 0x800A7A54
void fn_800A7B08(void* conditions, int which, int value);    // 0x800A7B08
int fn_800A7B78(void* conditions, int which);                // 0x800A7B78
int fn_800A8CDC(Actor* actor, void* target, float f);        // 0x800A8CDC
void fn_800B1AB8(Actor* actor, int phase, float f, int a);   // 0x800B1AB8
void fn_800B1ED4(Actor* actor, int phase);                   // 0x800B1ED4
int fn_800B1EF4(Actor* actor, int phase);                    // 0x800B1EF4
void fn_800B1F20(Actor* actor, int phase);                   // 0x800B1F20
int fn_800B22F8(Actor* actor, int phase);                    // 0x800B22F8
void fn_800B2418(Actor* actor, int phase, float f);          // 0x800B2418
int fn_800B2454(Actor* actor, int phase);                    // 0x800B2454
void fn_800B261C(Actor* actor, int phase, Target** out);     // 0x800B261C
int fn_800B26B8(Actor* actor);                               // 0x800B26B8
void fn_800B5028(Actor* actor);                              // 0x800B5028
void fn_800B6CAC(Actor* actor, void* fn);                    // 0x800B6CAC
int fn_800BA84C(int a);                                      // 0x800BA84C
int fn_800BAF40(Actor* actor);                               // 0x800BAF40
void fn_800BBCA8(Actor* actor);                              // 0x800BBCA8
void fn_800BC56C(Actor* actor, Target* target);              // 0x800BC56C
int fn_800BEF9C(int a);                                      // 0x800BEF9C
void fn_800C725C(Actor* actor);                              // 0x800C725C
int fn_800C748C(Actor* actor);                               // 0x800C748C
int fn_800CC984(Actor* actor);                               // 0x800CC984
int fn_800CE96C(Actor* actor);                               // 0x800CE96C
int fn_800CF064(Actor* actor);                               // 0x800CF064
void fn_80162610(Actor* actor, void* p, int a);              // 0x80162610
void fn_80162658(Actor* actor, void* p);                     // 0x80162658
}

// Mangled C++ callees.
int OwnerId(Owner* o);                                       // 0x800A3A40
int HasCondition(void* conditions, int which);               // 0x800A7AC4
int AssetFind(u32 table, u32 key);                           // 0x8007EB34
int ActorPlayAnim(int handle, u32 anim, int arg);            // 0x800C5654
Target* OwnerGetInfo(u32 id);                                // 0x8002394C
void* VAlloc(int size);                                      // 0x8009DA68
void VFree(void* p);                                         // 0x8009DA3C
void ActorAim(int* out, Actor* actor, int arg);              // 0x800B1A40

static int sAsset[2] = { -1, 0 };

int HostRideBMsg(int msg, void* data);
int HostRideBUpdate(void* actor);
void HostRideBFree(Actor* actor);
void HostRideBInit(Actor* actor);
int HostRideBState2Update(void* actor);
int HostRideBState2Leave(Actor* actor);
int HostRideBState2Enter(Actor* actor);
int HostRideBState1Update(void* actor);
int HostRideBState1Leave(Actor* actor);
int HostRideBState1Enter(Actor* actor);
int HostRideBState0Update(void* actor);
int HostRideBState0Leave(Actor* actor);
int HostRideBState0Enter(void* actor);
int HostRideBBehavior(void* actor);

static RideType gRideTypes[3] = {
    { (int (*)(Actor*))HostRideBState0Enter, HostRideBState0Leave, HostRideBState0Update, "" },
    { HostRideBState1Enter, HostRideBState1Leave, HostRideBState1Update, "" },
    { HostRideBState2Enter, HostRideBState2Leave, HostRideBState2Update, "" },
};

static inline Actor* GetActor(void* data) { return *(Actor**)data; }

// 0x8028C54C: message handler.
int HostRideBMsg(int msg, void* data) {
    void* p = *(void**)data;
    Actor* actor = (Actor*)p;

    switch (msg) {
    case 0x400: {
        Msg400* m = (Msg400*)data;
        m->ownerId = OwnerId((Owner*)actor);
        m->state = (actor->f904 > 0.0f) ? 1 : 0;
        m->f10 = 1.0f;
        m->f14 = 1.0f;
        m->flags = 2;
        m->unk1C = 0;
        return 1;
    }
    case 0x7102: {
        HostStream* s = (HostStream*)data;
        RideData* b = actor->beh;
        u32 hash;

        if (b == 0) {
            return 1;
        }
        if (s->remain < 0x20) {
            s->status = 0;
            return 1;
        }
        hash = StrHash("[CRDSAVE]");
        memcpy(s->ptr, &hash, 4);
        s->count += 4;
        s->remain -= 4;
        s->ptr += 4;
        memcpy(s->ptr, b, 0x1C);
        s->count += 0x1C;
        s->remain -= 0x1C;
        s->ptr += 0x1C;
        return 1;
    }
    case 0x7103: {
        HostStream* s = (HostStream*)data;
        RideData* b = actor->beh;
        int ok;

        if (b == 0) {
            return 1;
        }
        if (s->count < 0x20) {
            s->status = 0;
            return 1;
        }
        if (s->count < 4) {
            ok = 0;
        } else {
            u32* tag = (u32*)s->ptr;
            u32 hash;
            s->count += 4;
            s->remain -= 4;
            s->ptr += 4;
            hash = StrHash("[CRDSAVE]");
            if (*tag == hash) {
                ok = 1;
            } else {
                ok = 0;
            }
        }
        if (!ok) {
            return 1;
        }
        memcpy(b, s->ptr, 0x1C);
        s->count -= 0x1C;
        s->ptr += 0x1C;
        return 1;
    }
    case 0x41D: {
        Msg41D* m = (Msg41D*)data;
        RideData* b = actor->beh;

        if (fn_80019F88(data, actor)) {
            return 1;
        }
        switch (m->key) {
        case (int)0x8757F68E:
            b->unk08 = m->value;
            return 1;
        case (int)0xF1AA29A3:
            b->f0C = (float)m->value;
            return 1;
        case 0x28D2C00B:
            b->f10 = (float)m->value;
            return 1;
        case 0x178E2132:
            b->f14 = (float)m->value;
            return 1;
        case 0x63D91A9C:
            b->f18 = (float)m->value;
            return 1;
        }
        return 0;
    }
    case 0x41E: {
        Msg41E* m = (Msg41E*)data;
        RideData* b = actor->beh;

        switch (m->key) {
        case 0x63F94A69: {
            Target* t = fn_800A7A18(&actor->st);

            if (t != 0) {
                *(int*)&m->result = t->id;
            } else {
                *(int*)&m->result = -1;
            }
            return 1;
        }
        case 0x832EF50A:
            if (sAsset[0] == -1) {
                sAsset[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
            }
            *(int*)&m->result = sAsset[0];
            return 1;
        case (int)0xAD8576FA: {
            String a(gRideTypes[b->state].name);
            String nl("\n");
            const String& c = fn_800225C8(a, nl);
            fn_8028C188(&m->result, c.mData, c.mLength);
            return 0;
        }
        }
        return 0;
    }
    case 0x1800:
        fn_800C725C(actor);
        return 1;
    case 0x40A: {
        Msg40A* m = (Msg40A*)data;
        Msg400 q;

        q.actor = actor;
        q.ownerId = OwnerId((Owner*)actor);
        ((Actor*)m->actor)->msgFn(0x400, &q);
        if (ActorPlayAnim(OwnerId((Owner*)actor), StrHash("QUERY", m->unk08), m->unk04)) {
            m->unk0C = 0;
            return 1;
        }
        if (fn_80049940()) {
            if (m->unk08 == 0x7C44EC3D) {
                m->unk0C = 0xA53F7E23;
                return 1;
            }
            return 0;
        }
        return 1;
    }
    case 0x409: {
        Msg40A* m = (Msg40A*)data;
        return ActorPlayAnim(OwnerId((Owner*)actor), m->unk0C, m->unk08) != 0;
    }
    case 0x407:
        return fn_800A7B78(&actor->st, 0xA) != 0;
    case 0xC04:
    case 0xC0D: {
        MsgC04* m = (MsgC04*)data;

        if (actor->iD4 == -1) {
            actor->iD4 = fn_80090EA4(OwnerId((Owner*)actor), actor->iCC);
            if (actor->iD4 == -1) {
                return 0;
            }
        }
        if (HasCondition(&actor->st, 0xC)) {
            fn_800A7A54(&actor->st, 0xC, 0);
            fn_800A7A54(&actor->st, 0xE, 1);
            fn_8008F4E8(actor->iD4, 0);
            fn_8008F378(actor->iD4);
            fn_8008F4EC(actor->iD4, 0);
            fn_8008F230(actor->iD4, (float)fn_800BA84C(0xA), 0, 1);
        } else {
            fn_8008F230(actor->iD4, (float)fn_800BA84C(0xA), 0, 1);
        }
        fn_800A7A54(&actor->st, 0xC, 0);
        fn_800A7A54(&actor->st, 0xD, 1);
        {
            int denom = (m->unk10 != 0) ? m->unk10 : 0x1000;
            fn_80090778(actor->iD4, (float)denom / 4096.0f, 2, m->unk04, m->unk0C,
                        m->unk14, m->unk18);
        }
        fn_8008F4F0(actor->iD4, 2, 1.0f);
        return 1;
    }
    }
    return 0;
}

// 0x8028CE44: per-frame update.
int HostRideBUpdate(void* p) {
    Actor* a = (Actor*)p;
    RideData* b;
    int result;
    int aim[3];
    float dist;
    float dy, dx, dz;
    Target* t;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (HasCondition(&a->st, 9) || HasCondition(&a->st, 0xA) || HasCondition(&a->st, 0xB)) {
        fn_80162658(a, a->unk894);
        return 1;
    }
    if (a->i268 > fn_800BEF9C(a->unk2C4)) {
        a->i268 = fn_800BEF9C(a->unk2C4);
    }
    result = gRideTypes[b->state].update(a);
    if (fn_800BAF40(a) == -1) {
        a->i288 = 0;
        a->i26C = 0;
    } else if (b->unk08 != -1) {
        t = OwnerGetInfo(b->unk08);
        if (t != 0) {
            ActorAim(aim, a, 1);
            dy = (float)t->y - a->f88;
            dx = (float)t->x - a->f84;
            dz = (float)t->z - a->f8C;
            dist = dy * (float)aim[1] + dx * (float)aim[0] + dz * (float)aim[2];
            if (a->i21C == 1) {
                if (dist > b->f10 * b->f10) {
                    a->i21C = 2;
                }
            } else if (a->i21C == 2) {
                if (dist < b->f0C * b->f0C) {
                    a->i21C = 1;
                } else if (dist > b->f14 * b->f14) {
                    a->i21C = 3;
                }
            } else if (a->i21C == 3) {
                if (dist < b->f18 * b->f18) {
                    a->i21C = 2;
                }
            }
        }
    }
    fn_80162658(a, a->unk894);
    if (result != 0) {
        int next = HostRideBBehavior(a);
        if (next != b->state) {
            if (gRideTypes[b->state].leave(a)) {
                if (!gRideTypes[next].enter(a)) {
                    gRideTypes[b->state].enter(a);
                }
            }
        }
    }
    return 1;
}

// 0x8028D154: destroy callback.
void HostRideBFree(Actor* a) {
    a->i104 = 0;
    VFree(a->beh);
    a->beh = 0;
}

// 0x8028D194: init callback.
void HostRideBInit(Actor* a) {
    RideData* b;

    a->i104 = (int)HostRideBUpdate;
    b = (RideData*)VAlloc(0x1C);
    if (b == 0) {
        a->beh = 0;
        return;
    }
    a->beh = b;
    a->cbFC = (int)fn_800CE96C;
    a->cb604 = (int)fn_800CF064;
    a->cb5FC = (int)fn_800CC984;
    a->cb600 = (int)fn_800CC984;
    a->f91C = 0.2f;
    a->s920 = 5;
    b = a->beh;
    b->state = 0;
    b->unk04 = 0;
    b->unk08 = -1;
    b->f0C = 0.0f;
    b->f10 = 0.0f;
    b->f14 = 0.0f;
    b->f18 = 0.0f;
    fn_800B6CAC(a, (void*)fn_800B5028);
    fn_800A7B08(&a->st, 6, 1);
    gRideTypes[b->state].enter(a);
    sAsset[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
}

// 0x8028D2AC: state 2 update.
int HostRideBState2Update(void* p) {
    Actor* a = (Actor*)p;
    int phase;
    Target* target;

    if (a == 0) {
        return 0;
    }
    phase = fn_800B26B8(a);
    if (phase != 4) {
        fn_800B261C(a, phase, &target);
        if (target == 0 || fn_800B22F8(a, phase) > 1) {
            fn_800A7028(&a->st);
            return 1;
        }
        if (fn_800B1EF4(a, phase) == 0) {
            fn_800B2418(a, phase, 10000.0f);
            fn_800B1F20(a, phase);
            fn_800B1AB8(a, phase, -1.0f, 1);
            fn_800A78A0(&a->st, target);
        } else {
            if (fn_800B2454(a, phase)) {
                return 1;
            }
            fn_800B2418(a, phase, 10000.0f);
        }
    } else {
        fn_800A7028(&a->st);
        return 1;
    }
    if (a->s922 == a->s920 - 1) {
        if (fn_800A8CDC(a, target, 0.8f)) {
            fn_80162610(a, a->unk894, 1);
        }
    }
    if (a->i220 == 2) {
        a->cb10C = (int)fn_800C748C;
        if (fn_800A7A18(&a->st) != a->pTarget || a->i288 != a->i21C) {
            if (a->i21C != 0) {
                a->i288 = a->i21C;
                fn_800BC56C(a, a->pTarget);
            }
        }
        if (fn_800BAF40(a) == -1 && a->i21C != 0) {
            a->i288 = 0;
        }
    }
    return 0;
}

// 0x8028D488: state 2 leave.
int HostRideBState2Leave(Actor* a) {
    a->cb10C = (int)fn_800A70D0;
    fn_800A7B08(&a->st, 0x16, 0);
    fn_800A7B08(&a->st, 3, 0);
    fn_800A7028(&a->st);
    fn_80162610(a, a->unk894, 0);
    return 1;
}

// 0x8028D4F8: state 2 enter.
int HostRideBState2Enter(Actor* a) {
    a->beh->state = 2;
    fn_800A7B08(&a->st, 0x16, 1);
    fn_800A7B08(&a->st, 3, 1);
    return 1;
}

// 0x8028D550: state 1 update.
int HostRideBState1Update(void* p) {
    Actor* a = (Actor*)p;
    int phase;
    Target* target;

    if (a == 0) {
        return 0;
    }
    phase = fn_800B26B8(a);
    if (phase != 4) {
        fn_800B261C(a, phase, &target);
        if (fn_800B1EF4(a, phase) == 0) {
            if (fn_800B22F8(a, phase) <= 2) {
                return 1;
            }
            fn_800B1F20(a, phase);
        }
    }
    if (a->i220 == 0 && !fn_800A7B78(&a->st, 5)) {
        return 1;
    }
    if (a->i220 == 2) {
        a->cb10C = (int)fn_800C748C;
        if (fn_800A7A18(&a->st) != a->pTarget || a->i288 != a->i21C) {
            fn_800A78A0(&a->st, a->pTarget);
            if (a->i21C != 0) {
                a->i288 = a->i21C;
                fn_800BC56C(a, a->pTarget);
            }
        }
        if (fn_800BAF40(a) == -1 && a->i21C != 0) {
            fn_800A78A0(&a->st, 0);
            a->i288 = 0;
        }
    }
    return 0;
}

// 0x8028D6A4: state 1 leave.
int HostRideBState1Leave(Actor* a) {
    a->cb10C = (int)fn_800A70D0;
    if (a->i220 == 4) {
        a->i220 = 0;
    }
    if (HasCondition(&a->st, 0xD)) {
        fn_800A7A54(&a->st, 0xD, 0);
    }
    return 1;
}

// 0x8028D714: state 1 enter.
int HostRideBState1Enter(Actor* a) {
    a->beh->state = 1;
    return 1;
}

// 0x8028D728: state 0 update.
int HostRideBState0Update(void* p) {
    Actor* a = (Actor*)p;
    int phase;

    if (a == 0) {
        return 0;
    }
    if (a->i220 != 0) {
        return 1;
    }
    phase = fn_800B26B8(a);
    if (phase != 4) {
        if (fn_800B1EF4(a, phase) == 0) {
            return 1;
        }
    } else {
        fn_800A7B08(&a->st, 4, 0);
        fn_800A7028(&a->st);
    }
    fn_800BBCA8(a);
    return 0;
}

// 0x8028D7BC: state 0 leave.
int HostRideBState0Leave(Actor* a) {
    fn_800A7B08(&a->st, 4, 0);
    return 1;
}

// 0x8028D7EC: state 0 enter.
int HostRideBState0Enter(void* p) {
    Actor* a;
    int phase;
    Target* target;
    Msg400 q;

    if (!p) {
        return 0;
    }
    a = (Actor*)p;
    RideData* b = a->beh;
    a->i288 = 0;
    phase = fn_800B26B8(a);
    if (phase != 4) {
        fn_800B261C(a, phase, &target);
        if (target != 0 && SendMsg(target->id, 0x400, &q) && (q.flags & 2)) {
            fn_800A78A0(&a->st, target);
        } else {
            fn_800A7028(&a->st);
        }
    } else {
        fn_800A7028(&a->st);
    }
    fn_800A7B08(&a->st, 3, 0);
    b->state = 0;
    return 1;
}

// 0x8028D8C4: pick the state to switch to.
int HostRideBBehavior(void* p) {
    Actor* a;
    int phase;
    RideData* b;
    int n;
    int old;
    Target* target = 0;

    if (p == 0) {
        return 0;
    }
    a = (Actor*)p;
    b = a->beh;
    old = b->state;
    phase = fn_800B26B8(a);
    if (phase != 4) {
        n = fn_800B22F8(a, phase);
        fn_800B1ED4(a, phase);
    }
    if (fn_800A7B78(&a->st, 0x15) && a->i220 != 0) {
        return 2;
    }
    if (phase != 4) {
        fn_800B261C(a, phase, &target);
        fn_800B1ED4(a, phase);
        if (target == 0) {
            return 0;
        }
        if (n <= 1) {
            return 2;
        }
        if (n == 3) {
            if (fn_800B1EF4(a, phase) == 0) {
                fn_800B1F20(a, phase);
                return (old != b->state) ? b->state : 0;
            }
        }
        fn_800B1F20(a, phase);
    }
    if (old != b->state) {
        return b->state;
    }
    return a->i220 != 0;
}

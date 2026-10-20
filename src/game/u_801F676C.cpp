// u_801F676C.cpp: rideable-host behaviour state machine, variant A (0x801F676C-0x801F7F48).
// A host that can be ridden moves through three states; the table at 0x803B9C38 holds each
// state's enter/leave/update callbacks.  The unit also answers the animation, name and
// condition queries sent by the rider.  Names are descriptive guesses; the code is matched
// against the retail DOL.

#include <math.h>

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

// The actor-type info block hung off the actor at 0xC4.
struct ActorC4 {
    char pad0[0xC];
    u32 type;           // 0x0C
};

// Per-actor ride state (VAlloc'd, 0xC bytes).
struct RideData {
    float f0;           // 0x00
    u8 state;           // 0x04
    u8 flags;           // 0x05
    char pad06[2];
    float f8;           // 0x08
};

struct Mtx34 {
    float m[12];
};

// One entry of the model's bone table, 0x30 bytes each.
struct BoneInfo {
    char pad[0x2C];
    int f2C;
};

struct Actor {
    char pad00[0x24];
    int x;                                      // 0x24
    int y;                                      // 0x28
    int z;                                      // 0x2C
    int (*msgFn)(int msg, void* data);          // 0x30
    char pad34[4];
    int id;                                     // 0x38
    char pad3C[0x48];
    float f84;                                  // 0x84
    float f88;                                  // 0x88
    float f8C;                                  // 0x8C
    char pad90[0x34];
    ActorC4* pC4;                               // 0xC4
    char padC8[4];
    int model;                                  // 0xCC
    char padD0[0x20];
    int unkF0;                                  // 0xF0
    char padF4[0x10];
    int i104;                                   // 0x104
    char pad108[0x28];
    RideData* beh;                              // 0x130
    char pad134[4];
    float f138;                                 // 0x138
    char pad13C[0x30];
    int i16C;                                   // 0x16C
    char pad170[4];
    float f174;                                 // 0x174
    int i178;                                   // 0x178
    int i17C;                                   // 0x17C
    char pad180[4];
    int i184;                                   // 0x184
    char pad188[0xE8];
    char st[0x18];                              // 0x270
    int i288;                                   // 0x288
    char pad28C[0x328];
    float f5B4;                                 // 0x5B4
    char pad5B8[0x20];
    float f5D8;                                 // 0x5D8
    char pad5DC[0xC];
    int i5E8;                                   // 0x5E8
    char pad5EC[0x1C];
    int i608;                                   // 0x608
    char pad60C[0x288];
    u8 unk894[0x58];                            // 0x894
    int i8EC;                                   // 0x8EC
    int i8F0;                                   // 0x8F0
    char pad8F4[0x10];
    float f904;                                 // 0x904
    int i908;                                   // 0x908
    char pad90C[0x1DB8];
    void* pHost[4];                             // 0x26C4
    char pad26D4[0x10];
    Actor* p26E4;                               // 0x26E4
};

// Enter/leave/update/name for one ride state.
struct RideType {
    int (*enter)(Actor* actor);
    int (*leave)(Actor* actor);
    int (*update)(Actor* actor);
    const char* name;
};

// Payload of message 0x400 (rider query).
struct Msg400 {
    Actor* actor;       // 0x00
    int ownerId;        // 0x04
    int unk08;          // 0x08
    int unk0C;          // 0x0C
    float f10;          // 0x10
    float f14;          // 0x14
    int flags;          // 0x18
    int unk1C;          // 0x1C
};

// Payload of message 0x409.
struct Msg409 {
    Actor* actor;       // 0x00
    Target* target;     // 0x04
    int arg;            // 0x08
    int anim;           // 0x0C
};

// Payload of message 0x40A.
struct Msg40A {
    Actor* actor;       // 0x00
    int unk04;          // 0x04
    u32 unk08;          // 0x08
    int unk0C;          // 0x0C
};

// Payload of message 0x41D.  The answer word doubles as an output buffer
// for the name query.
struct Msg41D {
    Actor* actor;       // 0x00
    int key;            // 0x04
    int unk08;          // 0x08
    Buffer result;      // 0x0C
};

// Payload of message 0x41E.
struct Msg41E {
    Actor* actor;       // 0x00
    int key;            // 0x04
    int unk08;          // 0x08
    int unk0C;          // 0x0C
};

// Payload of the 0xC14 notifications.
struct MsgC14 {
    int unk0;           // 0x00
    int unk4;           // 0x04
    int unk8;           // 0x08
    int unk0C;          // 0x0C
};

// Payload of the 0x40B ride request.
struct Msg40B {
    int unk0;           // 0x00
    int unk4;           // 0x04
    int unk8;           // 0x08
    int unk0C;          // 0x0C
    int unk10;          // 0x10
};

extern "C" {
int SendMsg(int target, int msg, void* data);                // 0x800265D4
int rand(void);
String fn_800225C8(const String& a, const String& b);        // 0x800225C8
void fn_8028C188(Buffer* b, char* data, int len);            // 0x8028C188
void fn_8028C258(Buffer* b, const char* s);                  // 0x8028C258
int fn_80019F88(void* data, Actor* actor);                   // 0x80019F88
int fn_80049940(void);                                       // 0x80049940
char* fn_8003A3F0(int a, int b, int c);                      // 0x8003A3F0
void fn_800A025C(float* v, Mtx34* m, int a);                 // 0x800A025C
void fn_800A7028(void* conditions);                          // 0x800A7028
void fn_800A78A0(void* conditions, void* target);            // 0x800A78A0
void fn_800A78E4(void* conditions, float* v);                // 0x800A78E4
Target* fn_800A7A18(void* conditions);                       // 0x800A7A18
int fn_800A7A3C(void* conditions);                           // 0x800A7A3C
void fn_800A7A54(void* conditions, int which, int value);    // 0x800A7A54
void fn_800A7B08(void* conditions, int which, int value);    // 0x800A7B08
int fn_800A7B78(void* conditions, int which);                // 0x800A7B78
int fn_800A8CDC(Actor* actor, void* target, float f);        // 0x800A8CDC
float fn_800AA6C4(void* target, float* v);                   // 0x800AA6C4
void fn_800B1AB8(Actor* actor, int phase, float f, int a);   // 0x800B1AB8
int fn_800B1EF4(Actor* actor, int phase);                    // 0x800B1EF4
void fn_800B1F20(Actor* actor, int phase);                   // 0x800B1F20
int fn_800B22F8(Actor* actor, int phase);                    // 0x800B22F8
void fn_800B2418(Actor* actor, int phase, float f);          // 0x800B2418
int fn_800B2454(Actor* actor, int phase);                    // 0x800B2454
void fn_800B261C(Actor* actor, int phase, Target** out);     // 0x800B261C
int fn_800B26B8(Actor* actor);                               // 0x800B26B8
void fn_800B35D0(Actor* actor);                              // 0x800B35D0
int fn_800B7098(int a, int b, int c, int d);                 // 0x800B7098
void fn_800C9ED0(Actor* actor);                              // 0x800C9ED0
void fn_800C9FC0(Actor* actor, int id, float f);             // 0x800C9FC0
int fn_800E0DBC(void* v);                                    // 0x800E0DBC
void fn_80162610(Actor* actor, void* p, int a);              // 0x80162610
void fn_80162658(Actor* actor, void* p);                     // 0x80162658
int fn_80162940(void* p, int a);                             // 0x80162940
int fn_8020B97C(void* target);                               // 0x8020B97C
int fn_8029B070(Actor* actor, u32 key);                      // 0x8029B070
int fn_8029B140(Actor* actor, u32 key);                      // 0x8029B140
int fn_8029B3B0(Actor* actor, u32 key);                      // 0x8029B3B0
}

// Mangled C++ callees.
int OwnerId(Owner* o);                                       // 0x800A3A40
int HasCondition(void* conditions, int which);               // 0x800A7AC4
float OwnerValue(Owner* o, void* p);                         // 0x800AA050
int AssetFind(u32 table, u32 key);                           // 0x8007EB34
int ActorPlayAnim(int handle, u32 anim, int arg);            // 0x800C5654
Target* OwnerGetInfo(u32 id);                                // 0x8002394C
void* VAlloc(int size);                                      // 0x8009DA68
void VFree(void* p);                                         // 0x8009DA3C
void ActorGetMatrix(Mtx34* m, Actor* actor);                 // 0x8009ED70
int ModelFindBone(int model, int bone);                      // 0x80018AD4
int ModelAnimate(int model, char* p, int anim);              // 0x8003AA9C
int PlayerInHost(Actor* player);                             // 0x80049750
Actor* GetPlayer(void);                                      // 0x80049948
int Fn800F7DEC(int a, int b, int c, int d, void* e);         // 0x800F7DEC

static int sAsset[2] = { -1, 0 };

int HostRideAMsg(int msg, void* data);
int HostRideAUpdate(void* p);
void HostRideAFree(Actor* actor);
void HostRideAInit(void* p);
int HostRideAState2Update(Actor* actor);
int HostRideAState2Leave(Actor* actor);
int HostRideAState2Enter(Actor* actor);
int HostRideAState1Update(void* p);
int HostRideAState1Leave(void* p);
int HostRideAState1Enter(Actor* actor);
int HostRideAState0Update(void* p);
int HostRideAState0Leave(Actor* actor);
int HostRideAState0Enter(Actor* actor);
int HostRideAMsgSet(void* data);
int HostRideAMsgQuery(void* data);
int HostRideABehavior(void* data);

static RideType gRideTypes[3] = {
    { HostRideAState0Enter, HostRideAState0Leave, (int (*)(Actor*))HostRideAState0Update, "" },
    { HostRideAState1Enter, (int (*)(Actor*))HostRideAState1Leave, (int (*)(Actor*))HostRideAState1Update, "" },
    { HostRideAState2Enter, HostRideAState2Leave, HostRideAState2Update, "" },
};

// 0x801F676C: message handler.
int HostRideAMsg(int msg, void* data) {
    switch (msg) {
    case 0x400: {
        Msg400* m = (Msg400*)data;
        Actor* a = (Actor*)m->actor;

        m->ownerId = OwnerId((Owner*)a);
        m->unk0C = (a->f904 > 0.0f) ? 1 : 0;
        m->f10 = 1.0f;
        m->f14 = 1.0f;
        m->flags = 2;
        m->unk1C = 0;
        return 1;
    }
    case 0x40A:
        return HostRideABehavior(data);
    case 0x409: {
        Msg409* m = (Msg409*)data;
        RideData* b;
        Actor* a = (Actor*)m->actor;
        b = a->beh;

        if (ActorPlayAnim(OwnerId((Owner*)a), m->anim, m->arg)) {
            return 1;
        }
        switch (m->anim) {
        case 0x272CAC:
            if (gRideTypes[b->state].leave(a)) {
                fn_800A78A0(&a->st, m->target);
                if (!gRideTypes[2].enter(a)) {
                    gRideTypes[b->state].enter(a);
                }
            }
            return 1;
        }
        return 0;
    }
    case 0x41D:
        return HostRideAMsgSet(data);
    case 0x41E:
        return HostRideAMsgQuery(data);
    case 0x407: {
        Msg40A* m = (Msg40A*)data;
        RideData* b;
        Actor* a = (Actor*)m->actor;
        ActorC4* c4 = a->pC4;
        b = a->beh;

        if (c4->type == 0x12EC1 || c4->type == 0x906E8474) {
            if (!fn_800A7B78(&a->st, 0x28)) {
                if (rand() / 32767.0f > a->f5B4) {
                    if (a->i908 != 0x8CD18EE1) {
                        if (fn_8029B3B0(a, 0x13E6B)) {
                            if (a->id != -1) {
                                MsgC14 r;
                                r.unk8 = a->id;
                                r.unk0C = 0x93F5146E;
                                r.unk4 = 0;
                                SendMsg(a->id, 0xC14, &r);
                            }
                        }
                    }
                }
            }
        }
        if (b->state == 1) {
            b->f0 = 0.0f;
        }
        return 0;
    }
    }
    return 0;
}

// 0x801F69F4: per-frame update.
int HostRideAUpdate(void* p) {
    int next;
    Actor* a = (Actor*)p;
    RideData* b = a->beh;

    if (HasCondition(&a->st, 0xA) || HasCondition(&a->st, 0xB)) {
        fn_80162658(a, a->unk894);
        return 1;
    }
    next = gRideTypes[b->state].update(a);
    if (next != b->state && !(b->flags & 1)) {
        if (gRideTypes[b->state].leave(a)) {
            gRideTypes[next].enter(a);
            fn_800A7A54(&a->st, 0x40, 0);
            fn_800A7A54(&a->st, 0x41, 0);
            fn_800A7A54(&a->st, 0x43, 0);
            fn_800A7A54(&a->st, 0x42, 0);
        }
    }
    fn_80162658(a, a->unk894);
    return 1;
}

// 0x801F6B40: destroy callback.
void HostRideAFree(Actor* a) {
    VFree(a->beh);
    a->i104 = 0;
    a->beh = 0;
}

// 0x801F6B7C: init callback.
void HostRideAInit(void* p) {
    Actor* a = (Actor*)p;
    RideData* b = (RideData*)VAlloc(0xC);
    Msg400 q;
    Mtx34 mtx;
    float v[3];

    if (b == 0) {
        a->beh = 0;
        return;
    }
    if (a->i608 != -1) {
        if (SendMsg(a->i608, 0x400, &q) && q.actor != 0) {
            ActorGetMatrix(&mtx, q.actor);
            fn_800A025C(v, &mtx, 1);
            v[0] *= 1000.0f;
            v[1] *= 1000.0f;
            v[2] *= 1000.0f;
            v[0] += a->f84;
            v[1] += a->f88;
            v[2] += a->f8C;
            fn_800A78E4(&a->st, v);
            fn_800A7B08(&a->st, 4, 1);
        }
    }
    fn_800A7B08(&a->st, 0x16, 0);
    a->beh = b;
    b->f0 = 0.0f;
    b->state = 0;
    b->flags = 0;
    sAsset[0] = AssetFind(0xF1754BF2, 0x21C5E0);
    {
        BoneInfo* bones = (BoneInfo*)fn_8003A3F0(a->model, a->unkF0, ModelAnimate(a->model, 0, 0));
        int bone = ModelFindBone(a->model, sAsset[0]);

        b->f8 = (float)bones[bone].f2C;
    }
    gRideTypes[b->state].enter(a);
}

// 0x801F6D3C: state 2 update.
int HostRideAState2Update(Actor* a) {
    Target* t;
    RideData* b = a->beh;

    t = fn_800A7A18(&a->st);

    if (t == 0 || (int)((u32)t->id >> 24) != 2) {
        int id = a->id;
        Actor* p = GetPlayer();

        if (p->id != -1) {
            MsgC14 r;
            r.unk8 = id;
            r.unk0C = 0xAF699ED0;
            r.unk4 = 0;
            SendMsg(p->id, 0xC14, &r);
        }
        b->flags |= 2;
        return 1;
    }
    if (!(b->flags & 2)) {
        int result;
        int aid;

        b->flags |= 2;
        result = ActorPlayAnim(a->id, 0xDC35206A, t->id);
        aid = a->id;
        if (t->id != -1) {
            MsgC14 r;
            r.unk4 = result;
            r.unk8 = aid;
            r.unk0C = 0xAF699ED0;
            SendMsg(t->id, 0xC14, &r);
        }
    }
    return fn_8020B97C(t) == 0 ? 1 : 0;
}

// 0x801F6E50: state 2 leave.
int HostRideAState2Leave(Actor* a) {
    RideData* b = a->beh;

    if (!(b->flags & 2)) {
        return 0;
    }
    b->flags &= ~2;
    fn_800A7028(&a->st);
    return 1;
}

// 0x801F6E98: state 2 enter.
int HostRideAState2Enter(Actor* a) {
    RideData* b = a->beh;

    if (fn_800A7A18(&a->st) == 0) {
        return 0;
    }
    a->i288 = 0;
    b->state = 2;
    b->flags &= ~2;
    return 1;
}

// 0x801F6F04: state 1 leave.
int HostRideAState1Leave(void* p) {
    Actor* a = (Actor*)p;
    RideData* b = a->beh;

    b->f0 = 3500.0f * (rand() / 32767.0f) + 1500.0f;
    fn_800A7B08(&a->st, 4, 0);
    fn_8029B070(a, 0x73B1E248);
    if (a->pC4->type == 0x12EC1) {
        if (!fn_800A7B78(&a->st, 0x28)) {
            if (rand() / 32767.0f < a->f5B4) {
                if (a->i908 != 0x8CD18EE1) {
                    if (fn_8029B3B0(a, 0x13E6B)) {
                        if (a->id != -1) {
                            MsgC14 r;
                            r.unk8 = a->id;
                            r.unk0C = 0x93F5146E;
                            r.unk4 = 0;
                            SendMsg(a->id, 0xC14, &r);
                        }
                    }
                }
            }
        }
    }
    return 1;
}

// 0x801F7048: state 1 update.
int HostRideAState1Update(void* p) {
    Actor* a = (Actor*)p;
    RideData* b = a->beh;
    int phase = fn_800B26B8(a);
    Target* t;
    Target* target;

    if (phase != 4) {
        if (fn_800B1EF4(a, phase) == 0) {
            fn_800B261C(a, phase, &t);
            fn_800B22F8(a, phase);
            if (t == 0 || fn_800B22F8(a, phase) > 1) {
                return 0;
            }
            fn_800B2418(a, phase, 15000.0f);
            fn_800B1F20(a, phase);
            fn_800B1AB8(a, phase, -1.0f, 1);
            fn_800B261C(a, phase, &target);
            fn_800A78A0(&a->st, target);
            if (target != 0) {
                fn_800C9FC0(a, target->id, 10000.0f);
            }
        } else {
            fn_800B2418(a, phase, 15000.0f);
        }
    } else {
        return 0;
    }
    t = fn_800A7A18(&a->st);
    if (t == 0) {
        return 0;
    }
    if (!fn_8029B140(a, 0x73B1E248)) {
        return 0;
    }
    if (!HasCondition(&a->st, 8)) {
        if (!fn_800A8CDC(a, t, 0.8f)) {
            return 0;
        }
    }
    if (Fn800F7DEC(a->i8EC, a->i8F0, 0x20EE36, a->id, 0) == 0xC0BECF7E) {
        return 0;
    }
    fn_800C9ED0(a);
    if (fabsf(fn_800AA6C4(t, &a->f84)) < 5.0f) {
        b->f0 = (b->f0 < 800.0f) ? b->f0 : 800.0f;
    }
    if (!fn_800A7B78(&a->st, 0x28)) {
        if (!fn_800B7098(a->i608, a->id, t->id, 1) && a->id != -1) {
            MsgC14 r;
            r.unk8 = a->id;
            r.unk0C = 0x93F5146E;
            r.unk4 = 0;
            SendMsg(a->id, 0xC14, &r);
        }
    }
    if (fn_80162940(a->unk894, 1) && fn_80162940(a->unk894, 4)) {
        fn_80162610(a, a->unk894, 1);
    }
    b->f0 = b->f0 - a->f138;
    if (b->f0 <= 0.0f) {
        return 0;
    }
    return b->state;
}

// 0x801F7320: state 1 enter.
int HostRideAState1Enter(Actor* a) {
    RideData* b = a->beh;
    int phase = fn_800B26B8(a);

    if (phase != 4 && fn_800B22F8(a, phase) <= 1) {
        Target* target = 0;

        fn_800B261C(a, phase, &target);
        fn_800A78A0(&a->st, target);
    }
    b->state = 1;
    b->f0 = 2000.0f * (rand() / 32767.0f) + 4000.0f;
    fn_800A7B08(&a->st, 3, 1);
    fn_800A7B08(&a->st, 4, 1);
    fn_800A7A54(&a->st, 8, 1);
    return 1;
}

// 0x801F7418: state 0 leave.
int HostRideAState0Leave(Actor* a) {
    fn_800A7B08(&a->st, 4, 0);
    return 1;
}

// 0x801F7448: state 0 update.
int HostRideAState0Update(void* p) {
    Actor* a = (Actor*)p;
    RideData* b = a->beh;
    int phase = fn_800B26B8(a);
    int v1[3];
    int v2[3];

    if (phase != 4 && fn_800B22F8(a, phase) <= 1) {
        Target* target = 0;

        fn_800B261C(a, phase, &target);
        if (fn_800B2454(a, phase) == 0) {
            fn_800B2418(a, phase, 15000.0f);
        }
        if (target != 0) {
            fn_800A78A0(&a->st, target);
        }
        b->f0 = b->f0 - a->f138;
        if (b->f0 <= 0.0f) {
            if (fn_8029B3B0(a, 0x73B1E248)) {
                return 1;
            }
        }
    } else {
        b->f0 = 0.0f;
        if (fn_800A7A3C(&a->st) != 1) {
            fn_800A7028(&a->st);
        }
    }
    if (!fn_800A7B78(&a->st, 0x28)) {
        Target* t = fn_800A7A18(&a->st);

        if (t != 0) {
            if (!fn_800B7098(a->i608, a->id, t->id, 1) && a->id != -1) {
                MsgC14 r;
                r.unk8 = a->id;
                r.unk0C = 0x93F5146E;
                r.unk4 = 0;
                SendMsg(a->id, 0xC14, &r);
            }
        }
        if (a->i908 == 0x8CD18EE1) {
            Actor* pl = GetPlayer();

            if (pl != 0) {
                if (OwnerValue((Owner*)a, pl) > a->f5D8) {
                    Target* info;

                    if (a->i5E8 == -1 || (info = OwnerGetInfo(a->i5E8)) == 0) {
                        if (a->id != -1) {
                            MsgC14 r;
                            r.unk8 = a->id;
                            r.unk0C = 0x93F5146E;
                            r.unk4 = 0;
                            SendMsg(a->id, 0xC14, &r);
                        }
                    } else {
                        v1[0] = info->x - pl->x;
                        v1[1] = info->y - pl->y;
                        v1[2] = info->z - pl->z;
                        v2[0] = info->x - a->x;
                        v2[1] = info->y - a->y;
                        v2[2] = info->z - a->z;
                        if (fn_800E0DBC(v1) < fn_800E0DBC(v2) && a->id != -1) {
                            MsgC14 r;
                            r.unk8 = a->id;
                            r.unk0C = 0x93F5146E;
                            r.unk4 = 0;
                            SendMsg(a->id, 0xC14, &r);
                        }
                    }
                }
            }
        }
    }
    return b->state;
}

// 0x801F76F0: state 0 enter.
int HostRideAState0Enter(Actor* a) {
    a->beh->state = 0;
    fn_800A7B08(&a->st, 3, 0);
    fn_800A7B08(&a->st, 4, 1);
    a->i288 = 0;
    return 1;
}

// 0x801F7750: message 0x41D handler (get/set ride properties).
int HostRideAMsgSet(void* data) {
    Msg41D* m = (Msg41D*)data;
    Actor* a = (Actor*)m->actor;
    RideData* b = a->beh;

    switch (m->key) {
    case 0x832DF50A:
        *(int*)&m->result = sAsset[0];
        return 1;
    case 0x52AB9E79:
        *(float*)&m->result = b->f8;
        return 1;
    case (int)0xAD8576FA: {
        {
            String name(gRideTypes[b->state].name);
            String nl("\n");
            const String& c = fn_800225C8(name, nl);

            fn_8028C188(&m->result, c.mData, c.mLength);
        }
        if (fn_800A7B78(&a->st, 0x28)) {
            fn_8028C258(&m->result, "No Exit\n");
        }
        return 0;
    }
    case 0x21D542:
        if (*(int*)&m->result != 0) {
            b->flags |= 1;
            if (b->state != 0) {
                gRideTypes[b->state].leave(a);
                gRideTypes[0].enter(a);
            }
        } else {
            b->flags &= ~1;
        }
        return 1;
    case 0x7A1C0EF3:
        if (*(int*)&m->result != 0) {
            b->flags |= 1;
            if (b->state != 1) {
                gRideTypes[b->state].leave(a);
                gRideTypes[1].enter(a);
            }
        } else {
            b->flags &= ~1;
        }
        return 1;
    }
    return 0;
}

// 0x801F79C4: message 0x41E handler (query ride properties).
int HostRideAMsgQuery(void* data) {
    Msg41E* m = (Msg41E*)data;
    Actor* a = (Actor*)m->actor;
    RideData* b;

    if (fn_80019F88(data, a)) {
        return 1;
    }
    b = a->beh;
    switch (m->key) {
    case 0x63F94A69:
        if (m->unk0C != -1) {
            a->i17C = 2;
            a->i184 = m->unk0C;
            a->f174 = 3.4028235e38f;
            a->i16C |= 2;
            a->i16C |= 1;
            a->i16C |= 0x10;
            a->i178 = 1;
        } else {
            fn_800B35D0(a);
            fn_800A7028(&a->st);
        }
        return 1;
    case 0x21D542:
        if (m->unk0C != 0) {
            b->flags |= 1;
            if (b->state != 0) {
                gRideTypes[b->state].leave(a);
                gRideTypes[0].enter(a);
            }
        } else {
            b->flags &= ~1;
        }
        return 1;
    case 0x7A1C0EF3:
        if (m->unk0C != 0) {
            b->flags |= 1;
            if (b->state != 1) {
                gRideTypes[b->state].leave(a);
                gRideTypes[1].enter(a);
            }
        } else {
            b->flags &= ~1;
        }
        return 1;
    }
    return 0;
}

// 0x801F7BA8: decide which ride state to switch to.
int HostRideABehavior(void* data) {
    Msg40A* m = (Msg40A*)data;
    Actor* a = (Actor*)m->actor;
    RideData* b = a->beh;
    Msg400 q;

    q.actor = a;
    q.ownerId = OwnerId((Owner*)a);
    ((Actor*)m->actor)->msgFn(0x400, &q);
    if (ActorPlayAnim(OwnerId((Owner*)a), StrHash("QUERY", m->unk08), m->unk04)) {
        m->unk0C = 0;
        return 1;
    }
    if (fn_800A7B78(&a->st, 0xB)) {
        return 1;
    }
    if (a->f904 > 0.0f && !HasCondition(&a->st, 0xA) && (q.flags & 2)) {
        if (m->unk08 == 0x3CA34BD6) {
            if (fn_800A7B78(&a->st, 0x20)) {
                if (GetPlayer() == 0) {
                    return 0;
                }
                if (HasCondition(&a->st, 0xB)) {
                    return 0;
                }
                {
                    Msg40B r;

                    r.unk4 = 0;
                    r.unk0C = -1;
                    m->unk0C = 0x272CAC;
                    r.unk8 = m->unk0C;
                    SendMsg(m->unk04, 0x40B, &r);
                }
                return 1;
            } else {
                m->unk0C = 0x1379C466;
                return 1;
            }
        } else if (m->unk08 == 0x7C44EC3D) {
            if (fn_80049940()) {
                m->unk0C = 0xA53F7E23;
                return 1;
            }
            {
                int phase = fn_800B26B8(a);
                Actor* p = GetPlayer();

                if (p == 0) {
                    return 0;
                }
                if (phase != 4 && fn_800B22F8(a, phase) <= 1) {
                    return 0;
                }
                if (p->p26E4 != 0 && (int)((u32)p->p26E4->id >> 24) == 1) {
                    return 0;
                }
                if (*(u32*)p->pHost[PlayerInHost(p)] == 0x04624485 ||
                    *(u32*)p->pHost[PlayerInHost(p)] == 0x13C05 ||
                    *(u32*)p->pHost[PlayerInHost(p)] == 0xE0AB9B4F ||
                    *(u32*)p->pHost[PlayerInHost(p)] == 0xFFF5) {
                    return 0;
                }
                if (HasCondition(&a->st, 0xB)) {
                    return 0;
                }
                if (fn_800A7B78(&a->st, 0x1E)) {
                    return 0;
                }
                if (b->state == 0) {
                    Msg40B r;

                    r.unk4 = 0;
                    r.unk0C = -1;
                    m->unk0C = 0x272CAC;
                    r.unk8 = m->unk0C;
                    SendMsg(m->unk04, 0x40B, &r);
                    return 1;
                }
            }
        }
    }
    return 0;
}

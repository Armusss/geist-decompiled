// u_8023E7E0.cpp: rideable-host behaviour state machine D (0x8023E7E0-0x8023FA5C).
// A host that can be ridden runs a five-state enter/check/update machine (state block
// VAlloc'd, 0x1C bytes: timer, random threshold, state index, two distances, an
// accumulator and a flag).  The message handler answers the riding ghost's queries
// (0x400, 0x41D, 0x41E, 0xC14); the state table at 0x803BE540 holds each state's
// enter/check/update callbacks and name.  Names are descriptive guesses; the code is
// matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

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

// Per-actor ride state (VAlloc'd, 0x1C bytes).
struct Beh {
    float timer;    // 0x00
    float f4;       // 0x04
    u8 state;       // 0x08
    char pad09[3];
    int iC;         // 0x0C
    int i10;        // 0x10
    float f14;      // 0x14
    u8 f18;         // 0x18
};

// The actor-type record hung off the actor at 0xC4.
struct ActorC4 {
    char pad00[0x30];
    int f30;        // 0x30
    char pad34[0x94];
    int fC8;        // 0xC8
};

// Position and id of a ride target.
struct Target {
    char pad00[0x24];
    int x;          // 0x24
    int y;          // 0x28
    int z;          // 0x2C
    char pad30[8];
    int id;         // 0x38
};

struct Actor {
    char pad00[0x2C];
    int i2C;                            // 0x2C
    char pad30[0x94];
    ActorC4* volatile c4;               // 0xC4
    char padC8[0x34];
    int (*fFC)(Actor*);                 // 0xFC
    char pad100[4];
    int i104;                           // 0x104
    char pad108[4];
    int (*f10C)(Actor*);                // 0x10C
    char pad110[0x20];
    Beh* beh;                           // 0x130
    char pad134[4];
    float dt;                           // 0x138
    char pad13C[0xE4];
    int state220;                       // 0x220
    char pad224[8];
    void* target;                       // 0x22C
    char pad230[0x40];
    char st[0x18];                      // 0x270
    int i288;                           // 0x288
    char pad28C[0x378];
    int (*f604)(Actor*);                // 0x604
    char pad608[0x28C];
    char unk894[0x70];                  // 0x894
    float f904;                         // 0x904
};

// One entry of the state dispatch table (0x10 bytes).
typedef int (*RideFunc)(void* actor);

struct RideType {
    RideFunc enter;     // 0x0
    RideFunc check;     // 0x4
    RideFunc update;    // 0x8
    const char* name;   // 0xC
};

// Message payloads.  Offsets are shared; each case uses its own view.
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

struct Msg41D {
    Actor* actor;       // 0x00
    int key;            // 0x04
    int unk08;          // 0x08
    int arg;            // 0x0C
};

struct Msg41E {
    Actor* actor;       // 0x00
    int key;            // 0x04
    int unk08;          // 0x08
    Buffer result;      // 0x0C
};

struct Msg41ETime {
    Actor* actor;       // 0x00
    int key;            // 0x04
    int unk08;          // 0x08
    float value;        // 0x0C
};

// C-linkage callees (still unnamed in symbols.txt).
extern "C" {
int rand(void);                                              // 0x802F2CFC
int SendMsg(int to, int msg, void* data);                    // 0x800265D4
int fn_80019F88(void* data, Actor* actor);                   // 0x80019F88
String fn_800225C8(const String& a, const String& b);        // 0x800225C8
void fn_800A7028(void* conditions);                          // 0x800A7028
void fn_800A78A0(void* conditions, void* target);            // 0x800A78A0
void fn_800A7A54(void* conditions, int which, int value);    // 0x800A7A54
void fn_800A7B08(void* conditions, int which, int value);    // 0x800A7B08
int fn_800A7B78(void* conditions, int which);                // 0x800A7B78
int fn_800A8CDC(Actor* actor, void* target, float f);        // 0x800A8CDC
void fn_800B1AB8(Actor* actor, int kind, float f, int a);    // 0x800B1AB8
int fn_800B1EF4(Actor* actor, int kind);                     // 0x800B1EF4
void fn_800B1F20(Actor* actor, int kind);                    // 0x800B1F20
void fn_800B2418(Actor* actor, int kind, float f);           // 0x800B2418
int fn_800B2454(Actor* actor, int kind);                     // 0x800B2454
void fn_800B261C(Actor* actor, int kind, void** out);        // 0x800B261C
int fn_800B26B8(Actor* actor);                               // 0x800B26B8
void fn_800BBCA8(Actor* actor);                              // 0x800BBCA8
int fn_800CE96C(Actor* actor);                               // 0x800CE96C
int fn_800CF064(Actor* actor);                               // 0x800CF064
int fn_800A70D0(Actor* actor);                               // 0x800A70D0
void fn_80162610(Actor* actor, void* p, int a);              // 0x80162610
void fn_80162658(Actor* actor, void* p);                     // 0x80162658
int fn_80162940(void* p, int a);                             // 0x80162940
void fn_8028C188(Buffer* b, char* data, int len);            // 0x8028C188
int fn_8029B070(Actor* actor, u32 key);                      // 0x8029B070
int fn_8029B140(Actor* actor, u32 key);                      // 0x8029B140
int fn_8029B3B0(Actor* actor, u32 key);                      // 0x8029B3B0
}

// Mangled C++ callees.
int OwnerId(Owner* o);                                       // 0x800A3A40
int HasCondition(void* conditions, int which);               // 0x800A7AC4
float OwnerValue(Owner* o, void* target);                    // 0x800AA050
int AssetFind(u32 table, u32 key);                           // 0x8007EB34
void* VAlloc(int size);                                      // 0x8009DA68
void VFree(void* p);                                         // 0x8009DA3C

// Cached asset handle and the ride-time constants (0x804D34E0..0x804D34F8).
int gHostRideDAsset = -1;
static int sRideTimeLo = 1000;
static int sRideTimeHi = 3000;
static int sRideThresh = 2800;
static int sRideAccum = 2000;

// Scratch word kept across the init call (0x804D5498).
u32 sRidePrevValue[2];

int HostRideDMsg(int msg, int* data);
int HostRideDTick(void* owner);
void HostRideDFree(Actor* actor);
void HostRideDInit(void* owner);
int HostRideDState0Enter(Actor* actor);
int HostRideDState0Check(Actor* actor);
int HostRideDState0Update(void* owner);
int HostRideDState1Enter(Actor* actor);
int HostRideDState1Check(Actor* actor);
int HostRideDState1Update(void* owner);
int HostRideDState2Enter(void* owner);
int HostRideDState2Check(Actor* actor);
int HostRideDState2Update(void* owner);
int HostRideDState3Enter(void* owner);
int HostRideDState3Check(Actor* actor);
int HostRideDState3Update(void* owner);
int HostRideDState4Enter(Actor* actor);
int HostRideDState4Check(Actor* actor);
int HostRideDState4Update(void* owner);

RideType gHostRideDTypes[5] = {
    { (RideFunc)HostRideDState0Enter, (RideFunc)HostRideDState0Check, (RideFunc)HostRideDState0Update, "\n" },
    { (RideFunc)HostRideDState1Enter, (RideFunc)HostRideDState1Check, (RideFunc)HostRideDState1Update, "\n" },
    { (RideFunc)HostRideDState2Enter, (RideFunc)HostRideDState2Check, (RideFunc)HostRideDState2Update, "\n" },
    { (RideFunc)HostRideDState3Enter, (RideFunc)HostRideDState3Check, (RideFunc)HostRideDState3Update, "\n" },
    { (RideFunc)HostRideDState4Enter, (RideFunc)HostRideDState4Check, (RideFunc)HostRideDState4Update, "\n" },
};

// 0x8023E7E0: message handler.
int HostRideDMsg(int msg, int* data) {
    Beh* b;
    Actor* a = (Actor*)data[0];
    int key;

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
    case 0x41D: {
        Msg41D* m = (Msg41D*)data;

        if (fn_80019F88(data, a) != 0) {
            return 1;
        }
        key = data[1];
        b = a->beh;
        switch (key) {
        case 0x54170CB7:
            b->i10 = data[3];
            return 1;
        case 0x3C8D5C5D:
            b->iC = data[3];
            return 1;
        case 0x7F0191AA:
            if (data[3] == 0) {
                if (b->state == 0 && gHostRideDTypes[b->state].check(a)) {
                    b->timer = 0.0f;
                    gHostRideDTypes[b->state + 1].enter(a);
                }
            } else {
                fn_800A7028(a->st);
                a->state220 = 0;
                if (b->state != 0 && gHostRideDTypes[b->state].check(a)) {
                    b->timer = 0.0f;
                    gHostRideDTypes[1].enter(a);
                }
            }
            return 1;
        }
        return 0;
    }
    case 0x41E: {
        Msg41E* m = (Msg41E*)data;

        b = a->beh;
        switch (m->key) {
        case 0x52AB9E79:
            ((Msg41ETime*)data)->value = (float)a->i2C - 500.0f;
            return 1;
        case 0xAD8576FA: {
            String name(gHostRideDTypes[b->state].name);
            String newline("\n");
            const String& text = fn_800225C8(name, newline);

            fn_8028C188(&m->result, text.mData, text.mLength);
            return 0;
        }
        }
        return 0;
    }
    case 0xC14:
        if (data[3] != 0x37BD5BDD) {
            return 0;
        }
        fn_8029B070(a, 0xC1F6F975);
        return 1;
    case 3:
    case 4:
        return 0xFFF0BDC1;
    }
    return 0;
}

// 0x8023EB70: per-frame tick.
int HostRideDTick(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b = a->beh;
    Msg400 m;
    int next;

    fn_8029B140(a, 0xC1F6F975);
    if (HasCondition(a->st, 0xA) || HasCondition(a->st, 0xB)) {
        fn_80162658(a, a->unk894);
        return 1;
    }
    if (a->state220 == 2) {
        if (SendMsg(((Target*)a->target)->id, 0x400, &m) == 0 || m.state == 0) {
            a->state220 = 0;
            fn_800A7028(a->st);
        }
    }
    b->timer += a->dt;
    next = gHostRideDTypes[b->state].update(a);
    if (next != b->state && gHostRideDTypes[b->state].check(a)) {
        if (gHostRideDTypes[next].enter(a) != 0) {
            b->timer = 0.0f;
        } else {
            gHostRideDTypes[b->state].enter(a);
        }
        fn_800A7A54(a->st, 0x40, 0);
        fn_800A7A54(a->st, 0x41, 0);
        fn_800A7A54(a->st, 0x43, 0);
        fn_800A7A54(a->st, 0x42, 0);
    }
    fn_80162658(a, a->unk894);
    return 1;
}

// 0x8023ED44: destroy callback.
void HostRideDFree(Actor* a) {
    VFree(a->beh);
    a->i104 = 0;
    a->beh = 0;
}

// 0x8023ED80: init callback.
void HostRideDInit(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b;

    b = (Beh*)VAlloc(0x1C);
    if (b == 0) {
        a->beh = 0;
        return;
    }
    sRidePrevValue[0] = a->c4->f30;
    a->c4->f30 = sRidePrevValue[0];
    a->f904 = (float)a->c4->f30;
    a->beh = b;
    a->fFC = fn_800CE96C;
    a->f604 = fn_800CF064;
    b->timer = 0.0f;
    b->state = 3;
    b->f18 = 0;
    b->iC = 6000;
    b->i10 = 0x1D4C;
    b->f14 = 0.0f;
    gHostRideDAsset = AssetFind(0xF1754BF2, 0x21C5E0);
    gHostRideDTypes[b->state].enter(a);
}

// 0x8023EE8C: state 4 update.
int HostRideDState4Update(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b = a->beh;
    int kind = fn_800B26B8(a);
    void* target;

    if (a->state220 == 2) {
        if (OwnerValue((Owner*)a, a->target) <= (float)a->c4->fC8 &&
            fn_800A8CDC(a, a->target, 1.0f) != 0) {
            fn_800A78A0(a->st, a->target);
            return 2;
        }
    } else if (kind != 4) {
        fn_800B261C(a, kind, &target);
        if (fn_800A7B78(a->st, 5) == 0) {
            a->state220 = 0;
            return 1;
        }
    }
    if (a->state220 == 0 && fn_800A7B78(a->st, 5) == 0) {
        return 1;
    }
    return b->state;
}

// 0x8023EFAC: state 4 check.
int HostRideDState4Check(Actor* a) {
    if (a == 0) {
        return 0;
    }
    a->f10C = fn_800A70D0;
    if (a->state220 == 4) {
        a->state220 = 0;
    }
    if (HasCondition(a->st, 0xD) != 0) {
        fn_800A7A54(a->st, 0xD, 0);
    }
    return 1;
}

// 0x8023F028: state 4 enter.
int HostRideDState4Enter(Actor* a) {
    if (a == 0) {
        return 0;
    }
    a->beh->state = 4;
    return 1;
}

// 0x8023F04C: state 3 check.
int HostRideDState3Check(Actor* a) {
    fn_800A7A54(a->st, 0x13, 0);
    fn_800A7B08(a->st, 4, 0);
    return 1;
}

// 0x8023F098: state 3 update.
int HostRideDState3Update(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b = a->beh;
    int kind = fn_800B26B8(a);
    void* target;
    float v;

    if (kind < 4) {
        if (fn_800B1EF4(a, kind) == 0) {
            fn_800B1F20(a, kind);
            fn_800B1AB8(a, kind, -1.0f, 1);
        }
        if (fn_800B2454(a, kind) == 0) {
            fn_800B2418(a, kind, 8000.0f);
        }
        fn_800B261C(a, kind, &target);
        fn_800A78A0(a->st, target);
    } else {
        fn_800A7028(a->st);
        target = 0;
    }
    v = OwnerValue((Owner*)a, target);
    if (target == 0) {
        return 0;
    }
    if (v > (float)b->i10) {
        b->f14 += a->dt;
        if (b->f14 > (float)sRideAccum) {
            return 0;
        }
    } else {
        b->f14 = 0.0f;
        if (v < (float)sRideThresh) {
            return 2;
        }
    }
    if (b->timer > b->f4 && target != 0 && fn_800A8CDC(a, target, 1.0f) != 0) {
        return 2;
    }
    return b->state;
}

// 0x8023F258: state 3 enter.
int HostRideDState3Enter(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b = a->beh;
    int kind;

    b->state = 3;
    fn_800BBCA8(a);
    a->i288 = 0;
    fn_800A7A54(a->st, 0x13, 1);
    fn_800A7B08(a->st, 4, 1);
    b->f4 = (float)(sRideTimeHi - sRideTimeLo) * ((float)rand() / 32767.0f) + (float)sRideTimeLo;
    kind = fn_800B26B8(a);
    if (kind != 4) {
        void* target;

        if (fn_800B1EF4(a, kind) == 0) {
            fn_800B1F20(a, kind);
            fn_800B1AB8(a, kind, -1.0f, 1);
        }
        if (fn_800B2454(a, kind) == 0) {
            fn_800B2418(a, kind, 8000.0f);
        }
        fn_800B261C(a, kind, &target);
        fn_800A78A0(a->st, target);
    }
    return 1;
}

// 0x8023F3B0: state 2 check.
int HostRideDState2Check(Actor* a) {
    fn_80162610(a, a->unk894, 0);
    fn_800A7028(a->st);
    fn_800A7B08(a->st, 4, 0);
    return 1;
}

// 0x8023F400: state 2 update.
int HostRideDState2Update(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b = a->beh;
    int kind = fn_800B26B8(a);
    void* target;
    float v;

    if (kind >= 4) {
        fn_800A7028(a->st);
        return 3;
    }
    if (fn_800B1EF4(a, kind) == 0) {
        fn_800B1F20(a, kind);
        fn_800B1AB8(a, kind, -1.0f, 1);
    }
    if (fn_800B2454(a, kind) == 0) {
        fn_800B2418(a, kind, 8000.0f);
    }
    fn_800B261C(a, kind, &target);
    if (target == 0) {
        fn_800A7028(a->st);
        return 3;
    }
    fn_800A78A0(a->st, target);
    v = OwnerValue((Owner*)a, target);
    if (v <= (float)a->c4->fC8 && v <= (float)b->i10 && fn_800A8CDC(a, target, 0.8f) != 0) {
        b->f4 = 0.0f;
    } else {
        b->f4 += a->dt;
        if (b->f4 > 5000.0f || b->i10 == 0) {
            return 3;
        }
    }
    if (fn_800A7B78(a->st, 3) == 0) {
        return 3;
    }
    if (fn_80162940(a->unk894, 1) != 0 && fn_80162940(a->unk894, 4) != 0) {
        v = OwnerValue((Owner*)a, target);
        if (v >= (float)sRideThresh) {
            fn_80162610(a, a->unk894, 1);
        } else {
            fn_80162610(a, a->unk894, 2);
        }
    }
    return b->state;
}

// 0x8023F638: state 2 enter.
int HostRideDState2Enter(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b = ((Actor*)owner)->beh;
    void* target;
    int kind;

    b->state = 2;
    b->f4 = 0.0f;
    fn_800BBCA8((Actor*)owner);
    a->i288 = 0;
    fn_800A7B08(a->st, 3, 1);
    fn_800A7B08(a->st, 4, 1);
    kind = fn_800B26B8((Actor*)owner);
    if (kind != 4) {
        if (fn_800B1EF4(a, kind) == 0) {
            fn_800B1F20(a, kind);
            fn_800B1AB8(a, kind, -1.0f, 1);
        }
        if (fn_800B2454(a, kind) == 0) {
            fn_800B2418(a, kind, 8000.0f);
        }
        fn_800B261C(a, kind, &target);
        if (target != 0) {
            fn_800A78A0(a->st, target);
        } else {
            fn_800A7028(a->st);
            return 0;
        }
    } else {
        fn_800A7028(a->st);
        return 0;
    }
    return 1;
}

// 0x8023F764: state 1 check.
int HostRideDState1Check(Actor* a) {
    fn_800A7B08(a->st, 4, 0);
    return 1;
}

// 0x8023F794: state 1 update.
int HostRideDState1Update(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b = a->beh;
    int kind = fn_800B26B8(a);
    void* target;

    if (kind < 4) {
        if (fn_800B1EF4(a, kind) == 0) {
            fn_800B1F20(a, kind);
            fn_800B1AB8(a, kind, -1.0f, 1);
        }
        if (fn_800B2454(a, kind) == 0) {
            fn_800B2418(a, kind, 8000.0f);
        }
        fn_800B261C(a, kind, &target);
        fn_800A78A0(a->st, target);
        if (target != 0) {
            return fn_800A8CDC(a, target, 1.0f) ? 2 : 3;
        }
    } else {
        fn_800A7028(a->st);
    }
    if (a->state220 != 0) {
        return 4;
    }
    return b->state;
}

// 0x8023F8A0: state 1 enter.
int HostRideDState1Enter(Actor* a) {
    a->beh->state = 1;
    fn_800A7B08(a->st, 4, 1);
    return 1;
}

// 0x8023F8DC: state 0 check.
int HostRideDState0Check(Actor* a) {
    fn_800A7B08(a->st, 4, 0);
    fn_800A7A54(a->st, 6, 0);
    return 1;
}

// 0x8023F928: state 0 update.
int HostRideDState0Update(void* owner) {
    Actor* a = (Actor*)owner;
    Beh* b = a->beh;
    int kind = fn_800B26B8(a);
    void* target;
    float v;

    if (kind < 4) {
        fn_800B261C(a, kind, &target);
        if (target != 0) {
            v = OwnerValue((Owner*)a, target);
            if (v < (float)b->iC && fn_8029B3B0(a, 0xC1F6F975) != 0) {
                fn_800B1F20(a, kind);
                return 1;
            }
        }
    }
    if (a->state220 != 0) {
        return 4;
    }
    return b->state;
}

// 0x8023FA04: state 0 enter.
int HostRideDState0Enter(Actor* a) {
    a->beh->state = 0;
    fn_800A7A54(a->st, 6, 1);
    fn_800A7B08(a->st, 4, 1);
    return 1;
}

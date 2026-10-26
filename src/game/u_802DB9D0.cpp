// u_802DB9D0.cpp: the host "behaviour" component (0x802DB9D0-0x802DC82C).
// A four-state machine attached to a possessable host: state 0 looks up the host's current
// interaction, state 1 sends a 0x400 query to the host's target and counts down, state 2 waits
// for condition 0x31, and state 3 runs a timed condition-0x2E action.  The component also answers
// the 0x41E pose query (returning a tier-dependent delay) and the model/create message 0x6.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef signed short s16;

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
struct HostBehState;

// Condition block at actor+0x270.
struct Conditions {
    char pad[0x18];
};

// The actor object returned by fn_800A7A18 (the current interaction target).
struct TargetInfo {
    char pad0[0x38];
    int handle;     // 0x38
};

// Object at actor+0xC4; +0x30 is a count used to pick the difficulty tier.
struct Counter {
    char pad0[0x30];
    int count;      // 0x30
};

// Per-actor behaviour state (VAlloc'd, 0xC bytes).
struct HostBehState {
    int state;      // 0x00
    float f04;      // 0x04
    float f08;      // 0x08
};

// The possessable host actor. Only the fields this file touches are named.
struct Actor {
    char pad000[0x4C];
    u32 flags4C;            // 0x4C
    char pad050[0x74];
    Counter* counter;       // 0xC4
    char pad0C8[0x34];
    int cbFC;               // 0xFC
    char pad100[4];
    int cb104;              // 0x104
    char pad108[0x28];
    HostBehState* beh;      // 0x130
    char pad134[4];
    float f138;             // 0x138
    char pad13C[0x12C];
    int i268;               // 0x268
    int i26C;               // 0x26C
    Conditions st;          // 0x270
    int i288;               // 0x288
    char pad28C[0x38];
    int i2C4;               // 0x2C4
    char pad2C8[0x33C];
    int cb604;              // 0x604
    char pad608[0x28C];
    char unk894[0x70];      // 0x894
    float f904;             // 0x904
    char pad908[0x14];
    float f91C;             // 0x91C
};

// Message 0x41E: cmd at 0x4, the answer is written at 0xC (an int or a Buffer).
struct Msg41E {
    Actor* actor;   // 0x00
    int cmd;        // 0x04
    int unk08;      // 0x08
    Buffer result;  // 0x0C
};

// Mangled C++ callees.
void* VAlloc(int size);                             // 0x8009DA68
void VFree(void* p);                                // 0x8009DA3C
int AssetFind(u32 table, u32 key);                  // 0x8007EB34
void TextureLoad(s16* ids, int count);              // 0x8000F738

int HasCondition(void* conditions, int which);      // 0x800A7AC4

extern "C" {
int rand(void);
int SendMsg(int to, int msg, void* data);           // 0x800265D4
void fn_8000F550(s16* ids, int count);              // 0x8000F550
int fn_801625C8(Actor* actor, void* cond, int msg, void* data);  // 0x801625C8
void fn_80162610(Actor* actor, void* cond, int value);           // 0x80162610
void fn_80162658(Actor* actor, void* cond);                      // 0x80162658
int fn_80162940(void* cond, int which);                          // 0x80162940
void fn_800A7A54(void* conditions, int which, int value);        // 0x800A7A54
void fn_800A7B08(void* conditions, int which, int value);        // 0x800A7B08
void* fn_800A7A18(void* conditions);                             // 0x800A7A18
void fn_800A78A0(void* conditions, void* target);                // 0x800A78A0
int fn_800BAF40(Actor* actor);                                   // 0x800BAF40
void fn_800BBCA8(Actor* actor);                                  // 0x800BBCA8
int fn_800B26B8(Actor* actor);                                   // 0x800B26B8
void fn_800B261C(Actor* actor, int state, void** out);           // 0x800B261C
int fn_800BEF9C(int obj);                                        // 0x800BEF9C
void fn_800CE328(void);                                          // 0x800CE328
void fn_800B2D00(void);                                          // 0x800B2D00
void fn_8028C188(Buffer* buffer, const char* data, int length);  // 0x8028C188
String fn_800225C8(const String& a, const String& b);            // 0x800225C8
}
void EmitterDeferStart(int sound);

// Owned by the next unit's .sdata.
extern int lbl_804D3D84;
extern int lbl_804D3D88;

// Per-file tuning constants (.sdata).
int lbl_804D3D30 = 20000;
int lbl_804D3D34 = 800;
int lbl_804D3D38 = 7000;
int lbl_804D3D3C = 8000;
int lbl_804D3D40 = 1100;
int lbl_804D3D44 = 4000;
int lbl_804D3D48 = 60000;
float lbl_804D3D4C = 0.67f;
int lbl_804D3D50 = 6000;
int lbl_804D3D54 = 7000;
int lbl_804D3D58 = 800;
int lbl_804D3D5C = 3000;
int lbl_804D3D60 = 70000;
float lbl_804D3D64 = 0.34f;
int lbl_804D3D68 = 3500;
int lbl_804D3D6C = 4200;
int lbl_804D3D70 = 500;
int lbl_804D3D74 = 2000;
int lbl_804D3D78 = 80000;

// Uniform random value in [lo, hi] (lo/hi are integer tuning constants).
inline float HostBehLerp(int lo, int hi, float t) {
    float d = hi - lo;
    return lo + d * t;
}

typedef int (*HostBehFunc)(Actor*);

// One entry of the host behaviour dispatch table (0x10 bytes).
struct HostBehType {
    HostBehFunc enter;      // 0x0
    HostBehFunc check;      // 0x4
    HostBehFunc update;     // 0x8
    const char* name;       // 0xC
};

int HostBehMsg(int msg, void* data);
int HostBehUpdate(void* p);
void HostBehFree(Actor* actor);
void HostBehInit(Actor* actor);
int HostBehState3Update(Actor* actor);
int HostBehState3Check(Actor* actor);
int HostBehState3Enter(Actor* actor);
int HostBehState2Update(Actor* actor);
int HostBehState2Check(Actor* actor);
int HostBehState2Enter(Actor* actor);
int HostBehState1Update(Actor* actor);
int HostBehState1Check(Actor* actor);
int HostBehState1Enter(Actor* actor);
int HostBehState0Update(void* p);
int HostBehState0Check(Actor* actor);
int HostBehState0Enter(Actor* actor);

HostBehType lbl_803C57B0[] = {
    { HostBehState0Enter, HostBehState0Check, (HostBehFunc)HostBehState0Update, "" },
    { HostBehState1Enter, HostBehState1Check, HostBehState1Update, "" },
    { HostBehState2Enter, HostBehState2Check, HostBehState2Update, "" },
    { HostBehState3Enter, HostBehState3Check, HostBehState3Update, "" },
};

static inline void HostBehStateName(Buffer* out, const char* s) {
    String name(s);
    String newline("\n");
    const String& text = fn_800225C8(name, newline);

    fn_8028C188(out, text.mData, text.mLength);
}

static inline int HostBehQuery(void* data) {
    Actor* actor = *(Actor**)data;
    Msg41E* m = (Msg41E*)data;
    HostBehState* beh = actor->beh;

    switch (m->cmd) {
    case 0xAD8576FA:
        HostBehStateName(&m->result, lbl_803C57B0[beh->state].name);
        return 0;
    case 0x607EED34: {
        if (actor->f904 > (float)actor->counter->count * lbl_804D3D4C) {
            *(int*)&m->result = lbl_804D3D48;
        } else if (actor->f904 > (float)actor->counter->count * lbl_804D3D64) {
            *(int*)&m->result = lbl_804D3D60;
        } else {
            *(int*)&m->result = lbl_804D3D78;
        }
        return 1;
    }
    }
    return 0;
}

// Message handler (registered in the component descriptor at 0x803AA...).
int HostBehMsg(int msg, void* data) {
    Actor* actor = *(Actor**)data;

    switch (msg) {
    case 0x41E:
        return HostBehQuery(data);
    case 6: {
        s16 ids[2];

        ids[0] = AssetFind(0xF2E2BEB8, 0x35E0C459);
        ids[1] = AssetFind(0xF2E2BEB8, 0x92373E14);
        if (((int*)data)[1] != 0) {
            TextureLoad(ids, 2);
        } else {
            fn_8000F550(ids, 2);
        }
        return 0;
    }
    default:
        return fn_801625C8(actor, actor->unk894, msg, data) != 0;
    }
}

// Per-frame behaviour update.
int HostBehUpdate(void* p) {
    int next;
    Actor* actor;
    HostBehState* beh;

    if (p == 0) {
        return 0;
    }
    actor = (Actor*)p;
    beh = actor->beh;
    if (HasCondition(&actor->st, 9) || HasCondition(&actor->st, 0xA) ||
        HasCondition(&actor->st, 0xB)) {
        fn_80162658(actor, actor->unk894);
        return 1;
    }
    if (actor->i268 > fn_800BEF9C(actor->i2C4)) {
        actor->i268 = fn_800BEF9C(actor->i2C4);
    }
    if (fn_800BAF40(actor) == -1) {
        actor->i288 = 0;
        actor->i26C = 0;
    }
    next = lbl_803C57B0[beh->state].update(actor);
    fn_80162658(actor, actor->unk894);
    if (next != beh->state && lbl_803C57B0[beh->state].check(actor)) {
        lbl_803C57B0[next].enter(actor);
        fn_800A7A54(&actor->st, 0x40, 0);
        fn_800A7A54(&actor->st, 0x41, 0);
        fn_800A7A54(&actor->st, 0x43, 0);
        fn_800A7A54(&actor->st, 0x42, 0);
    }
    return 1;
}

// Component teardown.
void HostBehFree(Actor* actor) {
    VFree(actor->beh);
    actor->beh = 0;
    actor->cb104 = 0;
}

// Component setup.
void HostBehInit(Actor* actor) {
    HostBehState* beh;

    actor->cb104 = (int)HostBehUpdate;
    beh = (HostBehState*)VAlloc(0xC);
    if (beh == 0) {
        actor->beh = 0;
    } else {
        actor->beh = beh;
        actor->cbFC = (int)fn_800CE328;
        actor->cb604 = (int)fn_800B2D00;
        actor->f91C = 1.0f;
        beh->state = 0;
        actor->flags4C |= 0x20000;
    }
}

// State 3 update: timed condition-0x2E action.
int HostBehState3Update(Actor* actor) {
    HostBehState* beh = actor->beh;

    if (!HasCondition(&actor->st, 0x2E) && !HasCondition(&actor->st, 0x2F)) {
        return 1;
    }
    beh->f08 -= actor->f138;
    if (beh->f08 <= 0.0f && fn_80162940(actor->unk894, 1) && fn_80162940(actor->unk894, 4)) {
        fn_80162610(actor, actor->unk894, 1);
        if (actor->f904 > (float)actor->counter->count * lbl_804D3D4C) {
            beh->f08 = HostBehLerp(lbl_804D3D40, lbl_804D3D44, (float)rand() / 32767.0f);
        } else if (actor->f904 > (float)actor->counter->count * lbl_804D3D64) {
            beh->f08 = HostBehLerp(lbl_804D3D58, lbl_804D3D5C, (float)rand() / 32767.0f);
        } else {
            beh->f08 = HostBehLerp(lbl_804D3D70, lbl_804D3D74, (float)rand() / 32767.0f);
        }
    }
    return beh->state;
}

// State 3 leave hook.
int HostBehState3Check(Actor* actor) {
    if (actor == 0) {
        return 0;
    }
    fn_800A7A54(&actor->st, 0x2E, 0);
    fn_800A7A54(&actor->st, 0x2F, 0);
    return 1;
}

// State 3 enter: arm the condition-0x2E timer.
int HostBehState3Enter(Actor* actor) {
    HostBehState* beh;

    if (actor == 0) {
        return 0;
    }
    beh = actor->beh;
    beh->state = 3;
    actor->i288 = 0;
    beh->f04 = (float)lbl_804D3D30;
    if (actor->f904 > (float)actor->counter->count * lbl_804D3D4C) {
        beh->f08 = HostBehLerp(lbl_804D3D40, lbl_804D3D44, (float)rand() / 32767.0f);
    } else if (actor->f904 > (float)actor->counter->count * lbl_804D3D64) {
        beh->f08 = HostBehLerp(lbl_804D3D58, lbl_804D3D5C, (float)rand() / 32767.0f);
    } else {
        beh->f08 = HostBehLerp(lbl_804D3D70, lbl_804D3D74, (float)rand() / 32767.0f);
    }
    beh->f08 += (float)lbl_804D3D34;
    fn_800A7A54(&actor->st, 0x2E, 1);
    fn_800BBCA8(actor);
    return 1;
}

// State 2 update: wait for the target's condition 0x31.
int HostBehState2Update(Actor* actor) {
    HostBehState* beh = actor->beh;
    void* info;
    char buf[0x20];

    info = fn_800A7A18(&actor->st);
    if (info == 0 || !SendMsg(((TargetInfo*)info)->handle, 0x400, buf)) {
        return 0;
    }
    return HasCondition(&actor->st, 0x31) == 0 ? 1 : beh->state;
}

// State 2 check hook.
int HostBehState2Check(Actor* actor) {
    return 1;
}

// State 2 enter.
int HostBehState2Enter(Actor* actor) {
    if (actor == 0) {
        return 0;
    }
    actor->beh->state = 2;
    actor->i288 = 0;
    fn_800A7A54(&actor->st, 0x31, 1);
    fn_800BBCA8(actor);
    return 1;
}

// State 1 update: query the target, then count down.
int HostBehState1Update(Actor* actor) {
    HostBehState* beh = actor->beh;
    void* info;
    char buf[0x20];

    info = fn_800A7A18(&actor->st);
    if (info == 0 || !SendMsg(((TargetInfo*)info)->handle, 0x400, buf)) {
        return 0;
    }
    beh->f04 -= actor->f138;
    if (beh->f04 <= 0.0f) {
        return 2;
    }
    if (HasCondition(&actor->st, 5)) {
        fn_800A7A54(&actor->st, 5, 0);
        return 3;
    }
    return beh->state;
}

// State 1 leave hook.
int HostBehState1Check(Actor* actor) {
    fn_80162610(actor, actor->unk894, 0);
    fn_800A7B08(&actor->st, 4, 0);
    return 1;
}

// State 1 enter: arm the countdown.
int HostBehState1Enter(Actor* actor) {
    HostBehState* beh;

    if (actor == 0) {
        return 0;
    }
    beh = actor->beh;
    beh->state = 1;
    actor->i288 = 0;
    if (actor->f904 > (float)actor->counter->count * lbl_804D3D4C) {
        beh->f04 = HostBehLerp(lbl_804D3D38, lbl_804D3D3C, (float)rand() / 32767.0f);
    } else if (actor->f904 > (float)actor->counter->count * lbl_804D3D64) {
        beh->f04 = HostBehLerp(lbl_804D3D50, lbl_804D3D54, (float)rand() / 32767.0f);
    } else {
        beh->f04 = HostBehLerp(lbl_804D3D68, lbl_804D3D6C, (float)rand() / 32767.0f);
    }
    fn_800A7B08(&actor->st, 4, 1);
    fn_800BBCA8(actor);
    return 1;
}

// State 0 update: pass the host's current interaction to the condition block.
int HostBehState0Update(void* p) {
    Actor* actor = (Actor*)p;
    HostBehState* beh = actor->beh;
    void* target;
    int kind;

    kind = fn_800B26B8(actor);
    if (kind != 4) {
        target = 0;
        fn_800B261C(actor, kind, &target);
        if (target != 0) {
            fn_800A78A0(&actor->st, target);
            return 1;
        }
    }
    return beh->state;
}

// State 0 check hook.
int HostBehState0Check(Actor* actor) {
    return 1;
}

// State 0 enter.
int HostBehState0Enter(Actor* actor) {
    if (actor == 0) {
        return 0;
    }
    actor->beh->state = 0;
    actor->i288 = 0;
    fn_800BBCA8(actor);
    return 1;
}

// Stop the two host sound loops started elsewhere (called when the host stops being active).
void HostBehStop(void) {
    if (lbl_804D3D88 > -1) {
        EmitterDeferStart(lbl_804D3D88);
    }
    if (lbl_804D3D84 > -1) {
        EmitterDeferStart(lbl_804D3D84);
    }
}

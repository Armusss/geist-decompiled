// u_802D77AC.cpp: the host ride / possession state machine (0x802D77AC-0x802D8C7C).
// The component a possessable host runs while a ghost is riding it: it owns a VAlloc'd
// 0x1C-byte state block (state index, target handle, three distance thresholds), a
// three-entry enter/check/update dispatch table, the message handler that answers the
// ghost's queries (0x400, 0x407, 0x409, 0x40A, 0x41D, 0x41E, 0xC04, 0xC0D, 0x1800,
// 0x7102, 0x7103 card-save exchange), and the per-frame update that tracks the ghost's
// distance and switches the state.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Actor;
struct Owner;

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

// Per-actor host state (VAlloc'd, 0x1C bytes).
struct Beh {
    int state;      // 0x00
    int unk4;       // 0x04
    int target;     // 0x08
    float f0C;      // 0x0C
    float f10;      // 0x10
    float f14;      // 0x14
    float f18;      // 0x18
};

// Condition block at actor+0x270 (0x18 bytes).
struct HostState {
    char pad[0x18];
};

// What the ghost is looking at / locked on to (actor+0x22C while riding), and the
// owner info returned by OwnerGetInfo: position at 0x24, handle at 0x38.
struct Target {
    char pad0[0x24];
    u32 x;          // 0x24
    u32 y;          // 0x28
    u32 z;          // 0x2C
    char pad30[8];
    int id;         // 0x38
};

struct Actor {
    char pad00[0x30];
    int (*msgFn)(int msg, void* data);  // 0x30
    char pad34[0x50];
    float f84;      // 0x84
    float f88;      // 0x88
    float f8C;      // 0x8C
    char pad90[0x3C];
    int iCC;        // 0xCC
    char padD0[4];
    int iD4;        // 0xD4
    char padD8[0x24];
    int cbFC;       // 0xFC
    char pad100[4];
    int i104;       // 0x104
    char pad108[4];
    int cb10C;      // 0x10C
    char pad110[0x20];
    Beh* beh;       // 0x130
    char pad134[0xE8];
    int i21C;       // 0x21C
    int i220;       // 0x220
    char pad224[8];
    Target* pTarget;    // 0x22C
    char pad230[0x38];
    int i268;       // 0x268
    int i26C;       // 0x26C
    HostState st;   // 0x270
    int i288;       // 0x288
    char pad28C[0x38];
    int i2C4;       // 0x2C4
    char pad2C8[0x334];
    int cb5FC;      // 0x5FC
    int cb600;      // 0x600
    int cb604;      // 0x604
    char pad608[0x28C];
    char unk894[0x70];  // 0x894
    float f904;     // 0x904
    char pad908[0x14];
    float f91C;     // 0x91C
    short s920;     // 0x920
    short s922;     // 0x922
};

// One entry of the host dispatch table (0x10 bytes).
typedef int (*HostRideFunc)(Actor* actor);

struct HostRideType {
    HostRideFunc enter;     // 0x0
    HostRideFunc check;     // 0x4
    HostRideFunc update;    // 0x8
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

struct MsgSave {
    Actor* actor;   // 0x00
    int size;       // 0x04
    char* buf;      // 0x08
    int pos;        // 0x0C
    int result;     // 0x10
};

struct Msg41D {
    Actor* actor;   // 0x00
    int cmd;        // 0x04
    int unk08;      // 0x08
    u32 arg;        // 0x0C
};

struct Msg41E {
    Actor* actor;   // 0x00
    int cmd;        // 0x04
    int unk08;      // 0x08
    Buffer result;  // 0x0C
};

struct Msg40A {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    u32 anim;       // 0x08
    int result;     // 0x0C
};

struct Msg409 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int anim;       // 0x08
    int arg;        // 0x0C
};

struct MsgHdr {
    int actor;      // 0x00
};

struct MsgC04 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int unk08;      // 0x08
    int unk0C;      // 0x0C
    u32 unk10;      // 0x10
    int unk14;      // 0x14
    int unk18;      // 0x18
};

// Mangled C++ callees.
int OwnerId(Owner* o);                              // 0x800A3A40
void* VAlloc(int size);                             // 0x8009DA68
void VFree(void* p);                                // 0x8009DA3C
int HasCondition(void* conditions, int which);      // 0x800A7AC4
int AssetFind(u32 bank, u32 name);                  // 0x8007EB34
Target* OwnerGetInfo(u32 handle);                   // 0x8002394C
void ActorAim(int* out, Actor* actor, int flag);    // 0x8009F700
int ActorPlayAnim(int owner, u32 anim, int arg);    // 0x800C5654

// Still-unnamed helpers.
extern "C" {
int fn_80019F88(void* data, Actor* actor);              // 0x80019F88
String fn_800225C8(const String& a, const String& b);   // 0x800225C8
int fn_80049940();                                      // 0x80049940
void fn_800A7028(void* conditions);                     // 0x800A7028
void fn_800A70D0(Actor* actor);                         // 0x800A70D0
void fn_800A78A0(void* conditions, void* target);       // 0x800A78A0
u32* fn_800A7A18(void* conditions);                     // 0x800A7A18
void fn_800A7A54(void* conditions, int which, int value);  // 0x800A7A54
void fn_800A7B08(void* conditions, int which, int value);  // 0x800A7B08
int fn_800A7B78(void* conditions, int which);           // 0x800A7B78
int fn_800A8CDC(Actor* actor, void* target, float f);   // 0x800A8CDC
void fn_8008F230(int obj, float f, int a, int b);       // 0x8008F230
void fn_8008F378(int obj);                              // 0x8008F378
void fn_8008F4E8(int obj, int a);                       // 0x8008F4E8
void fn_8008F4EC(int obj, int a);                       // 0x8008F4EC
void fn_8008F4F0(float f, int obj, int a);              // 0x8008F4F0
void fn_80090778(int obj, float f, int a, int b, int c, int d, int e);  // 0x80090778
int fn_80090EA4(int owner, int a);                      // 0x80090EA4
int fn_800BA84C(int a);                                 // 0x800BA84C
int fn_800BAF40(Actor* actor);                          // 0x800BAF40
void fn_800BBCA8(Actor* actor);                         // 0x800BBCA8
void fn_800BC56C(Actor* actor, void* target);           // 0x800BC56C
void fn_800B1AB8(Actor* actor, int state, float f, int a);  // 0x800B1AB8
void fn_800B1ED4(Actor* actor, int state);              // 0x800B1ED4
int fn_800B1EF4(Actor* actor, int state);               // 0x800B1EF4
void fn_800B1F20(Actor* actor, int state);              // 0x800B1F20
int fn_800B22F8(Actor* actor, int state);               // 0x800B22F8
void fn_800B2418(Actor* actor, int state, float f);     // 0x800B2418
int fn_800B2454(Actor* actor, int state);               // 0x800B2454
void fn_800B261C(Actor* actor, int state, void** out);  // 0x800B261C
int fn_800B26B8(Actor* actor);                          // 0x800B26B8
void fn_800B5028(Actor* actor);                         // 0x800B5028
void fn_800B6CAC(Actor* actor, void (*fn)(Actor*));     // 0x800B6CAC
int fn_800BEF9C(int a);                                 // 0x800BEF9C
int fn_800C725C(Actor* actor);                          // 0x800C725C
void fn_800C748C(Actor* actor);                         // 0x800C748C
void fn_800CC984(Actor* actor);                         // 0x800CC984
void fn_800CE96C(Actor* actor);                         // 0x800CE96C
void fn_800CF064(Actor* actor);                         // 0x800CF064
void fn_80162610(Actor* actor, void* p, int a);         // 0x80162610
void fn_80162658(Actor* actor, void* p);                // 0x80162658
void fn_8028C188(Buffer* b, char* data, int len);       // 0x8028C188
int SendMsg(int to, int msg, void* data);               // 0x800265D4
void* memcpy(void* dst, const void* src, unsigned long n);
}

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
    return h % 0xFFFFFFFF;
}

int HostRideMsg(int msg, void* data);
int HostRideTick(void* actor);
void HostRideFree(Actor* actor);
void HostRideInit(Actor* actor);
int HostRideState2Update(void* actor);
int HostRideState2Exit(Actor* actor);
int HostRideState2Enter(Actor* actor);
int HostRideState1Update(void* actor);
int HostRideState1Exit(Actor* actor);
int HostRideState1Enter(Actor* actor);
int HostRideState0Update(Actor* actor);
int HostRideState0Check(Actor* actor);
int HostRideState0Enter(void* actor);
int HostRideBehavior(void* actor);

// The asset found by AssetFind for the host query reply, cached on first use.
int gHostAsset = -1;

HostRideType gHostRideTypes[] = {
    { (HostRideFunc)HostRideState0Enter, HostRideState0Check, HostRideState0Update, "" },
    { HostRideState1Enter, HostRideState1Exit, (HostRideFunc)HostRideState1Update, "" },
    { HostRideState2Enter, HostRideState2Exit, (HostRideFunc)HostRideState2Update, "" },
};

int HostRideMsg(int msg, void* data) {
    Actor* actor = *(Actor**)data;
    Msg400 q;

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
        MsgSave* m = (MsgSave*)data;
        Beh* beh = actor->beh;

        if (beh == 0) {
            return 1;
        }
        if (m->size < 0x20) {
            m->result = 0;
            return 1;
        }
        u32 hash = StrHash("[CRDSAVE]", 0);
        memcpy(m->buf, &hash, 4);
        m->pos += 4;
        m->size -= 4;
        m->buf += 4;
        memcpy(m->buf, beh, 0x1C);
        m->pos += 0x1C;
        m->size -= 0x1C;
        m->buf += 0x1C;
        return 1;
    }
    case 0x7103: {
        MsgSave* m = (MsgSave*)data;
        Beh* beh = actor->beh;
        bool ok;

        if (beh == 0) {
            return 1;
        }
        if (m->pos < 0x20) {
            m->result = 0;
            return 1;
        }
        if (m->pos < 4) {
            ok = false;
        } else {
            u32* p = (u32*)m->buf;
            m->pos += 4;
            m->size -= 4;
            m->buf += 4;
            ok = (*p == StrHash("[CRDSAVE]", 0));
        }
        if (!ok) {
            return 1;
        }
        memcpy(beh, m->buf, 0x1C);
        m->pos -= 0x1C;
        m->buf += 0x1C;
        return 1;
    }
    case 0x41D: {
        Msg41D* m = (Msg41D*)data;
        Beh* beh = actor->beh;

        if (fn_80019F88(data, actor)) {
            return 1;
        }
        switch (m->cmd) {
        case 0x8757F68E:
            beh->target = m->arg;
            return 1;
        case 0xF1AA29A3:
            beh->f0C = (float)m->arg;
            return 1;
        case 0x28D1C00B:
            beh->f10 = (float)m->arg;
            return 1;
        case 0x178E2132:
            beh->f14 = (float)m->arg;
            return 1;
        case 0x63D91A9C:
            beh->f18 = (float)m->arg;
            return 1;
        }
        return 0;
    }
    case 0x41E: {
        Msg41E* m = (Msg41E*)data;
        Beh* beh = actor->beh;

        switch (m->cmd) {
        case 0x63F94A69: {
            u32* p = fn_800A7A18(&actor->st);
            m->result.mData = p ? (char*)p[0x38 / 4] : (char*)-1;
            return 1;
        }
        case 0x832EF50A:
            if (gHostAsset == -1) {
                gHostAsset = AssetFind(0xF1754BF2, 0x21C5E0);
            }
            m->result.mData = (char*)gHostAsset;
            return 1;
        case 0xAD8576FA: {
            String name(gHostRideTypes[beh->state].name);
            String newline("\n");
            const String& text = fn_800225C8(name, newline);

            fn_8028C188(&m->result, text.mData, text.mLength);
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
        int r;

        q.actor = actor;
        q.ownerId = OwnerId((Owner*)actor);
        actor->msgFn(0x400, &q);
        u32 hash = StrHash("QUERY", m->anim);
        r = ActorPlayAnim(OwnerId((Owner*)actor), hash, m->unk04);
        if (r != 0) {
            m->result = 0;
            return 1;
        }
        if (fn_80049940() == 0) {
            return 1;
        }
        if (m->anim == 0x7C44EC3D) {
            m->result = (int)0xA53F7E23;
            return 1;
        }
        return 0;
    }
    case 0x409: {
        Msg409* m = (Msg409*)data;
        int r = ActorPlayAnim(OwnerId((Owner*)actor), m->arg, m->anim);

        return r != 0;
    }
    case 0x407:
        return fn_800A7B78(&actor->st, 10) != 0;
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
            fn_8008F230(actor->iD4, (float)(u32)fn_800BA84C(10), 0, 1);
        } else {
            fn_8008F230(actor->iD4, (float)(u32)fn_800BA84C(10), 0, 1);
        }
        fn_800A7A54(&actor->st, 0xC, 0);
        fn_800A7A54(&actor->st, 0xD, 1);
        {
            u32 v = m->unk10 ? m->unk10 : 0x1000;

            fn_80090778(actor->iD4, (float)v / 4096.0f, 2, m->unk04, m->unk0C, m->unk14, m->unk18);
        }
        fn_8008F4F0(1.0f, actor->iD4, 2);
        return 1;
    }
    }
    return 0;
}

int HostRideTick(void* p) {
    Beh* beh;
    Actor* actor;
    Target* info;
    int aim[3];
    float f;
    int r;

    if (p == 0) {
        return 0;
    }
    actor = (Actor*)p;
    beh = actor->beh;
    if (HasCondition(&actor->st, 9) || HasCondition(&actor->st, 10) ||
        HasCondition(&actor->st, 0xB)) {
        fn_80162658(actor, actor->unk894);
        return 1;
    }
    if (actor->i268 > fn_800BEF9C(actor->i2C4)) {
        actor->i268 = fn_800BEF9C(actor->i2C4);
    }
    r = gHostRideTypes[beh->state].update(actor);
    if (fn_800BAF40(actor) == -1) {
        actor->i288 = 0;
        actor->i26C = 0;
    } else if (beh->target != -1 && (info = OwnerGetInfo(beh->target)) != 0) {
        ActorAim(aim, actor, 1);
        f = (float)(u32)aim[0] * ((float)info->x - actor->f84) +
            (float)(u32)aim[1] * ((float)info->y - actor->f88) +
            (float)(u32)aim[2] * ((float)info->z - actor->f8C);
        switch (actor->i21C) {
        case 1:
            if (beh->f10 * beh->f10 < f) {
                actor->i21C = 2;
            }
            break;
        case 2:
            if (beh->f0C * beh->f0C <= f) {
                if (beh->f14 * beh->f14 < f) {
                    actor->i21C = 3;
                }
            } else {
                actor->i21C = 1;
            }
            break;
        case 3:
            if (f < beh->f18 * beh->f18) {
                actor->i21C = 2;
            }
            break;
        }
    }
    fn_80162658(actor, actor->unk894);
    if (r != 0) {
        int next = HostRideBehavior(actor);

        if (next != beh->state && gHostRideTypes[beh->state].check(actor) &&
            gHostRideTypes[next].enter(actor) == 0) {
            gHostRideTypes[beh->state].enter(actor);
        }
    }
    return 1;
}

void HostRideFree(Actor* actor) {
    actor->i104 = 0;
    VFree(actor->beh);
    actor->beh = 0;
}

void HostRideInit(Actor* actor) {
    Beh* b;

    actor->i104 = (int)HostRideTick;
    b = (Beh*)VAlloc(0x1C);
    if (b == 0) {
        actor->beh = 0;
        return;
    }
    actor->beh = b;
    actor->cbFC = (int)fn_800CE96C;
    actor->cb604 = (int)fn_800CF064;
    actor->cb5FC = (int)fn_800CC984;
    actor->cb600 = (int)fn_800CC984;
    actor->f91C = 0.2f;
    actor->s920 = 5;
    b = (Beh*)actor->beh;
    b->state = 0;
    b->unk4 = 0;
    b->target = -1;
    b->f0C = 0.0f;
    b->f10 = 0.0f;
    b->f14 = 0.0f;
    b->f18 = 0.0f;
    fn_800B6CAC(actor, fn_800B5028);
    fn_800A7B08(&actor->st, 6, 1);
    gHostRideTypes[b->state].enter(actor);
    gHostAsset = AssetFind(0xF1754BF2, 0x21C5E0);
}

int HostRideState2Update(void* p) {
    Actor* actor = (Actor*)p;
    int kind;
    void* target;

    if (actor == 0) {
        return 0;
    }
    kind = fn_800B26B8(actor);
    if (kind != 4) {
        fn_800B261C(actor, kind, &target);
        if (target == 0 || fn_800B22F8(actor, kind) > 1) {
            fn_800A7028(&actor->st);
            return 1;
        }
        if (fn_800B1EF4(actor, kind) == 0) {
            fn_800B2418(actor, kind, 10000.0f);
            fn_800B1F20(actor, kind);
            fn_800B1AB8(actor, kind, -1.0f, 1);
            fn_800A78A0(&actor->st, target);
        } else {
            if (fn_800B2454(actor, kind) != 0) {
                return 1;
            }
            fn_800B2418(actor, kind, 10000.0f);
        }
    } else {
        fn_800A7028(&actor->st);
        return 1;
    }
    if (actor->s922 == actor->s920 - 1 && fn_800A8CDC(actor, target, 0.8f)) {
        fn_80162610(actor, actor->unk894, 1);
    }
    if (actor->i220 == 2) {
        actor->cb10C = (int)fn_800C748C;
        if ((fn_800A7A18(&actor->st) != (u32*)actor->pTarget || actor->i288 != actor->i21C) &&
            actor->i21C != 0) {
            actor->i288 = actor->i21C;
            fn_800BC56C(actor, actor->pTarget);
        }
        if (fn_800BAF40(actor) == -1 && actor->i21C != 0) {
            actor->i288 = 0;
        }
    }
    return 0;
}

int HostRideState2Exit(Actor* actor) {
    actor->cb10C = (int)fn_800A70D0;
    fn_800A7B08(&actor->st, 0x16, 0);
    fn_800A7B08(&actor->st, 3, 0);
    fn_800A7028(&actor->st);
    fn_80162610(actor, actor->unk894, 0);
    return 1;
}

int HostRideState2Enter(Actor* actor) {
    actor->beh->state = 2;
    fn_800A7B08(&actor->st, 0x16, 1);
    fn_800A7B08(&actor->st, 3, 1);
    return 1;
}

int HostRideState1Update(void* p) {
    Actor* actor = (Actor*)p;
    int kind;
    void* target;

    if (actor == 0) {
        return 0;
    }
    kind = fn_800B26B8(actor);
    if (kind != 4) {
        fn_800B261C(actor, kind, &target);
        if (fn_800B1EF4(actor, kind) == 0) {
            if (fn_800B22F8(actor, kind) <= 2) {
                return 1;
            }
            fn_800B1F20(actor, kind);
        }
    }
    if (actor->i220 == 0 && fn_800A7B78(&actor->st, 5) == 0) {
        return 1;
    }
    if (actor->i220 == 2) {
        actor->cb10C = (int)fn_800C748C;
        if (fn_800A7A18(&actor->st) != (u32*)actor->pTarget || actor->i288 != actor->i21C) {
            fn_800A78A0(&actor->st, actor->pTarget);
            if (actor->i21C != 0) {
                actor->i288 = actor->i21C;
                fn_800BC56C(actor, actor->pTarget);
            }
        }
        if (fn_800BAF40(actor) == -1 && actor->i21C != 0) {
            fn_800A78A0(&actor->st, 0);
            actor->i288 = 0;
        }
    }
    return 0;
}

int HostRideState1Exit(Actor* actor) {
    actor->cb10C = (int)fn_800A70D0;
    if (actor->i220 == 4) {
        actor->i220 = 0;
    }
    if (HasCondition(&actor->st, 0xD)) {
        fn_800A7A54(&actor->st, 0xD, 0);
    }
    return 1;
}

int HostRideState1Enter(Actor* actor) {
    actor->beh->state = 1;
    return 1;
}

int HostRideState0Update(Actor* actor) {
    int kind;

    if (actor == 0) {
        return 0;
    }
    if (actor->i220 != 0) {
        return 1;
    }
    kind = fn_800B26B8(actor);
    if (kind != 4) {
        if (fn_800B1EF4(actor, kind) == 0) {
            return 1;
        }
    } else {
        fn_800A7B08(&actor->st, 4, 0);
        fn_800A7028(&actor->st);
    }
    fn_800BBCA8(actor);
    return 0;
}

int HostRideState0Check(Actor* actor) {
    fn_800A7B08(&actor->st, 4, 0);
    return 1;
}

int HostRideState0Enter(void* p) {
    Actor* actor;
    Beh* beh;
    Msg400 q;
    void* target;
    int kind;

    if (p == 0) {
        return 0;
    }
    actor = (Actor*)p;
    beh = actor->beh;
    actor->i288 = 0;
    kind = fn_800B26B8(actor);
    if (kind != 4) {
        fn_800B261C(actor, kind, &target);
        if (target != 0 && SendMsg(((Target*)target)->id, 0x400, &q) && (q.flags & 2)) {
            fn_800A78A0(&actor->st, target);
        } else {
            fn_800A7028(&actor->st);
        }
    } else {
        fn_800A7028(&actor->st);
    }
    fn_800A7B08(&actor->st, 3, 0);
    beh->state = 0;
    return 1;
}

int HostRideBehavior(void* p) {
    Actor* actor;
    int kind;
    Beh* beh;
    int level;
    int current;
    void* target = 0;

    if (p == 0) {
        return 0;
    }
    actor = (Actor*)p;
    beh = actor->beh;
    current = beh->state;
    kind = fn_800B26B8(actor);
    if (kind != 4) {
        level = fn_800B22F8(actor, kind);
        fn_800B1ED4(actor, kind);
    }
    if (fn_800A7B78(&actor->st, 0x15) && actor->i220 != 0) {
        return 2;
    }
    if (kind != 4) {
        fn_800B261C(actor, kind, &target);
        fn_800B1ED4(actor, kind);
        if (target == 0) {
            return 0;
        }
        if (level <= 1) {
            return 2;
        }
        if (level == 3 && fn_800B1EF4(actor, kind) == 0) {
            fn_800B1F20(actor, kind);
            return (current != beh->state) ? beh->state : 0;
        }
        fn_800B1F20(actor, kind);
    }
    if (current != beh->state) {
        return beh->state;
    }
    return actor->i220 != 0;
}

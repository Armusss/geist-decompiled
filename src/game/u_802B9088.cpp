// u_802B9088.cpp: ghost ride / host action state machine (0x802B9088-0x802BA078).
// The component a ghost runs while riding (or hijacking) a possessable host: it owns a VAlloc'd
// 0x14-byte state block, a two-entry enter/check/update dispatch table, a message handler that
// answers the host queries (0x400, 0x407, 0x409, 0x40A, 0x41D, 0x41E), the per-frame update and
// the roll/heading solver that drives the ridden model.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Vec3 {
    float x, y, z;
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

// Per-actor ride state (VAlloc'd, 0x14 bytes).
struct Beh {
    float f0;       // 0x00
    int unk4;       // 0x04
    int unk8;       // 0x08
    float fC;       // 0x0C
    u8 idx;         // 0x10
    u8 unk11;       // 0x11
    u8 pad12[2];
};

// What the rider is looking at / locked on to (actor+0x22C while riding).
struct Target {
    char pad0[0x24];
    u32 x;          // 0x24
    u32 y;          // 0x28
    u32 z;          // 0x2C
    char pad30[8];
    u32 id;         // 0x38
};

// The host's per-type definition data (at actor+0xC4).
struct RideData {
    u32 hash;       // 0x00
    char pad4[0x2C];
    u32 value;      // 0x30
    char pad34[0xB0];
    float fE4;      // 0xE4
    char padE8[8];
    float fF0;      // 0xF0
    float fF4;      // 0xF4
    float fF8;      // 0xF8
};

// Roll/heading state (at actor+0x270).
struct RideState {
    Vec3 pos;       // 0x00
    Vec3 rot;       // 0x0C
    int i18;        // 0x18
    float f1C;      // 0x1C
    int i20;        // 0x20
    char pad24[0x14];
    float f38;      // 0x38
    float f3C;      // 0x3C
    float f40;      // 0x40
};

struct Actor {
    char pad00[0x30];
    int (*msgFn)(int msg, void* data);  // 0x30
    char pad34[0x50];
    float f84;      // 0x84
    float f88;      // 0x88
    float f8C;      // 0x8C
    char pad90[0x34];
    RideData* data; // 0xC4
    char padC8[0x34];
    int cbFC;       // 0xFC
    int i100;       // 0x100
    int i104;       // 0x104
    int i108;       // 0x108
    int cb10C;      // 0x10C
    char pad110[8];
    int i118;       // 0x118
    char pad11C[0x14];
    Beh* beh;       // 0x130
    char pad134[4];
    float dt;       // 0x138
    char pad13C[0xDC];
    int i218;       // 0x218
    int i21C;       // 0x21C
    int i220;       // 0x220
    int i224;       // 0x224
    int i228;       // 0x228
    union {
        Target* pTarget;    // 0x22C
        float f22C;
    };
    int i230;       // 0x230
    float f234;     // 0x234
    char pad238[8];
    Vec3 v240;      // 0x240
    Vec3 v24C;      // 0x24C
    float f258;     // 0x258
    char pad25C[0xC];
    int i268;       // 0x268
    char pad26C[4];
    RideState st;   // 0x270
    char pad2B4[0x348];
    int cb5FC;      // 0x5FC
    int cb600;      // 0x600
    int cb604;      // 0x604
    char pad608[0x28C];
    char unk894[0x70];  // 0x894
    float f904;     // 0x904
};

// One entry of the ride dispatch table (0x10 bytes).
struct RideType {
    int (*enter)(Actor* actor);     // 0x0
    int (*check)(Actor* actor);     // 0x4
    int (*update)(void* actor);     // 0x8
    const char* name;               // 0xC
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

struct Msg400Reply {
    int unk00;      // 0x00
    int unk04;      // 0x04
    int unk08;      // 0x08
    int state;      // 0x0C
};

struct Msg407 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int unk08;      // 0x08
    float f0C;      // 0x0C
    int unk10;      // 0x10
    int hash;       // 0x14
};

struct Msg409 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int anim;       // 0x08
    int arg;        // 0x0C
};

struct Msg40A {
    Actor* actor;   // 0x00
    int id;         // 0x04
    u32 anim;       // 0x08
    int result;     // 0x0C
};

struct Msg41D {
    Actor* actor;   // 0x00
    int id;         // 0x04
    int unk08;      // 0x08
    int arg;        // 0x0C
};

struct Msg41E {
    Actor* actor;   // 0x00
    int hash;       // 0x04
    int unk08;      // 0x08
    Buffer result;  // 0x0C
};

// Mangled C++ callees.
int OwnerId(void* owner);                        // 0x800A3A40
int ActorPlayAnim(int actor, u32 anim, int arg); // 0x800C5654
void* VAlloc(int size);                          // 0x8009DA68
void VFree(void* p);                             // 0x8009DA3C
void Vec3Set(Vec3* v, float x, float y, float z);// 0x8016D67C
void EulerNormalize(Vec3* in, Vec3* out);        // 0x8016D55C
int HasCondition(void* conditions, int which);   // 0x800A7AC4

extern "C" int SendMsg(int to, int msg, void* data);   // 0x800265D4

// Still-unnamed helpers.
u32* fn_800A7A18(void* conditions);              // 0x800A7A18
int fn_800A7A3C(void* conditions);               // 0x800A7A3C
void fn_800A7A54(void* conditions, int which, int value);  // 0x800A7A54
void fn_800A78A0(void* conditions);              // 0x800A78A0
void fn_800A7B08(void* conditions, int which, int value);  // 0x800A7B08
int fn_800A7B78(void* conditions, int which);    // 0x800A7B78
void fn_800A794C(void* conditions, Vec3* out);   // 0x800A794C
void fn_800A7028(void* conditions);              // 0x800A7028
int fn_800A70D0(Actor* actor);                   // 0x800A70D0
float fn_800AA9FC(Actor* actor, Vec3* v);        // 0x800AA9FC
void fn_800E1098(Vec3* out, Vec3* in, float scale);   // 0x800E1098
void fn_8016D440(Vec3* out, Vec3* in);           // 0x8016D440
void fn_80162658(Actor* actor, void* p);         // 0x80162658
int fn_80019F88(void* data, Actor* actor);       // 0x80019F88
int fn_800B26B8(Actor* actor);                   // 0x800B26B8
int fn_800B22F8(Actor* actor, int state);        // 0x800B22F8
void fn_800B261C(Actor* actor, int state, int* out);  // 0x800B261C
void fn_800B1F20(Actor* actor, int state);       // 0x800B1F20
int fn_800C725C(Actor* actor);                   // 0x800C725C
void fn_800CE96C();                              // 0x800CE96C
void fn_800CF064();                              // 0x800CF064
void fn_800CC984();                              // 0x800CC984

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

void fn_800225C8(String* out, const String& a, const String& b); // 0x800225C8
void fn_8028C188(Buffer* b, char* data, int len);      // 0x8028C188
void fn_8028BF80(String* s, const char* format, ...);  // 0x8028BF80

int RideMsg(int msg, void* data);
int RideUpdate(void* p);
void RideFree(Actor* actor);
void RideInit(void* p);
int RideTick(void* p);
int RideState0Enter(Actor* actor);
int RideState0Check(Actor* actor);
int RideState0Update(void* p);
int RideState1Enter(Actor* actor);
int RideState1Check(Actor* actor);
int RideState1Update(void* p);

RideType gRideTypes[] = {
    { RideState0Enter, RideState0Check, RideState0Update, "" },
    { RideState1Enter, RideState1Check, RideState1Update, "" },
};

// Picks the ride state whose conditions are active: 1 when the current action is the second
// variant, 0 otherwise.
static inline int RideBehavior(Actor* actor) {
    u32* p;
    int t;

    if (actor == 0 || actor->beh == 0) {
        return 0;
    }
    p = fn_800A7A18(&actor->st);
    if (p != 0) {
        t = p[0x38 / 4] >> 24;
        if (t == 0x13 || t == 2) {
            int s = fn_800A7A3C(&actor->st);
            if (s == 1) {
                return 0;
            } else if (s == 2) {
                return 1;
            }
        }
    }
    return 0;
}

int RideMsg(int msg, void* data) {
    Actor* actor = *(Actor**)data;
    Beh* beh;
    Msg400* m400;
    Msg407* m407;
    Msg409* m409;
    Msg40A* m40A;
    Msg41D* m41D;
    Msg41E* m41E;
    Msg400 q;
    u32 hash;
    int r;

    switch (msg) {
    case 0x400:
        m400 = (Msg400*)data;
        m400->ownerId = OwnerId(actor);
        m400->state = (actor->f904 > 0.0f) ? 1 : 0;
        m400->f10 = 1.0f;
        m400->f14 = 1.0f;
        m400->flags = 2;
        m400->unk1C = 0;
        return 1;
    case 0x41E: {
        m41E = (Msg41E*)data;
        beh = actor->beh;
        if (m41E->hash != 0xAD8576FA) {
            return 0;
        }
        String s;
        String a(gRideTypes[beh->idx].name);
        String b("\n");
        String c;

        fn_800225C8(&c, a, b);
        fn_8028C188(&m41E->result, c.mData, c.mLength);
        fn_8028BF80(&s, "Roll: %f\n", actor->v24C.y);
        fn_8028C188(&m41E->result, s.mData, s.mLength);
        return 0;
    }
    case 0x41D:
        m41D = (Msg41D*)data;
        if (fn_80019F88(data, actor)) {
            return 1;
        }
        beh = actor->beh;
        switch (m41D->id) {
        case 0x63F94A69:
            if (m41D->arg == -1) {
                fn_800A7028(&actor->st);
            }
            return 0;
        case 0xA996F6DE:
            beh->unk4 = m41D->arg;
            return 1;
        }
        return 0;
    case 0x407:
        m407 = (Msg407*)data;
        beh = actor->beh;
        if (m407->hash == 0x403548D0) {
            return 1;
        }
        if (m407->hash == 0x76764749) {
            beh->unk8 = 1;
            beh->fC = 0.0f;
            return 1;
        }
        if (m407->hash == 0xB9FC42F5) {
            if (beh->unk8 != 0) {
                m407->f0C = 0.036f * (float)actor->data->value;
                beh->unk8 = 0;
                beh->fC = 0.0f;
            } else {
                m407->f0C = 0.34f * (float)actor->data->value;
            }
        }
        return 0;
    case 0x40A:
        m40A = (Msg40A*)data;
        q.actor = actor;
        q.ownerId = OwnerId(actor);
        m40A->actor->msgFn(0x400, &q);
        beh = actor->beh;
        hash = StrHash("QUERY", m40A->anim);
        r = ActorPlayAnim(OwnerId(actor), hash, m40A->id);
        if (r != 0) {
            m40A->result = 0;
            return 1;
        }
        if (fn_800A7B78(&actor->st, 0xB) != 0) {
            return 1;
        }
        if (actor->f904 > 0.0f && HasCondition(&actor->st, 0xA) == 0 && (q.flags & 2) != 0 &&
            m40A->anim == 0x3CA34BD6) {
            m40A->result = 0x1379C466;
            return 1;
        }
        return 0;
    case 0x409:
        m409 = (Msg409*)data;
        r = ActorPlayAnim(OwnerId(actor), m409->arg, m409->anim);
        return r != 0;
    case 0x1800:
        return fn_800C725C(actor);
    }
    return 0;
}

int RideUpdate(void* p) {
    Actor* actor = (Actor*)p;
    Msg400 m;
    Beh* beh = actor->beh;
    int idx;

    if (actor->f904 <= 0.0f) {
        return 1;
    }
    if (actor->i220 == 2) {
        if (!SendMsg(actor->pTarget->id, 0x400, &m) || m.state == 0) {
            actor->i220 = 0;
            fn_800A7028(&actor->st);
        }
    }
    beh->f0 += actor->dt;
    if (beh->unk8 != 0) {
        beh->fC += actor->dt;
        if (beh->fC > 500.0f) {
            beh->unk8 = 0;
            beh->fC = 0.0f;
        }
    }
    if (gRideTypes[beh->idx].update(actor)) {
        idx = RideBehavior(actor);
        if (beh->idx != idx && gRideTypes[beh->idx].check(actor)) {
            beh->f0 = 0.0f;
            gRideTypes[idx].enter(actor);
            fn_800A7A54(&actor->st, 0x40, 0);
            fn_800A7A54(&actor->st, 0x41, 0);
            fn_800A7A54(&actor->st, 0x43, 0);
            fn_800A7A54(&actor->st, 0x42, 0);
        }
    }
    fn_80162658(actor, actor->unk894);
    return 1;
}

void RideFree(Actor* actor) {
    VFree(actor->beh);
    actor->i104 = 0;
    actor->beh = 0;
}

void RideInit(void* p) {
    Actor* actor = (Actor*)p;
    Beh* b = (Beh*)VAlloc(0x14);

    if (b == 0) {
        return;
    }
    actor->beh = b;
    actor->cbFC = (int)fn_800CE96C;
    actor->cb604 = (int)fn_800CF064;
    actor->cb5FC = (int)fn_800CC984;
    actor->cb600 = (int)fn_800CC984;
    actor->cb10C = (int)RideTick;
    actor->i118 = 0;
    actor->i108 = 0;
    b->f0 = 0.0f;
    b->idx = RideBehavior(actor);
    b->unk11 = 0;
    b->unk4 = 0;
    b->fC = 0.0f;
    b->unk8 = 0;
    gRideTypes[b->idx].enter(actor);
}

int RideState1Check(Actor* actor) {
    return 1;
}

int RideState1Update(void* p) {
    Actor* actor = (Actor*)p;
    Beh* beh = actor->beh;
    int state = fn_800B26B8(actor);
    int r;
    int cond;

    if (state != 4 && beh->unk4 == 0) {
        r = fn_800B22F8(actor, state);
        fn_800B261C(actor, state, &cond);
        if (fn_800A7A18(&actor->st) != (u32*)cond) {
            fn_800A78A0(&actor->st);
            fn_800A7B08(&actor->st, 4, 1);
        }
        fn_800B1F20(actor, state);
        if (r != 4) {
            actor->i268 = r;
            return 1;
        }
    }
    return 0;
}

int RideState1Enter(Actor* actor) {
    actor->beh->idx = 1;
    actor->st.i18 = 3;
    return 1;
}

int RideState0Check(Actor* actor) {
    return 1;
}

int RideState0Update(void* p) {
    Actor* actor = (Actor*)p;
    Beh* beh = actor->beh;
    int state = fn_800B26B8(actor);
    int r;
    int cond;

    if (state != 4 && beh->unk4 == 0) {
        r = fn_800B22F8(actor, state);
        fn_800B261C(actor, state, &cond);
        if (fn_800A7A18(&actor->st) != (u32*)cond) {
            fn_800A78A0(&actor->st);
            fn_800A7B08(&actor->st, 4, 1);
        }
        fn_800B1F20(actor, state);
        if (r != 4) {
            actor->i268 = r;
            return 1;
        }
    }
    if (actor->i220 != 0) {
        return 1;
    }
    return fn_800A7A3C(&actor->st) != 0;
}

int RideState0Enter(Actor* actor) {
    actor->beh->idx = 0;
    actor->st.i18 = 0;
    return 1;
}

int RideTick(void* p) {
    Actor* actor;
    RideState* st;
    Vec3 v;
    Vec3 rot;
    Vec3 va;
    Vec3 vb;
    float f31;
    float f30;
    float f29;
    float dot;
    float thr;
    float len2;
    int flag;
    int n;
    int state;
    Target* target;

    if (p == 0) {
        return 0;
    }
    actor = (Actor*)p;
    st = &actor->st;
    if (st->i20 != 0 && fn_800A7B78(st, 0x16) && actor->data->hash != 0x8FA05C1A) {
        Vec3 dir;
        fn_800A794C(st, &dir);
        st->f40 = fn_800AA9FC(actor, &dir);
        st->f40 = st->f40 < -60.0f ? -60.0f : (st->f40 > 60.0f ? 60.0f : st->f40);
    } else {
        st->f40 = 0.0f;
    }
    state = actor->i220;
    if (state == 2 || state == 1) {
        f31 = actor->f84;
        f30 = actor->f88;
        f29 = actor->f8C;
        flag = 0;
        if (actor->i21C != 0) {
            target = actor->pTarget;
            st->i18 = actor->i21C;
            v.x = (float)target->x;
            v.y = (float)target->y;
            v.z = (float)target->z;
            st->pos.x = v.x - f31;
            st->pos.y = v.y - f30;
            st->pos.z = v.z - f29;
            st->f3C = *(float*)((char*)actor->data + st->i18 * 4 + 0xF4);
            st->f38 = actor->data->fF0;
            st->f1C = *(float*)((char*)actor->data + st->i18 * 4 + 0xE4);
        } else {
            st->i18 = actor->i218;
            st->pos.x = 0.0f;
            st->pos.y = 0.0f;
            st->pos.z = 0.0f;
            st->f3C = actor->data->fF4;
            st->f38 = actor->data->fE4;
            st->f1C = actor->data->fE4;
            fn_800A7B08(st, 4, 1);
        }
        if (fn_800A7B78(st, 4)) {
            fn_800A794C(st, &v);
        }
        v.x -= f31;
        v.y -= f30;
        v.z -= f29;
        n = (fabs(v.x) > 1.1920929e-7f) + (fabs(v.y) > 1.1920929e-7f) + (fabs(v.z) > 1.1920929e-7f);
        if (n != 0) {
            fn_8016D440(&rot, &v);
            Vec3Set(&st->rot, rot.z - actor->v24C.z, rot.x - actor->v24C.x, 0.0f);
            EulerNormalize(&st->rot, &st->rot);
        } else {
            Vec3Set(&st->rot, 0.0f, 0.0f, 0.0f);
        }
        fn_800E1098(&va, &actor->v240, 1.0f);
        fn_800E1098(&vb, &st->pos, 1.0f);
        dot = va.x * vb.x + va.y * vb.y + va.z * vb.z;
        if (dot > 0.95f) {
            thr = (actor->v240.x * actor->v240.x + actor->v240.y * actor->v240.y +
                   actor->v240.z * actor->v240.z) /
                  (2.0f * actor->data->fF8);
            thr = thr * thr;
        } else {
            thr = 16384.0f;
        }
        if (dot < 0.0f) {
            flag = 1;
        }
        len2 = st->pos.x * st->pos.x + st->pos.y * st->pos.y + st->pos.z * st->pos.z;
        if ((actor->i21C == 0 && fabs(st->rot.z) < 1.1920929e-7f) || len2 < thr || flag != 0) {
            if (actor->i220 == 2 && actor->pTarget != 0 &&
                (actor->pTarget->id >> 24) == 3) {
                actor->i220 = 0;
                actor->pTarget = 0;
                st->i18 = 1;
                st->f3C = actor->data->fF8;
                ActorPlayAnim(actor->i224, actor->i228, OwnerId(actor));
            } else {
                actor->i220 = 0;
                ActorPlayAnim(actor->i224, actor->i228, OwnerId(actor));
            }
        }
        return 1;
    }
    if (state == 3) {
        Vec3 d;
        d.z = actor->f234 - actor->v24C.z;
        d.x = actor->f22C - actor->v24C.x;
        if (fabs(d.z) < 1.1920929e-7f && fabs(d.x) < 1.1920929e-7f) {
            actor->i220 = 0;
            ActorPlayAnim(actor->i224, actor->i228, OwnerId(actor));
        } else if (!fn_800A7B78(st, 0x16) || fabs(st->f40 - actor->f258) < 10.0f) {
            st->i18 = actor->i218;
            Vec3Set(&st->rot, d.z, d.x, 0.0f);
            EulerNormalize(&st->rot, &st->rot);
            st->f3C = actor->data->fF4;
            st->f38 = actor->data->fE4;
            st->f1C = actor->data->fE4;
        }
        return 1;
    }
    return fn_800A70D0(actor);
}

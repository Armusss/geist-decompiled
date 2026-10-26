// u_80251CB8.cpp: the imp actor behaviour (0x80251CB8-0x80252F2C: message handler, tick and a three-state
// idle/attack/chase machine), the imp's attack sub-behaviour (0x80252F2C-0x80253614) and the "IMP FIRE"
// effect type (0x80253614-0x802540F4), plus the fire ball asset helpers (0x802540F4-0x802541D4).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHashSeed(const char* s, u32 h) {
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
    Buffer();
    virtual ~Buffer();
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();
    String(const char* s);
    ~String() {}
    virtual void Release(char* p);
};

struct Owner;
struct Actor;

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

// Six-word snapshot of an actor's collision source.
struct ZoneHitBox {
    u32 data[6];
};

// Position and handle of an owner.
struct Target {
    u8 pad[0x24];
    int x, y, z;
    u8 pad30[8];
    int id;  // 0x38
};

// The imp's per-actor state (VAlloc'd, 0x14 bytes).
struct ImpBeh {
    float timer;   // 0x0
    float near;    // 0x4: time spent close to the target
    float chase;   // 0x8
    float hurt;    // 0xC: cool-down of the hurt effect
    u8 state;      // 0x10
    u8 flags;      // 0x11
    s8 seed;       // 0x12
};

struct Actor {
    u8 pad00[0x24];
    int x, y, z;                        // 0x24
    int (*msgFn)(int msg, void* data);  // 0x30
    u8 pad34[4];
    int id;                             // 0x38
    u8 pad3C[4];
    void* model;                        // 0x40
    u8 pad44[0x84 - 0x44];
    Vec3 pos;                           // 0x84
    u8 pad90[0xD4 - 0x90];
    int unkD4;                          // 0xD4
    u8 padD8[0xFC - 0xD8];
    void* unkFC;                        // 0xFC
    u8 pad100[4];
    int unk104;                         // 0x104
    u8 pad108[4];
    void* unk10C;                       // 0x10C
    u8 pad110[0x130 - 0x110];
    ImpBeh* beh;                        // 0x130
    u8 pad134[4];
    float dt;                           // 0x138
    u8 pad13C[0x214 - 0x13C];
    int unk214;                         // 0x214
    u8 pad218[4];
    int unk21C;                         // 0x21C
    int unk220;                         // 0x220
    u8 pad224[0x22C - 0x224];
    Target* unk22C;                     // 0x22C
    u8 pad230[0x270 - 0x230];
    u8 st[0x18];                        // 0x270: condition block
    int unk288;                         // 0x288
    u8 pad28C[0x2C4 - 0x28C];
    int unk2C4;                         // 0x2C4
    u8 pad2C8[0x5FC - 0x2C8];
    void* unk5FC;                       // 0x5FC
    void* unk600;                       // 0x600
    void* unk604;                       // 0x604
    u8 pad608[0x894 - 0x608];
    u8 unk894[0x1E];                    // 0x894
    u16 nearMin;                        // 0x8B2
    u16 nearMax;                        // 0x8B4
    u8 pad8B6[0x8EC - 0x8B6];
    int unk8EC;                         // 0x8EC
    int unk8F0;                         // 0x8F0
    u8 pad8F4[0x900 - 0x8F4];
    float unk900;                       // 0x900
    float health;                       // 0x904
};

typedef int (*ImpStateFunc)(void* actor);

// One entry of the imp's state table (0x803BECD8, 0x10 bytes).
struct ImpStateType {
    ImpStateFunc enter;   // 0x0
    ImpStateFunc exit;    // 0x4
    ImpStateFunc update;  // 0x8
    const char* name;     // 0xC
};

extern ImpStateType lbl_803BECD8[3];

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
    float damage;   // 0x0C
    int unk10;      // 0x10
    u32 type;       // 0x14
    u8 pad18[0x44 - 0x18];
    int unk44;      // 0x44
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

int OwnerId(Owner* o);                              // 0x800A3A40
void* VAlloc(int size);                             // 0x8009DA68
void VFree(void* p);                                // 0x8009DA3C
int HasCondition(void* conditions, int which);      // 0x800A7AC4
float OwnerValue(Owner* o, void* value);            // 0x800AA050
int ActorPlayAnim(int owner, u32 anim, int arg);    // 0x800C5654
void* FxFind(const char* name);
void ActorGetMatrix(Mtx34* m, Actor* a);
int ModelBoneHitBox(int a, int b, int c, int d);

extern "C" {
int rand(void);
int SendMsg(int to, int msg, void* data);
int fn_80019F88(void* data, Actor* actor);
String fn_800225C8(const String& a, const String& b);
void fn_80090390(int a, int b);
void fn_800A7028(void* conditions);
void fn_800A78A0(void* conditions, void* target);
Target* fn_800A7A18(void* conditions);
void fn_800A7A54(void* conditions, int which, int value);
void fn_800A7B08(void* conditions, int which, int value);
int fn_800A7B78(void* conditions, int which);
int fn_800B1EF4(Actor* actor, int kind);
void fn_800B1F20(Actor* actor, int kind);
void fn_800B20B8(Actor* actor, int a);
void fn_800B2418(Actor* actor, int kind, float f);
int fn_800B2454(Actor* actor, int kind);
void fn_800B261C(Actor* actor, int kind, void* out);
int fn_800B26B8(Actor* actor);
int fn_800BAF40(Actor* actor);
void fn_800BAFF8(Actor* actor, Vec3* dir, int a, int b, Vec3* to, float f1, float f2);
int fn_800BB218(Actor* actor, int flags, Vec3* from, Vec3* to, Actor* actor2, float* scale, int* a, int* b,
                float dist);
void fn_800BBCA8(Actor* actor);
void fn_800BC56C(Actor* actor, void* target);
void fn_800BC5A8(Actor* actor, Vec3* to);
int fn_800BF2D0(Vec3* p);
void fn_800CC984();
void fn_800CE96C();
void fn_800CF064();
void fn_800C748C();
void fn_800A70D0();
float fn_800E1020(Vec3* v);
void fn_800E1098(void* out, void* in, float length);
void fn_80162610(Actor* actor, void* p, int a);
void fn_80162658(Actor* actor, void* p);
int fn_80162940(void* p, int a);
void fn_8016D864(ZoneHitBox* out, void* src);
int fn_8016F120(void* fx, int* pos);
void fn_8028C188(Buffer* b, char* data, int len);
}

extern void* lbl_804D3600;  // "trib_see" effect
extern void* lbl_804D3604;  // "ice_frozenimps" effect

static inline void ImpStateName(Buffer* out, const char* s) {
    String name(s);
    String newline("\n");
    const String& text = fn_800225C8(name, newline);

    fn_8028C188(out, text.mData, text.mLength);
}

inline int ImpHandle41D(void* data) {
    Msg41D* m = (Msg41D*)data;
    Actor* o = m->actor;
    ImpBeh* b;

    if (fn_80019F88(data, o) != 0) {
        return 1;
    }
    b = o->beh;
    switch (m->cmd) {
    case 0x56D8C010:
        lbl_803BECD8[b->state].exit(o);
        lbl_803BECD8[1].enter(o);
        fn_800A7B08(o->st, 3, 1);
        b->timer = 500.0f;
        return 0;
    }
    return 0;
}

int ImpMsg(int msg, void* data) {
    Actor* a = *(Actor**)data;

    switch (msg) {
    case 0x400: {
        Msg400* m = (Msg400*)data;

        m->ownerId = OwnerId((Owner*)a);
        m->state = (a->health > 0.0f) ? 1 : 0;
        m->f10 = 1.0f;
        m->f14 = 1.0f;
        m->flags = 2;
        m->unk1C = 0;
        return 1;
    }
    case 0x41E: {
        Msg41E* m = (Msg41E*)data;
        ImpBeh* b = a->beh;

        switch (m->cmd) {
        case 0xAD8576FA:
            ImpStateName(&m->result, lbl_803BECD8[b->state].name);
            return 0;
        }
        return 0;
    }
    case 0x41D:
        return ImpHandle41D(data);
    case 0x407: {
        Msg407* m = (Msg407*)data;
        u32 type = m->type;
        ImpBeh* b = a->beh;

        if (type == 0x20EE36 || type == 0x12DD2E2A) {
            return 1;
        }
        if (type == 0x11A6B) {
            if (HasCondition(a->st, 9) == 1) {
                return 1;
            }
            if (fn_800A7B78(a->st, 10) != 0) {
                return 1;
            }
            a->health -= m->damage;
            a->unk214 = m->unk44;
            if (a->health <= 0.0f) {
                if (HasCondition(a->st, 0xC)) {
                    fn_800A7A54(a->st, 0xC, 0);
                    fn_80090390(a->unkD4, 0);
                }
                fn_800A7A54(a->st, 9, 1);
            } else {
                fn_800A7A54(a->st, 4, 1);
            }
            if (b->hurt <= 0.0f) {
                fn_8016F120(lbl_804D3604, &a->x);
                b->hurt = 800.0f;
            }
            fn_800B20B8(a, m->unk08);
            return 1;
        }
        if (type == 0x1F336D) {
            m->damage = 1.0f + a->health;
        }
        return 0;
    }
    case 0x40A: {
        Msg40A* m = (Msg40A*)data;
        Msg400 q;

        q.actor = a;
        q.ownerId = OwnerId((Owner*)a);
        m->actor->msgFn(0x400, &q);
        if (ActorPlayAnim(OwnerId((Owner*)a), StrHashSeed("QUERY", m->anim), m->unk04) != 0) {
            m->result = 0;
            return 1;
        }
        if (fn_800A7B78(a->st, 0xB) != 0) {
            return 1;
        }
        if (a->health > 0.0f && HasCondition(a->st, 0xA) == 0 && (q.flags & 2) && m->anim == 0x3CA34BD6) {
            m->result = 0x1379C466;
            return 1;
        }
        return 0;
    }
    case 0x409: {
        Msg409* m = (Msg409*)data;

        return ActorPlayAnim(OwnerId((Owner*)a), m->arg, m->anim) != 0;
    }
    }
    return 0;
}

// Picks the state the imp should be in: 2 (chase) when told to, 1 (attack) with a target in sight, else 0.
inline int ImpPickState(Actor* a) {
    int kind;
    void* target;

    if (a == 0 || a->beh == 0) {
        return 0;
    }
    kind = fn_800B26B8(a);
    if (fn_800A7B78(a->st, 5) != 0 && (kind == 4 || a->unk220 != 0)) {
        return 2;
    }
    if (kind != 4) {
        fn_800B261C(a, kind, &target);
        if (target != 0) {
            return 1;
        }
    }
    return 0;
}

int ImpTick(void* owner) {
    Actor* a = (Actor*)owner;
    ImpBeh* b = a->beh;
    int next;

    if (a->health <= 0.0f) {
        return 1;
    }
    if (b->hurt > 0.0f) {
        b->hurt -= a->dt;
    }
    if (a->unk220 == 2) {
        Msg400 q;

        if (SendMsg(a->unk22C->id, 0x400, &q) == 0 || q.state == 0) {
            a->unk220 = 0;
            fn_800A7028(a->st);
            b->flags |= 1;
        }
    }
    if (lbl_803BECD8[b->state].update(a) != 0 || (b->flags & 1)) {
        b->flags &= ~1;
        next = ImpPickState(a);
        if (b->state != next && lbl_803BECD8[b->state].exit(a)) {
            lbl_803BECD8[next].enter(a);
            fn_800A7A54(a->st, 0x40, 0);
            fn_800A7A54(a->st, 0x41, 0);
            fn_800A7A54(a->st, 0x43, 0);
            fn_800A7A54(a->st, 0x42, 0);
        }
    }
    if (fn_800BAF40(a) == -1) {
        a->unk288 = 0;
    }
    fn_80162658(a, a->unk894);
    return 1;
}

void ImpFree(Actor* a) {
    VFree(a->beh);
    a->unk104 = 0;
    a->beh = 0;
}

void ImpInit(void* owner) {
    Actor* a = (Actor*)owner;
    ImpBeh* b;

    b = (ImpBeh*)VAlloc(sizeof(ImpBeh));
    if (b) {
        lbl_804D3600 = FxFind("trib_see");
        lbl_804D3604 = FxFind("ice_frozenimps");
        a->beh = b;
        a->unkFC = (void*)fn_800CE96C;
        a->unk604 = (void*)fn_800CF064;
        a->unk5FC = (void*)fn_800CC984;
        a->unk600 = (void*)fn_800CC984;
        b->state = ImpPickState(a);
        b->flags = 0;
        b->seed = rand() % 256;
        b->hurt = -1.0f;
        lbl_803BECD8[b->state].enter(a);
    }
}

int ImpChaseUpdate(Actor* a) {
    int kind;
    void* target;

    if (a == 0) {
        return 0;
    }
    kind = fn_800B26B8(a);
    if (kind != 4) {
        fn_800B261C(a, kind, &target);
        if (fn_800A7B78(a->st, 5) == 0) {
            a->unk220 = 0;
            return 1;
        }
    }
    if (a->unk220 == 0 && fn_800A7B78(a->st, 5) == 0) {
        return 1;
    }
    if (a->unk220 == 2) {
        a->unk10C = (void*)fn_800C748C;
        if (fn_800A7A18(a->st) != a->unk22C) {
            fn_800A78A0(a->st, a->unk22C);
            if (a->unk21C != 0) {
                a->unk288 = a->unk21C;
                fn_800BC56C(a, a->unk22C);
            }
        }
        if (fn_800BAF40(a) == -1 && a->unk21C != 0) {
            fn_800A78A0(a->st, 0);
            a->unk288 = 0;
        }
    }
    return 0;
}

int ImpChaseExit(Actor* a) {
    if (a == 0) {
        return 0;
    }
    a->unk10C = (void*)fn_800A70D0;
    if (a->unk220 == 4) {
        a->unk220 = 0;
    }
    if (HasCondition(a->st, 0xD)) {
        fn_800A7A54(a->st, 0xD, 0);
    }
    return 1;
}

int ImpChaseEnter(Actor* a) {
    if (a == 0) {
        return 0;
    }
    a->beh->state = 2;
    a->unk288 = a->unk21C;
    fn_800BC56C(a, a->unk22C);
    return 1;
}

int ImpAttackExit(Actor* a) {
    fn_800A7B08(a->st, 4, 0);
    fn_800A7B08(a->st, 3, 0);
    fn_800A7B08(a->st, 0, 0);
    return 1;
}

int ImpAttackUpdate(Actor* a) {
    ImpBeh* b = a->beh;
    int kind;
    Target* target;

    kind = fn_800B26B8(a);
    if (kind == 4) {
        return 1;
    }
    fn_800B261C(a, kind, &target);
    fn_800A78A0(a->st, target);
    if (fn_800B1EF4(a, kind) == 0) {
        fn_800B1F20(a, kind);
    } else if (fn_800B2454(a, kind) == 0) {
        fn_800B2418(a, kind, 10000.0f);
    }
    if (fn_800A7B78(a->st, 0) != 0) {
        fn_800A7B08(a->st, 4, 1);
        if (b->timer <= 0.0f) {
            if (fn_80162940(a->unk894, 2) && fn_80162940(a->unk894, 8)) {
                fn_80162610(a, a->unk894, 2);
                b->timer = 800.0f * (rand() / 32767.0f) + 1600.0f;
            }
        } else {
            b->timer -= a->dt;
        }
    } else if (fn_800A7B78(a->st, 3) != 0) {
        fn_800BBCA8(a);
        a->unk288 = 0;
    } else if (HasCondition(a->st, 5)) {
        b->timer = 4000.0f * (rand() / 32767.0f) + 3000.0f;
        fn_800A7A54(a->st, 5, 0);
    } else {
        b->timer -= a->dt;
        if (b->timer <= 0.0f) {
            fn_800A7B08(a->st, 3, 1);
            fn_800A7B08(a->st, 4, 0);
            b->timer = 500.0f;
        }
    }
    if (fn_800A7B78(a->st, 3) == 0) {
        b->chase -= a->dt;
        if (b->chase < 0.0f) {
            float dist = OwnerValue((Owner*)a, target);

            if (dist < 800.0f) {
                fn_800BBCA8(a);
                a->unk288 = 0;
            } else if (dist > 1800.0f) {
                fn_800BC56C(a, target);
                a->unk288 = 3;
            }
            if (b->chase < -6000.0f) {
                b->chase = 10000.0f * (rand() / 32767.0f) + 8000.0f;
            }
        } else if (fn_80162940(a->unk894, 0x800)) {
            fn_800A7B08(a->st, 0x1D, 1);
            fn_800BC56C(a, target);
            a->unk288 = 3;
        } else if (fn_80162940(a->unk894, 0x400)) {
            if (b->near > 800.0f) {
                Vec3 dir;
                Vec3 to;
                float scale;
                int hitA;
                int hitB;

                dir.x = target->x;
                dir.y = target->y;
                dir.z = target->z;
                dir.x = a->pos.x - dir.x;
                dir.y = a->pos.y - dir.y;
                dir.z = a->pos.z - dir.z;
                dir.z = 0.0f;
                fn_800E1098(&dir, &dir, 2000.0f);
                to.x = dir.x + a->pos.x;
                to.y = dir.y + a->pos.y;
                to.z = dir.z + a->pos.z;
                if (fn_800BB218(a, a->unk2C4, &a->pos, &to, a, &scale, &hitB, &hitA, a->unk900)) {
                    float len = fn_800E1020(&dir);
                    fn_800BAFF8(a, &dir, hitB, hitA, &to, scale, len * (1.0f - scale));
                }
                if (fn_800BF2D0(&to) == fn_800BF2D0(&a->pos)) {
                    fn_800A7B08(a->st, 0x1D, 1);
                    fn_800BC5A8(a, &to);
                    a->unk288 = 3;
                }
            }
        } else if (fn_800BAF40(a) != -1) {
            float dist = OwnerValue((Owner*)a, target);

            if (fn_800A7B78(a->st, 0x1D) != 0 && dist > (float)a->nearMin && dist < (float)a->nearMax) {
                fn_800A7B08(a->st, 0x1D, 0);
                fn_800BBCA8(a);
                a->unk288 = 0;
            }
        }
    }
    if (fn_80162940(a->unk894, 0x800) || fn_80162940(a->unk894, 0x400)) {
        b->near += a->dt;
    } else {
        b->near = 0.0f;
    }
    return 0;
}

int ImpAttackEnter(Actor* a) {
    ImpBeh* b = a->beh;

    b->state = 1;
    fn_800A7B08(a->st, 4, 1);
    fn_800A7B08(a->st, 3, 0);
    fn_800A7B08(a->st, 0, 0);
    b->timer = 4000.0f * (rand() / 32767.0f) + 3000.0f;
    b->near = 0.0f;
    b->chase = 10000.0f * (rand() / 32767.0f) + 8000.0f;
    fn_8016F120(lbl_804D3600, &a->x);
    return 1;
}

int ImpIdleExit(Actor* a) {
    return 1;
}

int ImpIdleUpdate(Actor* a) {
    if (fn_800B26B8(a) != 4) {
        return 1;
    }
    return a->unk220 != 0;
}

int ImpIdleEnter(Actor* a) {
    a->beh->state = 0;
    fn_800BBCA8(a);
    a->unk288 = 0;
    return 1;
}

// ---------------------------------------------------------------------------------------------------
// The imp's attack sub-behaviour: an eight-entry enter/exit/update table (0x803BED88) driven by the
// generic behaviour runner (BehUpdate and friends, 0x80161F40-0x80163xxx).

struct ImpAttackData {
    int state;   // 0x0
    int next;    // 0x4
    int unk8;
    u32 flags;   // 0xC
    int unk10;
};

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

struct Beh {
    u8 pad0[0x10];
    int state;                                       // 0x10
    u8 pad14[0x30 - 0x14];
    int targetKind;                                  // 0x30
    int target;                                      // 0x34
    u8 pad38[0x4C - 0x38];
    void (*onState)(void* owner, void* beh, int s);  // 0x4C
    u8 pad50[4];
    ImpAttackData* data;                             // 0x54
};

struct BehMsg {
    u8 pad[0x48];
    Actor* target;  // 0x48
    u8 pad4C[0x60 - 0x4C];
};

void BehUpdate(Owner* o, Beh* b);                         // 0x801632A8
void BehDone(Beh* b);                                     // 0x80162EBC
void BehReset(Beh* b);                                    // 0x801633D4
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f); // 0x800F7F5C
int Fn80161FF4(Owner* o, void* buf, float f);            // 0x80161FF4
void Fn8016295C(Owner* o, Beh* b);                        // 0x8016295C

extern BehState lbl_803BED88[8];

// Registered in gBehTypes (u_80161F40) under these names.
extern "C" {
int fn_80252F2C(void* owner, void* beh);
int fn_80252F34(void* owner, void* beh, int msg);
int fn_802531B4(void* owner, void* beh);
int fn_80253328(void* owner, void* beh);
int fn_80253360(void* owner, void* beh);
}

int fn_80252F2C(void* owner, void* beh) {
    return 0;
}

int fn_80252F34(void* owner, void* beh, int msg) {
    Actor* o = (Actor*)owner;
    Beh* b = (Beh*)beh;
    ImpAttackData* d = b->data;

    switch (msg) {
    case 1:
        d->next = 4;
        return 1;
    case 2:
        d->next = 5;
        return 1;
    case 0: {
        char buf[96];

        Fn800F7F5C(o->unk8EC, o->unk8F0, 0x1B83833F, OwnerId((Owner*)o), buf, o->dt);
        d->next = 0;
        return 1;
    }
    case 0x1B83833F: {
        char buf[96];

        Fn800F7F5C(o->unk8EC, o->unk8F0, 0x1B83833F, OwnerId((Owner*)o), buf, o->dt);
        d->flags |= 1;
        return 0;
    }
    case 0xF26951B8: {
        char buf[96];

        Fn800F7F5C(o->unk8EC, o->unk8F0, msg, OwnerId((Owner*)o), buf, o->dt);
        return 1;
    }
    case 0x0CAE7B51: {
        char buf[96];

        Fn800F7F5C(o->unk8EC, o->unk8F0, msg, OwnerId((Owner*)o), buf, o->dt);
        return 1;
    }
    case 0x20EE36: {
        BehMsg m;
        Mtx34 mtxA;
        Mtx34 mtxB;
        Msg400 q;
        ZoneHitBox boxA;
        ZoneHitBox boxB;

        if (b->targetKind == 2) {
            int id = b->target;

            if (id != -1) {
                Actor* t;

                q.ownerId = id;
                if (SendMsg(q.ownerId, 0x400, &q) != 0 && (t = q.actor) != 0) {
                    ActorGetMatrix(&mtxA, o);
                    ActorGetMatrix(&mtxB, t);
                    fn_8016D864(&boxA, o->model);
                    fn_8016D864(&boxB, t->model);
                    if (ModelBoneHitBox((int)&boxA, (int)&mtxA, (int)&boxB, (int)&mtxB)) {
                        m.target = t;
                        Fn800F7F5C(o->unk8EC, o->unk8F0, 0x20EE36, OwnerId((Owner*)o), &m, o->dt);
                        return 1;
                    }
                }
                return 0;
            }
        }
        return 0;
    }
    case 0xF829DB93:
        d->flags |= 4;
        return 1;
    }
    return 1;
}

int fn_802531B4(void* owner, void* beh) {
    Actor* o;
    Beh* b;
    ImpAttackData* d;

    if (!beh) {
        return 0;
    }
    o = (Actor*)owner;
    b = (Beh*)beh;
    d = b->data;
    if (HasCondition(o->st, 9) || HasCondition(o->st, 10) || HasCondition(o->st, 11)) {
        d->flags = 0;
        BehReset(b);
        BehDone(b);
    } else {
        if (lbl_803BED88[d->state].update) {
            lbl_803BED88[d->state].update(o, b);
        }
        if (d->next != d->state) {
            if (lbl_803BED88[d->state].exit) {
                lbl_803BED88[d->state].exit(o, b);
            }
            d->state = d->next;
            b->state = d->next;
            if (b->onState) {
                b->onState(o, b, d->next);
            }
            if (lbl_803BED88[d->state].enter) {
                lbl_803BED88[d->state].enter(o, b);
            }
        }
    }
    return 1;
}

int fn_80253328(void* owner, void* beh) {
    if (!beh) {
        return 0;
    }
    VFree(((Beh*)beh)->data);
    return 1;
}

int fn_80253360(void* owner, void* beh) {
    Beh* b;
    ImpAttackData* d;

    if (!beh) {
        return 0;
    }
    b = (Beh*)beh;
    d = (ImpAttackData*)VAlloc(sizeof(ImpAttackData));
    if (d == 0) {
        b->data = 0;
        return 0;
    }
    b->data = d;
    b->state = 0;
    d->state = 0;
    d->next = 0;
    d->flags = 0;
    if (lbl_803BED88[d->state].enter) {
        lbl_803BED88[d->state].enter(owner, b);
    }
    return 1;
}

void ImpAttackBehState5Update(void* owner, void* beh) {
    char buf[96];

    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        ImpAttackData* d = b->data;

        BehUpdate(o, b);
        Fn8016295C(o, b);
        Fn80161FF4(o, buf, 0.0f);
        Fn800F7F5C(((Actor*)o)->unk8EC, ((Actor*)o)->unk8F0, 0xF829DB93, OwnerId(o), buf, ((Actor*)o)->dt);
        d->next = 0;
    }
}

void ImpAttackBehState4Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        ImpAttackData* d = b->data;

        BehUpdate(o, b);
        Fn8016295C(o, b);
        if (d->flags & 1) {
            d->next = 0;
        }
    }
}

void ImpAttackBehState0Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}

void ImpAttackBehState5Exit(void* owner, void* beh) {}

void ImpAttackBehState4Exit(void* owner, void* beh) {}

void ImpAttackBehState0Exit(void* owner, void* beh) {}

void ImpAttackBehState5Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        b->data->flags = 0;
        BehDone(b);
    }
}

void ImpAttackBehState4Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        b->data->flags = 0;
        BehDone(b);
    }
}

void ImpAttackBehState0Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;
        ImpAttackData* d = b->data;

        d->flags = 0;
        d->next = 0;
        BehDone(b);
    }
}

// ---------------------------------------------------------------------------------------------------
// The "IMP FIRE" effect type.


extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);


inline u32 StrHash(const char* s) {
    if (s == 0) {
        return 0;
    } else {
        u32 h = 0;
        int i;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
        return h;
    }
}

struct EffectType {
    const char* name;
    u32 hash;
    int unk8[3];
    void* (*create)(int a, int id);
    void* destroy;
    void* msg;
    void* query;
    void* update;
    void* reset;
    int unk2C;
};

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}
struct EffectObj {
    int unk0;
    int unk4;
    int emitter;
    void* data;
};

extern "C" {
int fn_800F7D28(int emitter, int id);
void fn_800F86A0(int emitter);
}

void VFree(void* p);

struct Player;


struct ImpShot {
    u8 pad00[0x30];
    float x, y, z;
    u8 pad3C[0x48 - 0x3C];
    Target* target;
    int pad4C;
    float spread;
    float scale;
    u8 pad58[0x60 - 0x58];
};

struct ImpShotInfo {
    ImpShot* hit;
    float speed;
    float unk8;
    int unkC;
    int charge;
    int unk14;
    int unk18;
    int unk1C;
    int unk20;
};

struct MsgResult {
    void* p;
    char pad[0x1C];
};

struct ImpFireDef {
    int unk00;
    int unk04;  // ammo id
    int unk08;  // ammo max
    float speed;
    float unk10;
    int unk14;
    int charge;
    int drain;
    int maxCharge;
    int spread;
    int unk28;
    int unk2C;
    void* fx;
    u8 pad34[0x48 - 0x34];
    int model;
    float recoil;
    float recoilRate;
    float spreadMinA;
    float spreadMaxA;
    float spreadMinB;
    float spreadMaxB;
};

struct ImpFire {
    int unk00;  // state
    float unk04;  // timer
    int emitter;
    ImpFireDef* def;
    int unk10;
    float charge;
    float recoil;
    float recoilTimer;
    int arg;
    ImpShot shot;
    int unk84;
};

extern "C" {
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
int FxStart(void* fx, int a, int b);
int SendMsg(int target, int msg, void* data);
void fn_8000F550(short* ids, int count);
}

int Fn80175D60(unsigned long arg);
void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
void PlayerAnimOffUpdate(Player* p, float dt);
int AssetFind(u32 table, u32 key);
void TextureLoad(short* ids, int count);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);
void ImpFireAssetsFree(void);
void ImpFireAssetsLoad(void);

inline float Min(float a, float b) { return a < b ? a : b; }



// Launch parameters for the fire ball (see fn_802541D4).
struct ImpThrow {
    Vec3 pos;
    u8 pad0C[0x18 - 0x0C];
    Vec3 vel;
    Vec3 spin;
    Vec3 accel;
    u8 pad3C[0x4C - 0x3C];
};

extern "C" {
int rand(void);
float fn_800E1020(Vec3* v);
void fn_800E1098(void* out, void* in, float length);
}
void fn_802541D4(int arg, ImpThrow* t, int n);

inline float FastSqrt(float x) {
    register float r;
    float y;

    if (x <= 0.0f) {
        return 0.0f;
    } else {
        asm {
            lfs r, x
            ps_rsqrte r, r
            ps_res r, r
            stfs r, y
        }
        return y;
    }
}

inline int FxStartAt(void* fx, float* p, int* pos) {
    int h;
    if (p) {
        pos[0] = p[0];
        pos[1] = p[1];
        pos[2] = p[2];
        h = fn_8016F120(fx, pos);
        fn_8016F118(h, 0, 100000);
    } else {
        h = FxStart(fx, 0, 0);
    }
    return h;
}

extern EffectType lbl_803BEDE8;  // the "IMP FIRE" type descriptor

void ImpFireReset(EffectObj* obj) {
    ImpFire* g = (ImpFire*)obj->data;

    g->unk00 = 0;
    g->unk04 = 0.0f;
}

void ImpFireUpdate(EffectObj* obj, float dt) {
    ImpFire* g = (ImpFire*)obj->data;
    ImpFireDef* def = g->def;

    if (g == 0) {
        return;
    }
    switch (g->unk00) {
    case 0:
        break;
    case 1:
        g->unk04 -= dt;
        while (g->unk04 <= 0.0f) {
            if (fn_800F7D28(g->emitter, def->unk04) > 0) {
                MsgResult q;
                ImpThrow t;
                Vec3 d;
                int pos[3];
                Target* src;
                Target* dst;
                void* fx;
                float rz;
                float ry;
                float rx;
                float up;
                float root;

                if (g->shot.target == 0) {
                    return;
                }
                SendMsg(g->arg, 0x400, &q);
                src = (Target*)q.p;
                memset(&t, 0, sizeof(ImpThrow));
                t.pos.x = src->x;
                t.pos.y = src->y;
                t.pos.z = src->z;
                t.accel.x = 0.0f;
                t.accel.y = 0.0f;
                t.accel.z = -6000.0f;
                rz = rand() / 32767.0f;
                ry = rand() / 32767.0f;
                rx = rand() / 32767.0f;
                t.spin.x = 800.0f * rx - 400.0f;
                t.spin.y = 800.0f * ry - 400.0f;
                t.spin.z = 800.0f * rz - 400.0f;
                t.vel.x = 0.0f;
                t.vel.y = 0.0f;
                t.vel.z = 3500.0f;
                dst = g->shot.target;
                d.x = dst->x;
                d.y = dst->y;
                d.z = dst->z;
                d.x -= t.pos.x;
                d.y -= t.pos.y;
                d.z -= t.pos.z;
                up = -(t.vel.z / t.accel.z);
                root = FastSqrt((d.z - 0.5f * t.vel.z * up) / (0.5f * t.accel.z));
                {
                    float len = fn_800E1020(&d);
                    d.z = 0.0f;
                    fn_800E1098(&d, &d, len / (up + root));
                }
                t.vel.x += d.x;
                t.vel.y += d.y;
                t.vel.z += d.z;
                fn_802541D4(g->arg, &t, 1);
                fx = def->fx;
                FxStartAt(fx, &g->shot.x, pos);
            }
        }
        break;
    }
}

u32 ImpFireQuery(EffectObj* obj, int msg) {
    ImpFire* g = (ImpFire*)obj->data;
    ImpFireDef* def = g->def;

    switch (msg) {
    case 0x0020EE36:
        return g->unk00 != 0 ? 0x3DA260F7 : 0x04A3E183;
    case 0xF829DB93:
        if (g->unk00 != 0) {
            return 0x3DA260F7;
        }
        if (fn_800F7D28(g->emitter, def->unk04)) {
            return 0x04A3E183;
        }
        return fn_800F7D28(obj->emitter, def->unk04) != 0 ? 0xC0BECF7E : 0x11D2C025;
    case 0x9791AF51:
    case 0x968CBE3D:
    case 0x96224FCC:
    case 0xE6B71BCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        return 0x3DA260F7;
    }
    return 0x3DA260F7;
}

int ImpFireMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    ImpFire* g = (ImpFire*)obj->data;
    ImpFireDef* def = g->def;
    short texs[6];

    g->arg = arg;
    Fn80175D60(g->arg);
    switch (msg) {
    case 0x1B83833F:
        g->unk00 = 0;
        return 1;
    case 0xF26951B8:
        ImpFireAssetsLoad();
        return 1;
    case 0x0CAE7B51:
        ImpFireAssetsFree();
        return 1;
    case 0xF829DB93:
    case 0x0020EE36: {
        ImpShotInfo info;
        MsgResult q;
        int pos[3];
        void* fx;

        g->unk10 = 1;
        if (!fn_800F7D28(g->emitter, def->unk04)) {
            return 0;
        }
        g->charge += def->charge;
        g->charge = Min(g->charge, def->maxCharge);
        if (((arg >> 24) & 0xFF) == 2) {
            SendMsg(arg, 0x400, &q);
            PlayerAnimOffUpdate((Player*)q.p, def->recoil);
            g->recoil += def->recoil;
            g->recoilTimer = 300.0f;
        }
        info.hit = (ImpShot*)data;
        info.speed = def->speed;
        info.unk8 = def->unk10;
        info.unkC = def->unk14;
        info.charge = g->charge;
        info.unk18 = def->unk28;
        info.unk1C = def->unk2C;
        if (((g->arg >> 24) & 0xFF) == 2) {
            ((ImpShot*)data)->spread =
                ((ImpShot*)data)->spread * (def->spreadMaxA - def->spreadMinA) + def->spreadMinA;
        } else {
            ((ImpShot*)data)->spread =
                ((ImpShot*)data)->spread * (def->spreadMaxB - def->spreadMinB) + def->spreadMinB;
        }
        info.hit->spread *= info.hit->scale;
        Fn800F7F5C(g->emitter, def->unk04, 0x14967, arg, &info, dt);
        fx = def->fx;
        FxStartAt(fx, &g->shot.x, pos);
        return 1;
    }
    case 0x968CBE3D:
    case 0x8FDD7599:
    case 0xBC78F550:
    case 0xE6B71BCC:
    case 0x0B59E861:
        return 1;
    case 0xE452286F:
    case 0x141109BD: {
        short model;

        model = def->model;
        texs[0] = AssetFind(0xF2E2BEB8, 0x5541EDD2);
        texs[1] = AssetFind(0xF2E2BEB8, 0x56F6C671);
        texs[2] = AssetFind(0xF2E2BEB8, 0x58AB9F10);
        texs[3] = AssetFind(0xF2E2BEB8, 0x5A6077AF);
        texs[4] = AssetFind(0xF2E2BEB8, 0x2E20AEE7);
        texs[5] = AssetFind(0xF2E2BEB8, 0x961C77AB);
        if ((int)data == 1) {
            ModelLoadIds(&model, 1);
            TextureLoad(texs, 6);
        } else {
            ModelFreeIds(&model, 1);
            fn_8000F550(texs, 6);
        }
        return 1;
    }
    }
    return 0;
}

void ImpFireDestroy(EffectObj* obj) {
    ImpFire* g = (ImpFire*)obj->data;

    fn_800F86A0(g->emitter);
    VFree(g);
}

void* ImpFireCreate(int a, int id) {
    EffectType* type = &lbl_803BEDE8;
    ImpFire* g;

    EffectTypeHash(type);
    g = (ImpFire*)VAlloc(sizeof(ImpFire));
    memset(g, 0, sizeof(ImpFire));
    if (g == 0) {
        return 0;
    }
    g->def = (ImpFireDef*)fn_800EB21C(id);
    g->unk00 = 0;
    g->unk04 = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    return g;
}


// ---------------------------------------------------------------------------------------------------
// Fire ball asset helpers: entry 0 of the name table is the model, entries 1-8 its textures.

struct SplineNameEntry {
    char* name;
    u32 group;
    u32 hash;
    int asset;
};

int SplineAssetResolve(SplineNameEntry* entries, int count, int index);

extern "C" {
void fn_8003D710(int model);
void fn_8003D8F4(int model);
void fn_8000F60C(int tex);
void fn_8000F874(int tex);
}

extern SplineNameEntry lbl_803BEFB8[9];

void ImpFireAssetsFree(void) {
    u32 i;

    fn_8003D710(SplineAssetResolve(lbl_803BEFB8, 9, 0));
    for (i = 1; i < 9; i++) {
        fn_8000F60C(SplineAssetResolve(lbl_803BEFB8, 9, i));
    }
}

void ImpFireAssetsLoad(void) {
    u32 i;

    fn_8003D8F4(SplineAssetResolve(lbl_803BEFB8, 9, 0));
    for (i = 1; i < 9; i++) {
        fn_8000F874(SplineAssetResolve(lbl_803BEFB8, 9, i));
    }
}

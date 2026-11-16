// u_80251CB8.cpp: the imp actor behaviour (0x80251CB8-0x80252F2C: message handler, tick and a three-state
// idle/attack/chase machine). The attack sub-behaviour is u_80252F2C.cpp and the "IMP FIRE" effect type is
// u_80253614.cpp: each has its own copy of the action-name pool or of 0.0f / the int-to-float double.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

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

struct Owner;
struct Actor;

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

int ImpIdleEnter(Actor* a);
int ImpIdleExit(Actor* a);
int ImpIdleUpdate(Actor* a);
int ImpAttackEnter(Actor* a);
int ImpAttackExit(Actor* a);
int ImpAttackUpdate(void* owner);
int ImpChaseEnter(Actor* a);
int ImpChaseExit(Actor* a);
int ImpChaseUpdate(Actor* a);

ImpStateType gImpStates[3] = {
    { (ImpStateFunc)ImpIdleEnter, (ImpStateFunc)ImpIdleExit, (ImpStateFunc)ImpIdleUpdate, "" },
    { (ImpStateFunc)ImpAttackEnter, (ImpStateFunc)ImpAttackExit, ImpAttackUpdate, "" },
    { (ImpStateFunc)ImpChaseEnter, (ImpStateFunc)ImpChaseExit, (ImpStateFunc)ImpChaseUpdate, "" },
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
float fn_800E1020(Vec3* v);
void fn_800E1098(void* out, void* in, float length);
void fn_80162610(Actor* actor, void* p, int a);
void fn_80162658(Actor* actor, void* p);
int fn_80162940(void* p, int a);
void fn_8016D864(ZoneHitBox* out, void* src);
int fn_8016F120(void* fx, int* pos);
void fn_8028C188(Buffer* b, char* data, int len);
}
int MemFlagIsSet(void* bits, int index);
int ConditionUpdateBit(void* cond, int bit, int set);
int ConditionSetBit(void* cond, int bit, int set);
void* FollowGetTargetObj(void* range);
void FollowSetTargetObj(void* range, void* obj);
int FollowOffsetUpdate(void* self);
void RangeResetPending(void* range);

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
        gImpStates[b->state].exit(o);
        gImpStates[1].enter(o);
        ConditionUpdateBit(o->st, 3, 1);
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
            ImpStateName(&m->result, gImpStates[b->state].name);
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
            if (MemFlagIsSet(a->st, 10) != 0) {
                return 1;
            }
            a->health -= m->damage;
            a->unk214 = m->unk44;
            if (a->health <= 0.0f) {
                if (HasCondition(a->st, 0xC)) {
                    ConditionSetBit(a->st, 0xC, 0);
                    fn_80090390(a->unkD4, 0);
                }
                ConditionSetBit(a->st, 9, 1);
            } else {
                ConditionSetBit(a->st, 4, 1);
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
        if (MemFlagIsSet(a->st, 0xB) != 0) {
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
    if (MemFlagIsSet(a->st, 5) != 0 && (kind == 4 || a->unk220 != 0)) {
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
            RangeResetPending(a->st);
            b->flags |= 1;
        }
    }
    if (gImpStates[b->state].update(a) != 0 || (b->flags & 1)) {
        b->flags &= ~1;
        next = ImpPickState(a);
        if (b->state != next && gImpStates[b->state].exit(a)) {
            gImpStates[next].enter(a);
            ConditionSetBit(a->st, 0x40, 0);
            ConditionSetBit(a->st, 0x41, 0);
            ConditionSetBit(a->st, 0x43, 0);
            ConditionSetBit(a->st, 0x42, 0);
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
        gImpStates[b->state].enter(a);
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
        if (MemFlagIsSet(a->st, 5) == 0) {
            a->unk220 = 0;
            return 1;
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
    }
    return 0;
}

int ImpChaseExit(Actor* a) {
    if (a == 0) {
        return 0;
    }
    a->unk10C = (void*)FollowOffsetUpdate;
    if (a->unk220 == 4) {
        a->unk220 = 0;
    }
    if (HasCondition(a->st, 0xD)) {
        ConditionSetBit(a->st, 0xD, 0);
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
    ConditionUpdateBit(a->st, 4, 0);
    ConditionUpdateBit(a->st, 3, 0);
    ConditionUpdateBit(a->st, 0, 0);
    return 1;
}

int ImpAttackUpdate(void* owner) {
    Actor* a = (Actor*)owner;
    ImpBeh* b = a->beh;
    int kind;
    Target* target;

    kind = fn_800B26B8(a);
    if (kind != 4) {
        fn_800B261C(a, kind, &target);
        FollowSetTargetObj(a->st, target);
        if (fn_800B1EF4(a, kind) == 0) {
            fn_800B1F20(a, kind);
        } else if (fn_800B2454(a, kind) == 0) {
            fn_800B2418(a, kind, 10000.0f);
        }
    } else {
        return 1;
    }
    if (MemFlagIsSet(a->st, 0) != 0) {
        ConditionUpdateBit(a->st, 4, 1);
        if (b->timer <= 0.0f) {
            if (fn_80162940(a->unk894, 2) && fn_80162940(a->unk894, 8)) {
                float t;

                fn_80162610(a, a->unk894, 2);
                t = rand() / 32767.0f;
                b->timer = 800.0f * t + 1600.0f;
            }
        } else {
            b->timer -= a->dt;
        }
    } else if (MemFlagIsSet(a->st, 3) != 0) {
        fn_800BBCA8(a);
        a->unk288 = 0;
    } else if (HasCondition(a->st, 5)) {
        b->timer = 4000.0f * (rand() / 32767.0f) + 3000.0f;
        ConditionSetBit(a->st, 5, 0);
    } else {
        b->timer -= a->dt;
        if (b->timer <= 0.0f) {
            ConditionUpdateBit(a->st, 3, 1);
            ConditionUpdateBit(a->st, 4, 0);
            b->timer = 500.0f;
        }
    }
    if (MemFlagIsSet(a->st, 3) == 0) {
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
            ConditionUpdateBit(a->st, 0x1D, 1);
            fn_800BC56C(a, target);
            a->unk288 = 3;
        } else if (fn_80162940(a->unk894, 0x400)) {
            if (b->near > 800.0f) {
                Vec3 to;
                Vec3 dir;
                int hitB;
                int hitA;
                float scale;

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

                    len *= 1.0f - scale;
                    fn_800BAFF8(a, &dir, hitB, hitA, &to, scale, len);
                }
                if (fn_800BF2D0(&to) == fn_800BF2D0(&a->pos)) {
                    ConditionUpdateBit(a->st, 0x1D, 1);
                    fn_800BC5A8(a, &to);
                    a->unk288 = 3;
                }
            }
        } else if (fn_800BAF40(a) != -1) {
            float dist = OwnerValue((Owner*)a, target);

            if (MemFlagIsSet(a->st, 0x1D) != 0 && dist > (float)a->nearMin && dist < (float)a->nearMax) {
                ConditionUpdateBit(a->st, 0x1D, 0);
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
    ConditionUpdateBit(a->st, 4, 1);
    ConditionUpdateBit(a->st, 3, 0);
    ConditionUpdateBit(a->st, 0, 0);
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

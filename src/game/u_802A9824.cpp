// u_802A9824.cpp: companion (ally) AI behaviour of an actor (0x802A9824-0x802AE27C).
// A state machine whose states (idle, follow, pick-up, attack, ...) each have an enter function
// and an update function; the state record lives in the actor's behaviour block (+0x130).  The
// states look for a target (the actor's target-object slot at +0x270), query it through the
// variable messages 0x41E (read) / 0x41D (set) and walk, aim or interact with it.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned char u8;

#define NULL 0

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

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

// Variable query/set message: the receiver reads or writes the first word of the string.
struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[12];
};

struct Actor;

// Reply of the 0x400 "get actor info" message.
struct ActorInfoMsg {
    Actor* actor;           // 0x00
    int unk04;
    int unk08;
    int unk0C;
    int unk10;
    int unk14;
    u32 flags;              // 0x18
    int unk1C;
};

// Damage/strike message (0x407).
struct HitMsg {
    Actor* target;          // 0x00
    Actor* source;          // 0x04
    int handle;             // 0x08
    float force;            // 0x0C
    float scale;            // 0x10
    u32 type;               // 0x14
    int pos[3];             // 0x18
    int pad24[3];
    int delta[3];           // 0x30
    int pad3C[2];
    int z0;                 // 0x44
    int z1;                 // 0x48
    float one;              // 0x4C
    float zero;             // 0x50
};

// Behaviour block of the companion (Actor + 0x130).
struct AllyBeh {
    int state;              // 0x00
    float unk04;            // 0x04
    float timer;            // 0x08
    float unk0C;            // 0x0C
    int unk10;              // 0x10
    int unk14;              // 0x14
    int targetId;           // 0x18
    float unk1C;            // 0x1C
    u32 flags;              // 0x20
    int unk24;              // 0x24
    float pos[3];           // 0x28
    float unk34;            // 0x34
    int unk38;              // 0x38
    float unk3C;            // 0x3C
    u8 pad40[0x80 - 0x40];
};

struct Actor {
    u8 pad00[0x24];
    int pos[3];             // 0x24
    int pad30[2];
    int handle;             // 0x38
    u8 pad3C[0x84 - 0x3C];
    float fpos[3];          // 0x84
    u8 pad90[0x10C - 0x90];
    void* update;           // 0x10C
    u8 pad110[0x130 - 0x110];
    AllyBeh* beh;           // 0x130
    u8 pad134[4];
    float dt;               // 0x138
    u8 pad13C[0x21C - 0x13C];
    int unk21C;             // 0x21C
    int mode;               // 0x220
    u8 pad224[0x22C - 0x224];
    Actor* other;           // 0x22C
    u8 pad230[0x268 - 0x230];
    int unk268;             // 0x268
    int unk26C;             // 0x26C
    char conds[0x288 - 0x270]; // 0x270
    int anim;               // 0x288
    u8 pad28C[0x5EC - 0x28C];
    Actor* owner;           // 0x5EC
    u8 pad5F0[0x91C - 0x5F0];
    float timeScale;        // 0x91C
};

extern "C" int SendMsg(int to, int msg, void* data);

inline int QueryVar(int target, u32 hash, int* out) {
    QueryMsg m;
    int r;
    *(int*)&m.result = 0;
    m.hash = hash;
    m.unk8 = 0;
    r = SendMsg(target, 0x41E, &m);
    *out = *(int*)&m.result;
    return r;
}

inline void SetVar(int target, u32 hash, int value) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    SendMsg(target, 0x41D, &m);
}

struct AllyState {
    int (*enter)(Actor* a);         // 0x00
    int (*canLeave)(Actor* a);      // 0x04
    int (*update)(Actor* a);        // 0x08
    const char* name;               // 0x0C
};
extern AllyState gAllyStates[8];    // 0x803C40B8

// Follow-slot helpers (the target object and condition flags at Actor + 0x270).
Actor* FollowGetTargetObj(void* conds);
void FollowSetTargetObj(void* conds, void* obj);
void FollowSetTargetVec(void* conds, Vec3* v);
void RangeResetPending(void* conds);
void ConditionUpdateBit(void* conds, int bit, int on);
void ConditionSetBit(void* conds, int bit, int on);
int HasCondition(void* conds, int bit);
int MemFlagIsSet(void* flags, int bit);
int ActorPlayAnim(int handle, u32 hash, int arg);
void ActorAimOffset2D(void* actor, float* pos, float* a, float* b);
float OwnerValue(void* owner, void* other = 0);
void ActorGetMatrix(Mtx34* m, void* actor);
void EulerNormalize(Vec3* dst, Vec3* src);

extern "C" {
void* memcpy(void* dst, const void* src, u32 n);
void* memset(void* dst, int c, u32 n);
void fn_8029B070(Actor* a, u32 hash);
void fn_800CEEC8(void);
void fn_800CE538(void);
void fn_800C9B68(Actor* a);
void fn_800C9C68(Actor* a);
void fn_800B6CAC(Actor* a, void* fn);
void fn_800B5028(void);
void fn_800CC984(void);
int fn_80117D80(Actor* a);
void fn_800B20B8(Actor* a, int handle);
void fn_800C99E8(float f, Actor* a, int b, int c, int d, int e);
void fn_80162658(Actor* a, void* p);
void fn_8009FB94(void* a, void* b);
int ActorCount(void);
void* ActorGet(int i);
void VFree(void* p);
void* VAlloc(int size);
int AssetFind(u32 a, u32 b);

int SendMsg(int to, int msg, void* data);
float fn_802A2080(void);
int fn_800B26B8(Actor* a);
int fn_800BAF40(Actor* a);
int fn_800B8A7C(int a, u32 hash, int b, void* fn, Actor* c);
void fn_800BC56C(Actor* a, Actor* target);
void fn_800BBCA8(Actor* a);
void fn_800B261C(Actor* a, int slot, Actor** out);
int fn_800B22F8(Actor* a, int slot);
void fn_800B1F20(Actor* a, int slot);
int fn_800B2454(Actor* a, int slot);
void fn_800B2418(float f, Actor* a, int slot);
void fn_80116E30(Actor* a);
int fn_8029B3B0(Actor* a, u32 hash);
void fn_8029B218(Actor* a, u32 hash);
int fn_801174B0(Actor* a, Actor* target);
int fn_800B912C(int a, u32 hash, int b, int c, void* fn, Actor* d);
void fn_8001EAF8(int* pos, int radius, void* fn, Actor* a, int b);
void fn_801BDC3C(int a, Actor* b);
void fn_800C748C(void);
float sqrtf(float x);
void fn_800A70D0(void);
void fn_800A025C(Vec3* out, Mtx34* m, int col);
void fn_8016D440(float* out, Vec3* v);
void fn_8016D4EC(Vec3* out, float* e);
}
int AllyTargetFilter(int first, int second, Actor* a);
int AllyPickFilter(int id, Actor* self);
int AllyNearCallback(Actor* a, Actor* b);
int AllyStartMove(Actor* a, Actor* target);
int AllyPickState(Actor* a);
extern "C" {
int sprintf(char* s, const char* fmt, ...);
int rand(void);
void fn_800BB8B8(Actor* a, Vec3* out);
float ActorAimAngleToPos(Actor* a, Vec3* v);
void fn_800C51AC(const char* msg);
void fn_800E1020(Vec3* v);
void fn_800E1098(float f, Vec3* out, Vec3* in);
int fn_800BB218(float f, Actor* a, int b, float* from, Vec3* to, Actor* c, int* x, int* y, int* z);
void fn_800BC5A8(Actor* a, Vec3* v);
int fn_800BF2D0(Vec3* v);
}

extern int gAllyReact0;         // 0x804D38B0
extern int gAllyReact1;         // 0x804D38B4
extern float gAllyTime;         // 0x804D38D4
extern float gAllyNear;         // 0x804D38DC
extern float gAllyPosZ;         // 0x804D38CC

// Reads the target actor's "owner" variable and returns the actor it names, if it is a
// player-type actor (handle type 0x13).
static inline Actor* AllyOwnerOf(int handle) {
    int owner;
    int r = QueryVar(handle, 0x1C1A4CC9, &owner);
    ActorInfoMsg info;
    if (r == 0 || owner == -1) {
        return NULL;
    }
    r = SendMsg(owner, 0x400, &info);
    if (r == 0 || info.actor == NULL) {
        return NULL;
    }
    if ((u32)info.actor->handle >> 24 != 0x13) {
        return NULL;
    }
    return info.actor;
}

static inline float AllyReactTime(int flag) {
    return (float)(flag == 0 ? gAllyReact0 : gAllyReact1);
}

int AllyUpdateWander(Actor* a) {
    AllyBeh* b = a->beh;
    if (a->mode == 0) {
        b->unk1C -= a->dt;
        if (b->unk1C > 0.0f) {
            ActorInfoMsg info;
            int v;
            fn_800B26B8(a);
            if (fn_800BAF40(a) == -1) {
                if (b->timer <= 0.0f) {
                    int h = fn_800B8A7C(0, 0x20F7E6, 0, (void*)AllyTargetFilter, a);
                    if (h == -1 || SendMsg(h, 0x400, &info) == 0 || info.actor == NULL) {
                        return 1;
                    }
                    fn_800BC56C(a, NULL);
                    a->anim = 3;
                    QueryVar(b->targetId, 0x2EAA5F62, &v);
                    b->timer = AllyReactTime(v != 0);
                } else {
                    float step = a->dt / a->timeScale / fn_802A2080();
                    int v;
                    QueryVar(b->targetId, 0x2EAA5F62, &v);
                    if ((float)gAllyReact1 < b->timer && v != 0) {
                        b->timer = (float)gAllyReact1;
                    }
                    b->timer = b->timer - step;
                    a->anim = 0;
                }
            }
            return 0;
        }
        return 1;
    }
    return 1;
}

int AllyReset(Actor* a) {
    AllyBeh* b = a->beh;
    b->targetId = -1;
    b->unk1C = -1.0f;
    return 1;
}

int AllyEnterFollow(Actor* a) {
    AllyBeh* b;
    Actor* target;
    int slot;
    if (a == NULL) {
        return 0;
    }
    b = a->beh;
    target = FollowGetTargetObj(a->conds);
    if (target == NULL) {
        return 0;
    }
    b->targetId = target->handle;
    b->unk1C = gAllyTime;
    b->state = 7;
    a->anim = 0;
    RangeResetPending(a->conds);
    fn_800BBCA8(a);
    ConditionUpdateBit(a->conds, 4, 1);
    ConditionUpdateBit(a->conds, 3, 0);
    slot = fn_800B26B8(a);
    if (slot == 4) {
        return 0;
    }
    fn_800B261C(a, slot, &target);
    if (target == NULL) {
        return 0;
    }
    b->timer = 0.0f;
    return 1;
}

int AllyUpdateFollow(Actor* a) {
    AllyBeh* b = a->beh;
    if (a->mode != 0) {
        return 1;
    }
    if (a->owner->handle == -1) {
        return 1;
    }
    {
        int slot = fn_800B26B8(a);
        Actor* target;
        if (slot < 4) {
            fn_800B261C(a, slot, &target);
            if (target != NULL) {
                Actor* who = AllyOwnerOf(target->handle);
                if (who != NULL) {
                    fn_800B1F20(a, slot);
                    FollowSetTargetObj(a->conds, target);
                }
            }
            if (fn_800B2454(a, slot) == 0) {
                fn_800B2418(5000.0f, a, slot);
            }
        }
        target = FollowGetTargetObj(a->conds);
        if (target == NULL) {
            if ((b->flags & 4) == 0) {
                fn_80116E30(a);
                b->unk04 = -1.0f;
                return 1;
            }
        } else {
            Actor* who = AllyOwnerOf(target->handle);
            if (who == NULL) {
                RangeResetPending(a->conds);
            } else if (OwnerValue(a, target) < gAllyNear) {
                HitMsg hit;
                int owner;
                ActorPlayAnim(a->owner->handle, 0xCF0164E2, a->handle);
                owner = a->owner->handle;
                if (owner != -1) {
                    QueryMsg q;
                    q.unk8 = 0;
                    q.hash = 0xCF0164E2;
                    *(int*)&q.result = a->handle;
                    SendMsg(owner, 0xC14, &q);
                }
                hit.handle = a->handle;
                hit.force = 15000.0f;
                hit.scale = 0.75f;
                hit.type = 0xB9FC42F5;
                hit.z1 = 0;
                hit.one = 1.0f;
                hit.zero = 0.0f;
                hit.z0 = 0;
                hit.target = target;
                hit.pos[0] = a->pos[0];
                hit.pos[1] = a->pos[1];
                hit.pos[2] = a->pos[2];
                hit.delta[0] = target->pos[0] - hit.pos[0];
                hit.delta[1] = target->pos[1] - hit.pos[1];
                hit.delta[2] = target->pos[2] - hit.pos[2];
                hit.source = a;
                SendMsg(target->handle, 0x407, &hit);
                fn_80116E30(a);
                b->unk04 = -1.0f;
                return 1;
            }
        }
    }
    return 0;
}

int AllyTrue1(Actor* a) {
    return 1;
}

int AllyEnterState6(Actor* a) {
    if (a != NULL) {
        a->beh->state = 6;
        a->anim = 0;
        RangeResetPending(a->conds);
        fn_800BBCA8(a);
        ConditionUpdateBit(a->conds, 4, 0);
        ConditionUpdateBit(a->conds, 3, 0);
    }
    return a != NULL;
}

int AllyUpdateState6(Actor* a) {
    ActorInfoMsg info;
    ActorInfoMsg other;
    Actor* target;
    int slot;
    int h;
    int v;
    if (a->mode != 0) {
        return 1;
    }
    slot = fn_800B26B8(a);
    if (fn_800BAF40(a) == -1) {
        a->anim = 0;
        if (slot != 4) {
            Actor* t;
            fn_800B261C(a, slot, &t);
            if (t != NULL) {
                Actor* who = AllyOwnerOf(t->handle);
                if (who == NULL) {
                    a->anim = 0;
                    return 1;
                }
                target = FollowGetTargetObj(who->conds);
                if (target != NULL) {
                    int r = QueryVar(target->handle, 0x7C18FE9E, &v);
                    if (r != 0 && v != -1 && SendMsg(v, 0x400, &other) != 0 && other.actor != NULL) {
                        int r2 = QueryVar(v, 0x1C1A4CC9, &v);
                        if (r2 == 0 || v == -1) {
                            FollowSetTargetObj(a->conds, other.actor);
                            fn_800BC56C(a, other.actor);
                            a->anim = 3;
                        }
                    }
                }
            }
        }
    } else {
        Actor* t;
        a->anim = 3;
        t = FollowGetTargetObj(a->conds);
        if (t != NULL && SendMsg(t->handle, 0x400, &info) != 0 && info.actor != NULL) {
            int r = QueryVar(t->handle, 0x1C1A4CC9, &v);
            if (r != 0 && v == -1) {
                float p[3];
                float dist;
                p[0] = (float)t->pos[0];
                p[1] = (float)t->pos[1];
                dist = 1000.0f;
                p[2] = (float)t->pos[2];
                ActorAimOffset2D(a, p, NULL, &dist);
                if (dist >= 500.0f) {
                    return 0;
                }
                if (fn_8029B3B0(a, 0xB3D49C39) == 0) {
                    return 0;
                }
                fn_801174B0(a, t);
                RangeResetPending(a->conds);
                return 1;
            }
        }
        fn_800BBCA8(a);
        a->anim = 0;
    }
    return 0;
}

int AllyTrue2(Actor* a) {
    return 1;
}

int AllyEnterState5(Actor* a) {
    if (a != NULL) {
        a->beh->state = 5;
        a->anim = 0;
        RangeResetPending(a->conds);
        fn_800BBCA8(a);
        ConditionUpdateBit(a->conds, 4, 1);
        ConditionUpdateBit(a->conds, 3, 0);
    }
    return a != NULL;
}

int AllyUpdateState5(Actor* a) {
    ActorInfoMsg info;
    if (a->mode == 0 && a->owner->handle != -1) {
        Actor* t;
        fn_800B26B8(a);
        t = FollowGetTargetObj(a->conds);
        if (t != NULL && SendMsg(t->handle, 0x400, &info) != 0 && info.actor != NULL) {
            SendMsg(a->owner->handle, 0x400, &info);
            if ((info.flags & 2) != 0) {
                if (fn_800BAF40(a) == -1 && FollowGetTargetObj(a->conds) != NULL) {
                    fn_800BC56C(a, NULL);
                }
                return 0;
            }
            fn_80116E30(a);
            return 1;
        }
    }
    return 1;
}

int AllyTrue3(Actor* a) {
    return 1;
}

int AllyEnterState4(Actor* a) {
    ActorInfoMsg info;
    int h;
    if (a == NULL) {
        return 0;
    }
    a->beh->state = 4;
    a->anim = 3;
    ConditionUpdateBit(a->conds, 4, 0);
    ConditionUpdateBit(a->conds, 3, 0);
    RangeResetPending(a->conds);
    fn_800BBCA8(a);
    h = fn_800B912C(0, 0x53FA489C, a->handle, 0, (void*)AllyPickFilter, a);
    if (h == -1) {
        return 0;
    }
    if (SendMsg(h, 0x400, &info) == 0 || info.actor == NULL) {
        return 0;
    }
    FollowSetTargetObj(a->conds, info.actor);
    fn_800BC56C(a, info.actor);
    return 1;
}

int AllyUpdateState4(Actor* a) {
    AllyBeh* b = a->beh;
    float step = a->dt / a->timeScale / fn_802A2080();
    if (a->mode != 0) {
        return 1;
    }
    if (fn_800B26B8(a) == 4) {
        return 1;
    }
    {
        Actor* t = FollowGetTargetObj(a->conds);
        ActorInfoMsg info;
        int v;
        if (t == NULL) {
            a->unk268 = 4;
            RangeResetPending(a->conds);
            return 1;
        }
        if (SendMsg(t->handle, 0x400, &info) == 0 || info.actor == NULL) {
            a->unk268 = 4;
            RangeResetPending(a->conds);
            return 1;
        }
        if ((info.flags & 1) != 0) {
            a->unk268 = 4;
            RangeResetPending(a->conds);
            return 1;
        }
        {
            int r = QueryVar(t->handle, 0x6C47D293, &v);
            if (r != 0 && (u32)v >> 24 == 1) {
                a->unk268 = 4;
                RangeResetPending(a->conds);
                return 1;
            }
        }
        {
            int r = QueryVar(t->handle, 0x1C1A4CC9, &v);
            if (r != 0 && v != -1) {
                a->unk268 = 4;
                RangeResetPending(a->conds);
                return 1;
            }
        }
        if ((u32)t->handle >> 24 == 0x13 && MemFlagIsSet(t->conds, 0xB) != 0) {
            a->unk268 = 4;
            RangeResetPending(a->conds);
            return 1;
        }
        {
            int moving = fn_800BAF40(a);
            if (moving != -1) {
                float c = b->unk0C + step;
                int over = c > 1000.0f;
                b->unk0C = c;
                if (!over) {
                    goto aim;
                }
            }
            fn_800BBCA8(a);
            a->anim = 0;
            if (b->timer <= 0.0f) {
                AllyStartMove(a, t);
                a->anim = 3;
                QueryVar(t->handle, 0x2EAA5F62, &v);
                b->timer = AllyReactTime(v != 0);
            } else {
                QueryVar(t->handle, 0x2EAA5F62, &v);
                if ((float)gAllyReact1 < b->timer && v != 0) {
                    b->timer = (float)gAllyReact1;
                }
                b->timer = b->timer - step;
            }
        }
    aim:
        {
            float p[3];
            float dist;
            p[0] = (float)t->pos[0];
            p[1] = (float)t->pos[1];
            dist = 1000.0f;
            p[2] = (float)t->pos[2];
            ActorAimOffset2D(a, p, NULL, &dist);
            if (dist >= 500.0f || fn_8029B3B0(a, 0x55F3E922) == 0) {
                return 0;
            }
            fn_801174B0(a, t);
            RangeResetPending(a->conds);
            SetVar(t->handle, 0x392F63E9, 0);
            fn_8001EAF8(a->pos, 20000, (void*)AllyNearCallback, t, 0);
            return 1;
        }
    }
}

int AllyTrue4(Actor* a) {
    return 1;
}

int AllyEnterState3(Actor* a) {
    AllyBeh* b;
    Actor* t;
    int slot;
    if (a == NULL) {
        return 0;
    }
    b = a->beh;
    b->unk10 = 0;
    b->state = 3;
    a->anim = 0;
    RangeResetPending(a->conds);
    fn_800BBCA8(a);
    ConditionUpdateBit(a->conds, 4, 0);
    ConditionUpdateBit(a->conds, 3, 0);
    slot = fn_800B26B8(a);
    if (slot == 4) {
        return 0;
    }
    fn_800B261C(a, slot, &t);
    if (t == NULL) {
        return 0;
    }
    FollowSetTargetObj(a->conds, t);
    b->timer = 0.0f;
    b->pos[0] = (float)t->pos[0];
    b->pos[1] = (float)t->pos[1];
    b->pos[2] = (float)t->pos[2];
    AllyStartMove(a, t);
    a->anim = 3;
    b->unk34 = gAllyPosZ;
    return 1;
}

int AllyUpdateState3(Actor* a) {
    AllyBeh* b = a->beh;
    Actor* t = FollowGetTargetObj(a->conds);
    ActorInfoMsg info;
    int v;
    b->timer -= a->dt;
    if (t == NULL || SendMsg(t->handle, 0x400, &info) == 0 || info.actor == NULL) {
        return 1;
    }
    if (QueryVar(t->handle, 0x1C1A4CC9, &v) != 0 && v != -1) {
        return 1;
    }
    if ((u32)t->handle >> 24 == 0x13 && MemFlagIsSet(t->conds, 0xB) != 0) {
        RangeResetPending(a->conds);
        a->other = NULL;
        a->mode = 0;
        return 1;
    }
    {
        float p[3];
        float dist;
        p[0] = (float)t->pos[0];
        p[1] = (float)t->pos[1];
        dist = 1000.0f;
        p[2] = (float)t->pos[2];
        ActorAimOffset2D(a, p, NULL, &dist);
        if (dist >= 500.0f || fn_801174B0(a, t) == 0) {
            if (b->timer < 0.0f) {
                AllyStartMove(a, t);
                b->timer = (float)gAllyReact0;
            }
            return 0;
        }
        RangeResetPending(a->conds);
        a->other = NULL;
        a->mode = 0;
        switch ((u32)t->handle >> 24) {
        case 2:
            SetVar(t->handle, 0x392F63E9, 0);
            fn_8001EAF8(a->pos, 20000, (void*)AllyNearCallback, t, 0);
            fn_8029B218(a, 0x55F3E922);
            break;
        case 0:
            break;
        case 1:
            fn_8029B218(a, 0xB3D49C39);
            break;
        case 0x13:
            fn_801BDC3C(*(int*)(*(int*)((char*)t + 0xC4) + 0xC), a);
            break;
        }
        return 1;
    }
}

int AllyExit(Actor* a) {
    fn_800BBCA8(a);
    a->anim = 0;
    a->mode = 0;
    return 1;
}

int AllyEnterState2(Actor* a) {
    AllyBeh* b = a->beh;
    int v;
    b->state = 2;
    a->anim = 0;
    b->unk10 = 0;
    RangeResetPending(a->conds);
    fn_800BBCA8(a);
    ConditionUpdateBit(a->conds, 4, 1);
    ConditionUpdateBit(a->conds, 3, 0);
    if (a->mode == 5 && a->other != NULL) {
        ActorInfoMsg info;
        if (SendMsg(a->other->handle, 0x400, &info) != 0) {
            if (info.actor == NULL) {
                a->mode = 0;
                return 0;
            }
            if (QueryVar(a->other->handle, 0x1C1A4CC9, &v) == 0 || v == -1) {
                FollowSetTargetObj(a->conds, a->other);
                AllyStartMove(a, a->other);
                a->anim = 3;
                b->timer = (float)gAllyReact0;
                return 1;
            }
            a->mode = 0;
            return 0;
        }
    }
    a->mode = 0;
    return 0;
}

int AllyUpdateState2(Actor* a) {
    if (a->mode == 5) {
        return 1;
    }
    if (a->mode == 0 && MemFlagIsSet(a->conds, 5) == 0) {
        return 1;
    }
    if (a->mode == 2) {
        a->update = (void*)fn_800C748C;
        if (FollowGetTargetObj(a->conds) != a->other) {
            FollowSetTargetObj(a->conds, a->other);
            if (a->unk21C != 0) {
                fn_800BC56C(a, a->other);
            }
        }
        if (fn_800BAF40(a) == -1 && a->unk21C != 0) {
            FollowSetTargetObj(a->conds, NULL);
            a->anim = 0;
        }
    }
    return 0;
}

int AllyLeave(Actor* a) {
    a->update = (void*)fn_800A70D0;
    if (a->mode == 4) {
        a->mode = 0;
    }
    if (HasCondition(a->conds, 0xD) != 0) {
        ConditionSetBit(a->conds, 0xD, 0);
    }
    return 1;
}

int AllyEnterState1(Actor* a) {
    a->beh->state = 1;
    return 1;
}

int AllyUpdateState1(Actor* a) {
    AllyBeh* b = a->beh;
    b->unk3C += a->dt / a->timeScale;
    if (a->mode == 0) {
        if (b->unk3C > 1000.0f) {
            Mtx34 m;
            Vec3 up;
            float e[3];
            Vec3 pos;
            ActorGetMatrix(&m, a);
            fn_800A025C(&up, &m, 1);
            if (!(up.x > 1.1920929e-07f) && !(up.y > 1.1920929e-07f)) {
                up.x = 0.0f;
                up.y = 0.0f;
                up.z = 0.0f;
            } else {
                float inv = 1.0f / sqrtf(up.x * up.x + up.y * up.y);
                up.x *= inv;
                up.y *= inv;
                up.z = 0.0f * inv;
            }
            fn_8016D440(e, &up);
            e[1] = e[1] + 90.0f;
            EulerNormalize((Vec3*)e, (Vec3*)e);
            fn_8016D4EC(&pos, e);
            pos.x += a->fpos[0];
            pos.y += a->fpos[1];
            pos.z += a->fpos[2];
            FollowSetTargetVec(a->conds, &pos);
            b->unk3C = 0.0f;
        }
        {
            int slot = fn_800B26B8(a);
            if (slot < 4) {
                Actor* t;
                int r = fn_800B22F8(a, slot);
                fn_800B261C(a, slot, &t);
                fn_800B1F20(a, slot);
                a->unk268 = r;
                if (fn_800B2454(a, slot) == 0) {
                    fn_800B2418(5000.0f, a, slot);
                    return 1;
                }
            }
        }
        return 0;
    }
    return 1;
}

int AllyTrue5(Actor* a) {
    return 1;
}

int AllyEnterState0(Actor* a) {
    AllyBeh* b = a->beh;
    b->state = 0;
    a->anim = 0;
    RangeResetPending(a->conds);
    fn_800BBCA8(a);
    ConditionUpdateBit(a->conds, 3, 0);
    ConditionUpdateBit(a->conds, 4, 1);
    b->unk3C = 0.0f;
    return 1;
}

// ---- message handler -------------------------------------------------------------------

struct AllyMsgInfo {
    Actor* actor;           // 0x00
    int unk04;
    u32 seed;               // 0x08
    int unk0C;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
};

inline u32 StrHash(u32 h, const char* s) {
    const char* p;
    for (p = s; *p != 0; p++) {
        h = h * 31 + (unsigned char)toupper(*p);
    }
    h = h % 0xFFFFFFFF;
    return h;
}

extern "C" {
int fn_800C5028(int handle, u32 hash);
int fn_800C5210(int handle, int msg, void* data);
int fn_80019F88(void* m);
void* fn_8002431C(float f, int* col, int a, int b, int c);
void fn_80037EC0(int model, int id, int a, int b, int c, int d, int e);
int fn_800390D8(int model, int a);
int fn_8003A1F0(int a, int b, int c);
void fn_8003F15C(float f, int a, int b, int c, void* d, int e);
int fn_8003AA9C(int model, int a, int b);
int fn_800A3A40(Actor* a);
void* GetPlayer(void);
int PlayerInHost(void* p);
}
extern int gAllyStates_placeholder;
extern float gAllyScaleA;       // 0x804D38E0
extern float gAllyScaleB;       // 0x804D38E4
extern float gAllyFade;         // 0x804D38E8
extern int gAllyFxId;           // 0x804D38EC
extern u32 gAllyVisible[0x80];  // 0x804A4750

int AllyHit(int* m);                // message 0xC14 handler (defined below)

// Draw pass (messages 0 and 0x7300): tints the model while the companion is "ghosted" and
// draws the glow bones.
static inline int AllyDraw(Actor* a, int* m) {
    AllyBeh* b = a->beh;
    u32 owner = (u32)a->owner->handle;
    if (owner == 0xFFFFFFFF || owner >> 24 == 0x13) {
        if (b->targetId == -1) {
            float fade = *(float*)((int*)b + 0xE);
            int* blk;
            int i;
            m[4] = 1;
            *((u8*)m + 0x17) = 0xFF;
            if (fade < 0.0f) {
                blk = (int*)fn_8002431C(1.0f, m + 4, 0x32, 0x32, 0x32);
            } else {
                int lvl = (int)(200.0f * (fade / gAllyFade) + 50.0f);
                blk = (int*)fn_8002431C(1.0f, m + 4, 0x32, lvl, lvl);
            }
            for (i = 0; i < 0x40; i++) {
                m[4 + i] = blk[i];
            }
            fn_80037EC0(*(int*)((char*)a + 0xCC), gAllyFxId, 1, 0x80, 0x80, 0x80,
                        (int)(64.0f * (fade / gAllyFade)));
            for (i = 0; i < 0x40; i++) {
                gAllyVisible[i] = m[4 + i];
            }
            return 0;
        }
    }
    return 1;
}

// Companion message handler.  m[0] is the actor; the other words depend on the message.
int AllyMsg(int msg, int* m) {
    Actor* a = (Actor*)m[0];
    AllyBeh* b = a->beh;
    int r;

    if (msg == 0x40A) {
        u32 h = StrHash(m[2], "QUERY");
        void* pl;
        if (ActorPlayAnim(fn_800A3A40(a), h, m[1]) != 0) {
            m[3] = 0;
            return 1;
        }
        if (HasCondition(a->conds, 10) != 0 || HasCondition(a->conds, 9) != 0 ||
            HasCondition(a->conds, 0xB) != 0) {
            return 0;
        }
        if (fn_800C5028(a->handle, 0xDC35206A) == 0) {
            return 0;
        }
        if (MemFlagIsSet(a->conds, 0xB) != 0) {
            return 1;
        }
        if (m[2] == 0x3CA34BD6 && a->owner->handle != -1) {
            m[3] = 0x1379C466;
            return 1;
        }
        pl = GetPlayer();
        if (pl == NULL) {
            return 0;
        }
        {
            int* host = *(int**)((char*)pl + 0x26E4);
            if (host != NULL && (u32)host[0x38 / 4] >> 24 == 1) {
                return 0;
            }
        }
        {
            int** slots = (int**)((char*)pl + 0x26C4);
            if (*slots[PlayerInHost(pl)] == 0x4624485 || *slots[PlayerInHost(pl)] == 0x13C05 ||
                *slots[PlayerInHost(pl)] == -0x1F5464B1 || *slots[PlayerInHost(pl)] == 0xFFF5) {
                return 0;
            }
        }
        if (MemFlagIsSet(a->conds, 0x1E) != 0) {
            return 0;
        }
        if (m[2] == 0x7C44EC3D) {
            int slot = fn_800B26B8(a);
            if (slot != 4 && fn_800B22F8(a, slot) < 2) {
                Actor* t = NULL;
                fn_800B261C(a, slot, &t);
                if (t != NULL && t->handle == m[1]) {
                    return 0;
                }
            }
        }
        {
            struct {
                int unk0;
                int who;
                int what;
                int pad;
            } rq;
            rq.unk0 = 0;
            rq.pad = -1;
            m[3] = 0x272CAC;
            rq.what = m[3];
            SendMsg(m[1], 0x40B, &rq);
        }
        return 1;
    }
    switch (msg) {
    case 10:
        if (b->targetId == -1) {
            return 1;
        }
        return 0;
    case 2:
        if (a->owner->handle != -1 && ((Actor*)m[3])->handle == a->owner->handle) {
            return 0xFFF0BDC1;
        }
        return 0;
    case 4:
        if (a->owner->handle != -1 && ((Actor*)m[7])->handle == a->owner->handle) {
            return 0xFFF0BDC1;
        }
        return 0;
    case 0:
    case 0x7300:
        return AllyDraw(a, m);
    case 0x400:
        m[1] = fn_800A3A40(a);
        m[3] = (0.0f < *(float*)((char*)a + 0x904));
        ((float*)m)[4] = 1.0f;
        ((float*)m)[5] = 1.0f;
        m[6] = (a->owner->handle == -1) ? 1 : 2;
        m[7] = 0;
        return 1;
    case 0x407:
        if (fn_800C5210(fn_800A3A40(a), 0x407, m) != 0) {
            return 1;
        }
        if (MemFlagIsSet(a->conds, 10) != 0) {
            return 1;
        }
        if ((u32)a->owner->handle == 0xFFFFFFFF) {
            if (m[5] != 0x403548D0) {
                return 1;
            }
            {
                int* info = *(int**)((char*)a + 0xC4);
                if (info[0] == -0x1EB404B) {
                    m[3] = (int)((float)info[0xC] / gAllyScaleB);
                } else {
                    m[3] = (int)((float)info[0xC] / gAllyScaleA);
                }
                *(float*)((int*)b + 0xE) = gAllyFade;
            }
        } else if ((u32)a->owner->handle >> 24 == 2) {
            return 1;
        }
        return m[2] == a->owner->handle;
    case 0x409:
        if (ActorPlayAnim(fn_800A3A40(a), m[3], m[2]) != 0) {
            return 1;
        }
        if (m[3] == 0x272CAC) {
            struct {
                int who;
                int what;
                int pad;
                u32 type;
            } ack;
            ack.who = ActorPlayAnim(a->handle, 0xDC35206A, m[2]);
            ack.what = a->handle;
            if (m[2] != -1) {
                ack.type = 0xAF699ED0;
                SendMsg(m[2], 0xC14, &ack);
            }
            return 1;
        }
        return 0;
    case 0x41E:
        switch (m[1]) {
        case 0x1C1A4CC9:
            m[3] = b->targetId;
            return 1;
        case 0x15FE7E1:
            m[3] = (b->flags >> 1) & 1;
            return 1;
        case 0x365F2878:
            m[3] = 1;
            return 1;
        }
        return 0;
    case 0x41D:
        if (fn_80019F88(m) != 0) {
            return 1;
        }
        switch (m[1]) {
        case 0x15FE7E1:
            if (m[3] == 0) {
                b->flags &= ~2;
            } else {
                b->flags |= 2;
            }
            return 1;
        case -0x5DC1949E:
            if (m[3] == 0) {
                b->flags &= ~4;
            } else {
                b->flags |= 4;
            }
            return 1;
        case 0x1C1A4CC9:
            b->targetId = m[3];
            return 1;
        }
        return 0;
    case 0xC14:
        return AllyHit(m);
    case 0x3300:
        return 1;
    case 0x7102:
        if (b == NULL) {
            return 1;
        }
        if (m[1] < 0x44) {
            m[4] = 0;
            return 1;
        } else {
            u32 tag = StrHash(0, "[FGDSVE]");
            memcpy((void*)m[2], &tag, 4);
            m[3] += 4;
            m[1] -= 4;
            m[2] += 4;
            memcpy((void*)m[2], b, 0x40);
            m[3] += 0x40;
            m[1] -= 0x40;
            m[2] += 0x40;
            return 1;
        }
    case 0x7103:
        if (b == NULL) {
            return 1;
        }
        if (m[3] < 0x44) {
            m[4] = 0;
            return 1;
        } else {
            u32* p = (u32*)m[2];
            int ok;
            if (m[3] < 4) {
                ok = 0;
            } else {
                m[3] += 4;
                m[1] -= 4;
                m[2] += 4;
                ok = (*p == StrHash(0, "[FGDSVE]"));
            }
            if (ok) {
                memcpy(b, (void*)m[2], 0x40);
                m[3] -= 0x40;
                m[2] += 0x40;
            }
            return 1;
        }
    }
    return 0;
}

// Per-frame update of the companion.
int AllyUpdate(Actor* a) {
    AllyBeh* b;
    int canGo;
    float t0;
    float scale;

    if (a == NULL) {
        return 0;
    }
    b = a->beh;
    fn_800C99E8(25.0f, a, 0x4B, 0x96, 2, 2);
    if ((b->flags & 1) == 0 && b->unk24 == 0 && HasCondition(a->conds, 9) == 0 &&
        HasCondition(a->conds, 10) == 0) {
        canGo = 1;
    } else {
        canGo = 0;
    }
    if (canGo) {
        AllyBeh* bb = a->beh;
        if ((bb->flags & 1) == 0 && 1.0f == bb->unk1C) {
            bb->flags |= 1;
        }
    }
    if (0.0f <= *(float*)((int*)b + 0xE)) {
        *(float*)((int*)b + 0xE) -= a->dt;
    }
    if (a->owner->handle == -1 && b->targetId == -1) {
        float step = a->dt / a->timeScale;
        float k = step / fn_802A2080();
        float cur = fn_802A2080();
        float norm;
        if (1.0f <= cur) {
            norm = (a->timeScale - 1.0f) / (15.0f - 1.0f);
        } else {
            norm = (a->timeScale - 1.0f) / (50.0f - 1.0f);
        }
        if ((b->state & 1) == 0) {
            if (*(float*)((int*)b + 9) < 1.0f) {
                float v = *(float*)((int*)b + 9) + a->dt / 200.0f;
                if (v >= 1.0f) {
                    v = 1.0f;
                }
                *(float*)((int*)b + 9) = v;
            }
            a->dt = a->dt * fn_802A2080();
            norm = 0.0f;
        } else if (norm < 1.0f) {
            float v = norm + k / 1500.0f;
            norm = (v < 1.0f) ? v : 1.0f;
        }
        if (norm < 0.0f) {
            norm = 0.0f;
        } else if (norm > 1.0f) {
            norm = 1.0f;
        }
        if (1.0f <= fn_802A2080()) {
            a->timeScale = norm * (15.0f - 1.0f) + 1.0f;
        } else {
            a->timeScale = norm * (50.0f - 1.0f) + 1.0f;
        }
    } else {
        *(float*)((int*)b + 9) = 0.0f;
        a->timeScale = 1.0f;
    }
    if (HasCondition(a->conds, 10) == 0 && HasCondition(a->conds, 9) == 0) {
        u32 owner;
        if (b->state == -1) {
            b->state = AllyPickState(a);
            gAllyStates[b->state].enter(a);
        }
        owner = (u32)a->owner->handle;
        if (owner == 0xFFFFFFFF) {
            if (b->targetId != -1) {
                SendMsg(b->targetId, 0x400, NULL);
                if ((u32)b->targetId >> 24 == 2) {
                    /* mirror the target's position and basis */
                }
            }
        } else if (owner >> 24 == 2) {
            /* follow the owner's position, lifted by 200 */
        }
        if (a->owner->handle == -1 && b->unk04 > 0.0f) {
            b->unk04 = b->unk04 - a->dt;
        }
        canGo = gAllyStates[b->state].update(a);
        if (*(int*)((char*)a + 0x128) == -0x508DA753) {
            fn_80162658(a, (int*)a + 0x225);
            if (canGo != 0) {
                int next = AllyPickState(a);
                if (gAllyStates[b->state].canLeave(a) != 0) {
                    int old = b->state;
                    if (gAllyStates[next].enter(a) == 0) {
                        gAllyStates[old].enter(a);
                    }
                    ConditionSetBit(a->conds, 0x40, 0);
                    ConditionSetBit(a->conds, 0x41, 0);
                    ConditionSetBit(a->conds, 0x43, 0);
                    ConditionSetBit(a->conds, 0x42, 0);
                }
            }
            if (a->owner->handle == -1) {
                int i;
                for (i = 0; i < ActorCount(); i++) {
                    Actor* o = (Actor*)ActorGet(i);
                    if (PlayerInHost(o) != 0) {
                        float dx = a->fpos[0] - (float)o->pos[0];
                        float dy = a->fpos[1] - (float)o->pos[1];
                        float dz = a->fpos[2] - (float)o->pos[2];
                        float d2 = dz * dz + dx * dx + dy * dy;
                        if (d2 < 6553600.0f) {
                            float d = 0.0f;
                            if (0.0f < d2) {
                                d = 1.0f / (1.0f / sqrtf(d2));
                            }
                            if (o->handle != -1) {
                                struct {
                                    int unk;
                                    int src;
                                    u32 type;
                                    int amount;
                                } hit;
                                hit.amount = (int)(((2560.0f - d) / 2560.0f) * 20.0f);
                                hit.type = 0x23D64AF0;
                                hit.src = *(int*)((char*)a + 0x38);
                                SendMsg(o->handle, 0xC14, &hit);
                            }
                        }
                    }
                }
            }
            return 1;
        }
        return 1;
    }
    fn_80162658(a, (int*)a + 0x225);
    return 1;
}

void AllyDestroy(Actor* a) {
    AllyBeh* b;
    *(int*)((char*)a + 0x104) = 0;
    b = a->beh;
    fn_800C9B68(a);
    VFree(b);
    a->beh = NULL;
}

void AllyCreate(Actor* a) {
    AllyBeh* b = (AllyBeh*)VAlloc(0x40);
    if (b == NULL) {
        a->beh = NULL;
        return;
    }
    a->beh = b;
    *(void**)((char*)a + 0xFC) = (void*)fn_800CE538;
    *(void**)((char*)a + 0x604) = (void*)fn_800CEEC8;
    *(void**)((char*)a + 0x5FC) = (void*)fn_800CC984;
    *(void**)((char*)a + 0x600) = (void*)fn_800CC984;
    *(int*)((char*)a + 0x8F8) = 1;
    b->state = -1;
    b->unk04 = 0.0f;
    b->flags = 0;
    *(float*)((int*)b + 9) = 1.0f;
    b->unk10 = 0;
    b->targetId = -1;
    *(float*)((int*)b + 0xE) = -1.0f;
    gAllyFxId = AssetFind(0xD9044CBB, 0xE9042AED);
    if (fn_80117D80(a) == 0) {
        VFree(b);
        a->beh = NULL;
        return;
    }
    ConditionUpdateBit(a->conds, 6, 0);
    ConditionUpdateBit(a->conds, 0x1C, 1);
    ConditionUpdateBit(a->conds, 0x18, 1);
    fn_800B6CAC(a, (void*)fn_800B5028);
    fn_800C9C68(a);
    *(void**)((char*)a + 0x104) = (void*)AllyUpdate;
}

// Picks which of two candidate targets (ids) the companion prefers: the nearer one, unless
// the other is ahead of the one the companion is currently moving toward.
int AllyTargetFilter(int first, int second, Actor* a) {
    AllyBeh* b = a->beh;
    int best = second;
    float limit = 0.17f;
    ActorInfoMsg info;
    if (first != -1 && SendMsg(first, 0x400, &info) != 0 && info.actor != NULL) {
        Vec3 d1;
        float dist1;
        d1.x = (float)info.actor->pos[0] - a->fpos[0];
        d1.y = (float)info.actor->pos[1] - a->fpos[1];
        d1.z = (float)info.actor->pos[2] - a->fpos[2];
        dist1 = d1.z * d1.z + d1.x * d1.x + d1.y * d1.y;
        best = second;
        if (dist1 >= 250000.0f && second != -1 && SendMsg(second, 0x400, &info) != 0 &&
            info.actor != NULL) {
            Vec3 d2;
            float dist2;
            d2.x = (float)info.actor->pos[0] - a->fpos[0];
            d2.y = (float)info.actor->pos[1] - a->fpos[1];
            d2.z = (float)info.actor->pos[2] - a->fpos[2];
            dist2 = d2.z * d2.z + d2.x * d2.x + d2.y * d2.y;
            best = first;
            if (SendMsg(b->targetId, 0x400, &info) != 0 && info.actor != NULL) {
                Vec3 d3;
                Vec3 dirA;
                Vec3 dirB;
                float da;
                float db;
                d3.x = (float)info.actor->pos[0] - a->fpos[0];
                d3.y = (float)info.actor->pos[1] - a->fpos[1];
                d3.z = (float)info.actor->pos[2] - a->fpos[2];
                dirA = d1;
                dirB = d2;
                da = d3.x * dirA.x + d3.y * dirA.y + d3.z * dirA.z;
                db = d3.x * dirB.x + d3.y * dirB.y + d3.z * dirB.z;
                if (limit <= db || limit <= da) {
                    if (da < db) {
                        best = second;
                    }
                } else if (dist1 < dist2) {
                    best = second;
                }
            }
        }
    }
    return best;
}

// Message 0xC14 (hit / command from another actor).
int AllyHit(int* m) {
    int cmd = m[3];
    Actor* a = (Actor*)m[0];
    AllyBeh* b = a->beh;
    if (cmd == -0x33A6E905) {
        ActorInfoMsg info;
        if (SendMsg(m[2], 0x400, &info) == 0 || info.actor == NULL) {
            return 0;
        }
        RangeResetPending(a->conds);
        {
            u32 id = (u32)info.actor->handle;
            switch (id >> 24) {
            case 2:
                SetVar(id, 0x392F63E9, 0);
                fn_8001EAF8(a->pos, 20000, (void*)AllyNearCallback, info.actor, 0);
                fn_8029B218(a, 0x55F3E922);
                break;
            case 0:
                break;
            case 1:
                fn_8029B218(a, 0xB3D49C39);
                break;
            case 0x13:
                fn_801BDC3C(*(int*)(*(int*)((char*)info.actor + 0xC4) + 0xC), a);
                break;
            }
        }
        return 1;
    }
    if (cmd == -0x5798CC65) {
        b->unk04 = 10000.0f;
        if ((a->beh->flags & 1) != 0) {
            a->beh->flags &= ~1;
        }
        *(float*)((int*)b + 9) = 0.0f;
        fn_8029B070(a, 0x55F3E922);
        fn_8029B070(a, 0xB3D49C39);
        *(void**)((char*)a + 0x604) = (void*)fn_800CEEC8;
        return 1;
    }
    if (cmd == 0x771E036F) {
        ActorInfoMsg info;
        fn_80116E30(a);
        *(void**)((char*)a + 0xFC) = (void*)fn_800CE538;
        if ((u32)m[2] >> 24 == 2 && SendMsg(m[2], 0x400, &info) != 0 && info.actor != NULL) {
            FollowSetTargetObj(a->conds, info.actor);
            gAllyStates[b->state].canLeave(a);
            gAllyStates[7].enter(a);
        }
        return 1;
    }
    return 0;
}

// Picks the state to switch to from the current situation.
int AllyPickState(Actor* a) {
    AllyBeh* b;
    int slot;
    if (a == NULL) {
        return 0;
    }
    b = a->beh;
    slot = fn_800B26B8(a);
    if (a->mode == 0) {
        u32 owner = (u32)a->owner->handle;
        if (owner == 0xFFFFFFFF) {
            if (slot != 4) {
                Actor* t;
                ActorInfoMsg info;
                int v;
                int w;
                memset(&info, 0, 0x20);
                fn_800B261C(a, slot, &t);
                if (t == NULL) {
                    return 0;
                }
                SendMsg(t->handle, 0x400, &info);
                QueryVar(t->handle, 0x1C1A4CC9, &v);
                if ((info.flags & 2) != 0) {
                    int r = QueryVar(t->handle, 0x6C47D293, &w);
                    if (r != 0 && (u32)w >> 24 != 1) {
                        if (v == -1 && b->unk04 <= 0.0f) {
                            return 3;
                        }
                        if (b->unk04 <= 0.0f && (b->state != 5 || FollowGetTargetObj(a->conds) == NULL)) {
                            return 5;
                        }
                        FollowSetTargetObj(a->conds, t);
                        return 7;
                    }
                }
            }
        } else {
            u32 kind = owner >> 24;
            if (kind == 2) {
                return 4;
            }
            if (kind == 1) {
                return 6;
            }
            if (kind == 0x13) {
                return 6;
            }
        }
        return 0;
    }
    return (a->mode == 5) + 1;
}

int AllyPickFilter(int id, Actor* self) {
    ActorInfoMsg info;
    if (SendMsg(id, 0x400, &info) == 0 || info.actor == NULL) {
        return 0;
    }
    return !(OwnerValue(self) < 4000.0f);
}

int AllyNearCallback(Actor* a, Actor* b) {
    if (a != NULL && b != NULL && (u32)a->handle >> 24 == 0x13 &&
        **(int**)((char*)a + 0xC8) == -0x508DA753) {
        Actor* owner = AllyOwnerOf(a->handle);
        if (owner == NULL) {
            fn_800B20B8(a, b->handle);
            *(float*)((char*)a + 0x174) = 100000.0f;
        }
    }
    return 1;
}

// Starts the companion walking toward `target`: for a player-type target it probes a few
// jittered points around the last known position ("Found Player" / "Can't Find Player"),
// otherwise it simply heads for the target.
int AllyStartMove(Actor* a, Actor* target) {
    AllyBeh* b = a->beh;
    b->unk0C = 0.0f;
    if ((u32)target->handle >> 24 == 2) {
        Vec3 from;
        float angle;
        fn_800BB8B8(a, &from);
        angle = ActorAimAngleToPos(target, &from);
        if (fn_800BAF40(a) == -1) {
            if ((angle < 0.0f ? -angle : angle) <= 40.0f) {
                float dist = OwnerValue(a, target);
                if (dist >= 1500.0f) {
                    char buf[32];
                    Vec3 dir;
                    Vec3 pt;
                    int dummy0, dummy1, dummy2;
                    int side;
                    if (b->unk34 * 0.01f <= (float)rand() / 32767.0f) {
                        sprintf(buf, "Can't Find Player (%d%%)", (int)b->unk34);
                        fn_800C51AC(buf);
                        b->unk34 = b->unk34 + 30.0f;
                    } else {
                        sprintf(buf, "Found Player (%d%%)", (int)b->unk34);
                        fn_800C51AC(buf);
                        b->pos[0] = (float)target->pos[0];
                        b->pos[1] = (float)target->pos[1];
                        b->pos[2] = (float)target->pos[2];
                        b->unk34 = gAllyPosZ;
                    }
                    dir.x = b->pos[0] - a->fpos[0];
                    dir.y = b->pos[1] - a->fpos[1];
                    dir.z = b->pos[2] - a->fpos[2];
                    fn_800E1020(&dir);
                    side = ((float)rand() / 32767.0f >= 0.5f) ? -1 : 1;
                    {
                        float jitter = 500.0f + (2000.0f - 500.0f) * ((float)rand() / 32767.0f);
                        float yaw[3];
                        fn_8016D440(yaw, &dir);
                        yaw[1] = (float)side * (50.0f - 10.0f) * ((float)rand() / 32767.0f) + yaw[1];
                        fn_8016D4EC(&pt, yaw);
                        fn_800E1098(jitter, &pt, &pt);
                        pt.x += a->fpos[0];
                        pt.y += a->fpos[1];
                        pt.z += a->fpos[2];
                        if (fn_800BB218(*(float*)((char*)a + 0x900), a, *(int*)((char*)a + 0x2C4),
                                        a->fpos, &pt, a, &dummy0, &dummy1, &dummy2) != 0) {
                            fn_8016D440(yaw, &dir);
                            yaw[1] = -1.0f * (float)side * (50.0f - 10.0f) *
                                         ((float)rand() / 32767.0f) + yaw[1];
                            fn_8016D4EC(&pt, yaw);
                            fn_800E1098(jitter, &pt, &pt);
                            pt.x += a->fpos[0];
                            pt.y += a->fpos[1];
                            pt.z += a->fpos[2];
                            if (fn_800BB218(*(float*)((char*)a + 0x900), a,
                                            *(int*)((char*)a + 0x2C4), a->fpos, &pt, a, &dummy0,
                                            &dummy1, &dummy2) != 0) {
                                fn_800E1098(jitter, &pt, &dir);
                                pt.x += a->fpos[0];
                                pt.y += a->fpos[1];
                                pt.z += a->fpos[2];
                                if (fn_800BB218(*(float*)((char*)a + 0x900), a,
                                                *(int*)((char*)a + 0x2C4), a->fpos, &pt, a,
                                                &dummy0, &dummy1, &dummy2) != 0) {
                                    pt.x = (float)target->pos[0];
                                    pt.y = (float)target->pos[1];
                                    pt.z = (float)target->pos[2];
                                }
                            }
                        }
                    }
                    fn_800BC5A8(a, &pt);
                    a->anim = 3;
                } else {
                    fn_800BC56C(a, target);
                    a->anim = 3;
                }
            } else {
                Vec3 side;
                Mtx34 m;
                Vec3 up;
                float k = 45.0f;
                float yaw[3];
                ActorGetMatrix(&m, target);
                fn_800A025C(&up, &m, 1);
                fn_8016D440(yaw, &up);
                EulerNormalize((Vec3*)yaw, (Vec3*)yaw);
                if (b->unk10 == 0) {
                    if (0.0f < angle) {
                        k = k * -1.0f;
                    }
                } else {
                    k = k * (float)b->unk10;
                }
                yaw[2] = yaw[2] + (k + angle);
                fn_8016D4EC(&side, yaw);
                fn_800E1098(1024.0f, &side, &side);
                side.x += m.m[3];
                side.y += m.m[7];
                side.z += m.m[11];
                if (fn_800BF2D0(&side) == -1) {
                    if (b->unk10 != 0) {
                        fn_800BC56C(a, target);
                        return 1;
                    }
                    k = k * -2.0f;
                    yaw[2] = yaw[2] + k;
                    fn_8016D4EC(&side, yaw);
                    fn_800E1098(1024.0f, &side, &side);
                    side.x += m.m[3];
                    side.y += m.m[7];
                    side.z += m.m[11];
                    if (fn_800BF2D0(&side) == -1) {
                        fn_800BC56C(a, target);
                        return 1;
                    }
                }
                if (0.0f <= k) {
                    b->unk10 = 1;
                } else {
                    b->unk10 = -1;
                }
                fn_800BC5A8(a, &side);
            }
        }
    } else {
        fn_800BC56C(a, NULL);
    }
    return 1;
}


AllyState gAllyStates[8] = {
    { AllyEnterState0, AllyTrue5, AllyUpdateState1, "" },
    { AllyEnterState1, AllyLeave, AllyUpdateState2, "" },
    { AllyEnterState2, AllyExit, AllyUpdateState3, "" },
    { AllyEnterState3, AllyTrue4, AllyUpdateState4, "" },
    { AllyEnterState4, AllyTrue3, AllyUpdateState5, "" },
    { AllyEnterState5, AllyTrue2, AllyUpdateState6, "" },
    { AllyEnterState6, AllyTrue1, AllyUpdateFollow, "" },
    { AllyEnterFollow, AllyReset, AllyUpdateWander, "" },
};


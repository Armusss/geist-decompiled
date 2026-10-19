// u_8022ED88.cpp: the "Ghost Bee" / "Ghost Bee Swarm" spin actors (0x8022ED88-0x802301E0; merged with the former
// u_8022EEEC: one string pool and two descriptors at 0x803BD988/0x803BD9B8).
// It spins its model and emits three motion trails (the "SPIN" effect). The swarm actor keeps a list of
// bee actors, steers them around its goal and toggles them on and off with the host.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>

typedef unsigned long u32;
typedef unsigned char u8;

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

// Growable array (only the fields the swarm touches).
struct Vector {
    void* vtable;   // 0x00
    int unk4;       // 0x04
    unsigned size;  // 0x08
    char* data;     // 0x0C
    int capacity;   // 0x10
};

struct ActorInfo {
    char pad0[0x80];
    int f80;
    int f84;
    int f88;
    char pad8C[0x18];
    int fA4;
};

struct Actor;

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

struct ActorType {
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void (*update)(Actor* actor, float dt);
    int (*init)(Actor* actor);
    void* unk14;
    void (*destroy)(Actor* actor);
    void* unk1C[5];
};

struct Actor {
    char pad0[0x24];
    int x;             // 0x24
    int y;             // 0x28
    int z;             // 0x2C
    char pad30[0x8];
    int handle;        // 0x38
    char pad3C[0x4];
    void* f40;         // 0x40
    char pad44[0x8];
    u32 flags;         // 0x4C
    char pad50[0x34];
    ActorInfo* info;   // 0x84
    char pad88[0x1C];
    void* data;        // 0xA4
    char padA8[0x4];
    u32 flags2;        // 0xAC
    char padB0[0x60];
    int target;        // 0x110
    char pad114[0x4];
    float f118;        // 0x118
};

struct PlayerState {
    char pad0[0x38];
    u32 flags;
};

struct Player {
    char pad0[0x26E4];
    PlayerState* state;
};

// Per-bee state (0x28 bytes).
struct SpinData {
    int trail[3];  // 0x00
    float speed;   // 0x0C
    Vec3 vel;      // 0x10
    Vec3 dir;      // 0x1C
};

// Per-swarm state (0x64 bytes).
struct SwarmData {
    int unk0;           // 0x00
    u32 flags;          // 0x04
    Vector children;    // 0x08
    union {
        int target;     // 0x1C
        Vec3 point;     // 0x1C
    } goal;
    float dist;         // 0x28
    float minDist;      // 0x2C
    float maxDist;      // 0x30
    float unk34;        // 0x34
    float forceScale;   // 0x38
    float targetSpeed;  // 0x3C
    float blendInner;   // 0x40
    float blendOuter;   // 0x44
    float blendNear;    // 0x48
    float blendFar;     // 0x4C
    float weightForward;// 0x50
    float radius;       // 0x54
    float weightCenter; // 0x58
    float weightAlign;  // 0x5C
    float alignBias;    // 0x60
};

struct AnimMsg {
    int unk0;          // 0x00
    int asset;         // 0x04
    const char* anim;  // 0x08
    char pad0C[0x4];
    int time;          // 0x10
    char pad14[0x1C];
};

struct CountMsg {
    int unk0;   // 0x00
    int asset;  // 0x04
    int count;  // 0x08
    char pad0C[0x4];
};

struct HideMsg {
    int unk0;   // 0x00
    int hide;   // 0x04
};

struct TrailDesc {
    int unk0;            // 0x00
    int owner;           // 0x04
    int unk8;            // 0x08
    int segments;        // 0x0C
    float width;         // 0x10
    float unk14;         // 0x14
    char pad18[8];       // 0x18
    float unk20;         // 0x20
    float unk24;         // 0x24
    float unk28;         // 0x28
    float unk2C;         // 0x2C
    int texture;         // 0x30
    unsigned char color[3]; // 0x34
    char pad37[9];       // 0x37
};

struct SpawnDesc {
    Vec3 pos;         // 0x00
    char pad0C[0x10];
    int f1C;          // 0x1C
    u32 hash;         // 0x20
    char pad24[0x14];
};

extern "C" {
void fn_8016D4EC(Vec3* out, Vec3* euler);              // 0x8016D4EC
void fn_800E1098(Vec3* out, Vec3* in, float scale);    // 0x800E1098
float fn_800E1020(Vec3* v);                            // 0x800E1020
void fn_8016D864(float* out, void* in);                // 0x8016D864
int fn_8008EA9C(int a, int b);                         // 0x8008EA9C
Actor* fn_800C7E80(void* desc);                        // 0x800C7E80
void* fn_8028B3BC(Vector* vec, char* end, int size);   // 0x8028B3BC
void* memset(void* dst, int val, unsigned long n);     // 0x800050B4
int rand(void);                                        // 0x802F2CFC
}

void TrailDestroy(int trail);                                      // 0x801DE904
void VFree(void* p);                                               // 0x8009DA3C
void* VAlloc(int size);                                            // 0x8009DA68
void Vec3Set(Vec3* v, float x, float y, float z);                  // 0x8016BBD0
int AssetFind(u32 table, u32 key);                                 // 0x8007EB34
int TrailCreate(TrailDesc* desc);                                  // 0x801DEA28
void ActorGetMatrix(Mtx34* mtx, Actor* a);                         // 0x800F0D6C
void ActorRender(Actor* a);                                        // 0x800E0A90
int ActorFn80022FD8(Actor* a);                                     // 0x8022FD8
void ActorFn800231A8(Actor* a);                                    // 0x80231A8
Actor* GetPlayer(void);                                            // 0x80049948
int PlayerInHost(Actor* player);                                   // 0x80049750
int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg);   // 0x800E4C64
void ActorUpdate(Actor* a, float dt);                              // 0x800E48A0
int ActorInitBase(Actor* a);                                       // 0x800E6444
extern "C" int SendMsg(int to, int msg, void* data);               // 0x800265D4

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN", "",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    int i;
    u32 h = 0;

    if (s == 0) {
        return 0;
    }
    for (i = 0; s[i] != 0; i++) {
        h *= 31;
        h += (unsigned char)toupper(s[i]);
    }
    return h % 0xFFFFFFFF;
}

void SpinDestroy(Actor* a);
int SpinInit(Actor* a);
int SpinMsg(Actor* a, int msg, void* data, int arg);
void SwarmUpdate(Actor* a, float dt);
void SwarmDestroy(Actor* a);
int SwarmInit(Actor* a);
int SpinSetup(Actor* a);

ActorType gGhostBeeType = { 0, "Ghost Bee", ActorDefaultMsg, ActorUpdate, SpinInit, 0, SpinDestroy };
ActorType gGhostBeeSwarmType = { 0, "Ghost Bee Swarm", SpinMsg, SwarmUpdate, SwarmInit, 0, SwarmDestroy };

void SpinDestroy(Actor* a) {
    int* t = (int*)a->data;
    if (t) {
        TrailDestroy(t[0]);
        TrailDestroy(t[1]);
        TrailDestroy(t[2]);
        VFree(a->data);
        a->data = 0;
    }
}

int SpinInit(Actor* a) {
    if (!ActorInitBase(a)) {
        return 0;
    }
    SpinSetup(a);
    return 1;
}

static inline int SpinCheckKey(void* data) {
    switch (((int*)data)[1]) {
    case 0x530BE0C7:
        return 1;
    }
    return 0;
}

int SpinMsg(Actor* a, int msg, void* data, int arg) {
    int ret = 0;
    switch (msg) {
    case 0x41E:
        ret = SpinCheckKey(data);
        break;
    case 0x41D:
        ret = SpinCheckKey(data);
        break;
    }
    if (ret == 0) return ActorDefaultMsg(a, msg, data, arg);
    return ret == -999999 ? 0 : ret;
}

// Number of components whose magnitude is above the float epsilon.
inline int VecNonZero(Vec3* v) {
    return (fabs(v->x) > 1.1920929e-7f) + (fabs(v->y) > 1.1920929e-7f) + (fabs(v->z) > 1.1920929e-7f);
}

// Normalizes the vector in place; returns the (approximate) length.
inline float VecNormalize(Vec3* v) {
    double x, y, z, len, inv;

    if (VecNonZero(v) == 0) {
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 0.0f;
        return 0.0f;
    }
    x = (double)v->x;
    y = (double)v->y;
    z = (double)v->z;
    {
        double xx = x * x;
        double yy = y * y;
        double zz = z * z;

        len = xx + yy + zz;
    }
    inv = __frsqrte(len);
    v->x = (float)(x * inv);
    v->y = (float)(y * inv);
    v->z = (float)(z * inv);
    return __fres(len);
}

void SwarmUpdate(Actor* a, float dt) {
    SwarmData* d = (SwarmData*)a->data;
    Actor** p;
    Actor* child;
    SpinData* cd;
    Vec3 v[3];
    Vec3 ndelta;
    Vec3 cdir;
    Vec3 euler;
    Vec3 force;
    float weights[3];
    float ax, ay, az;
    float px, py, pz;
    float cx, cy, cz;
    float dx, dy, dz;
    float blend;
    float step, accel;
    float sumX, sumY, sumZ;
    float inv, avgX, avgY, avgZ;
    float dist2;
    float t;
    int i;

    if (!d) {
        return;
    }
    if ((d->children.size >> 2) == 0) {
        return;
    }
    a->flags2 |= 0x800000;
    ActorUpdate(a, dt);

    if ((float)rand() / 32767.0f < 0.005f) {
        int n = rand() % (d->children.size >> 2);
        char* q = d->children.data;
        char* end = d->children.data + d->children.size;

        while (q != end) {
            if (n == 0) break;
            n--;
            q += 4;
        }
    }

    t = (d->dist - d->blendInner * d->blendInner) / (d->blendOuter * d->blendOuter - d->blendInner * d->blendInner);
    if (t < 0.0f) {
        t = 0.0f;
    } else if (t > 1.0f) {
        t = 1.0f;
    }
    blend = d->blendNear * (1.0f - t) + d->blendFar * t;

    d->dist = 0.0f;
    d->minDist = 3.4028235e38f;
    d->maxDist = 0.0f;

    ax = (float)a->x;
    ay = (float)a->y;
    az = (float)a->z;

    if (d->flags & 4) {
        px = d->goal.point.x;
        py = d->goal.point.y;
        pz = d->goal.point.z;
    } else if (d->flags & 8) {
        Actor* target;

        if (SendMsg(d->goal.target, 0x400, &target)) {
            px = (float)target->x;
            py = (float)target->y;
            pz = (float)target->z;
        }
    } else {
        px = ax;
        py = ay;
        pz = az;
    }

    dx = ax - px;
    dy = ay - py;
    dz = az - pz;
    step = 0.001f * dt;
    accel = 0.2f * dt;
    sumX = 0.0f;
    sumY = 0.0f;
    sumZ = 0.0f;

    for (p = (Actor**)d->children.data; p != (Actor**)(d->children.data + d->children.size); p++) {
        child = *p;
        cd = (SpinData*)child->data;
        cx = (float)child->x;
        cy = (float)child->y;
        cz = (float)child->z;

        weights[0] = blend * d->weightForward;
        v[0].x = cd->dir.x;
        v[0].y = cd->dir.y;
        v[0].z = cd->dir.z;
        v[1].x = ax - cx;
        v[1].y = ay - cy;
        v[1].z = az - cz;
        weights[1] = blend * d->weightCenter *
                     ((v[1].x * v[1].x + v[1].y * v[1].y + v[1].z * v[1].z) / (d->radius * d->radius));

        ndelta.x = dx;
        ndelta.y = dy;
        ndelta.z = dz;
        cdir.x = cx - px;
        cdir.y = cy - py;
        cdir.z = cz - pz;
        VecNormalize(&ndelta);
        VecNormalize(&cdir);

        if (VecNonZero(&ndelta) != 0) {
            weights[2] = d->weightAlign * ((ndelta.x * cdir.x + ndelta.y * cdir.y + ndelta.z * cdir.z - d->alignBias) / d->alignBias);
        } else {
            weights[2] = d->weightAlign;
        }

        if (weights[2] > 0.0f) {
            float r = (float)rand() / 32767.0f;

            Vec3Set(&euler, 360.0f * ((float)rand() / 32767.0f), 90.0f * r - 45.0f, 0.0f);
            fn_8016D4EC(&v[2], &euler);
        }

        if (cd->speed > d->targetSpeed) {
            weights[0] *= 1.5f;
            weights[1] *= 0.4f;
            weights[2] *= 0.4f;
        }

        force.x = 0.0f;
        force.y = 0.0f;
        force.z = 0.0f;
        for (i = 0; i < 3; i++) {
            if (weights[i] > 0.0f && VecNonZero(&v[i]) != 0) {
                fn_800E1098(&v[i], &v[i], weights[i]);
                force.x += v[i].x;
                force.y += v[i].y;
                force.z += v[i].z;
            }
        }

        if (VecNonZero(&force) != 0) {
            fn_800E1098(&force, &force, d->forceScale * dt * 0.001f);
        }
        cd->vel.x += force.x;
        cd->vel.y += force.y;
        cd->vel.z += force.z;
        if (fn_800E1020(&cd->vel) > cd->speed) {
            fn_800E1098(&cd->vel, &cd->vel, cd->speed);
        }

        child->x = (int)(cx + cd->vel.x * step);
        child->y = (int)(cy + cd->vel.y * step);
        child->z = (int)(cz + cd->vel.z * step);

        if (cd->speed > d->targetSpeed) {
            cd->speed -= accel;
            if (cd->speed < d->targetSpeed) {
                cd->speed = d->targetSpeed;
            }
        } else if (cd->speed < d->targetSpeed) {
            cd->speed += accel;
            if (cd->speed > d->targetSpeed) {
                cd->speed = d->targetSpeed;
            }
        }

        dist2 = cdir.x * cdir.x + cdir.y * cdir.y + cdir.z * cdir.z;
        if (d->minDist > dist2) {
            d->minDist = dist2;
        }
        if (d->maxDist < dist2) {
            d->maxDist = dist2;
        }
        sumX += cx;
        sumY += cy;
        sumZ += cz;
        d->dist += dist2;

        if (d->flags & 8) {
            Actor* target;

            if (SendMsg(d->goal.target, 0x400, &target) && target) {
                float buf[6];
                Mtx34 mtx;

                fn_8016D864(buf, target->f40);
                ActorGetMatrix(&mtx, target);
            }
        }

        child->f118 = (float)child->z;
        child->flags2 |= 0x800000;
        if (!ActorFn80022FD8(child)) {
            ActorFn800231A8(child);
        }
    }

    d->dist /= (float)(d->children.size >> 2);
    inv = 1.0f / (float)(d->children.size >> 2);
    avgX = sumX * inv;
    avgY = sumY * inv;
    avgZ = sumZ * inv;

    if (!(a->flags & 1) && (a->target == -1 || fn_8008EA9C(a->target, 0) != 1)) {
        a->x = (int)avgX;
        a->y = (int)avgY;
        a->z = (int)avgZ;
    }

    {
        Actor* player = GetPlayer();

        if (!PlayerInHost(player) || (((Player*)player)->state->flags >> 24) == 1) {
            SwarmData* w = (SwarmData*)a->data;

            if (w->flags & 2) {
                if (w->flags & 1) {
                    HideMsg m;
                    char* q = w->children.data;

                    m.hide = 1;
                    while (q != w->children.data + w->children.size) {
                        SendMsg((*(Actor**)q)->handle, 0xC00, &m);
                        q += 4;
                    }
                }
                w->flags &= ~2;
            }
        } else {
            SwarmData* w = (SwarmData*)a->data;

            if (!(w->flags & 2)) {
                if (w->flags & 1) {
                    HideMsg m;
                    char* q = w->children.data;

                    m.hide = 0;
                    while (q != w->children.data + w->children.size) {
                        SendMsg((*(Actor**)q)->handle, 0xC00, &m);
                        q += 4;
                    }
                }
                w->flags |= 2;
            }
        }
    }
}

void SwarmDestroy(Actor* a) {
    Actor* child;
    SwarmData* d = (SwarmData*)a->data;
    SpinData* cd;

    if (d) {
        if ((d->children.size >> 2) != 0) {
            while ((d->children.size >> 2) != 0) {
                child = *(Actor**)d->children.data;
                cd = (SpinData*)child->data;

                if (cd) {
                    TrailDestroy(cd->trail[0]);
                    TrailDestroy(cd->trail[1]);
                    TrailDestroy(cd->trail[2]);
                    VFree(child->data);
                    child->data = 0;
                }
                ActorRender(child);
            }
            d->flags &= ~1;
            d->flags &= ~2;
        }
        VFree(d);
        a->data = 0;
    }
}

int SwarmInit(Actor* a) {
    float px, py, pz;
    int i;
    SwarmData* d;
    Vec3 euler;
    SpawnDesc spawn;

    if (!ActorInitBase(a)) {
        return 0;
    }
    a->flags |= 0x20000;
    d = (SwarmData*)VAlloc(100);
    a->data = d;
    memset(d, 0, 100);
    d->unk34 = 50.0f;
    d->forceScale = 10000.0f;
    d->targetSpeed = 2000.0f;
    d->blendInner = 2000.0f;
    d->blendOuter = 4000.0f;
    d->blendNear = 0.6f;
    d->blendFar = 0.1f;
    d->weightForward = 1.0f;
    d->radius = 500.0f;
    d->weightCenter = 1.0f;
    d->weightAlign = 0.7f;
    d->alignBias = 0.0f;

    px = (float)a->x;
    py = (float)a->y;
    pz = (float)a->z;
    d = (SwarmData*)a->data;

    memset(&spawn, 0, sizeof(spawn));
    spawn.f1C = a->info->fA4;
    {
    Vector* children = &d->children;
    for (i = 0; i < 10; i++) {
        Vec3Set(&euler, 360.0f * ((float)rand() / 32767.0f), 20.0f * ((float)rand() / 32767.0f) - 10.0f, 0.0f);
        fn_8016D4EC(&spawn.pos, &euler);
        fn_800E1098(&spawn.pos, &spawn.pos, d->radius);
        spawn.pos.x += px;
        spawn.pos.y += py;
        spawn.pos.z += pz;
        spawn.hash = StrHash("Ghost Bee");

        {
            Actor* child = fn_800C7E80(&spawn);

            if (!child) {
                break;
            }
            *(Actor**)fn_8028B3BC(children, children->data + children->size, 4) = child;
        }
    }
    }
    d->flags |= 1;
    d->flags &= ~2;
    return 1;
}

int SpinSetup(Actor* a) {
    TrailDesc desc;
    AnimMsg m;
    CountMsg s;
    SpinData* d;

    if (a->data == 0) {
        d = (SpinData*)VAlloc(0x28);
        a->data = d;
        memset(d, 0, 0x28);
        d->trail[0] = -1;
        d->trail[1] = -1;
        d->trail[2] = -1;
    } else {
        d = (SpinData*)a->data;
    }
    a->flags |= 0x20000;
    d->vel.x = 1.0f;
    d->vel.y = 0.0f;
    d->vel.z = 0.0f;
    d->dir.x = 0.0f;
    d->dir.y = 0.0f;
    d->dir.z = 0.0f;

    memset(&m, 0, sizeof(m));
    memset(&s, 0, sizeof(s));
    m.anim = "SPIN";
    m.asset = a->info->f80;
    m.time = 300;
    SendMsg(0x3F000000, 0x6300, &m);
    s.asset = m.asset;
    s.count = 1;
    SendMsg(0x3F000000, 0x6303, &s);

    m.anim = "SPIN";
    m.asset = a->info->f84;
    m.time = -400;
    SendMsg(0x3F000000, 0x6300, &m);
    s.asset = m.asset;
    s.count = 1;
    SendMsg(0x3F000000, 0x6303, &s);

    desc.owner = a->handle;
    desc.segments = 500;
    desc.unk14 = -4.0f;
    desc.unk20 = 0.01f;
    desc.unk24 = -10.0f;
    desc.unk28 = 50.0f;
    desc.color[0] = 250;
    desc.color[1] = 250;
    desc.color[2] = 250;
    desc.texture = a->info->f88;

    if (d->trail[0] == -1) {
        desc.width = 20.0f;
        desc.unk2C = -0.5f;
        desc.unk8 = AssetFind(0xF1754BF2, 0x934819A5);
        d->trail[0] = TrailCreate(&desc);
    }
    if (d->trail[1] == -1) {
        desc.width = 50.0f;
        desc.unk2C = 0.0f;
        desc.unk8 = AssetFind(0xF1754BF2, 0x934819A4);
        d->trail[1] = TrailCreate(&desc);
    }
    if (d->trail[2] == -1) {
        desc.width = 20.0f;
        desc.unk2C = 0.5f;
        desc.unk8 = AssetFind(0xF1754BF2, 0x934819A6);
        d->trail[2] = TrailCreate(&desc);
    }
    return 1;
}

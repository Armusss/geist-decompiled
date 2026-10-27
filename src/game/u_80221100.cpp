// u_80221100.cpp: "GhostBees" swarm effect state (0x80221100-0x80222C08).
// A swarm of bee actors orbits a centre point near its owner: a state machine (idle / flying out / hovering /
// returning / dying) moves them with spring-like steering, spawns them with trails, and tears them down.
// Names are descriptive guesses; the code is matched against the retail DOL.

#define fabs(x) __fabs(x)

typedef unsigned char u8;
typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

// Orientation (3x3) followed by the position, as the actor matrices are laid out.
struct Mtx34 {
    float rot[9];
    Vec3 pos;
};

struct Place {
    int w[12];
};

// Reply buffer of the 0x411 placement query.
struct PlaceQuery {
    int unk0;
    u32 hash;
    int pad[2];
    Place place;
};

// Descriptor for SendMsg(0x6300) / (0x6303) sound-like effect messages.
struct FxMsgA {
    int unk0;
    u32 hash;
    const char* name;
    int unkC;
    int value;
    u8 pad[0x30 - 0x14];
};

struct FxMsgB {
    int unk0;
    u32 hash;
    int flag;
    int pad;
};

struct TrailDesc {
    int unk0;
    int target;
    u32 hashA;
    int count;
    float f10;
    float f14;
    float f18;
    float f1c;
    float f20;
    float f24;
    float f28;
    float f2c;
    u32 hashB;
    u8 r, g, b;
    u8 pad[1];
};

struct SpawnDesc {
    Vec3 pos;
    u8 pad[0x1c - 0xc];
    u32 type;
    u8 pad2[0x38 - 0x20];
};

struct HitDesc {
    int unk0;
    int unk4;
    int unk8;
    u8 pad[0x5c - 0xc];
};

struct IVec3 {
    int x, y, z;
};

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Actor;

// Growable pointer array base class (out of line ctor/dtor).
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    u32 unk04;     // 0x04
    u32 size;      // 0x08 (bytes used)
    u8* data;      // 0x0c
};

extern "C" void fn_8028B960(void* object);

// Intermediate class: its destructor releases the storage before the base destructor runs.
class BeeBase : public Buffer {
public:
    BeeBase() {}
    virtual ~BeeBase() { fn_8028B960(this); }
};

class BeeList : public BeeBase {
public:
    BeeList() {}
    virtual ~BeeList() {}
};

extern "C" {
int fn_80049940();
int SendMsg(int to, int msg, void* data);
int rand(void);
void* memset(void* dst, int value, u32 size);
void fn_800E1098(Vec3* out, Vec3* in, float length);
float fn_800E1020(Vec3* v);
void fn_8016D4EC(Vec3* out, Vec3* in);
void fn_8016D864(void* mtx, int handle);
void* fn_800C7E80(void* place);
int fn_8028B3BC(BeeList* list, int offset, int size);
void fn_8028B35C(BeeList* list, int data, int size);
void fn_8028B2D4(BeeList* list);
int fn_801DE960(void* desc);
}

int PlayerInHost(Actor* actor);
void ActorAim(int* out, Actor* actor, int flag);
void ActorRender(Actor* actor);
void TrailDestroy(int trail);
void VFree(void* p);
void* VAlloc(int size);
void ActorPlayAnim(int handle, u32 anim, int arg);
void ActorGetMatrix(Mtx34* out, Actor* actor);
void Vec3Set(Vec3* v, float x, float y, float z);
int ModelBoneHitSphere(int a, int b, int c, int d, int e, int f);
void ActorSetPlacement(Actor* actor, void* place);
int ActorFn80022FD8(Actor* actor);
void ActorFn800231A8(Actor* actor);
u32 AssetFind(u32 table, u32 key);

extern int lbl_804D3460;
extern u32 lbl_804D3464;

// One bee of the swarm (0x38 bytes).
struct Bee {
    Actor* actor;   // 0x00
    int trail[3];   // 0x04
    float speed;    // 0x10
    Vec3 pos;       // 0x14
    Vec3 vel;       // 0x20
    Vec3 steer;     // 0x2c
};

// The swarm state hung off the owner actor at +0x9ec (0xac bytes).
void* VAlloc(int size);
void VFree(void* p);

struct BeesState {
    static void* operator new(u32 size) { return VAlloc(size); }
    static void operator delete(void* p) { VFree(p); }
    int state;               // 0x00
    float timer;             // 0x04
    u32 flags;               // 0x08
    BeeList bees;            // 0x0c
    u8 unk1c[0x20 - 0x1c];
    Vec3 center;             // 0x20
    float sumDist;           // 0x2c
    float minDist;           // 0x30
    float maxDist;           // 0x34
    float unk38;             // 0x38
    float unk3c;             // 0x3c
    float unk40;             // 0x40
    float unk44;             // 0x44
    float unk48;             // 0x48
    float speed;             // 0x4c
    float unk50;             // 0x50
    int spawnCount;          // 0x54
    int unk58;               // 0x58
    float spawnTimer;        // 0x5c
    float time;              // 0x60
    float unk64;             // 0x64
    float unk68;             // 0x68
    float unk6c;             // 0x6c
    float unk70;             // 0x70
    int unk74;               // 0x74
    float unk78;             // 0x78
    float unk7c;             // 0x7c
    float unk80;             // 0x80
    float unk84;             // 0x84
    float unk88;             // 0x88
    float unk8c;             // 0x8c
    float unk90;             // 0x90
    float unk94;             // 0x94
    float unk98;             // 0x98
    float unk9c;             // 0x9c
    float unka0;             // 0xa0
    float unka4;             // 0xa4
    float unka8;             // 0xa8
};

struct Actor {
    int rot[9];              // 0x00
    int pos[3];              // 0x24
    u8 unk30[0x38 - 0x30];
    int handle;              // 0x38
    u8 unk3c[0x40 - 0x3c];
    int unk40;               // 0x40
    u8 unk44[0x4c - 0x44];
    u32 flags;               // 0x4c
    u8 unk50[0x9ec - 0x50];
    BeesState* bees;         // 0x9ec
    u8 unk9f0[0x14fc - 0x9f0];
    float dt;                // 0x14fc
    u8 unk1500[0x1c68 - 0x1500];
    int unk1c68;             // 0x1c68
    u8 unk1c6c[0x26e4 - 0x1c6c];
    u8* unk26e4;             // 0x26e4
};

struct Msg {
    Actor* actor;   // 0x00
    int msg;        // 0x04
    int unk08;      // 0x08
    int value;      // 0x0c
};

inline float Clamp01(float t)
{
    float lo = 0.0f;
    float hi = 1.0f;
    float r;
    if (t < lo) {
        r = lo;
    } else if (t > hi) {
        r = hi;
    } else {
        r = t;
    }
    return r;
}

// Overall speed scale of the swarm, from the state's blend parameters.
extern "C" float fn_80221100(Actor* actor)  // BeesSpeedScale; called by other units under this name
{
    BeesState* s = actor->bees;
    float r;
    if (s->flags & 1) {
        float a = s->unk64 * s->unk64;
        float t = (s->sumDist - a) / (s->unk6c * s->unk6c - a);
        float u = Clamp01(t);
        float v = Clamp01(s->time / (1000.0f * (float)s->unk74));
        r = (s->unk68 * (1.0f - u) + s->unk70 * u) * (s->unk78 * v + (1.0f - v));
    } else {
        r = 1.0f;
    }
    return r;
}

int BeesSetTimeMsg(Msg* m)
{
    BeesState* s = m->actor->bees;
    switch (m->msg) {
    case 0x530BE0C7:
        s->unk38 = (float)m->value;
        s->timer = 0.0f;
        return 1;
    default:
        return 0;
    }
}

int BeesGetTimeMsg(Msg* m)
{
    BeesState* s = m->actor->bees;
    switch (m->msg) {
    case 0x530BE0C7:
        m->value = (int)s->unk38;
        return 1;
    default:
        return 0;
    }
}

// Non-zero test and approximate normalize.
inline int VecNonZero(const Vec3* v)
{
    return (fabs(v->x) > 1.1920929e-7f) + (fabs(v->y) > 1.1920929e-7f) + (fabs(v->z) > 1.1920929e-7f);
}

inline float VecNormalize(Vec3* v)
{
    double x, y, z, len;
    float inv;

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
    inv = __frsqrte((float)len);
    v->x = (float)(x * inv);
    v->y = (float)(y * inv);
    v->z = (float)(z * inv);
    return __fres(inv);
}

// Frees the swarm's bees and resets the counters.
inline void BeesFreeAll(BeesState* s)
{
    while ((s->bees.size >> 2) != 0) {
        Bee* b = *(Bee**)s->bees.data;
        ActorRender(b->actor);
        TrailDestroy(b->trail[0]);
        TrailDestroy(b->trail[1]);
        TrailDestroy(b->trail[2]);
        VFree(*(void**)s->bees.data);
        fn_8028B35C(&s->bees, (int)s->bees.data, 4);
    }
}

inline void BeesFreeReset(BeesState* s)
{
    BeesFreeAll(s);
    s->time = 0.0f;
    s->spawnTimer = 0.0f;
    s->flags &= ~1;
    s->flags &= ~2;
}

void BeesSpawn(Actor* actor, BeesState* s, int count);

int BeesUpdate(Actor* a)
{
    BeesState* s = a->bees;
    if (s != 0 && fn_80049940() == 0 && (a->flags & 1) == 0 && lbl_804D3460 != 0) {
        PlaceQuery q;
        Place place;
        int hit;
        float t;
        s->timer += a->dt;
        q.hash = lbl_804D3464;
        hit = SendMsg(a->handle, 0x411, &q);
        if (hit == 0) {
            place = *(Place*)a;
        } else {
            place = q.place;
        }
        if (PlayerInHost(a) != 0 || (t = s->unk38) < 0.0f) {
            int st = s->state;
            if (st == 3) {
                if (s->timer <= s->unk40 || PlayerInHost(a) == 0) {
                    float cx = (float)place.w[9];
                    float cy = (float)place.w[10];
                    float cz = (float)place.w[11];
                    int ang[3];
                    Vec3 aim;
                    Bee** p;
                    ActorAim(ang, (Actor*)&place, 1);
                    aim.x = (float)ang[0];
                    aim.y = (float)ang[1];
                    aim.z = (float)ang[2];
                    fn_800E1098(&aim, &aim, 1000.0f);
                    for (p = (Bee**)s->bees.data; p != (Bee**)(s->bees.data + s->bees.size); p++) {
                        Bee* b = *p;
                        b->steer.x = (float)(cx + aim.x) - b->pos.x;
                        b->steer.y = (float)(cy + aim.y) - b->pos.y;
                        b->steer.z = (float)(cz + aim.z) - b->pos.z;
                    }
                } else {
                    s->timer = 0.0f;
                    s->state = 4;
                }
            } else if (st < 3) {
                if (st == 0) {
                    s->timer = 0.0f;
                } else if (st >= 0) {
                    s->timer = 0.0f;
                    s->state = 3;
                }
            } else if (st < 5) {
                float cx = (float)place.w[9];
                float cy = (float)place.w[10];
                float cz = (float)place.w[11];
                Bee** p;
                for (p = (Bee**)s->bees.data; p != (Bee**)(s->bees.data + s->bees.size); p++) {
                    Bee* b = *p;
                    b->steer.x = b->pos.x - cx;
                    b->steer.y = b->pos.y - cy;
                    b->steer.z = b->pos.z - cz;
                }
                if (s->unk44 * s->unk44 < s->minDist) {
                    BeesFreeAll(s);
                    s->time = 0.0f;
                    s->spawnTimer = 0.0f;
                    s->flags &= ~1;
                    s->flags &= ~2;
                    s->state = 0;
                }
            }
        } else {
            int st = s->state;
            if (st < 3) {
                if (st == 0) {
                    if (0.0f <= t && t < s->timer) {
                        BeesSpawn(a, s, s->spawnCount);
                        s->state = 1;
                        ActorPlayAnim(a->handle, 0x6EB086CB, a->handle);
                    }
                } else if (st >= 0) {
                    float cx = (float)place.w[9];
                    float cy = (float)place.w[10];
                    float cz = (float)place.w[11];
                    Bee** p;
                    for (p = (Bee**)s->bees.data; p != (Bee**)(s->bees.data + s->bees.size); p++) {
                        Bee* b = *p;
                        b->steer.x = cx - b->pos.x;
                        b->steer.y = cy - b->pos.y;
                        b->steer.z = cz - b->pos.z;
                    }
                    if (s->state == 1 && s->minDist < 64000000.0f) {
                        ActorPlayAnim(a->handle, 0x48E9C4D8, a->handle);
                        s->state = 2;
                    }
                    if (s->unk58 > 0) {
                        float interval = 60000.0f / (float)s->unk58;
                        s->spawnTimer += a->dt;
                        while (interval < s->spawnTimer) {
                            s->spawnTimer = s->spawnTimer - interval;
                            BeesSpawn(a, s, 1);
                        }
                    }
                    if ((float)rand() / 32767.0f < s->unka4) {
                        u32 r = rand();
                        Bee* pick = 0;
                        Bee** p2 = (Bee**)s->bees.data;
                        u32 n = s->bees.size >> 2;
                        Bee** end = (Bee**)(s->bees.data + s->bees.size);
                        int k = r - (r / n) * n;
                        for (; p2 != end; p2++) {
                            if (k == 0) {
                                pick = *p2;
                                break;
                            }
                            k--;
                        }
                        if (pick != 0) {
                            pick->speed = s->speed * s->unka8;
                        }
                    }
                }
            } else if (st < 5) {
                s->timer = 0.0f;
                s->state = 2;
            }
        }
        if ((s->flags & 1) != 0) {
            float scale;
            float maxF = 3.4028235e38f;
            float zero = 0.0f;
            Mtx34 aMtx;
            Mtx34 bMtx;
            char camMtx[24];
            Bee** p;
            double sx, sy, sz;
            if (PlayerInHost(a) == 0) {
                float a2 = s->unk80 * s->unk80;
                float tt = (s->sumDist - a2) / (s->unk84 * s->unk84 - a2);
                float u = Clamp01(tt);
                scale = s->unk88 * (1.0f - u) + s->unk8c * u;
            } else {
                scale = s->unk8c;
            }
            s->sumDist = 0.0f;
            s->minDist = maxF;
            s->maxDist = zero;
            sx = sy = sz = 0.0;
            fn_8016D864(camMtx, a->unk40);
            ActorGetMatrix(&aMtx, a);
            for (p = (Bee**)s->bees.data; p != (Bee**)(s->bees.data + s->bees.size); p++) {
                Bee* b = *p;
                Vec3 toBee;
                Vec3 toCenter;
                Vec3 force[3];
                float w[3];
                Vec3 acc;
                Vec3 rnd;
                int i;
                toBee.x = b->pos.x - aMtx.pos.x;
                toBee.y = b->pos.y - aMtx.pos.y;
                toBee.z = b->pos.z - aMtx.pos.z;
                force[0] = b->steer;
                w[0] = scale * s->unk90;
                force[1].x = s->center.x - b->pos.x;
                toCenter.x = s->center.x - aMtx.pos.x;
                force[1].y = s->center.y - b->pos.y;
                toCenter.y = s->center.y - aMtx.pos.y;
                force[1].z = s->center.z - b->pos.z;
                toCenter.z = s->center.z - aMtx.pos.z;
                w[1] = (scale * s->unk98)
                       * ((force[1].z * force[1].z + force[1].x * force[1].x + force[1].y * force[1].y)
                          / (s->unk94 * s->unk94));
                VecNormalize(&toCenter);
                VecNormalize(&toBee);
                w[2] = s->unk9c * (1.0f / s->unka0)
                       * ((toCenter.z * toBee.z + toCenter.x * toBee.x + toCenter.y * toBee.y) - s->unka0);
                if (w[2] > 0.0f) {
                    float r1 = (float)rand() / 32767.0f;
                    float r2 = (float)rand() / 32767.0f;
                    Vec3Set(&rnd, 360.0f * r2, 90.0f * r1 - 45.0f, 0.0f);
                    fn_8016D4EC(&force[2], &rnd);
                }
                if (s->time < b->speed) {
                    w[0] = w[0] * 1.5f;
                    w[1] = w[1] * 0.4f;
                    w[2] = w[2] * 0.4f;
                }
                acc.x = 0.0f;
                acc.y = 0.0f;
                acc.z = 0.0f;
                for (i = 0; i < 3; i++) {
                    if (w[i] > 0.0f && VecNonZero(&force[i]) != 0) {
                        fn_800E1098(&force[i], &force[i], 1.0f);
                        acc.x += force[i].x;
                        acc.y += force[i].y;
                        acc.z += force[i].z;
                    }
                }
                if (VecNonZero(&acc) != 0) {
                    fn_800E1098(&acc, &acc, 0.001f * s->unk50 * a->dt);
                }
                b->vel.x = b->vel.x + acc.x;
                b->vel.y = b->vel.y + acc.y;
                b->vel.z = b->vel.z + acc.z;
                if (b->speed < fn_800E1020(&b->vel)) {
                    fn_800E1098(&b->vel, &b->vel, b->speed);
                }
                {
                    float dtk = 0.001f * a->dt;
                    b->pos.x = b->pos.x + b->vel.x * dtk;
                    b->pos.y = b->pos.y + b->vel.y * dtk;
                    b->pos.z = b->pos.z + b->vel.z * dtk;
                }
                if (b->speed > s->speed) {
                    b->speed = -(0.2f * a->dt - b->speed);
                    if (b->speed < s->speed) {
                        b->speed = s->speed;
                    }
                } else if (b->speed < s->speed) {
                    b->speed = 0.2f * a->dt + b->speed;
                    if (s->speed < b->speed) {
                        b->speed = s->speed;
                    }
                }
                {
                    float d2 = (b->pos.z - aMtx.pos.z) * (b->pos.z - aMtx.pos.z)
                               + (b->pos.x - aMtx.pos.x) * (b->pos.x - aMtx.pos.x)
                               + (b->pos.y - aMtx.pos.y) * (b->pos.y - aMtx.pos.y);
                    float m1 = d2;
                    if (s->minDist < d2) {
                        m1 = s->minDist;
                    }
                    s->minDist = m1;
                    m1 = d2;
                    if (d2 < s->maxDist) {
                        m1 = s->maxDist;
                    }
                    s->maxDist = m1;
                    s->sumDist = s->sumDist + d2;
                }
                sx = (float)(sx + (double)b->pos.x);
                sy = (float)(sy + (double)b->pos.y);
                sz = (float)(sz + (double)b->pos.z);
                if (a->unk1c68 == 0 && PlayerInHost(a) == 0
                    && ModelBoneHitSphere((int)&b->pos, (int)&toBee, (int)camMtx, (int)&aMtx, (int)&bMtx, (int)&acc)
                           != 0) {
                    HitDesc hd;
                    float tt;
                    float u;
                    memset(&hd, 0, 0x5c);
                    hd.unk4 = -1;
                    hd.unk8 = 0x1F0C31;
                    tt = s->time / (1000.0f * (float)s->unk74);
                    u = Clamp01(tt);
                    *(float*)&hd.unk4 = 0.001f * s->unk48 * a->dt * (s->unk78 * u + (1.0f - u));
                    SendMsg(a->handle, 0x407, &hd);
                }
                {
                    Mtx34 m;
                    Actor* ba = b->actor;
                    *(int*)((u8*)ba + 0x24) = (int)b->pos.x;
                    *(int*)((u8*)ba + 0x28) = (int)b->pos.y;
                    *(int*)((u8*)ba + 0x2c) = (int)b->pos.z;
                    *(int*)((u8*)ba + 0x118) = (int)b->pos.z;
                    *(u32*)((u8*)ba + 0xac) |= 0x800000;
                    ActorGetMatrix(&m, ba);
                    ActorSetPlacement(ba, &m);
                    if (ActorFn80022FD8(ba) == 0) {
                        ActorFn800231A8(ba);
                    }
                }
            }
            s->center.x = (float)(sx * (1.0f / (float)(s->bees.size >> 2)));
            s->center.y = (float)(sy * (1.0f / (float)(s->bees.size >> 2)));
            s->center.z = (float)(sz * (1.0f / (float)(s->bees.size >> 2)));
            s->sumDist = s->sumDist / (float)(s->bees.size >> 2);
            if (PlayerInHost(a) == 0 || (*(u32*)(a->unk26e4 + 0x38) >> 24) == 1) {
                if (((s->flags >> 1) & 1) != 0) {
                    if ((s->flags & 1) != 0) {
                        Bee** p3;
                        int off = 1;
                        for (p3 = (Bee**)s->bees.data; p3 != (Bee**)(s->bees.data + s->bees.size); p3++) {
                            SendMsg(*(int*)(*(int*)*p3 + 0x38), 0xC00, &off);
                        }
                    }
                    s->flags &= ~2;
                }
            } else if (((s->flags >> 1) & 1) != 1) {
                if ((s->flags & 1) != 0) {
                    Bee** p3;
                    int off = 0;
                    for (p3 = (Bee**)s->bees.data; p3 != (Bee**)(s->bees.data + s->bees.size); p3++) {
                        SendMsg(*(int*)(*(int*)*p3 + 0x38), 0xC00, &off);
                    }
                }
                s->flags |= 2;
            }
            if (PlayerInHost(a) == 0) {
                s->time = s->time + a->dt;
            }
        }
    }
    return 1;
}

void BeesDestroy(Actor* a)
{
    void* state = a->bees;
    if (state != 0) {
        BeesState* s = (BeesState*)state;
        if ((s->bees.size >> 2) != 0) {
            BeesFreeReset(s);
        }
        delete s;
        a->bees = 0;
    }
}

void BeesInit(Actor* a)
{
    if (fn_80049940() != 0) {
        a->bees = 0;
    } else {
        BeesState* s = new BeesState;
        a->bees = s;
        s->state = 0;
        s->timer = 0.0f;
        s->unk38 = -1.0f;
        s->unk3c = 12000.0f;
        s->unk40 = 8000.0f;
        s->unk44 = 8000.0f;
        s->unk48 = 100.0f;
        s->speed = 2000.0f;
        s->unk50 = 10000.0f;
        s->spawnCount = 15;
        s->unk58 = 0;
        s->spawnTimer = 0.0f;
        s->unk80 = 2000.0f;
        s->unk84 = 4000.0f;
        s->unk88 = 0.6f;
        s->unk8c = 0.1f;
        s->unk90 = 1.0f;
        s->unk94 = 500.0f;
        s->unk98 = 1.0f;
        s->unk9c = 0.6f;
        s->unka0 = 0.4f;
        s->unka4 = 0.005f;
        s->unka8 = 3.0f;
        s->time = 0.0f;
        s->unk64 = 400.0f;
        s->unk68 = 0.85f;
        s->unk6c = 3000.0f;
        s->unk70 = 1.0f;
        s->unk74 = 60;
        s->unk78 = 0.5f;
        s->unk7c = 5.0f;
        lbl_804D3464 = AssetFind(0xF1754BF2, 0x21C5E0);
    }
}

void BeesSpawn(Actor* actor, BeesState* s, int count)
{
    float px = (float)actor->pos[0];
    float py = (float)actor->pos[1];
    float pz = (float)actor->pos[2];
    SpawnDesc sd;
    TrailDesc td;
    FxMsgA fa;
    FxMsgB fb;
    int n;
    memset(&sd, 0, 0x38);
    sd.type = AssetFind(0x87DE5E6A, 0xC414C358);
    memset(&fa, 0, 0x30);
    memset(&fb, 0, 0x10);
    fa.name = "SPIN";
    fa.hash = AssetFind(0xF2E2BEB8, 0x140D0842);
    fa.value = 300;
    SendMsg(0x3F000000, 0x6300, &fa);
    fb.flag = 1;
    fb.hash = fa.hash;
    SendMsg(0x3F000000, 0x6303, &fb);
    fa.name = "SPIN";
    fa.hash = AssetFind(0xF2E2BEB8, 0xA7263B9F);
    fa.value = -400;
    SendMsg(0x3F000000, 0x6300, &fa);
    fb.flag = 1;
    fb.hash = fa.hash;
    SendMsg(0x3F000000, 0x6303, &fb);
    td.target = -1;
    td.hashA = 0;
    td.count = 500;
    td.f10 = 20.0f;
    td.f14 = -4.0f;
    td.f1c = 0.01f;
    td.f20 = -10.0f;
    td.f24 = 50.0f;
    td.f28 = 0.0f;
    td.hashB = AssetFind(0xF2E2BEB8, 0x4BD29352);
    td.r = 0xFA;
    td.g = 0xFA;
    td.b = 0xFA;
    for (n = 0; n < count; n++) {
        Bee* b = (Bee*)VAlloc(0x38);
        Bee** slot;
        Vec3 rnd;
        if (b == 0) {
            break;
        }
        slot = (Bee**)fn_8028B3BC(&s->bees, (int)s->bees.data + s->bees.size, 4);
        *slot = b;
        {
            float r1 = (float)rand() / 32767.0f;
            float r2 = (float)rand() / 32767.0f;
            Vec3Set(&rnd, 360.0f * r2, 20.0f * r1 - 10.0f, 0.0f);
        }
        fn_8016D4EC(&b->pos, &rnd);
        fn_800E1098(&b->pos, &b->pos, s->unk3c);
        b->pos.x = b->pos.x + px;
        b->pos.y = b->pos.y + py;
        b->pos.z = b->pos.z + pz;
        b->vel.x = 1.0f;
        b->vel.y = 0.0f;
        b->vel.z = 0.0f;
        b->steer.x = 0.0f;
        b->steer.y = 0.0f;
        b->steer.z = 0.0f;
        sd.pos = b->pos;
        b->actor = (Actor*)fn_800C7E80(&sd);
        if (b->actor == 0) {
            VFree(*(void**)(s->bees.data + s->bees.size - 4));
            fn_8028B2D4(&s->bees);
        } else {
            td.target = b->actor->handle;
            td.f28 = -0.5f;
            td.f10 = 20.0f;
            td.hashB = AssetFind(0xF2E2BEB8, 0x4BD29352);
            td.hashA = AssetFind(0xF1754BF2, 0x934819A5);
            b->trail[0] = fn_801DE960(&td);
            td.hashB = AssetFind(0xF2E2BEB8, 0x4BD29352);
            td.hashA = AssetFind(0xF1754BF2, 0x934819A4);
            td.f28 = 0.0f;
            td.f10 = 50.0f;
            b->trail[1] = fn_801DE960(&td);
            td.hashB = AssetFind(0xF2E2BEB8, 0x4BD29352);
            td.hashA = AssetFind(0xF1754BF2, 0x934819A6);
            td.f28 = 0.5f;
            td.f10 = 20.0f;
            b->trail[2] = fn_801DE960(&td);
            b->speed = s->speed;
        }
    }
    s->flags |= 1;
    s->flags &= ~2;
}

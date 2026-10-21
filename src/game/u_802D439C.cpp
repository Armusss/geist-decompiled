typedef unsigned char u8;
typedef unsigned long u32;

// From a shared header: names of the ghost actions (the function is unused here).
inline void GhostActionNames() {
    static const char* names[] = {
        "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
    };
}

struct Vec3 {
    float x, y, z;
};

struct Vec3i {
    int x, y, z;
};

struct Quat {
    float x, y, z, w;
};

struct Mtx34 {
    float m[3][4];
};

struct Actor;

typedef void (*MsgFunc)(int, void*);

struct Actor {
    char pad0[0x24];
    int cellX;         // 0x24
    int cellY;         // 0x28
    int cellZ;         // 0x2C
    MsgFunc handler;   // 0x30
    char pad1[4];      // 0x34
    u32 model;         // 0x38
    char pad2[0x10];   // 0x3C
    u32 flags;         // 0x4C
    char pad3[0x18];   // 0x50
    Vec3 pos;          // 0x68
    char pad4[0xC];    // 0x74
    Vec3 dir;          // 0x80
    char pad5[4];      // 0x8C
    float unk90;       // 0x90
    char pad6[0x20];   // 0x94
    Vec3 target;       // 0xB4
    char pad7[8];      // 0xC0
    int bone;          // 0xC8
    int unkCC;         // 0xCC
    char pad8[0x18];   // 0xD0
    float unkE8;       // 0xE8
    char pad9[4];      // 0xEC
    void* data;        // 0xF0
};

struct Params {
    char pad0[4];      // 0x00
    int unk4;          // 0x04
    int unk8;          // 0x08
    float unkC;        // 0x0C
    float unk10;       // 0x10
    char pad1[4];      // 0x14
    float unk18;       // 0x18
    int unk1C;         // 0x1C
    char pad2[0x24];   // 0x20
    void* unk44;       // 0x44
};

struct Owner {
    char pad0[0xC];
    int (*onHit)(Params*, Actor*, void*);   // 0x0C
};

struct Particle {
    float unk0;        // 0x00
    float life;        // 0x04
    float x;           // 0x08
    float y;           // 0x0C
    float z;           // 0x10
    float unk14[12];   // 0x14
};

struct EffectData {
    Particle particles[20];   // 0x000
    Vec3 trail[3];            // 0x550
    int trailIndex;           // 0x574
};

struct HitResult {
    Actor* obj;        // 0x00
    int unk04;         // 0x04
    int unk08;         // 0x08
    int unk0C;         // 0x0C
    int unk10;         // 0x10
    int unk14;         // 0x14
    int unk18;         // 0x18
    int unk1C;         // 0x1C
    int unk20;         // 0x20
    int unk24;         // 0x24
    int unk28;         // 0x28
    int unk2C;         // 0x2C
    Vec3i pos;         // 0x30
    Vec3i unk3C;       // 0x3C
};

struct HitQuery {
    HitResult* result;   // 0x00
    int type;            // 0x04
};

struct Msg {
    void* obj;         // 0x00
    Actor* actor;      // 0x04
    int actorCC;       // 0x08
    float damage;      // 0x0C
    int unk10;         // 0x10
    int unk14;         // 0x14
    Vec3i pos;         // 0x18
    Vec3i normal;      // 0x24
    Vec3i dir;         // 0x30
    int unk3C;         // 0x3C
    int unk40;         // 0x40
    int unk44;         // 0x44
    int unk48;         // 0x48
    float unk4C;       // 0x4C
    int unk50;         // 0x50
    int unk54;         // 0x54
    int unk58;         // 0x58
};

struct Desc {
    int unk0;          // 0x00
    int model;         // 0x04
    int type;          // 0x08
    char pad[0x34];    // 0x0C
};

extern "C" void* memset(void*, int, unsigned long);
extern "C" int rand(void);

extern "C" {
void fn_802A3778(Vec3* pos, int id);
void fn_800A0100(void* mtx);
void fn_800E1098(float scale, Vec3* out, Vec3* in);
int fn_80211470(void* particles, int stride, int count, float dt);
void fn_80211638(float life, float a, float b, float c, Particle* part,
                 Vec3* pos, Vec3* dir, int d, int e);
void fn_80210D80(void* particles, int stride, int count, int tex, int unk,
                 const float* tbl1, const float* tbl2);
void fn_8003F15C(int bone, Mtx34* mtx, int a, float f, int b, int c);
int fn_8001F928(Actor* actor, int* coords, u32 flags,
                int (*cb)(HitResult*, void*), void* query);
int fn_80049940(void);
void fn_80158C3C(Vec3* pos, u32 id, int a, int b, int c, int d, int e);
void fn_8013B6F8(float f, int tex, int mode);
void fn_8013B608(Vec3* cur, Vec3* prev, int a, int b, int c, u8 d, int e,
                 int f, int g, u8 h);
void fn_8013B5D4(void);
void fn_8016F120(void* fx, void* pos);
int fn_80184080(HitResult* result, void* query);
}

void* VAlloc(int size);
void VFree(void* ptr);
void* AssetFind(u32 a, u32 b);
void* FxFind(const char* name);
extern "C" void FxStart(void* fx, int a, int b);
void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void BoneGetMatrix(Mtx34* mtx, void* quat);

static float sParticleStep = 65.0f;
static int sTextureA = -1;
static int sTextureB = -1;

static float sParticleTable[] = {
    333.0f, 0.0f, 900.0f, 5000.0f, 0.0f,
};

static float sColorTable[] = {
    0.0f,    167.0f, 0.56f,  3.5f,   0.339f, 0.0f,
    0.8f,    0.8f,   0.8f,   1.0f,   0.3f,   0.3f,
    0.3f,    0.2f,
};

void GhostGunDestroy(Params* p, Actor* a) {
    Vec3 pos;

    pos.x = (float)a->cellX;
    pos.y = (float)a->cellY;
    pos.z = (float)a->cellZ;
    fn_802A3778(&pos, 0);
    VFree(a->data);
    a->data = 0;
}

int GhostGunUpdate(void* pp, Actor* a, float dt) {
    EffectData* d = (EffectData*)a->data;
    Params* p = (Params*)pp;
    HitResult hit;
    Msg msg;
    Mtx34 mtx;
    int coords[6];
    Vec3 spawn;
    Quat rot;
    HitQuery query;

    (void)p;
    query.result = &hit;
    query.type = a->unkCC;

    coords[0] = (int)a->target.x;
    coords[1] = (int)a->target.y;
    coords[2] = (int)a->target.z;
    coords[3] = (int)a->pos.x;
    coords[4] = (int)a->pos.y;
    coords[5] = (int)a->pos.z;


    fn_800E1098(2000.0f, &spawn, &a->dir);
    spawn.x += a->pos.x;
    spawn.y += a->pos.y;
    spawn.z += a->pos.z;

    fn_80211470(d, 0x44, 20, dt);
    for (int i = 0; i < 20; i++) {
        Particle* part = &d->particles[i];

        part->x = spawn.x;
        part->y = spawn.y;
        part->z = spawn.z;
        if (part->life <= 0.0f) {
            Vec3 zero = {0.0f, 0.0f, 0.0f};
            float t;

            rot.x = -1.0f;
            rot.y = 0.0f;
            rot.z = 0.0f;
            rot.w = 0.0f;
            BoneGetMatrix(&mtx, &rot);
            fn_800A0100(&mtx);
            t = (float)rand() / 32767.0f;
            fn_80211638(0.0f, 333.0f, 4095.0f * t, 0.0f, part, &a->pos,
                        &zero, 0, 0);
        }
    }

    d->trailIndex++;
    if (d->trailIndex == 3) {
        d->trailIndex = 0;
    }
    d->trail[d->trailIndex] = a->target;

    if (fn_8001F928(a, coords, 0x12002000, fn_80184080, &query) != 0) {
        if (p->unk44 != 0) {
            Owner* owner = (Owner*)p->unk44;

            if (owner->onHit != 0) {
                return owner->onHit(p, a, &hit) == 0 ? 1 : 0;
            }
        }

        a->pos = a->target;
        a->cellX = (int)a->target.x;
        a->cellY = (int)a->target.y;
        a->cellZ = (int)a->target.z;

        if (hit.obj != 0) {
            float dmg;

            if (fn_80049940() != 0) {
                dmg = (float)p->unk8 * a->unkE8;
                if ((int)(hit.obj->model >> 24) == 2) {
                    dmg *= p->unk10;
                }
            } else {
                dmg = (float)p->unk4 * a->unkE8;
                if ((int)(hit.obj->model >> 24) == 2) {
                    dmg *= p->unkC;
                }
            }

            msg.actor = a;
            msg.actorCC = a->unkCC;
            msg.unk14 = p->unk1C;
            msg.damage = dmg;
            msg.unk4C = p->unk18;
            msg.dir.x = (int)a->dir.x;
            msg.dir.y = (int)a->dir.y;
            msg.pos = hit.pos;
            msg.dir.z = (int)a->dir.z;
            msg.unk48 = hit.unk0C;
            msg.unk44 = hit.unk14;
            msg.normal = hit.unk3C;
            msg.unk40 = hit.unk18;
            msg.unk3C = hit.unk10;
            msg.obj = hit.obj;
            hit.obj->handler(0x407, &msg);

            if ((int)(hit.obj->model >> 24) == 0x1A) {
                Vec3 pos;

                pos.x = (float)hit.pos.x;
                pos.y = (float)hit.pos.y;
                pos.z = (float)hit.pos.z;
                fn_80158C3C(&pos, 0x3DEC6CB, -1, -1, -1, -1, -1);
                fn_8016F120(FxFind("finalboss_shieldhit"), &hit.pos);
            }
        }

        return 0;
    }

    return 1;
}

int GhostGunInit(Params* p, Actor* a) {
    EffectData* d;
    Desc desc;
    Mtx34 mtx;
    Quat rot;
    int i;

    d = (EffectData*)VAlloc(0x578);
    a->data = d;
    if (d == 0) {
        return 0;
    }

    a->unk90 = 15000.0f;
    a->flags |= 0x20000;

    memset(&desc, 0, sizeof(desc));
    desc.model = a->model;
    desc.type = -1;

    Vec3 zero = {0.0f, 0.0f, 0.0f};

    float t = 0.0f;
    rot.x = -1.0f;
    rot.y = t;
    rot.z = t;
    rot.w = t;
    BoneGetMatrix(&mtx, &rot);
    fn_800A0100(&mtx);

    for (i = 0; i < 20; i++) {
        float r = (float)rand() / 32767.0f;

        fn_80211638(t, 333.0f, 4095.0f * r, 0.0f, &d->particles[i], &a->pos,
                    &zero, 0, 0);
        t -= sParticleStep;
    }

    d->trailIndex = 0;
    d->trail[0] = a->pos;
    d->trail[1] = a->pos;
    d->trail[2] = a->pos;

    sTextureA = (int)AssetFind(0xF2E2BEB8, 0x415E24EA);
    sTextureB = (int)AssetFind(0xF2E2BEB8, 0x929869B8);
    FxStart(FxFind("geistghostgun"), 0, 0);

    return 1;
}

void GhostGunDraw(Params* p, Actor* a) {
    EffectData* d = (EffectData*)a->data;
    Mtx34 mtx;
    Vec3 prev;
    Vec3 cur;

    if (a->bone == -1) {
        return;
    }

    ActorGetMatrix(&mtx, a);
    fn_8003F15C(a->bone, &mtx, 1, 1.0f, 0, 0);
    fn_80210D80(d, 0x44, 20, sTextureA, 0, sParticleTable, sColorTable);

    d->trailIndex = 0;
    prev = a->pos;
    for (int i = 0; i < 3; i++) {
        int j = d->trailIndex - i;

        while (j < 0) {
            j += 3;
        }

        cur = d->trail[j];
        fn_8013B6F8((float)(9 - i * 2), sTextureB, 0);
        fn_8013B608(&cur, &prev, 0xB4, 0x50, 0x82,
                    (u8)(0xC8 - 0x28 * (i + 1)), 0xB4, 0x50, 0x82,
                    (u8)(0xC8 - i * 0x28));
        fn_8013B5D4();
        prev = cur;
    }
}

// u_801FC634.cpp: the "Rivet" fear/panic actor (0x801FC634-0x801FD994).
// Keeps a global list of fear objects, sets up the fear regions and spawns/updates/draws the
// red rivets that fly out of a frightened actor.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[12];
};

struct Actor;
struct FearRivet;

// ---------------------------------------------------------------------------------------------
// Query/hit structures.

// 0x48-byte block copied out of the collision query by the callback.
struct HitBlob {
    void* obj;   // 0x00
    int unk04;   // 0x04
    int unk08;   // 0x08
    int unk0C;   // 0x0C
    int unk10;   // 0x10
    int unk14;   // 0x14
    int unk18;   // 0x18
    int unk1C;   // 0x1C
    int unk20;   // 0x20
    int unk24;   // 0x24
    int unk28;   // 0x28
    int unk2C;   // 0x2C
    int pos[3];  // 0x30
    int vec[3];  // 0x3C
};

// The object the query points at (only the fields this unit touches).
struct Hit {
    char pad00[0x30];
    int (*msg)(int, void*);  // 0x30
    int unk34;               // 0x34
    int type;                // 0x38
    int vec[3];              // 0x3C
    char pad48[0x3C];        // 0x48
    int* unk84;              // 0x84
    int* unk88;              // 0x88
    char pad8C[0x38];        // 0x8C
    int* unkC4;              // 0xC4
    char padC8[0x840];       // 0xC8
    int unk908;              // 0x908
};

struct FearHitMsg401 {
    char pad00[0x14];
    float unk14;  // 0x14
};

typedef int (*QueryCallback)(void*, void**);

struct FearRivetCallback {
    char pad00[0xC];
    int (*cb)(FearRivet*, Actor*, void*);  // 0x0C
};

struct FearRivet {
    char pad00[4];
    int unk04;               // 0x04
    int unk08;               // 0x08
    float unk0C;             // 0x0C
    float unk10;             // 0x10
    float unk14;             // 0x14
    float unk18;             // 0x18
    int unk1C;               // 0x1C
    char pad20[0xC];         // 0x20
    int unk2C;               // 0x2C
    char pad30[0x14];        // 0x30
    FearRivetCallback* unk44;  // 0x44
};

struct Actor {
    char pad00[0x24];
    int unk24;           // 0x24
    int unk28;           // 0x28
    int unk2C;           // 0x2C
    float posX;          // 0x30
    float posY;          // 0x34
    float posZ;          // 0x38
    Vec3 vec3C;          // 0x3C
    int unk48;           // 0x48
    float unk4C;         // 0x4C
    float unk50;         // 0x50
    char pad54[0x14];    // 0x54
    float unk68;         // 0x68
    float unk6C;         // 0x6C
    float unk70;         // 0x70
    float unk74;         // 0x74
    float unk78;         // 0x78
    float unk7C;         // 0x7C
    Vec3 unk80;          // 0x80
    char pad8C[0x28];    // 0x8C
    float unkB4;         // 0xB4
    float unkB8;         // 0xB8
    float unkBC;         // 0xBC
    u16 flagsC0;         // 0xC0
    char padC2[2];
    float unkC4;         // 0xC4
    char padC8[4];
    u32 unkCC;           // 0xCC
    char padD0[4];
    float unkD4;         // 0xD4
    float unkD8;         // 0xD8
    float unkDC;         // 0xDC
    char padE0[8];
    float unkE8;         // 0xE8
    char padEC[0x15C];   // 0xEC
    int unk248;          // 0x248
    char pad24C[0x340];  // 0x24C
    int unk58C;          // 0x58C
    char pad590[0x16DC];  // 0x590
    int unk1C6C;         // 0x1C6C
    int unk1C70;         // 0x1C70
    int unk1C74;         // 0x1C74
};

struct SpawnInfo {
    Actor* actor;  // 0x00
    float time;    // 0x04
    char pad08[4];
    int unk0C;  // 0x0C
    int unk10;  // 0x10
};

struct SpawnDesc {
    Vec3 pos;         // 0x00
    Vec3 vec;         // 0x0C
    Vec3 offset;      // 0x18
    char pad24[0x28];  // 0x24
};

struct Query400 {
    Actor* actor;  // 0x00
    int unk04;     // 0x04
    int unk08;     // 0x08
    int unk0C;     // 0x0C
    int unk10;     // 0x10
    int unk14;     // 0x14
    u32 flags;     // 0x18
};

struct FearRivetHitMsg {
    void* hit;       // 0x00
    void* actor;     // 0x04
    u32 actorCC;     // 0x08
    float speed;     // 0x0C
    int unk10;       // 0x10
    int self1C;      // 0x14
    int pos[3];      // 0x18
    int vec[3];      // 0x24
    int actorPos[3]; // 0x30
    int hit10;       // 0x3C
    int hit18;       // 0x40
    int hit14;       // 0x44
    int hit0C;       // 0x48
    float self18;    // 0x4C
    int unk50;       // 0x50
    int unk54;       // 0x54
    int result;      // 0x58
};

struct AssetType {
    const char* name;  // 0x00
    u32 nameHash;      // 0x04
};

// Own type descriptor (referenced by the asset table).
struct FearOffset {
    Vec3 v;      // 0x00
    float pad;   // 0x0C
};

typedef int (*FearUpdateFn)(FearRivet*, Actor*);
typedef void (*FearDrawFn)(FearRivet*, Actor*);

struct FearRivetType {
    u32 id;          // 0x00
    u32 unk04;       // 0x04
    FearUpdateFn update;  // 0x08
    u32 unk0C;       // 0x0C
    u32 unk10;       // 0x10
    u32 unk14;       // 0x14
    FearDrawFn draw;  // 0x18
    u32 unk1C;       // 0x1C
    u32 unk20;       // 0x20
    u32 unk24;       // 0x24
};

struct FearObjEntry {
    void* obj;   // 0x00
    int count;   // 0x04
};

// ---------------------------------------------------------------------------------------------
// External data.

extern unsigned char __upper_map[256];
extern AssetType lbl_803B9E58;
extern u32 lbl_803B9FB8[];
extern int lbl_8049D2B0[4];
extern int lbl_804D3044;
extern int lbl_804D3378;
extern u32 lbl_804D52E8;
extern int lbl_804D5300;
extern FearObjEntry* lbl_804D530C;
extern int lbl_804D5310;
extern int lbl_804D5318;
extern u16* lbl_804D531C;
extern int lbl_804D5400;

Mtx34 gFearMtx;
extern const Vec3 sFearRivetOffset;
const Vec3 sFearRivetOffset = {0.0f, 2048.0f, 0.0f};

// ---------------------------------------------------------------------------------------------
// External functions.

extern "C" {
void* memset(void*, int, unsigned long);
int rand(void);
int SendMsg(void* to, int msg, void* data);
void FxStart(void* fx, int a, int b);

void fn_800E1098(void* out, void* in, float scale);
void fn_8016D440(Vec3* out, Vec3* in);
Vec3* fn_800772E0(void* actor);
void fn_800C5E00(int, int, int, int, int);
void fn_800E1BD8(int, int, int, int, int);
void fn_800B4C50(float* pos, u32 id, int a, int b, float f1, float f2);
void fn_8016D4EC(Vec3* out, Vec3* in);
void fn_8016D648(Vec3* out, Vec3* a, Vec3* b);
void fn_80186A88(int, void* desc, int, int, int, float);
int fn_8001F928(void* actor, int* ints, u32 flags, QueryCallback cb, void** ctx);
int fn_80183DD0(void* src, void** ctx);
void fn_8016F120(u32 id, int* pos);
void fn_80184BCC(int* delta, int* in, int* out);
int fn_80049940(void);
void fn_802C28EC(int* pos, int* vec, int a, void* actor);
void fn_80289EB4(int* pos, int* vec, int a, void* actor);
void fn_80184194(int, int* pos, u32 id);
void fn_80184704(int* pos, int* vec, int a, void* actor);
float fn_800E1020(Vec3* v);
void fn_80184A70(Vec3* a, Vec3* b, Vec3* out);
void fn_80289CEC(int* pos, int* vec);
void fn_802C2AEC(int* pos, int* vec, void* color, int mgr, int a, int b, int c, float f1,
                 float f2, float f3, float f4);
float fn_800A127C(float);
float fn_800A1418(float);
float fn_800A1394(float);
void fn_800A0094(Vec3* out, Vec3* in);
void fn_800A0100(Mtx34* mtx);
void fn_8013B6F8(float, int, int);
void fn_8013B608(float* mtx, Vec3* pos, int a, int b, int c, int d, int e, int f, int g,
                 int alpha);
void fn_8013B5D4(void);
int fn_8001A1B4(int* pos);
char* fn_8001A194(int id);
int fn_8001A14C(void* obj, int id);
void fn_8003D710(int);
void fn_801ED8CC(int a, void* hit, int b, int c, void* delta, int d, int e, float f1, float f2);
}

unsigned long AssetFind(unsigned long table, unsigned long key);
void ModelNodeSetup(int a, int b, int c, int d);
void ActorGetMatrix(Mtx34* mtx, Actor* actor);
unsigned long Fn80175D60(unsigned long id);

// ---------------------------------------------------------------------------------------------
// String hash (the shared n-Space helper).

inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    const char* p;
    u32 h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    for (p = s; *p != 0; p++) {
        h = h * 31 + (unsigned char)toupper(*p);
    }
    h = h % 0xFFFFFFFF;
    return h;
}

inline int VecNonZero(Vec3* v) {
    return (fabs(v->x) > 1.1920929e-7f) + (fabs(v->y) > 1.1920929e-7f) +
           (fabs(v->z) > 1.1920929e-7f);
}

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

void FearRivetSpawn(u32 id, int mode, SpawnInfo* info, int arg);

// ---------------------------------------------------------------------------------------------
// 0x801FC634

void FearRivetNoop(void) {}

// ---------------------------------------------------------------------------------------------
// 0x801FC638

int FearRivetGetConst(void) { return 0x3DA260F7; }

// ---------------------------------------------------------------------------------------------
// 0x801FC644

int FearRivetMsg(FearRivet* self, u32 msg, u32 id, SpawnInfo* info) {
    unsigned long v = Fn80175D60(id);

    switch (msg) {
    case 0x14967:
        FearRivetSpawn(id, v, info, *(int*)self);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------
// 0x801FC6CC

void FearRivetSpawn(u32 id, int mode, SpawnInfo* info, int arg) {
    Actor* actor = info->actor;
    SpawnDesc desc;
    Vec3 tmp;
    Vec3 v;
    int n;
    float a1;
    float a2;

    if (lbl_803B9E58.nameHash == 0) {
        lbl_803B9E58.nameHash = StrHash(lbl_803B9E58.name);
    }

    memset(&desc, 0, sizeof(desc));
    fn_800E1098(&desc.offset, &actor->vec3C, info->time);
    desc.pos.x = actor->posX;
    desc.pos.y = actor->posY;
    desc.pos.z = actor->posZ;
    fn_8016D440(&tmp, &actor->vec3C);
    desc.vec = tmp;

    if (mode == 0) {
        Query400 q;
        Actor* t;
        Vec3* vel;

        SendMsg((void*)id, 0x400, &q);
        t = q.actor;
        vel = fn_800772E0(t);
        desc.offset.x += vel->x * 1000.0f;
        desc.offset.y += vel->y * 1000.0f;
        desc.offset.z += vel->z * 1000.0f;
        if (t->unk1C6C == 0) {
            fn_800C5E00(t->unk248, 1, info->unk0C, info->unk10, 0);
        } else {
            fn_800C5E00(t->unk248, 1, t->unk1C70, t->unk1C74, 0);
        }
        fn_800E1BD8(t->unk58C, 1, 0xFA, 1000, 1000);
        if ((q.flags & 2) && lbl_804D5400 == 0) {
            fn_800B4C50(&actor->posX, id, 1, 0, 1000.0f, 10000.0f);
        }
    }

    n = 3 + actor->unk50;
    fn_8016D440(&v, &actor->vec3C);
    a1 = 360.0f * (float)(rand() % n) / 4096.0f;
    a2 = 360.0f * (float)(rand() % n) / 4096.0f;
    if (rand() % 2) {
        v.z += a1;
    } else {
        v.z -= a1;
    }
    if (rand() % 2) {
        v.x += a2;
    } else {
        v.x -= a2;
    }
    fn_8016D4EC(&desc.offset, &v);
    fn_800E1098(&desc.offset, &desc.offset, info->time);
    fn_80186A88(arg, &desc, id, actor->unk48, 0, actor->unk4C);
}

// ---------------------------------------------------------------------------------------------
// 0x801FCA4C

int FearRivetUpdate(FearRivet* self, Actor* actor) {
    int ints[6];
    HitBlob hitInfo;
    void* ctx[2];
    int deltaA[3];
    int deltaB[3];
    u32 flags;
    Hit* hit;
    int bad;
    int bad2;
    float speed;
    int result;
    Vec3 vtmp;

    ctx[0] = &hitInfo;
    ctx[1] = (void*)actor->unkCC;

    ints[0] = (int)actor->unkB4;
    ints[1] = (int)actor->unkB8;
    ints[2] = (int)actor->unkBC;
    ints[3] = (int)actor->unk68;
    ints[4] = (int)actor->unk6C;
    ints[5] = (int)actor->unk70;

    flags = 0x1200A000;
    if (actor->flagsC0 & 0x10) {
        flags |= 0x80000000;
    }

    if (fn_8001F928(actor, ints, flags, fn_80183DD0, ctx) == 0) {
        return 1;
    }

    if (self->unk44 != 0 && self->unk44->cb != 0) {
        return self->unk44->cb(self, actor, &hitInfo) == 0;
    }

    fn_8016F120(self->unk2C, hitInfo.pos);

    deltaA[0] = ints[3] - ints[0];
    deltaA[1] = ints[4] - ints[1];
    deltaA[2] = ints[5] - ints[2];
    fn_80184BCC(deltaA, hitInfo.vec, (int*)&vtmp);

    hit = (Hit*)hitInfo.obj;

    bad = 0;
    if (hit == 0) {
        bad = 1;
    } else {
        switch (hit->type >> 24) {
        case 1: {
            FearHitMsg401 m;

            m.unk14 = 0.0f;
            SendMsg(hit, 0x401, &m);
            if (hit->unk88 == 0) {
                bad = 1;
            } else if (m.unk14 <= 0.0f && hit->unk88[0] != 0xDCC384C1 &&
                       hit->unk88[0] != 0x3D66BA3) {
                bad = 1;
            }
            break;
        }
        case 2:
        case 0x13:
            break;
        default:
            bad = 1;
            break;
        }
    }

    if (bad) {
        bad2 = 0;
        if (hit != 0 && (hit->type >> 24) == 1 && hit->unk84 != 0 &&
            hit->unk84[1] == 0x201BAE) {
            bad2 = 1;
        }
        if ((actor->unkCC >> 24) == 2 && fn_80049940() == 0 && !bad2) {
            fn_80289EB4(hitInfo.pos, hitInfo.vec, hitInfo.unk0C, actor);
        } else {
            fn_802C28EC(hitInfo.pos, hitInfo.vec, hitInfo.unk0C, actor);
        }
        if (lbl_804D5400 == 1) {
            FxStart((void*)AssetFind(0x92234D44, 0x901A6EE3), 0, 0);
        } else {
            Vec3 dir;
            Vec3 diff;
            Vec3 v;
            Vec3 v68;
            float len;

            fn_80184194(hitInfo.unk10, hitInfo.pos, 0x96F4ED29);
            if ((actor->unkCC >> 24) == 2 && !bad2) {
                fn_80184704(hitInfo.pos, hitInfo.vec, hitInfo.unk0C, actor);
            }
            v.x = (float)hitInfo.vec[1];
            v.y = (float)hitInfo.vec[0];
            v.z = (float)hitInfo.vec[2];
            VecNormalize(&v);
            len = fn_800E1020(&actor->unk80);
            dir = actor->unk80;
            VecNormalize(&dir);
            fn_80184A70(&actor->unk80, &v, &actor->unk80);
            diff.x = dir.x - actor->unk80.x;
            diff.y = dir.y - actor->unk80.y;
            diff.z = dir.z - actor->unk80.z;
            if (VecNonZero(&diff)) {
                actor->unk68 = (float)hitInfo.pos[0];
                actor->unk6C = (float)hitInfo.pos[1];
                actor->unk70 = (float)hitInfo.pos[2];
                actor->unk24 = hitInfo.pos[0];
                actor->unk28 = hitInfo.pos[1];
                actor->unk2C = hitInfo.pos[2];
                fn_8016D440(&v68, &actor->unk80);
                actor->unk74 = v68.x;
                actor->unk78 = v68.y;
                actor->unk7C = v68.z;
            }
            fn_800E1098(&actor->unk80, &actor->unk80, len);
            return 1;
        }
    }

    if (hit == 0 || lbl_804D5400 != 0) {
        return 0;
    }

    if (fn_80049940() != 0) {
        speed = (float)self->unk08 * actor->unkE8;
        if ((actor->unkCC >> 24) == 2) {
            speed *= self->unk10;
        }
    } else {
        speed = (float)self->unk04 * actor->unkE8;
        if ((actor->unkCC >> 24) == 2) {
            speed *= self->unk0C;
        }
    }

    {
        FearRivetHitMsg m;

        m.hit = hit;
        m.actor = actor;
        m.actorCC = actor->unkCC;
        m.speed = speed;
        m.self1C = self->unk1C;
        m.pos[0] = hitInfo.pos[0];
        m.pos[1] = hitInfo.pos[1];
        m.pos[2] = hitInfo.pos[2];
        m.vec[0] = hitInfo.vec[0];
        m.vec[1] = hitInfo.vec[1];
        m.vec[2] = hitInfo.vec[2];
        m.actorPos[0] = (int)actor->unk80.x;
        m.actorPos[1] = (int)actor->unk80.y;
        m.actorPos[2] = (int)actor->unk80.z;
        m.hit10 = hitInfo.unk10;
        m.hit18 = hitInfo.unk18;
        m.hit14 = hitInfo.unk14;
        m.hit0C = hitInfo.unk0C;
        m.self18 = self->unk18;
        m.result = 0;
        result = hit->msg(0x407, &m);
    }

    if (result == 1) {
        u32 color;
        Vec3 v1;
        Vec3 v38;
        Vec3 neg;
        Vec3 v44;
        float f;
        float d;
        float len;
        int angle;

        if ((actor->unkCC >> 24) != 2 || fn_80049940() != 0) {
            fn_802C28EC(hitInfo.pos, hitInfo.vec, 0x86, actor);
            return 0;
        }
        actor->unkC4 = 0.0f;
        actor->flagsC0 |= 0x10;
        fn_80289CEC(hitInfo.pos, hitInfo.vec);
        color = 0xFFFFFFFF;
        fn_802C2AEC(hitInfo.pos, hitInfo.vec, &color, lbl_804D3044, 200, 50, 1, 50.0f, 96.0f,
                    200.0f, 8.0f);
        f = (float)rand() / 32767.0f;
        angle = rand() % 360;
        d = fn_800A127C(15.0f * f + 10.0f);
        len = fn_800E1020(&actor->unk80);
        v1.z = d * fn_800A1418((float)angle);
        v1.x = d * fn_800A1394((float)angle);
        v1.y = 1.0f;
        fn_8016D440(&v38, &v1);
        neg.x = -actor->unk80.x;
        neg.y = -actor->unk80.y;
        neg.z = -actor->unk80.z;
        fn_8016D440(&v44, &neg);
        fn_8016D648(&v44, &v44, &v38);
        fn_8016D4EC(&actor->unk80, &v44);
        fn_800E1098(&actor->unk80, &actor->unk80, 0.5f * len);
        actor->unk74 = v44.x;
        actor->unk78 = v44.y;
        actor->unk7C = v44.z;
        actor->unkD4 = (float)hitInfo.pos[0];
        actor->unkD8 = (float)hitInfo.pos[1];
        actor->unkDC = (float)hitInfo.pos[2];
        fn_800E1098(&actor->unk68, &actor->unk80, 100.0f);
        actor->unk68 += actor->unkD4;
        actor->unk6C += actor->unkD8;
        actor->unk70 += actor->unkDC;
        return 1;
    }

    deltaB[0] = ints[3] - ints[0];
    deltaB[1] = ints[4] - ints[1];
    deltaB[2] = ints[5] - ints[2];

    if ((hit->type >> 24) != 0x13) {
        return 0;
    }

    {
        HitBlob copy;
        int done;

        copy = hitInfo;
        if (hit->unk908 == 0x8CD18EE1) {
            return 0;
        }
        done = 0;
        switch (hit->unkC4[0]) {
        case 0x11BAC:
            fn_801ED8CC(0, &copy, 2000, 0, deltaB, 1, 1, 300.0f, 100.0f);
            done = 1;
            break;
        case 0x2889735F:
        case 0x27B2569E:
        case 0x4BD66AD8:
            fn_801ED8CC(2, &copy, 2000, 0, deltaB, 0, 1, 150.0f, 50.0f);
            done = 1;
            break;
        }
        if (!done) {
            fn_801ED8CC(1, &copy, 2000, 1, deltaB, 0, 1, 300.0f, 100.0f);
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------
// 0x801FD5E8

void FearRivetDraw(FearRivet* self, Actor* actor) {
    Vec3 pos = sFearRivetOffset;
    Vec3 p;
    Mtx34 mtx;
    int alpha;

    ActorGetMatrix(&mtx, actor);
    fn_800A0100(&mtx);
    fn_800A0094(&p, &pos);
    p.x += mtx.m[9];
    p.y += mtx.m[10];
    p.z += mtx.m[11];
    fn_8013B6F8(4.0f, -4, 0);
    alpha = (int)(((self->unk14 - actor->unkC4) / 1000.0f) * 255.0f);
    fn_8013B608(&mtx.m[9], &p, 0x6B, 0xF, 6, 0, 0xFF, 0, 0, alpha & 0xFF);
    fn_8013B5D4();
}

// ---------------------------------------------------------------------------------------------
// 0x801FD704

void FearObjAdd(void* obj) {
    if (lbl_804D530C == 0) {
        return;
    }
    if (lbl_804D5310 >= 0x80) {
        return;
    }
    lbl_804D530C[lbl_804D5310].obj = obj;
    lbl_804D530C[lbl_804D5310++].count = 0;
}

// ---------------------------------------------------------------------------------------------
// 0x801FD748

static inline void FearRegionCount(int id) {
    int i;

    for (i = 0; i < lbl_804D5310; i++) {
        if (fn_8001A14C(lbl_804D530C[i].obj, id)) {
            lbl_804D530C[i].count++;
        }
    }
}

void FearRegionSetup(int unused, Actor* actor) {
    int found;
    int id;
    int asset;
    u32 hash;
    char* name;
    int n;
    u16* ids;

    ActorGetMatrix(&gFearMtx, actor);
    id = fn_8001A1B4((int*)&gFearMtx.m[9]);
    name = fn_8001A194(id);
    hash = StrHash(name);
    asset = AssetFind(0xD9044CBB, hash);

    found = 0;
    n = lbl_804D5318;
    ids = lbl_804D531C;
    for (; n > 0; n--) {
        if (ids[found] == asset) {
            u32 bit = 1 << (found & 31);

            if ((lbl_803B9FB8[found / 32] & bit) == 0) {
                lbl_803B9FB8[found / 32] |= bit;
                if (id != 0) {
                    FearRegionCount(id);
                }
                ModelNodeSetup(lbl_8049D2B0[0], asset, 1, 0);
            }
            goto done;
        }
        found++;
    }
    found = -1;

done:
    if (found != -1) {
        lbl_804D5300 = found;
    }
}

// ---------------------------------------------------------------------------------------------
// 0x801FD8F0

static inline void FearObjReleaseBit(u32 bit) {
    int i;

    if (lbl_804D52E8 & bit) {
        if (lbl_804D3378 != -1) {
            fn_8003D710(lbl_804D3378);
            for (i = 0; i < lbl_804D5310; i++) {
                if (lbl_804D530C[i].count != 0) {
                    fn_8003D710(*(int*)((char*)lbl_804D530C[i].obj + 0x90));
                }
            }
        }
        lbl_804D52E8 &= ~bit;
    }
}

void FearObjRelease(int idx) {
    FearObjReleaseBit(1 << idx);
}

// ---------------------------------------------------------------------------------------------
// Asset table entry for the rivet type.

FearRivetType gFearRivetDesc = {
    0x04A601EE, 0, FearRivetUpdate, 0, 0, 0, FearRivetDraw, 0, 0, 0,
};

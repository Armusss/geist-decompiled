// u_801FC634.cpp: the "Rivet" fear/panic actor (0x801FC634-0x801FD994).
// Keeps a global list of fear objects, sets up the fear regions and spawns/updates/draws the
// red rivets that fly out of a frightened actor.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>
#include "game/types.h"
#include "game/effect.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

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
    int unk1C;     // 0x1C
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


// ---------------------------------------------------------------------------------------------
// External functions.

extern "C" {
void* memset(void*, int, unsigned long);
int rand(void);
int SendMsg(void* to, int msg, void* data);
void FxStart(void* fx, int a, int b);

void fn_800E1098(void* out, void* in, float scale);
void fn_8016D440(Vec3* out, Vec3* in);
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
Vec3* PlayerGetGunVec(void* owner);

unsigned long AssetFind(unsigned long table, unsigned long key);
void ModelNodeSetup(int a, int b, int c, int d);
void ActorGetMatrix(Mtx34* mtx, Actor* actor);
unsigned long Fn80175D60(unsigned long id);

// ---------------------------------------------------------------------------------------------
// String hash (the shared n-Space helper).

inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    u32 h;
    if (s == 0) {
        h = 0;
    } else {
        int i;
        h = 0;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
    }
    return h;
}

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
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

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

void FearRivetNoop(void);
int FearRivetGetConst(void);
int FearRivetMsg(FearRivet* self, u32 msg, u32 id, SpawnInfo* info);
void FearRivetSpawn(u32 id, int mode, SpawnInfo* info, int arg);

// Effect type descriptor of the rivet.
EffectType gRivetEffectType = {
    "Rivet", 0, {0, 0, 0}, 0, 0,
    (int (*)(EffectObj*, int, int, void*, float))FearRivetMsg,
    (unsigned long (*)(EffectObj*, int))FearRivetGetConst,
    (void (*)(EffectObj*, float))FearRivetNoop,
    0, 0,
};

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
    float a1;
    float a2;

    EffectType* type = &gRivetEffectType;
    EffectTypeHash(type);

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
        vel = PlayerGetGunVec(t);
        {
            float x = vel->x;
            float y = vel->y;
            float z = vel->z;
            x *= 1000.0f;
            y *= 1000.0f;
            z *= 1000.0f;
            desc.offset.x += x;
            desc.offset.y += y;
            desc.offset.z += z;
        }
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

    int n = 3;
    n += actor->unk50;
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

// u_8018555C.cpp (0x8018555C-0x80185A9C), split from u_8017D794.
// Behaviour with descriptor 0x803A8738 (hash 0x90663AD4; fn_8018555C, fn_80185880, fn_8018595C, fn_801859A4).
// Own double 0x804D83D0. sdata2 0x804D83B8-0x804D83E8.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include "game/types.h"

struct Actor;

// Per-projectile state allocated by the start function (0x28 bytes).
struct ThrustState {
    u32 handle;           // 0x00
    Vec3 dir;             // 0x04
    Vec3 side;            // 0x10
    float delay;          // 0x1C
    int fired;            // 0x20
    float glow;           // 0x24
};

// The host object the behaviour runs on (only the fields used here).
struct BehHost {
    u8 pad00[0x24];
    int cell[3];          // 0x24
    u8 pad30[0x8];
    u32 handle;           // 0x38
    u8 pad3C[0x2C];
    Vec3 pos;             // 0x68
    Vec3 rot;             // 0x74
    u8 pad80[0x18];
    Vec3 vel;             // 0x98
    u8 padA4[0x10];
    Vec3 target;          // 0xB4
    u8 padC0[0x4];
    float age;            // 0xC4
    int link;             // 0xC8
    int id;               // 0xCC
    u8 padD0[0x10];
    int grenade;          // 0xE0
    u8 padE4[0x4];
    float scale;          // 0xE8
    u8 padEC[0x4];
    ThrustState* state;   // 0xF0
};

struct BehInst;

struct BehHooks {
    void* unk00[3];
    int (*blocked)(BehInst* beh, BehHost* host, void* hit);  // 0x0C
};

struct BehInst {
    int unk00;
    int countA;           // 0x04
    int countB;           // 0x08
    u8 pad0C[0x10];
    int unk1C;            // 0x1C
    u8 pad20[0x14];
    float unk34;          // 0x34
    float unk38;          // 0x38
    float unk3C;          // 0x3C
    float unk40;          // 0x40
    BehHooks* hooks;      // 0x44
};

struct SweepCtx {
    void* hit;
    int id;
};

void ActorGetMatrix(Mtx34* m, Actor* a);
int Fn800F8A98(unsigned long hash);
void Draw3DBegin(int texture, void* arg);
void DrawWorldQuadFlipped(float size, float angle, float* pos, u8 r, u8 g, u8 b, u8 a);
void Draw3DEnd(void);
void VFree(void* p);
void* VAlloc(int size);
int GrenadeRemove(int owner);
int GrenadeAdd(int owner, Vec3* pos);

extern "C" {
int fn_8001F928(void* obj, int* box, u32 mask, void* callback, void* ctx);
int fn_80049940();
void fn_80158C3C(Vec3* pos, u32 hash, int a, int b, int c, int d, int e);
void fn_80122108(float a, float b, float c, float d, float e, float f, int h, int id, Vec3* pos, int k, float g,
                 int id2);
void fn_8003F15C(int link, void* m, int a, int b, int c, float f);
int fn_80183DD0();
int fn_800F7994(int name, u32 hash);
void fn_8016D4EC(Vec3* out, Vec3* in);
void fn_800E1098(void* out, void* in, float length);

inline void Cross(Vec3* out, float ax, float ay, float az, float bx, float by, float bz) {
    out->x = ay * bz - az * by;
    out->y = az * bx - ax * bz;
    out->z = ax * by - ay * bx;
}

inline void VecScale(Vec3* out, Vec3* v, float k) {
    out->x = v->x * k;
    out->y = v->y * k;
    out->z = v->z * k;
}

inline void VecAdd(Vec3* out, Vec3* a, Vec3* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

bool fn_8018555C(void* behp, void* hostp) {
    ThrustState* st = ((BehHost*)hostp)->state;
    BehInst* beh = (BehInst*)behp;
    BehHost* host = (BehHost*)hostp;
    SweepCtx ctx;
    int box[6];
    int* b = box;
    u8 hit[0x48];
    float f;
    Vec3 t;

    if (st == 0) {
        return false;
    }
    ctx.hit = hit;
    ctx.id = host->id;
    box[0] = host->target.x;
    box[1] = host->target.y;
    box[2] = host->target.z;
    box[3] = host->pos.x;
    box[4] = host->pos.y;
    box[5] = host->pos.z;
    if (fn_8001F928(host, b, 0x12002000, fn_80183DD0, &ctx) != 0) {
        if (beh->hooks != 0 && beh->hooks->blocked != 0) {
            return beh->hooks->blocked(beh, host, hit) == 0;
        }
        host->pos.x = host->target.x;
        host->pos.y = host->target.y;
        host->pos.z = host->target.z;
        host->cell[0] = host->target.x;
        host->cell[1] = host->target.y;
        host->cell[2] = host->target.z;
        fn_80158C3C(&host->pos, 0x3BB8992E, -1, -1, -1, -1, -1);
        if (fn_80049940()) {
            f = beh->countB * host->scale;
        } else {
            f = beh->countA * host->scale;
        }
        fn_80122108(beh->unk34, beh->unk38, f, 0.0f, beh->unk3C, beh->unk40, 0, host->id, &host->pos, beh->unk1C,
                    500.0f, host->id);
        return false;
    }
    if (st->fired == 0 && host->age > st->delay) {
        st->fired = 1;
        VecScale(&t, &st->side, 10000.0f);
        VecAdd(&host->vel, &host->vel, &t);
        VecScale(&t, &st->dir, 200000.0f);
        VecAdd(&host->vel, &host->vel, &t);
    }
    if (st->fired != 0 && host->age > st->delay + 100.0f) {
        host->vel.x = 0.0f;
        host->vel.y = 0.0f;
        host->vel.z = 0.0f;
    }
    return true;
}

void fn_80185880(void* behp, void* hostp) {
    BehHost* host = (BehHost*)hostp;

    if (host->link != -1) {
        Mtx34 m;
        ThrustState* st;

        ActorGetMatrix(&m, (Actor*)host);
        fn_8003F15C(host->link, &m, 1, 0, 0, 1.0f);
        st = host->state;
        if (st != 0 && st->fired != 0 && st->glow > 0.0f) {
            Draw3DBegin(fn_800F7994(Fn800F8A98(0x13DC9), 0x9FE04B6C), 0);
            DrawWorldQuadFlipped(st->glow, 0.0f, &host->target.x, 0xFF, 0xFF, 0xFF, 0xFF);
            Draw3DEnd();
            st->glow -= 10.0f;
        }
    }
}

void fn_8018595C(void* behp, void* hostp) {
    BehHost* host = (BehHost*)hostp;

    if (host->state != 0) {
        VFree(host->state);
        host->state = 0;
    }
    GrenadeRemove(host->grenade);
}

int fn_801859A4(void* behp, void* hostp) {
    BehHost* host = (BehHost*)hostp;
    ThrustState* st;
    Vec3 dir;
    Vec3 rot;
    Vec3 a;
    float y, x, z;

    st = (ThrustState*)VAlloc(0x28);
    if (st == 0) {
        return 0;
    }
    host->state = st;
    rot = host->rot;
    fn_8016D4EC(&dir, &rot);
    fn_800E1098(&st->dir, &dir, 1.0f);
    st->handle = host->handle;
    y = st->dir.y;
    x = st->dir.x;
    z = st->dir.z;
    Cross(&a, x, y, z, 0.0f, 0.0f, 1.0f);
    Cross(&st->side, a.x, a.y, a.z, x, y, z);
    st->delay = 300.0f;
    st->fired = 0;
    st->glow = 250.0f;
    return GrenadeAdd(host->grenade, &host->pos);
}

// Behaviour descriptor: {hash, start, update, 0, 0, stop, draw, 0, 0, 0}.
struct BehDesc {
    u32 hash;
    int (*start)(void* beh, void* host);
    bool (*update)(void* beh, void* host);
    u32 unk0C;
    u32 unk10;
    void (*stop)(void* beh, void* host);
    void (*draw)(void* beh, void* host);
    u32 unk1C;
    u32 flags;
    u32 unk24;
};

BehDesc lbl_803A8738 = {
    0x90663AD4, fn_801859A4, fn_8018555C, 0, 0, fn_8018595C, fn_80185880, 0, 0, 0,
};
}

// u_80180F18.cpp (0x80180F18-0x80181644), split from u_8017D794.
// Behaviour with descriptor 0x803A8590 (hash 0xB7AAD36E; fn_80180F18, fn_801810E0, fn_8018136C, fn_80181390;
// fn_80181434 is a callback taken by fn_801810E0). Own double 0x804D82D8. sdata2 0x804D82C8-0x804D82E8.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

struct BehInst;

// The host object the behaviour runs on (only the fields used here).
struct BehHost {
    u8 pad00[0x24];
    int cell[3];          // 0x24
    u8 pad30[0x8];
    u32 handle;           // 0x38
    u8 pad3C[0x10];
    u32 flags;            // 0x4C
    u8 pad50[0x18];
    Vec3 pos;             // 0x68
    u8 pad74[0x40];
    Vec3 target;          // 0xB4
    u8 padC0[0xC];
    int id;               // 0xCC
    u8 padD0[0x18];
    float scale;          // 0xE8
    u8 padEC[0x4];
    int trail;            // 0xF0
};

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
    u8 pad20[0xC];
    int unk2C;            // 0x2C
    u8 pad30[0xC];
    float unk3C;          // 0x3C
    float unk40;          // 0x40
    BehHooks* hooks;      // 0x44
};

// An object the sweep touched (0x48 bytes).
struct HitTarget {
    u8 pad00[0x30];
    int (*handler)(int msg, void* data);  // 0x30
};

struct SweepHit {
    HitTarget* obj;       // 0x00
    int unk04;
    int unk08;
    int unk0C;            // 0x0C
    int unk10;            // 0x10
    int unk14;            // 0x14
    int unk18;            // 0x18
    int unk1C;
    int unk20;
    int unk24;
    int unk28[2];
    IVec3 pos;            // 0x30
    IVec3 normal;         // 0x3C
};

struct SweepCtx {
    SweepHit* hit;
    int id;
};

// Trail description passed to TrailCreate (constructed with the trail defaults).
struct TrailDesc {
    TrailDesc() {
        owner = -1;
        unk8 = 0;
        segments = 500;
        unk14 = 0.0f;
        unk20 = 1.0f;
        unk24 = 0.0f;
        unk28 = 60.0f;
        unk2C = 0.0f;
        texture = -3;
        color[0] = 0x7D;
        color[1] = 0x7D;
        color[2] = 0x7D;
    }

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

int TrailCreate(TrailDesc* desc);
void TrailDestroy(int trail);

extern "C" {
int SendMsg(int to, int msg, void* data);
int fn_8001F928(void* obj, int* box, u32 mask, void* callback, void* ctx);
int fn_80049940();
void fn_80158C3C(Vec3* pos, u32 hash, int a, int b, int c, int d, int e);
void fn_80122108(float a, float b, float c, float d, float e, float f, int h, int id, Vec3* pos, int k, float g,
                 int id2);
BehInst* fn_801869B8(BehHost* host);
void fn_801869D0(BehHost* host);
void fn_8016F120(int a, int* cell);
int fn_802A1D04();
int fn_80181434(SweepHit* hit, SweepCtx* ctx);

// Impact: snap to the target point, play the hit effect and spawn the damage blast.
inline void ImpactExplode(BehHost* host) {
    BehInst* beh = fn_801869B8(host);
    float f;

    fn_8016F120(beh->unk2C, host->cell);
    host->pos.x = host->target.x;
    host->pos.y = host->target.y;
    host->pos.z = host->target.z;
    host->cell[0] = host->target.x;
    host->cell[1] = host->target.y;
    host->cell[2] = host->target.z;
    fn_80158C3C(&host->pos, 0x040A726F, -1, -1, -1, -1, -1);
    if (fn_80049940()) {
        f = beh->countB * host->scale;
    } else {
        f = beh->countA * host->scale;
    }
    fn_80122108(500.0f, 1000.0f, f, f, beh->unk3C, beh->unk40, 0, host->id, &host->pos, beh->unk1C, 1000.0f,
                host->id);
    if (fn_802A1D04() == 3) {
        fn_80122108(0.0f, 1000.0f, 5680.0f, 0.0f, beh->unk3C, beh->unk40, 0, host->id, &host->pos, 0x403548D0,
                    1000.0f, host->id);
    }
}

int fn_80180F18(int msg, void* data) {
    BehHost* host = *(BehHost**)data;

    switch (msg) {
    case 1:
        return 1;
    case 0x407:
        ImpactExplode(host);
        fn_801869D0(host);
        return 1;
    }
    return 0;
}

bool fn_801810E0(void* behp, void* hostp) {
    BehInst* beh = (BehInst*)behp;
    BehHost* host = (BehHost*)hostp;
    SweepCtx ctx;
    int box[6];
    SweepHit hit;

    ctx.hit = &hit;
    ctx.id = host->id;
    box[0] = host->target.x;
    box[1] = host->target.y;
    box[2] = host->target.z;
    box[3] = host->pos.x;
    box[4] = host->pos.y;
    box[5] = host->pos.z;
    if (fn_8001F928(host, box, 0x12002000, fn_80181434, &ctx) != 0) {
        if (beh->hooks != 0 && beh->hooks->blocked != 0) {
            return beh->hooks->blocked(beh, host, &hit) == 0;
        }
        ImpactExplode(host);
        return false;
    }
    return true;
}

void fn_8018136C(void* behp, void* hostp) {
    TrailDestroy(((BehHost*)hostp)->trail);
}

bool fn_80181390(void* behp, void* hostp) {
    BehHost* host = (BehHost*)hostp;
    TrailDesc d;

    d.owner = host->handle;
    d.width = 60.0f;
    d.unk8 = -1;
    host->trail = TrailCreate(&d);
    host->flags |= 8;
    return true;
}

// Asks the target's 0x41E handler for the value stored under a name hash.
static inline void QueryNameOut(int* out, int target, unsigned long hash) {
    QueryMsg q;

    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    SendMsg(target, 0x41E, &q);
    *out = *(int*)&q.result;
}

// Sweep filter: skips the shooter, its owner and objects it owns; keeps the first other hit.
int fn_80181434(SweepHit* hit, SweepCtx* ctx) {

    if (hit->obj != 0) {
        Msg400 info;

        info.actor = (Actor*)hit->obj;
        hit->obj->handler(0x400, &info);
        if (info.state == 0 || info.ownerId == ctx->id) {
            return 0;
        }
        if ((int)((u32)info.ownerId >> 24) == 0x13) {
            int v;
            QueryNameOut(&v, ctx->id, 0x1C1A4CC9);
            if (v == info.ownerId) {
                return 0;
            }
            QueryNameOut(&v, info.ownerId, 0x1C1A4CC9);
            if (v == ctx->id) {
                return 0;
            }
        }
    }
    *ctx->hit = *hit;
    return 1;
}

// Behaviour descriptor: {hash, start, update, 0, 0, stop, 0, msg, 0, 0}.
struct BehDesc {
    u32 hash;
    bool (*start)(void* beh, void* host);
    bool (*update)(void* beh, void* host);
    u32 unk0C;
    u32 unk10;
    void (*stop)(void* beh, void* host);
    u32 unk18;
    int (*msg)(int msg, void* data);
    u32 unk20;
    u32 unk24;
};

BehDesc lbl_803A8590 = {
    0xB7AAD36E, fn_80181390, fn_801810E0, 0, 0, fn_8018136C, 0, fn_80180F18, 0, 0,
};
}

// u_80184CD4.cpp (0x80184CD4-0x80184F70), split from u_8017D794.
// Behaviour with descriptor 0x803A86E8 (hash 0x3BB8992E; fn_80184CD4, fn_80184F18). Own double 0x804D8398.
// sdata2 0x804D8390-0x804D83A8.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include "game/types.h"

struct Actor;

// The host object the behaviour runs on (only the fields used here).
struct BehHost {
    u8 pad00[0x24];
    int cell[3];          // 0x24
    u8 pad30[0x38];
    Vec3 pos;             // 0x68
    u8 pad74[0x40];
    Vec3 target;          // 0xB4
    u8 padC0[0x8];
    int link;             // 0xC8
    int id;               // 0xCC
    u8 padD0[0x18];
    float scale;          // 0xE8
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

extern "C" {
int fn_8001F928(void* obj, int* box, u32 mask, void* callback, void* ctx);
int fn_80049940();
void fn_80158C3C(Vec3* pos, u32 hash, int a, int b, int c, int d, int e);
void fn_80122108(float a, float b, float c, float d, float e, float f, int h, int id, Vec3* pos, int k, float g,
                 int id2);
void fn_8003F15C(int link, void* m, int a, int b, int c, float f);
int fn_80183DD0();

bool fn_80184CD4(void* behp, void* hostp) {
    BehInst* beh = (BehInst*)behp;
    BehHost* host = (BehHost*)hostp;
    SweepCtx ctx;
    int box[6];
    u8 hit[0x48];
    float f;

    ctx.hit = hit;
    ctx.id = host->id;
    box[0] = host->target.x;
    box[1] = host->target.y;
    box[2] = host->target.z;
    box[3] = host->pos.x;
    box[4] = host->pos.y;
    box[5] = host->pos.z;
    if (fn_8001F928(host, box, 0x12002000, fn_80183DD0, &ctx) != 0) {
        if (beh->hooks != 0 && beh->hooks->blocked != 0) {
            return beh->hooks->blocked(beh, host, hit) == 0;
        }
        host->pos.x = host->target.x;
        host->pos.y = host->target.y;
        host->pos.z = host->target.z;
        host->cell[0] = host->target.x;
        host->cell[1] = host->target.y;
        host->cell[2] = host->target.z;
        if (fn_80049940()) {
            f = beh->countB * host->scale;
        } else {
            f = beh->countA * host->scale;
        }
        fn_80158C3C(&host->pos, 0x3BB8992E, -1, -1, -1, -1, -1);
        fn_80122108(beh->unk34, beh->unk38, f, 0.0f, beh->unk3C, beh->unk40, 0, host->id, &host->pos, beh->unk1C,
                    500.0f, host->id);
        return false;
    }
    return true;
}

void fn_80184F18(void* behp, void* hostp) {
    BehHost* host = (BehHost*)hostp;

    if (host->link != -1) {
        Mtx34 m;
        ActorGetMatrix(&m, (Actor*)host);
        fn_8003F15C(host->link, &m, 1, 0, 0, 1.0f);
    }
}

// Behaviour descriptor (same layout as gFearRivetDesc): {hash, 0, update, 0, 0, 0, draw, 0, 0, 0}.
struct BehDesc {
    u32 hash;
    u32 unk04;
    bool (*update)(void* beh, void* host);
    u32 unk0C;
    u32 unk10;
    u32 unk14;
    void (*draw)(void* beh, void* host);
    u32 unk1C;
    u32 unk20;
    u32 unk24;
};

BehDesc lbl_803A86E8 = {
    0x3BB8992E, 0, fn_80184CD4, 0, 0, 0, fn_80184F18, 0, 0, 0,
};
}

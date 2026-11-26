// u_80184F70.cpp (0x80184F70-0x8018555C), split from u_8017D794.
// Behaviour with descriptor 0x803A8710 (hash 0x7F5A49DC; fn_80184F70, fn_80184F74, fn_80185500, fn_80185524).
// The empty fn_80184F70 sits in this descriptor, so the file starts there. Own double 0x804D83B0.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include "game/types.h"

struct Actor;

// The projectile object the behaviour runs on (only the fields used here).
struct BehHost {
    u8 pad00[0x68];
    Vec3 pos;             // 0x68
    u8 pad74[0xC];
    Vec3 vel;             // 0x80
    u8 pad8C[0x28];
    Vec3 prev;            // 0xB4
    u8 padC0[0xC];
    int id;               // 0xCC
    u8 padD0[0x18];
    float scale;          // 0xE8
    void* trail;          // 0xEC
};

struct BehInst {
    int unk00;
    int damageA;          // 0x04
    int damageB;          // 0x08
    float factorA;        // 0x0C
    float factorB;        // 0x10
    u8 pad14[0x4];
    float unk18;          // 0x18
    int unk1C;            // 0x1C
    float maxSpeed;       // 0x20
};

struct HitTarget {
    u8 pad00[0x30];
    int (*handler)(int msg, void* data);  // 0x30
    u8 pad34[0x4];
    u32 handle;           // 0x38: top byte is the object kind
};

// Result of a sweep (0x48 bytes).
struct SweepHit {
    HitTarget* obj;       // 0x00
    u8 pad04[0x8];
    int unk0C;            // 0x0C
    int unk10;            // 0x10
    int unk14;            // 0x14
    int unk18;            // 0x18
    u8 pad1C[0x14];
    IVec3 pos;            // 0x30
    IVec3 normal;         // 0x3C
};

struct SweepCtx {
    SweepHit* hit;
    int id;
};

// Message 0x407 sent to the object that was hit (0x50 bytes).
struct ProjHitMsg {
    HitTarget* obj;       // 0x00
    BehHost* host;        // 0x04
    int id;               // 0x08
    float damage;         // 0x0C
    int unk10;            // 0x10
    int unk14;            // 0x14
    IVec3 pos;            // 0x18
    IVec3 normal;         // 0x24
    IVec3 vel;            // 0x30
    int unk3C;            // 0x3C
    int unk40;            // 0x40
    int unk44;            // 0x44
    int unk48;            // 0x48
    float unk4C;          // 0x4C
    int unk50[3];         // 0x50
};

void ActorGetMatrix(Mtx34* m, Actor* a);
void MtxSetEuler(Mtx34* m, Vec3* rot);
void PlacementToMatrix(Mtx34* m, PlaceF* p);
void GunTrailStateStart8017AF78(void* block);
void GunTrailEntryInit8017AFA4(void* block, void* sample, float dt);
void* GunTrailSpawn8017B060(int effect);

extern "C" {
int fn_8001F928(void* obj, int* box, u32 mask, void* callback, void* ctx);
int fn_80049940();
void fn_800E0E9C(int* out, int* in, int scale);
void fn_800E1098(void* out, void* in, float length);
void fn_8016D440(Vec3* out, Vec3* in);
int fn_80183F60();

void fn_80184F70(void* behp, void* hostp) {
}

bool fn_80184F74(const BehInst* beh, BehHost* host, float dt) {
    SweepCtx ctx1;
    SweepCtx ctx2;
    Vec3 delta;
    Vec3 step;
    Vec3 euler;
    int box1[6];
    int box2[6];
    PlaceF place;
    SweepHit hit1;
    SweepHit hit2;
    float x, y, z;
    float s;
    float max;

    host->prev.x = host->pos.x;
    host->prev.y = host->pos.y;
    host->prev.z = host->pos.z;
    ctx1.id = host->id;
    ctx1.hit = &hit1;
    box1[0] = host->pos.x;
    box1[1] = host->pos.y;
    box1[2] = host->pos.z;
    box1[3] = host->vel.x;
    box1[4] = host->vel.y;
    box1[5] = host->vel.z;
    fn_800E0E9C(&box1[3], &box1[3], 0x2000);
    box1[3] += box1[0];
    box1[4] += box1[1];
    box1[5] += box1[2];
    if (fn_8001F928(host, box1, 0x1200A000, fn_80183F60, &ctx1) != 0) {
        x = ctx1.hit->pos.x;
        y = ctx1.hit->pos.y;
        z = ctx1.hit->pos.z;
    } else {
        x = box1[3];
        y = box1[4];
        z = box1[5];
    }
    delta.x = x - host->pos.x;
    delta.y = y - host->pos.y;
    delta.z = z - host->pos.z;
    fn_800E1098(&delta, &delta, 8192.0f * dt / 1000.0f);
    max = beh->maxSpeed;
    host->vel.x += delta.x;
    host->vel.y += delta.y;
    host->vel.z += delta.z;
    if (host->vel.z * host->vel.z + (host->vel.x * host->vel.x + host->vel.y * host->vel.y) > max * max) {
        fn_800E1098(&host->vel, &host->vel, max);
    }
    s = dt / 1000.0f;
    step.x = host->vel.x * s;
    step.y = host->vel.y * s;
    step.z = host->vel.z * s;
    host->pos.x = host->vel.x + host->pos.x;
    host->pos.y = host->vel.y + host->pos.y;
    host->pos.z = host->vel.z + host->pos.z;
    fn_8016D440(&euler, &step);
    MtxSetEuler((Mtx34*)&place, &euler);
    place.pos.x = host->pos.x;
    place.pos.y = host->pos.y;
    place.pos.z = host->pos.z;
    PlacementToMatrix((Mtx34*)host, &place);
    ctx2.hit = &hit2;
    ctx2.id = host->id;
    box2[0] = host->prev.x;
    box2[1] = host->prev.y;
    box2[2] = host->prev.z;
    box2[3] = host->pos.x;
    box2[4] = host->pos.y;
    box2[5] = host->pos.z;
    if (fn_8001F928(host, box2, 0x1200A000, fn_80183F60, &ctx2) != 0) {
        if (hit2.obj != 0) {
            ProjHitMsg msg;
            float damage;

            if (fn_80049940()) {
                damage = beh->damageB * host->scale;
                if ((int)(hit2.obj->handle >> 24) == 2) {
                    damage *= beh->factorB;
                }
            } else {
                damage = beh->damageA * host->scale;
                if ((int)(hit2.obj->handle >> 24) == 2) {
                    damage *= beh->factorA;
                }
            }
            msg.host = host;
            msg.unk14 = beh->unk1C;
            msg.damage = damage;
            msg.unk4C = beh->unk18;
            msg.vel.x = host->vel.x;
            msg.vel.y = host->vel.y;
            msg.vel.z = host->vel.z;
            msg.pos = hit2.pos;
            msg.unk48 = hit2.unk0C;
            msg.unk44 = hit2.unk14;
            msg.normal = hit2.normal;
            msg.unk40 = hit2.unk18;
            msg.unk3C = hit2.unk10;
            msg.obj = hit2.obj;
            msg.id = host->id;
            hit2.obj->handler(0x407, &msg);
        }
        return false;
    } else {
        ActorGetMatrix((Mtx34*)&place, (Actor*)host);
        GunTrailEntryInit8017AFA4(host->trail, &place, dt);
        return true;
    }
}

void fn_80185500(void* behp, void* hostp) {
    GunTrailStateStart8017AF78(((BehHost*)hostp)->trail);
}

bool fn_80185524(void* behp, void* hostp) {
    ((BehHost*)hostp)->trail = GunTrailSpawn8017B060(0);
    return true;
}

// Behaviour descriptor: {hash, start, update, 0, 0, stop, draw, 0, flags, 0}.
struct BehDesc {
    u32 hash;
    bool (*start)(void* beh, void* host);
    bool (*update)(const BehInst* beh, BehHost* host, float dt);
    u32 unk0C;
    u32 unk10;
    void (*stop)(void* beh, void* host);
    void (*draw)(void* beh, void* host);
    u32 unk1C;
    u32 flags;
    u32 unk24;
};

BehDesc lbl_803A8710 = {
    0x7F5A49DC, fn_80185524, fn_80184F74, 0, 0, fn_80185500, fn_80184F70, 0, 1, 0,
};
}

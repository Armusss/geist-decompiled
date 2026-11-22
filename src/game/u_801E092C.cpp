// u_801E092C.cpp (0x801E092C-0x801E0EDC): the "Lightning" effect type (descriptor .data 0x803ADDD0: name
// 0x80348DF8, fn_801E09C8, fn_801E0C78, fn_801E0DC8, fn_801E0D70), split from the old u_801D9A70 range.
// Boundary evidence: own .sdata2 pool 0x804D8D60-0x804D8D80 (int-to-float double 0x804D8D68, a duplicate of
// the trail file's 0x804D8D38); .bss 0x8049B3D8-0x8049B468 and .sdata 0x804D3280 read only by fn_801E092C,
// fn_801E0C78, fn_801E0D70 and fn_801E0DC8; .rodata "Lightning" 0x80348DF8. The next file (u_801E0EDC) has its
// own ghost-action name table at 0x80348E08.
// Names are descriptive guesses; unmatched against retail unless noted.

#include "game/types.h"
#include "game/string.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0


// One beam of u_8017D794.cpp (partial layout).
struct Beam {
    u8 pad00[0x80];
    int enabled;            // 0x80
    u8 pad84[0x94 - 0x84];
    int model;              // 0x94
    u8 pad98[0xA0 - 0x98];
    Vec3 start;             // 0xA0
    Vec3 end;               // 0xAC
};

// A bone record of the actor's skeleton (0x1C bytes); the world position is at +0x10.
struct LightningBone {
    u8 pad00[0x10];
    Vec3 pos;               // 0x10
};

struct LightningActor {
    u8 pad00[0x90];
    int model;              // 0x90
    u8 pad94[0x9C - 0x94];
    LightningBone* bones;   // 0x9C
    u8 padA0[4];
    Beam* beam;             // 0xA4
};

// The two bone ids the bolt runs between and the node the beams are tested against.
struct LightningNodes {
    u32 from;               // 0x00
    u32 to;                 // 0x04
    int node;               // 0x08
    int unk0C;              // 0x0C
};

void ListRemoveFree(void* p, void** list, u32* bits, int count);
u8* ModelAnimate(int model, char* name, int arg);
void ActorUpdate(struct Actor* a, float dt);
extern "C" {
void fn_8017E15C(Beam* b, int v);
void fn_8017DC64(Beam* b);
void fn_8017DF34(Beam* b, Vec3* from, Vec3* to, float dt);
int fn_800388A4(int model, int node);
}

extern Beam* lbl_8049B3D8[32];          // live bolts
extern LightningNodes lbl_8049B458;
extern u32 lbl_804D3280[2];             // bolt slots in use

//@@801E092C
// Ends every live bolt whose beam is enabled and touches the tracked node.
extern "C" void fn_801E092C(void) {
    for (int i = 0; i < 32; i++) {
        if ((lbl_804D3280[i / 32] & (1 << (i & 31))) != 0 && lbl_8049B3D8[i]->enabled != 0 &&
            fn_800388A4(lbl_8049B3D8[i]->model, lbl_8049B458.node) != 0) {
            fn_8017DC64(lbl_8049B3D8[i]);
        }
    }
}
//@@end

// TODO (not drafted yet, 0x2B0 bytes at 0x801E09C8)
extern "C" void fn_801E09C8(void);

//@@801E0C78
static inline void BoltFollow(LightningActor* a, Beam* b, float dt) {
    if (b != NULL && b->enabled != 0) {
        u8* map = ModelAnimate(a->model, 0, 0);
        b->start = a->bones[map[lbl_8049B458.from]].pos;
        b->end = a->bones[map[lbl_8049B458.to]].pos;
        fn_8017DF34(b, &b->start, &b->end, dt);
    }
}

extern "C" void fn_801E0C78(LightningActor* a, float dt) {
    BoltFollow(a, a->beam, dt);
    ActorUpdate((struct Actor*)a, dt);
}
//@@end

//@@801E0D70
// Destroy: releases the actor's bolt slot.
extern "C" void fn_801E0D70(LightningActor* a) {
    if (a != NULL) {
        fn_8017E15C(a->beam, 0);
        ListRemoveFree(a->beam, (void**)lbl_8049B3D8, lbl_804D3280, 0x20);
        a->beam = NULL;
    }
}
//@@end

// TODO (not drafted yet, 0x114 bytes at 0x801E0DC8)
extern "C" void fn_801E0DC8(void);

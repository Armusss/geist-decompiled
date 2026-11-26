// u_801E092C.cpp (0x801E092C-0x801E0EDC): the "Lightning" effect type (descriptor .data 0x803ADDD0: name
// 0x80348DF8, fn_801E09C8, fn_801E0C78, fn_801E0DC8, fn_801E0D70), split from the old u_801D9A70 range.
// Boundary evidence: own .sdata2 pool 0x804D8D60-0x804D8D80 (int-to-float double 0x804D8D68, a duplicate of
// the trail file's 0x804D8D38); .bss 0x8049B3D8-0x8049B468 and .sdata 0x804D3280 read only by fn_801E092C,
// fn_801E0C78, fn_801E0D70 and fn_801E0DC8; .rodata "Lightning" 0x80348DF8. The next file (u_801E0EDC) has its
// own ghost-action name table at 0x80348E08.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/actor.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0


// One beam of u_8017D794.cpp (partial layout).
struct Beam {
    u8 pad00[0x40];
    float width;            // 0x40
    float speed;            // 0x44
    float length;           // 0x48
    int segments;           // 0x4C
    u8 pad50[0x80 - 0x50];
    int enabled;            // 0x80
    u8 pad84[0x90 - 0x84];
    int style;              // 0x90
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
void* ListAlloc(void** list, int size, u32* bits, int count);
u32 AssetFind(u32 table, u32 key);
int ActorInitBase(struct Actor* a);
int ActorDefaultMsg(struct Actor* a, int msg, void* data, int arg);
u8* ModelAnimate(int model, char* name, int arg);
void ActorUpdate(struct Actor* a, float dt);
extern "C" {
void fn_8017E15C(Beam* b, int v);
void fn_8017E174(Beam* b, int v);
int fn_80019F88(void* data, LightningActor* a);
void fn_8017DC64(Beam* b);
void fn_8017DF34(Beam* b, Vec3* from, Vec3* to, float dt);
int fn_800388A4(int model, int node);
}

Beam* sLightningBolts[32];              // live bolts
LightningNodes sLightningNodes;
u32 sLightningSlots[1] = {0};           // bolt slots in use

//@@801E092C
// Ends every live bolt whose beam is enabled and touches the tracked node.
extern "C" void fn_801E092C(void) {
    for (int i = 0; i < 32; i++) {
        if ((sLightningSlots[i / 32] & (1 << (i & 31))) != 0 && sLightningBolts[i]->enabled != 0 &&
            fn_800388A4(sLightningBolts[i]->model, sLightningNodes.node) != 0) {
            fn_8017DC64(sLightningBolts[i]);
        }
    }
}
//@@end

// Message 0xC14 (beam control) and 0x41D (set a beam variable): the hash and value words.
struct LightningMsg {
    LightningActor* actor;  // 0x00
    u32 var;                // 0x04: 0x41D variable hash
    int unk08;              // 0x08
    int value;              // 0x0C: 0xC14 command hash / 0x41D value
};

static inline int BeamCommand(LightningActor* a, LightningMsg* m) {
    Beam* b = a->beam;
    if (b == NULL) {
        return 0;
    }
    switch (m->value) {
    case 0x7850684C:
        fn_8017E15C(b, 1);
        return 1;
    case 0xBD60EC0C:
        fn_8017E15C(b, 0);
        return 1;
    case 0xCC0A97EA:
        b->style = 0;
        break;
    case 0x827330FB:
        b->style = 1;
        break;
    case 0x171BB9FC:
        b->style = 2;
        break;
    case 0x85A06832:
        b->style = 3;
        break;
    }
    return 0;
}

static inline int BeamSetVar(LightningActor* a, LightningMsg* m) {
    Beam* b;
    if (fn_80019F88(m, a) != 0) {
        return 1;
    }
    b = a->beam;
    if (b == NULL) {
        return 0;
    }
    switch (m->var) {
    case 0x829FA6E8:
        if (m->value >= 0) {
            b->width = (float)m->value / 100.0f;
            return 1;
        }
        break;
    case 0x875820C8:
        if (m->value >= 0) {
            b->speed = (float)m->value / 100.0f;
            return 1;
        }
        break;
    case 0xE0E8CE40:
        if (m->value >= 0) {
            b->length = (float)m->value;
            return 1;
        }
        break;
    case 0xD004D547:
        if (m->value > 0) {
            b->segments = m->value;
            return 1;
        }
        break;
    }
    return 0;
}

//@@801E09C8
// Message handler: beam commands (0xC14) and beam variables (0x41D), then the default actor messages.
extern "C" int fn_801E09C8(struct Actor* self, int msg, void* data, int arg) {
    LightningActor* a = ((LightningMsg*)data)->actor;
    switch (msg) {
    case 0xC14:
        if (BeamCommand(a, (LightningMsg*)data)) {
            return 1;
        }
        break;
    case 0x41D:
        if (BeamSetVar(a, (LightningMsg*)data)) {
            return 1;
        }
        break;
    }
    return ActorDefaultMsg(self, msg, data, arg);
}
//@@end

//@@801E0C78
// Update: moves the live beam between the two tracked bones, then the default actor update.
// The positions are copied field by field: a Vec3 struct assignment gives the same code but makes CodeWarrior
// emit this function after the rest of the file.
extern "C" void fn_801E0C78(LightningActor* a, float dt) {
    void* p = a->beam;
    if (p != NULL) {
        Beam* b = (Beam*)p;
        if (b->enabled != 0) {
            u8* map = ModelAnimate(a->model, 0, 0);
            Vec3* v = &a->bones[map[sLightningNodes.from]].pos;
            b->start.x = v->x;
            b->start.y = v->y;
            b->start.z = v->z;
            v = &a->bones[map[sLightningNodes.to]].pos;
            b->end.x = v->x;
            b->end.y = v->y;
            b->end.z = v->z;
            fn_8017DF34(b, &b->start, &b->end, dt);
            ActorUpdate((struct Actor*)a, dt);
        }
    }
}
//@@end

//@@801E0D70
// Destroy: releases the actor's bolt slot.
extern "C" void fn_801E0D70(LightningActor* a) {
    if (a != NULL) {
        fn_8017E15C(a->beam, 0);
        ListRemoveFree(a->beam, (void**)sLightningBolts, sLightningSlots, 0x20);
        a->beam = NULL;
    }
}
//@@end

//@@801E0DC8
// Init: looks up the bolt's bone ids and the tracked node, then gives the actor a beam.
extern "C" int fn_801E0DC8(LightningActor* a) {
    Beam* b;
    sLightningNodes.from = AssetFind(0xF1754BF2, 0x04B8CC42);
    sLightningNodes.to = AssetFind(0xF1754BF2, 0x00010CBB);
    sLightningNodes.node = AssetFind(0xD9044CBB, 0xCF0262AA);
    if (ActorInitBase((struct Actor*)a) == 0) {
        return 0;
    }
    if (a->beam == NULL) {
        b = (Beam*)ListAlloc((void**)sLightningBolts, 0xB8, sLightningSlots, 0x20);
        if (b == NULL) {
            return 0;
        }
        a->beam = b;
    }
    fn_8017E174(b, 2);
    b->start.x = 0.0f;
    b->start.y = 0.0f;
    b->start.z = 0.0f;
    b->end.x = 0.0f;
    b->end.y = 0.0f;
    b->end.z = 0.0f;
    b->model = a->model;
    return 1;
}
//@@end

ActorType gLightningType = {
    0, "Lightning", (ActorMsgFunc)fn_801E09C8, (void (*)(Actor*, float))fn_801E0C78,
    (int (*)(Actor*))fn_801E0DC8, 0, (void (*)(Actor*))fn_801E0D70, {0, 0, 0, 0, 0},
};

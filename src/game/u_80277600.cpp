// u_80277600.cpp: ODE 0.5 ode.cpp (0x802775AC-0x80278D2C): world, joint and body API in reverse source order
// (dWorld* first). "SceneNode" is ODE's dxBody (pos at 0xE8, lvel 0xF8, facc 0x108, tacc 0x118); its accessor
// names are the earlier descriptive guesses. Own .sdata2 pool 0x804DA2F8-0x804DA318 (0.2f, ..., FLT_MAX, 1.0f).
// Split (2026-11-26, data evidence in each file's header): u_80278D2C (math), u_80279558 (step.cpp),
// u_8027B4F8, u_8027BD5C (util.cpp), u_8027C650 (game code).

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

struct ListLink {
    int value;              // 0x00
    int unk04;              // 0x04
    ListLink* next;         // 0x08
};

struct SceneNode {
    u8 pad00[0xC];
    u32 owner;              // 0x00C
    u8 pad10[4];
    ListLink* list;         // 0x014
    u32 flags;              // 0x018
    u8 pad1C[0xCC];
    Vec3 pos;               // 0x0E8
    u8 padF4[4];
    Vec3 vel;               // 0x0F8
    u8 pad104[4];
    Vec3 accel;             // 0x108
    u8 pad114[4];
    Vec3 rotAdd;            // 0x118
    u8 pad124[0x1C];
    float prevA;            // 0x140
    float prevB;            // 0x144
    float curA;             // 0x148
    float curB;             // 0x14C
};

// ODE 0.5 ode.cpp world functions (0x802775AC-0x80277600; reverse source order, as the ODE library was
// built with -inline deferred). dxWorld layout from ODE 0.5 objects.h.
struct dxWorld {
    void* firstbody;     // 0x00
    void* firstjoint;    // 0x04
    int nb, nj;          // 0x08
    float gravity[4];    // 0x10
    float global_erp;    // 0x20
    float global_cfm;    // 0x24
    float adis[4];       // 0x28 dxAutoDisable
    int adis_flag;       // 0x38
    int qs_num_iterations;  // 0x3C dxQuickStepParameters
    float qs_w;          // 0x40
    float contactp[2];   // 0x44 dxContactParameters
};
typedef dxWorld* dWorldID;
typedef void dstepper_fn_t(dxWorld* world, void* body, int nb, void* joint, int nj, float stepsize);
extern "C" void fn_80279558(void);  // dInternalStepIsland (step.cpp)
extern "C" void fn_8027BD5C(void);  // dxProcessIslands (util.cpp)
void dxProcessIslands(dxWorld* world, float stepsize, dstepper_fn_t* stepper);
void dInternalStepIsland(dxWorld* world, void* body, int nb, void* joint, int nj, float stepsize);

void dWorldSetQuickStepNumIterations(dWorldID w, int num) {
    w->qs_num_iterations = num;
}

void dWorldSetAutoDisableFlag(dWorldID w, int do_auto_disable) {
    w->adis_flag = (do_auto_disable != 0);
}

void dWorldStep(dWorldID w, float stepsize) {
    dxProcessIslands(w, stepsize, &dInternalStepIsland);
}

void dWorldSetGravity(dWorldID w, float x, float y, float z) {
    w->gravity[0] = x;
    w->gravity[1] = y;
    w->gravity[2] = z;
}

// TODO (not drafted yet, 0xD4 bytes at 0x80277600)
extern "C" void fn_80277600(void);

// TODO (not drafted yet, 0xA4 bytes at 0x802776D4)
extern "C" void fn_802776D4(void);

// TODO (not drafted yet, 0x44 bytes at 0x80277778)
extern "C" void fn_80277778(void);

// TODO (not drafted yet, 0x48 bytes at 0x802777BC)
extern "C" void fn_802777BC(void);

// TODO (not drafted yet, 0x154 bytes at 0x80277804)
extern "C" void fn_80277804(void);

// TODO (not drafted yet, 0x1E0 bytes at 0x80277958)
extern "C" void fn_80277958(void);

// TODO (not drafted yet, 0x200 bytes at 0x80277B38)
extern "C" void fn_80277B38(void);

// TODO (not drafted yet, 0x48 bytes at 0x80277D38)
extern "C" void fn_80277D38(void);

// TODO (not drafted yet, 0x148 bytes at 0x80277D80)
extern "C" void fn_80277D80(void);

// TODO (not drafted yet, 0x204 bytes at 0x80277EC8)
extern "C" void fn_80277EC8(void);

// TODO (not drafted yet, 0x148 bytes at 0x802780CC)
extern "C" void fn_802780CC(void);

// TODO (not drafted yet, 0x148 bytes at 0x80278214)
extern "C" void fn_80278214(void);

void SceneNodeSetPinned(SceneNode* n, int pinned) {
    if (pinned != 0) {
        n->flags &= ~8;
        return;
    }
    n->flags |= 8;
}

u8 SceneNodeNotLocked(SceneNode* n) {
    return (n->flags >> 2 & 1) ^ 1;
}

void SceneNodeLock(SceneNode* n) {
    n->flags |= 4;
}

void SceneNodeSnapPrev(SceneNode* n) {
    n->flags &= ~4;
    n->curB = n->prevB;
    n->curA = n->prevA;
}

int SceneNodeListGet(SceneNode* n, int index) {
    ListLink* l = n->list;
    int i = 0;
    for (; l != NULL; l = l->next, i++) {
        if (i == index)
            return l->value;
    }
    return 0;
}

int SceneNodeListCount(SceneNode* n) {
    int count = 0;
    ListLink* l;
    for (l = n->list; l != NULL; l = l->next)
        count++;
    return count;
}

// TODO (not drafted yet, 0x118 bytes at 0x8027841C)
extern "C" void fn_8027841C(void);

// TODO (not drafted yet, 0xC4 bytes at 0x80278534)
extern "C" void fn_80278534(void);

// TODO (not drafted yet, 0xAC bytes at 0x802785F8)
extern "C" void fn_802785F8(void);

// TODO (not drafted yet, 0x7C bytes at 0x802786A4)
extern "C" void fn_802786A4(void);

// TODO (not drafted yet, 0x70 bytes at 0x80278720)
extern "C" void fn_80278720(void);

// TODO (not drafted yet, 0x70 bytes at 0x80278790)
extern "C" void fn_80278790(void);

void SceneNodeAddRot(float x, float y, float z, SceneNode* n) {
    n->rotAdd.x += x;
    n->rotAdd.y += y;
    n->rotAdd.z += z;
}

void SceneNodeAddAccel(float x, float y, float z, SceneNode* n) {
    n->accel.x += x;
    n->accel.y += y;
    n->accel.z += z;
}

// TODO (not drafted yet, 0x64 bytes at 0x80278850)
extern "C" void fn_80278850(void);

Vec3* SceneNodeVelocityPtr(SceneNode* n) {
    return &n->vel;
}

Vec3* SceneNodePosPtr(SceneNode* n) {
    return &n->pos;
}

void SceneNodeSetVelocity(float x, float y, float z, SceneNode* n) {
    n->vel.x = x;
    n->vel.y = y;
    n->vel.z = z;
}

void SceneNodeSetPos(float x, float y, float z, SceneNode* n) {
    n->pos.x = x;
    n->pos.y = y;
    n->pos.z = z;
}

// TODO (not drafted yet, 0x88 bytes at 0x802788E4)
extern "C" void fn_802788E4(void);

// TODO (not drafted yet, 0x54 bytes at 0x8027896C)
extern "C" void fn_8027896C(void);

u32 SceneNodeGetOwner(SceneNode* n) {
    return n->owner;
}

void SceneNodeSetOwner(SceneNode* n, u32 owner) {
    n->owner = owner;
}

// TODO (not drafted yet, 0x1A0 bytes at 0x802789D0)
extern "C" void fn_802789D0(void);

// TODO (not drafted yet, 0x1BC bytes at 0x80278B70)
extern "C" void fn_80278B70(void);

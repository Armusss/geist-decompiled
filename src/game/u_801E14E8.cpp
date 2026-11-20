// u_801E14E8.cpp (0x801E14E8-0x801E241C): the ghost barrier actor (message, update, kind registry,
// init/free) and its type descriptor gGhostBarrierType.
// Boundary evidence (the cut at 0x801E14E8, split from the old u_801E0EDC range):
// - .sdata2: this file's pool 0x804D8D80-0x804D8DB0 starts with its own 1.0f (BarrierActorMsg/Update), a
//   second copy of the 1.0f at 0x804D8D78 that the registry file (u_801E0EDC.cpp, fn_801E0EDC) reads; CW
//   merges identical literals within one file, so these are two files. The pool ends with the int-to-float
//   double 0x804D8DA8.
// - .rodata 0x80348E08-0x80348EA0: the shared ghost-action name strings (POSSESS..SCAN, a header table) then
//   "Ghost Barrier", the effect/sound names and "BarrierPass_NPC"; the next file starts its own copy of the
//   action names at 0x80348EA0.
// - .data 0x803ADE38-0x803ADF68: the 64-slot actor-kind table (all -1 in retail) and gGhostBarrierType.
// - .sdata 0x804D3288-0x804D32A0: the six asset/effect handles, -1 in retail, used only here.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/msg.h"

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// The 64 actor-kind hashes the barrier reacts to; -1 marks a free slot (0x803ADE38).
int lbl_803ADE38[64] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};


// Actor type descriptor of the barrier (0x803ADF38).
struct BarrierActorType {
    int unk0;
    const char* name;
    int (*msg)(Actor*, int, void*, int);
    void (*update)(void*, float);
    u32 (*init)(void*);
    void* unk14;
    void (*free)(void*);
    void* unk1C[5];
};

int BarrierActorMsg(Actor* actor, int msg, void* data, int arg);
void BarrierActorUpdate(void* actorPtr, float delta);
u32 BarrierActorInit(void* actorPtr);
void BarrierBehaviourFree(void* actorPtr);

BarrierActorType gGhostBarrierType = {
    0, "Ghost Barrier", BarrierActorMsg, BarrierActorUpdate, BarrierActorInit, 0, BarrierBehaviourFree,
};

// ---------------------------------------------------------------------------
// Barrier actor. The behaviour record behind Actor::m_a4 holds a small state
// machine (0 closed, 1 opening, 2 open, 3 closing) with a timer, two flag bits
// and the handle of the running effect. Names are descriptive guesses.
// ---------------------------------------------------------------------------

struct BarrierBeh {
    int state;        // 0x00
    int field04;      // 0x04
    u32 flags;        // 0x08: 0x1 = armed, 0x2 = enabled, 0x4 = animate
    float timer;      // 0x0C
    int fx;           // 0x10: effect handle, -1 when idle
    int field14;      // 0x14
};

class Actor;

struct BarrierActor {
    char pad00[0x24];
    int field24;              // 0x24
    char pad28[0x10];
    u32 field38;              // 0x38
    char pad3C[4];
    int field40;              // 0x40
    char pad44[8];
    u32 field4C;              // 0x4C
    char pad50[0x40];
    int model;                // 0x90
    char pad94[0x10];
    BarrierBeh* behaviour;    // 0xA4
    char padA8[4];
    u32 fieldAC;              // 0xAC
    char padB0[0x1438];
    float field14E8;          // 0x14E8
};

// View of an object that answers the "0x400" virtual query (vtable +0x30).
struct BarrierMsgSource {
    virtual void Unk08();
    virtual void Unk0C();
    virtual void Unk10();
    virtual void Unk14();
    virtual void Unk18();
    virtual void Unk1C();
    virtual void Unk20();
    virtual void Unk24();
    virtual void Unk28();
    virtual void Unk2C();
    virtual int Apply(int cmd, void* arg);  // 0x30
};

int lbl_804D3288 = -1;
int lbl_804D328C = -1;
int lbl_804D3290 = -1;
int lbl_804D3294 = -1;
int lbl_804D3298 = -1;
int lbl_804D329C = -1;

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int fn_8005AAF8(int a);
extern "C" int fn_80019F88(void* data, void* actor);
void PoolFree(void* block, void* pool, int size, void* param);
extern unsigned char lbl_8049B468[];
extern unsigned char lbl_8049B868[];
extern "C" int fn_80038474(int model, int node, int a, int b);
extern "C" void fn_8016D864(void* out, int src);
extern "C" int fn_8016F120(int fx, void* pos);
extern "C" void fn_8016EEF4(int fx);
extern "C" void fn_80037EC0(int model, int node, int a, int b, int c, int d, int e);
int BarrierKindRegistryMsg(void* actor, void* data);
extern "C" int rand(void);

void ActorGetMatrix(Mtx34* out, Actor* actor);
int ModelBoneHitBox(int a, int b, int c, int d);
int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg);
int ActorInitBase(Actor* actor);
void* PoolAlloc(void* pool, int size, void* pool2, int size2);
extern "C" void fn_80088C10(int fx, const char* name, int a, int b, int c, int d, int e, int f);
extern "C" void fn_80088F0C(int fx, int enable);
extern "C" void fn_80088FB0(int fx, int enable);
int PlayerGetPossessor(int obj);
void* AssetFind(u32 a, u32 b);
int ModelFindBone(int model, int node);
int PlayerInHost(Actor* actor);
float fn_80017084(float v);
int FxFind(const char* name);
int ModelNodeSetup(int model, int node, int a, int b);
void ActorUpdate(Actor* actor, float delta);

// The 64 actor-kind hashes the barrier reacts to.
static inline int BarrierKindIsValid(int kind) {
    for (int i = 0; i < 64; i++) {
        if (kind == lbl_803ADE38[i]) {
            return 1;
        }
    }
    return 0;
}

// The object that walks through the barrier (a player or host): it answers
// message 0x400 through its handler at +0x30.
struct BarrierVisitor {
    char pad00[0x30];
    int (*handler)(int msg, void* data);  // 0x30
    char pad34[0x14E8 - 0x34];
    float field14E8;                      // 0x14E8
};

// Whether the visitor may pass: a ghost (kind 0x13) always may, a player
// only when not inside a host (a hosted player is flagged instead).
static inline int BarrierVisitorPasses(BarrierBeh* beh, BarrierVisitor* target) {
    if (target != 0 && beh != 0 && beh->field04 != 0) {
        Msg400 q;
        q.actor = (Actor*)target;
        if (target->handler(0x400, &q) != 0) {
            switch ((int)((u32)q.ownerId >> 24)) {
            case 2:
                if (PlayerInHost((Actor*)target) != 0) {
                    target->field14E8 = 1.0f;
                    return 0;
                }
                return 1;
            case 0x13:
                return 1;
            }
        }
    }
    return 0;
}

static inline bool BarrierBoneDiffers(BarrierActor* self, int* data) {
    bool differs = ModelFindBone(self->model, lbl_804D328C) != data[5];
    return differs;
}

// Asks the target a keyed question (message 0x41E); returns the first word
// of the reply.
static inline int BarrierAsk(int target, u32 hash) {
    QueryMsg q;
    q.hash = hash;
    q.unk8 = 0;
    *(int*)&q.result = 0;
    SendMsg(target, 0x41E, &q);
    return *(int*)&q.result;
}

static inline int BarrierAnswer41E(int* data) {
    switch (data[1]) {
    case (int)0x914B98A8u:
        data[3] = 1;
        return 1;
    }
    return 0;
}
// Message handler of the barrier actor (states, hit tests, effect spawning).
// Descriptive name: BarrierActorMsg.
int BarrierActorMsg(Actor* actor, int msg, void* dataPtr, int arg) {
    BarrierActor* self = *(BarrierActor**)dataPtr;
    int* data = (int*)dataPtr;
    int result;
    int target;

    switch (msg) {
    case 0x405: {
        int hit;
        int result = 0;
        if ((int)(((BarrierActor*)data[1])->field38 >> 24) == 0x13) {
            Mtx34 otherMtx;
            Mtx34 selfMtx;
            int otherBox[6];
            int selfBox[6];
            hit = 0;
            ActorGetMatrix(&selfMtx, (Actor*)self);
            ActorGetMatrix(&otherMtx, (Actor*)data[1]);
            fn_8016D864(selfBox, ((BarrierActor*)data[0])->field40);
            fn_8016D864(otherBox, ((BarrierActor*)data[1])->field40);
            if (ModelBoneHitBox((int)selfBox, (int)&selfMtx, (int)otherBox, (int)&otherMtx) != 0) {
                hit = 1;
            }
            if ((data[4] = hit) != 0) {
                result = hit;
                fn_8016F120(lbl_804D329C, &self->field24);
            }
        }
        return result;
    }
    case 0x40A: {
        BarrierBeh* beh = self->behaviour;
        if (beh != 0 && (u32)data[2] == 0x7C44EC3D && (beh->flags & 1) != 0) {
            if (BarrierAsk(target = data[1], 0x947E97) != 0) {
                data[3] = (int)0xF21581DEu;
                int kind = data[1];
                if ((int)((u32)kind >> 24) == 2) {
                    kind = fn_8005AAF8(PlayerGetPossessor(kind));
                }
                struct {
                    int unk0;
                    int enabled;
                    int id;
                    void* asset;
                    int unk10;
                } reply;
                if (BarrierKindIsValid(kind)) {
                    reply.enabled = 0;
                } else {
                    reply.enabled = 0;
                }
                reply.id = data[3];
                reply.asset = AssetFind(0xC0A77187u, 0x25A645u);
                SendMsg(data[1], 0x40B, &reply);
            }
        }
        return 1;
    }
    case 0x409: {
        BarrierBeh* beh = self->behaviour;
        if (beh != 0) {
            int kind = data[2];
            if ((int)((u32)kind >> 24) == 2) {
                kind = fn_8005AAF8(PlayerGetPossessor(kind));
            }
            if (!BarrierKindIsValid(kind)) {
                if ((beh->flags & 2) != 0) {
                    beh->flags &= ~2u;
                } else {
                    beh->flags |= 2;
                }
            }
        }
        return 1;
    }
    case 0x41E:
        if (self->behaviour == 0) {
            return 0;
        }
        return BarrierAnswer41E(data);
    case 0x41D:
        BarrierKindRegistryMsg(self, data);
        return 1;
    case 2:
        result = ActorDefaultMsg(actor, msg, data, arg);
        if (result != 0) {
            target = data[3];
            if (BarrierVisitorPasses(self->behaviour, (BarrierVisitor*)target)) {
                return result;
            }
        }
        return 0;
    case 3:
        result = ActorDefaultMsg(actor, msg, data, arg);
        if (result != 0) {
            target = data[4];
            if (BarrierVisitorPasses(self->behaviour, (BarrierVisitor*)target)) {
                return result;
            }
        }
        return 0;
    case 4:
        result = ActorDefaultMsg(actor, msg, data, arg);
        if (result != 0) {
            target = data[7];
            if (BarrierVisitorPasses(self->behaviour, (BarrierVisitor*)target)) {
                return result;
            }
        }
        return 0;
    case 1: {
        int result = ActorDefaultMsg(actor, msg, data, arg);
        if (result != 0 && BarrierBoneDiffers(self, data)) {
            return result;
        }
        return 0;
    }
    }
    return ActorDefaultMsg(actor, msg, data, arg);
}

// Per-frame update of the barrier actor: runs the open/close state machine,
// drives the node colour and effect, then chains to ActorUpdate.
// Descriptive name: BarrierActorUpdate.
void BarrierActorUpdate(void* actorPtr, float delta) {
    BarrierBeh* beh;
    BarrierActor* actor = (BarrierActor*)actorPtr;
    beh = actor->behaviour;

    if (beh != 0) {
        // "level" scales the barrier effect; it starts at its maximum.
        float level = 1.0f;

        switch (beh->state) {
        case 0: {
            level = 0.0f;
            if ((beh->flags & 2) != 0) {
                if ((beh->flags & 4) != 0) {
                    beh->timer -= delta;
                } else {
                    beh->timer = level;
                }
                if (beh->timer <= 0.0f) {
                    beh->state = 1;
                    beh->timer = 100.0f;
                    ModelNodeSetup(actor->model, lbl_804D3288, 1, 0);
                    fn_80038474(actor->model, lbl_804D328C, 1, 0);
                }
            }
            break;
        }
        case 1: {
            beh->timer -= delta;
            level = level - 100.0f * beh->timer;
            if (beh->timer <= 0.0f) {
                beh->state = 2;
                beh->timer = 2500.0f * ((float)rand() / 32767.0f) + 500.0f;
                beh->fx = fn_8016F120(FxFind("g_barriers"), &actor->field24);
                fn_8016F120(FxFind("g_barriers_on"), &actor->field24);
            }
            break;
        }
        case 2: {
            if ((beh->flags & 2) != 0) {
                if ((beh->flags & 4) != 0) {
                    float speed = fn_80017084(beh->timer);
                    level = 0.15f * speed + 0.85f;
                    beh->timer -= delta;
                    if (beh->timer <= 0.0f) {
                        beh->state = 3;
                        beh->timer = 100.0f;
                    }
                }
            } else {
                beh->state = 3;
                beh->timer = 100.0f;
            }
            break;
        }
        case 3: {
            beh->timer -= delta;
            level = 100.0f * beh->timer;
            if (beh->timer <= 0.0f) {
                beh->state = 0;
                beh->timer = 100.0f;
                if (beh->fx != -1) {
                    fn_8016F120(FxFind("g_barriers_off"), &actor->field24);
                    fn_8016EEF4(beh->fx);
                    beh->fx = -1;
                }
                ModelNodeSetup(actor->model, lbl_804D3288, 0, 0);
                fn_80038474(actor->model, lbl_804D328C, 0, 0);
            }
            break;
        }
        }

        beh->field04 = (int)((5.0f * delta) * (level < 0.0f ? 0.0f : (level > 1.0f ? 1.0f : level)));
        fn_80037EC0(actor->model, lbl_804D3288, 1, 100, 60, 140,
                    (int)(255.0f * level) & 0xFF);
        if ((beh->flags & 2) != 0) {
            fn_80037EC0(actor->model, lbl_804D3294, 1, 0xFF, 0xFF, 0xFF, 0xFF);
            fn_80037EC0(actor->model, lbl_804D3298, 1, 0xFF, 0xFF, 0xFF, 0xFF);
        } else {
            fn_80037EC0(actor->model, lbl_804D3294, 1, 0x40, 0x40, 0x40, 0xFF);
            fn_80037EC0(actor->model, lbl_804D3298, 1, 0x40, 0x40, 0x40, 0xFF);
        }
    }
    ActorUpdate((Actor*)actor, delta);
}

// Message 0x41D handler: keeps the 64-slot table of actor-kind hashes the
// barrier reacts to (-1 marks a free slot). Descriptive name.
int BarrierKindRegistryMsg(void* actorPtr, void* dataPtr) {
    BarrierActor* actor = (BarrierActor*)actorPtr;
    int* data = (int*)dataPtr;
    BarrierBeh* beh = actor->behaviour;

    if (fn_80019F88(dataPtr, actorPtr) != 0) {
        return 1;
    }
    if (beh != 0) {
        switch (data[1]) {
        case 0x1314F:
            beh->flags &= ~2u;
            return 1;
        case 0x9DF:
            beh->flags |= 2;
            return 1;
        case (int)0xFCC8CDD8u:
            if (data[3] != 0) {
                beh->flags |= 4;
            } else {
                beh->flags &= ~4u;
            }
            beh->timer = 100.0f;
            return 1;
        case 0x1E417DFF: {
            int firstFree = -1;
            for (int i = 0; i < 64; i++) {
                if (data[3] == lbl_803ADE38[i]) {
                    return 1;
                }
                if (firstFree == -1 && lbl_803ADE38[i] == -1) {
                    firstFree = i;
                }
            }
            if (firstFree == -1) {
                return 1;
            }
            lbl_803ADE38[firstFree] = data[3];
            return 1;
        }
        case 0x5E1B40B7:
            for (int i = 0; i < 64; i++) {
                if (data[3] == lbl_803ADE38[i]) {
                    lbl_803ADE38[i] = -1;
                    return 1;
                }
            }
            return 1;
        }
    }
    return 0;
}

// Releases the barrier behaviour record back to its pool and clears the
// actor's handle to it. Descriptive name.
void BarrierBehaviourFree(void* actorPtr) {
    BarrierActor* actor = (BarrierActor*)actorPtr;
    PoolFree(actor->behaviour, lbl_8049B468, 0x20, lbl_8049B868);
    actor->behaviour = 0;
}

// Initialises the barrier actor: allocates its behaviour record, resolves the
// shared asset handles and spawns the closed-barrier effect.
// Descriptive name: BarrierActorInit.
u32 BarrierActorInit(void* actorPtr) {
    BarrierActor* actor = (BarrierActor*)actorPtr;
    BarrierBeh* beh;

    if (!ActorInitBase((Actor*)actor)) {
        return 0;
    }
    if (actor->behaviour == 0) {
        beh = (BarrierBeh*)PoolAlloc(lbl_8049B468, 0x20, lbl_8049B868, 0x20);
        if (beh == 0) {
            return 0;
        }
        actor->behaviour = beh;
    } else {
        beh = actor->behaviour;
    }

    actor->field4C |= 0x20000;
    actor->fieldAC |= 0x100000;

    lbl_804D3288 = (int)AssetFind(0xD9044CBBu, 0x16F02F97u);
    lbl_804D328C = (int)AssetFind(0xF1754BF2u, 0x16F02F97u);
    lbl_804D3290 = (int)AssetFind(0xF2E2BEB8u, 0x19528D48u);
    lbl_804D3294 = (int)AssetFind(0xD9044CBBu, 0x04295FAAu);
    lbl_804D3298 = (int)AssetFind(0xD9044CBBu, 0x9F50232Bu);

    fn_80088C10(lbl_804D3290, "TDRIL", 12000, 18000, 0x1000, 4, 1, 0x1000);
    fn_80088F0C(lbl_804D3290, 1);
    fn_80088FB0(lbl_804D3290, 1);

    beh->state = 2;
    beh->field04 = 0;
    beh->flags = 0;
    beh->timer = 0.0f;
    beh->fx = fn_8016F120(FxFind("g_barriers"), &actor->field24);
    beh->flags |= 2;
    lbl_804D329C = FxFind("BarrierPass_NPC");
    return 1;
}

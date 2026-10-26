// u_801E0EDC.cpp: barrier actor (message, update, init) and its small helpers (0x801E0EDC-0x801E241C).
// Names are descriptive guesses; the code is matched against the retail DOL.
//
// Active-object registry helpers. Descriptive names are guesses.
// These functions are still called by other units through their fn_
// placeholders (see u_80230504.cpp), so they keep C linkage and their
// original symbol names.

typedef unsigned long u32;

struct Vec3 {
    float x;
    float y;
    float z;
};

struct ActiveEntry {
    void* value;        // 0x0
    ActiveEntry* next;  // 0x4
};

extern ActiveEntry* lbl_804D5220;
extern float lbl_804D8D78;

extern "C" float fn_802A2080(void);
void VFree(void* p);

struct ActiveObject {
    virtual void Unk00();
    virtual void Unk04();
    virtual void Release(int flag);                                   // 0x8
    virtual void Unk0C();
    virtual void Unk10();
    virtual void Unk14();
    virtual void Unk18();
    virtual u32 Unk1C();
    virtual void Unk20();
    virtual void Unk24();
    virtual void Unk28();
    virtual void Unk2C();
    virtual void Apply(Vec3* pos, float scale, Vec3* vel, int kind);  // 0x30
};

static inline bool ActiveIsRegistered(void* p) {
    if (p == 0) return false;
    for (ActiveEntry* e = lbl_804D5220; e != 0; e = e->next) {
        if (e->value == p) return true;
    }
    return false;
}

// Applies a distance-scaled velocity to a registered active object.
extern "C" void fn_801E0EDC(void* actor, Vec3* a, Vec3* b, int kind, float scale) {
    if (ActiveIsRegistered(actor)) {
        float s = lbl_804D8D78 / fn_802A2080();
        Vec3 v;
        v.x = b->x * s;
        v.y = b->y * s;
        v.z = b->z * s;
        ((ActiveObject*)actor)->Apply(a, scale, &v, kind);
    }
}

// Unregisters an active object and frees its registry entry.
extern "C" void fn_801E0FD0(void** slot) {
    if (slot == 0) return;
    void* obj = *slot;
    if (obj != 0 && ActiveIsRegistered(obj)) {
        if (obj != 0) {
            ((ActiveObject*)obj)->Release(1);
        }
        ActiveEntry* cur = lbl_804D5220;
        if (cur->value == obj) {
            lbl_804D5220 = cur->next;
            VFree(cur);
        } else {
            ActiveEntry* prev = cur;
            cur = cur->next;
            while (cur != 0 && cur->value != obj) {
                prev = cur;
                cur = cur->next;
            }
            if (cur != 0) {
                prev->next = cur->next;
                VFree(cur);
            }
        }
        *slot = 0;
    }
}

// Ticks a registered active object; returns the hook's result, or 1 when the
// object is not registered.
extern "C" u32 fn_801E10D8(void* obj) {
    u32 result;
    if (ActiveIsRegistered(obj)) {
        result = ((ActiveObject*)obj)->Unk1C();
    } else {
        result = 1;
    }
    return result;
}

// Runs the cleanup hook of a registered active object.
extern "C" void fn_801E1150(void* obj) {
    if (ActiveIsRegistered(obj)) {
        ((ActiveObject*)obj)->Unk18();
    }
}

// Calls the update hook of a registered active object.
extern "C" void fn_801E11C0(void* obj) {
    if (ActiveIsRegistered(obj)) {
        ((ActiveObject*)obj)->Unk14();
    }
}

// Placement new: the factories below allocate raw storage and construct in it.
inline void* operator new(unsigned long, void* p) { return p; }

// Base of the nodes held in the active-object registry. Its constructor sets
// the base vtable; the derived constructor then overwrites it.
struct ActiveNodeBase {
    ActiveNodeBase() {}
    virtual void Unk00();
};

struct ActiveNode : ActiveNodeBase {
    ActiveNode() : m0C(0) {}
    virtual void Unk04();                                                         // 0x4
    virtual void Release(int flag);                                                // 0x8
    virtual unsigned char Init(void* a, void* b, void* c, void* d, void* e, void* f); // 0xC
    int m04;                                        // 0x4
    int m08;                                        // 0x8
    int m0C;                                        // 0xC
    int m10;                                        // 0x10
    int m14;                                        // 0x14
    int m18;                                        // 0x18
    int m1C;                                        // 0x1C
};

extern void* VAlloc(int size);

// Creates an active node, initialises it from the six arguments and registers
// it. Returns 0 (after releasing the half-built node) on any failure.
extern "C" void* fn_801E1230(void* a, void* b, void* c, void* d, void* e, void* f) {
    ActiveNode* node = (ActiveNode*)VAlloc(0x20);
    ActiveEntry* entry;
    if (node != 0) {
        new (node) ActiveNode;
    }
    if (node == 0) {
        return 0;
    }
    if (node->Init(a, b, c, d, e, f) == 0) {
        if (node != 0) {
            node->Release(1);
        }
        return 0;
    }
    entry = (ActiveEntry*)VAlloc(8);
    if (entry == 0) {
        if (node != 0) {
            node->Release(1);
        }
        return 0;
    }
    entry->value = node;
    entry->next = lbl_804D5220;
    lbl_804D5220 = entry;
    return node;
}

// Calls the per-object tick hook (vtable +0x20) of every registered object.
extern "C" void fn_801E1354(void) {
    for (ActiveEntry* e = lbl_804D5220; e != 0; e = e->next) {
        ((ActiveObject*)e->value)->Unk20();
    }
}

// View of a registered object that exposes the float-feed hook at vtable +0x10.
struct ActiveAnimObject {
    virtual void Unk00();
    virtual void Unk04();
    virtual void Unk08();
    virtual void Unk0C();
    virtual void Unk10(float value, int flag);  // 0x10
};

// Pushes a value to every registered object; the second argument flags it.
extern "C" void fn_801E13A0(float value) {
    for (ActiveEntry* e = lbl_804D5220; e != 0; e = e->next) {
        ((ActiveAnimObject*)e->value)->Unk10(value, 1);
    }
}

// Builds the name tables used by the active-object registry.
void u8020A650InitNameTables(void);

extern "C" void fn_801E1400(void) {
    u8020A650InitNameTables();
}

// Releases and frees every entry of the active-object registry.
extern "C" void fn_801E1420(void) {
    ActiveEntry* entry = lbl_804D5220;
    while (entry != 0) {
        ActiveEntry* next = entry->next;
        if (entry->value != 0) {
            ((ActiveObject*)entry->value)->Release(1);
        }
        VFree(entry);
        entry = next;
    }
    lbl_804D5220 = 0;
}

// Clears the head of the active-object registry.
// Kept under its fn_ placeholder: u_801BF578 still calls it by that name.
extern "C" void fn_801E1494(void) {
    lbl_804D5220 = 0;
}

// Vtable installed on released active objects.
extern char lbl_803ADE00;

// Resets an active object's vtable and optionally frees its storage.
// Kept under its fn_ placeholder: several retail vtable blocks reference it.
extern "C" void* fn_801E14A0(void* object, short freeNow) {
    if (object != 0) {
        *(void**)object = (void*)&lbl_803ADE00;
        if (freeNow > 0) {
            VFree(object);
        }
    }
    return object;
}

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

struct Mtx34 {
    float m[12];
};

// View of an object that answers the "0x400" virtual query (vtable +0x30).
struct BarrierMsgSource {
    virtual void Unk00();
    virtual void Unk04();
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

extern float lbl_804D8D80;
extern float lbl_804D8D84;
extern float lbl_804D8D88;
extern float lbl_804D8D8C;
extern float lbl_804D8D90;
extern float lbl_804D8D94;
extern float lbl_804D8D98;
extern float lbl_804D8D9C;
extern float lbl_804D8DA0;
extern float lbl_804D8DA4;
extern int lbl_804D3288;
extern int lbl_804D328C;
extern int lbl_804D3290;
extern int lbl_804D3294;
extern int lbl_804D3298;
extern int lbl_804D329C;
extern int lbl_803ADE38[];

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
int ActorDefaultMsg(Actor* actor, int msg, void* data);
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
static inline bool BarrierKindIsValid(u32 kind) {
    for (int i = 0; i < 64; i++) {
        if (kind == lbl_803ADE38[i]) {
            return true;
        }
    }
    return false;
}

// Notifies the object held in the message payload (the player using the
// barrier) and reports whether the barrier should stay closed.
static inline int BarrierNotifyTarget(Actor* actor, int msg, int* data, int target) {
    int result = ActorDefaultMsg(actor, msg, data);
    if (result == 0) {
        return 0;
    }
    BarrierBeh* beh = ((BarrierActor*)data[0])->behaviour;
    int valid;
    if (target != 0 && beh != 0 && beh->field04 != 0) {
        struct {
            int target;
            u32 info;
        } arg;
        arg.target = target;
        valid = ((BarrierMsgSource*)target)->Apply(0x400, &arg);
        if (valid != 0) {
            u32 category = arg.info >> 24;
            if (category == 0x13) {
                valid = 1;
            } else if (category < 0x13 && category == 2) {
                if (PlayerInHost((Actor*)target) == 0) {
                    valid = 1;
                } else {
                    ((BarrierActor*)target)->field14E8 = lbl_804D8D80;
                    valid = 0;
                }
            } else {
                valid = 0;
            }
        }
    } else {
        valid = 0;
    }
    if (valid != 0) {
        return result;
    }
    return 0;
}

// Message handler of the barrier actor (states, hit tests, effect spawning).
// Descriptive name: BarrierActorMsg.
u32 BarrierActorMsg(void* actorPtr, int msg, int* data) {
    Actor* actor = (Actor*)actorPtr;
    BarrierActor* self = (BarrierActor*)data[0];

    switch (msg) {
    case 1: {
        int result = ActorDefaultMsg(actor, msg, data);
        if (result != 0) {
            int bone = ModelFindBone(self->model, lbl_804D328C);
            if (bone != data[5]) {
                return result;
            }
        }
        return 0;
    }
    case 2:
        return BarrierNotifyTarget(actor, msg, data, data[3]);
    case 3:
        return BarrierNotifyTarget(actor, msg, data, data[4]);
    case 4:
        return BarrierNotifyTarget(actor, msg, data, data[7]);
    case 0x405: {
        BarrierActor* other = (BarrierActor*)data[1];
        if ((other->field38 >> 24) != 0x13) {
            return 0;
        }
        Mtx34 selfMtx;
        Mtx34 otherMtx;
        int selfBox[6];
        int otherBox[6];
        ActorGetMatrix(&selfMtx, (Actor*)self);
        ActorGetMatrix(&otherMtx, (Actor*)other);
        fn_8016D864(selfBox, self->field40);
        fn_8016D864(otherBox, other->field40);
        int hit = ModelBoneHitBox((int)selfBox, (int)&selfMtx, (int)otherBox,
                                  (int)&otherMtx);
        int result = 0;
        if (hit != 0) {
            result = 1;
        }
        data[4] = result;
        if (result != 0) {
            fn_8016F120(lbl_804D329C, &self->field24);
        }
        return result;
    }
    case 0x409: {
        BarrierBeh* beh = self->behaviour;
        if (beh != 0) {
            u32 kind = (u32)data[2];
            if ((kind >> 24) == 2) {
                kind = (u32)fn_8005AAF8((int)PlayerGetPossessor((int)kind));
            }
            if (!BarrierKindIsValid(kind)) {
                if ((beh->flags & 2) == 0) {
                    beh->flags |= 2;
                } else {
                    beh->flags &= ~2u;
                }
            }
        }
        return 1;
    }
    case 0x40A: {
        BarrierBeh* beh = self->behaviour;
        if (beh != 0 && (u32)data[2] == 0x7C44EC3D && (beh->flags & 1) != 0) {
            // Asks the possessor whether the barrier may open; the answer
            // comes back through the String out-parameter.
            struct {
                int unknown;
                u32 hash;
                int zero;
                int string;
            } nameMsg;
            nameMsg.hash = 0x947E97;
            nameMsg.zero = 0;
            nameMsg.string = 0;
            SendMsg(data[1], 0x41E, &nameMsg);
            if (nameMsg.string != 0) {
                data[3] = (int)0xF21581DEu;
                u32 kind = (u32)data[1];
                if ((kind >> 24) == 2) {
                    kind = (u32)fn_8005AAF8((int)PlayerGetPossessor((int)kind));
                }
                int enabled = 0;
                if (BarrierKindIsValid(kind)) {
                    enabled = 0;
                } else {
                    enabled = 0;
                }
                struct {
                    int unknown;
                    int zero;
                    int id;
                    void* asset;
                } assetMsg;
                assetMsg.zero = enabled;
                assetMsg.id = data[3];
                assetMsg.asset = AssetFind(0xC0A77187u, 0x25A645u);
                SendMsg(data[1], 0x40B, &assetMsg);
            }
        }
        return 1;
    }
    case 0x41D:
        BarrierKindRegistryMsg(self, data);
        return 1;
    case 0x41E:
        if (self->behaviour == 0) {
            return 0;
        }
        if ((u32)data[1] == 0x914B98A8u) {
            data[3] = 1;
            return 1;
        }
        return 0;
    }
    return ActorDefaultMsg(actor, msg, data);
}

// Per-frame update of the barrier actor: runs the open/close state machine,
// drives the node colour and effect, then chains to ActorUpdate.
// Descriptive name: BarrierActorUpdate.
void BarrierActorUpdate(void* actorPtr, float delta) {
    BarrierActor* actor = (BarrierActor*)actorPtr;
    BarrierBeh* beh = actor->behaviour;

    if (beh != 0) {
        // "level" scales the barrier effect; it starts at its maximum.
        float level = lbl_804D8D80;

        switch (beh->state) {
        case 0: {
            level = lbl_804D8D84;
            if ((beh->flags & 2) != 0) {
                if ((beh->flags & 4) != 0) {
                    beh->timer -= delta;
                } else {
                    beh->timer = level;
                }
                if (beh->timer <= lbl_804D8D84) {
                    beh->state = 1;
                    beh->timer = lbl_804D8D88;
                    ModelNodeSetup(actor->model, lbl_804D3288, 1, 0);
                    fn_80038474(actor->model, lbl_804D328C, 1, 0);
                }
            }
            break;
        }
        case 1: {
            beh->timer -= delta;
            level = level - lbl_804D8D88 * beh->timer;
            if (beh->timer <= lbl_804D8D84) {
                beh->state = 2;
                u32 seed = (u32)rand();
                beh->timer = lbl_804D8D94 * ((float)seed / lbl_804D8D8C) + lbl_804D8D90;
                beh->fx = fn_8016F120(FxFind("g_barriers"), &actor->field24);
                fn_8016F120(FxFind("g_barriers_on"), &actor->field24);
            }
            break;
        }
        case 2: {
            if ((beh->flags & 2) == 0) {
                beh->state = 3;
                beh->timer = lbl_804D8D88;
            } else if ((beh->flags & 4) != 0) {
                float speed = fn_80017084(beh->timer);
                beh->timer -= delta;
                level = lbl_804D8D9C * speed + lbl_804D8D98;
                if (beh->timer <= lbl_804D8D84) {
                    beh->state = 3;
                    beh->timer = lbl_804D8D88;
                }
            }
            break;
        }
        case 3: {
            beh->timer -= delta;
            level = lbl_804D8D88 * beh->timer;
            if (beh->timer <= lbl_804D8D84) {
                beh->state = 0;
                beh->timer = lbl_804D8D88;
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

        float clamped;
        if (level < lbl_804D8D84) {
            clamped = lbl_804D8D84;
        } else if (level > lbl_804D8D80) {
            clamped = lbl_804D8D80;
        } else {
            clamped = level;
        }
        beh->field04 = (int)((lbl_804D8DA0 * delta) * clamped);
        fn_80037EC0(actor->model, lbl_804D3288, 1, 100, 60, 140,
                    (int)(lbl_804D8DA4 * level) & 0xFF);
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
    if (beh == 0) {
        return 0;
    }

    switch (data[1]) {
    case 0x1314F:
        beh->flags &= ~2u;
        return 1;
    case 0x9DF:
        beh->flags |= 2;
        return 1;
    case (int)0xFCC9CDD8u:
        if (data[3] != 0) {
            beh->flags |= 4;
        } else {
            beh->flags &= ~4u;
        }
        beh->timer = lbl_804D8D88;
        return 1;
    case 0x1E417DFF: {
        int firstFree = -1;
        int* table = lbl_803ADE38;
        int i = 0;
        for (int n = 16; n != 0; n--) {
            if (data[3] == table[0]) {
                return 1;
            }
            if (firstFree == -1 && table[0] == -1) {
                firstFree = i;
            }
            i++;
            if (data[3] == table[1]) {
                return 1;
            }
            if (firstFree == -1 && table[1] == -1) {
                firstFree = i;
            }
            i++;
            if (data[3] == table[2]) {
                return 1;
            }
            if (firstFree == -1 && table[2] == -1) {
                firstFree = i;
            }
            i++;
            if (data[3] == table[3]) {
                return 1;
            }
            if (firstFree == -1 && table[3] == -1) {
                firstFree = i;
            }
            i++;
            table += 4;
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
    beh = actor->behaviour;
    if (beh == 0) {
        beh = (BarrierBeh*)PoolAlloc(lbl_8049B468, 0x20, lbl_8049B868, 0x20);
        if (beh == 0) {
            return 0;
        }
        actor->behaviour = beh;
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
    beh->timer = lbl_804D8D84;
    beh->fx = fn_8016F120(FxFind("g_barriers"), &actor->field24);
    beh->flags |= 2;
    lbl_804D329C = FxFind("BarrierPass_NPC");
    return 1;
}

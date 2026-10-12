// Repel.cpp: the "Repel" actor type (0x80191C6C-0x801920B8).
// A repel field weakens while it pushes actors of type 2 away and recharges when the global repel
// strength changes. Debug builds draw its two radii and print its strength.
// Names are descriptive guesses; the code is matched against the retail DOL.

struct Vec3 {
    float x, y, z;
};

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Actor;

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

struct ActorType {
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void (*update)(Actor* actor, float dt);
    int (*init)(Actor* actor);
    void* unk14;
    void (*destroy)(Actor* actor);
    void* unk1C[5];
};

struct RepelInfo {
    char pad0[0x30];
    float strength;
    float unk34;
    float unk38;
    float radius1;
    float radius2;
};

struct RepelData {
    float strength;
};

struct Actor {
    char pad0[0x24];
    int x;
    int y;
    int z;
    char pad30[8];
    unsigned long flags;
    char pad3C[0x48];
    RepelInfo* info;
    char pad88[0x1C];
    RepelData* data;
};

int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
void ActorUpdate(Actor* a, float dt);                     // 0x800E48A0
int ActorInitBase(Actor* a);                              // 0x800E6444
void* PoolAlloc(void* pool, int count, void* used, int size); // 0x800249A8
void PoolFree(void* p, void* pool, int count, void* used); // 0x80024978
int ActorCount(void);                                     // 0x80049930
Actor* ActorGet(int i);                                   // 0x802A2024
int ActorIsActive(Actor* a);                              // 0x80022D80
int ActorRepel(Actor* other, Actor* a, float strength, float a1, float a2, float r1, float r2); // 0x800695A0
void DebugDrawSphere(int r, int g, int b, int a, float x, float y, float z, float radius); // 0x80023C9C
void WorldToScreen(Vec3* in, Vec3* out);                  // 0x8007E7FC
void DebugTextPos(int x, int y, int size);                // 0x8007E570
void DebugText(const char* format, ...);                  // 0x801EB188

extern float gRepelStrength;                              // 0x804D3068

int RepelMsg(Actor* a, int msg, void* data, int arg);
void RepelUpdate(Actor* a, float dt);
int RepelInit(Actor* a);
void RepelDestroy(Actor* a);

ActorType gRepelType = { 0, "Repel", RepelMsg, RepelUpdate, RepelInit, 0, RepelDestroy };

static char sRepelPool[4][64];
static char sRepelUsed[16];

static int sRepelActive;
static int sRepelDebug;
static float sRepelLast = gRepelStrength;
static float sRepelRecharge;

void RepelDestroy(Actor* a) {
    if (a->data) {
        PoolFree(a->data, sRepelPool, 4, sRepelUsed);
    }
    a->data = 0;
    sRepelActive = 0;
}

void RepelUpdate(Actor* a, float dt) {
    int i;
    int n;
    RepelData* d = a->data;

    if (d == 0) return;

    n = ActorCount();
    if (sRepelRecharge > 0.0f) {
        sRepelRecharge = sRepelRecharge - 1.0f;
        d->strength = gRepelStrength;
    }

    for (i = 0; i < n; i++) {
        Actor* other = ActorGet(i);
        if (ActorIsActive(a)) {
            int hit;
            if ((int)(other->flags >> 24) == 2) {
                RepelInfo* info = a->info;
                float s = info->strength;
                if (a->data->strength > 0.0f && s > 0.0f && s < 1.0f) {
                    s = 1.0f;
                }
                hit = ActorRepel(other, a, s, info->unk34, info->unk38, info->radius1, info->radius2);
            } else {
                hit = 0;
            }
            if (hit) {
                d->strength = d->strength - dt;
                d->strength = d->strength > 0.0f ? d->strength : 0.0f;
            }
        }
    }

    if (sRepelLast != gRepelStrength) {
        sRepelRecharge = 64.0f;
    }
    sRepelLast = gRepelStrength;
    ActorUpdate(a, dt);
}

int RepelMsg(Actor* a, int msg, void* data, int arg) {
    if (msg == 0 && sRepelDebug != 0) {
        Actor* t = *(Actor**)data;
        Vec3 v;

        DebugDrawSphere(255, 0, 0, 64, t->x, t->y, t->z, t->info->radius1);
        DebugDrawSphere(0, 255, 0, 64, t->x, t->y, t->z, t->info->radius2);
        v.x = t->x;
        v.y = t->y;
        v.z = t->z;
        WorldToScreen(&v, &v);
        DebugTextPos(v.x, v.y, 2);
        DebugText("Strength %f", t->info->strength);
    }
    return ActorDefaultMsg(a, msg, data, arg);
}

int RepelInit(Actor* a) {
    RepelData* d;

    if (!ActorInitBase(a)) return 0;

    if (a->data == 0) {
        d = (RepelData*)PoolAlloc(sRepelPool, 4, sRepelUsed, 64);
        if (d == 0) return 0;
        a->data = d;
    }
    d->strength = 2000.0f;
    if (sRepelActive == 0) {
        sRepelActive = 1;
    }
    return 1;
}

// EvacPoint.cpp: the "Evac Point" actor type (0x8015C4B0-0x8015C79C).
// A trigger zone: when an actor of the right side (marine or ghost team, from the placement type)
// stands in it, the level gets a trigger message.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

struct Mtx33 {
    float m[3][3];
};

struct Mtx34 {
    float m[3][4];
};

struct IVec3 {
    int x, y, z;
};

// Orientation and float position, as used to place a new actor.
struct PlaceF {
    Mtx33 rot;
    Vec3 pos;
};

// Orientation and position, as stored at the start of every actor.
struct Placement {
    Mtx33 rot;
    IVec3 pos;
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

struct EvacDef {
    Vec3 pos;
    Vec3 rot;
    int unk18;
    char pad1C[8];
    u32 sound;
};

struct EvacInfo {
    char pad0[0x18];
    int type;
    char pad1C[0x14];
    float radius;
};

// Zone state (VAlloc'd, 0x140 bytes).
struct EvacZone {
    int team;
    char zone[0x3C];
    char list[0x100];
};

// Query sent to an actor's own message function.
struct ActorQuery {
    Actor* actor;
    int unk4[3];
    float health;
    int unk14[5];
};

struct TriggerMsg {
    int unk0;
    int team;
    int owner;
    u32 event;
};

struct Actor {
    Placement place;
    int (*query)(int msg, void* data);
    char pad34[4];
    int handle;
    char pad3C[0x14];
    char unk50[0x18];
    char unk68[0x18];
    EvacDef* def;
    EvacInfo* info;
    char pad88[4];
    int owner;
    char pad90[4];
    int unk94;
    int unk98;
    char pad9C[4];
    char unkA0[4];
    EvacZone* zone;
    int sound;
    u32 flags;
    char padB0[0x60];
    int unk110;
};

extern "C" int SendMsg(int target, int msg, void* data);            // 0x800265D4
int AssetFind(u32 bank, u32 name);                        // 0x8007EB34
void MtxSetEuler(Mtx34* m, Vec3* rot);                    // 0x8009FAC0
void ActorFn8002378C(Actor* actor, int a, int b, int c, int d, int e, int f); // 0x8002378C
void PlacementToMatrix(Mtx34* m, PlaceF* p);           // 0x8009EF04
void ActorFn800C7B5C(Actor* a, void* p);                  // 0x800C7B5C
int ActorFn80022FD8(Actor* a);                            // 0x80022FD8
void ActorFn800231A8(Actor* a);                           // 0x800231A8
void ActorSetPlacement(Actor* a, PlaceF* p);           // 0x800E7898
void ActorCollide(void);                                  // 0x800C8394
void ActorUpdate(Actor* a, float dt);                     // 0x800E48A0
void* VAlloc(int size);                                   // 0x8009DA68
void VFree(void* p);                                      // 0x8009DA3C
void ZoneUpdate(void* zone, IVec3* pos);                  // 0x800D15B4
void ZoneInit(void* zone, Actor* a, IVec3* pos, void* list, int n, float radius); // 0x800D1900
void ZoneSetCallback(void* zone, int a, int (*cb)(int, Actor*, Actor*), int b, Actor* arg); // 0x800D17B8
int HandleToPlayer(int handle);                           // 0x800FD720
int PlayerTeam(int player);                               // 0x800FD68C

void EvacDestroy(Actor* a);
void EvacUpdate(Actor* a, float dt);
int EvacInit(Actor* a);
int EvacEnter(int unused, Actor* o, Actor* a);

ActorType gEvacPointType = { 0, "Evac Point", 0, EvacUpdate, EvacInit, 0, EvacDestroy };

void EvacDestroy(Actor* a) {
    if (a && a->zone) {
        VFree(a->zone);
        a->zone = 0;
    }
}

void EvacUpdate(Actor* a, float dt) {
    ZoneUpdate(a->zone->zone, &a->place.pos);
    ActorUpdate(a, dt);
}

int EvacInit(Actor* a) {
    PlaceF pl;
    Mtx34 m;
    EvacZone* z;

    a->unk94 = 0;
    a->unk98 = 0;
    MtxSetEuler((Mtx34*)&pl.rot, &a->def->rot);
    pl.pos = a->def->pos;
    PlacementToMatrix(&m, &pl);
    ActorFn8002378C(a, a->owner, (int)&m, (int)ActorCollide, (int)a->unk68, (int)a->unk50, a->def->unk18);
    ActorFn800C7B5C(a, a->unkA0);
    if (!ActorFn80022FD8(a)) {
        ActorFn800231A8(a);
    }
    a->unk110 = -1;
    ActorSetPlacement(a, &pl);
    a->sound = AssetFind(-1, a->def->sound);
    z = (EvacZone*)VAlloc(sizeof(EvacZone));
    if (!z) {
        return 0;
    }
    a->zone = z;
    a->flags |= 0x22000000;
    switch (a->info->type) {
    case 0x00013C71:
        z->team = 1;
        break;
    case 0x001F285A:
        z->team = 0;
        break;
    }
    ZoneInit(z->zone, a, &a->place.pos, z->list, 16, a->info->radius);
    ZoneSetCallback(z->zone, 0, EvacEnter, 0, a);
    return 1;
}

int EvacEnter(int unused, Actor* o, Actor* a) {
    TriggerMsg m;
    ActorQuery q;
    EvacZone* z = a->zone;
    int player = HandleToPlayer(o->handle);

    if (player != -1) {
        q.actor = o;
        o->query(0x401, &q);
        if (q.health > 0.0f) {
            int team = PlayerTeam(player);
            if (team == z->team) {
                m.owner = a->owner;
                m.team = team;
                m.event = 0x0020A893;
                SendMsg(0x4B000000, 0xC14, &m);
            }
        }
    }
    return 1;
}

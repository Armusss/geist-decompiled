// Hazard.cpp: the "Hazard" actor type (0x8015C080-0x8015C4B0).
// A damaging area (fire, steam, ...) driven by a hazard emitter; it follows its actor's placement and
// passes trigger messages on to the level.
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

struct HazardDef {
    Vec3 pos;
    Vec3 rot;
    int unk18;
    char pad1C[8];
    u32 sound;
};

struct HazardInfo {
    char pad0[0x18];
    int type;
    char pad1C[0x14];
    float size;
    float strength;
};

struct Actor {
    Placement place;
    char pad30[8];
    int handle;
    char pad3C[0x14];
    char unk50[0x18];
    char unk68[0x18];
    HazardDef* def;
    HazardInfo* info;
    char pad88[4];
    int owner;
    char pad90[0x10];
    char unkA0[4];
    int hazard;
    int sound;
    char padAC[4];
    char unkB0[0x60];
    int unk110;
};

struct TriggerMsg {
    int unk0;
    int unk4;
    Actor* actor;
    u32 event;
};

int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
extern "C" int SendMsg(int target, int msg, void* data);            // 0x800265D4
int AssetFind(u32 bank, u32 name);                        // 0x8007EB34
void MtxSetEuler(Mtx34* m, Vec3* rot);                    // 0x8009FAC0
void ActorFn8002378C(Actor* actor, int a, int b, int c, int d, int e, int f); // 0x8002378C
extern "C" void Fn8007CEA4(void* p);                                 // 0x8007CEA4
void HazardSetActive(int hazard, int on);                 // 0x80122448
void HazardMove(int hazard, Vec3* pos);                   // 0x801223E4
void PlacementToMatrix(Mtx34* m, PlaceF* p);           // 0x8009EF04
void ActorFn800C7B5C(Actor* a, void* p);                  // 0x800C7B5C
int ActorFn80022FD8(Actor* a);                            // 0x80022FD8
void ActorFn800231A8(Actor* a);                           // 0x800231A8
void ActorSetPlacement(Actor* a, PlaceF* p);           // 0x800E7898
int HazardCreate(Actor* a, int handle, Vec3* pos, int type, int sound, float w, float h, float d, float s); // 0x80121F58
void HazardSetFlag(int hazard, int flag, int on);         // 0x80122304
void ActorCollide(void);                                  // 0x800C8394

int HazardMsg(Actor* a, int msg, void* data, int arg);
int HazardInit(Actor* a);

ActorType gHazardType = { 0, "Hazard", HazardMsg, 0, HazardInit };

int HazardMsg(Actor* a, int msg, void* data, int arg) {
    Vec3 pos;
    TriggerMsg m;

    switch (msg) {
    case 0xC14:
        if (((u32*)data)[3] == 0x001FF464) {
            int v = ((int*)data)[2];
            m.actor = a;
            m.event = 0x7C3F4E3E;
            m.unk4 = v;
            SendMsg(0x4B000000, 0xC14, &m);
            return 1;
        }
        break;
    case 0xC00:
        HazardSetActive((*(Actor**)data)->hazard, ((int*)data)[1]);
        return ActorDefaultMsg(a, msg, data, arg);
    case 0x413: {
        Actor* t = ((Actor**)data)[0];
        t->place = *((Placement**)data)[1];
        pos.x = t->place.pos.x;
        pos.y = t->place.pos.y;
        pos.z = t->place.pos.z;
        HazardMove(t->hazard, &pos);
        return 1;
    }
    case 0x411:
        ((Placement*)((char*)data + 0x10))[0] = (*(Actor**)data)->place;
        return 1;
    case 0x414:
        return 1;
    case 0x41E:
        if (((u32*)data)[1] == 0x7C18FE9E) {
            return ActorDefaultMsg(a, msg, data, arg);
        }
        return 0;
    case 0x41A:
    case 0x415:
        return ActorDefaultMsg(a, msg, data, arg);
    }
    return 0;
}

int HazardInit(Actor* a) {
    Vec3 pos;
    PlaceF pl;
    Mtx34 m;

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
    Fn8007CEA4(a->unkB0);
    pos.x = a->place.pos.x;
    pos.y = a->place.pos.y;
    pos.z = a->place.pos.z;
    a->hazard = HazardCreate(a, a->handle, &pos, a->info->type, a->sound, a->info->size, a->info->size,
                             a->info->strength, a->info->strength);
    HazardSetFlag(a->hazard, 0, 1);
    HazardSetFlag(a->hazard, 2, 0);
    HazardSetFlag(a->hazard, 19, 0);
    return 1;
}

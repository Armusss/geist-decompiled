// Skybox.cpp: the "Skybox" actor type (0x80150148-0x80150300).
// Registers its type descriptor, handles a show/hide message and starts its sound/effect when spawned.
// Names are descriptive guesses; the code is matched against the retail DOL.

struct Actor;

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);
typedef int (*ActorFunc)(Actor* actor);

// Actor type descriptor (0x30 bytes), registered in a table of 64 types.
struct ActorType {
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void* unkC;
    ActorFunc spawn;
    void* unk14[7];
};

struct ActorDef {
    char pad[0x1C];
    int sound;
};

struct ActorProps {
    char pad[8];
    int sound;
};

struct Actor {
    char pad0[0x80];
    ActorDef* def;
    ActorProps* props;
    char pad88[4];
    int unk8C;
    char pad90[0x20];
    char unkB0[1];
};

struct SoundMsg {
    int unk0;
    int unk4;
    int sound;
    float volume;
};

int ActorTypeRegister(ActorType* type);                  // 0x800C83F0
int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
void ActorFn80022D98(Actor* actor, int arg);             // 0x80022D98
extern "C" int SendMsg(int target, int msg, void* data);            // 0x800265D4
void ActorFn8002378C(Actor* actor, int a, int b, int c, int d, int e, int f); // 0x8002378C
extern "C" void Fn8007CEA4(void* p);                                 // 0x8007CEA4

int SkyboxMsg(Actor* actor, int msg, void* data, int arg);
int SkyboxSpawn(Actor* actor);

ActorType gSkyboxType = { 0, "Skybox", SkyboxMsg, 0, SkyboxSpawn };

void SkyboxRegister(void) { ActorTypeRegister(&gSkyboxType); }

int SkyboxMsg(Actor* actor, int msg, void* data, int arg) {
    SoundMsg m;

    switch (msg) {
    case 1:
    case 2:
    case 3:
    case 4:
        return 0;
    case 0xC00: {
        Actor* target = ((Actor**)data)[0];
        ActorFn80022D98(target, ((int*)data)[1]);
        if (((int*)data)[1] == 0) {
            m.sound = -1;
            m.unk4 = 0;
            m.volume = 255.0f;
            SendMsg(0x25000000, 0x5400, &m);
        } else {
            m.sound = target->def->sound;
            m.unk4 = 0;
            m.volume = 255.0f;
            SendMsg(0x25000000, 0x5400, &m);
        }
        return 1;
    }
    default:
        return ActorDefaultMsg(actor, msg, data, arg);
    }
}

int SkyboxSpawn(Actor* actor) {
    SoundMsg m;
    int ok = 1;

    m.unk4 = 0;
    m.volume = 1.0f;
    if (actor->def->sound != -1) {
        m.sound = actor->def->sound;
    } else if (actor->props->sound == -1) {
        ok = 0;
    } else {
        m.sound = actor->props->sound;
    }
    SendMsg(0x25000000, 0x5400, &m);
    ActorFn8002378C(actor, actor->unk8C, 0, 0, 0, 0, 0);
    Fn8007CEA4(actor->unkB0);
    return ok;
}

// Streamer.cpp: the "Streamer" actor type (0x8022EC64-0x8022ED88).
// A ribbon/trail effect created from the actor's placement data when it spawns.
// Names are descriptive guesses; the code is matched against the retail DOL.

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

struct StreamerInfo {
    char pad0[0x30];
    float red;
    float green;
    float blue;
    float segments;
    float width;
    char pad44[0x3C];
    int texture;
};

struct Actor {
    char pad0[0x38];
    int handle;
    char pad3C[0x48];
    StreamerInfo* info;
    char pad88[0x1C];
    int trail;
};

// Trail description passed to TrailCreate (0x40 bytes).
struct TrailDesc {
    int unk0;
    int owner;
    int unk8;
    int segments;
    float width;
    float unk14;
    char pad18[8];
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    int texture;
    unsigned char color[3];
    char pad37[9];
};

int ActorTypeRegister(ActorType* type);                  // 0x800C83F0
int ActorInitBase(Actor* a);                              // 0x800E6444
int TrailCreate(TrailDesc* desc);                         // 0x801DE960
void TrailDestroy(int trail);                             // 0x801DE904

void StreamerDestroy(Actor* a);
int StreamerInit(Actor* a);

ActorType gStreamerType = { 0, "Streamer", 0, 0, StreamerInit, 0, StreamerDestroy };

void StreamerRegister(void) { ActorTypeRegister(&gStreamerType); }

void StreamerDestroy(Actor* a) { TrailDestroy(a->trail); }

int StreamerInit(Actor* a) {
    TrailDesc d;

    ActorInitBase(a);
    d.owner = a->handle;
    d.unk8 = 0;
    d.segments = a->info->segments;
    d.width = a->info->width;
    d.texture = a->info->texture;
    d.color[0] = a->info->red;
    d.color[1] = a->info->green;
    d.color[2] = a->info->blue;
    d.unk14 = -4.0f;
    d.unk20 = 0.01f;
    d.unk24 = -5.0f;
    d.unk28 = 31.0f;
    d.unk2C = 0.3f;
    a->trail = TrailCreate(&d);
    return 1;
}

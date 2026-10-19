// Thermostat.cpp: the thermostat gauge actor (0x80266FAC-0x80267558). Two actor types, "Thermostat Model"
// and "Gauge Explode", sharing one set of functions: the gauge level rises while the player is in the right
// mode, drives an animation, and explodes past 0.7. The thermostat behaviour is in ThermoBeh.cpp.
// Names are descriptive guesses; the code is matched against the retail DOL.

#pragma auto_inline off

typedef unsigned short u16;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Owner;
struct Beh;

typedef void (*BehStateFunc)(void* owner, void* beh);
typedef void (*BehCallback)(Owner* o, Beh* b, int arg);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

struct ThermoState {
    int cur;
    int next;
    int arg;
    int unkC;
    int sound;
};

struct Beh {
    char pad0[0x10];
    int state;
    u16 lo1;
    u16 hi1;
    u16 lo2;
    u16 hi2;
    char pad1C[0x10];
    unsigned long flags;
    char pad30[0x1C];
    BehCallback callback;
    char pad50[4];
    ThermoState* data;
    int unk58;
    int unk5C;
};

struct Owner {
    char pad0[0x270];
    char conditions[0x894 - 0x270];
    Beh beh;
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

struct GaugeData {
    char pad0[0x18];
    int unk18;
    char pad1C[0x20];
    int state;
    unsigned long anim;
    float level;
};

struct ActorInfo {
    char pad0[0x94];
    int sound;
};

struct Actor {
    char pad0[0x38];
    int handle;
    char pad3C[0x48];
    ActorInfo* info;
    ActorType* type;
    char pad8C[0x18];
    GaugeData* data;
    char padA8[4];
    unsigned long flags;
    char padB0[0x60];
    int anim;
};

struct Player {
    char pad0[0x31C];
    int mode;
};

struct ActorMsg {
    Actor* actor;
    int sound;
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
};

struct MsgResult {
    void* p;
    char pad[0x1C];
};

struct Buf60 {
    char data[0x60];
};

extern "C" void* memset(void* dst, int val, unsigned long n);
void* VAlloc(int size);
void VFree(void* p);
int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
extern "C" int SendMsg(int target, int msg, void* data);            // 0x800265D4
int HasCondition(void* list, int id);                    // 0x800A7AC4
int BehTarget(Beh* b);                                   // 0x8016334C
float OwnerValue(Owner* o, void* target);                // 0x800AA050
void BehUpdate(Owner* o, Beh* b);                        // 0x801632A8
void BehDone(Beh* b);                                    // 0x80162EBC
void BehReset(Beh* b);                                   // 0x801633D4
int OwnerId(Owner* o);                                   // 0x800A3A40
int Fn800F7DEC(int a, int b, int c, int d, void* e);     // 0x800F7DEC
int Fn80161FF4(Owner* o, void* buf, float f);           // 0x80161FF4
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f); // 0x800F7F5C
int Fn800F8A98(unsigned long hash);                      // 0x800F8A98
int Fn800F840C(int a, int b, int c);                     // 0x800F840C
void Fn80211A84(Actor* a);                               // 0x80211A84
void ActorUpdate(Actor* a, float dt);                    // 0x800E48A0
int Fn80211F78(Actor* a);                                // 0x80211F78
int Fn802120A0(Actor* a, int msg, void* data, int arg);  // 0x802120A0
void Fn800910FC(int anim, int a, float f);               // 0x800910FC
void Fn800901AC(int anim, int a);                        // 0x800901AC
void Fn8008E33C(int anim, int a, float f);               // 0x8008E33C
Player* GetPlayer(void);                                 // 0x80049948
void Fn8006CE5C(Player* p, int sound, int a, int b, int c, int d, int e); // 0x8006CE5C
void Fn8006CDE8(Player* p, int a, int b);                // 0x8006CDE8
void Fn8006CD84(Player* p, int a, float f);              // 0x8006CD84
void ActorPlayAnim(int handle, unsigned long anim, int handle2); // 0x800C5654
void Fn80211AA4(Actor* a, float dt);                     // 0x80211AA4


int GaugeMsg(Actor* a, int msg, void* data, int arg);
void GaugeUpdate(Actor* a, float dt);
int GaugeInit(Actor* a);
void GaugeDestroy(Actor* a);

ActorType gThermostatType = { 0, "Thermostat Model", GaugeMsg, GaugeUpdate, GaugeInit, 0, GaugeDestroy };
ActorType gGaugeExplodeType = { 0, "Gauge Explode", GaugeMsg, GaugeUpdate, GaugeInit, 0, GaugeDestroy };

void GaugeDestroy(Actor* a) {
    if (a->data) {
        if (a->type == &gGaugeExplodeType) {
            Fn80211A84(a);
        } else {
            VFree(a->data);
            a->data = 0;
        }
    }
}

void GaugeTick(Actor* a, float dt);

void GaugeUpdate(Actor* a, float dt) {
    switch (a->data->state) {
    case 1:
        GaugeTick(a, dt);
    }
    ActorUpdate(a, dt);
}

int GaugeInit(Actor* a) {
    GaugeData* d = a->data;
    if (d == 0) a->data = d = (GaugeData*)VAlloc(sizeof(GaugeData));
    memset(d, 0, sizeof(GaugeData));
    if (!Fn80211F78(a)) {
        a->flags |= 0x01000000;
        return 0;
    }
    return 1;
}

static inline int GaugeOnActivate(void* data) {
    Actor* actor = ((Actor**)data)[0];
    GaugeData* d = actor->data;

    if (actor->type == &gGaugeExplodeType) return 0;

    if (((int*)data)[1] != 0) {
        ActorMsg m;
        d->state = 1;
        d->level = 0.0f;
        d->anim = 0x3E341EF2;
        m.actor = actor;
        m.sound = actor->info->sound;
        m.unk8 = 0;
        m.unkC = 0;
        m.unk10 = 0x1000;
        m.unk14 = 0;
        m.unk18 = 0;
        ActorDefaultMsg((Actor*)actor->handle, 0xC04, &m, 0);
        Fn800910FC(actor->anim, 0, 100.0f);
        Fn800901AC(actor->anim, 0);
        Fn8008E33C(actor->anim, 0, 0.0f);
    } else {
        d->state = 0;
    }
    return 0;
}

static inline int GaugeOnTrigger(void* data) {
    Actor* actor = ((Actor**)data)[0];
    GaugeData* d = actor->data;

    if (actor->type != &gGaugeExplodeType) return 0;

    switch (((int*)data)[3]) {
    case 0x7BF91F20:
        d->state = 1;
        d->level = 0.0f;
        d->anim = 0x3E341EF2;
        break;
    case 0x39CB0CB0:
        d->state = 0;
        break;
    }
    return 0;
}

int GaugeMsg(Actor* a, int msg, void* data, int arg) {
    int ret = 0;

    switch (msg) {
    case 0xC00:
        ret = GaugeOnActivate(data);
        break;
    case 0xC14:
        ret = GaugeOnTrigger(data);
        break;
    case 0xC04:
    case 0xC09:
    case 0xC0D:
    case 0xC10:
    case 0xC11:
    case 0xC12:
        ret = (*(Actor**)data)->data->state ? 1 : 0;
        break;
    }

    if (ret == 0) {
        if ((*(Actor**)data)->type == &gGaugeExplodeType) {
            ret = Fn802120A0(a, msg, data, arg);
        } else {
            ret = ActorDefaultMsg(a, msg, data, arg);
        }
    }
    return ret;
}

void GaugeTick(Actor* a, float dt) {
    GaugeData* d = a->data;
    Player* p = GetPlayer();
    float lvl;

    if (p == 0) return;

    if (p->mode != 2) {
        d->level -= 0.000175f * dt - (1.0f - d->level) * (0.0001f * dt);
    }
    if (p->mode == 1 || p->mode == 3) {
        d->level += 0.03f;
    }
    lvl = d->level;
    d->level = lvl < 0.0f ? 0.0f : (lvl > 1.0f ? 1.0f : lvl);

    if (a->type == &gGaugeExplodeType) {
        if (d->level > 0.0f) {
            Fn8006CE5C(GetPlayer(), a->info->sound, 0, 0, 0x1000, 0, 0);
            Fn8006CDE8(GetPlayer(), 0, 0);
        }
        Fn8006CD84(p, 0, d->level);
    } else {
        Fn8008E33C(a->anim, 0, d->level);
    }

    if (d->anim == 0x3E341EF2) {
        if (d->level > 0.35f) {
            d->anim = 0xA6531245;
            ActorPlayAnim(a->handle, d->anim, a->handle);
        }
    } else if (d->anim == 0xA6531245) {
        if (d->level < 0.35f) {
            d->anim = 0x3E341EF2;
            ActorPlayAnim(a->handle, d->anim, a->handle);
        } else if (d->level > 0.7f) {
            d->anim = 0x8BA73C40;
            ActorPlayAnim(a->handle, d->anim, a->handle);
            if (a->type == &gGaugeExplodeType) {
                d->unk18 = 0x40000000;
                Fn80211AA4(a, dt);
                d->state = 2;
            }
        }
    } else if (d->anim == 0x8BA73C40) {
        if (d->level < 0.7f) {
            d->anim = 0xA6531245;
            ActorPlayAnim(a->handle, d->anim, a->handle);
        }
    }
}


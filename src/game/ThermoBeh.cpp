// ThermoBeh.cpp: the thermostat behaviour (0x802665A8-0x80266FAC). A character behaviour (registered in the
// behaviour table at 0x803A78C0) with a small state machine: it watches a target value and reports whether it
// lies inside two ranges. Split from Thermostat.cpp: the retail .sdata2 has one constant pool per file
// (0x804DA0A8-0x804DA0B4 here, 0x804DA0B8.. for the gauge), and this file has no string table.
// Names are descriptive guesses; the code is matched against the retail DOL.

#pragma auto_inline off

typedef unsigned short u16;


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


void ThermoEnter0(void* owner, void* beh);
void ThermoExit0(void* owner, void* beh);
void ThermoUpdate0(void* owner, void* beh);
void ThermoEnter1(void* owner, void* beh);
void ThermoExit1(void* owner, void* beh);
void ThermoUpdate1(void* owner, void* beh);
void ThermoEnter2(void* owner, void* beh);
void ThermoExit2(void* owner, void* beh);
void ThermoUpdate2(void* owner, void* beh);

BehState gThermoStates[8] = {
    { ThermoEnter0, ThermoExit0, ThermoUpdate0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { ThermoEnter1, ThermoExit1, ThermoUpdate1 },
    { ThermoEnter2, ThermoExit2, ThermoUpdate2 },
    { 0, 0, 0 },
    { 0, 0, 0 },
};

void ThermoCheck(Owner* o, Beh* b) {
    MsgResult res;

    b->flags = 0;
    if (HasCondition(o->conditions, 9) && HasCondition(o->conditions, 10) && HasCondition(o->conditions, 11)) {
        return;
    }

    if (b->state == 0) {
        b->flags |= 1;
        b->flags |= 2;
    }

    if (BehTarget(b) != -1 && SendMsg(BehTarget(b), 0x400, &res) && res.p != 0) {
        float v = OwnerValue(o, res.p);

        if (v >= b->lo1 && v <= b->hi1) b->flags |= 4;
        if (v >= b->lo2 && v <= b->hi2) b->flags |= 8;
        if (b->flags == 0) {
            if (v < b->lo1) {
                b->flags |= 0x40;
            } else {
                b->flags |= 0x80;
            }
        }
    }
}

void ThermoUpdate2(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehUpdate(o, b);
        ThermoCheck(o, b);
    }
}

void ThermoUpdate1(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehUpdate(o, b);
        ThermoCheck(o, b);
    }
}

void ThermoUpdate0(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehUpdate(o, b);
        ThermoCheck(o, b);
    }
}

void ThermoExit2(void* owner, void* beh) {}

void ThermoExit1(void* owner, void* beh) {}

void ThermoExit0(void* owner, void* beh) {}

void ThermoEnter2(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        ThermoState* st = b->data;
        st->next = 0;
        st->unkC = 0;
        BehDone(b);
    }
}

void ThermoEnter1(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        ThermoState* st = b->data;
        st->unkC = 0;
        BehDone(b);
    }
}

void ThermoEnter0(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        ThermoState* st = b->data;
        st->unkC = 0;
        st->next = 0;
        st->arg = 0;
        BehDone(b);
    }
}

int ThermoBehFn5(void* owner, void* beh) { return 0; }

inline void ThermoChangeState(Owner* o, ThermoState* st) {
    if (gThermoStates[st->cur].exit) gThermoStates[st->cur].exit(o, &o->beh);
    st->cur = st->next;
    o->beh.state = st->next;
    if (o->beh.callback) o->beh.callback(o, &o->beh, st->arg);
    if (gThermoStates[st->cur].enter) gThermoStates[st->cur].enter(o, &o->beh);
}

int ThermoBehMsg(void* owner, void* beh, int msg) {
    Owner* o;
    Beh* b;
    ThermoState* st;

    if (beh == 0) return 0;
    o = (Owner*)owner;
    b = (Beh*)beh;

    st = b->data;
    switch (msg) {
    case 0:
        st->next = 0;
        st->arg = 0;
        ThermoChangeState(o, st);
        break;
    case 1:
        st->next = 4;
        st->arg = 1;
        if (st->cur == 0) ThermoChangeState(o, st);
        break;
    case 2:
        st->next = 5;
        st->arg = 2;
        if (st->cur == 0) ThermoChangeState(o, st);
        break;
    case 0x1B83833F:
        ThermoChangeState(o, st);
        break;
    case 0x0020EE36:
        if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, 0x0020EE36, OwnerId(o), 0) == 0x04A3E183) {
            Buf60 buf;
            if (!Fn80161FF4(o, &buf, 0.0f)) return 0;
            Fn800F7F5C(o->beh.unk58, o->beh.unk5C, 0x0020EE36, OwnerId(o), &buf, 0.0f);
        }
        break;
    case 0xF829DB93:
        if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, 0xF829DB93, OwnerId(o), 0) == 0x04A3E183) {
            Buf60 buf;
            if (!Fn80161FF4(o, &buf, 0.0f)) return 0;
            Fn800F7F5C(o->beh.unk58, o->beh.unk5C, 0xF829DB93, OwnerId(o), &buf, 0.0f);
        }
        break;
    case 0x7EB0F3EF:
        if (Fn800F7DEC(o->beh.unk58, st->sound, 0xF829DB93, OwnerId(o), 0) == 0x04A3E183) {
            Buf60 buf;
            if (!Fn80161FF4(o, &buf, 0.0f)) return 0;
            Fn800F7F5C(o->beh.unk58, st->sound, 0xF829DB93, OwnerId(o), &buf, 0.0f);
        }
        break;
    }
    return 1;
}

int ThermoBehUpdate(void* owner, void* beh) {
    Owner* o;
    Beh* b;
    ThermoState* st;

    if (beh == 0) return 0;
    o = (Owner*)owner;
    b = (Beh*)beh;

    st = b->data;
    if (HasCondition(o->conditions, 9) || HasCondition(o->conditions, 10) || HasCondition(o->conditions, 11)) {
        st->unkC = 0;
        BehReset(b);
        BehDone(b);
    } else if (gThermoStates[st->cur].update) {
        gThermoStates[st->cur].update(owner, beh);
    }
    return 1;
}

int ThermoBehDestroy(void* owner, void* beh) {
    Owner* o;
    Beh* b;
    if (beh == 0) return 0;
    o = (Owner*)owner;
    b = (Beh*)beh;
    VFree(b->data);
    return 1;
}

int ThermoBehInit(void* owner, void* beh) {
    Owner* o;
    Beh* b;
    ThermoState* st;

    if (beh == 0) return 0;
    o = (Owner*)owner;
    b = (Beh*)beh;
    if (o == 0) return 0;

    st = (ThermoState*)VAlloc(sizeof(ThermoState));
    if (st == 0) {
        b->data = 0;
        return 0;
    }

    b->data = st;
    b->state = 0;
    st->cur = 0;
    st->next = 0;
    st->arg = 0;
    st->unkC = 0;
    st->sound = Fn800F8A98(0x1970BECE);
    if (st->sound == -1 || !Fn800F840C(o->beh.unk58, st->sound, 1)) {
        st->sound = -1;
    }
    if (gThermoStates[st->cur].enter) gThermoStates[st->cur].enter(owner, beh);
    return 1;
}

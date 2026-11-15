// u_80252F2C.cpp: the imp's attack sub-behaviour (0x80252F2C-0x80253614), split from u_80251CB8.cpp
// (its own 0.0f at 0x804D9E18).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHashSeed(const char* s, u32 h) {
    int i;

    if (s == 0) {
        return 0;
    }
    for (i = 0; s[i] != 0; i++) {
        h *= 31;
        h += (unsigned char)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
}

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();
    String(const char* s);
    ~String() {}
    virtual void Release(char* p);
};

struct Owner;
struct Actor;

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

// Six-word snapshot of an actor's collision source.
struct ZoneHitBox {
    u32 data[6];
};

// Position and handle of an owner.
struct Target {
    u8 pad[0x24];
    int x, y, z;
    u8 pad30[8];
    int id;  // 0x38
};

// The imp's per-actor state (VAlloc'd, 0x14 bytes).
struct ImpBeh {
    float timer;   // 0x0
    float near;    // 0x4: time spent close to the target
    float chase;   // 0x8
    float hurt;    // 0xC: cool-down of the hurt effect
    u8 state;      // 0x10
    u8 flags;      // 0x11
    s8 seed;       // 0x12
};

struct Actor {
    u8 pad00[0x24];
    int x, y, z;                        // 0x24
    int (*msgFn)(int msg, void* data);  // 0x30
    u8 pad34[4];
    int id;                             // 0x38
    u8 pad3C[4];
    void* model;                        // 0x40
    u8 pad44[0x84 - 0x44];
    Vec3 pos;                           // 0x84
    u8 pad90[0xD4 - 0x90];
    int unkD4;                          // 0xD4
    u8 padD8[0xFC - 0xD8];
    void* unkFC;                        // 0xFC
    u8 pad100[4];
    int unk104;                         // 0x104
    u8 pad108[4];
    void* unk10C;                       // 0x10C
    u8 pad110[0x130 - 0x110];
    ImpBeh* beh;                        // 0x130
    u8 pad134[4];
    float dt;                           // 0x138
    u8 pad13C[0x214 - 0x13C];
    int unk214;                         // 0x214
    u8 pad218[4];
    int unk21C;                         // 0x21C
    int unk220;                         // 0x220
    u8 pad224[0x22C - 0x224];
    Target* unk22C;                     // 0x22C
    u8 pad230[0x270 - 0x230];
    u8 st[0x18];                        // 0x270: condition block
    int unk288;                         // 0x288
    u8 pad28C[0x2C4 - 0x28C];
    int unk2C4;                         // 0x2C4
    u8 pad2C8[0x5FC - 0x2C8];
    void* unk5FC;                       // 0x5FC
    void* unk600;                       // 0x600
    void* unk604;                       // 0x604
    u8 pad608[0x894 - 0x608];
    u8 unk894[0x1E];                    // 0x894
    u16 nearMin;                        // 0x8B2
    u16 nearMax;                        // 0x8B4
    u8 pad8B6[0x8EC - 0x8B6];
    int unk8EC;                         // 0x8EC
    int unk8F0;                         // 0x8F0
    u8 pad8F4[0x900 - 0x8F4];
    float unk900;                       // 0x900
    float health;                       // 0x904
};

typedef int (*ImpStateFunc)(void* actor);

// One entry of the imp's state table (0x803BECD8, 0x10 bytes).
struct ImpStateType {
    ImpStateFunc enter;   // 0x0
    ImpStateFunc exit;    // 0x4
    ImpStateFunc update;  // 0x8
    const char* name;     // 0xC
};


// Message layouts. The offsets are the same across message types; each case uses its own view.
struct Msg400 {
    Actor* actor;   // 0x00
    int ownerId;    // 0x04
    int unk08;      // 0x08
    int state;      // 0x0C
    float f10;      // 0x10
    float f14;      // 0x14
    int flags;      // 0x18
    int unk1C;      // 0x1C
};

struct Msg407 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int unk08;      // 0x08
    float damage;   // 0x0C
    int unk10;      // 0x10
    u32 type;       // 0x14
    u8 pad18[0x44 - 0x18];
    int unk44;      // 0x44
};

struct Msg409 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int anim;       // 0x08
    int arg;        // 0x0C
};

struct Msg40A {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    u32 anim;       // 0x08
    int result;     // 0x0C
};

struct Msg41D {
    Actor* actor;   // 0x00
    u32 cmd;        // 0x04
    int unk08;      // 0x08
    int arg;        // 0x0C
};

struct Msg41E {
    Actor* actor;   // 0x00
    u32 cmd;        // 0x04
    int unk08;      // 0x08
    Buffer result;  // 0x0C
};

int OwnerId(Owner* o);                              // 0x800A3A40
void* VAlloc(int size);                             // 0x8009DA68
void VFree(void* p);                                // 0x8009DA3C
int HasCondition(void* conditions, int which);      // 0x800A7AC4
float OwnerValue(Owner* o, void* value);            // 0x800AA050
int ActorPlayAnim(int owner, u32 anim, int arg);    // 0x800C5654
void* FxFind(const char* name);
void ActorGetMatrix(Mtx34* m, Actor* a);
int ModelBoneHitBox(int a, int b, int c, int d);

extern "C" {
int rand(void);
int SendMsg(int to, int msg, void* data);
int fn_80019F88(void* data, Actor* actor);
String fn_800225C8(const String& a, const String& b);
void fn_80090390(int a, int b);
int fn_800B1EF4(Actor* actor, int kind);
void fn_800B1F20(Actor* actor, int kind);
void fn_800B20B8(Actor* actor, int a);
void fn_800B2418(Actor* actor, int kind, float f);
int fn_800B2454(Actor* actor, int kind);
void fn_800B261C(Actor* actor, int kind, void* out);
int fn_800B26B8(Actor* actor);
int fn_800BAF40(Actor* actor);
void fn_800BAFF8(Actor* actor, Vec3* dir, int a, int b, Vec3* to, float f1, float f2);
int fn_800BB218(Actor* actor, int flags, Vec3* from, Vec3* to, Actor* actor2, float* scale, int* a, int* b,
                float dist);
void fn_800BBCA8(Actor* actor);
void fn_800BC56C(Actor* actor, void* target);
void fn_800BC5A8(Actor* actor, Vec3* to);
int fn_800BF2D0(Vec3* p);
void fn_800CC984();
void fn_800CE96C();
void fn_800CF064();
void fn_800C748C();
float fn_800E1020(Vec3* v);
void fn_800E1098(void* out, void* in, float length);
void fn_80162610(Actor* actor, void* p, int a);
void fn_80162658(Actor* actor, void* p);
int fn_80162940(void* p, int a);
void fn_8016D864(ZoneHitBox* out, void* src);
int fn_8016F120(void* fx, int* pos);
void fn_8028C188(Buffer* b, char* data, int len);
}
int MemFlagIsSet(void* bits, int index);
int ConditionUpdateBit(void* cond, int bit, int set);
int ConditionSetBit(void* cond, int bit, int set);
void* FollowGetTargetObj(void* range);
void FollowSetTargetObj(void* range, void* obj);
int FollowOffsetUpdate(void* self);
void RangeResetPending(void* range);

extern void* lbl_804D3600;  // "trib_see" effect
extern void* lbl_804D3604;  // "ice_frozenimps" effect

// ---------------------------------------------------------------------------------------------------
// The imp's attack sub-behaviour: an eight-entry enter/exit/update table (0x803BED88) driven by the
// generic behaviour runner (BehUpdate and friends, 0x80161F40-0x80163xxx).

struct ImpAttackData {
    int state;   // 0x0
    int next;    // 0x4
    int unk8;
    u32 flags;   // 0xC
    int unk10;
};

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

struct Beh {
    u8 pad0[0x10];
    int state;                                       // 0x10
    u8 pad14[0x30 - 0x14];
    int targetKind;                                  // 0x30
    int target;                                      // 0x34
    u8 pad38[0x4C - 0x38];
    void (*onState)(void* owner, void* beh, int s);  // 0x4C
    u8 pad50[4];
    ImpAttackData* data;                             // 0x54
};

struct BehMsg {
    u8 pad[0x48];
    Actor* target;  // 0x48
    u8 pad4C[0x60 - 0x4C];
};

void BehUpdate(Owner* o, Beh* b);                         // 0x801632A8
void BehDone(Beh* b);                                     // 0x80162EBC
void BehReset(Beh* b);                                    // 0x801633D4
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f); // 0x800F7F5C
int Fn80161FF4(Owner* o, void* buf, float f);            // 0x80161FF4
void Fn8016295C(Owner* o, Beh* b);                        // 0x8016295C

void ImpAttackBehState0Enter(void* owner, void* beh);
void ImpAttackBehState0Exit(void* owner, void* beh);
void ImpAttackBehState0Update(void* owner, void* beh);
void ImpAttackBehState4Enter(void* owner, void* beh);
void ImpAttackBehState4Exit(void* owner, void* beh);
void ImpAttackBehState4Update(void* owner, void* beh);
void ImpAttackBehState5Enter(void* owner, void* beh);
void ImpAttackBehState5Exit(void* owner, void* beh);
void ImpAttackBehState5Update(void* owner, void* beh);

BehState gImpAttackBehStates[8] = {
    { ImpAttackBehState0Enter, ImpAttackBehState0Exit, ImpAttackBehState0Update },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { ImpAttackBehState4Enter, ImpAttackBehState4Exit, ImpAttackBehState4Update },
    { ImpAttackBehState5Enter, ImpAttackBehState5Exit, ImpAttackBehState5Update },
    { 0, 0, 0 },
    { 0, 0, 0 },
};

// Registered in gBehTypes (u_80161F40) under these names.
extern "C" {
int fn_80252F2C(void* owner, void* beh);
int fn_80252F34(void* owner, void* beh, int msg);
int fn_802531B4(void* owner, void* beh);
int fn_80253328(void* owner, void* beh);
int fn_80253360(void* owner, void* beh);
}

int fn_80252F2C(void* owner, void* beh) {
    return 0;
}

int fn_80252F34(void* owner, void* beh, int msg) {
    Actor* o = (Actor*)owner;
    Beh* b = (Beh*)beh;
    ImpAttackData* d = b->data;

    switch (msg) {
    case 1:
        d->next = 4;
        return 1;
    case 2:
        d->next = 5;
        return 1;
    case 0: {
        char buf[96];

        Fn800F7F5C(o->unk8EC, o->unk8F0, 0x1B83833F, OwnerId((Owner*)o), buf, o->dt);
        d->next = 0;
        return 1;
    }
    case 0x1B83833F: {
        char buf[96];

        Fn800F7F5C(o->unk8EC, o->unk8F0, 0x1B83833F, OwnerId((Owner*)o), buf, o->dt);
        d->flags |= 1;
        return 0;
    }
    case 0xF26951B8: {
        char buf[96];

        Fn800F7F5C(o->unk8EC, o->unk8F0, msg, OwnerId((Owner*)o), buf, o->dt);
        return 1;
    }
    case 0x0CAE7B51: {
        char buf[96];

        Fn800F7F5C(o->unk8EC, o->unk8F0, msg, OwnerId((Owner*)o), buf, o->dt);
        return 1;
    }
    case 0x20EE36: {
        BehMsg m;
        Mtx34 mtxA;
        Mtx34 mtxB;
        Msg400 q;
        ZoneHitBox boxA;
        ZoneHitBox boxB;

        if (b->targetKind == 2) {
            int id = b->target;

            if (id != -1) {
                Actor* t;

                q.ownerId = id;
                if (SendMsg(q.ownerId, 0x400, &q) != 0 && (t = q.actor) != 0) {
                    ActorGetMatrix(&mtxA, o);
                    ActorGetMatrix(&mtxB, t);
                    fn_8016D864(&boxA, o->model);
                    fn_8016D864(&boxB, t->model);
                    if (ModelBoneHitBox((int)&boxA, (int)&mtxA, (int)&boxB, (int)&mtxB)) {
                        m.target = t;
                        Fn800F7F5C(o->unk8EC, o->unk8F0, 0x20EE36, OwnerId((Owner*)o), &m, o->dt);
                        return 1;
                    }
                }
                return 0;
            }
        }
        return 0;
    }
    case 0xF829DB93:
        d->flags |= 4;
        return 1;
    }
    return 1;
}

int fn_802531B4(void* owner, void* beh) {
    Actor* o;
    Beh* b;
    ImpAttackData* d;

    if (!beh) {
        return 0;
    }
    o = (Actor*)owner;
    b = (Beh*)beh;
    d = b->data;
    if (HasCondition(o->st, 9) || HasCondition(o->st, 10) || HasCondition(o->st, 11)) {
        d->flags = 0;
        BehReset(b);
        BehDone(b);
    } else {
        if (gImpAttackBehStates[d->state].update) {
            gImpAttackBehStates[d->state].update(o, b);
        }
        if (d->next != d->state) {
            if (gImpAttackBehStates[d->state].exit) {
                gImpAttackBehStates[d->state].exit(o, b);
            }
            d->state = d->next;
            b->state = d->next;
            if (b->onState) {
                b->onState(o, b, d->next);
            }
            if (gImpAttackBehStates[d->state].enter) {
                gImpAttackBehStates[d->state].enter(o, b);
            }
        }
    }
    return 1;
}

int fn_80253328(void* owner, void* beh) {
    if (!beh) {
        return 0;
    }
    VFree(((Beh*)beh)->data);
    return 1;
}

int fn_80253360(void* owner, void* beh) {
    Beh* b;
    ImpAttackData* d;

    if (!beh) {
        return 0;
    }
    b = (Beh*)beh;
    d = (ImpAttackData*)VAlloc(sizeof(ImpAttackData));
    if (d == 0) {
        b->data = 0;
        return 0;
    }
    b->data = d;
    b->state = 0;
    d->state = 0;
    d->next = 0;
    d->flags = 0;
    if (gImpAttackBehStates[d->state].enter) {
        gImpAttackBehStates[d->state].enter(owner, b);
    }
    return 1;
}

void ImpAttackBehState5Update(void* owner, void* beh) {
    char buf[96];

    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        ImpAttackData* d = b->data;

        BehUpdate(o, b);
        Fn8016295C(o, b);
        Fn80161FF4(o, buf, 0.0f);
        Fn800F7F5C(((Actor*)o)->unk8EC, ((Actor*)o)->unk8F0, 0xF829DB93, OwnerId(o), buf, ((Actor*)o)->dt);
        d->next = 0;
    }
}

void ImpAttackBehState4Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        ImpAttackData* d = b->data;

        BehUpdate(o, b);
        Fn8016295C(o, b);
        if (d->flags & 1) {
            d->next = 0;
        }
    }
}

void ImpAttackBehState0Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}

void ImpAttackBehState5Exit(void* owner, void* beh) {}

void ImpAttackBehState4Exit(void* owner, void* beh) {}

void ImpAttackBehState0Exit(void* owner, void* beh) {}

void ImpAttackBehState5Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        b->data->flags = 0;
        BehDone(b);
    }
}

void ImpAttackBehState4Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        b->data->flags = 0;
        BehDone(b);
    }
}

void ImpAttackBehState0Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;
        ImpAttackData* d = b->data;

        d->flags = 0;
        d->next = 0;
        BehDone(b);
    }
}

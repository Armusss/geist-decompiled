// u_800E9418.cpp: the gun actor(s) (0x800E9418-0x800EB1B8).
// The actor has a per-slot node table (sGunNodes) indexed by its state: each entry carries three
// callbacks (init/update/idle) and the asset hashes the init callbacks hand to the host. This file
// contains the callbacks for the first four entries, the three per-frame state checks they run, the
// message handler that forwards node changes to the owner, and the four small table dispatchers.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

// "new"/"free" query: the receiver writes its answer over the first word of the string.
struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

// Message 0xC06: ask the target whether it knows the node for a given message.
struct NodeQuery {
    int unk0;
    int msg;
    int unk8;
    int target;
    int unk10[4];
};

struct Actor;

// One entry of the per-gun node table (0x48 bytes each, 25 entries at 0x8039C570).
struct WeaponNode {
    void (*fn0)(Actor*);
    void (*fn1)(Actor*, float);
    void (*fn2)(Actor*, WeaponNode*);
    int unk0C[14];
    int unk44;
};

// The host's per-instance data block, reached through the actor's host array.
struct HostData {
    char pad00[0x14];
    int i14;
    int i18;
    int i1C;
    char pad20[4];
    int i24;
};

// The gun actor. Only the fields this file touches are named.
struct Actor {
    char pad00[0x24];
    int x;                                     // 0x24
    int y;                                     // 0x28
    int z;                                     // 0x2C
    char pad30[8];
    int target;                                // 0x38
    char pad3C[0x40];
    int model;                                 // 0x7C
    char pad80[0xC];
    int (*msgHandler)(int msg, void* data);    // 0x8C
    char pad90[8];
    int unk98;                                 // 0x98
    char pad9C[8];
    int unkA4;                                 // 0xA4
    char padA8[0x3C];
    int (*msgHandlerE4)(int msg, void* data);  // 0xE4
    char padE8[8];
    int unkF0;                                 // 0xF0
    char padF4[8];
    int unkFC;                                 // 0xFC
    char pad100[0x1F4];
    char unk2F4[0x20];                         // 0x2F4
    int unk314;                                // 0x314
    int unk318;                                // 0x318
    char pad31C[4];
    int unk320;                                // 0x320
    int unk324;                                // 0x324
    int unk328;                                // 0x328
    char pad32C[0x60];
    float unk38C;                              // 0x38C
    float unk390;                              // 0x390
    float unk394;                              // 0x394
    float unk398;                              // 0x398
    char pad39C[0x1EC];
    int unk588;                                // 0x588
    char pad58C[0x5C];
    float unk5E8;                              // 0x5E8
    char pad5EC[0x4C];
    int unk638;                                // 0x638
    char pad63C[0x3C];
    unsigned long unk678;                      // 0x678
    char pad67C[0xC0];
    int unk73C;                                // 0x73C
    char pad740[0x64];
    int state;                                 // 0x7A4
    int unk7A8;                                // 0x7A8
    char pad7AC[8];
    unsigned long flags;                       // 0x7B4
    char pad7B8[0x18];
    int unk7D0;                                // 0x7D0
    int unk7D4;                                // 0x7D4
    float unk7D8;                              // 0x7D8
    char pad7DC[0x24];
    float unk800;                              // 0x800
    char pad804[0xCF8];
    float unk14FC;                             // 0x14FC
    char pad1500[0x804];
    char buf[0x50];                            // 0x1D04
    float unk1D54;                             // 0x1D54
    char pad1D58[0x96C];
    void* hosts[1];                            // 0x26C4
    u32* unk26C8;                              // 0x26C8
};

// The table and the asset ids it fills in belong to units without a split here.
extern WeaponNode sGunNodes[25];               // 0x8039C570
extern int sGunAssetIds[16];                   // 0x80455A10

// This unit's small-data block.
static int sGunFx = -1;

// Mangled C++ callees.
void* FxFind(const char* name);                             // 0x80170834
int AssetFind(u32 table, u32 key);                          // 0x8007EB34
int PlayerInHost(Actor* a);                                 // 0x80049750
int ActorPlayAnim(int anim, u32 name, int arg);             // 0x800C5654
void DashUpdate(Actor* a);                                  // 0x80243DC8
int Fn800F7DEC(int a, int b, int c, int d, void* e);        // 0x800F7DEC
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f); // 0x800F7F5C
u32 Fn800F8A98(u32 key);                                    // 0x800F8A98

extern "C" {
int SendMsg(int to, int msg, void* data);                   // 0x800265D4
int FxStart(void* fx, int a, int b);                        // 0x8016F390
void fn_80048BA8(Actor* a, void* buf, int flag);            // 0x80048BA8
int fn_800498D0(Actor* a);                                  // 0x800498D0
void fn_800699A8(void* p);                                  // 0x800699A8
float fn_800699B8(Actor* a);                                // 0x800699B8
void fn_8006D17C(int* p, int v);                            // 0x8006D17C
void fn_8006D184(Actor* a, int v);                          // 0x8006D184
void fn_8006D288(Actor* a, int v);                          // 0x8006D288
void fn_8006D76C(Actor* a);                                 // 0x8006D76C
void fn_80072058(Actor* a, float dt);                       // 0x80072058
void fn_800769A0(Actor* a, float dt);                       // 0x800769A0
void fn_8008E824(int anim, int a, float speed);             // 0x8008E824
int fn_800A1BC8(int a, int b, u32 type, int c, int d);      // 0x800A1BC8
int fn_800C5210(int target, int msg, void* data);           // 0x800C5210
int fn_800F7994(int a, u32 key);                            // 0x800F7994
int fn_802A1D04(void);                                      // 0x802A1D04
int rand(void);                                             // 0x802F2CFC
}

int GunMsg(int msg, void* data);
int GunCheckCommon(Actor* a, float dt);
int GunCheckState8(Actor* a);
int GunCheckState5(Actor* a);

// One node query (0x41E), answered into a string the receiver overwrites with its node id.
inline void Query(int* out, int target, int msg, u32 hash) {
    QueryMsg m;

    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    *out = *(int*)&m.result;
}

// Same message, no answer expected: the receiver overwrites the preset value.
inline void SendNodeMsg(int target, int msg, u32 hash, int value) {
    QueryMsg m;

    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
}

// Frame update for the state-3 node.
void GunUpdateState3(Actor* a, float dt) {
    int q1;
    int q2;

    Query(&q1, a->target, 0x41E, 0x2DA8293C);
    Query(&q2, a->target, 0x41E, 0xDFF3097A);

    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    a->unk7D0 = 1;

    float f0;
    float f1;
    f0 = fabsf(a->unk394);
    f1 = fabsf(a->unk38C);
    f1 = (f0 > f1) ? f0 : f1;
    if (f1 != 0.0f) {
        fn_8008E824(a->model, 0, f1);
    }

    if (a->unk7D8 == 0.0f) {
        fn_8006D184(a, 0);
    } else if (a->unk678 & 0x80) {
        a->unk678 &= ~0x80;
        fn_8006D288(a, 3);
    } else {
        fn_8006D17C(&a->state, 3);
        if (a->unk26C8 != 0 && *(int*)((char*)a->unk26C8 + 0x12C) != 0xA921FADD) {
            a->flags &= ~0x200000;
        }
        DashUpdate(a);
        if (!GunCheckState5(a)) {
            if (!GunCheckState8(a)) {
                GunCheckCommon(a, dt);
            }
        }
    }
}

// Set up the animations for the state-3 node, retrying while the flag is still set.
void GunInitState3(Actor* a) {
    int idx;
    int q1;
    int q2;

    Query(&q1, a->target, 0x41E, 0x2DA8293C);
    Query(&q2, a->target, 0x41E, 0xDFF3097A);

    int host = PlayerInHost(a);
    idx = a->unk73C;
    int r = Fn800F7DEC(q1, q2, 0x20EE36, fn_800498D0(a), 0);
    int flag = 0;

    if (r == 0xC0BECF7E || r == 0x11D2C025) {
        flag = 1;
    }

    a->unk98 = -1;
    while (a->unk98 == -1) {
        a->unk98 = fn_800A1BC8(((HostData*)a->hosts[host])->i14,
                               ((HostData*)a->hosts[host])->i18, sGunNodes[3].unk0C[idx], flag, 0);
        a->msgHandler = GunMsg;
        a->unkA4 = 1;
        if (a->unk98 == -1 && flag) {
            flag = 0;
        }
    }

    if (a->flags & 0x80000) {
        HostData* n = (HostData*)a->hosts[host];

        a->unkF0 = fn_800A1BC8(n->i14, n->i24, sGunNodes[3].unk0C[idx], 0, 0);
    } else {
        HostData* n = (HostData*)a->hosts[host];

        a->unkF0 = fn_800A1BC8(n->i14, n->i1C, sGunNodes[3].unk0C[idx], 0, 0);
    }
    a->unkFC = 1;

    if (a->unk26C8 != 0 && *(int*)((char*)a->unk26C8 + 0x12C) != 0xA921FADD) {
        a->flags &= ~0x200000;
    }
    DashUpdate(a);
}

// Plays the state-2 node animation on the target.
void GunAnimState2(Actor* a) {
    ActorPlayAnim(0x2000000, 0x8B3A27A3, fn_800498D0(a));
}

// Frame update for the state-2 node.
void GunUpdateState2(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    if (a->unk26C8 != 0 && *(int*)((char*)a->unk26C8 + 0x12C) != 0xA921FADD) {
        a->flags &= ~0x200000;
    }
    DashUpdate(a);
    fn_8006D17C(&a->state, 3);
}

// Set up the state-2 node and start its effect.
void GunInitState2(Actor* a) {
    int q;

    Query(&q, a->target, 0x41E, 0xDFF3097A);

    int host = PlayerInHost(a);
    int idx = a->unk73C;

    a->unk98 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i18,
                           sGunNodes[2].unk0C[idx], 0, 0);
    a->unkA4 = 0x10;
    a->msgHandler = GunMsg;

    if (a->flags & 0x80000) {
        a->unkF0 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i24,
                               sGunNodes[2].unk0C[idx], 0, 0);
    } else {
        a->unkF0 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i1C,
                               sGunNodes[2].unk0C[idx], 0, 0);
    }
    a->unkFC = 0x10;

    sGunFx = fn_800F7994(q, 0xAFEF562B);
    FxStart((void*)sGunFx, 0, 0);
}

// Frame update for the state-1 node.
void GunUpdateState1(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    if (a->unk678 & 0x80) {
        a->unk678 &= ~0x80;
        fn_8006D288(a, 1);
    }
    fn_8006D17C(&a->state, 1);
    a->unk7D0 = 0;
    if (a->unk26C8 != 0 && *(int*)((char*)a->unk26C8 + 0x12C) != 0xA921FADD) {
        a->flags &= ~0x200000;
    }
    DashUpdate(a);
    if (!GunCheckState5(a)) {
        if (!GunCheckState8(a) && (a->unk320 & 1)) {
            if (a->unk7D0 == 0) {
                fn_8006D184(a, 2);
            } else {
                fn_8006D184(a, 9);
            }
        }
    }
}

// Set up the state-1 node.
void GunInitState1(Actor* a) {
    int host = PlayerInHost(a);
    int idx = a->unk73C;

    a->unk98 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i18,
                           sGunNodes[1].unk0C[idx], 0, 0);
    a->unkA4 = 1;
    a->msgHandler = 0;

    if (a->flags & 0x80000) {
        a->unkF0 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i24,
                               sGunNodes[1].unk0C[idx], 0, 0);
    } else {
        a->unkF0 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i1C,
                               sGunNodes[1].unk0C[idx], 0, 0);
    }
    a->msgHandlerE4 = GunMsg;
    a->unkFC = 1;
    a->unk7D0 = 0;
}

// Idle callback for the state-0 node.
void GunIdleState0(Actor* a) {
    fn_8006D76C(a);
}

// Frame update for the state-0 node.
void GunUpdateState0(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    a->unk7D0 = 1;
    fn_8006D17C(&a->state, 3);
    if (a->unk26C8 != 0 && *(int*)((char*)a->unk26C8 + 0x12C) != 0xA921FADD) {
        a->flags &= ~0x200000;
    }
    DashUpdate(a);
    if (!GunCheckState5(a) && !GunCheckState8(a)) {
        GunCheckCommon(a, dt);
    }
    if (a->unk394 != 0.0f || a->unk38C != 0.0f || a->unk390 != 0.0f || a->unk398 != 0.0f ||
        a->unk328) {
        fn_8006D288(a, 3);
    }
}

// Set up the state-0 node, picking a random sub-node when the owner is undecided.
void GunInitState0(Actor* a) {
    int q1;
    int q2;

    Query(&q1, a->target, 0x41E, 0x2DA8293C);
    Query(&q2, a->target, 0x41E, 0xDFF3097A);

    int r = Fn800F7DEC(q1, q2, 0x20EE36, fn_800498D0(a), 0);

    if (r == 0xC0BECF7E || r == 0x11D2C025) {
        a->unk7D4 = 2;
    } else if (((float)rand() / 32767.0f) >= 0.5f) {
        a->unk7D4 = 0;
    } else {
        a->unk7D4 = 1;
    }

    int host = PlayerInHost(a);
    int idx = a->unk73C;

    a->unk98 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i18,
                           sGunNodes[0].unk0C[0], a->unk7D4, 0);
    if (a->unk98 == -1) {
        a->unk7D4 = 0;
        a->unk98 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i18,
                               sGunNodes[0].unk0C[0], a->unk7D4, 0);
    }
    a->msgHandler = GunMsg;
    a->unkA4 = 1;

    int anim;
    HostData* n = (HostData*)a->hosts[host];
    int i14 = n->i14;
    if (a->flags & 0x80000) {
        anim = n->i24;
    } else {
        anim = n->i1C;
    }

    a->unkF0 = fn_800A1BC8(i14, anim, sGunNodes[0].unk0C[0], a->unk7D4, 0);
    if (a->unkF0 == -1) {
        a->unk7D4 = 0;
        a->unkF0 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, anim, sGunNodes[0].unk0C[0],
                               a->unk7D4, 0);
    }
    a->msgHandlerE4 = GunMsg;
    a->unkFC = 1;
}

// Per-frame state dispatch: advances through the firing/aiming sub-states.
int GunCheckCommon(Actor* a, float dt) {
    int q1;
    int q2;

    Query(&q1, a->target, 0x41E, 0x2DA8293C);
    Query(&q2, a->target, 0x41E, 0xDFF3097A);

    if (a->unk588 == 5 || a->unk588 == 4) {
        if (a->unk320 & 1) {
            fn_8006D76C(a);
            if (a->unk7D0 == 0) {
                fn_8006D184(a, 2);
            } else {
                fn_8006D184(a, 9);
            }
            return 1;
        }
        if (a->unk324 & 1) {
            fn_8006D76C(a);
            if (Fn800F7DEC(q1, q2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 10);
                return 1;
            }
            if (Fn800F7DEC(q1, q2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 0xB);
                return 1;
            }
        }
    } else if (a->unk320 != 0) {
        fn_8006D76C(a);
        a->unk5E8 += dt;
        if (fn_800699B8(a) == 1.0f) {
            if (a->unk7D0 == 0) {
                fn_8006D184(a, 2);
            } else {
                fn_8006D184(a, 9);
            }
            fn_800699A8(&a->unk2F4);
            return 1;
        }
    } else {
        float d = fn_800699B8(a);

        if (d > -1.0f && d < 0.0f) {
            if (Fn800F7DEC(q1, q2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 10);
                fn_800699A8(&a->unk2F4);
                return 1;
            }
            if (Fn800F7DEC(q1, q2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 0xB);
                fn_800699A8(&a->unk2F4);
                return 1;
            }
        }
        fn_800699A8(&a->unk2F4);
    }
    return 0;
}

// State-8 dispatch: reacts to the owner's node query result.
int GunCheckState8(Actor* a) {
    int q1;
    int q2;

    Query(&q1, a->target, 0x41E, 0x2DA8293C);
    Query(&q2, a->target, 0x41E, 0xDFF3097A);

    if (a->unk318 >= 1) {
        if (a->state == 1) {
            fn_8006D184(a, 2);
            return 1;
        }
        fn_8006D76C(a);
        int r = Fn800F7DEC(q1, q2, 0xF829DB93, fn_800498D0(a), 0);

        switch (r) {
        case 0x04A3E183:
            if (a->state != 8) {
                fn_8006D184(a, 8);
                return 1;
            }
            break;
        case 0xC0BECF7E:
            if (Fn800F7DEC(q1, q2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 0xB);
                return 1;
            }
            break;
        case 0x11D2C025:
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                SendNodeMsg(a->target, 0x41D, 0x7FFCFD4F, 0x91A0CEF7);
            }
            if (a->state == 8) {
                fn_8006D184(a, 3);
            }
            break;
        case 0x3DA260F7:
            if (a->state == 8) {
                fn_8006D184(a, 3);
            }
            break;
        }
    } else {
        if (a->state == 8) {
            fn_8006D184(a, 3);
        }
    }
    return 0;
}

// State-5 dispatch: reacts to the owner's node query result.
int GunCheckState5(Actor* a) {
    int q1;
    int q2;

    Query(&q1, a->target, 0x41E, 0x2DA8293C);
    Query(&q2, a->target, 0x41E, 0xDFF3097A);

    if (a->unk314 >= 1) {
        if (a->state == 1) {
            fn_8006D184(a, 2);
            return 1;
        }
        fn_8006D76C(a);
        int r = Fn800F7DEC(q1, q2, 0x20EE36, fn_800498D0(a), 0);

        switch (r) {
        case 0x04A3E183:
            fn_8006D184(a, 5);
            return 1;
        case 0xC0BECF7E:
            if (Fn800F7DEC(q1, q2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 10);
                return 1;
            }
            break;
        case 0x11D2C025:
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                SendNodeMsg(a->target, 0x41D, 0x7FFCFD4F, 0x91A0CEF7);
            }
            if (a->state == 5) {
                fn_8006D184(a, 3);
            }
            break;
        case 0x3DA260F7:
            if (a->state == 5) {
                fn_8006D184(a, 3);
            }
            break;
        }
    } else {
        if (a->state == 5) {
            fn_8006D184(a, 3);
        }
    }
    return 0;
}

// Message handler: answers node queries and forwards node changes to the owner.
int GunMsg(int msg, void* data) {
    Actor* a = (Actor*)data;
    NodeQuery nq;

    nq.target = a->target;
    nq.msg = msg;
    if (fn_800C5210(a->target, 0xC06, &nq) == 1) {
        return 0;
    }

    int q1;
    int q2;
    Query(&q1, a->target, 0x41E, 0x2DA8293C);
    Query(&q2, a->target, 0x41E, 0xDFF3097A);

    switch (msg) {
    case 0x1B83833F:
        if (a->state == 5) {
            int r = Fn800F7DEC(q1, q2, 0x20EE36, fn_800498D0(a), 0);

            if (r != 0x04A3E183) {
                if (r == 0xC0BECF7E || r == 0x11D2C025) {
                    ActorPlayAnim(a->target, 0xA7D124DE, a->target);
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                fn_8006D184(a, 3);
            } else {
                fn_8006D288(a, a->unk7A8);
            }
        } else if (a->state == 8) {
            int r = Fn800F7DEC(q1, q2, 0xF829DB93, fn_800498D0(a), 0);

            if (r != 0x04A3E183) {
                if ((q2 == (int)Fn800F8A98(0x7990FA64) || q2 == (int)Fn800F8A98(0xF8B88EC8)) &&
                    r != 0x11D2C025) {
                    fn_8006D184(a, 0xB);
                } else {
                    if (r == 0xC0BECF7E || r == 0x11D2C025) {
                        FxStart(FxFind("gun_empty"), 0, 0);
                    }
                    fn_8006D184(a, 3);
                }
            } else {
                fn_8006D288(a, a->unk7A8);
            }
        } else if (a->state == 0xD) {
            a->flags |= 0x20000;
        } else {
            fn_8006D288(a, a->unk7A8);
        }
        break;
    case 0x20EE36:
        fn_80048BA8(a, a->buf, 1);
        a->unk1D54 = a->unk800;
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        if (q2 == (int)Fn800F8A98(0x04C4EEE3)) {
            return 1;
        }
        break;
    case 0xF829DB93:
        fn_80048BA8(a, a->buf, 0);
        a->unk1D54 = a->unk800;
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        break;
    case 0x9791AF51:
    case 0x96224FCC:
    case 0x42E50A3D:
    case 0x0D19E338:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        break;
    case 0xE6B71BCC:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        ActorPlayAnim(a->target, 0xBC805C38, a->target);
        break;
    case 0x968CBE3D:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        break;
    }
    return 0;
}

// Node table dispatchers.
int GunNodeValue(Actor* a) {
    return sGunNodes[a->state].unk44;
}

void GunNodeFn2(Actor* a) {
    if (sGunNodes[a->state].fn2) {
        sGunNodes[a->state].fn2(a, &sGunNodes[a->state]);
    }
}

void GunNodeFn1(Actor* a, float dt) {
    if (a->state == 5 || a->state == 8) {
        if (a->unk800 < 1.0f) {
            a->unk800 += 0.0005f * dt;
        }
        a->unk800 = (a->unk800 < 1.0f) ? a->unk800 : 1.0f;
    } else {
        if (a->unk800 > 0.0f) {
            a->unk800 -= 0.001f * dt;
        }
        a->unk800 = (a->unk800 > 0.0f) ? a->unk800 : 0.0f;
    }

    if (sGunNodes[a->state].fn1) {
        sGunNodes[a->state].fn1(a, dt);
    }
}

void GunNodeFn0(Actor* a) {
    WeaponNode* n = &sGunNodes[a->state];

    if (n->fn0) {
        n->fn0(a);
    }
}

// Resolve the assets the gun states use.
void GunLoadAssets(void) {
    sGunAssetIds[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    sGunAssetIds[1] = AssetFind(0xF1754BF2, 0x0024805F);
    sGunAssetIds[2] = AssetFind(0xF1754BF2, 0x101844CC);
    sGunAssetIds[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    sGunAssetIds[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    sGunAssetIds[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    sGunAssetIds[6] = AssetFind(0xF1754BF2, 0xD9404234);
    sGunAssetIds[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    sGunAssetIds[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    sGunAssetIds[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    sGunAssetIds[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    sGunAssetIds[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    sGunAssetIds[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    sGunAssetIds[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    sGunAssetIds[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    sGunAssetIds[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

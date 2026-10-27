// u_8021427C.cpp: ghost action (0x8021427C-0x802154B0).
// The action a ghost runs when it possesses a prop: two per-frame updates (one for the "ride"
// variant, one for the "roll" variant that also spins the model), the setup/teardown pair, the
// message handler that answers the host's node-change notifications, and the prop-type table
// callbacks. Both updates finish by asking the owner for the current target, measuring the
// distance to it and flagging the actor when the target is close enough or when the host's
// distance window is open.
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

struct Vec3 {
    float x, y, z;
};

struct Vec3i {
    int x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct Actor;

// Reply to message 0x41A: the owner's current target id, -1 when there is none.
struct Msg41A {
    int unk0;
    int target;
};

// Message 0x400: view of the target actor.
struct Msg400 {
    Actor* actor;
    int ownerId;
    int unk08;
    int state;
    float f10;
    float f14;
    int flags;
    int unk1C;
};

// The host's per-instance data block, reached through the actor's host array.
struct HostData {
    char pad00[0x14];
    int i14;
    int i18;
    int i1C;
};

// The ghost that owns this action (actor+0x26E4).
struct Ghost {
    char pad00[0x38];
    u32 unk38;                                 // target of the ghost, only the top byte matters
    char pad3C[0x48];
    int* unk84;
};

// One entry of the prop-type table (0x48 bytes each, 25 entries at 0x803BC148).
struct GhostActionType {
    void (*fn0)(Actor*);
    void (*fn1)(Actor*, int);
    void (*fn2)(Actor*);
    int unk0C[14];
    int unk44;
};

// The prop the ghost is acting on. Only the fields this file touches are named.
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
    char padA8[0x48];
    int unkF0;                                 // 0xF0
    char padF4[8];
    int unkFC;                                 // 0xFC
    char pad100[0x28C];
    float unk38C;                              // 0x38C
    char pad390[4];
    float unk394;                              // 0x394
    char pad398[0x24C];
    int unk5E4;                                // 0x5E4
    char pad5E8[0x28];
    float unk610;                              // 0x610
    float unk614;                              // 0x614
    char pad618[0x20];
    int unk638;                                // 0x638
    char pad63C[0x14];
    Vec3 unk650;                               // 0x650
    char pad65C[0x1C];
    unsigned long unk678;                      // 0x678
    char pad67C[0xBC];
    float* unk738;                             // 0x738
    int unk73C;                                // 0x73C
    char pad740[0x64];
    int state;                                 // 0x7A4
    int unk7A8;                                // 0x7A8
    char pad7AC[8];
    unsigned long flags;                       // 0x7B4
    char pad7B8[0x18];
    int unk7D0;                                // 0x7D0
    char pad7D4[0x28];
    float unk7FC;                              // 0x7FC
    char pad800[0xCFC];
    float unk14FC;                             // 0x14FC
    char pad1500[0xB0];
    int unk15B0;                               // 0x15B0
    char pad15B4[0x750];
    char buf[0x100];                           // 0x1D04
    char pad1E04[0x8C0];
    void* hosts[1];                            // 0x26C4
    u32* unk26C8;                              // 0x26C8
    char pad26CC[0x18];
    Ghost* ghost;                              // 0x26E4
};

// The ghost's roll object whose pointer list is freed when the action is deactivated.
struct RollData {
    void* unk00[20];                           // 0x00
    int unk50;                                 // 0x50
    char pad54[0x2C];
    int unk80;                                 // 0x80
    int unk84;                                 // 0x84
    int unk88;                                 // 0x88
    int unk8C;                                 // 0x8C
    int unk90;                                 // 0x90
    int unk94;                                 // 0x94
    int unk98;                                 // 0x98
    void (*unk9C)(void*);                      // 0x9C
    int unkA0;                                 // 0xA0
    int unkA4;                                 // 0xA4
    int unkA8;                                 // 0xA8
    int unkAC;                                 // 0xAC
    char padB0[4];
    int unkB4;                                 // 0xB4
};

// This unit's small-data block.
static int sSqueekFx = -1;
static int sTextureId = -1;
static int sRollFx = -1;
static float sFarRange = 800.0f;
static float sNearRange = 120.0f;
static float sTargetRange = 1000.0f;

// Data owned by other units.
extern GhostActionType lbl_803BC148[25];       // 0x803BC148
extern int lbl_8049E260[16];                   // 0x8049E260
extern RollData* lbl_8049EE58[4];              // 0x8049EE58
extern int lbl_804D345C;                       // 0x804D345C
extern int lbl_804D53D4;                       // 0x804D53D4

// Mangled C++ callees.
int AssetFind(u32 table, u32 key);                          // 0x8007EB34
void* FxFind(const char* name);                             // 0x80170834
void VFree(void* p);                                        // 0x8009DA3C
int PlayerInHost(Actor* a);                                 // 0x800A45AC
void ActorGetMatrix(Mtx34* m, Actor* a);                    // 0x8009ED70
void TextureLoad(short* tex, int a);                        // 0x80010A58
int Fn800F7DEC(int a, int b, int c, int d, void* e);        // 0x800F7DEC
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f); // 0x800F7F5C

extern "C" {
int SendMsg(int to, int msg, void* data);                   // 0x800265D4
int FxStart(void* fx, int a, int b);                        // 0x8016F390
void fn_8000F550(short* tex, int a);                        // 0x8000F550
void fn_80048BA8(Actor* a, void* buf, int flag);            // 0x80048BA8
int fn_800498D0(Actor* a);                                  // 0x800498D0
void fn_800769A0(Actor* a, float dt);                       // 0x800769A0
void fn_8008E824(int anim, int a, float speed);             // 0x8008E824
void fn_800A0030(Vec3* out, Mtx34* m, Vec3* in);            // 0x800A0030
int fn_800A1BC8(int a, int b, u32 type, int c, int d);      // 0x800A1BC8
int fn_800C5210(int target, int msg, void* data);           // 0x800C5210
int fn_800E0DBC(Vec3i* v);                                  // 0x800E0DBC
float fn_800E1020(Vec3* v);                                 // 0x800E1020
void fn_8016EEF4(int fx);                                   // 0x8016EEF4
void fn_8016F018(int fx, int value);                        // 0x8016F018
void fn_8021DC40(void* p);                                  // 0x8021DC40
void fn_8021DCF4(void* p);                                  // 0x8021DCF4
}
void PlayerSelectCollisionUpdate(void* owner, float dt);
void PlayerSetValue4(void* owner, int value);
void PlayerSetActionMode(void* owner, int mode);
void PlayerSetHeldSlotMode(void* owner, int mode);

int GhostActionMsg(int msg, void* data);

// One node query (0x41E), answered into a string the receiver overwrites with its node id.
inline void Query(int* out, int target, int msg, u32 hash) {
    QueryMsg m;

    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    *out = *(int*)&m.result;
}

// Ask the owner for the current target and flag the actor when it is in range.
static inline void CheckTarget(Actor* a) {
    Msg41A m;
    Vec3i delta;
    Msg400 m400;

    SendMsg(a->ghost->unk38, 0x41A, &m);
    if (m.target != -1) {
        SendMsg(m.target, 0x400, &m400);
        Actor* t = m400.actor;

        delta.x = t->x - a->x;
        delta.y = t->y - a->y;
        delta.z = t->z - a->z;
        int dist = fn_800E0DBC(&delta);

        if (a->unk650.x != 0.0f || a->unk650.y != 0.0f) {
            QueryMsg qm;

            qm.hash = 0xF9D0235B;
            *(int*)&qm.result = 1;
            SendMsg(m.target, 0x41D, &qm);
        }

        if (a->flags & 0x4000000) {
            float d = a->unk610 - a->unk614;

            if (d < sNearRange) {
                a->unk5E4 = 1;
            } else if (d > sFarRange) {
                a->unk15B0 = sTextureId;
                a->unk5E4 = 1;
            } else {
                a->unk15B0 = -1;
            }
        } else {
            if (dist < sTargetRange && a->unk650.x == 0.0f && a->unk650.y == 0.0f) {
                a->unk5E4 = 1;
            }
        }
    }
}

// Frame update for the riding/holding variant.
void UpdateGhostAction(Actor* a, float dt) {
    int q1;
    int q2;

    Query(&q1, a->target, 0x41E, 0x2DA8293C);
    Query(&q2, a->target, 0x41E, 0xDFF3097A);

    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    a->unk7D0 = 1;

    float m;
    {
        float z = a->unk394;
        float x = a->unk38C;
        m = __fabsf(x) > __fabsf(z) ? __fabsf(x) : __fabsf(z);
    }
    if (m != 0.0f) {
        fn_8008E824(a->model, 0, m);
    }

    if (a->unk678 & 0x80) {
        a->unk678 &= ~0x80;
        PlayerSetActionMode(a, 3);
    } else {
        PlayerSetValue4(&a->state, 3);
    }

    CheckTarget(a);
}

// Set up the two model animations and install the message handler.
void SetupGhostAction(Actor* a) {
    int q1;
    int q2;

    Query(&q1, a->target, 0x41E, 0x2DA8293C);
    Query(&q2, a->target, 0x41E, 0xDFF3097A);

    int host = PlayerInHost(a);
    int idx = a->unk73C;

    Fn800F7DEC(q1, q2, 0x20EE36, fn_800498D0(a), 0);

    a->unk98 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i18, lbl_803BC148[3].unk0C[idx], 0, 0);
    a->msgHandler = GhostActionMsg;
    a->unkA4 = 1;
    a->unkF0 = fn_800A1BC8(((HostData*)a->hosts[host])->i14, ((HostData*)a->hosts[host])->i1C, lbl_803BC148[3].unk0C[idx], 0, 0);
    a->unkFC = 1;
}

// Frame update for the rolling variant (also drives the roll effect and the spin).
void UpdateGhostActionIdle(Actor* a, float dt) {
    float fade = 0.0f;

    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);

    if (a->unk678 & 0x80) {
        a->unk678 &= ~0x80;
        PlayerSetActionMode(a, 1);
    }
    PlayerSetValue4(&a->state, 1);
    a->unk7D0 = 0;

    if (sRollFx != -1) {
        int speed = (int)(4096.0f * (fn_800E1020(&a->unk650) / a->unk738[2]));

        speed = (speed < 0) ? 0 : ((speed > 0x1000) ? 0x1000 : speed);
        fn_8016F018(sRollFx, speed);
    }

    if (a->ghost != 0 && (int)(a->ghost->unk38 >> 24) == 1) {
        switch (*a->ghost->unk84) {
        case 0x6EF9D8F1:
        case 0xDE71952A:
        case 0x7AABF5C8:
            fade = 0.3f;
            break;
        }
        if (fade > 0.0f) {
            Mtx34 m;
            Vec3 v;

            ActorGetMatrix(&m, a);
            fn_800A0030(&v, &m, &a->unk650);
            a->unk7FC -= v.y * (a->unk14FC * fade);
            while (a->unk7FC >= 360.0f) {
                a->unk7FC -= 360.0f;
            }
            while (a->unk7FC <= -360.0f) {
                a->unk7FC += 360.0f;
            }
        }
    }

    CheckTarget(a);
}

// Tear the action down.
void ResetGhostAction(Actor* a) {
    PlayerInHost(a);
    a->unk98 = -1;
    a->unkF0 = -1;
    a->unk7D0 = 0;
}

// Message handler.
int GhostActionMsg(int msg, void* data) {
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
            int r = Fn800F7DEC(q1, q2, 0x0020EE36, fn_800498D0(a), 0);

            if (r != 0x04A3E183) {
                PlayerSetHeldSlotMode(a, 3);
            } else {
                PlayerSetActionMode(a, a->unk7A8);
            }
        } else if (a->state == 8) {
            int r = Fn800F7DEC(q1, q2, 0xF829DB93, fn_800498D0(a), 0);

            if (r != 0x04A3E183) {
                PlayerSetHeldSlotMode(a, 3);
            } else {
                PlayerSetActionMode(a, a->unk7A8);
            }
        } else if (a->state == 0xD) {
            a->flags |= 0x20000;
        } else {
            PlayerSetActionMode(a, a->unk7A8);
        }
        break;
    case 0x0020EE36:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        break;
    case 0xF829DB93:
        fn_80048BA8(a, a->buf, 0);
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        break;
    case 0x9791AF51:
    case 0x96224FCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        break;
    case 0xE6B71BCC:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        break;
    case 0x968CBE3D:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(q1, q2, msg, fn_800498D0(a), a->buf, a->unk14FC);
        break;
    }
    return 0;
}

// Free the roll effect and the action's texture.
void FreeGhostAction(Actor* a) {
    short tex = AssetFind(0xF2E2BEB8, 0x556EAEF5);

    fn_8000F550(&tex, 1);
    a->unk7FC = 0.0f;
    if (sRollFx != -1) {
        fn_8016EEF4(sRollFx);
        sRollFx = -1;
    }
}

// Load the action's texture and start the roll effect that matches the prop type.
void InitGhostAction(Actor* a) {
    short tex = AssetFind(0xF2E2BEB8, 0x556EAEF5);

    sTextureId = tex;
    TextureLoad(&tex, 1);
    a->unk7FC = 0.0f;

    u32 prop = *a->unk26C8;

    if (prop == 0x6EF9D8F1 || prop == 0x7AABF5C8) {
        sRollFx = FxStart(FxFind("can_rolling"), 0, 0);
        fn_8016F018(sRollFx, 0);
    } else if (prop == 0xBF39C942) {
        sRollFx = FxStart(FxFind("mopbucket_roll_l"), 0, 0);
        fn_8016F018(sRollFx, 0);
    } else if (prop == 0xDE71952A) {
        sRollFx = FxStart(FxFind("fuse_roll_l"), 0, 0);
        fn_8016F018(sRollFx, 0);
    }
}

// Prop-type table callbacks: read the entry's value or forward to one of its functions.
int GhostActionValue(Actor* a) {
    return lbl_803BC148[a->state].unk44;
}

void GhostActionFn2(Actor* a) {
    if (sRollFx != -1) {
        fn_8016F018(sRollFx, 0);
    }
    if (lbl_803BC148[a->state].fn2) {
        lbl_803BC148[a->state].fn2(a);
    }
}

void GhostActionFn1(Actor* a, int arg) {
    if (lbl_803BC148[a->state].fn1) {
        lbl_803BC148[a->state].fn1(a, arg);
    }
}

void GhostActionFn0(Actor* a) {
    GhostActionType* t = &lbl_803BC148[a->state];

    if (t->fn0) {
        t->fn0(a);
    }
}

// Load the sixteen prop assets and the squeak effect.
void InitGhostActionAssets(void) {
    lbl_8049E260[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_8049E260[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_8049E260[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_8049E260[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_8049E260[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_8049E260[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_8049E260[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_8049E260[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_8049E260[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_8049E260[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_8049E260[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_8049E260[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_8049E260[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_8049E260[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_8049E260[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_8049E260[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
    sSqueekFx = (int)FxFind("ToySqueek");
}

// True while a ghost is running this action.
int GhostActionIsActive(void) {
    return lbl_804D345C > -1;
}

// Activate or deactivate the action, freeing the roll object's buffers on activation.
// Frees the roll's recorded buffers.
inline void RollFreeAll(RollData* p) {
    for (int i = 0; i < p->unk50; i++) {
        VFree(p->unk00[i]);
    }
}

void GhostActionSetActive(int arg) {
    if (lbl_804D345C == -1) {
        return;
    }
    lbl_804D53D4 = 1;

    if (arg != 0) {
        RollData* p = lbl_8049EE58[lbl_804D345C];

        if (p->unkB4 != 0) {
            RollFreeAll(p);
            p->unk50 = 0;
            p->unk80 = 0;
            p->unk88 = 0;
            p->unk8C = 0;
            p->unk90 = 0;
            p->unk94 = 0;
            p->unk98 = 0;
            p->unkA4 = 0;
            p->unkA8 = 0;
            p->unk84 = 0;
        }
    }

    RollData* p = lbl_8049EE58[lbl_804D345C];
    p->unk9C = arg ? fn_8021DC40 : fn_8021DCF4;
    p->unkA0 = 0;
    if (p->unkA4 != 1) {
        p->unkAC = 500;
    }
    p->unkA4 = 3;
}

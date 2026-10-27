// u_802B4600.cpp: crab boss behaviour (0x802B4600-0x802B7120).
// A boss with a five-state machine (idle, charge, strafe, hurt ...) that kicks up sparks and
// debris particles from its legs and hurls rocks.  The state block (VAlloc'd, 0x1AD0 bytes)
// holds the current/next state, two particle pools (0x20 sparks, 0x40 debris with hazard
// handles) and the strafe/lean values.  The message handler answers the state-name query
// and registers the boss with the defer list.  Names are descriptive guesses; unmatched
// against retail so far.

typedef unsigned long u32;
typedef unsigned char u8;

extern "C" {
void* memset(void* dst, int c, unsigned long n);
int rand(void);
}

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

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[12];
};

// One particle of the spark/debris pools (0x44 bytes).
struct Particle {
    float life;             // 0x00
    float size;             // 0x04
    float x;                // 0x08
    float y;                // 0x0C
    float z;                // 0x10
    char pad14[0x44 - 0x14];
};

// Boss state block (VAlloc'd, 0x1AD0 bytes).
struct CrabBeh {
    int owner;              // 0x00
    int anim;               // 0x04
    int flag;               // 0x08
    int state;              // 0x0C
    int next;               // 0x10
    float f14;              // 0x14
    float f18;              // 0x18
    u32 flags;              // 0x1C
    int fx;                 // 0x20
    Particle sparks[0x20];  // 0x24
    int sparkIdx;           // 0x8A4
    float sparkTimer;       // 0x8A8
    Particle debris[0x40];  // 0x8AC
    int hazard[0x40];       // 0x19AC
    int debrisIdx;          // 0x1AAC
    float debrisTimer;      // 0x1AB0
    float hurtTimer;        // 0x1AB4
    float f1AB8;            // 0x1AB8
    float strafeGoal;       // 0x1ABC
    float strafe;           // 0x1AC0
    float lean;             // 0x1AC4
    float rage;             // 0x1AC8
    float hitDelay;         // 0x1ACC
};

struct Node {
    char pad00[0x10];
    float x;                // 0x10
    float y;                // 0x14
    float z;                // 0x18
};

struct ActorType {
    char pad00[0x30];
    int f30;                // 0x30
    char pad34[0x8C];
    float f_C0;             // 0xC0
};

struct Actor {
    char pad00[0x24];
    int x;                          // 0x24
    int y;                          // 0x28
    int z;                          // 0x2C
    char pad30[8];
    int id;                         // 0x38
    char pad3C[4];
    void* ptr40;                    // 0x40
    char pad44[0x84 - 0x44];
    float fx;                       // 0x84
    float fy;                       // 0x88
    float fz;                       // 0x8C
    char pad90[0xC4 - 0x90];
    ActorType* type;                // 0xC4
    char padC8[4];
    int model;                      // 0xCC
    int unkD0;                      // 0xD0
    int anim;                       // 0xD4
    char padD8[0xF0 - 0xD8];
    Node* node;                     // 0xF0
    char padF4[0x110 - 0xF4];
    void* f110;                     // 0x110
    char pad114[4];
    void* f118;                     // 0x118
    char pad11C[0x134 - 0x11C];
    CrabBeh* beh;                   // 0x134
    float dt;                       // 0x138
    char pad13C[0x240 - 0x13C];
    float vx;                       // 0x240
    float vy;                       // 0x244
    float vz;                       // 0x248
    float lean;                     // 0x24C
    char pad250[4];
    float spin;                     // 0x254
    char pad258[0x26C - 0x258];
    int i26C;                       // 0x26C
    char st[0x284 - 0x270];         // 0x270
    float steer;                    // 0x284
    int i288;                       // 0x288
    float f28C;                     // 0x28C
    char pad290[0x5F4 - 0x290];
    int i5F4;                       // 0x5F4
    char pad5F8[0x658 - 0x5F8];
    char zones[0x894 - 0x658];      // 0x658
    char animState[0x900 - 0x894];  // 0x894
    float f900;                     // 0x900
    float f904;                     // 0x904
};

// One entry of the state table (0x18 bytes).
typedef int (*StateFn)(Actor* a);
typedef int (*StateLeaveFn)(Actor* a, int next);
struct StateRec {
    u32 key;                // 0x00 animation key
    StateFn enter;          // 0x04
    StateLeaveFn leave;     // 0x08
    StateFn update;         // 0x0C
    StateLeaveFn onAnim;    // 0x10
    const char* name;       // 0x14
};

struct Msg {
    Actor* actor;           // 0x00
    u32 key;                // 0x04
    int unk08;              // 0x08
    int arg;                // 0x0C
};

struct Msg41E {
    Actor* actor;           // 0x00
    u32 key;                // 0x04
    int unk08;              // 0x08
    Buffer result;          // 0x0C
};

// Hit message (0x407).
struct HitMsg {
    int unk00;              // 0x00
    int from;               // 0x08 (aliased below)
};

struct HitInfo {
    int unk00[2];
    int source;             // 0x08
    float force;            // 0x0C
    char pad10[0x14 - 0x10];
    u32 key;                // 0x14
    int pos[3];             // 0x18
    int dir[3];             // 0x24
    int back[3];            // 0x30
    char pad3C[0x5C - 0x3C];
};

extern "C" {
int SendMsg(int to, int msg, void* data);
void fn_800CDD10(void);
void fn_8000F550(short* id, int n);
void fn_8000F738(short* id, int n);
void fn_80162610(Actor* a, void* anim, u32 key);
String fn_800225C8(const String& a, const String& b);
void fn_8028C188(Buffer* b, char* data, int len);
void fn_8016EEF4(int handle);
void fn_80211470(float dt, void* arr, int stride, int n);
void fn_800A8500(Actor* a);
void fn_8009F44C(int* v, Actor* a, int* out);
void fn_8016E13C(void* st, int* v, int a);
void fn_801D55CC(void);
void fn_80210D80(void* arr, int stride, int n, int tex, int a, void* tabA, void* tabB);
void fn_800A0EF0(Mtx34* out, Node* n);
void fn_800A025C(float* out, Mtx34* m, int axis);
void fn_8008F4F0(float f, int anim, int a);
void fn_80037EC0(int model, int a, int b, int c, int d, int e, int f);
void fn_8008E824(float f, int anim, int a);
void fn_800E1098(float f, void* in, void* out);
float fn_800E1020(void* v);
void fn_8016D4EC(float* a, float* b);
void fn_8016EDD8(int handle, int* pos);
void fn_800A0094(float* in, float* out);
void fn_80211638(float a, float b, float c, float d, Particle* p, float* pos, float* vel, int e, int f);
int fn_80121BB4(float a, float b, float c, float d, Actor* o, int id, float* pos, u32 key);
void fn_800C5E00(int a, int b, int c, int d, int e);
void fn_800E1C04(int a, int b, int c, int d, int e, int f, int g);
int fn_800BAB34(Actor* a, float* out);
float fn_800AA9FC(Actor* a, float* pos);
void fn_800AACF4(Actor* a, float* pos, float* x, float* y);
float fn_800CC2D0(Actor* a, int b, int c, int d);
int fn_8016F120(int fx, int* pos);
void fn_8016F390(int fx, int a, int b);
void fn_8016D864(void* out, void* in);
int fn_80018674(void* a, void* b, void* c, void* d);
void fn_800E0CB8(int* in, int* out);
void fn_80090390(int anim, int a);
void fn_800C5654(int id, u32 key, int arg);
void fn_8008F4E8(int anim, int a);
void fn_8008F378(int anim);
void fn_8008F4EC(int anim, int a);
void fn_8008F230(float f, int anim, int a, int b);
void fn_80090974(float f, int anim, int a, int b, int c, void* fn, void* arg);
int fn_800A1BC8(int a, int b, u32 key, int c, int d);
int fn_8008E7D0(int a);
void fn_800D17D0(void* zones, int a, int b);
void fn_800D1534(void* zones, void* fn, Actor* a);
int fn_8008EA9C(int anim, int a);
int fn_8008E964(int anim);
void fn_800913EC(float dt, int anim, float* out);
void fn_8008F630(int anim, Node* n, int a, int b);
void fn_800A0168(Mtx34* in, Mtx34* out);
void fn_800A0030(float* v, Mtx34* m, float* out);
void fn_8016D55C(float* a, float* b);
void fn_8016D67C(float x, float y, float z, float* out);
void fn_8016D440(float* out, void* st);
void fn_8016D614(float* a, float* b, float* c);
void fn_8009FAC0(Mtx34* m, float* e);
void fn_8009EF04(Actor* a, Mtx34* m);
void fn_8003A66C(int model, Mtx34* m, Node* n);
void fn_8009FB94(float* out, Mtx34* m);
void fn_80022E8C(Actor* a);
int fn_8016325C(void* anim);
}

int ConditionSetBit(void* cond, int bit, int set);
int ConditionUpdateBit(void* cond, int bit, int set);
int HasCondition(void* cond, int bit);
void* VAlloc(int size);
void VFree(void* p);
u32 AssetFind(u32 table, u32 key);
void ActorGetMatrix(Mtx34* out, Actor* a);
int ModelAnimate(int model, char* name, int a);
void HazardDestroy(int handle);
void DeferAdd(int a, Vec3* pos, void (*fn)(void*), void* arg);
int FxFind(const char* name);
int Rand();
int ActorCount();
Actor* ActorGet(int i);
Actor* GetPlayer();

// Data owned by other files.
extern StateRec lbl_803C43A8[5];
extern char lbl_803C4438[];
extern char lbl_803C444C[];
extern char lbl_803C4484[];
extern char lbl_803C4498[];
extern int lbl_804D3910;
extern int lbl_804D3914;
extern int lbl_804D3918;
extern int lbl_804D391C;
extern int lbl_804D395C;

int CrabMsg(int msg, int* data);
int CrabUpdate(Actor* a);
void CrabFree(Actor* a);
void CrabInit(int owner, Actor* a);
void CrabRender(Actor* a);
int CrabHitCb(u32 unused, Actor* other, Actor* self);
void CrabSetAnim(Actor* a, int blend);
void CrabAnimate(Actor* a);

// A pseudo-random offset in [-range/2, range/2).
static inline float Scatter(float range) {
    int r = (int)__fabsf(range);

    if (r != 0) {
        return (float)(Rand() % r - r / 2);
    }
    return 0.0f;
}

static inline float ClampF(float v, float lo, float hi) {
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

// Drop the debris effects and the voice effect (message 0xC00 / 0x3000).
static inline void CrabClearEffects(CrabBeh* b) {
    int i;

    for (i = 0; i < 0x40; i++) {
        b->debris[i].size = 0.0f;
        b->debris[i].x = 0.0f;
        if (b->hazard[i] != -1) {
            HazardDestroy(b->hazard[i]);
            b->hazard[i] = -1;
        }
    }
    if (b->fx != -1) {
        fn_8016EEF4(b->fx);
        b->fx = -1;
    }
}

static inline void CrabStateName(Buffer* out, const char* s) {
    String name(s);
    String newline("\n");
    const String& text = fn_800225C8(name, newline);

    fn_8028C188(out, text.mData, text.mLength);
}

// 0x802B4600: message handler.
int CrabMsg(int msg, int* data) {
    Actor* a = (Actor*)data[0];

    switch (msg) {
    case 0x41E:
        if (data[1] == 0xAD8576FA) {
            Msg41E* m = (Msg41E*)data;

            CrabStateName(&m->result, lbl_803C43A8[a->beh->state].name);
            return 0;
        }
        return 0;
    case 6: {
        short ids[2];

        ids[0] = (short)lbl_804D3918;
        ids[1] = (short)lbl_804D391C;
        if (data[1] == 0) {
            fn_8000F550(ids, 2);
        } else {
            fn_8000F738(ids, 2);
        }
        return 0;
    }
    case 0:
        DeferAdd(1, (Vec3*)&a->fx, (void (*)(void*))CrabRender, 0);
        return 1;
    case 0x41D: {
        CrabBeh* b = a->beh;

        if (data[1] == 0x20A5E84A) {
            if (data[3] == 0) {
                b->flags &= ~1;
            } else {
                b->flags |= 1;
            }
            return 1;
        }
        return 0;
    }
    case 0xC14: {
        CrabBeh* b = a->beh;

        if (data[3] == 0x90F4B327) {
            if ((float)rand() / 32767.0f < 0.5f) {
                b->hurtTimer = 1500.0f;
            }
            return 1;
        }
        return 0;
    }
    case 0xC00: {
        CrabBeh* b = a->beh;

        if (data[1] == 0) {
            CrabClearEffects(b);
        }
        return 0;
    }
    case 0x3000:
        CrabClearEffects(a->beh);
        return 0;
    }
    return 0;
}

// 0x802B4950: per-frame update.
int CrabUpdate(Actor* a) {
    CrabBeh* b = a->beh;
    int next = -1;
    int i;

    if (HasCondition(a->st, 4) == 0 || HasCondition(a->st, 9) != 0 || HasCondition(a->st, 10) != 0) {
        if (HasCondition(a->st, 0xD) != 0) {
            CrabAnimate(a);
            next = b->state;
        }
    } else {
        ConditionSetBit(a->st, 4, 0);
    }
    if (next < 0 || next == b->state) {
        next = lbl_803C43A8[b->state].update(a);
    }
    if (next >= 0 && next != b->state && lbl_803C43A8[b->state].leave(a, next) != 0) {
        lbl_803C43A8[next].enter(a);
    }
    fn_80211470(a->dt, b->sparks, 0x44, 0x20);
    fn_800A8500(a);
    for (i = 0; i < 0x20; i++) {
        if (b->sparks[i].life > 0.0f && b->sparks[i].size > 0.0f) {
            int v[3];

            v[0] = (int)b->sparks[i].x - a->x;
            v[1] = (int)b->sparks[i].y - a->y;
            v[2] = (int)b->sparks[i].z - a->z;
            fn_8009F44C(v, a, v);
            fn_8016E13C(a->st + 0x20, v, 1);
        }
    }
    fn_80211470(a->dt, b->debris, 0x44, 0x40);
    for (i = 0; i < 0x40; i++) {
        if (b->debris[i].life > 0.0f && b->debris[i].size > 0.0f) {
            int v[3];

            v[0] = (int)b->debris[i].x - a->x;
            v[1] = (int)b->debris[i].y - a->y;
            v[2] = (int)b->debris[i].z - a->z;
            fn_8009F44C(v, a, v);
            fn_8016E13C(a->st + 0x20, v, 1);
        }
        if (b->hazard[i] != -1 && b->debris[i].size <= 0.0f) {
            HazardDestroy(b->hazard[i]);
            b->hazard[i] = -1;
        }
    }
    return 1;
}

// 0x802B4C80: destroy callback.
void CrabFree(Actor* a) {
    VFree(a->beh);
    a->f110 = 0;
    a->beh = 0;
}

// 0x802B4CBC: init callback.
void CrabInit(int owner, Actor* a) {
    CrabBeh* b;

    if (a != 0) {
        b = (CrabBeh*)VAlloc(0x1AD0);
        if (b == 0) {
            a->beh = 0;
        } else {
            a->beh = b;
            a->f110 = (void*)CrabUpdate;
            a->f118 = (void*)fn_800CDD10;
            b->owner = owner;
            b->f14 = 0.0f;
            b->f18 = 0.0f;
            b->flags = 0;
            b->anim = -1;
            b->strafe = 0.0f;
            b->strafeGoal = 0.0f;
            b->fx = -1;
            b->state = 0;
            b->next = 0;
            b->flag = -1;
            b->hitDelay = -1.0f;
            if (a->i5F4 != 0) {
                fn_801D55CC();
                a->i5F4 = 0;
            }
            ConditionSetBit(a->st, 0x2D, 1);
            ConditionSetBit(a->st, 0x1B, 1);
            fn_800D17D0(a->zones, 1, 0);
            lbl_804D3910 = FxFind("crab_roll2");
            lbl_804D3914 = FxFind("SCI_BOSSHIT");
            lbl_804D3918 = AssetFind(0xF2E2BEB8, 0x2E20AEE7);
            lbl_804D391C = AssetFind(0xF2E2BEB8, 0x6E2251B7);
            b->sparkIdx = -1;
            b->sparkTimer = 0.0f;
            memset(b->sparks, 0, 0x880);
            b->debrisIdx = -1;
            b->debrisTimer = 150.0f;
            memset(b->debris, 0, 0x1100);
            memset(b->hazard, 0xFF, 0x100);
        }
    }
}

// 0x802B4E64: draw callback for the particle pools.
void CrabRender(Actor* a) {
    CrabBeh* b = a->beh;

    fn_80210D80(b->sparks, 0x44, 0x20, lbl_804D3918, 0, lbl_803C4438, lbl_803C444C);
    fn_80210D80(b->debris, 0x44, 0x40, lbl_804D391C, 0, lbl_803C4484, lbl_803C4498);
}

// 0x802B4EDC: animation-event callback for the charge states.
int CrabAnimEventA(int key, Actor* a) {
    CrabBeh* b = a->beh;

    if (key == 0x1B83833F) {
        if (b->state == 5) {
            ConditionSetBit(a->st, 0x30, 0);
        }
        lbl_803C43A8[b->state].leave(a, b->next);
        lbl_803C43A8[b->next].enter(a);
        fn_80162610(a, a->animState, 0x1B83833F);
    }
    return 1;
}

// 0x802B4FAC: animation-event callback for the other states.
int CrabAnimEventB(int key, Actor* a) {
    CrabBeh* b = a->beh;

    if (key == 0x1B83833F) {
        fn_80162610(a, a->animState, 0);
        lbl_803C43A8[b->state].leave(a, b->next);
        lbl_803C43A8[b->next].enter(a);
    }
    return 1;
}

// 0x802B5050: leave a state, keep running.
int CrabLeaveA(Actor* a, int next) {
    a->beh->next = next;
    return 0;
}

// 0x802B5060: leave a state, allow the switch.
int CrabLeaveB(Actor* a, int next) {
    a->beh->next = next;
    return 1;
}

// Ease the strafe amount toward its goal and blend the two strafe animations.
static inline void CrabStrafeBlend(Actor* a) {
    CrabBeh* b = a->beh;

    if (b->state == 2) {
        float cur = b->strafe;

        if (b->strafeGoal <= cur) {
            if (b->strafeGoal < cur) {
                b->strafe = -(100.0f * 0.0010000000474974513f * a->dt - cur);
                b->strafe = (b->strafe < b->strafeGoal) ? b->strafeGoal : b->strafe;
            }
        } else {
            b->strafe = 100.0f * 0.0010000000474974513f * a->dt + cur;
            b->strafe = (b->strafeGoal < b->strafe) ? b->strafeGoal : b->strafe;
        }
        {
            float k = ClampF(b->strafe / 40.0f + b->lean / 20.0f, -1.0f, 1.0f);

            fn_8008F4F0(ClampF(k, 0.0f, 1.0f), a->anim, 1);
            fn_8008F4F0(ClampF(-k, 0.0f, 1.0f), a->anim, 2);
        }
    }
}

// 0x802B5070: update of the strafing state (movement, sparks, debris, rocks).
int CrabUpdateStrafe(Actor* a) {
    CrabBeh* b = a->beh;
    Mtx34 mtx;
    float axisA[3];
    float axisB[3];
    float axisC[3];
    float dirA[3];
    float dirB[3];
    float delta[3];
    int i;

    fn_800A0EF0(&mtx, a->node);
    fn_800A025C(axisA, &mtx, 1);
    CrabAnimate(a);
    CrabStrafeBlend(a);
    fn_800A0EF0(&mtx, a->node);
    fn_800A025C(axisB, &mtx, 1);
    if (b->rage < 1.0f) {
        if ((b->flags & 1) == 0) {
            b->rage += a->dt / 3500.0f;
        }
        if (1.0f <= b->rage) {
            b->rage = 1.0f;
            ConditionUpdateBit(a->st, 10, 0);
        }
        fn_80037EC0(a->model, lbl_804D395C, 1, 0xFF, 0xFF, (int)(255.0f * b->rage), 0xFF);
    }
    if (0.0f < a->f28C) {
        float k = 1.2000000476837158f * (a->f28C / 7539.82275390625f);

        if (b->rage < 1.0f) {
            k = k * (-0.7999999523162842f * b->rage + 2.0f);
        }
        fn_8008E824(k, a->anim, 0);
        fn_8008E824(k, a->anim, 1);
        fn_8008E824(k, a->anim, 2);
    }
    fn_800E1098(1200.0f, dirA, axisA);
    fn_800E1098(1200.0f, dirB, axisB);
    fn_800A025C(delta, &mtx, 0);
    delta[0] = dirB[0] - dirA[0];
    delta[1] = dirB[1] - dirA[1];
    delta[2] = dirB[2] - dirA[2];
    {
        float len = fn_800E1020(delta);
        float speed = len / (0.0010000000474974513f * a->dt) / 1.2000000476837158f;

        if (b->rage < 1.0f) {
            speed *= 1.2000000476837158f * b->rage;
        }
        fn_8016D4EC(&a->vx, &a->lean);
        if (b->strafe != 0.0f) {
            float side[3];

            fn_800A025C(side, &mtx, 0);
            fn_800E1098(0.0010000000474974513f * -b->strafe * a->dt, side, side);
            a->vx += side[0];
            a->vy += side[1];
            a->vz += side[2];
        }
        fn_800E1098(speed, &a->vx, &a->vx);
    }
    if (b->fx != -1) {
        fn_8016EDD8(b->fx, &a->x);
    }
    if (0.0f < b->hitDelay) {
        b->hitDelay -= a->dt;
    }
    if (b->hitDelay <= 0.0f) {
        fn_800D1534(a->zones, (void*)CrabHitCb, a);
    }
    if (b->state == 2 && HasCondition(a->st, 0x24) == 0 && HasCondition(a->st, 0x25) == 0) {
        b->next = 3;
    }
    if (b->state == 2) {
        b->sparkTimer -= a->dt;
        if (b->sparkTimer <= 0.0f) {
            float pos[3];
            float vel[3];
            float seed;

            b->sparkIdx++;
            if (b->sparkIdx > 0x1F) {
                b->sparkIdx = 0;
            }
            seed = 4095.0f * ((float)rand() / 32767.0f);
            pos[0] = Scatter(50.0f);
            pos[1] = Scatter(50.0f);
            pos[2] = 0.0f + Scatter(0.0f);
            fn_800A0094(pos, pos);
            pos[0] += a->fx;
            pos[1] += a->fy;
            pos[2] += a->fz;
            vel[0] = Scatter(100.0f);
            vel[1] = Scatter(100.0f);
            vel[2] = -50.0f + Scatter(100.0f);
            fn_800A0094(vel, vel);
            vel[0] *= 0.0010000000474974513f;
            vel[1] *= 0.0010000000474974513f;
            vel[2] *= 0.0010000000474974513f;
            fn_80211638(0.0f, 6000.0f, seed, 0.0f, &b->sparks[b->sparkIdx], pos, vel, 0, 0);
            b->sparkTimer = 400.0f;
        }
        if ((a->f904 < 0.5f * (float)a->type->f30 && a->i288 == 3) || 0.0f < b->hurtTimer) {
            if (0.0f < b->hurtTimer) {
                b->hurtTimer -= a->dt;
            }
            b->debrisTimer -= a->dt;
            if (b->debrisTimer <= 0.0f) {
                float pos[3];
                float vel[3];
                float seed;

                b->debrisIdx++;
                if (b->debrisIdx > 0x3F) {
                    b->debrisIdx = 0;
                }
                seed = 4095.0f * ((float)rand() / 32767.0f);
                pos[0] = Scatter(50.0f);
                pos[1] = Scatter(50.0f);
                pos[2] = 0.0f + Scatter(0.0f);
                fn_800A0094(pos, pos);
                pos[0] += a->fx;
                pos[1] += a->fy;
                pos[2] += a->fz + -600.0f;
                vel[0] = Scatter(100.0f);
                vel[1] = Scatter(100.0f);
                vel[2] = -50.0f + Scatter(100.0f);
                fn_800A0094(vel, vel);
                vel[0] *= 0.0010000000474974513f;
                vel[1] *= 0.0010000000474974513f;
                vel[2] *= 0.0010000000474974513f;
                fn_80211638(0.0f, 6000.0f, seed, 0.0f, &b->debris[b->debrisIdx], pos, vel, 0, 0);
                b->hazard[b->debrisIdx] = fn_80121BB4(1500.0f, 1500.0f, 150.0f, 150.0f, a, a->id, pos, 0x20EE36);
                b->debrisTimer = 150.0f;
            }
        }
        for (i = 0; i < ActorCount(); i++) {
            Actor* p = ActorGet(i);
            float to[3];
            float d;
            float k;

            to[0] = *(float*)((char*)p + 0x67C) - a->fx;
            to[1] = *(float*)((char*)p + 0x680) - a->fy;
            to[2] = *(float*)((char*)p + 0x684) - a->fz;
            d = fn_800E1020(to);
            k = ClampF(1.0f - (d - 2000.0f) / 5000.0f, 0.0f, 1.0f);
            if (0.0f < k) {
                int strength = (int)(190.0f * k + 10.0f);

                if (a->i288 != 2) {
                    if (a->i288 < 2) {
                        if (0 < a->i288) {
                            strength = (int)((float)strength * 0.8999999761581421f);
                        }
                    } else if (a->i288 < 4) {
                        strength = (int)((float)strength * 1.5f);
                    }
                }
                fn_800C5E00(*(int*)((char*)p + 0x248), 1, 100, strength, 0);
                fn_800E1C04(*(int*)((char*)p + 0x58C), 3, 300, (int)(4.0f * k + 1.0f), (int)(4.0f * k + 1.0f),
                            (int)(-70.0f * k + 100.0f), (int)(-70.0f * k + 100.0f));
            }
        }
        GetPlayer();
        {
            float player[3];
            float side = 0.0f;
            float dist = 0.0f;
            Actor* pl;

            if (fn_800BAB34(a, player) != 0) {
                side = fn_800AA9FC(a, player);
                player[0] -= a->fx;
                player[1] -= a->fy;
                player[2] = 0.0f;
                dist = fn_800E1020(player);
            }
            pl = GetPlayer();
            if (pl == 0) {
                b->strafeGoal = 0.0f;
            } else {
                float pp[3];
                float off;
                float dummy[2];

                pp[0] = (float)pl->x;
                pp[1] = (float)pl->y;
                pp[2] = (float)pl->z;
                fn_800AACF4(a, pp, &off, dummy);
                if (60.0f <= __fabsf(off)) {
                    b->strafeGoal = 0.0f;
                } else {
                    float sa = (off < 0.0f) ? -1.0f : 1.0f;
                    float sb = (side < 0.0f) ? -1.0f : 1.0f;
                    float m = 0.0f;

                    if (sa == sb) {
                        m = ClampF(1.0f - dist / 500.0f, 0.0f, 1.0f);
                    }
                    b->strafeGoal = m * 40.0f * -((off < 0.0f) ? -1.0f : 1.0f);
                }
            }
        }
    }
    return b->state;
}

// 0x802B60EC: update that only advances the animation.
int CrabUpdateHold(Actor* a) {
    CrabBeh* b;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    CrabAnimate(a);
    return b->state;
}

// 0x802B612C: update of the idle state.
int CrabUpdateIdle(Actor* a) {
    CrabBeh* b = a->beh;

    CrabAnimate(a);
    if (fn_8016325C(a->animState) == 4 || fn_8016325C(a->animState) == 5) {
        return 4;
    }
    if (HasCondition(a->st, 0x30) != 0) {
        return 5;
    }
    if (HasCondition(a->st, 0x24) == 0 && HasCondition(a->st, 0x25) == 0) {
        return b->state;
    }
    return 1;
}

// 0x802B61DC: enter state 5.
int CrabEnter5(Actor* a) {
    CrabBeh* b = a->beh;

    b->state = 5;
    b->next = 0;
    b->flag = 0;
    ConditionSetBit(a->st, 0xE, 1);
    CrabSetAnim(a, 0x18);
    return 1;
}

// 0x802B623C: enter state 4.
int CrabEnter4(Actor* a) {
    CrabBeh* b = a->beh;

    b->state = 4;
    b->next = 0;
    b->flag = 0;
    ConditionSetBit(a->st, 0xE, 1);
    CrabSetAnim(a, 0x18);
    return 1;
}

// 0x802B629C: enter state 3.
int CrabEnter3(Actor* a) {
    CrabBeh* b = a->beh;

    b->state = 3;
    b->next = 0;
    ConditionSetBit(a->st, 0xE, 1);
    CrabSetAnim(a, 0x18);
    return 1;
}

// 0x802B62F8: enter state 2 (strafing).
int CrabEnter2(Actor* a) {
    CrabBeh* b = a->beh;

    if (b->state != 2) {
        b->strafeGoal = 0.0f;
        b->strafe = 0.0f;
        b->f1AB8 = 2000.0f * ((float)rand() / 32767.0f) + 1000.0f;
        b->rage = 0.0f;
        ConditionUpdateBit(a->st, 10, 1);
    }
    b->state = 2;
    b->next = 2;
    ConditionSetBit(a->st, 0xE, 1);
    CrabSetAnim(a, 0);
    return 1;
}

// 0x802B63C0: enter state 1.
int CrabEnter1(Actor* a) {
    CrabBeh* b = a->beh;

    b->state = 1;
    b->next = 2;
    a->i288 = 3;
    a->i26C = 3;
    if (HasCondition(a->st, 0x24) == 0) {
        b->flag = 1;
    } else {
        b->flag = 0;
    }
    ConditionSetBit(a->st, 0xE, 1);
    CrabSetAnim(a, 0x18);
    if (b->fx == -1) {
        b->fx = fn_8016F120(lbl_804D3910, &a->x);
    }
    return 1;
}

// 0x802B6474: enter state 0.
int CrabEnter0(Actor* a) {
    CrabBeh* b = a->beh;

    b->next = 0;
    b->state = 0;
    b->flag = 0;
    a->i288 = 0;
    a->i26C = 0;
    CrabSetAnim(a, 8);
    if (b->fx != -1) {
        fn_8016EEF4(b->fx);
        b->fx = -1;
    }
    return 1;
}

// 0x802B64D8: advance the animation and apply root motion.
void CrabAnimate(Actor* a) {
    CrabBeh* b = a->beh;
    float root[3];
    Mtx34 mtx;
    Mtx34 rot;
    float euler[3];
    float lean[3];

    if (HasCondition(a->st, 0x1C) != 0 || fn_8008EA9C(a->anim, 0) == 0 || fn_8008EA9C(a->anim, 0) == 2) {
        if (HasCondition(a->st, 0xD) == 0) {
            lbl_803C43A8[b->state].enter(a);
        }
        if (HasCondition(a->st, 0x1C) != 0 && HasCondition(a->st, 0xD) != 0 && fn_8008E964(a->anim) != 0) {
            fn_800C5654(a->id, 0xB2CB0AA4, a->id);
            ConditionSetBit(a->st, 0x1C, 0);
            ConditionSetBit(a->st, 0x1D, 1);
        }
        if (fn_8008E964(a->anim) == 0) {
            return;
        }
    }
    fn_800913EC(a->dt, a->anim, root);
    fn_8008F630(a->anim, a->node, ModelAnimate(a->model, (char*)a->node, 1), a->unkD0);
    if (HasCondition(a->st, 0xC) == 0) {
        if (HasCondition(a->st, 0xD) == 0) {
            float h = fn_800CC2D0(a, 0, 0, 0);

            if (h != -999999.0f) {
                a->fz = h + a->node->z;
            }
            a->z = (int)a->fz;
            a->node->x = 0.0f;
            a->node->y = 0.0f;
            a->node->z = 0.0f;
            {
                float rate = 3.0f * a->type->f_C0;

                if (b->state == 2) {
                    float steer = a->steer;

                    if (steer > 1.1920928955078125e-07f) {
                        float d = 0.0010000000474974513f * rate * a->dt;

                        a->spin += (d < steer) ? d : steer;
                    } else if (steer < -1.1920928955078125e-07f) {
                        float d = -(0.0010000000474974513f * rate * a->dt);

                        a->spin += (steer < d) ? d : steer;
                    }
                }
            }
            if (HasCondition(a->st, 0) != 0) {
                ActorGetMatrix(&mtx, a);
                fn_800A0168(&mtx, &mtx);
                fn_800A0030(root, &mtx, root);
                if (a->dt <= 0.0f) {
                    a->vx = 0.0f;
                    a->vy = 0.0f;
                    a->vz = 0.0f;
                } else {
                    float inv;

                    a->vx = root[0];
                    a->vy = root[1];
                    a->vz = root[2];
                    a->vz = 0.0f;
                    inv = 1000.0f / a->dt;
                    a->vx *= inv;
                    a->vy *= inv;
                    a->vz *= inv;
                }
            }
            fn_8016D55C(&a->lean, &a->lean);
            fn_8016D67C(a->spin, 0.0f, 0.0f, euler);
            fn_8016D55C(euler, euler);
            fn_8016D440(lean, a->st);
            fn_8016D614(lean, lean, &a->lean);
            fn_8016D55C(lean, lean);
            if (b->lean != lean[1]) {
                float rate;
                float d;

                if (__fabsf(b->lean) <= __fabsf(lean[1])) {
                    rate = 80.0f * a->dt;
                } else {
                    rate = 60.0f * a->dt;
                }
                d = __fabsf(lean[1] - b->lean);
                if (0.0010000000474974513f * rate < d) {
                    d = 0.0010000000474974513f * rate;
                }
                b->lean += d * ((lean[1] - b->lean < 0.0f) ? -1.0f : 1.0f);
            }
            fn_8009FAC0(&rot, euler);
            rot.m[9] = (float)a->x;
            rot.m[10] = (float)a->y;
            rot.m[11] = (float)a->z;
            fn_8009EF04(a, &rot);
            fn_8003A66C(a->model, &rot, a->node);
        } else {
            float h = fn_800CC2D0(a, 0, 0, 0);

            if (h != -999999.0f) {
                a->fz = h + a->node->z;
                a->node->z = 0.0f;
            }
            ActorGetMatrix(&mtx, a);
            fn_800A0168(&mtx, &mtx);
            fn_800A0030(root, &mtx, root);
            if (a->dt <= 0.0f) {
                a->vx = 0.0f;
                a->vy = 0.0f;
                a->vz = 0.0f;
            } else {
                float inv;

                a->vx = root[0];
                a->vy = root[1];
                a->vz = root[2];
                a->vz = 0.0f;
                inv = 1000.0f / a->dt;
                a->vx *= inv;
                a->vy *= inv;
                a->vz *= inv;
            }
            fn_8009FAC0(&rot, &a->lean);
            rot.m[9] = (float)a->x;
            rot.m[10] = (float)a->y;
            rot.m[11] = (float)a->z;
            fn_8009EF04(a, &rot);
            a->node->x = 0.0f;
            a->node->y = 0.0f;
            a->node->z = 0.0f;
            fn_8003A66C(a->model, &rot, a->node);
        }
    } else {
        if (fn_8008EA9C(a->anim, 0) != 0 && HasCondition(a->st, 0xC) != 0) {
            fn_800A0EF0(&rot, a->node);
            fn_8009FB94(&a->lean, &rot);
            fn_8009EF04(a, &rot);
            a->fx = a->node->x;
            a->fy = a->node->y;
            a->fz = a->node->z;
            a->x = (int)a->node->x;
            a->y = (int)a->node->y;
            a->z = (int)a->node->z;
        }
        fn_8003A66C(a->model, 0, a->node);
    }
}

// 0x802B6B7C: start the current state's animation with the given blend flags.
void CrabSetAnim(Actor* a, int flags) {
    CrabBeh* b = a->beh;
    float speed;
    int anim;

    if (HasCondition(a->st, 0xC) != 0) {
        ConditionSetBit(a->st, 0xC, 0);
        ConditionSetBit(a->st, 0xE, 1);
        fn_8008F4E8(a->anim, 0);
        fn_8008F378(a->anim);
        fn_8008F4EC(a->anim, 0);
    }
    speed = 1.0f;
    if ((b->state == 2 || b->state == 3) && 0.0f < a->f28C) {
        speed = a->f28C / 7539.82275390625f;
    }
    anim = fn_800A1BC8(*(int*)((char*)a->type + 0x14), *(int*)((char*)a->type + 0x1C), lbl_803C43A8[b->state].key,
                       b->flag, 0);
    if (fn_8008E7D0(anim) != 0) {
        ConditionSetBit(a->st, 0x1C, 0);
        ConditionSetBit(a->st, 0x1D, 1);
    }
    ConditionSetBit(a->st, 0, (flags >> 3) & 1);
    if (HasCondition(a->st, 0xE) == 0) {
        fn_8008F230(150.0f, a->anim, 0, 1);
    } else {
        fn_8008F230(0.0f, a->anim, 0, 0);
        ConditionSetBit(a->st, 0xE, 0);
    }
    b->anim = anim;
    fn_80090974(speed, a->anim, 0, anim, flags, (void*)lbl_803C43A8[b->state].onAnim, a);
    if (b->state == 2) {
        int a1 = fn_800A1BC8(*(int*)((char*)a->type + 0x14), *(int*)((char*)a->type + 0x1C), lbl_803C43A8[2].key, 2, 0);
        int a2;

        fn_80090974(speed, a->anim, 1, a1, flags, 0, 0);
        a2 = fn_800A1BC8(*(int*)((char*)a->type + 0x14), *(int*)((char*)a->type + 0x1C), lbl_803C43A8[b->state].key,
                         3, 0);
        fn_80090974(speed, a->anim, 2, a2, flags, 0, 0);
        CrabStrafeBlend(a);
    } else {
        fn_80090390(a->anim, 1);
        fn_80090390(a->anim, 2);
    }
}

// 0x802B6F38: zone-walk callback; knocks back actors touched by the boss's legs.
int CrabHitCb(u32 unused, Actor* other, Actor* self) {
    u32 kind;

    if (other == 0 || self == 0) {
        return 0;
    }
    kind = (u32)other->id >> 24;
    if (kind == 2 || kind == 1) {
        if (kind == 1 && (*(int**)((char*)other + 0x88) == 0 || **(int**)((char*)other + 0x88) != (int)0xDCC384C1)) {
            return 1;
        } else {
            CrabBeh* b = self->beh;
            char boxA[24];
            char boxB[24];
            Mtx34 mA;
            Mtx34 mB;

            fn_8016D864(boxA, self->ptr40);
            fn_8016D864(boxB, other->ptr40);
            ActorGetMatrix(&mA, self);
            ActorGetMatrix(&mB, other);
            if (fn_80018674(boxA, &mA, boxB, &mB) != 0) {
                HitInfo hit;

                memset(&hit, 0, 0x5C);
                hit.source = self->id;
                hit.key = 0x1BA3EE4A;
                hit.force = (self->i26C == 3) ? 2000.0f : 1400.0f;
                hit.pos[0] = self->x;
                hit.pos[1] = self->y;
                hit.pos[2] = self->z;
                hit.dir[0] = hit.pos[0] - other->x;
                hit.dir[1] = hit.pos[1] - other->y;
                hit.dir[2] = hit.pos[2] - other->z;
                fn_800E0CB8(hit.dir, hit.dir);
                hit.back[0] = -hit.dir[0];
                hit.back[1] = -hit.dir[1];
                hit.back[2] = -hit.dir[2];
                SendMsg(other->id, 0x407, &hit);
                if (((u32)other->id >> 24) == 2) {
                    fn_8016F390(lbl_804D3914, 0, 0);
                    b->hitDelay = 2000.0f;
                }
            }
            return 1;
        }
    }
    return 1;
}

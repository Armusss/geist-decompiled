// u_8025ECF8.cpp: shell crab enemy ("ShellCrab", 0x8025ECF8-0x802665A8).
// An armoured crab with a roll attack, a melee claw, a laser and a roar, driven by a table of
// 0x31 states (enter / leave / update / event per state).  The per-actor block (VAlloc'd,
// 0xBC bytes) holds the current and requested state, two animation handles, a damage flag
// word, a voice-effect handle and the timers the update functions advance.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct Mtx33 {
    float m[3][3];
};

// Orientation and position. The matrix helpers write a full 3x4 over rot, running into pos;
// pos is then filled in by the caller.
struct PlaceF {
    Mtx33 rot;
    Vec3 pos;
};

// Root bone: the translation column of its matrix lives at 0x10.
struct Bone {
    char pad0[0x10];
    float x, y, z;
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
    virtual void Release(char* p);
};

struct Actor;

// Per-actor state block (VAlloc'd, 0xBC bytes).
struct ShellBeh {
    int owner;          // 0x00
    int animA;          // 0x04
    int animB;          // 0x08
    int animC;          // 0x0C
    int animD;          // 0x10
    int animE;          // 0x14
    char pad18[0x28 - 0x18];
    float f28;          // 0x28
    float f2C;          // 0x2C
    float f30;          // 0x30
    char pad34[0x3C - 0x34];
    int i3C;            // 0x3C
    int i40;            // 0x40
    int state;          // 0x44
    int next;           // 0x48
    u32 hash4C;         // 0x4C
    int i50;            // 0x50
    int i54;            // 0x54
    int i58;            // 0x58
    float f5C;          // 0x5C
    float f60;          // 0x60
    u32 flags;          // 0x64: 2 = hurt, 4 = airborne, 8 = turning done
    int fx;             // 0x68
    float f6C;          // 0x6C
    float f70;          // 0x70
    float timer;        // 0x74
    float limit;        // 0x78
    int i7C;            // 0x7C
    char pad80[0x84 - 0x80];
    float f84;          // 0x84
    float f88;          // 0x88
    float f8C;          // 0x8C
    int i90;            // 0x90
    float f94;          // 0x94
    float f98;          // 0x98
    char pad9C[4];
    String* name;       // 0xA0
    float fA4;          // 0xA4
    float posX;         // 0xA8
    float posY;         // 0xAC
    float posZ;         // 0xB0
    float dist;         // 0xB4
    float fB8;          // 0xB8
};

// The shell crab's per-type definition data (at actor+0xC4).
struct ShellData {
    char pad0[0x14];
    int i14;            // 0x14
    char pad18[4];
    int i1C;            // 0x1C
    char pad20[0x10];
    int i30;            // 0x30
    char pad34[0x8C];
    float turnRate;     // 0xC0
};

// Second per-type block (at actor+0xC8).
struct ShellData2 {
    char pad0[0x2C];
    float heading;      // 0x2C
};

struct Actor {
    char pad00[0x24];
    int i24;            // 0x24
    int i28;            // 0x28
    int i2C;            // 0x2C
    char pad30[8];
    int handle;         // 0x38
    char pad3C[4];
    void* ptr40;        // 0x40
    char pad44[0x40];
    float f84;          // 0x84
    float f88;          // 0x88
    float f8C;          // 0x8C
    char pad90[0x34];
    ShellData* data;    // 0xC4
    ShellData2* data2;  // 0xC8
    int model;          // 0xCC
    int boneCount;      // 0xD0
    int anim;           // 0xD4
    char padD8[0x18];
    Bone* bones;        // 0xF0
    char padF4[0x14];
    int unk108;         // 0x108
    char pad10C[4];
    int (*cb110)(int, void*);   // 0x110
    char pad114[4];
    void (*cb118)(void);        // 0x118
    char pad11C[0x18];
    ShellBeh* beh;      // 0x134
    float dt;           // 0x138
    char pad13C[0x104];
    float f240;         // 0x240
    float f244;         // 0x244
    float f248;         // 0x248
    Vec3 euler;         // 0x24C
    char pad258[0x18];
    char conditions[0xC];   // 0x270
    float f27C;         // 0x27C
    char pad280[4];
    float f284;         // 0x284
    char pad288[0x2B0 - 0x288];
    float f2B0;         // 0x2B0
    char pad2B4[0x5F4 - 0x2B4];
    int h5F4;           // 0x5F4
    char pad5F8[0x894 - 0x5F8];
    char animState[0x70];   // 0x894
    float f904;         // 0x904
};

// One entry of the state dispatch table (0x18 bytes).
struct ShellState {
    u32 type;                                   // 0x00
    int (*enter)(Actor* a);                     // 0x04
    int (*setState)(Actor* a, int state);       // 0x08
    int (*update)(Actor* a);                    // 0x0C
    int (*event)(int msg, Actor* a);            // 0x10
    const char* name;                           // 0x14
};

// Hit message (0x407) payload.
struct HitInfo {
    int unk00[2];
    int source;         // 0x08
    float force;        // 0x0C
    char pad10[0x14 - 0x10];
    u32 key;            // 0x14
    int pos[3];         // 0x18
    int dir[3];         // 0x24
    int back[3];        // 0x30
    char pad3C[0x4C - 0x3C];
    float f4C;          // 0x4C
    float f50;          // 0x50
    char pad54[0x5C - 0x54];
};

// Message 0xC14 payload (effect trigger).
struct FxMsg {
    int a;
    int id;
    u32 key;
};

// Mangled C++ callees.
int ModelAnimate(int model, char* bones, int a);               // 0x8003AA9C
void BoneGetMatrix(Mtx34* m, void* bone);                       // 0x800A0EF0
void MtxGetEuler(Vec3* rot, Mtx34* m);                          // 0x8009FB94
void PlacementToMatrix(Mtx34* m, PlaceF* p);                    // 0x8009EF04
void ActorAim(int* out, Actor* a, int n);                       // 0x8009F700
void FollowSetTargetVec(void* follow, Vec3* v);                 // 0x800A78E4
int HasCondition(void* conditions, int which);                  // 0x800A7AC4
int ConditionSetBit(void* cond, int bit, int set);              // 0x800A7A54
int ConditionUpdateBit(void* cond, int bit, int set);           // 0x800A7B08
int MemFlagIsSet(void* flags, int bit);                         // 0x800A7B78
void* VAlloc(int size);                                         // 0x8009DA68
void VFree(void* p);                                            // 0x8009DA3C
u32 AssetFind(u32 table, u32 key);                              // 0x8007EB34
int FxFind(const char* name);                                   // 0x80170834
int ActorPlayAnim(int actor, u32 anim, int arg);
void ActorGetMatrix(Mtx34* m, Actor* a);                         // 0x8009ED70                // 0x800C5654
void FontRelease(int id);                                       // 0x8007EEA4
void Vec3Set(Vec3* v, float x, float y, float z);               // 0x8016D67C

extern "C" {
int SendMsg(int to, int msg, void* data);
int rand(void);
float __fabsf(float f);
inline float fabsf(float x) { return __fabsf(x); }
float fn_800CC2D0(Actor* a, int b, int c, int d);
float fn_800E1020(Vec3* v);
void fn_8016D440(float* out, Vec3* v);
int fn_8016325C(void* anim);
int fn_8016F120(int fx, int* pos);
void fn_8016EEF4(int fx);
void fn_801D55CC(int h);
void fn_8008DE4C(int anim, int slot, int a, int b);
void fn_80090138(int anim);
void fn_8008F4F0(float f, int anim, int a);
void fn_80090974(float f, int anim, int a, int h, int b, void* fn, void* arg);
int fn_800A1BC8(int a, int b, u32 type, int c, int d);
void fn_800CDD10(void);
void fn_8016D864(void* out, void* in);
int fn_80018674(void* a, void* b, void* c, void* d);
void fn_800E0CB8(int* in, int* out);
void* memset(void* p, int c, unsigned long n);
void fn_8008E5CC(int a);
void fn_8028B2D4(void);
}

extern int lbl_804D3740;
extern int lbl_804D3744;
extern int lbl_804D3748;
extern int lbl_804D374C;
extern int lbl_804D3750;
extern int lbl_804D3754;
extern int lbl_804D3758;
extern float lbl_804D36E4;
extern float lbl_804D36E8;
extern int lbl_804D54FC;
extern ShellState lbl_803BFB70[];
extern u32 lbl_803C0008[10];
extern u32 lbl_804A2A00[10];
extern u32 lbl_804D3760;
extern u32 lbl_804D3764;
extern u32 lbl_804D3768;
int ShellCrabMsg(int msg, void* data);


// TODO (not drafted yet, 0x430 bytes at 0x8025ECF8): spawn the two roll-dust emitters
// TODO (not drafted yet, 0x4DC bytes at 0x8025F128): per-frame effects
// TODO (not drafted yet, 0x6FC bytes at 0x8025F604): message handler
// TODO (not drafted yet, 0x78 bytes at 0x8025FDA0): String-derived class deleting destructor
// TODO (not drafted yet, 0x6DC bytes at 0x802600AC): state machine driver
// TODO (not drafted yet, 0x4D4 bytes at 0x80260788): state machine event
void ShellCrabFree(Actor* a);
void ShellCrabInit(int owner, Actor* a);
int ShellCrabEvent(int msg, Actor* a);
int ShellCrabEventSimple(int msg, Actor* a);
int ShellCrabAnimCb(int msg, int* data);
int ShellEnterBoneSnap(Actor* a, int next);
int ShellEnterBoneSnapMsg(Actor* a, int next);
bool ShellLeaveGuard(Actor* a, int next);
bool ShellLeaveHurt(Actor* a, int next);
bool ShellLeaveA(Actor* a, int next);
bool ShellLeaveB(Actor* a, int next);
bool ShellLeaveC(Actor* a, int next);
bool ShellLeaveD(Actor* a, int next);
bool ShellLeaveE(Actor* a, int next);
int ShellLeaveF(Actor* a, int next);
int ShellGetState(Actor* a);
int ShellStepAndAim(Actor* a);
// TODO (not drafted yet, 0x898 bytes at 0x80261480): state update
int ShellUpdateState(Actor* a);
// TODO (not drafted yet, 0xB44 bytes at 0x80261D58): state update
int ShellUpdateTurn(Actor* a);
int ShellUpdateB(Actor* a);
int ShellUpdateC(Actor* a);
int ShellUpdateD(Actor* a);
int ShellUpdateE(Actor* a);
int ShellUpdateF(Actor* a);
int ShellUpdateG(Actor* a);
int ShellUpdateH(Actor* a);
int ShellUpdateI(Actor* a);
int ShellUpdateJ(Actor* a);
int ShellUpdateK(Actor* a);
int ShellUpdateL(Actor* a);
int ShellUpdateM(Actor* a);
int ShellUpdateN(Actor* a);
int ShellUpdateStateB(Actor* a);
// TODO (not drafted yet, 0x8A4 bytes at 0x8026328C): state update
int ShellEnter30(Actor* a);
int ShellEnter2F(Actor* a);
int ShellEnter2E(Actor* a);
int ShellEnter2D(Actor* a);
int ShellEnter2C(Actor* a);
int ShellEnter2B(Actor* a);
int ShellEnter2A(Actor* a);
int ShellEnter29(Actor* a);
int ShellEnter28(Actor* a);
int ShellEnter27(Actor* a);
int ShellEnter26(Actor* a);
int ShellEnter25(Actor* a);
int ShellEnter24(Actor* a);
int ShellEnter23(Actor* a);
int ShellEnter02(Actor* a);
int ShellEnter1B(Actor* a);
int ShellEnter19(Actor* a);
int ShellEnter1A(Actor* a);
int ShellEnter18(Actor* a);
int ShellEnter17(Actor* a);
int ShellEnter16(Actor* a);
int ShellEnter12(Actor* a);
int ShellEnter13(Actor* a);
int ShellEnter11(Actor* a);
int ShellEnter15(Actor* a);
int ShellEnter10(Actor* a);
int ShellEnter0F(Actor* a);
int ShellEnter14(Actor* a);
int ShellEnter0E(Actor* a);
int ShellEnter0D(Actor* a);
int ShellEnter0C(Actor* a);
int ShellEnter0B(Actor* a);
int ShellEnter0A(Actor* a);
int ShellEnter09(Actor* a);
int ShellEnter05(Actor* a);
int ShellEnter22(Actor* a);
int ShellEnter21(Actor* a);
int ShellEnter20(Actor* a);
int ShellEnter1F(Actor* a);
int ShellEnter1E(Actor* a);
int ShellEnter1D(Actor* a);
int ShellEnter1C(Actor* a);
int ShellEnter08(Actor* a);
int ShellEnter07(Actor* a);
int ShellEnter06(Actor* a);
int ShellEnter04(Actor* a);
int ShellEnter03(Actor* a);
int ShellEnter01(Actor* a);
int ShellEnter00(Actor* a);
// TODO (not drafted yet, 0xD70 bytes at 0x80264D5C): per-frame step (state update, movement, effects)
// TODO (not drafted yet, 0x7B8 bytes at 0x80265ACC): animation setup for a state
int ShellCrabHitCb(u32 unused, Actor* other, Actor* self);
// TODO (not drafted yet, 0x68 bytes at 0x802663E4): String-derived class destructor
// TODO (not drafted yet, 0x48 bytes at 0x8026644C): array construct helper
// TODO (not drafted yet, 0x60 bytes at 0x80266494): array destruct helper
// TODO (not drafted yet, 0x8 bytes at 0x802664F4): returns 8
// TODO (not drafted yet, 0x20 bytes at 0x802664FC): array loop helper
// TODO (not drafted yet, 0x20 bytes at 0x8026651C): array loop helper
// TODO (not drafted yet, 0x6C bytes at 0x8026653C): static table init

void ShellCrabStep(Actor* a);
void ShellCrabSetAnim(Actor* a, int flags);

// Detach the sound handle, free the String and the state block (message 0x401).
void ShellCrabFree(Actor* a) {
    if (a != 0 && a->beh != 0) {
        ShellBeh* b = a->beh;
        if (b->i7C != -1) {
            FontRelease(b->i7C);
            b->i7C = -1;
        }
        while (((u32)b->name->mLength >> 3) != 0) {
            fn_8028B2D4();
        }
        if (b->name != 0) {
            delete b->name;
        }
        VFree(a->beh);
        a->cb110 = 0;
        a->beh = 0;
    }
}

int ShellCrabEvent(int msg, Actor* a) {
    ShellBeh* b = a->beh;

    if (b->state == 1 && msg == 0x1B83833F) {
        int n = b->next;
        if (n != 1) {
            if (n == 0x23 && b->f94 > 1.1920929e-07f) {
                b->next = 1;
            } else if (n == 0x16 && b->i90 == 3) {
                b->next = 1;
            }
        }
        lbl_804D54FC = b->i90;
        b->i90 = (b->i90 + 1) % 4;
        lbl_803BFB70[b->state].setState(a, b->next);
        lbl_803BFB70[b->next].enter(a);
    }
    return 1;
}

int ShellCrabEventSimple(int msg, Actor* a) {
    ShellBeh* b = a->beh;

    if (msg == 0x1B83833F) {
        lbl_803BFB70[b->state].setState(a, b->next);
        lbl_803BFB70[b->next].enter(a);
    } else if (msg < 0x1B83833F) {
        if (msg == -0x68C67389 && a->handle != -1) {
            FxMsg m;
            m.id = a->handle;
            m.key = 0xEC1B94EE;
            m.a = 0;
            SendMsg(a->handle, 0xC14, &m);
        }
    } else if (msg == 0x703BA9B7 && a->handle != -1) {
        FxMsg m;
        m.id = a->handle;
        m.key = 0x5A326F65;
        m.a = 0;
        SendMsg(a->handle, 0xC14, &m);
    }
    return 1;
}

// Animation event: blends the next animation for the actor and frees the event block.
int ShellCrabAnimCb(int msg, int* data) {
    Actor* a = (Actor*)data[0];

    if (msg == 0x1B83833F) {
        int h = fn_800A1BC8(a->data->i14, a->data->i1C, 0x06C8BBAB, data[1] - 3, 0);
        fn_8008F4F0(1.0f, a->anim, data[1]);
        fn_80090974(1.0f, a->anim, data[1], h, 0, 0, 0);
    }
    VFree(data);
    return 1;
}

int ShellEnterBoneSnap(Actor* a, int next) {
    ShellBeh* b = a->beh;
    Mtx34 m;
    Vec3 p;
    PlaceF place;

    a->f240 = 0.0f;
    a->f244 = 0.0f;
    a->f248 = 0.0f;
    BoneGetMatrix(&m, a->bones);
    MtxGetEuler(&a->euler, &m);
    p.x = a->bones->x;
    p.y = a->bones->y;
    p.z = a->bones->z;
    PlacementToMatrix((Mtx34*)a, (PlaceF*)&m);
    a->f84 = p.x;
    a->f88 = p.y;
    a->f8C = p.z;
    ConditionSetBit(a->conditions, 0x33, 0);
    b->next = next;
    b->flags &= ~4;
    return 1;
}

int ShellEnterBoneSnapMsg(Actor* a, int next) {
    ShellBeh* b = a->beh;
    Mtx34 m;
    Vec3 p;
    Vec3 d;
    float len;
    float tmp[3];

    a->f240 = 0.0f;
    a->f244 = 0.0f;
    a->f248 = 0.0f;
    BoneGetMatrix(&m, a->bones);
    MtxGetEuler(&a->euler, &m);
    p.x = a->bones->x;
    p.y = a->bones->y;
    p.z = a->bones->z;
    PlacementToMatrix((Mtx34*)a, (PlaceF*)&m);
    a->f84 = p.x;
    a->f88 = p.y;
    a->f8C = p.z;
    d.x = a->f84 - b->posX;
    d.y = a->f88 - b->posY;
    d.z = a->f8C - b->posZ;
    len = fn_800E1020(&d);
    b->dist = len;
    fn_8016D440(tmp, &d);
    b->fB8 = tmp[2];
    if (a->handle != -1) {
        FxMsg msg;
        msg.id = a->handle;
        msg.key = 0x0563C6EA;
        msg.a = 0;
        SendMsg(a->handle, 0xC14, &msg);
    }
    b->next = next;
    return 1;
}

bool ShellLeaveGuard(Actor* a, int next) {
    ShellBeh* b = a->beh;

    if (b->state == 2 && b->next == 2) {
        return false;
    }
    b->next = next;
    return false;
}

bool ShellLeaveHurt(Actor* a, int next) {
    ShellBeh* b = a->beh;

    b->next = next;
    b->i50 = 0;
    if (next != 0x30) {
        ConditionSetBit(a->conditions, 0x34, 0);
        ConditionSetBit(a->conditions, 0x35, 0);
    }
    return next == 0x30;
}

bool ShellLeaveA(Actor* a, int next) {
    a->beh->next = next;
    ConditionUpdateBit(a->conditions, 4, 0);
    return next == 0x30;
}

bool ShellLeaveB(Actor* a, int next) {
    ShellBeh* b = a->beh;
    bool r = next == 0x30;

    b->next = next;
    b->i90 = 0;
    b->f98 = 0.0f;
    b->f94 = 0.0f;
    return r;
}

bool ShellLeaveC(Actor* a, int next) {
    ShellBeh* b = a->beh;

    b->next = next;
    b->i90 = 0;
    return next == 0x30;
}

bool ShellLeaveD(Actor* a, int next) {
    ShellBeh* b = a->beh;

    b->next = next;
    ConditionSetBit(a->conditions, 0x37, 0);
    ConditionUpdateBit(a->conditions, 4, 0);
    b->i90 = 0;
    b->f98 = 0.0f;
    b->f94 = 0.0f;
    return next == 0x30;
}

bool ShellLeaveE(Actor* a, int next) {
    bool r = next == 0x30;

    a->beh->next = next;
    return r;
}

int ShellLeaveF(Actor* a, int next) {
    a->beh->next = next;
    return 1;
}

int ShellGetState(Actor* a) {
    if (a != 0) {
        return a->beh->state;
    }
    return 0;
}

int ShellStepAndAim(Actor* a) {
    ShellBeh* b = a->beh;
    int v[3];
    Vec3 p;

    ShellCrabStep(a);
    ActorAim(v, a, 1);
    v[0] = v[0] * 1000 + a->i24;
    v[1] = v[1] * 1000 + a->i28;
    v[2] = v[2] * 1000 + a->i2C;
    p.x = (float)v[0];
    p.y = (float)v[1];
    p.z = (float)v[2];
    FollowSetTargetVec(a->conditions, &p);
    return b->state;
}

int ShellUpdateState(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ShellBeh* b = a->beh;
    ShellCrabStep(a);
    return b->state;
}

// Allocate the state block and look up the assets and effects the crab uses.
void ShellCrabInit(int owner, Actor* a) {
    if (a == 0) {
        return;
    }
    ShellBeh* b = (ShellBeh*)VAlloc(0xBC);
    if (b == 0) {
        a->beh = 0;
        return;
    }
    a->beh = b;
    a->cb110 = ShellCrabMsg;
    a->cb118 = fn_800CDD10;
    b->owner = owner;
    b->f5C = 0.0f;
    b->f60 = 0.0f;
    b->flags = 0;
    b->hash4C = AssetFind(0xD9044CBB, 0x8C443E02);
    b->i50 = 0;
    b->i54 = 0;
    b->fx = -1;
    b->i7C = -1;
    b->f98 = 0.0f;
    b->f94 = 0.0f;
    b->fA4 = 1.0f;
    b->i90 = 0;
    b->f84 = 1.0f;
    b->f88 = 1.0f;
    b->f8C = 0.0f;
    b->i40 = 0;
    b->i58 = 0;
    b->name = new String();
    b->animA = -1;
    b->animB = -1;
    b->animC = -1;
    b->animD = -1;
    b->animE = -1;
    b->f28 = 0.0f;
    b->f2C = 0.0f;
    b->f30 = 0.0f;
    for (int i = 0; i < 10; i++) {
        lbl_804A2A00[i] = AssetFind(0x57F77CF9, lbl_803C0008[i]);
    }
    b->state = 1;
    b->next = 0x22;
    b->i3C = -1;
    lbl_804D3760 = AssetFind(0xF2E2BEB8, 0x0A8B74B2);
    lbl_804D3764 = AssetFind(0xF2E2BEB8, 0x0A36E7AC);
    lbl_804D3768 = AssetFind(0xF2E2BEB8, 0x0A28D02B);
    ConditionSetBit(a->conditions, 1, 1);
    if (a->h5F4 != 0) {
        fn_801D55CC(a->h5F4);
        a->h5F4 = 0;
    }
    ConditionUpdateBit(a->conditions, 0x25, 0);
    lbl_804D3740 = FxFind("crab_roll2");
    lbl_804D3744 = FxFind("crab_pain");
    lbl_804D3748 = FxFind("shell_pain2");
    lbl_804D3750 = FxFind("crab_meele");
    lbl_804D3754 = FxFind("shell_roar2");
    lbl_804D3758 = FxFind("shell_laser");
    lbl_804D374C = FxFind("crab_pain");
}

// Turn towards the heading, then step.
int ShellUpdateTurn(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ShellBeh* b = a->beh;
    if ((b->flags & 8) == 0) {
        float d = a->data2->heading - a->euler.z;
        while (d > 90.0f) {
            d -= 180.0f;
        }
        while (d < -90.0f) {
            d += 180.0f;
        }
        if (fabsf(d) >= 5.0f) {
            if (d > 1.1920929e-07f) {
                float lim = 0.001f * a->data->turnRate * a->dt;
                if (lim < d) {
                    d = lim;
                }
                a->euler.z += d;
            } else if (d < -1.1920929e-07f) {
                float lim = -(0.001f * a->data->turnRate * a->dt);
                if (d < lim) {
                    d = lim;
                }
                a->euler.z += d;
            }
        } else {
            b->flags |= 8;
            ShellCrabSetAnim(a, 0x418);
        }
        ShellCrabStep(a);
    } else {
        ShellCrabStep(a);
    }
    return b->state;
}

int ShellUpdateB(Actor* a) {
    ShellBeh* b = a->beh;

    ShellCrabStep(a);
    if (HasCondition(a->conditions, 0x24) || HasCondition(a->conditions, 0x25)) {
        if ((b->flags & 2) == 0) {
            return 0x2A;
        }
        return 0x28;
    }
    if (fn_8016325C(a->animState) == 4) {
        return 0x26;
    }
    if (HasCondition(a->conditions, 6) == 0) {
        return 0x26;
    }
    return b->state;
}

int ShellUpdateC(Actor* a) {
    ShellBeh* b = a->beh;

    ShellCrabStep(a);
    if (HasCondition(a->conditions, 6) == 0) {
        if (fn_8016325C(a->animState) == 4 || fn_8016325C(a->animState) == 5) {
            return b->state;
        }
        return 10;
    }
    return 10;
}

int ShellUpdateD(Actor* a) {
    ShellBeh* b = a->beh;

    ShellCrabStep(a);
    if (HasCondition(a->conditions, 6) == 0) {
        if (fn_8016325C(a->animState) == 4 || fn_8016325C(a->animState) == 5) {
            return b->state;
        }
        return 1;
    }
    return 1;
}

int ShellUpdateE(Actor* a) {
    ShellBeh* b = a->beh;

    ShellCrabStep(a);
    if (HasCondition(a->conditions, 6) == 0) {
        if (HasCondition(a->conditions, 0x34) == 0 && HasCondition(a->conditions, 0x35) == 0) {
            return 0x22;
        }
        return b->state;
    }
    return 0x22;
}

int ShellUpdateF(Actor* a) {
    ShellBeh* b = a->beh;

    ShellCrabStep(a);
    if (HasCondition(a->conditions, 6) == 0) {
        if (fn_8016325C(a->animState) == 4 || fn_8016325C(a->animState) == 5) {
            return b->state;
        }
        return 5;
    }
    return 5;
}

int ShellUpdateG(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ShellBeh* b = a->beh;
    ShellCrabStep(a);
    if (HasCondition(a->conditions, 0x30) == 0 && b->next == 0x1D) {
        b->next = 0x1E;
    }
    return b->state;
}

int ShellUpdateH(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ShellBeh* b = a->beh;
    a->euler.z = 0.0f;
    a->f2B0 = 0.0f;
    ShellCrabStep(a);
    return b->state;
}

int ShellUpdateStateB(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ShellBeh* b = a->beh;
    ShellCrabStep(a);
    return b->state;
}

int ShellUpdateI(Actor* a) {
    ShellBeh* b = a->beh;

    b->timer += a->dt;
    ShellCrabStep(a);
    if (b->limit < b->timer) {
        if (HasCondition(a->conditions, 0x2F)) {
            b->i58 = 1;
        }
        ConditionSetBit(a->conditions, 0x2F, 0);
    }
    if (fn_8016325C(a->animState) == 4) {
        return 3;
    }
    if (HasCondition(a->conditions, 0x3A)) {
        return 9;
    }
    if (HasCondition(a->conditions, 6)) {
        return 0x1A;
    }
    if (HasCondition(a->conditions, 0x2F) == 0 && b->i58 == 0) {
        return 0x1B;
    }
    return b->state;
}

int ShellUpdateJ(Actor* a) {
    ShellBeh* b = a->beh;

    b->timer += a->dt;
    ShellCrabStep(a);
    if (HasCondition(a->conditions, 6) == 0) {
        if (fn_8016325C(a->animState) == 4 || fn_8016325C(a->animState) == 5) {
            return b->state;
        }
        return 0x14;
    }
    return 0x14;
}

int ShellUpdateK(Actor* a) {
    ShellBeh* b = a->beh;

    b->timer += a->dt;
    ShellCrabStep(a);
    if (HasCondition(a->conditions, 0x30) == 0) {
        if (HasCondition(a->conditions, 0x34) || HasCondition(a->conditions, 0x35) ||
            MemFlagIsSet(a->conditions, 0x25) || b->timer > 250.0f) {
            return 0x13;
        }
        if (fn_8016325C(a->animState) == 5) {
            return 0x13;
        }
        if (HasCondition(a->conditions, 6) == 0) {
            return b->state;
        }
        return 1;
    }
    return 0x14;
}

int ShellUpdateL(Actor* a) {
    ShellBeh* b = a->beh;
    float chance;

    b->timer += a->dt;
    ShellCrabStep(a);
    if (HasCondition(a->conditions, 0x30) == 0) {
        if (HasCondition(a->conditions, 0x34) == 0 && HasCondition(a->conditions, 0x35) == 0 &&
            MemFlagIsSet(a->conditions, 0x25) == 0) {
            if (fn_8016325C(a->animState) == 5) {
                chance = 0.3f * (a->f904 / (float)a->data->i30) + 0.2f;
                if (chance <= (float)rand() / 32767.0f) {
                    return 0x10;
                }
                return 0xF;
            }
            if (b->timer <= 1000.0f) {
                if (HasCondition(a->conditions, 6) == 0) {
                    return b->state;
                }
                return 0x15;
            }
            return 0x12;
        }
        return 0x15;
    }
    return 0x15;
}

int ShellUpdateM(Actor* a) {
    ShellBeh* b = a->beh;

    ShellCrabStep(a);
    if (HasCondition(a->conditions, 0x30) == 0) {
        if (HasCondition(a->conditions, 0x37) == 0) {
            if (HasCondition(a->conditions, 0x34) == 0 && HasCondition(a->conditions, 0x35) == 0 &&
                HasCondition(a->conditions, 0x3A) != 0) {
                if (fn_8016325C(a->animState) == 5) {
                    return 0xB;
                }
                if (HasCondition(a->conditions, 6) == 0) {
                    return b->state;
                }
                return 0xC;
            }
            return 0xC;
        }
        return 0xD;
    }
    return 0xC;
}

int ShellUpdateN(Actor* a) {
    ShellBeh* b = a->beh;

    ShellCrabStep(a);
    if (HasCondition(a->conditions, 0x30) == 0) {
        if (fn_8016325C(a->animState) == 4) {
            return 3;
        }
        if (fn_8016325C(a->animState) == 5) {
            return 0x22;
        }
        if (HasCondition(a->conditions, 6) == 0) {
            return b->state;
        }
        return 0x23;
    }
    return 0x1D;
}

// State 0x30: dead; park both animation slots and announce it once.
int ShellEnter30(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x30;
    b->next = 0x30;
    fn_8008DE4C(a->anim, 0, b->animA, 0x10);
    fn_8008DE4C(a->anim, 1, b->animB, 0x10);
    if (HasCondition(a->conditions, 10) == 0) {
        ConditionSetBit(a->conditions, 10, 1);
        ActorPlayAnim(a->handle, 0xDC04E3D5, a->handle);
    }
    return 1;
}

int ShellEnter2F(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x2F;
    b->next = 0x30;
    ActorPlayAnim(a->handle, 0xDC04E3D5, a->handle);
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter2E(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x2E;
    b->next = 0x24;
    ConditionSetBit(a->conditions, 0xE, 1);
    ShellCrabSetAnim(a, 0x418);
    return 1;
}

int ShellEnter2D(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x2D;
    b->next = 0x2D;
    ConditionSetBit(a->conditions, 0xE, 1);
    ShellCrabSetAnim(a, 0x408);
    return 1;
}

int ShellEnter2C(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x2C;
    b->next = 0x24;
    ConditionSetBit(a->conditions, 0xE, 1);
    ShellCrabSetAnim(a, 0x418);
    return 1;
}

int ShellEnter2B(Actor* a) {
    ShellBeh* b = a->beh;

    if (b->state != 0x2B) {
        b->f8C = 0.0f;
    }
    b->state = 0x2B;
    b->next = 0x2B;
    ConditionSetBit(a->conditions, 0xE, 1);
    ShellCrabSetAnim(a, 0x408);
    return 1;
}

int ShellEnter2A(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x2A;
    b->next = 0x2B;
    b->f6C = 0.0f;
    if (HasCondition(a->conditions, 0x24) == 0) {
        b->i3C = 1;
    } else {
        b->i3C = 0;
    }
    ConditionSetBit(a->conditions, 0xE, 1);
    ShellCrabSetAnim(a, 0x418);
    b->fx = fn_8016F120(lbl_804D3740, &a->i24);
    return 1;
}

int ShellEnter29(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x29;
    b->next = 0x24;
    ConditionSetBit(a->conditions, 0xE, 1);
    ShellCrabSetAnim(a, 0x408);
    return 1;
}

int ShellEnter28(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x28;
    b->next = 0x24;
    b->flags |= 4;
    b->flags &= ~2;
    b->flags &= ~8;
    b->posX = a->f84;
    b->posY = a->f88;
    b->posZ = a->f8C;
    fn_80090138(a->anim);
    b->fx = fn_8016F120(lbl_804D3740, &a->i24);
    return 1;
}

int ShellEnter27(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x27;
    b->i50 = 0xFF;
    ConditionUpdateBit(a->conditions, 4, 1);
    ConditionSetBit(a->conditions, 0x37, 1);
    b->next = 1;
    fn_8016F120(lbl_804D3754, &a->i24);
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter26(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x26;
    if (fn_8016325C(a->animState) == 4) {
        b->next = 0;
    } else {
        b->next = 0x1D;
    }
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter25(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x25;
    if (HasCondition(a->conditions, 6) == 0) {
        b->next = 0x1D;
    } else {
        b->next = 0x24;
    }
    if (a->handle != -1) {
        FxMsg m;
        m.id = a->handle;
        m.key = 0xEC1B94EE;
        m.a = 0;
        SendMsg(a->handle, 0xC14, &m);
    }
    ShellCrabSetAnim(a, 0x410);
    fn_8016F120(lbl_804D3748, &a->i24);
    return 1;
}

int ShellEnter24(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x24;
    b->next = 0x24;
    ConditionSetBit(a->conditions, 0xE, 1);
    if (a->handle != -1) {
        FxMsg m;
        m.id = a->handle;
        m.key = 0x5A36ACDA;
        m.a = 0;
        SendMsg(a->handle, 0xC14, &m);
    }
    ShellCrabSetAnim(a, 0x408);
    if (b->fx != -1) {
        fn_8016EEF4(b->fx);
        b->fx = -1;
    }
    return 1;
}

int ShellEnter23(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x23;
    b->i50 = 0;
    if ((b->flags & 2) == 0) {
        b->next = 0x24;
    } else {
        b->next = 0x28;
    }
    ShellCrabSetAnim(a, 0x410);
    fn_8016F120(lbl_804D3744, &a->i24);
    return 1;
}

int ShellEnter02(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 2;
    b->next = 0;
    ConditionSetBit(a->conditions, 0xE, 1);
    ShellCrabSetAnim(a, 0x10);
    fn_8016F120(lbl_804D374C, &a->i24);
    return 1;
}

int ShellEnter1B(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x1B;
    if (HasCondition(a->conditions, 6) == 0) {
        b->next = 0x27;
    } else {
        b->next = 1;
    }
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter19(Actor* a) {
    ShellBeh* b = a->beh;

    if (b->state != 0x19) {
        b->timer = 0.0f;
        b->i58 = 0;
        if (a->handle != -1) {
            FxMsg m;
            m.id = a->handle;
            m.key = 0x843F5271;
            m.a = 0;
            SendMsg(a->handle, 0xC14, &m);
        }
    }
    b->state = 0x19;
    b->next = 0x19;
    b->limit = (lbl_804D36E8 - lbl_804D36E4) * (a->f904 / (float)a->data->i30) + lbl_804D36E4;
    ShellCrabSetAnim(a, 0x400);
    return 1;
}

int ShellEnter1A(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x1A;
    b->next = 0x24;
    fn_8016F120(lbl_804D3744, &a->i24);
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter18(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x18;
    b->next = 0x19;
    b->timer = 0.0f;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter17(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x17;
    b->next = 0x19;
    b->timer = 0.0f;
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter16(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x16;
    b->next = 0x17;
    b->timer = 0.0f;
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter12(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x12;
    b->next = 0x11;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter13(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x13;
    b->next = 0x14;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter11(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x11;
    b->next = 0x11;
    b->timer = 0.0f;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter15(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x15;
    b->next = 1;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter10(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x10;
    b->next = 0x14;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter0F(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0xF;
    b->next = 0x14;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter14(Actor* a) {
    ShellBeh* b = a->beh;

    if (b->state == 1 || b->state == 0x13) {
        b->timer = 0.0f;
    }
    b->state = 0x14;
    b->next = 0x14;
    ShellCrabSetAnim(a, 0);
    return 1;
}

int ShellEnter0E(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0xE;
    b->next = 0x14;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter0D(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0xD;
    b->next = 0x27;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter0C(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0xC;
    b->next = 1;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter0B(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0xB;
    b->next = 10;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter0A(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 10;
    b->next = 10;
    ShellCrabSetAnim(a, 0);
    return 1;
}

int ShellEnter09(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 9;
    b->next = 10;
    ShellCrabSetAnim(a, 0x10);
    return 1;
}

int ShellEnter05(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 5;
    b->next = 1;
    b->i90 = 0;
    b->f98 = 0.0f;
    ConditionUpdateBit(a->conditions, 0x25, 0);
    if (b->fx != -1) {
        fn_8016EEF4(b->fx);
        b->fx = -1;
    }
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter22(Actor* a) {
    ShellBeh* b = a->beh;

    ConditionSetBit(a->conditions, 0x36, 0);
    b->state = 0x22;
    b->next = 1;
    ShellCrabSetAnim(a, 0);
    return 1;
}

int ShellEnter21(Actor* a) {
    ShellBeh* b = a->beh;

    ConditionSetBit(a->conditions, 0x36, 1);
    b->state = 0x21;
    b->next = 0x21;
    ShellCrabSetAnim(a, 0);
    return 1;
}

int ShellEnter20(Actor* a) {
    ShellBeh* b = a->beh;

    ConditionSetBit(a->conditions, 0x36, 1);
    b->state = 0x20;
    b->next = 0x20;
    ShellCrabSetAnim(a, 0);
    return 1;
}

int ShellEnter1F(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x1F;
    b->i50 = 0xFF;
    if (HasCondition(a->conditions, 0x35) == 0) {
        b->next = 0x20;
    } else {
        b->next = 0x21;
    }
    ShellCrabSetAnim(a, 0);
    return 1;
}

int ShellEnter1E(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x1E;
    b->next = 1;
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter1D(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 0x1D;
    b->next = 0x1D;
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter1C(Actor* a) {
    ShellBeh* b = a->beh;

    if (b->state != 0x1C) {
        b->timer = 0.0f;
    }
    b->state = 0x1C;
    b->next = 0x1D;
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter08(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 8;
    b->next = 1;
    ShellCrabSetAnim(a, 0);
    return 1;
}

int ShellEnter07(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 7;
    b->next = 1;
    ShellCrabSetAnim(a, 0);
    return 1;
}

int ShellEnter06(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 6;
    b->next = 0;
    ShellCrabSetAnim(a, 0);
    return 1;
}

int ShellEnter04(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 4;
    b->next = 4;
    ShellCrabSetAnim(a, 0x400);
    return 1;
}

int ShellEnter03(Actor* a) {
    ShellBeh* b = a->beh;

    b->state = 3;
    b->next = 4;
    a->euler.z = 0.0f;
    a->f2B0 = 0.0f;
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter01(Actor* a) {
    ShellBeh* b = a->beh;

    if (b->state != 1) {
        b->timer = 0.0f;
    }
    b->next = 1;
    b->state = 1;
    ShellCrabSetAnim(a, 0x410);
    return 1;
}

int ShellEnter00(Actor* a) {
    ShellBeh* b = a->beh;

    b->next = 0;
    b->state = 0;
    ShellCrabSetAnim(a, 0);
    return 1;
}

// Touch damage: when the bounding boxes overlap, hit the other actor away from the crab.
int ShellCrabHitCb(u32 unused, Actor* other, Actor* self) {
    if (other == 0 || self == 0) {
        return 0;
    }
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
        hit.source = self->handle;
        hit.key = 0x53D456BE;
        hit.force = 800.0f * 0.001f * self->dt;
        hit.pos[0] = self->i24;
        hit.pos[1] = self->i28;
        hit.pos[2] = self->i2C;
        hit.dir[0] = hit.pos[0] - other->i24;
        hit.dir[1] = hit.pos[1] - other->i28;
        hit.dir[2] = hit.pos[2] - other->i2C;
        fn_800E0CB8(hit.dir, hit.dir);
        hit.back[0] = -hit.dir[0];
        hit.back[1] = -hit.dir[1];
        hit.back[2] = -hit.dir[2];
        hit.f4C = 10.0f;
        hit.f50 = 1.5f;
        SendMsg(other->handle, 0x407, &hit);
    }
    return 1;
}

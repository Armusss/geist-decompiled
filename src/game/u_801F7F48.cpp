// u_801F7F48.cpp: crouching fear behaviour variant (0x801F7F48-0x801FC634).
// The crouch sibling of the fear AI in u_801F2DA4: an actor with a ten-state machine (idle, run, cower, crouch ...) driven from a state table of
// {anim hash, enter, canLeave, update, handler, name} records.  The message handler answers the
// state-name query, forwards hits to the damage code and stashes one pending 0x1800 message to
// replay when the state machine ends.  Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the actor actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

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

struct Vec3 {
    float x, y, z;
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

extern "C" {
void* memcpy(void* dst, const void* src, unsigned long n);
int rand(void);
int SendMsg(int to, int msg, void* data);
}

struct CrouchBeh;

struct Mtx34 {
    float m[12];
};

struct Pose {
    char pad0[0x10];
    float pos[3];           // 0x10
};

struct ActorInfo {
    char pad0[8];
    float unk08;            // 0x08
    char pad0C[0xC4 - 0x0C];
    float unkC4;            // 0xC4
    char padC8[0x168 - 0xC8];
    u32 animSet;            // 0x168
};

// Actor (only the fields used here).
struct Actor {
    char pad0[0x24];
    int pos[3];             // 0x24
    char pad30[8];          // 0x30
    int handle;             // 0x38
    char pad3C[0x84 - 0x3C];
    float p84[3];           // 0x84
    char pad90[0xC4 - 0x90];
    ActorInfo* info;        // 0xC4
    char padC8[4];          // 0xC8
    int model;              // 0xCC
    int unkD0;              // 0xD0
    int unkD4;              // 0xD4
    char padD8[0xF0 - 0xD8];
    Pose* pose;             // 0xF0
    char padF4[0x110 - 0xF4];
    int (*update)(Actor*);  // 0x110
    char pad114[4];         // 0x114
    void* unk118;           // 0x118
    char pad11C[0x134 - 0x11C];
    CrouchBeh* beh;           // 0x134
    float dt;               // 0x138
    char pad13C[0x220 - 0x13C];
    int unk220;             // 0x220
    char pad224[0x240 - 0x224];
    float vel[3];           // 0x240
    float rot[3];           // 0x24C
    float unk258;           // 0x258
    char pad25C[0x26C - 0x25C];
    int unk26C;             // 0x26C
    char flags[0x27C - 0x270]; // 0x270 condition bits
    float unk27C;           // 0x27C
    char pad280[4];
    float speed;            // 0x284
    char pad288[0x2B0 - 0x288];
    float unk2B0;           // 0x2B0
    char pad2B4[0x5E4 - 0x2B4];
    float unk5E4;           // 0x5E4
    char pad5E8[8];
    void* unk5F0;           // 0x5F0
    char pad5F4[0x608 - 0x5F4];
    int unk608;             // 0x608
    char pad60C[0x894 - 0x60C];
    char sm[0x8EC - 0x894]; // 0x894 sub state machine
    int unk8EC;             // 0x8EC
    int unk8F0;             // 0x8F0
    char pad8F4[0x908 - 0x8F4];
    char* unk908;           // 0x908
    char pad90C[0x918 - 0x90C];
    float unk918;           // 0x918
};

typedef void (*EnterFn)(Actor*);
typedef int (*LeaveFn)(Actor*, int);
typedef int (*UpdateFn)(Actor*);
typedef int (*HandlerFn)(int, Actor*);

struct StateDef {
    u32 hash;               // 0x00 animation set hash
    EnterFn enter;          // 0x04
    LeaveFn canLeave;       // 0x08
    UpdateFn update;        // 0x0C
    HandlerFn handler;      // 0x10
    const char* name;       // 0x14
};

extern StateDef gCrouchStates[10];

// Behaviour block (VAlloc'd, 0x34 bytes).
struct CrouchBeh {
    u32 animSet;            // 0x00
    u32 hash;               // 0x04
    int state;              // 0x08
    int next;               // 0x0C
    int anim;               // 0x10
    int unk14;              // 0x14
    int unk18;              // 0x18
    float timer;            // 0x1C
    float timer2;           // 0x20
    float ground;           // 0x24
    u32 flags;              // 0x28
    int pendingMsg;         // 0x2C
    void* pendingData;      // 0x30
};

// Message block seen by the actor message handler.
struct ActorMsg {
    Actor* actor;           // 0x00
    int unk04;              // 0x04
    int unk08;              // 0x08
    int unk0C;              // 0x0C
    int unk10;              // 0x10
    int unk14;              // 0x14
};

struct EventMsg {
    int unk0;
    int value;              // 0x04
    int target;             // 0x08
    u32 key;                // 0x0C
};

extern "C" {
int fn_800CBAA0(void* msg, int a, int b);
int fn_800C5210(int id, int msg, void* data);
void fn_802C24E4(void* a, void* b);
void fn_801BE690(Actor* a);
void fn_801BD7D4(Actor* a);
void fn_801BE5D4(Actor* a);
int fn_8016325C(void* sm);
int fn_80162610(Actor* a, void* sm, u32 msg);
int fn_8008EA9C(int a, int b);
void fn_8008DE4C(int a, int b, int c, int d);
int fn_80212730(int h);
void fn_80090634(int a, int b, int c, int d);
void fn_800CDD10(void);
float fn_800CC2D0(Actor* a, int n, int* p, int* q);
}
extern int OwnerId(Actor* a);
extern int ModelBoneName(int model, int bone);
extern void AnimBankUnref(u32 a, u32 b);
extern void AnimBankActivate(u32 a, u32 b);
extern int Fn800F7DEC(int a, int b, int c, int d, void* e);
extern int HasCondition(void* flags, int bit);
extern void ConditionSetBit(void* flags, int bit, int on);
extern int MemFlagIsSet(void* flags, int bit);
extern void VFree(void* p);
extern void* VAlloc(int size);
extern int AssetFind(u32 a, u32 b);

extern "C" {
int fn_800A1BC8(int a, int b, u32 hash, int c, int d);
int fn_800A1AC4(int a, int b, u32 hash);
int fn_8008E7D0(void);
void fn_8008F4F0(float f, int h, int n);
void fn_80090974(float f, int h, int n, int v, int m, int (*cb)(int, Actor*), Actor* a);
void fn_8008F4E8(int h, int n);
void fn_8008F378(int h);
void fn_8008F4EC(int h, int n);
void fn_8008F230(float f, int h, int a, int b);
void fn_800BBCA8(Actor* a);
}
extern void ActorGetMatrix(Mtx34* m, int actor);
extern void MtxSetEuler(Mtx34* m, Vec3* e);
extern void MtxGetEuler(Vec3* e, Mtx34* m);
extern void PlacementToMatrix(Actor* a, Mtx34* m);
extern void ModelSetMatrix(int model, Mtx34* m, Pose* pose);
extern void BoneGetMatrix(Mtx34* m, Pose* p);
extern void BoneSetMatrix(float* q, float* v);
extern void LinkInfoInit(Mtx34* m);
extern void EulerNormalize(Vec3* dst, Vec3* src);
extern void Vec3Set(Vec3* v, float x, float y, float z);
extern int FollowGetTargetPos(void* flags, Vec3* out);
extern int ModelAnimate(int model, char* anim, int n);
extern int ModelFindBone(int model, int id);
extern void AnimApply(int h, Pose* p, int anim, int d);
extern "C" {
void fn_800913EC(float dt, int h, float* out);
void fn_801BE4B8(Actor* a);
void fn_800A0168(Mtx34* dst, Mtx34* src);
void fn_800A0030(float* dst, Mtx34* m, float* src);
void fn_8009FC6C(Mtx34* dst, Mtx34* a, Mtx34* b);
void fn_800CA1B0(Actor* a, int b1, int b2, Mtx34* m, void* extra);
void PSQUATMultiply(Pose* a, float* b, float* c);
void C_QUATSlerp(float t, float* a, float* b, Pose* out);
}
extern void BoneWorldMatrix(float* dst, Pose* p, float* src);
extern void ActorPlayAnim(int h, u32 hash, int h2);
extern void SplineQuatRotateRamp(float f, Pose* pose, int bone1, int bone2, u32 id);
extern int SendMsgOut(int to, int msg, void* data);
extern int sCrouchUnk;      // 0x804D3348
extern int sCrouchBone;     // 0x804D334C

int CrouchUpdate(Actor* a);
void CrouchAdvance(Actor* a);


// Start an animation of the current state's set and blend to it.
inline void CrouchPlayAnim(Actor* a, int variant) {
    CrouchBeh* b;
    int anim;
    if (a == 0) {
        return;
    }
    b = a->beh;
    if (HasCondition(a->flags, 0xC) != 0) {
        ConditionSetBit(a->flags, 0xC, 0);
        ConditionSetBit(a->flags, 0xE, 1);
        fn_8008F4E8(a->unkD4, 0);
        fn_8008F378(a->unkD4);
        fn_8008F4EC(a->unkD4, 0);
    }
    anim = fn_800A1BC8(b->hash, b->animSet, gCrouchStates[b->state].hash, variant, 0);
    if (fn_8008E7D0() != 0) {
        ConditionSetBit(a->flags, 0x1C, 0);
        fn_80090974(1.0f, a->unkD4, 0, anim, 9, (int (*)(int, Actor*))gCrouchStates[b->state].handler, a);
        if (HasCondition(a->flags, 0xE) != 0 || b->anim == anim) {
            fn_8008F230(0.0f, a->unkD4, 0, 0);
            ConditionSetBit(a->flags, 0xE, 0);
        } else {
            fn_8008F230(150.0f, a->unkD4, 0, 1);
        }
        b->anim = anim;
    }
}

// 0x801F7F48: actor message handler.
int CrouchMsg(int msg, void* p) {
    ActorMsg* m = (ActorMsg*)p;
    Actor* a;
    CrouchBeh* b;
    switch (msg) {
    case 0:
        return fn_800CBAA0(m, 0, 0);
    case 10:
        return fn_800CBAA0(m, 0, 1);
    case 0x7300:
        return fn_800CBAA0(m, 1, 0);
    case 0x41E:
        if (m->unk04 == -0x527A8906) {
            String s1(gCrouchStates[m->actor->beh->state].name);
            String s2("");
        }
        break;
    case 0x1800:
        b = m->actor->beh;
        if (b->pendingMsg != -1) {
            b->pendingMsg = -1;
            VFree(b->pendingData);
        }
        b->pendingMsg = 0x1800;
        b->pendingData = VAlloc(0x20);
        memcpy(b->pendingData, m, 0x20);
        b->flags |= 1;
        return 1;
    case 0xC14: {
        int key = m->unk0C;
        a = m->actor;
        b = a->beh;
        if (key == 0x672F35C2) {
            ConditionSetBit(a->flags, 0xE, 1);
            fn_8008DE4C(a->unkD4, 0, b->anim, 0x18);
            fn_801BE690(a);
            fn_801BD7D4(a);
        } else if (key < 0x672F35C2 && key == -0x6C0AEB92) {
            b->flags |= 1;
            return 1;
        }
        return 0;
    }
    }
    return 0;
}

// 0x801F8328: per-frame update.
int CrouchUpdate(Actor* a) {
    CrouchBeh* b;
    int next;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (b->timer2 > 0.0f) {
        b->timer2 -= a->dt;
    }
    if (HasCondition(a->flags, 0x1C) != 0 ||
        ((fn_8008EA9C(a->unkD4, 0) == 0 || fn_8008EA9C(a->unkD4, 0) == 2) &&
         HasCondition(a->flags, 0xD) == 0 && HasCondition(a->flags, 9) == 0 &&
         HasCondition(a->flags, 10) == 0 && b->state != 8 && b->state != 9)) {
        gCrouchStates[b->state].enter(a);
    }
    if (a->unk220 == 4) {
        CrouchAdvance(a);
        next = b->state;
    } else {
        next = gCrouchStates[b->state].update(a);
    }
    if (next != b->state) {
        if (gCrouchStates[b->state].canLeave(a, next) != 0) {
            gCrouchStates[next].enter(a);
        }
    }
    if ((b->flags & 2) != 0) {
        int msg = b->pendingMsg;
        void* data = b->pendingData;
        b->pendingMsg = -1;
        b->pendingData = 0;
        ConditionSetBit(a->flags, 0xE, 1);
        fn_8008DE4C(a->unkD4, 0, b->anim, 0x18);
        fn_801BE690(a);
        fn_801BD7D4(a);
        if (a->handle != -1) {
            EventMsg e;
            e.key = 0x7DF38296;
            e.value = 0;
            e.target = a->handle;
            SendMsg(a->handle, 0xC14, &e);
        }
        if (msg != -1 && data != 0) {
            SendMsg(a->handle, msg, data);
            VFree(data);
        }
    }
    return 1;
}

// 0x801F8594: destroy.
void CrouchDestroy(Actor* a) {
    CrouchBeh* b = a->beh;
    AnimBankUnref(b->hash, b->animSet);
    a->update = 0;
    if (b->pendingMsg != -1) {
        b->pendingMsg = -1;
        VFree(b->pendingData);
    }
    VFree(a->beh);
    a->beh = 0;
}

// 0x801F8608: create.
void CrouchCreate(int unused, Actor* a) {
    CrouchBeh* b;
    if (a != 0) {
        a->update = CrouchUpdate;
        a->unk118 = fn_800CDD10;
        b = (CrouchBeh*)VAlloc(0x34);
        if (b == 0) {
            a->beh = 0;
        } else {
            u32 h = 0;
            const char* name = "FearAI_AnimWayCrouch";
            const char* p = name;
            const char* q = name;
            a->beh = b;
            b->state = 0;
            b->next = 2;
            b->anim = -1;
            b->flags = 0;
            b->timer = 3000.0f * ((float)rand() / 32767.0f) + 2000.0f;
            b->timer2 = 0.0f;
            b->pendingMsg = -1;
            b->pendingData = 0;
            a->unk918 = -999999.0f;
            b->ground = fn_800CC2D0(a, 1, &b->unk14, &b->unk18);
            sCrouchUnk = AssetFind(0xF2E2BEB8, 0x154CDE5C);
            sCrouchBone = AssetFind(0xF1754BF2, 0x21C5E0);
            for (; *q != 0; q++) {
                u32 c;
                if ((unsigned char)*p == 0xFF) {
                    c = 0xFFFFFFFF;
                } else {
                    c = __upper_map[(unsigned char)*p];
                }
                h = h * 31 + (c & 0xFF);
                p++;
            }
            b->hash = h % 0xFFFFFFFF;
            b->animSet = a->info->animSet;
            AnimBankActivate(b->hash, b->animSet);
            fn_80090634(a->unkD4, 0, 0, 0);
            ConditionSetBit(a->flags, 0x1C, 1);
        }
    }
}

// 0x801F87F4: state-machine handler A.
int CrouchSmHandlerA(int msg, Actor* a) {
    if (a == 0) {
        return 0;
    }
    fn_80162610(a, a->sm, msg);
    if (msg == 0x1B83833F) {
        a->beh->flags |= 2;
    }
    return 1;
}

// 0x801F8870: state-machine handler B.
int CrouchSmHandlerB(int msg, Actor* a) {
    if (a == 0) {
        return 0;
    }
    fn_80162610(a, a->sm, msg);
    if (msg == 0x1B83833F) {
        CrouchBeh* b = a->beh;
        gCrouchStates[b->state].canLeave(a, b->next);
        gCrouchStates[b->next].enter(a);
    }
    return 1;
}

// 0x801F8934: leave callback that only clears the pose bits.
int CrouchLeaveClear(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ConditionSetBit(a->flags, 0, 0);
    ConditionSetBit(a->flags, 4, 0);
    return 0;
}

// 0x801F898C: leave callback (cower).
int CrouchLeaveCower(Actor* a, int next) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->next = next;
    b->flags &= ~4;
    if (b->state == 6) {
        fn_80162610(a, a->sm, 0);
    }
    return 1;
}

// 0x801F89E8: leave callback (default).
int CrouchLeaveDefault(Actor* a, int next) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (next == 8 || next == 9) {
        return 1;
    }
    b->next = next;
    b->flags &= ~4;
    ConditionSetBit(a->flags, 0, 0);
    ConditionSetBit(a->flags, 4, 0);
    return 0;
}

// 0x801F8A6C: current state accessor.
int CrouchGetState(Actor* a) {
    if (a == 0) {
        return 0;
    }
    return a->beh->state;
}

// 0x801F8A88: update that only steps the movement.
int CrouchUpdateAdvance(Actor* a) {
    CrouchBeh* b = a->beh;
    if (a->unk5F0 == 0 || fn_80212730(a->handle) != -1) {
        CrouchAdvance(a);
    }
    return b->state;
}

// 0x801F8AE4: update for the idle state.
int CrouchUpdateIdle(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    fn_80162610(a, a->sm, 0x1B83833F);
    if (Fn800F7DEC(a->unk8EC, a->unk8F0, 0x20EE36, a->handle, 0) == -0x3F413082) {
        fn_80162610(a, a->sm, 0xE6B71BCC);
    } else if (Fn800F7DEC(a->unk8EC, a->unk8F0, 0xF829DB93, a->handle, 0) == -0x3F413082) {
        fn_80162610(a, a->sm, 0x968CBE3D);
    }
    return b->next;
}

// 0x801F8BB4: update for the run state.
int CrouchUpdateRun(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 4;
    }
    b = a->beh;
    CrouchAdvance(a);
    if ((b->flags & 1) != 0) {
        return 1;
    }
    if (HasCondition(a->flags, 9) != 0 && HasCondition(a->flags, 10) == 0 && HasCondition(a->flags, 0xB) == 0) {
        return 8;
    }
    if (HasCondition(a->flags, 4) == 0 || HasCondition(a->flags, 9) != 0 || HasCondition(a->flags, 10) != 0 ||
        b->timer2 > 0.0f) {
        if (MemFlagIsSet(a->flags, 3) == 0) {
            return 5;
        }
        if (Fn800F7DEC(a->unk8EC, a->unk8F0, 0x20EE36, a->handle, 0) == -0x3F413082 ||
            Fn800F7DEC(a->unk8EC, a->unk8F0, 0xF829DB93, a->handle, 0) == -0x3F413082) {
            return 5;
        }
    } else {
        ConditionSetBit(a->flags, 4, 0);
        fn_801BE5D4(a);
    }
    return b->state;
}

// 0x801F8D34: update for the cower state.
int CrouchUpdateCower(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (HasCondition(a->flags, 9) == 0 || HasCondition(a->flags, 10) != 0 || HasCondition(a->flags, 0xB) != 0) {
        if (HasCondition(a->flags, 4) != 0 && HasCondition(a->flags, 9) == 0 &&
            HasCondition(a->flags, 10) == 0 && b->state != 7) {
            if (b->timer2 <= 0.0f) {
                ConditionSetBit(a->flags, 4, 0);
                if (b->state != 0) {
                    fn_801BE5D4(a);
                }
            }
        }
        CrouchAdvance(a);
        return b->state;
    }
    return 8;
}

// 0x801F8E44: update for the wait states.
int CrouchUpdateWait(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 2;
    }
    b = a->beh;
    CrouchAdvance(a);
    if (HasCondition(a->flags, 9) != 0 && HasCondition(a->flags, 10) == 0 && HasCondition(a->flags, 0xB) == 0) {
        return 8;
    }
    if (HasCondition(a->flags, 4) == 0 || HasCondition(a->flags, 9) != 0 || HasCondition(a->flags, 10) != 0 ||
        b->timer2 > 0.0f) {
        if ((b->flags & 1) != 0) {
            return 1;
        }
        if (Fn800F7DEC(a->unk8EC, a->unk8F0, 0x20EE36, a->handle, 0) == -0x3F413082 ||
            Fn800F7DEC(a->unk8EC, a->unk8F0, 0xF829DB93, a->handle, 0) == -0x3F413082) {
            return 6;
        }
    } else {
        ConditionSetBit(a->flags, 4, 0);
    }
    if (MemFlagIsSet(a->flags, 3) != 0) {
        return 3;
    }
    return b->state;
}

// 0x801F8FBC: enter the dead state.
int CrouchEnterDead(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 9;
    b->next = 9;
    a->unk26C = 0;
    fn_8008DE4C(a->unkD4, 0, b->anim, 0x18);
    if (HasCondition(a->flags, 10) == 0) {
        ConditionSetBit(a->flags, 10, 1);
        ActorPlayAnim(a->handle, 0xDC04E3D5, a->handle);
    }
    return 1;
}

// 0x801F9058: enter the hit-reaction state.
int CrouchEnterHit(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 8;
    b->next = 9;
    a->unk26C = 0;
    b->timer = 0.0f;
    ConditionSetBit(a->flags, 0, 1);
    ConditionSetBit(a->flags, 9, 1);
    if (a->unk5F0 == 0 || fn_80212730(a->handle) != -1) {
        if ((b->flags & 8) == 0) {
            CrouchPlayAnim(a, 0);
        } else {
            CrouchPlayAnim(a, 1);
        }
    }
    return 1;
}

// 0x801F9388: enter the stagger state.
int CrouchEnterStagger(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->next = b->state;
    b->state = 7;
    b->timer2 = 3000.0f;
    a->unk26C = 0;
    ConditionSetBit(a->flags, 0, 1);
    ConditionSetBit(a->flags, 4, 1);
    CrouchPlayAnim(a, 0);
    return 1;
}

// 0x801F9550: enter the cower state.
bool CrouchEnterCower(Actor* a) {
    if (a != 0) {
        CrouchBeh* b = a->beh;
        b->next = b->state;
        b->state = 6;
        a->unk26C = 0;
        fn_80162610(a, a->sm, 3);
    }
    return a != 0;
}

// 0x801F95A8: enter state 5.
int CrouchEnterState5(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 5;
    b->next = 2;
    a->unk26C = 0;
    b->flags &= ~4;
    if ((b->flags & 8) == 0) {
        CrouchPlayAnim(a, 0);
    } else {
        CrouchPlayAnim(a, 1);
    }
    return 1;
}

// 0x801F98A0: enter state 4.
int CrouchEnterState4(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 4;
    b->next = 4;
    a->unk26C = 0;
    b->flags &= ~4;
    if ((b->flags & 8) == 0) {
        CrouchPlayAnim(a, 0);
    } else {
        CrouchPlayAnim(a, 1);
    }
    ConditionSetBit(a->flags, 8, 0);
    return 1;
}

// 0x801F9BA4: enter state 3.
int CrouchEnterState3(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 3;
    b->next = 4;
    a->unk26C = 0;
    b->flags &= ~4;
    b->flags |= 8;
    CrouchPlayAnim(a, 1);
    return 1;
}

// 0x801F9D58: enter state 2.
int CrouchEnterState2(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 2;
    b->next = 2;
    a->unk26C = 0;
    CrouchPlayAnim(a, 0);
    return 1;
}

// 0x801F9EF0: enter state 1.
int CrouchEnterState1(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 1;
    b->next = 2;
    a->unk26C = 0;
    CrouchPlayAnim(a, 0);
    return 1;
}

// 0x801FA08C: enter state 0.
int CrouchEnterState0(Actor* a) {
    CrouchBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 0;
    b->next = 2;
    a->unk26C = 0;
    CrouchPlayAnim(a, 0);
    return 1;
}

// 0x801FA224: per-frame movement/animation step shared by all states.
void CrouchAdvance(Actor* a) {
    CrouchBeh* b = a->beh;
    if (a->pose == 0) {
        return;
    }
    {
        float d[3];
        Mtx34 mInv;
        Mtx34 mRot;
        Mtx34 mBone;
        Mtx34 mPlace;
        Mtx34 mTmp;
        Vec3 e;
        int msgbuf[8];
        fn_800913EC(a->dt, a->unkD4, d);
        AnimApply(a->unkD4, a->pose, ModelAnimate(a->model, (char*)a->pose, 1), a->unkD0);
        {
            int bone1 = ModelFindBone(a->model, AssetFind(0xF1754BF2, 0x922BF04E));
            int bone2 = ModelFindBone(a->model, AssetFind(0xF1754BF2, 0x21C5E0));
            SplineQuatRotateRamp(a->rot[0], a->pose, bone1, bone2, *(u32*)a->info);
        }
        if (HasCondition(a->flags, 0xD) == 0 && HasCondition(a->flags, 0xC) == 0) {
            if ((b->flags & 4) == 0) {
                float lim = a->info->unk08;
                float r = a->rot[0];
                float lo = -lim;
                if (lo <= r) {
                    lo = r;
                    if (lim < r) {
                        lo = lim;
                    }
                }
                a->rot[0] = lo;
                if (HasCondition(a->flags, 0) == 0) {
                    float spd;
                    float side;
                    float step;
                    if (-999999.0f != b->ground) {
                        a->p84[2] = b->ground + a->pose->pos[2];
                    }
                    a->pos[2] = (int)a->p84[2];
                    a->pose->pos[0] = 0.0f;
                    a->pose->pos[1] = 0.0f;
                    a->pose->pos[2] = 0.0f;
                    spd = a->speed;
                    side = a->unk27C;
                    step = a->info->unkC4;
                    if (spd > 1.1920929e-07f) {
                        float t = (step * a->dt) / 1000.0f;
                        if (t < spd) {
                            spd = t;
                        }
                        a->rot[2] = a->rot[2] + spd;
                    } else if (spd < -1.1920929e-07f) {
                        float t = -((step * a->dt) / 1000.0f);
                        if (spd < t) {
                            spd = t;
                        }
                        a->rot[2] = a->rot[2] + spd;
                    }
                    if (side > 1.1920929e-07f) {
                        float t = (step * a->dt) / 1000.0f;
                        if (t < side) {
                            side = t;
                        }
                        a->rot[0] = a->rot[0] + side;
                    } else if (side < -1.1920929e-07f) {
                        float t = -((step * a->dt) / 1000.0f);
                        if (side < t) {
                            side = t;
                        }
                        a->rot[0] = a->rot[0] + side;
                    }
                    EulerNormalize((Vec3*)a->rot, (Vec3*)a->rot);
                    Vec3Set(&e, a->rot[2], 0.0f, 0.0f);
                    EulerNormalize(&e, &e);
                    MtxSetEuler(&mBone, &e);
                    PlacementToMatrix(a, &mBone);
                    ActorGetMatrix(&mInv, (int)a);
                    fn_800A0168(&mInv, &mInv);
                    d[1] = 0.0f;
                    d[2] = 0.0f;
                    fn_800A0030(d, &mInv, d);
                    ModelSetMatrix(a->model, &mBone, a->pose);
                } else {
                    if (-999999.0f != b->ground) {
                        a->p84[2] = b->ground + a->pose->pos[2];
                    }
                    a->pos[2] = (int)a->p84[2];
                    a->pose->pos[0] = 0.0f;
                    a->pose->pos[1] = 0.0f;
                    a->pose->pos[2] = 0.0f;
                    ActorGetMatrix(&mInv, (int)a);
                    fn_800A0168(&mInv, &mInv);
                    fn_800A0030(d, &mInv, d);
                    if (a->dt <= 0.0f) {
                        a->vel[0] = 0.0f;
                        a->vel[1] = 0.0f;
                        a->vel[2] = 0.0f;
                    } else {
                        float inv;
                        a->vel[0] = d[0];
                        a->vel[1] = d[1];
                        a->vel[2] = d[2];
                        a->vel[2] = 0.0f;
                        inv = 1000.0f / a->dt;
                        a->vel[0] = a->vel[0] * inv;
                        a->vel[1] = a->vel[1] * inv;
                        a->vel[2] = a->vel[2] * inv;
                    }
                    Vec3Set(&e, a->rot[2], 0.0f, 0.0f);
                    EulerNormalize(&e, &e);
                    MtxSetEuler(&mBone, &e);
                    PlacementToMatrix(a, &mBone);
                    ModelSetMatrix(a->model, &mBone, a->pose);
                }
            } else {
                float lim = a->info->unk08;
                float r = a->rot[0];
                float lo = -lim;
                if (lo <= r) {
                    lo = r;
                    if (lim < r) {
                        lo = lim;
                    }
                }
                a->rot[0] = lo;
                if (a->unk608 == -1 || SendMsg(a->unk608, 0x400, msgbuf) == 0) {
                    ActorGetMatrix(&mPlace, (int)a);
                } else {
                    ActorGetMatrix(&mPlace, msgbuf[0]);
                }
                fn_800A0168(&mInv, &mPlace);
                fn_800A0030(d, &mInv, a->pose->pos);
                a->p84[0] = mPlace.m[3] + d[0];
                a->p84[1] = mPlace.m[7] + d[1];
                a->p84[2] = mPlace.m[11] + d[2];
                if (-999999.0f != b->ground) {
                    a->p84[2] = b->ground + a->pose->pos[2];
                }
                a->pos[0] = (int)a->p84[0];
                a->pos[1] = (int)a->p84[1];
                a->pos[2] = (int)a->p84[2];
                BoneGetMatrix(&mBone, a->pose);
                fn_8009FC6C(&mBone, &mBone, &mPlace);
                MtxGetEuler((Vec3*)a->rot, &mBone);
                a->rot[2] = a->rot[2] - 90.0f;
                EulerNormalize((Vec3*)a->rot, (Vec3*)a->rot);
                MtxSetEuler(&mRot, (Vec3*)a->rot);
                PlacementToMatrix(a, &mRot);
                a->pose->pos[0] = 0.0f;
                a->pose->pos[1] = 0.0f;
                a->pose->pos[2] = 0.0f;
                ModelSetMatrix(a->model, &mBone, a->pose);
            }
        } else if (HasCondition(a->flags, 0xC) == 0) {
            fn_800913EC(a->dt, a->unkD4, d);
            if (-999999.0f != b->ground) {
                a->p84[2] = b->ground + a->pose->pos[2];
                a->pose->pos[2] = 0.0f;
            }
            a->pos[2] = (int)a->p84[2];
            ActorGetMatrix(&mInv, (int)a);
            fn_800A0168(&mInv, &mInv);
            fn_800A0030(d, &mInv, d);
            if (a->dt <= 0.0f) {
                a->vel[0] = 0.0f;
                a->vel[1] = 0.0f;
                a->vel[2] = 0.0f;
            } else {
                float inv;
                a->vel[0] = d[0];
                a->vel[1] = d[1];
                a->vel[2] = d[2];
                a->vel[2] = 0.0f;
                inv = 1000.0f / a->dt;
                a->vel[0] = a->vel[0] * inv;
                a->vel[1] = a->vel[1] * inv;
                a->vel[2] = a->vel[2] * inv;
            }
            Vec3Set(&e, a->rot[2], 0.0f, 0.0f);
            EulerNormalize(&e, &e);
            MtxSetEuler(&mBone, &e);
            PlacementToMatrix(a, &mBone);
            ModelSetMatrix(a->model, &mBone, a->pose);
        } else {
            if (fn_8008EA9C(a->unkD4, 0) != 0) {
                if (HasCondition(a->flags, 0xC) == 0) {
                    Vec3Set(&e, a->rot[2], 0.0f, 0.0f);
                    EulerNormalize(&e, &e);
                    MtxSetEuler(&mBone, &e);
                    PlacementToMatrix(a, &mBone);
                } else {
                    BoneGetMatrix(&mBone, a->pose);
                    MtxGetEuler((Vec3*)a->rot, &mBone);
                    PlacementToMatrix(a, &mBone);
                    a->p84[0] = a->pose->pos[0];
                    a->p84[1] = a->pose->pos[1];
                    a->p84[2] = a->pose->pos[2];
                    a->pos[0] = (int)a->pose->pos[0];
                    a->pos[1] = (int)a->pose->pos[1];
                    a->pos[2] = (int)a->pose->pos[2];
                }
            }
            ModelSetMatrix(a->model, 0, a->pose);
        }
    }
}

extern int sCrouchFlag;     // 0x804D3364
extern int sCrouchTexture;  // 0x804D3350
extern int sCrouchMode;     // 0x804D3354
extern int sCrouchAngA;     // 0x804D52B8
extern int sCrouchAngB;     // 0x804D52BC
extern int sCrouchAngC;     // 0x804D52C0
extern "C" {
void fn_800F1138(int a, float* v, void (*cb)(Actor*));
void fn_8000F874(void);
}
void CrouchDrawA(Actor* a);
void CrouchDrawB(Actor* a);
void CrouchDrawC(Actor* a);
void CrouchDrawD(Actor* a);

// 0x801FAD18: spawn the effect of the given kind at the actor position.
void CrouchSpawnFx(Actor* a, int kind) {
    float v[3];
    v[0] = (float)a->pos[0];
    v[1] = (float)a->pos[1];
    v[2] = (float)a->pos[2];
    if (kind == 2) {
        fn_800F1138(sCrouchMode, v, CrouchDrawB);
    } else {
        if (kind < 2) {
            if (0 < kind) {
                fn_800F1138(sCrouchMode, v, CrouchDrawA);
                return;
            }
        } else if (kind < 4) {
            fn_800F1138(sCrouchMode, v, CrouchDrawC);
            return;
        }
        fn_800F1138(sCrouchMode, v, CrouchDrawD);
    }
}

// 0x801FAE14: set the effect mode flag.
void CrouchSetFlag(int v) { sCrouchFlag = v; }

// 0x801FAE1C: empty.
void CrouchNop(void) {}

// 0x801FAE20: advance the three effect phase counters.
void CrouchAdvancePhases(float dt) {
    float f = dt * 1.75f;
    sCrouchAngA = sCrouchAngA + (int)f;
    sCrouchAngB = sCrouchAngB + (int)(0.9f * f);
    sCrouchAngC = sCrouchAngC + (int)(0.8f * f);
}

// 0x801FAE8C: load the effect texture.
void CrouchInit(void) {
    sCrouchTexture = AssetFind(0xF2E2BEB8, 0xFA52DE75);
    if (sCrouchTexture != -1) {
        fn_8000F874();
        sCrouchMode = 3;
    }
}

// 0x801FAED4: reset the phase counters.
void CrouchResetPhases(void) {
    sCrouchAngA = 0;
    sCrouchAngB = 0;
    sCrouchAngC = 0;
}

struct DrawColors {
    u8 c[3];
};
extern DrawColors gCrouchColorsA;    // 0x803B9DD8
extern DrawColors gCrouchColorsB;    // 0x803B9DF8
extern DrawColors gCrouchColorsC;    // 0x803B9E18
extern u8 sCrouchRgbA[3];            // 0x804D52C8
extern u8 sCrouchRgbB[3];            // 0x804D52CB
void CrouchDrawGeneric(Actor* a, DrawColors* colors);

// 0x801FB2A8: draw with the third colour set.
void CrouchDrawC(Actor* a) {
    CrouchDrawGeneric(a, &gCrouchColorsB);
}

// 0x801FB2D0: draw with the first colour set.
void CrouchDrawB(Actor* a) {
    CrouchDrawGeneric(a, &gCrouchColorsA);
}

// 0x801FB2F8: draw with the second colour set.
void CrouchDrawD(Actor* a) {
    CrouchDrawGeneric(a, &gCrouchColorsC);
}

// 0x801FC5F4: copy the base colours.
void CrouchLoadColors(void) {
    sCrouchRgbA[0] = gCrouchColorsA.c[0];
    sCrouchRgbA[1] = gCrouchColorsA.c[1];
    sCrouchRgbA[2] = gCrouchColorsA.c[2];
    sCrouchRgbB[0] = gCrouchColorsB.c[0];
    sCrouchRgbB[1] = gCrouchColorsB.c[1];
    sCrouchRgbB[2] = gCrouchColorsB.c[2];
}

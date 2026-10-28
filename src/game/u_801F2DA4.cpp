// u_801F2DA4.cpp: fear behaviour of a possessed/AI actor (0x801F2DA4-0x801F676C).
// An actor with a ten-state machine (idle, run away, cower, corner ...) driven from a state table of
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

struct FearBeh;

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
    char pad0C[0x160 - 0x0C];
    u32 animSetB;           // 0x160
    u32 animSetA;           // 0x164
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
    FearBeh* beh;           // 0x134
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

extern StateDef gFearStates[10];

// Behaviour block (VAlloc'd, 0x68 bytes).
struct FearBeh {
    u32 animSet;            // 0x00
    u32 hash;               // 0x04
    int state;              // 0x08
    int next;               // 0x0C
    int anim;               // 0x10
    int unk14;              // 0x14
    int unk18;              // 0x18
    float timer;            // 0x1C
    float timer2;           // 0x20
    float unk24;            // 0x24
    float pos28[4];         // 0x28
    char pad38[0x4C - 0x38];
    float vec4C[3];         // 0x4C
    float unk58;            // 0x58
    u32 flags;              // 0x5C
    int pendingMsg;         // 0x60
    void* pendingData;      // 0x64
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
extern int SendMsgOut(int to, int msg, void* data);
extern int sFearUnk;      // 0x804D3338
extern int sFearBone;     // 0x804D333C

int FearUpdate(Actor* a);
void FearAdvance(Actor* a);

// Start an animation of the current state's set and blend to it.
inline void FearPlayAnim(Actor* a, int variant, int mode) {
    FearBeh* b;
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
    if (variant < 0) {
        int count = fn_800A1AC4(b->hash, b->animSet, gFearStates[b->state].hash);
        variant = rand();
        variant = variant - (variant / count) * count;
    }
    anim = fn_800A1BC8(b->hash, b->animSet, gFearStates[b->state].hash, variant, 0);
    if (fn_8008E7D0() != 0) {
        ConditionSetBit(a->flags, 0x1C, 0);
        fn_80090974(1.0f, a->unkD4, 0, anim, mode, (int (*)(int, Actor*))gFearStates[b->state].handler, a);
        if (HasCondition(a->flags, 0xE) != 0 || b->anim == anim) {
            fn_8008F230(0.0f, a->unkD4, 0, 0);
            ConditionSetBit(a->flags, 0xE, 0);
        } else if (b->state == 0) {
            fn_8008F230(350.0f, a->unkD4, 0, 1);
        } else {
            fn_8008F230(150.0f, a->unkD4, 0, 1);
        }
        b->anim = anim;
    }
}

inline int RandBit(void) {
    return rand() % 2;
}

// 0x801F2DA4: actor message handler.
int FearMsg(int msg, void* p) {
    ActorMsg* m = (ActorMsg*)p;
    Actor* a;
    FearBeh* b;
    switch (msg) {
    case 0:
        return fn_800CBAA0(m, 0, 0);
    case 10:
        return fn_800CBAA0(m, 0, 1);
    case 0x7300:
        return fn_800CBAA0(m, 1, 0);
    case 0x41E:
        if (m->unk04 == -0x527A8906) {
            String s1(gFearStates[m->actor->beh->state].name);
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

// 0x801F3184: per-frame update.
int FearUpdate(Actor* a) {
    FearBeh* b;
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
         HasCondition(a->flags, 10) == 0 && b->state != 9 && b->state != 10)) {
        gFearStates[b->state].enter(a);
    }
    if (a->unk220 == 4) {
        FearAdvance(a);
        next = b->state;
    } else {
        next = gFearStates[b->state].update(a);
    }
    if (next != b->state) {
        if (gFearStates[b->state].canLeave(a, next) != 0) {
            gFearStates[next].enter(a);
        }
    }
    if ((b->flags & 2) != 0) {
        int msg = b->pendingMsg;
        void* data = b->pendingData;
        b->pendingMsg = -1;
        b->pendingData = 0;
        a->unk2B0 = 0.0f;
        a->unk258 = 0.0f;
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

// 0x801F33FC: destroy.
void FearDestroy(Actor* a) {
    FearBeh* b = a->beh;
    AnimBankUnref(b->hash, b->animSet);
    a->update = 0;
    if (b->pendingMsg != -1) {
        b->pendingMsg = -1;
        VFree(b->pendingData);
    }
    VFree(a->beh);
    a->beh = 0;
}

// 0x801F3470: create (animation set from the actor info, variant A).
void FearCreateA(int unused, Actor* a) {
    FearBeh* b;
    if (a != 0) {
        a->update = FearUpdate;
        a->unk118 = fn_800CDD10;
        b = (FearBeh*)VAlloc(0x68);
        if (b == 0) {
            a->beh = 0;
        } else {
            u32 h = 0;
            const char* name = "FearAI_AnimWayCorner";
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
            a->unk918 = -999999.0f;
            b->unk58 = fn_800CC2D0(a, 1, &b->unk14, &b->unk18);
            sFearUnk = AssetFind(0xF2E2BEB8, 0x154CDE5C);
            sFearBone = AssetFind(0xF1754BF2, 0x21C5E0);
            fn_80090634(a->unkD4, 0, 0, 0);
            ConditionSetBit(a->flags, 0x1C, 1);
            ConditionSetBit(a->flags, 1, 1);
        }
    }
    b = a->beh;
    if (a != 0 && b != 0) {
        b->animSet = a->info->animSetA;
        AnimBankActivate(b->hash, b->animSet);
    }
}

// 0x801F3678: create (variant B).
void FearCreateB(int unused, Actor* a) {
    FearBeh* b;
    if (a != 0) {
        a->update = FearUpdate;
        a->unk118 = fn_800CDD10;
        b = (FearBeh*)VAlloc(0x68);
        if (b == 0) {
            a->beh = 0;
        } else {
            u32 h = 0;
            const char* name = "FearAI_AnimWayCorner";
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
            a->unk918 = -999999.0f;
            b->unk58 = fn_800CC2D0(a, 1, &b->unk14, &b->unk18);
            sFearUnk = AssetFind(0xF2E2BEB8, 0x154CDE5C);
            sFearBone = AssetFind(0xF1754BF2, 0x21C5E0);
            fn_80090634(a->unkD4, 0, 0, 0);
            ConditionSetBit(a->flags, 0x1C, 1);
            ConditionSetBit(a->flags, 1, 1);
        }
    }
    b = a->beh;
    if (a != 0 && b != 0) {
        b->animSet = a->info->animSetB;
        AnimBankActivate(b->hash, b->animSet);
    }
}

// 0x801F3880: state-machine handler A (arms the finish flag on the end-of-state message).
int FearSmHandlerA(int msg, Actor* a) {
    if (a == 0) {
        return 0;
    }
    fn_80162610(a, a->sm, msg);
    if (msg == 0x1B83833F) {
        a->beh->flags |= 2;
        a->unk5E4 = 0.0f;
    }
    return 1;
}

// 0x801F3904: state-machine handler B (switches state on the end-of-state message).
int FearSmHandlerB(int msg, Actor* a) {
    if (a == 0) {
        return 0;
    }
    fn_80162610(a, a->sm, msg);
    if (msg == 0x1B83833F) {
        FearBeh* b = a->beh;
        gFearStates[b->state].canLeave(a, b->next);
        gFearStates[b->next].enter(a);
    }
    return 1;
}

// 0x801F39C8: leave callback that only clears the pose bits.
int FearLeaveClear(Actor* a) {
    if (a == 0) {
        return 0;
    }
    ConditionSetBit(a->flags, 0, 0);
    ConditionSetBit(a->flags, 4, 0);
    return 0;
}

// 0x801F3A20: leave callback (cower).
int FearLeaveCower(Actor* a, int next) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->next = next;
    b->flags &= ~4;
    if (b->state == 7) {
        fn_80162610(a, a->sm, 0);
    }
    return 1;
}

// 0x801F3A7C: leave callback (default).
int FearLeaveDefault(Actor* a, int next) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (next == 9 || next == 10) {
        return 1;
    }
    b->next = next;
    b->flags &= ~4;
    ConditionSetBit(a->flags, 0, 0);
    ConditionSetBit(a->flags, 4, 0);
    return 0;
}

// 0x801F3B00: current state accessor.
int FearGetState(Actor* a) {
    if (a == 0) {
        return 0;
    }
    return a->beh->state;
}

// 0x801F3B1C: update that only steps the movement.
int FearUpdateAdvance(Actor* a) {
    FearBeh* b = a->beh;
    if (a->unk5F0 == 0 || fn_80212730(a->handle) != -1) {
        FearAdvance(a);
    }
    return b->state;
}

// 0x801F3B78: update for the idle state (checks whether a message is queued).
int FearUpdateIdle(Actor* a) {
    FearBeh* b;
    int r;
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

// 0x801F3C48: update for the run states.
int FearUpdateRun(Actor* a) {
    FearBeh* b;
    int next;
    if (a == 0) {
        return 5;
    }
    b = a->beh;
    FearAdvance(a);
    if ((b->flags & 1) != 0 && (b->flags & 8) == 0) {
        b->flags |= 2;
    }
    if (HasCondition(a->flags, 9) == 0 || HasCondition(a->flags, 10) != 0 || HasCondition(a->flags, 0xB) != 0) {
        if (HasCondition(a->flags, 4) != 0 && HasCondition(a->flags, 9) == 0 &&
            HasCondition(a->flags, 10) == 0) {
            if (b->timer2 <= 0.0f) {
                ConditionSetBit(a->flags, 4, 0);
                fn_801BE5D4(a);
            }
        }
        if (Fn800F7DEC(a->unk8EC, a->unk8F0, 0x20EE36, a->handle, 0) == -0x3F413082 ||
            Fn800F7DEC(a->unk8EC, a->unk8F0, 0xF829DB93, a->handle, 0) == -0x3F413082) {
            next = 7;
        } else if (MemFlagIsSet(a->flags, 3) == 0) {
            next = 6;
        } else {
            next = b->state;
        }
    } else {
        next = 9;
    }
    return next;
}

// 0x801F3DCC: update for the cower state.
int FearUpdateCower(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (HasCondition(a->flags, 9) == 0 || HasCondition(a->flags, 10) != 0 || HasCondition(a->flags, 0xB) != 0) {
        if (HasCondition(a->flags, 4) != 0 && HasCondition(a->flags, 9) == 0 &&
            HasCondition(a->flags, 10) == 0 && b->state != 8) {
            if (b->timer2 <= 0.0f) {
                ConditionSetBit(a->flags, 4, 0);
                fn_801BE5D4(a);
            }
        }
        FearAdvance(a);
        return b->state;
    }
    return 9;
}

// 0x801F3ED0: update for the idle states.
int FearUpdateWait(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 2;
    }
    b = a->beh;
    FearAdvance(a);
    if (HasCondition(a->flags, 9) == 0 || HasCondition(a->flags, 10) != 0 || HasCondition(a->flags, 0xB) != 0) {
        if (HasCondition(a->flags, 4) != 0 && HasCondition(a->flags, 9) == 0 &&
            HasCondition(a->flags, 10) == 0 && b->timer2 <= 0.0f) {
            ConditionSetBit(a->flags, 4, 0);
            fn_801BE5D4(a);
        }
        if (Fn800F7DEC(a->unk8EC, a->unk8F0, 0x20EE36, a->handle, 0) != -0x3F413082 &&
            Fn800F7DEC(a->unk8EC, a->unk8F0, 0xF829DB93, a->handle, 0) != -0x3F413082) {
            if ((b->flags & 1) != 0) {
                return 1;
            }
            if (b->state != 3) {
                if (HasCondition(a->flags, 0x1A) == 0) {
                    b->timer -= a->dt;
                    if (b->timer <= 0.0f) {
                        return 3;
                    }
                }
                if (MemFlagIsSet(a->flags, 3) != 0) {
                    return 4;
                }
            }
            return b->state;
        }
        return 7;
    }
    return 9;
}

// 0x801F4098: enter the dead state.
int FearEnterDead(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 10;
    b->next = 10;
    a->unk26C = 0;
    fn_8008DE4C(a->unkD4, 0, b->anim, 0x18);
    if (HasCondition(a->flags, 10) == 0) {
        ConditionSetBit(a->flags, 10, 1);
        ActorPlayAnim(a->handle, 0xDC04E3D5, a->handle);
    }
    return 1;
}

// 0x801F4134: enter the hit-reaction state.
int FearEnterHit(Actor* a) {
    FearBeh* b;
    int prev;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    prev = b->state;
    b->state = 9;
    b->next = 10;
    a->unk26C = 0;
    b->timer = 0.0f;
    ConditionSetBit(a->flags, 9, 1);
    if (a->unk5F0 == 0 || fn_80212730(a->handle) != -1) {
        b->flags |= 4;
        if (prev - 4U < 2 || prev == 6) {
            if ((b->flags & 8) == 0) {
                FearPlayAnim(a, 2, 9);
            } else {
                FearPlayAnim(a, 3, 9);
            }
        } else {
            FearPlayAnim(a, RandBit(), 9);
        }
    }
    return 1;
}

// 0x801F4674: enter the stagger state.
int FearEnterStagger(Actor* a) {
    FearBeh* b;
    int prev;
    int variant;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    prev = b->state;
    if (prev == 4 || prev == 5 || prev == 6) {
        if ((b->flags & 8) == 0) {
            variant = 2;
        } else {
            variant = 3;
        }
    } else {
        variant = RandBit();
    }
    b->next = b->state;
    b->state = 8;
    b->timer2 = 3000.0f;
    a->unk26C = 0;
    ConditionSetBit(a->flags, 4, 1);
    b->flags |= 4;
    FearPlayAnim(a, variant, 9);
    return 1;
}

// 0x801F48E4: enter the cower state.
int FearEnterCower(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (b->state != 7) {
        b->next = b->state;
    }
    b->state = 7;
    a->unk26C = 0;
    fn_80162610(a, a->sm, 3);
    return 1;
}

// 0x801F4944: enter state 6.
int FearEnterState6(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 6;
    b->next = 2;
    a->unk26C = 0;
    b->flags |= 4;
    if ((b->flags & 8) == 0) {
        FearPlayAnim(a, 0, 9);
    } else {
        FearPlayAnim(a, 1, 9);
    }
    return 1;
}

// 0x801F4C84: enter state 5.
int FearEnterState5(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 5;
    b->next = 5;
    a->unk26C = 0;
    b->flags |= 4;
    if ((b->flags & 8) == 0) {
        FearPlayAnim(a, 0, 9);
    } else {
        FearPlayAnim(a, 1, 9);
    }
    ConditionSetBit(a->flags, 8, 0);
    return 1;
}

// 0x801F4FD0: enter state 4.
int FearEnterState4(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 4;
    b->next = 5;
    a->unk26C = 0;
    b->flags |= 4;
    b->flags &= ~8;
    FearPlayAnim(a, 0, 9);
    return 1;
}

// 0x801F51A8: enter state 3.
int FearEnterState3(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 3;
    b->next = 2;
    a->unk26C = 0;
    b->timer = 3000.0f * ((float)rand() / 32767.0f) + 2000.0f;
    b->flags |= 4;
    FearPlayAnim(a, 0, 9);
    return 1;
}

// 0x801F53B0: enter state 2.
int FearEnterState2(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 2;
    b->next = 2;
    a->unk26C = 0;
    b->flags |= 4;
    FearPlayAnim(a, 0, 9);
    return 1;
}

// 0x801F5578: enter state 1.
int FearEnterState1(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 1;
    b->next = 2;
    a->unk26C = 0;
    b->flags |= 4;
    FearPlayAnim(a, 0, 0x18);
    return 1;
}

// 0x801F5744: enter state 0.
int FearEnterState0(Actor* a) {
    FearBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    b->state = 0;
    b->next = 2;
    a->unk26C = 0;
    b->flags |= 4;
    b->unk24 = 0.0f;
    if (a->unk608 != -1) {
        int msgbuf[8];
        if (SendMsg(a->unk608, 0x400, msgbuf) != 0) {
            Mtx34 m;
            ActorGetMatrix(&m, msgbuf[0]);
            BoneWorldMatrix(b->pos28, a->pose, a->pose->pos);
        }
    }
    FearPlayAnim(a, 0, 9);
    return 1;
}

// 0x801F5954: per-frame movement/animation step shared by all states.
void FearAdvance(Actor* a) {
    FearBeh* b = a->beh;
    if (a->pose == 0) {
        return;
    }
    {
        float d[3];
        Mtx34 mTmp;
        Mtx34 mInv;
        Mtx34 mRot;
        Mtx34 mBone;
        Mtx34 mPlace;
        Vec3 e;
        float qa[4];
        float qb[4];
        int msgbuf[8];
        fn_800913EC(a->dt, a->unkD4, d);
        AnimApply(a->unkD4, a->pose, ModelAnimate(a->model, (char*)a->pose, 1), a->unkD0);
        fn_801BE4B8(a);
        if (HasCondition(a->flags, 0xD) == 0 && HasCondition(a->flags, 0xC) == 0) {
            if (b->state == 0) {
                float lim = a->info->unk08;
                float r = a->rot[0];
                float lo = -lim;
                double t;
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
                t = b->unk24 / 350.0f;
                if (t < 0.0) {
                    t = 0.0;
                } else if (t > 1.0) {
                    t = 1.0;
                }
                b->unk24 = b->unk24 + a->dt;
                fn_800A0168(&mInv, &mPlace);
                fn_800A0030(&e.x, &mInv, a->pose->pos);
                a->p84[0] = (float)((e.x - b->vec4C[0]) * t + b->vec4C[0]);
                a->p84[1] = (float)((e.y - b->vec4C[1]) * t + b->vec4C[1]);
                a->p84[2] = (float)((e.z - b->vec4C[2]) * t + b->vec4C[2]);
                a->pos[0] = (int)a->p84[0];
                a->pos[1] = (int)a->p84[1];
                a->pos[2] = (int)a->p84[2];
                if (-999999.0f != b->unk58) {
                    a->p84[2] = b->unk58 + a->pose->pos[2];
                }
                a->pos[0] = (int)a->p84[0];
                a->pos[1] = (int)a->p84[1];
                a->pos[2] = (int)a->p84[2];
                a->pose->pos[0] = a->p84[0];
                a->pose->pos[1] = a->p84[1];
                a->pose->pos[2] = a->p84[2];
                BoneSetMatrix(qa, b->pos28);
                BoneSetMatrix(qb, (float*)&mPlace);
                PSQUATMultiply(a->pose, qb, qb);
                C_QUATSlerp((float)t, qa, qb, a->pose);
                BoneGetMatrix(&mBone, a->pose);
                MtxGetEuler((Vec3*)a->rot, &mBone);
                a->rot[2] = a->rot[2] - 90.0f;
                EulerNormalize((Vec3*)a->rot, (Vec3*)a->rot);
                MtxSetEuler(&mRot, (Vec3*)a->rot);
                PlacementToMatrix(a, &mRot);
                LinkInfoInit(&mBone);
            } else if ((b->flags & 4) == 0) {
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
                if (-999999.0f != b->unk58) {
                    a->p84[2] = b->unk58 + a->pose->pos[2];
                }
                a->pos[2] = (int)a->p84[2];
                a->pose->pos[0] = 0.0f;
                a->pose->pos[1] = 0.0f;
                a->pose->pos[2] = 0.0f;
                EulerNormalize((Vec3*)a->rot, (Vec3*)a->rot);
                Vec3Set(&e, a->rot[2], 0.0f, 0.0f);
                EulerNormalize(&e, &e);
                MtxSetEuler(&mBone, &e);
                PlacementToMatrix(a, &mBone);
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
                if (-999999.0f != b->unk58) {
                    a->p84[2] = b->unk58 + a->pose->pos[2];
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
            }
            {
                int bone1 = ModelFindBone(a->model, AssetFind(0xF1754BF2, 0x8C73DFA9));
                int bone2 = ModelFindBone(a->model, AssetFind(0xF1754BF2, 0x21C5E0));
                Vec3 target;
                if (MemFlagIsSet(a->flags, 0x16) == 0 || FollowGetTargetPos(a->flags, &target) == 0) {
                    fn_800CA1B0(a, bone1, bone2, &mBone, 0);
                } else {
                    fn_800CA1B0(a, bone1, bone2, &mBone, &target);
                }
                ModelSetMatrix(a->model, &mBone, a->pose);
            }
        } else if (HasCondition(a->flags, 0xC) == 0) {
            if (-999999.0f != b->unk58) {
                a->p84[2] = b->unk58 + a->pose->pos[2];
                a->pose->pos[2] = 0.0f;
            }
            a->pos[2] = (int)a->p84[2];
            ActorGetMatrix(&mInv, (int)a);
            fn_800A0168(&mInv, &mInv);
            fn_800A0030(d, &mInv, d);
            a->vel[0] = d[0];
            a->vel[1] = d[1];
            a->vel[2] = d[2];
            a->vel[2] = 0.0f;
            {
                float inv = 1000.0f / a->dt;
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

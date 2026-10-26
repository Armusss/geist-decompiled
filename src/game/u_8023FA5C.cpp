// u_8023FA5C.cpp: "large actor" states, aiming and follow helpers (0x8023FA5C-0x80243B0C).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long ulong;

struct Vec3 {
    float x;  // 0x00
    float y;  // 0x04
    float z;  // 0x08
};

struct Vec4 {
    float x;  // 0x00
    float y;  // 0x04
    float z;  // 0x08
    float w;  // 0x0C
};

struct Mtx34;
struct Place;

// Reference counted string base; only its destructor is used in this unit.
class Buffer {
public:
    ~Buffer();
    u8 mUnk00[0x14];  // 0x00 (first word is the vtable pointer)
};

class String : public Buffer {
public:
    String();
    String(const char* text);
};

class Skin;

struct Actor;

// One entry of the actor behaviour table at lbl_803BE590 (stride 0x18).
struct BehaviourState {
    int mUnk00;                       // 0x00
    void (*mEnter)(Actor*);           // 0x04
    int (*mCanChange)(Actor*, int);   // 0x08
    int (*mGetNext)(Actor*);          // 0x0C
    int (*mUnk10)(int, void*);        // 0x10 (display-list / message callback)
    const char* mName;                // 0x14
};

// Per actor behaviour instance, pointed to from Actor at 0x134.
struct Behaviour {
    void* mOwner;     // 0x00
    int mUnk04;       // 0x04
    int mState;       // 0x08
    int mNextState;   // 0x0C
    float mTime10;    // 0x10
    float mTime14;    // 0x14
    float mTime18;    // 0x18
    int mUnk1C;       // 0x1C
    u16 mFlags;       // 0x20
    short mPad22;     // 0x22 (signed block countdown)
    int mFxHandle;    // 0x24
    float mFxTimer;   // 0x28
};

// Object pointed to from Actor at 0xC4; only the word at 0x30 is used here.
struct ActorC4Data {
    u8 mUnk00[0x14];          // 0x00
    int mUnk14;               // 0x14
    u8 mUnk18[0x1C - 0x18];   // 0x18
    int mUnk1C;               // 0x1C
    u8 mUnk20[0x30 - 0x20];   // 0x20
    u32 mValue30;             // 0x30
};

struct Actor {
    u8 mUnk00[0x24];          // 0x00
    u8 mMsgBlock24[0x38 - 0x24];  // 0x24 (address passed to fn_8016F120)
    int mModel;               // 0x38
    u8 mUnk3C[0x84 - 0x3C];   // 0x3C
    Vec3 mPos;                // 0x84
    u8 mUnk90[0xC4 - 0x90];   // 0x90
    ActorC4Data* mDataC4;     // 0xC4
    u8 mUnkC8[0xCC - 0xC8];   // 0xC8
    int mSkin;                // 0xCC
    int mUnkD0;               // 0xD0
    int mUnkD4;               // 0xD4
    u8 mUnkD8[0xF0 - 0xD8];   // 0xD8
    int mUnkF0;               // 0xF0
    u8 mUnkF4[0x110 - 0xF4];         // 0xF4
    int (*mUpdateBehaviour)(void*);  // 0x110
    u8 mUnk114[0x118 - 0x114];       // 0x114
    void (*mRenderCallback)(void*);  // 0x118
    u8 mUnk11C[0x134 - 0x11C];       // 0x11C
    Behaviour* mBehaviour;           // 0x134
    float mDeltaTime;         // 0x138
    float mTimer;             // 0x13C
    u8 mUnk140[0x214 - 0x140];// 0x140
    int mUnk214;              // 0x214 (last bone hit by an event)
    u8 mUnk218[0x240 - 0x218];// 0x218
    Vec3 mVec240;             // 0x240
    Vec3 mAngles;             // 0x24C (euler angles; 0x254 is its Z)
    u8 mUnk258[0x26C - 0x258];// 0x258
    int mUnk26C;              // 0x26C
    u8 mCondition[8];         // 0x270 (condition owner for HasCondition)
    u8 mUnk278[0x27C - 0x278];// 0x278
    u8 mUnk27C[0x284 - 0x27C];// 0x27C
    float mUnk284;            // 0x284
    u8 mUnk288[0x894 - 0x288];// 0x288 (follow offset object passed to fn_8016D6A8)
    u8 mStateMachine[0x8F8 - 0x894]; // 0x894 (state object passed to fn_8016325C)
    int mMode;                // 0x8F8
    float mUnk8FC;            // 0x8FC
    u8 mUnk900[0x904 - 0x900];// 0x900
    float mUnk904;            // 0x904
};

// Generic actor message payload (the message handler's second argument).
struct MsgData {
    Actor* mActor;  // 0x00
    u32 mHash;      // 0x04
    int mOut0;      // 0x08
    int mOut1;      // 0x0C
    int mVec[3];    // 0x10, 0x14, 0x18
};

// 0x40 byte buffer exchanged with the model through message 0x411.
struct BoneQuery {
    int mUnk00;       // 0x00
    int mUnk04;       // 0x04
    u8 mUnk08[0x2C];  // 0x08
    int mPos[3];      // 0x34, 0x38, 0x3C
};

extern "C" void* memset(void*, int, unsigned long);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void fn_800225C8(void* out, String* a, String* b);
extern "C" void fn_80027690(int handle);
extern "C" void fn_800277E0(int handle, Vec3* pos, int, Vec4* color, int, int);
extern "C" int fn_80027A58(int type, void* pos, Vec4* color, int, int);
extern "C" int fn_8003A1F0(int, int, int);
extern "C" void fn_8003F15C(float, int, int, int, Skin*, int);
extern "C" int fn_8008EA9C(int, int);
extern "C" int fn_800AFD20(Actor* actor);
extern "C" void fn_8016EEF4(void);
extern "C" void fn_8028C188(int* out, int, int);
int SkinnedActorOnHitEvent(int* event);
extern "C" void fn_800913EC(int handle, Vec3* out, float time);
extern "C" void fn_800A0168(Mtx34* out, Mtx34* in);
extern "C" void fn_800A0030(Vec3* out, Mtx34* mtx, Vec3* in);
int MemFlagIsSet(void* owner, int flag);
void AnimApply(int handle, char* name, int anim, int arg);
void ModelSetMatrix(int model, Mtx34* mtx, char* name);
void MtxSetEuler(Mtx34* mtx, Vec3* euler);
void MtxGetEuler(Vec3* euler, Mtx34* mtx);
void BoneGetMatrix(Mtx34* mtx, void* bone);
void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void PlacementToMatrix(Place* place, Mtx34* mtx);
void Vec3Set(Vec3* v, float x, float y, float z);
void EulerNormalize(Vec3* out, Vec3* in);
void SkinnedActorUpdateModel(Actor* actor);

extern float lbl_803BE6F8[];
extern float lbl_804D9AC4;
extern float lbl_804D9AC8;
extern float lbl_804D9ACC;
extern float lbl_804D9AD0;
extern float lbl_804D9AD4;
extern float lbl_804D9AD8;

int ModelAnimate(int model, char* name, int arg);
int ModelFindBone(int model, int bone);
// The symbol name in symbols.txt does not spell out the two Vec3 pointers the
// retail call site passes, so they are passed as ints to keep the name.
int ModelBoneHitNearest(int from, int to, int model, int anim, int* bone,
                        Vec3* hitPos, Vec3* hitNormal, unsigned int* flags);
Skin* SkinBlendTint(Skin* skin, int a, int b, int c, float t);
int ActorPlayAnim(int model, unsigned long anim, int arg);
int HasCondition(void* owner, int condition);

void* VAlloc(int size);
void VFree(void* ptr);
unsigned long AssetFind(unsigned long group, unsigned long id);
int FxFind(const char* name);
extern "C" void fn_800CDD10(void*);
extern "C" unsigned int fn_800BA84C(int);
extern "C" void fn_8008DE4C(int, int, int, int);
extern "C" void fn_8016D6A8(void*);
void SkinnedActorPlayAnimSet(Actor* actor, int flags, int anim);
int Rand();

extern int lbl_803BE6C8[6];
extern unsigned int lbl_803BE6E0[6];
extern BehaviourState lbl_803BE590[];
extern Vec4 lbl_8034A1D0;
extern Vec4 lbl_8034A1E0;
extern Vec4 lbl_8034A1F0;
extern int lbl_804D3538;
extern float lbl_804D353C;
extern float lbl_804D3540;
extern int lbl_804D3544;
extern int lbl_804D3548;
extern int lbl_804D54A0;
extern int lbl_804D54A4;
extern int lbl_804D54A8;
extern int lbl_804D54AC;
extern float lbl_804D54B0;
extern float lbl_804D54B4;
extern float lbl_804D9A80;
extern float lbl_804D9A84;
extern float lbl_804D9A88;
extern float lbl_804D9A8C;
extern float lbl_804D9A98;
extern int lbl_804D34F8;
extern int lbl_804D34FC;
extern int lbl_804D3500;
extern int lbl_804D3504;
extern int lbl_804D3508;
extern int lbl_804D350C;
extern int lbl_804D3510;
extern int lbl_804D3514;
extern int lbl_804D3518;
extern int lbl_804D351C;
extern int lbl_804D3520;
extern int lbl_804D3524;
extern int lbl_804D3528;
extern int lbl_804D352C;
extern int lbl_804D3530;
extern int lbl_804D3534;

// Message handler of the skinned actor.
int SkinnedActorOnMessage(int msg, void* p)
{
    MsgData* data = (MsgData*)p;

    switch (msg) {
    case 0x408:
    {
        Actor* actor = data->mActor;
        BoneQuery buf;
        Vec3 from;
        Vec3 to;
        int bone;
        Vec3 hitPos;
        Vec3 hitNormal;
        unsigned int hitFlags;
        int anim;

        memset(&buf, 0, 0x40);
        buf.mUnk04 = lbl_803BE6C8[0];
        SendMsg(actor->mModel, 0x411, &buf);

        from.x = (float)data->mVec[0];
        from.y = (float)data->mVec[1];
        from.z = (float)data->mVec[2];
        to.x = (float)buf.mPos[0];
        to.y = (float)buf.mPos[1];
        to.z = (float)buf.mPos[2];

        anim = ModelAnimate(actor->mSkin, 0, 0);
        anim = fn_8003A1F0(actor->mSkin, actor->mUnkF0, anim);
        if (ModelBoneHitNearest((int)&from, (int)&to, actor->mSkin, anim,
                                &bone, &hitPos, &hitNormal, &hitFlags)) {
            data->mOut0 = bone;
        } else {
            data->mOut0 = ModelFindBone(actor->mSkin, lbl_803BE6C8[0]);
        }
        data->mHash = 1;
        return 1;
    }
    case 0:
    {
        Actor* actor = data->mActor;
        Behaviour* beh = actor->mBehaviour;
        Skin* skin;
        int a;
        int b;

        if (actor->mTimer > lbl_804D9A80) {
            ActorPlayAnim(actor->mModel, 0xdbe2cae6UL, actor->mModel);
        }
        actor->mTimer = lbl_804D9A84;

        a = fn_800AFD20(actor);
        b = ModelAnimate(actor->mSkin, 0, 0);
        b = fn_8003A1F0(a, actor->mUnkF0, b);

        skin = (Skin*)((u8*)p + 0x10);
        if (beh->mTime18 >= lbl_804D9A88) {
            beh->mFlags &= ~4;
        } else if (beh->mFlags & 4) {
            float t = (lbl_804D9A88 - beh->mTime18) / lbl_804D9A88;
            if (t < lbl_804D9A84) {
                t = lbl_804D9A84;
            } else if (t > lbl_804D9A8C) {
                t = lbl_804D9A8C;
            }
            skin = SkinBlendTint(skin, 0xff, 0, 0, t);
        }
        if (actor->mMode == 6) {
            return 1;
        }
        fn_8003F15C(lbl_804D9A8C, a, b, actor->mUnkD0, skin, 0);
        return 1;
    }
    case 10:
    {
        int state = data->mActor->mBehaviour->mState;

        if (state != 9 && state != 7) {
            return 0;
        }
        return 1;
    }
    case 0x407:
        return SkinnedActorOnHitEvent((int*)p);
    case 0x41D:
    {
        Actor* actor = data->mActor;
        Behaviour* beh = actor->mBehaviour;

        if (data->mHash != 0x7a1c0ef3) {
            return 0;
        }
        if (data->mOut1 != 0) {
            beh->mNextState = 9;
            lbl_803BE590[beh->mState].mCanChange(actor, beh->mNextState);
            lbl_803BE590[beh->mNextState].mEnter(actor);
        }
        return 1;
    }
    case 0x41E:
    {
        Actor* actor = data->mActor;
        Behaviour* beh = actor->mBehaviour;

        if (data->mHash == 0x4a1b522d) {
            data->mOut1 = ((beh->mFlags >> 1) & 1) ^ 1;
            return 1;
        }
        if (data->mHash == 0xad8576fa) {
            String stateName(lbl_803BE590[beh->mState].mName);
            String prefix("");
            Buffer full;

            fn_800225C8(&full, &stateName, &prefix);
            fn_8028C188(&data->mOut1, ((int*)&full)[1], ((int*)&full)[2]);
            return 0;
        }
        if (data->mHash == 0x7d45bf64) {
            data->mOut1 = 1;
            return 1;
        }
        return 0;
    }
    case 0xC14:
        if (data->mOut1 != 0x7bda2b92) {
            return 0;
        }
        data->mActor->mBehaviour->mFlags &= ~8;
        return 1;
    default:
        return 0;
    }
}

// Per frame behaviour update of the skinned actor.
int SkinnedActorUpdateBehaviour(void* owner)
{
    Actor* actor = (Actor*)owner;
    Behaviour* beh = actor->mBehaviour;
    int state;
    int next;

    beh->mTime18 += actor->mDeltaTime;
    beh->mTime10 += actor->mDeltaTime;

    if (beh->mFlags & 1) {
        if (beh->mFxHandle == -1) {
            Vec4 color = lbl_8034A1D0;

            beh->mFxHandle = fn_80027A58(1, &actor->mPos, &color, lbl_804D3538, 8);
        } else if (beh->mFxTimer < lbl_804D9A8C) {
            beh->mFxTimer += actor->mDeltaTime / (float)lbl_804D3544;
            if (beh->mFxTimer >= lbl_804D9A8C) {
                beh->mFxTimer = lbl_804D9A8C;
                beh->mFlags |= 2;
            }
        }
    } else if (beh->mFlags & 0x10) {
        if (beh->mFxHandle == -1) {
            Vec4 color = lbl_8034A1E0;

            beh->mFxHandle = fn_80027A58(1, &actor->mPos, &color, lbl_804D3538, 8);
            beh->mFxTimer = lbl_804D9A8C;
        } else if (beh->mFxTimer < lbl_804D9A8C) {
            beh->mFxTimer += actor->mDeltaTime / (float)lbl_804D3544;
            if (beh->mFxTimer >= lbl_804D9A8C) {
                beh->mFxTimer = lbl_804D9A8C;
            }
        }
    } else if (beh->mFxHandle != -1) {
        beh->mFxTimer -= actor->mDeltaTime / (float)lbl_804D3548;
        if (beh->mFxTimer <= lbl_804D9A84) {
            beh->mFxTimer = lbl_804D9A84;
            fn_80027690(beh->mFxHandle);
            beh->mFxHandle = -1;
        }
    }

    state = fn_8008EA9C(actor->mUnkD4, 0);
    if ((state == 0 || fn_8008EA9C(actor->mUnkD4, 0) == 2) &&
        HasCondition(actor->mCondition, 0xD) == 0 &&
        HasCondition(actor->mCondition, 0xA) == 0) {
        lbl_803BE590[beh->mState].mEnter(actor);
    } else if (HasCondition(actor->mCondition, 0xD) != 0) {
        SkinnedActorUpdateModel(actor);
        return 1;
    }

    next = lbl_803BE590[beh->mState].mGetNext(actor);
    if (next != beh->mState &&
        lbl_803BE590[beh->mState].mCanChange(actor, next)) {
        lbl_803BE590[next].mEnter(actor);
    }

    if ((beh->mState == 0xB || beh->mState == 0xC) && beh->mUnk1C != -1) {
        fn_8016EEF4();
        beh->mUnk1C = -1;
    }

    if (beh->mFxHandle != -1) {
        float t = beh->mFxTimer;
        Vec4 color = lbl_8034A1F0;
        BoneQuery buf;
        Vec3 pos;

        color.x = lbl_804D353C * t;
        color.y = lbl_804D54B0 * t;
        color.z = lbl_804D54B4 * t;
        color.w = lbl_804D3540 * t;

        if ((beh->mFlags & 2) || (beh->mFlags & 1)) {
            color.y = lbl_804D9A84;
            color.x = lbl_804D9A8C;
            color.z = lbl_804D9A84;
            color.w = beh->mFxTimer;
        } else if (beh->mFlags & 0x10) {
            color.x = lbl_804D9A84;
            color.y = lbl_804D9A84;
            color.z = lbl_804D9A8C;
            color.w = t;
        }

        buf.mUnk04 = lbl_804D54AC;
        SendMsg(actor->mModel, 0x411, &buf);

        pos.x = (float)buf.mPos[0];
        pos.y = (float)buf.mPos[1];
        pos.z = (float)buf.mPos[2];

        fn_800277E0(beh->mFxHandle, &pos, 0, &color, 0, 0);
    }
    return 1;
}

// Frees the effect state allocated by HydraSetupBehaviour.
void HydraFreeBehaviour(void* owner)
{
    Actor* actor = (Actor*)owner;

    if (actor != 0) {
        VFree(actor->mBehaviour);
        actor->mUpdateBehaviour = 0;
        actor->mBehaviour = 0;
    }
}

// Allocates the hydra effect state and loads the assets and effects it uses.
void HydraSetupBehaviour(void* owner, void* actorPtr)
{
    Actor* actor = (Actor*)actorPtr;
    Behaviour* beh;
    int i;

    if (actorPtr == 0) {
        return;
    }

    beh = (Behaviour*)VAlloc(0x2c);
    if (beh == 0) {
        actor->mBehaviour = 0;
    } else {
        actor->mBehaviour = beh;
        actor->mUpdateBehaviour = SkinnedActorUpdateBehaviour;
        actor->mRenderCallback = fn_800CDD10;

        beh->mOwner = owner;
        beh->mUnk04 = -1;
        beh->mTime18 = lbl_804D9A98;
        beh->mTime10 = lbl_804D9A98;
        beh->mTime14 = lbl_804D9A98;
        beh->mPad22 = 0;
        beh->mUnk1C = -1;
        beh->mFxHandle = -1;
        beh->mFxTimer = lbl_804D9A84;
        beh->mState = 1;
        beh->mNextState = 1;

        for (i = 0; i < 6; i++) {
            lbl_803BE6C8[i] = AssetFind(0xf1754bf2UL, lbl_803BE6E0[i]);
        }

        lbl_804D54A0 = AssetFind(0xf1754bf2UL, 0x751d97e6UL);
        lbl_804D54A4 = AssetFind(0xf1754bf2UL, 0x2e9564ebUL);
        lbl_804D54A8 = AssetFind(0xf1754bf2UL, 0x5b234f74UL);
        lbl_804D54AC = AssetFind(0xf1754bf2UL, 0xc44d06f6UL);

        lbl_804D34F8 = FxFind("T_GuardStart");
        lbl_804D34FC = FxFind("T_GuardLoop");
        lbl_804D3500 = FxFind("T_GuardEnd");
        lbl_804D3504 = FxFind("T_HideStart");
        lbl_804D3508 = FxFind("T_HideEnd");
        lbl_804D350C = FxFind("T_Melee");
        lbl_804D3510 = FxFind("T_Paralyze1");
        lbl_804D3514 = FxFind("T_Paralyze2");
        lbl_804D3518 = FxFind("T_Ready");
        lbl_804D351C = FxFind("T_FlyBy");
        lbl_804D3520 = FxFind("T_Taunt");
        lbl_804D3530 = FxFind("hyd_fire");
        lbl_804D3524 = FxFind("hyd_hit");
        lbl_804D3528 = FxFind("hyd_die");
        lbl_804D352C = FxFind("hyd_rico");
        lbl_804D3534 = FxFind("newbigroar");
    }
}

extern "C" int fn_80162610(Actor* actor, void* state, int newState = 0);
extern "C" int fn_8016325C(void* state);
extern "C" int fn_8016F120(int handle, void* block);
extern "C" void fn_8008E824(float value, int handle, int);
extern "C" void fn_80090390(int handle, int);
extern "C" int rand(void);
extern "C" int fn_800A1BC8(int, int, int, int, int);
extern "C" void fn_80090974(float, int, int, int, int,
                            int (*callback)(int, void*), void*);
extern "C" void fn_8008F4F0(float, int, int);
void ConditionUpdateBit(void* owner, int bit, int value);

extern float lbl_804D354C;
extern float lbl_804D3550;
extern float lbl_804D3554;
extern float lbl_804D3558;
extern float lbl_804D355C;
extern float lbl_804D9A9C;
extern float lbl_804D9AAC;
extern float lbl_804D9AB0;
extern float lbl_804D9AB4;
extern float lbl_804D9AB8;
extern float lbl_804D9ABC;
extern float lbl_804D9AC0;

// Hydra behaviour message handler: starts/stops the state and picks the Fx rate
// from the value the actor carries at 0xC4->0x30.
int HydraOnBehaviourStateMessage(int msg, void* p)
{
    Actor* actor = (Actor*)p;
    Behaviour* b = actor->mBehaviour;

    switch (msg) {
    case (int)0x1B83833F:
        fn_80162610(actor, actor->mStateMachine);
        lbl_803BE590[b->mState].mCanChange(actor, b->mNextState);
        lbl_803BE590[b->mNextState].mEnter(actor);
        b->mFlags &= ~1;
        b->mFlags &= ~2;
        break;
    case (int)0xBAC4B348:
    case 0x703C957C:
        b->mFlags |= 1;
        if (b->mState == 2) {
            u32 value = actor->mDataC4->mValue30;
            if (actor->mUnk904 >= (float)value * lbl_804D3558) {
                if (actor->mUnk904 >= (float)value * lbl_804D355C)
                    fn_8008E824(lbl_804D9A9C / lbl_804D354C, actor->mUnkD4, 0);
                else
                    fn_8008E824(lbl_804D9A9C / lbl_804D3550, actor->mUnkD4, 0);
            } else {
                fn_8008E824(lbl_804D9A9C / lbl_804D3554, actor->mUnkD4, 0);
            }
        }
        break;
    case 0x20EE36:
    case (int)0xF829DB93:
        if (fn_8016325C(actor->mStateMachine) == 5) {
            fn_80162610(actor, actor->mStateMachine, (int)0xF829DB93);
        } else {
            fn_80162610(actor, actor->mStateMachine, 0x20EE36);
            fn_8016F120(lbl_804D3530, actor->mMsgBlock24);
        }
        b->mFlags &= ~0x10;
        break;
    }
    return 1;
}

// Smaller hydra behaviour message handler: same start/stop handling plus the
// "hit" message that stops the Fx immediately.
int HydraOnBehaviourHitMessage(int msg, void* p)
{
    Actor* actor = (Actor*)p;
    Behaviour* b = actor->mBehaviour;

    switch (msg) {
    case (int)0x1B83833F:
        fn_80162610(actor, actor->mStateMachine);
        fn_80090390(actor->mUnkD4, 1);
        break;
    case 0x20EE36:
    case (int)0xF829DB93:
        if (fn_8016325C(actor->mStateMachine) == 5) {
            fn_80162610(actor, actor->mStateMachine, (int)0xF829DB93);
        } else {
            fn_80162610(actor, actor->mStateMachine, 0x20EE36);
            fn_8016F120(lbl_804D3530, actor->mMsgBlock24);
        }
        b->mFlags &= ~0x10;
        break;
    }
    return 1;
}

void ConditionSetBit(void* owner, int bit, int value);

// Behaviour state-change check: records the requested state, clears the two
// conditions it gates on and always refuses the change.
int HydraCanChangeFromIdle(Actor* actor, int nextState)
{
    Behaviour* beh = actor->mBehaviour;

    ConditionSetBit(actor->mCondition, 0, 0);
    ConditionSetBit(actor->mCondition, 4, 0);
    beh->mNextState = nextState;
    return 0;
}

// Behaviour state-change check: allows the change only for states 0xB and 0xC.
int HydraCanChangeToParalyze(Actor* actor, int nextState)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mNextState = nextState;
    ConditionSetBit(actor->mCondition, 0, 0);
    ConditionSetBit(actor->mCondition, 4, 0);
    if (nextState == 0xB || nextState == 0xC) {
        return 1;
    }
    return 0;
}

// Records the requested next state and clears the two conditions gating it.
// (0x80240B28)
int HydraSetBehaviourNextState(Actor* actor, int nextState)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mNextState = nextState;
    ConditionSetBit(actor->mCondition, 7, 0);
    ConditionSetBit(actor->mCondition, 8, 0);
    return 1;
}

// Returns the behaviour's current state, or 0 when there is no actor.
// (0x80240B7C)
int HydraGetBehaviourState(Actor* actor)
{
    if (actor != 0) {
        return actor->mBehaviour->mState;
    }
    return 0;
}

void ActorAim(int* out, Actor* actor, int target);
void FollowSetTargetVec(void* owner, Vec3* target);

// Aims the hydra at its follow target (in thousandths) and returns the
// behaviour state it is currently in. (0x80240B98)
int HydraAimAtFollowTarget(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;
    int aim[3];
    Vec3 target;
    // The three integers at 0x24 are the actor's aim offset.
    int* offset = (int*)actor->mMsgBlock24;

    SkinnedActorUpdateModel(actor);

    ActorAim(aim, actor, 1);
    aim[0] = aim[0] * 1000 + offset[0];
    aim[1] = aim[1] * 1000 + offset[1];
    aim[2] = aim[2] * 1000 + offset[2];

    target.x = (float)aim[0];
    target.y = (float)aim[1];
    target.z = (float)aim[2];

    FollowSetTargetVec(actor->mCondition, &target);
    return beh->mState;
}

// 3x4 transform matrix, filled in by ActorGetMatrix. The last three floats are
// the translation part.
struct Mtx34 {
    float m[9];         // 0x00
    Vec3 mTranslation;  // 0x24
};

// 0x1c byte reply buffer of the "target bone" message 0x400.
struct TargetBoneReply {
    u8 mUnk00[0x18];  // 0x00
    u32 mFlags;       // 0x18
};

// 0x18 byte transform of the target handed to the bone hit test.
struct TargetBoneXform {
    u8 mUnk00[0x18];  // 0x00
};

// 0x5c byte payload of the "aim at bone" message 0x407.
struct AimAtBoneMsg {
    Actor* mTarget;          // 0x00
    Actor* mOwner;           // 0x04
    int mModel;              // 0x08
    float mUnk0C;            // 0x0C
    int mUnk10;              // 0x10
    int mUnk14;              // 0x14
    int mBonePosX;           // 0x18
    int mBonePosY;           // 0x1C
    int mBonePosZ;           // 0x20
    u8 mUnk24[0x30 - 0x24];  // 0x24
    int mDeltaX;             // 0x30
    int mDeltaY;             // 0x34
    int mDeltaZ;             // 0x38
    int mUnk3C;              // 0x3C
    int mUnk40;              // 0x40
    int mUnk44;              // 0x44
    int mUnk48;              // 0x48
    float mUnk4C;            // 0x4C
    u8 mUnk50[0x5c - 0x50];  // 0x50
};

// 0x30 byte per-bone hit box record; the bone position is at 0x24.
struct BoneHitBox {
    u8 mUnk00[0x24];  // 0x00
    float mPosX;      // 0x24
    float mPosY;      // 0x28
    float mPosZ;      // 0x2C
};

extern float lbl_804D9AA0;
extern float lbl_804D9AA4;
extern float lbl_804D9AA8;

Actor* FollowGetTargetObj(void* owner);
Vec3* ModelBonePositions(int model);
void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void Vec3Copy(Vec3* dst, Vec3* src);
int ModelBoneHitBox(int, int, int, int);
extern "C" void fn_8016D864(void* out, int value);
extern "C" void fn_8016DD14(Vec3* vec, Vec3* offset);

// Tests the follow target's three aim bones against the hydra's model and asks
// the target to aim at the first bone that is hit. Returns the next behaviour
// state. (0x80240C88)
int HydraGetNextState(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;
    Actor* target;
    Vec3* bones;
    int hitBoxes;
    int found;
    int bone;

    SkinnedActorUpdateModel(actor);

    if (HasCondition(actor->mCondition, 9) != 0) {
        return 0xb;
    }
    if (fn_8016325C(actor->mStateMachine) != 5) {
        return HasCondition(actor->mCondition, 0x13) ? 0 : 3;
    }

    target = FollowGetTargetObj(actor->mCondition);
    found = 0;
    bone = -1;

    bones = ModelBonePositions(actor->mSkin);
    hitBoxes = fn_8003A1F0(actor->mSkin, actor->mUnkF0,
                           ModelAnimate(actor->mSkin, 0, 0));

    if (target != 0) {
        TargetBoneReply reply;
        int sent = SendMsg(target->mModel, 0x400, &reply);

        if (sent != 0 && (reply.mFlags & 2) != 0) {
            Vec3 offset = {lbl_804D9AA0, lbl_804D9AA0, lbl_804D9AA0};
            Mtx34 mtx;
            TargetBoneXform xform;
            Vec3 pos;

            ActorGetMatrix(&mtx, target);
            fn_8016D864(&xform, *(int*)((u8*)target + 0x40));

            int b = ModelFindBone(actor->mSkin, lbl_804D54A0);
            Vec3Copy(&pos, bones + b);
            fn_8016DD14(&pos, &offset);
            if (ModelBoneHitBox((int)&pos, hitBoxes + b * 0x30,
                                (int)&xform, (int)&mtx) != 0) {
                found = 1;
                bone = b;
            }

            b = ModelFindBone(actor->mSkin, lbl_804D54A4);
            Vec3Copy(&pos, bones + b);
            fn_8016DD14(&pos, &offset);
            if (ModelBoneHitBox((int)&pos, hitBoxes + b * 0x30,
                                (int)&xform, (int)&mtx) != 0) {
                found = 1;
                bone = b;
            }

            b = ModelFindBone(actor->mSkin, lbl_804D54A8);
            Vec3Copy(&pos, bones + b);
            fn_8016DD14(&pos, &offset);
            if (ModelBoneHitBox((int)&pos, hitBoxes + b * 0x30,
                                (int)&xform, (int)&mtx) != 0) {
                found = 1;
                bone = b;
            }

            if (found) {
                AimAtBoneMsg msg;
                BoneHitBox* hitBox = (BoneHitBox*)(hitBoxes + bone * 0x30);
                int* targetOffset = (int*)target->mMsgBlock24;
                int hx;
                int hy;
                int hz;

                memset(&msg, 0, 0x5c);
                msg.mUnk14 = 0x1f336d;
                msg.mUnk4C = lbl_804D9A8C;
                msg.mUnk3C = -1;
                msg.mUnk40 = -1;
                msg.mUnk44 = -1;
                msg.mUnk0C = lbl_804D9AA4 * (lbl_804D9AA8 * actor->mDeltaTime);
                msg.mTarget = target;
                msg.mOwner = actor;
                msg.mModel = actor->mModel;

                hx = (int)hitBox->mPosX;
                hy = (int)hitBox->mPosY;
                hz = (int)hitBox->mPosZ;
                msg.mBonePosX = hx;
                msg.mBonePosY = hy;
                msg.mBonePosZ = hz;
                msg.mDeltaX = targetOffset[0] - hx;
                msg.mDeltaY = targetOffset[1] - hy;
                msg.mDeltaZ = targetOffset[2] - hz;

                SendMsg(target->mModel, 0x407, &msg);
            }
        }
    }
    return beh->mState;
}

// Get-next-state hook of the hydra's block reaction state: keeps the block
// countdown running, fires the hit effect when it expires, and drops the guard
// once the block timer has run out. (0x80240F84)
int HydraGetHitNextState(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;
    int c4v;
    u32 roll;

    SkinnedActorUpdateModel(actor);

    if (HasCondition(actor->mCondition, 6) != 0) {
        return 6;
    }
    if (HasCondition(actor->mCondition, 9) != 0) {
        return 0xb;
    }
    if (fn_8016325C(actor->mStateMachine) == 5) {
        return 4;
    }

    if (beh->mPad22 > 0) {
        beh->mTime14 -= actor->mDeltaTime;
        if (beh->mTime14 <= lbl_804D9A84) {
            beh->mTime10 = lbl_804D9A84;
            beh->mPad22 = beh->mPad22 - 1;
            roll = (u32)rand();
            beh->mTime14 = lbl_804D9AB4 * ((float)roll / lbl_804D9AAC) + lbl_804D9AB0;
            c4v = fn_800A1BC8(*(int*)((u8*)actor->mDataC4 + 0x14),
                              *(int*)((u8*)actor->mDataC4 + 0x1c),
                              0x20ee36, 0, 0);
            fn_80090974(lbl_804D9A8C, actor->mUnkD4, 1, c4v, 0x10,
                        HydraOnBehaviourHitMessage, actor);
            fn_8008F4F0(lbl_804D9A8C, actor->mUnkD4, 1);
            beh->mFlags |= 0x10;
        }
    }

    if (fn_8008EA9C(actor->mUnkD4, 1) == 1 || beh->mPad22 > 0 ||
        beh->mState != 3 || beh->mTime10 <= lbl_804D9AB8) {
        return beh->mState;
    }

    ConditionUpdateBit(actor->mCondition, 0, 0);
    ConditionUpdateBit(actor->mCondition, 3, 0);
    ActorPlayAnim(actor->mModel, 0x658818e9UL, actor->mModel);
    return -((beh->mFlags >> 3) & 1) & 5;
}

// Get-next-state hook of the hydra's paralyze reaction state: a pending
// paralyze condition ends it, and a cleared condition 4 while the state is 2
// releases the state. (0x80241170)
int HydraGetParalyzeNextState(Actor* actor)
{
    Behaviour* beh;

    if (actor == 0) {
        return 0;
    }
    beh = actor->mBehaviour;
    SkinnedActorUpdateModel(actor);
    if (HasCondition(actor->mCondition, 9) != 0) {
        return 0xb;
    }
    if (HasCondition(actor->mCondition, 4) != 0) {
        ConditionSetBit(actor->mCondition, 4, 0);
        if (beh->mState == 2) {
            beh->mFlags &= ~1;
            beh->mFlags &= ~2;
            return 10;
        }
    }
    return beh->mState;
}

// Get-next-state hook: blocks while condition 6 is set, keeps the current
// state while the paralyze condition 0x13 is set and crouches otherwise.
// (0x80241224)
int HydraGetBlockNextState(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    SkinnedActorUpdateModel(actor);
    if (HasCondition(actor->mCondition, 6) != 0) {
        return 6;
    }
    if (HasCondition(actor->mCondition, 0x13) != 0) {
        return beh->mState;
    }
    return 2;
}

// Get-next-state hook: stays in the current state while condition 6 is set and
// goes to state 8 otherwise. (0x80241298)
int HydraGetIdleNextState(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    SkinnedActorUpdateModel(actor);
    if (HasCondition(actor->mCondition, 6) == 0) {
        return 8;
    }
    return beh->mState;
}

// Randomises the timer at 0x8FC on entering the "state C" behaviour state,
// resets the movement speed and plays the taunt animation once.
// (0x802412F0)
int HydraEnterStateC(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mState = 0xc;
    beh->mNextState = 0xc;
    actor->mUnk8FC = (float)fn_800BA84C(7);
    fn_8008DE4C(actor->mUnkD4, 0, beh->mUnk04, 0x10);
    if (HasCondition(actor->mCondition, 10) == 0) {
        ConditionSetBit(actor->mCondition, 10, 1);
        ActorPlayAnim(actor->mModel, 0xdc04e3d5UL, actor->mModel);
    }
    return 1;
}

// Leaves the "state B" behaviour: resets the follow offset, drops the aim
// handle and stops the death display list. (0x802413A8)
int HydraEnterStateB(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mState = 0xb;
    beh->mNextState = 0xc;
    fn_8016D6A8(actor->mUnk27C);
    SkinnedActorPlayAnimSet(actor, 0x18, -1);
    if (beh->mUnk1C != -1) {
        fn_8016EEF4();
        beh->mUnk1C = -1;
    }
    fn_8016F120(lbl_804D3528, actor->mMsgBlock24);
    return 1;
}

// Picks one of two message targets at random, then opens the "state D"
// behaviour state. (0x8024142C)
int HydraEnterStateD(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;
    int r;

    beh->mState = 10;
    beh->mNextState = 3;
    r = Rand() % 2;
    SkinnedActorPlayAnimSet(actor, 0x18, r);
    if (r == 0) {
        fn_8016F120(lbl_804D3510, actor->mMsgBlock24);
    } else {
        fn_8016F120(lbl_804D3514, actor->mMsgBlock24);
    }
    ConditionSetBit(actor->mCondition, 8, 1);
    return 1;
}

// Opens the "state E" behaviour state and drops the aim handle.
// (0x802414CC)
int HydraEnterStateE(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mState = 9;
    beh->mNextState = 3;
    SkinnedActorPlayAnimSet(actor, 0x19, -1);
    ConditionSetBit(actor->mCondition, 7, 1);
    return 1;
}

// Enters the hydra behaviour state 8 (next state 3): records both states, drops
// the aim handle, sets condition 7, plays the hide animation and restarts the
// "T_HideEnd" display list. Descriptive name; retail: 0x80241528.
int HydraEnterBehaviourState8(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mState = 8;
    beh->mNextState = 3;
    SkinnedActorPlayAnimSet(actor, 0x19, -1);
    ConditionSetBit(actor->mCondition, 7, 1);
    ActorPlayAnim(actor->mModel, 0xa6bef592UL, actor->mModel);
    fn_8016F120(lbl_804D3508, actor->mMsgBlock24);
    return 1;
}

// Payload of message 0xC14 exchanged with the model when leaving state 6.
struct HideEndMsg {
    int mUnk00;  // 0x00
    int mOut0;   // 0x04
    int mModel;  // 0x08
    u32 mHash;   // 0x0C
};

// Enters the hydra behaviour state 7 (next state 7): tells the model the hide
// sequence has ended when the previous state was 6, records both states and
// drops the aim handle. Descriptive name; retail: 0x802415A4.
int HydraEnterBehaviourState7(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    if (beh->mState == 6 && actor->mModel != -1) {
        HideEndMsg msg;

        msg.mOut0 = 0;
        msg.mModel = actor->mModel;
        msg.mHash = 0x37bd5bdd;
        SendMsg(actor->mModel, 0xC14, &msg);
    }
    beh->mState = 7;
    beh->mNextState = 7;
    SkinnedActorPlayAnimSet(actor, 0x19, -1);
    return 1;
}

// Enters the hydra behaviour state 6 (next state 7): records both states,
// drops the aim handle, restarts the "T_HideStart" display list and plays the
// hide animation. Descriptive name; retail: 0x80241634.
int HydraEnterBehaviourState6(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mState = 6;
    beh->mNextState = 7;
    SkinnedActorPlayAnimSet(actor, 0x19, -1);
    fn_8016F120(lbl_804D3504, actor->mMsgBlock24);
    ActorPlayAnim(actor->mModel, 0x8b3bd7a1UL, actor->mModel);
    return 1;
}

// Enters the hydra behaviour state 5 (next state 0): records both states,
// drops the aim handle, restarts the "T_Taunt" display list and sets
// condition 7. Descriptive name; retail: 0x802416A0.
int HydraEnterBehaviourState5(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mState = 5;
    beh->mNextState = 0;
    SkinnedActorPlayAnimSet(actor, 0x19, 0);
    fn_8016F120(lbl_804D3520, actor->mMsgBlock24);
    ConditionSetBit(actor->mCondition, 7, 1);
    return 1;
}

// Enters the hydra's "guard end" behaviour state 2 and picks the Fx playback
// rate from the value the actor carries at 0xC4->0x30. Descriptive name;
// retail: 0x80241708.
int HydraEnterBehaviourState2(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;
    int value;

    beh->mState = 2;
    beh->mNextState = 3;
    SkinnedActorPlayAnimSet(actor, 0x18, -1);

    value = (int)actor->mDataC4->mValue30;
    if (actor->mUnk904 < (float)value * lbl_804D3558) {
        fn_8008E824(lbl_804D9ABC, actor->mUnkD4, 0);
    } else if (actor->mUnk904 < (float)value * lbl_804D355C) {
        fn_8008E824(lbl_804D9AC0, actor->mUnkD4, 0);
    } else {
        fn_8008E824(lbl_804D9A8C, actor->mUnkD4, 0);
    }

    if (beh->mUnk1C != -1) {
        fn_8016EEF4();
        beh->mUnk1C = -1;
    }
    fn_8016F120(lbl_804D3500, actor->mMsgBlock24);
    ConditionSetBit(actor->mCondition, 7, 1);
    return 1;
}

// Enters the hydra's "guard loop" behaviour state 1 and, when no guard effect
// is running yet, starts the "T_GuardLoop" display list. Descriptive name;
// retail: 0x80241820.
int HydraEnterBehaviourState1(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mState = 1;
    beh->mNextState = 1;
    SkinnedActorPlayAnimSet(actor, 9, -1);
    if (beh->mUnk1C == -1) {
        beh->mUnk1C = fn_8016F120(lbl_804D34FC, actor->mMsgBlock24);
    }
    return 1;
}

// Enters the hydra's "guard start" behaviour state 0 (next state 1): records
// both states, drops the aim handle, starts the "T_GuardStart" display list,
// sets condition 8 and plays the guard-start animation. Descriptive name;
// retail: 0x8024188C.
int HydraEnterBehaviourState0(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mState = 0;
    beh->mNextState = 1;
    SkinnedActorPlayAnimSet(actor, 0x18, -1);
    ConditionSetBit(actor->mCondition, 8, 1);
    fn_8016F120(lbl_804D34F8, actor->mMsgBlock24);
    ActorPlayAnim(actor->mModel, 0xdc367006UL, actor->mModel);
    return 1;
}

// Enters the hydra's "melee" behaviour state 4: records both states, drops
// the aim handle and starts the "T_Melee" display list. Descriptive name;
// retail: 0x80241908.
int HydraEnterBehaviourState4(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;

    beh->mState = 4;
    beh->mNextState = 4;
    SkinnedActorPlayAnimSet(actor, 9, -1);
    fn_8016F120(lbl_804D350C, actor->mMsgBlock24);
    return 1;
}

// Enters the hydra's guard behaviour state 3: picks how many hits it will
// block from one of three probability tables (chosen by the distance value the
// actor carries at 0x904 against the value at 0xC4->0x30), rolls a random
// number against the chosen cumulative table and stores the result in the
// behaviour block counter. A zero count aborts the guard (state 5). Descriptive
// name; retail: 0x8024195C.
int HydraEnterBehaviourState3(Actor* actor)
{
    Behaviour* beh = actor->mBehaviour;
    const float* table;
    float roll;
    float total;
    float value;
    int i;

    if (MemFlagIsSet(actor->mCondition, 0) == 0) {
        total = lbl_804D9A84;
        ConditionUpdateBit(actor->mCondition, 0, 1);
        beh->mFlags &= ~8;
        roll = (float)rand() / lbl_804D9AAC;
        value = (float)(int)actor->mDataC4->mValue30;
        if (actor->mUnk904 < value * lbl_804D3558) {
            table = lbl_803BE6F8 + 12;
        } else if (actor->mUnk904 < value * lbl_804D355C) {
            table = lbl_803BE6F8 + 6;
        } else {
            table = lbl_803BE6F8;
        }
        for (i = 0; i < 6; i++) {
            total = total + table[i];
            if (roll < total || i == 5) {
                beh->mPad22 = i;
                break;
            }
        }
        if (beh->mPad22 < 1) {
            actor->mBehaviour->mState = 5;
            actor->mBehaviour->mNextState = 0;
            SkinnedActorPlayAnimSet(actor, 0x19, 0);
            fn_8016F120(lbl_804D3520, actor->mMsgBlock24);
            ConditionSetBit(actor->mCondition, 7, 1);
            return 1;
        }
        if (beh->mPad22 >= 3) {
            beh->mFlags |= 8;
        }
        beh->mTime14 =
            lbl_804D9AB4 * ((float)rand() / lbl_804D9AAC) + lbl_804D9AB0;
    }
    beh->mState = 3;
    beh->mNextState = 3;
    SkinnedActorPlayAnimSet(actor, 9, -1);
    return 1;
}

// Advances the skinned actor's model animation and rebuilds its placement
// matrix. While condition 0xC is set the matrix comes from a skeleton bone (or
// from the euler angles when the model is not bound); with condition 0xD the
// actor matrix drives the movement, and otherwise the euler angles are steered
// towards zero and integrated. Descriptive name; retail: 0x80241C0C.
void SkinnedActorUpdateModel(Actor* actor)
{
    Vec3 angles;
    Vec3 offset;
    Mtx34 actorMtx;
    Mtx34 mtx;
    int* ipos;
    float* placePos;
    float angle;
    float speed;
    float step;
    float scale;
    int anim;

    ipos = (int*)actor->mMsgBlock24;
    fn_800913EC(actor->mUnkD4, &offset, actor->mDeltaTime);
    anim = ModelAnimate(actor->mSkin, (char*)actor->mUnkF0, 1);
    AnimApply(actor->mUnkD4, (char*)actor->mUnkF0, anim, actor->mUnkD0);

    if (HasCondition(actor->mCondition, 0xC) != 0) {
        if (fn_8008EA9C(actor->mUnkD4, 0) != 0) {
            if (HasCondition(actor->mCondition, 0xC) != 0) {
                BoneGetMatrix(&mtx, (void*)actor->mUnkF0);
                MtxGetEuler(&angles, &mtx);
                PlacementToMatrix((Place*)actor, &mtx);
                placePos = (float*)((u8*)actor->mUnkF0 + 0x10);
                actor->mPos.x = placePos[0];
                actor->mPos.y = placePos[1];
                actor->mPos.z = placePos[2];
                ipos[0] = (int)placePos[0];
                ipos[1] = (int)placePos[1];
                ipos[2] = (int)placePos[2];
            } else {
                Vec3Set(&angles, actor->mAngles.z, lbl_804D9A84, lbl_804D9A84);
                EulerNormalize(&angles, &angles);
                MtxSetEuler(&mtx, &angles);
                mtx.mTranslation.x = (float)ipos[0];
                mtx.mTranslation.y = (float)ipos[1];
                mtx.mTranslation.z = (float)ipos[2];
                PlacementToMatrix((Place*)actor, &mtx);
            }
        }
        ModelSetMatrix(actor->mSkin, (Mtx34*)0, (char*)actor->mUnkF0);
        return;
    }

    if (HasCondition(actor->mCondition, 0xD) != 0) {
        ActorGetMatrix(&actorMtx, actor);
        fn_800A0168(&actorMtx, &actorMtx);
        fn_800A0030(&offset, &actorMtx, &offset);
        actor->mVec240.x = offset.x;
        actor->mVec240.y = offset.y;
        actor->mVec240.z = offset.z;
        actor->mVec240.z = lbl_804D9A84;
        scale = lbl_804D9AA4 * actor->mDeltaTime;
        actor->mVec240.x = actor->mVec240.x * scale;
        actor->mVec240.y = actor->mVec240.y * scale;
        actor->mVec240.z = actor->mVec240.z * scale;
        MtxSetEuler(&mtx, &actor->mAngles);
        mtx.mTranslation.x = (float)ipos[0];
        mtx.mTranslation.y = (float)ipos[1];
        mtx.mTranslation.z = (float)ipos[2];
        PlacementToMatrix((Place*)actor, &mtx);
        placePos = (float*)((u8*)actor->mUnkF0 + 0x10);
        placePos[0] = lbl_804D9A84;
        placePos[1] = lbl_804D9A84;
        placePos[2] = lbl_804D9A84;
        ModelSetMatrix(actor->mSkin, &mtx, (char*)actor->mUnkF0);
        return;
    }

    placePos = (float*)((u8*)actor->mUnkF0 + 0x10);
    placePos[0] = lbl_804D9A84;
    placePos[1] = lbl_804D9A84;
    placePos[2] = lbl_804D9A84;

    angle = actor->mUnk284;
    if (actor->mUnk26C == 1) {
        speed = *(float*)((u8*)actor->mDataC4 + 0xC0) * lbl_804D9AC4;
    } else if (angle > lbl_804D9AC8 || angle < lbl_804D9ACC) {
        speed = lbl_804D9AD0 * *(float*)((u8*)actor->mDataC4 + 0xC0);
    } else {
        speed = *(float*)((u8*)actor->mDataC4 + 0xC0);
    }
    if (angle > lbl_804D9AD4) {
        step = speed * actor->mDeltaTime / lbl_804D9AB8;
        if (step > angle) {
            step = angle;
        }
        actor->mAngles.z = actor->mAngles.z + step;
    } else if (angle < lbl_804D9AD8) {
        step = -(speed * actor->mDeltaTime / lbl_804D9AB8);
        if (step < angle) {
            step = angle;
        }
        actor->mAngles.z = actor->mAngles.z + step;
    }

    if (HasCondition(actor->mCondition, 0) != 0) {
        ActorGetMatrix(&actorMtx, actor);
        fn_800A0168(&actorMtx, &actorMtx);
        fn_800A0030(&offset, &actorMtx, &offset);
        actor->mVec240.x = offset.x;
        actor->mVec240.y = offset.y;
        actor->mVec240.z = offset.z;
        actor->mVec240.z = lbl_804D9A84;
        scale = lbl_804D9AB8 / actor->mDeltaTime;
        actor->mVec240.x = actor->mVec240.x * scale;
        actor->mVec240.y = actor->mVec240.y * scale;
        actor->mVec240.z = actor->mVec240.z * scale;
    }

    EulerNormalize(&actor->mAngles, &actor->mAngles);
    MtxSetEuler(&mtx, &actor->mAngles);
    mtx.mTranslation.x = (float)ipos[0];
    mtx.mTranslation.y = (float)ipos[1];
    mtx.mTranslation.z = (float)ipos[2];
    PlacementToMatrix((Place*)actor, &mtx);

    ActorGetMatrix(&actorMtx, actor);
    fn_800A0168(&actorMtx, &actorMtx);
    offset.y = lbl_804D9A84;
    offset.z = lbl_804D9A84;
    fn_800A0030(&offset, &actorMtx, &offset);
    mtx.mTranslation.x = mtx.mTranslation.x + offset.x;
    mtx.mTranslation.y = mtx.mTranslation.y + offset.y;
    mtx.mTranslation.z = mtx.mTranslation.z + offset.z;
    ModelSetMatrix(actor->mSkin, &mtx, (char*)actor->mUnkF0);
}

int AnimBankGetSetCount(unsigned short, unsigned short, unsigned short);
extern "C" void fn_8008F230(float value, int handle, int, int);
extern "C" void fn_8008F4E8(int handle, int);
extern "C" void fn_8008F378(int handle);
extern "C" void fn_8008F4EC(int handle, int);
extern float lbl_804D9ADC;

// Switches the animation the model of the current behaviour state plays to
// `anim` of the state's animation set (a negative index picks a random one of
// the set), starts the state's display-list callback and restarts the effect
// playback. (0x80242160)
void SkinnedActorPlayAnimSet(Actor* actor, int flags, int anim)
{
    Behaviour* beh = actor->mBehaviour;
    int unchanged = 0;
    int count;
    int oldAnim;

    if (HasCondition(actor->mCondition, 0xc) != 0) {
        ConditionSetBit(actor->mCondition, 0xc, 0);
        ConditionSetBit(actor->mCondition, 0xe, 1);
        fn_8008F4E8(actor->mUnkD4, 0);
        fn_8008F378(actor->mUnkD4);
        fn_8008F4EC(actor->mUnkD4, 0);
    }

    count = AnimBankGetSetCount(actor->mDataC4->mUnk14,
                                actor->mDataC4->mUnk1C,
                                lbl_803BE590[beh->mState].mUnk00);
    if (count == 0) {
        if (fn_8008EA9C(actor->mUnkD4, 0) == 1) {
            fn_8008DE4C(actor->mUnkD4, 0, beh->mUnk04, 0x10);
        }
    } else {
        if (anim < 0) {
            anim = rand() % count;
        }
        oldAnim = beh->mUnk04;
        beh->mUnk04 = fn_800A1BC8(actor->mDataC4->mUnk14,
                                  actor->mDataC4->mUnk1C,
                                  lbl_803BE590[beh->mState].mUnk00, anim, 0);
        if (oldAnim == beh->mUnk04) {
            unchanged = 1;
        }
        if (HasCondition(actor->mCondition, 0xe) != 0 || unchanged != 0) {
            fn_8008F230(lbl_804D9A84, actor->mUnkD4, 0, 0);
            ConditionSetBit(actor->mCondition, 0xe, 0);
        } else {
            fn_8008F230(lbl_804D9ADC, actor->mUnkD4, 0, 1);
        }
        if (flags & 8) {
            ConditionSetBit(actor->mCondition, 0, 1);
        } else {
            ConditionSetBit(actor->mCondition, 0, 0);
        }
        fn_80090974(lbl_804D9A8C, actor->mUnkD4, 0, beh->mUnk04, flags,
                    lbl_803BE590[beh->mState].mUnk10, actor);
        fn_8008F4F0(lbl_804D9A8C, actor->mUnkD4, 0);
    }
}

struct Owner;
int OwnerId(Owner* owner);
int ModelBoneName(int model, int bone);
extern "C" int fn_800C5210(int owner, int msg, void* data);
extern "C" void fn_800B20B8(Actor* actor, int value);
extern float lbl_804D9AE0;
extern float lbl_804D9AE4;

// Message handler of the skinned actor: drops the event unless the bone it hit
// is one of the actor's guard bones, then applies the "damage" the event carries
// as a movement delta and, once it has run out, releases the block conditions.
// (0x80242368)
int SkinnedActorOnHitEvent(int* event)
{
    Actor* actor = (Actor*)event[0];
    Behaviour* beh = actor->mBehaviour;
    u32 hash = (u32)event[5];
    int bone;
    int guarded;
    float* elapsed;
    float limit;
    u32 value;

    if (hash == 0xA349664Au) {
        return 1;
    }
    if (hash == 0x11A6Bu) {
        return 1;
    }
    if (hash != 0x6CA5F172u) {
        guarded = 0;
        bone = ModelBoneName(actor->mSkin, event[0x11]);
        if (beh->mFlags & 2) {
            if (bone == lbl_803BE6C8[0]) {
                guarded = 1;
            } else if (bone == lbl_803BE6C8[1]) {
                guarded = 1;
            } else if (bone == lbl_803BE6C8[2]) {
                guarded = 1;
            } else if (bone == lbl_803BE6C8[3]) {
                guarded = 1;
            } else if (bone == lbl_803BE6C8[4]) {
                guarded = 1;
            } else if (bone == lbl_803BE6C8[5]) {
                guarded = 1;
            }
        }
        if (guarded == 0) {
            if (((u32)event[2] >> 24) == 2) {
                fn_8016F120(lbl_804D352C, actor->mMsgBlock24);
            }
            event[0x16] = 1;
            return 1;
        }
        if (fn_800C5210(OwnerId((Owner*)actor), 0x407, event) != 0) {
            return 1;
        }
    }

    // 0x0C holds the remaining movement the event gives the actor, as a float.
    elapsed = (float*)event + 3;
    *elapsed = *elapsed * (lbl_804D9AE0 * (float)fn_800BA84C(6));

    if (MemFlagIsSet(actor->mCondition, 0xa) != 0) {
        limit = actor->mUnk904 - lbl_804D9A8C;
        if (*elapsed < limit) {
            limit = *elapsed;
        }
        *elapsed = limit;
    }

    if (((u32)event[2] >> 24) == 2) {
        beh->mFlags |= 4;
        fn_8016F120(lbl_804D3524, actor->mMsgBlock24);
    }

    value = actor->mDataC4->mValue30;
    limit = lbl_804D9AC4 * (float)value;
    if (actor->mUnk904 > limit && actor->mUnk904 - *elapsed <= limit) {
        ActorPlayAnim(actor->mModel, 0x24AE8385UL, event[2]);
    } else {
        limit = lbl_804D9AE4 * (float)value;
        if (actor->mUnk904 > limit && actor->mUnk904 - *elapsed <= limit) {
            ActorPlayAnim(actor->mModel, 0xECC78791UL, event[2]);
        }
    }

    actor->mUnk904 = actor->mUnk904 - *elapsed;
    actor->mUnk214 = event[0x11];
    limit = (float)actor->mDataC4->mValue30;
    if (actor->mUnk904 >= limit) {
        actor->mUnk904 = limit;
    }

    beh->mTime18 = lbl_804D9A84;
    if (actor->mUnk904 <= lbl_804D9A84 && actor->mMode != 4 &&
        actor->mMode != 6 && HasCondition(actor->mCondition, 9) == 0 &&
        HasCondition(actor->mCondition, 0xa) == 0) {
        if (HasCondition(actor->mCondition, 0xc) != 0) {
            ConditionSetBit(actor->mCondition, 0xc, 0);
            fn_80090390(actor->mUnkD4, 0);
        }
        ConditionSetBit(actor->mCondition, 9, 1);
        if (hash == 0x6CA5F172u) {
            ConditionSetBit(actor->mCondition, 0xf, 1);
        }
    }

    if (hash != 0x6CA5F172u) {
        if (actor->mUnk904 > lbl_804D9A84 &&
            HasCondition(actor->mCondition, 0x20) == 0) {
            ConditionSetBit(actor->mCondition, 4, 1);
        }
        fn_800B20B8(actor, event[2]);
    }
    return 1;
}

// Fixed-size block pool (descriptive name; layout inferred from the code).
struct BlockPool {
    int mBlockSize;   // 0x00 size of one block
    int mBlockCount;  // 0x04 blocks per chunk
    int* mChunks;     // 0x08 list of allocated chunks (first word links to the previous)
    int* mFreeList;   // 0x0C list of free blocks (first word links to the next free block)
    int mUsedCount;   // 0x10 blocks handed out
};

void VFree(void* ptr);
void* VAlloc(int size);

// Returns a block of mBlockSize bytes to the pool, or frees it when its size differs.
void BlockPoolFree(BlockPool* pool, int* block, int size)
{
    if (block != 0) {
        if (size == pool->mBlockSize) {
            *block = (int)pool->mFreeList;
            pool->mFreeList = block;
            pool->mUsedCount = pool->mUsedCount - 1;
        } else {
            VFree(block);
        }
    }
}

// Allocates mBlockSize bytes from the pool; other sizes go straight to VAlloc.
int* BlockPoolAlloc(BlockPool* pool, int size)
{
    int* block;
    int* chunk;
    int i;

    if (size != pool->mBlockSize) {
        return (int*)VAlloc(size);
    }
    if (pool->mFreeList == 0) {
        chunk = (int*)VAlloc(pool->mBlockCount * pool->mBlockSize + 4);
        if (chunk == 0) {
            return 0;
        }
        *chunk = (int)pool->mChunks;
        pool->mChunks = chunk;
        block = chunk + 1;
        for (i = 0; i != pool->mBlockCount; i++) {
            *block = (int)pool->mFreeList;
            pool->mFreeList = block;
            block = (int*)((char*)block + pool->mBlockSize);
        }
    }
    block = pool->mFreeList;
    if (block == 0) {
        return 0;
    }
    pool->mFreeList = (int*)*block;
    memset(block, 0, size);
    pool->mUsedCount = pool->mUsedCount + 1;
    return block;
}

// Initialises a block descriptor: rounded-up size, flags, and three zeroed words.
void MemBlockInit(u32* block, int size, u32 flags)
{
    block[0] = (size + 3) & 0xFFFFFFFC;
    block[1] = flags;
    block[2] = 0;
    block[3] = 0;
    block[4] = 0;
}

// Frees the node list hanging off a block descriptor and clears the descriptor;
// frees the descriptor itself when asked to.
void* MemBlockFree(u32* block, short freeBlock)
{
    u32* node;

    if (block != 0) {
        node = (u32*)block[2];
        while (node != 0) {
            u32* next = (u32*)node[0];
            VFree(node);
            node = next;
        }
        block[0] = 0;
        block[1] = 0;
        block[2] = 0;
        block[3] = 0;
        block[4] = 0;
        if (freeBlock > 0) {
            VFree(block);
        }
    }
    return block;
}

// Node of a multi-child tree whose children form a doubly linked sibling
// list (descriptive name; layout inferred from the code).
struct ChildNode {
    ChildNode* mParent;  // 0x00
    ChildNode* mFirst;   // 0x04 first child
    ChildNode* mLast;    // 0x08 last child
    ChildNode* mNext;    // 0x0C next sibling (towards the first child)
    ChildNode* mPrev;    // 0x10 previous sibling (towards the last child)
};

// Unlinks a node from its parent's child list.  Returns 0 when the root or
// the node is null, otherwise the unlinked node with its links cleared.
static inline ChildNode* UnlinkChildNode(ChildNode** root, ChildNode* node)
{
    ChildNode* parent;

    if (*root == 0 || node == 0) {
        return 0;
    }
    if (*root == node) {
        *root = 0;
    } else {
        parent = node->mParent;
        if (parent != 0) {
            if (parent->mFirst == node) {
                parent->mFirst = node->mNext;
                if (parent->mFirst == 0) {
                    parent->mLast = 0;
                } else {
                    parent->mFirst->mPrev = 0;
                }
            } else if (parent->mLast == node) {
                parent->mLast = node->mPrev;
                if (parent->mLast == 0) {
                    parent->mFirst = 0;
                } else {
                    parent->mLast->mNext = 0;
                }
            } else {
                if (node->mPrev != 0) {
                    node->mPrev->mNext = node->mNext;
                }
                if (node->mNext != 0) {
                    node->mNext->mPrev = node->mPrev;
                }
            }
        }
    }
    node->mParent = 0;
    node->mPrev = 0;
    node->mNext = 0;
    return node;
}

// Moves every child of src to the end of dst's child list, in order.
ChildNode* ReparentChildNodes(ChildNode** root, ChildNode* dst, ChildNode* src)
{
    ChildNode* node;

    while ((node = src->mFirst) != 0) {
        node = UnlinkChildNode(root, node);
        if (*root != 0 && dst != 0 && node != 0) {
            if (dst->mLast == 0) {
                dst->mFirst = node;
            } else {
                dst->mLast->mNext = node;
            }
            node->mPrev = dst->mLast;
            node->mNext = 0;
            node->mParent = dst;
            dst->mLast = node;
        }
    }
    return dst;
}

// Detaches a node from the tree; returns 0 when the root or the node is null.
ChildNode* DetachChildNode(ChildNode** root, ChildNode* node)
{
    ChildNode* parent;

    if (*root == 0 || node == 0) {
        return 0;
    }
    if (*root == node) {
        *root = 0;
    } else {
        parent = node->mParent;
        if (parent != 0) {
            if (parent->mFirst == node) {
                parent->mFirst = node->mNext;
                if (parent->mFirst == 0) {
                    parent->mLast = 0;
                } else {
                    parent->mFirst->mPrev = 0;
                }
            } else if (parent->mLast == node) {
                parent->mLast = node->mPrev;
                if (parent->mLast == 0) {
                    parent->mFirst = 0;
                } else {
                    parent->mLast->mNext = 0;
                }
            } else {
                if (node->mPrev != 0) {
                    node->mPrev->mNext = node->mNext;
                }
                if (node->mNext != 0) {
                    node->mNext->mPrev = node->mPrev;
                }
            }
        }
    }
    node->mParent = 0;
    node->mPrev = 0;
    node->mNext = 0;
    return node;
}

// Returns the child-node tree root stored in the given root slot.
// Descriptive name; retail: 0x80242BA4. Called from other units as
// fn_80242BA4, so the placeholder name is kept.
extern "C" unsigned int fn_80242BA4(unsigned int* root)
{
    return *root;
}

// Appends a node at the end of a parent's child list. Returns 0 when the tree
// root, the parent or the node is null, otherwise the appended node.
// Descriptive name; retail: 0x80242BAC.
ChildNode* AppendChildNode(ChildNode** root, ChildNode* parent, ChildNode* node)
{
    if (*root == 0 || parent == 0 || node == 0) {
        return 0;
    }
    if (parent->mLast == 0) {
        parent->mFirst = node;
    } else {
        parent->mLast->mNext = node;
    }
    node->mPrev = parent->mLast;
    node->mNext = 0;
    node->mParent = parent;
    parent->mLast = node;
    return node;
}

// Stores node as the tree root when the root slot is still empty.
// Returns 1 when it stored the node, 0 otherwise.
// Descriptive name; retail: 0x80242C08.
int SetChildNodeRootIfEmpty(ChildNode** root, ChildNode* node)
{
    if (*root == 0 && node != 0) {
        *root = node;
        return 1;
    }
    return 0;
}

// Container passed as the first argument of the detach/notify pair (layout
// inferred from the code; only the root slot is typed so far).
struct ChildNodeTree {
    ChildNode* mRoot;  // 0x00
    int mUnk04;        // 0x04 rounded size of the node block
    int mUnk08;        // 0x08 node block descriptor: size
    int mUnk0C;        // 0x0C node block descriptor: flags
    int mUnk10;        // 0x10 node block descriptor: free list head
    int mUnk14;        // 0x14 node block descriptor
    int mUnk18;        // 0x18 node block descriptor
};

// Payload handed to the visitor: a pointer to the tree's third word and a
// value copied from its second word.
struct ChildNodeNotify {
    int* mSlot;  // 0x00
    int mValue;  // 0x04
};

typedef u8 (*ChildNodeVisitor)(ChildNode*, void*);

u8 FreeChildNodeNotify(ChildNode* node, void* arg);
u8 WalkChildNodes(ChildNode* node, ChildNodeVisitor visitor, void* arg);

// Detaches node from its tree (same rules as DetachChildNode) and hands the
// unlinked node to the tree's visitor.
// Descriptive name; retail: 0x80242C30.
void DetachChildNodeAndNotify(ChildNodeTree* tree, ChildNode* node)
{
    ChildNodeNotify notify;

    if (node != 0) {
        node = UnlinkChildNode(&tree->mRoot, node);
        notify.mSlot = &tree->mUnk08;
        notify.mValue = tree->mUnk04;
        WalkChildNodes(node, FreeChildNodeNotify, &notify);
    }
}

// Returns a node detached from a child-node tree to the tree's block pool.
// Descriptive name; retail: 0x80242D60.
u8 FreeChildNodeNotify(ChildNode* node, void* arg)
{
    ChildNodeNotify* notify = (ChildNodeNotify*)arg;

    BlockPoolFree((BlockPool*)notify->mSlot, (int*)node, notify->mValue);
    return 0;
}

// Frees a child-node tree: detaches its root node, clears the block
// descriptor at 0x08 and, when asked to, releases the tree storage itself.
// Descriptive name; retail: 0x80242D94.
ChildNodeTree* FreeChildNodeTree(ChildNodeTree* tree, short freeTree)
{
    if (tree != 0) {
        DetachChildNodeAndNotify(tree, tree->mRoot);
        MemBlockFree((u32*)tree + 2, -1);
        if (freeTree > 0) {
            VFree(tree);
        }
    }
    return tree;
}

// Initialises a child-node tree: zeroes the header, records the rounded size
// of the node payload and sets up the node block descriptor at 0x08.
// Descriptive name; retail: 0x80242DF4.
ChildNodeTree* ChildNodeTreeInit(ChildNodeTree* tree, int nodeSize)
{
    tree->mRoot = 0;
    tree->mUnk04 = 0;
    tree->mUnk08 = 0;
    tree->mUnk0C = 0;
    tree->mUnk10 = 0;
    tree->mUnk14 = 0;
    tree->mUnk18 = 0;
    tree->mUnk04 = (nodeSize + 0x17) & ~3;
    MemBlockInit((u32*)&tree->mUnk08, tree->mUnk04, 0);
    return tree;
}

// Depth-first walk over a child-node tree: visits the descendants of the given
// node (down to a fixed depth, then recurses) and calls the visitor on every
// node.  Stops and returns 1 as soon as the visitor returns non-zero; when the
// visitor is null nothing is visited and 0 is returned.
// Descriptive name (0x80242E58).
u8 WalkChildNodes(ChildNode* node, ChildNodeVisitor visitor, void* arg)
{
    u8 r;

    if (!visitor)
        return 0;

    for (ChildNode* a = node->mFirst; a != 0;a = a->mNext) {
        if (!visitor) {
            r = 0;
        } else {
            for (ChildNode* b = a->mFirst; b != 0;b = b->mNext) {
                if (!visitor) {
                    r = 0;
                } else {
                    for (ChildNode* c = b->mFirst; c != 0;c = c->mNext) {
                        if (!visitor) {
                            r = 0;
                        } else {
                            for (ChildNode* d = c->mFirst; d != 0;d = d->mNext) {
                                if (!visitor) {
                                    r = 0;
                                } else {
                                    for (ChildNode* e = d->mFirst; e != 0;e = e->mNext) {
                                        if (!visitor) {
                                            r = 0;
                                        } else {
                                            for (ChildNode* g = e->mFirst; g != 0;g = g->mNext) {
                                                if (!visitor) {
                                                    r = 0;
                                                } else {
                                                    for (ChildNode* h = g->mFirst; h != 0;h = h->mNext) {
                                                        if (!visitor) {
                                                            r = 0;
                                                        } else {
                                                            for (ChildNode* i = h->mFirst; i != 0;i = i->mNext) {
                                                                if (!visitor) {
                                                                    r = 0;
                                                                } else {
                                                                    for (ChildNode* j = i->mFirst; j != 0;j = j->mNext) {
                                                                        if (!visitor) {
                                                                            r = 0;
                                                                        } else {
                                                                            r = WalkChildNodes(j, visitor, arg);
                                                                        }
                                                                        if (r)
                                                                            return 1;
                                                                    }
                                                                    r = visitor(i, arg);
                                                                }
                                                                if (r)
                                                                    return 1;
                                                            }
                                                            r = visitor(h, arg);
                                                        }
                                                        if (r)
                                                            return 1;
                                                    }
                                                    r = visitor(g, arg);
                                                }
                                                if (r)
                                                    return 1;
                                            }
                                            r = visitor(e, arg);
                                        }
                                        if (r)
                                            return 1;
                                    }
                                    r = visitor(d, arg);
                                }
                                if (r)
                                    return 1;
                            }
                            r = visitor(c, arg);
                        }
                        if (r)
                            return 1;
                    }
                    r = visitor(b, arg);
                }
                if (r)
                    return 1;
            }
            r = visitor(a, arg);
        }
        if (r)
            return 1;
    }
    return visitor(node, arg);
}

// Depth-first search over a child-node tree.  Tests the given node, then
// descends eight explicit levels before recursing on the children of the
// eighth level.  Returns 1 as soon as the visitor returns non-zero, 0 when
// the visitor is null or nothing was found.
// 0x8024311C. Other units call it as
// fn_8024311C, so the placeholder name is kept.
extern "C" u8 fn_8024311C(ChildNode* node, ChildNodeVisitor visitor, void* arg)
{
    u8 r;

    if (!visitor)
        return 0;
    if (visitor(node, arg))
        return 1;
    for (ChildNode* a = node->mFirst; a != 0; a = a->mNext) {
        if (!visitor) {
            r = 0;
        } else if (visitor(a, arg)) {
            r = 1;
        } else {
            r = 0;
            for (ChildNode* b = a->mFirst; b != 0; b = b->mNext) {
                if (!visitor) {
                    r = 0;
                } else if (visitor(b, arg)) {
                    r = 1;
                } else {
                    r = 0;
                    for (ChildNode* c = b->mFirst; c != 0; c = c->mNext) {
                        if (!visitor) {
                            r = 0;
                        } else if (visitor(c, arg)) {
                            r = 1;
                        } else {
                            r = 0;
                            for (ChildNode* d = c->mFirst; d != 0; d = d->mNext) {
                                if (!visitor) {
                                    r = 0;
                                } else if (visitor(d, arg)) {
                                    r = 1;
                                } else {
                                    r = 0;
                                    for (ChildNode* e = d->mFirst; e != 0; e = e->mNext) {
                                        if (!visitor) {
                                            r = 0;
                                        } else if (visitor(e, arg)) {
                                            r = 1;
                                        } else {
                                            r = 0;
                                            for (ChildNode* f = e->mFirst; f != 0; f = f->mNext) {
                                                if (!visitor) {
                                                    r = 0;
                                                } else if (visitor(f, arg)) {
                                                    r = 1;
                                                } else {
                                                    r = 0;
                                                    for (ChildNode* g = f->mFirst; g != 0; g = g->mNext) {
                                                        if (!visitor) {
                                                            r = 0;
                                                        } else if (visitor(g, arg)) {
                                                            r = 1;
                                                        } else {
                                                            r = 0;
                                                            for (ChildNode* h = g->mFirst; h != 0; h = h->mNext) {
                                                                if (!visitor) {
                                                                    r = 0;
                                                                } else if (visitor(h, arg)) {
                                                                    r = 1;
                                                                } else {
                                                                    r = 0;
                                                                    for (ChildNode* i = h->mFirst; i != 0; i = i->mNext) {
                                                                        if (fn_8024311C(i, visitor, arg))
                                                                            return 1;
                                                                    }
                                                                }
                                                                if (r)
                                                                    return 1;
                                                            }
                                                        }
                                                        if (r)
                                                            return 1;
                                                    }
                                                }
                                                if (r)
                                                    return 1;
                                            }
                                        }
                                        if (r)
                                            return 1;
                                    }
                                }
                                if (r)
                                    return 1;
                            }
                        }
                        if (r)
                            return 1;
                    }
                }
                if (r)
                    return 1;
            }
        }
        if (r)
            return 1;
    }
    return 0;
}

// Same depth-first search as fn_8024311C, but returns the first node the
// visitor accepts instead of a flag.
// 0x80243448. Other units call it as
// fn_80243448, so the placeholder name is kept.
extern "C" ChildNode* fn_80243448(ChildNode* node, ChildNodeVisitor visitor, void* arg)
{
    ChildNode* r;

    if (!visitor)
        return 0;
    if (visitor(node, arg))
        return node;
    for (ChildNode* a = node->mFirst; a != 0; a = a->mNext) {
        if (!visitor) {
            r = 0;
        } else if (visitor(a, arg)) {
            r = a;
        } else {
            r = 0;
            for (ChildNode* b = a->mFirst; b != 0; b = b->mNext) {
                if (!visitor) {
                    r = 0;
                } else if (visitor(b, arg)) {
                    r = b;
                } else {
                    r = 0;
                    for (ChildNode* c = b->mFirst; c != 0; c = c->mNext) {
                        if (!visitor) {
                            r = 0;
                        } else if (visitor(c, arg)) {
                            r = c;
                        } else {
                            r = 0;
                            for (ChildNode* d = c->mFirst; d != 0; d = d->mNext) {
                                if (!visitor) {
                                    r = 0;
                                } else if (visitor(d, arg)) {
                                    r = d;
                                } else {
                                    r = 0;
                                    for (ChildNode* e = d->mFirst; e != 0; e = e->mNext) {
                                        if (!visitor) {
                                            r = 0;
                                        } else if (visitor(e, arg)) {
                                            r = e;
                                        } else {
                                            r = 0;
                                            for (ChildNode* f = e->mFirst; f != 0; f = f->mNext) {
                                                if (!visitor) {
                                                    r = 0;
                                                } else if (visitor(f, arg)) {
                                                    r = f;
                                                } else {
                                                    r = 0;
                                                    for (ChildNode* g = f->mFirst; g != 0; g = g->mNext) {
                                                        if (!visitor) {
                                                            r = 0;
                                                        } else if (visitor(g, arg)) {
                                                            r = g;
                                                        } else {
                                                            r = 0;
                                                            for (ChildNode* h = g->mFirst; h != 0; h = h->mNext) {
                                                                if (!visitor) {
                                                                    r = 0;
                                                                } else if (visitor(h, arg)) {
                                                                    r = h;
                                                                } else {
                                                                    r = 0;
                                                                    for (ChildNode* i = h->mFirst; i != 0; i = i->mNext) {
                                                                        r = fn_80243448(i, visitor, arg);
                                                                        if (r)
                                                                            return r;
                                                                    }
                                                                }
                                                                if (r)
                                                                    return r;
                                                            }
                                                        }
                                                        if (r)
                                                            return r;
                                                    }
                                                }
                                                if (r)
                                                    return r;
                                            }
                                        }
                                        if (r)
                                            return r;
                                    }
                                }
                                if (r)
                                    return r;
                            }
                        }
                        if (r)
                            return r;
                    }
                }
                if (r)
                    return r;
            }
        }
        if (r)
            return r;
    }
    return 0;
}

// A block list: a block count followed by the pool the blocks come from.
struct BlockList {
    u32 mUnk00;      // 0x00
    int mCount;      // 0x04
    BlockPool mPool; // 0x08
};

// Allocates the next block of the list from its pool (name is a guess).
void BlockListAlloc(BlockList* list)
{
    BlockPoolAlloc(&list->mPool, list->mCount);
}

// Host object referenced from the player at 0x1D68 (name is a guess).
struct DashHost {
    u8 mUnk00[0x38];  // 0x00
    int mModel;       // 0x38
};

// One of the two animation sets selected by PlayerInHost(); only the words
// used by the dash state are known.
struct PlayerAnimSet {
    u8 mUnk00[0x14];          // 0x00
    int mUnk14;               // 0x14
    u8 mUnk18[0x1C - 0x18];   // 0x18
    int mUnk1C;               // 0x1C
    u8 mUnk20[0x28 - 0x20];   // 0x20
    int mUnk28;               // 0x28
};

// Message 0x41E payload sent to the host's model (name is a guess).
struct DashAnimQuery {
    Actor* mActor;  // 0x00
    u32 mHash;      // 0x04
    int mResult;    // 0x08
    String mName;   // 0x0C
};

// Large actor object; only the words touched by the functions below are known.
struct LargeActor {
    u8 mUnk00[0x38];              // 0x00
    int mModel;                   // 0x38
    u8 mUnk3C[0x50 - 0x3C];       // 0x3C
    u32 mFlags;                   // 0x50
    u8 mUnk54[0x78 - 0x54];       // 0x54
    int mUnk78;                   // 0x78
    int mUnk7C;                   // 0x7C
    u8 mUnk80[0x8C - 0x80];       // 0x80
    void (*mEventCallback)(int, Actor*); // 0x8C
    u8 mUnk90[0x98 - 0x90];       // 0x90
    int mUnk98;                   // 0x98
    u8 mUnk9C[0xA4 - 0x9C];       // 0x9C
    int mUnkA4;                   // 0xA4
    u8 mUnkA8[0xE4 - 0xA8];       // 0xA8
    int mUnkE4;                   // 0xE4
    u8 mUnkE8[0xF0 - 0xE8];       // 0xE8
    int mUnkF0;                   // 0xF0
    u8 mUnkF4[0xFC - 0xF4];       // 0xF4
    int mUnkFC;                   // 0xFC
    u8 mUnk100[0x58C - 0x100];    // 0x100
    int mUnk58C;                  // 0x58C
    u8 mUnk590[0x638 - 0x590];    // 0x590
    int mUnk638;                  // 0x638
    u8 mUnk63C[0x7A4 - 0x63C];    // 0x63C
    u8 mUnk7A4[0x7B4 - 0x7A4];    // 0x7A4
    u32 mUnk7B4;                  // 0x7B4
    u8 mUnk7B8[0x7CC - 0x7B8];    // 0x7B8
    int mUnk7CC;                  // 0x7CC
    int mUnk7D0;                  // 0x7D0
    u8 mUnk7D4[0x1C00 - 0x7D4];   // 0x7D4
    int mUnk1C00;                 // 0x1C00
    u8 mUnk1C04[0x1D68 - 0x1C04]; // 0x1C04
    DashHost* mHost;              // 0x1D68
    u8 mUnk1D6C[0x1DA0 - 0x1D6C]; // 0x1D6C
    int mUnk1DA0;                 // 0x1DA0
    u8 mUnk1DA4[0x26C4 - 0x1DA4]; // 0x1DA4
    PlayerAnimSet** mAnimSets;    // 0x26C4
    u8 mUnk26C8[0x26D8 - 0x26C8]; // 0x26C8
    int mUnk26D8;                 // 0x26D8
};

extern "C" void fn_8004D604(void);

// Clears the pending action and parks the actor's flags (name is a guess).
void LargeActorResetFlags(LargeActor* actor)
{
    if (actor->mUnk1DA0 == 0) {
        actor->mUnk78 = 0;
        actor->mFlags |= 0x200;
        actor->mFlags &= ~0x20;
        if ((actor->mFlags & 0x2000) != 0) {
            actor->mFlags &= ~0x20000;
            actor->mFlags &= ~0x2000;
        } else {
            fn_8004D604();
        }
    }
    actor->mUnk26D8 = 1;
}

int PadPressed(int pad, int port, int button);
int PlayerInHost(Actor* actor);
void PlayerSetValue28(void* value, int set);
void PlayerSetActionMode(void* actor, int mode);
void PlayerResetGunTimer(void* actor);
void DashOnEvent(int event, Actor* actor);
extern "C" void fn_80090634(int, int, int, int);

// Checks the dash buttons against the host's pad and plays the matching
// animation; when a dash was pending it returns the player to normal control
// (name is a guess).
void PlayerDashCheck(Actor* actorBase)
{
    LargeActor* actor = (LargeActor*)actorBase;
    DashHost* host;

    actor->mUnk638 = 1;
    actor->mUnk7B4 &= ~1u;
    host = actor->mHost;
    if (host != 0) {
        if (PadPressed(actor->mUnk58C, 0, 0) == 1)
            ActorPlayAnim(host->mModel, 0x70A1506UL, actor->mModel);
        if (PadPressed(actor->mUnk58C, 0, 1) == 1)
            ActorPlayAnim(host->mModel, 0x6CD1384DUL, actor->mModel);
        if (PadPressed(actor->mUnk58C, 0, 5) == 1)
            ActorPlayAnim(host->mModel, 0x6CD4B3B2UL, actor->mModel);
        if (PadPressed(actor->mUnk58C, 0, 6) == 1)
            ActorPlayAnim(host->mModel, 0x2E1823B1UL, actor->mModel);
        if (PadPressed(actor->mUnk58C, 0, 3) == 1)
            ActorPlayAnim(host->mModel, 0x779385F3UL, actor->mModel);
        if (PadPressed(actor->mUnk58C, 0, 4) == 1)
            ActorPlayAnim(host->mModel, 0x7AE5FF65UL, actor->mModel);
        if (actor->mUnk1DA0 != 0) {
            PlayerSetValue28(&actor->mUnk7A4, actor->mUnk1DA0);
            fn_80090634(actor->mUnk7C, 0, 0, 0);
            PlayerSetActionMode(actorBase, 0xC);
        }
    }
}

// Enters the dash state: asks the host's model which dash animation to use,
// then stores the two effect handles and the event callback (name is a guess).
void PlayerDashStart(Actor* actorBase)
{
    LargeActor* actor = (LargeActor*)actorBase;
    DashAnimQuery query;
    PlayerAnimSet* set;
    int index;

    PlayerResetGunTimer(actorBase);
    actor->mUnk1DA0 = 0;
    if (actor->mHost != 0) {
        query.mActor = actorBase;
        query.mHash = 0x801EFD79;
        query.mResult = 0;
        if (SendMsg(actor->mHost->mModel, 0x41E, &query) != 0)
            index = actor->mUnk7D0 + query.mResult * 2;
        else
            index = actor->mUnk7D0 +
                    ActorPlayAnim(actor->mHost->mModel, 0x4AFBF22FUL, actor->mModel) * 2;
    } else {
        index = actor->mUnk7D0;
    }
    set = actor->mAnimSets[PlayerInHost(actorBase)];
    actor->mUnk98 = fn_800A1BC8(0x3506D0CE, set->mUnk28, actor->mUnk7CC, index, 0);
    actor->mFlags |= 0x20020;
    actor->mUnk78 = 1;
    actor->mEventCallback = DashOnEvent;
    actor->mUnkA4 = 0x18;
    actor->mUnk26D8 = 0;
    actor->mUnkF0 = fn_800A1BC8(set->mUnk14, set->mUnk1C, 0x2A1A099, 0, 0);
    actor->mUnkFC = 0x10;
    actor->mUnkE4 = 0;
    actor->mUnk1C00 = 0;
}

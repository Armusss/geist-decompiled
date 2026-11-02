// FearActor.c: the player ("Fear") actor (0x80045EBC-0x80064FD4).
// The player ghost possesses hosts, so this file holds its per-frame update, the host-dependent
// animation sets (two of them, selected by whether the player is inside a host), blur-hit reactions,
// camera field of view easing and the state machines for movement and actions.
// Only the small functions are drafted so far; the rest are listed in the TODO block at the end.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// One of the two animation sets of the player (see PlayerInHost).
struct PlayerAnimSet {
    u32 mHash;                // 0x00 kind of the set
    u8 mUnk04[0x28 - 0x04];   // 0x04
    int mBank;                // 0x28 animation bank
};

struct Actor {
    u8 mUnk00[0x38];            // 0x00
    u32 mId;                    // 0x38
    u8 mUnk3C[0x50 - 0x3C];     // 0x3C
    u32 mFlags;                 // 0x50 0x200000 = inside a host
    u8 mUnk54[0x184 - 0x54];    // 0x54
    int mCamera;                // 0x184
    u8 mUnk188[0x248 - 0x188];  // 0x188
    int mBody;                  // 0x248
    u8 mUnk24C[0x7A4 - 0x24C];  // 0x24C
    int mMode;                  // 0x7A4
    u8 mUnk7A8[0x7B4 - 0x7A8];  // 0x7A8
    u32 mState;                 // 0x7B4
    u8 mUnk7B8[0x1DA8 - 0x7B8];  // 0x7B8
    int mAnimSet;               // 0x1DA8 currently active animation set
    u8 mUnk1DAC[0x26C4 - 0x1DAC];  // 0x1DAC
    PlayerAnimSet* mSet[2];     // 0x26C4
    u8 mUnk26CC[0x26E4 - 0x26CC];  // 0x26CC
    Actor* mHost;               // 0x26E4
    u8 mUnk26E8[0x278C - 0x26E8];  // 0x26E8
    float mFov;                 // 0x278C
    float mFovTarget;           // 0x2790
    u8 mUnk2794[0x27D8 - 0x2794];  // 0x2794
    float mBlurA;               // 0x27D8
    float mBlurB;               // 0x27DC
    float mBlurD;               // 0x27E0
    float mBlurC;               // 0x27E4
    u8 mUnk27E8[0x27F4 - 0x27E8];  // 0x27E8
    int mUnk27F4;               // 0x27F4
};

extern int lbl_804D43E4;  // player system running
extern int lbl_804D27F8;
extern const float lbl_804D6280;
extern const double lbl_804D6288;

#ifdef __cplusplus
extern "C" {
#endif
void fn_800C5E00(int body, int a, int b, int c, int d);
int fn_800A1BC8(int a, int b, int c, int d, int e);
void fn_80093478(int anim, int a);
#ifdef __cplusplus
}
#endif
int fn_8007EF48(int a, int b, const char* name, int c, int d, int e, int f, int g, int h, int i);
int AnimBankGetSetCount(int a, int b, int c);
void AnimUnref(int anim);
void CameraSetFov(int camera, float fov);

// Whether the player is inside a host (selects which animation set is current).
static inline int PlayerInHostInline(Actor* a)
{
    int r;
    if (a == 0 || lbl_804D43E4 == 0) {
        r = 0;
    } else if (a->mFlags & 0x200000) {
        r = 1;
    } else {
        r = 0;
    }
    return r;
}

// 0x80045EBC: clears the blur-hit state
extern "C" int fn_80045EBC(Actor* a)
{
    a->mFlags &= ~2;
    a->mFlags &= ~0x20000000;
    a->mUnk27F4 = -1;
    return 1;
}

// 0x80045EE4: starts a blur-hit reaction unless the host kind or state forbids it
extern "C" int fn_80045EE4(float a, float b, float c, float d, float e, Actor* p)
{
    int ok = PlayerInHostInline(p) == 1;
    if (ok && (int)(p->mHost->mId >> 24) != 1 && (p->mSet[1]->mHash != 0x1091C) && p->mSet[1]->mHash != 0xA7F71344 &&
        p->mSet[1]->mHash != 0x4624485 && p->mSet[1]->mHash != 0x13C05 && p->mSet[1]->mHash != 0xE0AB9B4F &&
        p->mSet[1]->mHash != 0xFFF5 && p->mSet[1]->mHash != 0x745A81AE && p->mSet[1]->mHash != 0x8FA80B9E &&
        p->mSet[1]->mHash != 0xBE90C1A && p->mSet[1]->mHash != 0x80F3B697 && p->mSet[1]->mHash != 0xF63E7769 &&
        p->mSet[1]->mHash != 0x8C3E2D96) {
        ok = 1;
    } else {
        ok = 0;
    }
    if (!ok) {
        return 0;
    }
    fn_800C5E00(p->mBody, 1, (int)d, (int)e, 0);
    fn_8007EF48(p->mBody, 1, "Blur Hit", 0, 0, 0, 0, 0, 0, (int)p);
    p->mFlags |= 2;
    p->mFlags |= 0x20000000;
    p->mState |= 0x40000;
    p->mState &= ~0x1000;
    p->mBlurA = a;
    p->mBlurB = c;
    p->mBlurC = b;
    p->mBlurD = c;
    return 1;
}

// 0x800460CC: switches the active animation set (releases the old set's animations, takes the new one's)
extern "C" void fn_800460CC(Actor* a, u32 set)
{
    int cur = a->mAnimSet;
    if (cur != set) {
        int i;
        int n;
        if (cur != 0 && a->mMode != 0xC && cur != 0) {
            n = AnimBankGetSetCount(0x3506D0CE, a->mSet[PlayerInHostInline(a)]->mBank, cur);
            for (i = 0; i < n; i++) {
                AnimUnref(fn_800A1BC8(0x3506D0CE, a->mSet[PlayerInHostInline(a)]->mBank, a->mAnimSet, i, 0));
            }
            a->mAnimSet = 0;
        }
        n = AnimBankGetSetCount(0x3506D0CE, a->mSet[PlayerInHostInline(a)]->mBank, set);
        if (n > 0) {
            for (i = 0; i < n; i++) {
                fn_80093478(fn_800A1BC8(0x3506D0CE, a->mSet[PlayerInHostInline(a)]->mBank, set, i, 0), 1);
            }
            a->mAnimSet = set;
        }
    }
}

// 0x800462C8: releases the active animation set
extern "C" void fn_800462C8(Actor* a)
{
    int i;
    int n;
    if (a->mMode == 0xC) {
        return;
    }
    if (a->mAnimSet != 0) {
        n = AnimBankGetSetCount(0x3506D0CE, a->mSet[PlayerInHostInline(a)]->mBank, a->mAnimSet);
        for (i = 0; i < n; i++) {
            AnimUnref(fn_800A1BC8(0x3506D0CE, a->mSet[PlayerInHostInline(a)]->mBank, a->mAnimSet, i, 0));
        }
        a->mAnimSet = 0;
    }
}

// 0x800463F0
extern "C" int fn_800463F0(void)
{
    return lbl_804D27F8;
}

// 0x800463F8: eases the field of view towards its target
extern "C" void fn_800463F8(float dt, Actor** pp)
{
    Actor* a = *pp;
    float step = lbl_804D6280 * dt;
    if (a->mFov < a->mFovTarget) {
        a->mFov = a->mFov + step;
        if (a->mFov > a->mFovTarget) {
            a->mFov = a->mFovTarget;
        }
    } else {
        a->mFov = a->mFov - step;
        if (a->mFov < a->mFovTarget) {
            a->mFov = a->mFovTarget;
        }
    }
    CameraSetFov(a->mCamera, (float)(int)a->mFov);
}

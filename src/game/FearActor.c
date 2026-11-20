// FearActor.c: the player ("Fear") actor (0x80045EBC-0x80064FD4).
// The player ghost possesses hosts, so this file holds its per-frame update, the host-dependent
// animation sets (two of them, selected by whether the player is inside a host), blur-hit reactions,
// camera field of view easing and the state machines for movement and actions.
//
// One source file (boundary check 2026-11-18): the unit's string pool (.rodata 0x80342880-0x80342C08) is a
// single pool with one copy of the POSSESS..SCAN header table and the file's own name "FearActor.c"; its
// .sdata2 pool (0x804D6280-0x804D6428) has no duplicated constant, and the same literals (0x804D6288 int-to-float,
// 0x804D6298 0.0f) are read from fn_800463F8 through fn_800647AC; the first and last five functions use the
// unit's own sdata/sbss (lbl_804D43E0/E4, lbl_804D270C). The .data 0x803940A0-0x80394158 and
// .bss 0x8041FFD8-0x80420038 are touched only from this range. So the range is not split.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

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
    u8 mUnk00[0x30];            // 0x00
    int (*mHandler)(int msg, void* data);  // 0x30
    u8 mUnk34[0x38 - 0x34];     // 0x34
    u32 mId;                    // 0x38
    u8 mUnk3C[0x4C - 0x3C];     // 0x3C
    u32 mFlags4C;               // 0x4C 0x400 = skip the update once
    u32 mFlags;                 // 0x50 0x200000 = inside a host
    int mModel54;               // 0x54 body model
    int mModel58;               // 0x58 weapon model
    int mUnk5C;                 // 0x5C
    u8 mUnk60[0x78 - 0x60];     // 0x60
    int mUnk78;                 // 0x78
    int mUnk7C;                 // 0x7C
    int mUnk80;                 // 0x80
    int mUnk84;                 // 0x84
    int mUnk88;                 // 0x88
    u8 mUnk8C[0xBC - 0x8C];     // 0x8C
    int mUnkBC;                 // 0xBC
    int mUnkC0;                 // 0xC0
    int mUnkC4;                 // 0xC4
    int mUnkC8;                 // 0xC8 bone count
    u8 mUnkCC[0xD4 - 0xCC];     // 0xCC
    int mUnkD4;                 // 0xD4 animation
    u8 mUnkD8[0x11C - 0xD8];    // 0xD8
    int mUnk11C;                // 0x11C
    int mBoneCount120;          // 0x120
    u8 mUnk124[0x184 - 0x124];  // 0x124
    int mCamera;                // 0x184
    u8 mUnk188[0x248 - 0x188];  // 0x188
    int mBody;                  // 0x248
    u8 mUnk24C[0x254 - 0x24C];  // 0x24C
    u32 mFlags254;              // 0x254
    u8 mUnk258[0x2F4 - 0x258];  // 0x258
    int mPadState[4];           // 0x2F4 per pad
    u8 mUnk304[0x584 - 0x304];  // 0x304
    u32 mFlags584;              // 0x584
    u8 mUnk588[0x58C - 0x588];  // 0x588
    int mUnk58C;                // 0x58C pad/controller number
    u8 mUnk590[0x73C - 0x590];  // 0x590
    int mUnk73C;                // 0x73C
    u8 mUnk740[0x7A4 - 0x740];  // 0x740
    int mMode;                  // 0x7A4
    int mUnk7A8;                // 0x7A8 next action mode
    u8 mUnk7AC[0x7B4 - 0x7AC];  // 0x7AC
    u32 mState;                 // 0x7B4
    u8 mUnk7B8[0x7D0 - 0x7B8];  // 0x7B8
    int mUnk7D0;                // 0x7D0
    u8 mUnk7D4[0x908 - 0x7D4];  // 0x7D4
    u32 mUnk908;                // 0x908 (in hosts of kind 0x13)
    u8 mUnk90C[0x9F0 - 0x90C];  // 0x90C
    Vec3* mBoneOffsets;         // 0x9F0
    u8 mUnk9F4[0xA0C - 0x9F4];  // 0x9F4
    u8 mUnkA0C[0x1400 - 0xA0C]; // 0xA0C
    int mUnk1400;               // 0x1400
    char* mBone1404;            // 0x1404
    char* mBone1408;            // 0x1408
    u8 mUnk140C[4];             // 0x140C
    char* mBone1410;            // 0x1410
    char* mBone1414;            // 0x1414
    u8 mUnk1418[0x14A8 - 0x1418];  // 0x1418
    int mHud;                   // 0x14A8
    u8 mUnk14AC[0x14B8 - 0x14AC];  // 0x14AC
    int mUnk14B8;               // 0x14B8
    int mFontA;                 // 0x14BC
    int mFontB;                 // 0x14C0
    u8 mUnk14C4[0x14CC - 0x14C4];  // 0x14C4
    int mFontC;                 // 0x14CC
    int mFontD;                 // 0x14D0
    u8 mUnk14D4[0x1500 - 0x14D4];  // 0x14D4
    float mUnk1500;             // 0x1500
    float mUnk1504;             // 0x1504
    u8 mUnk1508[0x1BA0 - 0x1508];  // 0x1508 HUD element
    Actor* mUnk1BA0;            // 0x1BA0
    int mUnk1BA4;               // 0x1BA4
    u8 mUnk1BA8[0x1D6C - 0x1BA8];  // 0x1BA8
    Actor* mUnk1D6C;            // 0x1D6C
    u8 mUnk1D70[0x1DA8 - 0x1D70];  // 0x1D70
    int mAnimSet;               // 0x1DA8 currently active animation set
    u8 mUnk1DAC[0x264C - 0x1DAC];  // 0x1DAC
    float mUnk264C;             // 0x264C
    float mUnk2650;             // 0x2650
    u8 mUnk2654[0x26C4 - 0x2654];  // 0x2654
    PlayerAnimSet* mSet[2];     // 0x26C4
    int mUnk26CC;               // 0x26CC
    u8 mUnk26D0[0x26E0 - 0x26D0];  // 0x26D0
    int mUnk26E0;               // 0x26E0
    Actor* mHost;               // 0x26E4
    u8 mUnk26E8[0x2730 - 0x26E8];  // 0x26E8
    float mUnk2730;             // 0x2730
    float mUnk2734;             // 0x2734
    u8 mUnk2738[0x2740 - 0x2738];  // 0x2738
    int mUnk2740;               // 0x2740
    u8 mUnk2744[0x278C - 0x2744];  // 0x2744
    float mFov;                 // 0x278C
    float mFovTarget;           // 0x2790
    u8 mUnk2794[0x2798 - 0x2794];  // 0x2794
    int mUnk2798;               // 0x2798
    u8 mUnk279C[0x27D8 - 0x279C];  // 0x279C
    float mBlurA;               // 0x27D8
    float mBlurB;               // 0x27DC
    float mBlurD;               // 0x27E0
    float mBlurC;               // 0x27E4
    u8 mUnk27E8[0x27F4 - 0x27E8];  // 0x27E8
    int mUnk27F4;               // 0x27F4
    int mUnk27F8;               // 0x27F8
    u8 mUnk27FC[0x2800 - 0x27FC];  // 0x27FC
};

extern Actor* lbl_804D43E0;  // array of players (0x2800 bytes each)
extern int lbl_804D43E4;  // number of players (0 = not running)
extern int lbl_804D43F0;
extern int lbl_804D4484;
extern int lbl_804D26FC;
extern int lbl_804D27F8;
extern const float lbl_804D6280;
extern const double lbl_804D6288;

#ifdef __cplusplus
extern "C" {
#endif
int SendMsg(int to, int msg, void* data);
void fn_800C5E00(int body, int a, int b, int c, int d);
int fn_800A1BC8(int a, int b, int c, int d, int e);
int fn_800C5E6C(void);
int fn_802A1E90(void);
void fn_80093478(int anim, int a);
void fn_8016EEF4(int a);
int fn_800F9614(int camera);
void fn_8010FB1C(float dt);
int HookFalse0(void);
int fn_8004A370(float dt, Actor* p);
#ifdef __cplusplus
}
#endif
int fn_8007EF48(int a, int b, const char* name, int c, int d, int e, int f, int g, int h, int i);
int AnimBankGetSetCount(int a, int b, int c);
void AnimUnref(int anim);
void CameraSetFov(int camera, float fov);
void FontRelease(int font);
struct Player;
int PlayerMeterFull(Player* p);
void FearRegionSetup(int a, Actor* p);

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
    Actor* a;
    dt = 0.05f * dt;
    a = *pp;
    if (a->mFov < a->mFovTarget) {
        a->mFov += dt;
        if (a->mFov > a->mFovTarget) {
            a->mFov = a->mFovTarget;
        }
    } else {
        a->mFov -= dt;
        if (a->mFov < a->mFovTarget) {
            a->mFov = a->mFovTarget;
        }
    }
    CameraSetFov(a->mCamera, (float)(int)a->mFov);
}


// Message 0x2400 to the level: where a player stands.
struct PlayerPosMsg {
    int mUnk0;     // 0x00
    u32 mArg;      // 0x04
    int mUnk8;     // 0x08
    int* mPos;     // 0x0C
    int mResult;   // 0x10
};

// 0x800495BC: asks the level for the player's slot (only when *out is still unset)
extern "C" void fn_800495BC(Actor* a, u32 arg, int* out)
{
    PlayerPosMsg m;
    m.mArg = arg;
    m.mPos = (int*)((u8*)a + 0x24);
    if (out != 0 && *out == -1) {
        SendMsg(0x23000000, 0x2400, &m);
        *out = m.mResult;
    } else if (out == 0) {
        SendMsg(0x23000000, 0x2400, &m);
    }
}

// 0x80049634: is the player inside a host of kind 1
extern "C" int fn_80049634(Actor* a)
{
    if (a->mHost != 0 && (int)(a->mHost->mId >> 24) == 1) {
        return 1;
    }
    return 0;
}

// 0x80049660: same checks as the blur-hit start, without side effects
extern "C" int fn_80049660(Actor* a)
{
    int ok = PlayerInHostInline(a) == 1;
    if (ok && (int)(a->mHost->mId >> 24) != 1 && (a->mSet[1]->mHash != 0x1091C) && a->mSet[1]->mHash != 0xA7F71344 &&
        a->mSet[1]->mHash != 0x4624485 && a->mSet[1]->mHash != 0x13C05 && a->mSet[1]->mHash != 0xE0AB9B4F &&
        a->mSet[1]->mHash != 0xFFF5 && a->mSet[1]->mHash != 0x745A81AE && a->mSet[1]->mHash != 0x8FA80B9E &&
        a->mSet[1]->mHash != 0xBE90C1A && a->mSet[1]->mHash != 0x80F3B697 && a->mSet[1]->mHash != 0xF63E7769 &&
        a->mSet[1]->mHash != 0x8C3E2D96) {
        return 1;
    }
    return 0;
}

// Whether the player is inside a host (1) or not (0).
int PlayerInHost(Actor* a)
{
    if (a == 0 || lbl_804D43E4 == 0) {
        return 0;
    }
    return (a->mFlags >> 21) & 1;
}

int PadPressed(int a, int b, int c);

// First player whose pad state for this slot is 1.
static inline Actor* PlayerWithPad(int pad)
{
    int j;
    for (j = 0; j < lbl_804D43E4; j++) {
        if (lbl_804D43E0[j].mPadState[pad] == 1) {
            return &lbl_804D43E0[j];
        }
    }
    return 0;
}

// 0x80049778: first player that pressed the button on a pad of this slot
extern "C" Actor* fn_80049778(int button, int pad)
{
    int i;
    for (i = 0; i < lbl_804D43E4; i++) {
        if (PlayerWithPad(pad) != 0 && PadPressed(i, 0, button) == 1) {
            return &lbl_804D43E0[i];
        }
    }
    return 0;
}

// 0x80049840: player by its second id
extern "C" Actor* fn_80049840(int id)
{
    int i;
    for (i = 0; i < lbl_804D43E4; i++) {
        if (id == lbl_804D43E0[i].mUnk58C) {
            return &lbl_804D43E0[i];
        }
    }
    return 0;
}

// Player that currently possesses the given actor id.
Actor* PlayerGetPossessor(int id)
{
    int i;
    for (i = 0; i < lbl_804D43E4; i++) {
        if (id == (int)lbl_804D43E0[i].mId) {
            return &lbl_804D43E0[i];
        }
    }
    return 0;
}

// 0x800498D0: actor id of a player from its address (-1 if it is not one)
extern "C" u32 fn_800498D0(Actor* a)
{
    int i = a - lbl_804D43E0;
    if (i >= 0 && i < lbl_804D43E4) {
        return (i & 0xFFFFFF) | 0x2000000;
    }
    return 0xFFFFFFFF;
}

// 0x80049914: the player at the current slot
extern "C" Actor* fn_80049914(void)
{
    return lbl_804D43E0 + lbl_804D26FC;
}

int PlayerHud(Actor* a)
{
    return a->mHud;
}

int ActorCount(void)
{
    return lbl_804D43E4;
}

extern "C" void fn_80049938(int v)
{
    lbl_804D4484 = v;
}

extern "C" int fn_80049940(void)
{
    return lbl_804D43F0;
}

// The player that owns the current body (falls back to the first one in the level modes that allow it).
Actor* GetPlayer(void)
{
    int body = fn_800C5E6C();
    int i;
    for (i = 0; i < lbl_804D43E4; i++) {
        if (body == lbl_804D43E0[i].mBody) {
            return &lbl_804D43E0[i];
        }
    }
    if ((lbl_804D43E4 > 0 && fn_802A1E90() == 7) || fn_802A1E90() == 8) {
        return lbl_804D43E0;
    }
    if (lbl_804D43E4 == 1) {
        return lbl_804D43E0;
    }
    return 0;
}

// Data the host kinds expose that the player reads (host kind 1 at 0x84, kind 0x13 at 0xC4).
struct HostKind1 {
    u8 mUnk00[0x44];
    float mFov;       // 0x44
    u8 mUnk48[4];     // 0x48
    float mUnk4C;     // 0x4C
};
struct HostKind19 {
    u8 mUnk00[0x104];
    int mFov;
};
struct HostObj {
    u8 mUnk00[0x38];
    u32 mId;
    u8 mUnk3C[0x84 - 0x3C];
    HostKind1* mKind1;
    u8 mUnk88[0xC4 - 0x88];
    HostKind19* mKind19;
};

extern const float lbl_804D629C;

// Keyed query with an integer reply (message 0x41E).
static inline void QueryInt(int target, u32 hash, int* out)
{
    QueryMsg q;
    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    SendMsg(target, 0x41E, &q);
    *out = *(int*)&q.result;
}

// Keyed command with an integer argument (message 0x41D).
static inline void SendInt(int target, u32 hash, int value)
{
    QueryMsg q;
    q.hash = hash;
    *(int*)&q.result = value;
    q.unk8 = 0;
    SendMsg(target, 0x41D, &q);
}

extern "C" void fn_800F7AF0(int a, int b);

// 0x80048428: applies the player's two keyed settings
extern "C" void fn_80048428(Actor** pp)
{
    Actor* a = *pp;
    int first;
    int second;
    QueryInt(a->mId, 0x2DA8293C, &first);
    QueryInt(a->mId, 0xDFF3097A, &second);
    fn_800F7AF0(first, second);
    a->mUnk2650 = 0.0f;
}

// 0x8004853C: ends the blur/font effects and resets the field of view for the current host
extern "C" void fn_8004853C(Actor** pp)
{
    Actor* a = *pp;
    HostObj* host;
    int kind;
    if (a->mFontA != -1) {
        FontRelease(a->mFontA);
        a->mFontA = -1;
    }
    if (a->mFontB != -1) {
        FontRelease(a->mFontB);
        a->mFontB = -1;
    }
    if (a->mUnk14B8 != -1) {
        fn_8016EEF4(a->mUnk14B8);
        a->mUnk14B8 = -1;
    }
    a->mFov = (float)fn_800F9614(a->mCamera);
    host = (HostObj*)a->mHost;
    kind = (int)(host->mId >> 24);
    if (kind == 1) {
        a->mFovTarget = host->mKind1->mFov;
    } else if (kind == 0x13) {
        a->mFovTarget = (float)host->mKind19->mFov;
    } else {
        a->mFovTarget = lbl_804D629C;
    }
}

// 0x8004A254: per-frame update of every player, then the shared HUD and the "MainMenu" check
extern "C" int fn_8004A254(float dt)
{
    int i;
    int off;
    int full;
    struct {
        int mUnk0;
        const char* mText;
    } msg;
    off = 0;
    for (i = 0; i < lbl_804D43E4; i++) {
        Actor* p = (Actor*)((u8*)lbl_804D43E0 + off);
        if (p->mFlags4C & 0x400) {
            p->mFlags4C &= ~0x400;
        } else {
            fn_8004A370(dt, p);
        }
        off += sizeof(Actor);
    }
    fn_8010FB1C(dt);
    if (HookFalse0() == 0) {
        i = 0;
        full = 0;
        off = i;
        for (; i < lbl_804D43E4; i++) {
            if (PlayerMeterFull((Player*)((u8*)lbl_804D43E0 + off)) != 0) {
                full++;
            }
            off += sizeof(Actor);
        }
        if (full == lbl_804D43E4) {
            msg.mText = "MainMenu";
            SendMsg(0x38000000, 0x5600, &msg);
        }
    }
    if (lbl_804D43E4 == 1) {
        FearRegionSetup(0, lbl_804D43E0);
    }
    return 1;
}

void PlayerInitGunState(void* p);
void PlayerSetActionMode(void* p, int mode);
float Fn802811A4(void);
struct Hud;
struct HudElem;
void HudElemInit(Hud* hud, HudElem* elem);
extern "C" int fn_802A1D04(void);
extern "C" void fn_8001BA6C(void* p);
extern int lbl_804D4478;

// 0x8004B2A8: resets every player for a new level
extern "C" void fn_8004B2A8(void)
{
    Actor* p;
    int i;
    for (i = 0; i < lbl_804D43E4; i++) {
        struct {
            Actor* mFrom;
            Actor* mPlayer;
            int mUnk08[4];
        } m;
        p = &lbl_804D43E0[i];
        PlayerInitGunState(p);
        PlayerSetActionMode(p, 3);
        m.mFrom = p;
        m.mPlayer = p;
        p->mHandler(0x800, &m);
        if (fn_802A1D04() == 0) {
            float v = 6800.0f * (1.0003f * Fn802811A4());
            p->mUnk1500 = v;
            p->mUnk1504 = v;
            HudElemInit((Hud*)p, (HudElem*)&p->mUnk1508);
        }
    }
    fn_8001BA6C((u8*)p + 0x24);
    lbl_804D4478 = 0;
}

// 0x8004BD64: starts the player system with no players and registers its two level callbacks
extern "C" int fn_80054174(int i, int a, Actor** out);
extern "C" void fn_800519D8(int kind, void* data);
extern "C" void fn_80026680(int kind, void* fn, int arg);
extern "C" void fn_80026660(int kind, void* fn, int arg);
extern "C" int fn_8004BD64(void)
{
    lbl_804D43E4 = 0;
    fn_80026680(2, (void*)fn_800519D8, 0);
    fn_80026660(2, (void*)fn_80054174, 0);
    return 1;
}

// 0x8004FD70: releases the two fonts and the effect of the blur
extern "C" void fn_8004FD70(Actor* a)
{
    if (a->mFontA != -1) {
        FontRelease(a->mFontA);
        a->mFontA = -1;
    }
    if (a->mFontB != -1) {
        FontRelease(a->mFontB);
        a->mFontB = -1;
    }
    if (a->mUnk14B8 != -1) {
        fn_8016EEF4(a->mUnk14B8);
        a->mUnk14B8 = -1;
    }
}

extern int lbl_804D43F0;
extern float lbl_804D2718;
extern float lbl_804D271C;

// 0x800507B4: state enter, shows the "White Out" font
extern "C" void fn_800507B4(Actor* p)
{
    if (p->mFontC != -1) {
        FontRelease(p->mFontC);
        p->mFontC = -1;
    }
    p->mFontC = fn_8007EF48(p->mBody, 1, "White Out", 0, 0, 0xC, 0xC, 0xC, 0, (int)p);
    if (lbl_804D43F0 != 0) {
        p->mUnk264C = lbl_804D271C;
    } else {
        p->mUnk264C = lbl_804D2718;
    }
}

// 0x80050850: state exit, releases the third font
extern "C" int fn_80050850(int a, int b, Actor* p)
{
    if (p->mFontC != -1) {
        FontRelease(p->mFontC);
        p->mFontC = -1;
    }
    return 0;
}

void GhostNotify(void* p, int a, int b);
extern "C" void fn_800FC764(u32 id, int a);

// Context of a player query: the player first.
struct PlayerQueryArgs {
    u8 mUnk00[0xC];
    float mUnk0C;     // 0x0C
};
struct PlayerQuery {
    Actor* mPlayer;   // 0x00
    PlayerQueryArgs* mUnk04;  // 0x04
    int mUnk08;       // 0x08
    int mUnkC;        // 0x0C
    int mUnk10;       // 0x10
    int mUnk14;       // 0x14
    int mUnk18;       // 0x18
    u8 mUnk1C[0x38 - 0x1C];
    int mId;          // 0x38
};

// 0x80051188
extern "C" int fn_80051188(PlayerQuery* q, int* out)
{
    Actor* p = q->mPlayer;
    if (p != 0) {
        Msg400 m;
        if ((int)fn_800498D0(p) != -1 && !PlayerInHostInline(p)) {
            return 0;
        }
        m.actor = p;
        q->mPlayer->mHandler(0x400, &m);
        out[0] = q->mId;
        out[1] = q->mUnkC;
        out[2] = m.ownerId;
        return 1;
    }
    out[0] = q->mId;
    out[1] = q->mUnkC;
    out[2] = -1;
    return 1;
}

// 0x800515DC: state exit, ends the possession effects
extern "C" int fn_800515DC(int a, int b, Actor* p)
{
    if (p->mFontD != -1) {
        p->mFontD = -1;
    }
    if (lbl_804D43F0 != 0) {
        int host = -1;
        if (p->mHost != 0 && p->mUnk2798 == -1) {
            host = p->mHost->mId;
        }
        GhostNotify(&p->mUnk26E0, 0, 1);
        fn_800FC764(p->mId, 0);
        if (host != -1) {
            fn_800FC764(host, 0);
        }
        SendInt(p->mUnk26CC, 0x7C18FE9E, -1);
        SendInt(p->mUnk26CC, 0x33032FA9, 0);
    }
    return 0;
}

// 0x80051744: state exit, releases the fourth font
extern "C" int fn_80051744(int a, int b, Actor* p)
{
    if (p->mFontD != -1) {
        FontRelease(p->mFontD);
        p->mFontD = -1;
    }
    return 0;
}

// Per-pad options (0x18 bytes each, one per player).
struct PadOptions {
    int mUnk0;   // 0x00
    int mUnk4;   // 0x04
    int mUnk8;   // 0x08
    int mUnkC;   // 0x0C
    int mUnk10;  // 0x10
    int mUnk14;  // 0x14
};
extern PadOptions lbl_8041FFD8[4];

// Player start (0x54 bytes) of the level file.
struct PlayerStart {
    u8 mUnk00[0xC];   // 0x00
    int mUnk0C;       // 0x0C
    int mUnk10;       // 0x10
    int mUnk14;       // 0x14
    int mUnk18;       // 0x18
    int mUnk1C;       // 0x1C
    u8 mUnk20[0x18];  // 0x20
    u8 mUnk38[0xC];   // 0x38
    u8 mUnk44[0xC];   // 0x44
    u8 mUnk50[4];     // 0x50
};
extern PlayerStart* lbl_804D43DC;
extern int lbl_804D43E8;
extern int lbl_804D43EC;
extern "C" void FileFlushBuffer(void* p, int size);
extern "C" void fn_800070C0(void* p);
extern "C" void fn_800070E0(void* p, int size);

// 0x800519D8: level-load callback, reads the player starts
extern "C" void fn_800519D8(int kind, void* data)
{
    if (kind == 2) {
        short* h = (short*)data;
        fn_800070E0(h, 0x10);
        if (h[0] == 8) {
            int n;
            int i;
            lbl_804D43DC = (PlayerStart*)((u8*)data + 0x10);
            n = h[1] > 4 ? 4 : h[1];
            lbl_804D43E4 = n;
            lbl_804D43E8 = n;
            lbl_804D43EC = n;
            for (i = 0; i < lbl_804D43E4; i++) {
                PlayerStart* s = &lbl_804D43DC[i];
                FileFlushBuffer(s, 0xC);
                fn_800070C0(&s->mUnk10);
                fn_800070C0(&s->mUnk14);
                FileFlushBuffer(s->mUnk20, 0x18);
                FileFlushBuffer(s->mUnk38, 0xC);
                FileFlushBuffer(s->mUnk44, 0xC);
                fn_800070C0(&s->mUnk18);
                fn_800070C0(&s->mUnk1C);
                fn_800070C0(&s->mUnk0C);
            }
        }
    }
}

// 0x80053F84
extern "C" void fn_80053F84(int v)
{
    lbl_8041FFD8[0].mUnk14 = v;
}

// 0x80053F94
extern "C" void fn_80053F94(int i, int on)
{
    if (on) {
        lbl_8041FFD8[i].mUnkC = 1;
    } else {
        lbl_8041FFD8[i].mUnkC = 0;
    }
}

// 0x80053FD4
extern "C" void fn_80053FD4(int i, int v)
{
    lbl_8041FFD8[i].mUnk8 = v;
}

extern "C" u32 fn_800E1BD0(void);
extern "C" void fn_800E1B5C(u32 mask);

// 0x80053FEC: turns a player's pad on or off in the active pad mask
extern "C" void fn_80053FEC(int i, int on)
{
    u32 mask = fn_800E1BD0();
    if (on == 1) {
        lbl_804D43E0[i].mUnk27F8 = 1;
        fn_800E1B5C(mask | (1 << lbl_804D43E0[i].mUnk58C));
    } else if (on == 0) {
        lbl_804D43E0[i].mUnk27F8 = 0;
        fn_800E1B5C(mask & ~(1 << lbl_804D43E0[i].mUnk58C));
    }
}

// 0x80054094
extern "C" void fn_80054094(int i, int on)
{
    if (on == 1) {
        lbl_804D43E0[i].mFlags254 |= 1;
        lbl_8041FFD8[i].mUnk4 = 1;
    } else {
        lbl_804D43E0[i].mFlags254 &= ~1;
        lbl_8041FFD8[i].mUnk4 = 0;
    }
}

// 0x80054104
extern "C" void fn_80054104(int i, int on)
{
    if (on == 1) {
        lbl_804D43E0[i].mFlags584 |= 1;
        lbl_8041FFD8[i].mUnk10 = 1;
    } else {
        lbl_804D43E0[i].mFlags584 &= ~1;
        lbl_8041FFD8[i].mUnk10 = 0;
    }
}

extern "C" int fn_800541C0(int a, Actor** out);

// 0x80054174: level callback, runs the player query for player i
extern "C" int fn_80054174(int i, int a, Actor** out)
{
    if (lbl_804D43E4 > 0) {
        *out = &lbl_804D43E0[i];
        return fn_800541C0(a, out);
    }
    return 0;
}

extern int lbl_804D4470;
extern int lbl_804D4474;

int ActorPlayAnim(int actor, u32 anim, int arg);
void* FxFind(const char* name);
extern "C" void FxStart(void* fx, int a, int b);
extern "C" void fn_80283864(int a);

// 0x8005624C: state enter, fades in from the host kind 1
extern "C" float fn_8005624C(Actor* p)
{
    p->mFlags &= ~8;
    if (p->mUnk1D6C != 0 && (int)(p->mUnk1D6C->mId >> 24) == 1) {
        fn_80283864(*(int*)((HostObj*)p->mUnk1D6C)->mKind1);
        FxStart(FxFind("fadein"), 0, 0);
        ActorPlayAnim(p->mUnk1D6C->mId, 0x8B40C0DC, p->mId);
    }
    return 0.0f;
}

// Message 0xC14 (0x10 bytes).
struct MsgC14 {
    int mUnk0;        // 0x00
    u32 mArg;         // 0x04
    u32 mFrom;        // 0x08
    u32 mKey;         // 0x0C
};
// Arguments of a host action event (message 0xC06).
struct PlayerActionArgs {
    Actor* mPlayer;   // 0x00
    u32 mHash;        // 0x04
    int mUnk08;       // 0x08
    int mTarget;      // 0x0C
    int mModel;       // 0x10
};
extern "C" void fn_800C5210(int target, int msg, void* data);

static inline void SendC14(int target, u32 from, u32 arg, u32 key)
{
    if (target != -1) {
        MsgC14 m;
        m.mFrom = from;
        m.mKey = key;
        m.mArg = arg;
        SendMsg(target, 0xC14, &m);
    }
}

// 0x8005955C
extern "C" int fn_8005955C(PlayerActionArgs* args)
{
    Actor* p = args->mPlayer;
    if (p->mMode == 0x16) {
        if (args->mHash == 0x1B83833F) {
            Msg400 r;
            if (p->mUnkC4 != -1 || p->mUnk11C != -1) {
                p->mUnk7A8 = 0x16;
            } else if (p->mUnk7D0 != 0) {
                p->mUnk7A8 = 3;
            } else {
                p->mUnk7A8 = 1;
            }
            if (p->mHost != 0) {
                SendC14(p->mHost->mId, p->mId, args->mHash, 0x83A2F974);
            }
            PlayerSetActionMode(p, p->mUnk7A8);
            SendMsg(p->mUnkBC, 0x400, &r);
            args->mUnk08 = p->mUnkC0;
            args->mPlayer = r.actor;
            fn_800C5210(p->mUnkBC, 0xC06, args);
        } else {
            int inHost = PlayerInHostInline(p);
            if ((inHost && p->mModel54 == args->mModel) || (!inHost && p->mUnk5C == args->mModel)) {
                fn_800C5210(args->mTarget, 0xC06, args);
            } else {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

// 0x80059E94
extern "C" void fn_80059E94(void)
{
    lbl_804D4470 = 0;
    lbl_804D4474 = 0;
}

static inline int HostNotKind19(Actor** pp)
{
    int r;
    if (*pp == 0) {
        r = 1;
    } else if ((int)((*pp)->mId >> 24) == 0x13) {
        r = 0;
    } else {
        r = 1;
    }
    return r;
}

struct HostPos {
    Actor* mHost;     // 0x00
    u8 mUnk04[0x30 - 0x04];
    int mPos[3];      // 0x30
};

// 0x8005A420: position of the host as floats (unless it is of kind 0x13)
extern "C" int fn_8005A420(HostPos* h, float* out)
{
    if (HostNotKind19(&h->mHost)) {
        out[0] = h->mPos[0];
        out[1] = h->mPos[1];
        out[2] = h->mPos[2];
        return 1;
    }
    return 0;
}

// 0x8005A4CC: whether the host in *pp is missing or not of kind 0x13
extern "C" int fn_8005A4CC(Actor** pp)
{
    Actor* h = *pp;
    if (h == 0) {
        return 1;
    }
    return (int)(h->mId >> 24) != 0x13;
}

// 0x8005AAF8: id of the player's host (-1 if the player is not inside one)
extern "C" u32 fn_8005AAF8(Actor* a)
{
    if (a != 0 && PlayerInHostInline(a) && a->mUnk2740 == 0) {
        return a->mHost->mId;
    }
    return 0xFFFFFFFF;
}

extern int lbl_804D270C;

// A camera setting (0x5C bytes).
struct CamSetting {
    int mUnk00;
    int mUnk04;
    int mUnk08;
    float mUnk0C;
    float mUnk10;
    int mUnk14;
    int mUnk18;
    int mUnk1C;
    int mUnk20;
    int mUnk24;
    int mUnk28;
    int mUnk2C;
    int mUnk30;
    int mUnk34;
    int mUnk38;
    int mUnk3C;
    int mUnk40;
    int mUnk44;
    int mUnk48;
    float mUnk4C;
    float mUnk50;
    int mUnk54;
    int mUnk58;
};

void PlayerApplyHeldSlotState(void* p);
void PlayerSelectHeldState(void* p);

// 0x8005B010
extern "C" int fn_8005B010(Actor** pp)
{
    Actor* a = *pp;
    int first;
    int second;
    a->mFlags4C |= 1;
    a->mFlags |= 0x20000;
    PlayerApplyHeldSlotState(a);
    PlayerSelectHeldState(a);
    QueryInt(a->mId, 0x2DA8293C, &first);
    QueryInt(a->mId, 0xDFF3097A, &second);
    fn_800F7AF0(first, second);
    return 1;
}

// Arguments of a player flag change.
struct PlayerFlagArgs {
    Actor* mPlayer;   // 0x00
    int mOn;          // 0x04
    u32 mBits;        // 0x08
};
extern "C" void fn_8006B360(void* pads);
extern "C" void fn_80090390(int a, int b);

// 0x8005CC5C: sets or clears player flags; setting them resets the held item and the keyed settings
extern "C" int fn_8005CC5C(PlayerFlagArgs* args)
{
    Actor* p = args->mPlayer;
    if (args->mOn) {
        int first;
        int second;
        fn_8006B360(p->mPadState);
        p->mFlags |= args->mBits;
        p->mUnk73C = 0;
        if (p->mMode != 0x16) {
            fn_80090390(p->mUnk7C, 0);
            p->mUnk80 = -1;
            p->mUnk84 = -1;
            p->mUnk88 = -1;
            PlayerApplyHeldSlotState(p);
            PlayerSelectHeldState(p);
        }
        QueryInt(p->mId, 0x2DA8293C, &first);
        QueryInt(p->mId, 0xDFF3097A, &second);
        fn_800F7AF0(first, second);
    } else {
        p->mFlags &= ~args->mBits;
    }
    return 1;
}

// 0x8005F590
extern "C" void fn_8005F590(CamSetting* d, CamSetting s)
{
    *d = s;
}

// 0x8005F64C: sends a keyed command with an integer argument
extern "C" int fn_8005F64C(int target, u32 hash, int value)
{
    QueryMsg q;
    q.hash = hash;
    *(int*)&q.result = value;
    q.unk8 = 0;
    return SendMsg(target, 0x41D, &q);
}

// 0x8005F6EC: sends a keyed query and returns its integer reply
extern "C" int fn_8005F6EC(int target, u32 hash, int* out)
{
    QueryMsg q;
    int r;
    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    r = SendMsg(target, 0x41E, &q);
    *out = *(int*)&q.result;
    return r;
}

extern "C" int fn_800B26B8(Actor* h);
extern "C" int fn_800B22F8(Actor* h, int a);
extern "C" void fn_800B20B8(Actor* h, int a);

// 0x8005F794
extern "C" int fn_8005F794(void* a, int* data)
{
    if ((int)(((Actor*)a)->mId >> 24) == 0x13) {
        Actor* h = (Actor*)a;
        if (h->mUnk908 == 0x8CD18EE1) {
            int r = fn_800B26B8(h);
            if (r == 4 || fn_800B22F8(h, r) > 1) {
                fn_800B20B8(h, data[2]);
                return 1;
            }
        }
    }
    return 0;
}

// Message 0x409 sent to the player's target.
struct PlayerMsg409 {
    Actor* mTarget;   // 0x00
    Actor* mFrom;     // 0x04
    u32 mFromId;      // 0x08
    int mArg;         // 0x0C
    u32 mId;          // 0x10
    int mUnk14;       // 0x14
    int mUnk18[2];    // 0x18
};

int ModelAnimate(int model, char* bone, int a);
int ModelBoneCount(int model);
void AnimApply(int anim, char* bone, int a, int b);
void ActorGetMatrix(Mtx34* m, Actor* a);
void ModelSetMatrix(int model, Mtx34* m, char* bone);
extern "C" int fn_8003A1F0(int model, char* bone, int anim);
// Bounding box (min, max).
struct Box {
    Vec3 min;
    Vec3 max;
};
extern "C" void fn_8016D864(Box* out, void* in);
extern "C" int fn_8002B11C(int model, Box* box, int anim, PlayerQueryArgs* args, float f, int a, int b, int c, int d);

// Camera parameters of the player for the view.
struct PlayerViewArgs {
    Actor* mPlayer;   // 0x00
    u8 mUnk04[0xC];   // 0x04
    float mUnk10;     // 0x10
    float mUnk14;     // 0x14
    float mUnk18;     // 0x18
    float mFov;       // 0x1C
};
static const float sFovTable[5] = { 4080.0f, 5456.0f, 6800.0f, 8160.0f, 9520.0f };
void HudMsgDispatch(void* a, int msg, void* data, int b);

// Field of view the player chose in the options (message 0x41E to the HUD).
static inline void OptionFov(Actor* p, float* out)
{
    QueryMsg q;
    *(int*)&q.result = p->mId;
    q.hash = 0x0E4043D9;
    HudMsgDispatch(0, 0x41E, &q, 0);
    *out = sFovTable[*(int*)&q.result];
}

// Matrix query on the player (bone index first).
struct PlayerMtxArgs {
    Actor* mPlayer;   // 0x00
    short mBone;      // 0x04
    Placement* mBones;  // 0x08
    u8 mUnk0C[4];     // 0x0C
    Placement mMtx;   // 0x10
};
extern "C" Placement* fn_8003A3F0(int model, int a, int anim);

// 0x800605C4: matrix of one of the body's bones (the player's own matrix when the bone is out of range)
extern "C" int fn_800605C4(PlayerMtxArgs* args)
{
    Actor* p = args->mPlayer;
    Placement* m;
    if (p != 0) {
        switch (p->mUnk78) {
        case 0:
            m = fn_8003A3F0(p->mModel54, p->mUnk1400, ModelAnimate(p->mModel54, 0, 0));
            break;
        case 1:
            m = fn_8003A3F0(p->mModel54, p->mUnk1400, ModelAnimate(p->mModel54, 0, 0));
            break;
        default:
            return 0;
        }
        args->mBones = m;
        if (args->mBone >= 0 && args->mBone < p->mUnkC8) {
            args->mMtx = m[args->mBone];
        } else {
            args->mMtx = *(Placement*)p;
        }
        return 1;
    }
    return 0;
}

// 0x80060760
extern "C" int fn_80060760(PlayerViewArgs* v)
{
    Actor* p = v->mPlayer;
    v->mUnk10 = p->mUnk2730;
    v->mUnk14 = p->mUnk2734;
    v->mUnk18 = p->mUnk1500;
    if (fn_802A1D04() == 3) {
        if (p->mUnk2798 != -1) {
            OptionFov(p, &v->mFov);
        } else {
            v->mFov = 6800.0f * (1.0003f * Fn802811A4());
        }
    } else if (fn_802A1D04() != 0) {
        OptionFov(p, &v->mFov);
    } else {
        v->mFov = 6800.0f * (1.0003f * Fn802811A4());
    }
    return 1;
}

// Animation of the weapon model at its bone; inside a kind-1 host the bone follows the host's matrix.
static inline int WeaponBoneAnim(Actor* p)
{
    int model;
    int anim;
    char* bone;
    int inHost = PlayerInHostInline(p);
    model = p->mModel58;
    if (inHost) {
        bone = p->mBone1414;
    } else {
        bone = p->mBone1410;
    }
    if (p->mHost != 0 && (int)(p->mHost->mId >> 24) == 1 && ((HostObj*)p->mHost)->mKind1->mUnk4C == 0.0f) {
        Mtx34 hm;
        anim = ModelAnimate(model, bone, 1);
        AnimApply(p->mUnkD4, bone, anim, ModelBoneCount(p->mModel58));
        ActorGetMatrix(&hm, p->mHost);
        ModelSetMatrix(model, &hm, bone);
        anim = fn_8003A1F0(model, bone, anim);
    } else {
        anim = fn_8003A1F0(model, bone, ModelAnimate(model, 0, 0));
    }
    return anim;
}

// 0x80060B7C: draws the player's weapon model at its bone
extern "C" int fn_80060B7C(PlayerQuery* q)
{
    Actor* p = q->mPlayer;
    int anim;
    Box box;
    if (p->mFlags & 0x80) {
        return 0;
    }
    anim = WeaponBoneAnim(p);
    fn_8016D864(&box, p->mUnkA0C);
    return fn_8002B11C(p->mModel58, &box, anim, q->mUnk04, q->mUnk04->mUnk0C, q->mUnk08, q->mUnkC, q->mUnk14, q->mUnk18);
}

// Arguments of the weapon-effect draw.
struct PlayerDrawArgs {
    Actor* mPlayer;   // 0x00
    int mUnk04;       // 0x04
    int mUnk08;       // 0x08
    int mUnk0C;       // 0x0C
    int mUnk10;       // 0x10
    int mUnk14;       // 0x14
    u32 mFlags;       // 0x18
    u8 mUnk1C[0x2C - 0x1C];
    int mUnk2C;       // 0x2C
    int mUnk30;       // 0x30
};
extern "C" int fn_8002D4E0(int model, Box* box, Mtx34* m, int anim, int a, int b, int c, int d, int e, u32 flags,
                           int f, int g);

// 0x80060CF0: draws an effect at the player's weapon bone
extern "C" int fn_80060CF0(PlayerDrawArgs* args)
{
    Actor* p = args->mPlayer;
    int anim;
    Box box;
    Mtx34 m;
    if (p->mFlags & 0x80) {
        return 0;
    }
    anim = WeaponBoneAnim(p);
    fn_8016D864(&box, p->mUnkA0C);
    ActorGetMatrix(&m, p);
    return fn_8002D4E0(p->mModel58, &box, &m, anim, args->mUnk04, args->mUnk08, args->mUnk0C, args->mUnk10,
                       args->mUnk14, (args->mFlags | 0x20000000) & ~0x10000000, args->mUnk2C, args->mUnk30);
}

// Arguments of a hit test against the player's weapon bones.
struct PlayerHitArgs {
    Actor* mPlayer;   // 0x00
    void* mBounds;    // 0x04
    Actor* mFrame;    // 0x08
};
void Vec3Copy(Vec3* d, const Vec3* s);
int ModelBoneHitBox(int box, int m, int v, int bone);

// 0x80060E94: whether the box hits one of the weapon bones
extern "C" int fn_80060E94(PlayerHitArgs* args)
{
    Actor* p = args->mPlayer;
    Placement* bones;
    int i;
    Vec3 v;
    Box box;
    Mtx34 m;
    if (p->mBoneCount120 >= 0xFE) {
        return 0;
    }
    if (p->mFlags & 0x80) {
        return 0;
    }
    if (p->mBoneCount120 > 1) {
        bones = (Placement*)WeaponBoneAnim(p);
        ActorGetMatrix(&m, args->mFrame);
        fn_8016D864(&box, args->mBounds);
        for (i = 0; i < p->mBoneCount120; i++) {
            Vec3Copy(&v, &p->mBoneOffsets[i]);
            if (ModelBoneHitBox((int)&box, (int)&m, (int)&v, (int)&bones[i])) {
                return 1;
            }
        }
        return 0;
    }
    return 1;
}

// 0x80064418
extern "C" float fn_80064418(Actor* p)
{
    PlayerMsg409 m;
    m.mTarget = p->mUnk1BA0;
    m.mArg = p->mUnk1BA4;
    m.mFrom = p;
    m.mFromId = fn_800498D0(p);
    m.mId = m.mFromId;
    m.mUnk14 = 0;
    ((int (*)(int, void*))*(int*)((u8*)m.mTarget + 0x30))(0x409, &m);
    return 0.0f;
}

// 0x800644B4: attaches a model to one of the player's bones (0: body, 1: weapon)
extern "C" void fn_800644B4(Actor* p, int which, int model, int alt)
{
    char* bone;
    int anim;
    if (which == 0) {
        if (alt) {
            bone = p->mBone1408;
        } else {
            bone = p->mBone1404;
        }
        anim = ModelAnimate(p->mModel54, 0, 0);
        fn_8003A1F0(model, bone, anim);
    } else if (which == 1) {
        bone = alt ? p->mBone1414 : p->mBone1410;
        if (p->mHost != 0 && (int)(p->mHost->mId >> 24) == 1 && ((HostObj*)p->mHost)->mKind1->mUnk4C == 0.0f) {
            Mtx34 m;
            anim = ModelAnimate(p->mModel58, bone, 1);
            AnimApply(p->mUnkD4, bone, anim, ModelBoneCount(p->mModel58));
            ActorGetMatrix(&m, p->mHost);
            ModelSetMatrix(model, &m, bone);
            fn_8003A1F0(model, bone, anim);
        } else {
            fn_8003A1F0(model, bone, ModelAnimate(p->mModel58, 0, 0));
        }
    }
}

// 0x80064EC4
extern "C" void fn_80064EC4(void)
{
    lbl_804D270C = -1;
}

// Sound slot set (0x40 bytes).
struct SoundSlots {
    int mHandle;      // 0x00
    int mUnk4[3];     // 0x04
    int mUnk10[3];    // 0x10
    int mUnk1C[3];    // 0x1C
    int mUnk28[3];    // 0x28
    int mUnk34[3];    // 0x34
};

// 0x80064ED0
extern "C" void fn_80064ED0(SoundSlots* s)
{
    s->mUnk4[0] = -1;
    s->mUnk4[1] = -1;
    s->mUnk4[2] = -1;
}

static inline void SoundSlotsReset(SoundSlots* s)
{
    int i;
    for (i = 0; i < 3; i++) {
        s->mUnk1C[i] = -1;
        s->mUnk28[i] = 0;
        s->mUnk10[i] = 0;
        s->mUnk34[i] = 0x1000;
    }
}

// 0x80064EE4
extern "C" void fn_80064EE4(SoundSlots* s)
{
    SoundSlotsReset(s);
}

extern "C" int fn_80090EA4(int a, int b);
extern "C" void fn_80090DF8(int handle, int b);

// 0x80064F24
extern "C" void fn_80064F24(SoundSlots* s, int a, int b, int c)
{
    if (s->mHandle == -1) {
        s->mHandle = fn_80090EA4(c, b);
        SoundSlotsReset(s);
    } else {
        fn_80090DF8(s->mHandle, b);
    }
}

// 0x80064FB0: message handler, answers 0x1C00 with the handle
extern "C" int fn_80064FB0(SoundSlots* s, int msg, int* data)
{
    switch (msg) {
    case 0x1C00:
        data[2] = s->mHandle;
        return 1;
    }
    return 0;
}

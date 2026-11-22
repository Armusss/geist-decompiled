// u_801B47CC.cpp (0x801B47CC-0x801B829C): a character AI with per-state callbacks (state table at .data 0x803AAE98).
// Split from the old u_801AE50C (0x801AE50C-0x801BEC4C). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary evidence (split 2026-11-22): own .sdata2 0x804D8938-0x804D8958 (0x4330 double at 0x804D8940 and 0.0f at
// 0x804D894C, both also in the files before and after), .sdata 0x804D31C0-0x804D31C8, the 0x708-byte state table
// .data 0x803AAE98-0x803AB5A0 (it points only at functions in this range) and .bss 0x80496C98 (16 asset ids, filled
// by AiLoadAssets_801B8080). No strings.

#include "game/string.h"
#include "game/msg.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

extern "C" int SendMsg(int to, int msg, void* data);                 // 0x800265D4
extern "C" void fn_800872BC(int flags, const char* fmt, ...);        // debug log

inline void QueryGet(int* out, int target, u32 hash) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, 0x41E, &m);
    *out = *(int*)&m.result;
}

// Animation set of a character type (three words used here).
struct AiAnimSet {
    u8 pad00[0x14];
    int a14;         // 0x14
    int a18;         // 0x18
    int a1C;         // 0x1C
    int a20;         // 0x20
    int a24;         // 0x24
};

// The actor a character is currently targeting / carried by.
struct AiTarget {
    u8 pad00[0x38];
    u32 handle;      // 0x38: type in the top byte
    u8 pad3C[0x270 - 0x3C];
    u8 conditions[4]; // 0x270
};

// Current / next state of a character (a Player-style state record).
struct AiStateRec {
    int cur;         // 0x00
    int next;        // 0x04
    int unk8;        // 0x08
    int unkC;        // 0x0C
};

// Enemy character actor (descriptive guess; only the fields used in this file).
struct Actor {
    u8 pad00[0x24];
    int px;          // 0x24
    int py;          // 0x28
    int pz;          // 0x2C
    u8 pad30[0x38 - 0x30];
    int handle;      // 0x38
    u8 pad3C[0x50 - 0x3C];
    u32 flags;       // 0x50
    u8 pad54[0x58 - 0x54];
    int model;       // 0x58
    u8 pad5C[0x78 - 0x5C];
    int hold;        // 0x78
    int unk7C;       // 0x7C
    u8 pad80[0x8C - 0x80];
    void* cb8C;      // 0x8C
    u8 pad90[0x98 - 0x90];
    int anim98;      // 0x98
    u8 pad9C[0xA4 - 0x9C];
    int animFlagA4;  // 0xA4
    u8 padA8[0xC4 - 0xA8];
    int animC4;      // 0xC4
    u8 padC8[0xE4 - 0xC8];
    void* cbE4;      // 0xE4
    u8 padE8[0xF0 - 0xE8];
    int animF0;      // 0xF0
    u8 padF4[0xFC - 0xF4];
    int animFlagFC;  // 0xFC
    u8 pad100[0x11C - 0x100];
    int anim11C;     // 0x11C
    u8 pad120[0x2F4 - 0x120];
    u8 sub2F4[0x20]; // 0x2F4
    int unk314;      // 0x314
    int unk318;      // 0x318
    u32 flags31C;    // 0x31C
    u32 flags320;    // 0x320
    u8 pad324[4];
    int unk328;      // 0x328
    u8 pad32C[0x38C - 0x32C];
    float f38C;      // 0x38C
    float f390;      // 0x390
    float f394;      // 0x394
    float f398;      // 0x398
    u8 pad39C[0x638 - 0x39C];
    int unk638;      // 0x638
    u8 pad63C[0x678 - 0x63C];
    u32 flags678;    // 0x678
    u8 pad67C[0x73C - 0x67C];
    int variant;     // 0x73C
    u8 pad740[0x7A4 - 0x740];
    AiStateRec state;   // 0x7A4
    u32 flags7B4;    // 0x7B4
    u8 pad7B8[0x7D0 - 0x7B8];
    int unk7D0;      // 0x7D0
    int unk7D4;      // 0x7D4
    float unk7D8;    // 0x7D8
    int unk7DC;      // 0x7DC
    u8 pad7E0[0x14FC - 0x7E0];
    float unk14FC;   // 0x14FC
    u8 pad1500[0x1D04 - 0x1500];
    u8 unk1D04[0x1D94 - 0x1D04];
    int unk1D94;     // 0x1D94
    u8 pad1D98[0x26B0 - 0x1D98];
    int unk26B0;     // 0x26B0
    u8 pad26B4[0x26C4 - 0x26B4];
    AiAnimSet* set[7]; // 0x26C4
    u8 sub26E0[4];   // 0x26E0
    Actor* host;   // 0x26E4
    AiTarget* target; // 0x26E8
    u8 pad26EC[0x279C - 0x26EC];
    int unk279C;     // 0x279C
};

// Engine services (names in symbols.txt come from other units).
void PlayerSetValue4(void* state, int v);
void PlayerResetGunTimer(void* a);
void PlayerSelectCollisionUpdate(void* a, float dt);
extern "C" void fn_800769A0(void* a, float dt);
int PlayerInHost(Actor* a);
void PlayerSetHeldSlotIndex(void* a, int slot);
void PlayerSetHeldSlotMode(void* a, int mode);
void PlayerSlideByVar(void* a);
void PlayerDispatchMotionUpdate(void* a, int mode, float dt);
extern "C" int fn_802A1D04();
extern "C" int fn_800C5210(int to, int msg, void* data);

struct MsgQuery {
    int result;      // 0x00
    u32 hash;        // 0x04
    int pad08;       // 0x08
    int handle;      // 0x0C
};
extern "C" void fn_800699A8(void* a);
void PlayerSetActionMode(void* a, int mode);
void DashUpdate(Actor* a);
int HasCondition(void* conditions, int id);
void ActorPlayAnim(int a, u32 b, int c);
int ModelBoneName(int model);
int ModelBoneNodeChain(int model, int bone, int* out, int n);
int Fn800F7DEC(int a, int b, int c, int d, void* e);
int Fn800F7F5C(int a, int b, int c, int d, void* e, float f);
extern "C" int fn_80175EFC(int v);
extern "C" void fn_80155A84(void* a, void* b, int c);
extern "C" int fn_800A1BC8(int a, int b, int c, int d, int e);
extern "C" void* fn_800A384C(void* t);
extern "C" int fn_800498D0(Actor* a);
extern "C" void fn_80048BA8(Actor* a, void* b, int c);
extern "C" void fn_80050EB0(Actor* a);
extern "C" int fn_800F7994(int a, u32 b);
extern "C" int FxStart(void* fx, int a, int b);

// Animation lookup table: one row per animation kind, one slot per character variant (data of another unit).
struct AnimRow {
    void (*cb00)(Actor*);  // 0x00
    void (*cb04)(Actor*);  // 0x04
    void (*msg)(Actor*);   // 0x08
    int byVariant[15];     // 0x0C
};
extern AnimRow lbl_803AAE98[];
extern AnimRow lbl_803A00D0[];
int PlayerGunAction(int action, void* p);
extern int lbl_80496C98[16];
u32 AssetFind(u32 table, u32 key);
extern "C" int fn_801B7B88(int action, void* p);

// Per-enemy data block (descriptive guess; only the fields used here).
struct EnemyData {
    int f0;          // 0x00
    int state;       // 0x04
    int f8;          // 0x08
    u8 padC[0x14 - 0xC];
    int f14;         // 0x14
    u8 pad18[0x24 - 0x18];
    float f24;       // 0x24
    u8 pad28[0x3C - 0x28];
    float f3C;       // 0x3C
    u8 pad40[0x50 - 0x40];
    float f50;       // 0x50
};

// Behaviour object with a data block at 0x130 (descriptive guess).
struct EnemyObj {
    u8 pad00[0x130];
    EnemyData* data; // 0x130
    u8 pad134[0x270 - 0x134];
    u8 cond[0x18];   // 0x270
    u8 pad288[0x5B8 - 0x288];
    float f5B8;      // 0x5B8
    u8 pad5BC[0x5D0 - 0x5BC];
    int i5D0;        // 0x5D0
    u8 pad5D4[0x5EC - 0x5D4];
    void* link;      // 0x5EC
    u8 pad5F0[0x91C - 0x5F0];
    float f91C;      // 0x91C
};

inline float Lerp(float lo, float hi, float t) {
    float d = hi - lo;
    return lo + d * t;
}
extern int lbl_804D31C4;
extern int lbl_804D31C0;
extern "C" int fn_800F7994(int a, u32 b);
extern "C" int FxStart(void* fx, int a, int b);
extern "C" int rand();
int Rand();
void ClearStateMachine(void* o);
void SetStateMachineState(void* o, u32 state);
void ConditionUpdateBit(void* c, int bit, int on);
extern "C" void fn_80118084(void* link, u32 hash, void* data);
extern "C" int fn_800B949C(u32 hash, int* out, int n, int x);
extern "C" int fn_801B1F18(EnemyObj* o);
extern "C" int fn_801B1594(EnemyObj* o, EnemyData* d);

// Picks the next goal: a random entry of a table or the result of the planner.
inline int EnemyPickGoal(EnemyObj* o) {
    int list[16];
    int goal;
    if (0.5f * (o->f5B8 * (float)o->i5D0) > (float)rand() / 32767.0f) {
        goal = fn_801B1F18(o);
    } else {
        int n = fn_800B949C(0x6D8FDBFF, list, 0x10, 1);
        if (n == 0) {
            goal = -1;
        } else {
            int r = Rand();
            goal = list[r - (r / n) * n];
        }
    }
    return goal;
}

// Enters the wander mode (state 4); shared tail of several mode-enter functions.
inline void EnemyEnterWanderBody(EnemyObj* o) {
    EnemyData* d = o->data;
    d->state = 4;
    d->f0 = 0;
    d->f8 = EnemyPickGoal(o);
    float speed = 1000.0f;
    ClearStateMachine(o);
    SetStateMachineState(o, 0xF1C37C5C);
    fn_80118084(o->link, 0x315C41D, &d->f8);
    fn_80118084(o->link, 0x33FA508, &speed);
    fn_80118084(o->link, 0x99A725C, (void*)1);
}

extern float lbl_804D31A8;
extern float lbl_804D31AC;
extern "C" void fn_8016EEF4(int v);
extern "C" void fn_80073E80(Actor* a);
extern "C" void fn_800B4C50(float a, float b, float* pos, int c, int d, int e);
extern "C" void fn_8016D68C(void* dst, void* src);
extern "C" int fn_80116E08(Actor* a);
extern "C" int fn_800E0DBC(void* v);

void AiState801B47CC(Actor* a, float dt) {
    a->unk638 = 0;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->animC4 == -1) {
        if (a->unk7D0 != 0) {
            PlayerSetValue4(&a->state, 3);
        } else {
            PlayerSetValue4(&a->state, 1);
        }
    } else {
        PlayerSetValue4(&a->state, 0x16);
    }
}

void AiState801B4868(Actor* a) {
    a->cb8C = 0;
    if (a->animC4 != -1) {
        a->anim98 = a->animC4;
    }
    if (a->anim11C == -1) {
        return;
    }
    a->animF0 = a->anim11C;
}

void AiState801B4894(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    PlayerSetValue4(&a->state, 0x15);
    if (a->unk279C == -1 || fn_80175EFC(a->unk279C) != 0) {
        PlayerSetValue4(&a->state, 3);
        fn_80155A84(a->sub26E0, a->target, 1);
    }
}

void AiState801B4914(Actor* a) {
    AiTarget* t;
    AiAnimSet* set;
    int ride;

    ride = 0;
    t = a->target;
    if ((int)(t->handle >> 24) == 0x13) {
        set = (AiAnimSet*)fn_800A384C(t);
        if (HasCondition(t->conditions, 0xB) != 0) {
            ride = 1;
        }
    }
    PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_803AAE98[21].byVariant[v], ride, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x19;
    a->animF0 = fn_800A1BC8(set->a14, set->a1C, lbl_803AAE98[21].byVariant[v], ride, 0);
    a->animFlagFC = 0x19;
    PlayerSetValue4(&a->state, 0x15);
}

void AiState801B4A10(Actor* a, float dt) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    PlayerSetValue4(&a->state, 3);
    PlayerInHost(a);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
}

void AiState801B4AC0(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->hold = 0;
    a->flags &= ~0x20000;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803AAE98[20].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803AAE98[20].byVariant[v], 0, 0);
    a->animFlagFC = 1;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg(a->host->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(a, 0);
        if (a->anim98 == -1) {
            PlayerSetActionMode(a, 1);
        }
    } else {
        PlayerSetHeldSlotIndex(a, 1);
        if (a->anim98 == -1) {
            PlayerSetActionMode(a, 3);
        }
    }
}

void AiState801B4C30(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    if (a->flags7B4 & 1) {
        a->flags7B4 &= ~1;
    } else {
        a->flags |= 0x20000;
        a->hold = 1;
    }
}

void AiState801B4C8C(Actor* a) {
    int idx;
    AiTarget* t;
    AiAnimSet* set;
    int ride;

    ride = 0;
    t = a->target;
    if ((int)(t->handle >> 24) == 0x13) {
        set = (AiAnimSet*)fn_800A384C(t);
        if (HasCondition(t->conditions, 0xB) != 0) {
            ride = 1;
        }
    }
    idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_803AAE98[19].byVariant[v], ride, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x19;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803AAE98[19].byVariant[v], 0, 0);
    a->animFlagFC = 0x19;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg(a->target->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(a, 0);
    } else {
        PlayerSetHeldSlotIndex(a, 1);
    }
}

void AiState801B4DEC(Actor* a) {
    fn_80155A84(a->sub26E0, a->target, 1);
}

void AiState801B4E18(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    a->flags |= 0x20000;
    a->hold = 1;
    PlayerSetValue4(&a->state, 0x13);
}

void AiState801B4E74(Actor* a) {
    AiAnimSet* set;
    int idx = PlayerInHost(a);
    AiTarget* t = a->target;
    int v = a->variant;
    int ride = 0;
    if ((int)(t->handle >> 24) == 0x13) {
        set = (AiAnimSet*)fn_800A384C(t);
        if (HasCondition(t->conditions, 0xB) != 0) {
            ride = 1;
        }
    }
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_803AAE98[18].byVariant[v], ride, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x59;
    AiAnimSet* set2 = a->set[idx];
    a->animF0 = fn_800A1BC8(set2->a14, set2->a1C, lbl_803AAE98[18].byVariant[v], 0, 0);
    a->animFlagFC = 0x59;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg(a->target->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(a, 0);
    } else {
        PlayerSetHeldSlotIndex(a, 1);
    }
}

void AiState801B4FD0(Actor* a) {
    a->flags &= ~0x40000;
}

void AiState801B4FE0(Actor* a, float dt) {
    int d1;
    int d2;
    QueryGet(&d1, a->handle, 0x2DA8293C);
    QueryGet(&d2, a->handle, 0xDFF3097A);
    PlayerSetValue4(&a->state, 0x11);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerDispatchMotionUpdate(a, 0, dt);
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    if (a->unk318 == 0) {
        PlayerSetValue4(&a->state, 3);
        if (a->state.cur == 0x11 || a->state.cur == 0x10) {
            fn_80048BA8(a, a->unk1D04, 1);
            Fn800F7F5C(id1, id2, 0x9791AF51, fn_800498D0(a), a->unk1D04, a->unk14FC);
            PlayerSetHeldSlotMode(a, 8);
        }
    } else {
        PlayerResetGunTimer(a);
    }
}

void AiState801B5264(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    a->unk7DC = Fn800F7DEC(id1, id2, 0x6FAB23FE, fn_800498D0(a), 0);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[17].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 1;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[17].byVariant[v], 0, 0);
    a->animFlagFC = 1;
    a->flags |= 0x40000;
}

void AiState801B5458(Actor* a) {
    a->flags &= ~0x40000;
}

void AiState801B5468(Actor* a, float dt) {
    int d1;
    int d2;
    QueryGet(&d1, a->handle, 0x2DA8293C);
    QueryGet(&d2, a->handle, 0xDFF3097A);
    PlayerSetValue4(&a->state, 0x11);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerDispatchMotionUpdate(a, 0, dt);
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    if (a->unk318 == 0) {
        PlayerSetValue4(&a->state, 3);
        if (a->state.cur == 0x11 || a->state.cur == 0x10) {
            fn_80048BA8(a, a->unk1D04, 1);
            Fn800F7F5C(id1, id2, 0x9791AF51, fn_800498D0(a), a->unk1D04, a->unk14FC);
            PlayerSetHeldSlotMode(a, 8);
        }
    } else {
        PlayerResetGunTimer(a);
    }
}

void AiState801B56EC(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    a->unk7DC = Fn800F7DEC(id1, id2, 0x6FAB23FE, fn_800498D0(a), 0);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[16].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 1;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[16].byVariant[v], 0, 0);
    a->animFlagFC = 1;
    a->flags |= 0x40000;
}

void AiState801B58E0(Actor* a) {
    a->flags &= ~0x40000;
}

void AiState801B58F0(Actor* a, float dt) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(a, dt);
}

void AiState801B5948(Actor* a) {
    PlayerResetGunTimer(a);
    a->flags |= 0x40000;
    fn_80050EB0(a);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[13].byVariant[v], 0, 0);
    a->animFlagA4 = 0x50;
    int n;
    int e = a->unk26B0;
    if (e == -1) {
        n = 8;
    } else {
        int bone = ModelBoneName(a->model);
        int r = ModelBoneNodeChain(a->model, bone, lbl_80496C98, 0x10);
        if (r == -1) {
            n = 8;
        } else {
            n = r;
        }
    }
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[13].byVariant[v], n << 1, 0);
    a->cbE4 = (void*)fn_801B7B88;
    a->animFlagFC = 0x50;
}

void AiState801B5AA0(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    PlayerSetValue4(&a->state, 3);
}

void AiState801B5B3C(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    fn_80048BA8(a, a->unk1D04, 1);
    Fn800F7F5C(id1, id2, 0xBC78F550, fn_800498D0(a), a->unk1D04, a->unk14FC);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[11].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x11;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[11].byVariant[v], 0, 0);
    a->animFlagFC = 0x11;
}

void AiState801B5D34(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    PlayerSetValue4(&a->state, 3);
}

void AiState801B5DD0(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    fn_80048BA8(a, a->unk1D04, 1);
    Fn800F7F5C(id1, id2, 0x8FDD7599, fn_800498D0(a), a->unk1D04, a->unk14FC);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[10].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x11;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[10].byVariant[v], 0, 0);
    a->animFlagFC = 0x11;
}

void AiState801B5FC8(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    PlayerSetValue4(&a->state, 1);
}

void AiState801B6064(Actor* a) {
    int id;
    QueryGet(&id, a->handle, 0xDFF3097A);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[9].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x10;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[9].byVariant[v], 0, 0);
    a->animFlagFC = 0x10;
    lbl_804D31C4 = fn_800F7994(id, 0x9D1C245C);
    FxStart((void*)lbl_804D31C4, 0, 0);
}

void AiState801B61E4(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if ((a->flags678 & 0x80) && a->variant != 4) {
        a->flags678 &= ~0x80;
        PlayerSetActionMode(a, 3);
    } else {
        PlayerSetValue4(&a->state, 3);
    }
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    if (a->unk318 == 0) {
        PlayerSetValue4(&a->state, 3);
        if (a->state.cur == 0x11 || a->state.cur == 0x10) {
            fn_80048BA8(a, a->unk1D04, 1);
            Fn800F7F5C(id1, id2, 0x9791AF51, fn_800498D0(a), a->unk1D04, a->unk14FC);
            PlayerSetHeldSlotMode(a, 8);
        }
    } else {
        PlayerResetGunTimer(a);
    }
}

void AiState801B640C(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[8].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x11;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[8].byVariant[v], 0, 0);
    a->animFlagFC = 0x11;
    PlayerSlideByVar(a);
}

void AiState801B6508(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    PlayerResetGunTimer(a);
    PlayerSetValue4(&a->state, 7);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    if (a->flags31C == 0) {
        int id1;
        int id2;
        QueryGet(&id1, a->handle, 0x2DA8293C);
        QueryGet(&id2, a->handle, 0xDFF3097A);
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0x238E87F4, fn_800498D0(a), a->unk1D04, a->unk14FC);
        if (a->state.cur == 7) {
            PlayerSetActionMode(a, 3);
        } else {
            PlayerSetHeldSlotMode(a, 3);
        }
    } else {
        PlayerResetGunTimer(a);
    }
    ActorPlayAnim(a->handle, 0x8B390117, a->handle);
    float pos[3];
    pos[0] = (float)a->px;
    pos[1] = (float)a->py;
    pos[2] = (float)a->pz;
    fn_800B4C50(201.0f, 5000.0f, pos, a->handle, 2, 0x12);
}

void AiState801B6790(Actor* a) {
    int idx = PlayerInHost(a);
    int v4 = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[7].byVariant[v4], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 1;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[3].byVariant[v4], 0, 0);
    a->animFlagFC = 1;
}

void AiState801B6890(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    PlayerSetValue4(&a->state, 5);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    if ((a->flags31C & 1) && a->unk1D94 == 0) {
        int id1;
        int id2;
        QueryGet(&id1, a->handle, 0x2DA8293C);
        QueryGet(&id2, a->handle, 0xDFF3097A);
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0x0D19E338, fn_800498D0(a), a->unk1D04, a->unk14FC);
        Fn800F7F5C(id1, id2, 0x4E424290, fn_800498D0(a), a->unk1D04, a->unk14FC);
        if (a->state.cur != 3) {
            PlayerSetHeldSlotMode(a, 7);
        } else {
            PlayerSetActionMode(a, 7);
        }
    }
    float pos[3];
    pos[0] = (float)a->px;
    pos[1] = (float)a->py;
    pos[2] = (float)a->pz;
    fn_800B4C50(201.0f, 2500.0f, pos, a->handle, 2, 0x12);
}

void AiState801B6B24(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[5].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 1;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[5].byVariant[v], 0, 0);
    a->animFlagFC = 1;
}

// TODO (not drafted yet, 0x338 bytes at 0x801B6C18): game logic
extern "C" void fn_801B6C18();

void AiState801B6F50(Actor* a) {
    int d1;
    int d2;
    QueryGet(&d1, a->handle, 0x2DA8293C);
    QueryGet(&d2, a->handle, 0xDFF3097A);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[3].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 1;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[3].byVariant[v], 0, 0);
    a->animFlagFC = 1;
}

void AiState801B7100(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    PlayerSetValue4(&a->state, 3);
}

void AiState801B719C(Actor* a) {
    int id;
    QueryGet(&id, a->handle, 0xDFF3097A);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[2].byVariant[v], 0, 0);
    a->animFlagA4 = 0x10;
    a->cb8C = (void*)fn_801B7B88;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[2].byVariant[v], 0, 0);
    a->animFlagFC = 0x10;
    lbl_804D31C0 = fn_800F7994(id, 0xAFEF562B);
    FxStart((void*)lbl_804D31C0, 0, 0);
}

void AiState801B731C(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->flags678 & 0x80) {
        a->flags678 &= ~0x80;
        PlayerSetActionMode(a, 1);
    }
    PlayerSetActionMode(a, 3);
    a->unk7D0 = 0;
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    if ((a->flags31C & 1) && a->unk1D94 == 0) {
        int id1;
        int id2;
        QueryGet(&id1, a->handle, 0x2DA8293C);
        QueryGet(&id2, a->handle, 0xDFF3097A);
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0x0D19E338, fn_800498D0(a), a->unk1D04, a->unk14FC);
        Fn800F7F5C(id1, id2, 0x4E424290, fn_800498D0(a), a->unk1D04, a->unk14FC);
        if (a->state.cur != 3) {
            PlayerSetHeldSlotMode(a, 7);
        } else {
            PlayerSetActionMode(a, 7);
        }
    }
}

void AiState801B7560(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[1].byVariant[v], 0, 0);
    a->animFlagA4 = 1;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[1].byVariant[v], 0, 0);
    a->cbE4 = (void*)fn_801B7B88;
    a->animFlagFC = 1;
    a->unk7D0 = 0;
}

void AiState801B765C(Actor* a) {
    PlayerResetGunTimer(a);
}

void AiState801B767C(Actor* a) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    a->unk7D0 = 1;
    PlayerSetValue4(&a->state, 3);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    if ((a->flags31C & 1) && a->unk1D94 == 0) {
        int id1;
        int id2;
        QueryGet(&id1, a->handle, 0x2DA8293C);
        QueryGet(&id2, a->handle, 0xDFF3097A);
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0x0D19E338, fn_800498D0(a), a->unk1D04, a->unk14FC);
        Fn800F7F5C(id1, id2, 0x4E424290, fn_800498D0(a), a->unk1D04, a->unk14FC);
        if (a->state.cur != 3) {
            PlayerSetHeldSlotMode(a, 7);
        } else {
            PlayerSetActionMode(a, 7);
        }
    }
    if (a->f394 != 0.0f || a->f38C != 0.0f || a->f390 != 0.0f || a->f398 != 0.0f || a->unk328 != 0) {
        PlayerSetActionMode(a, 3);
    }
}

// TODO (not drafted yet, 0x2C4 bytes at 0x801B78C4): game logic
extern "C" void fn_801B78C4();

// TODO (not drafted yet, 0x414 bytes at 0x801B7B88): game logic

int AiState801B7F9C(Actor* a) {
    return lbl_803AAE98[a->state.cur].byVariant[14];
}

void AiTypeMsg_801B7FB8(Actor* a) {
    if (lbl_803AAE98[a->state.cur].msg) {
        lbl_803AAE98[a->state.cur].msg(a);
    }
}

void AiTypeCallback04_801B7FFC(Actor* a) {
    if (lbl_803AAE98[a->state.cur].cb04) {
        lbl_803AAE98[a->state.cur].cb04(a);
    }
}

void AiTypeCallback00_801B8040(Actor* a) {
    if (lbl_803AAE98[a->state.cur].cb00) {
        lbl_803AAE98[a->state.cur].cb00(a);
    }
}

void AiLoadAssets_801B8080() {
    lbl_80496C98[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_80496C98[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_80496C98[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_80496C98[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_80496C98[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_80496C98[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_80496C98[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_80496C98[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_80496C98[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_80496C98[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_80496C98[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_80496C98[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_80496C98[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_80496C98[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_80496C98[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_80496C98[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

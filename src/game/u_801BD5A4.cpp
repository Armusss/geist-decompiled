// u_801BD5A4.cpp (0x801BD5A4-0x801BE008): a registry of behaviour types (6-word entries: hash, init, free, update, msg, params).
// Split from the old u_801AE50C (0x801AE50C-0x801BEC4C). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary evidence (split 2026-11-22): the registry table .data 0x803AB708-0x803AB9D8 with its own empty 8-byte params
// constant (.rodata 0x803486A0), .sdata 0x804D3200-0x804D3208; no float constants. Its .rodata const sits after the
// previous file's string pool (CW puts a file's string pool after its other .rodata), so it is not part of that file.
// The next file has a second registry with a different entry layout (5 words) and its own empty params constant
// (0x803486C0).

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

// TODO (not drafted yet, 0x88 bytes at 0x801BD5A4): game logic
extern "C" void fn_801BD5A4();

// TODO (not drafted yet, 0x68 bytes at 0x801BD62C): game logic
extern "C" void fn_801BD62C();

// TODO (not drafted yet, 0xB8 bytes at 0x801BD694): game logic
extern "C" void fn_801BD694();

// TODO (not drafted yet, 0x88 bytes at 0x801BD74C): game logic
extern "C" void fn_801BD74C();

// TODO (not drafted yet, 0x438 bytes at 0x801BD7D4): game logic
extern "C" void fn_801BD7D4();

// TODO (not drafted yet, 0x30 bytes at 0x801BDC0C): game logic
extern "C" void fn_801BDC0C();

// TODO (not drafted yet, 0x168 bytes at 0x801BDC3C): game logic
extern "C" void fn_801BDC3C();

// TODO (not drafted yet, 0xC8 bytes at 0x801BDDA4): game logic
extern "C" void fn_801BDDA4();

// TODO (not drafted yet, 0x184 bytes at 0x801BDE6C): game logic
extern "C" void fn_801BDE6C();

int AiReturnFalse_801BDFF0() {
    return 0;
}

int AiReturnFalse_801BDFF8() {
    return 0;
}

void AiNoOp_801BE000() {
}

void AiNoOp_801BE004() {
}

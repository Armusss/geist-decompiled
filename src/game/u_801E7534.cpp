// u_801E7534.cpp (0x801E7534-0x801EA880): the sniper host's player states (hang / grab / seat states, the
// first-person weapon, aim, zoom and collision-view states), its zoom strings ("zoom_out", "zoom_level1") and
// its 25-entry host behaviour table gSniperHostTypes 0x803B3B60. Same layout as u_80225E34.cpp (the zoom host).
// Boundary evidence:
// - start 0x801E7534: 0x801E72E0-0x801E7534 is the "SniperRound" effect type (descriptor 0x803B3B30, its own
//   ghost-action name table + "SniperRound" at .rodata 0x80348F68, .sdata2 0x804D8EF0-0x804D8EF8). This file
//   has its own ghost-action name table at .rodata 0x80348FC0 (strings to 0x80349038), and its host table
//   0x803B3B60 starts right after the SniperRound descriptor; fn_801E7534 is the first state it lists.
// - the old u_801E72E0 / u_801E7DE0 cut at 0x801E7DE0 was wrong: gSniperHostTypes lists states from
//   fn_801E7534 to PlayerWeaponTimerEnter (0x801E926C), and .sdata2 0x804D8EF8-0x804D8F30 (one 0.0f at
//   0x804D8F00, one int-to-float double at 0x804D8EF8) is read from PlayerFirstPersonSetup (0x801E7DE0) to
//   PlayerWeaponStateFrameUpdate (0x801EA49C) with no duplicate.
// - end 0x801EA880: .bss 0x8049C1E8-0x8049C228 is read only by PlayerFirstPersonSetup and fn_801EA660;
//   fn_801EA880 starts the debug text code (.data 0x803B4284 text buffer via .sdata 0x804D3318, shared with
//   u_801EAACC's TextDebugFlush/TextSysInit). The text file's .sdata2 starts at 0x804D8F30, its .data at
//   0x803B4268 (right after this table), its .bss at 0x8049C228. fn_801EA87C (empty) is placed here; nothing
//   references it.
// Names are descriptive guesses. Status: Matching (all 44 functions and the data above).

#include "game/msg.h"
#include <dolphin/gx/GXStruct.h>


// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// Per-local-player view data block (pointed to by Actor::views).
struct PlayerView {
    u8  unk000[0x14];           // 0x000
    int field14;                // 0x014
    int field18;                // 0x018
    int field1C;                // 0x01C
    u8  unk020[0x104 - 0x20];   // 0x020
    int fov;                    // 0x104
};

struct Actor {
    u8  unk000[0x38];             // 0x000
    void* host38;                 // 0x038
    u8  unk03C[0x50 - 0x3C];      // 0x03C
    u32 flags50;                  // 0x050
    u8  unk054[0x58 - 0x54];      // 0x054
    int model;                    // 0x058
    u8  unk05C[0x7C - 0x5C];      // 0x05C
    void* field7C;                // 0x07C
    u8 pad80[0x8C - 0x80];
    int (*field8C)(int, Actor*);  // 0x08C
    u8  unk090[0x98 - 0x90];      // 0x090
    int field98;                  // 0x098
    u8  unk09C[0xA4 - 0x9C];      // 0x09C
    int fieldA4;                  // 0x0A4
    u8  unk0A8[0xE4 - 0xA8];      // 0x0A8
    int (*updateE4)(int, Actor*); // 0x0E4
    u8 padE8[0xF0 - 0xE8];
    int fieldF0;                  // 0x0F0
    u8  unk0F4[0xFC - 0xF4];      // 0x0F4
    int fieldFC;                  // 0x0FC
    u8  unk100[0x184 - 0x100];    // 0x100
    int cameraIndex;              // 0x184
    u8  unk188[0x1F0 - 0x188];    // 0x188
    float field1F0;               // 0x1F0
    u8  unk1F4[0x1F8 - 0x1F4];    // 0x1F4
    float field1F8;               // 0x1F8
    u8  unk1FC[0x248 - 0x1FC];    // 0x1FC
    int field248;                 // 0x248
    u8  unk24C[0x2F4 - 0x24C];    // 0x24C
    int zoomTable[8];             // 0x2F4 (per-slot zoom state; later slots index past it)
    int field314;                 // 0x314
    int field318;                 // 0x318
    int field31C;                 // 0x31C
    int flags320;                 // 0x320
    u32 flags324;                 // 0x324
    int flags328;                 // 0x328
    u8  unk32C[0x38C - 0x32C];    // 0x32C
    float field38C;               // 0x38C
    float field390;               // 0x390
    float field394;               // 0x394
    float field398;               // 0x398
    u8  unk39C[0x588 - 0x39C];    // 0x39C
    int field588;                 // 0x588
    u8  unk58C[0x5E8 - 0x58C];    // 0x58C
    float field5E8;               // 0x5E8
    u8  unk5EC[0x638 - 0x5EC];    // 0x5EC
    int field638;                 // 0x638
    u8  unk63C[0x678 - 0x63C];    // 0x63C
    int field678;                 // 0x678
    u8  unk67C[0x73C - 0x67C];    // 0x67C
    int weaponSlot;               // 0x73C
    u8  unk740[0x7A4 - 0x740];    // 0x740
    int state7A4;                 // 0x7A4
    int field7A8;                 // 0x7A8
    u8  unk7AC[0x7B4 - 0x7AC];    // 0x7AC
    u32 flags7B4;                 // 0x7B4
    u8  unk7B8[0x7D0 - 0x7B8];    // 0x7B8
    int field7D0;                 // 0x7D0
    int field7D4;                 // 0x7D4
    float field7D8;               // 0x7D8
    u8  unk7DC[0x7E0 - 0x7DC];    // 0x7DC
    int field7E0;                 // 0x7E0
    u8  unk7E4[0x7E8 - 0x7E4];    // 0x7E4
    float field7E8;               // 0x7E8
    float field7EC;               // 0x7EC
    u8  unk7F0[0x14FC - 0x7F0];   // 0x7F0
    float field14FC;              // 0x14FC
    u8  unk1500[0x15B4 - 0x1500]; // 0x1500
    int field15B4;                // 0x15B4
    u8  unk15B8[0x1D04 - 0x15B8]; // 0x15B8
    u8  field1D04[0x26B0 - 0x1D04]; // 0x1D04
    int field26B0;                // 0x26B0
    u8  unk26B4[0x26C4 - 0x26B4]; // 0x26B4
    PlayerView* views[4];         // 0x26C4 (per local player; element 1 is at 0x26C8)
};

extern "C" {
extern void fn_80050EB0(void*);
extern int fn_800A1BC8(int, int, int, int, int);
extern void SendMsg(void* to, int msg, void* data);
extern void fn_80048BA8(void* actor, void* stream, int flag);
extern int fn_800498D0(void* actor);
extern int fn_80049750(void* actor);
extern void fn_800769A0(void*, float);
extern int fn_80049940(void);
extern int fn_802A1D04(void);
extern void fn_80073E80(void*);
extern void fn_8008E824(void*, int);
extern float fn_800699B8(void*);
extern void fn_800699A8(void*);
extern float fn_800F95E4(int);
extern void fn_80165D9C(void*);
extern void fn_8000F60C(void);
extern void fn_8000F7E4(int, int);
extern int fn_800C5210(void*, int, void*);
extern void fn_80165DA8(void*);
extern int fn_800F7994(int, u32);
extern void FxStart(int, int, int);
extern int rand(void);
}

int FxFind(const char* name);
int WeatherGetTypeId(int, int);       // name from its own unit; here it reads a model's variant handle
void WeatherSetByTypeId(int, int, int);
int AssetFind(unsigned long, unsigned long);

int PlayerZoomLevelUpdate(Actor* actor, int slotA, int slotB);
int PlayerHeldSlotActionCheck(Actor* actor);
int PlayerGunActionMessage(int msg, void* p);
int PlayerZoomStateUpdate(Actor* actor, float dt);

void PlayerResetGunTimer(void*);
extern int PlayerInHost(Actor*);
extern int ModelBoneName(int, int);
extern int ModelBoneNodeChain(int, int, int*, int);
extern void CameraSetFov(int, float);
extern void PlayerSelectCollisionUpdate(void*, float);
extern void PlayerSetValue4(void*, int);
extern void Fn800F7F5C(int, int, int, int, void*, float);
extern int Fn800F7DEC(int, int, int, int, void*);
extern void PlayerSetHeldSlotMode(void*, int);
extern void PlayerSetActionMode(void*, int);
extern int ActorPlayAnim(int, u32, int);

// Shared "leave to weapon select" check of the first-person states: zoom-level
// update when not in a host, otherwise re-enter the weapon-select state when the
// host asks for it and the player is free. Returns 1 when a state change was made.
static inline int PlayerWeaponReenter(Actor* actor)
{
    if (fn_802A1D04() == 0) {
        return PlayerZoomLevelUpdate(actor, 0xD, 0x16);
    }
    if (actor->flags328 & 1) {
        if (PlayerInHost(actor) != 0) {
            if (!(actor->flags7B4 & 0x40000) && !(actor->flags7B4 & 0x80000)) {
                fn_80073E80(actor);
                return 1;
            }
        }
    }
    return 0;
}

// Shared weapon-model toggle of the first-person states: outside a host, flips
// the weapon model handle between its two variants while the zoom slot is held;
// inside one, runs the secondary zoom-level update. Returns 1 when it acted.
static inline int PlayerWeaponToggle(Actor* actor)
{
    if (fn_802A1D04() == 0) {
        if (actor->field318 == 1 || actor->field318 == 3) {
            if (WeatherGetTypeId(actor->field248, 1) == 0x24a738) {
                WeatherSetByTypeId(actor->field248, 1, 0x9b286e6f);
            } else {
                WeatherSetByTypeId(actor->field248, 1, 0x24a738);
            }
            return 1;
        }
    } else {
        return PlayerZoomLevelUpdate(actor, 9, 0x15);
    }
    return 0;
}

// The host/player object, as laid out in u_80222D60.cpp's player state callbacks.
struct HostInfo {
    int type;               // 0x00
    u8 pad04[0x14 - 0x04];
    int animA;              // 0x14
    int animB;              // 0x18
    int animC;              // 0x1C
};
struct Player {
    u8 pad00[0x24];
    int pos[3];             // 0x24
    int pad30[2];
    int handle;             // 0x38
    u8 pad3C[0x50 - 0x3C];
    u32 flags;              // 0x50
    u8 pad54[0x78 - 0x54];
    int unk78;              // 0x78
    u8 pad7C[0x8C - 0x7C];
    void* fn8C;             // 0x8C
    u8 pad90[0x98 - 0x90];
    int v98;                // 0x98
    u8 pad9C[0xA4 - 0x9C];
    int vA4;                // 0xA4
    u8 padA8[0xC4 - 0xA8];
    int vC4;                // 0xC4
    u8 padC8[0xF0 - 0xC8];
    int vF0;                // 0xF0
    u8 padF4[0xFC - 0xF4];
    int vFC;                // 0xFC
    u8 pad100[0x11C - 0x100];
    int v11C;               // 0x11C
    u8 pad120[0x314 - 0x120];
    int v314;               // 0x314
    u8 pad318[0x638 - 0x318];
    int v638;               // 0x638
    u8 pad63C[0x678 - 0x63C];
    u32 flags678;           // 0x678
    float x67C;             // 0x67C
    float y680;             // 0x680
    float z684;             // 0x684
    u8 pad688[0x73C - 0x688];
    int hostIndex;          // 0x73C
    u8 pad740[0x7A4 - 0x740];
    int sub7A4;             // 0x7A4: movement sub-state object
    u8 pad7A8[0x7B4 - 0x7A8];
    u32 flags7B4;           // 0x7B4
    u8 pad7B8[0x7D0 - 0x7B8];
    int v7D0;               // 0x7D0
    u8 pad7D4[0x26C4 - 0x7D4];
    HostInfo* hosts[2];     // 0x26C4 (indexed by PlayerInHost)
    u8 pad26CC[0x26E0 - 0x26CC];
    int obj26E0;            // 0x26E0
    Player* target;         // 0x26E4
    Player* held;           // 0x26E8
    u8 pad26EC[0x279C - 0x26EC];
    int v279C;              // 0x279C
};

// Per-state behaviour table of the sniper host (25 entries of 0x48 bytes), the layout of u_80225E34.cpp's
// table: enter / per-frame update / notify callbacks, one animation hash per host index, and a weapon id.
struct HostType {
    void (*enter)(Actor* actor);                     // 0x00
    void (*update)(float dt, Actor* actor, int arg); // 0x04
    void (*notify)(Actor* actor);                    // 0x08
    u32 anims[14];                                   // 0x0C (indexed by Player::hostIndex)
    int weaponId;                                    // 0x44
};

extern HostType gSniperHostTypes[25];

// Zoom ramp divisors (.sdata 0x804D32F8-0x804D3308) and the effect handles of the drawn / view states.
float gSniperZoomRate480 = 480.0f;
float gSniperZoomRate600 = 600.0f;
float gSniperZoomRate160 = 160.0f;
float gSniperZoomRate200 = 200.0f;
int gSniperDrawnFx = -1;
int gSniperViewFx = -1;

// First-person view-bob phase and its first-use flag (.sbss 0x804D5288 / 0x804D528C).
float gSniperBobPhase;
signed char gSniperBobInit;

// Asset handles of the first-person weapon view (.bss 0x8049C1E8).
int gSniperViewAssets[16];

extern "C" void fn_80155A84(void* a, void* b, int c);
extern "C" int fn_80175EFC(int id);
extern "C" int fn_800A384C(void* o);
int HasCondition(void* conds, int bit);
void PlayerSetHeldSlotIndex(void* p, int v);
int PlayerGetHeldSlotIndex(void* p);
int PlayerGunActionMessage(int msg, void* p);

// Grab-state helper: is the held object a player-type actor with condition bit 0xB set?
static inline void HeldCheck(Player* held, int* info, int* special) {
    if ((int)((u32)held->handle >> 24) == 0x13) {
        *info = fn_800A384C(held);
        if (HasCondition((char*)held + 0x270, 0xB) != 0) {
            *special = 1;
        }
    }
}


//@@801E7534
extern "C" void fn_801E7534(float dt, Player* p);
void fn_801E7534(float dt, Player* p) {
    p->v638 = 0;
    p->flags7B4 &= ~1;
    fn_800769A0(p, dt);
    PlayerSelectCollisionUpdate(p, dt);
    if (p->vC4 == -1) {
        if (p->v7D0 != 0) {
            PlayerSetValue4(&p->sub7A4, 3);
        } else {
            PlayerSetValue4(&p->sub7A4, 1);
        }
    } else {
        PlayerSetValue4(&p->sub7A4, 0x16);
    }
}
//@@end
//@@801E75D0
extern "C" void fn_801E75D0(Player* p);
void fn_801E75D0(Player* p) {
    p->fn8C = 0;
    if (p->vC4 != -1) {
        p->v98 = p->vC4;
    }
    if (p->v11C != -1) {
        p->vF0 = p->v11C;
    }
}
//@@end
//@@801E75FC
extern "C" void fn_801E75FC(Player* p);
void fn_801E75FC(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    PlayerSetValue4(&p->sub7A4, 0x15);
    if (p->v279C == -1 || fn_80175EFC(p->v279C) != 0) {
        PlayerSetValue4(&p->sub7A4, 3);
        fn_80155A84(&p->obj26E0, p->held, 1);
    }
}
//@@end
//@@801E767C
extern "C" void fn_801E767C(Player* p);
void fn_801E767C(Player* p) {
    Player* held = p->held;
    int info;
    int special = 0;
    if ((int)((u32)held->handle >> 24) == 0x13) {
        info = fn_800A384C(held);
        if (HasCondition((char*)held + 0x270, 0xB) != 0) {
            special = 1;
        }
    }
    PlayerInHost((Actor*)p);
    int tbl = p->hostIndex;
    p->v98 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x18), gSniperHostTypes[21].anims[tbl], special, 0);
    p->fn8C = (void*)PlayerGunActionMessage;
    p->vA4 = 0x19;
    p->vF0 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x1C), gSniperHostTypes[21].anims[tbl], special, 0);
    p->vFC = 0x19;
    PlayerSetValue4(&p->sub7A4, 0x15);
}
//@@end
//@@801E7778
extern "C" void fn_801E7778(float dt, Player* p);
void fn_801E7778(float dt, Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    fn_800769A0(p, dt);
    PlayerSelectCollisionUpdate(p, dt);
    if (PlayerGetHeldSlotIndex(p) != 0) {
        PlayerSetValue4(&p->sub7A4, 2);
    } else {
        PlayerSetValue4(&p->sub7A4, 1);
    }
    PlayerInHost((Actor*)p);
    PlayerWeaponReenter((Actor*)p);
}
//@@end
//@@801E7868
extern "C" void fn_801E7868(Player* p);
void fn_801E7868(Player* p) {
    int idx = PlayerInHost((Actor*)p);
    int tbl = p->hostIndex;
    p->unk78 = 0;
    p->flags &= ~0x20000;
    p->v98 = fn_800A1BC8(p->hosts[idx]->animA, p->hosts[idx]->animB, gSniperHostTypes[20].anims[tbl], 0, 0);
    p->fn8C = (void*)PlayerGunActionMessage;
    p->vA4 = 1;
    p->vF0 = fn_800A1BC8(p->hosts[idx]->animA, p->hosts[idx]->animC, gSniperHostTypes[20].anims[tbl], 0, 0);
    p->vFC = 1;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg((void*)p->target->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(p, 0);
        if (p->v98 == -1) {
            PlayerSetActionMode(p, 1);
        }
    } else {
        PlayerSetHeldSlotIndex(p, 1);
        if (p->v98 == -1) {
            PlayerSetActionMode(p, 3);
        }
    }
}
//@@end
//@@801E79D8
extern "C" void fn_801E79D8(Player* p);
void fn_801E79D8(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    if ((p->flags7B4 & 1) != 0) {
        p->flags7B4 &= ~1;
    } else {
        p->flags |= 0x20000;
        p->unk78 = 1;
    }
}
//@@end
//@@801E7A34
extern "C" void fn_801E7A34(Player* p);
void fn_801E7A34(Player* p) {
    int info;
    int special = 0;
    HeldCheck(p->held, &info, &special);
    int idx = PlayerInHost((Actor*)p);
    int tbl = p->hostIndex;
    p->v98 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x18), gSniperHostTypes[19].anims[tbl], special, 0);
    p->fn8C = (void*)PlayerGunActionMessage;
    p->vA4 = 0x19;
    HostInfo* host = p->hosts[idx];
    p->vF0 = fn_800A1BC8(host->animA, host->animC, gSniperHostTypes[19].anims[tbl], 0, 0);
    p->vFC = 0x19;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg((void*)p->held->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(p, 0);
    } else {
        PlayerSetHeldSlotIndex(p, 1);
    }
}
//@@end
//@@801E7B94
extern "C" void fn_801E7B94(Player* p);
void fn_801E7B94(Player* p) {
    fn_80155A84(&p->obj26E0, p->held, 1);
}
//@@end
//@@801E7BC0
extern "C" void fn_801E7BC0(Player* p);
void fn_801E7BC0(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    p->flags |= 0x20000;
    p->unk78 = 1;
    PlayerSetValue4(&p->sub7A4, 0x13);
}
//@@end
//@@801E7C1C
extern "C" void fn_801E7C1C(Player* p);
void fn_801E7C1C(Player* p) {
    int info;
    int idx;
    Player* held;
    int tbl;
    int special;
    HostInfo* host;
    idx = PlayerInHost((Actor*)p);
    held = p->held;
    tbl = p->hostIndex;
    special = 0;
    if ((int)((u32)held->handle >> 24) == 0x13) {
        info = fn_800A384C(held);
        if (HasCondition((char*)held + 0x270, 0xB) != 0) {
            special = 1;
        }
    }
    p->v98 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x18), gSniperHostTypes[18].anims[tbl], special, 0);
    p->fn8C = (void*)PlayerGunActionMessage;
    p->vA4 = 0x59;
    host = p->hosts[idx];
    p->vF0 = fn_800A1BC8(host->animA, host->animC, gSniperHostTypes[18].anims[tbl], 0, 0);
    p->vFC = 0x59;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg((void*)p->held->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(p, 0);
    } else {
        PlayerSetHeldSlotIndex(p, 1);
    }
}
//@@end
//@@801E7D78
extern "C" void fn_801E7D78(Player* p);
void fn_801E7D78(Player* p) {
    p->flags &= ~0x40000;
}
//@@end
//@@801E7D88
extern "C" void fn_801E7D88(float dt, Player* p);
void fn_801E7D88(float dt, Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(p, dt);
}
//@@end


// Player first-person view setup: creates the two weapon handles, installs the
// per-frame update callback and syncs the camera field of view.
void PlayerFirstPersonSetup(Actor* actor)
{
    PlayerResetGunTimer(actor);
    actor->flags50 |= 0x40000;
    fn_80050EB0(actor);

    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;

    actor->field98 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field18,
                                 gSniperHostTypes[13].anims[slot], 0, 0);
    actor->fieldA4 = 0x50;

    int bone;
    if (actor->field26B0 == -1) {
        bone = 8;
    } else {
        bone = ModelBoneNodeChain(actor->model, ModelBoneName(actor->model, actor->field26B0),
                                  gSniperViewAssets, 0x10);
        if (bone == -1) {
            bone = 8;
        }
    }

    actor->fieldF0 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C,
                                 gSniperHostTypes[13].anims[slot], bone << 1, 0);
    actor->updateE4 = (int (*)(int, Actor*))PlayerGunActionMessage;
    actor->fieldFC = 0x50;
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
}

// Player per-frame update for the first-person view state.
void PlayerFirstPersonUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);

    if (fn_80049940()) {
        PlayerWeaponReenter(actor);
    }
    PlayerSetValue4(&actor->state7A4, 3);
}

// Asks the actor's host for the name handle selected by `hash`.
static inline int PlayerWeaponQueryName(void* host, u32 hash)
{
    QueryMsg query;

    query.hash = hash;
    *(int*)&query.result = 0;
    query.unk8 = 0;
    SendMsg(host, 0x41e, &query);
    return *(int*)&query.result;
}

// Out-parameter form of the same query.
static inline void PlayerWeaponQueryName(int* out, void* host, u32 hash)
{
    QueryMsg query;

    query.hash = hash;
    *(int*)&query.result = 0;
    query.unk8 = 0;
    SendMsg(host, 0x41e, &query);
    *out = *(int*)&query.result;
}

// Tells the actor's host to switch to the name handle `value` for `hash`.
static inline void PlayerWeaponSetName(void* host, u32 hash, int value)
{
    QueryMsg query;

    query.hash = hash;
    *(int*)&query.result = value;
    query.unk8 = 0;
    SendMsg(host, 0x41d, &query);
}

// First-person "weapon held" view setup: resolves the model and animation
// names from the host, records the view FOV, and installs the per-frame
// update callback for this state.
void PlayerWeaponFirstPersonInit(Actor* actor)
{
    actor->field15B4 = 0;

    int modelName;

    PlayerWeaponQueryName(&modelName, actor->host38, 0x2da8293c);
    int animName;
    PlayerWeaponQueryName(&animName, actor->host38, 0xdff3097a);

    fn_80048BA8(actor, actor->field1D04, 1);
    int anim = fn_800498D0(actor);
    Fn800F7F5C(modelName, animName, 0x8fdd7599, anim, actor->field1D04, actor->field14FC);

    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;

    actor->field98 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field18,
                                 gSniperHostTypes[10].anims[slot], 0, 0);
    actor->field8C = (int (*)(int, Actor*))PlayerGunActionMessage;
    actor->fieldA4 = 0x11;
    actor->fieldF0 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C,
                                 gSniperHostTypes[10].anims[slot], 0, 0);
    if (actor->fieldF0 == fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C,
                                      gSniperHostTypes[3].anims[slot], 0, 0)) {
        actor->fieldFC = 1;
    } else {
        actor->fieldFC = 0x11;
    }
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
}

// Plays the weapon-ready animation entry of the actor's current loadout.
void PlayerWeaponRaiseAnim(Actor* actor)
{
    ActorPlayAnim(0x2000000, 0xd39f69b4, fn_800498D0(actor));
}

// Player per-frame update used while the first-person collision view is up:
// refreshes the local-player view, advances collision and, if the host asks
// for it, re-enters the weapon-select state.
void PlayerCollisionViewUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);

    PlayerWeaponReenter(actor);
    PlayerSetValue4(&actor->state7A4, 1);
}

// First-person weapon view setup for the state entered from the host: resolves
// the held weapon's name handle, swaps in the view model's two draw handles and
// starts the weapon-tied effect.
void PlayerWeaponViewSetup(Actor* actor)
{
    int name;
    PlayerWeaponQueryName(&name, actor->host38, 0xdff3097a);

    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;

    actor->field98 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field18,
                                 gSniperHostTypes[9].anims[slot], 0, 0);
    actor->field8C = (int (*)(int, Actor*))PlayerGunActionMessage;
    actor->fieldA4 = 0x10;
    actor->fieldF0 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C,
                                 gSniperHostTypes[9].anims[slot], 0, 0);
    actor->fieldFC = 0x10;

    gSniperViewFx = fn_800F7994(name, 0x9d1c245c);
    FxStart(gSniperViewFx, 0, 0);
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
}

// Player per-frame update for the weapon-held first-person state: refreshes the
// local-player collision view, re-enters the weapon-select state when the host
// asks for it, and re-targets the weapon model handle between the two loaded
// variants before handing the state back to the base update.
void PlayerWeaponStateUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);
    PlayerSetValue4(&actor->state7A4, 5);

    PlayerWeaponReenter(actor);

    PlayerWeaponToggle(actor);

    if (actor->field314 == 0) {
        PlayerSetValue4(&actor->state7A4, 3);
    } else {
        PlayerResetGunTimer(actor);
    }
}

// Installs the two draw handles of the held weapon view model (one for the
// world pass, one for the view-model pass) from the local player's view data.
void PlayerWeaponHandlesInit(Actor* actor)
{
    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;

    actor->field98 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field18,
                                 gSniperHostTypes[5].anims[slot], 0, 0);
    actor->field8C = (int (*)(int, Actor*))PlayerGunActionMessage;
    actor->fieldA4 = 1;
    actor->fieldF0 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C,
                                 gSniperHostTypes[5].anims[slot], 0, 0);
    actor->fieldFC = 1;
}

// Weapon-held idle state: re-asks the host for both weapon name handles,
// refreshes collision, keeps the view FOV in sync with the larger of the two
// view angles, and runs the shared "leave to weapon select" checks.
void PlayerWeaponIdleStateUpdate(Actor* actor, float dt)
{
    PlayerWeaponQueryName(actor->host38, 0x2da8293c);
    PlayerWeaponQueryName(actor->host38, 0xdff3097a);

    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);
    actor->field7D0 = 1;

    float fovAbs;
    {
        float z = actor->field394;
        float x = actor->field38C;
        fovAbs = __fabsf(x) > __fabsf(z) ? __fabsf(x) : __fabsf(z);
    }
    if (0.0f != fovAbs) {
        fn_8008E824(actor->field7C, 0);
    }

    if (0.0f == actor->field7D8) {
        PlayerSetHeldSlotMode(actor, 0);
    } else if (actor->field678 & 0x80) {
        actor->field678 &= ~0x80;
        PlayerSetActionMode(actor, 3);
    } else {
        PlayerSetValue4(&actor->state7A4, 3);

        PlayerWeaponToggle(actor);

        if (PlayerHeldSlotActionCheck(actor) == 0) {
            if (PlayerWeaponReenter(actor) == 0) {
                PlayerZoomStateUpdate(actor, dt);
            }
        }
    }
}

// Weapon-held entry state: resolves the two host name handles, matches the
// weapon animation set, retries the draw handles until the host returns a
// valid one, and asks for the weapon-select state when the player is able.
void PlayerWeaponEnterState(Actor* actor)
{
    int localPlayer;
    int slot;
    int nameA;
    int nameB;
    int result;
    int matched;
    PlayerWeaponQueryName(&nameA, actor->host38, 0x2da8293c);
    PlayerWeaponQueryName(&nameB, actor->host38, 0xdff3097a);
    localPlayer = PlayerInHost(actor);
    slot = actor->weaponSlot;
    result = Fn800F7DEC(nameA, nameB, 0x20ee36, fn_800498D0(actor), 0);

    matched = 0;
    if (result == -0x3f413082 || result == 0x11d2c025) {
        matched = 1;
    }

    actor->field98 = -1;
    while (actor->field98 == -1) {
        actor->field98 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field18, gSniperHostTypes[3].anims[slot], matched, 0);
        actor->field8C = (int (*)(int, Actor*))PlayerGunActionMessage;
        actor->fieldA4 = 1;
        if (actor->field98 == -1 && matched != 0) {
            matched = 0;
        }
    }

    actor->fieldF0 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C, gSniperHostTypes[3].anims[slot], 0, 0);
    actor->fieldFC = 1;

    PlayerWeaponReenter(actor);
}

// Plays the weapon idle animation entry of the actor's current loadout.
void PlayerWeaponIdleAnim(Actor* actor)
{
    ActorPlayAnim(0x2000000, 0x8b3a27a3, fn_800498D0(actor));
}

// Player per-frame update for the collision-view state that hands the player
// back to the weapon-select state.
void PlayerCollisionViewStateUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);

    PlayerWeaponReenter(actor);
    PlayerSetValue4(&actor->state7A4, 3);
}

extern "C" {
}

// First-person "weapon drawn" view setup: asks the host for the weapon name
// handle, installs both draw handles from the 0x9C weapon-id table, starts the
// weapon-tied effect and syncs the camera field of view.
void PlayerWeaponDrawnSetup(Actor* actor)
{
    int name;
    PlayerWeaponQueryName(&name, actor->host38, 0xdff3097a);

    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;

    actor->field98 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field18,
                                 gSniperHostTypes[2].anims[slot], 0, 0);
    actor->fieldA4 = 0x10;
    actor->field8C = (int (*)(int, Actor*))PlayerGunActionMessage;
    actor->fieldF0 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C,
                                 gSniperHostTypes[2].anims[slot], 0, 0);
    actor->fieldFC = 0x10;

    gSniperDrawnFx = fn_800F7994(name, 0xafef562b);
    FxStart(gSniperDrawnFx, 0, 0);
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
}

// Player per-frame update for the first-person collision-view state: clears
// the one-shot state flags, keeps collision in sync, updates the equipped
// weapon ids and hands the player back to the weapon-select state.
void PlayerCollisionViewEnterUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);

    if (actor->field678 & 0x80) {
        actor->field678 &= ~0x80;
        PlayerSetActionMode(actor, 1);
    }

    PlayerSetValue4(&actor->state7A4, 1);
    actor->field7D0 = 0;

    PlayerWeaponToggle(actor);

    if (PlayerHeldSlotActionCheck(actor) != 0) {
        return;
    }

    if (PlayerWeaponReenter(actor) == 0 && (actor->flags320 & 1)) {
        if (actor->field7D0 == 0) {
            PlayerSetHeldSlotMode(actor, 2);
        } else {
            PlayerSetHeldSlotMode(actor, 9);
        }
    }
}

// Weapon-state setup for the timer variant: installs both view draw handles from
// the "weapons" id table, arms the per-frame update callback and clears the
// weapon-timer counter.
void PlayerWeaponTimerStateSetup(Actor* actor)
{
    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;

    actor->field98 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field18,
                                 gSniperHostTypes[1].anims[slot], 0, 0);
    actor->fieldA4 = 1;
    actor->fieldF0 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C,
                                 gSniperHostTypes[1].anims[slot], 0, 0);
    actor->updateE4 = (int (*)(int, Actor*))PlayerGunActionMessage;
    actor->fieldFC = 1;
    actor->field7D0 = 0;
}

// Resets the held-weapon timer (state-entry helper shared with the base setup).
void PlayerWeaponResetTimer(Actor* actor)
{
    PlayerResetGunTimer(actor);
}

// First-person aim state update: marks the state active, clears the one-shot
// flag, runs the shared "return to weapon select" checks and leaves the aim
// view when the aim offsets are not neutral or the aim flag is set.
void PlayerAimStateUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    actor->field7D0 = 1;
    PlayerSetValue4(&actor->state7A4, 3);

    PlayerWeaponReenter(actor);

    if (PlayerHeldSlotActionCheck(actor) == 0) {
        if (PlayerWeaponToggle(actor) == 0) {
            PlayerZoomStateUpdate(actor, dt);
        }
    }

    if (0.0f != actor->field394 ||
        0.0f != actor->field38C ||
        0.0f != actor->field390 ||
        0.0f != actor->field398 ||
        actor->flags328 != 0) {
        PlayerSetActionMode(actor, 3);
    }
}

// Weapon-timer state entry: resolves the two host weapon name handles, matches
// the current weapon animation set, picks the timer variant (coin flip when the
// weapon set does not force one) and installs both view draw handles, retrying
// once with the un-matched variant when the host returns no handle.
void PlayerWeaponTimerEnter(Actor* actor)
{
    int nameA;
    PlayerWeaponQueryName(&nameA, actor->host38, 0x2da8293c);
    int nameB;
    PlayerWeaponQueryName(&nameB, actor->host38, 0xdff3097a);
    int anim = fn_800498D0(actor);
    int result = Fn800F7DEC(nameA, nameB, 0x20ee36, anim, 0);

    if (result == -0x3f413082 || result == 0x11d2c025) {
        actor->field7D4 = 2;
    } else if ((float)rand() / 32767.0f >= 0.5f) {
        actor->field7D4 = 0;
    } else {
        actor->field7D4 = 1;
    }

    int localPlayer = PlayerInHost(actor);

    actor->field98 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field18, gSniperHostTypes[0].anims[0], actor->field7D4, 0);
    if (actor->field98 == -1) {
        actor->field7D4 = 0;
        actor->field98 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field18, gSniperHostTypes[0].anims[0], actor->field7D4, 0);
    }
    actor->field8C = (int (*)(int, Actor*))PlayerGunActionMessage;
    actor->fieldA4 = 1;

    actor->fieldF0 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C, gSniperHostTypes[0].anims[0], actor->field7D4, 0);
    if (actor->fieldF0 == -1) {
        actor->field7D4 = 0;
        actor->fieldF0 = fn_800A1BC8(actor->views[localPlayer]->field14, actor->views[localPlayer]->field1C, gSniperHostTypes[0].anims[0], actor->field7D4, 0);
    }
    actor->updateE4 = (int (*)(int, Actor*))PlayerGunActionMessage;
    actor->fieldFC = 1;
}

// Zoom/aim state update for the first-person weapon states: asks the host for
// both weapon name handles and either re-enters the zoomed weapon view or
// advances the hold timer.
int PlayerZoomStateUpdate(Actor* actor, float dt)
{
    int nameA;
    PlayerWeaponQueryName(&nameA, actor->host38, 0x2da8293c);
    int nameB;
    PlayerWeaponQueryName(&nameB, actor->host38, 0xdff3097a);

    if (actor->field588 == 5 || actor->field588 == 4) {
        if (actor->flags320 & 1) {
            PlayerResetGunTimer(actor);
            if (actor->field7D0 == 0) {
                PlayerSetHeldSlotMode(actor, 2);
            } else {
                PlayerSetHeldSlotMode(actor, 9);
            }
            return 1;
        }
        if (actor->flags324 & 1) {
            PlayerResetGunTimer(actor);

            int anim = fn_800498D0(actor);
            int result = Fn800F7DEC(nameA, nameB, 0xe6b71bcc, anim, 0);
            if (result == 0x4a3e183) {
                actor->field15B4 = 0;
                PlayerSetHeldSlotMode(actor, 10);
                return 1;
            }

            anim = fn_800498D0(actor);
            result = Fn800F7DEC(nameA, nameB, 0x968cbe3d, anim, 0);
            if (result == 0x4a3e183) {
                PlayerSetHeldSlotMode(actor, 11);
                return 1;
            }
        }
    } else if (actor->flags320 != 0) {
        PlayerResetGunTimer(actor);
        actor->field5E8 += dt;

        if (fn_800699B8(actor) == 1.0f) {
            if (actor->field7D0 == 0) {
                PlayerSetHeldSlotMode(actor, 2);
            } else {
                PlayerSetHeldSlotMode(actor, 9);
            }
            fn_800699A8(actor->zoomTable);
            return 1;
        }
    } else {
        float level = fn_800699B8(actor);
        if (level > -1.0f && level < 0.0f) {
            int anim = fn_800498D0(actor);
            int result = Fn800F7DEC(nameA, nameB, 0xe6b71bcc, anim, 0);
            if (result == 0x4a3e183) {
                PlayerSetHeldSlotMode(actor, 10);
                fn_800699A8(actor->zoomTable);
                return 1;
            }

            anim = fn_800498D0(actor);
            result = Fn800F7DEC(nameA, nameB, 0x968cbe3d, anim, 0);
            if (result == 0x4a3e183) {
                PlayerSetHeldSlotMode(actor, 11);
                fn_800699A8(actor->zoomTable);
                return 1;
            }
        }
        fn_800699A8(actor->zoomTable);
    }
    return 0;
}

// Per-frame zoom-level update: selects the target zoom level from the two slot
// state entries, drives the view FOV towards it and notifies the effect system
// when the level changed.
int PlayerZoomLevelUpdate(Actor* actor, int slotA, int slotB)
{
    int oldLevel = actor->field7E0;

    if (fn_80049940() == 0) {
        if (actor->zoomTable[slotB] != 0) {
            if (actor->state7A4 == 1) {
                PlayerSetHeldSlotMode(actor, 2);
                return 1;
            }
            actor->field7E8 = (1.0f - fn_800F95E4(actor->cameraIndex)) / gSniperZoomRate200;
            actor->field7EC = 1.0f;
            actor->field7E0 = 2;
        } else if (actor->zoomTable[slotA] != 0) {
            if (actor->state7A4 == 1) {
                PlayerSetHeldSlotMode(actor, 2);
                return 1;
            }
            actor->field7E8 = (8.0f - fn_800F95E4(actor->cameraIndex)) / gSniperZoomRate160;
            actor->field7EC = 8.0f;
            actor->field7E0 = 1;
        } else {
            actor->field7E8 = 0.0f;
            actor->field7E0 = 0;
        }
    } else {
        int slotState = actor->zoomTable[slotA];
        if (slotState == 1 || slotState == 3) {
            if (actor->state7A4 == 1) {
                PlayerSetHeldSlotMode(actor, 2);
                return 1;
            }
            switch (actor->field7E0) {
            case 0:
                actor->field7E8 = (8.0f - fn_800F95E4(actor->cameraIndex)) / gSniperZoomRate480;
                actor->field7EC = 8.0f;
                actor->field7E0 = 1;
                break;
            case 1:
                actor->field7E8 = (1.0f - fn_800F95E4(actor->cameraIndex)) / gSniperZoomRate600;
                actor->field7EC = 1.0f;
                actor->field7E0 = 2;
                break;
            case 2:
                actor->field7E8 = 0.0f;
                actor->field7E0 = 0;
                break;
            }
        }
    }

    if (actor->field7E0 != 0) {
        PlayerResetGunTimer(actor);
        actor->flags7B4 |= 0x01000000;
        fn_80165DA8(actor);
    } else {
        actor->flags7B4 &= ~0x01000000;
        fn_80165D9C(actor);
    }

    if (0.0f != actor->field7E8) {
        float fov = fn_800F95E4(actor->cameraIndex);
        float step = actor->field7E8 * actor->field14FC;
        float next;

        if (fn_80049940() == 0) {
            float target = actor->field7EC;
            if (fov > target) {
                next = fov + step;
                if (next < target) {
                    actor->field7E8 = 0.0f;
                    next = target;
                }
            } else if (fov < target) {
                next = fov + step;
                if (next > target) {
                    actor->field7E8 = 0.0f;
                    next = target;
                }
            }
        } else {
            float target = actor->field7EC;
            next = fov + step;
            if (next < target) {
                actor->field7E8 = 0.0f;
                next = target;
            }
        }
        CameraSetFov(actor->cameraIndex, next);
    
    } else {
        switch (actor->field7E0) {
        case 0:
            CameraSetFov(actor->cameraIndex, (float)actor->views[1]->fov);
            break;
        case 1:
            CameraSetFov(actor->cameraIndex, 8.0f);
            break;
        case 2:
            CameraSetFov(actor->cameraIndex, 1.0f);
            break;
        }
    }

    int level = actor->field7E0;
    if (level != oldLevel) {
        if (oldLevel < level) {
            FxStart(FxFind("zoom_out"), 0, 0);
        } else if (level == 1) {
            FxStart(FxFind("zoom_level1"), 0, 0);
        } else if (level == 2) {
            FxStart(FxFind("zoom_level2"), 0, 0);
        }
    }
    return 0;
}

// Checks the held-slot weapon action against the nameless action pair and
// switches the held-slot mode when the queried action resolves. Returns 1 when
// a mode change was made.
int PlayerHeldSlotActionCheck(Actor* actor)
{
    int nameA;
    PlayerWeaponQueryName(&nameA, actor->host38, 0x2da8293c);
    int nameB;
    PlayerWeaponQueryName(&nameB, actor->host38, 0xdff3097a);

    if (actor->field314 >= 1) {
        if (actor->state7A4 == 1) {
            PlayerSetHeldSlotMode(actor, 2);
            return 1;
        }

        PlayerResetGunTimer(actor);
        int anim = fn_800498D0(actor);
        int result = Fn800F7DEC(nameA, nameB, 0x20ee36, anim, 0);

        switch (result) {
        case 0x4a3e183:
            PlayerSetHeldSlotMode(actor, 5);
            return 1;
        case -0x3f413082: {
            int anim2 = fn_800498D0(actor);
            int second = Fn800F7DEC(nameA, nameB, 0xe6b71bcc, anim2, 0);
            actor->field15B4 = 0;
            if (second == 0x4a3e183) {
                PlayerSetHeldSlotMode(actor, 10);
                return 1;
            }
            break;
        }
        case 0x11d2c025:
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                PlayerWeaponSetName(actor->host38, 0x7ffcfd4f, 0x91a0cef7);
            }
            if (actor->state7A4 == 5) {
                PlayerSetHeldSlotMode(actor, 3);
            }
            break;
        case 0x3da260f7:
            if (actor->state7A4 == 5) {
                PlayerSetHeldSlotMode(actor, 3);
            }
            break;
        }
    } else {
        if (actor->state7A4 == 5) {
            PlayerSetHeldSlotMode(actor, 3);
        }
    }
    return 0;
}

// Per-frame weapon action message handler. Resolves the two action names once,
// then dispatches on the message id: either plays the matching animation or
// switches the held-slot mode (playing the "gun_empty" effect when the weapon
// has no ammo left).
// Plain 0x20-byte query block handed to fn_800C5210 (no String member).
struct ActionQuery {
    int unk00;      // 0x00
    u32 hash;       // 0x04
    int unk08;      // 0x08
    void* result;   // 0x0C
    int unk10[4];   // 0x10
};

// The actor arrives as void* and is copied to a local: retail ranks it as a
// local (r31) ahead of the two names, the way the void* message handlers do.
int PlayerGunActionMessage(int msg, void* p)
{
    Actor* actor = (Actor*)p;
    ActionQuery q;
    q.result = actor->host38;
    q.hash = msg;
    if (fn_800C5210(actor->host38, 0xc06, &q) == 1) {
        return 0;
    }

    int nameA;
    int nameB;
    PlayerWeaponQueryName(&nameA, actor->host38, 0x2da8293c);
    PlayerWeaponQueryName(&nameB, actor->host38, 0xdff3097a);

    switch (msg) {
    case 0x1b83833f:
        if (actor->state7A4 == 5) {
            int result = Fn800F7DEC(nameA, nameB, 0x20ee36, fn_800498D0(actor), 0);
            if (result != 0x4a3e183) {
                if (result == -0x3f413082 || result == 0x11d2c025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                PlayerSetHeldSlotMode(actor, 3);
            } else {
                PlayerSetActionMode(actor, actor->field7A8);
            }
        } else if (actor->state7A4 == 8) {
            int result = Fn800F7DEC(nameA, nameB, 0xf829db93, fn_800498D0(actor), 0);
            if (result != 0x4a3e183) {
                if (result == -0x3f413082 || result == 0x11d2c025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                PlayerSetHeldSlotMode(actor, 3);
            } else {
                PlayerSetActionMode(actor, actor->field7A8);
            }
        } else if (actor->state7A4 == 0xd) {
            actor->flags7B4 |= 0x20000;
        } else {
            PlayerSetActionMode(actor, actor->field7A8);
        }
        break;
    case 0x20ee36:
        fn_80048BA8(actor, actor->field1D04, 1);
        Fn800F7F5C(nameA, nameB, msg, fn_800498D0(actor), actor->field1D04, actor->field14FC);
        break;
    case -0x69ddb034:   // 0x96224fcc
    case 0xd19e338:
        fn_80048BA8(actor, actor->field1D04, 1);
        Fn800F7F5C(nameA, nameB, msg, fn_800498D0(actor), actor->field1D04, actor->field14FC);
        break;
    case -0x1948e434:   // 0xe6b71bcc
        fn_80048BA8(actor, actor->field1D04, 1);
        Fn800F7F5C(nameA, nameB, msg, fn_800498D0(actor), actor->field1D04, actor->field14FC);
        break;
    case -0x697341c3:   // 0x968cbe3d
        fn_80048BA8(actor, actor->field1D04, 1);
        Fn800F7F5C(nameA, nameB, msg, fn_800498D0(actor), actor->field1D04, actor->field14FC);
        break;
    }
    return 0;
}

// Leaves the zoomed first-person view through the weapon-effect variant:
// clears the zoom level, restores the camera field of view, drops the zoom flag
// and releases the zoom assets before the shared zoom-exit helper runs.
void PlayerWeaponZoomExitFx(Actor* actor)
{
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
    AssetFind(0xf2e2beb8, 0xdefd6fa5);
    fn_8000F60C();
    fn_80165D9C(actor);
}

// Leaves the zoomed first-person view: clears the zoom level, restores the
// camera field of view, drops the zoom flag and runs the shared zoom-exit
// helper.
void PlayerWeaponZoomExit(Actor* actor)
{
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
    fn_80165D9C(actor);
}

// Leaves the zoomed first-person view through the weapon-effect asset: clears
// the zoom level, restores the camera field of view, drops the zoom flag and
// hands the effect asset to the asset release helper before the shared
// zoom-exit helper runs.
void PlayerWeaponZoomExitFxAsset(Actor* actor)
{
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
    fn_8000F7E4(AssetFind(0xf2e2beb8, 0xdefd6fa5), 1);
    fn_80165D9C(actor);
}

// Returns the weapon id assigned to the actor's current weapon state.
int PlayerWeaponStateId(Actor* actor)
{
    return gSniperHostTypes[actor->state7A4].weaponId;
}


float fn_80017084(float);

// Notifies the actor's current weapon state through the +0x08 callback.
void PlayerWeaponStateNotify(Actor* actor)
{
    if (gSniperHostTypes[actor->state7A4].notify != 0) {
        gSniperHostTypes[actor->state7A4].notify(actor);
    }
}


// Shared per-frame effect for the first-person weapon states: advances the
// global view-bob phase, offsets the view position by the two sine terms and
// re-arms the gun timer, then runs the state's +0x04 frame callback.
void PlayerWeaponStateFrameUpdate(float dt, Actor* actor, int arg)
{
    if (gSniperBobInit == 0) {
        gSniperBobInit = 1;
        gSniperBobPhase = 0.0f;
    }

    if (actor->state7A4 == 3 || actor->state7A4 == 5) {
        if (actor->field7E0 == 1) {
            gSniperBobPhase = gSniperBobPhase + dt;
            float x = fn_80017084(0.0003f * gSniperBobPhase);
            x *= 0.008f;
            actor->field1F0 += x;
            float y = fn_80017084(0.0003f * (5000.0f + gSniperBobPhase));
            y *= 0.004f;
            actor->field1F8 += y;
            PlayerResetGunTimer(actor);
        } else if (actor->field7E0 == 2) {
            gSniperBobPhase = gSniperBobPhase + dt;
            float x = fn_80017084(0.0003f * gSniperBobPhase);
            x *= 0.004f;
            actor->field1F0 += x;
            float y = fn_80017084(0.0003f * (5000.0f + gSniperBobPhase));
            y *= 0.002f;
            actor->field1F8 += y;
            PlayerResetGunTimer(actor);
        }
    }

    if (gSniperHostTypes[actor->state7A4].update != 0) {
        gSniperHostTypes[actor->state7A4].update(dt, actor, arg);
    }
}

// Runs the enter handler of the actor's current weapon state, if any.
void Ea620StateEnter(Actor* actor)
{
    HostType* entry = &gSniperHostTypes[actor->state7A4];
    if (entry->enter != 0) {
        entry->enter(actor);
    }
}

// Loads the sixteen asset handles used by the first-person weapon view; they
// are all tagged with the same group hash and cached in the 0x8049C1E8 block.
// fn_801EA660
extern "C" void fn_801EA660(void)
{
    gSniperViewAssets[0] = AssetFind(0xf1754bf2, 0x21c5e0);
    gSniperViewAssets[1] = AssetFind(0xf1754bf2, 0x24805f);
    gSniperViewAssets[2] = AssetFind(0xf1754bf2, 0x101844cc);
    gSniperViewAssets[3] = AssetFind(0xf1754bf2, 0x48874f6e);
    gSniperViewAssets[4] = AssetFind(0xf1754bf2, 0x4a53fc1);
    gSniperViewAssets[5] = AssetFind(0xf1754bf2, 0x967daac6);
    gSniperViewAssets[6] = AssetFind(0xf1754bf2, 0xd9404234);
    gSniperViewAssets[7] = AssetFind(0xf1754bf2, 0x450b2bb);
    gSniperViewAssets[8] = AssetFind(0xf1754bf2, 0x922bf04e);
    gSniperViewAssets[9] = AssetFind(0xf1754bf2, 0x8c73dfa9);
    gSniperViewAssets[10] = AssetFind(0xf1754bf2, 0x90aeeee4);
    gSniperViewAssets[11] = AssetFind(0xf1754bf2, 0x4a2f9aa);
    gSniperViewAssets[12] = AssetFind(0xf1754bf2, 0x4a48bc0);
    gSniperViewAssets[13] = AssetFind(0xf1754bf2, 0x8671db2a);
    gSniperViewAssets[14] = AssetFind(0xf1754bf2, 0x44e6ca4);
    gSniperViewAssets[15] = AssetFind(0xf1754bf2, 0x44ffeba);
}

// Empty hook, kept so the screen-printf call sites have a stable target.
// (Descriptive name: DebugTextNoOp. Kept as fn_801EA87C because other units
// already call it by that placeholder.)
extern "C" void fn_801EA87C(void) {}

// The sniper host's state table (.data 0x803B3B60). Entry 12 is the shared dash state (0x80243778-0x80243B0C).
class LargeActor;
void PlayerDashStart(Actor* actor);
void PlayerDashCheck(Actor* actor);
void LargeActorResetFlags(LargeActor* actor);

HostType gSniperHostTypes[25] = {
    { PlayerWeaponTimerEnter, (void (*)(float, Actor*, int))PlayerAimStateUpdate, PlayerWeaponResetTimer,
      { 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4, 0x002237D4 }, 0 },
    { PlayerWeaponTimerStateSetup, (void (*)(float, Actor*, int))PlayerCollisionViewEnterUpdate, 0,
      { 0xA8BE217B, 0x744B56B9, 0x90A41C8E, 0x29D70F30, 0xA81C49F3, 0x0A798111, 0x35FC8E1C, 0xA9D72F66, 0x8254AA67, 0xD3193F08, 0x81548F05, 0x40EC2F58, 0xB1BEC6A3, 0x11042941 }, 0 },
    { PlayerWeaponDrawnSetup, (void (*)(float, Actor*, int))PlayerCollisionViewStateUpdate, PlayerWeaponIdleAnim,
      { 0x34CD59AA, 0xD84584AC, 0x23823037, 0x30B56155, 0x454C4232, 0x58FCAF54, 0x83AE54E9, 0xB0B5625F, 0x230C033E, 0x9101D67D, 0x4C4E12E0, 0xFFE79C2D, 0x140F6F82, 0x1F644124 }, 0 },
    { PlayerWeaponEnterState, (void (*)(float, Actor*, int))PlayerWeaponIdleStateUpdate, 0,
      { 0x02A1A099, 0xC5D70D57, 0x3BE1446C, 0x5514778E, 0xD9DF8711, 0xC1BBEDAF, 0xE424C17A, 0x2FEB2D44, 0x3BDD7485, 0xDC50BD66, 0x1C29E8A3, 0x5BC381B6, 0x1A596FC1, 0x0A32E1DF }, 0 },
    { 0, 0, 0,
      { 0x6DB3B0E5, 0x3940E623, 0x5599ABF8, 0xEECC9E9A, 0x6D11D95D, 0xCF6F107B, 0xFAF21D86, 0x6ECCBED0, 0x474A39D1, 0x980ECE72, 0x464A1E6F, 0x05E1BEC2, 0x76B4560D, 0xD5F9B8AB }, 0 },
    { PlayerWeaponHandlesInit, (void (*)(float, Actor*, int))PlayerWeaponStateUpdate, 0,
      { 0xCE83F460, 0x50AEF7E2, 0xF9A70A2D, 0x5D8FEACB, 0xCD335EE8, 0x1F00AC8A, 0x0F98B35F, 0x2AD0A655, 0x7DC7F2F4, 0xCD4789F3, 0x2A2D8316, 0x4D42FBA3, 0xB681A038, 0xCEC2725A }, 0 },
    { 0, 0, 0,
      { 0x48C2836E, 0xEEDBF5F0, 0x5D9BEABB, 0xEAC74C59, 0x352969F6, 0x1E731698, 0x6351AAED, 0x6ACB32E3, 0xA1FD1802, 0x69CB1781, 0x82F9C724, 0xB6563131, 0x5FD68346, 0xE93D7468 }, 0 },
    { 0, 0, 0,
      { 0x21820DA8, 0xA708E7AA, 0x659ED5B5, 0x557847D3, 0x8FC97230, 0x4D5A7E52, 0xD4E55167, 0xE41CB3DD, 0x1669CD3C, 0x22B5E8FB, 0xAAF73BDE, 0x78E156AB, 0xC3957780, 0x8F04A822 }, 0 },
    { 0, 0, 0,
      { 0x48C24DC9, 0xEEDBC04B, 0x5D9BB516, 0xEAC716B4, 0x35293451, 0x1E72E0F3, 0x63517548, 0x6ACAFD3E, 0xA1FCE25D, 0x69CAE1DC, 0x82F9917F, 0xB655FB8C, 0x5FD64DA1, 0xE93D3EC3 }, 0 },
    { PlayerWeaponViewSetup, (void (*)(float, Actor*, int))PlayerCollisionViewUpdate, PlayerWeaponRaiseAnim,
      { 0x64DDF349, 0x306B2887, 0x4CC3EE5C, 0xE5F6E0FE, 0x643C1BC1, 0xC69952DF, 0xF21C5FEA, 0x65F70134, 0x3E747C35, 0x8F3910D6, 0x3D7460D3, 0xFD0C0126, 0x6DDE9871, 0xCD23FB0F }, 0 },
    { PlayerWeaponFirstPersonInit, (void (*)(float, Actor*, int))PlayerFirstPersonUpdate, 0,
      { 0x51979943, 0xF50FC445, 0x404C6FD0, 0x4D7FA0EE, 0x621681CB, 0x75C6EEED, 0xA0789482, 0xCD7FA1F8, 0x3FD642D7, 0xADCC1616, 0x69185279, 0x1CB1DBC6, 0x30D9AF1B, 0x3C2E80BD }, 0 },
    { 0, 0, 0,
      { 0x1DE2C92C, 0xA369A32E, 0x61FF9139, 0x51D90357, 0x8C2A2DB4, 0x49BB39D6, 0xD1460CEB, 0xE07D6F61, 0x12CA88C0, 0x1F16A47F, 0xA757F762, 0x7542122F, 0xBFF63304, 0x8B6563A6 }, 0 },
    { PlayerDashStart, (void (*)(float, Actor*, int))PlayerDashCheck, (void (*)(Actor*))LargeActorResetFlags,
      { 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236, 0x72C27236 }, 0 },
    { PlayerFirstPersonSetup, (void (*)(float, Actor*, int))fn_801E7D88, (void (*)(Actor*))fn_801E7D78,
      { 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6, 0x1F6F94F6 }, 0 },
    { 0, 0, 0,
      { 0xF8B4BD22, 0x15C99AE0, 0x7F083F75, 0x97491197, 0x992AA79A, 0xD9168F38, 0x300A8583, 0x17C1FC4D, 0xC6CCBB0E, 0xF8BEAB6F, 0x0565F02C, 0x3D96C7BF, 0xC420B84A, 0x6797EB68 }, 0 },
    { 0, 0, 0,
      { 0x1DE2FED1, 0xA369D8D3, 0x61FFC6DE, 0x51D938FC, 0x8C2A6359, 0x49BB6F7B, 0xD1464290, 0xE07DA506, 0x12CABE65, 0x1F16DA24, 0xA7582D07, 0x754247D4, 0xBFF668A9, 0x8B65994B }, 0 },
    { 0, 0, 0,
      { 0x3FE4C227, 0x5CF99FE5, 0xC638447A, 0xDE79169C, 0xE05AAC9F, 0x2046943D, 0x773A8A88, 0x5EF20152, 0x0DFCC013, 0x3FEEB074, 0x4C95F531, 0x84C6CCC4, 0x0B50BD4F, 0xAEC7F06D }, 0 },
    { 0, 0, 0,
      { 0x38243A5E, 0xDB9C6560, 0x26D910EB, 0x340C4209, 0x48A322E6, 0x5C539008, 0x8705359D, 0xB40C4313, 0x2662E3F2, 0x9458B731, 0x4FA4F394, 0x033E7CE1, 0x17665036, 0x22BB21D8 }, 0 },
    { (void (*)(Actor*))fn_801E7C1C, (void (*)(float, Actor*, int))fn_801E7BC0, (void (*)(Actor*))fn_801E7B94,
      { 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C, 0x672F359C }, 0 },
    { (void (*)(Actor*))fn_801E7A34, (void (*)(float, Actor*, int))fn_801E79D8, 0,
      { 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466, 0x1379C466 }, 0 },
    { (void (*)(Actor*))fn_801E7868, (void (*)(float, Actor*, int))fn_801E7778, 0,
      { 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85, 0x1C1A4B85 }, 0 },
    { (void (*)(Actor*))fn_801E767C, (void (*)(float, Actor*, int))fn_801E75FC, 0,
      { 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89, 0x672F1D89 }, 0 },
    { (void (*)(Actor*))fn_801E75D0, (void (*)(float, Actor*, int))fn_801E7534, 0,
      { 0x02A1A099, 0xC5D70D57, 0x3BE1446C, 0x5514778E, 0xD9DF8711, 0xC1BBEDAF, 0xE424C17A, 0x2FEB2D44, 0x3BDD7485, 0xDC50BD66, 0x1C29E8A3, 0x5BC381B6, 0x1A596FC1, 0x0A32E1DF }, 0 },
    { 0, 0, 0,
      { 0 }, 0 },
    { 0, 0, 0,
      { 0 }, 0 },
};

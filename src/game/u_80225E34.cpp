// u_80225E34.cpp (0x80225E34-0x80227DD8): a host's player state callbacks (hang / grab / seat states),
// its camera zoom states (FOV printf "CurrentFOV = %0.9f", "zoom_out", "zoom_level1") and its 25-entry host
// behaviour table 0x803BCA60. The old u_80226DE8 range (0x80226DE8-0x8022EC64) held five files: this one,
// u_80227DD8, u_8022A180, u_8022BA30 (MCG effect) and u_8022CE78. Boundary evidence:
// - start 0x80225E34 (moved from u_80222D60, which ended at 0x80226DE8): the player state callbacks
//   PlayerHangExit (0x80225E34) .. PlayerSeatUpdateC fill entries 1-22 of this file's host table 0x803BCA60
//   (enter/update pairs at 0x803BCAA8 .. 0x803BD094; entry 1's enter is fn_80226DE8), read .sdata2
//   0x804D9658 / .rodata 0x80349C28 just before this file's pools, and are instruction-for-instruction the
//   same code as u_8022CE78's host states. CamTuneInit (0x80225D50) ends the camera tunables of u_80222D60.
// - .sdata2: this file's pool is 0x804D9660-0x804D9678 (0.0f at 0x804D9660); the next file has its own 0.0f at
//   0x804D967C. fn_80227B9C (last function here) is the last reader of the first pool, fn_80227DD8 the first of
//   the second.
// - .rodata: pool 0x80349C34-0x80349CB8 (POSSESS..SCAN, then the FOV strings); .data 0x803BCA60-0x803BD168 (the
//   host table, read by fn_80227AB8..fn_80227B5C); .sbss 0x804D5408-0x804D5418.
// Names are descriptive guesses; unmatched against retail unless noted.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

extern "C" {
int SendMsg(int to, int msg, void* data);
int PlayerInHost(void* player);
void PlayerResetGunTimer(void* p);
void PlayerSetValue4(void* p, int v);
void PlayerSetActionMode(void* p, int mode);
void PlayerSetHeldSlotIndex(void* p, int v);
int PlayerGetHeldSlotIndex(void* p);
void PlayerSetHeldSlotMode(void* p, int v);
void PlayerSelectCollisionUpdate(float dt, void* p);
void fn_800769A0(float dt, void* p);
int fn_800A384C(void* o);
int fn_800A1BC8(int a, int b, int c, int d, int e);
void fn_80155A84(void* a, void* b, int c);
int fn_80175EFC(void);
void fn_80050EB0(void* p);
void fn_80227594(void);
void fn_802272D4(void* p);
void fn_80226E78(void* p);
void fn_80212730(int id);
void MarkLookupEntryFlag(int id);
int OwnerGetInfo(u32 id);
void fn_8009F63C(void* m, int a);
void fn_8009F44C(void* out, void* m, void* in);
}

int HasCondition(void* conds, int bit);

// Variable query: the reply word is the first word of the string.
inline int QueryVar(int target, u32 hash) {
    QueryMsg m;
    *(int*)&m.result = 0;
    m.hash = hash;
    m.unk8 = 0;
    SendMsg(target, 0x41E, &m);
    return *(int*)&m.result;
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
// ---- player state callbacks --------------------------------------------------------------


extern int gHostAnimA[];        // 0x803BD054
extern int gHostAnimB[];        // 0x803BCFC4
extern int gHostAnimC[];        // 0x803BCF7C

// Exit of the hang state: back to the normal movement sub-state.
void PlayerHangExit(float dt, Player* p) {
    p->v638 = 0;
    p->flags7B4 &= ~1;
    fn_800769A0(dt, p);
    PlayerSelectCollisionUpdate(dt, p);
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

void PlayerHangResetAnim(Player* p) {
    p->fn8C = NULL;
    if (p->vC4 != -1) {
        p->v98 = p->vC4;
    }
    if (p->v11C == -1) {
        return;
    }
    p->vF0 = p->v11C;
}

void PlayerHangEnter(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    PlayerSetValue4(&p->sub7A4, 0x15);
    if (p->v279C == -1 || fn_80175EFC() != 0) {
        PlayerSetValue4(&p->sub7A4, 3);
        fn_80155A84(&p->obj26E0, p->held, 1);
    }
}

// Is the held object a player-type actor that has condition bit 0xB set?  (Helper of the
// grab-state enters below.)
static inline int HeldIsSpecial(Player* held, int* info) {
    int r = 0;
    if ((u32)held->handle >> 24 == 0x13) {
        *info = fn_800A384C(held);
        if (HasCondition((char*)held + 0x270, 0xB) != 0) {
            r = 1;
        }
    }
    return r;
}

void PlayerGrabEnterA(Player* p) {
    int info;
    int special;
    int idx;
    int tbl;
    Player* held = p->held;
    special = HeldIsSpecial(held, &info);
    PlayerInHost(p);
    tbl = p->hostIndex;
    p->v98 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x18), gHostAnimA[tbl], special, 0);
    p->fn8C = (void*)fn_80227594;
    *(int*)((char*)p + 0xA4) = 0x19;
    p->vF0 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x1C), gHostAnimA[tbl], special, 0);
    p->vFC = 0x19;
    PlayerSetValue4(&p->sub7A4, 0x15);
}

void PlayerGrabUpdate(float dt, Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    fn_800769A0(dt, p);
    PlayerSelectCollisionUpdate(dt, p);
    if (PlayerGetHeldSlotIndex(p) != 0) {
        PlayerSetValue4(&p->sub7A4, 2);
    } else {
        PlayerSetValue4(&p->sub7A4, 1);
    }
    PlayerInHost(p);
    fn_80226E78(p);
}

void PlayerHostSync(Player* p) {
    int a;
    int b;
    PlayerInHost(p);
    p->unk78 = 0;
    p->flags &= ~0x20000;
    p->v98 = QueryVar(p->target->handle, 0x6A0D51C0);
    b = QueryVar(p->target->handle, 0x8783452);
    if (b == 0) {
        PlayerSetHeldSlotIndex(p, 1);
        if (p->v98 == -1) {
            PlayerSetActionMode(p, 3);
        }
    } else {
        PlayerSetHeldSlotIndex(p, 0);
        if (p->v98 == -1) {
            PlayerSetActionMode(p, 1);
        }
    }
}

void PlayerHangHold(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    if ((p->flags7B4 & 1) != 0) {
        p->flags7B4 &= ~1;
    } else {
        p->flags |= 0x20000;
        p->unk78 = 1;
    }
}

void PlayerGrabEnterB(Player* p) {
    int info = 0;
    int special = 0;
    int idx;
    int tbl;
    HostInfo* host;
    Player* held = p->held;
    special = HeldIsSpecial(held, &info);
    idx = PlayerInHost(p);
    tbl = p->hostIndex;
    p->v98 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x18), gHostAnimB[tbl], special, 0);
    p->fn8C = (void*)fn_80227594;
    *(int*)((char*)p + 0xA4) = 0x19;
    host = p->hosts[idx];
    p->vF0 = fn_800A1BC8(host->animA, host->animC, gHostAnimB[tbl], 0, 0);
    p->vFC = 0x19;
    PlayerSetHeldSlotIndex(p, QueryVar(p->held->handle, 0x8783452) == 0);
}

void PlayerReleaseHeld(Player* p) {
    fn_80155A84(&p->obj26E0, p->held, 1);
}

void PlayerHangEnterC(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    p->flags |= 0x20000;
    p->unk78 = 1;
    PlayerSetValue4(&p->sub7A4, 0x13);
}

void PlayerGrabEnterC(Player* p) {
    int info = 0;
    int special;
    int idx;
    int tbl;
    HostInfo* host;
    idx = PlayerInHost(p);
    tbl = p->hostIndex;
    special = HeldIsSpecial(p->held, &info);
    p->v98 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x18), gHostAnimC[tbl], special, 0);
    p->fn8C = (void*)fn_80227594;
    *(int*)((char*)p + 0xA4) = 0x59;
    host = p->hosts[idx];
    p->vF0 = fn_800A1BC8(host->animA, host->animC, gHostAnimC[tbl], 0, 0);
    p->vFC = 0x59;
    PlayerSetHeldSlotIndex(p, QueryVar(p->held->handle, 0x8783452) == 0);
}

void PlayerClearFlag40000(Player* p) {
    p->flags &= ~0x40000;
}

void PlayerIdleUpdate(float dt, Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(dt, p);
}

void PlayerRideEnter(Player* p) {
    PlayerResetGunTimer(p);
    p->flags |= 0x40000;
    fn_80050EB0(p);
    PlayerInHost(p);
    PlayerSetHeldSlotMode(p, 3);
}

extern const int gRivetOffset[3];       // 0x80349C28

// While riding on a rivet host: moves the player onto the host's seat position.
static inline void PlayerSnapToSeat(Player* p) {
    int target;
    int v[3];
    u8 m[0x30];
    fn_80212730(p->handle);
    target = OwnerGetInfo(p->handle);
    v[0] = gRivetOffset[0];
    v[1] = gRivetOffset[1];
    v[2] = gRivetOffset[2];
    fn_8009F63C(m, target);
    fn_8009F44C(v, m, v);
    v[0] += *(int*)(target + 0x24);
    v[1] += *(int*)(target + 0x28);
    v[2] += *(int*)(target + 0x2C);
    p->pos[0] = v[0];
    p->pos[1] = v[1];
    p->pos[2] = v[2];
    p->x67C = (float)v[0];
    p->y680 = (float)v[1];
    p->z684 = (float)v[2];
    MarkLookupEntryFlag(p->handle);
}

void PlayerSeatUpdate(float dt, Player* p) {
    p->v638 = 1;
    p->flags7B4 &= ~1;
    if (p->hosts[1]->type == 0x66B15822) {
        PlayerSnapToSeat(p);
    }
    if ((p->flags & 2) == 0 && (p->flags & 8) == 0) {
        fn_800769A0(dt, p);
        PlayerSelectCollisionUpdate(dt, p);
        PlayerSetValue4(&p->sub7A4, 5);
        if (p->v314 == 0) {
            PlayerSetValue4(&p->sub7A4, 3);
        } else {
            PlayerResetGunTimer(p);
        }
        fn_80226E78(p);
    } else {
        PlayerSetValue4(&p->sub7A4, 3);
    }
}

void PlayerSeatEnter(Player* p) {
    PlayerInHost(p);
    p->v98 = *(int*)(*(int*)((char*)p->target + 0x84) + 0x98);
    p->fn8C = (void*)fn_80227594;
    *(int*)((char*)p + 0xA4) = 1;
    p->vF0 = *(int*)(*(int*)((char*)p->target + 0x84) + 0x94);
    p->vFC = 1;
}

void PlayerSeatUpdateB(float dt, Player* p) {
    if (p->hosts[1]->type == 0x66B15822) {
        PlayerSnapToSeat(p);
    }
    QueryVar(p->handle, 0x2DA8293C);
    QueryVar(p->handle, 0xDFF3097A);
    p->v638 = 0;
    p->flags7B4 &= ~1;
    fn_800769A0(dt, p);
    PlayerSelectCollisionUpdate(dt, p);
    p->v7D0 = 1;
    if ((p->flags678 & 0x80) == 0) {
        PlayerSetValue4(&p->sub7A4, 3);
    } else {
        p->flags678 &= ~0x80;
        PlayerSetActionMode(p, 3);
    }
    fn_802272D4(p);
    fn_80226E78(p);
}

void PlayerHostLeave(Player* p) {
    int idx = PlayerInHost(p);
    int type = p->hosts[idx]->type;
    if (type == 0x54E34D35 || type == 0x6B6F4068) {
        p->v98 = -1;
    } else {
        p->v98 = QueryVar(p->target->handle, 0x6A0D51C0);
    }
}

void PlayerSeatUpdateC(float dt, Player* p) {
    if (p->hosts[1]->type == 0x66B15822) {
        PlayerSnapToSeat(p);
    }
    p->v638 = 1;
    p->flags7B4 &= ~1;
    fn_800769A0(dt, p);
    PlayerSelectCollisionUpdate(dt, p);
    if ((p->flags678 & 0x80) != 0) {
        p->flags678 &= ~0x80;
        PlayerSetActionMode(p, 1);
    }
    PlayerSetValue4(&p->sub7A4, 1);
    p->v7D0 = 0;
    PlayerSetActionMode(p, 3);
}


// Per-host-type behaviour table (25 entries of 0x48 bytes), as in GunWithGrenade.cpp.
struct HostType {
    void (*enter)(Player* p);
    void (*update)(Player* p, int arg, float dt);
    void (*msg)(Player* p);
    u32 actions[14];
    void* name;
};

extern HostType gZoomHostTypes[25];

// TODO (not drafted yet, 0x90 bytes at 0x80226DE8)
// fn_80226DE8
// TODO (not drafted yet, 0x45C bytes at 0x80226E78)
// fn_80226E78
// TODO (not drafted yet, 0x2C0 bytes at 0x802272D4)
// fn_802272D4
// TODO (not drafted yet, 0x404 bytes at 0x80227594)
// fn_80227594
// TODO (not drafted yet, 0x90 bytes at 0x80227998)
// fn_80227998
// TODO (not drafted yet, 0x90 bytes at 0x80227A28)
// fn_80227A28
//@@80227AB8
extern "C" void* fn_80227AB8(Player* p);
void* fn_80227AB8(Player* p) {
    return gZoomHostTypes[p->sub7A4].name;
}
//@@end
//@@80227AD4
extern "C" void fn_80227AD4(Player* p);
void fn_80227AD4(Player* p) {
    if (gZoomHostTypes[p->sub7A4].msg) {
        gZoomHostTypes[p->sub7A4].msg(p);
    }
}
//@@end
//@@80227B18
extern "C" void fn_80227B18(Player* p, int arg, float dt);
void fn_80227B18(Player* p, int arg, float dt) {
    if (gZoomHostTypes[p->sub7A4].update) {
        gZoomHostTypes[p->sub7A4].update(p, arg, dt);
    }
}
//@@end
//@@80227B5C
extern "C" void fn_80227B5C(Player* p);
void fn_80227B5C(Player* p) {
    if (gZoomHostTypes[p->sub7A4].enter) {
        gZoomHostTypes[p->sub7A4].enter(p);
    }
}
//@@end
// TODO (not drafted yet, 0x23C bytes at 0x80227B9C)
// fn_80227B9C

// u_801E72E0.cpp (0x801E72E0-0x801E7DE0), split from the old u_801E241C range: the "SniperRound" effect type
// (descriptor 0x803B3B30: query 0x801E72E0, msg 0x801E72EC) and the first host states of the sniper host.
// Boundary evidence: .rodata pool 0x80348F68-0x80348FBB is its own ghost-action name table followed by
// "SniperRound" (the Sniper file has its own table at 0x80348F18); .sdata2 0x804D8EF0-0x804D8EF8.
// Boundary to report (not applied, it moves u_801E7DE0's start): the file continues past 0x801E7DE0. Its host
// table 0x803B3B60 (25 x 0x48 bytes) lists states from fn_801E7534 up to 0x801E926C, inside u_801E7DE0.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

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

// Per-host-type behaviour table (25 entries of 0x48 bytes), as in GunWithGrenade.cpp.
struct HostType {
    void (*enter)(Player* p);
    void (*update)(Player* p, int arg, float dt);
    void (*msg)(Player* p);
    u32 actions[14];
    void* name;
};

extern HostType gSniperHostTypes[25];

void PlayerResetGunTimer(void* p);
void PlayerSetValue4(void* p, int v);
void PlayerSelectCollisionUpdate(void* p, float dt);
extern "C" void fn_800769A0(float dt, void* p);
extern "C" void fn_80155A84(void* a, void* b, int c);
extern "C" int fn_80175EFC(void);

//@@801E72E0
extern "C" unsigned long fn_801E72E0(void* obj, int msg);
unsigned long fn_801E72E0(void* obj, int msg) {
    return 0x3DA260F7;
}
//@@end
// TODO (not drafted yet, 0x248 bytes at 0x801E72EC)
// fn_801E72EC
//@@801E7534
extern "C" void fn_801E7534(float dt, Player* p);
void fn_801E7534(float dt, Player* p) {
    p->v638 = 0;
    p->flags7B4 &= ~1;
    fn_800769A0(dt, p);
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
    if (p->v279C == -1 || fn_80175EFC() != 0) {
        PlayerSetValue4(&p->sub7A4, 3);
        fn_80155A84(&p->obj26E0, p->held, 1);
    }
}
//@@end
// TODO (not drafted yet, 0xFC bytes at 0x801E767C)
// fn_801E767C
// TODO (not drafted yet, 0xF0 bytes at 0x801E7778)
// fn_801E7778
// TODO (not drafted yet, 0x170 bytes at 0x801E7868)
// fn_801E7868
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
// TODO (not drafted yet, 0x160 bytes at 0x801E7A34)
// fn_801E7A34
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
// TODO (not drafted yet, 0x15C bytes at 0x801E7C1C)
// fn_801E7C1C
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

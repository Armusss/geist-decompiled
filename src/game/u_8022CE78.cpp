// u_8022CE78.cpp (0x8022CE78-0x8022EC64), split from the old u_80226DE8 range: the "MCG Ammo" and
// "MCG Grenade" effect types (descriptors 0x803BD1F8 / 0x803BD224) and the MCG host's states with its 25-entry
// host behaviour table 0x803BD250.
// Boundary evidence:
// - .rodata pool 0x80349D18-0x80349D78: its own copy of the ghost-action name table followed by "MCG Ammo" and
//   "MCG Grenade"; the MCG file before it has its own table at 0x80349CC8.
// - .sdata2: own pool 0x804D9758-0x804D9770 (int-to-float double 0x804D9760 duplicates the MCG file's
//   0x804D9730); .data 0x803BD1F8-0x803BD958 ends where Streamer.cpp's data starts.
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

extern HostType gMcgHostTypes[25];

void PlayerResetGunTimer(void* p);
void PlayerSetValue4(void* p, int v);
void PlayerSelectCollisionUpdate(void* p, float dt);
extern "C" void fn_800769A0(float dt, void* p);
extern "C" void fn_80155A84(void* a, void* b, int c);
extern "C" int fn_80175EFC(void);

//@@8022CE78
extern "C" unsigned long fn_8022CE78(void* obj, int msg);
unsigned long fn_8022CE78(void* obj, int msg) {
    return 0x3DA260F7;
}
//@@end
// TODO (not drafted yet, 0xA0 bytes at 0x8022CE84)
// fn_8022CE84
//@@8022CF24
extern "C" unsigned long fn_8022CF24(void* obj, int msg);
unsigned long fn_8022CF24(void* obj, int msg) {
    return 0x3DA260F7;
}
//@@end
// TODO (not drafted yet, 0x88 bytes at 0x8022CF30)
// fn_8022CF30
// TODO (not drafted yet, 0x538 bytes at 0x8022CFB8)
// fn_8022CFB8
// TODO (not drafted yet, 0x98 bytes at 0x8022D4F0)
// fn_8022D4F0
//@@8022D588
extern "C" void fn_8022D588(float dt, Player* p);
void fn_8022D588(float dt, Player* p) {
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
//@@8022D624
extern "C" void fn_8022D624(Player* p);
void fn_8022D624(Player* p) {
    p->fn8C = 0;
    if (p->vC4 != -1) {
        p->v98 = p->vC4;
    }
    if (p->v11C != -1) {
        p->vF0 = p->v11C;
    }
}
//@@end
//@@8022D650
extern "C" void fn_8022D650(Player* p);
void fn_8022D650(Player* p) {
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
// TODO (not drafted yet, 0xFC bytes at 0x8022D6D0)
// fn_8022D6D0
// TODO (not drafted yet, 0x98 bytes at 0x8022D7CC)
// fn_8022D7CC
// TODO (not drafted yet, 0xF0 bytes at 0x8022D864)
// fn_8022D864
//@@8022D954
extern "C" void fn_8022D954(Player* p);
void fn_8022D954(Player* p) {
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
// TODO (not drafted yet, 0x160 bytes at 0x8022D9B0)
// fn_8022D9B0
//@@8022DB10
extern "C" void fn_8022DB10(Player* p);
void fn_8022DB10(Player* p) {
    fn_80155A84(&p->obj26E0, p->held, 1);
}
//@@end
//@@8022DB3C
extern "C" void fn_8022DB3C(Player* p);
void fn_8022DB3C(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    p->flags |= 0x20000;
    p->unk78 = 1;
    PlayerSetValue4(&p->sub7A4, 0x13);
}
//@@end
// TODO (not drafted yet, 0x15C bytes at 0x8022DB98)
// fn_8022DB98
//@@8022DCF4
extern "C" void fn_8022DCF4(Player* p);
void fn_8022DCF4(Player* p) {
    p->flags &= ~0x40000;
}
//@@end
//@@8022DD04
extern "C" void fn_8022DD04(float dt, Player* p);
void fn_8022DD04(float dt, Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(p, dt);
}
//@@end
// TODO (not drafted yet, 0x1EC bytes at 0x8022DD5C)
// fn_8022DD5C
// TODO (not drafted yet, 0x29C bytes at 0x8022DF48)
// fn_8022DF48
// TODO (not drafted yet, 0x60 bytes at 0x8022E1E4)
// fn_8022E1E4
// TODO (not drafted yet, 0x1C8 bytes at 0x8022E244)
// fn_8022E244
//@@8022E40C
extern "C" void fn_8022E40C(Player* p);
void fn_8022E40C(Player* p) {
    p->v98 = -1;
}
//@@end
// TODO (not drafted yet, 0x98 bytes at 0x8022E418)
// fn_8022E418
//@@8022E4B0
extern "C" void fn_8022E4B0(Player* p);
void fn_8022E4B0(Player* p) {
    p->v98 = -1;
    p->v7D0 = 0;
}
//@@end
// TODO (not drafted yet, 0x31C bytes at 0x8022E4C4)
// fn_8022E4C4
// TODO (not drafted yet, 0x3A0 bytes at 0x8022E7E0)
// fn_8022E7E0
//@@8022EB80
extern "C" void* fn_8022EB80(Player* p);
void* fn_8022EB80(Player* p) {
    return gMcgHostTypes[p->sub7A4].name;
}
//@@end
//@@8022EB9C
extern "C" void fn_8022EB9C(Player* p);
void fn_8022EB9C(Player* p) {
    if (gMcgHostTypes[p->sub7A4].msg) {
        gMcgHostTypes[p->sub7A4].msg(p);
    }
}
//@@end
//@@8022EBE0
extern "C" void fn_8022EBE0(Player* p, int arg, float dt);
void fn_8022EBE0(Player* p, int arg, float dt) {
    if (gMcgHostTypes[p->sub7A4].update) {
        gMcgHostTypes[p->sub7A4].update(p, arg, dt);
    }
}
//@@end
//@@8022EC24
extern "C" void fn_8022EC24(Player* p);
void fn_8022EC24(Player* p) {
    if (gMcgHostTypes[p->sub7A4].enter) {
        gMcgHostTypes[p->sub7A4].enter(p);
    }
}
//@@end

// u_800E7E20.cpp (0x800E7E20-0x800E9418), split from the old u_800E1B5C range: a host's states (the same code
// as u_8022CE78.cpp / u_801E72E0.cpp host states).
// Boundary evidence: .rodata pool 0x80345068-0x803450B0 is its own ghost-action table (pointed to by the 0xD8
// byte table 0x8039C498 that fn_800E7E20 reads); the previous file has its own at 0x80344FC8. No .sdata2.
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

void PlayerResetGunTimer(void* p);
void PlayerSetValue4(void* p, int v);
void PlayerSelectCollisionUpdate(void* p, float dt);
extern "C" void fn_800769A0(float dt, void* p);
extern "C" void fn_80155A84(void* a, void* b, int c);
extern "C" int fn_80175EFC(void);

// TODO (not drafted yet, 0xBC bytes at 0x800E7E20)
// fn_800E7E20
//@@800E7EDC
extern "C" void fn_800E7EDC(float dt, Player* p);
void fn_800E7EDC(float dt, Player* p) {
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
//@@800E7F78
extern "C" void fn_800E7F78(Player* p);
void fn_800E7F78(Player* p) {
    p->fn8C = 0;
    if (p->vC4 != -1) {
        p->v98 = p->vC4;
    }
    if (p->v11C != -1) {
        p->vF0 = p->v11C;
    }
}
//@@end
//@@800E7FA4
extern "C" void fn_800E7FA4(Player* p);
void fn_800E7FA4(Player* p) {
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
// TODO (not drafted yet, 0xFC bytes at 0x800E8024)
// fn_800E8024
// TODO (not drafted yet, 0xC8 bytes at 0x800E8120)
// fn_800E8120
// TODO (not drafted yet, 0x170 bytes at 0x800E81E8)
// fn_800E81E8
//@@800E8358
extern "C" void fn_800E8358(Player* p);
void fn_800E8358(Player* p) {
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
// TODO (not drafted yet, 0x160 bytes at 0x800E83B4)
// fn_800E83B4
//@@800E8514
extern "C" void fn_800E8514(Player* p);
void fn_800E8514(Player* p) {
    fn_80155A84(&p->obj26E0, p->held, 1);
}
//@@end
//@@800E8540
extern "C" void fn_800E8540(Player* p);
void fn_800E8540(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    p->flags |= 0x20000;
    p->unk78 = 1;
    PlayerSetValue4(&p->sub7A4, 0x13);
}
//@@end
// TODO (not drafted yet, 0x15C bytes at 0x800E859C)
// fn_800E859C
//@@800E86F8
extern "C" void fn_800E86F8(Player* p);
void fn_800E86F8(Player* p) {
    p->flags &= ~0x40000;
}
//@@end
//@@800E8708
extern "C" void fn_800E8708(float dt, Player* p);
void fn_800E8708(float dt, Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(p, dt);
}
//@@end
// TODO (not drafted yet, 0x140 bytes at 0x800E8760)
// fn_800E8760
// TODO (not drafted yet, 0x94 bytes at 0x800E88A0)
// fn_800E88A0
// TODO (not drafted yet, 0x270 bytes at 0x800E8934)
// fn_800E8934
// TODO (not drafted yet, 0x94 bytes at 0x800E8BA4)
// fn_800E8BA4
// TODO (not drafted yet, 0x270 bytes at 0x800E8C38)
// fn_800E8C38
// TODO (not drafted yet, 0x34 bytes at 0x800E8EA8)
// fn_800E8EA8
// TODO (not drafted yet, 0x94 bytes at 0x800E8EDC)
// fn_800E8EDC
// TODO (not drafted yet, 0x170 bytes at 0x800E8F70)
// fn_800E8F70
// TODO (not drafted yet, 0xB8 bytes at 0x800E90E0)
// fn_800E90E0
// TODO (not drafted yet, 0xE4 bytes at 0x800E9198)
// fn_800E9198
// TODO (not drafted yet, 0xB8 bytes at 0x800E927C)
// fn_800E927C
// TODO (not drafted yet, 0xE4 bytes at 0x800E9334)
// fn_800E9334

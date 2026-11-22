// u_80226DE8.cpp (0x80226DE8-0x80227DD8): a host's camera zoom states (FOV printf "CurrentFOV = %0.9f",
// "zoom_out", "zoom_level1") and its 25-entry host behaviour table 0x803BCA60. The old u_80226DE8 range
// (0x80226DE8-0x8022EC64) held five files: this one, u_80227DD8, u_8022A180, u_8022BA30 (MCG effect) and
// u_8022CE78. Boundary evidence for the end at 0x80227DD8:
// - .sdata2: this file's pool is 0x804D9660-0x804D9678 (0.0f at 0x804D9660); the next file has its own 0.0f at
//   0x804D967C. fn_80227B9C (last function here) is the last reader of the first pool, fn_80227DD8 the first of
//   the second.
// - .rodata: pool 0x80349C34-0x80349CB8 (POSSESS..SCAN, then the FOV strings); .data 0x803BCA60-0x803BD168 (the
//   host table, read by fn_80227AB8..fn_80227B5C); .sbss 0x804D5408-0x804D5418.
// Boundary to report (not applied, it moves u_80222D60's end): the file really starts at 0x80225E34. The tail
// of u_80222D60 (PlayerHangExit 0x80225E34 .. PlayerSeatUpdateC) fills entries 1-22 of this file's host table
// 0x803BCA60 (entry 1's enter is fn_80226DE8 here), reads .sdata2 0x804D9658 / .rodata 0x80349C28 just before
// this file's pools, and is instruction-for-instruction the same code as u_8022CE78's host states.
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

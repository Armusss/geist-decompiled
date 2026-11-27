// u_8027C650.cpp: game code after the ODE files (0x8027C650-0x8027D3EC): player/possession state handlers
// (they work on a large actor through PlayerSetValue4 / PlayerSelectCollisionUpdate and the 25-entry state
// table at 0x803C0570). Names are descriptive guesses.
// Split from u_80277600: fn_8027C650 is the first non-ODE function (game calls only; ODE util.cpp ends with
// fn_8027C240); this part reads game .data (0x803C0570, 0x80393D7C) and .sdata2 0x804DA3B8 only.

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct ActorBig {
    u8 pad00[0x50];
    u32 flags50;            // 0x050
    u8 pad54[0x44];
    int target;             // 0x098
    u8 pad9C[0x5DC];
    u32 flags678;           // 0x678
    u8 pad67C[0x128];
    int stateIndex;         // 0x7A4
    u8 pad7A8[0x28];
    int timer;              // 0x7D0
};

// State table entry (0x48 bytes; 25 entries at 0x803C0570, the same shape as the player state table).
struct ActorStateEntry {
    u8 pad00[0x44];
    int value;              // 0x44
};
extern ActorStateEntry lbl_803C0570[25];
extern float lbl_804D3780;

// Functions called from other units under their placeholder names.
extern "C" void fn_8027D3E4(float v);

// TODO (not drafted yet, 0x9C bytes at 0x8027C650)
extern "C" void fn_8027C650(void);

// TODO (not drafted yet, 0x2C bytes at 0x8027C6EC)
extern "C" void fn_8027C6EC(void);

// TODO (not drafted yet, 0x80 bytes at 0x8027C718)
extern "C" void fn_8027C718(void);

// TODO (not drafted yet, 0xFC bytes at 0x8027C798)
extern "C" void fn_8027C798(void);

// TODO (not drafted yet, 0x78 bytes at 0x8027C894)
extern "C" void fn_8027C894(void);

// TODO (not drafted yet, 0x170 bytes at 0x8027C90C)
extern "C" void fn_8027C90C(void);

// TODO (not drafted yet, 0x60 bytes at 0x8027CA7C)
extern "C" void fn_8027CA7C(void);

// TODO (not drafted yet, 0x160 bytes at 0x8027CADC)
extern "C" void fn_8027CADC(void);

// TODO (not drafted yet, 0x2C bytes at 0x8027CC3C)
extern "C" void fn_8027CC3C(void);

// TODO (not drafted yet, 0x60 bytes at 0x8027CC68)
extern "C" void fn_8027CC68(void);

// TODO (not drafted yet, 0x15C bytes at 0x8027CCC8)
extern "C" void fn_8027CCC8(void);

void ActorClearFlag50(ActorBig* a) {
    a->flags50 &= ~0x40000;
}

// TODO (not drafted yet, 0x58 bytes at 0x8027CE34)
extern "C" void fn_8027CE34(void);

// TODO (not drafted yet, 0x110 bytes at 0x8027CE8C)
extern "C" void fn_8027CE8C(void);

// TODO (not drafted yet, 0x1A0 bytes at 0x8027CF9C)
extern "C" void fn_8027CF9C(void);

void ActorResetTarget(ActorBig* a) {
    a->target = -1;
}

// TODO (not drafted yet, 0x8C bytes at 0x8027D148)
extern "C" void fn_8027D148(void);

void ActorResetTargetAndTimer(ActorBig* a) {
    a->target = -1;
    a->timer = 0;
}

// TODO (not drafted yet, 0x5C bytes at 0x8027D1E8)
extern "C" void fn_8027D1E8(void);

// TODO (not drafted yet, 0x9C bytes at 0x8027D244)
extern "C" void fn_8027D244(void);

void ActorClearFlag678(ActorBig* a) {
    a->flags678 &= ~0x200;
}

void ActorSetFlag678(ActorBig* a) {
    a->flags678 |= 0x200;
}

int ActorGetStateValue(ActorBig* a) {
    return lbl_803C0570[a->stateIndex].value;
}

// TODO (not drafted yet, 0x44 bytes at 0x8027D31C)
extern "C" void fn_8027D31C(void);

// TODO (not drafted yet, 0x44 bytes at 0x8027D360)
extern "C" void fn_8027D360(void);

// TODO (not drafted yet, 0x40 bytes at 0x8027D3A4)
extern "C" void fn_8027D3A4(void);

void fn_8027D3E4(float v) {
    lbl_804D3780 = v;
}

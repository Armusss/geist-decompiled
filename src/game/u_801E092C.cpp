// u_801E092C.cpp (0x801E092C-0x801E0EDC): the "Lightning" effect type (descriptor .data 0x803ADDD0: name
// 0x80348DF8, fn_801E09C8, fn_801E0C78, fn_801E0DC8, fn_801E0D70), split from the old u_801D9A70 range.
// Boundary evidence: own .sdata2 pool 0x804D8D60-0x804D8D80 (int-to-float double 0x804D8D68, a duplicate of
// the trail file's 0x804D8D38); .bss 0x8049B3D8-0x8049B468 and .sdata 0x804D3280 read only by fn_801E092C,
// fn_801E0C78, fn_801E0D70 and fn_801E0DC8; .rodata "Lightning" 0x80348DF8. The next file (u_801E0EDC) has its
// own ghost-action name table at 0x80348E08.
// Names are descriptive guesses; unmatched against retail unless noted.

#include "game/types.h"
#include "game/string.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0


// TODO (not drafted yet, 0x9C bytes at 0x801E092C)
extern "C" void fn_801E092C(void);

// TODO (not drafted yet, 0x2B0 bytes at 0x801E09C8)
extern "C" void fn_801E09C8(void);

// TODO (not drafted yet, 0xF8 bytes at 0x801E0C78)
extern "C" void fn_801E0C78(void);

// TODO (not drafted yet, 0x58 bytes at 0x801E0D70)
extern "C" void fn_801E0D70(void);

// TODO (not drafted yet, 0x114 bytes at 0x801E0DC8)
extern "C" void fn_801E0DC8(void);

// u_801DE7A0.cpp (0x801DE7A0-0x801E092C): trail effects (TrailCreate / TrailDestroy, the trail list classes
// with vtables .data 0x803ADD90 / 0x803ADDB0, per-frame update and draw) and the effect-module manager hooks
// fn_801E0850..fn_801E08FC (reset / update / load / unload of the trail, crouch and timer modules), split from
// the old u_801D9A70 range.
// Boundary evidence:
// - start 0x801DE7A0: .sdata2 pool 0x804D8D28-0x804D8D60 (0.0f at 0x804D8D28 first read by fn_801DE7DC,
//   own int-to-float double 0x804D8D38); fn_801DE7A0 preloads the same texture (AssetFind 0xF2E2BEB8 /
//   0x17C7B021) that fn_801DE7DC uses, and is the trail module's load hook in fn_801E08CC.
// - .rodata 0x80348DD0-0x80348DF8, .data 0x803ADD28-0x803ADDD0, .sdata 0x804D3250-0x804D3280, .sbss
//   0x804D51F8-0x804D5220.
// - end 0x801E092C: the module-manager hooks have no data; they are kept here (guess). fn_801E092C reads
//   the Lightning file's .bss 0x8049B3D8 / 0x8049B458 and .sdata 0x804D3280.
// Names are descriptive guesses; unmatched against retail unless noted.

#include "game/types.h"
#include "game/string.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0


struct TrailDesc;

// TODO (not drafted yet, 0x3C bytes at 0x801DE7A0)
extern "C" void fn_801DE7A0(void);

// TODO (not drafted yet, 0x128 bytes at 0x801DE7DC)
extern "C" void fn_801DE7DC(void);

// TODO (not drafted yet, 0x5C bytes at 0x801DE904)
void TrailDestroy(int id);

// TODO (not drafted yet, 0x30 bytes at 0x801DE960)
int TrailCreate(TrailDesc* d);

// TODO (not drafted yet, 0x1DC bytes at 0x801DE990)
extern "C" void fn_801DE990(void);

// TODO (not drafted yet, 0xC4 bytes at 0x801DEB6C)
extern "C" void fn_801DEB6C(void);

// TODO (not drafted yet, 0x78 bytes at 0x801DEC30)
extern "C" void fn_801DEC30(void);

// TODO (not drafted yet, 0xEC bytes at 0x801DECA8)
extern "C" void fn_801DECA8(void);

// TODO (not drafted yet, 0x178 bytes at 0x801DED94)
extern "C" void fn_801DED94(void);

// TODO (not drafted yet, 0x28C bytes at 0x801DEF0C)
extern "C" void fn_801DEF0C(void);

// TODO (not drafted yet, 0x6A8 bytes at 0x801DF198)
extern "C" void fn_801DF198(void);

// TODO (not drafted yet, 0x108 bytes at 0x801DF840)
extern "C" void fn_801DF840(void);

// TODO (not drafted yet, 0x778 bytes at 0x801DF948)
extern "C" void fn_801DF948(void);

// TODO (not drafted yet, 0x3BC bytes at 0x801E00C0)
extern "C" void fn_801E00C0(void);

// TODO (not drafted yet, 0x27C bytes at 0x801E047C)
extern "C" void fn_801E047C(void);

// TODO (not drafted yet, 0x68 bytes at 0x801E06F8)
extern "C" void fn_801E06F8(void);

// TODO (not drafted yet, 0x48 bytes at 0x801E0760)
extern "C" void fn_801E0760(void);

// TODO (not drafted yet, 0x60 bytes at 0x801E07A8)
extern "C" void fn_801E07A8(void);

// TODO (not drafted yet, 0x8 bytes at 0x801E0808)
extern "C" void fn_801E0808(void);

// TODO (not drafted yet, 0x20 bytes at 0x801E0810)
extern "C" void fn_801E0810(void);

// TODO (not drafted yet, 0x20 bytes at 0x801E0830)
extern "C" void fn_801E0830(void);

// TODO (not drafted yet, 0x30 bytes at 0x801E0850)
extern "C" void fn_801E0850(void);

// TODO (not drafted yet, 0x4C bytes at 0x801E0880)
extern "C" void fn_801E0880(void);

// TODO (not drafted yet, 0x30 bytes at 0x801E08CC)
extern "C" void fn_801E08CC(void);

// TODO (not drafted yet, 0x30 bytes at 0x801E08FC)
extern "C" void fn_801E08FC(void);

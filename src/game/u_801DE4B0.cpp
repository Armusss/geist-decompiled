// u_801DE4B0.cpp (0x801DE4B0-0x801DE7A0): the "TentacleSpit" effect type (descriptor .data 0x803ADCF8: name
// 0x80348DBF, msg fn_801DE4BC at +0x1C, query fn_801DE4B0 at +0x20), split from the old u_801D9A70 range.
// Boundary evidence: own ghost-action name table + "TentacleSpit" at .rodata 0x80348D78-0x80348DD0; own
// .sdata2 pool 0x804D8D18-0x804D8D28 (360.0f, 4096.0f, int-to-float double 0x804D8D20, a duplicate of the
// creature file's 0x804D8CC8); .sdata 0x804D3248. fn_801DE4B0 returns the effect class hash 0x3DA260F7 like
// the SniperRound query fn_801E72E0.
// Names are descriptive guesses; unmatched against retail unless noted.

#include "game/types.h"
#include "game/string.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0


//@@801DE4B0
// Effect class query (descriptor +0x20): the same class hash as the SniperRound query fn_801E72E0.
extern "C" unsigned long fn_801DE4B0(void* obj, int msg);
unsigned long fn_801DE4B0(void* obj, int msg) {
    return 0x3DA260F7;
}
//@@end

// TODO (not drafted yet, 0x2E4 bytes at 0x801DE4BC)
extern "C" void fn_801DE4BC(void);

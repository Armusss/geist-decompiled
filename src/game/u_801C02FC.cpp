// u_801C02FC.cpp: 0x801C02FC-0x801C8B6C, 173 functions. A large character behaviour file (161 states).
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   string pool 0x80348718 (POSSESS..SCAN, "[FASSVE]", "\n")
//   state table 0x803ABBC0 (0xF18 = 161 entries of 0x18 bytes, every callback is in this range)
//   .sdata2 0x804D8A48-0x804D8AD0 (one 0.0 at 0x804D8A64 read from 0x801C0C14 to 0x801C8934),
//   .bss 0x80496D18 (asset handles filled by fn_801C11F0), .sdata 0x804D3218/0x804D321C
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


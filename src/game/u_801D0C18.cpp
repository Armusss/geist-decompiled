// u_801D0C18.cpp: 0x801D0C18-0x801D2694, 16 functions. The "Fire" actor type.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   actor type descriptor 0x803AD370 {0, "Fire", fn_801D12D8, fn_801D1054, fn_801D0E10, 0, fn_801D0FCC, ...}
//   string pool 0x803488D8 (POSSESS..SCAN, "Fire", the firetwo/darksmoke texture paths, "imp_flame")
//   texture table 0x803AD3A0, tables 0x803AD420/0x803AD7D0, 0x803ADB80; .sdata2 0x804D8BA0-0x804D8BC8
//   fn_801D0C18 is called by ImpFireBallSpawn
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


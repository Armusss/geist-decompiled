// u_801BF578.cpp: 0x801BF578-0x801C02FC, 9 functions. A character behaviour file with one state.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   string pool 0x803486C8: the shared POSSESS..SCAN table, then "\n" (0x80348710)
//   state table 0x803ABBA8 (one 0x18-byte entry: hash 0x002433D1, fn_801BFAB0/BFA60/BFA80/BF9BC)
//   .sdata2 0x804D8A20-0x804D8A40: its own 1.0/0.0 and int-to-float double (copies of these at
//   0x804D8A50/64/68 are read by the next file, so the cut is exactly at 0x801C02FC)
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


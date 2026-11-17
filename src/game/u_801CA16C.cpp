// u_801CA16C.cpp: 0x801CA16C-0x801CC6D0, 33 functions. A character behaviour file (8 states).
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   string pool 0x80348818 (POSSESS..SCAN, "QUERY", "[FMESVE]", "\n")
//   state table 0x803ACB38 (8 entries of 0x10 bytes, callbacks fn_801CACE0-fn_801CC34C)
//   .sdata2 0x804D8B00-0x804D8B44 (0.0 at 0x804D8B00 read from 0x801CA16C to 0x801CC34C), .sdata 0x804D3220
//   fn_801CC6C8 (return 1) is referenced by fn_801CBEEC, so it stays here
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


// u_801D8134.cpp: 0x801D8134-0x801D8768, 8 functions. App-system hooks (called from AppSystemsInit/Start/Shutdown).
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   .sbss 0x804D51D0-0x804D51D8, then a 4-byte pad at 0x804D51DC: small-data blocks are padded to 8 at
//   each file end (0x804D5194 after AsHeavy, 0x804D51CC after u_801D4738), so the next block is a new file
//   .sdata2 0x804D8C78 (its only constant)
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


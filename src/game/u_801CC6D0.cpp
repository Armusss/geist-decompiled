// u_801CC6D0.cpp: 0x801CC6D0-0x801CDFBC, 10 functions. The "Script Move" actor type.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   actor type descriptor 0x803ACBB8 {0, "Script Move" 0x80348878, fn_801CCCE0, fn_801CD484, fn_801CDE88,
//   0, fn_801CDE40, fn_801CCCDC, fn_801CCCD8}; fn_801CC6D0 registers it (the register function comes
//   first, as in Skybox.cpp); no POSSESS table in its pool
//   .data 0x803ACBE8, 0x803ACBF8; .sdata2 0x804D8B48-0x804D8B60
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


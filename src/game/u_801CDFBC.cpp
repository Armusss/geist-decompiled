// u_801CDFBC.cpp: 0x801CDFBC-0x801D0C18, 45 functions. The "rat" character file.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   function table 0x803ACC08 (fn_801CDFBC-fn_801CF038; read by fn_801CF3A4/fn_801CF57C), table 0x803ACC68
//   string pool 0x80348888 (POSSESS..SCAN, "rat_jump")
//   .sdata2 0x804D8B68-0x804D8B9C (its own 0.0/1.0/32767.0 copies), .bss 0x80496D98 (asset preload
//   fn_801D09FC at the end, called from PlayerInitGunState)
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


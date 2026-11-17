// u_801C9CE0.cpp: 0x801C9CE0-0x801CA16C, 3 functions. The "AshBullet" effect type.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   string pool 0x803487C0 (POSSESS..SCAN, "AshBullet")
//   type descriptor 0x803ACB08: create fn_801C9CEC (+0x14), destroy fn_801C9CE0 (+0x18), so the file starts
//   at the destroy callback
//   .sdata2 0x804D8AF0-0x804D8AFC; .bss 0x80496D58 filled by the asset preload fn_801C9F50 (the same
//   end-of-file preload shape as fn_801D09FC in u_801CDFBC; called from PlayerInitGunState)
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


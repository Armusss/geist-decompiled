// u_801D8994.cpp: 0x801D8994-0x801D8FD4, 9 functions. Graphics state helpers (GfxSetBlendMode).
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   .sbss 0x804D51F0/0x804D51F4 (used from fn_801D8994 to fn_801D8FC8), .sdata 0x804D3238
//   the next unit (u_801D9A70) starts its .sbss at 0x804D51F8
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


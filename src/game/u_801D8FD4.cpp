// u_801D8FD4.cpp: 0x801D8FD4-0x801D9A70, 8 functions. Message 0x74xx helpers.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   .sdata2 0x804D8C80-0x804D8CA0 (own 0.0 at 0x804D8C84 and int-to-float double: the scan cut)
//   fn_801D9580 is called only by fn_801D902C/fn_801D9114; fn_801D8FD4 sends msg 0x7404 and fn_801D96C4
//   sends 0x7403; fn_801D9028 is an empty hook called from AppSystemsInit
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


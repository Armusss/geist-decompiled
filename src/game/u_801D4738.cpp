// u_801D4738.cpp: 0x801D4738-0x801D8134, about 70 functions. Sound/physics objects with a material table.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   .sdata2 0x804D8C20-0x804D8C70 (0.0 at 0x804D8C38 read from 0x801D4DC4 to 0x801D7C24: one file)
//   material table 0x803ADB90 ("RED OAK", "STEEL", "RUBBER"), jump table 0x803ADBB4 (fn_801D6928),
//   vtable 0x803ADBD4 (fn_801D7FDC, fn_801D80EC, fn_801D8044, fn_801D808C; ScreenEffectSlot base),
//   rodata tables 0x80348AC0/0x80348BC0; .sbss 0x804D51A0-0x804D51C8 (+4 pad), .sdata 0x804D3228-0x804D3230
//   .bss 0x8049A9D8
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>


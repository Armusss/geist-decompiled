// u_801E241C.cpp (0x801E241C-0x801E3714): the "Plant" fear actor ("[FEAR_FOO_PLANT]"). The old u_801E241C range
// (0x801E241C-0x801E7DE0) held six files: this one, u_801E3714, u_801E47D0 (credits tips), u_801E5B3C,
// u_801E65F0 (Sniper effect) and u_801E72E0 (SniperRound effect + host states). Boundary evidence for the end:
// - .data: the actor descriptor 0x803ADF68 lists fn_801E241C, fn_801E2584, fn_801E29B0, fn_801E2DF8, fn_801E3324,
//   fn_801E335C; fn_801E3524 reads the 0x5B20-byte table 0x803ADFA0 that follows it (to 0x803B3AC0).
// - .sdata2: own pool 0x804D8DB0-0x804D8E00 (0.0f 0x804D8DC8, int-to-float double 0x804D8DC0); the next file has
//   its own 0.0f at 0x804D8E34 and double at 0x804D8E28. .rodata pool 0x80348EA0-0x80348F08 (POSSESS..SCAN,
//   "Plant", "[FEAR_FOO_PLANT]", "QUERY"); .sbss 0x804D5228-0x804D5230.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

// TODO (not drafted yet, 0x168 bytes at 0x801E241C)
// fn_801E241C
// TODO (not drafted yet, 0x178 bytes at 0x801E2584)
// fn_801E2584
// TODO (not drafted yet, 0x2B4 bytes at 0x801E26FC)
// fn_801E26FC
// TODO (not drafted yet, 0x448 bytes at 0x801E29B0)
// fn_801E29B0
// TODO (not drafted yet, 0x4DC bytes at 0x801E2DF8)
// fn_801E2DF8
// TODO (not drafted yet, 0x50 bytes at 0x801E32D4)
// fn_801E32D4
// TODO (not drafted yet, 0x38 bytes at 0x801E3324)
// fn_801E3324
// TODO (not drafted yet, 0x1C8 bytes at 0x801E335C)
// fn_801E335C
// TODO (not drafted yet, 0x1F0 bytes at 0x801E3524)
// fn_801E3524

// u_8022CE78.cpp (0x8022CE78-0x8022EC64), split from the old u_80226DE8 range: the "MCG Ammo" and
// "MCG Grenade" effect types (descriptors 0x803BD1F8 / 0x803BD224) and the MCG host's states with its 25-entry
// host behaviour table 0x803BD250.
// Boundary evidence:
// - .rodata pool 0x80349D18-0x80349D78: its own copy of the ghost-action name table followed by "MCG Ammo" and
//   "MCG Grenade"; the MCG file before it has its own table at 0x80349CC8.
// - .sdata2: own pool 0x804D9758-0x804D9770 (int-to-float double 0x804D9760 duplicates the MCG file's
//   0x804D9730); .data 0x803BD1F8-0x803BD958 ends where Streamer.cpp's data starts.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

// TODO (not drafted yet, 0xC bytes at 0x8022CE78)
// fn_8022CE78
// TODO (not drafted yet, 0xA0 bytes at 0x8022CE84)
// fn_8022CE84
// TODO (not drafted yet, 0xC bytes at 0x8022CF24)
// fn_8022CF24
// TODO (not drafted yet, 0x88 bytes at 0x8022CF30)
// fn_8022CF30
// TODO (not drafted yet, 0x538 bytes at 0x8022CFB8)
// fn_8022CFB8
// TODO (not drafted yet, 0x98 bytes at 0x8022D4F0)
// fn_8022D4F0
// TODO (not drafted yet, 0x9C bytes at 0x8022D588)
// fn_8022D588
// TODO (not drafted yet, 0x2C bytes at 0x8022D624)
// fn_8022D624
// TODO (not drafted yet, 0x80 bytes at 0x8022D650)
// fn_8022D650
// TODO (not drafted yet, 0xFC bytes at 0x8022D6D0)
// fn_8022D6D0
// TODO (not drafted yet, 0x98 bytes at 0x8022D7CC)
// fn_8022D7CC
// TODO (not drafted yet, 0xF0 bytes at 0x8022D864)
// fn_8022D864
// TODO (not drafted yet, 0x5C bytes at 0x8022D954)
// fn_8022D954
// TODO (not drafted yet, 0x160 bytes at 0x8022D9B0)
// fn_8022D9B0
// TODO (not drafted yet, 0x2C bytes at 0x8022DB10)
// fn_8022DB10
// TODO (not drafted yet, 0x5C bytes at 0x8022DB3C)
// fn_8022DB3C
// TODO (not drafted yet, 0x15C bytes at 0x8022DB98)
// fn_8022DB98
// TODO (not drafted yet, 0x10 bytes at 0x8022DCF4)
// fn_8022DCF4
// TODO (not drafted yet, 0x58 bytes at 0x8022DD04)
// fn_8022DD04
// TODO (not drafted yet, 0x1EC bytes at 0x8022DD5C)
// fn_8022DD5C
// TODO (not drafted yet, 0x29C bytes at 0x8022DF48)
// fn_8022DF48
// TODO (not drafted yet, 0x60 bytes at 0x8022E1E4)
// fn_8022E1E4
// TODO (not drafted yet, 0x1C8 bytes at 0x8022E244)
// fn_8022E244
// TODO (not drafted yet, 0xC bytes at 0x8022E40C)
// fn_8022E40C
// TODO (not drafted yet, 0x98 bytes at 0x8022E418)
// fn_8022E418
// TODO (not drafted yet, 0x14 bytes at 0x8022E4B0)
// fn_8022E4B0
// TODO (not drafted yet, 0x31C bytes at 0x8022E4C4)
// fn_8022E4C4
// TODO (not drafted yet, 0x3A0 bytes at 0x8022E7E0)
// fn_8022E7E0
// TODO (not drafted yet, 0x1C bytes at 0x8022EB80)
// fn_8022EB80
// TODO (not drafted yet, 0x44 bytes at 0x8022EB9C)
// fn_8022EB9C
// TODO (not drafted yet, 0x44 bytes at 0x8022EBE0)
// fn_8022EBE0
// TODO (not drafted yet, 0x40 bytes at 0x8022EC24)
// fn_8022EC24

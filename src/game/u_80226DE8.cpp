// u_80226DE8.cpp (0x80226DE8-0x80227DD8): a host's camera zoom states (FOV printf "CurrentFOV = %0.9f",
// "zoom_out", "zoom_level1") and its 25-entry host behaviour table 0x803BCA60. The old u_80226DE8 range
// (0x80226DE8-0x8022EC64) held five files: this one, u_80227DD8, u_8022A180, u_8022BA30 (MCG effect) and
// u_8022CE78. Boundary evidence for the end at 0x80227DD8:
// - .sdata2: this file's pool is 0x804D9660-0x804D9678 (0.0f at 0x804D9660); the next file has its own 0.0f at
//   0x804D967C. fn_80227B9C (last function here) is the last reader of the first pool, fn_80227DD8 the first of
//   the second.
// - .rodata: pool 0x80349C34-0x80349CB8 (POSSESS..SCAN, then the FOV strings); .data 0x803BCA60-0x803BD168 (the
//   host table, read by fn_80227AB8..fn_80227B5C); .sbss 0x804D5408-0x804D5418.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

// TODO (not drafted yet, 0x90 bytes at 0x80226DE8)
// fn_80226DE8
// TODO (not drafted yet, 0x45C bytes at 0x80226E78)
// fn_80226E78
// TODO (not drafted yet, 0x2C0 bytes at 0x802272D4)
// fn_802272D4
// TODO (not drafted yet, 0x404 bytes at 0x80227594)
// fn_80227594
// TODO (not drafted yet, 0x90 bytes at 0x80227998)
// fn_80227998
// TODO (not drafted yet, 0x90 bytes at 0x80227A28)
// fn_80227A28
// TODO (not drafted yet, 0x1C bytes at 0x80227AB8)
// fn_80227AB8
// TODO (not drafted yet, 0x44 bytes at 0x80227AD4)
// fn_80227AD4
// TODO (not drafted yet, 0x44 bytes at 0x80227B18)
// fn_80227B18
// TODO (not drafted yet, 0x40 bytes at 0x80227B5C)
// fn_80227B5C
// TODO (not drafted yet, 0x23C bytes at 0x80227B9C)
// fn_80227B9C

// u_80227DD8.cpp (0x80227DD8-0x8022A180), split from the old u_80226DE8 range: a texture-plotting overlay
// (TurfPlotPixel, GXInitTexObj, a 0x8049EF60 texture block) and the actor list at .sbss 0x804D5418.
// Boundary evidence:
// - .sdata2: own pool 0x804D9678-0x804D96F0 (0.0f at 0x804D967C, int-to-float doubles 0x804D96B0/0x804D96B8);
//   the previous file has its own 0.0f at 0x804D9660 and the next its own at 0x804D96F0. fn_80227DD8 is the
//   first reader, fn_8022A170 the last.
// - .rodata 0x80349CB8-0x80349CC8 (a 16-byte zero initialiser read by fn_80227DD8), .sbss 0x804D5418-0x804D542C.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

// TODO (not drafted yet, 0x800 bytes at 0x80227DD8)
// fn_80227DD8
// TODO (not drafted yet, 0x30 bytes at 0x802285D8)
// fn_802285D8
// TODO (not drafted yet, 0x80 bytes at 0x80228608)
// fn_80228608
// TODO (not drafted yet, 0x384 bytes at 0x80228688)
// fn_80228688
// TODO (not drafted yet, 0xC0 bytes at 0x80228A0C)
// fn_80228A0C
// TODO (not drafted yet, 0x18 bytes at 0x80228ACC)
// fn_80228ACC
// TODO (not drafted yet, 0x228 bytes at 0x80228AE4)
// fn_80228AE4
// TODO (not drafted yet, 0x208 bytes at 0x80228D0C)
// fn_80228D0C
// TODO (not drafted yet, 0x50 bytes at 0x80228F14)
// fn_80228F14
// TODO (not drafted yet, 0x47C bytes at 0x80228F64)
// fn_80228F64
// TODO (not drafted yet, 0x14 bytes at 0x802293E0)
// fn_802293E0
// TODO (not drafted yet, 0x444 bytes at 0x802293F4)
// fn_802293F4
// TODO (not drafted yet, 0x30C bytes at 0x80229838)
// fn_80229838
// TODO (not drafted yet, 0x610 bytes at 0x80229B44)
// fn_80229B44
// TODO (not drafted yet, 0x1C bytes at 0x8022A154)
// fn_8022A154
// TODO (not drafted yet, 0x10 bytes at 0x8022A170)
// fn_8022A170

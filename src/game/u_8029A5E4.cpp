// u_8029A5E4.cpp (0x8029A5E4-0x8029B070), split from the old u_80294548 range: two descriptor-driven actor
// extras (descriptors 0x803C30E0 and 0x803C3104 in .data, hashes 0xC5733545 / 0xFE16F9C0) and the 12-byte
// element array class (vtable 0x803C3128) the first one keeps at holder +0xF0.
// Boundary evidence:
// - .sdata2: own pool 0x804DA728-0x804DA760 (0.0f at 0x804DA730 duplicates the previous file's 0x804DA720 and
//   the next file's 0x804DA760; its own int-to-float doubles 0x804DA740/0x804DA758).
// - .data 0x803C30E0-0x803C3148: the first descriptor lists functions from both halves of the range
//   (fn_8029AE58, fn_8029A914, fn_8029ADC4, fn_8029AC60), so fn_8029A5E4..fn_8029B04C is one file; the
//   array vtable 0x803C3128 holds fn_8029AEF0, fn_8029B020, fn_8029AF58, fn_8029AFB0. The next file's code
//   uses only the vtables from 0x803C3148 on.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

// TODO (not drafted yet, 0x2D8 bytes at 0x8029A5E4)
// fn_8029A5E4
// TODO (not drafted yet, 0x58 bytes at 0x8029A8BC)
// fn_8029A8BC
// TODO (not drafted yet, 0x34C bytes at 0x8029A914)
// fn_8029A914
// TODO (not drafted yet, 0x164 bytes at 0x8029AC60)
// fn_8029AC60
// TODO (not drafted yet, 0x94 bytes at 0x8029ADC4)
// fn_8029ADC4
// TODO (not drafted yet, 0x98 bytes at 0x8029AE58)
// fn_8029AE58
// TODO (not drafted yet, 0x68 bytes at 0x8029AEF0)
// fn_8029AEF0
// TODO (not drafted yet, 0x58 bytes at 0x8029AF58)
// fn_8029AF58
// TODO (not drafted yet, 0x70 bytes at 0x8029AFB0)
// fn_8029AFB0
// TODO (not drafted yet, 0x8 bytes at 0x8029B020)
// fn_8029B020
// TODO (not drafted yet, 0x24 bytes at 0x8029B028)
// fn_8029B028
// TODO (not drafted yet, 0x24 bytes at 0x8029B04C)
// fn_8029B04C

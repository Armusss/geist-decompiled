// u_8022A180.cpp (0x8022A180-0x8022BA30), split from the old u_80226DE8 range: a follow/attack behaviour with
// its eight-entry enter/exit/update table 0x803BD168 (entries fn_8022AC74..fn_8022B1E8).
// Boundary evidence:
// - .sdata2: own pool 0x804D96F0-0x804D9720 (0.0f at 0x804D96F0, int-to-float double 0x804D9710); the previous
//   file has its own 0.0f at 0x804D967C, the next (MCG) its own 0.0f / double at 0x804D9728 / 0x804D9730.
// - .data 0x803BD168-0x803BD1C8 (the behaviour table); the next file starts with the MCG reset 0x8022BA30
//   listed in the MCG descriptor.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

// TODO (not drafted yet, 0xAF4 bytes at 0x8022A180)
// fn_8022A180
// TODO (not drafted yet, 0x2C bytes at 0x8022AC74)
// fn_8022AC74
// TODO (not drafted yet, 0x2C bytes at 0x8022ACA0)
// fn_8022ACA0
// TODO (not drafted yet, 0x100 bytes at 0x8022ACCC)
// fn_8022ACCC
// TODO (not drafted yet, 0x4C bytes at 0x8022ADCC)
// fn_8022ADCC
// TODO (not drafted yet, 0x144 bytes at 0x8022AE18)
// fn_8022AE18
// TODO (not drafted yet, 0x4C bytes at 0x8022AF5C)
// fn_8022AF5C
// TODO (not drafted yet, 0x4 bytes at 0x8022AFA8)
// fn_8022AFA8
// TODO (not drafted yet, 0x4 bytes at 0x8022AFAC)
// fn_8022AFAC
// TODO (not drafted yet, 0x48 bytes at 0x8022AFB0)
// fn_8022AFB0
// TODO (not drafted yet, 0x8 bytes at 0x8022AFF8)
// fn_8022AFF8
// TODO (not drafted yet, 0x4 bytes at 0x8022B000)
// fn_8022B000
// TODO (not drafted yet, 0x4 bytes at 0x8022B004)
// fn_8022B004
// TODO (not drafted yet, 0x38 bytes at 0x8022B008)
// fn_8022B008
// TODO (not drafted yet, 0x38 bytes at 0x8022B040)
// fn_8022B040
// TODO (not drafted yet, 0x38 bytes at 0x8022B078)
// fn_8022B078
// TODO (not drafted yet, 0x8 bytes at 0x8022B0B0)
// fn_8022B0B0
// TODO (not drafted yet, 0x130 bytes at 0x8022B0B8)
// fn_8022B0B8
// TODO (not drafted yet, 0x120 bytes at 0x8022B1E8)
// fn_8022B1E8
// TODO (not drafted yet, 0x8 bytes at 0x8022B308)
// fn_8022B308
// TODO (not drafted yet, 0x404 bytes at 0x8022B310)
// fn_8022B310
// TODO (not drafted yet, 0x1F4 bytes at 0x8022B714)
// fn_8022B714
// TODO (not drafted yet, 0x5C bytes at 0x8022B908)
// fn_8022B908
// TODO (not drafted yet, 0xCC bytes at 0x8022B964)
// fn_8022B964

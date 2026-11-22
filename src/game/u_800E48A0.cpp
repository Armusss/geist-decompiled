// u_800E48A0.cpp (0x800E48A0-0x800E7E20), split from the old u_800E1B5C range: ActorUpdate, ActorDefaultMsg,
// ActorInitBase, ActorSetPlacement and their helpers.
// Boundary evidence: .rodata 0x80344F48-0x80345068 (a 0x80-byte zero initialiser, then the ghost-action table,
// "QUERY", the play-state names, "Steel", "MISSING (%d)"); .sdata2 0x804D71F0-0x804D7230 (own 0.0f/1.0f and
// double, last constant 0x804D7228 read by fn_800E7D38); .data 0x8039C458-0x8039C498 (switch table).
// The next file starts at fn_800E7E20, the first reader of the second string pool (via 0x8039C498).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

// TODO (not drafted yet, 0x3C4 bytes at 0x800E48A0)
// ActorUpdate__FP5Actorf
// TODO (not drafted yet, 0x17E0 bytes at 0x800E4C64)
// ActorDefaultMsg__FP5ActoriPvi
// TODO (not drafted yet, 0x57C bytes at 0x800E6444)
// ActorInitBase__FP5Actor
// TODO (not drafted yet, 0x114 bytes at 0x800E69C0)
// fn_800E69C0
// TODO (not drafted yet, 0x284 bytes at 0x800E6AD4)
// fn_800E6AD4
// TODO (not drafted yet, 0x30 bytes at 0x800E6D58)
// fn_800E6D58
// TODO (not drafted yet, 0x304 bytes at 0x800E6D88)
// fn_800E6D88
// TODO (not drafted yet, 0x188 bytes at 0x800E708C)
// fn_800E708C
// TODO (not drafted yet, 0x3AC bytes at 0x800E7214)
// fn_800E7214
// TODO (not drafted yet, 0x1FC bytes at 0x800E75C0)
// fn_800E75C0
// TODO (not drafted yet, 0xD4 bytes at 0x800E77BC)
// fn_800E77BC
// TODO (not drafted yet, 0x8 bytes at 0x800E7890)
// fn_800E7890
// TODO (not drafted yet, 0xE4 bytes at 0x800E7898)
// ActorSetPlacement__FP5ActorP6PlaceF
// TODO (not drafted yet, 0xE8 bytes at 0x800E797C)
// fn_800E797C
// TODO (not drafted yet, 0xF8 bytes at 0x800E7A64)
// fn_800E7A64
// TODO (not drafted yet, 0xE8 bytes at 0x800E7B5C)
// fn_800E7B5C
// TODO (not drafted yet, 0xF4 bytes at 0x800E7C44)
// fn_800E7C44
// TODO (not drafted yet, 0xE8 bytes at 0x800E7D38)
// fn_800E7D38

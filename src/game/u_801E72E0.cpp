// u_801E72E0.cpp (0x801E72E0-0x801E7DE0), split from the old u_801E241C range: the "SniperRound" effect type
// (descriptor 0x803B3B30: query 0x801E72E0, msg 0x801E72EC) and the first host states of the sniper host.
// Boundary evidence: .rodata pool 0x80348F68-0x80348FBB is its own ghost-action name table followed by
// "SniperRound" (the Sniper file has its own table at 0x80348F18); .sdata2 0x804D8EF0-0x804D8EF8.
// Boundary to report (not applied, it moves u_801E7DE0's start): the file continues past 0x801E7DE0. Its host
// table 0x803B3B60 (25 x 0x48 bytes) lists states from fn_801E7534 up to 0x801E926C, inside u_801E7DE0.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

// TODO (not drafted yet, 0xC bytes at 0x801E72E0)
// fn_801E72E0
// TODO (not drafted yet, 0x248 bytes at 0x801E72EC)
// fn_801E72EC
// TODO (not drafted yet, 0x9C bytes at 0x801E7534)
// fn_801E7534
// TODO (not drafted yet, 0x2C bytes at 0x801E75D0)
// fn_801E75D0
// TODO (not drafted yet, 0x80 bytes at 0x801E75FC)
// fn_801E75FC
// TODO (not drafted yet, 0xFC bytes at 0x801E767C)
// fn_801E767C
// TODO (not drafted yet, 0xF0 bytes at 0x801E7778)
// fn_801E7778
// TODO (not drafted yet, 0x170 bytes at 0x801E7868)
// fn_801E7868
// TODO (not drafted yet, 0x5C bytes at 0x801E79D8)
// fn_801E79D8
// TODO (not drafted yet, 0x160 bytes at 0x801E7A34)
// fn_801E7A34
// TODO (not drafted yet, 0x2C bytes at 0x801E7B94)
// fn_801E7B94
// TODO (not drafted yet, 0x5C bytes at 0x801E7BC0)
// fn_801E7BC0
// TODO (not drafted yet, 0x15C bytes at 0x801E7C1C)
// fn_801E7C1C
// TODO (not drafted yet, 0x10 bytes at 0x801E7D78)
// fn_801E7D78
// TODO (not drafted yet, 0x58 bytes at 0x801E7D88)
// fn_801E7D88

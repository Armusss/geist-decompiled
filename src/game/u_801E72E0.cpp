// u_801E72E0.cpp (0x801E72E0-0x801E7534): the "SniperRound" effect type (descriptor 0x803B3B30: query
// 0x801E72E0, msg 0x801E72EC), split from the old u_801E241C range.
// Boundary evidence: .rodata pool 0x80348F68-0x80348FBB is its own ghost-action name table followed by
// "SniperRound" (the Sniper file has its own table at 0x80348F18, the sniper host file u_801E7534 its own at
// 0x80348FC0); .sdata2 0x804D8EF0-0x804D8EF8; .data 0x803B3B30-0x803B3B60 (the host table 0x803B3B60 that
// follows belongs to u_801E7534, whose first state fn_801E7534 ends this range).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

//@@801E72E0
extern "C" unsigned long fn_801E72E0(void* obj, int msg);
unsigned long fn_801E72E0(void* obj, int msg) {
    return 0x3DA260F7;
}
//@@end
// TODO (not drafted yet, 0x248 bytes at 0x801E72EC)
// fn_801E72EC

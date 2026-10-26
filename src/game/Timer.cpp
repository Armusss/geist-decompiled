// Timer.cpp: per-level hooks of a small timer module (0x8026A0F0-0x8026A158), called from the level code
// (u_801D9A70). Update clears a flag and adds the frame time to a tick counter; Load looks up the module's
// asset. Split from the HermitSpit unit: only these hooks use its .sdata and .sbss words.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

int AssetFind(u32 table, u32 key);  // 0x8007EB34

int sTimerAsset = -1;  // 0x804D3778
int sTimerFlag;        // 0x804D5510
int sTimerTicks;       // 0x804D5514

void TimerModuleReset(void) {
}

void TimerModuleUpdate(float dt) {
    sTimerFlag = 0;
    sTimerTicks += (int)dt;
}

void TimerModuleLoad(void) {
    sTimerAsset = AssetFind(0xF2E2BEB8, 0x6D8AA555);
}

void TimerModuleUnload(void) {
}

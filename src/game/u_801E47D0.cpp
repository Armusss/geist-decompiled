// u_801E47D0.cpp (0x801E47D0-0x801E5B3C), split from the old u_801E241C range: the credits tip screens
// (CreditsTipReset/Update/Show callers) with .sbss 0x804D5240-0x804D5270 and .sdata 0x804D32A8-0x804D32F0.
// Boundary evidence: own .sdata2 pool 0x804D8E60-0x804D8E80 (0.0f 0x804D8E60, double 0x804D8E70); the next file
// has its own 0.0f at 0x804D8E80 (scan cut exactly at fn_801E5B3C).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

void CreditsTipReset(void);
void CreditsTipUpdate(float dt);
void CreditsTipNop(void);

extern int lbl_804D5258;      // tips enabled
extern int lbl_804D525C;
extern int lbl_804D32B8;      // current tip, -1 = none
extern int lbl_804D5244;
extern float lbl_804D5240;    // tip timer
extern float lbl_804D32B0;
extern int lbl_804D524C;
extern float lbl_804D526C;

//@@801E47D0
extern "C" void fn_801E47D0(int on);
void fn_801E47D0(int on) {
    lbl_804D5258 = on;
}
//@@end
//@@801E47D8
extern "C" int fn_801E47D8(void);
int fn_801E47D8(void) {
    return lbl_804D32B8 != -1;
}
//@@end
//@@801E47F0
extern "C" void fn_801E47F0(int tip);
void fn_801E47F0(int tip) {
    if (lbl_804D5258 != 0) {
        if (lbl_804D525C == 0) {
            return;
        }
    }
    lbl_804D32B8 = tip;
    lbl_804D5244 = 4;
    lbl_804D5240 = lbl_804D32B0;
    lbl_804D524C = 0;
    lbl_804D525C = 0;
}
//@@end
//@@801E4830
extern "C" void fn_801E4830(void);
void fn_801E4830(void) {
    CreditsTipReset();
}
//@@end
// TODO (not drafted yet, 0x1C4 bytes at 0x801E4850)
// fn_801E4850
//@@801E4A14
extern "C" void fn_801E4A14(float dt);
void fn_801E4A14(float dt) {
    CreditsTipUpdate(dt);
    if (lbl_804D32B8 != -1) {
        lbl_804D5240 = (lbl_804D5240 - dt > 0.0f) ? lbl_804D5240 - dt : 0.0f;
    }
}
//@@end
//@@801E4A74
extern "C" void fn_80026660(int channel, int (*callback)(int, int, int), int unk);
extern "C" int fn_801E4AC4(int a, int b, int c);

extern "C" void fn_801E4A74(void);
void fn_801E4A74(void) {
    fn_80026660(0x59, fn_801E4AC4, 0);
    lbl_804D5244 = 0;
    lbl_804D32B8 = -1;
    lbl_804D5240 = 0.0f;
    lbl_804D526C = 0.0f;
    CreditsTipNop();
}
//@@end
// TODO (not drafted yet, 0xF8 bytes at 0x801E4AC4)
// fn_801E4AC4
// TODO (not drafted yet, 0xD00 bytes at 0x801E4BBC)
// fn_801E4BBC
// TODO (not drafted yet, 0x280 bytes at 0x801E58BC)
// fn_801E58BC

// FearMap2.c: the in-game "Objectives" screen and its state machine (0x801FD994-0x80202AB8).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ObjEntry {
    u8 unk0[0x10];
    float timer;        // 0x10
    u8 unk14[0x20 - 0x14];
    int flag;           // 0x20
    u8 unk24[4];
} ObjEntry;             // size 0x28

typedef struct ObjState {
    int (*enter)(void);
    void (*exit)(void);
    int (*update)(float dt, int arg);
    int unkC;
} ObjState;

extern int gObjCount;           // 0x804D5374
extern int gObjState;           // 0x804D5378
extern float gObjTime;          // 0x804D537C
extern int gObjCursor;          // 0x804D5384
extern int gObjButtonUp;        // 0x804D5388
extern int gObjButtonDown;      // 0x804D538C
extern float gObjLo;            // 0x804D3380
extern float gObjHi;            // 0x804D3384
extern ObjEntry gObjEntries[];  // 0x8049D6C0
extern ObjState gObjStates[];   // 0x803BA090

void fn_80202534(void);
void fn_80202728(void);
int fn_80200670(int arg);

// TODO (not drafted yet, 0x2F0 bytes at 0x801FD994): first function of the unit
// TODO (not drafted yet, 0x1B0 bytes at 0x801FDC84)
// TODO (not drafted yet, 0x104 bytes at 0x801FDE34)
// TODO (not drafted yet, 0xD0 bytes at 0x801FDF38)
// TODO (not drafted yet, 0x4 bytes at 0x801FE008)
// TODO (not drafted yet, 0x5C bytes at 0x801FE00C)
// TODO (not drafted yet, 0x268 bytes at 0x801FE068)
// TODO (not drafted yet, 0x7FC bytes at 0x801FE2D0)
// TODO (not drafted yet, 0x7E4 bytes at 0x801FEACC)
// TODO (not drafted yet, 0x6F4 bytes at 0x801FF2B0)
// TODO (not drafted yet, 0x3D4 bytes at 0x801FF9A4)
// TODO (not drafted yet, 0x8B8 bytes at 0x801FFD78)

// 0x80200630
void fn_80200630(void) {
    fn_80202534();
}

// 0x80200650
void fn_80200650(void) {
    fn_80202728();
}

// TODO (not drafted yet, 0x6B8 bytes at 0x80200670): draws the Objectives list
// TODO (not drafted yet, 0x2EC bytes at 0x80200D28)

// 0x80201014 advances the per-entry timers; returns 3 when every entry is ready, else the current state.
int ObjMenuAdvance(float dt) {
    float t = gObjTime;
    int n = gObjCount;
    ObjEntry* e = gObjEntries;
    int ok = 1;
    int i = 0;
    float limit;
    if (n > 0) {
        do {
            limit = 0.0f;
            if (i != 0) {
                limit = (float)i * ((gObjHi - gObjLo) / (float)n - 1.0f);
            }
            if (t <= limit) {
                ok = 0;
            } else if (0.0f < e->timer) {
                ok = 0;
                e->timer = e->timer - dt;
                if (e->timer < 0.0f) {
                    e->timer = 0.0f;
                }
            }
            e++;
            i++;
        } while (i < n);
    }
    return ok ? 3 : gObjState;
}

// 0x802010F8 resets the menu clock and clears every entry's flag.
int ObjMenuClearFlags(void) {
    int i;
    gObjTime = 0.0f;
    for (i = 0; i < gObjCount; i++) {
        gObjEntries[i].flag = 0;
    }
    return 1;
}

// 0x802011A4 resets the menu clock.
int ObjMenuResetClock(void) {
    gObjTime = 0.0f;
    return 1;
}

// 0x802011B4 sets every entry's timer to the base delay and resets the clock.
int ObjMenuSetTimers(void) {
    int i;
    float base = gObjLo;
    for (i = 0; i < gObjCount; i++) {
        gObjEntries[i].timer = base;
    }
    gObjTime = 0.0f;
    return 1;
}

// 0x80201260 runs the objectives state machine for one frame.
void fn_80201260(float dt, int arg, int closing) {   // ObjMenuUpdate; called from other units
    int next;
    gObjTime = gObjTime + dt;
    if (gObjState == 0 && gObjStates[1].enter() != 0) {
        gObjState = 1;
    }
    if (gObjState != 1 && closing != 0) {
        if (gObjStates[gObjState].exit != 0) {
            gObjStates[gObjState].exit();
        }
        if (gObjStates[1].enter != 0) {
            gObjStates[1].enter();
        }
        gObjState = 1;
    }
    if (gObjState == 1 && closing == 0) {
        if (gObjStates[1].exit != 0) {
            gObjStates[1].exit();
        }
        if (gObjStates[2].enter != 0) {
            gObjStates[2].enter();
        }
        gObjState = 2;
    }
    if (gObjStates[gObjState].update != 0) {
        next = gObjStates[gObjState].update(dt, arg);
        if (next != gObjState) {
            if (gObjStates[gObjState].exit != 0) {
                gObjStates[gObjState].exit();
            }
            gObjState = next;
            if (gObjStates[next].enter != 0) {
                gObjStates[next].enter();
            }
        }
    }
    fn_80200670(arg);
}

// TODO (not drafted yet, 0x30 bytes at 0x80201430): pad callback setting the up/down button flags
// TODO (not drafted yet, 0x27C bytes at 0x80201460)
// TODO (not drafted yet, 0x6E4 bytes at 0x802016DC)
// TODO (not drafted yet, 0x19C bytes at 0x80201DC0)
// TODO (not drafted yet, 0x64 bytes at 0x80201F5C)
// TODO (not drafted yet, 0x78 bytes at 0x80201FC0)
// TODO (not drafted yet, 0x58 bytes at 0x80202038)
// TODO (not drafted yet, 0x104 bytes at 0x80202090)
// TODO (not drafted yet, 0x3A0 bytes at 0x80202194)
// TODO (not drafted yet, 0x1F4 bytes at 0x80202534)
// TODO (not drafted yet, 0x1E8 bytes at 0x80202728)
// TODO (not drafted yet, 0x158 bytes at 0x80202910)
// TODO (not drafted yet, 0x10 bytes at 0x80202A68)
// TODO (not drafted yet, 0x40 bytes at 0x80202A78)

#ifdef __cplusplus
}
#endif

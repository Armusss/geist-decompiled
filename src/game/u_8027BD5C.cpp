// u_8027BD5C.cpp: ODE 0.5 util.cpp (0x8027BD5C-0x8027C650): dxProcessIslands (fn_8027BD5C, called by dWorldStep)
// and fn_8027C240 (auto-disable handling / body stepping with the 1/6 sinc term; guess).
// Split from u_80277600 on data evidence: own .sdata2 pool 0x804DA388-0x804DA3B8 (0.0f, 0.5f, 1e-4 double,
// 1.0f, 1/6, sqrt constants 0.5/3.0 doubles, 0.0 double), again duplicating the other ODE files' constants.

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

// TODO (not drafted yet, 0x4E4 bytes at 0x8027BD5C)
extern "C" void fn_8027BD5C(void);

// TODO (not drafted yet, 0x410 bytes at 0x8027C240)
extern "C" void fn_8027C240(void);

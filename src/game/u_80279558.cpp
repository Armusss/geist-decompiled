// u_80279558.cpp: ODE 0.5 step.cpp (0x80279558-0x8027B4F8): dInternalStepIsland (fn_80279558, called through
// dWorldStep) and its helper fn_8027A9C0 (guess).
// Split from u_80277600 on data evidence: own .sdata2 pool 0x804DA340-0x804DA358 (1.0f, -FLT_MAX, FLT_MAX,
// 0.0f, 0.9 double); 1.0f and 0.0f are also pooled by u_80278D2C and u_8027B4F8 (duplicates mean separate files).

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

// TODO (not drafted yet, 0x1468 bytes at 0x80279558)
extern "C" void fn_80279558(void);

// TODO (not drafted yet, 0xB38 bytes at 0x8027A9C0)
extern "C" void fn_8027A9C0(void);

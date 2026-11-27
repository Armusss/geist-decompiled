// u_80278D2C.cpp: ODE 0.5 math helpers (0x80278D2C-0x80279558); probably odemath.cpp (dNormalize3/4,
// dPlaneSpace use 1/sqrt(2)); guess.
// Split from u_80277600 on data evidence: own .sdata2 pool 0x804DA318-0x804DA340 (0.70710677f, the sqrt
// Newton constants 0.5/3.0 as doubles, 0.0, 1.0f) that the files before and after carry their own copies of;
// no code in 0x802775AC-0x80278D2C reads it.

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

// TODO (not drafted yet, 0x2B4 bytes at 0x80278D2C)
extern "C" void fn_80278D2C(void);

// TODO (not drafted yet, 0x16C bytes at 0x80278FE0)
extern "C" void fn_80278FE0(void);

// TODO (not drafted yet, 0x40C bytes at 0x8027914C)
extern "C" void fn_8027914C(void);

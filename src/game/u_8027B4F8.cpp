// u_8027B4F8.cpp: ODE 0.5 (0x8027B4F8-0x8027BD5C), probably rotation.cpp (half angles 0.5f, 2.0f in R-from-Q);
// guess.
// Split from u_80277600 on data evidence: own .sdata2 pool 0x804DA358-0x804DA388 (0.5f, 0.0f, 1.0f, sqrt
// constants 0.5/3.0 doubles, 0.0 double, 2.0f) duplicating constants of u_80278D2C / u_80279558 / u_8027BD5C;
// fn_8027B4F8 is the first reader of 0.5f at 0x804DA358, so the file starts there.

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

// TODO (not drafted yet, 0x78 bytes at 0x8027B4F8)
extern "C" void fn_8027B4F8(void);

// TODO (not drafted yet, 0x530 bytes at 0x8027B570)
extern "C" void fn_8027B570(void);

// TODO (not drafted yet, 0xA8 bytes at 0x8027BAA0)
extern "C" void fn_8027BAA0(void);

// TODO (not drafted yet, 0x78 bytes at 0x8027BB48)
extern "C" void fn_8027BB48(void);

// TODO (not drafted yet, 0x78 bytes at 0x8027BBC0)
extern "C" void fn_8027BBC0(void);

// TODO (not drafted yet, 0x74 bytes at 0x8027BC38)
extern "C" void fn_8027BC38(void);

// TODO (not drafted yet, 0x74 bytes at 0x8027BCAC)
extern "C" void fn_8027BCAC(void);

// TODO (not drafted yet, 0x3C bytes at 0x8027BD20)
extern "C" void fn_8027BD20(void);

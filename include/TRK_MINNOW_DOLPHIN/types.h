#ifndef _TRK_TYPES_H_
#define _TRK_TYPES_H_
/* Basic types for the MSL/Runtime/TRK sources (u32 is unsigned long, as in the CodeWarrior libraries). */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed long s32;
typedef unsigned long u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile s32 vs32;
typedef float f32;
typedef double f64;
typedef int BOOL;
#define FALSE 0
#define TRUE 1
#ifndef NULL
#define NULL ((void*)0)
#endif
#define ATTRIBUTE_ALIGN(num) __attribute__((aligned(num)))
#ifndef DECL_WEAK
#define DECL_WEAK __declspec(weak)
#endif
#define __REGISTER register
#include <cstddef>
#endif

#ifndef _TRK_OS_H_
#define _TRK_OS_H_
/* The few Dolphin OS declarations the TRK sources use. */
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct OSContext {
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    f64 fpr[32];
    u32 fpscr_pad;
    u32 fpscr;
    u32 srr0;
    u32 srr1;
    u16 mode;
    u16 state;
    u32 gqr[8];
    u32 psf_pad;
    f64 psf[32];
} OSContext;
typedef s16 __OSInterrupt;
typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, OSContext* context);
typedef void (*EXICallback)(s32 chan, OSContext* context);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
s32 OSEnableScheduler(void);
void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu);
DECL_WEAK void OSReport(const char* msg, ...);
typedef int OSHeapHandle;
extern volatile OSHeapHandle __OSCurrHeap;
void* OSGetArenaLo(void);
void* OSGetArenaHi(void);
void OSSetArenaLo(void* addr);
void* OSInitAlloc(void* arenaStart, void* arenaEnd, int maxHeaps);
OSHeapHandle OSCreateHeap(void* start, void* end);
OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);
void* OSAllocFromHeap(OSHeapHandle heap, u32 size);
void OSFreeToHeap(OSHeapHandle heap, void* ptr);
#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x) (((u32)(x)) & ~(32 - 1))
#ifdef __cplusplus
}
#endif
#endif

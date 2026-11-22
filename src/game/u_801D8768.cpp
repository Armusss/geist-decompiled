// u_801D8768.cpp: 0x801D8768-0x801D8994, 3 functions. Locked-cache copy (LCCopy, LCCopyEnd; called by VMem).
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   .sbss 0x804D51E0-0x804D51EC (its own 16-byte block between the u_801D8134 and u_801D8994 blocks)
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

#include <dolphin/os.h>
#include <dolphin/base/PPCArch.h>

// The locked cache as two 8 KB DMA buffers at 0xE0000000 and 0xE0002000.
static int sLCEnabled;      // 0x804D51E0
static void* sLCBuf[2];     // 0x804D51E4

static void LCCopyBlocks(void* dst, void* src, u32 size);

void LCCopyEnd(void) {
    if (sLCEnabled != 0) {
        LCDisable();
        sLCEnabled = 0;
    }
}

void LCCopy(void* dst, void* src, u32 size) {
    if (dst != 0 && src != 0 && size != 0) {
        if (sLCEnabled == 0) {
            LCEnable();
            sLCEnabled = 1;
            sLCBuf[0] = (void*)0xE0000000;
            sLCBuf[1] = (void*)0xE0002000;
        }
        LCCopyBlocks(dst, src, size);
    }
}

// Copies through the two locked-cache buffers: while one buffer is stored to dst the other one loads.
static void LCCopyBlocks(void* dst, void* src, u32 size) {
    u8* dst2 = (u8*)dst + 0x4000;
    u8* src2 = (u8*)src + 0x4000;
    void* srcs[2];
    void* dsts[2];
    int blocks;
    int rest;
    int i;
    u32 n;
    int wait;

    DCFlushRangeNoSync(src, size);
    DCInvalidateRange(dst, size);
    blocks = size >> 14;
    rest = size - (blocks << 14);
    PPCSync();
    for (i = 0; i < 2; i++) {
        srcs[i] = (u8*)src + i * 0x2000;
        dsts[i] = (u8*)dst + i * 0x2000;
        n = LCLoadData(sLCBuf[i], srcs[i], 0x2000);
    }
    if (blocks != 0) {
        LCQueueWait(n);
        wait = n;
        for (; blocks > 0; blocks--) {
            for (i = 0; i < 2; i++) {
                LCQueueWait(wait);
                LCStoreData(dsts[i], sLCBuf[i], 0x2000);
                srcs[i] = src2;
                src2 += 0x2000;
                dsts[i] = dst2;
                dst2 += 0x2000;
                LCLoadData(sLCBuf[i], srcs[i], 0x2000);
            }
        }
    }
    if (rest != 0) {
        LCQueueWait(8);
        LCStoreData(dsts[0], sLCBuf[0], rest);
    }
    LCQueueWait(0);
}

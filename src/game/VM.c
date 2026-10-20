// VM.c: page-level bookkeeping for the ARAM-backed virtual memory (0x80286AF0-0x8028703C).
// The virtual range is split into 4 KiB pages, each with a reference count (-1 = not mapped).
// Allocating a page pulls it in from ARAM (writing back whatever was there), deallocating drops the
// count and invalidates the page once nobody holds it. Compiled as C++ like the rest of the game.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include <dolphin/os.h>
#include <dolphin/ar.h>

extern "C" {
void* MemAlloc(int size, const char* file, int line);

// The VM page tables (0x803405xx-0x803408xx) and the ARAM slot helpers next to this file.
void* fn_803407D8(u32 page);
int fn_80340654(void* slot);
void fn_80340578(void* slot, u32 page);
void* fn_803407AC(void* slot);
void fn_8034067C(void* slot, int arg);
void fn_80340B6C(void (*cb)(void));
void fn_8028750C(void* slot);
u32 fn_8028757C(void* slot);
void fn_8028703C(void);
u32 __ARGetInterruptStatus(void);
void __ARClearInterrupt(void);

// State shared with the rest of the VM code (.sbss 0x804D565C-0x804D5694).
extern u8* lbl_804D565C;         // physical base of the paged memory
extern u32 lbl_804D5664;         // number of pages
extern void (*lbl_804D5668)(int op, void* page, u32 index, u32 usec, int dirty);
extern u8* lbl_804D5678;         // virtual base
extern u8* lbl_804D567C;         // virtual end
extern u8* lbl_804D5680;
extern u32 lbl_804D5684;
extern u32 lbl_804D5688;
extern u32 lbl_804D568C;         // free pages
extern s16* lbl_804D5690;        // per-page reference counts
}

void FatalError(const char* msg);
void DebugPrintf(const char* format, ...);

extern "C" void VMDeallocPages(void* addr, u32 size);
extern "C" void VMAllocPages(void* addr, u32 size);

inline u8* VMPageAddr(u32 page) { return lbl_804D565C + page * 0x1000; }

// Not referenced in Geist (dead-stripped), but its strings open VM.c's string pool.
void VMVerifyPages(void* addr, u32 size) {
    u32 count = ((size + 0x1000) >> 12) + 1;
    u32 i = (u32)((u8*)addr - lbl_804D5678) >> 12;

    count = i + count;
    if (count > lbl_804D5664) {
        FatalError("Invalid VM page range");
    }
    for (; i < count; i++) {
        if (lbl_804D5690[i] <= 0) {
            FatalError("VM page lock verify failed");
        }
    }
}

void VMDeallocPages(void* addr, u32 size) {
    u32 first = (u32)((u8*)addr - lbl_804D5678) >> 12;
    u32 count = first + ((size + 0x1000) >> 12) + 1;
    u32 i;

    if (count > lbl_804D5664) {
        FatalError("Invalid VM page range being deallocated");
    }
    for (i = first; i < count; i++) {
        lbl_804D5690[i]--;
        if (lbl_804D5690[i] == 0) {
            DCInvalidateRange(VMPageAddr(i), 0x1000);
            lbl_804D5690[i] = -1;
            lbl_804D568C++;
        } else if (lbl_804D5690[i] < 0) {
            DebugPrintf("Warning: VM page deallocated too many times\n");
            lbl_804D5690[i] = -1;
        }
    }
}

void VMAllocPages(void* addr, u32 size) {
    u32 first = (u32)((u8*)addr - lbl_804D5678) >> 12;
    u32 count = first + ((size + 0x1000) >> 12) + 1;
    u32 i;
    BOOL level;

    if (count > lbl_804D5664) {
        FatalError("Invalid VM page range being allocated");
    }
    level = OSDisableInterrupts();
    for (i = first; i < count; i++) {
        s16 ref = lbl_804D5690[i];

        if (ref == -1) {
            lbl_804D5690[i] = 1;
        } else if (ref == 0) {
            u32 start = OSTicksToMicroseconds(OSGetTime());
            u8* page;
            int dirty;
            void* slot;
            int intr;

            lbl_804D5690[i] = 1;
            dirty = 0;
            page = VMPageAddr(i);
            slot = fn_803407D8(i);
            while (ARGetDMAStatus()) {
            }
            intr = (u16)__ARGetInterruptStatus();
            if (slot) {
                if (fn_80340654(slot)) {
                    fn_8028750C(slot);
                    dirty = 1;
                    DCFlushRange(page, 0x1000);
                    ARStartDMA(0, (u32)page, fn_8028757C(slot), 0x1000);
                    while (ARGetDMAStatus()) {
                    }
                } else {
                    DCInvalidateRange(page, 0x1000);
                }
                fn_80340578(slot, i);
            }
            if (intr == 0) {
                __ARClearInterrupt();
            }
            if (lbl_804D5668) {
                lbl_804D5668(0, page, i, (u32)OSTicksToMicroseconds(OSGetTime()) - start, dirty);
            }
            lbl_804D568C--;
        } else {
            lbl_804D5690[i] = ref + 1;
        }
    }
    OSRestoreInterrupts(level);
}

void VMInitPages(void) {
    u32 i;

    lbl_804D5684 = 0;
    lbl_804D5678 = lbl_804D565C;
    lbl_804D5680 = lbl_804D565C;
    lbl_804D5690 = (s16*)MemAlloc(lbl_804D5664 * 2, "VM.c", 652);
    for (i = 0; i < lbl_804D5664; i++) {
        lbl_804D5690[i] = 0;
    }
    lbl_804D568C = lbl_804D5664;
    lbl_804D5688 = (lbl_804D5664 << 12) - 0x20000;
    lbl_804D567C = lbl_804D5678 + (lbl_804D5664 << 12);
}

int VMFlushOnePage(void) {
    static u32 sNext = 0;
    BOOL level;
    void* page;
    u32 i;
    void* slot;
    int intr;

    i = 0;
    OSDisableInterrupts();
    while (i++ <= lbl_804D5664) {
        if (lbl_804D5690[i] != 0) {
            continue;
        }
        slot = fn_803407D8(sNext++);
        if (sNext >= lbl_804D5664) {
            sNext = 0;
        }
        if (slot && fn_80340654(slot)) {
            page = fn_803407AC(slot);
            level = OSDisableInterrupts();
            while (ARGetDMAStatus()) {
            }
            intr = (u16)__ARGetInterruptStatus();
            DCFlushRange(page, 0x1000);
            ARStartDMA(0, (u32)page, fn_8028757C(slot), 0x1000);
            while (ARGetDMAStatus()) {
            }
            if (intr == 0) {
                __ARClearInterrupt();
            }
            OSRestoreInterrupts(level);
            fn_8028750C(slot);
            fn_8034067C(slot, 0);
            OSEnableInterrupts();
            return 1;
        }
    }
    OSEnableInterrupts();
    return 0;
}

void ARAMLevelLoaded(void) { fn_80340B6C(fn_8028703C); }

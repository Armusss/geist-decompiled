// Memory.cpp: Geist's memory manager (0x8009D710-0x8009DE94).
// - A system-heap wrapper (OSAllocFromHeap/OSFreeToHeap under a mutex) that zero-fills and reports
//   "Out of Memory <file> <line>" with the caller's __FILE__/__LINE__.
// - Two scratch ring buffers (0x160000 and 0x40000 bytes) handing out 32-byte aligned blocks.
// - A "virtual memory" pool object (VMem) living in a 0x6C0000-byte block; most game allocations go there.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef signed long s32;

struct OSMutex { u32 data[6]; };
extern "C" {
void OSInitMutex(OSMutex* mutex);
void OSLockMutex(OSMutex* mutex);
void OSUnlockMutex(OSMutex* mutex);
void* OSAllocFromHeap(int heap, u32 size);
void OSFreeToHeap(int heap, void* ptr);
extern volatile int __OSCurrHeap;
void* memset(void* dst, int val, u32 n);
void* memcpy(void* dst, const void* src, u32 n);
int sprintf(char* s, const char* format, ...);
}

void DebugPrintf(const char* format, ...);   // 0x8000781C
int SysHeapFreeBytes(void);                  // 0x80008B20
void FatalError(const char* message);         // 0x80008C38

// Block allocator used for the big pool (code in another file, 0x801BEC4C-0x801BF5xx).
class VMemList {
public:
    VMemList();                               // 0x801BF550
    u32 data[8];
};

class VMem {
public:
    VMem() : mUnk0(0), mUnk4(0), mUnk28(0) {}
    ~VMem() {}
    static void operator delete(void* p);

    int FreeBytes();                          // 0x801BEC4C
    int SizeOf(void* p);                      // 0x801BEC84
    void Free(void* p);                       // 0x801BF02C
    void* Alloc(int size, int a, int b);      // 0x801BF2D0
    void* Base();                             // 0x801BF444
    void Init(void* base, int size, int a, int b); // 0x801BF478

    int mUnk0;
    int mUnk4;
    VMemList mList;
    int mUnk28;
    int mUnk2C;
    int mUnk30;
};

VMem gVMem;
OSMutex gHeapMutex;
OSMutex gMemMutex2;
extern void* gPresetVMemBlock;                // 0x804D4088 (.sdata, another file)

static void* sScratchB;
static int sScratchBOffset;
static void* sScratchA;
static int sScratchAOffset;
static void* sMemCallback;

inline void VMem::operator delete(void* p) { gVMem.Free(p); }

void MemNop(void) {}

void MemSetCallback(void* cb) { sMemCallback = cb; }

extern "C" void MemFree(void* p, const char* file, int line) {
    OSLockMutex(&gHeapMutex);
    OSFreeToHeap(__OSCurrHeap, p);
    OSUnlockMutex(&gHeapMutex);
}

// Not referenced in Geist (dead-stripped), but its strings open Memory.cpp's string pool.
void MemOutOfMemory(void) {
    DebugPrintf("\n\n\nOUT OF MEMORY!  Gonna crash now...\n\n");
    FatalError("Out of Memory\n");
}

extern "C" void* MemAlloc(int size, const char* file, int line) {
    void* p;
    char msg[64];

    if (size > 0) {
        OSLockMutex(&gHeapMutex);
        p = OSAllocFromHeap(__OSCurrHeap, size);
        OSUnlockMutex(&gHeapMutex);

        if (p == 0) {
            sprintf(msg, "Out of Memory\n%s %d\n", file, line);
            DebugPrintf("Memory status:  %d bytes free\n", SysHeapFreeBytes());
            DebugPrintf("VMemory status: %d bytes free\n", gVMem.FreeBytes());
            FatalError(msg);
        }

        memset(p, 0, size);
        return p;
    }

    return 0;
}

void MemPrintStatus(void) {
    DebugPrintf("Memory status:  %d bytes free\n", SysHeapFreeBytes());
    DebugPrintf("VMemory status: %d bytes free\n", gVMem.FreeBytes());
}

void ScratchAReset(void) { sScratchAOffset = 0; }

void* ScratchAAlloc(int size) {
    int aligned = (size + 31) & ~31;
    void* p;

    if (aligned > 0x40000) {
        return 0;
    }

    if (sScratchAOffset + aligned > 0x40000) {
        sScratchAOffset = 0;
    }

    p = (char*)sScratchA + sScratchAOffset;
    sScratchAOffset += aligned;
    return p;
}

void ScratchNop(void) {}

void* ScratchBAlloc(int size) {
    int aligned = (size + 31) & ~31;
    void* p;

    if (aligned > 0x160000) {
        return 0;
    }

    if (sScratchBOffset + aligned > 0x160000) {
        sScratchBOffset = 0;
    }

    p = (char*)sScratchB + sScratchBOffset;
    sScratchBOffset += aligned;
    return p;
}

void* VRealloc(void* old, int size) {
    void* p = gVMem.Alloc(size, 0, 0);
    int oldSize;

    if (p == 0) {
        p = 0;
    } else {
        memset(p, 0, size);
    }

    oldSize = gVMem.SizeOf(old);

    if (p != 0 && old != 0 && oldSize != 0) {
        memcpy(p, old, oldSize < size ? oldSize : size);
    }

    gVMem.Free(old);
    return p;
}

void VFree(void* p) { gVMem.Free(p); }

void* VAlloc(int size) {
    void* p = gVMem.Alloc(size, 0, 0);

    if (p == 0) {
        return 0;
    }

    memset(p, 0, size);
    return p;
}

void MemShutdown(void) {
    MemFree(gVMem.Base(), 0, 0);
    MemFree(sScratchB, 0, 0);
    sScratchB = 0;
    sScratchBOffset = 0;
    MemFree(sScratchA, 0, 0);
    sScratchA = 0;
    sScratchAOffset = 0;
}

void MemInit(void) {
    void* block;

    OSInitMutex(&gHeapMutex);
    OSInitMutex(&gMemMutex2);

    if (gPresetVMemBlock != 0) {
        block = gPresetVMemBlock;
    } else {
        block = MemAlloc(0x6C0000, "Memory.cpp", 66);
    }
    gVMem.Init(block, 0x6C0000, 1, 0);

    sScratchB = MemAlloc(0x160000, "Memory.cpp", 74);
    sScratchBOffset = 0;

    sScratchA = MemAlloc(0x40000, "Memory.cpp", 77);
    sScratchAOffset = 0;
}

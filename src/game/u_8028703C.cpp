// u_8028703C.cpp: ARAM page transfer helpers (names are descriptive guesses).

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef long long s64;

// OS / runtime
extern "C" s64 OSGetTime(void);
extern "C" void* __RAS_OSDisableInterrupts_begin(void);
extern "C" void OSRestoreInterrupts(void* state);
extern "C" u32 ARGetDMAStatus(void);
extern "C" void ARStartDMA(u32 dir, void* dram, u32 aram, u32 len);
extern "C" void DCFlushRange(void* addr, u32 len);
extern "C" void DCInvalidateRange(void* addr, u32 len);
extern "C" void ICInvalidateRange(void* addr, u32 len);
extern "C" u16 __ARGetInterruptStatus(void);
extern "C" void __ARClearInterrupt(void);
extern "C" int sprintf(char* buf, const char* fmt, ...);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void PPCHalt(void);

// Audio driver / ARAM manager (callees are still fn_ placeholders in symbols.txt).
extern "C" void* fn_803407AC(void);
extern "C" int fn_803407D8(void);
extern "C" int fn_80340654(int stream);
extern "C" void fn_80340578(int stream, int page);
extern "C" void fn_803404E4(u32 addr, void* page, int pageIndex);

int VMAllocPageAny();

// Audio ARAM page pool base, transfer state and optional transfer callback.
extern "C" u32 lbl_804D3798;
extern "C" u32 lbl_804D5660;
extern "C" u8* lbl_804D565C;
extern "C" void (*lbl_804D5668)(u32 addr, void* page, int pageIndex, u32 elapsed, int copied);
extern "C" u32 lbl_804D5658;
extern "C" u32 lbl_804D566C;
extern "C" u32* lbl_804D5698;
extern "C" u32* lbl_804D569C;

// OS arena allocation
extern "C" void* OSGetArenaLo(void);
extern "C" void OSSetArenaLo(u32 lo);

// Audio driver registration helper (placeholder name) and its callback.
extern "C" void fn_80340418(void (*callback)(void));
extern "C" void fn_802870F0(void);
extern "C" void fn_80287E84(void* arg);
extern "C" int fn_80287EB8(u32 handle, int* data);

// Helpers of this unit, defined below.
extern "C" void fn_8028750C(u32 handle);
extern "C" u32 fn_8028757C(u32 handle);
u32 AramPageIsResident(u32 addr);
int AramPageIsMapped(u32 addr);
void AramReportUnmappedAddress(u32 addr);

// Converts the current OS time to the audio transfer timestamp unit
// (timebase ticks scaled by 8, divided by bus clock / 4 / 500000).
static inline u32 AramTimerNow(void)
{
    return (u32)((OSGetTime() << 3) / (*(volatile u32*)0x800000F8UL / 4 / 500000));
}

// Writes the pending 4 KB transfer buffer into the ARAM page belonging to `page`.
void AramWritePageFromDmaBuffer(u32 page)
{
    void* src = fn_803407AC();
    void* irq = __RAS_OSDisableInterrupts_begin();
    u16 intr;
    while (ARGetDMAStatus() != 0) {
    }
    intr = __ARGetInterruptStatus();
    DCFlushRange(src, 0x1000);
    ARStartDMA(0, src, fn_8028757C(page), 0x1000);
    while (ARGetDMAStatus() != 0) {
    }
    if (intr == 0) {
        __ARClearInterrupt();
    }
    OSRestoreInterrupts(irq);
    fn_8028750C(page);
}

// Saves/stages one 4 KB ARAM page: hands a finished audio stream page over to the
// ARAM manager, loads the page for `addr` into main memory, and reports timings.
void AramTransferPage(u32 addr)
{
    u32 start = AramTimerNow();
    u32 aligned = addr & 0xFFFFF000;
    int copied = 0;
    int pageIndex = VMAllocPageAny();
    u8* page = lbl_804D565C + pageIndex * 0x1000;
    int stream = fn_803407D8();
    void* irq = __RAS_OSDisableInterrupts_begin();
    u16 intr;
    while (ARGetDMAStatus() != 0) {
    }
    intr = __ARGetInterruptStatus();

    if (stream != 0) {
        if (fn_80340654(stream) != 0) {
            fn_8028750C(stream);
            copied = 1;
            DCFlushRange(page, 0x1000);
            ARStartDMA(0, page, fn_8028757C(stream), 0x1000);
            while (ARGetDMAStatus() != 0) {
            }
        }
        fn_80340578(stream, pageIndex);
    }

    if (AramPageIsResident(aligned) != 0) {
        ARStartDMA(1, page, fn_8028757C(aligned), 0x1000);
        while (ARGetDMAStatus() != 0) {
        }
        DCInvalidateRange(page, 0x1000);
        ICInvalidateRange(page, 0x1000);
    } else if (AramPageIsMapped(aligned) == 0) {
        AramReportUnmappedAddress(aligned);
    }

    if (intr == 0) {
        __ARClearInterrupt();
    }

    fn_803404E4(aligned, page, pageIndex);
    OSRestoreInterrupts(irq);

    if (lbl_804D5668 != 0) {
        u32 elapsed = AramTimerNow() - start;
        lbl_804D5668(addr, page, pageIndex, elapsed, copied);
    }
}

// TODO fn_8028703C (0xB4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8028703C(void)
{
}

// TODO fn_802870F0 (0x1F4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_802870F0(void)
{
}

// Reads the pending page count of the last ARAM transfer.
u32 AramGetPendingPageCount(void)
{
    return lbl_804D3798;
}

// Reads the ARAM manager's current page state.
u32 AramGetPageState(void)
{
    return lbl_804D5660;
}

// AramGetTransferState: returns the ARAM transfer state word polled by the
// audio driver. Kept under its placeholder name because u_80285ACC.cpp calls it.
extern "C" u32 lbl_804D5664;

extern "C" u32 fn_802872F4(void)
{
    return lbl_804D5664;
}

// Installs the ARAM transfer completion callback.
void AramSetTransferCallback(void (*callback)(u32 addr, void* page, int pageIndex, u32 elapsed, int copied))
{
    lbl_804D5668 = callback;
}

void AramAllocPageBuffer(void);
void AramInitPageTable(void);

// Initialises the ARAM manager once: derives the page count from the requested
// transfer size, installs the transfer callback and carves the ARAM window out
// of the current OS arena.
void AramInit(u32 size, u32 pageCount, u32 pageState)
{
    if (lbl_804D566C == 0) {
        void* irq = __RAS_OSDisableInterrupts_begin();
        lbl_804D5664 = size >> 12;
        lbl_804D566C = 1;
        lbl_804D3798 = pageCount;
        lbl_804D5658 = size;
        lbl_804D5660 = pageState;
        fn_80340418(fn_802870F0);
        AramAllocPageBuffer();
        AramInitPageTable();
        lbl_804D565C = (u8*)OSGetArenaLo();
        OSSetArenaLo((u32)lbl_804D565C + lbl_804D5658);
        OSRestoreInterrupts(irq);
    }
}

// Clears the 0x200-entry ARAM page descriptor table (0x4000 bytes) taken from
// the current OS arena.
void AramInitPageTable(void)
{
    lbl_804D5698 = (u32*)OSGetArenaLo();
    OSSetArenaLo((u32)lbl_804D5698 + 0x4000);
    for (u32 i = 0; i < 0x200; i++) {
        lbl_804D5698[i * 8 + 0] = 0;
        lbl_804D5698[i * 8 + 1] = 0;
        lbl_804D5698[i * 8 + 2] = 0;
        lbl_804D5698[i * 8 + 3] = 0;
        lbl_804D5698[i * 8 + 4] = 0;
        lbl_804D5698[i * 8 + 5] = 0;
        lbl_804D5698[i * 8 + 6] = 0;
        lbl_804D5698[i * 8 + 7] = 0;
    }
}

// Allocates the ARAM page descriptor table (0x8000 bytes) from the OS arena and
// zero-fills it. The table is indexed by 4 KB page number.
void AramAllocPageBuffer(void)
{
    lbl_804D569C = (u32*)OSGetArenaLo();
    OSSetArenaLo((u32)lbl_804D569C + 0x8000);

    int off = 0;
    for (int n = 0x400; n != 0; n--) {
        *(u32*)((u8*)lbl_804D569C + off) = 0;
        off += 4;
        *(u32*)((u8*)lbl_804D569C + off) = 0;
        off += 4;
        *(u32*)((u8*)lbl_804D569C + off) = 0;
        off += 4;
        *(u32*)((u8*)lbl_804D569C + off) = 0;
        off += 4;
        *(u32*)((u8*)lbl_804D569C + off) = 0;
        off += 4;
        *(u32*)((u8*)lbl_804D569C + off) = 0;
        off += 4;
        *(u32*)((u8*)lbl_804D569C + off) = 0;
        off += 4;
        *(u32*)((u8*)lbl_804D569C + off) = 0;
        off += 4;
    }
}

// Returns the ARAM residency flag of the 4 KB page containing `addr`.
u32 AramPageIsResident(u32 addr)
{
    return lbl_804D569C[addr >> 12] >> 31;
}

// Sets the resident/allocation bit of the page descriptor for `handle`.
// Kept under its placeholder name because VM.c calls it.
extern "C" void fn_8028750C(u32 handle)
{
    lbl_804D569C[handle >> 12] |= 0x80000000;
}

// Reports a virtual address that was never handed out by VMAlloc, then halts.
void AramReportUnmappedAddress(u32 addr)
{
    char msg[0x408];
    sprintf(msg, "Virtual address (%x) has not been allocated. Call VMAlloc on virtual address ranges before using them.", addr);
    PPCHalt();
}

// Returns non-zero when the ARAM page descriptor for `addr` has been allocated.
int AramPageIsMapped(u32 addr)
{
    return (lbl_804D569C[(addr >> 12) & 0x1FFF] & 0x7FFFFFFF) != 0;
}

// Returns the ARAM page descriptor flags for `addr`; reports the address and
// halts if it was never allocated.
// Kept under its placeholder name because other units call it.
extern "C" u32 fn_8028757C(u32 addr)
{
    u32 flags = lbl_804D569C[(addr >> 12) & 0x1FFF] & 0x7FFFFFFF;
    if (flags == 0) {
        AramReportUnmappedAddress(addr);
        flags = 0;
    }
    return flags;
}

// ARAM page allocation cursor state.
extern "C" char lbl_804D56A8;
extern "C" u32 lbl_804D56A4;
extern "C" u32 lbl_804D56A0;

// Maps `size` bytes of virtual address space starting at `addr` onto free ARAM
// pages, one 4 KB page at a time. Returns 0 when ARAM has no room left.
int AramMapVirtualRange(u32 addr, u32 size)
{
    if (lbl_804D56A8 == 0) {
        lbl_804D56A4 = 0;
        lbl_804D56A8 = 1;
    }

    u32 firstPage = AramGetPendingPageCount() >> 12;
    u32 pageState = AramGetPageState();
    u32 lastPage = firstPage + (pageState >> 12);

    if (lbl_804D56A4 < firstPage) {
        lbl_804D56A4 = firstPage;
    }

    if (AramGetPageState() < lbl_804D56A0 + size) {
        return 0;
    }

    u32 pages = (size + 0xFFF) >> 12;
    u32 offset = 0;
    if (size != 0) {
        do {
            u32 vaddr = addr + offset;
            do {
                lbl_804D56A4++;
                if (lastPage <= lbl_804D56A4) {
                    lbl_804D56A4 = firstPage;
                }
            } while (lbl_804D5698[lbl_804D56A4] != 0);

            lbl_804D5698[lbl_804D56A4] = vaddr;
            offset += 0x1000;
            lbl_804D569C[(vaddr >> 12) & 0x1FFF] = lbl_804D56A4 << 12;
            lbl_804D56A0 += 0x1000;
            pages--;
        } while (pages != 0);
    }
    return 1;
}

// One block of the ARAM page descriptor free list (links at 0x80/0x84).
struct AramPageFreeNode {
    u8 pad[0x80];
    AramPageFreeNode* next;
    AramPageFreeNode* prev;
};

extern "C" void VFree(void* ptr);
extern "C" AramPageFreeNode* lbl_804D56B0;

// AramFreePageList: unlinks and frees every block on the ARAM page descriptor
// free list. Kept under its placeholder name because u_80157730.cpp calls it.
extern "C" void fn_802876F0(void)
{
    AramPageFreeNode* node = lbl_804D56B0;
    while (node != 0) {
        if (node->prev != 0) {
            node->prev->next = node->next;
        }
        if (node->next != 0) {
            node->next->prev = node->prev;
        }
        if (node == lbl_804D56B0) {
            lbl_804D56B0 = node->next;
        }
        VFree(node);
        node = lbl_804D56B0;
    }
}

int AramPageNodeMessageHandler(u32 handle, int msg, int* data);

// Engine event/timer registration (placeholder callee).
extern "C" void fn_80026660(int slot, int (*callback)(u32, int, int*), int arg);
extern "C" u32 lbl_804D56B4;

// Resets the ARAM page free list and (re)registers the page-table maintenance
// timer that fn_8028787C services.
extern "C" void fn_80287764(void)
{
    lbl_804D56B0 = 0;
    lbl_804D56B4 = 0;
    fn_80026660(0x60, AramPageNodeMessageHandler, 0);
}

// Source of a pending ARAM page transfer: the owning object (if any) and the
// value handed to the page at +0x44.
struct AramPageSource {
    u8 pad[0x38];
    int ownerId; // 0x38
};

struct AramPageTransfer {
    AramPageSource* source; // 0x0
    u32 value;              // 0x4
};

struct AramPage {
    u8 pad[0x44];
    u32 value; // 0x44
};

// Seeds the ARAM page at `page` from a transfer request and tells the owning
// object that the page became ready (msg 0x407).
int AramPageSetupFromTransfer(AramPageTransfer* req, AramPage* page)
{
    page->value = req->value;
    if (req->source != 0) {
        SendMsg(req->source->ownerId, 0x407, page);
    }
    return 0;
}

// Copies the 0x48-byte ARAM page state record from `src` to `dst` and reports
// success. The record is 18 words assembled from the caller's transfer state.
struct AramPageStateRecord {
    u32 value[18];
};

int AramCopyPageStateRecord(AramPageStateRecord* src, AramPageStateRecord* dst)
{
    dst->value[0] = src->value[0];
    dst->value[1] = src->value[1];
    dst->value[2] = src->value[2];
    dst->value[3] = src->value[3];
    dst->value[4] = src->value[4];
    dst->value[5] = src->value[5];
    dst->value[6] = src->value[6];
    dst->value[7] = src->value[7];
    dst->value[8] = src->value[8];
    dst->value[9] = src->value[9];
    dst->value[10] = src->value[10];
    dst->value[11] = src->value[11];
    dst->value[12] = src->value[12];
    dst->value[13] = src->value[13];
    dst->value[14] = src->value[14];
    dst->value[15] = src->value[15];
    dst->value[16] = src->value[16];
    dst->value[17] = src->value[17];
    return 1;
}

// ARAM page nodes are 0x88-byte records kept on the lbl_804D56B0 list. A node
// starts with 0x30 bytes of transfer state that is copied to and from request
// buffers, followed by the page handle and its scheduling state.
struct AramPageHeader {
    u32 word[12];
};

struct AramPageNode {
    AramPageHeader header;   // 0x00
    void (*callback)(void*); // 0x30
    u32 pad34;               // 0x34
    u32 id;                  // 0x38
    void* state;             // 0x3c
    u32 pad40;               // 0x40
    int field44;             // 0x44
    u32 pad48;               // 0x48
    u32 flags;               // 0x4c
    int enabled;             // 0x50
    int field54;             // 0x54
    float f58;               // 0x58
    float f5C;               // 0x5c
    float f60;               // 0x60
    float f64;               // 0x64
    int i68;                 // 0x68
    int i6C;                 // 0x6c
    int i70;                 // 0x70
    int i74;                 // 0x74
    int i78;                 // 0x78
    int i7C;                 // 0x7c
    AramPageNode* next;      // 0x80
    AramPageNode* prev;      // 0x84
};

void* VAlloc(int size);
extern "C" void* memset(void* dst, int value, u32 size);
extern "C" int fn_802127A0(u32 id, int param1, int param2, int* data, int flag1, int flag2, int flag3);
extern "C" int fn_80212814(u32 id, int param1, int param2, int flag1, int flag2, int flag3);
extern "C" int fn_8021284C(u32 id);
// Late functions of this unit (defined below).
void AramPageSetupNodeCallback(u32 handle, int* data);
int AramPageUpdateNodeState(u32 handle, int* data);

// Initial page settings (audio driver defaults).
extern "C" float lbl_804DA4B0;
extern "C" float lbl_804DA4B4;
extern "C" float lbl_804DA4B8;

// Returns the ARAM page node whose handle id matches `key`.
static inline AramPageNode* AramFindPageNode(u32 key)
{
    AramPageNode* node = (AramPageNode*)lbl_804D56B0;
    for (; node != 0; node = node->next) {
        if ((node->id & 0xFFFFFF) == key) {
            break;
        }
    }
    return node;
}

// Message handler for the ARAM page node list: looks the page up by handle and
// services creation, transfer, timing and teardown requests.
int AramPageNodeMessageHandler(u32 handle, int msg, int* data)
{
    u32 key = handle & 0xFFFFFF;
    AramPageNode* node = AramFindPageNode(key);

    data[0] = (int)node;

    switch (msg) {
    case 0: {
        AramPageNode* found = (AramPageNode*)lbl_804D56B0;
        for (; found != 0; found = found->next) {
            if ((found->id & 0xFFFFFF) == key) {
                break;
            }
        }
        return 1;
    }
    case 0x400: {
        AramPageNode* found = AramFindPageNode(key);
        if (found == 0) {
            return 0;
        }
        data[0] = (int)found;
        return 1;
    }
    case 0x411: {
        AramPageNode* found = AramFindPageNode(key);
        if (found == 0) {
            return 0;
        }
        *(AramPageHeader*)(data + 4) = found->header;
        return 1;
    }
    case 0x413: {
        AramPageNode* found = AramFindPageNode(key);
        if (found == 0) {
            return 0;
        }
        found->header = *(AramPageHeader*)data[1];
        return 1;
    }
    case 0x414:
        return fn_80287EB8(handle, data);
    case 0x415: {
        AramPageNode* found = AramFindPageNode(key);
        if (found == 0) {
            return 0;
        }
        if (data[1] == -1) {
            found->field54 = 0;
            return fn_8021284C(found->id);
        }
        found->field54 = data[1];
        if (data[3] == 0) {
            return fn_80212814(found->id, data[1], data[2], 1, 0, 0);
        }
        return fn_802127A0(found->id, data[1], data[2], data + 4, 1, 0, 0);
    }
    case 0x41D: {
        AramPageNode* found = AramFindPageNode(key);
        switch (data[1]) {
        case 0x3FD77BF5:
            found->f5C = (float)data[3];
            found->i74 = 100;
            found->i78 = (int)found->f5C;
            found->i7C = 100;
            break;
        case 0x210E6C:
            if (found != 0) {
                if (found->prev != 0) {
                    found->prev->next = found->next;
                }
                if (found->next != 0) {
                    found->next->prev = found->prev;
                }
                if (found == (AramPageNode*)lbl_804D56B0) {
                    lbl_804D56B0 = (AramPageFreeNode*)found->next;
                }
                VFree(found);
            }
            break;
        case -0x705E9F6E:
            found->f64 = (float)data[3];
            break;
        case 0x7A281983:
            if (data[3] == 0) {
                found->enabled = 0;
                found->flags &= ~4;
            } else {
                found->enabled = 1;
                found->flags |= 4;
            }
            break;
        case 0x77C17F6F:
            found->f58 = (float)data[3];
            break;
        default:
            return 0;
        }
        return 1;
    }
    case 0x41E: {
        if (data[1] != 0x12D80) {
            return 0;
        }
        AramPageNode* found = (AramPageNode*)VAlloc(0x88);
        if (found != 0) {
            lbl_804D56B4++;
            memset(found, 0, 0x88);
            found->id = (lbl_804D56B4 & 0xFFFFFF) | 0x60000000;
            found->callback = fn_80287E84;
            found->field44 = -1;
            found->state = (u8*)found + 0x68;
            found->flags = 0x40;
            found->f64 = lbl_804DA4B0;
            found->enabled = 0;
            found->f58 = lbl_804DA4B4;
            found->f5C = lbl_804DA4B8;
            found->i68 = -100;
            found->i6C = 0;
            found->i70 = -100;
            found->i74 = 100;
            found->i78 = (int)found->f5C;
            found->i7C = 100;
            found->next = (AramPageNode*)lbl_804D56B0;
            found->prev = 0;
            if (lbl_804D56B0 != 0) {
                ((AramPageNode*)lbl_804D56B0)->prev = found;
            }
            lbl_804D56B0 = (AramPageFreeNode*)found;
            data[3] = (int)found->id;
        } else {
            data[3] = 0;
        }
        return 1;
    }
    default:
        return 0;
    }
}

// Actor / owner interfaces used by the page FX update below.
struct Actor;
struct Mtx34 {
    float m[12];
};

void ActorRelinkOwners(Actor* actor);
int OwnerGetInfo(u32 ownerId);
void ActorGetMatrix(Mtx34* out, Actor* actor);

extern "C" Mtx34* fn_800C6448(void);
extern "C" void fn_8028787C(u32 id, u32 handle, int* data, int flag);
extern "C" void fn_800A025C(float* out, Mtx34* mtx, int axis);
extern "C" void fn_800E1098(float angle, float* out, float* in);
extern "C" float fn_800E1020(float* v);
extern "C" int fn_800201E4(int ownerInfo, float* dir, u32 flags,
                           void (*callback)(void), void* state);
extern "C" int fn_8001A30C(float amount, int ownerInfo, float* pos, float* dir,
                           u32 flags, void (*callback)(void), void* params);

extern "C" float lbl_804DA4A0;
extern "C" float lbl_804DA4A4;

// Parameters for the page FX emitter started by fn_8001A30C.
struct AramPageFxParams {
    u32 pad00;        // 0x00
    int ownerInfo;    // 0x04
    u32 ownerId;      // 0x08
    float scale;      // 0x0c
    float pad10;      // 0x10
    u32 hash;         // 0x14
    u32 pad18[13];    // 0x18
    float f4c;        // 0x4c
    u32 pad50[2];     // 0x50
    u32 zero58;       // 0x58
};

// Page-node setup callback: starts the page transfer for the node stored in
// the message payload (installed as AramPageNode::callback).
void AramPageSetupNodeCallback(u32 handle, int* data)
{
    AramPageNode* node = *(AramPageNode**)data;
    fn_8028787C(node->id, handle, data, 0);
}

// Per-frame page update (message 0x414): relinks the node's owners, keeps the
// node positioned relative to its matrix and restarts the page FX emitter.
int AramPageUpdateNodeState(u32 handle, int* data)
{
    AramPageNode* node = AramFindPageNode(handle & 0xFFFFFF);
    if (node == 0) {
        return 0;
    }

    if (node->enabled == 1) {
        node->flags |= 4;
        ActorRelinkOwners((Actor*)node);

        float amount = *(float*)(data + 1);
        int ownerInfo = OwnerGetInfo(node->field54);
        node->f60 = node->f5C;
        if (ownerInfo != 0) {
            float vec0[3];
            float vec1[3];
            float vec2[3];
            Mtx34 mtx;
            AramPageStateRecord state;
            AramPageFxParams params;

            if (((u32)node->field54 >> 24) == 2) {
                mtx = *fn_800C6448();
            } else {
                ActorGetMatrix(&mtx, (Actor*)node);
            }

            fn_800A025C(vec0, &mtx, 1);
            vec1[0] = vec0[0] * lbl_804DA4A0 + mtx.m[9];
            vec1[1] = vec0[1] * lbl_804DA4A0 + mtx.m[10];
            vec1[2] = vec0[2] * lbl_804DA4A0 + mtx.m[11];

            fn_800E1098(node->f5C, vec2, vec0);
            vec2[0] = vec2[0] + vec1[0];
            vec2[1] = vec2[1] + vec1[1];
            vec2[2] = vec2[2] + vec1[2];

            params.ownerInfo = OwnerGetInfo(node->field54);
            params.ownerId = node->field54;
            params.scale = node->f58 * amount / lbl_804DA4A0;
            params.hash = 0x44DC6AB;
            params.f4c = lbl_804DA4A4;
            params.zero58 = 0;

            if (fn_800201E4(ownerInfo, vec1, 0x12000000,
                            (void (*)(void))AramCopyPageStateRecord,
                            &state) != 0) {
                vec2[0] = (float)(int)state.value[12];
                vec2[1] = (float)(int)state.value[13];
                vec2[2] = (float)(int)state.value[14];
                vec1[0] = vec2[0] - mtx.m[0];
                vec1[1] = vec2[1] - mtx.m[1];
                vec1[2] = vec2[2] - mtx.m[2];
                node->f60 = fn_800E1020(vec1);
                fn_800E1098(node->f60, vec1, vec0);
            } else {
                fn_800E1098(node->f5C, vec1, vec0);
            }

            fn_8001A30C(node->f64, ownerInfo, &mtx.m[9], vec1, 0x12000000,
                        (void (*)(void))AramPageSetupFromTransfer, &params);
        }
    }
    return 1;
}

// TODO fn_8028787C (0x608 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8028787C(u32 id, u32 handle, int* data, int flag)
{
}

// TODO fn_80287E84 (0x34 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80287E84(void* arg)
{
}

// TODO fn_80287EB8 (0x32C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" int fn_80287EB8(u32 handle, int* data)
{
    return 0;
}

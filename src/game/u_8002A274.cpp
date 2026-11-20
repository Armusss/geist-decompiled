// u_8002A274.cpp (0x8002A274-0x80037230), split from ModInfo.cpp.
// Integer-box geometry and the streaming slot table (file type 0x2E), plus large undrafted collision code.
// Boundaries: its .sdata2 pool 0x804D6170-0x804D61D0 holds its own copies of constants the previous file
// also has (0.0f 0x804D6178 vs 0x804D6118, 0.5f, 1.0f, the int-to-float doubles 0x804D61B8/0x804D61C0 vs
// 0x804D6128/0x804D6158); fn_8002A274 is the first reader and fn_80036DC4 the last. The next file has its own
// FLT_MAX (0x804D61D0 vs 0x804D6184 here). Its .sbss 0x804D42A8-0x804D4300 (stream slot table, chunk 6 records,
// debug option) and .bss 0x803DE5D8-0x803DEA58 are touched only from this range; the model database
// (.sbss 0x804D4300...) belongs to the next file. fn_800320F4 inlines fn_8002FD68 and StreamSlotsRelease
// (defined earlier here), so unlike ModInfo.cpp this file is built with automatic inlining.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef float f32;

// Stream slot table (lbl_804D42E4, count lbl_804D42E8), 0x40 bytes per slot.
struct StreamEntry;
struct StreamSlot {
    /*0x00*/ u8 unk00[0x0C - 0x00];
    /*0x0C*/ void* buffer;
    /*0x10*/ u8 unk10[0x30 - 0x10];
    /*0x30*/ u16 state;
    /*0x32*/ u8 unk32[0x38 - 0x32];
    /*0x38*/ int size;
    /*0x3C*/ u8 unk3C[0x40 - 0x3C];
};

extern "C" {
extern StreamSlot* lbl_804D42E4;
extern int lbl_804D42E8;
extern int lbl_804D42D4;
extern int lbl_804D42D8;
extern int lbl_804D2678;
extern int* lbl_804D42CC;
extern int lbl_804D42D0;
extern char* lbl_804D42BC;
extern int lbl_804D42C0;
extern int lbl_804D42A8;
extern int lbl_804D42AC;
extern int lbl_804D42B0;
extern int lbl_804D42B4;
extern int lbl_804D42C4;
extern int lbl_804D42DC;
extern int lbl_804D42E0;
extern int lbl_804D42EC;
extern int fn_80036688(void*, int, void*);
void fn_8002FC30(int, void*, void*);
}

typedef int (*FileHandlerFn)(void*, int, void*);
void StreamRead(int pri, StreamEntry* e, void (*done)(int, void*, void*), void* arg);
void StreamFree(void* p);
void GunStateTimerTick8017BAD0(void* p);
void GunStateStep8017BBBC(void* p);
void GunStateFinish8017BC5C(void* p);
void FileRemoveHandler(int type, FileHandlerFn fn, void* ctx);
void FileAddHandler(int type, FileHandlerFn fn, void* ctx);

static inline bool Fits(int avail, int need) {
    return avail > 0 && need > 0;
}

// 0x8002FA34
bool StreamSlotIdle(int index) {
    if (index >= 0 && index < lbl_804D42E8) {
        StreamSlot* s = &lbl_804D42E4[index];
        bool r = false;
        if (s->buffer != 0 || s->size == 0)
            r = true;
        return r;
    }
    return false;
}

// 0x8002FA84 (called from another unit under this placeholder name): total bytes of all slots, and of the slots in state 1 or 4
extern "C" void fn_8002FA84(int* loaded, int* total) {
    StreamSlot* s = lbl_804D42E4;
    int a = 0;
    int b = 0;
    int i;
    for (i = 0; i < lbl_804D42E8; i++) {
        b += s->size;
        if (s->state == 1 || s->state == 4)
            a += s->size;
        s++;
    }
    if (loaded != 0)
        *loaded = a;
    if (total != 0)
        *total = b;
}

// 0x8002FAE4: start reading every slot that is waiting (state 2)
void StreamSlotsStartPending() {
    if (lbl_804D42D4 != 0 && lbl_804D2678 != 0) {
        int i;
        lbl_804D42D4 = 0;
        for (i = 0; i < lbl_804D42E8; i++) {
            StreamSlot* s = &lbl_804D42E4[i];
            if (s->state == 2) {
                s->state = 1;
                StreamRead(0x2E, (StreamEntry*)&s->state, fn_8002FC30, s);
            }
        }
    }
}

// 0x8002FB90: release finished slots (state 3)
void StreamSlotsRelease() {
    if (lbl_804D42D8 != 0 && lbl_804D2678 != 0) {
        int i;
        lbl_804D42D8 = 0;
        for (i = 0; i < lbl_804D42E8; i++) {
            StreamSlot* s = &lbl_804D42E4[i];
            if (s->state == 3) {
                if (s->buffer != 0) {
                    s->state = 0;
                    StreamFree(s->buffer);
                    s->buffer = 0;
                } else {
                    lbl_804D42D8 = 1;
                }
            }
        }
    }
}

// TODO (not drafted yet, 0x138 bytes at 0x8002FC30): stream slot completion callback (declared above)

// 0x8002FD68 (called from another unit under this placeholder name)
extern "C" void fn_8002FD68() {
    int i;
    lbl_804D42D8 = 1;
    for (i = 0; i < lbl_804D42E8; i++)
        GunStateFinish8017BC5C(&lbl_804D42E4[i].state);
}

// 0x8002FDCC
void StreamSlotTick(int index) {
    if (index >= 0 && index < lbl_804D42E8) {
        lbl_804D42D8 = 1;
        GunStateTimerTick8017BAD0(&lbl_804D42E4[index].state);
    }
}

// 0x8002FE18
void StreamSlotStep(int index) {
    if (index >= 0 && index < lbl_804D42E8) {
        lbl_804D42D4 = 1;
        GunStateStep8017BBBC(&lbl_804D42E4[index].state);
    }
}

// 0x80031E48 (called from other units under this placeholder name): do two integer boxes overlap?
// Each box is {min x, y, z, max x, y, z}.
static inline int Abs(int x) {
    return x < 0 ? -x : x;
}

extern "C" int fn_80031E48(int* a, int* b) {
    int dx = (b[3] + b[0]) / 2 - (a[3] + a[0]) / 2;
    int dy = (b[4] + b[1]) / 2 - (a[4] + a[1]) / 2;
    int dz = (b[5] + b[2]) / 2 - (a[5] + a[2]) / 2;
    int hx = (a[3] - a[0]) / 2 + (b[3] - b[0]) / 2;
    int hy = (a[4] - a[1]) / 2 + (b[4] - b[1]) / 2;
    int hz = (a[5] - a[2]) / 2 + (b[5] - b[2]) / 2;
    if (hx < Abs(dx) || hy < Abs(dy) || hz < Abs(dz))
        return 0;
    return 1;
}

// 0x80031FAC (called from another unit under this placeholder name; Chunk6Load): file chunk handler for type 6 (array of 0x68-byte records)
extern "C" int fn_80031FAC(void* data, int size, void* ctx) {
    int* p = (size > 0) ? (int*)data : 0;
    if (*p == 6) {
        lbl_804D42D0 = p[1];
        lbl_804D42CC = Fits(size - 0x20, p[1] * 0x68) ? (int*)data + 8 : 0;
    }
    return 1;
}

// 0x80032014 (called from another unit under this placeholder name; ChunkNodesLoad): file chunk handler (0x48-byte records followed by 0x20-byte records)
extern "C" int fn_80032014(void* data, int size, void* ctx) {
    char* h = (size > 0) ? (char*)data : 0;
    int n = *(int*)(h + 4);
    char* p;
    int bytes;
    int pad;
    bytes = n * 0x48;
    pad = ((bytes + 0x1F) & 0xFFFFFFE0) - bytes;
    p = (char*)data + 0x20;
    p += bytes;
    p += pad;
    lbl_804D42BC = Fits(size - 0x20 - bytes - pad, *(int*)(h + 8) << 5) ? p : 0;
    lbl_804D42C0 = *(int*)(h + 8);
    return 1;
}

// 0x80032094 (called from another unit under this placeholder name; DebugOptionSet): debug option 0: value -1 toggles, 0/1 sets; returns the new value
extern "C" int fn_80032094(int which, int value) {
    int r = -1;
    switch (which) {
    case 0:
        switch (value) {
        case -1:
            r = (u32)__cntlzw(lbl_804D42A8) >> 5;
            lbl_804D42A8 = r;
            break;
        case 0:
        case 1:
            r = value;
            lbl_804D42A8 = value;
            break;
        default:
            r = lbl_804D42A8;
            break;
        }
        break;
    case 1:
        break;
    }
    return r;
}

static inline void SlotsRelease() {
    if (lbl_804D42D8 != 0 && lbl_804D2678 != 0) {
        int i;
        lbl_804D42D8 = 0;
        for (i = 0; i < lbl_804D42E8; i++) {
            StreamSlot* s = &lbl_804D42E4[i];
            if (s->state == 3) {
                if (s->buffer != 0) {
                    s->state = 0;
                    StreamFree(s->buffer);
                    s->buffer = 0;
                } else {
                    lbl_804D42D8 = 1;
                }
            }
        }
    }
}

static inline void SlotsFinishAll() {
    int i;
    lbl_804D42D8 = 1;
    for (i = 0; i < lbl_804D42E8; i++)
        GunStateFinish8017BC5C(&lbl_804D42E4[i].state);
}

// 0x800320F4 (called from another unit under this placeholder name; StreamSlotsShutdown)
extern "C" void fn_800320F4() {
    SlotsFinishAll();
    SlotsRelease();
    lbl_804D42AC = 0;
    lbl_804D42B0 = 0;
    lbl_804D42B4 = 0;
    lbl_804D42BC = 0;
    lbl_804D42C4 = 0;
    lbl_804D42CC = 0;
    lbl_804D42E0 = 0;
    lbl_804D42E4 = 0;
    lbl_804D42E8 = 0;
    lbl_804D42EC = 0;
    lbl_804D42D4 = 0;
    lbl_804D42D8 = 0;
}

// 0x80032200 (called from another unit under this placeholder name; StreamSlotsInit)
extern "C" int fn_80032200() {
    lbl_804D42AC = 0;
    lbl_804D42B0 = 0;
    lbl_804D42B4 = 0;
    lbl_804D42BC = 0;
    lbl_804D42C4 = 0;
    lbl_804D42CC = 0;
    lbl_804D42E0 = 0;
    lbl_804D42E4 = 0;
    lbl_804D42E8 = 0;
    lbl_804D42EC = 0;
    lbl_804D42DC = 0;
    lbl_804D42D4 = 0;
    lbl_804D42D8 = 0;
    FileAddHandler(0x2E, fn_80036688, 0);
    return 1;
}

// ---- Functions not drafted yet (declarations only; retail assembly is used for them) ----

// ---- Functions not drafted yet ----
// TODO (not drafted yet, 0x850 bytes at 0x8002A274)
extern "C" void fn_8002A274();
// TODO (not drafted yet, 0x28C bytes at 0x8002AAC4)
extern "C" void fn_8002AAC4();
// TODO (not drafted yet, 0x3CC bytes at 0x8002AD50)
extern "C" void fn_8002AD50();
// TODO (not drafted yet, 0x644 bytes at 0x8002B11C)
extern "C" void fn_8002B11C();
// TODO (not drafted yet, 0x8B4 bytes at 0x8002B760)
extern "C" void fn_8002B760();
// TODO (not drafted yet, 0xBD0 bytes at 0x8002C014)
extern "C" void fn_8002C014();
// TODO (not drafted yet, 0x178 bytes at 0x8002CBE4)
extern "C" void fn_8002CBE4();
// TODO (not drafted yet, 0x3C0 bytes at 0x8002CD5C)
extern "C" void fn_8002CD5C();
// TODO (not drafted yet, 0x3C4 bytes at 0x8002D11C)
extern "C" void fn_8002D11C();
// TODO (not drafted yet, 0x644 bytes at 0x8002D4E0)
extern "C" void fn_8002D4E0();
// TODO (not drafted yet, 0x914 bytes at 0x8002DB24)
extern "C" void fn_8002DB24();
// TODO (not drafted yet, 0xCD0 bytes at 0x8002E438)
extern "C" void fn_8002E438();
// TODO (not drafted yet, 0x2F0 bytes at 0x8002F108)
extern "C" void fn_8002F108();
// TODO (not drafted yet, 0x438 bytes at 0x8002F3F8)
extern "C" void fn_8002F3F8();
// TODO (not drafted yet, 0x204 bytes at 0x8002F830)
extern "C" void fn_8002F830();
// TODO (not drafted yet, 0x1450 bytes at 0x8002FE64)
extern "C" void fn_8002FE64();
// TODO (not drafted yet, 0x498 bytes at 0x800312B4)
extern "C" void fn_800312B4();
// TODO (not drafted yet, 0x6FC bytes at 0x8003174C)
extern "C" void fn_8003174C();
// TODO (not drafted yet, 0x210 bytes at 0x8003226C)
extern "C" void fn_8003226C();
// TODO (not drafted yet, 0x8B8 bytes at 0x8003247C)
extern "C" void fn_8003247C();
// TODO (not drafted yet, 0x120 bytes at 0x80032D34)
extern "C" void fn_80032D34();
// TODO (not drafted yet, 0x180 bytes at 0x80032E54)
extern "C" void fn_80032E54();
// TODO (not drafted yet, 0x224 bytes at 0x80032FD4)
extern "C" void fn_80032FD4();
// TODO (not drafted yet, 0x1E8 bytes at 0x800331F8)
extern "C" void fn_800331F8();
// TODO (not drafted yet, 0x70 bytes at 0x800333E0)
extern "C" void fn_800333E0();
// TODO (not drafted yet, 0x54 bytes at 0x80033450)
extern "C" void fn_80033450();
// TODO (not drafted yet, 0x314 bytes at 0x800334A4)
extern "C" void fn_800334A4();
// TODO (not drafted yet, 0x328 bytes at 0x800337B8)
extern "C" void fn_800337B8();
// TODO (not drafted yet, 0x654 bytes at 0x80033AE0)
extern "C" void fn_80033AE0();
// TODO (not drafted yet, 0x488 bytes at 0x80034134)
extern "C" void fn_80034134();
// TODO (not drafted yet, 0x1BC bytes at 0x800345BC)
extern "C" void fn_800345BC();
// TODO (not drafted yet, 0x6B8 bytes at 0x80034778)
extern "C" void fn_80034778();
// TODO (not drafted yet, 0x144 bytes at 0x80034E30)
extern "C" void fn_80034E30();
// TODO (not drafted yet, 0x1A8 bytes at 0x80034F74)
extern "C" void fn_80034F74();
// TODO (not drafted yet, 0x214 bytes at 0x8003511C)
extern "C" void fn_8003511C();
// TODO (not drafted yet, 0xC8 bytes at 0x80035330)
extern "C" void fn_80035330();
// TODO (not drafted yet, 0x3E8 bytes at 0x800353F8)
extern "C" void fn_800353F8();
// TODO (not drafted yet, 0x2A4 bytes at 0x800357E0)
extern "C" void fn_800357E0();
// TODO (not drafted yet, 0x59C bytes at 0x80035A84)
extern "C" void fn_80035A84();
// TODO (not drafted yet, 0x1E0 bytes at 0x80036020)
extern "C" void fn_80036020();
// TODO (not drafted yet, 0xE0 bytes at 0x80036200)
extern "C" void fn_80036200();
// TODO (not drafted yet, 0x36C bytes at 0x800362E0)
extern "C" void fn_800362E0();
// TODO (not drafted yet, 0x3C bytes at 0x8003664C)
extern "C" void fn_8003664C();
// TODO (not drafted yet, 0x244 bytes at 0x80036688)
// TODO (not drafted yet, 0x138 bytes at 0x800368CC)
extern "C" void fn_800368CC();
// TODO (not drafted yet, 0x1EC bytes at 0x80036A04)
extern "C" void fn_80036A04();
// TODO (not drafted yet, 0x1D4 bytes at 0x80036BF0)
extern "C" void fn_80036BF0();
// TODO (not drafted yet, 0x46C bytes at 0x80036DC4)
extern "C" void fn_80036DC4();

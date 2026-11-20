// u_8008DCDC.cpp (0x8008DCDC-0x80093BF0): the animation system: a table of 0x50-byte animation entries
// (lbl_804D4718: reference counting, apply/blend, clip lookup), a 300-entry table of 0x28-byte records
// (lbl_804D4724) and the 0x5C-byte record array helpers at the end.
// Boundary evidence (the end cut at 0x80093BF0; the next file is u_80093BF0.cpp, 2D drawing):
// - .sdata2: this file's pool is 0x804D6938-0x804D6960 (0.0f, u32 int-to-float double, 1.0f, -1.0f, 2^-15,
//   s32 int-to-float double); the next file has its own 0.0f/1.0f/doubles (0x804D69BC/0x804D69C0/0x804D69D0/
//   0x804D6A10). fn_80093B3C is the last reader of this pool, fn_80093BF0 the first of the next.
// - .sbss: only 0x804D4718-0x804D4727 is touched here; the next file uses 0x804D4728-0x804D4763.
// - fn_80093BCC (empty destructor loop over 0x5C-byte records, no data) is the partner of fn_80093B3C (the
//   constructor loop, which reads this pool's 0.0f) and is called from fn_80093AC4, so it stays here.
// G4 note: fn_80092E88 has the GC/2.0 pre-stwu load fingerprint (lwz r6,0x24(r3) / lfs before stwu); the file
// stays on GC/1.3.2 (per-file compiler changes need the user's approval).
// Only the small accessors are drafted so far; every larger function is declared with a TODO.
// Names are descriptive guesses; unmatched against retail unless noted.

typedef unsigned char u8;
typedef unsigned short u16;

typedef signed short s16;
typedef unsigned long u32;

#include "game/types.h"

struct StreamEntry;
int StreamRead(int type, StreamEntry* e, void (*cb)(int, void*, void*), void* arg);   // 0x8017C32C
void StreamFree(void* p);                                                            // 0x8017C1F4

// One animation table entry (0x50 bytes; only the fields used here). Starts with a stream entry header.
struct AnimEntry {
    u16 type;                 // 0x00 (1 = loaded clip, 2 = load requested, 3 = loaded and freeable, 4)
    u8 flag;                  // 0x02
    u8 pad03;                 // 0x03
    int id;                   // 0x04
    int size;                 // 0x08
    u8 pad0C[0x1C - 0x0C];    // 0x0C
    u32 unk1C;                // 0x1C
    void* data;               // 0x20
    int unk24[7];             // 0x24
    u16 unk40;                // 0x40
    u8 pad42[0x50 - 0x42];    // 0x42
};

// The table file (magic 50, count, then the entries).
struct AnimTable {
    int magic;                // 0x00
    u8 pad04[0x04];           // 0x04
    int count;                // 0x08
    AnimEntry* entries;       // 0x0C
};

extern AnimTable* lbl_804D4718;
extern int lbl_804D4720;
extern int lbl_804D471C;
extern int lbl_804D2A48;

// One 0x5C-byte record of a record set.
struct AnimItem {
    s16 state;                // 0x00
    s16 unk02;                // 0x02
    Vec3 unk04;               // 0x04
    int unk10;                // 0x10
    float unk14;              // 0x14
    float unk18;              // 0x18
    int unk1C;                // 0x1C
    float unk20;              // 0x20
    float unk24;              // 0x24
    int unk28;                // 0x28
    int flags;                // 0x2C
    float unk30;              // 0x30
    int unk34;                // 0x34
    int unk38;                // 0x38
    int unk3C;                // 0x3C
    int unk40;                // 0x40
    int unk44;                // 0x44
    float unk48;              // 0x48
    int unk4C;                // 0x4C
    int unk50;                // 0x50
    int unk54;                // 0x54
    float unk58;              // 0x58

    AnimItem() {
        unk10 = 0;
        unk14 = 0.0f;
        unk18 = 0.0f;
        unk1C = 0;
        unk20 = 0.0f;
        unk24 = 0.0f;
        unk28 = -1;
        unk30 = 0.0f;
        unk34 = 0;
        unk38 = -1;
        unk3C = 0;
        unk40 = -1;
        unk48 = 0.0f;
        unk4C = 0;
        unk50 = -1;
        unk54 = 0;
        state = 0;
        unk02 = 0;
        unk04.x = 0.0f;
        unk04.y = 0.0f;
        unk04.z = 0.0f;
    }
    ~AnimItem() {}
};

inline void* operator new(unsigned long, void* p) { return p; }

// A record set (0x28 bytes); lbl_804D4724 points at 300 of them. +0x14 is a Buffer-derived array of AnimItem.
struct AnimRecSet {
    int active;               // 0x00
    u8 pad04[0x0C - 0x04];    // 0x04
    int owner;                // 0x0C
    float unk10;              // 0x10
    void* arrVtbl;            // 0x14
    int arr04;                // 0x18
    u32 bytes;                // 0x1C
    AnimItem* items;          // 0x20
    u32 capacity;             // 0x24
};

extern AnimRecSet* lbl_804D4724;

extern "C" void fn_80092E88(void* clip, int track, Vec3* out, float t);

void GunStateTimerTick8017BAD0(void* p);   // 0x8017BAD0
void GunStateStep8017BBBC(void* p);        // 0x8017BBBC
void GunStateAbort8017BB74(void* p);       // 0x8017BB74
void GunStateAbort8017BB98(void* p);       // 0x8017BB98

// TODO (not drafted yet, 0x4F0 bytes at 0x8008DE4C): animation start helper called by the wrapper below
extern "C" void fn_8008DE4C(int a, int b, int c, int d);

// TODO (not drafted yet, 0x14C bytes at 0x8008DCDC)
extern "C" void fn_8008DCDC();

// Calls fn_8008DE4C with the last argument cleared.
extern "C" void fn_8008DE28(int a, int b, int c);
void fn_8008DE28(int a, int b, int c) {
    fn_8008DE4C(a, b, c, 0);
}

// TODO (not drafted yet, 0x290 bytes at 0x8008E33C): already named Fn8008E33C__Fiif in symbols.txt; real C++ signature still to be written
// Fn8008E33C__Fiif
// TODO (not drafted yet, 0x204 bytes at 0x8008E5CC)
extern "C" void fn_8008E5CC();

// Returns whether animation index i is a loaded clip with data.
extern "C" int fn_8008E7D0(int i);
int fn_8008E7D0(int i) {
    AnimTable* t = lbl_804D4718;
    u8 ok = 0;
    if (t != 0 && i >= 0 && i < t->count) {
        if (t->entries[i].type == 1 && t->entries[i].data != 0) {
            ok = 1;
        }
    }
    return ok;
}

// TODO (not drafted yet, 0x140 bytes at 0x8008E824)
extern "C" void fn_8008E824();
// TODO (not drafted yet, 0x138 bytes at 0x8008E964)
extern "C" void fn_8008E964();
// TODO (not drafted yet, 0x144 bytes at 0x8008EA9C)
extern "C" void fn_8008EA9C();
// TODO (not drafted yet, 0x150 bytes at 0x8008EBE0)
extern "C" void fn_8008EBE0();
// TODO (not drafted yet, 0xEC bytes at 0x8008ED30)
extern "C" void fn_8008ED30();
// TODO (not drafted yet, 0x238 bytes at 0x8008EE1C)
extern "C" void fn_8008EE1C();

// Returns the 16-bit field at +0x40 of animation i, or 0.
extern "C" int fn_8008F054(int i);
int fn_8008F054(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        return t->entries[i].unk40;
    }
    return 0;
}

//@@8008F090
// Returns the word at +0x1C of animation i as a float, or -1.
extern "C" float fn_8008F090(int i);
float fn_8008F090(int i) {
    AnimTable* t = lbl_804D4718;
    if (t == 0 || i < 0 || i >= t->count) {
        return -1.0f;
    }
    return t->entries[i].unk1C;
}
//@@end
// TODO (not drafted yet, 0x144 bytes at 0x8008F0EC)
extern "C" void fn_8008F0EC();
// TODO (not drafted yet, 0x148 bytes at 0x8008F230)
extern "C" void fn_8008F230();
// TODO (not drafted yet, 0x170 bytes at 0x8008F378)
extern "C" void fn_8008F378();

// Empty hooks called from many units.
extern "C" void fn_8008F4E8();
void fn_8008F4E8() {}
extern "C" void fn_8008F4EC();
void fn_8008F4EC() {}

// TODO (not drafted yet, 0x140 bytes at 0x8008F4F0)
extern "C" void fn_8008F4F0();
// TODO (not drafted yet, 0x294 bytes at 0x8008F630): already named AnimApply__FiPcii in symbols.txt; real C++ signature still to be written
// AnimApply__FiPcii
// TODO (not drafted yet, 0x314 bytes at 0x8008F8C4)
extern "C" void fn_8008F8C4();
// TODO (not drafted yet, 0x424 bytes at 0x8008FBD8)
extern "C" void fn_8008FBD8();
// TODO (not drafted yet, 0x13C bytes at 0x8008FFFC)
extern "C" void fn_8008FFFC();
//@@80090138
extern "C" void fn_80090138(int i);
void fn_80090138(int i) {
    int off;
    AnimRecSet* r;
    u32 k;
    if (i >= 0 && i < 300) {
        r = &lbl_804D4724[i];
        if (r->active != 0) {
            for (k = 0, off = 0; k < r->bytes / sizeof(AnimItem); k++, off += sizeof(AnimItem)) {
                if (*(s16*)((u8*)r->items + off) == 1) *(s16*)((u8*)r->items + off) = 2;
            }
        }
    }
}
//@@end
// TODO (not drafted yet, 0x144 bytes at 0x800901AC): already named Fn800901AC__Fii in symbols.txt; real C++ signature still to be written
// Fn800901AC__Fii
// TODO (not drafted yet, 0xA0 bytes at 0x800902F0)
extern "C" void fn_800902F0();
// TODO (not drafted yet, 0x160 bytes at 0x80090390)
extern "C" void fn_80090390();
// TODO (not drafted yet, 0x144 bytes at 0x800904F0)
extern "C" void fn_800904F0();
// TODO (not drafted yet, 0x144 bytes at 0x80090634)
extern "C" void fn_80090634();
// TODO (not drafted yet, 0x1FC bytes at 0x80090778)
extern "C" void fn_80090778();
// TODO (not drafted yet, 0x200 bytes at 0x80090974)
extern "C" void fn_80090974();
// TODO (not drafted yet, 0x1F8 bytes at 0x80090B74)
extern "C" void fn_80090B74();
//@@80090D6C
// Releases record set i and erases its records.
extern "C" void fn_8028B35C(void* arr, void* p, u32 bytes);
extern "C" void fn_80090D6C(int i);
void fn_80090D6C(int i) {
    if (lbl_804D4724 != 0 && i >= 0 && i < 300) {
        AnimRecSet* r = &lbl_804D4724[i];
        if (r->active != 0) {
            AnimItem* first;
            AnimItem* last;
            r->active = 0;
            first = r->items;
            last = (AnimItem*)((u8*)first + r->bytes);
            fn_8028B35C(&r->arrVtbl, first, (last - first) * sizeof(AnimItem));
        }
    }
}
//@@end
//@@80090DF8
static inline void AnimRecSetResetItems(AnimRecSet* r) {
    AnimItem* it;
    u32 k;
    int off;
    for (k = 0, off = 0; k < r->bytes / sizeof(AnimItem); k++, off += sizeof(AnimItem)) {
        it = (AnimItem*)((u8*)r->items + off);
        it->state = 0;
        it->unk28 = it->unk40 = it->unk10 = it->unk1C = -1;
        it->unk04.x = it->unk04.y = it->unk04.z = 0.0f;
        it->unk58 = 1.0f;
    }
}

extern "C" int fn_80090DF8(int i, int owner);
int fn_80090DF8(int i, int owner) {
    if (i >= 0 && i < 300) {
        AnimRecSet* r = &lbl_804D4724[i];
        if (r->active != 0) {
            AnimRecSetResetItems(r);
            r->unk10 = 0.0f;
            r->owner = owner;
            return i;
        }
    }
    return -1;
}
//@@end
// TODO (not drafted yet, 0x258 bytes at 0x80090EA4)
extern "C" void fn_80090EA4();
// TODO (not drafted yet, 0x2F0 bytes at 0x800910FC): already named Fn800910FC__Fiif in symbols.txt; real C++ signature still to be written
// Fn800910FC__Fiif
// TODO (not drafted yet, 0xC0 bytes at 0x800913EC)
extern "C" void fn_800913EC();
// TODO (not drafted yet, 0x2F4 bytes at 0x800914AC)
extern "C" void fn_800914AC();
//@@800917A0
// Height change of clip track 0 between time 0 and time t.
extern "C" float fn_800917A0(void* clip, float t);
float fn_800917A0(void* clip, float t) {
    Vec3 end;
    Vec3 start;
    fn_80092E88(clip, 0, &start, 0.0f);
    fn_80092E88(clip, 0, &end, t);
    return end.z - start.z;
}
//@@end
// TODO (not drafted yet, 0x3E8 bytes at 0x8009180C)
extern "C" void fn_8009180C();
// TODO (not drafted yet, 0x26C bytes at 0x80091BF4)
extern "C" void fn_80091BF4();
// TODO (not drafted yet, 0x5C8 bytes at 0x80091E60)
extern "C" void fn_80091E60();
// TODO (not drafted yet, 0x1A0 bytes at 0x80092428)
extern "C" void fn_80092428();
// TODO (not drafted yet, 0x3F8 bytes at 0x800925C8)
extern "C" void fn_800925C8();
// TODO (not drafted yet, 0x4C8 bytes at 0x800929C0)
extern "C" void fn_800929C0();
// TODO (not drafted yet, 0x1D4 bytes at 0x80092E88)
// (declared above)
//@@8009305C
// Sums the sizes of all animations (total, and loaded ones of type 1 or 4).
extern "C" void fn_8009305C(int* loaded, int* total);
void fn_8009305C(int* loaded, int* total) {
    AnimTable* t = lbl_804D4718;
    int sumLoaded = 0;
    int sumTotal = 0;
    if (t != 0) {
        int i;
        for (i = 0; i < t->count; i++) {
            AnimEntry* e = &t->entries[i];
            sumTotal += e->size;
            if (e->type == 1 || e->type == 4) {
                sumLoaded += e->size;
            }
        }
    }
    if (loaded != 0) *loaded = sumLoaded;
    if (total != 0) *total = sumTotal;
}
//@@end
//@@800930D0
// Requests a stream load for every animation in state 2.
extern "C" void fn_80093250(int, void*, void*);
extern "C" void fn_800930D0();
void fn_800930D0() {
    int i;
    if (lbl_804D471C != 0 && lbl_804D2A48 != 0) {
        lbl_804D471C = 0;
        for (i = 0; i < lbl_804D4718->count; i++) {
            AnimEntry* e = &lbl_804D4718->entries[i];
            if (e->type == 2) {
                e->type = 1;
                StreamRead(7, (StreamEntry*)e, fn_80093250, e);
            }
        }
    }
}
//@@end
//@@80093180
// Frees the data of every animation in state 3; flags a retry when one has none.
extern "C" void fn_80093180();
void fn_80093180() {
    int i;
    if (lbl_804D4720 != 0 && lbl_804D2A48 != 0) {
        lbl_804D4720 = 0;
        for (i = 0; i < lbl_804D4718->count; i++) {
            AnimEntry* e = &lbl_804D4718->entries[i];
            if (e->type == 3) {
                if (e->data != 0) {
                    void* data;
                    e->type = 0;
                    data = e->data;
                    if (e != 0) {
                        int k;
                        for (k = 0; k < 8; k++) ((int*)&e->data)[k] = 0;
                    }
                    StreamFree(data);
                } else {
                    lbl_804D4720 = 1;
                }
            }
        }
    }
}
//@@end
// TODO (not drafted yet, 0x12C bytes at 0x80093250)
// (declared above)

// Aborts animation i (variant A).
void AnimAbortA(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D4720 = 1;
        GunStateAbort8017BB74(&t->entries[i]);
    }
}

// Aborts animation i (variant B).
void AnimAbortB(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D471C = 1;
        GunStateAbort8017BB98(&t->entries[i]);
    }
}

// Ticks animation i's timer.
void AnimUnref(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D4720 = 1;
        GunStateTimerTick8017BAD0(&t->entries[i]);
    }
}

// Sets animation i's byte at +2 and steps it.
extern "C" void fn_80093478(int i, u8 flag);
void fn_80093478(int i, u8 flag) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D471C = 1;
        AnimEntry* e = &t->entries[i];
        e->flag = flag;
        GunStateStep8017BBBC(e);
    }
}

// Starts animation i and clears its byte at +2.
void AnimRef(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D471C = 1;
        AnimEntry* e = &t->entries[i];
        e->flag = 0;
        GunStateStep8017BBBC(e);
    }
}

// TODO (not drafted yet, 0x1E4 bytes at 0x8009352C)
extern "C" void fn_8009352C();
// TODO (not drafted yet, 0x1B0 bytes at 0x80093710)
extern "C" void fn_80093710();

// Empty hook.
extern "C" void fn_800938C0();
void fn_800938C0() {}

// TODO (not drafted yet, 0xCC bytes at 0x800938C4)
extern "C" void fn_800938C4();
//@@80093990
// Installs the animation table file (magic 50); the entries follow the 16-byte header.
extern "C" int fn_80093990(void* data, int size);
int fn_80093990(void* data, int size) {
    bool ok;
    lbl_804D4718 = (size > 0) ? (AnimTable*)data : 0;
    if (lbl_804D4718->magic != 50) {
        lbl_804D4718 = 0;
        return 0;
    }
    ok = false;
    if (size - 16 > 0 && lbl_804D4718->count * sizeof(AnimEntry) != 0) {
        ok = true;
    }
    lbl_804D4718->entries = ok ? (AnimEntry*)((u8*)data + 16) : 0;
    return 1;
}
//@@end
// TODO (not drafted yet, 0x68 bytes at 0x80093A04)
extern "C" void fn_80093A04();
//@@80093A6C
// Record array: construct hook (base call, then construct the new records).
extern "C" void fn_8028BC2C(void* arr, void* p, u32 bytes);
extern "C" void fn_8028BC28(void* arr, void* p, u32 bytes);
extern "C" void fn_80093B3C(AnimItem* p, AnimItem* end);
extern "C" void fn_80093BCC(AnimItem* p, AnimItem* end);
extern "C" void fn_80093A6C(void* arr, AnimItem* p, u32 bytes);
void fn_80093A6C(void* arr, AnimItem* p, u32 bytes) {
    fn_8028BC2C(arr, p, bytes);
    fn_80093B3C(p, p + bytes / sizeof(AnimItem));
}
//@@end
//@@80093AC4
static inline void AnimItemsDestroy(void* p, u32 bytes) { AnimItem* first = (AnimItem*)p; fn_80093BCC(first, first + bytes / sizeof(AnimItem)); }
extern "C" void fn_80093AC4(void* arr, AnimItem* p, u32 bytes);
void fn_80093AC4(void* arr, AnimItem* p, u32 bytes) {
    AnimItemsDestroy(p, bytes);
    fn_8028BC28(arr, p, bytes);
}
//@@end

// Size of one 0x5C-byte record.
int AnimRecordSize() {
    return 0x5C;
}

//@@80093B3C
// Constructs the records in [p, end).
extern "C" void fn_80093B3C(AnimItem* p, AnimItem* end);
void fn_80093B3C(AnimItem* p, AnimItem* end) {
    for (; p < end; p++) {
        new (p) AnimItem;
    }
}

// Destroys the records in [p, end) (nothing to do).
extern "C" void fn_80093BCC(AnimItem* p, AnimItem* end);
void fn_80093BCC(AnimItem* p, AnimItem* end) {
    for (; p < end; p++) {
        p->~AnimItem();
    }
}
//@@end


// u_8029B070.cpp (0x8029B070-0x8029C0D0), split from the old u_80294548 range: a manager over .sbss
// 0x804D5718 with two Buffer-derived array classes (vtables 0x803C3148 and 0x803C3168).
// Boundary evidence:
// - .sdata2: own pool 0x804DA760-0x804DA778 (0.0f at 0x804DA760 duplicates the previous file's 0x804DA730).
// - .data 0x803C3148-0x803C3188 (the two vtables) and .sbss 0x804D5718 are referenced only from this range;
//   the next unit's data starts at 0x803C3188 / 0x804DA778.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

void* VAlloc(int size);
void VFree(void* p);
extern "C" void fn_8028B960(void* buffer);
extern "C" void fn_8028B5FC(void* self, int size, int a);

inline void* operator new(unsigned long, void* p) { return p; }

// The Buffer class chain of u_8028A8D4.cpp (ScreenEffectSlot -> ScreenEffectBuffer -> Buffer; base names are
// guesses there). The arrays below override the element hooks and call the base ones directly.
class ScreenEffectSlot {
public:
    virtual ~ScreenEffectSlot() {}
    virtual void Clear();
    virtual int GetAlignment();
    virtual void Commit(int start, unsigned long size) = 0;

    char* mStart;   // 0x04
    int mSize;      // 0x08
    char* mStore;   // 0x0C
    u32 mCapacity;  // 0x10
};

class ScreenEffectBuffer : public ScreenEffectSlot {
public:
    virtual void Commit(int start, unsigned long size);
    virtual void Append(unsigned char* data, unsigned long size);
    virtual int GetSpan() = 0;
};

class Buffer : public ScreenEffectBuffer {
public:
    virtual ~Buffer();
    Buffer();
    virtual void Clear();
    virtual int GetSpan() { return 0; }
};

// 12-byte element. Its destroy hook ClearEntryRange (0x8029C0AC-0x8029C0D0, the last function here; it was the
// first function of the old u_8029C0AC range) is called only from EntryList::Append and pairs with the construct
// hook fn_8029C00C.
struct Entry12 {
    int a;
    int b;
    float c;
};
void ClearEntryRange(Entry12* begin, Entry12* end);
extern "C" void fn_8029C00C(Entry12* first, Entry12* last);

// An array of Entry12 (vtable 0x803C3168). Class name is a guess.
class EntryList : public Buffer {
public:
    EntryList() {}
    EntryList(const EntryList& src);
    virtual ~EntryList() { fn_8028B960(this); }
    virtual int GetAlignment();
    virtual void Commit(int start, unsigned long size);
    virtual void Append(unsigned char* data, unsigned long size);
    static void operator delete(void* p) { VFree(p); }
};

// 0x2C-byte record holding an EntryList at +0x08.
struct Slot {
    u32 key;          // 0x00
    int unk04;        // 0x04
    EntryList list;   // 0x08
    float unk1C;      // 0x1C
    u8 pad20[0xC];
};
extern "C" void fn_8029BF98(Slot* first, Slot* last);
extern "C" void fn_8029C030(Slot* first, Slot* last);

// An array of Slot (vtable 0x803C3148). Class name is a guess.
class SlotArray : public Buffer {
public:
    SlotArray() {}
    virtual ~SlotArray() { fn_8028B960(this); }
    virtual int GetAlignment();
    virtual void Commit(int start, unsigned long size);
    virtual void Append(unsigned char* data, unsigned long size);
    static void operator delete(void* p) { VFree(p); }
};

// TODO (not drafted yet, 0xD0 bytes at 0x8029B070)
// fn_8029B070
// TODO (not drafted yet, 0xD8 bytes at 0x8029B140)
// fn_8029B140
// TODO (not drafted yet, 0x198 bytes at 0x8029B218)
// fn_8029B218
// TODO (not drafted yet, 0x178 bytes at 0x8029B3B0)
// fn_8029B3B0
//@@8029B528
extern SlotArray* lbl_804D5718;

extern "C" void fn_8029B528(u32 key, float value);
void fn_8029B528(u32 key, float value) {
    Slot* end = (Slot*)(lbl_804D5718->mStore + lbl_804D5718->mSize);
    Slot* p = (Slot*)lbl_804D5718->mStore;

    for (; p != end; p++) {
        if (key == p->key) {
            p->unk1C = value;
            return;
        }
    }
}
//@@end
// TODO (not drafted yet, 0x270 bytes at 0x8029B560)
// fn_8029B560
// TODO (not drafted yet, 0x8C bytes at 0x8029B7D0)
// fn_8029B7D0
// TODO (not drafted yet, 0xC4 bytes at 0x8029B85C)
// fn_8029B85C
// TODO (not drafted yet, 0x31C bytes at 0x8029B920)
// fn_8029B920
//@@8029BC3C
extern "C" char* fn_8029BC3C(Buffer* b);
char* fn_8029BC3C(Buffer* b) {
    return b->mStart + b->mSize;
}
//@@end
// TODO (not drafted yet, 0x68 bytes at 0x8029BC4C)
// fn_8029BC4C
// TODO (not drafted yet, 0x68 bytes at 0x8029BCB4)
// fn_8029BCB4
//@@8029BD1C
void SlotArray::Commit(int start, unsigned long size) {
    ScreenEffectBuffer::Commit(start, size);
    fn_8029BF98((Slot*)start, (Slot*)start + size / sizeof(Slot));
}
//@@end
//@@8029BD74
static inline void SlotsDestroy(void* p, u32 bytes) {
    Slot* first = (Slot*)p;
    fn_8029C030(first, first + bytes / sizeof(Slot));
}

void SlotArray::Append(unsigned char* data, unsigned long size) {
    SlotsDestroy(data, size);
    ScreenEffectBuffer::Append(data, size);
}
//@@end
//@@8029BDE4
int SlotArray::GetAlignment() {
    return sizeof(Slot);
}
//@@end
//@@8029BDEC
void EntryList::Commit(int start, unsigned long size) {
    ScreenEffectBuffer::Commit(start, size);
    fn_8029C00C((Entry12*)start, (Entry12*)start + size / sizeof(Entry12));
}
//@@end
//@@8029BE44
static inline void EntriesDestroy(void* p, u32 bytes) {
    Entry12* first = (Entry12*)p;
    ClearEntryRange(first, first + bytes / sizeof(Entry12));
}

void EntryList::Append(unsigned char* data, unsigned long size) {
    EntriesDestroy(data, size);
    ScreenEffectBuffer::Append(data, size);
}
//@@end
//@@8029BEB4
int EntryList::GetAlignment() {
    return sizeof(Entry12);
}
//@@end
//@@8029BEBC
static inline void EntriesCopy(Entry12* dst, Entry12* first, Entry12* last) {
    for (; first != last; first++, dst++) {
        *dst = *first;
    }
}

EntryList::EntryList(const EntryList& src) {
    u32 bytes = src.mSize / sizeof(Entry12) * sizeof(Entry12);

    if (mCapacity < bytes + GetSpan()) {
        fn_8028B5FC(this, bytes, 1);
    }
    mSize = bytes;
    EntriesCopy((Entry12*)mStore, (Entry12*)src.mStart, (Entry12*)(src.mStart + src.mSize));
}
//@@end
//@@8029BF98
void fn_8029BF98(Slot* first, Slot* last) {
    for (; first < last; first++) {
        new (first) Slot;
    }
}
//@@end
//@@8029C00C
void fn_8029C00C(Entry12* first, Entry12* last) {
    for (; first < last; first++) {
    }
}
//@@end
//@@8029C030
void fn_8029C030(Slot* first, Slot* last) {
    for (; first < last; first++) {
        first->~Slot();
    }
}
//@@end
//@@8029C0AC
void ClearEntryRange(Entry12* begin, Entry12* end) {
    for (; begin < end; begin++) {
    }
}
//@@end

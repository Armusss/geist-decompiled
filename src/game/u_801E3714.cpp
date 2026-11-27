// u_801E3714.cpp (0x801E3714-0x801E47D0), split from the old u_801E241C range: a particle effect built on two
// Buffer-derived arrays (vtables 0x803B3AC0 / 0x803B3AE0) with its sbss state 0x804D5230-0x804D5240.
// Boundary evidence: own .sdata2 pool 0x804D8E00-0x804D8E60 (0.0f 0x804D8E34, double 0x804D8E28) between the Plant
// file's and the credits file's (0.0f 0x804D8E60); .rodata 0x80348F08-0x80348F18; .sdata 0x804D32A0-0x804D32A8.
// The four helpers fn_801E45C4..fn_801E4738 (called only from FearActor.cpp) use no data, so their file is not
// proven; they are kept here, the cut at 0x801E47D0 is the first function that touches the credits data.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

void* VAlloc(int size);
void VFree(void* p);
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
    char* Erase(char* pos, int size);  // 0x8028B35C
    char* Insert(char* pos, int size); // 0x8028B3BC
    void Free();                       // 0x8028B960
};

extern "C" void fn_801E4584(u32* first, u32* last);
extern "C" void fn_801E45A4(u32* first, u32* last);

// An array of 4-byte words (vtable 0x803B3AC0). Class name is a guess.
class WordArray : public Buffer {
public:
    WordArray() {}
    virtual ~WordArray() { Free(); }
    virtual int GetAlignment();
    virtual void Commit(int start, unsigned long size);
    virtual void Append(unsigned char* data, unsigned long size);
    static void operator delete(void* p) { VFree(p); }
    void Resize(u32 bytes) {
        if (mCapacity < bytes + GetSpan()) {
            fn_8028B5FC(this, bytes, 1);
        }
        mSize = bytes;
    }
};

// The same array with its own vtable (0x803B3AE0, only the destructor differs). Class name is a guess.
class WordList : public WordArray {
public:
    WordList() {}
    virtual ~WordList() {}
};

//@@801E3714
extern int lbl_804D32A4;

extern "C" void fn_801E3714(float t);
void fn_801E3714(float t) {
    lbl_804D32A4 = (int)(255.0f * t);
}
//@@end
// TODO (not drafted yet, 0x350 bytes at 0x801E3738)
// fn_801E3738
//@@801E3A88
int AssetFind(u32 table, u32 key);
extern "C" void fn_8000F874(int tex);
extern "C" int fn_8000FECC(int tex);
extern int lbl_804D32A0;
extern int lbl_804D5234;

extern "C" void fn_801E3A88(void);
void fn_801E3A88(void) {
    lbl_804D32A0 = AssetFind(0xF2E2BEB8, 0x748B5005);
    if (lbl_804D32A0 != -1) {
        fn_8000F874(lbl_804D32A0);
        lbl_804D5234 = fn_8000FECC(lbl_804D32A0);
    }
}
//@@end
//@@801E3AD4
extern WordList* lbl_804D5230;
extern int lbl_804D523C;

extern "C" void fn_801E3AD4(void);
void fn_801E3AD4(void) {
    while (lbl_804D5230->mSize / sizeof(u32) != 0) {
        VFree(*(void**)lbl_804D5230->mStore);
        ((Buffer*)lbl_804D5230)->Erase((char*)lbl_804D5230->mStore, sizeof(u32));
    }
    lbl_804D523C = 0;
    delete lbl_804D5230;
}
//@@end
// TODO (not drafted yet, 0x78 bytes at 0x801E3B44)
// fn_801E3B44
// TODO (not drafted yet, 0x258 bytes at 0x801E3BBC)
// fn_801E3BBC
//@@801E3E14
extern "C" void fn_801E3E14(void);
void fn_801E3E14(void) {
    lbl_804D5230 = new WordList;
    lbl_804D5230->Resize(0);
    lbl_804D523C = 0;
}
//@@end
// TODO (not drafted yet, 0xB8 bytes at 0x801E3EA4)
// fn_801E3EA4
// TODO (not drafted yet, 0x510 bytes at 0x801E3F5C)
// fn_801E3F5C
// TODO (not drafted yet, 0x68 bytes at 0x801E446C)
// fn_801E446C
//@@801E44D4
void WordArray::Commit(int start, unsigned long size) {
    ScreenEffectBuffer::Commit(start, size);
    fn_801E4584((u32*)start, (u32*)start + size / sizeof(u32));
}
//@@end
//@@801E451C
static inline void WordsDestroy(void* p, u32 bytes) {
    u32* first = (u32*)p;
    fn_801E45A4(first, first + bytes / sizeof(u32));
}

void WordArray::Append(unsigned char* data, unsigned long size) {
    WordsDestroy(data, size);
    ScreenEffectBuffer::Append(data, size);
}
//@@end
//@@801E457C
int WordArray::GetAlignment() {
    return sizeof(u32);
}
//@@end
//@@801E4584
void fn_801E4584(u32* first, u32* last) {
    for (; first < last; first++) {
    }
}
//@@end
//@@801E45A4
void fn_801E45A4(u32* first, u32* last) {
    for (; first < last; first++) {
    }
}
//@@end
// TODO (not drafted yet, 0x44 bytes at 0x801E45C4)
// fn_801E45C4
//@@801E4608
// The fear object's two banks of 32 slot handles at +0x82C (FearActor.cpp calls these).
struct FearSlots {
    u8 pad00[0x82C];
    int slots[2][32];  // 0x82C
};

extern "C" void fn_801E4608(FearSlots* f);
void fn_801E4608(FearSlots* f) {
    int i;
    int j;
    for (j = 0; j < 32; j++) {
        for (i = 0; i < 2; i++) {
            f->slots[i][j] = -1;
        }
    }
}
//@@end
// TODO (not drafted yet, 0x98 bytes at 0x801E46A0)
// fn_801E46A0
// TODO (not drafted yet, 0x98 bytes at 0x801E4738)
// fn_801E4738

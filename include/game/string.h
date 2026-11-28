#ifndef GAME_STRING_H
#define GAME_STRING_H

// The engine's growable byte store and its text class: ScreenEffectSlot -> ScreenEffectBuffer -> Buffer -> String
// (implemented in u_8028A8D4.cpp, 0x8028A8D4-0x8028C54C). Class, member and slot names are descriptive guesses;
// the two base class names come from the screen-effect code that was decompiled first.
//
// Evidence:
//   sizeof(Buffer) == sizeof(String) == 0x14: stack slots of String locals are 0x14 apart in every Matching
//   message handler (e.g. names at sp+12 / sp+32 / sp+52 in HostBehMsg).
//   The vptr is at +0x00: __ct__6BufferFv (0x8028BA9C) calls the ScreenEffectBuffer constructor (0x8028BD4C),
//   stores its vtable at +0x00 and clears +0x10; __ct__6StringFv (0x8028C500) calls it and stores __vt__6String.
//   Vtables (8-byte header, then): ScreenEffectSlot 0x803C29F0 {dtor, Clear 0x8028B1D4, GetAlignment 0x8028B1CC,
//   Commit = 0}; ScreenEffectBuffer 0x803C2A28 {dtor, Clear 0x8028BD18, GetAlignment, Commit 0x8028BC2C,
//   Append 0x8028BC28, GetSpan = 0}; Buffer 0x803C2A08 {~Buffer 0x8028B9C0, Clear 0x8028B2A0, GetAlignment,
//   Commit, Append, GetSpan 0x8007AD60}; __vt__6String 0x80393D7C {dtor, Buffer::Clear, GetAlignment, Commit,
//   Append, 0x80023884 (`return 1`: room for the terminator)}.
//   Slot meanings, from the overrides in the element arrays (include/game/bufarray.h): GetAlignment is the
//   element size, Commit runs over newly reserved store, Append over store about to be released, GetSpan is the
//   extra bytes every reserve keeps.
//   Buffer::GetSpan (`return 0`) is an inline virtual: it is referenced only from the Buffer-derived vtables and
//   its one surviving copy sits in u_800776FC.cpp, the lowest-addressed file that emits such a vtable (the
//   linker keeps the first weak copy).
//   ~Buffer is out of line; ~String is inline (clients set the String vtable, null-check, then call
//   ~Buffer(this, 0)). String's GetSpan is declared out of line here so that client files never emit
//   __vt__6String (it lives in another file's .data); retail's copy is the `return 1` at 0x80023884.
//   The objects are allocated through VAlloc and released through VFree: every deleting destructor of the
//   chain ends in VFree.

void* VAlloc(int size);
void VFree(void* p);

class Buffer;
extern "C" void fn_8028B5FC(Buffer* self, int size, unsigned char exact); // 0x8028B5FC: reserve (exact: no rounding)

class ScreenEffectSlot {
public:
    ScreenEffectSlot();                                       // 0x8028B23C
    virtual ~ScreenEffectSlot() {}
    virtual void Clear();                                     // 0x0C
    virtual int GetAlignment();                               // 0x10: element size
    virtual void Commit(int start, unsigned long size) = 0;   // 0x14: newly reserved store
    void Set(int start, int size);                            // 0x8028B1E4: Clear, then use outside text
    static void* operator new(unsigned long size) { return VAlloc(size); }
    static void operator delete(void* p) { VFree(p); }

    char* mData;                               // 0x04 outside text the store is built from
    int mLength;                               // 0x08 length in bytes
    char* mUnk0C;                              // 0x0C store
    unsigned long mOwned;                      // 0x10 store capacity (0 while the outside text is used; compared unsigned)
};

class ScreenEffectBuffer : public ScreenEffectSlot {
public:
    ScreenEffectBuffer();                                     // 0x8028BD4C
    virtual ~ScreenEffectBuffer() {}
    virtual void Clear();
    virtual void Commit(int start, unsigned long size);
    virtual void Append(unsigned char* data, unsigned long size); // 0x18: store about to be released
    virtual int GetSpan() = 0;                                    // 0x1C: extra bytes every reserve keeps
};

class Buffer : public ScreenEffectBuffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Clear();                      // 0x8028B2A0
    virtual int GetSpan() { return 0; }        // 0x8007AD60

    char* Erase(char* pos, int size);          // 0x8028B35C: remove `size` bytes at pos (an address in the store)
    char* Insert(char* pos, int size);         // 0x8028B3BC: open `size` bytes at pos, growing the store
    void Free();                               // 0x8028B960: release the store
    char* TextEnd() const;                     // 0x8029BC3C (u_8029B070.cpp): end of the outside text
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    String(const String& src);                 // 0x8028C46C
    int Equals(const char* name);              // 0x8028C110
    ~String() {}
    virtual int GetSpan();
};

#endif

#ifndef GAME_STRING_H
#define GAME_STRING_H

// Buffer and String, the engine's text classes (implemented in u_8028A8D4.cpp, 0x8028A8D4-0x8028C54C; the BufferOwner methods in u_8028865C.cpp).
// Names are descriptive guesses.
//
// Evidence:
//   sizeof(Buffer) == sizeof(String) == 0x14: stack slots of String locals are 0x14 apart in every Matching
//   message handler (e.g. names at sp+12 / sp+32 / sp+52 in HostBehMsg).
//   The vptr is at +0x00: __ct__6BufferFv (0x8028BA9C) calls a base constructor (0x8028BD4C), stores its
//   vtable at +0x00 and clears +0x10; __ct__6StringFv (0x8028C500) calls it and stores __vt__6String at +0x00.
//   __vt__6String (0x80393D7C, 0x24 bytes) has six slots after the 8-byte header:
//     +0x08 destructor, +0x0C 0x8028B2A0, +0x10 0x8028B1CC, +0x14 0x8028BC2C, +0x18 0x8028BC28, +0x1C 0x80023884.
//   The destructor is slot 0, so the classes are declared with `virtual ~Buffer()` first. Units that only use
//   strings never call the other five slots; they are declared here as one placeholder (`Release`), exactly as
//   the 69 client units declared it. u_8028A8D4.cpp, which implements the class, keeps its own fuller
//   declaration (two base classes, five named virtuals) until its constructors match: see the note there.
//   ~Buffer (__dt__6BufferFv, 0x8028B9C0) is out of line; ~String is inline (clients set the String vtable,
//   null-check, then call ~Buffer(this, 0)).

class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;                               // 0x04
    int mLength;                               // 0x08
    int mUnk0C;                                // 0x0C
    int mOwned;                                // 0x10
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

#endif

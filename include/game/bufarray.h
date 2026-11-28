#ifndef GAME_BUFARRAY_H
#define GAME_BUFARRAY_H

#include "game/string.h"

// BufferArray<T>: a Buffer that stores an array of T (template and member names are descriptive guesses).
//
// Evidence: many files end with the same five functions, each file its own copy at its own address:
//   Commit (calls ScreenEffectBuffer::Commit, then the construct loop over [start, start + size / sizeof(T))),
//   Append (the destroy loop over the same range, then ScreenEffectBuffer::Append), GetAlignment
//   (`return sizeof(T)`), and the two range loops (a counted `bdnz` loop that is empty for pointer elements;
//   u_800776FC's 8-byte elements zero both words behind placement new's null check).
// They fill the file's vtables {dtor, Buffer::Clear, GetAlignment, Commit, Append, Buffer::GetSpan}; the
// destructor releases the store (Buffer::Free) before ~Buffer. One copy per file and per element type, placed
// after all of the file's own functions, is what CodeWarrior's template instantiation produces: identical
// code with different element types gets different names, so the linker keeps every copy, while the
// non-template Buffer::GetSpan keeps only its first one.
// Most files derive a second class from the array whose vtable differs only in the destructor (see
// BufferPtrArray); the objects are created with `new` and released with `delete` (VAlloc / VFree).

inline void* operator new(unsigned long, void* p) { return p; }

template <class T>
void BufferDestroy(T* first, T* last)
{
    for (; first < last; first++) {
        first->~T();
    }
}

template <class T>
void BufferConstruct(T* first, T* last)
{
    for (; first < last; first++) {
        new (first) T;
    }
}

template <class T>
inline void BufferCopy(T* dst, T* first, T* last)
{
    for (; first != last; first++, dst++) {
        *dst = *first;
    }
}

template <class T>
class BufferArray : public Buffer {
public:
    BufferArray() {}
    BufferArray(const BufferArray& src);
    virtual ~BufferArray() { Free(); }
    virtual void Commit(int start, unsigned long size);
    virtual void Append(unsigned char* data, unsigned long size);
    virtual int GetAlignment();

    unsigned long Count() const { return (unsigned long)mLength / sizeof(T); }
    T* Begin() const { return (T*)mUnk0C; }
    T* End() const { return (T*)((char*)mUnk0C + mLength); }
    T& operator[](int i) const { return ((T*)mUnk0C)[i]; }   // u_80185A9C: an inline index temp ranks the regs
};

template <class T>
void BufferArray<T>::Commit(int start, unsigned long size)
{
    ScreenEffectBuffer::Commit(start, size);
    BufferConstruct((T*)start, (T*)start + size / sizeof(T));
}

template <class T>
void BufferArray<T>::Append(unsigned char* data, unsigned long size)
{
    T* first = (T*)data;

    BufferDestroy(first, first + size / sizeof(T));
    ScreenEffectBuffer::Append(data, size);
}

template <class T>
int BufferArray<T>::GetAlignment()
{
    return sizeof(T);
}

// Copy constructor (u_8029B070's Entry12 array, 0x8029BEBC): size the store exactly, then copy the elements
// from the source's text range.
template <class T>
BufferArray<T>::BufferArray(const BufferArray<T>& src)
{
    unsigned long bytes = src.mLength / sizeof(T) * sizeof(T);

    if (mOwned < bytes + GetSpan()) {
        fn_8028B5FC(this, bytes, 1);
    }
    mLength = bytes;
    BufferCopy((T*)mUnk0C, (T*)src.mData, (T*)(src.mData + src.mLength));
}

// BufferPtrArray<T>: an array of T pointers. Every pointer array in the game has this second vtable, which
// differs from BufferArray<T*>'s only in the destructor (u_8015AA24, u_80159AF4, u_80158C3C, u_801E3714,
// u_80221100, u_8028865C); the arrays of values (Vec3Array, AnimRecordArray, KindArray) have one vtable.
template <class T>
class BufferPtrArray : public BufferArray<T*> {
public:
    BufferPtrArray() {}
    virtual ~BufferPtrArray() {}
};

#endif

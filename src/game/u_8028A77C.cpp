// u_8028A77C.cpp: Buffer/String classes, screen-effect slots and buffers, and comm helpers (0x8028A77C-0x8028C54C).
// Names are descriptive guesses; the code is matched against the retail DOL.

// Type and member names are descriptive guesses.

#include <stdarg.h>

typedef unsigned long u32;
typedef unsigned char u8;

void VFree(void* p);

extern "C" void fn_8028B960(void* buffer);
extern "C" void fn_8028BC2C(void* self, void* dst, u32 size);
extern "C" void ScreenEffectMemFill(void* dst, u32 size, u8 fill);
extern u8 lbl_804D37C8;
extern "C" void fn_8028BC5C(void* self, int pos, int size);
extern "C" void ScreenEffectSlotResetPending(void* self);
void DestroyWordRange(u32 start, u32 end);
void ClearWordRange(u32 start, u32 end);

// Buffer: base class with a virtual destructor. CodeWarrior passes the
// delete flag to a virtual destructor as a hidden short argument.
// The two bases only exist to reproduce the destructor chain retail runs
// after the Buffer body (each base destructor restores its own vtable).
class BufferBase2 {
public:
    virtual ~BufferBase2() {}
};

class BufferBase : public BufferBase2 {
public:
    virtual ~BufferBase() {}
};

class Buffer : public BufferBase {
public:
    virtual void Prepare();
    virtual int Query();
    virtual void Fill(int offset, int size);
    virtual void Unk18(int data, u32 state); // 0x18: called with the backing store
    virtual int GetSpan();
    virtual ~Buffer();
    Buffer();
    void Append(u8* data, u32 size);
    int GetAlignment();
    static void operator delete(void* p) { VFree(p); }

    char* mStart; // 0x04: text source the store is built from
    int mSize;  // 0x08
    int unk0C;  // 0x0C
    u32 mState; // 0x10
};

// String: a Buffer whose text source (mStart) and length (mSize) describe an
// outside character array; the store is built from it on demand.
class String : public Buffer {
public:
    String();
    String(const char* text);
    String(const String& src);
    int Equals(const char* name);
};

extern "C" void fn_8028BC28(Buffer* self, u8* data, u32 size);

// Guessed class name: object built on a Buffer that releases itself
// through VFree when its deleting destructor asks for it.
class BufferOwner : public Buffer {
public:
    virtual ~BufferOwner();
    void Commit(int start, u32 size);
    static void operator delete(void* p) { VFree(p); }
};

// fn_8028A77C: deleting destructor of the Buffer-derived object.
BufferOwner::~BufferOwner()
{
    fn_8028B960(this);
}

// fn_8028A7E4: release the pending clear, then take [start, start + size) down.
void BufferOwner::Commit(int start, u32 size)
{
    fn_8028BC2C(this, (void*)start, size);
    DestroyWordRange(start, start + (size & ~3));
}

// fn_8028A82C: append `size` bytes of data to the buffer, then copy them in.
void Buffer::Append(u8* data, u32 size)
{
    ClearWordRange((u32)data, (u32)(data + (size & 0xFFFFFFFC)));
    fn_8028BC28(this, data, size);
}

// fn_8028A88C: the buffer's element alignment is always 4 bytes.
int Buffer::GetAlignment()
{
    return 4;
}

// fn_8028A894: run the empty destructor over a range of 4-byte elements
// (count is derived from the byte range; the loop body optimises away).
void DestroyWordRange(u32 start, u32 end)
{
    u32 count = (end + 3 - start) >> 2;
    if (start >= end) {
        return;
    }
    do {
        count--;
    } while (count != 0);
}

// fn_8028A8B4: same range walk for the byte buffer append path.
void ClearWordRange(u32 start, u32 end)
{
    u32 count = (end + 3 - start) >> 2;
    if (start >= end) {
        return;
    }
    do {
        count--;
    } while (count != 0);
}

extern int lbl_804D37B8;
extern int lbl_804D37BC;
extern "C" void fn_80026660(int channel, int (*callback)(int, int, int), int unk);
int GeistCommMessage(int a, int msg, int p);

// fn_8028A8D4: initialise the two state words to -1 and register the
// message callback for message 0x3C on channel 0. Called from u_80157730.
extern "C" void fn_8028A8D4(void)
{
    lbl_804D37B8 = -1;
    lbl_804D37BC = -1;
    fn_80026660(0x3C, GeistCommMessage, 0);
}

int ScreenEffectMessage(void* p);

// fn_8028A910: message callback; only message 0x5F03 is understood.
int GeistCommMessage(int a, int msg, int p)
{
    switch (msg) {
    case 0x5F03:
        return ScreenEffectMessage((void*)p);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Screen-fade effects.  The module keeps the currently registered effect
// handle, the actor animation handle and the two words that go with it in
// globals; effect messages carry the parameters.
extern int lbl_804D37C0;
extern int lbl_804D37C4;
extern int lbl_804D5700;
extern int gWideScreen;
extern float lbl_804DA568;
extern float lbl_804DA56C;

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void fn_8000706C(int a, int b);
extern "C" void fn_800F47FC(float value);
extern "C" void fn_800F60E4(int a, int b, u32 hash);
int FontRelease(int handle);
int FontLoad(int a, const char* name, int b, int c, int callback, int d);
int fn_8007EF48(int a, int b, const char* name, int c, int d,
                int e, int f, int g, int callback, int h);
void ActorPlayAnim(int anim, u32 a, int b);

extern "C" int ScreenEffectFadeOutFinish(void);
extern "C" int ScreenEffectAnimFinish(void);
extern "C" int ScreenEffectLBoxOffFinish(void);
extern "C" int ScreenEffectFadeFinish(void);
int ScreenEffectWhiteOutFinish();

// Message payload: an effect hash in `msg` plus the actor animation data.
struct ScreenEffectMsg {
    int unk00;
    int msg;
    int unk08;
    int anim;
    int unk10;
    int duration;
};

// Payload of message 0x40D; only the enable flag at +0x4 is written.
struct ScreenEffectNotice {
    int unk00;
    int enable;
    int unk08;
    int unk0C;
};

// Stop the actor animation the effect started, if there is one.
static inline void ScreenEffectReleaseAnim()
{
    if (lbl_804D37C0 != -1) {
        ActorPlayAnim(lbl_804D37C0, lbl_804D5700, lbl_804D37C4);
        lbl_804D37C0 = -1;
    }
}

// Tell the HUD that an effect started (1) or stopped (0).
static inline void ScreenEffectNotify(int enable)
{
    ScreenEffectNotice notice;
    notice.enable = enable;
    SendMsg(0x4E000000, 0x40D, &notice);
}

// Release the old effect and start one with the given name and callback.
static inline void ScreenEffectStart(const char* name, int callback, int duration)
{
    FontRelease(lbl_804D37B8);
    lbl_804D37B8 = fn_8007EF48(-1, 0, name, 0, 0,
                               duration, duration, duration, callback, 0);
}

// ScreenEffectMessage: set up one fade/white/LBox effect from the message
// data; the message id is the hash of the effect name.
int ScreenEffectMessage(void* p)
{
    ScreenEffectMsg* m = (ScreenEffectMsg*)p;
    int duration = 8;
    int i;

    if (m->duration > 0)
        duration = m->duration;

    lbl_804D37C0 = m->anim;
    lbl_804D37C4 = m->unk08;
    lbl_804D5700 = m->unk10;
    fn_8000706C(0xC, 0);

    switch (m->msg) {
    case 0x43706697: // White Out
        ScreenEffectStart("White Out", (int)ScreenEffectAnimFinish, duration);
        ScreenEffectNotify(1);
        return 1;
    case 0x195DA922:
        fn_800F47FC(lbl_804DA56C);
        return 1;
    case (int)0xCAE448F3: // Boost Flash
        FontLoad(0, "Boost Flash", 0, 0, (int)ScreenEffectAnimFinish, 0);
        return 1;
    case (int)0xAF98449C: // White In
        ScreenEffectStart("White In", (int)ScreenEffectFadeFinish, duration);
        return 1;
    case (int)0x9F81801A: // LBox On
        if (gWideScreen == 0) {
            FontRelease(lbl_804D37BC);
            lbl_804D37BC = FontLoad(0, "LBox On", duration, 0,
                                    (int)ScreenEffectAnimFinish, 0);
        } else {
            ScreenEffectReleaseAnim();
        }
        ScreenEffectNotify(1);
        return 1;
    case (int)0xE9BD5149: // Fade In
        ScreenEffectStart("Fade In", (int)ScreenEffectFadeFinish, duration);
        return 1;
    case 0x2A1BCBAC:
        fn_800F47FC(lbl_804DA568);
        return 1;
    case 0x286D828F:
        for (i = 0; i < 3; i++)
            fn_800F60E4(0, i, 0x24A738);
        ScreenEffectReleaseAnim();
        return 1;
    case 0x31471315: // Camera Flash
        FontLoad(0, "Camera Flash", 0, 0, (int)ScreenEffectAnimFinish, 0);
        return 1;
    case 0x508F8274: // LBox Off
        if (gWideScreen == 0) {
            FontRelease(lbl_804D37BC);
            lbl_804D37BC = FontLoad(0, "LBox Off", duration, 0,
                                    (int)ScreenEffectLBoxOffFinish, 0);
        } else {
            ScreenEffectReleaseAnim();
            ScreenEffectNotify(0);
        }
        return 1;
    case 0x4D577A0F:
        for (i = 0; i < 3; i++)
            fn_800F60E4(0, i, 0x61BC8CB7);
        ScreenEffectReleaseAnim();
        return 1;
    case 0x492EE0E7: // Fade Out
        ScreenEffectStart("Fade Out", (int)ScreenEffectFadeOutFinish, duration);
        ScreenEffectNotify(1);
        return 1;
    case 0x4DECEF8A: // Fade Out
        ScreenEffectStart("Fade Out", (int)ScreenEffectAnimFinish, duration);
        ScreenEffectNotify(1);
        return 1;
    case 0x746CC9FA: // White Out
        ScreenEffectStart("White Out", (int)ScreenEffectWhiteOutFinish,
                          duration);
        ScreenEffectNotify(1);
        return 1;
    case 0x6508556B: // White In Blur
        fn_8000706C(4, 0);
        ScreenEffectStart("White In Blur", (int)ScreenEffectFadeFinish, duration);
        return 1;
    case 0x746D3850: // White Out Blur
        ScreenEffectStart("White Out Blur", (int)ScreenEffectAnimFinish, duration);
        ScreenEffectNotify(1);
        return 1;
    }
    return 0;
}

// ScreenEffectWhiteOutFinish: callback of the White Out effect; turns the
// colour mode back on and releases the actor animation.
int ScreenEffectWhiteOutFinish()
{
    fn_8000706C(0xC, 1);
    ScreenEffectReleaseAnim();
    return 1;
}

// fn_8028B080: fade-out completion callback.  Stops the pending stream
// request, then plays the queued actor animation once.
extern "C" int ScreenEffectFadeOutFinish(void)
{
    fn_8000706C(4, 1);
    if (lbl_804D37C0 != -1) {
        ActorPlayAnim(lbl_804D37C0, lbl_804D5700, lbl_804D37C4);
        lbl_804D37C0 = -1;
    }
    return 1;
}

// fn_8028B0CC: plain completion callback.  Plays the queued actor animation
// once and clears the handle; no stream request is touched.
extern "C" int ScreenEffectAnimFinish(void)
{
    if (lbl_804D37C0 != -1) {
        ActorPlayAnim(lbl_804D37C0, lbl_804D5700, lbl_804D37C4);
        lbl_804D37C0 = -1;
    }
    return 1;
}

// 0x8028B10C: LBox Off completion callback.  Releases the actor animation,
// drops the LBox font handle and tells the HUD the effect has stopped.
extern "C" int ScreenEffectLBoxOffFinish(void)
{
    ScreenEffectReleaseAnim();
    lbl_804D37BC = -1;
    ScreenEffectNotify(0);
    return 1;
}

// 0x8028B16C: fade-in / white-in completion callback.  Releases the actor
// animation, drops the effect font handle and notifies the HUD.
extern "C" int ScreenEffectFadeFinish(void)
{
    ScreenEffectReleaseAnim();
    lbl_804D37B8 = -1;
    ScreenEffectNotify(0);
    return 1;
}

// 0x8028B1CC: completion callback that reports done immediately; there is no
// state to release for this effect.
extern "C" int ScreenEffectQuickFinish(void)
{
    return 1;
}

// 0x8028B1D4: clear the two queued-effect words of a screen-effect state.
extern "C" void ScreenEffectResetState(void* self)
{
    int* state = (int*)self;
    state[1] = 0;
    state[2] = 0;
}

// A small vtable-bearing slot that remembers a queued range.  The virtual
// function at vtable offset 0xC is called before the range is stored.
// Member and function names are descriptive guesses.
class ScreenEffectSlot {
public:
    ScreenEffectSlot();
    virtual ~ScreenEffectSlot();
    virtual void Prepare();
    virtual int Query();
    virtual void Fill(int offset, int size); // 0x14: clear the newly added store
    virtual void Unk18(int data, u32 state); // 0x18: not identified yet
    virtual int GetSpan();                   // 0x1C: bytes the next reserve needs
    void Set(int start, int size);
    void Clear();
    static void operator delete(void* p) { VFree(p); }

    int mStart; // 0x04
    int mSize;  // 0x08
    int unk0C;  // 0x0C
    u32 mState; // 0x10
};

// fn_8028B23C: constructor; installs the vtable and clears the range.
ScreenEffectSlot::ScreenEffectSlot()
{
    mStart = 0;
    mSize = 0;
}

// fn_8028B1E4: let the slot prepare itself, then store the new range.
void ScreenEffectSlot::Set(int start, int size)
{
    Prepare();
    mStart = start;
    mSize = size;
}

// fn_8028B258: deleting destructor of the slot.  It restores the class vtable
// and, when the hidden delete flag is positive, releases the slot through
// operator delete (VFree); the deleting destructor returns the object pointer.
ScreenEffectSlot::~ScreenEffectSlot()
{
}

// fn_8028B2A0: finish any pending slot work, then drop the queued handle in
// the state word at +0x10.
void ScreenEffectSlot::Clear()
{
    ScreenEffectSlotResetPending(this);
    mState = 0;
}

// 0x8028B2D4: write the range reported by the slot's virtual query into the
// pending window and shrink the window by the reported size.  Retail evaluates
// the query twice in a row (first result is the size, second the position).
// ScreenEffectSlotFlush
extern "C" void fn_8028B2D4(ScreenEffectSlot* slot)
{
    int size = slot->Query();
    int pos = slot->Query();
    fn_8028BC5C(slot, (slot->unk0C + slot->mSize) - pos, size);
    slot->mSize -= size;
}

// 0x8028B35C: mark [pos, pos + size) of the current window as written.  The
// window base is re-read from the object after the write call, as retail does.
// ScreenEffectSlotConsume
extern "C" int fn_8028B35C(ScreenEffectSlot* slot, int pos, int size)
{
    int rel = pos - slot->unk0C;
    fn_8028BC5C(slot, slot->unk0C + rel, size);
    slot->mSize -= size;
    return slot->unk0C + rel;
}

// 0x8009D974: grow (or, with a null base, allocate) a heap block.
void* VRealloc(void* block, int size);

u8* CopyBytes(const u8* src, u32 size, u8* dst);
extern "C" void fn_8028BC94(void* self, int pos, int size);
extern "C" void ScreenEffectSlotQueueCopy(void* self, int base, int offset, int size);

// Round `value` up to the next multiple of `align`.  The powers of two are
// special-cased; everything else uses a remainder division.
static inline u32 AlignUp(u32 value, u32 align)
{
    u32 rem;

    if (align == 4) {
        if (value & 3)
            value = (value & ~3u) + 4;
    } else if (align == 2) {
        if (value & 1)
            value += 1;
    } else if (align == 8) {
        if (value & 7)
            value = (value & ~7u) + 8;
    } else if (align == 16) {
        if (value & 15)
            value = (value & ~15u) + 16;
    } else {
        rem = value - (value / align) * align;
        if (rem != 0)
            value += align - rem;
    }
    return value;
}

// Reserve `size` bytes at absolute position `offset` in the buffer, growing the
// backing store (and copying the old contents over) when it does not fit, then
// return the address of the reserved window.  Kept under its placeholder name:
// five other units call it, so the lead relinks them later.
extern "C" void* fn_8028B3BC(void* buffer, int offset, int size)
{
    ScreenEffectSlot* self = (ScreenEffectSlot*)buffer;
    int oldBase = self->unk0C;
    int rel = offset - oldBase;
    int newPos = size + self->mSize;
    int span = self->GetSpan();

    if ((u32)self->mState < (u32)(newPos + span)) {
        u32 want = newPos + self->GetSpan();

        if ((u32)self->mState < want) {
            bool fresh = false;
            int oldOffset;
            int newBlock;
            u32 block;
            u32 count;

            if (self->mState == 0 && self->mStart != 0)
                fresh = true;
            oldOffset = fresh ? 0 : self->unk0C;

            block = AlignUp(0x40, (u32)self->Query());
            want = AlignUp(want, block);

            newBlock = (int)VRealloc((void*)oldOffset, (int)want);
            self->Fill(newBlock + self->mState, (int)(want - (u32)self->mState));
            if (oldOffset == 0 && self->mStart != 0) {
                count = want;
                if ((u32)self->mSize + 1 < want)
                    count = (u32)self->mSize + 1;
                CopyBytes((const u8*)self->mStart, count, (u8*)newBlock);
            }
            self->Set(newBlock, self->mSize);
            self->unk0C = newBlock;
            self->mState = want;
        }
    }
    self->mSize = newPos;
    fn_8028BC94(self, self->unk0C + rel, size);
    return (void*)(self->unk0C + rel);
}

// Reserve `size` bytes past the current position without a caller-supplied
// offset.  A non-zero `raw` skips the block rounding and is used for the
// unaligned writes of the stream code.  Also kept under its placeholder name.
extern "C" void fn_8028B5FC(void* buffer, int size, u8 raw)
{
    ScreenEffectSlot* self = (ScreenEffectSlot*)buffer;
    int newPos = size + self->GetSpan();

    if ((u32)self->mState < (u32)newPos) {
        bool fresh = false;
        int oldOffset;
        int newBlock;
        u32 want;
        u32 count;

        if (self->mState == 0 && self->mStart != 0)
            fresh = true;
        oldOffset = fresh ? 0 : self->unk0C;

        want = (u32)newPos;
        if (raw == 0) {
            u32 block = AlignUp(0x40, (u32)self->Query());
            want = AlignUp(want, block);
        }

        newBlock = (int)VRealloc((void*)oldOffset, (int)want);
        self->Fill(newBlock + self->mState, (int)(want - (u32)self->mState));
        if (oldOffset == 0 && self->mStart != 0) {
            count = want;
            if ((u32)self->mSize + 1 < want)
                count = (u32)self->mSize + 1;
            CopyBytes((const u8*)self->mStart, count, (u8*)newBlock);
        }
        self->Set(newBlock, self->mSize);
        self->unk0C = newBlock;
        self->mState = want;
    }
}

// 0x8028B828: make room for `size` bytes plus the slot's span, then store the
// new range and hand it to the helper at 0x8028BCCC.  The parameter names and
// the helper's meaning are guesses.
void ScreenEffectSlotReserveAndCopy(ScreenEffectSlot* self, int offset, int size)
{
    int newPos = size + self->GetSpan();

    if ((u32)self->mState < (u32)newPos) {
        u32 want = (u32)size + self->GetSpan();

        if ((u32)self->mState < want) {
            bool fresh = false;
            int oldOffset;
            int newBlock;

            if (self->mState == 0 && self->mStart != 0)
                fresh = true;
            oldOffset = fresh ? 0 : self->unk0C;

            newBlock = (int)VRealloc((void*)oldOffset, (int)want);
            self->Fill(newBlock + self->mState, (int)(want - (u32)self->mState));
            if (oldOffset == 0 && self->mStart != 0) {
                u32 count = want;
                if ((u32)self->mSize + 1 < want)
                    count = (u32)self->mSize + 1;
                CopyBytes((const u8*)self->mStart, count, (u8*)newBlock);
            }
            self->Set(newBlock, self->mSize);
            self->unk0C = newBlock;
            self->mState = (int)want;
        }
    }
    self->mSize = size;
    ScreenEffectSlotQueueCopy(self, self->unk0C, offset, size);
}

// 0x8028B960: release the backing store through VFree when the slot holds
// one, then clear the pending state.  Other units call this helper under its
// placeholder name, so the C symbol stays.
extern "C" void fn_8028B960(void* buffer)
{
    ScreenEffectSlot* self = (ScreenEffectSlot*)buffer;

    if (self->mState != 0) {
        self->Unk18(self->unk0C, self->mState);
        VFree((void*)self->unk0C);
    }
    ScreenEffectSlotResetPending(self);
    self->mState = 0;
}

// __dt__6BufferFv: deleting destructor of Buffer.  The body releases the
// backing store (unless the buffer is still owned by a caller-supplied
// range), tears the buffer down, then runs the two empty base destructors.
// A positive delete flag releases the object itself through VFree.
Buffer::~Buffer()
{
    bool skip = false;

    if (mState == 0 && mStart != 0)
        skip = true;

    if (!skip) {
        if (mState != 0) {
            Unk18(unk0C, mState);
            VFree((void*)unk0C);
        }
        ScreenEffectSlotResetPending(this);
        mState = 0;
    }
}

// __ct__6BufferFv: constructor; runs the base constructor, installs the
// Buffer vtable (implicit) and marks the buffer as holding no store yet.
Buffer::Buffer()
{
    mState = 0;
}

// 0x8028BAE0: deleting destructor of BufferBase.  Retail shows the shape
// CodeWarrior generates for a class whose virtual destructor is defined
// in-class: store BufferBase's vtable, run the inlined BufferBase2
// destructor body (which stores that base's vtable), then free the object
// when the hidden delete flag is positive.  Nothing to write by hand; the
// out-of-line copy (__dt__10BufferBaseFv) comes from the class definition.
// The vtable labels confirm the hierarchy (Buffer -> BufferBase -> BufferBase2).

// 0x8028BB3C: copy `size` bytes from `src` to `dst`, returning the first
// byte past the copied range.  The parameter order (source, size,
// destination) and the alignment / word / block loops are as retail shows;
// the name is a guess.
u8* CopyBytes(const u8* src, u32 size, u8* dst)
{
    if (size >> 2 > 2) {
        while (size != 0 && ((((u32)src & 3) != 0) || (((u32)dst & 3) != 0))) {
            *dst = *src;
            dst++;
            src++;
            size--;
        }
        u32 words = size >> 2;
        if (words != 0) {
            do {
                *(u32*)dst = *(const u32*)src;
                dst += 4;
                src += 4;
            } while (--words != 0);
            size &= 3;
        }
    }
    if (size != 0) {
        u32 blocks = size >> 3;
        if (blocks != 0) {
            do {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];
                dst[4] = src[4];
                dst[5] = src[5];
                dst[6] = src[6];
                dst[7] = src[7];
                dst += 8;
                src += 8;
            } while (--blocks != 0);
            size &= 7;
            if (size == 0)
                return dst;
        }
        do {
            *dst = *src;
            src++;
            dst++;
        } while (--size != 0);
    }
    return dst;
}

// fn_8028BC28: no-op hook; retail is a bare `blr`.
extern "C" void fn_8028BC28(Buffer* self, u8* data, u32 size) {}

// fn_8028BC2C: forward (dst, size) to the shared fill routine together with
// the table's fill byte. The first argument is unused.
extern "C" void fn_8028BC2C(void* self, void* dst, u32 size)
{
    ScreenEffectMemFill(dst, size, lbl_804D37C8);
}

void ScreenEffectRotateBytes(int start, int middle, int end);

// fn_8028BC5C: mark [pos, pos + size) of the window that ends at the slot's
// current write position (unk0C + mSize).
extern "C" void fn_8028BC5C(void* self, int pos, int size)
{
    ScreenEffectSlot* s = (ScreenEffectSlot*)self;

    ScreenEffectRotateBytes(pos, pos + size, s->unk0C + s->mSize);
}

// fn_8028BC94: mark the last `size` bytes before the slot's current write
// position.
extern "C" void fn_8028BC94(void* self, int pos, int size)
{
    ScreenEffectSlot* s = (ScreenEffectSlot*)self;
    int end = s->unk0C + s->mSize;

    ScreenEffectRotateBytes(pos, end - size, end);
}

// 0x8028BCCC: queue a copy of `size` bytes from `base` into the slot, unless
// the request is empty or already points at the slot's own store.
extern "C" void ScreenEffectSlotQueueCopy(void* self, int base, int offset, int size)
{
    if (offset != 0 && size != 0 && offset != *(int*)((char*)self + 0xC))
        CopyBytes((const u8*)offset, (u32)size, (u8*)base);
}

// 0x8028BD18: clear the slot's queued-effect state and forget the pending
// buffer pointer in the word at +0x10.
extern "C" void ScreenEffectSlotResetPending(void* self)
{
    ScreenEffectResetState(self);
    *(int*)((char*)self + 0xC) = 0;
}

// fn_8028BD4C: constructor of the slot variant that owns the pixel store.
// It runs the ScreenEffectSlot constructor, installs its own vtable and
// starts with no queued effect handle.  The overrides listed here are the
// descriptive guesses for the routines that follow this constructor.
class ScreenEffectBuffer : public ScreenEffectSlot {
public:
    ScreenEffectBuffer();
    virtual void Prepare();
    virtual int Query();
    virtual void Fill(int offset, int size);
    virtual void Unk18(int data, u32 state);
    virtual int GetSpan();
    virtual ~ScreenEffectBuffer();
};

// fn_8028BD4C: __ct__18ScreenEffectBufferFv
ScreenEffectBuffer::ScreenEffectBuffer()
{
    unk0C = 0;
}

// The fill routine replicates the fill byte through a word using the two
// shift amounts the runtime keeps in these words, masks the destination to
// 4-byte alignment first, then writes 4-byte words and byte runs.
extern u32 lbl_804D37CC;
extern u32 lbl_804D37D0;
extern u32 lbl_804D37D4;
extern u32 lbl_804D37D8;

// fn_8028BD90: fill `size` bytes at `dst` with `fill`.
extern "C" void ScreenEffectMemFill(void* dst, u32 size, u8 fill)
{
    u8* p = (u8*)dst;
    u32 shift;
    u32 word;
    u32 words;
    u32 blocks;

    if (size >> 2 > 2) {
        while (((u32)p & 3) != 0) {
            *p = fill;
            p++;
            size--;
        }
        shift = lbl_804D37D0;
        if (lbl_804D37CC < lbl_804D37D0)
            shift = lbl_804D37CC;
        word = fill | ((u32)fill << shift);
        shift = lbl_804D37D8;
        if (lbl_804D37D4 < lbl_804D37D8)
            shift = lbl_804D37D4;
        word |= word << shift;
        words = size >> 2;
        do {
            *(u32*)p = word;
            p += 4;
        } while (--words != 0);
        size &= 3;
    }
    if (size == 0)
        return;
    blocks = size >> 3;
    if (blocks != 0) {
        do {
            p[0] = fill;
            p[1] = fill;
            p[2] = fill;
            p[3] = fill;
            p[4] = fill;
            p[5] = fill;
            p[6] = fill;
            p[7] = fill;
            p += 8;
        } while (--blocks != 0);
        size &= 7;
        if (size == 0)
            return;
    }
    do {
        *p = fill;
        p++;
    } while (--size != 0);
}

// Reverse the byte range [first, last) in place.  Each pass swaps the outer
// bytes and stops as soon as the two ends meet.
static inline void ScreenEffectReverseBytes(u8* first, u8* last)
{
    while (first != last) {
        u8 tmp;

        --last;
        if (first == last)
            return;
        tmp = *first;
        *first = *last;
        *last = tmp;
        ++first;
    }
}

// Rotate the byte range [start, end) so that the part from `middle` on comes
// first: reverse both halves, then reverse the whole range.  The last pass is
// written so that it stops as soon as one end reaches the old middle and the
// remaining stretch is reversed on its own.  The callers pass byte addresses.
void ScreenEffectRotateBytes(int start, int middle, int end)
{
    u8* first = (u8*)start;
    u8* mid = (u8*)middle;
    u8* last = (u8*)end;

    if (first == mid)
        return;
    if (mid == last)
        return;

    ScreenEffectReverseBytes(first, mid);
    ScreenEffectReverseBytes(mid, last);

    while (first != mid && mid != last) {
        u8 tmp = *first;

        *first = *(last - 1);
        ++first;
        --last;
        *last = tmp;
    }
    if (first != mid)
        ScreenEffectReverseBytes(first, mid);
    else
        ScreenEffectReverseBytes(mid, last);
}

// The formatting routine comes from the C runtime and takes the destination,
// its byte count, the format string and the argument list.
extern "C" int vsnprintf(char* buffer, unsigned long count, const char* format,
                         va_list args);

// fn_8028BF80: format text into the buffer's store and record the length in
// mSize.  The first pass formats into a small local string to learn how long
// the text is; when the store cannot hold the result plus its terminator it is
// grown and the text is formatted again into the real store.  Other units call
// this under its placeholder name, so it keeps C linkage.
extern "C" int fn_8028BF80(Buffer* self, const char* format, ...)
{
    va_list args;
    char empty[4] = "";
    int len;

    va_start(args, format);
    len = vsnprintf(self->mState ? (char*)self->unk0C : empty,
                    self->mState ? (u32)self->mState : 1u, format, args);
    if (len >= 0) {
        if ((u32)len >= (u32)(self->mState ? self->mState : 1)) {
            fn_8028B5FC(self, len, 1);
            va_start(args, format);
            len = vsnprintf((char*)self->unk0C, (u32)self->mState, format,
                            args);
        }
        if ((u32)self->mState < (u32)(len + self->GetSpan()))
            fn_8028B5FC(self, len, 1);
        self->mSize = len;
        ((char*)self->unk0C)[len] = 0;
    }
    return len;
}

// The C runtime helpers the comparison below goes through.
extern "C" u32 strlen(const char* s);
extern "C" int memcmp(const void* a, const void* b, u32 n);
// Shared empty string used when a null name is passed.
extern char lbl_804DA570[];

// 0x8028C110: is the string's text exactly `name`?  A null name stands for
// the shared empty string.
int String::Equals(const char* name)
{
    u32 length;
    int result = 0;

    if (name == 0)
        name = lbl_804DA570;
    length = strlen(name);
    if (length == (u32)mSize && memcmp(mStart, name, (u32)mSize) == 0)
        result = 1;
    return result;
}

// 0x8028C188: append `size` bytes of text (AppendText), ignoring any trailing
// NULs, and keep the store NUL-terminated.  The text is copied so that it ends
// exactly at the buffer's write position.  Kept under its placeholder name:
// fourteen other units call it, so the lead relinks them later.
extern "C" void fn_8028C188(Buffer* self, char* text, int size)
{
    int len = size;
    int pos;

    while (len != 0 && text[len - 1] == 0)
        len--;
    pos = len + self->mSize;
    if ((u32)self->mState < (u32)(pos + self->GetSpan()))
        fn_8028B5FC(self, pos, 1);
    self->mSize = pos;
    ((char*)self->unk0C)[pos] = 0;
    CopyBytes((const u8*)text, (u32)len,
                (u8*)((char*)(self->unk0C + self->mSize) - len));
}

// fn_8028C258: append a text (AppendText), ignoring any trailing NULs, and
// keep the store NUL-terminated.  A null text stands for the shared empty
// string.  Other units call this under its placeholder name, so it keeps C
// linkage.
extern "C" void fn_8028C258(Buffer* self, const char* text)
{
    u32 len;
    int pos;

    if (text == 0)
        text = lbl_804DA570;
    len = strlen(text);
    while (len != 0 && text[len - 1] == 0)
        len--;
    pos = len + self->mSize;
    if ((u32)self->mState < (u32)(pos + self->GetSpan()))
        fn_8028B5FC(self, pos, 1);
    self->mSize = pos;
    ((char*)self->unk0C)[pos] = 0;
    CopyBytes((const u8*)text, len,
                (u8*)((char*)(self->unk0C + self->mSize) - len));
}

// fn_8028C338: replace the buffer's text with a text (SetText), ignoring any
// trailing NULs, and record its length.  A null text stands for the shared
// empty string.  Also kept under its placeholder name.
extern "C" void fn_8028C338(Buffer* self, const char* text)
{
    u32 len;

    if (text == 0)
        text = lbl_804DA570;
    len = strlen(text);
    while (len != 0 && text[len - 1] == 0)
        len--;
    if ((u32)self->mState < (u32)(len + self->GetSpan()))
        fn_8028B5FC(self, len, 1);
    self->mSize = len;
    ((char*)self->unk0C)[len] = 0;
    ScreenEffectSlotQueueCopy(self, self->unk0C, (int)text, (int)len);
}

// 0x8028C400: construct a String over an outside character array.  A null
// name stands for the shared empty string; the text pointer and its length
// become the buffer's source range.
String::String(const char* text)
{
    if (text == 0)
        text = lbl_804DA570;
    ((ScreenEffectSlot*)this)->Set((int)text, (int)strlen(text));
}

// 0x8028C46C: copy constructor.  When the source still points at outside
// text (no store of its own) the new string shares that text; otherwise the
// source is reserved into a fresh store and its bytes are copied over.
String::String(const String& src)
{
    bool share = false;

    if (src.mState == 0 && src.mStart != 0)
        share = true;
    if (share)
        ((ScreenEffectSlot*)this)->Set((int)src.mStart, src.mSize);
    else
        ScreenEffectSlotReserveAndCopy((ScreenEffectSlot*)this,
                                       (int)src.mStart, src.mSize);
}

// 0x8028C500: default constructor.  The string starts out empty and shares the
// module's shared empty text.
String::String()
{
    ((ScreenEffectSlot*)this)->Set((int)lbl_804DA570, 0);
}

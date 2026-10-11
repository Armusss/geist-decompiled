// VMem.cpp: the "virtual memory" block pool used by Memory.cpp (0x801BEC4C-0x801BF578).
// A pool is one big block split into 32-byte-header blocks kept in a doubly linked list. Free blocks are
// merged with free neighbours; Compact() slides movable blocks down (the owner's callback is told the
// new address). When the pool lives in VM (ARAM-backed pages, VM.c), headers and data pages are locked
// while in use.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

extern "C" {
void* memcpy(void* dst, const void* src, u32 n);
int VMIsVirtualAddr(void* p);                 // 0x80286A50 (VM.c)
void VMDeallocPages(void* p, u32 size);       // 0x80286AF0 (VM.c)
void VMAllocPages(void* p, u32 size);         // 0x80286BF8 (VM.c)
}

void LCCopyEnd(void);                         // 0x801D8768
void LCCopy(void* dst, void* src, u32 size);  // 0x801D879C

typedef void (*VMemCallback)(int op, void* data, int arg);

enum { VMEM_FREE = 1, VMEM_USED = 2 };

// One block header (32 bytes) in front of its data.
class VMemList {
public:
    VMemList();
    VMemList(u32 sz) : prev(0), next(0), size(sz), state(0), cb(0), arg(0), data(this + 1) {
        end = (char*)data + size;
    }

    void Unlink() {
        if (prev) prev->next = next;
        if (next) next->prev = prev;
        prev = 0;
        next = 0;
    }
    // Takes over the neighbour's data and header. (The "| 0" reproduces the retail evaluation order.)
    void Absorb(VMemList* o) {
        o->Unlink();
        SetSize(size + (o->size | 0) + 32);
    }
    void Link(VMemList* p, VMemList* n) {
        if (p) p->next = this;
        if (n) n->prev = this;
        prev = p;
        next = n;
    }
    void SetSize(u32 sz) {
        size = sz;
        end = (char*)data + sz;
    }

    VMemList* prev;
    VMemList* next;
    u32 size;
    int state;
    VMemCallback cb;
    int arg;
    void* data;
    void* end;
};

class VMem {
public:
    int FreeBytes();
    int SizeOf(void* p);
    int Contains(int p);
    u32 Compact(u32 maxBytes);
    void Free(void* p);
    void* Alloc(int size, int a, int b);
    void* Base();
    void Init(void* base, int size, int a, int b);

    char* mBase;
    int mSize;
    VMemList mHead;
    VMemList* mRover;
    int mFastCopy;
    int mVirtual;
};

inline void* operator new(u32, void* p) { return p; }

static inline void LockHeader(void* p) {
    if (VMIsVirtualAddr(p)) VMAllocPages(p, 32);
}

static inline void UnlockHeader(void* p) {
    if (VMIsVirtualAddr(p)) VMDeallocPages(p, 32);
}

static inline VMemList* Merge(VMemList* a, VMemList* b) {
    if (a < b) {
        a->Absorb(b);
        UnlockHeader(b);
        return a;
    } else {
        b->Absorb(a);
        UnlockHeader(a);
        return b;
    }
}

int VMem::FreeBytes() {
    int used = 0;
    VMemList* b;

    for (b = mHead.next; b != 0; b = b->next) {
        if (b->state != VMEM_FREE) used += b->size;
    }
    return mSize - used;
}

int VMem::SizeOf(void* p) {
    VMemList* b;

    for (b = mHead.next; b != 0; b = b->next) {
        if (b->data == p) return b->size;
    }
    return 0;
}

int VMem::Contains(int p) {
    if (p < (int)mBase || p >= (int)mBase + mSize) return 0;
    return 1;
}

u32 VMem::Compact(u32 maxBytes) {
    u32 freeSize;
    int fast;
    int isVirtual;
    u32 moved;
    VMemList* f;
    VMemList* b;
    u32 oldSize;
    void* oldData;
    VMemList* nf;

    moved = 0;

    for (f = mRover; f != 0; f = f->next) {
        if (f->state == VMEM_FREE) {
            for (b = f->next; b != 0; b = f->next) {
                if (b->state == VMEM_FREE) {
                    f = Merge(f, b);
                } else {
                    if (moved >= maxBytes) break;
                    isVirtual = mVirtual;
                    fast = mFastCopy;
                    moved += b->size;
                    if (b->cb == 0) {
                        nf = b;
                    } else {
                        b->cb(1, 0, b->arg);
                        freeSize = f->size;
                        b->Unlink();
                        f->SetSize(b->size);
                        f->state = VMEM_USED;
                        f->cb = b->cb;
                        f->arg = b->arg;
                        oldData = b->data;
                        oldSize = b->size;
                        UnlockHeader(b);
                        if (isVirtual) VMAllocPages(f->data, b->size);
                        if (fast) {
                            LCCopy(f->data, b->data, b->size);
                        } else {
                            memcpy(f->data, b->data, b->size);
                        }
                        if (isVirtual) VMDeallocPages(oldData, oldSize);
                        nf = (VMemList*)f->end;
                        LockHeader(nf);
                        new (nf) VMemList(freeSize);
                        nf->state = VMEM_FREE;
                        nf->Link(f, f->next);
                        f->cb(1, f->data, f->arg);
                    }
                    f = nf;
                }
                mRover = f;
            }
            if (mFastCopy) LCCopyEnd();
            return moved;
        }
    }
    return 0;
}

void VMem::Free(void* p) {
    VMemList* b;

    if (mBase != 0 && p != 0) {
    b = (VMemList*)p - 1;
    b->state = VMEM_FREE;
    b->cb = 0;
    b->arg = 0;
    if (mVirtual) VMDeallocPages(b->data, b->size);

    if (b->prev != 0 && b->prev->state == VMEM_FREE) b = Merge(b, b->prev);
    if (b->next != 0 && b->next->state == VMEM_FREE) b = Merge(b, b->next);

    if (mRover == 0 || b < mRover) mRover = b;
    }
}

void* VMem::Alloc(int size, int a, int c) {
    VMemList* b;
    u32 rest;
    VMemList* nb;
    u32 sz;

    if ((u32)size != 0) {
    sz = (size + 31) & ~31;
    for (b = mRover; b != 0; b = b->next) {
        if (b->state == VMEM_FREE) {
            if (b->size >= sz) {
                rest = b->size - sz;
                if (rest >= 64) {
                    b->SetSize(sz);
                    nb = (VMemList*)b->end;
                    LockHeader(nb);
                    new (nb) VMemList(rest - 32);
                    nb->state = b->state;
                    nb->Link(b, b->next);
                }
                b->state = VMEM_USED;
                b->cb = (VMemCallback)a;
                b->arg = c;
                if (mRover->state != VMEM_FREE) mRover = b->next;
                if (mVirtual) VMAllocPages(b->data, b->size);
                return b->data;
            }
            if (mRover->state != VMEM_FREE) mRover = b;
        }
    }
    }
    return 0;
}

void* VMem::Base() {
    char* base = mBase;
    VMemList* b;

    for (b = mHead.next; b != 0; b = b->next) {
    }
    mBase = 0;
    mSize = 0;
    mHead.next = 0;
    mRover = 0;
    return base;
}

void VMem::Init(void* base, int size, int fast, int isVirtual) {
    VMemList* b;

    mFastCopy = fast;
    mVirtual = isVirtual;
    mBase = (char*)base;
    mSize = (size + 31) & ~31;

    b = (VMemList*)mBase;
    LockHeader(b);
    new (b) VMemList(mSize - 32);
    b->state = VMEM_FREE;
    b->Link(&mHead, 0);
    mRover = mHead.next;
}

VMemList::VMemList() : prev(0), next(0), size(0), state(0), cb(0), arg(0), data(0), end(0) {}

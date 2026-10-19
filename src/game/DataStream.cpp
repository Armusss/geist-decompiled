// DataStream.cpp: background streaming of game data from the disc (0x8017BE38-0x8017D794).
// Read requests are kept in two priority queues (each a binary tree sorted by disc offset that is flattened
// into a list before reading) and served one at a time with DVDReadAsyncPrio into one of two VMem pools.
// Pool 1 (animations) reads through a 0x2000-byte bounce buffer in 0x800-byte chunks; freed pool-1 blocks
// are parked in an address-sorted list and released later in small time slices.
// Names are descriptive guesses; the code is matched against the retail DOL.

#pragma auto_inline off

#include <dolphin/os.h>
#include <dolphin/dvd.h>

#include <string.h>

extern "C" void* MemAlloc(int size, const char* file, int line);
extern "C" void MemFree(void* p, const char* file, int line);
void* VAlloc(int size);
void VFree(void* p);
void FatalError(const char* message);
void SystemIdle(void);
void SystemPoll(void);
int ArchiveFindOffset(int type, int id);      // 0x800190F0
int SysCheckMemory(void);                     // 0x800FC4A0
void CacheNotify(void* p, int size);          // 0x802868EC
void StreamHeapRelease(void* base);           // 0x80286A88
int StreamHeapSize(void);                     // 0x80286AE0
void* StreamHeapBase(void);                   // 0x80286AE8

class VMemList {
public:
    VMemList();
    u32 data[8];
};

class VMem {
public:
    VMem() : mUnk0(0), mUnk4(0), mUnk28(0) {}
    ~VMem() {}

    int Contains(int p);
    void Compact(u32 size);
    void Free(void* p);
    void* Alloc(int size, int a, int b);
    void* Base();
    void Init(void* base, int size, int a, int b);

    int mUnk0;
    int mUnk4;
    VMemList mList;
    int mUnk28;
    int mUnk2C;
    int mUnk30;
};

typedef void (*StreamCallback)(int result, void* buffer, void* user);

struct StreamRequest {
    StreamRequest* left;   // tree child; also the free-list link
    StreamRequest* right;  // tree child; also the queue link
    int offset;
    int size;
    void* buffer;
    int pool;
    StreamCallback callback;
    void* user;
};

struct StreamNode {
    void* data;
    StreamNode* next;
};

struct StreamEntry {
    u8 pad0[2];
    u8 priority;
    u8 pad3;
    int id;
    int size;
};

class StreamQueue {
public:
    StreamQueue() : lock(0), root(0), head(0), tail(0) {}
    ~StreamQueue() { Clear(); }
    static void operator delete(void* p) { VFree(p); }

    StreamRequest* Pop() {
        StreamRequest* r = 0;
        if (lock == 0) {
            lock = 1;
            if (head != 0) {
                r = head;
                head = head->right;
                if (head == 0) {
                    tail = 0;
                }
            }
            lock = 0;
        }
        return r;
    }

    void Clear();
    void Flush();
    void Push(StreamRequest* r);

    int lock;
    StreamRequest* root;
    StreamRequest* head;
    StreamRequest* tail;
};

StreamNode sNodes[512];
VMem sMem[2];
char sPath[256];
DVDFileInfo sFile;
StreamQueue sQueue0;
StreamQueue sQueue1;
u8 sReadBuf[0x2000] __attribute__((aligned(32)));

int sCbLock;
DVDFileInfo* sDeferredInfo;
StreamNode* sFreeNodes;
StreamNode* sCacheList;
s32 sEntryNum;
volatile StreamRequest* sCurRequest;
volatile int sFileOpen;
volatile int sPendingBytes;
int sCancel;
int sBusy;
int sOverflow;
int sUnk5084;
int sUnk5088;
int sUnk508C;
int sChunk;
int sChunkOffset;
int sRemaining;
StreamRequest* sRequestPool;
StreamRequest* sFreeRequests;

s32 sDeferredResult = -1;
int sLastPool = -1;

void StreamReadCallback(s32 result, DVDFileInfo* fi);
int StreamStartNext(void);

inline void FreeRequest(StreamRequest* r) {
    if (r != 0) {
        r->left = 0;
        r->right = 0;
        r->offset = 0;
        r->size = 0;
        r->buffer = 0;
        r->callback = 0;
        r->user = 0;
        r->left = sFreeRequests;
        sFreeRequests = r;
    }
}

inline StreamRequest* AllocRequest(void) {
    StreamRequest* r;

    if (sFreeRequests != 0) {
        r = sFreeRequests;
        sFreeRequests = r->left;
    } else {
        r = 0;
    }

    if (r != 0) {
        r->left = 0;
        r->right = 0;
        r->offset = 0;
        r->size = 0;
        r->buffer = 0;
        r->callback = 0;
        r->user = 0;
    }
    return r;
}

inline void StreamLock(void) {
    if (sCbLock == 0) {
        sCbLock = 1;
    }
}

inline void StreamUnlock(void) {
    if (sCbLock == 2) {
        StreamReadCallback(sDeferredResult, sDeferredInfo);
    }
    sCbLock = 0;
}

inline bool StreamActive(void) { return sFileOpen != 0 || sPendingBytes != 0; }

inline int StreamKick(void) {
    if (sPendingBytes != 0 && sFileOpen == 0 && sBusy == 0) {
        if (!DVDFastOpen(sEntryNum, &sFile)) {
            return 0;
        }
        sFileOpen = 1;
        if (!StreamStartNext()) {
            DVDClose(&sFile);
            sFileOpen = 0;
            return 0;
        }
    }
    return 1;
}

void StreamFlushQueues(void) {
    sQueue0.Flush();
    sQueue1.Flush();
}

void StreamCancelAll(void) {
    StreamRequest* r;

    if (sPendingBytes != 0) {
        sCancel = 1;
        DVDCancelAllAsync(0);
        if (sFileOpen == 0) {
            sCancel = 0;
        }

        sQueue0.Flush();
        for (r = sQueue0.Pop(); r != 0; r = sQueue0.Pop()) {
            r->callback(0, 0, r->user);
            FreeRequest(r);
        }
        sPendingBytes = 0;
    }
}

void StreamUpdate(void) {
    StreamNode* n;
    u32 start;
    int elapsed;

    if (sOverflow != 0) {
        FatalError("Streaming buffer overflow!");
    }

    if (sCacheList != 0) {
        sBusy = 1;
        StreamLock();
        start = OSGetTick();
        elapsed = 0;
        while ((n = sCacheList) != 0 && elapsed < 10000) {
            sCacheList = n->next;
            sMem[1].Free(n->data);
            n->data = 0;
            n->next = sFreeNodes;
            sFreeNodes = n;
            elapsed = OSGetTick() - start;
        }
        StreamUnlock();
    }

    if (sPendingBytes != 0) {
        if (sFileOpen == 0 && sBusy != 0) {
            sBusy = 0;
        }
    } else {
        SystemIdle();
        SystemPoll();
        if (!StreamActive()) {
            StreamLock();
            sMem[0].Compact(0x10000);
            StreamUnlock();
        }
    }

    if (sPendingBytes != 0) {
        StreamFlushQueues();
        StreamKick();
    }
}

inline int StreamFindPool(void* p) {
    int i;
    VMem* m = sMem;
    for (i = 0; i < 2; i++, m++) {
        if (m->Contains((int)p)) {
            break;
        }
    }
    return i;
}

void StreamFree(void* p) {
    int i;
    StreamNode* n;
    StreamNode* prev;
    StreamNode* cur;

    if (p != 0) {
        i = StreamFindPool(p);
        if (i < 2) {
            if (i == 0 || (n = sFreeNodes) == 0) {
                sBusy = 1;
                StreamLock();
                sMem[i].Free(p);
                StreamUnlock();
            } else {
                sFreeNodes = n->next;
                prev = 0;
                cur = sCacheList;
                n->data = p;
                while (cur != 0 && (u32)cur->data < (u32)n->data) {
                    prev = cur;
                    cur = cur->next;
                }
                if (prev != 0) {
                    n->next = prev->next;
                    prev->next = n;
                } else {
                    n->next = sCacheList;
                    sCacheList = n;
                }
            }
        }
    }
}

int StreamRead(int type, StreamEntry* e, StreamCallback cb, void* user) {
    int id = e->id;
    int size = e->size;
    int pool;
    int offset;
    StreamRequest* r;

    switch (type) {
    case 7:
        pool = 1;
        break;
    default:
        pool = 0;
        break;
    }

    if (size > 0) {
        offset = ArchiveFindOffset(type, id);
        if (offset >= 0) {
            r = AllocRequest();
            if (r != 0) {
                r->pool = pool;
                r->offset = offset;
                r->size = size;
                r->callback = cb;
                r->user = user;
                if (e->priority == 0) {
                    sQueue0.Push(r);
                } else {
                    sQueue1.Push(r);
                }
                sPendingBytes += size;
                return StreamKick();
            }
        }
    }

    cb(0, 0, user);
    return 0;
}

void* StreamAlloc(int size) { return sMem[0].Alloc(size, 0, 0); }

int StreamSetFile(const char* path) {
    strncpy(sPath, path, 256);
    sEntryNum = DVDConvertPathToEntrynum((char*)path);
    return 1;
}

void StreamShutdown(void) {
    void* base;

    StreamCancelAll();
    while (sFileOpen != 0) {
    }

    StreamHeapRelease(sMem[0].Base());
    base = sMem[1].Base();
    if (base != 0) {
        VFree(base);
    }

    if (sRequestPool != 0) {
        MemFree(sRequestPool, "DataStream.cpp", 313);
        sRequestPool = 0;
    }

    sFreeRequests = 0;
    sQueue0.Clear();
    sUnk508C = sUnk5088;
    sUnk5088 = 0;
}

void StreamNop(void) {}

void StreamInit(void) {
    int i;
    int size;
    void* buf;

    size = StreamHeapSize();
    sMem[0].Init(StreamHeapBase(), size, 1, 1);
    sCbLock = 0;

    if (!SysCheckMemory()) {
        buf = VAlloc(0x2C0000);
        if (buf == 0) {
            FatalError("Unable to allocate animation buffer!\n");
        }
        sMem[1].Init(buf, 0x2C0000, 0, 0);
    } else {
        buf = VAlloc(0x3C0000);
        if (buf == 0) {
            FatalError("Unable to allocate animation buffer!\n");
        }
        sMem[1].Init(buf, 0x3C0000, 0, 0);
    }

    sRequestPool = (StreamRequest*)MemAlloc(0x8000, "DataStream.cpp", 297);
    sFreeRequests = 0;
    for (i = 0; i < 0x400; i++) {
        sRequestPool[i].left = sFreeRequests;
        sFreeRequests = &sRequestPool[i];
    }

    sQueue0.Clear();
    sQueue1.Clear();

    sFileOpen = 0;
    sPendingBytes = 0;
    sCancel = 0;
    sBusy = 0;
    sOverflow = 0;
    sLastPool = -1;
    sCurRequest = 0;
    sUnk5088 = sUnk508C;
    sUnk5084 = 0;
    sCacheList = 0;
    sFreeNodes = 0;
    for (i = 0; i < 512; i++) {
        sNodes[i].next = sFreeNodes;
        sFreeNodes = &sNodes[i];
    }
}

int StreamStartNext(void) {
    StreamRequest* r;
    StreamNode* n;
    int left;

    for (;;) {
        r = sQueue1.Pop();
        if (r == 0) {
            r = sQueue0.Pop();
        }
        if (r == 0) {
            return 0;
        }

        r->buffer = sMem[r->pool].Alloc(r->size, (int)r->callback, (int)r->user);
        if (r->buffer == 0 && r->pool == 1 && sCacheList != 0) {
            sBusy = 1;
            StreamLock();
            while ((n = sCacheList) != 0) {
                sCacheList = n->next;
                sMem[1].Free(n->data);
                n->data = 0;
                n->next = sFreeNodes;
                sFreeNodes = n;
            }
            StreamUnlock();
            r->buffer = sMem[r->pool].Alloc(r->size, (int)r->callback, (int)r->user);
        }

        if (r->buffer != 0) {
            sCurRequest = r;
            if (r->pool == 0) {
                sChunk = r->size;
            } else {
                sChunk = r->size < 0x800 ? r->size : 0x800;
            }
            sChunkOffset = 0;
            sRemaining = r->size - sChunk;
            if (r->pool == 0) {
                DVDReadAsyncPrio(&sFile, r->buffer, sChunk, r->offset, StreamReadCallback, 2);
            } else {
                DVDReadAsyncPrio(&sFile, sReadBuf, sChunk, r->offset, StreamReadCallback, 2);
            }
            return 1;
        }

        r->callback(0, 0, r->user);
        left = sPendingBytes - r->size;
        sPendingBytes = left > 0 ? left : 0;
        sLastPool = r->pool;
        FreeRequest(r);
    }
}

inline bool StreamResultOk(s32 result) { return sCancel == 0 && result >= 0; }

void StreamReadCallback(s32 result, DVDFileInfo* fi) {
    int ok;
    int maxChunk;
    int left;
    volatile StreamRequest* r;

    ok = StreamResultOk(result);

    if (sCbLock == 1) {
        sDeferredInfo = fi;
        sDeferredResult = result;
        sCbLock = 2;
        return;
    }

    if (ok) {
        if (sCurRequest->pool == 0) {
            maxChunk = 0x2000;
        } else {
            maxChunk = 0x800;
        }

        if (sCurRequest->pool != 0) {
            memcpy((u8*)sCurRequest->buffer + sChunkOffset, sReadBuf, sChunk);
        }
        DCFlushRange((u8*)sCurRequest->buffer + sChunkOffset, sChunk);

        if (sCurRequest->pool == 1) {
            u32 page = (u32)sCurRequest->buffer & ~0xFFF;
            if (page != (((u32)sCurRequest->buffer + sChunk) & ~0xFFF)) {
                CacheNotify((void*)page, 0x1000);
            }
        }

        if (sRemaining > 0) {
            sChunkOffset += sChunk;
            sChunk = sRemaining < maxChunk ? sRemaining : maxChunk;
            sRemaining -= sChunk;
            DVDReadAsyncPrio(&sFile, sReadBuf, sChunk, sCurRequest->offset + sChunkOffset, StreamReadCallback, 2);
            return;
        }

        sCurRequest->callback(1, sCurRequest->buffer, sCurRequest->user);
        if (sCurRequest->pool == 1) {
            CacheNotify(sCurRequest->buffer, sCurRequest->size);
        }
        left = sPendingBytes - sCurRequest->size;
        sPendingBytes = left > 0 ? left : 0;
    } else {
        r = sCurRequest;
        left = sPendingBytes - r->size;
        sPendingBytes = left > 0 ? left : 0;
        r->callback(-1, 0, r->user);
        sMem[sCurRequest->pool].Free(sCurRequest->buffer);
    }

    FreeRequest((StreamRequest*)sCurRequest);
    sCurRequest = 0;

    if (!ok || !StreamStartNext()) {
        DVDClose(&sFile);
        sFileOpen = 0;
        sCancel = 0;
    }
}

void StreamQueue::Clear() {
    StreamRequest* r;

    Flush();
    lock = 1;
    while ((r = head) != 0) {
        head = r->right;
        FreeRequest(r);
    }
    root = 0;
    head = 0;
    tail = 0;
    lock = 0;
}

void TreeToList(StreamRequest* node, StreamRequest** first, StreamRequest** last);

void StreamQueue::Flush() {
    StreamRequest* first;
    StreamRequest* last;

    if (root != 0) {
        first = 0;
        last = 0;
        TreeToList(root, &first, &last);
        root = 0;
        lock = 1;
        if (head != 0 && tail != 0) {
            tail->right = first;
        } else {
            head = first;
        }
        tail = last;
        lock = 0;
    }
}

void TreeInsert(StreamRequest* node, StreamRequest* r);

void StreamQueue::Push(StreamRequest* r) {
    if (r != 0) {
        if (root != 0) {
            TreeInsert(root, r);
        } else {
            root = r;
        }
    }
}

inline void SetNext(StreamRequest* a, StreamRequest* b) { a->right = b; }

inline void TreeToList(StreamRequest* node, StreamRequest** first, StreamRequest** last) {
    StreamRequest* left = node->left;
    StreamRequest* right = node->right;

    node->right = 0;
    node->left = 0;

    if (left != 0) {
        TreeToList(left, first, last);
    }

    if (*first != 0) {
        SetNext(*last, node);
    } else {
        *first = node;
    }
    *last = node;

    if (right != 0) {
        TreeToList(right, first, last);
    }
}


inline void TreeInsert(StreamRequest* node, StreamRequest* r) {
    if (r->offset <= node->offset) {
        if (node->left != 0) {
            TreeInsert(node->left, r);
        } else {
            node->left = r;
        }
    } else {
        if (node->right != 0) {
            TreeInsert(node->right, r);
        } else {
            node->right = r;
        }
    }
}

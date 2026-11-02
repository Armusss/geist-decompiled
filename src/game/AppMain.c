// AppMain.c: application main loop and start-up (0x800866D8-0x800886C8).
// A small file-chunk handler for a table of 0x20-byte entries (registered with the file loader), the
// intrusive circular list helpers shared with the effect code, the per-frame "AppMain" loop with its
// FPS counter and fade, and the start-up sequence that initialises every game system in order.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;

// Table entry of the chunk handled by the first functions (0x20 bytes each).
struct ChunkEntry {
    int mId;       // 0x00
    int mArg[7];   // 0x04
};

struct ChunkHeader {
    int mVersion;  // 0x00 1
    int mCount;    // 0x04 number of ChunkEntry records following the header
};

// Intrusive doubly linked list node. The list head points at the first node, whose mPrev is the last node;
// the last node's mNext is null.
struct ListNode {
    ListNode* mPrev;  // 0x00
    ListNode* mNext;  // 0x04
};

// Table walked by the (empty) startup loop: three words per entry, the last one a count.
struct CountEntry {
    int mUnk0;
    int mUnk4;
    int mCount;
};

#ifdef __cplusplus
extern "C" {
#endif
void FileFlushBuffer(void);
void fn_800070C0(void* entry);
void fn_80088C10(int id, void* name, int a, int b, int c, int d, int e, int f);
void fn_80088FB0(int id, int on);
void fn_80088F0C(int id, int on);
void fn_80088E68(int id, int on);
#ifdef __cplusplus
}
#endif

int FileAddHandler(int tag, int (*handler)(void*, int, void*), void* arg);

extern CountEntry lbl_803983C0[];

static ChunkHeader* sChunk;
static ChunkEntry* sEntries;
static int sTableCount;

int ChunkHandler(void* data, int size, void* arg);

// Starts every entry of the loaded chunk.
int ChunkStart(void)
{
    int off;
    ChunkEntry* e;
    int i;
    if (sChunk != 0 && sEntries != 0) {
        off = 0;
        for (i = 0; i < sChunk->mCount; i++) {
            e = (ChunkEntry*)((u8*)sEntries + off);
            fn_80088C10(e->mId, &e->mArg[0], e->mArg[1], e->mArg[2], e->mArg[3], e->mArg[4], e->mArg[5], e->mArg[6]);
            fn_80088FB0(e->mId, 1);
            fn_80088F0C(e->mId, 1);
            fn_80088E68(e->mId, 0);
            off += 0x20;
        }
    }
    return 1;
}

// Registers the chunk handler (tag 0x20) with the file loader.
int ChunkInit(void)
{
    FileAddHandler(0x20, (int (*)(void*, int, void*))ChunkHandler, 0);
    sChunk = 0;
    sEntries = 0;
    return 1;
}

// Chunk handler: remembers the header and the entry table, and prepares every entry.
int ChunkHandler(void* data, int size, void* arg)
{
    int off;
    int i;
    ChunkHeader* chunk;
    FileFlushBuffer();
    chunk = size > 0 ? (ChunkHeader*)data : 0;
    sChunk = chunk;
    if (chunk->mVersion == 1) {
        u8 ok = 0;
        if (size - 8 > 0 && chunk->mCount * 32 != 0) {
            ok = 1;
        }
        if (ok) {
            sEntries = (ChunkEntry*)((u8*)data + 8);
        } else {
            sEntries = 0;
        }
        for (i = 0, off = 0; i < sChunk->mCount; i++) {
            fn_800070C0((u8*)sEntries + off + 4);
            off += 0x20;
        }
        return 1;
    }
    return 0;
}

void ChunkNop(void)
{
}

int ChunkTableWalk(void)
{
    int i;
    int j;
    CountEntry* e;
    sTableCount = 11;
    e = lbl_803983C0;
    for (i = 0; i < 11; i++, e++) {
        for (j = 0; j < e->mCount; j++) {
        }
    }
    return j;
}

// List helpers shared with the effect code (they keep their fn_ names: other files call them).
#ifdef __cplusplus
extern "C" {
#endif
// 0x80086928: first node for which pred(node, key) returns 0
ListNode* fn_80086928(ListNode** list, u32 key, int (*pred)(ListNode*, u32))
{
    ListNode* node = *list;
    while (node != 0) {
        if (pred(node, key) == 0) {
            return node;
        }
        node = node->mNext;
    }
    return 0;
}

// 0x800869A0: node number i
ListNode* fn_800869A0(ListNode** list, int i)
{
    ListNode* node = *list;
    int n = 0;
    while (node != 0) {
        if (n == i) {
            return node;
        }
        node = node->mNext;
        n++;
    }
    return 0;
}

// 0x800869CC: number of nodes
int fn_800869CC(ListNode** list)
{
    int n = 0;
    ListNode* node;
    for (node = *list; node != 0; node = node->mNext) {
        n++;
    }
    return n;
}

// 0x800869EC: unlinks a node
void fn_800869EC(ListNode** list, ListNode* node)
{
    ListNode* head = *list;
    ListNode* last;
    if (head == 0) {
        return;
    }
    if (node != 0) {
    } else {
        return;
    }
    if (node == head) {
        if (head != 0) {
            *list = head->mNext;
            if (*list != 0) {
                (*list)->mPrev = head->mPrev;
            }
            head->mPrev = 0;
            head->mNext = 0;
        }
    } else {
        last = head->mPrev;
        if (node == last) {
            if (head != 0) {
                if (last == head) {
                    *list = 0;
                } else {
                    head->mPrev = last->mPrev;
                    if ((*list)->mPrev != 0) {
                        (*list)->mPrev->mNext = 0;
                    }
                }
                last->mPrev = 0;
                last->mNext = 0;
            }
        } else {
            node->mNext->mPrev = node->mPrev;
            node->mPrev->mNext = node->mNext;
        }
    }
    node->mPrev = 0;
    node->mNext = 0;
}

// 0x80086AC0: takes the last node off the list
ListNode* fn_80086AC0(ListNode** list)
{
    ListNode* head = *list;
    ListNode* last;
    if (head == 0) {
        return 0;
    }
    last = head->mPrev;
    if (last == head) {
        *list = 0;
    } else {
        head->mPrev = last->mPrev;
        if ((*list)->mPrev != 0) {
            (*list)->mPrev->mNext = 0;
        }
    }
    last->mPrev = 0;
    last->mNext = 0;
    return last;
}

// 0x80086B20: adds a node at the end
void fn_80086B20(ListNode** list, ListNode* node)
{
    if (*list != 0) {
        node->mPrev = (*list)->mPrev;
        (*list)->mPrev->mNext = node;
        (*list)->mPrev = node;
        return;
    }
    node->mNext = 0;
    node->mPrev = node;
    *list = node;
}

// 0x80086B60: adds a node at the front
void fn_80086B60(ListNode** list, ListNode* node)
{
    if (*list != 0) {
        node->mNext = *list;
        node->mPrev = (*list)->mPrev;
        (*list)->mPrev = node;
    } else {
        node->mNext = 0;
        node->mPrev = node;
    }
    *list = node;
}

#ifdef __cplusplus
}
#endif

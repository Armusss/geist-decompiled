// Ring-buffer queues (0x80080714-0x80080F88): up to 16 queues of fixed-size elements over caller
// memory, tracked by a used-slot bitmask. Elements are pushed at the write pointer and popped at the
// read pointer; a queue either blocks when full or overwrites (flag 4).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

extern "C" {
void* memcpy(void* dst, const void* src, unsigned long n);

// Bit-set helpers in another file, not named yet.
int fn_80025C14(u32* mask, int value, int count); // find a clear bit
void fn_80025CFC(u32* mask, int bit);             // clear a bit
}

struct Queue {
    int size;
    u32 flags;
    int capacity;
    int free;
    u8* read;
    u8* write;
    u8* begin;
    u8* end;
};

static Queue sQueues[16];
static u32 sQueueMask[1];
static int sQueueCount;

#define BIT_TEST(mask, i) ((mask)[(int)(i) / 32] & (1 << ((int)(i) & 31)))
#define QueueValid(q) ((q) < 16 && BIT_TEST(sQueueMask, q))

void QueueClear(u32 q) {
    Queue* p;

    if (!QueueValid(q)) {
        return;
    }
    p = &sQueues[q];
    p->free = p->capacity;
    p->write = p->read = p->begin;
}

u8 QueueFull(u32 q) {
    if (!QueueValid(q)) {
        return 0;
    }
    return sQueues[q].free == 0;
}

void* QueuePeek(u32 q, int n) {
    u8* e;
    int i;

    if (!QueueValid(q)) {
        return 0;
    }
    if (sQueues[q].capacity - sQueues[q].free <= n) {
        return 0;
    }
    e = sQueues[q].read;
    for (i = 0; i < n; i++) {
        e += sQueues[q].size;
        if (e >= sQueues[q].end) {
            e = sQueues[q].begin;
        }
    }
    return e;
}

int QueueForEach(u32 q, void* user, int (*func)(void* elem, void* user), u32 flags) {
    int forward;
    int r;
    u8* e;
    int i;

    if (!QueueValid(q)) {
        return -1;
    }
    if (flags & 0x10) {
        forward = 1;
    } else if (flags & 0x100) {
        forward = 0;
    } else {
        forward = 0;
    }
    if (forward) {
        r = 0;
        i = 0;
        e = sQueues[q].read;
        while (i < sQueues[q].capacity - sQueues[q].free && r == 0) {
            r = func(e, user);
            if (r == 0) {
                i++;
                e += sQueues[q].size;
                if (e >= sQueues[q].end) {
                    e = sQueues[q].begin;
                }
            }
        }
        if (r) {
            return i;
        }
    } else {
        r = 0;
        e = sQueues[q].write;
        e -= sQueues[q].size;
        i = sQueues[q].capacity - sQueues[q].free;
        if (e < sQueues[q].begin) {
            e = sQueues[q].end - sQueues[q].size;
        }
        while (i > 0 && r == 0) {
            r = func(e, user);
            if (r == 0) {
                i--;
                e -= sQueues[q].size;
                if (e < sQueues[q].begin) {
                    e = sQueues[q].end - sQueues[q].size;
                }
            }
        }
        if (r) {
            return i;
        }
    }
    return -1;
}

int QueuePopEach(u32 q, int n, int (*func)(void* elem, int size, void* user), void* user) {
    int i;

    if (!QueueValid(q) || n < 0) {
        return 0;
    }
    i = 0;
    while (i < n && sQueues[q].capacity > sQueues[q].free) {
        if (func(sQueues[q].read, sQueues[q].size, user) == 1) {
            sQueues[q].read += sQueues[q].size;
            if (sQueues[q].read >= sQueues[q].end) {
                sQueues[q].read = sQueues[q].begin;
            }
            sQueues[q].free++;
            i++;
        } else {
            return i;
        }
    }
    return i;
}

int QueueSkip(u32 q, int n) {
    int i;

    if (!QueueValid(q) || n < 0) {
        return 0;
    }
    for (i = 0; i < n && sQueues[q].capacity > sQueues[q].free; i++) {
        sQueues[q].read += sQueues[q].size;
        if (sQueues[q].read >= sQueues[q].end) {
            sQueues[q].read = sQueues[q].begin;
        }
        sQueues[q].free++;
    }
    return i;
}

inline int QueueWrite(u32 q, const u8* src, int n, u32 overwrite) {
    int i;

    for (i = 0; i < n && (sQueues[q].free > 0 || overwrite); i++) {
        memcpy(sQueues[q].write, src, sQueues[q].size);
        src += sQueues[q].size;
        sQueues[q].write += sQueues[q].size;
        if (sQueues[q].write >= sQueues[q].end) {
            sQueues[q].write = sQueues[q].begin;
        }
        sQueues[q].free--;
    }
    if (sQueues[q].free < 0) {
        sQueues[q].free = 0;
    }
    return i;
}

int QueuePush(u32 q, const void* src, int n, u32 flags) {
    u32 mode;

    if (!QueueValid(q) || n < 0) {
        return 0;
    }
    if (flags != 0) {
        if (flags & 2) {
            mode = 1;
        } else if (flags & 4) {
            mode = 2;
        } else {
            mode = sQueues[q].flags;
        }
    } else {
        mode = sQueues[q].flags;
    }
    return QueueWrite(q, (const u8*)src, n, mode & 4);
}

void QueueFree(u32 q) {
    if (!QueueValid(q)) {
        return;
    }
    sQueues[q].size = 0;
    sQueues[q].flags = 0;
    sQueues[q].capacity = 0;
    sQueues[q].free = 0;
    sQueues[q].write = 0;
    sQueues[q].read = 0;
    sQueues[q].begin = 0;
    sQueues[q].end = 0;
    fn_80025CFC(sQueueMask, q);
    sQueueCount--;
}

int QueueCreate(u8* buffer, int size, int capacity, u32 flags) {
    int q;

    if (size <= 0 || capacity <= 0) {
        return -1;
    }
    if (sQueueCount >= 16) {
        return -1;
    }
    q = fn_80025C14(sQueueMask, 1, 16);
    if (q != -1) {
        sQueues[q].size = size;
        sQueues[q].flags = 0;
        sQueues[q].capacity = capacity;
        sQueues[q].free = capacity;
        sQueues[q].read = buffer;
        sQueues[q].write = buffer;
        sQueues[q].begin = buffer;
        sQueues[q].end = buffer + capacity * size;
        sQueueMask[q / 32] |= 1 << (q & 31);
        sQueueCount++;
        if (flags & 2) {
            sQueues[q].flags |= 2;
        } else if (flags & 4) {
            sQueues[q].flags |= 4;
        } else {
            sQueues[q].flags |= 2;
        }
        return q;
    }
    return -1;
}

void QueueInit(void) {
    sQueueMask[0] = 0;
    sQueueCount = 0;
}

void QueueShutdown(void) {
    sQueueMask[0] = 0;
    sQueueCount = 0;
}

// asyncio.cpp: buffered asynchronous DVD file streams (0x800E1144-0x800E1B5C).
// A stream reads ahead through four 0x2000-byte buffers with DVDReadAsyncPrio. Files that start with a
// "BILZ" header are zlib-compressed and inflated on the fly (zlib 1.1.4, linked in the game).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef signed long s32;
typedef unsigned char u8;

extern "C" {
typedef struct DVDCommandBlock {
    u32 data[12];
} DVDCommandBlock;
typedef struct DVDFileInfo DVDFileInfo;
typedef void (*DVDCallback)(s32 result, DVDFileInfo* fileInfo);
struct DVDFileInfo {
    DVDCommandBlock cb;
    u32 startAddr;
    u32 length;
    DVDCallback callback;
};
int DVDOpen(const char* fileName, DVDFileInfo* fileInfo);
int DVDClose(DVDFileInfo* fileInfo);
int DVDReadAsyncPrio(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset, DVDCallback callback, s32 prio);
void OSYieldThread(void);
void* memcpy(void* dst, const void* src, u32 n);
void exit(int status);

typedef struct z_stream_s {
    u8* next_in;
    u32 avail_in;
    u32 total_in;
    u8* next_out;
    u32 avail_out;
    u32 total_out;
    char* msg;
    void* state;
    void* zalloc;
    void* zfree;
    void* opaque;
    int data_type;
    u32 adler;
    u32 reserved;
} z_stream;
int inflateInit_(z_stream* strm, const char* version, int stream_size);
int inflate(z_stream* strm, int flush);
int inflateEnd(z_stream* strm);
}

void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);
void DebugPrintf(const char* format, ...);
int FileReadSync(void* file, void* dst, int size, int offset);   // 0x80007BC0
void FileFlushBuffer(void* dst, int size);                       // 0x80007088
void SystemIdle(void);                                            // 0x80007EC0
void SystemPoll(void);                                            // 0x8000AA20

struct AsyncBuffer {
    int start;
    int end;
    int ready;
    int last;
    int pad[4];
    u8 data[0x2040];
};

struct AsyncFile {
    DVDFileInfo fi;
    int cursor;
    int size;
    int rawSize;
    int rawUnk;
    int compressed;
    int readBuf;
    int idle;
    int cancel;
    int pad5C;
    AsyncBuffer buf[4];
    z_stream zs;
};

#define ZS_ACTIVE ((void*)0xBEEF)

static void AsyncReadCallback(s32 result, DVDFileInfo* fi);

void AsyncClose(AsyncFile* f) {
    while (f->idle == 0) {
        OSYieldThread();
    }

    if (f->zs.opaque != 0) {
        inflateEnd(&f->zs);
        f->zs.opaque = 0;
    }

    DVDClose(&f->fi);
}

inline void AsyncCheckHeader(AsyncFile* f) {
    if (f->compressed != 0) {
        int* header = (int*)MemAlloc(0x20, "asyncio.cpp", 123);
        FileReadSync(f, header, 0x20, 0);
        FileFlushBuffer(header, 0x20);
        if (header[0] == 'BILZ') {
            f->rawSize = header[2];
            f->rawUnk = header[3];
            f->cursor += 0x20;
            f->size -= 0x20;
            f->zs.zalloc = 0;
            f->zs.zfree = 0;
            inflateInit_(&f->zs, "1.1.4", sizeof(z_stream));
            f->zs.opaque = ZS_ACTIVE;
        }
        MemFree(header, "asyncio.cpp", 149);
    }
}

int AsyncSeek(AsyncFile* f, int offset, int whence) {
    int moved = 0;

    switch (whence) {
    case 0:
        if (offset != f->cursor) {
            moved = 1;
        }
        f->cursor = offset;
        break;
    case 1:
        if (offset > 0) {
            moved = 1;
        }
        f->cursor += offset;
        break;
    default:
        return 1;
    }

    if (moved) {
        f->cancel = 1;
        while (f->idle == 0) {
            SystemIdle();
            SystemPoll();
        }

        f->readBuf = 0;
        f->idle = 1;
        f->cancel = 0;
        f->buf[0].start = 0;
        f->buf[0].end = 0;
        f->buf[0].ready = 0;
        f->buf[0].last = 0;
        f->buf[1].start = 0;
        f->buf[1].end = 0;
        f->buf[1].ready = 0;
        f->buf[1].last = 0;
        f->buf[2].start = 0;
        f->buf[2].end = 0;
        f->buf[2].ready = 0;
        f->buf[2].last = 0;
        f->buf[3].start = 0;
        f->buf[3].end = 0;
        f->buf[3].ready = 0;
        f->buf[3].last = 0;

        if (f->zs.opaque != 0) {
            inflateEnd(&f->zs);
            f->zs.opaque = 0;
        }

        AsyncCheckHeader(f);

        f->readBuf = 0;
        f->buf[0].start = f->cursor & ~0x1F;
        if (f->buf[0].start < f->size) {
            f->idle = 0;
            f->buf[0].end = f->buf[0].start + 0x2000;
            if (f->buf[0].end > f->size) {
                f->buf[0].end = (f->size + 0x1F) & ~0x1F;
                f->buf[0].last = 1;
            }
            DVDReadAsyncPrio(&f->fi, f->buf[0].data, f->buf[0].end - f->buf[0].start, f->buf[0].start,
                             AsyncReadCallback, 2);
        }
    }

    return 0;
}

int AsyncRead(AsyncFile* f, void* dst, int size, int count, int noWait) {
    int total = 0;
    int remaining;
    int avail;
    u8* src;
    u32 b;
    AsyncBuffer* buf;
    int next;
    int allocated;
    int err;
    int totalIn;
    int before;

    if (f->cursor >= f->size) {
        return 0;
    }

    b = 0;
    if ((f->cursor < f->buf[0].start || f->cursor >= f->buf[0].end) &&
        (b = 1, f->cursor < f->buf[1].start || f->cursor >= f->buf[1].end) &&
        (b = 2, f->cursor < f->buf[2].start || f->cursor >= f->buf[2].end) &&
        (b = 3, f->cursor < f->buf[3].start || f->cursor >= f->buf[3].end)) {
        b = 4;
    }

    remaining = size * count;

    if (b == 4) {
        int i;
        DebugPrintf("Stream buffer not queued\n");
        DebugPrintf("    FileCursor %d\n", f->cursor);
        DebugPrintf("    ReadBuffer %d\n", f->readBuf);
        for (i = 0; i < 4; i++) {
            DebugPrintf("    Buffer %d:  [%d - %d], Ready %d\n", i, f->buf[i].start, f->buf[i].end, f->buf[i].ready);
        }
        exit(0);
        return 0;
    }

    while (remaining > 0) {
        buf = &f->buf[b];
        if (noWait != 0 && buf->ready == 0) {
            return 0;
        }

        while (buf->ready == 0) {
            SystemIdle();
            SystemPoll();
        }

        avail = buf->end - f->cursor;
        src = buf->data + (f->cursor - buf->start);

        if (f->rawSize < 1) {
            if (remaining < avail) {
                avail = remaining;
            }
            if (dst != 0) {
                memcpy(dst, src, avail);
                dst = (u8*)dst + avail;
            }
            remaining -= avail;
            f->cursor += avail;
        } else {
            allocated = dst == 0;
            totalIn = f->zs.total_in;
            f->zs.next_in = src;
            f->zs.avail_in = avail;
            f->zs.avail_out = remaining;
            if (dst == 0) {
                dst = MemAlloc(avail, "asyncio.cpp", 316);
                f->zs.next_out = (u8*)dst;
            } else {
                f->zs.next_out = (u8*)dst;
            }

            err = inflate(&f->zs, 2);
            before = f->zs.total_in;
            if (err == 1) {
                inflateEnd(&f->zs);
                f->zs.opaque = 0;
            } else if (err != 0) {
                DebugPrintf("Error %d in inflate (tot in : %d, tot out : %d\n", err, before, f->zs.total_out);
                break;
            }

            if (f->zs.avail_out != 0) {
                DebugPrintf("Didn't fill the whole output buffer, doah!\n");
            }

            if (allocated) {
                MemFree(dst, "asyncio.cpp", 354);
            }

            avail = remaining;
            remaining = 0;
            f->cursor += before - totalIn;
        }

        total += avail;

        if (buf->end <= f->cursor) {
            buf->start = 0;
            buf->end = 0;
            buf->ready = 0;
            if (buf->last != 0) {
                break;
            }

            if (f->idle != 0) {
                if (f->compressed != 0) {
                    buf->start = 0;
                }
                next = (f->readBuf + 1) % 4;
                f->idle = 1;
                buf = &f->buf[next];
                if (buf->ready == 0) {
                    buf->start = f->buf[f->readBuf].end;
                    buf->end = buf->start + 0x2000;
                    if (buf->end > f->size) {
                        buf->end = (f->size + 0x1F) & ~0x1F;
                        buf->last = 1;
                    }
                    if (buf->end - buf->start > 0) {
                        DVDReadAsyncPrio(&f->fi, buf->data, buf->end - buf->start, buf->start,
                                         AsyncReadCallback, 2);
                        f->readBuf = next;
                        f->idle = 0;
                    }
                }
            }
        }

        b = (b + 1) & 3;
    }

    return total / size;
}

int AsyncOpen(AsyncFile* f, const char* name, int compressed) {

    f->cursor = 0;
    f->size = 0;
    f->rawSize = 0;
    f->readBuf = 0;
    f->idle = 1;
    f->cancel = 0;
    f->zs.opaque = 0;
    f->buf[0].start = 0;
    f->buf[0].end = 0;
    f->buf[0].ready = 0;
    f->buf[0].last = 0;
    f->buf[1].start = 0;
    f->buf[1].end = 0;
    f->buf[1].ready = 0;
    f->buf[1].last = 0;
    f->buf[2].start = 0;
    f->buf[2].end = 0;
    f->buf[2].ready = 0;
    f->buf[2].last = 0;
    f->buf[3].start = 0;
    f->buf[3].end = 0;
    f->buf[3].ready = 0;
    f->buf[3].last = 0;

    if (DVDOpen(name, &f->fi)) {
        f->size = f->fi.length;
        f->compressed = compressed;

        AsyncCheckHeader(f);

        f->readBuf = 0;
        f->buf[0].start = f->cursor & ~0x1F;
        if (f->buf[0].start < f->size) {
            f->idle = 0;
            f->buf[0].end = f->buf[0].start + 0x2000;
            if (f->buf[0].end > f->size) {
                f->buf[0].end = (f->size + 0x1F) & ~0x1F;
                f->buf[0].last = 1;
            }
            DVDReadAsyncPrio(&f->fi, f->buf[0].data, f->buf[0].end - f->buf[0].start, f->buf[0].start,
                             AsyncReadCallback, 2);
        }
        return 1;
    }

    return 0;
}

static void AsyncReadCallback(s32 result, DVDFileInfo* fi) {
    int next;
    AsyncFile* f = (AsyncFile*)fi;
    AsyncBuffer* buf;

    if (f->cancel != 0) {
        f->idle = 1;
    } else {
        f->buf[f->readBuf].ready = 1;
        next = (f->readBuf + 1) % 4;
        f->idle = 1;
        buf = &f->buf[next];
        if (buf->ready == 0) {
            buf->start = f->buf[f->readBuf].end;
            buf->end = buf->start + 0x2000;
            if (buf->end > f->size) {
                buf->end = (f->size + 0x1F) & ~0x1F;
                buf->last = 1;
            }
            if (buf->end - buf->start > 0) {
                DVDReadAsyncPrio(&f->fi, buf->data, buf->end - buf->start, buf->start, AsyncReadCallback, 2);
                f->readBuf = next;
                f->idle = 0;
            }
        }
    }
}

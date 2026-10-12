// File.cpp: whole-file loading and the level ("OLC") chunk loader (0x800190D4-0x80019A04).
// An OLC file is a 16-byte header (version 3, chunk count) followed by chunks, each a 16-byte descriptor
// (size, type, disc offset) and its data. Chunk data is handed to the handler registered for its type;
// each type's disc offset is remembered for streaming (ArchiveFindOffset). The OLC lives in streaming memory,
// or in VMem when a preset block is used.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/os.h>
#include <dolphin/dvd.h>
#include <string.h>

void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);
void* VAlloc(int size);
void VFree(void* p);
void* ScratchAAlloc(int size);
void FatalError(const char* message);
void DebugPrintf(const char* format, ...);
void SystemIdle(void);
void SystemPoll(void);
int FileReadSync(void* file, void* dst, int size, int offset);
void FileFlushBuffer(void* dst, int size);
void* StreamAlloc(int size);
void StreamFree(void* p);
void StreamInit(void);
void StreamShutdown(void);

f32 LoadingTime(void);                                   // 0x8001598C
void LevelLoadNames(const char* name, int* a, char* b, char* c, char* d); // 0x80015A44
void LoadingTick(void);                                  // 0x80006D1C
void LoadingSetProgress(f32 progress, f32 time);         // 0x800D049C
int LoadingActive(void);                                 // 0x800D0510
void LoadingEnd(void);                                   // 0x800D0380
void LoadingClose(void);                                 // 0x800D0254
void LevelSetSky(char* name);                            // 0x8016E760
void LevelLoaded(const char* name);                      // 0x801709A4
void LevelUnload(void);                                  // 0x801708B0
void ARAMLevelLoaded(void);                              // 0x80287014

extern void* gPresetVMemBlock;

typedef int (*ChunkHandler)(void* data, int size, void* user);

struct ChunkHandlerEntry {
    int type;
    ChunkHandler handler;
    void* user;
};

struct OlcHeader {
    int version;
    int count;
    int pad[2];
};

struct OlcChunk {
    int size;
    int type;
    int offset;
    int pad;
};

ChunkHandlerEntry sHandlers[64];
int sTypeOffsets[64];

int sHandlerCount;
void* sOlcData;
int sOlcSize;

int FileOlcSize(void) { return gPresetVMemBlock == 0 ? sOlcSize : 0; }

int ArchiveFindOffset(int type, int id) {
    int offset;

    if (type >= 0 && type < 64) {
        offset = sTypeOffsets[type];
        if (offset != -1) {
            return offset + id;
        }
    }
    return -1;
}

void* FileReadAll(const char* name, int* outSize, int progress);

inline void* FileReadStream(const char* name, int* outSize, int progress) {
    DVDFileInfo fi;
    void* buf = 0;
    u32 size = 0;

    if (DVDOpen(name, &fi)) {
        size = fi.length;
        if (size != 0) {
            size = (size + 31) & ~31;
            buf = StreamAlloc(size);
            if (buf != 0) {
                u32 left = size;
                u32 offset = 0;
                u32 chunk = 0x40000;
                while (left != 0) {
                    if (chunk > left) {
                        chunk = left;
                    }
                    FileReadSync(&fi, (u8*)buf + offset, chunk, offset);
                    offset += chunk;
                    left -= chunk;
                    if (progress) {
                        LoadingSetProgress((f32)offset / (f32)size, LoadingTime());
                    }
                }
            }
        }
        DVDClose(&fi);
    }

    if (outSize != 0) {
        *outSize = size;
    }
    return buf;
}

inline void* OlcTake(u8** cur, int* remaining, int size) {
    bool ok = *remaining > 0 && size > 0;
    void* p = ok ? *cur : 0;
    *cur += size;
    *remaining -= size;
    return p;
}

int FileLoadOLC(const char* name) {
    char sky[256];
    char b[256];
    char c[256];
    int remaining;
    int unk;
    OlcHeader* header;
    OlcChunk* chunk;
    u8* cur;
    int progress;
    int ok;
    void* data;
    int i;
    int j;

    progress = LoadingActive();
    if (gPresetVMemBlock != 0) {
        DebugPrintf("Loading OLC into ARAM\n");
        sOlcData = FileReadAll(name, &remaining, progress);
    } else {
        sOlcData = FileReadStream(name, &remaining, progress);
    }

    if (sOlcData != 0) {
        ok = 1;
        sOlcSize = remaining;
        cur = (u8*)sOlcData;
        header = (OlcHeader*)OlcTake(&cur, &remaining, 16);
        if (header != 0) {
            FileFlushBuffer(header, 16);
            if (header->version == 3) {
                for (i = 0; i < header->count; i++) {
                    LoadingTick();
                    chunk = (OlcChunk*)OlcTake(&cur, &remaining, 16);
                    if (chunk != 0) {
                        FileFlushBuffer(chunk, 16);
                        if (chunk->type >= 0 && chunk->type < 64) {
                            sTypeOffsets[chunk->type] = chunk->offset;
                        }
                        data = OlcTake(&cur, &remaining, (chunk->size + 3) & ~3);
                        if (data != 0 && chunk->type >= 0) {
                            for (j = 0; j < sHandlerCount; j++) {
                                if (chunk->type == sHandlers[j].type &&
                                    !sHandlers[j].handler(data, chunk->size, sHandlers[j].user)) {
                                    DebugPrintf("Chunk load failure: Type %d, Size %d\n%s", chunk->type, chunk->size,
                                                name);
                                    ok = 0;
                                }
                            }
                        }
                    } else {
                        return 0;
                    }
                }

                LevelLoadNames(name, &unk, sky, b, c);
                LevelSetSky(sky);
                LevelLoaded(name);
                if (progress) {
                    LoadingEnd();
                    LoadingClose();
                }
                if (gPresetVMemBlock != 0) {
                    ARAMLevelLoaded();
                }
                return ok;
            }
        }

        if (gPresetVMemBlock == 0) {
            StreamFree(sOlcData);
        }
    } else {
        DebugPrintf("Unable to read '%s'", name);
        sOlcSize = 0;
    }
    return 0;
}

void FileRemoveHandler(int type, ChunkHandler handler, void* user) {
    int i;

    for (i = 0; i < sHandlerCount; i++) {
        if (type == sHandlers[i].type && sHandlers[i].handler == handler && sHandlers[i].user == user) {
            sHandlers[i].type = -1;
            sHandlers[i].handler = 0;
            sHandlers[i].user = 0;
        }
    }
}

void FileAddHandler(int type, ChunkHandler handler, void* user) {
    int i;

    FileRemoveHandler(type, handler, user);

    for (i = 0; i < sHandlerCount; i++) {
        if (sHandlers[i].type == -1) {
            sHandlers[i].type = type;
            sHandlers[i].handler = handler;
            sHandlers[i].user = user;
            return;
        }
    }

    if (sHandlerCount < 64) {
        sHandlers[sHandlerCount].type = type;
        sHandlers[sHandlerCount].handler = handler;
        sHandlers[sHandlerCount].user = user;
        sHandlerCount++;
    }
}

void FileShutdown(void) {
    if (sOlcData != 0) {
        if (gPresetVMemBlock == 0) {
            StreamFree(sOlcData);
        } else {
            VFree(sOlcData);
        }
        sOlcData = 0;
    }
    LevelUnload();
    StreamShutdown();
}

void FileInit(void) {
    int i;

    for (i = 0; i < 64; i++) {
        sTypeOffsets[i] = -1;
    }
    StreamInit();
}

void* FileReadAll(const char* name, int* outSize, int progress) {
    DVDFileInfo fi;
    void* tmp;
    void* buf;
    u32 size;
    u32 left;
    u32 offset;
    u32 chunk;

    buf = 0;
    size = 0;
    SystemIdle();
    SystemPoll();

    if (DVDOpen(name, &fi)) {
        size = fi.length;
        if (size != 0) {
            size = (size + 31) & ~31;
            buf = VAlloc(size);
            if (buf != 0) {
                tmp = ScratchAAlloc(0x10000);
                if (tmp != 0) {
                    left = size;
                    offset = 0;
                    chunk = 0x10000;
                    while (left != 0) {
                        if (chunk > left) {
                            chunk = left;
                        }
                        FileReadSync(&fi, tmp, chunk, offset);
                        memcpy((u8*)buf + offset, tmp, chunk);
                        offset += chunk;
                        left -= chunk;
                        if (progress) {
                            LoadingSetProgress((f32)offset / (f32)size, LoadingTime());
                        }
                    }
                }
            } else {
                FatalError("Not enough memory for OLC");
            }
        }
        DVDClose(&fi);
    }

    if (outSize != 0) {
        *outSize = size;
    }
    return buf;
}

void* FileLoad(const char* name, int* outSize, int unused) {
    DVDFileInfo fi;
    int chunk;
    u8* buf;
    int length;
    int left;
    int offset;
    int read;

    if (DVDOpen(name, &fi)) {
        length = fi.length;
        left = (length + 31) & ~31;
        buf = (u8*)MemAlloc(left, "File.cpp", 290);
        offset = 0;
        while (left != 0) {
            chunk = 0x40000;
            if (left < 0x40000) {
                chunk = left;
            }
            left -= chunk;
            read = FileReadSync(&fi, buf + offset, chunk, offset);
            if (read != chunk) {
                DebugPrintf("Read error %s\n", name);
                MemFree(buf, "File.cpp", 307);
                return 0;
            }
            offset += chunk;
        }
        DVDClose(&fi);
        *outSize = length;
        return buf;
    }

    DebugPrintf("Failed to open %s\n", name);
    return 0;
}

int FileLoadInto(const char* name, void* dst, u32 maxSize) {
    DVDFileInfo fi;
    int ok = 0;
    int size;
    int read;

    if (DVDOpen(name, &fi)) {
        size = (fi.length + 31) & ~31;
        if (fi.length <= maxSize) {
            read = FileReadSync(&fi, dst, size, 0);
            if (read == size) {
                ok = 1;
            } else {
                DebugPrintf("Read error %s\n", name);
            }
        }
        DVDClose(&fi);
    }
    return ok;
}

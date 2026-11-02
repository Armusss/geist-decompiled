// ModInfo.cpp: chunk/message handler tables, the global actor array (0x1C8-byte records),
// model node lookup and animation helpers (0x800265C8-0x8003B98C).
// Names are descriptive guesses; the code is matched against the retail DOL where noted
// in worker-notes.  Struct layouts are partial and derived from the retail asm.

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef float f32;

struct Vec3 {
    f32 x, y, z;
};

struct Mtx34 {
    f32 m[12];
};

// One record of the global actor array (lbl_804D4280, count lbl_804D4288).
struct Actor {
    /*0x00*/ u8 unk00[0x38 - 0x00];
    /*0x38*/ u32 id;
    /*0x3C*/ u8 unk3C[0x50 - 0x3C];
    /*0x50*/ f32 pos[3];
    /*0x5C*/ u8 unk5C[0x68 - 0x5C];
    /*0x68*/ f32 fieldF68;
    /*0x6C*/ s32 owner;
    /*0x70*/ u32 flags;
    /*0x74*/ u8 unk74[0xB4 - 0x74];
    /*0xB4*/ f32 sphere[3];
    /*0xC0*/ u8 unkC0[0xCC - 0xC0];
    /*0xCC*/ f32 color[3];
    /*0xD8*/ u8 unkD8[0xFC - 0xD8];
    /*0xFC*/ f32 radius;
    /*0x100*/ u8 unk100[0x1C8 - 0x100];
};

typedef int (*MsgHandler)(int to, int msg, void* data, void* ctx);
typedef int (*ChunkHandler)(int type, int size, void* data, void* ctx);

// Chunk record inside a type-3 ModInfo block (0x10 bytes).
struct ChunkEntry {
    /*0x00*/ int type;
    /*0x04*/ int size;
    /*0x08*/ int data;
    /*0x0C*/ int unk0C;
};

struct ModInfoHeader {
    /*0x00*/ s16 version;
    /*0x02*/ s16 chunkCount;
    /*0x04*/ int unk04;
    /*0x08*/ int tableCount;
    /*0x0C*/ void* tables;
};

// Handler tables (0x803DDB98..0x803DE1D8), reached from one base register: one static struct.
static ChunkHandler sChunkFn[100];
static void* sChunkCtx[100];
static MsgHandler sMsgFn[100];
static void* sMsgCtx[100];

extern "C" {
extern int lbl_804D427C;
extern ModInfoHeader* lbl_804D4278;
extern Actor* lbl_804D4280;
extern int lbl_804D4288;
extern int lbl_804D428C;
extern int lbl_804D4290;
extern int lbl_804D4294;
extern int lbl_804D429C;
extern Actor* lbl_803DE1D8[];

void fn_80007118(void*);
void fn_800070C0(void*);
void FileFlushBuffer(void* dst, int size);
void fn_8016D864(Vec3* out, void* in);
void fn_8021284C(u32);
}

typedef int (*FileHandlerFn)(void*, int, void*);
void FileRemoveHandler(int type, FileHandlerFn fn, void* ctx);
void FileAddHandler(int type, FileHandlerFn fn, void* ctx);
void DebugDrawSphere(int r, int g, int b, int a, float x, float y, float z, float radius);
void DebugDrawBoneBox(Vec3* pos, void* mtx, int r, int g, int b);
void ActorGetMatrix(Mtx34* m, Actor* a);
void ActorUnlinkOwnerObjects(Actor* a);

// 0x800265C8
void ModInfoResetCount() {
    lbl_804D427C = 0;
}

// 0x800265D4
extern "C" int SendMsg(int to, int msg, void* data) {
    int type;
    if (to != -1) {
        type = (u32)to >> 24;
        to = to & 0xFFFFFF;
        if (type >= 0) {
            if (type < 100) {
                MsgHandler fn = sMsgFn[type];
                if (fn != 0)
                    return fn(to, msg, data, sMsgCtx[type]);
            }
        }
    }
    return 0;
}

// 0x80026640 (called by no other unit; identical to the one below)
void MsgHandlerSetA(int type, MsgHandler fn, void* ctx) {
    sMsgFn[type] = fn;
    sMsgCtx[type] = ctx;
}

// 0x80026660 (called from other units under this placeholder name)
extern "C" void fn_80026660(int type, MsgHandler fn, void* ctx) {
    sMsgFn[type] = fn;
    sMsgCtx[type] = ctx;
}

// 0x80026680 (called from other units under this placeholder name)
extern "C" void fn_80026680(int type, ChunkHandler fn, void* ctx) {
    sChunkFn[type] = fn;
    sChunkCtx[type] = ctx;
}

int ModInfoLoad(void* data, int size, void* ctx);

// 0x800266A0: clear all handler tables and unregister the chunk loader
int ModInfoHandlersClear() {
    int i;
    for (i = 0; i < 100; i++) {
        sChunkFn[i] = 0;
        sChunkCtx[i] = 0;
        sMsgFn[i] = 0;
        sMsgCtx[i] = 0;
    }
    FileRemoveHandler(0xB, ModInfoLoad, 0);
    return 1;
}

// 0x800267EC: clear all handler tables and register the chunk loader
int ModInfoHandlersInit() {
    int i;
    for (i = 0; i < 100; i++) {
        sChunkFn[i] = 0;
        sChunkCtx[i] = 0;
        sMsgFn[i] = 0;
        sMsgCtx[i] = 0;
    }
    FileAddHandler(0xB, ModInfoLoad, 0);
    lbl_804D4278 = 0;
    lbl_804D427C = 0;
    return 1;
}

static inline bool Fits(int avail, int need) {
    return avail > 0 && need > 0;
}

// 0x80026944: chunk loader for file type 0xB; dispatches each chunk to its handler
int ModInfoLoad(void* data, int size, void* ctx) {
    char* p = (char*)data;
    int left;
    lbl_804D4278 = (ModInfoHeader*)(Fits(size, size) ? data : 0);
    p += 0x10;
    left = size - 0x10;
    fn_80007118(&lbl_804D4278->version);
    fn_80007118(&lbl_804D4278->chunkCount);
    fn_800070C0(&lbl_804D4278->unk04);
    fn_800070C0(&lbl_804D4278->tableCount);
    if (lbl_804D4278->version == 3) {
        int bytes = lbl_804D4278->tableCount * 16;
        int i;
        ChunkEntry* e;
        int* tab;
        lbl_804D4278->tables = Fits(left, bytes) ? p : 0;
        p += bytes;
        left -= bytes;
        FileFlushBuffer(lbl_804D4278->tables, bytes);
        tab = (int*)lbl_804D4278->tables;
        for (i = 0; i != lbl_804D4278->tableCount; i++) {
            int len = tab[i * 4];
            tab[i * 4] = (int)(Fits(left, len) ? p : 0);
            p += len;
            left -= len;
        }
        {
            int n = lbl_804D4278->chunkCount;
            int ebytes = n * 16;
            e = (ChunkEntry*)(Fits(left, ebytes) ? p : 0);
            p += ebytes;
            FileFlushBuffer(e, ebytes);
            for (i = 0; i < lbl_804D4278->chunkCount; i++) {
                int t;
                e->data = (int)(p + e->data);
                t = e->type;
                if (t >= 0 && t < 0x1B && sChunkFn[t] != 0)
                    sChunkFn[t](t, e->data, (void*)e->size, sChunkCtx[t]);
                e++;
            }
        }
    }
    return 1;
}

// 0x80027550
void IntSwap(int* a, int i, int j) {
    int t = a[i];
    a[i] = a[j];
    a[j] = t;
}

// TODO (not drafted yet, 0x55C bytes at 0x80026B50): actor spawn / free list management
// TODO (not drafted yet, 0xAC bytes at 0x800270AC)
// TODO (not drafted yet, 0x3F8 bytes at 0x80027158)

// 0x8002756C: debug draw of every flagged actor (sphere + bone box)
void ActorDebugDrawAll() {
    int i;
    if (lbl_804D429C != 0) {
        for (i = 0; i < lbl_804D4288; i++) {
            if (lbl_804D4280[i].flags & 2) {
                u8 r = (u8)(int)(255.0f * lbl_804D4280[i].color[0]);
                u8 g = (u8)(int)(255.0f * lbl_804D4280[i].color[1]);
                u8 b = (u8)(int)(255.0f * lbl_804D4280[i].color[2]);
                float rad = lbl_804D4280[i].radius;
                Vec3 v;
                Mtx34 m;
                rad = (rad > 100.0f) ? rad : 100.0f;
                DebugDrawSphere(r, g, b, 100, lbl_804D4280[i].sphere[0], lbl_804D4280[i].sphere[1],
                                lbl_804D4280[i].sphere[2], rad);
                fn_8016D864(&v, lbl_804D4280[i].pos);
                ActorGetMatrix(&m, &lbl_804D4280[i]);
                DebugDrawBoneBox(&v, &m, r, g, b);
            }
        }
    }
}

static inline void ActiveListRebuild() {
    int j;
    lbl_804D428C = 0;
    lbl_804D4294 = 0;
    lbl_804D4290 = 0;
    if (lbl_804D4280 != 0) {
        for (j = 0; j < lbl_804D4288; j++) {
            Actor* o = &lbl_804D4280[j];
            if ((o->flags & 2) && o->owner == 0) {
                lbl_803DE1D8[lbl_804D428C] = o;
                lbl_803DE1D8[lbl_804D428C]->fieldF68 = 0.0f;
                lbl_804D428C++;
            }
        }
    }
    lbl_804D4294 = lbl_804D428C;
}

// 0x80027690 (called from other units under this placeholder name)
extern "C" void fn_80027690(int id) {
    if (lbl_804D4280 != 0 && id != -1 && (int)((u32)id >> 24) == 0xC) {
        int idx = id & 0xFFFFFF;
        fn_8021284C(lbl_804D4280[idx].id);
        if (lbl_804D4280[idx].owner != 0)
            ActorUnlinkOwnerObjects(&lbl_804D4280[idx]);
        lbl_804D4280[idx].flags &= ~6;
        if (lbl_804D4280[idx].owner == 0)
            ActiveListRebuild();
    }
}

// 0x800277B0 / 0x800277E0
// TODO (not drafted yet, 0x278 bytes at 0x800277E0)

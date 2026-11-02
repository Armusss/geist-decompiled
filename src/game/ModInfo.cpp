// ModInfo.cpp: chunk/message handler tables, the global actor array (0x1C8-byte records),
// model node lookup and animation helpers (0x800265C8-0x8003B98C).
// Names are descriptive guesses; the code is matched against the retail DOL where noted
// in worker-notes.  Struct layouts are partial and derived from the retail asm.

#pragma auto_inline off

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

// Model database (lbl_804D4300) and its 0x48-byte node records; layouts are partial.
struct ModelNode {
    /*0x00*/ u8 unk00[0x18 - 0x00];
    /*0x18*/ u8 field18[0x34 - 0x18];
    /*0x34*/ u16 field34;
    /*0x36*/ u16 field36;
    /*0x38*/ u8 unk38[0x40 - 0x38];
    /*0x40*/ u16 boneCount;
    /*0x42*/ u16 field42;
    /*0x44*/ u8 unk44[0x46 - 0x44];
    /*0x46*/ u16 positionIndex;
};

struct ModelDb {
    /*0x00*/ u8 unk00[0x04 - 0x00];
    /*0x04*/ int count;
    /*0x08*/ u8 unk08[0x28 - 0x08];
    /*0x28*/ ModelNode* nodes;
    /*0x2C*/ u8 unk2C[0x34 - 0x2C];
    /*0x34*/ Vec3* positions;
    /*0x38*/ u8 unk38[0x44 - 0x38];
    /*0x44*/ s16* array44;
    /*0x48*/ s16* array48;
};

// Per-model draw slot (lbl_803E4998, 0x70 bytes each, 0x300 slots).
struct ModelSlot {
    /*0x00*/ u8 unk00[4];
    /*0x04*/ int field04;
    /*0x08*/ u8 color[4];
    /*0x0C*/ u8 unk0C[0x70 - 0x0C];
};

// Stream slot table (lbl_804D42E4, count lbl_804D42E8), 0x40 bytes per slot.
struct StreamEntry;
struct StreamSlot {
    /*0x00*/ u8 unk00[0x0C - 0x00];
    /*0x0C*/ void* buffer;
    /*0x10*/ u8 unk10[0x30 - 0x10];
    /*0x30*/ u16 state;
    /*0x32*/ u8 unk32[0x38 - 0x32];
    /*0x38*/ int size;
    /*0x3C*/ u8 unk3C[0x40 - 0x3C];
};

// One record of the global dynamic-light array (lbl_804D4280, count lbl_804D4288).
struct Actor {
    /*0x00*/ u8 unk00[0x24 - 0x00];
    /*0x24*/ s32 ipos[3];
    /*0x30*/ u8 unk30[0x38 - 0x30];
    /*0x38*/ u32 id;
    /*0x3C*/ u8 unk3C[0x4C - 0x3C];
    /*0x4C*/ u32 actorFlags;
    /*0x50*/ f32 pos[3];
    /*0x5C*/ u8 unk5C[0x68 - 0x5C];
    /*0x68*/ f32 fieldF68;
    /*0x6C*/ s32 owner;
    /*0x70*/ u32 flags;
    /*0x74*/ f32 field74;
    /*0x78*/ f32 field78;
    /*0x7C*/ u8 unk7C[0xB4 - 0x7C];
    /*0xB4*/ f32 sphere[3];
    /*0xC0*/ f32 dir[3];
    /*0xCC*/ f32 color[4];
    /*0xDC*/ u8 unkDC[0xF8 - 0xDC];
    /*0xF8*/ f32 fieldF8;
    /*0xFC*/ f32 radius;
    /*0x100*/ f32 field100;
    /*0x104*/ f32 field104;
    /*0x108*/ u8 unk108[0x1BC - 0x108];
    /*0x1BC*/ s32 field1BC;
    /*0x1C0*/ s32 field1C0;
    /*0x1C4*/ u8 unk1C4[0x1C8 - 0x1C4];
};

// Render record of one light (lbl_804D4284, 0x64 bytes each).
struct LightRender {
    /*0x00*/ u8 unk00[0x0C - 0x00];
    /*0x0C*/ f32 scaled[3];
    /*0x18*/ f32 color[4];
    /*0x28*/ u8 unk28[0x60 - 0x28];
    /*0x60*/ int unk60;
    /*0x64*/ u8 end[0];
};

typedef int (*MsgHandler)(int to, int msg, void* data, void* ctx);
typedef void (*ChunkHandler)(int type, void* data, int size, void* ctx);

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
extern LightRender* lbl_804D4284;
extern int lbl_804D4288;
extern int lbl_804D428C;
extern int lbl_804D4290;
extern int lbl_804D4294;
extern int lbl_804D429C;
extern Actor* lbl_803DE1D8[];
extern Actor* lbl_803DE3D8[];
extern int lbl_804D4298;
extern StreamSlot* lbl_804D42E4;
extern int lbl_804D42E8;
extern int lbl_804D42D4;
extern int lbl_804D42D8;
extern int lbl_804D2678;
extern int* lbl_804D42CC;
extern int lbl_804D42D0;
extern char* lbl_804D42BC;
extern int lbl_804D42C0;
extern int lbl_804D42A8;
extern int lbl_804D42AC;
extern int lbl_804D42B0;
extern int lbl_804D42B4;
extern int lbl_804D42C4;
extern int lbl_804D42DC;
extern int lbl_804D42E0;
extern int lbl_804D42EC;
extern int fn_80036688(void*, int, void*);
void fn_8002FC30(int, void*, void*);
extern ModelDb* lbl_804D4300;
extern ModelSlot lbl_803E4998[];

void fn_80007118(void*);
void fn_800070C0(void*);
void FileFlushBuffer(void* dst, int size);
void fn_8016D864(Vec3* out, void* in);
void fn_8021284C(u32);
void fn_80029F4C(Actor* a, void* pos);
void fn_80027158(Actor** arr, int lo, int hi);
void fn_8002828C(float dt, Actor* a, int b, int c);
void fn_80028A98(int index);
void fn_80029080(int msg, void* data);
void* VAlloc(int size);
void VFree(void* p);
extern "C" void fn_80026680(int, ChunkHandler, void*);
extern "C" void fn_80026660(int, MsgHandler, void*);
int fn_80112188(void* p);
}

typedef int (*FileHandlerFn)(void*, int, void*);
void StreamRead(int pri, StreamEntry* e, void (*done)(int, void*, void*), void* arg);
void StreamFree(void* p);
void GunStateTimerTick8017BAD0(void* p);
void GunStateStep8017BBBC(void* p);
void GunStateFinish8017BC5C(void* p);
void FileRemoveHandler(int type, FileHandlerFn fn, void* ctx);
void FileAddHandler(int type, FileHandlerFn fn, void* ctx);
void DebugDrawSphere(int r, int g, int b, int a, float x, float y, float z, float radius);
void DebugDrawBoneBox(Vec3* pos, void* mtx, int r, int g, int b);
void ActorGetMatrix(Mtx34* m, Actor* a);
void ActorUnlinkOwnerObjects(Actor* a);
float GfxLuma(float* color);

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
                    sChunkFn[t](t, (void*)e->data, e->size, sChunkCtx[t]);
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


// 0x8002756C (called from another unit under this placeholder name; ActorDebugDrawAll): debug draw of every flagged actor (sphere + bone box)
extern "C" void fn_8002756C() {
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

// 0x800277E0: update an existing light (called from other units under this placeholder name)
extern "C" void fn_800277E0(int id, float* pos, void* pos2, float* color, int* delay, float* extra) {
    int idx;
    if (lbl_804D4280 != 0 && (int)((u32)id >> 24) == 0xC) {
        idx = id & 0xFFFFFF;
        if (idx >= 0x20)
            return;
        {
            Actor* a = &lbl_804D4280[idx];
            if (pos != 0) {
                a->ipos[0] = (int)pos[0];
                a->ipos[1] = (int)pos[1];
                a->ipos[2] = (int)pos[2];
                a->flags |= 0x60000000;
            }
            if (pos2 != 0) {
                fn_80029F4C(a, pos2);
                a->flags |= 0x60000000;
            }
            if (color != 0) {
                a->color[0] = color[0];
                a->color[1] = color[1];
                a->color[2] = color[2];
                a->color[3] = color[3];
                a->flags |= 0x20000000;
            }
            if (delay != 0) {
                a->field104 = (float)*delay;
                a->field74 = (float)*delay;
                a->flags |= 0x60000000;
            }
            if (extra != 0) {
                a->field78 = *extra;
                a->flags |= 0x20000000;
            }
            {
                Actor* b = &lbl_804D4280[idx];
                if (b->owner == 0) {
                    LightRender* r = &lbl_804D4284[idx];
                    float luma = GfxLuma(b->color);
                    r->scaled[0] = b->dir[0] * luma;
                    r->scaled[1] = b->dir[1] * luma;
                    r->scaled[2] = b->dir[2] * luma;
                    lbl_804D4284[idx].color[0] = lbl_804D4280[idx].color[0];
                    lbl_804D4284[idx].color[1] = lbl_804D4280[idx].color[1];
                    lbl_804D4284[idx].color[2] = lbl_804D4280[idx].color[2];
                    lbl_804D4284[idx].color[3] = lbl_804D4280[idx].color[3];
                    ActiveListRebuild();
                    a->flags &= 0x9FFFFFFF;
                }
            }
        }
    }
}

// 0x800277B0 (called from other units under this placeholder name): move/recolour a light
extern "C" void fn_800277B0(int id, float* pos, float* color, int* delay) {
    fn_800277E0(id, pos, pos, color, delay, 0);
}

// 0x80027A8C: allocate a light slot (called from other units under this placeholder name)
extern "C" int fn_80027A8C(float spread, int owner, float* pos, void* pos2, float* color, int delay, u32 flags);

// 0x80027A58 (called from other units under this placeholder name)
extern "C" int fn_80027A58(int owner, float* pos, float* color, int delay, int flags) {
    return fn_80027A8C(180.0f, owner, pos, pos, color, delay, flags);
}

extern "C" int fn_80027A8C(float spread, int owner, float* pos, void* pos2, float* color, int delay, u32 flags) {
    int i;
    if (lbl_804D4280 != 0) {
        for (i = 0; i < 0x20; i++) {
            if ((lbl_804D4280[i].flags & 4) == 0) {
                lbl_804D4280[i].flags = flags | 6;
                lbl_804D4280[i].owner = owner;
                lbl_804D4280[i].color[0] = color[0];
                lbl_804D4280[i].color[1] = color[1];
                lbl_804D4280[i].color[2] = color[2];
                lbl_804D4280[i].color[3] = color[3];
                lbl_804D4280[i].field74 = (float)delay;
                lbl_804D4280[i].field78 = spread;
                lbl_804D4280[i].field1BC = 0;
                lbl_804D4280[i].field1C0 = 0;
                lbl_804D4280[i].fieldF8 = 0.0f;
                lbl_804D4280[i].radius = 0.0f;
                lbl_804D4280[i].field100 = 0.0f;
                lbl_804D4280[i].field104 = (float)delay;
                lbl_804D4280[i].ipos[0] = (int)pos[0];
                lbl_804D4280[i].ipos[1] = (int)pos[1];
                lbl_804D4280[i].ipos[2] = (int)pos[2];
                fn_80029F4C(&lbl_804D4280[i], pos2);
                fn_80028A98(i);
                if (lbl_804D4280[i].owner == 0)
                    ActiveListRebuild();
                return (i & 0xFFFFFF) | 0xC000000;
            }
        }
    }
    return -1;
}

// 0x80027FD8: after a frame, drop the 0x80000000 flag of the lights queued this frame
void LightsClearPending() {
    int i;
    for (i = lbl_804D4294; i < lbl_804D428C; i++)
        lbl_803DE1D8[i]->flags &= 0x7FFFFFFF;
    lbl_804D428C = lbl_804D4294;
    for (i = 0; i < lbl_804D4290; i++)
        lbl_803DE3D8[i]->flags &= 0x7FFFFFFF;
    lbl_804D4290 = 0;
}


void LightsLoad(int type, void* data, int size, void* ctx);
void LightMsg(int id, int msg, void* data, void* ctx);
int LightMsgNone(int id, int msg, void* data, void* ctx);

// 0x80028140: init the light system and register its message/chunk handlers
int LightsInit() {
    lbl_804D4280 = 0;
    lbl_804D4288 = 0;
    fn_80026680(0xC, LightsLoad, 0);
    fn_80026660(0xC, (MsgHandler)LightMsg, 0);
    fn_80026660(0x3A, LightMsgNone, 0);
    lbl_804D4298 = 0;
    return 1;
}


// 0x800289F8: free the light arrays
void LightsShutdown() {
    lbl_804D4280 = 0;
    lbl_804D4288 = 0;
    if (lbl_804D4284 != 0) {
        VFree(lbl_804D4284);
        lbl_804D4284 = 0;
    }
}

// 0x80028A38: send message 0x800 to every light
void LightsBroadcast() {
    int i;
    int data[2];
    for (i = 0; i < lbl_804D4288; i++) {
        data[1] = 0;
        SendMsg((i & 0xFFFFFF) | 0xC000000, 0x800, data);
    }
}


// 0x80028F2C: chunk loader for type 0xF: the light array
void LightsLoad(int type, void* data, int size, void* ctx) {
    char* base = (char*)data;
    void* hdr = (size > 0) ? data : 0;
    FileFlushBuffer(hdr, 0x10);
    if (*(int*)hdr == 0xF) {
        int i;
        lbl_804D4288 = ((int*)hdr)[1];
        lbl_804D4280 = (Actor*)(Fits(size - 0x10, lbl_804D4288 * 0x1C8) ? base + 0x10 : 0);
        lbl_804D4284 = (LightRender*)VAlloc(lbl_804D4288 * 0x64);
        for (i = 0; i < lbl_804D4288; i++) {
            FileFlushBuffer(&lbl_804D4280[i], 0x50);
            FileFlushBuffer((char*)&lbl_804D4280[i] + 0x50, 0x178);
            lbl_804D4284[i].unk60 = fn_80112188(&lbl_804D4284[i].unk28[0]);
        }
    }
}

// 0x80029040
int LightMsgNone(int id, int msg, void* data, void* ctx) {
    return 0;
}

// 0x80029048: light message handler: fetch the light record, then process the message
void LightMsg(int id, int msg, void* data, void* ctx) {
    *(Actor**)data = &lbl_804D4280[id];
    fn_80029080(msg, data);
}


// 0x80037B6C (called from other units under this placeholder name)
extern "C" s16* fn_80037B6C(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return lbl_804D4300->array48 + lbl_804D4300->nodes[idx].field42;
    return 0;
}

// 0x80037BAC (called from another unit under this placeholder name; ModelArray44)
extern "C" s16* fn_80037BAC(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return lbl_804D4300->array44 + lbl_804D4300->nodes[idx].field36;
    return 0;
}

// 0x80037BEC
int ModelBoneCount(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return *(u16*)((u8*)lbl_804D4300->nodes + idx * 0x48 + 0x40);
    return -1;
}

// 0x80037C20 (called from other units under this placeholder name)
extern "C" int fn_80037C20(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return *(u16*)((u8*)lbl_804D4300->nodes + idx * 0x48 + 0x34);
    return -1;
}

// 0x800381F8 (called from other units under this placeholder name)
extern "C" void fn_800381F8(u32 handle, int value, u8 r, u8 g, u8 b, u8 a) {
    int idx = handle >> 16;
    if (idx > 0 && idx < 0x300) {
        lbl_803E4998[idx].field04 = value;
        lbl_803E4998[idx].color[0] = r;
        lbl_803E4998[idx].color[1] = g;
        lbl_803E4998[idx].color[2] = b;
        lbl_803E4998[idx].color[3] = a;
    }
}

// 0x800390D8 (called from other units under this placeholder name)
extern "C" int fn_800390D8(int handle, int index) {
    if (index < 0)
        return -1;
    return (handle & 0xFFFF0000) | (index & 0xFFFF);
}

// 0x8003A14C
Vec3* ModelBonePositions(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return &lbl_804D4300->positions[*(u16*)((u8*)lbl_804D4300->nodes + idx * 0x48 + 0x46)];
    return 0;
}

// 0x8003A18C (called from other units under this placeholder name)
extern "C" void* fn_8003A18C(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return lbl_804D4300->nodes[idx].field18;
    return 0;
}

// 0x8003A1C0 (called from other units under this placeholder name)
extern "C" ModelNode* fn_8003A1C0(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return (ModelNode*)((u8*)lbl_804D4300->nodes + idx * 0x48);
    return 0;
}


// 0x8002FA34
bool StreamSlotIdle(int index) {
    if (index >= 0 && index < lbl_804D42E8) {
        StreamSlot* s = &lbl_804D42E4[index];
        bool r = false;
        if (s->buffer != 0 || s->size == 0)
            r = true;
        return r;
    }
    return false;
}

// 0x8002FA84 (called from another unit under this placeholder name): total bytes of all slots, and of the slots in state 1 or 4
extern "C" void fn_8002FA84(int* loaded, int* total) {
    StreamSlot* s = lbl_804D42E4;
    int a = 0;
    int b = 0;
    int i;
    for (i = 0; i < lbl_804D42E8; i++) {
        b += s->size;
        if (s->state == 1 || s->state == 4)
            a += s->size;
        s++;
    }
    if (loaded != 0)
        *loaded = a;
    if (total != 0)
        *total = b;
}

// 0x8002FAE4: start reading every slot that is waiting (state 2)
void StreamSlotsStartPending() {
    if (lbl_804D42D4 != 0 && lbl_804D2678 != 0) {
        int i;
        lbl_804D42D4 = 0;
        for (i = 0; i < lbl_804D42E8; i++) {
            StreamSlot* s = &lbl_804D42E4[i];
            if (s->state == 2) {
                s->state = 1;
                StreamRead(0x2E, (StreamEntry*)&s->state, fn_8002FC30, s);
            }
        }
    }
}

// 0x8002FB90: release finished slots (state 3)
void StreamSlotsRelease() {
    if (lbl_804D42D8 != 0 && lbl_804D2678 != 0) {
        int i;
        lbl_804D42D8 = 0;
        for (i = 0; i < lbl_804D42E8; i++) {
            StreamSlot* s = &lbl_804D42E4[i];
            if (s->state == 3) {
                if (s->buffer != 0) {
                    s->state = 0;
                    StreamFree(s->buffer);
                    s->buffer = 0;
                } else {
                    lbl_804D42D8 = 1;
                }
            }
        }
    }
}

// TODO (not drafted yet, 0x138 bytes at 0x8002FC30): stream slot completion callback (declared above)

// 0x8002FD68 (called from another unit under this placeholder name)
extern "C" void fn_8002FD68() {
    int i;
    lbl_804D42D8 = 1;
    for (i = 0; i < lbl_804D42E8; i++)
        GunStateFinish8017BC5C(&lbl_804D42E4[i].state);
}

// 0x8002FDCC
void StreamSlotTick(int index) {
    if (index >= 0 && index < lbl_804D42E8) {
        lbl_804D42D8 = 1;
        GunStateTimerTick8017BAD0(&lbl_804D42E4[index].state);
    }
}

// 0x8002FE18
void StreamSlotStep(int index) {
    if (index >= 0 && index < lbl_804D42E8) {
        lbl_804D42D4 = 1;
        GunStateStep8017BBBC(&lbl_804D42E4[index].state);
    }
}

// 0x800270AC: depth-sort an array of light pointers (quick sort pass, then insertion sort)
void LightsSort(Actor** arr, int n) {
    int i;
    int j;
    Actor* x;
    float key;
    n--;
    fn_80027158(arr, 0, n);
    for (i = 1; i <= n; i++) {
        x = arr[i];
        key = x->fieldF68;
        for (j = i; j > 0 && arr[j - 1]->fieldF68 > key; j--)
            arr[j] = arr[j - 1];
        arr[j] = x;
    }
}

// 0x80028060: recompute all lights and rebuild the active list
int LightsRefresh() {
    if (lbl_804D4280 != 0) {
        int i;
        for (i = 0; i < lbl_804D4288; i++)
            fn_80028A98(i);
        ActiveListRebuild();
    }
    return 1;
}

static inline bool ActorShouldUpdate(Actor* a) {
    bool r = false;
    if (a != 0) {
        if ((a->actorFlags & 0x40) || (a->actorFlags & 1) || (int)(a->id >> 24) == 0xC)
            r = true;
    }
    return r;
}

// 0x800281B0: per-frame update of every actor
int ActorsUpdate(float dt) {
    int i;
    for (i = 0; i < lbl_804D4288; i++) {
        Actor* a = &lbl_804D4280[i];
        if (a->actorFlags & 0x400) {
            a->actorFlags &= ~0x400;
        } else {
            if (ActorShouldUpdate(a))
                fn_8002828C(dt, a, 0, 0);
        }
    }
    lbl_804D4298 += (int)dt;
    return 1;
}

// 0x80031E48 (called from other units under this placeholder name): do two integer boxes overlap?
// Each box is {min x, y, z, max x, y, z}.
static inline int Abs(int x) {
    return x < 0 ? -x : x;
}

extern "C" int fn_80031E48(int* a, int* b) {
    int dx = (b[3] + b[0]) / 2 - (a[3] + a[0]) / 2;
    int dy = (b[4] + b[1]) / 2 - (a[4] + a[1]) / 2;
    int dz = (b[5] + b[2]) / 2 - (a[5] + a[2]) / 2;
    int hx = (a[3] - a[0]) / 2 + (b[3] - b[0]) / 2;
    int hy = (a[4] - a[1]) / 2 + (b[4] - b[1]) / 2;
    int hz = (a[5] - a[2]) / 2 + (b[5] - b[2]) / 2;
    if (hx < Abs(dx) || hy < Abs(dy) || hz < Abs(dz))
        return 0;
    return 1;
}

// 0x80031FAC (called from another unit under this placeholder name; Chunk6Load): file chunk handler for type 6 (array of 0x68-byte records)
extern "C" int fn_80031FAC(void* data, int size, void* ctx) {
    int* p = (size > 0) ? (int*)data : 0;
    if (*p == 6) {
        lbl_804D42D0 = p[1];
        lbl_804D42CC = Fits(size - 0x20, p[1] * 0x68) ? (int*)data + 8 : 0;
    }
    return 1;
}

// 0x80032014 (called from another unit under this placeholder name; ChunkNodesLoad): file chunk handler (0x48-byte records followed by 0x20-byte records)
extern "C" int fn_80032014(void* data, int size, void* ctx) {
    char* h = (size > 0) ? (char*)data : 0;
    int n = *(int*)(h + 4);
    char* p;
    int bytes;
    int pad;
    bytes = n * 0x48;
    pad = ((bytes + 0x1F) & 0xFFFFFFE0) - bytes;
    p = (char*)data + 0x20;
    p += bytes;
    p += pad;
    lbl_804D42BC = Fits(size - 0x20 - bytes - pad, *(int*)(h + 8) << 5) ? p : 0;
    lbl_804D42C0 = *(int*)(h + 8);
    return 1;
}

// 0x80032094 (called from another unit under this placeholder name; DebugOptionSet): debug option 0: value -1 toggles, 0/1 sets; returns the new value
extern "C" int fn_80032094(int which, int value) {
    int r = -1;
    switch (which) {
    case 0:
        switch (value) {
        case -1:
            r = (u32)__cntlzw(lbl_804D42A8) >> 5;
            lbl_804D42A8 = r;
            break;
        case 0:
        case 1:
            r = value;
            lbl_804D42A8 = value;
            break;
        default:
            r = lbl_804D42A8;
            break;
        }
        break;
    case 1:
        break;
    }
    return r;
}

static inline void SlotsRelease() {
    if (lbl_804D42D8 != 0 && lbl_804D2678 != 0) {
        int i;
        lbl_804D42D8 = 0;
        for (i = 0; i < lbl_804D42E8; i++) {
            StreamSlot* s = &lbl_804D42E4[i];
            if (s->state == 3) {
                if (s->buffer != 0) {
                    s->state = 0;
                    StreamFree(s->buffer);
                    s->buffer = 0;
                } else {
                    lbl_804D42D8 = 1;
                }
            }
        }
    }
}


static inline void SlotsFinishAll() {
    int i;
    lbl_804D42D8 = 1;
    for (i = 0; i < lbl_804D42E8; i++)
        GunStateFinish8017BC5C(&lbl_804D42E4[i].state);
}

// 0x800320F4 (called from another unit under this placeholder name; StreamSlotsShutdown)
extern "C" void fn_800320F4() {
    SlotsFinishAll();
    SlotsRelease();
    lbl_804D42AC = 0;
    lbl_804D42B0 = 0;
    lbl_804D42B4 = 0;
    lbl_804D42BC = 0;
    lbl_804D42C4 = 0;
    lbl_804D42CC = 0;
    lbl_804D42E0 = 0;
    lbl_804D42E4 = 0;
    lbl_804D42E8 = 0;
    lbl_804D42EC = 0;
    lbl_804D42D4 = 0;
    lbl_804D42D8 = 0;
}

// 0x80032200 (called from another unit under this placeholder name; StreamSlotsInit)
extern "C" int fn_80032200() {
    lbl_804D42AC = 0;
    lbl_804D42B0 = 0;
    lbl_804D42B4 = 0;
    lbl_804D42BC = 0;
    lbl_804D42C4 = 0;
    lbl_804D42CC = 0;
    lbl_804D42E0 = 0;
    lbl_804D42E4 = 0;
    lbl_804D42E8 = 0;
    lbl_804D42EC = 0;
    lbl_804D42DC = 0;
    lbl_804D42D4 = 0;
    lbl_804D42D8 = 0;
    FileAddHandler(0x2E, fn_80036688, 0);
    return 1;
}

// ---- Functions not drafted yet (declarations only; retail assembly is used for them) ----
// TODO (not drafted yet, 0x55C bytes at 0x80026B50): ModInfo function
extern "C" void fn_80026B50();
// TODO (not drafted yet, 0xAC bytes at 0x800270AC): ModInfo function
extern "C" void fn_800270AC();
// TODO (not drafted yet, 0x3F8 bytes at 0x80027158): ModInfo function
// TODO (not drafted yet, 0x300 bytes at 0x80027CD8): ModInfo function
extern "C" void fn_80027CD8();
// TODO (not drafted yet, 0xE0 bytes at 0x80028060): ModInfo function
extern "C" void fn_80028060();
// TODO (not drafted yet, 0xDC bytes at 0x800281B0): ModInfo function
extern "C" void fn_800281B0();
// TODO (not drafted yet, 0x76C bytes at 0x8002828C): ModInfo function
// TODO (not drafted yet, 0x1D4 bytes at 0x800298B4): ModInfo function
extern "C" void fn_800298B4();
// TODO (not drafted yet, 0x4C4 bytes at 0x80029A88): ModInfo function
extern "C" void fn_80029A88();
// TODO (not drafted yet, 0x850 bytes at 0x8002A274): ModInfo function
extern "C" void fn_8002A274();
// TODO (not drafted yet, 0x28C bytes at 0x8002AAC4): ModInfo function
extern "C" void fn_8002AAC4();
// TODO (not drafted yet, 0x3CC bytes at 0x8002AD50): ModInfo function
extern "C" void fn_8002AD50();
// TODO (not drafted yet, 0x644 bytes at 0x8002B11C): ModInfo function
extern "C" void fn_8002B11C();
// TODO (not drafted yet, 0x8B4 bytes at 0x8002B760): ModInfo function
extern "C" void fn_8002B760();
// TODO (not drafted yet, 0xBD0 bytes at 0x8002C014): ModInfo function
extern "C" void fn_8002C014();
// TODO (not drafted yet, 0x178 bytes at 0x8002CBE4): ModInfo function
extern "C" void fn_8002CBE4();
// TODO (not drafted yet, 0x3C0 bytes at 0x8002CD5C): ModInfo function
extern "C" void fn_8002CD5C();
// TODO (not drafted yet, 0x3C4 bytes at 0x8002D11C): ModInfo function
extern "C" void fn_8002D11C();
// TODO (not drafted yet, 0x644 bytes at 0x8002D4E0): ModInfo function
extern "C" void fn_8002D4E0();
// TODO (not drafted yet, 0x914 bytes at 0x8002DB24): ModInfo function
extern "C" void fn_8002DB24();
// TODO (not drafted yet, 0xCD0 bytes at 0x8002E438): ModInfo function
extern "C" void fn_8002E438();
// TODO (not drafted yet, 0x2F0 bytes at 0x8002F108): ModInfo function
extern "C" void fn_8002F108();
// TODO (not drafted yet, 0x438 bytes at 0x8002F3F8): ModInfo function
extern "C" void fn_8002F3F8();
// TODO (not drafted yet, 0x204 bytes at 0x8002F830): ModInfo function
extern "C" void fn_8002F830();
// TODO (not drafted yet, 0x1450 bytes at 0x8002FE64): ModInfo function
extern "C" void fn_8002FE64();
// TODO (not drafted yet, 0x498 bytes at 0x800312B4): ModInfo function
extern "C" void fn_800312B4();
// TODO (not drafted yet, 0x6FC bytes at 0x8003174C): ModInfo function
extern "C" void fn_8003174C();
// TODO (not drafted yet, 0x210 bytes at 0x8003226C): ModInfo function
extern "C" void fn_8003226C();
// TODO (not drafted yet, 0x8B8 bytes at 0x8003247C): ModInfo function
extern "C" void fn_8003247C();
// TODO (not drafted yet, 0x120 bytes at 0x80032D34): ModInfo function
extern "C" void fn_80032D34();
// TODO (not drafted yet, 0x180 bytes at 0x80032E54): ModInfo function
extern "C" void fn_80032E54();
// TODO (not drafted yet, 0x224 bytes at 0x80032FD4): ModInfo function
extern "C" void fn_80032FD4();
// TODO (not drafted yet, 0x1E8 bytes at 0x800331F8): ModInfo function
extern "C" void fn_800331F8();
// TODO (not drafted yet, 0x70 bytes at 0x800333E0): ModInfo function
extern "C" void fn_800333E0();
// TODO (not drafted yet, 0x54 bytes at 0x80033450): ModInfo function
extern "C" void fn_80033450();
// TODO (not drafted yet, 0x314 bytes at 0x800334A4): ModInfo function
extern "C" void fn_800334A4();
// TODO (not drafted yet, 0x328 bytes at 0x800337B8): ModInfo function
extern "C" void fn_800337B8();
// TODO (not drafted yet, 0x654 bytes at 0x80033AE0): ModInfo function
extern "C" void fn_80033AE0();
// TODO (not drafted yet, 0x488 bytes at 0x80034134): ModInfo function
extern "C" void fn_80034134();
// TODO (not drafted yet, 0x1BC bytes at 0x800345BC): ModInfo function
extern "C" void fn_800345BC();
// TODO (not drafted yet, 0x6B8 bytes at 0x80034778): ModInfo function
extern "C" void fn_80034778();
// TODO (not drafted yet, 0x144 bytes at 0x80034E30): ModInfo function
extern "C" void fn_80034E30();
// TODO (not drafted yet, 0x1A8 bytes at 0x80034F74): ModInfo function
extern "C" void fn_80034F74();
// TODO (not drafted yet, 0x214 bytes at 0x8003511C): ModInfo function
extern "C" void fn_8003511C();
// TODO (not drafted yet, 0xC8 bytes at 0x80035330): ModInfo function
extern "C" void fn_80035330();
// TODO (not drafted yet, 0x3E8 bytes at 0x800353F8): ModInfo function
extern "C" void fn_800353F8();
// TODO (not drafted yet, 0x2A4 bytes at 0x800357E0): ModInfo function
extern "C" void fn_800357E0();
// TODO (not drafted yet, 0x59C bytes at 0x80035A84): ModInfo function
extern "C" void fn_80035A84();
// TODO (not drafted yet, 0x1E0 bytes at 0x80036020): ModInfo function
extern "C" void fn_80036020();
// TODO (not drafted yet, 0xE0 bytes at 0x80036200): ModInfo function
extern "C" void fn_80036200();
// TODO (not drafted yet, 0x36C bytes at 0x800362E0): ModInfo function
extern "C" void fn_800362E0();
// TODO (not drafted yet, 0x3C bytes at 0x8003664C): ModInfo function
extern "C" void fn_8003664C();
// TODO (not drafted yet, 0x244 bytes at 0x80036688): ModInfo function
// TODO (not drafted yet, 0x138 bytes at 0x800368CC): ModInfo function
extern "C" void fn_800368CC();
// TODO (not drafted yet, 0x1EC bytes at 0x80036A04): ModInfo function
extern "C" void fn_80036A04();
// TODO (not drafted yet, 0x1D4 bytes at 0x80036BF0): ModInfo function
extern "C" void fn_80036BF0();
// TODO (not drafted yet, 0x46C bytes at 0x80036DC4): ModInfo function
extern "C" void fn_80036DC4();
// TODO (not drafted yet, 0x234 bytes at 0x80037230): ModInfo function
extern "C" void fn_80037230();
// TODO (not drafted yet, 0x240 bytes at 0x800376F0): ModInfo function
extern "C" void fn_800376F0();
// TODO (not drafted yet, 0x23C bytes at 0x80037930): ModInfo function
extern "C" void fn_80037930();
// TODO (not drafted yet, 0x26C bytes at 0x80037C54): ModInfo function
extern "C" void fn_80037C54();
// TODO (not drafted yet, 0x2DC bytes at 0x80037EC0): ModInfo function
extern "C" void fn_80037EC0();
// TODO (not drafted yet, 0x5C bytes at 0x8003819C): ModInfo function
extern "C" void fn_8003819C();
// TODO (not drafted yet, 0x244 bytes at 0x80038230): ModInfo function
extern "C" void fn_80038230();
// TODO (not drafted yet, 0x430 bytes at 0x80038474): ModInfo function
extern "C" void fn_80038474();
// TODO (not drafted yet, 0x244 bytes at 0x800388A4): ModInfo function
extern "C" void fn_800388A4();
// TODO (not drafted yet, 0xE8 bytes at 0x800390F4): ModInfo function
extern "C" void fn_800390F4();
// TODO (not drafted yet, 0x29C bytes at 0x800391DC): ModInfo function
extern "C" void fn_800391DC();
// TODO (not drafted yet, 0x1C0 bytes at 0x80039478): ModInfo function
extern "C" void fn_80039478();
// TODO (not drafted yet, 0x1C0 bytes at 0x80039638): ModInfo function
extern "C" void fn_80039638();
// TODO (not drafted yet, 0xB0 bytes at 0x800397F8): ModInfo function
extern "C" void fn_800397F8();
// TODO (not drafted yet, 0xB0 bytes at 0x800398A8): ModInfo function
extern "C" void fn_800398A8();
// TODO (not drafted yet, 0x7F4 bytes at 0x80039958): ModInfo function
extern "C" void fn_80039958();
// TODO (not drafted yet, 0x200 bytes at 0x8003A1F0): ModInfo function
extern "C" void fn_8003A1F0();
// TODO (not drafted yet, 0x27C bytes at 0x8003A3F0): ModInfo function
extern "C" void fn_8003A3F0();
// TODO (not drafted yet, 0x80 bytes at 0x8003ADD4): ModInfo function
extern "C" void fn_8003ADD4();
// TODO (not drafted yet, 0x4B8 bytes at 0x8003AE54): ModInfo function
extern "C" void fn_8003AE54();
// TODO (not drafted yet, 0xF4 bytes at 0x8003B30C): ModInfo function
extern "C" void fn_8003B30C();
// TODO (not drafted yet, 0xA4 bytes at 0x8003B400): ModInfo function
extern "C" void fn_8003B400();
// TODO (not drafted yet, 0x94 bytes at 0x8003B4A4): ModInfo function
extern "C" void fn_8003B4A4();
// TODO (not drafted yet, 0x454 bytes at 0x8003B538): ModInfo function
extern "C" void fn_8003B538();

// TODO (not drafted yet, 0x28C bytes at 0x80037464): find a node of a model by name hash
int ModelNodeFind(int model, int hash);
// TODO (not drafted yet, 0x5F0 bytes at 0x80038AE8): set up a model's node
int ModelNodeSetup(int a, int b, int c, int d);
// TODO (not drafted yet, 0x430 bytes at 0x8003A66C): set a model's matrix
void ModelSetMatrix(int model, Mtx34* m, char* name);
// TODO (not drafted yet, 0x338 bytes at 0x8003AA9C): animate a model
void ModelAnimate(int model, char* name, int frame);

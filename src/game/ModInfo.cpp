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
    /*0x3C*/ u8 unk3C[0x50 - 0x3C];
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
void fn_8002FC30(int, void*, void*);
extern ModelDb* lbl_804D4300;
extern ModelSlot lbl_803E4998[];

void fn_80007118(void*);
void fn_800070C0(void*);
void FileFlushBuffer(void* dst, int size);
void fn_8016D864(Vec3* out, void* in);
void fn_8021284C(u32);
void fn_80029F4C(Actor* a, void* pos);
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


// TODO (not drafted yet, 0x300 bytes at 0x80027CD8)
// TODO (not drafted yet, 0x88 bytes at 0x80027FD8 is below)

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

// TODO (not drafted yet, 0xE0 bytes at 0x80028060)

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

// TODO (not drafted yet, 0x70 bytes at 0x800281B0 and 0x76C bytes at 0x8002828C)

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

// TODO (not drafted yet, 0x494 bytes at 0x80028A98)

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

// TODO (not drafted yet, 0x234 bytes at 0x80037230 .. 0x240 bytes at 0x800376F0 and others between 0x80029080 and 0x80037B6C)

// 0x80037B6C (called from other units under this placeholder name)
extern "C" s16* fn_80037B6C(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return lbl_804D4300->array48 + lbl_804D4300->nodes[idx].field42;
    return 0;
}

// 0x80037BAC
s16* ModelArray44(int model) {
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

// TODO (not drafted yet, 0x1450 bytes at 0x8002FE64 and others between 0x80029080 and 0x8002FA34)

// 0x8002FA34
bool StreamSlotIdle(int index) {
    if (index >= 0 && index < lbl_804D42E8) {
        bool r = false;
        StreamSlot* s = &lbl_804D42E4[index];
        if (s->buffer != 0 || s->size == 0)
            r = true;
        return r;
    }
    return false;
}

// 0x8002FA84 (called from another unit under this placeholder name): total bytes of all slots, and of the slots in state 1 or 4
extern "C" void fn_8002FA84(int* loaded, int* total) {
    int a = 0;
    int b = 0;
    StreamSlot* s = lbl_804D42E4;
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

// TODO (not drafted yet, 0x138 bytes at 0x8002FC30)

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

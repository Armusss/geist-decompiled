// u_801D8134.cpp: 0x801D8134-0x801D8768, 8 functions. App-system hooks (called from AppSystemsInit/Start/Shutdown).
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   .sbss 0x804D51D0-0x804D51D8, then a 4-byte pad at 0x804D51DC: small-data blocks are padded to 8 at
//   each file end (0x804D5194 after AsHeavy, 0x804D51CC after u_801D4738), so the next block is a new file
//   .sdata2 0x804D8C78 (its only constant)
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

#include "game/types.h"

// One record per placed prop in the level's chunk 8 (0x40 bytes, byte-swapped in place on load).
struct PropRec {
    Vec3 pos;                                  // 0x00: world position (converted to fixed point)
    Vec3 euler;                                // 0x0C
    Vec3 offA;                                 // 0x18
    Vec3 offB;                                 // 0x24: offA + offB, scaled by 1/32768, is the local position
    unsigned long asset;                       // 0x30
    int texture;                               // 0x34
    u8 pad38[8];                               // 0x38
};

// The chunk: a 0x10-byte header, then the records.
struct PropChunk {
    int version;                               // 0x00: 2
    int count;                                 // 0x04
    u8 pad08[8];                               // 0x08
};

struct Actor;

// The runtime object of a prop (0x90 bytes, an array of them is VAlloc'd).
struct PropObj {
    Placement place;                           // 0x00
    int (*handler)(int msg, void* data);       // 0x30
    u8 pad34[4];                               // 0x34
    unsigned long handle;                      // 0x38: kind 8, low 24 bits = index
    u8 pad3C[0x14];                            // 0x3C
    PlaceF local;                              // 0x50
    int visible;                               // 0x80
    u8 pad84[0xC];                             // 0x84
};

struct PropMsg {
    PropObj* obj;                              // 0x00
    int arg;                                   // 0x04
    int pad08;                                 // 0x08: the 0x800 notify frame needs 0x0C-0x14 bytes
};

extern "C" {
void FileFlushBuffer(void* buf, int bytes);    // 0x80007088 (Cube)
void fn_800070C0(void* p);                     // 0x800070C0 (Cube)
void fn_8000F550(short* ids, int n);           // 0x8000F550
int fn_80026660(int kind, void* cb, int arg);  // 0x80026660: register a chunk lookup callback
int fn_80026680(int kind, void* cb, int arg);  // 0x80026680: register a chunk load callback
int fn_800C5210(unsigned long handle, int msg, void* data);
int fn_80084540(unsigned long handle, int msg, void* data, void* out);
}
void* VAlloc(int size);
void VFree(void* p);
int SaveIsBusy(void);
unsigned long AssetFind(unsigned long type, unsigned long id);
void TextureLoad(short* ids, int n);
void MtxSetEuler(Mtx34* m, Vec3* rot);
void PlacementToMatrix(Mtx34* m, PlaceF* place);
int ActorFn8002378C(Actor* a, int id, int a2, int a3, int a4, int a5, int a6);
int ActorFn80022D98(Actor* a, int arg);

static PropRec* sRecs;                         // 0x804D51D0
static PropObj* sObjs;                         // 0x804D51D4
static int sCount;                             // 0x804D51D8

extern "C" int fn_801D85F8(int msg, void* data);

extern "C" void fn_801D8134(void) {  // PropsShutdown
    if (sObjs != 0) {
        VFree(sObjs);
    }
}

extern "C" int fn_801D8160(void) {
    return 1;
}

extern "C" void fn_801D8168(void) {  // PropsNotify: 0x800, or 0x803 while saving
    PropMsg m;
    int i;
    int msg;

    if (SaveIsBusy() != 0) {
        msg = 0x803;
    } else {
        msg = 0x800;
    }
    for (i = 0; i < sCount; i++) {
        PropObj* o = &sObjs[i];
        m.obj = o;
        o->handler(msg, &m);
    }
}

static inline void PropSpawn(int i) {
    PropObj* o = &sObjs[i];
    PropRec* r = &sRecs[i];
    o->visible = 1;
    MtxSetEuler((Mtx34*)&o->local, &r->euler);
    o->local.pos.x = r->offB.x + r->offA.x;
    o->local.pos.y = r->offB.y + r->offA.y;
    o->local.pos.z = r->offB.z + r->offA.z;
    o->local.pos.x *= 3.0517578125e-05f;
    o->local.pos.y *= 3.0517578125e-05f;
    o->local.pos.z *= 3.0517578125e-05f;
    PlacementToMatrix((Mtx34*)o, &o->local);
    o->place.pos.x = (int)r->pos.x;
    o->place.pos.y = (int)r->pos.y;
    o->place.pos.z = (int)r->pos.z;
    ActorFn8002378C((Actor*)o, (i & 0xFFFFFF) | 0x8000000, (int)o, (int)fn_801D85F8, 0, 0, 0);
    ActorFn80022D98((Actor*)o, 0);
}

extern "C" int fn_801D81F4(void) {  // PropsSpawn
    int i;

    if (sCount != 0) {
        sObjs = (PropObj*)VAlloc(sCount * sizeof(PropObj));
    }
    for (i = 0; i < sCount; i++) {
        PropSpawn(i);
    }
    return 1;
}

extern "C" void fn_801D8538(int kind, void* data);
extern "C" int fn_801D83C0(int index, int msg, PropMsg* m);

extern "C" int fn_801D8368(void) {  // PropsStart
    sRecs = 0;
    sObjs = 0;
    sCount = 0;
    fn_80026680(8, fn_801D8538, 0);
    fn_80026660(8, fn_801D83C0, 0);
    return 1;
}

static inline int PropHandle(int msg, PropMsg* m) {
    PropObj* o = m->obj;
    short tex;
    int out;

    if (fn_800C5210(o->handle, msg, m) == 0 && fn_80084540(o->handle, msg, m, &out) == 0) {
        switch (msg) {
        case 0xC00:
            sObjs[m->obj->handle & 0xFFFFFF].visible = m->arg;
            return 1;
        case 0xC01:
            return sObjs[m->obj->handle & 0xFFFFFF].visible;
        case 0x41A:
            m->arg = AssetFind(-1, sRecs[m->obj->handle & 0xFFFFFF].asset);
            return 1;
        case 6:
            tex = sRecs[m->obj->handle & 0xFFFFFF].texture;
            if (m->arg != 0) {
                TextureLoad(&tex, 1);
            } else {
                fn_8000F550(&tex, 1);
            }
            return 1;
        }
        return 0;
    }
    return 1;
}

extern "C" int fn_801D83C0(int index, int msg, PropMsg* m) {  // PropsLookup
    m->obj = &sObjs[index];
    return PropHandle(msg, m);
}

extern "C" void fn_801D8538(int kind, void* data) {  // PropsLoad
    PropChunk* hdr = (PropChunk*)data;
    int i;

    FileFlushBuffer(hdr, sizeof(PropChunk));
    if (hdr->version == 2) {
        sRecs = (PropRec*)((u8*)data + sizeof(PropChunk));
        sCount = hdr->count;
        for (i = 0; i < sCount; i++) {
            PropRec* r = &sRecs[i];
            FileFlushBuffer(&r->offA, 0x18);
            FileFlushBuffer(&r->euler, 0xC);
            FileFlushBuffer(&r->pos, 0xC);
            fn_800070C0(&r->texture);
            fn_800070C0(&r->asset);
        }
    }
}

extern "C" int fn_801D85F8(int msg, void* data) {  // PropMsgHandler
    return PropHandle(msg, (PropMsg*)data);
}

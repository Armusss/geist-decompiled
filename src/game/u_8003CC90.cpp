// u_8003CC90.cpp: model table (0x8003CC90-0x800448D0).
// The model counterpart of the texture table in u_8000F2F0.cpp: entries of 0x68 bytes with a list
// of texture ids and a disc streaming slot, load / free / prefetch by id, plus the larger model
// loading, bone and instance code that follows.  Only the small id and bookkeeping functions are
// drafted so far; the rest are declared with TODO comments.
// Names are descriptive guesses; unmatched against retail so far.

#include <dolphin/gx.h>

typedef unsigned long u32;
typedef unsigned short u16;

// Functions called from other units under their placeholder names (C linkage).
struct ModelEntry;
extern "C" void fn_8003CE8C(int* resident, int* total);
extern "C" void fn_8003D710(unsigned long id);
extern "C" void fn_8003D7A0(short* ids, int n);
extern "C" void fn_8003D8F4(unsigned long id);
extern "C" int fn_8003DEFC(ModelEntry* e);
extern "C" void fn_8003E5AC(void);
extern "C" void fn_8003E5B0();
extern "C" void fn_8003F0D4(unsigned long id);

struct StreamEntry;

// Model table entry (0x68 bytes).
struct ModelEntry {
    int texCount;       // 0x00
    char pad04[0x28];
    int data;           // 0x2C: loaded data pointer
    char pad30[4];
    short state;        // 0x34: stream slot state (0 free, 1 queued, 2 requested, 4 resident)
    char prio;          // 0x36
    char pad37;
    int unk38;
    int size;           // 0x3C
    char pad40[4];
    int hasData;        // 0x44
    char pad48[0x68 - 0x48];
};

// The texture id list lives at +0x30 (index 0xC of the word view), the stream entry at +0x34.
struct ModelEntryView {
    int texCount;       // 0x00
    char pad04[0x2C];
    int* texIds;        // 0x30
    char stream[4];     // 0x34
};

void StreamRead(int pri, StreamEntry* e, void (*done)(int, void*, void*), void* arg);
void GunStateTimerTick8017BAD0(void* p);
void GunStateStep8017BBBC(void* p);

extern "C" {
void fn_8000F60C(int id);
void fn_8000F7E4(int id, char prio);
void fn_8000F874(int id);
int fn_80049940(void);
void fn_8009C87C(void);
// TODO (not drafted yet, 0x398 bytes at 0x8004256C): release a model instance
void fn_8004256C(u32 id, int flag);
}

typedef unsigned char u8;

struct ModelMeshRec {
    char pad00[0x20];
    u32 word20;         // 0x20
    char pad24[0x0C];
};

// Mesh table view of a model entry: mesh count at +4, mesh array (0x30 bytes each) at +0x2C.
struct ModelEntryMesh {
    int texCount;       // 0x00
    int meshCount;      // 0x04
    char pad08[0x24];
    ModelMeshRec* meshes;   // 0x2C
};

// Mesh and primitive records as read by the skinning code.
struct ModelMesh {
    char pad00[0x1C];
    int bone;           // 0x1C
};

struct ModelPrim {
    char pad00[8];
    int count;          // 0x08
    char pad0C[8];
    u8* recs;           // 0x14: 13-byte vertex records, big-endian vertex index at +12
    float* verts;       // 0x18
};

// Queued model draw request.
struct ModelDraw {
    int model;          // 0x000
    int anim;           // 0x004
    int flag;           // 0x008
    char xf[0x48 - 0x0C];   // 0x00C
    int v48;            // 0x048
    int v4C;            // 0x04C
    char pos[0x5C - 0x50];  // 0x050
    char skin[0x25C - 0x5C];    // 0x05C
    int hasSkin;        // 0x25C
    int flags;          // 0x260
    int extra;          // 0x264
};

extern "C" {
void fn_80096390(int a, void* b);
void fn_8010D024(void);
void fn_8010CF00(void);
void fn_80042BB4(void* m);
void fn_80040AE0(int model, int anim, void* xf, int v48, int v4C, int flag, void* pos, void* skin, int extra, int flags);
void fn_800A0100(float* m);
void fn_800A0094(float* out, float* in);
void fn_8003E0FC(struct ModelMesh* mesh, struct ModelPrim* prim, void* mats, float* out, int stride);
void fn_8004320C(float* src, float* dst);
}
void GfxSetVtxDesc(int v);                  // 0x80008DAC
void GfxSetBlendMode(int a, int b, int c, int d);   // 0x801D8A10
void GfxSetCullMode(int mode);              // 0x80008B44

extern int lbl_804D437C;
extern int lbl_804D4380;
extern int lbl_804D4384;
extern int lbl_804D4388;
extern int lbl_804D438C;
extern int lbl_804D4390;
extern GXColor lbl_804D6208;
extern int lbl_804D4334;
extern int lbl_804D4338;
extern int lbl_804D4378;
extern int lbl_804D435C;
extern int lbl_804D4360;
extern int lbl_804D269C;
extern int lbl_80393E08[][15];
extern ModelEntry* lbl_804D4340;
extern int lbl_804D4350;
extern int lbl_804D4354;
extern int lbl_804D4358;
extern int lbl_804D26D0;
extern int lbl_804D26C4;
extern int lbl_804D4330;
extern int lbl_8040E018[15];
extern int lbl_80410154[12];
extern u32 lbl_8040E054[64];

// TODO (not drafted yet, 0x1F8 bytes at 0x8003CC90): unknown
// (0x8003CE88 below is the empty function.)
void ModelNop(void) {}

// Total and resident streaming memory.
void fn_8003CE8C(int* resident, int* total) {
    int r = 0;
    int t = 0;
    ModelEntry* e = lbl_804D4340;
    int n = lbl_804D4350;

    if (n > 0) {
        do {
            t += e->size;
            if (e->state == 1 || e->state == 4) {
                r += e->size;
            }
            e++;
            n--;
        } while (n != 0);
    }
    if (resident != 0) {
        *resident = r;
    }
    if (total != 0) {
        *total = t;
    }
}

void ModelSlotDone(int result, void* arg, void* entry);

// Queue every model whose load was requested.
void ModelFlushRequests() {
    if (lbl_804D4354 != 0 && lbl_804D26D0 != 0) {
        lbl_804D4354 = 0;
        for (int i = 0; i < lbl_804D4350; i++) {
            ModelEntry* e = &lbl_804D4340[i];
            if (e->state == 2) {
                e->state = 1;
                StreamRead(0x2B, (StreamEntry*)&e->state, ModelSlotDone, e);
            }
        }
    }
}

// TODO (not drafted yet, 0x4CC bytes at 0x8003D1A0): stream completion callback (ModelSlotDone), chains the texture loads
// TODO (not drafted yet, 0x1F8 bytes at 0x8003CF98): unknown

void ModelFreeIds(short* ids, int n) {
    for (int i = 0; i < n; i++) {
        if ((int)(u16)*ids < lbl_804D4350) {
            lbl_804D4358 = 1;
            ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[(u16)*ids];
            GunStateTimerTick8017BAD0(e->stream);
            for (int j = 0; j < e->texCount; j++) {
                fn_8000F60C(e->texIds[j]);
            }
        }
        ids++;
    }
}

void fn_8003D710(u32 id) {
    if ((int)(id & 0xFFFF) < lbl_804D4350) {
        lbl_804D4358 = 1;
        ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[id & 0xFFFF];
        GunStateTimerTick8017BAD0(e->stream);
        for (int j = 0; j < e->texCount; j++) {
            fn_8000F60C(e->texIds[j]);
        }
    }
}

void fn_8003D7A0(short* ids, int n) {
    for (int i = 0; i < n; i++) {
        if ((int)(u16)*ids < lbl_804D4350) {
            lbl_804D4354 = 1;
            ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[(u16)*ids];
            ((ModelEntry*)e)->prio = 1;
            GunStateStep8017BBBC(e->stream);
            for (int j = 0; j < e->texCount; j++) {
                fn_8000F7E4(e->texIds[j], 1);
            }
        }
        ids++;
    }
}

void ModelLoadIds(short* ids, int n) {
    for (int i = 0; i < n; i++) {
        if ((int)(u16)*ids < lbl_804D4350) {
            lbl_804D4354 = 1;
            ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[(u16)*ids];
            GunStateStep8017BBBC(e->stream);
            ((ModelEntry*)e)->prio = 0;
            for (int j = 0; j < e->texCount; j++) {
                fn_8000F874(e->texIds[j]);
            }
        }
        ids++;
    }
}

void fn_8003D8F4(u32 id) {
    if ((int)(id & 0xFFFF) < lbl_804D4350) {
        lbl_804D4354 = 1;
        ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[id & 0xFFFF];
        GunStateStep8017BBBC(e->stream);
        ((ModelEntry*)e)->prio = 0;
        for (int j = 0; j < e->texCount; j++) {
            fn_8000F874(e->texIds[j]);
        }
    }
}

// TODO (not drafted yet, 0x22C bytes at 0x8003D988): unknown

void ModelSaveBlockA(int* src) {
    for (int i = 0; i < 15; i++) {
        lbl_8040E018[i] = src[i];
    }
}

void ModelLoadBlockA(int* dst) {
    for (int i = 0; i < 15; i++) {
        dst[i] = lbl_8040E018[i];
    }
}

// TODO (not drafted yet, 0x240 bytes at 0x8003DCBC): unknown

int fn_8003DEFC(ModelEntry* e) {
    if (e->hasData != 0) {
        return e->data;
    }
    return 0;
}

void ModelSaveBlockB(int* src) {
    for (int i = 0; i < 12; i++) {
        lbl_80410154[i] = src[i];
    }
}

// TODO (not drafted yet, 0x17C bytes at 0x8003DF80): unknown
// Skins the vertices of one primitive with the bone matrix of its mesh and offsets them by the
// matrix translation; writes `stride` floats apart into `out`.
void fn_8003E0FC(ModelMesh* mesh, ModelPrim* prim, void* mats, float* out, int stride) {
    float* m = (float*)((char*)mats + mesh->bone * 0x30);
    int off = 0;
    int i;
    fn_800A0100(m);
    for (i = 0; i < prim->count; i++) {
        float* v = out + off;
        u8* rec = prim->recs + i * 13;
        fn_800A0094(v, prim->verts + ((rec[12] << 8) | rec[13]) * 3);
        off += stride;
        v[0] += m[9];
        v[1] += m[10];
        v[2] += m[11];
    }
}

// TODO (not drafted yet, 0x2CC bytes at 0x8003E1C8): unknown
// Per-frame bookkeeping: clears the frame counters, advances the clock and reloads the render
// parameter block when the selected set changed.
void ModelFrameBegin(float dt) {
    lbl_804D4334 = 0;
    lbl_804D4378 += (int)dt;
    lbl_804D4338 = 0;
    if (lbl_804D435C != lbl_804D4360) {
        lbl_804D4360 = lbl_804D435C;
        for (int i = 0; i < 15; i++) {
            lbl_8040E018[i] = lbl_80393E08[lbl_804D435C][i];
        }
    }
    lbl_804D269C = (fn_80049940() == 0);
}

void fn_8003E5AC(void) {}

void fn_8003E5B0() {
    if (lbl_804D26C4 != 0) {
        u32* p = lbl_8040E054;
        for (int i = 0; i < lbl_804D4330; i++) {
            fn_8004256C(*p, 1);
            p++;
        }
        lbl_804D4330 = 0;
    }
}

// TODO (not drafted yet, 0x2BC bytes at 0x8003E620): unknown
// TODO (not drafted yet, 0x7F8 bytes at 0x8003E8DC): unknown

void fn_8003F0D4(u32 id) {
    if (fn_80049940() == 0 && lbl_804D4340 != 0 && (int)(id & 0xFFFF) < lbl_804D4350) {
        ModelEntry* e = &lbl_804D4340[id & 0xFFFF];
        int d;
        if (e->hasData == 0) {
            d = 0;
        } else {
            d = e->data;
        }
        if (d != 0) {
            fn_8009C87C();
        }
    }
}

// TODO (not drafted yet, 0x9FC bytes at 0x8003F15C): unknown
// TODO (not drafted yet, 0x414 bytes at 0x8003FB58): unknown
// TODO (not drafted yet, 0x694 bytes at 0x8003FF6C): unknown
// 0x80040600 (4 bytes) is another empty function; 0x80040604 .. 0x8004393C: not drafted yet:
// Returns word 8 of mesh `mesh` of model `id`, or 0 when out of range.
u32 ModelMeshWord(u32 id, int mesh) {
    int idx = id & 0xFFFF;
    ModelEntryMesh* e;
    if (idx < 0 || !(idx < lbl_804D4350)) {
        return 0;
    }
    e = (ModelEntryMesh*)&lbl_804D4340[idx];
    if (mesh >= 0 && mesh < e->meshCount) {
        return e->meshes[mesh].word20;
    }
    return 0;
}

// TODO (not drafted yet, 0x25C bytes at 0x8004065C)
// Clears the per-frame model statistics counters.
void ModelStatsReset() {
    lbl_804D4380 = 0;
    lbl_804D437C = 0;
    lbl_804D4384 = 0;
    lbl_804D4388 = 0;
    lbl_804D438C = 0;
    lbl_804D4390 = 0;
}

// TODO (not drafted yet, 0x19C bytes at 0x800408D8)
// Queues the model draw described by the request, adding the "queued" flag.
void ModelDrawQueue(ModelDraw* d) {
    fn_80040AE0(d->model, d->anim, d->xf, d->v48, d->v4C, d->flag, d->pos, d->hasSkin != 0 ? d->skin : 0, d->extra,
                d->flags | 4);
}

// TODO (not drafted yet, 0xDBC bytes at 0x80040AE0)
// TODO (not drafted yet, 0x2C8 bytes at 0x8004189C)
// TODO (not drafted yet, 0x22C bytes at 0x80041B64)
// TODO (not drafted yet, 0x504 bytes at 0x80041D90)
// TODO (not drafted yet, 0x2D8 bytes at 0x80042294)
// TODO (not drafted yet, 0x398 bytes at 0x8004256C)
// Draws the model in the plain opaque pass.
void ModelDrawOpaque(void* m) {
    fn_80096390(0xFF, 0);
    GfxSetVtxDesc(7);
    fn_8010D024();
    GfxSetBlendMode(0, 1, 1, 3);
    GXSetZMode(1, GX_LEQUAL, 0);
    GfxSetCullMode(0);
    GXSetDstAlpha(1, 0);
    fn_80042BB4(m);
    GXSetDstAlpha(0, 0);
}

// Two-sided draw, back faces first (colour from the model at +0x2C).
void ModelDrawBackFront(void* m) {
    fn_80096390(0xFF, (char*)m + 0x2C);
    GfxSetVtxDesc(7);
    fn_8010D024();
    GfxSetBlendMode(1, 1, 1, 3);
    GXSetZMode(1, GX_ALWAYS, 0);
    GfxSetCullMode(2);
    GXSetDstAlpha(1, 0);
    fn_80042BB4(m);
    GXSetZMode(1, GX_GREATER, 0);
    GfxSetCullMode(1);
    GXSetDstAlpha(1, 0xFF);
    fn_80042BB4(m);
    GXSetDstAlpha(0, 0);
}

// Two-sided draw, front faces first.
void ModelDrawFrontBack(void* m) {
    fn_80096390(0, (char*)m + 0x2C);
    GfxSetVtxDesc(7);
    fn_8010D024();
    GfxSetBlendMode(1, 1, 1, 3);
    GXSetZMode(1, GX_ALWAYS, 0);
    GfxSetCullMode(2);
    GXSetDstAlpha(1, 0xFF);
    fn_80042BB4(m);
    GXSetZMode(1, GX_GREATER, 0);
    GfxSetCullMode(1);
    GXSetDstAlpha(1, 0);
    fn_80042BB4(m);
    GXSetDstAlpha(0, 0);
}

// Two-pass draw with a constant TEV konst colour.
void ModelDrawKonst(void* m) {
    GXColor konst = lbl_804D6208;
    fn_80096390(0x7F, (char*)m + 0x2C);
    GXSetZMode(1, GX_GREATER, 0);
    GfxSetVtxDesc(7);
    GXColor kc = konst;
    GXSetTevKColor(GX_KCOLOR0, kc);
    fn_8010CF00();
    GfxSetBlendMode(1, 1, 1, 3);
    GfxSetCullMode(2);
    fn_80042BB4(m);
    GfxSetBlendMode(3, 1, 1, 3);
    GfxSetCullMode(1);
    fn_80042BB4(m);
}

// TODO (not drafted yet, 0x364 bytes at 0x80042BB4)
// TODO (not drafted yet, 0x290 bytes at 0x80042F18)
// Expands a packed 3x3 rotation + translation (12 floats) into a 3x4 matrix.
void MtxExpandPacked(float* src, float* dst) {
    float a = src[1], b = src[2], c = src[3], d = src[4], e = src[5], f = src[6], g = src[7], h = src[8];
    float tx = src[9], ty = src[10], tz = src[11];
    dst[0] = src[0];
    dst[1] = a;
    dst[2] = b;
    dst[3] = tx;
    dst[4] = c;
    dst[5] = d;
    dst[6] = e;
    dst[7] = ty;
    dst[8] = f;
    dst[9] = g;
    dst[10] = h;
    dst[11] = tz;
}

// Same, transposing the rotation.
void fn_8004320C(float* src, float* dst) {
    float a = src[3], b = src[6], c = src[1], d = src[4], e = src[7], f = src[2], g = src[5], h = src[8];
    float tx = src[9], ty = src[10], tz = src[11];
    dst[0] = src[0];
    dst[1] = a;
    dst[2] = b;
    dst[3] = tx;
    dst[4] = c;
    dst[5] = d;
    dst[6] = e;
    dst[7] = ty;
    dst[8] = f;
    dst[9] = g;
    dst[10] = h;
    dst[11] = tz;
}

// TODO (not drafted yet, 0x28C bytes at 0x80043270)
// TODO (not drafted yet, 0x440 bytes at 0x800434FC)
// TODO (not drafted yet, 0xF94 bytes at 0x8004393C)

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

void StreamFree(void* p);                                       // 0x8017C1F4
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

struct ModelMesh;

// Mesh table view of a model entry: mesh count at +4, mesh array (0x30 bytes each) at +0x2C.
struct ModelEntryMesh {
    int texCount;       // 0x00
    int meshCount;      // 0x04
    char pad08[0x24];
    ModelMesh* meshes;      // 0x2C
};

// Mesh and primitive records as read by the skinning code.
struct ModelPrim {
    int tex;            // 0x00: texture id
    char pad04[4];
    int count;          // 0x08: vertex count
    char pad0C[4];
    int nIds;           // 0x10
    u8* recs;           // 0x14: 13-byte vertex records, big-endian vertex index at +12
    float* verts;       // 0x18
    char pad1C[0x0C];
    unsigned short* ids;    // 0x28
    char pad2C[4];
};

struct ModelMesh {
    ModelPrim* prims;   // 0x00
    char pad04[8];
    int nPrims;         // 0x0C
    char pad10[0x0C];
    int bone;           // 0x1C: bone index / texture override (-1: none)
    u32 word20;         // 0x20
    char pad24[0x0C];
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
// Raw integer views used while a model's pointers are turned back into offsets.
struct PrimWords {
    int w[12];          // 0x30 bytes
};

struct MeshWords {
    PrimWords* prims;   // 0x00: becomes an index
    int w04;            // 0x04
    int w08;            // 0x08
    int nPrims;         // 0x0C
    char pad10[0x20];
};

struct ModelEntryRaw {
    int texCount;       // 0x00
    int meshCount;      // 0x04
    char pad08[0x24];
    MeshWords* meshes;  // 0x2C
    char pad30[4];
    short state;        // 0x34
    char pad36[0x0E];
    int base;           // 0x44
    int off48;          // 0x48
    int off4C;          // 0x4C
    int off50;          // 0x50
    int off54;          // 0x54
    int off58;          // 0x58
    int off5C;          // 0x5C
    int off60;          // 0x60
    int off64;          // 0x64
};

// Releases the data of models whose streaming slot is in the "releasing" state: converts the
// pointers inside the model back into offsets and clears the relocation bases.
void fn_8003CF98(void) {
    if (lbl_804D4358 != 0 && lbl_804D26D0 != 0) {
        lbl_804D4358 = 0;
        for (int i = 0; i < lbl_804D4350; i++) {
            ModelEntryRaw* e = (ModelEntryRaw*)&lbl_804D4340[i];
            if (e->state == 3) {
                if (e->base == 0) {
                    lbl_804D4358 = 1;
                } else {
                    e->state = 0;
                    if (e->base != 0) {
                        MeshWords* m = e->meshes;
                        for (int k = 0; k < e->meshCount; k++, m++) {
                            for (int j = 0; j < m->nPrims; j++) {
                                PrimWords* pr = &m->prims[j];
                                pr->w[5] -= e->off48;
                                pr->w[6] = (pr->w[6] - e->off4C) / 12;
                                pr->w[8] = (pr->w[8] - e->off54) / 4;
                                pr->w[9] = (pr->w[9] - e->off58) / 4;
                                pr->w[10] = (pr->w[10] - e->off64) / 2;
                            }
                            m->prims = (PrimWords*)(((int)m->prims - e->base) / 0x30);
                            m->w04 = (m->w04 - e->off5C) / 32;
                            m->w08 = (m->w08 - e->off60) / 0x24;
                        }
                    }
                    StreamFree((void*)e->base);
                    e->base = 0;
                    e->off4C = 0;
                    e->off50 = 0;
                    e->off54 = 0;
                    e->off58 = 0;
                    e->off5C = 0;
                    e->off60 = 0;
                    e->off64 = 0;
                }
            }
        }
    }
}


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

extern "C" u32 fn_80010000(int id);
extern "C" int fn_8003D988(float* box, u32 id);
extern "C" void fn_80008C00(const char* msg);
extern float lbl_804D6220;
extern float lbl_804D6224;

// (ModelMirrorBounds) Computes the bounding box (min xyz, max xyz) of the mirror-textured primitives of a model;
// returns the mirror texture id, -1 when there is no model data, -2 on error.
int fn_8003D988(float* box, u32 id) {
    int tex = -1;
    if ((int)(id & 0xFFFF) < lbl_804D4350) {
        ModelEntry* e = &lbl_804D4340[id & 0xFFFF];
        ModelMesh* mesh = e->hasData == 0 ? 0 : (ModelMesh*)e->data;
        if (mesh != 0) {
            bool found = false;
            box[0] = lbl_804D6220;
            box[1] = lbl_804D6220;
            box[2] = lbl_804D6220;
            box[3] = lbl_804D6224;
            box[4] = lbl_804D6224;
            box[5] = lbl_804D6224;
            for (int m = 0; m < ((ModelEntryMesh*)e)->meshCount; m++) {
                for (int p = 0; p < mesh->nPrims; p++) {
                    ModelPrim* prim = &mesh->prims[p];
                    if ((fn_80010000(prim->tex) & 0x20000) != 0) {
                        if (tex == -1) {
                            tex = prim->tex;
                        } else if (tex != prim->tex) {
                            fn_80008C00("Model uses more than one mirror texture");
                            return -2;
                        }
                        found = true;
                        for (int v = 0; v < prim->count; v++) {
                            u8* rec = prim->recs + v * 13;
                            float* pt = prim->verts + ((rec[12] << 8) | rec[13]) * 3;
                            box[0] = pt[0] < box[0] ? pt[0] : box[0];
                            box[1] = pt[1] < box[1] ? pt[1] : box[1];
                            box[2] = pt[2] < box[2] ? pt[2] : box[2];
                            box[3] = box[3] < pt[0] ? pt[0] : box[3];
                            box[4] = box[4] < pt[1] ? pt[1] : box[4];
                            box[5] = box[5] < pt[2] ? pt[2] : box[5];
                        }
                    }
                }
                mesh++;
            }
            if (!found) {
                return -2;
            }
            return tex;
        }
    }
    return -1;
}


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

// Collects the distinct texture ids used by mesh `mesh` of model `id` into `out` (at most `max`).
extern "C" int fn_80037C20(u32 id);
int ModelCollectTextures(u32 id, int mesh, u32* out, int max) {
    id &= 0xFFFF;
    if ((int)id < lbl_804D4350) {
        int count = 0;
        if (mesh < 0 || fn_80037C20(id) < mesh) {
            return 0;
        }
        ModelEntry* e = &lbl_804D4340[id];
        ModelMesh* meshes = e->hasData == 0 ? 0 : (ModelMesh*)e->data;
        if (meshes == 0) {
            return 0;
        }
        ModelMesh* m = &meshes[mesh];
        if (m->bone != -1) {
            out[0] = m->bone;
            return 1;
        }
        u32* dst = out;
        for (int p = 0; p < m->nPrims; p++) {
            ModelPrim* prim = &m->prims[p];
            for (int k = 0; k < prim->nIds; k++) {
                int n = 0;
                u32* q = out;
                for (int left = count; left > 0; left--) {
                    if (*q == prim->ids[k]) {
                        break;
                    }
                    q++;
                    n++;
                }
                if (n == count) {
                    count++;
                    *dst++ = prim->ids[k];
                    if (max <= count) {
                        return count;
                    }
                }
            }
        }
        return count;
    }
    return 0;
}

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

struct ModelParams {
    int w0;
    float f4, f8, fC;
    int w10, w14, w18, w1C, w20, w24, w28, w2C, w30, w34, w38;
};

extern GXColor lbl_8040E154[0x800];
extern int lbl_804D433C;
extern int lbl_804D4344;
extern int lbl_804D4348;
extern int lbl_804D2698;
int FileAddHandler(int type, int (*fn)(void*, int, void*), void* arg);
extern "C" void fn_80043270(void);
void ShadowResetAll();
void fn_80234AD8();
int FxLookupScrollType(const u8* name);

// Resets the model table and its colour/parameter caches and registers the stream handler.
void ModelTableInit() {
    GXColor* c = lbl_8040E154;
    int i;
    lbl_804D4360 = 0;
    lbl_804D435C = 0;
    *(ModelParams*)lbl_8040E018 = *(ModelParams*)lbl_80393E08;
    for (i = 0; i < 0x100; i++) {
        c[0].r = c[0].g = c[0].b = 0;
        c[0].a = 0xFF;
        c[1].r = c[1].g = c[1].b = 0;
        c[1].a = 0xFF;
        c[2].r = c[2].g = c[2].b = 0;
        c[2].a = 0xFF;
        c[3].r = c[3].g = c[3].b = 0;
        c[3].a = 0xFF;
        c[4].r = c[4].g = c[4].b = 0;
        c[4].a = 0xFF;
        c[5].r = c[5].g = c[5].b = 0;
        c[5].a = 0xFF;
        c[6].r = c[6].g = c[6].b = 0;
        c[6].a = 0xFF;
        c[7].r = c[7].g = c[7].b = 0;
        c[7].a = 0xFF;
        c += 8;
    }
    lbl_804D433C = 0;
    lbl_804D4340 = 0;
    lbl_804D4344 = 0;
    lbl_804D4348 = 0;
    lbl_804D4350 = 0;
    FileAddHandler(0x2B, (int (*)(void*, int, void*))fn_80043270, 0);
    ShadowResetAll();
    fn_80234AD8();
    lbl_804D2698 = FxLookupScrollType((const u8*)"EXPAND");
}

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

// Simple meshes (0x800448D0-0x800451D8): lists of triangles, strips, fans and lines over a vertex
// array (position, UV, color), drawn directly through the GX FIFO or deferred (copied to scratch
// memory and sorted by material).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/gx.h>

extern "C" {
// Callees in other files, not named yet.
int fn_8000FECC(int material);
int fn_801D89A4(int mode);
void fn_800092CC(void);
int fn_80010448(int material, int arg);
void fn_80010330(int material, int arg);
}

struct Vec3;
void DeferAdd(int priority, Vec3* pos, void (*draw)(void*), void* arg); // DeferPriority.cpp
void* ScratchBAlloc(int size);
void GfxSetVtxDesc(int desc);
void GfxSetMaterial(void* material);
void GfxSetCullMode(int mode);

extern int gGfxPrimCount; // 0x804D4004

enum {
    MESH_TRIANGLE,
    MESH_STRIP,
    MESH_FAN,
    MESH_LINE,
    MESH_LINESTRIP,
};

struct MeshVertex {
    float x, y, z;
    float u, v;
    GXColor color;
};

struct MeshPrim {
    int material;
    int type;
    union {
        s16 idx[4];
        struct {
            int start;
            int count;
        } range;
    };
};

struct Mesh {
    int unk0;
    MeshVertex* verts;
    MeshPrim* prims;
    s16 numVerts;
    s16 numPrims;
};

int sMeshEnabled = 1;

void MeshDrawPrim(MeshPrim* prim, MeshVertex* verts);
void MeshDeferPrim(MeshPrim* prim, MeshVertex* verts);

void MeshDraw(Mesh* mesh) {
    int i;

    if (sMeshEnabled) {
        for (i = 0; i < mesh->numPrims; i++) {
            MeshDrawPrim(&mesh->prims[i], mesh->verts);
        }
    }
}

void MeshNop0(void) {}

void MeshDefer(Mesh* mesh) {
    int i;

    if (sMeshEnabled && mesh->numPrims) {
        for (i = 0; i < mesh->numPrims; i++) {
            MeshDeferPrim(&mesh->prims[i], mesh->verts);
        }
    }
}

void MeshNop1(void) {}

void MeshEnable(void) { sMeshEnabled = 1; }

inline void MeshCopyVertex(MeshVertex* dst, MeshVertex* src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
    dst->u = src->u;
    dst->v = src->v;
    dst->color = src->color;
}

void MeshDrawDeferred(MeshPrim* prim);

void MeshDeferPrim(MeshPrim* prim, MeshVertex* verts) {
    int n;
    int i;
    MeshPrim* copy;
    MeshVertex* dv;

    switch (prim->type) {
    case MESH_TRIANGLE:
        n = 3;
        break;
    case MESH_STRIP:
        n = prim->range.count;
        break;
    case MESH_FAN:
        n = prim->range.count;
        break;
    case MESH_LINE:
        n = 2;
        break;
    case MESH_LINESTRIP:
        n = prim->range.count;
        break;
    default:
        n = 0;
        break;
    }

    copy = (MeshPrim*)ScratchBAlloc(n * sizeof(MeshVertex) + sizeof(MeshPrim));
    dv = (MeshVertex*)(copy + 1);
    *copy = *prim;

    switch (prim->type) {
    case MESH_TRIANGLE:
        for (i = 0; i < 3; i++) {
            MeshCopyVertex(&dv[i], &verts[prim->idx[i]]);
            copy->idx[i] = i;
        }
        break;
    case MESH_STRIP:
        copy->range.start = 0;
        for (i = 0; i < prim->range.count; i++) {
            MeshCopyVertex(&dv[i], &verts[prim->range.start + i]);
        }
        break;
    case MESH_FAN:
        copy->range.start = 0;
        for (i = 0; i < prim->range.count; i++) {
            MeshCopyVertex(&dv[i], &verts[prim->range.start + i]);
        }
        break;
    case MESH_LINE:
        MeshCopyVertex(&dv[0], &verts[prim->range.start]);
        copy->range.start = 0;
        MeshCopyVertex(&dv[1], &verts[prim->range.count]);
        copy->range.count = 1;
        break;
    case MESH_LINESTRIP:
        copy->range.start = 0;
        for (i = 0; i < prim->range.count; i++) {
            MeshCopyVertex(&dv[i], &verts[prim->range.start + i]);
        }
        break;
    }

    DeferAdd(fn_8000FECC(prim->material), (Vec3*)dv, (void (*)(void*))MeshDrawDeferred, copy);
}

void MeshDrawDeferred(MeshPrim* prim) {
    int old;
    MeshVertex* verts;

    GfxSetVtxDesc(2);
    GfxSetMaterial(0);
    old = fn_801D89A4(0);
    fn_800092CC();
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);
    GfxSetCullMode(0);

    verts = (MeshVertex*)(prim + 1);
    switch (fn_80010448(prim->material, 1)) {
    case 2:
        fn_80010330(prim->material, 1);
        break;
    case 0:
    case 8:
        fn_80010448(-3, 1);
        break;
    }

    MeshDrawPrim(prim, verts);
    fn_801D89A4(old);
}

inline void MeshEmitVertex(MeshVertex* v) {
    GXPosition3f32(v->x, v->y, v->z);
    GXColor4u8(v->color.r, v->color.g, v->color.b, v->color.a);
    GXTexCoord2f32(v->u, v->v);
    GXTexCoord2f32(0.0f, 0.0f);
}

void MeshDrawPrim(MeshPrim* prim, MeshVertex* verts) {
    int i;

    switch (prim->type) {
    case MESH_TRIANGLE:
        gGfxPrimCount++;
        GXBegin(GX_TRIANGLES, GX_VTXFMT2, 3);
        for (i = 0; i < 3; i++) {
            MeshEmitVertex(&verts[prim->idx[i]]);
        }
        break;
    case MESH_STRIP:
        gGfxPrimCount += prim->range.count - 2;
        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT2, prim->range.count);
        for (i = 0; i < prim->range.count; i++) {
            MeshEmitVertex(&verts[prim->range.start + i]);
        }
        break;
    case MESH_FAN:
        gGfxPrimCount += prim->range.count - 2;
        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT2, prim->range.count);
        for (i = 0; i < prim->range.count; i++) {
            MeshEmitVertex(&verts[prim->range.start + i]);
        }
        break;
    case MESH_LINE:
        GXBegin(GX_LINES, GX_VTXFMT2, 2);
        MeshEmitVertex(&verts[prim->range.start]);
        MeshEmitVertex(&verts[prim->range.count]);
        break;
    case MESH_LINESTRIP:
        GXBegin(GX_LINESTRIP, GX_VTXFMT2, prim->range.count);
        for (i = 0; i < prim->range.count; i++) {
            MeshEmitVertex(&verts[prim->range.start + i]);
        }
        break;
    }
}

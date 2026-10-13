// Shadow2.cpp: shadow volume models (0x8009C74C-0x8009D710).
// Up to 32 models get a triangle list with edge adjacency (triangles sharing an edge are linked) built
// from their meshes, plus a transformed vertex buffer; skinned meshes blend their bone matrices first.
// Also keeps the shadow light setup.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned short u16;

extern "C" {
void* memset(void* dst, int val, unsigned long n);

// Model helpers in other files, not named yet.
struct ShadowMesh* fn_8003DEFC(struct ShadowModelData* model);
void fn_8003E0FC(struct ShadowMesh* mesh, struct ShadowPrim* prim, void* links, struct Vec3* out, int stride);
void fn_8003E1C8(struct ShadowPrim* prim, struct Mtx43* mats, struct Vec3* out, int stride);
void fn_8009FE18(struct Mtx43* out, void* link, struct Mtx34* mtx);
}

struct Vec3 {
    float x;
    float y;
    float z;
    Vec3() {}
    Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
};

struct Mtx34 {
    float m[3][4];
};

struct Mtx43 {
    float m[4][3];
};

struct Bone {
    u8 pad0[0x10];
    float pos[3];
    int index;
};

struct LinkInfo {
    u8 pad[0x30];
};

void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);
void* ScratchAAlloc(int size);
void VFree(void* p);
void LinkInfoInit(void* info);
void BoneWorldMatrix(Mtx34* out, Bone* bone, float* pos);
void DebugPrintf(const char* format, ...);
void ScreenPrintf(const char* format, ...);

struct ShadowPrim {
    u8 pad0[4];
    unsigned long flags;
    int count;
    u8 padC[8];
    u8* dl;
    Vec3* pos;
    u8 pad1C[0x14];
};

struct ShadowWeight {
    int count;
    int bones[4];
    float weights[4];
};

struct ShadowMesh {
    ShadowPrim* prims;
    Bone* bones;
    ShadowWeight* weights;
    int nPrims;
    int nBones;
    int weightBase;
    int nWeights;
    int bone;
    u8 pad20[0x10];
};

struct ShadowModelData {
    u8 pad0[4];
    int nMeshes;
};

struct ShadowTri {
    u16 v[3];
    u8 pad6[2];
    ShadowTri* adj[3];
};

struct LightVec {
    float v[3];
};

struct ShadowLightPoint {
    LightVec pos;
    float w;
};

struct ShadowLightInfo {
    u8 pad0[0x10];
    ShadowLightPoint points[6];
};

struct ShadowLightBlock {
    int type;
    u8 pad4[0xC];
    LightVec a;
    float aw;
    LightVec b;
    float bw;
    u8 pad30[0x40];
};

class ShadowModel {
public:
    ShadowModel() {
        edges = 0;
        Reset();
    }
    ~ShadowModel() { Reset(); }

    void Reset() {
        model = 0;
        id = -1;
        if (edges) {
            MemFree(edges, "Shadow2.cpp", 0x583);
        }
        edges = 0;
        nTris = 0;
        nVerts = 0;
    }

    ShadowModelData* model;
    int id;
    ShadowTri* edges;
    int nTris;
    int nVerts;
};

static ShadowModel sShadowModels[32];
static ShadowLightInfo sShadowLightInfo;
static ShadowLightBlock sShadowLightA;
static ShadowLightBlock sShadowLightB;
static ShadowLightBlock sShadowLightC;

static int sShadowCount;
static Vec3* sShadowVerts;
static int sShadowSkin;

#define DL_INDEX(p, o) ((p)[o] << 8 | (p)[(o) + 1])

inline bool VecEq(const Vec3& a, const Vec3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

inline bool TriDegenerate(const Vec3& a, const Vec3& b, const Vec3& c) {
    return VecEq(a, b) || VecEq(b, c) || VecEq(c, a);
}

int ShadowBuild(ShadowModel* sm, int id, ShadowModelData* model);

void ShadowSetLight(ShadowLightInfo* info) {
    sShadowLightInfo = *info;
    sShadowLightA.type = 2;
    sShadowLightA.a = info->points[4].pos;
    sShadowLightA.aw = info->points[4].w;
    sShadowLightA.b = info->points[5].pos;
    sShadowLightA.bw = info->points[5].w;
    sShadowLightB.type = 2;
    sShadowLightB.a = info->points[1].pos;
    sShadowLightB.aw = info->points[1].w;
    sShadowLightB.b = info->points[3].pos;
    sShadowLightB.bw = info->points[3].w;
    sShadowLightC.type = 2;
    sShadowLightC.a = info->points[0].pos;
    sShadowLightC.aw = info->points[0].w;
    sShadowLightC.b = info->points[2].pos;
    sShadowLightC.bw = info->points[2].w;
}

inline ShadowModel* ShadowFind(int id) {
    int i;

    for (i = 0; i < sShadowCount; i++) {
        if (sShadowModels[i].id == id) {
            return &sShadowModels[i];
        }
    }
    return 0;
}

void ShadowAdd(int id, ShadowModelData* model) {
    if (ShadowFind(id) == 0 && sShadowCount < 32) {
        if (ShadowBuild(&sShadowModels[sShadowCount], id, model)) {
            sShadowCount++;
        }
    }
}

void ShadowResetAll(void) {
    int i;

    for (i = 0; i < 32; i++) {
        sShadowModels[i].Reset();
    }
    sShadowCount = 0;
}

void ShadowUpdate(void) {}

void ShadowFillTris(ShadowModelData* model, ShadowMesh* meshes, ShadowTri* out, int count);
int ShadowTransform(ShadowModelData* model, ShadowMesh* meshes, int id, LinkInfo* links, LinkInfo* links2, Vec3* out);
void ShadowLink(ShadowTri* a, Vec3* av, ShadowTri* b, Vec3* bv);

inline int ShadowCountTris(ShadowModelData* model, ShadowMesh* meshes) {
    int n = 0;
    int i;
    int j;
    int k;

    for (i = 0; i < model->nMeshes; i++) {
        for (j = 0; j < meshes[i].nPrims; j++) {
            ShadowPrim* prim = &meshes[i].prims[j];
            if (prim->flags & 1) {
                n += prim->count / 3;
            } else {
                for (k = 0; k < prim->count - 2; k++) {
                    u8* p = &prim->dl[k * 13];
                    Vec3* a = &prim->pos[DL_INDEX(p, 0xC)];
                    Vec3* b = &prim->pos[DL_INDEX(p, 0x19)];
                    Vec3* c = &prim->pos[DL_INDEX(p, 0x26)];
                    if (!TriDegenerate(*a, *b, *c)) {
                        n++;
                    }
                }
            }
        }
    }
    return n;
}

int ShadowBuild(ShadowModel* sm, int id, ShadowModelData* model) {
    LinkInfo links[0xFE];
    Vec3 av[3];
    Vec3 bv[3];
    ShadowMesh* meshes;
    int n;
    int i;
    int j;
    int k;

    meshes = fn_8003DEFC(model);
    if (meshes == 0) {
        return 0;
    }
    sm->model = model;
    sm->id = id;
    sm->nTris = ShadowCountTris(model, meshes);
    sm->edges = (ShadowTri*)MemAlloc(sm->nTris * sizeof(ShadowTri), "Shadow2.cpp", 0x59C);
    memset(sm->edges, 0, sm->nTris * sizeof(ShadowTri));
    ShadowFillTris(model, meshes, sm->edges, sm->nTris);
    {
        LinkInfo* l;
        for (i = 0, l = links; i < 0xFE; i++, l++) {
            LinkInfoInit(l);
        }
    }
    sShadowVerts = (Vec3*)ScratchAAlloc(sm->nTris * 0x24);
    sm->nVerts = ShadowTransform(model, meshes, id, links, links, sShadowVerts);
    for (n = 0; n < sm->nTris; n++) {
        av[0] = sShadowVerts[sm->edges[n].v[0]];
        av[1] = sShadowVerts[sm->edges[n].v[1]];
        av[2] = sShadowVerts[sm->edges[n].v[2]];
        for (j = n + 1; j < sm->nTris; j++) {
            bv[0] = sShadowVerts[sm->edges[j].v[0]];
            bv[1] = sShadowVerts[sm->edges[j].v[1]];
            bv[2] = sShadowVerts[sm->edges[j].v[2]];
            ShadowLink(&sm->edges[n], av, &sm->edges[j], bv);
        }
    }
    DebugPrintf("Shadow Model ID %d: %d Faces, %d Verts (%d max)\n", id, sm->nTris, sm->nVerts, 0x800);
    return 1;
}

struct EdgeVec {
    EdgeVec(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
    float x;
    float y;
    float z;
};

inline bool EdgeEq(const EdgeVec& a, const Vec3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

inline int ShadowSharedEdge(const EdgeVec& p0, const EdgeVec& p1, const Vec3* bv) {
    if (EdgeEq(p0, bv[0])) {
        if (EdgeEq(p1, bv[1])) {
            return 0;
        }
        if (EdgeEq(p1, bv[2])) {
            return 2;
        }
    }
    if (EdgeEq(p0, bv[1])) {
        if (EdgeEq(p1, bv[0])) {
            return 0;
        }
        if (EdgeEq(p1, bv[2])) {
            return 1;
        }
    }
    if (EdgeEq(p0, bv[2])) {
        if (EdgeEq(p1, bv[0])) {
            return 2;
        }
        if (EdgeEq(p1, bv[1])) {
            return 1;
        }
    }
    return -1;
}

void ShadowLink(ShadowTri* a, Vec3* av, ShadowTri* b, Vec3* bv) {
    int k;

    for (k = 0; k < 3; k++) {
        if (a->adj[k] == 0) {
            EdgeVec p0 = av[k];
            EdgeVec p1 = av[(k + 1) % 3];
            switch (ShadowSharedEdge(p0, p1, bv)) {
            case 0:
                a->adj[k] = b;
                b->adj[0] = a;
                break;
            case 1:
                a->adj[k] = b;
                b->adj[1] = a;
                break;
            case 2:
                a->adj[k] = b;
                b->adj[2] = a;
                break;
            }
        }
    }
}

int ShadowTransform(ShadowModelData* model, ShadowMesh* meshes, int id, LinkInfo* links, LinkInfo* links2, Vec3* out) {
    Mtx43 mats[761];
    Mtx34 mtx;
    int total = 0;
    int i;
    int j;
    int k;

    for (i = 0; i < model->nMeshes; i++, meshes++) {
        if (meshes->bone >= 0) {
            for (j = 0; j < meshes->nPrims; j++) {
                ShadowPrim* prim = &meshes->prims[j];
                fn_8003E0FC(meshes, prim, links, &out[total], sizeof(Vec3));
                total += prim->count;
            }
        } else {
            for (j = 0; j < meshes->nBones; j++) {
                BoneWorldMatrix(&mtx, &meshes->bones[j], meshes->bones[j].pos);
                fn_8009FE18(&mats[meshes->bones[j].index], &links2[meshes->bones[j].index], &mtx);
            }
            for (j = 0; j < meshes->nWeights; j++) {
                if (sShadowSkin) {
                    Mtx43* dst = &mats[meshes->weightBase + j];
                    memset(dst, 0, sizeof(Mtx43));
                    for (k = 0; k < meshes->weights[j].count; k++) {
                        float w = meshes->weights[j].weights[k];
                        Mtx43* src = &mats[meshes->weights[j].bones[k]];
                        dst->m[0][0] += w * src->m[0][0];
                        dst->m[1][0] += w * src->m[1][0];
                        dst->m[2][0] += w * src->m[2][0];
                        dst->m[0][1] += w * src->m[0][1];
                        dst->m[1][1] += w * src->m[1][1];
                        dst->m[2][1] += w * src->m[2][1];
                        dst->m[0][2] += w * src->m[0][2];
                        dst->m[1][2] += w * src->m[1][2];
                        dst->m[2][2] += w * src->m[2][2];
                        dst->m[3][0] += src->m[3][0] * w;
                        dst->m[3][1] += src->m[3][1] * w;
                        dst->m[3][2] += src->m[3][2] * w;
                    }
                } else {
                    mats[meshes->weightBase + j] = mats[meshes->weights[j].bones[0]];
                }
            }
            if (meshes->nWeights) {
                ScreenPrintf("Shadow VBones %d\n", meshes->nWeights);
            }
            for (j = 0; j < meshes->nPrims; j++) {
                ShadowPrim* prim = &meshes->prims[j];
                fn_8003E1C8(prim, mats, &out[total], sizeof(Vec3));
                total += prim->count;
            }
        }
    }
    return total;
}

void ShadowFillTris(ShadowModelData* model, ShadowMesh* meshes, ShadowTri* out, int count) {
    int base = 0;
    int n = 0;
    int i;
    int j;
    int k;

    for (i = 0; i < model->nMeshes; i++, meshes++) {
        for (j = 0; j < meshes->nPrims; j++) {
            ShadowPrim* prim = &meshes->prims[j];
            int step = 1;
            if (prim->flags & 1) {
                step = 3;
            }
            for (k = 0; k < prim->count - 2; k += step) {
                u8* p = &prim->dl[k * 13];
                Vec3* a = &prim->pos[DL_INDEX(p, 0xC)];
                Vec3* b = &prim->pos[DL_INDEX(p, 0x19)];
                Vec3* c = &prim->pos[DL_INDEX(p, 0x26)];
                if (!TriDegenerate(*a, *b, *c)) {
                    if ((k & 1) & ~(step >> 1)) {
                        out[n].v[0] = base + k + 2;
                        out[n].v[1] = base + k + 1;
                        out[n].v[2] = base + k;
                    } else {
                        out[n].v[0] = base + k;
                        out[n].v[1] = base + k + 1;
                        out[n].v[2] = base + k + 2;
                    }
                    n++;
                }
            }
            base += prim->count;
        }
    }
}

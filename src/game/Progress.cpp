// Progress.cpp: the loading screen (0x800D0254-0x800D0BB8).
// A ".LGC" file (version 6) holds up to 32 CMPR textures and the layout of a progress bar. Each frame
// draws a background picture, the bar frame, the bar filled by the current progress (four fill
// directions), an overlay and a "done" picture, as textured screen quads.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/gx.h>
#include <dolphin/os.h>

extern "C" {
char* strcpy(char* dst, const char* src);
size_t strlen(const char* s);
int sprintf(char* s, const char* format, ...);

// Callees in other files, not named yet.
void fn_80005A64(float dt);
int fn_800059D0(int arg);
void fn_8000ADD8(void);
void fn_80008D10(void);
void fn_800448D0(void* mesh); // draw an indexed mesh
}
void PadUpdateAll(float dt);

extern "C" void MemFree(void* p, const char* file, int line);
void* FileLoad(const char* name, int* outSize, int unused);
float LoadingTime(void);
void SystemPoll(void);
void GfxFrameEnd(void);
void GfxFrameSwap(void);
void GfxFrameBegin(void);
void GfxSetBlendMode(int type, int src, int dst, int op);
void GfxSetVtxDesc(int desc);
void GfxSetMaterial(void* material);
void GfxResetTev(void);

struct ProgressImage {
    u32 offset;
    int present;
    u32 pad8;
    s16 width;
    s16 height;
    int extra;
};

struct ProgressFile {
    int version;
    s16 unk4;
    s16 fillMode;
    float barX;
    float barY;
    float barW;
    float barH;
    float fillStart;
    float fillEnd;
    u32 pad20;
    int frameEnd;
    float doneX;
    float doneY;
    ProgressImage images[32];
};

struct ProgressSlot {
    int valid;
    int extra;
    GXTexObj tex;
    int unk28;
    float width;
    float height;
    float uScale;
    float vScale;
};

struct ProgressVertex {
    float x, y, z;
    float u, v;
    u8 r, g, b, a;
};

struct ProgressTri {
    int flags;
    int unk4;
    u16 idx[3];
    u16 pad;
};

struct ProgressMesh {
    int unk0;
    ProgressVertex* verts;
    ProgressTri* tris;
    u16 numVerts;
    u16 numTris;
};

ProgressVertex sQuadVerts[4] = {
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0},
    {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0, 0, 0, 0},
    {1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0, 0, 0, 0},
    {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0, 0, 0, 0},
};
ProgressTri sQuadTris[2] = {
    {-2, 0, {0, 0, 0}, 0},
    {-2, 0, {0, 0, 0}, 0},
};
ProgressMesh sQuadMesh = {0, sQuadVerts, sQuadTris, 4, 2};

static ProgressSlot sSlots[32];

static s16 sUnk4900;
static s16 sFillMode;
static int sFrameEnd;
static int sAlpha;
static float sProgress;
static int sBackground;
int gProgressActive;
static float sBarX;
static float sBarY;
static float sBarW;
static float sBarH;
static float sDoneX;
static float sDoneY;
static float sFillStart;
static float sFillEnd;
static ProgressFile* sFile;
static void (*sDrawCallback)(float, float);
static void (*sIdleCallback)(float, float);
static int (*sLoadCallback)(void);

void LoadingDraw(float progress, float dt, int alpha);

inline void LoadingFrame(float progress, float dt) {
    fn_80005A64(dt);
    PadUpdateAll(dt);
    LoadingDraw(progress, dt, 0xFF);
    if (sIdleCallback) {
        sIdleCallback(progress, dt);
        SystemPoll();
    }
}

void LoadingClose(void) {
    int i;

    for (i = 31; i >= 0; i--) {
        LoadingDraw(sProgress, 0.0f, i * 8);
    }

    for (i = 0; i < 32; i++) {
        if (sSlots[i].valid) {
            sSlots[i].valid = 0;
        }
    }

    sLoadCallback = 0;
    sDrawCallback = 0;
    sIdleCallback = 0;
    gProgressActive = 0;
    if (sFile) {
        MemFree(sFile, "Progress.cpp", 535);
        sFile = 0;
    }
}

void LoadingEnd(void) {
    if (gProgressActive) {
        while (gProgressActive) {
            float dt = LoadingTime();
            LoadingFrame(sProgress, dt);
            SystemPoll();
        }
    } else if (sSlots[4].valid) {
        while (fn_800059D0(20) != 1) {
            float dt = LoadingTime();
            LoadingFrame(sProgress, dt);
            SystemPoll();
        }
    }
}

void LoadingSetProgress(float progress, float dt) { LoadingFrame(progress, dt); }

int LoadingSetFile(ProgressFile* file);

int LoadingActive(const char* name) {
    int size = 0;
    char path[1024];
    void* data;

    sProgress = 0.0f;
    if (sLoadCallback && sLoadCallback()) {
        return 1;
    }

    strcpy(path, name);
    sprintf(path + strlen(path) - 3, "LGC");
    data = FileLoad(path, &size, 0);
    DCFlushRange(data, (size + 31) & ~31);
    if (data && size) {
        return LoadingSetFile((ProgressFile*)data);
    }
    return 0;
}

int LoadingSetFile(ProgressFile* file) {
    u8* texData;
    s16 w;
    int i;
    int j;
    s16 h;

    if (file) {
        sFile = file;
        if (file->version == 6) {
            sFrameEnd = file->frameEnd;
            sUnk4900 = file->unk4;
            sFillMode = file->fillMode;
            sBarX = file->barX;
            sBarY = file->barY;
            sBarW = file->barW;
            sBarH = file->barH;
            sDoneX = file->doneX;
            sDoneY = file->doneY;
            sFillStart = file->fillStart;
            sFillEnd = file->fillEnd;
            sBackground = 0;

            texData = (u8*)file + 0x2C0;
            for (i = 0; i < 32; i++) {
                if (sFile->images[i].present) {
                    h = sFile->images[i].height;
                    w = sFile->images[i].width;
                    GXInitTexObj(&sSlots[i].tex, texData + sFile->images[i].offset, w, h, GX_TF_CMPR,
                                 GX_CLAMP, GX_CLAMP, GX_FALSE);
                    sSlots[i].unk28 = 0;
                    sSlots[i].width = w;
                    sSlots[i].height = h;
                    sSlots[i].uScale = 1.0f;
                    sSlots[i].vScale = 1.0f;
                    sSlots[i].valid = 1;
                    sSlots[i].extra = sFile->images[i].extra;
                } else {
                    sSlots[i].valid = 0;
                    sSlots[i].extra = 0;
                }
            }

            {
                int alpha;
                for (j = 0, alpha = 0; j < 32; j++, alpha += 8) {
                    LoadingDraw(sProgress, 0.0f, alpha);
                }
            }
            LoadingDraw(sProgress, 0.0f, 0xFF);
            return 1;
        }
        MemFree(file, "Progress.cpp", 403);
        sFile = 0;
    }
    return 0;
}

void LoadingQuad(ProgressSlot* slot, int alpha, float x, float y, float w, float h, float u0, float u1,
                  float v0, float v1);

void LoadingDraw(float progress, float dt, int alpha) {
    sAlpha = alpha;
    sProgress = progress;
    if (sFrameEnd) {
        GfxFrameEnd();
    }
    GfxFrameSwap();
    SystemPoll();
    GfxSetBlendMode(1, 4, 5, 3);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_GREATER, 0x7F, GX_AOP_AND, GX_ALWAYS, 0);
    GfxSetVtxDesc(2);
    GfxSetMaterial(0);
    fn_80008D10();
    GfxResetTev();

    if (sSlots[sBackground].valid) {
        LoadingQuad(&sSlots[sBackground], alpha, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, sSlots[sBackground].uScale, 0.0f,
                     sSlots[sBackground].vScale);
    }

    if (sSlots[3].valid) {
        LoadingQuad(&sSlots[3], alpha, sBarX, sBarY, sBarW, sBarH, 0.0f, 1.0f, 0.0f, 1.0f);
    }

    if (sSlots[1].valid) {
        float x, y, w, h, u0, u1, v0, v1;
        float range = sFillEnd - sFillStart;
        float t = range * progress;

        switch (sFillMode) {
        case 0:
            x = sBarX;
            y = sFillStart * sBarH + sBarY;
            w = sBarW;
            h = t * sBarH;
            u0 = 0.0f;
            u1 = 1.0f;
            v0 = sFillStart;
            v1 = sFillStart + t;
            break;
        case 1:
            x = sBarX;
            y = sBarH * (sFillStart + (range - t)) + sBarY;
            w = sBarW;
            h = t * sBarH;
            u0 = 0.0f;
            u1 = 1.0f;
            v0 = sFillEnd - t;
            v1 = sFillEnd;
            break;
        case 3:
            x = sFillStart * sBarW + sBarX;
            y = sBarY;
            w = t * sBarW;
            h = sBarH;
            u0 = sFillStart;
            u1 = sFillStart + t;
            v0 = 0.0f;
            v1 = 1.0f;
            break;
        case 2:
        default:
            x = sBarW * (sFillStart + (range - t)) + sBarX;
            y = sBarY;
            w = t * sBarW;
            h = sBarH;
            u0 = sFillEnd - t;
            u1 = sFillEnd;
            v0 = 0.0f;
            v1 = 1.0f;
            break;
        }
        LoadingQuad(&sSlots[1], alpha, x, y, w, h, u0, u1, v0, v1);
    }

    if (sSlots[2].valid) {
        LoadingQuad(&sSlots[2], alpha, sBarX, sBarY, sBarW, sBarH, 0.0f, 1.0f, 0.0f, 1.0f);
    }

    if (sDrawCallback) {
        sDrawCallback(progress, dt);
    }

    if (sSlots[4].valid && 1.0f == progress) {
        LoadingQuad(&sSlots[4], alpha, sDoneX, sDoneY, sBarW, sBarH, 0.0f, 1.0f, 0.0f, 1.0f);
    }

    GfxFrameBegin();
    fn_8000ADD8();
}

void LoadingQuad(ProgressSlot* slot, int alpha, float x, float y, float w, float h, float u0, float u1,
                  float v0, float v1) {
    sQuadTris[0].idx[0] = 0;
    sQuadTris[0].idx[1] = 1;
    sQuadTris[0].idx[2] = 3;
    sQuadTris[1].idx[0] = 3;
    sQuadTris[1].idx[1] = 1;
    sQuadTris[1].idx[2] = 2;

    sQuadVerts[0].x = x;
    sQuadVerts[0].y = y;
    sQuadVerts[0].u = u0;
    sQuadVerts[0].v = v0;
    sQuadVerts[0].b = alpha;
    sQuadVerts[0].g = alpha;
    sQuadVerts[0].r = alpha;
    sQuadVerts[0].a = 0xFF;

    sQuadVerts[1].x = x;
    sQuadVerts[1].y = y + h;
    sQuadVerts[1].u = u0;
    sQuadVerts[1].v = v1;
    sQuadVerts[1].b = alpha;
    sQuadVerts[1].g = alpha;
    sQuadVerts[1].r = alpha;
    sQuadVerts[1].a = 0xFF;

    sQuadVerts[2].x = x + w;
    sQuadVerts[2].y = y + h;
    sQuadVerts[2].u = u1;
    sQuadVerts[2].v = v1;
    sQuadVerts[2].b = alpha;
    sQuadVerts[2].g = alpha;
    sQuadVerts[2].r = alpha;
    sQuadVerts[2].a = 0xFF;

    sQuadVerts[3].x = x + w;
    sQuadVerts[3].y = y;
    sQuadVerts[3].u = u1;
    sQuadVerts[3].v = v0;
    sQuadVerts[3].b = alpha;
    sQuadVerts[3].g = alpha;
    sQuadVerts[3].r = alpha;
    sQuadVerts[3].a = 0xFF;

    GXLoadTexObj(&slot->tex, GX_TEXMAP0);
    fn_800448D0(&sQuadMesh);
}

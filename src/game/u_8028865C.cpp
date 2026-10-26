// u_8028865C.cpp: clock texture pages and particle rendering (0x8028865C-0x8028A77C).
// Names are descriptive guesses; the code is matched against the retail DOL.
// Function names are descriptive guesses.

#include <math.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned int u32;

struct Vec3
{
    float x, y, z;
};

// 3x4 transformation matrix.
struct Mtx34
{
    float m[3][4];
};

struct Actor;

// One entry of the clock texture-page table (stride 0xD0).
struct VMTexturePage
{
    u8    unk00[0x50];
    float plane[4];      // 0x50: view plane (normal + distance)
    u8    unk60[0x10];
    u8    texMtx[0x30];  // 0x70: texture matrix
    u8    texObj[0x20];  // 0xA0: GXTexObj
    u32   texId;         // 0xC0
    float back[3];       // 0xC4: negated view direction
};

extern VMTexturePage lbl_804A3288[];

extern int lbl_804D37A0;
extern int lbl_804D37A4;
extern int lbl_804D37AC;
extern int lbl_804D56C8;
extern u8* lbl_804D56D0;
extern int lbl_804D56D4;
extern int lbl_804D56D8;
// Small GX material block the clock overlay fades (0x804A3BD8).
struct ClockMaterial
{
    u8    unk00[0x4];
    int   enabled;    // 0x04
    u8    chan[4];    // 0x08
    float unk0C;      // 0x0C
    float unk10;      // 0x10
};

extern ClockMaterial lbl_804A3BD8;

extern "C" void VMDeallocPages(void* page);
extern "C" void GXSetZMode(int compare, int func, int updateEnable);
extern "C" void GXSetTexCoordGen2(int dstCoord, int func, int srcParam, int mtx,
                                  int normalize, int postMtx);
extern "C" void GXLoadTexObj(void* obj, int id);
extern "C" void GXLoadTexMtxImm(void* mtx, int id, int type);
extern "C" void GXSetTevKColor(int id, void* color);
extern "C" void fn_80008AD8(void* state);
extern "C" void fn_8000895C(void* state);
extern "C" void fn_8000FBFC(int enable);
extern "C" void fn_8000FF7C();
extern "C" void fn_80012E64(int enable);
extern "C" void fn_80013F50();
extern "C" void fn_8001D5DC(u32 texId, void* page, void* extra);
extern "C" void fn_800C6200(void* camera);
extern "C" void fn_80104134(int count);
extern "C" void fn_8010CCB0();
extern "C" void fn_8010CDD8();
extern "C" void fn_8010F7EC();
extern "C" void fn_801D8A68(int mode);
void GfxSetCullMode(int mode);
void GfxResetTev();
extern "C" void fn_8000FF28(int enable);
void ClockComputeViewTransform(void* state, void* source, void* placement);
float* ClockSetupPageBasis(void* out, void* source, void* dir);
int ClockBuildPageEntries(void* handle, int index, float* pos, void* dir);

// Free every texture page the clock has loaded.
void ClockFreeTexturePages()
{
    int i;

    for (i = 0; i < lbl_804D56C8; i++)
        VMDeallocPages(lbl_804D56D0 + lbl_804D56D4 * i);
    lbl_804D56C8 = 0;
}

// Set up GX state and draw the resident VM texture pages.
// Other units call this by its placeholder name, so the symbol stays fn_802886B4.
extern "C" void fn_802886B4()
{
    u8 saved[0x14];
    u32 texIds[16];
    u8 camera[0x78];
    int i;
    int j;
    int n;

    if (lbl_804D37A0 <= 1 || lbl_804D56C8 == 0)
        return;

    fn_80008AD8(saved);
    fn_8000895C(&lbl_804A3BD8);
    fn_801D8A68(0x10);
    fn_8000FBFC(1);
    GfxSetCullMode(1);
    GXSetZMode(1, 2, 0);
    fn_80012E64(1);

    if (lbl_804D56D8 == 0)
    {
        if (lbl_804D37A4 != 0)
            fn_8010CCB0();
        else
            fn_8010CDD8();
        GXSetTexCoordGen2(0, 0, 0, 0x21, 0, 0x7D);
        for (i = 0; i < lbl_804D56C8; i++)
        {
            fn_80013F50();
            GXLoadTexObj(lbl_804A3288[i].texObj, 0);
            GXLoadTexMtxImm(lbl_804A3288[i].texMtx, 0x21, 0);
            fn_8001D5DC(lbl_804A3288[i].texId, &lbl_804A3288[i],
                        lbl_804D37A4 != 0 ? &lbl_804A3288[9] : 0);
        }
    }
    else
    {
        fn_800C6200(camera);
        n = 1;
        texIds[0] = lbl_804A3288[0].texId;
        for (i = 1; i < lbl_804D56C8; i++)
        {
            for (j = 0; j < n; j++)
            {
                if (texIds[j] == lbl_804A3288[i].texId)
                    break;
            }
            if (j == n)
                texIds[n++] = lbl_804A3288[i].texId;
        }
        fn_8010F7EC();
        fn_80104134(lbl_804D56C8);
        for (i = 0; i < lbl_804D56C8; i++)
        {
            GXLoadTexObj(lbl_804A3288[i].texObj, i);
            GXLoadTexMtxImm(lbl_804A3288[i].texMtx, 0x1E + i * 3, 0);
        }
        fn_80013F50();
        for (j = 0; j < n; j++)
            fn_8001D5DC(texIds[j], camera, 0);
    }

    fn_8000FBFC(0);
    fn_80012E64(0);
    fn_80013F50();
    fn_8000FF7C();
    {
        u32 kColor = lbl_804D37AC;

        GXSetTevKColor(0, &kColor);
    }
    GXSetZMode(1, 3, 1);
    fn_8000895C(saved);
    GfxResetTev();
}

// One clock proximity emitter: a world position, a live handle and the
// box (three min/max pairs) used to size its trigger radius.
struct ClockProximitySource
{
    u8    unk00[0x24];
    int   x;        // 0x24
    int   y;        // 0x28
    int   z;        // 0x2C
    void* handle;   // 0x30
    u8    unk34[0x8];
    int*  box;      // 0x3C
};

// Camera snapshot fn_800C64AC hands back.
struct ClockProximityCamera
{
    u8    unk00[0x24];
    float x;        // 0x24
    float y;        // 0x28
    float z;        // 0x2C
};

// Event payload; the proximity trigger sits at +0x14.
struct ClockProximityTarget
{
    u8 unk00[0x14];
    u8 trigger;     // 0x14
};

extern float lbl_804DA4E8;
extern float lbl_804DA4EC;
extern float lbl_804DA4F0;
extern "C" ClockProximityCamera* fn_800C64AC();
extern "C" float fn_800E1020(float* v);
extern "C" int fn_800E0DBC(int* v);
extern "C" void fn_80288CE0(void* texId, ClockProximitySource* source,
                            void* trigger, void* target);

// Fade the clock overlay in when the camera comes within reach of a live
// proximity emitter.
void ClockCheckProximityTrigger(ClockProximitySource* source,
                                ClockProximityTarget* target, void* arg)
{
    ClockProximityCamera* camera;
    float offset[3];
    int   extent[3];
    float distance;
    float radius;

    if (lbl_804D37A0 == 0 || source->handle == 0)
        return;

    camera = fn_800C64AC();
    offset[0] = camera->x - (float)source->x;
    offset[1] = camera->y - (float)source->y;
    offset[2] = camera->z - (float)source->z;
    distance = fn_800E1020(offset);

    extent[0] = source->box[3] - source->box[0];
    extent[1] = source->box[4] - source->box[1];
    extent[2] = source->box[5] - source->box[2];
    radius = (float)fn_800E0DBC(extent);

    if (distance < lbl_804DA4E8 + radius)
    {
        lbl_804A3BD8.unk0C = lbl_804DA4EC + radius;
        lbl_804A3BD8.unk10 = lbl_804DA4F0 + radius;
        fn_8000895C(&lbl_804A3BD8);
        fn_80288CE0(arg, source, &target->trigger, target);
    }
}

// Link info block at the head of the clock render state (0x804A3768).
struct ClockLinkState
{
    float basis[3][3]; // 0x00: orientation basis (X, dir, Z)
    Vec3  origin;      // 0x24: view origin the page is offset from
    int   enabled;     // 0x30
    float scale;       // 0x34
    u8    unk38[0x8];
    float r;           // 0x40
    float g;           // 0x44
    float b;           // 0x48
    int   i4C;         // 0x4C
    int   i50;         // 0x50
    int   i54;         // 0x54
    u8    unk58[0x8];
};

// First TEV pass description.
struct ClockTevPass
{
    void* actor;       // 0x00: actor rendered by this pass
    int   enabled;     // 0x04
    u8    unk08[0x8];
    int   i10;         // 0x10
    u8    chan[3];     // 0x14
    u8    alpha;       // 0x17
    int   i18;         // 0x18
    int   i1C;         // 0x1C
    u8    unk20[0x10];
    int   i30;         // 0x30
    u8    unk34[0x8];
    int   i3C;         // 0x3C
    float color[4];    // 0x40
    u8    unk50[0xC];
    float pos[3];      // 0x5C: page origin
    int   i68;         // 0x68
    u8    unk6C[0x1A4];
};

// Second TEV pass description.
struct ClockTevPassB
{
    int   i00;         // 0x00
    u8    b04;         // 0x04
    u8    b05;         // 0x05
    u8    b06;         // 0x06
    u8    b07;         // 0x07
    int   i08;         // 0x08
    int   i0C;         // 0x0C
    u8    unk10[0x10];
    int   i20;         // 0x20
    u8    unk24[0x8];
    int   i2C;         // 0x2C
    float color[4];    // 0x30
    u8    unk40[0xC];
    float f4C;         // 0x4C
    float f50;         // 0x50
    float f54;         // 0x54
    int   i58;         // 0x58
    u8    unk5C[0x1A4];
};

struct ClockRenderState
{
    ClockLinkState link;   // 0x000
    ClockTevPass   passA;  // 0x060
    ClockTevPassB  passB;  // 0x270
};

extern ClockRenderState lbl_804A3768;
extern void* lbl_804D56CC;
extern float lbl_804DA500;
extern float lbl_804DA504;
extern float lbl_804DA508;
extern float lbl_804DA50C;

extern "C" void* fn_800C69B8(void* info, int a, int b);
extern "C" void GXInitTexObj(void* obj, void* image, int width, int height,
                             int format, int wrapS, int wrapT, int mipmap);
extern "C" void GXInitTexObjLOD(void* obj, int minFilter, int magFilter,
                                float minLod, float maxLod, float lodBias,
                                int biasClamp, int edgeLodEnable, int maxAniso);
void LinkInfoInit(void* info);

// Build the clock overlay's texture objects and default TEV state.
void ClockInitRenderState()
{
    int   i;
    float lod;

    LinkInfoInit(&lbl_804A3768.link);
    lbl_804A3768.link.enabled = 0;
    lbl_804D56CC = fn_800C69B8(&lbl_804A3768.link, 0, 0);
    lbl_804A3768.link.scale = lbl_804DA500;
    lbl_804A3768.link.r = lbl_804DA504;
    lbl_804A3768.link.g = lbl_804DA504;
    lbl_804A3768.link.b = lbl_804DA504;
    lbl_804A3768.link.i4C = 0;
    lbl_804A3768.link.i50 = 0;
    lbl_804A3768.link.i54 = 0;

    lod = lbl_804DA508;
    for (i = 0; i < 6; i++)
    {
        GXInitTexObj(lbl_804A3288[i].texObj,
                     lbl_804D56D0 + lbl_804D56D4 * i, 0x100, 0x100,
                     0, 0, 0, 0);
        GXInitTexObjLOD(lbl_804A3288[i].texObj, 1, 1, lod, lod, lod, 0, 0, 0);
    }

    lbl_804A3768.passA.enabled = 1;
    lbl_804A3768.passA.i10 = 0;
    lbl_804A3768.passA.chan[0] = 0;
    lbl_804A3768.passA.chan[1] = 0;
    lbl_804A3768.passA.chan[2] = 0;
    lbl_804A3768.passA.alpha = 0xFF;
    lbl_804A3768.passA.i18 = 0;
    lbl_804A3768.passA.i1C = 0;
    lbl_804A3768.passA.i30 = 0;
    lbl_804A3768.passA.i3C = 0;
    lbl_804A3768.passA.color[0] = lbl_804DA50C;
    lbl_804A3768.passA.color[1] = lbl_804DA50C;
    lbl_804A3768.passA.color[2] = lbl_804DA50C;
    lbl_804A3768.passA.color[3] = lbl_804DA50C;
    lbl_804A3768.passA.i68 = -0x20;

    lbl_804A3768.passB.i00 = 0;
    lbl_804A3768.passB.b04 = 0;
    lbl_804A3768.passB.b05 = 0;
    lbl_804A3768.passB.b06 = 0;
    lbl_804A3768.passB.b07 = 0;
    lbl_804A3768.passB.i08 = 1;
    lbl_804A3768.passB.i0C = 0;
    lbl_804A3768.passB.i20 = 0;
    lbl_804A3768.passB.i2C = 0;
    lbl_804A3768.passB.color[0] = lbl_804DA50C;
    lbl_804A3768.passB.color[1] = lbl_804DA50C;
    lbl_804A3768.passB.color[2] = lbl_804DA50C;
    lbl_804A3768.passB.color[3] = lbl_804DA50C;
    lbl_804A3768.passB.f4C = lbl_804DA508;
    lbl_804A3768.passB.f50 = lbl_804DA508;
    lbl_804A3768.passB.f54 = lbl_804DA50C;
    lbl_804A3768.passB.i58 = -0x20;

    lbl_804A3BD8.enabled = 1;
    lbl_804A3BD8.chan[0] = 0xFF;
    lbl_804A3BD8.chan[1] = 0xFF;
    lbl_804A3BD8.chan[2] = 0xFF;
    lbl_804A3BD8.chan[3] = 0xFF;
    lbl_804A3BD8.unk0C = lbl_804DA4EC;
    lbl_804A3BD8.unk10 = lbl_804DA4F0;
    lbl_804D56C8 = 0;
}

extern "C" u32 GXGetTexBufferSize(u32 width, u32 height, u32 format, int mipmap,
                                  int maxLod);
extern "C" u8* fn_802869DC(int size);

// Allocate the six 0x100x0x100 texture pages the clock overlay uploads.
// fn_80288C9C
extern "C" void fn_80288C9C()
{
    lbl_804D56D4 = GXGetTexBufferSize(0x100, 0x100, 0, 0, 0);
    lbl_804D56D0 = fn_802869DC(lbl_804D56D4 * 6);
}

extern int lbl_804D2568;
extern int lbl_804D256C;
extern int lbl_804D37A8;
extern float lbl_804DA510;
extern float lbl_804DA514;

extern "C" void GXSetViewport(float fx, float fy, float fw, float fh,
                              float nearZ, float farZ);
extern "C" void GXSetScissor(int x, int y, u32 w, u32 h);
extern "C" void GXSetAlphaUpdate(int enable);
extern "C" void GXSetColorUpdate(int enable);
extern "C" void GXSetTexCopySrc(int x, int y, u32 w, u32 h);
extern "C" void GXSetTexCopyDst(u32 w, u32 h, u32 format, int toFb);
extern "C" void GXCopyTex(void* dest, int clear);
extern "C" void* GXGetTexObjData(void* obj);
extern "C" void VMAllocPages(void* page);
extern "C" void fn_8004320C(void* link, void* mtx);
extern "C" void fn_800091D4(void* src, void* src2, void* dst, int flag);
extern "C" void fn_80009848(float scale, void* mtx, void* src);
extern "C" void fn_80009E10();
extern "C" void fn_80009E44(int a, int b);
extern "C" void fn_80093BF0(int r, int g, int b);
extern "C" void fn_80093D18(int r, int g, int b);
extern "C" void fn_800C5E74(int arg);
extern "C" void fn_800C5E9C(int arg);
extern "C" void fn_800C6664(void* handle);
extern "C" void fn_800C6834(void* handle);
extern "C" void fn_8010CF00();
extern "C" void fn_80115F28(int mode);
extern void ActorGetMatrix(Mtx34* mtx, Actor* actor);
extern void ViewPlaneFromNormal(float* plane, const Vec3* normal,
                                const Vec3* point);

// Draw one clock texture page for the given source actor: build its view
// plane and texture matrix, let the actor fill the page, then copy the
// rendered 0x100x0x100 image into the page's texture.
void fn_80288CE0(void* texId, ClockProximitySource* source, void* trigger,
                 void* target)
{
    Mtx34  mtx;
    Mtx34  viewMtx;
    u8     proj[0x24];
    u8     link[0x30];
    Vec3   neg;
    Vec3   scaled;
    Vec3*  p;
    int    kColor;
    int    savedWidth;
    int    savedHeight;

    if (lbl_804D56C8 >= 6)
        return;

    lbl_804A3288[lbl_804D56C8].texId = (u32)texId;

    GXSetViewport(lbl_804DA508, lbl_804DA508, lbl_804DA510, lbl_804DA510,
                  lbl_804DA508, lbl_804DA50C);
    GXSetScissor(0, 0, 0x100, 0x100);

    savedWidth = lbl_804D2568;
    savedHeight = lbl_804D256C;
    lbl_804D2568 = 0x100;
    lbl_804D256C = 0x100;

    ActorGetMatrix(&mtx, (Actor*)source);
    p = (Vec3*)ClockSetupPageBasis(&lbl_804A3768, source, trigger);
    ClockComputeViewTransform(&lbl_804A3768, source, &mtx);

    fn_800C5E74(0);
    fn_800C6834(lbl_804D56CC);
    fn_800C5E9C(0);
    fn_800C6200(&lbl_804A3288[lbl_804D56C8]);

    neg.x = -p->x;
    neg.y = -p->y;
    neg.z = -p->z;
    scaled.x = p->x * lbl_804DA514 + lbl_804A3768.link.origin.x;
    scaled.y = p->y * lbl_804DA514 + lbl_804A3768.link.origin.y;
    scaled.z = p->z * lbl_804DA514 + lbl_804A3768.link.origin.z;

    ViewPlaneFromNormal(lbl_804A3288[lbl_804D56C8].plane, &neg, &scaled);

    lbl_804A3288[lbl_804D56C8].back[0] = -p->x;
    lbl_804A3288[lbl_804D56C8].back[1] = -p->y;
    lbl_804A3288[lbl_804D56C8].back[2] = -p->z;

    fn_80093BF0(0xFF, 0xFF, 0xFF);
    LinkInfoInit(link);
    fn_8004320C(link, &viewMtx);
    fn_800091D4(&viewMtx, &viewMtx, proj, 1);
    fn_80009848(lbl_804DA508, lbl_804A3288[lbl_804D56C8].texMtx, &viewMtx);

    lbl_804A3768.passA.actor = source;
    lbl_804A3768.passA.pos[0] = p->x;
    lbl_804A3768.passA.pos[1] = p->y;
    lbl_804A3768.passA.pos[2] = p->z;

    fn_8000FF28(1);
    fn_801D8A68(1);
    kColor = lbl_804D37A8;
    GXSetTevKColor(0, &kColor);
    fn_8010CF00();
    if (((int (*)(int, void*))source->handle)(10, &lbl_804A3768.passA) == 0)
        ((int (*)(int, void*))source->handle)(0, &lbl_804A3768.passA);

    fn_80009E10();
    fn_8000FF28(0);
    fn_80093D18(0xFF, 0xFF, 0xFF);
    fn_800C6664(lbl_804D56CC);
    fn_800C5E9C(0);
    fn_80115F28(2);
    GXSetAlphaUpdate(1);
    GXSetColorUpdate(1);
    GXSetTexCopySrc(0, 0, 0x100, 0x100);
    GXSetTexCopyDst(0x100, 0x100, 0x20, 0);
    GXSetZMode(1, 7, 1);
    VMAllocPages(lbl_804D56D0 + lbl_804D56D4 * lbl_804D56C8);
    GXCopyTex(GXGetTexObjData(lbl_804A3288[lbl_804D56C8].texObj), 1);

    lbl_804D2568 = savedWidth;
    lbl_804D256C = savedHeight;
    fn_80009E44(0, 0);
    lbl_804D56C8 = lbl_804D56C8 + 1;
}

// Reference directions the page angles are measured from.
extern float lbl_8034B478[3];
extern float lbl_8034B484[3];
extern float lbl_8034B490[3];
extern u8    lbl_80449068[];
extern float lbl_804A3BEC[3];
extern float lbl_804DA518;
extern float lbl_804DA51C;
extern float lbl_804DA520;
extern float lbl_804DA524;
extern float lbl_804DA528;
extern float lbl_804DA52C;
extern float lbl_804DA530;
extern float lbl_804DA534;

extern "C" void fn_800A0094(void* dst, void* src);
extern "C" void fn_800A0100(void* placement);
extern "C" double acos(double x);

// Actor whose bounding box is projected into the clock page's frame.
struct ClockBoxSource
{
    u8  unk00[0x40];
    int* box;           // 0x40: {minX, minY, minZ, maxX, maxY, maxZ}
};

// Placement applied to the box corners: a 3x3 basis followed by its origin.
struct ClockPagePlacement
{
    float basis[3][3];  // 0x00
    float origin[3];    // 0x24
};

// 4x3 page matrix: three basis rows and the origin row.
struct ClockPageMatrix
{
    float row[4][3];
};

// Normalise a direction in place; a near-zero vector collapses to zero.
static inline void ClockNormalize3(float* v)
{
    float x = v[0];
    float y = v[1];
    float z = v[2];
    float inv;

    if (fabs(x) <= lbl_804DA51C && fabs(y) <= lbl_804DA51C &&
        fabs(z) <= lbl_804DA51C)
    {
        v[0] = lbl_804DA508;
        v[1] = lbl_804DA508;
        v[2] = lbl_804DA508;
        return;
    }
    inv = 1.0f / sqrtf(x * x + y * y + z * z);
    v[0] = x * inv;
    v[1] = y * inv;
    v[2] = z * inv;
}

// Project the actor's bounding box into the page's local frame, then derive
// the page scale from the two extreme directions across the box.
void ClockComputeViewTransform(void* state_, void* source_, void* placement_)
{
    ClockRenderState*   state = (ClockRenderState*)state_;
    ClockBoxSource*     source = (ClockBoxSource*)source_;
    ClockPagePlacement* placement = (ClockPagePlacement*)placement_;
    Vec3  corners[8];
    float ref[3];
    float dirA[3];
    float dirB[3];
    float xmin;
    float xmax;
    float ymax;
    float zmin;
    float zmax;
    float angleA;
    float angleB;
    int*  box;
    int   i;

    box = source->box;
    corners[0].x = (float)box[0];
    corners[0].y = (float)box[1];
    corners[0].z = (float)box[2];
    corners[1].x = (float)box[3];
    corners[1].y = (float)box[1];
    corners[1].z = (float)box[2];
    corners[2].x = (float)box[3];
    corners[2].y = (float)box[4];
    corners[2].z = (float)box[2];
    corners[3].x = (float)box[0];
    corners[3].y = (float)box[4];
    corners[3].z = (float)box[2];
    corners[4].x = (float)box[0];
    corners[4].y = (float)box[1];
    corners[4].z = (float)box[5];
    corners[5].x = (float)box[3];
    corners[5].y = (float)box[1];
    corners[5].z = (float)box[5];
    corners[6].x = (float)box[3];
    corners[6].y = (float)box[4];
    corners[6].z = (float)box[5];
    corners[7].x = (float)box[0];
    corners[7].y = (float)box[4];
    corners[7].z = (float)box[5];

    fn_800A0100(placement);
    for (i = 0; i < 8; i++)
    {
        fn_800A0094(&corners[i], &corners[i]);
        corners[i].x += placement->origin[0];
        corners[i].y += placement->origin[1];
        corners[i].z += placement->origin[2];
    }

    *(ClockPageMatrix*)lbl_80449068 = *(ClockPageMatrix*)state;

    for (i = 0; i < 8; i++)
    {
        corners[i].x -= state->link.origin.x;
        corners[i].y -= state->link.origin.y;
        corners[i].z -= state->link.origin.z;
        fn_800A0094(&corners[i], &corners[i]);
    }

    xmin = corners[0].x;
    xmax = corners[0].x;
    ymax = corners[0].y;
    zmin = corners[0].z;
    zmax = corners[0].z;
    for (i = 1; i < 8; i++)
    {
        if (ymax <= corners[i].y)
            ymax = corners[i].y;
        if (xmin >= corners[i].x)
            xmin = corners[i].x;
        if (xmax <= corners[i].x)
            xmax = corners[i].x;
        if (zmin >= corners[i].z)
            zmin = corners[i].z;
        if (zmax <= corners[i].z)
            zmax = corners[i].z;
    }

    xmin -= lbl_804DA518;
    xmax += lbl_804DA518;
    ymax += lbl_804DA518;
    zmin -= lbl_804DA518;
    zmax += lbl_804DA518;

    ref[0] = lbl_8034B478[0];
    ref[1] = lbl_8034B478[1];
    ref[2] = lbl_8034B478[2];

    dirA[0] = (xmax > -xmin) ? xmax : -xmin;
    dirA[1] = ymax;
    dirA[2] = lbl_8034B484[2];

    dirB[0] = lbl_8034B490[0];
    dirB[1] = ymax;
    dirB[2] = (zmax > -zmin) ? zmax : -zmin;

    ClockNormalize3(dirA);
    if (1.0f - dirA[1] >= lbl_804DA51C)
        angleA = (float)acos(ref[1] * dirA[1] + ref[0] * dirA[0] +
                             ref[2] * dirA[2]) * lbl_804DA524;
    else
        angleA = lbl_804DA520;

    ClockNormalize3(dirB);
    if (1.0f - dirB[1] >= lbl_804DA51C)
        angleB = (float)acos(ref[1] * dirB[1] + ref[0] * dirB[0] +
                             ref[2] * dirB[2]) * lbl_804DA524;
    else
        angleB = lbl_804DA520;

    state->link.scale = lbl_804DA528 * (angleA > angleB ? angleA : angleB);
}

// Raw integer view of the page source used to build the page basis.
struct ClockPageSource
{
    u8  unk00[0xC];
    int extent[3];   // 0x0C
    u8  unk18[0xC];
    int pos[3];      // 0x24
};

// Build the page's orientation basis from the source's extents and a view
// direction, then offset the page origin along that direction.
float* ClockSetupPageBasis(void* out_, void* source_, void* dir_)
{
    ClockPageSource* source = (ClockPageSource*)source_;
    float*           dir = (float*)dir_;
    float bx;
    float by;
    float bz;
    float ax;
    float ay;
    float az;
    float cx;
    float cy;
    float cz;
    float inv;
    float offset;

    bx = (float)source->extent[0] * lbl_804DA52C;
    by = (float)source->extent[1] * lbl_804DA52C;
    bz = (float)source->extent[2] * lbl_804DA52C;

    if (dir[0] == lbl_804DA508 && dir[1] == lbl_804DA508 &&
        dir[2] == lbl_804DA508)
    {
        lbl_804A3BEC[0] = lbl_804DA508;
        lbl_804A3BEC[1] = lbl_804DA508;
        lbl_804A3BEC[2] = lbl_804DA530;
    }
    else if (fabs(dir[0]) <= lbl_804DA51C && fabs(dir[1]) <= lbl_804DA51C &&
             fabs(dir[2]) <= lbl_804DA51C)
    {
        lbl_804A3BEC[0] = lbl_804DA508;
        lbl_804A3BEC[1] = lbl_804DA508;
        lbl_804A3BEC[2] = lbl_804DA508;
    }
    else
    {
        ax = dir[0];
        ay = dir[1];
        az = dir[2];
        inv = 1.0f / sqrtf(ax * ax + ay * ay + az * az);
        lbl_804A3BEC[0] = ax * inv;
        lbl_804A3BEC[1] = ay * inv;
        lbl_804A3BEC[2] = az * inv;
    }

    ax = lbl_804A3BEC[0];
    ay = lbl_804A3BEC[1];
    az = lbl_804A3BEC[2];

    cx = ay * bz - az * by;
    cy = az * bx - ax * bz;
    cz = ax * by - ay * bx;

    lbl_804A3768.link.basis[0][0] = cx;
    lbl_804A3768.link.basis[0][1] = cy;
    lbl_804A3768.link.basis[0][2] = cz;
    lbl_804A3768.link.basis[1][0] = ax;
    lbl_804A3768.link.basis[1][1] = ay;
    lbl_804A3768.link.basis[1][2] = az;
    lbl_804A3768.link.basis[2][0] = cy * az - cz * ay;
    lbl_804A3768.link.basis[2][1] = cz * ax - cx * az;
    lbl_804A3768.link.basis[2][2] = cx * ay - cy * ax;

    offset = lbl_804DA534;
    ((ClockRenderState*)out_)->link.origin.x = (float)source->pos[0] + ax * offset;
    ((ClockRenderState*)out_)->link.origin.y = (float)source->pos[1] + ay * offset;
    ((ClockRenderState*)out_)->link.origin.z = (float)source->pos[2] + az * offset;

    return lbl_804A3BEC;
}

// ---- particle page emission ------------------------------------------------

// One ring of particle transforms around a page centre (stride 0x128).
struct ClockParticleGroup
{
    u8    unk00[0x64];
    int   sideCount;    // 0x64
    u8    unk68[0x50];
    int   vertexCount;  // 0xB8
    u8    unkBC[0x10];
    float radius;       // 0xCC
    u8    unkD0[0x58];
};

// Particle groups belonging to one material class (stride 0x258).
struct ClockParticleTable
{
    float              unk00;       // 0x00
    int                groupCount;  // 0x04
    ClockParticleGroup groups[2];   // 0x08
};

// One of the 64 particle draw transforms (stride 0x24).
struct ClockParticleDrawSlot
{
    float mtx[8];   // 0x00
    int   sample;   // 0x20
};

// Per-side particle sample id (stride 8).
struct ClockSideSample
{
    u32   asset;    // 0x00: asset id the side's texture is found by
    short sample;   // 0x04
    u8    unk06[2];
};

extern ClockParticleTable    lbl_803C0D90[];
extern ClockSideSample       lbl_803C0D78[];
extern ClockParticleDrawSlot lbl_804A3BF8[];
extern int                   lbl_804D37B0;
extern int                   lbl_804D56F4;
extern float                 lbl_804D56F8;
extern signed char           lbl_804D56FC;
extern float                 lbl_804DA548;
extern float                 lbl_804DA54C;
extern double                lbl_804DA550;
extern float                 lbl_804DA558;
extern float                 lbl_804DA55C;

extern "C" int fn_800C5B6C(float distance, void* pos);
void PerturbDirectionRandom(float* out, float* dir, float amount);
void SetParticleDrawTransform(float scale, float radius, float* mtx,
                              float* pos, float* dir);

// Emit the particle transforms for one page: the type byte selects the
// material class's particle-group table, dir is the view direction the
// transforms are fanned around. Type 0x8A is not handled by the original.
int ClockEmitParticlePage(float* pos, float* dir, u8 type)
{
    ClockParticleTable*    table;
    ClockParticleGroup*    group;
    ClockParticleDrawSlot* slot;
    float v[3];
    int   idx;
    int   i;
    int   j;
    short side;

    if (lbl_804D37B0 == 0)
        return 0;

    if (lbl_804D56FC == 0)
    {
        lbl_804D56FC = 1;
        lbl_804D56F8 = lbl_804DA548;
    }

    if (fn_800C5B6C(lbl_804D56F8, pos) == 0)
        return 0;

    if (type == 0x89)
        idx = 8;
    else if (type >= 0x89)
    {
        if (type == 0x8B)
            idx = 1;
        else if (type > 0x8B)
            idx = 2;
    }
    else if (type >= 0x83)
    {
        if (type >= 0x86)
            idx = 7;
        else
            idx = 2;
    }
    else if (type >= 0x81)
        idx = 1;
    else
        idx = 2;

    table = &lbl_803C0D90[idx];
    group = table->groups;
    for (i = 0; i != table->groupCount; i++, group++)
    {
        side = 0;
        for (j = 0; j != group->vertexCount; j++)
        {
            int sample = -1;

            if (side >= 0 && (unsigned)side < 3)
                sample = lbl_803C0D78[side].sample;

            PerturbDirectionRandom(v, dir, lbl_804DA54C);
            if (v[0] * dir[0] + v[1] * dir[1] + v[2] * dir[2] < lbl_804DA550)
            {
                v[0] = -v[0];
                v[1] = -v[1];
                v[2] = -v[2];
            }

            slot = &lbl_804A3BF8[lbl_804D56F4 % 64];
            slot->sample = sample;
            SetParticleDrawTransform(lbl_804DA558, lbl_804DA55C * group->radius,
                                     slot->mtx, pos, v);
            lbl_804D56F4 = lbl_804D56F4 + 1;
            side = (short)((side + 1) % group->sideCount);
        }
    }
    return 1;
}

// Convert two integer position triples to float and emit a page for them.
int ClockEmitParticlePageFromInts(int* a, int* b, u8 type)
{
    float va[3];
    float vb[3];

    if (lbl_804D37B0 == 0)
        return 0;

    va[0] = (float)a[0];
    va[1] = (float)a[1];
    va[2] = (float)a[2];
    vb[0] = (float)b[0];
    vb[1] = (float)b[1];
    vb[2] = (float)b[2];

    return ClockEmitParticlePage(va, vb, type);
}

// List of live particle pages the clock keeps; new pages are appended here.
struct ClockPageList
{
    u8  unk00[0xC];
    int count;      // 0x0C
};

extern ClockPageList* lbl_804D56E0;
extern float          lbl_804D56E4;
extern signed char    lbl_804D56E8;
extern float          lbl_804D56EC;
extern signed char    lbl_804D56F0;

void* VAlloc(int size);
void BasisFromVecs(float* out, Vec3* a, Vec3* b, Vec3* c);
void BoneSetMatrix(void* dst, Mtx34* src);
extern "C" void* fn_8028B3BC(void* owner, int index, int size);
int ClockBuildPageEmitters(void* page, int index, float* pos, void* dir,
                           float scale);

// Build a particle page from two integer position triples.
// Other units call this by its placeholder name, so the symbol stays fn_80289CEC.
extern "C" int fn_80289CEC(int* a, int* b)
{
    float pa[3];
    float pb[3];
    Vec3  neg;
    Mtx34 basis;
    u8    boneMtx[0x10];
    void* handle;
    int*  slot;

    if (lbl_804D37B0 == 0)
        return 0;

    pa[0] = (float)a[0];
    pa[1] = (float)a[1];
    pa[2] = (float)a[2];
    pb[0] = (float)b[0];
    pb[1] = (float)b[1];
    pb[2] = (float)b[2];

    if (lbl_804D56F0 == 0)
    {
        lbl_804D56F0 = 1;
        lbl_804D56EC = lbl_804DA548;
    }

    if (fn_800C5B6C(lbl_804D56EC, pa) == 0)
        return 0;

    handle = VAlloc(0x68);
    slot = (int*)fn_8028B3BC(lbl_804D56E0, lbl_804D56E0->count, 4);
    *slot = (int)handle;

    if (lbl_804D37B0 == 0)
        return 0;
    if (handle == 0)
        return 0;

    neg.x = -pb[0];
    neg.y = -pb[1];
    neg.z = -pb[2];
    BasisFromVecs(&basis.m[0][0], 0, 0, &neg);
    BoneSetMatrix(boneMtx, &basis);
    ClockBuildPageEmitters(handle, 0xb, pa, boneMtx, lbl_804DA558);
    return 1;
}

// Build a particle page from two integer position triples; the type byte
// selects which effect the new page is emitted for.
// Other units call this by its placeholder name, so the symbol stays fn_80289EB4.
extern "C" int fn_80289EB4(int* a, int* b, u8 type)
{
    float pa[3];
    float pb[3];
    Vec3  neg;
    Mtx34 basis;
    u8    boneMtx[0x10];
    void* handle;
    int*  slot;

    if (lbl_804D37B0 == 0)
        return 0;

    pa[0] = (float)a[0];
    pa[1] = (float)a[1];
    pa[2] = (float)a[2];
    pb[0] = (float)b[0];
    pb[1] = (float)b[1];
    pb[2] = (float)b[2];

    if (lbl_804D56E8 == 0)
    {
        lbl_804D56E8 = 1;
        lbl_804D56E4 = lbl_804DA548;
    }

    if (fn_800C5B6C(lbl_804D56E4, pa) == 0)
        return 0;

    handle = VAlloc(0x68);
    slot = (int*)fn_8028B3BC(lbl_804D56E0, lbl_804D56E0->count, 4);
    *slot = (int)handle;

    if (lbl_804D37B0 == 0)
        return 0;
    if (handle == 0)
        return 0;

    neg.x = -pb[0];
    neg.y = -pb[1];
    neg.z = -pb[2];
    BasisFromVecs(&basis.m[0][0], 0, 0, &neg);
    BoneSetMatrix(boneMtx, &basis);

    if (type == 0x89)
    {
        ClockBuildPageEmitters(handle, 8, pa, boneMtx, lbl_804DA558);
        return 1;
    }
    if (type < 0x89)
    {
        if (type > 0x82)
        {
            if (type < 0x86)
            {
                ClockBuildPageEntries(handle,2, pa, boneMtx);
                return 1;
            }
            ClockBuildPageEntries(handle,7, pa, boneMtx);
            return 1;
        }
        if (type > 0x80)
        {
            ClockBuildPageEntries(handle,1, pa, boneMtx);
            return 1;
        }
    }
    else if (type < 0x8c)
    {
        return 1;
    }
    ClockBuildPageEntries(handle,2, pa, boneMtx);
    return 1;
}

// First 0x2C bytes of a page entry, copied verbatim from a particle group.
struct ClockPageEntryCommon
{
    float pos[3];      // 0x00
    float unk0C;       // 0x0C
    float unk10;       // 0x10
    float unk14;       // 0x14
    float unk18;       // 0x18
    int   unk1C;       // 0x1C
    float radius;      // 0x20
    float unk24;       // 0x24
    int   unk28;       // 0x28
};

// One 0x30-byte page entry inside the 0x68-byte page handle.
struct ClockPageEntry
{
    ClockPageEntryCommon common;  // 0x00
    int                  texId;   // 0x2C
};

struct ClockPageHandle
{
    float          unk00;      // 0x00
    int            count;      // 0x04
    ClockPageEntry entries[2]; // 0x08
};

// Draw parameters handed to the page renderer (0xEC bytes).
struct ClockPageEmitParams
{
    Vec3  pos;       // 0x00
    Vec3  dir;       // 0x0C
    float dirZ;      // 0x18
    u8    unk1C[0x18];
    struct
    {
        int sample;  // 0x00
        int pad;     // 0x04
    } sides[8];      // 0x34
    u8    unk74[0x2C];
    void* entry;     // 0xA0
    u8    unkA4[0x48];
};

extern float lbl_804DA560;
extern "C" int fn_800D3D88(void* params);

// Emit the page entries for one particle-group table: build the renderer's
// draw parameters for every group and record the texture it hands back.
int ClockBuildPageEntries(void* handle_, int index, float* pos, void* dir_)
{
    float*              dir = (float*)dir_;
    ClockPageHandle*    handle = (ClockPageHandle*)handle_;
    u8*                 table = (u8*)&lbl_803C0D90[index];
    u8*                 group = table + 8;
    ClockPageEmitParams params;
    int                 i;
    int                 j;

    if (lbl_804D37B0 == 0)
        return 0;

    memset(handle, 0, 0x68);
    handle->unk00 = *(float*)table;

    for (i = 0; i != *(int*)(table + 4); i++, group += 0x128)
    {
        ClockPageEntry* entry = &handle->entries[handle->count];
        int count;

        entry->common = *(ClockPageEntryCommon*)group;
        entry->common.pos[0] = pos[0];
        entry->common.pos[1] = pos[1];
        entry->common.pos[2] = pos[2];
        entry->common.unk0C = lbl_804DA560;
        entry->common.unk10 = lbl_804DA558;
        entry->common.unk14 = lbl_804DA558;
        entry->common.unk18 = lbl_804DA558;

        params = *(ClockPageEmitParams*)(group + 0x38);
        params.pos.x = pos[0];
        params.pos.y = pos[1];
        params.pos.z = pos[2];
        params.dir.x = dir[0];
        params.dir.y = dir[1];
        params.dir.z = dir[2];
        params.dirZ = dir[3];

        count = *(int*)(group + 0x64);
        for (j = 0; j != count; j++)
        {
            short id = *(short*)(group + 0x2C + j * 4);
            int   sample = -1;

            if (id >= 0 && id < 3)
                sample = lbl_803C0D78[id].sample;
            params.sides[j].sample = sample;
        }

        if (*(int*)(group + 0xDC) != 0)
            params.entry = entry;

        entry->texId = fn_800D3D88(&params);
        if (entry->texId != -1)
            handle->count++;
    }
    return 1;
}

// One emitter list element in the clock page store (stride 0x30).
struct ClockPageEmitterNode
{
    float time;      // 0x00: remaining lifetime
    int   count;     // 0x04
    u8    unk08[0x2C];
    int   emitter;   // 0x34
};

struct ClockPageStoreVtbl
{
    void* unk00;                                  // 0x00
    void* unk04;                                  // 0x04
    void (*release)(void* self, int flags);       // 0x08
};

// Layout of the page store at 0x804D56E0: a vector of emitter lists.
struct ClockPageStore
{
    ClockPageStoreVtbl*    vtbl;      // 0x00
    u8                     unk04[4];
    u32                    usedBytes; // 0x08
    ClockPageEmitterNode** lists;     // 0x0C
};

void VFree(void* p);
void EmitterDestroy(int emitter);
extern "C" void fn_8028B35C(void* store, void* element, int size);

// Free every emitter list the particle page store still owns, then release
// the store itself.
// Other units call this by its placeholder name, so the symbol stays fn_8028A388.
extern "C" void fn_8028A388(void)
{
    while ((((ClockPageStore*)lbl_804D56E0)->usedBytes >> 2) != 0)
    {
        ClockPageStore*        store = (ClockPageStore*)lbl_804D56E0;
        ClockPageEmitterNode** lists = store->lists;
        ClockPageEmitterNode*  node = (ClockPageEmitterNode*)lists[0];
        int                    i;

        for (i = 0; i != node->count; i++)
        {
            if (node[i].emitter != -1)
                EmitterDestroy(node[i].emitter);
        }
        VFree(lists[0]);
        fn_8028B35C(store, lists, 4);
    }
    if (lbl_804D56E0 != 0)
    {
        ClockPageStore* store = (ClockPageStore*)lbl_804D56E0;

        store->vtbl->release(store, 1);
    }
}

// Vtable data of the clock page-buffer class (0x803C29D0) and of its
// intermediate base class (0x803C29B0).
extern u8 lbl_803C29D0[];
extern u8 lbl_803C29B0[];

// Releases the payload block owned by the page buffer's base class.
extern "C" void fn_8028B960(void* self);

// Engine pooled byte buffer; only its destructor (0x8028B9C0) is needed here.
// It is invoked non-virtually, the way a derived destructor calls its base.
struct Buffer
{
    Buffer();
    ~Buffer();
};

// Placement form of new: construction happens in memory from VAlloc.
inline void* operator new(unsigned long, void* ptr)
{
    return ptr;
}

// Destructor of the clock page buffer: restores the class and base-class
// vtables, releases the payload, then returns the storage to the VM heap when
// the compiler-generated delete flag is set.
void* ClockPageBufferDestroy(void* self_, short flags)
{
    u8** self = (u8**)self_;

    if (self != 0)
    {
        *self = lbl_803C29D0;
        if (self != 0)
        {
            *self = lbl_803C29B0;
            fn_8028B960(self);
            ((Buffer*)self)->~Buffer();
        }
        if (flags > 0)
            VFree(self);
    }
    return self;
}

extern float lbl_804DA538;

void DrawParticlePoints(float* mtx, int stride, int count, int sample,
                        void* data, u8 r, u8 g, u8 b, u8 a, float scale);

// Draw every resident particle transform through the particle point renderer.
// fn_8028A4BC
extern "C" void fn_8028A4BC()
{
    int i;

    for (i = 0; i != (lbl_804D56F4 < 0x40 ? lbl_804D56F4 : 0x40); i++)
        DrawParticlePoints(lbl_804A3BF8[i].mtx, 0x24, 1, lbl_804A3BF8[i].sample,
                           0, 0xFF, 0xFF, 0xFF, 0xFF, lbl_804DA538);
}

// Three-float block handed to the particle stepper along with the slots.
extern Vec3 lbl_8034B4A0;

extern "C" void fn_802113F0(float dt, float* slots, int stride, int count,
                            void* range);
void ParticleStepPlain(float dt, float* slots, int stride, int count);

// Age the live particle pages: a page whose timer ran out is destroyed and
// removed from the store, the rest are stepped one frame.
// Other units call this by its placeholder name, so the symbol stays fn_8028A554.
extern "C" void fn_8028A554(float dt)
{
    ClockPageStore*        store = (ClockPageStore*)lbl_804D56E0;
    ClockPageEmitterNode** list = store->lists;
    Vec3                   range;

    while (list != (ClockPageEmitterNode**)((u8*)store->lists + store->usedBytes))
    {
        ClockPageEmitterNode* node = *list;
        int                   i;

        if (node->time < dt)
        {
            for (i = 0; i != node->count; i++)
            {
                if (node[i].emitter != -1)
                    EmitterDestroy(node[i].emitter);
            }
            VFree(node);
            fn_8028B35C(store, list, 4);
        }
        else
        {
            node->time = node->time - dt;
            list++;
        }
    }

    range = lbl_8034B4A0;
    fn_802113F0(dt, (float*)lbl_804A3BF8, 0x24,
                lbl_804D56F4 < 0x40 ? lbl_804D56F4 : 0x40, &range);
    ParticleStepPlain(dt, (float*)lbl_804A3BF8, 0x24,
                      lbl_804D56F4 < 0x40 ? lbl_804D56F4 : 0x40);
}

unsigned long AssetFind(unsigned long type, unsigned long id);
void TextureLoad(short* dst, int mode);

// Resolve and load the three side textures the clock particle pages sample.
// Other units call this by its placeholder name, so the symbol stays fn_8028A6B8.
extern "C" void fn_8028A6B8()
{
    ClockSideSample* side = lbl_803C0D78;
    int              i;

    for (i = 0; i != 3; i++, side++)
    {
        side->sample = (short)AssetFind(0xF2E2BEB8, side->asset);
        TextureLoad(&side->sample, 1);
    }
}

// Creates the clock page buffer singleton (placeholder name fn_8028A728, still
// called by other units): allocates its storage, runs the Buffer base
// constructor, installs the class and base-class vtables and publishes the
// pointer.
extern "C" void fn_8028A728(void)
{
    u8** buf = (u8**)VAlloc(0x14);

    if (buf != 0)
    {
        new ((Buffer*)buf) Buffer();
        buf[0] = lbl_803C29B0;
        buf[0] = lbl_803C29D0;
    }
    lbl_804D56E0 = (ClockPageList*)buf;
}

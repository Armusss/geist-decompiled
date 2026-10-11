// Image.cpp: TPL texture files and full-screen 2D images (0x800F2B08-0x800F396C).
// - A local copy of the SDK's TPLBind (with its assert) fixes up a TPL file loaded from disc.
// - Helpers draw a TPL or raw texture as a screen-space quad (splash screens, loading pictures).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <dolphin/dvd.h>

typedef struct TPLHeader {
    u16 height;
    u16 width;
    u32 format;
    char* data;
    GXTexWrapMode wrapS;
    GXTexWrapMode wrapT;
    GXTexFilter minFilter;
    GXTexFilter magFilter;
    f32 LODBias;
    u8 edgeLODEnable;
    u8 minLOD;
    u8 maxLOD;
    u8 unpacked;
} TPLHeader;

typedef struct TPLClutHeader {
    u16 numEntries;
    u8 unpacked;
    u8 pad8;
    GXTlutFmt format;
    char* data;
} TPLClutHeader;

typedef struct TPLDescriptor {
    TPLHeader* textureHeader;
    TPLClutHeader* CLUTHeader;
} TPLDescriptor;

typedef struct TPLPalette {
    u32 versionNumber;
    u32 numDescriptors;
    TPLDescriptor* descriptorArray;
} TPLPalette;

// A raw (non-TPL) texture: size and a format code (0 RGBA8, 1 CMPR, 2 IA8, 3 I4).
struct ImageInfo {
    int width;
    int height;
    int type;
};

int FileReadSync(void* file, void* dst, int size, int offset);   // 0x80007BC0
void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);

void GfxLoadTexObj(GXTexObj* obj, int map);   // 0x8000F944
void GfxSetVtxDesc(int desc);                 // 0x80008DAC
void GfxSetMaterial(void* material);          // 0x80112AB0
void GfxResetTev(void);                       // 0x8010D130
void GfxSetBlendMode(int type, int src, int dst, int op); // 0x801D8A10
void GfxSetCullMode(int mode);                // 0x80008B44
void GfxFrameEnd(void);                       // 0x8000ADDC
void GfxFrameSwap(void);                      // 0x8000A028
void GfxFrameBegin(void);                     // 0x80009EE0

extern OSMutex gDVDMutex;                     // 0x803D7114
extern int gWideScreen;                       // 0x804D4008

#define TplVersion 2142000

inline void TPLBind(TPLPalette* pal) {
    u16 i;

    if (!(pal->versionNumber == TplVersion)) {
        OSPanic("Image.cpp", 337, "invalid version number for texture palette");
    }

    pal->descriptorArray = (TPLDescriptor*)((u32)pal->descriptorArray + (u32)pal);

    for (i = 0; i < pal->numDescriptors; i++) {
        if (pal->descriptorArray[i].textureHeader) {
            pal->descriptorArray[i].textureHeader =
                (TPLHeader*)((u32)pal + (u32)pal->descriptorArray[i].textureHeader);
            if (!pal->descriptorArray[i].textureHeader->unpacked) {
                pal->descriptorArray[i].textureHeader->data =
                    (char*)((u32)pal + (u32)pal->descriptorArray[i].textureHeader->data);
                pal->descriptorArray[i].textureHeader->unpacked = 1;
            }
        }
        if (pal->descriptorArray[i].CLUTHeader) {
            pal->descriptorArray[i].CLUTHeader =
                (TPLClutHeader*)((u32)pal + (u32)pal->descriptorArray[i].CLUTHeader);
            if (!pal->descriptorArray[i].CLUTHeader->unpacked) {
                pal->descriptorArray[i].CLUTHeader->data =
                    (char*)((u32)pal + (u32)pal->descriptorArray[i].CLUTHeader->data);
                pal->descriptorArray[i].CLUTHeader->unpacked = 1;
            }
        }
    }
}

inline void ImageQuad(f32 x0, f32 y0, f32 x1, f32 y1, u8 r, u8 g, u8 b, u8 a) {
    GXBegin(GX_QUADS, GX_VTXFMT2, 4);
    GXPosition3f32(x0, y0, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x0, y1, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(0.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x1, y1, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(1.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x1, y0, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(1.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
}

void ImageDrawTPL(TPLPalette* pal, u8 alpha) {
    TPLHeader* header;
    GXTexObj obj;
    Mtx44 proj;
    f32 x0, x1, y0, y1;

    if (pal == 0 || pal->numDescriptors == 0) {
        return;
    }

    header = pal->descriptorArray[0].textureHeader;
    GXInitTexObj(&obj, header->data, header->width, header->height, (GXTexFmt)header->format, GX_CLAMP, GX_CLAMP,
                 GX_FALSE);
    GfxLoadTexObj(&obj, 0);

    if (gWideScreen) {
        C_MTXOrtho(proj, 0.0f, 480.0f, -107.0f, 747.0f, -1.0f, 1.0f);
    } else {
        C_MTXOrtho(proj, 0.0f, 480.0f, 0.0f, 640.0f, -1.0f, 1.0f);
    }
    GXSetProjection(proj, GX_ORTHOGRAPHIC);

    GfxSetVtxDesc(2);
    GfxSetMaterial(0);
    GfxResetTev();
    GfxSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GfxSetCullMode(GX_CULL_NONE);

    x0 = (640 - header->width) / 2.0f;
    x1 = 640.0f - x0;
    y0 = (480 - header->height) / 2.0f;
    y1 = 480.0f - y0;

    ImageQuad(x0, y0, x1, y1, 255, 255, 255, alpha);
}

void ImageFreeTPL(TPLPalette** pal) {
    MemFree(*pal, "Image.cpp", 447);
    *pal = 0;
}

int ImageReadTPL(TPLPalette** pal, const char* name) {
    DVDFileInfo fi;

    OSLockMutex(&gDVDMutex);
    if (!DVDOpen(name, &fi) || fi.length == 0) {
        OSUnlockMutex(&gDVDMutex);
        return 0;
    }
    OSUnlockMutex(&gDVDMutex);

    DCInvalidateRange(*pal, (fi.length + 31) & ~31);
    FileReadSync(&fi, *pal, (fi.length + 31) & ~31, 0);
    OSLockMutex(&gDVDMutex);
    DVDClose(&fi);
    OSUnlockMutex(&gDVDMutex);
    DCFlushRange(*pal, (fi.length + 31) & ~31);

    TPLBind(*pal);
    return 1;
}

int ImageLoadTPL(TPLPalette** pal, const char* name) {
    DVDFileInfo fi;

    OSLockMutex(&gDVDMutex);
    if (!DVDOpen(name, &fi) || fi.length == 0) {
        OSUnlockMutex(&gDVDMutex);
        return 0;
    }
    OSUnlockMutex(&gDVDMutex);

    *pal = (TPLPalette*)MemAlloc((fi.length + 31) & ~31, "Image.cpp", 383);
    DCInvalidateRange(*pal, (fi.length + 31) & ~31);
    FileReadSync(&fi, *pal, (fi.length + 31) & ~31, 0);
    OSLockMutex(&gDVDMutex);
    DVDClose(&fi);
    OSUnlockMutex(&gDVDMutex);
    DCFlushRange(*pal, (fi.length + 31) & ~31);

    TPLBind(*pal);
    return 1;
}

void ImageSetPixelFmtRGBA6(void) {
    GfxFrameEnd();
    GfxFrameSwap();
    GfxFrameBegin();
    GXSetPixelFmt(GX_PF_RGBA6_Z24, GX_ZC_LINEAR);
    GfxFrameEnd();
    GfxFrameSwap();
    GfxFrameBegin();
}

void ImageSetPixelFmtRGB8(void) {
    GfxFrameEnd();
    GfxFrameSwap();
    GfxFrameBegin();
    GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    GfxFrameEnd();
    GfxFrameSwap();
    GfxFrameBegin();
}

void ImageDrawRaw(ImageInfo* info, void* data, u8 a, u8 r, u8 g, u8 b, f32 x0, f32 x1, f32 y0, f32 y1) {
    GXTexObj obj;
    Mtx44 proj;
    GXTexFmt fmt = GX_TF_CMPR;

    switch (info->type) {
    case 0:
        fmt = GX_TF_RGBA8;
        break;
    case 1:
        fmt = GX_TF_CMPR;
        break;
    case 2:
        fmt = GX_TF_IA8;
        break;
    }

    GfxSetVtxDesc(2);
    GfxSetMaterial(0);
    GfxResetTev();
    GfxSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GfxSetCullMode(GX_CULL_NONE);

    GXInitTexObj(&obj, data, info->width, info->height, fmt, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GfxLoadTexObj(&obj, 0);

    if (gWideScreen) {
        C_MTXOrtho(proj, 0.0f, 480.0f, -107.0f, 747.0f, -1.0f, 1.0f);
    } else {
        C_MTXOrtho(proj, 0.0f, 480.0f, 0.0f, 640.0f, -1.0f, 1.0f);
    }
    GXSetProjection(proj, GX_ORTHOGRAPHIC);

    GXBegin(GX_QUADS, GX_VTXFMT2, 4);
    GXPosition3f32(x0, y0, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x0, y1, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(0.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x1, y1, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(1.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x1, y0, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(1.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
}

void ImageDrawLoading(ImageInfo* info, void* data, ImageInfo* iconInfo, void* iconData, int frame, int alpha) {
    GXTexObj obj;
    Mtx44 proj;
    GXTexFmt fmt = GX_TF_CMPR;
    int phase;
    u8 a = alpha;

    switch (info->type) {
    case 0:
        fmt = GX_TF_RGBA8;
        break;
    case 1:
        fmt = GX_TF_CMPR;
        break;
    case 2:
        fmt = GX_TF_IA8;
        break;
    case 3:
        fmt = GX_TF_I4;
        break;
    }

    GfxSetVtxDesc(2);
    GfxSetMaterial(0);
    GfxResetTev();
    GfxSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GfxSetCullMode(GX_CULL_NONE);

    GXInitTexObj(&obj, data, info->width, info->height, fmt, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GfxLoadTexObj(&obj, 0);

    if (gWideScreen) {
        C_MTXOrtho(proj, 0.0f, 480.0f, -107.0f, 747.0f, -1.0f, 1.0f);
    } else {
        C_MTXOrtho(proj, 0.0f, 448.0f, 0.0f, 608.0f, -100.0f, 100.0f);
    }
    GXSetProjection(proj, GX_ORTHOGRAPHIC);

    {
        f32 x0 = 0.0f;
        f32 y0 = 0.0f;
        f32 x1 = 608.0f;
        f32 y1 = 448.0f;
        ImageQuad(x0, y0, x1, y1, 255, 255, 255, a);
    }

    if (iconInfo != 0) {
        GXInitTexObj(&obj, iconData, iconInfo->width, iconInfo->height, fmt, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GfxLoadTexObj(&obj, 0);

        phase = frame % 60;
        if (phase < 30) {
            phase = (u8)(8.533334f * phase);
        } else {
            phase = (u8)(8.533333f * (60 - phase));
        }

        {
            f32 x0 = 0.0f;
            f32 y0 = 359.0f;
            f32 x1 = 608.0f;
            f32 y1 = 407.0f;
            ImageQuad(x0, y0, x1, y1, 255, 255, 255, phase);
        }
    }
}

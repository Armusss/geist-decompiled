// Cube.cpp: 0x80005800-0x8000DC24, the GameCube platform layer: controller input mapping and shadowing,
// OS/DVD/VI/GX initialization, file reads, per-frame GX state setup, frame begin/swap/end, fatal errors.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/pad.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <string.h>
#include <dolphin/vi.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

// Copy of a pad's state as the game sees it (0x10 bytes).
struct PadSnap {
    int connected;     // 0x00
    u16 buttons;       // 0x04
    s8 stickX;         // 0x06
    s8 stickY;         // 0x07
    s8 substickX;      // 0x08
    s8 substickY;      // 0x09
    u8 triggerRight;   // 0x0A
    u8 triggerLeft;    // 0x0B
    u8 analogA;        // 0x0C
    u8 analogB;        // 0x0D
    u8 pad0E[2];
};

extern PADStatus lbl_803CFEE0[4];   // latest controller poll
extern int lbl_80341280[][27];      // button map per control scheme
extern int lbl_804D3FE0;            // control scheme
extern int lbl_804D3FE4;            // pad shadow mode (0 off, 1 record, 2 replay)
extern PADStatus* lbl_804D3FE8;     // pad shadow buffer

extern "C" int fn_80005D64(int button);

// PadShadow: mode 0 off, 1 record into / 2 replay from the given buffer.
int PadShadow(int mode, PADStatus* buffer) {
    switch (mode) {
    case 0:
        lbl_804D3FE4 = 0;
        lbl_804D3FE8 = 0;
        return 1;
    case 1:
        if (buffer != 0) {
            lbl_804D3FE8 = buffer;
            lbl_804D3FE4 = 1;
            return 1;
        }
        break;
    case 2:
        if (buffer != 0) {
            lbl_804D3FE8 = buffer;
            lbl_804D3FE4 = 2;
            return 1;
        }
        break;
    }
    return 0;
}

// PadSnapshot: copies the four pads into game format.
extern "C" void fn_80005878(PadSnap* out) {
    PADStatus* p = lbl_803CFEE0;
    int i;

    for (i = 0; i < 4; i++) {
        if (p->err == 0) {
            out->connected = 1;
            out->buttons = p->button;
            out->stickX = p->stickX;
            out->stickY = p->stickY;
            out->substickX = p->substickX;
            out->substickY = p->substickY;
            out->triggerRight = p->triggerRight;
            out->triggerLeft = p->triggerLeft;
            out->analogA = p->analogA;
            out->analogB = p->analogB;
        } else {
            out->connected = 0;
        }
        p++;
        out++;
    }
}

// PadAnyButton: nonzero if any pad has a button held.
extern "C" int fn_80005974(void) {
    PADStatus* p = lbl_803CFEE0;

    if (p->button != 0) {
        return 1;
    }
    if ((++p)->button != 0) {
        return 1;
    }
    if ((++p)->button != 0) {
        return 1;
    }
    if ((++p)->button != 0) {
        return 1;
    }
    return 0;
}

// PadMapped: tests a mapped action for the player in bits 16-23 of the key.
extern "C" int fn_800059D0(u32 key) {
    int r;
    int player = (key >> 16) & 0xFF;
    u32 idx = key & 0xFF00FFFF;

    if (lbl_803CFEE0[player].err == 0) {
        r = fn_80005D64(lbl_80341280[lbl_804D3FE0][idx]);
    } else {
        r = 0;
    }
    return r;
}

extern "C" {
void fn_800E2368(void);
}
void SystemPumpBusy(void);
void ARAMLevelLoaded(void);

extern u32 lbl_804D2580;   // system flag bits
extern int lbl_804D4118;
extern float lbl_804D4094;
extern float lbl_804D2560;

// PadShadowOff: resets the pad hardware wrapper and turns shadowing off.
extern "C" void fn_80005A38(void) {
    fn_800E2368();
    lbl_804D3FE4 = 0;
    lbl_804D3FE8 = 0;
}

void LoadingTick(void) {}

// LoadingSetState
extern "C" void fn_80006D20(int v) { lbl_804D4118 = v; }

extern "C" void fn_80006D28(void) {}

// SystemFlagsTest
extern "C" u32 fn_80007060(u32 mask) { return lbl_804D2580 & mask; }

// SystemFlagsSet: sets or clears the bits of mask.
extern "C" void fn_8000706C(u32 mask, int set) {
    if (set != 0) {
        lbl_804D2580 |= mask;
    } else {
        lbl_804D2580 &= ~mask;
    }
}

// FileFlushBuffer: byte-swaps every 32-bit word of a buffer in place (little-endian data).
// Hand-written assembly in retail: loops with subfc. and lwbrx, params spilled by the frame prologue.
extern "C" asm void FileFlushBuffer(void* buf, int bytes) {
    fralloc
    cmplwi r3, 0
    beq done
    and. r4, r4, r4
    beq done
    li r5, 4
loop:
    subfc. r4, r5, r4
    lwbrx r0, r4, r3
    stwx r0, r4, r3
    bne+ loop
done:
    frfree
    blr
}

// ByteSwap32: byte-swaps one 32-bit word in place (hand-written assembly).
extern "C" asm void fn_800070C0(void* p) {
    fralloc
    cmplwi r3, 0
    beq done
    lwbrx r0, r0, r3
    stwx r0, r0, r3
done:
    frfree
    blr
}

// ByteSwap16Array: byte-swaps every 16-bit word of a buffer in place (hand-written assembly).
extern "C" asm void fn_800070E0(void* buf, int bytes) {
    fralloc
    cmplwi r3, 0
    beq done
    and. r4, r4, r4
    beq done
    li r5, 2
loop:
    subfc. r4, r5, r4
    lhbrx r0, r4, r3
    sthx r0, r4, r3
    bne+ loop
done:
    frfree
    blr
}

// ByteSwap16: byte-swaps one 16-bit word in place (hand-written assembly).
extern "C" asm void fn_80007118(void* p) {
    fralloc
    cmplwi r3, 0
    beq done
    lhbrx r0, r0, r3
    sthx r0, r0, r3
done:
    frfree
    blr
}

// SystemPump
extern "C" void fn_80007138(void) { SystemPumpBusy(); }

extern "C" void fn_8000719C(float v) { lbl_804D4094 = v; }

extern "C" float fn_800071A4(void) { return lbl_804D4094; }

extern "C" void fn_800071AC(void) { ARAMLevelLoaded(); }

// Swaps in a new value, returning the old one.
extern "C" float fn_80007244(float v) {
    float old = lbl_804D2560;

    lbl_804D2560 = v;
    return old;
}

extern "C" {
void fn_8002FB90(void);
void fn_80012F78(void);
void fn_8003CF98(void);
void fn_80093180(void);
void fn_8002FAE4(void);
void fn_80012ECC(void);
void fn_800930D0(void);
}
void TextureFlushReleases(void);
void ModelFlushRequests(void);
void TextureFlushRequests(void);

struct ScreenRect {
    short x0, x1, y0, y1;
};
extern ScreenRect lbl_804D400C;   // screen rectangle
extern int lbl_804D4084;
extern int lbl_804D2548;
extern float lbl_804D2540;
extern int lbl_804D405C;
extern int lbl_804D4060;
extern int lbl_804D4048;
extern u8 lbl_804D403C, lbl_804D403D, lbl_804D403E;
extern u8 lbl_804D4040[4];   // clear color
extern int lbl_804D4024;
extern char lbl_8034EE8C[];
extern char lbl_803CFFE4[];

// ResourceFlush: lets every cache release and request what it queued.
extern "C" void fn_80007158(void) {
    fn_8002FB90();
    fn_80012F78();
    fn_8003CF98();
    TextureFlushReleases();
    fn_80093180();
    fn_8002FAE4();
    fn_80012ECC();
    ModelFlushRequests();
    TextureFlushRequests();
    fn_800930D0();
}

// SetScissorOffset: converts a position in 640x448 screen units to the current screen rectangle.
extern "C" void fn_800071CC(int x, int y) {
    ScreenRect* r = &lbl_804D400C;
    x = x * (r->x1 - r->x0);
    y = y * (r->y1 - r->y0);
    GXSetScissorBoxOffset(x / 640, y / 448);
}

extern "C" void fn_80008790(int a, int b) {
    lbl_804D405C = a;
    lbl_804D4060 = b;
}

extern "C" void fn_8000879C(const char* s) { strcpy(lbl_8034EE8C, s); }

// Swaps in a new value, returning the old one.
extern "C" int fn_800087C8(int v) {
    int old = lbl_804D4048;
    lbl_804D4048 = v;
    return old;
}

extern "C" void fn_800087D8(u8 r, u8 g, u8 b) {
    lbl_804D403C = r;
    lbl_804D403D = g;
    lbl_804D403E = b;
}

extern "C" void fn_80008940(u8 r, u8 g, u8 b) {
    lbl_804D4040[0] = r;
    lbl_804D4040[1] = g;
    lbl_804D4040[2] = b;
    lbl_804D4040[3] = 0xFF;
}

int SysHeapFreeBytes(void) { return OSCheckHeap(lbl_804D4024); }

// GfxSetCullMode: swaps front/back culling when mirrored.
void GfxSetCullMode(int mode) {
    if (lbl_804D4084 == 0) {
        GXSetCullMode((GXCullMode)mode);
    } else {
        if (mode == 1) {
            mode = 2;
        } else if (mode == 2) {
            mode = 1;
        }
        GXSetCullMode((GXCullMode)mode);
    }
}

extern "C" void fn_80008B94(int mode) {
    if (lbl_804D2548 != mode) {
        lbl_804D2548 = mode;
        GXSetClipMode(GX_CLIP_ENABLE);
    }
}

extern "C" void fn_80008BC8(void) { OSPanic("Cube.cpp", 0xCA8, ""); }
extern "C" void fn_80008C00(void) { OSPanic("Cube.cpp", 0xC7E, ""); }
void FatalError(const char* msg) { OSPanic("Cube.cpp", 0xC5F, ""); }

extern "C" float fn_80008C70(void) { return lbl_804D2540; }

extern "C" void fn_80008C78(void* m) { PSMTXCopy((float(*)[4])lbl_803CFFE4, (float(*)[4])m); }


extern "C" {
void fn_8000FF7C(void);
void fn_8010F7EC(void);
void fn_801D8FC8(void);
void fn_80013F50(void);
void fn_800F0FAC(void);
void fn_80040600(void);
double tan(double x);
}
void DeferFlush0to2(void);
void MeshNop0(void);

struct VtxDescSet {
    GXVtxDescList list[6];
};

struct DebugLine {
    int from[3];     // 0x00
    int to[3];     // 0x0C
    u8 r, g, b;      // 0x18
    u8 pad1b;
};

extern int lbl_804D4038;
extern int lbl_804D2530;
extern VtxDescSet lbl_8034F018[];
extern int lbl_803D712C[];
extern int lbl_803D7328[];
extern int lbl_804D4074;
extern int lbl_804D407C;
extern int lbl_804D4034;
extern DebugLine lbl_803D0114[0x400];
extern float lbl_804D2544;
extern int lbl_804D4050;
extern int lbl_804D4054;
extern int lbl_804D4058;
extern int lbl_804D2550;

static inline void Ortho2D(float t, float b, float l, float r) {
    Mtx44 id;
    Mtx44 m;

    C_MTXOrtho(m, t, b, l, r, 0.0f, -1.0f);
    GXSetProjection(m, GX_ORTHOGRAPHIC);
    PSMTX44Identity(id);
    GXLoadPosMtxImm((float(*)[4])id, 0);
    GXSetCurrentMtx(0);
    lbl_804D4038 = 0;
}

// Draw2DBegin640x480
extern "C" void fn_80008CA4(void) { Ortho2D(480.0f, 0.0f, 0.0f, 640.0f); }

// Draw2DBeginUnit
extern "C" void fn_80008D10(void) { Ortho2D(0.0f, 1.0f, 0.0f, 1.0f); }

extern "C" void fn_80008D7C(void) {
    lbl_804D2530 = -1;
    fn_8000FF7C();
    fn_8010F7EC();
    fn_801D8FC8();
}

void GfxSetVtxDesc(int set) {
    if (set != lbl_804D2530) {
        GXClearVtxDesc();
        GXSetVtxDescv(lbl_8034F018[set].list);
        lbl_804D2530 = set;
    }
}

extern "C" void fn_80008DFC(int i) {
    lbl_803D712C[i] = lbl_804D4074 - lbl_803D712C[i];
    lbl_803D7328[i] = lbl_804D407C - lbl_803D7328[i];
}

extern "C" void fn_80008E34(int i) {
    lbl_803D712C[i] = lbl_804D4074;
    lbl_803D7328[i] = lbl_804D407C;
}

extern "C" void fn_80008EF0(Vec* in, Vec* out) { PSMTX44MultVecSR((float(*)[4])lbl_803CFFE4, in, out); }

extern "C" void fn_80008F24(Vec* in, Vec* out) { PSMTXMultVec((float(*)[4])lbl_803CFFE4, in, out); }

extern "C" void fn_80008F58(void) { lbl_804D4034 = 0; }

// DebugLineAdd: queues a colored debug line (up to 1024 per frame).
extern "C" void fn_80009170(int* from, int* to, u8 r, u8 g, u8 b) {
    int n = lbl_804D4034;
    DebugLine* l;

    if (n >= 0x400) {
        return;
    }
    l = &lbl_803D0114[n];
    l->from[0] = from[0];
    l->from[1] = from[1];
    l->from[2] = from[2];
    l->to[0] = to[0];
    l->to[1] = to[1];
    l->to[2] = to[2];
    l->r = r;
    l->g = g;
    l->b = b;
    lbl_804D4034 = n + 1;
}

// Focal length in pixels for a field of view.
extern "C" float fn_800097B4(void) { return 240.0f * (float)tan(1.5707964f - lbl_804D2544 * 0.5f); }

extern "C" void fn_80009E10(void) {
    lbl_804D2548 = -2;
    DeferFlush0to2();
    fn_80013F50();
    fn_80040600();
    MeshNop0();
}

extern "C" int fn_8000A9E0(void) {
    if (lbl_804D4050 != 0 && lbl_804D4054 != 0) {
        return 1;
    }
    return 0;
}

extern "C" void fn_8000AA08(void) { lbl_804D4058 = 0; }

extern "C" void fn_8000AA14(void) { lbl_804D4058 = 1; }

extern "C" void fn_8000ADD0(int v) { lbl_804D2550 = v; }

extern "C" void fn_8000ADD8(void) {}

extern "C" {
void fn_80013560(void);
void fn_8004065C(void);
void fn_80013710(void);
void fn_8010F0AC(void);
void fn_801D8D30(void);
void fn_80234DAC(void);
void fn_800F724C(void);
void fn_8010EFB0(void);
void fn_801D8D3C(void);
int fn_80286EAC(void);
void fn_800E23CC(void);
void fn_8000BC5C(int arg);
void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);
}
void MeshNop1(void);
void TextureReleaseAll(void);
void GfxLightsClearStats(void);
void ModelTableInit(void);
void TextureSystemInit(void);
void MeshEnable(void);
void DeferNop(void);
void DeferShutdown(void);
void MemShutdown(void);
void ARQReset(void);
extern float gAspectScale;
extern float lbl_804D2540;
extern int lbl_804D404C;
extern OSThread* lbl_804D40E8;
extern int lbl_804D40FC;
extern int lbl_804D40F0;
extern int lbl_804D40EC;
extern int lbl_804D4110;
extern int lbl_804D4070;
extern int lbl_804D4078;

struct CameraState {
    u8 pad0[0x64];
    Mtx pos;      // 0x64
    Mtx nrm;      // 0xA4 (stride 0x40)
    u8 pad_[0xE4 - 0xA4 - sizeof(Mtx)];
    Mtx44 proj;   // 0xE4
};
extern CameraState lbl_803CFF80;

// LoadCameraMatrices: loads the current camera's projection and position/normal matrices into GX.
extern "C" void fn_800092CC(void) {
    CameraState* c = &lbl_803CFF80;

    GXSetProjection(c->proj, GX_PERSPECTIVE);
    GXLoadPosMtxImm(c->pos, 0);
    GXLoadNrmMtxImm(c->nrm, 0);
    GXSetCurrentMtx(0);
}

extern "C" void fn_800097F0(void) {
    CameraState* c = &lbl_803CFF80;

    GXSetProjection(c->proj, GX_PERSPECTIVE);
    GXLoadPosMtxImm(c->pos, 0);
    GXLoadNrmMtxImm(c->nrm, 0);
    GXSetCurrentMtx(0);
}

// LoadModelMatrix: concatenates a matrix with the view matrix and loads it into GX slot `slot`.
extern "C" void fn_80009324(Mtx m, int slot) {
    Mtx n;
    Mtx c;

    PSMTXConcat((float(*)[4])lbl_803CFFE4, m, c);
    GXLoadPosMtxImm(c, slot);
    PSMTXInvXpose(c, n);
    GXLoadNrmMtxImm(n, slot);
    GXSetCurrentMtx(slot);
    lbl_804D4038 = 1;
}

// TextureProjectionMatrix: out = lightPerspective * in.
extern "C" void fn_80009848(Mtx out, Mtx in, float fov) {
    Mtx m;

    C_MTXLightPerspective(m, lbl_804D2540 + fov, gAspectScale, 0.5f, -0.5f, 0.5f, 0.5f);
    PSMTXConcat(m, in, out);
}

extern "C" void fn_8000AE40(void) {
    fn_80013560();
    fn_8004065C();
    MeshNop1();
    TextureReleaseAll();
}

extern "C" void fn_8000AFB0(void) {
    GfxLightsClearStats();
    fn_80013710();
    ModelTableInit();
    TextureSystemInit();
    MeshEnable();
    DeferNop();
    lbl_804D4034 = 0;
    fn_8010F0AC();
    fn_801D8D30();
    fn_80234DAC();
    lbl_804D404C = VIGetRetraceCount();
}

extern "C" void fn_8000B000(void) {
    fn_800F724C();
    ARQReset();
    DeferShutdown();
    fn_8010EFB0();
    fn_801D8D3C();
    MemShutdown();
}

extern "C" void fn_8000B5CC(void) {
    if (lbl_804D40FC != 0 && lbl_804D40E8 != 0) {
        OSResumeThread(lbl_804D40E8);
    }
}

extern "C" void fn_8000C328(void) { lbl_804D40F0 = 1; }

extern "C" void fn_8000C334(void) { lbl_804D40EC += fn_80286EAC(); }

// FrameCountersAdd
extern "C" void fn_8000C354(int a, int b, int c, int d, int doExtra) {
    lbl_804D4074 = lbl_804D4074 + 1;
    lbl_804D4078 = lbl_804D4078 + d;
    if (doExtra != 0) {
        lbl_804D407C = lbl_804D407C + 1;
    }
}

extern "C" void* fn_8000D024(int size) { return MemAlloc(size, "Cube.cpp", 0x24A); }
extern "C" void fn_8000D054(void* p) { MemFree(p, "Cube.cpp", 0x243); }

extern "C" void fn_80006558(int on) {
    if (on != 0) {
        lbl_804D40FC = 1;
    } else {
        lbl_804D40FC = 0;
        lbl_804D2580 |= 1;
        lbl_804D4110 = 0;
        GXSetCurrentGXThread();
    }
}

extern "C" void fn_800065A4(void) {
    fn_8000BC5C(0);
    fn_8000BC5C(-1);
    lbl_804D4070 = 0;
}

extern "C" void fn_80005CEC(void) {
    PADStatus* p = lbl_803CFEE0;

    memset((char*)p + 0x30, 0, 0x30);
    memset((char*)p, 0, 0x30);
    memset((char*)p + 0x60, 0, 0x40);
    PADInit();
    fn_800E23CC();
    lbl_804D3FE0 = 0;
    lbl_804D3FE4 = 0;
    lbl_804D3FE8 = 0;
}

// Empty in retail (variadic stubs): the game's own OSReport/OSPanic do nothing in this build.
extern "C" void OSReport(const char* msg, ...) {}
extern "C" void OSPanic(const char* file, int line, const char* msg, ...) {}

extern int lbl_804D252C;
extern float lbl_804D2538;
extern float lbl_804D253C;
extern float lbl_804D254C;
extern int lbl_804D4064;

struct FogParams {
    float a;      // 0x00
    int b;        // 0x04
    u8 color[4];  // 0x08
    float c;      // 0x0C
    float d;      // 0x10
};

// FogGetParams
extern "C" void fn_80008AD8(FogParams* out) {
    out->b = lbl_804D252C;
    out->color[0] = lbl_804D4040[0];
    out->color[1] = lbl_804D4040[1];
    out->color[2] = lbl_804D4040[2];
    out->color[3] = lbl_804D4040[3];
    out->c = lbl_804D2538;
    out->d = lbl_804D253C;
    out->a = lbl_804D254C;
}

void GfxLightsBeginFrame(void);

// FrameSetup: resets render state at the start of a frame unless `skip` is set.
extern "C" void fn_80009E44(int arg, int skip) {
    if (skip != 0) {
        lbl_804D4064 = arg;
        fn_8000BC5C(-1);
    } else {
        lbl_804D2548 = -2;
        fn_800F0FAC();
        fn_80013F50();
        fn_80040600();
        MeshNop0();
        GfxSetCullMode(1);
        lbl_804D4064 = arg;
        fn_8000BC5C(-1);
        GfxLightsBeginFrame();
    }
}

// Functions not drafted yet (symbol names are still the placeholders):
// TODO (not drafted yet, 0x288 bytes at 0x80005A64): fn_80005A64
// TODO (not drafted yet, 0x588 bytes at 0x80005D64): fn_80005D64
// TODO (not drafted yet, 0x26C bytes at 0x800062EC): fn_800062EC
// TODO (not drafted yet, 0x180 bytes at 0x800065D8): fn_800065D8
// TODO (not drafted yet, 0x524 bytes at 0x800067F8): fn_800067F8
// TODO (not drafted yet, 0x334 bytes at 0x80006D2C): fn_80006D2C
// TODO (not drafted yet, 0x1AC bytes at 0x80007254): fn_80007254
// TODO (not drafted yet, 0x21C bytes at 0x80007400): fn_80007400
// TODO (not drafted yet, 0x200 bytes at 0x8000761C): fn_8000761C
// TODO (not drafted yet, 0x50 bytes at 0x8000781C): DebugPrintf__FPCce
// TODO (not drafted yet, 0x354 bytes at 0x8000786C): fn_8000786C
// TODO (not drafted yet, 0x300 bytes at 0x80007BC0): FileReadSync__FPvPvii
// TODO (not drafted yet, 0x54 bytes at 0x80007EC0): SystemIdle__Fv
// TODO (not drafted yet, 0x3F0 bytes at 0x80007F14): fn_80007F14
// TODO (not drafted yet, 0x48C bytes at 0x80008304): fn_80008304
// TODO (not drafted yet, 0x158 bytes at 0x800087E8): fn_800087E8
// TODO (not drafted yet, 0x17C bytes at 0x8000895C): fn_8000895C
// TODO (not drafted yet, 0x94 bytes at 0x80008E5C): fn_80008E5C
// TODO (not drafted yet, 0x158 bytes at 0x80008F64): fn_80008F64
// TODO (not drafted yet, 0xB4 bytes at 0x800090BC): fn_800090BC
// TODO (not drafted yet, 0xF8 bytes at 0x800091D4): fn_800091D4
// TODO (not drafted yet, 0x140 bytes at 0x80009398): fn_80009398
// TODO (not drafted yet, 0x2DC bytes at 0x800094D8): fn_800094D8
// TODO (not drafted yet, 0x558 bytes at 0x800098B8): fn_800098B8
// TODO (not drafted yet, 0x148 bytes at 0x80009EE0): GfxFrameBegin__Fv
// TODO (not drafted yet, 0x9B8 bytes at 0x8000A028): GfxFrameSwap__Fv
// TODO (not drafted yet, 0x240 bytes at 0x8000AA20): SystemPoll__Fv
// TODO (not drafted yet, 0x170 bytes at 0x8000AC60): fn_8000AC60
// TODO (not drafted yet, 0x64 bytes at 0x8000ADDC): GfxFrameEnd__Fv
// TODO (not drafted yet, 0x88 bytes at 0x8000AE6C): fn_8000AE6C
// TODO (not drafted yet, 0xBC bytes at 0x8000AEF4): fn_8000AEF4
// TODO (not drafted yet, 0x598 bytes at 0x8000B034): fn_8000B034
// TODO (not drafted yet, 0x36C bytes at 0x8000B604): fn_8000B604
// TODO (not drafted yet, 0x11C bytes at 0x8000B970): fn_8000B970
// TODO (not drafted yet, 0x1D0 bytes at 0x8000BA8C): fn_8000BA8C
// TODO (not drafted yet, 0x3D4 bytes at 0x8000BC5C): fn_8000BC5C
// TODO (not drafted yet, 0x2F8 bytes at 0x8000C030): fn_8000C030
// TODO (not drafted yet, 0x5B8 bytes at 0x8000C384): fn_8000C384
// TODO (not drafted yet, 0x4EC bytes at 0x8000C93C): fn_8000C93C
// TODO (not drafted yet, 0x1FC bytes at 0x8000CE28): fn_8000CE28
// TODO (not drafted yet, 0xBA0 bytes at 0x8000D084): fn_8000D084

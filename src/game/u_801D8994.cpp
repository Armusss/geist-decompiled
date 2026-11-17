// u_801D8994.cpp: 0x801D8994-0x801D8FD4, 9 functions. Graphics state helpers (GfxSetBlendMode).
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   .sbss 0x804D51F0/0x804D51F4 (used from fn_801D8994 to fn_801D8FC8), .sdata 0x804D3238
//   the next unit (u_801D9A70) starts its .sbss at 0x804D51F8
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

#include <dolphin/gx.h>

extern "C" void fn_800087E8(int on, int r, int g, int b);  // 0x800087E8 (Cube)

// Mode bits: 0x02/0x04/0x08/0x10/0x20 pick a blend preset, 0x100 turns the depth test off, 0x206 the depth write.
static int sMode;               // 0x804D51F0: the current mode word, 0 = unknown
static int sDestAlpha;          // 0x804D51F4: blend with the destination alpha instead of the source alpha
static u8 sZWrite = 1;          // 0x804D3238

extern "C" void fn_801D8994(int destAlpha) {  // GfxInit
    sDestAlpha = destAlpha;
    sMode = 0;
}

static inline void GfxApplyZMode(void) {
    GXSetZMode(!(sMode & 0x100), GX_LEQUAL, sZWrite != 0 && (sMode & 0x206) == 0);
}

extern "C" u8 fn_801D89A4(int enable) {  // GfxSetZWrite: -2 only reads
    u8 old = sZWrite;
    if (enable != -2) {
        sZWrite = enable;
        GfxApplyZMode();
    }
    return old;
}

void GfxSetBlendMode(int type, int src, int dst, int op) {
    sMode = 0;
    if (sDestAlpha == 0) {
        GXSetBlendMode((GXBlendMode)type, (GXBlendFactor)src, (GXBlendFactor)dst, (GXLogicOp)op);
    } else if (dst == GX_BL_INVSRCALPHA) {
        GXSetBlendMode((GXBlendMode)type, GX_BL_DSTALPHA, GX_BL_INVDSTALPHA, (GXLogicOp)op);
    } else {
        GXSetBlendMode((GXBlendMode)type, GX_BL_DSTALPHA, (GXBlendFactor)dst, (GXLogicOp)op);
    }
}

static inline int GfxModeIs(int mode) {
    if (sMode == mode) {
        return 1;
    }
    sMode = mode;
    return 0;
}

extern "C" void fn_801D8A68(int mode) {  // GfxSetMode
    if (GfxModeIs(mode) == 0) {
        GfxApplyZMode();
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        if (sMode & 2) {
            GfxSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_COPY);
            fn_800087E8(1, 0, 0, 0);
        } else if (sMode & 4) {
            GfxSetBlendMode(GX_BM_SUBTRACT, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_COPY);
            fn_800087E8(1, 0, 0, 0);
        } else if (sMode & 0x10) {
            GfxSetBlendMode(GX_BM_BLEND, GX_BL_ZERO, GX_BL_SRCCLR, GX_LO_COPY);
            fn_800087E8(1, 0xFF, 0xFF, 0xFF);
        } else if (sMode & 0x20) {
            GfxSetBlendMode(GX_BM_BLEND, GX_BL_INVDSTCLR, GX_BL_ZERO, GX_LO_COPY);
            fn_800087E8(1, 0, 0, 0);
        } else if (sMode & 8) {
            GfxSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
            fn_800087E8(0, 0, 0, 0);
        } else {
            GfxSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
            fn_800087E8(0, 0, 0, 0);
        }
    }
}

extern "C" void fn_801D8D30(void) {  // GfxShutdown
    sDestAlpha = 0;
}

extern "C" void fn_801D8D3C(void) {
}

// Retail repeats GfxSetMode's apply code here instead of calling it (a static inline helper for it is not
// inlined by CW at this size, so the code was written out twice).
extern "C" void fn_801D8D40(void) {  // GfxResetMode
    sZWrite = 1;
    sMode = 1;
    GfxApplyZMode();
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    if (sMode & 2) {
        GfxSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_COPY);
        fn_800087E8(1, 0, 0, 0);
    } else if (sMode & 4) {
        GfxSetBlendMode(GX_BM_SUBTRACT, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_COPY);
        fn_800087E8(1, 0, 0, 0);
    } else if (sMode & 0x10) {
        GfxSetBlendMode(GX_BM_BLEND, GX_BL_ZERO, GX_BL_SRCCLR, GX_LO_COPY);
        fn_800087E8(1, 0xFF, 0xFF, 0xFF);
    } else if (sMode & 0x20) {
        GfxSetBlendMode(GX_BM_BLEND, GX_BL_INVDSTCLR, GX_BL_ZERO, GX_LO_COPY);
        fn_800087E8(1, 0, 0, 0);
    } else if (sMode & 8) {
        GfxSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
        fn_800087E8(0, 0, 0, 0);
    } else {
        GfxSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
        fn_800087E8(0, 0, 0, 0);
    }
}

extern "C" void fn_801D8FC8(void) {  // GfxInvalidateMode
    sMode = 0;
}

// u_80093BF0.cpp (0x80093BF0-0x8009C74C), split from the old u_8008DCDC range: 2D/screen-space drawing
// through GX (immediate quads at 0xCC008000, colours, the screen rectangle set by AnimSetRect) and its sinit.
// Boundary evidence (the cut at 0x80093BF0):
// - .sdata2: this file's pool starts at 0x804D6960 and has its own copies of constants the previous file
//   (u_8008DCDC.cpp, pool 0x804D6938-0x804D6960) also has: 0.0f 0x804D69BC vs 0x804D6938, 1.0f 0x804D69C0 vs
//   0x804D6948, the int-to-float doubles 0x804D69D0/0x804D6A10 vs 0x804D6958/0x804D6940. Last reader of the old
//   pool is fn_80093B3C, first reader of the new one is fn_80093BF0.
// - The 4-byte colour constants at 0x804D6960-0x804D69B8 (local GXColor initialisers, never merged) are laid out
//   in exact reverse order of their using functions from fn_80093BF0 (0x804D69B8) down to fn_8009851C
//   (0x804D6960): one contiguous run, consistent with a single file; no float constant is duplicated after it.
// - .sbss: this file touches 0x804D4728-0x804D4763 (screen rect, draw state), the previous file only
//   0x804D4718-0x804D4727; .sdata 0x804D2A50-0x804D2B74 vs the previous file's 0x804D2A48.
// - .ctors 0x80341208 holds fn_8009C6CC, the last function here (this file's static initialiser).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/gx.h>

typedef unsigned char u8;
typedef unsigned short u16;

// Engine graphics wrappers (Cube.cpp / CubeTev.cpp).
void GfxSetBlendMode(int type, int src, int dst, int op);
void GfxSetVtxDesc(int desc);
void GfxSetMaterial(void* material);
void GfxSetCullMode(int mode);
extern "C" void fn_80008D10(void);
extern "C" void fn_8000FF7C(void);
extern "C" void fn_800097F0(void);
extern "C" void fn_8010CF00(void);
extern "C" void fn_8010D024(void);
void GfxLoadTexObj(GXTexObj* obj, int map);   // 0x8000F944
void GfxResetTev(void);
void GfxFbCommit();
void GfxSetCopyFilter(int mode);
void CubeTevSetupChannelSwap();
void CubeTevOneStageTextured();
extern "C" void fn_80009E10();
extern "C" GXTexObj* fn_80115C18(void* copy);
extern "C" void* fn_80115260(int a, int b, int c);
extern "C" void* fn_80115374(int a, int b, int c);
extern "C" void* fn_80115850(int a, int b, int c);
extern "C" void fn_80106AE0(float sx, float sy);
extern "C" void fn_80107BD8(float sx, float sy);
extern "C" void fn_80107514(float sx, float sy);
extern "C" void* fn_80115488(int a, int b, int c);
extern "C" void fn_8010829C();
extern "C" void fn_80108528();
extern "C" void fn_8010883C();
void CubeTevSetupIndirectTwo();
void CubeTevOneStageSwapC();
void CubeTevSetupRingAndBlend(float sx, float sy);
void CubeTevSetupRingIndirect(float sx, float sy);
void CubeTevSetupRingBlend15(float sx, float sy);
void CubeTevSetupRingSix(float sx, float sy);
void CubeTevSetupRingPairs(float sx, float sy);
extern "C" void fn_8010F7EC();
extern "C" void fn_8010CDD8();
extern float lbl_804D4738;
extern "C" void* fn_80115A34(int a, int b, short* rect);
extern "C" int fn_80010448(int id, int alpha);
extern "C" int fn_80010330(int id, int alpha);

extern int lbl_804D4154;
extern int lbl_804D2568;    // screen width
extern int lbl_804D256C;    // screen height

extern void* lbl_804D475C;
extern int lbl_804D4760;

extern "C" void fn_80098244(int r, int g, int b);
extern "C" void fn_80099D74(int a, int b, int c, int alpha);
extern "C" void fn_8009C4A4(int mode, int a, int b, int c, int d);
extern "C" void fn_8009B940(float t);
extern "C" void fn_80099F18(int mode, float t);
extern "C" void fn_8009925C(int alpha, float a, float b);
extern "C" void fn_8009ADC8(int mode, float a, float b);
extern float lbl_804D4728;
extern float lbl_804D472C;
extern float lbl_804D2A50;
extern float lbl_804D2A54;
extern float lbl_804D2A58;
extern float lbl_804D2A5C;

// TODO (not drafted yet, 0x128 bytes at 0x80093BF0)
extern "C" void fn_80093BF0();
//@@80093D18
// Sets up the line-drawing state and draws with full alpha.
extern "C" void fn_80093D18(int a, int b, int c);
void fn_80093D18(int a, int b, int c) {
    GfxSetVtxDesc(2);
    fn_80008D10();
    GfxSetMaterial(0);
    GXSetLineWidth(0x18, GX_TO_ONE);
    fn_8010D024();
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetAlphaUpdate(GX_TRUE);
    GXSetColorUpdate(GX_TRUE);
    GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
    GfxSetCullMode(0);
    GfxSetBlendMode(1, 4, 5, 3);
    fn_80099D74(a, b, c, 0xFF);
}
//@@end
//@@80093DE4
// Draws texture tex (or, without one, a copy of the cleared frame buffer) with effect mode at time t.
extern "C" void fn_80093DE4(int mode, int texArg, float t);
void fn_80093DE4(int mode, int texArg, float t) {
    GXTexObj* tex;
    if (lbl_804D4154 == 0) {
        if (texArg != 0) {
            tex = (GXTexObj*)texArg;
        } else {
            fn_80009E10();
            fn_80093D18(0, 0, 0);
            GfxSetCopyFilter(0);
            GfxFbCommit();
            tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        }
        if (tex != 0) {
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            CubeTevOneStageTextured();
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(0);
            GfxSetBlendMode(1, 4, 5, 3);
            GfxLoadTexObj(tex, 0);
            fn_80099F18(mode, t);
            fn_8000FF7C();
            fn_800097F0();
            GfxSetCullMode(1);
        }
    }
}
//@@end
//@@80093F88
// Releases image id (when in state 2), sets the flat 2D state and draws with the given colour.
extern "C" void fn_80093F88(int id, int a, int b, int c, int d);
void fn_80093F88(int id, int a, int b, int c, int d) {
    if (fn_80010448(id, 0) == 2) {
        fn_80010330(id, 0);
    }
    GfxSetVtxDesc(2);
    fn_80008D10();
    GfxSetMaterial(0);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);
    GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
    GfxSetCullMode(0);
    fn_8009C4A4(0, a, b, c, d);
}
//@@end
//@@80094030
// Draws the previous frame-buffer copy (if any), then takes a new copy.
extern "C" void fn_80094030();
void fn_80094030() {
    if (lbl_804D4154 == 0) {
        GfxFbCommit();
        if (lbl_804D475C != 0) {
            GXTexObj* tex = fn_80115C18(lbl_804D475C);
            GfxSetCopyFilter(0);
            lbl_804D475C = fn_80115260(0, 1, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            GfxSetBlendMode(3, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            GfxLoadTexObj(tex, 0);
            CubeTevOneStageTextured();
            fn_8009C4A4(0, 0xFF, 0xFF, 0xFF, 0xFF);
        } else {
            GfxSetCopyFilter(0);
            lbl_804D475C = fn_80115260(0, 1, 0);
        }
        fn_8000FF7C();
        fn_800097F0();
    }
}
//@@end
//@@80094150
// Draws the frame-buffer copy with the given alpha, then takes a new copy.
extern "C" void fn_80094150(int alpha);
void fn_80094150(int alpha) {
    if (lbl_804D4154 == 0) {
        if (lbl_804D475C != 0) {
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            GfxSetBlendMode(1, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetDstAlpha(GX_TRUE, 0xFF);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            GfxLoadTexObj((GXTexObj*)lbl_804D475C, 0);
            CubeTevOneStageTextured();
            fn_8009C4A4(0, 0xFF, 0xFF, 0xFF, alpha);
            GXSetDstAlpha(GX_FALSE, 0);
        }
        GfxSetCopyFilter(0);
        lbl_804D475C = fn_80115374(0, 0, 0);
        fn_8000FF7C();
        fn_800097F0();
    }
}
//@@end

// Clears the two cached words.
extern "C" void fn_8009425C();
void fn_8009425C() {
    lbl_804D475C = 0;
    lbl_804D4760 = 0;
}

// TODO (not drafted yet, 0x170 bytes at 0x8009426C)
extern "C" void fn_8009426C();
//@@800943DC
// Copies the frame buffer and draws it back through fn_80106AE0 (cell size 160.0f).
extern "C" void fn_800943DC();
void fn_800943DC() {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        GfxSetCopyFilter(2);
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            fn_80106AE0(lbl_804D2A50 / (160.0f * lbl_804D4738), lbl_804D2A54 / 160.0f);
            GfxSetBlendMode(1, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, 0xFF, 0xFF, 0xFF, 0xFF);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@800944D8
// Copies the frame buffer and draws it back through fn_80107BD8 (cell size 120.0f).
extern "C" void fn_800944D8();
void fn_800944D8() {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        GfxSetCopyFilter(2);
        tex = (GXTexObj*)fn_80115850(0, 1, 0);
        {
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            fn_80107BD8(lbl_804D2A50 / (120.0f * lbl_804D4738), lbl_804D2A54 / 120.0f);
            GfxSetBlendMode(3, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, 0xFF, 0xFF, 0xFF, 0xFF);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@800945CC
// Copies the frame buffer and draws it back through fn_80107BD8 (cell size 120.0f).
extern "C" void fn_800945CC();
void fn_800945CC() {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        GfxSetCopyFilter(2);
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            fn_80107BD8(lbl_804D2A50 / (120.0f * lbl_804D4738), lbl_804D2A54 / 120.0f);
            GfxSetBlendMode(0, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, 0xFF, 0xFF, 0xFF, 0xFF);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@800946C8
// Copies the frame buffer and draws it back through fn_80107514 (cell size 120.0f).
extern "C" void fn_800946C8();
void fn_800946C8() {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        GfxSetCopyFilter(2);
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            fn_80107514(lbl_804D2A50 / (120.0f * lbl_804D4738), lbl_804D2A54 / 120.0f);
            GfxSetBlendMode(1, 4, 1, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, 0xFF, 0xFF, 0xFF, 0x7F);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
// TODO (not drafted yet, 0x2F8 bytes at 0x800947C4)
extern "C" void fn_800947C4();
//@@80094ABC
// Blends the current frame-buffer copy with a second copy (dark).
extern "C" void fn_80094ABC();
void fn_80094ABC() {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GXTexObj* tex2;
        GfxFbCommit();
        GfxSetCopyFilter(2);
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            GfxSetCopyFilter(0);
            tex2 = (GXTexObj*)fn_80115488(0, 1, 0);
            GfxLoadTexObj(tex, 0);
            GXLoadTexObj(tex2, GX_TEXMAP1);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            fn_8010829C();
            GfxSetBlendMode(1, 4, 2, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, 0x20, 0x20, 0x20, 0xFF);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@80094BD4
// Blends the current frame-buffer copy with a second copy (full).
extern "C" void fn_80094BD4();
void fn_80094BD4() {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GXTexObj* tex2;
        GfxFbCommit();
        GfxSetCopyFilter(2);
        tex = (GXTexObj*)fn_80115850(0, 1, 0);
        {
            GfxSetCopyFilter(0);
            tex2 = (GXTexObj*)fn_80115488(0, 1, 0);
            GfxLoadTexObj(tex, 0);
            GXLoadTexObj(tex2, GX_TEXMAP1);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            fn_8010829C();
            GfxSetBlendMode(1, 4, 1, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, 0xFF, 0xFF, 0xFF, 0xFF);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@80094CEC
// Combines two frame-buffer copies and tints them (r, g, b).
extern "C" void fn_80094CEC(int r, int g, int b);
void fn_80094CEC(int r, int g, int b) {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GXTexObj* tex2;
        GfxFbCommit();
        GfxSetCopyFilter(2);
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            tex2 = (GXTexObj*)fn_80115850(0, 1, 0);
            GfxLoadTexObj(tex, 0);
            GXLoadTexObj(tex2, GX_TEXMAP1);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            fn_80108528();
            GfxSetBlendMode(0, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, r, g, b, 0xFF);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@80094E20
// Draws a bilinear-filtered frame-buffer copy through CubeTevSetupRingSix (scale sx, sy), faded to black by alpha.
extern "C" void fn_80094E20(float sx, float sy, float alpha);
void fn_80094E20(float sx, float sy, float alpha) {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        GfxSetCopyFilter(0);
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            GXInitTexObjFilter(tex, GX_LINEAR, GX_LINEAR);
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            CubeTevSetupRingSix(sx * (lbl_804D2A50 / lbl_804D4738), lbl_804D2A54 * sy);
            GfxSetBlendMode(1, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, 0, 0, 0, (u8)(255.0f * alpha));
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@80094F80
// Draws a frame-buffer copy through CubeTevSetupRingPairs (grey TEV colour) with colour (r, g, b, a).
extern "C" void fn_80094F80(int r, int g, int b, int a, float scale);
void fn_80094F80(int r, int g, int b, int a, float scale) {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            static const GXColor grey = {200, 200, 200, 255};
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            GXSetTevColor(GX_TEVREG0, grey);
            CubeTevSetupRingPairs(scale * (lbl_804D2A50 / lbl_804D4738), lbl_804D2A54 * scale);
            GfxSetBlendMode(1, 4, 1, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, r, g, b, a);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@800950C8
// Draws a frame-buffer copy through CubeTevSetupRingPairs (grey TEV colour) with colour (r, g, b, a).
extern "C" void fn_800950C8(int r, int g, int b, int a, float scale);
void fn_800950C8(int r, int g, int b, int a, float scale) {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        GfxSetCopyFilter(0);
        tex = (GXTexObj*)fn_80115850(0, 1, 0);
        {
            static const GXColor grey = {200, 200, 200, 255};
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            GXSetTevColor(GX_TEVREG0, grey);
            CubeTevSetupRingPairs(scale * (lbl_804D2A50 / lbl_804D4738), lbl_804D2A54 * scale);
            GfxSetBlendMode(1, 4, 1, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, r, g, b, a);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@80095210
// Draws a frame-buffer copy through CubeTevSetupRingAndBlend at the given scale with colour (r, g, b, a).
extern "C" void fn_80095210(int r, int g, int b, int a, float scale);
void fn_80095210(int r, int g, int b, int a, float scale) {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            CubeTevSetupRingAndBlend(scale * (lbl_804D2A50 / lbl_804D4738), lbl_804D2A54 * scale);
            GfxSetBlendMode(1, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, r, g, b, a);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@80095344
// Draws two frame-buffer copies through CubeTevSetupRingIndirect at the given scale with colour (r, g, b, a).
extern "C" void fn_80095344(int r, int g, int b, int a, float scale);
void fn_80095344(int r, int g, int b, int a, float scale) {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GXTexObj* tex2;
        GfxFbCommit();
        GfxSetCopyFilter(0);
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            tex2 = (GXTexObj*)fn_80115488(0, 1, 0);
            GfxLoadTexObj(tex, 0);
            GXLoadTexObj(tex2, GX_TEXMAP1);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            CubeTevSetupRingIndirect(scale * (lbl_804D2A50 / lbl_804D4738), lbl_804D2A54 * scale);
            GfxSetBlendMode(1, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, r, g, b, a);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@80095490
// Copies the frame buffer to a texture and draws it back full-screen.
extern "C" void fn_80095490();
void fn_80095490() {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        GfxSetCopyFilter(0);
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            CubeTevSetupChannelSwap();
            GfxSetBlendMode(1, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, 0xFF, 0xFF, 0xFF, 0xFF);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@80095570
// Draws a frame-buffer copy through CubeTevSetupRingBlend15 at the given scale with colour (r, g, b, a).
extern "C" void fn_80095570(int r, int g, int b, int a, float scale);
void fn_80095570(int r, int g, int b, int a, float scale) {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GfxFbCommit();
        tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        if (tex != 0) {
            GfxLoadTexObj(tex, 0);
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            CubeTevSetupRingBlend15(scale * (lbl_804D2A50 / lbl_804D4738), lbl_804D2A54 * scale);
            GfxSetBlendMode(1, 4, 5, 3);
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetColorUpdate(GX_TRUE);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            fn_8009C4A4(0, r, g, b, a);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@800956A4
// Draws two frame-buffer copies through the two-stage indirect TEV with colour (r, g, b, a).
extern "C" void fn_800956A4(int r, int g, int b, int a);
void fn_800956A4(int r, int g, int b, int a) {
    if (lbl_804D4154 == 0) {
        GXTexObj* tex;
        GXTexObj* tex2;
        GfxFbCommit();
        GfxSetCopyFilter(0);
        tex = (GXTexObj*)fn_80115850(0, 1, 0);
        tex2 = (GXTexObj*)fn_80115488(0, 1, 0);
        GfxLoadTexObj(tex, 0);
        GXLoadTexObj(tex2, GX_TEXMAP1);
        GfxSetVtxDesc(2);
        fn_80008D10();
        GfxSetMaterial(0);
        CubeTevSetupIndirectTwo();
        GfxSetBlendMode(1, 4, 5, 3);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_TRUE);
        GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
        GfxSetCullMode(1);
        fn_8009C4A4(0, r, g, b, a);
        fn_8000FF7C();
        fn_800097F0();
    }
}
//@@end
//@@800957C4
// Draws a frame-buffer copy with colour (r, g, b, a).
extern "C" void fn_800957C4(int r, int g, int b, int a);
void fn_800957C4(int r, int g, int b, int a) {
    if (lbl_804D4154 == 0) {
        GfxFbCommit();
        GfxSetCopyFilter(0);
        GfxLoadTexObj((GXTexObj*)fn_80115488(0, 1, 0), 0);
        GfxSetVtxDesc(2);
        fn_80008D10();
        GfxSetMaterial(0);
        fn_8010883C();
        GfxSetBlendMode(1, 4, 5, 3);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_TRUE);
        GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
        GfxSetCullMode(1);
        fn_8009C4A4(0, r, g, b, a);
        fn_8000FF7C();
        fn_800097F0();
    }
}
//@@end
// TODO (not drafted yet, 0xFC bytes at 0x800958CC)
extern "C" void fn_800958CC();
//@@800959C8
// Draws texture tex through fn_8009925C (a, b), then darkens the screen.
extern "C" void fn_800959C8(GXTexObj* tex, float a, float b);
void fn_800959C8(GXTexObj* tex, float a, float b) {
    if (lbl_804D4154 == 0) {
        fn_8010F7EC();
        GfxSetVtxDesc(2);
        GfxSetMaterial(0);
        GfxResetTev();
        fn_80008D10();
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetAlphaUpdate(GX_FALSE);
        GXSetColorUpdate(GX_TRUE);
        GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
        GfxSetCullMode(1);
        GfxSetBlendMode(1, 4, 5, 3);
        GfxLoadTexObj(tex, 0);
        fn_8009925C(0x7F, a, b);
        fn_8010CDD8();
        GfxSetBlendMode(1, 6, 7, 3);
        fn_8009C4A4(0, 0, 0, 0, 0xFF);
        fn_8000FF7C();
        fn_800097F0();
    }
}
//@@end
// TODO (not drafted yet, 0x288 bytes at 0x80095AD8)
extern "C" void fn_80095AD8();
//@@80095D60
// Draws texture tex full-screen with parameter t.
extern "C" void fn_80095D60(GXTexObj* tex, float t);
void fn_80095D60(GXTexObj* tex, float t) {
    if (lbl_804D4154 == 0) {
        GfxSetVtxDesc(2);
        GfxSetMaterial(0);
        GfxResetTev();
        fn_80008D10();
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetAlphaUpdate(GX_FALSE);
        GXSetColorUpdate(GX_TRUE);
        GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
        GfxSetCullMode(1);
        GfxSetBlendMode(1, 6, 7, 3);
        GfxLoadTexObj(tex, 0);
        fn_8009B940(t);
        fn_8000FF7C();
        fn_800097F0();
    }
}
//@@end
// TODO (not drafted yet, 0x378 bytes at 0x80095E28)
extern "C" void fn_80095E28();
//@@800961A0
// Draws texture tex masked by alpha > ref with colour (r, g, b) (blend mode 3).
extern "C" void fn_800961A0(u8 ref, int r, int g, int b, GXTexObj* tex, int mode);
void fn_800961A0(u8 ref, int r, int g, int b, GXTexObj* tex, int mode) {
    if (lbl_804D4154 == 0) {
        GfxSetVtxDesc(2);
        GfxSetMaterial(0);
        CubeTevOneStageSwapC();
        fn_80008D10();
        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_FALSE);
        GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
        GfxSetCullMode(1);
        GXSetAlphaCompare(GX_GREATER, ref, GX_AOP_AND, GX_ALWAYS, 0);
        GfxLoadTexObj(tex, 0);
        GfxSetBlendMode(3, 1, 1, 3);
        fn_8009C4A4(mode, r, g, b, 0xFF);
        fn_8000FF7C();
        fn_800097F0();
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    }
}
//@@end
//@@80096298
// Draws texture tex masked by alpha > ref with colour (r, g, b) (blend mode 1).
extern "C" void fn_80096298(u8 ref, int r, int g, int b, GXTexObj* tex, int mode);
void fn_80096298(u8 ref, int r, int g, int b, GXTexObj* tex, int mode) {
    if (lbl_804D4154 == 0) {
        GfxSetVtxDesc(2);
        GfxSetMaterial(0);
        CubeTevOneStageSwapC();
        fn_80008D10();
        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_FALSE);
        GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
        GfxSetCullMode(1);
        GXSetAlphaCompare(GX_GREATER, ref, GX_AOP_AND, GX_ALWAYS, 0);
        GfxLoadTexObj(tex, 0);
        GfxSetBlendMode(1, 1, 1, 3);
        fn_8009C4A4(mode, r, g, b, 0xFF);
        fn_8000FF7C();
        fn_800097F0();
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    }
}
//@@end
// TODO (not drafted yet, 0x1F4 bytes at 0x80096390)
extern "C" void fn_80096390();
// TODO (not drafted yet, 0x234 bytes at 0x80096584)
extern "C" void fn_80096584();
// TODO (not drafted yet, 0x378 bytes at 0x800967B8)
extern "C" void fn_800967B8();
// TODO (not drafted yet, 0x144 bytes at 0x80096B30)
extern "C" void fn_80096B30();
// TODO (not drafted yet, 0xE8 bytes at 0x80096C74)
extern "C" void fn_80096C74();
//@@80096D5C
// Sets the colour (144, 255, 160) scaled by f, unless drawing is disabled.
extern "C" void fn_80096D5C(float f);
void fn_80096D5C(float f) {
    if (lbl_804D4154 == 0) {
        fn_80098244((u8)(144.0f * f), (u8)(255.0f * f), (u8)(160.0f * f));
    }
}
//@@end
// TODO (not drafted yet, 0x98C bytes at 0x80096DD0)
extern "C" void fn_80096DD0();
// TODO (not drafted yet, 0x580 bytes at 0x8009775C)
extern "C" void fn_8009775C();
// TODO (not drafted yet, 0xF4 bytes at 0x80097CDC)
extern "C" void fn_80097CDC();
// TODO (not drafted yet, 0x128 bytes at 0x80097DD0)
extern "C" void fn_80097DD0();
//@@80097EF8
// Draws texture texArg (or a frame-buffer copy) additively.
extern "C" void fn_80097EF8(int texArg);
void fn_80097EF8(int texArg) {
    GXTexObj* tex;
    if (lbl_804D4154 == 0) {
        if (texArg != 0) {
            tex = (GXTexObj*)texArg;
        } else {
            GfxFbCommit();
            tex = (GXTexObj*)fn_80115850(0, 1, 0);
        }
        if (tex != 0) {
            GfxSetVtxDesc(2);
            GfxSetMaterial(0);
            CubeTevOneStageTextured();
            fn_80008D10();
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            GfxSetBlendMode(1, 2, 2, 3);
            GfxLoadTexObj(tex, 0);
            fn_8009C4A4(0, 0xFF, 0xFF, 0xFF, 0xFF);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
//@@80097FF0
// Draws texture texArg (or a frame-buffer copy) with colour (r, g, b, a).
extern "C" void fn_80097FF0(int r, int g, int b, int a, int texArg);
void fn_80097FF0(int r, int g, int b, int a, int texArg) {
    GXTexObj* tex;
    if (lbl_804D4154 == 0) {
        if (texArg != 0) {
            tex = (GXTexObj*)texArg;
        } else {
            GfxFbCommit();
            tex = (GXTexObj*)fn_80115850(0, 1, 0);
        }
        if (tex != 0) {
            GfxSetVtxDesc(2);
            GfxSetMaterial(0);
            CubeTevOneStageTextured();
            fn_80008D10();
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(1);
            GfxSetBlendMode(1, 4, 5, 3);
            GfxLoadTexObj(tex, 0);
            fn_8009C4A4(0, r, g, b, a);
            fn_8000FF7C();
            fn_800097F0();
        }
    }
}
//@@end
// TODO (not drafted yet, 0x14C bytes at 0x800980F8)
extern "C" void fn_800980F8();
// TODO (not drafted yet, 0x1B8 bytes at 0x80098244)
// (declared above)
// TODO (not drafted yet, 0x120 bytes at 0x800983FC)
extern "C" void fn_800983FC();
// TODO (not drafted yet, 0x120 bytes at 0x8009851C)
extern "C" void fn_8009851C();
// TODO (not drafted yet, 0x140 bytes at 0x8009863C)
extern "C" void fn_8009863C();
//@@8009877C
// Draws texture texArg (or a frame-buffer copy) through fn_8009ADC8 with point filtering.
extern "C" void fn_8009877C(int mode, int texArg, float a, float b);
void fn_8009877C(int mode, int texArg, float a, float b) {
    GXTexObj* tex;
    if (lbl_804D4154 == 0) {
        if (texArg != 0) {
            tex = (GXTexObj*)texArg;
        } else {
            GfxFbCommit();
            tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        }
        if (tex != 0) {
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            CubeTevOneStageTextured();
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(0);
            GfxSetBlendMode(1, 4, 5, 3);
            GXInitTexObjFilter(tex, GX_LINEAR, GX_LINEAR);
            GfxLoadTexObj(tex, 0);
            fn_8009ADC8(mode, a, b);
            fn_8000FF7C();
            fn_800097F0();
            GfxSetCullMode(1);
        }
    }
}
//@@end
//@@800988A8
// Draws texture texArg (or a frame-buffer copy) through fn_8009B940 at time t.
extern "C" void fn_800988A8(int texArg, float t);
void fn_800988A8(int texArg, float t) {
    GXTexObj* tex;
    if (lbl_804D4154 == 0) {
        if (texArg != 0) {
            tex = (GXTexObj*)texArg;
        } else {
            GfxSetCopyFilter(0);
            GfxFbCommit();
            tex = (GXTexObj*)fn_80115A34(0, 1, 0);
        }
        if (tex != 0) {
            GfxSetVtxDesc(2);
            fn_80008D10();
            GfxSetMaterial(0);
            CubeTevOneStageTextured();
            GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
            GXSetAlphaUpdate(GX_TRUE);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
            GfxSetCullMode(0);
            GfxSetBlendMode(1, 4, 5, 3);
            GfxLoadTexObj(tex, 0);
            fn_8009B940(t);
            fn_8000FF7C();
            fn_800097F0();
            GfxSetCullMode(1);
        }
    }
}
//@@end
// TODO (not drafted yet, 0x280 bytes at 0x800989AC)
extern "C" void fn_800989AC();
// TODO (not drafted yet, 0x388 bytes at 0x80098C2C)
extern "C" void fn_80098C2C();
// TODO (not drafted yet, 0x284 bytes at 0x80098FB4)
extern "C" void fn_80098FB4();

// Stores a rectangle (x0, y0, x1, y1) as origin and size.
void AnimSetRect(float x0, float y0, float x1, float y1) {
    lbl_804D4728 = x0;
    lbl_804D472C = y0;
    lbl_804D2A50 = x1;
    lbl_804D2A54 = y1;
    lbl_804D2A58 = x1 - x0;
    lbl_804D2A5C = y1 - y0;
}

// TODO (not drafted yet, 0xB18 bytes at 0x8009925C)
// (declared above)
// TODO (not drafted yet, 0x1A4 bytes at 0x80099D74)
// (declared above)
// TODO (not drafted yet, 0x668 bytes at 0x80099F18)
// (declared above)
// TODO (not drafted yet, 0x848 bytes at 0x8009A580)
extern "C" void fn_8009A580();
// TODO (not drafted yet, 0xB78 bytes at 0x8009ADC8)
// (declared above)
// TODO (not drafted yet, 0xB64 bytes at 0x8009B940)
// (declared above)
// TODO (not drafted yet, 0x228 bytes at 0x8009C4A4)
// (declared above)
//@@8009C6CC
// Static initialisers (__sinit_u_80093BF0_cpp, the .ctors entry): reciprocal screen size and aspect ratio.
float lbl_804D4730 = 1.0f / lbl_804D2568;
float lbl_804D4734 = 1.0f / lbl_804D256C;
float lbl_804D4738 = (float)lbl_804D2568 / lbl_804D256C;
//@@end

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

extern int lbl_804D4154;
extern int lbl_804D2568;    // screen width
extern int lbl_804D256C;    // screen height

extern int lbl_804D475C;
extern int lbl_804D4760;

extern "C" void fn_80098244(int r, int g, int b);
extern "C" void fn_80099D74(int a, int b, int c, int alpha);
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
// TODO (not drafted yet, 0x1A4 bytes at 0x80093DE4)
extern "C" void fn_80093DE4();
// TODO (not drafted yet, 0xA8 bytes at 0x80093F88)
extern "C" void fn_80093F88();
// TODO (not drafted yet, 0x120 bytes at 0x80094030)
extern "C" void fn_80094030();
// TODO (not drafted yet, 0x10C bytes at 0x80094150)
extern "C" void fn_80094150();

// Clears the two cached words.
extern "C" void fn_8009425C();
void fn_8009425C() {
    lbl_804D475C = 0;
    lbl_804D4760 = 0;
}

// TODO (not drafted yet, 0x170 bytes at 0x8009426C)
extern "C" void fn_8009426C();
// TODO (not drafted yet, 0xFC bytes at 0x800943DC)
extern "C" void fn_800943DC();
// TODO (not drafted yet, 0xF4 bytes at 0x800944D8)
extern "C" void fn_800944D8();
// TODO (not drafted yet, 0xFC bytes at 0x800945CC)
extern "C" void fn_800945CC();
// TODO (not drafted yet, 0xFC bytes at 0x800946C8)
extern "C" void fn_800946C8();
// TODO (not drafted yet, 0x2F8 bytes at 0x800947C4)
extern "C" void fn_800947C4();
// TODO (not drafted yet, 0x118 bytes at 0x80094ABC)
extern "C" void fn_80094ABC();
// TODO (not drafted yet, 0x118 bytes at 0x80094BD4)
extern "C" void fn_80094BD4();
// TODO (not drafted yet, 0x134 bytes at 0x80094CEC)
extern "C" void fn_80094CEC();
// TODO (not drafted yet, 0x160 bytes at 0x80094E20)
extern "C" void fn_80094E20();
// TODO (not drafted yet, 0x148 bytes at 0x80094F80)
extern "C" void fn_80094F80();
// TODO (not drafted yet, 0x148 bytes at 0x800950C8)
extern "C" void fn_800950C8();
// TODO (not drafted yet, 0x134 bytes at 0x80095210)
extern "C" void fn_80095210();
// TODO (not drafted yet, 0x14C bytes at 0x80095344)
extern "C" void fn_80095344();
// TODO (not drafted yet, 0xE0 bytes at 0x80095490)
extern "C" void fn_80095490();
// TODO (not drafted yet, 0x134 bytes at 0x80095570)
extern "C" void fn_80095570();
// TODO (not drafted yet, 0x120 bytes at 0x800956A4)
extern "C" void fn_800956A4();
// TODO (not drafted yet, 0x108 bytes at 0x800957C4)
extern "C" void fn_800957C4();
// TODO (not drafted yet, 0xFC bytes at 0x800958CC)
extern "C" void fn_800958CC();
// TODO (not drafted yet, 0x110 bytes at 0x800959C8)
extern "C" void fn_800959C8();
// TODO (not drafted yet, 0x288 bytes at 0x80095AD8)
extern "C" void fn_80095AD8();
// TODO (not drafted yet, 0xC8 bytes at 0x80095D60)
extern "C" void fn_80095D60();
// TODO (not drafted yet, 0x378 bytes at 0x80095E28)
extern "C" void fn_80095E28();
// TODO (not drafted yet, 0xF8 bytes at 0x800961A0)
extern "C" void fn_800961A0();
// TODO (not drafted yet, 0xF8 bytes at 0x80096298)
extern "C" void fn_80096298();
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
// TODO (not drafted yet, 0xF8 bytes at 0x80097EF8)
extern "C" void fn_80097EF8();
// TODO (not drafted yet, 0x108 bytes at 0x80097FF0)
extern "C" void fn_80097FF0();
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
// TODO (not drafted yet, 0x12C bytes at 0x8009877C)
extern "C" void fn_8009877C();
// TODO (not drafted yet, 0x104 bytes at 0x800988A8)
extern "C" void fn_800988A8();
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
extern "C" void fn_8009925C();
// TODO (not drafted yet, 0x1A4 bytes at 0x80099D74)
// (declared above)
// TODO (not drafted yet, 0x668 bytes at 0x80099F18)
extern "C" void fn_80099F18();
// TODO (not drafted yet, 0x848 bytes at 0x8009A580)
extern "C" void fn_8009A580();
// TODO (not drafted yet, 0xB78 bytes at 0x8009ADC8)
extern "C" void fn_8009ADC8();
// TODO (not drafted yet, 0xB64 bytes at 0x8009B940)
extern "C" void fn_8009B940();
// TODO (not drafted yet, 0x228 bytes at 0x8009C4A4)
extern "C" void fn_8009C4A4();
//@@8009C6CC
// Static initialisers (__sinit_u_80093BF0_cpp, the .ctors entry): reciprocal screen size and aspect ratio.
float lbl_804D4730 = 1.0f / lbl_804D2568;
float lbl_804D4734 = 1.0f / lbl_804D256C;
float lbl_804D4738 = (float)lbl_804D2568 / lbl_804D256C;
//@@end

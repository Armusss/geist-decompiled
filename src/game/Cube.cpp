// Cube.cpp: 0x80005800-0x8000DC24, the GameCube platform layer: controller input mapping and shadowing,
// OS/DVD/VI/GX initialization, file reads, per-frame GX state setup, frame begin/swap/end, fatal errors.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/pad.h>

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
void GXSetScissorBoxOffset(int x, int y);
void GXSetCullMode(int mode);
void GXSetClipMode(int mode);
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
extern void* lbl_804D4024;
extern char lbl_8034EE8C[];
extern "C" char* strcpy(char* d, const char* s);
extern "C" void OSPanic(const char* file, int line, const char* msg, ...);
extern "C" int OSCheckHeap(void* heap);
extern "C" void PSMTXCopy(void* src, void* dst);
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
        GXSetCullMode(mode);
    } else {
        if (mode == 1) {
            mode = 2;
        } else if (mode == 2) {
            mode = 1;
        }
        GXSetCullMode(mode);
    }
}

extern "C" void fn_80008B94(int mode) {
    if (lbl_804D2548 != mode) {
        lbl_804D2548 = mode;
        GXSetClipMode(0);
    }
}

extern "C" void fn_80008BC8(void) { OSPanic("Cube.cpp", 0xCA8, ""); }
extern "C" void fn_80008C00(void) { OSPanic("Cube.cpp", 0xC7E, ""); }
void FatalError(const char* msg) { OSPanic("Cube.cpp", 0xC5F, ""); }

extern "C" float fn_80008C70(void) { return lbl_804D2540; }

extern "C" void fn_80008C78(void* m) { PSMTXCopy(lbl_803CFFE4, m); }

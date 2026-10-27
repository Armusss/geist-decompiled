// u_80112308.cpp: GX lighting / channel state cache (0x80112308-0x80116B28).
// Keeps a cached copy of the colour channel setup (lighting enable bits, ambient and material
// colours, colour sources) and pushes only the changes to GX; GfxSetMaterial applies a whole
// material description (and resets it when called with null).  Names are descriptive guesses;
// unmatched against retail so far.  Only the small helpers at the start of the file are drafted.

#include <dolphin/gx.h>

typedef unsigned long u32;
typedef unsigned char u8;

// Channel state defaults (0x8039ECA8) and current values (0x8039ECC0).
struct ChanState {
    int ambSrc0;            // 0x00
    int ambSrc1;            // 0x04
    GXColor ambColor;       // 0x08
    int matSrc0;            // 0x0C
    int matSrc1;            // 0x10
    GXColor matColor;       // 0x14
};
extern ChanState gChanDefaults;  // 0x8039ECA8
extern ChanState gChanCur;       // 0x8039ECC0
extern int gGfxApplied;          // 0x804D4C58
extern int gGfxWanted;           // 0x804D4C5C
extern int gGfxLightMask;        // 0x804D4C60
extern int gGfxUnk64;            // 0x804D4C64
extern int gGfxSpecular;         // 0x804D4C68
extern GXColor gGfxBlack;        // 0x804D2DF4
extern GXColor gGfxAmbient;      // 0x804D2DEC
extern GXColor gGfxMatDefault;   // 0x804D2DF0
extern int gGfxAmbientOnly;      // 0x804D2DE8

extern void GfxApplyChannels(void);   // 0x80114EC8
extern void GfxFlushState(void);      // 0x8010F7C0

// Caller-visible material description (0xE8 bytes or so); only the used fields are named.
struct GfxMaterial {
    int enable;             // 0x00
    int useAmb;             // 0x04
    int useSpec;            // 0x08
    u32 lightMask;          // 0x0C
    int unk10;              // 0x10
    int specular;           // 0x14
    int ambSrc0;            // 0x18
    int ambSrc1;            // 0x1C
    GXColor ambColor;       // 0x20
    int matSrc0;            // 0x24
    int matSrc1;            // 0x28
    GXColor matColor;       // 0x2C
};

// Luma of an RGB vector, clamped to 0..1.
float GfxLuma(float* rgb) {
    float v = rgb[1] * 0.114f + rgb[0] * 0.299f + rgb[2] * 0.587f;
    if (v < 0.0f) {
        v = 0.0f;
    }
    if (v > 1.0f) {
        v = 1.0f;
    }
    return v;
}

// 0x80112350: both lighting and ambient bits are set.
bool GfxLightingAmbientOn(void) {
    int r = 0;
    if ((gGfxApplied & 1) != 0 && (gGfxApplied & 8) != 0) {
        r = 1;
    }
    return r;
}

// 0x80112374: turn the channel-0/1 lighting enables on or off.
void GfxSetChannelEnables(int chan0, int chan1, int* out) {
    if (out != 0) {
        out[1] = chan0;
        out[2] = chan1;
    }
    if (chan0 != 0) {
        gGfxWanted |= 2;
    } else {
        gGfxWanted &= ~2;
    }
    if (chan1 != 0) {
        gGfxWanted |= 4;
    } else {
        gGfxWanted &= ~4;
    }
    if (gGfxApplied != gGfxWanted) {
        gGfxApplied = gGfxWanted;
        GfxApplyChannels();
        GfxFlushState();
    }
}

// 0x801123E4: turn the lighting enable on or off.
void GfxSetLightingEnable(int on, int* out) {
    int bits;
    if (out != 0) {
        *out = on;
    }
    bits = gGfxWanted & ~1;
    if (on != 0) {
        bits = gGfxWanted | 1;
    }
    gGfxWanted = bits;
    if (gGfxApplied != bits) {
        gGfxApplied = bits;
        GfxApplyChannels();
        GfxFlushState();
    }
}

// Is the cached material colour fully opaque or specular active?
inline int GfxBlendNeeded(void) {
    if (gChanCur.matSrc1 == 0 && gChanCur.matColor.a < 0xFF) {
        return 1;
    }
    if (gGfxSpecular == 0) {
        return 0;
    }
    return 1;
}

// 0x8011243C: set the material colour of channel 0.
int GfxSetMatColor(GXColor* color, GfxMaterial* out) {
    if (out != 0) {
        out->matColor = *color;
    }
    if (*(u32*)&gChanCur.matColor != *(u32*)color) {
        gChanCur.matColor = *color;
        if ((gGfxApplied & 8) == 0) {
            GXSetChanMatColor(GX_COLOR0A0, gChanCur.matColor);
        } else {
            GXSetChanMatColor(GX_COLOR0, gGfxAmbient);
            GXSetChanMatColor(GX_ALPHA0, gChanCur.matColor);
        }
    }
    return GfxBlendNeeded();
}

// 0x80112554: set the ambient colour of channel 0.
int GfxSetAmbColor(GXColor* color, GfxMaterial* out) {
    if (out != 0) {
        out->ambColor = *color;
    }
    if (*(u32*)&gChanCur.ambColor != *(u32*)color) {
        gChanCur.ambColor = *color;
        if ((gGfxApplied & 8) == 0) {
            if ((gGfxApplied & 1) != 0 && (gGfxApplied & 2) != 0) {
                if (gGfxAmbientOnly == 0) {
                    GXSetChanAmbColor(GX_COLOR0A0, gChanCur.ambColor);
                } else {
                    GXColor c;
                    c.r = color->r;
                    c.g = color->g;
                    c.b = color->b;
                    c.a = color->a;
                    GXSetChanAmbColor(GX_COLOR0A0, c);
                }
            }
        }
    }
    return GfxBlendNeeded();
}

// Push the colour channel setup (used by both functions below).
inline void GfxPushChanCtrl(void) {
    if ((gGfxApplied & 8) == 0) {
        if ((gGfxApplied & 1) == 0 || (gGfxApplied & 2) == 0) {
            GXSetChanCtrl(GX_COLOR0, GX_DISABLE, GX_SRC_VTX, (GXColorSrc)gChanCur.matSrc0, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
            GXSetChanCtrl(GX_ALPHA0, GX_DISABLE, GX_SRC_VTX, (GXColorSrc)gChanCur.matSrc1, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
        } else {
            GXSetChanCtrl(GX_COLOR0, GX_ENABLE, (GXColorSrc)gChanCur.ambSrc0, (GXColorSrc)gChanCur.matSrc0, gGfxLightMask,
                          GX_DF_CLAMP, GX_AF_SPOT);
            if (gGfxSpecular == 0) {
                GXSetChanCtrl(GX_ALPHA0, GX_DISABLE, (GXColorSrc)gChanCur.ambSrc1, (GXColorSrc)gChanCur.matSrc1, GX_LIGHT_NULL,
                              GX_DF_NONE, GX_AF_NONE);
            } else {
                GXSetChanCtrl(GX_ALPHA0, GX_ENABLE, (GXColorSrc)gChanCur.ambSrc1, (GXColorSrc)gChanCur.matSrc1, gGfxSpecular,
                              GX_DF_CLAMP, GX_AF_SPOT);
            }
        }
    } else {
        GXSetChanCtrl(GX_COLOR1A1, GX_DISABLE, GX_SRC_VTX, (GXColorSrc)gChanCur.matSrc1, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    }
}

// 0x80112678: set the four colour-source selectors.
int GfxSetColorSrcs(int amb0, int amb1, int mat0, int mat1, GfxMaterial* out) {
    if (out != 0) {
        out->ambSrc0 = amb0;
        out->ambSrc1 = amb1;
        out->matSrc0 = mat0;
        out->matSrc1 = mat1;
    }
    if (gChanCur.ambSrc0 != amb0 || gChanCur.ambSrc1 != amb1 || gChanCur.matSrc0 != mat0 || gChanCur.matSrc1 != mat1) {
        gChanCur.ambSrc0 = amb0;
        gChanCur.ambSrc1 = amb1;
        gChanCur.matSrc0 = mat0;
        gChanCur.matSrc1 = mat1;
        GfxPushChanCtrl();
    }
    return GfxBlendNeeded();
}

// 0x8011284C: set the material sources and colour in one go.
int GfxSetSrcsAndColor(int* args, GfxMaterial* out) {
    int mat0 = args[0];
    int mat1 = args[1];
    int color = args[2];
    if (out != 0) {
        out->matSrc0 = mat0;
        out->matSrc1 = mat1;
        out->matColor = *(GXColor*)&color;
    }
    if (gChanCur.matSrc0 != mat0 || gChanCur.matSrc1 != mat1) {
        gChanCur.matSrc0 = mat0;
        gChanCur.matSrc1 = mat1;
        GfxPushChanCtrl();
    }
    if (*(int*)&gChanCur.matColor != color) {
        *(int*)&gChanCur.matColor = color;
        if ((gGfxApplied & 8) == 0) {
            GXSetChanMatColor(GX_COLOR0A0, gChanCur.matColor);
        } else {
            GXSetChanMatColor(GX_COLOR0, gGfxAmbient);
            GXSetChanMatColor(GX_ALPHA0, gChanCur.matColor);
        }
    }
    return GfxBlendNeeded();
}

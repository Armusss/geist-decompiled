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

// Per-frame light statistics and the light-array double buffer.
extern int gLightStats[7];       // 0x8046B5B4..0x8046B5CC
extern int gLightStats2[7];      // 0x8046B598..0x8046B5B0
extern int gLightsLoaded;        // 0x804D4C78
extern int gLightsFrame;         // 0x804D4C70
extern int gLightsCalced;        // 0x804D4C74
extern u8* gLightsBuf;           // 0x804D4C6C
extern u8 gLightsPool[];         // 0x8045EA78
extern "C" void fn_801145FC(void);
extern "C" void fn_801140E8(int idx);
extern "C" void fn_8011498C(void);

// 0x80113A10: start a new lighting frame.
void GfxLightsBeginFrame(void) {
    gLightStats[6] = 0;
    gLightStats[5] = 0;
    gLightStats[4] = 0;
    gLightStats[3] = 0;
    gLightStats[2] = 0;
    gLightStats[1] = 0;
    gLightStats[0] = 0;
    gLightsLoaded = 0;
    gLightsFrame = (gLightsFrame + 1) & 3;
    gLightsCalced = 0;
    gLightsBuf = gLightsPool + gLightsFrame * 0x3200;
    GXSetArray((GXAttr)24, gLightsBuf, 0x40);
}

// 0x80113ACC: clear the second statistics block.
void GfxLightsClearStats(void) {
    gLightStats2[0] = 0;
    gLightStats2[1] = 0;
    gLightStats2[2] = 0;
    gLightStats2[3] = 0;
    gLightStats2[4] = 0;
    gLightStats2[5] = 0;
    gLightStats2[6] = 0;
}

// 0x80114040: allocate/look up a light by id (1..200); returns slot or -1.
int GfxLightSlot(int id) {
    int bit = id - 1;
    if (bit < 0 || bit >= 200) {
        id = -1;
    } else {
        int word = bit >> 5;
        u32 mask = 1 << (bit & 0x1F);
        if ((mask & gLightStats[word]) == 0) {
            gLightStats[word] |= mask;
            gLightsLoaded++;
            fn_801145FC();
            fn_801140E8(id - 1);
        }
        id = id - 1;
    }
    return id;
}

// 0x8011496C: thunk.
void GfxLightsFlush(void) {
    fn_8011498C();
}

extern int gFbA;                 // 0x804D4C94
extern int gFbB;                 // 0x804D4C98
extern int gFbC;                 // 0x804D4C88
extern int gFbD;                 // 0x804D4C8C
extern u8* gFbBase;              // 0x804D4C90
extern u8* gFbBase2;             // 0x804D4CA4
extern "C" u8* fn_802869DC(int size);
extern "C" void* fn_800A3990(void);
extern "C" void fn_800C7B14(void);
extern "C" float fn_8020D47C(void);
extern int MemFlagIsSet(void* flags, int bit);
extern void ConditionUpdateBit(void* flags, int bit, int on);

// 0x80115E10: viewport rectangle as shorts.
void GfxGetViewportRect(short* out) {
    float vp[6];
    GXGetViewportv(vp);
    out[0] = (short)(int)vp[0];
    out[2] = (short)(int)vp[1];
    out[1] = (short)(int)(vp[0] + vp[2]);
    out[3] = (short)(int)(vp[1] + vp[3]);
}

// 0x80115EA0: roll the saved counters forward.
void GfxFbCommit(void) {
    gFbA = gFbB;
    gFbC = gFbD;
}

// 0x80115EB4: save or clear the counters.
void GfxFbSave(int keep) {
    if (keep != 0) {
        gFbB = gFbA;
        gFbD = gFbC;
        return;
    }
    gFbB = 0;
    gFbD = 0;
}

// 0x80115EE0: allocate the frame buffer pool (kept as fn_80115EE0 in symbols.txt: called elsewhere).
void GfxFbInit(void) {
    gFbBase = fn_802869DC(0x14A000);
    gFbBase2 = gFbBase + 0xA5000;
    gFbA = 0;
    gFbB = 0;
    gFbD = 0;
}

struct CopyFilter {
    u8 aa;                  // 0x00
    u8 pattern[24];         // 0x01
    u8 vf;                  // 0x19
    u8 vfilter[7];          // 0x1A
};
extern CopyFilter gCopyFilters[];    // 0x8039ECE8

// 0x80115F28: select a copy filter preset.
void GfxSetCopyFilter(int i) {
    GXSetCopyFilter(gCopyFilters[i].aa, (u8(*)[2])gCopyFilters[i].pattern, gCopyFilters[i].vf, gCopyFilters[i].vfilter);
}

// 0x80115F68: light brightness scale by owner type.
float GfxOwnerBrightness(u32 id) {
    id = id >> 24;
    if (id == 0x13) {
        void* a = fn_800A3990();
        if (a == 0) {
            return 0.0f;
        }
        if (MemFlagIsSet((char*)a + 0x270, 0x18) != 0) {
            return 0.0f;
        }
        if (MemFlagIsSet((char*)a + 0x270, 0x19) == 0) {
            return 1.0f;
        }
        return 0.5f;
    }
    if (id < 0x13 && id == 1) {
        fn_800C7B14();
        return fn_8020D47C();
    }
    return 0.0f;
}

// 0x8011600C: mark the player as lit / dark.
void GfxMarkPlayerLit(float v) {
    void* a = fn_800A3990();
    if (a != 0) {
        if (1.0f <= v) {
            ConditionUpdateBit((char*)a + 0x270, 0x18, 1);
        } else {
            ConditionUpdateBit((char*)a + 0x270, 0x19, 1);
        }
    }
}

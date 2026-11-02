// CubeTev.cpp: TEV (texture environment) setups for full-screen effects and cube/sprite drawing
// (0x80104134-0x80112308). Each setup function resets the GX state with a handful of texture
// coordinate generators, TEV stages and register writes that the SDK helpers would have issued.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// The game's view of the GX library state (only the TEV words and the "BP sent" flag are touched).
struct GxState {
    u16 unk0;
    u16 bpSentNot;     // 0x02
    u8 unk4[0x130 - 4];
    u32 tevc[16];      // 0x130
    u32 teva[16];      // 0x170
};
extern GxState* __GXData;

// Writes one raster (BP) register to the graphics FIFO.
static inline void BpWrite(u32 value) {
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = value;
}

// Shared TEV bookkeeping that every setup function clears first (3 words per cached texture slot).
struct TevSlot {
    int tex;
    int unk4;
    int unk8;
};
extern TevSlot gTevSlots[8];   // 0x8045E800
extern int gTevPrevA;          // 0x804D2DC4
extern int gTevPrevB;          // 0x804D2DC8
extern Mtx gTevBaseMtx;        // 0x8045E860
extern GXTexObj gTevTexObj;    // 0x8045E970
extern "C" float fn_80008C70(void);

extern "C" void fn_8000FF7C(void);
extern "C" float fn_800A1394(float x);   // sine of a scaled angle
extern "C" float fn_800A1418(float x);   // cosine of a scaled angle
void GfxSetChannelEnables(int a, int b, int* c);   // 0x80112374

static inline void TevOrder(int stage, int coord, int map, int color) {
    GXSetTevOrder((GXTevStageID)stage, (GXTexCoordID)coord, (GXTexMapID)map, (GXChannelID)color);
}

static inline void TexGen(int dst, int func, int src, int mtx, int norm, int post) {
    GXSetTexCoordGen2((GXTexCoordID)dst, (GXTexGenType)func, (GXTexGenSrc)src, mtx, (GXBool)norm, post);
}

static inline void TevStateReset() {
    gTevPrevA = -1;
    gTevPrevB = -2;
    gTevSlots[0].tex = 0;
    gTevSlots[1].tex = 0;
    gTevSlots[2].tex = 0;
    gTevSlots[3].tex = 0;
    gTevSlots[4].tex = 0;
    gTevSlots[5].tex = 0;
    gTevSlots[6].tex = 0;
    gTevSlots[7].tex = 0;
    PSMTXIdentity(gTevBaseMtx);
    fn_8000FF7C();
}

// Starts a stage: selects its register block and clears the "BP sent" flag.
static inline void TevStageSelect(int stage) {
    BpWrite((0x10 + stage) << 24);
    __GXData->bpSentNot = 0;
}

// Writes a TEV color-env/alpha-env pair of constant bits for a stage (the SDK's In/Op words, packed).
static inline void TevStageWords(int stage, u32 colorBits, u32 alphaBits) {
    u32 c = ((0xC0 + stage * 2) << 24) | colorBits;
    BpWrite(c);
    GxState* g = __GXData;
    g->tevc[stage] = c;
    g->bpSentNot = 0;
    u32 a = ((0xC1 + stage * 2) << 24) | alphaBits;
    BpWrite(a);
    g = __GXData;
    g->teva[stage] = a;
    g->bpSentNot = 0;
}

// 0x80104134 n stages, each texture coordinate generated from a position matrix.
extern "C" void fn_80104134(int n) {   // called from other units, keeps its placeholder name
    int i;
    int mtx;
    int stage;
    GXSetNumTevStages(n);
    GXSetNumTexGens(n);
    mtx = 0x1E;
    for (i = 0; i < n; i++) {
        TexGen(i, 0, 0, mtx, 0, 0x7D);
        mtx += 3;
    }
    TevStageSelect(0);
    stage = 0;
    TevOrder(0, 0, 0, 0xFF);
    TevStageWords(stage, 0x88FFF, 0x89FF0);
    for (i = 1; i < n; i++) {
        stage++;
        TevStageSelect(stage);
        TevOrder(stage, i, i, 0xFF);
        TevStageWords(stage, 0x8F08F, 0x8E270);
    }
}

// 0x801042F4 animated warp: two scaled/rotated texture matrices driven by a 10-bit phase.
void CubeTevSetupWarp(u32 phase) {
    Mtx scale;
    Mtx rot;
    Mtx m;
    float ang;
    float a2;
    float sy;
    float sx;
    TevStateReset();
    GXSetNumTevStages(3);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetNumTexGens(2);
    GfxSetChannelEnables(0, 0, 0);
    TexGen(0, 1, 4, 0x1E, 0, 0x7D);
    TexGen(1, 1, 4, 0x21, 0, 0x7D);
    ang = 360.0f * ((float)(int)(phase & 0x3FF) / 1023.0f);
    a2 = 2.0f * ang;
    sx = fn_800A1394(3.0f * ang);
    sy = 0.15f * sx + 0.75f;
    sx = fn_800A1394(a2);
    PSMTXScale(scale, 0.15f * sx + 0.75f, sy, 1.0f);
    PSMTXTrans(m, -0.5f, -0.5f, 0.0f);
    PSMTXRotRad(rot, 'z', 0.017453292f * ang);
    PSMTXConcat(rot, m, m);
    PSMTXConcat(scale, m, m);
    PSMTXTransApply(m, m, 0.5f, 0.5f, 0.0f);
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
    sx = fn_800A1394(a2);
    sy = 0.15f * sx + 0.75f;
    sx = fn_800A1394(4.0f * ang);
    PSMTXScale(scale, 0.15f * sx + 0.75f, sy, 1.0f);
    PSMTXTrans(m, -0.5f, -0.5f, 0.0f);
    PSMTXRotRad(rot, 'z', 0.017453292f * -ang);
    PSMTXConcat(rot, m, m);
    PSMTXConcat(scale, m, m);
    PSMTXTransApply(m, m, 0.5f, 0.5f, 0.0f);
    GXLoadTexMtxImm(m, 0x21, GX_MTX2x4);
    TevStageSelect(0);
    TevOrder(0, 0, 0, 0xFF);
    TevStageWords(0, 0x8FC8F, 0x8FA70);
    TevStageSelect(1);
    TevOrder(1, 1, 0, 0xFF);
    TevStageWords(1, 0x8F08F, 0x8E270);
    TevStageSelect(2);
    TevOrder(2, 0xFF, 0xFF, 4);
    TevStageWords(2, 0x8F0AF, 0x8E2F0);
}

// 0x8010475C two scrolling texture matrices (phase in 13/12-bit fixed point).
extern "C" void fn_8010475C(u32 phase) {   // called from other units, keeps its placeholder name
    Mtx m;
    TevStateReset();
    GXSetNumTevStages(3);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetNumTexGens(2);
    GfxSetChannelEnables(0, 0, 0);
    TexGen(0, 1, 4, 0x1E, 0, 0x7D);
    TexGen(1, 1, 4, 0x21, 0, 0x7D);
    PSMTXTrans(m, 0.0f, (float)(int)(phase & 0x1FFF) / 8191.0f, 0.0f);
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
    PSMTXTrans(m, 0.5f, -((float)(int)(phase & 0xFFF) / 4095.0f), 0.0f);
    GXLoadTexMtxImm(m, 0x21, GX_MTX2x4);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_RED);
    TevStageSelect(0);
    TevOrder(0, 0, 0, 0xFF);
    TevStageWords(0, 0x8FFFF, 0x89FF4);
    TevStageSelect(1);
    TevOrder(1, 1, 0, 0xFF);
    TevStageWords(1, 0x8FFFF, 0x8E274);
    TevStageSelect(2);
    TevOrder(2, 0xFF, 0xFF, 4);
    TevStageWords(2, 0x8AFFF, 0x8E2F0);
}

// 0x80104AAC tinted four-layer wobble (seven stages): the phase drives two small texture offsets.
extern "C" void fn_80104AAC(u32 phase, u8 r, u8 g, u8 b) {   // called from other units
    Mtx m;
    GXColor color;
    float ang;
    double dy;
    double dx;
    TevStateReset();
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = 0xFF;
    GXSetTevColor(GX_TEVREG2, color);
    GXSetNumTevStages(7);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetNumTexGens(4);
    GfxSetChannelEnables(1, 0, 0);
    TexGen(0, 1, 4, 0x1E, 0, 0x7D);
    TexGen(1, 1, 4, 0x21, 0, 0x7D);
    TexGen(2, 1, 4, 0x24, 0, 0x7D);
    TexGen(3, 1, 4, 0x27, 0, 0x7D);
    ang = 360.0f * ((float)(int)(phase & 0x7FF) / 2047.0f);
    dx = (float)(0.005 * fn_800A1394(ang));
    dy = (float)(0.005 * fn_800A1418(ang));
    PSMTXTrans(m, 0.0f, (float)(int)(phase & 0x1FFF) / 8191.0f, 0.0f);
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
    dx = 0.02 + dx;
    dy = 0.02 + dy;
    PSMTXTransApply(m, m, (float)dx, (float)dy, 0.0f);
    GXLoadTexMtxImm(m, 0x24, GX_MTX2x4);
    PSMTXTrans(m, 0.5f, (float)(int)(phase & 0x3FFF) / -16383.0f, 0.0f);
    GXLoadTexMtxImm(m, 0x21, GX_MTX2x4);
    PSMTXTransApply(m, m, (float)dx, (float)dy, 0.0f);
    GXLoadTexMtxImm(m, 0x27, GX_MTX2x4);
    TevStageSelect(0);
    TevOrder(0, 0, 0, 0xFF);
    TevStageWords(0, 0x8FC8F, 0x8FA70);
    TevStageSelect(1);
    TevOrder(1, 1, 0, 0xFF);
    TevStageWords(1, 0x4C8FF0, 0x48FF80);
    TevStageSelect(2);
    TevOrder(2, 2, 0, 0xFF);
    TevStageWords(2, 0x8FC8F, 0x8FA70);
    TevStageSelect(3);
    TevOrder(3, 3, 0, 0xFF);
    TevStageWords(3, 0x8C8FF0, 0x88FF80);
    TevStageSelect(4);
    TevOrder(4, 0xFF, 0xFF, 0xFF);
    TevStageWords(4, 0x0C2FF4, 0x183FA0);
    TevStageSelect(5);
    TevOrder(5, 0xFF, 0xFF, 0xFF);
    TevStageWords(5, 0x086FF0, 0x08FF80);
    TevStageSelect(6);
    TevOrder(6, 0xFF, 0xFF, 4);
    TevStageWords(6, 0x8F0AF, 0x8E2F0);
}

// 0x80105188 rotating texture matrix for the stage-0 coordinates.
extern "C" void fn_80105188(float angle) {   // called from other units
    Mtx m;
    Mtx rot;
    TevStateReset();
    PSMTXTrans(m, -0.5f, -0.5f, 0.0f);
    PSMTXRotRad(rot, 'z', 0.017453292f * angle);
    PSMTXConcat(rot, m, m);
    PSMTXTransApply(m, m, 0.5f, 0.5f, 0.0f);
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
}

// 0x80105260 four swirling texture matrices driven by a 15-bit phase.
extern "C" void fn_80105260(u32 phase) {   // called from other units
    Mtx m;
    float t;
    TevStateReset();
    GXSetNumTevStages(5);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetNumTexGens(4);
    GfxSetChannelEnables(1, 0, 0);
    TexGen(0, 1, 4, 0x1E, 0, 0x7D);
    TexGen(1, 1, 4, 0x21, 0, 0x7D);
    TexGen(2, 1, 4, 0x24, 0, 0x7D);
    TexGen(3, 1, 4, 0x27, 0, 0x7D);
    t = (float)(int)(phase & 0x7FFF) / 32767.0f;
    PSMTXRotRad(m, 'z', 0.017453292f * (360.0f * t));
    PSMTXTransApply(m, m, 0.0f, -t, 0.0f);
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
    PSMTXRotRad(m, 'z', 0.017453292f * (-360.0f * t));
    PSMTXTransApply(m, m, 0.5f, t, 0.0f);
    GXLoadTexMtxImm(m, 0x21, GX_MTX2x4);
    PSMTXTrans(m, 0.5f, t, 0.0f);
    GXLoadTexMtxImm(m, 0x24, GX_MTX2x4);
    PSMTXTrans(m, t, 0.5f, 0.0f);
    GXLoadTexMtxImm(m, 0x27, GX_MTX2x4);
    TevStageSelect(0);
    TevOrder(0, 0, 0, 0xff);
    TevStageWords(0, 0x08FC8F, 0x08FA70);
    TevStageSelect(1);
    TevOrder(1, 1, 0, 0xff);
    TevStageWords(1, 0x08F08F, 0x08E270);
    TevStageSelect(2);
    TevOrder(2, 2, 0, 0xff);
    TevStageWords(2, 0x08F08F, 0x08E270);
    TevStageSelect(3);
    TevOrder(3, 3, 0, 0xff);
    TevStageWords(3, 0x08F08F, 0x08E270);
    TevStageSelect(4);
    TevOrder(4, 0xff, 0xff, 4);
    TevStageWords(4, 0x08F0AF, 0x08E2F0);
}

// 0x8010576C seven-stage rotating/scrolling blend with a quarter-turn base matrix.
extern "C" void fn_8010576C(u32 phase) {   // called from other units
    Mtx base;
    Mtx m;
    TevStateReset();
    GXSetNumTevStages(7);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetNumTexGens(4);
    GfxSetChannelEnables(1, 0, 0);
    TexGen(0, 1, 4, 0x1E, 0, 0x7D);
    TexGen(1, 1, 4, 0x21, 0, 0x7D);
    TexGen(2, 1, 4, 0x24, 0, 0x7D);
    TexGen(3, 1, 4, 0x27, 0, 0x7D);
    PSMTXRotRad(base, 'z', -1.5707964f);
    PSMTXRotRad(m, 'z', 0.017453292f * 360.0f * ((float)(int)(phase & 0x3FFF) / 16383.0f));
    GXLoadTexMtxImm(m, 0x24, GX_MTX2x4);
    PSMTXConcat(m, base, m);
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
    PSMTXTrans(m, 0.0f, (float)(int)(phase & 0x1FFF) / 8191.0f, 0.0f);
    GXLoadTexMtxImm(m, 0x27, GX_MTX2x4);
    PSMTXConcat(m, base, m);
    GXLoadTexMtxImm(m, 0x21, GX_MTX2x4);
    TevStageSelect(0);
    TevOrder(0, 0, 0, 0xff);
    TevStageWords(0, 0x08FC8F, 0x08FA70);
    TevStageSelect(1);
    TevOrder(1, 1, 0, 0xff);
    TevStageWords(1, 0x08F08F, 0x08E270);
    TevStageSelect(2);
    TevOrder(2, 0xff, 0xff, 4);
    TevStageWords(2, 0x48F0AF, 0x48E2F0);
    TevStageSelect(3);
    TevOrder(3, 2, 1, 0xff);
    TevStageWords(3, 0x08FC8F, 0x08FA70);
    TevStageSelect(4);
    TevOrder(4, 3, 1, 0xff);
    TevStageWords(4, 0x08F08F, 0x08E270);
    TevStageSelect(5);
    TevOrder(5, 0xff, 0xff, 4);
    TevStageWords(5, 0x08F0AF, 0x08E2F0);
    TevStageSelect(6);
    TevOrder(6, 0xff, 0xff, 0xff);
    TevStageWords(6, 0x08F0C2, 0x08E310);
}

// 0x80105D84 rotating and scrolling two-stage blend.
extern "C" void fn_80105D84(u32 phase) {   // called from other units
    Mtx m;
    TevStateReset();
    GXSetNumTevStages(3);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetNumTexGens(2);
    GfxSetChannelEnables(1, 0, 0);
    TexGen(0, 1, 4, 0x1E, 0, 0x7D);
    TexGen(1, 1, 4, 0x21, 0, 0x7D);
    PSMTXRotRad(m, 'z', 0.017453292f * 360.0f * ((float)(int)(phase & 0x3FFF) / 16383.0f));
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
    PSMTXTrans(m, 0.0f, (float)(int)(phase & 0x1FFF) / 8191.0f, 0.0f);
    GXLoadTexMtxImm(m, 0x21, GX_MTX2x4);
    TevStageSelect(0);
    TevOrder(0, 0, 0, 0xff);
    TevStageWords(0, 0x08FC8F, 0x08FA70);
    TevStageSelect(1);
    TevOrder(1, 1, 0, 0xff);
    TevStageWords(1, 0x08F08F, 0x08E270);
    TevStageSelect(2);
    TevOrder(2, 0xff, 0xff, 4);
    TevStageWords(2, 0x08F0AF, 0x08E2F0);
}

// 0x801060C4 indirect "heat haze" warp: a texture object distorts the second texture coordinate.
extern "C" void fn_801060C4(float strength, GXColor* color) {   // called from other units
    GXColor c;
    float ind[2][3];
    Mtx m;
    float fov;
    gTevPrevA = -1;
    gTevPrevB = -2;
    gTevSlots[0].tex = 0;
    gTevSlots[1].tex = 0;
    gTevSlots[2].tex = 0;
    gTevSlots[3].tex = 0;
    gTevSlots[4].tex = 0;
    gTevSlots[5].tex = 0;
    gTevSlots[6].tex = 0;
    gTevSlots[7].tex = 0;
    PSMTXIdentity(gTevBaseMtx);
    fn_8000FF7C();
    GXSetNumTevStages(1);
    GXSetNumIndStages(1);
    GXSetNumChans(0);
    GXSetNumTexGens(2);
    GfxSetChannelEnables(0, 0, 0);
    GXLoadTexObj(&gTevTexObj, GX_TEXMAP1);
    c = *color;
    GXSetTevColor(GX_TEVREG0, c);
    TexGen(0, 0, 0, 0x39, 0, 0x43);
    TexGen(1, 0, 1, 0x36, 0, 0x46);
    fov = fn_80008C70();
    C_MTXLightPerspective(m, fov, 1.3333334f, 0.5f, -0.5f, 0.5f, 0.5f);
    GXLoadTexMtxImm(m, 0x43, GX_MTX3x4);
    PSMTXScale(m, 1.0f, -1.0f, 1.0f);
    GXLoadTexMtxImm(m, 0x46, GX_MTX3x4);
    ind[0][1] = 0.04f * strength;
    ind[0][0] = 0.0f;
    ind[0][2] = 0.0f;
    ind[1][2] = 0.0f;
    ind[1][1] = 0.0f;
    ind[1][0] = ind[0][1];
    GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD1, GX_TEXMAP1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);
    GXSetIndTexMtx(GX_ITM_0, ind, 1);
    GXSetTevIndWarp(GX_TEVSTAGE0, GX_INDTEXSTAGE0, GX_TRUE, GX_FALSE, GX_ITM_0);
    TevOrder(0, 0, 0, 0xff);
    TevStageWords(0, 0x08F82F, 0x083FF0);
}

// 0x801062F4 hexagonal ring of seven offset samples (a spread, x horizontally and y vertically scaled).
extern "C" void fn_801062F4(float x, float y) {   // called from other units
    Mtx m;
    float hx;
    float hy;
    float nx;
    GXColor c;
    TevStateReset();
    GXSetNumTevStages(9);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetNumTexGens(7);
    GfxSetChannelEnables(0, 0, 0);
    PSMTXTrans(m, x, 0.0f, 0.0f);
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
    PSMTXTrans(m, -1.0f * x, 0.0f, 0.0f);
    GXLoadTexMtxImm(m, 0x21, GX_MTX2x4);
    hx = 0.5f * x;
    hy = 0.866f * y;
    PSMTXTrans(m, hx, hy, 0.0f);
    GXLoadTexMtxImm(m, 0x24, GX_MTX2x4);
    nx = -0.5f * x;
    PSMTXTrans(m, nx, hy, 0.0f);
    GXLoadTexMtxImm(m, 0x27, GX_MTX2x4);
    hy = -0.866f * y;
    PSMTXTrans(m, hx, hy, 0.0f);
    GXLoadTexMtxImm(m, 0x2A, GX_MTX2x4);
    PSMTXTrans(m, nx, hy, 0.0f);
    GXLoadTexMtxImm(m, 0x2D, GX_MTX2x4);
    TexGen(0, 1, 4, 0x3C, 0, 0x7D);
    TexGen(1, 1, 4, 0x1E, 0, 0x7D);
    TexGen(2, 1, 4, 0x21, 0, 0x7D);
    TexGen(3, 1, 4, 0x24, 0, 0x7D);
    TexGen(4, 1, 4, 0x27, 0, 0x7D);
    TexGen(5, 1, 4, 0x2A, 0, 0x7D);
    TexGen(6, 1, 4, 0x2D, 0, 0x7D);
    c.r = c.g = c.b = c.a = 0x55;
    GXSetTevColor(GX_TEVREG2, c);
    TevStageSelect(0);
    TevOrder(0, 1, 0, 0xff);
    TevStageWords(0, 0x00F86F, 0x08FFF0);
    TevStageSelect(1);
    TevOrder(1, 2, 0, 0xff);
    TevStageWords(1, 0x00F860, 0x08FFF0);
    TevStageSelect(2);
    TevOrder(2, 3, 0, 0xff);
    TevStageWords(2, 0x00F860, 0x08FFF0);
    TevStageSelect(3);
    TevOrder(3, 4, 0, 0xff);
    TevStageWords(3, 0x00F860, 0x08FFF0);
    TevStageSelect(4);
    TevOrder(4, 5, 0, 0xff);
    TevStageWords(4, 0x00F860, 0x08FFF0);
    TevStageSelect(5);
    TevOrder(5, 6, 0, 0xff);
    TevStageWords(5, 0x38F860, 0x08FFF0);
    TevStageSelect(6);
    TevOrder(6, 0, 0, 0xff);
    TevStageWords(6, 0x0C8FF0, 0x08FFF0);
    TevStageSelect(7);
    TevOrder(7, 0xff, 0xff, 4);
    TevStageWords(7, 0x3B0FA0, 0x08FFF0);
    TevStageSelect(8);
    TevOrder(8, 0xff, 0xff, 0xff);
    TevStageWords(8, 0x08240F, 0x08DFF0);
}

// 0x80106AE0 the same seven-sample ring with thirteen stages (blends a texture with its offsets).
extern "C" void fn_80106AE0(float x, float y) {   // called from other units
    Mtx m;
    float hx;
    float hy;
    float nx;
    TevStateReset();
    GXSetNumTevStages(13);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetNumTexGens(7);
    GfxSetChannelEnables(0, 0, 0);
    PSMTXTrans(m, x, 0.0f, 0.0f);
    GXLoadTexMtxImm(m, 0x1E, GX_MTX2x4);
    PSMTXTrans(m, -1.0f * x, 0.0f, 0.0f);
    GXLoadTexMtxImm(m, 0x21, GX_MTX2x4);
    hx = 0.5f * x;
    hy = 0.866f * y;
    PSMTXTrans(m, hx, hy, 0.0f);
    GXLoadTexMtxImm(m, 0x24, GX_MTX2x4);
    nx = -0.5f * x;
    PSMTXTrans(m, nx, hy, 0.0f);
    GXLoadTexMtxImm(m, 0x27, GX_MTX2x4);
    hy = -0.866f * y;
    PSMTXTrans(m, hx, hy, 0.0f);
    GXLoadTexMtxImm(m, 0x2A, GX_MTX2x4);
    PSMTXTrans(m, nx, hy, 0.0f);
    GXLoadTexMtxImm(m, 0x2D, GX_MTX2x4);
    TexGen(0, 1, 4, 0x3C, 0, 0x7D);
    TexGen(1, 1, 4, 0x1E, 0, 0x7D);
    TexGen(2, 1, 4, 0x21, 0, 0x7D);
    TexGen(3, 1, 4, 0x24, 0, 0x7D);
    TexGen(4, 1, 4, 0x27, 0, 0x7D);
    TexGen(5, 1, 4, 0x2A, 0, 0x7D);
    TexGen(6, 1, 4, 0x2D, 0, 0x7D);
    TevStageSelect(0);
    TevOrder(0, 0, 0, 0xff);
    TevStageWords(0, 0x488FFF, 0x08FFF0);
    TevStageSelect(1);
    TevOrder(1, 1, 0, 0xff);
    TevStageWords(1, 0x3B28CF, 0x08FFF0);
    TevStageSelect(2);
    TevOrder(2, 1, 0, 0xff);
    TevStageWords(2, 0x48820F, 0x08FFF0);
    TevStageSelect(3);
    TevOrder(3, 2, 0, 0xff);
    TevStageWords(3, 0x3B28CF, 0x08FFF0);
    TevStageSelect(4);
    TevOrder(4, 2, 0, 0xff);
    TevStageWords(4, 0x48820F, 0x08FFF0);
    TevStageSelect(5);
    TevOrder(5, 3, 0, 0xff);
    TevStageWords(5, 0x3B28CF, 0x08FFF0);
    TevStageSelect(6);
    TevOrder(6, 3, 0, 0xff);
    TevStageWords(6, 0x48820F, 0x08FFF0);
    TevStageSelect(7);
    TevOrder(7, 4, 0, 0xff);
    TevStageWords(7, 0x3B28CF, 0x08FFF0);
    TevStageSelect(8);
    TevOrder(8, 4, 0, 0xff);
    TevStageWords(8, 0x48820F, 0x08FFF0);
    TevStageSelect(9);
    TevOrder(9, 5, 0, 0xff);
    TevStageWords(9, 0x3B28CF, 0x08FFF0);
    TevStageSelect(10);
    TevOrder(10, 5, 0, 0xff);
    TevStageWords(10, 0x48820F, 0x08FFF0);
    TevStageSelect(11);
    TevOrder(11, 6, 0, 0xff);
    TevStageWords(11, 0x3B28CF, 0x08FFF0);
    TevStageSelect(12);
    TevOrder(12, 6, 0, 4);
    TevStageWords(12, 0x08820F, 0x08BFF0);
}

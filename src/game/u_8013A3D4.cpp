// u_8013A3D4.cpp: 2D/3D draw helpers for the GX FIFO (names are descriptive guesses).

#include <dolphin/gx/GXGeometry.h>
#include <dolphin/gx/GXTransform.h>
#include <dolphin/gx/GXVert.h>

extern "C" f32 PSVECDistance(const f32* a, const f32* b);

extern "C" void GXGetProjectionv(f32* mtx);
extern "C" void GXGetViewportv(f32* vp);
extern "C" void GXSetAlphaUpdate(int enable);
extern "C" void GXSetColorUpdate(int enable);
extern "C" void GXSetZMode(int compare, int func, int update);
extern "C" void GXSetAlphaCompare(int comp0, u8 ref0, int op, int comp1, u8 ref1);

extern "C" void fn_8000FF7C(void);
extern "C" void fn_800097F0(void);
extern "C" void* fn_801D89A4(void* state);
extern "C" void fn_800A0FC4(f32* out, f32* in);

extern "C" void* lbl_804D4D20;

extern "C" f32 lbl_804D4D08;
extern "C" f32 lbl_804D4D0C;
extern "C" f32 lbl_804D4D10;
extern "C" f32 lbl_804D4D14;
extern "C" f32 lbl_804D4D18;
extern "C" f32 lbl_804D4D1C;

extern "C" f32 lbl_804D7A88;
extern "C" f32 lbl_804D7A8C;
extern "C" f32 lbl_804D7A90;
extern "C" f32 lbl_804D7A94;

// Projection / viewport / view-matrix cache filled in by the frame setup.
struct GXDrawCache {
    f32 proj[7];
    f32 vp[6];
    f32 mtx[3][4];
};
extern "C" GXDrawCache lbl_8046D2B0;

// Ends a 3D draw block (identical body to Draw2DEnd).
void Draw3DEnd(void)
{
    fn_8000FF7C();
    fn_801D89A4(lbl_804D4D20);
    fn_800097F0();
}

// Draws a screen-aligned, rotated, textured quad around a world-space point.
// width is measured along the camera's right axis; angle rotates the quad.
void DrawWorldQuad(f32 width, f32 angle, f32* pos, u8 r, u8 g, u8 b, u8 a)
{
    f32 p0[3];
    f32 p1[3];
    f32 rot[4];
    f32 radius;
    f32 x0, y0, x1, y1;
    f32 x2, y2, x3, y3;
    bool visible;

    GXProject(pos[0], pos[1], pos[2], lbl_8046D2B0.mtx, lbl_8046D2B0.proj,
              lbl_8046D2B0.vp, &p0[0], &p0[1], &p0[2]);
    p0[0] -= lbl_8046D2B0.vp[0];
    p0[1] -= lbl_8046D2B0.vp[1];

    GXProject(pos[0] + lbl_8046D2B0.mtx[0][0] * width,
              pos[1] + lbl_8046D2B0.mtx[0][1] * width,
              pos[2] + lbl_8046D2B0.mtx[0][2] * width,
              lbl_8046D2B0.mtx, lbl_8046D2B0.proj, lbl_8046D2B0.vp,
              &p1[0], &p1[1], &p1[2]);
    p1[0] -= lbl_8046D2B0.vp[0];
    p1[1] -= lbl_8046D2B0.vp[1];

    radius = PSVECDistance(p0, p1);

    visible = false;
    if ((lbl_804D4D10 < p0[0] + radius || p0[0] - radius < lbl_804D4D18) &&
        (lbl_804D4D14 < p0[1] + radius || p0[1] - radius < lbl_804D4D1C)) {
        rot[0] = lbl_804D7A88 + angle;
        rot[1] = lbl_804D7A8C + angle;
        fn_800A0FC4(rot, rot);
        x0 = p0[0] + radius * rot[2];
        y0 = p0[1] + radius * rot[0];
        x1 = p0[0] + radius * rot[3];
        y1 = p0[1] + radius * rot[1];

        rot[0] = lbl_804D7A90 + angle;
        rot[1] = lbl_804D7A94 + angle;
        fn_800A0FC4(rot, rot);
        x2 = p0[0] + radius * rot[2];
        y2 = p0[1] + radius * rot[0];
        x3 = p0[0] + radius * rot[3];
        y3 = p0[1] + radius * rot[1];
        visible = true;
    }

    if (visible) {
        GXBegin(GX_QUADS, GX_VTXFMT2, 4);
        GXPosition3f32(x0 * lbl_804D4D08, y0 * lbl_804D4D0C, p0[2]);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(0.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x1 * lbl_804D4D08, y1 * lbl_804D4D0C, p0[2]);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(0.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x2 * lbl_804D4D08, y2 * lbl_804D4D0C, p0[2]);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(1.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x3 * lbl_804D4D08, y3 * lbl_804D4D0C, p0[2]);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(1.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
    }
}

// Same as DrawWorldQuad, with the texture coordinates rotated one vertex.
void DrawWorldQuadFlipped(f32 width, f32 angle, f32* pos, u8 r, u8 g, u8 b, u8 a)
{
    f32 p0[3];
    f32 p1[3];
    f32 rot[4];
    f32 radius;
    f32 x0, y0, x1, y1;
    f32 x2, y2, x3, y3;
    bool visible;

    GXProject(pos[0], pos[1], pos[2], lbl_8046D2B0.mtx, lbl_8046D2B0.proj,
              lbl_8046D2B0.vp, &p0[0], &p0[1], &p0[2]);
    p0[0] -= lbl_8046D2B0.vp[0];
    p0[1] -= lbl_8046D2B0.vp[1];

    GXProject(pos[0] + lbl_8046D2B0.mtx[0][0] * width,
              pos[1] + lbl_8046D2B0.mtx[0][1] * width,
              pos[2] + lbl_8046D2B0.mtx[0][2] * width,
              lbl_8046D2B0.mtx, lbl_8046D2B0.proj, lbl_8046D2B0.vp,
              &p1[0], &p1[1], &p1[2]);
    p1[0] -= lbl_8046D2B0.vp[0];
    p1[1] -= lbl_8046D2B0.vp[1];

    radius = PSVECDistance(p0, p1);

    visible = false;
    if ((lbl_804D4D10 < p0[0] + radius || p0[0] - radius < lbl_804D4D18) &&
        (lbl_804D4D14 < p0[1] + radius || p0[1] - radius < lbl_804D4D1C)) {
        rot[0] = lbl_804D7A88 + angle;
        rot[1] = lbl_804D7A8C + angle;
        fn_800A0FC4(rot, rot);
        x0 = p0[0] + radius * rot[2];
        y0 = p0[1] + radius * rot[0];
        x1 = p0[0] + radius * rot[3];
        y1 = p0[1] + radius * rot[1];

        rot[0] = lbl_804D7A90 + angle;
        rot[1] = lbl_804D7A94 + angle;
        fn_800A0FC4(rot, rot);
        x2 = p0[0] + radius * rot[2];
        y2 = p0[1] + radius * rot[0];
        x3 = p0[0] + radius * rot[3];
        y3 = p0[1] + radius * rot[1];
        visible = true;
    }

    if (visible) {
        GXBegin(GX_QUADS, GX_VTXFMT2, 4);
        GXPosition3f32(x0 * lbl_804D4D08, y0 * lbl_804D4D0C, p0[2]);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(0.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x1 * lbl_804D4D08, y1 * lbl_804D4D0C, p0[2]);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(0.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x2 * lbl_804D4D08, y2 * lbl_804D4D0C, p0[2]);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(1.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(x3 * lbl_804D4D08, y3 * lbl_804D4D0C, p0[2]);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(1.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
    }
}

// Begins the 3D draw block: captures the current GX projection/viewport and
// sets up the FIFO state used by the world-quad helpers above.
extern "C" void fn_800092CC(void);
extern "C" void fn_8004320C(void* src, void* dst);
extern "C" void fn_80009324(void* p, int v);
extern "C" void fn_80008C78(void* p);
extern "C" void fn_80008D10(void);
extern "C" int fn_80010448(int a, int b);
extern "C" void fn_80010330(int a, int b);
extern "C" s32 lbl_804D2568;
extern "C" f32 lbl_804D2E7C;
extern "C" f32 lbl_804D2E80;
void GfxSetVtxDesc(int desc);
void GfxSetMaterial(void* mat);
void GfxSetCullMode(int mode);

void Draw3DBegin(int mode, void* setup)
{
    u8 buf[48];
    GXDrawCache* const cache = &lbl_8046D2B0;

    if (setup != 0) {
        fn_8004320C(setup, buf);
        fn_80009324(buf, 0);
    } else {
        fn_800092CC();
    }

    GXGetProjectionv(cache->proj);
    GXGetViewportv(cache->vp);
    fn_80008C78(cache->mtx);

    lbl_804D4D08 = 1.0f / cache->vp[2];
    lbl_804D4D0C = 1.0f / cache->vp[3];
    lbl_804D4D10 = cache->vp[0];
    lbl_804D4D14 = cache->vp[1];
    lbl_804D4D18 = cache->vp[0] + cache->vp[2];
    lbl_804D4D1C = cache->vp[1] + cache->vp[3];
    lbl_804D2E7C = cache->vp[2] / (f32)lbl_804D2568;
    lbl_804D2E80 = cache->vp[3] / (f32)lbl_804D2568;

    fn_80008D10();
    GfxSetVtxDesc(2);
    GfxSetMaterial(0);
    lbl_804D4D20 = fn_801D89A4(0);
    GXSetAlphaUpdate(1);
    GXSetColorUpdate(1);
    GfxSetCullMode(0);
    GXSetZMode(1, GX_LEQUAL, 0);

    switch (fn_80010448(mode, 1)) {
    case 2:
        fn_80010330(mode, 1);
        break;
    case 0:
    case 8:
        fn_80010448(-3, 1);
        break;
    }
}

// Draws a textured line strip whose vertices walk along a table of 24
// directions, interpolating the colour and the offsets across the strip.
extern "C" int rand(void);
extern "C" f32 lbl_804D7A98;
extern "C" f32 lbl_804D7A9C;
extern "C" f32 lbl_804D7AA0;
extern "C" f32 lbl_8046D314[];
extern "C" f32 lbl_803A0EE0[];

void DrawTrailStrip(int start, u8 r0, u8 g0, u8 b0, u8 a0,
                    u8 r1, u8 g1, u8 b1, u8 a1,
                    f32 height, f32 density, f32 x0, f32 x1, f32 jitter)
{
    int n = (int)(lbl_804D7A98 * density * height);
    if (n > 1) {
        f32 scale = lbl_804D7A9C;
        f32 denom = scale * (f32)n;
        f32 dr = (f32)(r0 - r1) / denom;
        f32 dg = (f32)(g0 - g1) / denom;
        f32 db = (f32)(b0 - b1) / denom;
        f32 da = (f32)(a0 - a1) / denom;
        f32 dy = height / (f32)n;
        f32 dx = (x1 - x0) / (f32)n;
        int idx = start % 24;
        int i;

        GXBegin(GX_LINESTRIP, GX_VTXFMT2, n);
        for (i = 0; i < n; i++) {
            const f32* rec = &lbl_8046D314[idx * 3];
            const f32* tc = &lbl_803A0EE0[(i & 1) * 2];
            f32 t = (f32)i;
            f32 j = jitter * ((f32)rand() / lbl_804D7AA0);
            f32 w = x0 + t * dx + j;

            GXPosition3f32(w * rec[0], t * dy, w * rec[2]);
            GXColor4u8((u8)(scale * (dr * t) + r0),
                       (u8)(scale * (dg * t) + g0),
                       (u8)(scale * (db * t) + b0),
                       (u8)(scale * (da * t) + a0));
            GXTexCoord2f32(tc[0], tc[1]);
            GXTexCoord2f32(0.0f, 0.0f);
            idx = (idx + 1) % 24;
        }
    }
}

// Ring buffer of trail vertices: the last `count` entries ending at `end` are
// drawn as one line strip, with the colour ramping by the stored steps.
extern "C" f32 lbl_804D2E78;

struct TrailBuffer {
    f32* verts;    // 0x00: xyz vertex ring, 12 bytes per vertex
    int count;     // 0x04: number of vertices to draw
    int end;       // 0x08: newest vertex index
    int max;       // 0x0C: ring size
    u8 base[4];    // 0x10: base colour
    f32 dr;        // 0x14
    f32 dg;        // 0x18
    f32 db;        // 0x1C
    f32 da;        // 0x20
};

void DrawTrailRing(TrailBuffer* p, u8 r, u8 g, u8 b, u8 a)
{
    f32 cr, cg, cb, ca;
    f32 sr, sg, sb, sa;
    int i, j;

    if (p->count <= 1)
        return;

    cr = lbl_804D2E78 * ((f32)p->base[0] * (f32)r);
    cg = lbl_804D2E78 * ((f32)p->base[1] * (f32)g);
    cb = lbl_804D2E78 * ((f32)p->base[2] * (f32)b);
    ca = lbl_804D2E78 * ((f32)p->base[3] * (f32)a);
    sr = lbl_804D2E78 * (p->dr * (f32)r);
    sg = lbl_804D2E78 * (p->dg * (f32)g);
    sb = lbl_804D2E78 * (p->db * (f32)b);
    sa = lbl_804D2E78 * (p->da * (f32)a);

    GXBegin(GX_LINESTRIP, GX_VTXFMT2, p->count);

    j = (p->end - p->count) + 1;
    if (j < 0)
        j += p->max;

    for (i = 0; j < p->max && i < p->count; i++, j++) {
        const f32* v = &p->verts[j * 3];
        const f32* tc = &lbl_803A0EE0[(j & 1) * 2];
        GXPosition3f32(v[0], v[1], v[2]);
        GXColor4u8((u8)cr, (u8)cg, (u8)cb, (u8)ca);
        GXTexCoord2f32(tc[0], tc[1]);
        GXTexCoord2f32(0.0f, 0.0f);
        cr += sr;
        cg += sg;
        cb += sb;
        ca += sa;
    }

    for (j = 0; j <= p->end && i < p->count; i++, j++) {
        const f32* v = &p->verts[j * 3];
        const f32* tc = &lbl_803A0EE0[(j & 1) * 2];
        GXPosition3f32(v[0], v[1], v[2]);
        GXColor4u8((u8)cr, (u8)cg, (u8)cb, (u8)ca);
        GXTexCoord2f32(tc[0], tc[1]);
        GXTexCoord2f32(0.0f, 0.0f);
        cr += sr;
        cg += sg;
        cb += sb;
        ca += sa;
    }
}

// Turns the texture-offset enables off and hands the cached material state back.
void DrawDisableTexOffsets(void)
{
    GXEnableTexOffsets(GX_TEXCOORD0, 0, 0);
    fn_801D89A4(lbl_804D4D20);
}

// Draws one GX line between two world-space points with a colour per endpoint.
void DrawLine(f32* p0, f32* p1, u8 r0, u8 g0, u8 b0, u8 a0,
              u8 r1, u8 g1, u8 b1, u8 a1)
{
    GXBegin(GX_LINES, GX_VTXFMT2, 2);
    GXPosition3f32(p0[0], p0[1], p0[2]);
    GXColor4u8(r0, g0, b0, a0);
    GXTexCoord2f32(1.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(p1[0], p1[1], p1[2]);
    GXColor4u8(r1, g1, b1, a1);
    GXTexCoord2f32(1.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
}

extern "C" f32 lbl_804D7AB0;
extern "C" f32 lbl_804D7AB4;

// Begins a 3D draw block like Draw3DBegin, with the line width taken from
// `value` clamped to [0.0f, lbl_804D7AB0].
void Draw3DBeginLine(int mode, void* setup, f32 value)
{
    u8 buf[48];
    f32 v;

    if (setup != 0) {
        fn_8004320C(setup, buf);
        fn_80009324(buf, 0);
    } else {
        fn_800092CC();
    }

    v = value < 0.0f ? 0.0f : (value > lbl_804D7AB0 ? lbl_804D7AB0 : value);

    GXEnableTexOffsets(GX_TEXCOORD0, 1, 0);
    GXSetLineWidth((u8)(int)(lbl_804D7AB4 * v), GX_TO_ONE);
    GfxSetVtxDesc(2);
    GfxSetMaterial(0);
    lbl_804D4D20 = fn_801D89A4(0);
    GXSetAlphaUpdate(1);
    GXSetColorUpdate(1);
    GfxSetCullMode(0);
    GXSetZMode(1, GX_LEQUAL, 0);

    switch (fn_80010448(mode, 1)) {
    case 2:
        fn_80010330(mode, 1);
        break;
    case 0:
    case 8:
        fn_80010448(-3, 1);
        break;
    }
}

// Turns the texture-offset enables off for the point pass and restores the
// cached material state.
void DrawDisablePointOffsets(void)
{
    GXEnableTexOffsets(GX_TEXCOORD0, 0, 0);
    fn_801D89A4(lbl_804D4D20);
}

// Emits a single GX point at a world-space position with a flat colour.
void DrawPoint(f32* pos, u8 r, u8 g, u8 b, u8 a)
{
    GXBegin(GX_POINTS, GX_VTXFMT2, 1);
    GXPosition3f32(pos[0], pos[1], pos[2]);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
}

// Begins a 3D point draw block, with the point size taken from `value`
// clamped to [0.0f, lbl_804D7AB0].
void Draw3DBeginPoint(int mode, void* setup, f32 value)
{
    u8 buf[48];
    f32 v;

    if (setup != 0) {
        fn_8004320C(setup, buf);
        fn_80009324(buf, 0);
    } else {
        fn_800092CC();
    }

    v = value < 0.0f ? 0.0f : (value > lbl_804D7AB0 ? lbl_804D7AB0 : value);

    GXEnableTexOffsets(GX_TEXCOORD0, 0, 1);
    GXSetPointSize((u8)(int)(lbl_804D7AB4 * v), GX_TO_ONE);
    GfxSetVtxDesc(2);
    GfxSetMaterial(0);
    lbl_804D4D20 = fn_801D89A4(0);
    GXSetAlphaUpdate(1);
    GXSetColorUpdate(1);
    GfxSetCullMode(0);
    GXSetZMode(1, GX_LEQUAL, 0);

    switch (fn_80010448(mode, 1)) {
    case 2:
        fn_80010330(mode, 1);
        break;
    case 0:
    case 8:
        fn_80010448(-3, 1);
        break;
    }
}

// Clears the texture-coordinate offset enables left over from a point/line
// pass that used them.
void DrawClearTexOffsets(void)
{
    GXEnableTexOffsets(GX_TEXCOORD0, 0, 0);
}

extern "C" f32 lbl_804D2E78;
// Two texture-coordinate rows, selected by the parity of the vertex index
// (declared as a flat array above; pairs are {x, y}).

struct DrawStripVertex {
    f32 x;
    f32 y;
    f32 z;
};

// Ring buffer of 2D vertices with a base colour and a per-vertex colour step.
struct DrawStrip {
    DrawStripVertex* verts;  // 0x00
    int count;               // 0x04
    int head;                // 0x08
    int capacity;            // 0x0C
    u8 color[4];             // 0x10
    f32 dr;                  // 0x14
    f32 dg;                  // 0x18
    f32 db;                  // 0x1C
    f32 da;                  // 0x20
};

// Draws the line strip held in the ring buffer `strip`, starting at the oldest
// vertex and fading the strip's base colour by the scale (r, g, b, a).
void DrawLineStrip(DrawStrip* strip, u8 r, u8 g, u8 b, u8 a)
{
    f32 cr, cg, cb, ca;
    f32 sr, sg, sb, sa;
    int i;
    int n;

    if (strip->count > 1) {
        cr = strip->color[0] * (f32)r * lbl_804D2E78;
        cg = strip->color[1] * (f32)g * lbl_804D2E78;
        cb = strip->color[2] * (f32)b * lbl_804D2E78;
        ca = strip->color[3] * (f32)a * lbl_804D2E78;
        sr = strip->dr * (f32)r * lbl_804D2E78;
        sg = strip->dg * (f32)g * lbl_804D2E78;
        sb = strip->db * (f32)b * lbl_804D2E78;
        sa = strip->da * (f32)a * lbl_804D2E78;

        GXBegin(GX_LINESTRIP, GX_VTXFMT2, (u16)strip->count);

        n = 0;
        i = (strip->head - strip->count) + 1;
        if (i < 0)
            i += strip->capacity;
        while (i < strip->capacity && n < strip->count) {
            GXPosition3f32(strip->verts[i].x, strip->verts[i].y, 0.0f);
            GXColor4u8((u8)cr, (u8)cg, (u8)cb, (u8)ca);
            GXTexCoord2f32(lbl_803A0EE0[(i & 1) * 2], lbl_803A0EE0[(i & 1) * 2 + 1]);
            GXTexCoord2f32(0.0f, 0.0f);
            cr += sr;
            cg += sg;
            cb += sb;
            ca += sa;
            i++;
            n++;
        }

        i = 0;
        while (i <= strip->head && n < strip->count) {
            GXPosition3f32(strip->verts[i].x, strip->verts[i].y, 0.0f);
            GXColor4u8((u8)cr, (u8)cg, (u8)cb, (u8)ca);
            GXTexCoord2f32(lbl_803A0EE0[(i & 1) * 2], lbl_803A0EE0[(i & 1) * 2 + 1]);
            GXTexCoord2f32(0.0f, 0.0f);
            cr += sr;
            cg += sg;
            cb += sb;
            ca += sa;
            i++;
            n++;
        }
    }
}

// Draws one GX line between two points projected to the 2D plane (z = 0) with
// a colour per endpoint.
void DrawLine2D(f32* p0, f32* p1, u8 r0, u8 g0, u8 b0, u8 a0,
                u8 r1, u8 g1, u8 b1, u8 a1)
{
    GXBegin(GX_LINES, GX_VTXFMT2, 2);
    GXPosition3f32(p0[0], p0[1], 0.0f);
    GXColor4u8(r0, g0, b0, a0);
    GXTexCoord2f32(1.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(p1[0], p1[1], 0.0f);
    GXColor4u8(r1, g1, b1, a1);
    GXTexCoord2f32(1.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
}

// Clamps `value` to [0.0f, lbl_804D7AB0] and starts a line block with
// the GX line width scaled from it. Unlike Draw3DBeginLine it does not touch
// the Gfx vertex/material state.
void Draw3DBeginLineSimple(f32 value, int mode)
{
    f32 v;

    v = value < 0.0f ? 0.0f : (value > lbl_804D7AB0 ? lbl_804D7AB0 : value);

    GXEnableTexOffsets(GX_TEXCOORD0, 0, 1);
    GXSetLineWidth((u8)(int)(lbl_804D7AB4 * v), GX_TO_ONE);

    switch (fn_80010448(mode, 1)) {
    case 2:
        fn_80010330(mode, 1);
        break;
    case 0:
    case 8:
        fn_80010448(-3, 1);
        break;
    }
}

// Ends the current draw batch (retail keeps this an empty hook).
void DrawFlush()
{
}

// Pushes one GX vertex with z fixed at 0, a flat colour and a texture
// coordinate.
void DrawVertex(u8 r, u8 g, u8 b, u8 a, f32 x, f32 y, f32 s, f32 t)
{
    GXPosition3f32(x, y, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(s, t);
    GXTexCoord2f32(0.0f, 0.0f);
}

// Starts a textured quad draw block (name is a descriptive guess).
void DrawSetTexture(int mode)
{
    switch (fn_80010448(mode, 1)) {
    case 2:
        fn_80010330(mode, 1);
        break;
    case 0:
    case 8:
        fn_80010448(-3, 1);
        break;
    }

    GXBegin(GX_QUADS, GX_VTXFMT2, 4);
}

// Starts a textured quad draw block and emits a quad whose texture coordinates
// are given explicitly (name is a descriptive guess).
void DrawTexQuadUV(int mode, u8 r, u8 g, u8 b, u8 a,
                   f32 x0, f32 y0, f32 x1, f32 y1,
                   f32 s0, f32 t0, f32 s1, f32 t1)
{
    switch (fn_80010448(mode, 1)) {
    case 2:
        fn_80010330(mode, 1);
        break;
    case 0:
    case 8:
        fn_80010448(-3, 1);
        break;
    }

    GXBegin(GX_QUADS, GX_VTXFMT2, 4);
    GXPosition3f32(x0, y1, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(s0, t0);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(x0, y0, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(s0, t1);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(x1, y0, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(s1, t1);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(x1, y1, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(s1, t0);
    GXTexCoord2f32(0.0f, 0.0f);
}

// Draws a textured, screen-space quad with the texture coordinates running
// across its full extent.
void DrawTexQuad(int mode, f32 x0, f32 y0, f32 x1, f32 y1,
                 u8 r, u8 g, u8 b, u8 a)
{
    switch (fn_80010448(mode, 1)) {
    case 2:
        fn_80010330(mode, 1);
        break;
    case 0:
    case 8:
        fn_80010448(-3, 1);
        break;
    }

    GXBegin(GX_QUADS, GX_VTXFMT2, 4);
    GXPosition3f32(x0, y1, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(x0, y0, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(0.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(x1, y0, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(1.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(x1, y1, 0.0f);
    GXColor4u8(r, g, b, a);
    GXTexCoord2f32(1.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
}

// Draws a flat white textured quad over the given screen rectangle (name is a
// descriptive guess).
void DrawWhiteTexQuad(int mode, f32 x0, f32 y0, f32 x1, f32 y1)
{
    switch (fn_80010448(mode, 1)) {
    case 2:
        fn_80010330(mode, 1);
        break;
    case 0:
    case 8:
        fn_80010448(-3, 1);
        break;
    }

    GXBegin(GX_QUADS, GX_VTXFMT2, 4);
    GXPosition3f32(x0, y1, 0.0f);
    GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
    GXTexCoord2f32(0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(x0, y0, 0.0f);
    GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
    GXTexCoord2f32(0.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(x1, y0, 0.0f);
    GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
    GXTexCoord2f32(1.0f, 1.0f);
    GXTexCoord2f32(0.0f, 0.0f);

    GXPosition3f32(x1, y1, 0.0f);
    GXColor4u8(0xFF, 0xFF, 0xFF, 0xFF);
    GXTexCoord2f32(1.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
}

extern "C" void fn_80008CA4(void);

// Ends a 2D draw block (identical body to Draw3DEnd).
void Draw2DEnd(void)
{
    fn_8000FF7C();
    fn_801D89A4(lbl_804D4D20);
    fn_800097F0();
}

// Begins a 2D draw block: sets up the GX state used by the screen-space quad
// and line helpers above.
void Draw2DBegin(void)
{
    fn_80008CA4();
    GfxSetVtxDesc(2);
    GfxSetMaterial(0);
    GfxSetCullMode(0);
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(1);
    GXSetAlphaCompare(GX_ALWAYS, 0, 0, GX_ALWAYS, 0);
    lbl_804D4D20 = fn_801D89A4(0);
    GXSetZMode(0, GX_ALWAYS, 0);
}

// Appends one vertex to the ring buffer, advancing and wrapping the write
// index, and growing the drawn count up to the buffer size.
void TrailBufferPush(TrailBuffer* p, f32* v)
{
    f32* dst;

    p->end = p->end + 1;
    if (p->end >= p->max) {
        p->end = 0;
    }

    dst = &p->verts[p->end * 3];
    dst[0] = v[0];
    dst[1] = v[1];
    dst[2] = v[2];

    if (p->count < p->max) {
        p->count = p->count + 1;
    }
}

// Initialises a ring buffer with `max` vertices and a per-vertex colour ramp
// running from the base colour to the end colour.
void TrailBufferInit(TrailBuffer* p, f32* verts, s32 max,
                     s32 rEnd, s32 gEnd, s32 bEnd, s32 aEnd,
                     u8 rBase, u8 gBase, u8 bBase, u8 aBase)
{
    f32 scale = 1.0f / (f32)max;
    s32 end = max - 1;
    s32 count = 0;

    p->verts = verts;
    p->count = count;
    p->end = end;
    p->max = max;
    p->base[0] = rBase;
    p->base[1] = gBase;
    p->base[2] = bBase;
    p->base[3] = aBase;
    p->dr = scale * (f32)((rEnd & 0xFF) - rBase);
    p->dg = scale * (f32)((gEnd & 0xFF) - gBase);
    p->db = scale * (f32)((bEnd & 0xFF) - bBase);
    p->da = scale * (f32)((aEnd & 0xFF) - aBase);
}

extern "C" f32 lbl_804D7AB8;
extern "C" f32 fn_800A1418(f32 angle);
extern "C" f32 fn_800A1394(f32 angle);

// Builds the 24-entry table of direction vectors used by DrawTrailStrip.
void DrawTrailBuildDirections(void)
{
    f32 step = lbl_804D7AB8;
    f32 angle;
    int i;

    for (i = 0; i < 24; i++) {
        angle = (f32)i * step;

        lbl_8046D314[i * 3 + 0] = fn_800A1418(angle);
        lbl_8046D314[i * 3 + 1] = 0.0f;
        lbl_8046D314[i * 3 + 2] = fn_800A1394(angle);
    }
}

// Current viewport dimensions, used to derive the screen-space scales.
extern "C" s32 lbl_804D2568;
extern "C" s32 lbl_804D256C;

// Recomputes the screen-space x/y scales from the current viewport size.
void DrawUpdateScreenScale(void)
{
    lbl_804D4D08 = 1.0f / (f32)lbl_804D2568;
    lbl_804D4D18 = (f32)lbl_804D2568;
    lbl_804D4D1C = (f32)lbl_804D256C;
    lbl_804D4D0C = 1.0f / (f32)lbl_804D256C;
}

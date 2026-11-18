// u_8013A3D4.cpp: 2D/3D draw helpers for the GX FIFO (names are descriptive guesses).

#include <dolphin/gx/GXGeometry.h>
#include <dolphin/gx/GXTransform.h>
#include <dolphin/gx/GXVert.h>
#include "game/types.h"

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


// Projection, viewport and view matrix captured by Draw3DBegin.
static f32 sProjection[7];
static f32 sViewport[6];
static f32 sViewMtx[3][4];

// Unit directions around the Y axis, 15 degrees apart (DrawTrailBuildDirections).
static f32 sDirections[24][3];

// Texture coordinates for even and odd strip vertices.
static f32 sTrailTexCoords[2][2] = {{1.0f, 1.0f}, {1.0f, 0.0f}};

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
    f32 rot[4];
    f32 p1[3];
    f32 p0[3];
    f32 radius;
    Vec3 v0, v1, v2, v3;
    bool onScreen;
    int visible;

    GXProject(pos[0], pos[1], pos[2], sViewMtx, sProjection, sViewport, &p0[0], &p0[1], &p0[2]);
    p0[0] -= sViewport[0];
    p0[1] -= sViewport[1];

    p1[0] = sViewMtx[0][0] * width + pos[0];
    p1[1] = sViewMtx[0][1] * width + pos[1];
    p1[2] = sViewMtx[0][2] * width + pos[2];
    GXProject(p1[0], p1[1], p1[2], sViewMtx, sProjection, sViewport, &p1[0], &p1[1], &p1[2]);
    p1[0] -= sViewport[0];
    p1[1] -= sViewport[1];

    radius = PSVECDistance(p0, p1);

    onScreen = (p0[0] + radius > lbl_804D4D10 || p0[0] - radius < lbl_804D4D18) &&
               (p0[1] + radius > lbl_804D4D14 || p0[1] - radius < lbl_804D4D1C);
    if (onScreen) {
        rot[0] = 135.0f + angle;
        rot[1] = 225.0f + angle;
        fn_800A0FC4(rot, rot);
        v0.x = radius * rot[2] + p0[0];
        v0.y = radius * rot[0] + p0[1];
        v1.x = radius * rot[3] + p0[0];
        v1.y = radius * rot[1] + p0[1];
        v0.z = p0[2];
        v1.z = p0[2];

        rot[0] = 315.0f + angle;
        rot[1] = 45.0f + angle;
        fn_800A0FC4(rot, rot);
        v2.x = radius * rot[2] + p0[0];
        v2.y = radius * rot[0] + p0[1];
        v3.x = radius * rot[3] + p0[0];
        v3.y = radius * rot[1] + p0[1];
        v2.z = p0[2];
        v3.z = p0[2];
        visible = 1;
    } else {
        visible = 0;
    }

    if (visible) {
        v0.x *= lbl_804D4D08;
        v0.y *= lbl_804D4D0C;
        v1.x *= lbl_804D4D08;
        v1.y *= lbl_804D4D0C;
        v2.x *= lbl_804D4D08;
        v2.y *= lbl_804D4D0C;
        v3.x *= lbl_804D4D08;
        v3.y *= lbl_804D4D0C;
        GXBegin(GX_QUADS, GX_VTXFMT2, 4);
        GXPosition3f32(v0.x, v0.y, v0.z);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(0.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(v1.x, v1.y, v1.z);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(0.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(v2.x, v2.y, v2.z);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(1.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(v3.x, v3.y, v3.z);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(1.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
    }
}

// Same as DrawWorldQuad, with the texture coordinates rotated one vertex.
void DrawWorldQuadFlipped(f32 width, f32 angle, f32* pos, u8 r, u8 g, u8 b, u8 a)
{
    f32 rot[4];
    f32 p1[3];
    f32 p0[3];
    f32 radius;
    Vec3 v0, v1, v2, v3;
    bool onScreen;
    int visible;

    GXProject(pos[0], pos[1], pos[2], sViewMtx, sProjection, sViewport, &p0[0], &p0[1], &p0[2]);
    p0[0] -= sViewport[0];
    p0[1] -= sViewport[1];

    p1[0] = sViewMtx[0][0] * width + pos[0];
    p1[1] = sViewMtx[0][1] * width + pos[1];
    p1[2] = sViewMtx[0][2] * width + pos[2];
    GXProject(p1[0], p1[1], p1[2], sViewMtx, sProjection, sViewport, &p1[0], &p1[1], &p1[2]);
    p1[0] -= sViewport[0];
    p1[1] -= sViewport[1];

    radius = PSVECDistance(p0, p1);

    onScreen = (p0[0] + radius > lbl_804D4D10 || p0[0] - radius < lbl_804D4D18) &&
               (p0[1] + radius > lbl_804D4D14 || p0[1] - radius < lbl_804D4D1C);
    if (onScreen) {
        rot[0] = 135.0f + angle;
        rot[1] = 225.0f + angle;
        fn_800A0FC4(rot, rot);
        v0.x = radius * rot[2] + p0[0];
        v0.y = radius * rot[0] + p0[1];
        v1.x = radius * rot[3] + p0[0];
        v1.y = radius * rot[1] + p0[1];
        v0.z = p0[2];
        v1.z = p0[2];

        rot[0] = 315.0f + angle;
        rot[1] = 45.0f + angle;
        fn_800A0FC4(rot, rot);
        v2.x = radius * rot[2] + p0[0];
        v2.y = radius * rot[0] + p0[1];
        v3.x = radius * rot[3] + p0[0];
        v3.y = radius * rot[1] + p0[1];
        v2.z = p0[2];
        v3.z = p0[2];
        visible = 1;
    } else {
        visible = 0;
    }

    if (visible) {
        v0.x *= lbl_804D4D08;
        v0.y *= lbl_804D4D0C;
        v1.x *= lbl_804D4D08;
        v1.y *= lbl_804D4D0C;
        v2.x *= lbl_804D4D08;
        v2.y *= lbl_804D4D0C;
        v3.x *= lbl_804D4D08;
        v3.y *= lbl_804D4D0C;
        GXBegin(GX_QUADS, GX_VTXFMT2, 4);
        GXPosition3f32(v0.x, v0.y, v0.z);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(0.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(v1.x, v1.y, v1.z);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(0.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(v2.x, v2.y, v2.z);
        GXColor4u8(r, g, b, a);
        GXTexCoord2f32(1.0f, 1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(v3.x, v3.y, v3.z);
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

    if (setup != 0) {
        fn_8004320C(setup, buf);
        fn_80009324(buf, 0);
    } else {
        fn_800092CC();
    }

    GXGetProjectionv(sProjection);
    GXGetViewportv(sViewport);
    fn_80008C78(sViewMtx);

    lbl_804D4D08 = 1.0f / sViewport[2];
    lbl_804D4D0C = 1.0f / sViewport[3];
    lbl_804D4D10 = sViewport[0];
    lbl_804D4D14 = sViewport[1];
    lbl_804D2E7C = sViewport[2] / (f32)lbl_804D2568;
    lbl_804D2E80 = sViewport[3] / (f32)lbl_804D2568;
    lbl_804D4D18 = sViewport[0] + sViewport[2];
    lbl_804D4D1C = sViewport[1] + sViewport[3];

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

void DrawTrailStrip(int start, u8 r0, u8 g0, u8 b0, u8 a0,
                    u8 r1, u8 g1, u8 b1, u8 a1,
                    f32 height, f32 density, f32 x0, f32 x1, f32 jitter)
{
    int n = (int)(24.0f * density * height);
    if (n > 1) {
        f32 dr, dg, db, da;
        f32 dy, dx;
        int idx;
        int i;

        dy = height / (f32)n;
        dx = (x1 - x0) / (f32)n;
        idx = start % 24;
        dr = (f32)(r0 - r1) / (255.0f * (f32)n);
        dg = (f32)(g0 - g1) / (255.0f * (f32)n);
        db = (f32)(b0 - b1) / (255.0f * (f32)n);
        da = (f32)(a0 - a1) / (255.0f * (f32)n);

        GXBegin(GX_LINESTRIP, GX_VTXFMT2, n);
        for (i = 0; i < n; i++) {
            f32 w = x0 + (f32)i * dx + jitter * ((f32)rand() / 32767.0f);

            GXPosition3f32(w * sDirections[idx][0], (f32)i * dy, w * sDirections[idx][2]);
            GXColor4u8((u8)(255.0f * (dr * (f32)i) + r0),
                       (u8)(255.0f * (dg * (f32)i) + g0),
                       (u8)(255.0f * (db * (f32)i) + b0),
                       (u8)(255.0f * (da * (f32)i) + a0));
            GXTexCoord2f32(sTrailTexCoords[i & 1][0], sTrailTexCoords[i & 1][1]);
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
    int count = p->count;

    if (count <= 1)
        return;

    cr = lbl_804D2E78 * ((f32)p->base[0] * (f32)r);
    cg = lbl_804D2E78 * ((f32)p->base[1] * (f32)g);
    cb = lbl_804D2E78 * ((f32)p->base[2] * (f32)b);
    ca = lbl_804D2E78 * ((f32)p->base[3] * (f32)a);
    sr = lbl_804D2E78 * (p->dr * (f32)r);
    sg = lbl_804D2E78 * (p->dg * (f32)g);
    sb = lbl_804D2E78 * (p->db * (f32)b);
    sa = lbl_804D2E78 * (p->da * (f32)a);

    GXBegin(GX_LINESTRIP, GX_VTXFMT2, count);

    i = 0;
    int start = (p->end - count) + 1;
    if (start < 0)
        start += p->max;
    j = start;

    while (j < p->max && i < count) {
        GXPosition3f32(p->verts[j * 3 + 0], p->verts[j * 3 + 1], p->verts[j * 3 + 2]);
        GXColor4u8((u8)cr, (u8)cg, (u8)cb, (u8)ca);
        GXTexCoord2f32(sTrailTexCoords[j & 1][0], sTrailTexCoords[j & 1][1]);
        GXTexCoord2f32(0.0f, 0.0f);
        cr += sr;
        cg += sg;
        cb += sb;
        ca += sa;
        i++;
        j++;
    }

    j = 0;
    while (j <= p->end && i < count) {
        GXPosition3f32(p->verts[j * 3 + 0], p->verts[j * 3 + 1], p->verts[j * 3 + 2]);
        GXColor4u8((u8)cr, (u8)cg, (u8)cb, (u8)ca);
        GXTexCoord2f32(sTrailTexCoords[j & 1][0], sTrailTexCoords[j & 1][1]);
        GXTexCoord2f32(0.0f, 0.0f);
        cr += sr;
        cg += sg;
        cb += sb;
        ca += sa;
        i++;
        j++;
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


// Begins a 3D draw block like Draw3DBegin, with the line width taken from
// `value` clamped to [0.0f, 42.5f].
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

    v = value < 0.0f ? 0.0f : (value > 42.5f ? 42.5f : value);

    GXEnableTexOffsets(GX_TEXCOORD0, 1, 0);
    GXSetLineWidth((u8)(int)(6.0f * v), GX_TO_ONE);
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
// clamped to [0.0f, 42.5f].
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

    v = value < 0.0f ? 0.0f : (value > 42.5f ? 42.5f : value);

    GXEnableTexOffsets(GX_TEXCOORD0, 0, 1);
    GXSetPointSize((u8)(int)(6.0f * v), GX_TO_ONE);
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
    int count = strip->count;
    int i;
    int n;

    if (count > 1) {
        cr = strip->color[0] * (f32)r * lbl_804D2E78;
        cg = strip->color[1] * (f32)g * lbl_804D2E78;
        cb = strip->color[2] * (f32)b * lbl_804D2E78;
        ca = strip->color[3] * (f32)a * lbl_804D2E78;
        sr = strip->dr * (f32)r * lbl_804D2E78;
        sg = strip->dg * (f32)g * lbl_804D2E78;
        sb = strip->db * (f32)b * lbl_804D2E78;
        sa = strip->da * (f32)a * lbl_804D2E78;

        GXBegin(GX_LINESTRIP, GX_VTXFMT2, (u16)count);

        n = 0;
        int start = (strip->head - count) + 1;
        if (start < 0)
            start += strip->capacity;
        i = start;
        while (i < strip->capacity && n < count) {
            GXPosition3f32(strip->verts[i].x, strip->verts[i].y, 0.0f);
            GXColor4u8((u8)cr, (u8)cg, (u8)cb, (u8)ca);
            GXTexCoord2f32(sTrailTexCoords[i & 1][0], sTrailTexCoords[i & 1][1]);
            GXTexCoord2f32(0.0f, 0.0f);
            cr += sr;
            cg += sg;
            cb += sb;
            ca += sa;
            n++;
            i++;
        }

        i = 0;
        while (i <= strip->head && n < count) {
            GXPosition3f32(strip->verts[i].x, strip->verts[i].y, 0.0f);
            GXColor4u8((u8)cr, (u8)cg, (u8)cb, (u8)ca);
            GXTexCoord2f32(sTrailTexCoords[i & 1][0], sTrailTexCoords[i & 1][1]);
            GXTexCoord2f32(0.0f, 0.0f);
            cr += sr;
            cg += sg;
            cb += sb;
            ca += sa;
            n++;
            i++;
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

// Clamps `value` to [0.0f, 42.5f] and starts a line block with
// the GX line width scaled from it. Unlike Draw3DBeginLine it does not touch
// the Gfx vertex/material state.
void Draw3DBeginLineSimple(f32 value, int mode)
{
    f32 v;

    v = value < 0.0f ? 0.0f : (value > 42.5f ? 42.5f : value);

    GXEnableTexOffsets(GX_TEXCOORD0, 0, 1);
    GXSetLineWidth((u8)(int)(6.0f * v), GX_TO_ONE);

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
void TrailBufferInit(TrailBuffer* p, f32* verts, u32 max,
                     u32 rEnd, u32 gEnd, u32 bEnd, u32 aEnd,
                     u8 rBase, u8 gBase, u8 bBase, u8 aBase)
{
    int dr = (rEnd & 0xFF) - rBase;
    int dg = (gEnd & 0xFF) - gBase;
    int db = (bEnd & 0xFF) - bBase;
    int da = (aEnd & 0xFF) - aBase;
    p->verts = verts;
    p->count = 0;
    p->end = max - 1;
    p->max = max;
    p->base[0] = rBase;
    p->base[1] = gBase;
    p->base[2] = bBase;
    p->base[3] = aBase;
    f32 scale = 1.0f / (f32)(int)max;
    p->dr = scale * (f32)dr;
    p->dg = scale * (f32)dg;
    p->db = scale * (f32)db;
    p->da = scale * (f32)da;
}

extern "C" f32 fn_800A1418(f32 angle);
extern "C" f32 fn_800A1394(f32 angle);

// Builds the 24-entry table of direction vectors used by DrawTrailStrip.
void DrawTrailBuildDirections(void)
{
    f32 step = 15.0f;
    f32 angle;
    int i;

    for (i = 0; i < 24; i++) {
        angle = (f32)i * step;

        sDirections[i][0] = fn_800A1418(angle);
        sDirections[i][1] = 0.0f;
        sDirections[i][2] = fn_800A1394(angle);
    }
}

// Current viewport dimensions, used to derive the screen-space scales.
extern "C" s32 lbl_804D2568;
extern "C" s32 lbl_804D256C;

// Recomputes the screen-space x/y scales from the current viewport size.
void DrawUpdateScreenScale(void)
{
    lbl_804D4D08 = 1.0f / (f32)lbl_804D2568;
    lbl_804D4D0C = 1.0f / (f32)lbl_804D256C;
    lbl_804D4D18 = (f32)lbl_804D2568;
    lbl_804D4D1C = (f32)lbl_804D256C;
}

// u_80210D80.cpp: particle stepping and draw parameters and the white-out effect (0x80210D80-0x802126C8).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/actor.h"

extern "C" void Draw3DBegin(int texture, void* arg);
extern "C" void DrawWorldQuadFlipped(float alpha, float depth, float* pos,
                                     unsigned char r, unsigned char g,
                                     unsigned char b, unsigned char a);
extern "C" void Draw3DEnd();
extern "C" void Draw3DBeginLine(int texture, void* arg, float width);
extern "C" void DrawLine(float* from, float* to, unsigned char c0, unsigned char c1,
                         unsigned char c2, unsigned char c3, unsigned char c4,
                         unsigned char c5, unsigned char c6, unsigned char c7);
extern "C" void DrawDisableTexOffsets();

extern "C" double fmod(double x, double y);

// Draws a run of alpha-blended, colour-ramped quads (particle billboards).
// Kept under its placeholder name: other units call it as fn_80210D80.
extern "C" void fn_80210D80(float* pts, int stride, int count, int texture,
                            void* arg, float* alphaRamp, float* colorRamp)
{
    if (pts == 0) {
        return;
    }
    Draw3DBegin(texture, arg);
    float zero = 0.0f;
    float* p = pts;
    for (int i = 0; i < count; i++) {
        float y = p[1];
        if (zero < y) {
            float x = p[0];
            if (zero < x) {
                float c0;
                float c1;
                float c2;
                float c3;
                if (x < colorRamp[0]) {
                    float t = 1.0f;
                    if (colorRamp[0] > 1.1920929e-07f) {
                        t = x / colorRamp[0];
                    }
                    c0 = t * (colorRamp[6] - colorRamp[2]) + colorRamp[2];
                    c1 = t * (colorRamp[7] - colorRamp[3]) + colorRamp[3];
                    c2 = t * (colorRamp[8] - colorRamp[4]) + colorRamp[4];
                    c3 = t * (colorRamp[9] - colorRamp[5]) + colorRamp[5];
                } else if (y < colorRamp[1]) {
                    float t = 1.0f;
                    if (colorRamp[1] > 1.1920929e-07f) {
                        t = y / colorRamp[1];
                    }
                    c0 = t * (colorRamp[6] - colorRamp[10]) + colorRamp[10];
                    c1 = t * (colorRamp[7] - colorRamp[11]) + colorRamp[11];
                    c2 = t * (colorRamp[8] - colorRamp[12]) + colorRamp[12];
                    c3 = t * (colorRamp[9] - colorRamp[13]) + colorRamp[13];
                } else {
                    c0 = colorRamp[6];
                    c1 = colorRamp[7];
                    c2 = colorRamp[8];
                    c3 = colorRamp[9];
                }
                float alpha;
                if (x < alphaRamp[0]) {
                    float t = 1.0f;
                    if (alphaRamp[0] > 1.1920929e-07f) {
                        t = x / alphaRamp[0];
                    }
                    alpha = t * (alphaRamp[3] - alphaRamp[2]) + alphaRamp[2];
                } else if (y < alphaRamp[1]) {
                    float t = 1.0f;
                    if (alphaRamp[1] > 1.1920929e-07f) {
                        t = y / alphaRamp[1];
                    }
                    alpha = t * (alphaRamp[3] - alphaRamp[4]) + alphaRamp[4];
                } else {
                    alpha = alphaRamp[3];
                }
                float s = 255.0f;
                DrawWorldQuadFlipped(alpha, p[11], p + 2,
                                     (unsigned char)(int)(s * c0),
                                     (unsigned char)(int)(s * c1),
                                     (unsigned char)(int)(s * c2),
                                     (unsigned char)(int)(s * c3));
            }
        }
        p = (float*)((int)p + stride);
    }
    Draw3DEnd();
}

// Draws a run of colour-ramped textured lines, one line per particle record.
// Kept under its placeholder name: other units call it as fn_80210FC8.
extern "C" void fn_80210FC8(float* pts, int stride, int count, int texture,
                            void* arg, float* rampA, float* rampB, float width,
                            float num, float den)
{
    if (pts == 0) {
        return;
    }
    Draw3DBeginLine(texture, arg, width);
    float zero = 0.0f;
    float scale = -num / den;
    float* p = pts;
    for (int i = 0; i < count; i++) {
        float y = p[1];
        if (zero < y) {
            float x = p[0];
            if (zero < x) {
                float a0;
                float a1;
                float a2;
                float a3;
                if (x < rampA[0]) {
                    float t = 1.0f;
                    if (rampA[0] > 1.1920929e-07f) {
                        t = x / rampA[0];
                    }
                    a0 = t * (rampA[6] - rampA[2]) + rampA[2];
                    a1 = t * (rampA[7] - rampA[3]) + rampA[3];
                    a2 = t * (rampA[8] - rampA[4]) + rampA[4];
                    a3 = t * (rampA[9] - rampA[5]) + rampA[5];
                } else if (y < rampA[1]) {
                    float t = 1.0f;
                    if (rampA[1] > 1.1920929e-07f) {
                        t = y / rampA[1];
                    }
                    a0 = t * (rampA[6] - rampA[10]) + rampA[10];
                    a1 = t * (rampA[7] - rampA[11]) + rampA[11];
                    a2 = t * (rampA[8] - rampA[12]) + rampA[12];
                    a3 = t * (rampA[9] - rampA[13]) + rampA[13];
                } else {
                    a0 = rampA[6];
                    a1 = rampA[7];
                    a2 = rampA[8];
                    a3 = rampA[9];
                }
                float b0;
                float b1;
                float b2;
                float b3;
                if (x < rampB[0]) {
                    float t = 1.0f;
                    if (rampB[0] > 1.1920929e-07f) {
                        t = x / rampB[0];
                    }
                    b0 = t * (rampB[6] - rampB[2]) + rampB[2];
                    b1 = t * (rampB[7] - rampB[3]) + rampB[3];
                    b2 = t * (rampB[8] - rampB[4]) + rampB[4];
                    b3 = t * (rampB[9] - rampB[5]) + rampB[5];
                } else if (y < rampB[1]) {
                    float t = 1.0f;
                    if (rampB[1] > 1.1920929e-07f) {
                        t = y / rampB[1];
                    }
                    b0 = t * (rampB[6] - rampB[10]) + rampB[10];
                    b1 = t * (rampB[7] - rampB[11]) + rampB[11];
                    b2 = t * (rampB[8] - rampB[12]) + rampB[12];
                    b3 = t * (rampB[9] - rampB[13]) + rampB[13];
                } else {
                    b0 = rampB[6];
                    b1 = rampB[7];
                    b2 = rampB[8];
                    b3 = rampB[9];
                }
                float end[3];
                end[0] = p[5] * scale;
                end[1] = p[6] * scale;
                end[2] = p[7] * scale;
                float s = 255.0f;
                DrawLine(p + 2, end,
                         (unsigned char)(int)(s * a0),
                         (unsigned char)(int)(s * a1),
                         (unsigned char)(int)(s * a2),
                         (unsigned char)(int)(s * a3),
                         (unsigned char)(int)(s * b0),
                         (unsigned char)(int)(s * b1),
                         (unsigned char)(int)(s * b2),
                         (unsigned char)(int)(s * b3));
            }
        }
        p = (float*)((int)p + stride);
    }
    DrawDisableTexOffsets();
}

// Draw3DBeginPoint's third argument (the point size) arrives in f1 and is
// passed straight through to it below.
void Draw3DBeginPoint(int texture, void* arg, float size);
void DrawPoint(float* pos, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a);
void DrawDisablePointOffsets(void);

// Draws every on-screen particle record as a screen-space point.
void DrawParticlePoints(float* pts, int stride, int count, int texture, void* arg,
                        unsigned char r, unsigned char g, unsigned char b,
                        unsigned char a, float size)
{
    if (pts == 0) {
        return;
    }
    Draw3DBeginPoint(texture, arg, size);
    float zero = 0.0f;
    float* p = pts;
    for (int i = 0; i < count; i++) {
        float y = p[1];
        if (y > zero) {
            float x = p[0];
            if (x > zero) {
                DrawPoint(p + 2, r, g, b, a);
            }
        }
        p = (float*)((int)p + stride);
    }
    DrawDisablePointOffsets();
}

// Integrates a scaled velocity vector into the motion fields of every
// on-screen particle record.
// fn_802113F0
extern "C" void fn_802113F0(float* pts, int stride, int count, float delta,
                              float* vec)
{
    if (pts == 0) {
        return;
    }
    if (vec == 0) {
        return;
    }
    float vx = vec[0] * delta;
    float vy = vec[1] * delta;
    float vz = vec[2] * delta;
    float zero = 0.0f;
    for (int i = 0; i < count; i++) {
        if (pts[1] > zero && pts[0] > zero) {
            pts[5] += vx;
            pts[6] += vy;
            pts[7] += vz;
        }
        pts = (float*)((int)pts + stride);
    }
}

// Advances each particle in the array by `dt`: integrates the position from the
// velocity, clamps the step to the remaining life, and wraps the spin angle.
// Returns the number of live particles advanced.
// fn_80211470
extern "C" int fn_80211470(float dt, float* particles, int stride, int count) {
    int live = 0;
    if (particles) {
        float zero = 0.0f;
        for (int i = 0; i < count; i++) {
            if (particles[1] > zero) {
                particles[0] = particles[0] + dt;
                float x = particles[0];
                if (x > zero) {
                    float step = dt;
                    if (x < dt)
                        step = x;
                    particles[1] = particles[1] - step;
                    particles[2] = particles[2] + particles[5] * step;
                    particles[3] = particles[3] + particles[6] * step;
                    particles[4] = particles[4] + particles[7] * step;
                    particles[11] = (float)fmod((double)(particles[12] * step + particles[11]),
                                                360.0);
                }
                live++;
            }
            particles = (float*)((char*)particles + stride);
        }
    }
    return live;
}

int ParticleStepPlain(float dt, float* particles, int stride, int count) {
    int live = 0;
    if (particles != 0) {
        float zero = 0.0f;
        for (int i = 0; i < count; i++) {
            if (particles[1] > zero) {
                particles[0] = particles[0] + dt;
                if (particles[0] > zero) {
                    float step = particles[0] < dt ? particles[0] : dt;
                    particles[1] = particles[1] - step;
                    float vx = particles[5] * step;
                    float vy = particles[6] * step;
                    float vz = particles[7] * step;
                    particles[2] += vx;
                    particles[3] += vy;
                    particles[4] += vz;
                }
                live++;
            }
            particles = (float*)((char*)particles + stride);
        }
    }
    return live;
}

// Fills in a particle draw-parameter block: screen position, rotation, scale,
// tint (default opaque white) and the optional texture coordinates (default
// 0,0,0,1). Every optional pointer falls back to its identity value.
// fn_80211638
extern "C" void fn_80211638(float x, float y, float alpha, float depth, float* out,
                           float* rot, float* scale, float* tint, float* uv)
{
    if (out == 0) {
        return;
    }
    out[0] = x;
    out[1] = y;
    if (rot != 0) {
        out[2] = rot[0];
        out[3] = rot[1];
        out[4] = rot[2];
    } else {
        out[2] = 0.0f;
        out[3] = 0.0f;
        out[4] = 0.0f;
    }
    if (scale != 0) {
        out[5] = scale[0];
        out[6] = scale[1];
        out[7] = scale[2];
    } else {
        out[5] = 0.0f;
        out[6] = 0.0f;
        out[7] = 0.0f;
    }
    if (tint != 0) {
        out[8] = tint[0];
        out[9] = tint[1];
        out[10] = tint[2];
    } else {
        out[8] = 0.0f;
        out[9] = 0.0f;
        out[10] = 1.0f;
    }
    out[11] = alpha;
    out[12] = depth;
    if (uv != 0) {
        out[13] = uv[0];
        out[14] = uv[1];
        out[15] = uv[2];
        out[16] = uv[3];
    } else {
        out[13] = 0.0f;
        out[14] = 0.0f;
        out[15] = 0.0f;
        out[16] = 1.0f;
    }
}

// Sets only the position/rotation/scale half of a particle draw-parameter
// block; rotation and scale default to zero when their pointer is null.
void SetParticleDrawTransform(float x, float y, float* out, float* rot, float* scale)
{
    if (out == 0) {
        return;
    }
    out[0] = x;
    out[1] = y;
    if (rot != 0) {
        out[2] = rot[0];
        out[3] = rot[1];
        out[4] = rot[2];
    } else {
        out[2] = 0.0f;
        out[3] = 0.0f;
        out[4] = 0.0f;
    }
    if (scale != 0) {
        out[5] = scale[0];
        out[6] = scale[1];
        out[7] = scale[2];
    } else {
        out[5] = 0.0f;
        out[6] = 0.0f;
        out[7] = 0.0f;
    }
}

// ---------------------------------------------------------------------------
// Particle direction scatter.

extern "C" int rand(void);
extern "C" float sqrtf(float x);
extern "C" double fabs(double x);

// Uniform random component built from rand(); the shared constants scale and
// centre it.
static inline float RandomComponent()
{
    return (float)rand() / 32767.0f - 0.5f;
}

// Scales a three-component vector to unit length.
static inline void NormalizeVector3(float* v)
{
    float inv = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = v[0] * inv;
    v[1] = v[1] * inv;
    v[2] = v[2] * inv;
}

// Scatters a unit vector `in` by a random tangential direction of strength
// `amount` and writes the renormalised result to `out`.
void PerturbDirectionRandom(float* out, float* in, float amount)
{
    float r[3];
    r[0] = RandomComponent();
    r[1] = RandomComponent();
    r[2] = RandomComponent();
    if ((fabs(r[0]) > 1.1920929e-07f) + (fabs(r[1]) > 1.1920929e-07f) +
            (fabs(r[2]) > 1.1920929e-07f) == 0) {
        r[0] = 0.0f;
        r[1] = 0.0f;
        r[2] = 0.0f;
    } else {
        NormalizeVector3(r);
    }
    float dot = r[0] * in[0] + r[1] * in[1] + r[2] * in[2];
    out[0] = amount * (r[0] - in[0] * dot) + in[0];
    out[1] = amount * (r[1] - in[1] * dot) + in[1];
    out[2] = amount * (r[2] - in[2] * dot) + in[2];
    if ((fabs(out[0]) > 1.1920929e-07f) + (fabs(out[1]) > 1.1920929e-07f) +
            (fabs(out[2]) > 1.1920929e-07f) == 0) {
        out[0] = 0.0f;
        out[1] = 0.0f;
        out[2] = 0.0f;
    } else {
        NormalizeVector3(out);
    }
}

struct Actor;

void PossessableDestroy(Actor* actor);

// Destroys the actor through the shared possessable-destroy path.
void Fn80211A84(Actor* actor)
{
    PossessableDestroy(actor);
}

// ---------------------------------------------------------------------------
// White-out ghost actor behaviour.

extern "C" float lbl_804D3410;

extern "C" float fabsf(float x);

// The 0xa4 behaviour block shared by the white-out ghost actors.
struct GhostState {
    char pad00[0x0C];
    unsigned int field0C;   // ghost id (high byte 2)
    float field10;          // draw alpha
    float field14;          // alpha clamp value
    unsigned int field18;   // behaviour flags
    char pad1C[0x30 - 0x1C];
    float field30;          // white-out timer
    float field34;
    float field38;
};

// The object at actor+0x84, describing the actor's current model/asset.
struct ActorModelInfo {
    char pad00[4];
    unsigned int field4;    // model asset id
    char pad08[0x18 - 0x8];
    int field18;            // model/animation hash
    char pad1C[0x24 - 0x1C];
    int field24;            // attachment node
    char pad28[0x2C - 0x28];
    void* field2C;          // model name hash / archive
    char pad30[0x80 - 0x30];
    int field80;
    int field84;
    int field88;
    int field8C;
    int field90;
    char pad94[0xA0 - 0x94];
    int fieldA0;            // overlay effect model id, -1 when absent
};

// A ghost actor owned by the game, as handed back by SendMsg.
struct WhiteOutGhost {
    char pad0000[0x248];
    int field248;
    char pad024C[0x14CC - 0x24C];
    int field14CC;          // font handle
    char pad14D0[0x264C - 0x14D0];
    float field264C;
    char pad2650[0x26E0 - 0x2650];
    char field26E0[0x10];
};

// Camera record returned by fn_800C5DD8.
struct CameraInfo {
    char pad00[0x24];
    float field24;
    float field28;
    float field2C;
};

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void GhostNotify(void* target, int a, int b);
extern "C" CameraInfo* fn_800C5DD8();
extern "C" void fn_800A025C(float* out, CameraInfo* cam, int index);
extern "C" void fn_800B4C50(float a, float b, float* pos, int c, int d, int e);
extern "C" int fn_802A1D04();
extern "C" void fn_800C7B78(Actor* actor);
void WhiteOutEffectStart(Actor* actor, unsigned int id, int flag);

void FontRelease(int font);
int fn_8007EF48(int a, int b, const char* text, int c, int d, int e, int f,
                int g, int h, int i);
void PossessableUpdate(Actor* actor, float dt);

// White-out ghost update: spawns the "White Out" overlay ghost, launches the
// white-out effect and fades the actor out while it is in front of the camera.
void Fn80211AA4(Actor* actor, float dt)
{
    GhostState* beh = ((GhostState*)actor->data);
    unsigned int flags = beh->field18;

    if ((flags & 0x80000000) == 0) {
        if (flags & 0x40000000) {
            unsigned int id = beh->field0C;
            if ((id >> 24) == 2) {
                int msg[8];
                SendMsg(id, 0x400, msg);
                WhiteOutGhost* ghost = (WhiteOutGhost*)msg[0];
                if (ghost->field14CC != -1) {
                    FontRelease(ghost->field14CC);
                    ghost->field14CC = -1;
                }
                ghost->field14CC = fn_8007EF48(ghost->field248, 1, "White Out", 0,
                                               0, 0x3c, 0x3c, 0x3c, 0, (int)ghost);
                ghost->field264C = 60.0f;
                GhostNotify(ghost->field26E0, 0, 1);
            }
            float pos[3];
            pos[0] = (float)actor->place.pos.x;
            pos[1] = (float)actor->place.pos.y;
            pos[2] = (float)actor->place.pos.z;
            fn_800B4C50(201.0f, 8000.0f, pos, actor->handle, 2, 0x12);
            beh->field10 = 0.0f;
            WhiteOutEffectStart(actor, id, 1);
        }
    } else if (beh->field30 == 0.0f) {
        CameraInfo* cam = fn_800C5DD8();
        float view[3];
        fn_800A025C(view, cam, 1);
        float dir[3];
        dir[0] = (float)actor->place.pos.x;
        dir[1] = (float)actor->place.pos.y;
        dir[2] = (float)actor->place.pos.z;
        dir[0] = dir[0] - cam->field24;
        dir[1] = dir[1] - cam->field28;
        dir[2] = dir[2] - cam->field2C;
        float len2 = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
        if ((fabsf(dir[0]) > 1.1920929e-07f) + (fabsf(dir[1]) > 1.1920929e-07f) +
                (fabsf(dir[2]) > 1.1920929e-07f) == 0) {
            dir[0] = 0.0f;
            dir[1] = 0.0f;
            dir[2] = 0.0f;
        } else {
            NormalizeVector3(dir);
        }
        float dot = dir[0] * view[0] + dir[1] * view[1] + dir[2] * view[2];
        if ((dot < 0.5f && len2 > lbl_804D3410) ||
                fn_802A1D04() != 0) {
            fn_800C7B78(actor);
        }
    } else if ((flags & 0x8000000) != 0) {
        beh->field30 = beh->field30 - dt;
        if (beh->field30 < 0.0f) {
            beh->field30 = 0.0f;
        }
    }
    PossessableUpdate(actor, dt);
}

extern "C" void* memset(void* dest, int value, unsigned long count);
void* VAlloc(int size);
int PossessableInit(Actor* actor);
int AssetFind(unsigned long name, unsigned long archive);
void ModelNodeSetupByName(int node, int a, int b, int c);
void ModelNodeSetup(int node, int asset, int a, int b);

// White-out ghost initialisation: allocates and clears the behaviour block,
// marks the actor possessable and sets up its overlay model node.
extern "C" int fn_80211E38(Actor* actor)
{
    int node;
    GhostState* beh = ((GhostState*)actor->data);
    if (beh == 0) {
        beh = (GhostState*)VAlloc(0x3c);
        actor->data = beh;
    }
    memset(beh, 0, 0x3c);

    int active;
    if (PossessableInit(actor) == 0) {
        active = 0;
        actor->flags |= 0x1000000;
    } else {
        actor->flags &= 0xdfffffff;
        if (((ActorModelInfo*)actor->info)->field4 != 0xa303c34 && ((ActorModelInfo*)actor->info)->field2C != 0) {
            node = AssetFind(0xd9044cbb,
                                 (unsigned long)((ActorModelInfo*)actor->info)->field2C);
            if (node != -1) {
                ModelNodeSetupByName(actor->model, 0, 0, 1);
                ModelNodeSetup(actor->model, node, 1, 1);
            }
        }
        active = 1;
        beh->field18 |= 0x8000000;
        beh->field18 &= 0xfffffff7;
        beh->field34 = 1800.0f;
        beh->field38 = 100000.0f;
    }

    if (active) {
        GhostState* g = ((GhostState*)actor->data);
        if (g != 0) {
            g->field18 |= 0x30000000;
        }
    }
    return active;
}

// White-out ghost init: clears the behaviour block, marks the actor
// possessable and sets up its overlay model node. Returns whether the actor
// became active.
int Fn80211F78(Actor* actor)
{
    GhostState* beh = ((GhostState*)actor->data);
    if (beh == 0) {
        beh = (GhostState*)VAlloc(0x3c);
        actor->data = beh;
    }
    memset(beh, 0, 0x3c);

    if (PossessableInit(actor) == 0) {
        actor->flags |= 0x1000000;
        return 0;
    }
    actor->flags &= 0xdfffffff;
    if (((ActorModelInfo*)actor->info)->field4 != 0xa303c34 && ((ActorModelInfo*)actor->info)->field2C != 0) {
        int node = AssetFind(0xd9044cbb, (unsigned long)((ActorModelInfo*)actor->info)->field2C);
        if (node != -1) {
            ModelNodeSetupByName(actor->model, 0, 0, 1);
            ModelNodeSetup(actor->model, node, 1, 1);
        }
    }
    beh->field18 |= 0x8000000;
    beh->field18 &= 0xfffffff7;
    beh->field34 = 1800.0f;
    beh->field38 = 100000.0f;
    return 1;
}

// Info record returned by OwnerGetInfo; only the type hash at 0x128 is used.
struct OwnerInfo {
    char pad00[0x128];
    int field128;           // owner type hash
};

OwnerInfo* OwnerGetInfo(unsigned long id);
extern "C" int fn_80049940(int typeHash);
int HandleToPlayer(int handle);
const char* StrFindKey(unsigned long key, unsigned long* outKey);
void HudMessageN(int handle, const char* text, unsigned long key, float a,
                 float b, float c, float d);
int PossessableMsg(Actor* actor, int msg, void* data, int arg);

// Message payload shared by the white-out ghost handlers: the target actor,
// the effect/state hash, the model id and the effect parameters.
struct GhostMsgData {
    Actor* actor;               // 0x00
    int hash;                   // 0x04
    unsigned int modelId;       // 0x08, high byte selects the owner type
    union {
        int flag;               // 0x0C, integer payload
        float amount;           // 0x0C, alpha decrement
    } value;
    int unk10;                  // 0x10
    int animHash;               // 0x14
};

// True when the owner behind `modelId` carries the white-out type hash, or the
// shared white-out check accepts it.
static inline bool IsWhiteOutOwner(unsigned int modelId)
{
    OwnerInfo* info = OwnerGetInfo(modelId);
    if (info == 0) {
        return false;
    }
    return info->field128 == 0x2c3fd46c || fn_80049940(info->field128) != 0;
}

// White-out ghost message handler: drives the fade/visibility state and the
// HUD white-out notification; anything it does not own goes to the shared
// possessable message handler.
int Fn802120A0(Actor* actor, int msg, void* data, int arg)
{
    GhostMsgData* msgData = (GhostMsgData*)data;
    int result = 0;

    switch (msg) {
    case 0x407: {
        Actor* target = msgData->actor;
        GhostState* beh = ((GhostState*)target->data);
        unsigned int type = msgData->modelId >> 24;

        if ((beh->field18 & 8) == 0 &&
                (type == 1 || type == 2 || type == 0x13) &&
                (type != 0x13 || IsWhiteOutOwner(msgData->modelId))) {
            switch (msgData->animHash) {
            case 0x1ba3ee4a:
            case -0x4603bd0b:
                beh->field10 = 0.0f;
                break;
            case 0x11a6b:
            case 0x403548d0:
                break;
            default:
                beh->field10 = beh->field10 - msgData->value.amount;
                break;
            }
            if (beh->field10 > 0.0f) {
                beh->field14 = 0.0f;
            }
            if ((beh->field18 & 0x80000000) == 0 &&
                    beh->field10 <= 0.0f) {
                WhiteOutEffectStart(target, msgData->modelId, 1);
            }
        }
        result = 1;
        break;
    }
    case 0xc14: {
        Actor* target = msgData->actor;
        GhostState* beh = ((GhostState*)target->data);

        switch (msgData->value.flag) {
        case 0x672f35c2:
            if (fn_802A1D04() != 0) {
                int player = HandleToPlayer(msgData->modelId);
                if (player >= 0) {
                    unsigned long key;
                    const char* text = StrFindKey(0x2df0c512, &key);
                    HudMessageN(player, text, key, 1.0f,
                                1.0f, 1.0f, 32.0f);
                }
            }
            break;
        case -0x30fe9b1e:
            beh->field18 |= 0x40000000;
            result = 1;
            break;
        case 0x6c345908:
            fn_800C7B78(target);
            result = 1;
            break;
        }
        break;
    }
    case 0x41d: {
        Actor* target = msgData->actor;
        GhostState* beh = ((GhostState*)target->data);

        switch (msgData->hash) {
        case -0x233c7b3f:
            WhiteOutEffectStart(target, target->handle, 1);
            result = 1;
            break;
        case -0x13ff57cf:
            WhiteOutEffectStart(target, target->handle, 0);
            result = 1;
            break;
        case -0x120729ad:
            beh->field18 |= 0x10000000;
            WhiteOutEffectStart(target, target->handle, 1);
            result = 1;
            break;
        case -0x203a3a46:
            if (msgData->value.flag != 0) {
                beh->field18 |= 0x8000000;
            } else {
                beh->field18 &= 0xf7ffffff;
            }
            break;
        }
        break;
    }
    }

    if (result == -999999) {
        result = 0;
    } else if (result == 0) {
        result = PossessableMsg(actor, msg, data, arg);
    }
    return result;
}

// ---------------------------------------------------------------------------
// White-out effect trigger.

extern "C" void fn_80158C3C(float* pos, int a, int b, int c, int d, int e,
                            int f);
extern "C" void fn_80122108(float a, float b, float c, float d, float e,
                            float f, float g, int h, int i, float* pos,
                            unsigned int hash, int j);
extern "C" void fn_801D55CC();
extern "C" void fn_800C5654(int handle, unsigned int hash, int id);
extern "C" void* memset(void* dest, int value, unsigned long count);

// Message block sent when the white-out overlay is spawned: the model id
// followed by six unused words and the actor position.
struct WhiteOutEffectMsg {
    int unk00;
    int model;              // 0x04
    int zero[6];            // 0x08
    float x;                // 0x20
    float y;                // 0x24
    float z;                // 0x28
    int unk2C;              // 0x2c
};

// Starts the white-out effect for a ghost that carries one: marks the
// behaviour as triggered, snaps its effect model to the actor position, spawns
// the overlay actor and notifies the effect system.
void WhiteOutEffectStart(Actor* actor, unsigned int id, int flag)
{
    GhostState* beh = ((GhostState*)actor->data);
    if ((beh->field18 & 0x80000000) != 0) {
        return;
    }
    beh->field18 |= 0x80000000;
    beh->field30 = 30000.0f;

    float pos[3];
    pos[0] = (float)actor->place.pos.x;
    pos[1] = (float)actor->place.pos.y;
    pos[2] = (float)actor->place.pos.z;

    if ((beh->field18 & 0x10000000) == 0) {
        ActorModelInfo* info = ((ActorModelInfo*)actor->info);
        fn_80158C3C(pos, info->field24, info->field80, info->field84,
                    info->field88, info->field8C, info->field90);
    }

    if (flag != 0 && (beh->field18 & 0x20000000) == 0) {
        int msg[8];
        Actor* target = actor;
        if (SendMsg(id, 0x400, msg) != 0 && msg[0] != 0) {
            target = (Actor*)msg[0];
        }
        fn_80122108(0.2f * beh->field34, beh->field34,
                    0.2f * beh->field38, beh->field38,
                    0.75f, 0.0f, 200.0f,
                    0, target->handle, pos, 0xb9fc42f5, actor->handle);
    }

    actor->flags |= 0x20000000;

    if (((ActorModelInfo*)actor->info)->fieldA0 == -1) {
        int msg[2];
        msg[1] = 0;
        SendMsg(actor->handle, 0xc00, msg);
    } else {
        int msg[8];
        memset(msg, 0, 0x20);
        msg[1] = ((ActorModelInfo*)actor->info)->fieldA0;
        msg[3] = 0x58;
        SendMsg(actor->handle, 0xc04, msg);
    }

    if ((int)actor->effect != 0) {
        fn_801D55CC();
        actor->effect = 0;
    }

    int model = ((ActorModelInfo*)actor->info)->field18;
    if (model != 0 && model != -1 && model != -0x55aded2f) {
        WhiteOutEffectMsg msg;
        msg.x = (float)actor->place.pos.x;
        msg.y = (float)actor->place.pos.y;
        msg.z = (float)actor->place.pos.z;
        msg.zero[0] = 0;
        msg.zero[1] = 0;
        msg.zero[2] = 0;
        msg.zero[3] = 0;
        msg.zero[4] = 0;
        msg.zero[5] = 0;
        msg.model = model;
        SendMsg(0x55000000, 0x1502, &msg);
    }

    fn_800C5654(actor->handle, 0x447d7202, id);
}

// White-out effect query; this variant never reports an active effect.
int WhiteOutEffectQuery()
{
    return 0;
}

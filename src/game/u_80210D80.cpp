// u_80210D80.cpp: particle stepping and draw parameters, the white-out effect and a lookup table (0x80210D80-0x802128E0).
// Names are descriptive guesses; the code is matched against the retail DOL.

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

extern "C" float lbl_804D9440;
extern "C" float lbl_804D9444;
extern "C" float lbl_804D9448;
extern "C" float lbl_804D944C;
extern "C" double lbl_804D9450;

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
    float zero = lbl_804D9440;
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
                    float t = lbl_804D9444;
                    if (colorRamp[0] > lbl_804D9448) {
                        t = x / colorRamp[0];
                    }
                    c0 = t * (colorRamp[6] - colorRamp[2]) + colorRamp[2];
                    c1 = t * (colorRamp[7] - colorRamp[3]) + colorRamp[3];
                    c2 = t * (colorRamp[8] - colorRamp[4]) + colorRamp[4];
                    c3 = t * (colorRamp[9] - colorRamp[5]) + colorRamp[5];
                } else if (y < colorRamp[1]) {
                    float t = lbl_804D9444;
                    if (colorRamp[1] > lbl_804D9448) {
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
                    float t = lbl_804D9444;
                    if (alphaRamp[0] > lbl_804D9448) {
                        t = x / alphaRamp[0];
                    }
                    alpha = t * (alphaRamp[3] - alphaRamp[2]) + alphaRamp[2];
                } else if (y < alphaRamp[1]) {
                    float t = lbl_804D9444;
                    if (alphaRamp[1] > lbl_804D9448) {
                        t = y / alphaRamp[1];
                    }
                    alpha = t * (alphaRamp[3] - alphaRamp[4]) + alphaRamp[4];
                } else {
                    alpha = alphaRamp[3];
                }
                float s = lbl_804D944C;
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
    float zero = lbl_804D9440;
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
                    float t = lbl_804D9444;
                    if (rampA[0] > lbl_804D9448) {
                        t = x / rampA[0];
                    }
                    a0 = t * (rampA[6] - rampA[2]) + rampA[2];
                    a1 = t * (rampA[7] - rampA[3]) + rampA[3];
                    a2 = t * (rampA[8] - rampA[4]) + rampA[4];
                    a3 = t * (rampA[9] - rampA[5]) + rampA[5];
                } else if (y < rampA[1]) {
                    float t = lbl_804D9444;
                    if (rampA[1] > lbl_804D9448) {
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
                    float t = lbl_804D9444;
                    if (rampB[0] > lbl_804D9448) {
                        t = x / rampB[0];
                    }
                    b0 = t * (rampB[6] - rampB[2]) + rampB[2];
                    b1 = t * (rampB[7] - rampB[3]) + rampB[3];
                    b2 = t * (rampB[8] - rampB[4]) + rampB[4];
                    b3 = t * (rampB[9] - rampB[5]) + rampB[5];
                } else if (y < rampB[1]) {
                    float t = lbl_804D9444;
                    if (rampB[1] > lbl_804D9448) {
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
                float s = lbl_804D944C;
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
    float zero = lbl_804D9440;
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
    float zero = lbl_804D9440;
    if (count < 1) {
        return;
    }
    do {
        float y = pts[1];
        if (zero < y) {
            float x = pts[0];
            if (zero < x) {
                pts[5] += vx;
                pts[6] += vy;
                pts[7] += vz;
            }
        }
        pts = (float*)((int)pts + stride);
    } while (--count);
}

// Advances each particle in the array by `dt`: integrates the position from the
// velocity, clamps the step to the remaining life, and wraps the spin angle.
// Returns the number of live particles advanced.
// fn_80211470
extern "C" int fn_80211470(float dt, float* particles, int stride, int count) {
    int live = 0;
    if (particles) {
        float zero = lbl_804D9440;
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
                                                lbl_804D9450);
                }
                live++;
            }
            particles = (float*)((char*)particles + stride);
        }
    }
    return live;
}

// Same particle integration as fn_80211470 but without the spin-angle
// wrap and with a count-down loop. Returns the number of live particles advanced.
int ParticleStepPlain(float dt, float* particles, int stride, int count) {
    int live = 0;
    if (particles && count > 0) {
        float zero = lbl_804D9440;
        do {
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
                }
                live++;
            }
            particles = (float*)((char*)particles + stride);
            count--;
        } while (count != 0);
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
    if (rot == 0) {
        out[2] = lbl_804D9440;
        out[3] = lbl_804D9440;
        out[4] = lbl_804D9440;
    } else {
        out[2] = rot[0];
        out[3] = rot[1];
        out[4] = rot[2];
    }
    if (scale == 0) {
        out[5] = lbl_804D9440;
        out[6] = lbl_804D9440;
        out[7] = lbl_804D9440;
    } else {
        out[5] = scale[0];
        out[6] = scale[1];
        out[7] = scale[2];
    }
    if (tint == 0) {
        out[8] = lbl_804D9440;
        out[9] = lbl_804D9440;
        out[10] = lbl_804D9444;
    } else {
        out[8] = tint[0];
        out[9] = tint[1];
        out[10] = tint[2];
    }
    out[11] = alpha;
    out[12] = depth;
    if (uv == 0) {
        out[13] = lbl_804D9440;
        out[14] = lbl_804D9440;
        out[15] = lbl_804D9440;
        out[16] = lbl_804D9444;
    } else {
        out[13] = uv[0];
        out[14] = uv[1];
        out[15] = uv[2];
        out[16] = uv[3];
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
    if (rot == 0) {
        out[2] = lbl_804D9440;
        out[3] = lbl_804D9440;
        out[4] = lbl_804D9440;
    } else {
        out[2] = rot[0];
        out[3] = rot[1];
        out[4] = rot[2];
    }
    if (scale == 0) {
        out[5] = lbl_804D9440;
        out[6] = lbl_804D9440;
        out[7] = lbl_804D9440;
    } else {
        out[5] = scale[0];
        out[6] = scale[1];
        out[7] = scale[2];
    }
}

// ---------------------------------------------------------------------------
// Particle direction scatter.

extern "C" int rand(void);
extern "C" float sqrtf(float x);
extern "C" double fabs(double x);
extern "C" float lbl_804D9458;
extern "C" float lbl_804D945C;

// Uniform random component built from rand(); the shared constants scale and
// centre it.
static inline float RandomComponent()
{
    return (float)rand() / lbl_804D9458 - lbl_804D945C;
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
    if ((fabs(r[0]) > lbl_804D9448) + (fabs(r[1]) > lbl_804D9448) +
            (fabs(r[2]) > lbl_804D9448) == 0) {
        r[0] = lbl_804D9440;
        r[1] = lbl_804D9440;
        r[2] = lbl_804D9440;
    } else {
        NormalizeVector3(r);
    }
    float dot = r[0] * in[0] + r[1] * in[1] + r[2] * in[2];
    out[0] = amount * (r[0] - in[0] * dot) + in[0];
    out[1] = amount * (r[1] - in[1] * dot) + in[1];
    out[2] = amount * (r[2] - in[2] * dot) + in[2];
    if ((fabs(out[0]) > lbl_804D9448) + (fabs(out[1]) > lbl_804D9448) +
            (fabs(out[2]) > lbl_804D9448) == 0) {
        out[0] = lbl_804D9440;
        out[1] = lbl_804D9440;
        out[2] = lbl_804D9440;
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

extern "C" float lbl_804D9468;
extern "C" float lbl_804D946C;
extern "C" float lbl_804D9470;
extern "C" float lbl_804D9474;
extern "C" float lbl_804D9478;
extern "C" float lbl_804D947C;
extern "C" float lbl_804D9488;
extern "C" float lbl_804D948C;
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

struct Actor {
    char pad00[0x24];
    int field24;            // position (fixed-point-ish ints)
    int field28;
    int field2C;
    char pad30[0x38 - 0x30];
    int field38;
    char pad3C[0x84 - 0x3C];
    ActorModelInfo* field84;
    char pad88[0x90 - 0x88];
    int field90;            // model node handle
    char pad94[0xA4 - 0x94];
    GhostState* fieldA4;
    int fieldA8;
    unsigned int fieldAC;
    char padB0[0x11C - 0xB0];
    int field11C;           // active white-out effect handle
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
    GhostState* beh = actor->fieldA4;
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
                ghost->field264C = lbl_804D9468;
                GhostNotify(ghost->field26E0, 0, 1);
            }
            float pos[3];
            pos[0] = (float)actor->field24;
            pos[1] = (float)actor->field28;
            pos[2] = (float)actor->field2C;
            fn_800B4C50(lbl_804D946C, lbl_804D9470, pos, actor->field38, 2, 0x12);
            beh->field10 = lbl_804D9474;
            WhiteOutEffectStart(actor, id, 1);
        }
    } else if (beh->field30 == lbl_804D9474) {
        CameraInfo* cam = fn_800C5DD8();
        float view[3];
        fn_800A025C(view, cam, 1);
        float dir[3];
        dir[0] = (float)actor->field24;
        dir[1] = (float)actor->field28;
        dir[2] = (float)actor->field2C;
        dir[0] = dir[0] - cam->field24;
        dir[1] = dir[1] - cam->field28;
        dir[2] = dir[2] - cam->field2C;
        float len2 = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
        if ((fabsf(dir[0]) > lbl_804D9478) + (fabsf(dir[1]) > lbl_804D9478) +
                (fabsf(dir[2]) > lbl_804D9478) == 0) {
            dir[0] = lbl_804D9474;
            dir[1] = lbl_804D9474;
            dir[2] = lbl_804D9474;
        } else {
            NormalizeVector3(dir);
        }
        float dot = dir[0] * view[0] + dir[1] * view[1] + dir[2] * view[2];
        if ((dot < lbl_804D947C && len2 > lbl_804D3410) ||
                fn_802A1D04() != 0) {
            fn_800C7B78(actor);
        }
    } else if ((flags & 0x8000000) != 0) {
        beh->field30 = beh->field30 - dt;
        if (beh->field30 < lbl_804D9474) {
            beh->field30 = lbl_804D9474;
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
extern "C" void fn_80211E38(Actor* actor)
{
    GhostState* beh = actor->fieldA4;
    if (beh == 0) {
        beh = (GhostState*)VAlloc(0x3c);
        actor->fieldA4 = beh;
    }
    memset(beh, 0, 0x3c);

    bool active;
    if (PossessableInit(actor) == 0) {
        active = false;
        actor->fieldAC |= 0x1000000;
    } else {
        actor->fieldAC &= 0xdfffffff;
        if (actor->field84->field4 != 0xa303c34 && actor->field84->field2C != 0) {
            int node = AssetFind(0xd9044cbb,
                                 (unsigned long)actor->field84->field2C);
            if (node != -1) {
                ModelNodeSetupByName(actor->field90, 0, 0, 1);
                ModelNodeSetup(actor->field90, node, 1, 1);
            }
        }
        active = true;
        beh->field18 |= 0x8000000;
        beh->field18 &= 0xfffffff7;
        beh->field34 = lbl_804D9488;
        beh->field38 = lbl_804D948C;
    }

    if (active && actor->fieldA4 != 0) {
        actor->fieldA4->field18 |= 0x30000000;
    }
}

// White-out ghost init: clears the behaviour block, marks the actor
// possessable and sets up its overlay model node. Returns whether the actor
// became active.
int Fn80211F78(Actor* actor)
{
    GhostState* beh = actor->fieldA4;
    if (beh == 0) {
        beh = (GhostState*)VAlloc(0x3c);
        actor->fieldA4 = beh;
    }
    memset(beh, 0, 0x3c);

    if (PossessableInit(actor) == 0) {
        actor->fieldAC |= 0x1000000;
        return 0;
    }
    actor->fieldAC &= 0xdfffffff;
    if (actor->field84->field4 != 0xa303c34 && actor->field84->field2C != 0) {
        int node = AssetFind(0xd9044cbb, (unsigned long)actor->field84->field2C);
        if (node != -1) {
            ModelNodeSetupByName(actor->field90, 0, 0, 1);
            ModelNodeSetup(actor->field90, node, 1, 1);
        }
    }
    beh->field18 |= 0x8000000;
    beh->field18 &= 0xfffffff7;
    beh->field34 = lbl_804D9488;
    beh->field38 = lbl_804D948C;
    return 1;
}

extern "C" float lbl_804D9490;
extern "C" float lbl_804D9494;

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
        GhostState* beh = target->fieldA4;
        unsigned int type = msgData->modelId >> 24;

        if ((beh->field18 & 8) == 0 &&
                (type == 1 || type == 2 || type == 0x13) &&
                (type != 0x13 || IsWhiteOutOwner(msgData->modelId))) {
            switch (msgData->animHash) {
            case 0x1ba3ee4a:
            case -0x4603bd0b:
                beh->field10 = lbl_804D9474;
                break;
            case 0x11a6b:
            case 0x403548d0:
                break;
            default:
                beh->field10 = beh->field10 - msgData->value.amount;
                break;
            }
            if (beh->field10 > lbl_804D9474) {
                beh->field14 = lbl_804D9474;
            }
            if ((beh->field18 & 0x80000000) == 0 &&
                    beh->field10 <= lbl_804D9474) {
                WhiteOutEffectStart(target, msgData->modelId, 1);
            }
        }
        result = 1;
        break;
    }
    case 0xc14: {
        Actor* target = msgData->actor;
        GhostState* beh = target->fieldA4;

        switch (msgData->value.flag) {
        case 0x672f35c2:
            if (fn_802A1D04() != 0) {
                int player = HandleToPlayer(msgData->modelId);
                if (player >= 0) {
                    unsigned long key;
                    const char* text = StrFindKey(0x2df0c512, &key);
                    HudMessageN(player, text, key, lbl_804D9490,
                                lbl_804D9490, lbl_804D9490, lbl_804D9494);
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
        GhostState* beh = target->fieldA4;

        switch (msgData->hash) {
        case -0x233c7b3f:
            WhiteOutEffectStart(target, target->field38, 1);
            result = 1;
            break;
        case -0x13ff57cf:
            WhiteOutEffectStart(target, target->field38, 0);
            result = 1;
            break;
        case -0x120729ad:
            beh->field18 |= 0x10000000;
            WhiteOutEffectStart(target, target->field38, 1);
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

extern "C" float lbl_804D9498;
extern "C" float lbl_804D949C;
extern "C" float lbl_804D94A0;
extern "C" float lbl_804D94A4;
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
    GhostState* beh = actor->fieldA4;
    if ((beh->field18 & 0x80000000) != 0) {
        return;
    }
    beh->field18 |= 0x80000000;
    beh->field30 = lbl_804D9498;

    float pos[3];
    pos[0] = (float)actor->field24;
    pos[1] = (float)actor->field28;
    pos[2] = (float)actor->field2C;

    if ((beh->field18 & 0x10000000) == 0) {
        ActorModelInfo* info = actor->field84;
        fn_80158C3C(pos, info->field24, info->field80, info->field84,
                    info->field88, info->field8C, info->field90);
    }

    if (flag != 0 && (beh->field18 & 0x20000000) == 0) {
        int msg[8];
        Actor* target = actor;
        if (SendMsg(id, 0x400, msg) != 0 && msg[0] != 0) {
            target = (Actor*)msg[0];
        }
        fn_80122108(lbl_804D949C * beh->field34, beh->field34,
                    lbl_804D949C * beh->field38, beh->field38,
                    lbl_804D94A0, lbl_804D9474, lbl_804D94A4,
                    0, target->field38, pos, 0xb9fc42f5, actor->field38);
    }

    actor->fieldAC |= 0x20000000;

    if (actor->field84->fieldA0 == -1) {
        int msg[2];
        msg[1] = 0;
        SendMsg(actor->field38, 0xc00, msg);
    } else {
        int msg[8];
        memset(msg, 0, 0x20);
        msg[1] = actor->field84->fieldA0;
        msg[3] = 0x58;
        SendMsg(actor->field38, 0xc04, msg);
    }

    if (actor->field11C != 0) {
        fn_801D55CC();
        actor->field11C = 0;
    }

    int model = actor->field84->field18;
    if (model != 0 && model != -1 && model != -0x55aded2f) {
        WhiteOutEffectMsg msg;
        msg.x = (float)actor->field24;
        msg.y = (float)actor->field28;
        msg.z = (float)actor->field2C;
        msg.zero[0] = 0;
        msg.zero[1] = 0;
        msg.zero[2] = 0;
        msg.zero[3] = 0;
        msg.zero[4] = 0;
        msg.zero[5] = 0;
        msg.model = model;
        SendMsg(0x55000000, 0x1502, &msg);
    }

    fn_800C5654(actor->field38, 0x447d7202, id);
}

// White-out effect query; this variant never reports an active effect.
int WhiteOutEffectQuery()
{
    return 0;
}

// Lookup helpers shared with the Actor unit (fn_80242BA4 / fn_80243448 /
// fn_80213970 live outside this split as placeholder-named symbols).
typedef int (*LookupCallback)(void* key, void* item);

extern "C" unsigned int lbl_804D53A8;
extern "C" unsigned int fn_80242BA4(unsigned int owner);
extern "C" int fn_80213970(void* key, void* item);
extern "C" unsigned int fn_80243448(unsigned int list, LookupCallback cb,
                                    void* key);

// Marks the table entry for actor id as active; returns 1 on success.
int MarkLookupEntryFlag(int id)
{
    int key[2];

    key[0] = id;
    if (id == -1) {
        return -1;
    }
    unsigned int entry = fn_80243448(fn_80242BA4(lbl_804D53A8), fn_80213970, key);
    if (entry == 0) {
        return -1;
    }
    *(unsigned int*)(entry + 0x1c) |= 0x10;
    return 1;
}

// Reads the word at offset 0x14 of the holder referenced by the actor's entry.
// fn_80212730
extern "C" int fn_80212730(int id)
{
    int key[2];

    key[0] = id;
    if (id == -1) {
        return -1;
    }
    int* entry = (int*)fn_80243448(fn_80242BA4(lbl_804D53A8), fn_80213970, key);
    if (entry == 0) {
        return -1;
    }
    if (*entry == 0) {
        return -1;
    }
    int* value = (int*)((char*)*entry + 0x14);
    if (value == 0) {
        return -1;
    }
    return *value;
}

extern "C" void fn_80212A58(void* a, void* b, void* c, void* info, void* d,
                            void* e, void* f);
void LinkInfoInit(void* info);

// Wrapper around fn_80212A58 that hands it a zeroed LinkInfo when the caller
// supplied none. Kept under its placeholder name: other units call it.
extern "C" void fn_802127A0(void* a, void* b, void* c, void* info, void* d,
                            void* e, void* f)
{
    if (info == 0) {
        char linkInfo[48];
        LinkInfoInit(linkInfo);
        fn_80212A58(a, b, c, linkInfo, d, e, f);
    } else {
        fn_80212A58(a, b, c, info, d, e, f);
    }
}

// Same wrapper, but always using the default zeroed LinkInfo. Kept under its
// placeholder name: other units call it.
extern "C" void fn_80212814(void* a, void* b, void* c, void* d, void* e,
                            void* f)
{
    fn_80212A58(a, b, c, 0, d, e, f);
}

// Pending-lookup table: base pointer, used-slot count.
extern "C" unsigned int lbl_804D53AC;
extern "C" int lbl_804D53B0;

// Appends a pending lookup entry (kind 2, actor id, two -1 words) to the
// 256-slot table and returns 1 on success. Kept under its placeholder name:
// other units call it as fn_8021284C.
extern "C" int fn_8021284C(int id)
{
    int key;

    key = id;
    if (id == -1) {
        return 0;
    }
    if (fn_80243448(fn_80242BA4(lbl_804D53A8), fn_80213970, &key) == 0) {
        return 0;
    }
    if (lbl_804D53B0 >= 0x100) {
        return 0;
    }
    int* entry = (int*)(lbl_804D53AC + lbl_804D53B0 * 0x44);
    entry[0] = 2;
    entry[1] = key;
    entry[2] = -1;
    entry[3] = -1;
    lbl_804D53B0++;
    return 1;
}

// u_80210D80.cpp: particle stepping and draw parameters (0x80210D80-0x80211A84); the white-out ghost after it is u_80211A84.cpp.
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

static inline float RampValue(float age, float life, float* r)
{
    if (age < r[0]) {
        float t = r[0] > 1.1920929e-07f ? age / r[0] : 1.0f;
        return t * (r[3] - r[2]) + r[2];
    } else if (life < r[1]) {
        float t = r[1] > 1.1920929e-07f ? life / r[1] : 1.0f;
        return t * (r[3] - r[4]) + r[4];
    }
    return r[3];
}
static inline void RampColor(float* c, float age, float life, float* r)
{
    if (age < r[0]) {
        float t = r[0] > 1.1920929e-07f ? age / r[0] : 1.0f;
        c[0] = t * (r[6] - r[2]) + r[2];
        c[1] = t * (r[7] - r[3]) + r[3];
        c[2] = t * (r[8] - r[4]) + r[4];
        c[3] = t * (r[9] - r[5]) + r[5];
    } else if (life < r[1]) {
        float t = r[1] > 1.1920929e-07f ? life / r[1] : 1.0f;
        c[0] = t * (r[6] - r[10]) + r[10];
        c[1] = t * (r[7] - r[11]) + r[11];
        c[2] = t * (r[8] - r[12]) + r[12];
        c[3] = t * (r[9] - r[13]) + r[13];
    } else {
        c[0] = r[6];
        c[1] = r[7];
        c[2] = r[8];
        c[3] = r[9];
    }
}
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
        if (p[1] > zero) {
            if (p[0] > zero) {
                float c[4];
                RampColor(c, p[0], p[1], colorRamp);
                float alpha = RampValue(p[0], p[1], alphaRamp);
                float s = 255.0f;
                DrawWorldQuadFlipped(alpha, p[11], p + 2,
                                     (unsigned char)(int)(s * c[0]),
                                     (unsigned char)(int)(s * c[1]),
                                     (unsigned char)(int)(s * c[2]),
                                     (unsigned char)(int)(s * c[3]));
            }
        }
        p = (float*)((int)p + stride);
    }
    Draw3DEnd();
}

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
        if (p[1] > zero) {
            if (p[0] > zero) {
                float a[4];
                float b[4];
                RampColor(a, p[0], p[1], rampA);
                RampColor(b, p[0], p[1], rampB);
                float end[3];
                end[0] = p[5] * scale;
                end[1] = p[6] * scale;
                end[2] = p[7] * scale;
                // Retail adds the start point to itself here, overwriting the scaled velocity.
                end[0] = p[2] + p[2];
                end[1] = p[3] + p[3];
                end[2] = p[4] + p[4];
                float s = 255.0f;
                DrawLine(p + 2, end,
                         (unsigned char)(int)(s * a[0]),
                         (unsigned char)(int)(s * a[1]),
                         (unsigned char)(int)(s * a[2]),
                         (unsigned char)(int)(s * a[3]),
                         (unsigned char)(int)(s * b[0]),
                         (unsigned char)(int)(s * b[1]),
                         (unsigned char)(int)(s * b[2]),
                         (unsigned char)(int)(s * b[3]));
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

extern "C" int fn_80211470(float dt, float* particles, int stride, int count) {
    int live = 0;
    if (particles) {
        float zero = 0.0f;
        float* p = particles;
        for (int i = 0; i < count; i++) {
            if (p[1] > zero) {
                p[0] = p[0] + dt;
                if (p[0] > zero) {
                    float step = p[0] < dt ? p[0] : dt;
                    p[1] = p[1] - step;
                    float vx = p[5] * step;
                    float vy = p[6] * step;
                    float vz = p[7] * step;
                    p[2] += vx;
                    p[3] += vy;
                    p[4] += vz;
                    p[11] = (float)fmod((double)(p[12] * step + p[11]), 360.0);
                }
                live++;
            }
            p = (float*)((char*)p + stride);
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

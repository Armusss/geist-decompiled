// u_8016D170.cpp: 12-bit fixed-point angle, IVec3, Euler-angle and Vec3 helpers (0x8016D170-0x8016D6BC).
// A separate source file between the collect_point behaviour and the box helpers: it carries its own copies
// of the 11.377778f and int-to-float constants (.sdata2 0x804D8028-0x804D804C, one pool shared by both
// halves). Built with GC/2.0: retail loads through the const input pointers before the stack frame is set
// up (STATUS G4).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"

extern "C" {
double sqrt(double);
int fn_800A1558(int a, int b);
int fn_800A1688(int a);
int fn_800A1624(int a);
float fn_800A1128(float y, float x);   // atan2 in degrees
void fn_800A0FC4(float* in, float* out);   // sin/cos of two angles
}

// 0x8016D170: swap x and y (used by the ghost action code).
void VecSwapXY(IVec3* out, IVec3* in) {
    int t = in->y;
    int x = in->x;

    out->x = t;
    t = in->z;
    out->y = x;
    out->z = t;
}

// 0x8016D18C: swap x and y (second copy used by the angle helpers).
void VecSwapXY2(IVec3* out, IVec3* in) {
    int t = in->y;
    int x = in->x;

    out->x = t;
    t = in->z;
    out->y = x;
    out->z = t;
}

// 0x8016D1A8: 12-bit fixed point -> degrees.
void FixedToRad(Vec3* out, const IVec3* in) {
    out->z = 0.087890625f * (float)in->z;
    out->x = 0.087890625f * (float)in->x;
    out->y = 0.087890625f * (float)in->y;
}

// 0x8016D220: degrees -> 12-bit fixed point.
void RadToFixed(IVec3* out, const Vec3* in) {
    out->z = (int)(11.377778f * in->z);
    out->x = (int)(11.377778f * in->x);
    out->y = (int)(11.377778f * in->y);
}

// 0x8016D278: horizontal direction of a vector, as fixed-point angles.
void VecToAngles(IVec3* out, const IVec3* in) {
    int len2 = in->x * in->x + in->y * in->y;
    int n;

    if (len2 <= 0) {
        n = 0;
    } else {
        n = (int)(float)sqrt((double)len2);
    }
    out->z = fn_800A1558(-in->x, in->y);
    out->x = fn_800A1558(in->z, n);
    out->y = 0;
}

// 0x8016D330: rotate a fixed-point vector by another.
void RotateVec(IVec3* out, IVec3* in) {
    int a;
    int b;
    int sina;
    int cosa;
    int sinb;
    int cosb;

    a = in->z;
    b = in->x;
    cosa = fn_800A1688(a);
    sina = fn_800A1624(a);
    cosb = fn_800A1688(b);
    sinb = fn_800A1624(b);
    out->x = (-sina * cosb) / 4096;
    out->y = (cosa * cosb) / 4096;
    out->z = sinb;
}

// 0x8016D3B4: wrap each fixed-point angle into [-0x800, 0x7FF].
void WrapAngle(IVec3* out, IVec3* in) {
    int v;

    v = in->z;
    if (v > 0x7FF) {
        out->z = v - 0x1000;
    } else if (v < -0x800) {
        out->z = v + 0x1000;
    }
    v = in->x;
    if (v > 0x7FF) {
        out->x = v - 0x1000;
    } else if (v < -0x800) {
        out->x = v + 0x1000;
    }
    v = in->y;
    if (v > 0x7FF) {
        out->y = v - 0x1000;
    } else if (v < -0x800) {
        out->y = v + 0x1000;
    }
}

// 0x8016D430: build a fixed-point vector from three components (z first).
void MakeVec(IVec3* out, int a, int b, int c) {
    out->z = a;
    out->x = b;
    out->y = c;
}

// Square root through the paired-single estimate instructions.
inline float FastSqrt(float x) {
    float r;
    if (x <= 0.0f) {
        r = 0.0f;
    } else {
        float y;
        register float t;
        asm {
            lfs t, x
            ps_rsqrte t, t
            ps_res t, t
            stfs t, y
        }
        r = y;
    }
    return r;
}

// EulerFromDirection: yaw (z) and pitch (x) angles of a direction vector, in degrees.
extern "C" void fn_8016D440(Vec3* out, const Vec3* v) {
    float sq = v->x * v->x + v->y * v->y;
    float len = FastSqrt(sq);
    out->z = fn_800A1128(-v->x, v->y);
    out->x = fn_800A1128(v->z, len);
    out->y = 0.0f;
}

// EulerToDirection: unit direction vector of a (z, x) angle pair.
extern "C" void fn_8016D4EC(Vec3* out, const Vec3* v) {
    float a[4];

    a[1] = v->x;
    a[0] = v->z;
    fn_800A0FC4(a, a);
    out->x = -a[0] * a[3];
    out->y = a[2] * a[3];
    out->z = a[1];
}

// Wraps each angle into [-180, 180).
void EulerNormalize(Vec3* out, Vec3* in) {
    float a = in->z;
    if (a >= 180.0f) {
        out->z = a - 360.0f;
    } else if (a < -180.0f) {
        out->z = 360.0f + a;
    }
    a = in->x;
    if (a >= 180.0f) {
        out->x = a - 360.0f;
    } else if (a < -180.0f) {
        out->x = 360.0f + a;
    }
    a = in->y;
    if (a >= 180.0f) {
        out->y = a - 360.0f;
    } else if (a < -180.0f) {
        out->y = 360.0f + a;
    }
}

// Vec3Sub
extern "C" void fn_8016D614(Vec3* out, const Vec3* a, const Vec3* b) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

// Vec3Add
extern "C" void fn_8016D648(Vec3* out, const Vec3* a, const Vec3* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

// Stores its three arguments in z, x, y order.
void Vec3Set(Vec3* v, float a, float b, float c) {
    v->z = a;
    v->x = b;
    v->y = c;
}

// Vec3Copy
extern "C" void fn_8016D68C(Vec3* dst, const Vec3* src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

// Vec3Zero
extern "C" void fn_8016D6A8(Vec3* v) {
    v->z = 0.0f;
    v->x = 0.0f;
    v->y = 0.0f;
}

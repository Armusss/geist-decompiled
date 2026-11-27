// u_80278D2C.cpp: ODE 0.5 math helpers (0x80278D2C-0x80279558); probably odemath.cpp (dNormalize3/4,
// dPlaneSpace use 1/sqrt(2)); guess.
// Split from u_80277600 on data evidence: own .sdata2 pool 0x804DA318-0x804DA340 (0.70710677f, the sqrt
// Newton constants 0.5/3.0 as doubles, 0.0, 1.0f) that the files before and after carry their own copies of;
// no code in 0x802775AC-0x80278D2C reads it.

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef float dReal;
typedef dReal dVector3[4];
typedef dReal dVector4[4];

#define REAL(x) (x##f)
#define M_SQRT1_2 REAL(0.7071067811865475244008443621048490)
#define dFabs(x) ((float)__fabs(x))
#define dSqrt(x) _dSqrtf(x)
#define dRecipSqrt(x) ((float)(1.0f / dSqrt(x)))
#define dCopySign(a, b) ((dReal)copysign(a, b))
#define dDOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])

extern "C" double copysign(double x, double y);
extern int __float_nan[];

// The game's MSL sqrtf (inline in its math.h), as in ode/matrix.cpp.
static inline int _dFpClassifyf(float x) {
    switch (*(unsigned long*)&x & 0x7F800000U) {
    case 0x7F800000U:
        if (*(unsigned long*)&x & 0x007FFFFFU) {
            return 1;
        }
        return 2;
    case 0:
        if (*(unsigned long*)&x & 0x007FFFFFU) {
            return 5;
        }
        return 3;
    default:
        return 4;
    }
}

static inline float _dSqrtf(float x) {
    static const double _half = 0.5;
    static const double _three = 3.0;

    if (x > 0.0f) {
        double dmag = (double)x;
        double guess = __frsqrte(dmag);
        guess = _half * guess * (_three - guess * guess * dmag);
        guess = _half * guess * (_three - guess * guess * dmag);
        guess = _half * guess * (_three - guess * guess * dmag);
        return (float)(dmag * guess);
    } else if (x < 0.0) {
        return *(float*)__float_nan;
    } else if (_dFpClassifyf(x) == 1) {
        return *(float*)__float_nan;
    }
    return x;
}

// dPlaneSpace
extern "C" void fn_80278D2C(const dVector3 n, dVector3 p, dVector3 q) {
    if (dFabs(n[2]) > M_SQRT1_2) {
        // choose p in y-z plane
        dReal a = n[1] * n[1] + n[2] * n[2];
        dReal k = dRecipSqrt(a);
        p[0] = 0;
        p[1] = -n[2] * k;
        p[2] = n[1] * k;
        // set q = n x p
        q[0] = a * k;
        q[1] = -n[0] * p[2];
        q[2] = n[0] * p[1];
    } else {
        // choose p in x-y plane
        dReal a = n[0] * n[0] + n[1] * n[1];
        dReal k = dRecipSqrt(a);
        p[0] = -n[1] * k;
        p[1] = n[0] * k;
        p[2] = 0;
        // set q = n x p
        q[0] = -n[2] * p[1];
        q[1] = n[2] * p[0];
        q[2] = a * k;
    }
}

// dNormalize4
extern "C" void fn_80278FE0(dVector4 a) {
    dReal l = dDOT(a, a) + a[3] * a[3];
    if (l > 0) {
        l = dRecipSqrt(l);
        a[0] *= l;
        a[1] *= l;
        a[2] *= l;
        a[3] *= l;
    } else {
        a[0] = 1;
        a[1] = 0;
        a[2] = 0;
        a[3] = 0;
    }
}

// dNormalize3
extern "C" void fn_8027914C(dVector3 a) {
    dReal a0, a1, a2, aa0, aa1, aa2, l;
    a0 = a[0];
    a1 = a[1];
    a2 = a[2];
    aa0 = dFabs(a0);
    aa1 = dFabs(a1);
    aa2 = dFabs(a2);
    if (aa1 > aa0) {
        if (aa2 > aa1) {
            goto aa2_largest;
        } else {  // aa1 is largest
            a0 /= aa1;
            a2 /= aa1;
            l = dRecipSqrt(a0 * a0 + a2 * a2 + 1);
            a[1] = dCopySign(l, a1);
            a[0] = a0 * l;
            a[2] = a2 * l;
        }
    } else {
        if (aa2 > aa0) {
        aa2_largest:  // aa2 is largest
            a0 /= aa2;
            a1 /= aa2;
            l = dRecipSqrt(a0 * a0 + a1 * a1 + 1);
            a[2] = dCopySign(l, a2);
            a[0] = a0 * l;
            a[1] = a1 * l;
        } else {  // aa0 is largest
            if (aa0 <= 0) {
                a[0] = 1;  // if all a's are zero, this is where we'll end up.
                a[1] = 0;  // return a default unit length vector.
                a[2] = 0;
                return;
            }
            a1 /= aa0;
            a2 /= aa0;
            l = dRecipSqrt(a1 * a1 + a2 * a2 + 1);
            a[0] = dCopySign(l, a0);
            a[1] = a1 * l;
            a[2] = a2 * l;
        }
    }
}

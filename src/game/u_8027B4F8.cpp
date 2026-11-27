// u_8027B4F8.cpp: ODE 0.5 rotation.cpp (0x8027B4F8-0x8027BD5C), reverse source order: dDQfromW, dQfromR, dRfromQ,
// dQMultiply3..0, dRSetIdentity (the dRFrom* functions in between are linker-stripped).
// Split from u_80277600 on data evidence: own .sdata2 pool 0x804DA358-0x804DA388 (0.5f, 0.0f, 1.0f, sqrt
// constants 0.5/3.0 doubles, 0.0 double, 2.0f) duplicating constants of u_80278D2C / u_80279558 / u_8027BD5C;
// fn_8027B4F8 is the first reader of 0.5f at 0x804DA358, so the file starts there.

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef float dReal;
typedef dReal dVector3[4];
typedef dReal dMatrix3[4 * 3];
typedef dReal dQuaternion[4];

#define REAL(x) (x##f)
#define dFabs(x) ((float)__fabs(x))
#define dSqrt(x) _dSqrtf(x)
#define dRecip(x) ((float)(1.0f / (x)))
#define dDOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])

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

#define _R(i, j) R[(i) * 4 + (j)]

// dDQfromW
extern "C" void fn_8027B4F8(dReal dq[4], const dVector3 w, const dQuaternion q) {
    dq[0] = REAL(0.5) * (-w[0] * q[1] - w[1] * q[2] - w[2] * q[3]);
    dq[1] = REAL(0.5) * (w[0] * q[0] + w[1] * q[3] - w[2] * q[2]);
    dq[2] = REAL(0.5) * (-w[0] * q[3] + w[1] * q[0] + w[2] * q[1]);
    dq[3] = REAL(0.5) * (w[0] * q[2] - w[1] * q[1] + w[2] * q[0]);
}

// dQfromR
extern "C" void fn_8027B570(dQuaternion q, const dMatrix3 R) {
    dReal tr, s;
    tr = _R(0, 0) + _R(1, 1) + _R(2, 2);
    if (tr >= 0) {
        s = dSqrt(tr + 1);
        q[0] = REAL(0.5) * s;
        s = REAL(0.5) * dRecip(s);
        q[1] = (_R(2, 1) - _R(1, 2)) * s;
        q[2] = (_R(0, 2) - _R(2, 0)) * s;
        q[3] = (_R(1, 0) - _R(0, 1)) * s;
    } else {
        // find the largest diagonal element and jump to the appropriate case
        if (_R(1, 1) > _R(0, 0)) {
            if (_R(2, 2) > _R(1, 1)) {
                goto case_2;
            }
            goto case_1;
        }
        if (_R(2, 2) > _R(0, 0)) {
            goto case_2;
        }
        goto case_0;

    case_0:
        s = dSqrt((_R(0, 0) - (_R(1, 1) + _R(2, 2))) + 1);
        q[1] = REAL(0.5) * s;
        s = REAL(0.5) * dRecip(s);
        q[2] = (_R(0, 1) + _R(1, 0)) * s;
        q[3] = (_R(2, 0) + _R(0, 2)) * s;
        q[0] = (_R(2, 1) - _R(1, 2)) * s;
        return;

    case_1:
        s = dSqrt((_R(1, 1) - (_R(2, 2) + _R(0, 0))) + 1);
        q[2] = REAL(0.5) * s;
        s = REAL(0.5) * dRecip(s);
        q[3] = (_R(1, 2) + _R(2, 1)) * s;
        q[1] = (_R(0, 1) + _R(1, 0)) * s;
        q[0] = (_R(0, 2) - _R(2, 0)) * s;
        return;

    case_2:
        s = dSqrt((_R(2, 2) - (_R(0, 0) + _R(1, 1))) + 1);
        q[3] = REAL(0.5) * s;
        s = REAL(0.5) * dRecip(s);
        q[1] = (_R(2, 0) + _R(0, 2)) * s;
        q[2] = (_R(1, 2) + _R(2, 1)) * s;
        q[0] = (_R(1, 0) - _R(0, 1)) * s;
        return;
    }
}

// dRfromQ
extern "C" void fn_8027BAA0(dMatrix3 R, const dQuaternion q) {
    // q = (s,vx,vy,vz)
    dReal qq1 = 2 * q[1] * q[1];
    dReal qq2 = 2 * q[2] * q[2];
    dReal qq3 = 2 * q[3] * q[3];
    _R(0, 0) = 1 - qq2 - qq3;
    _R(0, 1) = 2 * (q[1] * q[2] - q[0] * q[3]);
    _R(0, 2) = 2 * (q[1] * q[3] + q[0] * q[2]);
    _R(1, 0) = 2 * (q[1] * q[2] + q[0] * q[3]);
    _R(1, 1) = 1 - qq1 - qq3;
    _R(1, 2) = 2 * (q[2] * q[3] - q[0] * q[1]);
    _R(2, 0) = 2 * (q[1] * q[3] - q[0] * q[2]);
    _R(2, 1) = 2 * (q[2] * q[3] + q[0] * q[1]);
    _R(2, 2) = 1 - qq1 - qq2;
}

// dQMultiply3
extern "C" void fn_8027BB48(dQuaternion qa, const dQuaternion qb, const dQuaternion qc) {
    qa[0] = qb[0] * qc[0] - qb[1] * qc[1] - qb[2] * qc[2] - qb[3] * qc[3];
    qa[1] = -qb[0] * qc[1] - qb[1] * qc[0] + qb[2] * qc[3] - qb[3] * qc[2];
    qa[2] = -qb[0] * qc[2] - qb[2] * qc[0] + qb[3] * qc[1] - qb[1] * qc[3];
    qa[3] = -qb[0] * qc[3] - qb[3] * qc[0] + qb[1] * qc[2] - qb[2] * qc[1];
}

// dQMultiply2
extern "C" void fn_8027BBC0(dQuaternion qa, const dQuaternion qb, const dQuaternion qc) {
    qa[0] = qb[0] * qc[0] + qb[1] * qc[1] + qb[2] * qc[2] + qb[3] * qc[3];
    qa[1] = -qb[0] * qc[1] + qb[1] * qc[0] - qb[2] * qc[3] + qb[3] * qc[2];
    qa[2] = -qb[0] * qc[2] + qb[2] * qc[0] - qb[3] * qc[1] + qb[1] * qc[3];
    qa[3] = -qb[0] * qc[3] + qb[3] * qc[0] - qb[1] * qc[2] + qb[2] * qc[1];
}

// dQMultiply1
extern "C" void fn_8027BC38(dQuaternion qa, const dQuaternion qb, const dQuaternion qc) {
    qa[0] = qb[0] * qc[0] + qb[1] * qc[1] + qb[2] * qc[2] + qb[3] * qc[3];
    qa[1] = qb[0] * qc[1] - qb[1] * qc[0] - qb[2] * qc[3] + qb[3] * qc[2];
    qa[2] = qb[0] * qc[2] - qb[2] * qc[0] - qb[3] * qc[1] + qb[1] * qc[3];
    qa[3] = qb[0] * qc[3] - qb[3] * qc[0] - qb[1] * qc[2] + qb[2] * qc[1];
}

// dQMultiply0
extern "C" void fn_8027BCAC(dQuaternion qa, const dQuaternion qb, const dQuaternion qc) {
    qa[0] = qb[0] * qc[0] - qb[1] * qc[1] - qb[2] * qc[2] - qb[3] * qc[3];
    qa[1] = qb[0] * qc[1] + qb[1] * qc[0] + qb[2] * qc[3] - qb[3] * qc[2];
    qa[2] = qb[0] * qc[2] + qb[2] * qc[0] + qb[3] * qc[1] - qb[1] * qc[3];
    qa[3] = qb[0] * qc[3] + qb[3] * qc[0] + qb[1] * qc[2] - qb[2] * qc[1];
}

// dRSetIdentity
extern "C" void fn_8027BD20(dMatrix3 R) {
    R[0] = 1;
    R[1] = 0;
    R[2] = 0;
    R[3] = 0;
    R[4] = 0;
    R[5] = 1;
    R[6] = 0;
    R[7] = 0;
    R[8] = 0;
    R[9] = 0;
    R[10] = 1;
    R[11] = 0;
}

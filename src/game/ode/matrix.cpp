// matrix.cpp: Open Dynamics Engine (ODE) 0.5 "ode/src/matrix.cpp" (0x802766F0-0x80277190).
// ODE is Copyright (C) 2001-2003 Russell L. Smith, used under its BSD license.
// Written as ODE wrote it (single precision, dNODEBUG). Built with -inline deferred like the rest of the
// ODE library in Geist: functions come out in reverse source order. Only dSetZero, dSetValue,
// dFactorCholesky, dInvertPDMatrix and dIsPositiveDefinite survive the linker; dSolveCholesky and dSetZero
// are also inlined into dInvertPDMatrix. The other functions are here because they are part of the file:
// the literals they use first (1.0f in dLDLTRemove, 0.0f in dLDLTAddTL) set the retail .sdata2 order.

typedef float dReal;
typedef unsigned long size_t;

#define REAL(x) (x##f)
#define dRecip(x) ((float)(1.0f / (x)))
#define dSqrt(x) _dSqrtf(x)
#define M_SQRT1_2 REAL(0.7071067811865475244008443621048490)

#define dPAD(a) (((a) > 1) ? ((((a)-1) | 3) + 1) : (a))
#define EFFICIENT_ALIGNMENT 16
#define dEFFICIENT_SIZE(x) ((((x)-1) | (EFFICIENT_ALIGNMENT - 1)) + 1)
#define dALLOCA16(n) ((char*)dEFFICIENT_SIZE(((size_t)(__alloca((n) + (EFFICIENT_ALIGNMENT - 1))))))
#define ALLOCA dALLOCA16

#define dAASSERT(a)

extern "C" {
void* memcpy(void* dst, const void* src, size_t n);
void* memmove(void* dst, const void* src, size_t n);
}

extern int __float_nan[];

// The game's MSL sqrtf (inline in its math.h): three Newton steps on frsqrte; NaN for x < 0 and for NaN.
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

void dSolveL1(const dReal* L, dReal* b, int n, int lskip1);
void dSolveL1T(const dReal* L, dReal* b, int n, int lskip1);
dReal dDot(const dReal* a, const dReal* b, int n);

void dSetZero(dReal* a, int n)
{
    dAASSERT(a && n >= 0);
    while (n > 0) {
        *(a++) = 0;
        n--;
    }
}

void dSetValue(dReal* a, int n, dReal value)
{
    dAASSERT(a && n >= 0);
    while (n > 0) {
        *(a++) = value;
        n--;
    }
}

void dMultiply0(dReal* A, const dReal* B, const dReal* C, int p, int q, int r)
{
    int i, j, k, qskip, rskip, rpad;
    dAASSERT(A && B && C && p > 0 && q > 0 && r > 0);
    qskip = dPAD(q);
    rskip = dPAD(r);
    rpad = rskip - r;
    dReal sum;
    const dReal *b, *c, *bb;
    bb = B;
    for (i = p; i; i--) {
        for (j = 0; j < r; j++) {
            c = C + j;
            b = bb;
            sum = 0;
            for (k = q; k; k--, c += rskip) sum += (*(b++)) * (*c);
            *(A++) = sum;
        }
        A += rpad;
        bb += qskip;
    }
}

void dMultiply1(dReal* A, const dReal* B, const dReal* C, int p, int q, int r)
{
    int i, j, k, pskip, rskip;
    dReal sum;
    dAASSERT(A && B && C && p > 0 && q > 0 && r > 0);
    pskip = dPAD(p);
    rskip = dPAD(r);
    for (i = 0; i < p; i++) {
        for (j = 0; j < r; j++) {
            sum = 0;
            for (k = 0; k < q; k++) sum += B[i + k * pskip] * C[j + k * rskip];
            A[i * rskip + j] = sum;
        }
    }
}

void dMultiply2(dReal* A, const dReal* B, const dReal* C, int p, int q, int r)
{
    int i, j, k, z, rpad, qskip;
    dReal sum;
    const dReal *bb, *cc;
    dAASSERT(A && B && C && p > 0 && q > 0 && r > 0);
    rpad = dPAD(r) - r;
    qskip = dPAD(q);
    bb = B;
    for (i = p; i; i--) {
        cc = C;
        for (j = r; j; j--) {
            z = 0;
            sum = 0;
            for (k = q; k; k--, z++) sum += bb[z] * cc[z];
            *(A++) = sum;
            cc += qskip;
        }
        A += rpad;
        bb += qskip;
    }
}

int dFactorCholesky(dReal* A, int n)
{
    int i, j, k, nskip;
    dReal sum, *a, *b, *aa, *bb, *cc, *recip;
    dAASSERT(n > 0 && A);
    nskip = dPAD(n);
    recip = (dReal*)ALLOCA(n * sizeof(dReal));
    aa = A;
    for (i = 0; i < n; i++) {
        bb = A;
        cc = A + i * nskip;
        for (j = 0; j < i; j++) {
            sum = *cc;
            a = aa;
            b = bb;
            for (k = j; k; k--) sum -= (*(a++)) * (*(b++));
            *cc = sum * recip[j];
            bb += nskip;
            cc++;
        }
        sum = *cc;
        a = aa;
        for (k = i; k; k--, a++) sum -= (*a) * (*a);
        if (sum <= REAL(0.0)) return 0;
        *cc = dSqrt(sum);
        recip[i] = dRecip(*cc);
        aa += nskip;
    }
    return 1;
}

void dSolveCholesky(const dReal* L, dReal* b, int n)
{
    int i, k, nskip;
    dReal sum, *y;
    dAASSERT(n > 0 && L && b);
    nskip = dPAD(n);
    y = (dReal*)ALLOCA(n * sizeof(dReal));
    for (i = 0; i < n; i++) {
        sum = 0;
        for (k = 0; k < i; k++) sum += L[i * nskip + k] * y[k];
        y[i] = (b[i] - sum) / L[i * nskip + i];
    }
    for (i = n - 1; i >= 0; i--) {
        sum = 0;
        for (k = i + 1; k < n; k++) sum += L[k * nskip + i] * b[k];
        b[i] = (y[i] - sum) / L[i * nskip + i];
    }
}

int dInvertPDMatrix(const dReal* A, dReal* Ainv, int n)
{
    int i, j, nskip;
    dReal *L, *x;
    dAASSERT(n > 0 && A && Ainv);
    nskip = dPAD(n);
    L = (dReal*)ALLOCA(nskip * n * sizeof(dReal));
    memcpy(L, A, nskip * n * sizeof(dReal));
    x = (dReal*)ALLOCA(n * sizeof(dReal));
    if (dFactorCholesky(L, n) == 0) return 0;
    dSetZero(Ainv, n * nskip);  // make sure all padding elements set to 0
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) x[j] = 0;
        x[i] = 1;
        dSolveCholesky(L, x, n);
        for (j = 0; j < n; j++) Ainv[j * nskip + i] = x[j];
    }
    return 1;
}

int dIsPositiveDefinite(const dReal* A, int n)
{
    dAASSERT(n > 0 && A);
    int nskip = dPAD(n);
    dReal* Acopy = (dReal*)ALLOCA(nskip * n * sizeof(dReal));
    memcpy(Acopy, A, nskip * n * sizeof(dReal));
    return dFactorCholesky(Acopy, n);
}

void dVectorScale(dReal* a, const dReal* d, int n)
{
    dAASSERT(a && d && n >= 0);
    for (int i = 0; i < n; i++) a[i] *= d[i];
}

void dSolveLDLT(const dReal* L, const dReal* d, dReal* b, int n, int nskip)
{
    dAASSERT(L && d && b && n > 0 && nskip >= n);
    dSolveL1(L, b, n, nskip);
    dVectorScale(b, d, n);
    dSolveL1T(L, b, n, nskip);
}

void dLDLTAddTL(dReal* L, dReal* d, const dReal* a, int n, int nskip)
{
    int j, p;
    dReal *W1, *W2, W11, W21, alpha1, alpha2, alphanew, gamma1, gamma2, k1, k2, Wp, ell, dee;
    dAASSERT(L && d && a && n > 0 && nskip >= n);

    if (n < 2) return;
    W1 = (dReal*)ALLOCA(n * sizeof(dReal));
    W2 = (dReal*)ALLOCA(n * sizeof(dReal));

    W1[0] = 0;
    W2[0] = 0;
    for (j = 1; j < n; j++) W1[j] = W2[j] = a[j] * M_SQRT1_2;
    W11 = (REAL(0.5) * a[0] + 1) * M_SQRT1_2;
    W21 = (REAL(0.5) * a[0] - 1) * M_SQRT1_2;

    alpha1 = 1;
    alpha2 = 1;

    dee = d[0];
    alphanew = alpha1 + (W11 * W11) * dee;
    dee /= alphanew;
    gamma1 = W11 * dee;
    dee *= alpha1;
    alpha1 = alphanew;
    alphanew = alpha2 - (W21 * W21) * dee;
    dee /= alphanew;
    gamma2 = W21 * dee;
    alpha2 = alphanew;
    k1 = REAL(1.0) - W21 * gamma1;
    k2 = W21 * gamma1 * W11 - W21;
    for (p = 1; p < n; p++) {
        Wp = W1[p];
        ell = L[p * nskip];
        W1[p] = Wp - W11 * ell;
        W2[p] = k1 * Wp + k2 * ell;
    }

    for (j = 1; j < n; j++) {
        dee = d[j];
        alphanew = alpha1 + (W1[j] * W1[j]) * dee;
        dee /= alphanew;
        gamma1 = W1[j] * dee;
        dee *= alpha1;
        alpha1 = alphanew;
        alphanew = alpha2 - (W2[j] * W2[j]) * dee;
        dee /= alphanew;
        gamma2 = W2[j] * dee;
        dee *= alpha2;
        d[j] = dee;
        alpha2 = alphanew;

        k1 = W1[j];
        k2 = W2[j];
        for (p = j + 1; p < n; p++) {
            ell = L[p * nskip + j];
            Wp = W1[p] - k1 * ell;
            ell += gamma1 * Wp;
            W1[p] = Wp;
            Wp = W2[p] - k2 * ell;
            ell -= gamma2 * Wp;
            W2[p] = Wp;
            L[p * nskip + j] = ell;
        }
    }
}

// macros for dLDLTRemove() for accessing A - either access the matrix
// directly or access it via row pointers. we are only supposed to reference
// the lower triangle of A (it is symmetric), but indexes i and j come from
// permutation vectors so they are not predictable. so do a test on the
// indexes - this should not slow things down too much, as we don't do this
// in an inner loop.

#define _GETA(i, j) (A[i][j])
#define GETA(i, j) ((i > j) ? _GETA(i, j) : _GETA(j, i))

void dRemoveRowCol(dReal* A, int n, int nskip, int r);

void dLDLTRemove(dReal** A, const int* p, dReal* L, dReal* d, int n1, int n2, int r, int nskip)
{
    int i;
    dAASSERT(A && p && L && d && n1 > 0 && n2 > 0 && r >= 0 && r < n2 && n1 >= n2 && nskip >= n1);

    if (r == n2 - 1) {
        return;  // deleting last row/col is easy
    } else if (r == 0) {
        dReal* a = (dReal*)ALLOCA(n2 * sizeof(dReal));
        for (i = 0; i < n2; i++) a[i] = -GETA(p[i], p[0]);
        a[0] += REAL(1.0);
        dLDLTAddTL(L, d, a, n2, nskip);
    } else {
        dReal* t = (dReal*)ALLOCA(r * sizeof(dReal));
        dReal* a = (dReal*)ALLOCA((n2 - r) * sizeof(dReal));
        for (i = 0; i < r; i++) t[i] = L[r * nskip + i] / d[i];
        for (i = 0; i < (n2 - r); i++) a[i] = dDot(L + (r + i) * nskip, t, r) - GETA(p[r + i], p[r]);
        a[0] += REAL(1.0);
        dLDLTAddTL(L + r * nskip + r, d + r, a, n2 - r, nskip);
    }

    // snip out row/column r from L and d
    dRemoveRowCol(L, n2, nskip, r);
    if (r < (n2 - 1)) memmove(d + r, d + r + 1, (n2 - r - 1) * sizeof(dReal));
}

void dRemoveRowCol(dReal* A, int n, int nskip, int r)
{
    int i;
    dAASSERT(A && n > 0 && nskip >= n && r >= 0 && r < n);
    if (r >= n - 1) return;
    if (r > 0) {
        for (i = 0; i < r; i++) memmove(A + i * nskip + r, A + i * nskip + r + 1, (n - r - 1) * sizeof(dReal));
        for (i = r; i < (n - 1); i++) memcpy(A + i * nskip, A + i * nskip + nskip, r * sizeof(dReal));
    }
    for (i = r; i < (n - 1); i++) memcpy(A + i * nskip + r, A + i * nskip + nskip + r + 1, (n - r - 1) * sizeof(dReal));
}

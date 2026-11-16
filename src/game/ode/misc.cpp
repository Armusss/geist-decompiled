// misc.cpp: Open Dynamics Engine (ODE) 0.5 "ode/src/misc.cpp" (0x80277220-0x80277290).
// ODE is Copyright (C) 2001-2003 Russell L. Smith, used under its BSD license.
// Written as ODE wrote it (single precision). Built with -inline deferred (reverse source order):
// only dRandInt survives the linker (dRand is inlined into it). dRandReal is generated before dRandInt,
// so the unsigned-to-double constant it uses first comes before dRandInt's 2^32 and int-to-double ones.

typedef float dReal;
typedef unsigned long size_t;
struct FILE;

#define REAL(x) (x##f)
#define dFabs(x) ((float)__fabs(x))

extern "C" int fprintf(FILE* stream, const char* format, ...);

static unsigned long seed = 0;

unsigned long dRand()
{
    seed = (1664525L * seed + 1013904223L) & 0xffffffff;
    return seed;
}

unsigned long dRandGetSeed()
{
    return seed;
}

void dRandSetSeed(unsigned long s)
{
    seed = s;
}

int dTestRand()
{
    unsigned long oldseed = seed;
    int ret = 1;
    seed = 0;
    if (dRand() != 0x3c6ef35f || dRand() != 0x47502932 || dRand() != 0xd1ccf6e9 || dRand() != 0xaaf95334 ||
        dRand() != 0x915b2fd2)
        ret = 0;
    seed = oldseed;
    return ret;
}

int dRandInt(int n)
{
    double a = double(n) / 4294967296.0;
    return (int)(double(dRand()) * a);
}

dReal dRandReal()
{
    return ((dReal)dRand()) / ((dReal)0xffffffff);
}

//****************************************************************************
// matrix utility stuff

void dPrintMatrix(const dReal* A, int n, int m, char* fmt, FILE* f)
{
    int i, j;
    int skip = (m > 1) ? ((((m)-1) | 3) + 1) : (m);
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) fprintf(f, fmt, A[i * skip + j]);
        fprintf(f, "\n");
    }
}

void dMakeRandomVector(dReal* A, int n, dReal range)
{
    int i;
    for (i = 0; i < n; i++) A[i] = (dRandReal() * REAL(2.0) - REAL(1.0)) * range;
}

void dMakeRandomMatrix(dReal* A, int n, int m, dReal range)
{
    int i, j;
    int skip = (m > 1) ? ((((m)-1) | 3) + 1) : (m);
    for (i = 0; i < n * skip; i++) A[i] = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) A[i * skip + j] = (dRandReal() * REAL(2.0) - REAL(1.0)) * range;
    }
}

void dClearUpperTriangle(dReal* A, int n)
{
    int i, j;
    int skip = (n > 1) ? ((((n)-1) | 3) + 1) : (n);
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) A[i * skip + j] = 0;
    }
}

dReal dMaxDifference(const dReal* A, const dReal* B, int n, int m)
{
    int i, j;
    int skip = (m > 1) ? ((((m)-1) | 3) + 1) : (m);
    dReal diff, max;
    max = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            diff = dFabs(A[i * skip + j] - B[i * skip + j]);
            if (diff > max) max = diff;
        }
    }
    return max;
}

dReal dMaxDifferenceLowerTriangle(const dReal* A, const dReal* B, int n)
{
    int i, j;
    int skip = (n > 1) ? ((((n)-1) | 3) + 1) : (n);
    dReal diff, max;
    max = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j <= i; j++) {
            diff = dFabs(A[i * skip + j] - B[i * skip + j]);
            if (diff > max) max = diff;
        }
    }
    return max;
}

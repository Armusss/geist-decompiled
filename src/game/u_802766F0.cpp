// u_802766F0.cpp: matrix and memory helpers used by the physics/joint code
// (0x802766F0-0x80277600).
// - Cholesky factorisation and symmetric matrix inversion of small matrices.
// - A bump allocator over 0x2000-byte blocks plus small block-pool helpers.
// - Allocation hook setters and a small LCG.
// Names are descriptive guesses; the code is matched against the retail DOL.

#pragma auto_inline off

typedef unsigned long u32;
typedef signed long s32;

extern "C" {
void* memcpy(void* dst, const void* src, u32 n);
void* malloc(u32 size);
void free(void* p);
void fn_802734DC(int flags, const char* msg, ...);
void fn_8027BD5C(void* obj, void (*fn)(void));
void fn_80279558(void);
}

extern int __float_nan[];

typedef void* (*AllocCallback)(u32 size);
typedef void (*FreeCallback)(void* p, u32 size);

static AllocCallback sAllocCallback;    // 0x804D5530
static FreeCallback sHookCallback5534;  // 0x804D5534 (set here, used elsewhere)
static FreeCallback sFreeCallback;      // 0x804D5538
static u32 sRandState;                  // 0x804D5540

struct MemBlock {
    MemBlock* next;  // 0x00
    int used;        // 0x04
};

struct MemPool {
    MemBlock* blocks;   // 0x00
    MemBlock* current;  // 0x04
    MemBlock* arena;    // 0x08
    int offset;         // 0x0C
};

#define ALIGN_OFFSET(base, off) ((int)((((u32)(base) + (u32)(off) - 1) | 0xFU) + 1) - (int)(base))

// Classify a float the way the standard fpclassify macro does: 1 NaN, 2 inf,
// 3 zero, 5 subnormal, 4 anything else.
static inline int FpClassifyF(float x) {
    switch (*(u32*)&x & 0x7F800000U) {
    case 0x7F800000U:
        if (*(u32*)&x & 0x007FFFFFU) {
            return 1;
        }
        return 2;
    case 0:
        if (*(u32*)&x & 0x007FFFFFU) {
            return 5;
        }
        return 3;
    default:
        return 4;
    }
}

// Single-precision square root: reciprocal-square-root estimate refined with
// three Newton steps. Negative inputs and NaNs give NaN.
static inline float SqrtF(float x) {
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
    } else if (FpClassifyF(x) == 1) {
        return *(float*)__float_nan;
    }
    return x;
}

int SolverCholeskyFactor(float* a, int n);

// Returns non-zero when the n x n matrix is symmetric positive definite.
int SolverIsPositiveDefinite(const float* src, int n) {
    int n4 = n;

    if (1 < n) {
        n4 = ((n - 1) | 3) + 1;
    }
    u32 sp = (u32)__alloca(n4 * n * 4 + 0xF);
    float* mat = (float*)(((sp - 1) | 0xF) + 1);

    memcpy(mat, src, n4 * n * 4);
    return SolverCholeskyFactor(mat, n);
}

// Inverts a symmetric positive definite matrix through its Cholesky factor.
// dst is column-major with a padded stride of n4 floats.
static inline void SolverSolveCholesky(const float* L, float* b, int n) {
    int i, k, nskip;
    float sum, *y;

    nskip = (n > 1) ? (((n - 1) | 3) + 1) : n;
    y = (float*)((((u32)__alloca(n * sizeof(float) + 15)) - 1 | 15) + 1);
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

static inline void SolverSetZero(float* a, int n) {
    while (n > 0) {
        *(a++) = 0;
        n--;
    }
}

int SolverInvertSymmetric(const float* A, float* Ainv, int n) {
    int i, j, nskip;
    float *L, *x;

    nskip = (n > 1) ? (((n - 1) | 3) + 1) : n;
    L = (float*)((((u32)__alloca(nskip * n * sizeof(float) + 15)) - 1 | 15) + 1);
    memcpy(L, A, nskip * n * sizeof(float));
    x = (float*)((((u32)__alloca(n * sizeof(float) + 15)) - 1 | 15) + 1);
    if (SolverCholeskyFactor(L, n) == 0) return 0;
    SolverSetZero(Ainv, n * nskip);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) x[j] = 0;
        x[i] = 1;
        SolverSolveCholesky(L, x, n);
        for (j = 0; j < n; j++) Ainv[j * nskip + i] = x[j];
    }
    return 1;
}

// In-place Cholesky factorisation (lower triangle only). Returns 0 when the
// matrix is not positive definite.
int SolverCholeskyFactor(float* A, int n) {
    int i, j, k, nskip;
    float sum, *a, *b, *aa, *bb, *cc, *recip;

    nskip = (n > 1) ? (((n - 1) | 3) + 1) : n;
    recip = (float*)((((u32)__alloca(n * sizeof(float) + 15)) - 1 | 15) + 1);
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
        if (sum <= 0.0f) return 0;
        *cc = SqrtF(sum);
        recip[i] = 1.0f / *cc;
        aa += nskip;
    }
    return 1;
}

// Stores value into count floats.
void SolverFillFloats(float* dst, float value, int count) {
    while (count > 0) {
        *dst++ = value;
        count--;
    }
}

// Clears count floats.
void SolverClearFloats(float* dst, int count) {
    while (count > 0) {
        *dst++ = 0.0f;
        count--;
    }
}

// Frees a block through the installed callback, or the C library.
void SolverFreeBlock(void* p, u32 size) {
    if (p != 0) {
        if (sFreeCallback != 0) {
            sFreeCallback(p, size);
        } else {
            free(p);
        }
    }
}

// Allocates a block through the installed callback, or the C library.
void* SolverAllocBlock(u32 size) {
    if (sAllocCallback != 0) {
        return sAllocCallback(size);
    }
    return malloc(size);
}

void SolverSetFreeCallback(FreeCallback fn) {
    sFreeCallback = fn;
}

void SolverSetHookCallback(FreeCallback fn) {
    sHookCallback5534 = fn;
}

void SolverSetAllocCallback(AllocCallback fn) {
    sAllocCallback = fn;
}

// Random integer in [0, range), from a linear congruential generator.
static inline u32 SolverRand() {
    sRandState = (1664525L * sRandState + 1013904223L) & 0xffffffff;
    return sRandState;
}

int SolverRandRange(int n) {
    double a = double(n) / 4294967296.0;
    return (int)(double(SolverRand()) * a);
}

// Carves size bytes out of the arena's current block, 16-byte aligned.
void* SolverArenaAlloc(MemPool* p, int size) {
    if (p->arena == 0) {
        return 0;
    }
    p->offset += size;
    p->offset = ALIGN_OFFSET(p->arena, p->offset);
    if (p->offset >= p->arena->used) {
        p->arena = p->arena->next;
        if (p->arena == 0) {
            return 0;
        }
        p->offset = 8;
        p->offset = ALIGN_OFFSET(p->arena, p->offset);
    }
    return (char*)p->arena + p->offset;
}

// Rewinds the arena to the first block and returns its first free byte.
void* SolverArenaBegin(MemPool* p) {
    p->arena = p->blocks;
    p->offset = 8;
    if (p->arena != 0) {
        p->offset = ALIGN_OFFSET(p->arena, p->offset);
        return (char*)p->arena + p->offset;
    }
    return 0;
}

// Rewinds the block pool's bump pointer.
void SolverPoolReset(MemPool* p) {
    p->current = p->blocks;
    if (p->blocks == 0) {
        return;
    }
    p->blocks->used = 8;
    p->blocks->used = ALIGN_OFFSET(p->blocks, p->blocks->used);
}

// Allocates size bytes from the pool, adding a 0x2000-byte block when the
// current one is full.
void* SolverPoolAlloc(MemPool* p, int size) {
    void* ptr;

    if ((u32)size > 0x1FE9) {
        fn_802734DC(0, "num_bytes too large");
    }
    if (p->blocks == 0) {
        p->blocks = p->current = (MemBlock*)SolverAllocBlock(0x2000);
        p->blocks->next = 0;
        p->blocks->used = 8;
        p->blocks->used = ALIGN_OFFSET(p->blocks, p->blocks->used);
    } else if (p->current->used + size > 0x2000) {
        if (p->current->next == 0) {
            p->current->next = (MemBlock*)SolverAllocBlock(0x2000);
            p->current->next->next = 0;
        }
        p->current = p->current->next;
        p->current->used = 8;
        p->current->used = ALIGN_OFFSET(p->current, p->current->used);
    }
    ptr = (char*)p->current + p->current->used;
    p->current->used += size;
    p->current->used = ALIGN_OFFSET(p->current, p->current->used);
    return ptr;
}

// Releases every block of the pool, and the pool itself when freePool > 0.
void* SolverPoolDestroy(MemPool* p, short freePool) {
    if (p != 0) {
        MemBlock* b = p->blocks;
        while (b != 0) {
            MemBlock* next = b->next;
            SolverFreeBlock(b, 0x2000);
            b = next;
        }
        if (freePool > 0) {
            SolverFreeBlock(p, 0x10);
        }
    }
    return p;
}

struct SolverState {
    int f00;
    int f04;
    int f08;
    int f0C;
    float f10;
    float f14;
    float f18;
    int pad1C[7];
    int flag38;
    int field3C;
};

void SolverClearState(SolverState* s) {
    s->f00 = 0;
    s->f04 = 0;
    s->f08 = 0;
    s->f0C = 0;
}

void SolverSetField3C(SolverState* s, int value) {
    s->field3C = value;
}

void SolverSetFlag38(SolverState* s, u32 value) {
    s->flag38 = (value != 0);
}

void SolverInitState(SolverState* s) {
    fn_8027BD5C(s, fn_80279558);
}

void SolverSetVector(SolverState* s, float x, float y, float z) {
    s->f10 = x;
    s->f14 = y;
    s->f18 = z;
}

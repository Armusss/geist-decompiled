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
        double guess = __frsqrte((double)x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        x = (float)(x * guess);
    } else if (x < 0.0) {
        x = *(float*)__float_nan;
    } else if (FpClassifyF(x) == 1) {
        x = *(float*)__float_nan;
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
int SolverInvertSymmetric(const float* src, float* dst, int n) {
    int n4 = n;
    int i;
    int k;
    int j;

    if (1 < n) {
        n4 = ((n - 1) | 3) + 1;
    }
    u32 sp = (u32)__alloca(n4 * n * 4 + 0xF);
    float* mat = (float*)(((sp - 1) | 0xF) + 1);

    memcpy(mat, src, n4 * n * 4);
    sp = (u32)__alloca(n * 4 + 0xF);
    float* y = (float*)(((sp - 1) | 0xF) + 1);
    if (SolverCholeskyFactor(mat, n) == 0) {
        return 0;
    }
    {
        int count = n * n4;
        float* dp = dst;

        while (count > 0) {
            *dp++ = 0.0f;
            count--;
        }
    }
    for (k = 0; k < n; k++) {
        float sum;

        for (i = 0; i < n; i++) {
            y[i] = 0.0f;
        }
        y[k] = 1.0f;
        int stride = n;

        if (1 < stride) {
            stride = ((n - 1) | 3) + 1;
        }
        sp = (u32)__alloca(n * 4 + 0xF);
        float* tmp = (float*)(((sp - 1) | 0xF) + 1);
        // Forward substitution: solve L * tmp = e(k).
        for (i = 0; i < n; i++) {
            sum = 0.0f;
            for (j = 0; j < i; j++) {
                sum += tmp[j] * mat[i * stride + j];
            }
            tmp[i] = (y[i] - sum) / mat[i * stride + i];
        }
        // Back substitution: solve L' * y = tmp.
        for (i = n - 1; i >= 0; i--) {
            sum = 0.0f;
            for (j = i + 1; j < n; j++) {
                sum += y[j] * mat[j * stride + i];
            }
            y[i] = (tmp[i] - sum) / mat[i * stride + i];
        }
        for (i = 0; i < n; i++) {
            dst[k + i * n4] = y[i];
        }
    }
    return 1;
}

// In-place Cholesky factorisation (lower triangle only). Returns 0 when the
// matrix is not positive definite.
int SolverCholeskyFactor(float* a, int n) {
    int n4 = n;
    int i;
    int j;
    int k;

    if (1 < n) {
        n4 = ((n - 1) | 3) + 1;
    }
    u32 sp = (u32)__alloca(n * 4 + 0xF);
    float* inv = (float*)(((sp - 1) | 0xF) + 1);
    for (i = 0; i < n; i++) {
        float* row = a + i * n4;
        float* rp = row;
        float* col = a;
        float d;

        for (j = 0; j < i; j++) {
            float sum = *rp;
            float* p = row;
            float* q = col;
            int count = j;

            while (count != 0) {
                sum -= *p++ * *q++;
                count--;
            }
            *rp = sum * inv[j];
            rp++;
            col += n4;
        }
        d = *rp;
        {
            float* p = row;
            int count = i;

            while (count != 0) {
                d -= *p * *p;
                p++;
                count--;
            }
        }
        if (d <= 0.0f) {
            return 0;
        }
        *rp = SqrtF(d);
        inv[i] = 1.0f / *rp;
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
int SolverRandRange(int range) {
    sRandState = sRandState * 0x19660D + 0x3C6EF35F;
    return (int)(((double)range / 4294967296.0) * (double)sRandState);
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

// u_80279558.cpp: ODE 0.5 quickstep.cpp (0x80279558-0x8027B4F8): dxQuickStepper (fn_80279558, stored by
// dWorldQuickStep) and SOR_LCP (fn_8027A9C0). Evidence for quickstep (not step.cpp): fn_8027A9C0 is the
// SOR LCP iteration (dRandInt constraint shuffle every 8 iterations, warm-start lambda *= 0.9, inlined
// compute_invM_JT/multiply_invM_JT), fn_80279558 calls it and dxStepBody (fn_8027C240).
// Split from u_80277600 on data evidence: own .sdata2 pool 0x804DA340-0x804DA358 (1.0f, -FLT_MAX, FLT_MAX,
// 0.0f, 0.9 double); 1.0f and 0.0f are also pooled by u_80278D2C and u_8027B4F8 (duplicates mean separate files).

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

// TODO (not drafted yet, 0x1468 bytes at 0x80279558)
extern "C" void fn_80279558(void);

// ---- ODE 0.5 types, single precision.
typedef float dReal;
typedef dReal* dRealMutablePtr;
typedef const dReal* dRealPtr;
typedef unsigned long size_t;

extern "C" void* memcpy(void* dst, const void* src, size_t n);
void dSetZero(dReal* a, int n);
int dRandInt(int n);

// common.h: 16-byte aligned stack arrays
#define EFFICIENT_ALIGNMENT 16
#define dEFFICIENT_SIZE(x) ((((x)-1) | (EFFICIENT_ALIGNMENT - 1)) + 1)
#define dALLOCA16(n) ((char*)dEFFICIENT_SIZE(((size_t)(__alloca((n) + (EFFICIENT_ALIGNMENT - 1))))))
#define dRealAllocaArray(name, n) dReal* name = (dReal*)dALLOCA16((n) * sizeof(dReal))

static inline double _fabs(double x) { return __fabs(x); }
#define dFabs(x) ((float)_fabs(x))

static inline dReal dDOT(const dReal* a, const dReal* b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }

static inline void dMULTIPLY0_331(dReal* A, const dReal* B, const dReal* C)
{
    A[0] = dDOT(B, C);
    A[1] = dDOT(B + 4, C);
    A[2] = dDOT(B + 8, C);
}

struct dxBody {
    char pad[0x94];
    dReal invMass;  // 0x94
};

struct dxQuickStepParameters {
    int num_iterations;  // number of SOR iterations to perform
    dReal w;             // the SOR over-relaxation parameter
};

struct IndexError {
    dReal error;  // error to sort on
    int findex;
    int index;  // row index
};

// compute iMJ = inv(M)*J'
static inline void compute_invM_JT(int m, dRealMutablePtr J, dRealMutablePtr iMJ, int* jb, dxBody* const* body,
                                   dRealPtr invI)
{
    int i, j;
    dRealMutablePtr iMJ_ptr = iMJ;
    dRealMutablePtr J_ptr = J;
    for (i = 0; i < m; i++) {
        int b1 = jb[i * 2];
        int b2 = jb[i * 2 + 1];
        dReal k = body[b1]->invMass;
        for (j = 0; j < 3; j++) {
            iMJ_ptr[j] = k * J_ptr[j];
        }
        dMULTIPLY0_331(iMJ_ptr + 3, invI + 12 * b1, J_ptr + 3);
        if (b2 >= 0) {
            k = body[b2]->invMass;
            for (j = 0; j < 3; j++) {
                iMJ_ptr[j + 6] = k * J_ptr[j + 6];
            }
            dMULTIPLY0_331(iMJ_ptr + 9, invI + 12 * b2, J_ptr + 9);
        }
        J_ptr += 12;
        iMJ_ptr += 12;
    }
}

// compute out = inv(M)*J'*in.
static inline void multiply_invM_JT(int m, int nb, dRealMutablePtr iMJ, int* jb, dRealMutablePtr in,
                                    dRealMutablePtr out)
{
    int i, j;
    dSetZero(out, 6 * nb);
    dRealPtr iMJ_ptr = iMJ;
    for (i = 0; i < m; i++) {
        int b1 = jb[i * 2];
        int b2 = jb[i * 2 + 1];
        dRealMutablePtr out_ptr = out + b1 * 6;
        for (j = 0; j < 6; j++) {
            out_ptr[j] += iMJ_ptr[j] * in[i];
        }
        iMJ_ptr += 6;
        if (b2 >= 0) {
            out_ptr = out + b2 * 6;
            for (j = 0; j < 6; j++) {
                out_ptr[j] += iMJ_ptr[j] * in[i];
            }
        }
        iMJ_ptr += 6;
    }
}

// TODO (not drafted yet, 0x1468 bytes at 0x80279558): dxQuickStepper
extern "C" void fn_80279558(void);

// 0x8027A9C0: SOR_LCP. solve the LCP with projected successive over-relaxation (SOR) iterations.
extern "C" void fn_8027A9C0(int m, int nb, dRealMutablePtr J, int* jb, dxBody* const* body, dRealPtr invI,
                            dRealMutablePtr lambda, dRealMutablePtr invMforce, dRealMutablePtr rhs,
                            dRealMutablePtr lo, dRealMutablePtr hi, dRealPtr cfm, int* findex,
                            dxQuickStepParameters* qs)
{
    const int num_iterations = qs->num_iterations;
    const dReal sor_w = qs->w;  // SOR over-relaxation parameter

    int i, j;

    // for warm starting, this seems to be necessary to prevent
    // jerkiness in motor-driven joints. i have no idea why this works.
    for (i = 0; i < m; i++) {
        lambda[i] *= 0.9;
    }

    // the lambda computed at the previous iteration.
    // this is used to measure error for when we are reordering the indexes.
    dRealAllocaArray(last_lambda, m);

    // a copy of the 'hi' vector in case findex[] is being used
    dRealAllocaArray(hicopy, m);
    memcpy(hicopy, hi, m * sizeof(dReal));

    // precompute iMJ = inv(M)*J'
    dRealAllocaArray(iMJ, m * 12);
    compute_invM_JT(m, J, iMJ, jb, body, invI);

    // compute fc=(inv(M)*J')*lambda. we will incrementally maintain fc
    // as we change lambda.
    multiply_invM_JT(m, nb, iMJ, jb, lambda, invMforce);

    // precompute 1 / diagonals of A
    dRealAllocaArray(Ad, m);
    dRealPtr iMJ_ptr = iMJ;
    dRealMutablePtr J_ptr = J;
    for (i = 0; i < m; i++) {
        dReal sum = 0;
        for (j = 0; j < 6; j++) {
            sum += iMJ_ptr[j] * J_ptr[j];
        }
        if (jb[i * 2 + 1] >= 0) {
            for (j = 6; j < 12; j++) {
                sum += iMJ_ptr[j] * J_ptr[j];
            }
        }
        iMJ_ptr += 12;
        J_ptr += 12;
        Ad[i] = sor_w / (sum + cfm[i]);
    }

    // scale J and b by Ad
    J_ptr = J;
    for (i = 0; i < m; i++) {
        for (j = 0; j < 12; j++) {
            J_ptr[0] *= Ad[i];
            J_ptr++;
        }
        rhs[i] *= Ad[i];
    }

    // scale Ad by CFM
    for (i = 0; i < m; i++) {
        Ad[i] *= cfm[i];
    }

    // order to solve constraint rows in
    IndexError* order = (IndexError*)__alloca(m * sizeof(IndexError));

    // make sure constraints with findex < 0 come first.
    j = 0;
    for (i = 0; i < m; i++) {
        if (findex[i] < 0) {
            order[j++].index = i;
        }
    }
    for (i = 0; i < m; i++) {
        if (findex[i] >= 0) {
            order[j++].index = i;
        }
    }

    for (int iteration = 0; iteration < num_iterations; iteration++) {
        if ((iteration & 7) == 0) {
            for (i = 1; i < m; ++i) {
                IndexError tmp = order[i];
                int swapi = dRandInt(i + 1);
                order[i] = order[swapi];
                order[swapi] = tmp;
            }
        }

        // @@@ potential optimization: swap lambda and last_lambda pointers rather
        //     than copying the data. we must make sure lambda is properly
        //     returned to the caller
        memcpy(last_lambda, lambda, m * sizeof(dReal));

        for (int i = 0; i < m; i++) {
            // @@@ potential optimization: we could pre-sort J and iMJ, thereby
            //     linearizing access to those arrays.

            int index = order[i].index;
            J_ptr = J + index * 12;
            iMJ_ptr = iMJ + index * 12;

            // set the limits for this constraint. note that 'hicopy' is used.
            // this is the place where the QuickStep method differs from the
            // direct LCP solving method, since that method only performs this
            // limit adjustment once per time step, whereas this method performs
            // once per iteration per constraint row.
            // the constraints are ordered so that all lambda[] values needed have
            // already been computed.
            if (findex[index] >= 0) {
                hi[index] = dFabs(hicopy[index] * lambda[findex[index]]);
                lo[index] = -hi[index];
            }

            int b1 = jb[index * 2];
            int b2 = jb[index * 2 + 1];
            dReal delta = rhs[index] - lambda[index] * Ad[index];
            dRealMutablePtr fc_ptr = invMforce + 6 * b1;

            // @@@ potential optimization: SIMD-ize this and the b2 >= 0 case
            delta -= fc_ptr[0] * J_ptr[0] + fc_ptr[1] * J_ptr[1] + fc_ptr[2] * J_ptr[2] + fc_ptr[3] * J_ptr[3] +
                     fc_ptr[4] * J_ptr[4] + fc_ptr[5] * J_ptr[5];
            // @@@ potential optimization: handle 1-body constraints in a separate
            //     loop to avoid the cost of test & jump?
            if (b2 >= 0) {
                fc_ptr = invMforce + 6 * b2;
                delta -= fc_ptr[0] * J_ptr[6] + fc_ptr[1] * J_ptr[7] + fc_ptr[2] * J_ptr[8] +
                         fc_ptr[3] * J_ptr[9] + fc_ptr[4] * J_ptr[10] + fc_ptr[5] * J_ptr[11];
            }

            // compute lambda and clamp it to [lo,hi].
            dReal new_lambda = lambda[index] + delta;
            if (new_lambda < lo[index]) {
                delta = lo[index] - lambda[index];
                lambda[index] = lo[index];
            } else if (new_lambda > hi[index]) {
                delta = hi[index] - lambda[index];
                lambda[index] = hi[index];
            } else {
                lambda[index] = new_lambda;
            }

            // update invMforce.
            // @@@ potential optimization: SIMD for this and the b2 >= 0 case
            fc_ptr = invMforce + 6 * b1;
            fc_ptr[0] += delta * iMJ_ptr[0];
            fc_ptr[1] += delta * iMJ_ptr[1];
            fc_ptr[2] += delta * iMJ_ptr[2];
            fc_ptr[3] += delta * iMJ_ptr[3];
            fc_ptr[4] += delta * iMJ_ptr[4];
            fc_ptr[5] += delta * iMJ_ptr[5];
            // @@@ potential optimization: handle 1-body constraints in a separate
            //     loop to avoid the cost of test & jump?
            if (b2 >= 0) {
                fc_ptr = invMforce + 6 * b2;
                fc_ptr[0] += delta * iMJ_ptr[6];
                fc_ptr[1] += delta * iMJ_ptr[7];
                fc_ptr[2] += delta * iMJ_ptr[8];
                fc_ptr[3] += delta * iMJ_ptr[9];
                fc_ptr[4] += delta * iMJ_ptr[10];
                fc_ptr[5] += delta * iMJ_ptr[11];
            }
        }
    }
}

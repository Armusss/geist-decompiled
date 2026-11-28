// u_80279558.cpp: ODE 0.5 quickstep.cpp (0x80279558-0x8027B4F8): dxQuickStepper (fn_80279558, stored by
// dWorldQuickStep) and SOR_LCP (fn_8027A9C0). Evidence for quickstep (not step.cpp): fn_8027A9C0 is the
// SOR LCP iteration (dRandInt constraint shuffle every 8 iterations, warm-start lambda *= 0.9, inlined
// compute_invM_JT/multiply_invM_JT), fn_80279558 calls it and dxStepBody (fn_8027C240).
// Split from u_80277600 on data evidence: own .sdata2 pool 0x804DA340-0x804DA358 (1.0f, -FLT_MAX, FLT_MAX,
// 0.0f, 0.9 double); 1.0f and 0.0f are also pooled by u_80278D2C and u_8027B4F8 (duplicates mean separate files).
// Built with -inline deferred (user-approved 2026-11-27) and GC/2.6 (2026-11-28; fn_80279558 exact only on 2.5-2.7): SOR_LCP precedes dxQuickStepper as in
// ODE and the two come out reversed, as in retail.

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;


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

static inline dReal dDOT14(const dReal* a, const dReal* b) { return a[0] * b[0] + a[1] * b[4] + a[2] * b[8]; }

static inline void dMULTIPLY0_331(dReal* A, const dReal* B, const dReal* C)
{
    A[0] = dDOT(B, C);
    A[1] = dDOT(B + 4, C);
    A[2] = dDOT(B + 8, C);
}

static inline void dMULTIPLYADD0_331(dReal* A, const dReal* B, const dReal* C)
{
    A[0] += dDOT(B, C);
    A[1] += dDOT(B + 4, C);
    A[2] += dDOT(B + 8, C);
}

static inline void dMULTIPLY0_333(dReal* A, const dReal* B, const dReal* C)
{
    A[0] = dDOT14(B, C);
    A[1] = dDOT14(B, C + 1);
    A[2] = dDOT14(B, C + 2);
    A[4] = dDOT14(B + 4, C);
    A[5] = dDOT14(B + 4, C + 1);
    A[6] = dDOT14(B + 4, C + 2);
    A[8] = dDOT14(B + 8, C);
    A[9] = dDOT14(B + 8, C + 1);
    A[10] = dDOT14(B + 8, C + 2);
}

static inline void dMULTIPLY2_333(dReal* A, const dReal* B, const dReal* C)
{
    A[0] = dDOT(B, C);
    A[1] = dDOT(B, C + 4);
    A[2] = dDOT(B, C + 8);
    A[4] = dDOT(B + 4, C);
    A[5] = dDOT(B + 4, C + 4);
    A[6] = dDOT(B + 4, C + 8);
    A[8] = dDOT(B + 8, C);
    A[9] = dDOT(B + 8, C + 4);
    A[10] = dDOT(B + 8, C + 8);
}

#define dCROSS(a, op, b, c)                       \
    (a)[0] op((b)[1] * (c)[2] - (b)[2] * (c)[1]); \
    (a)[1] op((b)[2] * (c)[0] - (b)[0] * (c)[2]); \
    (a)[2] op((b)[0] * (c)[1] - (b)[1] * (c)[0]);

typedef dReal dVector3[4];
typedef dReal dMatrix3[4 * 3];
typedef dReal dQuaternion[4];

#define dInfinity 3.4028235e+38f
#define dRecip(x) ((float)(1.0f / (x)))

void dSetValue(dReal* a, int n, dReal value);

struct dxWorld;
struct dxJoint;
struct dxGeom;
struct dxJointNode {
    dxJoint* joint;
    struct dxBody* body;
    dxJointNode* next;
};

struct dMass {
    dReal mass;     // 0x20 in dxBody
    dReal c[4];
    dMatrix3 I;     // 0x34 in dxBody
};

struct dxAutoDisable {
    dReal linear_threshold;
    dReal angular_threshold;
    dReal idle_time;
    int idle_steps;
};

struct dxBody {
    dxWorld* world;            // 0x00
    void* next;                // 0x04
    void** tome;               // 0x08
    void* userdata;            // 0x0C
    int tag;                   // 0x10
    dxJointNode* firstjoint;   // 0x14
    int flags;                 // 0x18
    dxGeom* geom;              // 0x1C
    dMass mass;                // 0x20
    dMatrix3 invI;             // 0x64
    dReal invMass;             // 0x94
    dVector3 pos;              // 0x98
    dQuaternion q;             // 0xA8
    dMatrix3 R;                // 0xB8
    dVector3 lvel, avel;       // 0xE8
    dVector3 facc, tacc;       // 0x108
    dVector3 finite_rot_axis;  // 0x128
    dxAutoDisable adis;        // 0x138
    dReal adis_timeleft;       // 0x148
    int adis_stepsleft;        // 0x14C
};

enum { dxBodyNoGravity = 8 };

struct dJointFeedback {
    dVector3 f1;  // force applied to body 1
    dVector3 t1;  // torque applied to body 1
    dVector3 f2;  // force applied to body 2
    dVector3 t2;  // torque applied to body 2
};

struct dxJoint {
    struct Info1 {
        int m, nub;
    };
    struct Info2 {
        dReal fps, erp;
        dReal *J1l, *J1a, *J2l, *J2a;
        int rowskip;
        dReal *c, *cfm;
        dReal *lo, *hi;
        int* findex;
    };
    struct Vtable {
        int size;
        void (*init)(dxJoint* joint);
        void (*getInfo1)(dxJoint* joint, Info1* info);
        void (*getInfo2)(dxJoint* joint, Info2* info);
        int typenum;
    };

    dxWorld* world;           // 0x00
    void* next;               // 0x04
    void** tome;              // 0x08
    void* userdata;           // 0x0C
    int tag;                  // 0x10
    Vtable* vtable;           // 0x14
    int flags;                // 0x18
    dxJointNode node[2];      // 0x1C
    dJointFeedback* feedback; // 0x34
    dReal lambda[6];          // 0x38
};

struct dxQuickStepParameters {
    int num_iterations;  // number of SOR iterations to perform
    dReal w;             // the SOR over-relaxation parameter
};

struct dxContactParameters {
    dReal max_vel;
    dReal min_depth;
};

struct dxWorld {
    dxBody* firstbody;
    dxJoint* firstjoint;
    int nb, nj;
    dVector3 gravity;  // 0x10
    dReal global_erp;  // 0x20
    dReal global_cfm;  // 0x24
    dxAutoDisable adis;
    int adis_flag;
    dxQuickStepParameters qs;  // 0x3C
    dxContactParameters contactp;
};

extern "C" void fn_8027C240(dxBody* b, dReal h);  // dxStepBody (moveAndRotateBody), util.cpp

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

// compute out = J*in.
static inline void multiply_J(int m, dRealMutablePtr J, int* jb, dRealMutablePtr in, dRealMutablePtr out)
{
    int i, j;
    dRealPtr J_ptr = J;
    for (i = 0; i < m; i++) {
        int b1 = jb[i * 2];
        int b2 = jb[i * 2 + 1];
        dReal sum = 0;
        dRealMutablePtr in_ptr = in + b1 * 6;
        for (j = 0; j < 6; j++) {
            sum += J_ptr[j] * in_ptr[j];
        }
        J_ptr += 6;
        if (b2 >= 0) {
            in_ptr = in + b2 * 6;
            for (j = 0; j < 6; j++) {
                sum += J_ptr[j] * in_ptr[j];
            }
        }
        J_ptr += 6;
        out[i] = sum;
    }
}

#define SOR_LCP fn_8027A9C0

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

// 0x80279558: dxQuickStepper. one QuickStep (iterative LCP) step of the island body[0..nb-1],
// joint[0..nj-1].
extern "C" void fn_80279558(dxWorld* world, dxBody* const* body, int nb, dxJoint* const* _joint, int nj,
                            dReal stepsize)
{
    int i, j;

    dReal stepsize1 = dRecip(stepsize);

    // number all bodies in the body list - set their tag values
    for (i = 0; i < nb; i++) {
        body[i]->tag = i;
    }

    // make a local copy of the joint array, because we might want to modify it.
    // (the "dxJoint *const*" declaration says we're allowed to modify the joints
    // but not the joint array, because the caller might need it unchanged).
    //@@@ do we really need to do this? we'll be sorting constraint rows individually, not joints
    dxJoint** joint = (dxJoint**)__alloca(nj * sizeof(dxJoint*));
    memcpy(joint, _joint, nj * sizeof(dxJoint*));

    // for all bodies, compute the inertia tensor and its inverse in the global
    // frame, and compute the rotational force and add it to the torque
    // accumulator. I and invI are a vertical stack of 3x4 matrices, one per body.
    dRealAllocaArray(I, 3 * 4 * nb);  // need to remember all I's for feedback purposes only
    dRealAllocaArray(invI, 3 * 4 * nb);
    for (i = 0; i < nb; i++) {
        dMatrix3 tmp;
        // compute inertia tensor in global frame
        dMULTIPLY2_333(tmp, body[i]->mass.I, body[i]->R);
        dMULTIPLY0_333(I + i * 12, body[i]->R, tmp);
        // compute inverse inertia tensor in global frame
        dMULTIPLY2_333(tmp, body[i]->invI, body[i]->R);
        dMULTIPLY0_333(invI + i * 12, body[i]->R, tmp);
        // compute rotational force
        dMULTIPLY0_331(tmp, I + i * 12, body[i]->avel);
        dCROSS(body[i]->tacc, -=, body[i]->avel, tmp);
    }

    // add the gravity force to all bodies
    for (i = 0; i < nb; i++) {
        if ((body[i]->flags & dxBodyNoGravity) == 0) {
            body[i]->facc[0] += body[i]->mass.mass * world->gravity[0];
            body[i]->facc[1] += body[i]->mass.mass * world->gravity[1];
            body[i]->facc[2] += body[i]->mass.mass * world->gravity[2];
        }
    }

    // get joint information (m = total constraint dimension, nub = number of unbounded variables).
    // joints with m=0 are inactive and are removed from the joints array
    // entirely, so that the code that follows does not consider them.
    //@@@ do we really need to save all the info1's
    dxJoint::Info1* info = (dxJoint::Info1*)__alloca(nj * sizeof(dxJoint::Info1));
    for (i = 0, j = 0; j < nj; j++) {  // i=dest, j=src
        joint[j]->vtable->getInfo1(joint[j], info + i);
        if (info[i].m > 0) {
            joint[i] = joint[j];
            i++;
        }
    }
    nj = i;

    // create the row offset array
    int m = 0;
    int* ofs = (int*)__alloca(nj * sizeof(int));
    for (i = 0; i < nj; i++) {
        ofs[i] = m;
        m += info[i].m;
    }

    // if there are constraints, compute the constraint force
    dRealAllocaArray(J, m * 12);
    int* jb = (int*)__alloca(m * 2 * sizeof(int));
    if (m > 0) {
        // create a constraint equation right hand side vector `c', a constraint
        // force mixing vector `cfm', and LCP low and high bound vectors, and an
        // 'findex' vector.
        dRealAllocaArray(c, m);
        dRealAllocaArray(cfm, m);
        dRealAllocaArray(lo, m);
        dRealAllocaArray(hi, m);
        int* findex = (int*)__alloca(m * sizeof(int));
        dSetZero(c, m);
        dSetValue(cfm, m, world->global_cfm);
        dSetValue(lo, m, -dInfinity);
        dSetValue(hi, m, dInfinity);
        for (i = 0; i < m; i++) {
            findex[i] = -1;
        }

        // get jacobian data from constraints. an m*12 matrix will be created
        // to store the two jacobian blocks from each constraint. it has this
        // format:
        //
        //   l1 l1 l1 a1 a1 a1 l2 l2 l2 a2 a2 a2 \    .
        //   l1 l1 l1 a1 a1 a1 l2 l2 l2 a2 a2 a2  }-- jacobian for joint 0, body 1 and body 2 (3 rows)
        //   l1 l1 l1 a1 a1 a1 l2 l2 l2 a2 a2 a2 /
        //   l1 l1 l1 a1 a1 a1 l2 l2 l2 a2 a2 a2 }--- jacobian for joint 1, body 1 and body 2 (3 rows)
        //   etc...
        //
        //   (lll) = linear jacobian data
        //   (aaa) = angular jacobian data
        //
        dSetZero(J, m * 12);
        dxJoint::Info2 Jinfo;
        Jinfo.rowskip = 12;
        Jinfo.fps = stepsize1;
        Jinfo.erp = world->global_erp;
        for (i = 0; i < nj; i++) {
            Jinfo.J1l = J + ofs[i] * 12;
            Jinfo.J1a = Jinfo.J1l + 3;
            Jinfo.J2l = Jinfo.J1l + 6;
            Jinfo.J2a = Jinfo.J1l + 9;
            Jinfo.c = c + ofs[i];
            Jinfo.cfm = cfm + ofs[i];
            Jinfo.lo = lo + ofs[i];
            Jinfo.hi = hi + ofs[i];
            Jinfo.findex = findex + ofs[i];
            joint[i]->vtable->getInfo2(joint[i], &Jinfo);
            // adjust returned findex values for global index numbering
            for (j = 0; j < info[i].m; j++) {
                if (findex[ofs[i] + j] >= 0) {
                    findex[ofs[i] + j] += ofs[i];
                }
            }
        }

        // create an array of body numbers for each joint row
        int* jb_ptr = jb;
        for (i = 0; i < nj; i++) {
            int b1 = (joint[i]->node[0].body) ? (joint[i]->node[0].body->tag) : -1;
            int b2 = (joint[i]->node[1].body) ? (joint[i]->node[1].body->tag) : -1;
            for (j = 0; j < info[i].m; j++) {
                jb_ptr[0] = b1;
                jb_ptr[1] = b2;
                jb_ptr += 2;
            }
        }

        // compute the right hand side `rhs'
        dRealAllocaArray(tmp1, nb * 6);
        // put v/h + invM*fe into tmp1
        for (i = 0; i < nb; i++) {
            dReal body_invMass = body[i]->invMass;
            for (j = 0; j < 3; j++) {
                tmp1[i * 6 + j] = body[i]->facc[j] * body_invMass + body[i]->lvel[j] * stepsize1;
            }
            dMULTIPLY0_331(tmp1 + i * 6 + 3, invI + i * 12, body[i]->tacc);
            for (j = 0; j < 3; j++) {
                tmp1[i * 6 + 3 + j] += body[i]->avel[j] * stepsize1;
            }
        }

        // put J*tmp1 into rhs
        dRealAllocaArray(rhs, m);
        multiply_J(m, J, jb, tmp1, rhs);

        // complete rhs
        for (i = 0; i < m; i++) {
            rhs[i] = c[i] * stepsize1 - rhs[i];
        }

        // scale CFM
        for (i = 0; i < m; i++) {
            cfm[i] *= stepsize1;
        }

        // load lambda from the value saved on the previous iteration
        dRealAllocaArray(lambda, m);
        dSetZero(lambda, m);  //@@@ shouldn't be necessary
        for (i = 0; i < nj; i++) {
            memcpy(lambda + ofs[i], joint[i]->lambda, info[i].m * sizeof(dReal));
        }

        // solve the LCP problem and get lambda and invM*constraint_force
        dRealAllocaArray(cforce, nb * 6);
        SOR_LCP(m, nb, J, jb, body, invI, lambda, cforce, rhs, lo, hi, cfm, findex, &world->qs);

        // save lambda for the next iteration
        //@@@ note that this doesn't work for contact joints yet, as they are
        // recreated every iteration
        for (i = 0; i < nj; i++) {
            memcpy(joint[i]->lambda, lambda + ofs[i], info[i].m * sizeof(dReal));
        }

        // note that the SOR method overwrites rhs and J at this point, so
        // they should not be used again.

        // add stepsize * cforce to the body velocity
        for (i = 0; i < nb; i++) {
            for (j = 0; j < 3; j++) {
                body[i]->lvel[j] += stepsize * cforce[i * 6 + j];
            }
            for (j = 0; j < 3; j++) {
                body[i]->avel[j] += stepsize * cforce[i * 6 + 3 + j];
            }
        }

        // if joint feedback is requested, compute the constraint force.
        // BUT: cforce is inv(M)*J'*lambda, whereas we want just J'*lambda,
        // so we must compute M*cforce.
        // @@@ if any joint has a feedback request we compute the entire
        //     adjusted cforce, which is not the most efficient way to do it.
        for (j = 0; j < nj; j++) {
            if (joint[j]->feedback) {
                // compute adjusted cforce
                for (i = 0; i < nb; i++) {
                    dReal k = body[i]->mass.mass;
                    cforce[i * 6 + 0] *= k;
                    cforce[i * 6 + 1] *= k;
                    cforce[i * 6 + 2] *= k;
                    dVector3 tmp;
                    dMULTIPLY0_331(tmp, I + 12 * i, cforce + i * 6 + 3);
                    cforce[i * 6 + 3] = tmp[0];
                    cforce[i * 6 + 4] = tmp[1];
                    cforce[i * 6 + 5] = tmp[2];
                }
                // compute feedback for this and all remaining joints
                for (; j < nj; j++) {
                    dJointFeedback* fb = joint[j]->feedback;
                    if (fb) {
                        int b1 = joint[j]->node[0].body->tag;
                        memcpy(fb->f1, cforce + b1 * 6, 3 * sizeof(dReal));
                        memcpy(fb->t1, cforce + b1 * 6 + 3, 3 * sizeof(dReal));
                        if (joint[j]->node[1].body) {
                            int b2 = joint[j]->node[1].body->tag;
                            memcpy(fb->f2, cforce + b2 * 6, 3 * sizeof(dReal));
                            memcpy(fb->t2, cforce + b2 * 6 + 3, 3 * sizeof(dReal));
                        }
                    }
                }
            }
        }
    }

    // compute the velocity update:
    // add stepsize * invM * fe to the body velocity
    for (i = 0; i < nb; i++) {
        dReal body_invMass = body[i]->invMass;
        for (j = 0; j < 3; j++) {
            body[i]->lvel[j] += stepsize * body_invMass * body[i]->facc[j];
        }
        for (j = 0; j < 3; j++) {
            body[i]->tacc[j] *= stepsize;
        }
        dMULTIPLYADD0_331(body[i]->avel, invI + i * 12, body[i]->tacc);
    }

    // update the position and orientation from the new linear/angular velocity
    // (over the given timestep)
    for (i = 0; i < nb; i++) {
        fn_8027C240(body[i], stepsize);
    }

    // zero all force accumulators
    for (i = 0; i < nb; i++) {
        dSetZero(body[i]->facc, 3);
        dSetZero(body[i]->tacc, 3);
    }
}

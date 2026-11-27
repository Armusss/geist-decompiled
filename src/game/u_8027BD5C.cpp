// u_8027BD5C.cpp: ODE 0.5 util.cpp (0x8027BD5C-0x8027C650): dxProcessIslands (fn_8027BD5C, called by dWorldStep,
// with autoDisableBodies inlined) and dxStepBody (fn_8027C240: moves and rotates a body, sinc() term).
// Split from u_80277600 on data evidence: own .sdata2 pool 0x804DA388-0x804DA3B8 (0.0f, 0.5f, 1e-4 double,
// 1.0f, 1/6, sqrt constants 0.5/3.0 doubles, 0.0 double), again duplicating the other ODE files' constants.
// In the ODE source dxStepBody comes first; retail has it second, which -inline deferred (the ODE library
// setting, see configure.py) would give from source order. Here the functions are in address order.
// Both functions match (2/2). Not flippable yet: retail pools 1e-4 before 1.0f/(1/6) (sinc's textual order); every
// variant on GC/1.3.2/2.0/2.6 (also -inline deferred, ternary, local, REAL as cast) pools 1.0f, 1/6 first.
// Only GC/1.2.5n gives the retail pool order, with wrong code.

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

// ---- ODE 0.5 types (objects.h, mass.h), single precision; only what util.cpp uses.
typedef float dReal;
typedef dReal dVector3[4];
typedef dReal dVector4[4];
typedef dReal dMatrix3[4 * 3];
typedef dReal dQuaternion[4];

#define REAL(x) (x##f)

struct dxWorld;
struct dxBody;
struct dxJoint;
struct dxGeom;
struct Geom;

struct dObject {
    dxWorld* world;     // 0x00
    dObject* next;      // 0x04
    dObject** tome;     // 0x08
    void* userdata;     // 0x0C
    int tag;            // 0x10
};

struct dxJointNode {
    dxJoint* joint;     // 0x00
    dxBody* body;       // 0x04
    dxJointNode* next;  // 0x08
};

struct dxAutoDisable {
    dReal linear_threshold;
    dReal angular_threshold;
    dReal idle_time;
    int idle_steps;
};

struct dMass {
    dReal mass;
    dVector4 c;
    dMatrix3 I;
};

enum {
    dxBodyFlagFiniteRotation = 1,
    dxBodyFlagFiniteRotationAxis = 2,
    dxBodyDisabled = 4,
    dxBodyNoGravity = 8,
    dxBodyAutoDisable = 16
};

struct dxBody : public dObject {
    dxJointNode* firstjoint;   // 0x14
    int flags;                 // 0x18
    Geom* geom;                // 0x1C
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

struct dxJoint : public dObject {
};

struct dxWorld {
    dxBody* firstbody;   // 0x00
    dxJoint* firstjoint; // 0x04
    int nb, nj;          // 0x08
};

typedef void dstepper_fn_t(dxWorld* world, dxBody* const* body, int nb, dxJoint* const* joint, int nj,
                           dReal stepsize);

static inline dReal dDOT(const dReal* a, const dReal* b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

extern "C" {
double cos(double x);
double sin(double x);
void fn_80278FE0(dReal* a);                                           // dNormalize4 (odemath.cpp)
void fn_8027B4F8(dReal dq[4], const dVector3 w, const dQuaternion q); // dWtoDQ (rotation.cpp)
void fn_8027BAA0(dMatrix3 R, const dQuaternion q);                    // dQtoR (rotation.cpp)
void fn_8027BCAC(dQuaternion qa, const dQuaternion qb, const dQuaternion qc); // dQMultiply0 (rotation.cpp)
void fn_8026D3A0(Geom* g);                                            // dGeomMoved (collision_space.cpp)
}
Geom* GeomGetNext(Geom* g);                                           // dGeomGetBodyNext (collision_kernel.cpp)

extern int __float_nan[];

static inline double _fabs(double x) { return __fabs(x); }
static inline float _fabsf(float x) { return (float)_fabs((double)x); }
#define dFabs(x) _fabsf(x)
#define dCos(x) ((float)cos(x))
#define dSin(x) ((float)sin(x))

// The game's MSL sqrtf (inline in its math.h), as in ode/matrix.cpp.
static inline int _dFpClassifyf(float x)
{
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

static inline float _dSqrtf(float x)
{
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
#define dSqrt(x) _dSqrtf(x)

static inline dReal sinc(dReal x)
{
    if (dFabs(x) < 1.0e-4) {
        return REAL(1.0) - x * x * REAL(0.166666666666666666667);
    } else {
        return dSin(x) / x;
    }
}

// The C library's alloca, rounded up to 16 bytes (dALLOCA16).
#define ALLOCA(n) ((char*)((((u32)__alloca((n) + 15)) - 1 | 15) + 1))

static inline void autoDisableBodies(dxWorld* world, dReal stepsize)
{
    dxBody* bb;

    for (bb = world->firstbody; bb; bb = (dxBody*)bb->next) {
        if ((bb->flags & (dxBodyAutoDisable | dxBodyDisabled)) != dxBodyAutoDisable) {
            continue;
        }
        int idle = 1;
        dReal lspeed2 = dDOT(bb->lvel, bb->lvel);
        if (lspeed2 > bb->adis.linear_threshold) {
            idle = 0;
        } else {
            dReal aspeed = dDOT(bb->avel, bb->avel);
            if (aspeed > bb->adis.angular_threshold) {
                idle = 0;
            }
        }
        if (idle) {
            bb->adis_stepsleft--;
            bb->adis_timeleft -= stepsize;
        } else {
            bb->adis_stepsleft = bb->adis.idle_steps;
            bb->adis_timeleft = bb->adis.idle_time;
        }
        if (bb->adis_stepsleft < 0 && bb->adis_timeleft < 0) {
            bb->flags |= dxBodyDisabled;
        }
    }
}

// 0x8027BD5C
extern "C" void fn_8027BD5C(dxWorld* world, dReal stepsize, dstepper_fn_t* stepper)
{
    dxBody *b, *bb, **body;
    dxJoint *j, **joint;

    if (world->nb <= 0) {
        return;
    }
    autoDisableBodies(world, stepsize);

    body = (dxBody**)ALLOCA(world->nb * sizeof(dxBody*));
    joint = (dxJoint**)ALLOCA(world->nj * sizeof(dxJoint*));
    int bcount = 0;
    int jcount = 0;

    for (b = world->firstbody; b; b = (dxBody*)b->next) {
        b->tag = 0;
    }
    for (j = world->firstjoint; j; j = (dxJoint*)j->next) {
        j->tag = 0;
    }

    int stackalloc = (world->nj < world->nb) ? world->nj : world->nb;
    dxBody** stack = (dxBody**)ALLOCA(stackalloc * sizeof(dxBody*));

    for (bb = world->firstbody; bb; bb = (dxBody*)bb->next) {
        if (bb->tag || (bb->flags & dxBodyDisabled)) {
            continue;
        }
        bb->tag = 1;

        int stacksize = 0;
        b = bb;
        body[0] = bb;
        bcount = 1;
        jcount = 0;
        goto quickstart;
        while (stacksize > 0) {
            b = stack[--stacksize];
            body[bcount++] = b;
        quickstart:
            for (dxJointNode* n = b->firstjoint; n; n = n->next) {
                if (!n->joint->tag) {
                    n->joint->tag = 1;
                    joint[jcount++] = n->joint;
                    if (n->body && !n->body->tag) {
                        n->body->tag = 1;
                        stack[stacksize++] = n->body;
                    }
                }
            }
        }

        stepper(world, body, bcount, joint, jcount, stepsize);

        int i;
        for (i = 0; i < bcount; i++) {
            body[i]->tag = 1;
            body[i]->flags &= ~dxBodyDisabled;
        }
        for (i = 0; i < jcount; i++) {
            joint[i]->tag = 1;
        }
    }
}

// 0x8027C240
extern "C" void fn_8027C240(dxBody* b, dReal h)
{
    int j;

    for (j = 0; j < 3; j++) {
        b->pos[j] += h * b->lvel[j];
    }

    if (b->flags & dxBodyFlagFiniteRotation) {
        dVector3 irv;
        dQuaternion q;

        if (b->flags & dxBodyFlagFiniteRotationAxis) {
            dVector3 frv;
            dReal k = dDOT(b->finite_rot_axis, b->avel);
            frv[0] = b->finite_rot_axis[0] * k;
            frv[1] = b->finite_rot_axis[1] * k;
            frv[2] = b->finite_rot_axis[2] * k;
            irv[0] = b->avel[0] - frv[0];
            irv[1] = b->avel[1] - frv[1];
            irv[2] = b->avel[2] - frv[2];

            h *= REAL(0.5);
            dReal theta = k * h;
            q[0] = dCos(theta);
            dReal s = sinc(theta) * h;
            q[1] = frv[0] * s;
            q[2] = frv[1] * s;
            q[3] = frv[2] * s;
        } else {
            dReal wlen = dSqrt(b->avel[0] * b->avel[0] + b->avel[1] * b->avel[1] + b->avel[2] * b->avel[2]);
            h *= REAL(0.5);
            dReal theta = wlen * h;
            q[0] = dCos(theta);
            dReal s = sinc(theta) * h;
            q[1] = b->avel[0] * s;
            q[2] = b->avel[1] * s;
            q[3] = b->avel[2] * s;
        }

        dQuaternion q2;
        fn_8027BCAC(q2, q, b->q);
        for (j = 0; j < 4; j++) {
            b->q[j] = q2[j];
        }

        if (b->flags & dxBodyFlagFiniteRotationAxis) {
            dReal dq[4];
            fn_8027B4F8(dq, irv, b->q);
            for (j = 0; j < 4; j++) {
                b->q[j] += h * dq[j];
            }
        }
    } else {
        dReal dq[4];
        fn_8027B4F8(dq, b->avel, b->q);
        for (j = 0; j < 4; j++) {
            b->q[j] += h * dq[j];
        }
    }

    fn_80278FE0(b->q);
    fn_8027BAA0(b->R, b->q);

    for (Geom* geom = b->geom; geom; geom = GeomGetNext(geom)) {
        fn_8026D3A0(geom);
    }
}

// u_80273650.cpp (0x80273650-0x80276090): ODE joint library (joint parameter switch tables
// jumptable_803C0528/054C). Split from u_8026C200.
// Evidence: own .sdata2 pool 0x804DA258-0x804DA298 (0.0f, 1.0f, -FLT_MAX/FLT_MAX, pi, 0.5/3.0 doubles again).
// fn_80273650 opens it: it calls fn_80273B7C and sets joint fields; the error file before it has no such code.
// This is ODE 0.5 joint.cpp (the used subset: ball, hinge, contact and angular-motor joints, the
// dxJointLimitMotor helpers). The functions sit in reverse ODE source order (addLimot, the first in
// joint.cpp, is last), like the other ODE files built with -inline deferred; setBall is the exception.
// Function names are the ODE ones in comments; symbols keep their placeholder names.

typedef unsigned long u32;
typedef unsigned long size_t;

// ---- ODE 0.5 types (objects.h, joint.h, contact.h, odemath.h), single precision.
typedef float dReal;
typedef dReal dVector3[4];
typedef dReal dVector4[4];
typedef dReal dMatrix3[4 * 3];
typedef dReal dQuaternion[4];

#define REAL(x) (x##f)
#define dInfinity 3.4028235e+38f
#define M_PI REAL(3.1415926535897932384626433832795029)
#define dRecip(x) ((float)(1.0f / (x)))

struct dxWorld;
struct dxBody;
struct dxJoint;
struct dxGeom;

void dSetZero(dReal* a, int n);
void dBodyAddTorque(dxBody* b, dReal fx, dReal fy, dReal fz);
void dBodyAddForce(dxBody* b, dReal fx, dReal fy, dReal fz);
extern "C" void fn_8027914C(dReal* a);                                         // dNormalize3
extern "C" void fn_80278D2C(const dVector3 n, dVector3 p, dVector3 q);          // dPlaneSpace
extern "C" void fn_8027BB48(dQuaternion qa, const dQuaternion qb, const dQuaternion qc);  // dQMultiply3
extern "C" void fn_8027BBC0(dQuaternion qa, const dQuaternion qb, const dQuaternion qc);  // dQMultiply2
extern "C" void fn_8027BC38(dQuaternion qa, const dQuaternion qb, const dQuaternion qc);  // dQMultiply1
extern "C" double atan2(double y, double x);

#define dNormalize3 fn_8027914C
#define dPlaneSpace fn_80278D2C
#define dQMultiply1 fn_8027BC38
#define dQMultiply2 fn_8027BBC0
#define dQMultiply3 fn_8027BB48
#define dAtan2(y, x) ((float)atan2((y), (x)))

extern int __float_nan[];

// Classify a float the way the standard fpclassify macro does: 1 NaN, 2 inf,
// 3 zero, 5 subnormal, 4 anything else.
static inline int FpClassifyF(float x)
{
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

// Single-precision square root (the MSL sqrtf inline, ODE's dSqrt).
static inline float dSqrt(float x)
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
    } else if (FpClassifyF(x) == 1) {
        return *(float*)__float_nan;
    }
    return x;
}

// odemath.h
static inline dReal dDOT(const dReal* a, const dReal* b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
static inline dReal dDOT14(const dReal* a, const dReal* b) { return a[0] * b[0] + a[1] * b[4] + a[2] * b[8]; }
static inline dReal dDOT41(const dReal* a, const dReal* b) { return a[0] * b[0] + a[4] * b[1] + a[8] * b[2]; }

#define dCROSS(a, op, b, c)                           \
    (a)[0] op((b)[1] * (c)[2] - (b)[2] * (c)[1]);     \
    (a)[1] op((b)[2] * (c)[0] - (b)[0] * (c)[2]);     \
    (a)[2] op((b)[0] * (c)[1] - (b)[1] * (c)[0]);

#define dCROSSMAT(A, a, skip, plus, minus) \
    (A)[1] = minus(a)[2];                  \
    (A)[2] = plus(a)[1];                   \
    (A)[(skip) + 0] = plus(a)[2];          \
    (A)[(skip) + 2] = minus(a)[0];         \
    (A)[2 * (skip) + 0] = minus(a)[1];     \
    (A)[2 * (skip) + 1] = plus(a)[0];

static inline void dMULTIPLY0_331(dReal* A, const dReal* B, const dReal* C)
{
    A[0] = dDOT(B, C);
    A[1] = dDOT(B + 4, C);
    A[2] = dDOT(B + 8, C);
}

static inline void dMULTIPLY1_331(dReal* A, const dReal* B, const dReal* C)
{
    A[0] = dDOT41(B, C);
    A[1] = dDOT41(B + 1, C);
    A[2] = dDOT41(B + 2, C);
}

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

struct dxBody : public dObject {
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

struct dxQuickStepParameters {
    int num_iterations;
    dReal w;
};

struct dxContactParameters {
    dReal max_vel;
    dReal min_depth;
};

struct dxWorld {
    dxBody* firstbody;
    dxJoint* firstjoint;
    int nb, nj;
    dVector3 gravity;
    dReal global_erp;  // 0x20
    dReal global_cfm;  // 0x24
    dxAutoDisable adis;
    int adis_flag;
    dxQuickStepParameters qs;
    dxContactParameters contactp;
};

struct dJointFeedback;

struct dxJoint : public dObject {
    // naming convention: the "first" body this is connected to is node[0].body,
    // and the "second" body is node[1].body. if this joint is only connected
    // to one body then the second body is 0.

    // info returned by getInfo1 function. the constraint dimension is m (<=6).
    // i.e. that is the total number of rows in the jacobian. `nub' is the
    // number of unbounded variables (which have lo,hi = -/+ infinity).
    struct Info1 {
        int m, nub;
    };

    // info returned by getInfo2 function
    struct Info2 {
        // integrator parameters: frames per second (1/stepsize), default error
        // reduction parameter (0..1).
        dReal fps, erp;

        // for the first and second body, pointers to two (linear and angular)
        // n*3 jacobian sub matrices, stored by rows. these matrices will have
        // been initialized to 0 on entry. if the second body is zero then the
        // J2xx pointers may be 0.
        dReal *J1l, *J1a, *J2l, *J2a;

        // elements to jump from one row to the next in J's
        int rowskip;

        // right hand sides of the equation J*v = c + cfm * lambda. cfm is the
        // "constraint force mixing" vector. c is set to zero on entry, cfm is
        // set to a constant value (typically very small or zero) value on entry.
        dReal *c, *cfm;

        // lo and hi limits for variables (set to -/+ infinity on entry).
        dReal *lo, *hi;

        // findex vector for variables. see the LCP solver interface for a
        // description of what this does. this is set to -1 on entry.
        // note that the returned indexes are relative to the first index of
        // the constraint.
        int* findex;
    };

    struct Vtable {
        int size;
        void* init;
        void* getInfo1;
        void* getInfo2;
        int typenum;
    };

    Vtable* vtable;           // 0x14
    int flags;                // 0x18
    dxJointNode node[2];      // 0x1C
    dJointFeedback* feedback; // 0x34
    dReal lambda[6];          // 0x38
};

enum {
    dJOINT_INGROUP = 1,
    dJOINT_REVERSE = 2,
    dJOINT_TWOBODIES = 4
};

enum {
    dParamLoStop = 0,
    dParamHiStop,
    dParamVel,
    dParamFMax,
    dParamFudgeFactor,
    dParamBounce,
    dParamCFM,
    dParamStopERP,
    dParamStopCFM
};

enum {
    dAMotorUser = 0,
    dAMotorEuler = 1
};

enum {
    dContactMu2 = 0x001,
    dContactFDir1 = 0x002,
    dContactBounce = 0x004,
    dContactSoftERP = 0x008,
    dContactSoftCFM = 0x010,
    dContactMotion1 = 0x020,
    dContactMotion2 = 0x040,
    dContactSlip1 = 0x080,
    dContactSlip2 = 0x100,
    dContactApprox1_1 = 0x1000,
    dContactApprox1_2 = 0x2000
};

// contact.h
struct dSurfaceParameters {
    int mode;
    dReal mu;
    dReal mu2;
    dReal bounce;
    dReal bounce_vel;
    dReal soft_erp;
    dReal soft_cfm;
    dReal motion1, motion2;
    dReal slip1, slip2;
};

struct dContactGeom {
    dVector3 pos;
    dVector3 normal;
    dReal depth;
    dxGeom* g1, *g2;
};

struct dContact {
    dSurfaceParameters surface;
    dContactGeom geom;
    dVector3 fdir1;
};

// common limit and motor information for a single joint axis of movement
struct dxJointLimitMotor {
    dReal vel, fmax;        // powered joint: velocity, max force
    dReal lostop, histop;   // joint limits, relative to initial position
    dReal fudge_factor;     // when powering away from joint limits
    dReal normal_cfm;       // cfm to use when not at a stop
    dReal stop_erp, stop_cfm;  // erp and cfm for when at joint limit
    dReal bounce;           // restitution factor
    // variables used between getInfo1() and getInfo2()
    int limit;              // 0=free, 1=at lo limit, 2=at hi limit
    dReal limit_err;        // if at limit, amount over limit

    void init(dxWorld*);
    void set(int num, dReal value);
    int testRotationalLimit(dReal angle);
    int addLimot(dxJoint* joint, dxJoint::Info2* info, int row, dVector3 ax1, int rotational);
};

// ball and socket
struct dxJointBall : public dxJoint {
    dVector3 anchor1;  // anchor w.r.t first body
    dVector3 anchor2;  // anchor w.r.t second body
};

// hinge
struct dxJointHinge : public dxJoint {
    dVector3 anchor1;         // anchor w.r.t first body
    dVector3 anchor2;         // anchor w.r.t second body
    dVector3 axis1;           // axis w.r.t first body
    dVector3 axis2;           // axis w.r.t second body
    dQuaternion qrel;         // initial relative rotation body1 -> body2
    dxJointLimitMotor limot;  // limit and motor information
};

// contact
struct dxJointContact : public dxJoint {
    int the_m;  // number of rows computed by getInfo1
    dContact contact;
};

// angular motor
struct dxJointAMotor : public dxJoint {
    int num;                      // number of axes (0..3)
    int mode;                     // a dAMotorXXX constant
    int rel[3];                   // what the axes are relative to (global,b1,b2)
    dVector3 axis[3];             // three axes
    dxJointLimitMotor limot[3];   // limit+motor info for axes
    dReal angle[3];               // user-supplied angles for axes
    // these vectors are used for calculating euler angles
    dVector3 reference1;          // original axis[2], relative to body 1
    dVector3 reference2;          // original axis[0], relative to body 2
};

//****************************************************************************
// utility

// set three "ball-and-socket" rows in the constraint equation, and the
// corresponding right hand side.
static void setBall(dxJoint* joint, dxJoint::Info2* info, dVector3 anchor1, dVector3 anchor2);

// set the anchor points of a joint relative to its bodies
static inline void setAnchors(dxJoint* j, dReal x, dReal y, dReal z, dVector3 anchor1, dVector3 anchor2)
{
    if (j->node[0].body) {
        dReal q[4];
        q[0] = x - j->node[0].body->pos[0];
        q[1] = y - j->node[0].body->pos[1];
        q[2] = z - j->node[0].body->pos[2];
        q[3] = 0;
        dMULTIPLY1_331(anchor1, j->node[0].body->R, q);
        if (j->node[1].body) {
            q[0] = x - j->node[1].body->pos[0];
            q[1] = y - j->node[1].body->pos[1];
            q[2] = z - j->node[1].body->pos[2];
            q[3] = 0;
            dMULTIPLY1_331(anchor2, j->node[1].body->R, q);
        } else {
            anchor2[0] = x;
            anchor2[1] = y;
            anchor2[2] = z;
        }
    }
    anchor1[3] = 0;
    anchor2[3] = 0;
}

// set the joint axes relative to its bodies
static inline void setAxes(dxJoint* j, dReal x, dReal y, dReal z, dVector3 axis1, dVector3 axis2)
{
    if (j->node[0].body) {
        dReal q[4];
        q[0] = x;
        q[1] = y;
        q[2] = z;
        q[3] = 0;
        dNormalize3(q);
        if (axis1) {
            dMULTIPLY1_331(axis1, j->node[0].body->R, q);
            axis1[3] = 0;
        }
        if (axis2) {
            if (j->node[1].body) {
                dMULTIPLY1_331(axis2, j->node[1].body->R, q);
            } else {
                axis2[0] = x;
                axis2[1] = y;
                axis2[2] = z;
            }
            axis2[3] = 0;
        }
    }
}

// given two bodies (body1,body2), the hinge axis that they are connected by
// w.r.t. body1 (axis), and the initial relative orientation between them
// (q_initial), return the relative rotation angle.
static inline dReal getHingeAngleFromRelativeQuat(dQuaternion qrel, dVector3 axis)
{
    // extract the angle from the quaternion. cost2 = cos(theta/2),
    // sint2 = |sin(theta/2)|
    dReal cost2 = qrel[0];
    dReal sint2 = dSqrt(qrel[1] * qrel[1] + qrel[2] * qrel[2] + qrel[3] * qrel[3]);
    dReal theta = (dDOT(qrel + 1, axis) >= 0) ?  // @@@ padding assumptions
                      (2 * dAtan2(sint2, cost2)) :  // if u points in direction of axis
                      (2 * dAtan2(sint2, -cost2));  // if u points in opposite direction

    // the angle we get will be between 0..2*pi, but we want to return angles
    // between -pi..pi
    if (theta > M_PI) {
        theta -= 2 * M_PI;
    }

    // the angle we've just extracted has the wrong sign
    theta = -theta;

    return theta;
}

static inline dReal getHingeAngle(dxBody* body1, dxBody* body2, dVector3 axis, dQuaternion q_initial)
{
    // get qrel = relative rotation between the two bodies
    dQuaternion qrel;
    if (body2) {
        dQuaternion qq;
        dQMultiply1(qq, body1->q, body2->q);
        dQMultiply2(qrel, qq, q_initial);
    } else {
        // pretend body2->q is the identity
        dQMultiply3(qrel, body1->q, q_initial);
    }

    return getHingeAngleFromRelativeQuat(qrel, axis);
}

//****************************************************************************
// dxJointLimitMotor

inline void dxJointLimitMotor::init(dxWorld* world)
{
    vel = 0;
    fmax = 0;
    lostop = -dInfinity;
    histop = dInfinity;
    fudge_factor = 1;
    normal_cfm = world->global_cfm;
    stop_erp = world->global_erp;
    stop_cfm = world->global_cfm;
    bounce = 0;
    limit = 0;
    limit_err = 0;
}

inline void dxJointLimitMotor::set(int num, dReal value)
{
    switch (num) {
    case dParamLoStop:
        if (value <= histop) {
            lostop = value;
        }
        break;
    case dParamHiStop:
        if (value >= lostop) {
            histop = value;
        }
        break;
    case dParamVel:
        vel = value;
        break;
    case dParamFMax:
        if (value >= 0) {
            fmax = value;
        }
        break;
    case dParamFudgeFactor:
        if (value >= 0 && value <= 1) {
            fudge_factor = value;
        }
        break;
    case dParamBounce:
        bounce = value;
        break;
    case dParamCFM:
        normal_cfm = value;
        break;
    case dParamStopERP:
        stop_erp = value;
        break;
    case dParamStopCFM:
        stop_cfm = value;
        break;
    }
}

inline int dxJointLimitMotor::testRotationalLimit(dReal angle)
{
    if (angle <= lostop) {
        limit = 1;
        limit_err = angle - lostop;
        return 1;
    } else if (angle >= histop) {
        limit = 2;
        limit_err = angle - histop;
        return 1;
    } else {
        limit = 0;
        return 0;
    }
}

//****************************************************************************
// angular motor

static void amotorSetEulerReferenceVectors(dxJointAMotor* j);
static void amotorComputeEulerAngles(dxJointAMotor* joint, dVector3 ax[3]);
static void amotorComputeGlobalAxes(dxJointAMotor* joint, dVector3 ax[3]);

// 0x80273650: dJointSetAMotorMode
extern "C" void fn_80273650(dxJointAMotor* joint, int mode)
{
    joint->mode = mode;
    if (joint->mode == dAMotorEuler) {
        joint->num = 3;
        amotorSetEulerReferenceVectors(joint);
    }
}

// 0x80273688: dJointSetAMotorParam
extern "C" void fn_80273688(dxJointAMotor* joint, int parameter, dReal value)
{
    int anum = parameter >> 8;
    if (anum < 0) {
        anum = 0;
    }
    if (anum > 2) {
        anum = 2;
    }
    parameter &= 0xff;
    joint->limot[anum].set(parameter, value);
}

// 0x802737A4: dJointSetAMotorAxis
extern "C" void fn_802737A4(dxJointAMotor* joint, int anum, int rel, dReal x, dReal y, dReal z)
{
    if (anum < 0) {
        anum = 0;
    }
    if (anum > 2) {
        anum = 2;
    }
    joint->rel[anum] = rel;

    // x,y,z is always in global coordinates regardless of rel, so we may have
    // to convert it to be relative to a body
    dVector3 r;
    r[0] = x;
    r[1] = y;
    r[2] = z;
    r[3] = 0;
    if (rel > 0) {
        if (rel == 1) {
            dMULTIPLY1_331(joint->axis[anum], joint->node[0].body->R, r);
        } else {
            dMULTIPLY1_331(joint->axis[anum], joint->node[1].body->R, r);
        }
    } else {
        joint->axis[anum][0] = r[0];
        joint->axis[anum][1] = r[1];
        joint->axis[anum][2] = r[2];
    }
    dNormalize3(joint->axis[anum]);
    if (joint->mode == dAMotorEuler) {
        amotorSetEulerReferenceVectors(joint);
    }
}

// 0x80273918: dJointSetAMotorNumAxes
extern "C" void fn_80273918(dxJointAMotor* joint, int num)
{
    if (joint->mode == dAMotorEuler) {
        joint->num = 3;
    } else {
        if (num < 0) {
            num = 0;
        }
        if (num > 3) {
            num = 3;
        }
        joint->num = num;
    }
}

// 0x80273950: amotorGetInfo2
extern "C" void fn_80273950(dxJointAMotor* joint, dxJoint::Info2* info)
{
    int i;

    // compute the axes (if not global)
    dVector3 ax[3];
    amotorComputeGlobalAxes(joint, ax);

    // in euler angle mode we do not actually constrain the angular velocity
    // along the axes axis[0] and axis[2] (although we do use axis[1]) :
    //
    //    to get                    constrain
    //    ---------------------     ---------
    //    d(angle[0])/dt = 0        ax[1] x ax[2]
    //    d(angle[1])/dt = 0        ax[1]
    //    d(angle[2])/dt = 0        ax[0] x ax[1]
    //
    // constraining w2-w1 along an axis 'a' means that a'*(w2-w1)=0.
    // to get the constraint to be d(angle[i])/dt = 0, we need to use axes
    // ax[0] x ax[1] etc.

    dVector3* axptr[3];
    axptr[0] = &ax[0];
    axptr[1] = &ax[1];
    axptr[2] = &ax[2];

    dVector3 ax0_cross_ax1;
    dVector3 ax1_cross_ax2;
    if (joint->mode == dAMotorEuler) {
        dCROSS(ax0_cross_ax1, =, ax[0], ax[1]);
        axptr[2] = &ax0_cross_ax1;
        dCROSS(ax1_cross_ax2, =, ax[1], ax[2]);
        axptr[0] = &ax1_cross_ax2;
    }

    int row = 0;
    for (i = 0; i < joint->num; i++) {
        row += joint->limot[i].addLimot(joint, info, row, *(axptr[i]), 1);
    }
}

// 0x80273A70: amotorGetInfo1
extern "C" void fn_80273A70(dxJointAMotor* j, dxJoint::Info1* info)
{
    info->m = 0;
    info->nub = 0;

    // compute the axes and angles, if in euler mode
    if (j->mode == dAMotorEuler) {
        dVector3 ax[3];
        amotorComputeGlobalAxes(j, ax);
        amotorComputeEulerAngles(j, ax);
    }

    // see if we're powered or at a joint limit for each axis
    for (int i = 0; i < j->num; i++) {
        if (j->limot[i].testRotationalLimit(j->angle[i]) || j->limot[i].fmax > 0) {
            info->m++;
        }
    }
}

// 0x80273B7C: amotorSetEulerReferenceVectors. set the reference vectors as follows:
//   * reference1 = current axis[2] relative to body 1
//   * reference2 = current axis[0] relative to body 2
// this assumes that:
//    * axis[0] is relative to body 1
//    * axis[2] is relative to body 2
static void amotorSetEulerReferenceVectors(dxJointAMotor* j)
{
    if (j->node[0].body && j->node[1].body) {
        dVector3 r;  // axis[2] and axis[0] in global coordinates
        dMULTIPLY0_331(r, j->node[1].body->R, j->axis[2]);
        dMULTIPLY1_331(j->reference1, j->node[0].body->R, r);
        dMULTIPLY0_331(r, j->node[0].body->R, j->axis[0]);
        dMULTIPLY1_331(j->reference2, j->node[1].body->R, r);
    }
}

// 0x80273DF0: amotorComputeEulerAngles. compute the 3 euler angles, given the global axes ax[] (the
// reference vectors must be set up already)
static void amotorComputeEulerAngles(dxJointAMotor* joint, dVector3 ax[3])
{
    // assumptions:
    //   global axes already calculated --> ax
    //   axis[0] is relative to body 1 --> global ax[0]
    //   axis[2] is relative to body 2 --> global ax[2]
    //   ax[1] = ax[2] x ax[0]
    //   original ax[0] and ax[2] are perpendicular
    //   reference1 is perpendicular to ax[0] (in body 1 frame)
    //   reference2 is perpendicular to ax[2] (in body 2 frame)
    //   all ax[] and reference vectors are unit length

    // calculate references in global frame
    dVector3 ref1, ref2;
    dMULTIPLY0_331(ref1, joint->node[0].body->R, joint->reference1);
    dMULTIPLY0_331(ref2, joint->node[1].body->R, joint->reference2);

    // get q perpendicular to both ax[0] and ref1, get first euler angle
    dVector3 q;
    dCROSS(q, =, ax[0], ref1);
    joint->angle[0] = -dAtan2(dDOT(ax[2], q), dDOT(ax[2], ref1));

    // get q perpendicular to both ax[0] and ax[1], get second euler angle
    dCROSS(q, =, ax[0], ax[1]);
    joint->angle[1] = -dAtan2(dDOT(ax[2], ax[0]), dDOT(ax[2], q));

    // get q perpendicular to both ax[1] and ax[2], get third euler angle
    dCROSS(q, =, ax[1], ax[2]);
    joint->angle[2] = -dAtan2(dDOT(ref2, ax[1]), dDOT(ref2, q));
}

// 0x8027403C: amotorComputeGlobalAxes. compute the 3 axes in global coordinates
static void amotorComputeGlobalAxes(dxJointAMotor* joint, dVector3 ax[3])
{
    if (joint->mode == dAMotorEuler) {
        // special handling for euler mode
        dMULTIPLY0_331(ax[0], joint->node[0].body->R, joint->axis[0]);
        dMULTIPLY0_331(ax[2], joint->node[1].body->R, joint->axis[2]);
        dCROSS(ax[1], =, ax[2], ax[0]);
        dNormalize3(ax[1]);
    } else {
        for (int i = 0; i < joint->num; i++) {
            if (joint->rel[i] == 1) {
                // relative to b1
                dMULTIPLY0_331(ax[i], joint->node[0].body->R, joint->axis[i]);
            } else if (joint->rel[i] == 2) {
                // relative to b2
                dMULTIPLY0_331(ax[i], joint->node[1].body->R, joint->axis[i]);
            } else {
                // global - just copy it
                ax[i][0] = joint->axis[i][0];
                ax[i][1] = joint->axis[i][1];
                ax[i][2] = joint->axis[i][2];
            }
        }
    }
}

// 0x80274338: amotorInit
extern "C" void fn_80274338(dxJointAMotor* j)
{
    int i;
    j->num = 0;
    j->mode = dAMotorUser;
    for (i = 0; i < 3; i++) {
        j->rel[i] = 0;
        dSetZero(j->axis[i], 4);
        j->limot[i].init(j->world);
        j->angle[i] = 0;
    }
    dSetZero(j->reference1, 4);
    dSetZero(j->reference2, 4);
}

// 0x80274454: setBall
static void setBall(dxJoint* joint, dxJoint::Info2* info, dVector3 anchor1, dVector3 anchor2)
{
    // anchor points in global coordinates with respect to body PORs.
    dVector3 a1, a2;

    int s = info->rowskip;

    // set jacobian
    info->J1l[0] = 1;
    info->J1l[s + 1] = 1;
    info->J1l[2 * s + 2] = 1;
    dMULTIPLY0_331(a1, joint->node[0].body->R, anchor1);
    dCROSSMAT(info->J1a, a1, s, -, +);
    if (joint->node[1].body) {
        info->J2l[0] = -1;
        info->J2l[s + 1] = -1;
        info->J2l[2 * s + 2] = -1;
        dMULTIPLY0_331(a2, joint->node[1].body->R, anchor2);
        dCROSSMAT(info->J2a, a2, s, +, -);
    }

    // set right hand side
    dReal k = info->fps * info->erp;
    if (joint->node[1].body) {
        for (int j = 0; j < 3; j++) {
            info->c[j] = k * (a2[j] + joint->node[1].body->pos[j] - a1[j] - joint->node[0].body->pos[j]);
        }
    } else {
        for (int j = 0; j < 3; j++) {
            info->c[j] = k * (anchor2[j] - a1[j] - joint->node[0].body->pos[j]);
        }
    }
}

//****************************************************************************
// contact

// 0x80274714: contactGetInfo2
extern "C" void fn_80274714(dxJointContact* j, dxJoint::Info2* info)
{
    int i, s = info->rowskip;
    int s2 = 2 * s;

    // get normal, with sign adjusted for body1/body2 polarity
    dVector3 normal;
    if (j->flags & dJOINT_REVERSE) {
        normal[0] = -j->contact.geom.normal[0];
        normal[1] = -j->contact.geom.normal[1];
        normal[2] = -j->contact.geom.normal[2];
    } else {
        normal[0] = j->contact.geom.normal[0];
        normal[1] = j->contact.geom.normal[1];
        normal[2] = j->contact.geom.normal[2];
    }
    normal[3] = 0;  // @@@ hmmm

    // c1,c2 = contact points with respect to body PORs
    dVector3 c1, c2;
    for (i = 0; i < 3; i++) {
        c1[i] = j->contact.geom.pos[i] - j->node[0].body->pos[i];
    }

    // set jacobian for normal
    info->J1l[0] = normal[0];
    info->J1l[1] = normal[1];
    info->J1l[2] = normal[2];
    dCROSS(info->J1a, =, c1, normal);
    if (j->node[1].body) {
        for (i = 0; i < 3; i++) {
            c2[i] = j->contact.geom.pos[i] - j->node[1].body->pos[i];
        }
        info->J2l[0] = -normal[0];
        info->J2l[1] = -normal[1];
        info->J2l[2] = -normal[2];
        dCROSS(info->J2a, = -, c2, normal);
    }

    // set right hand side and cfm value for normal
    dReal erp = info->erp;
    if (j->contact.surface.mode & dContactSoftERP) {
        erp = j->contact.surface.soft_erp;
    }
    dReal k = info->fps * erp;
    dReal depth = j->contact.geom.depth - j->world->contactp.min_depth;
    if (depth < 0) {
        depth = 0;
    }
    dReal maxvel = j->world->contactp.max_vel;
    if (k * depth > maxvel) {
        info->c[0] = maxvel;
    } else {
        info->c[0] = k * depth;
    }
    if (j->contact.surface.mode & dContactSoftCFM) {
        info->cfm[0] = j->contact.surface.soft_cfm;
    }

    // deal with bounce
    if (j->contact.surface.mode & dContactBounce) {
        // calculate outgoing velocity (-ve for incoming contact)
        dReal outgoing = dDOT(info->J1l, j->node[0].body->lvel) + dDOT(info->J1a, j->node[0].body->avel);
        if (j->node[1].body) {
            outgoing += dDOT(info->J2l, j->node[1].body->lvel) + dDOT(info->J2a, j->node[1].body->avel);
        }
        // only apply bounce if the outgoing velocity is greater than the
        // threshold, and if the resulting c[0] exceeds what we already have.
        if (j->contact.surface.bounce_vel >= 0 && (-outgoing) > j->contact.surface.bounce_vel) {
            dReal newc = -j->contact.surface.bounce * outgoing;
            if (newc > info->c[0]) {
                info->c[0] = newc;
            }
        }
    }

    // set LCP limits for normal
    info->lo[0] = 0;
    info->hi[0] = dInfinity;

    // now do jacobian for tangential forces
    dVector3 t1, t2;  // two vectors tangential to normal

    // first friction direction
    if (j->the_m >= 2) {
        if (j->contact.surface.mode & dContactFDir1) {  // use fdir1 ?
            t1[0] = j->contact.fdir1[0];
            t1[1] = j->contact.fdir1[1];
            t1[2] = j->contact.fdir1[2];
            dCROSS(t2, =, normal, t1);
        } else {
            dPlaneSpace(normal, t1, t2);
        }
        info->J1l[s + 0] = t1[0];
        info->J1l[s + 1] = t1[1];
        info->J1l[s + 2] = t1[2];
        dCROSS(info->J1a + s, =, c1, t1);
        if (j->node[1].body) {
            info->J2l[s + 0] = -t1[0];
            info->J2l[s + 1] = -t1[1];
            info->J2l[s + 2] = -t1[2];
            dCROSS(info->J2a + s, = -, c2, t1);
        }
        // set right hand side
        if (j->contact.surface.mode & dContactMotion1) {
            info->c[1] = j->contact.surface.motion1;
        }
        // set LCP bounds and friction index. this depends on the approximation
        // mode
        info->lo[1] = -j->contact.surface.mu;
        info->hi[1] = j->contact.surface.mu;
        if (j->contact.surface.mode & dContactApprox1_1) {
            info->findex[1] = 0;
        }

        // set slip (constraint force mixing)
        if (j->contact.surface.mode & dContactSlip1) {
            info->cfm[1] = j->contact.surface.slip1;
        }
    }

    // second friction direction
    if (j->the_m >= 3) {
        info->J1l[s2 + 0] = t2[0];
        info->J1l[s2 + 1] = t2[1];
        info->J1l[s2 + 2] = t2[2];
        dCROSS(info->J1a + s2, =, c1, t2);
        if (j->node[1].body) {
            info->J2l[s2 + 0] = -t2[0];
            info->J2l[s2 + 1] = -t2[1];
            info->J2l[s2 + 2] = -t2[2];
            dCROSS(info->J2a + s2, = -, c2, t2);
        }
        // set right hand side
        if (j->contact.surface.mode & dContactMotion2) {
            info->c[2] = j->contact.surface.motion2;
        }
        // set LCP bounds and friction index. this depends on the approximation
        // mode
        if (j->contact.surface.mode & dContactMu2) {
            info->lo[2] = -j->contact.surface.mu2;
            info->hi[2] = j->contact.surface.mu2;
        } else {
            info->lo[2] = -j->contact.surface.mu;
            info->hi[2] = j->contact.surface.mu;
        }
        if (j->contact.surface.mode & dContactApprox1_2) {
            info->findex[2] = 0;
        }

        // set slip (constraint force mixing)
        if (j->contact.surface.mode & dContactSlip2) {
            info->cfm[2] = j->contact.surface.slip2;
        }
    }
}

// 0x80274E7C: contactGetInfo1
extern "C" void fn_80274E7C(dxJointContact* j, dxJoint::Info1* info)
{
    // make sure mu's >= 0, then calculate number of constraint rows and number
    // of unbounded rows.
    int m = 1, nub = 0;
    if (j->contact.surface.mu < 0) {
        j->contact.surface.mu = 0;
    }
    if (j->contact.surface.mode & dContactMu2) {
        if (j->contact.surface.mu > 0) {
            m++;
        }
        if (j->contact.surface.mu2 < 0) {
            j->contact.surface.mu2 = 0;
        }
        if (j->contact.surface.mu2 > 0) {
            m++;
        }
        if (j->contact.surface.mu == dInfinity) {
            nub++;
        }
        if (j->contact.surface.mu2 == dInfinity) {
            nub++;
        }
    } else {
        if (j->contact.surface.mu > 0) {
            m += 2;
        }
        if (j->contact.surface.mu == dInfinity) {
            nub += 2;
        }
    }

    j->the_m = m;
    info->m = m;
    info->nub = nub;
}

// 0x80274F3C: contactInit
extern "C" void fn_80274F3C(dxJointContact* j)
{
    // default frictionless contact. hmmm, this info gets overwritten
    // by dJointCreateContact() anyway, so why do we bother?
}

//****************************************************************************
// hinge

static inline void hingeComputeInitialRelativeRotation(dxJointHinge* joint)
{
    if (joint->node[0].body) {
        if (joint->node[1].body) {
            dQMultiply1(joint->qrel, joint->node[0].body->q, joint->node[1].body->q);
        } else {
            // set joint->qrel to the transpose of the first body q
            joint->qrel[0] = joint->node[0].body->q[0];
            for (int i = 1; i < 4; i++) {
                joint->qrel[i] = -joint->node[0].body->q[i];
            }
        }
    }
}

// 0x80274F40: dJointSetHingeParam
extern "C" void fn_80274F40(dxJointHinge* joint, int parameter, dReal value)
{
    joint->limot.set(parameter, value);
}

// 0x80274FF8: dJointSetHingeAxis
extern "C" void fn_80274FF8(dxJointHinge* joint, dReal x, dReal y, dReal z)
{
    setAxes(joint, x, y, z, joint->axis1, joint->axis2);
    hingeComputeInitialRelativeRotation(joint);
}

// 0x80275218: dJointSetHingeAnchor
extern "C" void fn_80275218(dxJointHinge* joint, dReal x, dReal y, dReal z)
{
    setAnchors(joint, x, y, z, joint->anchor1, joint->anchor2);
    hingeComputeInitialRelativeRotation(joint);
}

// 0x802753A4: hingeGetInfo2
extern "C" void fn_802753A4(dxJointHinge* joint, dxJoint::Info2* info)
{
    // set the three ball-and-socket rows
    setBall(joint, info, joint->anchor1, joint->anchor2);

    // set the two hinge rows. the hinge axis should be the only unconstrained
    // rotational axis, the angular velocity of the two bodies perpendicular to
    // the hinge axis should be equal. thus the constraint equations are
    //    p*w1 - p*w2 = 0
    //    q*w1 - q*w2 = 0
    // where p and q are unit vectors normal to the hinge axis, and w1 and w2
    // are the angular velocity vectors of the two bodies.

    dVector3 ax1;  // length 1 joint axis in global coordinates, from 1st body
    dVector3 p, q; // plane space vectors for ax1
    dMULTIPLY0_331(ax1, joint->node[0].body->R, joint->axis1);
    dPlaneSpace(ax1, p, q);

    int s3 = 3 * info->rowskip;
    int s4 = 4 * info->rowskip;

    info->J1a[s3 + 0] = p[0];
    info->J1a[s3 + 1] = p[1];
    info->J1a[s3 + 2] = p[2];
    info->J1a[s4 + 0] = q[0];
    info->J1a[s4 + 1] = q[1];
    info->J1a[s4 + 2] = q[2];

    if (joint->node[1].body) {
        info->J2a[s3 + 0] = -p[0];
        info->J2a[s3 + 1] = -p[1];
        info->J2a[s3 + 2] = -p[2];
        info->J2a[s4 + 0] = -q[0];
        info->J2a[s4 + 1] = -q[1];
        info->J2a[s4 + 2] = -q[2];
    }

    // compute the right hand side of the constraint equation. set relative
    // body velocities along p and q to bring the hinge back into alignment.
    // ax1 x ax2 is in the plane space of ax1, so we project the angular
    // velocity to p and q to find the right hand side.

    dVector3 ax2, b;
    if (joint->node[1].body) {
        dMULTIPLY0_331(ax2, joint->node[1].body->R, joint->axis2);
    } else {
        ax2[0] = joint->axis2[0];
        ax2[1] = joint->axis2[1];
        ax2[2] = joint->axis2[2];
    }
    dCROSS(b, =, ax1, ax2);
    dReal k = info->fps * info->erp;
    info->c[3] = k * dDOT(b, p);
    info->c[4] = k * dDOT(b, q);

    // if the hinge is powered, or has joint limits, add in the stuff
    joint->limot.addLimot(joint, info, 5, ax1, 1);
}

// 0x80275640: hingeGetInfo1
extern "C" void fn_80275640(dxJointHinge* j, dxJoint::Info1* info)
{
    info->nub = 5;

    // see if joint is powered
    if (j->limot.fmax > 0) {
        info->m = 6;  // powered hinge needs an extra constraint row
    } else {
        info->m = 5;
    }

    // see if we're at a joint limit.
    if ((j->limot.lostop >= -M_PI || j->limot.histop <= M_PI) && j->limot.lostop <= j->limot.histop) {
        dReal angle = getHingeAngle(j->node[0].body, j->node[1].body, j->axis1, j->qrel);
        if (j->limot.testRotationalLimit(angle)) {
            info->m = 6;
        }
    }
}

// 0x802758F4: hingeInit
extern "C" void fn_802758F4(dxJointHinge* j)
{
    dSetZero(j->anchor1, 4);
    dSetZero(j->anchor2, 4);
    dSetZero(j->axis1, 4);
    j->axis1[0] = 1;
    dSetZero(j->axis2, 4);
    j->axis2[0] = 1;
    dSetZero(j->qrel, 4);
    j->limot.init(j->world);
}

//****************************************************************************
// ball and socket

// 0x802759B8: dJointSetBallAnchor
extern "C" void fn_802759B8(dxJointBall* joint, dReal x, dReal y, dReal z)
{
    setAnchors(joint, x, y, z, joint->anchor1, joint->anchor2);
}

// 0x80275AC8: ballGetInfo2
extern "C" void fn_80275AC8(dxJointBall* joint, dxJoint::Info2* info)
{
    setBall(joint, info, joint->anchor1, joint->anchor2);
}

// 0x80275AF0: ballGetInfo1
extern "C" void fn_80275AF0(dxJointBall* j, dxJoint::Info1* info)
{
    info->m = 3;
    info->nub = 3;
}

// 0x80275B00: ballInit
extern "C" void fn_80275B00(dxJointBall* j)
{
    dSetZero(j->anchor1, 4);
    dSetZero(j->anchor2, 4);
}

// 0x80275B40: dxJointLimitMotor::addLimot. if the joint is powered, or has joint limits, add in
// the extra row (returns 1 if a row was added)
int dxJointLimitMotor::addLimot(dxJoint* joint, dxJoint::Info2* info, int row, dVector3 ax1, int rotational)
{
    int srow = row * info->rowskip;

    // if the joint is powered, or has joint limits, add in the extra row
    int powered = fmax > 0;
    if (powered || limit) {
        dReal* J1 = rotational ? info->J1a : info->J1l;
        dReal* J2 = rotational ? info->J2a : info->J2l;

        J1[srow + 0] = ax1[0];
        J1[srow + 1] = ax1[1];
        J1[srow + 2] = ax1[2];
        if (joint->node[1].body) {
            J2[srow + 0] = -ax1[0];
            J2[srow + 1] = -ax1[1];
            J2[srow + 2] = -ax1[2];
        }

        // linear limot torque decoupling step: apply the constraint forces at
        // the point halfway between the body centers (only for two bodies).

        dVector3 ltd;  // Linear Torque Decoupling vector (a torque)
        if (!rotational && joint->node[1].body) {
            dVector3 c;
            c[0] = REAL(0.5) * (joint->node[1].body->pos[0] - joint->node[0].body->pos[0]);
            c[1] = REAL(0.5) * (joint->node[1].body->pos[1] - joint->node[0].body->pos[1]);
            c[2] = REAL(0.5) * (joint->node[1].body->pos[2] - joint->node[0].body->pos[2]);
            dCROSS(ltd, =, c, ax1);
            info->J1a[srow + 0] = ltd[0];
            info->J1a[srow + 1] = ltd[1];
            info->J1a[srow + 2] = ltd[2];
            info->J2a[srow + 0] = ltd[0];
            info->J2a[srow + 1] = ltd[1];
            info->J2a[srow + 2] = ltd[2];
        }

        // if we're limited low and high simultaneously, the joint motor is
        // ineffective
        if (limit && (lostop == histop)) {
            powered = 0;
        }

        if (powered) {
            info->cfm[row] = normal_cfm;
            if (!limit) {
                info->c[row] = vel;
                info->lo[row] = -fmax;
                info->hi[row] = fmax;
            } else {
                // the joint is at a limit, AND is being powered. if the joint is
                // being powered into the limit then we apply the maximum motor force
                // in that direction, because the motor is working against the
                // immovable limit. if the joint is being powered away from the limit
                // then we have problems because actually we need *two* lcp
                // constraints to handle this case. so we fake it and apply some
                // fraction of the maximum force. the fraction to use can be set as
                // a fudge factor.

                dReal fm = fmax;
                if ((vel > 0) || (vel == 0 && limit == 2)) {
                    fm = -fm;
                }

                // if we're powering away from the limit, apply the fudge factor
                if ((limit == 1 && vel > 0) || (limit == 2 && vel < 0)) {
                    fm *= fudge_factor;
                }

                if (rotational) {
                    dBodyAddTorque(joint->node[0].body, -fm * ax1[0], -fm * ax1[1], -fm * ax1[2]);
                    if (joint->node[1].body) {
                        dBodyAddTorque(joint->node[1].body, fm * ax1[0], fm * ax1[1], fm * ax1[2]);
                    }
                } else {
                    dBodyAddForce(joint->node[0].body, -fm * ax1[0], -fm * ax1[1], -fm * ax1[2]);
                    if (joint->node[1].body) {
                        dBodyAddForce(joint->node[1].body, fm * ax1[0], fm * ax1[1], fm * ax1[2]);

                        // linear limot torque decoupling step: refer to above discussion
                        dBodyAddTorque(joint->node[0].body, -fm * ltd[0], -fm * ltd[1], -fm * ltd[2]);
                        dBodyAddTorque(joint->node[1].body, -fm * ltd[0], -fm * ltd[1], -fm * ltd[2]);
                    }
                }
            }
        }

        if (limit) {
            dReal k = info->fps * stop_erp;
            info->c[row] = -k * limit_err;
            info->cfm[row] = stop_cfm;

            if (lostop == histop) {
                // limited low and high simultaneously
                info->lo[row] = -dInfinity;
                info->hi[row] = dInfinity;
            } else {
                if (limit == 1) {
                    // low limit
                    info->lo[row] = 0;
                    info->hi[row] = dInfinity;
                } else {
                    // high limit
                    info->lo[row] = -dInfinity;
                    info->hi[row] = 0;
                }

                // deal with bounce
                if (bounce > 0) {
                    // calculate joint velocity
                    dReal vel;
                    if (rotational) {
                        vel = dDOT(joint->node[0].body->avel, ax1);
                        if (joint->node[1].body) {
                            vel -= dDOT(joint->node[1].body->avel, ax1);
                        }
                    } else {
                        vel = dDOT(joint->node[0].body->lvel, ax1);
                        if (joint->node[1].body) {
                            vel -= dDOT(joint->node[1].body->lvel, ax1);
                        }
                    }

                    // only apply bounce if the velocity is incoming, and if the
                    // resulting c[] exceeds what we already have.
                    if (limit == 1) {
                        // low limit
                        if (vel < 0) {
                            dReal newc = -bounce * vel;
                            if (newc > info->c[row]) {
                                info->c[row] = newc;
                            }
                        }
                    } else {
                        // high limit - all those computations are reversed
                        if (vel > 0) {
                            dReal newc = -bounce * vel;
                            if (newc < info->c[row]) {
                                info->c[row] = newc;
                            }
                        }
                    }
                }
            }
        }
        return 1;
    } else {
        return 0;
    }
}

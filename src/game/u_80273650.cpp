// u_80273650.cpp (0x80273650-0x80276090): ODE joint library (joint parameter switch tables
// jumptable_803C0528/054C). Split from u_8026C200.
// Evidence: own .sdata2 pool 0x804DA258-0x804DA298 (1, -1, 0, 2, -FLT_MAX/FLT_MAX, -pi/pi, 0.5/3.0/0.0 doubles, 2pi, 0.5).
// fn_80273650 opens it: it calls fn_80273B7C and sets joint fields; the error file before it has no such code.
// This is ODE 0.5 joint.cpp (used in retail: ball, hinge, contact and angular-motor joints, the dxJointLimitMotor
// helpers, and their vtables __dball/__dhinge/__dcontact/__damotor_vtable at .data 0x803C04D8).
// Built with -inline deferred (user-approved 2026-11-27): the functions are in ODE source order and come out
// reversed, as in retail. setBall is `static inline` as in ODE: its out-of-line copy is emitted next to its
// first non-inlined caller, which in retail is a linker-dropped joint between contact and amotor
// (universalGetInfo2).
// The joint types Geist never creates (slider, hinge2, universal, fixed, null) and the unused utilities/getters are
// restored from the real ODE 0.5 joint.cpp in source order; the linker drops them. They fix the .sdata2 pool head
// (1, -1, 0, 2: setBall/setFixedOrientation) and put setBall's out-of-line copy next to universalGetInfo2.
// Function names are the ODE ones in comments; symbols keep their placeholder names.

typedef unsigned long u32;
typedef unsigned long size_t;
#define NULL 0

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
extern "C" void fn_8027B570(dQuaternion q, const dMatrix3 R);                    // dQfromR
extern "C" void dRFrom2Axes(dMatrix3 R, dReal ax, dReal ay, dReal az, dReal bx, dReal by, dReal bz);
extern "C" void fn_8027337C(int num, const char* msg, ...);                      // dDebug

#define dNormalize3 fn_8027914C
#define dPlaneSpace fn_80278D2C
#define dQMultiply1 fn_8027BC38
#define dQMultiply2 fn_8027BBC0
#define dQMultiply3 fn_8027BB48
#define dRtoQ fn_8027B570
#define dDebug fn_8027337C
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

    typedef void init_fn(dxJoint* joint);
    typedef void getInfo1_fn(dxJoint* joint, Info1* info);
    typedef void getInfo2_fn(dxJoint* joint, Info2* info);
    struct Vtable {
        int size;
        init_fn* init;
        getInfo1_fn* getInfo1;
        getInfo2_fn* getInfo2;
        int typenum;  // a dJointTypeXXX type number
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
    dJointTypeNone = 0,
    dJointTypeBall,
    dJointTypeHinge,
    dJointTypeSlider,
    dJointTypeContact,
    dJointTypeUniversal,
    dJointTypeHinge2,
    dJointTypeFixed,
    dJointTypeNull,
    dJointTypeAMotor
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
    dParamStopCFM,
    dParamSuspensionERP,
    dParamSuspensionCFM
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
    dReal get(int num);
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

// universal
struct dxJointUniversal : public dxJoint {
    dVector3 anchor1;          // anchor w.r.t first body
    dVector3 anchor2;          // anchor w.r.t second body
    dVector3 axis1;            // axis w.r.t first body
    dVector3 axis2;            // axis w.r.t second body
    dQuaternion qrel1;         // initial relative rotation body1 -> virtual cross piece
    dQuaternion qrel2;         // initial relative rotation virtual cross piece -> body2
    dxJointLimitMotor limot1;  // limit and motor information for axis1
    dxJointLimitMotor limot2;  // limit and motor information for axis2
};

// slider. if body2 is 0 then qrel is the absolute rotation of body1 and
// offset is the position of body1 center along axis1.
struct dxJointSlider : public dxJoint {
    dVector3 axis1;           // axis w.r.t first body
    dQuaternion qrel;         // initial relative rotation body1 -> body2
    dVector3 offset;          // point relative to body2 that should be
                              // aligned with body1 center along axis1
    dxJointLimitMotor limot;  // limit and motor information
};

// contact
struct dxJointContact : public dxJoint {
    int the_m;  // number of rows computed by getInfo1
    dContact contact;
};

// hinge 2
struct dxJointHinge2 : public dxJoint {
    dVector3 anchor1;           // anchor w.r.t first body
    dVector3 anchor2;           // anchor w.r.t second body
    dVector3 axis1;             // axis 1 w.r.t first body
    dVector3 axis2;             // axis 2 w.r.t second body
    dReal c0, s0;               // cos,sin of desired angle between axis 1,2
    dVector3 v1, v2;            // angle ref vectors embedded in first body
    dxJointLimitMotor limot1;   // limit+motor info for axis 1
    dxJointLimitMotor limot2;   // limit+motor info for axis 2
    dReal susp_erp, susp_cfm;   // suspension parameters (erp,cfm)
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

// fixed
struct dxJointFixed : public dxJoint {
    dQuaternion qrel;  // initial relative rotation body1 -> body2
    dVector3 offset;   // relative offset between the bodies
};

// null joint, for testing only
struct dxJointNull : public dxJoint {
};

//****************************************************************************
// utility

// set three "ball-and-socket" rows in the constraint equation, and the
// corresponding right hand side.
static inline void setBall(dxJoint* joint, dxJoint::Info2* info, dVector3 anchor1, dVector3 anchor2)
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

// this is like setBall(), except that `axis' is a unit length vector
// (in global coordinates) that should be used for the first jacobian
// position row (the other two row vectors will be derived from this).
// `erp1' is the erp value to use along the axis.

static inline void setBall2 (dxJoint *joint, dxJoint::Info2 *info,
                            dVector3 anchor1, dVector3 anchor2,
                            dVector3 axis, dReal erp1)
{
    // anchor points in global coordinates with respect to body PORs.
    dVector3 a1,a2;

    int i,s = info->rowskip;

    // get vectors normal to the axis. in setBall() axis,q1,q2 is [1 0 0],
    // [0 1 0] and [0 0 1], which makes everything much easier.
    dVector3 q1,q2;
    dPlaneSpace (axis,q1,q2);

    // set jacobian
    for (i=0; i<3; i++) info->J1l[i] = axis[i];
    for (i=0; i<3; i++) info->J1l[s+i] = q1[i];
    for (i=0; i<3; i++) info->J1l[2*s+i] = q2[i];
    dMULTIPLY0_331 (a1,joint->node[0].body->R,anchor1);
    dCROSS (info->J1a,=,a1,axis);
    dCROSS (info->J1a+s,=,a1,q1);
    dCROSS (info->J1a+2*s,=,a1,q2);
    if (joint->node[1].body) {
        for (i=0; i<3; i++) info->J2l[i] = -axis[i];
        for (i=0; i<3; i++) info->J2l[s+i] = -q1[i];
        for (i=0; i<3; i++) info->J2l[2*s+i] = -q2[i];
        dMULTIPLY0_331 (a2,joint->node[1].body->R,anchor2);
        dCROSS (info->J2a,= -,a2,axis);
        dCROSS (info->J2a+s,= -,a2,q1);
        dCROSS (info->J2a+2*s,= -,a2,q2);
    }

    // set right hand side - measure error along (axis,q1,q2)
    dReal k1 = info->fps * erp1;
    dReal k = info->fps * info->erp;

    for (i=0; i<3; i++) a1[i] += joint->node[0].body->pos[i];
    if (joint->node[1].body) {
        for (i=0; i<3; i++) a2[i] += joint->node[1].body->pos[i];
        info->c[0] = k1 * (dDOT(axis,a2) - dDOT(axis,a1));
        info->c[1] = k * (dDOT(q1,a2) - dDOT(q1,a1));
        info->c[2] = k * (dDOT(q2,a2) - dDOT(q2,a1));
    }
    else {
        info->c[0] = k1 * (dDOT(axis,anchor2) - dDOT(axis,a1));
        info->c[1] = k * (dDOT(q1,anchor2) - dDOT(q1,a1));
        info->c[2] = k * (dDOT(q2,anchor2) - dDOT(q2,a1));
    }
}

// set three orientation rows in the constraint equation, and the
// corresponding right hand side.

// corresponding right hand side.

static void setFixedOrientation(dxJoint *joint, dxJoint::Info2 *info, dQuaternion qrel, int start_row)
{
    int s = info->rowskip;
    int start_index = start_row * s;

    // 3 rows to make body rotations equal
    info->J1a[start_index] = 1;
    info->J1a[start_index + s + 1] = 1;
    info->J1a[start_index + s*2+2] = 1;
    if (joint->node[1].body) {
        info->J2a[start_index] = -1;
        info->J2a[start_index + s+1] = -1;
        info->J2a[start_index + s*2+2] = -1;
    }

    // compute the right hand side. the first three elements will result in
    // relative angular velocity of the two bodies - this is set to bring them
    // back into alignment. the correcting angular velocity is
    //   |angular_velocity| = angle/time = erp*theta / stepsize
    //                      = (erp*fps) * theta
    //    angular_velocity  = |angular_velocity| * u
    //                      = (erp*fps) * theta * u
    // where rotation along unit length axis u by theta brings body 2's frame
    // to qrel with respect to body 1's frame. using a small angle approximation
    // for sin(), this gives
    //    angular_velocity  = (erp*fps) * 2 * v
    // where the quaternion of the relative rotation between the two bodies is
    //    q = [cos(theta/2) sin(theta/2)*u] = [s v]

    // get qerr = relative rotation (rotation error) between two bodies
    dQuaternion qerr,e;
    if (joint->node[1].body) {
        dQuaternion qq;
        dQMultiply1 (qq,joint->node[0].body->q,joint->node[1].body->q);
        dQMultiply2 (qerr,qq,qrel);
    }
    else {
        dQMultiply3 (qerr,joint->node[0].body->q,qrel);
    }
    if (qerr[0] < 0) {
        qerr[1] = -qerr[1];         // adjust sign of qerr to make theta small
        qerr[2] = -qerr[2];
        qerr[3] = -qerr[3];
    }
    dMULTIPLY0_331 (e,joint->node[0].body->R,qerr+1); // @@@ bad SIMD padding!
    dReal k = info->fps * info->erp;
    info->c[start_row] = 2*k * e[0];
    info->c[start_row+1] = 2*k * e[1];
    info->c[start_row+2] = 2*k * e[2];
}

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

static void getAnchor (dxJoint *j, dVector3 result, dVector3 anchor1)
{
    if (j->node[0].body) {
        dMULTIPLY0_331 (result,j->node[0].body->R,anchor1);
        result[0] += j->node[0].body->pos[0];
        result[1] += j->node[0].body->pos[1];
        result[2] += j->node[0].body->pos[2];
    }
}

static void getAnchor2 (dxJoint *j, dVector3 result, dVector3 anchor2)
{
    if (j->node[1].body) {
        dMULTIPLY0_331 (result,j->node[1].body->R,anchor2);
        result[0] += j->node[1].body->pos[0];
        result[1] += j->node[1].body->pos[1];
        result[2] += j->node[1].body->pos[2];
    }
    else {
        result[0] = anchor2[0];
        result[1] = anchor2[1];
        result[2] = anchor2[2];
    }
}

static void getAxis (dxJoint *j, dVector3 result, dVector3 axis1)
{
    if (j->node[0].body) {
        dMULTIPLY0_331 (result,j->node[0].body->R,axis1);
    }
}

static void getAxis2 (dxJoint *j, dVector3 result, dVector3 axis2)
{
    if (j->node[1].body) {
        dMULTIPLY0_331 (result,j->node[1].body->R,axis2);
    }
    else {
        result[0] = axis2[0];
        result[1] = axis2[1];
        result[2] = axis2[2];
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

inline dReal dxJointLimitMotor::get (int num)
{
    switch (num) {
    case dParamLoStop: return lostop;
    case dParamHiStop: return histop;
    case dParamVel: return vel;
    case dParamFMax: return fmax;
    case dParamFudgeFactor: return fudge_factor;
    case dParamBounce: return bounce;
    case dParamCFM: return normal_cfm;
    case dParamStopERP: return stop_erp;
    case dParamStopCFM: return stop_cfm;
    default: return 0;
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
                if (vel > 0) {
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

//****************************************************************************
// ball and socket

// 0x80275B00: ballInit
extern "C" void fn_80275B00(dxJointBall* j)
{
    dSetZero(j->anchor1, 4);
    dSetZero(j->anchor2, 4);
}

// 0x80275AF0: ballGetInfo1
extern "C" void fn_80275AF0(dxJointBall* j, dxJoint::Info1* info)
{
    info->m = 3;
    info->nub = 3;
}

// 0x80275AC8: ballGetInfo2
extern "C" void fn_80275AC8(dxJointBall* joint, dxJoint::Info2* info)
{
    setBall(joint, info, joint->anchor1, joint->anchor2);
}

// 0x802759B8: dJointSetBallAnchor
extern "C" void fn_802759B8(dxJointBall* joint, dReal x, dReal y, dReal z)
{
    setAnchors(joint, x, y, z, joint->anchor1, joint->anchor2);
}

dxJoint::Vtable __dball_vtable = { sizeof(dxJointBall),
                            (dxJoint::init_fn*)fn_80275B00,
                            (dxJoint::getInfo1_fn*)fn_80275AF0,
                            (dxJoint::getInfo2_fn*)fn_80275AC8,
                            dJointTypeBall };

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

// 0x80275218: dJointSetHingeAnchor
extern "C" void fn_80275218(dxJointHinge* joint, dReal x, dReal y, dReal z)
{
    setAnchors(joint, x, y, z, joint->anchor1, joint->anchor2);
    hingeComputeInitialRelativeRotation(joint);
}

// 0x80274FF8: dJointSetHingeAxis
extern "C" void fn_80274FF8(dxJointHinge* joint, dReal x, dReal y, dReal z)
{
    setAxes(joint, x, y, z, joint->axis1, joint->axis2);
    hingeComputeInitialRelativeRotation(joint);
}

// 0x80274F40: dJointSetHingeParam
extern "C" void fn_80274F40(dxJointHinge* joint, int parameter, dReal value)
{
    joint->limot.set(parameter, value);
}

dxJoint::Vtable __dhinge_vtable = { sizeof(dxJointHinge),
                            (dxJoint::init_fn*)fn_802758F4,
                            (dxJoint::getInfo1_fn*)fn_80275640,
                            (dxJoint::getInfo2_fn*)fn_802753A4,
                            dJointTypeHinge };

static void sliderInit (dxJointSlider *j)
{
    dSetZero (j->axis1,4);
    j->axis1[0] = 1;
    dSetZero (j->qrel,4);
    dSetZero (j->offset,4);
    j->limot.init (j->world);
}

extern "C" dReal dJointGetSliderPosition (dxJointSlider *joint)
{

    // get axis1 in global coordinates
    dVector3 ax1,q;
    dMULTIPLY0_331 (ax1,joint->node[0].body->R,joint->axis1);

    if (joint->node[1].body) {
        // get body2 + offset point in global coordinates
        dMULTIPLY0_331 (q,joint->node[1].body->R,joint->offset);
        for (int i=0; i<3; i++) q[i] = joint->node[0].body->pos[i] - q[i] -
                              joint->node[1].body->pos[i];
    }
    else {
        for (int i=0; i<3; i++) q[i] = joint->node[0].body->pos[i] -
                              joint->offset[i];

    }
    return dDOT(ax1,q);
}

extern "C" dReal dJointGetSliderPositionRate (dxJointSlider *joint)
{

    // get axis1 in global coordinates
    dVector3 ax1;
    dMULTIPLY0_331 (ax1,joint->node[0].body->R,joint->axis1);

    if (joint->node[1].body) {
        return dDOT(ax1,joint->node[0].body->lvel) -
            dDOT(ax1,joint->node[1].body->lvel);
    }
    else {
        return dDOT(ax1,joint->node[0].body->lvel);
    }
}

static void sliderGetInfo1 (dxJointSlider *j, dxJoint::Info1 *info)
{
    info->nub = 5;

    // see if joint is powered
    if (j->limot.fmax > 0)
        info->m = 6;        // powered slider needs an extra constraint row
    else info->m = 5;

    // see if we're at a joint limit.
    j->limot.limit = 0;
    if ((j->limot.lostop > -dInfinity || j->limot.histop < dInfinity) &&
            j->limot.lostop <= j->limot.histop) {
        // measure joint position
        dReal pos = dJointGetSliderPosition (j);
        if (pos <= j->limot.lostop) {
            j->limot.limit = 1;
            j->limot.limit_err = pos - j->limot.lostop;
            info->m = 6;
        }
        else if (pos >= j->limot.histop) {
            j->limot.limit = 2;
            j->limot.limit_err = pos - j->limot.histop;
            info->m = 6;
        }
    }
}

static void sliderGetInfo2 (dxJointSlider *joint, dxJoint::Info2 *info)
{
    int i,s = info->rowskip;
    int s3=3*s,s4=4*s;

    // pull out pos and R for both bodies. also get the `connection'
    // vector pos2-pos1.

    dReal *pos1,*pos2,*R1,*R2;
    dVector3 c;
    pos1 = joint->node[0].body->pos;
    R1 = joint->node[0].body->R;
    if (joint->node[1].body) {
        pos2 = joint->node[1].body->pos;
        R2 = joint->node[1].body->R;
        for (i=0; i<3; i++) c[i] = pos2[i] - pos1[i];
    }
    else {
        pos2 = 0;
        R2 = 0;
    }

    // 3 rows to make body rotations equal
    setFixedOrientation(joint, info, joint->qrel, 0);

    // remaining two rows. we want: vel2 = vel1 + w1 x c ... but this would
    // result in three equations, so we project along the planespace vectors
    // so that sliding along the slider axis is disregarded. for symmetry we
    // also substitute (w1+w2)/2 for w1, as w1 is supposed to equal w2.

    dVector3 ax1; // joint axis in global coordinates (unit length)
    dVector3 p,q; // plane space of ax1
    dMULTIPLY0_331 (ax1,R1,joint->axis1);
    dPlaneSpace (ax1,p,q);
    if (joint->node[1].body) {
        dVector3 tmp;
        dCROSS (tmp, = REAL(0.5) * ,c,p);
        for (i=0; i<3; i++) info->J2a[s3+i] = tmp[i];
        for (i=0; i<3; i++) info->J2a[s3+i] = tmp[i];
        dCROSS (tmp, = REAL(0.5) * ,c,q);
        for (i=0; i<3; i++) info->J2a[s4+i] = tmp[i];
        for (i=0; i<3; i++) info->J2a[s4+i] = tmp[i];
        for (i=0; i<3; i++) info->J2l[s3+i] = -p[i];
        for (i=0; i<3; i++) info->J2l[s4+i] = -q[i];
    }
    for (i=0; i<3; i++) info->J1l[s3+i] = p[i];
    for (i=0; i<3; i++) info->J1l[s4+i] = q[i];

    // compute last two elements of right hand side. we want to align the offset
    // point (in body 2's frame) with the center of body 1.
    dReal k = info->fps * info->erp;
    if (joint->node[1].body) {
        dVector3 ofs;               // offset point in global coordinates
        dMULTIPLY0_331 (ofs,R2,joint->offset);
        for (i=0; i<3; i++) c[i] += ofs[i];
        info->c[3] = k * dDOT(p,c);
        info->c[4] = k * dDOT(q,c);
    }
    else {
        dVector3 ofs;               // offset point in global coordinates
        for (i=0; i<3; i++) ofs[i] = joint->offset[i] - pos1[i];
        info->c[3] = k * dDOT(p,ofs);
        info->c[4] = k * dDOT(q,ofs);
    }

    // if the slider is powered, or has joint limits, add in the extra row
    joint->limot.addLimot (joint,info,5,ax1,0);
}

extern "C" void dJointSetSliderAxis (dxJointSlider *joint,
                                     dReal x, dReal y, dReal z)
{
    int i;
    setAxes (joint,x,y,z,joint->axis1,0);

    // compute initial relative rotation body1 -> body2, or env -> body1
    // also compute center of body1 w.r.t body 2
    if (joint->node[1].body) {
        dQMultiply1 (joint->qrel,joint->node[0].body->q,joint->node[1].body->q);
        dVector3 c;
        for (i=0; i<3; i++)
            c[i] = joint->node[0].body->pos[i] - joint->node[1].body->pos[i];
        dMULTIPLY1_331 (joint->offset,joint->node[1].body->R,c);
    }
    else {
        // set joint->qrel to the transpose of the first body's q
        joint->qrel[0] = joint->node[0].body->q[0];
        for (i=1; i<4; i++) joint->qrel[i] = -joint->node[0].body->q[i];
        for (i=0; i<3; i++) joint->offset[i] = joint->node[0].body->pos[i];
    }
}

extern "C" void dJointGetSliderAxis (dxJointSlider *joint, dVector3 result)
{
    getAxis (joint,result,joint->axis1);
}

extern "C" void dJointSetSliderParam (dxJointSlider *joint,
                                      int parameter, dReal value)
{
    joint->limot.set (parameter,value);
}

extern "C" dReal dJointGetSliderParam (dxJointSlider *joint, int parameter)
{
    return joint->limot.get (parameter);
}

extern "C" void dJointAddSliderForce (dxJointSlider *joint, dReal force)
{
    dVector3 axis;

    if (joint->flags & dJOINT_REVERSE)
        force -= force;

    getAxis (joint,axis,joint->axis1);
    axis[0] *= force;
    axis[1] *= force;
    axis[2] *= force;

    if (joint->node[0].body != 0)
        dBodyAddForce (joint->node[0].body,axis[0],axis[1],axis[2]);
    if (joint->node[1].body != 0)
        dBodyAddForce(joint->node[1].body, -axis[0], -axis[1], -axis[2]);
}

dxJoint::Vtable __dslider_vtable = {
    sizeof(dxJointSlider),
    (dxJoint::init_fn*) sliderInit,
    (dxJoint::getInfo1_fn*) sliderGetInfo1,
    (dxJoint::getInfo2_fn*) sliderGetInfo2,
    dJointTypeSlider};

//****************************************************************************
// contact

//****************************************************************************
// contact

// 0x80274F3C: contactInit
extern "C" void fn_80274F3C(dxJointContact* j)
{
    // default frictionless contact. hmmm, this info gets overwritten
    // by dJointCreateContact() anyway, so why do we bother?
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

dxJoint::Vtable __dcontact_vtable = { sizeof(dxJointContact),
                            (dxJoint::init_fn*)fn_80274F3C,
                            (dxJoint::getInfo1_fn*)fn_80274E7C,
                            (dxJoint::getInfo2_fn*)fn_80274714,
                            dJointTypeContact };

//****************************************************************************
// hinge 2. note that this joint must be attached to two bodies for it to work

static dReal measureHinge2Angle (dxJointHinge2 *joint)
{
    dVector3 a1,a2;
    dMULTIPLY0_331 (a1,joint->node[1].body->R,joint->axis2);
    dMULTIPLY1_331 (a2,joint->node[0].body->R,a1);
    dReal x = dDOT(joint->v1,a2);
    dReal y = dDOT(joint->v2,a2);
    return -dAtan2 (y,x);
}

static void hinge2Init (dxJointHinge2 *j)
{
    dSetZero (j->anchor1,4);
    dSetZero (j->anchor2,4);
    dSetZero (j->axis1,4);
    j->axis1[0] = 1;
    dSetZero (j->axis2,4);
    j->axis2[1] = 1;
    j->c0 = 0;
    j->s0 = 0;

    dSetZero (j->v1,4);
    j->v1[0] = 1;
    dSetZero (j->v2,4);
    j->v2[1] = 1;

    j->limot1.init (j->world);
    j->limot2.init (j->world);

    j->susp_erp = j->world->global_erp;
    j->susp_cfm = j->world->global_cfm;

    j->flags |= dJOINT_TWOBODIES;
}

static void hinge2GetInfo1 (dxJointHinge2 *j, dxJoint::Info1 *info)
{
    info->m = 4;
    info->nub = 4;

    // see if we're powered or at a joint limit for axis 1
    int atlimit=0;
    if ((j->limot1.lostop >= -M_PI || j->limot1.histop <= M_PI) &&
            j->limot1.lostop <= j->limot1.histop) {
        dReal angle = measureHinge2Angle (j);
        if (j->limot1.testRotationalLimit (angle)) atlimit = 1;
    }
    if (atlimit || j->limot1.fmax > 0) info->m++;

    // see if we're powering axis 2 (we currently never limit this axis)
    j->limot2.limit = 0;
    if (j->limot2.fmax > 0) info->m++;
}

// macro that computes ax1,ax2 = axis 1 and 2 in global coordinates (they are
// relative to body 1 and 2 initially) and then computes the constrained
// rotational axis as the cross product of ax1 and ax2.
// the sin and cos of the angle between axis 1 and 2 is computed, this comes
// from dot and cross product rules.

#define HINGE2_GET_AXIS_INFO(axis,sin_angle,cos_angle) \
    dVector3 ax1,ax2; \
    dMULTIPLY0_331 (ax1,joint->node[0].body->R,joint->axis1); \
    dMULTIPLY0_331 (ax2,joint->node[1].body->R,joint->axis2); \
    dCROSS (axis,=,ax1,ax2); \
    sin_angle = dSqrt (axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]); \
    cos_angle = dDOT (ax1,ax2);

static void hinge2GetInfo2 (dxJointHinge2 *joint, dxJoint::Info2 *info)
{
    // get information we need to set the hinge row
    dReal s,c;
    dVector3 q;
    HINGE2_GET_AXIS_INFO (q,s,c);
    dNormalize3 (q);              // @@@ quicker: divide q by s ?

    // set the three ball-and-socket rows (aligned to the suspension axis ax1)
    setBall2 (joint,info,joint->anchor1,joint->anchor2,ax1,joint->susp_erp);

    // set the hinge row
    int s3=3*info->rowskip;
    info->J1a[s3+0] = q[0];
    info->J1a[s3+1] = q[1];
    info->J1a[s3+2] = q[2];
    if (joint->node[1].body) {
        info->J2a[s3+0] = -q[0];
        info->J2a[s3+1] = -q[1];
        info->J2a[s3+2] = -q[2];
    }

    // compute the right hand side for the constrained rotational DOF.
    // axis 1 and axis 2 are separated by an angle `theta'. the desired
    // separation angle is theta0. sin(theta0) and cos(theta0) are recorded
    // in the joint structure. the correcting angular velocity is:
    //   |angular_velocity| = angle/time = erp*(theta0-theta) / stepsize
    //                      = (erp*fps) * (theta0-theta)
    // (theta0-theta) can be computed using the following small-angle-difference
    // approximation:
    //   theta0-theta ~= tan(theta0-theta)
    //                 = sin(theta0-theta)/cos(theta0-theta)
    //                 = (c*s0 - s*c0) / (c*c0 + s*s0)
    //                 = c*s0 - s*c0         assuming c*c0 + s*s0 ~= 1
    // where c = cos(theta), s = sin(theta)
    //       c0 = cos(theta0), s0 = sin(theta0)

    dReal k = info->fps * info->erp;
    info->c[3] = k * (joint->c0 * s - joint->s0 * c);

    // if the axis1 hinge is powered, or has joint limits, add in more stuff
    int row = 4 + joint->limot1.addLimot (joint,info,4,ax1,1);

    // if the axis2 hinge is powered, add in more stuff
    joint->limot2.addLimot (joint,info,row,ax2,1);

    // set parameter for the suspension
    info->cfm[0] = joint->susp_cfm;
}

// compute vectors v1 and v2 (embedded in body1), used to measure angle
// between body 1 and body 2

static void makeHinge2V1andV2 (dxJointHinge2 *joint)
{
    if (joint->node[0].body) {
        // get axis 1 and 2 in global coords
        dVector3 ax1,ax2,v;
        dMULTIPLY0_331 (ax1,joint->node[0].body->R,joint->axis1);
        dMULTIPLY0_331 (ax2,joint->node[1].body->R,joint->axis2);

        // don't do anything if the axis1 or axis2 vectors are zero or the same
        if ((ax1[0]==0 && ax1[1]==0 && ax1[2]==0) ||
                (ax2[0]==0 && ax2[1]==0 && ax2[2]==0) ||
                (ax1[0]==ax2[0] && ax1[1]==ax2[1] && ax1[2]==ax2[2])) return;

        // modify axis 2 so it's perpendicular to axis 1
        dReal k = dDOT(ax1,ax2);
        for (int i=0; i<3; i++) ax2[i] -= k*ax1[i];
        dNormalize3 (ax2);

        // make v1 = modified axis2, v2 = axis1 x (modified axis2)
        dCROSS (v,=,ax1,ax2);
        dMULTIPLY1_331 (joint->v1,joint->node[0].body->R,ax2);
        dMULTIPLY1_331 (joint->v2,joint->node[0].body->R,v);
    }
}

extern "C" void dJointSetHinge2Anchor (dxJointHinge2 *joint,
                                       dReal x, dReal y, dReal z)
{
    setAnchors (joint,x,y,z,joint->anchor1,joint->anchor2);
    makeHinge2V1andV2 (joint);
}

extern "C" void dJointSetHinge2Axis1 (dxJointHinge2 *joint,
                                      dReal x, dReal y, dReal z)
{
    if (joint->node[0].body) {
        dReal q[4];
        q[0] = x;
        q[1] = y;
        q[2] = z;
        q[3] = 0;
        dNormalize3 (q);
        dMULTIPLY1_331 (joint->axis1,joint->node[0].body->R,q);
        joint->axis1[3] = 0;

        // compute the sin and cos of the angle between axis 1 and axis 2
        dVector3 ax;
        HINGE2_GET_AXIS_INFO(ax,joint->s0,joint->c0);
    }
    makeHinge2V1andV2 (joint);
}

extern "C" void dJointSetHinge2Axis2 (dxJointHinge2 *joint,
                                      dReal x, dReal y, dReal z)
{
    if (joint->node[1].body) {
        dReal q[4];
        q[0] = x;
        q[1] = y;
        q[2] = z;
        q[3] = 0;
        dNormalize3 (q);
        dMULTIPLY1_331 (joint->axis2,joint->node[1].body->R,q);
        joint->axis1[3] = 0;

        // compute the sin and cos of the angle between axis 1 and axis 2
        dVector3 ax;
        HINGE2_GET_AXIS_INFO(ax,joint->s0,joint->c0);
    }
    makeHinge2V1andV2 (joint);
}

extern "C" void dJointSetHinge2Param (dxJointHinge2 *joint,
                                      int parameter, dReal value)
{
    if ((parameter & 0xff00) == 0x100) {
        joint->limot2.set (parameter & 0xff,value);
    }
    else {
        if (parameter == dParamSuspensionERP) joint->susp_erp = value;
        else if (parameter == dParamSuspensionCFM) joint->susp_cfm = value;
        else joint->limot1.set (parameter,value);
    }
}

extern "C" void dJointGetHinge2Anchor (dxJointHinge2 *joint, dVector3 result)
{
    if (joint->flags & dJOINT_REVERSE)
        getAnchor2 (joint,result,joint->anchor2);
    else
        getAnchor (joint,result,joint->anchor1);
}

extern "C" void dJointGetHinge2Anchor2 (dxJointHinge2 *joint, dVector3 result)
{
    if (joint->flags & dJOINT_REVERSE)
        getAnchor (joint,result,joint->anchor1);
    else
        getAnchor2 (joint,result,joint->anchor2);
}

extern "C" void dJointGetHinge2Axis1 (dxJointHinge2 *joint, dVector3 result)
{
    if (joint->node[0].body) {
        dMULTIPLY0_331 (result,joint->node[0].body->R,joint->axis1);
    }
}

extern "C" void dJointGetHinge2Axis2 (dxJointHinge2 *joint, dVector3 result)
{
    if (joint->node[1].body) {
        dMULTIPLY0_331 (result,joint->node[1].body->R,joint->axis2);
    }
}

extern "C" dReal dJointGetHinge2Param (dxJointHinge2 *joint, int parameter)
{
    if ((parameter & 0xff00) == 0x100) {
        return joint->limot2.get (parameter & 0xff);
    }
    else {
        if (parameter == dParamSuspensionERP) return joint->susp_erp;
        else if (parameter == dParamSuspensionCFM) return joint->susp_cfm;
        else return joint->limot1.get (parameter);
    }
}

extern "C" dReal dJointGetHinge2Angle1 (dxJointHinge2 *joint)
{
    if (joint->node[0].body) return measureHinge2Angle (joint);
    else return 0;
}

extern "C" dReal dJointGetHinge2Angle1Rate (dxJointHinge2 *joint)
{
    if (joint->node[0].body) {
        dVector3 axis;
        dMULTIPLY0_331 (axis,joint->node[0].body->R,joint->axis1);
        dReal rate = dDOT(axis,joint->node[0].body->avel);
        if (joint->node[1].body) rate -= dDOT(axis,joint->node[1].body->avel);
        return rate;
    }
    else return 0;
}

extern "C" dReal dJointGetHinge2Angle2Rate (dxJointHinge2 *joint)
{
    if (joint->node[0].body && joint->node[1].body) {
        dVector3 axis;
        dMULTIPLY0_331 (axis,joint->node[1].body->R,joint->axis2);
        dReal rate = dDOT(axis,joint->node[0].body->avel);
        if (joint->node[1].body) rate -= dDOT(axis,joint->node[1].body->avel);
        return rate;
    }
    else return 0;
}

extern "C" void dJointAddHinge2Torques (dxJointHinge2 *joint, dReal torque1, dReal torque2)
{
    dVector3 axis1, axis2;

    if (joint->node[0].body && joint->node[1].body) {
        dMULTIPLY0_331 (axis1,joint->node[0].body->R,joint->axis1);
        dMULTIPLY0_331 (axis2,joint->node[1].body->R,joint->axis2);
        axis1[0] = axis1[0] * torque1 + axis2[0] * torque2;
        axis1[1] = axis1[1] * torque1 + axis2[1] * torque2;
        axis1[2] = axis1[2] * torque1 + axis2[2] * torque2;
        dBodyAddTorque (joint->node[0].body,axis1[0],axis1[1],axis1[2]);
        dBodyAddTorque(joint->node[1].body, -axis1[0], -axis1[1], -axis1[2]);
    }
}

dxJoint::Vtable __dhinge2_vtable = {
    sizeof(dxJointHinge2),
    (dxJoint::init_fn*) hinge2Init,
    (dxJoint::getInfo1_fn*) hinge2GetInfo1,
    (dxJoint::getInfo2_fn*) hinge2GetInfo2,
    dJointTypeHinge2};

//****************************************************************************
// universal

// I just realized that the universal joint is equivalent to a hinge 2 joint with
// perfectly stiff suspension.  By comparing the hinge 2 implementation to
// the universal implementation, you may be able to improve this
// implementation (or, less likely, the hinge2 implementation).

static void universalInit (dxJointUniversal *j)
{
    dSetZero (j->anchor1,4);
    dSetZero (j->anchor2,4);
    dSetZero (j->axis1,4);
    j->axis1[0] = 1;
    dSetZero (j->axis2,4);
    j->axis2[1] = 1;
    dSetZero(j->qrel1,4);
    dSetZero(j->qrel2,4);
    j->limot1.init (j->world);
    j->limot2.init (j->world);
}

static void getUniversalAxes(dxJointUniversal *joint, dVector3 ax1, dVector3 ax2)
{
    // This says "ax1 = joint->node[0].body->R * joint->axis1"
    dMULTIPLY0_331 (ax1,joint->node[0].body->R,joint->axis1);

    if (joint->node[1].body) {
        dMULTIPLY0_331 (ax2,joint->node[1].body->R,joint->axis2);
    }
    else {
        ax2[0] = joint->axis2[0];
        ax2[1] = joint->axis2[1];
        ax2[2] = joint->axis2[2];
    }
}

static dReal getUniversalAngle1(dxJointUniversal *joint)
{
    if (joint->node[0].body) {
        // length 1 joint axis in global coordinates, from each body
        dVector3 ax1, ax2;
        dMatrix3 R;
        dQuaternion qcross, qq, qrel;

        getUniversalAxes (joint,ax1,ax2);

        // It should be possible to get both angles without explicitly
        // constructing the rotation matrix of the cross.  Basically,
        // orientation of the cross about axis1 comes from body 2,
        // about axis 2 comes from body 1, and the perpendicular
        // axis can come from the two bodies somehow.  (We don't really
        // want to assume it's 90 degrees, because in general the
        // constraints won't be perfectly satisfied, or even very well
        // satisfied.)
        //
        // However, we'd need a version of getHingeAngleFromRElativeQuat()
        // that CAN handle when its relative quat is rotated along a direction
        // other than the given axis.  What I have here works,
        // although it's probably much slower than need be.

        dRFrom2Axes(R, ax1[0], ax1[1], ax1[2], ax2[0], ax2[1], ax2[2]);
        dRtoQ (R,qcross);

        // This code is essential the same as getHingeAngle(), see the comments
        // there for details.

        // get qrel = relative rotation between node[0] and the cross
        dQMultiply1 (qq,joint->node[0].body->q,qcross);
        dQMultiply2 (qrel,qq,joint->qrel1);

        return getHingeAngleFromRelativeQuat(qrel, joint->axis1);
    }
    return 0;
}

static dReal getUniversalAngle2(dxJointUniversal *joint)
{
    if (joint->node[0].body) {
        // length 1 joint axis in global coordinates, from each body
        dVector3 ax1, ax2;
        dMatrix3 R;
        dQuaternion qcross, qq, qrel;

        getUniversalAxes (joint,ax1,ax2);

        // It should be possible to get both angles without explicitly
        // constructing the rotation matrix of the cross.  Basically,
        // orientation of the cross about axis1 comes from body 2,
        // about axis 2 comes from body 1, and the perpendicular
        // axis can come from the two bodies somehow.  (We don't really
        // want to assume it's 90 degrees, because in general the
        // constraints won't be perfectly satisfied, or even very well
        // satisfied.)
        //
        // However, we'd need a version of getHingeAngleFromRElativeQuat()
        // that CAN handle when its relative quat is rotated along a direction
        // other than the given axis.  What I have here works,
        // although it's probably much slower than need be.

        dRFrom2Axes(R, ax2[0], ax2[1], ax2[2], ax1[0], ax1[1], ax1[2]);
        dRtoQ(R, qcross);

        if (joint->node[1].body) {
            dQMultiply1 (qq, joint->node[1].body->q, qcross);
            dQMultiply2 (qrel,qq,joint->qrel2);
        }
        else {
            // pretend joint->node[1].body->q is the identity
            dQMultiply2 (qrel,qcross, joint->qrel2);
        }

        return - getHingeAngleFromRelativeQuat(qrel, joint->axis2);
    }
    return 0;
}

static void universalGetInfo1 (dxJointUniversal *j, dxJoint::Info1 *info)
{
    info->nub = 4;
    info->m = 4;

    // see if we're powered or at a joint limit.
    bool constraint1 = j->limot1.fmax > 0;
    bool constraint2 = j->limot2.fmax > 0;

    bool limiting1 = (j->limot1.lostop >= -M_PI || j->limot1.histop <= M_PI) &&
              j->limot1.lostop <= j->limot1.histop;
    bool limiting2 = (j->limot2.lostop >= -M_PI || j->limot2.histop <= M_PI) &&
              j->limot2.lostop <= j->limot2.histop;

    // We need to call testRotationLimit() even if we're motored, since it
    // records the result.
    if (limiting1 || limiting2) {
        dReal angle1, angle2;
        angle1 = getUniversalAngle1(j);
        angle2 = getUniversalAngle2(j);
        if (limiting1 && j->limot1.testRotationalLimit (angle1)) constraint1 = true;
        if (limiting2 && j->limot2.testRotationalLimit (angle2)) constraint2 = true;
    }
    if (constraint1)
        info->m++;
    if (constraint2)
        info->m++;
}

static void universalGetInfo2 (dxJointUniversal *joint, dxJoint::Info2 *info)
{
    // set the three ball-and-socket rows
    setBall (joint,info,joint->anchor1,joint->anchor2);

    // set the universal joint row. the angular velocity about an axis
    // perpendicular to both joint axes should be equal. thus the constraint
    // equation is
    //    p*w1 - p*w2 = 0
    // where p is a vector normal to both joint axes, and w1 and w2
    // are the angular velocity vectors of the two bodies.

    // length 1 joint axis in global coordinates, from each body
    dVector3 ax1, ax2;
    dVector3 ax2_temp;
    // length 1 vector perpendicular to ax1 and ax2. Neither body can rotate
    // about this.
    dVector3 p;
    dReal k;

    getUniversalAxes(joint, ax1, ax2);
    k = dDOT(ax1, ax2);
    ax2_temp[0] = ax2[0] - k*ax1[0];
    ax2_temp[1] = ax2[1] - k*ax1[1];
    ax2_temp[2] = ax2[2] - k*ax1[2];
    dCROSS(p, =, ax1, ax2_temp);
    dNormalize3(p);

    int s3=3*info->rowskip;

    info->J1a[s3+0] = p[0];
    info->J1a[s3+1] = p[1];
    info->J1a[s3+2] = p[2];

    if (joint->node[1].body) {
        info->J2a[s3+0] = -p[0];
        info->J2a[s3+1] = -p[1];
        info->J2a[s3+2] = -p[2];
    }

    // compute the right hand side of the constraint equation. set relative
    // body velocities along p to bring the axes back to perpendicular.
    // If ax1, ax2 are unit length joint axes as computed from body1 and
    // body2, we need to rotate both bodies along the axis p.  If theta
    // is the angle between ax1 and ax2, we need an angular velocity
    // along p to cover the angle erp * (theta - Pi/2) in one step:
    //
    //   |angular_velocity| = angle/time = erp*(theta - Pi/2) / stepsize
    //                      = (erp*fps) * (theta - Pi/2)
    //
    // if theta is close to Pi/2,
    // theta - Pi/2 ~= cos(theta), so
    //    |angular_velocity|  ~= (erp*fps) * (ax1 dot ax2)

    info->c[3] = info->fps * info->erp * - dDOT(ax1, ax2);

    // if the first angle is powered, or has joint limits, add in the stuff
    int row = 4 + joint->limot1.addLimot (joint,info,4,ax1,1);

    // if the second angle is powered, or has joint limits, add in more stuff
    joint->limot2.addLimot (joint,info,row,ax2,1);
}

static void universalComputeInitialRelativeRotations (dxJointUniversal *joint)
{
    if (joint->node[0].body) {
        dVector3 ax1, ax2;
        dMatrix3 R;
        dQuaternion qcross;

        getUniversalAxes(joint, ax1, ax2);

        // Axis 1.
        dRFrom2Axes(R, ax1[0], ax1[1], ax1[2], ax2[0], ax2[1], ax2[2]);
        dRtoQ(R, qcross);
        dQMultiply1 (joint->qrel1, joint->node[0].body->q, qcross);

        // Axis 2.
        dRFrom2Axes(R, ax2[0], ax2[1], ax2[2], ax1[0], ax1[1], ax1[2]);
        dRtoQ(R, qcross);
        if (joint->node[1].body) {
            dQMultiply1 (joint->qrel2, joint->node[1].body->q, qcross);
        }
        else {
            // set joint->qrel to qcross
            for (int i=0; i<4; i++) joint->qrel2[i] = qcross[i];
        }
    }
}

extern "C" void dJointSetUniversalAnchor (dxJointUniversal *joint,
                                          dReal x, dReal y, dReal z)
{
    setAnchors (joint,x,y,z,joint->anchor1,joint->anchor2);
    universalComputeInitialRelativeRotations(joint);
}

extern "C" void dJointSetUniversalAxis1 (dxJointUniversal *joint,
                                         dReal x, dReal y, dReal z)
{
    if (joint->flags & dJOINT_REVERSE)
        setAxes (joint,x,y,z,NULL,joint->axis2);
    else
        setAxes (joint,x,y,z,joint->axis1,NULL);
    universalComputeInitialRelativeRotations(joint);
}

extern "C" void dJointSetUniversalAxis2 (dxJointUniversal *joint,
                                         dReal x, dReal y, dReal z)
{
    if (joint->flags & dJOINT_REVERSE)
        setAxes (joint,x,y,z,joint->axis1,NULL);
    else
        setAxes (joint,x,y,z,NULL,joint->axis2);
    universalComputeInitialRelativeRotations(joint);
}

extern "C" void dJointGetUniversalAnchor (dxJointUniversal *joint,
                                          dVector3 result)
{
    if (joint->flags & dJOINT_REVERSE)
        getAnchor2 (joint,result,joint->anchor2);
    else
        getAnchor (joint,result,joint->anchor1);
}

extern "C" void dJointGetUniversalAnchor2 (dxJointUniversal *joint,
                                          dVector3 result)
{
    if (joint->flags & dJOINT_REVERSE)
        getAnchor (joint,result,joint->anchor1);
    else
        getAnchor2 (joint,result,joint->anchor2);
}

extern "C" void dJointGetUniversalAxis1 (dxJointUniversal *joint,
                                         dVector3 result)
{
    if (joint->flags & dJOINT_REVERSE)
        getAxis2 (joint,result,joint->axis2);
    else
        getAxis (joint,result,joint->axis1);
}

extern "C" void dJointGetUniversalAxis2 (dxJointUniversal *joint,
                                         dVector3 result)
{
    if (joint->flags & dJOINT_REVERSE)
        getAxis (joint,result,joint->axis1);
    else
        getAxis2 (joint,result,joint->axis2);
}

extern "C" void dJointSetUniversalParam (dxJointUniversal *joint,
                                     int parameter, dReal value)
{
    if ((parameter & 0xff00) == 0x100) {
        joint->limot2.set (parameter & 0xff,value);
    }
    else {
        joint->limot1.set (parameter,value);
    }
}

extern "C" dReal dJointGetUniversalParam (dxJointUniversal *joint, int parameter)
{
    if ((parameter & 0xff00) == 0x100) {
        return joint->limot2.get (parameter & 0xff);
    }
    else {
        return joint->limot1.get (parameter);
    }
}

extern "C" dReal dJointGetUniversalAngle1 (dxJointUniversal *joint)
{
    if (joint->flags & dJOINT_REVERSE)
        return getUniversalAngle2 (joint);
    else
        return getUniversalAngle1 (joint);
}

extern "C" dReal dJointGetUniversalAngle2 (dxJointUniversal *joint)
{
    if (joint->flags & dJOINT_REVERSE)
        return getUniversalAngle1 (joint);
    else
        return getUniversalAngle2 (joint);
}

extern "C" dReal dJointGetUniversalAngle1Rate (dxJointUniversal *joint)
{

    if (joint->node[0].body) {
        dVector3 axis;

        if (joint->flags & dJOINT_REVERSE)
            getAxis2 (joint,axis,joint->axis2);
        else
            getAxis (joint,axis,joint->axis1);

        dReal rate = dDOT(axis, joint->node[0].body->avel);
        if (joint->node[1].body) rate -= dDOT(axis, joint->node[1].body->avel);
        return rate;
    }
    return 0;
}

extern "C" dReal dJointGetUniversalAngle2Rate (dxJointUniversal *joint)
{

    if (joint->node[0].body) {
        dVector3 axis;

        if (joint->flags & dJOINT_REVERSE)
            getAxis (joint,axis,joint->axis1);
        else
            getAxis2 (joint,axis,joint->axis2);

        dReal rate = dDOT(axis, joint->node[0].body->avel);
        if (joint->node[1].body) rate -= dDOT(axis, joint->node[1].body->avel);
        return rate;
    }
    return 0;
}

extern "C" void dJointAddUniversalTorques (dxJointUniversal *joint, dReal torque1, dReal torque2)
{
    dVector3 axis1, axis2;

    if (joint->flags & dJOINT_REVERSE) {
        dReal temp = torque1;
        torque1 = - torque2;
        torque2 = - temp;
    }

    getAxis (joint,axis1,joint->axis1);
    getAxis2 (joint,axis2,joint->axis2);
    axis1[0] = axis1[0] * torque1 + axis2[0] * torque2;
    axis1[1] = axis1[1] * torque1 + axis2[1] * torque2;
    axis1[2] = axis1[2] * torque1 + axis2[2] * torque2;

    if (joint->node[0].body != 0)
        dBodyAddTorque (joint->node[0].body,axis1[0],axis1[1],axis1[2]);
    if (joint->node[1].body != 0)
        dBodyAddTorque(joint->node[1].body, -axis1[0], -axis1[1], -axis1[2]);
}

dxJoint::Vtable __duniversal_vtable = {
    sizeof(dxJointUniversal),
    (dxJoint::init_fn*) universalInit,
    (dxJoint::getInfo1_fn*) universalGetInfo1,
    (dxJoint::getInfo2_fn*) universalGetInfo2,
    dJointTypeUniversal};

//****************************************************************************
// angular motor

//****************************************************************************
// angular motor

static void amotorSetEulerReferenceVectors(dxJointAMotor* j);
static void amotorComputeEulerAngles(dxJointAMotor* joint, dVector3 ax[3]);
static void amotorComputeGlobalAxes(dxJointAMotor* joint, dVector3 ax[3]);

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

// 0x8027403C: amotorComputeGlobalAxes. compute the 3 axes in global coordinates
static void amotorComputeGlobalAxes(dxJointAMotor* joint, dVector3 ax[3])
{
    if (joint->mode == dAMotorEuler) {
        // special handling for euler mode
        dMULTIPLY0_331(ax[0], joint->node[0].body->R, joint->axis[0]);
        if (joint->node[1].body) {
            dMULTIPLY0_331(ax[2], joint->node[1].body->R, joint->axis[2]);
        } else {
            ax[2][0] = joint->axis[2][0];
            ax[2][1] = joint->axis[2][1];
            ax[2][2] = joint->axis[2][2];
        }
        dCROSS(ax[1], =, ax[2], ax[0]);
        dNormalize3(ax[1]);
    } else {
        for (int i = 0; i < joint->num; i++) {
            if (joint->rel[i] == 1) {
                // relative to b1
                dMULTIPLY0_331(ax[i], joint->node[0].body->R, joint->axis[i]);
            }
            if (joint->rel[i] == 2) {
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
    if (joint->node[1].body) {
        dMULTIPLY0_331(ref2, joint->node[1].body->R, joint->reference2);
    } else {
        ref2[0] = joint->reference2[0];
        ref2[1] = joint->reference2[1];
        ref2[2] = joint->reference2[2];
    }

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
    } else if (j->node[0].body) {  // jds
        dMULTIPLY1_331(j->reference1, j->node[0].body->R, j->axis[2]);
        dMULTIPLY0_331(j->reference2, j->node[0].body->R, j->axis[0]);
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

// 0x802737A4: dJointSetAMotorAxis
extern "C" void fn_802737A4(dxJointAMotor* joint, int anum, int rel, dReal x, dReal y, dReal z)
{
    if (anum < 0) {
        anum = 0;
    }
    if (anum > 2) {
        anum = 2;
    }

    // adjust rel to match the internal body order
    if (!joint->node[1].body && rel == 2) {
        rel = 1;
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

// 0x80273650: dJointSetAMotorMode
extern "C" void fn_80273650(dxJointAMotor* joint, int mode)
{
    joint->mode = mode;
    if (joint->mode == dAMotorEuler) {
        joint->num = 3;
        amotorSetEulerReferenceVectors(joint);
    }
}

dxJoint::Vtable __damotor_vtable = { sizeof(dxJointAMotor),
                            (dxJoint::init_fn*)fn_80274338,
                            (dxJoint::getInfo1_fn*)fn_80273A70,
                            (dxJoint::getInfo2_fn*)fn_80273950,
                            dJointTypeAMotor };

//****************************************************************************
// fixed joint

static void fixedInit (dxJointFixed *j)
{
    dSetZero (j->offset,4);
    dSetZero (j->qrel,4);
}

static void fixedGetInfo1 (dxJointFixed *j, dxJoint::Info1 *info)
{
    info->m = 6;
    info->nub = 6;
}

static void fixedGetInfo2 (dxJointFixed *joint, dxJoint::Info2 *info)
{
    int s = info->rowskip;

    // Three rows for orientation
    setFixedOrientation(joint, info, joint->qrel, 3);

    // Three rows for position.
    // set jacobian
    info->J1l[0] = 1;
    info->J1l[s+1] = 1;
    info->J1l[2*s+2] = 1;

    dVector3 ofs;
    dMULTIPLY0_331 (ofs,joint->node[0].body->R,joint->offset);
    if (joint->node[1].body) {
        dCROSSMAT (info->J1a,ofs,s,+,-);
        info->J2l[0] = -1;
        info->J2l[s+1] = -1;
        info->J2l[2*s+2] = -1;
    }

    // set right hand side for the first three rows (linear)
    dReal k = info->fps * info->erp;
    if (joint->node[1].body) {
        for (int j=0; j<3; j++)
            info->c[j] = k * (joint->node[1].body->pos[j] -
                        joint->node[0].body->pos[j] + ofs[j]);
    }
    else {
        for (int j=0; j<3; j++)
            info->c[j] = k * (joint->offset[j] - joint->node[0].body->pos[j]);
    }
}

extern "C" void dJointSetFixed (dxJointFixed *joint)
{
    int i;

    // This code is taken from sJointSetSliderAxis(), we should really put the
    // common code in its own function.
    // compute the offset between the bodies
    if (joint->node[0].body) {
        if (joint->node[1].body) {
            dQMultiply1 (joint->qrel,joint->node[0].body->q,joint->node[1].body->q);
            dReal ofs[4];
            for (i=0; i<4; i++) ofs[i] = joint->node[0].body->pos[i];
            for (i=0; i<4; i++) ofs[i] -= joint->node[1].body->pos[i];
            dMULTIPLY1_331 (joint->offset,joint->node[0].body->R,ofs);
        }
        else {
            // set joint->qrel to the transpose of the first body's q
            joint->qrel[0] = joint->node[0].body->q[0];
            for (i=1; i<4; i++) joint->qrel[i] = -joint->node[0].body->q[i];
            for (i=0; i<4; i++) joint->offset[i] = joint->node[0].body->pos[i];
        }
    }
}

dxJoint::Vtable __dfixed_vtable = {
    sizeof(dxJointFixed),
    (dxJoint::init_fn*) fixedInit,
    (dxJoint::getInfo1_fn*) fixedGetInfo1,
    (dxJoint::getInfo2_fn*) fixedGetInfo2,
    dJointTypeFixed};

//****************************************************************************
// null joint

static void nullGetInfo1 (dxJointNull *j, dxJoint::Info1 *info)
{
    info->m = 0;
    info->nub = 0;
}

static void nullGetInfo2 (dxJointNull *joint, dxJoint::Info2 *info)
{
    dDebug (0,"this should never get called");
}

dxJoint::Vtable __dnull_vtable = {
    sizeof(dxJointNull),
    (dxJoint::init_fn*) 0,
    (dxJoint::getInfo1_fn*) nullGetInfo1,
    (dxJoint::getInfo2_fn*) nullGetInfo2,
    dJointTypeNull};

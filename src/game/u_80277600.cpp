// u_80277600.cpp: ODE 0.5 ode.cpp (0x802775AC-0x80278D2C): world, joint and body API in reverse source order
// (dWorld* first). The dBody accessors (earlier guessed as a "scene node" API) are proven by their bodies and the
// dxBody layout (pos 0x98, lvel 0xE8, avel 0xF8, facc 0x108, tacc 0x118). Own .sdata2 pool 0x804DA2F8-0x804DA318 (0.2f, ..., FLT_MAX, 1.0f).
// Split (2026-11-26, data evidence in each file's header): u_80278D2C (math), u_80279558 (step.cpp),
// u_8027B4F8, u_8027BD5C (util.cpp), u_8027C650 (game code).

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

// ODE 0.5 ode.cpp world functions (0x802775AC-0x80277600; reverse source order, as the ODE library was
// built with -inline deferred).
// ---- ODE 0.5 types (objects.h, joint.h, obstack.h, mass.h, odemath.h), single precision.
typedef float dReal;
typedef dReal dVector3[4];
typedef dReal dVector4[4];
typedef dReal dMatrix3[4 * 3];
typedef dReal dQuaternion[4];
typedef unsigned long size_t;

#define REAL(x) (x##f)
#define dInfinity 3.4028235e+38f
#define dRecip(x) ((float)(1.0f / (x)))

void* dAlloc(size_t size);
void dFree(void* ptr, size_t size);
void dSetZero(dReal* a, int n);
int dInvertPDMatrix(const dReal* A, dReal* Ainv, int n);
extern "C" void* memcpy(void* dst, const void* src, size_t n);

struct dxWorld;
struct dxBody;
struct dxJoint;
struct dxGeom;
struct Geom;
struct Body;

struct dBase {
    void* operator new(size_t size) { return dAlloc(size); }
    void operator delete(void* ptr, size_t size) { dFree(ptr, size); }
};

struct dObStack : public dBase {
    dObStack();
    ~dObStack();
    void* alloc(int num_bytes);
    void freeAll();
    void* rewind();
    void* next(int num_bytes);

    void* first;
    void* last;
    void* current_arena;
    int current_ofs;
};

struct dMass;
extern "C" void fn_8027649C(dMass* m);   // dMassSetZero (mass.cpp)
extern "C" void fn_80276350(dMass* m, dReal themass, dReal cgx, dReal cgy, dReal cgz, dReal I11, dReal I22,
                            dReal I33, dReal I12, dReal I13, dReal I23);   // dMassSetParameters

struct dMass {
    dReal mass;
    dVector4 c;
    dMatrix3 I;

    dMass() { fn_8027649C(this); }
};

struct dObject : public dBase {
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

struct dxQuickStepParameters {
    int num_iterations;
    dReal w;
};

struct dxContactParameters {
    dReal max_vel;
    dReal min_depth;
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

struct dxWorld : public dBase {
    dxBody* firstbody;
    dxJoint* firstjoint;
    int nb, nj;
    dVector3 gravity;
    dReal global_erp;
    dReal global_cfm;
    dxAutoDisable adis;
    int adis_flag;
    dxQuickStepParameters qs;
    dxContactParameters contactp;
};

struct dJointFeedback;

struct dxJoint : public dObject {
    struct Vtable {
        int size;
        void (*init)(dxJoint* joint);
        void* getInfo1;
        void* getInfo2;
        int typenum;
    };

    Vtable* vtable;          // 0x14
    int flags;               // 0x18
    dxJointNode node[2];     // 0x1C
    dJointFeedback* feedback;   // 0x34
    dReal lambda[6];         // 0x38
};

enum {
    dJOINT_INGROUP = 1,
    dJOINT_REVERSE = 2,
    dJOINT_TWOBODIES = 4
};

struct dxJointGroup : public dBase {
    int num;
    dObStack stack;
};

typedef dxWorld* dWorldID;
typedef dxBody* dBodyID;
typedef dxJoint* dJointID;
typedef dxJointGroup* dJointGroupID;

#define dDOTpq(a, b, p, q) ((a)[0] * (b)[0] + (a)[p] * (b)[q] + (a)[2 * (p)] * (b)[2 * (q)])
#define dDOT(a, b) dDOTpq(a, b, 1, 1)
#define dMULTIPLYOP0_331(A, op, B, C) \
    (A)[0] op dDOT((B), (C));         \
    (A)[1] op dDOT((B + 4), (C));     \
    (A)[2] op dDOT((B + 8), (C));
#define dMULTIPLY0_331(A, B, C) dMULTIPLYOP0_331(A, =, B, C)
#define dCROSS(a, op, b, c)                            \
    (a)[0] op((b)[1] * (c)[2] - (b)[2] * (c)[1]);      \
    (a)[1] op((b)[2] * (c)[0] - (b)[0] * (c)[2]);      \
    (a)[2] op((b)[0] * (c)[1] - (b)[1] * (c)[0]);

static void addObjectToList(dObject* obj, dObject** first) {
    obj->next = *first;
    obj->tome = first;
    if (*first) {
        (*first)->tome = &obj->next;
    }
    (*first) = obj;
}

static void removeObjectFromList(dObject* obj) {
    if (obj->next) {
        obj->next->tome = obj->tome;
    }
    *(obj->tome) = obj->next;
    // safeguard
    obj->next = 0;
    obj->tome = 0;
}

static void removeJointReferencesFromAttachedBodies(dxJoint* j) {
    for (int i = 0; i < 2; i++) {
        dxBody* body = j->node[i].body;
        if (body) {
            dxJointNode* n = body->firstjoint;
            dxJointNode* last = 0;
            while (n) {
                if (n->joint == j) {
                    if (last) {
                        last->next = n->next;
                    } else {
                        body->firstjoint = n->next;
                    }
                    break;
                }
                last = n;
                n = n->next;
            }
        }
    }
    j->node[0].body = 0;
    j->node[0].next = 0;
    j->node[1].body = 0;
    j->node[1].next = 0;
}

static void initObject(dObject* obj, dxWorld* w) {
    obj->world = w;
    obj->next = 0;
    obj->tome = 0;
    obj->userdata = 0;
    obj->tag = 0;
}

typedef void dstepper_fn_t(dxWorld* world, void* body, int nb, void* joint, int nj, float stepsize);
extern "C" void fn_80279558(void);  // dInternalStepIsland (step.cpp)
extern "C" void fn_8027BD5C(void);  // dxProcessIslands (util.cpp)
void dxProcessIslands(dxWorld* world, float stepsize, dstepper_fn_t* stepper);
void dInternalStepIsland(dxWorld* world, void* body, int nb, void* joint, int nj, float stepsize);

void dWorldSetQuickStepNumIterations(dWorldID w, int num) {
    w->qs.num_iterations = num;
}

void dWorldSetAutoDisableFlag(dWorldID w, int do_auto_disable) {
    w->adis_flag = (do_auto_disable != 0);
}

void dWorldStep(dWorldID w, float stepsize) {
    dxProcessIslands(w, stepsize, &dInternalStepIsland);
}

void dWorldSetGravity(dWorldID w, float x, float y, float z) {
    w->gravity[0] = x;
    w->gravity[1] = y;
    w->gravity[2] = z;
}

extern "C" void fn_8027337C(int num, const char* msg, ...);   // dMessage (error.cpp)

// dWorldDestroy
extern "C" void fn_80277600(dxWorld* w) {
    // delete all bodies and joints
    dxBody *nextb, *b = w->firstbody;
    while (b) {
        nextb = (dxBody*)b->next;
        delete b;
        b = nextb;
    }
    dxJoint *nextj, *j = w->firstjoint;
    while (j) {
        nextj = (dxJoint*)j->next;
        if (j->flags & dJOINT_INGROUP) {
            // the joint is part of a group, so "deactivate" it instead
            j->world = 0;
            j->node[0].body = 0;
            j->node[0].next = 0;
            j->node[1].body = 0;
            j->node[1].next = 0;
            fn_8027337C(0, "warning: destroying world containing grouped joints");
        } else {
            size_t sz = j->vtable->size;
            dFree(j, sz);
        }
        j = nextj;
    }
    delete w;
}

// dWorldCreate
extern "C" dxWorld* fn_802776D4(void) {
    dxWorld* w = new dxWorld;
    w->firstbody = 0;
    w->firstjoint = 0;
    w->nb = 0;
    w->nj = 0;
    dSetZero(w->gravity, 4);
    w->global_erp = REAL(0.2);
    w->global_cfm = 1e-5f;

    w->adis.linear_threshold = REAL(0.001) * REAL(0.001);   // (magnitude squared)
    w->adis.angular_threshold = REAL(0.001) * REAL(0.001);  // (magnitude squared)
    w->adis.idle_steps = 10;
    w->adis.idle_time = 0;
    w->adis_flag = 0;

    w->qs.num_iterations = 20;
    w->qs.w = REAL(1.3);

    w->contactp.max_vel = dInfinity;
    w->contactp.min_depth = 0;

    return w;
}

// dAreConnectedExcluding
extern "C" int fn_80277778(dxBody* b1, dxBody* b2, int joint_type) {
    // look through b1's neighbour list for b2
    for (dxJointNode* n = b1->firstjoint; n; n = n->next) {
        if (joint_type != n->joint->vtable->typenum && n->body == b2) {
            return 1;
        }
    }
    return 0;
}

// dJointGetBody
extern "C" dxBody* fn_802777BC(dxJoint* joint, int index) {
    if (index == 0 || index == 1) {
        if (joint->flags & dJOINT_REVERSE) {
            return joint->node[1 - index].body;
        } else {
            return joint->node[index].body;
        }
    } else {
        return 0;
    }
}

// dJointAttach
extern "C" void fn_80277804(dxJoint* joint, dxBody* body1, dxBody* body2) {
    // remove any existing body attachments
    if (joint->node[0].body || joint->node[1].body) {
        removeJointReferencesFromAttachedBodies(joint);
    }

    // if a body is zero, make sure that it is body2, so 0 --> node[1].body
    if (body1 == 0) {
        body1 = body2;
        body2 = 0;
        joint->flags |= dJOINT_REVERSE;
    } else {
        joint->flags &= (~dJOINT_REVERSE);
    }

    // attach to new bodies
    joint->node[0].body = body1;
    joint->node[1].body = body2;
    if (body1) {
        joint->node[1].next = body1->firstjoint;
        body1->firstjoint = &joint->node[1];
    } else {
        joint->node[1].next = 0;
    }
    if (body2) {
        joint->node[0].next = body2->firstjoint;
        body2->firstjoint = &joint->node[0];
    } else {
        joint->node[0].next = 0;
    }
}

extern "C" void* __alloca(unsigned int size);
#define EFFICIENT_ALIGNMENT 16
#define dEFFICIENT_SIZE(x) ((((x)-1) | (EFFICIENT_ALIGNMENT - 1)) + 1)
#define dALLOCA16(n) ((char*)dEFFICIENT_SIZE(((size_t)(__alloca((n) + (EFFICIENT_ALIGNMENT - 1))))))
#define ALLOCA dALLOCA16

// dJointGroupEmpty
extern "C" void fn_80277958(dJointGroupID group) {
    // the joints in this group are detached starting from the most recently
    // added (at the top of the stack). this helps ensure that the various
    // linked lists are not traversed too much, as the joints will hopefully
    // be at the start of those lists.
    // if any group joints have their world pointer set to 0, their world was
    // previously destroyed. no special handling is required for these joints.

    int i;
    dxJoint** jlist = (dxJoint**)ALLOCA(group->num * sizeof(dxJoint*));
    dxJoint* j = (dxJoint*)group->stack.rewind();
    for (i = 0; i < group->num; i++) {
        jlist[i] = j;
        j = (dxJoint*)(group->stack.next(j->vtable->size));
    }
    for (i = group->num - 1; i >= 0; i--) {
        if (jlist[i]->world) {
            removeJointReferencesFromAttachedBodies(jlist[i]);
            removeObjectFromList(jlist[i]);
            jlist[i]->world->nj--;
        }
    }
    group->num = 0;
    group->stack.freeAll();
}

// dJointGroupDestroy
extern "C" void fn_80277B38(dJointGroupID group) {
    fn_80277958(group);
    delete group;
}

// dJointGroupCreate
extern "C" dxJointGroup* fn_80277D38(int max_size) {
    dxJointGroup* group = new dxJointGroup;
    group->num = 0;
    return group;
}

static dxJoint* createJoint(dWorldID w, dJointGroupID group, dxJoint::Vtable* vtable) {
    dxJoint* j;
    if (group) {
        j = (dxJoint*)group->stack.alloc(vtable->size);
        group->num++;
    } else {
        j = (dxJoint*)dAlloc(vtable->size);
    }

    initObject(j, w);
    j->vtable = 0;
    j->flags = 0;
    j->node[0].joint = j;
    j->node[0].body = 0;
    j->node[0].next = 0;
    j->node[1].joint = j;
    j->node[1].body = 0;
    j->node[1].next = 0;
    dSetZero(j->lambda, 6);
    addObjectToList(j, (dObject**)&w->firstjoint);
    w->nj++;

    j->vtable = vtable;
    if (group) {
        j->flags |= dJOINT_INGROUP;
    }
    if (vtable->init) {
        vtable->init(j);
    }
    j->feedback = 0;
    return j;
}

extern dxJoint::Vtable lbl_803C0514;
extern dxJoint::Vtable lbl_803C0500;
extern dxJoint::Vtable lbl_803C04EC;
extern dxJoint::Vtable lbl_803C04D8;

// dJointCreate* (joint type by vtable; guess: a later joint type than contact)
extern "C" dxJoint* fn_80277D80(dWorldID w, dJointGroupID group) {
    return createJoint(w, group, &lbl_803C0514);
}

// dContact (0.5 contact.h, as the copy size in retail).
struct dContactBytes {
    int data[0x58 / 4];
};

struct dxJointContact : public dxJoint {
    int the_m;
    dContactBytes contact;
};

// dJointCreateContact
extern "C" dxJoint* fn_80277EC8(dWorldID w, dJointGroupID group, const dContactBytes* c) {
    dxJointContact* j = (dxJointContact*)createJoint(w, group, &lbl_803C0500);
    j->contact = *c;
    return j;
}

extern "C" dxJoint* fn_802780CC(dWorldID w, dJointGroupID group) {
    return createJoint(w, group, &lbl_803C04EC);
}

extern "C" dxJoint* fn_80278214(dWorldID w, dJointGroupID group) {
    return createJoint(w, group, &lbl_803C04D8);
}

void dBodySetGravityMode(dBodyID b, int mode) {
    if (mode) {
        b->flags &= ~dxBodyNoGravity;
    } else {
        b->flags |= dxBodyNoGravity;
    }
}

int dBodyIsEnabled(dBodyID b) {
    return ((b->flags & dxBodyDisabled) == 0);
}

void dBodyDisable(dBodyID b) {
    b->flags |= dxBodyDisabled;
}

void dBodyEnable(dBodyID b) {
    b->flags &= ~dxBodyDisabled;
    b->adis_stepsleft = b->adis.idle_steps;
    b->adis_timeleft = b->adis.idle_time;
}

dJointID dBodyGetJoint(dBodyID b, int index) {
    int i = 0;
    for (dxJointNode* n = b->firstjoint; n; n = n->next, i++) {
        if (i == index) {
            return n->joint;
        }
    }
    return 0;
}

int dBodyGetNumJoints(dBodyID b) {
    int count = 0;
    for (dxJointNode* n = b->firstjoint; n; n = n->next, count++)
        ;
    return count;
}

// dBodyAddRelForceAtRelPos
extern "C" void fn_8027841C(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz, dBodyID b) {
    dVector3 frel, prel, f, p;
    frel[0] = fx;
    frel[1] = fy;
    frel[2] = fz;
    frel[3] = 0;
    prel[0] = px;
    prel[1] = py;
    prel[2] = pz;
    prel[3] = 0;
    dMULTIPLY0_331(f, b->R, frel);
    dMULTIPLY0_331(p, b->R, prel);
    b->facc[0] += f[0];
    b->facc[1] += f[1];
    b->facc[2] += f[2];
    dCROSS(b->tacc, +=, p, f);
}

// dBodyAddRelForceAtPos
extern "C" void fn_80278534(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz, dBodyID b) {
    dVector3 frel, f;
    frel[0] = fx;
    frel[1] = fy;
    frel[2] = fz;
    frel[3] = 0;
    dMULTIPLY0_331(f, b->R, frel);
    b->facc[0] += f[0];
    b->facc[1] += f[1];
    b->facc[2] += f[2];
    dVector3 q;
    q[0] = px - b->pos[0];
    q[1] = py - b->pos[1];
    q[2] = pz - b->pos[2];
    dCROSS(b->tacc, +=, q, f);
}

// dBodyAddForceAtRelPos
extern "C" void fn_802785F8(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz, dBodyID b) {
    dVector3 prel, f, p;
    f[0] = fx;
    f[1] = fy;
    f[2] = fz;
    f[3] = 0;
    prel[0] = px;
    prel[1] = py;
    prel[2] = pz;
    prel[3] = 0;
    dMULTIPLY0_331(p, b->R, prel);
    b->facc[0] += f[0];
    b->facc[1] += f[1];
    b->facc[2] += f[2];
    dCROSS(b->tacc, +=, p, f);
}

// dBodyAddForceAtPos
extern "C" void fn_802786A4(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz, dBodyID b) {
    b->facc[0] += fx;
    b->facc[1] += fy;
    b->facc[2] += fz;
    dVector3 f, q;
    f[0] = fx;
    f[1] = fy;
    f[2] = fz;
    q[0] = px - b->pos[0];
    q[1] = py - b->pos[1];
    q[2] = pz - b->pos[2];
    dCROSS(b->tacc, +=, q, f);
}

// dBodyAddRelTorque
extern "C" void fn_80278720(dReal fx, dReal fy, dReal fz, dBodyID b) {
    dVector3 t1, t2;
    t1[0] = fx;
    t1[1] = fy;
    t1[2] = fz;
    t1[3] = 0;
    dMULTIPLY0_331(t2, b->R, t1);
    b->tacc[0] += t2[0];
    b->tacc[1] += t2[1];
    b->tacc[2] += t2[2];
}

// dBodyAddRelForce
extern "C" void fn_80278790(dReal fx, dReal fy, dReal fz, dBodyID b) {
    dVector3 t1, t2;
    t1[0] = fx;
    t1[1] = fy;
    t1[2] = fz;
    t1[3] = 0;
    dMULTIPLY0_331(t2, b->R, t1);
    b->facc[0] += t2[0];
    b->facc[1] += t2[1];
    b->facc[2] += t2[2];
}

void dBodyAddTorque(dBodyID b, dReal fx, dReal fy, dReal fz) {
    b->tacc[0] += fx;
    b->tacc[1] += fy;
    b->tacc[2] += fz;
}

void dBodyAddForce(dBodyID b, dReal fx, dReal fy, dReal fz) {
    b->facc[0] += fx;
    b->facc[1] += fy;
    b->facc[2] += fz;
}

extern "C" void fn_8027BD20(dReal* R);   // dRSetIdentity (rotation.cpp)

// dBodySetMass
extern "C" void fn_80278850(dBodyID b, const dMass* mass) {
    memcpy(&b->mass, mass, sizeof(dMass));
    if (dInvertPDMatrix(b->mass.I, b->invI, 3) == 0) {
        fn_8027BD20(b->invI);
    }
    b->invMass = dRecip(b->mass.mass);
}

const dReal* dBodyGetAngularVel(dBodyID b) {
    return b->avel;
}

const dReal* dBodyGetLinearVel(dBodyID b) {
    return b->lvel;
}

void dBodySetAngularVel(dBodyID b, dReal x, dReal y, dReal z) {
    b->avel[0] = x;
    b->avel[1] = y;
    b->avel[2] = z;
}

void dBodySetLinearVel(dBodyID b, dReal x, dReal y, dReal z) {
    b->lvel[0] = x;
    b->lvel[1] = y;
    b->lvel[2] = z;
}

extern "C" void fn_8027B570(dReal* q, const dReal* R);   // dQfromR (rotation.cpp)
extern "C" void fn_80278FE0(dReal* a);                   // dNormalize4 (odemath.cpp)
extern "C" void fn_8027BAA0(dReal* R, const dReal* q);   // dRfromQ (rotation.cpp)
extern "C" void fn_8026D3A0(dxGeom* g);                  // dGeomMoved
dxGeom* GeomGetNext(Geom* g);                             // dGeomGetBodyNext
void GeomSetBody(Geom* g, Body* b);                       // dGeomSetBody

// dBodySetRotation
extern "C" void fn_802788E4(dBodyID b, const dReal* R) {
    dQuaternion q;
    fn_8027B570(q, R);
    fn_80278FE0(q);
    b->q[0] = q[0];
    b->q[1] = q[1];
    b->q[2] = q[2];
    b->q[3] = q[3];
    fn_8027BAA0(b->R, b->q);

    // notify all attached geoms that this body has moved
    for (dxGeom* geom = b->geom; geom; geom = GeomGetNext((Geom*)geom)) {
        fn_8026D3A0(geom);
    }
}

// dBodySetPosition
extern "C" void fn_8027896C(dReal x, dReal y, dReal z, dBodyID b) {
    b->pos[0] = x;
    b->pos[1] = y;
    b->pos[2] = z;

    // notify all attached geoms that this body has moved
    for (dxGeom* geom = b->geom; geom; geom = GeomGetNext((Geom*)geom)) {
        fn_8026D3A0(geom);
    }
}

void* dBodyGetData(dBodyID b) {
    return b->userdata;
}

void dBodySetData(dBodyID b, void* data) {
    b->userdata = data;
}

// dBodyDestroy
extern "C" void fn_802789D0(dxBody* b) {
    // all geoms that link to this body must be notified that the body is about
    // to disappear. note that the call to dGeomSetBody(geom,0) will result in
    // dGeomGetBodyNext() returning 0 for the body, so we must get the next body
    // before setting the body to 0.
    dxGeom* next_geom = 0;
    for (dxGeom* geom = b->geom; geom; geom = next_geom) {
        next_geom = GeomGetNext((Geom*)geom);
        GeomSetBody((Geom*)geom, 0);
    }

    // detach all neighbouring joints, then delete this body.
    dxJointNode* n = b->firstjoint;
    while (n) {
        // sneaky trick to speed up removal of joint references (black magic)
        n->joint->node[(n == n->joint->node)].body = 0;

        dxJointNode* next = n->next;
        n->next = 0;
        removeJointReferencesFromAttachedBodies(n->joint);
        n = next;
    }
    removeObjectFromList(b);
    b->world->nb--;
    delete b;
}

// dBodyCreate
extern "C" dxBody* fn_80278B70(dxWorld* w) {
    dxBody* b = new dxBody;
    initObject(b, w);
    b->firstjoint = 0;
    b->flags = 0;
    b->geom = 0;
    fn_80276350(&b->mass, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0);
    dSetZero(b->invI, 4 * 3);
    b->invI[0] = 1;
    b->invI[5] = 1;
    b->invI[10] = 1;
    b->invMass = 1;
    dSetZero(b->pos, 4);
    dSetZero(b->q, 4);
    b->q[0] = 1;
    fn_8027BD20(b->R);
    dSetZero(b->lvel, 4);
    dSetZero(b->avel, 4);
    dSetZero(b->facc, 4);
    dSetZero(b->tacc, 4);
    dSetZero(b->finite_rot_axis, 4);
    addObjectToList(b, (dObject**)&w->firstbody);
    w->nb++;

    // set auto-disable parameters (dBodySetAutoDisableDefaults, inlined)
    dWorldID world = b->world;
    b->adis = world->adis;
    if (!world->adis_flag) {
        b->flags &= ~dxBodyAutoDisable;
    } else {
        b->flags |= dxBodyAutoDisable;
    }
    b->adis_stepsleft = b->adis.idle_steps;
    b->adis_timeleft = b->adis.idle_time;

    return b;
}


// u_8026D428.cpp (0x8026D428-0x80272060): the ODE standard geometry classes and colliders (sphere, box,
// capsule, plane, ray; ODE's collision_std.cpp). Split from u_8026C200.
// Evidence: its own .sdata2 pool 0x804DA1D0-0x804DA224 (-FLT_MAX/FLT_MAX again at 0x804DA200 although
// u_8026C200 has them at 0x804DA1C0; 0.5f/-1/1/2 again in u_80272060); its class vtables lbl_803C0498/04AC/04C0;
// the out-of-line helpers MatMulVec/SqrtFloat/MatTransMulVec used only here close the file.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef signed long s32;

struct Geom;
struct Body;
struct Space;
struct ContactGeom;

typedef int (*ColliderFn)(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
typedef void (*SpaceCallback)(void* data, Geom* o1, Geom* o2);

// One contact point produced by a collider (dContactGeom).
struct ContactGeom {
    float pos[4];     // 0x00
    float normal[3];  // 0x10
    float pad1C;      // 0x1c
    float depth;      // 0x20
    Geom* g1;         // 0x24
    Geom* g2;         // 0x28
};

// Solver memory, used by the classes' operator new/delete.
void* dAlloc(u32 size);
void dFree(void* block, u32 size);
void dSetZero(float* p, int n);

enum { GEOM_HASH_SPACE = 9 };
enum { GEOM_DIRTY = 1, GEOM_AABB_BAD = 2 };

// The geometry base class (dxGeom). Its constructor, destructor and default AABB test
// live in the neighbouring unit.
struct Geom {
    int type;           // 0x00
    u32 flags;          // 0x04
    void* userData;     // 0x08
    Body* body;         // 0x0c
    Geom* bodyNext;     // 0x10  next geometry of the same body
    float* pos;         // 0x14
    float* rot;         // 0x18
    Geom* next;         // 0x1c  next geometry of the same space
    Geom** tome;        // 0x20  pointer back to this geometry in the list
    Space* parentSpace; // 0x24
    float aabb[6];      // 0x28
    u32 categoryBits;   // 0x40
    u32 collideBits;    // 0x44

    Geom(Space* space, int placeable);
    virtual ~Geom();                                  // 0x08
    virtual void computeAABB() = 0;                   // 0x0c
    virtual int AABBTest(Geom* other, float* aabb);   // 0x10

    void* operator new(u32 size) { return dAlloc(size); }
    void operator delete(void* p, u32 size) { dFree(p, size); }
};

// A collision space (dxSpace): a geometry that holds other geometries.
struct Space : Geom {
    int count;          // 0x4c
    Geom* first;        // 0x50
    int cleanup;        // 0x54
    int currentIndex;   // 0x58
    Geom* currentGeom;  // 0x5c
    int lockCount;      // 0x60

    Space(Space* parent) : Geom(parent, 0)
    {
        count = 0;
        first = 0;
        cleanup = 1;
        currentIndex = 0;
        currentGeom = 0;
        lockCount = 0;
    }
    virtual ~Space();                                                   // 0x08
    virtual void computeAABB();                                         // 0x0c
    virtual Geom* getGeom(int i);                                       // 0x14
    virtual void add(Geom* g, int placeable);                           // 0x18
    virtual void remove(Geom* g);                                       // 0x1c
    virtual void dirty(Geom* g);                                        // 0x20
    virtual void cleanGeoms() = 0;                                      // 0x24
    virtual void collide(void* data, SpaceCallback cb) = 0;            // 0x28
    virtual void collide2(void* data, Geom* g, SpaceCallback cb) = 0; // 0x2c
};

// The hash space (dxHashSpace): buckets geometries by size level, then cells.
struct HashSpace : Space {
    int minLevel;  // 0x64
    int maxLevel;  // 0x68

    HashSpace(Space* parent) : Space(parent)
    {
        type = GEOM_HASH_SPACE;
        minLevel = -3;
        maxLevel = 10;
    }
    virtual void cleanGeoms();
    virtual void collide(void* data, SpaceCallback cb);
    virtual void collide2(void* data, Geom* g, SpaceCallback cb);
};

extern "C" {
void* memcpy(void* dst, const void* src, unsigned long n);
void* memset(void* dst, int c, unsigned long n);
}

// Hash-space sizes per level (bucket counts).

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

// Single-precision square root: reciprocal-square-root estimate refined with
// three Newton steps. Negative inputs and NaNs give NaN.
static inline float SqrtF(float x)
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

// The game's MSL fabs/fabsf (inline in its math.h).
static inline double _fabs(double x)
{
    return __fabs(x);
}

static inline float FAbs(float x)
{
    return (float)_fabs((double)x);
}

// Collider helpers further down in this file / in the neighbouring unit.
extern "C" void fn_8027914C(float* v);  // Normalize3 (neighbouring unit)

// Flags a geometry and every space above it as moved (dGeomMoved).
static inline void GeomMoved(Geom* g)
{
    Space* parent = g->parentSpace;

    while (parent != 0 && (g->flags & GEOM_DIRTY) == 0) {
        g->flags |= GEOM_DIRTY | GEOM_AABB_BAD;
        parent->dirty(g);
        g = parent;
        parent = parent->parentSpace;
    }
    for (; g != 0; g = g->parentSpace) {
        g->flags |= GEOM_DIRTY | GEOM_AABB_BAD;
    }
}

// Unlinks a geometry from its space's list.
static inline void GeomUnlink(Geom* g)
{
    if (g->next != 0) {
        g->next->tome = g->tome;
    }
    *g->tome = g->next;
}

// Pushes a geometry on the head of a list.
static inline void GeomLink(Geom* g, Geom** list)
{
    g->next = *list;
    g->tome = list;
    if (*list != 0) {
        (*list)->tome = &g->next;
    }
    *list = g;
}


// A ray geometry: the base plus its length.
struct RayGeom : Geom {
    float length;  // 0x4c
};

// A plane geometry: the base plus the plane equation a*x + b*y + c*z = d.
struct PlaneGeom : Geom {
    float a;  // 0x4c
    float b;  // 0x50
    float c;  // 0x54
    float d;  // 0x58
};

// 0x8026D428: ray against plane (RayPlaneCollide)
extern "C" int fn_8026D428(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    RayGeom* ray = (RayGeom*)o1;
    PlaneGeom* plane = (PlaneGeom*)o2;
    float* pos = ray->pos;
    float dp = plane->a * pos[0] + plane->b * pos[1] + plane->c * pos[2];
    float alpha = plane->d - dp;
    float nsign = (alpha > 0.0f) ? -1.0f : 1.0f;
    float* rot = ray->rot;
    float k = plane->a * rot[2] + plane->b * rot[6] + plane->c * rot[10];
    float t;

    if (k == 0.0f) {
        return 0;
    }
    t = alpha / k;
    if (t < 0.0f || t > ray->length) {
        return 0;
    }
    contact->pos[0] = ray->pos[0] + t * ray->rot[2];
    contact->pos[1] = ray->pos[1] + t * ray->rot[6];
    contact->pos[2] = ray->pos[2] + t * ray->rot[10];
    contact->normal[0] = nsign * plane->a;
    contact->normal[1] = nsign * plane->b;
    contact->normal[2] = nsign * plane->c;
    contact->depth = t;
    contact->g1 = o1;
    contact->g2 = o2;
    return 1;
}

// TODO (not drafted yet, 0x518 bytes at 0x8026D538): ray against box
extern "C" int fn_8026D538(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// TODO (not drafted yet, 0x45C bytes at 0x8026DA50): ray against capsule
extern "C" int fn_8026DA50(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// The concrete geometry classes (dxSphere, dxCCylinder, dxBox); their constructors are
// inlined into the dCreate* functions below.
struct SphereGeom : Geom {
    float radius;  // 0x4c

    SphereGeom(Space* space, float r) : Geom(space, 1)
    {
        type = 0;
        radius = r;
    }
    virtual ~SphereGeom();
    virtual void computeAABB();
};

// A capsule geometry (capped cylinder): the base plus radius and length.
struct CapsuleGeom : Geom {
    float radius;  // 0x4c
    float lz;      // 0x50

    CapsuleGeom(Space* space, float r, float l) : Geom(space, 1)
    {
        type = 2;
        radius = r;
        lz = l;
    }
    virtual ~CapsuleGeom();
    virtual void computeAABB();
};

// A box geometry: the base plus its side lengths.
struct BoxGeom : Geom {
    float side[4];  // 0x4c (dVector3 has a padding word)

    BoxGeom(Space* space, float lx, float ly, float lz) : Geom(space, 1)
    {
        type = 1;
        side[0] = lx;
        side[1] = ly;
        side[2] = lz;
    }
    virtual ~BoxGeom();
    virtual void computeAABB();
};

int RaySphereTest(float radius, RayGeom* ray, float* center, ContactGeom* contact, int inside);

// 0x8026DEAC: ray against sphere (RaySphereCollide)
extern "C" int fn_8026DEAC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    RayGeom* ray = (RayGeom*)o1;
    SphereGeom* sphere = (SphereGeom*)o2;

    contact->g1 = o1;
    contact->g2 = o2;
    return RaySphereTest(sphere->radius, ray, sphere->pos, contact, 0);
}

// 0x8026DEE8: ray against a sphere given by centre and radius
int RaySphereTest(float radius, RayGeom* ray, float* center, ContactGeom* contact, int inside)
{
    float* pos = ray->pos;
    float* rot = ray->rot;
    float qy = pos[1] - center[1];
    float qx = pos[0] - center[0];
    float qz = pos[2] - center[2];
    float b = qz * rot[10] + qx * rot[2] + qy * rot[6];
    float c = -(radius * radius - (qz * qz + qx * qx + qy * qy));
    float k = b * b - c;
    float alpha;
    float sign;

    if (k < 0.0f) {
        return 0;
    }
    k = SqrtF(k);
    if (inside == 0 || c < 0.0f) {
        alpha = -b - k;
        if (alpha < 0.0f) {
            alpha = -b + k;
            if (alpha < 0.0f) {
                return 0;
            }
        }
    } else {
        alpha = -b + k;
        if (alpha < 0.0f) {
            return 0;
        }
    }
    if (alpha <= ray->length) {
        contact->pos[0] = alpha * ray->rot[2] + ray->pos[0];
        contact->pos[1] = alpha * ray->rot[6] + ray->pos[1];
        contact->pos[2] = alpha * ray->rot[10] + ray->pos[2];
        sign = 1.0f;
        if (c < 0.0f || inside != 0) {
            sign = -1.0f;
        }
        contact->normal[0] = sign * (contact->pos[0] - center[0]);
        contact->normal[1] = sign * (contact->pos[1] - center[1]);
        contact->normal[2] = sign * (contact->pos[2] - center[2]);
        fn_8027914C(contact->normal);
        contact->depth = alpha;
        return 1;
    }
    return 0;
}

// TODO (not drafted yet, 0x304 bytes at 0x8026E194): ray against cylinder
extern "C" int fn_8026E194(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// TODO (not drafted yet, 0x454 bytes at 0x8026E498): box against sphere / capsule family
extern "C" int fn_8026E498(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// Closest points between a line segment and a box (dClosestLineBoxPoints); later in this unit.
void ClosestLineBoxPoints(float* p1, float* p2, float* c, float* R, float* side, float* lret, float* bret);
// Collides two spheres (dCollideSpheres); later in this unit.
int CollideSpheres(float r1, float r2, float* p1, float* p2, ContactGeom* contact);

// 0x8026E8EC: capsule against box (CapsuleBoxCollide)
extern "C" int fn_8026E8EC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    CapsuleGeom* cap = (CapsuleGeom*)o1;
    BoxGeom* box = (BoxGeom*)o2;
    float* pos;
    float* rot;
    float half;
    float radius;
    float p1[3];
    float p2[3];
    float lret[4];
    float bret[4];

    contact->g1 = o1;
    contact->g2 = o2;
    rot = o1->rot;
    pos = o1->pos;
    half = 0.5f * cap->lz;
    p1[0] = half * rot[2] + pos[0];
    p2[0] = -(half * rot[2] - pos[0]);
    p1[1] = half * rot[6] + pos[1];
    p2[1] = -(half * rot[6] - pos[1]);
    p1[2] = half * rot[10] + pos[2];
    p2[2] = -(half * rot[10] - pos[2]);
    radius = cap->radius;
    ClosestLineBoxPoints(p1, p2, o2->pos, o2->rot, box->side, lret, bret);
    return CollideSpheres(radius, 0.0f, lret, bret, contact);
}

// 0x8026E9C8: capsule against sphere (CapsuleSphereCollide)
extern "C" int fn_8026E9C8(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    CapsuleGeom* cap = (CapsuleGeom*)o1;
    SphereGeom* sphere = (SphereGeom*)o2;
    float half;
    float alpha;
    float p[3];
    float* cpos;
    float* spos;
    float* rot;

    half = 0.5f * cap->lz;
    contact->g1 = o1;
    contact->g2 = o2;
    cpos = o1->pos;
    spos = o2->pos;
    rot = o1->rot;
    alpha = rot[10] * (spos[2] - cpos[2]) + rot[2] * (spos[0] - cpos[0]) + rot[6] * (spos[1] - cpos[1]);
    if (alpha > half) {
        alpha = half;
    }
    if (alpha < -half) {
        alpha = -half;
    }
    p[0] = alpha * rot[2] + cpos[0];
    p[1] = alpha * o1->rot[6] + o1->pos[1];
    p[2] = alpha * o1->rot[10] + o1->pos[2];
    return CollideSpheres(cap->radius, sphere->radius, p, o2->pos, contact);
}

// TODO (not drafted yet, 0x770 bytes at 0x8026EAAC): capsule against capsule
extern "C" int fn_8026EAAC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// TODO (not drafted yet, 0x2B4 bytes at 0x8026F21C): capsule against plane
extern "C" int fn_8026F21C(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// 0x8026F4D0: sphere against plane (SpherePlaneCollide)
extern "C" int fn_8026F4D0(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    SphereGeom* sphere = (SphereGeom*)o1;
    PlaneGeom* plane = (PlaneGeom*)o2;
    float* pos;
    float depth;

    contact->g1 = o1;
    contact->g2 = o2;
    pos = o1->pos;
    depth = sphere->radius + (plane->d - (pos[2] * plane->c + pos[0] * plane->a + pos[1] * plane->b));
    if (depth >= 0.0f) {
        contact->normal[0] = plane->a;
        contact->normal[1] = plane->b;
        contact->normal[2] = plane->c;
        contact->pos[0] = -(plane->a * sphere->radius - o1->pos[0]);
        contact->pos[1] = -(plane->b * sphere->radius - o1->pos[1]);
        contact->pos[2] = -(plane->c * sphere->radius - o1->pos[2]);
        contact->depth = depth;
        return 1;
    }
    return 0;
}

// TODO (not drafted yet, 0x468 bytes at 0x8026F590): sphere against box
extern "C" int fn_8026F590(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// 0x8026F9F8: sphere against sphere (SphereSphereCollide)
extern "C" int fn_8026F9F8(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    SphereGeom* s1 = (SphereGeom*)o1;
    SphereGeom* s2 = (SphereGeom*)o2;

    contact->g1 = s1;
    contact->g2 = s2;
    return CollideSpheres(s1->radius, s2->radius, s1->pos, s2->pos, contact);
}

// TODO (not drafted yet, 0x1708 bytes at 0x8026FA3C): box against box
extern "C" int fn_8026FA3C(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// TODO (not drafted yet, 0x3C4 bytes at 0x80271144): polygon point culling (cullPoints)
extern "C" void fn_80271144(int n, float* p, int m, int i0, int* iret);

// TODO (not drafted yet, 0x20C bytes at 0x80271508): clips a quad against a rectangle (intersectRectQuad)
extern "C" int fn_80271508(float* h, float* p, float* ret);

// TODO (not drafted yet, 0x18C bytes at 0x80271714): signed depth of a point in a capsule
extern "C" float fn_80271714(float x, float y, float z, Geom* capsule);

// 0x802718A0: reads a capsule's radius and length
void CapsuleGetParams(Geom* g, float* radius, float* length)
{
    CapsuleGeom* c = (CapsuleGeom*)g;

    *radius = c->radius;
    *length = c->lz;
}


// 0x802718B4: allocates a capsule (dCreateCCylinder)
Geom* CapsuleCreate(float radius, float length, Space* space)
{
    return new CapsuleGeom(space, radius, length);
}

// 0x80271938: bounds of a capsule
void CapsuleGeom::computeAABB()
{
    float* R = rot;
    float xr = 0.5f * FAbs(R[2] * lz) + radius;
    float yr = 0.5f * FAbs(R[6] * lz) + radius;
    float zr = 0.5f * FAbs(R[10] * lz) + radius;

    aabb[0] = pos[0] - xr;
    aabb[1] = pos[0] + xr;
    aabb[2] = pos[1] - yr;
    aabb[3] = pos[1] + yr;
    aabb[4] = pos[2] - zr;
    aabb[5] = pos[2] + zr;
}

// 0x802719E8: signed depth of a point inside a box (dGeomBoxPointDepth)
float BoxPointDepth(float x, float y, float z, Geom* g)
{
    BoxGeom* b = (BoxGeom*)g;
    float* R = b->rot;
    float* pos = b->pos;
    float p[4], q[4];
    p[0] = x - pos[0];
    p[1] = y - pos[1];
    p[2] = z - pos[2];
    // q = R^T p (dMULTIPLY1_331)
    q[0] = R[0] * p[0] + R[4] * p[1] + R[8] * p[2];
    q[1] = R[1] * p[0] + R[5] * p[1] + R[9] * p[2];
    q[2] = R[2] * p[0] + R[6] * p[1] + R[10] * p[2];
    float dx = b->side[0] * 0.5f - FAbs(q[0]);
    float dy = b->side[1] * 0.5f - FAbs(q[1]);
    float dz = b->side[2] * 0.5f - FAbs(q[2]);
    if (dx < dy) {
        if (dx < dz) {
            return dx;
        } else {
            return dz;
        }
    } else {
        if (dy < dz) {
            return dy;
        } else {
            return dz;
        }
    }
}

// 0x80271AB4: reads a box's side lengths
void BoxGetLengths(Geom* g, float* result)
{
    BoxGeom* b = (BoxGeom*)g;

    result[0] = b->side[0];
    result[1] = b->side[1];
    result[2] = b->side[2];
}

// 0x80271AD0: allocates a box (dCreateBox)
Geom* BoxCreate(float lx, float ly, float lz, Space* space)
{
    return new BoxGeom(space, lx, ly, lz);
}

// 0x80271B64: bounds of a box
void BoxGeom::computeAABB()
{
    float* R = rot;
    float xr = 0.5f * (FAbs(R[0] * side[0]) + FAbs(R[1] * side[1]) + FAbs(R[2] * side[2]));
    float yr = 0.5f * (FAbs(R[4] * side[0]) + FAbs(R[5] * side[1]) + FAbs(R[6] * side[2]));
    float zr = 0.5f * (FAbs(R[8] * side[0]) + FAbs(R[9] * side[1]) + FAbs(R[10] * side[2]));

    aabb[0] = pos[0] - xr;
    aabb[1] = pos[0] + xr;
    aabb[2] = pos[1] - yr;
    aabb[3] = pos[1] + yr;
    aabb[4] = pos[2] - zr;
    aabb[5] = pos[2] + zr;
}

// 0x80271C90: a sphere's radius
float SphereGetRadius(Geom* g)
{
    return ((SphereGeom*)g)->radius;
}

// 0x80271C98: allocates a sphere (dCreateSphere)
Geom* SphereCreate(float radius, Space* space)
{
    return new SphereGeom(space, radius);
}

// 0x80271D0C: bounds of a sphere
void SphereGeom::computeAABB()
{
    aabb[0] = pos[0] - radius;
    aabb[1] = pos[0] + radius;
    aabb[2] = pos[1] - radius;
    aabb[3] = pos[1] + radius;
    aabb[4] = pos[2] - radius;
    aabb[5] = pos[2] + radius;
}

// 0x80271D88
CapsuleGeom::~CapsuleGeom()
{
}

// 0x80271DEC
BoxGeom::~BoxGeom()
{
}

// 0x80271E50
SphereGeom::~SphereGeom()
{
}

// 0x80271EB4: out = R * v (dMULTIPLY0_331)
void MatMulVec(float* out, float* R, float* v)
{
    float v1 = v[1];
    float r5 = R[5];
    float v0 = v[0];
    float r9 = R[9];
    float r4 = R[4];
    float v2 = v[2];
    float r8 = R[8];
    float r6 = R[6];
    float r10 = R[10];

    out[0] = R[2] * v2 + R[0] * v0 + R[1] * v1;
    out[1] = r6 * v2 + r4 * v0 + r5 * v1;
    out[2] = r10 * v2 + r8 * v0 + r9 * v1;
}

// 0x80271F18: square root (dSqrt)
float SqrtFloat(float x)
{
    return SqrtF(x);
}

// 0x80271FFC: out = R^T * v (dMULTIPLY1_331)
void MatTransMulVec(float* out, float* R, float* v)
{
    float v1 = v[1];
    float r5 = R[5];
    float v0 = v[0];
    float r6 = R[6];
    float r1 = R[1];
    float v2 = v[2];
    float r2 = R[2];
    float r9 = R[9];
    float r10 = R[10];

    out[0] = R[8] * v2 + R[0] * v0 + R[4] * v1;
    out[1] = r9 * v2 + r1 * v0 + r5 * v1;
    out[2] = r10 * v2 + r2 * v0 + r6 * v1;
}

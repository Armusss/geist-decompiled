// u_8026C200.cpp: the second half of the ODE-style collision kernel: the hash space,
// the simple-space list helpers, the class-pair colliders (sphere, box, capsule,
// cylinder, plane, ray, ...) and the first part of the joint/body library.
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
void* SolverAllocBlock(u32 size);
void SolverFreeBlock(void* block, u32 size);
void SolverClearFloats(float* p, int n);

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

    void* operator new(u32 size) { return SolverAllocBlock(size); }
    void operator delete(void* p, u32 size) { SolverFreeBlock(p, size); }
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
extern int lbl_803C03B8[31];

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

static inline float FAbs(float x)
{
    return __fabs(x);
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

// 0x8026C200: allocates a hash space (dHashSpaceCreate)
Space* HashSpaceCreate(Space* parent)
{
    return new HashSpace(parent);
}

// Tests two geometries and calls back when their bounds overlap (collideAABBs).
static inline void CollideAabbs(Geom* g1, Geom* g2, void* data, SpaceCallback cb)
{
    float* a1;
    float* a2;

    if (g1->body == g2->body && g1->body != 0) {
        return;
    }
    if (((g1->categoryBits & g2->collideBits) != 0 || (g2->categoryBits & g1->collideBits) != 0) == 0) {
        return;
    }
    a1 = g1->aabb;
    a2 = g2->aabb;
    if (g1->aabb[0] > g2->aabb[1] || a1[1] < a2[0] || a1[2] > a2[3] || a1[3] < a2[2] || a1[4] > a2[5]
        || a1[5] < a2[4]) {
        return;
    }
    if (g1->AABBTest(g2, a2) == 0) {
        return;
    }
    if (g2->AABBTest(g1, a1) == 0) {
        return;
    }
    cb(data, g1, g2);
}

// 0x8026C2A0: collides one geometry against every geometry of the space
void HashSpace::collide2(void* data, Geom* g, SpaceCallback cb)
{
    Geom* o;

    lockCount++;
    cleanGeoms();
    if ((g->flags & GEOM_AABB_BAD) != 0) {
        g->computeAABB();
        g->flags &= ~GEOM_AABB_BAD;
    }
    for (o = first; o != 0; o = o->next) {
        CollideAabbs(o, g, data, cb);
    }
    lockCount--;
}

// TODO (not drafted yet, 0xA28 bytes at 0x8026C448): HashSpace::collide (levels, bucket grid)
extern "C" void fn_8026C448(HashSpace* s, void* data, SpaceCallback cb);

// 0x8026CE70: recomputes the bounds of every dirty geometry
void HashSpace::cleanGeoms()
{
    Geom* g;

    lockCount++;
    for (g = first; g != 0 && (g->flags & GEOM_DIRTY) != 0; g = g->next) {
        if (g->type >= 8 && g->type <= 10) {
            ((Space*)g)->cleanGeoms();
        }
        if ((g->flags & GEOM_AABB_BAD) != 0) {
            g->computeAABB();
            g->flags &= ~GEOM_AABB_BAD;
        }
        g->flags &= ~(GEOM_DIRTY | GEOM_AABB_BAD);
    }
    lockCount--;
}

// 0x8026CF38: moves a geometry to the head of the list (dxSpace::dirty)
void Space::dirty(Geom* g)
{
    GeomUnlink(g);
    GeomLink(g, &first);
}

// 0x8026CF84: removes a geometry from the space (dxSpace::remove)
void Space::remove(Geom* g)
{
    GeomUnlink(g);
    count--;
    g->next = 0;
    g->tome = 0;
    g->parentSpace = 0;
    currentGeom = 0;
    GeomMoved(this);
}

// 0x8026D050: adds a geometry to the space (dxSpace::add)
void Space::add(Geom* g, int placeable)
{
    g->parentSpace = this;
    GeomLink(g, &first);
    count++;
    currentGeom = 0;
    g->flags |= GEOM_DIRTY | GEOM_AABB_BAD;
    if (placeable == 0) {
        g->flags |= 0x10;
    }
    GeomMoved(this);
}

// 0x8026D138: n-th geometry of the space, with a one-entry cache for sequential walks
Geom* Space::getGeom(int i)
{
    Geom* g;
    int j;

    if (currentGeom != 0 && currentIndex == i - 1) {
        currentGeom = currentGeom->next;
        currentIndex = i;
        return currentGeom;
    }
    g = first;
    for (j = 0; j < i; j++) {
        if (g != 0) {
            g = g->next;
        } else {
            return 0;
        }
    }
    currentGeom = g;
    currentIndex = i;
    return g;
}

// 0x8026D1A4: recomputes the bounds of the space from its geometries
void Space::computeAABB()
{
    if (first != 0) {
        Geom* g;
        float a[6];

        a[0] = 3.4028235e38f;
        a[1] = -3.4028235e38f;
        a[2] = 3.4028235e38f;
        a[3] = -3.4028235e38f;
        a[4] = 3.4028235e38f;
        a[5] = -3.4028235e38f;
        for (g = first; g != 0; g = g->next) {
            if ((g->flags & GEOM_AABB_BAD) != 0) {
                g->computeAABB();
                g->flags &= ~GEOM_AABB_BAD;
            }
            if (g->aabb[0] < a[0]) {
                a[0] = g->aabb[0];
            }
            if (g->aabb[2] < a[2]) {
                a[2] = g->aabb[2];
            }
            if (g->aabb[4] < a[4]) {
                a[4] = g->aabb[4];
            }
            if (g->aabb[1] > a[1]) {
                a[1] = g->aabb[1];
            }
            if (g->aabb[3] > a[3]) {
                a[3] = g->aabb[3];
            }
            if (g->aabb[5] > a[5]) {
                a[5] = g->aabb[5];
            }
        }
        memcpy(aabb, a, 0x18);
    } else {
        SolverClearFloats(aabb, 6);
    }
}

// Destroys a geometry (dGeomDestroy); lives in the neighbouring unit.
void GeomDestroyGeom(Geom* g);

// 0x8026D2D8: destroys a space and its geometries (dxSpace::~dxSpace)
Space::~Space()
{
    Geom* g;

    if (cleanup != 0) {
        g = first;
        while (g != 0) {
            Geom* n = g->next;
            GeomDestroyGeom(g);
            g = n;
        }
    } else {
        g = first;
        while (g != 0) {
            Geom* n = g->next;
            remove(g);
            g = n;
        }
    }
}

// 0x8026D3A0: flags a geometry and its parents moved (dGeomMoved)
extern "C" void fn_8026D3A0(Geom* g)
{
    GeomMoved(g);
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
    BoxGeom* box = (BoxGeom*)g;
    float* p = g->pos;
    float* R = g->rot;
    float dy = y - p[1];
    float dx = x - p[0];
    float dz = z - p[2];
    float dX = 0.5f * box->side[0] - FAbs(R[8] * dz + R[0] * dx + R[4] * dy);
    float dY = 0.5f * box->side[1] - FAbs(R[9] * dz + R[1] * dx + R[5] * dy);
    float dZ = 0.5f * box->side[2] - FAbs(R[10] * dz + R[2] * dx + R[6] * dy);

    if (dY <= dX) {
        if (dY < dZ) {
            return dY;
        }
        return dZ;
    }
    if (dX < dZ) {
        return dX;
    }
    return dZ;
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
    float sx = side[0];
    float sy = side[1];
    float sz = side[2];
    float xr = 0.5f * (FAbs(R[0] * sx) + FAbs(R[1] * sy) + FAbs(R[2] * sz));
    float yr = 0.5f * (FAbs(R[4] * sx) + FAbs(R[5] * sy) + FAbs(R[6] * sz));
    float zr = 0.5f * (FAbs(R[8] * sx) + FAbs(R[9] * sy) + FAbs(R[10] * sz));

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

// TODO (not drafted yet, 0x1A4 bytes at 0x80272060): box against plane
extern "C" int fn_80272060(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// TODO (not drafted yet, 0x238 bytes at 0x80272204): box-box helper (separating axis tests)
extern "C" void fn_80272204(void);

// TODO (not drafted yet, 0x654 bytes at 0x8027243C): closest points between a line segment and a box
extern "C" void fn_8027243C(float* p1, float* p2, float* c, float* R, float* side, float* lret, float* bret);

// TODO (not drafted yet, 0x640 bytes at 0x80272A90): closest points between two box segments
extern "C" void fn_80272A90(void);

// 0x802730D0: closest approach of two lines (dLineClosestApproach)
void LineClosestApproach(float* pa, float* ua, float* pb, float* ub, float* alpha, float* beta)
{
    float uaub = ua[0] * ub[0] + ua[1] * ub[1] + ua[2] * ub[2];
    float q1 = ua[0] * (pb[0] - pa[0]) + ua[1] * (pb[1] - pa[1]) + ua[2] * (pb[2] - pa[2]);
    float q2 = -(ub[0] * (pb[0] - pa[0]) + ub[1] * (pb[1] - pa[1]) + ub[2] * (pb[2] - pa[2]));
    float d = 1.0f - uaub * uaub;

    if (d <= 0.0001f) {
        *alpha = 0.0f;
        *beta = 0.0f;
    } else {
        d = 1.0f / d;
        *alpha = (q1 + uaub * q2) * d;
        *beta = (uaub * q1 + q2) * d;
    }
}

// 0x8027317C: collides two spheres (dCollideSpheres)
int CollideSpheres(float r1, float r2, float* p1, float* p2, ContactGeom* c)
{
    float d = SqrtF((p1[0] - p2[0]) * (p1[0] - p2[0]) + (p1[1] - p2[1]) * (p1[1] - p2[1])
                    + (p1[2] - p2[2]) * (p1[2] - p2[2]));

    if (d > r1 + r2) {
        return 0;
    }
    if (d <= 0.0f) {
        c->pos[0] = p1[0];
        c->pos[1] = p1[1];
        c->pos[2] = p1[2];
        c->normal[0] = 1.0f;
        c->normal[1] = 0.0f;
        c->normal[2] = 0.0f;
        c->depth = r1 + r2;
    } else {
        float inv = 1.0f / d;
        float k = 0.5f * ((r2 - r1) - d);

        c->normal[0] = inv * (p1[0] - p2[0]);
        c->normal[1] = inv * (p1[1] - p2[1]);
        c->normal[2] = inv * (p1[2] - p2[2]);
        c->pos[0] = c->normal[0] * k + p1[0];
        c->pos[1] = c->normal[1] * k + p1[1];
        c->pos[2] = c->normal[2] * k + p1[2];
        c->depth = (r1 + r2) - d;
    }
    return 1;
}

// TODO (not drafted yet, 0x160 bytes at 0x8027337C): ODE message (varargs formatting through the debug console)
extern "C" void fn_8027337C(int num, const char* msg, ...);

// TODO (not drafted yet, 0x164 bytes at 0x802734DC): ODE internal error (varargs formatting, then aborts)
extern "C" void fn_802734DC(int num, const char* msg, ...);

typedef void (*OdeErrorHandler)(int num, const char* msg, void* ap);

static OdeErrorHandler sErrorHandler;    // 0x804D5524
static OdeErrorHandler sMessageHandler;  // 0x804D5520

// 0x80273640: sets the error handler (dSetErrorHandler)
void SetErrorHandler(OdeErrorHandler fn)
{
    sErrorHandler = fn;
}

// 0x80273648: sets the message handler (dSetMessageHandler)
void SetMessageHandler(OdeErrorHandler fn)
{
    sMessageHandler = fn;
}

// TODO (not drafted yet, 0x38 bytes at 0x80273650): ODE joint/body library routine
extern "C" void fn_80273650();

// TODO (not drafted yet, 0x11C bytes at 0x80273688): ODE joint/body library routine
extern "C" void fn_80273688();

// TODO (not drafted yet, 0x174 bytes at 0x802737A4): ODE joint/body library routine
extern "C" void fn_802737A4();

// TODO (not drafted yet, 0x38 bytes at 0x80273918): ODE joint/body library routine
extern "C" void fn_80273918();

// TODO (not drafted yet, 0x120 bytes at 0x80273950): ODE joint/body library routine
extern "C" void fn_80273950();

// TODO (not drafted yet, 0x10C bytes at 0x80273A70): ODE joint/body library routine
extern "C" void fn_80273A70();

// TODO (not drafted yet, 0x274 bytes at 0x80273B7C): ODE joint/body library routine
extern "C" void fn_80273B7C();

// TODO (not drafted yet, 0x24C bytes at 0x80273DF0): ODE joint/body library routine
extern "C" void fn_80273DF0();

// TODO (not drafted yet, 0x2FC bytes at 0x8027403C): ODE joint/body library routine
extern "C" void fn_8027403C();

// TODO (not drafted yet, 0x11C bytes at 0x80274338): ODE joint/body library routine
extern "C" void fn_80274338();

// TODO (not drafted yet, 0x2C0 bytes at 0x80274454): ODE joint/body library routine
extern "C" void fn_80274454();

// TODO (not drafted yet, 0x768 bytes at 0x80274714): ODE joint/body library routine
extern "C" void fn_80274714();

// TODO (not drafted yet, 0xC0 bytes at 0x80274E7C): ODE joint/body library routine
extern "C" void fn_80274E7C();

// TODO (not drafted yet, 0x4 bytes at 0x80274F3C): ODE joint/body library routine
extern "C" void fn_80274F3C();

// TODO (not drafted yet, 0xB8 bytes at 0x80274F40): ODE joint/body library routine
extern "C" void fn_80274F40();

// TODO (not drafted yet, 0x220 bytes at 0x80274FF8): ODE joint/body library routine
extern "C" void fn_80274FF8();

// TODO (not drafted yet, 0x18C bytes at 0x80275218): ODE joint/body library routine
extern "C" void fn_80275218();

// TODO (not drafted yet, 0x29C bytes at 0x802753A4): ODE joint/body library routine
extern "C" void fn_802753A4();

// TODO (not drafted yet, 0x2B4 bytes at 0x80275640): ODE joint/body library routine
extern "C" void fn_80275640();

// TODO (not drafted yet, 0xC4 bytes at 0x802758F4): ODE joint/body library routine
extern "C" void fn_802758F4();

// TODO (not drafted yet, 0x110 bytes at 0x802759B8): ODE joint/body library routine
extern "C" void fn_802759B8();

// TODO (not drafted yet, 0x28 bytes at 0x80275AC8): ODE joint/body library routine
extern "C" void fn_80275AC8();

// TODO (not drafted yet, 0x10 bytes at 0x80275AF0): ODE joint/body library routine
extern "C" void fn_80275AF0();

// TODO (not drafted yet, 0x40 bytes at 0x80275B00): ODE joint/body library routine
extern "C" void fn_80275B00();

// TODO (not drafted yet, 0x550 bytes at 0x80275B40): ODE joint/body library routine
extern "C" void fn_80275B40();

// TODO (not drafted yet, 0xE0 bytes at 0x80276090): ODE joint/body library routine
extern "C" void fn_80276090();

// TODO (not drafted yet, 0x13C bytes at 0x80276170): ODE joint/body library routine
extern "C" void fn_80276170();

// TODO (not drafted yet, 0xA4 bytes at 0x802762AC): ODE joint/body library routine
extern "C" void fn_802762AC();

// TODO (not drafted yet, 0x14C bytes at 0x80276350): ODE joint/body library routine
extern "C" void fn_80276350();

// TODO (not drafted yet, 0x48 bytes at 0x8027649C): ODE joint/body library routine
extern "C" void fn_8027649C();

// TODO (not drafted yet, 0x20C bytes at 0x802764E4): ODE joint/body library routine
extern "C" void fn_802764E4();

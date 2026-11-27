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


// ODE's dot products (odemath.h inlines)
static inline float Dot(const float* a, const float* b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
static inline float Dot14(const float* a, const float* b) { return a[0] * b[0] + a[1] * b[4] + a[2] * b[8]; }

// A ray geometry: the base plus its length.
struct RayGeom : Geom {
    float length;  // 0x4c
};

// A plane geometry (dxPlane): the base plus the plane equation p[0]*x + p[1]*y + p[2]*z = p[3].
struct PlaneGeom : Geom {
    float p[4];  // 0x4c
};

// 0x8026D428: ray against plane (RayPlaneCollide)
extern "C" int fn_8026D428(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    RayGeom* ray = (RayGeom*)o1;
    PlaneGeom* plane = (PlaneGeom*)o2;

    float alpha = plane->p[3] - Dot(plane->p, ray->pos);
    // note: if alpha > 0 the starting point is below the plane
    float nsign = (alpha > 0) ? -1.0f : 1.0f;
    float k = Dot14(plane->p, ray->rot + 2);
    if (k == 0) {
        return 0;  // ray parallel to plane
    }
    alpha /= k;
    if (alpha < 0 || alpha > ray->length) {
        return 0;
    }
    contact->pos[0] = ray->pos[0] + alpha * ray->rot[0 * 4 + 2];
    contact->pos[1] = ray->pos[1] + alpha * ray->rot[1 * 4 + 2];
    contact->pos[2] = ray->pos[2] + alpha * ray->rot[2 * 4 + 2];
    contact->normal[0] = nsign * plane->p[0];
    contact->normal[1] = nsign * plane->p[1];
    contact->normal[2] = nsign * plane->p[2];
    contact->depth = alpha;
    contact->g1 = ray;
    contact->g2 = plane;
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

int RaySphereTest(RayGeom* ray, float* sphere_pos, float radius, ContactGeom* contact, int mode);

// 0x8026DEAC: ray against sphere (dCollideRaySphere)
extern "C" int fn_8026DEAC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    RayGeom* ray = (RayGeom*)o1;
    SphereGeom* sphere = (SphereGeom*)o2;

    contact->g1 = ray;
    contact->g2 = sphere;
    return RaySphereTest(ray, sphere->pos, sphere->radius, contact, 0);
}


// 0x8026DEE8: ray against a sphere given by centre and radius (ray_sphere_helper); mode 1 uses the exit point
int RaySphereTest(RayGeom* ray, float* sphere_pos, float radius, ContactGeom* contact, int mode)
{
    float q[3];
    q[0] = ray->pos[0] - sphere_pos[0];
    q[1] = ray->pos[1] - sphere_pos[1];
    q[2] = ray->pos[2] - sphere_pos[2];
    float B = Dot14(q, ray->rot + 2);
    float C = Dot(q, q) - radius * radius;
    float k = B * B - C;
    if (k < 0) {
        return 0;
    }
    k = SqrtF(k);
    float alpha;
    if (mode && C >= 0) {
        alpha = -B + k;
        if (alpha < 0) {
            return 0;
        }
    } else {
        alpha = -B - k;
        if (alpha < 0) {
            alpha = -B + k;
            if (alpha < 0) {
                return 0;
            }
        }
    }
    if (alpha > ray->length) {
        return 0;
    }
    contact->pos[0] = ray->pos[0] + alpha * ray->rot[0 * 4 + 2];
    contact->pos[1] = ray->pos[1] + alpha * ray->rot[1 * 4 + 2];
    contact->pos[2] = ray->pos[2] + alpha * ray->rot[2 * 4 + 2];
    float nsign = (C < 0 || mode) ? -1.0f : 1.0f;
    contact->normal[0] = nsign * (contact->pos[0] - sphere_pos[0]);
    contact->normal[1] = nsign * (contact->pos[1] - sphere_pos[1]);
    contact->normal[2] = nsign * (contact->pos[2] - sphere_pos[2]);
    fn_8027914C(contact->normal);
    contact->depth = alpha;
    return 1;
}

enum { NUMC_MASK = 0xffff };
#define CONTACT(p, skip) ((ContactGeom*)(((char*)p) + (skip)))

// 0x8026E194: capsule against plane (dCollideCCylinderPlane)
extern "C" int fn_8026E194(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    CapsuleGeom* ccyl = (CapsuleGeom*)o1;
    PlaneGeom* plane = (PlaneGeom*)o2;

    // collide the deepest capping sphere with the plane
    float sign = (Dot14(plane->p, o1->rot + 2) > 0) ? -1.0f : 1.0f;
    float p[4];
    p[0] = o1->pos[0] + o1->rot[2] * ccyl->lz * 0.5f * sign;
    p[1] = o1->pos[1] + o1->rot[6] * ccyl->lz * 0.5f * sign;
    p[2] = o1->pos[2] + o1->rot[10] * ccyl->lz * 0.5f * sign;

    float k = Dot(p, plane->p);
    float depth = plane->p[3] - k + ccyl->radius;
    if (depth < 0) {
        return 0;
    }
    contact->normal[0] = plane->p[0];
    contact->normal[1] = plane->p[1];
    contact->normal[2] = plane->p[2];
    contact->pos[0] = p[0] - plane->p[0] * ccyl->radius;
    contact->pos[1] = p[1] - plane->p[1] * ccyl->radius;
    contact->pos[2] = p[2] - plane->p[2] * ccyl->radius;
    contact->depth = depth;

    int ncontacts = 1;
    if ((flags & NUMC_MASK) >= 2) {
        // collide the other capping sphere with the plane
        p[0] = o1->pos[0] - o1->rot[2] * ccyl->lz * 0.5f * sign;
        p[1] = o1->pos[1] - o1->rot[6] * ccyl->lz * 0.5f * sign;
        p[2] = o1->pos[2] - o1->rot[10] * ccyl->lz * 0.5f * sign;

        k = Dot(p, plane->p);
        depth = plane->p[3] - k + ccyl->radius;
        if (depth >= 0) {
            ContactGeom* c2 = CONTACT(contact, skip);
            c2->normal[0] = plane->p[0];
            c2->normal[1] = plane->p[1];
            c2->normal[2] = plane->p[2];
            c2->pos[0] = p[0] - plane->p[0] * ccyl->radius;
            c2->pos[1] = p[1] - plane->p[1] * ccyl->radius;
            c2->pos[2] = p[2] - plane->p[2] * ccyl->radius;
            c2->depth = depth;
            ncontacts = 2;
        }
    }

    for (int i = 0; i < ncontacts; i++) {
        CONTACT(contact, i * skip)->g1 = o1;
        CONTACT(contact, i * skip)->g2 = o2;
    }
    return ncontacts;
}

// TODO (not drafted yet, 0x454 bytes at 0x8026E498): box against sphere / capsule family
extern "C" int fn_8026E498(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// Closest points between a line segment and a box (dClosestLineBoxPoints); later in this unit.
void ClosestLineBoxPoints(float* p1, float* p2, float* c, float* R, float* side, float* lret, float* bret);
// Collides two spheres (dCollideSpheres); later in this unit.
int CollideSpheres(float* p1, float r1, float* p2, float r2, ContactGeom* contact);

// 0x8026E8EC: capsule against box (CapsuleBoxCollide)
extern "C" int fn_8026E8EC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    CapsuleGeom* cyl = (CapsuleGeom*)o1;
    BoxGeom* box = (BoxGeom*)o2;

    contact->g1 = o1;
    contact->g2 = o2;

    // get p1,p2 = cylinder axis endpoints, get radius
    float p1[4], p2[4];
    float clen = cyl->lz * 0.5f;
    p1[0] = o1->pos[0] + clen * o1->rot[2];
    p1[1] = o1->pos[1] + clen * o1->rot[6];
    p1[2] = o1->pos[2] + clen * o1->rot[10];
    p2[0] = o1->pos[0] - clen * o1->rot[2];
    p2[1] = o1->pos[1] - clen * o1->rot[6];
    p2[2] = o1->pos[2] - clen * o1->rot[10];
    float radius = cyl->radius;

    // copy out box center, rotation matrix, and side array
    float* c = o2->pos;
    float* R = o2->rot;
    float* side = box->side;

    // get the closest point between the cylinder axis and the box
    float pl[4], pb[4];
    ClosestLineBoxPoints(p1, p2, c, R, side, pl, pb);

    // generate contact point
    return CollideSpheres(pl, radius, pb, 0, contact);
}

// 0x8026E9C8: capsule against sphere (CapsuleSphereCollide)
extern "C" int fn_8026E9C8(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    CapsuleGeom* ccyl = (CapsuleGeom*)o1;
    SphereGeom* sphere = (SphereGeom*)o2;

    contact->g1 = o1;
    contact->g2 = o2;

    // find the point on the cylinder axis that is closest to the sphere
    float alpha = o1->rot[2] * (o2->pos[0] - o1->pos[0]) + o1->rot[6] * (o2->pos[1] - o1->pos[1]) +
                  o1->rot[10] * (o2->pos[2] - o1->pos[2]);
    float lz2 = ccyl->lz * 0.5f;
    if (alpha > lz2) {
        alpha = lz2;
    }
    if (alpha < -lz2) {
        alpha = -lz2;
    }

    // collide the spheres
    float p[4];
    p[0] = o1->pos[0] + alpha * o1->rot[2];
    p[1] = o1->pos[1] + alpha * o1->rot[6];
    p[2] = o1->pos[2] + alpha * o1->rot[10];
    return CollideSpheres(p, ccyl->radius, o2->pos, sphere->radius, contact);
}

// TODO (not drafted yet, 0x770 bytes at 0x8026EAAC): capsule against capsule
extern "C" int fn_8026EAAC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// Box-box contact generation (dBoxBox); later in this unit.
extern "C" int fn_8026FA3C(const float* p1, const float* R1, const float* side1, const float* p2,
                           const float* R2, const float* side2, float* normal, float* depth, int* return_code,
                           int maxc, ContactGeom* contact, int skip);

// 0x8026F21C: box against box (dCollideBoxBox)
extern "C" int fn_8026F21C(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    float normal[4];
    float depth;
    int code;
    BoxGeom* b1 = (BoxGeom*)o1;
    BoxGeom* b2 = (BoxGeom*)o2;
    int num = fn_8026FA3C(o1->pos, o1->rot, b1->side, o2->pos, o2->rot, b2->side, normal, &depth, &code,
                          flags & NUMC_MASK, contact, skip);
    for (int i = 0; i < num; i++) {
        CONTACT(contact, i * skip)->normal[0] = -normal[0];
        CONTACT(contact, i * skip)->normal[1] = -normal[1];
        CONTACT(contact, i * skip)->normal[2] = -normal[2];
        CONTACT(contact, i * skip)->g1 = o1;
        CONTACT(contact, i * skip)->g2 = o2;
    }
    return num;
}

// 0x8026F4D0: sphere against plane (dCollideSpherePlane)
extern "C" int fn_8026F4D0(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    SphereGeom* sphere = (SphereGeom*)o1;
    PlaneGeom* plane = (PlaneGeom*)o2;

    contact->g1 = o1;
    contact->g2 = o2;
    float k = Dot(o1->pos, plane->p);
    float depth = plane->p[3] - k + sphere->radius;
    if (depth >= 0) {
        contact->normal[0] = plane->p[0];
        contact->normal[1] = plane->p[1];
        contact->normal[2] = plane->p[2];
        contact->pos[0] = o1->pos[0] - plane->p[0] * sphere->radius;
        contact->pos[1] = o1->pos[1] - plane->p[1] * sphere->radius;
        contact->pos[2] = o1->pos[2] - plane->p[2] * sphere->radius;
        contact->depth = depth;
        return 1;
    } else {
        return 0;
    }
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
    return CollideSpheres(o1->pos, s1->radius, o2->pos, s2->radius, contact);
}

// TODO (not drafted yet, 0x1708 bytes at 0x8026FA3C): box-box contact generation (dBoxBox)

// ODE's float pi (M_PI as a dReal)
#define M_PI 3.1415926535897932384626433832795029f

extern "C" double atan2(double y, double x);

// 0x80271144: picks m of the n polygon points p[], spread evenly in angle around the centroid,
// starting with point i0 (cullPoints)
extern "C" void fn_80271144(int n, float* p, int m, int i0, int* iret)
{
    // compute the centroid of the polygon in cx,cy
    int i, j;
    float a, cx, cy, q;
    if (n == 1) {
        cx = p[0];
        cy = p[1];
    } else if (n == 2) {
        cx = 0.5f * (p[0] + p[2]);
        cy = 0.5f * (p[1] + p[3]);
    } else {
        a = 0;
        cx = 0;
        cy = 0;
        for (i = 0; i < (n - 1); i++) {
            q = p[i * 2] * p[i * 2 + 3] - p[i * 2 + 2] * p[i * 2 + 1];
            a += q;
            cx += q * (p[i * 2] + p[i * 2 + 2]);
            cy += q * (p[i * 2 + 1] + p[i * 2 + 3]);
        }
        q = p[n * 2 - 2] * p[1] - p[0] * p[n * 2 - 1];
        a = 1.0f / (3.0f * (a + q));
        cx = a * (cx + q * (p[n * 2 - 2] + p[0]));
        cy = a * (cy + q * (p[n * 2 - 1] + p[1]));
    }

    // compute the angle of each point w.r.t. the centroid
    float A[8];
    for (i = 0; i < n; i++) {
        A[i] = atan2(p[i * 2 + 1] - cy, p[i * 2] - cx);
    }

    // search for points that have angles closest to A[i0] + i*(2*pi/m).
    int avail[8];
    for (i = 0; i < n; i++) {
        avail[i] = 1;
    }
    avail[i0] = 0;
    iret[0] = i0;
    iret++;
    for (j = 1; j < m; j++) {
        a = float(j) * (2 * M_PI / m) + A[i0];
        if (a > M_PI) {
            a -= 2 * M_PI;
        }
        float maxdiff = 1e9, diff;
        for (i = 0; i < n; i++) {
            if (avail[i]) {
                diff = FAbs(A[i] - a);
                if (diff > M_PI) {
                    diff = 2 * M_PI - diff;
                }
                if (diff < maxdiff) {
                    maxdiff = diff;
                    *iret = i;
                }
            }
        }
        avail[*iret] = 0;
        iret++;
    }
}

// 0x80271508: clips a quad against a rectangle (intersectRectQuad); returns the point count
extern "C" int fn_80271508(float* h, float* p, float* ret)
{
    // q (and r) contain nq (and nr) coordinate points for the current (and chopped) polygons
    int nq = 4, nr;
    float buffer[16];
    float* q = p;
    float* r = ret;
    for (int dir = 0; dir <= 1; dir++) {
        // direction notation: xy[0] = x axis, xy[1] = y axis
        for (int sign = -1; sign <= 1; sign += 2) {
            // chop q along the line xy[dir] = sign*h[dir]
            float* pq = q;
            float* pr = r;
            nr = 0;
            for (int i = nq; i > 0; i--) {
                // go through all points in q and all lines between adjacent points
                if (sign * pq[dir] < h[dir]) {
                    // this point is inside the chopping line
                    pr[0] = pq[0];
                    pr[1] = pq[1];
                    pr += 2;
                    nr++;
                    if (nr & 8) {
                        q = r;
                        goto done;
                    }
                }
                float* nextq = (i > 1) ? pq + 2 : q;
                if ((sign * pq[dir] < h[dir]) ^ (sign * nextq[dir] < h[dir])) {
                    // this line crosses the chopping line
                    pr[1 - dir] = pq[1 - dir] + (nextq[1 - dir] - pq[1 - dir]) / (nextq[dir] - pq[dir]) * (sign * h[dir] - pq[dir]);
                    pr[dir] = sign * h[dir];
                    pr += 2;
                    nr++;
                    if (nr & 8) {
                        q = r;
                        goto done;
                    }
                }
                pq += 2;
            }
            q = r;
            r = (q == ret) ? buffer : ret;
            nq = nr;
        }
    }
done:
    if (q != ret) {
        memcpy(ret, q, nr * 2 * sizeof(float));
    }
    return nr;
}

// 0x80271714: signed depth of a point inside a capsule (dGeomCCylinderPointDepth)
extern "C" float fn_80271714(float x, float y, float z, Geom* g)
{
    CapsuleGeom* c = (CapsuleGeom*)g;
    float a[4];

    a[0] = x - c->pos[0];
    a[1] = y - c->pos[1];
    a[2] = z - c->pos[2];
    float beta = Dot14(a, c->rot + 2);
    float lz2 = c->lz * 0.5f;
    if (beta < -lz2) {
        beta = -lz2;
    } else if (beta > lz2) {
        beta = lz2;
    }
    a[0] = c->pos[0] + beta * c->rot[0 * 4 + 2];
    a[1] = c->pos[1] + beta * c->rot[1 * 4 + 2];
    a[2] = c->pos[2] + beta * c->rot[2 * 4 + 2];
    return c->radius - SqrtF((x - a[0]) * (x - a[0]) + (y - a[1]) * (y - a[1]) + (z - a[2]) * (z - a[2]));
}

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
void MatMulVec(float* A, const float* B, const float* C)
{
    A[0] = B[0] * C[0] + B[1] * C[1] + B[2] * C[2];
    A[1] = B[4] * C[0] + B[5] * C[1] + B[6] * C[2];
    A[2] = B[8] * C[0] + B[9] * C[1] + B[10] * C[2];
}

// 0x80271F18: square root (dSqrt)
float SqrtFloat(float x)
{
    return SqrtF(x);
}

// 0x80271FFC: out = R^T * v (dMULTIPLY1_331)
void MatTransMulVec(float* A, const float* B, const float* C)
{
    A[0] = B[0] * C[0] + B[4] * C[1] + B[8] * C[2];
    A[1] = B[1] * C[0] + B[5] * C[1] + B[9] * C[2];
    A[2] = B[2] * C[0] + B[6] * C[1] + B[10] * C[2];
}

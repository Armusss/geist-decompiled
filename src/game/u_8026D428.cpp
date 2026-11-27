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

// Single-precision square root (the MSL sqrtf inline): reciprocal-square-root estimate refined
// with three Newton steps. Negative inputs and NaNs give NaN.
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


// Out-of-line helpers at the end of this file (dMULTIPLY0_331, dMULTIPLY1_331, dSqrt).
void MatMulVec(float* A, const float* B, const float* C);
void MatTransMulVec(float* A, const float* B, const float* C);
float SqrtFloat(float x);

// ODE's dot products (odemath.h inlines)
static inline float Dot(const float* a, const float* b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
static inline float Dot14(const float* a, const float* b) { return a[0] * b[0] + a[1] * b[4] + a[2] * b[8]; }
static inline float Dot41(const float* a, const float* b) { return a[0] * b[0] + a[4] * b[1] + a[8] * b[2]; }
static inline float Dot44(const float* a, const float* b) { return a[0] * b[0] + a[4] * b[4] + a[8] * b[8]; }


#define FLT_MAX 3.402823466e+38f

// A ray geometry: the base plus its length.
struct RayGeom : Geom {
    float length;  // 0x4c
};

// A plane geometry (dxPlane): the base plus the plane equation p[0]*x + p[1]*y + p[2]*z = p[3].
struct PlaneGeom : Geom {
    float p[4];  // 0x4c
};

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

// 0x8026DA50: ray against box (dCollideRayBox)
extern "C" int fn_8026DA50(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    RayGeom* ray = (RayGeom*)o1;
    BoxGeom* box = (BoxGeom*)o2;

    contact->g1 = ray;
    contact->g2 = box;

    int i;

    // compute v = start point of segment relative to box center
    float v[4];
    for (i = 0; i < 3; i++) {
        v[i] = ray->pos[i] - box->pos[i];
    }

    // compute v = ray start relative to box, and transformed direction (s,t)
    float tmp[4], s[4], dir[4];
    MatTransMulVec(s, box->rot, v);
    tmp[0] = ray->rot[0 * 4 + 2];
    tmp[1] = ray->rot[1 * 4 + 2];
    tmp[2] = ray->rot[2 * 4 + 2];
    MatTransMulVec(dir, box->rot, tmp);

    // mirror the line so that v has all components >= 0
    float sign[4];
    for (i = 0; i < 3; i++) {
        if (dir[i] < 0) {
            s[i] = -s[i];
            dir[i] = -dir[i];
            sign[i] = 1;
        } else {
            sign[i] = -1;
        }
    }

    // compute h = half the side lengths
    float h[4];
    h[0] = 0.5f * box->side[0];
    h[1] = 0.5f * box->side[1];
    h[2] = 0.5f * box->side[2];

    // do a few early exit tests
    if ((s[0] < -h[0] && dir[0] <= 0) || s[0] > h[0] || (s[1] < -h[1] && dir[1] <= 0) || s[1] > h[1] ||
        (s[2] < -h[2] && dir[2] <= 0) || s[2] > h[2] || (dir[0] == 0 && dir[1] == 0 && dir[2] == 0)) {
        return 0;
    }

    // compute [lo..hi] range for t
    float lo = -FLT_MAX;
    float hi = FLT_MAX;
    int nlo = 0, nhi = 0;
    for (i = 0; i < 3; i++) {
        if (dir[i] != 0) {
            float k = (-h[i] - s[i]) / dir[i];
            if (k > lo) {
                lo = k;
                nlo = i;
            }
            k = (h[i] - s[i]) / dir[i];
            if (k < hi) {
                hi = k;
                nhi = i;
            }
        }
    }

    // check if the ray intersects
    if (lo > hi) {
        return 0;
    }
    float alpha;
    int n;
    if (lo >= 0) {
        alpha = lo;
        n = nlo;
    } else {
        alpha = hi;
        n = nhi;
    }
    if (alpha < 0 || alpha > ray->length) {
        return 0;
    }
    contact->pos[0] = ray->pos[0] + alpha * ray->rot[0 * 4 + 2];
    contact->pos[1] = ray->pos[1] + alpha * ray->rot[1 * 4 + 2];
    contact->pos[2] = ray->pos[2] + alpha * ray->rot[2 * 4 + 2];
    contact->normal[0] = box->rot[0 * 4 + n] * sign[n];
    contact->normal[1] = box->rot[1 * 4 + n] * sign[n];
    contact->normal[2] = box->rot[2 * 4 + n] * sign[n];
    contact->depth = alpha;
    return 1;
}

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


// Closest points between a line segment and a box (dClosestLineBoxPoints) and between two line
// segments (dClosestLineSegmentPoints); in collision_util (u_8027243C).
extern "C" void fn_8027243C(const float* p1, const float* p2, const float* c, const float* R, const float* side,
                            float* lret, float* bret);
extern "C" void fn_80272A90(const float* a1, const float* a2, const float* b1, const float* b2, float* cp1,
                            float* cp2);
// Collides two spheres (dCollideSpheres); in collision_util (u_8027243C).
int CollideSpheres(float* p1, float r1, float* p2, float r2, ContactGeom* contact);

// 0x8026E498: capsule against capsule (dCollideCCylinderCCylinder)
extern "C" int fn_8026E498(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    int i;
    const float tolerance = 1e-5f;

    CapsuleGeom* cyl1 = (CapsuleGeom*)o1;
    CapsuleGeom* cyl2 = (CapsuleGeom*)o2;

    contact->g1 = o1;
    contact->g2 = o2;

    // copy out some variables, for convenience
    float lz1 = cyl1->lz * 0.5f;
    float lz2 = cyl2->lz * 0.5f;
    float* pos1 = o1->pos;
    float* pos2 = o2->pos;
    float axis1[3], axis2[3];
    axis1[0] = o1->rot[2];
    axis1[1] = o1->rot[6];
    axis1[2] = o1->rot[10];
    axis2[0] = o2->rot[2];
    axis2[1] = o2->rot[6];
    axis2[2] = o2->rot[10];

    // if the cylinder axes are close to parallel, we'll try to detect up to
    // two contact points along the body of the cylinder. if we can't find any
    // points then we'll fall back to the closest-points algorithm.

    float sphere1[4], sphere2[4];
    float a1a2 = Dot(axis1, axis2);
    float det = 1.0f - a1a2 * a1a2;
    if (det < tolerance) {
        // the cylinder axes (almost) parallel, so we will generate up to two
        // contacts. first compute where the two cylinders overlap in alpha1 space:
        if (a1a2 < 0) {
            axis2[0] = -axis2[0];
            axis2[1] = -axis2[1];
            axis2[2] = -axis2[2];
        }
        float q[3];
        for (i = 0; i < 3; i++) {
            q[i] = pos1[i] - pos2[i];
        }
        float k = Dot(axis1, q);
        float a1lo = -lz1;
        float a1hi = lz1;
        float a2lo = -lz2 - k;
        float a2hi = lz2 - k;
        float lo = (a1lo > a2lo) ? a1lo : a2lo;
        float hi = (a1hi < a2hi) ? a1hi : a2hi;
        if (lo <= hi) {
            int num_contacts = flags & NUMC_MASK;
            if (num_contacts >= 2 && lo < hi) {
                // generate up to two contacts. if one of those contacts is
                // not made, fall back on the one-contact strategy.
                for (i = 0; i < 3; i++) {
                    sphere1[i] = pos1[i] + lo * axis1[i];
                }
                for (i = 0; i < 3; i++) {
                    sphere2[i] = pos2[i] + (lo + k) * axis2[i];
                }
                int n1 = CollideSpheres(sphere1, cyl1->radius, sphere2, cyl2->radius, contact);
                if (n1) {
                    for (i = 0; i < 3; i++) {
                        sphere1[i] = pos1[i] + hi * axis1[i];
                    }
                    for (i = 0; i < 3; i++) {
                        sphere2[i] = pos2[i] + (hi + k) * axis2[i];
                    }
                    ContactGeom* c2 = CONTACT(contact, skip);
                    int n2 = CollideSpheres(sphere1, cyl1->radius, sphere2, cyl2->radius, c2);
                    if (n2) {
                        c2->g1 = o1;
                        c2->g2 = o2;
                        return 2;
                    }
                }
            }

            // just one contact to generate, so put it in the middle of the range
            float alpha1 = (lo + hi) * 0.5f;
            float alpha2 = alpha1 + k;
            for (i = 0; i < 3; i++) {
                sphere1[i] = pos1[i] + alpha1 * axis1[i];
            }
            for (i = 0; i < 3; i++) {
                sphere2[i] = pos2[i] + alpha2 * axis2[i];
            }
            return CollideSpheres(sphere1, cyl1->radius, sphere2, cyl2->radius, contact);
        }
    }

    // use the closest point algorithm
    float a1[4], a2[4], b1[4], b2[4];
    a1[0] = o1->pos[0] + axis1[0] * lz1;
    a1[1] = o1->pos[1] + axis1[1] * lz1;
    a1[2] = o1->pos[2] + axis1[2] * lz1;
    a2[0] = o1->pos[0] - axis1[0] * lz1;
    a2[1] = o1->pos[1] - axis1[1] * lz1;
    a2[2] = o1->pos[2] - axis1[2] * lz1;
    b1[0] = o2->pos[0] + axis2[0] * lz2;
    b1[1] = o2->pos[1] + axis2[1] * lz2;
    b1[2] = o2->pos[2] + axis2[2] * lz2;
    b2[0] = o2->pos[0] - axis2[0] * lz2;
    b2[1] = o2->pos[1] - axis2[1] * lz2;
    b2[2] = o2->pos[2] - axis2[2] * lz2;

    fn_80272A90(a1, a2, b1, b2, sphere1, sphere2);
    return CollideSpheres(sphere1, cyl1->radius, sphere2, cyl2->radius, contact);
}

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
    fn_8027243C(p1, p2, c, R, side, pl, pb);

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

// 0x8026EAAC: box against plane (dCollideBoxPlane)
extern "C" int fn_8026EAAC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    BoxGeom* box = (BoxGeom*)o1;
    PlaneGeom* plane = (PlaneGeom*)o2;

    contact->g1 = o1;
    contact->g2 = o2;
    int ret = 0;

    //@@@ problem: using 4-vector (plane->p) as 3-vector (normal).
    const float* R = o1->rot;  // rotation of box
    const float* n = plane->p; // normal vector

    // project sides lengths along normal vector, get absolute values
    float Q1 = Dot14(n, R + 0);
    float Q2 = Dot14(n, R + 1);
    float Q3 = Dot14(n, R + 2);
    float A1 = box->side[0] * Q1;
    float A2 = box->side[1] * Q2;
    float A3 = box->side[2] * Q3;
    float B1 = FAbs(A1);
    float B2 = FAbs(A2);
    float B3 = FAbs(A3);

    // early exit test
    float depth = plane->p[3] + 0.5f * (B1 + B2 + B3) - Dot(n, o1->pos);
    if (depth < 0) {
        return 0;
    }

    // find number of contacts requested
    int maxc = flags & NUMC_MASK;
    if (maxc < 1) {
        maxc = 1;
    }
    if (maxc > 3) {
        maxc = 3;  // no more than 3 contacts per box allowed
    }

    // find deepest point
    float p[4];
    p[0] = o1->pos[0];
    p[1] = o1->pos[1];
    p[2] = o1->pos[2];
#define FOO(i, op)                               \
    p[0] op 0.5f * box->side[i] * R[0 + i];     \
    p[1] op 0.5f * box->side[i] * R[4 + i];     \
    p[2] op 0.5f * box->side[i] * R[8 + i];
#define BAR(i, iinc)    \
    if (A##iinc > 0) {  \
        FOO(i, -=)      \
    } else {            \
        FOO(i, +=)      \
    }
    BAR(0, 1);
    BAR(1, 2);
    BAR(2, 3);
#undef FOO
#undef BAR

    // the deepest point is the first contact point
    contact->pos[0] = p[0];
    contact->pos[1] = p[1];
    contact->pos[2] = p[2];
    contact->normal[0] = n[0];
    contact->normal[1] = n[1];
    contact->normal[2] = n[2];
    contact->depth = depth;
    ret = 1;  // ret is number of contact points found so far
    if (maxc == 1) {
        goto done;
    }

    // get the second and third contact points by starting from `p' and going
    // along the two sides with the smallest projected length.

#define FOO(i, j, op)                                                    \
    CONTACT(contact, i * skip)->pos[0] = p[0] op box->side[j] * R[0 + j]; \
    CONTACT(contact, i * skip)->pos[1] = p[1] op box->side[j] * R[4 + j]; \
    CONTACT(contact, i * skip)->pos[2] = p[2] op box->side[j] * R[8 + j];
#define BAR(ctact, side, sideinc)                 \
    depth -= B##sideinc;                          \
    if (depth < 0) goto done;                     \
    if (A##sideinc > 0) {                         \
        FOO(ctact, side, +);                      \
    } else {                                      \
        FOO(ctact, side, -);                      \
    }                                             \
    CONTACT(contact, ctact * skip)->depth = depth; \
    ret++;

    CONTACT(contact, skip)->normal[0] = n[0];
    CONTACT(contact, skip)->normal[1] = n[1];
    CONTACT(contact, skip)->normal[2] = n[2];
    if (maxc == 3) {
        CONTACT(contact, 2 * skip)->normal[0] = n[0];
        CONTACT(contact, 2 * skip)->normal[1] = n[1];
        CONTACT(contact, 2 * skip)->normal[2] = n[2];
    }

    if (B1 < B2) {
        if (B3 < B1) {
            goto use_side_3;
        } else {
            BAR(1, 0, 1);  // use side 1
            if (maxc == 2) goto done;
            if (B2 < B3) goto contact2_2;
            else goto contact2_3;
        }
    } else {
        if (B3 < B2) {
        use_side_3:  // use side 3
            BAR(1, 2, 3);
            if (maxc == 2) goto done;
            if (B1 < B2) goto contact2_1;
            else goto contact2_2;
        } else {
            BAR(1, 1, 2);  // use side 2
            if (maxc == 2) goto done;
            if (B1 < B3) goto contact2_1;
            else goto contact2_3;
        }
    }

contact2_1:
    BAR(2, 0, 1);
    goto done;
contact2_2:
    BAR(2, 1, 2);
    goto done;
contact2_3:
    BAR(2, 2, 3);
    goto done;
#undef FOO
#undef BAR

done:
    for (int i = 0; i < ret; i++) {
        CONTACT(contact, i * skip)->g1 = o1;
        CONTACT(contact, i * skip)->g2 = o2;
    }
    return ret;
}

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

// 0x8026F590: sphere against box (dCollideSphereBox)
extern "C" int fn_8026F590(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    // this is easy. get the sphere center `p' relative to the box, and then clip
    // that to the boundary of the box (call that point `q'). if q is on the
    // boundary of the box and |p-q| is <= sphere radius, they touch.
    // if q is inside the box, the sphere is inside the box, so set a contact
    // normal to push the sphere to the closest box edge.

    float l[4], t[4], p[4], q[4], r[4];
    float depth;
    int onborder = 0;

    SphereGeom* sphere = (SphereGeom*)o1;
    BoxGeom* box = (BoxGeom*)o2;

    contact->g1 = o1;
    contact->g2 = o2;

    p[0] = o1->pos[0] - o2->pos[0];
    p[1] = o1->pos[1] - o2->pos[1];
    p[2] = o1->pos[2] - o2->pos[2];

    l[0] = box->side[0] * 0.5f;
    t[0] = Dot14(p, o2->rot);
    if (t[0] < -l[0]) {
        t[0] = -l[0];
        onborder = 1;
    }
    if (t[0] > l[0]) {
        t[0] = l[0];
        onborder = 1;
    }

    l[1] = box->side[1] * 0.5f;
    t[1] = Dot14(p, o2->rot + 1);
    if (t[1] < -l[1]) {
        t[1] = -l[1];
        onborder = 1;
    }
    if (t[1] > l[1]) {
        t[1] = l[1];
        onborder = 1;
    }

    t[2] = Dot14(p, o2->rot + 2);
    l[2] = box->side[2] * 0.5f;
    if (t[2] < -l[2]) {
        t[2] = -l[2];
        onborder = 1;
    }
    if (t[2] > l[2]) {
        t[2] = l[2];
        onborder = 1;
    }

    if (!onborder) {
        // sphere center inside box. find closest face to `t'
        float min_distance = l[0] - FAbs(t[0]);
        int mini = 0;
        for (int i = 1; i < 3; i++) {
            float face_distance = l[i] - FAbs(t[i]);
            if (face_distance < min_distance) {
                min_distance = face_distance;
                mini = i;
            }
        }
        // contact position = sphere center
        contact->pos[0] = o1->pos[0];
        contact->pos[1] = o1->pos[1];
        contact->pos[2] = o1->pos[2];
        // contact normal points to closest face
        float tmp[4];
        tmp[0] = 0;
        tmp[1] = 0;
        tmp[2] = 0;
        tmp[mini] = (t[mini] > 0) ? 1.0f : -1.0f;
        MatMulVec(contact->normal, o2->rot, tmp);
        // contact depth = distance to wall along normal plus radius
        contact->depth = min_distance + sphere->radius;
        return 1;
    }

    t[3] = 0;  //@@@ hmmm
    MatMulVec(q, o2->rot, t);
    r[0] = p[0] - q[0];
    r[1] = p[1] - q[1];
    r[2] = p[2] - q[2];
    depth = sphere->radius - SqrtF(Dot(r, r));
    if (depth < 0) {
        return 0;
    }
    contact->pos[0] = q[0] + o2->pos[0];
    contact->pos[1] = q[1] + o2->pos[1];
    contact->pos[2] = q[2] + o2->pos[2];
    contact->normal[0] = r[0];
    contact->normal[1] = r[1];
    contact->normal[2] = r[2];
    fn_8027914C(contact->normal);
    contact->depth = depth;
    return 1;
}

// 0x8026F9F8: sphere against sphere (SphereSphereCollide)
extern "C" int fn_8026F9F8(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    SphereGeom* s1 = (SphereGeom*)o1;
    SphereGeom* s2 = (SphereGeom*)o2;

    contact->g1 = s1;
    contact->g2 = s2;
    return CollideSpheres(o1->pos, s1->radius, o2->pos, s2->radius, contact);
}

extern "C" void fn_80271144(int n, float* p, int m, int i0, int* iret);
extern "C" int fn_80271508(float* h, float* p, float* ret);

// Closest points of two lines (dLineClosestApproach); in collision_util.
void LineClosestApproach(float* pa, float* ua, float* pb, float* ub, float* alpha, float* beta);

// 0x8026FA3C: box-box contact generation (dBoxBox). Given two boxes (p1,R1,side1) and (p2,R2,side2),
// collide them together and generate contact points; returns the number of contacts (0 = none).
extern "C" int fn_8026FA3C(const float* p1, const float* R1, const float* side1, const float* p2,
                           const float* R2, const float* side2, float* normal, float* depth, int* return_code,
                           int maxc, ContactGeom* contact, int skip)
{
    const float fudge_factor = 1.05f;
    float p[4], pp[4], normalC[4];
    const float* normalR = 0;
    float A[3], B[3], R11, R12, R13, R21, R22, R23, R31, R32, R33, Q11, Q12, Q13, Q21, Q22, Q23, Q31, Q32, Q33, s,
        s2, l;
    int i, j, invert_normal, code;

    // get vector from centers of box 1 to box 2, relative to box 1
    p[0] = p2[0] - p1[0];
    p[1] = p2[1] - p1[1];
    p[2] = p2[2] - p1[2];
    MatTransMulVec(pp, R1, p);  // get pp = p relative to body 1

    // get side lengths / 2
    A[0] = side1[0] * 0.5f;
    A[1] = side1[1] * 0.5f;
    A[2] = side1[2] * 0.5f;
    B[0] = side2[0] * 0.5f;
    B[1] = side2[1] * 0.5f;
    B[2] = side2[2] * 0.5f;

    // Rij is R1'*R2, i.e. the relative rotation between R1 and R2
    R11 = Dot44(R1 + 0, R2 + 0);
    R12 = Dot44(R1 + 0, R2 + 1);
    R13 = Dot44(R1 + 0, R2 + 2);
    R21 = Dot44(R1 + 1, R2 + 0);
    R22 = Dot44(R1 + 1, R2 + 1);
    R23 = Dot44(R1 + 1, R2 + 2);
    R31 = Dot44(R1 + 2, R2 + 0);
    R32 = Dot44(R1 + 2, R2 + 1);
    R33 = Dot44(R1 + 2, R2 + 2);

    Q11 = FAbs(R11);
    Q12 = FAbs(R12);
    Q13 = FAbs(R13);
    Q21 = FAbs(R21);
    Q22 = FAbs(R22);
    Q23 = FAbs(R23);
    Q31 = FAbs(R31);
    Q32 = FAbs(R32);
    Q33 = FAbs(R33);

    // for all 15 possible separating axes:
    //   * see if the axis separates the boxes. if so, return 0.
    //   * find the depth of the penetration along the separating axis (s2)
    //   * if this is the largest depth so far, record it.
    // the normal vector will be set to the separating axis with the smallest
    // depth. note: normalR is set to point to a column of R1 or R2 if that is
    // the smallest depth normal so far. otherwise normalR is 0 and normalC is
    // set to a vector relative to body 1. invert_normal is 1 if the sign of
    // the normal should be flipped.

#define TST(expr1, expr2, norm, cc)          \
    s2 = FAbs(expr1) - (expr2);              \
    if (s2 > 0) return 0;                    \
    if (s2 > s) {                            \
        s = s2;                              \
        normalR = norm;                      \
        invert_normal = ((expr1) < 0);       \
        code = (cc);                         \
    }

    s = -FLT_MAX;
    invert_normal = 0;
    code = 0;

    // separating axis = u1,u2,u3
    TST(pp[0], (A[0] + B[0] * Q11 + B[1] * Q12 + B[2] * Q13), R1 + 0, 1);
    TST(pp[1], (A[1] + B[0] * Q21 + B[1] * Q22 + B[2] * Q23), R1 + 1, 2);
    TST(pp[2], (A[2] + B[0] * Q31 + B[1] * Q32 + B[2] * Q33), R1 + 2, 3);

    // separating axis = v1,v2,v3
    TST(Dot41(R2 + 0, p), (A[0] * Q11 + A[1] * Q21 + A[2] * Q31 + B[0]), R2 + 0, 4);
    TST(Dot41(R2 + 1, p), (A[0] * Q12 + A[1] * Q22 + A[2] * Q32 + B[1]), R2 + 1, 5);
    TST(Dot41(R2 + 2, p), (A[0] * Q13 + A[1] * Q23 + A[2] * Q33 + B[2]), R2 + 2, 6);

    // note: cross product axes need to be scaled when s is computed.
    // normal (n1,n2,n3) is relative to box 1.
#undef TST
#define TST(expr1, expr2, n1, n2, n3, cc)                \
    s2 = FAbs(expr1) - (expr2);                          \
    if (s2 > 0) return 0;                                \
    l = SqrtFloat((n1) * (n1) + (n2) * (n2) + (n3) * (n3)); \
    if (l > 0) {                                         \
        s2 /= l;                                         \
        if (s2 * fudge_factor > s) {                     \
            s = s2;                                      \
            normalR = 0;                                 \
            normalC[0] = (n1) / l;                       \
            normalC[1] = (n2) / l;                       \
            normalC[2] = (n3) / l;                       \
            invert_normal = ((expr1) < 0);               \
            code = (cc);                                 \
        }                                                \
    }

    // separating axis = u1 x (v1,v2,v3)
    TST(pp[2] * R21 - pp[1] * R31, (A[1] * Q31 + A[2] * Q21 + B[1] * Q13 + B[2] * Q12), 0, -R31, R21, 7);
    TST(pp[2] * R22 - pp[1] * R32, (A[1] * Q32 + A[2] * Q22 + B[0] * Q13 + B[2] * Q11), 0, -R32, R22, 8);
    TST(pp[2] * R23 - pp[1] * R33, (A[1] * Q33 + A[2] * Q23 + B[0] * Q12 + B[1] * Q11), 0, -R33, R23, 9);

    // separating axis = u2 x (v1,v2,v3)
    TST(pp[0] * R31 - pp[2] * R11, (A[0] * Q31 + A[2] * Q11 + B[1] * Q23 + B[2] * Q22), R31, 0, -R11, 10);
    TST(pp[0] * R32 - pp[2] * R12, (A[0] * Q32 + A[2] * Q12 + B[0] * Q23 + B[2] * Q21), R32, 0, -R12, 11);
    TST(pp[0] * R33 - pp[2] * R13, (A[0] * Q33 + A[2] * Q13 + B[0] * Q22 + B[1] * Q21), R33, 0, -R13, 12);

    // separating axis = u3 x (v1,v2,v3)
    TST(pp[1] * R11 - pp[0] * R21, (A[0] * Q21 + A[1] * Q11 + B[1] * Q33 + B[2] * Q32), -R21, R11, 0, 13);
    TST(pp[1] * R12 - pp[0] * R22, (A[0] * Q22 + A[1] * Q12 + B[0] * Q33 + B[2] * Q31), -R22, R12, 0, 14);
    TST(pp[1] * R13 - pp[0] * R23, (A[0] * Q23 + A[1] * Q13 + B[0] * Q32 + B[1] * Q31), -R23, R13, 0, 15);

#undef TST

    if (!code) {
        return 0;
    }

    // if we get to this point, the boxes interpenetrate. compute the normal
    // in global coordinates.
    if (normalR) {
        normal[0] = normalR[0];
        normal[1] = normalR[4];
        normal[2] = normalR[8];
    } else {
        MatMulVec(normal, R1, normalC);
    }
    if (invert_normal) {
        normal[0] = -normal[0];
        normal[1] = -normal[1];
        normal[2] = -normal[2];
    }
    *depth = -s;

    // compute contact point(s)

    if (code > 6) {
        // an edge from box 1 touches an edge from box 2.
        // find a point pa on the intersecting edge of box 1
        float pa[4];
        float sign;
        for (i = 0; i < 3; i++) {
            pa[i] = p1[i];
        }
        for (j = 0; j < 3; j++) {
            sign = (Dot14(normal, R1 + j) > 0) ? 1.0f : -1.0f;
            for (i = 0; i < 3; i++) {
                pa[i] += sign * A[j] * R1[i * 4 + j];
            }
        }

        // find a point pb on the intersecting edge of box 2
        float pb[4];
        for (i = 0; i < 3; i++) {
            pb[i] = p2[i];
        }
        for (j = 0; j < 3; j++) {
            sign = (Dot14(normal, R2 + j) > 0) ? -1.0f : 1.0f;
            for (i = 0; i < 3; i++) {
                pb[i] += sign * B[j] * R2[i * 4 + j];
            }
        }

        float alpha, beta;
        float ua[4], ub[4];
        for (i = 0; i < 3; i++) {
            ua[i] = R1[((code)-7) / 3 + i * 4];
        }
        for (i = 0; i < 3; i++) {
            ub[i] = R2[((code)-7) % 3 + i * 4];
        }

        LineClosestApproach(pa, ua, pb, ub, &alpha, &beta);
        for (i = 0; i < 3; i++) {
            pa[i] += ua[i] * alpha;
        }
        for (i = 0; i < 3; i++) {
            pb[i] += ub[i] * beta;
        }

        for (i = 0; i < 3; i++) {
            contact[0].pos[i] = 0.5f * (pa[i] + pb[i]);
        }
        contact[0].depth = *depth;
        *return_code = code;
        return 1;
    }

    // okay, we have a face-something intersection (because the separating
    // axis is perpendicular to a face). define face 'a' to be the reference
    // face (i.e. the normal vector is perpendicular to this) and face 'b' to be
    // the incident face (the closest face of the other box).

    const float *Ra, *Rb, *pa, *pb, *Sa, *Sb;
    if (code <= 3) {
        Ra = R1;
        Rb = R2;
        pa = p1;
        pb = p2;
        Sa = A;
        Sb = B;
    } else {
        Ra = R2;
        Rb = R1;
        pa = p2;
        pb = p1;
        Sa = B;
        Sb = A;
    }

    // nr = normal vector of reference face dotted with axes of incident box.
    // anr = absolute values of nr.
    float normal2[4], nr[4], anr[4];
    if (code <= 3) {
        normal2[0] = normal[0];
        normal2[1] = normal[1];
        normal2[2] = normal[2];
    } else {
        normal2[0] = -normal[0];
        normal2[1] = -normal[1];
        normal2[2] = -normal[2];
    }
    MatTransMulVec(nr, Rb, normal2);
    anr[0] = FAbs(nr[0]);
    anr[1] = FAbs(nr[1]);
    anr[2] = FAbs(nr[2]);

    // find the largest compontent of anr: this corresponds to the normal
    // for the indident face. the other axis numbers of the indicent face
    // are stored in a1,a2.
    int lanr, a1, a2;
    if (anr[1] > anr[0]) {
        if (anr[1] > anr[2]) {
            a1 = 0;
            lanr = 1;
            a2 = 2;
        } else {
            a1 = 0;
            a2 = 1;
            lanr = 2;
        }
    } else {
        if (anr[0] > anr[2]) {
            lanr = 0;
            a1 = 1;
            a2 = 2;
        } else {
            a1 = 0;
            a2 = 1;
            lanr = 2;
        }
    }

    // compute center point of incident face, in reference-face coordinates
    float center[4];
    if (nr[lanr] < 0) {
        for (i = 0; i < 3; i++) {
            center[i] = pb[i] - pa[i] + Sb[lanr] * Rb[i * 4 + lanr];
        }
    } else {
        for (i = 0; i < 3; i++) {
            center[i] = pb[i] - pa[i] - Sb[lanr] * Rb[i * 4 + lanr];
        }
    }

    // find the normal and non-normal axis numbers of the reference box
    int codeN, code1, code2;
    if (code <= 3) {
        codeN = code - 1;
    } else {
        codeN = code - 4;
    }
    if (codeN == 0) {
        code1 = 1;
        code2 = 2;
    } else if (codeN == 1) {
        code1 = 0;
        code2 = 2;
    } else {
        code1 = 0;
        code2 = 1;
    }

    // find the four corners of the incident face, in reference-face coordinates
    float quad[8];  // 2D coordinate of incident face (x,y pairs)
    float c1, c2, m11, m12, m21, m22;
    c1 = Dot14(center, Ra + code1);
    c2 = Dot14(center, Ra + code2);
    // optimize this? - we have already computed this data above, but it is not
    // stored in an easy-to-index format. for now it's quicker just to recompute
    // the four dot products.
    m11 = Dot44(Ra + code1, Rb + a1);
    m12 = Dot44(Ra + code1, Rb + a2);
    m21 = Dot44(Ra + code2, Rb + a1);
    m22 = Dot44(Ra + code2, Rb + a2);
    {
        float k1 = m11 * Sb[a1];
        float k2 = m21 * Sb[a1];
        float k3 = m12 * Sb[a2];
        float k4 = m22 * Sb[a2];
        quad[0] = c1 - k1 - k3;
        quad[1] = c2 - k2 - k4;
        quad[2] = c1 - k1 + k3;
        quad[3] = c2 - k2 + k4;
        quad[4] = c1 + k1 + k3;
        quad[5] = c2 + k2 + k4;
        quad[6] = c1 + k1 - k3;
        quad[7] = c2 + k2 - k4;
    }

    // find the size of the reference face
    float rect[2];
    rect[0] = Sa[code1];
    rect[1] = Sa[code2];

    // intersect the incident and reference faces
    float ret[16];
    int n = fn_80271508(rect, quad, ret);
    if (n < 1) {
        return 0;  // this should never happen
    }

    // convert the intersection points into reference-face coordinates,
    // and compute the contact position and depth for each point. only keep
    // those points that have a positive (penetrating) depth. delete points in
    // the 'ret' array as necessary so that 'point' and 'ret' correspond.
    float point[3 * 8];  // penetrating contact points
    float dep[8];        // depths for those points
    float det1 = 1.0f / (m11 * m22 - m12 * m21);
    m11 *= det1;
    m12 *= det1;
    m21 *= det1;
    m22 *= det1;
    int cnum = 0;  // number of penetrating contact points found
    for (j = 0; j < n; j++) {
        float k1 = m22 * (ret[j * 2] - c1) - m12 * (ret[j * 2 + 1] - c2);
        float k2 = -m21 * (ret[j * 2] - c1) + m11 * (ret[j * 2 + 1] - c2);
        for (i = 0; i < 3; i++) {
            point[cnum * 3 + i] = center[i] + k1 * Rb[i * 4 + a1] + k2 * Rb[i * 4 + a2];
        }
        dep[cnum] = Sa[codeN] - Dot(normal2, point + cnum * 3);
        if (dep[cnum] >= 0) {
            ret[cnum * 2] = ret[j * 2];
            ret[cnum * 2 + 1] = ret[j * 2 + 1];
            cnum++;
        }
    }
    if (cnum < 1) {
        return 0;  // this should never happen
    }

    // we can't generate more contacts than we actually have
    if (maxc > cnum) {
        maxc = cnum;
    }
    if (maxc < 1) {
        maxc = 1;
    }

    if (cnum <= maxc) {
        // we have less contacts than we need, so we use them all
        for (j = 0; j < cnum; j++) {
            ContactGeom* con = CONTACT(contact, skip * j);
            for (i = 0; i < 3; i++) {
                con->pos[i] = point[j * 3 + i] + pa[i];
            }
            con->depth = dep[j];
        }
    } else {
        // we have more contacts than are wanted, some of them must be culled.
        // find the deepest point, it is always the first contact.
        int i1 = 0;
        float maxdepth = dep[0];
        for (i = 1; i < cnum; i++) {
            if (dep[i] > maxdepth) {
                maxdepth = dep[i];
                i1 = i;
            }
        }

        int iret[8];
        fn_80271144(cnum, ret, maxc, i1, iret);

        for (j = 0; j < maxc; j++) {
            ContactGeom* con = CONTACT(contact, skip * j);
            for (i = 0; i < 3; i++) {
                con->pos[i] = point[iret[j] * 3 + i] + pa[i];
            }
            con->depth = dep[iret[j]];
        }
        cnum = maxc;
    }

    *return_code = code;
    return cnum;
}

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

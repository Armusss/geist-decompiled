// u_8027243C.cpp (0x8027243C-0x8027337C): ODE 0.5 collision_util.cpp: dClosestLineBoxPoints, the segment helper,
// dLineClosestApproach and dCollideSpheres. Split (2026-11-27) from u_80272060 (collision_transform.cpp ends at
// 0x8027243C). Evidence: own .sdata2 pool 0x804DA228-0x804DA258 (0.5f, -1, 1, 2 and the 0.5/3.0 doubles again).
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



static inline float Dot(const float* a, const float* b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static inline float Dot14(const float* a, const float* b)
{
    return a[0] * b[0] + a[1] * b[4] + a[2] * b[8];
}

// The collider dispatcher (dCollide, collision_kernel.cpp).
int GeomCollideDispatch(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

static inline float Dot41(const float* a, const float* b)
{
    return a[0] * b[0] + a[4] * b[1] + a[8] * b[2];
}

// odemath.h matrix-vector products
static inline void dMULTIPLY0_331(float* A, const float* B, const float* C)
{
    A[0] = Dot(B, C);
    A[1] = Dot(B + 4, C);
    A[2] = Dot(B + 8, C);
}

static inline void dMULTIPLY1_331(float* A, const float* B, const float* C)
{
    A[0] = Dot41(B, C);
    A[1] = Dot41(B + 1, C);
    A[2] = Dot41(B + 2, C);
}

// 0x8027243C: closest points between the line segment p1-p2 and the box (c,R,side) (dClosestLineBoxPoints).
// lret is the point on the segment and bret the point on the box.
extern "C" void fn_8027243C(const float* p1, const float* p2, const float* c, const float* R, const float* side,
                            float* lret, float* bret)
{
    int i;

    // compute the start and delta of the line p1-p2 relative to the box.
    // we will do all subsequent computations in this box-relative coordinate
    // system. we have to do a translation and rotation for each point.
    float tmp[4], s[4], v[4];
    tmp[0] = p1[0] - c[0];
    tmp[1] = p1[1] - c[1];
    tmp[2] = p1[2] - c[2];
    dMULTIPLY1_331(s, R, tmp);
    tmp[0] = p2[0] - p1[0];
    tmp[1] = p2[1] - p1[1];
    tmp[2] = p2[2] - p1[2];
    dMULTIPLY1_331(v, R, tmp);

    // mirror the line so that v has all components >= 0
    float sign[4];
    for (i = 0; i < 3; i++) {
        if (v[i] < 0) {
            s[i] = -s[i];
            v[i] = -v[i];
            sign[i] = -1;
        } else {
            sign[i] = 1;
        }
    }

    // compute v^2
    float v2[4];
    v2[0] = v[0] * v[0];
    v2[1] = v[1] * v[1];
    v2[2] = v[2] * v[2];

    // compute the half-sides of the box
    float h[3];
    h[0] = 0.5f * side[0];
    h[1] = 0.5f * side[1];
    h[2] = 0.5f * side[2];

    // region is -1,0,+1 depending on which side of the box planes each
    // coordinate is on. tanchor is the next t value at which there is a
    // transition, or the last one if there are no more.
    int region[3];
    float tanchor[3];

    // find the region and tanchor values for p1
    for (i = 0; i < 3; i++) {
        if (v[i] > 0) {
            if (s[i] < -h[i]) {
                region[i] = -1;
                tanchor[i] = (-h[i] - s[i]) / v[i];
            } else {
                region[i] = (s[i] > h[i]);
                tanchor[i] = (h[i] - s[i]) / v[i];
            }
        } else {
            region[i] = 0;
            tanchor[i] = 2;  // this will never be a valid tanchor
        }
    }

    // compute d|d|^2/dt for t=0. if it's >= 0 then p1 is the closest point
    float t = 0;
    float dd2dt = 0;
    for (i = 0; i < 3; i++) {
        dd2dt -= (region[i] ? v2[i] : 0) * tanchor[i];
    }
    if (dd2dt >= 0) {
        goto got_answer;
    }

    do {
        // find the point on the line that is at the next clip plane boundary
        float next_t = 1;
        for (i = 0; i < 3; i++) {
            if (tanchor[i] > t && tanchor[i] < 1 && tanchor[i] < next_t) {
                next_t = tanchor[i];
            }
        }

        // compute d|d|^2/dt for the next t
        float next_dd2dt = 0;
        for (i = 0; i < 3; i++) {
            next_dd2dt += (region[i] ? v2[i] : 0) * (next_t - tanchor[i]);
        }

        // if the sign of d|d|^2/dt has changed, solution = the crossover point
        if (next_dd2dt >= 0) {
            float m = (next_dd2dt - dd2dt) / (next_t - t);
            t -= dd2dt / m;
            goto got_answer;
        }

        // advance to the next anchor point / region
        for (i = 0; i < 3; i++) {
            if (tanchor[i] == next_t) {
                tanchor[i] = (h[i] - s[i]) / v[i];
                region[i]++;
            }
        }
        t = next_t;
        dd2dt = next_dd2dt;
    } while (t < 1);
    t = 1;

got_answer:

    // compute closest point on the line
    for (i = 0; i < 3; i++) {
        lret[i] = p1[i] + t * tmp[i];  // note: tmp=p2-p1
    }

    // compute closest point on the box
    for (i = 0; i < 3; i++) {
        tmp[i] = sign[i] * (s[i] + t * v[i]);
        if (tmp[i] < -h[i]) {
            tmp[i] = -h[i];
        } else if (tmp[i] > h[i]) {
            tmp[i] = h[i];
        }
    }
    dMULTIPLY0_331(s, R, tmp);
    for (i = 0; i < 3; i++) {
        bret[i] = s[i] + c[i];
    }
}

// 0x80272A90: closest points between the segments a1-a2 and b1-b2 (dClosestLineSegmentPoints)
extern "C" void fn_80272A90(const float* a1, const float* a2, const float* b1, const float* b2, float* cp1,
                            float* cp2)
{
    float a1a2[4], b1b2[4], a1b1[4], a1b2[4], a2b1[4], a2b2[4], n[4];
    float la, lb, k, da1, da2, da3, da4, db1, db2, db3, db4, det;

#define SET2(a, b) \
    a[0] = b[0];   \
    a[1] = b[1];   \
    a[2] = b[2];
#define SET3(a, b, op, c) \
    a[0] = b[0] op c[0];  \
    a[1] = b[1] op c[1];  \
    a[2] = b[2] op c[2];

    // check vertex-vertex features

    SET3(a1a2, a2, -, a1);
    SET3(b1b2, b2, -, b1);
    SET3(a1b1, b1, -, a1);
    da1 = Dot(a1a2, a1b1);
    db1 = Dot(b1b2, a1b1);
    if (da1 <= 0 && db1 >= 0) {
        SET2(cp1, a1);
        SET2(cp2, b1);
        return;
    }

    SET3(a1b2, b2, -, a1);
    da2 = Dot(a1a2, a1b2);
    db2 = Dot(b1b2, a1b2);
    if (da2 <= 0 && db2 <= 0) {
        SET2(cp1, a1);
        SET2(cp2, b2);
        return;
    }

    SET3(a2b1, b1, -, a2);
    da3 = Dot(a1a2, a2b1);
    db3 = Dot(b1b2, a2b1);
    if (da3 >= 0 && db3 >= 0) {
        SET2(cp1, a2);
        SET2(cp2, b1);
        return;
    }

    SET3(a2b2, b2, -, a2);
    da4 = Dot(a1a2, a2b2);
    db4 = Dot(b1b2, a2b2);
    if (da4 >= 0 && db4 <= 0) {
        SET2(cp1, a2);
        SET2(cp2, b2);
        return;
    }

    // check edge-vertex features.
    // if one or both of the lines has zero length, we will never get to here,
    // so we do not have to worry about the following divisions by zero.

    la = Dot(a1a2, a1a2);
    if (da1 >= 0 && da3 <= 0) {
        k = da1 / la;
        SET3(n, a1b1, -, k * a1a2);
        if (Dot(b1b2, n) >= 0) {
            SET3(cp1, a1, +, k * a1a2);
            SET2(cp2, b1);
            return;
        }
    }

    if (da2 >= 0 && da4 <= 0) {
        k = da2 / la;
        SET3(n, a1b2, -, k * a1a2);
        if (Dot(b1b2, n) <= 0) {
            SET3(cp1, a1, +, k * a1a2);
            SET2(cp2, b2);
            return;
        }
    }

    lb = Dot(b1b2, b1b2);
    if (db1 <= 0 && db2 >= 0) {
        k = -db1 / lb;
        SET3(n, -a1b1, -, k * b1b2);
        if (Dot(a1a2, n) >= 0) {
            SET2(cp1, a1);
            SET3(cp2, b1, +, k * b1b2);
            return;
        }
    }

    if (db3 <= 0 && db4 >= 0) {
        k = -db3 / lb;
        SET3(n, -a2b1, -, k * b1b2);
        if (Dot(a1a2, n) <= 0) {
            SET2(cp1, a2);
            SET3(cp2, b1, +, k * b1b2);
            return;
        }
    }

    // it must be edge-edge

    k = Dot(a1a2, b1b2);
    det = la * lb - k * k;
    if (det <= 0) {
        // this should never happen, but just in case...
        SET2(cp1, a1);
        SET2(cp2, b1);
        return;
    }
    det = 1.0f / det;
    float alpha = (lb * da1 - k * db1) * det;
    float beta = (k * da1 - la * db1) * det;
    SET3(cp1, a1, +, alpha * a1a2);
    SET3(cp2, b1, +, beta * b1b2);

#undef SET2
#undef SET3
}

// 0x802730D0: closest approach of two lines (dLineClosestApproach)
void LineClosestApproach(float* pa, float* ua, float* pb, float* ub, float* alpha, float* beta)
{
    float p[4];
    p[0] = pb[0] - pa[0];
    p[1] = pb[1] - pa[1];
    p[2] = pb[2] - pa[2];
    float uaub = Dot(ua, ub);
    float q1 = Dot(ua, p);
    float q2 = -Dot(ub, p);
    float d = 1 - uaub * uaub;

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
int CollideSpheres(float* p1, float r1, float* p2, float r2, ContactGeom* c)
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
        float d1 = 1.0f / d;
        c->normal[0] = (p1[0] - p2[0]) * d1;
        c->normal[1] = (p1[1] - p2[1]) * d1;
        c->normal[2] = (p1[2] - p2[2]) * d1;
        float k = 0.5f * (r2 - r1 - d);
        c->pos[0] = p1[0] + c->normal[0] * k;
        c->pos[1] = p1[1] + c->normal[1] * k;
        c->pos[2] = p1[2] + c->normal[2] * k;
        c->depth = r1 + r2 - d;
    }
    return 1;
}

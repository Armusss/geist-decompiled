// u_80272060.cpp (0x80272060-0x8027337C): ODE collision utilities (box-plane, box-box helpers, segment
// closest points, LineClosestApproach, CollideSpheres; ODE's collision_util.cpp). Split from u_8026C200.
// Evidence: own .sdata2 pool 0x804DA228-0x804DA258 (0.5f, -1, 1, 2 and the 0.5/3.0 doubles again).
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



// TODO (not drafted yet, 0x1A4 bytes at 0x80272060): box against plane
extern "C" int fn_80272060(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);

// TODO (not drafted yet, 0x238 bytes at 0x80272204): box-box helper (separating axis tests)
extern "C" void fn_80272204(void);

// TODO (not drafted yet, 0x654 bytes at 0x8027243C): closest points between a line segment and a box
extern "C" void fn_8027243C(float* p1, float* p2, float* c, float* R, float* side, float* lret, float* bret);

// TODO (not drafted yet, 0x640 bytes at 0x80272A90): closest points between two box segments
extern "C" void fn_80272A90(void);

static inline float Dot(const float* a, const float* b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
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

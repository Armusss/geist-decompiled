// u_80272060.cpp (0x80272060-0x8027243C): ODE 0.5 collision_transform.cpp: dCollideTransform (the class-6
// collider installed by initColliders, see u_8026A37C) and dxGeomTransform::computeFinalTx, the only parts of
// the file that survive the link. Split (2026-11-27) from the old u_80272060 range: from 0x8027243C on it is
// collision_util.cpp (u_8027243C, which owns the .sdata2 pool); this file references no data.
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

// ---- ODE 0.5 collision_transform.cpp (0x80272060-0x8027243C): the geometry transform class (dxGeomTransform),
// registered for class 6 by initColliders. Only its collider and computeFinalTx survive the link.
struct GeomTransform : Geom {
    Geom* obj;           // 0x4c  the transformed geometry
    int cleanup;         // 0x50
    int infomode;        // 0x54  report the transform itself as g1
    float finalPos[4];   // 0x58
    float finalR[12];    // 0x68

    void computeFinalTx();
};

// dMULTIPLY0_331 / dMULTIPLY0_333 (odemath.h): A = B*C.
static inline void Multiply0_331(float* A, const float* B, const float* C)
{
    A[0] = Dot(B, C);
    A[1] = Dot(B + 4, C);
    A[2] = Dot(B + 8, C);
}

static inline void Multiply0_333(float* A, const float* B, const float* C)
{
    A[0] = Dot14(B, C);
    A[1] = Dot14(B, C + 1);
    A[2] = Dot14(B, C + 2);
    A[4] = Dot14(B + 4, C);
    A[5] = Dot14(B + 4, C + 1);
    A[6] = Dot14(B + 4, C + 2);
    A[8] = Dot14(B + 8, C);
    A[9] = Dot14(B + 8, C + 1);
    A[10] = Dot14(B + 8, C + 2);
}

// 0x80272060: collides a transformed geometry (dCollideTransform)
extern "C" int fn_80272060(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    GeomTransform* tr = (GeomTransform*)o1;
    if (!tr->obj) {
        return 0;
    }

    float* posbak = tr->obj->pos;
    float* Rbak = tr->obj->rot;
    Body* bodybak = tr->obj->body;

    if (tr->flags & GEOM_AABB_BAD) {
        tr->computeFinalTx();
    }
    tr->obj->pos = tr->finalPos;
    tr->obj->rot = tr->finalR;
    tr->obj->body = o1->body;

    int n = GeomCollideDispatch(tr->obj, o2, flags, contact, skip);

    if (tr->infomode) {
        for (int i = 0; i < n; i++) {
            ContactGeom* c = (ContactGeom*)((char*)contact + skip * i);
            c->g1 = o1;
        }
    }

    tr->obj->pos = posbak;
    tr->obj->rot = Rbak;
    tr->obj->body = bodybak;
    return n;
}

// 0x80272204: final transform of the encapsulated geometry (dxGeomTransform::computeFinalTx)
void GeomTransform::computeFinalTx()
{
    Multiply0_331(finalPos, rot, obj->pos);
    finalPos[0] += pos[0];
    finalPos[1] += pos[1];
    finalPos[2] += pos[2];
    Multiply0_333(finalR, rot, obj->rot);
}

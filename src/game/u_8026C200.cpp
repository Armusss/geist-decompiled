// u_8026C200.cpp (0x8026BE98-0x8026D428): ODE 0.5 collision_space.cpp: dGeomMoved, the dxSpace/dxHashSpace
// methods, dHashSpaceCreate, the dSpace* API (dHashSpaceSetLevels .. dSpaceCollide2) and the implicit ~dxHashSpace.
// Boundary (2026-11-27): starts at 0x8026BE98, not 0x8026C200. 0x8026BE98 (was SpaceDestroy__FP5Spaces in
// u_8026A37C) is the compiler-generated ~HashSpace (stores __vt__9HashSpace then __vt__5Space, inlines ~Space,
// frees 0x6C bytes); u_8026A37C (collision_kernel.cpp) ends with space_geom_collider at 0x8026BB0C. The file
// keeps its old name. Data: .data 0x803C03B8-0x803C0498 (hash-table prime sizes, __vt__9HashSpace 0x803C0434,
// __vt__5Space 0x803C0464), .sdata2 0x804DA1C0-0x804DA1D0.
// Built with -inline deferred like the other ODE files (user-approved 2026-11-27): the functions are in ODE source
// order and come out reversed, with ~HashSpace and __vt__9HashSpace first, as in retail.
// The remaining names are descriptive guesses; the code is matched against the retail DOL.

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
enum { GEOM_DIRTY = 1, GEOM_AABB_BAD = 2, GEOM_ENABLED = 8, GEOM_STATIC = 0x10 };

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
double frexp(double x, int* exp);
double ldexp(double x, int exp);
double floor(double x);
}

// Hash-table sizes: the largest prime below each power of two (prime).
static int prime[31] = {
    1, 2, 3, 7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749, 65521, 131071, 262139,
    524287, 1048573, 2097143, 4194301, 8388593, 16777213, 33554393, 67108859, 134217689, 268435399,
    536870909, 1073741789,
};

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

// Destroys a geometry (dGeomDestroy); lives in the neighbouring unit.
void GeomDestroyGeom(Geom* g);

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
    if (a1[0] > a2[1] || a1[1] < a2[0] || a1[2] > a2[3] || a1[3] < a2[2] || a1[4] > a2[5]
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

// An axis-aligned box in the hash space's working lists (dxAABB).
struct HashAabb {
    HashAabb* next;  // 0x00 next box in its list
    int level;       // 0x04 cell size is 2^level
    int dbounds[6];  // 0x08 bounds in cells
    Geom* geom;      // 0x20
    int index;       // 0x24 position in the main list
};

// One cell entry of the hash table (Node).
struct HashNode {
    HashNode* next;  // 0x00
    int x, y, z;     // 0x04 cell position
    HashAabb* aabb;  // 0x10
};

// ALLOCA: stack memory rounded up to 16 bytes.
#define ALLOCA16(n) ((char*)((((u32)__alloca((n) + 15)) - 1 | 15) + 1))

// Smallest level whose cell size covers the box (findLevel).
static inline int FindLevel(float* bounds)
{
    float q, q2;
    int level;

    if (bounds[0] <= -3.4028235e38f || bounds[1] >= 3.4028235e38f || bounds[2] <= -3.4028235e38f
        || bounds[3] >= 3.4028235e38f || bounds[4] <= -3.4028235e38f || bounds[5] >= 3.4028235e38f) {
        return 0x7fffffff;
    }
    q = bounds[1] - bounds[0];
    q2 = bounds[3] - bounds[2];
    if (q2 > q) {
        q = q2;
    }
    q2 = bounds[5] - bounds[4];
    if (q2 > q) {
        q = q2;
    }
    frexp(q, &level);
    return level;
}

// Hash key of a cell (getVirtualAddress).
static inline u32 CellAddress(int level, int x, int y, int z)
{
    return level * 1000 + x * 100 + y * 10 + z;
}

// collideAABBs for the hash lists: two geometries that are both flagged static never collide.
static inline void CollideHashAabbs(HashAabb* a1, HashAabb* a2, void* data, SpaceCallback cb)
{
    if ((a1->geom->flags & GEOM_STATIC) != 0 && (a2->geom->flags & GEOM_STATIC) != 0) {
        return;
    }
    CollideAabbs(a1->geom, a2->geom, data, cb);
}

// 0x8026D3A0: flags a geometry and its parents moved (dGeomMoved)
extern "C" void fn_8026D3A0(Geom* g)
{
    GeomMoved(g);
}
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
        dSetZero(aabb, 6);
    }
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
// 0x8026D050: adds a geometry to the space (dxSpace::add)
void Space::add(Geom* g, int placeable)
{
    g->parentSpace = this;
    GeomLink(g, &first);
    count++;
    currentGeom = 0;
    g->flags |= GEOM_DIRTY | GEOM_AABB_BAD;
    if (placeable == 0) {
        g->flags |= GEOM_STATIC;
    }
    GeomMoved(this);
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
// 0x8026CF38: moves a geometry to the head of the list (dxSpace::dirty)
void Space::dirty(Geom* g)
{
    GeomUnlink(g);
    GeomLink(g, &first);
}
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
// 0x8026C448: collides every geometry of the space against the others (dxHashSpace::collide)
void HashSpace::collide(void* data, SpaceCallback cb)
{
    Geom* geom;
    int i, maxlevel;

    if (count < 2) {
        return;
    }
    lockCount++;
    cleanGeoms();

    int n = 0;
    HashAabb* first_aabb = 0;
    HashAabb* big_boxes = 0;
    maxlevel = minLevel - 1;
    for (geom = first; geom != 0; geom = geom->next) {
        if ((geom->flags & GEOM_ENABLED) == 0) {
            continue;
        }
        HashAabb* aabb = (HashAabb*)ALLOCA16(sizeof(HashAabb));
        aabb->geom = geom;
        int level = FindLevel(geom->aabb);
        if (level < minLevel) {
            level = minLevel;
        }
        if (level <= maxLevel) {
            aabb->next = first_aabb;
            first_aabb = aabb;
            aabb->level = level;
            if (level > maxlevel) {
                maxlevel = level;
            }
            float cellsize = (float)ldexp(1.0, level);
            for (i = 0; i < 6; i++) {
                aabb->dbounds[i] = (int)(float)floor(geom->aabb[i] / cellsize);
            }
            aabb->index = n;
            n++;
        } else {
            aabb->next = big_boxes;
            big_boxes = aabb;
        }
    }

    int tested_rowsize = (n + 7) >> 3;
    unsigned char* tested = (unsigned char*)__alloca(n * tested_rowsize);
    memset(tested, 0, n * tested_rowsize);

    for (i = 0; i < 31; i++) {
        if (prime[i] >= 8 * n) {
            break;
        }
    }
    if (i >= 31) {
        i = 30;
    }
    int sz = prime[i];

    HashNode** table = (HashNode**)ALLOCA16(sizeof(HashNode*) * sz);
    for (i = 0; i < sz; i++) {
        table[i] = 0;
    }

    HashAabb* aabb;
    for (aabb = first_aabb; aabb != 0; aabb = aabb->next) {
        int* dbounds = aabb->dbounds;
        for (int xi = dbounds[0]; xi <= dbounds[1]; xi++) {
            for (int yi = dbounds[2]; yi <= dbounds[3]; yi++) {
                for (int zi = dbounds[4]; zi <= dbounds[5]; zi++) {
                    u32 hi = CellAddress(aabb->level, xi, yi, zi) % sz;
                    HashNode* node = (HashNode*)__alloca(sizeof(HashNode));
                    node->x = xi;
                    node->y = yi;
                    node->z = zi;
                    node->aabb = aabb;
                    node->next = table[hi];
                    table[hi] = node;
                }
            }
        }
    }

    int db[6];
    for (aabb = first_aabb; aabb != 0; aabb = aabb->next) {
        for (i = 0; i < 6; i++) {
            db[i] = aabb->dbounds[i];
        }
        for (int level = aabb->level; level <= maxlevel; level++) {
            for (int xi = db[0]; xi <= db[1]; xi++) {
                for (int yi = db[2]; yi <= db[3]; yi++) {
                    for (int zi = db[4]; zi <= db[5]; zi++) {
                        u32 hi = CellAddress(level, xi, yi, zi) % sz;
                        HashNode* node;
                        for (node = table[hi]; node != 0; node = node->next) {
                            if (node->aabb == aabb) {
                                continue;
                            }
                            if (node->aabb->level == level && node->x == xi && node->y == yi && node->z == zi) {
                                unsigned char mask;
                                if (aabb->index <= node->aabb->index) {
                                    i = (aabb->index * tested_rowsize) + (node->aabb->index >> 3);
                                    mask = 1 << (node->aabb->index & 7);
                                } else {
                                    i = (node->aabb->index * tested_rowsize) + (aabb->index >> 3);
                                    mask = 1 << (aabb->index & 7);
                                }
                                if ((tested[i] & mask) == 0) {
                                    CollideHashAabbs(aabb, node->aabb, data, cb);
                                }
                                tested[i] |= mask;
                            }
                        }
                    }
                }
            }
            for (i = 0; i < 6; i++) {
                db[i] >>= 1;
            }
        }
    }

    for (aabb = first_aabb; aabb != 0; aabb = aabb->next) {
        for (HashAabb* aabb2 = big_boxes; aabb2 != 0; aabb2 = aabb2->next) {
            CollideHashAabbs(aabb, aabb2, data, cb);
        }
    }
    for (aabb = big_boxes; aabb != 0; aabb = aabb->next) {
        for (HashAabb* aabb2 = aabb->next; aabb2 != 0; aabb2 = aabb2->next) {
            CollideHashAabbs(aabb, aabb2, data, cb);
        }
    }
    lockCount--;
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
// 0x8026C200: allocates a hash space (dHashSpaceCreate)
Space* HashSpaceCreate(Space* parent)
{
    return new HashSpace(parent);
}
// 0x8026C1F4 (dHashSpaceSetLevels)
void SpaceSetFields(Space* s, int a, int b)
{
    ((HashSpace*)s)->minLevel = a;
    ((HashSpace*)s)->maxLevel = b;
}
// 0x8026C1D4 (dSpaceDestroy)
void GeomDelete(Geom* g)
{
    GeomDestroyGeom(g);
}
// 0x8026C1A8 (dSpaceAdd)
void SpaceAddGeom(Space* s, Geom* g, int arg)
{
    s->add(g, arg);
}
// 0x8026C17C (dSpaceRemove)
void SpaceRemoveGeom(Space* s, Geom* g)
{
    s->remove(g);
}
// 0x8026C174: number of geometries in a space (dSpaceGetNumGeoms)
int SpaceGetCount(Space* s)
{
    return s->count;
}
// 0x8026C148: n-th geometry of a space (dSpaceGetGeom). The symbol keeps its one-parameter guess
// (SpaceGetNumGeoms__FP4Geom), so the index is never named and passes through in r4 to the getGeom
// slot (0x14); the real prototype is Geom* (Space*, int).
struct SpaceGetGeomCall {
    char pad00[0x48];
    virtual void slot08();
    virtual void slot0C();
    virtual void slot10();
    virtual int getGeom();  // 0x14
};

int SpaceGetNumGeoms(Geom* s)
{
    return ((SpaceGetGeomCall*)s)->getGeom();
}

// 0x8026C11C: collides the geometries of a space with each other (dSpaceCollide)
void SpaceCollideSelf(Geom* s, void* data, SpaceCallback callback)
{
    ((Space*)s)->collide(data, callback);
}
// 0x8026BF70: collides two geometries, either of which may be a space (dSpaceCollide2)
void SpaceCollidePair(Geom* g1, Geom* g2, void* data, SpaceCallback callback)
{
    Space* s1;
    Space* s2;

    if (g1->type >= 8 && g1->type <= 10) {
        s1 = (Space*)g1;
    } else {
        s1 = 0;
    }
    if (g2->type >= 8 && g2->type <= 10) {
        s2 = (Space*)g2;
    } else {
        s2 = 0;
    }
    if (s1 != 0) {
        if (s2 != 0) {
            if (s1 == s2) {
                s1->collide(data, callback);
            } else if (s1->count < s2->count) {
                Geom* g = s1->first;
                while (g != 0) {
                    s2->collide2(data, g, callback);
                    g = g->next;
                }
            } else {
                Geom* g = s2->first;
                while (g != 0) {
                    s1->collide2(data, g, callback);
                    g = g->next;
                }
            }
        } else {
            s1->collide2(data, g2, callback);
        }
    } else {
        if (s2 != 0) {
            s2->collide2(data, g1, callback);
        } else {
            callback(data, g1, g2);
        }
    }
}
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

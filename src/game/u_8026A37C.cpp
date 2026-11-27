// u_8026A37C.cpp (0x8026A37C-0x8026BE98): ODE 0.5 collision_kernel.cpp: the user geometry class (dxUserGeom,
// dCreateGeomClass), the dGeom* API, dxGeom's constructor/destructor, dCollide and the class-pair collider matrix.
// It ends with space_geom_collider (0x8026BB0C); collision_space.cpp (u_8026C200) starts at 0x8026BE98.
// Built with -inline deferred like the other ODE files (user-approved 2026-11-27): the functions are in ODE source
// order and come out reversed; ~UserGeom inlines ~Geom.
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

// One entry of the class-pair collider matrix (dColliderEntry).
struct ColliderEntry {
    ColliderFn fn;    // 0x00
    int swap;         // 0x04
};

// A class registered at run time through GeomRegisterClass (dGeomClass).
struct UserGeomClass {
    u32 size;                                   // 0x00  per-geometry user data size
    ColliderFn (*getCollider)(int type);        // 0x04  pick a collider for the other type
    void (*computeAabb)(Geom* g, float* aabb);  // 0x08
    int (*test)(Geom* g, Geom* other, float* aabb); // 0x0c
    void (*release)(Geom* g);                   // 0x10
};

// A rigid body (only the fields used here).
struct Body {
    char pad00[0x1c];   // 0x00
    Geom* firstGeom;    // 0x1c
    char pad20[0x78];   // 0x20
    float pos[4];       // 0x98
    char padA8[0x10];   // 0xa8
    float rot[12];      // 0xb8
};

// ODE memory.cpp / error.cpp / matrix.cpp.
void* dAlloc(u32 size);
void dDebug(int level, const char* msg, ...);
void dFree(void* block, u32 size);
void dSetZero(float* p, int n);

enum { GEOM_DIRTY = 1, GEOM_AABB_BAD = 2, GEOM_PLACEABLE = 4, GEOM_ENABLED = 8 };

// The geometry base class (dxGeom); the vptr follows the data at 0x48.
struct Geom {
    int type;           // 0x00
    u32 flags;          // 0x04
    void* data;         // 0x08
    Body* body;         // 0x0c
    Geom* bodyNext;     // 0x10  next geometry of the same body
    float* pos;         // 0x14
    float* rot;         // 0x18
    Geom* next;         // 0x1c  next geometry of the same space
    Geom** tome;        // 0x20
    Space* parentSpace; // 0x24
    float aabb[6];      // 0x28
    int categoryBits;   // 0x40
    int collideBits;    // 0x44

    Geom(Space* space, int placeable);
    virtual ~Geom();                                  // 0x08
    virtual void computeAABB() = 0;                   // 0x0c
    virtual int AABBTest(Geom* other, float* aabb);   // 0x10

    void* operator new(u32 size) { return dAlloc(size); }
    void operator delete(void* p, u32 size) { dFree(p, size); }

    // Unlinks the geometry from its body's list (bodyRemove).
    void bodyRemove()
    {
        if (body != 0) {
            Geom** link = &body->firstGeom;
            Geom* p = *link;
            for (; p != 0; p = p->bodyNext) {
                if (p == this) {
                    *link = p->bodyNext;
                    break;
                }
                link = &p->bodyNext;
            }
            body = 0;
            bodyNext = 0;
        }
    }
};

// A geometry of a run-time registered class (dxUserGeom): the base plus the class's user data.
struct UserGeom : Geom {
    void* userData;     // 0x4c

    UserGeom(int type);
    ~UserGeom();
    void computeAABB();
    int AABBTest(Geom* other, float* aabb);
};

// The state passed down through a space traversal (SpaceGeomColliderData).
struct CollideData {
    int count;             // flags in the low half, remaining contact slots above
    ContactGeom* contact;  // 0x04
    int skip;              // 0x08
};

enum { GEOM_CLASSES = 15 };

extern "C" {
void* memcpy(void* dst, const void* src, unsigned long n);
void* memset(void* dst, int c, unsigned long n);
void fn_8026D3A0(Geom* g);                                 // dGeomMoved (collision_space.cpp)
void fn_802788E4(Body* body, const float* r);              // dBodySetRotation (ode.cpp)
void fn_8027896C(Body* body, float x, float y, float z);   // dBodySetPosition (ode.cpp)
void fn_8027BD20(float* m);                                // dRSetIdentity (rotation.cpp)
int fn_8026F9F8(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026F590(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026F4D0(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026F21C(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026EAAC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026E9C8(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026E8EC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026E498(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026E194(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026DEAC(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026DA50(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026D538(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_8026D428(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int fn_80272060(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
}

// collision_space.cpp.
void SpaceCollidePair(Geom* g1, Geom* g2, void* data, SpaceCallback callback);
void SpaceRemoveGeom(Space* s, Geom* g);
void SpaceAddGeom(Space* s, Geom* g, int arg);

// Collision kernel state.
ColliderEntry gColliders[GEOM_CLASSES][GEOM_CLASSES];  // 0x804A2A70
UserGeomClass gUserGeomClasses[4];                     // 0x804A3178

// Other files' statics (sbss).
extern int lbl_804D5518;
extern int gUserGeomClassCount;

int GeomCollideDispatch(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
void CollisionInit(void);

static void GeomSetCollider(int a, int b, ColliderFn fn)
{
    if (gColliders[a][b].fn == 0) {
        gColliders[a][b].fn = fn;
        gColliders[a][b].swap = 0;
    }
    if (gColliders[b][a].fn == 0) {
        gColliders[b][a].fn = fn;
        gColliders[b][a].swap = 1;
    }
}

static void GeomSetAllColliders(int type, ColliderFn fn)
{
    int i;

    for (i = 0; i < GEOM_CLASSES; i++) {
        GeomSetCollider(type, i, fn);
    }
}

// 0x8026BB0C (space_geom_collider)
void SpaceCollideCallback(void* arg, Geom* o1, Geom* o2)
{
    CollideData* data = (CollideData*)arg;

    if ((data->count & 0xffff) != 0) {
        int result = GeomCollideDispatch(o1, o2, data->count, data->contact, data->skip);

        data->contact = (ContactGeom*)((char*)data->contact + data->skip * result);
        data->count -= result;
    }
}

// 0x8026BAB4 (dCollideSpaceGeom)
int DefaultCollider(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    CollideData data;

    data.count = flags;
    data.contact = contact;
    data.skip = skip;
    SpaceCollidePair(o1, o2, &data, SpaceCollideCallback);
    return (flags & 0xffff) - (data.count & 0xffff);
}

// 0x8026B40C (initColliders)
void CollisionInit(void)
{
    int i;
    int j;

    if (lbl_804D5518 != 0) {
        return;
    }
    lbl_804D5518 = 1;
    memset(gColliders, 0, sizeof(gColliders));
    for (i = 8; i <= 10; i++) {
        for (j = 0; j < GEOM_CLASSES; j++) {
            GeomSetCollider(i, j, DefaultCollider);
        }
    }
    GeomSetCollider(0, 0, fn_8026F9F8);
    GeomSetCollider(0, 1, fn_8026F590);
    GeomSetCollider(0, 4, fn_8026F4D0);
    GeomSetCollider(1, 1, fn_8026F21C);
    GeomSetCollider(1, 4, fn_8026EAAC);
    GeomSetCollider(2, 0, fn_8026E9C8);
    GeomSetCollider(2, 1, fn_8026E8EC);
    GeomSetCollider(2, 2, fn_8026E498);
    GeomSetCollider(2, 4, fn_8026E194);
    GeomSetCollider(5, 0, fn_8026DEAC);
    GeomSetCollider(5, 1, fn_8026DA50);
    GeomSetCollider(5, 2, fn_8026D538);
    GeomSetCollider(5, 4, fn_8026D428);
    GeomSetAllColliders(6, fn_80272060);
}

// 0x8026B09C (dCollide)
int GeomCollideDispatch(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    if (o1 == o2) {
        return 0;
    }
    if (o1->body == o2->body && o1->body != 0) {
        return 0;
    }

    ColliderEntry* entry = &gColliders[o1->type][o2->type];
    int rc = 0;

    if (entry->fn != 0) {
        if (entry->swap != 0) {
            rc = entry->fn(o2, o1, flags, contact, skip);
            for (int i = 0; i < rc; i++) {
                ContactGeom* c = (ContactGeom*)((char*)contact + skip * i);

                c->normal[0] = -c->normal[0];
                c->normal[1] = -c->normal[1];
                c->normal[2] = -c->normal[2];
                Geom* tmp = c->g1;
                c->g1 = c->g2;
                c->g2 = tmp;
            }
        } else {
            rc = entry->fn(o1, o2, flags, contact, skip);
        }
    }
    return rc;
}

// 0x8026AF98
Geom::Geom(Space* space, int placeable)
{
    CollisionInit();
    type = -1;
    flags = GEOM_DIRTY | GEOM_AABB_BAD | GEOM_ENABLED;
    if (placeable != 0) {
        flags |= GEOM_PLACEABLE;
    }
    data = 0;
    body = 0;
    bodyNext = 0;
    if (placeable != 0) {
        float* p = (float*)dAlloc(0x40);
        pos = p;
        rot = p + 4;
        dSetZero(pos, 4);
        fn_8027BD20(rot);
    } else {
        pos = 0;
        rot = 0;
    }
    next = 0;
    tome = 0;
    parentSpace = 0;
    dSetZero(aabb, 6);
    categoryBits = -1;
    collideBits = -1;
    if (space != 0) {
        SpaceAddGeom(space, this, 1);
    }
}

// 0x8026AEBC
Geom::~Geom()
{
    if (parentSpace != 0) {
        SpaceRemoveGeom(parentSpace, this);
    }
    if ((flags & GEOM_PLACEABLE) != 0 && body == 0) {
        dFree(pos, 0x40);
    }
    bodyRemove();
}

// 0x8026AEB4
int Geom::AABBTest(Geom* other, float* aabb)
{
    return 1;
}

// 0x8026AEAC
Geom* GeomGetNext(Geom* g)
{
    return g->bodyNext;
}

// 0x8026AE74 (dGeomDestroy)
void GeomDestroyGeom(Geom* g)
{
    delete g;
}

// 0x8026AE6C (dGeomSetData)
void GeomSetGFlags(Geom* g, int flags)
{
    g->data = (void*)flags;
}

// 0x8026AE64 (dGeomGetData)
int GeomGetGFlags(Geom* g)
{
    return (int)g->data;
}

// 0x8026AD08
void GeomSetBody(Geom* g, Body* body)
{
    if (body != 0) {
        if (g->body == 0) {
            dFree(g->pos, 0x40);
        }
        g->pos = body->pos;
        g->rot = body->rot;
        fn_8026D3A0(g);
        if (g->body != body) {
            g->bodyRemove();
            g->body = body;
            g->bodyNext = body->firstGeom;
            body->firstGeom = g;
        }
    } else {
        if (g->body != 0) {
            float* p = (float*)dAlloc(0x40);

            g->pos = p;
            g->rot = p + 4;
            memcpy(g->pos, g->body->pos, 0x10);
            memcpy(g->rot, g->body->rot, 0x30);
            g->bodyRemove();
        }
    }
}

// 0x8026AD00
Body* GeomGetBody(Geom* g)
{
    return g->body;
}

// 0x8026ACB0
void GeomSetPosition(Geom* g, float x, float y, float z)
{
    if (g->body != 0) {
        fn_8027896C(g->body, x, y, z);
    } else {
        g->pos[0] = x;
        g->pos[1] = y;
        g->pos[2] = z;
        fn_8026D3A0(g);
    }
}

// 0x8026AC60
void GeomSetRotation(Geom* g, const float* r)
{
    if (g->body != 0) {
        fn_802788E4(g->body, r);
    } else {
        memcpy(g->rot, r, 0x30);
        fn_8026D3A0(g);
    }
}

// 0x8026AC58
float* GeomGetPosition(Geom* g)
{
    return g->pos;
}

// 0x8026AC50
float* GeomGetRotation(Geom* g)
{
    return g->rot;
}

// 0x8026ABE4
void GeomGetAabb(Geom* g, float* aabb)
{
    if ((g->flags & GEOM_AABB_BAD) != 0) {
        g->computeAABB();
        g->flags &= ~GEOM_AABB_BAD;
    }
    memcpy(aabb, g->aabb, 0x18);
}

// 0x8026ABDC
int GeomGetType(Geom* g)
{
    return g->type;
}

// 0x8026ABD4
void GeomSetCategoryBits(Geom* g, int bits)
{
    g->categoryBits = bits;
}

// 0x8026ABCC
void GeomSetCollideBits(Geom* g, int bits)
{
    g->collideBits = bits;
}

// 0x8026ABC4
int GeomGetCategoryBits(Geom* g)
{
    return g->categoryBits;
}

// 0x8026ABBC
int GeomGetCollideBits(Geom* g)
{
    return g->collideBits;
}

// 0x8026ABAC
void GeomEnable(Geom* g)
{
    g->flags |= GEOM_ENABLED;
}

// 0x8026AB9C
void GeomDisable(Geom* g)
{
    g->flags &= ~GEOM_ENABLED;
}

// 0x8026AB90
int GeomIsEnabled(Geom* g)
{
    return (g->flags >> 3) & 1;
}

// 0x8026AB08
UserGeom::UserGeom(int t) : Geom(0, 1)
{
    type = t;
    u32 size = gUserGeomClasses[type - 0xb].size;
    userData = dAlloc(size);
    memset(userData, 0, size);
}

// 0x8026A9D8
UserGeom::~UserGeom()
{
    UserGeomClass* cls = &gUserGeomClasses[type - 0xb];

    if (cls->release != 0) {
        cls->release(this);
    }
    dFree(userData, cls->size);
}

// 0x8026A998
void UserGeom::computeAABB()
{
    gUserGeomClasses[type - 0xb].computeAabb(this, aabb);
}

// 0x8026A948
int UserGeom::AABBTest(Geom* other, float* aabb)
{
    UserGeomClass* cls = &gUserGeomClasses[type - 0xb];

    if (cls->test != 0) {
        return cls->test(this, other, aabb);
    }
    return 1;
}

// 0x8026A53C (dCollideUserGeomWithGeom)
int UserGeomCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    int g1 = o1->type;
    int g2 = o2->type;
    ColliderFn fn = gUserGeomClasses[g1 - 0xb].getCollider(g2);
    int swap = 0;

    if (fn == 0 && g2 >= 0xb && g2 <= 0xe) {
        fn = gUserGeomClasses[g2 - 0xb].getCollider(g1);
        swap = 1;
    }
    gColliders[g1][g2].fn = fn;
    gColliders[g1][g2].swap = swap;
    gColliders[g2][g1].fn = fn;
    gColliders[g2][g1].swap = !swap;
    return GeomCollideDispatch(o1, o2, flags, contact, skip);
}

// 0x8026A37C (dCreateGeomClass)
int GeomRegisterClass(const UserGeomClass* cls)
{
    if (gUserGeomClassCount >= 4) {
        dDebug(0, "too many user classes, you must increase the limit and recompile ODE");
    }
    gUserGeomClasses[gUserGeomClassCount] = *cls;
    int type = gUserGeomClassCount + 0xb;
    CollisionInit();
    GeomSetAllColliders(type, UserGeomCollide);
    gUserGeomClassCount++;
    return type;
}

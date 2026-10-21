// u_8026A37C.cpp: the ODE-style collision kernel (geometry classes, the class-pair
// collider matrix, and collision spaces). This file is the part of the game's physics
// library that dispatches collision queries between geometry types.
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

// One entry of the class-pair collider matrix.
struct ColliderEntry {
    ColliderFn fn;    // 0x00
    int swap;         // 0x04
};

// A class registered at run time through GeomRegisterClass.
struct UserGeomClass {
    u32 size;                                   // 0x00  per-geometry user data size
    ColliderFn (*getCollider)(int type);        // 0x04  pick a collider for the other type
    void (*computeAabb)(Geom* g, float* aabb);  // 0x08
    int (*test)(Geom* g);                       // 0x0c
    void (*release)(Geom* g);                   // 0x10
};

// The per-object class table (only its first five entries are visible here; spaces
// have more entries, declared in SpaceVtbl below).
struct GeomVtbl {
    int unk00;                             // 0x00
    int unk04;                             // 0x04
    Geom* (*destroy)(Geom* g, short del);  // 0x08
    void (*computeAabb)(Geom* g);          // 0x0c
    int (*isDefault)(Geom* g);             // 0x10
    int (*getNumGeoms)(Geom* g);                // 0x14
    void (*addGeom)(Geom* g, Geom* child, int arg);   // 0x18
    void (*removeGeom)(Geom* g, Geom* child);   // 0x1c
    void (*unk20)(Geom* g);                     // 0x20
    void (*unk24)(Geom* g);                     // 0x24
    void (*collideSpace)(Geom* g, void* data, SpaceCallback cb);          // 0x28
    void (*collideGeom)(Geom* g, void* data, Geom* other, SpaceCallback cb); // 0x2c
};

// A collision space's class table has the same layout as a geometry's.
typedef GeomVtbl SpaceVtbl;

// A rigid body (only the fields used here).
struct Body {
    char pad00[0x1c];   // 0x00
    Geom* firstGeom;    // 0x1c
    char pad20[0x78];   // 0x20
    float pos[4];       // 0x98
    char padA8[0x10];   // 0xa8
    float rot[12];      // 0xb8
};

// The geometry base class (dxGeom).
struct Geom {
    int type;          // 0x00
    u32 flags;         // 0x04
    int unk08;         // 0x08
    Body* body;        // 0x0c
    Geom* next;        // 0x10  next geometry of the same body
    float* pos;        // 0x14
    float* rot;        // 0x18
    int nextInSpace;   // 0x1c  next geometry of the same space
    int unk20;         // 0x20
    Space* space;      // 0x24
    float aabb[6];     // 0x28
    int categoryBits;  // 0x40
    int collideBits;   // 0x44
    GeomVtbl* vtbl;    // 0x48
};

// A geometry of a run-time registered class: the base plus the class's user data.
struct UserGeom : Geom {
    void* data;        // 0x4c
};

// A collision space: a geometry that holds other geometries.
struct Space : Geom {
    int count;         // 0x4c
    Geom* firstGeom;   // 0x50
    int dirty;         // 0x54
    char pad58[0xc];   // 0x58
    int unk64;         // 0x64
    int unk68;         // 0x68
};

// The state passed down through a space traversal.
struct CollideData {
    int count;             // flags in the low half, remaining contact slots above
    ContactGeom* contact;  // 0x04
    int skip;              // 0x08
};

enum { GEOM_CLASSES = 15 };

extern "C" {
void* memcpy(void* dst, const void* src, unsigned long n);
void* memset(void* dst, int c, unsigned long n);
void fn_802734DC(int level, const char* msg, ...);
void fn_8026D3A0(Geom* g);
void fn_802788E4(Body* body);
void fn_8027896C(Body* body);
void fn_8027BD20(float* m);
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

// From the solver (shared with the rest of the physics library).
void* SolverAllocBlock(u32 size);
void SolverFreeBlock(void* block, u32 size);
void SolverClearFloats(float* p, int n);

// Class tables handed to spaces; they live in the neighbouring unit's .data.
extern GeomVtbl lbl_803C0434;
extern GeomVtbl lbl_803C0464;

// The class tables of this file.
extern GeomVtbl gUserGeomVtbl;
extern GeomVtbl gGeomVtbl;

// Collision kernel state.
ColliderEntry gColliders[GEOM_CLASSES][GEOM_CLASSES];  // 0x804A2A70
UserGeomClass gUserGeomClasses[4];                     // 0x804A3178

// Other files' statics (sbss).
extern int lbl_804D5518;
extern int lbl_804D551C;

static const char sTooManyUserClasses[] =
    "too many user classes, you must increase the limit and recompile ODE";

// Forward declarations.
int GeomRegisterClass(UserGeomClass* cls);
int UserGeomCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
int GeomUserTest(Geom* g);
void GeomUpdateUserAabb(Geom* g);
Geom* GeomUserDestroy(Geom* g, short del);
Geom* UserGeomCreate(UserGeom* g, int type);
Geom* GeomDestroyBase(Geom* g, short del);
Geom* GeomInit(Geom* g, Space* space, int isPlaceable);
int GeomCollideDispatch(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
void CollisionInit(void);
int DefaultCollider(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip);
void SpaceCollideCallback(void* data, Geom* o1, Geom* o2);
Geom* SpaceDestroy(Space* s, short del);
void SpaceCollidePair(Geom* g1, Geom* g2, void* data, SpaceCallback callback);
void SpaceCollideSelf(Geom* s, void* data, SpaceCallback callback);
int SpaceGetNumGeoms(Geom* s);
int SpaceGetCount(Space* s);
void SpaceRemoveGeom(Space* s, Geom* g);
void SpaceAddGeom(Space* s, Geom* g, int arg);
void GeomDelete(Geom* g);
void SpaceSetFields(Space* s, int a, int b);

// 0x8026A37C
int GeomRegisterClass(UserGeomClass* cls)
{
    int type;
    int i;

    if (lbl_804D551C >= 4) {
        fn_802734DC(0, sTooManyUserClasses);
    }
    type = lbl_804D551C + 0xb;
    gUserGeomClasses[lbl_804D551C] = *cls;
    CollisionInit();
    for (i = 0; i < GEOM_CLASSES; i++) {
        if (gColliders[type][i].fn == 0) {
            gColliders[type][i].fn = UserGeomCollide;
            gColliders[type][i].swap = 0;
        }
        if (gColliders[i][type].fn == 0) {
            gColliders[i][type].fn = UserGeomCollide;
            gColliders[i][type].swap = 1;
        }
    }
    lbl_804D551C += 1;
    return type;
}

// 0x8026A53C
int UserGeomCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    int g1 = o1->type;
    int g2 = o2->type;
    ColliderFn fn;
    int swap = 0;
    int result;
    int i;

    fn = gUserGeomClasses[g1 - 0xb].getCollider(g2);
    if (fn == 0 && g2 > 10 && g2 < 0xf) {
        fn = gUserGeomClasses[g2 - 0xb].getCollider(g1);
        swap = 1;
    }
    gColliders[g1][g2].fn = fn;
    gColliders[g1][g2].swap = swap;
    gColliders[g2][g1].fn = fn;
    gColliders[g2][g1].swap = !swap;
    if (o1 == o2) {
        result = 0;
    } else if (o1->body == o2->body && o1->body != 0) {
        result = 0;
    } else {
        result = 0;
        fn = gColliders[g1][g2].fn;
        if (fn != 0) {
            if (gColliders[g1][g2].swap == 0) {
                result = fn(o1, o2, flags, contact, skip);
            } else {
                result = fn(o2, o1, flags, contact, skip);
                for (i = 0; i < result; i++) {
                    ContactGeom* c = (ContactGeom*)((char*)contact + i * skip);
                    Geom* tmp;

                    c->normal[0] = -c->normal[0];
                    c->normal[1] = -c->normal[1];
                    c->normal[2] = -c->normal[2];
                    tmp = c->g1;
                    c->g1 = c->g2;
                    c->g2 = tmp;
                }
            }
        }
    }
    return result;
}

// 0x8026A948
int GeomUserTest(Geom* g)
{
    UserGeomClass* cls = &gUserGeomClasses[g->type - 0xb];
    int (*test)(Geom*) = cls->test;

    if (test != 0) {
        return test(g);
    }
    return 1;
}

// 0x8026A998
void GeomUpdateUserAabb(Geom* g)
{
    gUserGeomClasses[g->type - 0xb].computeAabb(g, g->aabb);
}

// 0x8026A9D8
Geom* GeomUserDestroy(Geom* g, short del)
{
    if (g != 0) {
        UserGeomClass* cls;
        Body* body;
        Geom* p;
        Geom** link;

        g->vtbl = &gUserGeomVtbl;
        cls = &gUserGeomClasses[g->type - 0xb];
        if (cls->release != 0) {
            cls->release(g);
        }
        SolverFreeBlock(((UserGeom*)g)->data, cls->size);
        if (g != 0) {
            g->vtbl = &gGeomVtbl;
            if (g->space != 0) {
                SpaceRemoveGeom(g->space, g);
            }
            if ((g->flags & 4) != 0 && g->body == 0) {
                SolverFreeBlock(g->pos, 0x40);
            }
            body = g->body;
            if (body != 0) {
                link = &body->firstGeom;
                p = *link;
                for (; p != 0; p = p->next) {
                    if (p == g) {
                        *link = p->next;
                        break;
                    }
                    link = &p->next;
                }
                g->body = 0;
                g->next = 0;
            }
        }
        if (del > 0) {
            SolverFreeBlock(g, 0x50);
        }
    }
    return g;
}

// 0x8026AB08
Geom* UserGeomCreate(UserGeom* g, int type)
{
    u32 size;

    GeomInit(g, 0, 1);
    g->vtbl = &gUserGeomVtbl;
    g->type = type;
    size = gUserGeomClasses[g->type - 0xb].size;
    g->data = SolverAllocBlock(size);
    memset(g->data, 0, size);
    return g;
}

// 0x8026AB90
int GeomIsEnabled(Geom* g)
{
    return (g->flags >> 3) & 1;
}

// 0x8026AB9C
void GeomDisable(Geom* g)
{
    g->flags &= ~8;
}

// 0x8026ABAC
void GeomEnable(Geom* g)
{
    g->flags |= 8;
}

// 0x8026ABBC
int GeomGetCollideBits(Geom* g)
{
    return g->collideBits;
}

// 0x8026ABC4
int GeomGetCategoryBits(Geom* g)
{
    return g->categoryBits;
}

// 0x8026ABCC
void GeomSetCollideBits(Geom* g, int bits)
{
    g->collideBits = bits;
}

// 0x8026ABD4
void GeomSetCategoryBits(Geom* g, int bits)
{
    g->categoryBits = bits;
}

// 0x8026ABDC
int GeomGetType(Geom* g)
{
    return g->type;
}

// 0x8026ABE4
void GeomGetAabb(Geom* g, float* aabb)
{
    if ((g->flags & 2) != 0) {
        g->vtbl->computeAabb(g);
        g->flags &= ~2;
    }
    memcpy(aabb, g->aabb, 0x18);
}

// 0x8026AC50
float* GeomGetRotation(Geom* g)
{
    return g->rot;
}

// 0x8026AC58
float* GeomGetPosition(Geom* g)
{
    return g->pos;
}

// 0x8026AC60
void GeomSetRotation(Geom* g, const float* r)
{
    Body* body = g->body;

    if (body != 0) {
        fn_802788E4(body);
    } else {
        memcpy(g->rot, r, 0x30);
        fn_8026D3A0(g);
    }
}

// 0x8026ACB0
void GeomSetPosition(Geom* g, float x, float y, float z)
{
    Body* body = g->body;

    if (body != 0) {
        fn_8027896C(body);
    } else {
        g->pos[0] = x;
        g->pos[1] = y;
        g->pos[2] = z;
        fn_8026D3A0(g);
    }
}

// 0x8026AD00
Body* GeomGetBody(Geom* g)
{
    return g->body;
}

// 0x8026AD08
void GeomSetBody(Geom* g, Body* body)
{
    if (body != 0) {
        Body* old;
        Geom* q;
        Geom** link;

        if (g->body == 0) {
            SolverFreeBlock(g->pos, 0x40);
        }
        g->pos = body->pos;
        g->rot = body->rot;
        fn_8026D3A0(g);
        old = g->body;
        if (old != body) {
            if (old != 0) {
                link = &old->firstGeom;
                q = *link;
                for (; q != 0; q = q->next) {
                    if (q == g) {
                        *link = q->next;
                        break;
                    }
                    link = &q->next;
                }
                g->body = 0;
                g->next = 0;
            }
            g->body = body;
            g->next = body->firstGeom;
            body->firstGeom = g;
        }
    } else {
        if (g->body != 0) {
            float* p = (float*)SolverAllocBlock(0x40);
            Body* old;
            Geom* q;
            Geom** link;

            g->pos = p;
            g->rot = p + 4;
            memcpy(g->pos, g->body->pos, 0x10);
            memcpy(g->rot, g->body->rot, 0x30);
            old = g->body;
            if (old != 0) {
                link = &old->firstGeom;
                q = *link;
                for (; q != 0; q = q->next) {
                    if (q == g) {
                        *link = q->next;
                        break;
                    }
                    link = &q->next;
                }
                g->body = 0;
                g->next = 0;
            }
        }
    }
}

// 0x8026AE64
int GeomGetGFlags(Geom* g)
{
    return g->unk08;
}

// 0x8026AE6C
void GeomSetGFlags(Geom* g, int flags)
{
    g->unk08 = flags;
}

// 0x8026AE74
void GeomDestroyGeom(Geom* g)
{
    if (g != 0) {
        Geom* (*fn)(Geom*, short) = g->vtbl->destroy;
        fn(g, 1);
    }
}

// 0x8026AEAC
Geom* GeomGetNext(Geom* g)
{
    return g->next;
}
// 0x8026AEB4
int GeomDefaultTest(Geom* g)
{
    return 1;
}

// 0x8026AEBC
Geom* GeomDestroyBase(Geom* g, short del)
{
    if (g != 0) {
        Body* body;
        Geom* p;
        Geom** link;

        g->vtbl = &gGeomVtbl;
        if (g->space != 0) {
            SpaceRemoveGeom(g->space, g);
        }
        if ((g->flags & 4) != 0 && g->body == 0) {
            SolverFreeBlock(g->pos, 0x40);
        }
        body = g->body;
        if (body != 0) {
            link = &body->firstGeom;
            p = *link;
            for (; p != 0; p = p->next) {
                if (p == g) {
                    *link = p->next;
                    break;
                }
                link = &p->next;
            }
            g->body = 0;
            g->next = 0;
        }
        if (del > 0) {
            SolverFreeBlock(g, 0x4c);
        }
    }
    return g;
}

// 0x8026AF98
Geom* GeomInit(Geom* g, Space* space, int isPlaceable)
{
    g->vtbl = &gGeomVtbl;
    CollisionInit();
    g->type = -1;
    g->flags = 0xb;
    if (isPlaceable != 0) {
        g->flags |= 4;
    }
    g->unk08 = 0;
    g->body = 0;
    g->next = 0;
    if (isPlaceable != 0) {
        float* p = (float*)SolverAllocBlock(0x40);
        g->pos = p;
        g->rot = p + 4;
        SolverClearFloats(g->pos, 4);
        fn_8027BD20(g->rot);
    } else {
        g->pos = 0;
        g->rot = 0;
    }
    g->nextInSpace = 0;
    g->unk20 = 0;
    g->space = 0;
    SolverClearFloats(g->aabb, 6);
    g->categoryBits = -1;
    g->collideBits = -1;
    if (space != 0) {
        SpaceAddGeom(space, g, 1);
    }
    return g;
}

// 0x8026B09C
int GeomCollideDispatch(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    ColliderFn fn;
    int result = 0;
    int i;
    int g1;
    int g2;

    if (o1 == o2) {
        return 0;
    }
    if (o1->body == o2->body && o1->body != 0) {
        return 0;
    }

    {
        ColliderEntry* entry;

        g1 = o1->type;
        g2 = o2->type;
        entry = &gColliders[g1][g2];

        fn = entry->fn;
        if (fn == 0) {
            return result;
        }
        if (entry->swap != 0) {
            result = fn(o2, o1, flags, contact, skip);
            for (i = 0; i < result; i++) {
                ContactGeom* c = (ContactGeom*)((char*)contact + i * skip);
                Geom* tmp;

                c->normal[0] = -c->normal[0];
                c->normal[1] = -c->normal[1];
                c->normal[2] = -c->normal[2];
                tmp = c->g1;
                c->g1 = c->g2;
                c->g2 = tmp;
            }
        } else {
            result = fn(o1, o2, flags, contact, skip);
        }
    }
    return result;
}

// 0x8026B40C
void CollisionInit(void)
{
    int i;
    int j;

    if (lbl_804D5518 == 0) {
        lbl_804D5518 = 1;
        memset(gColliders, 0, 0x708);
        for (i = 8; i < 0xb; i++) {
            for (j = 0; j < GEOM_CLASSES; j++) {
                if (gColliders[i][j].fn == 0) {
                    gColliders[i][j].fn = DefaultCollider;
                    gColliders[i][j].swap = 0;
                }
                if (gColliders[j][i].fn == 0) {
                    gColliders[j][i].fn = DefaultCollider;
                    gColliders[j][i].swap = 1;
                }
            }
        }
        if (gColliders[0][0].fn == 0) {
            gColliders[0][0].swap = 0;
            gColliders[0][0].fn = fn_8026F9F8;
        }
        if (gColliders[0][0].fn == 0) {
            gColliders[0][0].swap = 1;
            gColliders[0][0].fn = fn_8026F9F8;
        }
        if (gColliders[0][1].fn == 0) {
            gColliders[0][1].swap = 0;
            gColliders[0][1].fn = fn_8026F590;
        }
        if (gColliders[1][0].fn == 0) {
            gColliders[1][0].swap = 1;
            gColliders[1][0].fn = fn_8026F590;
        }
        if (gColliders[0][4].fn == 0) {
            gColliders[0][4].swap = 0;
            gColliders[0][4].fn = fn_8026F4D0;
        }
        if (gColliders[4][0].fn == 0) {
            gColliders[4][0].swap = 1;
            gColliders[4][0].fn = fn_8026F4D0;
        }
        if (gColliders[1][1].fn == 0) {
            gColliders[1][1].swap = 0;
            gColliders[1][1].fn = fn_8026F21C;
        }
        if (gColliders[1][1].fn == 0) {
            gColliders[1][1].swap = 1;
            gColliders[1][1].fn = fn_8026F21C;
        }
        if (gColliders[1][4].fn == 0) {
            gColliders[1][4].swap = 0;
            gColliders[1][4].fn = fn_8026EAAC;
        }
        if (gColliders[4][1].fn == 0) {
            gColliders[4][1].swap = 1;
            gColliders[4][1].fn = fn_8026EAAC;
        }
        if (gColliders[2][0].fn == 0) {
            gColliders[2][0].swap = 0;
            gColliders[2][0].fn = fn_8026E9C8;
        }
        if (gColliders[0][2].fn == 0) {
            gColliders[0][2].swap = 1;
            gColliders[0][2].fn = fn_8026E9C8;
        }
        if (gColliders[2][1].fn == 0) {
            gColliders[2][1].swap = 0;
            gColliders[2][1].fn = fn_8026E8EC;
        }
        if (gColliders[1][2].fn == 0) {
            gColliders[1][2].swap = 1;
            gColliders[1][2].fn = fn_8026E8EC;
        }
        if (gColliders[2][2].fn == 0) {
            gColliders[2][2].swap = 0;
            gColliders[2][2].fn = fn_8026E498;
        }
        if (gColliders[2][2].fn == 0) {
            gColliders[2][2].swap = 1;
            gColliders[2][2].fn = fn_8026E498;
        }
        if (gColliders[2][4].fn == 0) {
            gColliders[2][4].swap = 0;
            gColliders[2][4].fn = fn_8026E194;
        }
        if (gColliders[4][2].fn == 0) {
            gColliders[4][2].swap = 1;
            gColliders[4][2].fn = fn_8026E194;
        }
        if (gColliders[5][0].fn == 0) {
            gColliders[5][0].swap = 0;
            gColliders[5][0].fn = fn_8026DEAC;
        }
        if (gColliders[0][5].fn == 0) {
            gColliders[0][5].swap = 1;
            gColliders[0][5].fn = fn_8026DEAC;
        }
        if (gColliders[5][1].fn == 0) {
            gColliders[5][1].swap = 0;
            gColliders[5][1].fn = fn_8026DA50;
        }
        if (gColliders[1][5].fn == 0) {
            gColliders[1][5].swap = 1;
            gColliders[1][5].fn = fn_8026DA50;
        }
        if (gColliders[5][2].fn == 0) {
            gColliders[5][2].swap = 0;
            gColliders[5][2].fn = fn_8026D538;
        }
        if (gColliders[2][5].fn == 0) {
            gColliders[2][5].swap = 1;
            gColliders[2][5].fn = fn_8026D538;
        }
        if (gColliders[5][4].fn == 0) {
            gColliders[5][4].swap = 0;
            gColliders[5][4].fn = fn_8026D428;
        }
        if (gColliders[4][5].fn == 0) {
            gColliders[4][5].swap = 1;
            gColliders[4][5].fn = fn_8026D428;
        }
        for (j = 0; j < GEOM_CLASSES; j++) {
            if (gColliders[6][j].fn == 0) {
                gColliders[6][j].fn = fn_80272060;
                gColliders[6][j].swap = 0;
            }
            if (gColliders[j][6].fn == 0) {
                gColliders[j][6].fn = fn_80272060;
                gColliders[j][6].swap = 1;
            }
        }
    }
}

// 0x8026BAB4
int DefaultCollider(Geom* o1, Geom* o2, int flags, ContactGeom* contact, int skip)
{
    CollideData data;

    data.count = flags;
    data.contact = contact;
    data.skip = skip;
    SpaceCollidePair(o1, o2, &data, SpaceCollideCallback);
    return (flags & 0xffff) - (data.count & 0xffff);
}

// 0x8026BB0C
void SpaceCollideCallback(void* arg, Geom* o1, Geom* o2)
{
    CollideData* data = (CollideData*)arg;

    if ((data->count & 0xffff) != 0) {
        int flags = data->count;
        ContactGeom* contact = data->contact;
        int skip = data->skip;
        int g1 = o1->type;
        int g2 = o2->type;
        ColliderFn fn;
        int result;
        int i;

        if (o1 == o2) {
            result = 0;
        } else if (o1->body == o2->body && o1->body != 0) {
            result = 0;
        } else {
            result = 0;
            fn = gColliders[g1][g2].fn;
            if (fn != 0) {
                if (gColliders[g1][g2].swap == 0) {
                    result = fn(o1, o2, flags, contact, skip);
                } else {
                    result = fn(o2, o1, flags, contact, skip);
                    for (i = 0; i < result; i++) {
                        ContactGeom* c = (ContactGeom*)((char*)contact + i * skip);
                        Geom* tmp;

                        c->normal[0] = -c->normal[0];
                        c->normal[1] = -c->normal[1];
                        c->normal[2] = -c->normal[2];
                        tmp = c->g1;
                        c->g1 = c->g2;
                        c->g2 = tmp;
                    }
                }
            }
        }
        data->contact = (ContactGeom*)((char*)data->contact + data->skip * result);
        data->count -= result;
    }
}

// 0x8026BE98
Geom* SpaceDestroy(Space* s, short del)
{
    if (s != 0) {
        s->vtbl = &lbl_803C0434;
        if (s != 0) {
            s->vtbl = &lbl_803C0464;
            if (s->dirty == 0) {
                Geom* g = s->firstGeom;
                while (g != 0) {
                    Geom* next = (Geom*)g->nextInSpace;
                    ((SpaceVtbl*)s->vtbl)->removeGeom(s, g);
                    g = next;
                }
            } else {
                Geom* g = s->firstGeom;
                while (g != 0) {
                    Geom* next = (Geom*)g->nextInSpace;
                    GeomDestroyGeom(g);
                    g = next;
                }
            }
            GeomDestroyBase(s, 0);
        }
        if (del > 0) {
            SolverFreeBlock(s, 0x6c);
        }
    }
    return s;
}

// 0x8026BF70
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
                s1->vtbl->collideSpace(s1, data, callback);
            } else if (s1->count < s2->count) {
                Geom* g = s1->firstGeom;
                while (g != 0) {
                    s2->vtbl->collideGeom(s2, data, g, callback);
                    g = (Geom*)g->nextInSpace;
                }
            } else {
                Geom* g = s2->firstGeom;
                while (g != 0) {
                    s1->vtbl->collideGeom(s1, data, g, callback);
                    g = (Geom*)g->nextInSpace;
                }
            }
        } else {
            s1->vtbl->collideGeom(s1, data, g2, callback);
        }
    } else {
        if (s2 != 0) {
            s2->vtbl->collideGeom(s2, data, g1, callback);
        } else {
            callback(data, g1, g2);
        }
    }
}

// 0x8026C11C
void SpaceCollideSelf(Geom* s, void* data, SpaceCallback callback)
{
    ((SpaceVtbl*)s->vtbl)->collideSpace(s, data, callback);
}

// 0x8026C148
int SpaceGetNumGeoms(Geom* s)
{
    return ((SpaceVtbl*)s->vtbl)->getNumGeoms(s);
}

// 0x8026C174
int SpaceGetCount(Space* s)
{
    return s->count;
}

// 0x8026C17C
void SpaceRemoveGeom(Space* s, Geom* g)
{
    ((SpaceVtbl*)s->vtbl)->removeGeom(s, g);
}

// 0x8026C1A8
void SpaceAddGeom(Space* s, Geom* g, int arg)
{
    ((SpaceVtbl*)s->vtbl)->addGeom(s, g, arg);
}

// 0x8026C1D4
void GeomDelete(Geom* g)
{
    GeomDestroyGeom(g);
}

// 0x8026C1F4
void SpaceSetFields(Space* s, int a, int b)
{
    s->unk64 = a;
    s->unk68 = b;
}

// The two class tables of this file (0x803C0390, 0x803C03A4).
GeomVtbl gUserGeomVtbl = {0, 0, GeomUserDestroy, GeomUpdateUserAabb, GeomUserTest};
GeomVtbl gGeomVtbl = {0, 0, GeomDestroyBase, 0, GeomDefaultTest};

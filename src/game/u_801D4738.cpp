// u_801D4738.cpp: 0x801D4738-0x801D8134, 61 functions. The game's ODE physics glue: physics objects (a body
// and a geom with a material), joints, queued forces, the world step with its near callback and auto-disable,
// impact reports, debug drawing, and the setup/shutdown of the world, the hash space and the geom list.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   .sdata2 0x804D8C20-0x804D8C74 (0.0 at 0x804D8C38 read from 0x801D4DC4 to 0x801D7C24: one file)
//   material table 0x803ADB90 ("RED OAK", "STEEL", "RUBBER"), jump table 0x803ADBB4 (fn_801D6928),
//   vtable 0x803ADBD4 (fn_801D7FDC, fn_801D80EC, fn_801D8044, fn_801D808C; ScreenEffectSlot base),
//   rodata tables 0x80348AC0/0x80348BC0; .sbss 0x804D51A0-0x804D51C8 (+4 pad), .sdata 0x804D3228-0x804D3230
//   .bss 0x8049A9D8
// Every function keeps its fn_ placeholder name with C linkage: about 40 other units call them by that name.
// Names in comments are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"

typedef unsigned long u32;
typedef unsigned char u8;

struct Geom;
struct Space;
struct Body;
struct UserGeom;
struct UserGeomClass;
struct SceneNode;
struct dxWorld;
struct ContactGeom;

// A physics object (VAlloc'd, 0x48 bytes): an ODE body and geom plus the game's bookkeeping.
struct PhysObj {
    Vec3 offset;                // 0x00: centre of mass in object space
    SceneNode* body;            // 0x0C
    Geom* geom;                 // 0x10
    int owner;                  // 0x14
    int material;               // 0x18
    float idle;                 // 0x1C: time spent at rest
    float damp;                 // 0x20: linear damping
    float angDamp;              // 0x24: angular damping
    u32 flags;                  // 0x28: 1 enabled, 2 touched this step, 4 sliding
    u32 hits;                   // 0x2C: contact history bits
    int surface;                // 0x30
    float strength;             // 0x34
    Geom* other;                // 0x38
    Vec3 normal;                // 0x3C
};

// One queued force (0x28 bytes, 64 at 0x8049A9D8), applied every sub-step.
struct Force {
    int type;                   // 0x00: 0-7 (which body function), 8 = removed
    SceneNode* body;            // 0x04
    int unk08;                  // 0x08
    Geom* geom;                 // 0x0C
    Vec3 a;                     // 0x10
    Vec3 b;                     // 0x1C
};

// One entry of the material table: name, its hash (filled at init) and density.
struct Material {
    const char* name;
    u32 hash;
    float density;
};

// ODE dMass (only passed by address).
struct Mass {
    float data[17];
};

// The level-geometry geom's user data (see u_801D25CC).
struct RoomGeomData {
    float aabb[6];              // 0x00
    short room;                 // 0x18
    short unk1A;                // 0x1A
};

// User geom class registration block.
struct GeomClassDesc {
    int bytes;
    void* collider;
    void* aabb;
    void* aabbTest;
    void* dtor;
};

// The list of every geom the game created (a growable array of Geom*).
class GeomVec : public Buffer {
public:
    virtual ~GeomVec();
    virtual int ElemSize();
    virtual void Construct(void* p, u32 bytes);
    virtual void Destruct(void* p, u32 bytes);
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

// Engine functions with C++ names.
void* VAlloc(int size);                                         // 0x8009DA68
void VFree(void* p);                                            // 0x8009DA3C
void Mtx34StoreTranslation(Mtx34* m, Vec3* t);                  // 0x800A0280
void DebugDrawSphereWire(int r, int g, int b, float x, float y, float z, float radius); // 0x80023A9C
void DebugDrawBoneBox(Vec3* box, void* m, int r, int g, int b); // 0x80023EE4
void RoomGeomDebugDraw();                                       // 0x801D3EC4
void AmmoRefillResetStatics();                                  // 0x8026A31C
Geom* UserGeomAlloc(int cls);                                   // 0x8026A32C
RoomGeomData* UserGeomGetId(UserGeom* g);                       // 0x8026A374
int GeomRegisterClass(const UserGeomClass* cls);                // 0x8026A37C
SceneNode* GeomIsEnabled(Geom* g);                              // 0x8026AB90
void GeomDisable(Geom* g);                                      // 0x8026AB9C
void GeomEnable(Geom* g);                                       // 0x8026ABAC
u32 GeomGetCollideBits(Geom* g);                                // 0x8026ABBC
int GeomGetCategoryBits(Geom* g);                               // 0x8026ABC4
void GeomSetCollideBits(Geom* g, int bits);                     // 0x8026ABCC
void GeomSetCategoryBits(Geom* g, int bits);                    // 0x8026ABD4
int GeomGetType(Geom* g);                                       // 0x8026ABDC
void GeomGetAabb(Geom* g, float* aabb);                         // 0x8026ABE4
float* GeomGetRotation(Geom* g);                                // 0x8026AC50
float* GeomGetPosition(Geom* g);                                // 0x8026AC58
void GeomSetRotation(Geom* g, const float* r);                  // 0x8026AC60
void GeomSetPosition(Geom* g, float x, float y, float z);       // 0x8026ACB0
SceneNode* GeomGetBody(Geom* g);                                // 0x8026AD00
void GeomSetBody(Geom* g, Body* b);                             // 0x8026AD08
PhysObj* GeomGetGFlags(Geom* g);                                // 0x8026AE64
void GeomSetGFlags(Geom* g, int data);                          // 0x8026AE6C
void GeomDestroyGeom(Geom* g);                                  // 0x8026AE74
int GeomCollideDispatch(Geom* a, Geom* b, int flags, ContactGeom* c, int skip); // 0x8026B09C
void SpaceCollideSelf(Geom* space, void* data, void (*cb)(void*, Geom*, Geom*)); // 0x8026C11C
Geom* SpaceGetNumGeoms(Geom* space, int i);                     // 0x8026C148
int SpaceGetCount(Space* space);                                // 0x8026C174
void SpaceAddGeom(Space* space, Geom* g, int a);                // 0x8026C1A8
void GeomDelete(Geom* g);                                       // 0x8026C1D4
void SpaceSetFields(Space* space, int a, int b);                // 0x8026C1F4
Space* HashSpaceCreate(Space* parent);                          // 0x8026C200
void CapsuleGetParams(Geom* g, float* radius, float* length);   // 0x802718A0
Geom* CapsuleCreate(float radius, float length, Space* space);  // 0x802718B4
float BoxPointDepth(float x, float y, float z, Geom* g);        // 0x802719E8
void BoxGetLengths(Geom* g, float* out);                        // 0x80271AB4
Geom* BoxCreate(float x, float y, float z, Space* space);       // 0x80271AD0
float SphereGetRadius(Geom* g);                                 // 0x80271C90
Geom* SphereCreate(float radius, Space* space);                 // 0x80271C98
void SetErrorHandler(void (*fn)(int, const char*, void*));      // 0x80273640
void SetMessageHandler(void (*fn)(int, const char*, void*));    // 0x80273648
void dSetFreeHandler(void (*fn)(void*, u32));                   // 0x80277208
void dSetReallocHandler(void* (*fn)(void*, u32, u32));          // 0x80277210
void dSetAllocHandler(void* (*fn)(u32));                        // 0x80277218
void dWorldSetQuickStepNumIterations(dxWorld* w, int n);        // 0x802775AC
void dWorldSetAutoDisableFlag(dxWorld* w, int flag);            // 0x802775B4
void dWorldStep(dxWorld* w, float dt);                          // 0x802775C8
void dWorldSetGravity(dxWorld* w, float x, float y, float z);   // 0x802775F0
void SceneNodeSetPinned(SceneNode* b, int pinned);              // 0x8027835C
int SceneNodeNotLocked(SceneNode* b);                           // 0x80278384
void SceneNodeLock(SceneNode* b);                               // 0x80278398
void SceneNodeSnapPrev(SceneNode* b);                           // 0x802783A8
Geom* SceneNodeListGet(SceneNode* b, int i);                    // 0x802783C8
int SceneNodeListCount(SceneNode* b);                           // 0x802783FC
void SceneNodeAddRot(float x, float y, float z, SceneNode* b);  // 0x80278800
void SceneNodeAddAccel(float x, float y, float z, SceneNode* b); // 0x80278828
float* SceneNodeVelocityPtr(SceneNode* b);                      // 0x802788B4
float* SceneNodePosPtr(SceneNode* b);                           // 0x802788BC
void SceneNodeSetVelocity(float x, float y, float z, SceneNode* b); // 0x802788C4
void SceneNodeSetPos(float x, float y, float z, SceneNode* b);  // 0x802788D4
PhysObj* SceneNodeGetOwner(SceneNode* b);                       // 0x802789C0
void SceneNodeSetOwner(SceneNode* b, u32 owner);                // 0x802789C8

// Base Commit/Append (0x8028BC2C / 0x8028BC28) of the Buffer classes in u_8028A8D4.cpp, which the
// array overrides here call directly. Class and member names are guesses.
class ScreenEffectBuffer {
public:
    void Commit(int start, unsigned long size);
    void Append(unsigned char* data, unsigned long size);
};

extern "C" {
int sprintf(char* s, const char* fmt, ...);
int vsprintf(char* s, const char* fmt, void* ap);
u32 strlen(const char* s);
void fn_800090BC(Vec3* a, Vec3* b, int r, int g, int bl);       // 0x800090BC: debug line
void fn_800A0030(Vec3* out, const void* m, const Vec3* in);                 // 0x800A0030: transform
void fn_800A0168(Mtx34* out, void* in);                         // 0x800A0168
float fn_800A1394(float a);                                     // 0x800A1394: sin (degrees)
float fn_800A1418(float a);                                     // 0x800A1418: cos (degrees)
void fn_801D2634();
void fn_801D25D8();
void fn_801D25CC();
void fn_801D25D4();
void fn_801D8FD4(float strength, int a, int b, int kind, int surface, Vec3* normal, int material);
void fn_801E1354();
void fn_801E13A0(float dt);
void fn_801E1400();
void fn_801E1420();
void fn_801E1494();
float fn_80271714(float x, float y, float z, Geom* g);          // capsule point depth
void fn_80273650(void* j, int a);
void fn_80273688(float v, void* j, int param);
void fn_802737A4(float x, float y, float z, void* j, int row, int rel);
void fn_80273918(void* j, int n);
void fn_80274F40(float v, void* j, int param);
void fn_80274FF8(float x, float y, float z, void* j);
void fn_80275218(float x, float y, float z, void* j);
void fn_802759B8(float x, float y, float z, void* j);
void fn_8027649C(Mass* m);
void fn_80276090(float density, float x, float y, float z, Mass* m);
void fn_80276170(float density, float radius, float length, Mass* m, int dir);
void fn_802762AC(float density, float radius, Mass* m);
void fn_80277600(dxWorld* w);
dxWorld* fn_802776D4();
int fn_80277778(SceneNode* a, SceneNode* b, int type);
void* fn_802777BC(Geom* j, int i);
void fn_80277804(void* j, SceneNode* a, SceneNode* b);
void fn_80277958(void* group);
void fn_80277B38(void* group);
void* fn_80277D38(int n);
void* fn_80277D80(dxWorld* w, void* group);
void* fn_80277EC8(dxWorld* w, void* group, void* contact);
void* fn_802780CC(dxWorld* w, void* group);
void* fn_80278214(dxWorld* w, void* group);
void fn_8027841C(float a, float b, float c, float d, float e, float f, SceneNode* body);
void fn_80278534(float a, float b, float c, float d, float e, float f, SceneNode* body);
void fn_802785F8(float a, float b, float c, float d, float e, float f, SceneNode* body);
void fn_802786A4(float a, float b, float c, float d, float e, float f, SceneNode* body);
void fn_80278720(float x, float y, float z, SceneNode* body);
void fn_80278790(float x, float y, float z, SceneNode* body);
void fn_80278850(SceneNode* body, Mass* m);
SceneNode* fn_80278B70(dxWorld* w);
void fn_802789D0(SceneNode* body);
void* fn_8028B3BC(GeomVec* v, void* at, int size);
void fn_8028B35C(GeomVec* v, void* at, int size);
void fn_8028B960(GeomVec* v);
void fn_801D80F4(Geom** first, Geom** last);
void fn_801D8114(Geom** first, Geom** last);
u32 __cvt_fp2unsigned(float f);

int fn_801D4738(PhysObj* o, Vec3* p, float r);
void fn_801D7068(Geom* space, Geom* g);
PhysObj* fn_801D59A4(int owner, int material, Geom* g, Mass* m, int cat, Vec3* offset, int noBody);
void fn_801D6104(Geom* space);
void fn_801D64B8(float radius, float length, Vec3* pos, Mtx34* rot, u8* color);
void fn_801D6CB8(float dt, Geom* space);
int fn_801D6EC8(SceneNode* body, Geom* from, int n, SceneNode** list);
void fn_801D72F8(void* data, Geom* a, Geom* b);
void fn_801D77C4(PhysObj* o, float* vel);
void fn_801D7DDC(int num, const char* fmt, void* ap);
void fn_801D7E74(int num, const char* fmt, void* ap);
void fn_801D7F0C(void* p, u32 size);
void* fn_801D7F2C(void* p, u32 a, u32 b);
void* fn_801D7F34(u32 size);
}


// Material table at 0x803ADB90.
Material gMaterials[3] = {
    { "RED OAK", 0, 6.73f },
    { "STEEL", 0, 77.15f },
    { "RUBBER", 0, 15.06f },
};

Force gForces[64];                                              // 0x8049A9D8

GeomVec* sGeoms;                                                // 0x804D51A0
int sPaused;                                                    // 0x804D51A4
int sForceCount;                                                // 0x804D51A8
int sNextId;                                                    // 0x804D51AC
u32 sDebugFlags;                                                // 0x804D51B0
dxWorld* sWorld;                                                // 0x804D51B4
Space* sSpace;                                                  // 0x804D51B8
void* sContacts;                                                // 0x804D51BC
int sRoomClass;                                                 // 0x804D51C0

int sSpaceA = 0;                                                // 0x804D3228
int sSpaceB = 0;                                                // 0x804D322C
int sSteps = 1;                                                 // 0x804D3230

extern "C" {

// 0x801D4738: does the object's shape reach the point (with a margin)?
int fn_801D4738(PhysObj* o, Vec3* p, float r) {
    float depth;
    float margin;
    int type;

    if (o->body == 0) {
        return 0;
    }
    r *= 0.0023818598f;
    margin = r;
    type = GeomGetType(o->geom);
    if (type == 1) {
        depth = -BoxPointDepth(0.0023818598f * p->x, 0.0023818598f * p->y, 0.0023818598f * p->z, o->geom);
    } else if (type == 2) {
        depth = -fn_80271714(0.0023818598f * p->x, 0.0023818598f * p->y, 0.0023818598f * p->z, o->geom);
    } else {
        return 0;
    }
    if (depth < margin) {
        return 1;
    }
    return 0;
}

// 0x801D4824: universal-style joint between two objects with optional angle limits.
void fn_801D4824(void* group, PhysObj* a, PhysObj* b, const Vec3* axis1, const Vec3* axis2, int limits,
                 const float* angles) {
    void* j = fn_80277D80(sWorld, group);
    SceneNode* ba = a == 0 ? 0 : a->body;
    SceneNode* bb = b == 0 ? 0 : b->body;
    fn_80277804(j, ba, bb);
    fn_80273918(j, 2);
    {
        Vec3 c;
        c.x = axis2->y * axis1->z - axis2->z * axis1->y;
        c.y = axis2->z * axis1->x - axis2->x * axis1->z;
        c.z = axis2->x * axis1->y - axis2->y * axis1->x;
        fn_802737A4(axis1->x, axis1->y, axis1->z, j, 0, 1);
        fn_802737A4(c.x, c.y, c.z, j, 1, 1);
        fn_802737A4(axis2->x, axis2->y, axis2->z, j, 2, 2);
    }
    if (limits > 0) {
        fn_80273688(0.017453292f * angles[0], j, 0);
        fn_80273688(0.017453292f * angles[1], j, 1);
        fn_80273688(0.01f, j, 4);
    }
    if (limits > 1) {
        fn_80273688(0.017453292f * angles[4], j, 0x100);
        fn_80273688(0.017453292f * angles[5], j, 0x101);
        fn_80273688(0.01f, j, 4);
    }
    if (limits >= 2) {
        fn_80273688(0.017453292f * angles[8], j, 0x200);
        fn_80273688(0.017453292f * angles[9], j, 0x201);
        fn_80273688(0.01f, j, 4);
    }
    fn_80273650(j, 1);
}

// 0x801D4A70: hinge joint.
void fn_801D4A70(void* group, PhysObj* a, PhysObj* b, Vec3* anchor, Vec3* axis, float* limits) {
    void* j = fn_802780CC(sWorld, group);
    SceneNode* ba = a == 0 ? 0 : a->body;
    SceneNode* bb = b == 0 ? 0 : b->body;
    fn_80277804(j, ba, bb);
    float s = 0.0023818598f;
    fn_80275218(anchor->x * s, anchor->y * s, anchor->z * s, j);
    fn_80274FF8(axis->x, axis->y, axis->z, j);
    if (limits != 0) {
        fn_80274F40(0.017453292f * limits[0], j, 0);
        fn_80274F40(0.017453292f * limits[1], j, 1);
    }
}

// 0x801D4B5C: ball joint.
void fn_801D4B5C(void* group, PhysObj* a, PhysObj* b, Vec3* anchor) {
    void* j = fn_80278214(sWorld, group);
    SceneNode* ba = a == 0 ? 0 : a->body;
    SceneNode* bb = b == 0 ? 0 : b->body;
    fn_80277804(j, ba, bb);
    float s = 0.0023818598f;
    fn_802759B8(anchor->x * s, anchor->y * s, anchor->z * s, j);
}

// 0x801D4C0C
void fn_801D4C0C(void* group) {
    if (group != 0) {
        fn_80277B38(group);
    }
}

// 0x801D4C34
void* fn_801D4C34(void) {
    return fn_80277D38(0);
}

// 0x801D4C58
void fn_801D4C58(void) {
}

// 0x801D4C5C: adds a level-geometry box for a room.
void fn_801D4C5C(int* box, short room) {
    Geom* g = UserGeomAlloc(sRoomClass);
    *(Geom**)fn_8028B3BC(sGeoms, (char*)sGeoms->mUnk0C + sGeoms->mLength, 4) = g;
    RoomGeomData* d = UserGeomGetId((UserGeom*)g);
    d->aabb[0] = 0.0023818598f * (float)box[0];
    d->aabb[1] = 0.0023818598f * (float)box[3];
    d->aabb[2] = 0.0023818598f * (float)box[1];
    d->aabb[3] = 0.0023818598f * (float)box[4];
    d->aabb[4] = 0.0023818598f * (float)box[2];
    d->aabb[5] = 0.0023818598f * (float)box[5];
    d->room = room;
    d->unk1A = 0;
    SpaceAddGeom(sSpace, g, 0);
    GeomSetCategoryBits(g, 0x1FFFFFFF);
}

// Queues a force on an object's body (woken first).
static inline void ForceAdd(PhysObj* o, int type, const Vec3* a, const Vec3* b) {
    if (sForceCount != 64 && o != 0 && o->body != 0) {
        Force* f = &gForces[sForceCount++];
        f->type = type;
        if (o->body != 0 && SceneNodeNotLocked(o->body) == 0) {
            o->idle = 0.0f;
            SceneNodeSnapPrev(o->body);
        }
        f->body = o->body;
        f->geom = o->geom;
        f->unk08 = 0;
        if (a != 0) {
            f->a.x = a->x;
            f->a.y = a->y;
            f->a.z = a->z;
        } else {
            f->a.x = 0.0f;
            f->a.y = 0.0f;
            f->a.z = 0.0f;
        }
        if (b != 0) {
            f->b.x = b->x;
            f->b.y = b->y;
            f->b.z = b->z;
        } else {
            f->b.x = 0.0f;
            f->b.y = 0.0f;
            f->b.z = 0.0f;
        }
    }
}

// 0x801D4DC4
void fn_801D4DC4(PhysObj* o, const Vec3* v) {
    ForceAdd(o, 6, v, 0);
}

// 0x801D4EC4
void fn_801D4EC4(PhysObj* o, const Vec3* v, const Vec3* at) {
    Vec3 p;
    float s = 0.0023818598f;
    p.x = at->x * s;
    p.y = at->y * s;
    p.z = at->z * s;
    ForceAdd(o, 2, v, &p);
}

// 0x801D5010
void fn_801D5010(PhysObj* o, const Vec3* v) {
    ForceAdd(o, 0, v, 0);
}

// 0x801D5110
void fn_801D5110(PhysObj* o, int pinned) {
    if (o->body != 0) {
        SceneNodeSetPinned(o->body, pinned);
    }
}

// ODE rotation (3x4, rows) to the game's matrix layout.
static inline void RotToMtx(float* out, const float* in) {
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[4];
    out[4] = in[5];
    out[5] = in[6];
    out[6] = in[8];
    out[7] = in[9];
    out[8] = in[10];
    out[9] = in[3];
    out[10] = in[7];
    out[11] = in[11];
}

// 0x801D513C: the object's world matrix.
void fn_801D513C(PhysObj* o, Mtx34* m) {
    float* pos = GeomGetPosition(o->geom);
    float x;
    float y;
    float z;
    x = pos[0];
    y = pos[1];
    z = pos[2];
    x *= 419.84f;
    y *= 419.84f;
    z *= 419.84f;
    Vec3 v;
    RotToMtx((float*)m, GeomGetRotation(o->geom));
    fn_800A0030(&v, m, &o->offset);
    ((float*)m)[9] = x - v.x;
    ((float*)m)[10] = y - v.y;
    ((float*)m)[11] = z - v.z;
}

// 0x801D5260
void fn_801D5260(PhysObj* o, Vec3* v) {
    if (o->body != 0) {
        SceneNodeSetVelocity(v->x, v->y, v->z, o->body);
    }
}

// 0x801D5298
void fn_801D5298(PhysObj* o, Vec3* v) {
    if (o->body != 0) {
        SceneNodeSetPos(0.0023818598f * v->x, 0.0023818598f * v->y, 0.0023818598f * v->z, o->body);
    }
}

// 0x801D52E0: moves the object to a world matrix.
void fn_801D52E0(PhysObj* o, const Mtx34* m) {
    Vec3 v;
    float r[12];
    const float* f = (const float*)m;

    fn_800A0030(&v, m, &o->offset);
    v.x += f[9];
    v.y += f[10];
    v.z += f[11];
    GeomSetPosition(o->geom, 0.0023818598f * v.x, 0.0023818598f * v.y, 0.0023818598f * v.z);
    r[0] = f[0];
    r[1] = f[1];
    r[2] = f[2];
    r[4] = f[3];
    r[5] = f[4];
    r[6] = f[5];
    r[8] = f[6];
    r[9] = f[7];
    r[10] = f[8];
    r[3] = f[9];
    r[7] = f[10];
    r[11] = f[11];
    GeomSetRotation(o->geom, r);
}

// 0x801D53D4: enables the object and wakes what touches it.
void fn_801D53D4(PhysObj* o) {
    if (o->body != 0) {
        SceneNodeSnapPrev(o->body);
    }
    if (o->geom != 0) {
        GeomEnable(o->geom);
        GeomSetCollideBits(o->geom, -1);
    }
    o->flags |= 1;
    if (o->geom != 0) {
        fn_801D7068((Geom*)sSpace, o->geom);
    }
}

// 0x801D5448: puts the object's body to sleep.
void fn_801D5448(PhysObj* o) {
    if (o->body != 0) {
        SceneNodeLock(o->body);
        PhysObj* p = SceneNodeGetOwner(o->body);
        p->flags &= ~6;
        p->hits = 0;
    }
}

// 0x801D549C
int fn_801D549C(PhysObj* o) {
    if (o->body == 0) {
        return 0;
    }
    return SceneNodeNotLocked(o->body) != 0;
}

// 0x801D54DC
u32 fn_801D54DC(PhysObj* o) {
    return o->flags & 1;
}

// 0x801D54E8: next object id.
int fn_801D54E8(void) {
    int id = sNextId++;
    if (sNextId == 0x1FFFFFFF) {
        sNextId = 0x10000000;
    }
    return id;
}

// 0x801D550C: disables the object.
void fn_801D550C(PhysObj* o) {
    if (o->body != 0) {
        SceneNodeLock(o->body);
    }
    if (o->geom != 0) {
        GeomDisable(o->geom);
        GeomSetCollideBits(o->geom, 0);
    }
    o->flags &= ~1;
}

// 0x801D556C: enables the object.
void fn_801D556C(PhysObj* o) {
    if (o->body != 0) {
        SceneNodeSnapPrev(o->body);
    }
    if (o->geom != 0) {
        GeomEnable(o->geom);
        GeomSetCollideBits(o->geom, -1);
    }
    o->flags |= 1;
}

// 0x801D55CC: destroys the object.
static inline void GeomListRemove(Geom* g) {
    Geom** p;
    Geom** end;
    for (p = (Geom**)sGeoms->mUnk0C, end = (Geom**)((char*)p + sGeoms->mLength); p != end; p++) {
        if (*p == g) {
            fn_8028B35C(sGeoms, p, 4);
            break;
        }
    }
}
static inline void ForcesForget(Geom* g, Force* f) {
    int n = sForceCount;
    int i;
    for (i = 0; i < n; i++, f++) {
        if (f->geom == g) {
            f->type = 8;
        }
    }
}
void fn_801D55CC(PhysObj* o) {
    if (o != 0) {
        if (o->body != 0) {
            fn_802789D0(o->body);
        }
        if (o->geom != 0) {
            GeomListRemove(o->geom);
            ForcesForget(o->geom, gForces);
            GeomDestroyGeom(o->geom);
        }
        VFree(o);
    }
}

// The index of a material by name hash (RED OAK when unknown).
static inline int MaterialIndex(int material) {
    int i;
    for (i = 0; i < 3; i++) {
        if (material == gMaterials[i].hash) {
            break;
        }
    }
    if (i == 3) {
        i = 0;
    }
    return i;
}

// Adds a new geom to the geom list.
static inline void GeomListAdd(Geom* g) {
    *(Geom**)fn_8028B3BC(sGeoms, (char*)sGeoms->mUnk0C + sGeoms->mLength, 4) = g;
}

// 0x801D5694: capsule object.
PhysObj* fn_801D5694(float radius, float length, int owner, int mat, int material, int cat, int dir,
                     Vec3* offset, int noBody) {
    Mass m;
    fn_8027649C(&m);
    radius = 0.0023818598f * radius;
    length = 0.0023818598f * length;
    Geom* g = CapsuleCreate(radius, length, sSpace);
    if (noBody != 0) {
        *(u32*)((char*)g + 4) |= 0x10;
    }
    GeomListAdd(g);
    if (material == (int)0xBF4C00C0) {
        fn_80276090(1.0f, 1.0f, 1.0f, 1.0f, &m);
    } else {
        fn_80276170(gMaterials[MaterialIndex(material)].density, radius, length, &m, dir + 1);
    }
    return fn_801D59A4(owner, mat, g, &m, cat, offset, noBody);
}

// 0x801D5810: box object.
PhysObj* fn_801D5810(float x, float y, float z, int owner, int mat, int material, int cat, Vec3* offset,
                     int noBody) {
    Mass m;
    fn_8027649C(&m);
    x = 0.0023818598f * x;
    y = 0.0023818598f * y;
    z = 0.0023818598f * z;
    Geom* g = BoxCreate(x, y, z, sSpace);
    if (noBody != 0) {
        *(u32*)((char*)g + 4) |= 0x10;
    }
    GeomListAdd(g);
    if (material == (int)0xBF4C00C0) {
        fn_80276090(1.0f, 1.0f, 1.0f, 1.0f, &m);
    } else {
        fn_80276090(gMaterials[MaterialIndex(material)].density, x, y, z, &m);
    }
    return fn_801D59A4(owner, mat, g, &m, cat, offset, noBody);
}

// 0x801D59A4: wraps a geom (and a new body, unless noBody) into a physics object.
PhysObj* fn_801D59A4(int owner, int mat, Geom* g, Mass* m, int cat, Vec3* offset, int noBody) {
    PhysObj* o;
    SceneNode* body;

    if (g == 0) {
        return 0;
    }
    o = (PhysObj*)VAlloc(0x48);
    if (o == 0) {
        GeomDestroyGeom(g);
        return 0;
    }
    void* probe = VAlloc(0x150);
    if (probe == 0) {
        VFree(o);
        GeomDestroyGeom(g);
        return 0;
    }
    VFree(probe);
    if (noBody != 0) {
        body = 0;
    } else {
        body = fn_80278B70(sWorld);
    }
    GeomSetBody(g, (Body*)body);
    if (body != 0) {
        fn_80278850(body, m);
        SceneNodeLock(body);
        GeomSetCollideBits(g, 0);
    } else {
        GeomSetCollideBits(g, -1);
    }
    GeomSetCategoryBits(g, cat);
    if (offset != 0) {
        o->offset.x = offset->x;
        o->offset.y = offset->y;
        o->offset.z = offset->z;
    } else {
        o->offset.x = 0.0f;
        o->offset.y = 0.0f;
        o->offset.z = 0.0f;
    }
    o->owner = owner;
    o->material = mat;
    o->body = body;
    o->geom = g;
    o->idle = 0.0f;
    o->flags = 1;
    o->damp = 0.01f;
    o->angDamp = m->data[0] / 10.0f;
    GeomSetGFlags(g, (int)o);
    if (body != 0) {
        SceneNodeSetOwner(body, (u32)o);
    }
    return o;
}

// 0x801D5B38: object of any shape (capsule, sphere, box).
PhysObj* fn_801D5B38(float x, float y, float z, int owner, int mat, int shape, int material, int cat,
                     Vec3* offset, int noBody) {
    switch (shape) {
    case (int)0xEF17C6D3: {
        if (y < x) {
            y = x;
        }
        return fn_801D5694(0.5f * y, z, owner, mat, material, cat, 2, offset, noBody);
    }
    case (int)0x922B5BCD: {
        if (y < x) {
            y = x;
        }
        if (z < y) {
            z = y;
        }
        Mass m;
        fn_8027649C(&m);
        float r = 0.0023818598f * (0.5f * z);
        Geom* g = SphereCreate(r, sSpace);
        if (noBody != 0) {
            *(u32*)((char*)g + 4) |= 0x10;
        }
        GeomListAdd(g);
        if (material == (int)0xBF4C00C0) {
            fn_80276090(1.0f, 1.0f, 1.0f, 1.0f, &m);
        } else {
            fn_802762AC(gMaterials[MaterialIndex(material)].density, r, &m);
        }
        return fn_801D59A4(owner, mat, g, &m, cat, offset, noBody);
    }
    case 0x101AB:
        return fn_801D5810(x, y, z, owner, mat, material, cat, offset, noBody);
    }
    return fn_801D5810(x, y, z, owner, mat, material, cat, offset, noBody);
}

// 0x801D60A0: is a debug flag set?
u32 fn_801D60A0(int flag) {
    return sDebugFlags & (1 << flag);
}

// 0x801D60B4: debug drawing.
void fn_801D60B4(void) {
    if (sDebugFlags & 1) {
        fn_801D6104((Geom*)sSpace);
    }
    if (sDebugFlags & 1) {
        RoomGeomDebugDraw();
    }
    if (sDebugFlags & 2) {
        fn_801E1354();
    }
}

// 0x801D6104: draws every geom of a space.
void fn_801D6104(Geom* space) {
    int n = SpaceGetCount((Space*)space);
    int i;
    for (i = 0; i < n; i++) {
        Geom* g = SpaceGetNumGeoms(space, i);
        int type = GeomGetType(g);
        u8 color[4];
        SceneNode* body = GeomGetBody(g);
        if (body == 0) {
            color[0] = 0xFF;
            color[1] = 0;
            color[2] = 0;
            color[3] = 0x80;
        } else if (SceneNodeNotLocked(body) == 0) {
            color[0] = 0;
            color[1] = 0;
            color[2] = 0xFF;
            color[3] = 0xFF;
        } else {
            color[0] = 0;
            color[1] = 0xFF;
            color[2] = 0;
            color[3] = 0xFF;
        }
        GeomGetGFlags(g);
        switch (type) {
        case 0: {
            float* p = GeomGetPosition(g);
            float x = p[0] * 419.84f;
            float y = p[1] * 419.84f;
            float z = p[2] * 419.84f;
            DebugDrawSphereWire(color[0], color[1], color[2], x, y, z, SphereGetRadius(g) * 419.84f);
            break;
        }
        case 1: {
            Vec3 pos;
            float lengths[3];
            Vec3 lo;
            Vec3 hi;
            Mtx34 rot;
            Mtx34 m;
            float* p = GeomGetPosition(g);
            pos.x = p[0] * 419.84f;
            pos.y = p[1] * 419.84f;
            pos.z = p[2] * 419.84f;
            RotToMtx((float*)&rot, GeomGetRotation(g));
            BoxGetLengths(g, lengths);
            lengths[0] *= 419.84f;
            lengths[1] *= 419.84f;
            lengths[2] *= 419.84f;
            hi.x = 0.5f * lengths[0];
            hi.y = 0.5f * lengths[1];
            hi.z = 0.5f * lengths[2];
            lo.x = -hi.x;
            lo.y = -hi.y;
            lo.z = -hi.z;
            fn_800A0168(&m, &rot);
            Mtx34StoreTranslation(&m, &pos);
            DebugDrawBoneBox(&lo, &m, color[0], color[1], color[2]);
            break;
        }
        case 2: {
            Vec3 pos;
            Mtx34 rot;
            float radius;
            float length;
            float* p = GeomGetPosition(g);
            pos.x = p[0] * 419.84f;
            pos.y = p[1] * 419.84f;
            pos.z = p[2] * 419.84f;
            RotToMtx((float*)&rot, GeomGetRotation(g));
            CapsuleGetParams(g, &radius, &length);
            radius *= 419.84f;
            length *= 419.84f;
            fn_801D64B8(radius, length, &pos, &rot, color);
            break;
        }
        case 8:
        case 9:
            fn_801D6104(g);
            break;
        }
    }
}

// One wire segment of the capsule drawing.
static inline void CapsuleSegment(Mtx34* m, Vec3* a, Vec3* b, float px, float py, float pz, u8 r, u8 g, u8 bl) {
    fn_800A0030(a, m, a);
    fn_800A0030(b, m, b);
    a->x += px;
    a->y += py;
    a->z += pz;
    b->x += px;
    b->y += py;
    b->z += pz;
    fn_800090BC(a, b, r, g, bl);
}

// 0x801D64B8: draws a capsule as wire segments.
void fn_801D64B8(float radius, float length, Vec3* pos, Mtx34* rot, u8* color) {
    Mtx34 m = *rot;
    Vec3 a;
    Vec3 b;
    int angle;
    Mtx34StoreTranslation(&m, pos);
    u8 g = color[1];
    angle = 0;
    u8 bl = color[2];
    float top = 0.5f * length;
    u8 r = color[0];
    float bottom = 0.5f * -length;
    float px = pos->x;
    float py = pos->y;
    float pz = pos->z;
    do {
        float next = 45.0f + (float)angle;
        a.x = radius * fn_800A1394((float)angle);
        a.y = radius * fn_800A1418((float)angle);
        a.z = top;
        b.x = radius * fn_800A1394(next);
        b.y = radius * fn_800A1418(next);
        b.z = top;
        CapsuleSegment(&m, &a, &b, px, py, pz, r, g, bl);
        a.x = radius * fn_800A1394((float)angle);
        a.y = radius * fn_800A1418((float)angle);
        a.z = bottom;
        b.x = radius * fn_800A1394(next);
        b.y = radius * fn_800A1418(next);
        b.z = bottom;
        CapsuleSegment(&m, &a, &b, px, py, pz, r, g, bl);
        a.x = radius * fn_800A1394((float)angle);
        a.y = radius * fn_800A1418((float)angle);
        a.z = top;
        b.x = radius * fn_800A1394((float)angle);
        b.y = radius * fn_800A1418((float)angle);
        b.z = bottom;
        CapsuleSegment(&m, &a, &b, px, py, pz, r, g, bl);
        angle += 45;
    } while (angle < 360);
}

// 0x801D6928: steps the world.
void fn_801D6928(float dt) {
    int step;
    float h;

    if (dt < 10.0f) {
        sSteps = 1;
        dWorldSetGravity(sWorld, 0.0f, 0.0f, -9.81f);
    } else {
        sSteps = 4;
        dWorldSetGravity(sWorld, 0.0f, 0.0f, -9.81f);
    }
    h = dt / (1000.0f * (float)sSteps);
    for (step = 0; step < sSteps; step++) {
        Space* space = sSpace;
        Force* f = gForces;
        int n = sForceCount;
        int i;
        for (i = 0; i < n; i++, f++) {
            if (f->type != 8) {
                if (step == 0) {
                    fn_801D7068((Geom*)space, f->geom);
                }
                switch (f->type) {
                case 0:
                    SceneNodeAddAccel(f->a.x, f->a.y, f->a.z, f->body);
                    break;
                case 1:
                    fn_80278790(f->a.x, f->a.y, f->a.z, f->body);
                    break;
                case 2:
                    fn_802786A4(f->a.x, f->a.y, f->a.z, f->b.x, f->b.y, f->b.z, f->body);
                    break;
                case 3:
                    fn_80278534(f->a.x, f->a.y, f->a.z, f->b.x, f->b.y, f->b.z, f->body);
                    break;
                case 4:
                    fn_802785F8(f->a.x, f->a.y, f->a.z, f->b.x, f->b.y, f->b.z, f->body);
                    break;
                case 5:
                    fn_8027841C(f->a.x, f->a.y, f->a.z, f->b.x, f->b.y, f->b.z, f->body);
                    break;
                case 6:
                    SceneNodeAddRot(f->a.x, f->a.y, f->a.z, f->body);
                    break;
                case 7:
                    fn_80278720(f->a.x, f->a.y, f->a.z, f->body);
                    break;
                }
            }
        }
        {
            Space* sp = sSpace;
            int count = SpaceGetCount(sp);
            int k;
            for (k = 0; k < count; k++) {
                Geom* g = SpaceGetNumGeoms((Geom*)sp, k);
                SceneNode* body;
                if (GeomIsEnabled(g) != 0 && (body = GeomGetBody(g)) != 0 && SceneNodeNotLocked(body) != 0) {
                    PhysObj* o = GeomGetGFlags(g);
                    float* v = SceneNodePosPtr(body);
                    float d = -o->damp;
                    SceneNodeAddAccel(v[0] * d, v[1] * d, v[2] * d, body);
                    v = SceneNodeVelocityPtr(body);
                    d = -o->angDamp;
                    SceneNodeAddRot(v[0] * d, v[1] * d, v[2] * d, body);
                }
            }
        }
        SpaceCollideSelf((Geom*)sSpace, &step, fn_801D72F8);
        if (sPaused == 0) {
            static int sIterations = 2;
            dWorldSetQuickStepNumIterations(sWorld, sIterations);
            dWorldStep(sWorld, h);
        }
        fn_80277958(sContacts);
    }
    sForceCount = 0;
    fn_801D6CB8(dt, (Geom*)sSpace);
    fn_801E13A0(dt);
}

// Squared length of a vector.
static inline float LenSq(float* v) {
    float x = v[0] * v[0];
    float y = v[1] * v[1];
    float z = v[2] * v[2];
    return x + y + z;
}

// 0x801D6CB8: puts resting bodies (and resting islands) to sleep.
void fn_801D6CB8(float dt, Geom* space) {
    SceneNode* list[34];
    int n = SpaceGetCount((Space*)space);
    int i;
    for (i = 0; i < n; i++) {
        Geom* g = SpaceGetNumGeoms(space, i);
        SceneNode* body;
        if (GeomIsEnabled(g) != 0 && (body = GeomGetBody(g)) != 0 && SceneNodeNotLocked(body) != 0) {
            Geom** p = (Geom**)sGeoms->mUnk0C;
            if (g != 0) {
                Geom** end = (Geom**)((char*)p + sGeoms->mLength);
                for (; p != end && *p != g; p++) {
                }
                if (p != end) {
                    PhysObj* o = GeomGetGFlags(g);
                    float* pos = SceneNodePosPtr(body);
                    float* vel = SceneNodeVelocityPtr(body);
                    fn_801D77C4(o, pos);
                    if (LenSq(pos) + LenSq(vel) >= 0.1f) {
                        o->idle = 0.0f;
                    } else {
                        o->idle += dt;
                        if (o->idle >= 1000.0f) {
                            if (SceneNodeListCount(body) == 0) {
                                SceneNodeLock(body);
                                o->flags &= ~6;
                                o->hits = 0;
                            } else {
                                int count = fn_801D6EC8(body, 0, 0, list);
                                if (count != 0) {
                                    SceneNode** l = list;
                                    int k;
                                    for (k = 0; k < count; k++) {
                                        PhysObj* q = SceneNodeGetOwner(*l);
                                        SceneNodeLock(*l);
                                        l++;
                                        q->flags &= ~6;
                                        q->hits = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// 0x801D6EC8: collects the island of bodies jointed to a body; 0 if any of them still moves.
int fn_801D6EC8(SceneNode* body, Geom* from, int n, SceneNode** list) {
    int joints;
    int i;
    list[n++] = body;
    joints = SceneNodeListCount(body);
    for (i = 0; i < joints; i++) {
        SceneNode* other;
        Geom* j = SceneNodeListGet(body, i);
        if (j != from) {
            other = (SceneNode*)fn_802777BC(j, 0);
            if (other == body) {
                other = (SceneNode*)fn_802777BC(j, 1);
            }
            if (other != 0 && SceneNodeNotLocked(other) != 0) {
                int k;
                for (k = 0; k < n; k++) {
                    if (list[k] == other) {
                        break;
                    }
                }
                if (k == n) {
                    PhysObj* o = SceneNodeGetOwner(other);
                    float* pos = SceneNodePosPtr(other);
                    float* vel = SceneNodeVelocityPtr(other);
                    if (LenSq(pos) + LenSq(vel) >= 0.1f) {
                        return 0;
                    }
                    if (o->idle < 1000.0f) {
                        return 0;
                    }
                    {
                        int r = fn_801D6EC8(other, j, n, list);
                        if (r == 0) {
                            return 0;
                        }
                        n = r;
                    }
                }
            }
        }
    }
    return n;
}

// 0x801D7068: wakes every sleeping body whose box overlaps the geom's (grown) box, recursively.
void fn_801D7068(Geom* space, Geom* g) {
    float box[6];
    float other[6];
    if (g != 0) {
        GeomGetAabb(g, box);
        float* p = GeomGetPosition(g);
        box[0] += p[0] - 1.2195122f;
        box[1] += 1.2195122f + p[0];
        box[2] += p[1] - 1.2195122f;
        box[3] += 1.2195122f + p[1];
        box[4] += p[2] - 1.2195122f;
        box[5] += 1.2195122f + p[2];
        int n = SpaceGetCount((Space*)space);
        int i;
        for (i = 0; i < n; i++) {
            Geom* h = SpaceGetNumGeoms(space, i);
            SceneNode* body = GeomGetBody(h);
            if (body != 0 && SceneNodeNotLocked(body) == 0) {
                bool hit;
                GeomGetAabb(h, other);
                p = GeomGetPosition(h);
                other[0] += p[0];
                other[1] += p[0];
                other[2] += p[1];
                other[3] += p[1];
                other[4] += p[2];
                other[5] += p[2];
                if (0.5f * (box[1] - box[0]) + 0.5f * (other[1] - other[0]) <
                        __fabsf(0.5f * (other[1] + other[0]) - 0.5f * (box[1] + box[0])) ||
                    0.5f * (box[3] - box[2]) + 0.5f * (other[3] - other[2]) <
                        __fabsf(0.5f * (other[3] + other[2]) - 0.5f * (box[3] + box[2])) ||
                    0.5f * (box[5] - box[4]) + 0.5f * (other[5] - other[4]) <
                        __fabsf(0.5f * (other[5] + other[4]) - 0.5f * (box[5] + box[4]))) {
                    hit = false;
                } else {
                    hit = true;
                }
                if (hit) {
                    GeomGetGFlags(h)->idle = 0.0f;
                    SceneNodeSnapPrev(body);
                    fn_801D7068(space, h);
                }
            }
        }
    }
}

// Records a contact on an object for the impact sounds.
static inline void ContactNote(PhysObj* o, SceneNode* body, Geom* other, int surface, float nx, float ny, float nz) {
    if ((o->flags & 2) == 0) {
        float* v = SceneNodePosPtr(body);
        if (LenSq(v) > 0.1f) {
            if (o->hits == 0 || o->other != other || o->surface != surface ||
                __fabsf(o->normal.z * nz + o->normal.x * nx + o->normal.y * ny) < 0.99f) {
                o->hits = 1;
                o->other = other;
                o->surface = surface;
                o->normal.x = nx;
                o->normal.y = ny;
                o->normal.z = nz;
                float s = LenSq(v);
                float c = 1.0f;
                if (s >= 1.0f) {
                    c = s;
                    if (s > 40.0f) {
                        c = 40.0f;
                    }
                }
                c = 0.1010101f * c - 0.01010101f;
                float t = 0.0f;
                if (c >= 0.0f) {
                    t = c;
                    if (c > 1.0f) {
                        t = 1.0f;
                    }
                }
                o->strength = t;
            } else {
                o->hits <<= 1;
                o->hits |= 1;
            }
            o->flags |= 2;
        }
    }
}

// One ODE dContact (0x68 bytes): surface parameters, then the contact geometry at +0x2C.
struct Contact {
    int mode;
    float mu;
    float bounce;
    char pad0C[0x20];
    float pos[4];               // 0x2C
    float normal[4];            // 0x3C
    float depth;                // 0x4C
    char pad50[0x18];
};

// 0x801D72F8: ODE near callback: creates the contact joints.
void fn_801D72F8(void* data, Geom* a, Geom* b) {
    Contact contacts[16];
    SceneNode* ba = GeomGetBody(a);
    SceneNode* bb = GeomGetBody(b);
    if (ba != 0 && bb != 0 && fn_80277778(ba, bb, 4) != 0) {
        return;
    }
    bool any = false;
    if (ba != 0 || bb != 0) {
        any = true;
    }
    if (!any) {
        return;
    }
    if ((GeomGetCollideBits(a) & GeomGetCollideBits(b)) == 0) {
        return;
    }
    int ca = GeomGetCategoryBits(a);
    if (ca != 0x1FFFFFFF && ca == GeomGetCategoryBits(b)) {
        return;
    }
    int ea = ba == 0 ? 0 : SceneNodeNotLocked(ba);
    int eb = bb == 0 ? 0 : SceneNodeNotLocked(bb);
    if (ea == 0 && eb == 0) {
        return;
    }
    int n = GeomCollideDispatch(a, b, 0x10, (ContactGeom*)contacts[0].pos, 0x68);
    if (n == 0) {
        return;
    }
    if (*(int*)data == 0) {
        if (ba != 0 && a != 0) {
            PhysObj* o = GeomGetGFlags(a);
            ContactNote(o, ba, b, __cvt_fp2unsigned(contacts[0].pos[0]), contacts[0].normal[0],
                        contacts[0].normal[1], contacts[0].normal[2]);
        }
        if (bb != 0 && b != 0) {
            PhysObj* o = GeomGetGFlags(b);
            ContactNote(o, bb, a, __cvt_fp2unsigned(contacts[0].pos[0]), -contacts[0].normal[0],
                        -contacts[0].normal[1], -contacts[0].normal[2]);
        }
    }
    {
        Contact* c = contacts;
        int i;
        for (i = 0; i < n; i++, c++) {
            c->mode = 0x3000;
            c->mu = 0.7f;
            c->bounce = 0.0f;
            fn_80277804(fn_80277EC8(sWorld, sContacts, c), ba, bb);
        }
    }
}

// The owner id of the object that owns a geom (-1 if the geom is gone).
static inline int GeomOwnerId(Geom* g) {
    Geom** p = (Geom**)sGeoms->mUnk0C;
    if (g == 0) {
        return -1;
    }
    Geom** end = (Geom**)((char*)p + sGeoms->mLength);
    for (; p != end && *p != g; p++) {
    }
    if (p == end) {
        return -1;
    }
    PhysObj* o = GeomGetGFlags(g);
    if (o == 0) {
        return -1;
    }
    return o->owner;
}

// 0x801D77C4: reports impacts and slides of an object.
void fn_801D77C4(PhysObj* o, float* vel) {
    if (o->hits == 0) {
        return;
    }
    if ((o->flags & 2) == 0) {
        o->hits <<= 1;
    }
    u32 hits = o->hits;
    if ((o->flags & 4) == 0) {
        if (hits > 15) {
            int bits = 0;
            int id;
            for (; hits != 0; hits >>= 1) {
                if (hits & 1) {
                    bits++;
                }
            }
            if (LenSq(vel) < 1.0f || bits < 3) {
                id = GeomOwnerId(o->other);
                fn_801D8FD4(o->strength, o->owner, id, 1, o->surface, &o->normal, o->material);
                o->flags &= ~6;
                o->hits = 0;
            } else {
                id = GeomOwnerId(o->other);
                fn_801D8FD4(o->strength, o->owner, id, 2, o->surface, &o->normal, o->material);
                o->flags |= 4;
            }
            if (id != 0 && GeomGetBody(o->other) == 0) {
                Geom** p = (Geom**)sGeoms->mUnk0C;
                if (o->other == 0) {
                    return;
                }
                Geom** end = (Geom**)((char*)p + sGeoms->mLength);
                for (; p != end && *p != o->other; p++) {
                }
                if (p == end) {
                    return;
                }
                PhysObj* q = GeomGetGFlags(o->other);
                int mat = q == 0 ? 0 : q->material;
                Vec3 n;
                n.x = -o->normal.x;
                n.y = -o->normal.y;
                n.z = -o->normal.z;
                fn_801D8FD4(0.0f, id, o->owner, 1, 0, &n, mat);
            }
        }
    } else if ((hits & 0xF) == 0) {
        o->flags &= ~6;
        o->hits = 0;
    } else {
        float s = LenSq(vel);
        float c = 1.0f;
        if (s >= 1.0f) {
            c = s;
            if (s > 40.0f) {
                c = 40.0f;
            }
        }
        float t = 0.0f;
        float d = 0.1010101f * c - 0.01010101f;
        if (d >= t) {
            t = d;
            if (d > 1.0f) {
                t = 1.0f;
            }
        }
        fn_801D8FD4(t, o->owner, GeomOwnerId(o->other), 4, o->surface, &o->normal, o->material);
    }
    o->flags &= ~2;
}

// 0x801D7BA8
void fn_801D7BA8(void) {
    fn_801E1400();
}

// 0x801D7BC8: shuts the physics down.
void fn_801D7BC8(void) {
    fn_801E1420();
    fn_80277B38(sContacts);
    GeomDelete((Geom*)sSpace);
    fn_80277600(sWorld);
    AmmoRefillResetStatics();
    delete sGeoms;
}

// The engine's name hash.
static inline u32 StrHash(const char* s) {
    u32 h;
    if (s == 0) {
        h = 0;
    } else {
        int i;
        h = 0;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
    }
    return h;
}

// 0x801D7C24: sets the physics up.
void fn_801D7C24(void) {
    GeomClassDesc desc;
    dSetAllocHandler(fn_801D7F34);
    dSetReallocHandler(fn_801D7F2C);
    dSetFreeHandler(fn_801D7F0C);
    SetErrorHandler(fn_801D7E74);
    SetMessageHandler(fn_801D7DDC);
    sGeoms = new GeomVec;
    sWorld = fn_802776D4();
    dWorldSetAutoDisableFlag(sWorld, 1);
    sSpace = HashSpaceCreate(0);
    sContacts = fn_80277D38(0);
    dWorldSetGravity(sWorld, 0.0f, 0.0f, -9.81f);
    desc.bytes = 0x1C;
    desc.collider = (void*)fn_801D2634;
    desc.aabb = (void*)fn_801D25D8;
    desc.aabbTest = (void*)fn_801D25CC;
    desc.dtor = (void*)fn_801D25D4;
    sRoomClass = GeomRegisterClass((UserGeomClass*)&desc);
    sNextId = 0x10000000;
    {
        int i;
        for (i = 0; i < 3; i++) {
            gMaterials[i].hash = StrHash(gMaterials[i].name);
        }
    }
    sForceCount = 0;
    fn_801E1494();
    SpaceSetFields(sSpace, sSpaceA, sSpaceB);
}

// 0x801D7DDC: ODE message handler.
void fn_801D7DDC(int num, const char* fmt, void* ap) {
    char buf[0x100] = { 0 };
    sprintf(buf, "ODE Error: %d ", num);
    vsprintf(buf + strlen(buf), fmt, ap);
}

// 0x801D7E74: ODE error handler.
void fn_801D7E74(int num, const char* fmt, void* ap) {
    char buf[0x100] = { 0 };
    sprintf(buf, "ODE Internal Error: %d ", num);
    vsprintf(buf + strlen(buf), fmt, ap);
}

// 0x801D7F0C
void fn_801D7F0C(void* p, u32 size) {
    VFree(p);
}

// 0x801D7F2C
void* fn_801D7F2C(void* p, u32 a, u32 b) {
    return 0;
}

// 0x801D7F34
void* fn_801D7F34(u32 size) {
    return VAlloc(size);
}

// 0x801D7F54: a point (w = 1).
void fn_801D7F54(float* out, const Vec3* in) {
    out[0] = in->x;
    out[1] = in->y;
    out[2] = in->z;
    out[3] = 1.0f;
}

// 0x801D7F78
void fn_801D7F78(float* out, const float* in) {
    RotToMtx(out, in);
}

}  // extern "C"

// 0x801D7FDC
GeomVec::~GeomVec() {
    fn_8028B960(this);
}

// 0x801D8044
void GeomVec::Construct(void* p, u32 bytes) {
    ((ScreenEffectBuffer*)this)->Commit((int)p, bytes);
    fn_801D80F4((Geom**)p, (Geom**)((char*)p + (bytes & ~3)));
}

// 0x801D808C
void GeomVec::Destruct(void* p, u32 bytes) {
    Geom** first = (Geom**)p;
    fn_801D8114(first, (Geom**)((char*)first + (bytes & ~3)));
    ((ScreenEffectBuffer*)this)->Append((unsigned char*)p, bytes);
}

// 0x801D80EC
int GeomVec::ElemSize() {
    return 4;
}

// 0x801D80F4: constructs a range of elements (nothing to do for pointers).
extern "C" void fn_801D80F4(Geom** first, Geom** last) {
    for (; first < last; first++) {
    }
}

// 0x801D8114: destroys a range of elements (nothing to do for pointers).
extern "C" void fn_801D8114(Geom** first, Geom** last) {
    for (; first < last; first++) {
    }
}

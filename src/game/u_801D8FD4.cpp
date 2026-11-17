// u_801D8FD4.cpp: 0x801D8FD4-0x801D9A70, 8 functions. Message 0x74xx helpers.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   .sdata2 0x804D8C80-0x804D8CA0 (own 0.0 at 0x804D8C84 and int-to-float double: the scan cut)
//   fn_801D9580 is called only by fn_801D902C/fn_801D9114; fn_801D8FD4 sends msg 0x7404 and fn_801D96C4
//   sends 0x7403; fn_801D9028 is an empty hook called from AppSystemsInit
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

#include "game/types.h"

#define fabs(x) __fabs(x)

// The actor as seen by these helpers: the common prefix plus the shape at +0x40.
struct Actor {
    Placement place;                           // 0x00
    int (*handler)(int msg, void* data);       // 0x30
    char pad34[4];                             // 0x34
    int handle;                                // 0x38
    char pad3C[4];                             // 0x3C
    int* shape;                                // 0x40: octahedron of the visible bounds
};

// Message 0x7404 (0x24 bytes).
struct Msg7404 {
    int unk00;                                 // 0x00
    int a;                                     // 0x04
    int b;                                     // 0x08
    float f;                                   // 0x0C
    int c;                                     // 0x10
    Vec3 v;                                    // 0x14
    int d;                                     // 0x20
};

// Parameters of a blast: the curve values are given at the centre (Near) and at the radius (Far).
struct BlastParams {
    char pad00[8];
    float cone;                                // 0x08
    float radius;                              // 0x0C
    float forceNear;                           // 0x10
    float forceFar;                            // 0x14
    Vec3 spinNear;                             // 0x18
    Vec3 spinFar;                              // 0x24
};

// A blast sent to every actor in range (0x60 bytes).
struct Blast {
    Actor* source;                             // 0x00
    Actor* exclude;                            // 0x04
    PlaceF place;                              // 0x08
    float radius;                              // 0x38
    float cone;                                // 0x3C
    float forceNear;                           // 0x40
    float forceFar;                            // 0x44
    Vec3 spinNear;                             // 0x48
    Vec3 spinFar;                              // 0x54
};

// Message 0x7403: the blast as received by one actor (0x34 bytes).
struct Msg7403 {
    Actor* actor;                              // 0x00
    int unk04[3];                              // 0x04
    Vec3 pos;                                  // 0x10
    Vec3 force;                                // 0x1C
    Vec3 spin;                                 // 0x28
};

// Two direction vectors given to the spawn helper (0x18 bytes).
struct SpawnDirs {
    Vec3 a;                                    // 0x00
    Vec3 b;                                    // 0x0C
};

extern "C" {
int SendMsg(int to, int msg, void* data);
float fn_800E1020(Vec3* v);
void fn_8001ED98(IVec3* center, int radius, int (*callback)(Actor*, Blast*, float), void* arg, int flag);
void fn_800A025C(Vec3* out, Mtx33* m, int axis);
void fn_800A0168(Mtx34* out, Mtx34* in);
void fn_8016D864(SpawnDirs* out, int* shape);
void fn_8016D6BC(SpawnDirs* out, SpawnDirs* in);
void* fn_801D5B38(int handle, int a, int b, int c, int mask, void* dirs, int flag, float x, float y, float z);
void fn_801D5110(void* obj, int a);
void fn_801D5260(void* obj, Vec3* v);
void fn_801D5298(void* obj, Vec3* v);
void fn_801D52E0(void* obj, Mtx34* m);
void fn_801D556C(void* obj);
int fn_801D96C4(Actor* a, Blast* blast, float dist);
void fn_801D9580(Blast* blast);
}
float VarFindFloat(u32 set, u32 id, u32 key);
u32 VarFindHash(u32 a, u32 b, u32 c);
int ActorVisibleFlag(Actor* actor);
void ActorGetMatrix(Mtx34* m, Actor* a);
int* ViewOctahedronBounds(int* verts, const void* mtx);
int ViewFrustumTestOctahedron(const int* planes, const int* shape);
void fn_800167EC(float* out, const Vec3* pos, const Vec3* a, const Vec3* b, float f1, float f2);

// Sends message 0x7404.
extern "C" void fn_801D8FD4(int to, int a, int b, float f, int c, Vec3 v, int d) {
    Msg7404 msg;

    msg.a = a;
    msg.b = b;
    msg.f = f;
    msg.c = c;
    msg.d = d;
    msg.v = v;
    SendMsg(to, 0x7404, &msg);
}

// Empty hook called from AppSystemsInit.
extern "C" void fn_801D9028() {}

inline void BlastFire(Blast* blast, PlaceF* place, Actor* exclude, Actor* source) {
    blast->exclude = exclude;
    blast->source = source;
    blast->place = *place;
    fn_801D9580(blast);
}

// Starts a blast from a parameter block.
extern "C" int fn_801D902C(PlaceF* place, Actor* exclude, Actor* source, BlastParams* p) {
    if (exclude != 0) {
        Blast blast;

        blast.cone = p->cone;
        blast.radius = p->radius;
        blast.forceNear = p->forceNear;
        blast.forceFar = p->forceFar;
        blast.spinNear.x = p->spinNear.x;
        blast.spinNear.y = p->spinNear.y;
        blast.spinNear.z = p->spinNear.z;
        blast.spinFar.x = p->spinFar.x;
        blast.spinFar.y = p->spinFar.y;
        blast.spinFar.z = p->spinFar.z;
        BlastFire(&blast, place, exclude, source);
    }
    return 1;
}

// Starts a blast whose parameters are game variables of `id`.
extern "C" int fn_801D9114(PlaceF* place, Actor* exclude, Actor* source, u32 id) {
    if (exclude != 0) {
        Blast blast;

        blast.cone = VarFindFloat(0x7C73AF76, id, 0x942E1552);
        blast.radius = VarFindFloat(0x7C73AF76, id, 0x3FD77BF5);
        blast.forceNear = VarFindFloat(0x7C73AF76, id, 0x7A277147);
        blast.forceFar = VarFindFloat(0x7C73AF76, id, 0xD86F2F00);
        blast.spinNear.x = VarFindFloat(0x7C73AF76, id, 0x13298FE4);
        blast.spinNear.y = VarFindFloat(0x7C73AF76, id, 0x13298FE5);
        blast.spinNear.z = VarFindFloat(0x7C73AF76, id, 0x13298FE6);
        blast.spinFar.x = VarFindFloat(0x7C73AF76, id, 0x9C68889D);
        blast.spinFar.y = VarFindFloat(0x7C73AF76, id, 0x9C68889E);
        blast.spinFar.z = VarFindFloat(0x7C73AF76, id, 0x9C68889F);
        BlastFire(&blast, place, exclude, source);
    }
    return 1;
}

// Places a spawned object on the actor and resets its motion unless `keep` is set.
inline void SpawnAttach(void* obj, Actor* a, int keep) {
    Mtx34 m;

    ActorGetMatrix(&m, a);
    fn_800A0168(&m, &m);
    fn_801D52E0(obj, &m);
    if (keep == 0) {
        Vec3 zero;
        zero.x = 0.0f;
        zero.y = 0.0f;
        zero.z = 0.0f;
        fn_801D5298(obj, &zero);
        fn_801D5260(obj, &zero);
        fn_801D5110(obj, 1);
    }
    if (ActorVisibleFlag(a)) {
        fn_801D556C(obj);
    }
}

extern "C" void* fn_801D92F0(Actor* a, int b, int c, SpawnDirs* dirs, int keep, float x, float y, float z) {
    void* obj;

    if (a == 0) {
        return 0;
    }
    obj = fn_801D5B38(a->handle, -1, b, c, 0x1FFFFFFF, dirs, keep, 2.0f * x, 2.0f * y, 2.0f * z);
    if (obj != 0) {
        SpawnAttach(obj, a, keep);
    }
    return obj;
}

extern "C" void* fn_801D93F4(void* p, u32 id) {
    Actor* a = (Actor*)p;
    int keep;
    u32 b;
    u32 c;
    SpawnDirs dirs;
    void* obj;

    if (a == 0) {
        return 0;
    }
    b = VarFindHash(0xF3BEEEDF, id, 0x04B35781);
    c = VarFindHash(0xF3BEEEDF, id, 0x40795527);
    keep = VarFindHash(0xF3BEEEDF, id, 0x68824BFA) == 0x00276D8E;
    fn_8016D864(&dirs, a->shape);
    fn_8016D6BC(&dirs, &dirs);
    obj = fn_801D5B38(a->handle, -1, b, c, 0x1FFFFFFF, &dirs, keep, 2.0f * dirs.b.x, 2.0f * dirs.b.y,
                      2.0f * dirs.b.z);
    if (obj != 0) {
        SpawnAttach(obj, a, keep);
    }
    return obj;
}

// Sends the blast to its source alone (when in range) or to every actor in range.
extern "C" void fn_801D9580(Blast* blast) {
    Actor* source = blast->source;

    if (source != 0) {
        Vec3 d;
        d.x = (float)source->place.pos.x;
        d.y = (float)source->place.pos.y;
        d.z = (float)source->place.pos.z;
        d.x = d.x - blast->place.pos.x;
        d.y = d.y - blast->place.pos.y;
        d.z = d.z - blast->place.pos.z;
        if (fn_800E1020(&d) <= blast->radius) {
            fn_801D96C4(blast->source, blast, blast->radius);
        }
    } else {
        IVec3 c;
        c.x = (int)blast->place.pos.x;
        c.y = (int)blast->place.pos.y;
        c.z = (int)blast->place.pos.z;
        fn_8001ED98(&c, (int)blast->radius, fn_801D96C4, blast, 0);
    }
}

inline int VecNonZero(const Vec3* v) {
    return (fabs(v->x) > 1.1920929e-7f) + (fabs(v->y) > 1.1920929e-7f) + (fabs(v->z) > 1.1920929e-7f);
}

inline float VecNormalize(Vec3* v) {
    double x, y, z, len;
    float inv;

    if (VecNonZero(v) == 0) {
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 0.0f;
        return 0.0f;
    }
    x = (double)v->x;
    y = (double)v->y;
    z = (double)v->z;
    {
        double xx = x * x;
        double yy = y * y;
        double zz = z * z;

        len = xx + yy + zz;
    }
    inv = __frsqrte((float)len);
    v->x = (float)(x * inv);
    v->y = (float)(y * inv);
    v->z = (float)(z * inv);
    return __fres(inv);
}

// Whether the blast's cone reaches the actor's shape.
inline int BlastReaches(Blast* blast, int* shape) {
    float cone = blast->cone;
    Vec3 fwd;
    Vec3 up;
    float oct[28];

    if (cone > 359.0f) {
        return 1;
    }
    if (cone < 1.0f) {
        return 0;
    }
    fn_800A025C(&fwd, &blast->place.rot, 2);
    fn_800A025C(&up, &blast->place.rot, 1);
    if (cone > 180.0f) {
        Vec3 down;
        down.x = up.x * -1.0f;
        down.y = up.y * -1.0f;
        down.z = up.z * -1.0f;
        fn_800167EC(oct, &blast->place.pos, &fwd, &down, cone - 180.0f, 1.0f);
        if (ViewFrustumTestOctahedron((int*)oct, shape) != -1) {
            return 1;
        }
        cone = 180.0f;
    }
    fn_800167EC(oct, &blast->place.pos, &fwd, &up, cone, 1.0f);
    if (ViewFrustumTestOctahedron((int*)oct, shape) == -1) {
        return 0;
    }
    return 1;
}

// Blast callback for one actor in range: sends it message 0x7403 with the force and spin at its distance.
extern "C" int fn_801D96C4(Actor* a, Blast* blast, float dist) {
    if (a == blast->source || a != blast->exclude) {
        if (a->shape != 0 && a->handler != 0) {
            Mtx34 m;
            int* shape;

            ActorGetMatrix(&m, a);
            shape = ViewOctahedronBounds(a->shape, a);
            if (BlastReaches(blast, shape)) {
                Msg7403 msg;
                float t;
                float force;

                msg.pos.x = (float)a->place.pos.x;
                msg.pos.y = (float)a->place.pos.y;
                msg.pos.z = (float)a->place.pos.z;
                t = dist / (blast->radius * blast->radius);
                force = blast->forceNear + t * (blast->forceNear - blast->forceFar);
                if (force != 0.0f) {
                    Vec3 dir;
                    dir.x = msg.pos.x - blast->place.pos.x;
                    dir.y = msg.pos.y - blast->place.pos.y;
                    dir.z = msg.pos.z - blast->place.pos.z;
                    VecNormalize(&dir);
                    msg.force.x = dir.x * force;
                    msg.force.y = dir.y * force;
                    msg.force.z = dir.z * force;
                } else {
                    msg.force.x = 0.0f;
                    msg.force.y = 0.0f;
                    msg.force.z = 0.0f;
                }
                msg.spin.x = blast->spinNear.x + t * (blast->spinNear.x - blast->spinFar.x);
                msg.spin.y = blast->spinNear.y + t * (blast->spinNear.y - blast->spinFar.y);
                msg.spin.z = blast->spinNear.z + t * (blast->spinNear.z - blast->spinFar.z);
                msg.actor = a;
                a->handler(0x7403, &msg);
            }
        }
    }
    return 1;
}

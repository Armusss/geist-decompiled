// u_802A815C.cpp: a path-following actor type ("Actor62"): messages, spawn, transform, path update (0x802A815C-0x802A9824).
// Names are descriptive guesses. This unit owns a global doubly-linked list of
// actors (lbl_804D5768 is the head) and frees them all on shutdown.

typedef unsigned char u8;
typedef unsigned long u32;
typedef float f32;

#include "game/vecnorm.h"

// One entry of the type-0x62 actor's path: a world position plus three
// per-segment scalars (remaining length, random angle, random offset).
struct PathSeg {
    Vec3 pos;   // 0xC8
    f32 f;      // 0xD4
    f32 g;      // 0xD8
    f32 h;      // 0xDC
};

// The 0x30-byte block at the start of an actor; message 0x411 copies it out.
struct ActorHdr {
    int a[8];
    int b;
    int c[2];
    int d;
};

struct Actor {
    ActorHdr hdr;                // 0x00
    void (*handler)(int, int*);  // 0x30
    u8 pad_34[4];                // 0x34
    int id;                      // 0x38
    IVec3* posPtr;               // 0x3C (points at pos)
    u8 pad_40[4];                // 0x40
    int field_44;                // 0x44
    u8 pad_48[4];                // 0x48
    int field_4C;                // 0x4C
    IVec3 pos;                   // 0x50
    IVec3 size;                  // 0x5C
    int state;                   // 0x68
    int field_6C;                // 0x6C
    u8* field_70;                // 0x70 (points at field_80)
    int field_74;                // 0x74
    u8 pad_78[8];                // 0x78
    Vec3 field_80;               // 0x80
    Vec3 field_8C;               // 0x8C
    Vec3 field_98;               // 0x98
    Vec3 field_A4;               // 0xA4
    Vec3 field_B0;               // 0xB0
    Vec3 field_BC;               // 0xBC
    PathSeg path[0x9E];          // 0xC8
    f32 field_F98;               // 0xF98
    Actor* next;                 // 0xF9C
    Actor* prev;                 // 0xFA0
};

typedef u32 (*Actor62Handler)(u32 id, int msg, int* data, int param);

extern "C" {
// Path length / time scales: one pair for each fn_802A1D04() mode.
f32    lbl_804D3890 = 5000.0f;
f32    lbl_804D3894 = 1000.0f;
f32    lbl_804D3898 = 2660.0f;
f32    lbl_804D389C = 1000.0f;

f32    lbl_804D5760 = lbl_804D3890;  // current path length scale
f32    lbl_804D5764 = lbl_804D389C;  // current path time scale
Actor* lbl_804D5768;                 // head of the actor list
u32    lbl_804D576C;                 // last actor id handed out
f32    lbl_804D5770;                 // last update's time step

extern void* memset(void*, int, u32);
extern void* memcpy(void*, const void*, u32);

extern void fn_8000F60C(void);
extern void fn_8000F874(void);
extern void fn_80026660(int, Actor62Handler, int);
extern int  fn_80212730(u32);
extern u32  fn_802127A0(u32, int, int, int*, int, int, int);
extern u32  fn_80212814(u32, int, int, int, int, int);
extern u32  fn_8021284C(u32);
extern int  fn_802A1D04(void);
extern void fn_800A025C(void*, void*, int);
}

void* AssetFind(u32, u32);
void Actor62DrawPathTrail(void*);
void Draw3DBegin(int, void*);
void Draw3DEnd(void);
void DrawWorldQuadFlipped(f32, f32, f32*, u8, u8, u8, u8);
void* VAlloc(int);
void VFree(void*);
void ActorUnlinkOwnerObjects(Actor*);
void ActorRelinkOwners(Actor*);
void ActorFn800231A8(Actor*);
void DeferAdd(int, Vec3*, void (*)(void*), void*);

u32 Actor62Msg(u32 id, int msg, int* data, int param);
u32 Actor62SpawnMsg(u32 id, int* data);
u32 Actor62UpdatePath(u32 id, int* data, int owner);

// Fills a 3x3 rotation + translation block (PlaceF layout) from an actor.
void ActorGetMatrix(Mtx34*, Actor*);
// Per-actor handler trampoline and the msg 0x413 transform update.
void Actor62Dispatch(int msg, int* data);
u32 Actor62SetupTransform(u32 id, int* data);

// Free every actor still linked into the global list.
extern "C" void fn_802A815C(void)
{
    while (lbl_804D5768 != 0) {
        if (lbl_804D5768 != 0) {
            Actor* node = lbl_804D5768;

            AssetFind(0xF2E2BEB8, 0x0F6298F9);
            fn_8000F60C();
            fn_8021284C(node->id);
            ActorUnlinkOwnerObjects(node);
            if (node->next != 0) {
                node->next->prev = node->prev;
            }
            if (node->prev != 0) {
                node->prev->next = node->next;
            }
            if (lbl_804D5768 == node) {
                lbl_804D5768 = node->next;
            }
            VFree(node);
        }
    }
}

// Free every actor of the global list, then reset the list state and hand the
// actor-update callback (id 0x62) its handler.
extern "C" void fn_802A8204(void)
{
    while (lbl_804D5768 != 0) {
        if (lbl_804D5768 != 0) {
            Actor* node = lbl_804D5768;

            AssetFind(0xF2E2BEB8, 0x0F6298F9);
            fn_8000F60C();
            fn_8021284C(node->id);
            ActorUnlinkOwnerObjects(node);
            if (node->next != 0) {
                node->next->prev = node->prev;
            }
            if (node->prev != 0) {
                node->prev->next = node->next;
            }
            if (lbl_804D5768 == node) {
                lbl_804D5768 = node->next;
            }
            VFree(node);
        }
    }
    lbl_804D576C = 0;
    fn_80026660(0x62, Actor62Msg, 0);
}

// Look up a type-0x62 actor by its low 24-bit id.
static inline Actor* FindActor62(int id)
{
    for (Actor* a = lbl_804D5768; a != 0; a = a->next) {
        if ((a->id & 0xFFFFFF) == id) {
            return a;
        }
    }
    return 0;
}

static inline void IVec3Set(IVec3* v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

// Allocate and link a fresh type-0x62 actor. Returns its id, or 0 on failure.
static inline u32 CreateActor62(void)
{
    if (fn_802A1D04() == 0) {
        lbl_804D5760 = lbl_804D3890;
        lbl_804D5764 = lbl_804D3894;
    } else {
        lbl_804D5760 = lbl_804D3898;
        lbl_804D5764 = lbl_804D389C;
    }

    Actor* n = (Actor*)VAlloc(0xFA4);
    if (n != 0) {
        lbl_804D576C++;
        memset(n, 0, 0xFA4);
        n->id = (lbl_804D576C & 0xFFFFFF) | 0x62000000;
        n->handler = Actor62Dispatch;
        n->field_44 = -1;
        n->posPtr = &n->pos;
        n->field_4C = 0x44;
        n->state = 0;
        n->field_6C = 1;
        IVec3Set(&n->pos, -100, 0, -100);
        IVec3Set(&n->size, 100, (int)lbl_804D5760, 100);
        n->field_70 = (u8*)&n->field_80;
        n->field_74 = 3;
        AssetFind(0xF2E2BEB8, 0xF6298F9);
        fn_8000F874();
        n->next = lbl_804D5768;
        if (lbl_804D5768 != 0) {
            lbl_804D5768->prev = n;
        }
        lbl_804D5768 = n;
        return n->id;
    }
    return 0;
}

// msg 0x400: hand back the actor itself.
static inline u32 Actor62GetMsg(u32 id, int* data)
{
    id &= 0xFFFFFF;
    Actor* obj = FindActor62(id);
    if (obj != 0) {
        data[0] = (int)obj;
        return 1;
    }
    return 0;
}

// msg 0: queue the trail renderer.
static inline u32 Actor62DrawMsg(u32 id, int* data)
{
    id &= 0xFFFFFF;
    Actor* obj = FindActor62(id);
    if (obj != 0) {
        DeferAdd(1, &obj->field_80, Actor62DrawPathTrail, obj);
    }
    return 0;
}

// msg 0x411: copy the actor's header block out.
static inline u32 Actor62GetHdrMsg(u32 id, int* data)
{
    id &= 0xFFFFFF;
    Actor* obj = FindActor62(id);
    if (obj != 0) {
        *(ActorHdr*)(data + 4) = obj->hdr;
        return 1;
    }
    return 0;
}

// msg 0x415: attach, detach or stop the actor's owner link.
static inline u32 Actor62OwnerMsg(u32 id, int* data)
{
    id &= 0xFFFFFF;
    Actor* obj = FindActor62(id);
    if (obj != 0) {
        int arg = data[1];
        if (arg != -1) {
            if (data[3] != 0) {
                return fn_802127A0(obj->id, arg, data[2], data + 4, 1, 0, 0);
            }
            return fn_80212814(obj->id, arg, data[2], 1, 0, 0);
        }
        return fn_8021284C(obj->id);
    }
    return 0;
}

// msg 0x41E: create a new actor.
static inline u32 Actor62CreateMsg(u32 id, int* data)
{
    switch (data[1]) {
    case 0x12D80:
        data[3] = (int)CreateActor62();
        break;
    default:
        return 0;
    }
    return 1;
}

// Message handler for the type-0x62 actors: find, query, create and dispatch.
u32 Actor62Msg(u32 id, int msg, int* data, int param)
{
    id &= 0xFFFFFF;
    data[0] = (int)FindActor62(id);
    u32 agent = id | 0x62000000;

    switch (msg) {
    case 0x400:
        return Actor62GetMsg(agent, data);
    case 0:
        return Actor62DrawMsg(agent, data);
    case 0x414:
        return Actor62UpdatePath(agent, data, fn_80212730(agent));
    case 0x415:
        return Actor62OwnerMsg(agent, data);
    case 0x411:
        return Actor62GetHdrMsg(agent, data);
    case 0x413:
        return Actor62SetupTransform(agent, data);
    case 0x41D:
        return Actor62SpawnMsg(agent, data);
    case 0x41E:
        return Actor62CreateMsg(agent, data);
    }
    return 0;
}

// Second half of the type-0x62 messages: start a copy or destroy the actor.
u32 Actor62SpawnMsg(u32 id, int* data)
{
    id &= 0xFFFFFF;
    Actor* obj = FindActor62(id);

    switch (data[1]) {
    case 0x210E6C:
        if (obj != 0) {
            AssetFind(0xF2E2BEB8, 0xF6298F9);
            fn_8000F60C();
            fn_8021284C(obj->id);
            ActorUnlinkOwnerObjects(obj);
            if (obj->next != 0) {
                obj->next->prev = obj->prev;
            }
            if (obj->prev != 0) {
                obj->prev->next = obj->next;
            }
            if (lbl_804D5768 == obj) {
                lbl_804D5768 = obj->next;
            }
            VFree(obj);
        }
        break;
    case 0x7A281983:
        if (data[3] != 0 && obj->state == 0) {
            obj->state = 1;
            obj->field_F98 = 0.0f;
            obj->field_6C = 1;
            break;
        }
        if (data[3] == 0 && obj->state == 1) {
            int out[4];
            u32 newId = CreateActor62();
            Actor* n = FindActor62(newId & 0xFFFFFF);

            if (n != 0) {
                *(f32*)(out + 1) = lbl_804D5770;
                n->state = 2;
                memcpy(&n->field_80, &obj->field_80, 0x24);
                memcpy(&n->field_A4, &obj->field_A4, 0x24);
                memcpy(n->path, obj->path, 0xED0);
                n->hdr = obj->hdr;
                fn_80212814(n->id, fn_80212730(obj->id), -1, 1, 0, 0);
                ActorFn800231A8(n);
                Actor62UpdatePath(n->id, out, fn_80212730(obj->id));
            }
            obj->state = 0;
            memset(obj->path, 0, 0xED0);
        }
        break;
    default:
        return 0;
    }
    return 1;
}

// Trampoline installed as a type-0x62 actor's update handler: resolve the
// actor from the message payload and forward the message to the dispatcher.
void Actor62Dispatch(int msg, int* data)
{
    Actor* a = (Actor*)data[0];
    Actor62Msg(a->id, msg, data, 0);
}

// Message 0x413: copy the caller's transform into the actor and rebuild both
// of its world-space position vectors (B0 scaled by a per-frame factor, BC by
// the plain one); the result is latched into 0x8C/0x98 for the next update.
u32 Actor62SetupTransform(u32 id, int* data)
{
    id &= 0xFFFFFF;
    Actor* a = FindActor62(id);
    if (a != 0) {
        if (a->state == 1) {
            int* src = (int*)data[1];
            a->hdr = *(ActorHdr*)src;

            PlaceF mtx;
            ActorGetMatrix((Mtx34*)&mtx, a);

            a->field_80 = mtx.pos;
            fn_800A025C(&a->field_B0, &mtx, 1);
            f32 scale = lbl_804D5760 / 2.0f;
            a->field_B0.x *= scale;
            a->field_B0.y *= scale;
            a->field_B0.z *= scale;
            a->field_B0.x += a->field_80.x;
            a->field_B0.y += a->field_80.y;
            a->field_B0.z += a->field_80.z;

            fn_800A025C(&a->field_BC, &mtx, 1);
            f32 base = lbl_804D5760;
            a->field_BC.x *= base;
            a->field_BC.y *= base;
            a->field_BC.z *= base;
            a->field_BC.x += a->field_80.x;
            a->field_BC.y += a->field_80.y;
            a->field_BC.z += a->field_80.z;

            if (a->field_6C != 0) {
                a->field_8C = a->field_B0;
                a->field_98 = a->field_BC;
                a->field_6C = 0;
            }
        }
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Per-frame path update of a type-0x62 actor: advances its 158 path segments,
// rebuilds the world-space curve from 32 Bezier samples, scatters the trail
// effects and removes the actor once its path is used up. Returns 0 when the
// actor comes back out of the list, 1 otherwise.
// ---------------------------------------------------------------------------

// The 0x48-byte snapshot block that fn_800201E4 hands back through its hook.
struct Actor62State {
    int f0, f1, f2, f3, f4, f5, f6, f7, f8, f9;  // 0x00
    int p0[2];   // 0x28
    int p1[2];   // 0x30
    int f10;     // 0x38
    int p2[2];   // 0x3C
    int f11;     // 0x44
};

u32 Actor62CopyState(Actor62State* src, Actor62State* dst);

// Sample of the rebuilt curve (0x10 bytes): position plus arc length.
struct PathPoint {
    Vec3 p;
    f32 w;
};

// Out block of fn_800201E4; the result is copied out through Actor62CopyState,
// so it is 0x48 bytes as well.
struct PathQuery {
    u8 pad[0x30];
    int startX, startY, startZ;   // 0x30
    int dirX, dirY, dirZ;         // 0x3C
};

extern "C" {
int   fn_800201E4(int handle, Vec3* seg, u32 flags,
                  u32 (*copy)(Actor62State*, Actor62State*), PathQuery* out);
void  fn_8009F7A8(Mtx34* out, const Vec3* dir);
f32   fn_800E1020(Vec3* v);
void  fn_800E1098(Vec3* out, Vec3* in, f32 length);
void  fn_80122108(int, int, Vec3*, u32, int, f32, f32, f32, f32, f32, f32, f32);
void  SendMsg(int to, int msg, void* data);
int   rand(void);
}

void ViewBezierPoint(double t, int* curve, Vec3* out);

// x if it is positive, else 0.
static inline f32 PosOrZero(f32 x)
{
    return x > 0.0f ? x : 0.0f;
}

// v = v * keep + to * take, per axis.
static inline void Vec3Blend(Vec3* v, const Vec3* to, f32 keep, f32 take)
{
    v->x = v->x * keep + to->x * take;
    v->y = v->y * keep + to->y * take;
    v->z = v->z * keep + to->z * take;
}

// Take a type-0x62 actor out of the list and free it.
static inline void Actor62Destroy(Actor* a)
{
    if (a != 0) {
        AssetFind(0xF2E2BEB8, 0xF6298F9);
        fn_8000F60C();
        ActorUnlinkOwnerObjects(a);
        if (a->next != 0) {
            a->next->prev = a->prev;
        }
        if (a->prev != 0) {
            a->prev->next = a->next;
        }
        if (lbl_804D5768 == a) {
            lbl_804D5768 = a->next;
        }
        VFree(a);
    }
}

u32 Actor62UpdatePath(u32 id, int* data, int owner)
{
    int i;
    int moved;
    f32 dt;
    id &= 0xFFFFFF;
    Actor* a = FindActor62(id);
    if (a != 0) {
        int info[8];
        info[0] = 0;
        SendMsg(owner, 0x400, info);

        lbl_804D5770 = *(f32*)(data + 1);
        dt = lbl_804D5770;

        Vec3Blend(&a->field_8C, &a->field_B0, 0.8f, 0.2f);
        Vec3Blend(&a->field_98, &a->field_BC, 0.9f, 0.1f);

        if (a->state == 1) {
            int i;
            int n;
            f32 rate;
            f32 slip;
            f32 rem;

            a->field_F98 += dt;
            rate = 158.0f / lbl_804D5764;
            slip = lbl_804D5760 / lbl_804D5764;
            int count = (int)(a->field_F98 * rate);
            rem = (f32)count / rate;
            a->field_F98 -= rem;
            n = count;
            for (i = 0; i < 0x9E && n != 0; i++) {
                if (0.0f == a->path[i].f) {
                    n--;
                    a->path[i].f = lbl_804D5760 - slip * (rem / (f32)n);
                    a->path[i].g = (f32)(rand() % 360);
                    a->path[i].h = 1.0f - (f32)(rand() % 50 - 25) / 100.0f;
                }
            }
        }

        moved = 0;
        for (i = 0; i < 0x9E; i++) {
            f32 v = a->path[i].f - dt * (lbl_804D5760 / lbl_804D5764);

            a->path[i].f = v > 0.0f ? v : 0.0f;
            if (a->path[i].f > 0.0f) {
                moved = 1;
            }
        }

        PathPoint pts[32];
        pts[0].p.x = a->field_80.x;
        pts[0].p.y = a->field_80.y;
        pts[0].p.z = a->field_80.z;
        pts[0].w = 0.0f;

        f32 step = 0.03125f;
        f32 t = step;
        for (int i = 1; i < 32; i++) {
            Vec3 d;

            ViewBezierPoint(t, (int*)&a->field_70, &pts[i].p);
            d.x = pts[i].p.x - pts[i - 1].p.x;
            d.y = pts[i].p.y - pts[i - 1].p.y;
            d.z = pts[i].p.z - pts[i - 1].p.z;
            pts[i].w = pts[i - 1].w + fn_800E1020(&d);
            t += step;
        }

        Vec3 seg[2];
        seg[0].x = pts[0].p.x;
        seg[0].y = pts[0].p.y;
        seg[0].z = pts[0].p.z;
        seg[1].x = pts[31].p.x;
        seg[1].y = pts[31].p.y;
        seg[1].z = pts[31].p.z;

        PathQuery q;
        if (fn_800201E4(info[0], seg, 0x10000000, Actor62CopyState, &q) != 0) {
            Mtx34 basis;
            Vec3 dir;
            Vec3 delta;
            f32 range;
            f32 start = 0.0f;

            dir.x = (f32)q.dirX;
            dir.y = (f32)q.dirY;
            dir.z = (f32)q.dirZ;
            VecNormalize(&dir, &dir);
            fn_8009F7A8(&basis, &dir);

            delta.x = (f32)q.startX - seg[0].x;
            delta.y = (f32)q.startY - seg[0].y;
            delta.z = (f32)q.startZ - seg[0].z;
            range = PosOrZero(fn_800E1020(&delta) - 32.0f);
            if (start == range) {
                for (int i = 1; i < 32; i++) {
                    pts[i].p.x = pts[0].p.x;
                    pts[i].p.y = pts[0].p.y;
                    pts[i].p.z = pts[0].p.z;
                }
            } else {
                for (int i = 1; i < 32; i++) {
                    if (pts[i].w >= start + range) {
                        pts[i].w = 0.0f;
                    }
                }
            }
        }

        for (i = 0; i < 0x9E; i++) {
            if (a->path[i].f > 0.0f) {
                f32 want = lbl_804D5760 - a->path[i].f;
                for (int j = 1; j < 32; j++) {
                    if (pts[j].w >= want) {
                        Vec3 d;

                        want -= pts[j - 1].w;
                        d.x = pts[j].p.x - pts[j - 1].p.x;
                        d.y = pts[j].p.y - pts[j - 1].p.y;
                        d.z = pts[j].p.z - pts[j - 1].p.z;
                        fn_800E1098(&d, &d, want);
                        a->path[i].pos.x = d.x + pts[j - 1].p.x;
                        a->path[i].pos.y = d.y + pts[j - 1].p.y;
                        a->path[i].pos.z = d.z + pts[j - 1].p.z;
                        break;
                    }
                }
            }
        }

        PathSeg* ps;
        for (int i = 0; i < 0x9E; i++) {
            ps = &a->path[i];

            if (ps->f > 0.0f && i % 40 == 0) {
                if (fn_802A1D04() == 0) {
                    fn_80122108(info[0], owner, &ps->pos, 0xC8759B80, 0,
                                1.0f, 0.75f * (560.0f * (1.0f - ps->f / lbl_804D5760) + 40.0f),
                                400.0f, 400.0f, 0.75f, 0.4f, 1.0f);
                } else {
                    fn_80122108(info[0], owner, &ps->pos, 0xC8759B80, 0,
                                1.0f, 0.75f * (560.0f * (1.0f - ps->f / lbl_804D5760) + 40.0f),
                                500.0f, 500.0f, 0.75f, 0.4f, 1.0f);
                }
            }
        }

        ActorRelinkOwners(a);
        if (moved == 0 && a->state == 2) {
            Actor62Destroy(a);
            return 0;
        }
        return 1;
    }
    return 0;
}

// Copy one actor state snapshot over another. Returns 1.
u32 Actor62CopyState(Actor62State* src, Actor62State* dst)
{
    *dst = *src;
    return 1;
}

// Trail renderer for a type-0x62 actor, installed as its deferred update
// callback (so the actor arrives as an untyped pointer). Walks the 158 path
// segments and draws a quad at each one that is still alive; the quad's alpha
// fades as the segment's remaining length shrinks.
void Actor62DrawPathTrail(void* p)
{
    Actor* a = (Actor*)p;
    int i;

    if (a != 0) {
        Draw3DBegin((int)AssetFind(0xF2E2BEB8, 0xF6298F9), 0);
        for (i = 0; i < 0x9E; i++) {
            PathSeg* seg = &a->path[i];
            if (seg->f > 0.0f) {
                f32 k = 1.0f - seg->f / lbl_804D5760;
                f32 alpha = 255.0f * (1.0f - k);
                f32 size = 560.0f * k + 40.0f;

                size *= seg->h;
                DrawWorldQuadFlipped(size, seg->g, &seg->pos.x, 0xff, 0xff, 0xff,
                                     (int)alpha & 0xff);
            }
        }
        Draw3DEnd();
    }
}

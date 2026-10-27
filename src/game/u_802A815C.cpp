// u_802A815C.cpp: a path-following actor type ("Actor62"): messages, spawn, transform, path update (0x802A815C-0x802A9824).
// Names are descriptive guesses. This unit owns a global doubly-linked list of
// actors (lbl_804D5768 is the head) and frees them all on shutdown.

typedef unsigned char u8;
typedef unsigned long u32;
typedef float f32;

struct Vec3 {
    f32 x, y, z;
};

// One entry of the type-0x62 actor's path: a world position plus three
// per-segment scalars (remaining length, random angle, random offset).
struct PathSeg {
    Vec3 pos;   // 0xC8
    f32 f;      // 0xD4
    f32 g;      // 0xD8
    f32 h;      // 0xDC
};

// The 0x30-byte matrix the actor code works with: a 3x3 basis plus translation.
struct Mtx34 {
    f32 m[3][3];                 // 0x00
    Vec3 t;                      // 0x24
};

// The 0x30-byte block at the start of an actor; message 0x411 copies it out.
struct ActorHdr {
    int w[12];
};

struct Actor {
    ActorHdr hdr;                // 0x00
    void (*handler)(int, int*);  // 0x30
    u8 pad_34[4];                // 0x34
    u32 id;                      // 0x38
    Vec3* posPtr;                // 0x3C (points at pos)
    u8 pad_40[4];                // 0x40
    int field_44;                // 0x44
    u8 pad_48[4];                // 0x48
    int field_4C;                // 0x4C
    Vec3 pos;                    // 0x50
    Vec3 size;                   // 0x5C
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
extern Actor* lbl_804D5768;      // head of the actor list
extern u32    lbl_804D576C;      // last actor id handed out
extern f32    lbl_804D5760;
extern f32    lbl_804D5764;
extern f32    lbl_804D5770;
extern f32    lbl_804D3890;
extern f32    lbl_804D3894;
extern f32    lbl_804D3898;
extern f32    lbl_804D389C;
extern f32    lbl_804DA918;
extern f32    lbl_804DA91C;
extern f32    lbl_804DA920;
extern f32    lbl_804DA924;
extern f32    lbl_804DA928;
extern f32    lbl_804DA92C;
extern f32    lbl_804DA930;
extern f32    lbl_804DA934;
extern f32    lbl_804DA938;
extern f32    lbl_804DA93C;
extern f32    lbl_804DA940;
extern f32    lbl_804DA944;
extern f32    lbl_804DA948;
extern f32    lbl_804DA94C;
extern f32    lbl_804DA950;
extern f32    lbl_804DA954;
extern f32    lbl_804DA958;
extern f32    lbl_804DA95C;
extern f32    lbl_804DA968;

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
void DrawWorldQuadFlipped(f32, f32, Vec3*, u8, u8, u8, u8);
void* VAlloc(int);
void VFree(void*);
void ActorUnlinkOwnerObjects(Actor*);
void ActorRelinkOwners(Actor*);
void ActorFn800231A8(Actor*);
void DeferAdd(int, Vec3*, void (*)(void*), void*);

u32 Actor62Msg(u32 id, int msg, int* data, int param);
u32 Actor62SpawnMsg(u32 id, int* data);
u32 Actor62UpdatePath(u32 id, int* data, int owner);

// Fills a 3x3+translation matrix from an actor.
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
static inline Actor* FindActor62(u32 id)
{
    for (Actor* a = lbl_804D5768; a != 0; a = a->next) {
        if ((a->id & 0xFFFFFF) == id) {
            return a;
        }
    }
    return 0;
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
    u32 id = 0;
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
        n->pos.x = -100.0f;
        n->pos.y = 0.0f;
        n->pos.z = -100.0f;
        n->size.x = 100.0f;
        n->size.y = lbl_804D5760;
        n->size.z = 100.0f;
        n->field_70 = (u8*)&n->field_80;
        n->field_74 = 3;
        AssetFind(0xF2E2BEB8, 0xF6298F9);
        fn_8000F874();
        n->next = lbl_804D5768;
        if (lbl_804D5768 != 0) {
            lbl_804D5768->prev = n;
        }
        id = n->id;
        lbl_804D5768 = n;
    }
    return id;
}

// Message handler for the type-0x62 actors: find, query, create and dispatch.
u32 Actor62Msg(u32 id, int msg, int* data, int param)
{
    id &= 0xFFFFFF;
    Actor* obj = FindActor62(id);
    data[0] = (int)obj;
    u32 agent = id | 0x62000000;

    if (msg == 0x413) {
        return Actor62SetupTransform(agent, data);
    }

    if (msg < 0x413) {
        if (msg == 0x400) {
            obj = FindActor62(id);
            if (obj != 0) {
                data[0] = (int)obj;
                return 1;
            }
            return 0;
        }
        if (msg < 0x400) {
            if (msg == 0) {
                obj = FindActor62(id);
                if (obj != 0) {
                    DeferAdd(1, &obj->pos, Actor62DrawPathTrail, obj);
                }
                return 0;
            }
        } else if (msg == 0x411) {
            obj = FindActor62(id);
            if (obj != 0) {
                *(ActorHdr*)(data + 4) = obj->hdr;
                return 1;
            }
            return 0;
        }
    } else {
        if (msg == 0x41D) {
            return Actor62SpawnMsg(agent, data);
        }
        if (msg < 0x41D) {
            if (msg == 0x415) {
                obj = FindActor62(id);
                if (obj == 0) {
                    return 0;
                }
                int arg = data[1];
                if (arg != -1) {
                    if (data[3] != 0) {
                        return fn_802127A0(obj->id, arg, data[2], data + 4, 1, 0, 0);
                    }
                    return fn_80212814(obj->id, arg, data[2], 1, 0, 0);
                }
                return fn_8021284C(obj->id);
            }
            if (msg < 0x415) {
                return Actor62UpdatePath(agent, data, fn_80212730(agent));
            }
        } else if (msg < 0x41F) {
            if (data[1] != 0x12D80) {
                return 0;
            }
            data[3] = (int)CreateActor62();
            return 1;
        }
    }
    return 0;
}

// Second half of the type-0x62 messages: start a copy or destroy the actor.
u32 Actor62SpawnMsg(u32 id, int* data)
{
    id &= 0xFFFFFF;
    Actor* obj = FindActor62(id);

    if (data[1] == 0x7A281983) {
        if (data[3] != 0 && obj->state == 0) {
            obj->state = 1;
            obj->field_F98 = lbl_804DA918;
            obj->field_6C = 1;
            return 1;
        }
        if (data[3] == 0 && obj->state == 1) {
            int out;
            f32 scale = lbl_804D5770;
            u32 newId = CreateActor62();
            Actor* n = FindActor62(newId);

            if (n != 0) {
                n->state = 2;
                memcpy(&n->field_80, &obj->field_80, 0x24);
                memcpy(&n->field_A4, &obj->field_A4, 0x24);
                memcpy(n->path, obj->path, 0xED0);
                n->hdr = obj->hdr;
                int v = fn_80212730(obj->id);
                fn_80212814(n->id, v, -1, 1, 0, 0);
                ActorFn800231A8(n);
                v = fn_80212730(obj->id);
                Actor62UpdatePath(n->id, &out, v);
            }
            obj->state = 0;
            memset(obj->path, 0, 0xED0);
        }
    } else if (data[1] == 0x210E6C) {
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
    } else {
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
    if (a == 0) {
        return 0;
    }
    if (a->state == 1) {
        int* src = (int*)data[1];
        a->hdr = *(ActorHdr*)src;

        Mtx34 mtx;
        ActorGetMatrix(&mtx, a);

        a->field_80 = mtx.t;
        fn_800A025C(&a->field_B0, &mtx, 1);
        f32 scale = lbl_804D5760 * lbl_804DA91C;
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

// ---------------------------------------------------------------------------
// Per-frame path update of a type-0x62 actor: advances its 158 path segments,
// rebuilds the world-space curve from 32 Bezier samples, scatters the trail
// effects and removes the actor once its path is used up. Returns 0 when the
// actor comes back out of the list, 1 otherwise.
// ---------------------------------------------------------------------------

// The 0x48-byte snapshot block that fn_800201E4 hands back through its hook.
struct Actor62State {
    u32 w[0x12];
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
int   fn_800201E4(int handle, Vec3* start, int flags,
                  u32 (*copy)(Actor62State*, Actor62State*), PathQuery* out);
void  fn_8009F7A8(void* out, Vec3* dir);
f32   fn_800E1020(Vec3* v);
void  fn_800E1098(Vec3* v, f32 amount);
void  fn_80122108(int, int, Vec3*, u32, f32, f32, f32, f32, f32, f32, f32);
void  SendMsg(int to, int msg, void* data);
int   rand(void);
float fabsf(float);
float sqrtf(float);
}

void ViewBezierPoint(double t, int* curve, Vec3* out);

u32 Actor62UpdatePath(u32 id, int* data, int owner)
{
    id &= 0xFFFFFF;
    Actor* a = FindActor62(id);
    if (a == 0) {
        return 0;
    }

    int handle = 0;
    SendMsg(owner, 0x400, &handle);

    f32 dt = *(f32*)(data + 1);
    lbl_804D5770 = dt;

    a->field_8C.x = a->field_8C.x * lbl_804DA920 + a->field_B0.x * lbl_804DA924;
    a->field_8C.y = a->field_8C.y * lbl_804DA920 + a->field_B0.y * lbl_804DA924;
    a->field_8C.z = a->field_8C.z * lbl_804DA920 + a->field_B0.z * lbl_804DA924;
    a->field_98.x = a->field_98.x * lbl_804DA928 + a->field_BC.x * lbl_804DA92C;
    a->field_98.y = a->field_98.y * lbl_804DA928 + a->field_BC.y * lbl_804DA92C;
    a->field_98.z = a->field_98.z * lbl_804DA928 + a->field_BC.z * lbl_804DA92C;

    if (a->state == 1) {
        f32 slip = lbl_804D5760 / lbl_804D5764;
        f32 rate = lbl_804DA930 / lbl_804D5764;
        f32 rem;
        int n;
        int i;

        a->field_F98 += dt;
        n = (int)(a->field_F98 * rate);
        rem = (f32)n / rate;
        a->field_F98 -= rem;
        i = 0;
        while (i < 0x9E && n != 0) {
            if (a->path[i].f == lbl_804DA918) {
                n--;
                a->path[i].f = lbl_804D5760 - slip * (rem / (f32)n);
                a->path[i].g = (f32)(rand() % 360);
                a->path[i].h = lbl_804DA934 - (f32)(rand() % 50 - 25) / lbl_804DA938;
            }
            i++;
        }
    }

    int moved = 0;
    for (int i = 0; i < 0x4F; i++) {
        f32 v;

        v = a->path[i * 2].f - dt * (lbl_804D5760 / lbl_804D5764);
        if (v > lbl_804DA918) {
            a->path[i * 2].f = v;
        } else {
            a->path[i * 2].f = lbl_804DA918;
        }
        if (a->path[i * 2].f > lbl_804DA918) {
            moved = 1;
        }

        v = a->path[i * 2 + 1].f - dt * (lbl_804D5760 / lbl_804D5764);
        if (v > lbl_804DA918) {
            a->path[i * 2 + 1].f = v;
        } else {
            a->path[i * 2 + 1].f = lbl_804DA918;
        }
        if (a->path[i * 2 + 1].f > lbl_804DA918) {
            moved = 1;
        }
    }

    PathPoint pts[32];
    pts[0].p.x = a->field_80.x;
    pts[0].p.y = a->field_80.y;
    pts[0].p.z = a->field_80.z;
    pts[0].w = lbl_804DA918;

    f32 step = lbl_804DA93C;
    f32 t = step;
    for (int i = 1; i < 32; i++) {
        Vec3 d;

        ViewBezierPoint(t, (int*)a->field_70, &pts[i].p);
        d.x = pts[i].p.x - pts[i - 1].p.x;
        d.y = pts[i].p.y - pts[i - 1].p.y;
        d.z = pts[i].p.z - pts[i - 1].p.z;
        pts[i].w = pts[i - 1].w + fn_800E1020(&d);
        t += step;
    }

    Vec3 origin;
    origin.x = pts[0].p.x;
    origin.y = pts[0].p.y;
    origin.z = pts[0].p.z;

    PathQuery q;
    if (fn_800201E4(handle, &origin, 0x1000, Actor62CopyState, &q) != 0) {
        Mtx34 basis;
        Vec3 dir;
        Vec3 delta;
        f32 range;

        dir.x = (f32)q.dirX;
        dir.y = (f32)q.dirY;
        dir.z = (f32)q.dirZ;
        if (fabsf(dir.x) <= lbl_804DA940 && fabsf(dir.y) <= lbl_804DA940 &&
            fabsf(dir.z) <= lbl_804DA940) {
            dir.x = lbl_804DA918;
            dir.y = lbl_804DA918;
            dir.z = lbl_804DA918;
        } else {
            f32 inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

            dir.x *= inv;
            dir.y *= inv;
            dir.z *= inv;
        }
        fn_8009F7A8(&basis, &dir);

        delta.x = (f32)q.startX - pts[0].p.x;
        delta.y = (f32)q.startY - pts[0].p.y;
        delta.z = (f32)q.startZ - pts[0].p.z;
        range = fn_800E1020(&delta) - lbl_804DA944;
        if (range < lbl_804DA918) {
            range = lbl_804DA918;
        }
        if (range == lbl_804DA918) {
            for (int i = 1; i < 32; i++) {
                pts[i].p = pts[0].p;
            }
        } else {
            for (int i = 1; i < 32; i++) {
                if (pts[i].w >= range) {
                    pts[i].w = lbl_804DA918;
                }
            }
        }
    }

    for (int i = 0; i < 0x9E; i++) {
        f32 f = a->path[i].f;

        if (f > lbl_804DA918) {
            f32 want = lbl_804D5760 - f;
            for (int j = 1; j < 32; j++) {
                if (pts[j].w >= want) {
                    Vec3 d;

                    d.x = pts[j].p.x - pts[j - 1].p.x;
                    d.y = pts[j].p.y - pts[j - 1].p.y;
                    d.z = pts[j].p.z - pts[j - 1].p.z;
                    fn_800E1098(&d, want - pts[j].w);
                    a->path[i].pos.x = d.x + pts[j - 1].p.x;
                    a->path[i].pos.y = d.y + pts[j - 1].p.y;
                    a->path[i].pos.z = d.z + pts[j - 1].p.z;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < 0x9E; i++) {
        if (a->path[i].f > lbl_804DA918 && i % 40 == 0) {
            f32 k = lbl_804DA934 - a->path[i].f / lbl_804D5760;
            f32 s = lbl_804DA948 * (lbl_804DA950 * k + lbl_804DA94C);

            if (fn_802A1D04() == 0) {
                fn_80122108(handle, owner, &a->path[i].pos, 0xC8769B80,
                            lbl_804DA934, s, lbl_804DA954, lbl_804DA954,
                            lbl_804DA948, lbl_804DA958, lbl_804DA934);
            } else {
                fn_80122108(handle, owner, &a->path[i].pos, 0xC8769B80,
                            lbl_804DA934, s, lbl_804DA95C, lbl_804DA95C,
                            lbl_804DA948, lbl_804DA958, lbl_804DA934);
            }
        }
    }

    ActorRelinkOwners(a);
    if (moved == 0 && a->state == 2) {
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
        return 0;
    }
    return 1;
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
    if (p != 0) {
        Actor* a = (Actor*)p;
        f32 cutoff = lbl_804DA918;
        int i = 0;
        PathSeg* seg = a->path;

        Draw3DBegin((int)AssetFind(0xF2E2BEB8, 0xF6298F9), 0);
        do {
            if (seg->f > cutoff) {
                f32 k = lbl_804DA934 - seg->f / lbl_804D5760;
                f32 alpha = lbl_804DA968 * (lbl_804DA934 - k);

                DrawWorldQuadFlipped((lbl_804DA950 * k + lbl_804DA94C) * seg->h,
                                     seg->g, &seg->pos, 0xff, 0xff, 0xff,
                                     (int)alpha & 0xff);
            }
            i++;
            seg++;
        } while (i < 0x9E);
        Draw3DEnd();
    }
}

// Reset the type-0x62 path scales to their default values.
void Actor62InitPathScale(void)
{
    lbl_804D5760 = lbl_804D3890;
    lbl_804D5764 = lbl_804D389C;
}

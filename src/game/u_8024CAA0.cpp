// u_8024CAA0.cpp: camera-rail and scannable-beam helpers for an actor and its owner (0x8024CAA0-0x8024E374).
// Names of structures and functions are descriptive guesses.

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

// Returned by fn_800C6448; only its position (at 0x24) is used here.
struct UnkCam {
    u8 pad00[0x24];
    Vec3 pos;   // 0x24
};

// Player object returned by GetPlayer.
struct Player {
    u8 pad00[0x24];
    s32 px, py, pz;     // 0x24
    u8 pad30[0x08];     // 0x30
    u32 objId;          // 0x38
    u8 pad3c[0x4c];     // 0x3c
    u32* f88;           // 0x88
};

struct Obj44;
struct ActorHandle;

// Owner object passed in r3.
struct CamObj {
    u8 pad00[0x04];
    s32 id;             // 0x04
    s32 id2;            // 0x08
    float fc;           // 0x0c
    float f10;          // 0x10
    u8 pad14[0x04];     // 0x14
    float f18;          // 0x18
    u32 i1c;            // 0x1c
    u8 pad20[0x0c];     // 0x20
    void* f2c;          // 0x2c
    u8 pad30[0x14];     // 0x30
    Obj44* f44;         // 0x44
};

// Actor passed in r4.
struct CamActor {
    u8 pad00[0x24];
    s32 px, py, pz;     // 0x24
    u8 pad30[0x08];     // 0x30
    u32 objId;          // 0x38
    u8 pad3c[0x14];     // 0x3c
    u8 f50[0x18];       // 0x50
    Vec3 end;           // 0x68
    float f74;          // 0x74
    float f78;          // 0x78
    float f7c;          // 0x7c
    Vec3 rot;           // 0x80
    u8 pad8c[0x28];     // 0x8c
    Vec3 line;          // 0xb4
    u16 flags;          // 0xc0
    u16 padc2;          // 0xc2
    float fc4;          // 0xc4
    s32 fc8;            // 0xc8
    u32 unkcc;          // 0xcc
    u8 padd0[0x04];     // 0xd0
    Vec3 vel;           // 0xd4
    u8 pade0[0x04];     // 0xe0
    Player* owner;      // 0xe4
    float fe8;          // 0xe8
    u8 padec[0x04];     // 0xec
    ActorHandle* ff0;   // 0xf0
    u8 padf4[0x04];     // 0xf4
    float ff8;          // 0xf8
};

// Message 0x407 payload, zero-filled then filled in.
struct CamMsg407 {
    Player* player;     // 0x00
    CamActor* actor;    // 0x04
    u32 unk08;          // 0x08
    float speed;        // 0x0c
    u32 unk10;          // 0x10
    u32 unk14;          // 0x14
    s32 x, y, z;        // 0x18
    s32 dx, dy, dz;     // 0x24
    s32 rx, ry, rz;     // 0x30
    s32 m3c, m40, m44;  // 0x3c
    u32 zero48;         // 0x48
    float f4c;          // 0x4c
    u32 z50, z54, z58;  // 0x50
};

extern "C" UnkCam* fn_800C6448(void);
extern "C" void fn_8016D440(Vec3* out, const Vec3* in);
extern "C" void fn_8016D614(Vec3* out, const Vec3* a, const Vec3* b);
extern "C" void fn_8016D4EC(Vec3* out, const Vec3* in);
extern "C" float fn_800E1020(Vec3* v);
extern "C" void fn_800E1098(Vec3* dst, const Vec3* src, float scale);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void* memset(void* dst, int val, u32 n);
extern "C" float fabsf(float f);

float DistPointLineSq(const float* p, const float* a, const float* b, float* t,
                      float* out);
void EulerNormalize(Vec3* a, Vec3* b);
Player* GetPlayer();

extern float lbl_804D9C90;
extern float lbl_804D9C94;
extern float lbl_804D9C98;
extern float lbl_804D9C9C;
extern float lbl_804D9CA0;
extern s32 lbl_804D35C8;
extern s32 lbl_804D35CC;

// Advances an actor's aim and rotation toward a camera and, when the actor is
// close enough, reports contact through message 0x407. 0x8024CAA0.
bool CamRailUpdate(float scale, CamObj* obj, CamActor* actor) {
    UnkCam* cam = fn_800C6448();
    Vec3 delta;
    Vec3 dir;
    Vec3 aim;
    Vec3 mat;
    Vec3 rot;

    delta.x = cam->pos.x - (float)actor->px;
    delta.y = cam->pos.y - (float)actor->py;
    delta.z = cam->pos.z - (float)actor->pz;
    fn_8016D440(&dir, &delta);
    fn_8016D440(&aim, &actor->rot);
    fn_8016D614(&dir, &dir, &aim);
    EulerNormalize(&dir, &dir);
    if (fabsf(dir.z) > lbl_804D9C90) {
        dir.z = lbl_804D9C94;
    }

    float denom = fabsf(dir.z) + fabsf(dir.x);
    float ky = (lbl_804D9C98 * ((float)lbl_804D35C8 * scale)) / denom;
    if (ky < lbl_804D9C94) {
        ky = lbl_804D9C94;
    } else if (ky > lbl_804D9C9C) {
        ky = lbl_804D9C9C;
    }
    float kx = (lbl_804D9C98 * ((float)lbl_804D35CC * scale)) / denom;
    if (kx < lbl_804D9C94) {
        kx = lbl_804D9C94;
    } else if (kx > lbl_804D9C9C) {
        kx = lbl_804D9C9C;
    }
    aim.z = aim.z + dir.z * ky;
    aim.x = aim.x + dir.x * kx;
    fn_8016D4EC(&mat, &aim);
    fn_800E1098(&actor->rot, &mat, fn_800E1020(&actor->rot));
    fn_8016D440(&rot, &actor->rot);
    actor->f7c = rot.z;

    cam = fn_800C6448();
    delta.x = actor->end.x - actor->line.x;
    delta.y = actor->end.y - actor->line.y;
    delta.z = actor->end.z - actor->line.z;
    float t;
    float dist = DistPointLineSq(&cam->pos.x, &actor->line.x, &delta.x, &t, 0);
    float px, py, pz;
    if (t < lbl_804D9C94) {
        px = actor->line.x;
        py = actor->line.y;
        pz = actor->line.z;
        delta.x = px - cam->pos.x;
        delta.y = py - cam->pos.y;
        delta.z = pz - cam->pos.z;
        dist = px * px + py * py + pz * pz;
    } else if (t > lbl_804D9C9C) {
        px = actor->end.x;
        py = actor->end.y;
        pz = actor->end.z;
        delta.x = px - cam->pos.x;
        delta.y = py - cam->pos.y;
        delta.z = pz - cam->pos.z;
        dist = px * px + py * py + pz * pz;
    }
    if (dist < lbl_804D9CA0) {
        Player* player = GetPlayer();
        CamMsg407 msg;
        memset(&msg, 0, 0x5c);
        msg.actor = actor;
        msg.unk08 = actor->unkcc;
        msg.unk14 = obj->i1c;
        msg.speed = (float)obj->id * actor->fe8 * obj->fc;
        msg.f4c = obj->f18;
        msg.rx = (s32)actor->rot.x;
        msg.ry = (s32)actor->rot.y;
        msg.x = (s32)px;
        msg.y = (s32)py;
        msg.z = (s32)pz;
        msg.rz = (s32)actor->rot.z;
        msg.dx = msg.x - player->px;
        msg.dy = msg.y - player->py;
        msg.zero48 = 0;
        msg.dz = msg.z - player->pz;
        msg.m3c = -1;
        msg.m40 = -1;
        msg.m44 = -1;
        msg.player = player;
        msg.z58 = 0;
        SendMsg(player->objId, 0x407, &msg);
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Helpers for the rail-move update (0x8024CF24) and its debug draw (0x8024D8AC).

struct Actor;           // shared game class, defined in another unit

struct Mtx34 {
    u32 m[9];           // 0x00, 0x24 bytes
};

struct Vec3i {
    s32 x, y, z;
};

// Fire-loop handle allocated by RailFireloopStart, released by RailFireloopDestroy.
struct ActorHandle {
    void* obj;          // 0x00
    s32 emitterA;       // 0x04
    s32 emitterB;       // 0x08
    s32 id;             // 0x0c
};

// Payload of message 0x412: an asset handle going in, a position coming back.
struct Msg412 {
    u32 unk00;          // 0x00
    u32 asset;          // 0x04
    u8 pad08[0x2c];     // 0x08
    s32 x, y, z;        // 0x34
};

// An actor found by the rail query. Only two members are touched here.
struct Target {
    u8 pad00[0x30];
    int (*fn30)(int msg, void* data);   // 0x30
    u8 pad34[0x04];                     // 0x34
    u32 objId;                          // 0x38
};

// Result filled in by fn_8001F928 or by the owner's rail callback.
struct QueryResult {
    Target* found;      // 0x00
    u8 pad04[0x08];     // 0x04
    s32 a;              // 0x0c
    void* b;            // 0x10
    s32 c;              // 0x14
    s32 d;              // 0x18
};

struct QueryCtx {
    QueryResult* out;   // 0x00
    u32 type;           // 0x04
};

// Ray handed to fn_8001F928; the first two vectors are the actor's rail.
struct CamQuery {
    s32 lineX, lineY, lineZ;    // 0x00
    s32 endX, endY, endZ;       // 0x0c
    u8 pad18[0x40];             // 0x18
};

// Point/delta pair returned by fn_8016F120.
struct CamLine {
    s32 x, y, z;        // 0x00
    s32 dx, dy, dz;     // 0x0c
};

// Owner-side rail callback object reachable from CamObj::f44.
struct Obj44 {
    u8 pad00[0x0c];
    int (*fn0c)(CamObj* obj, CamActor* actor, QueryResult* res);  // 0x0c
    void* field10;                                                // 0x10
};

u32 AssetFind(u32 id, u32 mask);
void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void Draw3DBeginLine(int mode, void* p, float f);
void DrawLine(float* a, float* b, u8 c0, u8 c1, u8 c2, u8 c3, u8 c4, u8 c5,
              u8 c6, u8 c7);
void DrawDisableTexOffsets();
void HazardCreateSpline(Actor* actor, int type, Vec3* pos, int id, float f1,
                        float f2, float f3, float f4, float f5, float f6,
                        float f7);

extern "C" int rand(void);
extern "C" int fn_8001F928(CamActor* actor, CamQuery* q, u32 flags, void* cb,
                           QueryCtx* ctx);
extern "C" int fn_80049940(void);
extern "C" void fn_8003F15C(float f, s32 id, Mtx34* mtx, int a, int b, int c);
extern "C" void fn_8009F44C(Vec3i* out, CamActor* actor, Vec3i* in);
extern "C" void fn_800A0100(Mtx34* mtx);
extern "C" float fn_800A127C(float f);
extern "C" float fn_800A1394(float f);
extern "C" float fn_800A1418(float f);
extern "C" void fn_80158C3C(void* v, u32 id, int a, int b, int c, int d, int e);
extern "C" void fn_8016D648(Vec3* out, const Vec3* a, const Vec3* b);
extern "C" void fn_8016D6A8(Vec3* v);
extern "C" void fn_8016E13C(void* m, Vec3i* v, int flag);
extern "C" void fn_8016EDD8(s32 id, s32* pos);
extern "C" int fn_8016F120(void* obj, CamLine* out);
extern "C" void fn_80027690(void* p);
extern "C" void fn_8016EEF4(s32 id);
void* VAlloc(int size);
void VFree(void* p);
void EmitterDestroy(int emitter);
void* FxFind(const char* name);
extern "C" void fn_80184194(void* obj, void* pos, u32 id);
extern "C" void fn_80289CEC(CamLine* line, s32* delta);
extern "C" void fn_802C28EC(CamLine* line, s32* delta, int msg, CamActor* actor);
extern "C" void fn_802C2AEC(CamLine* line, s32* delta, u8* color, u32 g,
                            int a, int b, int c, float f1, float f2, float f3,
                            float f4);

extern float lbl_804D9CB0;
extern float lbl_804D9CB4;
extern float lbl_804D9CB8;
extern float lbl_804D9CBC;
extern float lbl_804D9CC0;
extern float lbl_804D9CC4;
extern float lbl_804D9CC8;
extern float lbl_804D9CCC;
extern float lbl_804D9CD0;
extern float lbl_804D9CD4;
extern float lbl_804D9CD8;
extern float lbl_804D9CDC;
extern float lbl_804D9CE0;
extern float lbl_804D9CE4;
extern float lbl_804D9CE8;
extern float lbl_804D9CEC;
extern float lbl_804D9CF0;
extern float lbl_804D9CF4;
extern u32 lbl_804D3044;

// Per-frame move along the actor's rail: aim the actor at either its owner or
// its own target, then either report a contact (message 0x407) or create a
// hazard spline. 0x8024CF24.
bool ScannableRailMove(float scale, CamObj* obj, CamActor* actor) {
    Vec3 euler;
    Vec3 delta;
    Vec3 dir;
    Vec3 aim;
    Vec3 nrm;
    Vec3 out;
    Vec3 v38;
    Vec3 v20;
    Vec3 v2c;
    Vec3i rel;
    CamQuery query;
    CamMsg407 msg;
    QueryResult res;
    QueryCtx ctx;
    CamLine line;
    u8 color[4];

    ctx.out = &res;
    ctx.type = actor->unkcc;

    query.lineX = (s32)actor->line.x;
    query.lineY = (s32)actor->line.y;
    query.lineZ = (s32)actor->line.z;
    query.endX = (s32)actor->end.x;
    query.endY = (s32)actor->end.y;
    query.endZ = (s32)actor->end.z;

    if (actor->ff0->id != -1) {
        fn_8016EDD8(actor->ff0->id, &query.endX);
    }

    fn_8016D440(&euler, &actor->rot);
    actor->f7c = euler.z;

    if (actor->owner) {
        Player* p = actor->owner;
        Msg412 buf;
        float f = lbl_804D9CB0;

        buf.asset = AssetFind(0xf1754bf2, 0x21c5e0);
        s32 ok = SendMsg(p->objId, 0x412, &buf);
        s32 rx, ry, rz;
        if (ok) {
            rx = buf.x;
            ry = buf.y;
            rz = buf.z;
        } else {
            rx = p->px;
            ry = p->py;
            rz = p->pz;
        }

        fn_8016D6A8(&nrm);
        bool flag = false;
        if ((p->objId >> 24) == 3) {
            if (SendMsg(0x02000000, 0x412, &buf)) {
                delta.x = (float)(buf.x - actor->px);
                delta.y = (float)(buf.y - actor->py);
                delta.z = (float)(buf.z - actor->pz);
                fn_8016D440(&nrm, &delta);
                flag = true;
                f = f * lbl_804D9CB4;
            }
        }

        delta.x = (float)(rx - actor->px);
        delta.y = (float)(ry - actor->py);
        delta.z = (float)(rz - actor->pz);
        fn_8016D440(&dir, &delta);
        if (flag) {
            dir.x = nrm.x;
        }
        fn_8016D440(&aim, &actor->rot);
        fn_8016D614(&dir, &dir, &aim);
        EulerNormalize(&dir, &dir);
        if (fabsf(dir.z) > lbl_804D9C90 ||
            (flag && actor->fc4 > lbl_804D9CB8)) {
            dir.z = lbl_804D9C94;
        }
        float k = (lbl_804D9C98 * (f * scale)) /
                  (fabsf(dir.z) + fabsf(dir.x));
        if (k < lbl_804D9C94) {
            k = lbl_804D9C94;
        } else if (k > lbl_804D9C9C) {
            k = lbl_804D9C9C;
        }
        aim.z = dir.z * k + aim.z;
        aim.x = dir.x * k + aim.x;
        fn_8016D4EC(&out, &aim);
        fn_800E1098(&actor->rot, &out, fn_800E1020(&actor->rot));
    }

    rel.x = (s32)actor->vel.x - actor->px;
    rel.y = (s32)actor->vel.y - actor->py;
    rel.z = (s32)actor->vel.z - actor->pz;
    fn_8009F44C(&rel, actor, &rel);
    fn_8016E13C(actor->f50, &rel, 1);

    u32 flags = 0x1201a000;
    if (actor->flags & 0x10) {
        flags |= 0x80000000;
    }
    void* cb = obj->f44->field10;
    if (!fn_8001F928(actor, &query, flags, cb, &ctx)) {
        return true;
    }
    if (obj->f44 && obj->f44->fn0c) {
        return obj->f44->fn0c(obj, actor, &res) == 0;
    }

    fn_8016F120(obj->f2c, &line);

    bool handled = false;
    if (res.found) {
        Target* t = res.found;
        float speed;
        if (fn_80049940()) {
            speed = (float)obj->id2 * actor->fe8;
            if ((t->objId >> 24) == 2) {
                speed = speed * obj->f10;
            }
        } else {
            speed = (float)obj->id * actor->fe8;
            if ((t->objId >> 24) == 2) {
                speed = speed * obj->fc;
            }
        }
        msg.player = (Player*)t;
        msg.actor = actor;
        msg.unk08 = actor->unkcc;
        msg.speed = speed;
        msg.unk14 = obj->i1c;
        msg.x = line.x;
        msg.y = line.y;
        msg.z = line.z;
        msg.dx = line.dx;
        msg.dy = line.dy;
        msg.dz = line.dz;
        msg.rx = (s32)actor->rot.x;
        msg.ry = (s32)actor->rot.y;
        msg.rz = (s32)actor->rot.z;
        msg.m3c = (s32)res.b;
        msg.m40 = res.d;
        msg.m44 = res.c;
        msg.zero48 = res.a;
        msg.f4c = obj->f18;
        msg.z58 = 0;
        t->fn30(0x407, &msg);
        handled = msg.z58 != 0;
    }

    if (!handled) {
        if ((actor->unkcc >> 24) == 2) {
            Vec3 sp;
            sp.x = (float)line.x;
            sp.y = (float)line.y;
            sp.z = (float)line.z;
            fn_80158C3C(&sp, 0x140beb1e, -1, -1, -1, -1, -1);
            HazardCreateSpline((Actor*)actor, (int)actor->unkcc, &sp, 0x20ee36,
                               lbl_804D9CC0, lbl_804D9CDC, lbl_804D9CB8,
                               lbl_804D9CB8, lbl_804D9CE0, lbl_804D9CE4,
                               lbl_804D9CE8);
        }
        return false;
    }

    if ((actor->unkcc >> 24) != 2 || fn_80049940() != 0) {
        fn_802C28EC(&line, &line.dx, 0x86, actor);
        return false;
    }

    actor->fc4 = lbl_804D9C94;
    actor->flags |= 0x10;
    fn_80289CEC(&line, &line.dx);
    color[0] = 0xff;
    color[1] = 0xff;
    color[2] = 0xff;
    color[3] = 0xff;
    fn_802C2AEC(&line, &line.dx, color, lbl_804D3044, 0xc8, 0, 1,
                lbl_804D9CB0, lbl_804D9CBC, lbl_804D9CC0, lbl_804D9CC4);
    fn_80184194(res.b, &line, 0xe5b84c95);

    float t = (float)rand() / lbl_804D9CC8;
    s32 r = rand();
    float ang = (float)(r % 360);
    float rad = fn_800A127C(lbl_804D9CD0 * t + lbl_804D9CCC);
    float sc = lbl_804D9CC4 * fn_800E1020(&actor->rot);
    v38.z = rad * fn_800A1418(ang);
    v38.x = rad * fn_800A1394(ang);
    v38.y = lbl_804D9C9C;
    fn_8016D440(&v20, &v38);
    v38.x = -actor->rot.x;
    v38.y = -actor->rot.y;
    v38.z = -actor->rot.z;
    fn_8016D440(&v2c, &v38);
    fn_8016D648(&v2c, &v2c, &v20);
    fn_8016D4EC(&actor->rot, &v2c);
    fn_800E1098(&actor->rot, &actor->rot, lbl_804D9CD4 * sc);
    actor->f74 = v2c.x;
    actor->f78 = v2c.y;
    actor->f7c = v2c.z;
    actor->vel.x = (float)line.x;
    actor->vel.y = (float)line.y;
    actor->vel.z = (float)line.z;
    fn_800E1098(&actor->end, &actor->rot, lbl_804D9CD8);
    actor->end.x += actor->vel.x;
    actor->end.y += actor->vel.y;
    actor->end.z += actor->vel.z;
    return true;
}

// Debug draw of the actor's rail: draws the current rail segment with a fade
// based on the actor's timer. 0x8024D8AC.
void ScannableDrawRail(int mode, CamActor* actor) {
    Mtx34 mtx;
    Vec3 pos;
    float f;

    ActorGetMatrix(&mtx, (Actor*)actor);
    fn_800A0100(&mtx);

    if (actor->fc8 != -1) {
        if ((actor->flags & 0x10) == 0) {
            f = lbl_804D9C9C - actor->fc4 / lbl_804D9CE8;
        } else {
            f = lbl_804D9C9C - actor->fc4 / lbl_804D9CEC;
        }
        if (f < lbl_804D9C94) {
            f = lbl_804D9C94;
        } else if (f > lbl_804D9C9C) {
            f = lbl_804D9C9C;
        }
        if ((actor->flags & 8) == 0) {
            ActorGetMatrix(&mtx, (Actor*)actor);
            fn_8003F15C(lbl_804D9C9C, actor->fc8, &mtx, 1, 0, 0);
        }
        if (f > lbl_804D9C94) {
            Draw3DBeginLine(-4, 0, lbl_804D9CC4);
            DrawLine(&actor->vel.x, &pos.x, 0x14, 0x3c, 100,
                     (u8)(s32)(lbl_804D9CF4 * f), 0x14, 0x3c, 100,
                     (u8)(s32)(lbl_804D9CF0 * f));
            DrawDisableTexOffsets();
            actor->ff8 = lbl_804D9C94;
        }
    }
}

// Releases the fire-loop handle kept at 0xf0: its owner object, its two emitters
// and the effect itself are destroyed, then the handle buffer is freed. 0x8024DA40.
void RailFireloopDestroy(int owner, CamActor* actor) {
    ActorHandle* h = actor->ff0;
    if (h) {
        fn_80027690(h->obj);
        EmitterDestroy(h->emitterA);
        EmitterDestroy(h->emitterB);
        fn_8016EEF4(h->id);
        VFree(h);
        actor->ff0 = 0;
    }
}

// Payload of message 0x41e: the actor's id, a request hash and a value the
// receiver fills in.
struct RailMsg41E {
    u32 unk00;          // 0x00
    u32 hash;           // 0x04
    u32 unk08;          // 0x08
    u32 value;          // 0x0c
};

// Starts the boss fire-loop: allocates the 0x10-byte handle, asks the actor for
// its 0x41e parameter, scales the actor's rotation by it and spawns the
// "demonboss_fireloop" effect at the actor's position. 0x8024DAB0.
int RailFireloopStart(int owner, CamActor* actor) {
    ActorHandle* h = (ActorHandle*)VAlloc(0x10);
    actor->ff0 = h;
    if (!h) {
        return 0;
    }
    u32 id = actor->unkcc;
    if ((id >> 24) != 0x13) {
        return 0;
    }
    RailMsg41E msg;
    msg.value = 0;
    msg.hash = 0x607eed34;
    msg.unk08 = 0;
    if (SendMsg(id, 0x41e, &msg)) {
        fn_800E1098(&actor->rot, &actor->rot, (float)msg.value);
    }
    CamLine line;
    line.x = (s32)actor->rot.x;
    line.y = (s32)actor->rot.y;
    line.z = (s32)actor->rot.z;
    h->id = fn_8016F120(FxFind("demonboss_fireloop"), &line);
    return 1;
}

// Flat parameter block consumed by EmitterCreate. Only the fields this unit
// writes are named; 0x31..0x6b is never initialised and stays padding.
struct EmitterDesc {
    float px, py, pz;               // 0x00
    float f0c, f10, f14;            // 0x0c
    float f18;                      // 0x18
    s32 i1c;                        // 0x1c
    s32 i20;                        // 0x20
    float f24;                      // 0x24
    s32 i28;                        // 0x28
    u32 asset;                      // 0x2c
    u8 alpha;                       // 0x30
    u8 pad31[0x3b];                 // 0x31
    float f6c, f70, f74, f78;       // 0x6c
    s32 i7c;                        // 0x7c
    float f80, f84;                 // 0x80
    s32 i88;                        // 0x88
    float f8c, f90, f94;            // 0x8c
    s32 i98, i9c, ia0, ia4, ia8;    // 0x98
    float fac, fb0, fb4, fb8;       // 0xac
    float fbc, fc0, fc4, fc8;       // 0xbc
    u8 color0[4];                   // 0xcc
    u8 color1[4];                   // 0xd0
    s32 id4, id8;                   // 0xd4
    s32 i_dc, i_e0, i_e4;           // 0xdc
};

// Payload sent with message 0x415 to the fire-loop effect and its emitters.
struct RailFxMsg {
    u32 unk00;          // 0x00
    u32 objId;          // 0x04
    s32 index;          // 0x08
    u8 pad0c[0x34];     // 0x0c
};

extern float lbl_804D9CF8;
extern float lbl_804D9CFC;
extern float lbl_804D9D00;
extern float lbl_804D9D04;
extern float lbl_804D9D08;
extern float lbl_804D9D0C;
extern float lbl_804D9D10;
extern float lbl_804D9D14;
extern float lbl_804D9D18;
extern float lbl_804D9D1C;
extern float lbl_804D9D20;
extern float lbl_804D9D24;
extern float lbl_804D9D28;
extern float lbl_804D9D2C;
extern float lbl_804D9D30;
extern float lbl_804D9D34;
extern float lbl_804D9D38;
extern float lbl_804D9D3C;
extern float lbl_804D9D40;
extern float lbl_804D9D44;
extern float lbl_804D9D48;
extern u8 lbl_803BEB48[];

extern "C" void* fn_80027A8C(float scale, int mode, Vec3* a, Vec3* b,
                             void* data, int flags, int count);
extern "C" void fn_800E0E9C(Vec3i* out, Vec3i* in, int amount);
s32 EmitterCreate(EmitterDesc* desc);
void EmitterSetEnabled(u32 emitter, int enabled);
void ActorAim(int* out, Actor* actor, int mode);

// Starts the scannable rail's fire-loop effect: the remote effect first, then
// the two local emitters placed at the actor's aim points. 0x8024DBF8.
int ScannableRailEffectsStart(int owner, CamActor* actor) {
    ActorHandle* h = (ActorHandle*)VAlloc(0x10);
    actor->ff0 = h;
    if (!h) {
        return 0;
    }
    h->id = -1;

    Player* p = actor->owner;
    if (p && (p->objId >> 24) == 1 && p->f88 && *p->f88 == 0x201bae) {
        actor->owner = 0;
    }

    RailFxMsg msg;
    memset(&msg, 0, sizeof(msg));
    msg.objId = actor->objId;
    msg.index = -1;

    h->obj = fn_80027A8C(lbl_804D9CF8, 1, &actor->vel, &actor->rot,
                         lbl_803BEB48, 0x9c4, 9);
    SendMsg((int)h->obj, 0x415, &msg);

    EmitterDesc descA;
    Vec3i aimA;
    ActorAim(&aimA.x, (Actor*)actor, 1);
    fn_800E0E9C(&aimA, &aimA, -0x3c);
    descA.px = (float)(aimA.x + actor->px);
    descA.py = (float)(aimA.y + actor->py);
    descA.pz = (float)(aimA.z + actor->pz);
    descA.f0c = lbl_804D9C94;
    descA.f10 = lbl_804D9C94;
    descA.f14 = lbl_804D9C94;
    descA.f18 = lbl_804D9C9C;
    descA.i1c = 1;
    descA.i20 = 1;
    descA.f24 = lbl_804D9C9C;
    descA.i28 = 1;
    descA.asset = AssetFind(0xf2e2beb8, 0x5f678088);
    descA.alpha = 0xff;
    descA.f6c = lbl_804D9CFC;
    descA.f70 = lbl_804D9D00;
    descA.f74 = lbl_804D9D04;
    descA.f78 = lbl_804D9D08;
    descA.i7c = 0x2f;
    descA.f80 = lbl_804D9D0C;
    descA.f84 = lbl_804D9C9C;
    descA.i88 = 0;
    descA.f8c = lbl_804D9C94;
    descA.f90 = lbl_804D9D10;
    descA.f94 = lbl_804D9D14;
    descA.i98 = 1;
    descA.i9c = 0;
    descA.ia0 = 0;
    descA.ia4 = 0;
    descA.ia8 = 0;
    descA.fac = lbl_804D9C94;
    descA.fb0 = lbl_804D9D18;
    descA.fb4 = lbl_804D9D1C;
    descA.fb8 = lbl_804D9D20;
    descA.fbc = lbl_804D9C94;
    descA.fc0 = lbl_804D9C94;
    descA.fc4 = lbl_804D9D1C;
    descA.fc8 = lbl_804D9C9C;
    descA.color0[0] = 0x00;
    descA.color0[1] = 0x59;
    descA.color0[2] = 0x3d;
    descA.color0[3] = 0xff;
    descA.color1[0] = 0xff;
    descA.color1[1] = 0xff;
    descA.color1[2] = 0xff;
    descA.color1[3] = 0xff;
    descA.id4 = -1;
    descA.id8 = -1;
    descA.i_dc = 0;
    descA.i_e0 = 0;
    descA.i_e4 = 0;
    h->emitterA = EmitterCreate(&descA);
    EmitterSetEnabled(h->emitterA, 1);
    SendMsg(h->emitterA, 0x415, &msg);

    EmitterDesc descB;
    Vec3i aimB;
    ActorAim(&aimB.x, (Actor*)actor, 1);
    fn_800E0E9C(&aimB, &aimB, -0x69);
    descB.px = (float)(aimB.x + actor->px);
    descB.py = (float)(aimB.y + actor->py);
    descB.pz = (float)(aimB.z + actor->pz);
    descB.f0c = lbl_804D9C94;
    descB.f10 = lbl_804D9C94;
    descB.f14 = lbl_804D9C94;
    descB.f18 = lbl_804D9C9C;
    descB.i1c = 1;
    descB.i20 = 1;
    descB.f24 = lbl_804D9C9C;
    descB.i28 = 1;
    descB.asset = AssetFind(0xf2e2beb8, 0x5f678088);
    descB.alpha = 0xff;
    descB.f6c = lbl_804D9D24;
    descB.f70 = lbl_804D9D28;
    descB.f74 = lbl_804D9D2C;
    descB.f78 = lbl_804D9D30;
    descB.i7c = 0x18;
    descB.f80 = lbl_804D9D34;
    descB.f84 = lbl_804D9C9C;
    descB.i88 = 8;
    descB.f8c = lbl_804D9C94;
    descB.f90 = lbl_804D9D38;
    descB.f94 = lbl_804D9D3C;
    descB.i98 = 1;
    descB.i9c = 0;
    descB.ia0 = 0;
    descB.ia4 = 0;
    descB.ia8 = 0;
    descB.fac = lbl_804D9C94;
    descB.fb0 = lbl_804D9CCC;
    descB.fb4 = lbl_804D9D40;
    descB.fb8 = lbl_804D9D44;
    descB.fbc = lbl_804D9C94;
    descB.fc0 = lbl_804D9C94;
    descB.fc4 = lbl_804D9C94;
    descB.fc8 = lbl_804D9CD4;
    descB.color0[0] = 0x00;
    descB.color0[1] = 0x81;
    descB.color0[2] = 0x83;
    descB.color0[3] = 0xff;
    descB.color1[0] = 0x00;
    descB.color1[1] = 0x81;
    descB.color1[2] = 0x83;
    descB.color1[3] = 0xbf;
    descB.id4 = -1;
    descB.id8 = -1;
    descB.i_dc = 0;
    descB.i_e0 = 0;
    descB.i_e4 = 0;
    h->emitterB = EmitterCreate(&descB);
    EmitterSetEnabled(h->emitterB, 1);
    SendMsg(h->emitterB, 0x415, &msg);
    return 1;
}

// Variant object the beam reads its per-variant id from.
struct BeamOwner {
    u8 pad00[0x170];
    u32 id;             // 0x170
};

// Beam object of the scannable rail effect. The three tunables at 0x5b8/0x5c4/
// 0x5c8 are set from message 0x41e; the rest is filled in when the effect is
// (re)started.
struct ScannableBeam {
    u8 pad00[0xc4];
    BeamOwner* owner;   // 0xc4
    u8 padc8[0x270 - 0xc8];
    u8 cond;            // 0x270
    u8 pad271[0x5b4 - 0x271];
    float f5b4;         // 0x5b4
    float f5b8;         // 0x5b8
    float f5bc;         // 0x5bc
    float f5c0;         // 0x5c0
    float f5c4;         // 0x5c4
    float f5c8;         // 0x5c8
    float f5cc;         // 0x5cc
    s32 state;          // 0x5d0
    float f5d4;         // 0x5d4
    float f5d8;         // 0x5d8
    float f5dc;         // 0x5dc
    s32 i5e0;           // 0x5e0
    float f5e4;         // 0x5e4
    s32 i5e8;           // 0x5e8
    u8 pad5ec[0x910 - 0x5ec];
    u32 f910;           // 0x910
};

// Sets one of the beam's three tunables from the message value; the hash picks
// which one. 0x8024E0D4.
int ScannableBeamSetParam(ScannableBeam* beam, RailMsg41E* msg) {
    switch ((s32)msg->hash) {
    case (s32)0xb9af66bf:
        beam->f5b8 = lbl_804D9D48 * (float)msg->value;
        return 1;
    case (s32)0xba683b26:
        beam->f5c4 = lbl_804D9D48 * (float)msg->value;
        return 1;
    case (s32)0x51979712:
        beam->f5c8 = lbl_804D9D48 * (float)msg->value;
        return 1;
    }
    return 0;
}

extern float lbl_804D9D58;
extern float lbl_804D9D5C;
extern float lbl_804D9D60;
extern float lbl_804D9D64;
float VarFindFloat(u32 set, u32 id, u32 key);
void ConditionUpdateBit(void* cond, int bit, int value);

// Initialises the scannable rail beam from variant 0x6249ec5a: when the object
// has not been given a variant id yet it resets its state and takes the id from
// its owner, then reads the seven float tunables and three fixed floats and
// flags condition 0x26 when the variant's 0x5ede8673 value is set. 0x8024E1C0.
int ScannableBeamInit(ScannableBeam* beam) {
    u32 variant = beam->f910;
    if (variant == 0) {
        beam->state = 4;
        beam->i5e0 = -1;
        beam->f5e4 = lbl_804D9D58;
        beam->i5e8 = -1;
        variant = beam->owner->id;
    }

    beam->f5b4 = lbl_804D9D48 * VarFindFloat(0x6249ec5a, variant, 0x9782a85b);
    beam->f5b8 = lbl_804D9D48 * VarFindFloat(0x6249ec5a, variant, 0xb9af66bf);
    beam->f5bc = lbl_804D9D48 * VarFindFloat(0x6249ec5a, variant, 0x03ff5cb8);
    beam->f5c0 = lbl_804D9D48 * VarFindFloat(0x6249ec5a, variant, 0x1d9b4d25);
    beam->f5c4 = lbl_804D9D48 * VarFindFloat(0x6249ec5a, variant, 0xba683b26);
    beam->f5c8 = lbl_804D9D48 * VarFindFloat(0x6249ec5a, variant, 0x51979712);
    beam->f5cc = lbl_804D9D48 * VarFindFloat(0x6249ec5a, variant, 0xc3cabadf);
    beam->f5d4 = lbl_804D9D5C;
    beam->f5dc = lbl_804D9D60;
    beam->f5d8 = lbl_804D9D64;
    if (VarFindFloat(0x6249ec5a, variant, 0x5ede8673) != lbl_804D9D58) {
        ConditionUpdateBit(&beam->cond, 0x26, 1);
    }
    return 1;
}

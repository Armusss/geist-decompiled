// u_80181644.cpp (0x80181644-0x8018424C), split from u_8017D794.
// Six behaviours sharing one set of state functions (descriptors 0x803A85B8, 0x803A85DC, 0x803A8600, 0x803A8624,
// 0x803A8648, 0x803A866C) plus the id table 0x803A8690. Own double 0x804D82F8; rodata 0x80346FE8-0x80347010,
// .bss 0x80495700, .sdata 0x804D3018/0x804D301C, sdata2 0x804D82E8-0x804D8350.
// Names are descriptive guesses; the code is matched against the retail DOL.
//
// The projectile and kind layouts are the ones of u_80185A9C.cpp (the projectile pool).

#include <dolphin/types.h>
#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

struct Actor;
struct BehKind;
struct Projectile;

// An object the sweep touched (0x48 bytes).
struct HitTarget {
    u8 pad00[0x30];
    int (*handler)(int msg, void* data);  // 0x30
    u8 pad34[4];
    u32 handle;                           // 0x38
    u8 pad3C[0x88];
    u32* type;                            // 0xC4
    u8 padC8[0x840];
    u32 unk908;                           // 0x908
    u8 pad90C[0x1DD8];
    HitTarget* host;                      // 0x26E4
};

struct SweepHit {
    HitTarget* obj;       // 0x00
    int unk04;
    int unk08;
    int surface;          // 0x0C
    int material;         // 0x10
    int unk14;            // 0x14
    int unk18;            // 0x18
    int unk1C;
    int unk20;
    int unk24;
    int unk28[2];
    IVec3 pos;            // 0x30
    IVec3 normal;         // 0x3C
};

struct SweepCtx {
    SweepHit* hit;
    int id;
};

struct BehDesc {
    u32 hash;                                                    // 0x00
    int (*start)(BehKind* kind, Projectile* p);                  // 0x04
    int (*update)(BehKind* kind, Projectile* p, float dt);       // 0x08
    int (*blocked)(BehKind* kind, Projectile* p, SweepHit* hit); // 0x0C
    int (*filter)(SweepHit* hit, SweepCtx* ctx);                 // 0x10
    void (*stop)(BehKind* kind, Projectile* p);                  // 0x14
    void (*notify)(BehKind* kind, Projectile* p);                // 0x18
    int (*msg)(int msg, void* data);                             // 0x1C
    u32 flags;                                                   // 0x20
};

struct BehKind {
    u32 row;              // 0x00
    int count;            // 0x04
    int count2;           // 0x08
    float unk0C;          // 0x0C
    float unk10;          // 0x10
    float lifetime;       // 0x14
    float unk18;          // 0x18
    u32 hash1C;           // 0x1C
    float unk20;          // 0x20
    int model;            // 0x24
    int fx;               // 0x28
    int hitFx;            // 0x2C
    int unk30;            // 0x30
    float unk34;          // 0x34
    float unk38;          // 0x38
    float unk3C;          // 0x3C
    float unk40;          // 0x40
    BehDesc* desc;        // 0x44
};

struct ProjSpawn {
    Vec3 pos;             // 0x00
    Vec3 rot;             // 0x0C
    Vec3 dir;             // 0x18
    Vec3 vel;             // 0x24
    Vec3 unk30;           // 0x30
    int unk3C;            // 0x3C
    Vec3 unk40;           // 0x40
};

struct Projectile {
    Placement place;      // 0x00
    u8 pad30[0x8];
    u32 handle;           // 0x38
    u8 pad3C[0x10];
    int flags;            // 0x4C
    u8 shape[0x18];       // 0x50
    ProjSpawn spawn;      // 0x68
    Vec3 prev;            // 0xB4
    u16 state;            // 0xC0
    u16 kind;             // 0xC2
    float age;            // 0xC4
    int link;             // 0xC8
    int id;               // 0xCC
    int fx;               // 0xD0
    Vec3 start;           // 0xD4
    u32 self;             // 0xE0
    HitTarget* target;    // 0xE4
    float scale;          // 0xE8
    int unkEC;            // 0xEC
    void* data;           // 0xF0
    BehKind* beh;         // 0xF4
    float fade;           // 0xF8
};

// Message 0x407 (damage), 0x50 bytes.
struct DamageMsg {
    HitTarget* target;    // 0x00
    Projectile* source;   // 0x04
    int owner;            // 0x08
    float amount;         // 0x0C
    int unk10;            // 0x10
    u32 hash;             // 0x14
    IVec3 pos;            // 0x18
    IVec3 normal;         // 0x24
    IVec3 dir;            // 0x30
    int material;         // 0x3C
    int unk18;            // 0x40
    int unk14;            // 0x44
    int surface;          // 0x48
    float force;          // 0x4C
    u8 pad50[0xC];
};

void ActorGetMatrix(Mtx34* m, Actor* a);
int ModelBoneCount(int model);
unsigned char* ModelAnimate(int model, char* bones, int a);
void ModelSetMatrix(int model, Mtx34* m, char* bones);
void Draw3DBeginLine(int texture, void* xf, float width);
void DrawLine(float* a, float* b, unsigned char r0, unsigned char g0, unsigned char b0, unsigned char a0,
              unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1);
void DrawDisableTexOffsets();
int VarFindInt(unsigned long group, unsigned long def, unsigned long name);
int PlayerInHost(Actor* a);
int HandleToPlayer(int handle);
int PlayerTeam(int player);

extern "C" {
int SendMsg(int to, int msg, void* data);
int fn_80049940();
int fn_8001F928(void* obj, void* box, u32 mask, void* callback, void* ctx);
void fn_8009F44C(void* out, void* actor, void* in);
void fn_8016E13C(void* shape, void* v, int flag);
int fn_8016F120(int fx, IVec3* pos);
void* fn_8003A1F0(int model, void* bones, void* map);
void fn_800A0100(void* m);
void fn_800A0094(Vec3* out, Vec3* in);
void fn_8003F15C(int link, void* m, int a, int b, int c, float f);
void fn_80289EB4(IVec3* pos, IVec3* normal, int surface, Projectile* p);
void fn_80184704(IVec3* pos, IVec3* normal, int surface, Projectile* p);
void fn_802C28EC(IVec3* pos, IVec3* normal, int surface, Projectile* p);
void fn_801ED8CC(float a, float b, int kind, SweepHit* hit, int time, int c, IVec3* dir, int d, int e);

int fn_80183DD0(SweepHit* hit, SweepCtx* ctx);
void fn_80184194(int material, IVec3* pos, u32 def);

struct BoneSet {
    u8 pad[0x10];
    float x, y, z;        // 0x10
    u8 pad1C[0x74];
};

BoneSet lbl_80495700;
u32 lbl_804D3018 = 0xE5B84C95;
float lbl_804D301C = 1.0f;

inline float Clamp(float x, float lo, float hi) {
    if (x < lo) {
        return lo;
    }
    if (x > hi) {
        return hi;
    }
    return x;
}

inline void IVec3ToVec3(Vec3* out, IVec3* in) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}

// Asks the target's 0x41E handler for the value stored under a name hash.
inline int QueryName(int* out, int target, unsigned long hash) {
    QueryMsg q;
    int r;

    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    r = SendMsg(target, 0x41E, &q);
    *out = *(int*)&q.result;
    return r;
}

bool fn_80181644(void* behp, void* hostp) {
    BehKind* kind = (BehKind*)behp;
    Projectile* p = (Projectile*)hostp;
    SweepCtx ctx;
    IVec3 d;
    IVec3 dist;
    IVec3 dist2;
    int box[6];
    SweepHit hit;

    ctx.hit = &hit;
    ctx.id = p->id;
    box[0] = p->prev.x;
    box[1] = p->prev.y;
    box[2] = p->prev.z;
    box[3] = p->spawn.pos.x;
    box[4] = p->spawn.pos.y;
    box[5] = p->spawn.pos.z;
    d.x = p->start.x;
    d.y = p->start.y;
    d.z = p->start.z;
    d.x -= p->place.pos.x;
    d.y -= p->place.pos.y;
    d.z -= p->place.pos.z;
    fn_8009F44C(&d, p, &d);
    fn_8016E13C(p->shape, &d, 1);
    if (fn_8001F928(p, box, 0x1200A000, fn_80183DD0, &ctx) != 0) {
    if (kind->desc != 0 && kind->desc->blocked != 0) {
        return kind->desc->blocked(kind, p, &hit) == 0;
    }
    fn_8016F120(kind->hitFx, &hit.pos);
    IVec3ToVec3(&p->spawn.pos, &ctx.hit->pos);
    p->place.pos = ctx.hit->pos;
    if (hit.obj == 0) {
        if ((int)((u32)p->id >> 24) == 2 && !fn_80049940()) {
            fn_80289EB4(&hit.pos, &hit.normal, hit.surface, p);
            if (!(p->state & 1)) {
                fn_80184704(&hit.pos, &hit.normal, hit.surface, p);
            }
        } else {
            fn_802C28EC(&hit.pos, &hit.normal, hit.surface, p);
        }
        if (!(p->state & 4)) {
            fn_80184194(hit.material, &hit.pos, lbl_804D3018);
        }
    } else {
        DamageMsg msg;
        float amount;
        int owner;
        HitTarget* target;
        if (fn_80049940()) {
            amount = kind->count2 * p->scale;
            if ((int)(hit.obj->handle >> 24) == 2) {
                amount *= kind->unk10;
            }
        } else {
            amount = kind->count * p->scale;
            if ((int)(hit.obj->handle >> 24) == 2) {
                amount *= kind->unk0C;
            }
        }
        msg.source = p;
        owner = p->id;
        msg.owner = owner;
        msg.hash = kind->hash1C;
        msg.amount = amount;
        msg.force = kind->unk18;
        msg.pos = hit.pos;
        msg.normal = hit.normal;
        msg.dir.x = p->spawn.dir.x;
        msg.dir.y = p->spawn.dir.y;
        msg.dir.z = p->spawn.dir.z;
        msg.surface = hit.surface;
        msg.unk14 = hit.unk14;
        msg.unk18 = hit.unk18;
        msg.material = hit.material;
        target = hit.obj;
        msg.target = target;
        if ((int)(target->handle >> 24) == 0x13 && (int)((u32)owner >> 24) == 2) {
            int handled = 0;
            SweepHit h = hit;
            int v;
            if (target->unk908 == 0x8CD18EE1) {
                return 0;
            }
            QueryName(&v, 0x4B000000, 0x0E83B88B);
            if (fn_80049940() && PlayerTeam(HandleToPlayer(hit.obj->handle)) == PlayerTeam(HandleToPlayer(p->id)) &&
                v == 0) {
                return 0;
            }
            dist.x = box[3] - box[0];
            dist.y = box[4] - box[1];
            dist.z = box[5] - box[2];
            if (*target->type == 0x11BAC) {
                fn_801ED8CC(300.0f, 100.0f, 0, &h, 2000, 0, 0, 1, 1);
                handled = 1;
            } else if (*target->type == 0x2889735F || *target->type == 0x27B2569E ||
                       *target->type == 0x4BD66AD8) {
                fn_801ED8CC(150.0f, 50.0f, 2, &h, 2000, 0, &dist, 0, 1);
                handled = 1;
            }
            if (!handled) {
                fn_801ED8CC(300.0f, 100.0f, 1, &h, 2000, 1, &dist, 0, 1);
            }
        } else if ((int)(target->handle >> 24) == 2 && (int)((u32)p->id >> 24) == 2 &&
                   (int)target->handle != p->id) {
            HitTarget* t = target;
            SweepHit h = hit;
            int v;
            QueryName(&v, 0x4B000000, 0x0E83B88B);
            if (fn_80049940() && PlayerTeam(HandleToPlayer(hit.obj->handle)) == PlayerTeam(HandleToPlayer(p->id)) &&
                v == 0) {
                return 0;
            }
            dist2.x = box[3] - box[0];
            dist2.y = box[4] - box[1];
            dist2.z = box[5] - box[2];
            if (!PlayerInHost((Actor*)t)) {
                return 0;
            }
            if (t->host == 0 || (int)(t->host->handle >> 24) != 0x13) {
                return 0;
            }
            fn_801ED8CC(300.0f, 100.0f, 1, &h, 2000, 1, &dist2, 0, 1);
        }
        hit.obj->handler(0x407, &msg);
    }
    return 0;
    }
    return 1;
}

void fn_801837FC(BehKind* kind, Projectile* p) {
    Mtx34 m;
    void* mtx;
    int bones = ModelBoneCount(p->link);
    void* map;
    if (bones > 5) {
        bones = 5;
    }
    ActorGetMatrix(&m, (Actor*)p);
    map = ModelAnimate(p->link, (char*)&lbl_80495700, 1);
    lbl_80495700.x = 0.0f;
    lbl_80495700.y = 0.0f;
    lbl_80495700.z = 0.0f;
    ModelSetMatrix(p->link, &m, (char*)&lbl_80495700);
    mtx = fn_8003A1F0(p->link, &lbl_80495700, map);
    fn_800A0100(&m);
    if (p->link != -1 && !(p->state & 8)) {
        ActorGetMatrix(&m, (Actor*)p);
        fn_8003F15C(p->link, mtx, bones, 0, 0, 1.0f);
    }
}

void fn_801838F8(BehKind* kind, Projectile* p) {
    Vec3 off = {0.0f, 768.0f, 0.0f};
    PlaceF m;
    Vec3 end;
    Vec3* pos;
    ActorGetMatrix((Mtx34*)&m, (Actor*)p);
    pos = &m.pos;
    fn_800A0100(&m);
    fn_800A0094(&end, &off);
    end.x += pos->x;
    end.y += pos->y;
    end.z += pos->z;
    Draw3DBeginLine(-4, 0, 4.0f);
    DrawLine(&pos->x, &end.x, 0, 0, 0, 0, 0xFF, 0xFF, 0xC8, 0xFF);
    DrawDisableTexOffsets();
}

void fn_801839D4(BehKind* kind, Projectile* p) {
    Vec3 off = {0.0f, 2048.0f, 0.0f};
    PlaceF m;
    Vec3 end;
    float t = Clamp(1.0f - p->age / 1500.0f, 0.0f, 1.0f);
    Vec3* pos;
    ActorGetMatrix((Mtx34*)&m, (Actor*)p);
    pos = &m.pos;
    fn_800A0100(&m);
    fn_800A0094(&end, &off);
    end.x += pos->x;
    end.y += pos->y;
    end.z += pos->z;
    if (!(p->state & 8)) {
        Draw3DBeginLine(-4, 0, 5.0f);
        DrawLine(&pos->x, &end.x, 0xFF, 0xFF, 0xC8, 0x32, 0xFF, 0xFF, 0xC8, 0xE1);
        DrawDisableTexOffsets();
    }
    if (t > 0.0f) {
        Draw3DBeginLine(-4, 0, 6.0f);
        DrawLine(&p->start.x, &pos->x, 0x60, 0x60, 0x60, (int)(48.0f * t), 0x60, 0x60, 0x60, (int)(64.0f * t));
        DrawDisableTexOffsets();
        p->fade = 0.0f;
    }
}

void fn_80183B98(BehKind* kind, Projectile* p) {
    Vec3 off = {0.0f, -1536.0f, 0.0f};
    PlaceF m;
    Vec3 end;
    float t = (p->state & 0x10) ? Clamp(1.0f - p->age / 800.0f, 0.0f, 1.0f)
                                : Clamp(1.0f - p->age / 300.0f, 0.0f, 1.0f);
    {
        float s = p->age / 40.0f;
        off.y *= lbl_804D301C < s ? lbl_804D301C : s;
    }
    Vec3* pos;
    ActorGetMatrix((Mtx34*)&m, (Actor*)p);
    pos = &m.pos;
    fn_800A0100(&m);
    fn_800A0094(&end, &off);
    end.x += pos->x;
    end.y += pos->y;
    end.z += pos->z;
    if (!(p->state & 8)) {
        Draw3DBeginLine(-4, 0, 3.0f);
        DrawLine(&end.x, &pos->x, 0xFF, 0xFF, 0xC8, 0, 0xFF, 0xFF, 0xC8, 0x64);
        DrawDisableTexOffsets();
    }
    if (t > 0.0f) {
        Draw3DBeginLine(-4, 0, 4.0f);
        DrawLine(&p->start.x, &pos->x, 0x40, 0x40, 0x40, (int)(32.0f * t), 0x40, 0x40, 0x40, (int)(48.0f * t));
        DrawDisableTexOffsets();
        p->fade = 0.0f;
    }
}

// Sweep filters: keep the first hit that is not the shooter (and passes the type test).
int fn_80183DD0(SweepHit* hit, SweepCtx* ctx) {
    if (hit->obj != 0) {
        Msg400 info;
        int owner;
        info.actor = (Actor*)hit->obj;
        hit->obj->handler(0x400, &info);
        owner = info.ownerId;
        if (owner == ctx->id || !(info.flags & 2)) {
            return 0;
        }
        if (ctx->id != -1) {
            int v;
            if (QueryName(&v, owner, 0x717E6E02) != 0 && v == ctx->id) {
                return 0;
            }
        }
    }
    *ctx->hit = *hit;
    return 1;
}

int fn_80183F60(SweepHit* hit, SweepCtx* ctx) {
    if (hit->obj != 0) {
        Msg400 info;
        info.actor = (Actor*)hit->obj;
        hit->obj->handler(0x400, &info);
        if (info.state == 0 || info.ownerId == ctx->id || !(info.flags & 1)) {
            return 0;
        }
    }
    *ctx->hit = *hit;
    return 1;
}

int fn_80184080(SweepHit* hit, SweepCtx* ctx) {
    if (hit->obj != 0) {
        Msg400 info;
        info.actor = (Actor*)hit->obj;
        hit->obj->handler(0x400, &info);
        if (info.state == 0 || info.ownerId == ctx->id) {
            return 0;
        }
    }
    *ctx->hit = *hit;
    return 1;
}

struct MaterialFx {
    int material;
    u32 name;
};

extern MaterialFx lbl_803A8690[11];

void fn_80184194(int material, IVec3* pos, u32 def) {
    int fx = -1;
    int i;
    for (i = 0; i < 11; i++) {
        if ((unsigned char)material == lbl_803A8690[i].material) {
            fx = VarFindInt(0xB1545494, def, lbl_803A8690[i].name);
            break;
        }
    }
    if (fx == -1) {
        fx = VarFindInt(0xB1545494, def, 0x86DF6221);
    }
    fn_8016F120(fx, pos);
}

}

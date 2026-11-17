// u_801CC6D0.cpp: 0x801CC6D0-0x801CDFBC, 10 functions. The "Script Move" actor type.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   actor type descriptor 0x803ACBB8 {0, "Script Move" 0x80348878, fn_801CCCE0, fn_801CD484, fn_801CDE88,
//   0, fn_801CDE40, fn_801CCCDC, fn_801CCCD8}; fn_801CC6D0 registers it (the register function comes
//   first, as in Skybox.cpp); no POSSESS table in its pool
//   .data 0x803ACBE8, 0x803ACBF8; .sdata2 0x804D8B48-0x804D8B60
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

#include <string.h>
#include "game/types.h"
#include "game/actor.h"
#include "game/msg.h"

#define fabs(x) __fabs(x)

// Per-actor state of a scripted mover (VAlloc'd, 0x4C bytes, at actor +0xA4).
struct ScriptMove {
    int target;                                // 0x00: handle of the actor to move towards (-1: none)
    int look;                                  // 0x04: handle of the actor to face (-1: none)
    int anim;                                  // 0x08: animation played when the move ends (-1: none)
    int animArg;                               // 0x0C
    unsigned long flags;                       // 0x10: 1 towards, 2 away, 8 start anim pending, 0x10 follow
    int mode;                                  // 0x14: speed set 0..3
    int state;                                 // 0x18: 0 idle, 1 start, 2 moving, 3 arrived
    Vec3 pos;                                  // 0x1C
    Vec3 vel;                                  // 0x28
    Vec3 dir;                                  // 0x34
    Vec3 unk40;                                // 0x40
};

// Per-type parameters (actor +0x84).
struct ScriptMoveInfo {
    unsigned long hash;                        // 0x00
    char pad04[0x2C];
    float maxSpeed[4];                         // 0x30
    float accel[4];                            // 0x40
    char pad50[0x44];
    int startAnim;                             // 0x94
    int endAnim;                               // 0x98
    char pad9C[0x38];
    int startText;                             // 0xD4
    int endText;                               // 0xD8
};

// Message 0xC12: play an animation and call back when it ends (0x20 bytes).
struct Msg0C12 {
    int unk00;                                 // 0x00
    int anim;                                  // 0x04
    int unk08;                                 // 0x08
    int blend;                                 // 0x0C
    int flags;                                 // 0x10
    void (*callback)(int msg, Actor* actor);   // 0x14
    Actor* actor;                              // 0x18
    int unk1C;                                 // 0x1C
};

// Message 0x1800: start moving.
struct Msg1800 {
    Actor* actor;                              // 0x00
    int target;                                // 0x04
    unsigned long speed;                       // 0x08
    int anim;                                  // 0x0C
    int animArg;                               // 0x10
};

int ActorTypeRegister(ActorType* type);
int ActorPlayAnim(int handle, u32 hash, int arg);
void ActorGetMatrix(Mtx34* m, Actor* a);
void PlacementToMatrix(Actor* a, Mtx34* m);
void* VAlloc(int size);
void VFree(void* p);

extern "C" {
int SendMsg(int to, int msg, void* data);
int fn_80019F88(void* data, Actor* actor);
float fn_800E1020(Vec3* v);
void fn_800E1098(Vec3* out, Vec3* in, float length);
int fn_800E4C64(Actor* actor, int msg, void* data, int arg);
void fn_800E48A0(Actor* actor, float dt);
int fn_800E6444(Actor* actor);
void fn_800E7898(Actor* actor, Mtx34* m);
int fn_8008E964(int anim);
void fn_8008DE4C(int anim, int a, int b, int c);
void fn_8016F390(int text, int a, int b);
void fn_800A02C0(Mtx34* m, Vec3* v, int row);
void fn_8016D6A8(Vec3* v);
void fn_8016D4EC(Vec3* out, void* in);
int fn_80090EA4(int id, int model);
}

int fn_801CCCE0(Actor* actor, int msg, void* data, int arg);
void fn_801CD484(Actor* actor, float dt);
int fn_801CDE88(Actor* actor);
void fn_801CDE40(Actor* actor);
void fn_801CCCDC(Actor* actor);
void fn_801CCCD8(Actor* actor);

ActorType lbl_803ACBB8 = {
    0, "Script Move", (ActorMsgFunc)fn_801CCCE0, fn_801CD484, fn_801CDE88, 0, fn_801CDE40,
    {(void*)fn_801CCCDC, (void*)fn_801CCCD8, 0, 0, 0},
};

// Speed set names, by index.
static unsigned long sSpeedHashes[4] = {0x00270002, 0x04B61EF4, 0x002889C9, 0x00013E6B};

static Vec3 sUp = {0.0f, 0.0f, 1.0f};

inline int VecNonZero(const Vec3* v) {
    return (fabs(v->x) > 1.1920929e-7f) + (fabs(v->y) > 1.1920929e-7f) + (fabs(v->z) > 1.1920929e-7f);
}

inline float VecNormalize(Vec3* out, const Vec3* in) {
    double x, y, z, len;
    float inv;

    if (VecNonZero(in) == 0) {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
        return 0.0f;
    }
    x = (double)in->x;
    y = (double)in->y;
    z = (double)in->z;
    len = x * x + y * y + z * z;
    inv = __frsqrte((float)len);
    out->x = (float)(x * inv);
    out->y = (float)(y * inv);
    out->z = (float)(z * inv);
    return __fres(inv);
}

// Direction from the mover's position to an actor.
inline void ScriptMoveAim(ScriptMove* s, Actor* to) {
    s->dir.x = (float)to->place.pos.x - s->pos.x;
    s->dir.y = (float)to->place.pos.y - s->pos.y;
    s->dir.z = (float)to->place.pos.z - s->pos.z;
}

// Ends the start animation / resets a finished move.
inline void ScriptMoveAnimDone(Actor* actor) {
    ScriptMove* s = (ScriptMove*)actor->data;

    switch (s->state) {
    case 1:
        ActorPlayAnim(actor->handle, 0x868574D2, actor->handle);
        s->state = 2;
        break;
    case 3:
        s->state = 0;
        s->target = -1;
        s->vel.x = 0.0f;
        s->vel.y = 0.0f;
        s->vel.z = 0.0f;
        ActorPlayAnim(actor->handle, 0xC4BD2165, actor->handle);
        if (s->anim != -1) {
            ActorPlayAnim(s->anim, s->animArg, actor->handle);
        }
        break;
    }
}

void fn_801CC6D0() { ActorTypeRegister(&lbl_803ACBB8); }

// Animation callback of message 0xC12.
void fn_801CC6F8(int msg, Actor* actor) {
    ScriptMove* s = (ScriptMove*)actor->data;

    switch (msg) {
    case 0x1B83833F:
        switch (s->state) {
        case 1:
            ActorPlayAnim(actor->handle, 0x868574D2, actor->handle);
            s->state = 2;
            break;
        case 3:
            s->state = 0;
            s->target = -1;
            s->vel.x = 0.0f;
            s->vel.y = 0.0f;
            s->vel.z = 0.0f;
            ActorPlayAnim(actor->handle, 0xC4BD2165, actor->handle);
            if (s->anim != -1) {
                ActorPlayAnim(s->anim, s->animArg, actor->handle);
            }
            break;
        }
        break;
    }
}

// Message 0x41D: script commands.
int fn_801CC7D4(Actor* actor, Msg41D* m) {
    ScriptMove* s;

    if (fn_80019F88(m, actor)) {
        return 1;
    }
    s = (ScriptMove*)actor->data;
    if (s == 0) {
        return 0;
    }
    switch (m->cmd) {
    case 0xCCE3DBE2:
        if (m->arg != 0) {
            MsgResult r;
            s->flags |= 1;
            s->flags &= ~2;
            if (s->target != -1 && SendMsg(s->target, 0x400, &r) && r.p != 0) {
                ScriptMoveAim(s, (Actor*)r.p);
                VecNormalize(&s->dir, &s->dir);
            }
        } else {
            s->flags &= ~1;
        }
        return 1;
    case 0x3143A646:
        if (m->arg != 0) {
            MsgResult r;
            s->flags |= 2;
            s->flags &= ~1;
            if (s->target != -1 && SendMsg(s->target, 0x400, &r) && r.p != 0) {
                ScriptMoveAim(s, (Actor*)r.p);
                s->dir.x = -s->dir.x;
                s->dir.y = -s->dir.y;
                s->dir.z = -s->dir.z;
                VecNormalize(&s->dir, &s->dir);
            }
        } else {
            s->flags &= ~2;
        }
        return 1;
    case 0x7CB4512A:
        if (m->arg != 0) {
            s->flags |= 0x10;
        } else {
            s->flags &= ~0x10;
        }
        return 1;
    case 0x9F5F93DE:
        s->dir.z = 0.0f;
        VecNormalize(&s->dir, &s->dir);
        return 1;
    }
    return 0;
}

void fn_801CCCD8(Actor* actor) {}

void fn_801CCCDC(Actor* actor) {}

// Starts the move: plays the start animation through message 0xC12 when the type has one.
inline void ScriptMoveStart(Actor* actor, int anim, int text) {
    Msg0C12 m;

    memset(&m, 0, sizeof(Msg0C12));
    m.anim = anim;
    m.blend = 0x50;
    m.flags = 0x1000;
    m.callback = fn_801CC6F8;
    m.actor = actor;
    SendMsg(actor->id, 0xC12, &m);
    fn_8016F390(text, 0, 0);
}

int fn_801CCCE0(Actor* self, int msg, void* data, int arg) {
    Actor* actor = *(Actor**)data;

    switch (msg) {
    case 0xC07: {
        ScriptMove* s = (ScriptMove*)actor->data;
        if (fn_800E4C64(self, msg, data, arg) == 0) {
            return 0;
        }
        s->target = -1;
        s->mode = 0;
        s->pos.x = (float)actor->place.pos.x;
        s->pos.y = (float)actor->place.pos.y;
        s->pos.z = (float)actor->place.pos.z;
        return 1;
    }
    case 0x41E: {
        Msg41D* m = (Msg41D*)data;
        int handled;
        if (m->cmd == 0xAFDE81D6) {
            ScriptMoveInfo* info = (ScriptMoveInfo*)actor->info;
            if (info != 0 && info->hash == 0xD34C643C) {
                m->arg = 0;
            } else {
                m->arg = 1;
            }
            handled = 1;
        } else {
            handled = 0;
        }
        if (handled) {
            return 1;
        }
        break;
    }
    case 0x41D:
        if (fn_801CC7D4(actor, (Msg41D*)data)) {
            return 1;
        }
        break;
    case 0x1800: {
        Msg1800* m = (Msg1800*)data;
        ScriptMove* s = (ScriptMove*)actor->data;
        int mode;
        int state;
        MsgResult r;

        s->target = m->target;
        if (m->speed == sSpeedHashes[0]) {
            mode = 0;
        } else if (m->speed == sSpeedHashes[1]) {
            mode = 1;
        } else if (m->speed == sSpeedHashes[2]) {
            mode = 2;
        } else if (m->speed == sSpeedHashes[3]) {
            mode = 3;
        } else {
            mode = 0;
        }
        s->mode = mode;
        s->anim = m->anim;
        s->animArg = m->animArg;
        s->vel.x = 0.0f;
        s->vel.y = 0.0f;
        s->vel.z = 0.0f;
        if (s->target != -1 && SendMsg(s->target, 0x400, &r) && r.p != 0) {
            Actor* t = (Actor*)r.p;
            if (s->flags & 1) {
                ScriptMoveAim(s, t);
                VecNormalize(&s->dir, &s->dir);
            } else if (s->flags & 2) {
                ScriptMoveAim(s, t);
                s->dir.x = -s->dir.x;
                s->dir.y = -s->dir.y;
                s->dir.z = -s->dir.z;
                VecNormalize(&s->dir, &s->dir);
            } else if (s->flags & 0x10) {
                actor->place = t->place;
                actor->place.pos.x = (int)s->pos.x;
                actor->place.pos.y = (int)s->pos.y;
                actor->place.pos.z = (int)s->pos.z;
            }
        }
        state = s->state;
        s->state = 1;
        if (state == 2 || state == 1 || ((ScriptMoveInfo*)actor->info)->startAnim == -1) {
            ScriptMoveAnimDone(actor);
        } else if (fn_8008E964(actor->anim) == 0) {
            s->flags |= 8;
            ScriptMoveAnimDone(actor);
        } else {
            ScriptMoveStart(actor, ((ScriptMoveInfo*)actor->info)->startAnim,
                            ((ScriptMoveInfo*)actor->info)->startText);
        }
        return 1;
    }
    case 0x1801: {
        Msg1800* m = (Msg1800*)data;
        ScriptMove* s = (ScriptMove*)actor->data;
        s->look = m->target;
        s->anim = m->anim;
        s->animArg = m->animArg;
        return 1;
    }
    }
    return fn_800E4C64(self, msg, data, arg);
}

void fn_801CD484(Actor* actor, float dt) {
    ScriptMove* s = (ScriptMove*)actor->data;
    ScriptMoveInfo* info;
    Vec3 fwd;
    MsgResult r;

    if (s == 0) {
        return;
    }
    fwd = s->dir;
    if (s->state == 2 && s->target != -1 && SendMsg(s->target, 0x400, &r) && r.p != 0 && s->state == 2) {
        Vec3 d;
        Vec3 vn;
        Vec3 dn;
        float dist;
        float speed;
        float accel;
        float dot;

        info = (ScriptMoveInfo*)actor->info;
        if ((s->flags & 8) && fn_8008E964(actor->anim)) {
            fn_8008DE4C(actor->anim, 0, info->startAnim, 0x50);
            s->flags &= ~8;
        }
        ScriptMoveAim(s, (Actor*)r.p);
        d = s->dir;
        d.x = (float)((Actor*)r.p)->place.pos.x - s->pos.x;
        d.y = (float)((Actor*)r.p)->place.pos.y - s->pos.y;
        d.z = (float)((Actor*)r.p)->place.pos.z - s->pos.z;
        dist = fn_800E1020(&d);
        speed = fn_800E1020(&s->vel);
        accel = info->accel[s->mode];
        VecNormalize(&vn, &s->vel);
        VecNormalize(&dn, &d);
        dot = vn.z * dn.z + vn.x * dn.x + vn.y * dn.y;
        if (fabs(accel) < 1.1920929e-7f) {
            fn_800E1098(&d, &s->vel, info->maxSpeed[s->mode]);
            fn_800E1098(&d, &d, speed * dt / 1000.0f);
            s->pos.x += d.x;
            s->pos.y += d.y;
            s->pos.z += d.z;
        } else if (dist <= 0.5f * (speed * speed) / accel) {
            fn_800E1098(&d, &d, accel * dt / 1000.0f);
            s->vel.x -= d.x;
            s->vel.y -= d.y;
            s->vel.z -= d.z;
            if (speed < 1.0f) {
                fn_800E1098(&s->vel, &s->vel, 1.0f);
            }
            fn_800E1098(&d, &s->vel, speed * dt / 1000.0f);
            s->pos.x += d.x;
            s->pos.y += d.y;
            s->pos.z += d.z;
        } else {
            float max;
            fn_800E1098(&d, &d, accel * dt / 1000.0f);
            s->vel.x += d.x;
            s->vel.y += d.y;
            s->vel.z += d.z;
            max = info->maxSpeed[s->mode];
            if (max < speed) {
                fn_800E1098(&s->vel, &s->vel, max);
                speed = max;
            }
            fn_800E1098(&d, &s->vel, speed * dt / 1000.0f);
            s->pos.x += d.x;
            s->pos.y += d.y;
            s->pos.z += d.z;
        }
        if (dot < 0.0f || dist <= fn_800E1020(&d)) {
            s->state = 3;
            if (info->endAnim == -1 || fn_8008E964(actor->anim) == 0) {
                ScriptMoveAnimDone(actor);
            } else {
                ScriptMoveStart(actor, info->endAnim, info->endText);
            }
            ActorPlayAnim(actor->handle, 0xCB382E92, actor->handle);
        }
    }
    if (s->look != -1 && SendMsg(s->look, 0x400, &r) && r.p != 0) {
        ScriptMoveAim(s, (Actor*)r.p);
        VecNormalize(&s->dir, &s->dir);
    }
    if ((s->flags & 0x10) == 0) {
        Vec3 side;
        Vec3 up;
        Placement place;

        side.x = fwd.y * sUp.z - fwd.z * sUp.y;
        side.z = fwd.x * sUp.y - fwd.y * sUp.x;
        side.y = fwd.z * sUp.x - fwd.x * sUp.z;
        up.y = side.z * fwd.x - side.x * fwd.z;
        up.x = side.y * fwd.z - side.z * fwd.y;
        up.z = side.x * fwd.y - side.y * fwd.x;
        fn_800A02C0((Mtx34*)&place, &side, 0);
        fn_800A02C0((Mtx34*)&place, &fwd, 1);
        fn_800A02C0((Mtx34*)&place, &up, 2);
        place.pos.x = (int)s->pos.x;
        place.pos.y = (int)s->pos.y;
        place.pos.z = (int)s->pos.z;
        PlacementToMatrix(actor, (Mtx34*)&place);
        fn_800E7898(actor, (Mtx34*)&place);
        actor->unk118 = (float)place.pos.z;
    } else if (s->target != -1 && SendMsg(s->target, 0x400, &r) && r.p != 0) {
        Placement place;

        ActorGetMatrix((Mtx34*)&place, (Actor*)r.p);
        place.pos.x = (int)s->pos.x;
        place.pos.y = (int)s->pos.y;
        place.pos.z = (int)s->pos.z;
        PlacementToMatrix(actor, (Mtx34*)&place);
        fn_800E7898(actor, (Mtx34*)&place);
        actor->unk118 = (float)place.pos.z;
    }
    actor->flags |= 0x800000;
    fn_800E48A0(actor, dt);
}

void fn_801CDE40(Actor* actor) {
    if (actor != 0) {
        if (actor->data == 0) {
            return;
        }
        VFree(actor->data);
        actor->data = 0;
    }
}

int fn_801CDE88(Actor* actor) {
    ScriptMove* s;

    if (fn_800E6444(actor) == 0) {
        return 0;
    }
    if (actor->data == 0) {
        s = (ScriptMove*)VAlloc(sizeof(ScriptMove));
        if (s == 0) {
            return 0;
        }
        actor->data = s;
    }
    s->flags = 0;
    s->target = -1;
    s->look = -1;
    s->anim = -1;
    s->animArg = -1;
    s->mode = 0;
    s->state = 0;
    s->vel.x = 0.0f;
    s->vel.y = 0.0f;
    s->vel.z = 0.0f;
    fn_8016D6A8(&s->unk40);
    s->pos.x = (float)actor->place.pos.x;
    s->pos.y = (float)actor->place.pos.y;
    s->pos.z = (float)actor->place.pos.z;
    fn_8016D4EC(&s->dir, (char*)actor->def + 0xC);
    actor->anim = fn_80090EA4(actor->id, actor->model);
    actor->flags |= 0x100000;
    return 1;
}

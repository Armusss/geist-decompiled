// u_801CC6D0.cpp: 0x801CC6D0-0x801CDFBC, 9 functions. The "Script Move" actor type.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries (rechecked 2026-11-21):
//   actor type descriptor gScriptMoveType 0x803ACBB8 {0, "Script Move" 0x80348878, Msg, Update, Init, 0,
//   Destroy, Save, Load}; ScriptMoveRegister registers it (the register function comes first, as in
//   Skybox.cpp); no POSSESS table in its pool. Data only this unit touches: .rodata 0x80348878-0x80348888,
//   .data 0x803ACBB8-0x803ACC08 (descriptor, speed-set hashes, up vector), .sdata2 0x804D8B48-0x804D8B68
//   (0.0f, epsilon, int->float bias, 1000, 0.5, 1.0; u_801CA16C reads 0x804D8B40/44, u_801CDFBC starts at
//   0x804D8B68). Neighbour u_801CDFBC is Matching from 0x801CDFBC.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

#include <string.h>
#include "game/types.h"
#include "game/actor.h"
#include "game/msg.h"


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
    float maxSpeed[4];                         // 0x30: per speed set
    float accel[4];                            // 0x40: per speed set
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

float __fabsf(float x);
int ActorTypeRegister(ActorType* type);
int ActorPlayAnim(int handle, u32 hash, int arg);
int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
int ActorInitBase(Actor* actor);                                // 0x800E6444
void ActorUpdate(Actor* actor, float dt);                       // 0x800E48A0
void ActorSetPlacement(Actor* actor, PlaceF* p);
void ActorGetMatrix(Mtx34* m, Actor* a);
void PlacementToMatrix(Mtx34* m, PlaceF* p);                    // 0x8009EF04
void* VAlloc(int size);
void VFree(void* p);

extern "C" {
int SendMsg(int to, int msg, void* data);
void FxStart(int text, int a, int b);                           // 0x8016F390
int fn_80019F88(void* data, Actor* actor);
float fn_800E1020(Vec3* v);
void fn_800E1098(Vec3* out, Vec3* in, float length);
int fn_8008E964(int anim);
void fn_8008DE4C(int anim, int a, int b, int c);
void fn_800A02C0(Mtx34* m, Vec3* v, int row);
void fn_8016D6A8(Vec3* v);
void fn_8016D4EC(Vec3* out, void* in);
int fn_80090EA4(int id, int model);
}

int ScriptMoveMsg(Actor* actor, int msg, void* data, int arg);
void ScriptMoveUpdate(Actor* actor, float dt);
int ScriptMoveInit(Actor* actor);
void ScriptMoveDestroy(Actor* actor);
void ScriptMoveSave(void* stream);
void ScriptMoveLoad(void* stream);

ActorType gScriptMoveType = {
    0, "Script Move", (ActorMsgFunc)ScriptMoveMsg, ScriptMoveUpdate, ScriptMoveInit, 0, ScriptMoveDestroy,
    {(void*)ScriptMoveSave, (void*)ScriptMoveLoad, 0, 0, 0},
};

// Speed set names, by index.
static unsigned long sSpeedHashes[4] = {0x00270002, 0x04B61EF4, 0x002889C9, 0x00013E6B};

static Vec3 sUp = {0.0f, 0.0f, 1.0f};

// Normalizes src into dst; returns the length (0 for a zero vector).
inline float VecNormalize(Vec3* dst, const Vec3* src) {
    float len;
    float az = __fabsf(src->z);
    float ay = __fabsf(src->y);
    float ax = __fabsf(src->x);
    if ((ax > 1.1920929e-7f) + (ay > 1.1920929e-7f) + (az > 1.1920929e-7f) == 0) {
        dst->x = 0.0f;
        dst->y = 0.0f;
        dst->z = 0.0f;
    } else {
        float x = src->x;
        float y = src->y;
        float z = src->z;
        float inv = __frsqrte(x * x + y * y + z * z);
        dst->x = x * inv;
        dst->y = y * inv;
        dst->z = z * inv;
        len = __fres(inv);
    }
    return len;
}

inline ScriptMoveInfo* ScriptMoveGetInfo(Actor* actor) { return (ScriptMoveInfo*)actor->info; }

// Direction from the mover's position to an actor.
inline void ScriptMoveAim(ScriptMove* s, Actor* to) {
    Vec3 p;
    p.x = (float)to->place.pos.x;
    p.y = (float)to->place.pos.y;
    p.z = (float)to->place.pos.z;
    s->dir.x = p.x - s->pos.x;
    s->dir.y = p.y - s->pos.y;
    s->dir.z = p.z - s->pos.z;
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

void ScriptMoveRegister() { ActorTypeRegister(&gScriptMoveType); }

// Animation callback of message 0xC12.
void ScriptMoveAnimEvent(int msg, Actor* actor) {
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
int ScriptMoveCommand(Actor* actor, Msg41D* m) {
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

void ScriptMoveLoad(void* stream) {}

void ScriptMoveSave(void* stream) {}

// Speed set index of a speed name (0 when unknown).
inline int ScriptMoveSpeed(unsigned long hash) {
    int i;

    for (i = 0; i < 4; i++) {
        if (hash == sSpeedHashes[i]) {
            return i;
        }
    }
    return 0;
}

// Message 0x41E: queries.
inline int ScriptMoveQuery(Actor* actor, Msg41D* m) {
    switch (m->cmd) {
    case 0xAFDE81D6: {
        ScriptMoveInfo* info = (ScriptMoveInfo*)actor->info;
        if (info != 0 && info->hash == 0xD34C643C) {
            m->arg = 0;
        } else {
            m->arg = 1;
        }
        return 1;
    }
    }
    return 0;
}

int ScriptMoveMsg(Actor* self, int msg, void* data, int arg) {
    Actor* actor = *(Actor**)data;

    switch (msg) {
    case 0x41E:
        if (ScriptMoveQuery(actor, (Msg41D*)data)) {
            return 1;
        }
        break;
    case 0x41D:
        if (ScriptMoveCommand(actor, (Msg41D*)data)) {
            return 1;
        }
        break;
    case 0x1800: {
        Msg1800* m = (Msg1800*)data;
        ScriptMove* s = (ScriptMove*)actor->data;
        int state;
        MsgResult r;

        s->target = m->target;
        s->mode = ScriptMoveSpeed(m->speed);
        s->anim = m->anim;
        s->animArg = m->animArg;
        s->vel.x = 0.0f;
        s->vel.y = 0.0f;
        s->vel.z = 0.0f;
        if (s->target != -1 && SendMsg(s->target, 0x400, &r) && r.p != 0) {
            if (s->flags & 1) {
                ScriptMoveAim(s, (Actor*)r.p);
                VecNormalize(&s->dir, &s->dir);
            } else if (s->flags & 2) {
                ScriptMoveAim(s, (Actor*)r.p);
                s->dir.x = -s->dir.x;
                s->dir.y = -s->dir.y;
                s->dir.z = -s->dir.z;
                VecNormalize(&s->dir, &s->dir);
            } else if (s->flags & 0x10) {
                actor->place = ((Actor*)r.p)->place;
                actor->place.pos.x = (int)s->pos.x;
                actor->place.pos.y = (int)s->pos.y;
                actor->place.pos.z = (int)s->pos.z;
            }
        }
        state = s->state;
        s->state = 1;
        if (state != 2 && state != 1 && ((ScriptMoveInfo*)actor->info)->startAnim != -1) {
            if (fn_8008E964(actor->anim) != 0) {
                {
                Msg0C12 m;

                memset(&m, 0, sizeof(Msg0C12));
                m.anim = ((ScriptMoveInfo*)actor->info)->startAnim;
                m.blend = 0x50;
                m.flags = 0x1000;
                m.callback = ScriptMoveAnimEvent;
                m.actor = actor;
                SendMsg(actor->id, 0xC12, &m);
                FxStart(((ScriptMoveInfo*)actor->info)->startText, 0, 0);
            }
            } else {
                s->flags |= 8;
                ScriptMoveAnimDone(actor);
            }
        } else {
            ScriptMoveAnimDone(actor);
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
    case 0xC07: {
        ScriptMove* s = (ScriptMove*)actor->data;
        if (ActorDefaultMsg(self, msg, data, arg) != 0) {
            s->target = -1;
            s->mode = 0;
            s->pos.x = (float)actor->place.pos.x;
            s->pos.y = (float)actor->place.pos.y;
            s->pos.z = (float)actor->place.pos.z;
            return 1;
        }
        return 0;
    }
    }
    return ActorDefaultMsg(self, msg, data, arg);
}

void ScriptMoveUpdate(Actor* actor, float dt) {
    ScriptMove* s = (ScriptMove*)actor->data;
    Vec3 fwd;
    MsgResult r;

    if (s == 0) {
        return;
    }
    fwd = s->dir;
    switch (s->state) {
    case 2:
    if (s->target != -1 && SendMsg(s->target, 0x400, &r) && r.p != 0 && s->state == 2) {
        Vec3 d;
        Vec3 vn;
        Vec3 dn;
        float dist;
        float speed;
        float accel;
        float dot;
        float aabs;

        if ((s->flags & 8) && fn_8008E964(actor->anim)) {
            fn_8008DE4C(actor->anim, 0, ((ScriptMoveInfo*)actor->info)->startAnim, 0x50);
            s->flags &= ~8;
        }
        d.x = (float)((Actor*)r.p)->place.pos.x;
        d.y = (float)((Actor*)r.p)->place.pos.y;
        d.z = (float)((Actor*)r.p)->place.pos.z;
        d.x -= s->pos.x;
        d.y -= s->pos.y;
        d.z -= s->pos.z;
        dist = fn_800E1020(&d);
        speed = fn_800E1020(&s->vel);
        accel = ScriptMoveGetInfo(actor)->accel[s->mode];
        VecNormalize(&vn, &s->vel);
        VecNormalize(&dn, &d);
        aabs = __fabsf(accel);
        dot = vn.x * dn.x + vn.y * dn.y + vn.z * dn.z;
        if (aabs < 1.1920929e-7f) {
            fn_800E1098(&s->vel, &d, ScriptMoveGetInfo(actor)->maxSpeed[s->mode]);
            fn_800E1098(&d, &s->vel, speed * dt / 1000.0f);
            s->pos.x += d.x;
            s->pos.y += d.y;
            s->pos.z += d.z;
        } else if (dist > 0.5f * (speed * speed) / accel) {
            fn_800E1098(&d, &d, accel * dt / 1000.0f);
            s->vel.x += d.x;
            s->vel.y += d.y;
            s->vel.z += d.z;
            if (speed > ScriptMoveGetInfo(actor)->maxSpeed[s->mode]) {
                speed = ScriptMoveGetInfo(actor)->maxSpeed[s->mode];
                fn_800E1098(&s->vel, &s->vel, speed);
            }
            fn_800E1098(&d, &s->vel, speed * dt / 1000.0f);
            s->pos.x += d.x;
            s->pos.y += d.y;
            s->pos.z += d.z;
        } else {
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
        }
        if (dot < 0.0f || fn_800E1020(&d) >= dist) {
            s->state = 3;
            if (((ScriptMoveInfo*)actor->info)->endAnim != -1 && fn_8008E964(actor->anim) != 0) {
                {
                Msg0C12 m;

                memset(&m, 0, sizeof(Msg0C12));
                m.anim = ((ScriptMoveInfo*)actor->info)->endAnim;
                m.blend = 0x50;
                m.flags = 0x1000;
                m.callback = ScriptMoveAnimEvent;
                m.actor = actor;
                SendMsg(actor->id, 0xC12, &m);
                FxStart(((ScriptMoveInfo*)actor->info)->endText, 0, 0);
            }
            } else {
                ScriptMoveAnimDone(actor);
            }
            ActorPlayAnim(actor->handle, 0xCB382E92, actor->handle);
        }
    }
        break;
    }
    if (s->look != -1 && SendMsg(s->look, 0x400, &r) && r.p != 0) {
        ScriptMoveAim(s, (Actor*)r.p);
        VecNormalize(&s->dir, &s->dir);
    }
    if (s->flags & 0x10) {
        if (s->target != -1 && SendMsg(s->target, 0x400, &r) && r.p != 0) {
            PlaceF place;

            ActorGetMatrix((Mtx34*)&place, (Actor*)r.p);
            place.pos = s->pos;
            PlacementToMatrix((Mtx34*)actor, &place);
            ActorSetPlacement(actor, &place);
            actor->unk118 = s->pos.z;
        }
    } else {
        Vec3 up;
        Vec3 side;
        PlaceF place;

        side.x = fwd.y * sUp.z - fwd.z * sUp.y;
        side.y = fwd.z * sUp.x - fwd.x * sUp.z;
        side.z = fwd.x * sUp.y - fwd.y * sUp.x;
        up.x = side.y * fwd.z - side.z * fwd.y;
        up.y = side.z * fwd.x - side.x * fwd.z;
        up.z = side.x * fwd.y - side.y * fwd.x;
        fn_800A02C0((Mtx34*)&place, &side, 0);
        fn_800A02C0((Mtx34*)&place, &fwd, 1);
        fn_800A02C0((Mtx34*)&place, &up, 2);
        place.pos = s->pos;
        PlacementToMatrix((Mtx34*)actor, &place);
        ActorSetPlacement(actor, &place);
        actor->unk118 = s->pos.z;
    }
    actor->flags |= 0x800000;
    ActorUpdate(actor, dt);
}

void ScriptMoveDestroy(Actor* actor) {
    if (actor == 0 || actor->data == 0) {
        return;
    }
    VFree(actor->data);
    actor->data = 0;
}

int ScriptMoveInit(Actor* actor) {
    ScriptMove* s;

    if (ActorInitBase(actor) == 0) {
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

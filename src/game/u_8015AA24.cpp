// u_8015AA24.cpp (0x8015AA24-0x8015C080): pooled timed line/curve effects (gTimedFxList, at most 8 live).
// Split from u_80157730. Evidence: its own BufferPtrArray<TimedFx> vtables (0x803A7028 / 0x803A7048) and the
// template methods closing the file (0x8015BF90-0x8015C080, include/game/bufarray.h); it alone uses .sbss
// 0x804D4EB0-0x804D4EB8, .sdata 0x804D2F50 and .sdata2 0x804D7D90-0x804D7DD4.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/bufarray.h"
#include "game/msg.h"

typedef unsigned char u8;
typedef unsigned long u32;
typedef int s32;
typedef float f32;

struct Actor;
struct Vec3;

void ActorAim(int*, Actor*, int);
void ActorGetMatrix(Mtx34*, Actor*);
void SplineHermiteEval(f32 t, Vec3* p0, Vec3* t0, Vec3* p1, Vec3* t1, Vec3* out);
void Draw3DBeginLine(int texture, void* xf, f32 width);
void DrawTrailStrip(int start, u8 r0, u8 g0, u8 b0, u8 a0, u8 r1, u8 g1, u8 b1, u8 a1, f32 len, f32 step,
                    f32 w0, f32 w1, f32 tile);
void DrawDisableTexOffsets();

extern "C" {
f32 fn_800A1394(f32 angle);
f32 fn_800E1020(Vec3* v);
void fn_800E1098(Vec3* out, Vec3* in, f32 len);
int SendMsg(int to, int msg, void* data);
void fn_8009EC5C(void*, void*);
}

// A pooled timed line effect (0x4C bytes). Types 0/1 draw a straight beam from a placement (1 follows
// a target point); types 2/3 draw a Hermite curve from a bone of `target` to an actor (2) or a point (3).
struct LineFx {
    PlaceF place;          // 0x0C
    f32 len;               // 0x3C
    const IVec3* target;   // 0x40 (type 1)
};

struct CurveFx {
    Vec3* pts;                 // 0x0C eight curve points
    int target;                // 0x10
    int bone;                  // 0x14
    union {
        const Placement* actor;  // 0x18 (type 2)
        Vec3 pos;                // 0x18 (type 3)
    } end;
    Vec3 tan0;                 // 0x24
    Vec3 tan1;                 // 0x30
};

struct TimedFx {
    int state;             // 0x00 1 = shown, 2 = fading, 0 = finished
    int type;              // 0x04
    f32 time;              // 0x08
    union {
        LineFx line;
        CurveFx curve;
    } u;                   // 0x0C
    u8 col[6];             // 0x44 start and end colour
};

extern "C" {
void fn_8015B5B4(TimedFx* fx);
void fn_8015BA5C(f32);
}

// The file's effect list: vtables 0x803A7028 (BufferArray<TimedFx*>) and 0x803A7048 (BufferPtrArray<TimedFx>).
typedef BufferPtrArray<TimedFx> TimedFxList;

TimedFxList* gTimedFxList;      // 0x804D4EB0
f32 gTimedFxShowTime;           // 0x804D4EB4: time a new effect stays fully shown (never set: 0)
f32 gTimedFxFadeTime = 1000.0f; // 0x804D2F50: fade-out time

extern "C" {

// 0x8015AA24: destroy the emitter pool.
void fn_8015AA24() {
    while (gTimedFxList->Count() != 0) {
        TimedFx* fx = *gTimedFxList->Begin();
        if (fx->type == 2 || fx->type == 3) {
            if (fx->u.curve.pts != 0) {
                VFree(fx->u.curve.pts);
                fx->u.curve.pts = 0;
            }
        }
        VFree(fx);
        gTimedFxList->Erase((char*)gTimedFxList->Begin(), sizeof(TimedFx*));
    }
    delete gTimedFxList;
}

// 0x8015AAC4 is TimedFxList's deleting destructor, emitted here for the delete above.

// 0x8015AB3C: update every emitter.
void fn_8015AB3C() {
    TimedFx** it;

    it = gTimedFxList->Begin();
    while (it != gTimedFxList->End()) {
        fn_8015B5B4(*it);
        it++;
    }
}

// 0x8015AB90: update all emitter effects.
void fn_8015AB90(f32 dt) {
    fn_8015BA5C(dt);
}

// 0x8015ABB0: empty hook.
void fn_8015ABB0() {
}

// 0x8015ABB4: create the emitter pool. (0x8015AC08, the BufferArray<TimedFx*> destructor, follows it.)
void fn_8015ABB4() {
    gTimedFxList = new TimedFxList;
}


// Reply of the 0x402 bone query: the bone's placement.
static inline u8 LerpU8(u8 a, u8 b, f32 t) {
    return b * t + a * (1.0f - t);
}

struct BoneQuery {
    int unk00;             // 0x00
    short bone;            // 0x04
    char pad06[0x0A];
    Placement place;       // 0x10
};

// 0x8015AC70: a curve from a bone of `target` to the point pos.
int fn_8015AC70(int target, int bone, Vec3* pos, u8 r0, u8 g0, u8 b0, u8 r1, u8 g1, u8 b1) {
    TimedFx* fx;
    int i;

    while (gTimedFxList->Count() >= 8) {
        TimedFx* old = *gTimedFxList->Begin();

        if ((old->type == 2 || old->type == 3) && old->u.curve.pts != 0) {
            VFree(old->u.curve.pts);
            old->u.curve.pts = 0;
        }
        VFree(old);
        gTimedFxList->Erase((char*)gTimedFxList->Begin(), sizeof(TimedFx*));
    }
    fx = (TimedFx*)VAlloc(0x4c);
    if (fx != 0) {
    *(TimedFx**)gTimedFxList->Insert((char*)gTimedFxList->End(), sizeof(TimedFx*)) = fx;
    fx->state = 1;
    fx->time = gTimedFxShowTime;
    fx->col[0] = r0;
    fx->col[1] = g0;
    fx->col[2] = b0;
    fx->col[3] = r1;
    fx->col[4] = g1;
    fx->col[5] = b1;
    fx->u.curve.target = target;
    fx->u.curve.bone = bone;
    fx->u.curve.pts = (Vec3*)VAlloc(0x60);
    fx->type = 3;
    fx->u.curve.end.pos.x = pos->x;
    fx->u.curve.end.pos.y = pos->y;
    fx->u.curve.end.pos.z = pos->z;
    {
        BoneQuery q;
        Vec3 a;
        Vec3 b;
        Vec3 d;
        IVec3 aim;
        f32 s;
        Vec3* tan;

        q.bone = fx->u.curve.bone;
        if (SendMsg(fx->u.curve.target, 0x402, &q)) {
            a.x = q.place.pos.x;
            a.y = q.place.pos.y;
            a.z = q.place.pos.z;
            b = *pos;
            d.x = b.x - a.x;
            d.y = b.y - a.y;
            d.z = b.z - a.z;
            s = 0.25f * fn_800E1020(&d);
            ActorAim(&aim.x, (Actor*)&q.place, 0);
            fx->u.curve.tan0.x = aim.x;
            fx->u.curve.tan0.y = aim.y;
            fx->u.curve.tan0.z = aim.z;
            tan = &fx->u.curve.tan0;
            fn_800E1098(tan, tan, s);
            fn_800E1098(&fx->u.curve.tan1, &fx->u.curve.tan0, -s);
            for (i = 0; i < 8; i++) {
                SplineHermiteEval(i / 8.0f, &a, &fx->u.curve.tan0, &b, &fx->u.curve.tan1, &fx->u.curve.pts[i]);
            }
        }
    }
    } else {
        return 0;
    }
    return 1;
}

// 0x8015AF74
// a curve from a bone of `target` to the actor.
int fn_8015AF74(int target, int bone, Placement* actor, u8 r0, u8 g0, u8 b0, u8 r1, u8 g1, u8 b1) {
    TimedFx* fx;
    int i;

    while (gTimedFxList->Count() >= 8) {
        TimedFx* old = *gTimedFxList->Begin();

        if ((old->type == 2 || old->type == 3) && old->u.curve.pts != 0) {
            VFree(old->u.curve.pts);
            old->u.curve.pts = 0;
        }
        VFree(old);
        gTimedFxList->Erase((char*)gTimedFxList->Begin(), sizeof(TimedFx*));
    }
    fx = (TimedFx*)VAlloc(0x4c);
    if (fx != 0) {
    *(TimedFx**)gTimedFxList->Insert((char*)gTimedFxList->End(), sizeof(TimedFx*)) = fx;
    fx->state = 1;
    fx->time = gTimedFxShowTime;
    fx->col[0] = r0;
    fx->col[1] = g0;
    fx->col[2] = b0;
    fx->col[3] = r1;
    fx->col[4] = g1;
    fx->col[5] = b1;
    fx->u.curve.target = target;
    fx->u.curve.bone = bone;
    fx->u.curve.pts = (Vec3*)VAlloc(0x60);
    fx->type = 2;
    fx->u.curve.end.actor = actor;
    {
        BoneQuery q;
        Vec3 a;
        Vec3 b;
        Vec3 d;
        IVec3 aim;
        f32 s;
        Vec3* tan;

        q.bone = fx->u.curve.bone;
        if (SendMsg(fx->u.curve.target, 0x402, &q)) {
            a.x = q.place.pos.x;
            a.y = q.place.pos.y;
            a.z = q.place.pos.z;
            b.x = actor->pos.x;
            b.y = actor->pos.y;
            b.z = actor->pos.z;
            d.x = b.x - a.x;
            d.y = b.y - a.y;
            d.z = b.z - a.z;
            s = 0.25f * fn_800E1020(&d);
            ActorAim(&aim.x, (Actor*)&q.place, 0);
            fx->u.curve.tan0.x = aim.x;
            fx->u.curve.tan0.y = aim.y;
            fx->u.curve.tan0.z = aim.z;
            tan = &fx->u.curve.tan0;
            fn_800E1098(tan, tan, s);
            fn_800E1098(&fx->u.curve.tan1, &fx->u.curve.tan0, -s);
            for (i = 0; i < 8; i++) {
                SplineHermiteEval(i / 8.0f, &a, &fx->u.curve.tan0, &b, &fx->u.curve.tan1, &fx->u.curve.pts[i]);
            }
        }
    }
    } else {
        return 0;
    }
    return 1;
}

// 0x8015B2A0: a straight beam of length len along the placement.
int fn_8015B2A0(f32 len, const PlaceF* place, u8 r0, u8 g0, u8 b0, u8 r1, u8 g1, u8 b1) {
    TimedFx* fx;

    while (gTimedFxList->Count() >= 8) {
        TimedFx* old = *gTimedFxList->Begin();

        if ((old->type == 2 || old->type == 3) && old->u.curve.pts != 0) {
            VFree(old->u.curve.pts);
            old->u.curve.pts = 0;
        }
        VFree(old);
        gTimedFxList->Erase((char*)gTimedFxList->Begin(), sizeof(TimedFx*));
    }
    fx = (TimedFx*)VAlloc(0x4c);
    if (fx != 0) {
    *(TimedFx**)gTimedFxList->Insert((char*)gTimedFxList->End(), sizeof(TimedFx*)) = fx;
    fx->state = 1;
    fx->type = 0;
    fx->u.line.place = *place;
    fx->time = gTimedFxShowTime;
    fx->u.line.len = len;
    fx->col[0] = r0;
    fx->col[1] = g0;
    fx->col[2] = b0;
    fx->col[3] = r1;
    fx->col[4] = g1;
    fx->col[5] = b1;
    } else {
        return 0;
    }
    return 1;
}

// 0x8015B428
// a beam from the placement towards the target point (its length follows the target).
int fn_8015B428(f32 len, const PlaceF* place, const IVec3* target, u8 r0, u8 g0, u8 b0, u8 r1, u8 g1, u8 b1) {
    TimedFx* fx;

    while (gTimedFxList->Count() >= 8) {
        TimedFx* old = *gTimedFxList->Begin();

        if ((old->type == 2 || old->type == 3) && old->u.curve.pts != 0) {
            VFree(old->u.curve.pts);
            old->u.curve.pts = 0;
        }
        VFree(old);
        gTimedFxList->Erase((char*)gTimedFxList->Begin(), sizeof(TimedFx*));
    }
    fx = (TimedFx*)VAlloc(0x4c);
    if (fx != 0) {
        *(TimedFx**)gTimedFxList->Insert((char*)gTimedFxList->End(), sizeof(TimedFx*)) = fx;
    fx->state = 1;
    fx->type = 1;
    fx->u.line.place = *place;
    fx->time = gTimedFxShowTime;
    fx->u.line.len = len;
    fx->u.line.target = target;
    fx->col[0] = r0;
    fx->col[1] = g0;
    fx->col[2] = b0;
    fx->col[3] = r1;
    fx->col[4] = g1;
    fx->col[5] = b1;
    } else {
        return 0;
    }
    return 1;
}

// 0x8015B5B4: draw one effect; it fades and thins out as its time runs down.
void fn_8015B5B4(TimedFx* fx) {
    if (fx != 0) {
        f32 frac = (gTimedFxFadeTime - fx->time) / gTimedFxFadeTime;
        f32 w0 = 40.0f * frac + 8.0f;
        f32 w1 = 10.0f * frac + 8.0f;
        int start = (int)(0.5f * fx->time);
        int off;
        u8 alpha = (int)(255.0f * (1.0f - fn_800A1394(90.0f * frac)));
        f32 lineW = 8.0f * frac + 2.0f;

        if (fx->type == 0 || fx->type == 1) {
            Draw3DBeginLine(-4, &fx->u.line.place, lineW);
            DrawTrailStrip(start, fx->col[0], fx->col[1], fx->col[2], alpha, fx->col[3], fx->col[4], fx->col[5],
                           alpha, fx->u.line.len, 0.005f, w0, w0, 8.0f);
            DrawTrailStrip(start + 0xb4, fx->col[0], fx->col[1], fx->col[2], alpha, fx->col[3], fx->col[4],
                           fx->col[5], alpha, fx->u.line.len, 0.005f, w1, w1, 8.0f);
            DrawDisableTexOffsets();
        } else if (fx->type == 2 || fx->type == 3) {
            int i;

            for (i = 0, off = 0; i < 7; i++, off += 12) {
                Vec3 d;
                IVec3 id;
                Placement pl;
                PlaceF m;
                f32 t0;
                f32 t1;
                u8 ra, ga, ba, rb, gb, bb;

                Vec3* cur = (Vec3*)((u8*)fx->u.curve.pts + off);
                Vec3* next = &fx->u.curve.pts[i + 1];

                d.x = next->x - cur->x;
                d.y = next->y - cur->y;
                d.z = next->z - cur->z;
                id.x = d.x;
                id.y = d.y;
                id.z = d.z;
                fn_8009EC5C(&pl, &id);
                ActorGetMatrix((Mtx34*)&m, (Actor*)&pl);
                cur = (Vec3*)((u8*)fx->u.curve.pts + off);
                m.pos.x = cur->x;
                m.pos.y = cur->y;
                m.pos.z = cur->z;
                t0 = i / 8.0f;
                t1 = (i + 1) / 8.0f;
                ra = LerpU8(fx->col[0], fx->col[3], t0);
                ga = LerpU8(fx->col[1], fx->col[4], t0);
                ba = LerpU8(fx->col[2], fx->col[5], t0);
                rb = LerpU8(fx->col[0], fx->col[3], t1);
                gb = LerpU8(fx->col[1], fx->col[4], t1);
                bb = LerpU8(fx->col[2], fx->col[5], t1);
                Draw3DBeginLine(-4, &m, lineW);
                DrawTrailStrip(start, ra, ga, ba, alpha, rb, gb, bb, alpha, fn_800E1020(&d), 0.005f, w0, w0, 8.0f);
                DrawTrailStrip(start, ra, ga, ba, alpha, rb, gb, bb, alpha, fn_800E1020(&d), 0.005f, w0, w0, 8.0f);
                DrawDisableTexOffsets();
            }
        }
    }
}

// 0x8015BA5C: per-frame update: follow the targets, count the time down, free the finished effects.
void fn_8015BA5C(f32 dt) {
    TimedFx** it;
    int i;

    it = gTimedFxList->Begin();
    while (it != gTimedFxList->End()) {
        TimedFx* fx = *it;

        if (fx->type == 1) {
            Vec3 d;
            f32 px = fx->u.line.place.pos.x;
            f32 py = fx->u.line.place.pos.y;
            f32 pz = fx->u.line.place.pos.z;
            f32 tx = fx->u.line.target->x;
            f32 ty = fx->u.line.target->y;
            f32 tz = fx->u.line.target->z;

            d.x = tx - px;
            d.y = ty - py;
            d.z = tz - pz;
            fx->u.line.len = fn_800E1020(&d);
            fx->u.line.len = 16384.0f < fx->u.line.len ? 16384.0f : fx->u.line.len;
        } else if (fx->type == 2) {
            BoneQuery q;
            Vec3 a;
            Vec3 b;
            IVec3 aim;
            f32 s;

            q.bone = fx->u.curve.bone;
            if (SendMsg(fx->u.curve.target, 0x402, &q)) {
                a.x = q.place.pos.x;
                a.y = q.place.pos.y;
                a.z = q.place.pos.z;
                b.x = fx->u.curve.end.actor->pos.x;
                b.y = fx->u.curve.end.actor->pos.y;
                b.z = fx->u.curve.end.actor->pos.z;
                s = fn_800E1020(&fx->u.curve.tan0);
                ActorAim(&aim.x, (Actor*)&q.place, 0);
                fx->u.curve.tan0.x = aim.x;
                fx->u.curve.tan0.y = aim.y;
                fx->u.curve.tan0.z = aim.z;
                    fn_800E1098(&fx->u.curve.tan0, &fx->u.curve.tan0, s);
                for (i = 0; i < 8; i++) {
                    SplineHermiteEval(i / 8.0f, &a, &fx->u.curve.tan0, &b, &fx->u.curve.tan1,
                                      &fx->u.curve.pts[i]);
                }
            }
        } else if (fx->type == 3) {
            BoneQuery q;
            Vec3 a;
            Vec3 b;
            IVec3 aim;
            f32 s;

            q.bone = fx->u.curve.bone;
            if (SendMsg(fx->u.curve.target, 0x402, &q)) {
                a.x = q.place.pos.x;
                a.y = q.place.pos.y;
                a.z = q.place.pos.z;
                b.x = fx->u.curve.end.pos.x;
                b.y = fx->u.curve.end.pos.y;
                b.z = fx->u.curve.end.pos.z;
                s = fn_800E1020(&fx->u.curve.tan0);
                ActorAim(&aim.x, (Actor*)&q.place, 0);
                fx->u.curve.tan0.x = aim.x;
                fx->u.curve.tan0.y = aim.y;
                fx->u.curve.tan0.z = aim.z;
                    fn_800E1098(&fx->u.curve.tan0, &fx->u.curve.tan0, s);
                for (i = 0; i < 8; i++) {
                    SplineHermiteEval(i / 8.0f, &a, &fx->u.curve.tan0, &b, &fx->u.curve.tan1,
                                      &fx->u.curve.pts[i]);
                }
            }
        }
        if (fx->state == 1) {
            if (fx->time <= dt) {
                fx->time = gTimedFxFadeTime;
                fx->state = 2;
            } else {
                fx->time -= dt;
            }
        } else if (fx->state == 2) {
            if (fx->time <= dt) {
                fx->state = 0;
            } else {
                fx->time -= dt;
            }
        }
        if (fx->state == 0) {
            if ((fx->type == 2 || fx->type == 3) && fx->u.curve.pts != 0) {
                VFree(fx->u.curve.pts);
                fx->u.curve.pts = 0;
            }
            VFree(*it);
            gTimedFxList->Erase((char*)it, sizeof(TimedFx*));
        } else {
            it++;
        }
    }
}

// 0x8015BF90-0x8015C080: BufferArray<TimedFx*>'s Commit, Append, GetAlignment and range loops
// (include/game/bufarray.h), instantiated at the end of the file.
} // extern "C"

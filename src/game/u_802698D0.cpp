// u_802698D0.cpp (0x802698D0-0x8026A0F0): a list of "view sensor" objects (class 0x5F). Each one has a view frustum
// built from its placement; on an update message it looks for actors in range and inside the frustum and tells its
// own actor which one it sees. Names are descriptive guesses; the code is matched against the retail DOL.
// Boundary evidence (split from u_802690C8, 2026-11-22): the HermitSpitAmmo part before it has its own 0.0f
// (.sdata2 0x804DA140) and int->float double (0x804DA1A0); this file reads a second 0.0f (0x804DA1A8) and a second
// 0x4330 double (0x804DA1B0), so it is another translation unit. Its data: .sdata2 0x804DA1A8-0x804DA1C0 and the
// sensor list / last id in .sbss 0x804D5508-0x804D5510; it touches none of the HermitSpitAmmo .data/.rodata.

#include <dolphin/types.h>

#include <string.h>

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

// Orientation and position, as returned by the actor matrix getters.
struct Mtx34 {
    float rot[3][3];
    Vec3 pos;
};

void* VAlloc(int size);
void VFree(void* p);
extern "C" void fn_800A025C(Vec3* out, Mtx34* m, int row);

// Orientation and position, as stored at the start of every actor.
struct Placement {
    float rot[3][3];
    IVec3 pos;
};

struct Sensor {
    Placement place;        // 0x00
    int (*msg)(int msg, Sensor** data);  // 0x30
    int unk34;
    int handle;             // 0x38
    int* bounds;            // 0x3C
    int unk40;
    int unk44;
    int unk48;
    int unk4C;
    int active;             // 0x50
    int unk54;
    int unk58;
    float angle;            // 0x5C
    float range;            // 0x60
    float planes[0x1C];     // 0x64
    IVec3 boxMin;           // 0xD4
    IVec3 boxMax;           // 0xE0
    Sensor* next;           // 0xEC
    Sensor* prev;           // 0xF0
};

struct SensorActor {
    u8 pad00[0x24];
    IVec3 pos;
    u8 pad30[0x38 - 0x30];
    u32 handle;
};

struct SensorMsgData {
    Sensor* sensor;
    int arg4;
    int arg8;
    int argC;
    Placement place;
};

extern Sensor* lbl_804D5508;  // the sensor list
extern u32 lbl_804D550C;      // the last sensor id

extern "C" {
void fn_80026660(int cls, int (*handler)(int, int, void*, int), int arg);
int fn_8001F928(Sensor* actor, void* seg, u32 flags, int a, int b);
void fn_8001ED98(IVec3* pos, int range, int (*cb)(SensorActor*, Sensor*), Sensor* s, int a);
int fn_802127A0(u32 handle, int a, int b, void* c, int d, int e, int f);
int fn_80212814(u32 handle, int a, int b, int c, int d, int e);
int fn_8021284C(u32 handle);
void fn_802698D0(void);
void fn_80269944(void);
}

int ViewFrustumClassifyPoints(const int* planes, const Vec3* pts, u32* out, int count, int first);
void ViewFrustumUpdate(float* out, const Vec3* eye, const Vec3* side, const Vec3* fwd, float angle, float fov,
                       float nearD, float farD);
struct Actor;
void ActorGetMatrix(Mtx34* m, Actor* a);
int ActorPlayAnim(int actor, u32 anim, int arg);

int SensorHandler(int id, int msg, void* data, int arg);
int SensorMsg(int msg, Sensor** data);

inline Sensor* SensorFind(int id) {
    Sensor* s;

    for (s = lbl_804D5508; s != 0; s = s->next) {
        if ((s->handle & 0xFFFFFF) == id) {
            return s;
        }
    }
    return 0;
}

inline void SensorUnlink(Sensor* s) {
    if (s->prev) {
        s->prev->next = s->next;
    }
    if (s->next) {
        s->next->prev = s->prev;
    }
    if (s == lbl_804D5508) {
        lbl_804D5508 = s->next;
    }
    VFree(s);
}

void fn_802698D0(void) {
    while (lbl_804D5508) {
        Sensor* s = lbl_804D5508;
        if (s) {
            SensorUnlink(s);
        }
    }
}

void fn_80269944(void) {
    lbl_804D5508 = 0;
    fn_80026660(0x5F, SensorHandler, 0);
}

// A line-of-sight segment from the sensor to the actor, as fn_8001F928 takes it.
struct SensorSegment {
    IVec3 from;
    IVec3 to;
};

// Called for each actor in range: if it is inside the sensor's frustum and the sensor has a clear
// line to it, the sensor's actor is told.
int SensorSeeActor(SensorActor* actor, Sensor* s) {
    Vec3 apos;
    SensorSegment seg;
    u32 out;

    seg.from = s->place.pos;
    seg.to = actor->pos;
    apos.x = seg.to.x;
    apos.y = seg.to.y;
    apos.z = seg.to.z;
    if (ViewFrustumClassifyPoints((int*)s->planes, &apos, &out, 1, 0) == 0 &&
        fn_8001F928(s, &seg, 0x80000000, 0, 0) == 0) {
        ActorPlayAnim(actor->handle, 0xF949E99C, s->handle);
    }
    return 1;
}

inline void IVecSet(IVec3* v, int x, int y, int z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

inline int SensorNew(void) {
    Sensor* s = (Sensor*)VAlloc(sizeof(Sensor));

    if (s != 0) {
        lbl_804D550C++;
        memset(s, 0, sizeof(Sensor));
        s->handle = (lbl_804D550C & 0xFFFFFF) | 0x5F000000;
        s->msg = SensorMsg;
        s->unk44 = -1;
        s->bounds = &s->boxMin.x;
        s->unk4C = 0x40;
        s->active = 0;
        s->angle = 30.0f;
        s->range = 10000.0f;
        IVecSet(&s->boxMin, -100, 0, -100);
        IVecSet(&s->boxMax, 100, s->range, 100);
        s->next = lbl_804D5508;
        s->prev = 0;
        if (lbl_804D5508) {
            lbl_804D5508->prev = s;
        }
        lbl_804D5508 = s;
        return s->handle;
    }
    return 0;
}

int SensorHandler(int id, int msg, void* data, int arg) {
    SensorMsgData* d = (SensorMsgData*)data;
    Sensor* s;

    id &= 0xFFFFFF;
    d->sensor = SensorFind(id);
    switch (msg) {
    case 0x400:
        s = SensorFind(id);
        if (s != 0) {
            d->sensor = s;
            return 1;
        }
        return 0;
    case 9:
        return 0;
    case 0x414:
        s = SensorFind(id);
        if (s != 0) {
            switch (s->active) {
            case 1: {
                Mtx34 m;
                Vec3 side;
                Vec3 fwd;

                ActorGetMatrix(&m, (Actor*)s);
                fn_800A025C(&side, &m, 1);
                fn_800A025C(&fwd, &m, 2);
                ViewFrustumUpdate(s->planes, &m.pos, &fwd, &side, s->angle, 0.0f, 0.0f, s->range);
                fn_8001ED98(&s->place.pos, (int)s->range, SensorSeeActor, s, 0);
                break;
            }
            }
            return 1;
        }
        return 0;
    case 0x415:
        s = SensorFind(id);
        if (s != 0) {
            if (d->arg4 != -1) {
                if (d->argC != 0) {
                    return fn_802127A0(s->handle, d->arg4, d->arg8, &d->place, 1, 0, 0);
                }
                return fn_80212814(s->handle, d->arg4, d->arg8, 1, 0, 0);
            }
            return fn_8021284C(s->handle);
        }
        return 0;
    case 0x411:
        s = SensorFind(id);
        if (s != 0) {
            d->place = s->place;
            return 1;
        }
        return 0;
    case 0x413:
        s = SensorFind(id);
        if (s != 0) {
            s->place = *(Placement*)d->arg4;
            return 1;
        }
        return 0;
    case 0x41D:
        s = SensorFind(id);
        switch (d->arg4) {
        case 0x210E6C:
            if (s) {
                SensorUnlink(s);
            }
            break;
        case 0x7A281983:
            if (d->argC) {
                s->active = 1;
            } else {
                s->active = 0;
            }
            break;
        case 0x110AD:
            s->angle = d->argC;
            break;
        case 0x3FD77BF5:
            s->range = d->argC;
            IVecSet(&s->boxMax, 100, s->range, 100);
            break;
        default:
            return 0;
        }
        return 1;
    case 0x41E:
        switch (d->arg4) {
        case 0x12D80:
            d->argC = SensorNew();
            break;
        default:
            return 0;
        }
        return 1;
    }
    return 0;
}

int SensorMsg(int msg, Sensor** data) {
    return SensorHandler((*data)->handle, msg, data, 0);
}

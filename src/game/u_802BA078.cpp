// u_802BA078.cpp: host ride action state machine (0x802BA078-0x802BB1F4).
// The action layer a possessable host runs while a ghost is riding it: a two-entry
// dispatch table (ride / ride-forward), a VAlloc'd 0x34-byte action state block,
// the action name query handler, and the per-frame update that animates the model
// and rebuilds its placement matrix, including the roll/turn/tilt motion controller.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct Mtx33 {
    float m[3][3];
};

// Orientation and position. The matrix helpers write a full 3x4 over rot, running into pos;
// pos is then filled in by the caller.
struct PlaceF {
    Mtx33 rot;
    Vec3 pos;
};

// Root bone: the translation column of its matrix lives at 0x10.
struct Bone {
    char pad0[0x10];
    float x, y, z;
};

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    virtual void Release(char* p);
};

struct Actor;

// Per-actor action state (VAlloc'd, 0x34 bytes).
struct Beh {
    int state;          // 0x00
    int next;           // 0x04
    int anim;           // 0x08
    int prevAnim;       // 0x0C
    int unk10;          // 0x10
    int unk14;          // 0x14
    float roll;         // 0x18: roll angle
    float f1C;          // 0x1C: motion accumulator
    float f20;          // 0x20
    float f24;          // 0x24
    float lastX;        // 0x28: previous frame's motion
    float lastY;        // 0x2C
    float lastZ;        // 0x30
};

// The host's per-type definition data (at actor+0xC4).
struct HostData {
    char pad0[8];
    float f8;           // 0x08: motion clamp
    char padC[8];
    int i14;            // 0x14
    int i18;            // 0x18
    int i1C;            // 0x1C
    char pad20[0xA0];
    float turnRate;     // 0xC0
    char padC4[0x2C];
    float fF0;          // 0xF0
    char padF4[0xC];
    float f100;         // 0x100
};

// One entry of the action dispatch table (0x18 bytes).
struct RideActionType {
    u32 type;                                   // 0x00
    int (*enter)(Actor* actor);                 // 0x04
    int (*setState)(Actor* actor, int state);   // 0x08
    int (*getState)(Actor* actor);              // 0x0C
    int (*event)(int msg, Actor* actor);        // 0x10
    const char* name;                           // 0x14
};

// Query block handed to the message handler: the queried actor, the query hash, and the
// caller-provided answer string.
struct RideMsg {
    Actor* actor;       // 0x00
    int hash;           // 0x04
    int unk08;          // 0x08
    Buffer result;      // 0x0C
};

struct Actor {
    char pad00[0x24];
    int i24;            // 0x24
    int i28;            // 0x28
    int i2C;            // 0x2C
    char pad30[8];
    int handle;         // 0x38
    char pad3C[0x48];
    float f84;          // 0x84
    float f88;          // 0x88
    float f8C;          // 0x8C
    char pad90[0x34];
    HostData* data;     // 0xC4
    int unkC8;          // 0xC8
    int model;          // 0xCC
    int boneCount;      // 0xD0
    int anim;           // 0xD4
    char padD8[0x18];
    char* bones;        // 0xF0
    char padF4[0x14];
    int unk108;         // 0x108
    char pad10C[4];
    int cb110;          // 0x110
    char pad114[4];
    int cb118;          // 0x118
    char pad11C[0x18];
    Beh* beh;           // 0x134
    float f138;         // 0x138
    char pad13C[0xE4];
    int unk220;         // 0x220
    char pad224[0x1C];
    float f240;         // 0x240
    float f244;         // 0x244
    float f248;         // 0x248
    Vec3 euler;         // 0x24C
    char pad258[0x18];
    char conditions[0xC];   // 0x270
    float f27C;         // 0x27C
    char pad280[4];
    float f284;         // 0x284
};

// Mangled C++ callees.
int ModelAnimate(int model, char* bones, int a);               // 0x8003AA9C
void AnimApply(int anim, char* bones, int a, int count);       // 0x8008F630
void ModelSetMatrix(int model, Mtx34* m, char* bones);         // 0x8003A66C
void ActorGetMatrix(Mtx34* m, Actor* a);                       // 0x8009ED70
void BoneGetMatrix(Mtx34* m, void* bone);                      // 0x800A0EF0
void MtxGetEuler(Vec3* rot, Mtx34* m);                         // 0x8009FB94
void MtxSetEuler(Mtx34* m, Vec3* rot);                         // 0x8009FAC0
void PlacementToMatrix(Mtx34* m, PlaceF* p);                   // 0x8009EF04
void EulerNormalize(Vec3* in, Vec3* out);                      // 0x8016D55C
void Vec3Set(Vec3* v, float x, float y, float z);              // 0x8016D67C
int HasCondition(void* conditions, int which);                 // 0x800A7AC4
void* VAlloc(int size);                                        // 0x8009DA68
void VFree(void* p);                                           // 0x8009DA3C
int ActorPlayAnim(int actor, u32 anim, int arg);               // 0x800C5654

extern "C" {
int fn_8008EA9C(int anim, int a);
void fn_8008F230(int anim, float f, int a, int b);
void fn_8008F378(int anim);
void fn_8008F4E8(int anim, int a);
void fn_8008F4EC(int anim, int a);
void fn_80090974(int anim, int a, int h, float f, int b, int (*cb)(int, Actor*), Actor* owner);
void fn_800913EC(int anim, float scale, Vec3* out);
int fn_800A1BC8(int a, int b, u32 type, int c, int d);
void fn_800A0168(Mtx34* out, Mtx34* in);
void fn_800A0030(Vec3* out, Mtx34* m, Vec3* in);
void fn_800A025C(Vec3* out, void* m, int row);
void fn_800A7A54(void* conditions, int which, int value);
int fn_8008E964(int anim);
int fn_8008E7D0(int anim);
float fn_800CC2D0(Actor* a, int b, int c, int d);
float __fabsf(float f);
String fn_800225C8(const String& a, const String& b);
void fn_8028C188(Buffer* b, char* data, int len);
void fn_8028BF80(String* s, const char* format, ...);
}

int RideActionMsg(int msg, void* data);
int RideActionUpdate(Actor* actor);
void RideActionDestroy(Actor* actor);
void RideActionInit(int unused, Actor* actor);
int RideActionEvent(int msg, Actor* actor);
int RideActionSetState(Actor* actor, int state);
int RideActionGetState1(Actor* actor);
int RideActionGetState0(Actor* actor);
int RideActionEnter1(Actor* actor);
int RideActionEnter0(Actor* actor);
void RideActionAnimate(Actor* actor);

RideActionType gRideActionType[] = {
    { 0x3333CFE5, RideActionEnter0, RideActionSetState, RideActionGetState0, RideActionEvent, "" },
    { 0x806E6EB5, RideActionEnter1, RideActionSetState, RideActionGetState1, RideActionEvent, "" },
};

int RideActionMsg(int msg, void* data) {
    RideMsg* d = (RideMsg*)data;

    switch (msg) {
    case 0xA:
        return 1;
    case 0x41E: {
        Beh* b = d->actor->beh;
        switch (d->hash) {
        case 0xAD8576FA: {
            String roll;
            String name(gRideActionType[b->state].name);
            String newline("\n");
            const String& text = fn_800225C8(name, newline);
            fn_8028C188(&d->result, text.mData, text.mLength);
            fn_8028BF80(&roll, "RollRate: %f\n", b->roll);
            fn_8028C188(&d->result, roll.mData, roll.mLength);
            return 0;
        }
        }
        return 0;
    }
    }
    return 0;
}

int RideActionUpdate(Actor* actor) {
    Beh* b;
    int state;

    if (!actor) return 0;

    b = actor->beh;
    if ((fn_8008EA9C(actor->anim, 0) == 0 || fn_8008EA9C(actor->anim, 0) == 2) &&
        !HasCondition(actor->conditions, 0xD) && !HasCondition(actor->conditions, 0xA)) {
        gRideActionType[b->state].enter(actor);
    }
    if (actor->unk220 == 4) {
        RideActionAnimate(actor);
        state = b->state;
    } else {
        state = gRideActionType[b->state].getState(actor);
    }
    if (state != b->state && gRideActionType[b->state].setState(actor, state)) {
        gRideActionType[state].enter(actor);
    }
    b->lastX = actor->f240;
    b->lastY = actor->f244;
    b->lastZ = actor->f248;
    return 1;
}

void RideActionDestroy(Actor* actor) {
    actor->cb110 = 0;
    VFree(actor->beh);
    actor->beh = 0;
}

void RideActionInit(int unused, Actor* actor) {
    Beh* b;

    if (!actor) return;
    b = (Beh*)VAlloc(0x34);
    if (!b) {
        actor->beh = 0;
        return;
    }
    actor->beh = b;
    actor->cb110 = (int)RideActionUpdate;
    actor->cb118 = 0;
    actor->unk108 = 0;
    b->state = 0;
    b->next = 0;
    b->roll = 0.0f;
}

int RideActionEvent(int msg, Actor* actor) {
    if (!actor) return 0;
    switch (msg) {
    case 0x1B83833F: {
        Beh* b = actor->beh;
        gRideActionType[b->state].setState(actor, b->next);
        gRideActionType[b->next].enter(actor);
    }
    }
    return 1;
}

int RideActionSetState(Actor* actor, int state) {
    if (!actor) return 0;
    actor->beh->next = state;
    return 1;
}

int RideActionGetState1(Actor* actor) {
    Beh* b;

    if (!actor) return 0;
    b = actor->beh;
    RideActionAnimate(actor);
    return b->state;
}

int RideActionGetState0(Actor* actor) {
    Beh* b = actor->beh;

    RideActionAnimate(actor);
    if (HasCondition(actor->conditions, 4)) {
        fn_800A7A54(actor->conditions, 4, 0);
        return 1;
    }
    return b->state;
}

static inline void ResetAction(Actor* actor) {
    Beh* b;

    if (!actor) return;
    b = actor->beh;
    b->f1C = 0.0f;
    b->f20 = 0.0f;
    b->f24 = 0.0f;
    if (HasCondition(actor->conditions, 0xC)) {
        fn_800A7A54(actor->conditions, 0xC, 0);
        fn_800A7A54(actor->conditions, 0xE, 1);
        fn_8008F4E8(actor->anim, 0);
        fn_8008F378(actor->anim);
        fn_8008F4EC(actor->anim, 0);
    }
    b->anim = fn_800A1BC8(actor->data->i14, actor->data->i1C, gRideActionType[b->state].type, 0, 0);
    if (fn_8008E7D0(b->anim)) {
        fn_800A7A54(actor->conditions, 0x1C, 0);
    }
    fn_80090974(actor->anim, 0, b->anim, 1.0f, 9, gRideActionType[b->state].event, actor);
    if (HasCondition(actor->conditions, 0xE)) {
        fn_8008F230(actor->anim, 0.0f, 0, 0);
        fn_800A7A54(actor->conditions, 0xE, 0);
    } else if (b->prevAnim == b->anim) {
        fn_8008F230(actor->anim, 0.0f, 0, 0);
    } else {
        fn_8008F230(actor->anim, 150.0f, 0, 1);
    }
}

int RideActionEnter1(Actor* actor) {
    Beh* b;

    if (!actor) return 0;
    b = actor->beh;
    b->state = 1;
    b->next = 0;
    b->prevAnim = b->anim;
    ResetAction(actor);
    return 1;
}

int RideActionEnter0(Actor* actor) {
    Beh* b;

    if (!actor) return 0;
    b = actor->beh;
    b->state = 0;
    b->next = 0;
    b->prevAnim = b->anim;
    ResetAction(actor);
    return 1;
}

static inline float ClampMotion(float value, float limit) {
    return value < -limit ? -limit : (value > limit ? limit : value);
}

static inline float TurnRate(Actor* actor) {
    return actor->data->turnRate * actor->f138 / 1000.0f;
}

void RideActionAnimate(Actor* actor) {
    PlaceF place;
    Mtx34 matrix;
    Mtx34 motion;
    Mtx34 basis;
    Vec3 rot;
    Vec3 delta;
    Vec3 axis;
    Beh* b = actor->beh;

    if (HasCondition(actor->conditions, 0x1C) ||
        fn_8008EA9C(actor->anim, 0) == 0 ||
        fn_8008EA9C(actor->anim, 0) == 2) {
        if (!HasCondition(actor->conditions, 0xD) &&
            !HasCondition(actor->conditions, 9) &&
            !HasCondition(actor->conditions, 0xA) &&
            !HasCondition(actor->conditions, 0xB)) {
            gRideActionType[b->state].enter(actor);
        }
        if (HasCondition(actor->conditions, 0x1C) && fn_8008E964(actor->anim)) {
            ActorPlayAnim(actor->handle, 0xB2CB0AA4, actor->handle);
        }
        if (HasCondition(actor->conditions, 0x1C) && HasCondition(actor->conditions, 0xD) &&
            fn_8008E964(actor->anim)) {
            ActorPlayAnim(actor->handle, 0xB2CB0AA4, actor->handle);
            fn_800A7A54(actor->conditions, 0x1C, 0);
            fn_800A7A54(actor->conditions, 0x1D, 1);
        }
    }
    fn_800913EC(actor->anim, actor->f138, &delta);
    int anim = ModelAnimate(actor->model, actor->bones, 1);
    AnimApply(actor->anim, actor->bones, anim, actor->boneCount);

    if (!HasCondition(actor->conditions, 0xD) && !HasCondition(actor->conditions, 0xC)) {
        actor->euler.x = ClampMotion(actor->euler.x, actor->data->f8);
        Bone* bone = (Bone*)actor->bones;
        bone->x = 0.0f;
        bone->y = 0.0f;
        bone->z = 0.0f;
        if (HasCondition(actor->conditions, 0)) {
            b->f1C += delta.x;
            b->f20 += delta.y;
            b->f24 += delta.z;
            ActorGetMatrix(&matrix, actor);
            fn_800A0168(&matrix, &matrix);
            fn_800A0030(&delta, &matrix, &delta);
            actor->f240 = delta.x;
            actor->f244 = delta.y;
            actor->f248 = delta.z;
            actor->f248 = 0.0f;
            float scale = 1000.0f / actor->f138;
            actor->f240 *= scale;
            actor->f244 *= scale;
            actor->f248 *= scale;
        } else {
            float turn = actor->f284;
            float tilt = actor->f27C;
            MtxSetEuler(&motion, &actor->euler);
            fn_800A025C(&axis, &motion, 0);
            float value = (axis.x * actor->f240 + axis.y * actor->f244 + axis.z * actor->f248
                + (axis.x * (actor->f240 - b->lastX) + axis.y * (actor->f244 - b->lastY)
                   + axis.z * (actor->f248 - b->lastZ)) / (0.00025f * actor->f138))
                / (4.0f * actor->data->f100 + actor->data->fF0);
            float target = 50.0f * value;
            float base = target - actor->euler.y;
            float scaled = 1.5f * base;
            float error = scaled - b->roll;
            float amount = 2.0f * __fabsf(error);
            if (error > 1.1920929e-07f) {
                b->roll += amount * actor->f138 / 1000.0f;
            } else if (error < -1.1920929e-07f) {
                b->roll += -amount * actor->f138 / 1000.0f;
            }
            if (turn > 1.1920929e-07f) {
                float rate = TurnRate(actor);
                actor->euler.z += rate < turn ? rate : turn;
            } else if (turn < -1.1920929e-07f) {
                float rate = TurnRate(actor);
                actor->euler.z += -rate > turn ? -rate : turn;
            }
            if (tilt > 1.1920929e-07f) {
                float rate = TurnRate(actor);
                actor->euler.x += rate < tilt ? rate : tilt;
            } else if (tilt < -1.1920929e-07f) {
                float rate = TurnRate(actor);
                actor->euler.x += -rate > tilt ? -rate : tilt;
            }
            actor->euler.y += b->roll * actor->f138 / 1000.0f;
        }
        EulerNormalize(&actor->euler, &actor->euler);
        Vec3Set(&rot, actor->euler.z, actor->euler.x, actor->euler.y);
        EulerNormalize(&rot, &rot);
        MtxSetEuler((Mtx34*)&place.rot, &rot);
        place.pos.x = (float)actor->i24;
        place.pos.y = (float)actor->i28;
        place.pos.z = (float)actor->i2C;
        PlacementToMatrix((Mtx34*)actor, &place);
        ModelSetMatrix(actor->model, (Mtx34*)&place, actor->bones);
    } else if (HasCondition(actor->conditions, 0xC)) {
        fn_800CC2D0(actor, 1, 0, 0);
        if (fn_8008EA9C(actor->anim, 0) != 0) {
            if (HasCondition(actor->conditions, 0xC)) {
                BoneGetMatrix((Mtx34*)&place, actor->bones);
                MtxGetEuler(&actor->euler, (Mtx34*)&place);
                PlacementToMatrix((Mtx34*)actor, &place);
                Bone* bone = (Bone*)actor->bones;
                actor->f84 = bone->x;
                actor->f88 = bone->y;
                actor->f8C = bone->z;
                bone = (Bone*)actor->bones;
                actor->i24 = (int)bone->x;
                actor->i28 = (int)bone->y;
                actor->i2C = (int)bone->z;
            } else {
                Vec3Set(&rot, actor->euler.z, 0.0f, 0.0f);
                EulerNormalize(&rot, &rot);
                MtxSetEuler((Mtx34*)&place, &rot);
                place.pos.x = (float)actor->i24;
                place.pos.y = (float)actor->i28;
                place.pos.z = (float)actor->i2C;
                PlacementToMatrix((Mtx34*)actor, &place);
            }
        }
        ModelSetMatrix(actor->model, 0, actor->bones);
    } else {
        float t = fn_800CC2D0(actor, 1, 0, 0);
        if (t != -999999.0f) {
            actor->f8C = t + ((Bone*)actor->bones)->z;
            ((Bone*)actor->bones)->z = 0.0f;
        }
        actor->i2C = (int)actor->f8C;
        Bone* bone = (Bone*)actor->bones;
        bone->x = 0.0f;
        bone->y = 0.0f;
        bone->z = 0.0f;
        ActorGetMatrix(&basis, actor);
        fn_800A0168(&basis, &basis);
        fn_800A0030(&delta, &basis, &delta);
        actor->f240 = delta.x;
        actor->f244 = delta.y;
        actor->f248 = delta.z;
        actor->f248 = 0.0f;
        float scale = 1000.0f / actor->f138;
        actor->f240 *= scale;
        actor->f244 *= scale;
        actor->f248 *= scale;
        Vec3Set(&rot, actor->euler.z, 0.0f, 0.0f);
        EulerNormalize(&rot, &rot);
        MtxSetEuler((Mtx34*)&place, &rot);
        place.pos.x = (float)actor->i24;
        place.pos.y = (float)actor->i28;
        place.pos.z = (float)actor->i2C;
        PlacementToMatrix((Mtx34*)actor, &place);
        ModelSetMatrix(actor->model, (Mtx34*)&place, actor->bones);
    }
}

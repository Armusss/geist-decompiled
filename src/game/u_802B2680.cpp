// u_802B2680.cpp: host action state machine (0x802B2680-0x802B3430).
// The component a possessable host runs while a ghost is riding it. It owns a VAlloc'd
// 0x24-byte action state block (current/next action, current/previous animation, motion
// accumulator), a one-entry dispatch table (enter / set-next / get-current / event), the
// action query handler that reports the action name back to the ghost, and the per-frame
// update that animates the model and rebuilds its placement matrix.
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
    ~String() {}
    virtual void Release(char* p);
};

struct Actor;
struct Owner;

// Per-actor action state (VAlloc'd, 0x24 bytes).
struct Beh {
    int state;          // 0x00
    int next;           // 0x04
    int anim;           // 0x08
    int prevAnim;       // 0x0C
    int unk10;          // 0x10
    int unk14;          // 0x14
    float f18;          // 0x18
    float f1C;          // 0x1C
    float f20;          // 0x20
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
    float fC0;          // 0xC0: turn rate
};

// One entry of the action dispatch table (0x18 bytes).
struct HostActionType {
    u32 type;                                   // 0x00
    int (*enter)(Actor* actor);                 // 0x04
    int (*setState)(Actor* actor, int state);   // 0x08
    int (*getState)(Actor* actor);              // 0x0C
    int (*event)(int msg, Actor* actor);        // 0x10
    const char* name;                           // 0x14
};

// Query block handed to the message handler: the queried actor, the query hash, and the
// caller-provided answer string.
struct HostMsg {
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
    char pad30[0x54];
    float f84;          // 0x84
    float f88;          // 0x88
    float f8C;          // 0x8C
    char pad90[0x14];
    void* a4;           // 0xA4
    char padA8[0x1C];
    HostData* data;     // 0xC4
    int unkC8;          // 0xC8
    int model;          // 0xCC
    int boneCount;      // 0xD0
    int anim;           // 0xD4
    char padD8[0x18];
    char* bones;        // 0xF0
    char padF4[0x1C];
    int cb110;          // 0x110
    char pad114[4];
    int cb118;          // 0x118
    char pad11C[0x18];
    Beh* beh;           // 0x134
    float f138;         // 0x138
    float f13C;         // 0x13C
    char pad140[0xE0];
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
int OwnerId(Owner* o);                                         // 0x800A3A40
void* VAlloc(int size);                                        // 0x8009DA68
void VFree(void* p);                                           // 0x8009DA3C

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
void fn_800A7A54(void* conditions, int which, int value);
float fn_800CC2D0(Actor* a, int b, int c, int d);
int fn_800CDD10();
int fn_80158AFC(int owner, void* data);
int fn_800AFD20(Actor* a);
int fn_8003A1F0(int a, void* bones, int b);
void fn_8003F15C(int a, int b, int c, float f, void* d, int e);
String fn_800225C8(const String& a, const String& b);
void fn_8028C188(Buffer* b, char* data, int len);
}

int HostActionMsg(int msg, void* data);
int HostActionUpdate(Actor* actor);
void HostActionDestroy(Actor* actor);
void HostActionInit(int unused, Actor* actor);
int HostActionEvent(int msg, Actor* actor);
int HostActionSetState(Actor* actor, int state);
int HostActionGetState(Actor* actor);
int HostActionEnter(Actor* actor);
void HostActionAnimate(Actor* actor);
void HostActionFree(Actor* actor);

HostActionType gHostActionType[] = {
    { 0x3333CFE5, HostActionEnter, HostActionSetState, HostActionGetState, HostActionEvent, "" },
};

// Poses the actor's model from its animation into the message buffer.
inline void HostPose(Actor* actor, HostMsg* d) {
    int owner = fn_800AFD20(actor);
    int anim = ModelAnimate(actor->model, 0, 0);
    anim = fn_8003A1F0(owner, actor->bones, anim);
    fn_8003F15C(owner, anim, actor->boneCount, 1.0f, &d->result.mData, 0);
    actor->f13C = 0.0f;
}

int HostActionMsg(int msg, void* data) {
    HostMsg* d = (HostMsg*)data;
    int anim;
    Actor* actor;
    int owner;

    switch (msg) {
    case 0:
        actor = d->actor;
        if (!actor) {
            return 1;
        }
        if (fn_80158AFC(OwnerId((Owner*)actor), d)) {
            return 1;
        }
        HostPose(actor, d);
        return 1;
    case 0x7300:
        actor = d->actor;
        if (!actor) {
            return 1;
        }
        HostPose(actor, d);
        return 1;
    case 0x41E: {
        Beh* b = d->actor->beh;
        switch (d->hash) {
        case 0xAD8576FA: {
            String name(gHostActionType[b->state].name);
            String newline("\n");
            const String& text = fn_800225C8(name, newline);
            fn_8028C188(&d->result, text.mData, text.mLength);
            return 0;
        }
        }
        return 0;
    }
    }
    return 0;
}

int HostActionUpdate(Actor* actor) {
    if (!actor) {
        return 0;
    }
    Beh* b = actor->beh;
    if ((fn_8008EA9C(actor->anim, 0) == 0 || fn_8008EA9C(actor->anim, 0) == 2) &&
        !HasCondition(actor->conditions, 0xD) && !HasCondition(actor->conditions, 0xA)) {
        gHostActionType[b->state].enter(actor);
    }
    int state;
    if (actor->unk220 == 4) {
        HostActionAnimate(actor);
        state = b->state;
    } else {
        state = gHostActionType[b->state].getState(actor);
    }
    if (state != b->state && gHostActionType[b->state].setState(actor, state)) {
        gHostActionType[state].enter(actor);
    }
    return 1;
}

void HostActionDestroy(Actor* actor) {
    actor->cb110 = 0;
    VFree(actor->beh);
    actor->beh = 0;
}

void HostActionInit(int unused, Actor* actor) {
    if (!actor) {
        return;
    }
    Beh* b = (Beh*)VAlloc(0x24);
    if (!b) {
        actor->beh = 0;
    } else {
        actor->beh = b;
        actor->cb110 = (int)HostActionUpdate;
        actor->cb118 = (int)fn_800CDD10;
        b->state = 0;
        b->next = 0;
    }
}

int HostActionEvent(int msg, Actor* actor) {
    if (!actor) {
        return 0;
    }
    switch (msg) {
    case 0x1B83833F: {
        Beh* b = actor->beh;
        gHostActionType[b->state].setState(actor, b->next);
        gHostActionType[b->next].enter(actor);
    }
    }
    return 1;
}

int HostActionSetState(Actor* actor, int state) {
    if (!actor) {
        return 0;
    }
    actor->beh->next = state;
    return 1;
}

int HostActionGetState(Actor* actor) {
    Beh* b = actor->beh;
    HostActionAnimate(actor);
    return b->state;
}

static inline void ResetAction(Actor* actor) {
    Beh* b;

    if (!actor) {
        return;
    }
    b = actor->beh;
    b->f18 = 0.0f;
    b->f1C = 0.0f;
    b->f20 = 0.0f;
    if (HasCondition(actor->conditions, 0xC)) {
        fn_800A7A54(actor->conditions, 0xC, 0);
        fn_800A7A54(actor->conditions, 0xE, 1);
        fn_8008F4E8(actor->anim, 0);
        fn_8008F378(actor->anim);
        fn_8008F4EC(actor->anim, 0);
    }
    b->anim = fn_800A1BC8(actor->data->i14, actor->data->i1C, gHostActionType[b->state].type, 0, 0);
    fn_80090974(actor->anim, 0, b->anim, 1.0f, 9, gHostActionType[b->state].event, actor);
    if (HasCondition(actor->conditions, 0xE)) {
        fn_8008F230(actor->anim, 0.0f, 0, 0);
        fn_800A7A54(actor->conditions, 0xE, 0);
    } else if (b->prevAnim == b->anim) {
        fn_8008F230(actor->anim, 0.0f, 0, 0);
    } else {
        fn_8008F230(actor->anim, 150.0f, 0, 1);
    }
}

int HostActionEnter(Actor* actor) {
    Beh* b;

    if (!actor) {
        return 0;
    }
    b = actor->beh;
    b->state = 0;
    b->next = 0;
    b->prevAnim = b->anim;
    ResetAction(actor);
    return 1;
}

static inline float ClampMotion(float value, float limit) {
    float lo = -limit;
    return value < lo ? lo : (value > limit ? limit : value);
}

static inline float TurnRate(Actor* actor) {
    return actor->data->fC0 * actor->f138 / 1000.0f;
}

void HostActionAnimate(Actor* actor) {
    PlaceF place;
    Mtx34 motion;
    Mtx34 matrix;
    Vec3 rot;
    Vec3 delta;

    Beh* b = actor->beh;
    fn_800913EC(actor->anim, actor->f138, &delta);
    int anim = ModelAnimate(actor->model, actor->bones, 1);
    AnimApply(actor->anim, actor->bones, anim, actor->boneCount);

    if (!HasCondition(actor->conditions, 0xD) && !HasCondition(actor->conditions, 0xC)) {
        actor->euler.x = ClampMotion(actor->euler.x, actor->data->f8);
        float t = fn_800CC2D0(actor, 1, 0, 0);
        if (t != -999999.0f) {
            actor->f8C = 200.0f + (t + ((Bone*)actor->bones)->z);
        }
        actor->i2C = (int)actor->f8C;
        {
            Bone* bone = (Bone*)actor->bones;
            bone->x = 0.0f;
            bone->y = 0.0f;
            bone->z = 0.0f;
        }
        if (HasCondition(actor->conditions, 0)) {
            b->f18 += delta.x;
            b->f1C += delta.y;
            b->f20 += delta.z;
            ActorGetMatrix(&motion, actor);
            fn_800A0168(&motion, &motion);
            fn_800A0030(&delta, &motion, &delta);
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
        }
        EulerNormalize(&actor->euler, &actor->euler);
        Vec3Set(&rot, actor->euler.z, 0.0f, 0.0f);
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
        {
            Bone* bone = (Bone*)actor->bones;
            bone->x = 0.0f;
            bone->y = 0.0f;
            bone->z = 0.0f;
        }
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

void HostActionFree(Actor* actor) {
    if (actor->a4) {
        VFree(actor->a4);
        actor->a4 = 0;
    }
}

// u_8016C244.cpp: the "collect_point" ghost behaviour (a ghost collectible that a nearby player picks
// up) plus the 12-bit fixed-point angle helpers it uses. The behaviour's descriptor (enter/done/update/
// name) lives in .data; the action-name string pool is the same header table every actor file carries.
// Reconstructed from the retail DOL; names are descriptive guesses.

typedef unsigned char u8;
typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

// Per-actor behaviour state, allocated by CollectInit.
struct Beh {
    int state;     // 0x00
    int id;        // 0x04
    Vec3 pos;      // 0x08
    float timer;   // 0x14
    int unk18;     // 0x18
};

// The ghost object the actor is attached to (owner->obj).
struct GhostObj {
    int (*setId)(int, void*);   // 0x000
    void* owner;                // 0x004
    char pad8[0xD4];            // 0x008
    int (*msg)(int, void*);     // 0x0DC
    char padE0[0x108];          // 0x0E0
    int zone;                   // 0x1E8
};

struct Desc {
    char pad0[0x30];
    int unk30;
};

struct Actor;
struct Owner;

struct Actor {
    char pad0[0x24];                // 0x000
    IVec3 pos;                      // 0x024
    int (*handler)(int, void*);     // 0x030
    char pad34[4];                  // 0x034
    u32 handle;                     // 0x038
    char pad3C[0x48];               // 0x03C
    char f84[0x18];                 // 0x084
    char pad9C[0x28];               // 0x09C
    Desc* desc;                     // 0x0C4
    char padC8[4];                  // 0x0C8
    int model;                      // 0x0CC
    int anim;                       // 0x0D0
    char padD4[0x1C];               // 0x0D4
    int bone;                       // 0x0F0
    char padF4[8];                  // 0x0F4
    void (*fc)();                   // 0x0FC
    char pad100[4];                 // 0x100
    int f104;                       // 0x104
    char pad108[0x20];              // 0x108
    u32 type;                       // 0x128
    char pad12C[4];                 // 0x12C
    Beh* beh;                       // 0x130
    char pad134[4];                 // 0x134
    float f138;                     // 0x138
    float f13c;                     // 0x13C
    char pad140[0x130];             // 0x140
    char cond[4];                   // 0x270
    char pad274[0x50];              // 0x274
    int f2c4;                       // 0x2C4
    char pad2C8[8];                 // 0x2C8
    int f2d0;                       // 0x2D0
    char pad2D4[0x318];             // 0x2D4
    GhostObj* obj;                  // 0x5EC
    char pad5F0[0x308];             // 0x5F0
    int f8f8;                       // 0x8F8
    char pad8FC[4];                 // 0x8FC
    float f900;                     // 0x900
    float f904;                     // 0x904
};

// Growable character buffer (support code at 0x8028B1E4-0x8028C5xx).
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

// Message body shared by the collect-point queries: the receiver answers by writing the id of the
// responding owner back over unk4 and setting unkC.
struct Msg {
    Actor* obj;      // 0x00
    u32 id;          // 0x04
    int unk8;        // 0x08
    int unkC;        // 0x0C
    float f10;       // 0x10
    float f14;       // 0x14
    int unk18;       // 0x18
    int unk1C;       // 0x1C
};

// The same message seen by the 0x41E handler, whose payload is a name string.
struct MsgName {
    Actor* obj;      // 0x00
    u32 id;          // 0x04
    int unk8;        // 0x08
    String name;     // 0x0C
};

// "What are you" query: the receiver overwrites the String with its answer (see Credits.cpp).
struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

typedef int (*BehFn)(void*);

struct BehType {
    BehFn enter;
    BehFn done;
    BehFn update;
    const char* name;
};

extern "C" {
int rand(void);
double sqrt(double);
int SendMsg(int target, int msg, void* data);                     // 0x800265D4
int fn_80117FB4(Actor* actor);
void fn_8011818C(Actor* actor);
void fn_80118250(Actor* actor, u32 id);
void fn_80118084(GhostObj* obj, u32 id, void* data);
int fn_80117FF8(GhostObj* obj, u32 id, void* data);
int fn_80116D40(Actor* actor, void* msg);
int fn_80019F88(void* msg, Actor* actor);
void fn_800A3210(Actor* actor);
int fn_800AFD20(Actor* actor);
int fn_8003A1F0(int anim, int bone, int model);
void fn_8003F15C(int bone, void* mtx, int anim, float blend, void* data, int unk);
void fn_800CE96C();
int fn_80117D80(Actor* actor);
void fn_800A7A54(void* cond, int id, int value);
void fn_800A7B08(void* cond, int id, int value);
void fn_800B6CAC(Actor* actor, void* fn);
void fn_800B5028();
void fn_800A31A0(Actor* actor);
void fn_8001ED98(void* pos, int id, int (*cb)(Actor*, Actor*), Actor* actor, int unk);
void fn_800B42E0(void* a, int b, float f, int c, int d);
int fn_800BB218(Actor* actor, int flags, Vec3* from, Vec3* to, Actor* actor2,
                float* scale, float* unk1, float* unk2, float dist);
void fn_8009F63C(void* mtx, Actor* actor);
void fn_800E0E9C(IVec3* out, IVec3* in, int scale);
void fn_8009F44C(IVec3* out, Mtx34* mtx, IVec3* in);
void fn_8016D430(IVec3* out, int a, int b, int c);
void fn_8016D330(IVec3* out, IVec3* in);
int fn_800A1558(int a, int b);
int fn_800A1688(int a);
int fn_800A1624(int a);
int fn_801BD694(Actor* actor);
int fn_80116E08(Actor* actor);
float fn_80120C14(u32 hash, int a, int h);
void fn_80120C80(float f, u32 hash, int a, int h);
void* FxFind(const char* name);
void FxStart(void* fx, int a, int b);
int fn_8001F928(Actor* actor, int* coords, u32 flags, int (*cb)(u32*, u32), Actor* actor2);
void fn_8028C188(String* dst, char* data, int len);
void* VAlloc(int size);
void VFree(void* ptr);
void* memset(void* dst, int val, unsigned long n);
int HasCondition(void* cond, int id);
int OwnerId(Owner* owner);
int PlayerInHost(Actor* actor);
int HandleToPlayer(int handle);
int ModelAnimate(int model, char* name, int unk);
void ZoneUpdate(void* zone, IVec3* pos);
String fn_800225C8(String& a, String& b);
}

// From a shared header: names of the ghost actions (the array itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

int CollectUpdate(void* p);
int CollectDone(void* p);
int CollectEnter(void* p);
int CollectCheck(Actor* a, Actor* o);
int CollectMatch(u32* id, u32 other);

static BehType gCollectBehTypes[] = {
    { CollectEnter, CollectDone, CollectUpdate, "" },
};

// The retail file carries this unit's two source files; the helpers below are the angle math.
void VecSwapXY(IVec3* out, IVec3* in);
void VecSwapXY2(IVec3* out, IVec3* in);
void FixedToRad(Vec3* out, IVec3* in);
void RadToFixed(IVec3* out, Vec3* in);
void VecToAngles(IVec3* out, IVec3* in);
void RotateVec(IVec3* out, IVec3* in);
void WrapAngle(IVec3* out, IVec3* in);

inline int Query(int target, int msg, u32 hash) {
    QueryMsg m;

    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    return *(int*)&m.result;
}

// 0x8016C244: per-frame update.
int CollectUpdate(void* p) {
    float scale, unk1, unk2;
    IVec3 ang, vec;
    Vec3 pos, out;
    Mtx34 mtx;
    Actor* o;
    Beh* st;

    if (p == 0) {
        return 0;
    }
    o = (Actor*)p;
    st = o->beh;
    st->timer -= o->f138;
    if (HasCondition(o->cond, 0xB)) {
        return 0;
    }
    if (fn_80117FB4(o) == 0) {
        return 1;
    }
    if (st->id == 0x3147A953) {
        return 1;
    }
    if (st->id == (int)0xACBFC0A8) {
        return 1;
    }
    if (o->f2d0 == -1) {
        pos.x = (float)o->pos.x;
        pos.y = (float)o->pos.y;
        pos.z = (float)o->pos.z;
        fn_8009F63C(&mtx, o);
        fn_8016D430(&ang, (int)(11.377778f * (float)(rand() % 360)), 0, 0);
        fn_8016D330(&vec, &ang);
        fn_800E0E9C(&vec, &vec, (rand() % 10 + 1) * 500);
        fn_8009F44C(&vec, &mtx, &vec);
        out.x = (float)(o->pos.x + vec.x);
        out.y = (float)(o->pos.y + vec.y);
        out.z = (float)(o->pos.z + vec.z);
        if (fn_800BB218(o, o->f2c4, &pos, &out, o, &scale, &unk1, &unk2, o->f900)) {
            out.x = pos.x + (float)vec.x * scale;
            out.y = pos.y + (float)vec.y * scale;
            out.z = pos.z + (float)vec.z * scale;
        }
        st->pos = out;
    }
    return 0;
}

// 0x8016C574: the behaviour is done after one pass.
int CollectDone(void* p) {
    return 1;
}

// 0x8016C57C: spawn the collect point at a random point around the owner.
int CollectEnter(void* p) {
    float scale, unk1, unk2;
    float radius;
    IVec3 ang, vec;
    Vec3 pos, out;
    Mtx34 mtx;
    Actor* o;
    Beh* st;

    if (p == 0) {
        return 0;
    }
    o = (Actor*)p;
    st = o->beh;
    st->state = 0;
    st->id = 0;
    pos.x = (float)o->pos.x;
    pos.y = (float)o->pos.y;
    pos.z = (float)o->pos.z;
    fn_8009F63C(&mtx, o);
    fn_8016D430(&ang, (int)(11.377778f * (float)(rand() % 360)), 0, 0);
    fn_8016D330(&vec, &ang);
    fn_800E0E9C(&vec, &vec, (rand() % 10 + 1) * 500);
    fn_8009F44C(&vec, &mtx, &vec);
    out.x = (float)(o->pos.x + vec.x);
    out.y = (float)(o->pos.y + vec.y);
    out.z = (float)(o->pos.z + vec.z);
    if (fn_800BB218(o, o->f2c4, &pos, &out, o, &scale, &unk1, &unk2, o->f900)) {
        out.x = pos.x + (float)vec.x * scale;
        out.y = pos.y + (float)vec.y * scale;
        out.z = pos.z + (float)vec.z * scale;
    }
    st->pos = out;
    radius = 300.0f;
    fn_8011818C(o);
    fn_80118250(o, 0x469ABA3B);
    fn_80118084(o->obj, 0x0329C15B, &st->pos);
    fn_80118084(o->obj, 0x033FA508, &radius);
    return 1;
}

// 0x8016C87C: the ghost object's id setter.
int CollectSetId(int id, void* p) {
    if (p == 0) {
        return 0;
    }
    ((Actor*)p)->beh->id = id;
    return 1;
}

// 0x8016C89C: message handler.
int CollectMsg(int msg, void* p) {
    Actor* o = *(Actor**)p;
    Msg* m = (Msg*)p;

    if (o->obj->msg != 0 && o->obj->msg(msg, p)) {
        return 1;
    }
    switch (msg) {
    case 0: {
        Beh* st;
        int anim;

        if (o == 0) {
            return 0;
        }
        st = o->beh;
        if (st->timer < 1000.0f && st->unk18 != 0) {
            return 1;
        }
        anim = fn_800AFD20(o);
        fn_8003F15C(anim, (void*)fn_8003A1F0(anim, o->bone, ModelAnimate(o->model, 0, 0)),
                    o->anim, 1.0f, &m->f10, 0);
        o->f13c = 0.0f;
        return 1;
    }
    case 1:
        return -999999;
    case 4:
        return 0;
    case 0x400:
        if (o == 0 || p == 0) {
            return 0;
        }
        m->id = OwnerId((Owner*)o);
        m->unkC = 1;
        m->f10 = 1.0f;
        m->f14 = 1.0f;
        m->unk18 = 2;
        m->unk1C = 0;
        return 1;
    case 0x401:
        return fn_80116D40(o, p);
    case 0x407:
        return 1;
    case 0x40A:
        return 1;
    case 0x41D:
        if (o == 0) {
            return 0;
        }
        return fn_80019F88(p, o) != 0;
    case 0x41E: {
        MsgName* n = (MsgName*)p;
        String* res = &n->name;

        if (fn_80117FF8(o->obj, n->id, res)) {
            return 1;
        }
        if (o == 0) {
            return 0;
        }
        if (n->id != 0xAD8576FA) {
            return 0;
        }
        {
            String name(gCollectBehTypes[o->beh->state].name);
            String nl("\n");
            String joined = fn_800225C8(name, nl);

            fn_8028C188(res, joined.mData, joined.mLength);
        }
        return 0;
    }
    case 0xC14:
        if (m->unkC != 0x6C345908) {
            return 0;
        }
        fn_800A3210(o);
        o->f8f8 = 1;
        return 1;
    default:
        return 0;
    }
}

// 0x8016CBEC: per-frame tick; runs the behaviour-table hooks and the pickup check.
int CollectTick(Actor* o) {
    Beh* st;

    if (o == 0) {
        return 0;
    }
    st = o->beh;
    if (st->state == -1) {
        st->state = 0;
        gCollectBehTypes[st->state].enter(o);
    }
    fn_800B42E0(o->f84, o->handle, 101.0f, 3, 0x82);
    ZoneUpdate(&o->obj->zone, &o->pos);
    st->unk18 = (st->unk18 == 0);
    if (gCollectBehTypes[st->state].update(o) != 0) {
        if (gCollectBehTypes[st->state].done(o) != 0) {
            gCollectBehTypes[st->state].enter(o);
        }
    }
    if (st->timer <= 0.0f) {
        fn_800A31A0(o);
        return 0;
    }
    fn_8001ED98(&o->pos, (int)o->f900, CollectCheck, o, 0);
    return 1;
}

// 0x8016CD84: spatial-query callback; decides whether "o" may be collected by "a".
int CollectCheck(Actor* a, Actor* o) {
    Msg q;

    q.obj = a;
    q.id = -1;
    a->handler(0x400, &q);
    if (q.unkC == 0) {
        return 0;
    }
    if (o->beh->timer < 0.0f) {
        return 0;
    }
    if (a->handle == o->handle) {
        return 0;
    }
    switch (q.id >> 24) {
    case 0x13:
        break;
    case 2:
        if (!PlayerInHost(a)) {
            return 0;
        }
        if (fn_80120C14(0x7BFB940D, 0, HandleToPlayer(a->handle)) >= 16.0f) {
            return 0;
        }
        break;
    default:
        return 1;
    }
    if (fn_801BD694(a) && !fn_80116E08(a)) {
        return 0;
    }
    if (a->type == (int)0x8CFF50E3) {
        return 0;
    }
    {
        int coords[6];

        coords[0] = a->pos.x;
        coords[1] = a->pos.y;
        coords[2] = a->pos.z;
        coords[3] = o->pos.x;
        coords[4] = o->pos.y;
        coords[5] = o->pos.z;
        if (fn_8001F928(o, coords, 0x12000000, CollectMatch, a) == 0) {
            fn_80120C80(1.0f, 0x7BFB940D, 0, HandleToPlayer(a->handle));
            FxStart(FxFind("collect_point"), 0, 0);
            o->beh->timer = -1.0f;
        }
    }
    return 1;
}

// 0x8016CF70: fn_8001F928 callback; rejects the owner's own entry.
int CollectMatch(u32* id, u32 other) {
    if (*id != 0) {
        return *id != other;
    }
    return 1;
}

// 0x8016CF98: destroy callback.
void CollectRelease(Actor* o) {
    if (o == 0) {
        return;
    }
    fn_8011818C(o);
    VFree(o->beh);
    o->f104 = 0;
    o->beh = 0;
}

// 0x8016CFDC: create callback.
void CollectInit(Actor* o) {
    int found;
    Beh* st;

    if (o == 0) {
        return;
    }
    st = (Beh*)VAlloc(0x1C);
    if (st == 0) {
        o->beh = 0;
        return;
    }
    o->beh = st;
    o->fc = fn_800CE96C;
    o->f904 = (float)o->desc->unk30;
    if (fn_80117D80(o) == 0) {
        VFree(st);
        o->beh = 0;
        return;
    }
    o->obj->setId = CollectSetId;
    o->obj->owner = o;
    st->state = 0;
    st->timer = 10000.0f;
    st->unk18 = 0;
    st->id = 0;
    st->pos.x = 0.0f;
    st->pos.y = 0.0f;
    st->pos.z = 0.0f;
    found = Query(0x4B000000, 0x41E, 0xC355E8EE);
    fn_800A7A54(o->cond, 0xB, found == 0);
    fn_800A7A54(o->cond, 1, 1);
    fn_800A7B08(o->cond, 6, 0);
    fn_800B6CAC(o, (void*)fn_800B5028);
}

// 0x8016D170: swap x and y (used by the ghost action code).
void VecSwapXY(IVec3* out, IVec3* in) {
    out->x = in->y;
    out->y = in->x;
    out->z = in->z;
}

// 0x8016D18C: swap x and y (second copy used by the angle helpers).
void VecSwapXY2(IVec3* out, IVec3* in) {
    out->x = in->y;
    out->y = in->x;
    out->z = in->z;
}

// 0x8016D1A8: 12-bit fixed point -> degrees.
void FixedToRad(Vec3* out, IVec3* in) {
    float z = 0.087890625f * (float)in->z;
    float x = 0.087890625f * (float)in->x;
    float y = 0.087890625f * (float)in->y;
    out->z = z;
    out->x = x;
    out->y = y;
}

// 0x8016D220: degrees -> 12-bit fixed point.
void RadToFixed(IVec3* out, Vec3* in) {
    float z = 11.377778f * in->z;
    float x = 11.377778f * in->x;
    float y = 11.377778f * in->y;
    out->z = (int)z;
    out->x = (int)x;
    out->y = (int)y;
}

// 0x8016D278: horizontal direction of a vector, as fixed-point angles.
void VecToAngles(IVec3* out, IVec3* in) {
    int len2 = in->y * in->y + in->x * in->x;
    int n;

    if (len2 <= 0) {
        n = 0;
    } else {
        n = (int)(float)sqrt((double)len2);
    }
    out->z = fn_800A1558(-in->x, in->y);
    out->x = fn_800A1558(in->z, n);
    out->y = 0;
}

// 0x8016D330: rotate a fixed-point vector by another.
void RotateVec(IVec3* out, IVec3* in) {
    int a;
    int b;
    int sina;
    int cosa;
    int sinb;
    int cosb;

    a = in->z;
    b = in->x;
    cosa = fn_800A1688(a);
    sina = fn_800A1624(a);
    cosb = fn_800A1688(b);
    sinb = fn_800A1624(b);
    out->x = (-sina * cosb) / 4096;
    out->y = (cosa * cosb) / 4096;
    out->z = sinb;
}

// 0x8016D3B4: wrap each fixed-point angle into [-0x800, 0x7FF].
void WrapAngle(IVec3* out, IVec3* in) {
    int v;

    v = in->z;
    if (v > 0x7FF) {
        out->z = v - 0x1000;
    } else if (v < -0x800) {
        out->z = v + 0x1000;
    }
    v = in->x;
    if (v > 0x7FF) {
        out->x = v - 0x1000;
    } else if (v < -0x800) {
        out->x = v + 0x1000;
    }
    v = in->y;
    if (v > 0x7FF) {
        out->y = v - 0x1000;
    } else if (v < -0x800) {
        out->y = v + 0x1000;
    }
}

// 0x8016D430: build a fixed-point vector from three components (z first).
void MakeVec(IVec3* out, int a, int b, int c) {
    out->z = a;
    out->x = b;
    out->y = c;
}

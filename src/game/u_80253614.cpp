// u_80253614.cpp: the "IMP FIRE" effect type (0x80253614-0x802540F4) plus the fire ball asset helpers
// (0x802540F4-0x802541D4), split from u_80251CB8.cpp (own action-name pool and constants).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHashSeed(const char* s, u32 h) {
    int i;

    if (s == 0) {
        return 0;
    }
    for (i = 0; s[i] != 0; i++) {
        h *= 31;
        h += (unsigned char)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
}

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();
    String(const char* s);
    ~String() {}
    virtual void Release(char* p);
};

struct Owner;
struct Actor;

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

// Six-word snapshot of an actor's collision source.
struct ZoneHitBox {
    u32 data[6];
};

// Position and handle of an owner.
struct Target {
    u8 pad[0x24];
    int x, y, z;
    u8 pad30[8];
    int id;  // 0x38
};

// The imp's per-actor state (VAlloc'd, 0x14 bytes).
struct ImpBeh {
    float timer;   // 0x0
    float near;    // 0x4: time spent close to the target
    float chase;   // 0x8
    float hurt;    // 0xC: cool-down of the hurt effect
    u8 state;      // 0x10
    u8 flags;      // 0x11
    s8 seed;       // 0x12
};

struct Actor {
    u8 pad00[0x24];
    int x, y, z;                        // 0x24
    int (*msgFn)(int msg, void* data);  // 0x30
    u8 pad34[4];
    int id;                             // 0x38
    u8 pad3C[4];
    void* model;                        // 0x40
    u8 pad44[0x84 - 0x44];
    Vec3 pos;                           // 0x84
    u8 pad90[0xD4 - 0x90];
    int unkD4;                          // 0xD4
    u8 padD8[0xFC - 0xD8];
    void* unkFC;                        // 0xFC
    u8 pad100[4];
    int unk104;                         // 0x104
    u8 pad108[4];
    void* unk10C;                       // 0x10C
    u8 pad110[0x130 - 0x110];
    ImpBeh* beh;                        // 0x130
    u8 pad134[4];
    float dt;                           // 0x138
    u8 pad13C[0x214 - 0x13C];
    int unk214;                         // 0x214
    u8 pad218[4];
    int unk21C;                         // 0x21C
    int unk220;                         // 0x220
    u8 pad224[0x22C - 0x224];
    Target* unk22C;                     // 0x22C
    u8 pad230[0x270 - 0x230];
    u8 st[0x18];                        // 0x270: condition block
    int unk288;                         // 0x288
    u8 pad28C[0x2C4 - 0x28C];
    int unk2C4;                         // 0x2C4
    u8 pad2C8[0x5FC - 0x2C8];
    void* unk5FC;                       // 0x5FC
    void* unk600;                       // 0x600
    void* unk604;                       // 0x604
    u8 pad608[0x894 - 0x608];
    u8 unk894[0x1E];                    // 0x894
    u16 nearMin;                        // 0x8B2
    u16 nearMax;                        // 0x8B4
    u8 pad8B6[0x8EC - 0x8B6];
    int unk8EC;                         // 0x8EC
    int unk8F0;                         // 0x8F0
    u8 pad8F4[0x900 - 0x8F4];
    float unk900;                       // 0x900
    float health;                       // 0x904
};

typedef int (*ImpStateFunc)(void* actor);

// One entry of the imp's state table (0x803BECD8, 0x10 bytes).
struct ImpStateType {
    ImpStateFunc enter;   // 0x0
    ImpStateFunc exit;    // 0x4
    ImpStateFunc update;  // 0x8
    const char* name;     // 0xC
};


// Message layouts. The offsets are the same across message types; each case uses its own view.
struct Msg400 {
    Actor* actor;   // 0x00
    int ownerId;    // 0x04
    int unk08;      // 0x08
    int state;      // 0x0C
    float f10;      // 0x10
    float f14;      // 0x14
    int flags;      // 0x18
    int unk1C;      // 0x1C
};

struct Msg407 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int unk08;      // 0x08
    float damage;   // 0x0C
    int unk10;      // 0x10
    u32 type;       // 0x14
    u8 pad18[0x44 - 0x18];
    int unk44;      // 0x44
};

struct Msg409 {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    int anim;       // 0x08
    int arg;        // 0x0C
};

struct Msg40A {
    Actor* actor;   // 0x00
    int unk04;      // 0x04
    u32 anim;       // 0x08
    int result;     // 0x0C
};

struct Msg41D {
    Actor* actor;   // 0x00
    u32 cmd;        // 0x04
    int unk08;      // 0x08
    int arg;        // 0x0C
};

struct Msg41E {
    Actor* actor;   // 0x00
    u32 cmd;        // 0x04
    int unk08;      // 0x08
    Buffer result;  // 0x0C
};

int OwnerId(Owner* o);                              // 0x800A3A40
void* VAlloc(int size);                             // 0x8009DA68
void VFree(void* p);                                // 0x8009DA3C
int HasCondition(void* conditions, int which);      // 0x800A7AC4
float OwnerValue(Owner* o, void* value);            // 0x800AA050
int ActorPlayAnim(int owner, u32 anim, int arg);    // 0x800C5654
void* FxFind(const char* name);
void ActorGetMatrix(Mtx34* m, Actor* a);
int ModelBoneHitBox(int a, int b, int c, int d);

extern "C" {
int rand(void);
int SendMsg(int to, int msg, void* data);
int fn_80019F88(void* data, Actor* actor);
String fn_800225C8(const String& a, const String& b);
void fn_80090390(int a, int b);
int fn_800B1EF4(Actor* actor, int kind);
void fn_800B1F20(Actor* actor, int kind);
void fn_800B20B8(Actor* actor, int a);
void fn_800B2418(Actor* actor, int kind, float f);
int fn_800B2454(Actor* actor, int kind);
void fn_800B261C(Actor* actor, int kind, void* out);
int fn_800B26B8(Actor* actor);
int fn_800BAF40(Actor* actor);
void fn_800BAFF8(Actor* actor, Vec3* dir, int a, int b, Vec3* to, float f1, float f2);
int fn_800BB218(Actor* actor, int flags, Vec3* from, Vec3* to, Actor* actor2, float* scale, int* a, int* b,
                float dist);
void fn_800BBCA8(Actor* actor);
void fn_800BC56C(Actor* actor, void* target);
void fn_800BC5A8(Actor* actor, Vec3* to);
int fn_800BF2D0(Vec3* p);
void fn_800CC984();
void fn_800CE96C();
void fn_800CF064();
void fn_800C748C();
float fn_800E1020(Vec3* v);
void fn_800E1098(void* out, void* in, float length);
void fn_80162610(Actor* actor, void* p, int a);
void fn_80162658(Actor* actor, void* p);
int fn_80162940(void* p, int a);
void fn_8016D864(ZoneHitBox* out, void* src);
int fn_8016F120(void* fx, int* pos);
void fn_8028C188(Buffer* b, char* data, int len);
}
int MemFlagIsSet(void* bits, int index);
int ConditionUpdateBit(void* cond, int bit, int set);
int ConditionSetBit(void* cond, int bit, int set);
void* FollowGetTargetObj(void* range);
void FollowSetTargetObj(void* range, void* obj);
int FollowOffsetUpdate(void* self);
void RangeResetPending(void* range);

extern void* lbl_804D3600;  // "trib_see" effect
extern void* lbl_804D3604;  // "ice_frozenimps" effect

// ---------------------------------------------------------------------------------------------------
// The "IMP FIRE" effect type.


extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);


inline u32 StrHash(const char* s) {
    if (s == 0) {
        return 0;
    } else {
        u32 h = 0;
        int i;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
        return h;
    }
}

struct EffectType {
    const char* name;
    u32 hash;
    int unk8[3];
    void* (*create)(int a, int id);
    void* destroy;
    void* msg;
    void* query;
    void* update;
    void* reset;
    int unk2C;
};

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}
struct EffectObj {
    int unk0;
    int unk4;
    int emitter;
    void* data;
};

extern "C" {
int fn_800F7D28(int emitter, int id);
void fn_800F86A0(int emitter);
}

void VFree(void* p);

struct Player;


struct ImpShot {
    u8 pad00[0x30];
    float x, y, z;
    u8 pad3C[0x48 - 0x3C];
    Target* target;
    int pad4C;
    float spread;
    float scale;
    u8 pad58[0x60 - 0x58];
};

struct ImpShotInfo {
    ImpShot* hit;
    float speed;
    float unk8;
    int unkC;
    int charge;
    int unk14;
    int unk18;
    int unk1C;
    int unk20;
};

struct MsgResult {
    void* p;
    char pad[0x1C];
};

struct ImpFireDef {
    int unk00;
    int unk04;  // ammo id
    int unk08;  // ammo max
    float speed;
    float unk10;
    int unk14;
    int charge;
    int drain;
    int maxCharge;
    int spread;
    int unk28;
    int unk2C;
    void* fx;
    u8 pad34[0x48 - 0x34];
    int model;
    float recoil;
    float recoilRate;
    float spreadMinA;
    float spreadMaxA;
    float spreadMinB;
    float spreadMaxB;
};

struct ImpFire {
    int unk00;  // state
    float unk04;  // timer
    int emitter;
    ImpFireDef* def;
    int unk10;
    float charge;
    float recoil;
    float recoilTimer;
    int arg;
    ImpShot shot;
    int unk84;
};

extern "C" {
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
int FxStart(void* fx, int a, int b);
int SendMsg(int target, int msg, void* data);
void fn_8000F550(short* ids, int count);
}

int Fn80175D60(unsigned long arg);
void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
void PlayerAnimOffUpdate(Player* p, float dt);
int AssetFind(u32 table, u32 key);
void TextureLoad(short* ids, int count);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);
void ImpFireAssetsFree(void);
void ImpFireAssetsLoad(void);

inline float Min(float a, float b) { return a < b ? a : b; }



// Launch parameters for the fire ball (see ImpFireBallSpawn).
struct ImpThrow {
    Vec3 pos;
    u8 pad0C[0x18 - 0x0C];
    Vec3 vel;
    Vec3 spin;
    Vec3 accel;
    u8 pad3C[0x4C - 0x3C];
};

extern "C" {
int rand(void);
float fn_800E1020(Vec3* v);
void fn_800E1098(void* out, void* in, float length);
}
int ImpFireBallSpawn(int owner, ImpThrow* t, int n);

inline float FastSqrt(float x) {
    register float r;
    float y;

    if (x <= 0.0f) {
        return 0.0f;
    } else {
        asm {
            lfs r, x
            ps_rsqrte r, r
            ps_res r, r
            stfs r, y
        }
        return y;
    }
}

inline int FxStartAt(void* fx, float* p, int* pos) {
    int h;
    if (p) {
        pos[0] = p[0];
        pos[1] = p[1];
        pos[2] = p[2];
        h = fn_8016F120(fx, pos);
        fn_8016F118(h, 0, 100000);
    } else {
        h = FxStart(fx, 0, 0);
    }
    return h;
}

extern EffectType gImpFireType;

void ImpFireReset(EffectObj* obj) {
    ImpFire* g = (ImpFire*)obj->data;

    g->unk00 = 0;
    g->unk04 = 0.0f;
}

void ImpFireUpdate(EffectObj* obj, float dt) {
    ImpFire* g = (ImpFire*)obj->data;
    ImpFireDef* def = g->def;

    if (g == 0) {
        return;
    }
    switch (g->unk00) {
    case 0:
        break;
    case 1:
        g->unk04 -= dt;
        while (g->unk04 <= 0.0f) {
            if (fn_800F7D28(g->emitter, def->unk04) > 0) {
                MsgResult q;
                ImpThrow t;
                Vec3 d;
                int pos[3];
                Target* src;
                Target* dst;
                void* fx;
                float rz;
                float ry;
                float rx;
                float up;
                float root;

                if (g->shot.target == 0) {
                    return;
                }
                SendMsg(g->arg, 0x400, &q);
                src = (Target*)q.p;
                memset(&t, 0, sizeof(ImpThrow));
                t.pos.x = src->x;
                t.pos.y = src->y;
                t.pos.z = src->z;
                t.accel.x = 0.0f;
                t.accel.y = 0.0f;
                t.accel.z = -6000.0f;
                rz = rand() / 32767.0f;
                ry = rand() / 32767.0f;
                rx = rand() / 32767.0f;
                t.spin.x = 800.0f * rx - 400.0f;
                t.spin.y = 800.0f * ry - 400.0f;
                t.spin.z = 800.0f * rz - 400.0f;
                dst = g->shot.target;
                t.vel.x = 0.0f;
                t.vel.y = 0.0f;
                t.vel.z = 3500.0f;
                d.x = dst->x;
                d.y = dst->y;
                d.z = dst->z;
                d.x -= t.pos.x;
                d.y -= t.pos.y;
                d.z -= t.pos.z;
                up = -(t.vel.z / t.accel.z);
                root = FastSqrt((d.z - 0.5f * t.vel.z * up) / (0.5f * t.accel.z));
                {
                    float s = fn_800E1020(&d) / (up + root);
                    d.z = 0.0f;
                    fn_800E1098(&d, &d, s);
                }
                t.vel.x += d.x;
                t.vel.y += d.y;
                t.vel.z += d.z;
                ImpFireBallSpawn(g->arg, &t, 1);
                fx = def->fx;
                FxStartAt(fx, &g->shot.x, pos);
            }
        }
        break;
    }
}

u32 ImpFireQuery(EffectObj* obj, int msg) {
    ImpFire* g = (ImpFire*)obj->data;
    ImpFireDef* def = g->def;

    switch (msg) {
    case 0x0020EE36:
        return g->unk00 != 0 ? 0x3DA260F7 : 0x04A3E183;
    case 0xF829DB93:
        if (g->unk00 != 0) {
            return 0x3DA260F7;
        }
        if (fn_800F7D28(g->emitter, def->unk04)) {
            return 0x04A3E183;
        }
        return fn_800F7D28(obj->emitter, def->unk04) != 0 ? 0xC0BECF7E : 0x11D2C025;
    case 0x9791AF51:
    case 0x968CBE3D:
    case 0x96224FCC:
    case 0xE6B71BCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        return 0x3DA260F7;
    }
    return 0x3DA260F7;
}

int ImpFireMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    ImpFire* g = (ImpFire*)obj->data;
    ImpFireDef* def = g->def;
    short texs[6];

    g->arg = arg;
    Fn80175D60(g->arg);
    switch (msg) {
    case 0x1B83833F:
        g->unk00 = 0;
        return 1;
    case 0xF26951B8:
        ImpFireAssetsLoad();
        return 1;
    case 0x0CAE7B51:
        ImpFireAssetsFree();
        return 1;
    case 0xF829DB93:
    case 0x0020EE36: {
        ImpShotInfo info;
        MsgResult q;
        int pos[3];
        void* fx;

        g->unk10 = 1;
        if (!fn_800F7D28(g->emitter, def->unk04)) {
            return 0;
        }
        g->charge += def->charge;
        g->charge = Min(g->charge, def->maxCharge);
        if (((arg >> 24) & 0xFF) == 2) {
            SendMsg(arg, 0x400, &q);
            PlayerAnimOffUpdate((Player*)q.p, def->recoil);
            g->recoil += def->recoil;
            g->recoilTimer = 300.0f;
        }
        info.hit = (ImpShot*)data;
        info.speed = def->speed;
        info.unk8 = def->unk10;
        info.unkC = def->unk14;
        info.charge = g->charge;
        info.unk18 = def->unk28;
        info.unk1C = def->unk2C;
        if (((g->arg >> 24) & 0xFF) == 2) {
            ((ImpShot*)data)->spread =
                ((ImpShot*)data)->spread * (def->spreadMaxA - def->spreadMinA) + def->spreadMinA;
        } else {
            ((ImpShot*)data)->spread =
                ((ImpShot*)data)->spread * (def->spreadMaxB - def->spreadMinB) + def->spreadMinB;
        }
        info.hit->spread *= info.hit->scale;
        Fn800F7F5C(g->emitter, def->unk04, 0x14967, arg, &info, dt);
        fx = def->fx;
        FxStartAt(fx, &g->shot.x, pos);
        return 1;
    }
    case 0x968CBE3D:
    case 0x8FDD7599:
    case 0xBC78F550:
    case 0xE6B71BCC:
    case 0x0B59E861:
        return 1;
    case 0xE452286F:
    case 0x141109BD: {
        short model;

        model = def->model;
        texs[0] = AssetFind(0xF2E2BEB8, 0x5541EDD2);
        texs[1] = AssetFind(0xF2E2BEB8, 0x56F6C671);
        texs[2] = AssetFind(0xF2E2BEB8, 0x58AB9F10);
        texs[3] = AssetFind(0xF2E2BEB8, 0x5A6077AF);
        texs[4] = AssetFind(0xF2E2BEB8, 0x2E20AEE7);
        texs[5] = AssetFind(0xF2E2BEB8, 0x961C77AB);
        if ((int)data == 1) {
            ModelLoadIds(&model, 1);
            TextureLoad(texs, 6);
        } else {
            ModelFreeIds(&model, 1);
            fn_8000F550(texs, 6);
        }
        return 1;
    }
    }
    return 0;
}

void ImpFireDestroy(EffectObj* obj) {
    ImpFire* g = (ImpFire*)obj->data;

    fn_800F86A0(g->emitter);
    VFree(g);
}

void* ImpFireCreate(int a, int id) {
    EffectType* type = &gImpFireType;
    ImpFire* g;

    EffectTypeHash(type);
    g = (ImpFire*)VAlloc(sizeof(ImpFire));
    memset(g, 0, sizeof(ImpFire));
    if (g == 0) {
        return 0;
    }
    g->def = (ImpFireDef*)fn_800EB21C(id);
    g->unk00 = 0;
    g->unk04 = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    return g;
}

EffectType gImpFireType = {
    "IMP FIRE", 0, {0, 0, 0}, ImpFireCreate, (void*)ImpFireDestroy,
    (void*)ImpFireMsg, (void*)ImpFireQuery, (void*)ImpFireUpdate, (void*)ImpFireReset, 0,
};


// ---------------------------------------------------------------------------------------------------
// Fire ball asset helpers: entry 0 of the name table is the model, entries 1-8 its textures.

struct SplineNameEntry {
    char* name;
    u32 group;
    u32 hash;
    int asset;
};

int SplineAssetResolve(SplineNameEntry* entries, int count, int index);

extern "C" {
void fn_8003D710(int model);
void fn_8003D8F4(int model);
void fn_8000F60C(int tex);
void fn_8000F874(int tex);
}

extern SplineNameEntry gImpFireAssets[11];

void ImpFireAssetsFree(void) {
    u32 i;

    fn_8003D710(SplineAssetResolve(gImpFireAssets, 9, 0));
    for (i = 1; i < 9; i++) {
        fn_8000F60C(SplineAssetResolve(gImpFireAssets, 9, i));
    }
}

void ImpFireAssetsLoad(void) {
    u32 i;

    fn_8003D8F4(SplineAssetResolve(gImpFireAssets, 9, 0));
    for (i = 1; i < 9; i++) {
        fn_8000F874(SplineAssetResolve(gImpFireAssets, 9, i));
    }
}

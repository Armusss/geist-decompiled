// u_80268780.cpp: the "HERMIT SPIT" effect type (message, destroy and create; its reset, update and
// query functions at 0x80268528-0x80268780 sit before this unit's split). A plain shot also starts the
// "crapElectricity" effect on a possessed target. Names are descriptive guesses; the code is matched
// against the retail DOL.

#include <dolphin/types.h>

#include <string.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_800F8AF4(int id);
int fn_800F7D28(int emitter, int id);
void fn_800F825C(int emitter, int id, int count);
void fn_800F86A0(int emitter);
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
int FxStart(void* fx, int a, int b);
void fn_8016EEF4(int handle);
void fn_8000F550(short* ids, int count);
void fn_80163824(int player, u32 id, int a, int b, int c);
void fn_80163854(int player, int a, int b);
int SendMsg(int target, int msg, void* data);
}

void* VAlloc(int size);
void VFree(void* p);
void Fn800F840C(int emitter, int a, int b);
void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
int Fn80175D60(unsigned long arg);
void* FxFind(const char* name);
int AssetFind(u32 table, u32 key);
int PlayerGetPossessor(int arg);
void TextureLoad(short* ids, int count);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);

extern int lbl_804D3770;  // .sdata flag: start the definition's hit effect on the next big shot

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

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

struct EffectObj {
    int unk0;
    int unk4;
    int emitter;
    void* data;
};

struct EffectType {
    const char* name;
    u32 hash;
    int unk8[3];
    void* (*create)(int a, int id);
    void (*destroy)(EffectObj* obj);
    int (*msg)(EffectObj* obj, int msg, int arg, void* data, float dt);
    u32 (*query)(EffectObj* obj, int msg);
    void (*update)(EffectObj* obj, float dt);
    void (*reset)(EffectObj* obj);
    int unk2C;
};

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}

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
    ~String() {}
    virtual void Release(char* p);
};

struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

struct ShotHit {
    u8 pad[0x30];
    float x, y, z;
    u8 pad3C[0x24];
};

struct SpitInfo {
    ShotHit* hit;
    float speed;
    float unk8;
    int unkC;
    int unk10;
    int unk14;
};

// The big shot's request carries two more words (unused here).
struct BigSpitInfo {
    ShotHit* hit;
    float speed;
    float unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
};

struct HermitSpitDef {
    int unk00;
    int id;
    int max;
    float speed;
    u8 pad10[0x48 - 0x10];
    int model2;
    u8 pad4C[0x64 - 0x4C];
    int id2;
    int max2;
    float speed2;
    float unk70;
    u8 pad74[0x90 - 0x74];
    void* hitFx;
    u8 pad94[0x98 - 0x94];
    int model;
    u8 pad9C[0xBC - 0x9C];
    int unkBC;
};

struct HermitSpit {
    int state;
    int emitter;
    int unk08;
    HermitSpitDef* def;
    int unk10;
    int arg;
    int pad18;
    ShotHit shot;
    u32 ammo;
    int fx;
    float unk84;
};

void HermitSpitReset(EffectObj* obj);  // 0x80268528, before this unit's split
void HermitSpitUpdate(EffectObj* obj, float dt);  // 0x80268538
u32 HermitSpitQuery(EffectObj* obj, int msg);  // 0x802685A0
int HermitSpitMsg(EffectObj* obj, int msg, int arg, void* data, float dt);
void HermitSpitDestroy(EffectObj* obj);
void* HermitSpitCreate(int a, int id);

EffectType gHermitSpitType = {
    "HERMIT SPIT", 0, {0, 0, 0}, HermitSpitCreate, HermitSpitDestroy, HermitSpitMsg, HermitSpitQuery,
    HermitSpitUpdate, HermitSpitReset, 0,
};

int HermitSpitMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    HermitSpit* s = (HermitSpit*)obj->data;
    HermitSpitDef* def = s->def;
    int busy = Fn80175D60(arg);
    BigSpitInfo info2;
    SpitInfo info1;
    short texs[10];
    int pos[3];
    short tex;
    short model;

    s->arg = arg;
    switch (msg) {
    case 0x0020EE36:
        if (s->state == 1) {
            memcpy(&s->shot, data, 0x60);
        } else {
            if (!fn_800F7D28(s->emitter, def->id)) {
                return 0;
            }
            info1.hit = (ShotHit*)data;
            info1.speed = def->speed;
            info1.unk14 = 1;
            info1.unk10 = 0;
            Fn800F7F5C(s->emitter, def->id, 0x14967, arg, &info1, dt);
            {
                QueryMsg q;
                q.hash = 0x5A32795A;
                *(int*)&q.result = 0;
                q.unk8 = 0;
                SendMsg(arg, 0x41E, &q);
            }
            if (s->fx == -1 && ((arg >> 24) & 0xFF) == 2) {
                s->fx = FxStart(FxFind("crapElectricity"), 0, 0);
            }
            s->unk84 = 0.0f;
        }
        return 1;
    case 0x96224FCC:
        if (s->state != 1) {
            s->state = 1;
        }
        memcpy(&s->shot, data, 0x60);
        return 1;
    case 0x0D19E338:
        s->state = 0;
        return 1;
    case 0xF829DB93:
        if (s->state == 1) {
            memcpy(&s->shot, data, 0x60);
            s->arg = arg;
        } else {
            float* p;
            void* fx;

            if (!fn_800F7D28(s->emitter, def->id2)) {
                return 0;
            }
            info2.hit = (ShotHit*)data;
            info2.speed = def->speed2;
            info2.unk8 = def->unk70;
            info2.unkC = 0;
            info2.unk10 = 0;
            Fn800F7F5C(s->emitter, def->id2, 0x14967, arg, &info2, dt);
            fx = def->hitFx;
            p = &info2.hit->x;
            if (lbl_804D3770) {
                if (p) {
                    pos[0] = p[0];
                    pos[1] = p[1];
                    pos[2] = p[2];
                    fn_8016F118(fn_8016F120(fx, pos), 0, 100000);
                } else {
                    FxStart(fx, 0, 0);
                }
                lbl_804D3770 = 0;
            }
            {
                QueryMsg q;
                q.hash = 0x5A32795A;
                *(int*)&q.result = 0;
                q.unk8 = 0;
                SendMsg(arg, 0x41E, &q);
            }
        }
        return 1;
    case 0x42E50A3D:
        if (s->state != 1) {
            s->state = 1;
        }
        memcpy(&s->shot, data, 0x60);
        s->arg = arg;
        return 1;
    case 0x24725EAA:
        memcpy(&s->shot, data, 0x60);
        s->arg = arg;
        return 1;
    case 0x9791AF51:
        s->state = 0;
        return 1;
    case 0xE6B71BCC: {
        int n;
        int m;
        int r;

        n = fn_800F7D28(s->emitter, def->id);
        if (n == def->max) {
            return 0;
        }
        m = fn_800F7D28(obj->emitter, def->id);
        if (m == 0) {
            return 0;
        }
        n = (m < def->max - n) ? m : def->max - n;
        {
            QueryMsg q;
            q.hash = 0x5A32795A;
            *(int*)&q.result = 0;
            q.unk8 = 0;
            SendMsg(arg, 0x41E, &q);
            r = *(int*)&q.result;
        }
        if (r == 0) {
            fn_800F825C(obj->emitter, def->id, n);
        }
        Fn800F840C(s->emitter, def->id, n);
        return 1;
    }
    case 0x315A1A7F:
        memcpy(&s->shot, data, 0x60);
        return 1;
    case 0xE452286F:
    case 0x141109BD:
        tex = AssetFind(0xF2E2BEB8, 0x2184AD04);
        if ((int)data == 1) {
            TextureLoad(&tex, 1);
        } else {
            fn_8000F550(&tex, 1);
        }
        model = def->model;
        tex = AssetFind(0xF2E2BEB8, 0x5F678088);
        if ((int)data == 1) {
            ModelLoadIds(&model, 1);
            TextureLoad(&tex, 1);
        } else {
            ModelFreeIds(&model, 1);
            fn_8000F550(&tex, 1);
        }
        model = def->model2;
        texs[0] = AssetFind(0xF2E2BEB8, 0x5541EDD2);
        texs[1] = AssetFind(0xF2E2BEB8, 0x56F6C671);
        texs[2] = AssetFind(0xF2E2BEB8, 0x58AB9F10);
        texs[3] = AssetFind(0xF2E2BEB8, 0x5A6077AF);
        texs[4] = AssetFind(0xF2E2BEB8, 0x2E20AEE7);
        texs[5] = AssetFind(0xF2E2BEB8, 0x961C77AB);
        texs[6] = AssetFind(0xF2E2BEB8, 0x42F9014E);
        texs[7] = AssetFind(0xF2E2BEB8, 0x5C1B2515);
        if ((int)data == 1) {
            ModelLoadIds(&model, 1);
            TextureLoad(texs, 8);
        } else {
            ModelFreeIds(&model, 1);
            fn_8000F550(texs, 8);
        }
        return 1;
    case 0x0B59E861:
        if (busy == 0) {
            int player = PlayerGetPossessor(arg);
            int a = fn_800F7D28(obj->emitter, def->id);
            fn_80163824(player, 0x878DC2AD, a, fn_800F7D28(s->emitter, def->id), def->max);
            fn_80163854(player, player + 0x1508, def->unkBC);
        }
        return 1;
    case 0x7FAB1247:
        s->unk10 = 1;
        return 1;
    }
    return 0;
}

void HermitSpitDestroy(EffectObj* obj) {
    HermitSpit* s = (HermitSpit*)obj->data;

    if (s->fx != -1) {
        fn_8016EEF4(s->fx);
        s->fx = -1;
    }
    fn_800F86A0(s->emitter);
    VFree(s);
}

void* HermitSpitCreate(int a, int id) {
    EffectType* type = &gHermitSpitType;
    HermitSpit* g;

    EffectTypeHash(type);
    g = (HermitSpit*)VAlloc(sizeof(HermitSpit));
    memset(g, 0, sizeof(HermitSpit));
    if (g == 0) {
        return 0;
    }
    g->def = (HermitSpitDef*)fn_800EB21C(id);
    g->state = 0;
    g->ammo = fn_800F8AF4(id);
    g->fx = -1;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->unk08, g->def->max);
    Fn800F840C(g->emitter, g->def->id, g->def->max);
    Fn800F840C(g->emitter, g->def->id2, g->def->max2);
    return g;
}


// ---------------------------------------------------------------------------------------------------
// 0x802690C8-0x802698D0: the "Hermit Spit Ammo" effect type. Its type descriptor and colours sit in
// .data right after a second copy of the ghost action table (0x803C0238), so this part is most likely
// its own file.

struct Vec3 {
    float x, y, z;
};

struct Vec4 {
    float x, y, z, w;
};

struct IVec3 {
    int x, y, z;
};

// Orientation and position, as returned by the actor matrix getters.
struct Mtx34 {
    float rot[3][3];
    Vec3 pos;
};

struct SpitActor {
    u8 pad00[0x24];
    IVec3 pos;
    u8 pad30[0x38 - 0x30];
    int handle;
    u8 pad3C[0x138 - 0x3C];
    float scale;
    u8 pad13C[0x270 - 0x13C];
    u8 target[4];
    u8 pad274[0x1480 - 0x274];
    Vec3 eye;
};

// The hit message sent to the target (0x407).
struct SpitHitMsg {
    int unk00;
    int unk04;
    int source;
    float damage;
    int unk10;
    u32 kind;
    IVec3 pos;
    IVec3 dir;
    IVec3 back;
    u8 pad3C[0x4C - 0x3C];
    float unk4C;
    u8 pad50[0x5C - 0x50];
};

// One entry of the ghost action table (from a shared header; unused here).
struct ActionEntry {
    const char* name;
    int unk4;
    int score;
    float unkC;
    float unk10;
    int flag;
};

static ActionEntry sAmmoActions[9] = {
    { "POSSESS", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "GHOSTACTION", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "SLIP", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "HIJACK", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "HOSTACTION", 0, 1000, 1600.0f, 4033.0f, 1 },
    { "EXCHANGE", 0, 1000, 1600.0f, 3600.0f, 1 },
    { "H_HIJACK", 0, 1000, 1600.0f, 3600.0f, 1 },
    { "TALK", 0, 1000, 1120.0f, 3600.0f, 1 },
    { "SCAN", 0, 1000, 1120.0f, 3600.0f, 1 },
};

extern "C" {
void fn_800A7BF0(int actor, Mtx34* m, int a);
void fn_800A0168(Mtx34* out, Mtx34* in);
void fn_8009FC6C(Mtx34* out, Mtx34* a, Mtx34* b);
void fn_800A0030(Vec3* out, Mtx34* m, Vec3* in);
void fn_800A025C(Vec3* out, Mtx34* m, int row);
SpitActor* fn_800A7A18(void* link);
void fn_8016D440(Vec3* out, Vec3* in);
void fn_8016D4EC(Vec3* out, Vec3* in);
float fn_800E1020(Vec3* v);
float fn_800A1418(float x);
void fn_800E1098(Vec3* out, Vec3* in, float length);
void fn_800E0CB8(IVec3* out, IVec3* in);
int rand(void);
}

void MtxSetEuler(Mtx34* m, Vec3* rot);
int GhostShotFire(float a, float b, float c, int d, Vec3* p5, Vec3* p6, Vec3* p7, Vec3* p8, Vec4* p9,
                  Vec4* p10, int k, int l, int m, int n, Vec3* opt);

u32 HermitSpitAmmoQuery(EffectObj* obj, int msg);
int HermitSpitAmmoMsg(EffectObj* obj, int msg, int arg, void* data, float dt);

// An effect type without the trailing word.
struct AmmoType {
    const char* name;
    u32 hash;
    int unk8[3];
    void* (*create)(int a, int id);
    void (*destroy)(EffectObj* obj);
    int (*msg)(EffectObj* obj, int msg, int arg, void* data, float dt);
    u32 (*query)(EffectObj* obj, int msg);
    void (*update)(EffectObj* obj, float dt);
    void (*reset)(EffectObj* obj);
};

static AmmoType sHermitSpitAmmoType = {
    "Hermit Spit Ammo", 0, {0, 0, 0}, 0, 0, HermitSpitAmmoMsg, HermitSpitAmmoQuery, 0, 0,
};
static Vec4 sSpitColor = { 0.25f, 0.1f, 0.0f, 0.7f };
static Vec4 sSpitColor2 = { 0.25f, 0.2f, 0.0f, 0.0f };

inline u32 AmmoTypeHash(AmmoType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}

inline int VecNonZero(const Vec3* v) {
    return (__fabs(v->x) > 1.1920929e-7f) + (__fabs(v->y) > 1.1920929e-7f) + (__fabs(v->z) > 1.1920929e-7f);
}

inline float VecNormalize(Vec3* v) {
    double x, y, z, len;
    float inv;

    if (VecNonZero(v) == 0) {
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 0.0f;
        return 0.0f;
    }
    x = (double)v->x;
    y = (double)v->y;
    z = (double)v->z;
    {
        double xx = x * x;
        double yy = y * y;
        double zz = z * z;

        len = xx + yy + zz;
    }
    inv = __frsqrte((float)len);
    v->x = (float)(x * inv);
    v->y = (float)(y * inv);
    v->z = (float)(z * inv);
    return __fres(inv);
}

u32 HermitSpitAmmoQuery(EffectObj* obj, int msg) {
    return 0x3DA260F7;
}

// Spits one glob from the player's mouth towards its target (or ahead), and tells a target that is in
// front of the player that it was hit.
void HermitSpitAmmoFire(int arg, int busy, void* data, int unk) {
    SpitHitMsg hit;
    Mtx34 mtx;
    Mtx34 rotMtx;
    Mtx34 aim;
    Mtx34 tmtx;
    int res[8];
    Vec3 dir;
    Vec3 vel;
    Vec3 end;
    Vec3 fwd;
    Vec3 rot;
    Vec3 ang;
    Vec3 delta;
    Vec3 facing;
    Vec3 toTarget;
    SpitActor* actor;
    SpitActor* target;
    float dist;
    float yaw;
    float cone;
    float len;

    AmmoTypeHash(&sHermitSpitAmmoType);
    SendMsg(arg, 0x400, res);
    actor = (SpitActor*)res[0];
    fn_800A7BF0((int)actor, &mtx, 0);
    rot.z = 0.0f;
    rot.x = 0.0f;
    rot.y = 90.0f * (rand() / 32767.0f) - 45.0f;
    if (rand() / 32767.0f < 0.5f) {
        rot.y += 180.0f;
    }
    MtxSetEuler(&rotMtx, &rot);
    fn_800A0168(&aim, &mtx);
    dir.x = 0.43589f;
    dir.y = -0.9f;
    dir.z = 0.0f;
    fn_8009FC6C(&aim, &aim, &rotMtx);
    fn_800A0030(&dir, &aim, &dir);
    dist = 5000.0f;
    fn_800A025C(&fwd, &mtx, 1);
    target = fn_800A7A18(actor->target);
    if (target) {
        delta.x = target->pos.x;
        delta.y = target->pos.y;
        delta.z = target->pos.z;
        if (((u32)target->handle >> 24) == 2) {
            delta.x += target->eye.x;
            delta.y += target->eye.y;
            delta.z += target->eye.z;
        }
        delta.x -= mtx.pos.x;
        delta.y -= mtx.pos.y;
        delta.z -= mtx.pos.z;
        fn_8016D440(&ang, &delta);
        yaw = ang.x;
    }
    fn_8016D440(&ang, &fwd);
    ang.x = yaw;
    ang.z += 5.0f * (rand() / 32767.0f) - 2.5f;
    fn_8016D4EC(&fwd, &ang);
    if (target) {
        dist = fn_800E1020(&delta);
    }
    cone = fn_800A1418(0.5f * (4.0f * ((7800.0f - dist) / 3800.0f) + 1.0f));
    fn_800E1098(&fwd, &fwd, dist);
    end.x = fwd.x + mtx.pos.x;
    end.y = fwd.y + mtx.pos.y;
    end.z = fwd.z + mtx.pos.z;
    len = fn_800E1020(&fwd);
    fn_800E1098(&dir, &dir, 0.05f);
    fn_800E1098(&vel, &dir, 0.0f);
    GhostShotFire(2.0f * len, 10.0f + 120.0f * len / 5000.0f, 1500.0f, actor->handle, &mtx.pos, &dir,
                  &end, &vel, &sSpitColor, &sSpitColor2, 0x24A738, 600, 0, 1, &fwd);
    target = fn_800A7A18(actor->target);
    if (target) {
        fn_800A7BF0((int)actor, &tmtx, 0);
        fn_800A025C(&facing, &tmtx, 1);
        facing.z = 0.0f;
        VecNormalize(&facing);
        toTarget.x = target->pos.x;
        toTarget.y = target->pos.y;
        toTarget.z = target->pos.z;
        toTarget.x -= tmtx.pos.x;
        toTarget.y -= tmtx.pos.y;
        toTarget.z -= tmtx.pos.z;
        toTarget.z = 0.0f;
        VecNormalize(&toTarget);
        if (facing.x * toTarget.x + facing.y * toTarget.y + facing.z * toTarget.z > cone) {
            memset(&hit, 0, sizeof(hit));
            hit.source = actor->handle;
            hit.kind = 0x53D456BE;
            hit.unk4C = 10.0f;
            hit.damage = 600.0f * (0.001f * actor->scale);
            hit.pos = actor->pos;
            hit.dir.x = hit.pos.x - target->pos.x;
            hit.dir.y = hit.pos.y - target->pos.y;
            hit.dir.z = hit.pos.z - target->pos.z;
            fn_800E0CB8(&hit.dir, &hit.dir);
            hit.back.x = -hit.dir.x;
            hit.back.y = -hit.dir.y;
            hit.back.z = -hit.dir.z;
            SendMsg(target->handle, 0x407, &hit);
        }
    }
}

int HermitSpitAmmoMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    int busy = Fn80175D60(arg);

    switch (msg) {
    case 0x14967:
        HermitSpitAmmoFire(arg, busy, data, obj->unk0);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------
// 0x802698D0-0x8026A0F0: a list of "view sensor" objects (class 0x5F). Each one has a view frustum
// built from its placement; on an update message it looks for actors in range and inside the
// frustum and tells its own actor which one it sees. It uses its own float conversion constant in
// .sdata2 (0x804DA1B0), so this part is most likely another file.

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
int fn_8001F928(Sensor* actor, IVec3* pos, u32 flags, int a, int b);
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

inline void IVecToVec(Vec3* out, IVec3 in) {
    out->x = in.x;
    out->y = in.y;
    out->z = in.z;
}

// Called for each actor in range: if it is inside the sensor's frustum and the sensor has a clear
// line to it, the sensor's actor is told.
int SensorSeeActor(SensorActor* actor, Sensor* s) {
    Vec3 apos;
    IVec3 spos;
    u32 out;

    spos = s->place.pos;
    IVecToVec(&apos, actor->pos);
    if (ViewFrustumClassifyPoints((int*)s->planes, &apos, &out, 1, 0) == 0 &&
        fn_8001F928(s, &spos, 0x80000000, 0, 0) == 0) {
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

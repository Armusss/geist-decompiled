// u_80251CB8.cpp: the "IMP FIRE" effect type (0x8025362C-0x802540F4: Update drafted, Reset/Query/Msg/Destroy/Create
// match). Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);

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

struct Target {
    u8 pad[0x24];
    int x, y, z;
};

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
void fn_802540F4(void);
void fn_80254164(void);

inline float Min(float a, float b) { return a < b ? a : b; }

struct Vec3 {
    float x, y, z;
};


// Launch parameters for the fire ball (see fn_802541D4).
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
void fn_802541D4(int arg, ImpThrow* t, int n);

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

extern EffectType lbl_803BEDE8;  // the "IMP FIRE" type descriptor

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
                t.vel.x = 0.0f;
                t.vel.y = 0.0f;
                t.vel.z = 3500.0f;
                dst = g->shot.target;
                d.x = dst->x;
                d.y = dst->y;
                d.z = dst->z;
                d.x -= t.pos.x;
                d.y -= t.pos.y;
                d.z -= t.pos.z;
                up = -(t.vel.z / t.accel.z);
                root = FastSqrt((d.z - 0.5f * t.vel.z * up) / (0.5f * t.accel.z));
                {
                    float len = fn_800E1020(&d);
                    d.z = 0.0f;
                    fn_800E1098(&d, &d, len / (up + root));
                }
                t.vel.x += d.x;
                t.vel.y += d.y;
                t.vel.z += d.z;
                fn_802541D4(g->arg, &t, 1);
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
        fn_80254164();
        return 1;
    case 0x0CAE7B51:
        fn_802540F4();
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
    EffectType* type = &lbl_803BEDE8;
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


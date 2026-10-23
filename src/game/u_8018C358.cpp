// u_8018C358.cpp: the "SPLINE GUN" effect type (0x8018C358-0x8018D2D4).
// A chargeable gun: while charging (state 2) a spline shot builds up over three seconds and a light
// follows the shot; releasing fires the spline bullet from the definition's second ammo. A plain shot
// fires the first ammo. Its reset function (0x8018C2F8) sits just before this unit's split.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include <string.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_800F7D28(int emitter, int id);
void fn_800F825C(int emitter, int id, int count);
void fn_800F86A0(int emitter);
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
int FxStart(void* fx, int a, int b);
void fn_8016EDD8(int handle, int* pos);
void fn_8016EEF4(int handle);
void fn_800E0E9C(int* out, int* in, int scale);
void fn_800B4C50(float* pos, int arg, int a, int b, float f1, float f2);
void* fn_8015A78C(u8 p1, u8 p2, u8 p3, u8 p4, u8 p5, u8 p6, u8 p7, u8 p8);
void fn_80159AF4(void* light, u8 r, u8 g, u8 b);
void fn_8015A10C(void* light);
void fn_8015A4AC(float a, void* light, void* shot, int active, int arg, float b, void* pos);
void fn_8000F550(short* ids, int count);
void fn_80163854(int player, int a, int b);
int SendMsg(int target, int msg, void* data);
}

void* VAlloc(int size);
void VFree(void* p);
void Fn800F840C(int emitter, int a, int b);
void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
int Fn80175D60(unsigned long arg);
struct Actor;
void ActorAim(int* out, Actor* a, int b);
void* FxFind(const char* name);
int PlayerGetPossessor(int arg);
int AssetFind(u32 table, u32 key);
void TextureLoad(short* ids, int count);

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

struct MsgResult {
    void* p;
    char pad[0x1C];
};

struct Target {
    u8 pad[0x24];
    int x, y, z;
};

struct ShotHit {
    u8 pad[0x30];
    float x, y, z;
    u8 pad3C[0x24];
};

struct SplineShot {
    ShotHit* hit;
    float speed;
    int unk8;
    int charge;
    int unk10;
    int unk14;
};

struct SplineGunDef {
    int unk0;
    int id;
    int max;
    float speed;
    int unk10;
    int unk14;
    int unk18;
    int drain;
    u8 pad20[0x30 - 0x20];
    void* fx;
    u8 pad34[0x3C - 0x34];
    void* hitFx;
    u8 pad40[0x64 - 0x40];
    int id2;
    int max2;
    float speed2;
    u8 pad70[0x90 - 0x70];
    void* fx2;
    void* hitFx2;
    u8 pad98[0xBC - 0x98];
    int unkBC;
};

struct SplineGun {
    int state;
    float charge;
    int emitter;
    SplineGunDef* def;
    int unk10;
    float progress;
    int fx;
    float range;
    float length;
    int color;
    void* light;
    int arg;
    int pad30;
    ShotHit shot;
};

void SplineGunReset(EffectObj* obj);  // 0x8018C2F8, just before this unit's split
void SplineGunUpdate(EffectObj* obj, float dt);
u32 SplineGunQuery(EffectObj* obj, int msg);
int SplineGunMsg(EffectObj* obj, int msg, int arg, void* data, float dt);
void SplineGunDestroy(EffectObj* obj);
void* SplineGunCreate(int a, int id);

EffectType gSplineGunType = {
    "SPLINE GUN", 0, {0, 0, 0}, SplineGunCreate, SplineGunDestroy, SplineGunMsg, SplineGunQuery, SplineGunUpdate,
    SplineGunReset, 0,
};


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

inline void SplineGunTrack(int fx, int target) {
    MsgResult q;
    int pos[3];

    if (fx != -1 && target != -1 && SendMsg(target, 0x400, &q) && q.p) {
        ActorAim(pos, (Actor*)q.p, 1);
        fn_800E0E9C(pos, pos, 1000);
        pos[0] = pos[0] + ((Target*)q.p)->x;
        pos[1] = pos[1] + ((Target*)q.p)->y;
        pos[2] = pos[2] + ((Target*)q.p)->z;
        fn_8016EDD8(fx, pos);
    }
}

void SplineGunUpdate(EffectObj* obj, float dt) {
    SplineGun* g = (SplineGun*)obj->data;
    SplineGunDef* def = g->def;
    int n = fn_800F7D28(g->emitter, g->unk10);

    if (g->state == 2 && g->progress < 3.0f && n > 1) {
        g->progress += dt / 1000.0f;
        g->length = g->progress / 3.0f * g->range;
    }
    if (g->fx != -1) {
        SplineGunTrack(g->fx, g->arg);
    }
    if (g->charge > 0.0f) {
        g->charge -= 0.001f * dt * def->drain;
        g->charge = g->charge > 0.0f ? g->charge : 0.0f;
    }
}

u32 SplineGunQuery(EffectObj* obj, int msg) {
    SplineGun* g = (SplineGun*)obj->data;
    SplineGunDef* def = g->def;

    switch (msg) {
    case 0x96224FCC:
    case 0x0020EE36:
        if (fn_800F7D28(g->emitter, g->unk10)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->id)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0x0D19E338:
        if (g->state == 1) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
    case 0xE6B71BCC:
        if (fn_800F7D28(g->emitter, g->unk10) == def->max) {
            return 0x4848CA1E;
        }
        if (fn_800F7D28(obj->emitter, def->id)) {
            return 0x04A3E183;
        }
        return 0x11D2C025;
    case 0xF829DB93:
    case 0x42E50A3D:
        if (fn_800F7D28(g->emitter, def->id2)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->id2)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0x9791AF51:
        if (g->state == 2) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
    case 0x968CBE3D:
        if (fn_800F7D28(g->emitter, def->id2) == def->max2) {
            return 0x4848CA1E;
        }
        if (fn_800F7D28(obj->emitter, def->id2)) {
            return 0x04A3E183;
        }
        return 0x11D2C025;
    case 0x6FAB23FE: {
        float r = g->progress / 3.0f;
        if (r < 0.5f) {
            return 0x7183F329;
        }
        if (r < 1.0f) {
            return 0x7183F5A1;
        }
        return 0xBEF8884D;
    }
    }
    return 0x3DA260F7;
}

int SplineGunMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    SplineGun* g = (SplineGun*)obj->data;
    SplineGunDef* def = g->def;
    int busy = Fn80175D60(arg);
    SplineShot infoA;
    SplineShot infoB;
    int posC[3];
    int posD[3];
    int posA[3];
    int posB[3];
    int posE[3];
    int posF[3];

    g->arg = arg;
    switch (msg) {
    case 0x0020EE36:
        if (g->state == 1) {
            memcpy(&g->shot, data, 0x60);
        } else {
            float* p;
            void* fx;
            int h;

            if (!fn_800F7D28(g->emitter, def->id)) {
                return 0;
            }
            if (((arg >> 24) & 0xFF) == 2) {
                fn_800B4C50(&g->shot.x, arg, 2, 1, 101.0f, 32000.0f);
            }
            infoA.hit = (ShotHit*)data;
            infoA.speed = def->speed;
            infoA.unk8 = def->unk14;
            infoA.charge = g->charge;
            infoA.unk14 = 1;
            infoA.unk10 = 0;
            Fn800F7F5C(g->emitter, def->id, 0x14967, arg, &infoA, dt);
            fx = def->fx;
            g->fx = FxStartAt(fx, &infoA.hit->x, posA);
        }
        return 1;
    case 0x96224FCC:
        if (g->state != 1) {
            g->state = 1;
        }
        memcpy(&g->shot, data, 0x60);
        return 1;
    case 0x0D19E338:
        g->state = 0;
        return 1;
    case 0xE6B71BCC: {
        int n;
        int m;
        int r;

        n = fn_800F7D28(g->emitter, def->id);
        if (n == def->max) {
            return 0;
        }
        m = fn_800F7D28(obj->emitter, def->id);
        if (m == 0) {
            return 0;
        }
        n = (m < def->max - n) ? m : def->max - n;
        {
            QueryMsg qA;
            qA.hash = 0x5A32795A;
            *(int*)&qA.result = 0;
            qA.unk8 = 0;
            SendMsg(arg, 0x41E, &qA);
            r = *(int*)&qA.result;
        }
        if (r == 0) {
            fn_800F825C(obj->emitter, def->id, n);
        }
        Fn800F840C(g->emitter, def->id, n);
        return 1;
    }
    case 0xF829DB93:
        if (g->state == 2) {
            memcpy(&g->shot, data, 0x60);
        } else {
            float* p;
            void* fx;
            int h;

            if (!fn_800F7D28(g->emitter, def->id2)) {
                return 0;
            }
            infoB.hit = (ShotHit*)data;
            infoB.speed = def->speed2;
            infoB.unk8 = def->unk14;
            infoB.charge = g->charge;
            infoB.unk10 = 1;
            infoB.unk14 = (int)(10.0f * g->progress) + 1;
            Fn800F7F5C(g->emitter, def->id2, 0x14967, arg, &infoB, dt);
            fx = def->fx2;
            g->fx = FxStartAt(fx, &infoB.hit->x, posB);
        }
        return 1;
    case 0x42E50A3D:
        if (g->state != 2) {
            g->state = 2;
            g->progress = 0.0f;
        }
        memcpy(&g->shot, data, 0x60);
        if (g->light == 0) {
            g->light = fn_8015A78C(0x77, 0x39, 0xF0, 0xFF, 0x77, 0x39, 0xF0, 0);
            if (g->light) {
                if (g->color == 0) {
                    fn_80159AF4(g->light, 0, 0x50, 0xC8);
                } else {
                    fn_80159AF4(g->light, 0xFF, 0x64, 0);
                }
            }
        }
        posC[0] = g->shot.x;
        posC[1] = g->shot.y;
        posC[2] = g->shot.z;
        g->fx = fn_8016F120(FxFind("Ghost_charge"), posC);
        return 1;
    case 0x9791AF51:
        g->state = 0;
        if (g->light) {
            fn_8015A10C(g->light);
            g->light = 0;
        }
        fn_8016EEF4(g->fx);
        g->fx = -1;
        return 1;
    case 0x968CBE3D: {
        int n;
        int m;
        int k;
        int r;

        if (data == 0) {
            return 0;
        }
        n = fn_800F7D28(g->emitter, def->id2);
        if (n == def->max2) {
            return 0;
        }
        m = fn_800F7D28(obj->emitter, def->id2);
        if (m == 0) {
            return 0;
        }
        k = (m < 1) ? m : 1;
        {
            QueryMsg qB;
            qB.hash = 0x5A32795A;
            *(int*)&qB.result = 0;
            qB.unk8 = 0;
            SendMsg(arg, 0x41E, &qB);
            r = *(int*)&qB.result;
        }
        if (r == 0) {
            fn_800F825C(obj->emitter, def->id2, k);
        }
        Fn800F840C(g->emitter, g->unk10, def->max2 - n);
        return 1;
    }
    case 0x315A1A7F:
        memcpy(&g->shot, data, 0x60);
        if (g->light) {
            fn_8015A4AC(g->progress, g->light, &g->shot, 1, g->color, g->length, &((ShotHit*)data)->x);
            if (((arg >> 24) & 0xFF) == 2) {
                fn_800B4C50(&g->shot.x, arg, 2, 1, 101.0f, 32000.0f);
            }
        }
        if (g->fx != -1) {
            posD[0] = g->shot.x;
            posD[1] = g->shot.y;
            posD[2] = g->shot.z;
            fn_8016EDD8(g->fx, posD);
        }
        return 1;
    case 0x8FDD7599: {
        ShotHit* hit;
        void* fx;
        int h;

        fx = def->hitFx;
        hit = (ShotHit*)data;
        g->fx = FxStartAt(fx, &hit->x, posE);
        return 1;
    }
    case 0xBC78F550: {
        ShotHit* hit;
        void* fx;
        int h;

        fx = def->hitFx2;
        hit = (ShotHit*)data;
        g->fx = FxStartAt(fx, &hit->x, posF);
        return 1;
    }
    case 0x0B59E861:
        if (busy == 0) {
            *(int*)((u8*)data + 0x84) = def->unkBC;
            return 1;
        }
        break;
    case 0xE452286F: {
        int player = PlayerGetPossessor(arg);
        short tex = AssetFind(0xF2E2BEB8, 0x2184AD04);
        if ((int)data == 1) {
            TextureLoad(&tex, 1);
        } else {
            fn_8000F550(&tex, 1);
        }
        fn_80163854(player, player + 0x1508, def->unkBC);
        return 1;
    }
    case 0x141109BD:
        return 1;
    }
    return 0;
}

void SplineGunDestroy(EffectObj* obj) {
    SplineGun* g = (SplineGun*)obj->data;

    if (g->light != 0) {
        fn_8015A10C(g->light);
        g->light = 0;
    }
    if (g->fx != -1) {
        fn_8016EEF4(g->fx);
        g->fx = -1;
    }
    fn_800F86A0(g->emitter);
    VFree(g);
}

void* SplineGunCreate(int a, int id) {
    EffectType* type = &gSplineGunType;
    SplineGun* g;

    EffectTypeHash(type);
    g = (SplineGun*)VAlloc(sizeof(SplineGun));
    memset(g, 0, sizeof(SplineGun));
    if (g == 0) {
        return 0;
    }
    g->def = (SplineGunDef*)fn_800EB21C(id);
    g->fx = -1;
    g->progress = 0.0f;
    g->light = 0;
    g->range = 100.0f;
    g->charge = 0.0f;
    g->state = 0;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->unk10, g->def->max);
    Fn800F840C(g->emitter, g->def->id, 1);
    Fn800F840C(g->emitter, g->def->id2, g->def->max2);
    return g;
}

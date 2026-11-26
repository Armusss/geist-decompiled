// u_800E3EC4.cpp (0x800E3EC4-0x800E48A0): the "Pistol" effect type, split from the old u_800E1B5C range.
// Boundary evidence: descriptor 0x8039C428 lists reset 0x800E3EC4, update 0x800E3EC8, query 0x800E4030,
// msg 0x800E4154, destroy 0x800E4724 and create 0x800E475C: exactly this range; .rodata pool
// 0x80344EF8-0x80344F48 (ghost-action table + "Pistol"); .sdata2 0x804D71D8-0x804D71F0 (own 0.0f and double).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include <string.h>
#include "game/types.h"
#include "game/msg.h"
#include "game/effect.h"

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_80019A04();
int fn_800F8AF4(int id);
void fn_80175EE4(void* p);
int fn_800F7D28(int emitter, int id);
int SendMsg(int to, int msg, void* data);
void fn_800F825C(int emitter, int id, int count);
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
int FxStart(void* fx, int a, int b);
void fn_80163824(int player, u32 id, int a, int b, int c);
void fn_80163854(int player, int a, int b);
}
int PlayerGetPossessor(int arg);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);
unsigned long Fn80175D60(unsigned long arg);
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f);
struct Player;
void PlayerAnimOffUpdate(Player* p, float dt);

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);
// Time of the last hit spark (at most one every 3 frames).
static int sPistolHitTime;

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

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct PistolDef {
    u8 pad00[0x4];
    int unk04;              // 0x04: emitter row
    int unk08;              // 0x08: clip size
    float speed;            // 0x0C
    int unk10;              // 0x10
    int unk14;              // 0x14
    int charge;             // 0x18: charge added per shot
    int drain;              // 0x1C: charge lost per second
    int maxCharge;          // 0x20
    int unk24;              // 0x24
    int model;              // 0x28
    void* sparkFx;          // 0x2C
    void* fx;               // 0x30
    u8 pad34[0x3C - 0x34];
    void* hitFx;            // 0x3C
    u8 pad40[0x4C - 0x40];
    float recoil;           // 0x4C
    float recoilRate;       // 0x50
    u8 pad54[0xBC - 0x54];
    int hud;                // 0xBC
};

// The shot passed with the fire messages (only the hit position is read here).
struct PistolShot {
    u8 pad00[0x30];
    float x, y, z;          // 0x30
};

struct PistolShotInfo {
    PistolShot* hit;        // 0x00
    float speed;            // 0x04
    float unk8;             // 0x08
    int unkC;               // 0x0C
    int charge;             // 0x10
    int unk14;              // 0x14
    int unk18;              // 0x18
    int unk1C;              // 0x1C
    int unk20;              // 0x20
};

inline float Min(float a, float b) { return a < b ? a : b; }

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

inline void Query(int* out, int target, int msg, u32 hash) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    *out = *(int*)&m.result;
}

struct Pistol {
    int emitter;            // 0x00
    PistolDef* def;         // 0x04
    int unk08;              // 0x08
    u32 arg;                // 0x0C: the holder's handle
    int unk10;              // 0x10: fx handle
    float unk14;            // 0x14: charge
    float unk18;            // 0x18: recoil
    float unk1C;            // 0x1C: recoil timer
};

extern "C" void fn_800E3EC4(EffectObj* obj);
extern "C" void fn_800E3EC8(EffectObj* obj, float dt);
extern "C" u32 fn_800E4030(EffectObj* obj, int msg);
extern "C" int fn_800E4154(EffectObj* obj, int msg, int arg, void* data, float dt);
extern "C" void fn_800E4724(EffectObj* obj);
void* PistolCreate(int a, int id);

EffectType gPistolType = {
    "Pistol", 0, {0, 0, 0}, PistolCreate, fn_800E4724, fn_800E4154, fn_800E4030, fn_800E3EC8, fn_800E3EC4, 0,
};

//@@800E3EC4
extern "C" void fn_800E3EC4(EffectObj* obj);
void fn_800E3EC4(EffectObj* obj) {}
//@@end
//@@800E3EC8
// Update: drains the charge and plays the recoil back once the recoil timer has run out.
extern "C" void fn_800E3EC8(EffectObj* obj, float dt);
void fn_800E3EC8(EffectObj* obj, float dt) {
    Pistol* g = (Pistol*)obj->data;
    PistolDef* def = g->def;

    if (g->unk14 > 0.0f) {
        g->unk14 -= 0.001f * dt * def->drain;
        g->unk14 = g->unk14 > 0.0f ? g->unk14 : 0.0f;
    }
    g->unk1C -= dt;
    g->unk1C = g->unk1C > 0.0f ? g->unk1C : 0.0f;
    if (g->unk18 > 0.0f && 0.0f == g->unk1C) {
        float step = def->recoilRate * dt;
        if (step <= 0.0f) {
            g->unk18 = 0.0f;
        }
        if (g->unk18 > step) {
            if ((int)(g->arg >> 24) == 2) {
                MsgResult q;
                SendMsg(g->arg, 0x400, &q);
                PlayerAnimOffUpdate((Player*)q.p, -step);
            }
            g->unk18 -= step;
        } else {
            if ((int)(g->arg >> 24) == 2) {
                MsgResult q;
                SendMsg(g->arg, 0x400, &q);
                PlayerAnimOffUpdate((Player*)q.p, -g->unk18);
            }
            g->unk18 = 0.0f;
        }
    }
}
//@@end

//@@800E4030
// Query: the fire/reload answers for the shot messages.
extern "C" u32 fn_800E4030(EffectObj* obj, int msg);
u32 fn_800E4030(EffectObj* obj, int msg) {
    Pistol* g = (Pistol*)obj->data;
    PistolDef* def = g->def;

    switch (msg) {
    case 0xF829DB93:
    case 0x0020EE36:
        if (fn_800F7D28(g->emitter, def->unk04)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->unk04)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0xE6B71BCC:
    case 0x968CBE3D:
        if (def->unk08 == fn_800F7D28(g->emitter, def->unk04)) {
            return 0x4848CA1E;
        }
        if (fn_800F7D28(obj->emitter, def->unk04)) {
            return 0x04A3E183;
        }
        return 0x11D2C025;
    }
    return 0x3DA260F7;
}
//@@end
//@@800E4154
// Message handler: fire, reload, hit sparks, HUD refresh and model loading.
extern "C" int fn_800E4154(EffectObj* obj, int msg, int arg, void* data, float dt);
int fn_800E4154(EffectObj* obj, int msg, int arg, void* data, float dt) {
    Pistol* g = (Pistol*)obj->data;
    PistolDef* def = g->def;
    int busy;

    if (g == 0) {
        return 0;
    }
    busy = Fn80175D60(arg);
    g->arg = arg;
    switch (msg) {
    case 0x0020EE36:
    case 0xF829DB93: {
        PistolShotInfo info;
        MsgResult q;
        int posB[3];
        int posA[3];
        void* fx;
        int kind;

        if (!fn_800F7D28(g->emitter, def->unk04)) {
            return 0;
        }
        g->unk08 = 1;
        g->unk14 += def->charge;
        g->unk14 = Min(g->unk14, def->maxCharge);
        kind = (arg >> 24) & 0xFF;
        if (kind == 2) {
            SendMsg(arg, 0x400, &q);
            PlayerAnimOffUpdate((Player*)q.p, def->recoil);
            g->unk18 += def->recoil;
            g->unk1C = 300.0f;
        }
        info.hit = (PistolShot*)data;
        info.speed = def->speed;
        info.unkC = def->unk14;
        info.charge = g->unk14;
        info.unk18 = def->model;
        info.unk1C = (int)def->sparkFx;
        Fn800F7F5C(g->emitter, def->unk04, 0x14967, arg, &info, dt);
        if (fn_80019A04() - sPistolHitTime >= 3 && kind == 2) {
            FxStartAt(def->sparkFx, &info.hit->x, posB);
            sPistolHitTime = fn_80019A04();
        }
        fx = def->fx;
        g->unk10 = FxStartAt(fx, &info.hit->x, posA);
        fn_800F825C(g->emitter, def->unk04, 1);
        return 1;
    }
    case 0xE6B71BCC:
    case 0x968CBE3D: {
        int n;
        int m;
        int r;

        g->unk08 = 1;
        n = fn_800F7D28(g->emitter, def->unk04);
        if (n == def->unk08) {
            return 0;
        }
        m = fn_800F7D28(obj->emitter, def->unk04);
        if (m == 0) {
            return 0;
        }
        n = (m < def->unk08 - n) ? m : def->unk08 - n;
        Query(&r, arg, 0x41E, 0x5A32795A);
        if (r == 0) {
            fn_800F825C(obj->emitter, def->unk04, n);
        }
        Fn800F840C(g->emitter, def->unk04, n);
        return 1;
    }
    case 0x8FDD7599:
    case 0xBC78F550: {
        PistolShot* hit;
        void* fx;
        int pos[3];

        g->unk08 = 1;
        fx = def->hitFx;
        hit = (PistolShot*)data;
        g->unk10 = FxStartAt(fx, &hit->x, pos);
        return 1;
    }
    case 0x0B59E861:
        if (busy == 0) {
            int player = PlayerGetPossessor(arg);
            int a = fn_800F7D28(obj->emitter, def->unk04);
            int b = fn_800F7D28(g->emitter, def->unk04);
            int c = def->unk08;

            fn_80163824(player, 0x878DC2AD, a, b, c);
            fn_80163854(player, player + 0x1508, def->hud);
        }
        return 1;
    case 0xE452286F: {
        int player = PlayerGetPossessor(arg);
        short id;

        id = def->model;
        if ((int)data == 1) {
            ModelLoadIds(&id, 1);
        } else {
            ModelFreeIds(&id, 1);
        }
        fn_80163854(player, player + 0x1508, def->hud);
        return 1;
    }
    case 0x141109BD: {
        short id;

        id = def->model;
        if ((int)data == 1) {
            ModelLoadIds(&id, 1);
        } else {
            ModelFreeIds(&id, 1);
        }
        return 1;
    }
    }
    return 0;
}
//@@end
//@@800E4724
extern "C" void fn_800F86A0(int emitter);
void VFree(void* p);
extern "C" void fn_800E4724(EffectObj* obj);
void fn_800E4724(EffectObj* obj) {
    Pistol* g = (Pistol*)obj->data;
    fn_800F86A0(g->emitter);
    VFree(g);
}
//@@end

void* PistolCreate(int a, int id) {
    EffectType* type = &gPistolType;
    Pistol* g;

    EffectTypeHash(type);
    g = (Pistol*)VAlloc(sizeof(Pistol));
    memset(g, 0, sizeof(Pistol));
    if (g == 0) {
        return 0;
    }
    g->def = (PistolDef*)fn_800EB21C(id);
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    g->unk08 = 1;
    g->unk10 = -1;
    g->unk14 = 0.0f;
    g->unk18 = 0.0f;
    g->unk1C = 0.0f;
    sPistolHitTime = fn_80019A04();
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    return g;
}

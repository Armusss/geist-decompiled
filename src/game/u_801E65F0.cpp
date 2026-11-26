// u_801E65F0.cpp (0x801E65F0-0x801E72E0): the "Sniper" effect type, split from the old u_801E241C range.
// Boundary evidence:
// - .data: the type descriptor 0x803B3B00 lists reset 0x801E65F0, update 0x801E6600, query 0x801E6814,
//   msg 0x801E69AC, destroy 0x801E7160 and create 0x801E7198: exactly this range.
// - .rodata: pool 0x80348F18-0x80348F68, the ghost-action name table followed by "Sniper"; the next file has its
//   own table at 0x80348F68. .sdata2: own pool 0x804D8ED8-0x804D8EF0 (0.0f 0x804D8ED8, double 0x804D8EE0).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include <string.h>
#include "game/types.h"
#include "game/msg.h"
#include "game/effect.h"
#include "game/vecnorm.h"

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_80019A04();
int fn_800F7D28(int emitter, int id);
int SendMsg(int to, int msg, void* data);
void fn_800F825C(int emitter, int id, int count);
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
void fn_8016EDD8(int handle, int* pos);
int FxStart(void* fx, int a, int b);
void fn_800E0E9C(int* out, int* in, int scale);
void fn_80163824(int player, u32 id, int a, int b, int c);
void fn_80163810(int player, u32 id, int a, int b, int c);
void fn_80163854(int player, int a, int b);
}
struct Actor;
struct Player;
void PlayerAnimOffUpdate(Player* p, float dt);
void ActorAim(int* out, Actor* a, int b);
int PlayerGetPossessor(int arg);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);
unsigned long Fn80175D60(unsigned long arg);
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f);

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);

// Time of the last hit spark (at most one every 3 frames).
static int sSniperHitTime;

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

struct SniperDef {
    u32 kind;               // 0x00: weapon hash
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

struct Sniper {
    int unk00;              // 0x00: state
    float unk04;            // 0x04: charge
    float unk08;            // 0x08: recoil
    float unk0C;            // 0x0C: recoil timer
    int emitter;            // 0x10
    SniperDef* def;         // 0x14
    int unk18;              // 0x18
    int arg;                // 0x1C: the holder's handle
    u8 pad20[0x30 - 0x20];
    int unk30;              // 0x30: shot fx
    int unk34;              // 0x34: hit fx
};

// The shot passed with the fire messages.
struct SniperShot {
    u8 pad00[0x30];
    Vec3 pos;               // 0x30
    Vec3 dir;               // 0x3C
};

struct SniperShotInfo {
    SniperShot* hit;        // 0x00
    float speed;            // 0x04
    float unk8;             // 0x08
    int unkC;               // 0x0C
    int charge;             // 0x10
    int unk14;              // 0x14
    int unk18;              // 0x18
    int unk1C;              // 0x1C
    int unk20;              // 0x20
};

struct SniperTarget {
    u8 pad[0x24];
    int x, y, z;
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

inline void GunTrack(int fx, int target) {
    MsgResult q;
    int pos[3];

    if (fx != -1 && target != -1 && SendMsg(target, 0x400, &q) && q.p) {
        ActorAim(pos, (Actor*)q.p, 1);
        fn_800E0E9C(pos, pos, 1000);
        pos[0] = pos[0] + ((SniperTarget*)q.p)->x;
        pos[1] = pos[1] + ((SniperTarget*)q.p)->y;
        pos[2] = pos[2] + ((SniperTarget*)q.p)->z;
        fn_8016EDD8(fx, pos);
    }
}

extern "C" void fn_801E65F0(EffectObj* obj);
extern "C" void fn_801E6600(EffectObj* obj, float dt);
extern "C" u32 fn_801E6814(EffectObj* obj, int msg);
extern "C" int fn_801E69AC(EffectObj* obj, int msg, int arg, void* data, float dt);
extern "C" void fn_801E7160(EffectObj* obj);
void* SniperCreate(int a, int id);

EffectType gSniperType = {
    "Sniper", 0, {0, 0, 0}, SniperCreate, fn_801E7160, fn_801E69AC, fn_801E6814, fn_801E6600, fn_801E65F0, 0,
};



//@@801E65F0
void fn_801E65F0(EffectObj* obj) {
    ((Sniper*)obj->data)->unk00 = 0;
}
//@@end
//@@801E6600
// Update: keeps the hit fx on the holder's aim, drains the charge and plays the recoil back.
void fn_801E6600(EffectObj* obj, float dt) {
    Sniper* g = (Sniper*)obj->data;
    SniperDef* def = g->def;

    if (g->unk34 != -1) {
        GunTrack(g->unk34, g->arg);
    }
    if (g->unk04 > 0.0f) {
        g->unk04 -= 0.001f * dt * def->drain;
        g->unk04 = g->unk04 > 0.0f ? g->unk04 : 0.0f;
    }
    g->unk0C -= dt;
    g->unk0C = g->unk0C > 0.0f ? g->unk0C : 0.0f;
    if (g->unk08 > 0.0f && 0.0f == g->unk0C) {
        float step = def->recoilRate * dt;
        if (step <= 0.0f) {
            g->unk08 = 0.0f;
        }
        if (g->unk08 > step) {
            if (((g->arg >> 24) & 0xFF) == 2) {
                MsgResult q;
                SendMsg(g->arg, 0x400, &q);
                PlayerAnimOffUpdate((Player*)q.p, -step);
            }
            g->unk08 -= step;
        } else {
            if (((g->arg >> 24) & 0xFF) == 2) {
                MsgResult q;
                SendMsg(g->arg, 0x400, &q);
                PlayerAnimOffUpdate((Player*)q.p, -g->unk08);
            }
            g->unk08 = 0.0f;
        }
    }
}
//@@end

//@@801E6814
// Query: the fire/reload answers for the shot messages.
u32 fn_801E6814(EffectObj* obj, int msg) {
    Sniper* g = (Sniper*)obj->data;
    SniperDef* def = g->def;

    switch (msg) {
    case 0x96224FCC:
    case 0x0020EE36:
        if (fn_800F7D28(g->emitter, def->unk04)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->unk04)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0xF829DB93:
    case 0x42E50A3D:
        return 0x04A3E183;
    case 0x0D19E338:
        return 0x04A3E183;
    case 0x9791AF51:
        return 0x04A3E183;
    case 0x968CBE3D:
    case 0xE6B71BCC:
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

//@@801E69AC
// Message handler: fire, reload, hit sparks, HUD refresh and model loading.
int fn_801E69AC(EffectObj* obj, int msg, int arg, void* data, float dt) {
    int kind;
    Sniper* g = (Sniper*)obj->data;
    SniperDef* def = g->def;
    int busy = Fn80175D60(arg);
    Vec3 dirA;
    Vec3 dirB;

    g->arg = arg;
    switch (msg) {
    case 0x0020EE36: {
        SniperShotInfo info;
        MsgResult q;
        int posB[3];
        int posA[3];
        void* fx;

        if (!fn_800F7D28(g->emitter, def->unk04)) {
            return 0;
        }
        g->unk04 += def->charge;
        g->unk04 = Min(g->unk04, def->maxCharge);
        kind = (arg >> 24) & 0xFF;
        if (kind == 2) {
            SendMsg(arg, 0x400, &q);
            PlayerAnimOffUpdate((Player*)q.p, def->recoil);
            g->unk08 += def->recoil;
        }
        info.hit = (SniperShot*)data;
        info.speed = def->speed;
        info.unkC = def->unk14;
        info.charge = g->unk04;
        info.unk18 = def->model;
        info.unk1C = (int)def->sparkFx;
        Fn800F7F5C(g->emitter, def->unk04, 0x14967, arg, &info, dt);
        VecNormalize(&dirA, &info.hit->dir);
        dirA.x += info.hit->pos.x;
        dirA.y += info.hit->pos.y;
        dirA.z += info.hit->pos.z;
        if (fn_80019A04() - sSniperHitTime >= 3 && kind == 2) {
            FxStartAt(def->sparkFx, &dirA.x, posB);
            sSniperHitTime = fn_80019A04();
        }
        fx = def->fx;
        g->unk30 = FxStartAt(fx, &dirA.x, posA);
        fn_800F825C(g->emitter, def->unk04, 1);
        return 1;
    }
    case 0x315A1A7F:
        return 1;
    case 0xF829DB93:
        return 1;
    case 0x0D19E338:
        g->unk00 = 0;
        return 1;
    case 0xE6B71BCC:
    case 0x968CBE3D: {
        int n;
        int m;
        int r;

        g->unk18 = 1;
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
        SniperShot* hit;
        void* fx;
        int pos[3];

        g->unk18 = 1;
        hit = (SniperShot*)data;
        VecNormalize(&dirB, &hit->dir);
        dirB.x += hit->pos.x;
        dirB.y += hit->pos.y;
        dirB.z += hit->pos.z;
        fx = def->hitFx;
        g->unk34 = FxStartAt(fx, &dirB.x, pos);
        return 1;
    }
    case 0x0B59E861:
        if (busy == 0) {
            int player = PlayerGetPossessor(arg);
            int a = fn_800F7D28(obj->emitter, def->unk04);
            int b = fn_800F7D28(g->emitter, def->unk04);
            int c = def->unk08;

            fn_80163824(player, 0x878DC2AD, a, b, c);
            if (def->kind == 0x920FC8EF) {
                fn_80163810(player, 0xBA915556, -1, -1, -1);
            } else {
                fn_80163810(player, 0xEA9B9EE8, -1, -1, -1);
            }
            fn_80163854(player, player + 0x1508, def->hud);
        }
        return 1;
    case 0xE452286F: {
        short id;

        id = def->model;
        if ((int)data == 1) {
            ModelLoadIds(&id, 1);
        } else {
            ModelFreeIds(&id, 1);
        }
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
//@@801E7160
extern "C" void fn_800F86A0(int emitter);
void VFree(void* p);
void fn_801E7160(EffectObj* obj) {
    Sniper* g = (Sniper*)obj->data;
    fn_800F86A0(g->emitter);
    VFree(g);
}
//@@end

void* SniperCreate(int a, int id) {
    EffectType* type = &gSniperType;
    Sniper* g;

    EffectTypeHash(type);
    g = (Sniper*)VAlloc(sizeof(Sniper));
    memset(g, 0, sizeof(Sniper));
    if (g == 0) {
        return 0;
    }
    g->def = (SniperDef*)fn_800EB21C(id);
    g->unk00 = 0;
    g->unk30 = -1;
    g->unk34 = -1;
    g->unk04 = 0.0f;
    g->unk08 = 0.0f;
    g->unk0C = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    sSniperHitTime = fn_80019A04();
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    return g;
}


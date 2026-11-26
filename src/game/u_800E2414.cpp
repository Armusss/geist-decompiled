// u_800E2414.cpp (0x800E2414-0x800E39F4): the "AUG" effect type, split from the old u_800E1B5C range.
// Boundary evidence: the descriptor 0x8039C3C8 lists reset 0x800E2414, update 0x800E244C, query 0x800E288C,
// msg 0x800E2A24, destroy 0x800E3820, create 0x800E3860 and 0x800E39D0 (slot +0x0C); .rodata pool
// 0x80344E58-0x80344EA8 (ghost-action table + "AUG"); .sdata2 0x804D71A0-0x804D71C0 (own 0.0f and double).
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
void fn_8016EDD8(int handle, int* pos);
void fn_800E0E9C(int* out, int* in, int scale);
int FxStart(void* fx, int a, int b);
void fn_80175DE8(void* p, float dt);
}
struct Actor;
struct Player;
void PlayerAnimOffUpdate(Player* p, float dt);
void ActorAim(int* out, Actor* a, int b);
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f);

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);
extern int lbl_804D4A10;
extern int lbl_804D4A18;

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

struct AugDef {
    u32 kind;               // 0x00
    int unk04;              // 0x04: emitter row
    int unk08;              // 0x08: clip size
    float speed;            // 0x0C
    float unk10;            // 0x10
    int unk14;              // 0x14
    int charge;             // 0x18: charge added per shot
    int drain;              // 0x1C: charge lost per second
    int maxCharge;          // 0x20
    int spread;             // 0x24: spread added per shot
    int model;              // 0x28
    void* sparkFx;          // 0x2C
    void* fx;               // 0x30
    u8 pad34[0x4C - 0x34];
    float recoil;           // 0x4C
    float recoilRate;       // 0x50
};

// The shot the gun keeps firing while the trigger is held (copied from the fire messages).
struct AugShot {
    u8 pad00[0x30];
    float x, y, z;          // 0x30
    Vec3 dir;               // 0x3C
    u8 pad48[0x50 - 0x48];
    float spread;           // 0x50
    float scale;            // 0x54
    u8 pad58[0x60 - 0x58];
};

struct Aug {
    int unk00;              // 0x00: state (1 firing)
    float unk04;            // 0x04: time to the next shot
    float unk08;            // 0x08: time between shots
    int emitter;            // 0x0C
    AugDef* def;            // 0x10
    int unk14;              // 0x14
    u32 arg;                // 0x18: the holder's handle
    int unk1C;              // 0x1C
    AugShot shot;           // 0x20
    int unk80;              // 0x80
    int unk84;              // 0x84: shot fx
    float unk88;            // 0x88: charge
    float unk8C;            // 0x8C: recoil
    float unk90;            // 0x90: recoil timer
    int unk94;              // 0x94
    u8 pad98[0x38];
};

struct AugShotInfo {
    AugShot* hit;           // 0x00
    float speed;            // 0x04
    float unk8;             // 0x08
    int unkC;               // 0x0C
    int charge;             // 0x10
    int unk14;              // 0x14
    int unk18;              // 0x18
    int unk1C;              // 0x1C
    int unk20;              // 0x20
};

struct AugTarget {
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

inline void GunTrack(int fx, int target) {
    MsgResult q;
    int pos[3];

    if (fx != -1 && target != -1 && SendMsg(target, 0x400, &q) && q.p) {
        ActorAim(pos, (Actor*)q.p, 1);
        fn_800E0E9C(pos, pos, 1000);
        pos[0] = pos[0] + ((AugTarget*)q.p)->x;
        pos[1] = pos[1] + ((AugTarget*)q.p)->y;
        pos[2] = pos[2] + ((AugTarget*)q.p)->z;
        fn_8016EDD8(fx, pos);
    }
}

extern EffectType gAugType;  // the "AUG" type descriptor

//@@800E2414
extern "C" void fn_80175DA8(void* p);
extern "C" void fn_800E2414(EffectObj* obj);
void fn_800E2414(EffectObj* obj) {
    Aug* g = (Aug*)obj->data;
    g->unk00 = 0;
    g->unk04 = 0.0f;
    fn_80175DA8(&g->unk94);
}
//@@end
//@@800E244C
// Update: fires while the trigger is held, keeps the shot fx on the holder's aim, drains the charge and plays the
// recoil back.
extern "C" void fn_800E244C(EffectObj* obj, float dt);
void fn_800E244C(EffectObj* obj, float dt) {
    Aug* g = (Aug*)obj->data;
    AugDef* def = g->def;

    g->unk14 = 0;
    if (g->unk00 == 1) {
        g->unk04 -= dt;
        while (g->unk04 <= 0.0f) {
            AugShotInfo info;
            int pos[3];
            void* fx;

            g->unk04 += g->unk08;
            if (!fn_800F7D28(g->emitter, def->unk04)) {
                return;
            }
            info.hit = &g->shot;
            info.speed = def->speed;
            info.unk8 = def->unk10;
            info.unkC = def->unk14;
            info.charge = g->unk88;
            info.unk18 = def->model;
            info.unk1C = (int)def->sparkFx;
            info.hit->spread += def->spread;
            Fn800F7F5C(g->emitter, def->unk04, 0x14967, g->arg, &info, dt);
            fx = def->fx;
            g->unk84 = FxStartAt(fx, &info.hit->x, pos);
            GunTrack(g->unk84, g->arg);
            fn_800F825C(g->emitter, def->unk04, 1);
        }
    }
    if (g->unk84 != -1) {
        GunTrack(g->unk84, g->arg);
    }
    if (g->unk88 > 0.0f) {
        g->unk88 -= 0.001f * dt * def->drain;
        g->unk88 = g->unk88 > 0.0f ? g->unk88 : 0.0f;
    }
    g->unk90 -= dt;
    g->unk90 = g->unk90 > 0.0f ? g->unk90 : 0.0f;
    if (g->unk8C > 0.0f && 0.0f == g->unk90) {
        float step = def->recoilRate * dt;
        if (step <= 0.0f) {
            g->unk8C = 0.0f;
        }
        if (g->unk8C > step) {
            if ((int)(g->arg >> 24) == 2) {
                MsgResult q;
                SendMsg(g->arg, 0x400, &q);
                PlayerAnimOffUpdate((Player*)q.p, -step);
            }
            g->unk8C -= step;
        } else {
            if ((int)(g->arg >> 24) == 2) {
                MsgResult q;
                SendMsg(g->arg, 0x400, &q);
                PlayerAnimOffUpdate((Player*)q.p, -g->unk8C);
            }
            g->unk8C = 0.0f;
        }
    }
    fn_80175DE8(&g->unk94, dt);
}
//@@end

//@@800E288C
// Query: the fire/reload answers for the shot messages.
extern "C" u32 fn_800E288C(void* p, int msg);
u32 fn_800E288C(void* p, int msg) {
    EffectObj* obj = (EffectObj*)p;
    Aug* g = (Aug*)obj->data;
    AugDef* def = g->def;

    switch (msg) {
    case 0x96224FCC:
    case 0x0020EE36:
    case 0xF829DB93:
    case 0x42E50A3D:
        if (fn_800F7D28(g->emitter, def->unk04)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->unk04)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0x0D19E338:
    case 0x9791AF51:
        if (g->unk00 == 1) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
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
// TODO (not drafted yet, 0xDFC bytes at 0x800E2A24)
// fn_800E2A24
//@@800E3820
extern "C" void fn_80175EA4(void* p);
extern "C" void fn_800F86A0(int emitter);
void VFree(void* p);
extern "C" void fn_800E3820(EffectObj* obj);
void fn_800E3820(EffectObj* obj) {
    Aug* g = (Aug*)obj->data;
    fn_80175EA4(&g->unk94);
    fn_800F86A0(g->emitter);
    VFree(g);
}
//@@end

void* AugCreate(int a, int id) {
    EffectType* type = &gAugType;
    Aug* g;

    EffectTypeHash(type);
    g = (Aug*)VAlloc(sizeof(Aug));
    memset(g, 0, sizeof(Aug));
    if (g == 0) {
        return 0;
    }
    g->def = (AugDef*)fn_800EB21C(id);
    g->unk08 = 100.0f;
    g->unk00 = 0;
    g->unk04 = 0.0f;
    g->unk14 = 1;
    g->unk80 = fn_800F8AF4(id);
    g->unk84 = -1;
    g->unk88 = 0.0f;
    g->unk8C = 0.0f;
    g->unk90 = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    lbl_804D4A10 = fn_80019A04();
    fn_80175EE4(&g->unk94);
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    return g;
}

//@@800E39D0
extern "C" void fn_80175EF8(void* p);
extern "C" int fn_800E39D0(void* p);
int fn_800E39D0(void* p) {
    fn_80175EF8(p);
    return 1;
}
//@@end

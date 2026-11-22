// u_80299F0C.cpp (0x80299F0C-0x8029A5E4): the "SpitAndSqueal" effect type, split from the old u_80294548
// range (0x80294548-0x8029C0AC). Boundary evidence:
// - .sdata2: its own pool 0x804DA720-0x804DA728 (0.0f, 200.0f); the 0.0f duplicates the previous file's
//   0x804DA658 and the next file's 0x804DA730. Last reader of the previous pool is fn_80299E60 (via the
//   behaviour table 0x803C3050), first reader of this one is SpitAndSquealReset (0x80299F0C).
// - .rodata: the string pool 0x8034B7C0-0x8034B818 is the ghost-action name table (POSSESS..SCAN) followed
//   by "SpitAndSqueal"; the previous file's pool (0x8034B688-0x8034B7C0) has its own copy of the table.
// - .data: the type descriptor 0x803C30B0 (0x30 bytes) lists exactly the six functions of this range.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include <string.h>
#include "game/effect.h"

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_800F7D28(int emitter, int id);
void* fn_8016F120(void* fx, int* pos);
void fn_8016F118(void* handle, int a, int b);
void fn_800F86A0(int emitter);
}

void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
extern "C" void FxStart(void* fx, int a, int b);
void* VAlloc(int size);
void VFree(void* p);
void Fn800F840C(int emitter, int a, int b);
void Fn80175D60(unsigned long arg);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);

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

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}

// Per-type definition: two ammo slots (spit, squeal), each with an id, a maximum and two shot parameters.
struct SpitAndSquealDef {
    int unk00;
    int id;          // 0x04: first slot ammo id
    int max;         // 0x08
    float speed;     // 0x0C
    float unk10;     // 0x10
    u8 pad14[0x28 - 0x14];
    int model;       // 0x28
    u8 pad2C[0x30 - 0x2C];
    void* fx;        // 0x30
    u8 pad34[0x48 - 0x34];
    int model2;      // 0x48
    u8 pad4C[0x64 - 0x4C];
    int id2;         // 0x64: second slot ammo id
    int max2;        // 0x68
    float speed2;    // 0x6C
    float unk70;     // 0x70
    u8 pad74[0x88 - 0x74];
    int model3;      // 0x88
    u8 pad8C[0x98 - 0x8C];
    int model4;      // 0x98
};

struct SpitAndSqueal {
    int unk00;
    float timer;     // 0x04: reload time left
    float rate;      // 0x08
    int emitter;     // 0x0C
    SpitAndSquealDef* def;  // 0x10
    int arg;         // 0x14
    u8 pad18[0x10];
};

struct ShotHit {
    u8 pad[0x30];
    float x, y, z;
    u8 pad3C[0x4C - 0x3C];
    float scale;     // 0x4C
};

struct ShotInfo {
    ShotHit* hit;
    float speed;
    float unk8;
    int unkC;
    int charge;
    int unk14;
    int unk18;
    int unk1C;
    int unk20;
};

void SpitAndSquealReset(EffectObj* obj);
void SpitAndSquealUpdate(EffectObj* obj, float dt);
u32 SpitAndSquealQuery(EffectObj* obj, int msg);
int SpitAndSquealMsg(EffectObj* obj, int msg, int arg, void* data, float dt);
void SpitAndSquealDestroy(EffectObj* obj);
void* SpitAndSquealCreate(int a, int id);

EffectType gSpitAndSquealType = {
    "SpitAndSqueal", 0, {0, 0, 0}, SpitAndSquealCreate, SpitAndSquealDestroy, SpitAndSquealMsg,
    SpitAndSquealQuery, SpitAndSquealUpdate, SpitAndSquealReset, 0,
};

void SpitAndSquealReset(EffectObj* obj) {
    SpitAndSqueal* g = (SpitAndSqueal*)obj->data;
    g->unk00 = 0;
    g->timer = 0.0f;
}

void SpitAndSquealUpdate(EffectObj* obj, float dt) {
    SpitAndSqueal* g = (SpitAndSqueal*)obj->data;
    if (g->timer >= 0.0f) {
        g->timer -= dt;
    }
}

u32 SpitAndSquealQuery(EffectObj* obj, int msg) {
    SpitAndSqueal* g = (SpitAndSqueal*)obj->data;
    SpitAndSquealDef* def = g->def;

    switch (msg) {
    case 0x96224FCC:
    case 0x0020EE36:
        if (g->timer >= 0.0f) {
            return 0x3DA260F7;
        }
        if (fn_800F7D28(g->emitter, def->id)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->id)) {
            return 0xC0BECF7E;
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
    case 0x0D19E338:
        return 0x04A3E183;
    case 0x9791AF51:
        return 0x04A3E183;
    case 0x968CBE3D:
    case 0xE6B71BCC:
        return 0x3DA260F7;
    }
    return 0x3DA260F7;
}

int SpitAndSquealMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    SpitAndSqueal* g = (SpitAndSqueal*)obj->data;
    SpitAndSquealDef* def = g->def;

    Fn80175D60(arg);
    switch (msg) {
    case 0x0020EE36: {
        int pos[3];
        ShotInfo info;
        float* p;
        void* fx;

        if (!fn_800F7D28(g->emitter, def->id)) {
            return 0;
        }
        info.hit = (ShotHit*)data;
        info.speed = def->speed;
        info.unk8 = def->unk10;
        Fn800F7F5C(g->emitter, def->id, 0x14967, arg, &info, dt);
        fx = def->fx;
        p = &info.hit->x;
        if (p) {
            pos[0] = p[0];
            pos[1] = p[1];
            pos[2] = p[2];
            fn_8016F118(fn_8016F120(fx, pos), 0, 100000);
        } else {
            FxStart(fx, 0, 0);
        }
        g->timer = 200.0f * info.hit->scale;
        g->arg = arg;
        return 1;
    }
    case 0x315A1A7F:
        g->arg = arg;
        return 1;
    case 0xF829DB93: {
        int pos[3];
        ShotInfo info;
        float* p;
        void* fx;

        if (!fn_800F7D28(g->emitter, def->id)) {
            return 0;
        }
        info.hit = (ShotHit*)data;
        info.speed = def->speed2;
        info.unk8 = def->unk70;
        Fn800F7F5C(g->emitter, def->id2, 0x14967, arg, &info, dt);
        fx = def->fx;
        p = &info.hit->x;
        if (p) {
            pos[0] = p[0];
            pos[1] = p[1];
            pos[2] = p[2];
            fn_8016F118(fn_8016F120(fx, pos), 0, 100000);
        } else {
            FxStart(fx, 0, 0);
        }
        g->timer = 200.0f * info.hit->scale;
        g->arg = arg;
        return 1;
    }
    case 0x0D19E338:
        g->unk00 = 0;
        return 1;
    case 0x968CBE3D:
    case 0xE6B71BCC:
        return 1;
    case 0xBC78F550:
    case 0x8FDD7599:
        return 1;
    case 0x0B59E861:
        return 1;
    case 0x141109BD: {
        short ids[4];
        ids[0] = def->model;
        ids[1] = def->model2;
        ids[2] = def->model3;
        ids[3] = def->model4;
        if ((int)data == 1) {
            ModelLoadIds(ids, 4);
        } else {
            ModelFreeIds(ids, 4);
        }
        return 1;
    }
    }
    return 0;
}

void SpitAndSquealDestroy(EffectObj* obj) {
    SpitAndSqueal* g = (SpitAndSqueal*)obj->data;
    fn_800F86A0(g->emitter);
    VFree(g);
}

void* SpitAndSquealCreate(int a, int id) {
    EffectType* type = &gSpitAndSquealType;
    SpitAndSqueal* g;

    EffectTypeHash(type);
    g = (SpitAndSqueal*)VAlloc(sizeof(SpitAndSqueal));
    memset(g, 0, sizeof(SpitAndSqueal));
    if (g == 0) {
        return 0;
    }
    g->def = (SpitAndSquealDef*)fn_800EB21C(id);
    g->rate = 200.0f;
    g->unk00 = 0;
    g->timer = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->def->id, g->def->max);
    Fn800F840C(g->emitter, g->def->id2, g->def->max2);
    return g;
}

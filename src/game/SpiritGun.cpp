// SpiritGun.cpp: the "SpiritGun" weapon effect type (0x800F0374-0x800F0B10).
// While firing, a timer spawns a projectile through the owner's emitter every `rate` ms; each shot
// starts the definition's effect at the hit point. Messages start/stop firing or fire one shot.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include <string.h>

extern "C" {

// Callees in other files, not named yet.
int fn_800F7D28(int emitter, int id);
void* fn_8016F120(void* fx, int* pos);
void fn_8016F118(void* handle, int a, int b);
void fn_80175D60(int arg);
void fn_800F86A0(int emitter);
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
}

void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
void FxStart(void* fx, int a, int b);
void* VAlloc(int size);
void VFree(void* p);
void Fn800F840C(int emitter, int a, int b);

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    u32 h;
    if (s == 0) {
        h = 0;
    } else {
        int i;
        h = 0;
        for (i = 0; s[i] != 0; i++) {
            h = h * 31 + (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
    }
    return h;
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
        type->hash = StrHash(type->name);
    }
    return type->hash;
}

struct SpiritGunDef {
    int unk0;
    int id;
    int unk8;
    float speed;
    int unk10;
    int unk14;
    int unk18;
    int drain;
    int unk20[2];
    int unk28;
    int unk2C;
    void* fx;
};

struct SpiritGun {
    int firing;
    float timer;
    float rate;
    float charge;
    int emitter;
    SpiritGunDef* def;
    int arg;
    int unk1C;
    u8 shot[0x60];
    int unk80;
};

struct ShotHit {
    u8 pad[0x30];
    float x, y, z;
};

struct ShotInfo {
    ShotHit* hit;
    float speed;
    int unk8;
    int unkC;
    int charge;
    int unk14;
    int unk18;
    int unk1C;
};

void SpiritGunReset(EffectObj* obj);
void SpiritGunUpdate(EffectObj* obj, float dt);
u32 SpiritGunQuery(EffectObj* obj, int msg);
int SpiritGunMsg(EffectObj* obj, int msg, int arg, void* data, float dt);
void SpiritGunDestroy(EffectObj* obj);
void* SpiritGunCreate(int a, int id);

EffectType gSpiritGunType = {
    "SpiritGun", 0, {0, 0, 0}, SpiritGunCreate, SpiritGunDestroy, SpiritGunMsg, SpiritGunQuery, SpiritGunUpdate,
    SpiritGunReset, 0,
};

inline void SpiritGunFire(SpiritGun* s, SpiritGunDef* def, void* hit, int arg, float dt) {
    int pos[3];
    ShotInfo info;
    float* p;
    void* fx;

    info.hit = (ShotHit*)hit;
    info.speed = def->speed;
    info.unkC = def->unk14;
    info.charge = s->charge;
    info.unk18 = def->unk28;
    info.unk1C = def->unk2C;
    Fn800F7F5C(s->emitter, def->id, 0x14967, arg, &info, dt);
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
}

// The timed shot: aims at the stored shot and reads the stored argument.
inline void SpiritGunRefire(SpiritGun* s, SpiritGunDef* def, float dt) {
    int pos[3];
    ShotInfo info;
    float* p;
    void* fx;

    info.hit = (ShotHit*)s->shot;
    info.speed = def->speed;
    info.unkC = def->unk14;
    info.charge = s->charge;
    info.unk18 = def->unk28;
    info.unk1C = def->unk2C;
    Fn800F7F5C(s->emitter, def->id, 0x14967, s->arg, &info, dt);
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
}

void SpiritGunReset(EffectObj* obj) {
    SpiritGun* s = (SpiritGun*)obj->data;
    s->firing = 0;
    s->timer = 0.0f;
}

void SpiritGunUpdate(EffectObj* obj, float dt) {
    SpiritGun* s = (SpiritGun*)obj->data;
    SpiritGunDef* def = s->def;

    if (s->firing == 1) {
        s->timer -= dt;
        while (s->timer <= 0.0f) {
            s->timer += s->rate;
            if (!fn_800F7D28(s->emitter, def->id)) {
                return;
            }
            SpiritGunRefire(s, def, dt);
        }
    }
    if (s->charge > 0.0f) {
        s->charge -= 0.001f * dt * def->drain;
        s->charge = s->charge > 0.0f ? s->charge : 0.0f;
    }
}

u32 SpiritGunQuery(EffectObj* obj, int msg) {
    SpiritGun* s = (SpiritGun*)obj->data;
    SpiritGunDef* def = s->def;

    switch (msg) {
    case 0xF829DB93:
    case 0x96224FCC:
    case 0x0020EE36:
    case 0x42E50A3D:
        if (fn_800F7D28(s->emitter, def->id)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->id)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0x9791AF51:
    case 0x0D19E338:
        if (s->firing == 1) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
    case 0x968CBE3D:
    case 0xE6B71BCC:
        return 0x3DA260F7;
    }
    return 0x3DA260F7;
}

int SpiritGunMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    SpiritGun* s = (SpiritGun*)obj->data;
    SpiritGunDef* def = s->def;

    fn_80175D60(arg);
    s->arg = arg;
    switch (msg) {
    case 0xF829DB93:
    case 0x0020EE36:
        if (s->firing == 1) {
            memcpy(s->shot, data, 0x60);
        } else {
            if (!fn_800F7D28(s->emitter, def->id)) {
                return 0;
            }
            SpiritGunFire(s, def, data, arg, dt);
        }
        return 1;
    case 0x96224FCC:
    case 0x42E50A3D:
        if (s->firing != 1) {
            s->firing = 1;
            s->timer = s->rate;
        }
        memcpy(s->shot, data, 0x60);
        return 1;
    case 0x315A1A7F:
        memcpy(s->shot, data, 0x60);
        return 1;
    case 0x9791AF51:
    case 0x0D19E338:
        s->firing = 0;
        return 1;
    case 0x968CBE3D:
    case 0xE6B71BCC:
        return 1;
    case 0x8FDD7599:
    case 0xBC78F550:
        return 1;
    case 0x0B59E861:
        return 1;
    }
    return 0;
}

void SpiritGunDestroy(EffectObj* obj) {
    SpiritGun* s = (SpiritGun*)obj->data;
    fn_800F86A0(s->emitter);
    VFree(s);
}

void* SpiritGunCreate(int a, int id) {
    SpiritGun* s;

    EffectTypeHash(&gSpiritGunType);
    s = (SpiritGun*)VAlloc(sizeof(SpiritGun));
    memset(s, 0, sizeof(SpiritGun));
    if (s == 0) {
        return 0;
    }
    s->def = (SpiritGunDef*)fn_800EB21C(id);
    s->firing = 0;
    s->rate = 200.0f;
    s->timer = 0.0f;
    s->unk80 = -1;
    s->charge = 0.0f;
    s->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(s->emitter, s->def->id, s->def->unk8);
    return s;
}

// u_801DE4B0.cpp (0x801DE4B0-0x801DE7A0): the "TentacleSpit" effect type (descriptor .data 0x803ADCF8: name
// 0x80348DBF, msg fn_801DE4BC at +0x1C, query fn_801DE4B0 at +0x20), split from the old u_801D9A70 range.
// Boundary evidence: own ghost-action name table + "TentacleSpit" at .rodata 0x80348D78-0x80348DD0; own
// .sdata2 pool 0x804D8D18-0x804D8D28 (360.0f, 4096.0f, int-to-float double 0x804D8D20, a duplicate of the
// creature file's 0x804D8CC8); .sdata 0x804D3248. fn_801DE4B0 returns the effect class hash 0x3DA260F7 like
// the SniperRound query fn_801E72E0.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <string.h>

#include "game/types.h"
#include "game/effect.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" {
void fn_800E1098(void* out, void* in, float length);
void fn_8016D440(Vec3* out, Vec3* in);
void fn_8016D4EC(Vec3* out, Vec3* in);
void fn_80186A88(int owner, void* desc, int arg, int a, int b, float f);
}
unsigned long Fn80175D60(unsigned long arg);
extern "C" int rand(void);

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
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
    }
    return h;
}

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}

// The shooter's placement (only the fields read here).
struct SpitShot {
    u8 pad[0x30];
    Vec3 pos;      // 0x30
    Vec3 dir;      // 0x3C
    int unk48;     // 0x48
    float unk4C;   // 0x4C
    float spread;  // 0x50
};

struct SpitFire {
    SpitShot* shot;  // 0x00
    float speed;     // 0x04
};

struct SpitDesc {
    Vec3 pos;          // 0x00
    Vec3 up;           // 0x0C
    Vec3 vel;          // 0x18
    u8 pad24[0x28];    // 0x24
};

// Number of globs per shot.
static int sSpitCount = 4;

extern "C" unsigned long fn_801DE4B0(EffectObj* obj, int msg);
extern "C" int fn_801DE4BC(EffectObj* obj, int msg, int arg, void* data, float dt);

EffectType gTentacleSpitType = {
    "TentacleSpit", 0, {0, 0, 0}, 0, 0, fn_801DE4BC, fn_801DE4B0, 0, 0, 0,
};

//@@801DE4B0
// Effect class query (descriptor +0x20): the same class hash as the SniperRound query fn_801E72E0.
unsigned long fn_801DE4B0(EffectObj* obj, int msg) {
    return 0x3DA260F7;
}
//@@end

//@@801DE4BC
// Effect message (descriptor +0x1C): on fire, spawns sSpitCount globs along the shot direction,
// each turned by a random angle around z and x.
int fn_801DE4BC(EffectObj* obj, int msg, int arg, void* data, float dt) {
    Fn80175D60(arg);

    switch (msg) {
    case 0x14967: {
        int n;
        int i;
        SpitShot* shot;
        int owner;
        Vec3* vel;
        EffectType* type;
        SpitDesc desc;
        Vec3 v;
        Vec3 up;

        n = 10;
        shot = ((SpitFire*)data)->shot;
        type = &gTentacleSpitType;
        n += shot->spread;
        owner = obj->unk0;
        EffectTypeHash(type);
        memset(&desc, 0, sizeof(desc));
        desc.pos = shot->pos;
        fn_8016D440(&up, &shot->dir);
        desc.up = up;
        vel = &desc.vel;
        for (i = 0; i < sSpitCount; i++) {
            float a2;
            float a1;
            fn_8016D440(&v, &shot->dir);
            a1 = 360.0f * (float)(rand() % n) / 4096.0f;
            a2 = 360.0f * (float)(rand() % n) / 4096.0f;
            if (rand() % 2) {
                v.z += a1;
            } else {
                v.z -= a1;
            }
            if (rand() % 2) {
                v.x += a2;
            } else {
                v.x -= a2;
            }
            fn_8016D4EC(vel, &v);
            fn_800E1098(vel, vel, ((SpitFire*)data)->speed);
            fn_80186A88(owner, &desc, arg, shot->unk48, 0, shot->unk4C);
        }
        return 1;
    }
    }
    return 0;
}
//@@end

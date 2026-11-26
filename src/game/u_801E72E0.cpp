// u_801E72E0.cpp (0x801E72E0-0x801E7534): the "SniperRound" effect type (descriptor 0x803B3B30: query
// 0x801E72E0, msg 0x801E72EC), split from the old u_801E241C range.
// Boundary evidence: .rodata pool 0x80348F68-0x80348FBB is its own ghost-action name table followed by
// "SniperRound" (the Sniper file has its own table at 0x80348F18, the sniper host file u_801E7534 its own at
// 0x80348FC0); .sdata2 0x804D8EF0-0x804D8EF8; .data 0x803B3B30-0x803B3B60 (the host table 0x803B3B60 that
// follows belongs to u_801E7534, whose first state fn_801E7534 ends this range).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include <string.h>
#include "game/types.h"
#include "game/msg.h"
#include "game/effect.h"

extern "C" {
void fn_800E1098(void* out, void* in, float length);
void fn_8016D440(Vec3* out, Vec3* in);
void fn_800C5E00(int a, int b, int c, int d, int e);
void fn_800E1BD8(int a, int b, int c, int d, int e);
void fn_800B4C50(float* pos, int id, int a, int b, float f1, float f2);
void fn_80186A88(int owner, void* desc, int arg, int a, int b, float f);
int SendMsg(int to, int msg, void* data);
}
void* PlayerGetGunVec(void* owner);
unsigned long Fn80175D60(unsigned long arg);

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
struct RoundShot {
    u8 pad[0x30];
    Vec3 pos;      // 0x30
    Vec3 dir;      // 0x3C
    int unk48;     // 0x48
    float unk4C;   // 0x4C
};

struct RoundFire {
    RoundShot* shot;  // 0x00
    float speed;      // 0x04
    int unk8;         // 0x08
    int animA;        // 0x0C
    int animB;        // 0x10
};

struct RoundDesc {
    Vec3 pos;          // 0x00
    Vec3 up;           // 0x0C
    Vec3 vel;          // 0x18
    u8 pad24[0x28];    // 0x24
};

// The shooting player (only the fields read here).
struct RoundTarget {
    u8 pad[0x248];
    int model;                     // 0x248
    u8 pad24C[0x58C - 0x24C];
    int rumble;                    // 0x58C
    u8 pad590[0x1C6C - 0x590];
    int hasAnim;                   // 0x1C6C
    int animA;                     // 0x1C70
    int animB;                     // 0x1C74
};

extern "C" unsigned long fn_801E72E0(EffectObj* obj, int msg);
extern "C" int fn_801E72EC(EffectObj* obj, int msg, int arg, void* data, float dt);

EffectType gSniperRoundType = {
    "SniperRound", 0, {0, 0, 0}, 0, 0, fn_801E72EC, fn_801E72E0, 0, 0, 0,
};

//@@801E72E0
unsigned long fn_801E72E0(EffectObj* obj, int msg) {
    return 0x3DA260F7;
}
//@@end

//@@801E72EC
// Effect message (descriptor +0x1C): on fire, spawns the round along the shot direction; when the
// shooter is free it adds the gun's velocity, plays the recoil animation and rumble.
int fn_801E72EC(EffectObj* obj, int msg, int arg, void* data, float dt) {
    int busy = Fn80175D60(arg);

    switch (msg) {
    case 0x14967: {
        RoundShot* shot;
        int owner;
        EffectType* type;
        RoundDesc desc;
        Msg400 q;
        Vec3 up;

        owner = obj->unk0;
        type = &gSniperRoundType;
        shot = ((RoundFire*)data)->shot;
        EffectTypeHash(type);
        memset(&desc, 0, sizeof(desc));
        fn_800E1098(&desc.vel, &shot->dir, ((RoundFire*)data)->speed);
        desc.pos = shot->pos;
        fn_8016D440(&up, &shot->dir);
        desc.up = up;
        if (busy == 0) {
            RoundTarget* t;
            Vec3* v;
            SendMsg(arg, 0x400, &q);
            t = (RoundTarget*)q.actor;
            v = (Vec3*)PlayerGetGunVec(t);
            {
                float x = v->x;
                float y = v->y;
                float z = v->z;
                x *= 1000.0f;
                y *= 1000.0f;
                z *= 1000.0f;
                desc.vel.x += x;
                desc.vel.y += y;
                desc.vel.z += z;
            }
            if (t->hasAnim == 0) {
                fn_800C5E00(t->model, 1, ((RoundFire*)data)->animA, ((RoundFire*)data)->animB, 0);
            } else {
                fn_800C5E00(t->model, 1, t->animA, t->animB, 0);
            }
            fn_800E1BD8(t->rumble, 1, 0x50, 1000, 1000);
            if (q.flags & 2) {
                fn_800B4C50(&shot->pos.x, arg, 1, 0, 1000.0f, 50000.0f);
            }
        }
        fn_80186A88(owner, &desc, arg, shot->unk48, 0, shot->unk4C);
        return 1;
    }
    }
    return 0;
}
//@@end

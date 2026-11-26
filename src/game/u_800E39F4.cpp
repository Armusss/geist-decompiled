// u_800E39F4.cpp (0x800E39F4-0x800E3EC4): the "Bullet" effect type, split from the old u_800E1B5C range.
// Boundary evidence: descriptor 0x8039C3F8 (update 0x800E39F4, query 0x800E39F8, msg 0x800E3A04; fn_800E3A8C
// reads it); .rodata pool 0x80344EA8-0x80344EF8 (ghost-action table + "Bullet"); .sdata2 0x804D71C0-0x804D71D8
// (own int-to-float double 0x804D71D0 next to AUG's 0x804D71A8).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include <string.h>
#include "game/types.h"
#include "game/msg.h"
#include "game/effect.h"

int Fn80175D60(unsigned long arg);

extern "C" {
void fn_800E1098(void* out, void* in, float length);
void fn_8016D440(Vec3* out, Vec3* in);
void fn_8016D4EC(Vec3* out, Vec3* in);
void fn_8016D648(Vec3* out, Vec3* a, Vec3* b);
float fn_800A127C(float);
float fn_800A1418(float);
float fn_800A1394(float);
int fn_80212730(u32 handle);
int fn_800F8AF4(int id);
void fn_800C5E00(int a, int b, int c, int d, int e);
void fn_800E1BD8(int a, int b, int c, int d, int e);
void fn_800B4C50(float* pos, int id, int a, int b, float f1, float f2);
void fn_80186A88(int owner, void* desc, int arg, int a, int b, float f);
int SendMsg(int to, int msg, void* data);
int rand(void);
}
void* PlayerGetGunVec(void* owner);

// The owner record (only the velocity read here).
struct OwnerInfo {
    u8 pad[0x240];
    Vec3 vel;  // 0x240
};
OwnerInfo* OwnerGetInfo(u32 owner);

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
struct BulletShot {
    u8 pad[0x30];
    Vec3 pos;      // 0x30
    Vec3 dir;      // 0x3C
    int unk48;     // 0x48
    float unk4C;   // 0x4C
    float spread;  // 0x50
};

struct BulletFire {
    BulletShot* shot;  // 0x00
    float speed;       // 0x04
    int unk8;          // 0x08
    int animA;         // 0x0C
    int animB;         // 0x10
};

struct BulletDesc {
    Vec3 pos;          // 0x00
    Vec3 up;           // 0x0C
    Vec3 vel;          // 0x18
    u8 pad24[0x28];    // 0x24
};

// The shooting player (only the fields read here).
struct BulletTarget {
    u8 pad[0x38];
    u32 handle;                    // 0x38
    u8 pad3C[0x248 - 0x3C];
    int model;                     // 0x248
    u8 pad24C[0x58C - 0x24C];
    int rumble;                    // 0x58C
    u8 pad590[0x1C6C - 0x590];
    int hasAnim;                   // 0x1C6C
    int animA;                     // 0x1C70
    int animB;                     // 0x1C74
};

inline void Vec3Set(Vec3* v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

extern "C" void fn_800E39F4(EffectObj* obj, float dt);
extern "C" unsigned long fn_800E39F8(EffectObj* obj, int msg);
extern "C" int fn_800E3A04(EffectObj* obj, int msg, int arg, void* data, float dt);

EffectType gBulletType = {
    "Bullet", 0, {0, 0, 0}, 0, 0, fn_800E3A04, fn_800E39F8, fn_800E39F4, 0, 0,
};

//@@800E39F4
void fn_800E39F4(EffectObj* obj, float dt) {}
//@@end
//@@800E39F8
unsigned long fn_800E39F8(EffectObj* obj, int msg) {
    return 0x3DA260F7;
}
//@@end
//@@800E3A04
extern "C" void fn_800E3A8C(int arg, int busy, void* data, int owner);
int fn_800E3A04(EffectObj* obj, int msg, int arg, void* data, float dt) {
    int busy = Fn80175D60(arg);

    switch (msg) {
    case 0x14967:
        fn_800E3A8C(arg, busy, data, obj->unk0);
        return 1;
    }
    return 0;
}
//@@end
//@@800E3A8C
// Fires one bullet: random spread cone around the shot direction; when the shooter is free it adds
// the gun's (and a riding owner's) velocity, plays the recoil animation and a per-weapon rumble.
void fn_800E3A8C(int arg, int busy, void* data, int owner) {
    BulletShot* shot = ((BulletFire*)data)->shot;
    EffectType* type = &gBulletType;
    float s;
    float r;
    float ang;
    float spread;
    BulletDesc desc;
    Msg400 q;
    Vec3 up;
    Vec3 v;
    Vec3 dir;
    Vec3 rot;

    EffectTypeHash(type);
    memset(&desc, 0, sizeof(desc));
    fn_800E1098(&desc.vel, &shot->dir, ((BulletFire*)data)->speed);
    desc.pos = shot->pos;
    fn_8016D440(&up, &shot->dir);
    desc.up = up;
    spread = shot->spread;
    r = (float)rand() / 32767.0f;
    ang = (float)(rand() % 360);
    s = fn_800A127C(spread * r);
    Vec3Set(&rot, s * fn_800A1394(ang), 1.0f, s * fn_800A1418(ang));
    fn_8016D440(&dir, &rot);
    fn_8016D440(&v, &shot->dir);
    fn_8016D648(&v, &v, &dir);
    fn_8016D4EC(&desc.vel, &v);
    fn_800E1098(&desc.vel, &desc.vel, ((BulletFire*)data)->speed);
    if (busy == 0) {
        BulletTarget* t;
        Vec3* g;
        u32 h;
        SendMsg(arg, 0x400, &q);
        t = (BulletTarget*)q.actor;
        g = (Vec3*)PlayerGetGunVec(t);
        {
            float x = g->x;
            float y = g->y;
            float z = g->z;
            x *= 1000.0f;
            y *= 1000.0f;
            z *= 1000.0f;
            desc.vel.x += x;
            desc.vel.y += y;
            desc.vel.z += z;
        }
        h = fn_80212730(t->handle);
        if ((int)h != -1 && (int)(h >> 24) == 0x13) {
            OwnerInfo* info = OwnerGetInfo(h);
            if (info != 0) {
                desc.vel.x += info->vel.x;
                desc.vel.y += info->vel.y;
                desc.vel.z += info->vel.z;
            }
        }
        if (t->hasAnim == 0) {
            fn_800C5E00(t->model, 1, ((BulletFire*)data)->animA, ((BulletFire*)data)->animB, 0);
        } else {
            fn_800C5E00(t->model, 1, t->animA, t->animB, 0);
        }
        switch (fn_800F8AF4(owner)) {
        case 0x6BA7CC3B:
            fn_800E1BD8(t->rumble, 1, 0xFA, 1000, 1000);
            break;
        case (int)0x8FA99EF5:
        case 0x7C179063:
            fn_800E1BD8(t->rumble, 1, 0x96, 1000, 1000);
            break;
        case 0x06C781AA:
            fn_800E1BD8(t->rumble, 1, t->animB, 1000, 1000);
            break;
        default:
            fn_800E1BD8(t->rumble, 1, 0x50, 1000, 1000);
            break;
        }
        if (q.flags & 2) {
            fn_800B4C50(&shot->pos.x, arg, 1, 0, 1000.0f, 10000.0f);
        }
    }
    fn_80186A88(owner, &desc, arg, shot->unk48, 0, shot->unk4C);
}
//@@end

// Homing.cpp: the "Homing" weapon effect type (0x800F0B10-0x800F0D24).
// Fires a homing projectile along the shot direction; when the shooter is free it adds the target's
// velocity and plays the target's hit animation.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include <string.h>

extern "C" {

// Callees in other files, not named yet.
int fn_80175D60(int arg);
void fn_800E1098(void* out, void* in, float length);
void fn_8016D440(void* out, void* in);
void* fn_800772E0(void* actor);
void fn_800C5E00(int a, int b, int c, int d, int e);
void fn_80186A88(int owner, void* info, int arg, int a, int b, float f);
}

int SendMsg(int to, int msg, void* data);

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

struct Vec3 {
    float x, y, z;
};

inline Vec3 operator*(const Vec3& a, float s) {
    Vec3 r;
    r.x = a.x * s;
    r.y = a.y * s;
    r.z = a.z * s;
    return r;
}

struct HomingShot {
    u8 pad[0x30];
    Vec3 pos;
    Vec3 dir;
    int unk48;
    float unk4C;
};

struct HomingFire {
    HomingShot* shot;
    float speed;
    int unk8;
    int animA;
    int animB;
};

struct HomingInfo {
    Vec3 pos;
    Vec3 up;
    Vec3 vel;
    u8 pad24[0x28];
};

struct HomingTarget {
    u8 pad[0x248];
    int model;
    u8 pad24C[0x1C6C - 0x24C];
    int hasAnim;
    int animA;
    int animB;
};

u32 HomingQuery(EffectObj* obj, int msg);
int HomingMsg(EffectObj* obj, int msg, int arg, void* data, float dt);

EffectType gHomingType = {
    "Homing", 0, {0, 0, 0}, 0, 0, HomingMsg, HomingQuery, 0, 0, 0,
};

u32 HomingQuery(EffectObj* obj, int msg) { return 0x3DA260F7; }

int HomingMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    int busy = fn_80175D60(arg);

    switch (msg) {
    case 0x14967: {
        int owner = obj->unk0;
        HomingFire* fire = (HomingFire*)data;
        HomingShot* shot;
        HomingInfo info;
        int target;
        Vec3 up;

        EffectTypeHash(&gHomingType);
        shot = fire->shot;
        memset(&info, 0, sizeof(HomingInfo));
        fn_800E1098(&info.vel, &shot->dir, fire->speed);
        info.pos = shot->pos;
        fn_8016D440(&up, &shot->dir);
        info.up = up;
        if (busy == 0) {
            HomingTarget* t;
            Vec3* v;
            SendMsg(arg, 0x400, &target);
            t = (HomingTarget*)target;
            v = (Vec3*)fn_800772E0(t);
            {
                Vec3 d = *v * 1000.0f;
                info.vel.x += d.x;
                info.vel.y += d.y;
                info.vel.z += d.z;
            }
            if (t->hasAnim == 0) {
                fn_800C5E00(t->model, 1, fire->animA, fire->animB, 0);
            } else {
                fn_800C5E00(t->model, 1, t->animA, t->animB, 0);
            }
        }
        fn_80186A88(owner, &info, arg, shot->unk48, 0, shot->unk4C);
        return 1;
    }
    }
    return 0;
}

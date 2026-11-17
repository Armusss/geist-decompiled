// u_801C9CE0.cpp: 0x801C9CE0-0x801CA16C, 3 functions. The "AshBullet" effect type.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   string pool 0x803487C0 (POSSESS..SCAN, "AshBullet")
//   type descriptor 0x803ACB08: msg AshBulletMsg (+0x1C), query AshBulletQuery (+0x20), so the file starts
//   at the query callback (same layout and code shape as Homing.cpp)
//   .sdata2 0x804D8AF0-0x804D8AFC; .bss 0x80496D58 filled by the asset preload fn_801C9F50 (the same
//   end-of-file preload shape as fn_801D09FC in u_801CDFBC; called from PlayerInitGunState)
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

#include <string.h>
#include "game/types.h"
#include "game/msg.h"
#include "game/effect.h"

extern "C" {
void fn_800E1098(void* out, void* in, float length);
void fn_8016D440(void* out, void* in);
void fn_800C5E00(int a, int b, int c, int d, int e);
void* fn_80186A88(int owner, void* info, int arg, int a, int b, float f);
void fn_80186F08(void* shot, void* out);
void fn_800B4C50(float* pos, int arg, int a, int b, float f1, float f2);
void fn_8015B428(void* actor, void* pos, int r1, int g1, int b1, int r2, int g2, int b2, float t);
int SendMsg(int to, int msg, void* data);
}
void* PlayerGetGunVec(void* owner);
unsigned long Fn80175D60(unsigned long arg);
int AssetFind(u32 bank, u32 name);

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

struct AshShot {
    u8 pad[0x30];
    Vec3 pos;                                  // 0x30
    Vec3 dir;                                  // 0x3C
    int unk48;                                 // 0x48
    float unk4C;                               // 0x4C
};

struct AshFire {
    AshShot* shot;                             // 0x00
    float speed;                               // 0x04
    int unk8;                                  // 0x08
    int animA;                                 // 0x0C
    int animB;                                 // 0x10
};

struct AshInfo {
    Vec3 pos;                                  // 0x00
    Vec3 up;                                   // 0x0C
    Vec3 vel;                                  // 0x18
    u8 pad24[0x28];
};

struct AshTarget {
    u8 pad[0x248];
    int model;                                 // 0x248
    u8 pad24C[0x1C6C - 0x24C];
    int hasAnim;                               // 0x1C6C
    int animA;                                 // 0x1C70
    int animB;                                 // 0x1C74
};

struct AshHit {
    u8 pad[0x24];
    IVec3 pos;                                 // 0x24
};

u32 AshBulletQuery(EffectObj* obj, int msg);
int AshBulletMsg(EffectObj* obj, int msg, int arg, void* data, float dt);

EffectType gAshBulletType = {
    "AshBullet", 0, {0, 0, 0}, 0, 0, AshBulletMsg, AshBulletQuery, 0, 0, 0,
};

int lbl_80496D58[16];  // assets preloaded by AshBulletPreload

u32 AshBulletQuery(EffectObj* obj, int msg) { return 0x3DA260F7; }

int AshBulletMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    int busy = Fn80175D60(arg);

    switch (msg) {
    case 0x14967: {
        int owner = obj->unk0;
        EffectType* type = &gAshBulletType;
        AshFire* fire = (AshFire*)data;
        AshShot* shot;
        AshInfo info;
        Msg400 target;
        MsgResult hit;
        Vec3 up;

        EffectTypeHash(type);
        shot = fire->shot;
        memset(&info, 0, sizeof(AshInfo));
        fn_800E1098(&info.vel, &shot->dir, fire->speed);
        info.pos = shot->pos;
        fn_8016D440(&up, &shot->dir);
        info.up = up;
        if (busy == 0) {
            AshTarget* t;
            Vec3* v;
            SendMsg(arg, 0x400, &target);
            t = (AshTarget*)target.actor;
            v = (Vec3*)PlayerGetGunVec(t);
            {
                float x = v->x;
                float y = v->y;
                float z = v->z;
                x *= 25.0f;
                y *= 25.0f;
                z *= 25.0f;
                info.vel.x += x;
                info.vel.y += y;
                info.vel.z += z;
            }
            if (t->hasAnim == 0) {
                fn_800C5E00(t->model, 1, fire->animA, fire->animB, 0);
            } else {
                fn_800C5E00(t->model, 1, t->animA, t->animB, 0);
            }
            if (target.flags & 2) {
                fn_800B4C50(&shot->pos.x, arg, 1, 0, 1000.0f, 32000.0f);
            }
        }
        fn_80186F08(fn_80186A88(owner, &info, arg, shot->unk48, 0, shot->unk4C), &hit);
        fn_8015B428(shot, &((AshHit*)hit.p)->pos, 0x77, 0x39, 0xF0, 0x77, 0x39, 0xF0, 0.0f);
        return 1;
    }
    }
    return 0;
}

// Looks up the assets the ash bullets use (called from PlayerInitGunState).
extern "C" void fn_801C9F50() {
    lbl_80496D58[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_80496D58[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_80496D58[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_80496D58[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_80496D58[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_80496D58[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_80496D58[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_80496D58[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_80496D58[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_80496D58[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_80496D58[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_80496D58[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_80496D58[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_80496D58[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_80496D58[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_80496D58[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

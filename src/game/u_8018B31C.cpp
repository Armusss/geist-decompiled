// u_8018B31C.cpp (0x8018B31C-0x8018C2F8), split from u_8017D794.
// "SplineBullet" effect type: descriptor 0x803A8F08 (name at 0x80347057 after its own action-name table
// 0x80347010) lists fn_8018B31C and fn_8018B328; fn_8018B3B0 is its create function. sdata2 0x804D8440-0x804D84C8.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include <string.h>
#include "game/types.h"
#include "game/msg.h"
#include "game/effect.h"

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

// The spline shot being released (only the fields used here).
struct SplineShotSrc {
    u8 pad00[0x30];
    Vec3 pos;             // 0x30
    Vec3 dir;             // 0x3C
};

// Arguments of message 0x14967.
struct SplineFire {
    SplineShotSrc* shot;  // 0x00
    float speed;          // 0x04
    u8 pad08[0x8];
    int charged;          // 0x10
    int arg;              // 0x14
};

// Launch block (0x4C bytes), cleared and filled here.
struct SplineLaunch {
    Vec3 pos;             // 0x00
    Vec3 rot;             // 0x0C
    Vec3 vel;             // 0x18
    u8 pad24[0x28];
};

// A world object the bullet can home on (only the fields used here).
struct HomeTarget {
    u8 pad00[0x24];
    int pos[3];           // 0x24
    int (*handler)(int msg, void* data);  // 0x30
    u8 pad34[0x4];
    u32 handle;           // 0x38
    u8 pad3C[0x4C];
    int* kind;            // 0x88
};

// The player character that shoots (mode 0).
struct Shooter {
    u8 pad00[0x67C];
    Vec3 pos;             // 0x67C
    u8 pad688[0x167C];
    u8 aim[0x30];         // 0x1D04
    Vec3 aimBase;         // 0x1D34
    Vec3 aimDir;          // 0x1D40
    u8 pad1D4C[0x18];
    HomeTarget* target;   // 0x1D64
};

// The host that shoots (mode 1).
struct ShooterHost {
    u8 pad00[0x38];
    u32 handle;           // 0x38
    u8 pad3C[0x48];
    Vec3 pos;             // 0x84
    u8 pad90[0x1B0];
    Vec3 gunVel;          // 0x240
    u8 pad24C[0x24];
    u8 follow[0x4];       // 0x270
};

struct Msg400Flags {
    void* actor;          // 0x00
    int unk04[5];         // 0x04
    int flags;            // 0x18
    int unk1C;            // 0x1C
};

extern EffectType lbl_803A8F08;

extern "C" {
int SendMsg(int target, int msg, void* data);
int rand();
void fn_800E1098(Vec3* out, Vec3* in, float length);
void fn_8016D440(Vec3* out, Vec3* in);
void* fn_800C6448(void);
}
int Fn80175D60(unsigned long arg);
Vec3* PlayerGetGunVec(void* owner);
void MtxSetEuler(Mtx34* m, Vec3* rot);
void fn_800A0168(Mtx34* out, Mtx34* in);
void fn_8009FC6C(Mtx34* out, Mtx34* a, Mtx34* b);
void fn_800A0030(Vec3* out, Mtx34* m, Vec3* in);
void fn_800A025C(Vec3* out, void* m, int row);
float fn_800E1020(Vec3* v);
int fn_800498D0(void* a);
void fn_80048BA8(void* a, void* buf, int mode);
void* FollowGetTargetObj(void* range);
int GhostShotFire(float a, float b, float c, int d, Vec3* p5, Vec3* p6, Vec3* p7, Vec3* p8, Vec4* p9, Vec4* p10,
                  int k, int l, int m, int n, Vec3* opt);
int GhostShotFire2(float a, float b, float c, int d, Vec3* p5, Vec3* p6, int state, Vec3* p8, Vec4* p9,
                   Vec4* p10, int k, int l, int m, int n, Vec3* opt);

extern "C" {

inline void VecScale(Vec3* out, Vec3* v, float k) {
    out->x = v->x * k;
    out->y = v->y * k;
    out->z = v->z * k;
}

inline float Min(float a, float b) {
    return a < b ? a : b;
}

inline void VecSet(Vec3* v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

inline void VecAdd(Vec3* out, Vec3* a, Vec3* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

unsigned long fn_8018B31C(EffectObj* obj, int msg) {
    return 0x3DA260F7;
}

void fn_8018B3B0(int target, int mode, SplineFire* fire, int unused);

int fn_8018B328(EffectObj* obj, int msg, int arg, void* data, float dt) {
    int r = Fn80175D60(arg);

    switch (msg) {
    case 0x14967:
        fn_8018B3B0(arg, r, (SplineFire*)data, obj->unk0);
        return 1;
    }
    return 0;
}

void fn_8018B3B0(int target, int mode, SplineFire* fire, int unused) {
    EffectType* type = &lbl_803A8F08;
    SplineShotSrc* shot;
    SplineLaunch launch;
    Vec3 rot;
    float dist;
    float off;
    int charged;
    int life;
    Vec3 t;

    EffectTypeHash(type);
    shot = fire->shot;
    memset(&launch, 0, 0x4C);
    fn_800E1098(&launch.vel, &shot->dir, fire->speed);
    launch.pos = shot->pos;
    fn_8016D440(&rot, &shot->dir);
    launch.rot = rot;
    if (mode == 0) {
        MsgResult info;
        Msg400Flags tinfo;
        Mtx34 m1;
        Mtx34 m2;
        Vec3 ang;
        Vec3 d;
        Vec3 up;
        Vec3 side;
        Vec4 c1;
        Vec4 c2;
        Vec3 p;
        Shooter* owner;
        void* mtx;
        HomeTarget* tgt;

        SendMsg(target, 0x400, &info);
        owner = (Shooter*)info.p;
        VecScale(&t, PlayerGetGunVec(owner), 1000.0f);
        VecAdd(&launch.vel, &launch.vel, &t);
        mtx = fn_800C6448();
        c1.x = 0.0f;
        c1.y = 0.05025f;
        c1.z = 0.134f;
        c1.w = 0.469f;
        c2.x = 0.0f;
        c2.y = 0.134f;
        c2.z = 0.11725f;
        c2.w = 0.0f;
        ang.z = 0.0f;
        ang.x = 0.0f;
        ang.y = 5.0f * (8.0f * (rand() / 32767.0f)) - 20.0f;
        if (rand() / 32767.0f < 0.5f) {
            ang.y += 180.0f;
        }
        tgt = owner->target;
        if (tgt != 0) {
            if (SendMsg(tgt->handle, 0x400, &tinfo) == 0) {
                tgt = 0;
            } else if (!(tinfo.flags & 2) && (int)(tgt->handle >> 24) != 1) {
                tgt = 0;
            } else if ((int)(tgt->handle >> 24) == 1 && (tgt->kind == 0 || *tgt->kind == 0x201BAE)) {
                tgt = 0;
            }
        }
        if (tgt != 0) {
            if (fire->charged == 0) {
                MtxSetEuler(&m1, &ang);
                fn_800A0168(&m2, (Mtx34*)mtx);
                VecSet(&up, 1.0f, 10.0f * (rand() / 32767.0f - 0.5f), 0.0f);
                fn_8009FC6C(&m2, &m2, &m1);
                fn_800A0030(&up, &m2, &up);
            } else {
                MtxSetEuler(&m1, &ang);
                fn_800A0168(&m2, (Mtx34*)mtx);
                up.x = 0.0f;
                up.y = 1.0f;
                up.z = 0.0f;
                fn_800A0030(&up, &m2, &up);
            }
            d.x = tgt->pos[0];
            d.y = tgt->pos[1];
            d.z = tgt->pos[2];
            d.x -= owner->pos.x;
            d.y -= owner->pos.y;
            d.z -= owner->pos.z;
            dist = fn_800E1020(&d);
            fn_800A025C(&d, mtx, 1);
            d.x = -d.x;
            d.y = -d.y;
            d.z = -d.z;
            if (dist > 5000.0f) {
                off = 4000.0f * (rand() / 32767.0f);
                if (fire->charged != 0) {
                    off = 0.0f;
                } else {
                    off *= 0.5f;
                }
                if (fire->charged != 0) {
                    fn_800E1098(&side, &up, -off);
                } else {
                    fn_800E1098(&side, &up, 2.0f * -off);
                }
                if (fire->charged != 0) {
                    fn_800E1098(&up, &up, off);
                } else {
                    fn_800E1098(&up, &up, 0.5f * off);
                }
                charged = fire->charged;
                life = 0x177;
                if (charged != 0) {
                    life = 0x96;
                }
                dist = Min(5.0f * dist, 125000.0f);
                GhostShotFire2(dist, 133.0f, 2000.0f, fn_800498D0(owner), &fire->shot->pos, &up, tgt->handle, &side,
                               &c1, &c2, 0xCF0262AA, life, charged, fire->arg, &d);
            } else {
                off = 4000.0f * (rand() / 32767.0f);
                if (fire->charged != 0) {
                    off = 0.0f;
                } else {
                    off *= 0.5f;
                }
                if (fire->charged != 0) {
                    fn_800E1098(&side, &up, -off);
                } else {
                    fn_800E1098(&side, &up, 0.5f * -off);
                }
                if (fire->charged != 0) {
                    fn_800E1098(&up, &up, off);
                } else {
                    fn_800E1098(&up, &up, 0.0f);
                }
                charged = fire->charged;
                life = 0x177;
                if (charged != 0) {
                    life = 0x96;
                }
                GhostShotFire2(7.0f * dist, 10.0f + 120.0f * dist / 5000.0f, 2000.0f, fn_800498D0(owner),
                               &fire->shot->pos, &up, tgt->handle, &side, &c1, &c2, 0xCF0262AA, life, charged,
                               fire->arg, &d);
            }
        } else {
            fn_80048BA8(owner, owner->aim, 0);
            if (fire->charged == 0) {
                MtxSetEuler(&m1, &ang);
                fn_800A0168(&m2, (Mtx34*)mtx);
                VecSet(&up, 1.0f, 10.0f * (rand() / 32767.0f - 0.5f), 0.0f);
                fn_8009FC6C(&m2, &m2, &m1);
                fn_800A0030(&up, &m2, &up);
            } else {
                MtxSetEuler(&m1, &ang);
                fn_800A0168(&m2, (Mtx34*)mtx);
                up.x = 0.0f;
                up.y = 1.0f;
                up.z = 0.0f;
                fn_800A0030(&up, &m2, &up);
            }
            d = owner->aimDir;
            p.x = owner->aimBase.x + d.x;
            p.y = owner->aimBase.y + d.y;
            p.z = owner->aimBase.z + d.z;
            dist = fn_800E1020(&d);
            fn_800A025C(&d, mtx, 1);
            d.x = -d.x;
            d.y = -d.y;
            d.z = -d.z;
            if (dist > 5000.0f) {
                off = 4000.0f * (rand() / 32767.0f);
                if (fire->charged != 0) {
                    off = 0.0f;
                } else {
                    off *= 0.5f;
                }
                if (fire->charged != 0) {
                    fn_800E1098(&side, &up, -off);
                } else {
                    fn_800E1098(&side, &up, 2.0f * -off);
                }
                if (fire->charged != 0) {
                    fn_800E1098(&up, &up, 0.5f * off);
                } else {
                    fn_800E1098(&up, &up, 0.0f);
                }
                int charged = fire->charged;
                int life = 0x177;
                if (charged != 0) {
                    life = 0x96;
                }
                dist = Min(5.0f * dist, 35000.0f);
                GhostShotFire(dist, 133.0f, 2000.0f, fn_800498D0(owner), &fire->shot->pos, &up, &p, &side, &c1, &c2,
                              0xCF0262AA, life, charged, fire->arg, &d);
            } else {
                off = 4000.0f * (rand() / 32767.0f);
                if (fire->charged != 0) {
                    off = 0.0f;
                } else {
                    off *= 0.5f;
                }
                if (fire->charged != 0) {
                    fn_800E1098(&side, &up, -off);
                } else {
                    fn_800E1098(&side, &up, 0.5f * -off);
                }
                if (fire->charged != 0) {
                    fn_800E1098(&up, &up, 0.5f * off);
                } else {
                    fn_800E1098(&up, &up, 0.0f);
                }
                int charged = fire->charged;
                int life = 0x177;
                if (charged != 0) {
                    life = 0x96;
                }
                GhostShotFire(2.0f * dist, 10.0f + 120.0f * dist / 5000.0f, 2000.0f, fn_800498D0(owner),
                              &fire->shot->pos, &up, &p, &side, &c1, &c2, 0xCF0262AA, life, charged, fire->arg, &d);
            }
        }
    } else if (mode == 1) {
        MsgResult info;
        Msg400Flags tinfo;
        Mtx34 m1;
        Mtx34 m2;
        Vec3 ang;
        Vec3 d;
        Vec3 up;
        Vec3 side;
        Vec4 c1;
        Vec4 c2;
        ShooterHost* host;
        void* mtx;
        HomeTarget* tgt;

        SendMsg(target, 0x400, &info);
        host = (ShooterHost*)info.p;
        VecScale(&t, &host->gunVel, 1000.0f);
        VecAdd(&launch.vel, &launch.vel, &t);
        mtx = fn_800C6448();
        c1.x = 0.25f;
        c1.y = 0.1f;
        c1.z = 0.0f;
        c1.w = 0.7f;
        c2.x = 0.25f;
        c2.y = 0.2f;
        c2.z = 0.0f;
        c2.w = 0.0f;
        ang.z = 0.0f;
        ang.x = 0.0f;
        ang.y = 5.0f * (8.0f * (rand() / 32767.0f)) - 20.0f;
        if (rand() / 32767.0f < 0.5f) {
            ang.y += 180.0f;
        }
        tgt = (HomeTarget*)FollowGetTargetObj(host->follow);
        if (tgt != 0) {
            tinfo.actor = tgt;
            tgt->handler(0x400, &tinfo);
        }
        if (tgt != 0 && (tinfo.flags & 2)) {
            if (fire->charged == 0) {
                MtxSetEuler(&m1, &ang);
                fn_800A0168(&m2, (Mtx34*)mtx);
                VecSet(&up, 1.0f, 0.0f, 0.0f);
                fn_8009FC6C(&m2, &m2, &m1);
                fn_800A0030(&up, &m2, &up);
            } else {
                MtxSetEuler(&m1, &ang);
                fn_800A0168(&m2, (Mtx34*)mtx);
                up.x = 0.0f;
                up.y = 1.0f;
                up.z = 0.0f;
                fn_800A0030(&up, &m2, &up);
            }
            d.x = tgt->pos[0];
            d.y = tgt->pos[1];
            d.z = tgt->pos[2];
            d.x -= host->pos.x;
            d.y -= host->pos.y;
            d.z -= host->pos.z;
            dist = fn_800E1020(&d);
            fn_800A025C(&d, mtx, 1);
            d.x = -d.x;
            d.y = -d.y;
            d.z = -d.z;
            if (dist > 5000.0f) {
                off = dist * (0.25f + rand() / 32767.0f);
                if (fire->charged == 0) {
                    off *= 0.5f;
                }
                fn_800E1098(&up, &up, off);
                fn_800E1098(&side, &up, -off);
                GhostShotFire2(Min(5.0f * dist, 25000.0f), 133.0f, 1500.0f, host->handle, &host->pos, &up,
                               tgt->handle, &side, &c1, &c2, 0xCF0262AA, 0x96, fire->charged, fire->arg, &d);
            } else {
                off = 3.0f * dist * (0.25f + rand() / 32767.0f);
                if (fire->charged == 0) {
                    off *= 0.75f;
                }
                fn_800E1098(&up, &up, off);
                fn_800E1098(&side, &up, -off);
                GhostShotFire2(2.0f * dist, 10.0f + 120.0f * dist / 5000.0f, 1500.0f, host->handle, &host->pos, &up,
                               tgt->handle, &side, &c1, &c2, 0xCF0262AA, 0x96, fire->charged, fire->arg, &d);
            }
        }
    }
}
}

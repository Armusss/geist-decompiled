// u_80243B0C.cpp: dash / aim helpers of a possessed-actor behaviour (0x80243B0C-0x80244728).
// The four functions drive the dash-in/dash-out action state, its effects and sounds, and a
// callback that tests whether the aimed-at target is within reach before reporting a hit.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;
typedef unsigned char u8;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// From a shared header: names of the actor actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s, u32 h) {
    if (s == 0) {
        h = 0;
    } else {
        int i;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
    }
    return h;
}

struct Animator {
    char pad0[0x38];
    int handle;
};

struct AnimInfo {
    int min[3];
    int max[3];
};

struct ObjInfo {
    char pad0[0x28];
    u32 id;
};

struct IVec {
    int x, y, z;
};

struct IMtx {
    int rot[9];
    IVec pos;
};

struct IBox {
    int v[6];
};

struct Actor {
    IMtx matrix;           // 0x00
    char pad30[8];         // 0x30
    int handle;            // 0x38
    char pad3C[4];         // 0x3C
    AnimInfo* anim;        // 0x40
    char pad44[0x38];      // 0x44
    int unk7C;             // 0x7C
    char pad80[0x2A8];     // 0x80
    int unk328;            // 0x328
    char pad32C[0x34C];    // 0x32C
    int unk678;            // 0x678
    float vx;              // 0x67C
    float vy;              // 0x680
    float vz;              // 0x684
    char pad688[0x11C];    // 0x688
    int unk7A4;            // 0x7A4
    int unk7A8;            // 0x7A8
    char pad7AC[8];        // 0x7AC
    u32 flags;             // 0x7B4
    char pad7B8[0x18];     // 0x7B8
    short* animIds;        // 0x7D0
    char pad7D4[0x238];    // 0x7D4
    int bounds[6];         // 0xA0C
    char padA24[0xAD4];    // 0xA24
    int unk14F8;           // 0x14F8
    float speed;           // 0x14FC
    char pad1500[0x868];   // 0x1500
    Animator* animator;    // 0x1D68
    char pad1D6C[0x30];    // 0x1D6C
    u32 unk1D9C;           // 0x1D9C
    int unk1DA0;           // 0x1DA0
    char pad1DA4[0x8DC];   // 0x1DA4
    float dashValue;       // 0x2680
    char pad2684[0x40];    // 0x2684
    void* objects[2];      // 0x26C4
};

// The possessed actor as the dash callbacks see it.
struct LargeActor : Actor {};

struct MsgC06 {
    int unk0;
    u32 event;
    int unk8;
    int handle;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
};

struct MsgC14 {
    int unk0;
    int unk4;
    int handle;
    u32 key;
};

struct FxMsg {
    int unk0;
    u32 id;
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
};

struct PosMsg {
    int unk0;
    int type;
    int unk8;
    int unkC;
    int x;
    int y;
    int z;
    int unk1C;
    int unk20;
    int unk24;
};

struct PlaceMsg {
    int unk0;
    u32 key;
    int unk8;
};

extern "C" int SendMsg(int to, int msg, void* data);                 // 0x800265D4
int ActorPlayAnim(int actor, u32 anim, int arg);          // 0x800C5654
int PlayerInHost(Actor* player);                          // 0x80049750
void* FxFind(const char* name);                           // 0x80170834
extern "C" void FxStart(void* fx, int a, int b);                     // 0x8016F390
int ActorAim(int* out, Actor* a, int index);              // 0x8009F700
int AssetFind(u32 table, u32 key);                        // 0x8007EB34

extern "C" {
void fn_80090634(int a, int b, int c, int d);
int fn_800C5210(int handle, int msg, void* data);
void* fn_8016F120(void* fx, IVec* pos);
int fn_8001ED98(IVec* p, int len, int (*cb)(Actor*, Actor*), Actor* a, int b);
int fn_8001F45C(Actor* a, int* box, IMtx* pos, int flags, int b, int c);
int fn_800A1BC8(u32 key, u32 value, int a, int b, int c);
int fn_80073E80(Actor* a);
void fn_800E1098(float* out, float* in, float length);
}
void PlayerSetValue28(void* owner, int value);
void PlayerSetActionMode(void* owner, int mode);

static float sDashOutRate = 5000.0f;
static float sDashInRate = 5000.0f;

int DashOnEvent(int event, Actor* actor);
void DashSendAnim(Actor* a, int arg);
int DashUpdate(Actor* a);
int DashAim(Actor* actor, Actor* target);

// Tells the animator the anim finished (0 if it has no handle).
inline int AnimNotify(int to, int handle, u32 key) {
    MsgC14 q;

    if (to == -1) {
        return 0;
    }
    q.handle = handle;
    q.key = key;
    q.unk4 = 0;
    return SendMsg(to, 0xC14, &q);
}

int DashOnEvent(int event, Actor* actor) {
    LargeActor* a = (LargeActor*)actor;
    MsgC06 m;

    m.handle = a->handle;
    m.event = event;
    if (fn_800C5210(a->handle, 0xC06, &m) == 1) {
        return 0;
    }

    switch (event) {
    case 0x1B83833F: {
        Animator* an = a->animator;
        if (an != 0) {
            u32 h = StrHash("Done", a->unk1D9C);

            if (ActorPlayAnim(an->handle, h, a->handle) != 0 || AnimNotify(an->handle, a->handle, h) != 0) {
                if (a->unk1DA0 != 0) {
                    PlayerSetValue28(&a->unk7A4, a->unk1DA0);
                    fn_80090634(a->unk7C, 0, 0, 0);
                    PlayerSetActionMode(a, 0xC);
                }
            } else {
                PlayerSetActionMode(a, a->unk7A8);
            }
        } else {
            PlayerSetActionMode(a, a->unk7A8);
        }
        break;
    }
    }
    return 0;
}

void DashSendAnim(Actor* a, int arg) {
    short* index;
    int ok;
    int handle;
    int value;
    Animator* an = a->animator;

    if (an != 0) {
        handle = an->handle;
        {
            QueryMsg q;

            q.hash = 0x801EFD79;
            *(int*)&q.result = 0;
            q.unk8 = 0;
            ok = SendMsg(handle, 0x41E, &q);
            value = *(int*)&q.result;
        }
        if (ok != 0) {
            index = a->animIds + value;
        } else {
            int idx = ActorPlayAnim(an->handle, 0x4AFBF22F, a->handle);

            index = a->animIds + idx;
        }
    } else {
        index = a->animIds;
    }

    int i = PlayerInHost(a);

    fn_800A1BC8(0x3506D0CE, ((ObjInfo*)a->objects[i])->id, arg, (int)index, 0);
}

// Sets an animator query value on an actor.
inline void SetQuery(int handle, u32 hash, int value) {
    QueryMsg q;

    q.hash = hash;
    *(int*)&q.result = value;
    q.unk8 = 0;
    SendMsg(handle, 0x41D, &q);
}

int DashUpdate(Actor* a) {
    int done = 0;
    int state = *(int*)((char*)a->objects[1] + 0x12C);

    switch (state) {
    case 0xB1F00260:
        if (a->flags & 0x10000000) {
            a->dashValue -= a->speed / sDashOutRate;
            a->dashValue = MAX(a->dashValue, 0.0f);
            if ((a->unk328 & 1) != 0 || a->dashValue <= 0.0f) {
                FxMsg m;
                int fx;

                a->flags &= ~0x10000000;
                ActorPlayAnim(0x4E000000, 0x9968CC53, a->handle);
                fx = (int)FxFind("dashout");
                fn_8016F120((void*)fx, &a->matrix.pos);
                m.id = 0xCAE448F3;
                m.unkC = -1;
                m.unk10 = 0;
                m.unk14 = 0x14;
                SendMsg(0x3C000000, 0x5F03, &m);
                SetQuery(a->handle, 0xA7140990, 0x24A738);
                if (a->dashValue <= 0.0f) {
                    int id1 = AssetFind(0xAC67AC45, 0x7D3F60E5);
                    int id2 = AssetFind(0xAC67AC45, 0x7D3F60E6);

                    if (id1 != -1) {
                        AnimNotify(id1, a->handle, 0xCAD6DE50);
                    }
                    if (id2 != -1) {
                        AnimNotify(id2, a->handle, 0xCAD6DE50);
                    }
                }
            } else {
                done = 1;
            }
        } else {
            if (a->unk328 & 1) {
                FxMsg m;
                int fx;

                a->flags |= 0x10000000;
                a->unk14F8 = 2;
                ActorPlayAnim(0x4E000000, 0x798BF8DB, a->handle);
                fx = (int)FxFind("dashin");
                fn_8016F120((void*)fx, &a->matrix.pos);
                m.id = 0xCAE448F3;
                m.unkC = -1;
                m.unk10 = 0;
                m.unk14 = 0x14;
                SendMsg(0x3C000000, 0x5F03, &m);
                SetQuery(a->handle, 0xA7140990, 0x86DF6221);
                fn_8001ED98(&a->matrix.pos, a->anim->max[1] - a->anim->min[1], DashAim, a, 0);
                done = 1;
            } else {
                if (a->dashValue < 1.0f) {
                    a->dashValue += a->speed / sDashInRate;
                    a->dashValue = MIN(a->dashValue, 1.0f);
                }
            }
        }
        break;
    case 0xA921FADD:
        if (a->unk328 != 0 && (a->flags & 0x100000) == 0) {
            a->flags |= 0x200000;
            return 1;
        }
        if (a->unk328 == 0) {
            a->flags &= ~0x200000;
        }
        break;
    case 0x164545B1:
        if ((a->unk328 & 1) && PlayerInHost(a) && (a->flags & 0x40000) == 0 && (a->flags & 0x80000) == 0) {
            fn_80073E80(a);
            return 1;
        }
        break;
    case 0x8DEF6E7D:
        if (a->unk328 == 1) {
            if (a->flags & 0x80000) {
                IBox box;
                IMtx pos;

                box.v[0] = a->bounds[0];
                box.v[1] = a->bounds[1];
                box.v[2] = a->bounds[2];
                box.v[3] = a->bounds[3];
                box.v[4] = a->bounds[4];
                box.v[5] = a->bounds[5];
                pos = a->matrix;
                box.v[0] = -110;
                box.v[1] = -110;
                box.v[2] = -120;
                box.v[3] = 110;
                box.v[4] = 110;
                box.v[5] = 120;
                pos.pos.z += 400;
                if (fn_8001F45C(a, box.v, &pos, 0x10000000, 0, 0) == 0) {
                    a->flags &= ~0x80000;
                    a->unk678 |= 0x80;
                }
            } else {
                a->flags |= 0x80000;
                a->unk678 |= 0x80;
            }
            done = 1;
        }
        break;
    case 0xA9315EDB:
        if (a->unk328 == 1) {
            int fx = (int)FxFind("rat_jump");

            FxStart((void*)fx, 0, 0);
            return 1;
        }
        break;
    case 0x164350F8:
    case 0xB2270119:
        break;
    }

    return done;
}

int DashAim(Actor* actor, Actor* target) {
    if (actor != 0) {
        LargeActor* a = (LargeActor*)actor;

        if ((int)((u32)target->handle >> 24) == 2) {
            int h = a->handle;

            if ((int)((u32)h >> 24) == 1) {
                PosMsg m;
                int ok;
                int value;

                {
                    QueryMsg q;

                    q.hash = 0x914B98A8;
                    *(int*)&q.result = 0;
                    q.unk8 = 0;
                    ok = SendMsg(h, 0x41E, &q);
                    value = *(int*)&q.result;
                }
                if (ok != 0 && value >= 0) {
                    float r[3];
                    int v[3];
                    float ax, ay, az;
                    float t;

                    ActorAim(v, a, value);
                    ax = (float)v[0] * 0.000244140625f;
                    ay = (float)v[1] * 0.000244140625f;
                    az = (float)v[2] * 0.000244140625f;
                    v[0] = target->matrix.pos.x - a->matrix.pos.x;
                    v[1] = target->matrix.pos.y - a->matrix.pos.y;
                    v[2] = target->matrix.pos.z - a->matrix.pos.z;
                    t = (ax * (float)v[0] + ay * (float)v[1] + az * (float)v[2]) / (ax * ax + ay * ay + az * az);
                    r[0] = ax * t;
                    r[1] = ay * t;
                    r[2] = az * t;

                    float range = (float)(target->anim->max[1] - target->anim->min[1]);
                    float limit;

                    if (t < 0.0f) {
                        limit = 1.1f * (range - (float)a->anim->min[value]);
                    } else {
                        limit = 1.1f * (range + (float)a->anim->max[value]);
                    }
                    if (r[0] * r[0] + r[1] * r[1] + r[2] * r[2] < limit * limit) {
                        float px = -r[0];
                        float py = -r[1];
                        float pz = -r[2];

                        px += target->vx;
                        py += target->vy;
                        pz += target->vz;
                        fn_800E1098(r, r, limit);
                        r[0] += px;
                        r[1] += py;
                        r[2] += pz;
                        m.type = 1;
                        m.x = (int)r[0];
                        m.y = (int)r[1];
                        m.z = (int)r[2];
                        SendMsg(target->handle, 0xC07, &m);
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}

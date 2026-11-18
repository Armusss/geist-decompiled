// u_8017EC24.cpp (0x8017EC24-0x8017F9C0), split from u_8017D794.
// "HydraSpit" effect type. Starts after fn_8017EAB0 (called only by fn_8017E288 of the previous file and
// using its constants); its descriptor 0x803A8508 (name "HydraSpit", rodata 0x80346F40 with its own copy of the
// POSSESS..SCAN action names) and vtable-like table 0x803A84A8 point only into 0x8017EC24-0x8017F9C0.
// sdata2 0x804D8270-0x804D8298, sbss 0x804D50C8.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"
#include "game/owner.h"
#include "game/effect.h"


void BehUpdate(Owner* owner, Beh* beh);                         // 0x801632A8
void BehDone(Beh* beh);                                         // 0x80162EBC
void Fn8016295C(Owner* owner, Beh* beh);                        // 0x8016295C
void VFree(void* p);                                            // 0x8009DA3C
void* VAlloc(int size);                                         // 0x8009DA68
void BehReset(Beh* beh);                                        // 0x801633D4
int HasCondition(void* owner, int id);                          // 0x800A7AC4
int AssetFind(u32 group, u32 name);                             // 0x8007EB34
int OwnerId(Owner* owner);                                      // 0x800A3A40
int Fn800F7DEC(int a, int b, int msg, int who, void* data);     // 0x800F7DEC
void Fn800F7F5C(int a, int b, int msg, int who, void* data, float f); // 0x800F7F5C
int Fn80161FF4(Owner* owner, void* data, float f);              // 0x80161FF4

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

extern BehState lbl_803A84A8[8];

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

struct SpitShot {
    u8 pad[0x30];
    Vec3 pos;             // 0x30
    Vec3 dir;             // 0x3C
    int unk48;            // 0x48
    float unk4C;          // 0x4C
    float spread;         // 0x50
};

struct SpitFire {
    SpitShot* shot;
    float speed;
};

// Projectile spawn parameters for fn_80186A88 (0x4C bytes, as in Homing).
struct SpitInfo {
    Vec3 pos;             // 0x00
    Vec3 up;              // 0x0C
    Vec3 vel;             // 0x18
    u8 pad24[0x4];
    float range;          // 0x28
    u8 pad2C[0x20];
};

extern EffectType lbl_803A8508;  // the "HydraSpit" type descriptor
unsigned long Fn80175D60(unsigned long arg);

struct HydraSpitData;

// Context for the spit hit sweep (fn_8001ED98 calls fn_8017EC24 for each object near pos).
struct SpitCtx {
    Owner* owner;         // 0x00
    Beh* beh;             // 0x04
    HydraSpitData* data;  // 0x08
    int asset;            // 0x0C
    IVec3 pos;            // 0x10
};

// A model node (0x1C bytes); the world position is at +0x10.
struct ModelNode {
    u8 pad00[0x10];
    Vec3 pos;             // 0x10
};

int ModelAnimate(int model, char* name, int a);                 // 0x8003AA9C
static int sHydraSpitAssets[2];

// Leave the current state for d->next, telling the owner (arg = d->unk08).
inline void HydraSpitGoto(Owner* o, HydraSpitData* d);

// The behaviour's own state (Beh::data).
struct HydraSpitData {
    int state;            // 0x00: index into sHydraSpitStates
    int next;             // 0x04
    int unk08;            // 0x08
    int flags;            // 0x0C
    int target;           // 0x10: set by message 0x41D 0x2C4F0F0A
    int hitCount;         // 0x14
    int hitAsset[4];      // 0x18: which spit (asset) hit ...
    int hitHandle[4];     // 0x28: ... which object
};

inline void HydraSpitGoto(Owner* o, HydraSpitData* d) {
    if (lbl_803A84A8[d->state].exit) {
        lbl_803A84A8[d->state].exit(o, &o->beh);
    }
    d->state = d->next;
    o->beh.state = d->next;
    if (o->beh.callback) {
        o->beh.callback(o, &o->beh, d->unk08);
    }
    if (lbl_803A84A8[d->state].enter) {
        lbl_803A84A8[d->state].enter(o, &o->beh);
    }
}

extern "C" {
int SendMsg(int to, int msg, void* data);                       // 0x800265D4

// An object found by the sweep.
struct SpitTarget {
    u8 pad00[0x24];
    IVec3 pos;            // 0x24
    u8 pad30[0x8];
    int handle;           // 0x38: top byte is the object kind
};

// Message 0x407 sent to a hit object (0x50 bytes).
struct SpitHitMsg {
    SpitTarget* obj;      // 0x00
    Owner* owner;         // 0x04
    int ownerId;          // 0x08
    float damage;         // 0x0C
    int unk10;            // 0x10
    u32 hash;             // 0x14
    IVec3 pos;            // 0x18
    int unk24[3];         // 0x24
    IVec3 delta;          // 0x30
    int unk3C;            // 0x3C
    int unk40;            // 0x40
    int unk44;            // 0x44
    int unk48;            // 0x48
    float unk4C;          // 0x4C
    int unk50[3];         // 0x50
};

int fn_800E0DBC(IVec3* v);

void fn_8001ED98(IVec3* pos, int radius, int (*callback)(void*, void*), void* ctx, int flag);

int rand();
void fn_800E1098(void* out, void* in, float length);
void fn_8016D440(Vec3* out, Vec3* in);
void fn_8016D4EC(Vec3* out, Vec3* in);
void fn_80186A88(int owner, void* info, int arg, int a, int b, float f);

inline int SpitRange(int base, SpitShot* shot) {
    return base + shot->spread;
}

int fn_8017EC24(void* obj_, void* ctx_) {
    SpitTarget* obj = (SpitTarget*)obj_;
    SpitCtx* ctx = (SpitCtx*)ctx_;
    int handle;
    int i;
    IVec3 delta;
    Msg400 q;
    SpitHitMsg m;

    if (obj == 0) {
        return 0;
    }
    handle = obj->handle;
    if (handle == ctx->owner->id) {
        return 0;
    }
    if ((int)((u32)handle >> 24) != 2) {
        return 0;
    }
    for (i = 0; i < ctx->data->hitCount; i++) {
        if (ctx->data->hitAsset[i] == ctx->asset && ctx->data->hitHandle[i] == handle) {
            return 0;
        }
    }
    SendMsg(handle, 0x400, &q);
    if (q.state == 0) {
        return 0;
    }
    delta.x = obj->pos.x - ctx->pos.x;
    delta.y = obj->pos.y - ctx->pos.y;
    delta.z = obj->pos.z - ctx->pos.z;
    if (fn_800E0DBC(&delta) > 1500) {
        return 0;
    }
    m.owner = ctx->owner;
    m.ownerId = ctx->owner->id;
    m.hash = 0x6D57E804;
    m.damage = 150.0f;
    m.unk4C = 1.0f;
    m.pos.x = obj->pos.x;
    m.pos.y = obj->pos.y;
    m.pos.z = obj->pos.z;
    m.unk48 = 0;
    m.unk44 = -1;
    m.unk40 = -1;
    m.unk3C = -1;
    m.obj = obj;
    m.delta.x = delta.x;
    m.delta.y = delta.y;
    m.delta.z = delta.z;
    SendMsg(obj->handle, 0x407, &m);
    ctx->data->hitHandle[ctx->data->hitCount] = obj->handle;
    ctx->data->hitAsset[ctx->data->hitCount] = ctx->asset;
    ctx->data->hitCount++;
    return 1;
}

void fn_8017EE10(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        HydraSpitData* d = (HydraSpitData*)b->data;
        u8* map;
        SpitCtx ctx;

        BehUpdate(o, b);
        Fn8016295C(o, b);
        map = (u8*)ModelAnimate(o->model, 0, 0);
        ctx.owner = o;
        ctx.beh = b;
        ctx.data = d;
        switch (d->target) {
        case 0:
        case 1: {
            Vec3* n;
            ctx.asset = sHydraSpitAssets[0];
            n = &((ModelNode*)o->nodeF0)[map[ctx.asset]].pos;
            ctx.pos.x = n->x;
            ctx.pos.y = n->y;
            ctx.pos.z = n->z;
            fn_8001ED98(&ctx.pos, 750, fn_8017EC24, &ctx, 0);
            ctx.asset = sHydraSpitAssets[1];
            n = &((ModelNode*)o->nodeF0)[map[ctx.asset]].pos;
            ctx.pos.x = n->x;
            ctx.pos.y = n->y;
            ctx.pos.z = n->z;
            fn_8001ED98(&ctx.pos, 750, fn_8017EC24, &ctx, 0);
            break;
        }
        case 2:
            break;
        }
    }
}

void fn_8017EF9C(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}

void fn_8017EFE8(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}

void fn_8017F034(void* owner, void* beh) {
}

void fn_8017F038(void* owner, void* beh) {
}

void fn_8017F03C(void* owner, void* beh) {
}

void fn_8017F040(void* owner, void* beh) {
    if (beh != 0) {
        HydraSpitData* d = (HydraSpitData*)((Beh*)beh)->data;
        d->flags = 0;
        d->next = 0;
        d->unk08 = 0;
        BehDone((Beh*)beh);
        d->hitCount = 0;
        memset(d->hitHandle, 0, sizeof(d->hitHandle));
        memset(d->hitAsset, 0, sizeof(d->hitAsset));
    }
}

void fn_8017F0B0(void* owner, void* beh) {
    if (beh != 0) {
        HydraSpitData* d = (HydraSpitData*)((Beh*)beh)->data;
        d->flags = 0;
        d->next = 0;
        d->unk08 = 0;
        BehDone((Beh*)beh);
    }
}

void fn_8017F0F0(void* owner, void* beh) {
    if (beh != 0) {
        HydraSpitData* d = (HydraSpitData*)((Beh*)beh)->data;
        d->flags = 0;
        d->next = 0;
        d->unk08 = 0;
        BehDone((Beh*)beh);
    }
}

int fn_8017F130(Owner* owner, Beh* beh, int msg, void* data) {
    HydraSpitData* d;

    if (beh == 0) {
        return 0;
    }
    d = (HydraSpitData*)beh->data;
    switch (msg) {
    case 0x41D:
        switch ((int)((Msg41D*)data)->cmd) {
        case 0x2C4F0F0A:
            d->target = ((Msg41D*)data)->arg;
            return 1;
        }
        return 0;
    }
    return 0;
}

int fn_8017F188(void* owner, void* beh, int msg) {
    Owner* o;
    Beh* b;

    if (beh == 0) {
        return 0;
    }
    o = (Owner*)owner;
    b = (Beh*)beh;
    {
        HydraSpitData* d = (HydraSpitData*)b->data;

        switch (msg) {
        case 0:
            d->next = 0;
            d->unk08 = 0;
            HydraSpitGoto(o, d);
            break;
        case 1:
            d->next = 4;
            d->unk08 = 1;
            if (d->state == 0) {
                HydraSpitGoto(o, d);
            }
            break;
        case 2:
            d->next = 5;
            d->unk08 = 2;
            if (d->state == 0) {
                HydraSpitGoto(o, d);
            }
            break;
        case 0x1B83833F:
            HydraSpitGoto(o, d);
            break;
        case 0x20EE36:
            if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, 0x20EE36, OwnerId(o), 0) == 0x4A3E183) {
                char buf[0x60];
                if (Fn80161FF4(o, buf, 0.0f) == 0) {
                    return 0;
                }
                Fn800F7F5C(o->beh.unk58, o->beh.unk5C, 0x20EE36, OwnerId(o), buf, 0.0f);
            }
            break;
        case (int)0xF829DB93:
            break;
        }
    }
    return 1;
}

int fn_8017F504(void* owner_, void* beh_) {
    Beh* b = (Beh*)beh_;
    HydraSpitData* d;
    Owner* o = (Owner*)owner_;

    if (b == 0) {
        return 0;
    }
    d = (HydraSpitData*)b->data;
    if (HasCondition(o->conditions, 9) != 0 ||
        HasCondition(o->conditions, 10) != 0 ||
        HasCondition(o->conditions, 11) != 0) {
        d->flags = 0;
        BehReset(b);
        BehDone(b);
    } else {
        if (lbl_803A84A8[d->state].update) {
            lbl_803A84A8[d->state].update(o, b);
        }
    }
    return 1;
}

int fn_8017F5DC(Owner* owner, Beh* beh) {
    if (beh == 0) {
        return 0;
    }
    VFree(beh->data);
    return 1;
}

int fn_8017F614(void* owner_, void* beh_) {
    Beh* beh;

    if (beh_ == 0) {
        return 0;
    }
    beh = (Beh*)beh_;
    {
        HydraSpitData* d = (HydraSpitData*)VAlloc(0x38);
        if (d == 0) {
            beh->data = 0;
            return 0;
        }
        beh->data = d;
        beh->state = 0;
        d->state = 0;
        d->next = 0;
        d->unk08 = 0;
        d->flags = 0;
        d->target = 0;
        sHydraSpitAssets[0] = AssetFind(0xF1754BF2, 0x9147B62C);
        sHydraSpitAssets[1] = AssetFind(0xF1754BF2, 0x870AA272);
        if (lbl_803A84A8[d->state].enter) {
            lbl_803A84A8[d->state].enter(owner_, beh_);
        }
    }
    return 1;
}

u32 fn_8017F700(EffectObj* obj, int msg) {
    return 0x3DA260F7;
}

int fn_8017F70C(EffectObj* obj, int msg, int arg, void* data, float dt) {
    int busy = Fn80175D60(arg);

    switch (msg) {
    case 0x14967: {
        int range;
        SpitFire* fire = (SpitFire*)data;
        SpitShot* shot = fire->shot;
        EffectType* type = &lbl_803A8508;
        int owner;
        float a;
        float b;
        SpitInfo info;
        Vec3 rot;
        Vec3 up;

        range = SpitRange(10, shot);
        owner = obj->unk0;
        EffectTypeHash(type);
        memset(&info, 0, sizeof(SpitInfo));
        info.pos = shot->pos;
        fn_8016D440(&up, &shot->dir);
        info.up = up;
        info.range = 5000.0f;
        fn_8016D440(&rot, &shot->dir);
        a = 360.0f * (float)(rand() % range) / 4096.0f;
        b = 360.0f * (float)(rand() % range) / 4096.0f;
        if (rand() % 2) {
            rot.z += a;
        } else {
            rot.z -= a;
        }
        if (rand() % 2) {
            rot.x += b;
        } else {
            rot.x -= b;
        }
        fn_8016D4EC(&info.vel, &rot);
        fn_800E1098(&info.vel, &info.vel, fire->speed);
        fn_80186A88(owner, &info, arg, shot->unk48, 0, shot->unk4C);
        return 1;
    }
    }
    return 0;
}

// State table at 0x803A84A8: enter / exit / update for states 0-7.
BehState lbl_803A84A8[8] = {
    { fn_8017F0F0, fn_8017F03C, fn_8017EFE8 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { fn_8017F0B0, fn_8017F038, fn_8017EF9C },
    { fn_8017F040, fn_8017F034, fn_8017EE10 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
};

EffectType lbl_803A8508 = {
    "HydraSpit", 0, {0, 0, 0}, 0, 0, fn_8017F70C, fn_8017F700, 0, 0, 0,
};
}

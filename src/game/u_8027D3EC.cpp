// u_8027D3EC.cpp: possessed-actor effects (create, update, query) and effect helpers (0x8027D3EC-0x8028009C).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;
typedef int s32;
typedef float f32;
typedef double f64;

extern "C" void* memset(void* dst, int val, u32 len);
extern "C" void* memcpy(void* dst, const void* src, u32 len);
extern "C" void fn_8016D68C(void* dst, u32 arg);
extern "C" u32 fn_800C7E80(void* desc);
extern "C" u32 fn_801DE990(void* params);
extern "C" void fn_800265D4(u32 handle, s32 flag, void** out);
u32 AssetFind(u32 a, u32 b);

struct Actor;
void PossessableDestroy(Actor* actor);

extern f32 lbl_804D3780;
extern f32 lbl_804DA3C0;
extern f32 lbl_804DA3C4;
extern f32 lbl_804DA3C8;
extern f32 lbl_804DA3CC;
extern f32 lbl_804DA3D0;
extern f32 lbl_804DA3D4;
extern f32 lbl_804DA3D8;
extern f32 lbl_804DA3DC;
extern f32 lbl_804DA3E0;
extern f32 lbl_804DA3E4;
extern f32 lbl_804DA3E8;
extern f32 lbl_804DA3EC;
extern f32 lbl_804DA3F0;
extern f64 lbl_804DA3F8;

// Spawn descriptor filled in before the lookup (0x38 bytes).
struct SpawnDesc {
    f32 posX;                // 0x00
    f32 posY;                // 0x04
    f32 posZ;                // 0x08
    u8  unk0c[0x14];         // 0x0c
    u32 id;                  // 0x20
    u32 unk24;               // 0x24
    u32 handle;              // 0x28
    u8  unk2c[0x0c];         // 0x2c
};

struct EffTarget;

// Object returned by fn_800C7E80.
struct SpawnOwner {
    u8  unk00[0x24];         // 0x00
    u32 a24;                 // 0x24
    u32 a28;                 // 0x28
    u32 a2c;                 // 0x2c
    u8  unk30[0x08];         // 0x30
    u32 a38;                 // 0x38
    u8  unk3c[0x68];         // 0x3c
    EffTarget* target;       // 0xa4
};

// Object the effects are attached to.
struct EffTarget {
    u8  unk00[0x0c];         // 0x00
    u32 a0c;                 // 0x0c
    f32 f10;                 // 0x10
    u8  unk14[0x04];         // 0x14
    u32 flags;               // 0x18
    u8  unk1c[0x14];         // 0x1c
    f32 f30;                 // 0x30
    u32 a34;                 // 0x34
    u32 a38;                 // 0x38
    u32 a3c;                 // 0x3c
    u32 a40;                 // 0x40
    u32 a44;                 // 0x44
    u32 a48;                 // 0x48
    u32 a4c;                 // 0x4c
    u32 a50;                 // 0x50
    u32 a54;                 // 0x54
    f32 f58;                 // 0x58
    f32 f5c;                 // 0x5c
    f32 f60;                 // 0x60
    f32 f64;                 // 0x64
    f32 f68;                 // 0x68
    f32 f6c;                 // 0x6c
    f32 f70;                 // 0x70
    f32 f74;                 // 0x74
};

// Object handed back through the query helper.
struct EffObject {
    u8  unk00[0x4c];         // 0x00
    u32 flags;               // 0x4c
};

// Effect creation parameters (0x3c bytes).
struct EffectParams {
    u8  unk00[0x04];         // 0x00
    s32 owner;               // 0x04
    s32 mode;                // 0x08
    s32 life;                // 0x0c
    f32 f10;                 // 0x10
    f32 f14;                 // 0x14
    f32 f18;                 // 0x18
    s32 n1c;                 // 0x1c
    f32 f20;                 // 0x20
    f32 f24;                 // 0x24
    f32 f28;                 // 0x28
    f32 f2c;                 // 0x2c
    u32 asset;               // 0x30
    u8  r;                   // 0x34
    u8  g;                   // 0x35
    u8  b;                   // 0x36
    u8  unk37;               // 0x37
    u8  unk38[0x04];         // 0x38
};

// Builds the effect set for an object spawned from a descriptor.
u32 ActorCreateEffects(u32 param_1, u32 param_2, f32* param_3, u32 param_4, int param_5) {
    SpawnDesc desc;
    EffectParams fx;
    EffObject* out;
    SpawnOwner* owner;
    EffTarget* ent;

    memset(&desc, 0, sizeof(desc));
    desc.id = param_1;
    desc.handle = 0xffffffff;
    desc.posX = param_3[0];
    desc.posY = param_3[1];
    desc.posZ = param_3[2];
    fn_8016D68C(desc.unk0c, param_4);

    owner = (SpawnOwner*)fn_800C7E80(&desc);
    if (owner == 0 || owner->target == 0) {
        return 0xffffffff;
    }
    ent = owner->target;

    ent->flags |= 0x20000000;
    ent->a34 = param_2;
    ent->a38 = param_2;
    ent->f58 = lbl_804D3780;
    ent->f30 = lbl_804DA3C0;

    memset(&fx, 0, sizeof(fx));
    fx.owner = 0xffffffff;
    fx.mode = 0;
    fx.life = 0x96;
    fx.f14 = lbl_804DA3C4;
    fx.f18 = lbl_804DA3C0;
    fx.n1c = 0x60;
    fx.f20 = lbl_804DA3C8;
    fx.f24 = lbl_804DA3C0;
    fx.f28 = lbl_804DA3C0;
    fx.f2c = lbl_804DA3C0;
    fx.asset = AssetFind(0xf2e2beb8, 0x259fba81);
    fx.r = 0xff;
    fx.g = 0xff;
    fx.b = 0xff;
    fx.owner = owner->a38;
    fx.f10 = lbl_804DA3CC;
    fx.mode = 0xffffffff;
    ent->a48 = fn_801DE990(&fx);
    fx.f20 = lbl_804DA3D0;
    ent->a4c = fn_801DE990(&fx);
    fn_800265D4(ent->a48, 0x400, (void**)&out);
    if (out != 0) {
        out->flags &= 0xfffdffff;
    }
    fn_800265D4(ent->a4c, 0x400, (void**)&out);
    if (out != 0) {
        out->flags &= 0xfffdffff;
    }

    fx.owner = 0xffffffff;
    fx.mode = 0;
    fx.life = 1000;
    fx.f14 = lbl_804DA3C0;
    fx.f18 = lbl_804DA3D4;
    fx.n1c = 0x60;
    fx.f20 = lbl_804DA3C8;
    fx.f24 = lbl_804DA3D8;
    fx.f28 = lbl_804DA3C0;
    fx.f2c = lbl_804DA3C0;
    fx.asset = AssetFind(0xf2e2beb8, 0xf9ecbd80);
    fx.r = 0xff;
    fx.g = 0xff;
    fx.b = 0xff;
    fx.owner = owner->a38;
    fx.f10 = lbl_804DA3D4;
    fx.mode = 0xffffffff;
    ent->a3c = fn_801DE990(&fx);
    fn_800265D4(ent->a3c, 0x400, (void**)&out);
    if (out != 0) {
        out->flags &= 0xfffdffff;
    }

    fx.f24 = lbl_804DA3DC;
    fx.f14 = lbl_804DA3E0;
    fx.f18 = lbl_804DA3D4;
    fx.f20 = lbl_804DA3E4;
    ent->a40 = fn_801DE990(&fx);
    fn_800265D4(ent->a40, 0x400, (void**)&out);
    if (out != 0) {
        out->flags &= 0xfffdffff;
    }

    fx.f24 = lbl_804DA3E8;
    fx.f14 = lbl_804DA3EC;
    fx.f18 = lbl_804DA3D4;
    fx.f20 = lbl_804DA3F0;
    ent->a44 = fn_801DE990(&fx);
    fn_800265D4(ent->a44, 0x400, (void**)&out);
    if (out != 0) {
        out->flags &= 0xfffdffff;
    }

    if (param_5 != 0) {
        ent->flags |= 0x40000000;
    }

    ent->f5c = (f32)owner->a24;
    ent->f60 = (f32)owner->a28;
    ent->f64 = (f32)owner->a2c;
    ent->f68 = (f32)owner->a24;
    ent->f6c = (f32)owner->a28;
    ent->f70 = (f32)owner->a2c;
    return owner->a38;
}

// Releases the possessable object passed in.
void DestroyPossessable(Actor* actor) {
    PossessableDestroy(actor);
}

// ---- possession effect helpers ---------------------------------------------

struct Vec3 {
    f32 x;                   // 0x00
    f32 y;                   // 0x04
    f32 z;                   // 0x08
};

// Bounding box in minimum/maximum corner form.
struct Bounds {
    f32 minX;                // 0x00
    f32 minY;                // 0x04
    f32 minZ;                // 0x08
    f32 maxX;                // 0x0c
    f32 maxY;                // 0x10
    f32 maxZ;                // 0x14
};

// 3x4 transformation matrix.
struct Mtx34 {
    f32 m[3][4];             // 0x00
};

// Placement handed to the actor interface (position + basis, 3x4).
struct PlaceF {
    f32 m[3][4];             // 0x00
};

// Object found through the actor's 0x84 pointer; carries an id at 0xa4.
struct IdSource {
    u8  unk00[0xa4];         // 0x00
    s32 id;                  // 0xa4
};

struct Actor {
    u8  unk00[0x24];         // 0x00
    s32 x24;                 // 0x24
    s32 x28;                 // 0x28
    s32 x2c;                 // 0x2c
    u8  unk30[0x08];         // 0x30
    u32 a38;                 // 0x38
    u8  unk3c[0x24];         // 0x3c
    s32 x60;                 // 0x60
    u8  unk64[0x20];         // 0x64
    IdSource* src;           // 0x84
    u8  unk88[0x1c];         // 0x88
    EffTarget* target;       // 0xa4
    u8  unka8[0x04];         // 0xa8
    u32 ac;                  // 0xac
};

// Message payloads handled by the possession effect actor.
struct PossessDamageMsg {
    Actor* actor;            // 0x00
    u32 unk04;               // 0x04
    u32 kind;                // 0x08
    f32 amount;              // 0x0c
    u32 unk10;               // 0x10
    s32 id;                  // 0x14
};

struct PossessFlagMsg {
    Actor* actor;            // 0x00
    s32 hash;                // 0x04
    u32 unk08;               // 0x08
    s32 value;               // 0x0c
};

struct PossessQueryMsg {
    Actor* actor;            // 0x00
    s32 hash;                // 0x04
    u32 unk08;               // 0x08
    u32 out;                 // 0x0c
};

struct PossessTeleportMsg {
    Actor* actor;            // 0x00
    u32 flags;               // 0x04
    u8  unk08[0x08];         // 0x08
    s32 x;                   // 0x10
    s32 y;                   // 0x14
    s32 z;                   // 0x18
};

struct PossessRenderMsg {
    Actor* actor;            // 0x00
    u32 unk04;               // 0x04
    s32 a08;                 // 0x08
    s32 hash;                // 0x0c
};

// Result buffer filled by the proximity query object.
struct QueryOut {
    void* object;            // 0x00
    s32 found;               // 0x04
    u8  unk08[0x10];         // 0x08
    u32 flags;               // 0x18
};

// Context handed to the proximity query callback.
struct QueryCtx {
    void* object;            // 0x00
    u8  unk04[0x2c];         // 0x04
    s32 a30;                 // 0x30
    s32 a34;                 // 0x34
    s32 a38;                 // 0x38
};

typedef void (*QueryFn)(s32, QueryOut*);

extern f32 lbl_804D3784;
extern f32 lbl_804DA400;
extern f32 lbl_804DA404;
extern s32 lbl_804A3208;
extern s32 lbl_804A320C;
extern s32 lbl_804A3210;
extern s32 lbl_804D5548;
extern s32 lbl_804D554C;

void ActorRender(Actor* actor);
void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void ActorSetPlacement(Actor* actor, PlaceF* place);
int ActorFn80022FD8(Actor* actor);
void PossessableUpdate(Actor* actor, f32 dt);
void TrailDestroy(int id);
void* VAlloc(int size);
int PossessableInit(Actor* actor);
int PossessableMsg(Actor* actor, int msg, void* data, int arg);
void ModelLoadIds(short* ids, int count);
extern "C" int SendMsg(int to, int msg, void* data);

extern "C" int fn_800201E4(Actor* actor, Bounds* bounds, u32 mask,
                           int (*cb)(void*, void*), void* ctx);
extern "C" void fn_800A0168(PlaceF* out, Mtx34* in);
extern "C" void fn_800A0030(Vec3* out, PlaceF* place, Vec3* in);
extern "C" void fn_800E1098(Vec3* out, Vec3* in, f32 scale);
extern "C" void fn_800C7B78(Actor* actor);
extern "C" int fn_802A1D04(void);
extern "C" int fn_80049940(void);
u32 WhiteOutEffectActive(void);
void PossessionEffectWhiteOut(Actor* actor, u32 id);

int PossessedEffectQuery(void* ctx, void* target);

// Destroys the effect's trails and starts the fade-out timer.
static inline void StartPossessionFade(EffTarget* self) {
    TrailDestroy(self->a3c);
    TrailDestroy(self->a40);
    TrailDestroy(self->a44);
    TrailDestroy(self->a48);
    TrailDestroy(self->a4c);
    self->f74 = lbl_804DA404;
    self->flags |= 0x04000000;
}

// Releases the possession effect once the possessed actor is gone.
static inline void ReleasePossessionEffect(EffTarget* self, Actor* actor, f32 dt) {
    if (fn_802A1D04() == 0) {
        StartPossessionFade(self);
    } else if (self->flags & 0x20000000) {
        StartPossessionFade(self);
    } else if (self->f74 == lbl_804DA3C0) {
        fn_800C7B78(actor);
    } else if (self->flags & 0x10000000) {
        self->f74 -= dt;
        if (self->f74 < lbl_804DA3C0) {
            self->f74 = lbl_804DA3C0;
        }
    }
}

// Per-frame update of the possession effect: follows the possessed actor's
// motion and releases the effect when the possession ends.
void PossessedEffectUpdate(Actor* actor, f32 dt) {
    EffTarget* self = actor->target;
    Vec3 move;
    Vec3 expand;
    Bounds bounds;
    u8 msgBuf[16];
    Mtx34 mtxPlace;
    Mtx34 mtxWork;
    Mtx34 mtxTmp;
    PlaceF place;
    f32 startX;
    f32 startY;
    f32 startZ;
    int found;

    move.x = lbl_804DA3C0;
    move.y = lbl_804D3780 * dt;
    move.z = lbl_804DA3C0;
    self->f30 += dt;

    if (self->flags & 0x04000000) {
        self->f74 -= dt;
        if (self->f74 < lbl_804DA3C0) {
            if (WhiteOutEffectActive() == 0) {
                ActorRender(actor);
            }
        }
        return;
    }

    if (self->flags & 0x80000000) {
        ReleasePossessionEffect(self, actor, dt);
        return;
    }

    if (SendMsg((int)actor->a38, 0xc01, msgBuf) != 0 &&
        (self->flags & 0x40000000) && !(self->flags & 1)) {
        self->f68 = self->f5c;
        self->f6c = self->f60;
        self->f70 = self->f64;
        startX = self->f5c;
        startY = self->f60;
        startZ = self->f64;

        ActorGetMatrix(&mtxTmp, actor);
        fn_800A0168(&place, &mtxTmp);
        fn_800A0030(&move, &place, &move);

        self->f5c += move.x;
        self->f60 += move.y;
        self->f64 += move.z;

        actor->x24 = (s32)self->f5c;
        actor->x28 = (s32)self->f60;
        actor->x2c = (s32)self->f64;

        ActorGetMatrix(&mtxPlace, actor);
        ActorSetPlacement(actor, (PlaceF*)&mtxPlace);
        actor->ac |= 0x80000000;

        bounds.minX = startX;
        bounds.minY = startY;
        bounds.minZ = startZ;
        bounds.maxX = self->f5c;
        bounds.maxY = self->f60;
        bounds.maxZ = self->f64;

        expand.x = self->f5c - startX;
        expand.y = self->f60 - startY;
        expand.z = self->f64 - startZ;
        fn_800E1098(&expand, &expand, lbl_804DA400);

        bounds.minX -= expand.x;
        bounds.maxX += expand.x;
        bounds.minY -= expand.y;
        bounds.maxY += expand.y;
        bounds.minZ -= expand.z;
        bounds.maxZ += expand.z;

        lbl_804D5548 = 0;
        lbl_804D554C = 0;
        found = fn_800201E4(actor, &bounds, 0x12000000,
                            PossessedEffectQuery, &self->a38);
        if (found == 0) {
            if (lbl_804D5548 == 0) {
                self->a38 = 0xffffffff;
            }
        } else if (lbl_804D554C != 0 || lbl_804D3784 < self->f30 ||
                   fn_80049940() != 0) {
            actor->x24 = lbl_804A3208;
            actor->x28 = lbl_804A320C;
            actor->x2c = lbl_804A3210;
            PossessionEffectWhiteOut(actor, self->a34);
        }
    }

    PossessableUpdate(actor, dt);
    if (ActorFn80022FD8(actor) == 0) {
        ReleasePossessionEffect(self, actor, dt);
    }
}

// Proximity query callback: resolves the candidate object through the context
// object and reports whether it is the effect's current target.
int PossessedEffectQuery(void* ctx, void* target) {
    QueryCtx* q = (QueryCtx*)ctx;
    s32* handle = (s32*)target;
    QueryOut out;

    if (q->object != 0) {
        QueryFn fn = *(QueryFn*)((u8*)q->object + 0x30);
        out.object = q->object;
        fn(0x400, &out);
        if (out.found == *handle || (out.flags & 2) == 0) {
            if (out.found == *handle) {
                lbl_804D5548 = 1;
            }
            return 0;
        }
        lbl_804D554C = 1;
    }
    lbl_804A3208 = q->a30;
    lbl_804A320C = q->a34;
    lbl_804A3210 = q->a38;
    return 1;
}

// Creates the possession effect state attached to the actor and seeds its
// bounding positions from the actor's current cell coordinates.
int PossessedEffectCreate(Actor* actor) {
    EffTarget* self;
    IdSource* src;
    s32 id;
    short ids[4];

    self = actor->target;
    if (self == 0) {
        self = (EffTarget*)VAlloc(0x78);
        actor->target = self;
    }
    memset(self, 0, 0x78);
    if (PossessableInit(actor) == 0) {
        actor->ac |= 0x01000000;
        return 0;
    }

    self->f5c = (f32)actor->x24;
    self->f60 = (f32)actor->x28;
    self->f64 = (f32)actor->x2c;
    self->f68 = (f32)actor->x24;
    self->f6c = (f32)actor->x28;
    self->f70 = (f32)actor->x2c;
    actor->ac &= 0xdfffffff;
    self->flags |= 0x10000000;
    self->flags &= 0xfffffff7;
    self->a50 = 0xffffffff;
    self->a54 = 0xffffffff;
    self->a3c = 0xffffffff;
    self->a40 = 0xffffffff;
    self->a44 = 0xffffffff;
    self->a48 = 0xffffffff;
    self->a4c = 0xffffffff;

    if (fn_802A1D04() != 0) {
        src = actor->src;
        if (src != 0) {
            id = src->id;
            if (id != -1) {
                ids[0] = (short)id;
                ModelLoadIds(ids, 1);
            }
        }
    }
    return 1;
}

// Message handler for the possession effect actor. Opcodes 0x407, 0x41d,
// 0x41e, 0xc07 and 0xc14 are handled here; everything else falls through to
// the shared possessable handler.
int PossessedEffectMsg(Actor* actor, int msg, void* data, int arg) {
    Actor* a;
    EffTarget* self;
    Mtx34 mtx;
    Vec3 delta;
    int result;
    s32 id;
    u32 kind;

    result = 0;
    switch (msg) {
    case 0x407: {
        PossessDamageMsg* m = (PossessDamageMsg*)data;
        a = m->actor;
        self = a->target;
        kind = m->kind >> 24;
        if (kind == 2 || kind == 1) {
            id = m->id;
            switch (id) {
            case 0x403548d0:
                self->flags |= 2;
                break;
            case -0x4603bd0b:
                self->f10 = lbl_804DA3C0;
                break;
            case -0x59fd581a:
            case 0x75715902:
                self->f10 -= m->amount;
                break;
            case 0x6ca5f172:
                if (fn_80049940() != 0) {
                    self->f10 -= m->amount;
                }
                break;
            }
            if ((self->flags & 0x80000000) == 0 &&
                self->f10 <= lbl_804DA3C0) {
                PossessionEffectWhiteOut(a, m->kind);
            }
        }
        result = 1;
        break;
    }
    case 0x41d: {
        PossessFlagMsg* m = (PossessFlagMsg*)data;
        a = m->actor;
        self = a->target;
        switch (m->hash) {
        case -0x233c7b3f:
            PossessionEffectWhiteOut(a, self->a0c);
            result = 1;
            break;
        case -0x203a3a46:
            if (m->value != 0) {
                self->flags |= 0x10000000;
            } else {
                self->flags &= 0xefffffff;
            }
            result = 1;
            break;
        case 0x60b3a4ab:
            self->f58 = (f32)m->value;
            result = 1;
            break;
        case 0x6ab33cb:
            if (m->value != 0) {
                self->flags |= 0x40000000;
            } else {
                self->flags &= 0xbfffffff;
            }
            result = 0;
            break;
        }
        break;
    }
    case 0x41e: {
        PossessQueryMsg* m = (PossessQueryMsg*)data;
        a = m->actor;
        self = a->target;
        if (m->hash == 0x6ab33cb) {
            m->out = self->flags & 0x40000000;
            result = 1;
        } else {
            result = 0;
        }
        break;
    }
    case 0xc07: {
        PossessTeleportMsg* m = (PossessTeleportMsg*)data;
        a = m->actor;
        self = a->target;
        if (m->flags & 1) {
            self->f5c = (f32)m->x;
            self->f60 = (f32)m->y;
            self->f64 = (f32)m->z;
            self->f68 = (f32)m->x;
            self->f6c = (f32)m->y;
            self->f70 = (f32)m->z;
        }
        result = 0;
        break;
    }
    case 0xc14: {
        PossessRenderMsg* m = (PossessRenderMsg*)data;
        a = m->actor;
        self = a->target;
        switch (m->hash) {
        case 0x672f35c2:
            delta.x = self->f5c - self->f68;
            delta.y = self->f60 - self->f6c;
            delta.z = self->f64 - self->f70;
            fn_800E1098(&delta, &delta, (f32)a->x60);
            self->f68 -= delta.x;
            self->f6c -= delta.y;
            self->f70 -= delta.z;
            self->f5c -= delta.x;
            self->f60 -= delta.y;
            self->f64 -= delta.z;
            a->x24 -= (s32)delta.x;
            a->x28 -= (s32)delta.y;
            a->x2c -= (s32)delta.z;
            ActorGetMatrix(&mtx, a);
            ActorSetPlacement(a, (PlaceF*)&mtx);
            result = 0;
            break;
        case -0x30fe9b1e:
            if ((self->flags & 0x40000000) == 0) {
                self->flags |= 0x40000000;
                self->a34 = m->a08;
            }
            result = 1;
            break;
        }
        break;
    }
    }

    if (result == -999999) {
        return 0;
    }
    if (result != 0) {
        return result;
    }
    return PossessableMsg(actor, msg, data, arg);
}

// ---- white-out effect -------------------------------------------------------

// Player object returned by the actor's 0x400 query.
struct WhiteOutPlayer {
    u8  unk00[0x248];            // 0x00
    s32 a248;                    // 0x248
    u8  unk24c[0x14cc - 0x24c];  // 0x24c
    s32 font;                    // 0x14cc
    u8  unk14d0[0x264c - 0x14d0];// 0x14d0
    f32 f264c;                   // 0x264c
    u8  unk2650[0x26e0 - 0x2650];// 0x2650
    u8  ghost[0x10];             // 0x26e0
};

extern f32 lbl_804DA408;
extern f32 lbl_804DA40C;
extern f32 lbl_804DA410;

void FontRelease(int font);
int fn_8007EF48(int a, int b, const char* name, int c, int d, int e, int f,
                int g, int h, void* owner);
void GhostNotify(void* ghost, int a, int b);
extern "C" void fn_80158C3C(Vec3* pos, u32 hash, int a, int b, int c, int d, int e);
extern "C" u32 fn_800DF5CC(u32 a, u32 b, u32 c);
extern "C" void fn_80122108(u32 a, u32 b, f64 c, f64 d, u32 e, u32 f, f64 g,
                            int h, int i, Vec3* pos, u32 j, int k);

// Plays the "White Out" screen effect for the possessor and records the
// possession end position on the effect target.
void PossessionEffectWhiteOut(Actor* actor, u32 id) {
    EffTarget* self = actor->target;
    WhiteOutPlayer* player;
    Actor* obj;
    Vec3 pos;
    s32 handle;
    s32 out;

    if (self->flags & 0x80000000) {
        return;
    }

    handle = self->a0c;
    if (handle != -1) {
        id = handle;
        SendMsg(id, 0x400, &player);
        if (player->font != -1) {
            FontRelease(player->font);
            player->font = -1;
        }
        player->font = fn_8007EF48(player->a248, 1, "White Out", 0, 0, 0x3c, 0x3c,
                                   0x3c, 0, player);
        player->f264c = lbl_804DA408;
        GhostNotify(player->ghost, 0, 1);
        self->f10 = lbl_804DA3C0;
    }

    self->flags |= 0x10;
    pos.x = (f32)actor->x24;
    pos.y = (f32)actor->x28;
    pos.z = (f32)actor->x2c;
    fn_80158C3C(&pos, 0xc4f86cb4, -1, -1, -1, -1, -1);

    out = 0;
    if (SendMsg(id, 0x400, &out) != 0 && out != 0) {
        obj = (Actor*)out;
    } else {
        obj = actor;
    }

    {
        u32 h0 = fn_800DF5CC(0x866f7eab, 0xdb3b5046, 0x88ff8a1a);
        u32 h1 = fn_800DF5CC(0x866f7eab, 0xdb3b5046, 0x1727c1bf);
        u32 h2 = fn_800DF5CC(0x866f7eab, 0xdb3b5046, 0xcf962098);
        u32 h3 = fn_800DF5CC(0x866f7eab, 0xdb3b5046, 0xa7355386);

        fn_80122108(h0, h1, lbl_804DA40C, lbl_804DA40C, h2, h3, lbl_804DA3CC,
                    0, obj->a38, &pos, 0xb9fc42f5, actor->a38);
    }

    actor->ac |= 0x20000000;
    if ((self->flags & 0x20000000) == 0) {
        self->f74 = lbl_804DA410;
    } else {
        s32 v = 0;
        SendMsg(actor->a38, 0xc00, &v);
    }
    self->flags |= 0x80000000;
}

extern u32 lbl_804D5558;
extern u32 lbl_804D555C;
extern u32 lbl_804D5560;
extern u32 lbl_804D5564;
extern u32 lbl_804D5568;
extern u32 lbl_804D556C;
extern u32 lbl_804D5570;
extern u32 lbl_804D5580;
extern u32 lbl_804D5584;
extern u32 lbl_804D5588;
extern u32 lbl_804D558C;
extern u32 lbl_804D5590;
extern u32 lbl_804D55D0;
extern u32 lbl_804D55D4;
extern u32 lbl_804D55D8;
extern u32 lbl_804D55E4;
extern u32 lbl_804D55E8;
extern u32 lbl_804D55A0;
extern u32 lbl_804D55A4;
extern u32 lbl_804D55A8;
extern u32 lbl_804D55B8;
extern u32 lbl_804D55BC;
extern u32 lbl_804D55C0;
extern u32 lbl_804D5550;
extern u32 lbl_804D5554;
extern u32 lbl_804D5574;
extern u32 lbl_804D557C;
extern u32 lbl_804D559C;
extern u32 lbl_804D55B4;
extern u32 lbl_804D55CC;
extern u32 lbl_804D55EC;
extern u32 lbl_804A3278[4];

void VFree(void* ptr);
extern "C" void fn_8000F60C(void* handle);
extern "C" void fn_801ED838(int enable);
extern "C" void fn_800C6900(void* handle);
extern "C" void fn_800C7B14(Actor* actor);
u32 Fn800DF3B8(u32 asset, int index, u32 hash);
void AnimUnref(int id);

// Releases the white-out effect assets created by this unit. Called by other
// units under its placeholder name, so it keeps C linkage.
extern "C" void fn_8027E76C(void) {
    if (lbl_804D5558 != 0) {
        VFree((void*)lbl_804D5558);
        lbl_804D5558 = 0;
        lbl_804D555C = 0;
        lbl_804D5560 = 0;
    }
    if (lbl_804D5564 != 0) {
        VFree((void*)lbl_804D5564);
        lbl_804D5564 = 0;
        lbl_804D5568 = 0;
    }
    if (lbl_804D556C != 0) {
        VFree((void*)lbl_804D556C);
        lbl_804D556C = 0;
        lbl_804D5570 = 0;
    }
    if (lbl_804D5580 != 0) {
        VFree((void*)lbl_804D5580);
        lbl_804D5580 = 0;
        lbl_804D5584 = 0;
        lbl_804D5588 = 0;
    }
    if (lbl_804D558C != 0) {
        VFree((void*)lbl_804D558C);
        lbl_804D558C = 0;
        lbl_804D5590 = 0;
    }
    if (lbl_804D55D0 != 0) {
        VFree((void*)lbl_804D55D0);
        lbl_804D55D0 = 0;
        lbl_804D55D4 = 0;
        lbl_804D55D8 = 0;
    }
    if (lbl_804D55E4 != 0) {
        VFree((void*)lbl_804D55E4);
        lbl_804D55E4 = 0;
        lbl_804D55E8 = 0;
    }
    if (lbl_804D55A0 != 0) {
        VFree((void*)lbl_804D55A0);
        lbl_804D55A0 = 0;
        lbl_804D55A4 = 0;
        lbl_804D55A8 = 0;
    }
    if (lbl_804D55B8 != 0) {
        VFree((void*)lbl_804D55B8);
        lbl_804D55B8 = 0;
        lbl_804D55BC = 0;
        lbl_804D55C0 = 0;
    }
    fn_8000F60C((void*)AssetFind(0xf2e2beb8, 0x3fd6348d));
    fn_8000F60C((void*)AssetFind(0xf2e2beb8, 0x1651d499));
    fn_8000F60C((void*)AssetFind(0xf2e2beb8, 0xd141d32e));
    fn_8000F60C((void*)AssetFind(0xf2e2beb8, 0x66979e2c));
}

// WhiteOutMaskReset: clears the mask-compositing state used by the "White Out"
// screen effect (the layer buffers and the cached region sizes). Called from
// another unit under its placeholder name, so it keeps C linkage.
extern "C" void fn_8027E8F8(void) {
    lbl_804D5558 = 0;
    lbl_804D555C = 0;
    lbl_804D5560 = 0;
    lbl_804D5564 = 0;
    lbl_804D5568 = 0;
    lbl_804D556C = 0;
    lbl_804D5570 = 0;
    lbl_804D5580 = 0;
    lbl_804D5584 = 0;
    lbl_804D5588 = 0;
    lbl_804D55A0 = 0;
    lbl_804D55A4 = 0;
    lbl_804D55A8 = 0;
    lbl_804D55B8 = 0;
    lbl_804D55BC = 0;
    lbl_804D55C0 = 0;
    lbl_804D55D0 = 0;
    lbl_804D55D4 = 0;
    lbl_804D55D8 = 0;
    lbl_804D55E4 = 0;
    lbl_804D55E8 = 0;
}

// Message payload for the mask-compositing opcode (0x7104): a destination
// buffer whose 8x8 mask blocks get the pending mask layer OR-ed in.
struct MaskCompositeMsg {
    u32 unk00;               // 0x00
    u8* base;                // 0x04
};

u32 WhiteOutMaskLoad(void* data);
u32 WhiteOutMaskSave(void* data);
extern "C" int fn_800DE2F8(int a, int b);
extern "C" short fn_800259C8(u32 a, u32 b, int c);

// Message handler for the "White Out" mask effect. Opcode 0x7104 composites
// the pending mask layer into the message's buffer and records the region
// size; 0x7100 and 0x7101 are forwarded to the sibling handlers.
u32 WhiteOutMaskMsg(u32 self, int msg, void* data) {
    switch (msg) {
    case 0x7100:
        return WhiteOutMaskSave(data);
    case 0x7101:
        return WhiteOutMaskLoad(data);
    case 0x7104: {
        MaskCompositeMsg* m = (MaskCompositeMsg*)data;
        int count = (s32)(lbl_804D55BC + 0x1f) / 32;
        int idx;
        short s0;
        short s1;
        short s2;
        short s3;
        short s4;

        if (lbl_804D55B8 == 0) {
            return 1;
        }
        if (count > 0) {
            u32* dst = (u32*)(m->base + 0x1c);
            u32* src = (u32*)lbl_804D55B8;
            int i;
            for (i = 0; i < count; i++) {
                dst[i] |= src[i];
            }
        }
        idx = fn_800DE2F8(5, 0);
        *(short*)(m->base + idx * 2 + 0x24) =
            (short)lbl_804D5560 + (short)lbl_804D5588 + (short)lbl_804D55A8 +
            (short)lbl_804D55C0 + (short)lbl_804D55D8;
        s0 = fn_800259C8(lbl_804D5580, lbl_804D5584, 1);
        s1 = fn_800259C8(lbl_804D5558, lbl_804D555C, 1);
        s2 = fn_800259C8(lbl_804D55A0, lbl_804D55A4, 1);
        s3 = fn_800259C8(lbl_804D55B8, lbl_804D55BC, 1);
        s4 = fn_800259C8(lbl_804D55D0, lbl_804D55D4, 1);
        *(short*)(m->base + idx * 2 + 0x2a) = s1 + s0 + s2 + s3 + s4;
        return 1;
    }
    }
    return 0;
}

// Save-stream cursor used by the white-out mask serialisation handlers.
struct SaveStream {
    u8  unk00[0x04];         // 0x00
    s32 remaining;           // 0x04
    u8* cur;                 // 0x08
    s32 pos;                 // 0x0c
    s32 status;              // 0x10
};

extern const char* lbl_804D378C;
extern u8 __upper_map[];

// Checksum of the save-block name, folding each character through the upper-case
// map. An empty name hashes to zero.
static inline u32 WhiteOutNameHash(const char* name) {
    u32 h = 0;
    if (name != 0) {
        const char* p = name;
        while (*p != 0) {
            u8 c = (u8)*p;
            u32 v;
            if ((signed char)c == -1) {
                v = 0xffffffff;
            } else {
                v = __upper_map[c];
            }
            h = h * 31 + (v & 0xff);
            p++;
        }
        h = h % 0xffffffff;
    }
    return h;
}

// Reads the white-out mask layers back from a save stream: verifies the stream
// checksum and the 0x103 block header, then copies each layer into its buffer.
u32 WhiteOutMaskLoad(void* data) {
    SaveStream* s = (SaveStream*)data;
    s32 pos = s->pos;
    u32 hash;
    u32* p;
    u32* hdr;
    u32 s0;
    u32 n;
    int ok;

    if (pos < 0x2c) {
        return 1;
    }
    if (pos < 4) {
        ok = 0;
    } else {
        p = (u32*)s->cur;
        s->pos = pos + 4;
        s->remaining -= 4;
        s->cur = (u8*)(p + 1);
        hash = WhiteOutNameHash(lbl_804D378C);
        ok = (*p == hash);
    }
    if (!ok) {
        return 1;
    }
    hdr = (u32*)s->cur;
    s->pos += 0x28;
    s->remaining -= 0x28;
    s->cur = (u8*)(hdr + 10);
    if (hdr[0] != 0x103) {
        return 1;
    }
    s0 = hdr[1];
    if (s0 >> 2 == (u32)((s32)(lbl_804D555C + 0x1f) / 32) &&
        hdr[2] >> 2 == (u32)((s32)(lbl_804D5568 + 0x1f) / 32) &&
        hdr[3] >> 2 == (u32)((s32)(lbl_804D5570 + 0x1f) / 32) &&
        hdr[4] >> 2 == (u32)((s32)(lbl_804D5584 + 0x1f) / 32) &&
        hdr[5] >> 2 == (u32)((s32)(lbl_804D5590 + 0x1f) / 32) &&
        hdr[6] >> 2 == (u32)((s32)(lbl_804D55D4 + 0x1f) / 32) &&
        hdr[7] >> 2 == (u32)((s32)(lbl_804D55E8 + 0x1f) / 32) &&
        hdr[8] >> 2 == (u32)((s32)(lbl_804D55A4 + 0x1f) / 32) &&
        hdr[9] >> 2 == (u32)((s32)(lbl_804D55BC + 0x1f) / 32)) {
        memcpy((void*)lbl_804D5558, s->cur, s0);
        s->pos -= s0;
        s->cur += s0;
        n = hdr[2];
        memcpy((void*)lbl_804D5564, s->cur, n);
        s->pos -= n;
        s->cur += n;
        n = hdr[3];
        memcpy((void*)lbl_804D556C, s->cur, n);
        s->pos -= n;
        s->cur += n;
        n = hdr[4];
        memcpy((void*)lbl_804D5580, s->cur, n);
        s->pos -= n;
        s->cur += n;
        n = hdr[5];
        memcpy((void*)lbl_804D558C, s->cur, n);
        s->pos -= n;
        s->cur += n;
        n = hdr[6];
        memcpy((void*)lbl_804D55D0, s->cur, n);
        s->pos -= n;
        s->cur += n;
        n = hdr[7];
        memcpy((void*)lbl_804D55E4, s->cur, n);
        s->pos -= n;
        s->cur += n;
        n = hdr[8];
        memcpy((void*)lbl_804D55A0, s->cur, n);
        s->pos -= n;
        s->cur += n;
        n = hdr[9];
        memcpy((void*)lbl_804D55B8, s->cur, n);
        s->pos -= n;
        s->cur += n;
    }
    return 1;
}

// Writes the white-out mask layers (checksum, 0x103 block header with the layer
// sizes, then each layer) into a save stream. Refuses to write when the stream
// does not have room for the whole block.
u32 WhiteOutMaskSave(void* data) {
    SaveStream* s = (SaveStream*)data;
    u32 hash;
    u32* hdr;
    u32 n;

    if (s->remaining <
        ((s32)(lbl_804D555C + 0x1f) / 32) * 4 + 0x2c +
        ((s32)(lbl_804D5568 + 0x1f) / 32) * 4 +
        ((s32)(lbl_804D5570 + 0x1f) / 32) * 4 +
        ((s32)(lbl_804D5584 + 0x1f) / 32) * 4 +
        ((s32)(lbl_804D5590 + 0x1f) / 32) * 4 +
        ((s32)(lbl_804D55D4 + 0x1f) / 32) * 4 +
        ((s32)(lbl_804D55E8 + 0x1f) / 32) * 4 +
        ((s32)(lbl_804D55A4 + 0x1f) / 32) * 4 +
        ((s32)(lbl_804D55BC + 0x1f) / 32) * 4) {
        s->status = 0;
        return 0;
    }
    hash = WhiteOutNameHash(lbl_804D378C);
    memcpy(s->cur, &hash, 4);
    s->pos += 4;
    s->remaining -= 4;
    s->cur += 4;
    hdr = (u32*)s->cur;
    s->pos += 0x28;
    s->remaining -= 0x28;
    s->cur += 0x28;
    hdr[0] = 0x103;
    hdr[1] = ((s32)(lbl_804D555C + 0x1f) / 32) * 4;
    hdr[2] = ((s32)(lbl_804D5568 + 0x1f) / 32) * 4;
    hdr[3] = ((s32)(lbl_804D5570 + 0x1f) / 32) * 4;
    hdr[4] = ((s32)(lbl_804D5584 + 0x1f) / 32) * 4;
    hdr[5] = ((s32)(lbl_804D5590 + 0x1f) / 32) * 4;
    hdr[6] = ((s32)(lbl_804D55D4 + 0x1f) / 32) * 4;
    hdr[7] = ((s32)(lbl_804D55E8 + 0x1f) / 32) * 4;
    hdr[8] = ((s32)(lbl_804D55A4 + 0x1f) / 32) * 4;
    hdr[9] = ((s32)(lbl_804D55BC + 0x1f) / 32) * 4;
    n = hdr[1];
    memcpy(s->cur, (void*)lbl_804D5558, n);
    s->pos += n;
    s->remaining -= n;
    s->cur += n;
    n = hdr[2];
    memcpy(s->cur, (void*)lbl_804D5564, n);
    s->pos += n;
    s->remaining -= n;
    s->cur += n;
    n = hdr[3];
    memcpy(s->cur, (void*)lbl_804D556C, n);
    s->pos += n;
    s->remaining -= n;
    s->cur += n;
    n = hdr[4];
    memcpy(s->cur, (void*)lbl_804D5580, n);
    s->pos += n;
    s->remaining -= n;
    s->cur += n;
    n = hdr[5];
    memcpy(s->cur, (void*)lbl_804D558C, n);
    s->pos += n;
    s->remaining -= n;
    s->cur += n;
    n = hdr[6];
    memcpy(s->cur, (void*)lbl_804D55D0, n);
    s->pos += n;
    s->remaining -= n;
    s->cur += n;
    n = hdr[7];
    memcpy(s->cur, (void*)lbl_804D55E4, n);
    s->pos += n;
    s->remaining -= n;
    s->cur += n;
    n = hdr[8];
    memcpy(s->cur, (void*)lbl_804D55A0, n);
    s->pos += n;
    s->remaining -= n;
    s->cur += n;
    n = hdr[9];
    memcpy(s->cur, (void*)lbl_804D55B8, n);
    s->pos += n;
    s->remaining -= n;
    s->cur += n;
    return 1;
}

// Releases every white-out resource this unit still holds: the four mask
// layers (freeing each allocated sub-object, unrefing its animation and
// releasing its actor), the two effect assets and the cached effect object.
void WhiteOutShutdown(void) {
    u32 reply0[2];
    u32 reply1[2];
    u32 reply2[2];
    u32 reply3[2];
    int i;
    u32 obj;

    lbl_804D5554 = 0;
    fn_801ED838(1);
    fn_8000F60C((void*)lbl_804A3278[1]);
    fn_8000F60C((void*)lbl_804A3278[0]);
    fn_8000F60C((void*)lbl_804A3278[2]);
    fn_8000F60C((void*)lbl_804A3278[3]);
    lbl_804D5550 = 0;

    if (lbl_804D5558 != 0) {
        for (i = 0; i < (s32)lbl_804D555C; i++) {
            fn_8000F60C((void*)Fn800DF3B8(0x5e130113, i, 0x27873a));
        }
        obj = lbl_804D557C;
        if (obj != 0) {
            reply0[1] = 0;
            SendMsg((int)obj, 6, reply0);
            AnimUnref((int)Fn800DF3B8(0x5e130113, lbl_804D5574, 0x6f9e1166));
            fn_800C7B14((Actor*)obj);
            ActorRender((Actor*)obj);
        }
        lbl_804D557C = 0;
    }

    if (lbl_804D5580 != 0) {
        for (i = 0; i < (s32)lbl_804D5584; i++) {
            fn_8000F60C((void*)Fn800DF3B8(0x80edd05c, i, 0x27873a));
        }
        obj = lbl_804D559C;
        if (obj != 0) {
            reply1[1] = 0;
            SendMsg((int)obj, 6, reply1);
            fn_800C7B14((Actor*)obj);
            ActorRender((Actor*)obj);
        }
        lbl_804D559C = 0;
    }

    if (lbl_804D55A0 != 0) {
        for (i = 0; i < (s32)lbl_804D55A4; i++) {
            fn_8000F60C((void*)Fn800DF3B8(0x258124ce, i, 0x27873a));
        }
        obj = lbl_804D55B4;
        if (obj != 0) {
            reply2[1] = 0;
            SendMsg((int)obj, 6, reply2);
            fn_800C7B14((Actor*)obj);
            ActorRender((Actor*)obj);
        }
        lbl_804D55B4 = 0;
    }

    if (lbl_804D55B8 != 0) {
        for (i = 0; i < (s32)lbl_804D55BC; i++) {
            fn_8000F60C((void*)Fn800DF3B8(0xd8e68ab7, i, 0x27873a));
        }
        obj = lbl_804D55CC;
        if (obj != 0) {
            reply3[1] = 0;
            SendMsg((int)obj, 6, reply3);
            fn_800C7B14((Actor*)obj);
            ActorRender((Actor*)obj);
        }
        lbl_804D55CC = 0;
    }

    fn_8000F60C((void*)AssetFind(0xf2e2beb8, 0x52be59c3));
    fn_8000F60C((void*)AssetFind(0xf2e2beb8, 0x6983cad1));
    fn_800C6900((void*)lbl_804D55EC);
}

// Non-zero while a white-out shutdown has been requested. The possession
// effect keeps rendering the actor until the shutdown completes.
u32 WhiteOutEffectActive(void) {
    return lbl_804D5554;
}

// ---- white-out mask layer setup --------------------------------------------

extern "C" void fn_8000F7E4(u32 handle, int flag);
extern "C" void fn_8000F874(u32 handle);
extern "C" u32 fn_800C6AE8(void* owner, int a, int b, int c);
extern "C" u32 fn_8014FF10(const char* name);

int SaveCommand(int slot, int mode);
int SaveGetBestLevel(int slot);
const char* SaveGetSlotName(int slot);
void* VarObjData(u32 asset, int index);
int GameEvent(int a, int b);

extern u32 lbl_8034B0D8[];
extern u8  lbl_804A3218[];

extern u32 lbl_804D55F0;
extern u32 lbl_804D55F4;
extern u32 lbl_804D55F8;
extern u32 lbl_804D55FC;
extern u32 lbl_804D5578;
extern u32 lbl_804D5594;
extern u32 lbl_804D5598;
extern u32 lbl_804D55AC;
extern u32 lbl_804D55B0;
extern u32 lbl_804D55C4;
extern u32 lbl_804D55C8;
extern u32 lbl_804D55DC;
extern u32 lbl_804D55E0;

// Walks a mask layer from its last element down to zero. Elements that are
// still missing (bit clear and no object data yet) are resolved through the
// level table; the table's level id is returned, -1 when nothing was found.
static inline int WhiteOutLayerScan(u32* mask, int count, u32 asset, u32 hash) {
    int found;
    int i;
    int j;
    char* data;
    int level;

    for (i = count; i >= 0; i--) {
        if (mask[i / 32] & (1 << (i & 31))) {
            found = 1;
        } else if (found == 1) {
            data = (char*)VarObjData(asset, i);
            if (data == 0 || *data == 0) {
                level = GameEvent(0xbaa22e62, Fn800DF3B8(asset, i, hash));
                for (j = 0; j < 13; j++) {
                    if (lbl_8034B0D8[j * 2] == level) {
                        return lbl_8034B0D8[j * 2 + 1];
                    }
                }
            }
        }
    }
    return -1;
}

// Looks for the object whose level id matches this save slot's level and
// records its animation index (index + 1, or -1 while there is no match).
static inline void WhiteOutFindSlotObject(u32 asset, u32 hash, int count,
                                          u32* out) {
    int i;

    for (i = 0; i < count; i++) {
        if (GameEvent(0xbaa22e62, Fn800DF3B8(asset, i, hash)) ==
            lbl_8034B0D8[lbl_804D55FC * 2]) {
            *out = i + 1;
            break;
        } else {
            *out = 0xffffffff;
        }
    }
}

// Builds (or refreshes) every white-out mask layer: frees the layer assets of
// the previous level, scans the remaining layer objects for the highest
// unlocked level and finally resolves each layer's save-slot object.
void WhiteOutInit(void) {
    int i;
    int level;

    lbl_804D5554 = 1;
    fn_801ED838(0);
    lbl_804D55F8 = SaveGetBestLevel(SaveCommand(5, 0));
    lbl_804D55FC = fn_8014FF10(SaveGetSlotName(SaveCommand(5, 0)));
    fn_8000F7E4(lbl_804A3278[1], 1);
    fn_8000F7E4(lbl_804A3278[0], 1);
    fn_8000F7E4(lbl_804A3278[2], 1);
    fn_8000F7E4(lbl_804A3278[3], 1);

    if (lbl_804D5558 != 0) {
        for (i = 0; i < (s32)lbl_804D555C; i++) {
            fn_8000F874(Fn800DF3B8(0x5e130113, i, 0x27873a));
        }
        level = WhiteOutLayerScan((u32*)lbl_804D5558, (s32)lbl_804D555C,
                                  0x5e130113, 0x198917dc);
        if ((s32)lbl_804D55F8 < level) {
            lbl_804D55F8 = level;
        }
    }

    if (lbl_804D5580 != 0) {
        for (i = 0; i < (s32)lbl_804D5584; i++) {
            fn_8000F874(Fn800DF3B8(0x80edd05c, i, 0x27873a));
        }
        level = WhiteOutLayerScan((u32*)lbl_804D5580, (s32)lbl_804D5584,
                                  0x80edd05c, 0x198917dc);
        if ((s32)lbl_804D55F8 < level) {
            lbl_804D55F8 = level;
        }
    }

    if (lbl_804D55D0 != 0) {
        level = WhiteOutLayerScan((u32*)lbl_804D55D0, (s32)lbl_804D55D4,
                                  0x5eb7a22e, 0x273d2d);
        if ((s32)lbl_804D55F8 < level) {
            lbl_804D55F8 = level;
        }
    }

    if (lbl_804D55A0 != 0) {
        for (i = 0; i < (s32)lbl_804D55A4; i++) {
            fn_8000F874(Fn800DF3B8(0x258124ce, i, 0x27873a));
        }
        level = WhiteOutLayerScan((u32*)lbl_804D55A0, (s32)lbl_804D55A4,
                                  0x258124ce, 0x273d2d);
        if ((s32)lbl_804D55F8 < level) {
            lbl_804D55F8 = level;
        }
    }

    if (lbl_804D55B8 != 0) {
        for (i = 0; i < (s32)lbl_804D55BC; i++) {
            fn_8000F874(Fn800DF3B8(0xd8e68ab7, i, 0x27873a));
        }
        level = WhiteOutLayerScan((u32*)lbl_804D55B8, (s32)lbl_804D55BC,
                                  0xd8e68ab7, 0x273d2d);
        if ((s32)lbl_804D55F8 < level) {
            lbl_804D55F8 = level;
        }
    }

    fn_8000F874(AssetFind(0xf2e2beb8, 0x52be59c3));
    fn_8000F874(AssetFind(0xf2e2beb8, 0x6983cad1));
    lbl_804D55EC = fn_800C6AE8(lbl_804A3218, 0, 0, 0);

    lbl_804D55F0 = 0;
    lbl_804D55F4 = 0;
    WhiteOutFindSlotObject(0x5e130113, 0x198917dc, (s32)lbl_804D5560,
                           &lbl_804D5574);
    lbl_804D5578 = 0;
    lbl_804D557C = 0;
    WhiteOutFindSlotObject(0x80edd05c, 0x198917dc, (s32)lbl_804D5584,
                           &lbl_804D5594);
    lbl_804D5598 = 0;
    lbl_804D559C = 0;
    WhiteOutFindSlotObject(0x258124ce, 0x273d2d, (s32)lbl_804D55A4,
                           &lbl_804D55AC);
    lbl_804D55B0 = 0;
    lbl_804D55B4 = 0;
    WhiteOutFindSlotObject(0xd8e68ab7, 0x273d2d, (s32)lbl_804D55BC,
                           &lbl_804D55C4);
    lbl_804D55C8 = 0;
    lbl_804D55CC = 0;
    WhiteOutFindSlotObject(0x5eb8a22e, 0x273d2d, (s32)lbl_804D55D4,
                           &lbl_804D55DC);
    lbl_804D55E0 = 0;
}

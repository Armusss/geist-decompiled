// u_80247EB0.cpp: the "Grenade" actor type (0x80247EB0-0x80248FC8).
// A thrown grenade: it spawns a trail, detonates when its fuse expires or when it comes to rest,
// and drives an explosion emitter plus a HUD "White Out" flash.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

struct Vec4 {
    float v[4];
};

struct Mtx33 {
    float m[3][3];
};

struct Mtx34 {
    float m[3][4];
};

struct IVec3 {
    int x, y, z;
};

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// Colour of the explosion's damage sphere.
static const Vec4 sBlastColor = { 255.0f, 0.0f, 0.0f, 1.0f };

struct Actor;

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

struct ActorType {
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void (*update)(Actor* actor, float dt);
    int (*init)(Actor* actor);
    void* unk14;
    void (*destroy)(Actor* actor);
    void* unk1C[5];
};

// Orientation and integer position, as stored at the start of every actor.
struct Placement {
    Mtx33 rot;
    IVec3 pos;
};

// Orientation and float position, as filled in by fn_801D513C.
struct PlaceF {
    Mtx33 rot;
    Vec3 pos;
};

// Per-actor runtime data (0x64 bytes, allocated by GrenadeInit).
struct GrenadeData {
    char pad0[0xC];
    int handle;         // 0x0C
    float timer;        // 0x10
    char pad14[4];
    u32 flags;          // 0x18
    char pad1C[0x14];
    float fuse;         // 0x30
    int target;         // 0x34
    int fx;             // 0x38
    IVec3 unk3C;        // 0x3C
    IVec3 unk48;        // 0x48
    int trail;          // 0x54
    int emitter;        // 0x58
    int blast;          // 0x5C
    float life;         // 0x60
};

struct Actor {
    Placement place;            // 0x00
    void (*send)(int, void*);   // 0x30
    char pad34[4];              // 0x34
    int handle;                 // 0x38
    char pad3C[0x10];           // 0x3C
    u32 flags4C;                // 0x4C
    char pad50[0x34];           // 0x50
    char unk84[0x20];           // 0x84
    GrenadeData* data;          // 0xA4
    char padA8[4];              // 0xA8
    u32 flagsAC;                // 0xAC
    char padB0[0x30];           // 0xB0
    Mtx34 matrix;               // 0xE0
    char pad110[0xC];           // 0x110
    void* effect;               // 0x11C
};

// A ghost, as far as the "White Out" flash is concerned.
struct Ghost {
    char pad0[0x248];
    int unk248;                 // 0x248
    char pad24C[0x1280];
    int font;                   // 0x14CC
    char pad14D0[0x117C];
    float timer;                // 0x264C
    char pad2650[0x90];
    int notify;                 // 0x26E0
};

// Description passed to fn_800C7E80 to create the grenade actor (0x38 bytes).
struct SpawnDesc {
    Vec3 pos;               // 0x00
    Vec3 rot;               // 0x0C
    char pad18[8];
    u32 type;               // 0x20
    int unk24;              // 0x24
    int unk28;              // 0x28
    char pad2C[0xC];
};

// Trail description passed to TrailCreate (0x3C bytes).
// Blast description (0x40 bytes); set up but not used here.
struct BlastDesc {
    int unk0;
    int owner;
    int asset;
    int unkC[13];
};

struct TrailDesc {
    int unk0;               // 0x00
    int owner;              // 0x04
    int asset;              // 0x08
    int segments;           // 0x0C
    float width;            // 0x10
    float unk14;            // 0x14
    char pad18[8];          // 0x18
    float unk20;            // 0x20
    float unk24;            // 0x24
    float unk28;            // 0x28
    float unk2C;            // 0x2C
    int texture;            // 0x30
    unsigned char color[3]; // 0x34
    char pad37[5];          // 0x37
};

// Particle emitter description (0xEC bytes) passed to EmitterCreate.
struct EmitterDesc {
    Vec3 pos;               // 0x00
    char unkC[0x10];        // 0x0C: a Mtx34 is written here
    int unk1C;              // 0x1C
    int unk20;              // 0x20
    float unk24;            // 0x24
    int unk28;              // 0x28
    int texture;            // 0x2C
    unsigned char unk30;    // 0x30
    char pad31[0x3B];
    float unk6C;
    float unk70;
    float unk74;
    float unk78;
    int unk7C;
    float unk80;
    float unk84;
    int unk88;
    float unk8C;
    float unk90;
    float unk94;
    int unk98;
    int unk9C;
    int unkA0;
    int unkA4;
    int unkA8;
    float unkAC;
    float unkB0;
    float unkB4;
    float unkB8;
    float unkBC;
    float unkC0;
    float unkC4;
    float unkC8;
    unsigned char color[4];
    char padD0[4];
    int unkD4;
    int unkD8;
    int unkDC;
    int unkE0;
    int unkE4;
    char padE8[4];
};

// Message payload seen by the actor's message handler.
struct GrenadeMsgData {
    Actor* actor;           // 0x00
    int unk4;               // 0x04
    int unk8;               // 0x08
    int unkC;               // 0x0C
    char pad10[4];
    int unk14;              // 0x14
};

struct NotifyMsg {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
};

// Out-parameter written by SendMsg for 0x400 (the first word is the target actor).
struct MsgResult {
    char* p;
    char pad[0x1C];
};

struct GrenadeHitMsg {
    Actor* actor;
    Vec3 pos;
    float power;
};

extern "C" {
void* memset(void* dst, int val, u32 n);
int SendMsg(int to, int msg, void* data);                       // 0x800265D4
void fn_8016D68C(Vec3* dst, Vec3* src);                         // 0x8016D68C
Actor* fn_800C7E80(void* desc);                                 // 0x800C7E80
int fn_801D92F0(float, float, float, Actor*, u32, u32, Vec3*, int); // 0x801D92F0
void fn_800A025C(Vec3* out, Mtx34* m, int a);                   // 0x800A025C
int fn_80158BE0(void);                                          // 0x80158BE0
void fn_800B4C50(Vec3* pos, int handle, float a, float b, int c, int d); // 0x800B4C50
void fn_801D513C(void* effect, void* out);                       // 0x801D513C
void fn_80027690(int hud);                                      // 0x80027690
int fn_800E0DBC(IVec3* v);                                      // 0x800E0DBC
void fn_801D55CC(void* effect);                                 // 0x801D55CC
void fn_8016D6A8(Vec3* v);                                      // 0x8016D6A8
void fn_800D3430(int emitter, int a);                           // 0x800D3430
int fn_80027A58(int type, Vec3* pos, Vec4* color, int a, int b); // 0x80027A58
void fn_8009F028(Actor* actor, Vec3* v);                        // 0x8009F028
void fn_8016EEF4(int fx);                                       // 0x8016EEF4
int fn_8016F120(int fx, IVec3* pos);                            // 0x8016F120
void fn_8016EDD8(int fx, IVec3* pos);                           // 0x8016EDD8
void fn_80158C3C(Vec3* pos, u32 id, int a, int b, int c, int d, int e); // 0x80158C3C
void fn_80122108(int a, int b, Vec3* pos, u32 id, float f1, float f2, float f3,
                 float f4, float f5, float f6, float f7, int c); // 0x80122108
int fn_80049940(void);                                          // 0x80049940
void fn_800C7B78(Actor* actor);                                 // 0x800C7B78
}

int AssetFind(u32 bank, u32 name);                               // 0x8007EB34
int TrailCreate(TrailDesc* desc);                                // 0x801DE960
void TrailDestroy(int trail);                                    // 0x801DE904
int EmitterCreate(EmitterDesc* desc);                            // 0x800D3D88
void EmitterDestroy(int handle);                                 // 0x800D3694
void MtxSetEuler(Mtx34* m, Vec3* rot);                           // 0x8009FAC0
void EulerNormalize(Vec3* in, Vec3* out);                        // 0x8016D55C
void BoneSetMatrix(void* bone, Mtx34* m);                        // 0x800A0AC4
void ActorGetMatrix(Mtx34* m, Actor* a);                         // 0x8009ED70
void ActorRender(Actor* a);                                      // 0x800C7CE8
void GhostNotify(void* ghost, int a, int b);                     // 0x80154594
void ActorRefreshHostState(Actor* actor, int a);                // 0x80022B1C
int ActorVisibleFlag(Actor* actor);                              // 0x80022B10
void VecToAngles(IVec3* out, IVec3* in);                         // 0x8016D278
int fn_8007EF48(int a, int b, const char* text, int c, int d, int e, int f, int g, int h,
                int ghost);                                      // 0x8007EF48
void FontRelease(int font);                                      // 0x8007C8A4
float VarFindFloat(u32 a, u32 b, u32 key);                       // 0x800E4FC8
void* VAlloc(int size);                                          // 0x8007D4E0
int PossessableInit(Actor* a);                                   // 0x8020D554
void PossessableUpdate(Actor* a, float dt);                      // 0x8020D750
int PossessableMsg(Actor* a, int msg, void* data, int arg);      // 0x8020D738
void PossessableDestroy(Actor* a);                               // 0x8020D4E0
void* FxFind(const char* name);                                  // 0x8007D??
int Fn800816B0(void);                                            // 0x800816B0

static int sGrenadeHitFx = -1;
static int sGrenadeRollFx = -1;

bool GrenadeIsNull(int* p);
int GrenadeTrue(void);
int GrenadeThrow(int a, Vec3* pos, Vec3* rot, int duration, float power);
int GrenadeSpawn(int target, Vec3* pos, Vec3* rot, int duration, float power);
void GrenadeDestroy(Actor* actor);
void GrenadeUpdate(Actor* actor, float dt);
int GrenadeInit(Actor* actor);
int GrenadeMsg(Actor* actor, int msg, void* data, int arg);
int GrenadeCommand(GrenadeMsgData* data);
void GrenadeDetonate(Actor* actor, int target);

ActorType gGrenadeType = { 0, "Grenade", GrenadeMsg, GrenadeUpdate, GrenadeInit, 0, GrenadeDestroy };

bool GrenadeIsNull(int* p) {
    return *p == 0;
}

int GrenadeTrue(void) {
    return 1;
}

int GrenadeThrow(int a, Vec3* pos, Vec3* rot, int duration, float power) {
    if (duration != 0) {
        return GrenadeSpawn(a, pos, rot, 3000, power);
    }
    return GrenadeSpawn(a, pos, rot, 0, power);
}

int GrenadeSpawn(int target, Vec3* pos, Vec3* rot, int duration, float power) {
    Vec3 zero;
    Vec3 v;
    GrenadeHitMsg msg;
    SpawnDesc desc;
    TrailDesc trail;
    Mtx34 m;
    GrenadeData* data;
    Actor* actor;

    memset(&desc, 0, sizeof(desc));
    desc.type = 0x3BB8992E;
    desc.unk28 = -1;
    desc.pos = *pos;
    fn_8016D68C(&desc.rot, rot);
    actor = fn_800C7E80(&desc);
    if (actor == 0 || (data = actor->data) == 0) {
        return -1;
    }

    data->flags |= 0x20000000;
    data->target = target;
    if (duration != 0) {
        GrenadeData* d = actor->data;
        if (!(d->flags & 0x80000000)) {
            d->flags |= 0x40000000;
            d->fuse = (float)duration;
            d->target = target;
        }
    }

    if (fn_80049940()) {
        data->flags &= ~4;
    } else if ((int)((u32)data->target >> 24) == 2) {
        data->flags &= ~4;
    }

    trail.owner = -1;
    trail.asset = 0;
    trail.segments = 500;
    trail.unk14 = 0.0f;
    trail.unk20 = 1.0f;
    trail.unk24 = 0.0f;
    trail.unk28 = 0.0f;
    trail.unk2C = 0.0f;
    trail.texture = -3;
    trail.color[0] = 125;
    trail.color[1] = 125;
    trail.color[2] = 125;
    trail.owner = actor->handle;
    trail.width = 30.0f;
    trail.asset = AssetFind(0xF1754BF2, 0x3BB8992E);
    data->trail = TrailCreate(&trail);

    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    actor->effect = (void*)fn_801D92F0(25.0f, 25.0f, 25.0f, actor, 0x922B5BCD, 0x04B8D9AB, &zero, 0);
    if (power > 1.1920929e-07f) {
        msg.power = power;
        MtxSetEuler(&m, rot);
        fn_800A025C(&v, &m, 1);
        msg.pos = v;
        msg.actor = actor;
        actor->send(0x7405, &msg);
    }
    return actor->handle;
}

void GrenadeDestroy(Actor* actor) {
    ActorRefreshHostState(actor, -1);
    PossessableDestroy(actor);
}

void GrenadeUpdate(Actor* actor, float dt) {
    GrenadeData* data = actor->data;
    PlaceF xform;
    BlastDesc x;
    Mtx34 m;
    MsgResult gres;
    MsgResult res;
    Vec3 fpos;
    IVec3 delta;
    Vec3 pos;
    Vec3 rot;
    Vec4 color;
    Vec3 v;
    EmitterDesc ed;

    if (!(data->flags & 0x80000000)) {

    if (data->flags & 0x40000000) {
        if (!fn_80158BE0() && (actor->flags4C & 4) && Fn800816B0() <= -1) {
            data->fuse -= dt;
        }
        if (data->fuse <= 0.0f) {
            int handle = data->handle;
            if (handle != -1 && (int)((u32)handle >> 24) == 2) {
                Ghost* ghost;
                SendMsg(handle, 0x400, &gres);
                ghost = (Ghost*)gres.p;
                if (ghost->font != -1) {
                    FontRelease(ghost->font);
                    ghost->font = -1;
                }
                ghost->font = fn_8007EF48(ghost->unk248, 1, "White Out", 0, 0, 60, 60, 60, 0, (int)ghost);
                ghost->timer = 60.0f;
                GhostNotify(&ghost->notify, 0, 1);
            }
            fpos.x = (float)actor->place.pos.x;
            fpos.y = (float)actor->place.pos.y;
            fpos.z = (float)actor->place.pos.z;
            fn_800B4C50(&fpos, actor->handle, 201.0f, 8000.0f, 2, 0x12);
            data->timer = 0.0f;
            GrenadeDetonate(actor, data->target);
            return;
        }
    }

    if (actor->effect != 0) {
        fn_801D513C(actor->effect, &xform);
        if (ActorVisibleFlag(actor) == 0) {
            data->flags |= 0x80000000;
            if (data->trail != -1) {
                TrailDestroy(data->trail);
                data->trail = -1;
            }
            if (data->emitter != -1) {
                EmitterDestroy(data->emitter);
                data->emitter = -1;
            }
            if (data->blast != -1) {
                fn_80027690(data->blast);
                data->blast = -1;
            }
        }

        delta.x = data->unk48.x - actor->place.pos.x;
        delta.y = data->unk48.y - actor->place.pos.y;
        delta.z = data->unk48.z - actor->place.pos.z;
        if (fn_800E0DBC(&delta) == 0) {
            Actor* hit;

            fn_801D55CC(actor->effect);
            actor->effect = 0;

            pos.x = (float)actor->place.pos.x;
            pos.y = (float)actor->place.pos.y;
            pos.z = (float)actor->place.pos.z;
            memset(&x, 0, sizeof(x));

            fn_8016D6A8(&rot);
            rot.x = 180.0f;
            memset(&ed, 0, sizeof(ed));
            ed.pos = pos;
            EulerNormalize(&rot, &rot);
            MtxSetEuler(&m, &rot);
            BoneSetMatrix(&ed.unkC, &m);

            ed.unk1C = 1;
            ed.unk20 = 2;
            ed.unk24 = 1.0f;
            ed.texture = AssetFind(0xF2E2BEB8, 0xF4D23CAF);
            ed.unk28 = 1;
            ed.unk30 = 0xFF;
            ed.unkD4 = -1;
            ed.unkD8 = -1;
            ed.unkDC = 0;
            ed.unkE0 = actor->handle;
            ed.unkE4 = -1;
            x.owner = actor->handle;
            x.asset = -1;
            ed.color[0] = 255;
            ed.color[1] = 255;
            ed.color[2] = 255;
            ed.color[3] = 255;
            ed.unk6C = 0.0f;
            ed.unk70 = 0.0f;
            ed.unk74 = 1.0f;
            ed.unk78 = 2.0f;
            ed.unk7C = 0x28;
            ed.unk80 = 0.0f;
            ed.unk84 = 1.0f;
            ed.unk88 = 0x10;
            ed.unk8C = 0.0f;
            ed.unk90 = 3.0f;
            ed.unk94 = 504.0f;
            ed.unk98 = 1;
            ed.unk9C = 0;
            ed.unkA0 = 0;
            ed.unkA4 = 0;
            ed.unkA8 = 0;
            ed.unkAC = 9.0f;
            ed.unkB0 = 30.0f;
            ed.unkB4 = 0.0f;
            ed.unkB8 = 3.0f;
            ed.unkBC = 0.25f;
            ed.unkC0 = 0.0f;
            ed.unkC4 = 0.0f;
            ed.unkC8 = 3.0f;
            data->emitter = EmitterCreate(&ed);
            fn_800D3430(data->emitter, 1);

            color = sBlastColor;
            pos.z += 5.0f;
            data->blast = fn_80027A58(1, &pos, &color, 0x96, 9);

            if (SendMsg(data->target, 0x400, &res) && res.p != 0) {
                hit = (Actor*)res.p;
                delta.x = hit->place.pos.x - actor->place.pos.x;
                delta.y = hit->place.pos.y - actor->place.pos.y;
                delta.z = hit->place.pos.z - actor->place.pos.z;
                VecToAngles((IVec3*)&v, &delta);
                fn_8009F028(actor, &v);
            }
            actor->place.pos.x = (int)xform.pos.x;
            actor->place.pos.y = (int)xform.pos.y;
            actor->place.pos.z = (int)xform.pos.z;
            ActorGetMatrix(&actor->matrix, actor);
            actor->flagsAC |= 0x800000;
        }

        data->unk48 = data->unk3C;
        data->unk3C = actor->place.pos;
    }

    if (data->fx != -1) {
        fn_8016EDD8(data->fx, &actor->place.pos);
    }
    } else {
        if (data->flags & 0x20000000) {
            ActorRender(actor);
            return;
        }
        if (data->life) {
            if (data->flags & 0x10000000) {
                data->life -= dt;
                if (data->life < 0.0f) {
                    data->life = 0.0f;
                }
            }
        } else {
            fn_800C7B78(actor);
        }
    }
    PossessableUpdate(actor, dt);
}

int GrenadeInit(Actor* actor) {
    GrenadeData* data;

    if (actor->data == 0) {
        data = (GrenadeData*)VAlloc(100);
        actor->data = data;
    } else {
        data = actor->data;
    }
    memset(data, 0, 100);
    data->fx = -1;
    data->blast = -1;
    data->emitter = -1;
    data->trail = -1;
    if (PossessableInit(actor) == 0) {
        actor->flagsAC |= 0x1000000;
        return 0;
    }
    actor->flagsAC &= ~0x20000000;
    sGrenadeHitFx = (int)FxFind("GrenadeHit");
    sGrenadeRollFx = (int)FxFind("GrenadeRoll");
    data->flags |= 0x10000000;
    data->flags &= ~8;
    ActorRefreshHostState(actor, 1);
    return 1;
}

int GrenadeMsg(Actor* actor, int msg, void* data_, int arg) {
    Actor* self = *(Actor**)data_;
    GrenadeData* info = self->data;
    GrenadeMsgData* data = (GrenadeMsgData*)data_;
    int result = 0;

    switch (msg) {
    case 0x407: {
        int r;
        if ((int)((u32)data->unk8 >> 24) != 2
            && (int)((u32)data->unk8 >> 24) != 1) {
            r = 1;
        } else {
            switch (data->unk14) {
            case (int)0xB9FC42F5:
                info->timer = 0.0f;
                break;
            }
            if (!(info->flags & 0x80000000) && info->timer <= 0.0f) {
                GrenadeDetonate(self, data->unk8);
            }
            r = 1;
        }
        result = r;
        break;
    }
    case 0x40A:
        if (fn_80049940() || info->target == ((GrenadeMsgData*)data_)->unk4) {
            result = -999999;
        } else {
            result = 0;
        }
        break;
    case 0xC14: {
        int r;
        switch (data->unkC) {
        case 0xCF0164E2: {
            int target = data->unk8;
            if (!(info->flags & 0x80000000)) {
                info->flags |= 0x40000000;
                info->fuse = 0.0f;
                info->target = target;
            }
            r = 1;
            break;
        }
        case 0x672F35C2:
            fn_801D55CC(self->effect);
            self->effect = 0;
            if (info->trail != -1) {
                TrailDestroy(info->trail);
                info->trail = -1;
            }
            if (info->emitter != -1) {
                EmitterDestroy(info->emitter);
                info->emitter = -1;
            }
            if (info->blast != -1) {
                fn_80027690(info->blast);
                info->blast = -1;
            }
            r = 0;
            break;
        case 0x237655D3:
            if (self->effect == 0) {
                Vec3 zero;
                zero.x = 0.0f;
                zero.y = 0.0f;
                zero.z = 0.0f;
                self->effect = (void*)fn_801D92F0(25.0f, 25.0f, 25.0f, self, 0x922B5BCD, 0x04B8D9AB, &zero, 0);
            }
            r = 0;
            break;
        default:
            r = 0;
            break;
        }
        result = r;
        break;
    }
    case 0x41D:
        result = GrenadeCommand(data);
        break;
    case 0x7404: {
        int r;
        if (info->flags & 0x80000000) {
            r = 0;
        } else {
            switch (data->unk8) {
            case 1:
                fn_8016F120(sGrenadeHitFx, &self->place.pos);
                break;
            case 2:
                if (info->fx == -1) {
                    info->fx = fn_8016F120(sGrenadeRollFx, &self->place.pos);
                }
                break;
            }
            r = 0;
        }
        result = r;
        break;
    }
    case 3:
    case 4:
        return 0;
    }

    if (result == -999999) {
        return 0;
    }
    if (result) {
        return result;
    }
    return PossessableMsg(actor, msg, data_, arg);
}

int GrenadeCommand(GrenadeMsgData* data) {
    Actor* actor = data->actor;
    GrenadeData* info = actor->data;

    switch (data->unk4) {
    case 0xDCC384C1:
        GrenadeDetonate(actor, actor->handle);
        return 1;
    case 0x3ECC2A7C:
        if (data->unkC != 0) {
            NotifyMsg m;
            info->flags |= 0x80000000;
            m.unk4 = 0;
            SendMsg(actor->handle, 0xC00, &m);
            if (info->fx != -1) {
                fn_8016EEF4(info->fx);
                info->fx = -1;
            }
            if (info->trail != -1) {
                TrailDestroy(info->trail);
                info->trail = -1;
            }
            if (info->emitter != -1) {
                EmitterDestroy(info->emitter);
                info->emitter = -1;
            }
            if (info->blast != -1) {
                fn_80027690(info->blast);
                info->blast = -1;
            }
        }
        return 1;
    case 0xDFC5C5BA:
        if (data->unkC != 0) {
            info->flags |= 0x10000000;
        } else {
            info->flags &= ~0x10000000;
        }
        return 1;
    }
    return 0;
}

void GrenadeDetonate(Actor* actor, int target) {
    GrenadeData* data = actor->data;
    Vec3 pos;
    NotifyMsg msg;
    MsgResult res;
    Actor* other;
    float a, b, c, d;

    if (data->trail != -1) {
        TrailDestroy(data->trail);
        data->trail = -1;
    }
    if (data->emitter != -1) {
        EmitterDestroy(data->emitter);
        data->emitter = -1;
    }
    if (data->blast != -1) {
        fn_80027690(data->blast);
        data->blast = -1;
    }
    if (data->flags & 0x80000000) {
        return;
    }

    data->flags |= 0x80000000;
    data->flags |= 0x10;
    if (data->fx != -1) {
        fn_8016EEF4(data->fx);
        data->fx = -1;
    }
    pos.x = (float)actor->place.pos.x;
    pos.y = (float)actor->place.pos.y;
    pos.z = (float)actor->place.pos.z;
    fn_80158C3C(&pos, 0x3BB8992E, -1, -1, -1, -1, -1);
    if (SendMsg(target, 0x400, &res) && res.p != 0) {
        other = (Actor*)res.p;
    } else {
        other = actor;
    }
    a = VarFindFloat(0x866F7EAB, 0xA584772C, 0x88FF8A1A);
    b = VarFindFloat(0x866F7EAB, 0xA584772C, 0x1727C1BF);
    c = VarFindFloat(0x866F7EAB, 0xA584772C, 0xCF962098);
    d = VarFindFloat(0x866F7EAB, 0xA584772C, 0xA7355386);
    pos.z += 200.0f;
    fn_80122108(0, other->handle, &pos, 0xB9FC42F5, a, b, 15000.0f, 15000.0f, c, d, 100.0f,
                actor->handle);
    if (actor->effect != 0) {
        fn_801D55CC(actor->effect);
        actor->effect = 0;
    }
    if (data->flags & 0x20000000) {
        msg.unk4 = 0;
        SendMsg(actor->handle, 0xC00, &msg);
    } else {
        data->life = 30000.0f;
    }
}

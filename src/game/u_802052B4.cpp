// u_802052B4.cpp: turret ("gun") actor behaviour (0x802052B4-0x80208238).
// Names are descriptive guesses based on what the code does.

#include <math.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef long s32;

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    s32 x, y, z;
};

// 3x4 float matrix, as produced by ActorGetMatrix.
struct Mtx34 {
    float m[12];
};

// Placement: float position in the last row.
struct PlaceF {
    u8   unk00[0x24];   // 0x00
    Vec3 pos;           // 0x24
};

struct Actor;
struct TurretBeh;

// Actor class/type descriptor.
struct ActorClass {
    s32   typeId;       // 0x00
    u8    unk04[0x14];  // 0x04
    s32   f18;          // 0x18
    s32   f1c;          // 0x1c
    u8    unk20[0x10];  // 0x20
    float f30;          // 0x30
    float f34;          // 0x34
    float f38;          // 0x38
    float f3c;          // 0x3c
    float f40;          // 0x40
    float f44;          // 0x44
    u8    unk48[0x38];  // 0x48
    s32   data[0x20];   // 0x80
};

// Behaviour state of the turret.
struct TurretBeh {
    u8    unk00[0x04];  // 0x00
    s32   f04;          // 0x04
    u8    unk08[0x04];  // 0x08
    s32   field0c;      // 0x0c
    float f10;          // 0x10
    float f14;          // 0x14
    u32   flags;        // 0x18
    u8    unk1c[0x14];  // 0x1c
    s32   mode;         // 0x30
    s32   state;        // 0x34
    float f38;          // 0x38
    float f3c;          // 0x3c
    float f40;          // 0x40
    u32   unk44;        // 0x44
    float f48;          // 0x48
    float f4c;          // 0x4c
    PlaceF place;       // 0x50
    Vec3  rot;          // 0x80
    Vec3  rot2;         // 0x8c
    s32   field98;      // 0x98
    float f9c;          // 0x9c
    float fA0;          // 0xa0
    u32   ownerId;      // 0xa4
    float fA8;          // 0xa8
    float timer;        // 0xac
    s32   fieldB0;      // 0xb0
    s32   fieldB4;      // 0xb4
    s32   fieldB8;      // 0xb8
    s32   emitter1;     // 0xbc
    s32   emitter2;     // 0xc0
    Vec3  beaconPos;    // 0xc4
    Vec3  beaconRot;    // 0xd0
    float beaconF;      // 0xdc
    s32   beaconI;      // 0xe0
    float beaconG;      // 0xe4
    float beaconH;      // 0xe8
    u8    unkec[0x04];  // 0xec
    void* fxHandle;     // 0xf0
    float fxTimer;      // 0xf4
    s32   fxEmitter0;   // 0xf8
    s32   fxEmitter1;   // 0xfc
    u32   lightId;      // 0x100
    float f104;         // 0x104
    s32   disabled;     // 0x108
};

// Actor: fixed-point 3x4 matrix at offset 0; its translation doubles as the
// integer actor position at 0x24.
struct Actor {
    s32         mat[12];  // 0x00
    u8          unk30[0x08];  // 0x30
    u32         id;       // 0x38
    u8          unk3c[0x44];  // 0x3c
    void*       field80;  // 0x80
    ActorClass* cls;      // 0x84
    u8          unk88[0x08];  // 0x88
    s32         model;    // 0x90
    u8          unk94[0x08];  // 0x94
    s32         field9c;  // 0x9c
    u8          unka0[0x04];  // 0xa0
    TurretBeh*  beh;      // 0xa4
    u8          unka8[0x04];  // 0xa8
    u32         fieldac;  // 0xac
    u8          unkb0[0x30];  // 0xb0
    Mtx34       fieldE0;  // 0xe0
    u32         stream;   // 0x110
    u8          unk114[0x08]; // 0x114
    void*       field11c; // 0x11c
    u8          unk120[0x64]; // 0x120
    s32         f184;     // 0x184
};

// Payload of message 0x402: the handler fills in a fixed-point matrix.
struct Msg402 {
    u32 unk00;      // 0x00
    u16 id;         // 0x04
    u8  unk06[0x0a];// 0x06
    s32 mtx[12];    // 0x10
};

// Payload of message 0xC10.
struct MsgC10 {
    u32   unk00;    // 0x00
    s32   id;       // 0x04
    s32   unk08;    // 0x08
    s32   unk0c;    // 0x0c
    s32   unk10;    // 0x10
    void* fn;       // 0x14
    void* owner;    // 0x18
};

// Payload of message 0x400: two handles, both must be non-zero.
struct Msg400 {
    void* handle;   // 0x00
    u8    unk04[0x08]; // 0x04
    s32   ok;       // 0x0c
};

// Payload of light message 0xC02.
struct MsgC02 {
    u32 unk00;      // 0x00
    s32 fx;         // 0x04
    s32 unk08;      // 0x08
    s32 unk0c;      // 0x0c
};

// Payload of light message 0xC15.
struct MsgC15 {
    u32 unk00;      // 0x00
    s32 fx;         // 0x04
    s32 unk08;      // 0x08
    u8  r;          // 0x0c
    u8  g;          // 0x0d
    u8  b;          // 0x0e
    u8  a;          // 0x0f
};

// Shared light definition used by the turret's glow.
struct FxLightDef {
    u8  unk00[0x14];    // 0x00
    s32 fxId;           // 0x14
    u8  unk18[0x48];    // 0x18
};

extern "C" float lbl_804D9258;
extern "C" float lbl_804D925C;
extern "C" float lbl_804D9260;
extern "C" float lbl_804D9264;
extern "C" float lbl_804D9268;
extern "C" float lbl_804D9278;
extern "C" float lbl_804D927C;
extern "C" float lbl_804D9280;
extern "C" float lbl_804D9284;
extern "C" float lbl_804D9288;
extern "C" float lbl_804D928C;
extern "C" float lbl_804D9290;
extern "C" float lbl_804D9294;
extern "C" float lbl_804D9298;
extern "C" float lbl_804D929C;
extern "C" float lbl_804D9318;
extern "C" void* lbl_804D33A4;
extern "C" void* lbl_804D33A8;
extern "C" void* lbl_804D33AC;
extern "C" void* lbl_804D33B0;
extern "C" void* lbl_804D33B4;
extern "C" FxLightDef lbl_803BA220;

void* FxFind(const char* name);
extern "C" void* FxStart(void* fx, int a, int b);
extern "C" void fn_80027690(int id);
extern "C" void fn_8016EEF4(void* fx);
extern "C" void fn_8016F018(void* fx, int value);
extern "C" void fn_800C7B78(Actor* a);
void EmitterSetEnabled(u32 id, int enable);
void EmitterDestroy(int id);
void PossessableUpdate(Actor* a, float dt);
int Fn800816B0(void);
Actor* GetPlayer(void);
extern "C" int fn_800F94C8(int x);
extern "C" int fn_800C64E4(void);

extern "C" void SendMsg(int to, int msg, void* data);
extern "C" int fn_8001F928(void* self, void* seg, u32 mask, void* cb, int arg);
extern "C" int fn_8008EBE0(u32 stream, int idx);
extern "C" int fn_8008EA9C(u32 stream, int idx);
extern "C" void fn_8016D68C(Vec3* out, Vec3* in);
extern "C" void fn_8016D614(Vec3* a, Vec3* b, Vec3* c);
extern "C" void fn_8016D4EC(Vec3* out, Vec3* in);
extern "C" void fn_8016D440(Vec3* out, Vec3* in);
extern "C" void fn_8009F190(IVec3* out, Actor* a);
extern "C" void fn_8009F028(Actor* a, IVec3* v);
extern "C" void fn_8009F7A8(Mtx34* m, Vec3* v);
extern "C" void fn_800A0030(Vec3* out, Mtx34* m, Vec3* in);
extern "C" void fn_800A0168(Mtx34* m, Mtx34* src);
extern "C" int fn_80212730(u32 id);

int TurretVisible(void);
int TurretGunStateMsg(int msg, void* data);
int TurretGunOpenMsg(int msg, void* data);

Actor* OwnerGetInfo(u32 id);
int PlayerInHost(Actor* a);
void PlacementToMatrix(Mtx34* m, PlaceF* place);
void ActorAim(int* out, Actor* a, int idx);
void ActorGetMatrix(Mtx34* m, Actor* a);
void WrapAngle(IVec3* out, IVec3* in);
void EulerNormalize(Vec3* out, Vec3* in);
void ActorSetPlacement(Actor* a, PlaceF* place);

// 3x4 fixed-point matrix payload used by message 0x411.
struct Mat {
    s32 m[12];
};

// Payload for the 0x405/0x40a/0x411 handlers.
struct Msg411 {
    Actor* actor;   // 0x00
    s32    index;   // 0x04
    s32    anim;    // 0x08
    s32    base;    // 0x0c
    Mat    mat;     // 0x10
};

// Payload for the 0x413/0xc07 handlers.
struct Msg413 {
    Actor* dest;    // 0x00
    Actor* src;     // 0x04
    u8     unk08[0x08]; // 0x08
};

// Payload of the weapon timing message (0x407).
struct TurretWeaponMsg {
    Actor* self;    // 0x00
    s32    unk04;   // 0x04
    s32    anim;    // 0x08
    float  timer;   // 0x0c
    s32    unk10;   // 0x10
    s32    id;      // 0x14
};

// Particle emitter definition passed to EmitterCreate.
struct EmitterDesc {
    Vec3  pos;        // 0x00
    float f0c;        // 0x0c
    float f10;        // 0x10
    float f14;        // 0x14
    float f18;        // 0x18
    s32   f1c;        // 0x1c
    s32   f20;        // 0x20
    float f24;        // 0x24
    s32   f28;        // 0x28
    s32   f2c;        // 0x2c
    u8    f30;        // 0x30
    u8    pad31[0x3b];// 0x31
    float f6c;        // 0x6c
    float f70;        // 0x70
    float f74;        // 0x74
    float f78;        // 0x78
    s32   f7c;        // 0x7c
    float f80;        // 0x80
    float f84;        // 0x84
    s32   f88;        // 0x88
    float f8c;        // 0x8c
    float f90;        // 0x90
    float f94;        // 0x94
    s32   f98;        // 0x98
    s32   f9c;        // 0x9c
    s32   fa0;        // 0xa0
    s32   fa4;        // 0xa4
    float fa8;        // 0xa8
    float fac;        // 0xac
    float fb0;        // 0xb0
    float fb4;        // 0xb4
    float fb8;        // 0xb8
    float fbc;        // 0xbc
    float fc0;        // 0xc0
    float fc4;        // 0xc4
    float fc8;        // 0xc8
    u8    fcc[8];     // 0xcc
    s32   fd4;        // 0xd4
    s32   fd8;        // 0xd8
    s32   fdc;        // 0xdc
    s32   fe0;        // 0xe0
    s32   fe4;        // 0xe4
};

extern "C" double lbl_804D9270;
extern "C" float lbl_804D92A0;
extern "C" float lbl_804D92A4;
extern "C" float lbl_804D92A8;
extern "C" float lbl_804D92AC;
extern "C" float lbl_804D92B0;
extern "C" float lbl_804D92B4;
extern "C" float lbl_804D92B8;
extern "C" float lbl_804D92BC;
extern "C" float lbl_804D92C0;
extern "C" float lbl_804D92C4;
extern "C" float lbl_804D92C8;
extern "C" float lbl_804D92CC;
extern "C" float lbl_804D92D0;
extern "C" float lbl_804D92D4;
extern "C" float lbl_804D92D8;
extern "C" float lbl_804D92DC;
extern "C" float lbl_804D92E0;
extern "C" float lbl_804D92E4;
extern "C" float lbl_804D92E8;
extern "C" float lbl_804D92EC;
extern "C" float lbl_804D92F0;
extern "C" float lbl_804D92F4;
extern "C" float lbl_804D92F8;
extern "C" float lbl_804D92FC;
extern "C" float lbl_804D9300;
extern "C" float lbl_804D9304;
extern "C" float lbl_804D9308;
extern "C" float lbl_804D930C;

void* EmitterCreate(EmitterDesc* desc);
int ActorDefaultMsg(Actor* a, int msg, void* data, int arg);
int PossessableMsg(Actor* a, int msg, void* data, int arg);
int ModelAnimate(int model, char* name, int unk);
extern "C" int fn_8003A3F0(int model, int field9c, int anim);
extern "C" void fn_800E69C0(Actor* a, int value);
extern "C" void fn_801D52E0(void* p, Mtx34* m);
void ActorPlayAnim(int id, u32 hash, int idx);
int TurretWeaponUpdate(TurretWeaponMsg* m);
int TurretWeaponMessage(void* data);
int TurretQueryState(void* data);
int TurretGunCommand(void* data);
void TurretMuzzleFx(Actor* self);

// Name/asset pair of the turret's light table at lbl_803BA220.
struct FxEntry {
    u32   id;     // 0x00
    void* asset;  // 0x04
};

// Payload shared by the turret's 0xc14 and 0x41e sub-messages.
struct TurretSubMsg {
    Actor* self;   // 0x00
    s32    time;   // 0x04
    s32    target; // 0x08
    s32    value;  // 0x0c
};

// Result filled in by message 0x401 (a start time and a duration).
struct Msg401Times {
    u8    unk00[0x10]; // 0x00
    float start;       // 0x10
    float length;      // 0x14
};

// Minimal runtime string used by the debug print in TurretQueryState; the
// real implementation lives in another unit.
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
};

class String : public Buffer {
public:
    String();
    ~String() {}
    char* text;   // 0x04
    s32   length; // 0x08
};

u32 AssetFind(u32 parent, u32 id);
void MtxSetEuler(Mtx34* m, Vec3* euler);
void ModelSetMatrix(int model, Mtx34* m, char* name);
extern "C" void fn_8016D6A8(void* v);
extern "C" int fn_80019F88(void* msg, Actor* a);
extern "C" int fn_8020DF98(void* msg);
extern "C" void fn_8028BF80(String* s, const char* fmt, ...);
extern "C" void fn_8028C188(s32* out, char* text, s32 len);
extern "C" float lbl_804D9310;

// Track/aim the turret at the actor that owns it.
void TurretAimUpdate(void* owner, float dt)
{
    Actor* self = (Actor*)owner;
    TurretBeh* beh = self->beh;

    if (beh->disabled != 0) {
        return;
    }
    Actor* home = (Actor*)OwnerGetInfo(beh->ownerId);
    if (home == 0) {
        return;
    }
    if (beh->flags & 1) {
        return;
    }
    if ((beh->ownerId >> 24) == 2 && PlayerInHost(home) == 0) {
        return;
    }

    Vec3 spin;
    fn_8016D68C(&spin, &beh->rot);

    if (fn_80212730(self->id) == -1) {
        IVec3 save;
        save.x = self->mat[9];
        save.y = self->mat[10];
        save.z = self->mat[11];
        PlacementToMatrix((Mtx34*)self->mat, &beh->place);
        self->mat[9] = save.x;
        self->mat[10] = save.y;
        self->mat[11] = save.z;
    }

    if (self->cls->typeId == 0x66B15822) {
        Actor* home2 = (Actor*)OwnerGetInfo(fn_80212730(self->id));
        IVec3 save;
        save.x = self->mat[9];
        save.y = self->mat[10];
        save.z = self->mat[11];
        IVec3 aim;
        fn_8009F190(&aim, home2);
        aim.z += 0x800;
        WrapAngle(&aim, &aim);
        fn_8009F028(self, &aim);
        self->mat[9] = save.x;
        self->mat[10] = save.y;
        self->mat[11] = save.z;
    }

    Mtx34 mSelf;
    Mtx34 mHome;
    ActorGetMatrix(&mSelf, self);
    ActorGetMatrix(&mHome, home);

    Vec3 dir;
    dir.x = mHome.m[9] - mSelf.m[9];
    dir.y = mHome.m[10] - mSelf.m[10];
    dir.z = mHome.m[11] - mSelf.m[11];
    fn_800A0030(&dir, &mSelf, &dir);

    Vec3 euler;
    fn_8016D440(&euler, &dir);
    fn_8016D614(&euler, &euler, &beh->rot);
    EulerNormalize(&euler, &euler);

    float sz = lbl_804D925C;
    if (fabsf(euler.z) >= lbl_804D9258) {
        sz = euler.z / fabsf(euler.z);
    }
    float sx = lbl_804D925C;
    if (fabsf(euler.x) >= lbl_804D9258) {
        sx = euler.x / fabsf(euler.x);
    }

    float limZ = fabsf(euler.z);
    float limX = fabsf(euler.x);
    float dz = sz * (lbl_804D9260 * self->cls->f40 * dt);
    float dx = sx * (lbl_804D9260 * self->cls->f38 * dt);
    if (dz < -limZ) {
        dz = -limZ;
    } else if (dz > limZ) {
        dz = limZ;
    }
    if (dx < -limX) {
        dx = -limX;
    } else if (dx > limX) {
        dx = limX;
    }

    float maxZ = self->cls->f3c * lbl_804D9264;
    if (maxZ >= lbl_804D9268) {
        maxZ = lbl_804D9268;
    }
    float maxX = self->cls->f34 * lbl_804D9264;
    if (maxX >= lbl_804D9268) {
        maxX = lbl_804D9268;
    }

    beh->rot.z += dz;
    if (self->cls->typeId != 0x6B87B566) {
        beh->rot.x += dx;
    }
    EulerNormalize(&beh->rot, &beh->rot);
    if (beh->rot.z < -maxZ) {
        beh->rot.z = -maxZ;
    } else if (beh->rot.z > maxZ) {
        beh->rot.z = maxZ;
    }
    if (beh->rot.x < -maxX) {
        beh->rot.x = -maxX;
    } else if (beh->rot.x > maxX) {
        beh->rot.x = maxX;
    }

    fn_8016D614(&spin, &beh->rot, &spin);
    Vec3 turned;
    fn_8016D68C(&turned, &beh->rot);
    fn_8016D4EC(&dir, &turned);

    Mtx34 mCur;
    ActorGetMatrix(&mCur, self);
    fn_800A0168(&mCur, &mCur);
    fn_800A0030(&dir, &mCur, &dir);
    fn_8009F7A8(&mSelf, &dir);

    PlaceF place;
    place.pos.x = (float)self->mat[9];
    place.pos.y = (float)self->mat[10];
    place.pos.z = (float)self->mat[11];
    PlacementToMatrix((Mtx34*)self->mat, &place);
    ActorSetPlacement(self, &place);
}

// Fire control: periodically aim, test the line of sight and trigger the
// next animation step of the turret.
void TurretFireUpdate(void* owner, float dt)
{
    Actor* self = (Actor*)owner;
    TurretBeh* beh = self->beh;
    int dead = 0;

    if (beh->ownerId == 0 || beh->ownerId == (u32)-1 || beh->fieldB4 == -1 ||
        beh->fieldB8 != 1 || (beh->flags & 1) != 0) {
        dead = 1;
    } else {
        beh->timer -= dt;
        if (beh->timer <= 0.0f) {
            beh->timer += lbl_804D927C;

            Msg402 msg;
            msg.id = (u16)beh->fieldB4;
            SendMsg(self->id, 0x402, &msg);

            Actor* target = (Actor*)OwnerGetInfo(beh->ownerId);
            if ((beh->ownerId >> 24) == 2 && PlayerInHost(target) == 0) {
                dead = 1;
            }
            if (!dead) {
                int aimv[3];
                ActorAim(aimv, (Actor*)msg.mtx, 1);

                Vec3 v;
                v.x = lbl_804D9280 * (float)aimv[0];
                v.y = lbl_804D9280 * (float)aimv[1];
                v.z = lbl_804D9280 * (float)aimv[2];

                int n = 0;
                if (fabsf(v.x) > lbl_804D9258) {
                    n++;
                }
                if (fabsf(v.y) > lbl_804D9258) {
                    n++;
                }
                if (fabsf(v.z) > lbl_804D9258) {
                    n++;
                }
                if (n == 0) {
                    v.x = lbl_804D925C;
                    v.y = lbl_804D925C;
                    v.z = lbl_804D925C;
                } else {
                    float inv = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
                    v.x *= inv;
                    v.y *= inv;
                    v.z *= inv;
                }

                Vec3 dir;
                dir.x = (float)(target->mat[9] - msg.mtx[9]);
                dir.y = (float)(target->mat[10] - msg.mtx[10]);
                dir.z = (float)(target->mat[11] - msg.mtx[11]);

                int m = 0;
                if (fabsf(dir.x) > lbl_804D9258) {
                    m++;
                }
                if (fabsf(dir.y) > lbl_804D9258) {
                    m++;
                }
                if (fabsf(dir.z) > lbl_804D9258) {
                    m++;
                }
                if (m == 0) {
                    dir.x = lbl_804D925C;
                    dir.y = lbl_804D925C;
                    dir.z = lbl_804D925C;
                } else {
                    float inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
                    dir.x *= inv;
                    dir.y *= inv;
                    dir.z *= inv;
                }

                if (v.z * dir.z + v.x * dir.x + v.y * dir.y < lbl_804D9284) {
                    dead = 1;
                }
            }
            if (!dead) {
                s32 seg[6];
                seg[0] = self->mat[9];
                seg[1] = self->mat[10];
                seg[2] = self->mat[11];
                seg[3] = target->mat[9];
                seg[4] = target->mat[10];
                seg[5] = target->mat[11];
                if (fn_8001F928(self, seg, 0x9000A200, (void*)TurretVisible, 0) != 0) {
                    dead = 1;
                }
            }
            if (!dead) {
                ActorClass* cls = self->cls;
                int idx = (cls->typeId == 0x9EC9500A) ? 6 : 5;
                if (cls->data[idx] != fn_8008EBE0(self->stream, 0) ||
                    fn_8008EA9C(self->stream, 0) != 1) {
                    self->beh->unk44 = 0;
                    MsgC10 m;
                    m.id = cls->data[idx];
                    m.unk08 = 0;
                    m.unk0c = 1;
                    m.unk10 = 0x1000;
                    m.fn = (void*)TurretGunOpenMsg;
                    m.owner = self;
                    SendMsg(self->id, 0xC10, &m);
                }
            }
        }
    }

    if (dead) {
        ActorClass* cls = self->cls;
        if (cls->typeId == 0x6B87B566) {
            TurretBeh* b = self->beh;
            int idx = (b->state == 1 || b->state == 2 || b->state == 3) ? 0x11 : 8;
            if (cls->data[idx] != fn_8008EBE0(self->stream, 0) ||
                fn_8008EA9C(self->stream, 0) != 1) {
                self->beh->unk44 = 0;
                MsgC10 m;
                m.id = cls->data[idx];
                m.unk08 = 0;
                m.unk0c = 1;
                m.unk10 = 0x1000;
                m.fn = 0;
                m.owner = 0;
                SendMsg(self->id, 0xC10, &m);
            }
        } else if (self != 0 && self->beh != 0) {
            self->beh->unk44 = 1;
        }
    }
}

// Line-of-sight callback handed to fn_8001F928: always reports a hit.
int TurretVisible(void)
{
    return 1;
}

// Per-frame turret update: weapon step timing, aim/fire sequencing, the
// glow/particle feedback and finally the generic possessable update.
void TurretUpdate(Actor* self, float dt)
{
    TurretBeh* beh = self->beh;

    if (beh->fieldB0 != -1) {
        fn_80027690(beh->fieldB0);
        beh->fieldB0 = -1;
    }

    switch (beh->state) {
    case 4:
        if (beh->f38 >= lbl_804D9288) {
            beh->f38 -= dt;
            if (beh->f38 < lbl_804D9288) {
                if (self->cls->data[0x12] != fn_8008EBE0(self->stream, 0) ||
                    fn_8008EA9C(self->stream, 0) != 1) {
                    FxStart(FxFind("gunopen"), 0, 0);
                    beh->unk44 = 0;

                    MsgC10 msg;
                    msg.id = self->cls->data[0x12];
                    msg.unk08 = 0;
                    msg.unk0c = 0x10;
                    msg.unk10 = 0x1000;
                    msg.fn = (void*)TurretGunStateMsg;
                    msg.owner = self;
                    SendMsg(self->id, 0xC10, &msg);

                    if (beh->field0c != -1) {
                        msg.fn = (void*)self->id;
                        msg.owner = 0;
                        SendMsg(beh->field0c, 0xC04, &msg);
                    }
                }
            }
        }
        break;

    case 3:
        if (beh->f38 >= lbl_804D9288) {
            beh->f38 -= dt;
            if (beh->f38 < lbl_804D9288) {
                if (self->cls->data[0x13] != fn_8008EBE0(self->stream, 0) ||
                    fn_8008EA9C(self->stream, 0) != 1) {
                    FxStart(FxFind("gunopen"), 0, 0);
                    beh->unk44 = 0;

                    MsgC10 msg;
                    msg.id = self->cls->data[0x13];
                    msg.unk08 = 0;
                    msg.unk0c = 0x10;
                    msg.unk10 = 0x1000;
                    msg.fn = (void*)TurretGunStateMsg;
                    msg.owner = self;
                    SendMsg(self->id, 0xC10, &msg);

                    if (beh->field0c != -1) {
                        msg.fn = (void*)self->id;
                        msg.owner = 0;
                        SendMsg(beh->field0c, 0xC04, &msg);
                    }
                }
            }
        }
        break;
    }

    switch (beh->mode) {
    case 2: {
        Msg400 msg;
        msg.handle = 0;
        msg.ok = 0;
        SendMsg(beh->ownerId, 0x400, &msg);
        if (msg.handle == 0 || msg.ok == 0) {
            beh->mode = 1;
            if (self != 0 && self->beh != 0) {
                self->beh->unk44 = 1;
            }
        } else {
            TurretAimUpdate(self, dt);
            TurretFireUpdate(self, dt);
        }
        break;
    }

    case 3:
        if (beh->f3c > lbl_804D925C) {
            beh->f40 += dt;
            if (beh->f40 > beh->f3c) {
                float save = self->beh->f3c;
                fn_800C7B78(self);
                self->beh->f3c = save;
                TurretUpdate(self, lbl_804D928C);
                return;
            }
        }
        break;
    }

    float v = beh->f104;
    if (v < lbl_804D925C) {
        v = lbl_804D925C;
    }
    beh->f104 = v;

    if (beh->disabled != 0) {
        float level = beh->f104;
        if (level >= lbl_804D9290) {
            MsgC02 m;
            m.fx = lbl_803BA220.fxId;
            m.unk08 = 0;
            m.unk0c = 1;
            SendMsg(beh->lightId, 0xC02, &m);

            MsgC15 c;
            c.fx = lbl_803BA220.fxId;
            c.unk08 = 1;
            c.r = 0xFF;
            c.g = 0xFF;
            c.b = 0xFF;
            c.a = 0;
            SendMsg(beh->lightId, 0xC15, &c);

            fn_8016EEF4(beh->fxHandle);
            beh->fxHandle = (void*)-1;
        } else {
            MsgC02 m;
            m.fx = lbl_803BA220.fxId;
            m.unk08 = 1;
            m.unk0c = 1;
            SendMsg(beh->lightId, 0xC02, &m);

            if (level < lbl_804D925C) {
                level = lbl_804D925C;
            } else if (level > lbl_804D9290) {
                level = lbl_804D9290;
            }

            int lit = (int)floor((double)(lbl_804D9294 * (lbl_804D9290 - level)));
            if (Fn800816B0() > -1) {
                lit = 0;
            } else {
                int cur = fn_800F94C8(GetPlayer()->f184);
                if (fn_800C64E4() != cur) {
                    lit = 0;
                }
            }

            if (beh->fxHandle == (void*)-1) {
                beh->fxHandle = FxStart(lbl_804D33A4, 0, 0);
                fn_8016F018(beh->fxHandle, lit);
            } else {
                fn_8016F018(beh->fxHandle, lit);
            }

            MsgC15 c;
            c.fx = lbl_803BA220.fxId;
            c.unk08 = 1;
            c.r = 0xFF;
            c.g = 0xFF;
            c.b = 0xFF;
            c.a = (u8)(0xE0 - (int)(lbl_804D9298 * level));
            SendMsg(beh->lightId, 0xC15, &c);
        }
    }

    TurretBeh* b = self->beh;
    if (b->fxEmitter0 != -1) {
        float t = b->fxTimer;
        b->fxTimer += dt;
        if (b->fxTimer >= lbl_804D929C && t < lbl_804D929C) {
            EmitterSetEnabled(b->fxEmitter0, 0);
        }
        if (b->fxTimer >= lbl_804D929C && t < lbl_804D929C) {
            EmitterSetEnabled(b->fxEmitter1, 0);
        }
        if (b->fxTimer >= lbl_804D9288) {
            EmitterDestroy(b->emitter1);
            EmitterDestroy(b->emitter2);
            b->fxEmitter0 = -1;
            b->fxEmitter1 = -1;
        }
    }

    PossessableUpdate(self, dt);
}

// Message handler for the turret's possessable behaviour: the weapon and
// animation messages are handled here, everything else is forwarded.
int TurretPossessableMsg(Actor* self, int msg, void* data, int arg)
{
    switch (msg) {
    case 0x407:
        return TurretWeaponUpdate((TurretWeaponMsg*)data);

    case 0x405:
        ((Msg411*)data)->mat.m[0] = 0;
        return 1;

    case 3:
    case 4:
        return 0;

    case 0x40a: {
        Actor* a = ((Msg411*)data)->actor;
        if (a->beh->state == 1 || a->beh->mode == 3) {
            ((Msg411*)data)->base = 0;
            return 1;
        }
        break;
    }

    case 0x411: {
        Msg411* m = (Msg411*)data;
        Actor* a = m->actor;
        if (a == 0) {
            return 0;
        }
        if (a->model == -1) {
            m->mat = *(Mat*)a->mat;
            m->anim = 0;
            m->base = 0;
        } else {
            int anim = ModelAnimate(a->model, 0, 0);
            int base = fn_8003A3F0(a->model, a->field9c, anim);
            if (m->index == -1 || ((u8*)anim)[m->index] == 0xff) {
                m->mat = *(Mat*)a->mat;
                m->anim = 0;
                m->base = 0;
            } else {
                m->mat = ((Mat*)base)[m->index];
                m->anim = base;
                m->base = anim;
            }
        }
        return 1;
    }

    case 0x413: {
        Msg413* m = (Msg413*)data;
        Actor* a = m->dest;
        if (a == 0) {
            return 0;
        }
        Mtx34 mtx;
        ActorGetMatrix(&mtx, m->src);
        PlacementToMatrix((Mtx34*)a, (PlaceF*)&mtx);
        ActorSetPlacement(a, (PlaceF*)&mtx);
        if (a->field11c != 0) {
            fn_800A0168(&mtx, &mtx);
            fn_801D52E0(a->field11c, &mtx);
        }
        a->fieldac |= 0x800000;
        return 1;
    }

    case 0x41d:
        if (TurretGunCommand(data)) {
            return 1;
        }
        break;

    case 0x41e:
        if (TurretQueryState(data)) {
            return 1;
        }
        break;

    case 0xc07: {
        Actor* a = ((Msg413*)data)->dest;
        TurretBeh* beh = a->beh;
        int r = ActorDefaultMsg((Actor*)a->id, 0xc07, data, 0);
        ActorGetMatrix((Mtx34*)&beh->place, a);
        return r;
    }

    case 0xc14:
        if (TurretWeaponMessage(data)) {
            return 1;
        }
        break;
    }
    return PossessableMsg(self, msg, data, arg);
}

// Weapon timer update (message 0x407): counts the shot charge down, fires the
// 0xc05 event and spawns the muzzle emitters.
int TurretWeaponUpdate(TurretWeaponMsg* m)
{
    Actor* self = m->self;
    TurretBeh* beh = self->beh;

    if (m->id == 0x403548d0) {
        beh->flags |= 2;
        m->timer = lbl_804D925C;
    } else if (m->id == -0x4603bd0b) {
        m->timer = lbl_804D92A0;
    }

    if (beh->f10 > lbl_804D925C && (beh->flags & 8) == 0) {
        beh->f10 -= m->timer;
        if (beh->f10 <= lbl_804D925C) {
            u8 msgbuf[0x10];
            SendMsg(self->id, 0xc05, msgbuf);
            beh->mode = 3;
            beh->f40 = lbl_804D925C;
            if (self != 0 && self->beh != 0) {
                self->beh->unk44 = 1;
            }
            ActorPlayAnim(self->id, 0xdc04e3d5, m->anim);
            if (0 < self->cls->data[10]) {
                TurretMuzzleFx(self);
                fn_800E69C0(self, self->cls->data[10]);

                EmitterDesc desc;
                desc.pos.x = (float)self->mat[9];
                desc.pos.y = (float)self->mat[10];
                desc.pos.z = (float)self->mat[11];
                desc.f0c = lbl_804D92A4;
                desc.f10 = lbl_804D92A8;
                desc.f14 = lbl_804D92AC;
                desc.f18 = lbl_804D92B0;
                desc.f1c = 0;
                desc.f20 = 2;
                desc.f24 = lbl_804D92B4;
                desc.f28 = 1;
                desc.f2c = self->cls->data[0];
                desc.f30 = 0xff;
                desc.f6c = lbl_804D92B8;
                desc.f70 = lbl_804D92BC;
                desc.f74 = lbl_804D92C0;
                desc.f78 = lbl_804D92C4;
                desc.f7c = 0xf;
                desc.f80 = lbl_804D92C8;
                desc.f84 = lbl_804D9290;
                desc.f88 = 2;
                desc.f8c = lbl_804D925C;
                desc.f90 = lbl_804D92B4;
                desc.f94 = lbl_804D92CC;
                desc.f98 = 0;
                desc.f9c = 0;
                desc.fa4 = 0;
                desc.fac = lbl_804D925C;
                desc.fb0 = lbl_804D925C;
                desc.fb4 = lbl_804D925C;
                desc.fb8 = lbl_804D92D0;
                desc.fbc = lbl_804D925C;
                desc.fc0 = lbl_804D925C;
                desc.fc4 = lbl_804D925C;
                desc.fc8 = lbl_804D92D4;
                desc.fcc[0] = 0xff;
                desc.fcc[1] = 0xff;
                desc.fcc[2] = 0xff;
                desc.fcc[3] = 0xff;
                desc.fcc[4] = 0xff;
                desc.fcc[5] = 0xff;
                desc.fcc[6] = 0xff;
                desc.fcc[7] = 0xff;
                desc.fd4 = -1;
                desc.fd8 = -1;
                desc.fdc = 0;
                desc.fe0 = 0;
                desc.fe4 = 0;
                beh->emitter1 = (s32)EmitterCreate(&desc);
                EmitterSetEnabled((u32)beh->emitter1, 1);

                beh->beaconPos.x = (float)self->mat[9];
                beh->beaconPos.y = (float)self->mat[10];
                beh->beaconPos.z = (float)self->mat[11];
                beh->beaconRot.x = lbl_804D925C;
                beh->beaconRot.y = lbl_804D925C;
                beh->beaconRot.z = lbl_804D925C;
                beh->beaconF = lbl_804D9290;
                beh->beaconI = 0;
                beh->beaconG = lbl_804D9300;
                beh->beaconH = lbl_804D925C;

                desc.pos.x = (float)self->mat[9];
                desc.pos.y = (float)self->mat[10];
                desc.pos.z = (float)self->mat[11];
                desc.f0c = lbl_804D925C;
                desc.f10 = lbl_804D92D8;
                desc.f14 = lbl_804D92DC;
                desc.f18 = lbl_804D92E0;
                desc.f1c = 0;
                desc.f20 = 2;
                desc.f24 = lbl_804D92E4;
                desc.f28 = 1;
                desc.f2c = self->cls->data[1];
                desc.f30 = 0xff;
                desc.f6c = lbl_804D92E8;
                desc.f70 = lbl_804D92EC;
                desc.f74 = lbl_804D92F0;
                desc.f78 = lbl_804D92F4;
                desc.f7c = 100;
                desc.f80 = lbl_804D92C8;
                desc.f84 = lbl_804D9290;
                desc.f88 = 0x10;
                desc.f8c = lbl_804D925C;
                desc.f90 = lbl_804D92F8;
                desc.f94 = lbl_804D92FC;
                desc.f98 = 1;
                desc.f9c = 1;
                desc.fa0 = (s32)&beh->beaconPos;
                desc.fa4 = 0;
                desc.fb0 = lbl_804D9304;
                desc.fb4 = lbl_804D925C;
                desc.fb8 = lbl_804D9308;
                desc.fbc = lbl_804D925C;
                desc.fc0 = lbl_804D925C;
                desc.fc4 = lbl_804D9264;
                desc.fc8 = lbl_804D930C;
                desc.fcc[0] = 0x19;
                desc.fcc[1] = 0x19;
                desc.fcc[2] = 0x19;
                desc.fcc[3] = 0xff;
                desc.fcc[4] = 0x19;
                desc.fcc[5] = 0x19;
                desc.fcc[6] = 0x19;
                desc.fcc[7] = 0xff;
                desc.fd4 = -1;
                desc.fd8 = -1;
                desc.fdc = 0;
                desc.fe0 = 0;
                desc.fe4 = 0;
                beh->emitter2 = (s32)EmitterCreate(&desc);
                EmitterSetEnabled((u32)beh->emitter2, 1);
            }
        }
        float t = beh->f10;
        float maxv = self->cls->f30;
        float r;
        if (t < lbl_804D925C) {
            r = lbl_804D925C;
        } else if (t > maxv) {
            r = maxv;
        } else {
            r = t;
        }
        beh->f10 = r;
        beh->f14 = lbl_804D925C;
    }
    return 1;
}

// Handler for the turret's light/weapon sub-messages (message 0xc14).
int TurretWeaponMessage(void* data)
{
    TurretSubMsg* msg = (TurretSubMsg*)data;
    Actor* self = msg->self;
    TurretBeh* beh = self->beh;
    int idx;
    int cur;

    switch (msg->value) {
    case 0x39CB0CB0:
        if (beh->fxHandle != (void*)-1) {
            fn_8016EEF4(beh->fxHandle);
            beh->fxHandle = (void*)-1;
        }
        if (beh->state == 1 || beh->state == 2 || beh->state == 3) {
            idx = 0x11;
        } else {
            idx = 8;
        }
        cur = fn_8008EBE0(self->stream, 0);
        if (cur != self->cls->data[idx] || fn_8008EA9C(self->stream, 0) != 1) {
            beh->unk44 = 0;
            MsgC10 m;
            m.id = self->cls->data[idx];
            m.unk08 = 0;
            m.unk0c = 1;
            m.unk10 = 0x1000;
            m.fn = 0;
            m.owner = 0;
            SendMsg(self->id, 0xc10, &m);
        }
        break;

    case 0x672F35C2:
        if (self->cls->typeId != 0x66B15822) {
            Mtx34 mat;
            Vec3* place = (Vec3*)self->field80;

            MtxSetEuler(&mat, (Vec3*)((u8*)self->field80 + 0xc));
            mat.m[9] = place->x;
            mat.m[10] = place->y;
            mat.m[11] = place->z;
            PlacementToMatrix(&mat, (PlaceF*)self->field80);
            self->fieldE0 = mat;
            ModelAnimate(self->model, (char*)self->field9c, 1);
            ModelSetMatrix(self->model, &mat, (char*)self->field9c);
            fn_8016D6A8(&beh->rot);
            fn_8016D6A8(&beh->rot2);
            beh->field98 = 0;
            ActorGetMatrix((Mtx34*)&beh->place, self);
        }
        beh->lightId = msg->target;
        if (beh->state == 1 || beh->state == 2 || beh->state == 3) {
            idx = 0x11;
        } else {
            idx = 8;
        }
        cur = fn_8008EBE0(self->stream, 0);
        if (cur != self->cls->data[idx] || fn_8008EA9C(self->stream, 0) != 1) {
            beh->unk44 = 0;
            MsgC10 m;
            m.id = self->cls->data[idx];
            m.unk08 = 0;
            m.unk0c = 1;
            m.unk10 = 0x1000;
            m.fn = 0;
            m.owner = 0;
            SendMsg(self->id, 0xc10, &m);
        }
        beh->disabled = 1;
        break;

    case 0x7BF91F20: {
        FxEntry* entry = (FxEntry*)&lbl_803BA220;
        int i;

        for (i = 0; i < 12; i++) {
            entry[i].asset = (void*)AssetFind(0xD9044CBB, entry[i].id);
        }
        {
            MsgC02 m;
            m.unk08 = 0;
            m.unk0c = 1;
            for (i = 0; i < 12; i++) {
                m.fx = (s32)entry[i].asset;
                SendMsg(msg->target, 0xc02, &m);
            }
        }
        {
            MsgC15 m;
            m.fx = (s32)entry[2].asset;
            m.unk08 = 1;
            m.r = 0xff;
            m.g = 0xff;
            m.b = 0xff;
            m.a = 0;
            SendMsg(msg->target, 0xc15, &m);
        }
        {
            Msg401Times t;
            SendMsg(msg->target, 0x401, &t);
            beh->f104 = (t.start - (float)msg->time) / t.length;
        }
        if (beh->f104 < lbl_804D925C || lbl_804D9290 < beh->f104) {
            beh->f104 = lbl_804D9290;
        }
        if (beh->state != 5 && self->cls->data[0x12] != -1) {
            beh->state = 4;
            beh->f38 = lbl_804D9310;
        }
    } break;

    case 0xDE64098E:
        if (beh->mode != 3) {
            Msg401Times t;
            SendMsg(msg->target, 0x401, &t);
            beh->f104 = (t.start - (float)msg->time) / t.length;
            beh->lightId = msg->target;
        }
        break;

    case 0xD5746CF2:
        beh->disabled = 0;
        break;

    case 0x83A2F974:
        if (beh->state == 4) {
            beh->state = 5;
        } else {
            beh->state = 2;
        }
        break;
    }
    return 0;
}

// Reports the turret's current state or a state-dependent id (message 0x41e).
int TurretQueryState(void* data)
{
    TurretSubMsg* msg = (TurretSubMsg*)data;
    Actor* self = msg->self;
    TurretBeh* beh = self->beh;

    if (fn_80019F88(data, self)) {
        return 1;
    }

    switch (msg->value) {
    case 0x084505BB:
        msg->value = (beh->state == 1) ? 1 : 0;
        return 1;

    case 0x6A0D51C0:
        if (beh->state == 0 || beh->state == 5) {
            msg->value = self->cls->data[8];
        } else if (beh->state == 4) {
            beh->state = 5;
            msg->value = self->cls->data[8];
        } else if (beh->state == 3) {
            beh->state = 2;
            msg->value = self->cls->data[0x11];
        } else {
            msg->value = self->cls->data[0x11];
        }
        return 1;

    case 0x73B1E248:
        msg->value = beh->fieldB8;
        return 1;

    case 0x93116591:
        msg->value = (s32)beh->ownerId;
        return 1;

    case 0xAD8576FA:
        {
            String s;
            fn_8028BF80(&s, "Yaw: %f Pitch: %f", beh->rot.z, beh->rot.x);
            fn_8028C188(&msg->value, s.text, s.length);
            fn_8028BF80(&s, "axis: %d, %d", self->mat[0], self->mat[1]);
            fn_8028C188(&msg->value, s.text, s.length);
        }
        return fn_8020DF98(data);

    case 0xF8C4B41D:
        if (beh->state == 5 || beh->state == 4 || beh->state == 0) {
            msg->value = 0;
        } else {
            msg->value = 1;
        }
        return 1;
    }
    return fn_8020DF98(data);
}

// Payload of the turret's gun commands (message 0x41d).
struct TurretCmd {
    Actor* self;    // 0x00
    u32    cmd;     // 0x04
    u32    unk08;   // 0x08
    s32    value;   // 0x0c
};

// Payload of the 0xc14 message the gun command handler sends to its owner.
struct MsgC14Cmd {
    u32 unk00;      // 0x00
    s32 unk04;      // 0x04
    u32 id;         // 0x08
    u32 hash;       // 0x0c
};

extern "C" float lbl_804D9314;
extern "C" int fn_800398A8(const char* name);
extern "C" int fn_8020DD08(void* data);
void ModelNodeSetup(int model, int node, int a, int b);

// Gun command handler (message 0x41d): opens/closes the gun, changes its owner
// and answers state queries.
int TurretGunCommand(void* data)
{
    TurretCmd* msg = (TurretCmd*)data;
    Actor* self = msg->self;
    TurretBeh* beh = self->beh;
    s32 state;

    if (fn_80019F88(data, self) != 0) {
        return 1;
    }

    switch (msg->cmd) {
    case 0x032C46E5:
        beh->f3c = (float)msg->value;
        return 1;

    case 0xF8C4B41D:
        state = beh->state;
        if (state == 0) {
            return 1;
        }
        if (msg->value == 1 && state != 2) {
            beh->state = 3;
            beh->f38 = lbl_804D9314;
            if (beh->field0c != -1 && (beh->field0c >> 24) == 2) {
                MsgC14Cmd m;
                m.unk04 = 0;
                m.id = self->id;
                m.hash = 0x771E036F;
                SendMsg(beh->field0c, 0xC14, &m);
            }
            return 1;
        }
        if (msg->value == 0 && state != 5) {
            beh->state = 4;
            if (beh->field0c == -1) {
                beh->f38 = lbl_804D9314;
            } else {
                beh->f38 = lbl_804D9310;
            }
            beh->flags &= ~8;
            return 1;
        }
        break;

    case 0x93116591:
        self = msg->self;
        if (self == 0) {
            return 0;
        }
        beh = self->beh;
        if (beh == 0) {
            return 0;
        }
        if (beh->ownerId == (u32)msg->value) {
            return 1;
        }
        beh->ownerId = (u32)msg->value;
        if (beh->ownerId == 0 || beh->ownerId == 0xFFFFFFFF) {
            beh->mode = 1;
            beh->ownerId = 0;
            ModelNodeSetup(self->model, fn_800398A8("Light_On"), 1, 1);
            ModelNodeSetup(self->model, fn_800398A8("Light_Off"), 0, 1);
            if (self != 0 && self->beh != 0) {
                self->beh->unk44 = 1;
            }
        } else {
            beh->mode = 2;
            ModelNodeSetup(self->model, fn_800398A8("Light_On"), 0, 1);
            ModelNodeSetup(self->model, fn_800398A8("Light_Off"), 1, 1);
        }
        return 1;

    case 0x73B1E248:
        self->beh->fieldB8 = msg->value;
        return 1;

    case 0x084505BB:
        state = beh->state;
        if (state == 0) {
            return 1;
        }
        if (msg->value == 1 && state != 1) {
            if (state != 2 &&
                (fn_8008EBE0(self->stream, 0) != self->cls->data[0x13] ||
                 fn_8008EA9C(self->stream, 0) != 1)) {
                FxStart(FxFind("gunopen"), 0, 0);
                beh->unk44 = 0;

                MsgC10 m;
                m.id = self->cls->data[0x13];
                m.unk08 = 0;
                m.unk0c = 0x10;
                m.unk10 = 0x1000;
                m.fn = (void*)TurretGunStateMsg;
                m.owner = self;
                SendMsg(self->id, 0xC10, &m);

                if (beh->field0c != -1) {
                    m.fn = (void*)self->id;
                    m.owner = 0;
                    SendMsg(beh->field0c, 0xC04, &m);
                }
            }
            beh->state = 1;
            beh->flags |= 8;
            return 1;
        }
        if (msg->value == 0 && state == 1) {
            beh->state = 2;
            beh->flags &= ~8;
            return 1;
        }
        break;
    }
    return fn_8020DD08(data);
}

void PossessableDestroy(Actor* a);

// Destruction: releases the turret's effects, then runs the common possessable
// teardown.
void TurretDestroy(Actor* a)
{
    TurretBeh* beh = a->beh;

    if (beh->fieldB0 != -1) {
        fn_80027690(beh->fieldB0);
        beh->fieldB0 = -1;
    }
    if (beh->fxHandle != (void*)-1) {
        fn_8016EEF4(beh->fxHandle);
        beh->fxHandle = (void*)-1;
    }
    PossessableDestroy(a);
}

void* VAlloc(int size);
int PossessableInit(Actor* a);
int ModelFindBone(int model, int bone);

// Brings the turret's behaviour state up: allocates it, releases any previous
// owner, fills in the per-class tuning and creates the effects it needs.
int TurretInit(Actor* self)
{
    TurretBeh* beh = self->beh;
    int state;
    int idx;
    int v;

    if (beh == 0) {
        beh = (TurretBeh*)VAlloc(0x10c);
        self->beh = beh;
    }
    if (PossessableInit(self) == 0) {
        self->fieldac |= 0x1000000;
        return 0;
    }

    beh->f3c = lbl_804D925C;
    beh->f40 = lbl_804D925C;
    if (self != 0 && self->beh != 0 && self->beh->ownerId != 0) {
        TurretBeh* b = self->beh;
        b->ownerId = 0;
        if (self->beh->ownerId == 0 || self->beh->ownerId == 0xFFFFFFFF) {
            beh->mode = 1;
            beh->ownerId = 0;
            ModelNodeSetup(self->model, fn_800398A8("Light_On"), 1, 1);
            ModelNodeSetup(self->model, fn_800398A8("Light_Off"), 0, 1);
            if (self != 0 && self->beh != 0) {
                beh->unk44 = 1;
            }
        } else {
            beh->mode = 2;
            ModelNodeSetup(self->model, fn_800398A8("Light_On"), 0, 1);
            ModelNodeSetup(self->model, fn_800398A8("Light_Off"), 1, 1);
        }
    }

    fn_8016D6A8(&beh->rot);
    beh->field98 = 0;
    if (self->cls->f1c == 0x942B64F0) {
        beh->flags &= ~8;
    }
    beh->f48 = self->cls->f3c;
    beh->f4c = (lbl_804D930C * self->cls->f3c) / self->cls->f40;
    beh->f9c = self->cls->f44;
    beh->fA0 = lbl_804D9318;
    beh->ownerId = 0;
    beh->fA8 = lbl_804D9278;
    beh->fieldB8 = 1;
    fn_8016D6A8(&beh->rot2);
    ActorGetMatrix((Mtx34*)&beh->place, self);
    beh->timer = lbl_804D925C;
    beh->fieldB0 = -1;
    beh->fieldB4 = ModelFindBone(self->model, AssetFind(0xF1754BF2, 0x7B91FFD5));
    beh->mode = 0;
    if (self->cls->data[0x13] == -1) {
        beh->state = 0;
    } else {
        beh->state = 2;
    }
    beh->f38 = lbl_804D925C;

    state = self->beh->state;
    if (state == 1 || state == 2 || state == 3) {
        idx = 0x11;
    } else {
        idx = 8;
    }
    v = fn_8008EBE0(self->stream, 0);
    if (self->cls->data[idx] != v || fn_8008EA9C(self->stream, 0) != 1) {
        self->beh->unk44 = 0;

        MsgC10 m;
        m.id = self->cls->data[idx];
        m.unk08 = 0;
        m.unk0c = 1;
        m.unk10 = 0x1000;
        m.fn = 0;
        m.owner = 0;
        SendMsg(self->id, 0xC10, &m);
    }

    lbl_804D33A4 = FxFind("StaticWhite");
    beh->fxHandle = (void*)-1;
    lbl_804D33A8 = (void*)AssetFind(0xF2E2BEB8, 0xDE1813E0);
    lbl_804D33AC = (void*)AssetFind(0xF2E2BEB8, 0x513EB38C);
    lbl_804D33B0 = (void*)AssetFind(0xF2E2BEB8, 0x514CCB0D);
    lbl_804D33B4 = (void*)AssetFind(0xF2E2BEB8, 0x515AE28E);
    beh->fxTimer = lbl_804D925C;
    beh->fxEmitter0 = -1;
    beh->fxEmitter1 = -1;
    beh->f104 = lbl_804D9290;
    beh->lightId = 0;
    beh->disabled = 0;
    return 1;
}

// Gun state message (0x1b83833f): toggles the open/closed state and restarts
// the gun's animation if it is not already playing the right one.
int TurretGunStateMsg(int msg, void* data)
{
    Actor* self = (Actor*)data;
    TurretBeh* beh = self->beh;
    int state;
    int idx;
    int v;

    if (msg == 0x1B83833F) {
        if (beh->state == 4) {
            beh->state = 5;
        } else {
            beh->state = 2;
        }
        state = self->beh->state;
        if (state == 1 || state == 2 || state == 3) {
            idx = 0x11;
        } else {
            idx = 8;
        }
        v = fn_8008EBE0(self->stream, 0);
        if (self->cls->data[idx] != v || fn_8008EA9C(self->stream, 0) != 1) {
            self->beh->unk44 = 0;

            MsgC10 m;
            m.id = self->cls->data[idx];
            m.unk08 = 0;
            m.unk0c = 1;
            m.unk10 = 0x1000;
            m.fn = 0;
            m.owner = 0;
            SendMsg(self->id, 0xC10, &m);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// Helpers for the turret's effect messages. Data and code below live in other
// units; the function names are placeholders from symbols.txt.
// ---------------------------------------------------------------------------

extern "C" float lbl_804D33A0;
extern "C" float lbl_804D931C;
extern "C" float lbl_804D9320;
extern "C" float lbl_804D9324;
extern "C" float lbl_804D9328;
extern "C" float lbl_804D932C;
extern "C" float lbl_804D9330;
extern "C" float lbl_804D9334;
extern "C" float lbl_804D9338;
extern "C" float lbl_804D933C;
extern "C" float lbl_804D9340;
extern "C" float lbl_804D9344;
extern "C" float lbl_804D9348;
extern "C" float lbl_804D934C;
extern "C" float lbl_804D9350;
extern "C" float lbl_804D9354;
extern "C" float lbl_804D9358;

extern "C" void LinkInfoClear(void* info);
extern "C" int fn_800F8A98(int type);
extern "C" void fn_800F7F5C(double a, int b, int c, int msg, u32 id, void* info);
extern "C" int fn_800EB1B8(int fx, int idx);
extern "C" void fn_800F840C(int a, int handle, int value);
extern "C" int fn_80027A58(int a, Vec3* pos, void* rot, int seed, int count);
extern "C" u32 fn_802F2CFC(void);
extern "C" void fn_800A025C(Vec3* out, Mtx34* m, int row);
extern "C" void* fn_800D3D88(void* desc);
extern "C" void fn_800D3430(void* handle, int value);
void BoneSetMatrix(void* out, Mtx34* m);

// Link information handed to the effect system (message 0x20ee36).
struct TurretLinkInfo {
    s32   mtx[12];  // 0x00
    Vec3  pos;      // 0x30
    Vec3  delta;    // 0x3c
    void* owner;    // 0x48
    float f4c;      // 0x4c
    float f50;      // 0x50
    float f54;      // 0x54
    u32   id;       // 0x58
};

struct Vec4 {
    float x, y, z, w;
};

// Emitter definition used by the muzzle effects; the first 0x1c bytes are a
// position followed by a 4-float rotation filled in from the actor matrix.
struct EmitterDescFx {
    Vec3  pos;         // 0x00
    float rot[4];      // 0x0c
    s32   f1c;         // 0x1c
    s32   f20;         // 0x20
    float f24;         // 0x24
    s32   f28;         // 0x28
    void* f2c;         // 0x2c
    u8    f30;         // 0x30
    u8    pad31[3];    // 0x31
    void* f34;         // 0x34
    u8    f38;         // 0x38
    u8    pad39[3];    // 0x39
    void* f3c;         // 0x3c
    u8    f40;         // 0x40
    u8    pad41[0x2b]; // 0x41
    float f6c;         // 0x6c
    float f70;         // 0x70
    float f74;         // 0x74
    float f78;         // 0x78
    s32   f7c;         // 0x7c
    float f80;         // 0x80
    float f84;         // 0x84
    s32   f88;         // 0x88
    float f8c;         // 0x8c
    float f90;         // 0x90
    float f94;         // 0x94
    s32   f98;         // 0x98
    s32   f9c;         // 0x9c
    s32   fa0;         // 0xa0
    s32   fa4;         // 0xa4
    float fa8;         // 0xa8
    float fac;         // 0xac
    float fb0;         // 0xb0
    float fb4;         // 0xb4
    float fb8;         // 0xb8
    float fbc;         // 0xbc
    float fc0;         // 0xc0
    float fc4;         // 0xc4
    float fc8;         // 0xc8
    u8    fcc[8];      // 0xcc
    s32   fd4;         // 0xd4
    s32   fd8;         // 0xd8
    s32   fdc;         // 0xdc
    s32   fe0;         // 0xe0
    s32   fe4;         // 0xe4
};

// Anchor shared by the second muzzle emitter.
struct FxAnchor {
    Vec3  pos;   // 0x00
    float a;     // 0x0c
    float b;     // 0x10
    float c;     // 0x14
    float d;     // 0x18
    s32   e;     // 0x1c
    float f;     // 0x20
    float g;     // 0x24
    u8    pad28[0x08]; // 0x28
};

extern "C" FxAnchor lbl_8049E0C0;

// Message handler (0x1b83833f / 0x20ee36): reports the gun's open state and,
// on 0x20ee36, links the gun to its owner and kicks off the muzzle effect.
int TurretGunOpenMsg(int msg, void* data)
{
    Actor* self = (Actor*)data;
    TurretBeh* beh = self->beh;

    if (msg == 0x1B83833F) {
        if (beh->unk44 != 0) {
            u8 buf[0x10];
            SendMsg(self->id, 0xC05, buf);
        }
        return 1;
    }
    if (msg != 0x20EE36) {
        return 1;
    }

    Msg402 out;
    out.id = (u16)beh->fieldB4;
    SendMsg(self->id, 0x402, &out);

    Vec3 pos;
    pos.x = (float)out.mtx[9];
    pos.y = (float)out.mtx[10];
    pos.z = (float)out.mtx[11];

    Actor* owner = OwnerGetInfo(beh->ownerId);
    if (owner == 0) {
        return 1;
    }

    TurretLinkInfo info;
    info.owner = owner;
    info.f4c = lbl_804D9290;
    info.f50 = lbl_804D9290;
    info.f54 = lbl_804D33A0;
    info.id = self->id;
    LinkInfoClear(&info);
    info.pos = pos;
    info.delta.x = (float)owner->mat[9] - pos.x;
    info.delta.y = (float)owner->mat[10] - pos.y;
    info.delta.z = (float)owner->mat[11] - pos.z;

    int fx = fn_800F8A98(self->cls->f18);
    if (fx != -1) {
        fn_800F7F5C(lbl_804D931C, beh->f04, fx, 0x20EE36, self->id, &info);
        fn_800F7F5C(lbl_804D931C, beh->f04, fx, 0xE6B71BCC, self->id, &info);
        int handle = fn_800EB1B8(fx, 0);
        if (handle != -1) {
            fn_800F840C(beh->f04, handle, 9999);
        }
    }

    int seed = (int)(lbl_804D9328 * ((float)fn_802F2CFC() / lbl_804D9320) +
                     lbl_804D9324);
    Vec4 rot;
    rot.x = lbl_804D9290;
    rot.y = lbl_804D9330 * ((float)fn_802F2CFC() / lbl_804D9320) + lbl_804D932C;
    rot.z = lbl_804D925C;
    rot.w = lbl_804D9290;

    if (beh->fieldB0 == -1) {
        beh->fieldB0 = fn_80027A58(1, &pos, &rot, seed, 9);
    }
    return 1;
}

// Creates the two particle emitters that make up the muzzle flash.
void TurretMuzzleFx(Actor* self)
{
    TurretBeh* beh = self->beh;

    beh->fxTimer = lbl_804D925C;

    Mtx34 m;
    Vec3 trans;
    ActorGetMatrix(&m, self);
    fn_800A025C(&trans, &m, 2);
    fn_8009F7A8(&m, &trans);

    EmitterDescFx desc;
    desc.pos.x = m.m[9];
    desc.pos.y = m.m[10];
    desc.pos.z = m.m[11];
    BoneSetMatrix(&desc.rot[0], &m);

    desc.f1c = 0;
    desc.f20 = 2;
    desc.f24 = lbl_804D9290;
    desc.f28 = 1;
    desc.f2c = lbl_804D33A8;
    desc.f30 = 0xff;
    desc.f6c = lbl_804D9334;
    desc.f70 = lbl_804D9334;
    desc.f74 = lbl_804D9338;
    desc.f78 = lbl_804D933C;
    desc.f7c = 0x1e;
    desc.f80 = lbl_804D927C;
    desc.f84 = lbl_804D9290;
    desc.f88 = 0x10;
    desc.f8c = lbl_804D925C;
    desc.f90 = lbl_804D9290;
    desc.f94 = lbl_804D9340;
    desc.f98 = 0;
    desc.f9c = 0;
    desc.fa4 = 0;
    desc.fac = lbl_804D9304;
    desc.fb0 = lbl_804D925C;
    desc.fb4 = lbl_804D9344;
    desc.fb8 = lbl_804D925C;
    desc.fbc = lbl_804D925C;
    desc.fc0 = lbl_804D925C;
    desc.fc4 = lbl_804D9348;
    desc.fc8 = lbl_804D9344;
    desc.fcc[0] = 0x73;
    desc.fcc[1] = 0x73;
    desc.fcc[2] = 0x73;
    desc.fcc[3] = 0xff;
    desc.fcc[4] = 0x73;
    desc.fcc[5] = 0x73;
    desc.fcc[6] = 0x73;
    desc.fcc[7] = 0xff;
    desc.fd4 = -1;
    desc.fd8 = -1;
    desc.fdc = 0;
    desc.fe0 = 0;
    desc.fe4 = 0;

    beh->fxEmitter0 = (s32)fn_800D3D88(&desc);
    fn_800D3430((void*)beh->fxEmitter0, 1);

    EmitterDescFx desc2;
    desc2.pos.x = m.m[9];
    desc2.pos.y = m.m[10];
    desc2.pos.z = m.m[11];
    BoneSetMatrix(&desc2.rot[0], &m);

    desc2.f1c = 0;
    desc2.f20 = 2;
    desc2.f24 = lbl_804D9290;
    desc2.f28 = 3;
    desc2.f2c = lbl_804D33AC;
    desc2.f30 = 0xff;
    desc2.f34 = lbl_804D33B0;
    desc2.f38 = 0xff;
    desc2.f3c = lbl_804D33B4;
    desc2.f40 = 0xff;
    desc2.f6c = lbl_804D9334;
    desc2.f70 = lbl_804D9334;
    desc2.f74 = lbl_804D934C;
    desc2.f78 = lbl_804D9350;
    desc2.f7c = 0x28;
    desc2.f80 = lbl_804D92C8;
    desc2.f84 = lbl_804D9290;
    desc2.f88 = 0x10;
    desc2.f8c = lbl_804D925C;
    desc2.f90 = lbl_804D9290;
    desc2.f94 = lbl_804D9354;
    desc2.f98 = 0;
    desc2.f9c = 1;
    desc2.fa0 = (s32)&lbl_8049E0C0;

    lbl_8049E0C0.pos.x = m.m[9];
    lbl_8049E0C0.pos.y = m.m[10];
    lbl_8049E0C0.pos.z = m.m[11];
    lbl_8049E0C0.a = lbl_804D92E4;
    lbl_8049E0C0.b = lbl_804D925C;
    lbl_8049E0C0.c = lbl_804D925C;
    lbl_8049E0C0.d = lbl_804D925C;
    lbl_8049E0C0.e = 0;
    lbl_8049E0C0.f = lbl_804D9358;
    lbl_8049E0C0.g = lbl_804D925C;

    desc2.fa4 = 0;
    desc2.fac = lbl_804D925C;
    desc2.fb0 = lbl_804D925C;
    desc2.fb4 = lbl_804D925C;
    desc2.fb8 = lbl_804D925C;
    desc2.fbc = lbl_804D925C;
    desc2.fc0 = lbl_804D925C;
    desc2.fc4 = lbl_804D925C;
    desc2.fc8 = lbl_804D925C;
    desc2.fcc[0] = 200;
    desc2.fcc[1] = 200;
    desc2.fcc[2] = 0xff;
    desc2.fcc[3] = 0xff;
    desc2.fcc[4] = 200;
    desc2.fcc[5] = 200;
    desc2.fcc[6] = 0xff;
    desc2.fcc[7] = 0xff;
    desc2.fd4 = -1;
    desc2.fd8 = -1;
    desc2.fdc = 0;
    desc2.fe0 = 0;
    desc2.fe4 = 0;

    beh->fxEmitter1 = (s32)fn_800D3D88(&desc2);
    fn_800D3430((void*)beh->fxEmitter1, 1);
}

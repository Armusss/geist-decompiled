// u_80064FD4.cpp: the Player class implementation (0x80064FD4-0x80067FF8).
// Player update/movement, its pad input handling, the possession and action sub-updates, and the
// model/bone pose blending. Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Quat {
    float x, y, z, w;
};

struct Mtx33 {
    float m[3][3];
};

struct Mtx34 {
    float m[3][4];
};

// Orientation and position. The matrix helpers write a full 3x4 over rot, running into pos.
struct PlaceF {
    Mtx33 rot;
    Vec3 pos;
};

// A 0x48-byte pose record copied around by fn_80067558.
struct Pose48 {
    u32 w[18];
};

// Movement tuning record reached through Player+0x738.
struct MoveCfg {
    char pad00[0x30];
    float speed;        // 0x030
    float unk34;        // 0x034
    char pad38[4];
    float angle;        // 0x03C
};

// The object the player possesses, and the player's handle to it (0x26E4).
struct PossessedObject {
    char pad00[0x30];
    float unk30;        // 0x030
    float unk34;        // 0x034
    float unk38;        // 0x038
    float unk3C;        // 0x03C
    char pad40[0x14];
    float unk54;        // 0x054: extra push velocity
};

struct PossessedHost {
    char pad00[0x38];
    u32 flags38;        // 0x038 (type byte in the top 8 bits)
    char pad3C[0x48];
    PossessedObject* obj; // 0x084
};

// Host object queried by PlayerQueryPlace (its first field is the possessed object).
struct QueryObj {
    char pad00[0x38];
    int id;             // 0x038
};

struct PlaceQueryOwner {
    void* obj;          // 0x000
    char pad04[0x18];
    Vec3 rot;           // 0x01C
    Vec3 pos;           // 0x028
};

// Raw pad input state at Player+0x40 (six 32-bit samples).
struct PadState {
    int v[6];
};

struct Player {
    void Update(float dt);
    void UpdateMove(float dt);

    char pad0[0x24];
    u32 unk24;          // 0x024
    char pad28[0x18];
    PadState* pad;      // 0x040
    char pad44[0xC];
    u32 flags;          // 0x50
    char pad54[0x130];
    int model;          // 0x184
    char pad188[0x38];
    PlaceF place;       // 0x1C0
    Vec3 offset;        // 0x1F0
    Vec3 unk1FC;        // 0x1FC
    float unk208;       // 0x208
    float unk20C;       // 0x20C
    float unk210;       // 0x210
    float unk214;       // 0x214
    float unk218;       // 0x218
    float unk21C;       // 0x21C
    float unk220;       // 0x220
    float unk224;       // 0x224
    float unk228;       // 0x228
    float unk22C;       // 0x22C
    float unk230;       // 0x230
    float unk234;       // 0x234
    float unk238;       // 0x238
    float unk23C;       // 0x23C
    char pad240[0xC];
    float unk24C;       // 0x24C
    char pad250[4];
    u32 flags254;       // 0x254
    char pad258[0x10];
    float unk268;       // 0x268
    int unk26C;         // 0x26C
    float unk270;       // 0x270
    int unk274;         // 0x274
    float unk278;       // 0x278
    float unk27C;       // 0x27C
    float unk280;       // 0x280
    float unk284;       // 0x284
    float unk288;       // 0x288
    PlaceF anim;        // 0x28C
    void* unk2BC;       // 0x2BC
    Mtx34 unk2C0;       // 0x2C0
    char pad2F0[0x40];
    int unk330;         // 0x330
    int unk334;         // 0x334
    char pad338[0x58];
    float unk390;       // 0x390
    float unk394;       // 0x394
    float unk398;       // 0x398
    char pad39C[0x1EC];
    int unk588;         // 0x588
    char pad58C[0x14];
    float unk5A0;       // 0x5A0
    char pad5A4[0xD8];
    float unk67C;       // 0x67C
    float unk680;       // 0x680
    float unk684;       // 0x684
    Vec3 unk688;        // 0x688
    char pad694[0xA4];
    MoveCfg* unk738;    // 0x738
    int unk73C;         // 0x73C
    char pad740[0x64];
    int state;          // 0x7A4
    char pad7A8[0xC];
    u32 flags7B4;       // 0x7B4
    char pad7B8[0x28];
    int unk7E0;         // 0x7E0
    char pad7E4[0x14B0];
    int unk1C94;        // 0x1C94
    char pad1C98[0x10];
    int state1CA8;      // 0x1CA8
    char pad1CAC[4];
    int unk1CB0;        // 0x1CB0
    char pad1CB4[0xD4];
    int unk1D88;        // 0x1D88
    char pad1D8C[0x34];
    int unk1DC0;        // 0x1DC0
    int unk1DC4;        // 0x1DC4
    int unk1DC8;        // 0x1DC8
    int unk1DCC;        // 0x1DCC
    char pad1DD0[0x914];
    PossessedHost* unk26E4; // 0x26E4
};

// The standalone model/animation context set up by fn_80067D80.
struct RawQuat {
    u32 w[4];
};

struct ModelCtx {
    void* model;        // 0x000
    RawQuat rot0;       // 0x004
    RawQuat rot14;      // 0x014
    RawQuat rot24;      // 0x024
    float unk34;        // 0x034
    int unk38;          // 0x038
    Mtx34 mtx;          // 0x03C
    float unk6C;        // 0x06C
    float unk70;        // 0x070
    float unk74;        // 0x074
    float unk78;        // 0x078
    float unk7C;        // 0x07C
    float unk80;        // 0x080
    float unk84;        // 0x084
    float unk88;        // 0x088
    float unk8C;        // 0x08C
    float unk90;        // 0x090
    float unk94;        // 0x094
    float unk98;        // 0x098
    float unk9C;        // 0x09C
    float unkA0;        // 0x0A0
    float unkA4;        // 0x0A4
    float unkA8;        // 0x0A8
    float unkAC;        // 0x0AC
    float unkB0;        // 0x0B0
    int unkB4;          // 0x0B4
    float unkB8;        // 0x0B8
    u16 unkBC;          // 0x0BC
    char padBE[6];
    void* unkC4;        // 0x0C4
    char padC8[4];
    int unkCC;          // 0x0CC
    int unkD0;          // 0x0D0
    float unkD4;        // 0x0D4
    float unkD8;        // 0x0D8
    float unkDC;        // 0x0DC
    float unkE0;        // 0x0E0
    float unkE4;        // 0x0E4
    int unkE8;          // 0x0E8
    float unkEC;        // 0x0EC
    int unkF0;          // 0x0F0
    float unkF4;        // 0x0F4
    float unkF8;        // 0x0F8
    float unkFC;        // 0x0FC
    float unk100;       // 0x100
    char pad104[0x34];
    void* unk138;       // 0x138
    PlaceF place13C;    // 0x13C
};

struct Actor;

void MtxSetEuler(Mtx34* m, Vec3* rot);                     // 0x8009FAC0
void BoneSetMatrix(void* bone, Mtx34* m);                  // 0x800A0AC4
void BoneGetMatrix(Mtx34* m, void* bone);                  // 0x800A0EF0
void ActorGetMatrix(Mtx34* m, Actor* a);                   // 0x8009ED70
void PlacementToMatrix(Mtx34* m, PlaceF* p);               // 0x8009EF04
void RadToFixed(IVec3* out, Vec3* in);                     // 0x8016D220
void VecSwapXY2(IVec3* out, IVec3* in);                    // 0x8016D18C
void MakeVec(IVec3* out, int x, int y, int z);             // 0x8016D430
int PadPressed(int, int, int);                             // 0x800DC930

extern "C" {
int SendMsg(int to, int msg, void* data);
void C_QUATSlerp(const Quat* p, const Quat* q, Quat* r, float t); // 0x8030D4D0
void fn_8016D68C(Vec3* dst, Vec3* src);                    // 0x8016D68C
void fn_8016D648(Vec3* out, Vec3* a, Vec3* b);             // 0x8016D648
void fn_8016D6A8(void* p);                                 // 0x8016D6A8
void fn_800F94F8(int model, int frame);                    // 0x800F94F8
void* fn_800F9724(Mtx34* m, void* arg);                    // 0x800F9724
void fn_800F96E0(void* model);                             // 0x800F96E0
void fn_800F9578(void* model, void* p);                    // 0x800F9578
void* fn_800C9874(void* p, void* arg);                     // 0x800C9874
void fn_8009F028(void* out, void* vec);                    // 0x8009F028
void fn_800A0420(void* out, void* m);                      // 0x800A0420
int fn_80049634();                                         // 0x80049634
void fn_800C9830(void* p);                                 // 0x800C9830
void fn_800C97EC(void* p);                                 // 0x800C97EC
void fn_8001BA6C(void* p);                                 // 0x8001BA6C
float fn_800D8BE4(int, int, int, int);                     // 0x800D8BE4
void* fn_800D1BE0(void* p);                                // 0x800D1BE0
void fn_8006D76C(Actor* a);                                // 0x8006D76C
void fn_800A0168(Mtx34* out, Mtx33* m);                    // 0x800A0168
void fn_800A0030(Vec3* out, void* m, Vec3* v);             // 0x800A0030
float fn_800A1394(float x);                                // 0x800A1394
float fn_800A1418(float x);                                // 0x800A1418
void fn_800A02C0(Mtx34* m, Vec3* v, int col);              // 0x800A02C0
void fn_8009FC6C(Mtx34* out, Mtx34* a, Mtx34* b);          // 0x8009FC6C
void fn_8009F63C(void* out, void* src);                    // 0x8009F63C
float fn_800E1020(Vec3* v);                                // 0x800E1020
void fn_800E1098(float scale, Vec3* v);                    // 0x800E1098
int fn_8001ACD4(Player* p, Vec3* cur, Vec3* a, Vec3* b, Vec3* c,
                Vec3* vel, int flags, int (*cb)(void*, Vec3*),
                Vec3* out);                                // 0x8001ACD4
void fn_8009F7A8(Mtx33* out, Vec3* v);                     // 0x8009F7A8
}

// C++-mangled engine helpers.
int PlayerInHost(Actor* a);                                // 0x80049750
void ActorAim(int* out, Actor* a, int idx);                // 0x8009F700

// Other functions of this file (forward declarations; the base at 0x80064FD4 calls the later ones).
void PlayerPoseUpdate(Player* p, float dt);                // 0x800666B4
void PlayerGroundMove(Player* p, float dt);                // 0x800656EC
void PlayerAirMove(Player* p, float dt);                   // 0x80065F58
void PlayerAnimOffUpdate(Player* p, float dt);             // 0x80065F48
void PlayerModelUpdate(Player* p);                         // 0x8006766C
void PlayerStepMove(Player* p, float dt);                  // 0x800653B4
void PlayerExtraMove(Player* p, float dt);                 // 0x800652E0
void PlayerSetTurnPreset(Player* p, int mode);             // 0x800665B8
int PlayerQueryPlace(void* owner, Vec3* out);              // 0x8006660C
int Pose48Copy(Pose48* src, Pose48* dst);                  // 0x80067558
void PlayerClearBlend(Player* p);                          // 0x800675F0
void PlayerBeginBlend(Player* p, PlaceF* src, float f);    // 0x800675FC
void PlayerNoOp();                                         // 0x80067D60
void PlayerResetOffset(Player* p);                         // 0x80067D64
void ModelCtxInit(ModelCtx* p, Vec3* src, int unused, Actor* actor, void* arg); // 0x80067D80
void PlayerStaticInit();                                   // 0x80067FE4

// File-scope state, retail .sdata (writable tuning values).
static const Vec3 sPushVec = {0.0f, -1000.0f, 200.0f};  // 0x80342C08
static const Vec3 sZeroVec = {0.0f, 0.0f, 0.0f};        // 0x80342C14
static float sCfg00 = 5.0f;      // 0x804D2818
static float sCfg01 = 7.0f;      // 0x804D281C
static float sCfg02 = 8.0f;      // 0x804D2820
static float sCfg03 = 10.0f;     // 0x804D2824
static float sCfg04 = 10.0f;     // 0x804D2828
static float sCfg05 = 10.0f;     // 0x804D282C
static float sCfg06 = 200.0f;    // 0x804D2830
static float sCfg07 = 350.0f;    // 0x804D2834
static float sCfg08 = 60.0f;     // 0x804D2838
static float sCfg09 = 80.0f;     // 0x804D283C
static float sCfg10 = 50.0f;     // 0x804D2840
static float sCfg11 = 50.0f;     // 0x804D2844
static float sCfg12 = 0.01f;     // 0x804D2848
static float sCfg13 = 0.005f;    // 0x804D284C
static float sCfg14 = 0.5f;      // 0x804D2850
static float sCfg15 = 7.0f;      // 0x804D2854
static float sCfg16 = 0.5f;      // 0x804D2858
static float sCfg17 = -3000.0f;  // 0x804D285C
static float sCfg18 = 0.00001f;  // 0x804D2860
static float sCfg19 = 0.0004f;   // 0x804D2864
static float sCfg20 = 85.0f;     // 0x804D2868
static float sCfg21 = 0.00015f;  // 0x804D286C
static float sCfg22 = 1000.0f;   // 0x804D2870
static float sCfg23 = 500.0f;    // 0x804D2874
static u32 sFlagPair[2] = {1, 0}; // 0x804D2878

// Retail .sbss state.
static u32 sUnk44B0;     // 0x804D44B0 (air control enable flag)
static float sUnk44B4;   // 0x804D44B4
static float sUnk44B8;   // 0x804D44B8
static float sUnk44BC;   // 0x804D44BC
static float sUnk44C0;   // 0x804D44C0
static float sCfg07Copy; // 0x804D44C4
static float sCfg09Copy; // 0x804D44C8
static u32 sLiftActive;  // 0x804D44CC
static float sUnk44D0;   // 0x804D44D0
static float sUnk44D4;   // 0x804D44D4
static u8 sUnk44D8;      // 0x804D44D8
static u8 sPad44D9[3];
static float sUnk44DC;   // 0x804D44DC
static u8 sUnk44E0;      // 0x804D44E0
static u8 sPad44E1[3];
static float sAngleX;    // 0x804D44E4
static float sAngleZ;    // 0x804D44E8

void Player::Update(float dt) {
    Vec3 v;
    Quat qOut;
    Quat qTgt;
    Quat qCur;
    PlaceF p;
    float px, py, pz;
    float ox, oy, oz, ratio, inv;

    if (state != 0xC && !(flags & 0x8000)) {
        PlayerModelUpdate(this);
    }
    unk1FC.x = 0.0f;
    unk1FC.y = 0.0f;
    unk1FC.z = 0.0f;
    fn_8016D68C(&v, &unk688);
    if (!(flags & 4)) {
        PlayerPoseUpdate(this, dt);
        PlayerGroundMove(this, dt);
        PlayerAirMove(this, dt);
    }
    fn_8016D648(&v, &v, &offset);
    if (!(flags & 0x60000)) {
        px = place.pos.x;
        py = place.pos.y;
        pz = place.pos.z;
        MtxSetEuler((Mtx34*)&place.rot, &v);
        place.pos.x = px;
        place.pos.y = py;
        place.pos.z = pz;
    }
    fn_800F94F8(model, (int)unk238);
    float v238 = unk238;
    if (v238 < unk23C) {
        unk238 = dt * unk278 + v238;
        if (unk238 > unk23C) {
            unk238 = unk23C;
        }
    } else {
        unk238 = v238 - dt * unk278;
        if (unk238 < unk23C) {
            unk238 = unk23C;
        }
    }
    unk23C = 0.0f;
    float limit = unk284;
    if (limit) {
        float t = unk288 + dt;
        t = (t < limit) ? t : limit;
        unk288 = t;
        if (unk288 == unk284) {
            unk284 = 0.0f;
        } else {
            p = place;
            ratio = unk288 / unk284;
            inv = 1.0f - ratio;
            ox = anim.pos.x * inv + p.pos.x * ratio;
            oy = anim.pos.y * inv + p.pos.y * ratio;
            oz = anim.pos.z * inv + p.pos.z * ratio;
            BoneSetMatrix(&qCur, (Mtx34*)&p.rot);
            BoneSetMatrix(&qTgt, (Mtx34*)&anim.rot);
            C_QUATSlerp(&qTgt, &qCur, &qOut, ratio);
            BoneGetMatrix((Mtx34*)&p.rot, &qOut);
            p.pos.x = ox;
            p.pos.y = oy;
            p.pos.z = oz;
            place = p;
        }
    }
}

// Extra external-push velocity, then the possession movement update.
void PlayerExtraMove(Player* p, float dt) {
    Vec3 v;
    Mtx34 m;

    if (fn_80049634() != 0) {
        if (p->unk26E4->obj->unk54 != 0.0f) {
            fn_800A0168(&m, &p->place.rot);
            float f = p->unk26E4->obj->unk54;
            v.x = 0.0f;
            v.y = f;
            v.z = 0.0f;
            fn_800A0030(&v, &m, &v);
            p->unk1FC.x += v.x;
            p->unk1FC.y += v.y;
            p->unk1FC.z += v.z;
        }
    }
    PlayerStepMove(p, dt);
}

// Stepping-stone / climbing move: while in state 0x1CA8 == 1, move relative to the pad and
// tilt the host placement, else turn the effect off.
void PlayerStepMove(Player* p, float dt) {
    Vec3 neg;
    Vec3 world;
    Vec3 axis2;
    Vec3 axis;
    PlaceF out;
    Mtx34 m34;
    Mtx34 tmp;
    Mtx34 rotZ;
    Mtx34 rotX;

    if (p->state1CA8 == 1) {
        Vec3 off = {0.0f, 0.0f, 0.0f};
        off.y = sCfg17;
        if (sLiftActive == 0) {
            fn_800C9830((void*)p->unk2BC);
            sLiftActive = 1;
            fn_8001BA6C(&p->unk24);
        }
        if (PadPressed(0, 0, 10)) {
            sCfg17 += dt;
        } else if (PadPressed(0, 0, 11)) {
            sCfg17 -= dt;
        }
        sAngleX = 0.1f * (dt * fn_800D8BE4(0, 0, 3, 0)) + sAngleX;
        axis.z = 0.0f;
        axis.y = 0.0f;
        axis.x = 1.0f;
        fn_800A02C0(&rotX, &axis, 0);
        axis.z = fn_800A1394(sAngleX);
        axis.y = fn_800A1418(sAngleX);
        axis.x = 0.0f;
        fn_800A02C0(&rotX, &axis, 1);
        axis.z = fn_800A1418(sAngleX);
        axis.y = -fn_800A1394(sAngleX);
        axis.x = 0.0f;
        fn_800A02C0(&rotX, &axis, 2);

        sAngleZ = 0.1f * (dt * fn_800D8BE4(0, 0, 0, 0)) + sAngleZ;
        axis2.z = 0.0f;
        axis2.y = fn_800A1394(sAngleZ);
        axis2.x = fn_800A1418(sAngleZ);
        fn_800A02C0(&rotZ, &axis2, 0);
        axis2.z = 0.0f;
        axis2.y = fn_800A1418(sAngleZ);
        axis2.x = -fn_800A1394(sAngleZ);
        fn_800A02C0(&rotZ, &axis2, 1);
        axis2.x = 0.0f;
        axis2.y = 0.0f;
        axis2.z = 1.0f;
        fn_800A02C0(&rotZ, &axis2, 2);

        fn_8009FC6C(&rotZ, &rotZ, &rotX);
        fn_800A0030(&off, &rotZ, &off);
        fn_8009F63C(&tmp, p);
        ActorGetMatrix(&m34, (Actor*)&tmp);
        fn_800A0030(&world, &m34, &off);
        neg.x = -world.x;
        neg.y = -world.y;
        neg.z = -world.z;
        fn_8009F7A8(&out.rot, &neg);
        out.pos.x = p->unk67C + world.x;
        out.pos.y = p->unk680 + world.y;
        out.pos.z = p->unk684 + world.z;
        PlacementToMatrix(&p->unk2C0, &out);
    } else if (sLiftActive != 0) {
        fn_800C97EC((void*)p->unk2BC);
        sLiftActive = 0;
        fn_8001BA6C(0);
    }
}

// Ground movement: picks a speed target from the stick and the state, moves the player
// forward and clamps it against the walkable bounds.
void PlayerGroundMove(Player* p, float dt) {
    Vec3 dir;
    fn_8016D6A8(&dir);

    if (p->state1CA8 == 1) {
        return;
    }

    PossessedHost* host = p->unk26E4;
    float speed;
    int limit;
    if (host != 0 && (host->flags38 >> 24) == 1) {
        PossessedObject* obj = host->obj;
        float ratio = obj->unk38 / 4096.0f;
        speed = 360.0f * ratio;
        limit = (int)obj->unk34;
        if (limit < 0) {
            limit = (int)p->unk738->angle;
        }
        if (limit > 90) {
            limit = 90;
        }
        if (speed < 0.0f) {
            speed = p->unk738->speed;
        }
    } else {
        speed = p->unk738->speed;
        limit = (int)p->unk738->angle;
    }

    if (p->unk1C94 != 0) {
        u32 bit = p->flags & 0x100000;
        if (p->unk1DCC != 0) {
            speed *= 0.5f;
        } else if (fn_800D1BE0(p) != 0) {
            int go = 0;
            if (p->unk1DC0 != 0) {
                if (bit == 0) {
                    go = 1;
                } else if (p->unk1DC4 != 0) {
                    go = 1;
                }
            } else if (p->unk1DC4 != 0 && bit != 0) {
                go = 1;
            }
            if (go && p->unk1DC8 != 0 && p->unk1D88 != 0) {
                speed *= 0.5f;
            }
        }
    }

    if (p->unk394 == 0.0f) {
        p->unk24C = 0.0f;
    } else if (p->unk73C != 3) {
        p->unk24C = 0.0f;
    } else if (p->unk390 != 0.0f) {
        p->unk24C = 0.0f;
    } else {
        p->unk24C += dt;
    }

    float mag = p->unk398;
    if (mag < 0.0f) {
        if (p->unk21C > 0.0f) {
            p->unk228 = 0.00015f;
            p->unk22C = 1000.0f;
            p->unk230 = sUnk44D0;
            p->unk234 = 500.0f;
        }
    } else if (p->unk21C < 0.0f) {
        p->unk228 = 0.00015f;
        p->unk22C = 1000.0f;
        p->unk230 = sUnk44D0;
        p->unk234 = 500.0f;
    }

    switch (p->unk7E0) {
    case 1:
        mag *= sUnk44BC;
        break;
    case 2:
        mag *= sUnk44C0;
        break;
    case 3:
        mag *= 0.4f;
        break;
    case 4:
        mag *= sUnk44B4;
        break;
    case 5:
        mag *= sUnk44B8;
        break;
    }

    if (p->unk334 != 0) {
        float a = p->unk394;
        if (p->flags254 & 1) {
            a = -a;
        }
        if (a < -0.6f) {
            mag = (0.6f + a) / 0.39999998f;
        } else if (a > 0.6f) {
            mag = (a - 0.6f) / 0.39999998f;
        } else {
            mag = 0.0f;
        }
        mag += p->unk5A0;
    }
    p->unk21C = mag;

    float target;
    if (p->unk330 != 0 && p->unk398 == 0.0f) {
        float a = p->unk394;
        if (p->flags254 & 1) {
            a = -a;
        }
        dir.x = a * (float)limit;
        p->offset.x = (dir.x - p->offset.x) * (p->unk280 * dt) + p->offset.x;
    } else if (p->unk588 == 8) {
        dir.x = 0.0f;
        p->offset.x = (dir.x - p->offset.x) * (p->unk280 * dt) + p->offset.x;
    } else {
        if (mag - p->unk5A0 != 0.0f) {
            fn_8006D76C((Actor*)p);
            p->unk24C = 0.0f;
        }

        target = speed * mag;
        if (target == 0.0f) {
            if (p->unk214 == 0.0f) {
                p->unk218 = p->unk738->unk34;
                p->unk228 = 0.00015f;
                p->unk22C = 1000.0f;
                p->unk230 = sUnk44D0;
                p->unk234 = 500.0f;
            } else {
                p->unk218 = p->unk220;
            }
        } else if (p->unk224 == 0.0f) {
            p->unk218 = p->unk738->unk34;
            p->unk228 = 0.00015f;
            p->unk22C = 1000.0f;
            p->unk230 = sUnk44D0;
            p->unk234 = 500.0f;
        }

        if (p->unk22C > 0.0f) {
            if (dt > p->unk22C) {
                float old = p->unk22C;
                p->unk22C = 0.0f;
                p->unk234 -= dt - old;
            } else {
                p->unk22C -= dt;
            }
            p->unk218 = 0.00015f;
        } else if (p->unk234 > 0.0f) {
            p->unk234 -= dt;
            p->unk218 = sUnk44D0;
        } else {
            p->unk218 += (sCfg18 / 1000.0f) * dt;
            if (p->unk218 > 0.0004f) {
                p->unk218 = 0.0004f;
            }
        }

        if (p->unk214 != 0.0f) {
            float ratio = p->unk214 / 85.0f;
            p->unk220 = ratio < 0.0f ? -ratio : ratio;
        }

        float cur = p->unk214;
        float diff = target - cur;
        if (cur < target) {
            float acc = (p->unk218 * dt) * (diff * 7.0f + 0.5f);
            p->unk214 = cur + acc;
            if (p->unk214 >= target) {
                p->unk214 = target;
            }
        } else if (cur > target) {
            float acc = (p->unk218 * dt) * (diff * 7.0f - 0.5f);
            p->unk214 = cur + acc;
            if (p->unk214 <= target) {
                p->unk214 = target;
            }
        }
        p->offset.x = p->unk214 * dt + p->offset.x;
    }

    if (p->unk274 != 0 && p->unk1CB0 == 0) {
        int hit = 0;
        QueryObj* q = (QueryObj*)fn_800D1BE0(p);
        if (q != 0) {
            u32 type = ((u32)q->id) >> 24;
            if (type == 2 || type == 0x13) {
                hit = 1;
            }
        }
        if ((q == 0 || hit == 0) && p->unk334 == 0 && p->unk330 == 0) {
            float lim;
            if (p->unk588 != 9) {
                lim = p->unk270;
            } else {
                lim = p->unk270 * 0.5f;
            }
            if (p->unk24C >= lim) {
                float edge = (float)p->unk26C;
                float pos = p->offset.x;
                float absTurn = p->unk394 < 0.0f ? -p->unk394 : p->unk394;
                float push = absTurn - 0.5f;
                if (pos < edge) {
                    float step;
                    if (p->unk588 != 9) {
                        step = push * (p->unk268 * dt);
                    } else {
                        step = push * ((3.0f * p->unk268) * 0.5f * dt);
                    }
                    p->offset.x = pos + step;
                    if (p->offset.x > edge) {
                        p->offset.x = edge;
                    }
                } else if (pos > edge) {
                    float step;
                    if (p->unk588 != 9) {
                        step = push * (p->unk268 * dt);
                    } else {
                        step = push * ((3.0f * p->unk268) * 0.5f * dt);
                    }
                    p->offset.x = pos - step;
                    if (p->offset.x < edge) {
                        p->offset.x = edge;
                    }
                }
            }
        }
    }

    float bound = (float)limit;
    if (p->offset.x > bound) {
        p->offset.x = bound;
    } else if (p->offset.x < -bound) {
        p->offset.x = -bound;
    }
    if (p->offset.y > 800.0f) {
        p->offset.y = 800.0f;
    } else if (p->offset.y < -800.0f) {
        p->offset.y = -800.0f;
    }
    p->unk224 = target;
}

void PlayerAnimOffUpdate(Player* p, float dt) {
    p->offset.x += dt;
}

// Air control / gravity blend. Moves the player along the air-control vector and
// blends the horizontal speed toward its target using the stick deflection.
void PlayerAirMove(Player* p, float dt) {
    Vec3 pos = sPushVec;
    float len = fn_800E1020(&pos);

    if (PlayerInHost((Actor*)p) != 0) {
        p->unk208 = 0.0f;
        p->flags254 &= ~2u;
        return;
    }

    float speed;
    if ((p->flags7B4 & 0x100000) != 0) {
        speed = 1.0f;
    } else {
        speed = 0.0f;
    }
    if (sUnk44B0 == 0) {
        speed = 0.0f;
        p->unk208 = 0.0f;
    } else if (speed > 0.61f) {
        speed = (0.0025f * dt) * ((speed - 0.61f) / 0.39f - p->unk208);
    } else {
        speed = (0.0075f * dt) * (0.0f - p->unk208);
    }
    p->unk208 += speed;

    if (p->unk208 != 0.0f) {
        Mtx34 rotA;
        Mtx34 rotB;
        Mtx34 mtx;
        Vec3 axis;
        Vec3 axis2;
        Vec3 world;
        Vec3 vel;
        Vec3 cur;
        Vec3 delta;
        Vec3 fwd0;
        Vec3 fwd1;
        Vec3 fwd2;
        Vec3 res[2];
        IVec3 aim;
        PlaceF tmp;
        float ang;
        float s;
        float c;

        ang = -p->offset.x;
        axis.x = 1.0f;
        axis.y = 0.0f;
        axis.z = 0.0f;
        fn_800A02C0(&rotA, &axis, 0);
        s = fn_800A1394(ang);
        c = fn_800A1418(ang);
        axis.x = 0.0f;
        axis.y = c;
        axis.z = s;
        fn_800A02C0(&rotA, &axis, 1);
        s = fn_800A1418(ang);
        c = fn_800A1394(ang);
        axis.x = 0.0f;
        axis.y = -c;
        axis.z = s;
        fn_800A02C0(&rotA, &axis, 2);

        p->unk210 = 0.0043945313f * (1.7578125f * p->unk390 - p->unk210) + p->unk210;

        s = fn_800A1394(p->unk210);
        c = fn_800A1418(p->unk210);
        axis2.x = c;
        axis2.y = s;
        axis2.z = 0.0f;
        fn_800A02C0(&rotB, &axis2, 0);
        s = fn_800A1418(p->unk210);
        c = fn_800A1394(p->unk210);
        axis2.x = -c;
        axis2.y = s;
        axis2.z = 0.0f;
        fn_800A02C0(&rotB, &axis2, 1);
        axis2.x = 0.0f;
        axis2.y = 0.0f;
        axis2.z = 1.0f;
        fn_800A02C0(&rotB, &axis2, 2);

        fn_8009FC6C(&rotB, &rotB, &rotA);
        fn_800A0030(&pos, &rotB, &pos);
        fn_8009F63C(&tmp, p);
        ActorGetMatrix(&mtx, (Actor*)&tmp);
        fn_800A0030(&world, &mtx, &pos);

        vel.x = world.x * p->unk208;
        vel.y = world.y * p->unk208;
        vel.z = world.z * p->unk208;

        cur.x = p->place.pos.x;
        cur.y = p->place.pos.y;
        cur.z = p->place.pos.z;

        int d0 = p->pad->v[0] - p->pad->v[3];
        int d1 = p->pad->v[4] - p->pad->v[1];
        float f0 = (float)d0;
        float f1 = (float)d1;
        float scale = 0.5f * ((f0 < f1) ? f1 : f0);

        ActorAim((int*)&aim, (Actor*)p, 0);
        fwd0.x = (float)aim.x;
        fwd0.y = (float)aim.y;
        fwd0.z = (float)aim.z;
        fn_800E1098(scale, &fwd0);

        ActorAim((int*)&aim, (Actor*)p, 1);
        fwd1.x = (float)aim.x;
        fwd1.y = (float)aim.y;
        fwd1.z = (float)aim.z;
        fn_800E1098(scale, &fwd1);

        ActorAim((int*)&aim, (Actor*)p, 2);
        fwd2.x = (float)aim.x;
        fwd2.y = (float)aim.y;
        fwd2.z = (float)aim.z;
        fn_800E1098(0.25f * (float)(p->pad->v[5] - p->pad->v[2]), &fwd2);

        if (fn_8001ACD4(p, &cur, &fwd0, &fwd1, &fwd2, &vel, 0x1000,
                        PlayerQueryPlace, res) == 1) {
            delta.x = res[1].x - p->place.pos.x;
            delta.y = res[1].y - p->place.pos.y;
            delta.z = res[1].z - p->place.pos.z;
            p->unk208 = fn_800E1020(&delta) / len;
        }

        p->unk1FC.x += vel.x;
        p->unk1FC.y += vel.y;
        p->unk1FC.z += vel.z;

        if (p->unk208 > 0.005f) {
            p->flags254 |= 2u;
            p->unk27C = 15.380859f;
            p->unk278 = 0.3f;
        } else {
            p->flags254 &= ~2u;
            p->unk27C = 2.1972656f;
            p->unk278 = 0.1f;
            p->flags7B4 &= ~0x100000u;
        }
    } else {
        p->flags254 &= ~2u;
    }

    if (p->unk208 > 0.0f) {
        if (p->unk208 > 0.8f) {
            p->unk20C = 0.0f;
        } else {
            p->unk20C = 1.0f - p->unk208;
        }
    } else {
        p->unk20C = 1.0f;
    }
}

// Turn-rate preset for a movement mode (1 = fast, else normal).
void PlayerSetTurnPreset(Player* p, int mode) {
    if (mode == 1) {
        p->flags254 |= 2;
        p->unk27C = 15.380859f;
        p->unk278 = 0.3f;
    } else {
        p->flags254 &= ~2;
        p->unk27C = 2.1972656f;
        p->unk278 = 0.1f;
        p->flags7B4 &= ~0x100000;
    }
}

// Queries the current placement from the possessed object; returns 0 when unavailable.
int PlayerQueryPlace(void* owner, Vec3* out) {
    PlaceQueryOwner* self = (PlaceQueryOwner*)owner;
    QueryObj* obj = (QueryObj*)self->obj;
    struct Msg {
        char pad[0x18];
        u32 flags;
    } msg;

    if (obj != 0) {
        if (SendMsg(obj->id, 0x400, &msg) != 0) {
            if ((msg.flags & 1) == 0) {
                return 0;
            }
        } else {
            return 0;
        }
    }
    out->x = self->pos.x;
    out->y = self->pos.y;
    out->z = self->pos.z;
    out[1].x = self->rot.x;
    out[1].y = self->rot.y;
    out[1].z = self->rot.z;
    return 1;
}

// Stance / pose blend update. TODO: full body.
void PlayerPoseUpdate(Player* p, float dt) {
    (void)p;
    (void)dt;
}

// Copies a 0x48-byte pose record.
int Pose48Copy(Pose48* src, Pose48* dst) {
    dst->w[0] = src->w[0];
    dst->w[1] = src->w[1];
    dst->w[2] = src->w[2];
    dst->w[3] = src->w[3];
    dst->w[4] = src->w[4];
    dst->w[5] = src->w[5];
    dst->w[6] = src->w[6];
    dst->w[7] = src->w[7];
    dst->w[8] = src->w[8];
    dst->w[9] = src->w[9];
    dst->w[10] = src->w[10];
    dst->w[11] = src->w[11];
    dst->w[12] = src->w[12];
    dst->w[13] = src->w[13];
    dst->w[14] = src->w[14];
    dst->w[15] = src->w[15];
    dst->w[16] = src->w[16];
    dst->w[17] = src->w[17];
    return 1;
}

void PlayerClearBlend(Player* p) {
    p->unk284 = 0.0f;
}

// Starts a 0x284/0x288 blend from `src` (the animation placement).
void PlayerBeginBlend(Player* p, PlaceF* src, float f) {
    p->unk284 = f;
    p->unk288 = 0.0f;
    p->anim = *src;
}

// Model / pose setup. TODO: full body.
void PlayerModelUpdate(Player* p) {
    (void)p;
}

void PlayerNoOp() {}

void PlayerResetOffset(Player* p) {
    p->unk208 = 0.0f;
    p->offset.y = 0.0f;
    p->offset.x = -2.4609375f;
    p->offset.z = 0.0f;
}

// Initializes the player's model and movement state.
void ModelCtxInit(ModelCtx* p, Vec3* src, int unused, Actor* actor, void* arg) {
    IVec3 fixed;
    IVec3 swapped;
    IVec3 vec;
    Mtx34 mtx;

    RadToFixed(&fixed, (Vec3*)((char*)src + 0x84));
    VecSwapXY2(&swapped, &fixed);
    MakeVec(&vec, swapped.z, swapped.y, swapped.x);
    fn_8009F028(&mtx, &vec);
    fn_800A0420(&p->rot0, &mtx);
    p->rot24.w[0] = p->rot0.w[0];
    p->rot24.w[1] = p->rot0.w[1];
    p->rot24.w[2] = p->rot0.w[2];
    p->rot24.w[3] = p->rot0.w[3];
    p->rot14.w[0] = p->rot0.w[0];
    p->rot14.w[1] = p->rot0.w[1];
    p->rot14.w[2] = p->rot0.w[2];
    p->rot14.w[3] = p->rot0.w[3];
    p->unk34 = 0.0f;
    p->unk38 = 1;
    p->unk6C = -2.4609375f;
    p->unk70 = 0.0f;
    p->unk74 = 0.0f;
    ActorGetMatrix(&p->mtx, actor);
    p->unk78 = 0.0f;
    p->unk7C = 0.0f;
    p->unk80 = 0.0f;
    p->unkB8 = 0.0f;
    p->unkBC = 0;
    p->unk90 = 0.0f;
    p->unk94 = 0.0f;
    p->unk98 = 0.0f;
    p->unk9C = 1.0f;
    p->unkA0 = 0.0f;
    p->unkA4 = sCfg21;
    p->unkA8 = sCfg22;
    p->unkAC = sUnk44D0;
    p->unkB0 = sCfg23;
    p->unkC4 = arg;
    p->unk100 = 0.0f;
    p->model = fn_800F9724(&p->mtx, arg);
    p->place13C = *(PlaceF*)actor;
    p->unk138 = fn_800C9874(&p->place13C, arg);
    fn_800F96E0(p->model);
    fn_800F9578(p->model, &p->unk78);
    p->unkD4 = 0.0f;
    p->unkD8 = 0.0f;
    p->unkDC = 0.0f;
    p->unkE0 = 0.0f;
    p->unkEC = 1000.0f;
    p->unkE4 = 0.06f;
    p->unkF0 = 1;
    p->unkCC = 0;
    p->unkFC = 0.00017578126f;
    p->unk84 = 0.0f;
    p->unk88 = 0.0f;
    p->unk8C = 0.0f;
    p->unkD0 = 0;
    p->unkE8 = -2;
    p->unkF8 = 2.1972656f;
    p->unkF4 = 0.1f;
    sUnk44B4 = 0.35f;
    sUnk44B8 = 0.21f;
    sUnk44BC = 0.065f;
    sUnk44C0 = 0.017f;
}

// Static initializer for the two tuning globals.
void PlayerStaticInit() {
    sCfg07Copy = 350.0f;
    sCfg09Copy = 80.0f;
}

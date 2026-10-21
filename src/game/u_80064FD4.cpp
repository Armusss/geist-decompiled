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

struct Player {
    void Update(float dt);
    void UpdateMove(float dt);

    char pad0[0x24];
    u32 unk24;          // 0x024
    char pad28[0x28];
    u32 flags;          // 0x50
    char pad54[0x130];
    int model;          // 0x184
    char pad188[0x38];
    PlaceF place;       // 0x1C0
    Vec3 offset;        // 0x1F0
    Vec3 unk1FC;        // 0x1FC
    float unk208;       // 0x208
    char pad20C[0x2C];
    float unk238;       // 0x238
    float unk23C;       // 0x23C
    char pad240[0x14];
    u32 flags254;       // 0x254
    char pad258[0x20];
    float unk278;       // 0x278
    float unk27C;       // 0x27C
    float unk280;       // 0x280
    float unk284;       // 0x284
    float unk288;       // 0x288
    PlaceF anim;        // 0x28C
    void* unk2BC;       // 0x2BC
    Mtx34 unk2C0;       // 0x2C0
    char pad2F0[0x38C];
    float unk67C;       // 0x67C
    float unk680;       // 0x680
    float unk684;       // 0x684
    Vec3 unk688;        // 0x688
    char pad694[0x110];
    int state;          // 0x7A4
    char pad7A8[0xC];
    u32 flags7B4;       // 0x7B4
    char pad7B8[0x14F0];
    int state1CA8;      // 0x1CA8
    char pad1CAC[0xA38];
    void* unk26E4;      // 0x26E4
};

// The standalone model/animation context set up by fn_80067D80.
struct ModelCtx {
    void* model;        // 0x000
    Quat rot0;          // 0x004
    Quat rot14;         // 0x014
    Quat rot24;         // 0x024
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
void fn_800666B4(Player* p, float dt);                     // 0x800666B4
void fn_800656EC(Player* p, float dt);                     // 0x800656EC
void fn_80065F58(Player* p, float dt);                     // 0x80065F58
void fn_80065F48(Player* p, float dt);                     // 0x80065F48
void fn_8006766C(Player* p);                               // 0x8006766C
void fn_800653B4(Player* p, float dt);                     // 0x800653B4
void fn_800652E0(Player* p, float dt);                     // 0x800652E0
int fn_80049634();                                         // 0x80049634
void fn_800C9830(void* p);                                 // 0x800C9830
void fn_800C97EC(void* p);                                 // 0x800C97EC
void fn_8001BA6C(void* p);                                 // 0x8001BA6C
float fn_800D8BE4(int, int, int, int);                     // 0x800D8BE4
void fn_800A0168(Mtx33* out, Mtx33* m);                    // 0x800A0168
void fn_800A0030(Vec3* out, void* m, Vec3* v);             // 0x800A0030
float fn_800A1394(float x);                                // 0x800A1394
float fn_800A1418(float x);                                // 0x800A1418
void fn_800A02C0(Mtx33* m, Vec3* v, int col);              // 0x800A02C0
void fn_8009FC6C(Mtx33* out, Mtx33* a, Mtx33* b);          // 0x8009FC6C
void fn_8009F63C(Mtx33* out, void* src);                   // 0x8009F63C
void fn_8009F7A8(Mtx33* out, Vec3* v);                     // 0x8009F7A8
void fn_800665B8(Player* p, int mode);                     // 0x800665B8
int fn_8006660C(void* owner, Vec3* out);                   // 0x8006660C
int fn_80067558(Pose48* dst, Pose48* src);                 // 0x80067558
void fn_800675F0(Player* p);                               // 0x800675F0
void fn_800675FC(Player* p, PlaceF* src, float f);         // 0x800675FC
void fn_80067D60();                                        // 0x80067D60
void fn_80067D64(Player* p);                               // 0x80067D64
void fn_80067D80(ModelCtx* p, Vec3* src, int unused, Actor* actor, void* arg); // 0x80067D80
void fn_80067FE4();                                        // 0x80067FE4
}

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
static float sUnk44B0;   // 0x804D44B0
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
        fn_8006766C(this);
    }
    unk1FC.x = 0.0f;
    unk1FC.y = 0.0f;
    unk1FC.z = 0.0f;
    fn_8016D68C(&v, &unk688);
    if (!(flags & 4)) {
        fn_800666B4(this, dt);
        fn_800656EC(this, dt);
        fn_80065F58(this, dt);
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
void fn_800652E0(Player* p, float dt) {
    Vec3 v;
    Mtx33 m;

    if (fn_80049634() != 0) {
        float f = *(float*)(*(int*)((char*)p->unk26E4 + 0x84) + 0x54);
        if (f != 0.0f) {
            fn_800A0168(&m, &p->place.rot);
            v.x = 0.0f;
            v.y = f;
            v.z = 0.0f;
            fn_800A0030(&v, &m, &v);
            p->unk1FC.x += v.x;
            p->unk1FC.y += v.y;
            p->unk1FC.z += v.z;
        }
    }
    fn_800653B4(p, dt);
}

// Stepping-stone / climbing move: while in state 0x1CA8 == 1, move relative to the pad and
// tilt the host placement, else turn the effect off.
void fn_800653B4(Player* p, float dt) {
    Vec3 neg;
    Vec3 world;
    Vec3 off;
    Vec3 axis2;
    Vec3 axis;
    PlaceF out;
    Mtx34 m34;
    Mtx33 tmp;
    Mtx33 rotZ;
    Mtx33 rotX;

    if (p->state1CA8 == 1) {
        off = sZeroVec;
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
// forward and clamps it against the walkable bounds. TODO: full body.
void fn_800656EC(Player* p, float dt) {
    (void)p;
    (void)dt;
}

void fn_80065F48(Player* p, float dt) {
    p->offset.x += dt;
}

// Air control / gravity blend. TODO: full body.
void fn_80065F58(Player* p, float dt) {
    (void)p;
    (void)dt;
}

// Turn-rate preset for a movement mode (1 = fast, else normal).
void fn_800665B8(Player* p, int mode) {
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
int fn_8006660C(void* owner, Vec3* out) {
    char* self = (char*)owner;
    void* obj = *(void**)self;
    struct Msg {
        char pad[0x18];
        u32 flags;
    } msg;

    if (obj != 0) {
        if (SendMsg(*(int*)((char*)obj + 0x38), 0x400, &msg) != 0) {
            if ((msg.flags & 1) == 0) {
                return 0;
            }
        } else {
            return 0;
        }
    }
    out->x = *(float*)((char*)self + 0x28);
    out->y = *(float*)((char*)self + 0x2C);
    out->z = *(float*)((char*)self + 0x30);
    out[1].x = *(float*)((char*)self + 0x1C);
    out[1].y = *(float*)((char*)self + 0x20);
    out[1].z = *(float*)((char*)self + 0x24);
    return 1;
}

// Stance / pose blend update. TODO: full body.
void fn_800666B4(Player* p, float dt) {
    (void)p;
    (void)dt;
}

// Copies a 0x48-byte pose record.
int fn_80067558(Pose48* src, Pose48* dst) {
    for (int i = 0; i < 18; i++) {
        dst->w[i] = src->w[i];
    }
    return 1;
}

void fn_800675F0(Player* p) {
    p->unk284 = 0.0f;
}

// Starts a 0x284/0x288 blend from `src` (the animation placement).
void fn_800675FC(Player* p, PlaceF* src, float f) {
    p->unk284 = f;
    p->unk288 = 0.0f;
    p->anim = *src;
}

// Model / pose setup. TODO: full body.
void fn_8006766C(Player* p) {
    (void)p;
}

void fn_80067D60() {}

void fn_80067D64(Player* p) {
    p->unk208 = 0.0f;
    p->offset.y = 0.0f;
    p->offset.x = -2.4609375f;
    p->offset.z = 0.0f;
}

// Initializes the player's model and movement state.
void fn_80067D80(ModelCtx* p, Vec3* src, int unused, Actor* actor, void* arg) {
    IVec3 fixed;
    IVec3 swapped;
    IVec3 vec;
    Mtx33 mtx;

    RadToFixed(&fixed, (Vec3*)((char*)src + 0x84));
    VecSwapXY2(&swapped, &fixed);
    MakeVec(&vec, swapped.z, swapped.y, swapped.x);
    fn_8009F028(&mtx, &vec);
    fn_800A0420(&p->rot0, &mtx);
    p->rot24 = p->rot0;
    p->rot14 = p->rot0;
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
void fn_80067FE4() {
    sCfg07Copy = 350.0f;
    sCfg09Copy = 80.0f;
}

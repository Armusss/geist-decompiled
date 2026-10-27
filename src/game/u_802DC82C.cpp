// u_802DC82C.cpp: the "Host" actor type (0x802DC82C-0x802DDEDC).
// A possessable human host: shows the "Lighten Loop"/"Focus Loop" HUD widgets, applies the
// slow-motion finish effect, and runs the death sequence (trail particles, screen effect and a
// model reset).  Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct Bone {
    char pad0[0x10];
    float unk10;
};

// Per-bone offset data (stride 0x1C), stored behind the model's animation data.
struct BoneOff {
    char pad0[0x10];
    Vec3 off;
};

struct HostObj {
    char pad0[0x38];
    int actor;    // 0x38
    char pad3C[0x5E4];
    u32 unk620;   // 0x620
};

struct Obj26C8 {
    int type;         // 0x00
    char pad4[0x168];
    int unk16C;       // 0x16C
};

struct ExtraHolder {
    char pad0[0x30];
};

// Particle emitter description (0xEC bytes) passed to EmitterCreate.
struct EmitterDesc {
    Vec3 pos;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    int unk1C;
    int unk20;
    float unk24;
    int unk28;
    int texture;
    unsigned char unk30;
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
    void* unkA0;
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
    unsigned char color[8];
    int unkD4;
    int unkD8;
    int unkDC;
    char padE0[0xC];
};

// Target of the high emitter (filled from its description).
struct TrailTarget {
    Vec3 pos;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    int unk1C;
    float unk20;
    float unk24;
    char pad28[8];
};

// Six Vec3 used to place the six death-trail particles.
struct TrailVecs {
    float f[18];
};

// The actor spawned for the death effect.
struct SpawnObj {
    char pad0[0x8C];
    int model;        // 0x8C
    char pad90[0xC];
    Bone* bone;       // 0x9C
};

struct Actor;

struct Actor {
    char pad0[0x24];
    int posX;                  // 0x24
    int posY;                  // 0x28
    int posZ;                  // 0x2C
    char pad30[8];
    u32 id;                    // 0x38
    char pad3C[0x14];
    u32 flags;                 // 0x50
    char pad54[4];
    int model;                 // 0x58
    char pad5C[0x78];
    int anim;                  // 0xD4
    char padD8[0x170];
    int hud;                   // 0x248
    char pad24C[0x3C4];
    float unk610;              // 0x610
    char pad614[0x24];
    int unk638;                // 0x638
    char pad63C[0x10];
    float unk64C;              // 0x64C
    char pad650[0x164];
    u32 flags7B4;              // 0x7B4
    char pad7B8[0x1A8];
    int state;                 // 0x960
    int state2;                // 0x964
    char pad968[8];
    float unk970;              // 0x970
    int unk974;                // 0x974
    int unk978;                // 0x978
    int unk97C;                // 0x97C
    int unk980;                // 0x980
    float unk984;              // 0x984
    float unk988;              // 0x988
    u32 parts[6];              // 0x98C
    SpawnObj* unk9A4;          // 0x9A4
    int unk9A8;                // 0x9A8
    Mtx34 mat;                 // 0x9AC
    int unk9DC;                // 0x9DC
    int unk9E0;                // 0x9E0
    int unk9E4;                // 0x9E4
    int unk9E8;                // 0x9E8
    char pad9EC[0x8F0];
    int unk12DC;               // 0x12DC
    char pad12E0[0x12C];
    char* bones;               // 0x140C
    BoneOff* boneOffsets;      // 0x1410
    char pad1414[0x74];
    float unk1488;             // 0x1488
    char pad148C[8];
    float unk1494;             // 0x1494
    char pad1498[0x11A8];
    float unk2640;             // 0x2640
    char pad2644[0x84];
    Obj26C8* unk26C8;          // 0x26C8
    char pad26CC[0x18];
    HostObj* unk26E4;          // 0x26E4
    char pad26E8[0x48];
    float unk2730;             // 0x2730
    char pad2734[0x7C];
    ExtraHolder extra;         // 0x27B0
};

struct QuerySub {
    char pad0[0x30];
    int unk30;
};

// The actor returned by a 0x400 query on the possessed actor.
struct QueryActor {
    char pad0[0x24];
    int posX;             // 0x24
    int posY;             // 0x28
    int posZ;             // 0x2C
    char pad30[8];
    int id;               // 0x38
    char pad3C[0x48];
    float fx;             // 0x84
    float fy;             // 0x88
    float fz;             // 0x8C
    char pad90[0x34];
    QuerySub* sub;        // 0xC4
    char padC8[0x528];
    ExtraHolder* holder;  // 0x5F0
    char pad5F4[0x310];
    float f904;           // 0x904
};

// Message sent to the trail subsystem.
struct ExtraMsg {
    char pad0[0x14];
    u32 type;      // 0x14
    IVec3 pos;     // 0x18
    char pad24[0xC];
    IVec3 vel;     // 0x30
    char pad3C[0x10];
    float dt;      // 0x4C
    float scale;   // 0x50
};

// 0xC07: effect position message.
struct FxPosMsg {
    int unk0;
    int type;
    int unk8;
    int unkC;
    int x, y, z;
};

// 0xC00: effect flag message.
struct FxFlagMsg {
    int unk0;
    int flag;
    int unk8;
    int unkC;
};

// 0xC10: animation/callback registration message.
struct FxAnimMsg {
    int unk0;
    u32 asset;
    int unk8;
    int unkC;
    int unk10;
    void (*callback)(int, void*);
    Actor* actor;
    int pad1C;
};

// 0x3004: begin/end message.
struct BeginMsg {
    int unk0;
    int hash;
    int unk8;
    int unkC;
    int unk10;
    int pad14[3];
};

// 0x400: generic query message; only the first word is used.
struct ActorMsg {
    int actor;
    int pad4[7];
};

// Message filled in by the death callback.
struct DeathMsg {
    int unk0;
    int arg;
    int unk8;
    int target;
    int pad10[4];
};

// Description of the death-effect actor (0x38 bytes).
struct SpawnDesc {
    Vec3 pos;        // 0x00
    Vec3 rot;        // 0x0C
    int unk18;       // 0x18
    int unk1C;       // 0x1C
    u32 type;        // 0x20
    int unk24;       // 0x24
    int unk28;       // 0x28
    char pad2C[0xC];
};

extern "C" {
void* memset(void* dst, int val, u32 n);
int SendMsg(int to, int msg, void* data);
void FxStart(void* fx, int a, int b);
void PADRecalibrate(u32 mask);
void VISetBlack(int black);
void VIFlush(void);
void VIWaitForRetrace(void);
void CARDUnmount(int chan);
void OSResetSystem(int reset, int code, int force);

// Unmatched functions in neighbouring units.
void fn_801D55CC(int object);
int fn_802A1D04(void);
void fn_800A0168(Mtx34* out, Mtx34* in);
void fn_800A0030(void* out, void* m, void* in);
void fn_8016D220(void* out, void* in);
void fn_8016D18C(void* out, void* in);
void fn_8016D1A8(void* out, void* in);
void fn_8007EE6C(int font, int a, int b, int c, int d, int e);
int fn_80212730(int id);
void fn_80090974(int anim, int a, int b, int c, int d, Actor* actor, float f);
void fn_8009F63C(void* out, Actor* actor);
void fn_8009F44C(void* out, void* in, void* in2);
void fn_8009F190(void* out, Actor* actor);
int fn_80049940(void);
void fn_80170E60(int a);
void fn_80173DF8(int a);
int fn_8007EF48(int hud, int a, const char* name, int b, int c, int d, int e, int f,
                int g, int h);
void fn_800BA7EC(int a);
void fn_8017BA10(void);
void fn_8001BAA8(int a);
void fn_800F60E4(int a, int b, u32 c);
void fn_8006811C(Actor* a, u32 hash);
void fn_80158AAC(int a, Actor* actor, int b);
void fn_80158990(int a, Actor* actor);
void fn_80048A34(Actor* a, int b);
float fn_80018A40(int model);
void fn_8008F230(int anim, float f, int a, int b);
void fn_800913EC(int anim, float f, void* buf);
float fn_800C63A0(int a);
float fn_800A127C(float f);
int fn_800C7E80(void* pos);
int fn_801D54E8(void);
int fn_801D5810(float x, float y, float z, int model, int a, u32 hash, int handle, void* dir,
                int flag);
void fn_801D52E0(int object, Mtx34* m);
void fn_801D556C(int object);
int fn_800C5210(int target, int msg, void* data);
void fn_8000F7E4(int a, int b);
void fn_800EBCC4(int a, int b);
void fn_800ECB18(void);
}
void PlayerSetHeldSlotMode(void* owner, int mode);
void EmitterSetEnabled(u32 handle, int a);

int PlayerInHost(Actor* a);                                       // 0x80049750
int ActorPlayAnim(int actor, u32 anim, int arg);                  // 0x800C5654
void ActorRender(Actor* a);                                       // 0x800C7CE8
int AnimRef(int anim);                                            // 0x800934D0
void AnimUnref(int anim);                                         // 0x80093424
int AssetFind(u32 bank, u32 name);                                // 0x8007EB34
int EmitterCreate(EmitterDesc* desc);                             // 0x800D3D88
int ModelAnimate(int model, char* bones, int a);                  // 0x8003AA9C
int ModelBoneCount(int model);                                    // 0x80037BEC
void AnimApply(int anim, char* bones, int a, int count);          // 0x8008F630
void ModelSetMatrix(int model, Mtx34* m, char* bones);            // 0x8003A66C
void ActorGetMatrix(Mtx34* m, Actor* a);                          // 0x8009ED70
void BoneWorldMatrix(Mtx34* m, Bone* bone, float* f);             // 0x800A09A8
void MtxGetEuler(Vec3* rot, Mtx34* m);                            // 0x8009FB94
void MtxSetEuler(Mtx34* m, Vec3* rot);                            // 0x8009FAC0
int FxFind(const char* name);                                     // 0x80170834
void* OwnerGetInfo(u32 handle);                                   // 0x8002394C
void FontRelease(int font);                                       // 0x8007F0A8
void ExtraFree(ExtraHolder* holder);                              // 0x8023040C
void ExtraTrailAttach(ExtraHolder* holder);                       // 0x80230FA8
void ExtraTrailUpdate(ExtraHolder* holder, float dt);             // 0x80230B1C
void ExtraTrailApply(ExtraHolder* holder, ExtraMsg* msg);         // 0x80230504
void Fn8021E654(int a);                                           // 0x8021E654
int Fn800DF3B8(u32 a, int b, u32 c);                              // 0x800DF3B8

extern "C" {
extern int lbl_804D3D84;
extern int lbl_804D3D88;
extern int lbl_804D585C;
extern int lbl_804D3DA4;
extern int lbl_804D5854;
}

extern float gAspectScale;  // owned by another unit

static const TrailVecs sTrailDirs0 = {{0}};
static const TrailVecs sTrailPos0 = {{0}};
static const Vec3 sZeroVec = {0.0f, 0.0f, 0.0f};

static TrailTarget sTrailTarget;

static float sSlowTimer = 600.0f;

static int sTrailAnimA;
static int sTrailAnimB;
static int sFxTarget;
static int sFxTarget2;
static int sSlowActive;
static int sSlowUnused;

void HostCreateEmitters(Actor* a);
void HostTimerReset(void);
u8 HostIsActive(Actor* a);
void HostFreeVisuals(Actor* a);
void HostUpdate(Actor* a, float dt);
void HostExtra(Actor* a);
void HostAction(Actor* a);
void HostDie(Actor* a);
void HostDeathCallback(int msg, void* data);
void HostReset(Actor* a);
void HostReboot(int a, int b);
void HostCardCleanup(void);

// 0x802DC82C: create the two emitters used by the host (body glow and trail target).
void HostCreateEmitters(Actor* a) {
    EmitterDesc highDesc;
    EmitterDesc lowDesc;
    BoneOff* bone;
    char index;

    memset(&highDesc, 0, sizeof(highDesc));
    memset(&lowDesc, 0, sizeof(lowDesc));
    index = ((unsigned char*)ModelAnimate(a->model, a->bones, 1))[a->unk12DC];
    if (index == 0xFF) {
        return;
    }

    lowDesc.pos.x = (float)a->posX;
    lowDesc.pos.y = (float)a->posY;
    lowDesc.pos.z = (float)a->posZ;
    bone = &a->boneOffsets[index];
    lowDesc.pos.x += bone->off.x;
    lowDesc.pos.y += bone->off.y;
    lowDesc.pos.z += bone->off.z;
    lowDesc.pos.z += 150.0f;

    lowDesc.unk1C = 1;
    lowDesc.unk20 = 2;
    lowDesc.unk24 = -107374180.0f;
    lowDesc.unk28 = 1;
    lowDesc.texture = AssetFind(0xF2E2BEB8, 0x092B6567);
    lowDesc.unk30 = 0xFE;
    lowDesc.unk6C = 4.838f;
    lowDesc.unk70 = 5.379f;
    lowDesc.unk74 = 0.0f;
    lowDesc.unk78 = 0.0f;
    lowDesc.unk7C = 8;
    lowDesc.unk80 = 300.0f;
    lowDesc.unk84 = 1.0f;
    lowDesc.unk88 = 0x10;
    lowDesc.unk8C = 0.0f;
    lowDesc.unk90 = 0.15f;
    lowDesc.unk94 = 10000.0f;
    lowDesc.unk98 = 1;
    lowDesc.unk9C = 0;
    lowDesc.unkA4 = 0;
    lowDesc.unkAC = 3.0f;
    lowDesc.unkB0 = 0.0f;
    lowDesc.unkB4 = 0.15f;
    lowDesc.unkB8 = 0.0f;
    lowDesc.unkBC = 0.0f;
    lowDesc.unkC0 = 0.0f;
    lowDesc.unkC4 = 0.075f;
    lowDesc.unkC8 = 0.0575f;
    lowDesc.color[0] = 0xD6;
    lowDesc.color[1] = 0xE4;
    lowDesc.color[2] = 0x99;
    lowDesc.color[3] = 0xFF;
    lowDesc.color[4] = 0xFF;
    lowDesc.color[5] = 0xFF;
    lowDesc.color[6] = 0xFF;
    lowDesc.color[7] = 0xFF;
    lowDesc.unkD4 = -1;
    lowDesc.unkD8 = -1;
    lowDesc.unkDC = 0;

    highDesc.pos.x = lowDesc.pos.x;
    highDesc.pos.y = lowDesc.pos.y;
    highDesc.pos.z = lowDesc.pos.z;
    highDesc.unk1C = 1;
    highDesc.unk20 = 0;
    highDesc.unk24 = 20.0f;
    highDesc.unk28 = 1;
    highDesc.texture = AssetFind(0xF2E2BEB8, 0x60A26EDB);
    highDesc.unk30 = 0xFE;
    highDesc.unk6C = 45.014f;
    highDesc.unk70 = 25.895f;
    highDesc.unk74 = 0.0f;
    highDesc.unk78 = 0.0f;
    highDesc.unk7C = 0x7D0;
    highDesc.unk80 = 200.0f;
    highDesc.unk84 = 1.0f;
    highDesc.unk88 = 8;
    highDesc.unk8C = 0.0f;
    highDesc.unk90 = 0.15f;
    highDesc.unk94 = 0.0f;
    highDesc.unk98 = 1;
    highDesc.unk9C = 1;

    sTrailTarget.pos.x = highDesc.pos.x;
    sTrailTarget.pos.y = highDesc.pos.y;
    sTrailTarget.pos.z = highDesc.pos.z;
    sTrailTarget.unkC = highDesc.unkC;
    sTrailTarget.unk10 = highDesc.unk10;
    sTrailTarget.unk14 = highDesc.unk14;
    sTrailTarget.unk18 = highDesc.unk18;
    sTrailTarget.unk1C = 1;
    sTrailTarget.unk20 = -60.0f;
    sTrailTarget.unk24 = 0.0f;

    highDesc.unkA0 = &sTrailTarget;
    highDesc.unkA4 = 0;
    highDesc.unkAC = 0.0f;
    highDesc.unkB0 = 0.0f;
    highDesc.unkB4 = 0.1f;
    highDesc.unkB8 = 0.0f;
    highDesc.unkBC = 0.0f;
    highDesc.unkC0 = 0.0f;
    highDesc.unkC4 = 0.0f;
    highDesc.unkC8 = 0.167f;
    highDesc.color[0] = 0xFF;
    highDesc.color[1] = 0xFF;
    highDesc.color[2] = 0xFF;
    highDesc.color[3] = 0xFF;
    highDesc.color[4] = 0xFF;
    highDesc.color[5] = 0xFF;
    highDesc.color[6] = 0xFF;
    highDesc.color[7] = 0xFF;
    highDesc.unkD4 = -1;
    highDesc.unkD8 = -1;
    highDesc.unkDC = 0;

    lbl_804D3D88 = EmitterCreate(&lowDesc);
    lbl_804D3D84 = EmitterCreate(&highDesc);
    EmitterSetEnabled(lbl_804D3D88, 1);
    EmitterSetEnabled(lbl_804D3D84, 1);
}

// 0x802DCBF0: restart the slow-motion timer.
void HostTimerReset(void) {
    sSlowActive = 1;
    sSlowTimer = 600.0f;
}

// 0x802DCC04: true while the host is active in any of its three phases.
u8 HostIsActive(Actor* a) {
    return a->unk9E4 != 0 || a->state2 != 0 || a->state != 0;
}

// 0x802DCC38: release the effect actor, the six trail particles and the loop animation.
void HostFreeVisuals(Actor* a) {
    int i;

    if (a->unk9A4 != 0) {
        ActorRender((Actor*)a->unk9A4);
    }
    for (i = 0; i < 6; i++) {
        if (a->parts[i] != 0) {
            fn_801D55CC(a->parts[i]);
            a->parts[i] = 0;
        }
    }
    if (a->unk978 != -1) {
        AnimUnref(a->unk978);
        a->unk978 = -1;
    }
}

void HostDie(Actor* a);

// 0x802DCCD0: per-frame update.
void HostUpdate(Actor* a, float dt) {
    Mtx34 m;
    Vec3 v68;
    FxPosMsg msg;
    ActorMsg out;
    Vec3 rot;
    IVec3 tmp;
    Vec3 v;
    QueryActor* q;

    if (fn_802A1D04() != 0 && fn_802A1D04() != 3 && PlayerInHost(a)) {
        if (a->unk2730 == 0.0f) {
            a->unk974 = (int)((float)a->unk974 + dt);
        } else {
            a->unk974 = 0;
        }
    } else {
        if (a->state == 1) {
            a->unk974 = (int)((float)a->unk974 + dt);
            if (sSlowActive) {
                sSlowTimer -= dt;
                sSlowTimer = sSlowTimer > 0.0f ? sSlowTimer : 0.0f;
                a->unk970 = sSlowTimer / 600.0f;
            } else {
                a->unk970 = 1.0f;
            }
            if (PlayerInHost(a) || a->unk9E4 == 1) {
                v = sZeroVec;
                v.y = (float)a->unk9A8;
                MtxGetEuler(&rot, &a->mat);
                rot.y += 0.02f * dt;
                MtxSetEuler(&a->mat, &rot);
                fn_800A0168(&m, &a->mat);
                fn_800A0030(&v, &m, &v);
                msg.x = (int)(a->mat.m[2][1] + v.x);
                msg.y = (int)(a->mat.m[2][2] + v.y);
                msg.z = (int)(a->mat.m[2][3] + v.z);
                MtxGetEuler(&rot, &a->mat);
                fn_8016D220(&tmp, &rot);
                fn_8016D18C(&v68, &tmp);
                msg.type = 3;
                SendMsg(sFxTarget, 0xC07, &msg);
            }
        } else {
            if (!PlayerInHost(a) && a->unk9E4 != 1) {
                return;
            }
            if (a->unk9E4 != 0 && a->unk9E0 != 0 && a->unk9E0 != -1 &&
                ((u32)a->unk9E0 >> 24) == 0x13) {
                out.actor = 0;
                SendMsg(a->unk9E0, 0x400, &out);
                q = (QueryActor*)out.actor;
                if (q != 0 && q->holder != 0 && fn_80212730(q->id) == -1) {
                    ExtraTrailUpdate(q->holder, dt);
                    q->fx = (float)q->posX;
                    q->fy = (float)q->posY;
                    q->fz = (float)q->posZ;
                }
            }
            if (a->state2 == 1) {
                a->unk988 += dt;
                if (a->unk988 >= 1000.0f) {
                    a->unk984 = (a->unk988 - 1000.0f) / 1125.0f;
                    a->unk984 = a->unk984 < 1.0f ? a->unk984 : 1.0f;
                    if (a->unk97C != -1) {
                        fn_8007EE6C(a->unk97C, 0, 0, (int)(255.0f * a->unk984), 0, 0);
                    }
                    if (a->unk980 != -1) {
                        fn_8007EE6C(a->unk980, (int)(20.0f * a->unk984), 0, 0, 0, 0);
                    }
                    if (a->unk988 >= 2125.0f) {
                        HostDie(a);
                        a->unk988 = 0.0f;
                    }
                }
            }
        }
    }
}

// 0x802DD128: switch the host into its loop animation or (re)attach the trail effect.
void HostExtra(Actor* a) {
    ExtraMsg msg;
    Mtx34 v;

    if (a->unk978 != -1) {
        a->unk638 = 1;
        a->flags &= ~4;
        fn_80090974(a->anim, 0, a->unk978, 0x58, 0, a, 1.0f);
    } else if (a->unk26C8 != 0 && a->unk26C8->unk16C != 0) {
        ExtraFree(&a->extra);
        ExtraTrailAttach(&a->extra);
        msg.dt = 1.0f;
        msg.scale = 0.0f;
        msg.type = 0x75715902;
        msg.vel.x = 0;
        msg.vel.y = 0;
        msg.vel.z = 0;
        fn_8009F63C(&v, a);
        fn_8009F44C(&msg.vel, &v, &msg.vel);
        msg.pos.x = a->posX;
        msg.pos.y = a->posY;
        msg.pos.z = a->posZ;
        ExtraTrailApply(&a->extra, &msg);
    }
}

void HostDeathCallback(int msg, void* data);

// 0x802DD234: start possessing / release the host.
void HostAction(Actor* a) {
    IVec3 v2;
    FxPosMsg msg;
    FxAnimMsg animMsg;
    ActorMsg out;
    BeginMsg beginMsg;
    FxFlagMsg flagMsg;
    IVec3 v;

    if (fn_80049940() != 0) {
        return;
    }
    if (a->state2 != 0 || a->state != 0) {
        return;
    }
    {
        Fn8021E654(1);
        fn_80170E60(1);
        fn_80173DF8(0);
        fn_80173DF8(1);
        fn_80173DF8(2);
        if (PlayerInHost(a) || a->unk9E4 == 1) {
            if (a->unk97C == -1) {
                a->unk97C = fn_8007EF48(a->hud, 1, "Lighten Loop", 0, 0, 0, 0, 0, 0, 0);
            }
            if (a->unk980 == -1) {
                a->unk980 = fn_8007EF48(a->hud, 1, "Focus Loop", 0, 0, 0, 0, 0, 0, 0);
            }
            a->unk984 = 0.0f;
            a->state2 = 1;
            fn_800BA7EC(0);
            if (a->unk9E0 != -1) {
                ActorPlayAnim(a->unk9E0, 0xDC8A96AD, a->id);
            } else {
                if (a->unk26E4 != 0) {
                    ActorPlayAnim(a->unk26E4->actor, 0xDC8A96AD, a->id);
                }
            }
            if (a->unk978 != -1) {
                AnimRef(a->unk978);
            }
        } else {
            fn_8017BA10();
            beginMsg.hash = 0x3BF55545;
            beginMsg.unk8 = -1;
            beginMsg.unk10 = 0;
            SendMsg(0x2D000000, 0x3004, &beginMsg);
            a->state = 1;
            a->unk974 = 0;
            a->unk970 = 255.0f;
            if (sFxTarget == -1) {
                a->flags7B4 |= 0x20000;
            } else {
                fn_8001BAA8(0);
                fn_800F60E4(a->hud, 0, 0x0024A738);
                msg.x = a->posX;
                msg.y = a->posY;
                msg.z = a->posZ;
                fn_8009F190(&v, a);
                fn_8016D18C(&v2, &v);
                msg.type = 3;
                SendMsg(sFxTarget, 0xC07, &msg);
                SendMsg(sFxTarget2, 0xC07, &msg);
                flagMsg.flag = 1;
                SendMsg(sFxTarget, 0xC00, &flagMsg);
                animMsg.asset = sTrailAnimA;
                animMsg.unk8 = 0;
                animMsg.unkC = 0x10;
                animMsg.unk10 = 0x1000;
                animMsg.callback = HostDeathCallback;
                animMsg.actor = a;
                SendMsg(sFxTarget, 0xC10, &animMsg);
                out.actor = 0;
                if (sFxTarget2 != -1) {
                    SendMsg(sFxTarget2, 0x400, &out);
                }
                fn_8006811C(a, 0xB5D5E938);
                fn_80158AAC(a->hud, a, out.actor);
            }
        }
    }
}

// 0x802DD534: play the death sequence.
void HostDie(Actor* a) {
    TrailVecs dirs;
    TrailVecs pos;
    IVec3 v16c;
    FxPosMsg msg;
    Mtx34 v120;
    SpawnDesc desc;
    Mtx34 boneMtx;
    Mtx34 m;
    ActorMsg out;
    BeginMsg beginMsg;
    FxFlagMsg flagMsg;
    IVec3 v2c;
    IVec3 t;
    IVec3 v14;
    float buf[3];
    float* p;
    float* d;
    int i;
    int handle;
    int fx;
    void* info;
    QueryActor* q;
    float r;
    float ang;
    float c;
    float v1;
    float v2;
    float v3;

    if (a->unk9E4 != 0 && a->unk9E0 != -1) {
        SendMsg(a->unk9E0, 0x400, &out);
        q = (QueryActor*)out.actor;
        q->f904 = (float)q->sub->unk30;
        fn_80048A34(a, a->unk9E0);
        a->unk2730 = 0.0f;
    }
    fn_8017BA10();
    beginMsg.hash = 0x7B96F5AC;
    beginMsg.unk8 = -1;
    beginMsg.unk10 = 0;
    SendMsg(0x2D000000, 0x3004, &beginMsg);
    sSlowActive = 0;
    fn_800BA7EC(1);
    fn_8006811C(a, 0xB5D5E938);
    if (a->unk26C8 == 0) {
        i = -2000;
    } else {
        switch (a->unk26C8->type) {
        case 0x13C05:
        case 0xE0AB9B4F:
        case 0x04624485:
            i = -1000;
            break;
        default:
            i = -2000;
            break;
        }
    }
    a->unk9A8 = i;
    t.x = 0;
    t.y = a->unk9A8;
    t.z = 0;
    if (a->unk97C != -1) {
        FontRelease(a->unk97C);
        a->unk97C = -1;
    }
    if (a->unk980 != -1) {
        FontRelease(a->unk980);
        a->unk980 = -1;
    }
    a->state2 = 0;
    a->state = 1;
    a->unk974 = 0;
    a->unk970 = 255.0f;
    a->unk988 = 0.0f;
    a->unk9E0 = -1;
    a->unk9E4 = 0;
    if (sFxTarget == -1) {
        a->flags7B4 |= 0x20000;
        return;
    }
    fn_8001BAA8(0);
    fn_800F60E4(a->hud, 1, 0x0024A738);
    msg.x = a->posX;
    msg.y = a->posY;
    msg.z = a->posZ;
    fn_8009F190(&v2c, a);
    fn_8016D18C(&v16c, &v2c);
    fn_8009F63C(&v120, a);
    fn_8009F44C(&t, &v120, &t);
    msg.x += t.x;
    msg.y += t.y;
    msg.z += t.z;
    ActorGetMatrix(&a->mat, a);
    msg.type = 3;
    SendMsg(sFxTarget, 0xC07, &msg);
    flagMsg.flag = 1;
    SendMsg(sFxTarget, 0xC00, &flagMsg);
    fn_80158990(a->hud, a);
    a->flags7B4 |= 0x20000;
    a->unk2640 = 4000.0f;
    if (a->unk978 == -1) {
        ang = fn_800C63A0(0);
        c = fn_800A127C(ang * gAspectScale);
        v1 = (float)(-a->unk9A8) * c;
        v2 = v1 * 0.9f;
        v3 = (float)(-a->unk9A8) * fn_800A127C(ang);
        dirs = sTrailDirs0;
        dirs.f[0] = v2;
        dirs.f[3] = -v2;
        dirs.f[7] = v2;
        dirs.f[10] = -v2;
        dirs.f[14] = v3;
        dirs.f[17] = (-v1) * 0.5f + -50.0f - (a->unk1494 - a->unk1488);
        pos = sTrailPos0;
        pos.f[0] = v2;
        pos.f[1] = v2;
        pos.f[2] = v2;
        pos.f[3] = v2;
        pos.f[4] = v2;
        pos.f[5] = v2;
        pos.f[6] = v2;
        pos.f[7] = v2;
        pos.f[8] = v2;
        pos.f[9] = v2;
        pos.f[10] = v2;
        pos.f[11] = v2;
        pos.f[12] = v2;
        pos.f[13] = v2;
        pos.f[14] = v3;
        pos.f[15] = v2;
        pos.f[16] = v2;
        pos.f[17] = v1;
        desc.unk1C = 0;
        desc.unk28 = -1;
        desc.unk18 = 0;
        desc.pos.x = (float)a->posX;
        desc.pos.y = (float)a->posY;
        desc.pos.z = (float)a->posZ;
        fn_8009F190(&v14, a);
        fn_8016D1A8(&desc.rot, &v14);
        a->unk9A4 = (SpawnObj*)fn_800C7E80(&desc);
        p = pos.f;
        d = dirs.f;
        for (i = 0; i < 6; i++) {
            handle = fn_801D54E8();
            a->parts[i] = fn_801D5810(p[0], p[1], p[2], a->unk9A4->model, -1, 0x04B8D9AB, handle, d, 1);
            BoneWorldMatrix(&boneMtx, a->unk9A4->bone, &a->unk9A4->bone->unk10);
            fn_800A0168(&boneMtx, &boneMtx);
            fn_801D52E0(a->parts[i], &boneMtx);
            fn_801D556C(a->parts[i]);
            p += 3;
            d += 3;
        }
    }
    info = OwnerGetInfo(a->unk9E0);
    if (info != 0) {
        fx = Fn800DF3B8(0xE6BCD867, ((HostObj*)info)->unk620, 0x5590A293);
        if (fx == -1) {
            fx = FxFind("host_die");
        }
    } else if (a->unk26E4 != 0) {
        fx = Fn800DF3B8(0xE6BCD867, a->unk26E4->unk620, 0x5590A293);
        if (fx == -1) {
            fx = FxFind("host_die");
        }
    } else {
        fx = FxFind("host_die");
    }
    FxStart((void*)fx, 0, 0);
    a->unk64C = fn_80018A40(a->model);
    a->posZ = (int)(a->unk610 + a->unk64C);
    a->flags7B4 &= ~0x02000000;
    a->flags7B4 &= ~0x00080000;
    a->state = 0;
    PlayerSetHeldSlotMode(a, 1);
    a->state = 1;
    fn_8008F230(a->anim, 0.0f, 0, 1);
    fn_800913EC(a->anim, 100.0f, buf);
    handle = ModelAnimate(a->model, a->bones, 1);
    i = ModelBoneCount(a->model);
    AnimApply(a->anim, a->bones, handle, i);
    ActorGetMatrix(&m, a);
    ModelSetMatrix(a->model, &m, a->bones);
}

// 0x802DDC14: callback used by the death effect.
void HostDeathCallback(int msg, void* data) {
    DeathMsg m;
    Actor* a;

    a = (Actor*)data;
    m.arg = msg;
    m.target = sFxTarget;
    if (fn_800C5210(sFxTarget, 0xC06, &m) != 1) {
        if (msg == 0x1B83833F) {
            a->flags7B4 |= 0x20000;
        }
    }
}

// 0x802DDC8C: reset every host-specific field (and reload the shared effect assets).
void HostReset(Actor* a) {
    FxFlagMsg msg;

    a->state = 0;
    a->state2 = 0;
    a->unk970 = 255.0f;
    a->unk974 = 0;
    a->unk97C = -1;
    a->unk980 = -1;
    a->unk984 = 0.0f;
    a->unk9A8 = -2000;
    a->unk9E8 = 0;
    a->unk9E0 = -1;
    a->unk9E4 = 0;
    a->unk978 = -1;
    a->unk9DC = 1;
    a->unk9A4 = 0;
    a->parts[0] = 0;
    a->parts[1] = 0;
    a->parts[2] = 0;
    a->parts[3] = 0;
    a->parts[4] = 0;
    a->parts[5] = 0;
    sFxTarget = -1;
    sFxTarget2 = -1;
    sTrailAnimA = -1;
    sTrailAnimB = -1;
    sSlowActive = 0;
    if (fn_802A1D04() != 0) {
        return;
    }
    if (sTrailAnimA == -1) {
        sTrailAnimA = AssetFind(0x2FA2416F, 0xB07AF7DD);
        AnimRef(sTrailAnimA);
    }
    if (sTrailAnimB == -1) {
        sTrailAnimB = AssetFind(0x2FA2416F, 0x893DD7ED);
        AnimRef(sTrailAnimB);
    }
    if (sFxTarget == -1) {
        sFxTarget = AssetFind(0x760CB725, 0x769B4279);
        if (sFxTarget != -1) {
            msg.flag = 1;
            SendMsg(sFxTarget, 0x7203, &msg);
        }
    }
    if (sFxTarget2 == -1) {
        sFxTarget2 = AssetFind(0x000110A6, 0x0E9C3BBB);
        if (sFxTarget2 != -1) {
            msg.flag = 1;
            SendMsg(sFxTarget2, 0x7203, &msg);
        }
    }
    fn_8000F7E4(AssetFind(0xF2E2BEB8, 0x092B6567), 0);
    fn_8000F7E4(AssetFind(0xF2E2BEB8, 0x60A26EDB), 0);
}

// 0x802DDE38: reset the console (used from the debug menu).
void HostReboot(int reset, int force) {
    PADRecalibrate(0xF0000000);
    VISetBlack(1);
    VIFlush();
    VIWaitForRetrace();
    CARDUnmount(0);
    OSResetSystem(reset, 3, force);
}

// 0x802DDE9C: shut the card system down if it was started.
void HostCardCleanup(void) {
    if (lbl_804D585C == 1) {
        fn_800EBCC4(lbl_804D3DA4, lbl_804D5854);
        fn_800ECB18();
    }
    lbl_804D585C = 0;
}

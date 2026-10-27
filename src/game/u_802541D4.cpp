// u_802541D4.cpp: the imp's fire ball actor ("Imp Fire", 0x802541D4-0x80254F54). Split from the host type
// callbacks: its own string pool (starting with POSSESS at 0x8034A724) and its own pooled static block (the
// ghost-action table at 0x803BEE18, the flame tables, the ActorType and the asset names).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include <string.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_80019A04();
int fn_800F8AF4(int id);
void fn_80175D90(void* p);
void fn_80175EE4(void* p);
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);
extern int lbl_804D54D0;

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    if (s == 0) {
        return 0;
    } else {
        u32 h = 0;
        int i;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
        return h;
    }
}


// ---------------------------------------------------------------------------------------------------
// 0x8025643C-0x8025808C: the gun-with-grenade host states (aim, fire, reload, grenade throw) and the
// per-host-type table callbacks. Its own string pool ("gun_empty"), host table (0x803BF068) and
// .sdata2 constants (0x804D9E98-0x804D9EBC) suggest it is a file of its own.

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();
    ~String() {}
    virtual void Release(char* p);
};

struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

struct Actor;

// Per-host-type behaviour table (25 entries at 0x803BF068).
struct HostType {
    void (*enter)(Actor* actor);
    void (*update)(Actor* actor, int arg, float dt);
    void (*msg)(Actor* actor);
    u32 actions[14];
    void* name;
};

extern HostType lbl_803BF068[];

// The host's per-instance animation block, reached through the actor's host array.
struct HostData {
    char pad00[0x14];
    int set;              // 0x14
    int aim;              // 0x18
    int walk;             // 0x1C
    int unk20;
    int run;              // 0x24
};

typedef int (*HostMsgFunc)(int msg, void* data);

struct Actor {
    char pad0[0x24];
    int pos[3];           // 0x24
    char pad30[0x38 - 0x30];
    int handle;           // 0x38
    char pad3C[0x50 - 0x3C];
    u32 flags50;          // 0x50
    char pad54[0x58 - 0x54];
    int model58;          // 0x58
    char pad5C[0x78 - 0x5C];
    int unk78;            // 0x78
    int model;            // 0x7C
    char pad80[0x8C - 0x80];
    HostMsgFunc msgHandler;  // 0x8C
    char pad90[0x98 - 0x90];
    int anim;             // 0x98
    char pad9C[0xA4 - 0x9C];
    int animMode;         // 0xA4
    char padA8[0xC4 - 0xA8];
    int prevAnim;         // 0xC4
    char padC8[0xE4 - 0xC8];
    HostMsgFunc msgHandler2;  // 0xE4
    char padE8[0xF0 - 0xE8];
    int anim2;            // 0xF0
    char padF4[0xFC - 0xF4];
    int animMode2;        // 0xFC
    char pad100[0x11C - 0x100];
    int prevAnim2;        // 0x11C
    char pad120[0x270 - 0x120];
    char conditions[0x2F4 - 0x270];  // 0x270
    char timer[0x314 - 0x2F4];  // 0x2F4
    int ammo;             // 0x314
    int grenades;         // 0x318
    char pad31C[4];
    int pressed;          // 0x320
    u32 pressed2;         // 0x324
    int unk328;           // 0x328
    char pad32C[0x38C - 0x32C];
    float move[4];        // 0x38C
    char pad39C[0x588 - 0x39C];
    int mode;             // 0x588
    char pad58C[0x5E8 - 0x58C];
    float hold;           // 0x5E8
    char pad5EC[0x638 - 0x5EC];
    int unk638;           // 0x638
    char pad63C[0x678 - 0x63C];
    u32 flags678;         // 0x678
    char pad67C[0x73C - 0x67C];
    int weapon;           // 0x73C
    char pad740[0x7A4 - 0x740];
    int type;             // 0x7A4
    int nextType;         // 0x7A8
    char pad7AC[8];
    u32 flags;            // 0x7B4
    char pad7B8[0x7D0 - 0x7B8];
    int unk7D0;           // 0x7D0
    int variant;          // 0x7D4
    float unk7D8;         // 0x7D8
    char pad7DC[0x800 - 0x7DC];
    float aim;            // 0x800
    char pad804[0x14FC - 0x804];
    float speed;          // 0x14FC
    char pad1500[0x1D04 - 0x1500];
    char buf[0x50];       // 0x1D04
    float bufAim;         // 0x1D54
    char pad1D58[0x26B0 - 0x1D58];
    int bone;             // 0x26B0
    char pad26B4[0x26C4 - 0x26B4];
    HostData* hosts[7];   // 0x26C4
    char grab[4];         // 0x26E0
    Actor* holder;        // 0x26E4
    Actor* host;          // 0x26E8
    char pad26EC[0x279C - 0x26EC];
    int unk279C;          // 0x279C
};

extern "C" {
int fn_800498D0(Actor* a);
int fn_800A1BC8(int set, int anim, u32 action, int a, int b);
int fn_802A1D04(void);
float fn_800699B8(Actor* a);
void fn_800699A8(void* timer);
void fn_8008E824(int model, int a);
int fn_800F7994(int a, u32 b);
int fn_800C5210(int to, int msg, void* data);
void fn_80048BA8(Actor* a, void* buf, int mode);
int rand(void);
void fn_800769A0(Actor* a, float dt);
HostData* fn_800A384C(Actor* a);
void fn_80155A84(void* grab, Actor* host, int a);
int fn_80175EFC(int a);
void fn_80050EB0(Actor* a);
}
void PlayerSelectCollisionUpdate(void* owner, float dt);
void PlayerResetGunTimer(void* owner);
void PlayerSetValue4(void* owner, int value);
void PlayerSetHeldSlotIndex(void* owner, int index);
int PlayerGetHeldSlotIndex(void* owner);
void PlayerSetActionMode(void* owner, int mode);
void PlayerSetHeldSlotMode(void* owner, int mode);

int HasCondition(void* conditions, int which);
int ModelBoneName(int model, int bone);
int ModelBoneNodeChain(int model, int name, int* chain, int count);
extern int lbl_804D3624;
extern int lbl_804D3628;

int ActorPlayAnim(int actor, u32 anim, int arg);
void DashUpdate(Actor* a);
int PlayerInHost(Actor* a);
extern "C" int SendMsg(int to, int msg, void* data);
int Fn800F7DEC(int a, int b, int c, int d, void* e);
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f);
void* FxFind(const char* name);
extern "C" int FxStart(void* fx, int a, int b);
extern int lbl_804D3620;

struct MsgQuery {
    int unk0;
    u32 hash;
    int unk8;
    int result;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
};

inline void Query(int* out, int target, int msg, u32 hash) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    *out = *(int*)&m.result;
}

inline void QueryValue(int target, u32 hash, int value) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    SendMsg(target, 0x41D, &m);
}

// ---------------------------------------------------------------------------------------------------
// 0x802541D4-0x80254F54: the imp's fire ball actor ("Imp Fire"): spawned by the imp's fire effect, it
// flies along its throw, trails two sets of flame particles and burns whatever it touches.
// Its .data (the ghost action table at 0x803BEE18, the flame tables, the actor type and the asset names),
// .rodata (0x8034A718-0x8034A978) and .sdata/.sdata2 constants are separate from the host part below.

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct IBox {
    IVec3 min;
    IVec3 max;
};

struct PlaceF;

// Launch parameters for the fire ball (also built by the imp's fire effect).
struct ImpThrow {
    Vec3 pos;    // 0x00
    Vec3 dir;    // 0x0C
    Vec3 vel;    // 0x18
    Vec3 spin;   // 0x24
    Vec3 accel;  // 0x30
    u8 pad3C[0x4C - 0x3C];
};

// One flame particle (0x44 bytes).
struct FlameParticle {
    float age;    // 0x00
    float life;   // 0x04
    Vec3 pos;     // 0x08
    u8 pad14[0x44 - 0x14];
};

// The fire ball's state (VAlloc'd, 0x1F0 bytes).
struct ImpFireBall {
    int unk0;        // 0x00
    int owner;       // 0x04: handle of the throwing imp
    int fx;          // 0x08: the flame sound/effect handle, -1 if none
    ImpThrow t;      // 0x0C
    u8 pad58[0x1F0 - 0x58];  // 0x58: the flame particles, sCounts[0] then sCounts[1]
};

struct FireActor;

// The kind of object a handle refers to (its top byte).
inline int HandleKind(int handle) {
    return (u32)handle >> 24;
}

struct ActorType {
    int unk0;
    const char* name;
    int (*msg)(FireActor* self, int msg, void* data, int arg);
    void (*update)(FireActor* a, float dt);
    int (*init)(FireActor* a);
    void* unk14;
    void (*destroy)(FireActor* a);
    void* unk1C[5];
};

// The view of an actor the fire ball code uses.
struct FireActor {
    char pad0[0x24];
    int x, y, z;          // 0x24
    char pad30[0x38 - 0x30];
    int handle;           // 0x38
    char pad3C[4];
    IBox* box;            // 0x40
    char pad44[0x54 - 0x44];
    int height;           // 0x54
    char pad58[0x88 - 0x58];
    ActorType* type;      // 0x88
    int unk8C;            // 0x8C
    char pad90[0xA4 - 0x90];
    ImpFireBall* fire;    // 0xA4
    char padA8[0xAC - 0xA8];
    u32 flags;            // 0xAC
    char padB0[0xC4 - 0xB0];
    u32* kind;            // 0xC4
    char padC8[0xE0 - 0xC8];
    Mtx34 mtx;            // 0xE0
    char pad110[0x118 - 0x110];
    float z118;           // 0x118
    char pad11C[0x8F0 - 0x11C];
    int target;           // 0x8F0
};

struct SplineNameEntry {
    char* name;
    u32 group;
    u32 hash;
    int asset;
};

typedef int (*FireTouchFunc)(FireActor* other, FireActor* a);
typedef int (*FireHitFunc)(void* hit, FireActor* a);

void SplineClearNameHashes(SplineNameEntry* entries, int count);
int SplineAssetResolve(SplineNameEntry* entries, int count, int index);
void VFree(void* p);
void DeferAdd(int layer, Vec3* pos, void (*fn)(void*), void* arg);
void ActorGetMatrix(Mtx34* m, Actor* a);
void MtxSetEuler(Mtx34* m, Vec3* rot);
void PlacementToMatrix(Mtx34* m, PlaceF* p);
void ActorRender(Actor* a);
void ActorRelinkOwners(Actor* a);
void ActorUpdate(Actor* a, float dt);
int ActorDefaultMsg(Actor* a, int msg, void* data, int arg);

extern "C" {
void fn_8003D710(int model);
void fn_8003D8F4(int model);
void fn_8000F60C(int tex);
void fn_8000F874(int tex);
int fn_801D0C18(ImpThrow* t, int a, int b);
int fn_800C8198(void);
FireActor* fn_800C7E80(void* desc);
void fn_8016D440(Vec3* out, Vec3* in);
int fn_8016F120(void* fx, int* pos);
void fn_8016EEF4(int handle);
void fn_80211638(void* p, ImpThrow* t, int a, int b, int c, float age, float life, float angle, float f4);
void fn_80211470(void* p, int size, int count, float dt);
void fn_80210D80(void* p, int size, int count, int tex, int a, void* params, void* colors);
void fn_8007C5C8(ImpThrow* t, int a);
int fn_8001F45C(FireActor* a, IBox* box, void* pos, u32 flags, FireTouchFunc fn, FireActor* arg);
int fn_8001F928(FireActor* a, void* seg, u32 flags, FireHitFunc fn, FireActor* arg);
void fn_80158C3C(Vec3* pos, u32 type, int a, int b, int c, int d, int e);
int fn_80049940(void);
void fn_80122108(int a, int owner, Vec3* pos, u32 type, float f1, float f2, float f3, float f4, float f5,
                 float f6, float f7, int handle);
int fn_8001F0DC(FireActor* a, IVec3* pos, u32 flags, int b, int c, int d, int e, int f);
void fn_8003E8DC(int model, IBox* box, Mtx34* m, void* out);
void fn_800E0CB8(IVec3* out, IVec3* in);
void fn_8009F76C(FireActor* a);
void fn_800E0E9C(int* out, int* in, int scale);
}

// From a shared header: the ghost actions.
struct GhostAction {
    const char* name;
    int unk4;
    int time;
    float range;
    float range2;
    int hostOnly;
};

static GhostAction sGhostActions[] = {
    { "POSSESS", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "GHOSTACTION", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "SLIP", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "HIJACK", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "HOSTACTION", 0, 1000, 1600.0f, 4033.0f, 1 },
    { "EXCHANGE", 0, 1000, 1600.0f, 3600.0f, 1 },
    { "H_HIJACK", 0, 1000, 1600.0f, 3600.0f, 1 },
    { "TALK", 0, 1000, 1120.0f, 3600.0f, 1 },
    { "SCAN", 0, 1000, 1120.0f, 3600.0f, 1 },
};

struct FlameParams {
    float v[5];
};

struct FlameColors {
    float v[14];
};

static FlameParams sFlameParams[2] = {
    { 500.0f, 0.0f, 125.0f, 8.0f, 0.0f },
    { 500.0f, 0.0f, 0.0f, 160.0f, 0.0f },
};

static FlameColors sFlameColors[2] = {
    { 500.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
    { 200.0f, 300.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
};

int ImpFireBallMsg(FireActor* self, int msg, void* data, int arg);
void ImpFireBallUpdate(FireActor* a, float dt);
void ImpFireBallDestroy(FireActor* a);

static ActorType sImpFireBallType = {
    0, "Imp Fire", ImpFireBallMsg, ImpFireBallUpdate, 0, 0, ImpFireBallDestroy,
};

// Entry 0 is the fire ball's shadow model, entries 1-8 its flame textures.
SplineNameEntry gImpFireAssets[11] = {
    { "P:\\Fear\\Data\\Common models\\blob_shadow\\blob_shadow.amf", 0x87DE5E6A, 0, 0 },
    { "P:\\Fear\\Data\\Common Models\\fire_collision_box\\flame1b.tga", 0xF2E2BEB8, 0, 0 },
    { "P:\\Fear\\Data\\Common Models\\fire_collision_box\\flame2b.tga", 0xF2E2BEB8, 0, 0 },
    { "P:\\Fear\\Data\\Common Models\\fire_collision_box\\flame3b.tga", 0xF2E2BEB8, 0, 0 },
    { "P:\\Fear\\Data\\Common Models\\fire_collision_box\\flame4b.tga", 0xF2E2BEB8, 0, 0 },
    { "p:\\fear\\data\\common models\\fire_collision_box\\darksmoke.tga", 0xF2E2BEB8, 0, 0 },
    { "p:\\fear\\data\\common models\\fire_collision_box\\darksmoke2.tga", 0xF2E2BEB8, 0, 0 },
    { "p:\\Fear\\Data\\Particles\\BlueFire\\BlobOverlay.tga", 0xF2E2BEB8, 0, 0 },
    { "p:\\Fear\\Data\\Particles\\BlueFire\\BlueFire.tga", 0xF2E2BEB8, 0, 0 },
};

static int sFlameCounts[2] = { 2, 4 };
static float sFlameLife[2] = { 500.0f, 500.0f };
static float sFireDamage = 25.0f;

inline void ImpFireBallLoadAssets(void) {
    u32 i;

    fn_8003D8F4(SplineAssetResolve(gImpFireAssets, 9, 0));
    for (i = 1; i < 9; i++) {
        fn_8000F874(SplineAssetResolve(gImpFireAssets, 9, i));
    }
}

inline void ImpFireBallFreeAssets(void) {
    u32 i;

    fn_8003D710(SplineAssetResolve(gImpFireAssets, 9, 0));
    for (i = 1; i < 9; i++) {
        fn_8000F60C(SplineAssetResolve(gImpFireAssets, 9, i));
    }
}

struct ActorQuery {
    FireActor* actor;
    int unk4[7];
};

struct SpawnDesc {
    Vec3 pos;      // 0x00
    Vec3 rot;      // 0x0C
    int unk18;
    int def;       // 0x1C
    u32 kind;      // 0x20
    int unk24;
    int unk28;     // 0x28
    int unk2C[3];
};

int ImpFireBallSpawn(int owner, ImpThrow* t, int n) {
    ActorQuery q;
    SpawnDesc desc;
    float step;
    ImpFireBall* f;
    FireActor* a;
    int i;
    int j;
    int target = -1;
    FlameParticle* base;

    if (t == 0) {
        return 0;
    }
    if (n == 0) {
        return fn_801D0C18(t, 0, 0);
    }
    if (fn_800C8198() == 0) {
        return 0;
    }
    f = (ImpFireBall*)VAlloc(sizeof(ImpFireBall));
    if (f == 0) {
        return 0;
    }
    if (SendMsg(owner, 0x400, &q) != 0 && q.actor != 0) {
        switch (HandleKind(q.actor->handle)) {
        case 0x13:
            target = q.actor->target;
            break;
        case 2:
            Query(&target, q.actor->handle, 0x41E, 0xDFF3097A);
            break;
        }
    } else {
        return 0;
    }
    memset(f, 0, sizeof(ImpFireBall));
    desc.kind = 0x0D0270C1;
    desc.def = fn_800F7994(target, 0xC66596C0);
    desc.unk28 = -1;
    memcpy(&f->t, t, sizeof(ImpThrow));
    desc.pos.x = t->pos.x;
    desc.pos.y = t->pos.y;
    desc.pos.z = t->pos.z;
    fn_8016D440(&desc.rot, &t->vel);
    a = fn_800C7E80(&desc);
    if (a == 0) {
        VFree(f);
        return 0;
    }
    a->fire = f;
    a->type = &sImpFireBallType;
    f->unk0 = a->unk8C;
    f->owner = owner;
    f->fx = fn_8016F120(FxFind("imp_flame"), &a->x);
    SplineClearNameHashes(gImpFireAssets, 9);
    {
        base = (FlameParticle*)f->pad58;

        for (i = 0; i < 2; i++) {
            step = sFlameLife[i] / sFlameCounts[i];

            for (j = 0; j < sFlameCounts[i]; j++) {
                float r = rand() / 32767.0f;
                fn_80211638(&base[j], &f->t, 0, 0, 0, 250.0f - j * step, sFlameLife[i], 360.0f * r, 0.0f);
            }
            base += sFlameCounts[i];
        }
    }
    ImpFireBallLoadAssets();
    return 1;
}

void ImpFireBallDestroy(FireActor* a) {
    ImpFireBall* f = a->fire;

    ImpFireBallFreeAssets();
    if (f->fx != -1) {
        fn_8016EEF4(f->fx);
        f->fx = -1;
    }
    VFree(f);
}

int ImpFireBallTouch(FireActor* o, FireActor* a);
int ImpFireBallHit(void* hit, FireActor* a);

struct FireSegment {
    IVec3 from;
    IVec3 to;
};

void ImpFireBallUpdate(FireActor* a, float dt) {
    int ox;
    int oy;
    int oz;
    ImpFireBall* f = a->fire;
    Vec3 d = { 0.0f, 0.0f, 0.0f };
    Vec3 rot;
    Vec3 pos;
    FireSegment seg;
    Mtx34 m;
    int i;

    ox = a->x;
    oy = a->y;
    oz = a->z;
    fn_8007C5C8(&f->t, 0);
    d.x = a->x;
    d.y = a->y;
    d.z = a->z;
    d.x = f->t.pos.x - d.x;
    d.y = f->t.pos.y - d.y;
    d.z = f->t.pos.z - d.z;
    fn_8016D440(&rot, &f->t.dir);
    MtxSetEuler(&m, &rot);
    PlacementToMatrix((Mtx34*)a, (PlaceF*)&m);
    a->x = f->t.pos.x;
    a->y = f->t.pos.y;
    a->z = f->t.pos.z;
    {
        FlameParticle* base = (FlameParticle*)f->pad58;

        for (i = 0; i < 2; i++) {
            FlameParticle* p = base;
            int j;

            for (j = 0; j < sFlameCounts[i]; j++) {
                p->pos.x += d.x;
                p->pos.y += d.y;
                p->pos.z += d.z;
                if (p->life <= 0.0f) {
                    p->age = 0.0f;
                    p->life = sFlameLife[i];
                }
                p++;
            }
            fn_80211470(base, sizeof(FlameParticle), sFlameCounts[i], dt);
            base += sFlameCounts[i];
        }
    }
    seg.from.x = ox;
    seg.from.y = oy;
    seg.from.z = oz;
    seg.to.x = a->x;
    seg.to.y = a->y;
    seg.to.z = a->z;
    fn_8001F45C(a, a->box, a, 0x40000000, ImpFireBallTouch, a);
    if (fn_8001F928(a, &seg, 0x12000000, ImpFireBallHit, a)) {
        if (f->fx != -1) {
            fn_8016EEF4(f->fx);
            f->fx = -1;
        }
        pos.x = a->x;
        pos.y = a->y;
        pos.z = a->z;
        fn_80158C3C(&pos, 0x12DD2E2A, -1, -1, -1, -1, -1);
        if (fn_80049940()) {
            fn_80122108(0, f->owner, &pos, 0x12DD2E2A, 400.0f, 1000.0f, 16000.0f, 16000.0f, 0.75f, 0.4f, 500.0f,
                        a->handle);
        } else {
            fn_80122108(0, f->owner, &pos, 0x12DD2E2A, 200.0f, 500.0f, 500.0f, 100.0f, 0.75f, 0.4f, 500.0f,
                        a->handle);
        }
        ActorRender((Actor*)a);
    } else {
        ActorGetMatrix(&a->mtx, (Actor*)a);
        a->z118 = a->z;
        a->flags |= 0x800000;
        ActorRelinkOwners((Actor*)a);
        ActorUpdate((Actor*)a, dt);
    }
}

void ImpFireBallDraw(void* p);

// Payload of the actor messages: the actor, then message specific data (the draw list for message 0).
struct ActorMsgData {
    FireActor* actor;
    u8 pad04[0x10 - 0x04];
    u8 out[4];  // 0x10
};

// Message 0: queues the flame particles for drawing and draws the fire ball's shadow.
inline int ImpFireBallShadow(FireActor* a, ImpFireBall* f, void* data) {
    ActorMsgData* d = (ActorMsgData*)data;
    Mtx34 m;
    IVec3 pos;
    IBox box;

    DeferAdd(1, &f->t.pos, ImpFireBallDraw, a);
    ActorGetMatrix(&m, (Actor*)a);
    pos.x = a->x;
    pos.y = a->y;
    pos.z = a->z;
    m.m[2][3] = fn_8001F0DC(a, &pos, 0x4000000, 0, 0, 0, 0, 0);
    m.m[2][3] += 25.0f;
    box = *a->box;
    box.max.x *= 0.33f;
    box.max.y *= 0.33f;
    box.max.z *= 0.33f;
    box.min.x *= 0.33f;
    box.min.y *= 0.33f;
    box.min.z *= 0.33f;
    fn_8003E8DC(SplineAssetResolve(gImpFireAssets, 9, 0), &box, &m, d->out);
    return 1;
}

int ImpFireBallMsg(FireActor* self, int msg, void* data, int arg) {
    ActorMsgData* d = (ActorMsgData*)data;
    FireActor* a = d->actor;
    ImpFireBall* f = a->fire;

    switch (msg) {
    case 0x407:
        return 1;
    case 0:
        return ImpFireBallShadow(a, f, data);
    case 1:
        return 0;
    case 3:
    case 4:
        return 0;
    case 0x41E:
    default:
        return ActorDefaultMsg((Actor*)self, msg, d, arg);
    }
}

void ImpFireBallDraw(void* p) {
    FireActor* a = (FireActor*)p;
    FlameParticle* base = (FlameParticle*)a->fire->pad58;
    int i;

    for (i = 0; i < 2; i++) {
        fn_80210D80(base, sizeof(FlameParticle), sFlameCounts[i], SplineAssetResolve(gImpFireAssets, 9, i + 7), 0,
                    &sFlameParams[i], &sFlameColors[i]);
        base += sFlameCounts[i];
    }
}

// Burns an actor the fire ball flies through (not the imp that threw it).
struct BurnMsg {
    int unk0;
    int unk4;
    int target;     // 0x08
    float damage;   // 0x0C
    int unk10;
    u32 kind;       // 0x14
    IVec3 pos;      // 0x18
    int unk24[3];
    IVec3 dir;      // 0x30
    int unk3C[2];
    int unk44;      // 0x44
    int unk48;
    float force;    // 0x4C
    int unk50[4];
};

int ImpFireBallTouch(FireActor* o, FireActor* a) {
    ImpFireBall* f = a->fire;
    BurnMsg m;

    if (o != 0 && o != a && o->handle != f->owner) {
        m.target = o->handle;
        m.damage = sFireDamage;
        m.kind = 0x12DD2E2A;
        m.force = 0.1f;
        m.unk44 = -1;
        m.pos.x = f->t.pos.x;
        m.pos.y = f->t.pos.y;
        m.pos.z = f->t.pos.z;
        m.dir.x = o->x - m.pos.x;
        m.dir.y = o->y - m.pos.y;
        m.dir.z = o->z - m.pos.z;
        if ((m.dir.x | m.dir.y | m.dir.z) == 0) {
            m.dir.x = 0;
            m.dir.y = 0;
            m.dir.z = 0x1000;
        } else {
            fn_800E0CB8(&m.dir, &m.dir);
        }
        SendMsg(o->handle, 0x407, &m);
    }
    return 1;
}

struct FireHit {
    FireActor* actor;   // 0x00
    u8 pad04[0x30 - 0x04];
    int push[3];        // 0x30
    int normal[3];      // 0x3C
};

// Collision callback of the fire ball's flight: stops it on anything but its thrower (and its own kind).
int ImpFireBallHit(void* p, FireActor* a) {
    FireHit* hit = (FireHit*)p;
    ImpFireBall* f = a->fire;

    if (hit->actor != 0) {
        if (hit->actor == a || hit->actor->handle == f->owner) {
            return 0;
        }
        if (HandleKind(hit->actor->handle) == 0x13 && hit->actor->kind != 0 && hit->actor->kind[0] == 0x0D028AC2) {
            return 0;
        }
    }
    f->t.dir.x = 0.0f;
    f->t.dir.y = 0.0f;
    f->t.dir.z = 0.0f;
    f->t.vel.x = 0.0f;
    f->t.vel.y = 0.0f;
    f->t.vel.z = 0.0f;
    f->t.spin.x = 0.0f;
    f->t.spin.y = 0.0f;
    f->t.spin.z = 0.0f;
    f->t.accel.x = 0.0f;
    f->t.accel.y = 0.0f;
    f->t.accel.z = 0.0f;
    fn_8009F76C(a);
    fn_800E0E9C(&a->x, hit->normal, 100);
    a->x += hit->push[0];
    a->y += hit->push[1];
    a->z += hit->push[2];
    a->y += a->height;
    return 1;
}


int GunHostCheckGrenade(Actor* a);
int GunHostCheckGun(Actor* a);
int GunHostCheckReload(Actor* a, float dt);
int GunHostMsg(int msg, void* data);

int AssetFind(u32 table, u32 key);

extern int lbl_804A2930[16];  // the host's sounds

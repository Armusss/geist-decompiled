// u_80254F54.cpp: the host type callbacks (0x80254F54-0x8025643C, entries 5-22 of the host table) and the
// gun-with-grenade host states (0x8025643C-0x8025808C). Split from the imp fire ball: its own string pool
// (POSSESS at 0x8034A978, then "gun_empty") and its own .sdata (from 0x804D3620) and .sdata2 (from 0x804D9E98).
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
void fn_8006D184(Actor* a, int state);
void fn_8006D288(Actor* a, int state);
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
void fn_80072058(Actor* a, float dt);
void fn_8006D17C(int* type, int state);
void fn_8006D76C(Actor* a);
HostData* fn_800A384C(Actor* a);
void fn_80155A84(void* grab, Actor* host, int a);
int fn_8006CAB8(Actor* a);
void fn_8006CAC0(Actor* a, int b);
int fn_80175EFC(int a);
void fn_80050EB0(Actor* a);
}

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

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// Declarations shared with the imp fire ball (u_802541D4.cpp).
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
int GunHostCheckGrenade(Actor* a);
int GunHostCheckGun(Actor* a);
int GunHostCheckReload(Actor* a, float dt);
int GunHostMsg(int msg, void* data);

int AssetFind(u32 table, u32 key);

extern int lbl_804A2930[16];  // the host's sounds

// ---------------------------------------------------------------------------------------------------
// 0x80254F54-0x8025643C: more host type callbacks (entries 5-22 of the host table; entry 12 points to
// 0x80243778-0x80243988, outside this file). Each entry holds an enter, update and message callback.

void GunHostState22Update(Actor* a, float dt) {
    a->unk638 = 0;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    if (a->prevAnim == -1) {
        if (a->unk7D0 != 0) {
            fn_8006D17C(&a->type, 3);
        } else {
            fn_8006D17C(&a->type, 1);
        }
    } else {
        fn_8006D17C(&a->type, 22);
    }
}

void GunHostState22Enter(Actor* a) {
    a->msgHandler = 0;
    if (a->prevAnim != -1) {
        a->anim = a->prevAnim;
    }
    if (a->prevAnim2 != -1) {
        a->anim2 = a->prevAnim2;
    }
}

void GunHostState21Update(Actor* a, float dt) {
    fn_8006D76C(a);
    a->unk638 = 1;
    a->flags &= ~1;
    fn_8006D17C(&a->type, 21);
    if (a->unk279C == -1 || fn_80175EFC(a->unk279C) != 0) {
        fn_8006D17C(&a->type, 3);
        fn_80155A84(a->grab, a->host, 1);
    }
}

void GunHostState21Enter(Actor* a) {
    Actor* h = a->host;
    HostData* hd;
    int cond = 0;
    int weapon;

    if (HandleKind(h->handle) == 0x13) {
        hd = fn_800A384C(h);
        if (HasCondition(h->conditions, 11)) {
            cond = 1;
        }
    }
    PlayerInHost(a);
    weapon = a->weapon;
    a->anim = fn_800A1BC8(hd->set, hd->aim, lbl_803BF068[21].actions[weapon], cond, 0);
    a->msgHandler = GunHostMsg;
    a->animMode = 25;
    a->anim2 = fn_800A1BC8(hd->set, hd->walk, lbl_803BF068[21].actions[weapon], cond, 0);
    a->animMode2 = 25;
    fn_8006D17C(&a->type, 21);
}

void GunHostState20Update(Actor* a, float dt) {
    fn_8006D76C(a);
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    if (fn_8006CAB8(a)) {
        fn_8006D17C(&a->type, 2);
    } else {
        fn_8006D17C(&a->type, 1);
    }
    PlayerInHost(a);
    DashUpdate(a);
}

void GunHostState20Enter(Actor* a) {
    int host = PlayerInHost(a);
    int weapon = a->weapon;

    a->unk78 = 0;
    a->flags50 &= ~0x20000;
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[20].actions[weapon], 0, 0);
    a->msgHandler = GunHostMsg;
    a->animMode = 1;
    a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[20].actions[weapon], 0, 0);
    a->animMode2 = 1;
    {
        QueryMsg m;

        m.hash = 0x08783452;
        SendMsg(a->holder->handle, 0x41E, &m);
        if (*(int*)&m.result != 0) {
            fn_8006CAC0(a, 0);
            if (a->anim == -1) {
                fn_8006D288(a, 1);
            }
        } else {
            fn_8006CAC0(a, 1);
            if (a->anim == -1) {
                fn_8006D288(a, 3);
            }
        }
    }
}

void GunHostState19Update(Actor* a, float dt) {
    fn_8006D76C(a);
    a->unk638 = 1;
    if (a->flags & 1) {
        a->flags &= ~1;
    } else {
        a->flags50 |= 0x20000;
        a->unk78 = 1;
    }
}

void GunHostState19Enter(Actor* a) {
    Actor* h = a->host;
    HostData* hd;
    int host;
    int weapon;
    int cond = 0;

    if (HandleKind(h->handle) == 0x13) {
        hd = fn_800A384C(h);
        if (HasCondition(h->conditions, 11)) {
            cond = 1;
        }
    }
    host = PlayerInHost(a);
    weapon = a->weapon;
    a->anim = fn_800A1BC8(hd->set, hd->aim, lbl_803BF068[19].actions[weapon], cond, 0);
    a->msgHandler = GunHostMsg;
    a->animMode = 25;
    a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[19].actions[weapon], 0, 0);
    a->animMode2 = 25;
    {
        QueryMsg m;

        m.hash = 0x08783452;
        SendMsg(a->host->handle, 0x41E, &m);
        if (*(int*)&m.result != 0) {
            fn_8006CAC0(a, 0);
        } else {
            fn_8006CAC0(a, 1);
        }
    }
}

void GunHostState18Msg(Actor* a) {
    fn_80155A84(a->grab, a->host, 1);
}

void GunHostState18Update(Actor* a, float dt) {
    fn_8006D76C(a);
    a->unk638 = 1;
    a->flags &= ~1;
    a->flags50 |= 0x20000;
    a->unk78 = 1;
    fn_8006D17C(&a->type, 19);
}

void GunHostState18Enter(Actor* a) {
    int host = PlayerInHost(a);
    Actor* h = a->host;
    HostData* hd;
    int weapon = a->weapon;
    int cond = 0;

    if (HandleKind(h->handle) == 0x13) {
        hd = fn_800A384C(h);
        if (HasCondition(h->conditions, 11)) {
            cond = 1;
        }
    }
    a->anim = fn_800A1BC8(hd->set, hd->aim, lbl_803BF068[18].actions[weapon], cond, 0);
    a->msgHandler = GunHostMsg;
    a->animMode = 89;
    a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[18].actions[weapon], 0, 0);
    a->animMode2 = 89;
    {
        QueryMsg m;

        m.hash = 0x08783452;
        SendMsg(a->host->handle, 0x41E, &m);
        if (*(int*)&m.result != 0) {
            fn_8006CAC0(a, 0);
        } else {
            fn_8006CAC0(a, 1);
        }
    }
}

void GunHostState13Msg(Actor* a) {
    a->flags50 &= ~0x40000;
}

void GunHostState13Update(Actor* a, float dt) {
    fn_8006D76C(a);
    a->unk638 = 1;
    a->flags &= ~1;
    fn_80072058(a, dt);
}

void GunHostState13Enter(Actor* a) {
    int host;
    int weapon;
    int chain;

    fn_8006D76C(a);
    a->flags50 |= 0x40000;
    fn_80050EB0(a);
    host = PlayerInHost(a);
    weapon = a->weapon;
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[13].actions[weapon], 0, 0);
    a->animMode = 80;
    if (a->bone == -1) {
        chain = 8;
    } else {
        chain = ModelBoneNodeChain(a->model58, ModelBoneName(a->model58, a->bone), lbl_804A2930, 16);
        if (chain == -1) {
            chain = 8;
        }
    }
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[13].actions[weapon], chain * 2, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[13].actions[weapon], chain * 2, 0);
    }
    a->msgHandler2 = GunHostMsg;
    a->animMode2 = 80;
}

void GunHostState11Update(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    DashUpdate(a);
    fn_8006D17C(&a->type, 3);
}

void GunHostState11Enter(Actor* a) {
    int r1;
    int r2;
    int host;
    int weapon;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    fn_80048BA8(a, a->buf, 1);
    Fn800F7F5C(r1, r2, 0xBC78F550, fn_800498D0(a), a->buf, a->speed);
    host = PlayerInHost(a);
    weapon = a->weapon;
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[11].actions[weapon], 0, 0);
    a->msgHandler = GunHostMsg;
    a->animMode = 17;
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[11].actions[weapon], 0, 0);
        if (a->anim2 == fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[3].actions[weapon], 0, 0)) {
            a->animMode2 = 1;
        } else {
            a->animMode2 = 17;
        }
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[11].actions[weapon], 0, 0);
        if (a->anim2 == fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[3].actions[weapon], 0, 0)) {
            a->animMode2 = 1;
        } else {
            a->animMode2 = 17;
        }
    }
}

void GunHostState10Update(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    DashUpdate(a);
    fn_8006D17C(&a->type, 3);
}

void GunHostState10Enter(Actor* a) {
    int r1;
    int r2;
    int host;
    int weapon;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    fn_80048BA8(a, a->buf, 1);
    Fn800F7F5C(r1, r2, 0x8FDD7599, fn_800498D0(a), a->buf, a->speed);
    host = PlayerInHost(a);
    weapon = a->weapon;
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[10].actions[weapon], 0, 0);
    a->msgHandler = GunHostMsg;
    a->animMode = 17;
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[10].actions[weapon], 0, 0);
        if (a->anim2 == fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[3].actions[weapon], 0, 0)) {
            a->animMode2 = 1;
        } else {
            a->animMode2 = 17;
        }
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[10].actions[weapon], 0, 0);
        if (a->anim2 == fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[3].actions[weapon], 0, 0)) {
            a->animMode2 = 1;
        } else {
            a->animMode2 = 17;
        }
    }
}

void GunHostState9Msg(Actor* a) {
    ActorPlayAnim(0x2000000, 0xD39F69B4, fn_800498D0(a));
}

void GunHostState9Update(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    DashUpdate(a);
    fn_8006D17C(&a->type, 1);
}

void GunHostState9Enter(Actor* a) {
    int r2;
    int host;
    int weapon;

    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    host = PlayerInHost(a);
    weapon = a->weapon;
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[9].actions[weapon], 0, 0);
    a->msgHandler = GunHostMsg;
    a->animMode = 16;
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[9].actions[weapon], 0, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[9].actions[weapon], 0, 0);
    }
    a->animMode2 = 16;
    lbl_804D3624 = fn_800F7994(r2, 0x9D1C245C);
    FxStart((void*)lbl_804D3624, 0, 0);
}

void GunHostState8Update(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    fn_8006D17C(&a->type, 8);
    DashUpdate(a);
    if (a->grenades == 0) {
        fn_8006D17C(&a->type, 3);
    } else {
        fn_8006D76C(a);
    }
}

void GunHostState8Enter(Actor* a) {
    int r2;
    int host;
    int weapon;

    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    host = PlayerInHost(a);
    weapon = a->weapon;
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[8].actions[weapon], 0, 0);
    a->msgHandler = GunHostMsg;
    a->animMode = 1;
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[8].actions[weapon], 0, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[8].actions[weapon], 0, 0);
    }
    a->msgHandler2 = 0;
    a->animMode2 = 1;
    lbl_804D3628 = fn_800F7994(r2, 0x6E56393C);
    fn_8016F120((void*)lbl_804D3628, a->pos);
}

void GunHostState5Update(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    fn_8006D17C(&a->type, 5);
    DashUpdate(a);
    if (a->ammo == 0 || a->grenades != 0) {
        fn_8006D17C(&a->type, 3);
    } else {
        fn_8006D76C(a);
    }
}

void GunHostState5Enter(Actor* a) {
    int host = PlayerInHost(a);
    int weapon = a->weapon;

    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[5].actions[weapon], 0, 0);
    a->msgHandler = GunHostMsg;
    a->animMode = 1;
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[5].actions[weapon], 0, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[5].actions[weapon], 0, 0);
    }
    a->animMode2 = 1;
}

void GunHostAimUpdate(Actor* a, float dt) {
    int r1;
    int r2;
    float m;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    a->unk7D0 = 1;
    {
        float z = a->move[2];
        float x = a->move[0];

        m = __fabsf(x) > __fabsf(z) ? __fabsf(x) : __fabsf(z);
    }
    if (m != 0.0f) {
        fn_8008E824(a->model, 0);
    }
    if (a->unk7D8 == 0.0f) {
        fn_8006D184(a, 0);
    } else if (a->flags678 & 0x80) {
        a->flags678 &= ~0x80;
        fn_8006D288(a, 3);
    } else {
        fn_8006D17C(&a->type, 3);
        DashUpdate(a);
        if (!GunHostCheckGrenade(a) && !GunHostCheckGun(a)) {
            GunHostCheckReload(a, dt);
        }
    }
}

void GunHostAimEnter(Actor* a) {
    int weapon;
    int r1;
    int r2;
    int host;
    int empty;
    int r;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    host = PlayerInHost(a);
    weapon = a->weapon;
    r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(a), 0);
    empty = 0;
    if (r == 0xC0BECF7E || r == 0x11D2C025) {
        empty = 1;
    }
    a->anim = -1;
    while (a->anim == -1) {
        a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[3].actions[weapon], empty, 0);
        a->msgHandler = GunHostMsg;
        a->animMode = 1;
        if (a->anim == -1 && empty) {
            empty = 0;
        }
    }
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[3].actions[weapon], 0, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[3].actions[weapon], 0, 0);
    }
    a->animMode2 = 1;
}

void GunHostSignal(Actor* a) {
    ActorPlayAnim(0x2000000, 0x8B3A27A3, fn_800498D0(a));
}

void GunHostIdleUpdate(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    DashUpdate(a);
    fn_8006D17C(&a->type, 3);
}

void GunHostFireEnter(Actor* a) {
    int r2;
    int host;
    int weapon;

    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    host = PlayerInHost(a);
    weapon = a->weapon;
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[2].actions[weapon], 0, 0);
    a->animMode = 16;
    a->msgHandler = GunHostMsg;
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[2].actions[weapon], 0, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[2].actions[weapon], 0, 0);
    }
    a->animMode2 = 16;
    lbl_804D3620 = fn_800F7994(r2, 0xAFEF562B);
    FxStart((void*)lbl_804D3620, 0, 0);
}

void GunHostFireUpdate(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    if (a->flags678 & 0x80) {
        a->flags678 &= ~0x80;
        fn_8006D288(a, 1);
    }
    fn_8006D17C(&a->type, 1);
    a->unk7D0 = 0;
    DashUpdate(a);
    if (!GunHostCheckGun(a) && !GunHostCheckGrenade(a) && (a->pressed & 1)) {
        if (a->unk7D0 == 0) {
            fn_8006D184(a, 2);
        } else {
            fn_8006D184(a, 9);
        }
    }
}

void GunHostIdleEnter(Actor* a) {
    int host = PlayerInHost(a);
    int weapon = a->weapon;

    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[1].actions[weapon], 0, 0);
    a->animMode = 1;
    a->msgHandler = 0;
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[1].actions[weapon], 0, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[1].actions[weapon], 0, 0);
    }
    a->msgHandler2 = GunHostMsg;
    a->animMode2 = 1;
    a->unk7D0 = 0;
}

void GunHostStop(Actor* a) {
    fn_8006D76C(a);
}

void GunHostWalkUpdate(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    a->unk7D0 = 1;
    fn_8006D17C(&a->type, 3);
    DashUpdate(a);
    if (!GunHostCheckGun(a) && !GunHostCheckGrenade(a)) {
        GunHostCheckReload(a, dt);
    }
    if (a->move[2] != 0.0f || a->move[0] != 0.0f || a->move[1] != 0.0f || a->move[3] != 0.0f || a->unk328 != 0) {
        fn_8006D288(a, 3);
    }
}

void GunHostThrowEnter(Actor* a) {
    int r1;
    int r2;
    int host;
    int r;
    int walk;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(a), 0);
    if (r == 0xC0BECF7E || r == 0x11D2C025) {
        a->variant = 2;
    } else if (rand() / 32767.0f >= 0.5f) {
        a->variant = 0;
    } else {
        a->variant = 1;
    }
    host = PlayerInHost(a);
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[0].actions[0], a->variant, 0);
    if (a->anim == -1) {
        a->variant = 0;
        a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[0].actions[0], a->variant, 0);
    }
    a->msgHandler = GunHostMsg;
    a->animMode = 1;
    {
        HostData* h = a->hosts[host];
        int set = h->set;

        if (a->flags & 0x80000) {
            walk = h->run;
        } else {
            walk = h->walk;
        }
        a->anim2 = fn_800A1BC8(set, walk, lbl_803BF068[0].actions[0], a->variant, 0);
    }
    if (a->anim2 == -1) {
        a->variant = 0;
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, walk, lbl_803BF068[0].actions[0], a->variant, 0);
    }
    a->msgHandler2 = GunHostMsg;
    a->animMode2 = 1;
}

int GunHostCheckReload(Actor* a, float dt) {
    int r1;
    int r2;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    if (a->mode == 5 || a->mode == 4) {
        if (a->pressed & 1) {
            fn_8006D76C(a);
            if (a->unk7D0 == 0) {
                fn_8006D184(a, 2);
            } else {
                fn_8006D184(a, 9);
            }
            return 1;
        }
        if (a->pressed2 & 1) {
            fn_8006D76C(a);
            if (Fn800F7DEC(r1, r2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 10);
                return 1;
            }
            if (Fn800F7DEC(r1, r2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 11);
                return 1;
            }
        }
    } else if (a->pressed != 0) {
        fn_8006D76C(a);
        a->hold += dt;
        if (fn_800699B8(a) == 1.0f) {
            if (a->unk7D0 == 0) {
                fn_8006D184(a, 2);
            } else {
                fn_8006D184(a, 9);
            }
            fn_800699A8(a->timer);
            return 1;
        }
    } else {
        float t = fn_800699B8(a);
        if (t > -1.0f && t < 0.0f) {
            if (Fn800F7DEC(r1, r2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 10);
                fn_800699A8(a->timer);
                return 1;
            }
            if (Fn800F7DEC(r1, r2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 11);
                fn_800699A8(a->timer);
                return 1;
            }
        }
        fn_800699A8(a->timer);
    }
    return 0;
}

int GunHostCheckGrenade(Actor* a) {
    int r1;
    int r2;
    int r;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    if (a->grenades >= 1) {
        if (a->type == 1) {
            fn_8006D184(a, 2);
            return 1;
        }
        fn_8006D76C(a);
        r = Fn800F7DEC(r1, r2, 0xF829DB93, fn_800498D0(a), 0);
        switch (r) {
        case 0x04A3E183:
            if (a->type != 8) {
                fn_8006D184(a, 8);
                return 1;
            }
            break;
        case 0xC0BECF7E:
            if (Fn800F7DEC(r1, r2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 11);
                return 1;
            }
            break;
        case 0x11D2C025:
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                QueryValue(a->handle, 0x7FFCFD4F, 0x91A0CEF7);
            }
            if (a->type == 8) {
                fn_8006D184(a, 3);
            }
            break;
        case 0x3DA260F7:
            if (a->type == 8) {
                fn_8006D184(a, 3);
            }
            break;
        }
    } else if (a->type == 8) {
        fn_8006D184(a, 3);
    }
    return 0;
}

int GunHostCheckGun(Actor* a) {
    int r1;
    int r2;
    int r;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    if (a->ammo >= 1) {
        if (a->type == 1) {
            fn_8006D184(a, 2);
            return 1;
        }
        fn_8006D76C(a);
        r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(a), 0);
        switch (r) {
        case 0x04A3E183:
            fn_8006D184(a, 5);
            fn_8006D17C(&a->type, 5);
            return 1;
        case 0xC0BECF7E:
            if (Fn800F7DEC(r1, r2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 10);
                return 1;
            }
            break;
        case 0x11D2C025:
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                QueryValue(a->handle, 0x7FFCFD4F, 0x91A0CEF7);
            }
            if (a->type == 5) {
                fn_8006D184(a, 3);
            }
            break;
        case 0x3DA260F7:
            if (a->type == 5) {
                fn_8006D184(a, 3);
            }
            break;
        }
    } else if (a->type == 5) {
        fn_8006D184(a, 3);
    }
    return 0;
}

// Animation message handler of the gun states: passes fire/reload actions on to the host's weapon.
int GunHostMsg(int msg, void* data) {
    Actor* a = (Actor*)data;
    MsgQuery q0;
    int r1;
    int r2;
    int r;

    q0.result = a->handle;
    q0.hash = msg;
    if (fn_800C5210(a->handle, 0xC06, &q0) == 1) {
        return 0;
    }
    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    switch (msg) {
    case 0x1B83833F:
        if (a->type == 5) {
            r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(a), 0);
            if (r != 0x04A3E183) {
                if (r == 0xC0BECF7E || r == 0x11D2C025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                fn_8006D184(a, 3);
            } else {
                fn_8006D288(a, a->nextType);
            }
        } else if (a->type == 8) {
            r = Fn800F7DEC(r1, r2, 0xF829DB93, fn_800498D0(a), 0);
            if (r != 0x04A3E183) {
                if (r == 0xC0BECF7E || r == 0x11D2C025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                fn_8006D184(a, 3);
            } else {
                fn_8006D288(a, a->nextType);
            }
        } else if (a->type == 13) {
            a->flags |= 0x20000;
        } else {
            fn_8006D288(a, a->nextType);
        }
        break;
    case 0x0020EE36:
        fn_80048BA8(a, a->buf, 1);
        a->bufAim = a->aim;
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        break;
    case 0xF829DB93:
        fn_80048BA8(a, a->buf, 0);
        a->bufAim = a->aim;
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        return 1;
    case 0x9791AF51:
    case 0x96224FCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        break;
    case 0xE6B71BCC:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        break;
    case 0x968CBE3D:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        break;
    }
    return 0;
}


void* GunHostTypeData(Actor* actor) {
    return lbl_803BF068[actor->type].name;
}

void GunHostTypeMsg(Actor* actor) {
    if (lbl_803BF068[actor->type].msg) {
        lbl_803BF068[actor->type].msg(actor);
    }
}

// Raises the aim while in the aiming state (type 5), lowers it otherwise.
void GunHostTypeUpdate(Actor* actor, int arg, float dt) {
    if (actor->type == 5) {
        if (actor->aim < 1.0f) {
            actor->aim = 0.0005f * dt + actor->aim;
        }
        actor->aim = actor->aim < 1.0f ? actor->aim : 1.0f;
    } else {
        if (actor->aim > 0.0f) {
            actor->aim = -(0.001f * dt - actor->aim);
        }
        actor->aim = actor->aim > 0.0f ? actor->aim : 0.0f;
    }
    if (lbl_803BF068[actor->type].update) {
        lbl_803BF068[actor->type].update(actor, arg, dt);
    }
}

void GunHostTypeEnter(Actor* actor) {
    if (lbl_803BF068[actor->type].enter) {
        lbl_803BF068[actor->type].enter(actor);
    }
}

void GunHostLoadSounds(void) {
    lbl_804A2930[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_804A2930[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_804A2930[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_804A2930[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_804A2930[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_804A2930[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_804A2930[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_804A2930[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_804A2930[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_804A2930[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_804A2930[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_804A2930[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_804A2930[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_804A2930[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_804A2930[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_804A2930[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

// u_80244728.cpp: foe (guard) AI behaviour of an enemy actor (0x80244728-0x80247EB0).
// A six-state machine (idle, alert, chase, attack, stun, ...) driven from a state table of
// {enter, canLeave, update, name} records.  The actor message handler answers the state-name,
// owner and target queries, applies hit damage (with bone based multipliers) and saves/restores
// the 0x5C byte behaviour block.  Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the actor actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

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
    String(const char* s);
    ~String() {}
    virtual void Release(char* p);
};

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[12];
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

extern "C" {
void* memcpy(void* dst, const void* src, unsigned long n);
void* memset(void* dst, int c, unsigned long n);
int rand(void);
int SendMsg(int to, int msg, void* data);
}

struct FoeBeh;
struct Actor;

struct ActorInfo {
    char pad0[0x30];
    int maxHealth;          // 0x30
};

// Target as seen through the 0x400 query (position, flags).
struct Target {
    char pad0[0x24];
    int pos[3];             // 0x24
    char pad30[8];
    u32 id;                 // 0x38
};

// Actor (only the fields used here).
struct Actor {
    char pad0[0x24];
    int pos[3];             // 0x24
    char pad30[8];          // 0x30
    int handle;             // 0x38
    char pad3C[0x4C - 0x3C];
    u32 flags4C;            // 0x4C
    char pad50[0x84 - 0x50];
    float p84[3];           // 0x84
    char pad90[0xC4 - 0x90];
    ActorInfo* info;        // 0xC4
    char padC8[4];          // 0xC8
    int model;              // 0xCC
    int unkD0;              // 0xD0
    int unkD4;              // 0xD4
    char padD8[0xF0 - 0xD8];
    float* pose;            // 0xF0
    char padF4[0xFC - 0xF4];
    void* unkFC;            // 0xFC
    char pad100[4];
    int (*update)(Actor*);  // 0x104
    char pad108[4];
    void* unk10C;           // 0x10C
    char pad110[0x130 - 0x110];
    FoeBeh* beh;            // 0x130
    char pad134[4];
    float dt;               // 0x138
    float unk13C;           // 0x13C
    char pad140[0x214 - 0x140];
    int unk214;             // 0x214
    char pad218[4];
    int unk21C;             // 0x21C
    int unk220;             // 0x220
    char pad224[4];
    int unk228;             // 0x228
    int unk22C;             // 0x22C
    char pad230[0x268 - 0x230];
    int unk268;             // 0x268
    int unk26C;             // 0x26C
    char flags[0x288 - 0x270]; // 0x270 condition bits
    int unk288;             // 0x288
    char pad28C[0x2C4 - 0x28C];
    int unk2C4;             // 0x2C4
    char pad2C8[0x5F0 - 0x2C8];
    int unk5F0;             // 0x5F0
    char pad5F4[4];
    void* unk5F8;           // 0x5F8
    void* unk5FC;           // 0x5FC
    void* unk600;           // 0x600
    void* unk604;           // 0x604
    char pad608[0x894 - 0x608];
    char sm[0x8F8 - 0x894]; // 0x894 sub state machine
    int unk8F8;             // 0x8F8
    char pad8FC[0x904 - 0x8FC];
    float health;           // 0x904
};

// Behaviour block (VAlloc'd, 0x5C bytes).
struct FoeBeh {
    int state;              // 0x00
    float maxHp;            // 0x04
    float hp;               // 0x08
    float timerA;           // 0x0C
    float timerB;           // 0x10
    u32 flags;              // 0x14
    int unk18;              // 0x18
    int targetId;           // 0x1C
    float unk20;            // 0x20
    float stunTimer;        // 0x24
    int link;               // 0x28
    int fire;               // 0x2C
    int unk30;              // 0x30
    int bones[10];          // 0x34
};

typedef int (*EnterFn)(Actor*);
typedef int (*LeaveFn)(Actor*);
typedef int (*UpdateFn)(Actor*);

struct StateDef {
    EnterFn enter;          // 0x00
    LeaveFn canLeave;       // 0x04
    UpdateFn update;        // 0x08
    const char* name;       // 0x0C
};

extern StateDef gFoeStates[6];

// Message block seen by the actor message handler.
struct Msg {
    Actor* actor;           // 0x00
    int key;                // 0x04
    int arg08;              // 0x08
    int arg0C;              // 0x0C
    int arg10;              // 0x10
    int arg14;              // 0x14
    int arg18;              // 0x18
    int arg1C;              // 0x1C
    char pad20[0x44 - 0x20];
    int arg44;              // 0x44
    char pad48[0x58 - 0x48];
    int arg58;              // 0x58
};

struct EventMsg {
    int unk0;
    int value;              // 0x04
    int target;             // 0x08
    u32 key;                // 0x0C
};

extern "C" {
void fn_800BBCA8(Actor* a);
int fn_80162610(Actor* a, void* sm, u32 msg);
int fn_8016325C(void* sm);
void fn_80162658(Actor* a, void* sm);
int fn_80162940(void* sm, int n);
void fn_800CE96C(void);
void fn_800CF064(void);
void fn_800CC984(void);
int fn_8008EA9C(int a, int b);
int fn_800BA84C(int n);
void fn_800B20B8(Actor* a, int id);
int fn_800B26B8(Actor* a);
void fn_800B261C(Actor* a, int i, int* out);
int fn_800B22F8(Actor* a, int i);
int fn_800B1EF4(Actor* a, int i);
void fn_800B2418(float f, Actor* a, int i);
void fn_800B1F20(Actor* a, int i);
void fn_800B1AB8(float f, Actor* a, int i, int n);
int fn_800B2454(Actor* a, int i);
int fn_800B1ED4(Actor* a, int i);
int fn_800B8D68(int a, int b, int c);
int fn_800B7020(int a);
int fn_800BEF9C(int a);
int fn_800BAF40(Actor* a);
void fn_800BC56C(Actor* a, int n);
void fn_800C725C(Actor* a, void* msg);
void fn_800C748C(void);
int fn_800AFD20(Actor* a);
int fn_80019F88(Msg* m, Actor* a);
void fn_8003F15C(float f, int model, int bone, int d, void* c, int e);
int fn_8003A1F0(int model, float* pose, int anim);
void fn_8018F880(float a, float b, float c, float d, int e, int f, int g, int h, int i, int j, int k, int l, int m);
void fn_8007E570(int a, int b, int c);
void fn_801ED0C8(void);
void fn_801ED2A0(int a);
void fn_801ED1E4(int a, int b, int c, int d);
void fn_801ED15C(int a);
void fn_801EB188(const char* s);
int fn_80049940(void);
int fn_80049750(int p);
int fn_80049948(void);
int fn_8020B97C(void* p);
int fn_8001F928(Actor* a, void* seg, u32 mask, int (*cb)(void), int n);
int fn_800A8CDC(float f, Actor* a, void* target);
void fn_800901AC(int h, int n);
void fn_800AA9FC(Actor* a, float* v);
void fn_8016D440(float* dst, float* src);
void fn_80230FA8(int p);
void fn_80230504(int p, Msg* m);
int fn_80230450(void);
}
extern int ModelAnimate(int model, char* anim, int n);
extern int OwnerId(Actor* a);
extern int ActorPlayAnim(int h, u32 hash, int arg);
extern int ActorAnimState(int h, u32 hash);
extern int ModelBoneName(int model, int bone);
extern void* FollowGetTargetObj(void* flags);
extern void FollowSetTargetObj(void* flags, void* t);
extern void RangeResetPending(void* flags);
extern int HasCondition(void* flags, int bit);
extern void ConditionSetBit(void* flags, int bit, int on);
extern void ConditionUpdateBit(void* flags, int bit, int on);
extern int MemFlagIsSet(void* flags, int bit);
extern void VFree(void* p);
extern void* VAlloc(int size);
extern int AssetFind(u32 a, u32 b);
extern void FollowOffsetUpdate(void* p);
extern int GrenadeSpawn(float f, int owner, Vec3* pos, Vec3* vel, int n);
extern int GrenadeTrue(void);
extern "C" int fn_80247EB0(void* p);

extern int sFoeBone;        // 0x804D3568
extern int sFoeHeadBone;    // 0x804D356C
extern float gFoeStunTimer; // 0x804D54B8
extern float gFoeStunInit;  // 0x804D9B18 (placeholder constant)
extern float gFoeA;         // 0x804D3570
extern float gFoeB;         // 0x804D3574
extern float gFoeC;         // 0x804D3578
extern float gFoeD;         // 0x804D357C
extern float gFoeE;         // 0x804D3580

int FoeSaveHit(Actor* a, Msg* m);
u32 FoeNextState(Actor* a);
int FoeQuery(Actor* a, Msg* m);
int FoeStunDraw(Actor* a, Msg* m);

extern unsigned char gCharMap[];
extern unsigned char gSig[];

// Hash of the stream signature string, via a character translation table.
inline u32 SigHash(const unsigned char* s) {
    u32 h = 0;
    const unsigned char* p = s;
    const unsigned char* q = s;
    for (; *q != 0; q++) {
        u32 c;
        if (*p == 0xFF) {
            c = 0xFFFFFFFF;
        } else {
            c = gCharMap[*p];
        }
        h = h * 31 + (c & 0xFF);
        p++;
    }
    return h % 0xFFFFFFFF;
}

// 0x80244728: actor message handler.
int FoeMsg(int msg, void* p) {
    Msg* m = (Msg*)p;
    Actor* a = m->actor;
    FoeBeh* b = a->beh;
    switch (msg) {
    case 0x41E:
        switch (m->key) {
        case 0x17A802E8:
            m->arg0C = b->targetId;
            return 1;
        case -0x527A8906: {
            String s1(gFoeStates[b->state].name);
            String s2("");
            return 0;
        }
        case -0x7CD20AF6:
            if (sFoeBone == -1) {
                sFoeBone = AssetFind(0xF1754BF2, 0x21C5E0);
            }
            m->arg0C = sFoeBone;
            return 1;
        case 0x63F94A69: {
            Target* t = (Target*)FollowGetTargetObj(a->flags);
            if (t == 0) {
                m->arg0C = -1;
            } else {
                m->arg0C = t->id;
            }
            return 1;
        }
        }
        return 0;
    case 0x407:
        return FoeSaveHit(a, m);
    case 0x400: {
        m->key = OwnerId(a);
        m->arg0C = 0.0f < a->health;
        m->arg10 = 1.0f;
        m->arg14 = 1.0f;
        m->arg18 = 2;
        m->arg1C = 0;
        return 1;
    }
    case 0:
        a->unk13C = 0.0f;
        {
            int model = fn_800AFD20(a);
            int bone = fn_8003A1F0(model, a->pose, ModelAnimate(a->model, 0, 0));
            void* color = &m->arg10;
            if (b->stunTimer < 1000.0f) {
                float t = (1000.0f - b->stunTimer) / 1000.0f;
                float lo = 0.0f;
                if (lo <= t) {
                    lo = t;
                    if (1.0f < t) {
                        lo = 1.0f;
                    }
                }
                color = (void*)0;
            }
            fn_8003F15C(1.0f, model, bone, a->unkD0, color, 0);
        }
        return 1;
    case 0x40A:
        return FoeQuery(a, m);
    case 0x409: {
        int owner = OwnerId(a);
        if (ActorPlayAnim(owner, m->arg0C, m->arg08) != 0) {
            return 1;
        }
        if (m->arg0C != 0x272CAC) {
            return 0;
        }
        if (gFoeStates[b->state].canLeave(a) != 0) {
            FollowSetTargetObj(a->flags, (void*)m->key);
            if (gFoeStates[5].enter(a) == 0) {
                gFoeStates[b->state].enter(a);
            }
        }
        return 1;
    }
    case 0xC14:
        return FoeStunDraw(a, m);
    case 0x1800:
        fn_800C725C(a, 0);
        ConditionUpdateBit(a->flags, 4, 1);
        b->targetId = m->key;
        return 1;
    case 0x3000:
        b->stunTimer = 1100.0f;
        return 0;
    case 0x7102: {
        u32 sig;
        if (b == 0) {
            return 1;
        }
        if (m->key < 0x60) {
            m->arg10 = 0;
            return 1;
        }
        sig = SigHash(gSig);
        memcpy((void*)m->arg08, &sig, 4);
        m->arg0C += 4;
        m->key -= 4;
        m->arg08 += 4;
        memcpy((void*)m->arg08, b, 0x5C);
        m->arg0C += 0x5C;
        m->key -= 0x5C;
        m->arg08 += 0x5C;
        return 1;
    }
    case 0x7103: {
        int ok;
        if (b == 0) {
            return 1;
        }
        if (m->arg0C < 0x60) {
            m->arg10 = 0;
            return 1;
        }
        if (m->arg0C < 4) {
            ok = 0;
        } else {
            u32* got = (u32*)m->arg08;
            m->arg0C += 4;
            m->key -= 4;
            m->arg08 += 4;
            if (*got == SigHash(gSig)) {
                ok = 1;
            } else {
                ok = 0;
            }
        }
        if (ok == 0) {
            return 1;
        }
        memcpy(b, (void*)m->arg08, 0x5C);
        m->arg0C -= 0x5C;
        m->arg08 += 0x5C;
        return 1;
    }
    }
    if (msg > 0x41C) {
        if (fn_80019F88(m, a) != 0) {
            return 1;
        }
        switch (m->key) {
        case 0x6B22F882:
        case -0x47E1635A:
            b->maxHp = (float)m->arg0C;
            b->hp = b->maxHp;
            return 1;
        case -0x182BA101:
            if (b->link != -1 && m->arg0C == 0) {
                String s;
                SendMsg(b->link, 0x41D, &s);
                b->link = -1;
            }
            break;
        case 0x6CDD4580:
            b->timerA = (float)m->arg0C;
            return 1;
        }
    }
    return 0;
}

// 0x80244F24: "QUERY" message: decides whether the actor wants to talk to the player.
int FoeQuery(Actor* a, Msg* m) {
    struct {
        Actor* from;
        int owner;
    } req;
    EventMsg e;
    u32 h;
    const unsigned char* name = (const unsigned char*)"QUERY";
    const unsigned char* p;
    const unsigned char* q;
    int r;
    req.from = m->actor;
    req.owner = OwnerId(0);
    (*(void (**)(int, void*))((char*)m->actor + 0x30))(0x400, &req);
    h = (u32)m->arg08;
    p = name;
    for (q = name; *q != 0; q++) {
        u32 c;
        if (*p == 0xFF) {
            c = 0xFFFFFFFF;
        } else {
            c = gCharMap[*p];
        }
        h = h * 31 + (c & 0xFF);
        p++;
    }
    r = ActorPlayAnim(OwnerId(a), h % 0xFFFFFFFF, m->key);
    if (r != 0) {
        return 1;
    }
    if (fn_80049940() != 0) {
        return 0;
    }
    if (MemFlagIsSet(a->flags, 0xB) != 0) {
        return 0;
    }
    if (HasCondition(a->flags, 10) != 0 || HasCondition(a->flags, 9) != 0) {
        return 0;
    }
    if (m->arg08 == 0x3CA34BD6 && MemFlagIsSet(a->flags, 0x20) == 0) {
        m->arg0C = 0x1379C466;
        return 1;
    }
    if (ActorAnimState(a->handle, 0xDC35206A) == 0) {
        return 0;
    }
    if (HasCondition(a->flags, 0xB) != 0) {
        return 0;
    }
    {
        int pl = fn_80049948();
        if (pl == 0) {
            return 0;
        }
        if (*(int*)(pl + 0x26E4) != 0 && (*(u32*)(*(int*)(pl + 0x26E4) + 0x38) >> 24) == 1) {
            return 0;
        }
        if (**(int**)(pl + fn_80049750(pl) * 4 + 0x26C4) == 0x4624485 ||
            **(int**)(pl + fn_80049750(pl) * 4 + 0x26C4) == 0x13C05 ||
            **(int**)(pl + fn_80049750(pl) * 4 + 0x26C4) == -0x1F5464B1 ||
            **(int**)(pl + fn_80049750(pl) * 4 + 0x26C4) == 0xFFF5) {
            return 0;
        }
    }
    if (MemFlagIsSet(a->flags, 0x1E) != 0) {
        return 0;
    }
    if (m->arg08 == 0x7C44EC3D && fn_800B26B8(a) != 4 && fn_800B22F8(a, 0) < 2) {
        return 0;
    }
    e.unk0 = 0;
    e.key = 0xFFFFFFFF;
    m->arg0C = 0x272CAC;
    e.target = m->arg0C;
    SendMsg(m->key, 0x40B, &e);
    return 1;
}

// 0x80245288: 0xC14 event (stun overlay and flag messages).
int FoeStunDraw(Actor* a, Msg* m) {
    FoeBeh* b = a->beh;
    float init = gFoeStunInit;
    int key = m->arg0C;
    if (key == 0x7603EBEB) {
        b->link = m->key;
        return 1;
    }
    if (key < 0x7603EBEB) {
        if (key == -0x1E767F1D) {
            b->unk30 = 1;
            return 1;
        }
        if (key < -0x1E767F1D && key == -0x4D8C0E6D) {
            float t = 1.0f;
            float r;
            int alpha;
            if (b->hp / b->maxHp < 1.0f) {
                t = b->hp / b->maxHp;
            }
            r = 0.0f;
            if (r < t) {
                r = t;
            }
            if (b->state == 4) {
                if ((b->flags & 1) == 0) {
                    alpha = 0;
                } else {
                    alpha = 0x80;
                }
                b->flags ^= 1;
            } else {
                alpha = 0x80;
            }
            fn_8018F880(1.0f, 1.0f, 1.0f, r, -3, 0x140, 0x1A6, 0x140, 7, 0xFF, alpha, 0, 0xFF);
            fn_801ED0C8();
            fn_801ED2A0(10);
            fn_8007E570(0xA0, 0x1AE, 1);
            fn_801ED1E4(0xFF, 0xFF, 0, 0xFF);
            fn_801ED15C(1);
            fn_801EB188("Stun");
            return 1;
        }
    } else if (key == 0x7BDA2B92) {
        a->flags4C &= ~8;
        *(float*)&gFoeStunTimer = init;
        return 1;
    }
    return 0;
}

// 0x8024543C: hit handler (damage, stagger and retargeting).
int FoeSaveHit(Actor* a, Msg* m) {
    FoeBeh* b = a->beh;
    if (a->unk8F8 == 6) {
        return 1;
    }
    if (m->arg14 == -0x5CB699B6) {
        return 1;
    }
    if (m->arg14 == 0x11A6B) {
        return 1;
    }
    if (((u32)m->arg08 >> 24) != 2) {
        return 1;
    }
    {
        int rnd = fn_800BA84C(6);
        *(float*)&m->arg0C = *(float*)&m->arg0C * 0.01f * (float)rnd;
    }
    a->unk214 = m->arg44;
    if (b->state == 4 || m->arg14 == -0x4603BD0B) {
        if (m->arg14 != -0x4603BD0B) {
            return 1;
        }
        if (MemFlagIsSet(a->flags, 10) != 0) {
            float lim = a->health - 1.0f;
            if (*(float*)&m->arg0C < lim) {
                lim = *(float*)&m->arg0C;
            }
            *(float*)&m->arg0C = lim;
        }
        a->health = -(*(float*)&m->arg0C * gFoeE - a->health);
        {
            int hp = (int)a->health;
            int max = a->info->maxHealth;
            if (hp < max) {
                max = hp;
            }
            a->health = (float)max;
        }
        b->hp = b->maxHp;
        ConditionSetBit(a->flags, 4, 1);
        ConditionSetBit(a->flags, 5, 0);
        ActorPlayAnim(a->handle, 0xA464660E, m->arg08);
        if (a->health <= 0.0f) {
            if (a->unk5F0 != 0 && fn_80230450() == 0) {
                fn_80230FA8(a->unk5F0);
            }
            if (a->unk8F8 != 4 && a->unk8F8 != 6 && HasCondition(a->flags, 9) == 0 &&
                HasCondition(a->flags, 10) == 0) {
                if (HasCondition(a->flags, 0xC) != 0) {
                    ConditionSetBit(a->flags, 0xC, 0);
                    fn_800901AC(a->unkD4, 0);
                }
                ConditionSetBit(a->flags, 9, 1);
                if (m->arg14 == 0x6CA5F172) {
                    ConditionSetBit(a->flags, 0xF, 1);
                }
            }
        }
        if (m->arg14 == 0x6CA5F172) {
            return 1;
        }
        fn_800B20B8(a, m->arg08);
        if (a->unk5F0 == 0) {
            return 1;
        }
        fn_80230504(a->unk5F0, m);
        return 1;
    }
    if (b->hp <= 0.0f || sFoeHeadBone == ModelBoneName(a->model, m->arg44)) {
        m->arg58 = 1;
        return 1;
    }
    if (HasCondition(a->flags, 0x3B) != 0 && b->link == -1) {
        int i = 0;
        int* bone = b->bones;
        do {
            if (*bone == ModelBoneName(a->model, m->arg44)) {
                break;
            }
            i++;
            bone++;
        } while (i < 10);
        if (i < 10) {
            b->hp = -(0.34f * b->maxHp - b->hp);
            a->health = -(*(float*)&m->arg0C * gFoeC - a->health);
            ConditionSetBit(a->flags, 4, 1);
            ActorPlayAnim(a->handle, 0x47DCD34, m->arg08);
        } else {
            a->health = -(*(float*)&m->arg0C * gFoeB - a->health);
            ActorPlayAnim(a->handle, 0x47DCD34, m->arg08);
        }
        if (b->hp > 0.0f) {
            return 1;
        }
        b->stunTimer = 0.0f;
        ActorPlayAnim(a->handle, 0x2288A7B4, m->arg08);
        return 1;
    }
    if (b->link == -1 && b->unk30 == 1) {
        b->unk20 = b->unk20 + gFoeD;
    }
    {
        int i = 0;
        int* bone = b->bones;
        do {
            if (*bone == ModelBoneName(a->model, m->arg44)) {
                break;
            }
            i++;
            bone++;
        } while (i < 10);
        if (i < 10) {
            a->health = -(*(float*)&m->arg0C * gFoeC - a->health);
            ConditionSetBit(a->flags, 4, 1);
            ActorPlayAnim(a->handle, 0x47DCD34, m->arg08);
        } else {
            a->health = -(*(float*)&m->arg0C * gFoeB - a->health);
            ActorPlayAnim(a->handle, 0x47DCD34, m->arg08);
        }
    }
    if (b->hp <= 0.0f) {
        ActorPlayAnim(a->handle, 0x2288A7B4, m->arg08);
        return 1;
    }
    {
        void* target = FollowGetTargetObj(a->flags);
        int found;
        char msg[0x20];
        if (a->unk220 != 0) {
            return 1;
        }
        if (1.0f - gFoeA <= b->unk20) {
            return 1;
        }
        if (b->link != -1) {
            return 1;
        }
        memset(msg, 0, 0x20);
        found = -1;
        if (b->unk18 != -1) {
            if (b->targetId == -1) {
                found = fn_800B8D68(b->unk18, 0, 0);
            } else if ((float)rand() / 32767.0f < 0.33f) {
                found = fn_800B8D68(b->unk18, 0, 0);
            }
        }
        *(int*)(msg + 0x8) = found;
        *(int*)(msg + 0x14) = (0.6f * (float)a->info->maxHealth <= a->health) ? 0x2889C9 : 0x13E6B;
        fn_800C725C(a, msg);
        FollowSetTargetObj(a->flags, target);
        b->targetId = found;
        if ((float)rand() / 32767.0f < 0.33f && fn_8008EA9C(a->unkD4, 1) == 0) {
            RangeResetPending(a->flags);
        }
    }
    return 1;
}

// 0x80245AFC: per-frame update.
int FoeUpdate(Actor* a) {
    FoeBeh* b;
    int ok;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (b->stunTimer < 1000.0f) {
        b->stunTimer = b->stunTimer + a->dt;
    }
    if (HasCondition(a->flags, 9) == 0 && HasCondition(a->flags, 10) == 0 && HasCondition(a->flags, 0xB) == 0) {
        if (0.0f != gFoeStunTimer) {
            gFoeStunTimer = gFoeStunTimer - a->dt;
            if (gFoeStunTimer < 0.0f) {
                gFoeStunTimer = 0.0f;
                a->flags4C |= 8;
            }
        }
        if (fn_800BEF9C(a->unk2C4) < a->unk268) {
            a->unk268 = fn_800BEF9C(a->unk2C4);
        }
        ok = gFoeStates[b->state].update(a);
        if (fn_800BAF40(a) == -1) {
            a->unk288 = 0;
            a->unk26C = 0;
        }
        fn_80162658(a, a->sm);
        if (ok != 0) {
            int next = FoeNextState(a);
            if (next != b->state && gFoeStates[b->state].canLeave(a) != 0 && gFoeStates[next].enter(a) == 0) {
                gFoeStates[b->state].enter(a);
            }
        }
        return 1;
    }
    fn_80162658(a, a->sm);
    return 1;
}

// 0x80245CE4: destroy.
void FoeDestroy(Actor* a) {
    a->update = 0;
    VFree(a->beh);
    a->beh = 0;
}

// 0x80245D24: create.
void FoeCreate(Actor* a) {
    FoeBeh* b;
    a->update = FoeUpdate;
    b = (FoeBeh*)VAlloc(0x5C);
    if (b == 0) {
        a->beh = 0;
    } else {
        a->beh = b;
        a->unkFC = (void*)fn_800CE96C;
        a->unk604 = (void*)fn_800CF064;
        a->unk5FC = (void*)fn_800CC984;
        a->unk600 = (void*)fn_80247EB0;
        b = a->beh;
        b->state = 0;
        b->maxHp = 8000.0f;
        b->hp = b->maxHp;
        b->flags = 0;
        b->timerA = 100.0f;
        b->timerB = 12000.0f;
        b->targetId = -1;
        b->unk18 = -1;
        b->unk20 = 1.0f;
        b->stunTimer = 1100.0f;
        b->link = -1;
        b->fire = -1;
        b->unk30 = 1;
        b->bones[0] = AssetFind(0xF1754BF2, 0x922BF04E);
        b->bones[1] = AssetFind(0xF1754BF2, 0x922BF04F);
        b->bones[2] = AssetFind(0xF1754BF2, 0x21C5E0);
        b->bones[3] = AssetFind(0xF1754BF2, 0x24805F);
        b->bones[4] = AssetFind(0xF1754BF2, 0x11520);
        b->bones[5] = AssetFind(0xF1754BF2, 0xB87B120F);
        b->bones[6] = AssetFind(0xF1754BF2, 0xD5FCFDC3);
        b->bones[7] = AssetFind(0xF1754BF2, 0x101844CC);
        b->bones[8] = AssetFind(0xF1754BF2, 0x48874F6E);
        b->bones[9] = AssetFind(0xF1754BF2, 0x4A53FC1);
        sFoeHeadBone = AssetFind(0xF1754BF2, 0x645E8EAC);
        ConditionUpdateBit(a->flags, 6, 1);
        ConditionUpdateBit(a->flags, 0x1C, 1);
        gFoeStates[b->state].enter(a);
        sFoeBone = AssetFind(0xF1754BF2, 0x21C5E0);
    }
}

// 0x80245F6C: enter-state check for the stun state (is the player allowed to interrupt?).
u32 FoeUpdateStunCheck(Actor* a) {
    FoeBeh* b;
    Target* t;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    t = (Target*)FollowGetTargetObj(a->flags);
    if (t != 0 && (t->id >> 24) == 2) {
        if ((b->flags & 2) == 0) {
            int r;
            b->flags |= 2;
            r = ActorPlayAnim(a->handle, 0xDC35206A, t->id);
            if (t->id != (u32)-1) {
                EventMsg e;
                e.key = 0xAF699ED0;
                e.unk0 = r;
                e.value = a->handle;
                SendMsg(t->id, 0xC14, &e);
            }
        }
        return fn_8020B97C(t) == 0;
    }
    {
        int h = a->handle;
        int pl = fn_80049948();
        if (*(int*)(pl + 0x38) != -1) {
            EventMsg e;
            e.key = 0xAF699ED0;
            e.unk0 = 0;
            e.value = h;
            SendMsg(*(int*)(pl + 0x38), 0xC14, &e);
        }
        b->flags |= 2;
        return 1;
    }
}

// 0x8024608C: leave-state check for the stun state.
int FoeLeaveStun(Actor* a) {
    u32 f;
    if (a == 0) {
        return 0;
    }
    f = a->beh->flags;
    if ((f & 2) == 0) {
        return 0;
    }
    a->beh->flags = f & ~2;
    RangeResetPending(a->flags);
    if (MemFlagIsSet(a->flags, 0x1F) == 0) {
        ConditionUpdateBit(a->flags, 0x16, 0);
    }
    return 1;
}

// 0x80246110: enter the stun state.
int FoeEnterStun(Actor* a) {
    FoeBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (FollowGetTargetObj(a->flags) == 0) {
        return 0;
    }
    a->unk288 = 0;
    b->state = 5;
    b->flags &= ~2;
    if (MemFlagIsSet(a->flags, 0x1F) == 0) {
        ConditionUpdateBit(a->flags, 4, 1);
    }
    return 1;
}

// 0x802461AC: update for the attack state (retarget, aim, recharge).
int FoeUpdateAttack(Actor* a) {
    FoeBeh* b = a->beh;
    int ret;
    int target = 0;
    int idx;
    if (MemFlagIsSet(a->flags, 5) != 0) {
        return 1;
    }
    idx = fn_800B26B8(a);
    if (idx == 4) {
        RangeResetPending(a->flags);
    } else {
        if (fn_800B1EF4(a, idx) == 0) {
            fn_800B261C(a, idx, &target);
            fn_800B22F8(a, idx);
            if (target == 0 || fn_800B22F8(a, idx) > 1) {
                RangeResetPending(a->flags);
            } else {
                fn_800B2418(1000000.0f, a, idx);
                fn_800B1F20(a, idx);
                fn_800B1AB8(-1.0f, a, idx, 1);
            }
        } else if (fn_800B2454(a, idx) == 0) {
            fn_800B2418(1000000.0f, a, idx);
        }
        fn_800B261C(a, idx, &target);
        if (target != (int)FollowGetTargetObj(a->flags)) {
            FollowSetTargetObj(a->flags, (void*)target);
        }
    }
    if (target != 0 && ((*(u32*)(target + 0x38)) >> 24) == 2 && fn_80049750(0) == 0) {
        RangeResetPending(a->flags);
    }
    target = (int)FollowGetTargetObj(a->flags);
    if (target == 0) {
        ConditionUpdateBit(a->flags, 4, 0);
    } else {
        Vec3 v;
        float d;
        v.x = (float)*(int*)(target + 0x24);
        v.y = (float)*(int*)(target + 0x28);
        v.z = (float)*(int*)(target + 0x2C);
        fn_800AA9FC(a, &v.x);
        d = 0.0f;
        if (MemFlagIsSet(a->flags, 4) == 0) {
            static float sThresh = 45.0f;
            if (d > sThresh) {
                ConditionUpdateBit(a->flags, 4, 1);
            }
        } else if (d < 5.0f) {
            ConditionUpdateBit(a->flags, 4, 0);
        }
    }
    b->hp = 0.001f * 1000.0f * a->dt + b->hp;
    if (b->hp < b->maxHp) {
        ret = 0;
    } else {
        b->hp = b->maxHp;
        ret = 1;
    }
    return ret;
}

// 0x80246494: leave the stun state (reset timers by remaining health).
int FoeLeaveAttack(Actor* a) {
    FoeBeh* b = a->beh;
    ConditionSetBit(a->flags, 5, 0);
    ConditionUpdateBit(a->flags, 4, 1);
    b->unk20 = 0.0f;
    b->unk30 = 0;
    if (a->health <= 0.667f * (float)a->info->maxHealth) {
        if (a->health <= 0.333f * (float)a->info->maxHealth) {
            b->timerA = 250.0f;
            b->timerB = 12000.0f;
        } else {
            b->timerA = 800.0f;
            b->timerB = 8000.0f;
        }
    } else {
        b->timerA = 1600.0f;
        b->timerB = 9000.0f;
    }
    return 1;
}

// 0x8024658C: enter the fire state (spawns the grenade/projectile).
int FoeEnterFire(Actor* a) {
    FoeBeh* b = a->beh;
    b->state = 4;
    fn_800BBCA8(a);
    a->unk288 = 0;
    fn_80162610(a, a->sm, 0);
    ConditionSetBit(a->flags, 5, 1);
    if (b->link == -1) {
        float speed;
        Vec3 dir;
        Vec3 pos;
        u32 h = 0;
        const unsigned char* name = (const unsigned char*)"P_FIRE2";
        const unsigned char* p = name;
        const unsigned char* q;
        float tmp[3];
        struct {
            int unk0;
            u32 key;
            char pad[0x34 - 8];
            float pos[3];
        } query;
        if (a->health <= 0.667f * (float)a->info->maxHealth) {
            if (a->health <= 0.333f * (float)a->info->maxHealth) {
                speed = 3333.0f;
            } else {
                speed = 1666.0f;
            }
        } else {
            speed = 555.0f;
        }
        for (q = name; *q != 0; q++) {
            u32 c;
            if (*p == 0xFF) {
                c = 0xFFFFFFFF;
            } else {
                c = gCharMap[*p];
            }
            h = h * 31 + (c & 0xFF);
            p++;
        }
        query.key = AssetFind(0xF1754BF2, h % 0xFFFFFFFF);
        SendMsg(a->handle, 0x411, &query);
        tmp[0] = query.pos[0] - a->p84[0];
        tmp[1] = query.pos[1] - a->p84[1];
        tmp[2] = query.pos[2] - a->p84[2];
        fn_8016D440(&dir.x, tmp);
        dir.x = -20.0f;
        if (b->fire != -1) {
            int pl = fn_80049948();
            if (*(int*)(pl + 0x26E4) == 0 || *(int*)(*(int*)(pl + 0x26E4) + 0x38) != b->fire) {
                String s;
                SendMsg(b->fire, 0x41D, &s);
                b->fire = -1;
            }
        }
        pos.x = query.pos[0];
        pos.y = query.pos[1];
        pos.z = query.pos[2];
        b->fire = GrenadeSpawn(speed, a->handle, &pos, &dir, 0);
        ActorPlayAnim(a->handle, 0x1F52224C, b->fire);
    }
    return 1;
}

// 0x80246830: leave check for the fire state.
int FoeLeaveFire(Actor* a) {
    if (MemFlagIsSet(a->flags, 5) != 0) {
        return 1;
    }
    return HasCondition(a->flags, 0x30) == 0;
}

// 0x80246888: states with nothing to check.
int FoeLeaveOk(void) { return 1; }

// 0x80246890: enter the guard state.
int FoeEnterGuard(Actor* a) {
    a->beh->state = 3;
    fn_800BBCA8(a);
    a->unk288 = 0;
    fn_80162610(a, a->sm, 0);
    return 1;
}

// Pick a target to retaliate against; shared by the chase update's retarget branches.
inline int FoeFindTarget(FoeBeh* b) {
    int found = -1;
    if (b->unk18 != -1) {
        if (b->targetId == -1) {
            found = fn_800B8D68(b->unk18, 0, 0);
        } else if ((float)rand() / 32767.0f < 0.33f) {
            found = fn_800B8D68(b->unk18, 0, 0);
        }
        if (found == -1 || found == b->targetId) {
            struct {
                int unk0;
                int id;
            } q;
            SendMsg(b->targetId, 0x41A, &q);
            if (q.id == -1 || (found = q.id, b->unk18 != fn_800B7020(q.id))) {
                found = fn_800B8D68(b->unk18, 0, 0);
            }
        }
    }
    return found;
}

// 0x802468E4: update for the chase state (target tracking, shooting decisions).
int FoeUpdateChase(Actor* a) {
    FoeBeh* b;
    int idx;
    int target;
    int retarget = 0;
    int canShoot = 1;
    float distSq;
    int wantBurst = 0;
    int wantMelee = 0;
    Target* tgt;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (MemFlagIsSet(a->flags, 5) != 0) {
        return 1;
    }
    if (b->hp <= 0.0f) {
        return 1;
    }
    idx = fn_800B26B8(a);
    if (a->unk21C == 0 && FollowGetTargetObj(a->flags) == 0) {
        fn_800B261C(a, idx, &target);
        if (target != 0) {
            FollowSetTargetObj(a->flags, (void*)target);
        }
    }
    if (idx == 4) {
        RangeResetPending(a->flags);
    } else {
        if (fn_800B1EF4(a, idx) == 0) {
            fn_800B261C(a, idx, &target);
            fn_800B22F8(a, idx);
            fn_800B2418(1000000.0f, a, idx);
            fn_800B1F20(a, idx);
            fn_800B1AB8(-1.0f, a, idx, 1);
        } else if (fn_800B2454(a, idx) == 0) {
            fn_800B2418(1000000.0f, a, idx);
        }
        fn_800B261C(a, idx, &target);
        if (target != (int)FollowGetTargetObj(a->flags) && a->unk21C == 0) {
            FollowSetTargetObj(a->flags, (void*)target);
        }
    }
    target = (int)FollowGetTargetObj(a->flags);
    if (b->link != -1) {
        int linked = 0;
        SendMsg(b->link, 0x400, &linked);
        if (linked == 0) {
            b->link = -1;
        } else if (a->unk220 == 0) {
            int next = FoeFindTarget(b);
            Target* lt;
            SendMsg(next, 0x400, &lt);
            if (lt != 0) {
                float dx = (float)lt->pos[0] - a->p84[0];
                float dy = (float)lt->pos[1] - a->p84[1];
                if (dx * dx + dy * dy > 0.0f) {
                    retarget = 1;
                    FollowSetTargetObj(a->flags, (void*)target);
                    a->unk288 = a->unk21C;
                    b->targetId = next;
                    fn_80162610(a, a->sm, 0);
                    b->link = -1;
                }
            }
        }
    }
    if (target != 0) {
        Vec3 seg[2];
        seg[0].x = (float)a->pos[0];
        seg[0].y = (float)a->pos[1];
        seg[0].z = (float)a->pos[2];
        seg[1].x = (float)*(int*)(target + 0x24);
        seg[1].y = (float)*(int*)(target + 0x28);
        seg[1].z = (float)*(int*)(target + 0x2C);
        if (fn_8001F928(a, seg, 0x9000A200, (int (*)(void))GrenadeTrue, 0) != 0) {
            canShoot = 0;
        }
    }
    tgt = 0;
    if (target == 0 || SendMsg(*(int*)(target + 0x38), 0x400, &tgt) == 0 || tgt == 0 ||
        (((u32*)tgt)[0x10 / 4 + 0] & 2) == 0) {
        u32 h = 0;
        const unsigned char* name = (const unsigned char*)"onLostTarget";
        const unsigned char* p = name;
        const unsigned char* q;
        ConditionUpdateBit(a->flags, 0x15, 0);
        RangeResetPending(a->flags);
        for (q = name; *q != 0; q++) {
            u32 c;
            if (*p == 0xFF) {
                c = 0xFFFFFFFF;
            } else {
                c = gCharMap[*p];
            }
            h = h * 31 + (c & 0xFF);
            p++;
        }
        ActorPlayAnim(a->handle, h % 0xFFFFFFFF, a->handle);
    } else {
        float dx = (float)*(int*)(target + 0x24) - a->p84[0];
        float dy = (float)*(int*)(target + 0x28) - a->p84[1];
        float dz = (float)*(int*)(target + 0x2C) - a->p84[2];
        b->timerA = b->timerA - a->dt;
        b->timerB = b->timerB - a->dt;
        distSq = dz * dz + dx * dx + dy * dy;
        if (HasCondition(a->flags, 0x10) == 0) {
            if (fn_80162940(a->sm, 2) != 0 && fn_80162940(a->sm, 8) != 0 && a->unk288 == 0 && canShoot != 0 &&
                fn_8008EA9C(a->unkD4, 1) == 0) {
                wantBurst = 1;
            }
            if (fn_80162940(a->sm, 1) != 0 && fn_80162940(a->sm, 4) != 0 && b->unk20 < 1.0f - gFoeA &&
                fn_8008EA9C(a->unkD4, 1) == 0) {
                wantMelee = 1;
            }
        }
        if (distSq < 409600.0f && a->unk21C == 0) {
            fn_800901AC(a->unkD4, 1);
            ConditionSetBit(a->flags, 0x30, 1);
            return 1;
        }
        if (a->health <= 0.667f * (float)a->info->maxHealth) {
            if (a->health <= 0.333f * (float)a->info->maxHealth) {
                if (fn_800A8CDC(0.8f, a, FollowGetTargetObj(a->flags)) == 0) {
                    if (wantBurst) {
                        fn_80162610(a, a->sm, 2);
                        b->timerA = 500.0f * ((float)rand() / 32767.0f) + 1000.0f;
                    } else {
                        canShoot = 0;
                    }
                } else if (b->timerA <= 0.0f) {
                    if (wantBurst && (float)rand() / 32767.0f < gFoeA + b->unk20) {
                        fn_80162610(a, a->sm, 2);
                        b->timerA = 500.0f * ((float)rand() / 32767.0f) + 1000.0f;
                        b->unk20 = 0.0f;
                    } else if (wantMelee) {
                        ConditionSetBit(a->flags, 0x2F, 1);
                        fn_80162610(a, a->sm, 1);
                        b->timerA = 350.0f * ((float)rand() / 32767.0f) + 250.0f;
                    }
                }
            } else if (fn_800A8CDC(0.8f, a, FollowGetTargetObj(a->flags)) == 0) {
                canShoot = 0;
            } else if (b->timerA <= 0.0f) {
                if (wantBurst && (float)rand() / 32767.0f < gFoeA + b->unk20) {
                    fn_80162610(a, a->sm, 2);
                    b->timerA = 1500.0f * ((float)rand() / 32767.0f) + 1500.0f;
                    b->unk20 = 0.0f;
                } else if (wantMelee) {
                    ConditionSetBit(a->flags, 0x2F, 1);
                    fn_80162610(a, a->sm, 1);
                    b->timerA = 800.0f * ((float)rand() / 32767.0f) + 800.0f;
                }
            }
        } else if (fn_800A8CDC(0.8f, a, FollowGetTargetObj(a->flags)) == 0) {
            canShoot = 0;
        } else if (b->timerA <= 0.0f) {
            if (wantBurst && b->timerA < 0.0f && (float)rand() / 32767.0f < gFoeA + b->unk20) {
                fn_80162610(a, a->sm, 2);
                b->timerA = 1000.0f * ((float)rand() / 32767.0f) + 3000.0f;
                b->unk20 = 0.0f;
            } else if (wantMelee) {
                ConditionSetBit(a->flags, 0x2F, 1);
                fn_80162610(a, a->sm, 1);
                b->timerA = 1400.0f * ((float)rand() / 32767.0f) + 1600.0f;
            }
        }
    }
    if ((fn_8016325C(a->sm) == 5 && retarget == 0) || HasCondition(a->flags, 0x10) != 0) {
        return 0;
    }
    if (a->unk220 == 2) {
        a->unk10C = (void*)fn_800C748C;
        if ((int)FollowGetTargetObj(a->flags) != a->unk22C || a->unk288 != a->unk21C) {
            if (a->unk21C != 0) {
                a->unk288 = a->unk21C;
                fn_800BC56C(a, a->unk22C);
            }
        }
        if (fn_800BAF40(a) == -1 && a->unk21C != 0) {
            a->unk288 = 0;
        }
    } else if (a->unk220 == 0) {
        a->unk21C = 0;
        a->unk288 = 0;
        if (canShoot == 0 && b->link == -1) {
            struct {
                char pad[8];
                int target;
                int unk;
                int flags;
            } msg;
            memset(&msg, 0, 0x20);
            msg.target = FoeFindTarget(b);
            msg.flags = (0.6f * (float)a->info->maxHealth <= a->health) ? 0x2889C9 : 0x13E6B;
            fn_800C725C(a, &msg);
            FollowSetTargetObj(a->flags, (void*)target);
            a->unk288 = a->unk21C;
            fn_800BC56C(a, a->unk22C);
            b->targetId = msg.target;
            if ((float)rand() / 32767.0f < 0.33f) {
                RangeResetPending(a->flags);
            }
        }
    }
    return 0;
}

// 0x802478E4: enter the idle-look state.
int FoeEnterLook(Actor* a) {
    a->unk10C = (void*)FollowOffsetUpdate;
    fn_80162610(a, a->sm, 0);
    return 1;
}

// 0x8024791C: enter state 2.
int FoeEnterState2(Actor* a) {
    a->beh->state = 2;
    ConditionUpdateBit(a->flags, 4, 1);
    ConditionUpdateBit(a->flags, 3, 1);
    return 1;
}

// 0x80247974: update for state 1.
int FoeUpdateState1(Actor* a) {
    int idx;
    if (a == 0) {
        return 0;
    }
    idx = fn_800B26B8(a);
    if (idx != 4) {
        float tmp[4];
        fn_800B261C(a, idx, (int*)tmp);
        if (fn_800B1EF4(a, idx) == 0) {
            if (fn_800B22F8(a, idx) < 3) {
                return 1;
            }
            fn_800B1F20(a, idx);
        }
    }
    if (a->unk220 == 0 && MemFlagIsSet(a->flags, 5) == 0) {
        return 1;
    }
    if (a->unk220 == 2) {
        a->unk10C = (void*)fn_800C748C;
        if ((int)FollowGetTargetObj(a->flags) != a->unk22C || a->unk288 != a->unk21C) {
            FollowSetTargetObj(a->flags, 0);
            if (a->unk21C != 0) {
                a->unk288 = a->unk21C;
                fn_800BC56C(a, a->unk22C);
            }
        }
        if (fn_800BAF40(a) == -1 && a->unk21C != 0) {
            FollowSetTargetObj(a->flags, 0);
            a->unk288 = 0;
        }
    }
    return 0;
}

// 0x80247AC8: enter state 1.
int FoeEnterState1(Actor* a) {
    a->unk10C = (void*)FollowOffsetUpdate;
    if (a->unk220 == 4) {
        a->unk220 = 0;
    }
    if (HasCondition(a->flags, 0xD) != 0) {
        ConditionSetBit(a->flags, 0xD, 0);
    }
    ConditionUpdateBit(a->flags, 0x16, 0);
    return 1;
}

// 0x80247B48: enter the plain state 1 (no side effects).
int FoeEnterPlain(Actor* a) {
    a->beh->state = 1;
    return 1;
}

// 0x80247B5C: update for state 0.
int FoeUpdateState0(Actor* a) {
    int idx;
    if (a == 0) {
        return 0;
    }
    if (0.0f < a->beh->hp) {
        if (a->unk220 == 0) {
            idx = fn_800B26B8(a);
            if (idx == 4) {
                ConditionUpdateBit(a->flags, 4, 0);
                RangeResetPending(a->flags);
            } else if (fn_800B1EF4(a, idx) == 0) {
                return 1;
            }
            fn_800BBCA8(a);
            return 0;
        }
        return 1;
    }
    return 1;
}

// 0x80247C10: leave check that always succeeds.
int FoeLeaveAlways(void) { return 1; }

// 0x80247C18: enter state 0.
int FoeEnterState0(Actor* a) {
    FoeBeh* b;
    int idx;
    int target;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    a->unk288 = 0;
    idx = fn_800B26B8(a);
    if (idx == 4) {
        RangeResetPending(a->flags);
    } else {
        char tmp[0x18];
        fn_800B261C(a, idx, &target);
        if (target == 0 || SendMsg(*(int*)(target + 0x38), 0x400, tmp) == 0 || (*(u32*)(tmp + 0x1C) & 2) == 0) {
            RangeResetPending(a->flags);
        } else {
            FollowSetTargetObj(a->flags, (void*)target);
        }
    }
    ConditionUpdateBit(a->flags, 3, 0);
    b->state = 0;
    return 1;
}

// 0x80247CF0: choose the next state.
u32 FoeNextState(Actor* a) {
    FoeBeh* b;
    u32 cur;
    int idx;
    int cnt = 0;
    int target = 0;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    cur = b->state;
    idx = fn_800B26B8(a);
    if (idx != 4) {
        cnt = fn_800B22F8(a, idx);
        fn_800B1ED4(a, idx);
    }
    if (MemFlagIsSet(a->flags, 5) == 0 || (idx != 4 && a->unk220 == 0)) {
        if (0.0f < b->hp) {
            if (HasCondition(a->flags, 0x30) == 0) {
                if (MemFlagIsSet(a->flags, 0x15) == 0 || a->unk220 == 0) {
                    if (idx != 4) {
                        fn_800B261C(a, idx, &target);
                        fn_800B1ED4(a, idx);
                        if (target == 0) {
                            return 0;
                        }
                        if (cnt < 2) {
                            return 2;
                        }
                        if (cnt == 3 && fn_800B1EF4(a, idx) == 0) {
                            u32 now;
                            fn_800B1F20(a, idx);
                            now = b->state;
                            return now & (int)((cur - now) | (now - cur)) >> 31;
                        }
                        fn_800B1F20(a, idx);
                    }
                    {
                        u32 now = b->state;
                        if (cur == now) {
                            return ((0 - a->unk220) | a->unk220) >> 31;
                        }
                        return now;
                    }
                }
                return 2;
            }
            return 3;
        }
        return 4;
    }
    return 1;
}

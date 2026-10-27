// u_80248FC8.cpp: possessed-actor state machine "Cord" (0x80248FC8-0x8024CAA0).
// A 34-state behaviour (idle, walk, run, jump, grenade, melee, hurt ...) driven from a state table
// of {enter, canLeave, update, handler, name, hash} records.  The actor message handler routes
// hit/animation messages to the current state handler, and the update callback runs the state's
// update function and switches states when it asks for another one.  Names are descriptive
// guesses; unmatched against retail so far.

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

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

extern "C" {
void* memcpy(void* dst, const void* src, unsigned long n);
int rand(void);
int SendMsg(int to, int msg, void* data);
}

struct CordBeh;
struct Actor;
void CordAdvance(Actor* a);
void CordPlayAnim(Actor* a, int variant);


struct ActorInfo {
    char pad0[8];
    float unk08;            // 0x08
    char pad0C[0x14 - 0x0C];
    int unk14;              // 0x14
    char pad18[4];
    int unk1C;              // 0x1C
    char pad20[0x30 - 0x20];
    int maxHealth;          // 0x30
    char pad34[0xC0 - 0x34];
    float unkC0;            // 0xC0
};

struct Pose {
    char pad0[0x10];
    float x, y, z;          // 0x10
};

// Actor (only the fields used here).
struct Actor {
    char pad0[0x24];
    int pos[3];             // 0x24
    char pad30[8];          // 0x30
    int handle;             // 0x38
    char pad3C[0x84 - 0x3C];
    float p84[3];           // 0x84
    char pad90[0xC4 - 0x90];
    ActorInfo* info;        // 0xC4
    char padC8[4];          // 0xC8
    int model;              // 0xCC
    int unkD0;              // 0xD0
    int unkD4;              // 0xD4
    char padD8[0xF0 - 0xD8];
    Pose* pose;             // 0xF0
    float unkF4;            // 0xF4
    float unkF8;            // 0xF8
    char padFC[0x10C - 0xFC];
    void* unk10C;           // 0x10C
    int (*update)(Actor*);  // 0x110
    char pad114[4];         // 0x114
    void* unk118;           // 0x118
    char pad11C[0x134 - 0x11C];
    CordBeh* beh;           // 0x134
    float dt;               // 0x138
    char pad13C[0x220 - 0x13C];
    int unk220;             // 0x220
    char pad224[0x240 - 0x224];
    float vel[3];           // 0x240
    float rot[3];           // 0x24C
    float heading;          // 0x258
    float turnRate;         // 0x25C
    char pad260[0x26C - 0x260];
    int unk26C;             // 0x26C
    char flags[0x27C - 0x270]; // 0x270 condition bits
    float unk27C;           // 0x27C
    char pad280[4];
    float speed;            // 0x284
    int unk288;             // 0x288
    char pad28C[0x2B0 - 0x28C];
    float targetHeading;    // 0x2B0
    char pad2B4[0x5F0 - 0x2B4];
    int unk5F0;             // 0x5F0
    char pad5F4[0x894 - 0x5F4];
    char sm[0x904 - 0x894]; // 0x894 sub state machine
    float unk904;           // 0x904
};

typedef void (*EnterFn)(Actor*);
typedef int (*LeaveFn)(Actor*, int);
typedef int (*UpdateFn)(Actor*);
typedef int (*HandlerFn)(int, Actor*);

struct StateDef {
    EnterFn enter;          // 0x00
    LeaveFn canLeave;       // 0x04
    UpdateFn update;        // 0x08
    HandlerFn handler;      // 0x0C
    const char* name;       // 0x10
    u32 hash;               // 0x14
};

extern StateDef gCordStates[34];

// Behaviour block (VAlloc'd, 0x38 bytes).
struct CordBeh {
    int state;              // 0x00
    int next;               // 0x04
    int prevInput;          // 0x08
    int anim;               // 0x0C
    int unk10;              // 0x10
    char pad14[4];
    float unk18;            // 0x18
    int fx;                 // 0x1C
    int sound;              // 0x20
    int fxId;               // 0x24
    int unk28;              // 0x28
    float vel[3];           // 0x2C
};

// Message block seen by the actor message handler (0x14+ bytes).
struct ActorMsg {
    Actor* actor;           // 0x00
    int arg;                // 0x04
    char* data;             // 0x08
    int size;               // 0x0C
    int unk10;              // 0x10
};

// Externals.
extern "C" {
int fn_800A1BC8(int a, int b, u32 hash, int c, int d);
int fn_8008E7D0(void);
void fn_8008F4F0(float f, int h, int n);
void fn_80090974(float f, int h, int n, int v, int m, int (*cb)(int, Actor*), Actor* a);
void fn_8008DE4C(int a, int b, int c, int d);
}
extern void RangeResetFields(void* p);
struct Mtx34 {
    float m[12];
};
extern void ActorGetMatrix(Mtx34* m, Actor* a);
extern void MtxSetEuler(Mtx34* m, Vec3* e);
extern void MtxGetEuler(Vec3* e, Mtx34* m);
extern void PlacementToMatrix(Actor* a, Mtx34* m);
extern void ModelSetMatrix(int model, Mtx34* m, Pose* pose);
extern void BoneGetMatrix(Mtx34* m, Pose* p);
extern void EulerNormalize(Vec3* dst, Vec3* src);
extern void Vec3Set(Vec3* v, float x, float y, float z);
extern void BoneAddRotX(Pose* p, int bone, float f);
extern void SplineQuatRotateRamp(float f, float* v, int a, int b, u32 id);
extern void AnimApply(int h, Pose* p, int anim, int d);
extern "C" {
void fn_800913EC(float dt, int h, float* out);
float fn_800CC2D0(Actor* a, int n, int* p, int* q);
int fn_800CADE0(Actor* a);
void fn_800CAD0C(Actor* a, int v);
void fn_800A0168(Mtx34* dst, Mtx34* src);
void fn_800A0030(float* dst, Mtx34* m, float* src);
void fn_80090390(int h, int n);
void fn_8008F4E8(int h, int n);
void fn_8008F378(int h);
void fn_8008F4EC(int h, int n);
void fn_8008F230(float f, int h, int a, int b);
}
extern float gCordRise;     // 0x804D3590
extern float gCordRiseDiv;  // 0x804D3594
extern float gCordFall;     // 0x804D3598
extern float gCordFallDiv;  // 0x804D359C
extern float sCordRiseRate; // 0x804D54C8
extern float sCordFallRate; // 0x804D54CC
extern u32 gCordAnimHash[]; // 0x803BE9E0
extern void ActorPlayAnim(int h, u32 hash, int h2);
extern int TextureLoad(short* id, int n);
extern int HasCondition(void* flags, int bit);
extern void ConditionSetBit(void* flags, int bit, int on);
extern int MemFlagIsSet(void* flags, int bit);
extern void MemCopyVec3(float* dst, float* src);
extern void VFree(void* p);
extern void* VAlloc(int size);
extern int AssetFind(u32 a, u32 b);
extern int FxFind(const char* name);
extern int ModelFindBone(int model, int id);
extern int ModelAnimate(int model, char* anim, int n);
extern int ZoneClassifyDirection(void* actor, float* v);
extern "C" {
int fn_8016325C(void* sm);
int fn_80162610(Actor* a, void* sm, u32 msg);
void fn_800CB8BC(Actor* a, int p1, int p2, int p3);
void fn_8015A10C(int fx);
void fn_80027690(int snd);
void fn_8015A4AC(float a, float b, int fx, int pos, int c, int d, int e);
void fn_800277B0(int snd, int pos, void* v, void* flags);
int fn_80027A58(int a, int pos, void* v, int flags, int c);
void fn_8016F120(int fx, void* pos);
int fn_8015A570(float a, float b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l);
int fn_8003A1F0(int model, int boneSet, int anim);
int fn_8009DA3C(void* p);
void fn_80090634(int a, int b, int c, int d);
void fn_800B5004(float* v, float* o);
void fn_800BBCA8(Actor* a);
int fn_8008E964(int a);
int fn_8008EA9C(int a, int b);
void fn_80122108(float a, float b, float c, float d, float e, float f, float g, int h, int i, float* v, u32 j, int k);
int fn_80212730(int h);
void fn_800CDD10(void);
}
extern int GetPlayerHandle(void);
extern float sCordLandSpeed; // 0x804D35A0
extern void fn_8007EF48(int a, int b, const char* s, int c, int d, int e, int f, int g, int h, int i);

extern int sCordBone;      // 0x804D35A4
extern int sCordHash;      // 0x804D35A8
extern int sCordFxHurt;    // 0x804D35AC
extern int sCordFxMad;     // 0x804D35B0
extern int sCordFxRecover; // 0x804D35B4
extern int sCordHash2;     // 0x804D35B8
extern int sCordFxBeep;    // 0x804D35BC
extern int sCordFxPower;   // 0x804D35C0
extern int sCordFxMelee;   // 0x804D35C4

// Stream take: advances the stream block and returns the old pointer.
inline void* StreamTake(ActorMsg* m, int n) {
    void* p = m->data;
    m->size += n;
    m->arg -= n;
    m->data += n;
    return p;
}

// Hash of the stream signature string (0x8034A4B8), via a character translation table.
extern unsigned char gCharMap[];
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
extern unsigned char gSig[];
extern u32 gCordVec0[4];
extern u32 gCordVec1[4];

// 0x80248FC8: actor message handler.
int CordMsg(int msg, void* p) {
    ActorMsg* m = (ActorMsg*)p;
    Actor* a;
    CordBeh* b;
    switch (msg) {
    case 0xC06: {
        a = m->actor;
        u32 id = m->arg;
        if (id == 0x46341C3A || id == 0x239807 || id == 0x4A5C9FC) {
            fn_800CB8BC(a, (int)a->info, a->beh->fxId, a->unk26C);
        }
        return 0;
    }
    case 0x803:
        if (m->actor->beh != 0) {
            gCordStates[m->actor->beh->state].enter(m->actor);
        }
        return 0;
    case 0x41E:
        if (m->arg != -0x527A8906) {
            return 0;
        }
        {
            String s1(gCordStates[m->actor->beh->state].name);
            String s2("");
            return 0;
        }
    case 0xC00:
    case 0x3000:
        a = m->actor;
        b = a->beh;
        if (msg == 0x3000 || m->arg == 0) {
            if (b->fx != 0) {
                fn_8015A10C(b->fx);
                b->fx = 0;
            }
            if (b->sound != -1) {
                fn_80027690(b->sound);
                b->sound = -1;
            }
            fn_80162610(a, a->sm, 0xD19E338);
            fn_80162610(a, a->sm, 0);
        }
        return 0;
    case 0xC14: {
        int pl = GetPlayerHandle();
        if (m->size != 0x7BDA2B92) {
            return 0;
        }
        fn_8007EF48(*(int*)(pl + 0x248), 1, "Blur Hit", 0, 0, 0, 0, 0, 0, pl);
        return 1;
    }
    case 0x7102: {
        CordBeh* beh = m->actor->beh;
        u32 sig;
        if (beh == 0) {
            return 1;
        }
        if (m->arg < 0x3C) {
            m->unk10 = 0;
            return 1;
        }
        sig = SigHash(gSig);
        memcpy(m->data, &sig, 4);
        m->size += 4;
        m->arg -= 4;
        m->data += 4;
        memcpy(m->data, beh, 0x38);
        m->size += 0x38;
        m->arg -= 0x38;
        m->data += 0x38;
        return 1;
    }
    case 0x7103: {
        CordBeh* beh = m->actor->beh;
        if (beh == 0) {
            return 1;
        }
        if (m->size < 0x3C) {
            m->unk10 = 0;
            return 1;
        }
        {
            u32* got = (u32*)StreamTake(m, 4);
            if (*got != SigHash(gSig)) {
                return 1;
            }
        }
        memcpy(beh, m->data, 0x38);
        m->size -= 0x38;
        m->data += 0x38;
        return 1;
    }
    }
    return 0;
}

// 0x80249544: per-frame update.
int CordUpdate(Actor* a) {
    CordBeh* b;
    int next;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (HasCondition(a->flags, 9) != 0 && b->state != 0x20 && b->state != 0x21) {
        gCordStates[b->state].canLeave(a, 0x20);
        gCordStates[32].enter(a);
    }
    if (HasCondition(a->flags, 0x1C) != 0 || fn_8008EA9C(a->unkD4, 0) == 0 || fn_8008EA9C(a->unkD4, 0) == 2) {
        if (HasCondition(a->flags, 0xD) == 0 && HasCondition(a->flags, 9) == 0 &&
            HasCondition(a->flags, 10) == 0 && HasCondition(a->flags, 11) == 0) {
            gCordStates[b->state].enter(a);
        }
        if (HasCondition(a->flags, 0x1C) != 0 && HasCondition(a->flags, 0xD) != 0) {
            fn_8008E964(a->unkD4);
        }
    }
    if (a->unk220 == 4) {
        CordAdvance(a);
        next = b->state;
    } else {
        next = gCordStates[b->state].update(a);
    }
    if (next != b->state && gCordStates[b->state].canLeave(a, next) != 0) {
        gCordStates[next].enter(a);
    }
    if (b->fx != 0 || b->sound != -1) {
        int anim = ModelAnimate(a->model, 0, 0);
        int bone = fn_8003A1F0(a->model, (int)a->pose, anim);
        if (b->fx != 0) {
            fn_8015A4AC(0.0f, 0.0f, b->fx, bone + sCordBone * 0x30, 0, 0, 0);
        }
        if (b->sound != -1) {
            u32 v[4] = {gCordVec1[0], gCordVec1[1], gCordVec1[2], gCordVec1[3]};
            int flags = 0x180;
            fn_800277B0(b->sound, bone + sCordBone * 0x30 + 0x24, v, &flags);
        }
    }
    return 1;
}

// 0x80249868: destroy.
void CordDestroy(Actor* a) {
    CordBeh* b = a->beh;
    a->update = 0;
    if (b->fx != 0) {
        fn_8015A10C(b->fx);
        b->fx = 0;
    }
    if (b->sound != -1) {
        fn_80027690(b->sound);
        b->sound = -1;
    }
    VFree(a->beh);
    a->beh = 0;
}

// 0x802498E4: create.
void CordCreate(int unused, Actor* a) {
    CordBeh* b;
    u32 sz;
    short id[8];
    if (a == 0) {
        return;
    }
    a->update = CordUpdate;
    a->unk118 = fn_800CDD10;
    b = (CordBeh*)VAlloc(0x38);
    if (b != 0) {
        a->beh = b;
        if (HasCondition(a->flags, 10) != 0 || a->unk904 <= 0.0f) {
            b->state = 0x20;
            b->next = 0x21;
        } else {
            b->state = 3;
            b->next = 3;
        }
        b->unk10 = 0;
        b->unk18 = 0.0f;
        b->fx = 0;
        b->sound = -1;
        sCordHash = AssetFind(0xF2E2BEB8, 0x49BB30EF);
        sCordHash2 = AssetFind(0xF1754BF2, 0xD9404234);
        sCordFxHurt = FxFind("cord_hurt");
        sCordFxMad = FxFind("cord_mad");
        sCordFxRecover = FxFind("cord_recover");
        sCordFxBeep = FxFind("Cord_Grenade_Beep");
        sCordFxPower = FxFind("Cord_Grenade_PowerUp");
        sCordFxMelee = FxFind("Cord_Melee_Attack");
        sCordBone = AssetFind(0xF1754BF2, 0x3B5A8CE4);
        sCordBone = ModelFindBone(a->model, sCordBone);
        id[0] = (short)sCordHash;
        TextureLoad(id, 2);
        fn_80090634(a->unkD4, 0, 0, 0);
        return;
    }
    a->beh = 0;
}

// 0x80249AAC: state handler (animation/hit events).
int CordHandlerBasic(int msg, Actor* a) {
    CordBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (msg == 0x1B83833F) {
        gCordStates[b->state].canLeave(a, b->next);
        gCordStates[b->next].enter(a);
    } else {
        if (msg < 0x1B83833F) {
            if (msg != 0x4A5C9FC) {
                if (msg > 0x4A5C9FB) {
                    return 1;
                }
                if (msg != 0x239807) {
                    return 1;
                }
            }
        } else if (msg != 0x46341C3A) {
            return 1;
        }
        fn_800CB8BC(a, (int)a->info, b->fxId, a->unk26C);
    }
    return 1;
}

struct EventMsg {
    int unk0;
    u32 hash;
    int owner;
    int unkC;
};

struct PosQuery {
    int unk0;
    u32 hash;
    char pad8[0x34 - 8];
    int pos[3];             // 0x34
};

// 0x80249BA8: state handler for attack states.
int CordHandlerAttack(int msg, Actor* a) {
    CordBeh* b;
    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (msg == 0x26772E) {
        fn_8016F120(sCordFxMad, (char*)a + 0x24);
        return 1;
    }
    if (msg < 0x26772E) {
        if (msg == 0x216D8D) {
            u32 v[4] = {gCordVec0[0], gCordVec0[1], gCordVec0[2], gCordVec0[3]};
            int anim = ModelAnimate(a->model, 0, 0);
            int bone = fn_8003A1F0(a->model, (int)a->pose, anim);
            b->fx = fn_8015A570(3.0f, 16.0f, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, -1);
            b->sound = fn_80027A58(1, bone + sCordBone * 0x30 + 0x24, v, 0x180, 9);
            fn_8016F120(sCordFxBeep, (char*)a + 0x24);
            fn_8016F120(sCordFxPower, (char*)a + 0x24);
            return 1;
        }
        if (msg < 0x216D8D) {
            if (msg != 0x20EE36) {
                if (msg > 0x20EE35) {
                    return 1;
                }
                if (msg != -0x7D6246D) {
                    return 1;
                }
            }
            if (b->fx != 0) {
                fn_8015A10C(b->fx);
                b->fx = 0;
            }
            if (b->sound != -1) {
                fn_80027690(b->sound);
                b->sound = -1;
            }
            if (fn_8016325C(a->sm) != 5) {
                fn_80162610(a, a->sm, 0xD19E338);
                fn_80162610(a, a->sm, 0x20EE36);
                return 1;
            }
            {
                EventMsg e;
                e.unk0 = fn_80162610(a, a->sm, 0xF829DB93);
                if (e.unk0 == -1) {
                    return 1;
                }
                e.hash = a->handle;
                if (e.hash == (u32)-1) {
                    return 1;
                }
                e.owner = 0x7603EBEB;
                SendMsg(e.hash, 0xC14, &e);
            }
            return 1;
        }
        if (msg != 0x239807) {
            return 1;
        }
    } else {
        if (msg == 0x1B83833F) {
            gCordStates[b->state].canLeave(a, b->next);
            gCordStates[b->next].enter(a);
            if (fn_8016325C(a->sm) == 0) {
                return 1;
            }
            fn_80162610(a, a->sm, 0x1B83833F);
            return 1;
        }
        if (msg < 0x1B83833F) {
            if (msg != 0x4A5C9FC) {
                if (msg > 0x4A5C9FB) {
                    return 1;
                }
                if (msg != 0x45D9554) {
                    return 1;
                }
                {
                    PosQuery q;
                    float v[3];
                    float big;
                    q.hash = sCordHash2;
                    SendMsg(a->handle, 0x411, &q);
                    big = 500.0f;
                    v[0] = (float)q.pos[0];
                    v[1] = (float)q.pos[1];
                    v[2] = (float)q.pos[2];
                    fn_80122108(256.0f, 256.0f, big, big, 0.75f, 0.4f, big, 0, a->handle, v, 0xB87F5DC2, a->handle);
                }
                return 1;
            }
        } else if (msg != 0x46341C3A) {
            return 1;
        }
    }
    fn_800CB8BC(a, (int)a->info, b->fxId, a->unk26C);
    return 1;
}

// 0x80249F7C: callback for timed actions (hit/anim end).
int CordHitCallback(int msg, Actor* a) {
    if (a == 0) {
        return 0;
    }
    if (msg == 0x20EE36) {
    done:
        if (fn_8016325C(a->sm) == 5) {
            EventMsg e;
            e.unk0 = fn_80162610(a, a->sm, 0xF829DB93);
            if (e.unk0 != -1) {
                e.hash = a->handle;
                if (e.hash != (u32)-1) {
                    e.owner = 0x7603EBEB;
                    SendMsg(e.hash, 0xC14, &e);
                }
            }
        } else {
            fn_80162610(a, a->sm, 0xD19E338);
            fn_80162610(a, a->sm, 0x20EE36);
        }
    } else {
        if (msg < 0x20EE36) {
            if (msg == -0x7D6246D) {
                goto done;
            }
            if (msg > -0x7D6246E) {
                return 1;
            }
            if (msg != -0x7BC0BBB1) {
                return 1;
            }
        } else if (msg != 0x3A821DDD) {
            if (msg > 0x3A821DDC) {
                return 1;
            }
            if (msg != 0x1B83833F) {
                return 1;
            }
            if (fn_8016325C(a->sm) == 0) {
                return 1;
            }
            fn_80162610(a, a->sm, 0x1B83833F);
            return 1;
        }
        fn_80162610(a, a->sm, 5);
    }
    return 1;
}

// 0x8024A0E0: leave callbacks (record the next state and clear pose bits).
int CordLeaveClear(Actor* a, int next) {
    CordBeh* b = a->beh;
    b->next = next;
    ConditionSetBit(a->flags, 0, 0);
    ConditionSetBit(a->flags, 0x3B, 0);
    ConditionSetBit(a->flags, 0x30, 0);
    if (b->state == 0x15) {
        ConditionSetBit(a->flags, 0x10, 0);
    }
    return 1;
}

int CordLeaveFx(Actor* a, int next) {
    CordBeh* b = a->beh;
    b->next = next;
    if (b->fx != 0) {
        fn_8015A10C(b->fx);
        b->fx = 0;
    }
    if (b->sound != -1) {
        fn_80027690(b->sound);
        b->sound = -1;
    }
    ConditionSetBit(a->flags, 0, 0);
    ConditionSetBit(a->flags, 0x3B, 0);
    if (b->state == 0x15) {
        ConditionSetBit(a->flags, 0x10, 0);
    }
    return 1;
}

int CordLeaveNotify(Actor* a, int next) {
    CordBeh* b = a->beh;
    EventMsg e;
    b->next = next;
    ConditionSetBit(a->flags, 0, 0);
    ConditionSetBit(a->flags, 0x3B, 0);
    if (a->handle != -1) {
        e.owner = 0xE18980E3;
        e.unk0 = 0;
        e.hash = a->handle;
        SendMsg(a->handle, 0xC14, &e);
    }
    if (b->state == 0x15) {
        ConditionSetBit(a->flags, 0x10, 0);
    }
    return 1;
}

int CordLeaveDefault(Actor* a, int next) {
    CordBeh* b = a->beh;
    b->next = next;
    ConditionSetBit(a->flags, 0, 0);
    ConditionSetBit(a->flags, 0x3B, 0);
    if (b->state == 0x15) {
        ConditionSetBit(a->flags, 0x10, 0);
    }
    return 1;
}

// 0x8024A330: current state accessor and trivial updates.
int CordGetState(Actor* a) { return a->beh->state; }

int CordUpdateDead(Actor* a) {
    CordBeh* b = a->beh;
    if (a->unk5F0 == 0 || fn_80212730(a->handle) != -1) {
        CordAdvance(a);
    }
    return b->state;
}

int CordUpdateAdvance(Actor* a) {
    CordBeh* b = a->beh;
    CordAdvance(a);
    return b->state;
}

int CordUpdateAdvance2(Actor* a) {
    CordBeh* b = a->beh;
    CordAdvance(a);
    return b->state;
}

// Movement class of the actor: 0 still, 1 moving, 2 forced.
inline int MoveClass(Actor* a) {
    if (a->unk288 == 0) {
        if (__fabs(a->speed) <= 25.0f) {
            return 0;
        }
        return 1;
    }
    return 2;
}

// Animation-end bookkeeping shared by the walk/run states.
inline void SmStep(Actor* a) {
    if (fn_8016325C(a->sm) == 6) {
        fn_80162610(a, a->sm, 0xE6B71BCC);
        fn_80162610(a, a->sm, 0x1B83833F);
    } else if (fn_8016325C(a->sm) == 7) {
        fn_80162610(a, a->sm, 0x968CBE3D);
        fn_80162610(a, a->sm, 0x1B83833F);
    }
}

inline int RandHalf(void) {
    return (float)rand() / 32767.0f <= 0.5f;
}

// 0x8024A3F8: update for the three recover states.
int CordUpdateRecover(Actor* a) {
    CordBeh* b = a->beh;
    int next;
    CordAdvance(a);
    if (MemFlagIsSet(a->flags, 5) == 0) {
        if (HasCondition(a->flags, 4) != 0 && b->state == 0x1B) {
            ConditionSetBit(a->flags, 4, 0);
            next = 0x1F;
        } else if (HasCondition(a->flags, 5) == 0 && b->state != 0x1E) {
            next = 0x1E;
        } else {
            next = b->state;
            if (next != 0x1E && next != 0x1A) {
                if (a->speed > 1.1920929e-07f) {
                    next = 0x1C;
                } else if (a->speed < -1.1920929e-07f) {
                    next = 0x1D;
                }
            }
        }
    } else {
        if (a->handle != -1) {
            EventMsg e;
            e.owner = 0xE18980E3;
            e.unk0 = 0;
            e.hash = a->handle;
            SendMsg(a->handle, 0xC14, &e);
        }
        next = 3;
    }
    return next;
}

// 0x8024A518: update for the ledge state.
int CordUpdateLedge(Actor* a) {
    CordBeh* b = a->beh;
    if (b->state == 0x16) {
        float zero[3];
        zero[0] = 0.0f;
        zero[1] = 0.0f;
        zero[2] = 0.0f;
        a->unk288 = 0;
        a->unk26C = 0;
        fn_800B5004(a->vel, zero);
        MemCopyVec3((float*)a->flags, zero);
        fn_800BBCA8(a);
    }
    CordAdvance(a);
    if (HasCondition(a->flags, 5) != 0) {
        return 0x1A;
    }
    return b->state;
}

// 0x8024A5BC: update for the run states.
int CordUpdateRun(Actor* a) {
    CordBeh* b = a->beh;
    int next = MoveClass(a);
    int dir = ZoneClassifyDirection(a, a->vel);
    if (b->state == 0x16 && fn_8016325C(a->sm) != 0) {
        float zero[3];
        zero[0] = 0.0f;
        zero[1] = 0.0f;
        zero[2] = 0.0f;
        a->unk288 = 0;
        a->unk26C = 0;
        fn_800B5004(a->vel, zero);
        MemCopyVec3((float*)a->flags, zero);
        fn_800BBCA8(a);
    }
    if (HasCondition(a->flags, 5) == 0) {
        SmStep(a);
        if (fn_8016325C(a->sm) == 0) {
            ConditionSetBit(a->flags, 0, 0);
        }
        if (fn_8016325C(a->sm) == 0 &&
            (next != 2 || ((1.1920929e-07f < __fabs(a->vel[0])) + (1.1920929e-07f < __fabs(a->vel[1])) +
                           (1.1920929e-07f < __fabs(a->vel[2]))) != 0)) {
            if (HasCondition(a->flags, 0x30) == 0) {
                if (MemFlagIsSet(a->flags, 3) != 0) {
                    next = next + (dir & (int)~((next - 2) | (2 - next)) >> 31) + 3;
                }
            } else {
                if ((float)rand() / 32767.0f >= 0.666f) {
                    next = 0x18;
                } else {
                    next = 0x17;
                }
            }
        } else {
            CordAdvance(a);
            next = b->state;
        }
    } else {
        next = 0x1A;
    }
    return next;
}

// 0x8024A854: update for the walk states.
int CordUpdateWalk(Actor* a) {
    CordBeh* b = a->beh;
    int next = MoveClass(a);
    CordAdvance(a);
    if (HasCondition(a->flags, 5) == 0) {
        SmStep(a);
        if (fn_8016325C(a->sm) == 4) {
            next = next + 0xF;
        } else if (fn_8016325C(a->sm) == 5) {
            next = 0x16;
        } else if (b->next == next) {
            next = b->state;
        } else {
            next = next + 9;
        }
    } else {
        next = 0x1A;
    }
    return next;
}

// 0x8024A9A4: update for the idle states.
int CordUpdateIdle(Actor* a) {
    CordBeh* b = a->beh;
    int next = MoveClass(a);
    int dir = ZoneClassifyDirection(a, a->vel);
    CordAdvance(a);
    if (HasCondition(a->flags, 5) == 0) {
        if (HasCondition(a->flags, 0x30) == 0) {
            if (HasCondition(a->flags, 4) != 0) {
                ConditionSetBit(a->flags, 4, 0);
                if (a != 0 && a->unkF8 >= sCordLandSpeed) {
                    int anim = fn_800A1BC8(a->info->unk14, a->info->unk1C, 0xC5936568, 0, 0);
                    if (fn_8008E7D0() != 0) {
                        ConditionSetBit(a->flags, 0x1C, 0);
                        fn_8008F4F0(a->unkF4, a->unkD4, 2);
                        fn_80090974(1.0f, a->unkD4, 2, anim, 0x10, CordHitCallback, a);
                        a->unkF8 = 0.0f;
                    }
                }
            }
            SmStep(a);
            if (fn_8016325C(a->sm) == 4 && HasCondition(a->flags, 0x2F) != 0) {
                int beh = (int)a->beh;
                ConditionSetBit(a->flags, 0x2F, 0);
                if (a != 0 && fn_8008EA9C(a->unkD4, 1) != 1) {
                    int pick;
                    if (a->unk904 <= 0.6f * (float)a->info->maxHealth) {
                        pick = RandHalf() ? 3 : 2;
                    } else {
                        pick = RandHalf() ? 1 : 0;
                    }
                    pick = fn_800A1BC8(a->info->unk14, a->info->unk1C, 0xDEAFAC63, pick, 0);
                    if (fn_8008E7D0() != 0) {
                        ConditionSetBit(a->flags, 0x1C, 0);
                        fn_8008F4F0(((CordBeh*)beh)->unk18, a->unkD4, 1);
                        fn_80090974(1.0f, a->unkD4, 1, pick, 0x10, CordHitCallback, a);
                    }
                }
            }
            if (fn_8016325C(a->sm) == 5) {
                next = 0x16;
            } else if (HasCondition(a->flags, 0x10) == 0) {
                if (MemFlagIsSet(a->flags, 3) != 0) {
                    if (b->next == next && dir == b->unk10) {
                        next = b->state;
                    } else {
                        next = next + (dir & (int)~((next - 2) | (2 - next)) >> 31) + 3;
                    }
                }
            } else {
                next = 0x15;
            }
        } else {
            if ((float)rand() / 32767.0f >= 0.666f) {
                next = 0x18;
            } else {
                next = 0x17;
            }
        }
    } else {
        next = 0x1A;
    }
    return next;
}

// 0x8024ADCC: update for the crouch states.
int CordUpdateCrouch(Actor* a) {
    CordBeh* b = a->beh;
    int next = MoveClass(a);
    CordAdvance(a);
    if (HasCondition(a->flags, 5) == 0) {
        SmStep(a);
        if (HasCondition(a->flags, 0x2F) == 0) {
            if (fn_8016325C(a->sm) == 5) {
                next = 0x16;
            } else if (HasCondition(a->flags, 0x10) == 0) {
                if (MemFlagIsSet(a->flags, 3) == 0) {
                    if (b->next == next) {
                        next = b->state;
                    }
                } else {
                    next = next + 3;
                }
            } else {
                next = 0x15;
            }
        } else {
            ConditionSetBit(a->flags, 0x2F, 0);
            next = next + 0xF;
        }
    } else {
        next = 0x1A;
    }
    return next;
}

// 0x8024AF6C
int CordEnterDead(Actor* a) {
    CordBeh* b = a->beh;
    a->unk10C = RangeResetFields;
    b->state = 0x21;
    b->next = 0x21;
    b->prevInput = 0;
    if (a->unk5F0 == 0 || fn_80212730(a->handle) != -1) {
        fn_8008DE4C(a->unkD4, 0, b->anim, 0x18);
    }
    if (HasCondition(a->flags, 10) == 0) {
        ConditionSetBit(a->flags, 10, 1);
        ActorPlayAnim(a->handle, 0xDC04E3D5, a->handle);
    }
    return 1;
}

// 0x8024B02C
int CordEnterDown(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x20;
    b->next = 0x21;
    b->prevInput = 0;
    a->unk220 = 0;
    ConditionSetBit(a->flags, 0, 1);
    ConditionSetBit(a->flags, 9, 1);
    if (a->unk5F0 == 0 || fn_80212730(a->handle) != -1) {
        CordPlayAnim(a, 0);
    }
    return 1;
}

// 0x8024B0C0
int CordEnterRecoverB(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x1F;
    b->next = 3;
    b->prevInput = 0;
    ConditionSetBit(a->flags, 0, 1);
    CordPlayAnim(a, 0);
    fn_8016F120(sCordFxMad, (char*)a + 0x24);
    return 1;
}

// 0x8024B130
int CordEnterRecoverA(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x1E;
    b->next = 3;
    b->prevInput = 0;
    ConditionSetBit(a->flags, 0, 1);
    CordPlayAnim(a, 0);
    fn_8016F120(sCordFxRecover, (char*)a + 0x24);
    return 1;
}

// 0x8024B1A0
int CordEnterTurnB(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x1D;
    b->next = 0x1B;
    b->prevInput = 0;
    ConditionSetBit(a->flags, 0, 1);
    CordPlayAnim(a, 2);
    return 1;
}

// 0x8024B204
int CordEnterTurnA(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x1C;
    b->next = 0x1B;
    b->prevInput = 0;
    ConditionSetBit(a->flags, 0, 1);
    CordPlayAnim(a, 1);
    return 1;
}

// 0x8024B268
int CordEnterAlert(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x1B;
    b->next = 0x1B;
    b->prevInput = 0;
    ConditionSetBit(a->flags, 0, 1);
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B2C8
int CordEnterHurt(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x1A;
    b->next = 0x1B;
    b->prevInput = 0;
    ConditionSetBit(a->flags, 4, 0);
    ConditionSetBit(a->flags, 0, 1);
    CordPlayAnim(a, 0);
    fn_8016F120(sCordFxHurt, (char*)a + 0x24);
    return 1;
}

// 0x8024B348
int CordEnterGrenadeB(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x19;
    b->next = 3;
    b->prevInput = 0;
    ConditionSetBit(a->flags, 0, 1);
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B3AC
int CordEnterGrenadeA(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x18;
    b->next = 3;
    b->prevInput = 0;
    b->unk18 = 0.0f;
    ConditionSetBit(a->flags, 0, 1);
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B418
int CordEnterMelee(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x17;
    b->next = 0x19;
    b->prevInput = 0;
    ConditionSetBit(a->flags, 4, 0);
    ConditionSetBit(a->flags, 0, 1);
    CordPlayAnim(a, 0);
    fn_8016F120(sCordFxMelee, (char*)a + 0x24);
    return 1;
}

// 0x8024B498
int CordEnterLedge(Actor* a) {
    CordBeh* b = a->beh;
    ConditionSetBit(a->flags, 0, 1);
    ConditionSetBit(a->flags, 0x3B, 1);
    b->state = 0x16;
    b->next = 3;
    b->prevInput = 0;
    CordPlayAnim(a, RandHalf());
    return 1;
}

// 0x8024B550
int CordEnterDrop(Actor* a) {
    CordBeh* b = a->beh;
    ConditionSetBit(a->flags, 0, 1);
    b->state = 0x15;
    b->next = 3;
    b->prevInput = 0;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B5BC
int CordEnterRunStart(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x10;
    b->next = 4;
    b->prevInput = 1;
    if (0.5f * (float)a->info->maxHealth <= a->unk904) {
        CordPlayAnim(a, 0);
    } else {
        CordPlayAnim(a, 1);
    }
    return 1;
}

// 0x8024B644
int CordEnterRunB(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 10;
    b->next = 0x10;
    b->prevInput = 1;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B688
int CordEnterRunA(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 4;
    b->next = 4;
    b->prevInput = 1;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B6C8
int CordEnterIdleRun(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 1;
    b->next = 1;
    b->prevInput = 1;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B704
int CordEnterWalkStart(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0x11;
    b->next = 5;
    b->prevInput = 2;
    if (0.5f * (float)a->info->maxHealth <= a->unk904) {
        CordPlayAnim(a, 0);
    } else {
        CordPlayAnim(a, 1);
    }
    return 1;
}

// 0x8024B78C
int CordEnterWalkB(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 0xB;
    b->next = 0x11;
    b->prevInput = 2;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B7D0
int CordEnterStrafeD(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 8;
    b->next = 8;
    b->prevInput = 2;
    b->unk10 = 3;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B818
int CordEnterStrafeC(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 7;
    b->next = 7;
    b->prevInput = 2;
    b->unk10 = 2;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B85C
int CordEnterStrafeB(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 6;
    b->next = 6;
    b->prevInput = 2;
    b->unk10 = 1;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B8A4
int CordEnterStrafeA(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 5;
    b->next = 5;
    b->prevInput = 2;
    b->unk10 = 0;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B8EC
int CordEnterWalkIdle(Actor* a) {
    CordBeh* b = a->beh;
    b->state = 2;
    b->next = 2;
    b->prevInput = 2;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024B928
int CordEnterStandStart(Actor* a) {
    CordBeh* b = a->beh;
    ConditionSetBit(a->flags, 0, 1);
    b->state = 0xF;
    b->next = 3;
    b->prevInput = 0;
    if (0.5f * (float)a->info->maxHealth <= a->unk904) {
        CordPlayAnim(a, 0);
    } else {
        CordPlayAnim(a, 1);
    }
    return 1;
}

// 0x8024B9DC
int CordEnterStandB(Actor* a) {
    CordBeh* b = a->beh;
    ConditionSetBit(a->flags, 0, 1);
    b->state = 9;
    b->next = 0xF;
    b->prevInput = 0;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024BA48
int CordEnterIdleA(Actor* a) {
    CordBeh* b = a->beh;
    ConditionSetBit(a->flags, 0, 1);
    b->state = 3;
    b->next = 3;
    b->prevInput = 0;
    b->unk10 = 0;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024BAB4
int CordEnterIdle(Actor* a) {
    CordBeh* b = a->beh;
    ConditionSetBit(a->flags, 0, 1);
    b->state = 0;
    b->next = 0;
    b->prevInput = 0;
    CordPlayAnim(a, 0);
    return 1;
}

// 0x8024BB18: per-frame movement/animation step shared by all states.
void CordAdvance(Actor* a) {
    CordBeh* b = a->beh;
    int stand = 0;
    if (a->pose == 0) {
        return;
    }
    if (fn_8008EA9C(a->unkD4, 1) != 1 || (double)gCordRise <= (double)b->unk18) {
        if (fn_8008EA9C(a->unkD4, 1) == 2) {
            b->unk18 = 0.001 * a->dt * -sCordRiseRate + b->unk18;
            if (b->unk18 > 0.0f) {
                fn_8008F4F0(b->unk18, a->unkD4, 1);
            } else {
                b->unk18 = 0.0f;
                fn_80090390(a->unkD4, 1);
            }
        }
    } else {
        b->unk18 = 0.001 * a->dt * sCordRiseRate + b->unk18;
        b->unk18 = (b->unk18 < gCordRise) ? b->unk18 : gCordRise;
        fn_8008F4F0(b->unk18, a->unkD4, 1);
    }
    a->unkF8 = 0.001 * a->dt + a->unkF8;
    if (fn_8008EA9C(a->unkD4, 2) != 1 || (double)gCordFall <= (double)a->unkF4) {
        if (fn_8008EA9C(a->unkD4, 2) == 2) {
            a->unkF4 = 0.001 * a->dt * -sCordFallRate + a->unkF4;
            if (a->unkF4 > 0.0f) {
                fn_8008F4F0(a->unkF4, a->unkD4, 2);
            } else {
                a->unkF4 = 0.0f;
                fn_80090390(a->unkD4, 2);
            }
        }
    } else {
        a->unkF4 = 0.001 * a->dt * sCordFallRate + a->unkF4;
        a->unkF4 = (a->unkF4 < gCordFall) ? a->unkF4 : gCordFall;
        fn_8008F4F0(a->unkF4, a->unkD4, 2);
    }
    {
        float d[3];
        float vel[3];
        Mtx34 m1, m2;
        Mtx34 m3;
        Vec3 e;
        float ground;
        fn_800913EC(a->dt, a->unkD4, d);
        AnimApply(a->unkD4, a->pose, ModelAnimate(a->model, (char*)a->pose, 1), a->unkD0);
        if (HasCondition(a->flags, 10) == 0 && HasCondition(a->flags, 9) == 0) {
            int bone1 = ModelFindBone(a->model, AssetFind(0xF1754BF2, 0x922BF04E));
            int bone2 = ModelFindBone(a->model, AssetFind(0xF1754BF2, 0x21C5E0));
            BoneAddRotX(a->pose, bone2, a->heading);
            SplineQuatRotateRamp(a->rot[0], (float*)a->pose, bone1, bone2, *(u32*)a->info);
        }
        if (HasCondition(a->flags, 0xD) == 0 && HasCondition(a->flags, 0xC) == 0) {
            ground = fn_800CC2D0(a, 1, &b->fxId, &b->unk28);
            if (-999999.0f != ground) {
                a->p84[2] = ground + a->pose->z;
            }
            a->pos[2] = (int)a->p84[2];
            a->pose->x = 0.0f;
            a->pose->y = 0.0f;
            a->pose->z = 0.0f;
            if (MemFlagIsSet(a->flags, 0x16) != 0 || 0.0f != a->heading) {
                int sign = 1;
                float diff = a->targetHeading - a->heading;
                if ((int)diff < 0) {
                    sign = -1;
                }
                if (__fabs(diff) <= (0.5f * __fabs(a->turnRate * a->turnRate)) / 200.0f) {
                    if (a->turnRate <= 1.1920929e-07f) {
                        if (1.1920929e-07f <= a->turnRate) {
                            a->turnRate = 0.0f;
                        } else {
                            float t = 0.001f * 200.0f * a->dt + a->turnRate;
                            if (t > 0.0f) {
                                t = 0.0f;
                            }
                            a->turnRate = t;
                        }
                    } else {
                        float t = -(0.001f * 200.0f * a->dt - a->turnRate);
                        if (t < 0.0f) {
                            t = 0.0f;
                        }
                        a->turnRate = t;
                    }
                } else {
                    float dir = 1.0f;
                    if (diff < 0.0f) {
                        dir = -1.0f;
                    }
                    a->turnRate = 0.001f * a->dt * 200.0f * dir + a->turnRate;
                }
                {
                    float t = a->turnRate;
                    float lim = -300.0f;
                    if (-300.0f <= t) {
                        lim = t;
                        if (300.0f < t) {
                            lim = 300.0f;
                        }
                    }
                    a->turnRate = lim;
                }
                a->heading = 0.001f * a->turnRate * a->dt + a->heading;
                {
                    float rem = a->targetHeading - a->heading;
                    float dir = 1.0f;
                    if (rem < 0.0f) {
                        dir = -1.0f;
                    }
                    if ((float)sign != dir || __fabs(a->heading - a->targetHeading) < 2.0f) {
                        a->heading = a->targetHeading;
                        a->turnRate = 0.0f;
                    }
                }
            }
            if (HasCondition(a->flags, 0) == 0 && MemFlagIsSet(a->flags, 4) == 0 &&
                MemFlagIsSet(a->flags, 0x17) == 0 && HasCondition(a->flags, 0x40) == 0) {
                stand = fn_800CADE0(a);
                ConditionSetBit(a->flags, 0x2D, 0);
            } else {
                ConditionSetBit(a->flags, 0x2D, 1);
            }
            {
                double spd = a->speed;
                double side = a->unk27C;
                float lim = 90.0f;
                float lim2;
                if (b->state != 0x1B && b->state != 0x1D && b->state != 0x1C) {
                    if (a->unk26C == 1) {
                        lim = a->info->unkC0 * 0.5f;
                    } else if (a->unk26C == 0) {
                        lim = 4.5f * a->info->unkC0;
                    } else if (HasCondition(a->flags, 0x28) != 0) {
                        lim = 4.0f * a->info->unkC0;
                    } else if (HasCondition(a->flags, 0x27) == 0) {
                        lim = 5.5f * a->info->unkC0;
                    } else {
                        lim = a->info->unkC0;
                    }
                }
                lim2 = 0.4f * lim;
                if (spd > 1.1920929e-07f) {
                    double step = (lim * a->dt) / 1000.0f;
                    double use = spd;
                    if (step < spd) {
                        use = step;
                    }
                    a->rot[2] = a->rot[2] + use;
                } else if (spd < -1.1920929e-07f) {
                    double step = -((lim * a->dt) / 1000.0f);
                    double use = spd;
                    if (spd < step) {
                        use = step;
                    }
                    a->rot[2] = a->rot[2] + use;
                }
                if (side > 1.1920929e-07f) {
                    double step = (lim2 * a->dt) / 1000.0f;
                    if (step < side) {
                        side = step;
                    }
                    a->rot[0] = a->rot[0] + side;
                } else if (side < -1.1920929e-07f) {
                    double step = -((lim2 * a->dt) / 1000.0f);
                    if (side < step) {
                        side = step;
                    }
                    a->rot[0] = a->rot[0] + side;
                }
                {
                    float r = a->rot[0];
                    float hi = a->info->unk08;
                    float lo = -hi;
                    if (lo <= r) {
                        lo = r;
                        if (hi < r) {
                            lo = hi;
                        }
                    }
                    a->rot[0] = lo;
                }
                if (1.1920929e-07f < __fabs(spd)) {
                    a->heading = a->targetHeading;
                }
                if (HasCondition(a->flags, 0) == 0) {
                    if (HasCondition(a->flags, 0x2D) == 0) {
                        fn_800CAD0C(a, stand);
                    }
                } else {
                    b->vel[0] = b->vel[0] + d[0];
                    b->vel[1] = b->vel[1] + d[1];
                    b->vel[2] = b->vel[2] + d[2];
                    ActorGetMatrix(&m3, a);
                    fn_800A0168(&m3, &m3);
                    fn_800A0030(d, &m3, d);
                    a->vel[0] = d[0];
                    a->vel[1] = d[1];
                    a->vel[2] = d[2];
                    a->vel[2] = 0.0f;
                    {
                        float inv = 1000.0f / a->dt;
                        a->vel[0] = a->vel[0] * inv;
                        a->vel[1] = a->vel[1] * inv;
                        a->vel[2] = a->vel[2] * inv;
                    }
                }
            }
            EulerNormalize((Vec3*)a->rot, (Vec3*)a->rot);
            Vec3Set(&e, a->heading, 0.0f, 0.0f);
            EulerNormalize(&e, &e);
            MtxSetEuler(&m1, &e);
            {
                float p[3];
                p[0] = (float)a->pos[0];
                p[1] = (float)a->pos[1];
                p[2] = (float)a->pos[2];
                PlacementToMatrix(a, &m1);
                ActorGetMatrix(&m3, a);
                fn_800A0168(&m3, &m3);
                d[1] = 0.0f;
                d[2] = 0.0f;
                fn_800A0030(d, &m3, d);
                p[0] = d[0] + p[0];
                p[1] = d[1] + p[1];
                p[2] = d[2] + p[2];
            }
            ModelSetMatrix(a->model, &m1, a->pose);
        } else if (HasCondition(a->flags, 0xC) == 0) {
            a->rot[0] = 0.0f;
            ground = fn_800CC2D0(a, 1, &b->fxId, &b->unk28);
            if (-999999.0f != ground) {
                a->p84[2] = ground + a->pose->z;
                a->pose->z = 0.0f;
            }
            a->pos[2] = (int)a->p84[2];
            ActorGetMatrix(&m2, a);
            fn_800A0168(&m2, &m2);
            fn_800A0030(d, &m2, d);
            a->vel[0] = d[0];
            a->vel[1] = d[1];
            a->vel[2] = d[2];
            a->vel[2] = 0.0f;
            {
                float inv = 1000.0f / a->dt;
                a->vel[0] = a->vel[0] * inv;
                a->vel[1] = a->vel[1] * inv;
                a->vel[2] = a->vel[2] * inv;
            }
            Vec3Set(&e, a->heading, 0.0f, 0.0f);
            EulerNormalize(&e, &e);
            MtxSetEuler(&m1, &e);
            PlacementToMatrix(a, &m1);
            ModelSetMatrix(a->model, &m1, a->pose);
        } else {
            fn_800CC2D0(a, 1, &b->fxId, &b->unk28);
            if (fn_8008EA9C(a->unkD4, 0) != 0) {
                if (HasCondition(a->flags, 0xC) == 0) {
                    Vec3Set(&e, a->heading, 0.0f, 0.0f);
                    EulerNormalize(&e, &e);
                    MtxSetEuler(&m1, &e);
                    PlacementToMatrix(a, &m1);
                } else {
                    BoneGetMatrix(&m1, a->pose);
                    MtxGetEuler((Vec3*)a->rot, &m1);
                    PlacementToMatrix(a, &m1);
                    a->p84[0] = a->pose->x;
                    a->p84[1] = a->pose->y;
                    a->p84[2] = a->pose->z;
                    a->pos[0] = (int)a->pose->x;
                    a->pos[1] = (int)a->pose->y;
                    a->pos[2] = (int)a->pose->z;
                }
            }
            ModelSetMatrix(a->model, 0, a->pose);
        }
    }
}

// 0x8024C7D4: start an animation for the current state.
void CordPlayAnim(Actor* a, int variant) {
    CordBeh* b;
    int anim;
    float blend;
    if (a == 0) {
        return;
    }
    b = a->beh;
    blend = 1.0f;
    if (b->state == 0x16 && a->unk904 < 0.6f * (float)a->info->maxHealth) {
        blend = 1.25f;
    }
    if (b->state == 0x19 || b->state == 0x17) {
        blend = 4.0f;
    }
    if (b->state == 0x1D || b->state == 0x1C) {
        blend = 2.0f;
    }
    if (b->state == 0x18) {
        blend = 0.6f;
    }
    b->vel[0] = 0.0f;
    b->vel[1] = 0.0f;
    b->vel[2] = 0.0f;
    if (HasCondition(a->flags, 0xC) != 0) {
        ConditionSetBit(a->flags, 0xC, 0);
        ConditionSetBit(a->flags, 0xE, 1);
        fn_8008F4E8(a->unkD4, 0);
        fn_8008F378(a->unkD4);
        fn_8008F4EC(a->unkD4, 0);
    }
    anim = fn_800A1BC8(a->info->unk14, a->info->unk1C, *(u32*)((char*)gCordStates + b->state * 0x18 - 4), variant, 0);
    if (fn_8008E7D0() != 0) {
        ConditionSetBit(a->flags, 0x1C, 0);
        if (HasCondition(a->flags, 0xE) == 0 && anim != b->anim) {
            float t = 150.0f;
            if ((b->state == 0x17 || b->state == 0x18) &&
                (b->anim == fn_800A1BC8(a->info->unk14, a->info->unk1C, gCordAnimHash[0], 0, 0) ||
                 b->anim == fn_800A1BC8(a->info->unk14, a->info->unk1C, gCordAnimHash[0], 1, 0))) {
                t = 400.0f * blend;
            }
            fn_8008F230(t, a->unkD4, 0, 1);
        } else {
            fn_8008F230(0.0f, a->unkD4, 0, 0);
            ConditionSetBit(a->flags, 0xE, 0);
        }
        fn_80090974(blend, a->unkD4, 0, anim, 9, (int (*)(int, Actor*))gCordStates[b->state].handler, a);
        b->anim = anim;
    }
}

// 0x8024CA7C: derive the two ramp rates.
void CordInitRates(void) {
    sCordRiseRate = gCordRise / gCordRiseDiv;
    sCordFallRate = gCordFall / gCordFallDiv;
}

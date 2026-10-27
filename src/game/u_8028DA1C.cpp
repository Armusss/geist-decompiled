// u_8028DA1C.cpp: walking/driving host behaviour with a six-state machine (0x8028DA1C-0x80290C58).
// A controllable actor keeps a 0x40-byte state block (current/next state, animation hash,
// vehicle-style velocity and lean values).  The message handler answers state-name queries,
// forwards messages to the linked actor, saves/loads the block, and the update moves the
// actor with swept-collision steps.  The state table at 0x803C2AA8 holds each state's
// animation key and callbacks.  Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned char u8;
typedef unsigned short u16;

extern "C" {
void* memcpy(void* dst, const void* src, unsigned long n);
void* memset(void* dst, int c, unsigned long n);
int toupper(int c);
}

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Mtx34 {
    float m[12];
};

struct Place {
    float f[9];
};

struct Actor;

// Actor type record (hung off the actor at 0xC4).
struct ActorType {
    int id;                 // 0x00
    char pad04[4];
    float maxLean;          // 0x08
    char pad0C[8];
    int boneA;              // 0x14
    char pad18[4];
    int boneB;              // 0x1C
    char pad20[0xA0];
    float topSpeed;         // 0xC0
};

// Per-actor state block (VAlloc'd, 0x40 bytes).
struct Beh {
    int state;              // 0x00
    int next;               // 0x04
    int flag;               // 0x08
    int anim;               // 0x0C
    int unk10;              // 0x10
    int unk14;              // 0x14
    int linked;             // 0x18
    float vel[3];           // 0x1C
    float lean;             // 0x28
    int leanSide;           // 0x2C
    float prevA;            // 0x30
    float prevB;            // 0x34
    short idleA;            // 0x38
    short idleB;            // 0x3A
    short idleC;            // 0x3C
    short pad3E;
};

struct Node {
    char pad00[0x10];
    float x;                        // 0x10
    float y;                        // 0x14
    float z;                        // 0x18
};

struct Actor {
    char pad00[0x24];
    int x;                          // 0x24
    int y;                          // 0x28
    int z;                          // 0x2C
    char pad30[8];
    int id;                         // 0x38
    char pad3C[0x84 - 0x3C];
    float fx;                       // 0x84
    float fy;                       // 0x88
    float fz;                       // 0x8C
    char pad90[0xC4 - 0x90];
    ActorType* type;                // 0xC4
    char padC8[4];
    int model;                      // 0xCC
    int unkD0;                      // 0xD0
    int anim;                       // 0xD4
    char padD8[0xF0 - 0xD8];
    Node* node;                     // 0xF0
    char padF4[0x10C - 0xF4];
    void* f10C;                     // 0x10C
    void* f110;                     // 0x110
    char pad114[4];
    void* f118;                     // 0x118
    char pad11C[0x134 - 0x11C];
    Beh* beh;                       // 0x134
    float dt;                       // 0x138
    char pad13C[0x220 - 0x13C];
    int state220;                   // 0x220
    char pad224[0x22C - 0x224];
    Actor* target;                  // 0x22C
    char pad230[0x240 - 0x230];
    float vx;                       // 0x240
    float vy;                       // 0x244
    float vz;                       // 0x248
    float lean;                     // 0x24C
    char pad250[4];
    float spin;                     // 0x254
    float f258;                     // 0x258
    char pad25C[0x26C - 0x25C];
    int i26C;                       // 0x26C
    char st[0x284 - 0x270];         // 0x270
    float steer;                    // 0x284
    int i288;                       // 0x288
    char pad28C[0x2AC - 0x28C];
    float f2AC;                     // 0x2AC
    float f2B0;                     // 0x2B0
    char pad2B4[0x2C0 - 0x2B4];
    int i2C0;                       // 0x2C0
    int i2C4;                       // 0x2C4
    char pad2C8[0x5F0 - 0x2C8];
    u32 i5F0;                       // 0x5F0
    char pad5F4[0x894 - 0x5F4];
    char animState[0x900 - 0x894];  // 0x894
    float f900;                     // 0x900
    float f904;                     // 0x904
    u32 i908;                       // 0x908
    char pad90C[0x920 - 0x90C];
    short s920;                     // 0x920
    short s922;                     // 0x922
};

// One entry of the state table (0x18 bytes).
typedef int (*StateFn)(Actor* a);
typedef int (*StateLeaveFn)(Actor* a, int next);
struct StateRec {
    u32 key;                // 0x00 animation key
    StateFn enter;          // 0x04
    StateLeaveFn leave;     // 0x08
    StateFn update;         // 0x0C
    StateLeaveFn onAnim;    // 0x10
    const char* name;       // 0x14
};

// Message payloads.
struct StreamMsg {
    Actor* actor;           // 0x00
    int left;               // 0x04
    char* ptr;              // 0x08
    int used;               // 0x0C
    int ok;                 // 0x10
};

struct Msg41E {
    Actor* actor;           // 0x00
    u32 key;                // 0x04
    int unk08;              // 0x08
    Buffer result;          // 0x0C
};

struct Msg7203 {
    Actor* actor;
    int a;
    int b;
    int c;
};

struct Msg108 {
    int w[0x42];
};

extern "C" {
int SendMsg(int to, int msg, void* data);
void fn_800CB8BC(Actor* a, ActorType* t, int b, int c);
int fn_8008DCDC(int anim, int a, int b, int c);
void fn_8008F4E8(int anim, int a);
void fn_8008F378(int anim);
void fn_8008F4EC(int anim, int a);
void fn_800C5210(int id, int msg, void* data);
String fn_800225C8(const String& a, const String& b);
void fn_8028C188(Buffer* b, char* data, int len);
void fn_800B6CAC(Actor* a, void* fn);
void fn_800B6D04(Actor* a, void* fn);
float fn_800CDD10(Actor* a, float* out);
int fn_80212730(int id);
void fn_8021284C(int id);
void fn_8009F190(Vec3* out, void* in);
void fn_8009F028(Actor* a, Vec3* v);
void fn_800913EC(float dt, int anim, float* out);
void fn_8008F630(int anim, Node* n, int a, int b);
int fn_8008EA9C(int anim, int a);
int fn_8008E964(int anim);
int fn_8016DFBC(void* p);
int fn_800C8198(void);
int fn_800C7E80(void* p);
void fn_80090634(int anim, int a, int b, int c);
int fn_80049940(void);
int fn_800BA7FC(int a);
float fn_800E1020(void* v);
void fn_800E1098(float f, void* in, void* out);
int fn_800BB218(float f, Actor* a, int b, void* from, void* to, Actor* c, float* d, int* e, int* f2);
void fn_800BAFF8(float a, float b, Actor* c, void* d, int e, int f, void* g);
void fn_800BAF78(void* out, int a, int b, int c);
void fn_800BB8B8(Actor* a, float* out);
int fn_8016325C(void* anim);
void fn_80162610(Actor* a, void* anim, u32 key);
void fn_8008DE4C(int anim, int a, int b, int c);
void fn_802127A0(int a, int b, int c, void* d, int e, int f, int g);
void fn_800C5654(int id, u32 key, int arg);
float fn_800CC308(Actor* a, int b, int c, int* d, int* e);
void fn_800A0168(Mtx34* in, Mtx34* out);
void fn_800A0030(float* v, Mtx34* m, float* out);
void fn_8016D4EC(float* a, float* b);
void fn_8016D440(float* out, void* st);
void fn_8016D614(float* a, float* b, float* c);
int fn_800A1BC8(int a, int b, u32 key, int c, int d);
void fn_8008F230(float f, int anim, int a, int b);
void fn_80090974(float f, int anim, int a, int b, int c, void* fn, void* arg);
void fn_8008F4F0(float f, int anim, int a);
void fn_800A0EF0(Mtx34* out, Node* n);
int fn_8008E7D0(int a);
void fn_800B3164(Actor* a, int b, int c);
void fn_800CECCC(Actor* a);
void fn_800B42E0(float f, float* pos, int id, int a, int b);
void fn_800A7068(void* p);
void fn_800B1834(Actor* a);
void fn_800B2BFC(Actor* a);
}

int ConditionSetBit(void* cond, int bit, int set);
int ConditionUpdateBit(void* cond, int bit, int set);
int HasCondition(void* cond, int bit);
int MemFlagIsSet(void* bits, int index);
void* VAlloc(int size);
void VFree(void* p);
u32 AssetFind(u32 table, u32 key);
void ActorGetMatrix(Mtx34* out, Actor* a);
int ModelAnimate(int model, char* name, int a);
void AnimApply(int anim, char* node, int a, int b);
void ModelSetMatrix(int model, Mtx34* m, Node* n);
int ActorIsActive(Actor* a);
void ActorRelinkOwners(Actor* a);
int ModelBoneName(int model, int a);
int ModelFindBone(int model, int key);
void ActorPlayAnim(int id, u32 key, int arg);
void ActorUpdateBoneBounds(void* p);
void ActorAim(int* out, Actor* a, int b);
int OwnerGetInfo(u32 handle);
int ZoneFindNearestHit(void* zone, Vec3* from, Vec3* to, float* dist, int* hit, float* frac, int flags);
void VecSwapXY(IVec3* a, IVec3* b);
void FixedToRad(Vec3* out, IVec3* in);
void MtxGetEuler(Vec3* out, Mtx34* m);
void EulerNormalize(float* a, float* b);
void Vec3Set(float* out, float x, float y, float z);
void MtxSetEuler(Mtx34* m, float* e);
void PlacementToMatrix(Mtx34* m, Actor* a);
void BoneAddRotX(Node* n, int bone, float f);
void SplineQuatRotateRamp(float f, Node* n, int a, int b, u32 key);
void TextureLoad(short* id, int n);
void OwnerExpireAnimActors(Actor* o);
void OwnerUpdateSlots(Actor* o);

// Data owned by other files.
extern StateRec lbl_803C2AA8[6];
extern char lbl_803C2A78[];
extern int lbl_804D37E8;
extern int lbl_804D37EC;
extern int lbl_804D37F0;
extern int lbl_804D37F4;
extern int lbl_804D37F8;
extern int lbl_804D37FC;
extern unsigned char lbl_803C6B08[];
extern unsigned char lbl_8034B5C8[];

int HostDriveMsg(int msg, int* data);
int HostDriveInit2(int* data);
int HostDriveUpdate(Actor* a);
void HostDriveFree(Actor* a);
void HostDriveInit(int unused, Actor* a);
float HostDriveSweep(Actor* a);
float HostDriveSnap(Actor* a, float* out);
void HostDriveAnimate(Actor* a);

// The save-block magic: hash of "[FCOSVE]" through the toupper table.
static inline u32 HostDriveMagic() {
    u32 h = 0;
    const unsigned char* s = lbl_8034B5C8;
    const unsigned char* p;

    for (p = s; *p != 0; p++) {
        u32 c;

        if (*s == 0xFF) {
            c = 0xFFFFFFFF;
        } else {
            c = lbl_803C6B08[*s];
        }
        h = h * 31 + (c & 0xFF);
        s++;
    }
    h = h % 0xFFFFFFFF;
    return h;
}

static inline void HostDriveStateName(Buffer* out, const char* s) {
    String name(s);
    String newline("\n");
    const String& text = fn_800225C8(name, newline);

    fn_8028C188(out, text.mData, text.mLength);
}

// 0x8028DA1C: message handler.
int HostDriveMsg(int msg, int* data) {
    Actor* a;
    Beh* b;

    switch (msg) {
    case 0xC06: {
        int key;

        a = (Actor*)data[0];
        if (data[3] != a->id) {
            return 1;
        }
        key = data[1];
        if (key == 0x46341C3A || key == 0x239807 || key == 0x4A5C9FC) {
            fn_800CB8BC(a, a->type, a->beh->unk10, a->i26C);
        }
        if (data[1] == 0x1B83833F && (fn_8008DCDC(a->anim, 0, 0, 0) & 0x10) != 0) {
            if (a->state220 == 4) {
                a->state220 = 0;
            }
            ConditionSetBit(a->st, 0xD, 0);
            if (HasCondition(a->st, 0xC) != 0) {
                ConditionSetBit(a->st, 0xC, 0);
                ConditionSetBit(a->st, 0xE, 1);
                fn_8008F4E8(a->anim, 0);
                fn_8008F378(a->anim);
                fn_8008F4EC(a->anim, 0);
            }
        }
        fn_800C5210(a->id, 0xC06, data);
        return 1;
    }
    case 0x41E: {
        Msg41E* m = (Msg41E*)data;

        a = m->actor;
        switch (m->key) {
        case 0x1F1BE1:
            data[3] = ((int*)a->beh)[6];
            return 1;
        case 0xAD8576FA:
            HostDriveStateName(&m->result, lbl_803C2AA8[a->beh->state].name);
            return 0;
        }
        return 0;
    }
    case 10:
        b = ((Actor*)data[0])->beh;
        if (b->linked != -1) {
            Msg108 copy = *(Msg108*)data;

            SendMsg(b->linked, 10, &copy);
            return 1;
        }
        return 0;
    case 0x41D: {
        int key = data[1];

        a = (Actor*)data[0];
        b = a->beh;
        switch (key) {
        case 0x5D284369:
            b->idleC = (short)data[3];
            return 1;
        case 0x255C25:
            b->idleA = (short)data[3];
            return 1;
        case 0xD9386E39:
            if (data[3] == 0) {
                fn_800B6CAC(a, (void*)HostDriveSnap);
                a->f118 = (void*)fn_800CDD10;
            } else {
                fn_800B6CAC(a, (void*)HostDriveSnap);
                fn_800B6D04(a, (void*)HostDriveSnap);
                a->f118 = (void*)HostDriveSweep;
            }
            return 1;
        case 0x6A421D57:
            b->idleB = (short)data[3];
            return 1;
        }
        return 0;
    }
    case 0x803:
        a = (Actor*)data[0];
        if (a->beh != 0) {
            lbl_803C2AA8[a->beh->state].enter(a);
        }
        return 0;
    case 0x7103: {
        StreamMsg* m = (StreamMsg*)data;

        b = m->actor->beh;
        if (b == 0) {
            return 1;
        }
        if (m->used < 0x44) {
            m->ok = 0;
            return 1;
        }
        {
            int good;

            if (m->used < 4) {
                good = 0;
            } else {
                u32* p = (u32*)m->ptr;

                m->used += 4;
                m->left -= 4;
                m->ptr += 4;
                good = (*p == HostDriveMagic());
            }
            if (!good) {
                return 1;
            }
        }
        memcpy(b, m->ptr, 0x40);
        m->used -= 0x40;
        m->ptr += 0x40;
        return 1;
    }
    case 0x7102: {
        StreamMsg* m = (StreamMsg*)data;
        u32 magic;

        b = m->actor->beh;
        if (b == 0) {
            return 1;
        }
        if (m->left < 0x44) {
            m->ok = 0;
            return 1;
        }
        magic = HostDriveMagic();
        memcpy(m->ptr, &magic, 4);
        m->used += 4;
        m->left -= 4;
        m->ptr += 4;
        memcpy(m->ptr, b, 0x40);
        m->used += 0x40;
        m->left -= 0x40;
        m->ptr += 0x40;
        return 1;
    }
    case 0xC07:
        return HostDriveInit2(data);
    case 0x7203: {
        Msg7203 m;

        m.actor = (Actor*)data[0];
        if (m.actor->beh->linked != -1) {
            m.a = data[1];
            m.b = data[2];
            m.c = data[3];
            SendMsg(m.actor->beh->linked, 0x7203, &m);
        }
        return 0;
    }
    }
    return 0;
}

// 0x8028E0F8: place the actor from a teleport message and refresh its model.
int HostDriveInit2(int* data) {
    Actor* a = (Actor*)data[0];
    Beh* b = a->beh;
    Mtx34 mtx;

    if (fn_80212730(a->id) != -1) {
        fn_8021284C(a->id);
    }
    if ((data[1] & 4) != 0) {
        char reply[0x60];
        Vec3 euler;

        reply[0x0C] = 0;
        if (SendMsg(data[2], 0x402, reply) == 0) {
            return 0;
        }
        fn_8009F190(&euler, reply + 0x14);
        FixedToRad(&euler, (IVec3*)(data + 7));
        data[4] = (int)euler.x;
        data[5] = (int)euler.y;
        data[6] = (int)euler.z;
    }
    if ((data[1] & 1) != 0) {
        a->fx = (float)data[4];
        a->fy = (float)data[5];
        a->fz = (float)data[6];
        a->x = data[4];
        a->y = data[5];
        a->z = data[6];
    }
    if ((data[1] & 2) != 0) {
        Vec3 v;

        VecSwapXY((IVec3*)&v, (IVec3*)(data + 7));
        FixedToRad((Vec3*)a->st, (IVec3*)&v);
        fn_8009F028(a, &v);
    }
    fn_800913EC(1.0f, a->anim, 0);
    ActorGetMatrix(&mtx, a);
    fn_8008F630(a->anim, a->node, ModelAnimate(a->model, (char*)a->node, 1), a->unkD0);
    a->node->x = 0.0f;
    a->node->y = 0.0f;
    a->node->z = 0.0f;
    ModelSetMatrix(a->model, &mtx, a->node);
    {
        int active = ActorIsActive(a);

        ActorRelinkOwners(a);
        if (active == 0) {
            int m[2];

            m[1] = 0;
            SendMsg(a->id, 0xC00, m);
        }
    }
    fn_802127A0(b->linked, a->id, ModelBoneName(a->model, 0), lbl_803C2A78, 1, 0, 0);
    return 1;
}

static inline int CondAny(Actor* a) {
    return HasCondition(a->st, 0xD) != 0 || HasCondition(a->st, 9) != 0 || HasCondition(a->st, 10) != 0 ||
           HasCondition(a->st, 0xB) != 0;
}

// 0x8028E334: per-frame update of the actor.
int HostDriveUpdate(Actor* a) {
    Beh* b;
    int next;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    if (HasCondition(a->st, 9) != 0 && b->state != 4 && b->state != 5) {
        lbl_803C2AA8[b->state].leave(a, 4);
        lbl_803C2AA8[4].enter(a);
    }
    if (HasCondition(a->st, 0x1C) != 0 || fn_8008EA9C(a->anim, 0) == 0 || fn_8008EA9C(a->anim, 0) == 2) {
        if (!CondAny(a)) {
            lbl_803C2AA8[b->state].enter(a);
        }
        if (HasCondition(a->st, 0x1C) != 0 && HasCondition(a->st, 0xD) != 0 && fn_8008E964(a->anim) != 0) {
            ConditionSetBit(a->st, 0x1C, 0);
            ConditionSetBit(a->st, 0x1D, 1);
        }
    }
    if (b->linked == -1) {
        b->prevA = 0.0f;
        b->prevB = 0.0f;
    } else {
        Msg41E m;
        int ok;
        u32 other;
        float ta;
        float tb;

        m.key = 0x1C1A4CC9;
        m.unk08 = 0;
        ok = SendMsg(b->linked, 0x41E, &m);
        other = *(u32*)&m.result;
        if (ok == 0 || other == 0xFFFFFFFF || (other >> 24) != 2) {
            b->prevA = 0.0f;
            b->prevB = 0.0f;
        } else {
            int info[9];

            if (SendMsg(other, 0x400, info) == 0 || info[0] == 0) {
                ta = 0.0f;
                tb = 0.0f;
            } else {
                int kind = *(int*)(info[0] + 0x588);

                if (kind == 0xB || kind == 3) {
                    ta = *(float*)(info[0] + 0x394);
                    tb = *(float*)(info[0] + 0x390);
                } else if (kind == 2) {
                    ta = *(float*)(info[0] + 0x394);
                    tb = *(float*)(info[0] + 0x38C);
                } else {
                    ta = 0.0f;
                    tb = 0.0f;
                }
            }
            if (tb > -0.5f && b->prevA <= -0.5f) {
                fn_800C5654(a->id, 0x15466978, 0x2000000);
            } else if (tb >= 0.5f && b->prevA < 0.5f) {
                fn_800C5654(a->id, 0x93DD26AB, 0x2000000);
            }
            b->prevA = tb;
            b->prevB = ta;
        }
    }
    if (a->state220 == 4) {
        HostDriveAnimate(a);
        next = b->state;
    } else {
        next = lbl_803C2AA8[b->state].update(a);
    }
    if (next != b->state && lbl_803C2AA8[b->state].leave(a, next) != 0) {
        lbl_803C2AA8[next].enter(a);
    }
    ActorUpdateBoneBounds(a);
    return 1;
}

// 0x8028E81C: destroy callback.
void HostDriveFree(Actor* a) {
    a->f110 = 0;
    VFree(a->beh);
    a->beh = 0;
}

// 0x8028E85C: init callback.
void HostDriveInit(int unused, Actor* a) {
    Beh* b;

    if (a == 0) {
        return;
    }
    a->f110 = (void*)HostDriveUpdate;
    a->f118 = (void*)fn_800CDD10;
    b = (Beh*)VAlloc(0x40);
    if (b == 0) {
        a->beh = 0;
        return;
    }
    memset(b, 0, 0x40);
    a->beh = b;
    if (HasCondition(a->st, 10) != 0 || a->f904 <= 0.0f) {
        b->state = 4;
        b->next = 5;
    } else {
        b->state = 0;
        b->next = 0;
    }
    lbl_804D37E8 = AssetFind(0xF2E2BEB8, 0x49BB30EF);
    if (a->type->id == 0x40D2FC38) {
        fn_800B6D04(a, (void*)HostDriveSnap);
    }
    if (a->type->id != 0x22AF26 && fn_800C8198() != 0) {
        int spawn[14];

        memset(spawn, 0, 0x38);
        if (a->type->id == 0x40D2FC38) {
            spawn[5] = 0xCFEC1D72;
        } else {
            spawn[5] = 0x8B87B997;
        }
        spawn[4] = -1;
        spawn[13] = -1;
        {
            Mtx34 m;
            Vec3 euler;

            ActorGetMatrix(&m, a);
            MtxGetEuler(&euler, &m);
        }
        spawn[0] = *(int*)&a->fx;
        spawn[1] = *(int*)&a->fy;
        spawn[2] = *(int*)&a->fz;
        {
            int other = fn_800C7E80(spawn);

            if (other != 0) {
                b->linked = *(int*)(other + 0x38);
                fn_802127A0(b->linked, a->id, ModelBoneName(a->model, 0), lbl_803C2A78, 1, 0, 0);
            }
        }
    }
    ConditionSetBit(a->st, 0x1B, 1);
    {
        short tex[2];

        tex[0] = (short)lbl_804D37E8;
        TextureLoad(tex, 2);
    }
    fn_80090634(a->anim, 0, 0, 0);
}

void HostDriveSetAnim(Actor* a);

static inline float Sq(float v) {
    return v * v;
}

// 0x8028EA78: move the actor by its velocity with swept collision; returns the moved fraction.
float HostDriveSweep(Actor* a) {
    Beh* b = a->beh;
    float frac = 1.0f;

    if (a->i2C0 == 1 || a->i2C4 != -1) {
        float k = a->dt / 1000.0f;
        float step[3];
        float from[3];
        float to[3];
        float dest[3];
        float probe[3];

        step[0] = a->vx * k;
        step[1] = a->vy * k;
        step[2] = a->vz * k;
        from[0] = a->fx;
        to[0] = from[0] + step[0];
        from[1] = a->fy;
        to[1] = from[1] + step[1];
        from[2] = a->fz;
        to[2] = from[2] + step[2];
        dest[0] = to[0];
        dest[1] = to[1];
        dest[2] = to[2];
        probe[0] = to[0];
        probe[1] = to[1];
        probe[2] = to[2];
        if (HasCondition(a->st, 1) == 0 && (HasCondition(a->st, 0) == 0 || a->i2C4 != -1)) {
            if (a->state220 == 2 && a->target != 0) {
                int axis[3];
                Actor* t = a->target;
                float dir[3];
                float rel[3];
                float along;
                float side;

                ActorAim(axis, t, 1);
                dir[0] = 0.000244140625f * (float)axis[0];
                dir[1] = 0.000244140625f * (float)axis[1];
                dir[2] = 0.000244140625f * (float)axis[2];
                rel[0] = dest[0] - (float)t->x;
                rel[1] = dest[1] - (float)t->y;
                rel[2] = dest[2] - (float)t->z;
                side = dir[0] * rel[1] - dir[1] * rel[0];
                along = (dir[2] * rel[2] + dir[0] * rel[0] + dir[1] * rel[1]) /
                        (dir[2] * dir[2] + dir[0] * dir[0] + dir[1] * dir[1]);
                if ((b->idleA == b->idleC && side < 0.0f) || (b->idleA == b->idleB && side > 0.0f)) {
                    to[0] = dir[0] * along + (float)t->x;
                    to[1] = dir[1] * along + (float)t->y;
                    to[2] = dir[2] * along + (float)t->z;
                    dest[0] = to[0];
                    dest[1] = to[1];
                    dest[2] = to[2];
                }
            }
            {
                float snap[3];
                float d[3];

                fn_800BB8B8(a, snap);
                d[0] = snap[0] - a->fx;
                d[1] = snap[1] - a->fy;
                d[2] = snap[2] - a->fz;
                if ((1.1920928955078125e-07f < __fabsf(snap[0]) || 1.1920928955078125e-07f < __fabsf(snap[1]) ||
                     1.1920928955078125e-07f < __fabsf(snap[2])) &&
                    Sq(d[2]) + Sq(d[0]) + Sq(d[1]) < Sq(step[2]) + Sq(step[0]) + Sq(step[1])) {
                    dest[0] = snap[0];
                    dest[1] = snap[1];
                    dest[2] = snap[2];
                    to[0] = snap[0];
                    to[1] = snap[1];
                    to[2] = snap[2];
                }
            }
            if (fn_80049940() == 0 && fn_800BA7FC(0x2B) == 0) {
                float dist;
                int hit;
                float frac2;

                if (ZoneFindNearestHit(a, (Vec3*)&a->fx, (Vec3*)dest, &dist, &hit, &frac2, -1) != 0) {
                    u32 kind = *(u32*)(hit + 0x38) >> 24;

                    if (kind != 2 && kind != 1 && dist < 1.0f) {
                        char reply[0x40];
                        float rel[3];

                        SendMsg(*(u32*)(hit + 0x38), 0x400, reply);
                        if ((*(u32*)(reply + 0x18) & 2) != 0) {
                            float hx = (float)*(int*)(hit + 0x24);
                            float hy = (float)*(int*)(hit + 0x28);
                            float hz = (float)*(int*)(hit + 0x2C);

                            rel[0] = dest[0] - hx;
                            rel[1] = dest[1] - hy;
                            rel[2] = dest[2] - hz;
                            fn_800E1098(frac2 + a->f900, rel, rel);
                            to[0] = rel[0] + hx;
                            to[1] = rel[1] + hy;
                            to[2] = rel[2] + hz;
                            frac = dist;
                            dest[0] = to[0];
                            dest[1] = to[1];
                            dest[2] = to[2];
                        }
                    }
                }
            }
            if (a->i2C0 != 1) {
                float part;
                int segA;
                int segB;

                ConditionUpdateBit(a->st, 7, 0);
                if (fn_800BB218(a->f900, a, a->i2C4, from, to, a, &part, &segA, &segB) == 0) {
                    dest[0] = to[0];
                    dest[1] = to[1];
                    dest[2] = to[2];
                } else {
                    float len = fn_800E1020(step);
                    float rest = 1.0f - part;

                    part = 0.0f;
                    ConditionUpdateBit(a->st, 7, 1);
                    fn_800BAFF8(part, len * rest, a, step, segA, segB, to);
                    step[0] *= part;
                    step[1] *= part;
                    step[2] *= part;
                    from[0] += step[0];
                    from[1] += step[1];
                    from[2] += step[2];
                    probe[0] = from[0];
                    probe[1] = from[1];
                    probe[2] = from[2];
                    {
                        float part2;
                        int segC;
                        int segD;

                        if (fn_800BB218(a->f900, a, a->i2C4, probe, to, a, &part2, &segC, &segD) == 0) {
                            dest[0] = to[0];
                            dest[1] = to[1];
                            dest[2] = to[2];
                        } else {
                            part = 0.0f;
                            if (segA != segC || (segA == segC && segB != segD)) {
                                dest[0] = from[0];
                                dest[1] = from[1];
                                dest[2] = from[2];
                            } else {
                                float push[3];

                                fn_800BAF78(push, a->i2C4, segC, segD);
                                fn_800E1098(5.0f, push, push);
                                dest[0] = a->fx + push[0];
                                dest[1] = a->fy + push[1];
                                dest[2] = a->fz + push[2];
                            }
                        }
                        frac = part;
                        if (1.0f < frac) {
                            frac = 1.0f;
                        }
                    }
                }
            }
            a->fx = dest[0];
            a->fy = dest[1];
            a->fz = dest[2];
            a->x = (int)a->fx;
            a->y = (int)a->fy;
            a->z = (int)a->fz;
        } else {
            frac = 1.0f;
            a->fx = to[0];
            a->fy = to[1];
            a->fz = to[2];
            a->x = (int)a->fx;
            a->y = (int)a->fy;
            a->z = (int)a->fz;
        }
    } else {
        frac = 0.0f;
        a->vx = 0.0f;
        a->vy = 0.0f;
        a->vz = 0.0f;
        a->i26C = 0;
    }
    return frac;
}

// 0x8028F258: nudge toward the target's line; returns the push strength.
float HostDriveSnap(Actor* a, float* out) {
    float strength = 0.0f;

    if (a->state220 == 2 && a->target != 0) {
        Actor* t = a->target;
        int axis[3];
        float dir[3];
        int rel[3];
        float along;
        float len;

        ActorAim(axis, t, 1);
        rel[0] = a->x - t->x;
        rel[1] = a->y - t->y;
        rel[2] = a->z - t->z;
        dir[0] = 0.000244140625f * (float)axis[0];
        dir[1] = 0.000244140625f * (float)axis[1];
        dir[2] = 0.000244140625f * (float)axis[2];
        along = (dir[2] * (float)rel[2] + dir[0] * (float)rel[0] + dir[1] * (float)rel[1]) /
                (dir[2] * dir[2] + dir[0] * dir[0] + dir[1] * dir[1]);
        out[0] = (dir[0] * along + (float)t->x) - a->fx;
        out[1] = (dir[1] * along + (float)t->y) - a->fy;
        out[2] = (dir[2] * along + (float)t->z) - a->fz;
        len = fn_800E1020(out);
        strength = 0.0f;
        if (1.1920928955078125e-07f < len) {
            fn_800E1098(0.0010000000474974513f * a->f2AC * a->dt, out, out);
            ConditionSetBit(a->st, 0x40, 1);
            strength = 0.019999999552965164f * len;
        }
    }
    return strength;
}

// 0x8028F484: animation-event callback (states 0, 1, 4, 5).
int HostDriveAnimEvent(int key, Actor* a) {
    Beh* b;
    void* self;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    self = a;
    switch (key) {
    case 0x1B83833F:
        lbl_803C2AA8[b->state].leave(a, b->next);
        lbl_803C2AA8[b->next].enter(a);
        break;
    case 0x239807:
    case 0x4A5C9FC:
    case 0x46341C3A:
        fn_800CB8BC((Actor*)self, ((Actor*)self)->type, b->unk10, ((Actor*)self)->i26C);
        break;
    }
    return 1;
}

// 0x8028F580: animation-event callback (states 2, 3).
int HostDriveAnimEventB(int key, Actor* a) {
    Beh* b;

    if (a == 0) {
        return 0;
    }
    b = a->beh;
    switch (key) {
    case 0x1B83833F:
        lbl_803C2AA8[b->state].leave(a, b->next);
        lbl_803C2AA8[b->next].enter(a);
        if (fn_8016325C(a->animState) == 0) {
            break;
        }
        fn_80162610(a, a->animState, 0x1B83833F);
        break;
    case 0x20EE36:
    case 0xF829DB93:
        if (fn_8016325C(a->animState) == 4) {
            fn_80162610(a, a->animState, 0x20EE36);
            break;
        }
        if (fn_8016325C(a->animState) != 5) {
            break;
        }
        fn_80162610(a, a->animState, 0xF829DB93);
        break;
    case 0x239807:
    case 0x4A5C9FC:
    case 0x46341C3A:
        fn_800CB8BC(a, a->type, b->unk10, a->i26C);
        break;
    }
    return 1;
}

// 0x8028F714: leave a state.
int HostDriveLeave(Actor* a, int next) {
    a->beh->next = next;
    ConditionSetBit(a->st, 0, 0);
    return 1;
}

// 0x8028F74C: stay in the current state.
int HostDriveStay(Actor* a) {
    return a->beh->state;
}

// 0x8028F758: update for the idle-in-place state.
int HostDriveIdle(Actor* a) {
    Beh* b = a->beh;

    if (a->i5F0 == 0 || fn_80212730(a->id) != -1) {
        HostDriveAnimate(a);
    }
    return b->state;
}

// 0x8028F7B4: update for the walking states (turn left variant).
int HostDriveWalkA(Actor* a) {
    int target;
    Beh* b;

    b = a->beh;
    target = (a->i288 != 0) ? 1 : 0;
    HostDriveAnimate(a);
    if (fn_8016325C(a->animState) == 6) {
        fn_80162610(a, a->animState, 0xE6B71BCC);
        fn_80162610(a, a->animState, 0x1B83833F);
    } else if (fn_8016325C(a->animState) == 7) {
        fn_80162610(a, a->animState, 0x968CBE3D);
        fn_80162610(a, a->animState, 0x1B83833F);
    }
    if (b->flag != target) {
        return target + 2;
    }
    if (fn_8016325C(a->animState) == 4 || fn_8016325C(a->animState) == 5) {
        return b->state;
    }
    return target;
}

// 0x8028F8B8: update for the walking states (turn right variant).
int HostDriveWalkB(Actor* a) {
    int target;
    Beh* b;

    b = a->beh;
    target = (a->i288 != 0) ? 1 : 0;
    HostDriveAnimate(a);
    if (fn_8016325C(a->animState) == 6) {
        fn_80162610(a, a->animState, 0xE6B71BCC);
        fn_80162610(a, a->animState, 0x1B83833F);
    } else if (fn_8016325C(a->animState) == 7) {
        fn_80162610(a, a->animState, 0x968CBE3D);
        fn_80162610(a, a->animState, 0x1B83833F);
    }
    if (b->flag != target) {
        return target;
    }
    if (fn_8016325C(a->animState) == 4 || fn_8016325C(a->animState) == 5) {
        return target + 2;
    }
    return b->state;
}

// 0x8028F9BC: enter state 5.
int HostDriveEnter5(Actor* a) {
    Beh* b = a->beh;

    a->f10C = (void*)fn_800A7068;
    b->state = 5;
    b->next = 5;
    b->flag = 0;
    if (a->i5F0 == 0 || fn_80212730(a->id) != -1) {
        fn_8008DE4C(a->anim, 0, b->anim, 0x18);
    }
    if (HasCondition(a->st, 10) == 0) {
        ConditionSetBit(a->st, 10, 1);
        fn_800C5654(a->id, 0xDC04E3D5, a->id);
    }
    return 1;
}

// 0x8028FA7C: enter state 4.
int HostDriveEnter4(Actor* a) {
    Beh* b = a->beh;

    b->state = 4;
    b->next = 5;
    b->flag = 0;
    a->state220 = 0;
    ConditionSetBit(a->st, 0, 1);
    ConditionSetBit(a->st, 9, 1);
    if (a->i5F0 == 0 || fn_80212730(a->id) != -1) {
        HostDriveSetAnim(a);
    }
    return 1;
}

// 0x8028FB0C: enter state 3.
int HostDriveEnter3(Actor* a) {
    Beh* b = a->beh;

    b->state = 3;
    b->next = 3;
    b->flag = 1;
    HostDriveSetAnim(a);
    return 1;
}

// 0x8028FB48: enter state 2.
int HostDriveEnter2(Actor* a) {
    Beh* b = a->beh;

    b->state = 2;
    b->next = 2;
    b->flag = 0;
    HostDriveSetAnim(a);
    return 1;
}

// 0x8028FB84: enter state 1.
int HostDriveEnter1(Actor* a) {
    Beh* b = a->beh;

    b->state = 1;
    b->next = 1;
    b->flag = 1;
    HostDriveSetAnim(a);
    return 1;
}

// 0x8028FBBC: enter state 0.
int HostDriveEnter0(Actor* a) {
    Beh* b = a->beh;

    ConditionSetBit(a->st, 0, 1);
    b->state = 0;
    b->next = 0;
    b->flag = 0;
    HostDriveSetAnim(a);
    return 1;
}

static inline float ClampF(float v, float lo, float hi) {
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

// Forward lean/spin response shared by the two movement branches.
static inline void HostDriveLean(Actor* a) {
    float steer = a->steer;
    float t = ClampF((__fabsf(steer) - (float)lbl_804D37F8) / (float)(lbl_804D37FC - lbl_804D37F8), 0.0f, 1.0f);
    float rate = t * (a->type->topSpeed - (float)lbl_804D37F4) + (float)lbl_804D37F4;

    if (steer > 1.1920928955078125e-07f) {
        float d = (rate * a->dt) / 1000.0f;

        a->spin += (d < steer) ? d : steer;
    } else if (steer < -1.1920928955078125e-07f) {
        float d = -((rate * a->dt) / 1000.0f);

        a->spin += (steer < d) ? d : steer;
    }
    a->lean = ClampF(a->lean, -a->type->maxLean, a->type->maxLean);
    if (1.1920928955078125e-07f < __fabsf(steer)) {
        a->f258 = a->f2B0;
    }
}

// 0x8028FC1C: advance the animation and move the actor by its root motion.
void HostDriveAnimate(Actor* a) {
    Beh* b = a->beh;
    float root[3];
    Mtx34 mtx;
    Mtx34 rot;
    float euler[3];
    float local[3];

    if (a->node != 0) {
        fn_800913EC(a->dt, a->anim, root);
        if (a->s922 == a->s920 - 1) {
            fn_8008F630(a->anim, a->node, ModelAnimate(a->model, (char*)a->node, 1), a->unkD0);
        }
        if (HasCondition(a->st, 10) == 0 && HasCondition(a->st, 9) == 0) {
            int k1 = ModelFindBone(a->model, AssetFind(0xF1754BF2, 0x922BF04E));
            int k2 = ModelFindBone(a->model, AssetFind(0xF1754BF2, 0x21C5E0));

            BoneAddRotX(a->node, k2, a->f258);
            SplineQuatRotateRamp(a->lean, a->node, k1, k2, a->type->id);
        }
        if (HasCondition(a->st, 0xD) == 0 && HasCondition(a->st, 0xC) == 0) {
            float h = fn_800CC308(a, 200, 0, &b->unk10, &b->unk14);

            if (a->s922 == a->s920 - 1 && h != -999999.0f) {
                a->fz = h + a->node->z;
            }
            a->z = (int)a->fz;
            a->node->x = 0.0f;
            a->node->y = 0.0f;
            a->node->z = 0.0f;
            HostDriveLean(a);
            if (HasCondition(a->st, 0) != 0) {
                b->vel[0] += root[0];
                b->vel[1] += root[1];
                b->vel[2] += root[2];
                ActorGetMatrix(&mtx, a);
                fn_800A0168(&mtx, &mtx);
                fn_800A0030(root, &mtx, root);
                a->vx = root[0];
                a->vy = root[1];
                a->vz = root[2];
                a->vz = 0.0f;
                {
                    float inv = 1000.0f / a->dt;

                    a->vx *= inv;
                    a->vy *= inv;
                    a->vz *= inv;
                }
            }
            {
                float speed = fn_800E1020(&a->vx);

                fn_8016D4EC(&a->vx, &a->lean);
                fn_800E1098(speed, &a->vx, &a->vx);
                EulerNormalize(&a->lean, &a->lean);
            }
            if (b->state == 1) {
                float tilt[3];
                float diff;
                int side;

                fn_8016D440(tilt, a->st);
                fn_8016D614(tilt, tilt, &a->lean);
                EulerNormalize(tilt, tilt);
                diff = tilt[1];
                if (b->lean != diff) {
                    float rate;
                    float d;

                    if (__fabsf(b->lean) <= __fabsf(diff)) {
                        rate = (float)lbl_804D37EC * a->dt;
                    } else {
                        rate = (float)lbl_804D37F0 * a->dt;
                    }
                    d = __fabsf(diff - b->lean);
                    if (5.0f * rate < d) {
                        d = 5.0f * rate;
                    }
                    b->lean += d * ((diff - b->lean < 0.0f) ? -1.0f : 1.0f);
                }
                side = (b->lean < 0.0f) ? 2 : 1;
                if (side != b->leanSide) {
                    int anim;

                    b->leanSide = side;
                    anim = fn_800A1BC8(a->type->boneA, a->type->boneB, lbl_803C2AA8[b->state].key, side, 0);
                    fn_8008F230(0.0f, a->anim, 1, 0);
                    fn_80090974(1.0f, a->anim, 1, anim, 1, 0, 0);
                }
                fn_8008F4F0(ClampF(__fabsf(b->lean) / 30.0f, 0.0f, 1.0f), a->anim, 1);
            }
            Vec3Set(euler, a->spin, 0.0f, 0.0f);
            EulerNormalize(euler, euler);
            MtxSetEuler(&rot, euler);
            local[0] = (float)a->x;
            local[1] = (float)a->y;
            local[2] = (float)a->z;
            PlacementToMatrix(&mtx, a);
            ActorGetMatrix(&mtx, a);
            fn_800A0168(&mtx, &mtx);
            root[1] = 0.0f;
            root[2] = 0.0f;
            fn_800A0030(root, &mtx, root);
            ModelSetMatrix(a->model, &rot, a->node);
        } else if (HasCondition(a->st, 0xC) == 0) {
            float drive[3];
            Mtx34 m2;
            Mtx34 m3;
            float rel[3];
            float fwd[3];

            a->lean = 0.0f;
            if (a->s922 == a->s920 - 1) {
                float h = fn_800CC308(a, 200, 0, &b->unk10, &b->unk14);

                if (h != -999999.0f) {
                    a->fz = h + a->node->z;
                    a->node->z = 0.0f;
                }
            }
            HostDriveLean(a);
            a->z = (int)a->fz;
            ActorGetMatrix(&m2, a);
            fn_800A0168(&m2, &m2);
            fn_800A0030(root, &m2, root);
            if (a->type->id == 0x40D2FC38 && a->state220 == 2 && a->target != 0) {
                Actor* t = a->target;
                float s0;
                float s1;

                ActorGetMatrix(&m3, t);
                rel[0] = a->fx - (float)t->x;
                rel[1] = a->fy - (float)t->y;
                rel[2] = a->fz - (float)t->z;
                fn_800A0030(rel, &m3, rel);
                fn_800A0030(fwd, &m3, root);
                s0 = (rel[0] < 0.0f) ? -1.0f : 1.0f;
                s1 = (fwd[0] < 0.0f) ? -1.0f : 1.0f;
                if (s1 == s0) {
                    fwd[0] = 0.0f;
                    fn_800A0168(&m3, &m3);
                    fn_800A0030(root, &m3, fwd);
                }
            }
            a->fx += root[0];
            a->fy += root[1];
            a->fz += root[2];
            a->x = (int)a->fx;
            a->y = (int)a->fy;
            a->z = (int)a->fz;
            a->node->x = 0.0f;
            a->node->y = 0.0f;
            a->node->z = 0.0f;
            Vec3Set(drive, a->spin, 0.0f, 0.0f);
            EulerNormalize(drive, drive);
            MtxSetEuler(&rot, drive);
            local[0] = (float)a->x;
            local[1] = (float)a->y;
            local[2] = (float)a->z;
            PlacementToMatrix(&rot, a);
            ModelSetMatrix(a->model, &rot, a->node);
        } else {
            if (a->s922 == a->s920 - 1) {
                fn_800CC308(a, 200, 1, &b->unk10, &b->unk14);
            }
            if (fn_8008EA9C(a->anim, 0) != 0) {
                if (HasCondition(a->st, 0xC) == 0) {
                    float e[3];

                    Vec3Set(e, a->spin, 0.0f, 0.0f);
                    EulerNormalize(e, e);
                    MtxSetEuler(&rot, e);
                    PlacementToMatrix(&rot, a);
                } else {
                    fn_800A0EF0(&rot, a->node);
                    PlacementToMatrix(&rot, a);
                    a->fx = a->node->x;
                    a->fy = a->node->y;
                    a->fz = a->node->z;
                    a->x = (int)a->node->x;
                    a->y = (int)a->node->y;
                    a->z = (int)a->node->z;
                }
            }
            ModelSetMatrix(a->model, 0, a->node);
        }
    }
}

// 0x80290930: switch the model to the current state's animation.
void HostDriveSetAnim(Actor* a) {
    Beh* b;
    int side;
    int anim;
    int anim2;
    float blend;

    if (a == 0) {
        return;
    }
    b = a->beh;
    b->vel[0] = 0.0f;
    b->vel[1] = 0.0f;
    b->vel[2] = 0.0f;
    if (HasCondition(a->st, 0xC) != 0) {
        ConditionSetBit(a->st, 0xC, 0);
        ConditionSetBit(a->st, 0xE, 1);
        fn_8008F4E8(a->anim, 0);
        fn_8008F378(a->anim);
        fn_8008F4EC(a->anim, 0);
    }
    side = (b->lean < 0.0f) ? 2 : 1;
    blend = ClampF(__fabsf(b->lean) / 30.0f, 0.0f, 1.0f);
    anim = fn_800A1BC8(a->type->boneA, a->type->boneB, lbl_803C2AA8[b->state].key, 0, 0);
    anim2 = fn_800A1BC8(a->type->boneA, a->type->boneB, lbl_803C2AA8[b->state].key, side, 0);
    if (fn_8008E7D0(anim) != 0) {
        ConditionSetBit(a->st, 0x1C, 0);
        if (HasCondition(a->st, 0xE) == 0 && anim != b->anim) {
            fn_8008F230(150.0f, a->anim, 0, 1);
        } else {
            fn_8008F230(0.0f, a->anim, 0, 0);
            ConditionSetBit(a->st, 0xE, 0);
        }
        fn_80090974(1.0f, a->anim, 0, anim, 9, (void*)lbl_803C2AA8[b->state].onAnim, a);
        fn_8008F230(0.0f, a->anim, 1, 0);
        fn_80090974(1.0f, a->anim, 1, anim2, 1, 0, 0);
        fn_8008F4F0(blend, a->anim, 1);
        b->anim = anim;
        b->leanSide = side;
    }
}

// 0x80290B80 (HostDriveTick; kept as fn_80290B80 because u_80290C58 calls it by that name): per-frame tick of the owner (animation actors and slot updates).
extern "C" int fn_80290B80(Actor* a) {
    if (a == 0) {
        return 0;
    }
    if (MemFlagIsSet(a->st, 5) == 0) {
        fn_800B3164(a, 0, 1);
        fn_800CECCC(a);
        fn_800B2BFC(a);
    } else {
        fn_800B1834(a);
    }
    if (a->i908 != 0 && HasCondition(a->st, 0xB) == 0 && HasCondition(a->st, 9) == 0 &&
        HasCondition(a->st, 10) == 0) {
        fn_800B42E0(101.0f, &a->fx, a->id, 3, 0x12);
    }
    return 1;
}

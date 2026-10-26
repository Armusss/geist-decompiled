// u_801AADA8.cpp: the "FSHSVE" actor: message handler, update, follow states and visuals (0x801AADA8-0x801AE50C).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef long s32;

struct Vec3 {
    float x;                     // 0x00
    float y;                     // 0x04
    float z;                     // 0x08
};

struct ZoneList {
    u8 unk00[0x38];              // 0x00
};

// Follow target as seen by the aim code: three integer coordinates at 0x24.
struct FollowTarget {
    u8 unk00[0x24];              // 0x00
    s32 unk24;                   // 0x24
    s32 unk28;                   // 0x28
    s32 unk2C;                   // 0x2C
    u8 unk30[0x38 - 0x30];       // 0x30
    s32 unk38;                   // 0x38
};

class Owner {
public:
    u8 unk00[0x24];              // 0x00
};

// State block hung off Actor::unk130.  The lazily hashed type name is "[FSHSVE]".
struct FSHSVE {
    s32 unk00;                   // 0x00 state index
    float unk04;                 // 0x04
    s32 unk08;                   // 0x08
    float unk0C;                 // 0x0C
    float unk10;                 // 0x10
    float unk14;                 // 0x14
    float unk18;                 // 0x18
    float unk1C;                 // 0x1C
    s32 unk20;                   // 0x20
    u32 unk24;                   // 0x24 flags
    float unk28;                 // 0x28
    float unk2C;                 // 0x2C blend
    s32 unk30;                   // 0x30
    s32 unk34;                   // 0x34
    float unk38;                 // 0x38
    float unk3C;                 // 0x3C
};

class Actor : public Owner {
public:
    u8 unk24[0x38 - 0x24];       // 0x24
    s32 unk38;                   // 0x38 object id
    u8 unk3C[0x84 - 0x3C];       // 0x3C
    Vec3 unk84;                  // 0x84 position
    u8 unk90[0xC4 - 0x90];       // 0x90
    void* unkC4;                 // 0xC4
    u8 unkC8[0xCC - 0xC8];       // 0xC8
    int unkCC;                   // 0xCC model handle
    int unkD0;                   // 0xD0
    u8 unkD4[0xE8 - 0xD4];       // 0xD4
    void* unkE8;                 // 0xE8
    u8 unkEC[0xF0 - 0xEC];       // 0xEC
    int unkF0;                   // 0xF0
    u8 unkF4[0xFC - 0xF4];       // 0xF4
    void (*unkFC)();             // 0xFC behaviour callback
    u8 unk100[0x104 - 0x100];    // 0x100
    void (*unk104)();            // 0x104 destroy callback
    u8 unk108[0x10C - 0x108];    // 0x108
    void (*unk10C)(void*);       // 0x10C update callback
    u8 unk110[0x130 - 0x110];    // 0x110
    FSHSVE* unk130;              // 0x130
    u8 unk134[4];                // 0x134
    float unk138;                // 0x138
    float unk13C;                // 0x13C
    u8 unk140[0x144 - 0x140];    // 0x140
    u32 unk144;                  // 0x144 follow slot 0 flags
    u8 unk148[0x158 - 0x148];    // 0x148
    u32 unk158;                  // 0x158 follow slot 0 condition flags
    u8 unk15C[0x16C - 0x15C];    // 0x15C
    u32 unk16C;                  // 0x16C follow slot 1 flags
    u8 unk170[0x180 - 0x170];    // 0x170
    u32 unk180;                  // 0x180 follow slot 1 condition flags
    u8 unk184[0x194 - 0x184];    // 0x184
    u32 unk194;                  // 0x194 follow slot 2 flags
    u8 unk198[0x1A8 - 0x198];    // 0x198
    u32 unk1A8;                  // 0x1A8 follow slot 2 condition flags
    u8 unk1AC[0x1BC - 0x1AC];    // 0x1AC
    u32 unk1BC;                  // 0x1BC follow slot 3 flags
    u8 unk1C0[0x1D0 - 0x1C0];    // 0x1C0
    u32 unk1D0;                  // 0x1D0 follow slot 3 condition flags
    u8 unk1D4[0x21C - 0x1D4];    // 0x1D4
    s32 unk21C;                  // 0x21C queued state
    s32 unk220;                  // 0x220 follow mode
    u8 unk224[0x22C - 0x224];    // 0x224
    s32 unk22C;                  // 0x22C follow target id
    u8 unk230[0x268 - 0x230];    // 0x230
    s32 unk268;                  // 0x268
    s32 unk26C;                  // 0x26C
    u8 unk270[0x288 - 0x270];    // 0x270
    s32 unk288;                  // 0x288
    u8 unk28C[0x2B4 - 0x28C];    // 0x28C
    Vec3 unk2B4;                 // 0x2B4 follow probe point
    u8 unk2C0[0x2C4 - 0x2C0];    // 0x2C0
    s32 unk2C4;                  // 0x2C4
    u8 unk2C8[0x604 - 0x2C8];    // 0x2C8
    void (*unk604)();            // 0x604 message callback
    u8 unk608[0x658 - 0x608];    // 0x608
    ZoneList unk658;             // 0x658
    u8 unk690[0x894 - 0x690];    // 0x690
    u8 unk894[0x8B2 - 0x894];    // 0x894
    u16 unk8B2;                  // 0x8B2
    u16 unk8B4;                  // 0x8B4
    u8 unk8B6[0x8F8 - 0x8B6];    // 0x8B6
    int unk8F8;                  // 0x8F8
    float unk8FC;                // 0x8FC
    float unk900;                // 0x900
    float unk904;                // 0x904
    u8 unk908[0x91C - 0x908];    // 0x908
    float unk91C;                // 0x91C
};

class Buffer {
public:
    virtual ~Buffer();
};

class String : public Buffer {
public:
    String(const char* str);
    String(const String& a, const String& b);
    u32 unk04;                   // 0x04
    u32 unk08;                   // 0x08
    u8 unk0C[8];                 // 0x0C
};

// Per-state descriptor table at lbl_803AAD30, 16 bytes per entry.
struct StateEntry {
    int (*unk00)(Actor*);        // 0x00
    int (*unk04)(Actor*);        // 0x04
    int (*unk08)(Actor*);        // 0x08
    const char* unk0C;           // 0x0C
};

// Shared name/data block at lbl_80348490 (type name plus assorted literals).
struct TypeNameData {
    u8 unk00[0x48];              // 0x00
    char name[9];                // 0x48 "[FSHSVE]"
    char newline[2];             // 0x51 "\n"
    char standAttack[13];        // 0x53 "Stand Attack\n"
    char dashin[7];              // 0x61 "dashin"
    char dashout[8];             // 0x68 "dashout"
    char shMove[8];              // 0x70 "SH_Move"
    char query[6];               // 0x78 "QUERY"
};

extern StateEntry lbl_803AAD30[];
extern TypeNameData lbl_80348490;
extern unsigned char __upper_map[];

extern float lbl_804D30C8;
extern float lbl_804D30CC;
extern float lbl_804D30D0;
extern int lbl_804D30D4;
extern int lbl_804D30D8;
extern int lbl_804D30E0;
extern int lbl_804D30E4;
extern int lbl_804D30E8;
extern int lbl_804D30EC;
extern int lbl_804D30F0;
extern float lbl_804D30F4;
extern float lbl_804D30F8;
extern int lbl_804D30B8;
extern int lbl_804D30BC;
extern int lbl_804D30C0;
extern int lbl_804D30C4;
extern int lbl_804D30DC;
extern float lbl_804D3154;
extern float lbl_804D8810;
extern float lbl_804D8814;
extern float lbl_804D8818;
extern float lbl_804D881C;
extern float lbl_804D8828;
extern float lbl_804D882C;

extern int OwnerId(Owner* owner);
extern int ActorPlayAnim(int owner, u32 anim, int arg);
extern void* FollowGetTargetObj(void* follow);
extern void FollowSetTargetObj(void* follow, void* target);
extern int HasCondition(void* cond, int bit);
extern void ConditionSetBit(void* cond, int bit, int value);

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void* memcpy(void* dst, const void* src, u32 size);
extern "C" int rand(void);
extern "C" int fn_80049940(void);
extern "C" int fn_80019F88(int* args, Actor* actor);
extern "C" int fn_801625C8(Actor* actor, void* data, int msg, int* args);
extern "C" void fn_80162658(Actor* actor, void* data);
extern "C" void fn_8029B070(Actor* actor, u32 msg);
extern "C" int fn_8016F120(int id, void* handle);
extern "C" void fn_8016EEF4(int handle);
extern "C" void fn_8016EDD8(int handle, void* handle2);
extern "C" int fn_800BEF9C(int id);
extern "C" int fn_800BAF40(Actor* actor);
extern "C" void fn_800C99E8(Actor* actor, float radius, int a, int b, int c, int d);
extern "C" float fn_802A2080(void);
extern "C" void fn_8028C188(void* dst, u32 a, u32 b);
extern "C" void fn_8028C258(void* dst, const char* str);
int FSHSVEVisualUpdate(int* args);
int FSHSVETargetFollowCheck(int* args);
int FSHSVEPickNextState(Actor* actor);
extern "C" void fn_800C9B68(void);
extern "C" void fn_800C9C68(Actor* actor);
extern "C" void fn_801AB4BC(void);
int FSHSVEFrameUpdate(Actor* actor);
int FSHSVEZoneNodeProbe(int node, Owner* owner, int* out);
int FSHSVETargetBlocksFollow(Actor* actor, u32 targetId);

// Shared runtime helpers (C++ mangled names in the retail binary).
void* VAlloc(int size);
void VFree(void* p);
void ConditionUpdateBit(void* cond, int bit, int value);
u32 AssetFind(u32 asset, u32 kind);
int FxFind(const char* name);

// Lazy type-name hash: uppercase each character through __upper_map, 31 per step.
static inline u32 NameHash(const char* name)
{
    u32 hash = 0;
    if (name) {
        const char* p = name;
        const char* end = p;
        while (*end != 0) {
            int c = *p;
            u32 v;
            if (c == -1) {
                v = 0xffffffff;
            } else {
                v = __upper_map[(u8)c];
            }
            hash *= 31;
            hash += v & 0xff;
            p++;
            end++;
        }
        hash %= 0xffffffff;
    }
    return hash;
}

// Name is a descriptive guess: message handler for the "[FSHSVE]" actor state block.
int HandleFSHSVEMessages(int msg, int* args)
{
    Actor* actor = (Actor*)args[0];

    switch (msg) {
    case 0:
        return FSHSVEVisualUpdate(args);

    case 0x400:
        args[1] = OwnerId(actor);
        args[3] = actor->unk904 > lbl_804D8810;
        ((float*)args)[4] = lbl_804D8814;
        ((float*)args)[5] = lbl_804D8814;
        args[6] = 3 - !fn_80049940();
        args[7] = 0;
        return 1;

    case 0x407: {
        FSHSVE* st = actor->unk130;
        switch (args[5]) {
        case 0x2efc66ca:
            st->unk2C = lbl_804D8810;
            actor->unk91C = lbl_804D8814;
            return 0;
        case -0x4603bd0b:
            ((float*)args)[3] *= lbl_804D30F4;
            return 0;
        }
        return 0;
    }

    case 0x409: {
        if (ActorPlayAnim(OwnerId(actor), (u32)args[3], args[2]) != 0) {
            return 1;
        }
        if (args[3] != 0x272cac) {
            return 0;
        }
        int anim = ActorPlayAnim(actor->unk38, 0xdc35206a, args[2]);
        if (args[2] != -1) {
            int info[4];
            info[1] = anim;
            info[2] = actor->unk38;
            info[3] = 0xaf699ed0;
            SendMsg(args[2], 0xc14, info);
        }
        return 1;
    }

    case 0x40a:
        return FSHSVETargetFollowCheck(args);

    case 0x41d: {
        FSHSVE* st = actor->unk130;
        if (fn_80019F88(args, actor) != 0) {
            return 1;
        }
        switch (args[1]) {
        case 0x356c05f2:
            if (args[3] != 0) {
                st->unk24 |= 4;
            } else {
                st->unk24 &= ~4;
            }
            return 1;
        case -0x70627f32:
            if (args[3] != 0) {
                st->unk24 |= 8;
                st->unk38 = lbl_804D30F8;
            } else {
                st->unk24 &= ~8;
                st->unk38 = lbl_804D3154;
            }
            return 0;
        }
        return 0;
    }

    case 0x41e: {
        FSHSVE* st = actor->unk130;
        switch (args[1]) {
        case -0x7cd20af6:
            args[3] = lbl_804D30B8;
            return 1;
        case -0x527a8906: {
            {
                String strName(lbl_803AAD30[st->unk00].unk0C);
                String strNewline(lbl_80348490.newline);
                String text(strName, strNewline);
                fn_8028C188(&args[3], text.unk04, text.unk08);
            }
            if (st->unk24 & 4) {
                fn_8028C258(&args[3], lbl_80348490.standAttack);
            }
            return 0;
        }
        case 0x2eaa5f62:
            args[3] = actor->unk91C > lbl_804D8814;
            return 1;
        case 0x356c05f2:
            args[3] = (st->unk24 & 4) != 0;
            return 1;
        }
        return 0;
    }

    case 0x7102: {
        FSHSVE* st = actor->unk130;
        if (st != 0) {
            if (args[1] < 0x44) {
                args[4] = 0;
                return 1;
            }
            const char* name = lbl_80348490.name;
            u32 hash = NameHash(name);
            memcpy((void*)args[2], &hash, 4);
            args[3] += 4;
            args[1] -= 4;
            args[2] += 4;
            memcpy((void*)args[2], st, 0x40);
            args[3] += 0x40;
            args[1] -= 0x40;
            args[2] += 0x40;
        }
        return 1;
    }

    case 0x7103: {
        FSHSVE* st = actor->unk130;
        if (st != 0) {
            if (args[3] < 0x44) {
                args[4] = 0;
                return 1;
            }
            bool match;
            if (args[3] < 4) {
                match = false;
            } else {
                u32* p = (u32*)args[2];
                args[3] += 4;
                args[1] -= 4;
                args[2] += 4;
                const char* name = lbl_80348490.name;
                u32 hash = NameHash(name);
                match = *p == hash;
            }
            if (!match) {
                return 1;
            }
            memcpy(st, (void*)args[2], 0x40);
            args[3] -= 0x40;
            args[2] += 0x40;
        }
        return 1;
    }

    case 0xc14: {
        FSHSVE* st = actor->unk130;
        if (args[3] != 0x2efc66ca) {
            return 0;
        }
        st->unk14 = lbl_804D8810;
        actor->unk91C = lbl_804D8814;
        return 0;
    }
    }
    return fn_801625C8(actor, actor->unk894, msg, args) != 0;
}

// Name is a descriptive guess: per-frame update of the "[FSHSVE]" actor state block.
int UpdateFSHSVE(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }

    FSHSVE* st = actor->unk130;
    fn_800C99E8(actor, lbl_804D8818, 0x4b, 0x96, 0, 2);

    int info[7];
    Actor* target = (Actor*)FollowGetTargetObj(actor->unk270);
    if (target != 0) {
        SendMsg(target->unk38, 0x400, info);
    } else {
        info[6] = 2;
    }

    float delta = actor->unk138 / actor->unk91C;
    delta = delta / fn_802A2080();

    if (st->unk14 < lbl_804D30D0) {
        st->unk2C = lbl_804D8810;
        st->unk14 += delta;
    }

    if (st->unk1C > lbl_804D8810) {
        st->unk1C -= delta;
        if (info[6] & 2) {
            if (st->unk1C > lbl_804D8810) {
                st->unk2C = lbl_804D8810;
            } else {
                st->unk2C = lbl_804D8814;
            }
        }
    }

    float ratio;
    float fadeTime;
    float riseTime;
    if (fn_802A2080() < lbl_804D8814) {
        ratio = (actor->unk91C - lbl_804D8814) / (lbl_804D30C8 - lbl_804D8814);
        if (st->unk20 == 0) {
            st->unk28 = (float)(lbl_804D30E4 - lbl_804D30E0) *
                            ((float)rand() / lbl_804D881C) +
                        (float)lbl_804D30E0;
        }
        fadeTime = st->unk28;
        riseTime = (float)lbl_804D30E8;
    } else {
        ratio = (actor->unk91C - lbl_804D8814) / (lbl_804D30CC - lbl_804D8814);
        fadeTime = (float)lbl_804D30D4;
        riseTime = (float)lbl_804D30D8;
    }

    if (st->unk24 & 2) {
        st->unk2C -= delta / fadeTime;
        if (st->unk2C <= lbl_804D8810) {
            FSHSVE* other = actor->unk130;
            fn_8029B070(actor, 0x3c72b83);
            if (other->unk24 & 2) {
                other->unk24 &= ~2;
                other->unk30 = fn_8016F120(lbl_804D30C0, actor->unk24);
                if (other->unk34 != -1) {
                    fn_8016EEF4(other->unk34);
                    other->unk34 = -1;
                }
                other->unk38 = lbl_804D3154;
            }
        }
    } else {
        if (st->unk2C < lbl_804D8814) {
            st->unk2C += delta / riseTime;
            if (st->unk2C >= lbl_804D8814) {
                st->unk2C = lbl_804D8814;
            }
        }
    }

    if (st->unk24 & 2) {
        if (ratio < lbl_804D8814) {
            ratio += delta / (float)lbl_804D30EC;
            if (ratio >= lbl_804D8814) {
                ratio = lbl_804D8814;
            }
        }
    } else {
        if (ratio > lbl_804D8810) {
            ratio -= delta / (float)lbl_804D30F0;
            if (ratio <= lbl_804D8810) {
                ratio = lbl_804D8810;
            }
        }
    }

    float blend;
    if (ratio < lbl_804D8810) {
        blend = lbl_804D8810;
    } else if (ratio > lbl_804D8814) {
        blend = lbl_804D8814;
    } else {
        blend = ratio;
    }

    if (fn_802A2080() < lbl_804D8814) {
        actor->unk91C = blend * (lbl_804D30C8 - lbl_804D8814) + lbl_804D8814;
    } else {
        actor->unk91C = blend * (lbl_804D30CC - lbl_804D8814) + lbl_804D8814;
    }

    if (st->unk38 > lbl_804D8810) {
        st->unk38 -= delta;
    }
    st->unk3C += actor->unk138;
    if (st->unk34 != -1) {
        fn_8016EDD8(st->unk34, actor->unk24);
    }

    if (HasCondition(actor->unk270, 10) || HasCondition(actor->unk270, 11)) {
        fn_80162658(actor, actor->unk894);
        FSHSVE* other = actor->unk130;
        fn_8029B070(actor, 0x3c72b83);
        if (other->unk24 & 2) {
            other->unk24 &= ~2;
            other->unk30 = fn_8016F120(lbl_804D30C0, actor->unk24);
            if (other->unk34 != -1) {
                fn_8016EEF4(other->unk34);
                other->unk34 = -1;
            }
            other->unk38 = lbl_804D3154;
        }
        return 1;
    }

    int count = fn_800BEF9C(actor->unk2C4);
    if (actor->unk268 > count) {
        actor->unk268 = fn_800BEF9C(actor->unk2C4);
    }
    if (fn_800BAF40(actor) == -1) {
        actor->unk288 = 0;
        actor->unk26C = 0;
    }

    int result = lbl_803AAD30[st->unk00].unk08(actor);
    fn_80162658(actor, actor->unk894);
    if (result != 0) {
        int newState = FSHSVEPickNextState(actor);
        if (newState != st->unk00 && lbl_803AAD30[st->unk00].unk04(actor) != 0) {
            lbl_803AAD30[newState].unk00(actor);
            ConditionSetBit(actor->unk270, 0x40, 0);
            ConditionSetBit(actor->unk270, 0x41, 0);
            ConditionSetBit(actor->unk270, 0x43, 0);
            ConditionSetBit(actor->unk270, 0x42, 0);
        }
    }

    if (info[6] & 2) {
        st->unk20 = 0;
    } else {
        st->unk20 = 1;
    }
    return 1;
}

// Name is a descriptive guess: tears down the "[FSHSVE]" actor state block.
void FSHSVEBlockFree(Actor* actor)
{
    fn_800C9B68();
    VFree(actor->unk130);
    actor->unk130 = 0;
    actor->unk104 = 0;
}

// Name is a descriptive guess: allocates and initialises the "[FSHSVE]"
// actor state block, its callbacks and the shared fx handles.
void FSHSVEBlockInit(Actor* actor)
{
    FSHSVE* st;
    u32 result;

    actor->unk104 = fn_801AB4BC;
    st = (FSHSVE*)VAlloc(0x40);
    if (st == 0) {
        actor->unk130 = 0;
        return;
    }

    actor->unk130 = st;
    actor->unkFC = (void (*)())FSHSVEFrameUpdate;
    actor->unk604 = (void (*)())FSHSVETargetBlocksFollow;
    actor->unk91C = lbl_804D8814;

    st->unk00 = 0;
    st->unk04 = lbl_804D8810;
    st->unk08 = 0;
    st->unk0C = lbl_804D8810;
    st->unk10 = lbl_804D8810;
    st->unk24 = 0;
    st->unk2C = lbl_804D8810;
    st->unk30 = -1;
    st->unk34 = -1;
    st->unk14 = lbl_804D8828;
    st->unk1C = lbl_804D882C;
    st->unk20 = 0;
    st->unk28 = (float)lbl_804D30DC;

    ConditionUpdateBit(actor->unk270, 6, 1);
    ConditionUpdateBit(actor->unk270, 0x22, 1);

    result = AssetFind(0xF1754BF2, 0x21C5E0);
    lbl_804D30B8 = result;
    lbl_804D30BC = FxFind(lbl_80348490.dashin);
    lbl_804D30C0 = FxFind(lbl_80348490.dashout);
    lbl_804D30C4 = FxFind(lbl_80348490.shMove);

    fn_800C9C68(actor);
}

extern float lbl_804D8830;
extern float lbl_804D881C;
extern float lbl_804D8834;
extern float lbl_804D8838;
extern float lbl_804D883C;
extern float lbl_804D8840;
extern float lbl_804D8848;
extern float lbl_804D884C;
extern float lbl_804D8850;
extern float lbl_804D8854;

extern "C" int fn_800B26B8(Actor* actor);
extern "C" int fn_800B22F8(Actor* actor, int idx);
extern "C" int fn_800B1EF4(Actor* actor, int idx);
extern "C" int fn_800B261C(Actor* actor, int idx, int* out);
extern "C" u32 fn_800B1ED4(Actor* actor, int idx);
extern "C" void fn_800B1F20(Actor* actor, int idx);
extern "C" void fn_800B2418(float time, Actor* actor, int idx);
extern "C" void fn_800B1AB8(float time, Actor* actor, int idx, int flag);
extern "C" int fn_800A7AC4(void* cond, int bit);
extern "C" int fn_8029B3B0(Actor* actor, u32 msg);
extern "C" int fn_800A8CDC(float blend, Actor* actor, void* target);
extern "C" void fn_800BC56C(Actor* actor, void* target);
extern "C" void fn_800A7B08(void* cond, int bit, int value);
extern "C" void fn_800BBCA8(Actor* actor);
extern "C" void fn_800C748C(void* actor);

// Follow-state helpers used by the "[FSHSVE]" second follow update.
extern "C" int fn_800B2454(Actor* actor, int idx);
extern float OwnerValue(Owner* owner, Vec3* target);
extern int RangeIsIdle(void* follow);
extern void FollowSetTargetVec(void* follow, Vec3* target);
extern void OwnerSlotGetPosition(int owner, int slot, float* pos);
extern int FollowLineCheckSweep(void* owner, float* out);
extern int MemFlagIsSet(void* cond, int bit);
extern void RangeResetPending(void* follow);
extern float OwnerValue(Owner* owner, void* target);
extern int FollowLineCheckActorDist(void* actor, void* target, float dist);
extern float ActorAimAngleToPos(void* obj, Vec3* pos);
extern void TalkPlay(void* owner, int id);
extern void ZoneWalkNodes(ZoneList* list, u32 (*cb)(u32, float, int), int* out);
extern "C" int fn_80162940(void* handle, int flag);
extern "C" int fn_8016325C(void* handle);
extern "C" void fn_80162610(Actor* actor, void* handle, int value);
extern "C" void fn_800CCA88(Actor* actor, s32* out);
extern "C" int fn_800BF2D0(Vec3* pos);
extern "C" void fn_800E1098(Vec3* vec);
extern "C" float fn_800E1020(Vec3* vec);
extern "C" int fn_800BB218(Actor* actor, s32 id, Vec3* from, Vec3* to,
                           Actor* owner, float* outDist, int* outA, int* outB,
                           float scale);
extern "C" void fn_800BAFF8(Actor* actor, Vec3* vec, int a, int b, Vec3* other,
                            float f1, float f2);
extern "C" void fn_800BC5A8(Actor* actor, Vec3* vec);
extern "C" float fabsf(float);

// Name is a descriptive guess: follow-target update of the "[FSHSVE]" actor state.
int FSHSVEFollowUpdate(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }

    FSHSVE* st = actor->unk130;
    void* target = FollowGetTargetObj(actor->unk270);
    int kind = fn_800B26B8(actor);

    if (target == 0) {
        ConditionUpdateBit(actor->unk270, 4, 0);
        return 1;
    }

    if (kind < 2) {
        fn_800B22F8(actor, kind);
        if (fn_800B1EF4(actor, kind) == 0) {
            int curTarget;
            fn_800B261C(actor, kind, &curTarget);
            u32 flags = fn_800B1ED4(actor, kind);
            if (flags & 0x200) {
                st->unk2C = lbl_804D8814;
            }
            if (target != 0 && target != (void*)curTarget) {
                fn_800B1F20(actor, kind);
                fn_800B2418(lbl_804D8830, actor, kind);
                fn_800B1AB8(lbl_804D882C, actor, kind, 1);
            }
        }
    } else {
        int curTarget;
        if (fn_800B261C(actor, kind, &curTarget) == 0 ||
            curTarget != (int)target) {
            if (fn_800B1EF4(actor, kind) == 0) {
                return 1;
            }
        } else {
            fn_800B1F20(actor, kind);
        }
    }

    int msg[8];
    SendMsg(((Actor*)target)->unk38, 0x400, msg);

    if ((msg[6] & 2) == 0) {
        if (st->unk20 == 0 && (st->unk24 & 2) == 0) {
            st->unk2C = lbl_804D8814;
        }

        bool ready;
        if ((st->unk24 & 2) == 0 && st->unk2C == lbl_804D8814 &&
            fn_800A7AC4(actor->unk270, 9) == 0 &&
            fn_800A7AC4(actor->unk270, 10) == 0 &&
            fn_8029B3B0(actor, 0x3c72b83) != 0) {
            ready = true;
        } else {
            ready = false;
        }

        if (ready && (st->unk24 & 2) == 0 && st->unk2C == lbl_804D8814 &&
            fn_8029B3B0(actor, 0x3c72b83) != 0) {
            st->unk24 |= 2;
            st->unk30 = fn_8016F120(lbl_804D30BC, actor->unk24);
            if (st->unk34 == -1) {
                st->unk34 = fn_8016F120(lbl_804D30C4, actor->unk24);
            }
            st->unk38 = lbl_804D30F8;
        }
    }

    if (fn_800A8CDC(lbl_804D8814, actor, target) == 0) {
        if (fn_800BAF40(actor) == -1) {
            fn_800BC56C(actor, target);
            if (fn_800BAF40(actor) == -1) {
                actor->unk288 = 0;
                return 1;
            }
            actor->unk288 = 3;
        }
        return 0;
    }
    return 1;
}

// Name is a descriptive guess: aborts the "[FSHSVE]" follow state.
int FSHSVEFollowAbort(void)
{
    fn_800BBCA8(0);
    return 1;
}

// Name is a descriptive guess: selects and stores the follow target for the
// "[FSHSVE]" actor state.
int FSHSVEFollowSelectTarget(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }

    actor->unk130->unk00 = 6;
    ConditionUpdateBit(actor->unk270, 3, 1);

    int kind = fn_800B26B8(actor);
    if (kind == 4) {
        return 0;
    }

    int target;
    fn_800B261C(actor, kind, &target);
    if (target == 0) {
        return 0;
    }

    FollowSetTargetObj(actor->unk270, (void*)target);
    fn_800BC56C(actor, (void*)target);
    actor->unk288 = 3;
    return 1;
}

// Name is a descriptive guess: second per-frame follow update of the "[FSHSVE]"
// actor state (target re-acquisition, approach start, aim and talk triggers).
int FSHSVEFollowProcess(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }

    FSHSVE* st = actor->unk130;
    Actor* target = (Actor*)FollowGetTargetObj(actor->unk270);

    if (target == 0 ||
        (actor->unk604 != 0 &&
         ((int (*)(Actor*, int))actor->unk604)(actor, target->unk38) == 0)) {
        RangeResetPending(actor->unk270);
        return 1;
    }
    if (MemFlagIsSet(actor->unk270, 5) != 0) {
        return 1;
    }

    int kind = fn_800B26B8(actor);
    if (kind != 4) {
        if (fn_800B1EF4(actor, kind) == 0) {
            if (fn_800B1ED4(actor, kind) & 0x200) {
                st->unk2C = lbl_804D8814;
            }
            int curTarget;
            fn_800B261C(actor, kind, &curTarget);
            fn_800B22F8(actor, kind);
            if (curTarget != 0 && fn_800B22F8(actor, kind) < 2) {
                fn_800B2418(lbl_804D8830, actor, kind);
                fn_800B1F20(actor, kind);
                fn_800B1AB8(lbl_804D882C, actor, kind, 1);
                FollowSetTargetObj(actor->unk270, (void*)curTarget);
            }
        } else {
            if (fn_800B2454(actor, kind) != 0) {
                return 1;
            }
            fn_800B2418(lbl_804D8830, actor, kind);
        }
    }

    int msg[8];
    SendMsg(target->unk38, 0x400, msg);

    if (msg[6] & 2) {
        if (actor != 0) {
            FSHSVE* s = actor->unk130;
            s->unk04 -= actor->unk138;
            if (s->unk04 <= lbl_804D8810) {
                int found = 0;
                ZoneWalkNodes(&actor->unk658,
                              (u32 (*)(u32, float, int))FSHSVEZoneNodeProbe,
                              &found);
                if (found != 0) {
                    TalkPlay((void*)OwnerId(actor), 6);
                } else {
                    TalkPlay((void*)OwnerId(actor), 5);
                }
                s->unk04 = lbl_804D8810;
            }
        }
    }

    float dist = OwnerValue(actor, target);
    if (dist > (float)*(u32*)((u8*)actor->unkC4 + 0xC8) ||
        FollowLineCheckActorDist(actor, target, lbl_804D8834) == 0) {
        return 1;
    }

    float thrA;
    float thrB;

    if ((msg[6] & 2) == 0) {
        if (st->unk20 == 0 && (st->unk24 & 2) == 0) {
            st->unk2C = lbl_804D8814;
            if ((actor->unk130->unk24 & 2) == 0 &&
                actor->unk130->unk2C == lbl_804D8814 &&
                HasCondition(actor->unk270, 9) == 0 &&
                HasCondition(actor->unk270, 10) == 0 &&
                fn_8029B3B0(actor, 0x3c72b83) != 0) {
                FSHSVE* s = actor->unk130;
                if ((s->unk24 & 2) == 0 && s->unk2C == lbl_804D8814 &&
                    fn_8029B3B0(actor, 0x3c72b83) != 0) {
                    s->unk24 |= 2;
                    s->unk30 = fn_8016F120(lbl_804D30BC, actor->unk24);
                    if (s->unk34 == -1) {
                        s->unk34 = fn_8016F120(lbl_804D30C4, actor->unk24);
                    }
                    s->unk38 = lbl_804D30F8;
                }
            }
        }

        thrA = lbl_804D8814;
        thrB = lbl_804D882C;
        if ((actor->unk130->unk24 & 2) == 0 &&
            actor->unk130->unk2C == thrA &&
            HasCondition(actor->unk270, 9) == 0 &&
            HasCondition(actor->unk270, 10) == 0 &&
            fn_8029B3B0(actor, 0x3c72b83) != 0) {
            FSHSVE* s = actor->unk130;
            if ((s->unk24 & 2) == 0 && s->unk2C == lbl_804D8814 &&
                fn_8029B3B0(actor, 0x3c72b83) != 0) {
                s->unk24 |= 2;
                s->unk30 = fn_8016F120(lbl_804D30BC, actor->unk24);
                if (s->unk34 == -1) {
                    s->unk34 = fn_8016F120(lbl_804D30C4, actor->unk24);
                }
                s->unk38 = lbl_804D30F8;
            }
        }
    } else {
        st->unk1C = lbl_804D8838;
        thrA = lbl_804D882C;
        thrB = lbl_804D8814;
        if ((actor->unk130->unk24 & 2) == 0 &&
            actor->unk130->unk2C == thrB &&
            HasCondition(actor->unk270, 9) == 0 &&
            HasCondition(actor->unk270, 10) == 0 &&
            fn_8029B3B0(actor, 0x3c72b83) != 0) {
            FSHSVE* s = actor->unk130;
            if ((s->unk24 & 2) == 0 && s->unk2C == lbl_804D8814 &&
                fn_8029B3B0(actor, 0x3c72b83) != 0) {
                s->unk24 |= 2;
                s->unk30 = fn_8016F120(lbl_804D30BC, actor->unk24);
                if (s->unk34 == -1) {
                    s->unk34 = fn_8016F120(lbl_804D30C4, actor->unk24);
                }
                s->unk38 = lbl_804D30F8;
            }
        }
    }

    if (thrA <= lbl_804D8810) {
        fn_8029B070(actor, 0x73B1E248);
    }
    if (thrB <= lbl_804D8810) {
        fn_8029B070(actor, 0x86A69297);
    }

    st->unk18 -= actor->unk138;

    if (fn_80162940(actor->unk894, 2) != 0 &&
        fn_80162940(actor->unk894, 8) != 0 &&
        thrB >= (float)rand() / lbl_804D881C &&
        fn_8029B3B0(actor, 0x86A69297) != 0 && actor->unk288 == 0) {
        float angle = ActorAimAngleToPos(target, &actor->unk84);
        bool fire = true;
        if (fabsf(angle) >= lbl_804D883C) {
            st->unk18 -= actor->unk138;
            fire = st->unk18 <= lbl_804D8810;
        }
        if (fire && fn_8029B3B0(actor, 0x86A69297) != 0) {
            st->unk18 = lbl_804D8840;
            fn_80162610(actor, actor->unk894, 2);
        }
    } else if (fn_80162940(actor->unk894, 1) != 0 &&
               fn_80162940(actor->unk894, 4) != 0 &&
               thrA >= (float)rand() / lbl_804D881C &&
               fn_8029B3B0(actor, 0x73B1E248) != 0) {
        fn_80162610(actor, actor->unk894, 1);
    }

    if ((st->unk24 & 4) == 0 && fn_8016325C(actor->unk894) != 5) {
        if (fn_80162940(actor->unk894, 0x800) != 0) {
            ConditionUpdateBit(actor->unk270, 0x1d, 1);
            fn_800BC56C(actor, target);
            actor->unk288 = 3;
        } else if (fn_80162940(actor->unk894, 0x400) != 0) {
            FollowTarget* tgt = (FollowTarget*)target;

            Vec3 delta;
            delta.x = (float)tgt->unk24;
            delta.y = (float)tgt->unk28;
            delta.z = (float)tgt->unk2C;
            delta.x = actor->unk84.x - delta.x;
            delta.y = actor->unk84.y - delta.y;
            delta.z = actor->unk84.z - delta.z;
            fn_800E1098(&delta);

            Vec3 aim;
            aim.x = delta.x + actor->unk84.x;
            aim.y = delta.y + actor->unk84.y;
            aim.z = delta.z + actor->unk84.z;

            float outDist;
            int outA;
            int outB;
            if (fn_800BB218(actor, actor->unk2C4, &actor->unk84, &aim, actor,
                            &outDist, &outA, &outB, actor->unk900) != 0) {
                float scale = fn_800E1020(&delta);
                fn_800BAFF8(actor, &delta, outA, outB, &aim, outDist,
                            scale * (lbl_804D8814 - outDist));
            }
            if (fn_800BF2D0(&actor->unk84) == fn_800BF2D0(&aim)) {
                ConditionUpdateBit(actor->unk270, 0x1d, 1);
                fn_800BC5A8(actor, &aim);
                actor->unk288 = 3;
            }
        } else if (fn_800BAF40(actor) != -1) {
            float value = OwnerValue(actor, target);
            if (MemFlagIsSet(actor->unk270, 0x1d) != 0 &&
                value > (float)actor->unk8B2 && value < (float)actor->unk8B4) {
                ConditionUpdateBit(actor->unk270, 0x1d, 0);
                fn_800BBCA8(actor);
            }
        } else {
            if (st->unk10 > lbl_804D8848) {
                if (fabsf(ActorAimAngleToPos(target, &actor->unk84)) <
                    lbl_804D884C) {
                    st->unk10 = lbl_804D8848;
                }
            }
            st->unk10 -= actor->unk138;
            if (st->unk10 < lbl_804D8810) {
                st->unk08 = 0;
                fn_800CCA88(actor, &st->unk08);
                st->unk10 =
                    lbl_804D8854 * ((float)rand() / lbl_804D881C) +
                    lbl_804D8850;
            }
        }
    }
    return 0;
}

// Name is a descriptive guess: resets the "[FSHSVE]" follow range and clears
// the two follow condition bits.
bool FSHSVEFollowReset(Actor* actor)
{
    if (actor == 0) {
        return false;
    }
    RangeResetPending(actor->unk270);
    ConditionUpdateBit(actor->unk270, 0, 0);
    ConditionUpdateBit(actor->unk270, 4, 0);
    fn_8029B070(actor, 0x73b1e248);
    fn_8029B070(actor, 0x86a69297);
    return true;
}

// Name is a descriptive guess: starts the "[FSHSVE]" follow state (state 5) and
// re-acquires the current follow target.
int FSHSVEFollowBegin(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }

    FSHSVE* st = actor->unk130;
    st->unk00 = 5;
    st->unk04 = lbl_804D8810;
    st->unk10 = lbl_804D8854 * ((float)rand() / lbl_804D881C) + lbl_804D8850;
    st->unk18 = lbl_804D8840;

    ConditionUpdateBit(actor->unk270, 4, 1);
    ConditionUpdateBit(actor->unk270, 3, 1);
    actor->unk288 = 0;

    int kind = fn_800B26B8(actor);
    if (kind < 4 && fn_800B1EF4(actor, kind) == 0) {
        if (fn_800B1ED4(actor, kind) & 0x200) {
            st->unk2C = lbl_804D8814;
        }
        fn_800B1F20(actor, kind);
        fn_800B1AB8(lbl_804D882C, actor, kind, 1);

        int target;
        fn_800B261C(actor, kind, &target);
        if (target == 0) {
            return 0;
        }
        FollowSetTargetObj(actor->unk270, (void*)target);
    }
    return 1;
}

// Name is a descriptive guess: exit callback of the "[FSHSVE]" follow state.
// Retail passes the callee no argument (no r3 setup), so it is reached through
// a no-argument signature here.
int FSHSVEFollowExitState(void)
{
    ((void (*)())fn_800BBCA8)();
    return 1;
}

// Name is a descriptive guess: aborts the "[FSHSVE]" follow state, flags the
// actor as state 3 and drops the pending range conditions.
int FSHSVEFollowAbortState(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }
    actor->unk288 = 3;
    actor->unk130->unk00 = 4;
    ConditionUpdateBit(actor->unk270, 3, 1);
    RangeResetPending(actor->unk270);
    return 1;
}

// Name is a descriptive guess: second exit callback of the "[FSHSVE]" follow
// state.  Retail sets up no argument, so it is reached through a no-argument
// signature here as well.
int FSHSVEDetachExitState(void)
{
    ((void (*)())fn_800BBCA8)();
    return 1;
}

// Name is a descriptive guess: resets the "[FSHSVE]" follow state to its
// starting state and re-arms the range conditions.
int FSHSVEResetRangeState(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }
    actor->unk288 = 3;
    actor->unk130->unk00 = 3;
    ConditionUpdateBit(actor->unk270, 3, 1);
    RangeResetPending(actor->unk270);
    return 1;
}

// Name is a descriptive guess: re-validates the follow target of the "[FSHSVE]"
// follow state and picks the next follow sub-state.
int FSHSVEFollowTargetUpdate(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }

    FSHSVE* st = actor->unk130;
    int kind = fn_800B26B8(actor);

    if (kind == 4) {
        actor->unk268 = 4;
        return 1;
    }

    if (fn_800B1EF4(actor, kind) == 0 || fn_800B2454(actor, kind) == 0) {
        if (fn_800B1ED4(actor, kind) & 0x200) {
            st->unk2C = lbl_804D8814;
        }

        if (RangeIsIdle(actor->unk270) != 0) {
            fn_800B1F20(actor, kind);
            Vec3 pos;
            OwnerSlotGetPosition((int)actor, kind, (float*)&pos);
            FollowSetTargetVec(actor->unk270, &pos);
            fn_800BC5A8(actor, &pos);
        } else {
            if (fn_800B1EF4(actor, kind) == 0) {
                RangeResetPending(actor->unk270);
                actor->unk268 = fn_800B22F8(actor, kind);
                return 1;
            }
            int count = fn_800B22F8(actor, kind);
            if (count < 2) {
                actor->unk268 = count;
                return 1;
            }
        }
    }

    if (fn_800BAF40(actor) == -1) {
        actor->unk288 = 0;
    } else {
        if (st->unk00 == 2) {
            actor->unk288 = 2;
        } else {
            actor->unk288 = 3;
        }

        if (FollowLineCheckSweep(actor, (float*)&actor->unk2B4) != 0) {
            int maxDist = 0x1f40;
            int limit = *(int*)((u8*)actor->unkC4 + 0xC8);
            if (limit < maxDist) {
                maxDist = limit;
            }
            if (OwnerValue(actor, &actor->unk2B4) <= (float)maxDist) {
                actor->unk268 = 4;
                fn_800B2418(lbl_804D8814, actor, kind);
                return 1;
            }
        }
        fn_800B2418(lbl_804D8830, actor, kind);
    }
    return 0;
}

// Name is a descriptive guess: exit callback of a "[FSHSVE]" state; it forwards
// the owner straight to the abort helper.
int FSHSVEStateAbortExit(Actor* actor)
{
    fn_800BBCA8(actor);
    return 1;
}

// Name is a descriptive guess: entry callback of the "[FSHSVE]" follow state
// that switches the actor to state 2 and re-arms the range conditions.
int FSHSVEStateTwoEnter(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }
    actor->unk288 = 2;
    actor->unk130->unk00 = 2;
    ConditionUpdateBit(actor->unk270, 3, 1);
    RangeResetPending(actor->unk270);
    return 1;
}

// Name is a descriptive guess: per-frame refresh of the "[FSHSVE]" follow
// target; it re-validates the queued follow target and picks the state index.
int FSHSVEFollowRetargetUpdate(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }

    FSHSVE* st = actor->unk130;
    int kind = fn_800B26B8(actor);

    if (kind != 4) {
        actor->unk268 = fn_800B22F8(actor, kind);
        if (fn_800B1ED4(actor, kind) & 0x200) {
            st->unk2C = lbl_804D8814;
        }
        if (MemFlagIsSet(actor->unk270, 5) == 0) {
            return 1;
        }
    }

    if (actor->unk220 == 0 && MemFlagIsSet(actor->unk270, 5) == 0) {
        return 1;
    }

    if (actor->unk220 == 2) {
        actor->unk10C = fn_800C748C;
        if (FollowGetTargetObj(actor->unk270) != (void*)actor->unk22C) {
            FollowSetTargetObj(actor->unk270, (void*)actor->unk22C);
            if (actor->unk21C != 0) {
                actor->unk288 = actor->unk21C;
                fn_800BC56C(actor, (void*)actor->unk22C);
            }
        }
        if (fn_800BAF40(actor) == -1 && actor->unk21C != 0) {
            FollowSetTargetObj(actor->unk270, 0);
            actor->unk288 = 0;
        }
    } else {
        actor->unk288 = 0;
    }

    return 0;
}

extern void FollowOffsetUpdate(void* param);

// Begin following: install the per-frame follow-offset update and clear the
// follow condition left over from an earlier attempt.
int FollowOffsetInit(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }
    actor->unk10C = FollowOffsetUpdate;
    if (actor->unk220 == 4) {
        actor->unk220 = 0;
    }
    if (HasCondition(actor->unk270, 0xd)) {
        ConditionSetBit(actor->unk270, 0xd, 0);
    }
    return 1;
}

// Enter follow-offset state 1.
int FollowStateStart(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }
    actor->unk130->unk00 = 1;
    return 1;
}

// Decide whether the follow-offset behaviour may engage this frame: it is
// blocked while condition bit 5 is set, then the follow target info is
// refreshed and accepted when the target's kind tag is the engaged value.
int FSHSVEFollowCheckTarget(Actor* actor)
{
    u32 r;
    FSHSVE* st;
    int kind;
    int target[3];

    if (actor == 0) {
        return 0;
    }
    st = actor->unk130;
    kind = fn_800B26B8(actor);
    if (MemFlagIsSet(actor->unk270, 5) != 0) {
        return 1;
    }
    if (kind != 4) {
        fn_800B261C(actor, kind, target);
        r = fn_800B1ED4(actor, kind);
        if ((r & 0x200) != 0) {
            st->unk2C = lbl_804D8814;
        }
        if ((void*)target[0] != 0 && (*(u32*)(target[0] + 0x38) >> 24) == 2) {
            actor->unk268 = fn_800B22F8(actor, kind);
            return 1;
        }
    }
    return actor->unk220 != 0;
}

// State-validity callback for the follow-offset state: always allowed.
int FSHSVEFollowStateValid(void)
{
    return 1;
}

// Name is a descriptive guess: clears the "[FSHSVE]" state block and drops the
// follow probe before aborting the current follow search.
int FSHSVEClearStateAndAbort(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }

    actor->unk130->unk00 = 0;
    actor->unk288 = 0;
    fn_800BBCA8(actor);
    return 1;
}

// Name is a descriptive guess: picks the next state index of the "[FSHSVE]"
// actor from the follow target, the follow mode and the state flags.
int FSHSVEPickNextState(Actor* actor)
{
    if (actor == 0) {
        return 0;
    }

    FSHSVE* st = actor->unk130;
    int mode = actor->unk268;
    int kind = fn_800B26B8(actor);
    int state;
    Actor* target;

    if ((st->unk24 & 1) != 0 &&
        (target = (Actor*)FollowGetTargetObj(actor->unk270)) != 0) {
        if (FollowLineCheckActorDist(actor, target, lbl_804D8814) != 0) {
            int bound = 0x1f40;
            int limit = *(int*)((u8*)actor->unkC4 + 0xC8);
            if (limit < bound) {
                bound = limit;
            }
            if (OwnerValue(actor, target) <= (float)bound) {
                state = 2;
            } else {
                state = 3;
            }
        } else {
            state = 3;
        }
    } else if (MemFlagIsSet(actor->unk270, 5) != 0) {
        state = 4;
    } else if (mode < 2 || (st->unk24 & 1) != 0) {
        if (kind == 4) {
            state = 0;
        } else {
            int targetObj;
            fn_800B261C(actor, kind, &targetObj);
            if ((st->unk24 & 4) != 0) {
                return 5;
            }
            if (targetObj == 0) {
                state = 1;
            } else if (((u32)((Actor*)targetObj)->unk38 >> 24) == 2) {
                if (FollowLineCheckActorDist(actor, (void*)targetObj,
                                             lbl_804D8814) != 0) {
                    int bound = 0x1f40;
                    int limit = *(int*)((u8*)actor->unkC4 + 0xC8);
                    if (limit < bound) {
                        bound = limit;
                    }
                    if (OwnerValue(actor, (void*)targetObj) <= (float)bound) {
                        state = 2;
                    } else {
                        state = 3;
                    }
                } else {
                    state = 3;
                }
            } else {
                fn_800B1F20(actor, kind);
                state = 0;
            }
        }
    } else if (mode < 4) {
        state = 1;
    } else {
        state = 0;
    }

    if (state == 4) {
        return 1;
    }
    if (state == 1) {
        if (mode == 1) {
            return 4;
        }
        return (mode == 2) ? 3 : 0;
    }
    if (state == 2) {
        return 5;
    }
    if (state == 3) {
        return 6;
    }
    if (state == 0) {
        return actor->unk220 != 0;
    }
    return 0;
}

// An owner as seen by the zone probe: a message sink at 0x30.
struct ProbeOwner {
    u8 unk00[0x30];                          // 0x00
    void (*unk30)(int msg, void* data);      // 0x30
};

// Name is a descriptive guess: zone-walk probe that asks the owner for its
// "seen" report (message 0x401) and flags a positive reading through out.
int FSHSVEZoneNodeProbe(int node, Owner* owner, int* out)
{
    int info[8];

    if (OwnerId(owner) != -1) {
        info[0] = (int)owner;
        ((ProbeOwner*)owner)->unk30(0x401, info);
        if (((float*)info)[4] > lbl_804D8810) {
            *out = 1;
        }
    }
    return 1;
}

extern "C" void fn_800B2E34(Actor* actor, int a, int b);
extern "C" void fn_800B3164(Actor* actor, int a, int b);
extern "C" void fn_800CECCC(Actor* actor);
extern void OwnerUpdateSlots(Owner* owner);

// Name is a descriptive guess: per-frame behaviour callback of the "[FSHSVE]"
// actor; it refreshes the owner slots and blends the state block out for each
// of the four follow slots that is in the flagged state.
int FSHSVEFrameUpdate(Actor* actor)
{
    FSHSVE* st = actor->unk130;

    if (MemFlagIsSet(actor->unk270, 5) == 0) {
        fn_800B2E34(actor, 0, 0);
        fn_800B3164(actor, 0, 0);
        fn_800CECCC(actor);
    } else {
        OwnerUpdateSlots(actor);
    }

    if ((actor->unk144 & 1) != 0 && (actor->unk144 & 4) == 0 &&
        (actor->unk158 & 0x200) != 0) {
        st->unk2C = lbl_804D8814;
    }
    if ((actor->unk16C & 1) != 0 && (actor->unk16C & 4) == 0 &&
        (actor->unk180 & 0x200) != 0) {
        st->unk2C = lbl_804D8814;
    }
    if ((actor->unk194 & 1) != 0 && (actor->unk194 & 4) == 0 &&
        (actor->unk1A8 & 0x200) != 0) {
        st->unk2C = lbl_804D8814;
    }
    if ((actor->unk1BC & 1) != 0 && (actor->unk1BC & 4) == 0 &&
        (actor->unk1D0 & 0x200) != 0) {
        st->unk2C = lbl_804D8814;
    }
    return 1;
}

// Name is a descriptive guess: 0x400 reply block written by a target actor's
// message handler and read back by the "[FSHSVE]" target queries.
struct FollowTargetInfo {
    void* obj;                   // 0x00
    s32 unk04;                   // 0x04
    s32 unk08;                   // 0x08
    s32 unk0C;                   // 0x0C
    float unk10;                 // 0x10
    float unk14;                 // 0x14
    s32 unk18;                   // 0x18
    s32 unk1C;                   // 0x1C
};

extern "C" void* memset(void* dst, int c, u32 size);

// Name is a descriptive guess: queries the current follow target (packed owner
// id, kind in the high byte) and reports whether its state blocks the follow
// behaviour.
int FSHSVETargetBlocksFollow(Actor* actor, u32 targetId)
{
    FollowTargetInfo info;

    memset(&info, 0, sizeof(info));
    if (SendMsg(targetId, 0x400, &info) == 0 || info.obj == 0) {
        return 0;
    }

    switch ((int)(targetId >> 24)) {
    case 0x13:
        if (HasCondition((u8*)info.obj + 0x270, 0xb) != 0 || info.unk0C == 0 ||
            (info.unk18 & 1) != 0) {
            return 0;
        }
        return (info.unk18 & 2) == 0;

    case 2:
        return info.unk0C != 0;
    }
    return 1;
}

// Name is a descriptive guess: the player object as seen by the follow query;
// its slot table holds the asset tags of the actors the player is aiming at.
struct PlayerObject {
    u8 unk00[0x26C4];            // 0x00
    int* unk26C4[2];             // 0x26C4 host slot tag pointers
    u8 unk26D4[0x26E4 - 0x26CC]; // 0x26CC
    void* unk26E4;               // 0x26E4 current actor
};

PlayerObject* GetPlayer(void);
int PlayerInHost(Actor* actor);
int ActorAnimState(int actor, u32 anim);

// Seeded form of the lazy name hash: the message's own hash is folded in first.
static inline u32 NameHashSeed(u32 hash, const char* name)
{
    if (name) {
        const char* p = name;
        const char* end = p;
        while (*end != 0) {
            int c = *p;
            u32 v;
            if (c == -1) {
                v = 0xffffffff;
            } else {
                v = __upper_map[(u8)c];
            }
            hash *= 31;
            hash += v & 0xff;
            p++;
            end++;
        }
        hash %= 0xffffffff;
    }
    return hash;
}

// Asset tag of the actor in the player's current host slot.
static inline int PlayerSlotTag(PlayerObject* player)
{
    return *player->unk26C4[PlayerInHost((Actor*)player)];
}

// Name is a descriptive guess: decides whether the "[FSHSVE]" actor may play
// its follow anim this frame; on success it answers the query at args[1] and
// queues a fresh follow anim request in args[3].
int FSHSVETargetFollowCheck(int* args)
{
    Actor* actor = (Actor*)args[0];
    FSHSVE* st = actor->unk130;
    FollowTargetInfo info;
    PlayerObject* player;

    info.obj = actor;
    info.unk04 = OwnerId(actor);
    ((ProbeOwner*)actor)->unk30(0x400, &info);

    u32 hash = NameHashSeed((u32)args[2], lbl_80348490.query);
    if (ActorPlayAnim(OwnerId(actor), hash, args[1]) != 0) {
        args[3] = 0;
        return 1;
    }

    if (fn_80049940() != 0) {
        if (args[2] == 0x7C44EC3D) {
            args[3] = (int)0xA53F7E23;
            return 1;
        }
        return 0;
    }

    if (MemFlagIsSet(actor->unk270, 0xb) != 0) {
        return 1;
    }

    if (st->unk14 < lbl_804D30D0) {
        return 1;
    }

    if (HasCondition(actor->unk270, 10) != 0 ||
        HasCondition(actor->unk270, 9) != 0) {
        return 0;
    }

    if (args[2] == 0x3CA34BD6 && MemFlagIsSet(actor->unk270, 0x20) == 0) {
        args[3] = 0x1379C466;
        return 1;
    }

    if (ActorAnimState(actor->unk38, 0xDC35206A) == 0) {
        return 0;
    }

    if (HasCondition(actor->unk270, 0xb) != 0) {
        return 0;
    }

    player = GetPlayer();
    if (player == 0) {
        return 0;
    }

    if (player->unk26E4 != 0 &&
        (*(u32*)((u8*)player->unk26E4 + 0x38) >> 24) == 1) {
        return 0;
    }

    if (PlayerSlotTag(player) == 0x4624485 ||
        PlayerSlotTag(player) == 0x13C05 ||
        PlayerSlotTag(player) == (int)0xE0AB9B4F ||
        PlayerSlotTag(player) == 0xFFF5) {
        return 0;
    }

    if (MemFlagIsSet(actor->unk270, 0x1e) != 0) {
        return 0;
    }

    if (args[2] == 0x7C44EC3D) {
        int kind = fn_800B26B8(actor);
        if (kind != 4 && fn_800B22F8(actor, kind) < 2) {
            return 0;
        }
    }

    {
        int block[4];
        block[1] = 0;
        block[3] = -1;
        args[3] = 0x272CAC;
        block[2] = args[3];
        SendMsg(args[1], 0x40b, block);
    }
    return 1;
}

// Tint blocks at lbl_803AADA0 / lbl_803AADDC: a float at 0x08 and four integer
// colour components at 0x10..0x1C, 0x3C bytes each.
struct Tint {
    u8 unk00[0x08];              // 0x00
    float unk08;                 // 0x08
    u8 unk0C[0x10 - 0x0C];       // 0x0C
    int unk10;                   // 0x10
    int unk14;                   // 0x14
    int unk18;                   // 0x18
    int unk1C;                   // 0x1C
    u8 unk20[0x3C - 0x20];       // 0x20
};

extern Tint lbl_803AADA0;
extern Tint lbl_803AADDC;

// Integer colour pairs of the state-block tint table in .sdata.
extern int lbl_804D30FC;
extern int lbl_804D3100;
extern int lbl_804D3104;
extern int lbl_804D3108;
extern int lbl_804D310C;
extern int lbl_804D3110;
extern int lbl_804D3114;
extern int lbl_804D3118;
extern int lbl_804D311C;
extern int lbl_804D3120;
extern float lbl_804D3124;
extern float lbl_804D3128;
extern int lbl_804D312C;
extern int lbl_804D3130;
extern int lbl_804D3134;
extern int lbl_804D3138;
extern int lbl_804D313C;
extern int lbl_804D3140;
extern int lbl_804D3144;
extern int lbl_804D3148;
extern int lbl_804D314C;
extern int lbl_804D3150;
extern int lbl_804D3158;
extern int lbl_804D315C;
extern int lbl_804D3160;
extern int lbl_804D3164;
extern int lbl_804D3168;
extern int lbl_804D316C;
extern int lbl_804D3170;
extern int lbl_804D3174;
extern int lbl_804D3178;
extern int lbl_804D317C;
extern float lbl_804D8860;
extern float lbl_804D8864;
extern float lbl_804D8868;

class Skin;

extern "C" int fn_800AFD48(Actor* actor, int* out, int arg);
extern int ModelAnimate(int handle, char* name, int arg);
extern "C" int fn_8003A1F0(int model, int part, int anim);
extern "C" void fn_8003F15C(float blend, int model, int skin, int arg4,
                             int* out, Tint* tint);
extern "C" void fn_800C9970(Actor* actor);
extern "C" int fn_800BA84C(int id);
extern int Fn80080FA8(void);
extern "C" int fn_80049660(PlayerObject* player);
extern "C" int fn_800BA7FC(int id);
extern "C" float fn_80115F68(int id);
extern "C" int fn_800CC21C(void);
extern float fn_80017084(float angle);
extern int SkinBlendTint(Skin* skin, int a, int b, int c, float blend);

// Name is a descriptive guess: per-update visual refresh of the "[FSHSVE]"
// actor state block.  It picks the model/skin to draw, blends the state tint
// block, chooses the follow-target fade skin and finally schedules the
// secondary "seen target" skin blend.
int FSHSVEVisualUpdate(int* args)
{
    Actor* actor = (Actor*)args[0];
    FSHSVE* st = actor->unk130;
    int local98[2];
    int anim;
    int skin;
    int model;
    int* out;
    float f;
    float t;
    float u;
    u32 flags;

    if (lbl_804D8860 < actor->unk13C) {
        ActorPlayAnim(actor->unk38, 0xdbe2cae6, actor->unk38);
    }
    actor->unk13C = lbl_804D8810;

    model = fn_800AFD48(actor, local98, 0);
    anim = ModelAnimate(actor->unkCC, 0, 0);
    skin = fn_8003A1F0(model, actor->unkF0, anim);

    f = st->unk38;
    if (lbl_804D8810 < f || (st->unk24 & 0xA) != 0) {
        flags = st->unk24 & 0xA;
        if (flags == 0 || f <= lbl_804D8810) {
            if (flags == 0) {
                t = f / lbl_804D3154;
                if (t < lbl_804D8810) {
                    t = lbl_804D8810;
                } else if (t > lbl_804D8814) {
                    t = lbl_804D8814;
                }
                u = lbl_804D8814 - t;
                lbl_803AADA0.unk08 =
                    (float)lbl_804D316C * u + (float)lbl_804D3158 * t;
                lbl_803AADA0.unk10 =
                    (float)lbl_804D3170 * u + (float)lbl_804D315C * t;
                lbl_803AADA0.unk14 =
                    (float)lbl_804D3174 * u + (float)lbl_804D3160 * t;
                lbl_803AADA0.unk18 =
                    (float)lbl_804D3178 * u + (float)lbl_804D3164 * t;
                lbl_803AADA0.unk1C =
                    (float)lbl_804D317C * u + (float)lbl_804D3168 * t;
            } else {
                float s = fn_80017084(st->unk3C * lbl_804D3124);
                float blend = lbl_804D8864 * (lbl_804D8814 + s);
                u = lbl_804D8814 - blend;
                lbl_803AADA0.unk08 =
                    (float)lbl_804D3140 * u + (float)lbl_804D312C * blend;

                s = fn_80017084(st->unk3C * lbl_804D3128 + lbl_804D8838);
                blend = lbl_804D8864 * (lbl_804D8814 + s);
                u = lbl_804D8814 - blend;
                lbl_803AADA0.unk10 =
                    (float)lbl_804D3144 * u + (float)lbl_804D3130 * blend;
                lbl_803AADA0.unk14 =
                    (float)lbl_804D3148 * u + (float)lbl_804D3134 * blend;
                lbl_803AADA0.unk18 =
                    (float)lbl_804D314C * u + (float)lbl_804D3138 * blend;
                lbl_803AADA0.unk1C =
                    (float)lbl_804D3150 * u + (float)lbl_804D313C * blend;
            }
        } else {
            t = f / lbl_804D30F8;
            if (t < lbl_804D8810) {
                t = lbl_804D8810;
            } else if (t > lbl_804D8814) {
                t = lbl_804D8814;
            }
            u = lbl_804D8814 - t;
            lbl_803AADA0.unk08 =
                (float)lbl_804D3110 * u + (float)lbl_804D30FC * t;
            lbl_803AADA0.unk10 =
                (float)lbl_804D3114 * u + (float)lbl_804D3100 * t;
            lbl_803AADA0.unk14 =
                (float)lbl_804D3118 * u + (float)lbl_804D3104 * t;
            lbl_803AADA0.unk18 =
                (float)lbl_804D311C * u + (float)lbl_804D3108 * t;
            lbl_803AADA0.unk1C =
                (float)lbl_804D3120 * u + (float)lbl_804D310C * t;
        }
        fn_8003F15C(lbl_804D8814, model, skin, actor->unkD0, args + 4,
                    &lbl_803AADA0);
    }

    fn_800C9970(actor);

    f = st->unk14;
    if (f >= lbl_804D30D0) {
        if (actor->unk91C <= lbl_804D8814) {
            out = args + 4;
        } else {
            float d = lbl_804D30CC;
            if (lbl_804D30CC >= lbl_804D30C8) {
                d = lbl_804D30C8;
            }
            float r = (actor->unk91C - lbl_804D8814) / d;
            if (r < lbl_804D8810) {
                r = lbl_804D8810;
            } else if (r > lbl_804D8814) {
                r = lbl_804D8814;
            }
            if (r < lbl_804D8810) {
                r = lbl_804D8810;
            } else if (r > lbl_804D8814) {
                r = lbl_804D8814;
            }
            out = (int*)SkinBlendTint((Skin*)(args + 4), 0xff, 0, 0, r);
        }
    } else {
        float a;
        if (f >= lbl_804D8868) {
            if (f <= lbl_804D30D0 - lbl_804D8868) {
                a = lbl_804D8814;
            } else {
                a = (lbl_804D30D0 - f) / lbl_804D8868;
            }
        } else {
            a = f / lbl_804D8868;
        }
        out = (int*)SkinBlendTint((Skin*)(args + 4), 0, 0xff, 0, a);
    }

    if (actor->unk8F8 == 6) {
        int v = fn_800BA84C(9);
        t = ((float)v - actor->unk8FC) / (float)v;
        if (t < lbl_804D8810) {
            t = lbl_804D8810;
        } else if (t > lbl_804D8814) {
            t = lbl_804D8814;
        }
        if (lbl_804D8810 < t) {
            fn_8003F15C(lbl_804D8814, model, skin, actor->unkD0, args + 4, 0);
        }
        return 1;
    }

    fn_8003F15C(lbl_804D8814, model, skin, actor->unkD0, out, 0);

    if (HasCondition(actor->unk270, 0x22) != 0) {
        return 1;
    }
    if (HasCondition(actor->unk270, 0x21) == 0) {
        if (local98[0] > 5) {
            return 1;
        }
        if (*(u32*)actor->unkC4 != 0xC51E1645) {
            return 1;
        }
        if (Fn80080FA8() != 0) {
            return 1;
        }
        if (HasCondition(actor->unk270, 9) != 0) {
            return 1;
        }
        if (HasCondition(actor->unk270, 10) != 0) {
            return 1;
        }
        if (fn_80049660(GetPlayer()) != 0 && fn_800BA7FC(0x35) == 0) {
            return 1;
        }
    }

    if (fn_80115F68(actor->unk38) <= lbl_804D8810) {
        lbl_803AADDC.unk10 = 0xff;
        lbl_803AADDC.unk14 = 0;
        lbl_803AADDC.unk18 = 0;
        int part = *(int*)((u8*)actor->unkE8 + 0x18);
        if (part >= 0) {
            PlayerObject* player = GetPlayer();
            int inHost = PlayerInHost((Actor*)player);
            if (inHost == 0 ||
                (player->unk26E4 != 0 &&
                 (((*(u32*)((u8*)player->unk26E4 + 0x38) >> 24) == 1) ||
                  (((*(u32*)((u8*)player->unk26E4 + 0x38) >> 24) == 0x13) &&
                   fn_800CC21C() != 0)))) {
                int partAnim = ModelAnimate(actor->unkCC, 0, 0);
                int partSkin = fn_8003A1F0(part, actor->unkF0, partAnim);
                fn_8003F15C(lbl_804D8814, part, partSkin, actor->unkD0, 0,
                            &lbl_803AADDC);
            }
        }
    }
    return 1;
}

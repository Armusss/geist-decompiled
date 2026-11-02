// u_801AE50C.cpp: enemy / weapon behaviour code (0x801AE50C-0x801BEC4C).
// Only the functions drafted so far have bodies; every other function is declared with a TODO comment.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;


extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

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

// Query messages: the receiver writes its answer over the first word of the String.
struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

extern "C" int SendMsg(int to, int msg, void* data);                 // 0x800265D4
extern "C" void fn_800872BC(int flags, const char* fmt, ...);        // debug log

inline void QueryGet(int* out, int target, u32 hash) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, 0x41E, &m);
    *out = *(int*)&m.result;
}

// Animation set of a character type (three words used here).
struct AiAnimSet {
    u8 pad00[0x14];
    int a14;         // 0x14
    int a18;         // 0x18
    int a1C;         // 0x1C
    int a20;         // 0x20
    int a24;         // 0x24
};

// The actor a character is currently targeting / carried by.
struct AiTarget {
    u8 pad00[0x38];
    u32 handle;      // 0x38: type in the top byte
    u8 pad3C[0x270 - 0x3C];
    u8 conditions[4]; // 0x270
};

// Current / next state of a character (a Player-style state record).
struct AiStateRec {
    int cur;         // 0x00
    int next;        // 0x04
    int unk8;        // 0x08
    int unkC;        // 0x0C
};

// Enemy character actor (descriptive guess; only the fields used in this file).
struct Actor {
    u8 pad00[0x38];
    int handle;      // 0x38
    u8 pad3C[0x50 - 0x3C];
    u32 flags;       // 0x50
    u8 pad54[0x58 - 0x54];
    int model;       // 0x58
    u8 pad5C[0x78 - 0x5C];
    int hold;        // 0x78
    int unk7C;       // 0x7C
    u8 pad80[0x8C - 0x80];
    void* cb8C;      // 0x8C
    u8 pad90[0x98 - 0x90];
    int anim98;      // 0x98
    u8 pad9C[0xA4 - 0x9C];
    int animFlagA4;  // 0xA4
    u8 padA8[0xC4 - 0xA8];
    int animC4;      // 0xC4
    u8 padC8[0xE4 - 0xC8];
    void* cbE4;      // 0xE4
    u8 padE8[0xF0 - 0xE8];
    int animF0;      // 0xF0
    u8 padF4[0xFC - 0xF4];
    int animFlagFC;  // 0xFC
    u8 pad100[0x11C - 0x100];
    int anim11C;     // 0x11C
    u8 pad120[0x2F4 - 0x120];
    u8 sub2F4[0x20]; // 0x2F4
    int unk314;      // 0x314
    int unk318;      // 0x318
    u32 flags31C;    // 0x31C
    u32 flags320;    // 0x320
    u8 pad324[4];
    int unk328;      // 0x328
    u8 pad32C[0x38C - 0x32C];
    float f38C;      // 0x38C
    float f390;      // 0x390
    float f394;      // 0x394
    float f398;      // 0x398
    u8 pad39C[0x638 - 0x39C];
    int unk638;      // 0x638
    u8 pad63C[0x678 - 0x63C];
    u32 flags678;    // 0x678
    u8 pad67C[0x73C - 0x67C];
    int variant;     // 0x73C
    u8 pad740[0x7A4 - 0x740];
    AiStateRec state;   // 0x7A4
    u32 flags7B4;    // 0x7B4
    u8 pad7B8[0x7D0 - 0x7B8];
    int unk7D0;      // 0x7D0
    int unk7D4;      // 0x7D4
    float unk7D8;    // 0x7D8
    int unk7DC;      // 0x7DC
    u8 pad7E0[0x14FC - 0x7E0];
    float unk14FC;   // 0x14FC
    u8 pad1500[0x1D04 - 0x1500];
    u8 unk1D04[0x1D94 - 0x1D04];
    int unk1D94;     // 0x1D94
    u8 pad1D98[0x26B0 - 0x1D98];
    int unk26B0;     // 0x26B0
    u8 pad26B4[0x26C4 - 0x26B4];
    AiAnimSet* set[7]; // 0x26C4
    u8 sub26E0[4];   // 0x26E0
    Actor* host;   // 0x26E4
    AiTarget* target; // 0x26E8
    u8 pad26EC[0x279C - 0x26EC];
    int unk279C;     // 0x279C
};

// Engine services (names in symbols.txt come from other units).
void PlayerSetValue4(void* state, int v);
void PlayerResetGunTimer(void* a);
void PlayerSelectCollisionUpdate(void* a, float dt);
extern "C" void fn_800769A0(void* a, float dt);
int PlayerInHost(Actor* a);
void PlayerSetHeldSlotIndex(void* a, int slot);
void PlayerSetHeldSlotMode(void* a, int mode);
void PlayerDispatchMotionUpdate(void* a, int mode, float dt);
extern "C" int fn_802A1D04();
extern "C" int fn_800C5210(int to, int msg, void* data);

struct MsgQuery {
    int result;      // 0x00
    u32 hash;        // 0x04
    int pad08;       // 0x08
    int handle;      // 0x0C
};
extern "C" void fn_800699A8(void* a);
void PlayerSetActionMode(void* a, int mode);
void DashUpdate(Actor* a);
int HasCondition(void* conditions, int id);
void ActorPlayAnim(int a, u32 b, int c);
int ModelBoneName(int model);
int ModelBoneNodeChain(int model, int bone, int* out, int n);
int Fn800F7DEC(int a, int b, int c, int d, void* e);
int Fn800F7F5C(int a, int b, int c, int d, void* e, float f);
extern "C" int fn_80175EFC(int v);
extern "C" void fn_80155A84(void* a, void* b, int c);
extern "C" int fn_800A1BC8(int a, int b, int c, int d, int e);
extern "C" void* fn_800A384C(void* t);
extern "C" int fn_800498D0(Actor* a);
extern "C" void fn_80048BA8(Actor* a, void* b, int c);
extern "C" void fn_80050EB0(Actor* a);
extern "C" int fn_800F7994(int a, u32 b);
extern "C" int FxStart(void* fx, int a, int b);


// Animation lookup table: one row per animation kind, one slot per character variant (data of another unit).
struct AnimRow {
    void (*cb00)(Actor*);  // 0x00
    void (*cb04)(Actor*);  // 0x04
    void (*msg)(Actor*);   // 0x08
    int byVariant[15];     // 0x0C
};
extern AnimRow lbl_803AAE98[];
extern AnimRow lbl_803A00D0[];
int PlayerGunAction(int action, void* p);
extern int lbl_80496C98[16];
u32 AssetFind(u32 table, u32 key);
extern "C" int fn_801B7B88(int action, void* p);

// Per-enemy data block (descriptive guess; only the fields used here).
struct EnemyData {
    int f0;          // 0x00
    int state;       // 0x04
    int f8;          // 0x08
    u8 padC[0x14 - 0xC];
    int f14;         // 0x14
    u8 pad18[0x24 - 0x18];
    float f24;       // 0x24
    u8 pad28[0x50 - 0x28];
    float f50;       // 0x50
};

// Behaviour object with a data block at 0x130 (descriptive guess).
struct EnemyObj {
    u8 pad00[0x130];
    EnemyData* data; // 0x130
    u8 pad134[0x91C - 0x134];
    float f91C;      // 0x91C
};

inline float Lerp(float lo, float hi, float t) {
    float d = hi - lo;
    return lo + d * t;
}
extern int lbl_804D31C4;
extern int lbl_804D31C0;
extern "C" int fn_800F7994(int a, u32 b);
extern "C" int FxStart(void* fx, int a, int b);
extern float lbl_804D31A8;
extern float lbl_804D31AC;
extern "C" int rand();
extern "C" void fn_8016EEF4(int v);
extern "C" void fn_80073E80(Actor* a);

// TODO (not drafted yet, 0x500 bytes at 0x801AE50C): game logic
extern "C" void fn_801AE50C();

// TODO (not drafted yet, 0x1A4 bytes at 0x801AEA0C): game logic
extern "C" void fn_801AEA0C();

// TODO (not drafted yet, 0xB0 bytes at 0x801AEBB0): game logic
extern "C" void fn_801AEBB0();

// TODO (not drafted yet, 0x8 bytes at 0x801AEC60): game logic
extern "C" void fn_801AEC60();

// TODO (not drafted yet, 0x1B0 bytes at 0x801AEC68): game logic
extern "C" void fn_801AEC68();

// TODO (not drafted yet, 0xD8 bytes at 0x801AEE18): game logic
extern "C" void fn_801AEE18();

// TODO (not drafted yet, 0x520 bytes at 0x801AEEF0): game logic
extern "C" void fn_801AEEF0();

// TODO (not drafted yet, 0xC0 bytes at 0x801AF410): game logic
extern "C" void fn_801AF410();

// TODO (not drafted yet, 0xDC bytes at 0x801AF4D0): game logic
extern "C" void fn_801AF4D0();

// TODO (not drafted yet, 0xC8 bytes at 0x801AF5AC): game logic
extern "C" void fn_801AF5AC();

// TODO (not drafted yet, 0xA8 bytes at 0x801AF674): game logic
extern "C" void fn_801AF674();

// TODO (not drafted yet, 0xD0 bytes at 0x801AF71C): game logic
extern "C" void fn_801AF71C();

// TODO (not drafted yet, 0x790 bytes at 0x801AF7EC): game logic
extern "C" void fn_801AF7EC();

// TODO (not drafted yet, 0x300 bytes at 0x801AFF7C): game logic
extern "C" void fn_801AFF7C();

int EnemyMsg801B027C(EnemyObj* o) {
    o->data->f14 = 0;
    return 1;
}

int AiReturnTrue_801B0290() {
    return 1;
}

int AiReturnTrue_801B0298() {
    return 1;
}

int AiReturnTrue_801B02A0() {
    return 1;
}

int EnemyMsg801B02A8(EnemyObj* o) {
    o->data->f8 = -1;
    return 1;
}

int EnemyMsg801B02BC(EnemyObj* o) {
    o->data->f8 = -1;
    return 1;
}

int EnemyMsg801B02D0(EnemyObj* o) {
    o->data->f8 = -1;
    return 1;
}

int EnemyMsg801B02E4(EnemyObj* o) {
    EnemyData* d = o->data;
    d->f8 = -1;
    o->f91C = 1.0f;
    if (-1.0f != d->f50) {
        fn_8016EEF4((int)d->f50);
        d->f50 = -1.0f;
    }
    d->f24 = Lerp(lbl_804D31A8, lbl_804D31AC, (float)rand() / 32767.0f);
    return 1;
}

// TODO (not drafted yet, 0x2D0 bytes at 0x801B0384): game logic
extern "C" void fn_801B0384();

// TODO (not drafted yet, 0x204 bytes at 0x801B0654): game logic
extern "C" void fn_801B0654();

// TODO (not drafted yet, 0x1E4 bytes at 0x801B0858): game logic
extern "C" void fn_801B0858();

// TODO (not drafted yet, 0x168 bytes at 0x801B0A3C): game logic
extern "C" void fn_801B0A3C();

// TODO (not drafted yet, 0x1B8 bytes at 0x801B0BA4): game logic
extern "C" void fn_801B0BA4();

// TODO (not drafted yet, 0x1B8 bytes at 0x801B0D5C): game logic
extern "C" void fn_801B0D5C();

// TODO (not drafted yet, 0x448 bytes at 0x801B0F14): game logic
extern "C" void fn_801B0F14();

// TODO (not drafted yet, 0x238 bytes at 0x801B135C): game logic
extern "C" void fn_801B135C();

// TODO (not drafted yet, 0x338 bytes at 0x801B1594): game logic
extern "C" void fn_801B1594();

// TODO (not drafted yet, 0x64C bytes at 0x801B18CC): game logic
extern "C" void fn_801B18CC();

// TODO (not drafted yet, 0x250 bytes at 0x801B1F18): game logic
extern "C" void fn_801B1F18();

// TODO (not drafted yet, 0x100 bytes at 0x801B2168): game logic
extern "C" void fn_801B2168();

// TODO (not drafted yet, 0x68C bytes at 0x801B2268): game logic
extern "C" void fn_801B2268();

// TODO (not drafted yet, 0x3F4 bytes at 0x801B28F4): game logic
extern "C" void fn_801B28F4();

// TODO (not drafted yet, 0x634 bytes at 0x801B2CE8): game logic
extern "C" void fn_801B2CE8();

// TODO (not drafted yet, 0x2D4 bytes at 0x801B331C): game logic
extern "C" void fn_801B331C();

// TODO (not drafted yet, 0x20 bytes at 0x801B35F0): game logic
extern "C" void fn_801B35F0();

// TODO (not drafted yet, 0x7D8 bytes at 0x801B3610): game logic
extern "C" void fn_801B3610();

// TODO (not drafted yet, 0x6FC bytes at 0x801B3DE8): game logic
extern "C" void fn_801B3DE8();

// TODO (not drafted yet, 0x78 bytes at 0x801B44E4): game logic
extern "C" void fn_801B44E4();

// TODO (not drafted yet, 0x260 bytes at 0x801B455C): game logic (string: Ghost_Speed_01_L)
extern "C" void fn_801B455C();

bool AiIsZero_801B47BC(int* p) {
    return *p == 0;
}

void AiState801B47CC(Actor* a, float dt) {
    a->unk638 = 0;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->animC4 == -1) {
        if (a->unk7D0 != 0) {
            PlayerSetValue4(&a->state, 3);
        } else {
            PlayerSetValue4(&a->state, 1);
        }
    } else {
        PlayerSetValue4(&a->state, 0x16);
    }
}

void AiState801B4868(Actor* a) {
    a->cb8C = 0;
    if (a->animC4 != -1) {
        a->anim98 = a->animC4;
    }
    if (a->anim11C == -1) {
        return;
    }
    a->animF0 = a->anim11C;
}

void AiState801B4894(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    PlayerSetValue4(&a->state, 0x15);
    if (a->unk279C == -1 || fn_80175EFC(a->unk279C) != 0) {
        PlayerSetValue4(&a->state, 3);
        fn_80155A84(a->sub26E0, a->target, 1);
    }
}

void AiState801B4914(Actor* a) {
    AiTarget* t;
    AiAnimSet* set;
    int ride;

    ride = 0;
    t = a->target;
    if ((int)(t->handle >> 24) == 0x13) {
        set = (AiAnimSet*)fn_800A384C(t);
        if (HasCondition(t->conditions, 0xB) != 0) {
            ride = 1;
        }
    }
    PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_803AAE98[21].byVariant[v], ride, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x19;
    a->animF0 = fn_800A1BC8(set->a14, set->a1C, lbl_803AAE98[21].byVariant[v], ride, 0);
    a->animFlagFC = 0x19;
    PlayerSetValue4(&a->state, 0x15);
}

void AiState801B4A10(Actor* a, float dt) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    PlayerSetValue4(&a->state, 3);
    PlayerInHost(a);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
}

void AiState801B4AC0(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->hold = 0;
    a->flags &= ~0x20000;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803AAE98[20].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803AAE98[20].byVariant[v], 0, 0);
    a->animFlagFC = 1;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg(a->host->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(a, 0);
        if (a->anim98 == -1) {
            PlayerSetActionMode(a, 1);
        }
    } else {
        PlayerSetHeldSlotIndex(a, 1);
        if (a->anim98 == -1) {
            PlayerSetActionMode(a, 3);
        }
    }
}

void AiState801B4C30(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    if (a->flags7B4 & 1) {
        a->flags7B4 &= ~1;
    } else {
        a->flags |= 0x20000;
        a->hold = 1;
    }
}

void AiState801B4C8C(Actor* a) {
    int idx;
    AiTarget* t;
    AiAnimSet* set;
    int ride;

    ride = 0;
    t = a->target;
    if ((int)(t->handle >> 24) == 0x13) {
        set = (AiAnimSet*)fn_800A384C(t);
        if (HasCondition(t->conditions, 0xB) != 0) {
            ride = 1;
        }
    }
    idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_803AAE98[19].byVariant[v], ride, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x19;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803AAE98[19].byVariant[v], 0, 0);
    a->animFlagFC = 0x19;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg(a->target->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(a, 0);
    } else {
        PlayerSetHeldSlotIndex(a, 1);
    }
}

void AiState801B4DEC(Actor* a) {
    fn_80155A84(a->sub26E0, a->target, 1);
}

void AiState801B4E18(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    a->flags |= 0x20000;
    a->hold = 1;
    PlayerSetValue4(&a->state, 0x13);
}

void AiState801B4E74(Actor* a) {
    AiAnimSet* set;
    int idx = PlayerInHost(a);
    AiTarget* t = a->target;
    int v = a->variant;
    int ride = 0;
    if ((int)(t->handle >> 24) == 0x13) {
        set = (AiAnimSet*)fn_800A384C(t);
        if (HasCondition(t->conditions, 0xB) != 0) {
            ride = 1;
        }
    }
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_803AAE98[18].byVariant[v], ride, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x59;
    AiAnimSet* set2 = a->set[idx];
    a->animF0 = fn_800A1BC8(set2->a14, set2->a1C, lbl_803AAE98[18].byVariant[v], 0, 0);
    a->animFlagFC = 0x59;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg(a->target->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(a, 0);
    } else {
        PlayerSetHeldSlotIndex(a, 1);
    }
}

void AiState801B4FD0(Actor* a) {
    a->flags &= ~0x40000;
}

void AiState801B4FE0(Actor* a, float dt) {
    int d1;
    int d2;
    QueryGet(&d1, a->handle, 0x2DA8293C);
    QueryGet(&d2, a->handle, 0xDFF3097A);
    PlayerSetValue4(&a->state, 0x11);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerDispatchMotionUpdate(a, 0, dt);
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    if (a->unk318 == 0) {
        PlayerSetValue4(&a->state, 3);
        if (a->state.cur == 0x11 || a->state.cur == 0x10) {
            fn_80048BA8(a, a->unk1D04, 1);
            Fn800F7F5C(id1, id2, 0x9791AF51, fn_800498D0(a), a->unk1D04, a->unk14FC);
            PlayerSetHeldSlotMode(a, 8);
        }
    } else {
        PlayerResetGunTimer(a);
    }
}

void AiState801B5264(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    a->unk7DC = Fn800F7DEC(id1, id2, 0x6FAB23FE, fn_800498D0(a), 0);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[17].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 1;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[17].byVariant[v], 0, 0);
    a->animFlagFC = 1;
    a->flags |= 0x40000;
}

void AiState801B5458(Actor* a) {
    a->flags &= ~0x40000;
}

void AiState801B5468(Actor* a, float dt) {
    int d1;
    int d2;
    QueryGet(&d1, a->handle, 0x2DA8293C);
    QueryGet(&d2, a->handle, 0xDFF3097A);
    PlayerSetValue4(&a->state, 0x11);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerDispatchMotionUpdate(a, 0, dt);
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    if (a->unk318 == 0) {
        PlayerSetValue4(&a->state, 3);
        if (a->state.cur == 0x11 || a->state.cur == 0x10) {
            fn_80048BA8(a, a->unk1D04, 1);
            Fn800F7F5C(id1, id2, 0x9791AF51, fn_800498D0(a), a->unk1D04, a->unk14FC);
            PlayerSetHeldSlotMode(a, 8);
        }
    } else {
        PlayerResetGunTimer(a);
    }
}

void AiState801B56EC(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    a->unk7DC = Fn800F7DEC(id1, id2, 0x6FAB23FE, fn_800498D0(a), 0);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[16].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 1;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[16].byVariant[v], 0, 0);
    a->animFlagFC = 1;
    a->flags |= 0x40000;
}

void AiState801B58E0(Actor* a) {
    a->flags &= ~0x40000;
}

void AiState801B58F0(Actor* a, float dt) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(a, dt);
}

// TODO (not drafted yet, 0x158 bytes at 0x801B5948): game logic
extern "C" void fn_801B5948();

void AiState801B5AA0(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    PlayerSetValue4(&a->state, 3);
}

// TODO (not drafted yet, 0x1F8 bytes at 0x801B5B3C): game logic
extern "C" void fn_801B5B3C();

void AiState801B5D34(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    PlayerSetValue4(&a->state, 3);
}

// TODO (not drafted yet, 0x1F8 bytes at 0x801B5DD0): game logic
extern "C" void fn_801B5DD0();

void AiState801B5FC8(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    PlayerSetValue4(&a->state, 1);
}

void AiState801B6064(Actor* a) {
    int id;
    QueryGet(&id, a->handle, 0xDFF3097A);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[9].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_801B7B88;
    a->animFlagA4 = 0x10;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[9].byVariant[v], 0, 0);
    a->animFlagFC = 0x10;
    lbl_804D31C4 = fn_800F7994(id, 0x9D1C245C);
    FxStart((void*)lbl_804D31C4, 0, 0);
}

// TODO (not drafted yet, 0x228 bytes at 0x801B61E4): game logic
extern "C" void fn_801B61E4();

// TODO (not drafted yet, 0xFC bytes at 0x801B640C): game logic
extern "C" void fn_801B640C();

// TODO (not drafted yet, 0x288 bytes at 0x801B6508): game logic
extern "C" void fn_801B6508();

// TODO (not drafted yet, 0x100 bytes at 0x801B6790): game logic
extern "C" void fn_801B6790();

// TODO (not drafted yet, 0x294 bytes at 0x801B6890): game logic
extern "C" void fn_801B6890();

// TODO (not drafted yet, 0xF4 bytes at 0x801B6B24): game logic
extern "C" void fn_801B6B24();

// TODO (not drafted yet, 0x338 bytes at 0x801B6C18): game logic
extern "C" void fn_801B6C18();

// TODO (not drafted yet, 0x1B0 bytes at 0x801B6F50): game logic
extern "C" void fn_801B6F50();

void AiState801B7100(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    PlayerSetValue4(&a->state, 3);
}

void AiState801B719C(Actor* a) {
    int id;
    QueryGet(&id, a->handle, 0xDFF3097A);
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[2].byVariant[v], 0, 0);
    a->animFlagA4 = 0x10;
    a->cb8C = (void*)fn_801B7B88;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[2].byVariant[v], 0, 0);
    a->animFlagFC = 0x10;
    lbl_804D31C0 = fn_800F7994(id, 0xAFEF562B);
    FxStart((void*)lbl_804D31C0, 0, 0);
}

void AiState801B731C(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->flags678 & 0x80) {
        a->flags678 &= ~0x80;
        PlayerSetActionMode(a, 1);
    }
    PlayerSetActionMode(a, 3);
    a->unk7D0 = 0;
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    if ((a->flags31C & 1) && a->unk1D94 == 0) {
        int id1;
        int id2;
        QueryGet(&id1, a->handle, 0x2DA8293C);
        QueryGet(&id2, a->handle, 0xDFF3097A);
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0x0D19E338, fn_800498D0(a), a->unk1D04, a->unk14FC);
        Fn800F7F5C(id1, id2, 0x4E424290, fn_800498D0(a), a->unk1D04, a->unk14FC);
        if (a->state.cur != 3) {
            PlayerSetHeldSlotMode(a, 7);
        } else {
            PlayerSetActionMode(a, 7);
        }
    }
}

void AiState801B7560(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    int anim;
    if (a->flags7B4 & 0x80000) {
        anim = a->set[idx]->a20;
    } else {
        anim = a->set[idx]->a18;
    }
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, anim, lbl_803AAE98[1].byVariant[v], 0, 0);
    a->animFlagA4 = 1;
    AiAnimSet* set = a->set[idx];
    if (a->flags7B4 & 0x80000) {
        anim = set->a24;
    } else {
        anim = set->a1C;
    }
    a->animF0 = fn_800A1BC8(set->a14, anim, lbl_803AAE98[1].byVariant[v], 0, 0);
    a->cbE4 = (void*)fn_801B7B88;
    a->animFlagFC = 1;
    a->unk7D0 = 0;
}

void AiState801B765C(Actor* a) {
    PlayerResetGunTimer(a);
}

void AiState801B767C(Actor* a) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    a->unk7D0 = 1;
    PlayerSetValue4(&a->state, 3);
    if (a->unk328 & 1) {
        if (PlayerInHost(a) != 0) {
            if (!(a->flags7B4 & 0x40000) && !(a->flags7B4 & 0x80000)) {
                fn_80073E80(a);
            }
        }
    }
    if ((a->flags31C & 1) && a->unk1D94 == 0) {
        int id1;
        int id2;
        QueryGet(&id1, a->handle, 0x2DA8293C);
        QueryGet(&id2, a->handle, 0xDFF3097A);
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0x0D19E338, fn_800498D0(a), a->unk1D04, a->unk14FC);
        Fn800F7F5C(id1, id2, 0x4E424290, fn_800498D0(a), a->unk1D04, a->unk14FC);
        if (a->state.cur != 3) {
            PlayerSetHeldSlotMode(a, 7);
        } else {
            PlayerSetActionMode(a, 7);
        }
    }
    if (a->f394 != 0.0f || a->f38C != 0.0f || a->f390 != 0.0f || a->f398 != 0.0f || a->unk328 != 0) {
        PlayerSetActionMode(a, 3);
    }
}

// TODO (not drafted yet, 0x2C4 bytes at 0x801B78C4): game logic
extern "C" void fn_801B78C4();

// TODO (not drafted yet, 0x414 bytes at 0x801B7B88): game logic

int AiState801B7F9C(Actor* a) {
    return lbl_803AAE98[a->state.cur].byVariant[14];
}

void AiTypeMsg_801B7FB8(Actor* a) {
    if (lbl_803AAE98[a->state.cur].msg) {
        lbl_803AAE98[a->state.cur].msg(a);
    }
}

void AiTypeCallback04_801B7FFC(Actor* a) {
    if (lbl_803AAE98[a->state.cur].cb04) {
        lbl_803AAE98[a->state.cur].cb04(a);
    }
}

void AiTypeCallback00_801B8040(Actor* a) {
    if (lbl_803AAE98[a->state.cur].cb00) {
        lbl_803AAE98[a->state.cur].cb00(a);
    }
}

void AiLoadAssets_801B8080() {
    lbl_80496C98[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_80496C98[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_80496C98[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_80496C98[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_80496C98[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_80496C98[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_80496C98[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_80496C98[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_80496C98[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_80496C98[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_80496C98[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_80496C98[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_80496C98[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_80496C98[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_80496C98[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_80496C98[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

// TODO (not drafted yet, 0x1EC bytes at 0x801B829C): game logic
extern "C" void fn_801B829C();

// TODO (not drafted yet, 0x30 bytes at 0x801B8488): game logic
extern "C" void fn_801B8488();

void AiState801B84B8(Actor* a) {
    PlayerResetGunTimer(a);
}

// TODO (not drafted yet, 0x474 bytes at 0x801B84D8): game logic (string: QUERY)
extern "C" void fn_801B84D8();

// TODO (not drafted yet, 0x44 bytes at 0x801B894C): game logic
extern "C" void fn_801B894C();

// TODO (not drafted yet, 0xD0 bytes at 0x801B8990): game logic
extern "C" void fn_801B8990();

// TODO (not drafted yet, 0x560 bytes at 0x801B8A60): game logic
extern "C" void fn_801B8A60();

// TODO (not drafted yet, 0x1AC bytes at 0x801B8FC0): game logic
extern "C" void fn_801B8FC0();

// TODO (not drafted yet, 0x21C bytes at 0x801B916C): game logic
extern "C" void fn_801B916C();

// TODO (not drafted yet, 0xA4 bytes at 0x801B9388): game logic
extern "C" void fn_801B9388();

// TODO (not drafted yet, 0xD0 bytes at 0x801B942C): game logic
extern "C" void fn_801B942C();

// TODO (not drafted yet, 0x94 bytes at 0x801B94FC): game logic
extern "C" void fn_801B94FC();

// TODO (not drafted yet, 0x54 bytes at 0x801B9590): game logic
extern "C" void fn_801B9590();

// TODO (not drafted yet, 0x48 bytes at 0x801B95E4): game logic
extern "C" void fn_801B95E4();

// TODO (not drafted yet, 0x84 bytes at 0x801B962C): game logic
extern "C" void fn_801B962C();

// TODO (not drafted yet, 0xC4 bytes at 0x801B96B0): game logic
extern "C" void fn_801B96B0();

// TODO (not drafted yet, 0x60 bytes at 0x801B9774): game logic
extern "C" void fn_801B9774();

// TODO (not drafted yet, 0x40 bytes at 0x801B97D4): game logic
extern "C" void fn_801B97D4();

// TODO (not drafted yet, 0x90 bytes at 0x801B9814): game logic
extern "C" void fn_801B9814();

// TODO (not drafted yet, 0x40 bytes at 0x801B98A4): game logic
extern "C" void fn_801B98A4();

// TODO (not drafted yet, 0xA8 bytes at 0x801B98E4): game logic
extern "C" void fn_801B98E4();

// TODO (not drafted yet, 0x1C bytes at 0x801B998C): game logic
extern "C" void fn_801B998C();

// TODO (not drafted yet, 0x144 bytes at 0x801B99A8): game logic
extern "C" void fn_801B99A8();

// TODO (not drafted yet, 0x118 bytes at 0x801B9AEC): game logic
extern "C" void fn_801B9AEC();

// TODO (not drafted yet, 0x358 bytes at 0x801B9C04): game logic
extern "C" void fn_801B9C04();

// TODO (not drafted yet, 0x310 bytes at 0x801B9F5C): game logic
extern "C" void fn_801B9F5C();

// TODO (not drafted yet, 0x1B8 bytes at 0x801BA26C): game logic
extern "C" void fn_801BA26C();

// TODO (not drafted yet, 0x1AC bytes at 0x801BA424): game logic
extern "C" void fn_801BA424();

// TODO (not drafted yet, 0x1A4 bytes at 0x801BA5D0): game logic
extern "C" void fn_801BA5D0();

// TODO (not drafted yet, 0x1A4 bytes at 0x801BA774): game logic
extern "C" void fn_801BA774();

// TODO (not drafted yet, 0x1A8 bytes at 0x801BA918): game logic
extern "C" void fn_801BA918();

// TODO (not drafted yet, 0x204 bytes at 0x801BAAC0): game logic
extern "C" void fn_801BAAC0();

// TODO (not drafted yet, 0xEC bytes at 0x801BACC4): game logic
extern "C" void fn_801BACC4();

// TODO (not drafted yet, 0x244 bytes at 0x801BADB0): game logic
extern "C" void fn_801BADB0();

// TODO (not drafted yet, 0x204 bytes at 0x801BAFF4): game logic
extern "C" void fn_801BAFF4();

// TODO (not drafted yet, 0x2E0 bytes at 0x801BB1F8): game logic
extern "C" void fn_801BB1F8();

// TODO (not drafted yet, 0x1C4 bytes at 0x801BB4D8): game logic
extern "C" void fn_801BB4D8();

// TODO (not drafted yet, 0x78C bytes at 0x801BB69C): game logic
extern "C" void fn_801BB69C();

// TODO (not drafted yet, 0x1180 bytes at 0x801BBE28): game logic (string: QUERY | FASSVE])
extern "C" void fn_801BBE28();

// TODO (not drafted yet, 0x1E0 bytes at 0x801BCFA8): game logic
extern "C" void fn_801BCFA8();

// TODO (not drafted yet, 0x40 bytes at 0x801BD188): game logic
extern "C" void fn_801BD188();

// TODO (not drafted yet, 0x3DC bytes at 0x801BD1C8): game logic
extern "C" void fn_801BD1C8();

// TODO (not drafted yet, 0x88 bytes at 0x801BD5A4): game logic
extern "C" void fn_801BD5A4();

// TODO (not drafted yet, 0x68 bytes at 0x801BD62C): game logic
extern "C" void fn_801BD62C();

// TODO (not drafted yet, 0xB8 bytes at 0x801BD694): game logic
extern "C" void fn_801BD694();

// TODO (not drafted yet, 0x88 bytes at 0x801BD74C): game logic
extern "C" void fn_801BD74C();

// TODO (not drafted yet, 0x438 bytes at 0x801BD7D4): game logic
extern "C" void fn_801BD7D4();

// TODO (not drafted yet, 0x30 bytes at 0x801BDC0C): game logic
extern "C" void fn_801BDC0C();

// TODO (not drafted yet, 0x168 bytes at 0x801BDC3C): game logic
extern "C" void fn_801BDC3C();

// TODO (not drafted yet, 0xC8 bytes at 0x801BDDA4): game logic
extern "C" void fn_801BDDA4();

// TODO (not drafted yet, 0x184 bytes at 0x801BDE6C): game logic
extern "C" void fn_801BDE6C();

int AiReturnFalse_801BDFF0() {
    return 0;
}

int AiReturnFalse_801BDFF8() {
    return 0;
}

void AiNoOp_801BE000() {
}

void AiNoOp_801BE004() {
}

// TODO (not drafted yet, 0x4B0 bytes at 0x801BE008): game logic
extern "C" void fn_801BE008();

// TODO (not drafted yet, 0x11C bytes at 0x801BE4B8): game logic
extern "C" void fn_801BE4B8();

// TODO (not drafted yet, 0xBC bytes at 0x801BE5D4): game logic
extern "C" void fn_801BE5D4();

// TODO (not drafted yet, 0x16C bytes at 0x801BE690): game logic
extern "C" void fn_801BE690();

// TODO (not drafted yet, 0x178 bytes at 0x801BE7FC): game logic
extern "C" void fn_801BE7FC();

// TODO (not drafted yet, 0x88 bytes at 0x801BE974): game logic
extern "C" void fn_801BE974();

// TODO (not drafted yet, 0xC0 bytes at 0x801BE9FC): game logic
extern "C" void fn_801BE9FC();

// TODO (not drafted yet, 0x17C bytes at 0x801BEABC): game logic
extern "C" void fn_801BEABC();

// TODO (not drafted yet, 0x14 bytes at 0x801BEC38): game logic
extern "C" void fn_801BEC38();


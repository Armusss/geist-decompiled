// u_80127D80.cpp: FearAIBot bot engine and enemy-character AI behaviour code (0x80127D80-0x80136884).
// Several near-identical per-character-type blocks of state callbacks follow each other here.
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

// Bot engine state (shared .bss block at 0x8039F788).
struct BotEngine {
    int maxBots;     // 0x00
    int count;       // 0x04
    int slot[7];     // 0x08
    int unk24;       // 0x24
};
extern BotEngine lbl_8039F788;

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
};

// The actor a character is currently targeting / carried by.
struct AiTarget {
    u8 pad00[0x38];
    u32 handle;      // 0x38: type in the top byte
    u8 pad3C[0x270 - 0x3C];
    u8 conditions[4]; // 0x270
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
    u8 pad7C[0x8C - 0x7C];
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
    u8 pad120[0x638 - 0x120];
    int unk638;      // 0x638
    u8 pad63C[0x73C - 0x63C];
    int variant;     // 0x73C
    u8 pad740[0x7A4 - 0x740];
    u8 state7A4[0x10]; // 0x7A4
    u32 flags7B4;    // 0x7B4
    u8 pad7B8[0x7D0 - 0x7B8];
    int unk7D0;      // 0x7D0
    u8 pad7D4[0x14FC - 0x7D4];
    float unk14FC;   // 0x14FC
    u8 pad1500[0x1D04 - 0x1500];
    u8 unk1D04[0x26B0 - 0x1D04];
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
extern "C" int fn_8012AF68(Actor* a);
extern int lbl_804D2E44;
extern int lbl_804D2E40;
// Animation lookup table: one row per animation kind, one slot per character variant (data of another unit).
struct AnimRow {
    int head[3];
    int byVariant[15];
};
extern AnimRow lbl_8039F7C8[];
extern int lbl_8046D1F0[];
extern "C" void fn_8012B4C8(Actor* a);

// TODO (not drafted yet, 0x590 bytes at 0x80127D80): game logic
extern "C" void fn_80127D80();

// TODO (not drafted yet, 0x620 bytes at 0x80128310): game logic
extern "C" void fn_80128310();

int BotQuerySet(int target, u32 hash, int value) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    return SendMsg(target, 0x41D, &m);
}

// TODO (not drafted yet, 0x2BC bytes at 0x801289D0): game logic (string: FearAIBot : Starting up bots)
extern "C" void fn_801289D0();

// TODO (not drafted yet, 0xF0 bytes at 0x80128C8C): game logic (string: FearAIBot : Terminating bots | FearAIBot : Removing last bot | FearAIBot : No more bots to remove!!)
extern "C" void fn_80128C8C();

void BotEngineInit() {
    fn_800872BC(0x40000, "FearAIBot : Initializing bot engine");
    lbl_8039F788.count = 0;
    lbl_8039F788.slot[0] = -1;
    lbl_8039F788.slot[1] = -1;
    lbl_8039F788.slot[2] = -1;
    lbl_8039F788.slot[3] = -1;
    lbl_8039F788.slot[4] = -1;
    lbl_8039F788.slot[5] = -1;
    lbl_8039F788.slot[6] = -1;
}

int BotEngineMsg(int self, int msg, void* data) {
    int* d = (int*)data;
    switch (msg) {
    case 0x41E:
        switch (d[1]) {
        case -0x21E2AACA:
            d[3] = lbl_8039F788.count;
            return 1;
        case -0x6B56663: {
            int i = d[3];
            if (i >= 0 && i < lbl_8039F788.count) {
                d[3] = lbl_8039F788.slot[i];
            } else {
                d[3] = -1;
            }
            return 1;
        }
        }
        return 0;
    case 0x41D:
        switch (d[1]) {
        case -0x21E2AACA:
            lbl_8039F788.maxBots = d[3];
            return 1;
        }
        return 0;
    }
    return 0;
}

void AiState80128EC8(Actor* a, float dt) {
    a->unk638 = 0;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->animC4 == -1) {
        if (a->unk7D0 != 0) {
            PlayerSetValue4(a->state7A4, 3);
        } else {
            PlayerSetValue4(a->state7A4, 1);
        }
    } else {
        PlayerSetValue4(a->state7A4, 0x16);
    }
}

void AiState80128F64(Actor* a) {
    a->cb8C = 0;
    if (a->animC4 != -1) {
        a->anim98 = a->animC4;
    }
    if (a->anim11C == -1) {
        return;
    }
    a->animF0 = a->anim11C;
}

void AiState80128F90(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    PlayerSetValue4(a->state7A4, 0x15);
    if (a->unk279C == -1 || fn_80175EFC(a->unk279C) != 0) {
        PlayerSetValue4(a->state7A4, 3);
        fn_80155A84(a->sub26E0, a->target, 1);
    }
}

void AiState80129010(Actor* a) {
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
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_8039F7C8[21].byVariant[v], ride, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 0x19;
    a->animF0 = fn_800A1BC8(set->a14, set->a1C, lbl_8039F7C8[21].byVariant[v], ride, 0);
    a->animFlagFC = 0x19;
    PlayerSetValue4(a->state7A4, 0x15);
}

void AiState8012910C(Actor* a, float dt) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    PlayerSetValue4(a->state7A4, 2);
    PlayerInHost(a);
    Fn800F7DEC(id1, id2, 0x20EE36, fn_800498D0(a), 0);
    DashUpdate(a);
}

void AiState8012928C(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->hold = 0;
    a->flags &= ~0x20000;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[20].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[20].byVariant[v], 0, 0);
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

void AiState801293FC(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    if (a->flags7B4 & 1) {
        a->flags7B4 &= ~1;
    } else {
        a->flags |= 0x20000;
        a->hold = 1;
    }
}

void AiState80129458(Actor* a) {
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
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_8039F7C8[19].byVariant[v], ride, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 0x19;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[19].byVariant[v], 0, 0);
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

void AiState801295B8(Actor* a) {
    fn_80155A84(a->sub26E0, a->target, 1);
}

void AiState801295E4(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    a->flags |= 0x20000;
    a->hold = 1;
    PlayerSetValue4(a->state7A4, 0x13);
}

void AiState80129640(Actor* a) {
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
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_8039F7C8[18].byVariant[v], ride, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 0x59;
    AiAnimSet* set2 = a->set[idx];
    a->animF0 = fn_800A1BC8(set2->a14, set2->a1C, lbl_8039F7C8[18].byVariant[v], 0, 0);
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

void AiState8012979C(Actor* a) {
    a->flags &= ~0x40000;
}

void AiState801297AC(Actor* a, float dt) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(a, dt);
}

void AiState80129804(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    PlayerResetGunTimer(a);
    a->flags |= 0x40000;
    fn_80048BA8(a, a->unk1D04, 1);
    Fn800F7F5C(id1, id2, 0xF829DB93, fn_800498D0(a), a->unk1D04, a->unk14FC);
    fn_80050EB0(a);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[13].byVariant[v], 0, 0);
    a->animFlagA4 = 0x50;
    int n;
    if (a->unk26B0 == -1) {
        n = 8;
    } else {
        int bone = ModelBoneName(a->model);
        n = ModelBoneNodeChain(a->model, bone, lbl_8046D1F0, 0x10);
        if (n == -1) {
            n = 8;
        }
    }
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[13].byVariant[v], n << 1, 0);
    a->cbE4 = (void*)fn_8012B4C8;
    a->animFlagFC = 0x50;
}

void AiState80129A18(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(a->state7A4, 3);
}

void AiState80129A84(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    fn_80048BA8(a, a->unk1D04, 1);
    Fn800F7F5C(id1, id2, 0xBC78F550, fn_800498D0(a), a->unk1D04, a->unk14FC);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[11].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 0x11;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[11].byVariant[v], 0, 0);
    int r = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[3].byVariant[v], 0, 0);
    if (a->animF0 == r) {
        a->animFlagFC = 1;
    } else {
        a->animFlagFC = 0x11;
    }
}

void AiState80129C7C(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    fn_8012AF68(a);
    PlayerSetValue4(a->state7A4, 3);
}

void AiState80129CF0(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    fn_80048BA8(a, a->unk1D04, 1);
    Fn800F7F5C(id1, id2, 0x8FDD7599, fn_800498D0(a), a->unk1D04, a->unk14FC);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[10].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 0x11;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[10].byVariant[v], 0, 0);
    int r = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[3].byVariant[v], 0, 0);
    if (a->animF0 == r) {
        a->animFlagFC = 1;
    } else {
        a->animFlagFC = 0x11;
    }
}

void AiState80129EE8(Actor* a) {
    ActorPlayAnim(0x2000000, 0xD39F69B4, fn_800498D0(a));
}

void AiState80129F1C(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(a->state7A4, 1);
}

void AiState80129F88(Actor* a) {
    int id;
    QueryGet(&id, a->handle, 0xDFF3097A);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[9].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 0x10;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[9].byVariant[v], 0, 0);
    a->animFlagFC = 0x10;
    lbl_804D2E44 = fn_800F7994(id, 0x9D1C245C);
    FxStart((void*)lbl_804D2E44, 0, 0);
}

void AiState8012A0C8(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    PlayerSetValue4(a->state7A4, 3);
    DashUpdate(a);
}

void AiState8012A134(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[8].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 0x11;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[8].byVariant[v], 0, 0);
    a->animFlagFC = 0x11;
}

void AiState8012A1E8(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    PlayerSetValue4(a->state7A4, 10);
    DashUpdate(a);
    fn_8012AF68(a);
}

void AiState8012A25C(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[5].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 0x11;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[5].byVariant[v], 0, 0);
    a->animFlagFC = 0x11;
    ActorPlayAnim(a->handle, 0xFB38FB7A, a->handle);
}

// TODO (not drafted yet, 0x264 bytes at 0x8012A324): game logic
extern "C" void fn_8012A324();

void AiState8012A588(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[3].byVariant[v], 0, 0);
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[3].byVariant[v], 0, 0);
    a->animFlagFC = 1;
    DashUpdate(a);
}

void AiState8012A644(Actor* a) {
    ActorPlayAnim(0x2000000, 0x8B3A27A3, fn_800498D0(a));
}

void AiState8012A678(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(a->state7A4, 3);
}

void AiState8012A6E4(Actor* a) {
    int id;
    QueryGet(&id, a->handle, 0xDFF3097A);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[2].byVariant[v], 0, 0);
    a->animFlagA4 = 0x10;
    a->cb8C = (void*)fn_8012B4C8;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[2].byVariant[v], 0, 0);
    a->animFlagFC = 0x10;
    lbl_804D2E40 = fn_800F7994(id, 0xAFEF562B);
    FxStart((void*)lbl_804D2E40, 0, 0);
}

// TODO (not drafted yet, 0x200 bytes at 0x8012A824): game logic
extern "C" void fn_8012A824();

// TODO (not drafted yet, 0x1AC bytes at 0x8012AA24): game logic
extern "C" void fn_8012AA24();

void AiState8012ABD0(Actor* a) {
    PlayerResetGunTimer(a);
}

// TODO (not drafted yet, 0x20C bytes at 0x8012ABF0): game logic
extern "C" void fn_8012ABF0();

// TODO (not drafted yet, 0x16C bytes at 0x8012ADFC): game logic
extern "C" void fn_8012ADFC();

// TODO (not drafted yet, 0x28C bytes at 0x8012AF68): game logic

// TODO (not drafted yet, 0x2D4 bytes at 0x8012B1F4): game logic
extern "C" void fn_8012B1F4();

// TODO (not drafted yet, 0x43C bytes at 0x8012B4C8): game logic

// TODO (not drafted yet, 0x1C bytes at 0x8012B904): game logic
extern "C" void fn_8012B904();

// TODO (not drafted yet, 0x44 bytes at 0x8012B920): game logic
extern "C" void fn_8012B920();

// TODO (not drafted yet, 0x44 bytes at 0x8012B964): game logic
extern "C" void fn_8012B964();

// TODO (not drafted yet, 0x40 bytes at 0x8012B9A8): game logic
extern "C" void fn_8012B9A8();

// TODO (not drafted yet, 0x21C bytes at 0x8012B9E8): game logic
extern "C" void fn_8012B9E8();

// TODO (not drafted yet, 0x4 bytes at 0x8012BC04): game logic
extern "C" void fn_8012BC04();

// TODO (not drafted yet, 0x1B8 bytes at 0x8012BC08): game logic
extern "C" void fn_8012BC08();

// TODO (not drafted yet, 0x18C bytes at 0x8012BDC0): game logic
extern "C" void fn_8012BDC0();

// TODO (not drafted yet, 0x4A0 bytes at 0x8012BF4C): game logic
extern "C" void fn_8012BF4C();

// TODO (not drafted yet, 0x38 bytes at 0x8012C3EC): game logic
extern "C" void fn_8012C3EC();

// TODO (not drafted yet, 0x17C bytes at 0x8012C424): game logic (string: Proximity Mine)
extern "C" void fn_8012C424();

// TODO (not drafted yet, 0x70 bytes at 0x8012C5A0): game logic
extern "C" void fn_8012C5A0();

// TODO (not drafted yet, 0x70 bytes at 0x8012C610): game logic
extern "C" void fn_8012C610();

// TODO (not drafted yet, 0x134 bytes at 0x8012C680): game logic
extern "C" void fn_8012C680();

// TODO (not drafted yet, 0x1B4 bytes at 0x8012C7B4): game logic
extern "C" void fn_8012C7B4();

// TODO (not drafted yet, 0xC4 bytes at 0x8012C968): game logic
extern "C" void fn_8012C968();

// TODO (not drafted yet, 0x34 bytes at 0x8012CA2C): game logic
extern "C" void fn_8012CA2C();

// TODO (not drafted yet, 0x100 bytes at 0x8012CA60): game logic
extern "C" void fn_8012CA60();

// TODO (not drafted yet, 0x748 bytes at 0x8012CB60): game logic
extern "C" void fn_8012CB60();

// TODO (not drafted yet, 0x598 bytes at 0x8012D2A8): game logic
extern "C" void fn_8012D2A8();

// TODO (not drafted yet, 0xD4 bytes at 0x8012D840): game logic
extern "C" void fn_8012D840();

// TODO (not drafted yet, 0x1D4 bytes at 0x8012D914): game logic
extern "C" void fn_8012D914();

// TODO (not drafted yet, 0x180 bytes at 0x8012DAE8): game logic
extern "C" void fn_8012DAE8();

// TODO (not drafted yet, 0x48C bytes at 0x8012DC68): game logic
extern "C" void fn_8012DC68();

// TODO (not drafted yet, 0x28 bytes at 0x8012E0F4): game logic
extern "C" void fn_8012E0F4();

// TODO (not drafted yet, 0x3F8 bytes at 0x8012E11C): game logic
extern "C" void fn_8012E11C();

// TODO (not drafted yet, 0x94 bytes at 0x8012E514): game logic
extern "C" void fn_8012E514();

// TODO (not drafted yet, 0xA0 bytes at 0x8012E5A8): game logic
extern "C" void fn_8012E5A8();

// TODO (not drafted yet, 0x378 bytes at 0x8012E648): game logic
extern "C" void fn_8012E648();

// TODO (not drafted yet, 0x27C bytes at 0x8012E9C0): game logic
extern "C" void fn_8012E9C0();

// TODO (not drafted yet, 0x5D4 bytes at 0x8012EC3C): game logic
extern "C" void fn_8012EC3C();

// TODO (not drafted yet, 0x140 bytes at 0x8012F210): game logic
extern "C" void fn_8012F210();

// TODO (not drafted yet, 0x804 bytes at 0x8012F350): game logic
extern "C" void fn_8012F350();

// TODO (not drafted yet, 0x478 bytes at 0x8012FB54): game logic
extern "C" void fn_8012FB54();

// TODO (not drafted yet, 0x39C bytes at 0x8012FFCC): game logic
extern "C" void fn_8012FFCC();

// TODO (not drafted yet, 0x76C bytes at 0x80130368): game logic
extern "C" void fn_80130368();

// TODO (not drafted yet, 0xB0 bytes at 0x80130AD4): game logic
extern "C" void fn_80130AD4();

// TODO (not drafted yet, 0x70 bytes at 0x80130B84): game logic
extern "C" void fn_80130B84();

// TODO (not drafted yet, 0x40 bytes at 0x80130BF4): game logic
extern "C" void fn_80130BF4();

// TODO (not drafted yet, 0x8 bytes at 0x80130C34): game logic
extern "C" void fn_80130C34();

// TODO (not drafted yet, 0xE0 bytes at 0x80130C3C): game logic
extern "C" void fn_80130C3C();

// TODO (not drafted yet, 0xB0 bytes at 0x80130D1C): game logic
extern "C" void fn_80130D1C();

// TODO (not drafted yet, 0xD8 bytes at 0x80130DCC): game logic
extern "C" void fn_80130DCC();

// TODO (not drafted yet, 0xF4 bytes at 0x80130EA4): game logic
extern "C" void fn_80130EA4();

// TODO (not drafted yet, 0x6C bytes at 0x80130F98): game logic
extern "C" void fn_80130F98();

// TODO (not drafted yet, 0x114 bytes at 0x80131004): game logic
extern "C" void fn_80131004();

// TODO (not drafted yet, 0x1C8 bytes at 0x80131118): game logic
extern "C" void fn_80131118();

// TODO (not drafted yet, 0x2CC bytes at 0x801312E0): game logic
extern "C" void fn_801312E0();

// TODO (not drafted yet, 0x54 bytes at 0x801315AC): game logic
extern "C" void fn_801315AC();

// TODO (not drafted yet, 0x104 bytes at 0x80131600): game logic
extern "C" void fn_80131600();

// TODO (not drafted yet, 0xC bytes at 0x80131704): game logic
extern "C" void fn_80131704();

// TODO (not drafted yet, 0x88 bytes at 0x80131710): game logic
extern "C" void fn_80131710();

// TODO (not drafted yet, 0x6C0 bytes at 0x80131798): game logic (string: ShotgunShell)
extern "C" void fn_80131798();

// TODO (not drafted yet, 0x28 bytes at 0x80131E58): game logic
extern "C" void fn_80131E58();

// TODO (not drafted yet, 0x384 bytes at 0x80131E80): game logic
extern "C" void fn_80131E80();

// TODO (not drafted yet, 0x168 bytes at 0x80132204): game logic
extern "C" void fn_80132204();

// TODO (not drafted yet, 0x9D4 bytes at 0x8013236C): game logic
extern "C" void fn_8013236C();

// TODO (not drafted yet, 0x40 bytes at 0x80132D40): game logic
extern "C" void fn_80132D40();

// TODO (not drafted yet, 0x158 bytes at 0x80132D80): game logic (string: hotgun)
extern "C" void fn_80132D80();

// TODO (not drafted yet, 0x24 bytes at 0x80132ED8): game logic
extern "C" void fn_80132ED8();

// TODO (not drafted yet, 0xC0 bytes at 0x80132EFC): game logic
extern "C" void fn_80132EFC();

// TODO (not drafted yet, 0x8 bytes at 0x80132FBC): game logic
extern "C" void fn_80132FBC();

// TODO (not drafted yet, 0xDC bytes at 0x80132FC4): game logic
extern "C" void fn_80132FC4();

// TODO (not drafted yet, 0x444 bytes at 0x801330A0): game logic
extern "C" void fn_801330A0();

// TODO (not drafted yet, 0x90 bytes at 0x801334E4): game logic
extern "C" void fn_801334E4();

// TODO (not drafted yet, 0xC0 bytes at 0x80133574): game logic
extern "C" void fn_80133574();

// TODO (not drafted yet, 0x8 bytes at 0x80133634): game logic
extern "C" void fn_80133634();

// TODO (not drafted yet, 0x8 bytes at 0x8013363C): game logic
extern "C" void fn_8013363C();

// TODO (not drafted yet, 0x220 bytes at 0x80133644): game logic
extern "C" void fn_80133644();

// TODO (not drafted yet, 0x2C0 bytes at 0x80133864): game logic
extern "C" void fn_80133864();

// TODO (not drafted yet, 0x24 bytes at 0x80133B24): game logic
extern "C" void fn_80133B24();

// TODO (not drafted yet, 0x8 bytes at 0x80133B48): game logic
extern "C" void fn_80133B48();

// TODO (not drafted yet, 0x8 bytes at 0x80133B50): game logic
extern "C" void fn_80133B50();

// TODO (not drafted yet, 0x30 bytes at 0x80133B58): game logic
extern "C" void fn_80133B58();

// TODO (not drafted yet, 0x8 bytes at 0x80133B88): game logic
extern "C" void fn_80133B88();

// TODO (not drafted yet, 0x84 bytes at 0x80133B90): game logic
extern "C" void fn_80133B90();

// TODO (not drafted yet, 0x8 bytes at 0x80133C14): game logic
extern "C" void fn_80133C14();

// TODO (not drafted yet, 0x8 bytes at 0x80133C1C): game logic
extern "C" void fn_80133C1C();

// TODO (not drafted yet, 0x98 bytes at 0x80133C24): game logic
extern "C" void fn_80133C24();

// TODO (not drafted yet, 0x6C bytes at 0x80133CBC): game logic
extern "C" void fn_80133CBC();

// TODO (not drafted yet, 0xC4 bytes at 0x80133D28): game logic
extern "C" void fn_80133D28();

// TODO (not drafted yet, 0xE4 bytes at 0x80133DEC): game logic
extern "C" void fn_80133DEC();

// TODO (not drafted yet, 0x18C bytes at 0x80133ED0): game logic
extern "C" void fn_80133ED0();

// TODO (not drafted yet, 0x2C bytes at 0x8013405C): game logic
extern "C" void fn_8013405C();

// TODO (not drafted yet, 0xDC bytes at 0x80134088): game logic
extern "C" void fn_80134088();

// TODO (not drafted yet, 0x9C bytes at 0x80134164): game logic
extern "C" void fn_80134164();

// TODO (not drafted yet, 0x2C bytes at 0x80134200): game logic
extern "C" void fn_80134200();

// TODO (not drafted yet, 0x80 bytes at 0x8013422C): game logic
extern "C" void fn_8013422C();

// TODO (not drafted yet, 0xFC bytes at 0x801342AC): game logic
extern "C" void fn_801342AC();

// TODO (not drafted yet, 0xA0 bytes at 0x801343A8): game logic
extern "C" void fn_801343A8();

// TODO (not drafted yet, 0x170 bytes at 0x80134448): game logic
extern "C" void fn_80134448();

// TODO (not drafted yet, 0x5C bytes at 0x801345B8): game logic
extern "C" void fn_801345B8();

// TODO (not drafted yet, 0x160 bytes at 0x80134614): game logic
extern "C" void fn_80134614();

// TODO (not drafted yet, 0x2C bytes at 0x80134774): game logic
extern "C" void fn_80134774();

// TODO (not drafted yet, 0x5C bytes at 0x801347A0): game logic
extern "C" void fn_801347A0();

// TODO (not drafted yet, 0x15C bytes at 0x801347FC): game logic
extern "C" void fn_801347FC();

// TODO (not drafted yet, 0x10 bytes at 0x80134958): game logic
extern "C" void fn_80134958();

// TODO (not drafted yet, 0x58 bytes at 0x80134968): game logic
extern "C" void fn_80134968();

// TODO (not drafted yet, 0x110 bytes at 0x801349C0): game logic
extern "C" void fn_801349C0();

// TODO (not drafted yet, 0x6C bytes at 0x80134AD0): game logic
extern "C" void fn_80134AD0();

// TODO (not drafted yet, 0x1B8 bytes at 0x80134B3C): game logic
extern "C" void fn_80134B3C();

// TODO (not drafted yet, 0x6C bytes at 0x80134CF4): game logic
extern "C" void fn_80134CF4();

// TODO (not drafted yet, 0x1F8 bytes at 0x80134D60): game logic
extern "C" void fn_80134D60();

// TODO (not drafted yet, 0x7C bytes at 0x80134F58): game logic
extern "C" void fn_80134F58();

// TODO (not drafted yet, 0xFC bytes at 0x80134FD4): game logic
extern "C" void fn_80134FD4();

// TODO (not drafted yet, 0x6C bytes at 0x801350D0): game logic
extern "C" void fn_801350D0();

// TODO (not drafted yet, 0x1F8 bytes at 0x8013513C): game logic
extern "C" void fn_8013513C();

// TODO (not drafted yet, 0x34 bytes at 0x80135334): game logic
extern "C" void fn_80135334();

// TODO (not drafted yet, 0x6C bytes at 0x80135368): game logic
extern "C" void fn_80135368();

// TODO (not drafted yet, 0x140 bytes at 0x801353D4): game logic
extern "C" void fn_801353D4();

// TODO (not drafted yet, 0x90 bytes at 0x80135514): game logic
extern "C" void fn_80135514();

// TODO (not drafted yet, 0xB4 bytes at 0x801355A4): game logic
extern "C" void fn_801355A4();

// TODO (not drafted yet, 0x90 bytes at 0x80135658): game logic
extern "C" void fn_80135658();

// TODO (not drafted yet, 0xB4 bytes at 0x801356E8): game logic
extern "C" void fn_801356E8();

// TODO (not drafted yet, 0x124 bytes at 0x8013579C): game logic
extern "C" void fn_8013579C();

// TODO (not drafted yet, 0xBC bytes at 0x801358C0): game logic
extern "C" void fn_801358C0();

// TODO (not drafted yet, 0x34 bytes at 0x8013597C): game logic
extern "C" void fn_8013597C();

// TODO (not drafted yet, 0x6C bytes at 0x801359B0): game logic
extern "C" void fn_801359B0();

// TODO (not drafted yet, 0x140 bytes at 0x80135A1C): game logic
extern "C" void fn_80135A1C();

// TODO (not drafted yet, 0xE8 bytes at 0x80135B5C): game logic
extern "C" void fn_80135B5C();

// TODO (not drafted yet, 0xBC bytes at 0x80135C44): game logic
extern "C" void fn_80135C44();

// TODO (not drafted yet, 0x20 bytes at 0x80135D00): game logic
extern "C" void fn_80135D00();

// TODO (not drafted yet, 0xE0 bytes at 0x80135D20): game logic
extern "C" void fn_80135D20();

// TODO (not drafted yet, 0x16C bytes at 0x80135E00): game logic
extern "C" void fn_80135E00();

// TODO (not drafted yet, 0x338 bytes at 0x80135F6C): game logic
extern "C" void fn_80135F6C();

// TODO (not drafted yet, 0x2F0 bytes at 0x801362A4): game logic
extern "C" void fn_801362A4();

// TODO (not drafted yet, 0x2F0 bytes at 0x80136594): game logic
extern "C" void fn_80136594();


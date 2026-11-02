// u_80127D80.cpp: FearAIBot bot engine and enemy-character AI behaviour code (0x80127D80-0x80136884).
// Several near-identical per-character-type blocks of state callbacks follow each other here.
// Only the functions drafted so far have bodies; every other function is declared with a TODO comment.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    u32 h;
    int i;
    if (s == 0) {
        h = 0;
    } else {
        h = 0;
        for (i = 0; s[i] != 0; i++) {
            h = h * 31 + (u8)toupper(s[i]);
        }
        h = h % 0xFFFFFFFF;
    }
    return h;
}

// Description handed to the bot spawner (descriptive guess).
struct BotSpawn {
    u32 hash;        // 0x00
    int a;           // 0x04
    int b;           // 0x08
    int unkC[3];     // 0x0C
    float pos[3];    // 0x18
    u8 matrix[0x10]; // 0x24
};

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

// Current / next state of a character (a Player-style state record).
struct AiStateRec {
    int cur;         // 0x00
    int next;        // 0x04
    int unk8;        // 0x08
    int unkC;        // 0x0C
};

// Enemy character actor (descriptive guess; only the fields used in this file).
struct Actor {
    u8 pad00[0x24];
    int px;          // 0x24
    int py;          // 0x28
    int pz;          // 0x2C
    u8 pad30[0x38 - 0x30];
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
    u8 pad31C[4];
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
    u8 pad7DC[0x14FC - 0x7DC];
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
void PlayerSetHeldSlotMode(void* a, int mode);
extern "C" int fn_8012B1F4(Actor* a);
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
extern "C" int rand();
extern "C" void fn_80155A84(void* a, void* b, int c);
extern "C" int fn_800A1BC8(int a, int b, int c, int d, int e);
extern "C" void* fn_800A384C(void* t);
extern "C" int fn_800498D0(Actor* a);
extern "C" void fn_80048BA8(Actor* a, void* b, int c);
extern "C" void fn_80050EB0(Actor* a);
extern "C" int fn_800F7994(int a, u32 b);
extern "C" int FxStart(void* fx, int a, int b);
extern "C" int fn_8012AF68(Actor* a);

// Weapon-object behaviour support (descriptive guesses; fields used by this file).
struct FoeData {
    int mode;        // 0x00
    int h4;          // 0x04
    int f8;          // 0x08
    float fC;        // 0x0C
    int f10;         // 0x10
    int f14;         // 0x14
    int f18;         // 0x18
    float f1C;       // 0x1C
    int f20;         // 0x20
    float f24;       // 0x24
    int f28;         // 0x28
    int f2C;         // 0x2C
    int h30;         // 0x30
    int f34;         // 0x34
    int f38;         // 0x38
    int f3C;         // 0x3C
};
struct FoeBeh {
    void (*notify)(u32 hash, void* obj); // 0x00
    u8 pad04[0xBC - 0x04];
    int fBC;         // 0xBC
    u8 padC0[0xD8 - 0xC0];
    void* cbD8;      // 0xD8
    u8 padDC[4];
    FoeData* data;   // 0xE0
};
struct FoeObj {
    u8 pad00[0x270];
    u8 cond[0x18];   // 0x270
    int f288;        // 0x288
    u8 pad28C[0x894 - 0x28C];
    u8 pad894[4];    // 0x894
};
extern "C" int fn_80162940(void* a, int b);
extern "C" void fn_800B6CAC(void* o, void* cb);
extern "C" void fn_800B6D04(void* o, void* cb);
extern "C" void fn_800BBCA8(void* o);
void ConditionUpdateBit(void* c, int bit, int on);
void RangeResetPending(void* c);
void FollowSetTargetObj(void* c, void* t);
extern "C" void fn_800BC56C(void* o, void* t);
extern "C" int fn_800BAF40(void* o);
extern "C" int fn_800B26B8(void* o);
extern "C" void fn_800B1AB8(float f, void* o, int r, int n);
extern "C" void fn_80131798(u32 a, int b, int* c, int d);
int Fn80175D60(u32 a);
void* VAlloc(int n);
struct FoeState {
    int (*enter)(FoeObj*, FoeBeh*);
    int (*unk4)();
    int (*unk8)();
};
extern FoeState lbl_8039FF90[];
extern "C" int fn_800B93DC(int* list, int a, int* out, int b, int c);
extern "C" int fn_800A2DE8(void* desc, int a, int b, int c);
extern "C" void fn_800A3990(int id);
extern "C" void fn_800A31A0();
extern "C" void fn_8016D6A8(void* m);
int MinigameAddBot();
void MinigameRemovePlayer(int id);
extern "C" void fn_80136594(Actor* a);
extern "C" void fn_801362A4(Actor* a);
extern int lbl_804D2E44;
extern int lbl_804D2E40;
extern int lbl_804D2E6C;
extern int lbl_804D2E68;

// Proximity mine slots (24 pointers at 0x8039FF00, in-use bits at 0x804D2E58).
struct MineSlot {
    int state;       // 0x00
    int handle;      // 0x04
    u32 f08;         // 0x08
    u8 pad0C[4];
    u32 f10;         // 0x10
    int f14;         // 0x14
    int f18;         // 0x18
    float f1C;       // 0x1C
    u8 pad20[0xC];
    u8 flag2C80 : 1; // 0x2C top bit
    u8 flags2C : 7;
};
struct MineMsg {
    u32 size;
    int on;
    int target;
    u32 event;
};
extern MineSlot* lbl_8039FF00[24];
extern u32 lbl_804D2E58;
inline int MineSlotUsed(int i) {
    return (1 << (i & 0x1F)) & (&lbl_804D2E58)[i / 32];
}
void VFree(void* p);
void ListRemoveFree(void* a, void** list, u32* bits, int n);
extern "C" void fn_800F86A0(int h);
// Animation lookup table: one row per animation kind, one slot per character variant (data of another unit).
struct AnimRow {
    void (*cb00)(Actor*);  // 0x00
    void (*cb04)(Actor*);  // 0x04
    void (*msg)(Actor*);   // 0x08
    int byVariant[15];     // 0x0C
};
extern AnimRow lbl_8039F7C8[];
extern AnimRow lbl_803A00D0[];
int PlayerGunAction(int action, void* p);
extern int lbl_8046D1F0[16];
u32 AssetFind(u32 table, u32 key);
extern "C" int fn_800F7D28(int a, int b);
extern "C" int fn_8012B4C8(int action, void* p);

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

void BotEngineStart(const char** names) {
    fn_800872BC(0x40000, "FearAIBot : Starting up bots");
    BotEngine* e = &lbl_8039F788;
    for (int n = 0; n < e->maxBots; n++) {
        BotSpawn spawn;
        int target;
        Actor* found[8];
        u32 h = StrHash(names[0]);
        if (e->count != 7) {
            spawn.hash = h;
            spawn.a = 0;
            spawn.b = 0;
            spawn.pos[0] = 0.0f;
            spawn.pos[1] = 0.0f;
            spawn.pos[2] = 0.0f;
            fn_8016D6A8(spawn.matrix);
            int r = fn_800B93DC(&e->unk24, 7, &target, 1, 1);
            if (r > 0 && target != -1 && SendMsg(target, 0x400, found) != 0) {
                spawn.pos[0] = (float)found[0]->px;
                spawn.pos[1] = (float)found[0]->py;
                spawn.pos[2] = (float)found[0]->pz;
            }
            int bot = fn_800A2DE8(&spawn, 0, 0, 0);
            if (bot != -1) {
                if (MinigameAddBot() == -1) {
                    fn_800A3990(bot);
                    fn_800A31A0();
                } else {
                    e->slot[e->count++] = bot;
                    int tag;
                    QueryGet(&tag, 0x4B000000, 0xC343772);
                    BotQuerySet(bot, 0xC343772, tag);
                }
            }
        }
    }
}

void BotEngineShutdown() {
    fn_800872BC(0x40000, "FearAIBot : Terminating bots");
    while (lbl_8039F788.count > 0) {
        fn_800872BC(0x40000, "FearAIBot : Removing last bot");
        if (lbl_8039F788.count == 0) {
            fn_800872BC(0x40000, "FearAIBot : No more bots to remove!!");
        } else {
            int i = lbl_8039F788.count - 1;
            int id = lbl_8039F788.slot[i];
            MinigameRemovePlayer(id);
            fn_800A3990(id);
            fn_800A31A0();
            lbl_8039F788.count--;
            lbl_8039F788.slot[i] = -1;
        }
    }
}

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
            PlayerSetValue4(&a->state, 3);
        } else {
            PlayerSetValue4(&a->state, 1);
        }
    } else {
        PlayerSetValue4(&a->state, 0x16);
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
    PlayerSetValue4(&a->state, 0x15);
    if (a->unk279C == -1 || fn_80175EFC(a->unk279C) != 0) {
        PlayerSetValue4(&a->state, 3);
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
    PlayerSetValue4(&a->state, 0x15);
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
    PlayerSetValue4(&a->state, 2);
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
    PlayerSetValue4(&a->state, 0x13);
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
    PlayerSetValue4(&a->state, 3);
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
    PlayerSetValue4(&a->state, 3);
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
    PlayerSetValue4(&a->state, 1);
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
    PlayerSetValue4(&a->state, 3);
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
    PlayerSetValue4(&a->state, 10);
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
    PlayerSetValue4(&a->state, 3);
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

void AiState8012A824(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->flags678 & 0x80) {
        a->flags678 &= ~0x80;
        PlayerSetActionMode(a, 1);
    }
    PlayerSetValue4(&a->state, 1);
    a->unk7D0 = 0;
    DashUpdate(a);
    if (fn_8012B1F4(a) == 0 && fn_8012AF68(a) == 0) {
        int id1;
        int id2;
        QueryGet(&id1, a->handle, 0x2DA8293C);
        QueryGet(&id2, a->handle, 0xDFF3097A);
        if (a->flags320 & 1) {
            PlayerResetGunTimer(a);
            int r = Fn800F7DEC(id1, id2, 0x20EE36, fn_800498D0(a), 0);
            if (a->unk7D0 == 0) {
                if (r != 0x11D2C025) {
                    PlayerSetHeldSlotMode(a, 2);
                }
            } else {
                PlayerSetHeldSlotMode(a, 9);
            }
            fn_800699A8(a->sub2F4);
        }
    }
}

void AiState8012AA24(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    Fn800F7DEC(id1, id2, 0x20EE36, fn_800498D0(a), 0);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[1].byVariant[v], 0, 0);
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[1].byVariant[v], 0, 0);
    a->cbE4 = (void*)fn_8012B4C8;
    a->animFlagFC = 1;
    a->unk7D0 = 0;
}

void AiState8012ABD0(Actor* a) {
    PlayerResetGunTimer(a);
}

void AiState8012ABF0(Actor* a) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    a->unk7D0 = 1;
    PlayerSetValue4(&a->state, 3);
    DashUpdate(a);
    if (fn_8012B1F4(a) == 0 && fn_8012AF68(a) == 0) {
        int id1;
        int id2;
        QueryGet(&id1, a->handle, 0x2DA8293C);
        QueryGet(&id2, a->handle, 0xDFF3097A);
        if (a->flags320 & 1) {
            PlayerResetGunTimer(a);
            int r = Fn800F7DEC(id1, id2, 0x20EE36, fn_800498D0(a), 0);
            if (a->unk7D0 == 0) {
                if (r != 0x11D2C025) {
                    PlayerSetHeldSlotMode(a, 2);
                }
            } else {
                PlayerSetHeldSlotMode(a, 9);
            }
            fn_800699A8(a->sub2F4);
        }
    }
    if (a->f394 != 0.0f || a->f38C != 0.0f || a->f390 != 0.0f || a->f398 != 0.0f || a->unk328 != 0) {
        PlayerSetActionMode(a, 3);
    }
}

void AiState8012ADFC(Actor* a) {
    if ((float)rand() / 32767.0f >= 0.5f) {
        a->unk7D4 = 0;
    } else {
        a->unk7D4 = 1;
    }
    int idx = PlayerInHost(a);
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[0].byVariant[0], a->unk7D4, 0);
    if (a->anim98 == -1) {
        a->unk7D4 = 0;
        a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_8039F7C8[0].byVariant[0], a->unk7D4, 0);
    }
    a->cb8C = (void*)fn_8012B4C8;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[0].byVariant[0], a->unk7D4, 0);
    if (a->animF0 == -1) {
        a->unk7D4 = 0;
        a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_8039F7C8[0].byVariant[0], a->unk7D4, 0);
    }
    a->cbE4 = (void*)fn_8012B4C8;
    a->animFlagFC = 1;
}

int fn_8012AF68(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    if (a->unk318 >= 1) {
        int r = Fn800F7DEC(id1, id2, 0x20EE36, fn_800498D0(a), 0);
        if (a->state.cur == 1 && r == 0x4A3E183) {
            PlayerSetHeldSlotMode(a, 2);
            return 1;
        }
        r = Fn800F7DEC(id1, id2, 0xF829DB93, fn_800498D0(a), 0);
        PlayerResetGunTimer(a);
        if (r == 0x4A3E183) {
            fn_80048BA8(a, a->unk1D04, 1);
            Fn800F7F5C(id1, id2, 0xF829DB93, fn_800498D0(a), a->unk1D04, a->unk14FC);
            return 1;
        }
        if (r < 0x4A3E183) {
            if (r != -0x3F413082) {
                return 0;
            }
        } else if (r != 0x11D2C025) {
            return 0;
        }
        if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
            BotQuerySet(a->handle, 0x7FFCFD4F, 0x91A0CEF7);
        }
    } else {
        if (a->state.cur == 8) {
            PlayerSetHeldSlotMode(a, 3);
        }
    }
    return 0;
}

int fn_8012B1F4(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    if (a->unk314 >= 1) {
        int r = Fn800F7DEC(id1, id2, 0x20EE36, fn_800498D0(a), 0);
        if (a->state.cur == 1 && r == 0x4A3E183) {
            PlayerSetHeldSlotMode(a, 2);
            return 1;
        }
        PlayerResetGunTimer(a);
        if (r == 0x11D2C025) {
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                BotQuerySet(a->handle, 0x7FFCFD4F, 0x91A0CEF7);
            }
            if (a->state.cur == 5) {
                PlayerSetHeldSlotMode(a, 3);
            }
        } else if (r < 0x11D2C025) {
            if (r == 0x4A3E183) {
                if (a->state.cur != 5) {
                    PlayerSetHeldSlotMode(a, 5);
                    return 1;
                }
            } else if (r < 0x4A3E183 && r == -0x3F413082) {
                if (Fn800F7DEC(id1, id2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x4A3E183) {
                    PlayerSetHeldSlotMode(a, 10);
                    return 1;
                }
            }
        } else if (r == 0x3DA260F7 && a->state.cur == 5) {
            PlayerSetHeldSlotMode(a, 3);
        }
    } else {
        if (a->state.cur == 5) {
            PlayerSetHeldSlotMode(a, 3);
        }
    }
    return 0;
}

int fn_8012B4C8(int action, void* p) {
    Actor* a = (Actor*)p;
    int id1;
    int id2;
    MsgQuery q;
    q.handle = a->handle;
    q.hash = action;
    if (fn_800C5210(q.handle, 0xC06, &q) == 1) {
        return 0;
    }
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    switch (action) {
    case 0x1B83833F:
        switch (a->state.cur) {
        case 5: {
            int r = Fn800F7DEC(id1, id2, 0x20EE36, fn_800498D0(a), 0);
            if (r == 0x11D2C025) {
                PlayerSetActionMode(a, 1);
            } else {
                PlayerSetActionMode(a, 10);
            }
            return 0;
        }
        case 8: {
            int r = Fn800F7DEC(id1, id2, 0xF829DB93, fn_800498D0(a), 0);
            if (r != 0x4A3E183) {
                PlayerSetHeldSlotMode(a, 3);
            } else {
                PlayerSetActionMode(a, a->state.next);
            }
            return 0;
        }
        case 2: {
            int r = Fn800F7DEC(id1, id2, 0x20EE36, fn_800498D0(a), 0);
            if (r == -0x3F413082) {
                fn_80048BA8(a, a->unk1D04, 1);
                Fn800F7F5C(id1, id2, 0xE6B71BCC, fn_800498D0(a), a->unk1D04, a->unk14FC);
            }
            PlayerSetActionMode(a, a->state.next);
            return 0;
        }
        case 0xD:
            a->flags7B4 |= 0x20000;
            return 0;
        }
        PlayerSetActionMode(a, a->state.next);
        return 0;
    case 0x20EE36:
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0x20EE36, fn_800498D0(a), a->unk1D04, a->unk14FC);
        return 0;
    case 0xE6B71BCC:
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0xE6B71BCC, fn_800498D0(a), a->unk1D04, a->unk14FC);
        return 0;
    case 0x968CBE3D:
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0x968CBE3D, fn_800498D0(a), a->unk1D04, a->unk14FC);
        return 0;
    case 0xF829DB93:
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, 0xF829DB93, fn_800498D0(a), a->unk1D04, a->unk14FC);
        break;
    case 0x9791AF51:
    case 0x96224FCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        fn_80048BA8(a, a->unk1D04, 1);
        Fn800F7F5C(id1, id2, action, fn_800498D0(a), a->unk1D04, a->unk14FC);
        break;
    }
    return 0;
}

int AiState8012B904(Actor* a) {
    return lbl_8039F7C8[a->state.cur].byVariant[14];
}

void AiTypeMsg(Actor* a) {
    if (lbl_8039F7C8[a->state.cur].msg) {
        lbl_8039F7C8[a->state.cur].msg(a);
    }
}

void AiTypeCallback04(Actor* a) {
    if (lbl_8039F7C8[a->state.cur].cb04) {
        lbl_8039F7C8[a->state.cur].cb04(a);
    }
}

void AiTypeCallback00(Actor* a) {
    if (lbl_8039F7C8[a->state.cur].cb00) {
        lbl_8039F7C8[a->state.cur].cb00(a);
    }
}

void AiLoadAssets() {
    lbl_8046D1F0[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_8046D1F0[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_8046D1F0[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_8046D1F0[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_8046D1F0[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_8046D1F0[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_8046D1F0[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_8046D1F0[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_8046D1F0[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_8046D1F0[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_8046D1F0[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_8046D1F0[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_8046D1F0[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_8046D1F0[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_8046D1F0[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_8046D1F0[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

void AiNoOp() {
}

// TODO (not drafted yet, 0x1B8 bytes at 0x8012BC08): game logic
extern "C" void fn_8012BC08();

struct AiGunInfo {
    int unk0;
    int unk4;
    int unk8;
};

struct AiGunData {
    int mode;        // 0x00
    int unk4;
    int unk8;
    int unkC;        // 0x0C
    u8 pad10[0x20 - 0x10];
    AiGunInfo* info; // 0x20
    int unk24;
};

struct AiGunOwner {
    u8 pad00[8];
    int unk8;        // 0x08
    AiGunData* data; // 0x0C
};

int AiActionResult(AiGunOwner* owner, int action) {
    AiGunData* d = owner->data;
    AiGunInfo* g = d->info;
    int r;
    switch (action) {
    case 0xF829DB93:
    case 0x42E50A3D:
        return 0x4A3E183;
    case 0x96224FCC:
    case 0x20EE36:
        r = fn_800F7D28(d->unkC, g->unk4);
        if (r != 0) {
            return 0x4A3E183;
        }
        r = fn_800F7D28(owner->unk8, g->unk4);
        if (r != 0) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0x0D19E338:
        if (d->mode == 1) {
            return 0x4A3E183;
        }
        return 0x3DA260F7;
    case 0x968CBE3D:
    case 0xE6B71BCC:
        r = fn_800F7D28(d->unkC, g->unk4);
        if (g->unk8 == r) {
            return 0x4848CA1E;
        }
        r = fn_800F7D28(owner->unk8, g->unk4);
        if (r != 0) {
            return 0x4A3E183;
        }
        return 0x11D2C025;
    }
    return 0x3DA260F7;
}

// TODO (not drafted yet, 0x4A0 bytes at 0x8012BF4C): game logic
extern "C" void fn_8012BF4C();

struct MineBlock {
    u8 pad00[0xC];
    int handle;      // 0x0C
};

struct MineOwner {
    u8 pad00[0xC];
    MineBlock* block; // 0x0C
};

void MineBlockFree(MineOwner* o) {
    MineBlock* b = o->block;
    fn_800F86A0(b->handle);
    VFree(b);
}

// TODO (not drafted yet, 0x17C bytes at 0x8012C424): game logic (string: Proximity Mine)
extern "C" void fn_8012C424();

void MineSlotsFree() {
    for (int i = 0; i < 24; i++) {
        if (lbl_8039FF00[i] != 0) {
            VFree(lbl_8039FF00[i]);
        }
        lbl_8039FF00[i] = 0;
    }
    lbl_804D2E58 = 0;
}

void MineSlotsClear() {
    for (int i = 0; i < 24; i++) {
        lbl_8039FF00[i] = 0;
    }
    lbl_804D2E58 = 0;
}

void MineSetStateByOwner(int owner, int value) {
    for (int i = 0; i < 24; i++) {
        if (MineSlotUsed(i)) {
            MineSlot* m = lbl_8039FF00[i];
            if (m != 0) {
                if (m->f18 == owner) {
                    m->state = value;
                }
            }
        }
    }
}

// TODO (not drafted yet, 0x1B4 bytes at 0x8012C7B4): game logic
extern "C" void fn_8012C7B4();

void MineSlotsNotify(int owner) {
    for (int i = 0; i < 24; i++) {
        MineSlot* m = lbl_8039FF00[i];
        if (m != 0) {
            m->flag2C80 = 0;
            if (MineSlotUsed(i) && m->f14 == owner) {
                MineMsg msg;
                msg.target = m->handle;
                msg.event = 0x3700759A;
                SendMsg(msg.target, 0xC14, &msg);
            }
        }
    }
}

void MineSlotsRelease(Actor* a) {
    ListRemoveFree((void*)a->animFlagA4, (void**)lbl_8039FF00, &lbl_804D2E58, 24);
}

// TODO (not drafted yet, 0x100 bytes at 0x8012CA60): game logic
extern "C" void fn_8012CA60();

// TODO (not drafted yet, 0x748 bytes at 0x8012CB60): game logic
extern "C" void fn_8012CB60();

// TODO (not drafted yet, 0x598 bytes at 0x8012D2A8): game logic
extern "C" void fn_8012D2A8();

void MineSetFlagByOwner(int owner, int on) {
    for (int i = 0; i < 24; i++) {
        MineSlot* m = lbl_8039FF00[i];
        if (m != 0) {
            m->flag2C80 = on;
            if (MineSlotUsed(i) && (m->f18 == owner || owner == -1)) {
                MineMsg msg;
                msg.target = m->handle;
                msg.event = 0x3700759A;
                SendMsg(msg.target, 0xC14, &msg);
            }
        }
    }
}

// TODO (not drafted yet, 0x1D4 bytes at 0x8012D914): game logic
extern "C" void fn_8012D914();

// TODO (not drafted yet, 0x180 bytes at 0x8012DAE8): game logic
extern "C" void fn_8012DAE8();

// TODO (not drafted yet, 0x48C bytes at 0x8012DC68): game logic
extern "C" void fn_8012DC68();

int MineHitFilter(u32* ignore, u32 target) {
    u32 x = *ignore;
    if (x != 0) {
        return x != target;
    }
    return 1;
}

// TODO (not drafted yet, 0x3F8 bytes at 0x8012E11C): game logic
extern "C" void fn_8012E11C();

void MineSlotsNotifyTarget(u32 target) {
    for (int i = 0; i < 24; i++) {
        MineSlot* m = lbl_8039FF00[i];
        if (m != 0 && target == m->f10) {
            MineMsg msg;
            msg.target = m->handle;
            msg.event = 0x3700759A;
            SendMsg(msg.target, 0xC14, &msg);
        }
    }
}

void MineSlotsNotifyEither(u32 target) {
    for (int i = 0; i < 24; i++) {
        MineSlot* m = lbl_8039FF00[i];
        if (m != 0 && (target == m->f10 || target == m->f08)) {
            MineMsg msg;
            msg.target = m->handle;
            msg.event = 0x505DC9AA;
            SendMsg(msg.target, 0xC14, &msg);
        }
    }
}

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

int FoeCheckDown(FoeObj* o, FoeBeh* b) {
    int r;
    if (o == 0) {
        r = 0;
    } else {
        FoeData* d = b->data;
        if (fn_80162940(o->pad894, 0x1000) != 0) {
            b->notify(0xE929ADB6, o);
            r = 1;
        } else if (d->h4 != -1) {
            r = 1;
        } else {
            b->notify(0xACBEC0A8, o);
            r = 1;
        }
    }
    return r;
}

int FoeLeave(FoeObj* o, FoeBeh* b) {
    FoeData* d = b->data;
    if (d->f14 != 0) {
        fn_800B6CAC(o, (void*)fn_8012F210);
        d->f14 = 0;
    }
    ConditionUpdateBit(o->cond, 0x17, 0);
    RangeResetPending(o->cond);
    return 1;
}

int FoeDetach(FoeObj* o) {
    fn_800BBCA8(o);
    fn_800B6CAC(o, (void*)fn_8012EC3C);
    return 1;
}

int FoeReturnTrue() {
    return 1;
}

int FoeEnterWait(FoeObj* o, FoeBeh* b) {
    FoeData* d;
    int msg[8];
    if (o == 0) {
        return 0;
    }
    {
        d = b->data;
        d->mode = 4;
        d->f8 = 0;
        d->fC = 1000.0f * ((float)rand() / 32767.0f) + 500.0f;
        d->f10 = -1;
        d->f14 = 0;
        SendMsg(d->h4, 0x400, msg);
        FollowSetTargetObj(o->cond, (void*)msg[0]);
        ConditionUpdateBit(o->cond, 4, 1);
        ConditionUpdateBit(o->cond, 3, 1);
        o->f288 = 3;
    }
    return 1;
}

int FoeEnterChase(FoeObj* o, FoeBeh* b) {
    float t = 3000.0f;
    if (o == 0) {
        return 0;
    }
    int msg[8];
    FoeData* d = b->data;
    d->mode = 3;
    d->f10 = -1;
    d->f24 = t;
    RangeResetPending(o->cond);
    ConditionUpdateBit(o->cond, 3, 1);
    ConditionUpdateBit(o->cond, 4, 1);
    o->f288 = 0;
    if (SendMsg(d->h30, 0x400, msg) != 0) {
        FollowSetTargetObj(o->cond, (void*)msg[0]);
    }
    return 1;
}

int FoeEnterTrack(FoeObj* o, FoeBeh* b) {
    float t = 5000.0f;
    if (o == 0 || b == 0) {
        return 0;
    }
    int msg[8];
    FoeData* d = b->data;
    d->mode = 2;
    d->f10 = -1;
    d->f24 = t;
    ConditionUpdateBit(o->cond, 3, 1);
    ConditionUpdateBit(o->cond, 4, 1);
    o->f288 = 3;
    RangeResetPending(o->cond);
    if (SendMsg(d->h30, 0x400, msg) != 0) {
        FollowSetTargetObj(o->cond, (void*)msg[0]);
        fn_800B6CAC(o, (void*)fn_8012EC3C);
        fn_800B6D04(o, (void*)fn_8012EC3C);
    }
    return 1;
}

int FoeEnterAttack(FoeObj* o, FoeBeh* b) {
    float t = 10000.0f;
    if (o == 0) {
        return 0;
    }
    int msg[8];
    FoeData* d = b->data;
    d->mode = 1;
    d->f1C = t;
    SendMsg(d->h4, 0x400, msg);
    FollowSetTargetObj(o->cond, (void*)msg[0]);
    ConditionUpdateBit(o->cond, 4, 0);
    ConditionUpdateBit(o->cond, 3, 1);
    o->f288 = 3;
    fn_800BC56C(o, (void*)msg[0]);
    int r = fn_800BAF40(o);
    if (r == -1) {
        r = fn_800B26B8(o);
        if (r != 4) {
            fn_800B1AB8(1000.0f, o, r, 5);
            b->notify(0x30A0D567, o);
        }
    }
    return 1;
}

int FoeEnterIdle(FoeObj* o, FoeBeh* b) {
    if (o == 0) {
        return 0;
    }
    b->data->mode = 0;
    ConditionUpdateBit(o->cond, 4, 0);
    ConditionUpdateBit(o->cond, 3, 1);
    o->f288 = 3;
    return 1;
}

// TODO (not drafted yet, 0x114 bytes at 0x80131004): game logic
extern "C" void fn_80131004();

// TODO (not drafted yet, 0x1C8 bytes at 0x80131118): game logic
extern "C" void fn_80131118();

// TODO (not drafted yet, 0x2CC bytes at 0x801312E0): game logic
extern "C" void fn_801312E0();

void FoeFree(FoeObj* o, FoeBeh* b) {
    if (o != 0) {
        VFree(b->data);
        fn_800B6CAC(o, (void*)fn_8012F210);
        fn_800B6CAC(o, (void*)fn_8012EC3C);
    }
}

void FoeInit(FoeObj* o, FoeBeh* b) {
    if (o != 0) {
        FoeData* d = (FoeData*)VAlloc(0x40);
        if (d == 0) {
            b->data = 0;
        } else {
            b->cbD8 = (void*)fn_801312E0;
            float attack = 10000.0f;
            b->data = d;
            float track = 5000.0f;
            d->mode = 0;
            d->h4 = -1;
            d->f8 = 0;
            d->f10 = -1;
            d->f14 = 0;
            d->f18 = (b->fBC == -1) ? 1 : 2;
            d->f1C = attack;
            d->f20 = 0;
            d->f24 = track;
            d->f28 = -1;
            d->f2C = 0;
            d->h30 = -1;
            d->f34 = -1;
            d->f38 = -1;
            d->f3C = -1;
            fn_800BBCA8(o);
            RangeResetPending(o->cond);
            lbl_8039FF90[d->mode].enter(o, b);
        }
    }
}

int FoeNoMsgResult() {
    return 0x3DA260F7;
}

int FoeSpawnMsg(int* p, int msg, u32 param, int* extra) {
    int v = Fn80175D60(param);
    if (msg == 0x14967) {
        fn_80131798(param, v, extra, *p);
        return 1;
    }
    return 0;
}

// TODO (not drafted yet, 0x6C0 bytes at 0x80131798): game logic (string: ShotgunShell)

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

void AiState80134164(Actor* a, float dt) {
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

void AiState80134200(Actor* a) {
    a->cb8C = 0;
    if (a->animC4 != -1) {
        a->anim98 = a->animC4;
    }
    if (a->anim11C == -1) {
        return;
    }
    a->animF0 = a->anim11C;
}

void AiState8013422C(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    PlayerSetValue4(&a->state, 0x15);
    if (a->unk279C == -1 || fn_80175EFC(a->unk279C) != 0) {
        PlayerSetValue4(&a->state, 3);
        fn_80155A84(a->sub26E0, a->target, 1);
    }
}

void AiState801342AC(Actor* a) {
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
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_803A00D0[21].byVariant[v], ride, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 0x19;
    a->animF0 = fn_800A1BC8(set->a14, set->a1C, lbl_803A00D0[21].byVariant[v], ride, 0);
    a->animFlagFC = 0x19;
    PlayerSetValue4(&a->state, 0x15);
}

// TODO (not drafted yet, 0xA0 bytes at 0x801343A8): game logic
extern "C" void fn_801343A8();

void AiState80134448(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->hold = 0;
    a->flags &= ~0x20000;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[20].byVariant[v], 0, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[20].byVariant[v], 0, 0);
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

void AiState801345B8(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    if (a->flags7B4 & 1) {
        a->flags7B4 &= ~1;
    } else {
        a->flags |= 0x20000;
        a->hold = 1;
    }
}

void AiState80134614(Actor* a) {
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
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_803A00D0[19].byVariant[v], ride, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 0x19;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[19].byVariant[v], 0, 0);
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

void AiState80134774(Actor* a) {
    fn_80155A84(a->sub26E0, a->target, 1);
}

void AiState801347A0(Actor* a) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    a->flags |= 0x20000;
    a->hold = 1;
    PlayerSetValue4(&a->state, 0x13);
}

void AiState801347FC(Actor* a) {
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
    a->anim98 = fn_800A1BC8(set->a14, set->a18, lbl_803A00D0[18].byVariant[v], ride, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 0x59;
    AiAnimSet* set2 = a->set[idx];
    a->animF0 = fn_800A1BC8(set2->a14, set2->a1C, lbl_803A00D0[18].byVariant[v], 0, 0);
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

void AiState80134958(Actor* a) {
    a->flags &= ~0x40000;
}

void AiState80134968(Actor* a, float dt) {
    PlayerResetGunTimer(a);
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(a, dt);
}

// TODO (not drafted yet, 0x110 bytes at 0x801349C0): game logic
extern "C" void fn_801349C0();

void AiState80134AD0(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(&a->state, 3);
}

void AiState80134B3C(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    fn_80048BA8(a, a->unk1D04, 0);
    Fn800F7F5C(id1, id2, 0xBC78F550, fn_800498D0(a), a->unk1D04, a->unk14FC);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[11].byVariant[v], 0, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 0x11;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[11].byVariant[v], 0, 0);
    a->animFlagFC = 0x11;
}

void AiState80134CF4(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(&a->state, 3);
}

void AiState80134D60(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    fn_80048BA8(a, a->unk1D04, 0);
    Fn800F7F5C(id1, id2, 0xBC792AF5, fn_800498D0(a), a->unk1D04, a->unk14FC);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[15].byVariant[v], 0, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 0x11;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[15].byVariant[v], 0, 0);
    int r = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[3].byVariant[v], 0, 0);
    if (a->animF0 == r) {
        a->animFlagFC = 1;
    } else {
        a->animFlagFC = 0x11;
    }
}

void AiState80134F58(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    fn_80136594(a);
    fn_801362A4(a);
    PlayerSetValue4(&a->state, 10);
}

// TODO (not drafted yet, 0xFC bytes at 0x80134FD4): game logic
extern "C" void fn_80134FD4();

void AiState801350D0(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(&a->state, 10);
}

void AiState8013513C(Actor* a) {
    int id1;
    int id2;
    QueryGet(&id1, a->handle, 0x2DA8293C);
    QueryGet(&id2, a->handle, 0xDFF3097A);
    fn_80048BA8(a, a->unk1D04, 0);
    Fn800F7F5C(id1, id2, 0x0E567C7E, fn_800498D0(a), a->unk1D04, a->unk14FC);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[14].byVariant[v], 0, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 0x11;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[14].byVariant[v], 0, 0);
    int r = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[3].byVariant[v], 0, 0);
    if (a->animF0 == r) {
        a->animFlagFC = 1;
    } else {
        a->animFlagFC = 0x11;
    }
}

void AiState80135334(Actor* a) {
    ActorPlayAnim(0x2000000, 0xD39F69B4, fn_800498D0(a));
}

void AiState80135368(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(&a->state, 1);
}

void AiState801353D4(Actor* a) {
    int id;
    QueryGet(&id, a->handle, 0xDFF3097A);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[9].byVariant[v], 0, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 0x10;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[9].byVariant[v], 0, 0);
    a->animFlagFC = 0x10;
    lbl_804D2E6C = fn_800F7994(id, 0x9D1C245C);
    FxStart((void*)lbl_804D2E6C, 0, 0);
}

// TODO (not drafted yet, 0x90 bytes at 0x80135514): game logic
extern "C" void fn_80135514();

void AiState801355A4(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[8].byVariant[v], 0, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[8].byVariant[v], 0, 0);
    a->animFlagFC = 1;
}

// TODO (not drafted yet, 0x90 bytes at 0x80135658): game logic
extern "C" void fn_80135658();

void AiState801356E8(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[5].byVariant[v], 0, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[5].byVariant[v], 0, 0);
    a->animFlagFC = 1;
}

// TODO (not drafted yet, 0x124 bytes at 0x8013579C): game logic
extern "C" void fn_8013579C();

void AiState801358C0(Actor* a) {
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[3].byVariant[v], 0, 0);
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[3].byVariant[v], 0, 0);
    a->animFlagFC = 1;
    DashUpdate(a);
}

void AiState8013597C(Actor* a) {
    ActorPlayAnim(0x2000000, 0x8B3A27A3, fn_800498D0(a));
}

void AiState801359B0(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags7B4 &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(&a->state, 3);
}

void AiState80135A1C(Actor* a) {
    int id;
    QueryGet(&id, a->handle, 0xDFF3097A);
    int idx = PlayerInHost(a);
    int v = a->variant;
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[2].byVariant[v], 0, 0);
    a->animFlagA4 = 0x10;
    a->cb8C = (void*)PlayerGunAction;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[2].byVariant[v], 0, 0);
    a->animFlagFC = 0x10;
    lbl_804D2E68 = fn_800F7994(id, 0xAFEF562B);
    FxStart((void*)lbl_804D2E68, 0, 0);
}

// TODO (not drafted yet, 0xE8 bytes at 0x80135B5C): game logic
extern "C" void fn_80135B5C();

// TODO (not drafted yet, 0xBC bytes at 0x80135C44): game logic
extern "C" void fn_80135C44();

void AiState80135D00(Actor* a) {
    PlayerResetGunTimer(a);
}

// TODO (not drafted yet, 0xE0 bytes at 0x80135D20): game logic
extern "C" void fn_80135D20();

void AiState80135E00(Actor* a) {
    if ((float)rand() / 32767.0f >= 0.5f) {
        a->unk7D4 = 0;
    } else {
        a->unk7D4 = 1;
    }
    int idx = PlayerInHost(a);
    a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[0].byVariant[0], a->unk7D4, 0);
    if (a->anim98 == -1) {
        a->unk7D4 = 0;
        a->anim98 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a18, lbl_803A00D0[0].byVariant[0], a->unk7D4, 0);
    }
    a->cb8C = (void*)PlayerGunAction;
    a->animFlagA4 = 1;
    a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[0].byVariant[0], a->unk7D4, 0);
    if (a->animF0 == -1) {
        a->unk7D4 = 0;
        a->animF0 = fn_800A1BC8(a->set[idx]->a14, a->set[idx]->a1C, lbl_803A00D0[0].byVariant[0], a->unk7D4, 0);
    }
    a->cbE4 = (void*)PlayerGunAction;
    a->animFlagFC = 1;
}

// TODO (not drafted yet, 0x338 bytes at 0x80135F6C): game logic
extern "C" void fn_80135F6C();

// TODO (not drafted yet, 0x2F0 bytes at 0x801362A4): game logic

// TODO (not drafted yet, 0x2F0 bytes at 0x80136594): game logic


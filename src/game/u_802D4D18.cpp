// u_802D4D18.cpp: host-ride (rideable actor) behaviour helpers (0x802D4D18-0x802D77AC).
// Names are descriptive guesses; the code is matched against the retail DOL.
// Names are descriptive guesses; structure layouts come from retail offsets.

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed int s32;
typedef float f32;
typedef double f64;

extern "C" void* memcpy(void* dst, const void* src, u32 n);
extern "C" u8 __upper_map[];
extern "C" char lbl_8034C308[];   // rodata block holding the event name strings

// Engine/game helpers that are still placeholders in symbols.txt.
extern "C" int fn_800AFD20(void* obj);
extern "C" int fn_8003A1F0(int a, u32 b, int c);
extern "C" void fn_8003F15C(f32 a, int b, int c, u32 d, void* e, int f);
extern "C" void fn_8016F120(void* mgr, void* target);
extern "C" u32 fn_8008DCDC(u32 handle, int a, int b, int c);
extern "C" void fn_8008F4E8(u32 handle, int v);
extern "C" void fn_8008F378(u32 handle);
extern "C" void fn_8008F4EC(u32 handle, int v);
extern "C" void fn_800C5210(s32 id, s32 msg, void* arg);
extern "C" int fn_8016325C(void* stream);
extern "C" void fn_80162610(void* owner, void* stream, u32 msg);
extern "C" void fn_8028C188(void* dst, const void* src, u32 len);
extern "C" void fn_80037EC0(int model, int node, int on, int r, int g, int b, int a);
extern "C" int fn_8008EA9C(u32 handle, int a);
extern "C" int fn_8008E964(u32 handle);
extern "C" f32 fn_800A1394(f32 x);
void RideHostUpdate(void* argp);
extern "C" int fn_80212730(s32 entity);
extern "C" void fn_8008DE4C(s32 handle, int a, s32 b, int c);
extern "C" int fn_8008E7D0(void);
extern "C" void fn_8008F230(f32 v, u32 handle, int a, int b);
extern "C" void fn_80090974(f32 v, u32 handle, int a, int b, int c, u32 callback,
                            void* owner);
extern "C" int fn_800A1BC8(u32 bank, u32 set, u16 anim, int frame, int a);
void VFree(void* p);
int Rand();
int AnimBankGetSetCount(u16 bank, u16 set, u16 anim);

// Named engine helpers (mangled names already exist in symbols.txt).
void ModelNodeSetup(int model, int node, int a, int b);
int ModelAnimate(int model, char* data, int i);
int ModelBoneName(int a, int b);
void ConditionSetBit(void* flags, int bit, int on);
int HasCondition(void* flags, int bit);
int MemFlagIsSet(void* flags, int bit);
void ActorPlayAnim(int id, u32 anim, int arg);
void RangeResetFields(void* argp);

// Math/pose helpers (names already exist in symbols.txt).
struct Mtx34 {
    f32 m[12];   // 0x00  3x4 row-major matrix
};
struct Vec3 {
    f32 x;       // 0x00
    f32 y;       // 0x04
    f32 z;       // 0x08
};
struct PlaceF;
struct Actor;

void ActorGetMatrix(Mtx34* m, Actor* a);
void BoneGetMatrix(Mtx34* m, void* bone);
void PlacementToMatrix(Mtx34* m, PlaceF* place);
void MtxGetEuler(Vec3* rot, Mtx34* m);
void MtxSetEuler(Mtx34* m, Vec3* rot);
void EulerNormalize(Vec3* out, Vec3* in);
void Vec3Set(Vec3* v, f32 x, f32 y, f32 z);
void ModelSetMatrix(int model, Mtx34* m, char* data);
int AnimApply(int handle, char* data, int anim, int arg);

// Placeholder engine helpers used by the per-frame pose update.
extern "C" void fn_800913EC(int handle, void* out, f32 v);
extern "C" f32 fn_800CC2D0(void* owner, int mode, void* a, void* b);
extern "C" void fn_800A0168(void* a, void* b);
extern "C" void fn_800A0030(void* out, void* mtx, void* in);
extern "C" void fn_8016D4EC(void* a, void* b);
extern "C" f32 fn_800E1020(void* v);
extern "C" void fn_800E1098(void* a, void* b, f32 f);

// Small string used by the name-report event.
class Buffer {
public:
    virtual ~Buffer();
    void* mData;   // 0x04
    u32 mSize;     // 0x08
    u32 mUnk0C;    // 0x0C
    u32 mUnk10;    // 0x10
};

class String : public Buffer {
public:
    String(const char* s);
};

extern "C" String fn_800225C8(const String* a, const String* b);

// 0x0C payload word: a float magnitude for damage events, a counter for the
// stream events (0x7102/0x7103).
union EventWord {
    s32 i;
    u32 u;
    f32 f;
};

// 0x08 payload word: a byte cursor for the stream events, an id otherwise.
union EventRef {
    u8* p;
    u32 u;
};

struct RideBehaviour;

// Animation delta block reached through RideActor::unk0F0 (only the translation is used here).
struct RideAnim {
    u8 unk00[0x10];
    float unk10;  // 0x10
    float unk14;  // 0x14
    float unk18;  // 0x18
};

// Entry of the per-behaviour-type table at 0x803C5664 (0x18 bytes each).
struct RideBehaviourType {
    u32 unk00;                                  // 0x00
    void (*unk04)(void* owner);                 // 0x04
    int  (*unk08)(void* owner, int type);       // 0x08
    int  (*unk0C)(void* owner);                 // 0x0C
    u32 unk10;                                  // 0x10
    const char* name;                           // 0x14
};

// Static tables shared by this unit's handlers (0x803C54D8).
struct RideActorTables {
    u8  pad000[0xD8];      // 0x000
    s32 nodeShow4[4];      // 0x0D8  node ids shown for the four primary parts
    s32 nodeHide4[4];      // 0x0E8  node ids hidden when a primary part breaks
    s32 boneName[4];       // 0x0F8  primary part bone name hashes
    s32 nodeName[11];      // 0x108  secondary part bone name hashes
    s32 nodeShow[11];      // 0x134  node ids enabled when a part is intact
    s32 nodeHide[11];      // 0x160  node ids enabled when a part breaks (-1: none)
};

// Per-actor behaviour data (RideActor::behaviour).
struct RideBehaviour {
    s32 type;              // 0x00  index into the 0x803C5664 table
    s32 unk04;             // 0x04  type-specific state (passed to the handlers)
    s32 unk08;             // 0x08  ride effect node/id
    s32 unk0C;             // 0x0C
    u8  pad10[0x14 - 0x10];
    f32 unk14;             // 0x14
    s32 lastPart;          // 0x18  part that broke most recently
    f32 value[19];         // 0x1C  part health [0..3], cooldowns [4..7],
                           //       respawn timers [8..18]
    u8  pad68[0x6C - 0x68];
    s32 unk6C;             // 0x6C
};

// Object at RideActor::state; it also holds the animation bank/set ids.
struct RideState {
    u8  pad00[0x14];
    u32 unk14;             // 0x14  animation bank id
    u8  pad18[0x1C - 0x18];
    u32 unk1C;             // 0x1C  animation set id
    u8  pad20[0x30 - 0x20];
    u32 unk30;             // 0x30
};

// Host-ride actor. Every gap is padded to keep the retail offsets.
struct RideActor {
    u8  pad000[0x024];
    u8  unk024[0x14];      // 0x024  transform/target passed to fn_8016F120
    s32 unk038;            // 0x038  entity id
    u8  pad03C[0xC4 - 0x03C];
    RideState* state;      // 0x0C4
    u8  pad0C8[0xCC - 0x0C8];
    s32 model;             // 0x0CC  model instance
    s32 unk0D0;            // 0x0D0
    s32 unk0D4;            // 0x0D4
    u8  pad0D8[0x0F0 - 0x0D8];
    s32 unk0F0;            // 0x0F0
    u8  pad0F4[0x10C - 0x0F4];
    void (*unk10C)(void*); // 0x10C  per-type range field reset callback
    s32 unk110;            // 0x110
    u8  pad114[0x118 - 0x114];
    s32 unk118;            // 0x118
    u8  pad11C[0x134 - 0x11C];
    RideBehaviour* behaviour;  // 0x134
    f32 unk138;            // 0x138  per-frame fade step
    f32 unk13C;            // 0x13C
    u8  pad140[0x220 - 0x140];
    s32 unk220;            // 0x220
    u8  pad224[0x26C - 0x224];
    s32 unk26C;            // 0x26C
    u8  unk270[0x288 - 0x270];  // 0x270  condition/flag block
    s32 unk288;                 // 0x288  ride link word (0 = no ride target)
    u8  pad28C[0x5F0 - 0x28C];
    s32 unk5F0;                 // 0x5F0  ride stream state handle (0 = none)
    u8  unk5F4[0x904 - 0x5F4];  // 0x5F4  rest of the condition/flag block
    f32 unk904;                 // 0x904
};

// Event structure handed to the actor's message handler.
struct RideEvent {
    RideActor* owner;      // 0x00
    s32 name;              // 0x04  event name hash
    EventRef ref;          // 0x08
    EventWord word;        // 0x0C
    s32 unk10;             // 0x10
    s32 unk14;             // 0x14
    u8  pad18[0x44 - 0x18];
    s32 bone;              // 0x44  model bone index
    u8  pad48[0x58 - 0x48];
    s32 unk58;             // 0x58
};

extern "C" RideActorTables lbl_803C54D8;
extern "C" s32 lbl_803C560C[];
extern "C" RideBehaviourType lbl_803C5664[];
extern "C" f32 lbl_804DB120;
extern "C" f32 lbl_804DB124;
extern "C" f32 lbl_804DB138;
extern "C" f32 lbl_804DB13C;
extern "C" f32 lbl_804DB140;
extern "C" f32 lbl_804D3D04;
extern "C" f32 lbl_804D3D0C;
extern "C" f32 lbl_804DB128;
extern "C" f32 lbl_804DB150;
extern "C" f32 lbl_804D5808;
extern "C" void* lbl_804D3D10;
extern "C" void* lbl_804D3D14;
extern "C" void* lbl_804D3D18;

int RideApplyDamage(void* arg);

// 31-based hash of an upper-cased name string, as used by the name lookups.
static inline u32 RideNameHash(const char* name)
{
    u32 hash = 0;
    if (name != 0) {
        const char* p = name;
        while (*p != 0) {
            u32 c;
            if (*p == -1) {
                c = 0xffffffff;
            } else {
                c = __upper_map[(u8)*p];
            }
            hash = hash * 31 + (c & 0xff);
            p++;
        }
        hash = hash % 0xffffffff;
    }
    return hash;
}

// Actor message handler. Message ids are engine event codes; the 0x71xx pair
// streams the behaviour block in and out of a byte buffer.
int RideEventHandler(int msg, void* argp)
{
    RideEvent* e = (RideEvent*)argp;

    switch (msg) {
    case 0: {
        RideActor* a = e->owner;
        a->unk13C = 0.0f;
        int h = fn_800AFD20(a);
        int anim = ModelAnimate(a->model, 0, 0);
        int r = fn_8003A1F0(h, a->unk0F0, anim);
        fn_8003F15C(lbl_804DB124, h, r, a->unk0D0, &e->unk10, 0);
        return 1;
    }
    case 0x407:
        return RideApplyDamage(e);
    case 0x41D: {
        RideBehaviour* beh = e->owner->behaviour;
        if (e->name == -0x5CF85AE8) {
            beh->unk6C = e->word.i;
        }
        return 0;
    }
    case 0x41E: {
        if (e->name != -0x527A8906) {
            return 0;
        }
        RideBehaviour* beh = e->owner->behaviour;
        String a(lbl_803C5664[beh->type].name);
        String b(lbl_8034C308 + 0x51);
        String c = fn_800225C8(&a, &b);
        fn_8028C188(&e->word, c.mData, c.mSize);
        return 0;
    }
    case 0x803: {
        RideBehaviour* beh = e->owner->behaviour;
        if (beh != 0) {
            lbl_803C5664[beh->type].unk04(e->owner);
        }
        return 0;
    }
    case 0xC06: {
        RideActor* a = e->owner;
        RideBehaviour* beh = a->behaviour;
        if (e->word.i != a->unk038) {
            return 1;
        }
        switch (e->name) {
        case 0x119A6C2D:
            ModelNodeSetup(a->model, lbl_803C560C[2], 0, 1);
            ModelNodeSetup(a->model, lbl_803C560C[3], 0, 1);
            break;
        case -0x30FB04BF:
            if (beh->value[10] > 0.0f) {
                ModelNodeSetup(a->model, lbl_803C560C[2], 1, 1);
            }
            if (beh->value[11] > 0.0f) {
                ModelNodeSetup(a->model, lbl_803C560C[3], 1, 1);
            }
            fn_8016F120(lbl_804D3D18, a->unk024);
            break;
        case 0x1B83833F:
            if ((fn_8008DCDC(a->unk0D4, 0, 0, 0) & 0x10) != 0) {
                if (a->unk220 == 4) {
                    a->unk220 = 0;
                }
                ConditionSetBit(a->unk270, 0xD, 0);
                if (HasCondition(a->unk270, 0xC) != 0) {
                    ConditionSetBit(a->unk270, 0xC, 0);
                    ConditionSetBit(a->unk270, 0xE, 1);
                    fn_8008F4E8(a->unk0D4, 0);
                    fn_8008F378(a->unk0D4);
                    fn_8008F4EC(a->unk0D4, 0);
                }
            }
            break;
        }
        fn_800C5210(a->unk038, 0xC06, e);
        return 1;
    }
    case 0x7102: {
        RideBehaviour* beh = e->owner->behaviour;
        if (beh == 0) {
            return 1;
        }
        if (e->name < 0x74) {
            e->unk10 = 0;
            return 1;
        }
        u32 hash = RideNameHash(lbl_8034C308 + 0x48);
        memcpy(e->ref.p, &hash, 4);
        e->word.i += 4;
        e->name -= 4;
        e->ref.p += 4;
        memcpy(e->ref.p, beh, 0x70);
        e->word.i += 0x70;
        e->name -= 0x70;
        e->ref.p += 0x70;
        return 1;
    }
    case 0x7103: {
        RideBehaviour* beh = e->owner->behaviour;
        if (beh == 0) {
            return 1;
        }
        s32 size = e->word.i;
        bool ok;
        if (size < 0x74) {
            e->unk10 = 0;
            return 1;
        }
        if (size < 4) {
            ok = false;
        } else {
            u8* p = e->ref.p;
            e->word.i = size + 4;
            e->name -= 4;
            e->ref.p = p + 4;
            ok = (*(u32*)p == RideNameHash(lbl_8034C308 + 0x48));
        }
        if (!ok) {
            return 1;
        }
        memcpy(beh, e->ref.p, 0x70);
        e->word.i -= 0x70;
        e->ref.p += 0x70;
        return 1;
    }
    }
    return 0;
}

// Part damage event (0x407): looks the hit bone up in the part name tables,
// drains that part's health and breaks the part when it reaches zero.
int RideApplyDamage(void* argp)
{
    RideEvent* e = (RideEvent*)argp;
    RideActor* a = e->owner;
    RideBehaviour* beh = a->behaviour;

    if (MemFlagIsSet(a->unk270, 10)) {
        return 1;
    }
    if (beh->type == 2) {
        return 1;
    }
    if (e->unk14 == 0x20EE36) {
        return 1;
    }

    int bone = ModelBoneName(a->model, e->bone);

    // Primary parts: health drains while the actor is still mobile.
    for (int i = 0; i < 4; i++) {
        if (bone != lbl_803C54D8.boneName[i]) {
            continue;
        }
        if (beh->value[i] <= 0.0f) {
            break;
        }
        if (i == 2 && (beh->value[0] > 0.0f || beh->value[1] > 0.0f)) {
            break;
        }
        if (i == 3 && beh->value[2] > 0.0f) {
            break;
        }
        if (beh->unk6C != 0) {
            f32 v = beh->value[i] - lbl_804D3D04;
            if (e->word.f < v) {
                f32 nv;
                if (v > 0.0f) {
                    nv = v;
                } else {
                    nv = 0.0f;
                }
                e->word.f = nv;
            }
        }
        beh->value[i] -= e->word.f;
        if ((e->ref.u >> 24) == 2 &&
            beh->value[4 + i] < lbl_804D3D0C - lbl_804D5808) {
            beh->value[4 + i] = lbl_804D3D0C;
        }
        if (beh->value[i] <= 0.0f) {
            ConditionSetBit(a->unk270, 4, 1);
            a->unk904 -= lbl_804DB128 * (f32)a->state->unk30;
            beh->lastPart = i;
            if (i == 0) {
                ActorPlayAnim(a->unk038, 0x452BE671, e->ref.u);
            } else if (i == 1) {
                ActorPlayAnim(a->unk038, 0xB6FDA8FA, e->ref.u);
            } else if (i == 2) {
                ActorPlayAnim(a->unk038, 0x93C25FA2, e->ref.u);
            } else if (i == 3) {
                ActorPlayAnim(a->unk038, 0xAE7A5E2B, e->ref.u);
            }
        }
        fn_8016F120(lbl_804D3D10, a->unk024);
        return 1;
    }

    // Secondary parts: timers count down to the part coming back.
    for (int i = 0; i < 11; i++) {
        if (bone != lbl_803C54D8.nodeName[i]) {
            continue;
        }
        if (beh->value[8 + i] <= 0.0f) {
            break;
        }
        beh->value[8 + i] -= e->word.f;
        if (beh->value[8 + i] > 0.0f) {
            return 1;
        }
        ModelNodeSetup(a->model, lbl_803C54D8.nodeShow[i], 0, 1);
        if (lbl_803C54D8.nodeHide[i] == -1) {
            return 1;
        }
        ModelNodeSetup(a->model, lbl_803C54D8.nodeHide[i], 1, 1);
        return 1;
    }

    fn_8016F120(lbl_804D3D14, a->unk024);
    e->unk58 = 1;
    return 1;
}

// Per-frame update of the four primary part visuals: toggles each part's node,
// fades a part out while its respawn timer runs down and drives the behaviour
// type state machine (enter type 3, idle, then update/transition).
int RideUpdatePartVisuals(void* argp)
{
    RideActor* a = (RideActor*)argp;
    if (a == 0) {
        return 0;
    }
    RideBehaviour* beh = a->behaviour;
    s32* show = lbl_803C54D8.nodeShow4;
    s32* hide = lbl_803C54D8.nodeHide4;
    RideBehaviourType* types = lbl_803C5664;

    for (int i = 0; i < 4; i++) {
        if (beh->value[i] <= lbl_804DB120) {
            ModelNodeSetup(a->model, show[i], 0, 1);
        }
        if (i == 3) {
            if (beh->value[2] > lbl_804DB120 || beh->value[3] <= lbl_804DB120) {
                ModelNodeSetup(a->model, show[i], 0, 1);
            } else {
                ModelNodeSetup(a->model, show[i], 1, 1);
            }
        }
        if (beh->value[i + 4] > lbl_804DB120) {
            f32 t = lbl_804DB138 * (beh->value[i + 4] / lbl_804D3D0C);
            if (t < lbl_804DB120) {
                t = lbl_804DB120;
            } else if (t > lbl_804DB124) {
                t = lbl_804DB124;
            }
            f32 r = fn_800A1394(lbl_804DB13C * t);
            u8 shade = (u8)(s32)(lbl_804DB140 * (lbl_804DB124 - r));
            fn_80037EC0(a->model, show[i], 1, 0xFF, shade, shade, 0xFF);
            beh->value[i + 4] -= a->unk138;
            if (beh->value[i + 4] <= lbl_804DB120) {
                fn_80037EC0(a->model, show[i], 0, 0xFF, 0xFF, 0xFF, 0xFF);
                ModelNodeSetup(a->model, hide[i], 0, 1);
            }
        }
    }

    if (HasCondition(a->unk270, 9) != 0 && beh->type != 3 && beh->type != 4) {
        types[beh->type].unk08(a, 3);
        types[3].unk04(a);
    }

    if (HasCondition(a->unk270, 0x1C) != 0 ||
        (fn_8008EA9C(a->unk0D4, 0) != 0 && fn_8008EA9C(a->unk0D4, 0) != 2)) {
        if (HasCondition(a->unk270, 0xD) == 0 && HasCondition(a->unk270, 9) == 0 &&
            HasCondition(a->unk270, 0xA) == 0 && HasCondition(a->unk270, 0xB) == 0) {
            types[beh->type].unk04(a);
        }
        if (HasCondition(a->unk270, 0x1C) != 0 && HasCondition(a->unk270, 0xD) != 0 &&
            fn_8008E964(a->unk0D4) != 0) {
            ConditionSetBit(a->unk270, 0x1C, 0);
            ConditionSetBit(a->unk270, 0x1D, 1);
        }
    }

    int next;
    if (a->unk220 == 4) {
        RideHostUpdate(a);
        next = beh->type;
    } else {
        next = types[beh->type].unk0C(a);
    }
    if (next != beh->type) {
        if (types[beh->type].unk08(a, next) != 0) {
            types[next].unk04(a);
        }
    }
    return 1;
}

// Releases the actor's behaviour block and clears the cached pointer.
void RideFreeBehaviour(void* argp)
{
    RideActor* a = (RideActor*)argp;
    a->unk110 = 0;
    VFree(a->behaviour);
    a->behaviour = 0;
}

// Helpers used by the behaviour setup below.
void* VAlloc(int size);
extern "C" void* memset(void* dst, int c, u32 n);
u32 AssetFind(u32 group, u32 id);
void* FxFind(const char* name);
extern "C" void fn_800CDD10(void* owner);
extern "C" void fn_80090634(int handle, int a, int b, int c);
extern "C" void fn_800CB8BC(void* owner, void* state, int arg, int param);
extern "C" f32 lbl_804DB144;
extern "C" f32 lbl_804DB148;
extern "C" f32 lbl_804DB14C;
extern "C" f32 lbl_804D3D08;

// Allocates the actor's behaviour block, looks up the part/bone tables and
// seeds the part health, cooldown timers and node visibility.
void RideInitBehaviour(void* arg0, void* argp)
{
    RideActor* a = (RideActor*)argp;

    if (a == 0) {
        return;
    }
    a->unk110 = (s32)RideUpdatePartVisuals;
    a->unk118 = (s32)fn_800CDD10;

    RideBehaviour* beh = (RideBehaviour*)VAlloc(0x70);
    if (beh == 0) {
        a->behaviour = 0;
        return;
    }
    memset(beh, 0, 0x70);
    a->behaviour = beh;

    if (HasCondition(a->unk270, 10) != 0 || a->unk904 <= lbl_804DB120) {
        beh->type = 3;
        beh->unk04 = 4;
    } else {
        beh->type = 0;
        beh->unk04 = 0;
    }

    RideActorTables* t = &lbl_803C54D8;

    // Primary part node ids (shown while intact, hidden once broken).
    t->nodeShow4[0] = (s32)AssetFind(0xD9044CBB, 0xA05ECF4F);
    t->nodeShow4[1] = (s32)AssetFind(0xD9044CBB, 0xA05ECF55);
    t->nodeShow4[2] = (s32)AssetFind(0xD9044CBB, 0x9FAF456B);
    t->nodeShow4[3] = (s32)AssetFind(0xD9044CBB, 0x40C493BD);
    t->nodeHide4[0] = (s32)AssetFind(0xD9044CBB, 0x84EE32BE);
    t->nodeHide4[1] = (s32)AssetFind(0xD9044CBB, 0xC2539644);
    t->nodeHide4[2] = (s32)AssetFind(0xD9044CBB, 0xB0BCA3DA);
    t->nodeHide4[3] = (s32)AssetFind(0xD9044CBB, 0xB0BCA3DA);

    // Part bone name hashes.
    t->boneName[0] = (s32)AssetFind(0xF1754BF2, 0xA05ECF4F);
    t->boneName[1] = (s32)AssetFind(0xF1754BF2, 0xA05ECF55);
    t->boneName[2] = (s32)AssetFind(0xF1754BF2, 0x9FAF456B);
    t->boneName[3] = (s32)AssetFind(0xF1754BF2, 0x40C493BD);

    // Secondary part bone name hashes.
    t->nodeName[0] = (s32)AssetFind(0xF1754BF2, 0x43340A56);
    t->nodeName[1] = (s32)AssetFind(0xF1754BF2, 0xBCCEA45C);
    t->nodeName[2] = (s32)AssetFind(0xF1754BF2, 0xD5650569);
    t->nodeName[3] = (s32)AssetFind(0xF1754BF2, 0xD565056F);
    t->nodeName[4] = (s32)AssetFind(0xF1754BF2, 0x83ED00BC);
    t->nodeName[5] = (s32)AssetFind(0xF1754BF2, 0x83ED00BD);
    t->nodeName[6] = (s32)AssetFind(0xF1754BF2, 0x83ED00BE);
    t->nodeName[7] = (s32)AssetFind(0xF1754BF2, 0x63E20FC6);
    t->nodeName[8] = (s32)AssetFind(0xF1754BF2, 0x63E20FC7);
    t->nodeName[9] = (s32)AssetFind(0xF1754BF2, 0x63E20FC8);
    t->nodeName[10] = (s32)AssetFind(0xF1754BF2, 0x63E20FC9);

    // Secondary part node ids: shown while intact, hidden once broken.
    t->nodeShow[0] = (s32)AssetFind(0xD9044CBB, 0x43340A56);
    t->nodeShow[1] = (s32)AssetFind(0xD9044CBB, 0xBCCEA45C);
    t->nodeShow[2] = (s32)AssetFind(0xD9044CBB, 0xD5650569);
    t->nodeShow[3] = (s32)AssetFind(0xD9044CBB, 0xD565056F);
    t->nodeShow[4] = (s32)AssetFind(0xD9044CBB, 0x83ED00BC);
    t->nodeShow[5] = (s32)AssetFind(0xD9044CBB, 0x83ED00BD);
    t->nodeShow[6] = (s32)AssetFind(0xD9044CBB, 0x83ED00BE);
    t->nodeShow[7] = (s32)AssetFind(0xD9044CBB, 0x63E20FC6);
    t->nodeShow[8] = (s32)AssetFind(0xD9044CBB, 0x63E20FC7);
    t->nodeShow[9] = (s32)AssetFind(0xD9044CBB, 0x63E20FC8);
    t->nodeShow[10] = (s32)AssetFind(0xD9044CBB, 0x63E20FC9);

    t->nodeHide[0] = (s32)AssetFind(0xD9044CBB, 0x75C15C70);
    t->nodeHide[1] = (s32)AssetFind(0xD9044CBB, 0x90E5D9F6);
    for (int i = 2; i < 11; i++) {
        t->nodeHide[i] = -1;
    }

    lbl_804D3D10 = FxFind(lbl_8034C308 + 0x53);   // Impact_Truck
    lbl_804D3D14 = FxFind(lbl_8034C308 + 0x60);   // Impact_NoDamage
    lbl_804D3D18 = FxFind(lbl_8034C308 + 0x70);   // Truck_Horn

    f32 health = lbl_804DB144;
    for (int i = 0; i < 4; i++) {
        beh->value[4 + i] = health;
        ModelNodeSetup(a->model, t->nodeHide4[i], 0, 1);
    }

    beh->value[0] = lbl_804DB148;
    beh->value[1] = lbl_804DB148;
    beh->value[2] = lbl_804DB14C;
    beh->value[3] = lbl_804DB148;

    for (int i = 0; i < 11; i++) {
        beh->value[8 + i] = lbl_804D3D08;
        if (t->nodeHide[i] != -1) {
            ModelNodeSetup(a->model, t->nodeHide[i], 0, 1);
        }
    }

    beh->unk14 = lbl_804DB120;
    ConditionSetBit(a->unk270, 1, 1);
    fn_80090634(a->unk0D4, 0, 0, 0);
}

// Behaviour message handler: switch-type events run the per-type enter/exit
// handlers, the impact events forward to the shared collision routine.
int RideHandleBehaviourMsg(int msg, void* argp)
{
    if (argp == 0) {
        return 0;
    }

    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;

    if (msg == 0x1B83833F) {
        lbl_803C5664[beh->type].unk08(a, beh->unk04);
        lbl_803C5664[beh->unk04].unk04(a);
    } else if (msg == 0x04A5C9FC || msg == 0x00239807 || msg == 0x46341C3A) {
        fn_800CB8BC(a, a->state, beh->unk0C, a->unk26C);
    }
    return 1;
}

// Second behaviour message handler of the host-ride actor: runs the
// per-type switch handlers for the 0x1B83833F event, re-sends the switch event
// once the streaming ride state reports it is ready, and forwards the
// collision events to the shared impact routine.
int RideHostMsgHandler(int msg, void* argp)
{
    if (argp == 0) {
        return 0;
    }

    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;
    void* stream = &a->unk5F4[0x894 - 0x5F4];  // 0x894, ride state stream object

    switch (msg) {
    case 0x04A5C9FC:
    case 0x00239807:
    case 0x46341C3A:
        fn_800CB8BC(a, a->state, beh->unk0C, a->unk26C);
        break;
    case 0x20EE36:
    case 0xF829DB93:
        if (fn_8016325C(stream) == 4) {
            fn_80162610(a, stream, 0x20EE36);
        } else if (fn_8016325C(stream) == 5) {
            fn_80162610(a, stream, 0xF829DB93);
        }
        break;
    case 0x1B83833F:
        lbl_803C5664[beh->type].unk08(a, beh->unk04);
        lbl_803C5664[beh->unk04].unk04(a);
        if (fn_8016325C(stream) != 0) {
            fn_80162610(a, stream, 0x1B83833F);
        }
        break;
    }
    return 1;
}

// Sets the behaviour's type-specific state word and clears the actor's
// "busy" condition bit; used as a per-type handler entry.
int RideBehaviourSetIndex(void* argp, int value)
{
    RideActor* a = (RideActor*)argp;
    a->behaviour->unk04 = value;
    ConditionSetBit(a->unk270, 0, 0);
    return 1;
}

// Returns the behaviour's type index (the first word of the behaviour block).
int RideBehaviourGetType(void* argp)
{
    RideActor* a = (RideActor*)argp;
    return a->behaviour->type;
}

// Refreshes the ride behaviour: when no ride stream state exists yet, or the
// entity reports a valid ride target, the per-frame ride update runs; the
// behaviour's type index is returned either way.
int RideRefreshStreamState(void* argp)
{
    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;

    if (a->unk5F0 == 0 || fn_80212730(a->unk038) != -1) {
        RideHostUpdate(a);
    }
    return beh->type;
}

// Runs the per-frame ride update and returns the behaviour's type index.
int RideUpdateAndGetType(void* argp)
{
    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;

    RideHostUpdate(a);
    return beh->type;
}

// Per-frame ride stream handler: refreshes the ride update, re-sends the
// switch event while the stream reports it is still starting up, and reports
// 0 (no ride target), 2 (condition reached) or the behaviour type.
int RideStreamStateHandler(void* argp)
{
    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;
    u8* stream = a->unk5F4 + (0x894 - 0x5F4);  // 0x894  ride state stream object

    RideHostUpdate(a);
    if (fn_8016325C(stream) == 6) {
        fn_80162610(a, stream, 0xE6B71BCC);
        fn_80162610(a, stream, 0x1B83833F);
    } else if (fn_8016325C(stream) == 7) {
        fn_80162610(a, stream, 0x968CBE3D);
        fn_80162610(a, stream, 0x1B83833F);
    }
    if (a->unk288 == 0) {
        return 0;
    }
    if (HasCondition(a->unk270, 4) == 0) {
        return beh->type;
    }
    ConditionSetBit(a->unk270, 4, 0);
    return 2;
}

// Refreshes the ride stream, forwards its startup/end events, then reports 1
// while a ride target is linked and the behaviour type otherwise.
int RideStreamUpdateHandler(void* argp)
{
    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;
    u8* stream = a->unk5F4 + (0x894 - 0x5F4);  // 0x894  ride state stream object

    RideHostUpdate(a);
    if (fn_8016325C(stream) == 6) {
        fn_80162610(a, stream, 0xE6B71BCC);
        fn_80162610(a, stream, 0x1B83833F);
    } else if (fn_8016325C(stream) == 7) {
        fn_80162610(a, stream, 0x968CBE3D);
        fn_80162610(a, stream, 0x1B83833F);
    }
    if (a->unk288 == 0) {
        return beh->type;
    }
    return 1;
}

// Starts the ride: installs the range-field reset callback, sets the behaviour
// type, restarts the ride effect, and queues the ride animation once.
int RideStartHandler(void* argp)
{
    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;

    a->unk10C = RangeResetFields;
    beh->type = 4;
    beh->unk04 = 4;
    if (a->unk5F0 == 0 || fn_80212730(a->unk038) != -1) {
        fn_8008DE4C(a->unk0D4, 0, beh->unk08, 0x18);
    }
    if (HasCondition(a->unk270, 10) == 0) {
        ConditionSetBit(a->unk270, 10, 1);
        ActorPlayAnim(a->unk038, 0xDC04E3D5, a->unk038);
    }
    return 1;
}

// Enters the "broken" behaviour type (3): flags the actor as destroyed, then
// picks and plays one of the animation sets of that behaviour type.
int RideEnterBrokenType(void* argp)
{
    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;

    beh->type = 3;
    beh->unk04 = 4;
    a->unk220 = 0;
    ConditionSetBit(a->unk270, 0, 1);
    ConditionSetBit(a->unk270, 9, 1);
    if ((a->unk5F0 == 0 || fn_80212730(a->unk038) != -1) && a != 0) {
        RideBehaviour* b = a->behaviour;
        if (HasCondition(a->unk270, 0xC) != 0) {
            ConditionSetBit(a->unk270, 0xC, 0);
            ConditionSetBit(a->unk270, 0xE, 1);
            fn_8008F4E8(a->unk0D4, 0);
            fn_8008F378(a->unk0D4);
            fn_8008F4EC(a->unk0D4, 0);
        }
        int count = AnimBankGetSetCount(a->state->unk14, a->state->unk1C,
                                        lbl_803C5664[b->type].unk00);
        if (count != 0) {
            int idx = Rand() % count;
            int anim = fn_800A1BC8(a->state->unk14, a->state->unk1C,
                                   lbl_803C5664[b->type].unk00, idx, 0);
            if (fn_8008E7D0() != 0) {
                ConditionSetBit(a->unk270, 0x1C, 0);
                if (HasCondition(a->unk270, 0xE) == 0 && anim != b->unk08) {
                    fn_8008F230(lbl_804DB150, a->unk0D4, 0, 1);
                } else {
                    fn_8008F230(lbl_804DB120, a->unk0D4, 0, 0);
                    ConditionSetBit(a->unk270, 0xE, 0);
                }
                fn_80090974(lbl_804DB124, a->unk0D4, 0, anim, 9,
                            lbl_803C5664[b->type].unk10, a);
                b->unk08 = anim;
            }
        }
    }
    return 1;
}

// Enters the "damaged" behaviour type (2): flags the actor as hurt, then
// re-plays the animation set it used last (a random one if that is stale).
int RideEnterDamagedType(void* argp)
{
    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;

    beh->type = 2;
    beh->unk04 = 1;
    int idx = beh->lastPart;
    if (a != 0) {
        RideBehaviour* b = a->behaviour;
        if (HasCondition(a->unk270, 0xC) != 0) {
            ConditionSetBit(a->unk270, 0xC, 0);
            ConditionSetBit(a->unk270, 0xE, 1);
            fn_8008F4E8(a->unk0D4, 0);
            fn_8008F378(a->unk0D4);
            fn_8008F4EC(a->unk0D4, 0);
        }
        int count = AnimBankGetSetCount(a->state->unk14, a->state->unk1C,
                                        lbl_803C5664[b->type].unk00);
        if (count != 0) {
            if (idx < 0 || idx >= count) {
                idx = Rand() % count;
            }
            int anim = fn_800A1BC8(a->state->unk14, a->state->unk1C,
                                   lbl_803C5664[b->type].unk00, idx, 0);
            if (fn_8008E7D0() != 0) {
                ConditionSetBit(a->unk270, 0x1C, 0);
                if (HasCondition(a->unk270, 0xE) == 0 && anim != b->unk08) {
                    fn_8008F230(lbl_804DB150, a->unk0D4, 0, 1);
                } else {
                    fn_8008F230(lbl_804DB120, a->unk0D4, 0, 0);
                    ConditionSetBit(a->unk270, 0xE, 0);
                }
                fn_80090974(lbl_804DB124, a->unk0D4, 0, anim, 9,
                            lbl_803C5664[b->type].unk10, a);
                b->unk08 = anim;
            }
        }
    }
    return 1;
}

// Enters the "intact" behaviour type (1): picks the fade-in value once, selects
// the animation index from the actor's damage distance and plays the set.
extern "C" f32 lbl_804D3CFC;
extern "C" f32 lbl_804D3D00;
extern "C" f32 lbl_804DB154;
extern "C" f32 lbl_804DB158;
extern "C" f32 lbl_804DB15C;
extern "C" int rand(void);

int RideEnterIntactType(void* argp)
{
    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;

    if (beh->type != 1) {
        beh->unk14 = (lbl_804D3D00 - lbl_804D3CFC) *
                         ((f32)rand() / lbl_804DB154) + lbl_804D3CFC;
    }
    beh->type = 1;
    beh->unk04 = 1;

    f32 span = (f32)a->state->unk30;
    int idx;
    if (a->unk904 < lbl_804DB158 * span) {
        idx = 2;
    } else if (a->unk904 < lbl_804DB15C * span) {
        idx = 1;
    } else {
        idx = 0;
    }

    if (a != 0) {
        RideBehaviour* b = a->behaviour;
        if (HasCondition(a->unk270, 0xC) != 0) {
            ConditionSetBit(a->unk270, 0xC, 0);
            ConditionSetBit(a->unk270, 0xE, 1);
            fn_8008F4E8(a->unk0D4, 0);
            fn_8008F378(a->unk0D4);
            fn_8008F4EC(a->unk0D4, 0);
        }
        int count = AnimBankGetSetCount(a->state->unk14, a->state->unk1C,
                                        lbl_803C5664[b->type].unk00);
        if (count != 0) {
            if (count <= idx) {
                idx = rand() % count;
            }
            int anim = fn_800A1BC8(a->state->unk14, a->state->unk1C,
                                   lbl_803C5664[b->type].unk00, idx, 0);
            if (fn_8008E7D0() != 0) {
                ConditionSetBit(a->unk270, 0x1C, 0);
                if (HasCondition(a->unk270, 0xE) == 0 && anim != b->unk08) {
                    fn_8008F230(lbl_804DB150, a->unk0D4, 0, 1);
                } else {
                    fn_8008F230(lbl_804DB120, a->unk0D4, 0, 0);
                    ConditionSetBit(a->unk270, 0xE, 0);
                }
                fn_80090974(lbl_804DB124, a->unk0D4, 0, anim, 9,
                            lbl_803C5664[b->type].unk10, a);
                b->unk08 = anim;
            }
        }
    }
    return 1;
}

// Enters the idle behaviour type (0): clears the actor's busy bit, then picks
// and plays a random animation set of the type.
int RideEnterIdleType(void* argp)
{
    RideActor* a = (RideActor*)argp;
    RideBehaviour* beh = a->behaviour;

    ConditionSetBit(a->unk270, 0, 1);
    beh->type = 0;
    beh->unk04 = 0;

    if (a != 0) {
        RideBehaviour* b = a->behaviour;
        if (HasCondition(a->unk270, 0xC) != 0) {
            ConditionSetBit(a->unk270, 0xC, 0);
            ConditionSetBit(a->unk270, 0xE, 1);
            fn_8008F4E8(a->unk0D4, 0);
            fn_8008F378(a->unk0D4);
            fn_8008F4EC(a->unk0D4, 0);
        }
        int count = AnimBankGetSetCount(a->state->unk14, a->state->unk1C,
                                        lbl_803C5664[b->type].unk00);
        if (count != 0) {
            int idx = Rand() % count;
            int anim = fn_800A1BC8(a->state->unk14, a->state->unk1C,
                                   lbl_803C5664[b->type].unk00, idx, 0);
            if (fn_8008E7D0() != 0) {
                ConditionSetBit(a->unk270, 0x1C, 0);
                if (HasCondition(a->unk270, 0xE) == 0 && anim != b->unk08) {
                    fn_8008F230(lbl_804DB150, a->unk0D4, 0, 1);
                } else {
                    fn_8008F230(lbl_804DB120, a->unk0D4, 0, 0);
                    ConditionSetBit(a->unk270, 0xE, 0);
                }
                fn_80090974(lbl_804DB124, a->unk0D4, 0, anim, 9,
                            lbl_803C5664[b->type].unk10, a);
                b->unk08 = anim;
            }
        }
    }
    return 1;
}

// Per-frame ride pose/physics update. It advances the rider's transform from
// the animation delta, applies the slope/elevation forces and writes the
// resulting bone matrix back to the model. Called from the actor tick paths.
// TODO RideHostUpdate (0x924 bytes): the DeepSeek draft did not compile (unknown RideActor fields); the
// empty body keeps the file building (unit is NonMatching).
void RideHostUpdate(void* argp)
{
}

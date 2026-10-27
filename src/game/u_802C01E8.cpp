// u_802C01E8.cpp: possession/status behaviour of a possessed actor: its message
// handler (0x400 status query, 0x407/0x409/0x40A anim control, 0x41D/0x41E name
// query, 0x1800 object lookup) and its per-frame state-machine update.
// Function and field names below are descriptive guesses; offsets are from retail.

typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef unsigned long u32;

struct Owner;
struct BehaviorState;
struct BehaviorDesc;

// Growable character buffer (retail 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;                               // 0x04
    int mLength;                               // 0x08
    int mUnk0C;                                // 0x0C
    int mOwned;                                // 0x10
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

// Seeded name hash: retail accumulates in the caller's variable and walks two
// cursors (indexed access strength-reduced), hence the s[i] form.
static inline void StrHash(u32& h, const char* s)
{
    int i;

    if (s == 0) {
        h = 0;
        return;
    }
    for (i = 0; s[i] != 0; i++) {
        h *= 31;
        h += (unsigned char)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
}

// State table entry (0x803C4CE0, 0x10 bytes each): callbacks plus the name
// fragment used to answer the 0x41E query.
struct BehaviorDesc {
    int (*onEnter)(Owner* owner);              // 0x00
    int (*onLeave)(Owner* owner);              // 0x04
    int (*onUpdate)(Owner* owner);             // 0x08
    const char* sep;                           // 0x0C
};

// Live state block pointed to by Owner::state.
struct BehaviorState {
    float timer;                               // 0x00
    float unk04;                               // 0x04
    u8 index;                                  // 0x08
    u8 unk09[3];                               // 0x09
    int unk0C;                                 // 0x0C
    int unk10;                                 // 0x10
    int unk14;                                 // 0x14
    int unk18;                                 // 0x18
    int unk1C;                                 // 0x1C
    int unk20;                                 // 0x20
    int unk24;                                 // 0x24
    int unk28;                                 // 0x28
    int unk2C;                                 // 0x2C
    int unk30;                                 // 0x30
    int unk34;                                 // 0x34
    int unk38;                                 // 0x38
    int unk3C;                                 // 0x3C
    int unk40;                                 // 0x40
    int unk44;                                 // 0x44
    int unk48;                                 // 0x48
    int unk4C;                                 // 0x4C
    int unk50;                                 // 0x50
    int unk54;                                 // 0x54
};

// Message target reached through Owner::target (id at +0x38).
struct MsgTarget {
    u8 unk00[0x38];                            // 0x00
    int id;                                    // 0x38
};

struct Owner {
    u8 unk000[0x30];                                    // 0x000
    int (*msg)(int msg, void* data, Owner* owner);      // 0x030
    u8 unk034[0x038 - 0x034];                           // 0x034
    int id;                                             // 0x038
    u8 unk03C[0x084 - 0x03C];                           // 0x03C
    float f84;                                          // 0x084
    float f88;                                          // 0x088
    float f8C;                                          // 0x08C
    u8 unk090[0xC4 - 0x090];                            // 0x090
    u8* unkC4;                                          // 0xC4
    u8 unk0C8[0x0FC - 0x0C8];                           // 0x0C8
    void (*unkFC)(void);                                // 0x0FC
    u8 unk100[0x104 - 0x100];                           // 0x100
    int unk104;                                         // 0x104
    int unk108;                                         // 0x108
    int (*unk10C)(Owner*);                              // 0x10C
    u8 unk110[0x118 - 0x110];                           // 0x110
    int unk118;                                         // 0x118
    u8 unk11C[0x130 - 0x11C];                           // 0x11C
    BehaviorState* state;                               // 0x130
    float unk134;                                       // 0x134
    float unk138;                                       // 0x138
    u8 unk13C[0x218 - 0x13C];                           // 0x13C
    int unk218;                                         // 0x218
    int unk21C;                                         // 0x21C
    int mode;                                           // 0x220
    int animId;                                         // 0x224
    u32 animHash;                                       // 0x228
    union {
        MsgTarget* target;                              // 0x22C
        float f22C;                                     // 0x22C
    };
    u8 unk230[0x234 - 0x230];                           // 0x230
    float f234;                                         // 0x234
    u8 unk238[0x240 - 0x238];                           // 0x238
    float f240;                                         // 0x240
    float f244;                                         // 0x244
    float f248;                                         // 0x248
    float f24C;                                         // 0x24C
    float f250;                                         // 0x250
    float f254;                                         // 0x254
    float f258;                                         // 0x258
    u8 unk25C[0x270 - 0x25C];                           // 0x25C
    u8 conditions[0x288 - 0x270];                       // 0x270
    int unk288;                                         // 0x288
    float f28C;                                         // 0x28C
    int unk290;                                         // 0x290
    u8 unk294[0x2A8 - 0x294];                           // 0x294
    float f2A8;                                         // 0x2A8
    float f2AC;                                         // 0x2AC
    float f2B0;                                         // 0x2B0
    u8 unk2B4[0x5FC - 0x2B4];                           // 0x2B4
    void (*unk5FC)(void);                               // 0x5FC
    void (*unk600)(void);                               // 0x600
    void (*unk604)(void);                               // 0x604
    u8 unk608[0x894 - 0x608];                           // 0x608
    u8 follow[0x904 - 0x894];                           // 0x894
    float f904;                                         // 0x904
};

// Answer payload of the 0x400 status query: the receiver writes it over the
// first 0x14 bytes of the result String (see the Credits/Host query idiom).
struct QueryAnswer {
    Owner* owner;                              // 0x00
    int id;                                    // 0x04
    u32 hash;                                  // 0x08
    int valid;                                 // 0x0C
    float unk10;                               // 0x10
    float unk14;                               // 0x14
    int flags;                                 // 0x18
    int unk1C;                                 // 0x1C
};

struct QueryResult {
    String name;                               // 0x00
};

struct QueryMsg {
    Owner* owner;                              // 0x00
    int id;                                    // 0x04
    u32 hash;                                  // 0x08
    QueryResult result;                        // 0x0C
};

extern float lbl_804DAD78;                     // shared float constant (other unit)
extern float lbl_804DAD7C;                     // shared float constant (other unit)
extern float lbl_804DAD9C;                     // range factor (other unit)
extern float lbl_804DADB0;                     // range scale (other unit)
extern float lbl_804DADB8;                     // range factor (other unit)
extern float lbl_804DADBC;                     // range factor (other unit)
extern float lbl_804DADC0;                     // range factor (other unit)
extern float lbl_804D39E0;                     // range factor (other unit)
extern BehaviorDesc lbl_803C4CE0[];            // state table (other unit)

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int fn_800B9360(u32 hash, int a, int b, int c);
extern "C" int fn_800C725C(Owner* owner);
extern "C" String fn_800225C8(const String& a, const String& b);
extern "C" void fn_8028C188(Buffer* b, char* data, int len);
extern "C" void fn_80162658(Owner* owner, void* follow);
extern "C" int fn_80019F88(void* data, Owner* owner);
int PossessedApplyStateProperty(void* data);   // 0x802C1978
int PossessedApplyRangeProperty(void* data);   // 0x802C1C10

int OwnerId(Owner* owner);                     // 0x800A3A40
int ActorPlayAnim(int actor, u32 anim, int a); // 0x800C5654
int MemFlagIsSet(void* mem, int flag);         // 0x800A7B78
int HasCondition(void* mem, int cond);         // 0x800A7AC4
void RangeResetPending(void* mem);             // 0x800A7028
void* FollowGetTargetObj(void* follow);        // 0x800A7A18
void FollowSetTargetObj(void* follow, void* obj);    // 0x800A78A0
void ConditionSetBit(void* mem, int bit, int value); // 0x800A7A54

extern "C" int fn_802C01E8(int msg, void* data)
{
    QueryAnswer* q = (QueryAnswer*)data;
    Owner* owner = q->owner;

    switch (msg) {
    case 0x400:
        q->id = OwnerId(owner);
        q->valid = owner->f904 > lbl_804DAD78;
        q->unk10 = lbl_804DAD7C;
        q->unk14 = lbl_804DAD7C;
        q->flags = 2;
        q->unk1C = 0;
        return 1;
    case 0x407:
        return PossessedApplyRangeProperty(data);
    case 0x409:
        return ActorPlayAnim(OwnerId(owner), (u32)q->valid, q->hash) != 0;
    case 0x40A: {
        QueryAnswer a;
        const char* name = "QUERY";
        u32 hash = q->hash;

        a.owner = owner;
        a.id = OwnerId(owner);
        owner->msg(0x400, &a, owner);
        StrHash(hash, name);
        if (ActorPlayAnim(OwnerId(owner), hash, q->id) != 0) {
            q->valid = 0;
            return 1;
        }
        if (MemFlagIsSet(owner->conditions, 0xB) != 0)
            return 1;
        if (owner->f904 > lbl_804DAD78 && HasCondition(owner->conditions, 0xA) == 0 &&
            (a.flags & 2) != 0 && q->hash == 0x3CA34BD6) {
            q->valid = 0x1379C466;
            return 1;
        }
        return 0;
    }
    case 0x41D:
        return PossessedApplyStateProperty(data);
    case 0x41E: {
        QueryMsg* m = (QueryMsg*)data;

        if (q->id == 0x63CE7FBD) {
            q->valid = fn_800B9360(0x71FE0BC0, 0, owner->state->unk0C, 0);
            return 1;
        }
        if (q->id == 0xAD8576FA) {
            String nm(lbl_803C4CE0[owner->state->index].sep);
            String sep("\n");
            const String& text = fn_800225C8(nm, sep);

            fn_8028C188(&m->result.name, text.mData, text.mLength);
            return 0;
        }
        return 0;
    }
    case 0x1800:
        return fn_800C725C(owner);
    }
    return 0;
}

// Per-frame behaviour: cycles the state machine and forwards the anim/step
// progress to the follow block.
extern "C" int fn_802C0564(Owner* owner)
{
    BehaviorState* st = owner->state;

    if (owner->f904 > lbl_804DAD78) {
        if (owner->mode == 2) {
            QueryAnswer q;
            int ok = SendMsg(owner->target->id, 0x400, &q);

            if (ok == 0 || q.valid == 0) {
                owner->mode = 0;
                RangeResetPending(owner->conditions);
            }
        }
        st->timer += owner->unk138;
        if (lbl_803C4CE0[st->index].onUpdate(owner) != 0) {
            u32 v;

            if (owner == 0 || owner->state == 0) {
                v = 0;
            } else {
                FollowGetTargetObj(owner->conditions);
                v = owner->mode != 0;
            }
            if (st->index != v && lbl_803C4CE0[st->index].onLeave(owner) != 0) {
                st->timer = lbl_804DAD78;
                lbl_803C4CE0[v].onEnter(owner);
                ConditionSetBit(owner->conditions, 0x40, 0);
                ConditionSetBit(owner->conditions, 0x41, 0);
                ConditionSetBit(owner->conditions, 0x43, 0);
                ConditionSetBit(owner->conditions, 0x42, 0);
            }
        }
        fn_80162658(owner, owner->follow);
    }
    return 1;
}

void VFree(void* p);                                   // 0x8009DA3C
void* VAlloc(int size);                                // 0x8009DA68
void ConditionUpdateBit(void* mem, int bit, int value); // 0x800A7B08
extern "C" void fn_800CE96C(void);                     // 0x800CE96C
extern "C" void fn_800CF064(void);                     // 0x800CF064
extern "C" void fn_800CC984(void);                     // 0x800CC984
int PossessedFollowUpdate(Owner* owner);                // 0x802C1F08 (defined below)

// Frees the possession behaviour state block and clears the two related
// owner fields.
void PossessedStatusFreeState(Owner* owner)
{
    VFree(owner->state);
    owner->unk104 = 0;
    owner->state = 0;
}

// Allocates the 0x58-byte possession behaviour state, installs the callbacks
// and timers on it, resets its target slots and enters the initial state
// through the 0x803C4CE0 state table.
void PossessedStatusInitState(Owner* owner)
{
    BehaviorState* st = (BehaviorState*)VAlloc(0x58);

    if (st != 0) {
        int eff;

        owner->state = st;
        owner->unkFC = fn_800CE96C;
        owner->unk604 = fn_800CF064;
        owner->unk5FC = fn_800CC984;
        owner->unk600 = fn_800CC984;
        owner->unk10C = PossessedFollowUpdate;
        owner->unk118 = 0;
        owner->unk108 = 0;
        st->timer = lbl_804DAD78;
        if (owner == 0 || owner->state == 0) {
            eff = 0;
        } else {
            FollowGetTargetObj(owner->conditions);
            eff = owner->mode != 0;
        }
        st->index = (u8)eff;
        st->unk09[0] = 0;
        st->unk0C = -1;
        st->unk10 = -1;
        st->unk14 = -1;
        st->unk18 = -1;
        st->unk1C = -1;
        st->unk20 = -1;
        st->unk24 = 0;
        st->unk28 = 0;
        st->unk2C = 0;
        st->unk30 = 0;
        st->unk34 = -1;
        st->unk38 = -1;
        st->unk3C = -1;
        st->unk40 = -1;
        st->unk44 = -1;
        st->unk48 = 0;
        st->unk4C = 0;
        st->unk50 = 0;
        ConditionUpdateBit(owner->conditions, 0x21, 1);
        lbl_803C4CE0[st->index].onEnter(owner);
        st->unk54 = 1;
    }
}

// Always-true predicate (retail returns 1 unconditionally); used as a
// behaviour gate callback.
int PossessedStatusAllowUpdate()
{
    return 1;
}

// Message body sent as 0xC07 with the actor's position (retail size 0x28).
struct AnimRequest {
    u8 unk00[4];                                       // 0x00
    int unk04;                                         // 0x04
    u8 unk08[0x10 - 0x08];                             // 0x08
    int x;                                             // 0x10
    int y;                                             // 0x14
    int z;                                             // 0x18
    u8 unk1C[0x28 - 0x1C];                             // 0x1C
};

// Object returned by the follow query; only the flag word at 0x38 is read.
struct FollowTarget {
    u8 unk00[0x38];                                    // 0x00
    u32 flags;                                         // 0x38
};

extern float lbl_804DAD80;                             // shared float constant (other unit)
extern "C" void* memset(void* dst, int c, unsigned long n);
extern "C" int fn_800B26B8();
extern "C" int fn_800BF2D0(void* p);
extern "C" int fn_800B1EF4(Owner* owner, int kind);
extern "C" void fn_800B261C(Owner* owner, int kind, void* out);
extern "C" void fn_800B22F8(Owner* owner, int kind);
extern "C" void fn_800B1F20(Owner* owner, int kind);
extern "C" void fn_800B1AB8(float v, Owner* owner, int kind, int a);

// Per-frame follow/anim tick: pushes the actor position to the anim target,
// advances the follow range and stores a newly found follow target.
int PossessedStatusFollowTick(Owner* owner)
{
    BehaviorState* st = owner->state;
    int kind = fn_800B26B8();
    FollowTarget* target = (FollowTarget*)FollowGetTargetObj(owner->conditions);

    if (st->unk10 != -1 && fn_800BF2D0(&owner->f84) != -1) {
        AnimRequest req;

        memset(&req, 0, sizeof(req));
        req.unk04 = 1;
        req.x = (int)owner->f84;
        req.y = (int)owner->f88;
        req.z = (int)owner->f8C;
        SendMsg(st->unk10, 0xC07, &req);
    }
    if (kind == 4) {
        RangeResetPending(owner->conditions);
    } else if (fn_800B1EF4(owner, kind) == 0) {
        fn_800B261C(owner, kind, &target);
        fn_800B22F8(owner, kind);
        if (target != 0 && (target->flags >> 24) == 2) {
            fn_800B1F20(owner, kind);
            fn_800B1AB8(lbl_804DAD80, owner, kind, 1);
            FollowSetTargetObj(owner->conditions, target);
            ConditionUpdateBit(owner->conditions, 4, 1);
            return 1;
        }
    }
    return owner->mode == 0;
}

// Enters state 1 of the possession state machine and arms the three-frame
// transition counter. Always reports success.
int PossessedStatusEnterStateOne(Owner* owner)
{
    owner->state->index = 1;
    owner->unk288 = 3;
    return 1;
}

// Payload of the 0x41D name query: each of the four possession object slots
// fills in the display-name String.
struct PossessedNameQuery {
    int unk00;                                 // 0x00
    u32 hash;                                  // 0x04
    int unk08;                                 // 0x08
    String name;                               // 0x0C
};

// Asks the four possession object slots (state +0x14..+0x20) for their
// display names.
int PossessedStatusQueryNames(Owner* owner)
{
    BehaviorState* st = owner->state;

    {
        PossessedNameQuery q;

        q.hash = 0x93116591;
        q.unk08 = 0;
        SendMsg(st->unk14, 0x41D, &q);
    }
    {
        PossessedNameQuery q;

        q.hash = 0x93116591;
        q.unk08 = 0;
        SendMsg(st->unk18, 0x41D, &q);
    }
    {
        PossessedNameQuery q;

        q.hash = 0x93116591;
        q.unk08 = 0;
        SendMsg(st->unk1C, 0x41D, &q);
    }
    {
        PossessedNameQuery q;

        q.hash = 0x93116591;
        q.unk08 = 0;
        SendMsg(st->unk20, 0x41D, &q);
    }
    return 1;
}

// Player singleton returned by GetPlayer (0x80049948). Only the possessed
// behaviour pointer at +0x26E4 is read here.
struct PlayerActor {
    u8 unk0000[0x26E4];                        // 0x0000
    void* possessed;                           // 0x26E4
};

// Answer block written by the target of the 0x400 status query: the owner
// pointer at +0x00 and the follow range at +0x2C.
struct StatusAnswer {
    MsgTarget* owner;                          // 0x00
    u8 unk04[0x2C - 0x04];                     // 0x04
    u32 range;                                 // 0x2C
};

extern float lbl_804DAD84;                     // shared float constant (other unit)
extern float lbl_804DAD90;                     // shared float constant (other unit)
extern float lbl_804DAD94;                     // shared float constant (other unit)
extern float lbl_804D39E4;                     // shared float constant (other unit)
extern float lbl_804D39E8;                     // shared float constant (other unit)
extern float lbl_804D39EC;                     // shared float constant (other unit)

extern float lbl_804D39D0;                     // shared float constant (other unit)
extern float lbl_804D39D4;                     // shared float constant (other unit)
extern float lbl_804D39D8;                     // shared float constant (other unit)
extern float lbl_804D39DC;                     // shared float constant (other unit)
extern float lbl_804DAD88;                     // shared float constant (other unit)
extern float lbl_804DAD8C;                     // shared float constant (other unit)
extern float lbl_804DADB0;                     // shared float constant (other unit)
extern float lbl_804DADB4;                     // shared float constant (other unit)

extern "C" double fabs(double x);
extern "C" unsigned int rand();
PlayerActor* GetPlayer();                      // 0x80049948
extern "C" int fn_800B8B64(int obj, int a, int b, int (*cb)(int, void*, int, int), Owner* owner);
extern "C" int fn_800B8D68(int obj, int a, int b);
int PossessedStatusCompareTarget(int target, void* candidate, int a, int b);

// Per-frame possession update: validates the possessing player, ticks the
// countdown, forwards the actor position to the anim object, tracks the follow
// range against the queried target and finally runs the follow/anim tick.
// TODO(fn_802C0C14): the block between the range update and the follow tick
// (retail 0x802C108C-0x802C16xx) is not transcribed yet.
int PossessedStatusUpdate(Owner* owner)
{
    BehaviorState* st = owner->state;
    PlayerActor* player = GetPlayer();

    if (player == 0 || player->possessed == 0 ||
        (*(u32*)((u8*)player->possessed + 0x38) >> 24) != 1) {
        st->unk50 = 0;
    } else {
        st->unk50 = 1;
        st->unk04 = lbl_804DAD78;
    }
    st->unk04 = st->unk04 - owner->unk138;

    if (st->unk10 != -1 && fn_800BF2D0(&owner->f84) != -1) {
        AnimRequest req;

        memset(&req, 0, sizeof(req));
        req.unk04 = 1;
        req.x = (int)owner->f84;
        req.y = (int)owner->f88;
        req.z = (int)owner->f8C;
        SendMsg(st->unk10, 0xC07, &req);
    }

    if (fabs(owner->f240) < lbl_804DAD84 && fabs(owner->f244) < lbl_804DAD84) {
        StatusAnswer ans;

        owner->f2AC = lbl_804DAD78;
        if (owner->target == 0) {
            int obj;
            int found;

            if (st->unk50 == 0)
                obj = *(int*)((u8*)st + st->unk4C * 4 + 0x40);
            else
                obj = *(int*)((u8*)st + st->unk48 * 4 + 0x34);
            found = fn_800B8B64(obj, 0, 0, PossessedStatusCompareTarget, owner);
            if (found == -1)
                found = fn_800B8D68(obj, 0, 0);
            SendMsg(found, 0x400, &ans);
            if (ans.owner != 0)
                owner->target = ans.owner;
        } else {
            SendMsg(owner->target->id, 0x400, &ans);
        }
        if (ans.owner != 0) {
            float range = (float)ans.range;
            float z = owner->f8C;
            float* step = &owner->f248;

            if (range < z) {
                if (z - range <= lbl_804D39E8 &&
                    (lbl_804DAD84 <= *step || *step <= -lbl_804D39E4)) {
                    float v = *step;

                    if (lbl_804DAD84 <= v && v < lbl_804D39E4) {
                        *step = lbl_804DAD94 * owner->unk138 * lbl_804D39EC + v;
                        if (*step > lbl_804D39E4)
                            *step = lbl_804D39E4;
                    }
                } else {
                    *step = -(lbl_804DAD94 * owner->unk138 * lbl_804D39EC - *step);
                    if (*step < -lbl_804D39E4)
                        *step = -lbl_804D39E4;
                }
            } else {
                if (range - z <= lbl_804D39E8 &&
                    (*step <= lbl_804DAD90 || lbl_804D39E4 <= *step)) {
                    float v = *step;

                    if (v <= lbl_804DAD90 && -lbl_804D39E4 < v) {
                        *step = -(lbl_804DAD94 * owner->unk138 * lbl_804D39EC - v);
                        if (*step < -lbl_804D39E4)
                            *step = -lbl_804D39E4;
                    }
                } else {
                    *step = lbl_804DAD94 * owner->unk138 * lbl_804D39EC + *step;
                    if (*step > lbl_804D39E4)
                        *step = lbl_804D39E4;
                }
            }
        }
    }

    {
        int kind = fn_800B26B8();
        FollowTarget* target = (FollowTarget*)FollowGetTargetObj(owner->conditions);

        if (kind == 4) {
            RangeResetPending(owner->conditions);
        } else if (fn_800B1EF4(owner, kind) == 0) {
            fn_800B261C(owner, kind, &target);
            fn_800B22F8(owner, kind);
            if (target != 0 && (target->flags >> 24) == 2) {
                fn_800B1F20(owner, kind);
                fn_800B1AB8(lbl_804DAD80, owner, kind, 1);
                FollowSetTargetObj(owner->conditions, target);
                ConditionUpdateBit(owner->conditions, 4, 1);
            }
        }
    }
    return 1;
}

// Randomises the possession state on (re)entry: clears the state index, picks a
// new countdown and two random integers, then derives the +0x54 flag from the
// clamp of a value built from the current follow distance.
int PossessedStatusRandomize(Owner* owner)
{
    BehaviorState* st = owner->state;
    float dist;
    float clamped;

    st->index = 0;
    st->unk04 = (lbl_804D39D0 - lbl_804D39D4) * ((float)rand() / lbl_804DAD88) + lbl_804D39D4;
    st->unk24 = (int)((lbl_804D39DC - lbl_804D39D8) * ((float)rand() / lbl_804DAD88) +
                      lbl_804D39D8);
    st->unk28 = (int)((lbl_804D39DC - lbl_804D39D8) * ((float)rand() / lbl_804DAD88) +
                      lbl_804D39D8);

    dist = (float)*(u32*)(owner->unkC4 + 0x30);
    clamped = lbl_804DAD7C - (owner->f904 - lbl_804DAD8C * dist) / dist;
    if (clamped < lbl_804DAD78)
        clamped = lbl_804DAD78;
    else if (clamped > lbl_804DAD7C)
        clamped = lbl_804DAD7C;

    st->unk54 = (float)rand() / lbl_804DAD88 <= lbl_804DADB4 * clamped + lbl_804DADB0;
    return 1;
}

// Payload of the 0x41D "set state property" message.
struct PropertySetMsg {
    Owner* owner;                              // 0x00
    int hash;                                  // 0x04
    int unk08;                                 // 0x08
    int value;                                 // 0x0C
};

// 0x41D handler: writes one property of the live state block, keyed by the
// property name hash. -1 in the payload means "leave it alone". The target
// property (0x63F94A69) instead re-targets the actor through the 0x400 query.
int PossessedApplyStateProperty(void* data)
{
    PropertySetMsg* q = (PropertySetMsg*)data;
    Owner* owner = q->owner;
    BehaviorState* state;
    int answer[8];

    if (fn_80019F88(data, owner) != 0)
        return 1;
    state = owner->state;
    switch (q->hash) {
    case 0x63F94A69:
        if (q->value == -1) {
            owner->mode = 0;
            owner->target = 0;
            RangeResetPending(owner->conditions);
        } else {
            SendMsg(q->value, 0x400, answer);
            owner->mode = 2;
            owner->target = (MsgTarget*)answer[0];
            FollowSetTargetObj(owner->conditions, (void*)answer[0]);
        }
        return 1;
    case 0x94D64981:
        if (q->value != -1)
            state->unk14 = q->value;
        return 1;
    case 0x94D65D67:
        if (q->value != -1)
            state->unk18 = q->value;
        return 1;
    case 0xA604ED5C:
        if (q->value != -1)
            state->unk1C = q->value;
        return 1;
    case 0x4342FCC2:
        if (q->value != -1)
            state->unk20 = q->value;
        return 1;
    case 0x476F629C:
        if (q->value != -1)
            state->unk40 = q->value;
        return 1;
    case 0x476F7682:
        if (q->value != -1)
            state->unk44 = q->value;
        return 1;
    case 0x84DE0B52:
        if (q->value != -1)
            state->unk34 = q->value;
        return 1;
    case 0x84DE1F38:
        if (q->value != -1)
            state->unk38 = q->value;
        return 1;
    case 0xC5CC738A:
        if (q->value != -1)
            state->unk3C = q->value;
        return 1;
    case 0x3585F265:
        if (q->value != -1)
            state->unk0C = q->value;
        return 1;
    case 0x7772B926:
        if (q->value != -1)
            state->unk10 = q->value;
        return 1;
    }
    return 0;
}

// Payload of the 0x407 "range/animation property" message.
struct PropertyAnimMsg {
    Owner* owner;                              // 0x00
    int unk04;                                 // 0x04
    u32 type;                                  // 0x08
    float value;                               // 0x0C
    int unk10;                                 // 0x10
    u32 name;                                  // 0x14
    u8 unk18[0x58 - 0x18];                     // 0x18
    int unk58;                                 // 0x58
};

// 0x407 handler: only answers for the "range" property class (type top byte 2).
// The name hash scales the requested range, then the actor's distance (0x904)
// picks which of the three range rings it is in and sets the matching state.
int PossessedApplyRangeProperty(void* data)
{
    PropertyAnimMsg* q = (PropertyAnimMsg*)data;
    Owner* owner = q->owner;
    BehaviorState* state = owner->state;
    u32 tick;
    float limit;

    if ((q->type >> 24) != 2) {
        q->unk58 = 1;
        return 1;
    }
    tick = *(u32*)(owner->unkC4 + 0x30);
    if (q->name == 0x403548D0)
        q->value = (float)tick * lbl_804DADB8;
    if (q->name == 0xB9FC42F5) {
        q->value = (float)tick * lbl_804D39E0;
        ActorPlayAnim(owner->id, 0xEC365A5E, q->type);
    }
    if (q->name == 0x75715902)
        q->value *= lbl_804DADB0;

    limit = (float)tick * lbl_804DADBC;
    if (owner->f904 > limit && limit >= owner->f904 - q->value) {
        ActorPlayAnim(owner->id, 0x24AE8385, q->type);
        state->unk4C = 0;
        state->unk48 = 1;
    } else {
        limit = (float)tick * lbl_804DAD9C;
        if (owner->f904 > limit && limit >= owner->f904 - q->value) {
            ActorPlayAnim(owner->id, 0xECC78791, q->type);
            state->unk4C = 1;
            state->unk48 = 2;
        } else {
            limit = (float)tick * lbl_804DADC0;
            if (limit < owner->f904 && owner->f904 - q->value <= limit) {
                ActorPlayAnim(owner->id, 0x96BCC25A, q->type);
                state->unk4C = 1;
                state->unk48 = 2;
            }
        }
    }
    return 0;
}

// View of the object answering the 0x400 status query: the answering object
// at +0x00 and its world position at +0x24.
struct StatusObject {
    void* owner;                               // 0x00
    u8 unk04[0x24 - 0x04];                     // 0x04
    float x;                                   // 0x24
    float y;                                   // 0x28
    float z;                                   // 0x2C
};

extern "C" int fn_8001F928(void* obj, float* pos, int msg, void (*cb)(void), int a);
extern "C" void fn_802C24DC(void);

// 0x800B8B64 candidate callback: asks the candidate for its 0x400 status
// answer and compares its position with ours through the 0x9000A200 query.
// Returns 1 when the query found no match.
int PossessedStatusCompareTarget(int target, void* candidate, int unused0, int unused1)
{
    StatusObject* obj = (StatusObject*)candidate;
    StatusObject answer;
    float pos[6];

    SendMsg(target, 0x400, &answer);
    if (answer.owner == 0)
        return 0;
    pos[0] = obj->x;
    pos[1] = obj->y;
    pos[2] = obj->z;
    pos[3] = ((StatusObject*)answer.owner)->x;
    pos[4] = ((StatusObject*)answer.owner)->y;
    pos[5] = ((StatusObject*)answer.owner)->z;
    return fn_8001F928(candidate, pos, 0x9000A200, fn_802C24DC, 0) == 0;
}

// Candidate target position (three ints at +0x24).
struct PossessedTargetPos {
    u8 unk00[0x24];                            // 0x00
    int x;                                     // 0x24
    int y;                                     // 0x28
    int z;                                     // 0x2C
};

extern "C" void FollowGetTargetPos(void* follow, float* pos);      // 0x800A794C
extern "C" float ActorAngleToPos(void* owner, void* pos);          // 0x800AA9FC
extern "C" void fn_8016D440(float* out, float* in);                // 0x8016D440
extern "C" void fn_800E1098(float angle, float* out, float* in);   // 0x800E1098
extern "C" void Vec3Set(float* dst, float x, float y, float z);    // 0x8016D67C
extern "C" void EulerNormalize(float* dst, float* src);            // 0x8016D55C
extern "C" int FollowOffsetUpdate(void* p);
extern float lbl_804DADC4;
extern float lbl_804DADC8;
extern float lbl_804DADCC;
extern float lbl_804DADD0;
extern float lbl_804DADD4;
extern float lbl_804DADD8;

// Per-frame follow/position update installed as owner->unk10C: tracks the
// follow target position, clamps the approach angle, turns the offset into a
// direction vector and plays the matching animation.
int PossessedFollowUpdate(Owner* owner)
{
    if (owner == 0)
        return 0;

    if (owner->unk290 != 0 && MemFlagIsSet(owner->conditions, 0x16) != 0 &&
        *(int*)owner->unkC4 != 0x8FA05C1A) {
        float target[3];
        float angle;

        FollowGetTargetPos(owner->conditions, target);
        angle = ActorAngleToPos(owner, target);
        owner->f2B0 = angle;
        if (owner->f2B0 < lbl_804DADC4)
            owner->f2B0 = lbl_804DADC4;
        else if (owner->f2B0 > lbl_804DADC8)
            owner->f2B0 = lbl_804DADC8;
    } else {
        owner->f2B0 = lbl_804DAD78;
    }

    if (owner->mode == 2 || owner->mode == 1) {
        float x = owner->f84;
        float y = owner->f88;
        float z = owner->f8C;
        float target[3];
        float av[3];
        float bv[3];
        float ra[3];
        float rb[3];
        float dot;
        float limit;

        if (owner->unk21C != 0) {
            PossessedTargetPos* t = (PossessedTargetPos*)owner->target;
            float fx = (float)t->x;
            float fy = (float)t->y;
            float fz = (float)t->z;

            owner->unk288 = owner->unk21C;
            target[0] = fx;
            target[1] = fy;
            target[2] = fz;
            *(float*)(owner->conditions + 0x00) = fx - x;
            *(float*)(owner->conditions + 0x04) = fy - y;
            *(float*)(owner->conditions + 0x08) = fz - z;
            owner->f2AC = *(float*)(owner->unkC4 + owner->unk288 * 4 + 0xF4);
            owner->f2A8 = *(float*)(owner->unkC4 + 0xF0);
            owner->f28C = *(float*)(owner->unkC4 + owner->unk288 * 4 + 0xE4);
        } else {
            owner->unk288 = owner->unk218;
            *(float*)(owner->conditions + 0x00) = 0.0f;
            *(float*)(owner->conditions + 0x04) = 0.0f;
            *(float*)(owner->conditions + 0x08) = 0.0f;
            owner->f2AC = *(float*)(owner->unkC4 + 0xF4);
            owner->f2A8 = *(float*)(owner->unkC4 + 0xE4);
            owner->f28C = *(float*)(owner->unkC4 + 0xE4);
            ConditionUpdateBit(owner->conditions, 4, 1);
        }

        if (MemFlagIsSet(owner->conditions, 4) != 0)
            FollowGetTargetPos(owner->conditions, target);

        target[0] = target[0] - x;
        target[1] = target[1] - y;
        target[2] = target[2] - z;

        if (((lbl_804DAD84 < fabs(target[0])) + (lbl_804DAD84 < fabs(target[1])) +
             (lbl_804DAD84 < fabs(target[2]))) == 0) {
            Vec3Set((float*)(owner->conditions + 0x0C), 0.0f, 0.0f, 0.0f);
        } else {
            float res[3];

            fn_8016D440(res, target);
            Vec3Set((float*)(owner->conditions + 0x0C),
                    res[2] - owner->f254, res[0] - owner->f24C, 0.0f);
            EulerNormalize((float*)(owner->conditions + 0x0C),
                           (float*)(owner->conditions + 0x0C));
        }

        av[0] = owner->f240;
        av[1] = owner->f244;
        av[2] = 0.0f;
        bv[0] = *(float*)(owner->conditions + 0x00);
        bv[1] = *(float*)(owner->conditions + 0x04);
        bv[2] = 0.0f;
        fn_800E1098(lbl_804DAD7C, ra, av);
        fn_800E1098(lbl_804DAD7C, rb, bv);
        dot = ra[0] * rb[0] + ra[1] * rb[1] + ra[2] * rb[2];

        limit = lbl_804DADD4;
        if (lbl_804DADCC < dot) {
            limit = (av[2] * av[2] + av[0] * av[0] + av[1] * av[1]) /
                    (lbl_804DADD0 * *(float*)(owner->unkC4 + 0xF8));
            limit = limit * limit;
        }

        if ((owner->unk21C == 0 && fabs(*(float*)(owner->conditions + 0x14)) < lbl_804DAD84) ||
            (bv[0] * bv[0] + bv[1] * bv[1] + bv[2] * bv[2] < limit)) {
            if (owner->mode == 2 && owner->target != 0 &&
                (*(u32*)((u8*)owner->target + 0x38) >> 24) == 3) {
                owner->mode = 0;
                owner->unk288 = 1;
                owner->f2AC = *(float*)(owner->unkC4 + 0xF8);
                owner->f248 = 0.0f;
                ActorPlayAnim(owner->animId, owner->animHash, OwnerId(owner));
            } else {
                owner->mode = 0;
                ActorPlayAnim(owner->animId, owner->animHash, OwnerId(owner));
            }
        }
        return 1;
    }

    if (owner->mode == 3) {
        float dx = owner->f234 - owner->f254;
        float dy = owner->f22C - owner->f24C;

        if (fabs(dx) < lbl_804DAD84 && fabs(dy) < lbl_804DAD84) {
            owner->mode = 0;
            ActorPlayAnim(owner->animId, owner->animHash, OwnerId(owner));
            return 1;
        }
        if (MemFlagIsSet(owner->conditions, 0x16) != 0 &&
            fabs(owner->f2B0 - owner->f258) < lbl_804DADD8)
            return 1;
        owner->unk288 = owner->unk218;
        Vec3Set((float*)(owner->conditions + 0x0C), dx, dy, 0.0f);
        EulerNormalize((float*)(owner->conditions + 0x0C),
                       (float*)(owner->conditions + 0x0C));
        owner->f2AC = *(float*)(owner->unkC4 + 0xF4);
        owner->f2A8 = *(float*)(owner->unkC4 + 0xE4);
        owner->f28C = *(float*)(owner->unkC4 + 0xE4);
        return 1;
    }

    {
        int reset = owner->f2AC < lbl_804DAD84;
        int result = FollowOffsetUpdate(owner);

        if (reset != 0) {
            owner->f2AC = 0.0f;
            owner->f28C = owner->f248;
        }
        return result;
    }
}

// Always-on flag queried from the effects system (retail 0x802C24DC).
int IsEnabled() {
    return 1;
}

// ---- effect-particle ring buffer (retail 0x802C24E4) ----
// 32 entries of 0x2C bytes; entry addresses come from a pointer allocated
// elsewhere, so only the entry layout and the cursor live here.
struct EffectParticle {
    float x;          // 0x00
    float y;          // 0x04
    float z;          // 0x08
    u8 color[4];      // 0x0C RGBA
    float scale;      // 0x10
    float speed;      // 0x14
    u8 life;          // 0x18
    u8 alpha;         // 0x19
    s16 id;           // 0x1A
    float unk1C;      // 0x1C
    float unk20;      // 0x20
    float unk24;      // 0x24
    int unk28;        // 0x28
};

// Table at 0x803C4D00: three entries, each an asset id plus the texture handle
// loaded from it. The handle at +0x04 is stamped into each particle.
struct EffectTextureEntry {
    int asset;                      // 0x00
    s16 handle;                     // 0x04
    u8 pad06[0x08 - 0x06];          // 0x06
};

extern EffectParticle* lbl_804D57A0;
extern int lbl_804D57A4;
extern EffectTextureEntry lbl_803C4D00[3];
extern float lbl_804DADE0;
extern float lbl_804DADE4;
extern float lbl_804DADE8;
extern float lbl_804DADEC;
extern float lbl_804DADF0;
extern float lbl_804DADF4;
extern float lbl_804DAE00;
extern float lbl_804DAE04;
extern float lbl_804DAE08;
extern float lbl_804DAE0C;
extern float lbl_804DAE10;
extern "C" void* memcpy(void* dest, const void* src, unsigned long n);
extern "C" int fn_800C5B6C(float* pos, float f);

// Emits one particle at `start` moving along `dir` (both converted from
// integer storage); returns 0 when the spawn check at the position fails.
// fn_802C24E4
extern "C" bool fn_802C24E4(int* start, int* dir) {
    float pos[3];
    float vec[3];
    u8 color[4];
    EffectParticle* p;
    int idx;

    color[0] = 0xFF;
    color[1] = 0xFF;
    color[2] = 0xFF;
    color[3] = 0xFF;
    pos[0] = (float)start[0];
    pos[1] = (float)start[1];
    pos[2] = (float)start[2];
    vec[0] = (float)dir[0];
    vec[1] = (float)dir[1];
    vec[2] = (float)dir[2];
    if (fn_800C5B6C(pos, lbl_804DADE0) == 0) {
        return false;
    }
    idx = lbl_804D57A4;
    lbl_804D57A4 = (idx + 1) % 32;
    p = &lbl_804D57A0[idx];
    fn_800E1098(lbl_804DADE4, &p->x, vec);
    p->x += pos[0];
    p->y += pos[1];
    p->z += pos[2];
    memcpy(p->color, color, 4);
    p->unk24 = lbl_804DADEC * ((float)rand() / lbl_804DADE8);
    p->unk1C = lbl_804DADF0;
    p->unk20 = lbl_804DADF0;
    p->scale = lbl_804DADF4;
    p->speed = lbl_804DADE0;
    p->life = 0x32;
    p->alpha = 0xFF;
    p->id = lbl_803C4D00[1].handle;
    p->unk28 = 1;
    return true;
}

// Emits one tinted particle at `start` moving along `dir` (both converted from
// integer storage); returns false when the spawn check at the position fails.
bool SpawnEffectParticleTinted(int* start, int* dir) {
    u8 color[4];
    float pos[3];
    float vec[3];
    EffectParticle* p;
    int idx;

    color[0] = 0x32;
    color[1] = 0x64;
    color[2] = 0x64;
    color[3] = 0xFF;
    pos[0] = (float)start[0];
    pos[1] = (float)start[1];
    pos[2] = (float)start[2];
    vec[0] = (float)dir[0];
    vec[1] = (float)dir[1];
    vec[2] = (float)dir[2];
    if (fn_800C5B6C(pos, lbl_804DAE00) == 0) {
        return false;
    }
    idx = lbl_804D57A4;
    lbl_804D57A4 = (idx + 1) % 32;
    p = &lbl_804D57A0[idx];
    fn_800E1098(lbl_804DAE04, &p->x, vec);
    p->x += pos[0];
    p->y += pos[1];
    p->z += pos[2];
    memcpy(p->color, color, 4);
    p->unk24 = lbl_804DADEC * ((float)rand() / lbl_804DADE8);
    p->unk1C = lbl_804DAE08;
    p->unk20 = lbl_804DAE08;
    p->scale = lbl_804DAE0C;
    p->speed = lbl_804DAE00;
    p->life = 0xFF;
    p->alpha = 0x32;
    p->id = lbl_803C4D00[2].handle;
    p->unk28 = 1;
    return true;
}

// Emits one white particle at `pos` moving along `vec` (retail 0x802C28EC).
// Other units still call this under its placeholder name, so the symbol is
// kept as fn_802C28EC with C linkage; descriptive name: SpawnEffectParticleWhite.
extern "C" bool fn_802C28EC(int* pos, int* dir) {
    u8 color[4];
    float vec[3];
    float posf[3];
    EffectParticle* p;
    int idx;

    posf[0] = (float)pos[0];
    posf[1] = (float)pos[1];
    posf[2] = (float)pos[2];
    vec[0] = (float)dir[0];
    vec[1] = (float)dir[1];
    vec[2] = (float)dir[2];
    color[0] = 0xFF;
    color[1] = 0xFF;
    color[2] = 0xFF;
    color[3] = 0xFF;
    if (fn_800C5B6C(posf, lbl_804DAE10) == 0) {
        return false;
    }
    idx = lbl_804D57A4;
    lbl_804D57A4 = (idx + 1) % 32;
    p = &lbl_804D57A0[idx];
    fn_800E1098(lbl_804DADE4, &p->x, vec);
    p->x += posf[0];
    p->y += posf[1];
    p->z += posf[2];
    memcpy(p->color, color, 4);
    p->unk24 = lbl_804DADEC * ((float)rand() / lbl_804DADE8);
    p->unk1C = lbl_804DADF0;
    p->unk20 = lbl_804DADF0;
    p->scale = lbl_804DADF4;
    p->speed = lbl_804DAE10;
    p->life = 0;
    p->alpha = 0xFF;
    p->id = lbl_803C4D00[0].handle;
    p->unk28 = 0;
    return true;
}

// Generic effect-particle spawner the three wrappers above specialise (retail
// 0x802C2AEC). `start`/`dir` are integer positions/directions, `color` is a
// four-byte RGBA value, and scale/speed/size plus the rotation angle come from
// the caller. The spawn check uses the larger of scale and speed.
// Other units still call this under its placeholder name, so the symbol keeps C
// linkage; descriptive name: SpawnEffectParticleEx.
extern "C" bool fn_802C2AEC(int* start, int* dir, float scale, float speed, float size,
                            float angle, u8* color, s16 id, u8 life, u8 alpha, int unk28) {
    float pos[3];
    float vec[3];
    EffectParticle* p;
    int idx;

    pos[0] = (float)start[0];
    pos[1] = (float)start[1];
    pos[2] = (float)start[2];
    vec[0] = (float)dir[0];
    vec[1] = (float)dir[1];
    vec[2] = (float)dir[2];
    if (fn_800C5B6C(pos, scale > speed ? scale : speed) == 0) {
        return false;
    }
    idx = lbl_804D57A4;
    lbl_804D57A4 = (idx + 1) % 32;
    p = &lbl_804D57A0[idx];
    fn_800E1098(angle, &p->x, vec);
    p->x += pos[0];
    p->y += pos[1];
    p->z += pos[2];
    memcpy(p->color, color, 4);
    p->unk24 = lbl_804DADEC * ((float)rand() / lbl_804DADE8);
    p->unk1C = size;
    p->unk20 = size;
    p->scale = scale;
    p->speed = speed;
    p->life = life;
    p->alpha = alpha;
    p->id = id;
    p->unk28 = unk28;
    return true;
}

// Releases the effect-particle ring buffer and resets its cursor (retail
// 0x802C2D20). Other units still call this under its placeholder name, so the
// symbol keeps C linkage; descriptive name: EffectParticlePoolShutdown.
extern "C" void fn_802C2D20(void) {
    if (lbl_804D57A0 != 0) {
        VFree(lbl_804D57A0);
        lbl_804D57A0 = 0;
    }
    lbl_804D57A4 = 0;
}

// ---- effect-particle drawing passes (retail 0x802C2D5C, 0x802C2E5C) ----
// Each entry is a camera-facing quad whose near/far colour pair (life/alpha)
// and size pair (scale/speed) are blended by t = unk1C / unk20.
extern float lbl_804DAE14;   // 0.0f
extern float lbl_804DAE18;   // 1.0f
extern void Draw3DBegin(int id, void* data);
extern void DrawWorldQuadFlipped(float size, float angle, float* pos,
                                 u8 r, u8 g, u8 b, u8 a);
extern void Draw3DEnd(void);

// Draws the particles whose category field (unk28) is 0 (retail 0x802C2D5C).
// fn_802C2D5C
extern "C" void fn_802C2D5C(void) {
    int i;

    for (i = 0; i < 32; i++) {
        EffectParticle* p = &lbl_804D57A0[i];
        float t;

        if (p->unk28 != 0 && lbl_804DAE14 < p->unk1C) {
            Draw3DBegin(p->id, 0);
            t = p->unk1C / p->unk20;
            DrawWorldQuadFlipped(p->scale * t + p->speed * (lbl_804DAE18 - t),
                                 p->unk24, &p->x, p->color[0], p->color[1], p->color[2],
                                 (int)((float)p->life * t +
                                       (float)p->alpha * (lbl_804DAE18 - t)));
            Draw3DEnd();
        }
    }
}

// Draws every particle whose category field (unk28) is not 1 (retail 0x802C2E5C).
// fn_802C2E5C
extern "C" void fn_802C2E5C(void) {
    int i;

    for (i = 0; i < 32; i++) {
        EffectParticle* p = &lbl_804D57A0[i];
        float t;

        if (p->unk28 != 1 && lbl_804DAE14 < p->unk1C) {
            Draw3DBegin(p->id, 0);
            t = p->unk1C / p->unk20;
            DrawWorldQuadFlipped(p->scale * t + p->speed * (lbl_804DAE18 - t),
                                 p->unk24, &p->x, p->color[0], p->color[1], p->color[2],
                                 (int)((float)p->life * t +
                                       (float)p->alpha * (lbl_804DAE18 - t)));
            Draw3DEnd();
        }
    }
}

// Decrements the timer field (unk1C) of all 32 effect particles by one frame's
// delta, clamping expired timers to zero (retail 0x802C2F5C). Other units still
// call this under its placeholder name, so the symbol keeps C linkage;
// descriptive name: EffectParticlesTickTimers.
extern "C" void fn_802C2F5C(float dt)
{
    int i;

    for (i = 0; i < 32; i++) {
        if (lbl_804D57A0[i].unk1C <= dt)
            lbl_804D57A0[i].unk1C = lbl_804DADF4;
        else
            lbl_804D57A0[i].unk1C -= dt;
    }
}

int AssetFind(u32 type, u32 id);                // 0x8007EB34
void TextureLoad(s16* handle, int flag);        // 0x8000F738

// Allocates the 32-entry effect-particle ring buffer, clears its cursor and
// resolves and loads the three effect textures (retail 0x802C301C). Other units
// still call this under its placeholder name, so the symbol keeps C linkage;
// descriptive name: EffectParticlePoolInit.
extern "C" void fn_802C301C(void)
{
    u32 i;

    lbl_804D57A0 = (EffectParticle*)VAlloc(0x580);
    lbl_804D57A4 = 0;
    for (i = 0; i != 3; i++) {
        lbl_803C4D00[i].handle = (s16)AssetFind(0xF2E2BEB8, lbl_803C4D00[i].asset);
        TextureLoad(&lbl_803C4D00[i].handle, 1);
    }
}

// No-op hook: still called as a placeholder from another unit, so it keeps
// its symbol name. Retail is a single `blr`.
extern "C" void fn_802C30A0(void) {
}

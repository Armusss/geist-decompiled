// u_80116B28.cpp: owner type-id, ramp and type-name helpers (0x80116B28-0x8011818C).
// Names are descriptive guesses; the code is matched against the retail DOL.

// Names of functions and fields are descriptive guesses; refine while matching.

extern "C" void* memset(void* dst, int val, unsigned long size);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int fn_80116E30(int self);

// A {value, limit} pair: value ramps up toward limit.
struct U80116B28Range {
    float value;    // 0x0
    float limit;    // 0x4
};

struct IVec3 {
    int x;
    int y;
    int z;
};

struct U80116B28Actor;

// One 12-byte skeleton entry of the actor's bone table.
struct U80116B28BoneEntry {
    int handle;     // 0x0
    int field_4;    // 0x4
    int field_8;    // 0x8
};

struct U80116B28BoneSet {
    char pad00[0x30];
    U80116B28BoneEntry entries[6];    // 0x30
};

// The world object attached to the owner at 0xC4.
struct U80116B28Actor {
    char pad00[0x10];
    int field_10;                     // 0x10
    char pad14[8];
    int field_1C;                     // 0x1C
    char pad20[0x10];
    int field_30;                     // 0x30
    char pad34[0x14];
    int field_48;                     // 0x48
    char pad4C[0x94];
    float field_E0;                   // 0xE0
    char padE4[0x28];
    int field_10C[4];                 // 0x10C
    char pad11C[0x18];
    int field_134;                    // 0x134
    int field_138;                    // 0x138
    int field_13C;                    // 0x13C
    int field_140;                    // 0x140
    int field_144;                    // 0x144
    char pad148[0x24];
    int field_16C;                    // 0x16C
};

struct U80116B28Sub {
    int field_0;                      // 0x00
    int field_4;                      // 0x04
    U80116B28Range rangeA;            // 0x08
    int field_10;                     // 0x10
    int field_14;                     // 0x14
    char pad18[4];
    U80116B28Actor* field_1C;         // 0x1C
    char pad20[0x3C];
    float field_5C;                   // 0x5C
    U80116B28Range rangeB;            // 0x60
    char pad68[0x14];
    int field_7C;                     // 0x7C
    int field_80;                     // 0x80
    int field_84;                     // 0x84
    char pad88[0x34];
    int field_BC;                     // 0xBC: current type id, -1 when none
    int field_C0;                     // 0xC0
    int field_C4;                     // 0xC4
    int field_C8;                     // 0xC8
    unsigned int field_CC;            // 0xCC: id in the global type table
    int field_D0;                     // 0xD0
    int field_D4;                     // 0xD4
    int field_D8;                     // 0xD8
    int field_DC;                     // 0xDC
    int field_E0;                     // 0xE0
    float field_E4;                   // 0xE4
    char field_E8[0x100];             // 0xE8: zone query storage
    char field_1E8[0x3C];             // 0x1E8: embedded zone object
};

struct U80116B28Owner {
    char pad00[0x24];
    int field_24;                     // 0x24
    int field_28;                     // 0x28
    int field_2C;                     // 0x2C
    char pad30[8];
    int field_38;                     // 0x38: message target
    char pad3C[0x88];
    U80116B28Actor* field_C4;         // 0xC4
    char padC8[4];
    int field_CC;                     // 0xCC
    int field_D0;                     // 0xD0
    int field_D4;                     // 0xD4
    int field_D8;                     // 0xD8
    int field_DC;                     // 0xDC
    int field_E0;                     // 0xE0
    int field_E4;                     // 0xE4
    U80116B28BoneSet* field_E8;       // 0xE8
    char padEC[4];
    int field_F0;                     // 0xF0
    char padF4[8];
    int field_FC;                     // 0xFC
    char pad100[0x120];
    int field_220;                    // 0x220: state
    char pad224[8];
    int field_22C;                    // 0x22C: stored type id
    char pad230[8];
    int field_238;                    // 0x238
    char pad23C[0x2C];
    int field_268;                    // 0x268
    char pad26C[4];
    char field_270[0x18];             // 0x270: condition block
    int field_288;                    // 0x288
    char pad28C[0x360];
    U80116B28Sub* field_5EC;          // 0x5EC
    int field_5F0;                    // 0x5F0
    char pad5F4[0x10];
    int field_604;                    // 0x604
    char pad608[0x18];
    int field_620;                    // 0x620
    char pad624[0x270];
    char field_894[0x58];             // 0x894
    int field_8EC;                    // 0x8EC
    int field_8F0;                    // 0x8F0
    char pad8F4[0xC];
    float field_900;                  // 0x900
    float field_904;                  // 0x904
};

// Query buffer for message 0x400: the target writes its type id into [0].
struct U80116B28Query {
    int id;        // 0x0
    int pad[7];    // 0x4
};

// Payload for message 0x41E.
struct U80116B28NameMsg {
    int pad00;
    unsigned int nameHash;    // 0x4
    int pad08;                // 0x8
};

// Local view of the String object that receives the type name.
class U80116B28String {
public:
    U80116B28String();
    ~U80116B28String();
    int field_0;              // 0x0
};

// Sets the owner's attached type id; on failure to resolve the name it falls
// back to state 5 with the queried id.
extern "C" int U80116B28SetTypeId(U80116B28Owner* owner, int id)
{
    U80116B28Query query;
    U80116B28Sub* sub;
    int current;
    int result;
    int value;

    memset(&query, 0, 0x20);
    sub = owner->field_5EC;
    current = sub->field_BC;
    if (current == id)
        return 1;
    if (current != -1)
        fn_80116E30((int)owner);
    if (id == -1)
        return 1;
    if (SendMsg(id, 0x400, &query) == 0)
        return 1;
    if (query.id == 0)
        return 1;

    U80116B28NameMsg msg;
    U80116B28String name;

    name.field_0 = 0;
    msg.nameHash = 0x1C1A4CC9;
    msg.pad08 = 0;
    result = SendMsg(owner->field_38, 0x41E, &msg);
    value = name.field_0;
    if (result == 0 || value == -1) {
        owner->field_22C = query.id;
        owner->field_220 = 5;
    }
    return 1;
}

// Per-call ramp parameters (rate and the "clamped" flag out-parameter).
struct U80116B28Rate {
    char pad00[4];
    float rate;    // 0x4
    int flag;      // 0x8: 1 when the value reached its limit
};

// Ramps the relevant range's value toward its limit by `rate` per call and
// stores the result on the owner. Returns 1, or 0 if there is nothing to step.
extern "C" int U80116B28RampValue(U80116B28Owner* owner, U80116B28Rate* rate)
{
    U80116B28Sub* sub;
    U80116B28Range* range;
    bool active;

    if (owner == 0)
        return 0;
    sub = owner->field_5EC;
    if (sub == 0)
        return 0;

    active = sub != 0 && sub->field_BC != -1;
    if (active)
        range = &sub->rangeB;
    else
        range = &sub->rangeA;

    if (range->value < range->limit) {
        rate->flag = 0;
        range->value = range->value + (float)(int)(range->limit * rate->rate);
    } else {
        rate->flag = 1;
    }
    if (range->limit < range->value)
        range->value = range->limit;

    owner->field_904 = range->value;
    return 1;
}

// Destination of the current range values (four floats at 0x10).
struct U80116B28RangeOut {
    char pad00[0x10];
    float x;    // 0x10
    float y;    // 0x14
    float z;    // 0x18
    float w;    // 0x1C
};

// Copies the current range values into `out`: the effect range when a type is
// attached, the base range otherwise. Returns 0 when there is no type object.
// fn_80116D40
extern "C" int fn_80116D40(U80116B28Owner* owner, U80116B28RangeOut* out)
{
    if (owner == 0 || owner->field_5EC == 0)
        return 0;
    if (owner->field_5EC->field_BC == -1) {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = owner->field_5EC->rangeA.value;
        out->w = owner->field_5EC->rangeA.limit;
    } else {
        out->x = owner->field_5EC->rangeB.value;
        out->y = owner->field_5EC->rangeB.limit;
        out->z = owner->field_5EC->rangeA.value;
        out->w = owner->field_5EC->rangeA.limit;
    }
    return 1;
}

// Returns the type id of the attached type object, or 0 when the target does
// not answer the 0x400 query.
// fn_80116DC4
extern "C" int fn_80116DC4(U80116B28Owner* owner)
{
    U80116B28Query query;

    if (SendMsg(owner->field_5EC->field_BC, 0x400, &query) == 0)
        return 0;
    return query.id;
}

// ---- External helpers used by fn_80116E30 ---------------------------------

class Owner;
class ExtraHolder;
class Actor;

int OwnerId(Owner* owner);
void VecSwapXY2(IVec3* dst, IVec3* src);
void VecToAngles(IVec3* dst, IVec3* src);
void MemCopyVec3(float* dst, float* src);
void ActorAim(int* out, Actor* actor, int flag);
void ModelNodeSetup(int a, int b, int c, int d);
int PlayerTeam(int handle);
void ActorPlayAnim(int handle, unsigned long anim, int target);
int ModelBoneCount(int handle);
void ItemColorApply(int handle, int color, int variant);
void VFree(void* ptr);
void* VAlloc(int size);
int HandleToPlayer(int ownerId);
float StatGet(int stat, int index, int who);
void StatSet(int stat, int index, int who, float value);
int Rand(void);
void* Fn800DF814(unsigned long hash, int index);
int AssetFind(unsigned long hash, unsigned long id);
void RangeResetPending(void* cond);
void ConditionUpdateBit(void* cond, int bit, int value);
void ExtraTrailClear(ExtraHolder* holder);
void ExtraTrailInit(ExtraHolder* holder, unsigned long a, unsigned long b, char* data,
                    unsigned long c);

extern "C" void fn_800B6EDC(int handle, int arg);
extern "C" int fn_802A1D04(void);
extern "C" void fn_8009F190(void* out, void* owner);
extern "C" int fn_801BDC0C(void* owner);
extern "C" void fn_801BD7D4(void* owner);
extern "C" void fn_8003F0D4(int arg);
extern "C" void fn_800390F4(int handle);
extern "C" int fn_800391DC(int handle);
extern "C" void fn_801626A0(void* owner, void* out);
extern "C" void fn_801627A4(void* owner, void* out, int value);
extern "C" void fn_801BE9FC(void* owner);
extern "C" void fn_801BEABC(int a, int b, void* owner);
extern "C" int fn_800F8AF4(int handle);
extern "C" void fn_800BBCA8(void* owner);
extern "C" void fn_800CE96C(void);
extern "C" void fn_800CF064(void);
extern "C" void fn_800B5004(void* dst, void* src);
extern "C" void fn_80090634(int a, int b, int c, int d);
extern "C" int fn_801BD694(void* owner);
extern "C" float lbl_804D7758;

// Payload of message 0xC14: {pad, arg, arg, hash}.
struct U80116B28MsgC14 {
    int pad00;
    int field_4;
    int field_8;
    unsigned int hash;
};

// Payload of message 0xC07: a kind followed by two positions.
struct U80116B28MsgC07 {
    int pad00;
    int kind;
    int pad08;
    int pad0C;
    IVec3 pos;        // 0x10
    IVec3 swapped;    // 0x1C
};

// Payload of message 0xC00.
struct U80116B28MsgC00 {
    int pad00;
    int value;
};

// Payload of message 0x407 (a large event record).
struct U80116B28Msg407 {
    int pad00;            // 0x00
    int owner;            // 0x04
    int ownerId;          // 0x08
    float value;          // 0x0C
    int pad10;            // 0x10
    unsigned int hash;    // 0x14
    char pad18[0x2C];     // 0x18
    int field_44;         // 0x44
    char pad48[4];        // 0x48
    float field_4C;       // 0x4C
};

// The type object attached to the owner: message handler at 0x30, message
// target at 0x38 and an embedded condition block at 0x270.
struct U80116B28Type {
    char pad00[0x30];
    int (*handler)(int msg, void* data);    // 0x30
    char pad34[4];
    int field_38;                           // 0x38
    char pad3C[0x234];
    char field_270[0x18];                   // 0x270
};

// Reply buffer of the type's 0x400/0x401 queries: the type id at 0x4, the
// range values at 0x10.
struct U80116B28TypeQuery {
    void* self;        // 0x0
    int id;            // 0x4
    char pad08[8];
    float value;       // 0x10
    float limit;       // 0x14
};

// Payload of the type's 0x40a query: caller, target and a name hash.
struct U80116B28TypeBind {
    void* self;          // 0x0
    int target;          // 0x4
    unsigned int hash;   // 0x8
};

// Reply of message 0x419: the world object the type attaches to.
struct U80116B28Msg419 {
    int pad00;
    int value;    // 0x4
};

// Payload of message 0x402: a halfword flag and the aimed actor at 0x10.
struct U80116B28Msg402 {
    int pad00;         // 0x0
    short field_4;     // 0x4
    char pad06[0xA];
    Actor* actor;      // 0x10
};

// Payload of message 0xC07 sent while attaching: kind, owner id and position.
struct U80116B28MsgC07Aim {
    int pad00;      // 0x0
    int kind;       // 0x4
    int ownerId;    // 0x8
    int pad0C;      // 0xC
    IVec3 pos;      // 0x10
};

// Payload of message 0xC02: an asset id and two flags.
struct U80116B28MsgC02 {
    int pad00;    // 0x0
    int asset;    // 0x4
    int value;    // 0x8
    int flag;     // 0xC
};

// Sends a name request and returns the value the receiver leaves in the
// String object that follows the message payload.
static inline int U80116B28RequestName(int target, int msgId, unsigned int hash, int value)
{
    U80116B28NameMsg msg;
    U80116B28String name;

    name.field_0 = value;
    msg.nameHash = hash;
    msg.pad08 = 0;
    SendMsg(target, msgId, &msg);
    return name.field_0;
}

// Returns whether the owner currently has a type object attached.
extern "C" unsigned char fn_80116E08(void* ownerPtr)
{
    U80116B28Owner* owner = (U80116B28Owner*)ownerPtr;

    return owner->field_5EC != 0 && owner->field_5EC->field_BC != -1;
}

// Detaches the owner's current type object: notifies its player, resets the
// ranges, sends the teardown messages and re-initialises the owner's model,
// bones and trail effect. Returns 0 when nothing is attached.
extern "C" int fn_80116E30(int self)
{
    U80116B28Owner* owner = (U80116B28Owner*)self;
    U80116B28Sub* sub;
    U80116B28Actor* actor;
    U80116B28BoneSet* bones;
    IVec3 raw;
    int typeId;
    int ownerId;
    int target;
    int typeGroup;
    int i;
    int maxBones;
    int bone;
    int value;
    int handle;

    sub = owner->field_5EC;
    if (owner == 0)
        return 0;
    if (sub->field_BC == -1)
        return 0;

    ownerId = OwnerId((Owner*)owner);
    target = OwnerId((Owner*)owner);
    if (target != -1) {
        U80116B28MsgC14 msg;

        msg.field_4 = 0;
        msg.field_8 = ownerId;
        msg.hash = 0x274B01CB;
        SendMsg(target, 0xC14, &msg);
    }

    typeId = sub->field_BC;
    sub->field_BC = -1;
    sub->field_14 = 0;
    sub->field_10 = 0;
    sub->rangeB.limit = sub->rangeA.limit;
    sub->field_5C = sub->rangeA.value;
    sub->rangeB.value = sub->rangeA.value;
    owner->field_C4 = sub->field_1C;
    owner->field_904 = sub->rangeA.value;

    if (owner->field_238 != -1 && ((unsigned int)owner->field_238 >> 24) == 3) {
        fn_800B6EDC(owner->field_238, 0);
        owner->field_238 = -1;
    }

    typeGroup = (unsigned int)typeId >> 24;
    if (typeGroup == 0x13) {
        if (fn_802A1D04() != 3) {
            U80116B28MsgC07 msg;

            fn_8009F190(&raw, owner);
            VecSwapXY2(&msg.swapped, &raw);
            msg.pos.x = owner->field_24;
            msg.pos.y = owner->field_28;
            msg.pos.z = owner->field_2C;
            msg.kind = 3;
            SendMsg(typeId, 0xC07, &msg);
        }
        {
            U80116B28MsgC00 msg;

            msg.value = 1;
            SendMsg(typeId, 0xC00, &msg);
        }
    } else if (typeGroup == 2) {
        U80116B28RequestName(typeId, 0x41D, 0x392F63E9, 1);
    }

    if (fn_801BDC0C(owner) != 0)
        fn_801BD7D4(owner);

    owner->field_8F0 = sub->field_C4;
    owner->field_8EC = sub->field_C0;

    {
        U80116B28Msg407 msg;

        msg.owner = (int)owner;
        msg.ownerId = OwnerId((Owner*)owner);
        msg.field_4C = lbl_804D7758;
        msg.field_44 = -1;
        if (fn_802A1D04() != 3 || typeGroup == 0x13) {
            msg.hash = 0x6CA5F172;
            msg.value = sub->field_5C - sub->rangeB.value;
            SendMsg(typeId, 0x407, &msg);
        }
        msg.value = lbl_804D7758;
        msg.hash = 0x672F35C2;
        SendMsg(typeId, 0x407, &msg);
    }

    ActorPlayAnim(typeId, 0xC6144531, OwnerId((Owner*)owner));
    ActorPlayAnim(OwnerId((Owner*)owner), 0xC6144531, OwnerId((Owner*)owner));

    if (typeId != -1) {
        U80116B28MsgC14 msg;

        msg.field_4 = (int)&sub->field_10;
        msg.field_8 = owner->field_38;
        msg.hash = 0x39CB0CB0;
        SendMsg(typeId, 0xC14, &msg);
    }
    if (owner->field_38 != -1) {
        U80116B28MsgC14 msg;

        msg.field_4 = (int)&sub->field_10;
        msg.field_8 = typeId;
        msg.hash = 0xA867339B;
        SendMsg(owner->field_38, 0xC14, &msg);
    }

    U80116B28RequestName(typeId, 0x41D, 0x1C1A4CC9, -1);

    actor = owner->field_C4;
    owner->field_E8 = (U80116B28BoneSet*)((char*)actor + 0x3C);
    bones = owner->field_E8;
    fn_8003F0D4(actor->field_48);
    fn_800390F4(owner->field_CC);
    owner->field_CC = fn_800391DC(bones->entries[0].handle);
    owner->field_D0 = ModelBoneCount(owner->field_CC);
    if (fn_802A1D04() != 0)
        ItemColorApply(owner->field_CC, -1, -1);

    if (owner->field_F0 != 0) {
        maxBones = ModelBoneCount(bones->entries[0].handle);
        VFree((void*)owner->field_F0);
        for (i = 1; i < 6; i++) {
            bone = ModelBoneCount(bones->entries[i].handle);
            if (maxBones < bone)
                maxBones = ModelBoneCount(bones->entries[i].handle);
        }
        owner->field_F0 = (int)VAlloc(maxBones * 0x1C);
    }

    if (typeGroup == 0x13) {
        value = U80116B28RequestName(0x4B000000, 0x41E, 0x001512C7, 0);
        if (value != 0) {
            handle = HandleToPlayer(OwnerId((Owner*)owner));
            StatGet(0x7BFB940D, 0, handle);
            StatSet(0x7BFB940D, 3, typeId & 0xFFFFFF, 0.0f);
        }
    }

    owner->field_FC = (int)fn_800CE96C;
    fn_801626A0(owner, owner->field_894);
    fn_801627A4(owner, owner->field_894, fn_800F8AF4(owner->field_8F0));
    fn_801BE9FC(owner);
    fn_801BEABC(actor->field_10, actor->field_1C, owner);

    if (actor->field_16C == 0) {
        if (owner->field_5F0 != 0) {
            ExtraTrailClear((ExtraHolder*)owner->field_5F0);
            VFree((void*)owner->field_5F0);
            owner->field_5F0 = 0;
        }
    } else {
        if (owner->field_5F0 == 0)
            owner->field_5F0 = (int)VAlloc(0x20);
        else
            ExtraTrailClear((ExtraHolder*)owner->field_5F0);
        ExtraTrailInit((ExtraHolder*)owner->field_5F0, owner->field_38, owner->field_CC,
                       (char*)owner->field_F0, actor->field_16C);
    }

    owner->field_D8 = AssetFind(0xF1754BF2, actor->field_13C);
    owner->field_DC = AssetFind(0xF1754BF2, actor->field_140);
    owner->field_E0 = AssetFind(0xF1754BF2, actor->field_144);
    owner->field_E4 = AssetFind(0xD9044CBB, actor->field_144);
    owner->field_900 = actor->field_E0;
    owner->field_620 = (int)Fn800DF814(0xE6BCD867, actor->field_10C[Rand() % 4]);

    RangeResetPending(owner->field_270);
    fn_800BBCA8(owner);
    ConditionUpdateBit(owner->field_270, 4, 0);
    owner->field_288 = 0;
    owner->field_268 = 4;

    return 1;
}

// Attaches a type object to the owner's geist state: asks the type for its id
// and range (messages 0x400/0x40a/0x401), forwards the attach events, then
// rebuilds the owner's model, bones, trail and asset references.
// Called by other units as fn_801174B0, so the placeholder name and C linkage
// are kept.
extern "C" int fn_801174B0(void* ownerPtr, int typePtr)
{
    U80116B28Owner* owner = (U80116B28Owner*)ownerPtr;
    U80116B28Type* type = (U80116B28Type*)typePtr;
    U80116B28Sub* sub;
    U80116B28Actor* actor;
    U80116B28BoneSet* bones;
    U80116B28TypeQuery q400;
    U80116B28TypeQuery q401;
    U80116B28TypeBind bind;
    U80116B28Msg419 q419;
    U80116B28Msg402 q402;
    U80116B28MsgC07Aim msgC07;
    U80116B28MsgC02 msgC02a;
    U80116B28MsgC02 msgC02b;
    IVec3 aim;
    IVec3 angles;
    IVec3 swapped;
    float offsets[3];
    int maxBones;
    int bone;
    int i;
    int ownerId;
    int nameA;
    int nameB;

    if (owner == 0 || type == 0)
        return 0;
    sub = owner->field_5EC;
    if (sub->field_BC != -1)
        return 0;

    if (type->field_38 != -1) {
        U80116B28MsgC14 msg;

        msg.field_4 = 0;
        msg.field_8 = owner->field_38;
        msg.hash = 0x672F35C2;
        SendMsg(type->field_38, 0xC14, &msg);
    }

    q400.self = type;
    q401.self = type;
    bind.self = type;
    bind.target = owner->field_38;
    bind.hash = 0x3CA34BD6;

    if (type->handler(0x400, &q400) == 0)
        return 0;
    if (type->handler(0x40A, &bind) == 0)
        return 0;
    if (type->handler(0x401, &q401) == 0)
        return 0;

    sub->field_14 = (int)type;
    sub->field_10 = (int)owner;
    sub->field_BC = q400.id;
    sub->field_5C = q401.value;
    sub->rangeB.value = q401.value;
    sub->rangeB.limit = q401.limit;
    sub->field_1C = owner->field_C4;

    if (SendMsg(sub->field_BC, 0x419, &q419) == 0 || q419.value == 0)
        return 0;
    owner->field_C4 = (U80116B28Actor*)q419.value;
    sub->rangeA.value = owner->field_904;
    owner->field_904 = q401.value;

    ActorPlayAnim(sub->field_BC, 0xC6144531, OwnerId((Owner*)owner));
    ownerId = OwnerId((Owner*)owner);
    ActorPlayAnim(OwnerId((Owner*)owner), 0xC6144531, ownerId);

    q402.field_4 = -1;
    SendMsg(sub->field_BC, 0x402, &q402);

    offsets[0] = lbl_804D7758;
    offsets[1] = lbl_804D7758;
    offsets[2] = lbl_804D7758;
    fn_800B5004((char*)owner + 0x240, offsets);
    MemCopyVec3((float*)owner->field_270, offsets);
    RangeResetPending(owner->field_270);
    owner->field_288 = 0;

    ActorAim((int*)&aim, q402.actor, 1);
    aim.z = 0;
    VecToAngles(&angles, &aim);
    VecSwapXY2(&swapped, &angles);

    msgC07.kind = 3;
    msgC07.ownerId = OwnerId((Owner*)owner);
    msgC07.pos = aim;
    SendMsg(OwnerId((Owner*)owner), 0xC07, &msgC07);

    actor = owner->field_C4;
    owner->field_E8 = (U80116B28BoneSet*)((char*)actor + 0x3C);
    bones = owner->field_E8;
    fn_8003F0D4(actor->field_48);
    fn_800390F4(owner->field_CC);
    owner->field_CC = fn_800391DC(bones->entries[0].handle);
    owner->field_D0 = ModelBoneCount(owner->field_CC);
    if (fn_802A1D04() != 0)
        ItemColorApply(owner->field_CC, -1, PlayerTeam(HandleToPlayer(owner->field_38)));

    if (fn_802A1D04() != 3 && ((unsigned int)type->field_38 >> 24) == 0x13) {
        U80116B28MsgC00 msg;

        ConditionUpdateBit(type->field_270, 0xB, 1);
        msg.value = 0;
        SendMsg(type->field_38, 0xC00, &msg);
    }

    if (owner->field_F0 != 0) {
        maxBones = ModelBoneCount(bones->entries[0].handle);
        VFree((void*)owner->field_F0);
        for (i = 1; i < 6; i++) {
            bone = ModelBoneCount(bones->entries[i].handle);
            if (maxBones < bone)
                maxBones = ModelBoneCount(bones->entries[i].handle);
        }
        owner->field_F0 = (int)VAlloc(maxBones * 0x1C);
    }

    nameA = U80116B28RequestName(sub->field_BC, 0x41E, 0x2DA8293C, 0);
    nameB = U80116B28RequestName(sub->field_BC, 0x41E, 0xDFF3097A, 0);
    sub->field_C4 = owner->field_8F0;
    sub->field_C0 = owner->field_8EC;
    owner->field_8EC = nameA;
    owner->field_8F0 = nameB;

    fn_801626A0(owner, owner->field_894);
    fn_801627A4(owner, owner->field_894, fn_800F8AF4(owner->field_8F0));

    if (U80116B28RequestName(0x4B000000, 0x41E, 0x001512C7, 0) != 0) {
        float stat;
        int player;

        stat = StatGet(0x7BFB940D, 3, sub->field_BC & 0xFFFFFF);
        player = HandleToPlayer(OwnerId((Owner*)owner));
        StatSet(0x7BFB940D, 0, player, stat);
    }

    fn_80090634(owner->field_D4, 0, 0, 0);
    fn_801BE9FC(owner);
    if (fn_801BD694(owner) != 0)
        fn_801BEABC(0x165E8B4A, actor->field_1C, owner);
    else
        fn_801BEABC(actor->field_10, actor->field_1C, owner);

    if (actor->field_16C != 0) {
        if (owner->field_5F0 == 0)
            owner->field_5F0 = (int)VAlloc(0x20);
        else
            ExtraTrailClear((ExtraHolder*)owner->field_5F0);
        ExtraTrailInit((ExtraHolder*)owner->field_5F0, owner->field_38, owner->field_CC,
                       (char*)owner->field_F0, actor->field_16C);
    } else if (owner->field_5F0 != 0) {
        ExtraTrailClear((ExtraHolder*)owner->field_5F0);
        VFree((void*)owner->field_5F0);
        owner->field_5F0 = 0;
    }

    owner->field_FC = (int)fn_800CE96C;
    owner->field_604 = (int)fn_800CF064;
    ModelNodeSetup(owner->field_CC, owner->field_E4, 0, 1);

    if (actor->field_134 != 0) {
        msgC02a.asset = AssetFind(0xD9044CBB, 0x4A2FB1A9);
        msgC02a.value = 0;
        msgC02a.flag = 1;
        SendMsg(owner->field_38, 0xC02, &msgC02a);
        msgC02a.asset = AssetFind(0xD9044CBB, actor->field_134);
        msgC02a.value = 1;
        msgC02a.flag = 1;
        SendMsg(owner->field_38, 0xC02, &msgC02a);
    }
    if (actor->field_138 != 0) {
        msgC02b.asset = AssetFind(0xD9044CBB, 0xEAB66145);
        msgC02b.value = 0;
        msgC02b.flag = 1;
        SendMsg(owner->field_38, 0xC02, &msgC02b);
        msgC02b.asset = AssetFind(0xD9044CBB, actor->field_138);
        msgC02b.value = 1;
        msgC02b.flag = 0;
        SendMsg(owner->field_38, 0xC02, &msgC02b);
    }

    owner->field_D8 = AssetFind(0xF1754BF2, actor->field_13C);
    owner->field_DC = AssetFind(0xF1754BF2, actor->field_140);
    owner->field_E0 = AssetFind(0xF1754BF2, actor->field_144);
    owner->field_E4 = AssetFind(0xD9044CBB, actor->field_144);
    owner->field_900 = actor->field_E0;
    owner->field_620 = (int)Fn800DF814(0xE6BCD867, actor->field_10C[Rand() % 4]);

    U80116B28RequestName(type->field_38, 0x41D, 0x1C1A4CC9, OwnerId((Owner*)owner));
    if (owner->field_38 != -1) {
        U80116B28MsgC14 msg;

        msg.field_4 = (int)type;
        msg.field_8 = type->field_38;
        msg.hash = 0xCC5916FB;
        SendMsg(owner->field_38, 0xC14, &msg);
    }
    return 1;
}

// Releases the owner's attached type state block.
// Called by other units as fn_80117D40, so the placeholder name and C linkage
// are kept.
extern "C" void fn_80117D40(void* agentPtr)
{
    U80116B28Owner* owner = (U80116B28Owner*)agentPtr;

    if (owner->field_5EC != 0) {
        VFree(owner->field_5EC);
        owner->field_5EC = 0;
    }
}

// ---- Zone helpers used when the state block is created -------------------

class GhostSlot;
class ZoneObject;

// One entry of the global per-type reset table at lbl_8039ED68.
struct U80116B28TypeTableEntry {
    unsigned int id;                                // 0x0
    char pad04[4];                                  // 0x4
    void (*reset)(U80116B28Owner* owner, U80116B28Sub* sub);    // 0x8
    char pad0C[4];                                  // 0xC
    int (*dispatch)(void* sub, int arg1, int arg2, int active); // 0x10
    char pad14[4];                                  // 0x14
    const char* name;                               // 0x18
};

void GhostRecordReset(GhostSlot* slot);
void ZoneInit(void* zone, Actor* owner, IVec3* pos, void* data, int count, float value);
void ZoneObjectSetSlot14(ZoneObject* obj, unsigned long value);
void ZoneSetCallback(void* zone, int a, int (*callback)(int, Actor*, Actor*), int b, Actor* owner);
void ZoneObjectSetSlot18(ZoneObject* obj, float value);
extern "C" void fn_800CD5D8(void* owner, int value);
extern "C" float lbl_804D775C;
extern "C" U80116B28TypeTableEntry lbl_8039ED68[];
extern "C" int lbl_804D2E08;
extern "C" char lbl_803457B0[];

// Allocates and initialises the owner's 0x224-byte geist state block, including
// its embedded zone object. Returns 1 if a state already exists (or one was
// created), 0 if the allocation failed.
// Called by other units as fn_80117D80, so the placeholder name and C linkage
// are kept.
extern "C" int fn_80117D80(void* ownerPtr)
{
    U80116B28Owner* owner = (U80116B28Owner*)ownerPtr;
    U80116B28Sub* sub;
    int index;
    int i;

    if (owner->field_5EC != 0)
        return 1;

    sub = (U80116B28Sub*)VAlloc(0x224);
    owner->field_5EC = sub;
    if (sub == 0)
        return 0;
    if (sub != 0) {
        if (sub->field_CC != 0xFFFFFFFF) {
            index = -1;
            for (i = 0; i < lbl_804D2E08; i++) {
                if (sub->field_CC == lbl_8039ED68[i].id) {
                    index = i;
                    break;
                }
            }
            if (index != -1)
                lbl_8039ED68[index].reset(owner, sub);
        }
        sub->field_C8 = -1;
        sub->field_CC = 0xFFFFFFFF;
        sub->field_D0 = -1;
        sub->field_D4 = -1;
    }

    sub->field_0 = 0;
    sub->field_4 = 0;
    sub->field_E0 = 0;
    sub->field_D8 = 0;
    sub->field_DC = 0;
    if (owner->field_C4 == 0) {
        sub->rangeA.value = lbl_804D7758;
        sub->rangeA.limit = lbl_804D7758;
    } else {
        sub->rangeA.value = (float)owner->field_C4->field_30;
        sub->rangeA.limit = sub->rangeA.value;
    }
    sub->field_BC = -1;
    sub->field_C0 = 0;
    sub->field_C4 = 0;
    sub->field_7C = -1;
    sub->field_80 = -1;
    sub->field_84 = -1;
    GhostRecordReset((GhostSlot*)&sub->field_10);
    sub->field_E4 = lbl_804D775C;
    ZoneInit(sub->field_1E8, (Actor*)owner, (IVec3*)((char*)owner + 0x24), sub->field_E8,
             0x10, sub->field_E4);
    ZoneObjectSetSlot14((ZoneObject*)sub->field_1E8, 2);
    ZoneSetCallback(sub->field_1E8, 0, 0, 0, (Actor*)owner);
    ZoneObjectSetSlot18((ZoneObject*)sub->field_1E8, sub->field_E4);
    fn_800CD5D8((char*)owner + 0x5B0, 1);
    return 1;
}

// Runs the state block's optional 0xD8 callback. Returns 0 when there is no
// state block or no callback installed.
// Called by other units as fn_80117FB4, so the placeholder name and C linkage
// are kept.
extern "C" int fn_80117FB4(void* ownerPtr)
{
    U80116B28Owner* owner = (U80116B28Owner*)ownerPtr;
    int (*callback)(void*);

    if (owner->field_5EC == 0)
        return 0;
    callback = (int (*)(void*))owner->field_5EC->field_D8;
    if (callback == 0)
        return 0;
    return callback(owner);
}

// Looks the state block's registered type-table id up in the global per-type
// table and calls that entry's 0x10 callback with `active` set, forwarding the
// caller's extra arguments. Returns 0 when the id is not registered.
// Called by other units as fn_80117FF8, so the placeholder name and C linkage
// are kept.
extern "C" int fn_80117FF8(U80116B28Sub* sub, int arg1, int arg2)
{
    int index = -1;
    int i;

    for (i = 0; i < lbl_804D2E08; i++) {
        if (sub->field_CC == lbl_8039ED68[i].id) {
            index = i;
            break;
        }
    }
    if (index == -1)
        return 0;
    return lbl_8039ED68[index].dispatch(sub, arg1, arg2, 1);
}

// Same per-type dispatch as fn_80117FF8, but calls the entry's 0x10 callback
// with `active` clear.
// Called by other units as fn_80118084, so the placeholder name and C linkage
// are kept.
extern "C" int fn_80118084(U80116B28Sub* sub, int arg1, int arg2)
{
    int index = -1;
    int i;

    for (i = 0; i < lbl_804D2E08; i++) {
        if (sub->field_CC == lbl_8039ED68[i].id) {
            index = i;
            break;
        }
    }
    if (index == -1)
        return 0;
    return lbl_8039ED68[index].dispatch(sub, arg1, arg2, 0);
}

// Returns the name registered for the state block's type id in the global
// per-type table (entry field 0x18), or the empty-name default string when
// there is no state block or the id is not registered.
const char* U80116B28GetTypeName(U80116B28Owner* owner)
{
    int count;
    int i;

    if (owner->field_5EC == 0)
        return &lbl_803457B0[0x47];
    count = lbl_804D2E08;
    for (i = 0; i < count; i++) {
        if (owner->field_5EC->field_CC == lbl_8039ED68[i].id)
            return lbl_8039ED68[i].name;
    }
    return &lbl_803457B0[0x47];
}

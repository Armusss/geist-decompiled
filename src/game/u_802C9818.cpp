// u_802C9818.cpp: NPC state functions that follow a target, plus their message handling
// (0x802C9818-0x802CBB44).  The actor type this unit drives is not identified yet.
// Names are descriptive guesses; struct layouts are partial and derived from the retail asm.

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef float f32;

struct Beh;
struct Actor;

// Per-actor state control block (actor + 0x130), 0x88 bytes.
struct StateBlock {
    /*0x00*/ int state;
    /*0x04*/ f32 timer;
    /*0x08*/ s16 field08;
    /*0x0A*/ s16 field0A;
    /*0x0C*/ f32 a[8];
    /*0x2C*/ f32 b[8];
    /*0x4C*/ f32 c[8];
    /*0x6C*/ f32 hitTimer;
    /*0x70*/ u8 unk70[0x74 - 0x70];
    /*0x74*/ int field74;
    /*0x78*/ f32 field78;
    /*0x7C*/ int field7C;
    /*0x80*/ int field80;
    /*0x84*/ int field84;
};

// Partial layout of the actor class (only the offsets used here).
struct Actor {
    /*0x000*/ u8 unk000[0x84];
    /*0x084*/ f32 pos[3];
    /*0x090*/ u8 unk090[0xCC - 0x90];
    /*0x0CC*/ int model;
    /*0x0D0*/ u8 unk0D0[0xFC - 0xD0];
    /*0x0FC*/ void* field0FC;
    /*0x100*/ u8 unk100[0x104 - 0x100];
    /*0x104*/ void* field104;
    /*0x108*/ u8 unk108[0x130 - 0x108];
    /*0x130*/ StateBlock* block;
    /*0x134*/ u8 unk134[0x138 - 0x134];
    /*0x138*/ f32 dt;
    /*0x13C*/ u8 unk13C[0x270 - 0x13C];
    /*0x270*/ u8 conditions[0x18];
    /*0x288*/ int field288;
    /*0x28C*/ u8 unk28C[0x604 - 0x28C];
    /*0x604*/ void* field604;
    /*0x608*/ int targetId;
    /*0x60C*/ u8 unk60C[0x894 - 0x60C];
    /*0x894*/ u8 beh[0x91C - 0x894];
    /*0x91C*/ f32 field91C;
    /*0x920*/ u8 unk920[0x994 - 0x920];
};

// Tuning values (.sdata).
static f32 sFollowScale = 0.85f;
static f32 sTimerMin = 3000.0f;
static f32 sTimerMax = 5000.0f;

// Reply to message 0x400 (sent to the followed actor and to the target).
struct Reply400 {
    /*0x00*/ Actor* actor;
    /*0x04*/ u8 unk04[0x0C - 0x04];
    /*0x0C*/ int hit;
    /*0x10*/ u8 unk10[0x20 - 0x10];
};

// Name hashes of the model nodes this actor animates (8 each), and the asset ids found for them.
static const u32 sHashA[8] = {0x9CA18DD0, 0x9C9EC4D7, 0xF74B24F1, 0xEF40825B, 0xEF408315, 0x014BE569, 0x014BE623, 0x9FAFABBD};
static const u32 sHashB[8] = {0x3D33E0B9, 0xF65F386F, 0x5C7672B5, 0x05E3C5C2, 0xBF9C6C7C, 0xC2A29D50, 0x7C5B440A, 0x4494EE74};
static const u32 sHashC[8] = {0xD20F7090, 0xD20CA797, 0x6F999A31, 0x80C0B51B, 0x80C0B5D5, 0x92CC1829, 0x92CC18E3, 0x3E35D0FD};
static int sIdA[8];
static int sIdB[8];
static int sIdC[8];
static int sIdD[8];

extern "C" {
int rand();

int fn_80162940(void* beh, int id);
int fn_800B26B8(Actor* a);
int fn_800BAF40(Actor* a);
void fn_800B261C(Actor* a, int arg, void* out);
int fn_8029B3B0(Actor* a, u32 hash);
void fn_8029B070(Actor* a, u32 hash);
extern int lbl_804D3B3C;
extern int lbl_804D3B40;
extern int lbl_804D3B44;
extern int lbl_804D3B48;
extern int lbl_804D3B4C;
extern int lbl_804D3B54;
extern f32 lbl_804DAF00;
extern f32 lbl_804DAF10;
extern f32 lbl_804DAF38;
int fn_802C9D1C(void* owner);
void fn_800CE328();
void fn_800B2D00();
void VFree(void* p);
void fn_800BBCA8(Actor* actor);
void fn_800BC56C(Actor* actor, void* target);
void fn_80162610(Actor* actor, void* beh, int arg);
}
int MemFlagIsSet(void* flags, int id);
int SendMsg(int to, int msg, void* data);
float ActorAimAngleToPos(void* actor, f32* pos);
struct Owner;
float OwnerValue(Owner* owner, void* actor);
int fn_80037EC0(int model, int node, int a, int r, int g, int b, int al);
void BehReset(Beh* beh);
void* VAlloc(int size);
int AssetFind(u32 type, u32 name);
int ModelNodeSetup(int model, int node, int a, int b);
int FxFind(const char* name);
void ConditionUpdateBit(void* conditions, int id, int value);
void* FollowGetTargetObj(void* conditions);
int HasCondition(void* conditions, int id);
void RangeResetPending(void* conditions);
void FollowSetTargetObj(void* conditions, void* target);

// 0x802CA508: release the state block
void StateBlockFree(Actor* a) {
    VFree(a->block);
    a->block = 0;
    a->field104 = 0;
}

// 0x802CA9B8: stop following and reset the behaviour
int FollowStateStop(Actor* a) {
    BehReset((Beh*)a->beh);
    fn_80162610(a, a->beh, 0);
    ConditionUpdateBit(a->conditions, 4, 0);
    ConditionUpdateBit(a->conditions, 0x16, 0);
    return 1;
}

// 0x802CAA1C: enter state 2
int FollowStateEnter2(Actor* a) {
    a->block->state = 2;
    a->field288 = 0;
    ConditionUpdateBit(a->conditions, 4, 1);
    ConditionUpdateBit(a->conditions, 0x16, 1);
    fn_800BBCA8(a);
    return 1;
}

// 0x802CAC38: leave following
int FollowStateLeave(Actor* a) {
    a->field288 = 0;
    fn_800BBCA8(a);
    ConditionUpdateBit(a->conditions, 4, 0);
    ConditionUpdateBit(a->conditions, 0x16, 0);
    return 1;
}

// 0x802CAC90: enter state 1 and pick up the current follow target
int FollowStateEnter1(void* owner) {
    Actor* a = (Actor*)owner;
    void* target;
    a->block->state = 1;
    a->field288 = 0;
    target = FollowGetTargetObj(a->conditions);
    if (target != 0) {
        FollowSetTargetObj(a->conditions, target);
        fn_800BC56C(a, target);
        a->field288 = 3;
        ConditionUpdateBit(a->conditions, 4, 1);
        ConditionUpdateBit(a->conditions, 0x16, 1);
    }
    return 1;
}

// 0x802CAFB8
int FollowStateNone(Actor* a) {
    return 1;
}

// 0x802CAFC0: state 0
int FollowStateEnter0(Actor* a) {
    a->block->state = 0;
    a->field288 = 0;
    fn_800BBCA8(a);
    return 1;
}

// 0x802CA544: allocate and initialise the state block, look up the model nodes and effects
void FollowInit(Actor* a) {
    StateBlock* b;
    int i;
    a->field104 = (void*)fn_802C9D1C;
    b = (StateBlock*)VAlloc(0x88);
    if (b == 0) {
        a->block = 0;
    } else {
        f32 other;
        f32 zero;
        a->block = b;
        a->field0FC = (void*)fn_800CE328;
        a->field604 = (void*)fn_800B2D00;
        a->field91C = lbl_804DAF10;
        zero = lbl_804DAF00;
        other = lbl_804DAF38;
        b->state = 0;
        b->field08 = 0;
        b->field0A = 0;
        b->field74 = -1;
        b->field78 = zero;
        b->field7C = -1;
        b->field80 = -1;
        b->field84 = -1;
        for (i = 0; i < 8; i++) {
            b->a[i] = zero;
            b->b[i] = other;
            b->c[i] = zero;
            sIdD[i] = AssetFind(0xD9044CBB, sHashC[i]);
            sIdA[i] = AssetFind(0xD9044CBB, sHashA[i]);
            sIdB[i] = AssetFind(0xF1754BF2, sHashA[i]);
            sIdC[i] = AssetFind(0xD9044CBB, sHashB[i]);
            ModelNodeSetup(a->model, sIdD[i], 0, 1);
            fn_80037EC0(a->model, sIdD[i], 1, 0x32, 0x32, 0xFF, 0xFF);
            fn_80037EC0(a->model, sIdC[i], 1, 0xFF, 0xFF, 0xFF, 0x32);
        }
        lbl_804D3B3C = AssetFind(0xF2E2BEB8, 0xDE1813E0);
        lbl_804D3B40 = AssetFind(0xF2E2BEB8, 0x0A8B74B2);
        lbl_804D3B44 = AssetFind(0xF2E2BEB8, 0x0A36E7AC);
        lbl_804D3B48 = AssetFind(0xF2E2BEB8, 0x0A28D02B);
        lbl_804D3B4C = AssetFind(0xF2E2BEB8, 0x4A9BE9A9);
        lbl_804D3B54 = FxFind("stat_hit");
    }
}

// 0x802CA79C: update while in state 2 (waiting for the target to come within range)
int FollowState2Update(void* owner) {
    Actor* a = (Actor*)owner;
    StateBlock* b = a->block;
    void* target = FollowGetTargetObj(a->conditions);
    Reply400 reply;
    if (MemFlagIsSet(a->conditions, 0x24) != 0)
        return 0;
    if (target == 0 || SendMsg(*(int*)((u8*)target + 0x38), 0x400, &reply) == 0 || reply.hit == 0)
        return 0;
    if (fn_80162940(a->beh, 1) != 0 && fn_80162940(a->beh, 4) != 0 && b->timer <= 0.0f) {
        fn_80162610(a, a->beh, 1);
        b->timer = (sTimerMax - sTimerMin) * ((f32)rand() / 32767.0f) + sTimerMin;
    }
    b->timer -= a->dt;
    if (SendMsg(a->targetId, 0x400, &reply) == 0 || reply.hit == 0)
        return 1;
    {
        Actor* other = reply.actor;
        f32 d1 = ActorAimAngleToPos(target, a->pos);
        f32 d2 = ActorAimAngleToPos(target, other->pos);
        f32 s = sFollowScale * OwnerValue((Owner*)other, target);
        f32 t = OwnerValue((Owner*)a, target);
        if (s <= t || 40.0f <= __fabsf(d1)) {
            if (__fabsf(d2) <= 50.0f)
                return b->state;
            return 1;
        }
        return 1;
    }
}

// 0x802CAA84: update while in state 1 (following the target)
int FollowState1Update(void* owner) {
    Actor* a = (Actor*)owner;
    StateBlock* b = a->block;
    void* target;
    Reply400 reply;
    fn_800B26B8(a);
    target = FollowGetTargetObj(a->conditions);
    if (target == 0) {
        ConditionUpdateBit(a->conditions, 4, 0);
        return 0;
    }
    if (MemFlagIsSet(a->conditions, 0x24) != 0) {
        ConditionUpdateBit(a->conditions, 4, 0);
        return 0;
    }
    if (SendMsg(a->targetId, 0x400, &reply) != 0 && reply.hit != 0) {
        Actor* other = reply.actor;
        f32 d1 = ActorAimAngleToPos(target, a->pos);
        f32 d2 = ActorAimAngleToPos(target, other->pos);
        f32 s = OwnerValue((Owner*)other, target);
        f32 t = OwnerValue((Owner*)a, target);
        if (s < sFollowScale * t && __fabsf(d2) < 40.0f)
            return 2;
        if (50.0f < __fabsf(d1))
            return 2;
    }
    if (fn_800BAF40(a) == -1)
        fn_800BC56C(a, target);
    if (fn_800BAF40(a) == -3)
        a->field288 = 3;
    else
        a->field288 = 0;
    return b->state;
}

// 0x802CAD28: update while in state 0 (idle, looking for a target)
int FollowState0Update(void* owner) {
    Actor* a = (Actor*)owner;
    StateBlock* b = a->block;
    Actor* target = 0;
    Reply400 reply;
    int i;
    if (MemFlagIsSet(a->conditions, 0x24) != 0) {
        b->hitTimer -= a->dt;
        if (b->hitTimer <= 0.0f) {
            ConditionUpdateBit(a->conditions, 0x24, 0);
            for (i = 0; i < 8; i++) {
                b->a[i] = 0.0f;
                b->b[i] = 999999.0f;
                ModelNodeSetup(a->model, sIdD[i], 0, 1);
                ModelNodeSetup(a->model, sIdA[i], 1, 1);
                fn_80037EC0(a->model, sIdA[i], 0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_80037EC0(a->model, sIdD[i], 1, 0x32, 0x32, 0xFF, 0xFF);
            }
        }
    }
    if (HasCondition(a->conditions, 0x3B) == 0) {
        int k = fn_800B26B8(a);
        if (k == 4) {
            target = (Actor*)FollowGetTargetObj(a->conditions);
            if (target != 0) {
                SendMsg(*(int*)((u8*)target + 0x38), 0x400, &reply);
                if ((*(u32*)((u8*)&reply + 0x18) & 2) == 0)
                    RangeResetPending(a->conditions);
            }
        } else {
            fn_800B261C(a, k, &target);
            if (target != 0) {
                SendMsg(*(int*)((u8*)target + 0x38), 0x400, &reply);
                if ((*(u32*)((u8*)&reply + 0x18) & 2) != 0)
                    FollowSetTargetObj(a->conditions, target);
            }
        }
        target = (Actor*)FollowGetTargetObj(a->conditions);
        if (target != 0) {
            Reply400 r2;
            if (SendMsg(a->targetId, 0x400, &r2) == 0 || r2.hit == 0)
                r2.actor = 0;
            if (r2.actor != 0) {
                f32 s = OwnerValue((Owner*)r2.actor, target);
                f32 t = OwnerValue((Owner*)a, target);
                if (s <= t)
                    return 2;
            }
            if (fn_8029B3B0(a, 0x31992E8D) != 0) {
                fn_8029B070(a, 0x31992E8D);
                return 1;
            }
        }
    }
    return b->state;
}

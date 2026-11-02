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

// Per-actor state control block (actor + 0x130).
struct StateBlock {
    /*0x00*/ int state;
    /*0x04*/ u8 unk04[0x10 - 0x04];
};

// Partial layout of the actor class (only the offsets used here).
struct Actor {
    /*0x000*/ u8 unk000[0x104];
    /*0x104*/ int field104;
    /*0x108*/ u8 unk108[0x130 - 0x108];
    /*0x130*/ StateBlock* block;
    /*0x134*/ u8 unk134[0x270 - 0x134];
    /*0x270*/ u8 conditions[0x18];
    /*0x288*/ int field288;
    /*0x28C*/ u8 unk28C[0x894 - 0x28C];
    /*0x894*/ u8 beh[0x900 - 0x894];
};

extern "C" {
void VFree(void* p);
void fn_800BBCA8(Actor* actor);
void fn_800BC56C(Actor* actor, void* target);
void fn_80162610(Actor* actor, void* beh, int arg);
}
void BehReset(Beh* beh);
void ConditionUpdateBit(void* conditions, int id, int value);
void* FollowGetTargetObj(void* conditions);
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
int FollowStateEnter1(Actor* a) {
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

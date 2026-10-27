// u_801A6094.cpp: AI state machine states and messages for one actor type (0x801A6094-0x801AADA8).
// Names are descriptive guesses; the code is matched against the retail DOL.

// u_801A6094.cpp: actor behaviour state-machine message handler and per-frame tick.
// Names are descriptive guesses.

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long ulong;

struct Vec3 {
    float x;            // 0x0
    float y;            // 0x4
    float z;            // 0x8
};

// 3x3 rotation matrix used by the placement helpers.
struct Mtx34 {
    float m[9];         // 0x00
};

// Rotation plus translation; the layout matches Mtx34 followed by the position
// vector at 0x24.
struct PlaceF {
    Mtx34 m;            // 0x00
    Vec3 pos;           // 0x24
};

// Placement record hanging off the actor at 0xf0.
struct Placement {
    u8 unk00[0x10];     // 0x00
    float x;            // 0x10
    float y;            // 0x14
    float z;            // 0x18
};

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    void* mData;        // 0x04
    u32 mSize;          // 0x08
    u32 mCapacity;      // 0x0c
    u32 mUnk10;         // 0x10
};

class String : public Buffer {
public:
    String();
    String(const char* text);
};

struct ConditionSet {
    u8 bits[0xc];       // 0x0
};

struct Actor;

struct AiStateMachine {
    void* owner;        // 0x00
    int unk04;          // 0x04
    int state;          // 0x08
    int unk0C;          // 0x0c
    int unk10;          // 0x10
    int unk14;          // 0x14
    int unk18;          // 0x18
    float unk1C;        // 0x1c
    float unk20;        // 0x20
    int value24;        // 0x24
    int unk28;          // 0x28
    int unk2C;          // 0x2c
    u8 unk30;           // 0x30
};

typedef void (*StateUpdateFn)(Actor*);
typedef int (*StateChangeFn)(Actor*, int);
typedef int (*StateQueryFn)(Actor*);

struct StateEntry {
    u32 unk00;                  // 0x00
    StateUpdateFn update;       // 0x04
    StateChangeFn change;       // 0x08
    StateQueryFn query;         // 0x0c
    void* unk10;                // 0x10
    const char* name;           // 0x14
};

// Named-entry table used to resolve sound effect names; the hash is recomputed
// lazily from the name when the table is set up.
struct SplineNameEntry {
    const char* name;   // 0x00
    u32 unk04;          // 0x04
    u32 hash;           // 0x08
    u32 value;          // 0x0c
};

// Sub-object hanging off the actor at 0xc4; only the two words passed to the
// effect lookup are known.
struct ActorContext {
    u8 unk00[0x8];              // 0x00
    float unk08;                // 0x08
    u8 unk0C[0x14 - 0x0C];      // 0x0c
    int value14;                // 0x14
    u8 unk18[0x1C - 0x18];      // 0x18
    int value1C;                // 0x1c
    u8 unk20[0xC0 - 0x20];      // 0x20
    float unkC0;                // 0xc0
};

struct Actor {
    Mtx34 matrix;                   // 0x000
    int cellX;                      // 0x024 (actor cell position)
    int cellY;                      // 0x028
    int cellZ;                      // 0x02c
    u8 unk030[0x38 - 0x30];         // 0x030
    int anim;                       // 0x038
    u8 unk03C[0x84 - 0x03C];        // 0x03c
    Vec3 pos;                       // 0x084 (actor float position)
    u8 unk090[0xC4 - 0x090];        // 0x090
    ActorContext* context;          // 0x0c4
    u8 unk0C8[0xCC - 0xC8];         // 0x0c8
    int animHandle;                 // 0x0cc (model animation state)
    int unk0D0;                     // 0x0d0
    void* zone;                     // 0x0d4
    u8 unk0D8[0xF0 - 0x0D8];        // 0x0d8
    Placement* placement;           // 0x0f0
    u8 unk0F4[0xFC - 0x0F4];        // 0x0f4
    int unk0FC;                     // 0x0fc
    u8 unk100[0x110 - 0x100];       // 0x100
    int (*update)(Actor*);          // 0x110 per-frame update callback
    u8 unk114[0x118 - 0x114];       // 0x114
    int (*unk118)(Actor*);          // 0x118
    u8 unk11C[0x134 - 0x11C];       // 0x11c
    AiStateMachine* ai;             // 0x134
    float unk138;                   // 0x138
    u8 unk13C[0x240 - 0x13C];       // 0x13c
    Vec3 unk240;                    // 0x240
    Vec3 euler;                     // 0x24c
    u8 unk258[0x26C - 0x258];       // 0x258
    int unk26C;                     // 0x26c
    ConditionSet conditions;        // 0x270
    float unk27C;                   // 0x27c
    float unk280;                   // 0x280
    float unk284;                   // 0x284
    int unk288;                     // 0x288
    u8 unk28C[0x5F0 - 0x28C];       // 0x28c
    void* unk5F0;                   // 0x5f0
    void* unk5F4;                   // 0x5f4
    u8 unk5F8[0x918 - 0x5F8];       // 0x5f8
    float unk918;                   // 0x918
};

// Payload of the behaviour messages; the same storage is reused by several
// message types, so the fields at 0x0c and up are type-dependent.
struct ActorMsgData {
    Actor* actor;       // 0x00
    int unk04;          // 0x04
    int unk08;          // 0x08
    u32 unk0C;          // 0x0c (a String destination for message 0x41e)
    u32 unk10;          // 0x10
    u32 unk14;          // 0x14
    u32 unk18;          // 0x18
    u32 unk1C;          // 0x1c
    u32 unk20;          // 0x20
    u32 unk24;          // 0x24
    u32 unk28;          // 0x28
    u32 unk2C;          // 0x2c
    u32 posX;           // 0x30
    u32 posY;           // 0x34
    u32 posZ;           // 0x38
};

extern StateEntry lbl_803AAAD8[];
extern SplineNameEntry lbl_803AAAB8[2];
extern u8 __upper_map[];

extern u32 lbl_804D30A0;    // sound effect handles resolved at behaviour setup
extern u32 lbl_804D30A4;
extern u32 lbl_804D30A8;
extern u32 lbl_804D30AC;
extern u32 lbl_804D30B0;
extern u32 lbl_804D30B4;

void VFree(void* ptr);
void* VAlloc(int size);
void SplineClearNameHashes(SplineNameEntry* entries, int count);
int FxFind(const char* name);
void fn_800B5004(Vec3* out, Vec3* in);
void MemCopyVec3(float* dest, float* src);
void fn_800BBCA8(Actor* actor);
void ExtraTrailAttach(void* holder);
void fn_801D55CC(void);
void fn_800A7B08(void* cond, int a, int b);
void fn_800A7A54(void* cond, int a, int b);

void ModelSetMatrix(int anim, Mtx34* mtx, char* placement);
int ModelAnimate(int anim, char* placement, int flag);
void AnimApply(int zone, char* placement, int anim, int unk);
void BoneGetMatrix(Mtx34* out, void* placement);
void MtxGetEuler(Vec3* out, Mtx34* mtx);
void PlacementToMatrix(Mtx34* out, PlaceF* place);
void ActorGetMatrix(Mtx34* out, Actor* actor);
void MtxSetEuler(Mtx34* mtx, Vec3* euler);
void EulerNormalize(Vec3* out, Vec3* in);
void Vec3Set(Vec3* out, float x, float y, float z);
void DogActorUpdateMove(Actor* actor);

extern float lbl_804D87D8;
extern float lbl_804D87F4;
extern float lbl_804D87F8;
extern float lbl_804D87FC;
extern float lbl_804D8800;
extern float lbl_804D8804;
extern float lbl_804D8808;

extern "C" {
void* memset(void* dest, int value, unsigned long size);
int fn_800A1BC8(int a, int b, u32 c, int d, int e);
int fn_800CDD10(Actor* actor);
int fn_800CBAA0(void* data, int a, int b);
void fn_80090390(void* obj, int idx);
void fn_800225C8(String* out, String* a, String* b);
void fn_8028C188(void* dest, void* data, unsigned long size);
int fn_8008E964(void* zone);
int fn_8008EA9C(void* zone, int idx);
void fn_800913EC(void* zone, float value, Vec3* out);
float fn_800CC2D0(Actor* actor, int a, int b, int c);
void fn_800A0168(Mtx34* a, Mtx34* b);
void fn_800A0030(Vec3* out, Mtx34* mtx, Vec3* in);
void fn_800CAD0C(Actor* actor, int handle);
int fn_800CADE0(Actor* actor);
int fn_80212730(int anim);
int fn_8016325C(void* obj);
void fn_8008F4E8(void* zone, int arg);
void fn_8008F378(void* zone);
void fn_8008F4EC(void* zone, int arg);
void fn_8008F230(float value, void* zone, int arg, int flag);
void fn_80090974(float value, void* zone, int arg, int handle, int flag,
                  void* fn, Actor* actor);
void fn_8016EEF4(int handle);
void fn_8008DE4C(void* zone, int a, int b, int c);
int fn_8016F120(u32 handle, void* target);
}

void ConditionSetBit(void* cond, int bit, int value);
void ConditionUpdateBit(void* cond, int bit, int value);
int HasCondition(void* cond, int bit);
void ActorPlayAnim(int anim, unsigned long flags, int arg);
const char* OLTFindValue(unsigned long hash, int id);
u8 ZoneClassifyDirection(void* obj, Vec3* vec);
int MemFlagIsSet(void* cond, int bit);
int AnimBankGetSetCount(unsigned short a, unsigned short b, unsigned short c);

int GeistA28A4StateMessage(int msg, int* data) {
    ActorMsgData* m = (ActorMsgData*)data;

    switch (msg) {
    case 0x41e: {
        Actor* actor = m->actor;
        AiStateMachine* ai = actor->ai;
        if (m->unk04 == -0x527a8906) {
            {
                String stateName(lbl_803AAAD8[ai->state].name);
                String suffix("\n");
                String text;
                fn_800225C8(&text, &stateName, &suffix);
                fn_8028C188(&m->unk0C, text.mData, text.mSize);
            }
            {
                const char* value = OLTFindValue(0x2fa2416fUL, ai->unk04);
                String valueString(value);
                String suffix("\n");
                String text;
                fn_800225C8(&text, &valueString, &suffix);
                fn_8028C188(&m->unk0C, text.mData, text.mSize);
            }
        }
        break;
    }
    case 0x0000:
        return fn_800CBAA0(data, 0, 0);
    case 0x000a:
        return fn_800CBAA0(data, 0, 1);
    case 0x7300:
        return fn_800CBAA0(data, 1, 0);
    case 0x407: {
        Actor* actor = m->actor;
        AiStateMachine* ai = actor->ai;
        if (m->unk14 != 0x672f35c2) {
            Vec3 pos;
            pos.x = -(float)m->posX;
            pos.y = -(float)m->posY;
            pos.z = -(float)m->posZ;
            ai->unk30 = ZoneClassifyDirection(actor, &pos);
            return 0;
        }
        fn_80090390(actor->zone, 0);
        fn_80090390(actor->zone, 1);
        ConditionSetBit(&actor->conditions, 0x0d, 0);
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        if (ai->state != 7) {
            lbl_803AAAD8[ai->state].change(actor, 7);
            lbl_803AAAD8[7].update(actor);
        }
        ConditionUpdateBit(&actor->conditions, 0x0b, 0);
        return 1;
    }
    case 0xc14: {
        int type = m->unk0C;
        Actor* actor = m->actor;
        if (type == 0x672f35c2) {
            lbl_803AAAD8[actor->ai->state].change(actor, 0x15);
            lbl_803AAAD8[0x15].update(actor);
        } else if (type == 0x39cb0cb0) {
            lbl_803AAAD8[actor->ai->state].change(actor, 7);
            lbl_803AAAD8[7].update(actor);
        }
        break;
    }
    }
    return 0;
}

int GeistA28A4StateUpdate(Actor* actor) {
    int nextState = -1;
    AiStateMachine* ai = actor->ai;

    if (ai->unk1C > 0.0f)
        ai->unk1C -= actor->unk138;
    if (ai->unk20 > 0.0f)
        ai->unk20 -= actor->unk138;

    if (HasCondition(&actor->conditions, 0x1c) != 0) {
        if (fn_8008E964(actor->zone) != 0) {
            ConditionSetBit(&actor->conditions, 0x1c, 0);
            ConditionSetBit(&actor->conditions, 0x1d, 1);
            if (HasCondition(&actor->conditions, 0x0d) == 0 &&
                HasCondition(&actor->conditions, 9) == 0 &&
                HasCondition(&actor->conditions, 0x0a) == 0) {
                lbl_803AAAD8[ai->state].update(actor);
            }
        }
    } else if (fn_8008EA9C(actor->zone, 0) == 0) {
        int state = ai->state;
        if (state != 0x18 && state != 0x17 && state != 0x13 && state != 0x12) {
            lbl_803AAAD8[state].update(actor);
        }
    }

    if (HasCondition(&actor->conditions, 9) != 0 &&
        ai->state != 0x17 && ai->state != 0x18) {
        ConditionSetBit(&actor->conditions, 0x0a, 1);
        ActorPlayAnim(actor->anim, 0xdc04e3d5UL, actor->anim);
        nextState = 0x17;
    }

    if (nextState < 0 || nextState == ai->state) {
        nextState = lbl_803AAAD8[ai->state].query(actor);
    }

    if (nextState >= 0 && ai->state != nextState &&
        lbl_803AAAD8[ai->state].change(actor, nextState) != 0) {
        lbl_803AAAD8[nextState].update(actor);
    }

    return 1;
}

// Frees the actor's AI state block and clears the per-frame update callback and
// the AI pointer. Called when the behaviour is torn down.
void DogActorReleaseAi(Actor* actor) {
    if (actor != 0) {
        VFree(actor->ai);
        actor->update = 0;
        actor->ai = 0;
    }
}

// Recomputes a name hash: h = h * 31 + c through the game's upper-case map,
// with 0xff mapping to -1, followed by a 32-bit modulo.
static inline u32 SplineNameHash(const char* name) {
    u32 hash = 0;
    char c = *name;
    while (c != 0) {
        u32 value;
        if (c == -1)
            value = 0xFFFFFFFF;
        else
            value = __upper_map[(u8)c];
        hash = hash * 31 + value;
        name++;
        c = *name;
    }
    return hash % 0xFFFFFFFF;
}

// Allocates the actor's AI state block, installs the per-frame update callbacks
// and resolves the dog's named sound effects.
void DogActorInitBehaviour(void* owner, Actor* actor) {
    if (actor == 0)
        return;

    SplineClearNameHashes(lbl_803AAAB8, 2);
    if (lbl_803AAAB8[0].name == 0)
        lbl_803AAAB8[0].hash = 0;
    else
        lbl_803AAAB8[0].hash = SplineNameHash(lbl_803AAAB8[0].name);
    if (lbl_803AAAB8[1].name == 0)
        lbl_803AAAB8[1].hash = 0;
    else
        lbl_803AAAB8[1].hash = SplineNameHash(lbl_803AAAB8[1].name);

    {
        ActorContext* ctx = actor->context;
        lbl_803AAAB8[0].value = fn_800A1BC8(ctx->value14, ctx->value1C,
                                            (u32)lbl_803AAAD8[8].unk00, 0, 0);
        lbl_803AAAB8[1].value = fn_800A1BC8(ctx->value14, ctx->value1C,
                                            (u32)lbl_803AAAD8[8].unk00, 1, 0);
    }

    {
        AiStateMachine* ai = (AiStateMachine*)VAlloc(0x34);
        if (ai == 0) {
            actor->ai = 0;
        } else {
            actor->ai = ai;
            actor->update = GeistA28A4StateUpdate;
            actor->unk118 = fn_800CDD10;
            memset(ai, 0, 0x34);
            ai->owner = owner;
            ConditionSetBit(&actor->conditions, 0x10, 0);
            ConditionSetBit(&actor->conditions, 0x2d, 1);
            lbl_804D30A0 = FxFind("stand_barkhowl");
            lbl_804D30A4 = FxFind("stand_bark");
            lbl_804D30A8 = FxFind("pain_back");
            lbl_804D30AC = FxFind("death1");
            lbl_804D30B0 = FxFind("death2");
            lbl_804D30B4 = FxFind("idle");
            ai->state = 0;
            ai->unk14 = 0;
        }
    }
}

// Payload of the "target lost" message; the leading word is set by the caller
// of SendMsg, not here.
struct TargetLostMsg {
    u32 unk00;          // 0x00
    int unk04;          // 0x04
    int unk08;          // 0x08
    u32 unk0C;          // 0x0c
};

extern "C" {
void SendMsg(int to, int msg, void* data);
void fn_80162610(void* actor, void* data);
void fn_800CB8BC(Actor* actor, ActorContext* ctx, int a, int b);
}

void* FollowGetTargetObj(void* cond);
void RangeResetPending(void* cond);

// Behaviour message handler: reacts to the state, target-lost and target
// notification messages and dispatches the active state's hooks. Always
// returns 1 (the message was consumed).
int GeistA28A4Message(int msg, Actor* actor) {
    AiStateMachine* ai = actor->ai;

    switch (msg) {
    case 0x4a5c9fc:
    case 0x46341c3a:
    case 0x239807:
        fn_800CB8BC(actor, actor->context, ai->value24, actor->unk26C);
        break;
    case 0x1b83833f:
        fn_80162610(actor, (u8*)actor + 0x894);
        lbl_803AAAD8[ai->state].change(actor, ai->unk14);
        lbl_803AAAD8[ai->unk14].update(actor);
        break;
    case 0x10b38: {
        void* target = FollowGetTargetObj(&actor->conditions);
        if (target != 0) {
            int id = *(int*)((u8*)target + 0x38);
            if (id != -1) {
                TargetLostMsg data;
                data.unk04 = 0;
                data.unk08 = actor->anim;
                data.unk0C = 0x879f2bda;
                SendMsg(id, 0xc14, &data);
            }
            RangeResetPending(&actor->conditions);
        }
        break;
    }
    }
    return 1;
}

// Clears the two follow-range conditions and stores the caller's value as the
// pending AI state target.
int GeistA28A4ClearFollowCondition(Actor* actor, int value) {
    AiStateMachine* ai = actor->ai;
    ConditionSetBit(&actor->conditions, 0, 0);
    ConditionSetBit(&actor->conditions, 4, 0);
    ai->unk14 = value;
    return 0;
}

extern "C" int rand(void);
extern double lbl_804D87D0;     // 2^52 bias for the unsigned int -> float idiom
extern float lbl_804D87DC;      // random range scale
extern float lbl_804D87E0;      // delay base
extern float lbl_804D87E4;      // delay scale
extern float lbl_804D87D8;      // AI timer threshold
extern float lbl_804D87E8;      // effect fade-in delay
extern float lbl_804D87EC;      // effect delay
extern float lbl_804D87F0;      // AI timer reset value

// Behaviour "set target" callback: records the caller's value as the pending AI
// target and arms a randomised delay before the next state change.
int DogActorSetTargetDelay(Actor* actor, int value) {
    AiStateMachine* ai = actor->ai;
    u32 roll = rand();
    ai->unk14 = value;
    ai->unk20 = lbl_804D87E4 * ((float)roll / lbl_804D87DC) + lbl_804D87E0;
    return 1;
}

// Behaviour "cancel follow" callback: clears the follow conditions, stores the
// pending AI target and reports whether the caller asked for state 0x18.
int DogActorCancelFollow(Actor* actor, int value) {
    AiStateMachine* ai = actor->ai;
    ai->unk14 = value;
    ConditionSetBit(&actor->conditions, 0, 0);
    ConditionSetBit(&actor->conditions, 4, 0);
    if (ai->state == 0x12)
        ConditionUpdateBit(&actor->conditions, 0xb, 0);
    return value == 0x18;
}

// State-change callback: clears the entry flag for the old state and records the
// newly requested state in the AI state machine.
int DogActorSetAiState(Actor* actor, int state) {
    AiStateMachine* ai = actor->ai;

    ConditionSetBit(&actor->conditions, 0x10, 0);
    if (ai->state == 0x0d) {
        ConditionSetBit(&actor->conditions, 0x29, 0);
    }
    if (ai->state == 0x14 && state != 0x14) {
        ConditionSetBit(&actor->conditions, 0x0d, 0);
    }
    ai->unk14 = state;
    return 1;
}

// Returns the AI state machine's current state, or 0 when there is no actor.
int DogActorGetAiState(Actor* actor) {
    if (actor != 0) {
        return actor->ai->state;
    }
    return 0;
}

// Per-frame AI tick: refreshes the AI state machine when the actor has no
// pending target or when the animation query reports the clip is done, then
// returns the current AI state.
int DogActorTickAiAndGetState(Actor* actor) {
    AiStateMachine* ai = actor->ai;
    if (actor->unk5F0 == 0 || fn_80212730(actor->anim) != -1) {
        DogActorUpdateMove(actor);
    }
    return ai->state;
}

// Picks the AI sub-state for this frame: handles the queued hit/death reactions
// first, then follows the zone direction and the behaviour condition flags.
int DogActorChooseAiSubState(Actor* actor) {
    AiStateMachine* ai = actor->ai;
    DogActorUpdateMove(actor);

    if (HasCondition(&actor->conditions, 0x0d) != 0)
        return 0x14;
    if (HasCondition(&actor->conditions, 0x3c) != 0)
        return 0x0a;
    if (HasCondition(&actor->conditions, 0x29) != 0)
        return 0x0d;
    if (HasCondition(&actor->conditions, 4) != 0) {
        ConditionSetBit(&actor->conditions, 4, 0);
        if (ai->unk1C <= lbl_804D87D8)
            return 0x16;
    }

    u8 direction = ZoneClassifyDirection(actor, &actor->unk240);
    if (ZoneClassifyDirection(actor, &actor->unk240) == 3 && actor->unk288 != 0)
        return 0x11;

    if (ai->unk30 != direction && ai->state == 0x10) {
        AiStateMachine* state = actor->ai;
        state->state = 0x10;
        state->unk14 = 0x10;
        state->unk10 = state->unk0C;
        state->unk0C = 3;
        state->unk30 = ZoneClassifyDirection(actor, &actor->unk240);
        if (state->unk30 == 3)
            state->unk30 = 0;

        u8 subState = state->unk30;
        AiStateMachine* anim = actor->ai;
        if (HasCondition(&actor->conditions, 0x0c) != 0) {
            ConditionSetBit(&actor->conditions, 0x0c, 0);
            ConditionSetBit(&actor->conditions, 0x0e, 1);
            fn_8008F4E8(actor->zone, 0);
            fn_8008F378(actor->zone);
            fn_8008F4EC(actor->zone, 0);
        }

        int count = AnimBankGetSetCount(actor->context->value14,
                                        actor->context->value1C,
                                        lbl_803AAAD8[anim->state].unk00);
        if (count != 0 && subState <= count) {
            if (subState < 0)
                subState = rand() % count;
            anim->unk04 = fn_800A1BC8(actor->context->value14,
                                      actor->context->value1C,
                                      lbl_803AAAD8[anim->state].unk00,
                                      subState, 0);
            fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
            ConditionSetBit(&actor->conditions, 0, 0);
            fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 1,
                        lbl_803AAAD8[anim->state].unk10, actor);
            if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
                fn_8016EEF4(anim->unk28);
                anim->unk28 = -1;
            }
        }
    }

    int zone = actor->unk288;
    if (ai->unk0C == zone)
        return ai->state;
    else if (zone == 3)
        return 0x10;
    else if (zone < 3 && zone != 0 && zone > -1)
        return 0x0f;

    if (MemFlagIsSet(&actor->conditions, 8) != 0)
        return 7;
    if (MemFlagIsSet(&actor->conditions, 9) != 0)
        return 8;
    if (MemFlagIsSet(&actor->conditions, 0x0f) != 0)
        return 3;
    if (MemFlagIsSet(&actor->conditions, 0x0e) != 0)
        return 2;
    return MemFlagIsSet(&actor->conditions, 0x10) != 0 ? 4 : 0;
}

// Returns the condition-selected state id: 0x14 while condition 0x0d is set,
// otherwise the AI state machine's current state.
int GeistA714CQueryState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 0x0d) != 0)
        return 0x14;
    return ai->state;
}

// Returns the state id selected by condition 0x0d / the zone gate, falling
// back to the memory-flag state (10, 7, 8, 3, 2, 4 or 0).
int GeistA71A4QueryState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 0x0d) != 0 &&
        fn_8008EA9C(actor->zone, 0) == 1)
        return ai->state;
    actor->unk288 = 0;
    if (HasCondition(&actor->conditions, 0x3c) != 0)
        return 10;
    if (MemFlagIsSet(&actor->conditions, 0x08) != 0)
        return 7;
    if (MemFlagIsSet(&actor->conditions, 0x09) != 0)
        return 8;
    if (MemFlagIsSet(&actor->conditions, 0x0f) != 0)
        return 3;
    if (MemFlagIsSet(&actor->conditions, 0x0e) != 0)
        return 2;
    return MemFlagIsSet(&actor->conditions, 0x10) != 0 ? 4 : 0;
}

// Returns the follow-delay state id: 0x14 while condition 0x0d is set, 0x16
// once the follow condition has expired, otherwise the current AI state.
int GeistA72C0QueryState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 0x0d) != 0)
        return 0x14;
    if (HasCondition(&actor->conditions, 4) != 0) {
        ConditionSetBit(&actor->conditions, 4, 0);
        if (ai->unk1C <= lbl_804D87D8)
            return 0x16;
    }
    return ai->state;
}

// Returns 0x18 while condition 9 is set, otherwise the current AI state.
int GeistA7358QueryState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 9) != 0)
        return 0x18;
    return ai->state;
}

// Idles the AI state machine when the actor has no pending state request and
// its current animation is not the one that blocks the idle transition.
int ActorGetIdleState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    if (actor->unk5F0 == 0 || fn_80212730(actor->anim) != -1) {
        DogActorUpdateMove(actor);
    }
    return ai->state;
}

// Picks the next state id from the actor's flee-related conditions.
int ActorGetFleeState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 0x0d) != 0) {
        return 0x14;
    }
    if (HasCondition(&actor->conditions, 0x3c) == 0) {
        return 0xc;
    }
    return ai->state;
}

// Returns the AI sub-state selected by the behaviour condition flags when the
// actor is not being chased; also refreshes the direction/memory sub-state.
int DogActorChooseIndexedSubState(Actor* actor) {
    AiStateMachine* ai = actor->ai;
    int result;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 0x0d) != 0)
        return 0x14;
    if (HasCondition(&actor->conditions, 0x3c) != 0)
        return 10;
    if (HasCondition(&actor->conditions, 0x29) != 0)
        return 0x0d;
    if (HasCondition(&actor->conditions, 4) != 0) {
        ConditionSetBit(&actor->conditions, 4, 0);
        if (ai->unk1C <= lbl_804D87D8)
            return 0x16;
    }
    if (MemFlagIsSet(&actor->conditions, 9) != 0) {
        result = ai->state;
    } else {
        if (MemFlagIsSet(&actor->conditions, 8) != 0)
            ai->unk18 = 7;
        if (MemFlagIsSet(&actor->conditions, 0x0f) != 0)
            ai->unk18 = 3;
        if (MemFlagIsSet(&actor->conditions, 0x0e) != 0)
            ai->unk18 = 2;
        if (MemFlagIsSet(&actor->conditions, 0x10) != 0)
            ai->unk18 = 4;
        ai->unk18 = 0;
        result = 9;
    }
    return result;
}

// Returns the AI sub-state for the current zone direction: keeps the state
// when the zone is unchanged, otherwise reports 0x10 / 0x0f or falls back to
// the memory-flag state.
int DogActorChooseZoneSubState(Actor* actor) {
    AiStateMachine* ai = actor->ai;
    int zone;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 0x0d) != 0)
        return 0x14;
    if (HasCondition(&actor->conditions, 0x3c) != 0)
        return 10;
    if (HasCondition(&actor->conditions, 0x29) != 0)
        return 0x0d;
    if (HasCondition(&actor->conditions, 4) != 0) {
        ConditionSetBit(&actor->conditions, 4, 0);
        if (ai->unk1C <= lbl_804D87D8)
            return 0x16;
    }

    zone = actor->unk288;
    if (ai->unk0C == zone) {
        if (MemFlagIsSet(&actor->conditions, 8) != 0)
            return ai->state;
        if (MemFlagIsSet(&actor->conditions, 9) != 0)
            return 8;
        if (MemFlagIsSet(&actor->conditions, 0x0f) != 0)
            return 3;
        if (MemFlagIsSet(&actor->conditions, 0x0e) != 0)
            return 2;
        return MemFlagIsSet(&actor->conditions, 0x10) != 0 ? 4 : 0;
    }
    if (zone == 3)
        return 0x10;
    if (zone < 3 && zone != 0 && zone > 0)
        return 0x0f;
    if (MemFlagIsSet(&actor->conditions, 0x0e) != 0)
        return 2;
    return 0;
}

// Picks the AI sub-state from the queued hit/death conditions, the zone
// direction and the behaviour memory flags; the direction slot is only used
// for the hit reaction, not for the state selection.
int DogActorChooseFlagSubState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 0x0d) != 0)
        return 0x14;
    if (HasCondition(&actor->conditions, 0x3c) != 0)
        return 10;
    if (HasCondition(&actor->conditions, 0x29) != 0)
        return 0x0d;
    if (HasCondition(&actor->conditions, 4) != 0) {
        ConditionSetBit(&actor->conditions, 4, 0);
        if (ai->unk1C <= lbl_804D87D8)
            return 0x16;
    }

    if (ZoneClassifyDirection(actor, &actor->unk240) == 3 && actor->unk288 != 0)
        return 0x11;

    if (MemFlagIsSet(&actor->conditions, 8) != 0)
        return 7;
    if (MemFlagIsSet(&actor->conditions, 0x0e) != 0)
        return 2;
    if (MemFlagIsSet(&actor->conditions, 0x0f) != 0)
        return 3;
    if (MemFlagIsSet(&actor->conditions, 0x10) != 0) {
        if (ai->unk20 <= lbl_804D87D8)
            return 5;
        return ai->state;
    }
    return 0;
}

// Picks the AI sub-state from the queued hit/death conditions, the zone
// direction and the behaviour memory flags; unlike the flag variant above it
// also honours the actor's zone slot and the interaction sub-object.
int DogActorChooseZoneFlagSubState(Actor* actor) {
    AiStateMachine* ai = actor->ai;
    int zone;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 0x0d) != 0)
        return 0x14;
    if (HasCondition(&actor->conditions, 0x3c) != 0)
        return 10;
    if (HasCondition(&actor->conditions, 0x29) != 0)
        return 0x0d;
    if (fn_8016325C((u8*)actor + 0x894) == 4)
        return 0x0e;
    if (HasCondition(&actor->conditions, 4) != 0) {
        ConditionSetBit(&actor->conditions, 4, 0);
        if (ai->unk1C <= lbl_804D87D8)
            return 0x16;
    }

    if (ZoneClassifyDirection(actor, &actor->unk240) == 3 && actor->unk288 != 0)
        return 0x11;

    if (MemFlagIsSet(&actor->conditions, 8) != 0)
        return 7;

    zone = actor->unk288;
    if (ai->unk0C == zone) {
        if (MemFlagIsSet(&actor->conditions, 8) == 0) {
            if (MemFlagIsSet(&actor->conditions, 0x0e) != 0)
                return 2;
            if (MemFlagIsSet(&actor->conditions, 0x10) != 0)
                return 4;
        }
        return ai->state;
    }
    if (zone == 3)
        return 0x10;
    if (zone < 3 && zone != 0 && zone > -1)
        return 0x0f;
    return ai->state;
}

// Returns the value the behaviour uses to pick the next state from the
// current condition flags. Picks between the two zone/flag sub-states.
// Name is a guess.
int GeistA28A4StateQuery(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    DogActorUpdateMove(actor);
    if (HasCondition(&actor->conditions, 0x0d) != 0)
        return 0x14;
    if (HasCondition(&actor->conditions, 0x3c) != 0)
        return 10;
    if (HasCondition(&actor->conditions, 0x29) != 0)
        return 0x0d;
    if (HasCondition(&actor->conditions, 4) != 0) {
        ConditionSetBit(&actor->conditions, 4, 0);
        if (ai->unk1C <= lbl_804D87D8)
            return 0x16;
    }

    if (ZoneClassifyDirection(actor, &actor->unk240) == 3 && actor->unk288 != 0)
        return 0x11;

    if (MemFlagIsSet(&actor->conditions, 8) != 0)
        return 7;

    if (ai->unk0C != actor->unk288) {
        if (actor->unk288 == 3)
            return 0x10;
        if (actor->unk288 > 0 && actor->unk288 < 3)
            return 0x0f;
        if (MemFlagIsSet(&actor->conditions, 0x0f) != 0)
            return 3;
        if (MemFlagIsSet(&actor->conditions, 0x0e) != 0)
            return 2;
        return 0;
    }

    if (ai->state < 2) {
        if (MemFlagIsSet(&actor->conditions, 0x0e) != 0)
            return 2;
        if (MemFlagIsSet(&actor->conditions, 0x0f) != 0)
            return 3;
        if (MemFlagIsSet(&actor->conditions, 0x10) != 0)
            return 4;
    } else if (MemFlagIsSet(&actor->conditions, 0x0e) == 0) {
        return 0;
    }
    if (ai->unk20 <= lbl_804D87D8)
        return 1;
    return ai->state;
}

// Enters the counter-attack/alert state 0x18: arms the state timer, resets the
// zone bookkeeping and starts the one-shot animation once. Name is a guess.
int GeistA28A4State24Enter(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 0x18;
    ai->unk14 = 0x18;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;
    fn_8008DE4C(actor->zone, 0, ai->unk04, 0x18);
    if (HasCondition(&actor->conditions, 10) == 0) {
        ConditionSetBit(&actor->conditions, 10, 1);
        ActorPlayAnim(actor->anim, 0xdc04e3d5UL, actor->anim);
    }
    return 1;
}

// Enters the death state 0x17: arms the state timer, restarts the animation set
// (random clip) and plays the death sound. Returns 1. Name is a guess.
int GeistA7DA0EnterDeathState(Actor* actor) {
    int result = 1;
    AiStateMachine* ai = actor->ai;

    ai->state = 0x17;
    ai->unk14 = 0x18;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    if (actor->unk5F0 == 0 || fn_80212730(actor->anim) != -1) {
        ai = actor->ai;
        result = HasCondition(&actor->conditions, 0x0c);
        if (result != 0) {
            ConditionSetBit(&actor->conditions, 0x0c, 0);
            ConditionSetBit(&actor->conditions, 0x0e, 1);
            fn_8008F4E8(actor->zone, 0);
            fn_8008F378(actor->zone);
            fn_8008F4EC(actor->zone, 0);
        }
        int count = AnimBankGetSetCount(actor->context->value14,
                                        actor->context->value1C,
                                        lbl_803AAAD8[ai->state].unk00);
        if (count == 0) {
            result = -1;
        } else {
            result = rand();
            result = result - (result / count) * count;
            ai->unk04 = fn_800A1BC8(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[ai->state].unk00, result, 0);
            fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
            ConditionSetBit(&actor->conditions, 0, 1);
            fn_80090974(lbl_804D87EC, actor->zone, 0, ai->unk04, 0x18,
                        lbl_803AAAD8[ai->state].unk10, actor);
            if (ai->unk28 != -1 && ai->state != 0 && ai->state != 1) {
                fn_8016EEF4(ai->unk28);
                ai->unk28 = -1;
            }
        }
    }

    if (result == 0)
        fn_8016F120(lbl_804D30AC, (u8*)actor + 0x24);
    else
        fn_8016F120(lbl_804D30B0, (u8*)actor + 0x24);
    return 1;
}

// Enters state 0x16: arms the turn timer and restarts the direction animation
// set, then plays the pain sound when the direction slot holds 3. Returns 1.
// Name is a guess.
int GeistA7F94EnterFleeState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->unk1C = lbl_804D87F0;
    ai->unk14 = ai->state;
    ai->state = 0x16;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    int sub = ai->unk30;
    ai = actor->ai;

    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[ai->state].unk00);
    if (count != 0 && sub <= count) {
        if (sub < 0)
            sub = rand() % count;
        ai->unk04 = fn_800A1BC8(actor->context->value14,
                                actor->context->value1C,
                                lbl_803AAAD8[ai->state].unk00, sub, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, ai->unk04, 8,
                    lbl_803AAAD8[ai->state].unk10, actor);
        if (ai->unk28 != -1 && ai->state != 0 && ai->state != 1) {
            fn_8016EEF4(ai->unk28);
            ai->unk28 = -1;
        }
    }

    if ((signed char)ai->unk30 == 3)
        fn_8016F120(lbl_804D30A8, (u8*)actor + 0x24);
    return 1;
}

// Enters state 3: remembers the state it came from, clears the direction timer
// and the direction slot, then returns 1. Name is a guess.
int GeistA8168EnterState3(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->unk14 = ai->state;
    ai->state = 3;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;
    ai->unk30 = 0;
    return 1;
}

// Enters state 0x14 (20): remembers the state it came from unless it is already
// in 0x14, clears the direction timer and slot, then returns 1. Name is a guess.
int GeistA8198EnterState20(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    if (ai->state != 0x14)
        ai->unk14 = ai->state;
    ai->state = 0x14;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;
    ai->unk30 = 0;
    return 1;
}

// Enters state 0x13: arms the state timer, restarts the animation set (random
// clip) and plays the direction sound. Returns 1, or 0 without an actor.
// Name is a guess.
int GeistA81D0EnterState19(Actor* actor) {
    if (actor == 0)
        return 0;

    AiStateMachine* ai = actor->ai;
    ai->state = 0x13;
    ai->unk14 = 0x13;
    ai->unk30 = 0;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0 && count >= 0) {
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00, 0, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 0);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 1,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 0x12: resets the actor's movement and condition blocks, then
// either restarts the animation set or detaches the extra trail, and finally
// clears the pending interaction pointer. Returns 1, or 0 without an actor.
// Name is a guess.
int GeistA8370EnterState18(Actor* actor) {
    if (actor == 0)
        return 0;

    AiStateMachine* ai = actor->ai;
    Vec3 zero;
    zero.x = lbl_804D87D8;
    zero.y = lbl_804D87D8;
    zero.z = lbl_804D87D8;

    actor->unk288 = 0;
    actor->unk26C = 0;
    fn_800B5004(&actor->unk240, &zero);
    MemCopyVec3((float*)&actor->conditions, &zero.x);
    fn_800BBCA8(actor);
    RangeResetPending(&actor->conditions);

    ai->state = 0x12;
    ai->unk14 = 0x13;
    ai->unk30 = 0;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    if (actor->unk5F0 == 0) {
        AiStateMachine* anim = actor->ai;
        if (HasCondition(&actor->conditions, 0x0c) != 0) {
            ConditionSetBit(&actor->conditions, 0x0c, 0);
            ConditionSetBit(&actor->conditions, 0x0e, 1);
            fn_8008F4E8(actor->zone, 0);
            fn_8008F378(actor->zone);
            fn_8008F4EC(actor->zone, 0);
        }
        int count = AnimBankGetSetCount(actor->context->value14,
                                        actor->context->value1C,
                                        lbl_803AAAD8[anim->state].unk00);
        if (count != 0 && count >= 0) {
            anim->unk04 = fn_800A1BC8(actor->context->value14,
                                      actor->context->value1C,
                                      lbl_803AAAD8[anim->state].unk00, 0, 0);
            fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
            ConditionSetBit(&actor->conditions, 0, 0);
            fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 1,
                        lbl_803AAAD8[anim->state].unk10, actor);
            if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
                fn_8016EEF4(anim->unk28);
                anim->unk28 = -1;
            }
        }
    } else {
        ExtraTrailAttach(actor->unk5F0);
        if (actor->unk5F4 != 0) {
            fn_801D55CC();
            actor->unk5F4 = 0;
        }
    }

    fn_800A7B08(&actor->conditions, 4, 0);
    fn_800A7A54(&actor->conditions, 0x0b, 1);
    ai->unk2C = actor->unk0FC;
    actor->unk0FC = 0;
    return 1;
}

// Enters state 0x11: remembers the state it came from, marks the direction
// timer as running and restarts the animation set with a random clip.
// Returns 1. Name is a guess.
int GeistA85BCEnterState17(Actor* actor) {
    AiStateMachine* ai = actor->ai;
    ai->state = 0x11;
    ai->unk14 = 0x11;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 1;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00,
                                  rand() % count, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 0);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 1,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 0x10: remembers the state it came from, classifies the actor's
// facing direction and restarts the animation set, picking the clip from that
// direction or at random. Returns 1. Name is a guess.
int GeistA875CEnterState16(Actor* actor) {
    AiStateMachine* ai = actor->ai;
    ai->state = 0x10;
    ai->unk14 = 0x10;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 3;
    ai->unk30 = ZoneClassifyDirection(actor, &actor->unk240);
    if (ai->unk30 == 3)
        ai->unk30 = 0;

    int dir = ai->unk30;
    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        if (dir < 0)
            dir = rand() % count;
        else if (dir > count)
            return 1;
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00, dir, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 0);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 1,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 0x0f: remembers the state it came from, marks the direction
// timer as running and restarts the animation set with a random clip.
// Returns 1. Name is a guess.
int GeistA8940EnterState15(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 0x0f;
    ai->unk14 = 0x0f;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 2;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00,
                                  rand() % count, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 0);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 1,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 0x0e: remembers the state it came from (except from state 0x16,
// which is recorded as 3), resets the direction timer and restarts the
// animation set with a random clip. Returns 1. Name is a guess.
int GeistA8AE0EnterState14(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    if (ai->state == 0x16)
        ai->unk14 = 3;
    else
        ai->unk14 = ai->state;
    ai->state = 0x0e;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00,
                                  rand() % count, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 0x0d: records the state it came from, resets the direction timer
// and starts a fixed animation slot (index 0) in the current animation set.
// Returns 1. Name is a guess.
int GeistA8C98EnterState13(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 0x0d;
    ai->unk14 = 3;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0 && count >= 0) {
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00,
                                  0, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 0x18,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 0x0c: records the state it came from and starts animation slot 2
// of the current animation set. Returns 1. Name is a guess.
int GeistA8E2CEnterState12(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 0x0c;
    ai->unk14 = 3;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0 && count >= 2) {
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00,
                                  2, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 0x0b: records state 0x0b as the previous state, resets the
// direction timer and starts animation slot 1 of the current animation set.
// Returns 1. Name is a guess.
int GeistA8FC4EnterState11(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 0x0b;
    ai->unk14 = 0x0b;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0 && count > 0) {
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00,
                                  1, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 0x0a: records state 0x0b as the previous state, resets the
// direction timer and starts animation slot 0 of the current animation set.
// Returns 1. Name is a guess.
int GeistA9158EnterState10(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 0x0a;
    ai->unk14 = 0x0b;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0 && count >= 0) {
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00,
                                  0, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 9: remembers the previous state, resets the direction timer and
// restarts the animation set with animation slot 2 when the bank has at least
// two clips. Returns 1. Name is a guess.
extern "C" int fn_800A1AC4(int a, int b, int c);
int SplineAssetResolve(SplineNameEntry* entries, int a, int b);

int GeistA92ECEnterState9(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 9;
    ai->unk14 = ai->unk18;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0 && count >= 2) {
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00, 2, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 0);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 1,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 8 (the dog's cower state): remembers the previous state, resets
// the direction timer and, when the cowering spline entry is active, restarts
// the animation set with animation slot 1 or 0. Returns 1. Name is a guess.
int GeistA9484EnterState8(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 8;
    ai->unk14 = 8;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    int resolved = SplineAssetResolve(lbl_803AAAB8, 2, 0);
    if (ai->unk04 == resolved ||
        ai->unk04 == SplineAssetResolve(lbl_803AAAB8, 2, 1)) {
        AiStateMachine* anim = actor->ai;
        if (HasCondition(&actor->conditions, 0x0c) != 0) {
            ConditionSetBit(&actor->conditions, 0x0c, 0);
            ConditionSetBit(&actor->conditions, 0x0e, 1);
            fn_8008F4E8(actor->zone, 0);
            fn_8008F378(actor->zone);
            fn_8008F4EC(actor->zone, 0);
        }
        int count = AnimBankGetSetCount(actor->context->value14,
                                        actor->context->value1C,
                                        lbl_803AAAD8[anim->state].unk00);
        if (count != 0 && count >= 1) {
            anim->unk04 = fn_800A1BC8(actor->context->value14,
                                      actor->context->value1C,
                                      lbl_803AAAD8[anim->state].unk00, 1, 0);
            fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
            ConditionSetBit(&actor->conditions, 0, 0);
            fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 1,
                        lbl_803AAAD8[anim->state].unk10, actor);
            if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
                fn_8016EEF4(anim->unk28);
                anim->unk28 = -1;
            }
        }
    } else {
        AiStateMachine* anim = actor->ai;
        if (HasCondition(&actor->conditions, 0x0c) != 0) {
            ConditionSetBit(&actor->conditions, 0x0c, 0);
            ConditionSetBit(&actor->conditions, 0x0e, 1);
            fn_8008F4E8(actor->zone, 0);
            fn_8008F378(actor->zone);
            fn_8008F4EC(actor->zone, 0);
        }
        int count = AnimBankGetSetCount(actor->context->value14,
                                        actor->context->value1C,
                                        lbl_803AAAD8[anim->state].unk00);
        if (count != 0 && count > 0) {
            anim->unk04 = fn_800A1BC8(actor->context->value14,
                                      actor->context->value1C,
                                      lbl_803AAAD8[anim->state].unk00, 0, 0);
            fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
            ConditionSetBit(&actor->conditions, 0, 0);
            fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 1,
                        lbl_803AAAD8[anim->state].unk10, actor);
            if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
                fn_8016EEF4(anim->unk28);
                anim->unk28 = -1;
            }
        }
    }
    return 1;
}

// Enters state 7: records 7 as the previous state, resets the direction timer
// and starts a random animation from the current set; the sound-effect handle
// is selected by whether the random clip index came out as the first one.
// Returns 1. Name is a guess.
int GeistA9798EnterState7(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 7;
    ai->unk14 = 7;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int index;
    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count == 0) {
        index = -1;
    } else {
        index = rand() % count;
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00, index, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    if (index == 0) {
        fn_8016F120(lbl_804D30A4, &actor->cellX);
    } else {
        fn_8016F120(lbl_804D30A0, &actor->cellX);
    }
    return 1;
}

// Enters state 6: records 0x0f as the previous state, resets the direction
// timer and starts a random animation from the current set. Returns 1. Name is
// a guess.
int GeistA9968EnterState6(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->unk14 = 0x0f;
    ai->state = 6;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        int index = rand() % count;
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00, index, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 5: records the previous state, resets the direction timer and
// starts a random animation from the current set with event flag 9. Returns 1.
// Name is a guess.
int GeistA9B0CEnterState5(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->unk14 = ai->state;
    ai->state = 5;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        int index = rand() % count;
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00, index, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 4: records 4 as both the state and the previous state, resets
// the direction timer and starts a random animation from the current set with
// event flag 9. Returns 1. Name is a guess.
int GeistA9CB0EnterState4(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 4;
    ai->unk14 = 4;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        int index = rand() % count;
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00, index, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 3: records 3 as both the state and the previous state, resets
// the direction timer and starts a random animation from the current set with
// event flag 9. Returns 1. Name is a guess.
int GeistA9E50EnterState3(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->state = 3;
    ai->unk14 = 3;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        int index = rand() % count;
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00, index, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// Enters state 2: records 2 as both the previous state and the state, resets
// the direction timer and starts a random animation from the current set with
// event flag 9. Returns 1. Name is a guess.
int GeistA9FF0EnterState2(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    ai->unk14 = 2;
    ai->state = 2;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        int index = rand() % count;
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00, index, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }
    return 1;
}

// State-enter callback for the dog's idle state: commits the outgoing state,
// clears the transition slot, refreshes the animation set and restarts the
// "idle" sound effect with a random variant.
int DogActorEnterIdleState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    if (ai->unk28 != -1 && ai->state != 0 && ai->state != 1) {
        fn_8016EEF4(ai->unk28);
        ai->unk28 = -1;
    }
    ai->unk14 = ai->state;
    ai->state = 1;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        int index = rand() % count;
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00,
                                  index, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }

    if (ai->unk28 == -1)
        ai->unk28 = fn_8016F120(lbl_804D30B4, (u8*)actor + 0x24);
    return 1;
}

// State-enter callback for the dog's rest state: clears the current state and
// the pending transition, refreshes the animation set and keeps the idle sound
// playing with a random variant.
int DogActorEnterRestState(Actor* actor) {
    AiStateMachine* ai = actor->ai;

    if (ai->unk28 != -1 && ai->state != 0 && ai->state != 1) {
        fn_8016EEF4(ai->unk28);
        ai->unk28 = -1;
    }
    ai->state = 0;
    ai->unk14 = 0;
    ai->unk10 = ai->unk0C;
    ai->unk0C = 0;

    AiStateMachine* anim = actor->ai;
    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        ConditionSetBit(&actor->conditions, 0x0c, 0);
        ConditionSetBit(&actor->conditions, 0x0e, 1);
        fn_8008F4E8(actor->zone, 0);
        fn_8008F378(actor->zone);
        fn_8008F4EC(actor->zone, 0);
    }

    int count = AnimBankGetSetCount(actor->context->value14,
                                    actor->context->value1C,
                                    lbl_803AAAD8[anim->state].unk00);
    if (count != 0) {
        int index = rand() % count;
        anim->unk04 = fn_800A1BC8(actor->context->value14,
                                  actor->context->value1C,
                                  lbl_803AAAD8[anim->state].unk00,
                                  index, 0);
        fn_8008F230(lbl_804D87E8, actor->zone, 0, 1);
        ConditionSetBit(&actor->conditions, 0, 1);
        fn_80090974(lbl_804D87EC, actor->zone, 0, anim->unk04, 9,
                    lbl_803AAAD8[anim->state].unk10, actor);
        if (anim->unk28 != -1 && anim->state != 0 && anim->state != 1) {
            fn_8016EEF4(anim->unk28);
            anim->unk28 = -1;
        }
    }

    if (ai->unk28 == -1)
        ai->unk28 = fn_8016F120(lbl_804D30B4, (u8*)actor + 0x24);
    return 1;
}

// Per-frame locomotion/animation update for the dog actor: advances the current
// animation, then either snaps the actor's matrix to the placement (conditions
// 0x0c / 0x0d) or integrates the steering angles and rebuilds the model matrix.
void DogActorUpdateMove(Actor* actor) {
    Vec3 offset;            // 0x08
    Vec3 angle;             // 0x14
    Mtx34 mtx;              // 0x20
    PlaceF place;           // 0x50
    int handle;

    if (fn_8008E964(actor->zone) == 0 &&
        HasCondition(&actor->conditions, 0x0c) != 0)
        return;

    fn_800913EC(actor->zone, actor->unk138, &offset);

    int animIndex = ModelAnimate(actor->animHandle, (char*)actor->placement, 1);
    AnimApply((int)actor->zone, (char*)actor->placement, animIndex,
              actor->unk0D0);

    if (HasCondition(&actor->conditions, 0x0c) != 0) {
        actor->unk918 = lbl_804D87F4;
        fn_800CC2D0(actor, 0, 0, 0);
        if (fn_8008EA9C(actor->zone, 0) != 0) {
            if (HasCondition(&actor->conditions, 0x0c) != 0) {
                BoneGetMatrix(&place.m, actor->placement);
                MtxGetEuler(&actor->euler, &place.m);
                PlacementToMatrix(&actor->matrix, &place);
                actor->pos.x = actor->placement->x;
                actor->pos.y = actor->placement->y;
                actor->pos.z = actor->placement->z;
                actor->cellX = (int)actor->placement->x;
                actor->cellY = (int)actor->placement->y;
                actor->cellZ = (int)actor->placement->z;
            } else {
                Vec3Set(&angle, actor->euler.z, lbl_804D87D8, lbl_804D87D8);
                EulerNormalize(&angle, &angle);
                MtxSetEuler(&place.m, &angle);
                place.pos.x = (float)actor->cellX;
                place.pos.y = (float)actor->cellY;
                place.pos.z = (float)actor->cellZ;
                PlacementToMatrix(&actor->matrix, &place);
            }
        }
        ModelSetMatrix(actor->animHandle, 0, (char*)actor->placement);
        return;
    }

    if (HasCondition(&actor->conditions, 0x0d) != 0) {
        actor->unk918 = lbl_804D87F4;
        float f = fn_800CC2D0(actor, 0, 0, 0);
        if (f != lbl_804D87F4) {
            actor->pos.z = f + actor->placement->z;
            actor->placement->z = lbl_804D87D8;
        }
        actor->cellZ = (int)actor->pos.z;

        ActorGetMatrix(&mtx, actor);
        fn_800A0168(&mtx, &mtx);
        fn_800A0030(&offset, &mtx, &offset);

        actor->unk240.x = lbl_804D87D8;
        actor->unk240.y = lbl_804D87D8;
        actor->unk240.z = lbl_804D87D8;

        EulerNormalize(&actor->euler, &actor->euler);
        Vec3Set(&angle, actor->euler.z, lbl_804D87D8, lbl_804D87D8);
        EulerNormalize(&angle, &angle);
        MtxSetEuler(&place.m, &angle);
        place.pos.x = (float)actor->cellX;
        place.pos.y = (float)actor->cellY;
        place.pos.z = (float)actor->cellZ;
        PlacementToMatrix(&actor->matrix, &place);

        ActorGetMatrix(&mtx, actor);
        fn_800A0168(&mtx, &mtx);
        offset.z = lbl_804D87D8;
        fn_800A0030(&offset, &mtx, &offset);
        place.pos.x += offset.x;
        place.pos.y += offset.y;
        place.pos.z += offset.z;
        ModelSetMatrix(actor->animHandle, &place.m, (char*)actor->placement);
        return;
    }

    handle = 0;

    float limit = actor->context->unk08;
    float value = actor->euler.x;
    if (value < -limit)
        value = -limit;
    else if (value > limit)
        value = limit;
    actor->euler.x = value;

    actor->unk918 = lbl_804D87F4;
    float f = fn_800CC2D0(actor, 0, 0, 0);
    if (f != lbl_804D87F4)
        actor->pos.z = f + actor->placement->z;

    actor->cellZ = (int)actor->pos.z;
    actor->placement->x = lbl_804D87D8;
    actor->placement->y = lbl_804D87D8;
    actor->placement->z = lbl_804D87D8;

    if (HasCondition(&actor->conditions, 0) == 0 &&
        MemFlagIsSet(&actor->conditions, 4) == 0 &&
        MemFlagIsSet(&actor->conditions, 0x17) == 0 &&
        HasCondition(&actor->conditions, 0x40) == 0) {
        handle = fn_800CADE0(actor);
        ConditionSetBit(&actor->conditions, 0x2d, 0);
    } else {
        ConditionSetBit(&actor->conditions, 0x2d, 1);
    }

    float s = actor->unk284;
    float t = actor->unk27C;
    float speed;
    if (HasCondition(&actor->conditions, 0x28) != 0)
        speed = lbl_804D87F8 * actor->context->unkC0;
    else if (HasCondition(&actor->conditions, 0x27) != 0)
        speed = actor->context->unkC0;
    else
        speed = lbl_804D87FC * actor->context->unkC0;

    if (s > lbl_804D8800) {
        float d = (speed * actor->unk138) / lbl_804D8804;
        if (d >= s)
            d = s;
        actor->euler.z = actor->euler.z + d;
    } else if (s < lbl_804D8808) {
        float d = -(speed * actor->unk138 / lbl_804D8804);
        if (d <= s)
            d = s;
        actor->euler.z = actor->euler.z + d;
    }

    if (t > lbl_804D8800) {
        float d = (actor->context->unkC0 * actor->unk138) / lbl_804D8804;
        if (d >= t)
            d = t;
        actor->euler.x = actor->euler.x + d;
    } else if (t < lbl_804D8808) {
        float d = -(actor->context->unkC0 * actor->unk138 / lbl_804D8804);
        if (d <= t)
            d = t;
        actor->euler.x = actor->euler.x + d;
    }

    if (HasCondition(&actor->conditions, 0) != 0) {
        ActorGetMatrix(&mtx, actor);
        fn_800A0168(&mtx, &mtx);
        fn_800A0030(&offset, &mtx, &offset);
        if (actor->unk138 > lbl_804D87D8) {
            actor->unk240.x = offset.x;
            actor->unk240.y = offset.y;
            actor->unk240.z = offset.z;
            actor->unk240.z = lbl_804D87D8;
            float scale = lbl_804D8804 / actor->unk138;
            actor->unk240.x = actor->unk240.x * scale;
            actor->unk240.y = actor->unk240.y * scale;
            actor->unk240.z = actor->unk240.z * scale;
        } else {
            actor->unk240.x = lbl_804D87D8;
            actor->unk240.y = lbl_804D87D8;
            actor->unk240.z = lbl_804D87D8;
        }
    } else if (HasCondition(&actor->conditions, 0x2d) == 0) {
        fn_800CAD0C(actor, handle);
    }

    EulerNormalize(&actor->euler, &actor->euler);
    Vec3Set(&angle, actor->euler.z, lbl_804D87D8, lbl_804D87D8);
    MtxSetEuler(&place.m, &angle);
    place.pos.x = (float)actor->cellX;
    place.pos.y = (float)actor->cellY;
    place.pos.z = (float)actor->cellZ;
    PlacementToMatrix(&actor->matrix, &place);

    ActorGetMatrix(&mtx, actor);
    fn_800A0168(&mtx, &mtx);
    offset.y = lbl_804D87D8;
    offset.z = lbl_804D87D8;
    fn_800A0030(&offset, &mtx, &offset);
    place.pos.x += offset.x;
    place.pos.y += offset.y;
    place.pos.z += offset.z;
    ModelSetMatrix(actor->animHandle, &place.m, (char*)actor->placement);
}

// TODO fn_801AA570 (0x838 bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_801AA570(void);

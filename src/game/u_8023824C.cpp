// u_8023824C.cpp: follow-marker and follow-behaviour states and messages (0x8023824C-0x8023B96C).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct Vector3f {
    float x;  // 0x00
    float y;  // 0x04
    float z;  // 0x08
};

// Object info returned by the game's owner/object lookup helpers.
// Field names and offsets are guesses based on usage.
struct FollowObject {
    u8 unk00[0x24];
    s32 x;        // 0x24
    s32 y;        // 0x28
    s32 z;        // 0x2c
    u8 unk30[0x38 - 0x30];
    s32 ownerId;  // 0x38
};

// Per-actor behaviour state reached through owner->state (0x130).
struct FollowState {
    s32 stateKind;  // 0x00 - behaviour sub-mode written on every tick
    s32 stateId;    // 0x04 - current state name hash
    int msgTarget;  // 0x08 - handle used as SendMsg destination
    s32 targetId;   // 0x0c - current target owner id
    float timer;    // 0x10
    float unk14;    // 0x14
};

// Message payload filled by SendMsg (message 0x400), 0x20 bytes.
struct MsgData {
    void* obj;  // 0x00
    u32 unk04[7];
};

// Model holder reached through Owner::unkC4; +0x30 is the model's bone or
// animation count, which the behaviour initialiser converts from an integer.
struct FollowModel {
    u8 unk00[0x30];
    s32 unk30;  // 0x30
};

struct Owner {
    u8 unk000[0x24];
    s32 x;                     // 0x24
    s32 y;                     // 0x28
    s32 z;                     // 0x2C
    void (*unk30)(int, void*); // 0x30 - message handler
    u8 unk34[0x38 - 0x34];
    s32 unk38;                 // 0x38 - object id handed to the effect system
    u8 unk3C[0x84 - 0x3C];
    u8 unk84[0xC4 - 0x84];     // 0x84 - transform block handed to the effect system
    FollowModel* unkC4;        // 0xC4 - model holder (bone count at +0x30)
    u8 unkC8[0xCC - 0xC8];
    s32 unkCC;                 // 0xCC - model id
    u8 unkD0[0xD4 - 0xD0];
    s32 unkD4;                 // 0xD4
    u8 unkD8[0xFC - 0xD8];
    void (*unkFC)();           // 0xFC
    u8 unk100[0x104 - 0x100];
    s32 unk104;                // 0x104 - cleared when the behaviour is released
    u8 unk108[0x130 - 0x108];
    FollowState* state;        // 0x130
    u8 unk134[0x138 - 0x134];
    float unk138;              // 0x138
    u8 unk13C[0x1E4 - 0x13C];
    s32 unk1E4[12];            // 0x1E4 - four 3-word candidate entries
    u32 unk214;                // 0x214
    u8 unk218[0x270 - 0x218];
    u8 followTarget;           // 0x270 - condition block
    u8 unk271[0x288 - 0x271];
    s32 unk288;                // 0x288 - follow view state (0 or 3)
    u8 unk28C[0x5B0 - 0x28C];
    u8 markerBlock[4];         // 0x5B0 - block updated through the marker message
    float unk5B4;              // 0x5B4 - one of the distance thresholds
    float unk5B8;              // 0x5B8 - distance mode value
    float unk5BC;              // 0x5BC
    u8 unk5C0[0x5D0 - 0x5C0];
    s32 unk5D0;                // 0x5D0 - distance mode selector
    u8 unk5D4[0x5EC - 0x5D4];
    int unk5EC;                // 0x5EC
    s32 unk5F0;                // 0x5F0 - extra trail holder
    s32 unk5F4;                // 0x5F4
    u8 unk5F8[0x600 - 0x5F8];
    void (*unk600)();          // 0x600 - behaviour tick callback
    void (*unk604)();          // 0x604 - message handler callback
    u8 unk608[0x658 - 0x608];
    u8 unk658[0x894 - 0x658];  // 0x658 - zone list block

    u8 unk894[0x8EC - 0x894];  // 0x894 - follow view block
    int unk8EC;                // 0x8EC - event sink key
    u8 unk8F0[0x8F8 - 0x8F0];
    s32 unk8F8;                // 0x8F8 - marker refreshed flag
    u8 unk8FC[0x904 - 0x8FC];
    float unk904;              // 0x904 - measured target distance
};

extern float lbl_804D34D0;
extern float lbl_804D99A0;
extern float lbl_804D99A4;
extern float lbl_804D99A8;
extern float lbl_804D99AC;
extern float lbl_804D99B0;
extern float lbl_804D99B4;

int OwnerId(Owner* owner);
int HandleToPlayer(int handle);
int PlayerTeam(int player);
int FollowLineCheckActorDist(void* a, void* b, float dist);
FollowObject* OwnerGetInfo(u32 id);
void ClearStateMachine(void* owner);
void SetStateMachineState(void* owner, u32 state);
void FollowSetTargetObj(void* a, void* b);

extern "C" int fn_800B26B8(void* owner);
extern "C" void fn_800B261C(void* owner, int index, FollowObject** out);
extern "C" void fn_800B1F20(void* owner, int index);
extern "C" void fn_800B1AB8(float value, void* owner, int index, int flag);
extern "C" int fn_800BC01C(void* owner, Vector3f* pos);
extern "C" int fn_80117FB4(void* owner);
extern "C" int fn_80117FF8(int handle, u32 name, int* out);
extern "C" void fn_80118084(int handle, u32 name, int* out);
extern "C" void fn_80116E30(void* owner);
extern "C" int fn_80237F68(void* owner, int id);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int rand(void);

// State tick that accumulates a timer and retargets the follow behaviour.
int FollowStateTimerUpdate(void* p) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    FollowState* state = owner->state;
    int index = fn_800B26B8(owner);
    FollowObject* obj;
    fn_800B261C(owner, index, &obj);
    state->timer = state->timer + owner->unk138;
    if (index < 4 && state->timer > lbl_804D34D0 &&
        fn_80237F68(owner, obj->ownerId) != 0) {
        fn_800B1F20(owner, index);
        fn_800B1AB8(lbl_804D99A0, owner, index, 1);
        if (PlayerTeam(HandleToPlayer(OwnerId(owner))) !=
            PlayerTeam(HandleToPlayer(obj->ownerId))) {
            Vector3f pos;
            pos.x = (float)obj->x;
            pos.y = (float)obj->y;
            pos.z = (float)obj->z;
            if (fn_800BC01C(owner, &pos) != 0 ||
                FollowLineCheckActorDist(owner, obj, lbl_804D99A4) != 0) {
                state->targetId = obj->ownerId;
                return 1;
            }
        }
    }
    if (fn_80117FB4(owner) == 0) {
        return 1;
    }
    MsgData msg;
    SendMsg(state->msgTarget, 0x400, &msg);
    switch (state->stateId) {
    case 0x3147A953:
        return 1;
    case 0x3529A482:
        return 1;
    case -0x53413F58:  // 0xACBFC0A8
        return 1;
    case -0x454EF93F:  // 0xBAB106C1
        return 1;
    case -0x16D6524A:  // 0xE92AADB6
        fn_80116E30(owner);
        return 1;
    case 0x5A19E256:
    case 0x64F367A2: {
        int id = -1;
        fn_80117FF8(owner->unk5EC, 0x4D4915E6, &id);
        if (id == -1) {
            fn_80117FF8(owner->unk5EC, 0xFBA23962, &id);
        }
        if (id != -1) {
            FollowObject* info = (FollowObject*)OwnerGetInfo(id);
            Vector3f pos;
            pos.x = (float)info->x;
            pos.y = (float)info->y;
            pos.z = (float)info->z;
            if (fn_800BC01C(owner, &pos) != 0) {
                ClearStateMachine(owner);
                SetStateMachineState(owner, 0xDF1271D9);
                fn_80118084(owner->unk5EC, 0x315C41D, &id);
            }
            state->stateId = 0;
            return 0;
        }
        return 1;
    }
    default:
        return 0;
    }
}

// State tick that uses a random threshold to decide whether to pick a new
// target, then reports whether the message target handle is still unset.
int FollowStateRandUpdate(void* p) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    FollowState* state = owner->state;
    if (fn_80117FB4(owner) == 0) {
        return 1;
    }
    switch (state->stateId) {
    case 0x30A0D567:
        return 1;
    case 0x3529A482:
        return 1;
    case -0x53413F58:  // 0xACBFC0A8
        return 1;
    case -0x454EF93F:  // 0xBAB106C1
        return 1;
    case -0x16D6524A:  // 0xE92AADB6
        fn_80116E30(owner);
        return 1;
    case 0x64F367A2: {
        int id = -1;
        if (owner->unk5BC <
            lbl_804D99AC +
                (lbl_804D99B0 * ((float)rand() / lbl_804D99A8)) /
                    lbl_804D99B4) {
            fn_80117FF8(owner->unk5EC, 0x4D4915E6, &id);
            if (id == -1) {
                fn_80117FF8(owner->unk5EC, 0xFBA23962, &id);
            }
            if (id != -1) {
                fn_80118084(owner->unk5EC, 0x315C41D, &id);
                MsgData msg;
                SendMsg(id, 0x400, &msg);
                FollowSetTargetObj(&owner->followTarget, msg.obj);
            }
            state->stateId = 0;
        }
        break;
    }
    default:
        break;
    }
    return state->msgTarget == -1;
}

// Minimal stand-in for the engine's String class; the real constructor and
// destructor live in another unit.  Only the leading handle word is used here.
class String {
public:
    String();
    String(const char* text);
    ~String();
    void* mData;   // 0x00 - handle written back by the message handler
    u32 mHash;     // 0x04
    u32 mUnk08;    // 0x08
};

// Payload for the 0x41E lookup message: a three-word header followed by the
// String the handler fills in.
struct NameQuery {
    u32 unk00;    // 0x00
    u32 hash;     // 0x04 - lookup hash
    u32 unk08;    // 0x08
    String name;  // 0x0c
};

extern "C" void fn_801174B0(void* owner, void* obj);
extern "C" void fn_800CE96C();
extern "C" void fn_800CF064();

// State tick for the follow behaviour's search states: resolves the current
// target through the 0x41E lookup message and reports 1 while the state has
// nothing else to do.
int FollowSearchStateUpdate(void* p) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    FollowState* state = owner->state;
    if (fn_80117FB4(owner) == 0) {
        return 1;
    }
    switch (state->stateId) {
    case 0x30A0D567:
        return 1;
    case -0x53413F58:  // 0xACBFC0A8
        return 1;
    case 0x3529A482:
        return 1;
    case 0x3147A953: {
        NameQuery query;
        query.hash = 0x33032FA9;
        query.unk08 = 0;
        query.name.mData = 0;
        SendMsg(state->msgTarget, 0x41E, &query);
        void* handle = query.name.mData;
        if (handle == (void*)1) {
            NameQuery second;
            second.hash = 0x7C18FE9E;
            second.unk08 = 0;
            second.name.mData = 0;
            int found = SendMsg(state->msgTarget, 0x41E, &second);
            int target = (int)second.name.mData;
            if (found != 0 && target != -1) {
                MsgData msg;
                SendMsg(target, 0x400, &msg);
                if (msg.obj != 0 && (msg.unk04[0] >> 24) == 0x13) {
                    fn_801174B0(owner, msg.obj);
                    if (*(s32*)(*(s32*)((char*)msg.obj + 0xC4) + 0x174) ==
                        -0x646CB21C) {
                        owner->unkFC = fn_800CE96C;
                        owner->unk604 = fn_800CF064;
                    }
                    return 1;
                }
            }
        }
        return 1;
    }
    default:
        break;
    }
    return state->msgTarget == -1;
}

// Tiny state predicate: always reports the state as still active.
int FollowSearchStateActive(void) {
    return 1;
}

// State callback: clears the follow behaviour's message target handle and
// reports that the state handled the call.
int FollowClearMsgTarget(void* p) {
    if (p == 0) {
        return 0;
    }
    ((Owner*)p)->state->msgTarget = -1;
    return 1;
}

// Second variant of the message-target clearing callback, used by another
// state that ends the search the same way.
int FollowClearMsgTargetAlt(void* p) {
    if (p == 0) {
        return 0;
    }
    ((Owner*)p)->state->msgTarget = -1;
    return 1;
}

extern float lbl_804D99B8;
extern float lbl_804D99BC;
extern "C" int fn_80116E08(void* owner);
int FollowSelectTarget(void* p, void* s);
extern "C" int fn_800B93DC(void* table, int count, void* out, int size, int flag);
int Rand(void);

// Sub-object reached through Owner::unk5EC (the behaviour's message table); the
// follow behaviour inspects a state id at 0xBC.
struct FollowHandleObj {
    u8 unk00[0xC];
    float unk0C;  // 0x0C - behaviour's distance scale
    u8 unk10[0xBC - 0x10];
    s32 unkBC;  // 0xBC
};

// Picks a new follow target: resolves the owner's team through the 0x41E name
// lookup, builds the per-team candidate table and lets the engine choose one of
// the reachable candidates, then hands it to the state machine.
// Inlined by both the pick-target state and the target-check state below.
static inline int FollowPickRandomTarget(void* p) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    FollowState* state = owner->state;
    state->stateKind = 2;
    state->stateId = 0;
    fn_80116E08(owner);
    float dist = lbl_804D99B8;
    int team = PlayerTeam(HandleToPlayer(OwnerId(owner)));
    void* handle;
    {
        NameQuery query;
        query.hash = 0x31809F6F;
        query.unk08 = 2;
        SendMsg(0x4B000000, 0x41E, &query);
        handle = query.name.mData;
    }
    u32 table[3];
    switch (team) {
    case 0:
    case 5:
        table[0] = 0xA2C244AB;
        table[1] = 0x5BE2B379;
        table[2] = 0x0D5B0588;
        break;
    case 1:
    case 4:
        table[0] = 0xA4B6BB62;
        table[1] = 0x5BE2B379;
        table[2] = 0x0D5B0588;
        break;
    case 2:
    case 6:
        table[0] = 0xA2C244AB;
        table[1] = 0xA4B6BB62;
        table[2] = 0x0D5B0588;
        break;
    case 3:
    case 7:
        table[0] = 0xA2C244AB;
        table[1] = 0xA4B6BB62;
        table[2] = 0x5BE2B379;
        break;
    }
    int count;
    u32 entries[16];
    if (handle == (void*)0xE2C7F0EF) {
        count = fn_800B93DC(table, 3, entries, 0x10, 1);
    } else if (handle == (void*)0x3D23AB6C) {
        count = fn_800B93DC(table, 1, entries, 0x10, 1);
    }
    state->msgTarget = (int)entries[Rand() % count];
    state->targetId = -1;
    state->timer = lbl_804D99BC;
    ClearStateMachine(owner);
    if (((FollowHandleObj*)owner->unk5EC)->unkBC == -1) {
        SetStateMachineState(owner, 0x469ABA3B);
    } else {
        SetStateMachineState(owner, 0xF1C37C5C);
    }
    fn_80118084(owner->unk5EC, 0x315C41D, &state->msgTarget);
    fn_80118084(owner->unk5EC, 0x033FA508, (int*)&dist);
    return 1;
}

// State tick that picks a random follow target for the current team and then
// re-enters the behaviour's chase state.
int FollowPickRandomTargetState(void* p) {
    return FollowPickRandomTarget(p);
}

// State tick that validates the target picked on the previous tick: it keeps the
// pending target if there is one, otherwise asks the follow behaviour for a new
// one and falls back to a random target search when that fails.  When a target
// exists it re-enters the chase state and records it in the behaviour table.
int FollowCheckTargetState(void* p) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    FollowState* state = owner->state;
    state->stateKind = 1;
    state->stateId = 0;
    if (state->targetId == -1) {
        state->msgTarget = FollowSelectTarget(owner, state);
    } else {
        state->msgTarget = state->targetId;
    }
    state->targetId = -1;
    if (state->msgTarget == -1) {
        FollowPickRandomTarget(owner);
    } else {
        ClearStateMachine(owner);
        SetStateMachineState(owner, 0xDF1271D9);
        fn_80118084(owner->unk5EC, 0x315C41D, &state->msgTarget);
    }
    return 1;
}

extern float lbl_804D99C0;
extern float lbl_804D99C4;
extern float lbl_804D99C8;
extern float lbl_804D99CC;
extern float lbl_804D99D0;
extern float lbl_804D99D4;
extern float lbl_804D99D8;
extern float lbl_804D99E0;
extern u32 lbl_8034A0C8[4];

int FollowScanRandomTarget(void* owner);
int FollowScanNearestTarget(void* owner);
extern "C" int fn_800BBF9C(void* owner, int id);
extern "C" int fn_800F8AF4(void* obj);
int MemFlagIsSet(void* mem, int flag);
float OwnerValue(Owner* owner, void* obj);
int FollowScanBestTarget(void* owner);

// State tick of the follow behaviour: it refreshes the target by picking one of
// the three target-acquisition states according to the behaviour's distance
// mode, and when nothing was found it re-enters the behaviour with a randomly
// chosen target.
int FollowTargetValidateTick(void* p) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    FollowState* state = owner->state;
    state->stateKind = 0;
    state->stateId = 0;
    FollowHandleObj* handle = (FollowHandleObj*)owner->unk5EC;
    int kind = owner->unk5D0;
    int result;
    if (kind == 2) {
        if (owner->unk5B8 <= lbl_804D99D0 || owner->unk5B4 <= lbl_804D99C8 ||
            owner->unk904 <= lbl_804D99D4 * handle->unk0C) {
            result = FollowScanNearestTarget(owner);
        } else {
            result = FollowScanBestTarget(owner);
        }
    } else if (kind == 0) {
        result = FollowScanRandomTarget(owner);
    } else if (kind >= 2 && kind < 4) {
        if (owner->unk904 < lbl_804D99CC * handle->unk0C) {
            result = FollowScanNearestTarget(owner);
        } else {
            result = FollowScanBestTarget(owner);
        }
    } else {
        if (owner->unk5B8 < lbl_804D99C0) {
            result = FollowScanRandomTarget(owner);
        } else if (owner->unk5B8 <= lbl_804D99C4 ||
                   owner->unk5B4 <= lbl_804D99C8 ||
                   owner->unk904 <= lbl_804D99CC * handle->unk0C) {
            result = FollowScanNearestTarget(owner);
        } else {
            result = FollowScanBestTarget(owner);
        }
    }
    state->msgTarget = result;
    if (state->msgTarget == -1) {
        FollowPickRandomTarget(owner);
    } else {
        float dist = lbl_804D99D8;
        ClearStateMachine(owner);
        SetStateMachineState(owner, 0x469ABA3B);
        fn_80118084(owner->unk5EC, 0x315C41D, &state->msgTarget);
        fn_80118084(owner->unk5EC, 0x033FA508, (int*)&dist);
    }
    return 1;
}

// State tick that scans the behaviour's candidate objects, keeps the closest
// reachable one for each candidate class and returns the best target id (-1
// when none was reachable).
int FollowScanBestTarget(void* p) {
    u32 table[4] = {lbl_8034A0C8[0], lbl_8034A0C8[1], lbl_8034A0C8[2],
                    lbl_8034A0C8[3]};
    u32 entries[32];
    float bestAny = lbl_804D99E0;
    float bestTypeA = lbl_804D99E0;
    float bestTypeB = lbl_804D99E0;
    int idAny = -1;
    int idTypeA = -1;
    int idTypeB = -1;
    int count = fn_800B93DC(table, 4, entries, 0x20, 1);
    if (count == 0) {
        return -1;
    }
    for (int i = 0; i < count; i++) {
        int id = (int)entries[i];
        NameQuery first;
        first.hash = 0x33032FA9;
        first.unk08 = 0;
        first.name.mData = 0;
        SendMsg(id, 0x41E, &first);
        void* handle = first.name.mData;
        NameQuery second;
        second.hash = 0x7C18FE9E;
        second.unk08 = 0;
        second.name.mData = 0;
        SendMsg(id, 0x41E, &second);
        void* target = second.name.mData;
        if (handle != (void*)1 || target == (void*)-1 ||
            ((u32)target >> 24) != 0x13) {
            continue;
        }
        MsgData msg;
        SendMsg((int)target, 0x400, &msg);
        u8* obj = (u8*)msg.obj;
        if (obj == 0 || msg.unk04[2] == 0) {
            continue;
        }
        if (MemFlagIsSet(obj + 0x270, 0xB) != 0) {
            continue;
        }
        if (*(s32*)(obj + 0x2C4) == -1 || fn_800BBF9C(p, (int)obj) == 0) {
            continue;
        }
        float dist = OwnerValue((Owner*)p, obj);
        NameQuery third;
        third.hash = 0xFF3C0C68;
        third.unk08 = 0;
        third.name.mData = 0;
        SendMsg(*(u32*)(obj + 0x38), 0x41E, &third);
        if (third.name.mData != 0) {
            continue;
        }
        int kind = fn_800F8AF4(*(void**)(obj + 0x8F0));
        if (kind == 0x020C054A) {
            if (dist < bestTypeA) {
                idTypeA = id;
                bestTypeA = dist;
            }
        } else if (kind < 0x020C054A && kind == -0x7477138) {
            if (dist < bestTypeB) {
                idTypeB = id;
                bestTypeB = dist;
            }
        } else if (dist < bestAny) {
            idAny = id;
            bestAny = dist;
        }
    }
    if (idTypeB == -1) {
        idTypeB = idAny;
        if (idTypeA != -1) {
            idTypeB = idTypeA;
        }
    }
    return idTypeB;
}

extern u32 lbl_8034A0B8[4];

// Candidate scan used by the follow behaviour's target-refresh state: it walks
// the behaviour's target list starting at a random entry and returns the last
// candidate that passes the reachability checks (-1 when none does).
int FollowScanRandomTarget(void* p) {
    u32 table[4] = {lbl_8034A0B8[0], lbl_8034A0B8[1], lbl_8034A0B8[2],
                    lbl_8034A0B8[3]};
    u32 entries[32];
    int best = -1;
    int count = fn_800B93DC(table, 4, entries, 0x20, 1);
    if (count == 0) {
        return -1;
    }
    int start = Rand() % count;
    int end = start + count;
    for (int i = start; i < end; i++) {
        int id = (int)entries[i % count];
        NameQuery first;
        first.hash = 0x33032FA9;
        first.unk08 = 0;
        first.name.mData = 0;
        SendMsg(id, 0x41E, &first);
        void* handle = first.name.mData;
        NameQuery second;
        second.hash = 0x7C18FE9E;
        second.unk08 = 0;
        second.name.mData = 0;
        SendMsg(id, 0x41E, &second);
        void* target = second.name.mData;
        if (handle != (void*)1 || target == (void*)-1 ||
            ((u32)target >> 24) != 0x13) {
            continue;
        }
        MsgData msg;
        SendMsg((int)target, 0x400, &msg);
        u8* obj = (u8*)msg.obj;
        if (obj == 0 || msg.unk04[2] == 0) {
            continue;
        }
        if (MemFlagIsSet(obj + 0x270, 0xB) != 0) {
            continue;
        }
        if (*(s32*)(obj + 0x2C4) == -1 || fn_800BBF9C(p, (int)obj) == 0) {
            continue;
        }
        NameQuery third;
        third.hash = 0xFF3C0C68;
        third.unk08 = 0;
        third.name.mData = 0;
        SendMsg(*(u32*)(obj + 0x38), 0x41E, &third);
        if (third.name.mData != 0) {
            continue;
        }
        best = id;
    }
    return best;
}

extern u32 lbl_8034A0A8[4];

// Candidate scan used when the follow behaviour wants the nearest reachable
// target: it walks the behaviour's target list and keeps the candidate with the
// smallest measured distance (-1 when none is reachable).
int FollowScanNearestTarget(void* p) {
    u32 table[4] = {lbl_8034A0A8[0], lbl_8034A0A8[1], lbl_8034A0A8[2],
                    lbl_8034A0A8[3]};
    u32 entries[32];
    float bestDist = lbl_804D99E0;
    int best = -1;
    int count = fn_800B93DC(table, 4, entries, 0x20, 1);
    if (count == 0) {
        return -1;
    }
    for (int i = 0; i < count; i++) {
        int id = (int)entries[i];
        NameQuery first;
        first.hash = 0x33032FA9;
        first.unk08 = 0;
        first.name.mData = 0;
        SendMsg(id, 0x41E, &first);
        void* handle = first.name.mData;
        NameQuery second;
        second.hash = 0x7C18FE9E;
        second.unk08 = 0;
        second.name.mData = 0;
        SendMsg(id, 0x41E, &second);
        void* target = second.name.mData;
        if (handle != (void*)1 || target == (void*)-1 ||
            ((u32)target >> 24) != 0x13) {
            continue;
        }
        MsgData msg;
        SendMsg((int)target, 0x400, &msg);
        u8* obj = (u8*)msg.obj;
        if (obj == 0 || msg.unk04[2] == 0) {
            continue;
        }
        if (MemFlagIsSet(obj + 0x270, 0xB) != 0) {
            continue;
        }
        if (*(s32*)(obj + 0x2C4) == -1 || fn_800BBF9C(p, (int)obj) == 0) {
            continue;
        }
        NameQuery third;
        third.hash = 0xFF3C0C68;
        third.unk08 = 0;
        third.name.mData = 0;
        SendMsg(*(u32*)(obj + 0x38), 0x41E, &third);
        if (third.name.mData != 0) {
            continue;
        }
        float dist = OwnerValue((Owner*)p, msg.obj);
        if (dist < bestDist) {
            bestDist = dist;
            best = id;
        }
    }
    return best;
}

// Payload for the 0x41E lookup message whose trailing String slot carries a
// float result (the handler writes the distance into the object's first word).
struct FloatQuery {
    u32 unk00;     // 0x00
    u32 hash;      // 0x04
    u32 unk08;     // 0x08
    String value;  // 0x0C
};

// Extended 0x20-byte message payload; the 0x400/0x401 handlers report the
// candidate's distance and rank through the two floats at 0x10 and 0x14.
struct MsgDataEx {
    void* obj;      // 0x00
    u32 unk04[3];   // 0x04 (unk04[2] at 0x0C)
    float unk10;    // 0x10
    float unk14;    // 0x14
    u32 unk18;      // 0x18
    u32 unk1C;      // 0x1C
};

extern float lbl_804D99DC;
extern float lbl_804D99E4;
extern float lbl_804D99E8;
extern float lbl_804D99EC;
extern float lbl_804D99F0;
extern float lbl_804D99F4;
extern "C" int fn_800FD3EC(void);
extern "C" int fn_800FD6EC(int index);
float StatGet(int stat, int index, int player);

// Selects the follow behaviour's next target.  It first reads the distance the
// world associates with the owner (0x41E message), then measures how far the
// players' per-player statistics spread apart and derives a search mode from
// the owner's configured thresholds.  The mode picks a target from the owner's
// four-entry candidate table, reuses the owner's pending target, or scans the
// global actor list (message 0x400/0x401) for the nearest / best candidate.
int FollowSelectTarget(void* p, void* s) {
    Owner* owner = (Owner*)p;
    FollowState* state = (FollowState*)s;

    FloatQuery query;
    query.hash = 0xD1DA12E7;
    query.unk08 = 1;
    *(float*)&query.value = lbl_804D99BC;
    SendMsg(0x4B000000, 0x41E, &query);
    float range = *(float*)&query.value;

    int player = HandleToPlayer(OwnerId(owner));
    int base = (int)StatGet(0xE2D6A9C0, 0, player);
    int spread = 0;
    for (int i = 0; i < fn_800FD3EC(); i++) {
        if (i != player) {
            if ((int)StatGet(0xE2D6A9C0, 0, i) - base > spread) {
                spread = (int)StatGet(0xE2D6A9C0, 0, i) - base;
            }
        }
    }

    int mode;
    if (state->stateId == (int)0xACBFC0A8) {
        if (state->stateKind == 1) {
            return -1;
        }
        mode = 4;
    } else if (owner->unk5B4 < lbl_804D99E4) {
        mode = 3;
    } else if (owner->unk5B4 > lbl_804D99E8) {
        if (range < lbl_804D99EC || spread > 3) {
            mode = 2;
        } else {
            mode = 1;
        }
    } else {
        float scale = *(float*)(owner->unk5EC + 0x64);
        if (owner->unk904 < lbl_804D99D4 * scale) {
            mode = 3;
        } else {
            mode = 1;
        }
    }

    // The owner keeps a four-entry table of candidate handles; the first live
    // entry whose 0x400 event payload reports a usable object is returned.
    int index = 0;
    int remaining = 4;
    do {
        int* entry = &owner->unk1E4[index * 3];
        if (entry[0] == -1) {
            index++;
            remaining--;
            continue;
        }
        int id = entry[0];
        int handle;
        if (((u32)id >> 24) == 1) {
            NameQuery nameQuery;
            nameQuery.hash = 0x1C1A4CC9;
            nameQuery.unk08 = 0;
            nameQuery.name.mData = 0;
            SendMsg(id, 0x41E, &nameQuery);
            handle = (int)nameQuery.name.mData;
        } else {
            NameQuery nameQuery;
            nameQuery.hash = 0x6C47D293;
            nameQuery.unk08 = 0;
            nameQuery.name.mData = 0;
            SendMsg(id, 0x41E, &nameQuery);
            handle = (int)nameQuery.name.mData;
        }
        if (handle == -1 || handle == 0) {
            MsgDataEx msg;
            if (SendMsg(id, 0x400, &msg) != 0 && msg.obj != 0 &&
                msg.unk04[2] == 1) {
                return id;
            }
        }
        break;
    } while (remaining != 0);

    int target = state->targetId;
    if (target != -1) {
        return target;
    }

    float bestTime = lbl_804D99DC;
    float bestDist = lbl_804D99F0;
    int bestScore = -100000;
    int found[8];
    int foundCount = 0;
    int picked = -1;
    for (int i = 0; i < fn_800FD3EC(); i++) {
        int id = fn_800FD6EC(i);
        if (fn_80237F68(owner, id) == 0) {
            continue;
        }
        switch (mode) {
        case 0:
            return id;
        case 2:
            if ((float)bestScore < StatGet(0xE2D6A9C0, 0, i)) {
                picked = id;
                bestScore = (int)StatGet(0xE2D6A9C0, 0, i);
            }
            break;
        case 3: {
            MsgDataEx msg;
            SendMsg(id, 0x401, &msg);
            if (msg.unk14 > lbl_804D99BC && msg.unk10 < bestTime) {
                bestTime = msg.unk10;
                picked = id;
            }
            break;
        }
        case 4:
            found[foundCount++] = id;
            break;
        default: {
            MsgDataEx msg;
            SendMsg(id, 0x400, &msg);
            float dist = OwnerValue(owner, msg.obj);
            if (dist < bestDist) {
                bestDist = dist;
                picked = id;
            }
            break;
        }
        }
    }
    if (mode == 4 && foundCount > 0) {
        picked = found[Rand() % foundCount];
    }
    return picked;
}

// Event data handed to the proximity checks: the object being considered, the
// id the owner's event sink is keyed with, and the object's world position.
struct ProximityMsg {
    u32 unk00;      // 0x00
    u32 unk04;      // 0x04
    void* obj;      // 0x08 - object whose engine type selects the response
    u32 unk0C;      // 0x0C
    u32 unk10;      // 0x10
    int targetId;   // 0x14 - passed to the owner's event sink
    u32 unk18;      // 0x18
    u32 unk1C;      // 0x1C
    float x;        // 0x20
    float y;        // 0x24
    float z;        // 0x28
};

// Payload of the 0xC0A callback issued to the owner: the source object, the
// threshold value and the handler's verdict word.
struct HitMsg {
    u32 obj;      // 0x00
    float value;  // 0x04
    int result;   // 0x08
};

int Fn800F8A98(unsigned long obj);
void Fn800F840C(int a, int type, int value);

// Proximity test used by the follow behaviour's state machine.  It rejects
// objects outside the owner's engagement radius, classifies the message's
// object through the engine's type query and then either reports the distance
// to the owner through the message handler or records the event on the owner's
// event sink, depending on the reported type.
int FollowProximityMessageCheck(Owner* owner, ProximityMsg* msg) {
    if (owner->unk904 <= lbl_804D99BC) {
        return 0;
    }
    float dy = (float)owner->y - msg->y;
    float dx = (float)owner->x - msg->x;
    float dz = (float)owner->z - msg->z;
    float distSq = dy * dy + dx * dx + dz * dz;
    if (distSq > lbl_804D99F4 || distSq < -lbl_804D99F4) {
        return 0;
    }
    if (msg->obj == 0) {
        return 0;
    }
    int type = Fn800F8A98((unsigned long)msg->obj);
    if (type == 0) {
        return 0;
    }
    switch (type) {
    case 0x1F5A5ED3:
        if (fn_80116E08(owner) != 0) {
            return 0;
        }
        Fn800F840C(owner->unk8EC, type, msg->targetId);
        return 1;
    case 0x20A5AA68:
    case 0x32819B90:
    case 0x3BFF94C2:
    case 0x8B267329:
    case 0xDC95C315:
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        Fn800F840C(owner->unk8EC, type, msg->targetId);
        return 1;
    case 0xA7D6DA15: {
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        HitMsg hit;
        hit.obj = msg->unk00;
        hit.value = lbl_804D99A4;
        owner->unk30(0xC0A, &hit);
        return hit.result != 1;
    }
    case 0xECA3D57B: {
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        HitMsg hit;
        hit.obj = msg->unk00;
        hit.value = lbl_804D99E4;
        owner->unk30(0xC0A, &hit);
        return hit.result != 1;
    }
    case 0xECA3D5B9: {
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        HitMsg hit;
        hit.obj = msg->unk00;
        hit.value = lbl_804D99AC;
        owner->unk30(0xC0A, &hit);
        return hit.result != 1;
    }
    case 0xECA3D5F7: {
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        HitMsg hit;
        hit.obj = msg->unk00;
        hit.value = lbl_804D99D4;
        owner->unk30(0xC0A, &hit);
        return hit.result != 1;
    }
    default:
        return 0;
    }
}

// Message payload of the extra/trail behaviour tick (class ExtraMsg in retail;
// field names and offsets are guesses from the code that uses them).
struct ExtraMsg {
    u32 unk00;       // 0x00
    u32 id;          // 0x04 - state-transition message id
    int handle;      // 0x08 - owner handle used as a message target
    int value;       // 0x0C - amount applied by the tick (read as float too)
    u8 unk10[0x14 - 0x10];
    u32 stateId;     // 0x14 - current state name hash
    u8 unk18[0x44 - 0x18];
    int unk44;       // 0x44 - bone / animation id
    u8 unk48[0x54 - 0x48];
    int unk54;       // 0x54 - running flag
};

struct ExtraHolder;

extern float lbl_804D99F8;

int ModelBoneName(int model, int id);
unsigned long AssetFind(unsigned long, unsigned long);
int HasCondition(void*, int);
void ConditionSetBit(void*, int, int);
void ExtraTrailAttach(ExtraHolder*);
void ExtraTrailApply(ExtraHolder*, ExtraMsg*);
extern "C" int fn_800F7C64(int, unsigned long);
extern "C" int fn_800FC3B8(int);
extern "C" void fn_800FC764(int, int);
extern "C" void fn_802C24E4(void*, void*);
extern "C" void fn_80090390(int, int);
extern "C" void fn_8013F4B8(void*, int);
extern "C" void fn_800B20B8(void*, int);
extern "C" void fn_801D55CC(void);

// Payload of the 0xC14 event issued when the behaviour's distance runs out.
struct ExtraEventData {
    u32 unk00;    // 0x00
    int handle;   // 0x04 - message handle (same as ExtraMsg::handle)
    int ownerId;  // 0x08 - owner id of the behaviour
    u32 hash;     // 0x0C - event id
};

// Extra behaviour tick: filters the incoming message, decays the behaviour's
// remaining distance by the message value and drives the extra trail effect.
// Returns whether the behaviour should keep running.  Names are guesses.
int ExtraApplyDamageTick(void* p, ExtraMsg* msg) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    msg->unk54 = 1;
    float value = lbl_804D99BC;
    int handle;
    {
        NameQuery query;
        query.hash = 0x0E83B88B;
        query.unk08 = 0;
        query.name.mData = 0;
        SendMsg(0x4B000000, 0x41E, &query);
        handle = (int)query.name.mData;
    }
    if (handle == 0 && msg->handle != OwnerId(owner)) {
        int team = PlayerTeam(HandleToPlayer(msg->handle));
        if (PlayerTeam(HandleToPlayer(OwnerId(owner))) == team) {
            return 1;
        }
    }
    int bone = (int)AssetFind(0xF1754BF2UL, 0x21C5E0UL);
    if (ModelBoneName(owner->unkCC, msg->unk44) == bone) {
        *(float*)&msg->value = lbl_804D99F8 * *(float*)&msg->value;
    }
    if (fn_800F7C64(owner->unk8EC, 0x32819B90UL) != 0 ||
        fn_800FC3B8(0x13) != 0) {
        *(float*)&msg->value = lbl_804D99D4 * *(float*)&msg->value;
    }
    switch (msg->stateId) {
    case 0x2EFC66CA:
        if (fn_80116E08(owner) != 0) {
            fn_80116E30(owner);
        }
        break;
    case 0xEA32D667:
    case 0xB9FC42F5:
    case 0x8FC151EE:
    case 0x0020EE36:
        if (fn_80116E08(owner) != 0) {
            value = *(float*)&msg->value;
        }
        break;
    case 0x403548D0:
        if (fn_80116E08(owner) == 0) {
            value = *(float*)&msg->value * lbl_804D99D4;
        }
        break;
    case 0x75715902:
        if (fn_80116E08(owner) != 0) {
            fn_802C24E4((char*)msg + 0x18, (char*)msg + 0x24);
            value = *(float*)&msg->value;
        }
        break;
    case 0x672F35C2:
        if (owner->unk904 <= lbl_804D99BC) {
            fn_800FC764(OwnerId(owner), 0);
            return 1;
        }
        break;
    default:
        value = *(float*)&msg->value;
        break;
    }
    if (owner->unk904 > lbl_804D99BC) {
        if (value > lbl_804D99BC) {
            owner->unk904 = owner->unk904 - value;
            owner->unk214 = msg->unk44;
            if (owner->unk904 > lbl_804D99BC) {
                ConditionSetBit(&owner->followTarget, 4, 1);
            } else {
                if (HasCondition(&owner->followTarget, 0xC) != 0) {
                    ConditionSetBit(&owner->followTarget, 0xC, 0);
                    fn_80090390(owner->unkD4, 0);
                }
                ConditionSetBit(&owner->followTarget, 9, 1);
                int target;
                {
                    NameQuery query;
                    query.hash = 0x2D88B734;
                    query.unk08 = 0;
                    query.name.mData = 0;
                    SendMsg(0x4B000000, 0x41E, &query);
                    target = (int)query.name.mData;
                }
                if (target != 0) {
                    fn_8013F4B8(owner, owner->unk38);
                }
                ExtraEventData data;
                data.handle = msg->handle;
                data.ownerId = OwnerId(owner);
                data.hash = 0x001FF464;
                if (msg->handle != -1) {
                    SendMsg(msg->handle, 0xC14, &data);
                }
                msg->unk54 = 0;
                if (owner->unk5F0 != 0) {
                    ExtraTrailAttach((ExtraHolder*)owner->unk5F0);
                    if (owner->unk5F4 != 0) {
                        fn_801D55CC();
                        owner->unk5F4 = 0;
                    }
                }
            }
            if (fn_80116E08(owner) != 0) {
                *(float*)((char*)owner->unk5EC + 0x60) = owner->unk904;
            } else {
                *(float*)((char*)owner->unk5EC + 0x08) = owner->unk904;
            }
            if (msg->stateId != 0x6CA5F172) {
                fn_800B20B8(owner, msg->handle);
                if (owner->unk5F0 != 0) {
                    ExtraTrailApply((ExtraHolder*)owner->unk5F0, msg);
                }
            }
        }
        return 1;
    }
    if (owner->unk5F0 != 0) {
        ExtraTrailApply((ExtraHolder*)owner->unk5F0, msg);
    }
    return 1;
}

// Behaviour state-machine record: three callbacks followed by the state name.
// What each callback does is a guess derived from how the unit calls them.
struct FollowStateRecord {
    void (*enter)(void*);   // 0x00
    int (*check)(void*);    // 0x04
    int (*update)(void*);   // 0x08
    const char* name;       // 0x0C
};
extern FollowStateRecord lbl_803BE260[];
extern char lbl_8034A0D8[];  // string pool
struct Actor;
Actor* GetPlayer();
int PlayerInHost(Actor* actor);
extern "C" void fn_8028C188(void*, u32, u32);
extern "C" String fn_800225C8(String*, String*);

// State-transition handler of the extra behaviour: turns the message id into
// the next state hash and returns whether the state handled the message.  Names
// and ids are guesses derived from the retail code.
int ExtraMsgStateChange(void* p, ExtraMsg* msg) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    FollowState* state = owner->state;
    switch (msg->id) {
    case 0x0C343772:
        msg->value = owner->unk5D0;
        return 1;
    case 0xAD8576FA: {
        const char* stateName = lbl_803BE260[state->stateKind].name;
        String base(stateName);
        String suffix(lbl_8034A0D8 + 0x48);
        String full = fn_800225C8(&base, &suffix);
        fn_8028C188((char*)msg + 0x0C, full.mHash, full.mUnk08);
        return 0;
    }
    case 0x9BD4EF06:
        msg->value = PlayerTeam(HandleToPlayer(OwnerId(owner)));
        return 1;
    case 0x8132F1B1:
        msg->value = 1;
        return 1;
    case 0xDFFA4206:
        if (fn_80116E08(owner) == 0) {
            if (PlayerInHost(GetPlayer()) != 0) {
                msg->value = 0;
            } else {
                msg->value = 1;
            }
        }
        return 1;
    case 0x5A32795A:
        msg->value = 0;
        return 1;
    case 0x365F2878:
        msg->value = 1;
        return 1;
    case 0x5ED5BF20: {
        if (owner->unk904 <= lbl_804D99BC) {
            msg->value = -1;
            return 1;
        }
        int team = PlayerTeam(HandleToPlayer(OwnerId(owner)));
        switch (team) {
        case 0:
            if (fn_80116E08(owner) != 0) {
                msg->value = 0xA78FF993;
            } else {
                msg->value = 0x010843B8;
            }
            break;
        case 1:
            if (fn_80116E08(owner) != 0) {
                msg->value = 0x8EB9A32A;
            } else {
                msg->value = 0xFF13CD01;
            }
            break;
        case 2:
            if (fn_80116E08(owner) != 0) {
                msg->value = 0xEF89229C;
            } else {
                msg->value = 0xB8343BCF;
            }
            break;
        case 3:
            if (fn_80116E08(owner) != 0) {
                msg->value = 0x05C6D2AD;
            } else {
                msg->value = 0x69AC8DDE;
            }
            break;
        case 4:
            if (fn_80116E08(owner) != 0) {
                msg->value = 0xC062204F;
            } else {
                msg->value = 0x027AF47C;
            }
            break;
        case 5:
            if (fn_80116E08(owner) != 0) {
                msg->value = 0x9F01CD67;
            } else {
                msg->value = 0xF7D0EA64;
            }
            break;
        case 6:
            if (fn_80116E08(owner) != 0) {
                msg->value = 0x6D4CC9F5;
            } else {
                msg->value = 0xF2E57F96;
            }
            break;
        case 7:
            if (fn_80116E08(owner) != 0) {
                msg->value = 0x59F980AF;
            } else {
                msg->value = 0x9BCFA01C;
            }
            break;
        default:
            msg->value = 0x9BCFA01C;
            break;
        }
        return 1;
    }
    default:
        return 0;
    }
}

// Per-owner marker state reached through Owner::state (0x130).  The marker
// keeps a mode word, an RGBA tint and a "ready" word the engine fills in.
struct FollowMarkerState {
    u8 unk00[0x10];
    s32 mode;     // 0x10
    u8 color[4];  // 0x14 - RGBA tint
    s32 ready;    // 0x18
};

struct Actor;
Actor* GetPlayer();
int PlayerInHost(Actor* actor);
int HasCondition(void* block, int id);
void StatRank(int stat, int index, int* out, int arg4, int arg5);
void ItemColorDrawQuad(int drawId, int pos, int width, int height, float* color);
void ItemColorDrawQuadRange(int drawId, int pos, int width, int height,
                            float* color);

extern "C" int fn_80158AFC(int ownerId, void* state);
extern "C" void fn_80026B50(void* block, int a, int b);
extern "C" int fn_801409EC(int index);
extern "C" float* fn_800FD424(int player);

extern float lbl_804D9990;
extern double lbl_804D9998;

// Per-frame update of the follow marker: tints the marker for the local player
// (opaque white for remote players), resolves the two marker names through the
// 0x41E lookup message and draws the small marker quad, plus the enlarged quad
// when the owner's rank matches the target's.
int FollowMarkerUpdate(Owner* owner, FollowMarkerState* state, int flag) {
    if (owner == 0) {
        return 0;
    }
    if (flag == 0) {
        if (fn_80158AFC(OwnerId(owner), state) != 0) {
            return 1;
        }
    }
    if (fn_80116E08(owner) == 0) {
        if (PlayerInHost(GetPlayer()) != 0) {
            fn_80026B50(&state->mode, 0x80, 1);
            if (state->ready == 0) {
                return 1;
            }
            state->color[0] = 0;
            state->color[1] = 0;
            state->color[2] = 0;
            state->color[3] = 0;
            state->mode = 1;
        } else {
            state->color[0] = 0xFF;
            state->color[1] = 0xFF;
            state->color[2] = 0xFF;
            state->color[3] = 0xFF;
            state->mode = 1;
        }
    }
    if (fn_80116E08(owner) == 0 && PlayerInHost(GetPlayer()) != 0) {
        return 0;
    }
    if (HasCondition(&owner->followTarget, 10) != 0) {
        return 0;
    }

    int player = HandleToPlayer(OwnerId(owner));
    float* color = fn_800FD424(PlayerTeam(player));

    Vector3f pos;
    pos.x = (float)owner->x;
    pos.y = (float)owner->y;
    pos.z = (float)owner->z;
    pos.z += lbl_804D9990;

    void* found;
    {
        NameQuery query;
        query.name.mData = 0;
        query.hash = 0x896F8C7F;
        query.unk08 = 0;
        SendMsg(0x4B000000, 0x41E, &query);
        found = query.name.mData;
    }
    if (found == 0) {
        int drawId = fn_801409EC(5);
        ItemColorDrawQuad(drawId, (int)&pos, 0x60, 0x60, color);
    }

    void* target;
    {
        NameQuery query;
        query.name.mData = 0;
        query.hash = 0xBDEDC248;
        query.unk08 = 0;
        SendMsg(0x4B000000, 0x41E, &query);
        target = query.name.mData;
    }
    if (target != 0) {
        int rank;
        StatRank(0xE2D6A9C0, 0, &rank, 1, 1);
        if (StatGet(0xE2D6A9C0, 0, player) == StatGet(0xE2D6A9C0, 0, rank)) {
            int drawId = fn_801409EC(2);
            ItemColorDrawQuadRange(drawId, (int)&pos, 0xC0, 0xC0, color);
        }
    }
    return 0;
}

// Writes a marker/state word into the owner's behaviour state and reports that
// the value was stored.
int FollowMarkerSetStateId(int value, Owner* owner) {
    if (owner == 0) {
        return 0;
    }
    owner->state->stateId = value;
    return 1;
}

// Message sink reached through Owner::unk5EC.  +0xBC holds the owner's engine
// handle (-1 when unassigned), +0xDC a message filter and +0x1E8 a zone block
// updated from the owner position.
struct FollowSink {
    int (*setStateId)(int value, Owner* owner);  // 0x00 - behaviour entry callback
    Owner* owner;                                // 0x04 - owner handed to it
    u8 unk008[0xBC - 0x08];
    s32 handle;   // 0xBC - engine handle, -1 when unassigned
    u8 unk0C0[0xDC - 0xC0];
    int (*filter)(int msg, void* data, FollowSink* self);  // 0xDC
    u8 unk0E0[0x1E8 - 0xE0];
    u8 zoneBlock[0x40];  // 0x1E8 - zone block handed to ZoneUpdate
};

// Payload of the follow behaviour's messages: the owning actor followed by the
// query/command words.  Field meanings are guesses from the retail code.
struct FollowMsg {
    Owner* owner;  // 0x00
    u32 id;        // 0x04 - message/command id
    u32 obj;       // 0x08 - object word
    int value;     // 0x0C - main value (read as a float by other handlers)
    float unk10;   // 0x10
    float unk14;   // 0x14
    int unk18;     // 0x18
    int unk1C;     // 0x1C
};

// Payload of the 0xC14 name query.
struct FollowQuery {
    u32 unk00;    // 0x00 - left unset
    int id;       // 0x04
    int ownerId;  // 0x08
    u32 name;     // 0x0C - lookup hash
};

struct IVec3 {
    s32 x;  // 0x00
    s32 y;  // 0x04
    s32 z;  // 0x08
};

extern float lbl_804D99FC;

extern "C" int fn_80019F88(void* msg, void* owner);
extern "C" void fn_800CD5D8(void* block, int value);
extern "C" int fn_80116D40(void* owner, void* msg);
extern "C" int fn_800A3210(void* owner);
extern "C" int fn_800F81EC(int sink);
extern "C" void fn_800F8130(int sink, int index);
extern "C" int fn_80162658(void* owner, void* block);
extern "C" void fn_800B42E0(float scale, void* block, int id, int a, int b);
extern "C" int fn_800BAF40(void* owner);
extern "C" int U80116B28RampValue(void* owner, void* msg);

void ZoneUpdate(void* zone, IVec3* pos);

// Message handler of the follow behaviour.  Every message is first offered to
// the owner's sink filter; the handler then implements the behaviour protocol
// (marker updates, target queries, state changes).  Message ids and payload
// fields are guesses derived from the retail code.
int FollowBehaviourMessage(int msg, void* data) {
    FollowMsg* m = (FollowMsg*)data;
    Owner* owner = m->owner;
    FollowSink* sink = (FollowSink*)owner->unk5EC;
    if (sink->filter != 0 && sink->filter(msg, data, sink) != 0) {
        return 1;
    }
    switch (msg) {
    case 0:
        return FollowMarkerUpdate(owner, (FollowMarkerState*)m, 0);
    case 10:
        return fn_80116E08(owner) == 0;
    case 0x400:
        if (owner == 0 || m == 0) {
            return 0;
        }
        m->id = OwnerId(owner);
        m->value = owner->unk904 > lbl_804D99BC;
        m->unk10 = lbl_804D99A4;
        m->unk14 = lbl_804D99A4;
        m->unk18 = (sink->handle == -1) ? 1 : 2;
        m->unk1C = 0;
        return 1;
    case 0x401:
        return fn_80116D40(owner, m);
    case 0x407:
        return ExtraApplyDamageTick(owner, (ExtraMsg*)m);
    case 0x40A:
        if (owner->unk904 <= lbl_804D99BC) {
            return 0;
        }
        if (m->obj != 0x3CA34BD6) {
            return 0;
        }
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        if (fn_800F7C64(owner->unk8EC, 0xDC95C315) > 0) {
            return 0;
        }
        if (fn_800FC3B8(0x14) != 0) {
            return 0;
        }
        m->value = 0x7F044060;
        return 1;
    case 0x41D:
        if (owner == 0) {
            return 0;
        }
        if (fn_80019F88(m, owner) != 0) {
            return 1;
        }
        if (m->id != 0x0C343772) {
            return 0;
        }
        fn_800CD5D8(owner->markerBlock, m->value);
        return 1;
    case 0x7300:
        return FollowMarkerUpdate(owner, (FollowMarkerState*)m, 1);
    case 0x1501:
        return FollowProximityMessageCheck(owner, (ProximityMsg*)m);
    case 0xC0A:
        return U80116B28RampValue(owner, m);
    case 0xC14:
        if (m->value == 0x274B01CB) {
            OwnerId(owner);
            int count = fn_800F81EC(owner->unk8EC);
            for (int i = 0; i < count; i++) {
                fn_800F8130(owner->unk8EC, i);
            }
            return 1;
        }
        if (m->value == 0x8140BB1F) {
            FollowQuery query;
            query.id = m->id;
            query.ownerId = OwnerId(owner);
            query.name = 0x7C3F4E3E;
            SendMsg(0x4B000000, 0xC14, &query);
            return 1;
        }
        if (m->value == 0x6C345908) {
            int saved = owner->unk5D0;
            int handle = sink->handle;
            if (handle != -1) {
                fn_80116E30(owner);
                fn_800FC764(handle, 0);
            }
            fn_800A3210(owner);
            owner->unk5D0 = saved;
            fn_800CD5D8(owner->markerBlock, saved);
            owner->unk8F8 = 1;
            return 1;
        }
        return 0;
    default:
        if (msg >= 0x41F && msg < 0xC0A) {
            if (fn_80117FF8(owner->unk5EC, m->id, &m->value) != 0) {
                return 1;
            }
            return ExtraMsgStateChange(owner, (ExtraMsg*)m);
        }
        return 0;
    }
}

void VFree(void* p);

// Resolves the follow behaviour's target: asks the selection helper for a
// candidate owner id, converts the candidate's position to float and rejects it
// when the position check or the line-of-sight distance check fails.  Returns
// the behaviour state index to use (1 when the candidate was accepted).
static inline int FollowResolveTarget(void* p) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    FollowState* state = owner->state;
    if (fn_80116E08(owner) == 0) {
        return 0;
    }
    int id = FollowSelectTarget(owner, state);
    if (id == -1) {
        return 2;
    }
    FollowObject* obj = OwnerGetInfo(id);
    Vector3f pos;
    pos.x = (float)obj->x;
    pos.y = (float)obj->y;
    pos.z = (float)obj->z;
    if (fn_800BC01C(owner, &pos) != 0 ||
        FollowLineCheckActorDist(owner, obj, lbl_804D99A4) != 0) {
        state->targetId = id;
        return 1;
    }
    return 2;
}

// Per-frame tick of the follow behaviour: on the first tick it resolves the
// behaviour state index, then it drives the effect trail, runs the state's
// update callback, re-resolves the target and re-enters a state when the check
// callback asks for it, and finally picks the follow view mode.
int FollowBehaviourTick(void* p) {
    if (p == 0) {
        return 0;
    }
    Owner* owner = (Owner*)p;
    FollowState* state = owner->state;
    if (owner->unk904 <= lbl_804D99BC) {
        fn_80162658(owner, owner->unk894);
        return 1;
    }
    if (state->stateKind == -1) {
        state->stateKind = FollowResolveTarget(owner);
        lbl_803BE260[state->stateKind].enter(owner);
    }
    if (((FollowSink*)owner->unk5EC)->handle == -1) {
        fn_800B42E0(lbl_804D99FC, owner->unk84, owner->unk38, 1, 1);
    } else {
        fn_800B42E0(lbl_804D99FC, owner->unk84, owner->unk38, 1, 2);
    }
    ZoneUpdate(((FollowSink*)owner->unk5EC)->zoneBlock, (IVec3*)&owner->x);
    int result = lbl_803BE260[state->stateKind].update(owner);
    fn_80162658(owner, owner->unk894);
    if (result != 0) {
        int kind = FollowResolveTarget(owner);
        if (lbl_803BE260[state->stateKind].check(owner) != 0) {
            lbl_803BE260[kind].enter(owner);
        }
    }
    if (fn_800BAF40(owner) != -1) {
        owner->unk288 = 3;
    } else {
        owner->unk288 = 0;
    }
    return 1;
}

// Releases the follow behaviour: notifies the owning object through the 0xC14
// event, clears the actor's state machine, frees the per-behaviour state block
// and drops the behaviour pointers.
void FollowBehaviourRelease(void* p) {
    if (p == 0) {
        return;
    }
    Owner* owner = (Owner*)p;
    ExtraEventData data;
    data.handle = 0;
    data.ownerId = owner->unk38;
    data.hash = 0xEB4A0875;
    SendMsg(0x53000000, 0xC14, &data);
    ClearStateMachine(owner);
    VFree(owner->state);
    owner->unk104 = 0;
    owner->state = 0;
}

// Zone list type reached through Owner::unk658; only ever used by pointer.
struct ZoneList;

extern unsigned long lbl_804D34D4;
extern float lbl_804D9A00;
void* VAlloc(int size);
void ConditionUpdateBit(void*, int, int);
void ZoneFlagSet(ZoneList* zone, int index, int flag);
extern "C" void fn_800CE75C();
extern "C" void fn_800CEE08();
extern "C" void fn_80237EF0();
extern "C" int fn_80117D80(void* owner);

// Initialises the follow behaviour on the owner passed by the state machine:
// registers the model event, allocates the behaviour state block, installs the
// tick/message callbacks and the behaviour's entry callback, primes the zone and
// condition blocks and notifies the world through the 0xC14 event.
// Field meanings and the 3 in the event's handle word are guesses.
void FollowBehaviourInit(void* p) {
    if (p == 0) {
        return;
    }
    Owner* owner = (Owner*)p;
    lbl_804D34D4 = AssetFind(0xF2E2BEB8UL, 0xEDA570C7UL);
    FollowState* state = (FollowState*)VAlloc(0x18);
    if (state == 0) {
        owner->state = 0;
        return;
    }
    owner->state = state;
    owner->unkFC = fn_800CE75C;
    owner->unk604 = fn_800CEE08;
    owner->unk904 = (float)owner->unkC4->unk30;
    owner->unk600 = fn_80237EF0;
    if (fn_80117D80(owner) == 0) {
        VFree(state);
        owner->state = 0;
        return;
    }
    FollowSink* sink = (FollowSink*)owner->unk5EC;
    sink->setStateId = FollowMarkerSetStateId;
    sink->owner = owner;
    state->stateKind = -1;
    ConditionSetBit(&owner->followTarget, 1, 0);
    ConditionUpdateBit(&owner->followTarget, 6, 0);
    ConditionUpdateBit(&owner->followTarget, 0, 0);
    ConditionUpdateBit(&owner->followTarget, 0x23, 1);
    state->unk14 = lbl_804D9A00;
    state->targetId = -1;
    state->timer = lbl_804D99BC;
    ExtraEventData data;
    data.handle = 3;
    data.ownerId = owner->unk38;
    data.hash = 0x2840341C;
    SendMsg(0x53000000, 0xC14, &data);
    ZoneFlagSet((ZoneList*)&owner->unk658, 1, 0);
}

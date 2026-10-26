// u_8011818C.cpp: item actor state machine (0x8011818C-0x8012071C): states, event handlers, drop setup.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>

#pragma auto_inline off

// State-machine table used by this unit. Entry layout (0x1C bytes):
//   +0x00 state id, +0x04 enter func, +0x08 leave func,
//   +0x0C/+0x14 pointers stored on the machine, +0x10/+0x18 unnamed.
typedef void (*StateFunc)(void* owner, void* machine);

struct StateEntry {
    unsigned long id;    // 0x00
    StateFunc enter;     // 0x04
    StateFunc leave;     // 0x08
    void* data0C;        // 0x0C
    unsigned long unk10; // 0x10
    void* data14;        // 0x14
    unsigned long unk18; // 0x18
};

struct StateMachine {
    unsigned long pad00[0xC8 / 4];
    unsigned long prevState; // 0x0C8
    unsigned long state;     // 0x0CC
    unsigned long unkD0;     // 0x0D0
    unsigned long unkD4;     // 0x0D4
    void* unkD8;             // 0x0D8
    void* unkDC;             // 0x0DC
};

struct StateOwner {
    unsigned long pad00[0x5EC / 4];
    StateMachine* machine; // 0x5EC
};

extern "C" {
extern StateEntry lbl_8039ED68[];
extern int lbl_804D2E08;
}

// Clears the owner's current state (runs the leave callback first).
int ClearStateMachine(void* arg)
{
    StateOwner* owner = (StateOwner*)arg;
    StateMachine* sm = owner->machine;
    unsigned long state;
    int idx;
    int i;

    if (sm == 0) {
        return -1;
    }
    state = sm->state;
    if (state != 0xFFFFFFFF) {
        idx = -1;
        for (i = 0; i < lbl_804D2E08; i++) {
            if (state == lbl_8039ED68[i].id) {
                idx = i;
                break;
            }
        }
        if (idx != -1) {
            lbl_8039ED68[idx].leave(arg, sm);
        }
    }
    sm->prevState = 0xFFFFFFFF;
    sm->state = 0xFFFFFFFF;
    sm->unkD0 = 0xFFFFFFFF;
    sm->unkD4 = 0xFFFFFFFF;
    return -1;
}

// Switches the owner to a new state, leaving the old one first.
// Returns the state the machine ends up in (-1 when the id is unknown).
int SetStateMachineState(void* arg, unsigned long newState)
{
    StateOwner* owner = (StateOwner*)arg;
    StateMachine* sm = owner->machine;
    unsigned long state;
    int idx;
    int i;

    if (sm == 0) {
        return -1;
    }
    state = sm->state;
    if (state != 0xFFFFFFFF) {
        idx = -1;
        for (i = 0; i < lbl_804D2E08; i++) {
            if (state == lbl_8039ED68[i].id) {
                idx = i;
                break;
            }
        }
        if (idx != -1) {
            lbl_8039ED68[idx].leave(arg, sm);
        }
        sm->prevState = sm->state;
    }
    idx = -1;
    for (i = 0; i < lbl_804D2E08; i++) {
        if (newState == lbl_8039ED68[i].id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        sm->state = 0xFFFFFFFF;
    } else {
        sm->state = newState;
        lbl_8039ED68[idx].enter(arg, sm);
        sm->unkD8 = lbl_8039ED68[idx].data0C;
        sm->unkDC = lbl_8039ED68[idx].data14;
    }
    return sm->state;
}

// Minimal view of an actor that exposes its owning player handle.
struct TargetActor {
    unsigned char pad00[0x38];
    int owner; // 0x38
};

int HandleToPlayer(int owner);
int PlayerTeam(int player);

// Returns 1 when the actor referenced by *slot must be skipped as a target:
// the slot is empty, or it holds a different actor on the same team.
// A slot holding the actor itself is not skipped (returns 0).
int IsSameTeamOtherActor(void* slotPtr, void* targetPtr)
{
    TargetActor** slot = (TargetActor**)slotPtr;
    TargetActor* target = (TargetActor*)targetPtr;

    if (*slot == 0) {
        return 1;
    }
    if (*slot != target) {
        int targetTeam = PlayerTeam(HandleToPlayer(target->owner));
        int slotTeam = PlayerTeam(HandleToPlayer((*slot)->owner));
        if (slotTeam == targetTeam) {
            return 1;
        }
    }
    return 0;
}

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();
    String(const char* s);
    ~String() {}
    virtual void Release(char* p);
};

// The receiver of a 0x41E name query writes its answer over the first word
// of the string it is handed.
struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    unsigned long hash;
    int unk8;
    QueryResult result;
};

// Payload of the 0x400 actor query.
struct Msg400 {
    void* actor;      // 0x00
    int ownerId;      // 0x04
    int unk08;        // 0x08
    int state;        // 0x0C
    float f10;        // 0x10
    float f14;        // 0x14
    int flags;        // 0x18
};

struct OwnerInfo {
    unsigned char pad00[0x174];
    unsigned long type; // 0x174
};

struct Owner {
    unsigned char pad00[0xC4];
    OwnerInfo* info; // 0xC4
};

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int fn_800B21EC(void* owner, int handle, int* out);
void* VAlloc(int size);
void VFree(void* p);

int OwnerId(Owner* owner);

// Asks the target's 0x41E handler for the value stored under a name hash.
static inline int QueryName(int target, unsigned long hash)
{
    QueryMsg q;

    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    SendMsg(target, 0x41E, &q);
    return *(int*)&q.result;
}

// Returns 1 when the handle is a valid attack target for the owner: alive,
// not on the owner's own team, and neither flagged as protected nor owned by
// a shielded actor.
int IsValidAttackTarget(Owner* self, int handle)
{
    Msg400 info;
    int type;
    int result;
    int targetTeam;

    if (handle == OwnerId(self)) {
        return 0;
    }
    if (SendMsg(handle, 0x400, &info) &&
        info.actor != 0 &&
        info.state == 1) {
        targetTeam = PlayerTeam(HandleToPlayer(handle));
        if (PlayerTeam(HandleToPlayer(OwnerId(self))) == targetTeam) {
            return 0;
        }
        if (fn_800B21EC(self, handle, &type) != 0 && type == 5) {
            return 0;
        }
        if (PlayerTeam(HandleToPlayer(handle)) == -1) {
            result = QueryName(handle, 0xDC13F8AB);
            if (result != 0) {
                return 0;
            }
            result = QueryName(handle, 0x6C47D293);
            if (result == -1) {
                result = QueryName(handle, 0x01E4326A);
            }
            if (result == -1) {
                return 0;
            }
            if (PlayerTeam(HandleToPlayer(result)) ==
                PlayerTeam(HandleToPlayer(OwnerId(self)))) {
                return 0;
            }
        }
        if (self->info->type == 0x9B934DE4) {
            return 1;
        }
        if (info.flags & 2) {
            return 1;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// HUD item/status icon drawing.
// ---------------------------------------------------------------------------

struct HudOwner {
    unsigned char pad00[0x24];
    int posX;               // 0x24
    int posY;               // 0x28
    int posZ;               // 0x2C
    unsigned char pad30[0x240];
    int condition;          // 0x270
    unsigned char pad274[0x378];
    StateMachine* machine;  // 0x5EC
    unsigned char pad5F0[0x2FC];
    int itemState;          // 0x8EC
};

extern "C" {
extern float lbl_804D7760;
extern float lbl_804D7770;
extern float lbl_804D7774;
extern float lbl_804D7778;
extern float lbl_804D777C;
extern float lbl_804D7780;
extern float lbl_804D7784;

int fn_80116E08(void* owner);
int fn_801409EC(int index);
int fn_800FD424(int team);
int fn_800F7C64(int state, unsigned long hash);
void fn_8012071C(unsigned long type, int arg, int* out, int a, int b);
double fn_80120C14(unsigned long type, int arg, int handle);
}

struct Actor;
Actor* GetPlayer();
int PlayerInHost(Actor* actor);
int HasCondition(void* owner, int condition);
int ItemColorDrawQuad(int index, float* pos, int width, int height, int color);
int ItemColorDrawQuadRange(int index, float* pos, int width, int height, int color);

// Draws the owner's item icons on the HUD: the carried item, the highlight
// for a matching item of the other player, then the pickup/drop indicator
// that belongs to the owner's current item state.
void DrawItemStatusIcons(void* arg)
{
    HudOwner* owner = (HudOwner*)arg;
    float pos[3];
    int player;
    int color;
    int value;

    if (fn_80116E08(owner) == 0) {
        if (PlayerInHost(GetPlayer()) != 0) {
            return;
        }
    }
    if (HasCondition(&owner->condition, 10) != 0) {
        return;
    }
    player = HandleToPlayer(OwnerId((Owner*)arg));
    color = fn_800FD424(PlayerTeam(player));

    pos[0] = (float)owner->posX;
    pos[1] = (float)owner->posY;
    pos[2] = (float)owner->posZ + lbl_804D7770;

    if (QueryName(0x4B000000, 0x896F8C7F) == 0) {
        ItemColorDrawQuad(fn_801409EC(5), pos, 0x60, 0x60, color);
    }
    if (QueryName(0x4B000000, 0xBDEDC248) != 0) {
        fn_8012071C(0xE2D6A9C0, 0, &value, 1, 1);
        if (fn_80120C14(0xE2D6A9C0, 0, player) ==
            fn_80120C14(0xE2D6A9C0, 0, value)) {
            ItemColorDrawQuadRange(fn_801409EC(2), pos, 0xC0, 0xC0, color);
        }
    }

    if (fn_800F7C64(owner->itemState, 0x6B4B4A10) != 0) {
        ItemColorDrawQuadRange(fn_801409EC(3), pos, 0xC0, 0xC0, fn_800FD424(1));
    } else if (fn_800F7C64(owner->itemState, 0x114F1979) != 0) {
        ItemColorDrawQuadRange(fn_801409EC(3), pos, 0xC0, 0xC0, fn_800FD424(0));
    } else if (fn_800F7C64(owner->itemState, 0x21425B82) != 0) {
        ItemColorDrawQuadRange(fn_801409EC(3), pos, 0xC0, 0xC0, fn_800FD424(2));
    } else if (fn_800F7C64(owner->itemState, 0xFAD5AC93) != 0) {
        ItemColorDrawQuadRange(fn_801409EC(3), pos, 0xC0, 0xC0, fn_800FD424(3));
    } else if (fn_800F7C64(owner->itemState, 0x6BA1BC35) != 0) {
        ItemColorDrawQuadRange(fn_801409EC(3), pos, 0xC0, 0xC0, fn_800FD424(4));
    } else if (fn_800F7C64(owner->itemState, 0xDBEC014D) != 0) {
        ItemColorDrawQuadRange(fn_801409EC(3), pos, 0xC0, 0xC0, fn_800FD424(5));
    } else if (fn_800F7C64(owner->itemState, 0xE54B6BDB) != 0) {
        ItemColorDrawQuadRange(fn_801409EC(3), pos, 0xC0, 0xC0, fn_800FD424(6));
    } else if (fn_800F7C64(owner->itemState, 0x154F7C95) != 0) {
        ItemColorDrawQuadRange(fn_801409EC(3), pos, 0xC0, 0xC0, fn_800FD424(7));
    }
}

// ---------------------------------------------------------------------------
// Item status message handling.
// ---------------------------------------------------------------------------

struct EventInfo {
    int unk0;      // 0x00
    int type;      // 0x04
    int target;    // 0x08
    float f0C;     // 0x0C
};

// Payload of the 0xC14 "unlock" broadcast.
struct UnlockMsg {
    int unk0;
    int unk4;
    unsigned long flags;
    unsigned long key;
};

struct EventState {
    int (*setInfoType)(int value, void* owner); // 0x00
    void* owner;            // 0x04
    unsigned char pad08[0x4];
    float f0C;              // 0x0C
    unsigned char pad10[0x54];
    float f64;              // 0x64
    unsigned char pad68[0x54];
    int fBC;                // 0xBC
};

// Pending item slot on the owner: the handle of the actor the item points at,
// then two words the item states use.
struct ItemSlot {
    int handle; // 0x00
    int unk04;  // 0x04
    int unk08;  // 0x08
};

struct EventOwner {
    unsigned char pad00[0x24];
    int posX;               // 0x24
    int posY;               // 0x28
    int posZ;               // 0x2C
    unsigned char pad30[0x8];
    int handle;             // 0x38
    unsigned char pad3C[0x88];
    void* unkC4;            // 0xC4
    unsigned char padC8[0x34];
    void (*unkFC)();        // 0xFC
    unsigned char pad100[0x4];
    int f104;               // 0x104
    unsigned char pad108[0x28];
    EventInfo* info;        // 0x130
    unsigned char pad134[0x4];
    float f138;             // 0x138
    unsigned char pad13C[0xA8];
    ItemSlot slots[4];      // 0x1E4
    unsigned char pad214[0x5C];
    unsigned char unk270[0x18]; // 0x270
    int f288;               // 0x288
    unsigned char pad28C[0x328];
    float f5B4;             // 0x5B4
    float f5B8;             // 0x5B8
    float f5BC;             // 0x5BC
    unsigned char pad5C0[0x10];
    int f5D0;               // 0x5D0
    unsigned char pad5D4[0x18];
    EventState* state;      // 0x5EC
    unsigned char pad5F0[0x10];
    int (*filter)(void* slot, void* target); // 0x600
    void (*unk604)();       // 0x604
    unsigned char pad608[0x50];
    unsigned char zoneFlags[0x23C]; // 0x658
    unsigned char unk894[4]; // 0x894
    unsigned char pad898[0x54];
    int itemState;          // 0x8EC
    unsigned char pad8F0[0x14];
    float f904;             // 0x904
};

struct TrackedActor {
    unsigned char pad00[0x24];
    int x; // 0x24
    int y; // 0x28
    int z; // 0x2C
};

extern "C" {
int rand();
int fn_80117FB4();
void fn_80117FF8(void* state, unsigned long hash, int* out);
void fn_80118084(void* state, unsigned long hash, int* out);
TrackedActor* fn_8002394C();
int fn_800BC01C(void* owner, float* pos);
extern float lbl_804D77A0;
int fn_800A1F94();
int fn_800B949C(int hash, int* out, int count, int arg);
int fn_800B6FE4(int handle);
void fn_800B261C(void* owner, int handle, void** out);
}

int ItemSetupDropScanTargets(void* arg);

// Handles the item-status message the owner just received. Returns 1 when the
// message is consumed, 0 when the caller keeps it.
int HandleItemStatusEvent(void* arg)
{
    EventOwner* owner;
    EventInfo* info;
    TrackedActor* target;
    Msg400 reply;
    float pos[3];
    float limit;
    int value;

    if (arg == 0) {
        return 0;
    }
    owner = (EventOwner*)arg;
    info = owner->info;
    if (fn_80117FB4() == 0) {
        return 1;
    }
    SendMsg(info->target, 0x400, &reply);
    switch (info->type) {
    case 0x3529A482:
    case -0x16D6524A: // 0xE929ADB6
    case -0x454EF93F: // 0xBAB106C1
    case -0x53413F58: // 0xACBEC0A8
    case 0x3147A953:
    case 0x7A16C04F:
        return 1;
    case 0x5A19E256:
    case 0x64F367A2:
        value = -1;
        limit = lbl_804D7778 +
                (lbl_804D777C * ((float)rand() / lbl_804D7774)) / lbl_804D7760;
        if (owner->f5BC < limit) {
            limit = (owner->state->fBC == -1) ? owner->state->f0C
                                              : owner->state->f64;
            if (lbl_804D7780 * limit < owner->f904) {
                limit = (lbl_804D7784 * ((float)rand() / lbl_804D7774)) /
                        lbl_804D7760;
                if (limit < owner->f5B4) {
                    fn_80117FF8(owner->state, 0x4D4915E6, &value);
                    if (value == -1) {
                        fn_80117FF8(owner->state, 0xFBA23962, &value);
                    }
                    if (value == -1) {
                        return 1;
                    }
                    target = fn_8002394C();
                    pos[0] = (float)target->x;
                    pos[1] = (float)target->y;
                    pos[2] = (float)target->z;
                    if (fn_800BC01C(owner, pos) != 0) {
                        ClearStateMachine(owner);
                        SetStateMachineState(owner, 0xDF1271D9);
                        fn_80118084(owner->state, 0x0315C41D, &value);
                    }
                    info->type = 0;
                }
            }
        }
        return 0;
    default:
        return 0;
    }
}

// ---------------------------------------------------------------------------
// Item action event handling.
// ---------------------------------------------------------------------------

// Target filter callback: returns 1 when the slot's actor must be skipped.
typedef int (*TargetFilter)(void* slot, void* target);

// Two positions (the owner's and the target's), as the 0x411 replies return
// them and as the effect spawner consumes them.
struct TwoPositions {
    int a[3];
    int b[3];
};

// Payload of the 0x411 asset-position query.
struct AssetQuery {
    int unk00;              // 0x00
    void* asset;            // 0x04
    unsigned char pad08[0x2C];
    int posX;               // 0x34
    int posY;               // 0x38
    int posZ;               // 0x3C
};

extern "C" {
extern float lbl_804D7788;
extern float lbl_804D778C;
extern float lbl_804D7790;
extern float lbl_804D7794;
extern float lbl_804D7798;
extern float lbl_804D779C;

void fn_80116E30(void* owner);
int fn_80162940(void* p, int arg);
int fn_801BD694();
int fn_800B26B8(void* owner);
int fn_800B1ED4(void* owner, int arg = 0);
void fn_800A78A0(void* p, void* actor);
unsigned long fn_8011D7E4(void* owner, int arg);
void fn_800CE75C();
void fn_800CEE08();
int fn_80117D80(void* owner);
void fn_800A7A54(void* list, int a, int b);
void fn_800A7B08(void* list, int a, int b);
int fn_80162658(void* owner, void* p);
void fn_800B42E0(void* p, int handle, float f, int a, int b);
int fn_800BAF40(void* owner);
extern unsigned long lbl_804D2E10;
}

struct ZoneList;
void ZoneFlagSet(ZoneList* list, int on, int unused);

struct IVec3 {
    int x;
    int y;
    int z;
};
void ZoneUpdate(void* zone, IVec3* pos);

// Item state table: 0x10-byte entries, one per item state id.
struct ItemStateEntry {
    void (*enter)(void* owner); // 0x00
    int (*update)(void* owner); // 0x04
    int (*check)(void* owner);  // 0x08
    int pad;                    // 0x0C
};

extern ItemStateEntry lbl_8039EDD8[];
extern float lbl_804D77E8;

int Rand();
void* AssetFind(unsigned long type, unsigned long id);
float OwnerValue(Owner* owner, void* value);
float StatGet(int a, int b, int c);
extern "C" int fn_8001F928(void* owner, TwoPositions* pos, unsigned long id,
                           TargetFilter filter, void* target);

// Handles an item-action event for the actor that owns the item. Returns 1
// when the event is consumed, 0 when the caller keeps it.
int HandleItemActionEvent(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    Msg400 reply;
    TwoPositions pos;
    AssetQuery query;
    TargetFilter filter;
    float zero;
    int proceed;
    int skip = 0;
    int i;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    if (QueryName(0x4B000000, 0x9C3599CE) == 0) {
        proceed = 1;
    } else if (owner->itemState == 0) {
        proceed = 0;
    } else if (fn_800F7C64(owner->itemState, 0x6B4B4A10) > 0 ||
               fn_800F7C64(owner->itemState, 0x114F1979) > 0 ||
               fn_800F7C64(owner->itemState, 0x21425B82) > 0 ||
               fn_800F7C64(owner->itemState, 0xFAD5AC93) > 0 ||
               fn_800F7C64(owner->itemState, 0x6BA1BC35) > 0 ||
               fn_800F7C64(owner->itemState, 0xDBEC014D) > 0 ||
               fn_800F7C64(owner->itemState, 0xE54B6BDB) > 0 ||
               fn_800F7C64(owner->itemState, 0x154F7C95) > 0) {
        proceed = 1;
    } else {
        proceed = 0;
    }
    if (!proceed) {
        return 1;
    }
    if (fn_80117FB4() == 0) {
        return 1;
    }
    SendMsg(info->target, 0x400, &reply);

    if (info->f0C < lbl_804D7788) {
        float f = info->f0C;
        if (lbl_804D778C < f) {
            info->f0C = f - owner->f138;
        }
        if (lbl_804D778C < info->f0C) {
            return 0;
        }
        info->f0C = lbl_804D7788;
        fn_80116E30(arg);
        return 1;
    }

    switch (info->type) {
    case 0x3147A953:
        if (OwnerValue((Owner*)owner, reply.actor) <= lbl_804D7790) {
            info->f0C -= owner->f138;
            return 0;
        }
        if (OwnerValue((Owner*)owner, reply.actor) <= lbl_804D7794) {
            EventOwner* target = (EventOwner*)reply.actor;
            filter = owner->filter;
            if (filter == 0) {
                filter = IsSameTeamOtherActor;
            }
            query.asset = AssetFind(0xF1754BF2, 0x21C5E0);
            if (SendMsg(owner->handle, 0x411, &query) != 0) {
                pos.a[0] = query.posX;
                pos.a[1] = query.posY;
                pos.a[2] = query.posZ;
            } else {
                pos.a[0] = owner->posX;
                pos.a[1] = owner->posY;
                pos.a[2] = owner->posZ;
            }
            query.asset = AssetFind(0xF1754BF2, 0x922BF04E);
            if (SendMsg(target->handle, 0x411, &query) != 0) {
                pos.b[0] = query.posX;
                pos.b[1] = query.posY;
                pos.b[2] = query.posZ;
            } else {
                pos.b[0] = target->posX;
                pos.b[1] = target->posY;
                pos.b[2] = target->posZ;
            }
            if (fn_8001F928(arg, &pos, 0x5000A200, filter, reply.actor) == 0) {
                skip = 1;
            } else {
                info->type = 0;
                zero = lbl_804D7794;
                ClearStateMachine(arg);
                SetStateMachineState(arg, 0x362FC3C7);
                fn_80118084(owner->state, 0x0315C41D, &info->target);
                fn_80118084(owner->state, 0x033FA508, (int*)&zero);
            }
        }
        break;
    case 0xACBEC0A8:
    case 0x7A16C04F:
        return 1;
    case 0x5A19E256:
    case 0x3529A482:
    case 0x64F367A2: {
        Msg400 probe;
        int percent =
            (int)StatGet(0x7BFB940D, 0, HandleToPlayer(OwnerId((Owner*)owner)));
        int ok;

        if (fn_80162940((char*)owner + 0x894, 0x1000) != 0) {
            ok = 0;
        } else if (percent < 0) {
            ok = 1;
        } else if (percent >= 10) {
            ok = 0;
        } else {
            unsigned long handle = fn_8011D7E4(arg, 0);
            if ((handle >> 24) == 0x13 && SendMsg(handle, 0x400, &probe) != 0 &&
                probe.actor != 0 && fn_801BD694() == 0) {
                ok = 1;
            } else if (fn_800B26B8(arg) < 4 &&
                       (fn_800B1ED4(arg) & 0x80) != 0) {
                ok = 1;
            } else {
                ok = 1;
                for (i = 0; i <= percent; i++) {
                    if (owner->f5B4 <
                        (float)(Rand() % 100 + 1) / lbl_804D7760) {
                        ok = 0;
                        break;
                    }
                }
            }
        }
        if (ok && lbl_804D7798 * owner->state->f64 < owner->f904) {
            return 1;
        }
        break;
    }
    default:
        break;
    }

    if (OwnerValue((Owner*)owner, reply.actor) <= lbl_804D7790) {
        info->f0C -= owner->f138;
        return 0;
    }
    if (((StateMachine*)owner->state)->state == 0x362FC3C7) {
        EventOwner* target = (EventOwner*)reply.actor;
        filter = owner->filter;
        if (filter == 0) {
            filter = IsSameTeamOtherActor;
        }
        query.asset = AssetFind(0xF1754BF2, 0x21C5E0);
        if (SendMsg(owner->handle, 0x411, &query) != 0) {
            pos.a[0] = query.posX;
            pos.a[1] = query.posY;
            pos.a[2] = query.posZ;
        } else {
            pos.a[0] = owner->posX;
            pos.a[1] = owner->posY;
            pos.a[2] = owner->posZ;
        }
        query.asset = AssetFind(0xF1754BF2, 0x922BF04E);
        if (SendMsg(target->handle, 0x411, &query) != 0) {
            pos.b[0] = query.posX;
            pos.b[1] = query.posY;
            pos.b[2] = query.posZ;
        } else {
            pos.b[0] = target->posX;
            pos.b[1] = target->posY;
            pos.b[2] = target->posZ;
        }
        if (fn_8001F928(arg, &pos, 0x5000A200, filter, reply.actor) == 0) {
            skip = 1;
        }
    }
    if (skip) {
        info->type = 0;
        zero = lbl_804D7790;
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xF1C37C5C);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        fn_80118084(owner->state, 0x033FA508, (int*)&zero);
    }
    return 0;
}

// Handles a secondary item-state event. Returns 1 when the event is consumed,
// otherwise whether the event still has no target (-1).
int HandleItemDropEvent(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    Msg400 reply;
    int value;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    if (fn_80117FB4() == 0) {
        return 1;
    }
    switch (info->type) {
    case 0xBAB106C1:
        return 1;
    case 0x30A0D567:
        return 1;
    case 0xACBEC0A8:
        return 1;
    case 0x3529A482:
        return 1;
    case 0xE929ADB6:
        return 1;
    case 0x64F367A2:
        value = -1;
        if (owner->f5BC < lbl_804D779C +
                (lbl_804D777C * ((float)rand() / lbl_804D7774)) /
                    lbl_804D7760) {
            fn_80117FF8(owner->state, 0x4D4915E6, &value);
            if (value == -1) {
                fn_80117FF8(owner->state, 0xFBA23962, &value);
            }
            if (value != -1) {
                fn_80118084(owner->state, 0x0315C41D, &value);
                SendMsg(value, 0x400, &reply);
                fn_800A78A0((char*)owner + 0x270, reply.actor);
            }
            info->type = 0;
        }
        break;
    default:
        break;
    }
    return info->target == -1;
}

int ItemUseBeginDrop(void* arg);

// Validates an item-use event for the actor that owns the item. Returns 1 when
// the event is consumed (or cannot be handled), 0 when the caller keeps it.
int HandleItemUseEvent(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    if (fn_80117FB4() == 0) {
        return 1;
    }
    switch (info->type) {
    case 0x3147A953:
        unsigned int target = info->target;
        if ((int)(target >> 24) != 0x13 || fn_800A1F94() == 0) {
            return 1;
        }
        ItemUseBeginDrop(arg);
        break;
    case 0x30A0D567:
        ItemUseBeginDrop(arg);
        break;
    case 0xACBEC0A8:
        return 1;
    case 0x64F367A2:
        return 1;
    case 0x6D5846FD:
        break;
    }
    return 0;
}

// Handles an item-target event: resolves the actor the item is aimed at, picks
// a new target when the item has none, and starts the follow-up state.
// Returns 1 when the event is consumed.
int HandleItemTargetEvent(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    float value;
    int proceed;
    int count;
    int i;
    int found;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    if (fn_80117FB4() == 0) {
        return 1;
    }
    switch (info->type) {
    case 0x30A0D567:
        ItemSetupDropScanTargets(arg);
        break;
    case 0xACBEC0A8:
        return 1;
    case 0x3147A953: {
        if (QueryName(info->target, 0x33032FA9) == 1) {
            QueryMsg q;
            Msg400 reply;
            int sent;
            int target;

            q.hash = 0x7C18FE9E;
            *(int*)&q.result = 0;
            q.unk8 = 0;
            sent = SendMsg(info->target, 0x41E, &q);
            target = *(int*)&q.result;
            if (sent != 0 && target != -1) {
                SendMsg(target, 0x400, &reply);
                if (reply.actor != 0 &&
                    QueryName(target, 0x5C8F66F9) == 0x5E5BCE3E) {
                    return 1;
                }
            }
        }
        ItemSetupDropScanTargets(arg);
        break;
    }
    case 0x7A16C04F:
        return 1;
    case 0x6D5846FD:
        break;
    case 0x64F367A2:
        return 1;
    }

    proceed = 0;
    if (QueryName(0x4B000000, 0x9C3599CE) == 0) {
        proceed = 1;
    } else if (owner->itemState != 0) {
        if (fn_800F7C64(owner->itemState, 0x6B4B4A10) > 0 ||
            fn_800F7C64(owner->itemState, 0x114F1979) > 0 ||
            fn_800F7C64(owner->itemState, 0x21425B82) > 0 ||
            fn_800F7C64(owner->itemState, 0xFAD5AC93) > 0 ||
            fn_800F7C64(owner->itemState, 0x6BA1BC35) > 0 ||
            fn_800F7C64(owner->itemState, 0xDBEC014D) > 0 ||
            fn_800F7C64(owner->itemState, 0xE54B6BDB) > 0 ||
            fn_800F7C64(owner->itemState, 0x154F7C95) > 0) {
            proceed = 1;
        }
    }
    if (proceed) {
        return 1;
    }
    if (info->target == -1) {
        int handles[8];

        count = fn_800B949C(0x0F1A463A, handles, 8, 1);
        found = -1;
        for (i = 0; i < count; i++) {
            if (fn_800B6FE4(handles[i]) != 0) {
                found = handles[i];
                break;
            }
        }
        info->target = found;
        if (info->target == -1) {
            return 1;
        }
        value = lbl_804D77A0;
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xF1C37C5C);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        fn_80118084(owner->state, 0x033FA508, (int*)&value);
    }
    return 0;
}

// Handles the item-focus event: when the tracked target answers the
// 0x33032FA9 query with 1, the target's own 0x7C18FE9E answer is resolved and
// the owner is pointed at it (and given its follow callbacks when it is a
// shielded actor).
extern "C" {
void fn_801174B0(void* owner, int handle);
void fn_800CE96C();
void fn_800CF064();
}

int HandleItemFocusEvent(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    int sent;
    int target;
    Msg400 reply;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    if (fn_80117FB4() == 0) {
        return 1;
    }
    switch (info->type) {
    case 0x3147A953:
        if (QueryName(info->target, 0x33032FA9) == 1) {
            QueryMsg q;

            q.hash = 0x7C18FE9E;
            *(int*)&q.result = 0;
            q.unk8 = 0;
            sent = SendMsg(info->target, 0x41E, &q);
            target = *(int*)&q.result;
            if (sent != 0 && target != -1) {
                SendMsg(target, 0x400, &reply);
                if (reply.actor != 0 &&
                    ((unsigned int)reply.ownerId >> 24) == 0x13) {
                    fn_801174B0(arg, (int)reply.actor);
                    if (((Owner*)reply.actor)->info->type == 0x9B934DE4) {
                        *(void**)((char*)arg + 0xFC) = (void*)fn_800CE96C;
                        *(void**)((char*)arg + 0x604) = (void*)fn_800CF064;
                    }
                    return 1;
                }
            }
        }
        return 1;
    case 0x30A0D567:
        return 1;
    case 0xACBEC0A8:
        return 1;
    case 0x3529A482:
        return 1;
    default:
        break;
    }
    return info->target == -1;
}

// Item predicate used by the behaviour table: this item is always usable.
int ItemUsableAlways()
{
    return 1;
}

// Item predicate used by the behaviour table: always true.
int ItemPredicateAlwaysTrue2()
{
    return 1;
}

// Item predicate used by the behaviour table: always true.
int ItemPredicateAlwaysTrue3()
{
    return 1;
}

// Item predicate used by the behaviour table: always true.
int ItemPredicateAlwaysTrue4()
{
    return 1;
}

// Item predicate used by the behaviour table: clears the item's target actor
// and reports success (doing nothing when there is no owner).
int ItemClearTarget(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;

    if (owner == 0) {
        return 0;
    }
    owner->info->target = -1;
    return 1;
}

// Clears the behaviour's current target actor. Returns 0 without an owner.
int ItemClearTargetFirst(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;

    if (owner == 0) {
        return 0;
    }
    owner->info->target = -1;
    return 1;
}

// Clears the behaviour's current target actor. Returns 0 without an owner.
int ItemClearTargetSecond(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;

    if (owner == 0) {
        return 0;
    }
    owner->info->target = -1;
    return 1;
}

// Clears the behaviour's current target actor. Returns 0 without an owner.
int ItemClearTargetThird(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;

    if (owner == 0) {
        return 0;
    }
    owner->info->target = -1;
    return 1;
}

// Always succeeds; the status check this replaces has no extra conditions.
int ItemStatusDefault(void)
{
    return 1;
}

// Item predicate used by the behaviour table: always true.
int ItemPredicateAlwaysTrue5(void)
{
    return 1;
}

// Loot tables: four item ids each, copied by value before a pick.
struct ItemIdTable {
    unsigned long ids[4];
};

extern "C" {
extern const ItemIdTable lbl_80345828;
extern const ItemIdTable lbl_80345848;
extern float lbl_804D77A4;
extern float lbl_804D77A8;
int fn_800B93DC(void* table, int count, int* out, int maxOut, int arg);
}

// Copies a loot table and lets the game pick up to maxOut of its entries;
// returns one of the picked ids, or -1 when the table is empty.
static inline int PickItemId(const ItemIdTable& src, int* out, int maxOut)
{
    ItemIdTable table = src;
    int n = fn_800B93DC(&table, 4, out, maxOut, 1);

    if (n == 0) {
        return -1;
    }
    if (n == 1) {
        return out[0];
    }
    return out[Rand() % n];
}

// Picks the id of the item to drop from one of the loot tables and starts the
// matching drop state. Returns 1, or 0 without an owner.
int ItemSetupDrop(int arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    EventState* state;
    float factor;
    float value;
    int result;
    int kind;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    info->unk0 = 0xD;
    info->type = 0;
    if (fn_80116E08(owner) == 0) {
        factor = ((EventState*)owner->state)->f0C;
    } else {
        factor = ((EventState*)owner->state)->f64;
    }
    kind = owner->f5D0;
    switch (kind) {
    case 0: {
        int out[8];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345828, out, 8);
        break;
    }
    case 2:
    case 3: {
        int out[16];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345848, out, 16);
        break;
    }
    default:
        if (owner->f5B4 <= lbl_804D7798) {
            if (owner->f904 <= lbl_804D77A4 * factor ||
                owner->f5B8 <= lbl_804D77A8) {
                int out[8];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345828, out, 8);
            } else {
                int out[16];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345848, out, 16);
            }
        } else {
            int out[16];
            value = lbl_804D7794;
            result = PickItemId(lbl_80345848, out, 16);
        }
        break;
    }
    info->target = result;
    ClearStateMachine(owner);
    state = (EventState*)owner->state;
    if (state->fBC == -1) {
        SetStateMachineState(owner, 0x469ABA3B);
    } else {
        SetStateMachineState(owner, 0xF1C37C5C);
    }
    fn_80118084(owner->state, 0x0315C41D, &info->target);
    fn_80118084(owner->state, 0x033FA508, (int*)&value);
    return 1;
}

// Shared tail of the item-setup events: picks the id of the item to drop from
// one of the loot tables and starts the matching drop state. Written as an
// inline helper, which is why every inlined copy re-checks the owner pointer.
// Returns 0 only when handed a null owner.
static inline int SetupItemDrop(void* arg)
{
    EventOwner* owner;
    EventInfo* info;
    EventState* state;
    float factor;
    float value;
    int result;
    int kind;

    if (arg == 0) {
        return 0;
    }
    owner = (EventOwner*)arg;
    info = owner->info;
    info->unk0 = 0xD;
    info->type = 0;
    if (fn_80116E08(owner) == 0) {
        factor = owner->state->f0C;
    } else {
        factor = owner->state->f64;
    }
    kind = owner->f5D0;
    switch (kind) {
    case 0: {
        int out[8];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345828, out, 8);
        break;
    }
    case 2:
    case 3: {
        int out[16];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345848, out, 16);
        break;
    }
    default:
        if (owner->f5B4 <= lbl_804D7798) {
            if (owner->f904 <= lbl_804D77A4 * factor ||
                owner->f5B8 <= lbl_804D77A8) {
                int out[8];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345828, out, 8);
            } else {
                int out[16];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345848, out, 16);
            }
        } else {
            int out[16];
            value = lbl_804D7794;
            result = PickItemId(lbl_80345848, out, 16);
        }
        break;
    }
    info->target = result;
    ClearStateMachine(arg);
    state = owner->state;
    if (state->fBC == -1) {
        SetStateMachineState(arg, 0x469ABA3B);
    } else {
        SetStateMachineState(arg, 0xF1C37C5C);
    }
    fn_80118084(owner->state, 0x0315C41D, &info->target);
    fn_80118084(owner->state, 0x033FA508, (int*)&value);
    return 1;
}

extern "C" {
extern float lbl_804D77AC;
extern const ItemIdTable lbl_80345838;
}

// Picks the candidate item whose owner value is lowest and starts the matching
// state; falls back to the normal drop pick when the table came back empty.
// Returns 1 when handled, 0 without an owner.
int ItemSetupNearest(void* arg)
{
    EventOwner* owner;
    EventInfo* info;
    Msg400 reply;
    float best;
    float value;
    int ids[8];
    int count;
    int result;
    int i;

    if (arg == 0) {
        return 0;
    }
    owner = (EventOwner*)arg;
    info = owner->info;
    info->unk0 = 0xC;
    info->type = 0;
    best = lbl_804D77AC;
    result = -1;
    {
        ItemIdTable table = lbl_80345838;
        count = fn_800B93DC(&table, 4, ids, 8, 1);
    }
    if (count == 0) {
        result = -1;
    } else {
        for (i = 0; i < count; i++) {
            float dist;
            SendMsg(ids[i], 0x400, &reply);
            dist = OwnerValue((Owner*)owner, reply.actor);
            if (dist < best) {
                best = dist;
                result = ids[i];
            }
        }
    }
    info->target = result;
    if (info->target != -1) {
        value = lbl_804D7794;
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xF1C37C5C);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        fn_80118084(owner->state, 0x033FA508, (int*)&value);
        return 1;
    }
    SetupItemDrop(arg);
    return 1;
}

// Starts the item's 0xB state when the pending handle resolves, otherwise
// falls back to the normal drop pick. Returns 0 without an owner.
int ItemSetupFromHandle(void* arg)
{
    EventOwner* owner;
    EventInfo* info;
    int result;

    if (arg == 0) {
        return 0;
    }
    owner = (EventOwner*)arg;
    info = owner->info;
    info->unk0 = 0xB;
    info->type = 0;
    result = fn_8011D7E4(arg, 4);
    info->target = result;
    if (info->target != -1) {
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xDF1271D9);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        return 1;
    }
    SetupItemDrop(arg);
    return 1;
}

// Tries to resolve a pending handle of kind 2 and starts the matching drop
// state; falls back to the normal drop pick. The owner pointer is not checked
// before the info block is touched, matching the retail entry. Returns 1.
int ItemSetupFromHandleType2(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info = owner->info;
    int result;

    info->unk0 = 0xA;
    info->type = 0;
    result = fn_8011D7E4(arg, 2);
    info->target = result;
    if (info->target != -1) {
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xDF1271D9);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        return 1;
    }
    SetupItemDrop(arg);
    return 1;
}

// Same as above for a pending handle of kind 3; unlike type 2 this one rejects
// a null owner up front. Returns 0 without an owner.
int ItemSetupFromHandleType3(void* arg)
{
    EventOwner* owner;
    EventInfo* info;
    int result;

    if (arg == 0) {
        return 0;
    }
    owner = (EventOwner*)arg;
    info = owner->info;
    info->unk0 = 9;
    info->type = 0;
    result = fn_8011D7E4(arg, 3);
    info->target = result;
    if (info->target != -1) {
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xDF1271D9);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        return 1;
    }
    SetupItemDrop(arg);
    return 1;
}

// Tries to resolve a pending handle of kind 1 and starts the matching drop
// state; falls back to the normal drop pick. The owner pointer is not checked
// before the info block is touched, matching the retail entry. Returns 1.
int ItemSetupFromHandleType1(void* arg)
{
    EventOwner* owner;
    EventInfo* info;
    int result;

    if (arg == 0) {
        return 0;
    }
    owner = (EventOwner*)arg;
    info = owner->info;
    info->unk0 = 8;
    info->type = 0;
    result = fn_8011D7E4(arg, 1);
    info->target = result;
    if (info->target != -1) {
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xDF1271D9);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        return 1;
    }
    SetupItemDrop(arg);
    return 1;
}

// ---------------------------------------------------------------------------
// Item-use setup.
// ---------------------------------------------------------------------------

// Second half of the item-use setup functions: picks the id of the item to drop
// from one of the loot tables and starts the matching drop state. Written as an
// inline helper, which is why every inlined copy re-checks the owner pointer.
// Returns 1 even when handed a null owner.
static inline int SetupItemDropForUse(void* arg)
{
    EventOwner* owner;
    EventInfo* info;
    EventState* state;
    float factor;
    float value;
    int result;
    int kind;

    if (arg == 0) {
        return 1;
    }
    owner = (EventOwner*)arg;
    info = owner->info;
    info->unk0 = 0xD;
    info->type = 0;
    if (fn_80116E08(owner) == 0) {
        factor = owner->state->f0C;
    } else {
        factor = owner->state->f64;
    }
    kind = owner->f5D0;
    switch (kind) {
    case 0: {
        int out[8];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345828, out, 8);
        break;
    }
    case 2:
    case 3: {
        int out[16];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345848, out, 16);
        break;
    }
    default:
        if (owner->f5B4 <= lbl_804D7798) {
            if (owner->f904 <= lbl_804D77A4 * factor ||
                owner->f5B8 <= lbl_804D77A8) {
                int out[8];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345828, out, 8);
            } else {
                int out[16];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345848, out, 16);
            }
        } else {
            int out[16];
            value = lbl_804D7794;
            result = PickItemId(lbl_80345848, out, 16);
        }
        break;
    }
    info->target = result;
    ClearStateMachine(arg);
    state = owner->state;
    if (state->fBC == -1) {
        SetStateMachineState(arg, 0x469ABA3B);
    } else {
        SetStateMachineState(arg, 0xF1C37C5C);
    }
    fn_80118084(owner->state, 0x0315C41D, &info->target);
    fn_80118084(owner->state, 0x033FA508, (int*)&value);
    return 1;
}

// Item-use setup: takes the item handle the actor already carries when the
// pending slot resolves, otherwise falls back to picking an item from the loot
// tables and starting the drop state. Returns 0 without an owner.
int ItemUseBeginDrop(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    void* slot;
    float value;
    int handle;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    info->unk0 = 7;
    info->type = 0;
    info->target = -1;
    handle = fn_800B26B8(arg);
    if (handle < 4 && (fn_800B1ED4(arg, handle) & 0x80) != 0) {
        fn_800B261C(arg, handle, &slot);
        if (slot != 0) {
            info->target = ((TargetActor*)slot)->owner;
        }
    }
    if (info->target != -1) {
        value = lbl_804D77A0;
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xF1C37C5C);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        fn_80118084(owner->state, 0x033FA508, (int*)&value);
        return 1;
    }
    return SetupItemDropForUse(arg);
}

// Item-use setup for the shielded item: asks the shared item store for the
// carried handle first, then falls back to the loot tables and drop state.
int ItemUseBeginDropFromStore(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    float value;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    info->unk0 = 6;
    info->type = 0;
    info->target = QueryName(0x4B000000, 0xCC991E05);
    if (info->target != -1) {
        value = lbl_804D77A0;
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xF1C37C5C);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        fn_80118084(owner->state, 0x033FA508, (int*)&value);
        return 1;
    }
    return SetupItemDropForUse(arg);
}

// Scans the tracked actor handles for a live one and starts the item drop state
// on it; when none is alive, falls back to picking an item id from one of the
// loot tables and starting the matching drop state. Returns 0 only when handed
// a null owner.
int ItemSetupDropScanTargets(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    EventState* state;
    float factor;
    float value;
    int handles[8];
    int result;
    int count;
    int kind;
    int i;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    info->unk0 = 5;
    info->type = 0;
    count = fn_800B949C(0x0F1A463A, handles, 8, 1);
    result = -1;
    for (i = 0; i < count; i++) {
        if (fn_800B6FE4(handles[i]) != 0) {
            result = handles[i];
            break;
        }
    }
    info->target = result;
    if (info->target != -1) {
        value = lbl_804D77A0;
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0xF1C37C5C);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        fn_80118084(owner->state, 0x033FA508, (int*)&value);
        return 1;
    }
    if (arg == 0) {
        return 1;
    }
    info = owner->info;
    info->unk0 = 0xD;
    info->type = 0;
    if (fn_80116E08(owner) == 0) {
        factor = owner->state->f0C;
    } else {
        factor = owner->state->f64;
    }
    kind = owner->f5D0;
    switch (kind) {
    case 0: {
        int out[8];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345828, out, 8);
        break;
    }
    case 2:
    case 3: {
        int out[16];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345848, out, 16);
        break;
    }
    default:
        if (owner->f5B4 <= lbl_804D7798) {
            if (owner->f904 <= lbl_804D77A4 * factor ||
                owner->f5B8 <= lbl_804D77A8) {
                int out[8];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345828, out, 8);
            } else {
                int out[16];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345848, out, 16);
            }
        } else {
            int out[16];
            value = lbl_804D7794;
            result = PickItemId(lbl_80345848, out, 16);
        }
        break;
    }
    info->target = result;
    ClearStateMachine(arg);
    state = owner->state;
    if (state->fBC == -1) {
        SetStateMachineState(arg, 0x469ABA3B);
    } else {
        SetStateMachineState(arg, 0xF1C37C5C);
    }
    fn_80118084(owner->state, 0x0315C41D, &info->target);
    fn_80118084(owner->state, 0x033FA508, (int*)&value);
    return 1;
}

extern "C" {
extern float lbl_804D77B0;
}

int ItemStorePickRandomHandle(void* owner);

// Starts the item drop state on the handle the actor's item store reports;
// when it has none, falls back to picking an item id from one of the loot
// tables and starting the matching drop state. Returns 0 only when handed a
// null owner.
int ItemSetupDropStoredTarget(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    EventState* state;
    float factor;
    float value;
    int result;
    int kind;

    if (arg == 0) {
        return 0;
    }
    info = owner->info;
    info->unk0 = 4;
    info->type = 0;
    info->target = ItemStorePickRandomHandle(arg);
    if (info->target != -1) {
        value = lbl_804D77B0;
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0x469ABA3B);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        fn_80118084(owner->state, 0x033FA508, (int*)&value);
        return 1;
    }
    if (arg == 0) {
        return 1;
    }
    info = owner->info;
    info->unk0 = 0xD;
    info->type = 0;
    if (fn_80116E08(owner) == 0) {
        factor = owner->state->f0C;
    } else {
        factor = owner->state->f64;
    }
    kind = owner->f5D0;
    switch (kind) {
    case 0: {
        int out[8];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345828, out, 8);
        break;
    }
    case 2:
    case 3: {
        int out[16];
        value = lbl_804D7794;
        result = PickItemId(lbl_80345848, out, 16);
        break;
    }
    default:
        if (owner->f5B4 <= lbl_804D7798) {
            if (owner->f904 <= lbl_804D77A4 * factor ||
                owner->f5B8 <= lbl_804D77A8) {
                int out[8];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345828, out, 8);
            } else {
                int out[16];
                value = lbl_804D7794;
                result = PickItemId(lbl_80345848, out, 16);
            }
        } else {
            int out[16];
            value = lbl_804D7794;
            result = PickItemId(lbl_80345848, out, 16);
        }
        break;
    }
    info->target = result;
    ClearStateMachine(arg);
    state = owner->state;
    if (state->fBC == -1) {
        SetStateMachineState(arg, 0x469ABA3B);
    } else {
        SetStateMachineState(arg, 0xF1C37C5C);
    }
    fn_80118084(owner->state, 0x0315C41D, &info->target);
    fn_80118084(owner->state, 0x033FA508, (int*)&value);
    return 1;
}

int ItemStorePickClosestHandle(void* arg);

int ItemStorePickPreferredHandle(void* owner);

// Item-setup event: asks the actor's item store for a pending handle (kind 3)
// and starts the drop state on it; otherwise falls back to picking a loot id.
// Returns 0 only when handed a null owner.
int ItemSetupDropPending3(void* arg)
{
    EventOwner* owner;
    EventInfo* info;
    float value;

    if (arg == 0) {
        return 0;
    }
    owner = (EventOwner*)arg;
    info = owner->info;
    info->unk0 = 3;
    info->type = 0;
    info->target = ItemStorePickPreferredHandle(arg);
    if (info->target != -1) {
        value = lbl_804D77B0;
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0x469ABA3B);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        fn_80118084(owner->state, 0x033FA508, (int*)&value);
        return 1;
    }
    return SetupItemDropForUse(arg);
}

// Item-setup event: asks the item store for a pending handle (kind 2) and
// starts the drop state on it; otherwise falls back to picking a loot id.
// Returns 0 only when handed a null owner.
int ItemSetupDropPending2(void* arg)
{
    EventOwner* owner;
    EventInfo* info;
    float value;

    if (arg == 0) {
        return 0;
    }
    owner = (EventOwner*)arg;
    info = owner->info;
    info->unk0 = 2;
    info->type = 0;
    info->target = ItemStorePickClosestHandle(arg);
    if (info->target != -1) {
        value = lbl_804D77B0;
        ClearStateMachine(arg);
        SetStateMachineState(arg, 0x469ABA3B);
        fn_80118084(owner->state, 0x0315C41D, &info->target);
        fn_80118084(owner->state, 0x033FA508, (int*)&value);
        return 1;
    }
    return SetupItemDropForUse(arg);
}

// Item predicate used by the behaviour table: always true.
int ItemPredicateAlwaysTrue6()
{
    return 1;
}

// Item predicate used by the behaviour table: always true.
int ItemPredicateAlwaysTrue7()
{
    return 1;
}

extern "C" {
extern float lbl_804D77C4;
extern float lbl_804D77C8;
extern float lbl_804D77D0;
extern float lbl_804D77D4;
int fn_800FD3EC();
int fn_800FD6EC(int index);
int IsValidAttackTarget(Owner* owner, int handle);
}

// Asks the target's 0x41E handler for a named float, defaulting to def when it
// has no answer.
static inline float QueryNameFloat(int target, unsigned long hash, float def)
{
    QueryMsg q;

    q.hash = hash;
    *(float*)&q.result = def;
    q.unk8 = 1;
    SendMsg(target, 0x41E, &q);
    return *(float*)&q.result;
}

// Returns the item rank code (9, 10 or 11) the HUD shows for the owner: it
// compares the owner's stat with the best of the other players and with the
// owner's own item condition thresholds.
int ItemRankCode(void* arg, void* other)
{
    EventOwner* owner = (EventOwner*)arg;
    float value;
    int i;
    int player;
    int mine;
    int max;

    if (arg == 0 || other == 0) {
        return 0;
    }
    max = 0;
    value = QueryNameFloat(0x4B000000, 0xD1DA12E7, lbl_804D778C);
    player = HandleToPlayer(OwnerId((Owner*)arg));
    mine = (int)StatGet(0xE2D6A9C0, 0, player);
    for (i = 0; i < fn_800FD3EC(); i++) {
        if (i != player &&
            (int)StatGet(0xE2D6A9C0, 0, i) - mine > max) {
            max = (int)StatGet(0xE2D6A9C0, 0, i) - mine;
        }
    }
    if (owner->f5B4 < lbl_804D77C4) {
        return 0xB;
    }
    if (owner->f5B4 > lbl_804D77A4) {
        if (value < lbl_804D77C8 || max > 3) {
            return 9;
        }
        return 10;
    }
    if (owner->state->fBC != -1) {
        if (owner->f904 < lbl_804D77A8 * owner->state->f64) {
            return 0xB;
        }
        return 10;
    }
    return 10;
}

// Reply of the 0x6705 item-state query: the target's item state at +4.
struct ItemStateReply {
    int unk00;     // 0x00
    int itemState; // 0x04
};

// Reply of the 0x401 position query: the two floats it reports.
struct PosReply {
    unsigned char pad00[0x10];
    float f10; // 0x10
    float f14; // 0x14
};

TrackedActor* OwnerGetInfo(unsigned long handle);

// Finds the actor handle the owner's item should act on. Mode 0 collects every
// candidate and returns a random one; mode 1 returns the first player whose
// item state is one of the known items; mode 3 the player with the highest
// stat; modes 2, 4 and the default pick the nearest candidate. Returns -1 when
// there is no candidate.
int FindItemTarget(void* arg, int mode)
{
    EventOwner* owner = (EventOwner*)arg;
    EventInfo* info;
    Msg400 reply;
    ItemStateReply stateReply;
    PosReply posReply;
    TrackedActor* actor;
    float pos[3];
    float bestPos = lbl_804D77D0;
    int bestStat = -99999;
    int count = 0;
    float bestValue = lbl_804D77D4;
    int result = -1;
    int found[9];
    int handle;
    int named;
    int i;

    info = owner->info;
    if (info->type == 0xACBEC0A8 &&
        (info->unk0 == 8 || info->unk0 == 9 || info->unk0 == 10 ||
         info->unk0 == 0xB)) {
        return -1;
    }
    for (i = 0; i < 4; i++) {
        if (owner->slots[i].handle != -1) {
            if ((unsigned long)owner->slots[i].handle >> 24 == 1) {
                named = QueryName(owner->slots[i].handle, 0x1C1A4CC9);
            } else {
                named = QueryName(owner->slots[i].handle, 0x6C47D293);
            }
            if ((named == -1 || named == 0) &&
                SendMsg(owner->slots[i].handle, 0x400, &reply) != 0 &&
                reply.actor != 0 && reply.state == 1) {
                return owner->slots[i].handle;
            }
            break;
        }
    }
    for (i = 0; i < fn_800FD3EC(); i++) {
        handle = fn_800FD6EC(i);
        if (IsValidAttackTarget((Owner*)arg, handle) == 0) {
            continue;
        }
        switch (mode) {
        case 0:
            found[count++] = handle;
            break;
        case 1:
            SendMsg(handle, 0x6705, &stateReply);
            if (fn_800F7C64(stateReply.itemState, 0x6B4B4A10) > 0 ||
                fn_800F7C64(stateReply.itemState, 0x114F1979) > 0 ||
                fn_800F7C64(stateReply.itemState, 0x21425B82) > 0 ||
                fn_800F7C64(stateReply.itemState, 0xFAD5AC93) > 0 ||
                fn_800F7C64(stateReply.itemState, 0x6BA1BC35) > 0 ||
                fn_800F7C64(stateReply.itemState, 0xDBEC014D) > 0 ||
                fn_800F7C64(stateReply.itemState, 0xE54B6BDB) > 0 ||
                fn_800F7C64(stateReply.itemState, 0x154F7C95) > 0) {
                return handle;
            }
            break;
        case 3:
            if ((float)bestStat < StatGet(0xE2D6A9C0, 0, i)) {
                bestStat = (int)StatGet(0xE2D6A9C0, 0, i);
                result = handle;
            }
            break;
        case 4:
            SendMsg(handle, 0x401, &posReply);
            if (posReply.f14 > lbl_804D778C && posReply.f10 < bestPos) {
                bestPos = posReply.f10;
                result = handle;
            }
            break;
        default:
            SendMsg(handle, 0x400, &reply);
            {
                float value = OwnerValue((Owner*)arg, reply.actor);

                if (value < bestValue) {
                    bestValue = value;
                    result = handle;
                }
            }
            break;
        }
    }
    if (mode == 0) {
        if (count < 1) {
            result = -1;
        } else {
            result = found[Rand() % count];
        }
    }
    if (result != -1) {
        actor = OwnerGetInfo(result);
        pos[0] = (float)actor->x;
        pos[1] = (float)actor->y;
        pos[2] = (float)actor->z;
        if (fn_800BC01C(arg, pos) == 0) {
            result = -1;
        }
    }
    return result;
}

extern "C" {
extern const ItemIdTable lbl_803457F8;
extern const ItemIdTable lbl_80345808;
extern const ItemIdTable lbl_80345818;
extern float lbl_804D77CC;
int fn_800F8AF4(int item);
int fn_800BBF9C(void* owner, void* actor);
int fn_800A7B78(void* p, int arg);
}

// Item-store query used when an item needs a drop target: asks the shared item
// table for its entries, keeps the handles that answer the name queries with
// the item's marker, and takes the closest candidate. Handles of the two
// special item categories win over plain ones. Returns -1 when none qualifies.
int ItemStorePickPreferredHandle(void* owner)
{
    ItemIdTable table = lbl_80345818;
    Msg400 reply;
    int handles[32];
    float bestPlain = lbl_804D77CC;
    float bestSpecial = lbl_804D77CC;
    float bestCarried = lbl_804D77CC;
    int foundPlain = -1;
    int foundSpecial = -1;
    int foundCarried = -1;
    int count;
    int i;

    count = fn_800B93DC(&table, 4, handles, 32, 1);
    if (count == 0) {
        return -1;
    }
    for (i = 0; i < count; i++) {
        int handle = handles[i];
        int target;
        int kind;
        float dist;

        if (QueryName(handle, 0x33032FA9) != 1) {
            continue;
        }
        target = QueryName(handle, 0x7C18FE9E);
        if (target == -1 || ((unsigned int)target >> 24) != 0x13) {
            continue;
        }
        if (SendMsg(target, 0x400, &reply) == 0) {
            continue;
        }
        if (reply.actor == 0 || reply.state == 0) {
            continue;
        }
        if (*(int*)((char*)reply.actor + 0x2C4) == -1) {
            continue;
        }
        if (fn_800BBF9C(owner, reply.actor) == 0) {
            continue;
        }
        if (fn_800A7B78((char*)reply.actor + 0x270, 0xB) != 0) {
            continue;
        }
        if (QueryName(*(int*)((char*)reply.actor + 0x38), 0xFF3C0C68) != 0) {
            continue;
        }
        dist = OwnerValue((Owner*)owner, reply.actor);
        kind = fn_800F8AF4(*(int*)((char*)reply.actor + 0x8F0));
        switch (kind) {
        case 0x020C054A:
        case -0x59FD581A:
            if (dist < bestSpecial) {
                foundSpecial = handle;
                bestSpecial = dist;
            }
            break;
        case -0x7477138:
        case 0x7990FA64:
            if (dist < bestCarried) {
                foundCarried = handle;
                bestCarried = dist;
            }
            break;
        default:
            if (dist < bestPlain) {
                foundPlain = handle;
                bestPlain = dist;
            }
            break;
        }
    }
    if (foundCarried == -1) {
        foundCarried = foundPlain;
        if (foundSpecial != -1) {
            foundCarried = foundSpecial;
        }
    }
    return foundCarried;
}

// Item-store query used when an item needs a drop target: walks the shared item
// table starting at a random entry and returns the first handle that answers
// the name queries with the item's marker. Returns -1 when none qualifies.
int ItemStorePickRandomHandle(void* owner)
{
    ItemIdTable table = lbl_80345808;
    Msg400 reply;
    int handles[32];
    int count;
    int start;
    int i;

    count = fn_800B93DC(&table, 4, handles, 32, 1);
    if (count == 0) {
        return -1;
    }
    start = Rand() % count;
    for (i = start; i < start + count; i++) {
        int handle = handles[i % count];
        int target;

        if (QueryName(handle, 0x33032FA9) != 1) {
            continue;
        }
        target = QueryName(handle, 0x7C18FE9E);
        if (target == -1 || ((unsigned int)target >> 24) != 0x13) {
            continue;
        }
        if (SendMsg(target, 0x400, &reply) == 0) {
            continue;
        }
        if (reply.actor == 0 || reply.state == 0) {
            continue;
        }
        if (*(int*)((char*)reply.actor + 0x2C4) == -1) {
            continue;
        }
        if (fn_800BBF9C(owner, reply.actor) == 0) {
            continue;
        }
        if (fn_800A7B78((char*)reply.actor + 0x270, 0xB) != 0) {
            continue;
        }
        if (QueryName(*(int*)((char*)reply.actor + 0x38), 0xFF3C0C68) != 0) {
            continue;
        }
        return handle;
    }
    return -1;
}

// Item-store query used when an item needs a drop target: picks the handle
// whose target actor is nearest to the owner among the handles that answer the
// name queries. Returns -1 when none qualifies.
int ItemStorePickClosestHandle(void* arg)
{
    ItemIdTable table = lbl_803457F8;
    Msg400 reply;
    int handles[32];
    float best = lbl_804D77CC;
    int result = -1;
    int count;
    int i;

    count = fn_800B93DC(&table, 4, handles, 32, 1);
    if (count == 0) {
        return -1;
    }
    for (i = 0; i < count; i++) {
        int handle = handles[i];
        int target;
        float dist;

        if (QueryName(handle, 0x33032FA9) != 1) {
            continue;
        }
        target = QueryName(handle, 0x7C18FE9E);
        if (target == -1 || ((unsigned int)target >> 24) != 0x13) {
            continue;
        }
        if (SendMsg(target, 0x400, &reply) == 0) {
            continue;
        }
        if (reply.actor == 0 || reply.state == 0) {
            continue;
        }
        if (fn_800A7B78((char*)reply.actor + 0x270, 0xB) != 0) {
            continue;
        }
        if (*(int*)((char*)reply.actor + 0x2C4) == -1) {
            continue;
        }
        if (fn_800BBF9C(arg, reply.actor) == 0) {
            continue;
        }
        if (QueryName(*(int*)((char*)reply.actor + 0x38), 0xFF3C0C68) != 0) {
            continue;
        }
        dist = OwnerValue((Owner*)arg, reply.actor);
        if (dist < best) {
            best = dist;
            result = handle;
        }
    }
    return result;
}

extern "C" {
extern float lbl_804D7768;
extern float lbl_804D77B4;
extern float lbl_804D77B8;
extern float lbl_804D77BC;
extern float lbl_804D77C0;
void fn_800A3A40(void* owner);
int fn_800FD720();
int fn_8011D610(void* owner, int handle);
}

// True when the owner's item holds one of the known item ids.
static inline int ItemHasKnownState(EventOwner* owner)
{
    if (fn_800F7C64(owner->itemState, 0x6B4B4A10) > 0 ||
        fn_800F7C64(owner->itemState, 0x114F1979) > 0 ||
        fn_800F7C64(owner->itemState, 0x21425B82) > 0 ||
        fn_800F7C64(owner->itemState, 0xFAD5AC93) > 0 ||
        fn_800F7C64(owner->itemState, 0x6BA1BC35) > 0 ||
        fn_800F7C64(owner->itemState, 0xDBEC014D) > 0 ||
        fn_800F7C64(owner->itemState, 0xE54B6BDB) > 0 ||
        fn_800F7C64(owner->itemState, 0x154F7C95) > 0) {
        return 1;
    }
    return 0;
}

// Picks the item state implied by the owner's item kind and condition.
static inline int ItemStateFromKind(EventOwner* owner)
{
    int kind = owner->f5D0;

    if (kind == 2) {
        if (owner->f5B8 <= lbl_804D77C0 || owner->f5B4 <= lbl_804D77BC ||
            owner->f904 <= lbl_804D77A8 * owner->state->f0C) {
            return 2;
        }
        return 3;
    }
    if (kind == 0) {
        return 4;
    }
    if (kind >= 2 && kind < 4) {
        if (lbl_804D7798 * owner->state->f0C <= owner->f904) {
            return 3;
        }
        return 2;
    }
    if (lbl_804D77B4 <= owner->f5B8) {
        if (owner->f5B8 <= lbl_804D77B8 || owner->f5B4 <= lbl_804D77BC ||
            owner->f904 <= lbl_804D7798 * owner->state->f0C) {
            return 2;
        }
        return 3;
    }
    return 4;
}

// Returns 1 when the owner's item may change state right now: the player's
// stat roll and the item's condition allow it.
static inline int ItemCanUse(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    Msg400 reply;
    int percent;
    int handle;
    int i;

    fn_800A3A40(owner);
    handle = fn_800FD720();
    percent = (int)fn_80120C14(0x7BFB940D, 0, handle);
    if (fn_80162940((char*)owner + 0x894, 0x1000) != 0) {
        return 0;
    }
    if (percent < 0) {
        return 1;
    }
    if (percent >= 10) {
        return 0;
    }
    handle = (int)fn_8011D7E4(owner, 0);
    if ((handle >> 24) == 0x13 && SendMsg(handle, 0x400, &reply) != 0 &&
        reply.actor != 0 && fn_801BD694() == 0) {
        return 1;
    }
    if (fn_800B26B8(owner) < 4 && (fn_800B1ED4(owner) & 0x80) != 0) {
        return 1;
    }
    for (i = 0; i <= percent; i++) {
        if (owner->f5B4 < (float)(Rand() % 100 + 1) / lbl_804D7760) {
            return 0;
        }
    }
    return 1;
}

// Returns the first live handle among the tracked item handles, or -1.
static inline int FindLiveItemHandle(int* handles)
{
    int count = fn_800B949C(0x0F1A463A, handles, 8, 1);
    int i;

    for (i = 0; i < count; i++) {
        if (fn_800B6FE4(handles[i]) != 0) {
            return handles[i];
        }
    }
    return -1;
}

// Picks the item state id the owner's item should be in right now: the item's
// condition and kind decide it while the item-select screen is closed, the
// carried item and the player's stat otherwise.
int ItemSelectState(void* arg)
{
    EventOwner* owner = (EventOwner*)arg;
    int kind;
    int handle;
    int handles[8];
    int found;

    if (arg == 0) {
        return 0;
    }
    kind = fn_800B26B8(owner);
    if (fn_80116E08(owner) == 0) {
        return ItemStateFromKind(owner);
    }
    if (QueryName(0x4B000000, 0x9C3599CE) == 0 ||
        (owner->itemState != 0 && ItemHasKnownState(owner))) {
        if (!ItemCanUse(arg)) {
            return 0xC;
        }
        if (kind < 4 && (fn_800B1ED4(owner, kind) & 0x80) != 0) {
            return 7;
        }
        handle = (int)fn_8011D7E4(owner, 0);
        if (handle == -1) {
            fn_800A3A40(owner);
            handle = fn_800FD720();
            if ((int)fn_80120C14(0x7BFB940D, 0, handle) < 1) {
                return 0xD;
            }
            return 0xC;
        }
        return fn_8011D610(owner, handle);
    }
    if (ItemCanUse(arg)) {
        if (kind < 4 && (fn_800B1ED4(owner, kind) & 0x80) != 0) {
            return 7;
        }
        if ((int)fn_8011D7E4(owner, 1) != -1) {
            return 8;
        }
        handle = (int)fn_8011D7E4(owner, 0);
        if (handle != -1) {
            return fn_8011D610(owner, handle);
        }
        found = FindLiveItemHandle(handles);
        return (found == -1) ? 6 : 5;
    }
    found = FindLiveItemHandle(handles);
    if (found != -1) {
        return 5;
    }
    handle = (int)fn_8011D7E4(owner, 1);
    if (handle == -1) {
        return 6;
    }
    if (fn_80162940((char*)owner + 0x894, 0x1000) == 0) {
        return 8;
    }
    fn_80116E30(owner);
    return ItemStateFromKind(owner);
}

// ---------------------------------------------------------------------------
// Item effect and item wear message handling.
// ---------------------------------------------------------------------------

// The extra-trail holder lives in another unit; only its name is needed here.
struct ExtraHolder;

// Payload of the item effect message.
struct EffectMsg {
    int handle; // 0x00
    int unk04;  // 0x04
    int type;   // 0x08
    int unk0C;  // 0x0C
    int unk10;  // 0x10
    int arg;    // 0x14
    int unk18;  // 0x18
    int unk1C;  // 0x1C
    float x;    // 0x20
    float y;    // 0x24
    float z;    // 0x28
};

// Owner fields used by the item effect message handler.
struct EffectOwner {
    unsigned char pad00[0x24];
    int posX;                             // 0x24
    int posY;                             // 0x28
    int posZ;                             // 0x2C
    int (*dispatch)(int msg, void* data); // 0x30
    unsigned char pad34[0x8B8];
    int itemState; // 0x8EC
    unsigned char pad8F0[0x14];
    float condition; // 0x904
};

// Reply the 0xC0A handler writes back through the data pointer.
struct EffectReply {
    int handle;  // 0x00
    float value; // 0x04
    int result;  // 0x08
};

// Payload of the item wear message; the extra-trail system consumes the same
// block.
struct ExtraMsg {
    int unk00;              // 0x00
    int unk04;              // 0x04
    int handle;             // 0x08
    float f0C;              // 0x0C
    int unk10;              // 0x10
    int type;               // 0x14
    int pos0[3];            // 0x18
    int pos1[3];            // 0x24
    unsigned char pad30[0x14];
    int bone;               // 0x44
    unsigned char pad48[0xC];
    int active;             // 0x54
};

// Owner fields used by the item wear message handler.
struct WearOwner {
    unsigned char pad00[0xCC];
    int model;                // 0xCC
    unsigned char padD0[0x4];
    int unkD4;                // 0xD4
    unsigned char padD8[0x13C];
    int unk214;               // 0x214
    unsigned char pad218[0x58];
    unsigned char cond[0x18]; // 0x270
    unsigned char pad288[0x364];
    void* state;              // 0x5EC
    void* trail;              // 0x5F0
    void* trailOwner;         // 0x5F4
    unsigned char pad5F8[0x2F4];
    int itemState; // 0x8EC
    unsigned char pad8F0[0x14];
    float condition; // 0x904
};

extern "C" {
int fn_800FC3B8(int arg);
void fn_800FC764(int id, int value);
void fn_802C24E4(int* a, int* b);
void fn_8013F4B8(void* owner, int handle);
void fn_80090390(void* p, int value);
void fn_801D55CC(void* p);
void fn_800B20B8(void* owner, int handle);

extern float lbl_804D77D8;
extern float lbl_804D77DC;
extern float lbl_804D77E0;
extern float lbl_804D77E4;
}

int Fn800F8A98(unsigned long id);
int Fn800F840C(int itemState, int id, int arg);
int ModelBoneName(int model, int bone);
void ExtraTrailApply(ExtraHolder* holder, ExtraMsg* msg);
void ExtraTrailAttach(ExtraHolder* holder);

// Message handler for the item's effect: consumes the message when the owner
// still has condition left and the message's position is within blast range,
// then dispatches on the message's item-state type. Returns 1 when consumed.
int ItemHitMessage(EffectOwner* owner, EffectMsg* msg)
{
    EffectReply reply;
    int id;
    float dy;
    float dx;
    float dz;

    if (owner->condition <= lbl_804D778C) {
        return 0;
    }
    dy = (float)owner->posY - msg->y;
    dx = (float)owner->posX - msg->x;
    dz = (float)owner->posZ - msg->z;
    if (fabs(dy * dy + dx * dx + dz * dz) > lbl_804D77D8) {
        return 0;
    }
    if (msg->type == 0) {
        return 0;
    }
    id = Fn800F8A98((unsigned long)msg->type);
    if (id == 0) {
        return 0;
    }
    switch (msg->type) {
    case 0xA7D6DA15:
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        reply.value = lbl_804D77DC;
        reply.handle = msg->handle;
        owner->dispatch(0xC0A, &reply);
        return reply.result != 1;
    case 0xECA3D5B9:
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        reply.value = lbl_804D779C;
        reply.handle = msg->handle;
        owner->dispatch(0xC0A, &reply);
        return reply.result != 1;
    case 0xECA3D5F7:
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        reply.value = lbl_804D77A8;
        reply.handle = msg->handle;
        owner->dispatch(0xC0A, &reply);
        return reply.result != 1;
    case 0xECA3D57B:
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        reply.value = lbl_804D77C4;
        reply.handle = msg->handle;
        owner->dispatch(0xC0A, &reply);
        return reply.result != 1;
    case 0x114F1979:
    case 0xE54B6BDB:
    case 0xDBEC014D:
    case 0xFAD5AC93:
    case 0x154F7C95:
    case 0x21425B82:
    case 0x6B4B4A10:
    case 0x6BA1BC35:
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        Fn800F840C(owner->itemState, id, msg->arg);
        return 1;
    case 0x1F5A5ED3:
        if (fn_80116E08(owner) != 0) {
            return 0;
        }
        Fn800F840C(owner->itemState, id, msg->arg);
        return 1;
    case 0x8B267329:
    case 0xDC95C315:
    case 0x32819B90:
    case 0x20A5AA68:
    case 0x3BFF94C2:
        if (fn_80116E08(owner) == 0) {
            return 0;
        }
        Fn800F840C(owner->itemState, id, msg->arg);
        return 1;
    default:
        return 0;
    }
}

// Message handler for item wear: applies the message's condition loss to the
// owner, tears the item down when its condition runs out and forwards the
// message to the extra-trail holder. Returns 1 when consumed.
int ItemWearMessage(WearOwner* owner, ExtraMsg* msg)
{
    UnlockMsg um;
    float value;
    int team;
    int asset;
    int handle;
    int ownerId;

    if (owner == 0) {
        return 0;
    }
    msg->active = 1;
    value = lbl_804D778C;
    if (QueryName(0x4B000000, 0x0E83B88B) == 0) {
        if (msg->handle != OwnerId((Owner*)owner)) {
            team = PlayerTeam(HandleToPlayer(msg->handle));
            if (PlayerTeam(HandleToPlayer(OwnerId((Owner*)owner))) == team) {
                return 1;
            }
        }
    }
    asset = (int)AssetFind(0xF1754BF2, 0x21C5E0);
    if (ModelBoneName(owner->model, msg->bone) == asset) {
        msg->f0C = lbl_804D77E0 * msg->f0C;
    }
    if (fn_800F7C64(owner->itemState, 0x32819B90) != 0 ||
        fn_800FC3B8(0x13) != 0) {
        msg->f0C = lbl_804D77A8 * msg->f0C;
    }
    switch (msg->type) {
    case 0x2EFC66CA:
        if (fn_80116E08(owner) != 0) {
            fn_80116E30(owner);
        }
        break;
    case 0xEA31D667:
        if (fn_80116E08(owner) != 0) {
            value = msg->f0C;
        }
        break;
    case 0xB9FC42F5:
        if (fn_80116E08(owner) != 0) {
            value = msg->f0C;
        }
        break;
    case 0x8FC151EE:
    case 0x0021EE36:
        if (fn_80116E08(owner) != 0) {
            value = msg->f0C;
        }
        break;
    case 0x672F35C2:
        if (owner->condition <= lbl_804D778C) {
            fn_800FC764(OwnerId((Owner*)owner), 0);
            return 1;
        }
        break;
    case 0x403548D0:
        if (fn_80116E08(owner) == 0) {
            value = msg->f0C * lbl_804D77A8;
        }
        break;
    case 0x75715902:
        if (fn_80116E08(owner) != 0) {
            fn_802C24E4(msg->pos0, msg->pos1);
            value = msg->f0C;
        }
        break;
    default:
        value = msg->f0C;
        break;
    }
    if (owner->condition <= lbl_804D778C) {
        if (owner->trail != 0) {
            ExtraTrailApply((ExtraHolder*)owner->trail, msg);
        }
        return 1;
    }
    if (value > lbl_804D77E4) {
        owner->condition = owner->condition - value;
        owner->unk214 = msg->bone;
        if (owner->condition > lbl_804D778C) {
            fn_800A7A54(owner->cond, 4, 1);
        } else {
            if (HasCondition(owner->cond, 0xC) != 0) {
                fn_800A7A54(owner->cond, 0xC, 0);
                fn_80090390((void*)owner->unkD4, 0);
            }
            fn_800A7A54(owner->cond, 9, 1);
            if (QueryName(0x4B000000, 0x2D88B734) != 0) {
                fn_8013F4B8(owner, *(int*)((char*)owner + 0x38));
            }
            handle = msg->handle;
            ownerId = OwnerId((Owner*)owner);
            if (msg->handle != -1) {
                um.unk4 = handle;
                um.flags = ownerId;
                um.key = 0x1FF464;
                SendMsg(msg->handle, 0xC14, &um);
            }
            msg->active = 0;
            if (owner->trail != 0) {
                ExtraTrailAttach((ExtraHolder*)owner->trail);
                if (owner->trailOwner != 0) {
                    fn_801D55CC(owner->trailOwner);
                    owner->trailOwner = 0;
                }
            }
        }
        if (fn_80116E08(owner) == 0) {
            *(float*)((char*)owner->state + 8) = owner->condition;
        } else {
            *(float*)((char*)owner->state + 0x60) = owner->condition;
        }
        if (msg->type != 0x6CA5F172) {
            fn_800B20B8(owner, msg->handle);
            if (owner->trail != 0) {
                ExtraTrailApply((ExtraHolder*)owner->trail, msg);
            }
        }
    }
    return 1;
}

// TODO fn_8011F66C (0x660 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8011F66C(void) {}

// State-machine callback: writes a type code into the item's info block.
int ItemSetInfoType(int value, void* arg)
{
    EventOwner* o;

    if (arg == 0) {
        return 0;
    }
    o = (EventOwner*)arg;
    o->info->type = value;
    return 1;
}

// TODO fn_8011FCEC (0x674 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8011FCEC(void) {}

// Per-frame item update: runs the current item state's callbacks, keeps the
// zone and the attachment position in sync and refreshes the carried state.
int ItemUpdate(EventOwner* o)
{
    EventInfo* info;
    int active;
    int next;

    if (o == 0) {
        return 0;
    }
    info = o->info;
    if (o->f904 > lbl_804D778C) {
        if (info->unk0 == -1) {
            info->unk0 = ItemSelectState(o);
            lbl_8039EDD8[info->unk0].enter(o);
        }
        if (o->state->fBC == -1) {
            fn_800B42E0((char*)o + 0x84, o->handle, lbl_804D77E8, 1, 1);
        } else {
            fn_800B42E0((char*)o + 0x84, o->handle, lbl_804D77E8, 1, 2);
        }
        ZoneUpdate((char*)o->state + 0x1E8, (IVec3*)((char*)o + 0x24));
        active = lbl_8039EDD8[info->unk0].check(o);
        fn_80162658(o, (char*)o + 0x894);
        if (active != 0) {
            next = ItemSelectState(o);
            if (lbl_8039EDD8[info->unk0].update(o) != 0) {
                lbl_8039EDD8[next].enter(o);
            }
        }
        if (fn_800BAF40(o) == -1) {
            o->f288 = 0;
        } else {
            o->f288 = 3;
        }
    } else {
        fn_80162658(o, (char*)o + 0x894);
    }
    return 1;
}

// Destructor: broadcasts that the item is being removed, then clears the
// state machine, frees the info block and drops the item's handle.
void ItemDestroy(void* arg)
{
    EventOwner* o;
    EventInfo* info;
    UnlockMsg um;

    if (arg == 0) {
        return;
    }
    o = (EventOwner*)arg;
    um.flags = o->handle;
    um.key = 0xEB4A0875;
    um.unk4 = 0;
    SendMsg(0x53000000, 0xC14, &um);
    info = o->info;
    ClearStateMachine(o);
    VFree(info);
    o->info = 0;
    o->f104 = 0;
}

// Constructor: looks up the item asset, allocates the info block, installs
// the item callbacks and broadcasts the item's unlock event.
void ItemCreate(void* arg)
{
    EventOwner* o;
    EventInfo* info;
    UnlockMsg um;

    if (arg == 0) {
        return;
    }
    o = (EventOwner*)arg;
    lbl_804D2E10 = (unsigned long)AssetFind(0xF2E2BEB8, 0xEDA570C7);
    info = (EventInfo*)VAlloc(0x10);
    if (info == 0) {
        o->info = 0;
        return;
    }
    o->info = info;
    o->unkFC = fn_800CE75C;
    o->unk604 = fn_800CEE08;
    o->f904 = (float)*(int*)(*(int*)((char*)o + 0xC4) + 0x30);
    o->filter = IsSameTeamOtherActor;
    info->unk0 = -1;
    info->f0C = lbl_804D7788;
    info->target = -1;
    info->type = 0;
    if (fn_80117D80(o) == 0) {
        VFree(info);
        o->info = 0;
        return;
    }
    o->state->setInfoType = ItemSetInfoType;
    o->state->owner = o;
    fn_800A7A54(o->unk270, 1, 0);
    fn_800A7B08(o->unk270, 6, 0);
    fn_800A7B08(o->unk270, 0, 0);
    fn_800A7B08(o->unk270, 0x23, 1);
    um.flags = o->handle;
    um.key = 0x2840341C;
    um.unk4 = 3;
    SendMsg(0x53000000, 0xC14, &um);
    ZoneFlagSet((ZoneList*)o->zoneFlags, 1, 0);
}

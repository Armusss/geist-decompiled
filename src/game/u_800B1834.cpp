// u_800B1834.cpp: owner behaviour-slot and animation bookkeeping.
// Drafted from the retail assembly; type and member names are descriptive guesses.

#include "game/types.h"
#include "game/string.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed int    s32;

extern "C" int SendMsg(int to, int msg, void* data);

class Owner;

int   OwnerId(Owner* owner);
float OwnerValue(Owner* owner, void* actor);
void  BitClear(u32* flags, int bit);
void  ActorPlayAnim(int actor, u32 anim, int arg);

void VFree(void* p);
void TalkFree();
void TalkInit();

extern "C" void fn_800A4F28(void);
extern "C" void fn_800A51B8(void);
extern "C" void fn_800A5254(void);
extern "C" void fn_800A54E8(void);
extern "C" void fn_80026660(int a, void* callback, int c);
extern "C" void fn_800BF428(void);
extern "C" void fn_800BF4B0(void);
extern "C" void fn_8029B7D0(void);
extern "C" void fn_8029B920(void);
extern "C" void fn_800B2E34(void* object, int a, int b);
extern "C" void fn_800B3164(void* object, int a, int b);
extern "C" void fn_800BAA14(void);
extern "C" void fn_800AB47C(void);

extern "C" void fn_800B2BA4(void);
extern "C" void fn_800B4614(void);
extern "C" void fn_800B4EFC(void);
extern "C" void fn_800B98F8(void);
extern "C" void fn_800B9A8C(void);
extern "C" int  fn_800B84E4(void* a, void* b, float* outDist);
extern "C" int  fn_800A37CC(void* object);
void OwnerUpdateSlot1(Owner* owner);
extern "C" int  fn_800A3758(Owner* owner);
int OwnerFindSoundTarget(Owner* owner, int* actorId, float* pos, float* outPos,
                         int* outPriority, u32* outFlags, u32 requiredFlags,
                         u32 excludedFlags);
int OwnerFindSightTarget(Owner* owner, int* actorId, float* outPos, int* unk0C,
                         u32* unk14, int a, int b);
extern "C" void fn_8016D68C(void* dst, void* src);
extern "C" void* memset(void* dst, int value, u32 size);
extern "C" void FileFlushBuffer(void* dst, u32 size);
int HasCondition(void* object, int id);
extern "C" void fn_800BA7AC(u8* begin, u8* end);
extern "C" void fn_800BA7CC(u8* begin, u8* end);
int GroupMessageHandler(int msg, void* data);
extern float lbl_804D6DA0;
extern float lbl_804D6DA4;
extern float lbl_804D6D90;
extern float lbl_804D6D94;

extern "C" void* fn_8028B3BC(Buffer* buf, u8* end, u32 len);
extern "C" int   rand(void);
extern "C" void fn_8028B960(void* object);
extern "C" void fn_800A3AF4(float value);
extern "C" void fn_800B9528(float value);
extern "C" void fn_800B4D94(float value);
extern "C" void fn_800B4414(float value);
extern "C" void fn_800B29FC(float value);
extern "C" void TalkUpdate(float value);
extern "C" void fn_8029B85C(float value);
extern char   lbl_8039B708[];
extern float  lbl_804D6E48;
extern double lbl_804D6E50;

// Group table shared by the 0x800B8Exx/0x800B9xxx helpers.
extern u32  lbl_8039B728[];    // 13-entry global table
extern u32  lbl_804D2B90[2];   // 60-bit global flag set
extern u32* lbl_804D4830;      // group table base
extern int  lbl_804D4834;
extern int  lbl_804D4838;      // number of groups
extern int  lbl_804D4840;      // selected group index

extern "C" void fn_800B8ED0(void* a, void* b, void* c, void* d, void* e, void* f);
extern "C" void fn_800B912C(void* a, void* b, void* c, void* d, void* e, void* f);

// Per-group record, stride 0x78.
struct GroupData {
    s32   field00;   // 0x00
    u32   id;        // 0x04
    u32   flag;      // 0x08
    u32   field0C;   // 0x0c
    float x;         // 0x10
    float y;         // 0x14
    float z;         // 0x18
    u8    bone[0x10];// 0x1c
    u32   field2C;   // 0x2c
};

struct GroupEntry {
    u8         pad038[0x38];
    u32        field38;  // 0x38
    u8         pad03C[0x14];
    s32        field50;  // 0x50
    s32        field54;  // 0x54
    s32        field58;  // 0x58
    s32        field5C;  // 0x5c
    s32        field60;  // 0x60
    s32        field64;  // 0x64
    GroupData* data;     // 0x68
    u32        flags;    // 0x6c
    u32        asset;    // 0x70
    u32        field74;  // 0x74
};

class Actor;

void BoneGetMatrix(Mtx34* out, void* bone);
void PlacementToMatrix(Mtx34* out, PlaceF* in);
void ActorFn800231A8(Actor* actor);
void ActorFn8002378C(Actor* actor, int a, int b, int c, int d, int e, int f);
void ActorRefreshHostState(Actor* actor, int state);
void ObjSendC00(void* object, int value);
int  SelectionSetTest(u32 id);
extern "C" u32 fn_800BF2D0(void* p);

u32 AssetFind(u32 tag, u32 id);

extern u32   lbl_80344930[];   // animation ids, indexed by animation slot
extern float lbl_804D6D30;     // slot owner-value thresholds / timers
extern float lbl_804D6D34;
extern float lbl_804D6D38;
extern float lbl_804D6D3C;
extern float lbl_804D6D40;

// One of four behaviour slots, stride 0x28, starting at owner+0x144.
struct OwnerSlot {
    u32   flags;      // 0x00
    float unk04;      // 0x04
    float timer;      // 0x08
    s32   unk0C;      // 0x0c
    s32   state;      // 0x10
    s32   unk14;      // 0x14
    s32   actorId;    // 0x18
    float posX;       // 0x1c
    float posY;       // 0x20
    float posZ;       // 0x24
};

// One of four animation entries, stride 0xc, starting at owner+0x1e4.
struct AnimEntry {
    s32   actorId;    // 0x00
    s32   anim;       // 0x04
    float time;       // 0x08
};

class Owner {
public:
    u8        pad000[0x24];
    u32       cellX;             // 0x24  integer cell coordinates, converted to
    u32       cellY;             // 0x28  float when the owner is searched for
    u32       cellZ;             // 0x2c
    u8        pad030[0x54];
    float     posX;              // 0x84
    float     posY;              // 0x88
    float     posZ;              // 0x8c
    u8        pad090[0xA8];
    float     dt;                // 0x138
    u8        pad13C[0x8];
    OwnerSlot slots[4];          // 0x144
    AnimEntry anims[4];          // 0x1e4
    u8        pad214[0x28];
    s32       field23c;          // 0x23c
    u8        pad240[0x394];
    float     radius;            // 0x5d4
    u8        pad5D8[0x2C];
    int     (*callback)(Owner*, int); // 0x604
    u8        pad608[0x300];
    void*     target;            // 0x908
};

// Behaviour slot `index` of `owner`.
static inline OwnerSlot* GetSlot(Owner* owner, int index)
{
    return (OwnerSlot*)((u8*)owner + index * 0x28 + 0x144);
}

// Payload of the 0x400 owner query.
struct OwnerMsg {
    void* actor;      // 0x00
    u32   unk04;      // 0x04
    u32   unk08;      // 0x08
    int   unk0C;      // 0x0c
};

// Full 0x20-byte payload of the 0x400 owner query.
struct OwnerQuery {
    void* actor;      // 0x00
    u32   unk04;      // 0x04
    u32   unk08;      // 0x08
    int   unk0C;      // 0x0c
    u8    pad10[0x10];
};

extern "C" void fn_800B1AB8(float time, Owner* owner, int slotIndex, int anim);

void OwnerUpdateSlots(Owner* owner)
{
    if (owner == 0) {
        return;
    }

    int id = owner->field23c;
    if (id != -1) {
        OwnerSlot* slot = &owner->slots[2];
        OwnerMsg msg;
        int ok = SendMsg(id, 0x400, &msg);
        if (ok != 0 && msg.actor != 0) {
            float value = OwnerValue(owner, msg.actor);
            if (slot->actorId == id && value > lbl_804D6D30) {
                BitClear(&slot->flags, 0);
                slot->actorId = -1;
                slot->state = 0;
                owner->slots[1].flags |= 2;
            } else if (slot->actorId != id && value < lbl_804D6D34) {
                slot->state = 2;
                slot->timer = lbl_804D6D38;
                slot->actorId = id;
                slot->flags |= 2;
                slot->flags |= 1;
                slot->flags |= 0x10;
                slot->unk0C = 1;
                fn_800B1AB8(lbl_804D6D3C, owner, 2, 1);
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        OwnerSlot* slot = &owner->slots[i];
        if ((slot->flags & 0x10) == 0) {
            BitClear(&slot->flags, 0);
            slot->actorId = -1;
            slot->state = 0;
        } else if ((slot->flags & 1) != 0) {
            if (slot->actorId == -1 || owner->callback == 0 || owner->callback(owner, slot->actorId) != 0) {
                OwnerMsg msg;
                SendMsg(slot->actorId, 0x400, &msg);
                u32 kind = (u32)slot->actorId >> 24;
                if ((kind == 2 || kind == 0x13) && msg.unk0C == 0) {
                    BitClear(&slot->flags, 2);
                    BitClear(&slot->flags, 0);
                    BitClear(&slot->flags, 1);
                } else if ((slot->flags & 4) != 0) {
                    slot->timer -= owner->dt;
                    if (slot->timer <= lbl_804D6D40) {
                        BitClear(&slot->flags, 2);
                        BitClear(&slot->flags, 0);
                        BitClear(&slot->flags, 1);
                    }
                }
            } else {
                BitClear(&slot->flags, 2);
                BitClear(&slot->flags, 0);
                BitClear(&slot->flags, 1);
            }
        }
    }
}

// Payload of the 0x41e owner-resolution query.
struct ActorQuery {
    int result;     // 0x00 written by the handler
    u32 hash;       // 0x04
    int unk;        // 0x08
    int ownerId;    // 0x0c
};

// Resolve an actor id to its owning actor through message 0x41e.
static inline int ResolveOwner(int actor)
{
    ActorQuery q;
    q.ownerId = 0;
    q.hash = 0x6C47D293;
    q.unk = 0;
    int ret = SendMsg(actor, 0x41e, &q);
    if (ret != 0 && q.ownerId != -1) {
        return q.ownerId;
    }
    return actor;
}

// Highest animation index that the slot may still request.
static inline int SlotAnimLimit(Owner* owner, int slotIndex)
{
    int limit;
    if (owner != 0) {
        OwnerSlot* slot = &owner->slots[slotIndex];
        int id = slot->actorId;
        if (id == -1) {
            limit = 4;
        } else {
            id = ResolveOwner(id);
            int i;
            for (i = 0; i < 4; i++) {
                if (owner->anims[i].actorId == id) {
                    limit = owner->anims[i].anim;
                    if (slot->unk0C < limit) {
                        limit = slot->unk0C;
                    }
                    break;
                }
            }
            if (i == 4) {
                limit = slot->unk0C;
            }
        }
    } else {
        limit = 4;
    }
    return limit;
}

extern "C" void fn_800B1AB8(float time, Owner* owner, int slotIndex, int anim)
{
    bool found = false;
    if (owner == 0) {
        return;
    }

    OwnerSlot* slot = &owner->slots[slotIndex];
    int actorId = slot->actorId;

    if (SlotAnimLimit(owner, slotIndex) > anim) {
        ActorPlayAnim(OwnerId(owner), lbl_80344930[anim], slot->actorId);
    }

    actorId = ResolveOwner(actorId);

    int i;
    for (i = 0; i < 4; i++) {
        if (owner->anims[i].actorId == actorId) {
            if (anim < owner->anims[i].anim) {
                owner->anims[i].anim = anim;
            }
            found = true;
            owner->anims[i].time = time;
            break;
        }
    }

    if (!found) {
        for (i = 0; i < 4; i++) {
            if (owner->anims[i].actorId == -1) {
                owner->anims[i].actorId = actorId;
                owner->anims[i].anim = anim;
                owner->anims[i].time = time;
                owner->anims[(i + 1) % 4].actorId = -1;
                return;
            }
        }
    }
}

// Registry of active owner behaviour slots (8 entries, 16 bytes each).
struct OwnerSlotEntry {
    s32 id;    // 0x00
    u32 a;     // 0x04  callback
    u32 b;     // 0x08  callback argument
    s32 c;     // 0x0c  priority
};

extern OwnerSlotEntry lbl_80449568[8];
extern OwnerSlot      lbl_804495E8;   // shadow copy of the last added slot
extern float          lbl_804D2B78;   // time stamp of the last registration
extern float          lbl_804D6D44;

// Drop every registry entry that belongs to this owner.
void OwnerSlotRemove(Owner* owner)
{
    OwnerSlotEntry* e;
    int i;

    if (owner == 0) {
        return;
    }
    e = lbl_80449568;
    for (i = 0; i < 8; i++, e++) {
        if (e->id == OwnerId(owner)) {
            e->id = -1;
            e->a = 0;
            e->b = 0;
            e->c = 0;
        }
    }
}

// Register one of the owner's behaviour slots in the global slot registry,
// then attach the three caller values to it.
void OwnerSlotAdd(Owner* owner, int index, u32 a, u32 b, u32 c)
{
    if (owner == 0) {
        return;
    }

    lbl_804495E8 = owner->slots[index];

    for (int i = 0; i < 8; i++) {
        OwnerSlotEntry* entry = &lbl_80449568[i];
        if (entry->id == -1 || entry->id == OwnerId(owner)) {
            lbl_804D2B78 = lbl_804D6D44;
            entry->id = OwnerId(owner);
            entry->a = a;
            entry->b = b;
            entry->c = c;
            return;
        }
    }
}

// Value stored in behaviour slot `index` at owner+0x158 (slot field 0x14).
extern "C" u32 fn_800B1ED4(Owner* owner, int index)
{
    if (owner == 0) {
        return 0;
    }
    OwnerSlot* slot = GetSlot(owner, index);
    return slot->unk14;
}

// Nonzero when behaviour slot `index` does not have its bit-1 flag set.
extern "C" u32 fn_800B1EF4(Owner* owner, int index)
{
    if (owner == 0) {
        return 0;
    }
    return !(GetSlot(owner, index)->flags & 2);
}

// Per-behaviour-slot animation entry: animation ids by slot index.
extern s32 lbl_80344920[];

// Payload of the 0x41e owner-resolution query. The retail code passes a
// four-word record; the handler fills the last word.
struct TypeQuery {
    u32 result;   // 0x00 out
    u32 hash;     // 0x04 type-name hash
    u32 zero;     // 0x08
    u32 name;     // 0x0c resolved actor id
};

// Highest animation index the slot may still play: the smaller of the slot's
// own limit and the animation registered for the resolved actor.
static inline int SlotAnimLimit2(Owner* owner, int index)
{
    if (owner == 0) {
        return 4;
    }

    OwnerSlot* slot = &owner->slots[index];
    s32 id = slot->actorId;
    if (id == -1) {
        return 4;
    }

    TypeQuery q;
    q.name = 0;
    q.hash = 0x6C47D293;
    q.zero = 0;
    int ok = SendMsg(id, 0x41e, &q);
    if (ok != 0 && q.name != 0xffffffff) {
        id = q.name;
    }

    for (int i = 0; i < 4; i++) {
        if (owner->anims[i].actorId == id) {
            int anim = owner->anims[i].anim;
            if (slot->unk0C < anim) {
                return slot->unk0C;
            }
            return anim;
        }
    }
    return slot->unk0C;
}

// Fire the queued animation of behaviour slot `index`: play the slot's
// animation id when it has one, then play the slot's current animation.
extern "C" void fn_800B1F20(Owner* owner, int index)
{
    if (owner == 0) {
        return;
    }

    OwnerSlot* slot = &owner->slots[index];
    if ((slot->flags & 2) == 0) {
        return;
    }
    BitClear(&slot->flags, 1);

    s32 anim = lbl_80344920[index];
    if (anim != -1) {
        ActorPlayAnim(OwnerId(owner), anim, slot->actorId);
    }

    int limit = SlotAnimLimit2(owner, index);
    ActorPlayAnim(OwnerId(owner), lbl_80344930[limit], slot->actorId);
}

extern float lbl_804D6D48;

// Point behaviour slot 1 at a new target actor: resolve the target through
// the 0x41e query, ignore the update when the target is unchanged, otherwise
// start the slot's "new target" timer.
// fn_800B20B8
extern "C" void fn_800B20B8(Owner* owner, u32 data)
{
    if (owner == 0) {
        return;
    }

    OwnerSlot* slot = &owner->slots[1];

    TypeQuery q;
    q.name = 0;
    q.hash = 0x273A5D;
    q.zero = 0;
    int ok = SendMsg((s32)data, 0x41e, &q);
    void* target = (void*)q.name;
    if (ok == 0) {
        target = 0;
    }

    if ((data >> 24) == 0x13) {
        void* cur = owner->target;
        if (cur == 0) {
            return;
        }
        if (target == 0) {
            return;
        }
        if (cur == target) {
            return;
        }
    } else if ((data >> 24) == 2) {
        if (owner->target == target) {
            return;
        }
    }

    if ((slot->flags & 1) != 0 && (slot->flags & 0x10) != 0) {
        return;
    }

    slot->state = 2;
    slot->actorId = (s32)data;
    slot->timer = lbl_804D6D48;
    slot->flags |= 2;
    slot->flags |= 1;
    BitClear(&slot->flags, 4);
    slot->unk0C = 1;
}

// Look up the animation entry registered for the resolved actor and report
// its animation id through the out-parameter. Returns 1 when it was found.
// fn_800B21EC
extern "C" int fn_800B21EC(Owner* owner, int actorId, int* limit)
{
    if (owner == 0) {
        return 4;
    }

    int id = ResolveOwner(actorId);
    for (int i = 0; i < 4; i++) {
        if (owner->anims[i].actorId == id) {
            if (limit != 0 && *limit > owner->anims[i].anim) {
                *limit = owner->anims[i].anim;
            }
            return 1;
        }
    }
    return 0;
}

// Highest animation index that the given behaviour slot may still play: the
// smaller of the slot's own limit and the animation of the resolved actor.
extern "C" int fn_800B22F8(Owner* owner, int slotIndex)
{
    if (owner == 0) {
        return 4;
    }

    OwnerSlot* slot = &owner->slots[slotIndex];
    s32 id = slot->actorId;
    if (id == -1) {
        return 4;
    }

    id = ResolveOwner(id);
    for (int i = 0; i < 4; i++) {
        if (owner->anims[i].actorId == id) {
            if (owner->anims[i].anim < slot->unk0C) {
                return owner->anims[i].anim;
            }
            return slot->unk0C;
        }
    }
    return slot->unk0C;
}

// Store `value` into behaviour slot `index`, but only when it lies above the
// shared threshold. Returns 1 when the slot was written.
extern "C" int fn_800B2418(float value, Owner* owner, int index)
{
    if (owner == 0 || value <= lbl_804D6D40) {
        return 0;
    }
    OwnerSlot* slot = &owner->slots[index];
    slot->timer = value;
    slot->unk04 = value;
    return 1;
}

// Nonzero when behaviour slot `index` has both the active and the timed flag.
extern "C" u32 fn_800B2454(Owner* owner, int index)
{
    if (owner == 0) {
        return 0;
    }
    bool armed = false;
    u32 flags = ((OwnerSlot*)((u8*)owner + index * 0x28 + 0x144))->flags;
    if ((flags & 1) != 0 && (flags & 4) != 0) {
        armed = true;
    }
    return armed;
}

// Behaviour slot as seen by the position query: flags at 0x00, the actor id
// at 0x18 and the slot's stored position at 0x1c.
struct QuerySlot {
    u32   flags;     // 0x00
    u8    pad04[0x14];
    s32   actorId;   // 0x18
    float x;         // 0x1c
    float y;         // 0x20
    float z;         // 0x24
};

struct QuerySlotTable {
    u8        pad000[0x144];
    QuerySlot slots[4];
};

// Fixed-point actor position at 0x24.
struct QueryActor {
    u8  pad024[0x24];
    s32 x;   // 0x24
    s32 y;   // 0x28
    s32 z;   // 0x2c
};

// Read the world position of behaviour slot `index` into `out`. Returns 1 when
// the slot is active, 0 otherwise; the position is written in both cases.
int OwnerSlotGetPosition(int owner, int index, float* out)
{
    if (owner == 0 || out == 0 || index >= 4 || index < 0) {
        return 0;
    }

    QuerySlot* slot = &((QuerySlotTable*)owner)->slots[index];
    if ((slot->flags & 1) != 0) {
        if (out != 0) {
            if (index == 3 || slot->actorId == -1) {
                out[0] = slot->x;
                out[1] = slot->y;
                out[2] = slot->z;
            } else {
                int msg[16];
                int ok = SendMsg(slot->actorId, 0x400, msg);
                if (ok == 0 || msg[0] == 0) {
                    out[0] = slot->x;
                    out[1] = slot->y;
                    out[2] = slot->z;
                } else {
                    QueryActor* pos = (QueryActor*)msg[0];
                    out[0] = (float)pos->x;
                    out[1] = (float)pos->y;
                    out[2] = (float)pos->z;
                }
            }
        }
        return 1;
    }

    QueryActor* pos = (QueryActor*)owner;
    out[0] = (float)pos->x;
    out[1] = (float)pos->y;
    out[2] = (float)pos->z;
    return 0;
}

// Read the actor id backing behaviour slot `index` through the 0x400 query.
// Returns 1 and stores the actor when the slot is active, 0 otherwise.
extern "C" int fn_800B261C(u32 owner, int index, int* out)
{
    if (owner == 0) {
        return 0;
    }

    QuerySlot* slot = &((QuerySlotTable*)owner)->slots[index];
    if ((slot->flags & 1) != 0 && out != 0) {
        void* msg[8];
        int ok = SendMsg(slot->actorId, 0x400, msg);
        if (ok != 0 && msg[0] != 0) {
            *out = (int)msg[0];
            return 1;
        }
    }

    if (out != 0) {
        *out = 0;
    }
    return 0;
}

// OwnerSelectTargetSlot: rank the four behaviour slots and return the index of
// the best candidate. A lower slot kind wins; ties are broken by the slot
// flags, by whether the animation entry was found for the slot's actor, and by
// the squared distance between the slot position and the owner position.
// Kept as fn_800B26B8: other units still call it by this placeholder name.
extern "C" int fn_800B26B8(Owner* owner)
{
    if (owner == 0) {
        return 4;
    }

    float bestDist = lbl_804D6D38;
    int bestKind = 6;
    int result = 4;
    OwnerSlot* bestSlot = 0;
    int bestFound = 0;

    for (int i = 0; i < 4; i++) {
        OwnerSlot* slot = &owner->slots[i];
        int kind = slot->unk0C;
        if ((slot->flags & 1) == 0) {
            continue;
        }
        if (slot->actorId != -1 && owner->callback != 0 && owner->callback(owner, slot->actorId) == 0) {
            continue;
        }

        float dx = slot->posX - owner->posX;
        float dy = slot->posY - owner->posY;
        float dz = slot->posZ - owner->posZ;
        u32 mag = (u32)(dx * dx + dy * dy + dz * dz);
        float dist = (float)mag;

        if (kind > bestKind) {
            continue;
        }

        if (kind == 1) {
            if (dist < owner->radius * owner->radius) {
                if (dist < bestDist) {
                    result = i;
                    bestKind = kind;
                    bestSlot = slot;
                    bestFound = 0;
                    bestDist = dist;
                }
                continue;
            }
        }

        int found = fn_800B21EC(owner, slot->actorId, &kind);

        if ((found == 0 || kind != 5) &&
            (kind != bestKind ||
             (((found != 0 || bestFound != 1) &&
               ((slot->flags & 2) != 0 || (bestSlot->flags & 2) == 0)) &&
              ((slot->flags & 4) == 0 || (bestSlot->flags & 4) != 0))) &&
            (kind < bestKind ||
             ((((slot->flags & 4) == 0) && ((bestSlot->flags & 4) != 0)) ||
              (found == 1 && bestFound == 0) ||
              (((slot->flags & 2) != 0 && (bestSlot->flags & 2) == 0) ||
               (dist < bestDist))))) {
            bestDist = dist;
            bestFound = found;
            bestSlot = slot;
            bestKind = kind;
            result = i;
        }
    }
    return result;
}

// Age the four animation entries: each entry's timer counts down with the
// owner frame time and, once it expires, the entry's actor reference is
// dropped unless the actor is still alive.
int OwnerExpireAnimActors(Owner* owner)
{
    if (owner == 0) {
        return 0;
    }

    float limit = lbl_804D6D40;
    for (int i = 0; i < 4; i++) {
        AnimEntry* entry = &owner->anims[i];
        if (entry->time > limit) {
            entry->time -= owner->dt;
            if (entry->time <= limit) {
                entry->actorId = -1;
                continue;
            }
        }

        if (((u32)entry->actorId >> 24) == 0x13) {
            int actor = 0;
            SendMsg(entry->actorId, 0x416, &actor);
            if (actor == 0) {
                entry->actorId = -1;
            } else if (HasCondition((u8*)actor + 0x270, 0xb)) {
                entry->actorId = -1;
            }
        }
    }
    return 1;
}

// Query the actor behind `handle` and report whether it is still active.
extern "C" int fn_800B2D00(void* unused, u32 handle)
{
    u8 query[0x20];
    memset(query, 0, sizeof(query));
    if (SendMsg((int)handle, 0x400, query) == 0 || *(u8**)query == 0) {
        return 0;
    }
    u8* actor = *(u8**)query;
    switch (handle >> 24) {
    case 0x13:
        if (HasCondition(actor + 0x270, 0x0b) || *(int*)(query + 0x0c) == 0) {
            return 0;
        }
        return 1;
    case 0x02:
        return *(int*)(query + 0x0c) != 0;
    }
    return 1;
}

// Reset the instance if it is ready, then report success.
extern "C" int fn_800B2DC8(void* object)
{
    if (object == 0) {
        return 0;
    }
    if (fn_800A37CC(object) != 0) {
        fn_800B2E34(object, 0, 0);
        fn_800B3164(object, 0, 0);
        OwnerUpdateSlot1((Owner*)object);
    }
    return 1;
}

// Refresh behaviour slot 3 from the nearest target found by the owner search;
// while the target stays valid the slot is kept, otherwise its timer runs down
// and the slot is cleared when it expires.
// fn_800B2E34
extern "C" void fn_800B2E34(void* owner_, int a, int b)
{
    Owner* owner = (Owner*)owner_;
    if (owner == 0) {
        return;
    }

    OwnerSlot* slot = &owner->slots[3];

    float pos[3];
    pos[0] = (float)owner->cellX;
    pos[1] = (float)owner->cellY;
    pos[2] = (float)owner->cellZ;

    int active;
    if (slot->actorId == -1 || owner->callback == 0 || owner->callback(owner, slot->actorId) != 0) {
        active = 1;
    } else {
        active = 0;
    }

    int foundActorId;
    float foundPos[3];
    int foundUnk0C;
    u32 foundUnk14;

    int result = OwnerFindSoundTarget(owner, &foundActorId, pos, foundPos,
                                      &foundUnk0C, &foundUnk14, a, b);

    if (result != 0) {
        slot->state = 1;
        if ((slot->flags & 1) == 0) {
            slot->flags |= 1;
            slot->flags |= 2;
            BitClear(&slot->flags, 2);
            slot->actorId = foundActorId;
            slot->unk0C = foundUnk0C;
            slot->timer = lbl_804D6D40;
            slot->unk14 = foundUnk14;
            slot->posX = foundPos[0];
            slot->posY = foundPos[1];
            slot->posZ = foundPos[2];
        } else if (slot->actorId == foundActorId && slot->unk14 == foundUnk14 &&
                   slot->unk0C == foundUnk0C && active) {
            BitClear(&slot->flags, 2);
            slot->timer = slot->unk04;
            slot->posX = foundPos[0];
            slot->posY = foundPos[1];
            slot->posZ = foundPos[2];
        } else if (foundUnk0C > slot->unk0C) {
            if (slot->timer <= lbl_804D6D40 || !active) {
                slot->flags |= 1;
                slot->flags |= 2;
                BitClear(&slot->flags, 2);
                slot->actorId = foundActorId;
                slot->unk0C = foundUnk0C;
                slot->timer = lbl_804D6D40;
                slot->unk14 = foundUnk14;
                slot->posX = foundPos[0];
                slot->posY = foundPos[1];
                slot->posZ = foundPos[2];
            } else {
                slot->timer -= owner->dt;
            }
        } else {
            slot->flags |= 1;
            slot->flags |= 2;
            BitClear(&slot->flags, 2);
            slot->actorId = foundActorId;
            slot->unk0C = foundUnk0C;
            slot->timer = lbl_804D6D40;
            slot->unk14 = foundUnk14;
            slot->posX = foundPos[0];
            slot->posY = foundPos[1];
            slot->posZ = foundPos[2];
        }
    } else if ((slot->flags & 1) != 0) {
        if (slot->timer >= lbl_804D6D40) {
            slot->flags |= 4;
            slot->timer -= owner->dt;
            if (slot->timer <= lbl_804D6D40 || !active) {
                BitClear(&slot->flags, 0);
                BitClear(&slot->flags, 2);
            }
        }
    }
}

// Refresh behaviour slot 2: the target search only runs while the owner is
// flagged as ready, and the slot is tracked and timed out like slot 3.
// fn_800B3164
extern "C" void fn_800B3164(void* owner_, int a, int b)
{
    Owner* owner = (Owner*)owner_;
    if (owner == 0) {
        return;
    }

    OwnerSlot* slot = &owner->slots[2];

    int active;
    if (slot->actorId == -1 || owner->callback == 0 || owner->callback(owner, slot->actorId) != 0) {
        active = 1;
    } else {
        active = 0;
    }

    if (fn_800A3758(owner) != 0) {
        int foundActorId;
        float foundPos[3];
        int foundUnk0C;
        u32 foundUnk14;

        int result = OwnerFindSightTarget(owner, &foundActorId, foundPos, &foundUnk0C,
                                 &foundUnk14, a, b);

        if (result != 0) {
            slot->state = 2;
            if ((slot->flags & 1) == 0) {
                slot->flags |= 1;
                slot->flags |= 2;
                BitClear(&slot->flags, 2);
                slot->unk0C = foundUnk0C;
                slot->actorId = foundActorId;
                slot->timer = lbl_804D6D40;
                slot->unk14 = foundUnk14;
                slot->posX = foundPos[0];
                slot->posY = foundPos[1];
                slot->posZ = foundPos[2];
            } else if (slot->actorId == foundActorId && slot->unk14 == foundUnk14 &&
                       slot->unk0C == foundUnk0C && active) {
                BitClear(&slot->flags, 2);
                slot->timer = slot->unk04;
            } else if (foundUnk0C > slot->unk0C) {
                if (slot->timer <= lbl_804D6D40 || !active) {
                    slot->flags |= 1;
                    slot->flags |= 2;
                    BitClear(&slot->flags, 2);
                    slot->unk0C = foundUnk0C;
                    slot->actorId = foundActorId;
                    slot->timer = lbl_804D6D40;
                    slot->unk14 = foundUnk14;
                    slot->posX = foundPos[0];
                    slot->posY = foundPos[1];
                    slot->posZ = foundPos[2];
                } else {
                    slot->timer -= owner->dt;
                }
            } else {
                slot->flags |= 1;
                slot->flags |= 2;
                BitClear(&slot->flags, 2);
                slot->unk0C = foundUnk0C;
                slot->actorId = foundActorId;
                slot->timer = lbl_804D6D40;
                slot->unk14 = foundUnk14;
                slot->posX = foundPos[0];
                slot->posY = foundPos[1];
                slot->posZ = foundPos[2];
            }
        } else if ((slot->flags & 1) != 0) {
            slot->flags |= 4;
            slot->timer -= owner->dt;
            if (slot->timer <= lbl_804D6D40 || !active) {
                BitClear(&slot->flags, 0);
                BitClear(&slot->flags, 2);
            }
        }
    } else if ((slot->flags & 4) != 0) {
        slot->timer -= owner->dt;
        if (slot->timer <= lbl_804D6D40 || !active) {
            BitClear(&slot->flags, 0);
            BitClear(&slot->flags, 2);
        }
    }
}

// Update behaviour slot 1 from its target query: resolve the slot's actor,
// run the slot timer and clear the slot's one-shot flag afterwards.
void OwnerUpdateSlot1(Owner* owner)
{
    if (owner == 0) {
        return;
    }

    OwnerSlot* slot = &owner->slots[1];
    OwnerQuery msg;
    if ((slot->flags & 1) != 0) {
        if (slot->actorId == -1 || owner->callback == 0 || owner->callback(owner, slot->actorId) != 0) {
            SendMsg(slot->actorId, 0x400, &msg);
            int kind = (u32)slot->actorId >> 24;
            if ((kind == 2 || kind == 0x13) && msg.unk0C == 0) {
                BitClear(&slot->flags, 2);
                BitClear(&slot->flags, 0);
                BitClear(&slot->flags, 1);
            } else if ((slot->flags & 4) != 0) {
                slot->timer -= owner->dt;
                if (slot->timer <= 0.0f) {
                    BitClear(&slot->flags, 0);
                    BitClear(&slot->flags, 2);
                }
            } else if ((slot->flags & 2) == 0 && (slot->flags & 0x10) == 0) {
                slot->flags |= 4;
            }
        } else {
            BitClear(&slot->flags, 2);
            BitClear(&slot->flags, 0);
            BitClear(&slot->flags, 1);
        }
    }
    BitClear(&slot->flags, 1);
}

// Clear all four behaviour slots and reset the animation entries.
extern "C" void fn_800B35D0(Owner* owner)
{
    if (owner == 0) {
        return;
    }
    for (int i = 0; i < 4; i++) {
        BitClear(&owner->slots[i].flags, 0);
        owner->slots[i].actorId = -1;
        owner->slots[i].state = 0;
    }
    for (int i = 0; i < 4; i++) {
        owner->anims[i].actorId = -1;
        owner->anims[i].anim = 6;
    }
}

// One 0x1c-byte record of the AI_NEO_SIGHT_DATA sight table at lbl_80449610.
struct ResetSlot1 {
    u8    pad00[0x10];
    float value;      // 0x10
    u8    pad14[0x08];
};
extern ResetSlot1 lbl_80449610[16];

// Stream fields used by the sight-data loader: cursor, remaining bytes and
// the running position that the reader advances on every take.
struct StreamData {
    u8  pad00[4];
    int remaining;   // 0x04
    u8* cursor;      // 0x08
    int position;    // 0x0c
};

// Take `size` bytes from the stream and return the data they started at.
static inline u8* StreamTake(StreamData* stream, int size)
{
    u8* data = stream->cursor;
    stream->position += size;
    stream->remaining -= size;
    stream->cursor = data + size;
    return data;
}

// Character mapping table of the CodeWarrior runtime (lower -> upper case).
extern u8 __upper_map[];

// Type name of the AI_NEO_SIGHT_DATA entity; the loader hashes it lazily.
extern const char* lbl_804D2B80;

// Load the AI_NEO_SIGHT_DATA block from `stream`: the leading word is the hash
// of the type name, followed by 16 0x1c-byte sight records copied into the
// global table. Returns 1 in every case.
// fn_800B365C
extern "C" int fn_800B365C(void* streamPtr)
{
    StreamData* stream = (StreamData*)streamPtr;
    const char* name = lbl_804D2B80;
    int avail = stream->position;
    if (avail < 0x1c4) {
        return 1;
    }

    bool ok;
    if (avail < 4) {
        ok = false;
    } else {
        u8* header = StreamTake(stream, 4);
        u32 hash;
        if (name == 0) {
            hash = 0;
        } else {
            hash = 0;
            const char* p = name;
            while (*p != 0) {
                char c = *p;
                u32 value;
                if (c == -1) {
                    value = 0xffffffff;
                } else {
                    value = (u8)__upper_map[(u8)c];
                }
                hash = hash * 31;
                hash += value & 0xff;
                p++;
            }
            hash = hash % 0xffffffff;
        }
        ok = (*(u32*)header == hash);
    }

    if (!ok) {
        return 1;
    }

    for (int i = 0; i < 8; i++) {
        u8* p = StreamTake(stream, 0x1c);
        *(ResetSlot1*)&lbl_80449610[i * 2] = *(ResetSlot1*)p;
        p = StreamTake(stream, 0x1c);
        *(ResetSlot1*)&lbl_80449610[i * 2 + 1] = *(ResetSlot1*)p;
    }
    return 1;
}

extern "C" void* memcpy(void* dst, const void* src, u32 size);

// Writer stream: the reader layout of StreamData plus the status word the
// writer clears when the block does not fit in the remaining space.
struct WriteStream {
    u8  pad00[4];
    int remaining;   // 0x04
    u8* cursor;      // 0x08
    int position;    // 0x0c
    int status;      // 0x10
};

// Serialize the AI_NEO_SIGHT_DATA block into `stream`: the leading word is the
// hash of the type name, followed by the 16 0x1c-byte sight records taken from
// the global table. This is the writer counterpart of fn_800B365C; returns 1.
// fn_800B3818 (kept under its placeholder name: u_8009DE94 calls it)
extern "C" int fn_800B3818(void* streamPtr)
{
    WriteStream* stream = (WriteStream*)streamPtr;
    if (stream->remaining < 0x1c4) {
        stream->status = 0;
        return 1;
    }

    u32 hash;
    const char* name = lbl_804D2B80;
    if (name == 0) {
        hash = 0;
    } else {
        hash = 0;
        const char* p = name;
        while (*p != 0) {
            char c = *p;
            u32 value;
            if (c == -1) {
                value = 0xffffffff;
            } else {
                value = (u8)__upper_map[(u8)c];
            }
            hash = hash * 31;
            hash += value & 0xff;
            p++;
        }
        hash = hash % 0xffffffff;
    }

    memcpy(stream->cursor, &hash, 4);
    StreamTake((StreamData*)stream, 4);

    for (int i = 0; i < 8; i++) {
        u8* p = StreamTake((StreamData*)stream, 0x1c);
        *(ResetSlot1*)p = lbl_80449610[i * 2];
        p = StreamTake((StreamData*)stream, 0x1c);
        *(ResetSlot1*)p = lbl_80449610[i * 2 + 1];
    }
    return 1;
}

// ---- owner sight search ---------------------------------------------------
// Tunables of the sight search.
extern float lbl_804D6D60;
extern float lbl_804D6D64;
extern float lbl_804D6D68;
extern float lbl_804D6D6C;
extern float lbl_804D6D70;
extern float lbl_804D6D74;
extern float lbl_804D6D78;
extern float lbl_804D6D7C;
extern s32   lbl_804D4820;    // first word of the 16-bit sight-slot enable mask

extern "C" float fn_800A1418(float value);
extern "C" float sqrtf(float value);
extern "C" void* fn_8003A3F0(int model, int field, int anim);
extern "C" int   fn_80049940(void);
extern "C" int   fn_80019A04(void);
extern "C" int   fn_8001F928(void* owner, void* info, u32 flags, void* callback, int actor);
extern "C" int   fn_800B466C(u32* p, u32 value);
class Actor;
int  MemFlagIsSet(void* flags, int bit);
int  ModelAnimate(int model, char* name, int arg);
int  ModelFindBone(int model, int bone);
void ActorAim(int* out, Actor* actor, int arg);
int  NeoAgentIsCurrent(int value);
int  HandleToPlayer(int handle);
int  PlayerTeam(int player);

static inline float AbsF(float value)
{
    return value < 0.0f ? -value : value;
}

// Owner fields the sight search reads, with the offsets the retail code uses.
struct SightOwner {
    u8    pad000[0x24];
    u32   cellX;                     // 0x24
    u32   cellY;                     // 0x28
    u32   cellZ;                     // 0x2c
    u8    pad030[0x08];
    int   id;                        // 0x38
    u8    pad03C[0x88];
    u8*   aimAgent;                  // 0xc4
    u8    pad0C8[0x04];
    int   model;                     // 0xcc
    u8    pad0D0[0x20];
    int   modelField;                // 0xf0
    u8    pad0F4[0x34];
    int   field128;                  // 0x128
    u8    pad12C[0x68];
    u32   field194;                  // 0x194
    u8    pad198[0x14];
    int   field1ac;                  // 0x1ac
    u8    pad1B0[0xC0];
    u8    flags270[8];               // 0x270
    u8    pad278[0x35C];
    float radius;                    // 0x5d4
    u8    pad5D8[0x24];
    void* field5fc;                  // 0x5fc
    u8    pad600[0x04];
    int (*callback)(SightOwner*, int); // 0x604
    u8    pad608[0x300];
    void* target;                    // 0x908
};

// One 0x1c-byte entry of the AI_NEO_SIGHT_DATA table, seen as float fields.
struct SightRecord {
    float x;          // 0x00
    float y;          // 0x04
    float z;          // 0x08
    int   actor;      // 0x0c
    float unk10;      // 0x10
    int   priority;   // 0x14
    u32   flags;      // 0x18
};

// Reply payload of the second 0x41e query; the handler overwrites the float.
struct TypeQueryF {
    u32   result;     // 0x00
    u32   hash;       // 0x04
    u32   zero;       // 0x08
    float value;      // 0x0c
};

// Pick the best visible actor out of the 16 AI_NEO_SIGHT_DATA entries: the
// entry with the lowest priority value whose position is inside the sight
// range and inside the view cone, has a live agent and passes the owner's
// visibility callback. Returns non-zero when an entry was accepted.
// fn_800B39DC
int OwnerFindSightTarget(Owner* ownerPtr, int* actorId, float* outPos,
                         int* outPriority, u32* outFlags, int a, int b)
{
    SightOwner* owner = (SightOwner*)ownerPtr;

    float maxDist;
    float maxCos;
    if (MemFlagIsSet(owner->flags270, 0x1c) != 0) {
        maxDist = lbl_804D6D60;
        maxCos = lbl_804D6D64;
    } else if (MemFlagIsSet(owner->flags270, 0x1b) != 0) {
        maxDist = lbl_804D6D68;
        maxCos = fn_800A1418(lbl_804D6D6C);
    } else {
        maxDist = lbl_804D6D60;
        maxCos = fn_800A1418(*(float*)(owner->aimAgent + 0x38));
    }

    float bestDist = maxDist;
    int bestPriority = 6;
    int lastResult = 0;

    int anim = ModelAnimate(owner->model, 0, 0);
    u8* animBase = (u8*)fn_8003A3F0(owner->model, owner->modelField, anim);

    // Ask the owner which bone/actor it is aiming at.
    TypeQuery boneQuery;
    boneQuery.name = 0;
    boneQuery.hash = 0x832DF50A;
    boneQuery.zero = 0;
    int boneOk = SendMsg(owner->id, 0x41e, &boneQuery);
    int bone = (int)boneQuery.name;

    u32 cellX;
    u32 cellY;
    u32 cellZ;
    int aimPos[3];
    if (boneOk == 0 || bone == -1) {
        cellX = owner->cellX;
        cellY = owner->cellY;
        cellZ = owner->cellZ;
        ActorAim(aimPos, (Actor*)owner, 1);
    } else {
        int index = ModelFindBone(owner->model, bone);
        u8* boneEntry = animBase + index * 0x30;
        cellX = ((u32*)boneEntry)[0x24 / 4];
        cellY = ((u32*)boneEntry)[0x28 / 4];
        cellZ = ((u32*)boneEntry)[0x2c / 4];
        ActorAim(aimPos, (Actor*)boneEntry, 1);
    }

    // Second query: the handler returns the height to look from.
    TypeQueryF aimQuery;
    aimQuery.value = lbl_804D6D70;
    aimQuery.hash = 0x52AB9E79;
    aimQuery.zero = 1;
    int aimOk = SendMsg(owner->id, 0x41e, &aimQuery);
    float aimReply = aimQuery.value;
    u32 aimHeight = cellZ;
    if (aimOk != 0) {
        aimHeight = (u32)aimReply;
    }

    float aimX = (float)(s32)cellX;
    float aimY = (float)(s32)cellY;
    float aimZ = (float)(s32)aimHeight;
    float dirX = lbl_804D6D74 * (float)aimPos[0];
    float dirY = lbl_804D6D74 * (float)aimPos[1];
    float dirZ = lbl_804D6D74 * (float)aimPos[2];

    SightRecord* record = (SightRecord*)lbl_80449610;
    u32* sightFlags = (u32*)&lbl_804D4820;

    for (int i = 0; i < 16; i++, record++) {
        if ((sightFlags[i >> 5] & (1u << (i & 31))) == 0) {
            continue;
        }

        u32 flags = record->flags;
        if ((flags & (u32)b) != 0) {
            continue;
        }
        if ((u32)a != ((u32)a & flags)) {
            continue;
        }
        if (owner->id == record->actor) {
            continue;
        }

        int priority = record->priority;
        int result = fn_800B21EC(ownerPtr, record->actor, &priority);
        if (result != 0 && priority == 5) {
            continue;
        }

        // Resolve the entry's owner through the 0x41e query.
        TypeQuery targetQuery;
        targetQuery.name = 0;
        targetQuery.hash = 0x273A5D;
        targetQuery.zero = 0;
        int targetOk = SendMsg(record->actor, 0x41e, &targetQuery);
        int target = (int)targetQuery.name;
        if (targetOk != 0 && owner->target != 0 && target != 0) {
            if ((int)owner->target == target) {
                continue;
            }
            if (priority > 1) {
                priority = 1;
            }
        }

        if (fn_80049940() != 0) {
            int entryTeam = PlayerTeam(HandleToPlayer(record->actor));
            int ownerTeam = PlayerTeam(HandleToPlayer(OwnerId(ownerPtr)));
            if (entryTeam == ownerTeam) {
                continue;
            }
            if (priority > 1 && (flags & 0x80) == 0) {
                priority = 1;
            }
        }

        if (priority > bestPriority) {
            continue;
        }
        if (record->actor != -1 && owner->callback != 0 &&
            owner->callback(owner, record->actor) == 0) {
            continue;
        }

        // Horizontal offset from the aim position and its squared length.
        float dx = record->x - aimX;
        float dy = record->y - aimY;
        float dz = record->z - aimZ;
        dz = lbl_804D6D70;
        float distSq = dx * dx + dy * dy + lbl_804D6D70 * lbl_804D6D70;
        if (*(int*)(owner->aimAgent + 0x0c) == 0x12EC1 && owner->field128 != 0x12EC1) {
            distSq *= lbl_804D6D78;
        }
        if (distSq > maxDist) {
            continue;
        }
        if (priority == bestPriority && priority == 1 &&
            distSq > owner->radius * owner->radius) {
            if ((result == 0 && lastResult == 1) ||
                (result == lastResult && distSq > bestDist)) {
                continue;
            }
        }

        // Normalize the offset and the aim direction, then test the view cone.
        float offsetX = dx;
        float offsetY = dy;
        float offsetZ = lbl_804D6D70;
        if (AbsF(offsetX) <= lbl_804D6D7C && AbsF(offsetY) <= lbl_804D6D7C &&
            AbsF(lbl_804D6D70) <= lbl_804D6D7C) {
            offsetX = lbl_804D6D70;
            offsetY = lbl_804D6D70;
            offsetZ = lbl_804D6D70;
        } else {
            float inv = 1.0f / sqrtf(offsetX * offsetX + offsetY * offsetY +
                                     lbl_804D6D70 * lbl_804D6D70);
            offsetX *= inv;
            offsetY *= inv;
            offsetZ = lbl_804D6D70 * inv;
        }
        if (AbsF(dirX) <= lbl_804D6D7C && AbsF(dirY) <= lbl_804D6D7C &&
            AbsF(dirZ) <= lbl_804D6D7C) {
            dirX = lbl_804D6D70;
            dirY = lbl_804D6D70;
            dirZ = lbl_804D6D70;
        } else {
            float inv = 1.0f / sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);
            dirX *= inv;
            dirY *= inv;
            dirZ *= inv;
        }
        float dot = offsetX * dirX + offsetY * dirY + offsetZ * dirZ;
        if ((flags & 0x80) != 0) {
            maxCos = lbl_804D6D64;
        }
        if (dot < maxCos) {
            continue;
        }

        int info[6];
        info[0] = bone;
        info[1] = (int)cellX;
        info[2] = (int)cellY;
        info[3] = (int)record->x;
        info[4] = (int)record->y;
        info[5] = (int)record->z;

        if (NeoAgentIsCurrent((int)ownerPtr) == 0) {
            continue;
        }

        OwnerMsg msg;
        if (SendMsg(record->actor, 0x400, &msg) == 0 || msg.actor == 0) {
            continue;
        }

        u32 visFlags = 0x2000A200;
        int blocked = 0;
        if (owner->field1ac == record->actor && (owner->field194 & 1) != 0) {
            int v = (owner->id & 0xFFFFFF) + fn_80019A04();
            if (v % 8 != 0) {
                blocked = 1;
            }
        }
        if ((flags & 0x80) != 0) {
            visFlags = 0xA000A200;
        }

        if (blocked == 0) {
            void* callback = owner->field5fc;
            if (callback == 0) {
                callback = (void*)fn_800B466C;
            }
            if (fn_8001F928(ownerPtr, info, visFlags, callback, (int)msg.actor) != 0) {
                continue;
            }
        }

        *outPriority = priority;
        *outFlags = flags;
        *actorId = record->actor;
        outPos[0] = record->x;
        outPos[1] = record->y;
        outPos[2] = record->z;
        bestDist = distSq;
        lastResult = result;
        bestPriority = priority;
    }

    return bestPriority != 6;
}

// Store a sight entry: refresh the entry whose actor/priority/flags triple
// matches, otherwise take the first free slot of the 16-entry table. Returns 1
// when the entry was stored, 0 when the table is full.
// fn_800B42E0
// fn_800B42E0
extern "C" int fn_800B42E0(float value, float* pos, int actor, int priority, u32 flags)
{
    SightRecord* records = (SightRecord*)lbl_80449610;
    u32* mask = (u32*)&lbl_804D4820;

    SightRecord* record = records;
    for (int i = 0; i < 16; i++, record++) {
        if ((mask[i / 32] & (1u << (i & 31))) != 0) {
            if (actor == record->actor &&
                priority == record->priority &&
                flags == record->flags) {
                record->x = pos[0];
                record->y = pos[1];
                record->z = pos[2];
                record->unk10 = value;
                return 1;
            }
        }
    }

    for (int i = 0; i < 16; i++) {
        if ((mask[i / 32] & (1u << (i & 31))) == 0) {
            records[i].actor = actor;
            records[i].priority = priority;
            records[i].flags = flags;
            records[i].unk10 = value;
            records[i].x = pos[0];
            records[i].y = pos[1];
            records[i].z = pos[2];
            mask[i / 32] |= (1u << (i & 31));
            return 1;
        }
    }
    return 0;
}

// True unless `p` already names `value` (a zero word never matches).
extern "C" int fn_800B466C(u32* p, u32 value)
{
    u32 current = p[0];
    if (current == 0 || current != value) {
        return 1;
    }
    return 0;
}

// Sound-data block written back into the global tables: the leading word is
// compared against the hash of the AI_NEO_SOUND_DATA type name, then the
// stream supplies the count word and the 0x200-byte sound slot table.
// fn_800B4690
extern const char* lbl_804D2B88;
extern s32        lbl_804D4828;

// One 0x20-byte entry of the AI_NEO_SOUND_DATA target table.
struct ResetSlot2 {
    float x;         // 0x00
    float y;         // 0x04
    float z;         // 0x08
    float value;     // 0x0c selection value, smaller wins
    int   priority;  // 0x10
    float radius;    // 0x14
    u32   flags;     // 0x18
    s32   actor;     // 0x1c
};
extern ResetSlot2 lbl_804497D0[16];

// fn_800B4690
extern "C" int fn_800B4690(void* streamPtr)
{
    StreamData* stream = (StreamData*)streamPtr;
    int avail = stream->position;
    if (avail < 0x208) {
        return 1;
    }

    bool ok;
    if (avail < 4) {
        ok = false;
    } else {
        u8* header = StreamTake(stream, 4);
        const char* name = lbl_804D2B88;
        u32 hash;
        if (name == 0) {
            hash = 0;
        } else {
            hash = 0;
            const char* p = name;
            while (*p != 0) {
                char c = *p;
                u32 value;
                if (c == -1) {
                    value = 0xffffffff;
                } else {
                    value = (u8)__upper_map[(u8)c];
                }
                hash = hash * 31;
                hash += value & 0xff;
                p++;
            }
            hash = hash % 0xffffffff;
        }
        ok = (*(u32*)header == hash);
    }

    if (!ok) {
        return 1;
    }

    memcpy(&lbl_804D4828, stream->cursor, 4);
    stream->position -= 4;
    stream->cursor += 4;
    memcpy(lbl_804497D0, stream->cursor, 0x200);
    stream->position -= 0x200;
    stream->cursor += 0x200;
    return 1;
}

// Write the AI_NEO_SOUND_DATA block back into `stream`: the type-name hash,
// the active-entry mask and the 16 0x20-byte target records. Returns 1 in
// every case. The symbol keeps its placeholder name because another unit
// (u_8009DE94) calls it; descriptive name: NeoSoundDataSave.
extern "C" int fn_800B47EC(void* streamPtr)
{
    StreamData* stream = (StreamData*)streamPtr;
    if (stream->remaining < 0x208) {
        stream->position = 0;
        return 1;
    }

    const char* name = lbl_804D2B88;
    u32 hash;
    if (name == 0) {
        hash = 0;
    } else {
        hash = 0;
        const char* p = name;
        while (*p != 0) {
            char c = *p;
            u32 value;
            if (c == -1) {
                value = 0xffffffff;
            } else {
                value = (u8)__upper_map[(u8)c];
            }
            hash = hash * 31;
            hash += value & 0xff;
            p++;
        }
        hash = hash % 0xffffffff;
    }
    memcpy(stream->cursor, &hash, 4);
    stream->position += 4;
    stream->remaining -= 4;
    stream->cursor += 4;

    memcpy(stream->cursor, &lbl_804D4828, 4);
    stream->position += 4;
    stream->remaining -= 4;
    stream->cursor += 4;

    memcpy(stream->cursor, lbl_804497D0, 0x200);
    stream->position += 0x200;
    stream->remaining -= 0x200;
    stream->cursor += 0x200;
    return 1;
}

// Search the 16 AI_NEO_SOUND_DATA records for the best target: the active
// entry whose flags contain `requiredFlags` and avoid `excludedFlags`, which is
// not the owner's own actor, is inside the record's radius and passes the
// owner's visibility callback. Writes the winner's actor id, position,
// priority and flags. Returns non-zero when an entry was accepted.
// fn_800B4944 (descriptive name: OwnerFindSoundTarget)
int OwnerFindSoundTarget(Owner* ownerPtr, int* actorId, float* pos, float* outPos,
                         int* outPriority, u32* outFlags, u32 requiredFlags,
                         u32 excludedFlags)
{
    SightOwner* owner = (SightOwner*)ownerPtr;
    ResetSlot2* record = lbl_804497D0;
    u32* mask = (u32*)&lbl_804D4828;

    float bestValue = lbl_804D6D90;
    int bestPriority = 6;
    int bestResult = 0;

    for (int i = 0; i < 16; i++, record++) {
        if ((mask[i >> 5] & (1u << (i & 31))) == 0) {
            continue;
        }
        if ((excludedFlags & record->flags) != 0) {
            continue;
        }
        if (requiredFlags != (requiredFlags & record->flags)) {
            continue;
        }
        if (owner->id == record->actor) {
            continue;
        }

        int priority = record->priority;
        int result = fn_800B21EC(ownerPtr, record->actor, &priority);
        if (result != 0 && priority == 5) {
            continue;
        }

        // Resolve the entry's actor through the 0x41e query.
        TypeQuery query;
        query.name = 0;
        query.hash = 0x273A5D;
        query.zero = 0;
        int targetOk = SendMsg(record->actor, 0x41e, &query);
        int target = (int)query.name;
        if (targetOk != 0 && owner->target != 0 && target != 0) {
            if ((int)owner->target == target) {
                continue;
            }
            if (priority > 1) {
                priority = 1;
            }
        }

        if (fn_80049940() != 0) {
            int entryTeam = PlayerTeam(HandleToPlayer(record->actor));
            int ownerTeam = PlayerTeam(HandleToPlayer(OwnerId(ownerPtr)));
            if (entryTeam == ownerTeam) {
                continue;
            }
            if (priority > 1) {
                priority = 1;
            }
        }

        if (priority > bestPriority) {
            continue;
        }
        if (record->actor != -1 && owner->callback != 0 &&
            owner->callback(owner, record->actor) == 0) {
            continue;
        }

        // Squared distance from the search position.
        float dx = pos[0] - record->x;
        float dy = pos[1] - record->y;
        float dz = pos[2] - record->z;
        float distSq = dx * dx + dy * dy + dz * dz;
        if (*(int*)(owner->aimAgent + 0x0c) == 0x12EC1 &&
            owner->field128 != 0x12EC1) {
            distSq *= lbl_804D6D94;
        }
        if (distSq > record->radius * record->radius) {
            continue;
        }
        if (priority == bestPriority && priority == 1 &&
            distSq > owner->radius * owner->radius) {
            if ((result == 0 && bestResult == 1) ||
                (result == bestResult && bestValue < record->value)) {
                continue;
            }
        }

        bestResult = result;
        bestValue = record->value;
        bestPriority = priority;
        *outPriority = priority;
        *outFlags = record->flags;
        *actorId = record->actor;
        outPos[0] = record->x;
        outPos[1] = record->y;
        outPos[2] = record->z;
    }

    return bestPriority != 6;
}

// Register a sound target: update the active record matching actor, priority,
// radius and flags, or fill the first free slot. Returns 1 on success.
// fn_800B4C50 (descriptive name: SoundTargetAdd)
extern "C" int fn_800B4C50(float value, float radius, float* pos, int actor,
                           int priority, u32 flags)
{
    u32* mask = (u32*)&lbl_804D4828;
    for (int i = 0; i < 16; i++) {
        if ((mask[i / 32] & (1 << (i & 31))) != 0) {
            ResetSlot2* record = &lbl_804497D0[i];
            if (record->actor == actor) {
                if (record->priority == priority) {
                    if (record->radius == radius) {
                        if (record->flags == flags) {
                            record->value = value;
                            record->x = pos[0];
                            record->y = pos[1];
                            record->z = pos[2];
                            return 1;
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < 16; i++) {
        u32 bit = 1u << (i & 31);
        if ((mask[i / 32] & bit) == 0) {
            ResetSlot2* record = &lbl_804497D0[i];
            record->value = value;
            record->priority = priority;
            record->radius = radius;
            record->flags = flags;
            record->actor = actor;
            record->x = pos[0];
            record->y = pos[1];
            record->z = pos[2];
            mask[i / 32] |= bit;
            return 1;
        }
    }
    return 0;
}

// ---- static table resets (defined last so callers above do not inline them) ----

extern ResetSlot1 lbl_80449610[16];
extern ResetSlot2 lbl_804497D0[16];
extern float      lbl_804D6D64;
extern float      lbl_804D6D98;
extern s32        lbl_804D4820;
extern s32        lbl_804D4828;

void fn_800B4614()
{
    for (int i = 0; i < 16; i++) {
        lbl_80449610[i].value = lbl_804D6D64;
    }
    lbl_804D4820 = 0;
}

void fn_800B4EFC()
{
    for (int i = 0; i < 16; i++) {
        lbl_804497D0[i].value = lbl_804D6D98;
        lbl_804497D0[i].actor = -1;
    }
    lbl_804D4828 = 0;
}

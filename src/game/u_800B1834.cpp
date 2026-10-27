// u_800B1834.cpp: owner behaviour-slot and animation bookkeeping.
// Drafted from the retail assembly; type and member names are descriptive guesses.

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
extern "C" void fn_8028BC28(void* a, void* b, u32 c);
extern "C" int  fn_800A37CC(void* object);
void OwnerUpdateSlot1(Owner* owner);
extern "C" int  fn_800A3758(Owner* owner);
int OwnerFindSoundTarget(Owner* owner, int* actorId, float* pos, float* outPos,
                         int* outPriority, u32* outFlags, u32 requiredFlags,
                         u32 excludedFlags);
int OwnerFindSightTarget(Owner* owner, int* actorId, float* outPos, int* unk0C,
                         u32* unk14, int a, int b);
extern "C" void fn_8016D68C(void* dst, void* src);
extern "C" void fn_8028BC2C(void* a, void* b, u32 c);
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

class Buffer {
public:
    void* vtable;   // 0x00
    u32   unk04;    // 0x04
    u32   size;     // 0x08 (bytes used)
    u8*   data;     // 0x0c
    Buffer();
    ~Buffer();
};

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

// Rotation basis shared by the bone helpers (0x24 bytes).
class PlaceF {
public:
    float basis[9];  // 0x00
};

// Bone transform: rotation basis plus integer cell coordinates (0x30 bytes).
class Mtx34 : public PlaceF {
public:
    s32 cellX;       // 0x24
    s32 cellY;       // 0x28
    s32 cellZ;       // 0x2c
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
    int     (*callback)(Owner*); // 0x604
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
            if (slot->actorId == -1 || owner->callback == 0 || owner->callback(owner) != 0) {
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
    if (owner == 0) {
        return;
    }
    Owner* o = owner;
    for (int i = 0; i < 8; i++) {
        if (lbl_80449568[i].id == OwnerId(o)) {
            lbl_80449568[i].id = -1;
            lbl_80449568[i].a = 0;
            lbl_80449568[i].b = 0;
            lbl_80449568[i].c = 0;
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
        if (slot->actorId != -1 && owner->callback != 0 && owner->callback(owner) == 0) {
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
        if (!HasCondition(actor + 0x270, 0x0b)) {
            if (*(int*)(query + 0x0c) != 0) {
                return 1;
            }
        }
        return 0;
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
    if (slot->actorId == -1 || owner->callback == 0 || owner->callback(owner) != 0) {
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
    if (slot->actorId == -1 || owner->callback == 0 || owner->callback(owner) != 0) {
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
        if (slot->actorId == -1 || owner->callback == 0 || owner->callback(owner) != 0) {
            SendMsg(slot->actorId, 0x400, &msg);
            int kind = (u32)slot->actorId >> 24;
            if ((kind == 2 || kind == 0x13) && msg.unk0C == 0) {
                BitClear(&slot->flags, 2);
                BitClear(&slot->flags, 0);
                BitClear(&slot->flags, 1);
            } else if ((slot->flags & 4) != 0) {
                float limit = lbl_804D6D40;
                slot->timer -= owner->dt;
                if (slot->timer <= limit) {
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
    int (*callback)(SightOwner*);    // 0x604
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
            owner->callback(owner) == 0) {
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
    if (current != 0 && current == value) {
        return 0;
    }
    return 1;
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
            owner->callback(owner) == 0) {
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



// Reset the instance's transform and timers to their defaults.
extern "C" int fn_800B4F98(u8* object)
{
    if (object == 0) {
        return 0;
    }
    float rest = lbl_804D6DA4;
    float idle = lbl_804D6DA0;
    *(u32*)(object + 0x26c) = 0;
    *(float*)(object + 0x260) = idle;
    *(u32*)(object + 0x268) = 4;
    *(float*)(object + 0x264) = idle;
    *(float*)(object + 0x240) = rest;
    *(float*)(object + 0x244) = rest;
    *(float*)(object + 0x248) = rest;
    fn_8016D68C(object + 0x24c, (void*)(*(u32*)(object + 0xc8) + 0x24));
    return 1;
}

// Copy three floats from `src` unless it is null.
extern "C" void fn_800B5004(float* dst, float* src)
{
    if (src == 0) {
        return;
    }
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

// Aim/target evaluation for the owner's behaviour slot.  Picks one of two
// candidate heading vectors (or, when the trace lands in front of the owner,
// one of two offset points), writes the winning direction to `out` and returns
// the animation timing value.  Opaque offsets are used because the owner layout
// beyond 0x240 is not modelled yet.
// fn_800B5028 (descriptive name: OwnerAimUpdate)
extern float lbl_804D6DA8;
extern float lbl_804D6DAC;
extern float lbl_804D6DB0;
extern float lbl_804D6DB4;
extern float lbl_804D6DB8;
extern float lbl_804D6DBC;
extern float lbl_804D6DC0;
extern float lbl_804D6DC4;
extern float lbl_804D6DC8;
extern double lbl_804D6DD0;
extern float lbl_8039B6F8[];

extern "C" int    fn_800BAF40(void);
extern "C" void   fn_800BB8B8(int owner, float* out);
extern "C" float  fn_800E1020(float* v);
extern "C" void   fn_800A7CB4(int owner, void* pos, float* target, float* frac,
                              int* hitActor, float* offset, int flags);
extern "C" u32    fn_800BA7FC(int index);
extern "C" void   fn_800090BC(void* from, void* to, int r, int g, int b);
extern "C" float  fn_800AA9FC(int owner, float* pos);
extern "C" float  fn_800A109C(void);
extern "C" void   fn_8009F9BC(float angle, void* out, void* table);
extern "C" void   fn_800A0030(float* out, void* mtx, float* vec);
extern "C" void   fn_800E1098(float scale, float* src, float* dst);
extern "C" void   fn_800BB218(float a, int owner, u32 unk, void* pos, void* point,
                              int owner2, float* out, void* out1, void* out2);
extern "C" void   ConditionSetBit(void* object, int bit, int value);
extern "C" double fabs(double x);

extern "C" float fn_800B5028(int ownerPtr, float* out)
{
    u8* o = (u8*)ownerPtr;
    if (o == 0) {
        return lbl_804D6DA8;
    }

    float* aim = (float*)(o + 0x240);
    float* pos = (float*)(o + 0x84);

    int outside = (fabs(aim[0]) > lbl_804D6DAC) +
                  (fabs(aim[1]) > lbl_804D6DAC) +
                  (fabs(aim[2]) > lbl_804D6DAC);
    if (outside == 0 || fn_800BAF40() != -3) {
        ConditionSetBit((void*)(o + 0x270), 0x40, 0);
        return lbl_804D6DA8;
    }

    float target[3];
    if (HasCondition((void*)(o + 0x270), 0x27) == 0) {
        fn_800BB8B8(ownerPtr, target);
    } else {
        target[0] = aim[0] + pos[0];
        target[1] = aim[1] + pos[1];
        target[2] = aim[2] + pos[2];
    }

    float dir[3];
    dir[0] = target[0] - pos[0];
    dir[1] = target[1] - pos[1];
    dir[2] = target[2] - pos[2];
    float dirLen = fn_800E1020(dir);

    float frac;
    int hitActor;
    float offset;
    fn_800A7CB4(ownerPtr, pos, target, &frac, &hitActor, &offset, -1);

    if (fn_800BA7FC(0x2f) != 0) {
        if (lbl_804D6DB0 <= frac || hitActor == 0) {
            fn_800090BC(pos, target, 0, 0xff, 0);
        } else {
            float hitPos[3];
            hitPos[0] = (target[0] - pos[0]) * frac + pos[0];
            hitPos[1] = (target[1] - pos[1]) * frac + pos[1];
            hitPos[2] = (target[2] - pos[2]) * frac + pos[2];
            float hitOrigin[3];
            hitOrigin[0] = (float)*(s32*)(hitActor + 0x24);
            hitOrigin[1] = (float)*(s32*)(hitActor + 0x28);
            hitOrigin[2] = (float)*(s32*)(hitActor + 0x2c);
            fn_800090BC(pos, target, 0, 0xff, 0xff);
            fn_800090BC(hitPos, hitOrigin, 0xff, 0, 0);
        }
    }

    float result = lbl_804D6DA8;
    if (frac < lbl_804D6DB0) {
        float hitPos[3];
        hitPos[0] = (float)*(s32*)(hitActor + 0x24);
        hitPos[1] = (float)*(s32*)(hitActor + 0x28);
        hitPos[2] = (float)*(s32*)(hitActor + 0x2c);

        float angle = fn_800AA9FC(ownerPtr, hitPos);
        if (fabs(angle) <= lbl_804D6DB4) {
            float vec[6];
            int chosen;

            if (lbl_804D6DA8 <= frac) {
                float toHit[3];
                toHit[0] = hitPos[0] - pos[0];
                toHit[1] = hitPos[1] - pos[1];
                toHit[2] = hitPos[2] - pos[2];

                float hitDist = fn_800E1020(toHit);
                float spread = lbl_804D6DB4;
                if ((float)((*(float*)(o + 0x900) + offset) / hitDist) <
                    lbl_804D6DB0) {
                    float t = fn_800A109C();
                    spread = lbl_804D6DB0 + (lbl_804D6DB4 - t);
                }

                u8 angBuf[52];
                float scale;

                fn_8009F9BC(spread, angBuf, lbl_8039B6F8);
                fn_800A0030(vec, angBuf, toHit);
                scale = lbl_804D6DC8 * *(float*)(o + 0x900);
                if (dirLen < scale) {
                    scale = dirLen;
                }
                fn_800E1098(scale, vec, vec);

                float pt[6];
                pt[0] = vec[0] + pos[0];
                pt[1] = vec[1] + pos[1];
                pt[2] = vec[2] + pos[2];

                fn_8009F9BC(-spread, angBuf, lbl_8039B6F8);
                fn_800A0030(vec + 3, angBuf, toHit);
                scale = lbl_804D6DC8 * *(float*)(o + 0x900);
                if (dirLen < scale) {
                    scale = dirLen;
                }
                fn_800E1098(scale, vec + 3, vec + 3);

                pt[3] = vec[3] + pos[0];
                pt[4] = vec[4] + pos[1];
                pt[5] = vec[5] + pos[2];

                float score[2];
                int side[2];
                side[0] = 0;
                side[1] = 0;
                for (int i = 0; i < 2; i++) {
                    float value;
                    int a, b;
                    fn_800A7CB4(ownerPtr, pos, pt + i * 3, &score[i], &side[i],
                                &offset, *(int*)(hitActor + 0x38));
                    fn_800BB218(*(float*)(o + 0x900), ownerPtr,
                                *(u32*)(o + 0x2c4), pos, pt + i * 3, ownerPtr,
                                &value, &a, &b);
                    if (value < score[i]) {
                        score[i] = value;
                        side[i] = 0;
                    }
                    if (score[i] < dirLen) {
                        score[i] = dirLen;
                    }
                }

                if (lbl_804D6DAC <= fabs(score[0] - score[1])) {
                    chosen = (score[0] <= score[1]) ? 1 : 0;
                } else if (vec[2] * dir[2] + vec[0] * dir[0] +
                               vec[1] * dir[1] <=
                           vec[5] * dir[2] + vec[3] * dir[0] +
                               vec[4] * dir[1]) {
                    chosen = 1;
                } else {
                    chosen = 0;
                }

                if (fn_800BA7FC(0x2f) != 0) {
                    for (int i = 0; i < 2; i++) {
                        if (score[i] < lbl_804D6DB0) {
                            float point[3];
                            point[0] = vec[i * 3] * score[i] + pos[0];
                            point[1] = vec[i * 3 + 1] * score[i] + pos[1];
                            point[2] = vec[i * 3 + 2] * score[i] + pos[2];
                            float source[3];
                            if (side[i] == 0) {
                                source[0] = point[0];
                                source[1] = point[1];
                                source[2] = point[2] - lbl_804D6DBC;
                            } else {
                                source[0] = (float)*(s32*)(side[i] + 0x24);
                                source[1] = (float)*(s32*)(side[i] + 0x28);
                                source[2] = (float)*(s32*)(side[i] + 0x2c);
                            }
                            fn_800090BC(point, source, 0xff, 0, 0);
                        }
                        fn_800090BC(pos, pt + i * 3, 0, 0xff,
                                    (chosen != i) ? 0xff : 0);
                    }
                }
            } else {
                float base[3];
                base[0] = pos[0] - hitPos[0];
                base[1] = pos[1] - hitPos[1];
                base[2] = pos[2] - hitPos[2];
                fn_800E1098(offset + *(float*)(o + 0x900), base, base);
                base[0] += hitPos[0];
                base[1] += hitPos[1];
                base[2] += hitPos[2];

                float rel[3];
                rel[0] = base[0] - pos[0];
                rel[1] = base[1] - pos[1];
                rel[2] = base[2] - pos[2];

                float cross[6];
                cross[2] = rel[0] * lbl_8039B6F8[1] - rel[1] * lbl_8039B6F8[0];
                cross[1] = rel[2] * lbl_8039B6F8[0] - rel[0] * lbl_8039B6F8[2];
                cross[0] = rel[1] * lbl_8039B6F8[2] - rel[2] * lbl_8039B6F8[1];
                fn_800E1098(offset + *(float*)(o + 0x900), cross, cross);
                cross[3] = -cross[0];
                cross[4] = -cross[1];
                cross[5] = -cross[2];

                vec[0] = cross[0] + base[0];
                vec[1] = cross[1] + base[1];
                vec[2] = cross[2] + base[2];
                vec[3] = cross[3] + base[0];
                vec[4] = cross[4] + base[1];
                vec[5] = cross[5] + base[2];

                float hit[2];
                if (cross[2] * dir[2] + cross[0] * dir[0] +
                        cross[1] * dir[1] <= lbl_804D6DB8) {
                    if (cross[5] * dir[2] + cross[3] * dir[0] +
                            cross[4] * dir[1] <= lbl_804D6DB8) {
                        float value;
                        int a, b;
                        fn_800BB218(*(float*)(o + 0x900), ownerPtr,
                                    *(u32*)(o + 0x2c4), base, vec, ownerPtr,
                                    &hit[0], &a, &b);
                        fn_800BB218(*(float*)(o + 0x900), ownerPtr,
                                    *(u32*)(o + 0x2c4), base, vec + 3, ownerPtr,
                                    &hit[1], &a, &b);
                        chosen = (hit[0] <= hit[1]) ? 1 : 0;
                    } else {
                        chosen = 1;
                    }
                } else {
                    chosen = 0;
                }

                if (fn_800BA7FC(0x2f) != 0) {
                    for (int i = 0; i < 2; i++) {
                        if (hit[i] < lbl_804D6DB0) {
                            float point[3];
                            point[2] = cross[i * 3 + 2] * hit[i] + base[2];
                            point[0] = cross[i * 3] * hit[i] + base[0];
                            point[1] = cross[i * 3 + 1] * hit[i] + base[1];
                            float source[3];
                            source[2] = point[2] - lbl_804D6DBC;
                            source[0] = point[0];
                            source[1] = point[1];
                            fn_800090BC(source, point, 0xff, 0, 0);
                        }
                        fn_800090BC(base, vec + i * 3, 0, 0xff,
                                    (chosen != i) ? 0xff : 0);
                    }
                }
            }

            out[0] = vec[chosen * 3];
            out[1] = vec[chosen * 3 + 1];
            out[2] = lbl_804D6DA8;
            fn_800E1098((*(float*)(o + 0x2ac) * *(float*)(o + 0x138)) /
                            lbl_804D6DC0,
                        out, out);
            ConditionSetBit((void*)(o + 0x270), 0x40, 1);
            result = lbl_804D6DC4;
        } else {
            ConditionSetBit((void*)(o + 0x270), 0x40, 0);
            result = lbl_804D6DA8;
        }
    } else {
        ConditionSetBit((void*)(o + 0x270), 0x40, 0);
        result = lbl_804D6DA8;
    }
    return result;
}

extern float lbl_804D6DD8;
extern float lbl_804D6DDC;

// Clamp the slot's aim vector against the radius implied by the owner's speed
// and write the result to `out`; returns a hint value based on how far the
// result is from the origin.  Opaque offsets: the owner layout beyond 0x240 is
// not modelled yet.
// fn_800B5BF8 (descriptive name: OwnerSlotClampAim)
extern "C" float fn_800B5BF8(int ownerPtr, float* out)
{
    u8* o = (u8*)ownerPtr;
    if (o == 0) {
        return lbl_804D6DA8;
    }

    float* aim = (float*)(o + 0x240);
    float* vel = (float*)(o + 0x270);

    float speed = *(float*)(o + 0x28c);
    float radius = lbl_804D6DD8 * speed;
    float radiusSq = radius * radius;
    float lenSq = aim[0] * aim[0] + aim[1] * aim[1] + aim[2] * aim[2];
    float scale = (*(float*)(o + 0x2ac) * *(float*)(o + 0x138)) /
                  lbl_804D6DC0;

    int outside = (fabs(vel[0]) > lbl_804D6DAC) +
                  (fabs(vel[1]) > lbl_804D6DAC) +
                  (fabs(vel[2]) > lbl_804D6DAC);

    if (outside == 0) {
        float dist = fn_800E1020(aim);
        if (scale <= dist) {
            fn_800E1098(-scale, out, aim);
        } else {
            out[0] = -aim[0];
            out[1] = -aim[1];
            out[2] = -aim[2];
        }
    } else if (lenSq <= radiusSq) {
        fn_800E1098(scale, out, aim);
    } else {
        float diff = lenSq - radiusSq;
        float value = lbl_804D6DA8;
        if (diff > lbl_804D6DA8) {
            value = sqrtf(diff);
        }
        value = -value;
        if (-scale > value) {
            value = -scale;
        }
        fn_800E1098(value, out, aim);
    }

    int outOutside = (fabs(out[0]) > lbl_804D6DAC) +
                     (fabs(out[1]) > lbl_804D6DAC) +
                     (fabs(out[2]) > lbl_804D6DAC);
    return (outOutside == 0) ? lbl_804D6DA8 : lbl_804D6DDC;
}

extern float lbl_804D6DE0;
extern float lbl_804D6DE4;
extern float lbl_804D6DE8;
extern float lbl_804D6DEC;
extern float lbl_804D6DF0;
extern float lbl_804D6DF4;
extern float lbl_804D6DF8;

// Vec3Set and EulerNormalize are the retail symbols at 0x8016D67C and
// 0x8016D55C; the two plain fn_ helpers are still unnamed in symbols.txt.
struct Vec3 {
    float x;
    float y;
    float z;
};
void Vec3Set(Vec3* dst, float x, float y, float z);
void EulerNormalize(Vec3* dst, Vec3* src);
extern "C" void fn_8016D440(float* out, float* in);
extern "C" void fn_8016D614(float* out, float* a, float* b);

// Fold the owner's four behaviour-slot accelerations into its velocity, then
// resolve that velocity against the aim direction and update the orientation
// and condition state.  Returns 1 when it ran.  Opaque offsets: the owner
// layout beyond 0x240 is not modelled yet.
// fn_800B5E10 (descriptive name: OwnerSlotIntegrate)
extern "C" int fn_800B5E10(int ownerPtr)
{
    u8* o = (u8*)ownerPtr;
    int result = 0;
    if (o == 0) {
        return 0;
    }

    float weight[4];
    float vec[4][3];
    float speedSq = *(float*)(o + 0x28c) * *(float*)(o + 0x28c);
    bool flag = false;

    for (int i = 0; i < 4; i++) {
        vec[i][0] = lbl_804D6DA8;
        vec[i][1] = lbl_804D6DA8;
        vec[i][2] = lbl_804D6DA8;
        float (*callback)(int, float*) =
            *(float (**)(int, float*))(o + 0x5a0 + i * 4);
        if (callback == 0) {
            weight[i] = lbl_804D6DA8;
        } else {
            weight[i] = callback(ownerPtr, vec[i]);
        }
    }

    float* aim = (float*)(o + 0x240);
    float* vel = (float*)(o + 0x270);

    if (MemFlagIsSet(vel, 4) == 0 &&
        (fabs(vel[0]) > lbl_804D6DAC || fabs(vel[1]) > lbl_804D6DAC ||
         fabs(vel[2]) > lbl_804D6DAC) &&
        (fabs(aim[0]) > lbl_804D6DAC || fabs(aim[1]) > lbl_804D6DAC ||
         fabs(aim[2]) > lbl_804D6DAC)) {
        if (*(int*)(o + 0x2d0) == -1) {
            flag = true;
        } else {
            float hit[3];
            fn_800BB8B8(ownerPtr, hit);
            hit[0] -= *(float*)(o + 0x84);
            hit[1] -= *(float*)(o + 0x88);
            hit[2] -= *(float*)(o + 0x8c);
            float dir[3];
            fn_8016D440(dir, hit);
            Vec3Set((Vec3*)(o + 0x27c), dir[2] - *(float*)(o + 0x254),
                    dir[0] - *(float*)(o + 0x24c), lbl_804D6DA8);
            if (HasCondition(vel, 0x38) != 0) {
                *(float*)(o + 0x284) += lbl_804D6DE0;
            }
            EulerNormalize((Vec3*)(o + 0x27c), (Vec3*)(o + 0x27c));
        }
    } else {
        float pos[3];
        if (MemFlagIsSet(vel, 4) == 0) {
            pos[0] = aim[0];
            pos[1] = aim[1];
            pos[2] = aim[2];
        } else {
            float raw[3];
            fn_800BB8B8(ownerPtr, raw);
            pos[0] = raw[0] - *(float*)(o + 0x24);
            pos[1] = raw[1] - *(float*)(o + 0x28);
            pos[2] = raw[2] - *(float*)(o + 0x2c);
        }

        bool aimSet = (fabs(aim[0]) > lbl_804D6DAC ||
                       fabs(aim[1]) > lbl_804D6DAC ||
                       fabs(aim[2]) > lbl_804D6DAC);
        float up = lbl_804D6DA8;
        if (aimSet) {
            float aimDir[3];
            float posDir[3];
            fn_8016D440(aimDir, aim);
            fn_8016D440(posDir, pos);
            fn_8016D614(posDir, posDir, aimDir);
            EulerNormalize((Vec3*)posDir, (Vec3*)posDir);
            up = aimDir[1];
        }

        float proj = lbl_804D6DE4;
        float slope = lbl_804D6DA8;
        if (fabs(up) > lbl_804D6DAC && aimSet) {
            proj = (aim[0] * pos[0] + aim[1] * pos[1] + aim[2] * pos[2]) /
                   (aim[0] * aim[0] + aim[1] * aim[1] + aim[2] * aim[2]);
            slope = fabs(up) / *(float*)(*(int*)(o + 0xc4) + 0xc0);
        }

        float total = lbl_804D6DA8 + weight[0] + weight[1] + weight[2] +
                      weight[3];
        if (total > lbl_804D6DAC) {
            weight[0] /= total;
            weight[1] /= total;
            weight[2] /= total;
            weight[3] /= total;
        }

        float sum[3];
        sum[0] = lbl_804D6DA8;
        sum[1] = lbl_804D6DA8;
        sum[2] = lbl_804D6DA8;
        for (int i = 0; i < 4; i++) {
            sum[0] += vec[i][0] * weight[i];
            sum[1] += vec[i][1] * weight[i];
            sum[2] += vec[i][2] * weight[i];
        }

        if (fn_800BA7FC(0x30) != 0) {
            for (int i = 0; i < 4; i++) {
                if (fabs(vec[i][0]) > lbl_804D6DAC ||
                    fabs(vec[i][1]) > lbl_804D6DAC ||
                    fabs(vec[i][2]) > lbl_804D6DAC) {
                    float origin[3];
                    origin[0] = (float)*(s32*)(o + 0x24);
                    origin[1] = (float)*(s32*)(o + 0x28);
                    origin[2] = (float)*(s32*)(o + 0x2c);
                    float end[3];
                    fn_800E1098(lbl_804D6DE8, end, vec[i]);
                    end[0] += origin[0];
                    end[1] += origin[1];
                    end[2] += origin[2];
                    fn_800090BC(origin, end, 0, 0xff, 0xff);
                }
            }
        }

        if (fn_800BA7FC(0x31) != 0) {
            float origin[3];
            origin[0] = (float)*(s32*)(o + 0x24);
            origin[1] = (float)*(s32*)(o + 0x28);
            origin[2] = (float)*(s32*)(o + 0x2c);
            float end[3];
            fn_800E1098(lbl_804D6DEC, end, sum);
            end[0] += origin[0];
            end[1] += origin[1];
            end[2] += origin[2];
            fn_800090BC(origin, end, 0xff, 0xff, 0xff);

            float end2[3];
            fn_800E1098(lbl_804D6DF0, end2, aim);
            end2[0] += origin[0];
            end2[1] += origin[1];
            end2[2] += origin[2];
            fn_800090BC(origin, end2, 0x80, 0xff, 0x80);
        }

        if (fn_800BA7FC(0x2e) != 0) {
            float origin[3];
            origin[0] = (float)*(s32*)(o + 0x24);
            origin[1] = (float)*(s32*)(o + 0x28);
            origin[2] = (float)*(s32*)(o + 0x2c);
            float end[3];
            end[0] = origin[0] + aim[0] * lbl_804D6DF4;
            end[1] = origin[1] + aim[1] * lbl_804D6DF4;
            end[2] = origin[2] + aim[2] * lbl_804D6DF4;
            fn_800090BC(origin, end, 0, 0xff, 0xff);
        }

        aim[0] += sum[0];
        aim[1] += sum[1];
        aim[2] += sum[2];
        float lenSq = aim[0] * aim[0] + aim[1] * aim[1] + aim[2] * aim[2];

        float angle = lbl_804D6DE4;
        if (fabs(up) > lbl_804D6DAC && aimSet) {
            angle = (aim[0] * pos[0] + aim[1] * pos[1] + aim[2] * pos[2]) /
                    lenSq;
        }

        if (*(int*)(o + 0x2d0) != -1) {
            float pHit[3];
            fn_800BB8B8(ownerPtr, pHit);
            pHit[0] -= *(float*)(o + 0x84);
            pHit[1] -= *(float*)(o + 0x88);
            pHit[2] -= *(float*)(o + 0x8c);
            float dist = fn_800E1020(pHit);
            float speed = lbl_804D6DA8;
            if (lbl_804D6DA8 < lenSq) {
                speed = sqrtf(lenSq);
            }
            if (dist < (*(float*)(o + 0x138) * speed) / lbl_804D6DC0) {
                fn_800E1098((lbl_804D6DC0 * dist) / *(float*)(o + 0x138),
                            aim, aim);
                lenSq = dist * dist;
            }
        }

        if (slope <= proj || angle <= slope ||
            HasCondition(vel, 0x2d) == 0) {
            if (slope <= proj || HasCondition(vel, 0x2d) == 0) {
                if (speedSq < lenSq) {
                    fn_800E1098(*(float*)(o + 0x28c), aim, aim);
                }
            } else if (aimSet) {
                float k = -fn_800E1020(sum);
                sum[0] = aim[0] * k;
                sum[1] = aim[1] * k;
                sum[2] = aim[2] * k;
                for (int i = 0; i < 3; i++) {
                    if (aim[i] != lbl_804D6DA8) {
                        float next = sum[i] + aim[i];
                        float nextSign = (next < lbl_804D6DA8)
                                             ? lbl_804D6DF8
                                             : lbl_804D6DB0;
                        float curSign = (aim[i] < lbl_804D6DA8)
                                            ? lbl_804D6DF8
                                            : lbl_804D6DB0;
                        aim[i] = (curSign == nextSign) ? next : lbl_804D6DA8;
                    }
                }
            }
        } else {
            if (fabs(aim[1] * sum[2] - aim[2] * sum[1]) <= lbl_804D6DAC &&
                fabs(aim[2] * sum[0] - aim[0] * sum[2]) <= lbl_804D6DAC &&
                fabs(aim[0] * sum[1] - aim[1] * sum[0]) <= lbl_804D6DAC) {
                aim[0] -= sum[0];
                aim[1] -= sum[1];
                aim[2] -= sum[2];
            }
        }

        *(float*)(o + 0x26c) = *(float*)(o + 0x288);
        if (flag) {
            float dir[3];
            fn_8016D440(dir, aim);
            pos[0] = aim[0];
            pos[1] = aim[1];
            pos[2] = aim[2];
            Vec3Set((Vec3*)(o + 0x27c), dir[2] - *(float*)(o + 0x254),
                    dir[0] - *(float*)(o + 0x24c), lbl_804D6DA8);
            if (HasCondition(vel, 0x38) != 0) {
                *(float*)(o + 0x284) += lbl_804D6DE0;
            }
            EulerNormalize((Vec3*)(o + 0x27c), (Vec3*)(o + 0x27c));
        }
    }

    ConditionSetBit(vel, 0x40, 0);
    for (int i = 1; i < 4; i++) {
        if (lbl_804D6DAC < weight[i]) {
            ConditionSetBit(vel, 0x40, 1);
            break;
        }
    }
    result = 1;
    return result;
}

// Clear the first of the four group handles at object+0x5a0 that equals `id`.
extern "C" int fn_800B6CAC(u32* object, u32 id)
{
    if (object == 0) {
        return 0;
    }
    for (int i = 0; i < 4; i++) {
        if (object[0x5a0 / 4 + i] == id) {
            object[0x5a0 / 4 + i] = 0;
            return 1;
        }
    }
    return 0;
}

// Store `value` in the first free group-handle word at object+0x5a0.
extern "C" int fn_800B6D04(u32* object, u32 value)
{
    if (object == 0) {
        return 0;
    }
    u32* base = object;
    for (int i = 0; i < 4; i++) {
        if (object[0x5a0 / 4 + i] == 0) {
            base[0x5a0 / 4 + i] = value;
            return 1;
        }
    }
    return 0;
}

// Clear the four group-handle bookkeeping words at object+0x5a0.
extern "C" int fn_800B6D58(u8* object)
{
    if (object == 0) {
        return 0;
    }
    ((u32*)object)[0x5a0 / 4] = 0;
    ((u32*)object)[0x5a4 / 4] = 0;
    ((u32*)object)[0x5a8 / 4] = 0;
    ((u32*)object)[0x5ac / 4] = 0;
    return 1;
}

// --- group-target helpers ------------------------------------------------
extern float lbl_804D6E04;
extern float lbl_804D6E08;
extern float lbl_804D6E0C;
extern float lbl_804D6E10;
extern float lbl_804D6E14;
extern float lbl_804D6E18;
extern float lbl_804D6E1C;

extern "C" void* fn_800A3990(void* object);
extern "C" int   fn_800E0DBC(void* vector);
extern "C" void  fn_800BBCA8(void* object);
extern "C" void  fn_801BDC3C(int actor, void* object);
extern "C" void  fn_801BE7FC(int actor, int value, void* object);

void* GetPlayer();
int   VarObjValue2(u32 varObject, int which);
void  ConditionUpdateBit(void* condition, int index, int value);
void  FollowOffsetReset(void* object);
void  FollowOffsetUpdate(void* object);

typedef void (*VoidFn)(void*);

// Bind the group named by `handle` to `object`: resolve the group's two
// var-objects and install the follow-offset callback on the object.
int OwnerGroupBindTarget(u32 handle, void* object)
{
    if ((handle >> 24) != 3 || (int)(handle & 0xffffff) >= lbl_804D4838) {
        return 0;
    }
    if (object == 0) {
        return 0;
    }

    GroupEntry* entry =
        (GroupEntry*)((u8*)lbl_804D4830 + (handle & 0xffffff) * 0x78);
    int first = VarObjValue2(entry->data->id, 0);
    int second = VarObjValue2(entry->data->id, 1);
    if (first != -1 && first != 0 && second != -1 && second != 0) {
        u8* obj = (u8*)object;
        *(u32*)(obj + 0x608) = handle;

        u8 saved[8];
        memcpy(saved, obj + 0x294, 8);
        fn_800BBCA8(obj);
        fn_800B4F98(obj);
        FollowOffsetReset(obj);
        *(VoidFn*)(obj + 0x10c) = FollowOffsetUpdate;
        *(u32*)(obj + 0x220) = 0;
        memcpy(obj + 0x294, saved, 8);
        ConditionUpdateBit(obj + 0x270, 0x15, 0);
        fn_801BDC3C(first, obj);
        fn_801BE7FC(second, *(int*)(*(u8**)(obj + 0xc4) + 0x1c), obj);
    }
    return 1;
}

// Set or clear the secondary flag of the group named by `handle`.
extern "C" int fn_800B6EDC(u32 handle, int enable)
{
    int type = handle >> 24;
    if (type != 3 || (int)(handle & 0xffffff) >= lbl_804D4838) {
        return 0;
    }
    int index = handle & 0xffffff;
    GroupEntry* entry = (GroupEntry*)((u8*)lbl_804D4830 + index * 0x78);
    if (enable != 0) {
        entry->flags |= 2;
    } else {
        entry->flags &= ~2;
    }
    return 1;
}

// Group field at offset 0x70 of `entry`.
extern "C" u32 fn_800B6F38(void* entry)
{
    return *(u32*)((u8*)entry + 0x70);
}

// Set or clear the primary flag of the group named by `handle`; clearing it
// re-resolves the group's asset id.
extern "C" int fn_800B6F40(u32 handle, int enable)
{
    int type = handle >> 24;
    if (type != 3 || (int)(handle & 0xffffff) >= lbl_804D4838) {
        return 0;
    }
    int index = handle & 0xffffff;
    GroupEntry* entry = (GroupEntry*)((u8*)lbl_804D4830 + index * 0x78);
    if (enable != 0) {
        entry->flags |= 1;
    } else {
        entry->flags &= ~1;
        if (entry->data->flag != 0) {
            entry->asset = AssetFind(0xffffffff, entry->data->flag);
        } else {
            entry->asset = 0xffffffff;
        }
    }
    return 1;
}

// Primary flag of the group named by `handle`.
extern "C" u32 fn_800B6FE4(u32 handle)
{
    int type = handle >> 24;
    if (type != 3 || (int)(handle & 0xffffff) >= lbl_804D4838) {
        return 0;
    }
    int index = handle & 0xffffff;
    return *(u32*)((u8*)lbl_804D4830 + index * 0x78 + 0x6c) & 1;
}

// Third data word of the group named by `handle`.
extern "C" u32 fn_800B7020(u32 handle)
{
    int type = handle >> 24;
    if (type != 3 || (int)(handle & 0xffffff) >= lbl_804D4838) {
        return 0;
    }
    int index = handle & 0xffffff;
    return (*(GroupData**)((u8*)lbl_804D4830 + index * 0x78 + 0x68))->field0C;
}

// Data id of the group named by `handle`.
extern "C" u32 fn_800B705C(u32 handle)
{
    int type = handle >> 24;
    if (type != 3 || (int)(handle & 0xffffff) >= lbl_804D4838) {
        return 0;
    }
    int index = handle & 0xffffff;
    return (*(GroupData**)((u8*)lbl_804D4830 + index * 0x78 + 0x68))->id;
}

// Zero a direction vector that is already within the epsilon box, otherwise
// scale it to unit length.
static inline void NormalizeSmallVec(float* v)
{
    int outside = (fabs(v[0]) > lbl_804D6E0C) + (fabs(v[1]) > lbl_804D6E0C) +
                  (fabs(v[2]) > lbl_804D6E0C);
    if (outside == 0) {
        v[0] = lbl_804D6E04;
        v[1] = lbl_804D6E04;
        v[2] = lbl_804D6E04;
    } else {
        float inv = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        v[0] *= inv;
        v[1] *= inv;
        v[2] *= inv;
    }
}

// Return whether the group named by `handle` is a valid aim target for
// `object`; the group's data id selects the facing test that is applied.
// fn_800B7098
extern "C" int fn_800B7098(u32 handle, void* object, u32 sender, int arg)
{
    if (handle == 0xffffffff || (int)(handle & 0xffffff) >= lbl_804D4838 ||
        handle == sender) {
        return 0;
    }

    GroupEntry* entry =
        (GroupEntry*)((u8*)lbl_804D4830 + (handle & 0xffffff) * 0x78);
    u8* owner = (u8*)fn_800A3990(object);
    if (owner == 0) {
        return 0;
    }

    u32 query[8];
    memset(query, 0, 0x20);
    if (SendMsg((int)sender, 0x400, query) == 0 || query[0] == 0) {
        return 0;
    }
    u8* target = (u8*)query[0];

    if (MemFlagIsSet(owner + 0x270, 0x2a) != 0 &&
        *(int*)((u8*)entry + 0x74) != *(int*)(owner + 0x2c4)) {
        return 0;
    }

    float px = (float)*(s32*)(target + 0x24);
    float py = (float)*(s32*)(target + 0x28);
    float pz = (float)*(s32*)(target + 0x2c);
    GroupData* data = entry->data;
    float dx = data->x - px;
    float dy = data->y - py;
    float dz = data->z - pz;

    if (lbl_804D6E04 + (px - data->x) * (px - data->x) +
            (py - data->y) * (py - data->y) < lbl_804D6E08) {
        return 0;
    }
    if ((entry->flags & 4) == 0) {
        return 0;
    }

    float ox = *(float*)(owner + 0x84) - px;
    float oy = *(float*)(owner + 0x88) - py;
    float oz = *(float*)(owner + 0x8c) - pz;
    if (oz * dz + (ox * dx + oy * dy) < lbl_804D6E04) {
        return 0;
    }

    if (*(int*)(owner + 0x908) == -0x732e711f) {
        u8* player = (u8*)GetPlayer();
        if (player != 0) {
            int delta[3];
            delta[0] = *(int*)(player + 0x24) - *(int*)((u8*)entry + 0x24);
            delta[1] = *(int*)(player + 0x28) - *(int*)((u8*)entry + 0x28);
            delta[2] = *(int*)(player + 0x2c) - *(int*)((u8*)entry + 0x2c);
            float dist = (float)fn_800E0DBC(delta);
            if (lbl_804D6E04 < *(float*)(owner + 0x5d8) &&
                *(float*)(owner + 0x5d8) < dist) {
                return 0;
            }
            if (*(float*)(owner + 0x5dc) != lbl_804D6E04 &&
                dist < *(float*)(owner + 0x5dc)) {
                return 0;
            }
        }
    }

    s32 id = (s32)data->id;
    if (id == 0x7de5b6e) {
        int aimv[3];
        ActorAim(aimv, (Actor*)entry, 1);
        float aim[3];
        aim[0] = (float)aimv[0];
        aim[1] = (float)aimv[1];
        aim[2] = (float)aimv[2];
        NormalizeSmallVec(aim);

        float off[3];
        off[0] = (float)(*(int*)(target + 0x24) - *(int*)((u8*)entry + 0x24));
        off[1] = (float)(*(int*)(target + 0x28) - *(int*)((u8*)entry + 0x28));
        off[2] = (float)(*(int*)(target + 0x2c) - *(int*)((u8*)entry + 0x2c));
        NormalizeSmallVec(off);

        if (lbl_804D6E1C <= dz * dz + (dx * dx + dy * dy)) {
            float limit = arg != 0 ? lbl_804D6E14 : lbl_804D6E18;
            if (aim[2] * off[2] + aim[0] * off[0] + aim[1] * off[1] > limit) {
                return 1;
            }
        }
        return 0;
    }

    if (id < 0x7de5b6e) {
        if (id == -0x9b7f9f1) {
            int aimv[3];
            ActorAim(aimv, (Actor*)entry, 1);
            float aim[3];
            aim[0] = (float)aimv[0];
            aim[1] = (float)aimv[1];
            aim[2] = (float)aimv[2];
            NormalizeSmallVec(aim);

            float off[3];
            off[0] = (float)(*(int*)(target + 0x24) - *(int*)((u8*)entry + 0x24));
            off[1] = (float)(*(int*)(target + 0x28) - *(int*)((u8*)entry + 0x28));
            off[2] = (float)(*(int*)(target + 0x2c) - *(int*)((u8*)entry + 0x2c));
            NormalizeSmallVec(off);

            if (lbl_804D6E10 <= dz * dz + (dx * dx + dy * dy)) {
                float limit = arg != 0 ? lbl_804D6E14 : lbl_804D6E18;
                if (aim[2] * off[2] + aim[0] * off[0] + aim[1] * off[1] >
                        limit &&
                    aim[0] * off[1] - aim[1] * off[0] > lbl_804D6E04) {
                    return 1;
                }
            }
            return 0;
        }
        if (id < -0x9b7f9f1 && id == -0x73eff26c) {
            int aimv[3];
            ActorAim(aimv, (Actor*)entry, 1);
            float aim[3];
            aim[0] = (float)aimv[0];
            aim[1] = (float)aimv[1];
            aim[2] = (float)aimv[2];
            NormalizeSmallVec(aim);

            float off[3];
            off[0] = (float)(*(int*)(target + 0x24) - *(int*)((u8*)entry + 0x24));
            off[1] = (float)(*(int*)(target + 0x28) - *(int*)((u8*)entry + 0x28));
            off[2] = (float)(*(int*)(target + 0x2c) - *(int*)((u8*)entry + 0x2c));
            NormalizeSmallVec(off);

            if (lbl_804D6E10 <= dz * dz + (dx * dx + dy * dy)) {
                float limit = arg != 0 ? lbl_804D6E14 : lbl_804D6E18;
                if (aim[2] * off[2] + aim[0] * off[0] + aim[1] * off[1] >
                        limit &&
                    aim[0] * off[1] - aim[1] * off[0] < lbl_804D6E04) {
                    return 1;
                }
            }
            return 0;
        }
    } else if (id == 0x356c05f2) {
        return 1;
    }
    return 0;
}

// Forward the whole object to the copy helper with flag 0.
extern "C" void fn_800B7CBC(void* a, void* b)
{
    fn_800B84E4(a, b, 0);
}

// --- group aim-target search --------------------------------------------
extern float lbl_804D6E28;
extern float lbl_804D6E2C;
extern float lbl_804D6E30;
extern float lbl_804D6E34;

void* OwnerGetInfo(u32 ownerId);
extern "C" int fn_800CAB48(void* actor, void* position);

// Search the group table for the best aim target seen from `target`. The
// owner's weapon-direction vector (from OwnerGetInfo) decides whether the
// groups are ranked by distance or by the dot product of that direction and
// the direction to the group. Returns the winning group handle, or -1.
extern "C" int fn_800B7CE0(void* target)
{
    float bestDist = lbl_804D6E28;
    float bestDot = lbl_804D6E2C;
    int best = -1;
    int bestCab = 0;
    u8* owner = 0;
    int msg = 0;

    if (SendMsg((int)target, 0x400, &msg) == 0 || msg == 0) {
        return -1;
    }

    u8* base = (u8*)msg;
    float bx = (float)*(int*)(base + 0x24);
    float by = (float)*(int*)(base + 0x28);
    float bz = (float)*(int*)(base + 0x2c);

    if ((*(u32*)(base + 0x38) >> 24) == 0x13) {
        owner = base;
    }

    float dir[3];
    if (owner != 0 && *(int*)(owner + 0x5e8) != -1) {
        u8* info = (u8*)OwnerGetInfo(*(u32*)(owner + 0x5e8));
        if (info == 0) {
            dir[0] = lbl_804D6E04;
            dir[1] = lbl_804D6E04;
            dir[2] = lbl_804D6E04;
        } else {
            dir[0] = (float)*(int*)(info + 0x24) - bx;
            dir[1] = (float)*(int*)(info + 0x28) - by;
            dir[2] = (float)*(int*)(info + 0x2c) - bz;
            NormalizeSmallVec(dir);
        }
    }

    for (int i = 0; i < lbl_804D4838; i++) {
        u8* entry = (u8*)lbl_804D4830 + i * 0x78;
        u32 flags = *(u32*)(entry + 0x6c);
        if ((flags & 4) == 0 || (flags & 3) != 0) {
            continue;
        }
        if (owner != 0 && *(int*)(owner + 0x5e0) == *(int*)(entry + 0x38) &&
            *(float*)(owner + 0x5e4) < lbl_804D6E30) {
            continue;
        }

        int aim[3];
        ActorAim(aim, (Actor*)entry, 1);
        float aimDir[3];
        aimDir[0] = (float)aim[0];
        aimDir[1] = (float)aim[1];
        aimDir[2] = (float)aim[2];
        NormalizeSmallVec(aimDir);

        float toEntry[3];
        toEntry[0] = (float)*(int*)(entry + 0x24) - bx;
        toEntry[1] = (float)*(int*)(entry + 0x28) - by;
        toEntry[2] = (float)*(int*)(entry + 0x2c) - bz;
        NormalizeSmallVec(toEntry);

        float dot = aimDir[0] * toEntry[0] + aimDir[1] * toEntry[1] +
                    aimDir[2] * toEntry[2];
        if (dot < lbl_804D6E34) {
            continue;
        }

        float distVec[3];
        distVec[0] = (float)*(int*)(entry + 0x24) - bx;
        distVec[1] = (float)*(int*)(entry + 0x28) - by;
        distVec[2] = (float)*(int*)(entry + 0x2c) - bz;
        float dist = distVec[0] * distVec[0] + distVec[1] * distVec[1] +
                     distVec[2] * distVec[2];
        float limit = lbl_804D6E04;
        if (owner != 0) {
            limit = *(float*)(owner + 0x900) * *(float*)(owner + 0x900);
        }
        if (dist < limit) {
            continue;
        }

        int cab = fn_800CAB48(base, entry + 0x24);
        if (cab == 0 && bestCab == 1) {
            continue;
        }

        NormalizeSmallVec(distVec);
        float dot2 = dir[0] * distVec[0] + dir[1] * distVec[1] +
                     dir[2] * distVec[2];
        int dirOutside = (fabs(dir[0]) > lbl_804D6E0C) +
                         (fabs(dir[1]) > lbl_804D6E0C) +
                         (fabs(dir[2]) > lbl_804D6E0C);
        if (dirOutside == 0 || dot2 > lbl_804D6E34) {
            if (dist >= bestDist) {
                continue;
            }
        } else if (dot2 < bestDot) {
            continue;
        }

        bestDist = dist;
        bestDot = dot2;
        bestCab = cab;
        best = *(u32*)(entry + 0x38);
    }
    return best;
}

// Same group aim-target search as fn_800B7CE0, but for the pair (a, b):
// fn_800B7098 rejects the groups that cannot be aimed at, and the
// winning distance is also returned through `outDist`. Returns the winning
// group handle, or -1.
extern "C" int fn_800B84E4(void* a, void* b, float* outDist)
{
    float bestDist = lbl_804D6E28;
    float bestDot = lbl_804D6E2C;
    int best = -1;
    int bestCab = 0;
    u8* owner = 0;
    int msg[8];

    if (SendMsg((int)a, 0x400, msg) == 0 || msg[0] == 0) {
        return -1;
    }

    u8* base = (u8*)msg[0];
    float bx = (float)*(int*)(base + 0x24);
    float by = (float)*(int*)(base + 0x28);
    float bz = (float)*(int*)(base + 0x2c);

    if (SendMsg((int)b, 0x400, msg) == 0 || msg[0] == 0) {
        return -1;
    }

    if ((*(u32*)(base + 0x38) >> 24) == 0x13) {
        owner = base;
    }

    float dir[3];
    if (owner != 0 && *(int*)(owner + 0x5e8) != -1) {
        u8* info = (u8*)OwnerGetInfo(*(u32*)(owner + 0x5e8));
        if (info == 0) {
            dir[0] = lbl_804D6E04;
            dir[1] = lbl_804D6E04;
            dir[2] = lbl_804D6E04;
        } else {
            dir[0] = (float)*(int*)(info + 0x24) - bx;
            dir[1] = (float)*(int*)(info + 0x28) - by;
            dir[2] = (float)*(int*)(info + 0x2c) - bz;
            NormalizeSmallVec(dir);
        }
    }

    for (int i = 0; i < lbl_804D4838; i++) {
        u8* entry = (u8*)lbl_804D4830 + i * 0x78;
        u32 flags = *(u32*)(entry + 0x6c);
        if ((flags & 4) == 0 || (flags & 3) != 0) {
            continue;
        }
        if (owner != 0 && *(int*)(owner + 0x5e0) == *(int*)(entry + 0x38) &&
            *(float*)(owner + 0x5e4) < lbl_804D6E30) {
            continue;
        }

        if (fn_800B7098(*(u32*)(entry + 0x38), a, (u32)b, 0) == 0) {
            continue;
        }

        float distVec[3];
        distVec[0] = (float)*(int*)(entry + 0x24) - bx;
        distVec[1] = (float)*(int*)(entry + 0x28) - by;
        distVec[2] = (float)*(int*)(entry + 0x2c) - bz;
        float dist = distVec[0] * distVec[0] + distVec[1] * distVec[1] +
                     distVec[2] * distVec[2];
        float limit = lbl_804D6E04;
        if (owner != 0) {
            limit = *(float*)(owner + 0x900) * *(float*)(owner + 0x900);
        }
        if (dist < limit) {
            continue;
        }

        int cab = fn_800CAB48(base, entry + 0x24);
        if (cab == 0 && bestCab == 1) {
            continue;
        }

        NormalizeSmallVec(distVec);
        float dot2 = dir[0] * distVec[0] + dir[1] * distVec[1] +
                     dir[2] * distVec[2];
        int dirOutside = (fabs(dir[0]) > lbl_804D6E0C) +
                         (fabs(dir[1]) > lbl_804D6E0C) +
                         (fabs(dir[2]) > lbl_804D6E0C);
        if (dirOutside == 0 || dot2 > lbl_804D6E34) {
            if (dist >= bestDist) {
                continue;
            }
        } else if (dot2 < bestDot) {
            continue;
        }

        bestDist = dist;
        bestDot = dot2;
        bestCab = cab;
        best = *(u32*)(entry + 0x38);
    }
    if (outDist != 0) {
        *outDist = bestDist;
    }
    return best;
}

// Fold `callback` over every group that matches the given data kind/id.
extern "C" int fn_800B8A7C(u32 kind, u32 dataId, int needUnused,
                           int (*callback)(u32, int, int), int arg)
{
    int result = -1;
    for (int i = 0; i < lbl_804D4838; i++) {
        GroupEntry* entry = (GroupEntry*)((u8*)lbl_804D4830 + i * 0x78);
        if (kind != 0 && kind != entry->data->field0C) {
            continue;
        }
        if (dataId != 0 && dataId != entry->data->id) {
            continue;
        }
        if (!(entry->flags & 4)) {
            continue;
        }
        if (entry->flags & 3) {
            continue;
        }
        if (needUnused != 0 && entry->asset != 0xffffffff) {
            continue;
        }
        result = callback(entry->field38, result, arg);
    }
    return result;
}

// Collect every group matching the requested data kind/id (and optional
// unused-asset / predicate filters) into a temporary buffer, then return one
// of the collected handles at random. Returns -1 if nothing matched.
extern "C" int fn_800B8B64(u32 kind, u32 dataId, int needUnused,
                           int (*callback)(u32, int), int arg)
{
    Buffer buf;
    buf.vtable = lbl_8039B708;

    int offset = 0;
    for (int i = 0; i < lbl_804D4838; i++) {
        GroupEntry* entry = (GroupEntry*)((u8*)lbl_804D4830 + offset);
        if (kind == 0 || kind == entry->data->field0C) {
            if (dataId == 0 || dataId == entry->data->id) {
                if ((entry->flags & 4) != 0 && (entry->flags & 3) == 0 &&
                    (needUnused == 0 || (s32)entry->asset == -1) &&
                    (callback == 0 || callback(entry->field38, arg) != 0)) {
                    u32* slot =
                        (u32*)fn_8028B3BC(&buf, buf.data + buf.size, 4);
                    *slot = *(u32*)((u8*)lbl_804D4830 + offset + 0x38);
                }
            }
        }
        offset += 0x78;
    }

    u32 count = buf.size >> 2;
    u32 value = 0xffffffff;
    if (count != 0) {
        value = ((u32*)buf.data)[(u32)rand() % count];
    }

    buf.vtable = lbl_8039B708;
    fn_8028B960(&buf);
    buf.~Buffer();
    return (int)value;
}

// Destroy the buffer held at `object`, optionally freeing the storage.
extern "C" void* fn_800B8D00(void* object, short freeFlag)
{
    if (object != 0) {
        *(char**)object = lbl_8039B708;
        fn_8028B960(object);
        ((Buffer*)object)->~Buffer();
        if (freeFlag > 0) {
            VFree(object);
        }
    }
    return object;
}

// Same random-handle pick as fn_800B8B64, without the predicate filter.
extern "C" int fn_800B8D68(u32 kind, u32 dataId, int needUnused)
{
    Buffer buf;
    buf.vtable = lbl_8039B708;

    int offset = 0;
    for (int i = 0; i < lbl_804D4838; i++) {
        GroupEntry* entry = (GroupEntry*)((u8*)lbl_804D4830 + offset);
        if (kind == 0 || kind == entry->data->field0C) {
            if (dataId == 0 || dataId == entry->data->id) {
                if ((entry->flags & 4) != 0 && (entry->flags & 3) == 0 &&
                    (needUnused == 0 || (s32)entry->asset == -1)) {
                    u32* slot =
                        (u32*)fn_8028B3BC(&buf, buf.data + buf.size, 4);
                    *slot = *(u32*)((u8*)lbl_804D4830 + offset + 0x38);
                }
            }
        }
        offset += 0x78;
    }

    u32 count = buf.size >> 2;
    u32 value = 0xffffffff;
    if (count != 0) {
        value = ((u32*)buf.data)[(u32)rand() % count];
    }

    buf.vtable = lbl_8039B708;
    fn_8028B960(&buf);
    buf.~Buffer();
    return (int)value;
}

// Group-table query forwarding to the internal matcher with default tail args.
extern "C" void fn_800B9104(void* a, void* b, void* c, void* d)
{
    fn_800B8ED0(a, b, c, d, 0, 0);
}

// Group nearest-match query with default tail args.
extern "C" void fn_800B9360(void* a, void* b, void* c, void* d)
{
    fn_800B912C(a, b, c, d, 0, 0);
}

// Count groups whose data carries `id`, optionally skipping unused ones.
extern "C" int fn_800B9388(u32 id, int skipUnused)
{
    GroupEntry* entry = (GroupEntry*)lbl_804D4830;
    int count = 0;
    for (int i = lbl_804D4838; i > 0; i--) {
        if ((skipUnused != 0 || entry->data->flag == 0) && id == entry->data->id) {
            count++;
        }
        entry = (GroupEntry*)((u8*)entry + 0x78);
    }
    return count;
}

// Emit a handle for every group whose data id appears in `ids`.
extern "C" int fn_800B93DC(u32* ids, int idCount, u32* out, int max, int includeUnused)
{
    int i = 0;
    int count = 0;
    for (; i < *(int volatile*)&lbl_804D4838; i++) {
        if (includeUnused == 0 &&
            ((s32)((GroupEntry*)((u8*)*(u32* volatile*)&lbl_804D4830 + i * 0x78))->asset) != -1) {
            continue;
        }
        u32 handle = (i & 0xffffff) | 0x03000000;
        for (int j = 0; j < idCount; j++) {
            if (((GroupEntry*)((u8*)*(u32* volatile*)&lbl_804D4830 + i * 0x78))->data->id == ids[j]) {
                *out++ = handle;
                count++;
                if (count == max) {
                    return count;
                }
            }
        }
    }
    return count;
}

// Collect up to `max` handles of groups whose data id equals `id`.
extern "C" int fn_800B949C(u32 id, u32* out, int max, int includeUnused)
{
    int i = 0;
    int count = 0;
    for (; i < *(int volatile*)&lbl_804D4838 && count < max; i++) {
        if (includeUnused == 0 &&
            ((s32)((GroupEntry*)((u8*)*(u32* volatile*)&lbl_804D4830 + i * 0x78))->asset) != -1) {
            continue;
        }
        if (id == ((GroupEntry*)((u8*)*(u32* volatile*)&lbl_804D4830 + i * 0x78))->data->id) {
            *out++ = (i & 0xffffff) | 0x03000000;
            count++;
        }
    }
    return count;
}

// Number of groups in the table.
extern "C" int fn_800B9518(void)
{
    return lbl_804D4838;
}

extern "C" int fn_800B9520(void)
{
    return 1;
}


// Rebuild every group slot from its group data: bone matrix, placement and the
// actor used to display it.  Address-based name (0x800B969C).
extern "C" void fn_800B969C(void)
{
    int offset = 0;

    for (int i = 0; i < lbl_804D4838; i++, offset += 0x78) {
        GroupEntry* entry = (GroupEntry*)((u8*)lbl_804D4830 + offset);
        GroupData*  data  = entry->data;
        u32        index  = (u32)((u8*)entry - (u8*)lbl_804D4830) / 0x78;
        GroupEntry* slot  = (GroupEntry*)((u8*)lbl_804D4830 + index * 0x78);

        slot->data = data;

        Mtx34 matrix;
        Mtx34 place;
        BoneGetMatrix(&matrix, (u8*)data + 0x1c);
        PlacementToMatrix(&place, &matrix);
        place.cellX = (s32)data->x;
        place.cellY = (s32)data->y;
        place.cellZ = (s32)data->z;

        slot->field50 = -data->field00;
        slot->field54 = -data->field00;
        slot->field58 = -data->field00;
        slot->field5C = data->field00;
        slot->field60 = data->field00;
        slot->field64 = data->field00;

        ActorFn8002378C((Actor*)slot, (int)(index & 0xffffff) | 0x3000000,
                        (int)&place, (int)GroupMessageHandler, (int)&slot->field50,
                        (int)&slot->field50, 0);
        ActorFn800231A8((Actor*)slot);

        if (data->flag != 0)
            slot->asset = AssetFind(0xffffffff, data->flag);
        else
            slot->asset = 0xffffffff;

        slot->flags = 4;
        slot->field74 = fn_800BF2D0(&data->x);

        if (SelectionSetTest(data->field2C) == 0) {
            ActorRefreshHostState((Actor*)slot, 0);
            ObjSendC00(slot, 0);
        }
    }
}

extern "C" void fn_800B9824(void);

extern "C" void fn_800B985C(void);



// Decode the group-table header that `data` points at.
extern "C" void fn_800B9AE0(void* unused, u8* data, int size)
{
    u8* header = size > 0 ? data : 0;
    FileFlushBuffer(header, 0x10);
    if (*(int*)header == 0x0f) {
        u32 count = *(u32*)(header + 4);
        lbl_804D4838 = count;
        bool ok = false;
        if (size - 0x10 > 0 && count * 0x30 != 0) {
            ok = true;
        }
        u8* table;
        if (ok) {
            table = data + 0x10;
        } else {
            table = 0;
        }
        lbl_804D4834 = (int)table;
        FileFlushBuffer(table, *(int volatile*)&lbl_804D4838 * 0x30);
    }
}

// Resolve group `index` and hand it to the group initialiser.
extern "C" void fn_800B9B8C(int index, int msg, GroupEntry** out)
{
    *out = (GroupEntry*)((u8*)lbl_804D4830 + index * 0x78);
    GroupMessageHandler(msg, out);
}

// ---- message handler 0x800B9BC4 -------------------------------------------
// String scratch buffer used by the 0xad8576fa text query.  The character
// storage lives in the shared Buffer base class.
class String : public Buffer {
public:
    String();
};

// Integer vector used by the placement swap helpers.
class IVec3 {
public:
    s32 x;  // 0x00
    s32 y;  // 0x04
    s32 z;  // 0x08
};

// Payload of every group message.  `value`, `pos` and `rest` are one
// contiguous block that the 0x402/0x403 query copies a group record into.
struct GroupMsg {
    GroupEntry* actor;  // 0x00
    u32         arg1;   // 0x04
    u32         arg2;   // 0x08
    u32         arg3;   // 0x0c
    union {
        float f[3];
        s32   i[3];
    } value;            // 0x10
    s32         pos[3]; // 0x1c
    u32         rest[6];// 0x28
};

// Reply buffer of the 0x402 placement query.
struct GroupPlaceQuery {
    u32 field00;      // 0x00
    u16 field04;      // 0x04
    u8  pad06[0x0a];  // 0x06
    u8  place[0x24];  // 0x10
    s32 cellX;        // 0x34
    s32 cellY;        // 0x38
    s32 cellZ;        // 0x3c
};

extern float lbl_804D6E04;

extern "C" int  fn_800C5210(u32 entity, int msg, void* data);
extern "C" int  fn_80084540(u32 entity, int msg, void* data, void* out);
extern "C" void fn_8028BF80(void* string, const char* format);
extern "C" void fn_8028C188(void* out, u32 a, u32 b);
extern "C" void fn_8009F190(void* out, void* src);
extern "C" void fn_8009F028(void* actor, void* src);
extern "C" void fn_800BADAC(u32 id, u32 value);
extern "C" int  fn_800BA3CC(void* actor, void* data);

void VecSwapXY(IVec3* a, IVec3* b);
void VecSwapXY2(IVec3* a, IVec3* b);
void ActorRelinkOwners(Actor* actor);

// Group entry named by the packed actor handle in field38 (0x03xxxxxx).
static inline GroupEntry* GroupFromPackedId(u32 id)
{
    if ((id >> 24) == 3) {
        u32 index = id & 0xffffff;
        if ((s32)index < lbl_804D4838)
            return (GroupEntry*)((u8*)lbl_804D4830 + index * 0x78);
    }
    return 0;
}

int GroupDrawDebugOverlay(void* actorPtr, void* data);

// Group message handler: dispatch, then the per-message group bookkeeping.
int GroupMessageHandler(int msg, void* data)
{
    GroupMsg*   message = (GroupMsg*)data;
    GroupEntry* actor   = message->actor;
    int         result;

    result = fn_800C5210(actor->field38, msg, data);
    if (result != 0)
        return result;

    u8 dispatchOut[4];
    result = fn_80084540(actor->field38, msg, data, dispatchOut);
    if (result != 0)
        return result;

    switch (msg) {
    case 0x000:
        return GroupDrawDebugOverlay(actor, data);

    case 0x400: {
        if (actor == 0 || data == 0)
            return 0;
        u32 index = (u32)((s32)((u8*)actor - (u8*)lbl_804D4830) / 0x78);
        message->arg1 = (index & 0xffffff) | 0x3000000;
        message->pos[0] = 0;
        message->arg3 = (actor->flags >> 2) & 1;
        message->value.f[0] = lbl_804D6E04;
        message->value.f[1] = lbl_804D6E04;
        message->value.i[2] = 3;
        return 1;
    }

    case 0x402:
    case 0x403:
    case 0x411:
    case 0x412: {
        if (actor == 0 || data == 0)
            return 0;
        u32*       dst = (u32*)&message->value;
        const u32* src = (const u32*)actor;
        message->arg2 = 0;
        for (int i = 0; i < 12; i++)
            dst[i] = src[i];
        return 1;
    }

    case 0x404: {
        if (actor == 0 || data == 0 || message->arg1 == 0)
            return 0;
        const u32* src = (const u32*)message->arg1;
        u32*       dst = (u32*)actor;
        for (int i = 0; i < 12; i++)
            dst[i] = src[i];
        return 1;
    }

    case 0x41a:
        message->arg1 = actor->asset;
        return 1;

    case 0x41d: {
        u32 hash = message->arg1;
        if (hash == 0x33032fa9) {
            GroupEntry* entry = GroupFromPackedId(actor->field38);
            if (entry != 0) {
                if (message->arg3 == 0) {
                    entry->flags &= ~1u;
                    if (entry->data->flag == 0)
                        entry->asset = 0xffffffff;
                    else
                        entry->asset = AssetFind(0xffffffff, entry->data->flag);
                } else {
                    entry->flags |= 1;
                }
            }
            return 1;
        }
        if (hash == 0x2ccf78b9) {
            fn_800BADAC(actor->field38, message->arg3);
            return 1;
        }
        if (hash == 0x7c18fe9e) {
            actor->asset = message->arg3;
            return 1;
        }
        return 0;
    }

    case 0x41e: {
        u32 hash = message->arg1;
        if (hash == 0x40efe5f) {
            GroupEntry* entry = GroupFromPackedId(actor->field38);
            message->arg3 = entry ? entry->data->field0C : 0;
            return 0;
        }
        if (hash == 0x3d3b158) {
            GroupEntry* entry = GroupFromPackedId(actor->field38);
            message->arg3 = entry ? entry->data->id : 0;
            return 1;
        }
        if (hash == 0xad8576fa) {
            String text;
            if (actor->data->id != 0) {
                fn_8028BF80(&text, "Class: 0x%08x  ");
                fn_8028C188(&message->arg3, text.unk04, text.size);
            }
            if (actor->data->field0C != 0) {
                fn_8028BF80(&text, "Group: 0x%08x  ");
                fn_8028C188(&message->arg3, text.unk04, text.size);
            }
            return 1;
        }
        if (hash == 0x7c18fe9e) {
            if (actor->asset == 0xffffffff)
                return 0;
            message->arg3 = actor->asset;
            return 1;
        }
        if (hash == 0x33032fa9) {
            GroupEntry* entry = GroupFromPackedId(actor->field38);
            message->arg3 = entry ? (entry->flags & 1) : 0;
            return 1;
        }
        return 0;
    }

    case 0xc00:
        if (message->arg1 == 0)
            actor->flags &= ~4u;
        else
            actor->flags |= 4;
        return 1;

    case 0xc07: {
        if (actor == 0 || data == 0)
            return 0;
        if ((message->arg1 & 4) != 0) {
            GroupPlaceQuery query;
            IVec3           vec;
            query.field04 = 0xffff;
            if (SendMsg(message->arg2, 0x402, &query) == 0)
                return 0;
            fn_8009F190(&vec, query.place);
            VecSwapXY2((IVec3*)message->pos, &vec);
            message->value.i[0] = query.cellX;
            message->value.i[1] = query.cellY;
            message->value.i[2] = query.cellZ;
        }
        if ((message->arg1 & 2) != 0) {
            IVec3 vec;
            VecSwapXY(&vec, (IVec3*)message->pos);
            fn_8009F028(actor, &vec);
        }
        if ((message->arg1 & 1) != 0) {
            actor->field50 = message->value.i[0];
            actor->field54 = message->value.i[1];
            actor->field58 = message->value.i[2];
        }
        ActorRelinkOwners((Actor*)actor);
        return 1;
    }

    case 0xc14: {
        if (message->arg3 != 0x6c345908)
            return 0;
        GroupData*  gdata = actor->data;
        u32         index = (u32)((s32)((u8*)actor - (u8*)lbl_804D4830) / 0x78);
        GroupEntry* slot  = (GroupEntry*)((u8*)lbl_804D4830 + index * 0x78);

        slot->data = gdata;

        Mtx34 matrix;
        Mtx34 place;
        BoneGetMatrix(&matrix, (u8*)gdata + 0x1c);
        PlacementToMatrix(&place, &matrix);
        place.cellX = (s32)gdata->x;
        place.cellY = (s32)gdata->y;
        place.cellZ = (s32)gdata->z;

        slot->field50 = -gdata->field00;
        slot->field54 = -gdata->field00;
        slot->field58 = -gdata->field00;
        slot->field5C = gdata->field00;
        slot->field60 = gdata->field00;
        slot->field64 = gdata->field00;

        ActorFn8002378C((Actor*)slot, (int)(index & 0xffffff) | 0x3000000,
                        (int)&place, (int)GroupMessageHandler, (int)&slot->field50,
                        (int)&slot->field50, 0);
        ActorFn800231A8((Actor*)slot);

        if (gdata->flag != 0)
            slot->asset = AssetFind(0xffffffff, gdata->flag);
        else
            slot->asset = 0xffffffff;

        slot->flags = 4;
        slot->field74 = fn_800BF2D0(&gdata->x);

        if (SelectionSetTest(gdata->field2C) == 0) {
            ActorRefreshHostState((Actor*)slot, 0);
            ObjSendC00(slot, 0);
        }
        return 1;
    }

    default:
        return 1;
    }
}

void ActorGetMatrix(Mtx34* out, Actor* actor);
void DebugDrawAxes(Vec3* out, void* matrix, float scale);
void DebugDrawSphere(int r, int g, int b, int a, float x, float y, float z,
                     float radius);

extern float lbl_804D6E38;
extern float lbl_804D6E3C;
extern float lbl_804D6E40;
extern float lbl_804D6E44;
extern u32   lbl_804D6E00;

// Debug overlay for a group message target: axes from the actor matrix, a
// colour-coded sphere at the actor position, and the line to the actor the
// group is currently talking to.  Address-based name (0x800BA3CC).
int GroupDrawDebugOverlay(void* actorPtr, void* data)
{
    u8* actor = (u8*)actorPtr;

    if (fn_800BA7FC(0x15) != 0) {
        Mtx34 matrix;
        ActorGetMatrix(&matrix, (Actor*)actor);
        DebugDrawAxes((Vec3*)&matrix.cellX, &matrix, lbl_804D6E38);
    }

    if (fn_800BA7FC(0x16) != 0) {
        u32 flags = *(u32*)(actor + 0x6c);
        u32 color = lbl_804D6E00;
        u8* colorBytes = (u8*)&color;

        if (flags & 1) {
            colorBytes[0] = 0xff;
            colorBytes[1] = 0xff;
            colorBytes[2] = 0x00;
            colorBytes[3] = 0x40;
        } else if (flags & 2) {
            colorBytes[0] = 0x00;
            colorBytes[1] = 0xff;
            colorBytes[2] = 0xff;
            colorBytes[3] = 0x40;
        } else if (*(s32*)(actor + 0x70) != -1) {
            colorBytes[0] = 0xff;
            colorBytes[1] = 0x64;
            colorBytes[2] = 0x64;
            colorBytes[3] = 0x40;
        }
        if ((flags & 4) == 0)
            colorBytes[3] = 0x10;

        DebugDrawSphere(colorBytes[0], colorBytes[1], colorBytes[2], colorBytes[3],
                        (float)*(u32*)(actor + 0x24),
                        (float)*(u32*)(actor + 0x28),
                        (float)*(u32*)(actor + 0x2c),
                        (float)*(u32*)*(void**)(actor + 0x68));
    }

    if (fn_800BA7FC(0x17) != 0 && *(s32*)(actor + 0x70) != -1) {
        int target[8];

        if (SendMsg(*(s32*)(actor + 0x70), 0x400, target) != 0 && target[0] != 0) {
            u8* targetActor = (u8*)target[0];

            Vec3 pos;
            pos.x = (float)*(u32*)(actor + 0x24);
            pos.y = (float)*(u32*)(actor + 0x28);
            pos.z = (float)*(u32*)(actor + 0x2c);

            Vec3 targetPos;
            targetPos.x = (float)*(u32*)(targetActor + 0x24);
            targetPos.y = (float)*(u32*)(targetActor + 0x28);
            targetPos.z = (float)*(u32*)(targetActor + 0x2c);

            Vec3 dir;
            dir.x = pos.x - targetPos.x;
            dir.y = pos.y - targetPos.y;
            dir.z = pos.z - targetPos.z;

            fn_800E1098(lbl_804D6E3C, &dir.x, &dir.x);

            Vec3 end;
            dir.z = dir.z + lbl_804D6E40 + targetPos.z;
            end.x = dir.x + targetPos.x;
            end.y = dir.y + targetPos.y;
            end.z = dir.z - lbl_804D6E44;
            dir.x = end.x;
            dir.y = end.y;

            fn_800090BC(&targetPos, &pos, 0xff, 0xff, 0);
            fn_800090BC(&targetPos, &dir, 0xff, 0xff, 0);
            fn_800090BC(&targetPos, &end, 0xff, 0xff, 0);
        }
    }

    return 1;
}

// Copy `size` bytes through the unaligned helper after clearing the tail.
extern "C" void fn_800BA6FC(void* dst, u8* src, u32 size)
{
    fn_8028BC2C(dst, src, size);
    fn_800BA7AC(src, src + (size & ~3u));
}

// Clear the aligned tail, then copy `size` bytes from `src` to `dst`.
extern "C" void fn_800BA744(void* dst, u8* src, u32 size)
{
    fn_800BA7CC(src, src + (size & ~3u));
    fn_8028BC28(dst, src, size);
}

extern "C" int fn_800BA7A4(void)
{
    return 4;
}

extern "C" void fn_800BA7AC(u8* begin, u8* end)
{
    for (u8* p = begin; p < end; p += 4) {
    }
}

extern "C" void fn_800BA7CC(u8* begin, u8* end)
{
    for (u8* p = begin; p < end; p += 4) {
    }
}

extern "C" void fn_800BA7EC(int value)
{
    lbl_804D4840 = value;
}

extern "C" int fn_800BA7F4(void)
{
    return lbl_804D4840;
}

// Bit `index` of the 60-bit global flag set.
extern "C" u32 fn_800BA7FC(int index)
{
    if (index < 0 || index >= 0x3c) {
        return 0;
    }
    return lbl_804D2B90[index / 32] & (1 << (index % 32));
}

// Entry `index` of the 13-entry global table.
extern "C" u32 fn_800BA84C(int index)
{
    if (index < 0 || index >= 13) {
        return 0;
    }
    return lbl_8039B728[index];
}

// Store `value` into entry `index` of the 13-entry global table.
extern "C" void fn_800BA878(int index, u32 value)
{
    if (index >= 0 && index < 13) {
        lbl_8039B728[index] = value;
    } else {
        return;
    }
}

// Reset the group tables and tear down the related subsystems.
extern "C" void fn_800BA8A0(void)
{
    lbl_804D4840 = 0;
    fn_800A4F28();
    fn_800B9824();
    TalkFree();
    fn_800BF428();
    fn_8029B7D0();
}

// Scale the frame time by the global group count and drive all subsystems.
extern "C" void fn_800BA8D8(float time)
{
    float scaled = lbl_804D6E48 * (float)(s32)lbl_8039B728[0x0b] * time;
    fn_800A3AF4(scaled);
    fn_800B9528(scaled);
    fn_800B4D94(scaled);
    fn_800B4414(scaled);
    fn_800B29FC(scaled);
    TalkUpdate(scaled);
    fn_8029B85C(scaled);
}

extern "C" void fn_800BA970(void)
{
    fn_800A51B8();
    fn_800B985C();
}

extern "C" void fn_800BA994(void)
{
    fn_800B98F8();
    fn_800A5254();
    TalkInit();
}

// Initialise the group system and its subsystems.
extern "C" void fn_800BA9BC(void)
{
    lbl_804D4840 = 1;
    fn_80026660(0x27, (void*)fn_800AB47C, 0);
    fn_800BAA14();
    fn_800BF4B0();
    fn_800B9A8C();
    fn_800A54E8();
    fn_800B2BA4();
    fn_800B4EFC();
    fn_800B4614();
    fn_8029B920();
}

// Release the group table.
extern "C" void fn_800B9824(void)
{
    if (lbl_804D4830 != 0) {
        VFree(lbl_804D4830);
    }
    lbl_804D4834 = 0;
    lbl_804D4838 = 0;
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

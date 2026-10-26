// u_800B1834.cpp: owner behaviour-slot and animation bookkeeping.
// Drafted from the retail assembly; type and member names are descriptive guesses.

typedef unsigned char u8;
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
extern "C" void fn_800B84E4(void* a, void* b, int c);
extern "C" void fn_8028BC28(void* a, void* b, u32 c);
extern "C" int  fn_800A37CC(void* object);
void OwnerUpdateSlot1(Owner* owner);
extern "C" int  fn_800A3758(Owner* owner);
extern "C" int  fn_800B4944(Owner* owner, int* actorId, float* pos, float* outPos,
                            int* unk0C, u32* unk14, int a, int b);
extern "C" int  fn_800B39DC(Owner* owner, int* actorId, float* outPos, int* unk0C,
                            u32* unk14, int a, int b);
extern "C" void fn_8016D68C(void* dst, void* src);
extern "C" void fn_8028BC2C(void* a, void* b, u32 c);
extern "C" void* memset(void* dst, int value, u32 size);
extern "C" void FileFlushBuffer(void* dst, u32 size);
int HasCondition(void* object, int id);
extern "C" void fn_800BA7AC(u8* begin, u8* end);
extern "C" void fn_800BA7CC(u8* begin, u8* end);
extern "C" void fn_800B9BC4(void* object, void* out);
extern float lbl_804D6DA0;
extern float lbl_804D6DA4;

class Buffer {
public:
    ~Buffer();
};

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
    u8  pad004[4];
    u32 id;        // 0x04
    u32 flag;      // 0x08
    u32 field0C;   // 0x0c
};

struct GroupEntry {
    u8         pad038[0x38];
    u32        field38;  // 0x38
    u8         pad03C[0x2c];
    GroupData* data;     // 0x68
    u32        flags;    // 0x6c
    u32        asset;    // 0x70
};

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
int fn_800B261C(u32 owner, int index, int* out)
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
void OwnerUpdateSlot3(Owner* owner, int a, int b)
{
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

    int result = fn_800B4944(owner, &foundActorId, pos, foundPos, &foundUnk0C,
                             &foundUnk14, a, b);

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
void OwnerUpdateSlot2(Owner* owner, int a, int b)
{
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

        int result = fn_800B39DC(owner, &foundActorId, foundPos, &foundUnk0C,
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

// TODO fn_800B3818 (0x1C4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B3818(void) {}

// TODO fn_800B39DC (0x904 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" int fn_800B39DC(Owner* owner, int* actorId, float* outPos, int* unk0C,
                           u32* unk14, int a, int b) { return 0; }

// TODO fn_800B42E0 (0x134 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B42E0(void) {}



// True unless `p` already names `value` (a zero word never matches).
extern "C" int fn_800B466C(u32* p, u32 value)
{
    u32 current = p[0];
    if (current != 0 && current == value) {
        return 0;
    }
    return 1;
}

// TODO fn_800B4690 (0x15C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B4690(void) {}

// TODO fn_800B47EC (0x158 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B47EC(void) {}

// TODO fn_800B4944 (0x30C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" int fn_800B4944(Owner* owner, int* actorId, float* pos, float* outPos,
                           int* unk0C, u32* unk14, int a, int b) { return 0; }

// TODO fn_800B4C50 (0x144 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B4C50(void) {}



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

// TODO fn_800B5028 (0xBD0 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B5028(void) {}

// TODO fn_800B5BF8 (0x218 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B5BF8(void) {}

// TODO fn_800B5E10 (0xE9C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B5E10(void) {}

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

// TODO fn_800B6D84 (0x158 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B6D84(void) {}

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

// TODO fn_800B7098 (0xC24 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B7098(void) {}

// Forward the whole object to the copy helper with flag 0.
extern "C" void fn_800B7CBC(void* a, void* b)
{
    fn_800B84E4(a, b, 0);
}

// TODO fn_800B7CE0 (0x804 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B7CE0(void) {}

// TODO fn_800B84E4 (0x598 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B84E4(void* a, void* b, int c) {}

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

// TODO fn_800B8B64 (0x19C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B8B64(void) {}

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

// TODO fn_800B8D68 (0x168 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B8D68(void) {}

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


// TODO fn_800B969C (0x188 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B969C(void) {}

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
extern "C" void fn_800B9B8C(int index, void* object, GroupEntry** out)
{
    *out = (GroupEntry*)((u8*)lbl_804D4830 + index * 0x78);
    fn_800B9BC4(object, out);
}

// TODO fn_800B9BC4 (0x808 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B9BC4(void* object, void* out) {}

// TODO fn_800BA3CC (0x330 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800BA3CC(void) {}

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

struct ResetSlot2 {
    u8    pad00[0x0c];
    float value;      // 0x0c
    u8    pad10[0x0c];
    s32   state;      // 0x1c
};

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
        lbl_804497D0[i].state = -1;
    }
    lbl_804D4828 = 0;
}

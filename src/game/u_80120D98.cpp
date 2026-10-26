// u_80120D98.cpp: hazard spline zones and the hazard target-tracking code
// (0x80120D98-0x80125DE4). Names are descriptive guesses; the code is matched
// against the retail DOL.

// --- shared types for this unit (names/types are descriptive guesses) ---
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef int s32;
typedef float f32;

struct IVec3 {
    f32 x, y, z;
};
struct Vec3 {
    f32 x, y, z;
};
struct ZoneList;
struct SplineNameEntry {
    u32 unk[4]; // 0x10 bytes each
};

struct Actor {
    u8 pad00[0x24];
    IVec3 pos;        // 0x24
    u8 pad30[0x54];   // 0x30
    s32* typeId;      // 0x84
    u8 pad88[0x4];    // 0x88
    s32 handle;       // 0x8C
    s32 model;        // 0x90
    u8 pad94[0x10];   // 0x94
    u8* zone;         // 0xA4
    u8 padA8[0x4];    // 0xA8
    u32 flags;        // 0xAC
};

// Data owned by other units; placeholders where no name exists yet.
extern u8 lbl_803458D8[];                // shared string pool
extern u8 lbl_8039EED8[];                // spline span/bit table
extern SplineNameEntry lbl_8039F408[9];  // spline name-hash entries
extern u32 lbl_804D2E18;
extern u32 lbl_804D2E1C[1];
extern u32 lbl_804D4CB8;
extern s8 lbl_804D4CBC;
extern s32 lbl_804D4CC0;
extern s8 lbl_804D4CC4;

typedef s32 (*ZoneNodeFn)(s32, f32, void*, void*);

void SplineBitClearSpan(int, int, int, u32*);
void SplineClearNameHashes(SplineNameEntry*, int);
void ZoneUpdate(void*, IVec3*);
void ZoneWalkNodes(ZoneList*, ZoneNodeFn, int);
void ActorUpdate(Actor*, f32);
s32 FxFind(const char*);
s32 HazardZoneNodeCheck(s32 unused, f32 dist, void* walker, void* owner);
extern "C" s32 SendMsg(s32, s32, void*);
extern "C" void fn_8012C968(s32);
extern "C" void fn_8016EEF4(s32);
extern "C" s32 fn_8016F120(s32, void*);

// SendMsg fills the leading word of a payload; the caller fills the rest.
struct MsgReply {
    u32 size;
    s32 sender;
};
struct MsgEvent {
    u32 size;
    s32 on;
    s32 target;
    u32 event;
};

// Clears the actor's spline span and the shared spline name-hash table.
void HazardSplineReset(Actor* self) {
    SplineBitClearSpan((int)self->zone, (int)lbl_8039EED8, 0x14c, lbl_804D2E1C);
    SplineClearNameHashes(lbl_8039F408, 9);
}

// Per-frame hazard update: zone walk, capture-hum sound, base actor update.
void HazardUpdate(Actor* self, f32 dt) {
    u8* zone = self->zone;

    if (lbl_804D4CBC == 0) {
        lbl_804D4CB8 = 1;
        lbl_804D4CBC = 1;
    }
    if (lbl_804D4CC4 == 0) {
        lbl_804D4CC0 = -2;
        lbl_804D4CC4 = 1;
    }
    if (zone != 0) {
        MsgReply reply;

        self->flags &= 0xddffffff;
        ZoneUpdate(zone + 8, &self->pos);
        ZoneWalkNodes((ZoneList*)(zone + 8), HazardZoneNodeCheck, (int)self);
        SendMsg(self->handle, 0x41a, &reply);
        if ((self->flags & 0x22000000) == 0x22000000) {
            if (reply.sender != -1) {
                MsgEvent ev;

                ev.target = self->handle;
                ev.event = 0xeb9ee198;
                ev.on = 1;
                SendMsg(reply.sender, 0xc14, &ev);
            }
            if (lbl_804D4CB8 != 0) {
                fn_8012C968(self->handle);
                fn_8016EEF4(lbl_804D4CC0);
            }
            lbl_804D4CB8 = 0;
        } else {
            if (reply.sender != -1) {
                MsgEvent ev;

                ev.target = self->handle;
                ev.event = 0xeb9ee198;
                ev.on = 0;
                SendMsg(reply.sender, 0xc14, &ev);
            }
            if (lbl_804D4CB8 == 0) {
                lbl_804D4CB8 = 1;
                lbl_804D2E18 = 0x898;
                lbl_804D4CC0 = fn_8016F120(FxFind("capture_hum"), &self->pos);
            }
        }
        if (lbl_804D4CB8 != 0 && lbl_804D4CC0 == -2) {
            lbl_804D4CC0 = fn_8016F120(FxFind("capture_hum"), &self->pos);
        }
        ActorUpdate(self, dt);
    }
}

// --- hazard actor support (names/types are descriptive guesses) ---

// Payload of the messages this actor receives; word 0 is the owning actor.
struct HazardMsgData {
    Actor* owner;  // 0x00
    s32 hash;      // 0x04
    s32 unk08;     // 0x08
    s32 value;     // 0x0C
    s32 unk10;     // 0x10
    s32 unk14;     // 0x14
    s32 state;     // 0x18
};

// One 0x14C-byte spline slot as handed out by SplineSlotAlloc.
struct HazardSpline {
    s32 color;          // 0x00
    s32 unk04;          // 0x04
    u8 zoneObj[0x3C];   // 0x08
    u8 zoneCtx[0x108];  // 0x44
};

// Engine String/Buffer, used only while registering the hazard type.
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
};
class String : public Buffer {
public:
    String();
    virtual ~String();
};

// Registration message sent once when the first hazard is created.
struct HazardRegMsg {
    u32 size;   // 0x00, filled in by SendMsg
    s32 hash;   // 0x04
    s32 unk08;  // 0x08
    s32 value;  // 0x0C
};

struct ZoneObject;
void ZoneObjectSetSlot18(ZoneObject*, f32);
void ZoneObjectSetSlot14(ZoneObject*, u32);
void ZoneListInit(ZoneList*);
void ZoneInit(void*, Actor*, IVec3*, void*, int, f32);
void* SplineSlotAlloc(void*, int, u32*, int);
u32 AssetFind(u32, u32);
void ModelNodeSetup(s32, s32, s32, s32);
s32 ActorDefaultMsg(Actor*, s32, void*);
s32 ActorInitBase(Actor*);
extern "C" s32 fn_80019F88(void*, Actor*);
extern "C" s32 fn_800FC3B8(s32);
extern "C" float* fn_800FD3FC(s32);
extern "C" void fn_80037EC0(s32, s32, s32, s32, s32, s32, s32);
extern double lbl_804D77F8;
extern double lbl_804D7800;
extern float lbl_804D7808;
extern s32 lbl_804D4CB0;
extern s8 lbl_804D4CB4;

// Gives the hazard's sprite the colour of the given variant (inlined per site).
static inline void HazardSetSprite(Actor* self, HazardSpline* slot, s32 index) {
    s32 ready = fn_800FC3B8(10);

    if (slot == 0) {
        return;
    }
    u32 asset = AssetFind(0xd9044cbb, 0x46266b2);

    if (ready == 0) {
        fn_80037EC0(self->model, (s32)asset, 1, 0x80, 0x80, 0x80, 0xff);
    } else {
        f32* rgba = fn_800FD3FC(index);

        fn_80037EC0(self->model, (s32)asset, 1,
                    (s32)(f32)(lbl_804D77F8 * rgba[0]) & 0xff,
                    (s32)(f32)(lbl_804D77F8 * rgba[1]) & 0xff,
                    (s32)(f32)(lbl_804D77F8 * rgba[2]) & 0xff,
                    (s32)(f32)(lbl_804D77F8 * rgba[3]) & 0xff);
    }
    slot->color = (ready != 0) ? index : -1;
    ZoneListInit((ZoneList*)slot->zoneObj);
}

// Message handler of the hazard actor.
s32 HazardMsgHandler(Actor* self, s32 msg, HazardMsgData* data) {
    Actor* owner = data->owner;

    switch (msg) {
    case 0x400:
        ActorDefaultMsg(self, msg, data);
        data->state = 2;
        return 1;
    case 0x41d:
        if (fn_80019F88(data, owner) != 0) {
            return 1;
        }
        if (data->hash == -0x167d1e20) {
            lbl_804D2E18 = data->value;
            for (s32 i = 0; i < 4; i++) {
                if (lbl_804D2E1C[i >> 5] & (1 << i)) {
                    ZoneObjectSetSlot18((ZoneObject*)(lbl_8039EED8 + i * 0x14c + 8),
                                        (f32)lbl_804D2E18);
                }
            }
            return 1;
        }
        if (data->hash == -0x642b10fa) {
            HazardSetSprite(owner, (HazardSpline*)owner->zone, data->value);
            return 1;
        }
        return 0;
    case 0x41e:
        if (owner->zone == 0) {
            return -1;
        }
        if (data->hash == 0x5c8f66f9) {
            data->value = 0x6ce2b7e8;
            return 1;
        }
        if (data->hash == -0x642b10fa) {
            data->value = *(s32*)owner->zone;
            return 1;
        }
        return 0;
    case 0xc14:
    default:
        return ActorDefaultMsg(self, msg, data);
    }
}

// Creates the hazard's spline slot, models and zone.
s32 HazardActorInit(Actor* self) {
    if (lbl_804D4CB4 == 0) {
        lbl_804D4CB0 = 0;
        lbl_804D4CB4 = 1;
    }
    if (lbl_804D4CB0 == 0) {
        String name;
        HazardRegMsg msg;

        msg.size = 0;
        msg.hash = -0x167d1e20;
        msg.unk08 = 0;
        msg.value = 0x898;
        SendMsg(0x4b000000, 0x41d, &msg);
        lbl_804D4CB0 = 1;
    }

    s32 base = ActorInitBase(self);
    if (base == 0) {
        return 0;
    }

    HazardSpline* slot = (HazardSpline*)SplineSlotAlloc(lbl_8039EED8, 0x14c, lbl_804D2E1C, 4);
    if (slot == 0) {
        return -1;
    }
    slot->unk04 = -1;
    self->zone = (u8*)slot;

    ModelNodeSetup(self->model, (s32)AssetFind(0xd9044cbb, 0xbe327ee8), 0, 1);
    ModelNodeSetup(self->model, (s32)AssetFind(0xd9044cbb, 0x8201dc23), 0, 1);
    ModelNodeSetup(self->model, (s32)AssetFind(0xd9044cbb, 0x08665215), 0, 1);
    ModelNodeSetup(self->model, (s32)AssetFind(0xd9044cbb, 0x22674382), 0, 1);

    switch (*self->typeId) {
    case -0xb89c2f6:
    case 0x366e03db:
        HazardSetSprite(self, slot, 0);
        break;
    case -0x2676e5c0:
    case 0x6c0d7f31:
        HazardSetSprite(self, slot, 1);
        break;
    case -0x40d6135d:
    case -0x43d5000e:
        HazardSetSprite(self, slot, 2);
        break;
    case -0x560217dc:
    case 0x4d1f3eb5:
        HazardSetSprite(self, slot, 3);
        break;
    }

    ZoneInit(slot->zoneObj, self, &self->pos, slot->zoneCtx, 0x10, lbl_804D7808);
    ZoneObjectSetSlot14((ZoneObject*)slot->zoneObj, 0);
    return base;
}

// --- zone-node walk callback ---

extern "C" s32 fn_800F7A90();
extern "C" s32 fn_800F81EC(s32);
extern "C" s32 fn_800F8130(s32, s32);
extern "C" double sqrt(double);
u32 Fn800F8A98(u32);

// An actor visited by the zone walk; 0x30 holds its query callback.
struct HazardWalkTarget {
    u8 pad00[0x30];
    s32 (*query)(s32, void*);  // 0x30
    u32 kind;                  // 0x38
    u8 pad3C[0x5F0];
    s32 handle;                // 0x62C
};

// The hazard owner the walk callback was registered for.
struct HazardWalkOwner {
    u8 pad00[0x38];
    s32 handle;   // 0x38
    u8 pad3C[0x68];
    u8* zone;     // 0xA4
    u8 padA8[0x4];
    u32 flags;    // 0xAC
};

// Zone-node walk callback: refreshes the hazard radius from the walker's
// distance and checks whether the walker carries the hazard's type hash.
s32 HazardZoneNodeCheck(s32 unused, f32 dist, void* walkerPtr, void* ownerPtr) {
    HazardWalkTarget* walker = (HazardWalkTarget*)walkerPtr;
    HazardWalkOwner* owner = (HazardWalkOwner*)ownerPtr;
    s32 query[2];
    s32 hash;

    if (owner == 0) {
        return -1;
    }
    if (dist > (f32)(lbl_804D2E18 * lbl_804D2E18)) {
        if ((walker->kind >> 24) == 2) {
            if (walker->handle != owner->handle) {
                return 1;
            }
            s32 v = (s32)dist;
            if (v < 1) {
                lbl_804D2E18 = 0;
            } else {
                lbl_804D2E18 = (s32)(f32)sqrt((double)v);
            }
        }
    }
    query[0] = (s32)walker;
    if (walker->query(0x6705, query) != 0) {
        HazardSpline* slot = (HazardSpline*)owner->zone;

        if (slot == 0) {
            hash = -1;
        } else if (fn_800FC3B8(10) == 0) {
            Fn800F8A98(0x6b4b4a10);
            hash = fn_800F7A90();
        } else {
            switch ((u32)slot->color) {
            case 0:
                hash = Fn800F8A98(0x114f1979);
                break;
            case 2:
                hash = Fn800F8A98(0x21425b82);
                break;
            case 3:
                hash = Fn800F8A98(0xfad5ac93);
                break;
            case 4:
                hash = Fn800F8A98(0x6ba1bc35);
                break;
            case 5:
                hash = Fn800F8A98(0xdbec014d);
                break;
            case 6:
                hash = Fn800F8A98(0xe54b6bdb);
                break;
            case 7:
                hash = Fn800F8A98(0x154f7c95);
                break;
            default:
                hash = Fn800F8A98(0x6b4b4a10);
                break;
            }
        }
        s32 count = fn_800F81EC(query[1]);
        if (fn_800FC3B8(10) == 0) {
            for (s32 i = 0; i < count; i++) {
                fn_800F8130(query[1], i);
                if (hash == fn_800F7A90()) {
                    owner->flags |= 0x22000000;
                    return 0;
                }
            }
        } else {
            for (s32 i = 0; i < count; i++) {
                if (hash == fn_800F8130(query[1], i)) {
                    owner->flags |= 0x22000000;
                    return 0;
                }
            }
        }
    }
    owner->flags &= 0xdfffffff;
    return 1;
}

// --- hazard spline-zone creation ---

void* VAlloc(s32 size);
extern "C" void** fn_8028B3BC(void* list, s32 offset, s32 size);
struct HazardSplineActor;
struct HazardSplineZone;
s32 HazardSplineZoneCheck(f32, HazardSplineActor*, HazardSplineZone*);
void ZoneSetCallback(void*, s32, s32 (*)(s32, Actor*, Actor*), s32, Actor*);

// Base of the hazard spline-instance list: releases the engine-side resources.
extern "C" void fn_8028B960(void* self);
class HazardZoneBase : public Buffer {
public:
    HazardZoneBase() {}
    virtual void Clear(s32 flag);
    virtual ~HazardZoneBase() { fn_8028B960(this); }
};

// Dynamic list of hazard spline instances; the third virtual clears the list.
class HazardZoneBuffer : public HazardZoneBase {
public:
    HazardZoneBuffer() {}
    virtual ~HazardZoneBuffer();
    virtual u32 GetSize();         // bytes the list needs with the current contents
    u32 unk04Data;                 // 0x04
    u32 len;                       // 0x08
    s32 used;                      // 0x0C
    u32 capacity;                  // 0x10
};
extern HazardZoneBuffer* lbl_804D4CC8;
extern s32 lbl_804D4CCC;
extern float lbl_804D7810;
extern float lbl_804D7814;
extern float lbl_804D7818;

// The 0x4CC-byte hazard spline instance created for one zone.
struct HazardSplineZone {
    s32 id;          // 0x000
    IVec3 pos;       // 0x004
    s32 unk10;       // 0x010
    f32 unk14;       // 0x014
    f32 unk18;       // 0x018
    f32 unk1C;       // 0x01C
    f32 unk20;       // 0x020
    f32 unk24;       // 0x024
    f32 unk28;       // 0x028
    u8 zone[0x3C];   // 0x02C
    s32 unk68;       // 0x068
    s32 unk6C;       // 0x06C
    f32 unk70;       // 0x070
    s32 unk74;       // 0x074
    s32 unk78;       // 0x078
    s32 unk7C;       // 0x07C
    s32 unk80;       // 0x080
    u8 buf84[0x40];  // 0x084
    u8 padC4[0x3C0]; // 0x0C4
    f32 unk484;      // 0x484
    f32 unk488;      // 0x488
    f32 unk48C;      // 0x48C
    u8 pad490[0x3C]; // 0x490
};

// Allocates and initialises one hazard spline zone; returns its serial number.
s32 HazardSplineZoneCreate(f32 p1, f32 p2, f32 p3, f32 p4, Actor* owner, s32 flag6, f32* vec,
                           s32 flag8) {
    HazardSplineZone* obj = (HazardSplineZone*)VAlloc(0x4cc);

    if (obj == 0) {
        return -1;
    }
    HazardSplineZone** slot = (HazardSplineZone**)fn_8028B3BC(
        lbl_804D4CC8, lbl_804D4CC8->used + lbl_804D4CC8->len, 4);
    *slot = obj;

    s32 id = ++lbl_804D4CCC;
    obj->id = id;
    obj->unk10 = 1;
    obj->unk14 = p3;
    obj->unk18 = p4;
    obj->unk28 = lbl_804D7810;
    obj->unk68 = flag8;
    obj->unk24 = lbl_804D7814;
    obj->unk6C = flag6;
    obj->unk484 = p1;
    obj->unk488 = p2;
    obj->unk48C = lbl_804D7818;
    obj->unk78 = 1;
    obj->unk7C = 0;
    obj->unk80 = 0;
    obj->pos.x = vec[0];
    obj->pos.y = vec[1];
    obj->pos.z = vec[2];

    s32 iv[3];
    iv[0] = (s32)vec[0];
    iv[1] = (s32)vec[1];
    iv[2] = (s32)vec[2];

    ZoneInit(obj->zone, owner, (IVec3*)iv, obj->buf84, 0x40, p2);
    ZoneSetCallback(obj->zone, (s32)HazardSplineZoneCheck,
                    (s32(*)(s32, Actor*, Actor*))HazardSplineZoneCheck, 0, (Actor*)obj);
    ZoneObjectSetSlot14((ZoneObject*)obj->zone, 0);
    return lbl_804D4CCC;
}

// Creates one hazard spline zone from an integer position vector and seven
// spline parameters; returns its serial number.
s32 HazardCreateSpline(Actor* owner, s32 typeA, Vec3* vec, s32 typeB, f32 p1, f32 p2, f32 p3,
                       f32 p4, f32 p5, f32 p6, f32 p7) {
    HazardSplineZone* obj = (HazardSplineZone*)VAlloc(0x4cc);

    if (obj == 0) {
        return -1;
    }
    HazardSplineZone** slot = (HazardSplineZone**)fn_8028B3BC(
        lbl_804D4CC8, lbl_804D4CC8->used + lbl_804D4CC8->len, 4);
    *slot = obj;

    s32 id = ++lbl_804D4CCC;
    obj->id = id;
    obj->unk10 = 0;
    obj->unk14 = p3;
    obj->unk18 = p4;
    obj->unk28 = lbl_804D7810;
    obj->unk68 = typeB;
    obj->unk24 = p7;
    obj->unk6C = typeA;
    obj->unk484 = p1;
    obj->unk488 = p2;
    obj->unk48C = p2 - p1;
    obj->unk78 = 1;
    obj->unk7C = 0;
    obj->unk80 = 0;
    obj->unk70 = lbl_804D7810;
    obj->unk74 = -1;
    obj->unk1C = p5;
    obj->unk20 = p6;
    obj->pos.x = vec->x;
    obj->pos.y = vec->y;
    obj->pos.z = vec->z;

    s32 iv[3];
    iv[0] = (s32)vec->x;
    iv[1] = (s32)vec->y;
    iv[2] = (s32)vec->z;

    ZoneInit(obj->zone, owner, (IVec3*)iv, obj->buf84, 0x40, p1);
    ZoneSetCallback(obj->zone, (s32)HazardSplineZoneCheck, 0, 0, (Actor*)obj);
    ZoneObjectSetSlot14((ZoneObject*)obj->zone, 0);
    return lbl_804D4CCC;
}

// Creates a hazard spline zone whose spline values come from the shared
// tables; returns its serial number.
s32 HazardCreate(Actor* owner, s32 typeA, Vec3* vec, s32 typeB, s32 typeC, f32 p1, f32 p2,
                 f32 p3, f32 p4) {
    HazardSplineZone* obj = (HazardSplineZone*)VAlloc(0x4cc);

    if (obj == 0) {
        return -1;
    }
    HazardSplineZone** slot = (HazardSplineZone**)fn_8028B3BC(
        lbl_804D4CC8, lbl_804D4CC8->used + lbl_804D4CC8->len, 4);
    *slot = obj;

    s32 id = ++lbl_804D4CCC;
    obj->id = id;
    obj->unk10 = 1;
    obj->unk14 = p3;
    obj->unk18 = p4;
    obj->unk28 = lbl_804D7810;
    obj->unk68 = typeB;
    obj->unk24 = lbl_804D7814;
    obj->unk6C = typeA;
    obj->unk484 = p1;
    obj->unk488 = p2;
    obj->unk48C = lbl_804D7818;
    obj->unk78 = 1;
    obj->unk7C = typeC;
    obj->unk80 = 0;
    obj->pos.x = vec->x;
    obj->pos.y = vec->y;
    obj->pos.z = vec->z;

    s32 iv[3];
    iv[0] = (s32)vec->x;
    iv[1] = (s32)vec->y;
    iv[2] = (s32)vec->z;

    ZoneInit(obj->zone, owner, (IVec3*)iv, obj->buf84, 0x40, p2);
    ZoneSetCallback(obj->zone, (s32)HazardSplineZoneCheck, (s32(*)(s32, Actor*, Actor*))HazardSplineZoneCheck, 0,
                    (Actor*)obj);
    ZoneObjectSetSlot14((ZoneObject*)obj->zone, 0);
    return lbl_804D4CCC;
}

// Creates a hazard spline zone from an integer position vector and seven
// spline parameters, selecting the sprite set from typeC; returns its serial
// number. The symbol stays the placeholder fn_80122108: other units
// (u_80285ACC, u_80247EB0) call it through extern "C" declarations of that name.
s32 fn_80122108(Actor* owner, s32 typeA, Vec3* vec, s32 typeB, s32 typeC, f32 p1, f32 p2,
                        f32 p3, f32 p4, f32 p5, f32 p6, f32 p7) {
    HazardSplineZone* obj = (HazardSplineZone*)VAlloc(0x4cc);

    if (obj == 0) {
        return -1;
    }
    HazardSplineZone** slot = (HazardSplineZone**)fn_8028B3BC(
        lbl_804D4CC8, lbl_804D4CC8->used + lbl_804D4CC8->len, 4);
    *slot = obj;

    s32 id = ++lbl_804D4CCC;
    obj->id = id;
    obj->unk10 = 0;
    obj->unk14 = p3;
    obj->unk18 = p4;
    obj->unk28 = lbl_804D7810;
    obj->unk68 = typeB;
    obj->unk24 = p7;
    obj->unk6C = typeA;
    obj->unk484 = p1;
    obj->unk488 = p2;
    obj->unk48C = p2 - p1;
    obj->unk78 = 1;
    obj->unk7C = typeC;
    obj->unk80 = 0;
    obj->unk70 = lbl_804D7810;
    obj->unk74 = -1;
    obj->unk1C = p5;
    obj->unk20 = p6;
    obj->pos.x = vec->x;
    obj->pos.y = vec->y;
    obj->pos.z = vec->z;

    s32 iv[3];
    iv[0] = (s32)vec->x;
    iv[1] = (s32)vec->y;
    iv[2] = (s32)vec->z;

    ZoneInit(obj->zone, owner, (IVec3*)iv, obj->buf84, 0x40, p1);
    ZoneSetCallback(obj->zone, (s32)HazardSplineZoneCheck, 0, 0, (Actor*)obj);
    ZoneObjectSetSlot14((ZoneObject*)obj->zone, 0);
    return lbl_804D4CCC;
}

void ZoneFlagSet(ZoneList*, s32, s32);

// Sets the given flags on the hazard spline zone with the given serial number.
void HazardSetFlag(s32 id, s32 flagA, s32 flagB) {
    HazardSplineZone* found = 0;
    HazardSplineZone** p = (HazardSplineZone**)lbl_804D4CC8->used;
    HazardSplineZone** end = (HazardSplineZone**)(lbl_804D4CC8->used + lbl_804D4CC8->len);

    for (; p != end; p++) {
        if ((*p)->id == id) {
            found = *p;
            break;
        }
    }
    if (found != 0) {
        ZoneFlagSet((ZoneList*)found->zone, flagA, flagB);
    }
}

extern "C" void fn_8028B35C(void* list, void* at, s32 size);
void VFree(void*);

// Frees the hazard spline with serial number id and removes it from the list.
void HazardDestroy(s32 id) {
    HazardSplineZone** p = (HazardSplineZone**)lbl_804D4CC8->used;
    HazardSplineZone** end = (HazardSplineZone**)(lbl_804D4CC8->used + lbl_804D4CC8->len);

    while (p != end) {
        HazardSplineZone* obj = *p;
        if (id == obj->id) {
            VFree(obj);
            fn_8028B35C(lbl_804D4CC8, p, 4);
            return;
        }
        p++;
    }
}

// Moves the hazard spline with serial number id to the given position.
void HazardMove(s32 id, Vec3* pos) {
    HazardSplineZone** p = (HazardSplineZone**)lbl_804D4CC8->used;
    HazardSplineZone** end = (HazardSplineZone**)(lbl_804D4CC8->used + lbl_804D4CC8->len);
    HazardSplineZone* found = 0;

    for (; p != end; p++) {
        if ((*p)->id == id) {
            found = *p;
            break;
        }
    }
    if (found != 0) {
        found->pos.x = pos->x;
        found->pos.y = pos->y;
        found->pos.z = pos->z;
    }
}

// Enables or disables the hazard spline with serial number id.
void HazardSetActive(s32 id, s32 active) {
    HazardSplineZone* found = 0;
    HazardSplineZone** p = (HazardSplineZone**)lbl_804D4CC8->used;
    HazardSplineZone** end = (HazardSplineZone**)(lbl_804D4CC8->used + lbl_804D4CC8->len);

    for (; p != end; p++) {
        if ((*p)->id == id) {
            found = *p;
            break;
        }
    }
    if (found == 0) {
        return;
    }
    if (active == 0) {
        found->unk78 = 0;
        return;
    }
    found->unk78 = 1;
}

// Frees every hazard spline instance and clears the list.
void HazardSplineClearAll() {
    while (lbl_804D4CC8->len >> 2 != 0) {
        VFree(*(void**)lbl_804D4CC8->used);
        fn_8028B35C(lbl_804D4CC8, (void*)lbl_804D4CC8->used, 4);
    }
    if (lbl_804D4CC8 != 0) {
        lbl_804D4CC8->Clear(1);
    }
}

// Destructor of the hazard spline-instance list (the deleting-destructor slot
// of its vtable): tears down the engine base object and releases the storage.
extern "C" void fn_8028B960(void* self);

HazardZoneBuffer::~HazardZoneBuffer() {
}

// Draws a debug wire sphere for every hazard spline instance in the list; the
// first flag gates the whole debug view.
extern s32 lbl_804D4CD0;
void DebugDrawSphereWire(int, int, int, f32, f32, f32, f32);

// The part of a hazard spline instance the debug view reads.
struct HazardDrawEntry {
    u8 pad00[4];
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    u8 pad10[0x34];
    f32 radius;     // 0x44
    u8 pad48[0x30];
    s32 active;     // 0x78
};

void HazardDebugDrawSpheres() {
    if (lbl_804D4CD0 == 0) {
        return;
    }

    HazardDrawEntry** p = (HazardDrawEntry**)lbl_804D4CC8->used;
    HazardDrawEntry** end = (HazardDrawEntry**)(lbl_804D4CC8->used + lbl_804D4CC8->len);

    while (p != end) {
        HazardDrawEntry* obj = *p;

        if (obj->active == 1) {
            DebugDrawSphereWire(0xff, 0xa0, 0, obj->x, obj->y, obj->z, obj->radius);
        } else {
            DebugDrawSphereWire(0xa0, 0xff, 0, obj->x, obj->y, obj->z, obj->radius);
        }
        p++;
    }
}

// --- per-frame update of every live hazard spline instance ---

extern float lbl_804D2E28;   // global time scale / last delta
extern float lbl_804D781C;   // reset value for a spline's expiry timer
extern Vec3 lbl_80345980;    // shared constant vector
extern "C" float fn_802A2080();
extern "C" void fn_800D1798(f32, u8*);
extern "C" s32 fn_8001A30C(f32, s32, u8*, void*, u32, void*, void*);
extern "C" void fn_8009DA3C();
struct HazardSplineQuery;
s32 HazardSplineProximityFx(HazardSplineQuery*, HazardSplineZone*);

// Advances every live hazard spline instance by dt: grows its lifetime timer,
// follows the zone it sits in, triggers the expiry message and finally drops
// the splines whose lifetime ran out. Returns 1.
s32 HazardSplineUpdateAll(f32 dt) {
    u32 offset = 0;

    lbl_804D2E28 = dt;
    for (u32 i = 0; i < (u32)lbl_804D4CC8->len >> 2; i++, offset += 4) {
        HazardSplineZone* obj = *(HazardSplineZone**)(lbl_804D4CC8->used + offset);
        s32 iv[3];

        if (obj->unk78 == 0) {
            continue;
        }
        if (obj->unk80 == 0) {
            obj->unk28 += dt;
        } else {
            obj->unk28 = dt * fn_802A2080() + obj->unk28;
        }
        if (obj->unk10 == 1) {
            iv[0] = (s32)obj->pos.x;
            iv[1] = (s32)obj->pos.y;
            iv[2] = (s32)obj->pos.z;
            ZoneUpdate(obj->zone, (IVec3*)iv);
        } else if (obj->unk10 == 2) {
            iv[0] = (s32)obj->pos.x;
            iv[1] = (s32)obj->pos.y;
            iv[2] = (s32)obj->pos.z;
            ZoneUpdate(obj->zone, (IVec3*)iv);
        } else {
            Vec3 zero = lbl_80345980;
            f32 value = obj->unk48C * (obj->unk28 / obj->unk24) + obj->unk484;

            iv[0] = (s32)obj->pos.x;
            iv[1] = (s32)obj->pos.y;
            iv[2] = (s32)obj->pos.z;
            fn_800D1798(value, obj->zone);
            ZoneUpdate(obj->zone, (IVec3*)iv);
            if (obj->unk74 != -1) {
                if (lbl_804D7810 < obj->unk70) {
                    obj->unk70 -= dt;
                } else {
                    fn_8001A30C(value, 0, (u8*)&obj->pos.x, &zero, 0x82000000,
                                (void*)HazardSplineProximityFx, obj);
                    obj->unk70 = lbl_804D781C;
                }
            }
        }
    }

    HazardSplineZone** p = (HazardSplineZone**)lbl_804D4CC8->used;
    HazardSplineZone** end = (HazardSplineZone**)(lbl_804D4CC8->used + lbl_804D4CC8->len);

    while (p != end) {
        f32 life = (*p)->unk24;

        if (lbl_804D7814 == life || (*p)->unk28 < life) {
            p++;
        } else {
            fn_8009DA3C();
            fn_8028B35C(lbl_804D4CC8, p, 4);
        }
    }
    return 1;
}

// No-op hook kept for the spline code path that used to do work here.
void HazardSplineNoop() {}

// Initialises the global list of hazard spline instances: allocates the list
// object, sizes it and clears its used length.
extern "C" void fn_8028B5FC(void* list, s32 offset, s32 size);

s32 HazardZoneBufferInit() {
    lbl_804D4CCC = 0;

    HazardZoneBuffer* list = (HazardZoneBuffer*)VAlloc(0x14);

    if (list != 0) {
        list->HazardZoneBuffer();
    }
    lbl_804D4CC8 = list;

    u32 size = list->GetSize();

    if (list->capacity < size) {
        fn_8028B5FC(list, 0, 1);
    }
    list->len = 0;
    return 1;
}

// Base class of the hazard spline-instance list: its destructor tears down the
// engine base object and, when the delete flag is set, releases the storage.
class HazardZoneList : public Buffer {
public:
    virtual ~HazardZoneList();
    static void operator delete(void* p) { VFree(p); }
};

HazardZoneList::~HazardZoneList() {
    fn_8028B960(this);
}

// --- spline zone proximity-effect callback ---

// Positions the spline zone probes against; the zone centre is compared with
// near, while from/to become the two endpoints of the spawned effect.
struct HazardSplineQuery {
    u8 pad00[0x10];
    IVec3 near;  // 0x10
    IVec3 from;  // 0x1C
    IVec3 to;    // 0x28
};

extern "C" int rand(void);
extern "C" void fn_80227DD8(IVec3*, IVec3*, f32, f32, f32, f32, s32, s32, s32, s32);
extern float lbl_804D7820;
extern float lbl_804D7824;
extern float lbl_804D7828;
extern float lbl_804D782C;
extern float lbl_804D2E20;
extern float lbl_804D2E24;

// Spawns the spline's proximity effect when the queried point lies inside the
// spline's current radius. Returns 1.
s32 HazardSplineProximityFx(HazardSplineQuery* query, HazardSplineZone* zone) {
    IVec3 from = query->from;
    IVec3 to = query->to;
    f32 dx = zone->pos.x - query->near.x;
    f32 dy = zone->pos.y - query->near.y;
    f32 dz = zone->pos.z - query->near.z;
    f32 reach = zone->unk48C * (zone->unk28 / zone->unk24) + zone->unk484;
    f32 spread = reach * reach - (dz * dz + dy * dy + dx * dx);
    f32 scale = lbl_804D7810;

    if (lbl_804D7810 < spread) {
        scale = (f32)(1.0 / (1.0 / sqrt((double)spread)));
    }
    u8 color = rand() % 16 + 16;

    fn_80227DD8(&from, &to, scale, lbl_804D7820, lbl_804D7824, lbl_804D7828, color, color, color,
                zone->unk74);
    return 1;
}

// --- spline zone hit callback ---

// The actor a spline zone tests against; 0x30 holds its message callback.
struct HazardSplineActor {
    u8 pad00[0x24];
    IVec3 pos;                     // 0x24
    void (*callback)(s32, void*);  // 0x30
    u8 pad34[0x4];
    u32 kind;                      // 0x38
    s32 unk3C;                     // 0x3C
    u8 pad40[0x44];
    s32* typeId;                   // 0x84
};

// Probe positions handed to the collision test.
struct SplineProbe {
    s32 zoneX, zoneY, zoneZ;     // 0x00
    s32 actorX, actorY, actorZ;  // 0x0C
};

// Result the collision test writes back into the probe.
struct SplineProbeResult {
    s32 unk00;  // 0x00
    s32 unk04;  // 0x04
    s32 unk08;  // 0x08
    s32 unk0C;  // 0x0C
    s32 unk10;  // 0x10
    s32 unk14;  // 0x14
};

// Context handed to the collision test callback.
struct SplineProbeCtx {
    HazardSplineZone* zone;     // 0x00
    HazardSplineActor* actor;   // 0x04
    SplineProbeResult* result;  // 0x08
};

// Payload of message 0x408 (collision query).
struct SplineQueryMsg {
    HazardSplineActor* actor;  // 0x00
    s32 unk04;                 // 0x04
    s32 unk08;                 // 0x08
    u8 pad0C[4];               // 0x0C
    s32 x, y, z;               // 0x10
};

// Payload of message 0x407 (hit event).
struct SplineHitEvent {
    HazardSplineActor* actor;    // 0x00
    s32 reply;                   // 0x04
    s32 id;                      // 0x08
    f32 v1;                      // 0x0C
    f32 v2;                      // 0x10
    s32 type;                    // 0x14
    s32 zoneX, zoneY, zoneZ;     // 0x18
    u8 pad24[0xC];               // 0x24
    s32 deltaX, deltaY, deltaZ;  // 0x30
    u8 pad3C[0x8];               // 0x3C
    s32 unk44;                   // 0x44
    s32 unk48;                   // 0x48
    f32 unk4C;                   // 0x4C
    f32 unk50;                   // 0x50
};

extern "C" f32 fn_800AA230(HazardSplineActor*);
extern "C" s32 fn_8001F928(s32, void*, u32, void*, void*);

// Hit record handed to the collision-test callback (0x48 bytes); the callback
// copies it verbatim into the caller's result buffer.
struct SplineHitData {
    Actor* actor;  // 0x00
    s32 unk04;     // 0x04
    s32 unk08;     // 0x08
    u32 flags;     // 0x0C
    s32 unk10;     // 0x10
    s32 unk14;     // 0x14
    s32 unk18;     // 0x18
    s32 unk1C;     // 0x1C
    s32 unk20;     // 0x20
    s32 unk24;     // 0x24
    s32 unk28;     // 0x28
    s32 unk2C;     // 0x2C
    s32 unk30;     // 0x30
    s32 unk34;     // 0x34
    s32 unk38;     // 0x38
    s32 unk3C;     // 0x3C
    s32 unk40;     // 0x40
    s32 unk44;     // 0x44
};

// Context registered with the collision test: the spline zone, the actor that
// was tested and where the resulting hit record has to be written.
struct SplineHitCtx {
    HazardSplineZone* zone;    // 0x00
    HazardSplineActor* actor;  // 0x04
    SplineHitData* result;     // 0x08
};

// Collision-test callback: rejects the hit when the zone does not belong to the
// tested actor or when the actor's bone capsule misses the spline, otherwise
// copies the hit record out. Returns 1 when the hit has to be discarded.
s32 HazardSplineProbeFilter(SplineHitData* hit, SplineHitCtx* ctx);

// Zone hit callback: refreshes the spline radius from the probe distance,
// tests the colliding actor and sends it the resulting hit message. Returns 1.
s32 HazardSplineZoneCheck(f32 dist, HazardSplineActor* actor, HazardSplineZone* zone) {
    if (zone == 0) {
        return 1;
    }
    if (actor == 0) {
        return 1;
    }
    u32 kind = actor->kind >> 24;

    if (kind != 2 && kind != 0x13 && kind != 1) {
        return 1;
    }
    if (kind == 1 && actor->typeId != 0 && actor->typeId[1] == 0x7e9acc12) {
        return 1;
    }

    f32 inner = zone->unk484;
    f32 outer = zone->unk488;
    f32 reach;

    if (dist > lbl_804D7810) {
        reach = (f32)(1.0 / (1.0 / sqrt((double)dist)));
    } else {
        reach = lbl_804D7810;
    }
    reach -= fn_800AA230(actor);
    if (reach < lbl_804D7810) {
        reach = lbl_804D7810;
    }
    if (reach > outer) {
        return 1;
    }

    f32 v1;
    f32 v2;

    if (reach > inner) {
        f32 t = (reach - inner) / (outer - inner);

        if (t < lbl_804D7810) {
            t = lbl_804D7810;
        } else if (t > lbl_804D7818) {
            t = lbl_804D7818;
        }
        v1 = zone->unk14 * (lbl_804D7818 - t) + zone->unk18 * t;
        v2 = zone->unk1C * (lbl_804D7818 - t) + zone->unk20 * t;
    } else {
        v1 = zone->unk14;
        v2 = zone->unk1C;
    }
    if (zone->unk10 == 1) {
        v1 *= lbl_804D782C * lbl_804D2E28;
    }

    s32 reply = 0;

    SendMsg(zone->unk6C, 0x416, &reply);

    SplineProbe probe;
    probe.zoneX = (s32)zone->pos.x;
    probe.zoneY = (s32)zone->pos.y;
    probe.zoneZ = (s32)zone->pos.z;
    probe.actorX = (s32)actor->pos.x;
    probe.actorY = (s32)actor->pos.y;
    probe.actorZ = (s32)actor->pos.z;

    SplineProbeResult result;
    SplineProbeCtx ctx;

    ctx.zone = zone;
    ctx.actor = actor;
    ctx.result = &result;

    if (zone->unk68 == 0x2efc66ca && (actor->kind >> 24) != 2) {
        v1 = lbl_804D7810;
    }

    SplineQueryMsg query;

    query.actor = actor;
    query.unk04 = 0;
    query.unk08 = -1;
    query.x = (s32)zone->pos.x;
    query.y = (s32)zone->pos.y;
    query.z = (s32)zone->pos.z;

    s32 hit = SendMsg(actor->kind, 0x408, &query);

    if (hit != 0 && query.unk04 != 0) {
        SplineHitEvent ev;

        ev.actor = actor;
        ev.reply = reply;
        ev.id = zone->unk6C;
        ev.v1 = v1;
        ev.v2 = v2;
        ev.type = zone->unk68;
        ev.zoneX = 0;
        ev.zoneY = 0;
        ev.zoneZ = 0;
        ev.deltaX = 0;
        ev.deltaY = 0;
        ev.deltaZ = 0;
        ev.unk44 = -1;
        ev.unk48 = 0;
        ev.unk4C = lbl_804D2E20;
        ev.unk50 = lbl_804D2E24;
        actor->callback(0x407, &ev);
    } else if (fn_8001F928(actor->unk3C, &probe, 0x12002000, (void*)HazardSplineProbeFilter, &ctx) == 0) {
        SplineHitEvent ev;

        ev.actor = actor;
        ev.reply = reply;
        ev.id = zone->unk6C;
        ev.v1 = v1;
        ev.v2 = v2;
        ev.type = zone->unk68;
        ev.zoneX = (s32)zone->pos.x;
        ev.zoneY = (s32)zone->pos.y;
        ev.zoneZ = (s32)zone->pos.z;
        ev.deltaX = (s32)actor->pos.x - ev.zoneX;
        ev.deltaY = (s32)actor->pos.y - ev.zoneY;
        ev.deltaZ = (s32)actor->pos.z - ev.zoneZ;
        ev.unk44 = result.unk0C;
        ev.unk48 = result.unk14;
        ev.unk4C = lbl_804D2E20;
        ev.unk50 = lbl_804D2E24;
        actor->callback(0x407, &ev);
    }
    return 1;
}

// Row-major 3x4 transform, as produced by the actor matrix helpers.
struct Mtx34 {
    f32 m[3][4];
};

// Bone-local probe point filled in by the collision test (0x18 bytes).
struct HazardProbePoint {
    f32 pos[3];     // 0x00
    f32 unk0C[3];   // 0x0C
};

void ActorGetMatrix(Mtx34*, Actor*);
s32 ModelBoneHitCapsule(s32, s32, f32, s32);
extern "C" void fn_8016D864(HazardProbePoint*, s32);

// Spline collision-test callback (the probe filter registered by
// HazardSplineZoneCheck): drops hits that do not belong to the tested zone or
// that miss the spline's bone capsule, otherwise copies the hit record into the
// caller's result buffer. Returns 1 when the hit has to be discarded.
s32 HazardSplineProbeFilter(SplineHitData* hit, SplineHitCtx* ctx) {
    if (ctx == 0) {
        return 1;
    }
    if (hit == 0) {
        return 1;
    }
    if ((hit->flags & 0x200) != 0) {
        return 0;
    }

    HazardSplineActor* actor = (HazardSplineActor*)hit->actor;

    if (actor != 0) {
        Mtx34 matrix;
        HazardProbePoint point;

        ActorGetMatrix(&matrix, (Actor*)actor);
        fn_8016D864(&point, *(s32*)((u8*)actor + 0x40));
        if (actor != ctx->actor && (s32)actor->kind != ctx->zone->unk7C &&
            ModelBoneHitCapsule((s32)&point, (s32)&matrix, lbl_804D7810,
                                (s32)&ctx->zone->pos) == 0) {
            return 1;
        }
    }
    *ctx->result = *hit;
    return 0;
}

// Quiesces the hazard spline storage pool, then releases the 4-byte-aligned
// range [start, start + size).
extern "C" void fn_8028BC2C();
void HazardSplineRangeReset(u8* start, u8* end);

void HazardSplineBufferClear(s32, u8* start, u32 size) {
    fn_8028BC2C();
    HazardSplineRangeReset(start, start + (size & 0xfffffffc));
}

// Quiesces the 4-byte-aligned prefix of the source range, then copies `size`
// bytes from src to dst.
extern "C" void fn_8028BC28(void* dst, void* src, u32 size);
void HazardBlockRangeReset(u8* start, u8* end);

void HazardBlockCopy(void* dst, u8* src, u32 size) {
    u32 n = size & 0xfffffffc;
    HazardBlockRangeReset(src, src + n);
    fn_8028BC28(dst, src, size);
}

// Granularity, in bytes, of the block copy helpers above.
s32 HazardBlockAlign(void) { return 4; }

// Walks the 4-byte-aligned spline pool range [start, end) one word at a time.
// The per-word step does nothing for this pool, so only the counter is left.
void HazardSplineRangeReset(u8* start, u8* end) {
    u32 count = ((u32)end + 3 - (u32)start) >> 2;
    if ((u32)end <= (u32)start) {
        return;
    }
    do {
        count--;
    } while (count != 0);
}

// Same walk as HazardSplineRangeReset, for the block-copy source range.
void HazardBlockRangeReset(u8* start, u8* end) {
    u32 count = ((u32)end + 3 - (u32)start) >> 2;
    if ((u32)end <= (u32)start) {
        return;
    }
    do {
        count--;
    } while (count != 0);
}

// --- hazard target selection (descriptive names; the retail names are unknown) ---

// Reply of the 0x400 "describe target" message.
struct HazardTargetInfo {
    u32 size;   // 0x00, filled in by SendMsg
    s32 unk04;  // 0x04
    s32 unk08;  // 0x08
    s32 kind;   // 0x0C
    s32 unk10;  // 0x10
    s32 unk14;  // 0x14
    u32 flags;  // 0x18
};

// Payload of the 0x41e "look an object up by type name" message.
struct HazardTypeQuery {
    u32 size;   // 0x00, filled in by SendMsg
    s32 hash;   // 0x04
    s32 unk08;  // 0x08
    s32 value;  // 0x0C, reply
};

struct Owner {
    u8 pad00[0xC4];
    u8* type;  // 0xC4
};

s32 OwnerId(Owner*);
s32 HandleToPlayer(s32);
s32 PlayerTeam(s32);
extern "C" s32 fn_800A87A0(void*, void*);

// Returns 1 when `handle` is a live, hostile target of `self`.
s32 HazardCheckTarget(Owner* self, s32 handle) {
    if (handle == OwnerId(self)) {
        return 0;
    }

    HazardTargetInfo info;
    if (SendMsg(handle, 0x400, &info) == 0) {
        return 0;
    }
    if (info.size == 0) {
        return 0;
    }
    if (info.kind != 1) {
        return 0;
    }

    s32 team = PlayerTeam(HandleToPlayer(handle));
    if (PlayerTeam(HandleToPlayer(OwnerId(self))) == team) {
        return 0;
    }

    if (PlayerTeam(HandleToPlayer(handle)) == -1) {
        String name;
        HazardTypeQuery q;
        q.size = 0;
        q.hash = -0x23ec0755;
        q.unk08 = 0;
        q.value = 0;
        SendMsg(handle, 0x41e, &q);
        s32 found = q.value;
        if (found != 0) {
            return 0;
        }

        String name2;
        HazardTypeQuery q2;
        q2.size = 0;
        q2.hash = -0x93b82d6d;
        q2.unk08 = 0;
        q2.value = 0;
        SendMsg(handle, 0x41e, &q2);
        found = q2.value;

        if (found == -1) {
            String name3;
            HazardTypeQuery q3;
            q3.size = 0;
            q3.hash = 0x01e4326a;
            q3.unk08 = 0;
            q3.value = 0;
            SendMsg(handle, 0x41e, &q3);
            found = q3.value;
        }
        if (found == -1) {
            return 0;
        }

        s32 otherTeam = PlayerTeam(HandleToPlayer(found));
        if (PlayerTeam(HandleToPlayer(OwnerId(self))) == otherTeam) {
            return 0;
        }
    }

    if (*(s32*)(self->type + 0x174) == -0x5a54f8b8) {
        return 1;
    }
    if (info.flags & 2) {
        return 1;
    }
    return 0;
}

// Message port of the tracked object: a callback at 0x30 takes the payload.
struct HazardMsgPort {
    u8 pad00[0x30];
    s32 (*send)(s32, void*);  // 0x30
};

struct HazardTargetMsg {
    void* port;  // 0x00
    s32 handle;  // 0x04
};

struct HazardTargetList {
    s32 unk00;     // 0x00
    s32 active;    // 0x04, 0 = idle, 1/2 = tracking mode
    f32 limit;     // 0x08, distance at which the target is dropped
    union {
        Vec3 pos;   // 0x0C, target position while active == 1
        s32 target; // 0x0C, target handle while active == 2
    };
    u32 state;     // 0x18
    s32 handle;    // 0x1C
    s32 count;     // 0x20
    f32 timer;     // 0x24
    u32 flags;     // 0x28
    s32 items[4];  // 0x2C
};

// A tracked actor: the message port that reaches it plus its entry list.
struct HazardTrackOwner {
    void (*send)(u32 msg, void* self);  // 0x00
    u8 pad04[0xDC];
    HazardTargetList* list;  // 0xE0
};

// Target actor as handed back by SendMsg(handle, 0x400, &actor).
struct HazardTargetActor {
    u8 pad00[0x24];
    s32 posFixed[3];  // 0x24, positions kept as integers
    u8 pad30[0x08];
    s32 handle;  // 0x38
};

struct HazardTrackActor {
    u8 pad00[0x24];
    s32 posFixed[3];  // 0x24, positions kept as integers
    u8 pad30[0x108];
    f32 timerCut;  // 0x138
    u8 pad13C[0x104];
    f32 moveDir[3];  // 0x240
    u8 pad24C[0x1C];
    s32 unk268;       // 0x268
    u8 pad26C[0x4];
    u8 pad270[0x18];
    s32 unk288;      // 0x288
    u8 pad28C[0x20];
    f32 scale2AC;  // 0x2AC
    u8 pad2B0[0x308];
    f32 reach5B8;  // 0x5B8
    u8 pad5BC[0x30];
    HazardTrackOwner* owner;  // 0x5EC
    u8 pad5F0[0x2A4];
    u8 pad894[0x6C];
    f32 field900;  // 0x900
};

extern float lbl_804D7830;
extern float lbl_804D7834;
extern float lbl_804D7838;
extern float lbl_804D783C;
extern float lbl_804D7840;
extern float lbl_804D7844;
extern float lbl_804D7848;
extern float lbl_804D784C;
extern float lbl_804D7850;
extern float lbl_804D7854;
extern float lbl_804D7858;
extern float lbl_804D785C;
extern float lbl_804D7860;
extern float lbl_804D7864;
extern float lbl_804D7868;
extern float lbl_804D7878;
extern float lbl_804D787C;
extern "C" f32 sqrtf(f32);

// Engine helpers used by the hazard aim/pad code below.
extern "C" f32 fn_800A8600(Vec3*, Vec3*, Vec3*, Vec3*, f32*);
extern "C" void fn_800A0030(Vec3*, Mtx34*, Vec3*);
extern "C" void fn_800A0168(Mtx34*, Mtx34*);
extern "C" void fn_800E1098(Vec3*, Vec3*, f32);
extern "C" void fn_800A78A0(void*, void*);
extern "C" void fn_800A7028(void*);
void ActorGetMatrix(Mtx34*, Actor*);

// Controller helpers.
extern "C" s32 fn_800B26B8();
extern "C" void fn_800B261C(void*, s32, HazardTargetActor**);
extern "C" s32 fn_800B1EF4(void*, s32);
extern "C" void fn_800B1F20(void*, s32);
extern "C" u32 fn_800B1ED4(void*, s32);
extern "C" void fn_800B1AB8(f32, void*, s32, s32);
extern "C" s32 fn_80162940(void*, s32);
extern "C" void fn_80162610(void*, void*, s32);
extern "C" s32 rand();

// Keeps the owner's list of up to four tracked targets up to date.
s32 HazardTrackTarget(f32 dist, s32 type, HazardMsgPort* port, HazardTrackActor* actor) {
    HazardTargetList* list = actor->owner->list;
    f32 range = (f32)dist;

    HazardTargetMsg msg;
    msg.port = port;
    port->send(0x400, &msg);

    if (type >= 0 && type < 2) {
        s32 handle = msg.handle;
        String name;
        HazardTypeQuery q;
        q.size = 0;
        q.hash = 0x5c8f66f9;
        q.unk08 = 0;
        q.value = 0;
        SendMsg(handle, 0x41e, &q);
        s32 result = q.value;

        if (msg.port != 0 && result == -0x41718503) {
            s32 found = 0;
            for (s32 i = 0; i < list->count; i++) {
                if (list->items[i] == handle) {
                    found = 1;
                    break;
                }
            }

            if (!found) {
                f32 d = lbl_804D7830;
                if (lbl_804D7830 < range) {
                    d = sqrtf(range);
                }
                if (d < lbl_804D7834 && fn_800A87A0(actor, msg.port) != 0) {
                    if (list->count != 4) {
                        list->items[list->count] = handle;
                        list->count++;
                    }
                }
            } else {
                f32 d = lbl_804D7830;
                if (lbl_804D7830 < range) {
                    d = sqrtf(range);
                }
                if (d > lbl_804D7834) {
                    s32 index = -1;
                    for (s32 i = 0; i < list->count; i++) {
                        if (list->items[i] == handle) {
                            index = i;
                            break;
                        }
                    }
                    if (index != -1) {
                        for (s32 i = index; i < list->count - 1; i++) {
                            list->items[i] = list->items[i + 1];
                        }
                        list->count--;
                        list->items[list->count] = -1;
                    }
                }
            }
        }
    }
    return 1;
}

// Averages an aim direction toward the nearest of the tracked targets and
// returns the accumulated weight. Writes the direction into out (0 when none).
// TODO HazardTargetAimDir (0x5E8 bytes): only the early-out checks are written; the averaging loop and
// the return value are placeholders (unit is NonMatching).
f32 HazardTargetAimDir(HazardTrackActor* self, Vec3* out) {
    HazardTrackOwner* owner = self->owner;
    HazardTargetList* list = owner->list;

    if (self == 0) {
        return lbl_804D7830;
    }
    if (list == 0) {
        return lbl_804D7830;
    }
    if (list->count < 1) {
        return lbl_804D7830;
    }

    f32 total = lbl_804D7830;
    out->x = lbl_804D7830;
    out->y = lbl_804D7830;
    out->z = lbl_804D7830;

    for (s32 i = 0; i < list->count; i--) {
    }
    return 0.0f;
}

// Reply of the 0x400 message: word 0 is the object the target belongs to.
struct HazardTargetProbe {
    void* object;  // 0x00
    u8 pad04[0x1C];
};

// Engine/actor helpers used by the target tracking code below.
extern "C" s32 fn_800A1F94();
extern "C" HazardTargetActor* fn_800A7A18(void*);
extern "C" f32 fn_800AA170(void*, void*);
f32 OwnerValue(Owner*, void*);
extern "C" s32 fn_800A8A74(void*, void*);
typedef void (*HazardCallback)(void*, void*);
extern "C" void fn_800B6CAC(void*, HazardCallback);
extern "C" s32 fn_800BAF40(void*);
extern "C" void fn_800BC5A8(void*, void*);
extern "C" void fn_800BC56C(void*, void*);
extern "C" void* memset(void*, int, unsigned long);

// Keeps the hazard's list of tracked targets current: adopts the target the
// controller points at and drops entries that no longer answer the 0x400 query.
// With a target in the list the hazard also ages the list timer and checks the
// controller buttons that make it call out to the target.
s32 HazardTrackListUpdate(HazardTrackActor* self, HazardTrackOwner* owner) {
    if (self == 0) {
        return 0;
    }

    HazardTargetList* list = owner->list;
    s32 slot = fn_800B26B8();
    HazardTargetActor* target;

    fn_800B261C(self, slot, &target);
    if (slot < 4 && fn_800B1EF4(self, slot) == 0 && target != 0 &&
        HazardCheckTarget((Owner*)self, target->handle) != 0) {
        fn_800B1F20(self, slot);
        if ((fn_800B1ED4(self, slot) & 0x80) == 0) {
            fn_800B1AB8(lbl_804D7878, self, slot, 1);
        }
        if (fn_800B1ED4(self, slot) & 0x80) {
            owner->send(0x7a16c04f, self);
        } else {
            s32 team = PlayerTeam(HandleToPlayer(OwnerId((Owner*)self)));

            if (PlayerTeam(HandleToPlayer(target->handle)) != team) {
                list->handle = target->handle;
                if (slot == 1) {
                    owner->send(0x64f367a2, self);
                } else if (slot == 2) {
                    owner->send(0x5a19e256, self);
                }
            } else {
                if (slot == 1) {
                    owner->send(0xbedce15e, self);
                } else if (slot == 2) {
                    owner->send(0xcd30830a, self);
                }
            }
        }
    }

    while (list->count > 0) {
        HazardTargetProbe probe;

        if (SendMsg(list->items[0], 0x400, &probe) == 1) {
            fn_800A78A0(self->pad270, probe.object);
            break;
        }

        HazardTargetList* targets = owner->list;
        s32 index = -1;

        for (s32 i = 0; i < targets->count; i++) {
            if (targets->items[i] == list->items[0]) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            fn_800A7028(self->pad270);
            return 1;
        }
        for (; index < targets->count - 1; index++) {
            targets->items[index] = targets->items[index + 1];
        }
        targets->count--;
        targets->items[targets->count] = -1;
    }

    if (list->active == 0) {
        return 1;
    }
    if (list->count < 1) {
        return 1;
    }
    if (fn_80162940(self->pad894, 0x1000) != 0) {
        owner->send(0xe929adb6, self);
        return 1;
    }
    if (fn_80162940(self->pad894, 2) != 0 && fn_80162940(self->pad894, 8) != 0 &&
        self->reach5B8 > (f32)(u32)rand() / lbl_804D787C) {
        fn_80162610(self, self->pad894, 2);
    } else if (fn_80162940(self->pad894, 1) != 0 && fn_80162940(self->pad894, 4) != 0) {
        fn_80162610(self, self->pad894, 1);
    }

    f32 t = list->timer - self->timerCut;

    list->timer = t;
    if (t < lbl_804D7830) {
        list->timer = lbl_804D7844;
        return 1;
    }
    return 0;
}

// --- hazard target tracking (continued) ---

extern "C" f32 fn_800AA050(void*);
extern float lbl_804D7880;

// Per-frame update of the hazard's target tracking state: adopts the target the
// controller points at, ages the target distance limit, drops targets that stop
// answering the 0x400 query and drives the target call-out messages. Returns 1
// while the hazard should keep tracking.
s32 HazardTrackListUpdateMain(HazardTrackActor* self, HazardTrackOwner* owner) {
    if (self == 0) {
        return 0;
    }

    HazardTargetList* list = owner->list;

    if ((list->state >> 24) == 0x13 && fn_800A1F94() == 0) {
        list->state = -1;
        fn_800A7028(self->pad270);
        return 1;
    }

    fn_800A7A18(self->pad270);
    s32 slot = fn_800B26B8();
    HazardTargetActor* target;

    fn_800B261C(self, slot, &target);
    if (slot < 4 && fn_800B1EF4(self, slot) == 0 && target != 0 &&
        HazardCheckTarget((Owner*)self, target->handle) != 0) {
        fn_800B1F20(self, slot);
        if ((fn_800B1ED4(self, slot) & 0x80) == 0) {
            fn_800B1AB8(lbl_804D7878, self, slot, 1);
        }
        if (fn_800B1ED4(self, slot) & 0x80) {
            owner->send(0x7a16c04f, self);
        } else {
            s32 team = PlayerTeam(HandleToPlayer(OwnerId((Owner*)self)));

            if (PlayerTeam(HandleToPlayer(target->handle)) == team) {
                if (slot == 1) {
                    owner->send(0xbedce15e, self);
                } else if (slot == 2) {
                    owner->send(0xcd30830a, self);
                }
            } else {
                list->handle = target->handle;
                if (slot == 1) {
                    owner->send(0x64f367a2, self);
                } else if (slot == 2) {
                    owner->send(0x5a19e256, self);
                }
            }
        }
    }

    if (list->active == 0) {
        return 1;
    }
    if (list->count < 1) {
        fn_800B6CAC(self, (HazardCallback)HazardTargetAimDir);
        return 1;
    }

    f32 dist;

    if (list->active == 1) {
        dist = fn_800AA170(self, &list->items[0]);
    } else if (list->active < 1 || list->active > 2) {
        dist = lbl_804D7878;
    } else {
        HazardTargetProbe probe;

        if (list->items[0] == -1 || SendMsg(list->items[0], 0x400, &probe) == 0 ||
            probe.object == 0) {
            dist = lbl_804D7878;
        } else {
            dist = fn_800AA050(self);
        }
    }

    if (dist < list->limit) {
        owner->send(0x3147a953, self);
        list->active = 0;
        fn_800A7028(self->pad270);
        return 1;
    }

    s32 valid;

    if (list->active == 1) {
        valid = fn_800A8A74(self, &list->items[0]);
    } else if (list->active < 1 || list->active > 2) {
        valid = 0;
    } else {
        HazardTargetProbe probe;

        memset(&probe, 0, 0x20);
        if (SendMsg(list->items[0], 0x400, &probe) == 0) {
            valid = 0;
        } else {
            valid = fn_800A87A0(self, probe.object);
        }
    }

    if (valid != 0) {
        if (list->active == 2) {
            String name;
            HazardTypeQuery q;

            q.size = 0;
            q.hash = 0x33032fa9;
            q.unk08 = 0;
            q.value = 0;
            if (SendMsg(list->items[0], 0x41e, &q) != 0) {
                if (q.value == 0) {
                    owner->send(0x30a0d567, self);
                } else {
                    owner->send(0x6d5846fd, self);
                }
            }
        } else {
            owner->send(0x6d5846fd, self);
        }
    }

    f32 timer = list->timer - self->timerCut;

    list->timer = timer;
    if (timer < lbl_804D7830) {
        return 1;
    }

    if (fn_800BAF40(self) == -1) {
        if (list->active == 1) {
            fn_800BC5A8(self, &list->items[0]);
        } else if (list->active > 0 && list->active < 3) {
            HazardTargetProbe probe;

            memset(&probe, 0, 0x20);
            if (SendMsg(list->items[0], 0x400, &probe) != 0) {
                fn_800BC56C(self, probe.object);
            }
        }
        if (fn_800BAF40(self) == -1) {
            owner->send(0xacbec0a8, self);
        }
    }
    return 0;
}

// Variant of the target-list update: also validates the target it is already
// tracking through the 0x400 query and reacts to the controller buttons that
// make the hazard call out to the target. Returns 1 while tracking continues.
s32 HazardTrackListUpdateAlt(HazardTrackActor* self, HazardTrackOwner* owner) {
    if (self == 0) {
        return 0;
    }

    HazardTargetList* list = owner->list;

    if ((list->state >> 24) == 0x13 && fn_800A1F94() == 0) {
        list->state = -1;
        fn_800A7028(self->pad270);
        return 1;
    }

    HazardTargetActor* current = fn_800A7A18(self->pad270);
    s32 slot = fn_800B26B8();
    HazardTargetActor* target;

    fn_800B261C(self, slot, &target);
    if (slot < 4 && fn_800B1EF4(self, slot) == 0 && target != 0 && target != current &&
        HazardCheckTarget((Owner*)self, target->handle) != 0) {
        String name;
        HazardTypeQuery q;

        q.size = 0;
        q.hash = 0x6c47d293;
        q.unk08 = 0;
        q.value = 0;
        SendMsg(target->handle, 0x41e, &q);
        s32 found = q.value;

        fn_800B1F20(self, slot);
        if ((fn_800B1ED4(self, slot) & 0x80) == 0 &&
            (found == -1 || ((u32)found >> 24) != 1)) {
            fn_800B1AB8(lbl_804D7878, self, slot, 1);
        }
        if (fn_800B1ED4(self, slot) & 0x80) {
            owner->send(0x7a16c04f, self);
        } else {
            s32 team = PlayerTeam(HandleToPlayer(OwnerId((Owner*)self)));

            if (PlayerTeam(HandleToPlayer(target->handle)) == team) {
                if (slot == 1) {
                    owner->send(0xbedce15e, self);
                } else if (slot == 2) {
                    owner->send(0xcd30830a, self);
                }
            } else {
                list->handle = target->handle;
                if (slot == 1) {
                    owner->send(0x64f367a2, self);
                } else if (slot == 2) {
                    owner->send(0x5a19e256, self);
                }
            }
        }
    }

    if (list->active == 0) {
        return 1;
    }
    if (list->state == -1) {
        return 1;
    }
    if (current == 0 || HazardCheckTarget((Owner*)self, current->handle) == 0) {
        list->state = -1;
        return 1;
    }

    f32 dist;

    if (list->active == 1) {
        dist = fn_800AA170(self, &list->items[0]);
    } else if (list->active < 1 || list->active > 2) {
        dist = lbl_804D7878;
    } else {
        HazardTargetProbe probe;

        if (list->items[0] == -1 || SendMsg(list->items[0], 0x400, &probe) == 0 ||
            probe.object == 0) {
            dist = lbl_804D7878;
        } else {
            dist = fn_800AA050(self);
        }
    }

    if (dist < list->limit) {
        owner->send(0x3147a953, self);
        list->active = 0;
        return 1;
    }

    if (fn_800AA050(self) > lbl_804D7880) {
        self->unk268 = 4;
        list->state = -1;
        return 1;
    }

    if (fn_80162940(self->pad894, 0x1000) != 0) {
        owner->send(0xe929adb6, self);
    } else {
        if (fn_80162940(self->pad894, 2) != 0 && fn_80162940(self->pad894, 8) != 0 &&
            (f32)(u32)rand() / lbl_804D787C < self->reach5B8) {
            fn_80162610(self, self->pad894, 2);
        } else if (fn_80162940(self->pad894, 1) != 0 &&
                   fn_80162940(self->pad894, 4) != 0) {
            fn_80162610(self, self->pad894, 1);
        }
    }

    s32 valid;

    if (list->active == 1) {
        valid = fn_800A8A74(self, &list->items[0]);
    } else if (list->active < 1 || list->active > 2) {
        valid = 0;
    } else {
        HazardTargetProbe probe;

        memset(&probe, 0, 0x20);
        if (SendMsg(list->items[0], 0x400, &probe) == 0) {
            valid = 0;
        } else {
            valid = fn_800A87A0(self, probe.object);
        }
    }

    if (valid != 0) {
        if (list->active == 2) {
            String name;
            HazardTypeQuery q;

            q.size = 0;
            q.hash = 0x33032fa9;
            q.unk08 = 0;
            q.value = 0;
            if (SendMsg(list->items[0], 0x41e, &q) != 0) {
                if (q.value == 0) {
                    owner->send(0x30a0d567, self);
                } else {
                    owner->send(0x6d5846fd, self);
                }
            }
        } else {
            owner->send(0x6d5846fd, self);
        }
    }

    if (fn_800BAF40(self) == -1) {
        if (list->active == 1) {
            fn_800BC5A8(self, &list->items[0]);
        } else if (list->active > 0 && list->active < 3) {
            HazardTargetProbe probe;

            memset(&probe, 0, 0x20);
            if (SendMsg(list->items[0], 0x400, &probe) != 0) {
                fn_800BC56C(self, probe.object);
            }
        }
        if (fn_800BAF40(self) == -1) {
            owner->send(0xacbec0a8, self);
        }
    }
    return 0;
}

// Layout of the auto-target list shared by the hazard's tracking callbacks.
struct HazardAutoTargetList {
    s32 unk00;    // 0x00
    s32 active;   // 0x04
    f32 limit;    // 0x08, distance at which the target is dropped
    s32 target;   // 0x0C, handle of the tracked target
    u8 pad10[0x08];
    u32 state;  // 0x18
    s32 handle; // 0x1C
    s32 count;  // 0x20
    f32 timer;  // 0x24
    u32 flags;  // 0x28
};

// The tracked actor's message port plus the target list it keeps alive.
struct HazardAutoTargetOwner {
    void (*send)(u32 msg, void* self);  // 0x00
    u8 pad04[0xDC];
    HazardAutoTargetList* list;  // 0xE0
};

extern float lbl_804D7884;
extern "C" void fn_800B2418(f32, void*, s32);
extern "C" s32 fn_800B22F8(void*, s32);
extern "C" void fn_800B2454(void*, s32);

// Refreshes the list of targets the hazard tracks: adopts the target the
// controller points at, asks it for its type hash and drops entries that no
// longer answer the 0x400 query. Returns 1 while a target is being tracked.
s32 HazardTrackListRefresh(HazardTrackActor* self, HazardAutoTargetOwner* owner) {
    if (self == 0) {
        return 0;
    }

    HazardAutoTargetList* list = owner->list;
    s32 slot = fn_800B26B8();
    HazardTargetActor* target;

    fn_800B261C(self, slot, &target);
    if (slot < 4 && (list->flags & 4) == 0 && fn_800B1EF4(self, slot) == 0) {
        if (target == 0 || HazardCheckTarget((Owner*)self, target->handle) == 0) {
            if (slot == 3) {
                owner->send(0x3529a482, self);
                fn_800B1F20(self, slot);
            }
        } else {
            String name;
            HazardTypeQuery q;

            q.size = 0;
            q.hash = 0x6c47d293;
            q.unk08 = 0;
            q.value = 0;
            SendMsg(target->handle, 0x41e, &q);
            s32 found = q.value;

            fn_800B1F20(self, slot);
            if ((fn_800B1ED4(self, slot) & 0x80) == 0 &&
                (found == -1 || ((u32)found >> 24) != 1)) {
                fn_800B1AB8(lbl_804D7878, self, slot, 1);
            }
            if (fn_800B1ED4(self, slot) & 0x80) {
                owner->send(0x7a16c04f, self);
            } else {
                s32 team = PlayerTeam(HandleToPlayer(OwnerId((Owner*)self)));

                if (PlayerTeam(HandleToPlayer(target->handle)) != team) {
                    list->state = target->handle;
                    list->handle = target->handle;
                    if (slot == 1) {
                        owner->send(0x64f367a2, self);
                    } else if (slot == 2) {
                        owner->send(0x5a19e256, self);
                    }
                    return 1;
                }
                if (slot == 1) {
                    owner->send(0xbedce15e, self);
                } else if (slot == 2) {
                    owner->send(0xcd30830a, self);
                }
            }
        }
    }

    if (list->active == 0) {
        return 1;
    }
    if (list->count > 0) {
        return 1;
    }

    f32 dist;

    if (list->active == 1) {
        dist = fn_800AA170(self, &list->target);
    } else if (list->active < 1 || list->active > 2) {
        dist = lbl_804D7878;
    } else {
        HazardTargetProbe probe;

        if (list->target == -1 || SendMsg(list->target, 0x400, &probe) == 0 ||
            probe.object == 0) {
            dist = lbl_804D7878;
        } else {
            dist = OwnerValue((Owner*)self, probe.object);
        }
    }

    if (dist < list->limit) {
        owner->send(0x3147a953, self);
        list->active = 0;
        fn_800A7028(self->pad270);
        return 1;
    }

    s32 valid;

    if (list->active == 1) {
        valid = fn_800A8A74(self, &list->target);
    } else if (list->active < 1 || list->active > 2) {
        valid = 0;
    } else {
        HazardTargetProbe probe;

        memset(&probe, 0, 0x20);
        if (SendMsg(list->target, 0x400, &probe) == 0) {
            valid = 0;
        } else {
            valid = fn_800A87A0(self, probe.object);
        }
    }

    if (valid != 0) {
        if (list->active == 2) {
            String name;
            HazardTypeQuery q;

            q.size = 0;
            q.hash = 0x33032fa9;
            q.unk08 = 0;
            q.value = 0;
            if (SendMsg(list->target, 0x41e, &q) != 0) {
                if (q.value == 0) {
                    owner->send(0x30a0d567, self);
                } else {
                    owner->send(0x6d5846fd, self);
                }
            }
        } else {
            owner->send(0x6d5846fd, self);
        }
    }

    if (fn_800BAF40(self) == -1) {
        if (list->active == 1) {
            fn_800BC5A8(self, &list->target);
        } else if (list->active > 0 && list->active < 3) {
            HazardTargetProbe probe;

            memset(&probe, 0, 0x20);
            if (SendMsg(list->target, 0x400, &probe) != 0) {
                fn_800BC56C(self, probe.object);
            }
        }
        if (fn_800BAF40(self) == -1) {
            owner->send(0xacbec0a8, self);
        }
    }
    return 0;
}

// Controller-slot hook of the tracked actor: when the first slot becomes
// active it starts the actor's move and remembers where it is heading.
s32 HazardTrackSlotStart(HazardTrackActor* self, HazardAutoTargetOwner* owner) {
    if (self == 0) {
        return 0;
    }

    s32 slot;
    HazardAutoTargetList* list;
    HazardTargetActor* target;

    list = owner->list;
    slot = fn_800B26B8();
    fn_800B261C(self, slot, &target);
    if (slot == 1) {
        if (fn_800B1EF4(self, slot) == 0) {
            fn_800B1F20(self, slot);
            fn_800B2418(lbl_804D7884, self, slot);
            self->unk268 = fn_800B22F8(self, slot);
            return 1;
        }
        fn_800B2454(self, slot);
    }
    if (list->active != 0) {
        return 1;
    }
    return 0;
}

// Resets the hazard's tracking helper and installs its per-frame callback.
s32 HazardTrackReset(HazardTrackActor* self) {
    fn_800A7028(self->pad270);
    fn_800B6CAC(self, (HazardCallback)HazardTargetAimDir);
    return 1;
}

extern "C" void fn_800A7B08(void*, s32, s32);
extern float lbl_804D7888;

// Restarts the hazard's target tracking: resets the list entry and its timer,
// re-enables the tracking helper and resolves the current target's object.
s32 HazardTrackRestart(HazardTrackActor* self, HazardTrackOwner* owner) {
    HazardTargetList* list;

    if (self == 0) {
        return 0;
    }
    list = owner->list;
    list->unk00 = 4;
    list->handle = -1;
    list->timer = lbl_804D7888;
    fn_800A7B08(self->pad270, 3, 1);
    fn_800A7B08(self->pad270, 4, 1);
    self->unk288 = 0;
    fn_800A7028(self->pad270);

    HazardTargetProbe probe;

    if (SendMsg(list->items[0], 0x400, &probe) != 0) {
        fn_800A78A0(self->pad270, probe.object);
    }
    return 1;
}

extern "C" void fn_800B6D04(void*, void (*)(void*, void*));

// Restarts the hazard's target tracking in mode 3: resets the list entry and
// timer, re-enables the tracking helper and installs the per-frame callback.
s32 HazardTargetStartMode(HazardTrackActor* self, HazardTrackOwner* owner) {
    if (self == 0) {
        return 0;
    }

    HazardTargetList* list = owner->list;
    list->unk00 = 3;
    list->handle = -1;
    list->timer = lbl_804D7844;
    fn_800A7B08(self->pad270, 3, 1);
    fn_800A7B08(self->pad270, 4, 1);
    self->unk288 = 3;
    fn_800A7028(self->pad270);

    HazardTargetProbe probe;

    if (SendMsg(list->items[0], 0x400, &probe) != 0) {
        fn_800A78A0(self->pad270, probe.object);
        fn_800B6D04(self, (HazardCallback)HazardTargetAimDir);
    }
    return 1;
}

// Restarts the hazard's target tracking in mode 2: resets the list entry,
// re-enables the tracking helper and queries the current target's object.
s32 HazardTargetStartQuery(HazardTrackActor* self, HazardTrackOwner* owner) {
    if (self == 0) {
        return 0;
    }

    HazardTargetList* list = owner->list;
    list->unk00 = 2;
    list->handle = -1;
    fn_800A7B08(self->pad270, 3, 1);
    fn_800A7B08(self->pad270, 4, 1);
    self->unk288 = 3;

    HazardTargetProbe probe;

    memset(&probe, 0, 0x20);
    if (SendMsg(list->state, 0x400, &probe) != 0) {
        fn_800A78A0(self->pad270, probe.object);
    }
    return 1;
}

extern "C" void fn_800A78E4(void*, void*);

// Restarts the hazard's target tracking in mode 1: resets the target list,
// re-enables the tracking helper and resolves the current target's object.
s32 HazardTargetStartTrack(HazardTrackActor* self, HazardAutoTargetOwner* owner) {
    HazardAutoTargetList* list;

    if (self == 0) {
        return 0;
    }
    list = owner->list;
    list->unk00 = 1;
    list->handle = -1;
    fn_800A7B08(self->pad270, 3, 1);
    fn_800A7B08(self->pad270, 4, 0);
    self->unk288 = 3;
    if (list->active == 1) {
        fn_800A78E4(self->pad270, &list->target);
    } else if (list->active > 1 && list->active < 3) {
        HazardTargetProbe probe;

        memset(&probe, 0, 0x20);
        if (SendMsg(list->target, 0x400, &probe) != 0) {
            fn_800A78A0(self->pad270, probe.object);
        }
    }
    return 1;
}

// Puts the hazard's target tracking on hold: clears the target list entry and
// re-enables the tracking helper in its default state.
s32 HazardTargetStopTrack(HazardTrackActor* self, HazardAutoTargetOwner* owner) {
    if (self == 0) {
        return 0;
    }
    owner->list->unk00 = 0;
    fn_800A7B08(self->pad270, 3, 1);
    fn_800A7B08(self->pad270, 4, 0);
    self->unk288 = 3;
    return 1;
}

// Payload of the "release tracked entry" message (0xC14).
struct HazardTrackDetachMsg {
    void* unk00;  // 0x00, not filled in by the sender
    s32 item;     // 0x04, entry index
    s32 owner;    // 0x08, owner handle
    u32 hash;     // 0x0C
};

// Handler of the release message (0xC14): passes the release on to the tracked
// object and drops the entry from the owner's list.
s32 HazardTrackReleaseMsg(s32 msg, void* p) {
    HazardMsgData* data = (HazardMsgData*)p;
    HazardTrackActor* actor = (HazardTrackActor*)data->owner;

    if (msg != 0xc14) {
        return 0;
    }
    HazardTargetList* list = actor->owner->list;
    if (data->value != 0x1ff464) {
        return 0;
    }
    s32 item = data->unk08;
    s32 ownerId = OwnerId((Owner*)actor);
    s32 handle = OwnerId((Owner*)actor);

    if (handle != -1) {
        HazardTrackDetachMsg out;

        out.item = item;
        out.owner = ownerId;
        out.hash = 0x8140bb1f;
        SendMsg(handle, 0xc14, &out);
    }
    if (data->unk08 == list->state) {
        list->state = -1;
    } else if (data->unk08 == list->handle) {
        list->handle = -1;
    }
    return 1;
}

// Handler of the track-state messages: reads (set != 0) or writes the fields of
// the owner's target list that the message names.
s32 HazardTrackStateMsg(void* self, s32 msg, void* p, s32 set) {
    HazardTargetList* list = ((HazardTrackOwner*)self)->list;
    s32* data = (s32*)p;

    if (set == 0) {
        switch (msg) {
        case 0x329c15b:
            list->pos.x = ((f32*)data)[0];
            list->pos.y = ((f32*)data)[1];
            list->pos.z = ((f32*)data)[2];
            list->active = 1;
            return 1;
        case 0x33fa508:
            list->limit = *(f32*)data;
            return 1;
        case 0x315c41d: {
            HazardMsgData notify;

            list->target = *data;
            list->active = 2;
            SendMsg(list->target, 0x400, &notify);
            return 1;
        }
        case 0xfba23962:
            list->state = *data;
            return 1;
        case 0x4d4915e6:
            list->handle = *data;
            return 1;
        case 0x4a6c281e:
            if ((s32)data == 1) {
                list->flags |= 1;
            } else {
                list->flags &= ~1u;
            }
            break;
        case 0x7b88da40:
            if (*data == 1) {
                list->flags |= 4;
            } else {
                list->flags &= ~4u;
            }
            return 1;
        case 0x99a725c:
            break;
        default:
            return 0;
        }
        if ((s32)data == 1) {
            list->flags |= 2;
        } else {
            list->flags &= ~2u;
        }
        return 0;
    }

    switch (msg) {
    case 0x329c15b:
        ((f32*)data)[0] = list->pos.x;
        ((f32*)data)[1] = list->pos.y;
        ((f32*)data)[2] = list->pos.z;
        return 1;
    case 0x33fa508:
        *(f32*)data = list->limit;
        return 1;
    case 0x315c41d:
        *data = list->target;
        return 1;
    case 0xfba23962:
        *data = list->state;
        return 1;
    case 0x4d4915e6:
        *data = list->handle;
        return 1;
    default:
        return 0;
    }
}

// One entry of the per-state callback table at lbl_8039F528 (5 states).
struct HazardStateFns {
    void (*enter)(void*, void*);  // 0x00, run when the state is entered
    s32 (*leave)(void*, void*);   // 0x04, 0 = the transition is refused
    s32 (*update)(void*, void*);  // 0x08, 0 = nothing to do this frame
};

// Tracking list of the state callbacks: the leading word is the index of the
// current state, 0x18/0x1C hold a requested transition.
struct HazardTrackStateList {
    s32 index;    // 0x00, index into lbl_8039F528
    s32 active;   // 0x04
    f32 speed;    // 0x08
    s32 target;   // 0x0C, -1 while there is no target
    u8 pad10[0x08];
    s32 state;    // 0x18
    s32 queued;   // 0x1C
    s32 count;    // 0x20
    f32 timer;    // 0x24
    s32 unk28;    // 0x28
    s32 unk2C;    // 0x2C
    s32 unk30;    // 0x30
    s32 unk34;    // 0x34
    s32 unk38;    // 0x38
};

// Actor the state callbacks run on: it embeds its zone list and keeps its
// tracking list at 0xE0.
struct HazardTrackStateActor {
    u8 pad00[0xBC];
    s32 unkBC;                   // 0xBC, -1 while the actor is not attached
    u8 padC0[0x18];
    s32 (*update)(void*, HazardTrackStateActor*);  // 0xD8, per-frame callback
    u8 padDC[0x4];
    HazardTrackStateList* list;  // 0xE0
    u8 padE4[0x104];
    u8 zoneList;  // 0x1E8, first byte of the embedded ZoneList
};

extern HazardStateFns lbl_8039F528[];

// The zone walk takes the walker as an int (see the retail mangled name).
typedef s32 (*ZoneWalkFn)(u32, u32, f32, s32);
void ZoneWalkNodes(ZoneList*, ZoneWalkFn, s32);
s32 HazardTrackTarget(f32, s32, HazardMsgPort*, HazardTrackActor*);

// State the list asks for: idle until it has a target, then a transition
// through the queued state, then the distance/time based decisions. Several
// callbacks carry a copy of this, so the owner null check is repeated.
static inline s32 HazardTrackStatePick(void* owner, HazardTrackStateActor* self) {
    if (owner == 0) {
        return 0;
    }

    HazardTrackStateList* list = self->list;
    if (list->active == 0) {
        return 0;
    }

    if (list->state == -1 && list->queued == -1) {
        if (list->count <= 0) {
            return 1;
        }
        if (list->timer >= lbl_804D7830) {
            return 3;
        }
        return 4;
    }

    if (list->queued != -1) {
        list->state = list->queued;
    }
    return 2;
}

// Runs the state callback of the frame, then moves the list to the state it
// asks for. Returns 0 when the actor is not attached to a zone.
s32 HazardTrackStateUpdate(void* owner, HazardTrackStateActor* self) {
    if (owner == 0) {
        return 0;
    }

    HazardTrackStateList* list = self->list;
    if (self->unkBC == -1) {
        return 0;
    }

    ZoneWalkNodes((ZoneList*)&self->zoneList, (ZoneWalkFn)HazardTrackTarget, (s32)owner);

    if (lbl_8039F528[list->index].update(owner, self) != 0) {
        s32 next = HazardTrackStatePick(owner, self);
        s32 cur = list->index;
        if (next != cur || next == 2) {
            if (lbl_8039F528[cur].leave(owner, self) != 0) {
                lbl_8039F528[next].enter(owner, self);
            }
        }
    }
    return 1;
}

// Drops the tracking list and unhooks the per-actor update callback.
void HazardTrackStateShutdown(void* owner, HazardTrackStateActor* self) {
    if (owner != 0) {
        VFree(self->list);
        fn_800B6CAC(owner, (HazardCallback)HazardTargetAimDir);
    }
}

extern "C" void fn_800BBCA8(void*);

// Installs the tracking state callbacks on a hazard track actor: allocates the
// state list, fills in its defaults and runs the entry callback of state 0.
void HazardTrackStateInit(void* owner, HazardTrackStateActor* self) {
    if (owner == 0) {
        return;
    }

    HazardTrackStateList* list = (HazardTrackStateList*)VAlloc(0x3c);
    if (list == 0) {
        self->list = 0;
        return;
    }

    self->update = HazardTrackStateUpdate;
    self->list = list;
    list->index = 0;
    list->target = -1;
    list->active = 0;
    list->speed = lbl_804D785C;
    list->state = -1;
    list->queued = -1;
    list->timer = lbl_804D7844;
    list->count = 0;
    list->unk28 = 0;
    list->unk2C = -1;
    list->unk30 = -1;
    list->unk34 = -1;
    list->unk38 = -1;

    fn_800BBCA8(owner);
    fn_800A7028(((HazardTrackActor*)self)->pad270);
    lbl_8039F528[list->index].enter(owner, self);
}

// Global the hazard messages are registered in, plus the shared flag word that
// says whether the hazard message pump is running.
struct HazardTrackEvent {
    u8 pad00[0x04];
    Actor* actor;  // 0x04
    u8 pad08[0x0C];
    s32 state;     // 0x14
};

extern HazardTrackEvent lbl_8039F568;
extern u32 lbl_804D2E30[1];

// Tests one bit of the shared hazard flag word.
static inline s32 HazardFlagSet(s32 bit) {
    return lbl_804D2E30[bit / 32] & (1 << (bit & 31));
}

extern "C" s32 fn_800F8AF4(u32);
extern "C" s32 fn_800FD424(s32);
extern "C" void fn_800FC764(s32, s32);
extern "C" s32 fn_801409EC(s32);
extern "C" void fn_801633FC(u32, s32, s32);
s32 Fn80175D60(u32);

// Message handler of the hazard track actor: maps the hashed name in the
// message payload to one of the seven animation states of the track.
s32 HazardTrackEventMsg(u32* msg, s32 id, u32 track, s32 param) {
    s32 found = Fn80175D60(track);

    HazardTrackEvent* ev = 0;
    if (HazardFlagSet(0) != 0) {
        ev = &lbl_8039F568;
    }
    if (ev == 0) {
        return 0;
    }

    switch (id) {
    case 0x002026EF:
        if (found == 0 && ev->state == 1) {
            ev->state = 0;
        }
        fn_800FC764(ev->actor->handle, param);
        return 1;

    case 0x0B59E861:
        if (found != 0) {
            return 1;
        }

        s32 state;
        switch (fn_800F8AF4(*msg)) {
        case 0x114F1979:
            state = fn_800FD424(0);
            break;
        case 0x6B4B4A10:
            state = fn_800FD424(1);
            break;
        case 0x21425B82:
            state = fn_800FD424(2);
            break;
        case -0x52A536D:
            state = fn_800FD424(3);
            break;
        case 0x6BA1BC35:
            state = fn_800FD424(4);
            break;
        case -0x2413FEB3:
            state = fn_800FD424(5);
            break;
        case -0x1AB49425:
            state = fn_800FD424(6);
            break;
        case 0x154F7C95:
            state = fn_800FD424(7);
            break;
        default:
            state = fn_800FD424(1);
            break;
        }

        fn_801633FC(track, state, fn_801409EC(0xb));
        return 1;

    default:
        return 0;
    }
}

// Nine name-hash entries the hazard track event registers and clears.
extern SplineNameEntry lbl_8039F580[];

// Shuts the hazard track event down: drops the active state, then clears the
// spline span and the nine name hashes it owns.
void HazardTrackEventShutdown(Actor* self) {
    if (self->zone == 0) {
        return;
    }

    HazardTrackEvent* ev = (HazardTrackEvent*)self->zone;
    if (ev->state == 1) {
        ev->state = 0;
    }

    SplineBitClearSpan((int)self->zone, (int)&lbl_8039F568, 0x18, lbl_804D2E30);
    SplineClearNameHashes(lbl_8039F580, 9);
}

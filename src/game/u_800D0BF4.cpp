// u_800D0BF4.cpp: the "Zone" actor type (0x800D0BB8-0x800D32E8).
// The streams carry power-up state between machines: a tagged header ("[PWRUPX]" clears the
// stored power-up, "[PWERUP]" is followed by two 12-byte blocks).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

struct ActorType;

struct Vec3 {
    float x;
    float y;
    float z;
};

struct ActorInfo {
    char pad0[0x18];
    u32 unk18;     // 0x18
    u32 unk1C;     // 0x1C
    u32 unk20;     // 0x20
    u32 unk24;     // 0x24
    char pad28[0x8];
    float posX;    // 0x30
    float posY;    // 0x34
    float posZ;    // 0x38
    float scale;   // 0x3C
    char pad40[0x6C];
    u32 field_AC;  // 0xAC
    char padB0[0x10];
    int unkC0;     // 0xC0
};

// Sibling layout of the linked object (see ActorInfo), kept in a 0x28-byte record.
struct ZoneState {
    u32 unk00;     // 0x00
    u32 unk04;     // 0x04
    u32 unk08;     // 0x08
    int posX;      // 0x0C
    int posY;      // 0x10
    int posZ;      // 0x14
    u32 active;    // 0x18
    u32 unk1C;     // 0x1C
    u32 unk20;     // 0x20
    float timer;   // 0x24
};

// Node of a singly-linked list of 0x10-byte records with a free list.
struct ZoneListNode {
    u32 key;                 // 0x00
    float unk04;             // 0x04
    int unk08;               // 0x08
    ZoneListNode* next;      // 0x0C
};

struct Actor;

// Node pool plus the two list heads and the per-slot flag word used by the
// zone hit queries.
struct ZoneList {
    u8* buf;                 // 0x00
    ZoneListNode* head;      // 0x04
    ZoneListNode* free;      // 0x08
    int count;               // 0x0C
    struct Actor* actor;     // 0x10
    u32 unk14;               // 0x14
    u32 unk18;               // 0x18
    u32 mask[8];             // 0x1C
};

struct ActorType;

struct Actor {
    char pad0[0x24];
    u32 x;             // 0x24 (integer world position)
    u32 y;             // 0x28
    u32 z;             // 0x2C
    int (*handle)(int cmd, void* data);  // 0x30 (message handler)
    char pad34[0x4];   // 0x34
    u32 unk38;         // 0x38 (actor id)
    char pad3C[0x4];   // 0x3C
    void* unk40;       // 0x40
    char pad44[0x10];  // 0x44
    int unk54;         // 0x54 (model of a class-2 actor)
    char pad58[0x2C];  // 0x58
    ActorInfo* info;   // 0x84
    ActorType* type;   // 0x88
    int id;            // 0x8C
    int unk90;         // 0x90 (model of a class-1 actor)
    char pad94[0x10];  // 0x94
    u8* powerUp;       // 0xA4
    char padA8[0x1C];  // 0xA8
    u32* unkC4;        // 0xC4 (first word is a class hash)
    char padC8[0x4];   // 0xC8
    int unkCC;         // 0xCC (model of a class-0x13 actor)
    char padD0[0x58];  // 0xD0
    u32 unk128;        // 0x128
    char pad12C[0x40]; // 0x12C
    u32 unk16C;        // 0x16C (state flags)
    char pad170[0x4];  // 0x170
    float unk174;      // 0x174
    int unk178;        // 0x178
    int unk17C;        // 0x17C
    char pad180[0x4];  // 0x180
    void* unk184;      // 0x184
    char pad188[0x68]; // 0x188
    float unk1F0;      // 0x1F0 (focus range)
    char pad1F4[0x5C0]; // 0x1F4
    u32 unk7B4;        // 0x7B4 (actor flags)
    char pad7B8[0xAD0]; // 0x7B8
    void* unk1288;     // 0x1288
    char pad128C[0xAD8]; // 0x128C
    Actor* unk1D64;    // 0x1D64 (linked object A)
    Actor* unk1D68;    // 0x1D68 (linked object B)
    char pad1D6C[0x18];
    float unk1D84;     // 0x1D84
    u32 unk1D88;       // 0x1D88
    u32 unk1D8C;       // 0x1D8C
    int unk1D90;       // 0x1D90 (last hit distance)
    int unk1D94;       // 0x1D94 (current query result)
    int unk1D98;       // 0x1D98 (target rejected)
    int unk1D9C;       // 0x1D9C (tracked actor id)
    int unk1DA0;       // 0x1DA0
    u32 unk1DA4;       // 0x1DA4
    u32 unk1DA8;       // 0x1DA8
    Actor* unk1DAC;    // 0x1DAC
    int unk1DB0;       // 0x1DB0
    int unk1DB4;       // 0x1DB4 (candidate actor id)
    int unk1DB8;       // 0x1DB8
    char pad1DBC[0x4]; // 0x1DBC
    u32 unk1DC0;       // 0x1DC0
    u32 unk1DC4;       // 0x1DC4
    char pad1DC8[0x8]; // 0x1DC8
    int unk1DD0;       // 0x1DD0
    ZoneList zoneList; // 0x1DD4
    u8 zoneBuf[0x814]; // 0x1E10 (node pool of the zone list)
    struct { Vec3 rot; Vec3 pos; } place;  // 0x2624
    char pad263C[0xC]; // 0x263C
    float unk2648;     // 0x2648
    char pad264C[0x24]; // 0x264C
    u8 unk270[0x58];   // 0x2670
    void* unk26C8;     // 0x26C8
    char pad26CC[0x18]; // 0x26CC
    Actor* unk26E4;    // 0x26E4
    char pad26E8[0xAC]; // 0x26E8
    Actor* unk2794;    // 0x2794
    u32 unk2798;       // 0x2798
    char pad279C[0x14]; // 0x279C
    int unk27B0;       // 0x27B0
};

// Power-up record referenced through actor->powerUp.
struct ZonePowerUp {
    u32 data[7];   // 0x00
    int active;    // 0x1C
    float timer;   // 0x20
};

// Serialization stream state shared by the handlers below.
struct ZoneStream {
    Actor* actor;  // 0x00
    int size;      // 0x04
    u8* ptr;       // 0x08
    int avail;     // 0x0C
    int status;    // 0x10
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s, u32 h) {
    int i;

    if (s == 0) {
        return 0;
    }
    for (i = 0; s[i] != 0; i++) {
        h = h * 31 + (unsigned char)toupper(s[i]);
    }
    return h % 0xFFFFFFFF;
}

extern "C" void fn_80026660(int id, void (*handler)(), int arg);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void* memcpy(void* dst, const void* src, u32 n);
extern "C" void* memset(void* dst, int c, u32 n);
void VFree(void* p);
void* VAlloc(int size);
int ActorInitBase(Actor* actor);
extern float lbl_804D7018;
int ActorTypeRegister(ActorType* type);
bool ZoneSpawnActorMsg(Actor* actor, int msg, void* data);
extern ActorType lbl_8039B9F0;

void ZoneRegisterActor(void) {
    fn_80026660(0x28, (void (*)())ZoneSpawnActorMsg, 0);
    ActorTypeRegister(&lbl_8039B9F0);
}

void ZoneReadPowerUp(ZoneStream* s) {
    Actor* a = s->actor;

    if (s->avail >= 4) {
        u32* tag = (u32*)s->ptr;

        s->avail += 4;
        s->size -= 4;
        s->ptr += 4;
        if (*tag == StrHash("[PWRUPX]", 0)) {
            VFree(a->powerUp);
            a->powerUp = 0;
        } else if (*tag == StrHash("[PWERUP]", 0) && s->avail >= 0x18) {
            memcpy(a->powerUp, s->ptr, 0xC);
            s->avail -= 0xC;
            s->ptr += 0xC;
            memcpy(a->powerUp + 0xC, s->ptr, 0xC);
            s->avail -= 0xC;
            s->ptr += 0xC;
        }
    }
}

void ZoneWritePowerUp(ZoneStream* s) {
    u8* data = s->actor->powerUp;

    if (data == 0) {
        if (s->size < 4) {
            s->status = 0;
        } else {
            u32 tag = StrHash("[PWRUPX]", 0);

            memcpy(s->ptr, &tag, 4);
            s->avail += 4;
            s->size -= 4;
            s->ptr += 4;
        }
    } else if (s->size < 0x1C) {
        s->status = 0;
    } else {
        u32 tag = StrHash("[PWERUP]", 0);

        memcpy(s->ptr, &tag, 4);
        s->avail += 4;
        s->size -= 4;
        s->ptr += 4;
        memcpy(s->ptr, data, 0xC);
        s->avail += 0xC;
        s->size -= 0xC;
        s->ptr += 0xC;
        memcpy(s->ptr, data + 0xC, 0xC);
        s->avail += 0xC;
        s->size -= 0xC;
        s->ptr += 0xC;
    }
}

int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg);  // 0x800E4C64
void ActorUpdate(Actor* actor, float dt);                         // 0x800E48A0
int ActorIsActive(Actor* actor);                                  // 0x80022D80
void ActorRender(Actor* actor);                                   // 0x800C7CE8
int AssetFind(u32 table, u32 key);                                // 0x8007EB34
extern "C" Actor* fn_800C7E80(void* desc);                        // 0x800C7E80
extern "C" void fn_8016F120(u32 a, void* b);                      // 0x8016F120

// Payload for the 0x400/0x409/0x40A zone messages.
struct ZoneSaveMsg {
    Actor* actor;  // 0x00
    u32 target;    // 0x04
    u32 unk08;     // 0x08
    u32 unk0C;     // 0x0C
    float unk10;   // 0x10
    float unk14;   // 0x14
    int unk18;     // 0x18
    Actor* self;   // 0x1C
};

// Argument block handed to the linked object's 0x1501 handler.
struct ZoneParamInfo {
    void* target;  // 0x00
    u32 unk04;     // 0x04 (left unset)
    u32 data[6];   // 0x08 (power-up record copy)
    float pos[3];  // 0x20 (actor position)
};

// Object linked through ZoneSaveMsg::target; slot 0x30 is its 0x1501 handler.
struct ZoneMsgTarget {
    char pad0[0x30];
    int (*handle)(int cmd, ZoneParamInfo* info);  // 0x30
};

// Spawn descriptor handed to fn_800C7E80; the first three words are an XYZ position.
struct ZoneSpawnDesc {
    float x;       // 0x00
    float y;       // 0x04
    float z;       // 0x08
    u32 pad0C[4];
    int unk1C;     // 0x1C
    void* param;   // 0x20
    u32 pad24;     // 0x24
};

void ZonePowerUpUpdate(Actor* actor, float dt) {
    ZonePowerUp* p = (ZonePowerUp*)actor->powerUp;

    if (p != 0) {
        if (p->active != 0) {
            p->timer -= dt;
            if (p->timer <= 0.0f) {
                int msg[2];

                msg[1] = 1;
                SendMsg(actor->id, 0xC00, msg);
                p->active = 0;
            }
        }
        ActorUpdate(actor, dt);
    }
}

int ZonePowerUpMsg(Actor* actor, int msg, void* data, int arg) {
    ZoneSaveMsg* m = (ZoneSaveMsg*)data;

    if (msg == 0x409) {
        Actor* a = m->actor;
        ZoneMsgTarget* target = (ZoneMsgTarget*)m->target;

        if (a == 0 || target == 0 || a->powerUp == 0) {
            return 0;
        } else {
            ZonePowerUp* p = (ZonePowerUp*)a->powerUp;
            ZoneParamInfo info;

            info.target = target;
            info.data[0] = p->data[0];
            info.data[1] = p->data[1];
            info.data[2] = p->data[2];
            info.data[3] = p->data[3];
            info.data[4] = p->data[4];
            info.data[5] = p->data[5];
            info.pos[0] = (float)a->x;
            info.pos[1] = (float)a->y;
            info.pos[2] = (float)a->z;
            if (target->handle(0x1501, &info) != 0) {
                fn_8016F120(a->info->field_AC, &a->x);
                ActorRender(a);
            }
            return 1;
        }
    } else if (msg < 0x409) {
        if (msg == 0x400) {
            Actor* a = m->actor;

            m->target = a->id;
            m->unk0C = 1;
            m->unk10 = ActorIsActive(a) ? 1.0f : 0.0f;
            m->unk14 = 0.0f;
            m->self = a;
            m->unk08 = 0;
            m->unk18 = 3;
            return 1;
        }
    } else if (msg < 0x40B) {
        if (m->unk08 == 0x7C44EC3D) {
            int buf[4];

            buf[1] = 0;
            buf[3] = -1;
            m->unk0C = -0xDEA7E22;
            buf[2] = m->unk0C;
            buf[3] = AssetFind(0xC0A77187, 0x7EC7F65C);
            SendMsg(m->target, 0x40B, buf);
        }
        return 1;
    }
    return ActorDefaultMsg(actor, msg, data, arg);
}

bool ZoneSpawnActorMsg(Actor* actor, int msg, void* data) {
    ZoneSpawnDesc desc;
    int* d;
    Actor* spawned;
    bool ok;

    if (msg == 0x1500) {
        d = (int*)data;
        desc.unk1C = -1;
        desc.x = ((float*)d)[8];
        desc.y = ((float*)d)[9];
        desc.z = ((float*)d)[10];
        desc.param = (void*)d[1];
        spawned = fn_800C7E80(&desc);
        ok = spawned != 0;
        if (ok) {
            spawned->type = &lbl_8039B9F0;
        }
        return ok;
    }
    return false;
}

// Releases the per-actor 0x28-byte state record and clears the slot.
void ZoneFreePowerUp(Actor* actor) {
    VFree(actor->powerUp);
    actor->powerUp = 0;
}

// Snapshot of the linked object's placement into the 0x28-byte state record,
// allocating the record on first use.
int ZoneRefreshPowerUp(Actor* actor) {
    ZoneState* state;

    if (!ActorInitBase(actor)) {
        return 0;
    }
    state = (ZoneState*)actor->powerUp;
    if (state == 0) {
        state = (ZoneState*)VAlloc(0x28);
        if (state == 0) {
            return 0;
        }
        actor->powerUp = (u8*)state;
    }
    state->posX = (int)actor->info->posX;
    state->unk00 = actor->info->unk18;
    state->posY = (int)actor->info->posY;
    state->unk04 = actor->info->unk1C;
    state->posZ = (int)actor->info->posZ;
    state->unk08 = actor->info->unk20;
    if (actor->info->unk24 == 0x6C345908) {
        float base = lbl_804D7018;

        state->active = 1;
        state->timer = base * actor->info->scale;
    }
    return 1;
}

// Unlinks the node with the given key and returns it to the free list.
int ZoneUnlinkNode(ZoneList* list, u32 key) {
    ZoneListNode** link = &list->head;
    ZoneListNode* node = list->head;

    while (node != 0) {
        if (node->key == key) {
            *link = node->next;
            memset(node, 0, 0x10);
            node->next = list->free;
            list->free = node;
            return 1;
        }
        node = node->next;
        link = &(*link)->next;
    }
    return 0;
}

// Callback used when walking a zone's node list; returning 0 stops the walk.
typedef int (*ZoneNodeCallback)(u32 key, u32 value, float f, int param);

// Walks the list and stops at the first node whose callback returns 0. Always reports success.
int ZoneWalkNodes(ZoneList* list, ZoneNodeCallback cb, int param) {
    ZoneListNode* node = list->head;

    while (node != 0) {
        if (cb(node->unk08, node->key, node->unk04, param) == 0) {
            return 1;
        }
        node = node->next;
    }
    return 1;
}

// Integer position used as the centre of a zone search.
struct IVec3 {
    int x;
    int y;
    int z;
};

int ZoneHitListAdd(Actor* actor, ZoneList* list, float value);
int ZoneHitListAddRaw(Actor* actor, ZoneList* list, float value);

// Callbacks stored in the per-kind table. The setter installs them as
// plain pointers; the update walk calls whichever entry the node selects.
typedef int (*ZoneSetFn)(int kind, Actor* a, Actor* b);
typedef int (*ZoneWalkFn)(int kind, u32 key, float value, Actor* owner);

// One slot of the three-entry table at 0x2C.
union ZoneEntry {
    int value;
    ZoneSetFn set;
    ZoneWalkFn walk;
};

// A zone: the node pool (same head/free record as ZoneList), the current
// search target, and the per-kind callbacks with their argument.
struct Zone {
    u8* buf;               // 0x00
    ZoneListNode* head;    // 0x04
    ZoneListNode* free;    // 0x08
    int count;             // 0x0C
    Actor* actor;          // 0x10
    int mode;              // 0x14
    float radius;          // 0x18
    u32 mask;              // 0x1C
    IVec3 target;          // 0x20
    ZoneEntry entries[3];  // 0x2C
    Actor* param;          // 0x38
};

// Spatial queries that call back for every actor overlapping the target.
extern "C" int fn_8001ED98(IVec3* target, int radius,
                           int (*cb)(Actor*, ZoneList*, float), void* zone, u32* mask);
extern "C" int fn_8001EAF8(IVec3* target, int radius,
                           int (*cb)(Actor*, ZoneList*, float), void* zone, u32* mask);

// Updates the search centre, recycles the nodes that were already visited by
// the previous update, refreshes the hit list and then walks it, calling the
// callback each node selects. A callback that returns 0 aborts the walk.
int ZoneUpdate(void* zone, IVec3* pos) {
    Zone* z = (Zone*)zone;
    ZoneListNode** link;
    ZoneListNode* node;

    if (pos != 0) {
        z->target.x = pos->x;
        z->target.y = pos->y;
        z->target.z = pos->z;
    }
    link = &z->head;
    node = z->head;
    while (node != 0) {
        if (node->unk08 == 2) {
            *link = node->next;
            memset(node, 0, 0x10);
            node->next = z->free;
            z->free = node;
            node = *link;
        } else {
            node->unk08 = 2;
            link = &node->next;
            node = node->next;
        }
    }
    switch (z->mode) {
    case 0:
        fn_8001ED98(&z->target, (int)z->radius, ZoneHitListAddRaw, z, &z->mask);
        break;
    case 1:
        fn_8001ED98(&z->target, (int)z->radius, ZoneHitListAdd, z, &z->mask);
        break;
    case 2:
        fn_8001EAF8(&z->target, (int)z->radius, ZoneHitListAddRaw, z, &z->mask);
        break;
    case 3:
        fn_8001EAF8(&z->target, (int)z->radius, ZoneHitListAdd, z, &z->mask);
        break;
    }
    node = z->head;
    while (node != 0) {
        ZoneListNode* next = node->next;
        ZoneEntry* entry = &z->entries[node->unk08];

        if (entry->walk != 0 && entry->walk(node->unk08, node->key, node->unk04, z->param) == 0) {
            break;
        }
        node = next;
    }
    return 1;
}

// Small accessors for two slots of the object the zone callbacks are attached to.
struct ZoneObject {
    char pad0[0x14];
    u32 slot14;    // 0x14
    float slot18;  // 0x18
};

// Stores a float slot and returns the previous value.
float ZoneObjectSetSlot18(ZoneObject* obj, float value) {
    float old = obj->slot18;

    obj->slot18 = value;
    return old;
}

// Stores an integer slot and returns the previous value.
u32 ZoneObjectSetSlot14(ZoneObject* obj, u32 value) {
    u32 old = obj->slot14;

    obj->slot14 = value;
    return old;
}

// Installs the per-kind callback set: the two plain values and their owner.
int ZoneSetCallback(void* zone, int a, ZoneSetFn fn, int b, Actor* c) {
    Zone* z = (Zone*)zone;

    z->entries[0].value = a;
    z->entries[1].set = fn;
    z->entries[2].value = b;
    z->param = c;
    return 1;
}

void BitClear(u32* bits, int bit);

// Sets, clears or wipes the flag word at 0x1C. Flags 0..26 are valid; setting
// or clearing uses the flags selected by the index, 0 wipes the whole word.
int ZoneFlagSet(ZoneList* list, int flag, int on) {
    if (list == 0 || flag >= 27) {
        return 0;
    }
    if (flag <= 0) {
        if (on != 0) {
            memset(list->mask, 0xFF, 4);
        } else {
            memset(list->mask, 0, 4);
        }
    } else if (on != 0) {
        list->mask[flag / 32] |= 1 << (flag & 31);
    } else {
        BitClear(list->mask, flag);
    }
    return 1;
}

// Clears the node pool and threads every node onto the free list, newest
// block first. The used list starts empty.
int ZoneListInit(ZoneList* list) {
    ZoneListNode* node;
    int i;

    memset(list->buf, 0, list->count << 4);
    list->head = 0;
    list->free = 0;
    for (i = 0; i < list->count; i++) {
        node = (ZoneListNode*)(list->buf + i * 0x10);
        node->next = list->free;
        list->free = node;
    }
    return 1;
}

// Initialises a zone: clears it, records the actor, the search mode and
// radius, copies the search target, then clears the node pool and threads
// every node onto the free list.
int ZoneInit(void* zone, Actor* actor, IVec3* pos, void* buf, int count, float radius) {
    Zone* z = (Zone*)zone;
    ZoneListNode* node;
    int i;

    memset(z, 0, 0x3C);
    z->actor = actor;
    z->mode = 2;
    z->radius = radius;
    z->buf = (u8*)buf;
    z->count = count;
    z->target.x = pos->x;
    z->target.y = pos->y;
    z->target.z = pos->z;
    memset(z->buf, 0, z->count << 4);
    z->head = 0;
    z->free = 0;
    for (i = 0; i < z->count; i++) {
        node = (ZoneListNode*)(z->buf + i * 0x10);
        node->next = z->free;
        z->free = node;
    }
    return 1;
}

struct Mtx34 {
    float m[3][4];
};

// Six-word snapshot of the actor's collision source.
struct ZoneHitBox {
    u32 data[6];
};

void ActorGetMatrix(Mtx34* mtx, Actor* actor);
extern "C" void fn_8016D864(ZoneHitBox* out, void* src);
int ModelBoneHitBox(int a, int b, int c, int d);

// Records the actor in the zone's hit list with the given value if its model
// overlaps the zone target and its class bit has not been consumed yet.
int ZoneHitListAdd(Actor* actor, ZoneList* list, float value) {
    Mtx34 actorMtx;
    Mtx34 targetMtx;
    ZoneHitBox actorBox;
    ZoneHitBox targetBox;
    ZoneListNode* node;
    int kind;

    if (actor == list->actor && list->actor != 0) {
        return 1;
    }
    ActorGetMatrix(&actorMtx, actor);
    ActorGetMatrix(&targetMtx, list->actor);
    fn_8016D864(&actorBox, actor->unk40);
    fn_8016D864(&targetBox, list->actor->unk40);
    if (ModelBoneHitBox((int)&actorBox, (int)&actorMtx, (int)&targetBox, (int)&targetMtx) == 0) {
        return 1;
    }
    kind = actor->unk38 >> 24;
    if (kind >= 0x1B) {
        return 1;
    }
    if ((list->mask[kind / 32] & (1 << (kind & 31))) != 0) {
        return 1;
    }
    for (node = list->head; node != 0; node = node->next) {
        if (node->key == (u32)actor) {
            node->unk04 = value;
            node->unk08 = 1;
            return 1;
        }
    }
    node = list->free;
    if (node != 0) {
        list->free = node->next;
        node->key = (u32)actor;
        node->unk04 = value;
        node->unk08 = 0;
        node->next = list->head;
        list->head = node;
    }
    return 1;
}

// Adds the actor to the zone's hit list with the given value. Unlike
// ZoneHitListAdd this does no overlap test; the caller has already done it.
int ZoneHitListAddRaw(Actor* actor, ZoneList* list, float value) {
    ZoneListNode* node;
    int kind;

    if (actor != list->actor || list->actor == 0) {
        kind = actor->unk38 >> 24;
        if (kind < 0x1B && (list->mask[kind / 32] & (1 << (kind & 31))) == 0) {
            for (node = list->head; node != 0; node = node->next) {
                if (node->key == (u32)actor) {
                    node->unk04 = value;
                    node->unk08 = 1;
                    return 1;
                }
            }
            node = list->free;
            if (node != 0) {
                list->free = node->next;
                node->key = (u32)actor;
                node->unk04 = value;
                node->unk08 = 0;
                node->next = list->head;
                list->head = node;
            }
        }
    }
    return 1;
}

extern float lbl_804D2BC8;
extern float lbl_804D2BC4;

// Accessor for the .sdata tuning value at 0x804D2BC8.
float ZoneGetUnkC8(void) {
    return lbl_804D2BC8;
}

// Accessor for the .sdata tuning value at 0x804D2BC4.
float ZoneGetUnkC4(void) {
    return lbl_804D2BC4;
}

// Sub-object of the actor at 0x1D64 that the zone code drives. The first two
// fields alias Actor::unk1D64/unk1D68 and the node list starts at 0x70.
struct ZoneDash {
    Actor* linkedA;   // 0x00
    Actor* linkedB;   // 0x04
    u32 unk08;        // 0x08
    u32 unk0C;        // 0x0C
    int unk10;        // 0x10
    float unk14;      // 0x14
    float unk18;      // 0x18
    float unk1C;      // 0x1C
    float unk20;      // 0x20
    u32 unk24;        // 0x24
    u32 unk28;        // 0x28
    u32 unk2C;        // 0x2C
    int state;        // 0x30
    u32 unk34;        // 0x34
    u32 unk38;        // 0x38
    int anim;         // 0x3C
    u32 unk40;        // 0x40
    u32 unk44;        // 0x44
    u32 unk48;        // 0x48
    u32 unk4C;        // 0x4C
    int unk50;        // 0x50
    u32 unk54;        // 0x54
    u32 unk58;        // 0x58
    u32 unk5C;        // 0x5C
    u32 unk60;        // 0x60
    u32 unk64;        // 0x64
    u32 unk68;        // 0x68
    u32 unk6C;        // 0x6C
    ZoneList zone;    // 0x70
};

struct Mtx33 {
    float m[3][3];
};

struct PlaceF {
    Mtx33 rot;
    Vec3 pos;
};

// Matrix produced from a PlaceF; PlacementToMatrix stores fixed-point integers.
struct Mtx34i {
    int m[3][4];
};

// Line query handed to the spatial query helper: two integer endpoints and the
// same first endpoint as floats.
struct ZoneLineMsg {
    int pos[3];     // 0x00
    int end[3];     // 0x0C
    void* owner;    // 0x18
    float fpos[3];  // 0x1C
};

// Argument block of the 0x400 handler at slot 0x30 of a linked object.
struct ZoneRefInfo {
    void* target;   // 0x00
    u32 unk04;      // 0x04
    u32 unk08;      // 0x08
    int unk0C;      // 0x0C
    u32 unk10;      // 0x10
    u32 unk14;      // 0x14
    int unk18;      // 0x18
};

struct ZoneRefHandler {
    char pad0[0x30];
    int (*handle)(int cmd, ZoneRefInfo* info);  // 0x30
};

extern float lbl_804D7020;
extern float lbl_804D7024;
extern void* lbl_804D4948;

void MtxSetEuler(Mtx34* m, Vec3* rot);        // 0x8009FAC0
void PlacementToMatrix(Mtx34* m, PlaceF* p);  // 0x8009EF04
int PlayerInHost(Actor* actor);               // 0x80049750
void ActorPlayAnim(int id, u32 anim, int owner);  // 0x800C5654
int DashSendAnim(Actor* actor, int anim);     // 0x80266208

extern "C" void fn_8016D440(void* out, void* in);                     // 0x8016D440
extern "C" void fn_8009F63C(Mtx34* out, Mtx34* in);                   // 0x8009F63C
extern "C" void fn_8009F44C(int* out, const void* mtx, const int* in);  // 0x8009F44C
extern "C" int fn_8006CAB8(Actor* actor);                             // 0x8006CAB8
extern "C" int fn_8001F928(Actor* actor, int* data, u32 flags, void (*cb)(),
                           void** ctx);                               // 0x8001F928
extern "C" void fn_800F94C8(void* p);                                 // 0x800F94C8
extern "C" void* fn_800C6510(void);                                   // 0x800C6510
extern "C" void fn_800460CC(Actor* actor);                            // 0x800460CC
extern "C" void fn_800462C8(Actor* actor);                            // 0x800462C8
extern "C" int fn_8008E7D0(void);                                     // 0x8008E7D0
struct ZoneHitEntry;
struct ZoneDistQuery;
struct ZoneLineEntry;
struct ZoneLineQuery;

int ZoneActorFilter(u32* stored, u32 actor);
void ZoneLinkedUpdate(Actor* actor);
int ZoneLineFilter(ZoneLineEntry* entry, ZoneLineQuery* query);
int ZoneDistFilter(ZoneHitEntry* entry, ZoneDistQuery* query);

int ZoneDashNode(u32 unused, Actor* actor, float value, Actor* owner); // 0x800D2290

// Returns the actor the zone follows: the second linked object wins, otherwise
// the first is used when its class is one the zone understands (0x13 and 0x02).
Actor* ZoneGetLinked(Actor* actor) {
    if (actor->unk1D68 != 0) {
        return actor->unk1D68;
    }
    if (actor->unk1D64 != 0) {
        int kind = (u32)actor->unk1D64->unk38 >> 24;

        if (kind == 0x13 || kind == 2) {
            return actor->unk1D64;
        }
    }
    return 0;
}

// Resets the zone state block; only the first 0x70 bytes are touched.
void ZoneDashInit(ZoneDash* dash) {
    dash->linkedA = 0;
    dash->linkedB = 0;
    dash->unk14 = lbl_804D7020;
    dash->unk18 = lbl_804D7020;
    dash->unk1C = lbl_804D7020;
    dash->unk20 = lbl_804D7024;
    dash->unk10 = -1;
    dash->unk24 = 0;
    dash->unk28 = 0;
    dash->state = 0;
    dash->unk34 = 0;
    dash->anim = 0;
    dash->unk5C = 0;
    dash->unk60 = 0;
    dash->unk64 = 0;
    dash->unk68 = 0;
    dash->unk58 = 0;
    dash->unk2C = 0;
    dash->unk40 = 0;
    dash->unk4C = 0;
    dash->unk48 = 0;
    dash->unk6C = 0;
    dash->unk50 = -1;
}

// Per-frame update: resets the block, fires a line query along the actor's
// facing direction, walks the zone list and drives the dash animation state.
void ZoneDashUpdate(ZoneDash* dash, Actor* actor) {
    Vec3 euler;
    int vec[3];
    ZoneRefInfo info;
    ZoneLineMsg msg;
    Mtx34i mtxI;
    Mtx34 mtxF;
    PlaceF place;
    Actor* saved;
    Actor* linked;
    float keep;
    int id;

    if (dash->state != 0) {
        saved = ZoneGetLinked(actor);
    } else {
        saved = 0;
    }
    dash->linkedA = 0;
    dash->linkedB = 0;
    dash->unk14 = lbl_804D7020;
    dash->unk18 = lbl_804D7020;
    dash->unk1C = lbl_804D7020;
    dash->unk20 = lbl_804D7024;
    dash->unk10 = -1;
    dash->unk24 = 0;
    dash->unk28 = 0;
    dash->state = 0;
    dash->unk34 = 0;
    dash->anim = 0;
    dash->unk5C = 0;
    dash->unk60 = 0;
    dash->unk64 = 0;
    dash->unk68 = 0;
    dash->unk58 = 0;
    dash->unk2C = 0;
    dash->unk40 = 0;
    dash->unk4C = 0;
    dash->unk48 = 0;
    dash->unk6C = 0;
    dash->unk50 = -1;
    ZoneUpdate(&dash->zone, (IVec3*)&actor->x);

    fn_8016D440(&euler, &actor->place.rot);
    MtxSetEuler((Mtx34*)&place.rot, &euler);
    place.pos = actor->place.pos;
    PlacementToMatrix((Mtx34*)&mtxI, &place);
    fn_8009F63C(&mtxF, (Mtx34*)&mtxI);
    vec[0] = 0;
    vec[1] = 0x4000;
    vec[2] = 0;
    fn_8009F44C(vec, &mtxF, vec);
    msg.pos[0] = mtxI.m[2][0];
    msg.pos[1] = mtxI.m[2][1];
    msg.pos[2] = mtxI.m[2][2];
    msg.end[0] = msg.pos[0] + vec[0];
    msg.end[1] = msg.pos[1] + vec[1];
    msg.end[2] = msg.pos[2] + vec[2];
    msg.owner = dash;
    msg.fpos[0] = (float)msg.pos[0];
    msg.fpos[1] = (float)msg.pos[1];
    msg.fpos[2] = (float)msg.pos[2];
    if (PlayerInHost(actor) == 1 && fn_8006CAB8(actor) != 0) {
        lbl_804D4948 = &actor->unk1D64;
        fn_8001F928(actor, (int*)&msg, 0x16002800, (void (*)())ZoneLineFilter,
                    (void**)&msg.owner);
    }

    fn_800F94C8(actor->unk184);
    PlacementToMatrix((Mtx34*)&mtxI, (PlaceF*)fn_800C6510());
    fn_8009F63C(&mtxF, (Mtx34*)&mtxI);
    vec[0] = 0;
    vec[1] = 0x4000;
    vec[2] = 0;
    fn_8009F44C(vec, &mtxF, vec);
    msg.pos[0] = mtxI.m[2][0];
    msg.pos[1] = mtxI.m[2][1];
    msg.pos[2] = mtxI.m[2][2];
    msg.end[0] = msg.pos[0] + vec[0];
    msg.end[1] = msg.pos[1] + vec[1];
    msg.end[2] = msg.pos[2] + vec[2];
    msg.owner = dash;
    msg.fpos[0] = (float)msg.pos[0];
    msg.fpos[1] = (float)msg.pos[1];
    msg.fpos[2] = (float)msg.pos[2];
    lbl_804D4948 = &actor->unk1D68;
    keep = actor->unk1D84;
    actor->unk1D84 = lbl_804D7024;
    fn_8001F928(actor, (int*)&msg, 0x16002800, (void (*)())ZoneLineFilter,
                (void**)&msg.owner);

    ZoneWalkNodes(&dash->zone, (ZoneNodeCallback)ZoneDashNode, (int)actor);

    if (actor->unk1D64 != 0 && actor->unk1D64 == ZoneGetLinked(actor)) {
        actor->unk1D84 = keep;
    }
    linked = ZoneGetLinked(actor);
    if (linked != 0) {
        info.target = linked;
        if (((ZoneRefHandler*)linked)->handle(0x400, &info) != 0) {
            actor->unk1D88 = info.unk0C;
            actor->unk1D8C = info.unk18;
        }
    }
    ZoneLinkedUpdate(actor);

    if (dash->state != 0) {
        linked = ZoneGetLinked(actor);
        if (saved != linked) {
            if (saved != 0) {
                ActorPlayAnim(saved->unk38, 0x852DE19B, actor->unk38);
            }
            if (linked != 0) {
                ActorPlayAnim(linked->unk38, 0xDCC36A29, actor->unk38);
            }
        }
        if (linked != 0 && linked->unk38 != -1) {
            int buf[4];

            buf[1] = 0;
            buf[2] = actor->unk38;
            buf[3] = 0xC36DA63E;
            SendMsg(linked->unk38, 0xC14, buf);
        }
    } else if (saved != 0) {
        ActorPlayAnim(saved->unk38, 0x852DE19B, actor->unk38);
    }

    if (dash->state != 0 && dash->anim != 0) {
        fn_800460CC(actor);
        id = DashSendAnim(actor, dash->anim);
        if (fn_8008E7D0() == 0 && id != -1) {
            dash->state = 0;
        }
    } else {
        fn_800462C8(actor);
    }
}

// Data slots and tuning values used by the dash zone callback.
extern int lbl_804D2BC0;      // 0x804D2BC0: asset id resolved once at startup
extern double lbl_804D7028;   // 0x804D7028: 2^52, the unsigned-int conversion bias
extern float lbl_804D7030;    // 0x804D7030: zone radius
extern float lbl_804D7034;
extern float lbl_804D7038;
extern float lbl_804D703C;
extern float lbl_804D7040;
extern float lbl_804D7044;
extern float lbl_804D7048;    // minimum focus range

int ModelFindBone(int model, int bone);           // 0x80018AD4
void ActorAim(int* out, Actor* actor, int flag);  // 0x8009F700
extern "C" PlaceF* fn_800C6448(void);             // 0x800C6448
extern "C" void fn_800E0CB8(void* out, const void* in);  // 0x800E0CB8
extern "C" float fn_800E797C(Actor* actor, int target);  // 0x800E797C
extern "C" float fn_800E7D38(Actor* actor, int target);  // 0x800E7D38
extern "C" float fn_800A1418(float value);               // 0x800A1418
extern "C" int fn_800F7C64(const char* text, u32 hash);  // 0x800F7C64
extern "C" int fn_800FC3B8(int arg);                     // 0x800FC3B8
extern "C" int fn_802A1D04(void);                        // 0x802A1D04
extern "C" int fn_800A7B78(void* p, int mode);    // 0x800A7B78
extern "C" void fn_800B1AB8(Actor* actor, int a, int b, int c, int d, float e);  // 0x800B1AB8

// Resolves the asset every zone queries for bone positions, once.
void ZoneLoadAsset(void) {
    lbl_804D2BC0 = AssetFind(0xF1754BF2, 0x72C27236);
}

// Wires the actor's zone to its own 0x80-node pool and clears the two slots
// that hold the last hit distance / actor.
void ZoneActorSetup(Actor* actor) {
    actor->unk1DA8 = 0;
    actor->unk1DB8 = -1;
    ZoneInit(&actor->zoneList, actor, (IVec3*)&actor->x, actor->zoneBuf, 0x80, lbl_804D7030);
}

// --- types used by the dash zone callback (0x800D2290) ---

// Minimal String/Buffer pair from the shared engine headers. The 0x41E query
// below only reads the first word of the String, but its constructor and
// destructor calls belong to the message sequence.
class Buffer {
public:
    Buffer();
    virtual ~Buffer();                    // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                             // 0x8028C500
    ~String() {}
    virtual void Release(char* p);
};

struct QueryResult {
    String name;
};

// 0x41E "name registered under this hash" payload.
struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

// Asks `target` for the name stored under `hash`; returns SendMsg's result and
// hands the name back through `out`.
static inline int ZoneQueryName(int target, u32 hash, int* out) {
    QueryMsg q;
    int ret;

    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    ret = SendMsg(target, 0x41E, &q);
    *out = *(int*)&q.result;
    return ret;
}

// Reply to the 0x400 query: the actor and the id whose top byte is its class.
struct ZoneKindReply {
    Actor* actor;     // 0x00
    u32 id;           // 0x04
    int unk08;        // 0x08
    int unk0C;        // 0x0C (saved to the owner's 0x1D88)
};

// 0x402 query: world position of a bone; the handler fills the position.
struct ZoneBoneQuery {
    int unk00;        // 0x00
    short bone;       // 0x04
    char pad06[0x2E];
    int pos[3];       // 0x34
};

// 0x40A "is this actor visible" query.
struct ZoneVisQuery {
    Actor* actor;     // 0x00
    int target;       // 0x04
    u32 hash;         // 0x08
    void* result;     // 0x0C
};

// Placement matrix as PlacementToMatrix stores it: fixed-point rotation
// followed by the translation.
struct ZonePlaceMtx {
    int rot[9];       // 0x00
    int pos[3];       // 0x24
};

// Per-node callback of the dash zone: finds the bone the zone tracks, aims the
// zone at the actor and runs the visibility / focus message chain. Always
// returns 1 so the walk over the zone's node list continues.
int ZoneDashNode(u32 unused, Actor* actor, float value, Actor* owner) {
    ZoneKindReply reply;
    ZoneBoneQuery boneQuery;
    ZoneVisQuery visQuery;
    ZonePlaceMtx mtx;
    PlaceF* place;
    IVec3 zonePos;
    IVec3 actorPos;
    IVec3 delta;
    IVec3 aim;
    int bone;
    int kind;
    int dist;
    int playerDist;
    int visible;
    int queried;
    int result;
    int ownerName;
    int actorName;
    float dx;
    float dy;
    float dz;

    (void)unused;
    queried = 0;
    reply.actor = actor;
    actor->handle(0x400, &reply);
    kind = reply.id >> 24;
    switch (kind) {
    case 1:
        if (actor->info != 0 && actor->info->unkC0 == -1) {
            return 1;
        }
        bone = ModelFindBone(actor->unk90, lbl_804D2BC0);
        break;
    case 2:
        if (PlayerInHost(actor) == 0) {
            return 1;
        }
        bone = ModelFindBone(actor->unk54, lbl_804D2BC0);
        break;
    case 10:
        bone = -1;
        break;
    case 0x13:
        if (actor->unk128 == 0x276198) {
            return 1;
        }
        bone = ModelFindBone(actor->unkCC, lbl_804D2BC0);
        break;
    default:
        return 1;
    }

    place = fn_800C6448();
    PlacementToMatrix((Mtx34*)&mtx, place);
    zonePos.x = mtx.pos[0];
    zonePos.y = mtx.pos[1];
    zonePos.z = mtx.pos[2];
    if (bone == -1) {
        actorPos.x = actor->x;
        actorPos.y = actor->y;
        actorPos.z = actor->z;
    } else {
        boneQuery.bone = (short)bone;
        SendMsg(actor->unk38, 0x402, &boneQuery);
        actorPos.x = boneQuery.pos[0];
        actorPos.y = boneQuery.pos[1];
        actorPos.z = boneQuery.pos[2];
    }
    if (bone == -1) {
        delta.x = actor->x - zonePos.x;
        delta.y = actor->y - zonePos.y;
        delta.z = actor->z - zonePos.z;
    } else {
        boneQuery.bone = (short)bone;
        SendMsg(actor->unk38, 0x402, &boneQuery);
        delta.x = boneQuery.pos[0] - zonePos.x;
        delta.y = boneQuery.pos[1] - zonePos.y;
        delta.z = boneQuery.pos[2] - zonePos.z;
    }
    fn_800E0CB8(&delta, &delta);
    ActorAim((int*)&aim, (Actor*)&mtx, 1);
    dist = (aim.x * delta.x + aim.y * delta.y + aim.z * delta.z) / 4096;

    if (owner->unk1D68 == 0 && owner->unk1DD0 == 0) {
        int isPlayer = PlayerInHost(owner);

        visQuery.actor = actor;
        visQuery.result = 0;
        visQuery.target = owner->unk38;
        if (isPlayer == 0) {
            visQuery.hash = 0x3CA34BD6;
        } else {
            visQuery.hash = 0x7C44EC3D;
        }
        if (visQuery.hash == 0x7C44EC3D && (owner->unk7B4 & 0x80000) != 0 &&
            (owner->unk7B4 & 0x2000000) == 0) {
            visible = 0;
        } else {
            visible = actor->handle(0x40A, &visQuery);
        }
        if (visible != 0) {
            if (visQuery.result == 0) {
                visible = 0;
            } else {
                playerDist = (int)fn_800E797C(owner, (int)visQuery.result);
            }
        }
        if (visible == 0 && kind != 2 && kind != 0x13) {
            return 1;
        }
        if (visible != 0 && playerDist < dist && owner->unk1D90 < dist &&
            actor != owner->unk2794 && actor != owner->unk26E4) {
            result = fn_8001F928(owner, (int*)&zonePos, 0x12000000,
                                 (void (*)())ZoneActorFilter, (void**)reply.actor);
            queried = 1;
            if (result == 0) {
                if (kind == 0x13) {
                    if (*actor->unkC4 == 0x39FB0DAD) {
                        return 1;
                    }
                    owner->unk1DC0 = 1;
                } else if (kind == 2) {
                    owner->unk1DC4 = 1;
                }
                owner->unk1DA4 = 1;
                owner->unk1D90 = dist;
                owner->unk1D68 = actor;
                owner->unk1D88 = reply.unk0C;
                dy = (float)actor->y - place->pos.y;
                dx = (float)actor->x - place->pos.x;
                dz = (float)actor->z - place->pos.z;
                owner->unk1D84 = dy * dy + dx * dx + dz * dz;
            }
        }
    }

    if (PlayerInHost(owner) == 0) {
        return 1;
    }
    if (ZoneQueryName(owner->unk38, 0x273A5D, &ownerName) != 0) {
        if (ZoneQueryName(actor->unk38, 0x273A5D, &actorName) != 0) {
            if (ownerName == actorName) {
                return 1;
            }
        }
    }

    if ((kind == 0x13 || kind == 2) && visQuery.result != 0) {
        int limit = (int)(lbl_804D7034 - lbl_804D7034 * (lbl_804D2BC8 / lbl_804D7038));

        if (dist > limit && dist > owner->unk1DB0 &&
            actor != owner->unk2794 && actor != owner->unk26E4) {
            if (queried == 0) {
                result = fn_8001F928(owner, (int*)&zonePos, 0x12000000,
                                     (void (*)())ZoneActorFilter, (void**)reply.actor);
            }
            if (result == 0) {
                owner->unk1DB0 = dist;
                owner->unk1DAC = actor;
            }
        }
    }

    if (value < lbl_804D703C && kind == 0x13 && visQuery.result != 0 &&
        actorName != 0 && PlayerInHost(owner) != 0) {
        if (fn_800A7B78(actor->unk270, 5) == 0) {
            actor->unk17C = 2;
            actor->unk174 = lbl_804D7040;
            actor->unk184 = (void*)owner->unk38;
            actor->unk16C = 0;
            actor->unk16C |= 2;
            actor->unk16C |= 1;
            actor->unk178 = 1;
            fn_800B1AB8(actor, 1, 1, 0, owner->unk38, lbl_804D7044);
        }
    }
    return 1;
}

// Accepts the candidate when it is not already the actor stored at `stored`.
int ZoneActorFilter(u32* stored, u32 actor) {
    if (*stored != actor || *stored == 0) {
        return 1;
    }
    return 0;
}

// Spatial-query entry holding the candidate actor and its fixed-point position.
struct ZoneHitEntry {
    Actor* actor;       // 0x00
    char pad04[0x2C];   // 0x04
    int x;              // 0x30
    int y;              // 0x34
    int z;              // 0x38
};

// Object that receives the squared distance from the query point.
struct ZoneDistTarget {
    char pad0[0x20];
    float dist;         // 0x20
};

// Query description: the object to fill and the point to measure from.
struct ZoneDistQuery {
    ZoneDistTarget* target;  // 0x00
    float x;                 // 0x04
    float y;                 // 0x08
    float z;                 // 0x0C
    Actor* owner;            // 0x10
};

// Distance filter of the spatial query: stores the squared distance between the
// entry's fixed-point position and the query point, and rejects null entries.
int ZoneDistFilter(ZoneHitEntry* entry, ZoneDistQuery* query) {
    if (entry->actor != 0) {
        float dx = (float)entry->x - query->x;
        float dy = (float)entry->y - query->y;
        float dz = (float)entry->z - query->z;

        query->target->dist = dx * dx + dy * dy + dz * dz;
        return 0;
    }
    return 1;
}

// Segment handed to the zone walk as its data argument: the zone position and
// the position of the actor it tracks, both in fixed point.
struct ZoneSeg {
    IVec3 from;  // 0x00
    IVec3 to;    // 0x0C
};

// Actor-id table the zone owner stores at 0x1288; the tracked actor must not be
// one of these entries before the name checks run.
struct ZoneOwnerIds {
    char pad00[0x4];
    int unk04;           // 0x04
    char pad08[0x44];
    int unk4C;           // 0x4C
    char pad50[0x2C];
    int unk7C;           // 0x7C
    char pad80[0x14];
    int unk94;           // 0x94
};

// 0x6705 reply: the second word points at the returned name.
struct ZoneNameMsg {
    int unk00;           // 0x00
    const char* name;    // 0x04
};

// Per-frame update of the actor the zone is linked to (owner->unk1D68): checks
// visibility, the current hit distance and the name-based exclusions, then
// remembers the accepted target.
void ZoneLinkedUpdate(Actor* owner) {
    ZoneOwnerIds* ids;
    ZoneVisQuery vis;
    Actor* target;
    int isPlayer;
    int id;
    int name;
    float limit;
    float focus;
    float scale;
    float dist;
    float ratio;
    float distSq;

    target = owner->unk1D68;
    if (target == 0) {
        return;
    }
    isPlayer = PlayerInHost(owner);
    vis.actor = target;
    vis.result = 0;
    vis.target = (int)owner->unk38;
    if (isPlayer == 0) {
        vis.hash = 0x3CA34BD6;
    } else {
        vis.hash = 0x7C44EC3D;
    }
    owner->unk1DB4 = -1;
    owner->unk1DA0 = 0;
    if (vis.hash == 0x7C44EC3D && (owner->unk7B4 & 0x80000) != 0 &&
        (owner->unk7B4 & 0x2000000) == 0) {
        owner->unk1D94 = 0;
    } else {
        owner->unk1D94 = target->handle(0x40A, &vis);
    }
    if (owner->unk1D94 == 0) {
        return;
    }
    if (vis.result != 0) {
        owner->unk1D9C = (int)vis.result;
    }
    if (owner->unk1DB4 == -1 && owner->unk1DA0 == 0 && vis.result == 0) {
        owner->unk1D94 = 0;
        return;
    }
    if (owner->unk1DA4 == 1) {
        limit = (float)owner->unk1D90;
        if (limit < fn_800E797C(owner, owner->unk1D9C)) {
            owner->unk1D94 = 0;
            return;
        }
    }
    focus = fn_800A1418(owner->unk1F0);
    scale = lbl_804D7048;
    if (scale < focus) {
        scale = focus;
    }
    dist = fn_800E7D38(owner, owner->unk1D9C);
    ratio = dist / scale;
    distSq = ratio * ratio;
    if (owner->unk1DA4 == 1) {
        ZonePlaceMtx mtx;
        ZoneSeg seg;
        ZoneKindReply reply;
        ZoneDistQuery query;
        ZoneBoneQuery boneQuery;
        PlaceF* place;
        int bone;
        int kind;

        place = fn_800C6448();
        PlacementToMatrix((Mtx34*)&mtx, place);
        query.target = (ZoneDistTarget*)&owner->unk1D64;
        query.x = (float)mtx.pos[0];
        query.y = (float)mtx.pos[1];
        query.z = (float)mtx.pos[2];
        query.owner = owner;
        reply.actor = target;
        target->handle(0x400, &reply);
        kind = reply.id >> 24;
        switch (kind) {
        case 1:
            bone = ModelFindBone(target->unk90, lbl_804D2BC0);
            break;
        case 2:
            bone = ModelFindBone(target->unk54, lbl_804D2BC0);
            break;
        case 0x13:
            bone = ModelFindBone(target->unkCC, lbl_804D2BC0);
            break;
        default:
            bone = 1;
            break;
        }
        if (bone == -1) {
            seg.from.x = mtx.pos[0];
            seg.from.y = mtx.pos[1];
            seg.from.z = mtx.pos[2];
            seg.to.x = target->x;
            seg.to.y = target->y;
            seg.to.z = target->z;
        } else {
            boneQuery.bone = (short)bone;
            SendMsg(target->unk38, 0x402, &boneQuery);
            seg.from.x = mtx.pos[0];
            seg.from.y = mtx.pos[1];
            seg.from.z = mtx.pos[2];
            seg.to.x = boneQuery.pos[0];
            seg.to.y = boneQuery.pos[1];
            seg.to.z = boneQuery.pos[2];
        }
        fn_8001F928(owner, (int*)&seg, 0x12000000, (void (*)())ZoneDistFilter,
                    (void**)&query);
    }
    if (owner->unk1D84 > distSq) {
        owner->unk1D94 = 0;
        owner->unk1D98 = 1;
        return;
    }
    ids = (ZoneOwnerIds*)owner->unk1288;
    id = owner->unk1D9C;
    if (id == ids->unk4C || id == ids->unk04 || id == ids->unk7C ||
        id == ids->unk94) {
        if (owner->unk2798 != -1 || lbl_804D7020 != owner->unk2648) {
            owner->unk1D94 = 0;
            return;
        }
    }
    if (id == ids->unk4C || id == ids->unk94) {
        if (fn_802A1D04() != 3) {
            ZoneNameMsg msg;

            ZoneQueryName(owner->unk38, 0x2DA8293C, &name);
            if (fn_800F7C64((const char*)name, 0x1F5A5ED3) == 0 &&
                fn_800FC3B8(0x11) == 0) {
                owner->unk1D94 = 0;
                return;
            }
            SendMsg(target->unk38, 0x6705, &msg);
            if (fn_800F7C64(msg.name, 0xDC95C315) != 0) {
                owner->unk1D94 = 0;
                return;
            }
        }
    }
    owner->unk1DB8 = owner->unk1DB4;
}

int ModelBoneName(int model, int bone);            // 0x80018834
int ModelBoneCount(int model);                     // 0x80018B88
u32 fn_80037230(int model, int bone);              // 0x80037230
unsigned short* fn_80037B6C(int model);            // 0x80037B6C

// Candidate handed to the line query: the actor, the flags that select which
// bone the line must hit, and the two integer endpoints of the segment.
struct ZoneLineEntry {
    Actor* actor;       // 0x00
    char pad04[0x8];    // 0x04
    u32 flags;          // 0x0C
    char pad10[0x4];    // 0x10
    int bone;           // 0x14
    char pad18[0x4];    // 0x18
    int model;          // 0x1C
    char pad20[0x10];   // 0x20
    int x;              // 0x30
    int y;              // 0x34
    int z;              // 0x38
};

// Record the query fills with the closest hit found so far; the distance slot
// doubles as the current limit, so only a closer candidate replaces the record.
struct ZoneLineHit {
    char pad00[0xC];    // 0x00
    int model;          // 0x0C
    int bone;           // 0x10
    float x;            // 0x14
    float y;            // 0x18
    float z;            // 0x1C
    float dist;         // 0x20
    int unk24;          // 0x24
    char pad28[0x34];   // 0x28
    int kind13;         // 0x5C
    int kind2;          // 0x60
    int overlap;        // 0x64
    char pad68[0x4];    // 0x68
    int invalid;        // 0x6C
};

// Description of the line query handed to the spatial walk.
struct ZoneLineQuery {
    ZoneLineHit* hit;   // 0x00
    float x;            // 0x04
    float y;            // 0x08
    float z;            // 0x0C
    Actor* owner;       // 0x10
};

// 0x400 reply, filled for the query owner and for the candidate alike; bit 11
// of `mask` tags the bones the line may hit.
struct ZoneLineReply {
    Actor* actor;       // 0x00
    u32 id;             // 0x04
    u32 unk08;          // 0x08
    int unk0C;          // 0x0C
    u32 unk10;          // 0x10
    u32 unk14;          // 0x14
    u32 mask;           // 0x18
    u32 unk1C;          // 0x1C
};

// Scans the model's bone list for one tagged with bit 11 of the query flags.
static inline int ZoneBoneFlagged(int model, unsigned short* bones, int count) {
    int i;

    for (i = 0; i < count; i++) {
        if ((fn_80037230(model, bones[i]) & 0x800) != 0) {
            return 1;
        }
    }
    return 0;
}

// Line-of-sight filter of the zone queries: rejects candidates the owner cannot
// see, then keeps the closest one whose segment reaches the tracked bone.
int ZoneLineFilter(ZoneLineEntry* entry, ZoneLineQuery* query) {
    ZoneLineReply actorReply;
    ZoneLineReply ownerReply;
    ZoneLineReply reply;
    Actor* actor;
    unsigned short* bones;
    int count;
    int bone;
    int found;
    int i;
    int model;
    float dy;
    float dx;
    float dz;
    float dist;
    Actor* owner;

    owner = query->owner;
    if (PlayerInHost(owner)) {
        if ((int)(owner->unk26E4->unk38 >> 24) == 1) {
            actor = entry->actor;
            if (actor != 0) {
                if ((int)(actor->unk38 >> 24) == 0xA) {
                    return 0;
                }
            }
            if (actor != 0) {
                if ((int)(actor->unk38 >> 24) == 1) {
                    if (actor->info != 0) {
                        if (actor->info->unk1C != 0x6BD564F0) {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    if (entry->actor != 0 && PlayerInHost(owner) && owner->unk26C8 != 0 &&
        *(u32*)owner->unk26C8 == 0xFFF5 &&
        (int)(entry->actor->unk38 >> 24) != 0x13) {
        return 0;
    }
    actor = entry->actor;
    if (actor != 0) {
    dy = (float)entry->y - query->y;
    dx = (float)entry->x - query->x;
    dz = (float)entry->z - query->z;
    dist = dy * dy + dx * dx + dz * dz;
    actorReply.actor = actor;
    actor->handle(0x400, &actorReply);
    ownerReply.actor = owner;
    owner->handle(0x400, &ownerReply);
    if (entry->actor == ownerReply.actor->unk2794 ||
        entry->actor == ownerReply.actor->unk26E4) {
        return 0;
    }
    if ((ownerReply.mask & actorReply.mask) != 0 || ownerReply.mask == 1) {
        if (dist < query->hit->dist) {
            bone = ModelFindBone(entry->model, lbl_804D2BC0);
            model = entry->model;
            bones = fn_80037B6C(model);
            count = ModelBoneCount(model);
            found = ZoneBoneFlagged(model, bones, count);
            query->hit->invalid = 0;
            if ((bone == -1 && found == 0) ||
                (bone != -1 && bone == entry->bone) ||
                (found == 1 && (entry->flags & 0x800) != 0)) {
                *(Actor**)lbl_804D4948 = entry->actor;
                query->hit->model = entry->model;
                query->hit->bone = ModelBoneName(entry->model, entry->bone);
                query->hit->x = dx;
                query->hit->y = dy;
                query->hit->z = dz;
                query->hit->dist = dist;
                if ((int)(actorReply.id >> 24) == 0x13 ||
                    (int)(actorReply.id >> 24) == 2) {
                    if (actorReply.unk0C != 0) {
                        reply.actor = 0;
                        SendMsg(actorReply.id, 0x400, &reply);
                        if (reply.actor == actorReply.actor) {
                            if ((int)(actorReply.id >> 24) == 0x13) {
                                query->hit->kind13 = 1;
                            } else if ((int)(actorReply.id >> 24) == 2) {
                                query->hit->kind2 = 1;
                            }
                            if ((ownerReply.mask & actorReply.mask) != 0) {
                                query->hit->overlap = 1;
                            }
                            query->hit->unk24 = actorReply.unk0C;
                        }
                    }
                }
                return 1;
            }
            query->hit->invalid = 1;
        }
    }
    return 0;
    }
    return 1;
}

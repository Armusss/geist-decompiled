// u_800D0BF4.cpp: the "Powerup" actor type (0x800D0BB8-0x800D14A8); the zone list code after it is u_800D14A8.cpp.
// The streams carry power-up state between machines: a tagged header ("[PWRUPX]" clears the
// stored power-up, "[PWERUP]" is followed by two 12-byte blocks).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct ActorType;

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
    int x;             // 0x24 (integer world position)
    int y;             // 0x28
    int z;             // 0x2C
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

inline u32 StrHash(const char* s) {
    int i;
    u32 h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    for (i = 0; s[i] != 0; i++) {
        h = h * 31 + (u8)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
}

inline u32 StrHash(const char* s, u32 h) {
    int i;

    if (s == 0) {
        return 0;
    }
    for (i = 0; s[i] != 0; i++) {
        h = h * 31 + (unsigned char)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
}

extern "C" void fn_80026660(int id, void (*handler)(), int arg);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void* memcpy(void* dst, const void* src, u32 n);
extern "C" void* memset(void* dst, int c, u32 n);
void VFree(void* p);
void* VAlloc(int size);
int ActorInitBase(Actor* actor);
int ActorTypeRegister(ActorType* type);
bool ZoneSpawnActorMsg(Actor* actor, int msg, void* data);

// Actor type descriptor (same layout as the other actor files' types).
struct ActorType {
    int unk0;                                              // 0x00
    const char* name;                                      // 0x04
    int (*msg)(Actor* actor, int msg, void* data, int arg);  // 0x08
    void (*update)(Actor* actor, float dt);                // 0x0C
    int (*init)(Actor* actor);                             // 0x10
    void* unk14;                                           // 0x14
    void (*destroy)(Actor* actor);                         // 0x18
    void (*save)(ZoneStream* s);                           // 0x1C
    void (*load)(ZoneStream* s);                           // 0x20
    void* unk24[3];                                        // 0x24
};

int ZonePowerUpMsg(Actor* actor, int msg, void* data, int arg);
void ZonePowerUpUpdate(Actor* actor, float dt);
int ZoneRefreshPowerUp(Actor* actor);
void ZoneFreePowerUp(Actor* actor);
void ZoneWritePowerUp(ZoneStream* s);
void ZoneReadPowerUp(ZoneStream* s);

ActorType gPowerupType = { 0, "Powerup", ZonePowerUpMsg, ZonePowerUpUpdate, ZoneRefreshPowerUp, 0, ZoneFreePowerUp,
                           ZoneWritePowerUp, ZoneReadPowerUp };

void ZoneRegisterActor(void) {
    fn_80026660(0x28, (void (*)())ZoneSpawnActorMsg, 0);
    ActorTypeRegister(&gPowerupType);
}

// Takes n bytes off the front of a load stream and returns where they were.
inline void* StreamTake(ZoneStream* s, int n) {
    void* p = s->ptr;
    s->avail += n;
    s->size -= n;
    s->ptr += n;
    return p;
}

void ZoneReadPowerUp(ZoneStream* s) {
    u32* tag;
    u32 hash;
    int ok;
    Actor* a = s->actor;

    if (s->avail >= 4) {
        tag = (u32*)StreamTake(s, 4);
        hash = StrHash("[PWRUPX]");
        if (*tag == hash) {
            ok = 1;
        } else {
            ok = 0;
        }
        if (ok) {
            VFree(a->powerUp);
            a->powerUp = 0;
        } else {
            hash = StrHash("[PWERUP]");
            if (*tag == hash && s->avail >= 0x18) {
                u8* d = a->powerUp;

                memcpy(d, s->ptr, 0xC);
                s->avail -= 0xC;
                s->ptr += 0xC;
                memcpy(d + 0xC, s->ptr, 0xC);
                s->avail -= 0xC;
                s->ptr += 0xC;
            }
        }
    }
}

void ZoneWritePowerUp(ZoneStream* s) {
    u8* data = s->actor->powerUp;

    if (data == 0) {
        if (s->size < 4) {
            s->status = 0;
        } else {
            u32 tag = StrHash("[PWRUPX]");

            memcpy(s->ptr, &tag, 4);
            s->avail += 4;
            s->size -= 4;
            s->ptr += 4;
        }
    } else if (s->size < 0x1C) {
        s->status = 0;
    } else {
        u32 tag = StrHash("[PWERUP]");

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

// Payload of the 0x40B animation message (as in the Pickup actor).
struct ZoneAnimMsg {
    int unk0;    // 0x00
    int unk4;    // 0x04
    u32 key;     // 0x08
    int asset;   // 0x0C
    int unk10;   // 0x10
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
    u32 pad28[4];  // 0x28
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

    switch (msg) {
    case 0x40A:
        if (m->unk08 == 0x7C44EC3D) {
            ZoneAnimMsg am;

            am.unk4 = 0;
            am.asset = -1;
            m->unk0C = 0xF21581DE;
            am.key = m->unk0C;
            am.asset = AssetFind(0xC0A77187, 0x7EC7F65C);
            SendMsg(m->target, 0x40B, &am);
        }
        return 1;
    case 0x409: {
        Actor* a = m->actor;
        ZoneMsgTarget* target = (ZoneMsgTarget*)m->target;
        ZonePowerUp* p;
        ZoneParamInfo info;

        if (a == 0 || target == 0 || (p = (ZonePowerUp*)a->powerUp) == 0) {
            return 0;
        }
            info.target = target;
            info.data[0] = p->data[0];
            info.data[3] = p->data[3];
            info.data[1] = p->data[1];
            info.data[4] = p->data[4];
            info.data[2] = p->data[2];
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
    case 0x400: {
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
    }
    return ActorDefaultMsg(actor, msg, data, arg);
}

bool ZoneSpawnActorMsg(Actor* actor, int msg, void* data) {
    ZoneSpawnDesc desc;
    int* d;
    Actor* spawned;
    bool ok;

    switch (msg) {
    case 0x1500:
        d = (int*)data;
        desc.unk1C = -1;
        desc.x = ((float*)d)[8];
        desc.y = ((float*)d)[9];
        desc.z = ((float*)d)[10];
        desc.param = (void*)d[1];
        spawned = fn_800C7E80(&desc);
        if (spawned != 0) {
            spawned->type = &gPowerupType;
            ok = true;
        } else {
            ok = false;
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
        float base = 1000.0f;

        state->active = 1;
        state->timer = base * actor->info->scale;
    }
    return 1;
}

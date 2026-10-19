// u_801A1D24.cpp: the "Pickup" actor type (0x801A1D24-0x801A28A4).
// A pickup the ghost can possess: while carried it counts down, and a global handler
// (message 0x1502) finds a matching pickup and hands it to the ghost.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// Referenced by the actor type descriptor in the retail data section.
static const char* sActorName[] = { "Pickup" };

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

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Mtx33 {
    float m[3][3];
};

struct Mtx34 {
    float m[3][4];
};

struct PlaceF {
    Mtx33 rot;
    Vec3 pos;
};

struct Placement {
    Mtx33 rot;
    IVec3 pos;
};

struct Actor;
struct HostData;
struct HostInfo;

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

struct Actor {
    Placement place;
    int (*query)(int msg, void* data);
    char pad34[4];
    u32 handle;
    char pad3C[0x44];
    HostInfo* def;
    HostInfo* info;
    char pad88[4];
    int id;
    char pad90[0x14];
    HostData* data;
    int unkA8;
    u32 flags;
    char padB0[0x60];
    int unk110;
    char pad114[4];
    float unk118;
};

struct HostInfo {
    char pad0[0x18];
    int unk18;
    int unk1C;
    int unk20;
    u32 key;
    char pad28[8];
    float unk30;
    float unk34;
    float unk38;
    float unk3C;
};

// Per-pickup state (0x30 bytes, allocated from sHostPool).
struct HostData {
    u32 unk0;
    int unk4;
    int unk8;
    int x;
    int y;
    int z;
    int unk18;
    int active;
    float timer;
    float timerMax;
    int unk28;
    int handle;
};

// Save-game stream handed to the pack/unpack callbacks.
struct HostStream {
    Actor* actor;
    int remain;
    u8* ptr;
    int count;
    int status;
};

struct HostMsg {
    Actor* target;
    Actor* host;
    u32 anim;
    u32 key;
};

struct AnimMsg {
    int unk0;
    int unk4;
    u32 key;
    int asset;
    int unk10;
};

struct UnlockMsg {
    int unk0;
    int unk4;
    u32 flags;
    u32 key;
};

struct ShowMsg {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
};

struct InspectMsg {
    Actor* actor;
    int id;
    int unk8;
    int unkC;
    float active;
    float unk14;
    int unk18;
    Actor* unk1C;
};

// Query sent to the ghost that is scanning a pickup.
struct PickupQuery {
    Actor* host;
    int unk4;
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
    float px;
    float py;
    float pz;
};

// Payload of the global 0x1502 message.
struct PickupScanData {
    int unk0;
    u32 id;
    char pad8[0x18];
    Vec3 pos;
};

int ActorIsActive(Actor* a);                                // 0x80022D80
void ActorFn80022D98(Actor* a, int b);                       // 0x80022D98
int ActorFn80022FD8(Actor* a);                               // 0x80022FD8
void* PoolAlloc(void* pool, int size, void* used, int count); // 0x800249A8
void PoolFree(void* p, void* pool, int size, void* used);    // 0x80024978
extern "C" int SendMsg(int target, int msg, void* data);                // 0x800265D4
int AssetFind(u32 bank, u32 key);                            // 0x8007EB34
void ActorGetMatrix(Mtx34* m, Actor* a);                     // 0x8009ED70
void PlacementToMatrix(Mtx34* m, PlaceF* p);                 // 0x8009EF04
void MtxSetEuler(Mtx34* m, Vec3* rot);                       // 0x8009FAC0
int ActorPlayAnim(int handle, u32 anim, int arg);            // 0x800C5654
void ActorUpdate(Actor* a, float dt);                        // 0x800E48A0
int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
int ActorInitBase(Actor* a);                                 // 0x800E6444
void ActorSetPlacement(Actor* a, PlaceF* p);                 // 0x800E7898
void Vec3Set(Vec3* v, float x, float y, float z);            // 0x8016D67C
void SoundPlay(u32 id);                                      // 0x8018FA30
extern "C" void fn_8009F190(IVec3* v, Actor* a);             // 0x8009F190
extern "C" void fn_8009F028(Actor* a, IVec3* v);             // 0x8009F028
extern "C" void fn_800902F0(int x);                          // 0x800902F0

extern "C" void* memcpy(void* dst, const void* src, u32 n);

static HostData sHostPool[0x40];
static int sHostUsed[4];

// 0x801A1D24: unpack the pickup state from a save-game stream.
// Takes n bytes off the front of a load stream and returns where they were.
inline void* StreamTake(HostStream* s, int n) {
    void* p = s->ptr;
    s->count += n;
    s->remain -= n;
    s->ptr += n;
    return p;
}

void PickupLoad(HostStream* s) {
    HostData* d;
    u32* tag;
    u32 hash;
    int ok;
    Actor* a = s->actor;

    if (s->count >= 0x1C) {
        if (s->count < 4) {
            ok = 0;
        } else {
            tag = (u32*)StreamTake(s, 4);
            hash = StrHash("[INVENT]");
            if (*tag == hash) {
                ok = 1;
            } else {
                ok = 0;
            }
        }
        if (ok) {
            d = (HostData*)a->data;
            memcpy(d, s->ptr, 0xC);
            s->count -= 0xC;
            s->ptr += 0xC;
            memcpy((char*)d + 0xC, s->ptr, 0xC);
            s->count -= 0xC;
            s->ptr += 0xC;
        }
    }
}

// 0x801A1E8C: pack the pickup state into a save-game stream.
void PickupSave(HostStream* s) {
    HostData* d = (HostData*)s->actor->data;
    u32 hash;

    if (d == 0) {
        return;
    }
    if (s->remain < 0x1C) {
        s->status = 0;
        return;
    }
    hash = StrHash("[INVENT]");
    memcpy(s->ptr, &hash, 4);
    s->count += 4;
    s->remain -= 4;
    s->ptr += 4;
    memcpy(s->ptr, d, 0xC);
    s->count += 0xC;
    s->remain -= 0xC;
    s->ptr += 0xC;
    memcpy(s->ptr, (char*)d + 0xC, 0xC);
    s->count += 0xC;
    s->remain -= 0xC;
    s->ptr += 0xC;
}

// 0x801A2004: per-frame update while the pickup is being carried.
void PickupUpdate(Actor* a, float dt) {
    HostData* d = (HostData*)a->data;

    if (d->active) {
        d->timer -= dt;
        if (d->timer <= 0.0f) {
            ShowMsg m;
            m.unk4 = 1;
            SendMsg(a->id, 0xC00, &m);
            d->active = 0;
        }
    }
    if (d->unk28) {
        IVec3 angle;
        PlaceF pl;
        fn_8009F190(&angle, a);
        angle.z += (int)dt;
        fn_8009F028(a, &angle);
        ActorGetMatrix((Mtx34*)&pl, a);
        ActorSetPlacement(a, &pl);
        a->flags |= 0x800000;
    }
    ActorUpdate(a, dt);
}

// 0x801A2104: message handler.
int PickupMsg(Actor* a, int msg, void* data, int arg) {
    ShowMsg sm;
    UnlockMsg um;
    AnimMsg am;
    PickupQuery q;
    HostMsg* m;

    switch (msg) {
    case 0x40A: {
        m = (HostMsg*)data;
        if (ActorPlayAnim((int)a, StrHash("QUERY", m->anim), (int)m->host)) {
            return 0;
        }
        if (m->anim == 0x7C44EC3D) {
            am.unk4 = 0;
            am.asset = -1;
            m->key = 0xF21581DE;
            am.key = m->key;
            am.asset = AssetFind(0xC0A77187, 0x8CA7FE7C);
            SendMsg((int)m->host, 0x40B, &am);
        }
        return 1;
    }
    case 0x409: {
        HostData* d;
        Actor* host = ((Actor**)data)[1];
        Actor* target = ((Actor**)data)[0];
        if (target && host) {
            d = target->data;
            if (d) {
                q.host = host;
                q.unk8 = d->unk0;
                q.unk14 = d->x;
                q.unkC = d->unk4;
                q.unk18 = d->y;
                q.unk10 = d->unk8;
                q.unk1C = d->z;
                q.px = (float)target->place.pos.x;
                q.py = (float)target->place.pos.y;
                q.pz = (float)target->place.pos.z;
                if (host->query(0x1503, &q)) {
                    ActorPlayAnim(target->id, 0xB94AE51B, target->id);
                    sm.unk4 = 0;
                    SendMsg(target->id, 0xC00, &sm);
                    if (d->unk18) {
                        d->timer = d->timerMax;
                        d->active = 1;
                    }
                }
                if ((int)(host->handle >> 24) == 2) {
                    SoundPlay(q.unk8);
                    um.flags = host->handle;
                    um.key = 0x1C19D386;
                    um.unk4 = 1;
                    SendMsg(0x20000000, 0xC14, &um);
                }
            }
        }
        return 1;
    }
    case 0x405:
        return 1;
    case 0x400: {
        Actor* t = ((InspectMsg*)data)->actor;
        InspectMsg* m = (InspectMsg*)data;
        m->id = t->id;
        m->unkC = 1;
        m->active = ActorIsActive(t) ? 1.0f : 0.0f;
        m->unk14 = 0.0f;
        m->unk1C = t;
        m->unk8 = 0;
        m->unk18 = 3;
        return 1;
    }
    case 1:
        if (((((int*)data)[2] & 0x04000000) ? -999999 : 0) == -999999) {
            return 0;
        }
        break;
    case 0x41D: {
        Actor* target = ((Actor**)data)[0];
        int handled;
        HostData* d = target->data;
        switch (((int*)data)[1]) {
        case (int)0x9D93F3A0:
            d->unk28 = ((int*)data)[3];
            handled = 1;
            break;
        default:
            handled = 0;
            break;
        }
        if (handled) {
            return 1;
        }
        break;
    }
    }
    return ActorDefaultMsg(a, msg, data, arg);
}

// 0x801A24BC: global handler for message 0x1502 (the ghost scanning for a pickup).
int PickupScanMsg(int value, int msg, void* data) {
    int i;
    PickupScanData* m = (PickupScanData*)data;

    switch (msg) {
    case 0x1502:
        for (i = 0; i < 0x40; i++) {
        Vec3 v;
        InspectMsg find;
        HostData* d;
        Actor* target;

        if (!(sHostUsed[i / 32] & (1 << (i & 31)))) {
            continue;
        }
        if (!SendMsg(sHostPool[i].handle, 0x400, &find)) {
            continue;
        }
        target = find.actor;
        if (target == 0) {
            continue;
        }
        d = target->data;
        if (!ActorFn80022FD8(target)) {
            continue;
        }
        if (d->unk0 != m->id) {
            continue;
        }
        HostData* h = (HostData*)target->data;
        do {
            if (h == 0) {
                if (!ActorInitBase(target)) {
                    break;
                }
                h = (HostData*)PoolAlloc(sHostPool, 0x30, sHostUsed, 0x40);
                if (h == 0) {
                    break;
                }
                target->data = h;
            } else {
                ActorFn80022D98(target, 1);
                fn_800902F0(target->unk110);
            }
            h->x = (int)target->info->unk30;
            h->unk0 = target->info->unk18;
            h->y = (int)target->info->unk34;
            h->unk4 = target->info->unk1C;
            h->z = (int)target->info->unk38;
            h->unk8 = target->info->unk20;
            h->unk28 = 0;
            h->handle = target->handle;
            if (target->info->key == 0x6C345908) {
                h->unk18 = 1;
                h->timerMax = 1000.0f * target->info->unk3C;
            }
        } while (0);
        {
            PlaceF pl;
            d->unk28 = 1;
            Vec3Set(&v, 0.0f, 45.0f, 0.0f);
            MtxSetEuler((Mtx34*)&pl.rot, &v);
            pl.pos = m->pos;
            PlacementToMatrix((Mtx34*)&target->place, &pl);
            ActorSetPlacement(target, &pl);
            target->unk118 = pl.pos.z;
            ActorPlayAnim(target->handle, 0x8B3A294E, target->handle);
        }
        return 1;
        }
        return 0;
    }
    return 0;
}

// 0x801A2710: destroy callback.
void PickupDestroy(Actor* a) {
    PoolFree(a->data, sHostPool, 0x30, sHostUsed);
    a->data = 0;
}

// 0x801A275C: init callback.
int PickupInit(Actor* a) {
    HostData* h;

    if (a->data == 0) {
        if (!ActorInitBase(a)) {
            return 0;
        }
        h = (HostData*)PoolAlloc(sHostPool, 0x30, sHostUsed, 0x40);
        if (h == 0) {
            return 0;
        }
        a->data = h;
    } else {
        h = (HostData*)a->data;
        ActorFn80022D98(a, 1);
        fn_800902F0(a->unk110);
    }
    h->x = (int)a->info->unk30;
    h->unk0 = a->info->unk18;
    h->y = (int)a->info->unk34;
    h->unk4 = a->info->unk1C;
    h->z = (int)a->info->unk38;
    h->unk8 = a->info->unk20;
    h->unk28 = 0;
    h->handle = a->handle;
    if (a->info->key == 0x6C345908) {
        h->unk18 = 1;
        h->timerMax = 1000.0f * a->info->unk3C;
    }
    return 1;
}

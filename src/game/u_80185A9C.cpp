// u_80185A9C.cpp (0x80185A9C-0x80187108), split from u_8017D794.
// Behaviour registry: the table 0x803A8784 of behaviour descriptors (incl. the empty one 0x803A8760),
// .bss 0x80495790, .sdata 0x804D3028-0x804D3050, .sbss 0x804D50D0-0x804D50E0, and the screen-effect slot type
// 0x803A87DC (fn_80186F88.. fn_801870E4). Starts at fn_80185A9C, which only fn_801869D0 calls. Own double 0x804D8400.
// Names are descriptive guesses; the code is matched against the retail DOL.
//
// The file runs a pool of 64 projectiles (lbl_80495790 / lbl_804D50D8, ListAlloc/ListRemoveFree). Each projectile
// has a kind (+0xC2), an index into lbl_804D50D4, an array of 0x48-byte kind records built from the variable
// table by fn_80186314. A kind record points at a behaviour descriptor found by name hash in lbl_803A8784.
// The kind array is a Buffer-derived container (vtable 0x803A87DC: destructor, Clear, element size 0x48,
// construct, destroy, span); its element constructor and destructor are empty, so the range loops compile to
// bare counts (fn_801870C0 / fn_801870E4).

// auto_inline off: fn_801868AC calls the empty fn_80185A9C defined above it (an auto-inlined copy would vanish).
#pragma auto_inline off

#include <dolphin/types.h>
#include "game/types.h"

struct Actor;
struct BehKind;
struct Projectile;

void* VAlloc(int size);
void VFree(void* p);
void ActorGetMatrix(Mtx34* m, Actor* a);
int ActorFn80022FD8(Actor* a);
void ActorUnlinkOwnerObjects(Actor* a);
void ActorRelinkOwners(Actor* a);
void ActorFn800231A8(Actor* a);
void ActorFn8002378C(Actor* actor, int a, int b, int c, int d, int e, int f);
void ListRemoveFree(void* p, void** list, u32* bits, int count);
void* ListAlloc(void** list, int size, u32* bits, int count);
void MtxSetEuler(Mtx34* m, Vec3* euler);
void PlacementToMatrix(Mtx34* m, PlaceF* p);
int Rand();
int EffectRowIndex(int row);
u32 HashMix(unsigned long a, unsigned long b);
int VarGroupObjCount(unsigned long group);
int AssetFind(unsigned long group, unsigned long name);
void TextureLoad(short* ids, int count);

extern "C" {
int SendMsg(int to, int msg, void* data);
int rand();
void fn_80026660(int type, int (*fn)(int, int, void*), int ctx);
void fn_8016EEF4(int fx);
void fn_8021284C(u32 handle);
void fn_800390F4(int link);
int fn_800391DC(int asset);
void* fn_8003A1C0(int asset);
void fn_8003F15C(int link, void* m, int a, int b, int c, float f);
void fn_8007C5C8(Vec3* pos, int a, float dt);
int fn_80084540(u32 handle, int msg, void* data, int* result);
void fn_8016EDD8(int fx, IVec3* pos);
int fn_8016F120(int fx, IVec3* pos);
float fn_802A2080(void);
u32 fn_800F7A90(int row);
float fn_800F7920(int row, u32 hash);
int fn_800F7994(int row, u32 hash);
}

// --- The Buffer class chain (implemented in u_8028A8D4.cpp; declared here as that unit declares it) ---------
class ScreenEffectSlot {
public:
    ScreenEffectSlot();
    virtual ~ScreenEffectSlot() {}
    virtual void Clear();                           // 0x0C
    virtual int ElementSize();                      // 0x10
    virtual void Construct(char* data, u32 size);   // 0x14
    virtual void Destroy(char* data, u32 size);     // 0x18
    virtual int Span();                             // 0x1C
    static void* operator new(unsigned long size) { return VAlloc(size); }
    static void operator delete(void* p) { VFree(p); }

    char* mText;      // 0x04
    u32 mLength;      // 0x08 bytes in use
    char* mStore;     // 0x0C
    u32 mCapacity;    // 0x10
};

class ScreenEffectBuffer : public ScreenEffectSlot {
public:
    ScreenEffectBuffer();
    virtual ~ScreenEffectBuffer() {}
};

class Buffer : public ScreenEffectBuffer {
public:
    Buffer();
    virtual ~Buffer();
};

extern "C" {
void fn_8028B960(Buffer* self);
char* fn_8028B3BC(Buffer* self, char* pos, u32 size);
void fn_8028B35C(Buffer* self, char* pos, u32 size);
void fn_8028BC2C(Buffer* self, char* data, u32 size);
void fn_8028BC28(Buffer* self, char* data, u32 size);
}

// --- Behaviour descriptors ---------------------------------------------------------------------------------
// {hash, start, update, blocked, 0, stop, notify, msg, flags}; flags bit 0: no movement, bit 1: no world link,
// bit 2: wait for the fade time before dying.
struct BehDesc {
    u32 hash;                                                    // 0x00
    int (*start)(BehKind* kind, Projectile* p);                  // 0x04
    int (*update)(BehKind* kind, Projectile* p, float dt);       // 0x08
    void* blocked;                                               // 0x0C
    void* unk10;                                                 // 0x10
    void (*stop)(BehKind* kind, Projectile* p);                  // 0x14
    void (*notify)(BehKind* kind, Projectile* p);                // 0x18
    int (*msg)(int msg, void* data);                             // 0x1C
    u32 flags;                                                   // 0x20
};

// One projectile kind (0x48 bytes), read from the variable table.
struct BehKind {
    BehKind() {}
    u32 row;              // 0x00 effect row index
    int count;            // 0x04
    int count2;           // 0x08
    float unk0C;          // 0x0C
    float unk10;          // 0x10
    float lifetime;       // 0x14
    float unk18;          // 0x18
    u32 hash1C;           // 0x1C
    float unk20;          // 0x20
    int model;            // 0x24
    int fx;               // 0x28
    int unk2C;            // 0x2C
    int unk30;            // 0x30
    float unk34;          // 0x34
    float unk38;          // 0x38
    float unk3C;          // 0x3C
    float unk40;          // 0x40
    BehDesc* desc;        // 0x44
};

// Spawn placement (0x4C bytes), copied to +0x68.
struct ProjSpawn {
    Vec3 pos;             // 0x00
    Vec3 rot;             // 0x0C
    Vec3 unk18;           // 0x18
    Vec3 vel;             // 0x24
    Vec3 unk30;           // 0x30
    int unk3C;            // 0x3C
    Vec3 unk40;           // 0x40
};

struct ProjShape {
    Vec3 min;
    Vec3 max;
};

struct Projectile {
    Placement place;      // 0x00
    u8 pad30[0x8];
    u32 handle;           // 0x38
    u8 pad3C[0x10];
    int flags;            // 0x4C
    ProjShape shape;      // 0x50
    ProjSpawn spawn;      // 0x68
    Vec3 prev;            // 0xB4
    u16 state;            // 0xC0
    u16 kind;             // 0xC2
    float age;            // 0xC4
    int link;             // 0xC8
    int id;               // 0xCC
    int fx;               // 0xD0
    Vec3 start;           // 0xD4
    u32 self;             // 0xE0
    int owner;            // 0xE4
    float scale;          // 0xE8
    int unkEC;            // 0xEC
    void* data;           // 0xF0
    BehKind* beh;         // 0xF4
    float fade;           // 0xF8
};

inline void* operator new(unsigned long, void* p) { return p; }

// The kind array.
class KindArray : public Buffer {
public:
    KindArray() {}
    virtual ~KindArray();
    virtual void Construct(char* data, u32 size);
    virtual void Destroy(char* data, u32 size);
    virtual int ElementSize();

    u32 Count() { return mLength / sizeof(BehKind); }
    BehKind* Begin() { return (BehKind*)mStore; }
    BehKind* End() { return (BehKind*)(mStore + mLength); }
    BehKind& operator[](int i) { return ((BehKind*)mStore)[i]; }
};

struct Msg400 {
    void* actor;          // 0x00
    int ownerId;          // 0x04
    int unk08;            // 0x08
    int state;            // 0x0C
    float f10;            // 0x10
    float f14;            // 0x14
    int flags;            // 0x18
    int unk1C;            // 0x1C
};

struct ProjMsg {
    Projectile* actor;    // 0x00
    void* arg;            // 0x04
    int unk08;            // 0x08
    int hash;             // 0x0C
    Placement place;      // 0x10
};

extern "C" {

extern BehDesc* lbl_803A8784[22];
Projectile* lbl_80495790[64];
int lbl_804D3028 = -1;
int lbl_804D302C = -1;
int lbl_804D3030 = -1;
int lbl_804D3034 = -1;
int lbl_804D3038 = -1;
int lbl_804D303C = -1;
int lbl_804D3040 = -1;
int lbl_804D3044 = -1;
int lbl_804D3048 = -1;
int lbl_804D50D0;
KindArray* lbl_804D50D4;
u32 lbl_804D50D8[2];

inline int BitTest(u32* bits, int i) {
    return (1 << (i & 31)) & bits[i / 32];
}

inline u32 ProjIndex(Projectile* p) {
    return p->handle & 0xFFFFFF;
}

inline BehKind* ProjKind(Projectile* p) {
    return &(*lbl_804D50D4)[p->kind];
}

inline void ProjFree(Projectile* p) {
    BehKind* kind = ProjKind(p);
    if (kind->desc != 0 && kind->desc->stop != 0) {
        kind->desc->stop(kind, p);
    }
    if (p->fx != -1) {
        fn_8016EEF4(p->fx);
    }
    fn_8021284C(p->handle);
    if (ActorFn80022FD8((Actor*)p)) {
        ActorUnlinkOwnerObjects((Actor*)p);
    }
    if (p->link != -1) {
        fn_800390F4(p->link);
    }
    ListRemoveFree(p, (void**)lbl_80495790, lbl_804D50D8, 64);
}

inline int ProjQuery(Projectile* p, Msg400* out) {
    Msg400 reply;
    SendMsg(p->id, 0x400, &reply);
    out->actor = p;
    out->ownerId = p->self;
    out->flags = reply.flags;
    out->state = 1;
    out->f10 = 4096.0f;
    out->f14 = 0.0f;
    return 1;
}

void fn_80185A9C(void) {
}

int fn_80185AEC(int msg, void* data);

int fn_80185AA0(int index, int msg, void* data) {
    Projectile* p = lbl_80495790[index];
    if (p != 0) {
        *(Projectile**)data = p;
        return fn_80185AEC(msg, data);
    }
    return 0;
}

void fn_80185F7C(Projectile* p, float dt);

int fn_80185AEC(int msg, void* data) {
    u32 index;
    ProjMsg* m;
    BehDesc* desc;
    int result;
    m = (ProjMsg*)data;
    index = m->actor->handle & 0xFFFFFF;
    desc = (*lbl_804D50D4)[m->actor->kind].desc;

    if (desc != 0 && desc->msg != 0) {
        result = desc->msg(msg, m);
        if (result == -999999) {
            return 0;
        }
        if (result != 0) {
            return result;
        }
    }
    switch (msg) {
    case 0: {
        Projectile* p = lbl_80495790[index];
        BehKind* k = ProjKind(p);
        if (k->desc != 0 && k->desc->notify != 0) {
            k->desc->notify(k, p);
            return 1;
        }
        if (p->link != -1) {
            if (p->link != -1) {
                Mtx34 mtx;
                ActorGetMatrix(&mtx, (Actor*)p);
                fn_8003F15C(p->link, &mtx, 1, 0, 0, 1.0f);
            }
            return 1;
        }
        return 0;
    }
    case 0x411:
    case 0x412: {
        Projectile* p = lbl_80495790[index];
        if (m->actor != 0) {
            m->place = p->place;
            m->unk08 = 0;
            m->hash = 0;
            return 1;
        }
        return 0;
    }
    case 0x404:
        lbl_80495790[index]->place = *(Placement*)m->arg;
        return 1;
    case 0x400:
        return ProjQuery(lbl_80495790[index], (Msg400*)m);
    case 0x414:
        fn_80185F7C(m->actor, *(float*)&m->arg);
        m->actor->flags |= 0x400;
        return 1;
    case 0xC14:
        if (m->hash != 0x879F2BDA) {
            break;
        }
        ProjFree(m->actor);
        return 1;
    case 0x407:
        break;
    }
    return fn_80084540(index | 0x4A000000, msg, m, &result) != 0;
}

void fn_80185EE4(float dt) {
    int i;
    for (i = 0; i < 64; i++) {
        if (BitTest(lbl_804D50D8, i)) {
            fn_80185F7C(lbl_80495790[i], dt);
        }
    }
}

void fn_80185F7C(Projectile* p, float dt) {
    BehKind* kind = ProjKind(p);

    if (kind->desc == 0) {
        ProjFree(p);
        return;
    }
    if ((p->flags & 0x400) == 1) {
        return;
    }
    if (!(p->flags & 0x20000)) {
        dt *= fn_802A2080();
    }
    if (p->age == 0.0f) {
        dt *= rand() / 32767.0f;
    }
    p->age += dt;
    if (p->age >= kind->lifetime) {
        p->state |= 8;
    }
    if (!(p->state & 8)) {
        if (!(kind->desc->flags & 1)) {
            Vec3 rot;
            Mtx34 mtx;
            p->prev.x = p->spawn.pos.x;
            p->prev.y = p->spawn.pos.y;
            p->prev.z = p->spawn.pos.z;
            fn_8007C5C8(&p->spawn.pos, 0, dt);
            rot = p->spawn.rot;
            MtxSetEuler(&mtx, &rot);
            PlacementToMatrix((Mtx34*)p, (PlaceF*)&mtx);
            p->place.pos.x = p->spawn.pos.x;
            p->place.pos.y = p->spawn.pos.y;
            p->place.pos.z = p->spawn.pos.z;
        }
        if (kind->desc->update != 0 && !kind->desc->update(kind, p, dt)) {
            p->state |= 8;
        }
        if (p->fx != -1) {
            fn_8016EDD8(p->fx, &p->place.pos);
        }
        if (kind->desc->flags & 2) {
            ActorRelinkOwners((Actor*)p);
            if (!ActorFn80022FD8((Actor*)p)) {
                p->spawn.pos.x = p->prev.x;
                p->spawn.pos.y = p->prev.y;
                p->spawn.pos.z = p->prev.z;
                ActorRelinkOwners((Actor*)p);
                p->state |= 8;
            }
        }
    }
    if ((p->state & 8) &&
        (!(kind->desc->flags & 4) || p->fade > 1.1920929e-7f || p->age >= kind->lifetime)) {
        ProjFree(p);
    } else {
        p->fade += dt;
    }
}

inline void KindsSkip() {
    u32 i;
    for (i = 0; i < lbl_804D50D4->Count(); i++) {
    }
}

void fn_80186314(void) {
    int n = VarGroupObjCount(0x47F42363);
    int row;
    for (row = 0; row < n; row++) {
        if (fn_800F7A90(row) == 0x1EB6CE) {
            BehKind kind;
            int index;
            u32 hash;
            int i;
            memset(&kind, 0, sizeof(BehKind));
            index = EffectRowIndex(row);
            KindsSkip();
            hash = HashMix(row, 0x7EE45046);
            for (i = 0; i < 22; i++) {
                if (hash == lbl_803A8784[i]->hash) {
                    kind.desc = lbl_803A8784[i];
                    break;
                }
            }
            if (kind.desc != 0) {
                kind.row = index;
                kind.count = fn_800F7920(row, 0x269718BC);
                kind.count2 = fn_800F7920(row, 0xA236FD15);
                kind.unk0C = fn_800F7920(row, 0x835DD393);
                kind.unk10 = fn_800F7920(row, 0x36F335AC);
                kind.lifetime = fn_800F7920(row, 0x67E713C9);
                kind.unk18 = fn_800F7920(row, 0x10A0020D);
                kind.unk20 = fn_800F7920(row, 0x6338424B);
                kind.hash1C = HashMix(row, 0xAFAA454B);
                kind.model = fn_800F7994(row, 0xB96BFA12);
                kind.fx = fn_800F7994(row, 0xB9C0C7F8);
                kind.unk2C = fn_800F7994(row, 0xE9525A02);
                kind.unk30 = fn_800F7994(row, 0xACF34481);
                kind.unk34 = fn_800F7920(row, 0x88FF8A1A);
                kind.unk38 = fn_800F7920(row, 0x1727C1BF);
                kind.unk3C = fn_800F7920(row, 0xCF962098);
                kind.unk40 = fn_800F7920(row, 0xA7355386);
                *(BehKind*)fn_8028B3BC(lbl_804D50D4, lbl_804D50D4->mStore + lbl_804D50D4->mLength,
                                       sizeof(BehKind)) = kind;
            }
        }
    }
    {
        short tex[8];
        lbl_804D3028 = AssetFind(0xF2E2BEB8, 0x30BE673B);
        lbl_804D302C = AssetFind(0xF2E2BEB8, 0xEDA570C7);
        lbl_804D3030 = AssetFind(0xF2E2BEB8, 0xCC7DEB86);
        lbl_804D3034 = AssetFind(0xF2E2BEB8, 0xF56C5256);
        lbl_804D3038 = AssetFind(0xF2E2BEB8, 0x43CF5CB9);
        lbl_804D303C = AssetFind(0xF2E2BEB8, 0x205E0464);
        lbl_804D3040 = AssetFind(0xF2E2BEB8, 0x5141FA62);
        lbl_804D3044 = AssetFind(0xF2E2BEB8, 0x60A26EDB);
        tex[0] = lbl_804D3028;
        tex[1] = lbl_804D302C;
        tex[2] = lbl_804D3030;
        tex[3] = lbl_804D3034;
        tex[4] = lbl_804D3038;
        tex[5] = lbl_804D303C;
        tex[6] = lbl_804D3040;
        tex[7] = lbl_804D3044;
        TextureLoad(tex, 8);
    }
    lbl_804D3048 = AssetFind(0x92234D44, 0x660D0D5C);
    lbl_804D50D0 = 0;
}

int fn_80186754(void) {
    Projectile* p;
    int i;
    for (i = 0; i < 64; i++) {
        p = lbl_80495790[i];
        if (p != 0) {
            ProjFree(p);
            lbl_80495790[i] = 0;
        }
    }
    if (lbl_804D50D4->Count() != 0) {
        fn_8028B35C(lbl_804D50D4, (char*)lbl_804D50D4->Begin(),
                    (lbl_804D50D4->End() - lbl_804D50D4->Begin()) * sizeof(BehKind));
    }
    delete lbl_804D50D4;
    return 1;
}

inline void BitsClear(u32* bits, int words) {
    int i;
    for (i = 0; i < words; i++) {
        bits[i] = 0;
    }
}

int fn_801868AC(void) {
    int i;
    fn_80026660(0x4A, (int (*)(int, int, void*))fn_80185AA0, 0);
    lbl_804D50D4 = new KindArray;
    BitsClear(lbl_804D50D8, 2);
    for (i = 0; i < 64; i++) {
        lbl_80495790[i] = 0;
    }
    fn_80185A9C();
    return 1;
}

BehKind* fn_801869B8(Projectile* p) {
    return ProjKind(p);
}

void fn_801869D0(Projectile* p) {
    ProjFree(p);
}

int fn_80186A88(int row, ProjSpawn* spawn, int id, int owner, int state, float scale) {
    int index;
    Projectile* p;
    int kindIndex;
    BehKind* kind;
    Rand();
    kindIndex = -1;
    index = EffectRowIndex(row);
    {
        u32 i;
        for (i = 0; i < lbl_804D50D4->Count(); i++) {
            if (index == (*lbl_804D50D4)[i].row) {
                kindIndex = i;
                break;
            }
        }
    }
    if (kindIndex == -1) {
        return 0;
    }
    p = (Projectile*)ListAlloc((void**)lbl_80495790, sizeof(Projectile), lbl_804D50D8, 64);
    if (p != 0) {
        Mtx34 mtx;
        Placement place;
        Vec3 rot;
        for (index = 0; index < 64; index++) {
            if (lbl_80495790[index] == p) {
                break;
            }
        }
        kind = &(*lbl_804D50D4)[kindIndex];
        memset(p, 0, sizeof(Projectile));
        p->kind = kindIndex;
        p->age = 0.0f;
        p->spawn = *spawn;
        p->prev.x = p->spawn.pos.x;
        p->prev.y = p->spawn.pos.y;
        p->prev.z = p->spawn.pos.z;
        p->id = id;
        p->start.x = p->spawn.pos.x;
        p->start.y = p->spawn.pos.y;
        p->start.z = p->spawn.pos.z;
        p->self = (index & 0xFFFFFF) | 0x4A000000;
        p->owner = owner;
        p->scale = scale;
        p->unkEC = 0;
        if (kind->model != -1) {
            p->link = fn_800391DC(kind->model);
            if (p->link == kind->model) {
                ListRemoveFree(p, (void**)lbl_80495790, lbl_804D50D8, 64);
                return -1;
            }
        } else {
            kind->desc->flags |= 2;
        }
        rot = p->spawn.rot;
        MtxSetEuler(&mtx, &rot);
        PlacementToMatrix((Mtx34*)&place, (PlaceF*)&mtx);
        place.pos.x = p->spawn.pos.x;
        place.pos.y = p->spawn.pos.y;
        place.pos.z = p->spawn.pos.z;
        if (!(kind->desc->flags & 2)) {
            p->shape = *(ProjShape*)fn_8003A1C0(kind->model);
        }
        ActorFn8002378C((Actor*)p, p->self, (int)&place, (int)fn_80185AEC, (int)&p->shape, (int)&p->shape, 0);
        if (kind->desc != 0 && kind->desc->start != 0 && !kind->desc->start(kind, p)) {
            if (p->link != -1) {
                fn_800390F4(p->link);
            }
            ListRemoveFree(p, (void**)lbl_80495790, lbl_804D50D8, 64);
            return -1;
        }
        p->state = state;
        p->beh = kind;
        if (kind->fx != -1) {
            p->fx = fn_8016F120(kind->fx, &p->place.pos);
        } else {
            p->fx = -1;
        }
        if (!(kind->desc->flags & 2)) {
            ActorFn800231A8((Actor*)p);
        }
    }
    return index;
}

int fn_80186F08(int index, Msg400* out) {
    return ProjQuery(lbl_80495790[index], out);
}

}

extern "C" void fn_801870C0(BehKind* first, BehKind* last);
extern "C" void fn_801870E4(BehKind* first, BehKind* last);

KindArray::~KindArray() {
    fn_8028B960(this);
}

void KindArray::Construct(char* data, u32 size) {
    fn_8028BC2C(this, data, size);
    fn_801870C0((BehKind*)data, (BehKind*)(data + size / sizeof(BehKind) * sizeof(BehKind)));
}

void KindArray::Destroy(char* data, u32 size) {
    BehKind* first = (BehKind*)data;
    fn_801870E4(first, first + size / sizeof(BehKind));
    fn_8028BC28(this, data, size);
}

int KindArray::ElementSize() {
    return sizeof(BehKind);
}

extern "C" {

void fn_801870C0(BehKind* first, BehKind* last) {
    for (; first < last; first++) {
        new (first) BehKind;
    }
}

void fn_801870E4(BehKind* first, BehKind* last) {
    for (; first < last; first++) {
        first->~BehKind();
    }
}

}

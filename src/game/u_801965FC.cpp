// u_801965FC.cpp: the "Foam" item type (0x801965FC-0x801969D8) and the "EXTINGUISHER" effect type
// (0x801969D8-0x80197E50). The two halves have their own string pools (each starting with the action-name
// table) and their own .sdata2 constants, so they are probably two source files.
// Foam: picking it up spawns the WP_FIRE1 model at the owner and tells the level; dropping frees it.
// Extinguisher: while spraying (state 1) it drains ammo and raycasts the spray; state 2 waits for a clear
// line, state 3 drives a carried target. Names are descriptive guesses; the code is matched against the
// retail DOL.

#include <dolphin/types.h>
#include <string.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_800F8AF4(int id);
int fn_800F7D28(int emitter, int id);
void fn_800F825C(int emitter, int id, int count);
void fn_800F86A0(int emitter);
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
int FxStart(void* fx, int a, int b);
void fn_8016EDD8(int handle, int* pos);
void fn_8016EEF4(int handle);
void fn_800E1098(void* out, void* in, float length);
int fn_800201E4(void* player, void* seg, int a, int b, int c);
int fn_8001A30C(int target, void* pos, void* delta, int a, int (*cb)(void*, void*), void* out, float range);
int fn_802A1D04(void);
void fn_80163824(int player, u32 id, int a, int b, int c);
void fn_8000F550(short* ids, int count);
int SendMsg(int target, int msg, void* data);
}

void* VAlloc(int size);
void VFree(void* p);
void Fn800F840C(int emitter, int a, int b);
void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
int Fn80175D60(unsigned long arg);
int AssetFind(u32 table, u32 key);
int PlayerGetPossessor(int arg);
void TextureLoad(short* ids, int count);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);
struct Player;
Player* GetPlayer(void);
void PlayerAnimOffUpdate(Player* p, float dt);

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    if (s == 0) {
        return 0;
    } else {
        u32 h = 0;
        int i;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
        return h;
    }
}

struct Vec3 {
    float x, y, z;
};

struct Mtx33 {
    float m[3][3];
};

struct Mtx34 {
    float m[3][4];
};

void MtxSetEuler(Mtx34* m, Vec3* rot);
void Vec3Set(Vec3* v, float x, float y, float z);

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
    ~String() {}
    virtual void Release(char* p);
};

struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

struct MsgResult {
    void* p;
    int unk4;
    u8 pad8[0x18];
};

struct ItemType {
    const char* name;
    int unk4[6];
    int (*msg)(int item, int msg, u32 owner, void* data);
    u32 (*id)(void);
    void (*update)(void);
    int unk28[2];
};

struct SpawnMsg {
    int unk0;
    u32 owner;
    int asset;
    int unkC;
    Mtx33 rot;
    Vec3 pos;
};

// ---- Foam ----

void FoamUpdate(void);
u32 FoamId(void);
int FoamMsg(int item, int msg, u32 owner, void* data);

ItemType gFoamType = { "Foam", { 0 }, FoamMsg, FoamId, FoamUpdate };

void FoamUpdate(void) {}

u32 FoamId(void) { return 0x3DA260F7; }

int FoamMsg(int item, int msg, u32 owner, void* data) {
    Fn80175D60(owner);
    AssetFind(0xF2E2BEB8, 0xFAC4DCB6);

    switch (msg) {
    case 0x96224FCC:
        if (data) {
            if (*((int**)data)[1] == -1) {
                QueryMsg q1;
                QueryMsg q2;
                SpawnMsg m;
                Vec3 rot;

                q1.hash = 0x00012D80;
                SendMsg(0x61000000, 0x41E, &q1);
                *((int**)data)[1] = *(int*)&q1.result;
                q2.hash = 0x03F08D2D;
                *(int*)&q2.result = 0;
                SendMsg(*((int**)data)[1], 0x41D, &q2);
                m.owner = owner;
                m.asset = AssetFind(0xF1754BF2, StrHash("WP_FIRE1"));
                m.unkC = 1;
                Vec3Set(&rot, -90.0f, 0.0f, 0.0f);
                MtxSetEuler((Mtx34*)&m.rot, &rot);
                m.pos.x = 0.0f;
                m.pos.y = 0.0f;
                m.pos.z = 0.0f;
                SendMsg(*((int**)data)[1], 0x415, &m);
            }
            if (*((int**)data)[1]) {
                QueryMsg q;
                q.hash = 0x7A281983;
                *(int*)&q.result = 1;
                SendMsg(*((int**)data)[1], 0x41D, &q);
            }
        }
        return 1;
    case 0x0D19E338:
        if (data) {
            if (*((int**)data)[1]) {
                QueryMsg q;
                q.hash = 0x7A281983;
                *(int*)&q.result = 0;
                SendMsg(*((int**)data)[1], 0x41D, &q);
                q.hash = 0x03F08D2D;
                *(int*)&q.result = 0;
                SendMsg(*((int**)data)[1], 0x41D, &q);
            }
        }
        return 1;
    case 0x00014967:
        if (*((int**)data)[1]) {
            QueryMsg q;
            q.hash = 0x03F08D2D;
            *(int*)&q.result = 1;
            SendMsg(*((int**)data)[1], 0x41D, &q);
        }
        return 1;
    }
    return 0;
}

// ---- Extinguisher ----

struct EffectObj {
    int unk0;
    int unk4;
    int emitter;
    void* data;
};

struct EffectType {
    const char* name;
    u32 hash;
    int unk8[3];
    void* (*create)(int a, int id);
    void (*destroy)(EffectObj* obj);
    int (*msg)(EffectObj* obj, int msg, int arg, void* data, float dt);
    u32 (*query)(EffectObj* obj, int msg);
    void (*update)(EffectObj* obj, float dt);
    void (*reset)(EffectObj* obj);
    int unk2C;
};

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}

struct ShotHit {
    u8 pad[0x30];
    float x, y, z;
    Vec3 dir;
    u8 pad48[0x18];
};

struct ExtinguisherDef {
    int unk00;
    int id;
    int max;
    u8 pad0C[0x14 - 0x0C];
    int unk14;
    int charge;
    int unk1C;
    int maxCharge;
    u8 pad24[0x30 - 0x24];
    void* fx;
    void* fx2;
    u8 pad38[0x48 - 0x38];
    int model;
    u8 pad4C[0x64 - 0x4C];
    int id2;
    int unk68;
    float speed;
    float unk70;
    u8 pad74[0x90 - 0x74];
    void* fx3;
    int unk94;
    int model2;
    float anim;
};

struct Extinguisher {
    int state;
    float timer;
    float duration;
    float charge;
    float unk10;
    float unk14;
    int emitter;
    ExtinguisherDef* def;
    int unk20;
    int arg;
    int pad28;
    ShotHit shot;
    u32 ammo;
    int target;
    int spraying;
    int fx;
    Vec3 carry;
};

struct SprayRef {
    ShotHit* shot;
    int* target;
};

struct Segment {
    Vec3 start;
    Vec3 end;
};

struct IVec3 {
    int v[3];
};

struct HitInfo {
    u8* obj;
    int unk4;
    int unk8;
    int unkC;
    IVec3 pos;
    IVec3 normal;
    IVec3 unk28;
};

struct SprayInfo {
    ShotHit* hit;
    float speed;
    float unk8;
    int unkC;
    int charge;
    int pad14[4];
};

inline float Min(float a, float b) { return a < b ? a : b; }

struct GrabMsg {
    int unk0;
    int asset;
    u8 pad08[0x34 - 0x08];
    int x, y, z;
};

struct PushMsg {
    u8* hit;
    void* target;
    int unk8;
    float force;
    int unk10;
    u32 hash;
    u8 pad18[0x58 - 0x18];
};

void ExtinguisherReset(EffectObj* obj);
void ExtinguisherUpdate(EffectObj* obj, float dt);
u32 ExtinguisherQuery(EffectObj* obj, int msg);
int ExtinguisherMsg(EffectObj* obj, int msg, int arg, void* data, float dt);
void ExtinguisherDestroy(EffectObj* obj);
void* ExtinguisherCreate(int a, int id);
int ExtinguisherHitCopy(void* hit, void* out);

EffectType gExtinguisherType = {
    "EXTINGUISHER", 0, {0, 0, 0}, ExtinguisherCreate, ExtinguisherDestroy, ExtinguisherMsg, ExtinguisherQuery,
    ExtinguisherUpdate, ExtinguisherReset, 0,
};

void ExtinguisherReset(EffectObj* obj) {
    Extinguisher* e = (Extinguisher*)obj->data;

    e->state = 0;
    e->timer = 0.0f;
    if (e->fx != -1) {
        fn_8016EEF4(e->fx);
        e->fx = -1;
    }
    if (e->target != -1) {
        QueryMsg q;
        q.hash = 0x7A281983;
        *(int*)&q.result = 0;
        SendMsg(e->target, 0x41D, &q);
    }
    e->spraying = 0;
}

static inline int ExtinguisherRay(Extinguisher* e, Segment* seg, Vec3* fwd, Vec3* back) {
    seg->start = *(Vec3*)&e->shot.x;
    seg->end = seg->start;
    *fwd = e->shot.dir;
    *back = *fwd;
    fn_800E1098(fwd, fwd, 50.0f);
    fn_800E1098(back, back, 200.0f);
    seg->end.x += fwd->x;
    seg->end.y += fwd->y;
    seg->end.z += fwd->z;
    seg->start.x -= back->x;
    seg->start.y -= back->y;
    seg->start.z -= back->z;
    return fn_800201E4(GetPlayer(), seg, 0, 0, 0);
}

void ExtinguisherUpdate(EffectObj* obj, float dt) {
    Extinguisher* e = (Extinguisher*)obj->data;
    ExtinguisherDef* def = e->def;
    PushMsg m;
    GrabMsg grab;
    HitInfo out;
    Segment seg1;
    Segment seg2;
    MsgResult t;
    Vec3 fwd1;
    Vec3 back1;
    int pos1[3];
    Vec3 fwd2;
    Vec3 back2;
    int pos2[3];
    Vec3 cur;
    Vec3 delta;
    SprayRef ref;
    SprayRef ref2;

    ref.shot = &e->shot;
    ref.target = &e->target;
    e->unk20 = 0;
    if (e->state == 1) {
        int hit = ExtinguisherRay(e, &seg1, &fwd1, &back1);
        int n = fn_800F7D28(e->emitter, def->id);
        int go = 1;

        if (e->ammo == 0x98C957B7) {
            if (n <= 2.2f * dt || hit) {
                Fn800F7F5C(e->emitter, def->id, 0x0D19E338, e->arg, &ref, dt);
                Fn800F7F5C(e->emitter, def->id, 0x00014967, e->arg, &ref, dt);
                Fn800F7F5C(e->emitter, def->id, 0x96224FCC, e->arg, &ref, dt);
                e->state = 2;
                go = 0;
                fn_8016EEF4(e->fx);
                pos1[0] = e->shot.x;
                pos1[1] = e->shot.y;
                pos1[2] = e->shot.z;
                e->fx = fn_8016F120(def->fx2, pos1);
            }
        } else if (e->ammo == 0xC8759B80 || e->ammo == 0xE762FC9C) {
            if (n > 1 && n <= (int)(2.2f * dt)) {
                Fn800F7F5C(e->emitter, def->id, 0x0D19E338, e->arg, &ref, dt);
                e->spraying = 0;
                go = 0;
                if (fn_802A1D04()) {
                    fn_800F825C(e->emitter, def->id, n);
                }
            }
        }
        if (go) {
            Fn800F7F5C(e->emitter, def->id, 0x96224FCC, e->arg, &ref, dt);
            e->spraying = 1;
            e->timer -= dt;
            if (e->ammo != 0xC8759B80 || e->ammo == 0xE762FC9C) {
                fn_800F825C(e->emitter, def->id, (int)(2.2f * dt));
            }
        }
    } else if (e->state == 2) {
        if (e->ammo == 0x98C957B7 && fn_800F7D28(e->emitter, def->id) > 2.2f * dt) {
            if (ExtinguisherRay(e, &seg2, &fwd2, &back2) == 0) {
                ref2.shot = &e->shot;
                ref2.target = &e->target;
                Fn800F7F5C(e->emitter, def->id, 0x0D19E338, e->arg, &ref2, dt);
                Fn800F7F5C(e->emitter, def->id, 0x96224FCC, e->arg, &ref2, dt);
                e->spraying = 1;
                e->state = 1;
                e->timer = e->duration;
                pos2[0] = e->shot.x;
                pos2[1] = e->shot.y;
                pos2[2] = e->shot.z;
                e->fx = fn_8016F120(def->fx, pos2);
                return;
            }
        }
    } else if (e->state == 3) {
        grab.asset = AssetFind(0xF1754BF2, 0x19D2D7D6);
        grab.unk0 = 0;
        SendMsg(e->arg, 0x411, &grab);
        cur.x = grab.x;
        cur.y = grab.y;
        cur.z = grab.z;
        delta.x = cur.x - e->carry.x;
        delta.y = cur.y - e->carry.y;
        delta.z = cur.z - e->carry.z;
        t.p = 0;
        SendMsg(e->arg, 0x400, &t);
        if (fn_8001A30C((int)t.p, &cur, &delta, 0, ExtinguisherHitCopy, &out, 100.0f) && out.obj) {
            m.hit = out.obj;
            m.force = 0.001f * (1000.0f * dt);
            m.hash = 0x03C5E451;
            m.target = t.p;
            m.unk8 = t.unk4;
            SendMsg(*(int*)(out.obj + 0x38), 0x407, &m);
        }
        e->carry.x = cur.x;
        e->carry.y = cur.y;
        e->carry.z = cur.z;
    }
    if ((e->state == 0 || e->state == 3) && e->ammo == 0x98C957B7 &&
        fn_800F7D28(e->emitter, def->id) < def->max) {
        Fn800F840C(e->emitter, def->id, (int)(0.7f * dt));
        e->unk20 = 1;
    }
}

int ExtinguisherHitCopy(void* hit, void* out) {
    HitInfo* s = (HitInfo*)hit;
    HitInfo* d = (HitInfo*)out;
    d->obj = s->obj;
    d->unk4 = s->unk4;
    d->unk8 = s->unk8;
    d->unkC = s->unkC;
    d->pos = s->pos;
    d->normal = s->normal;
    d->unk28 = s->unk28;
    return 1;
}

u32 ExtinguisherQuery(EffectObj* obj, int msg) {
    Extinguisher* e = (Extinguisher*)obj->data;
    ExtinguisherDef* def = e->def;

    switch (msg) {
    case 0x96224FCC:
    case 0x0020EE36:
        if (fn_800F7D28(e->emitter, def->id)) {
            return 0x04A3E183;
        }
        if (e->state == 2 && e->ammo == 0x98C957B7) {
            return 0x04A3E183;
        }
        return 0x11D2C025;
    case 0x0D19E338:
        if (e->state == 1 || e->state == 2) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
    case 0xF829DB93:
        if (fn_800F7D28(e->emitter, def->id2)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->id2)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0x9791AF51:
    case 0x42E50A3D:
        return 0x04A3E183;
    case 0x968CBE3D:
    case 0xE6B71BCC:
        if (fn_800F7D28(e->emitter, def->id) == def->max) {
            return 0x4848CA1E;
        }
        if (fn_800F7D28(obj->emitter, def->id)) {
            return 0x04A3E183;
        }
        return 0x11D2C025;
    }
    return 0x3DA260F7;
}

int ExtinguisherMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    Extinguisher* e = (Extinguisher*)obj->data;
    ExtinguisherDef* def = e->def;
    int busy = Fn80175D60(arg);
    SprayInfo info;
    GrabMsg grab;
    MsgResult q;
    short texs[9];
    int posA[3];
    int posB[3];
    int posC[3];
    SprayRef refA;
    SprayRef refB;
    short models[3];
    short tex;

    switch (msg) {
    case 0x0020EE36:
        return 1;
    case 0x96224FCC:
        memcpy(&e->shot, data, 0x60);
        e->arg = arg;
        if (e->state != 1) {
            refA.shot = &e->shot;
            refA.target = &e->target;
            Fn800F7F5C(e->emitter, def->id, 0x96224FCC, e->arg, &refA, dt);
            e->spraying = 1;
            e->state = 1;
            e->timer = e->duration;
            posA[0] = e->shot.x;
            posA[1] = e->shot.y;
            posA[2] = e->shot.z;
            e->fx = fn_8016F120(def->fx, posA);
        }
        return 1;
    case 0x315A1A7F:
        memcpy(&e->shot, data, 0x60);
        e->arg = arg;
        if (e->fx != -1) {
            posB[0] = e->shot.x;
            posB[1] = e->shot.y;
            posB[2] = e->shot.z;
            fn_8016EDD8(e->fx, posB);
        }
        return 1;
    case 0x0D19E338: {
        e->state = 0;
        refB.shot = &e->shot;
        refB.target = &e->target;
        Fn800F7F5C(e->emitter, def->id, 0x0D19E338, e->arg, &refB, dt);
        e->spraying = 0;
        fn_8016EEF4(e->fx);
        e->fx = -1;
        return 1;
    }
    case 0xF829DB93: {
        float* p;
        void* fx;
        int h;

        e->unk20 = 1;
        if (!fn_800F7D28(e->emitter, def->id2)) {
            return 0;
        }
        e->charge += def->charge;
        e->charge = Min(e->charge, def->maxCharge);
        if (((arg >> 24) & 0xFF) == 2) {
            SendMsg(arg, 0x400, &q);
            PlayerAnimOffUpdate((Player*)q.p, def->anim);
            e->unk10 += def->anim;
            e->unk14 = 300.0f;
        }
        info.hit = (ShotHit*)data;
        info.speed = def->speed;
        info.unk8 = def->unk70;
        info.unkC = def->unk14;
        info.charge = e->charge;
        Fn800F7F5C(e->emitter, def->id2, 0x14967, arg, &info, dt);
        fx = def->fx3;
        p = &info.hit->x;
        if (p) {
            posC[0] = p[0];
            posC[1] = p[1];
            posC[2] = p[2];
            h = fn_8016F120(fx, posC);
            fn_8016F118(h, 0, 100000);
        } else {
            h = FxStart(fx, 0, 0);
        }
        e->fx = h;
        return 1;
    }
    case 0x42E50A3D:
        if (e->state != 3) {
            e->state = 3;
            grab.asset = AssetFind(0xF1754BF2, 0x19D2D7D6);
            grab.unk0 = 0;
            SendMsg(e->arg, 0x411, &grab);
            e->carry.x = grab.x;
            e->carry.y = grab.y;
            e->carry.z = grab.z;
        }
        return 1;
    case 0x9791AF51:
        e->state = 0;
        return 1;
    case 0x968CBE3D:
    case 0xE6B71BCC: {
        int n;
        int m;
        int r;

        e->unk20 = 1;
        n = fn_800F7D28(e->emitter, def->id);
        if (n == def->max) {
            return 0;
        }
        m = fn_800F7D28(obj->emitter, def->id);
        if (m == 0) {
            return 0;
        }
        n = (m < def->max - n) ? m : def->max - n;
        {
            QueryMsg q;
            q.hash = 0x5A32795A;
            *(int*)&q.result = 0;
            q.unk8 = 0;
            SendMsg(arg, 0x41E, &q);
            r = *(int*)&q.result;
        }
        if (r == 0) {
            fn_800F825C(obj->emitter, def->id, n);
        }
        Fn800F840C(e->emitter, def->id, n);
        return 1;
    }
    case 0x8FDD7599:
        e->unk20 = 1;
        return 1;
    case 0x0B59E861:
        if (busy == 0) {
            int player = PlayerGetPossessor(arg);
            int n = fn_800F7D28(e->emitter, def->id);
            if (n < 0) {
                n = 0;
            }
            fn_80163824(player, 0x878DC2AD, -1, (int)(100.0f * ((float)n / (float)def->max)), 100);
        }
        return 1;
    case 0x7FAB1247:
        e->unk20 = 1;
        return 1;
    case 0xE452286F:
        if (e->ammo == 0x98C957B7) {
            tex = AssetFind(0xF2E2BEB8, 0xFAC4DCB6);
            if ((int)data == 1) {
                TextureLoad(&tex, 1);
            } else {
                fn_8000F550(&tex, 1);
            }
        } else {
            texs[0] = AssetFind(0xF2E2BEB8, 0x0F6298F9);
            texs[1] = AssetFind(0xF2E2BEB8, 0x5541EDD2);
            texs[2] = AssetFind(0xF2E2BEB8, 0x56F6C671);
            texs[3] = AssetFind(0xF2E2BEB8, 0x58AB9F10);
            texs[4] = AssetFind(0xF2E2BEB8, 0x5A6077AF);
            texs[5] = AssetFind(0xF2E2BEB8, 0x2E20AEE7);
            texs[6] = AssetFind(0xF2E2BEB8, 0x961C77AB);
            texs[7] = AssetFind(0xF2E2BEB8, 0x42F9014E);
            texs[8] = AssetFind(0xF2E2BEB8, 0x5C1B2515);
            models[0] = AssetFind(0x87DE5E6A, 0x6C64233F);
            models[1] = def->model;
            models[2] = def->model2;
            if ((int)data != 0) {
                TextureLoad(texs, 9);
                ModelLoadIds(models, 3);
            } else {
                fn_8000F550(texs, 9);
                ModelFreeIds(models, 3);
            }
        }
        return 1;
    case 0x141109BD:
        return 1;
    }
    return 0;
}

void ExtinguisherDestroy(EffectObj* obj) {
    Extinguisher* e = (Extinguisher*)obj->data;
    fn_800F86A0(e->emitter);
    VFree(e);
}

void* ExtinguisherCreate(int a, int id) {
    EffectType* type = &gExtinguisherType;
    Extinguisher* g;

    EffectTypeHash(type);
    g = (Extinguisher*)VAlloc(sizeof(Extinguisher));
    memset(g, 0, sizeof(Extinguisher));
    if (g == 0) {
        return 0;
    }
    g->def = (ExtinguisherDef*)fn_800EB21C(id);
    g->duration = 100.0f;
    g->target = -1;
    g->spraying = 0;
    g->fx = -1;
    g->state = 0;
    g->timer = 0.0f;
    g->unk20 = 1;
    g->ammo = fn_800F8AF4(id);
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->def->id, g->def->max);
    Fn800F840C(g->emitter, g->def->id2, g->def->unk68);
    return g;
}


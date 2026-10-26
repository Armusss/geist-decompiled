// u_8020A650.cpp: callback timer, name tables and model building for one actor type (0x8020A650-0x8020B77C).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned short u16;

struct Vec3 {
    float x, y, z;
};

struct Quat {
    float x, y, z, w;
};

struct Mtx34 {
    float m[12];
};

// One 0x1C-byte animation frame: orientation followed by position.
struct u8020A650Anim {
    Quat rot;                   // 0x00
    Vec3 pos;                   // 0x10
};

// One entry of the callback list owned by u8020A650Owner.
struct u8020A650Entry {
    int field00;                // 0x00
    int field04;                // 0x04
    Quat quat;                  // 0x08
    Vec3 pos;                   // 0x18
    void* cb;                   // 0x24
};

// Object owning a countdown timer and a list of callback entries.
struct u8020A650Owner {
    u8 unk00[0x4];
    short modelId;              // 0x4
    u16 count;                  // 0x6
    int modelIdx;               // 0x8
    u8020A650Entry* entries;    // 0xC
    void* ctx;                  // 0x10
    u8* slotMap;                // 0x14
    u8 unk18[0x4];
    float timer;                // 0x1C
};

extern "C" void fn_801D5448(void* cb);

// Ticks the timer down; once it runs out, releases every listed entry.
void TickCallbackTimer(float amount, u8020A650Owner* o)
{
    if (o->timer > 0.0f) {
        o->timer -= amount;
        if (o->timer <= 0.0f && o->entries != 0) {
            for (int i = 1; i < o->count; i++) {
                if (o->entries[i].cb != 0) {
                    fn_801D5448(o->entries[i].cb);
                }
            }
        }
    }
}

// Two vtables of the class hierarchy this object belongs to.
extern void* lbl_803BB8CC;
extern void* lbl_803ADE00;

void* VAlloc(int size);
void VFree(void* p);

// Tears the object down (derived vtable, virtual cleanup, base vtable) and
// optionally frees the storage.
void* DestroyVtObject(void* p, short flag)
{
    if (p != 0) {
        *(void**)p = &lbl_803BB8CC;
        (*(void (**)(void*))(*(char**)p + 0x24))(p);
        if (p != 0) {
            *(void**)p = &lbl_803ADE00;
        }
        if (flag > 0) {
            VFree(p);
        }
    }
    return p;
}

// Upper-case table used by the lazy name hash.
extern unsigned char __upper_map[256];

// One model slot: cached hash of its name plus the name itself, and the
// ranges of the body-part / attachment tables it uses.
struct u8020A650ModelName {
    unsigned long hash;             // 0x00
    unsigned short numParts;        // 0x04
    unsigned short partSet;         // 0x06
    unsigned short attachCount;     // 0x08
    unsigned short attachIndex;     // 0x0A
    char* name;                     // 0x0C
};

// One body-part slot: two name hashes and the assets they resolved to.
struct u8020A650PartName {
    unsigned long hash;             // 0x00
    unsigned long asset;            // 0x04
    unsigned long hash2;            // 0x08
    unsigned long asset2;           // 0x0C
    int id;                         // 0x10
    float f14;                      // 0x14
    float f18;                      // 0x18
    char* name;                     // 0x1C
    char* name2;                    // 0x20
};

// One attachment slot: two names and the assets they resolved to.
struct u8020A650AttachName {
    unsigned long asset;            // 0x00
    unsigned long asset2;           // 0x04
    int type;                       // 0x08
    float f0C;                      // 0x0C
    float f10;                      // 0x10
    float f14;                      // 0x14
    float f18;                      // 0x18
    float f1C;                      // 0x1C
    float f20;                      // 0x20
    float f24;                      // 0x24
    float f28;                      // 0x28
    unsigned char unk2C[0x4];       // 0x2C
    float f30;                      // 0x30
    float f34;                      // 0x34
    char* name;                     // 0x3C
    char* name2;                    // 0x40
};

// The name tables initialised below, laid out back to back.
struct u8020A650NameTables {
    unsigned char unk000[0x10C];
    u8020A650ModelName models[3];       // 0x10C
    u8020A650PartName parts[0x26];      // 0x13C
    u8020A650AttachName attach[0x20];   // 0x694
};

extern u8020A650NameTables lbl_803BA9B8;
extern unsigned char lbl_804D53A0;

extern "C" unsigned long AssetFind(unsigned long kind, unsigned long hash);

// Lazy string hash used by every name table: 31x multiply, upper-cased
// characters, with 0xFF meaning "keep -1".
static inline unsigned long HashName(const char* name)
{
    unsigned long hash = 0;
    if (name != 0) {
        for (; *name != 0; name++) {
            unsigned long c;
            if (*name == -1) {
                c = 0xFFFFFFFF;
            } else {
                c = __upper_map[(unsigned char)*name];
            }
            hash = hash * 31 + (c & 0xFF);
        }
        hash %= 0xFFFFFFFF;
    }
    return hash;
}

// Fills in the cached name hashes and asset ids of the model, body-part and
// attachment name tables.
void u8020A650InitNameTables(void)
{
    u8020A650NameTables* t = &lbl_803BA9B8;
    int i;

    for (i = 3; i != 0; i--) {
        u8020A650ModelName* m = t->models + (3 - i);
        m->hash = HashName(m->name);
    }
    for (i = 0; i < 0x26; i++) {
        u8020A650PartName* p = t->parts + i;
        p->hash = HashName(p->name);
        p->asset = AssetFind(0xF1754BF2, p->hash);
        p->hash2 = HashName(p->name2);
        p->asset2 = AssetFind(0xF1754BF2, p->hash2);
    }
    for (i = 0; i < 0x20; i++) {
        u8020A650AttachName* a = t->attach + i;
        a->asset = AssetFind(0xF1754BF2, HashName(a->name));
        a->asset2 = AssetFind(0xF1754BF2, HashName(a->name2));
    }
    lbl_804D53A0 = 0;
}

// Constants and globals shared with the rest of the game code.
extern float lbl_804D9388;      // 0.0f
extern float lbl_804D938C;
extern float lbl_804D9390;
extern float lbl_804D9394;
extern float lbl_804D9398;
extern float lbl_804D939C;
extern float lbl_804D93A0;
extern float lbl_804D93A4;
extern int lbl_804D33B8;

extern "C" float fabsf(float);
extern "C" float sqrtf(float);

// SDK vector helpers (plain C linkage in the retail binary).
extern "C" void PSQUATMultiply(Quat* a, Quat* b, Quat* out);
extern "C" void PSQUATNormalize(Quat* q, Quat* out);

// Model / skeleton helpers.
void ModelAnimate(int modelId, char* frameBuf, int mode);
void ModelSetMatrix(int modelId, Mtx34* mtx, char* bones);
void BoneGetMatrix(Mtx34* mtx, void* rot);
void Mtx34StoreTranslation(Mtx34* mtx, Vec3* v);
void QuatRotateVec(Vec3* out, float* q, Vec3* in);
void QuatRotateVecNorm(Vec3* out, float* q, Vec3* in);

// Actor-part helpers, still only known by address.
extern "C" void* fn_801D54E8(void);
extern "C" void* fn_801D5B38(float value, int zero, int asset, int id,
                              unsigned long hash, void* ctx, Vec3* out, int again);
extern "C" void fn_801D52E0(void* cb);
extern "C" void fn_801D5298(void* cb, Vec3* v);
extern "C" void fn_801D5260(void* cb, Vec3* v);
extern "C" void fn_801D5110(void* cb, int value);
extern "C" void fn_801D556C(void* cb);
extern "C" void* fn_801D4C34(void);
extern "C" void fn_801D4B5C(void* ctx, void* cb, Vec3* v);
extern "C" void fn_801D4824(void* ctx, void* cbA, void* cbB, Vec3* v1,
                             Vec3* v2, int count, Mtx34* mtx);
extern "C" void fn_801D513C(void* cb, void* out);
extern "C" void fn_801D4A70(void* ctx, void* a, void* b, void* out, Vec3* v,
                             void* extra);
extern "C" void fn_800A0168(Mtx34* mtx, Mtx34* out);
extern "C" void fn_800A0030(Vec3* v, Mtx34* mtx, Vec3* out);
extern "C" unsigned short* fn_80037B6C(int modelId);
extern "C" int fn_80037930(int modelId);

// Builds the runtime representation of one body: allocates a callback per
// body part, places it on the animated skeleton and links the attachments.
int u8020A650BuildModel(u8020A650Owner* owner, int setIndex, u8* slotMap,
                        u8020A650Anim* anims, Mtx34* mtx)
{
    u8020A650ModelName* set = &lbl_803BA9B8.models[setIndex];
    u8020A650PartName* parts = &lbl_803BA9B8.parts[set->partSet];
    u8020A650AttachName* attach = &lbl_803BA9B8.attach[set->attachIndex];
    char animBuf[0x1C0C];
    u8020A650Anim* frames = (u8020A650Anim*)animBuf;
    unsigned short numParts;
    void* ctx;
    int i;

    owner->entries[0].cb = 0;
    owner->entries[0].quat = anims[0].rot;
    owner->entries[0].pos = anims[0].pos;

    numParts = lbl_803BA9B8.models[owner->modelIdx].numParts;

    ModelAnimate(owner->modelId, animBuf, 1);
    frames[0].pos.x = 0.0f;
    frames[0].pos.y = 0.0f;
    frames[0].pos.z = 0.0f;
    ModelSetMatrix(owner->modelId, mtx, animBuf);
    ctx = fn_801D54E8();

    for (i = 1; i < numParts; i++) {
        u8020A650PartName* part = &parts[i];
        int slot = slotMap[part->asset];
        int slot2 = slotMap[part->asset2];

        if (slot != 0xFF && slot2 != 0xFF) {
            Vec3 delta;
            Vec3 axis;
            float scale;
            float limit;

            owner->slotMap[i] = (u8)slot;
            scale = lbl_804D938C * part->f14;
            delta.x = frames[slot2].pos.x - frames[slot].pos.x;
            delta.y = frames[slot2].pos.y - frames[slot].pos.y;
            delta.z = frames[slot2].pos.z - frames[slot].pos.z;
            axis.x = 0.0f;
            axis.y = 0.0f;
            axis.z = lbl_804D9390 * (lbl_804D938C * part->f14 + part->f18);
            limit = lbl_804D9394;
            if (fabsf(scale) >= limit) {
                if (fabsf(scale) >= limit && fabsf(part->f18) >= limit) {
                    unsigned long hash;
                    void* handle;

                    if (lbl_804D33B8 == 0) {
                        hash = HashName("RED OAK");
                    } else {
                        hash = 0xBF4C00C0;
                    }
                    handle = fn_801D5B38(scale, 0, part->asset, part->id, hash,
                                         ctx, &axis, 0);
                    owner->entries[slot].cb = handle;

                    if (owner->entries[slot].cb != 0) {
                        u8020A650Entry* e = &owner->entries[slot];
                        void* cb = e->cb;
                        Vec3 dir = delta;
                        Quat q;
                        Mtx34 partMtx;
                        Vec3 zero;
                        float inv;
                        float t;

                        if (fabsf(dir.x) > limit || fabsf(dir.y) > limit ||
                            fabsf(dir.z) > limit) {
                            inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y +
                                               dir.z * dir.z);
                            dir.x *= inv;
                            dir.y *= inv;
                            dir.z *= inv;
                        } else {
                            dir.x = 0.0f;
                            dir.y = 0.0f;
                            dir.z = 0.0f;
                        }

                        t = lbl_804D938C * dir.z + lbl_804D938C;
                        if (t > 0.0f) {
                            float len = sqrtf(t);
                            q.x = -(lbl_804D9398 * dir.y) / len;
                            q.y = -(lbl_804D9398 * dir.x) / len;
                            q.z = 0.0f;
                            q.w = lbl_804D9390 * len;
                            PSQUATNormalize(&q, &q);
                        } else {
                            q.x = 0.0f;
                            q.y = 0.0f;
                            q.z = 0.0f;
                            q.w = lbl_804D9398;
                        }

                        BoneGetMatrix(&partMtx, &q);
                        Mtx34StoreTranslation(&partMtx, &frames[slot].pos);
                        PSQUATMultiply(&frames[slot].rot, &q, &e->quat);
                        e->quat.x = -e->quat.x;
                        e->quat.y = -e->quat.y;
                        e->quat.z = -e->quat.z;

                        fn_801D52E0(cb);
                        zero.x = 0.0f;
                        zero.y = 0.0f;
                        zero.z = 0.0f;
                        fn_801D5298(cb, &zero);
                        fn_801D5260(cb, &zero);
                        fn_801D5110(cb, 0);
                        fn_801D556C(cb);
                    }
                    owner->entries[slot].field04 = part->hash;
                }
            }
        }
    }

    owner->ctx = fn_801D4C34();
    {
        unsigned short* list = fn_80037B6C(owner->modelId);

        for (i = 0; i < owner->count; i++) {
            int slot = slotMap[list[i]];
            int slot2;
            Vec3 delta;
            Quat inv;

            if (slot == 0xFF) {
                continue;
            }
            if (owner->entries[slot].cb != 0) {
                continue;
            }
            slot2 = fn_80037930(owner->modelId);
            if (slot2 == -1) {
                continue;
            }
            slot2 = slotMap[slot2];
            owner->entries[slot].field00 = slot2;
            delta.x = frames[slot].pos.x - frames[slot2].pos.x;
            delta.y = frames[slot].pos.y - frames[slot2].pos.y;
            delta.z = frames[slot].pos.z - frames[slot2].pos.z;
            owner->entries[slot].pos = delta;
            QuatRotateVec(&owner->entries[slot].pos, &frames[slot2].rot.x,
                          &owner->entries[slot].pos);
            inv.x = -frames[slot2].rot.x;
            inv.y = -frames[slot2].rot.y;
            inv.z = -frames[slot2].rot.z;
            inv.w = frames[slot2].rot.w;
            PSQUATMultiply(&frames[slot].rot, &inv, &owner->entries[slot].quat);
        }
    }

    {
        void* scratch = VAlloc(0x2000);
        if (scratch == 0) {
            return 0;
        }
        VFree(scratch);
    }

    for (i = 0; i < set->attachCount; i++) {
        u8020A650AttachName* a = &attach[i];
        int slotA = slotMap[a->asset];
        int slotB = slotMap[a->asset2];
        Vec3 v1;
        Vec3 v2;
        Quat inv;
        char tmp[0x48];

        if (slotA == 0xFF || slotB == 0xFF) {
            continue;
        }
        if (owner->entries[slotA].cb == 0) {
            continue;
        }
        if (owner->entries[slotB].cb == 0) {
            continue;
        }

        switch (a->type) {
        case 0: {
            Mtx34 attachMtx;

            v1.x = a->f0C;
            v1.y = a->f10;
            v1.z = a->f14;
            v2.x = a->f18;
            v2.y = a->f1C;
            v2.z = a->f20;

            fn_801D4B5C(owner->ctx, owner->entries[slotA].cb, &frames[slotA].pos);
            if (owner->entries[slotA].cb != 0) {
                Mtx34 boneMtx;

                BoneGetMatrix(&boneMtx, &frames[slotA].rot);
                fn_800A0168(&boneMtx, &boneMtx);
                fn_800A0030(&v1, &boneMtx, &v1);
                fn_800A0030(&v2, &boneMtx, &v2);
            } else {
                inv.x = -owner->entries[slotA].quat.x;
                inv.y = -owner->entries[slotA].quat.y;
                inv.z = -owner->entries[slotA].quat.z;
                inv.w = owner->entries[slotA].quat.w;
                QuatRotateVecNorm(&v1, &inv.x, &v1);
                QuatRotateVecNorm(&v2, &inv.x, &v2);
            }

            attachMtx.m[0] = a->f24;
            attachMtx.m[1] = a->f28;
            attachMtx.m[2] = lbl_804D939C;
            attachMtx.m[3] = 0.0f;
            attachMtx.m[4] = lbl_804D93A0;
            attachMtx.m[5] = lbl_804D93A4;
            attachMtx.m[6] = lbl_804D939C;
            attachMtx.m[7] = 0.0f;
            attachMtx.m[8] = a->f30;
            attachMtx.m[9] = a->f34;
            attachMtx.m[10] = lbl_804D939C;
            attachMtx.m[11] = 0.0f;
            fn_801D4824(owner->ctx, owner->entries[slotA].cb,
                        owner->entries[slotB].cb, &v1, &v2, 3, &attachMtx);
            break;
        }
        case 1: {
            v1.x = a->f0C;
            v1.y = a->f10;
            v1.z = a->f14;

            if (owner->entries[slotA].cb != 0) {
                Mtx34 boneMtx;

                BoneGetMatrix(&boneMtx, &frames[slotA].rot);
                fn_800A0168(&boneMtx, &boneMtx);
                fn_800A0030(&v1, &boneMtx, &v1);
            } else {
                inv.x = -owner->entries[slotA].quat.x;
                inv.y = -owner->entries[slotA].quat.y;
                inv.z = -owner->entries[slotA].quat.z;
                inv.w = owner->entries[slotA].quat.w;
                QuatRotateVecNorm(&v1, &inv.x, &v1);
            }

            if (owner->entries[slotA].cb != 0) {
                fn_801D513C(owner->entries[slotA].cb, tmp);
                fn_801D4A70(owner->ctx, owner->entries[slotA].cb,
                            owner->entries[slotB].cb, tmp + 0x24, &v1, &a->f24);
            } else {
                fn_801D4A70(owner->ctx, &inv, owner->entries[slotB].cb,
                            &owner->entries[slotA].pos, &v1, &a->f24);
            }
            break;
        }
        }
    }

    for (i = 1; i < owner->count; i++) {
        int slot;
        Vec3 delta;
        Quat inv;

        if (owner->entries[i].cb != 0) {
            continue;
        }
        slot = owner->entries[i].field00;
        delta.x = anims[i].pos.x - anims[slot].pos.x;
        delta.y = anims[i].pos.y - anims[slot].pos.y;
        delta.z = anims[i].pos.z - anims[slot].pos.z;
        owner->entries[i].pos = delta;
        QuatRotateVec(&owner->entries[i].pos, &anims[slot].rot.x,
                      &owner->entries[i].pos);
        inv.x = -anims[slot].rot.x;
        inv.y = -anims[slot].rot.y;
        inv.z = -anims[slot].rot.z;
        inv.w = anims[slot].rot.w;
        PSQUATMultiply(&anims[i].rot, &inv, &owner->entries[i].quat);
    }

    return 1;
}

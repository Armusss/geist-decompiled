// u_8020A650.cpp: the body/sound-source owner of one actor type (0x80209C30-0x8020B77C): sound triggers
// and source lists, mode setup, callback timer, name tables and model building.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

// One 0x1C-byte bone frame: orientation followed by position.
struct u8020A650Anim {
    Quat rot;                   // 0x00
    Vec3 pos;                   // 0x10
};

// One 0x28-byte entry of the owner's per-bone list: the parent bone index, the
// name hash it was registered under, its rest transform and its live source.
struct u8020A650Entry {
    s32 index;                  // 0x00: parent transform index in the output array
    u32 nameHash;               // 0x04
    u8020A650Anim rest;         // 0x08: rest transform
    void* source;               // 0x24: live source, 0 for an unused entry
};

// The owner object: a real virtual class (vptr at 0, Stop at vtable +0x24 and
// Activate at +0x28) with a countdown timer and one entry per model bone.
class u8020A650Owner {
public:
    virtual ~u8020A650Owner();
    virtual void Virtual0C();
    virtual void Virtual10();
    virtual void Virtual14();
    virtual void Virtual18();
    virtual void Virtual1C();
    virtual void Virtual20();
    virtual void Stop();                    // 0x24
    virtual void Activate(int flag);        // 0x28

    short modelId;                  // 0x04
    u16 count;                      // 0x06: bone/entry count
    s32 mode;                       // 0x08: index into the model name table
    u8020A650Entry* entries;        // 0x0C
    void* ctx;                      // 0x10
    u8* slotMap;                    // 0x14: per-mode buffer
    u8* boneSets;                   // 0x18: bone-set record of the mode
    float timer;                    // 0x1C
};

// Upper-case table used by the name hashes.
extern unsigned char __upper_map[256];
static inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

void* VAlloc(int size);
void VFree(void* p);
extern "C" void PSQUATMultiply(Quat* a, Quat* b, Quat* out);
void Mtx34StoreTranslation(Mtx34* mtx, Vec3* v);
void QuatRotateVec(Vec3* out, float* q, Vec3* in);
extern "C" void fn_801D5110(void* cb, int value);
extern "C" void fn_801D513C(void* cb, void* out);
extern "C" void fn_801D5448(void* cb);
int u8020A650BuildModel(u8020A650Owner* owner, int mode, u8* data,
                        u8020A650Anim* anim, Mtx34* mtx);

// The sound-list view of the same owner object (the functions below keep this
// name in their signatures): the entry array is walked with a byte offset.
// The entry stride is 0x28 and entry 0 is
// skipped; each entry keeps the name hash it was registered under at +0x04 and
// the source object at +0x24.
struct SoundList {
    u8 pad00[0x06];
    unsigned short count;       // 0x06: entry count
    u8 pad08[0x04];
    u8* entries;                // 0x0C: entry array
    void* field10;              // 0x10: secondary source
    void* field14;              // 0x14: extra buffer
    u8 pad18[0x04];
    float timer;                // 0x1C: retrigger delay written on every trigger
};

extern Vec3 lbl_8049E130[];     // 0x14 recent trigger positions
extern int lbl_804D53A0;        // number of recent trigger positions
extern float lbl_804D9378;      // trigger range scale
extern float lbl_804D937C;      // retrigger delay
extern const char* lbl_803BAA90[];  // per-mode name strings

inline u32 StrHash(const char* s)
{
    if (s == 0)
    {
        return 0;
    }
    else
    {
        u32 h = 0;
        int i;
        for (i = 0; s[i] != 0; i++)
        {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
        return h;
    }
}

extern "C" int fn_801D60A0(int flag);
extern "C" int fn_801D4738(void* source, Vec3* pos, float seek);
extern "C" void fn_801D5010(void* source, void* param);
extern "C" void fn_801D55CC(void* source);
extern "C" void fn_801D4C0C(void* source);
extern "C" int fn_801D549C(void* source);

// Triggers the sources of a list around the given position: mode 0 tests each
// entry for range, mode 1 scales the working vector, and any other mode filters
// the entries by the hash of that mode's name.
static inline void Vec3Init(Vec3* v, float x, float y, float z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void BatsqueekTriggerModeSounds(SoundList* list, float seek, Vec3* pos, Vec3* vec, int mode)
{
    Vec3 local;

    if (list->entries == 0)
        return;

    Vec3Init(&local, vec->x, vec->y, vec->z);

    if (fn_801D60A0(1) != 0 && lbl_804D53A0 < 0x14)
        Vec3Init(&lbl_8049E130[lbl_804D53A0++], pos->x, pos->y, pos->z);

    if (mode == 0)
    {
        int hits = 0;
        int i;

        for (i = 1; i < list->count; i++)
        {
            if (((u8020A650Entry*)list->entries)[i].source != 0 &&
                fn_801D4738(((u8020A650Entry*)list->entries)[i].source, pos, seek) != 0)
            {
                hits++;
                fn_801D5010(((u8020A650Entry*)list->entries)[i].source, &local);
            }
        }

        if (hits == 0)
        {
            local.x *= 0.03125f;
            local.y *= 0.03125f;
            local.z *= 0.03125f;
            int i;

            for (i = 1; i < list->count; i++)
            {
                if (((u8020A650Entry*)list->entries)[i].source != 0)
                    fn_801D5010(((u8020A650Entry*)list->entries)[i].source, &local);
                }
        }
    }
    else if (mode == 1)
    {
        local.x *= 0.03125f;
        local.y *= 0.03125f;
        local.z *= 0.03125f;
        int i;

        for (i = 1; i < list->count; i++)
        {
            if (((u8020A650Entry*)list->entries)[i].source != 0)
                fn_801D5010(((u8020A650Entry*)list->entries)[i].source, vec);
        }
    }
    else
    {
        u32 hash = StrHash(lbl_803BAA90[mode]);
        int i;
        int off;

        for (i = 1, off = 0x28; i < list->count; i++)
        {
            u8020A650Entry* entry = (u8020A650Entry*)(list->entries + off);

            if (entry->source != 0 && hash == entry->nameHash)
                fn_801D5010(entry->source, vec);
            off += 0x28;
        }
    }

    list->timer = 10000.0f;
}

// Finds the sound-list entry registered under the given name hash and hands its
// source object to the trigger routine; if nothing matches, restarts the delay.
void SoundListTriggerEntry(SoundList* list, u32 hash, void* param)
{
    u8020A650Entry* entries = (u8020A650Entry*)list->entries;

    if (entries != 0)
    {
        for (int i = 1; i < list->count; i++)
        {
            if (((u8020A650Entry*)list->entries)[i].source != 0 &&
                hash == ((u8020A650Entry*)list->entries)[i].nameHash)
            {
                fn_801D5010(entries[i].source, param);
                return;
            }
        }
        list->timer = lbl_804D937C;
    }
}

// Releases every live source held by the list.
void SoundListStopAll(SoundList* list)
{
    if (list->entries != 0)
    {
        int off;
        int i;

        for (i = 1, off = 0x28; i < list->count; i++)
        {
            void* source = *(void**)(list->entries + off + 0x24);

            if (source != 0)
                fn_801D5110(source, 1);
            off += 0x28;
        }
    }
}

// Releases every live source held by the list, then frees the entry array, the
// secondary source and the extra buffer, if they exist.
void SoundListTeardown(SoundList* list)
{
    if (list->entries != 0)
    {
        int off;
        int i;

        for (i = 1, off = 0x28; i < list->count; i++)
        {
            void* source = *(void**)(list->entries + off + 0x24);

            if (source != 0)
                fn_801D55CC(source);
            off += 0x28;
        }

        VFree(list->entries);
        fn_801D4C0C(list->field10);
    }

    if (list->field14 != 0)
        VFree(list->field14);
}

// Returns non-zero when none of the live sources of the list is still marked
// active; a single active source makes the whole list report "not finished".
int SoundListSourcesFinished(SoundList* list)
{
    int off;
    int i;

    if (list->entries == 0)
        return 1;

    i = 1;
    off = 0x28;
    for (; i < list->count; i++)
    {
        void* source = *(void**)(list->entries + off + 0x24);

        if (source != 0 && fn_801D549C(source) != 0)
            return 0;
        off += 0x28;
    }

    return 1;
}

extern "C" void fn_801D53D4(void* source);
extern "C" void fn_801D550C(void* source);



void BoneSetMatrix(void* bone, Mtx34* matrix);
void Mtx34GetTranslation(Vec3* out, Mtx34* matrix);
void QuatRotateVec(Vec3* out, float* quat, Vec3* vec);


// Updates every live source of the list: when 'mode' is non-zero the sources are
// paused, otherwise they are resumed and re-registered with their source object.
void SoundListApplySourceMode(SoundList* list, int mode)
{
    if (list->entries == 0)
        return;

    int off;
    int i;

    for (i = 1, off = 0x28; i < list->count; i++)
    {
        void* source = *(void**)(list->entries + off + 0x24);

        if (source != 0)
        {
            if (mode != 0)
            {
                fn_801D53D4(source);
            }
            else
            {
                fn_801D550C(source);
                fn_801D5448(*(void**)(list->entries + off + 0x24));
            }
        }

        off += 0x28;
    }
}

// Builds the per-source transforms of the list into 'out': entry 0 supplies the
// base transform, every following entry either takes its live bone matrix or is
// rotated by the transform of the entry its index points at.
// Conjugate (inverse rotation) of a unit quaternion.
static inline void QuatConjugate(Quat* out, Quat* q)
{
    out->x = -q->x;
    out->y = -q->y;
    out->z = -q->z;
    out->w = q->w;
}

static inline void Vec3AddTo(Vec3* a, Vec3* b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}

void SoundListBuildSourceMatrices(SoundList* list, int arg2, int arg3, Mtx34* out)
{
    u8020A650Anim* frames = (u8020A650Anim*)out;
    Quat quat;
    Mtx34 matrix;

    if (list->entries == 0)
        return;

    frames->pos = ((u8020A650Entry*)list->entries)->rest.pos;
    frames->rot = ((u8020A650Entry*)list->entries)->rest.rot;

    int off;
    u8020A650Anim* cur;
    int i;
    for (i = 1, off = 0x28, cur = frames + 1; i < list->count; i++)
    {
        if (((u8020A650Entry*)(list->entries + off))->source != 0)
        {
            fn_801D513C(((u8020A650Entry*)(list->entries + off))->source, &matrix);
            BoneSetMatrix(cur, &matrix);
            PSQUATMultiply(&cur->rot, &((u8020A650Entry*)(list->entries + off))->rest.rot, &cur->rot);

            cur->rot.x = -cur->rot.x;
            cur->rot.y = -cur->rot.y;
            cur->rot.z = -cur->rot.z;

            Mtx34GetTranslation(&cur->pos, &matrix);
        }
        else
        {
            PSQUATMultiply(&((u8020A650Entry*)(list->entries + off))->rest.rot, &frames[((u8020A650Entry*)(list->entries + off))->index].rot, &cur->rot);
            QuatConjugate(&quat, &frames[((u8020A650Entry*)(list->entries + off))->index].rot);
            QuatRotateVec(&cur->pos, (float*)&quat, &((u8020A650Entry*)(list->entries + off))->rest.pos);
            Vec3AddTo(&cur->pos, &frames[((u8020A650Entry*)(list->entries + off))->index].pos);
        }

        off += 0x28;
        cur++;
    }

    frames->rot = frames[1].rot;
    frames->pos = frames[1].pos;
}

// Per-mode setup record of the owner: the mode hash the caller passes, the size
// of the mode buffer and the index of the bone-set table this mode uses.
struct BatsqueekOwnerMode {
    u32 hash;                   // 0x00
    u16 bufferSize;             // 0x04
    u16 boneSetIndex;           // 0x06
    u8 pad08[0x08];
};
extern BatsqueekOwnerMode lbl_803BAAC4[];   // three records

// 0x24-byte bone-set records referenced by the table above.
struct BatsqueekOwnerBoneSet {
    u8 pad00[0x24];
};
extern BatsqueekOwnerBoneSet lbl_803BAAF4[];

int ModelBoneCount(int model);
extern "C" void* memset(void* dest, int value, u32 count);

// Sets up one of the owner's Batsqueeks modes: allocates the per-bone entry array
// and the mode buffer, then lets the model builder fill them in. Returns 0 when
// the mode hash is unknown or an allocation or the build fails.
int BatsqueekOwnerSetup(u8020A650Owner* owner, int model, u32 modeHash, u8* data,
                        Mtx34* mtx, u8020A650Anim* anim)
{
    if (model < -1)
        return 0;

    if (owner->entries != 0)
        owner->Stop();

    int mode;
    for (mode = 0; mode < 3; mode++)
    {
        if (modeHash == lbl_803BAAC4[mode].hash)
            break;
    }

    if (mode == 3)
        return 0;

    owner->mode = mode;
    owner->count = (u16)ModelBoneCount(model);
    owner->entries = (u8020A650Entry*)VAlloc(owner->count * 0x28);
    if (owner->entries == 0)
        return 0;

    memset(owner->entries, 0, owner->count * 0x28);

    owner->slotMap = (u8*)VAlloc(lbl_803BAAC4[mode].bufferSize);
    if (owner->slotMap == 0)
    {
        VFree(owner->entries);
        owner->entries = 0;
        return 0;
    }

    memset(owner->slotMap, 0, lbl_803BAAC4[mode].bufferSize);
    owner->modelId = (short)model;
    owner->boneSets = (u8*)&lbl_803BAAF4[lbl_803BAAC4[mode].boneSetIndex];
    owner->timer = 10000.0f;

    if (u8020A650BuildModel(owner, mode, data, anim, mtx) == 0)
    {
        VFree(owner->entries);
        owner->entries = 0;
        VFree(owner->slotMap);
        owner->slotMap = 0;
        return 0;
    }

    owner->Activate(0);
    return 1;
}

// Mtx34 plus context, initialized by the runtime before every debug draw.
struct BatsqueekDebugLink {
    Mtx34 matrix;               // 0x00: translation of the drawn box
    u8 pad30[0x08];
};

void LinkInfoInit(void* link);
void DebugDrawBoneBox(Vec3* box, void* link, int color, int a, int b);

extern float lbl_804D9380;
extern float lbl_804D9384;

// Draws a debug box at every position currently recorded by the sound trigger
// list, then clears the recorded position count.
void BatsqueekDrawTriggerBoxes(void)
{
    Vec3 box[2];
    BatsqueekDebugLink link;

    box[0].x = -24.0f;
    box[0].y = -24.0f;
    box[0].z = -24.0f;
    box[1].x = 24.0f;
    box[1].y = 24.0f;
    box[1].z = 24.0f;

    LinkInfoInit(&link);

    for (int i = 0; i < lbl_804D53A0; i++)
    {
        Mtx34StoreTranslation(&link.matrix, &lbl_8049E130[i]);
        DebugDrawBoneBox(box, &link, 0xff, 0, 0);
    }

    lbl_804D53A0 = 0;
}


// Ticks the timer down; once it runs out, releases every listed entry.
void TickCallbackTimer(float amount, u8020A650Owner* o)
{
    if (o->timer > 0.0f) {
        o->timer -= amount;
        if (o->timer <= 0.0f && o->entries != 0) {
            for (int i = 1; i < o->count; i++) {
                if (o->entries[i].source != 0) {
                    fn_801D5448(o->entries[i].source);
                }
            }
        }
    }
}

// Two vtables of the class hierarchy this object belongs to.
extern char lbl_803BB8CC[];
extern char lbl_803ADE00[];


// Tears the object down (derived vtable, virtual cleanup, base vtable) and
// optionally frees the storage.

void* DestroyVtObject(void* p, short flag)
{
    if (p != 0) {
        *(void**)p = &lbl_803BB8CC;
        ((u8020A650Owner*)p)->Stop();
        if (p != 0) {
            *(void**)p = &lbl_803ADE00;
        }
        if (flag > 0) {
            VFree(p);
        }
    }
    return p;
}


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
    unsigned char unk38[4];         // 0x38
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

extern "C" unsigned long AssetFind(unsigned long kind, unsigned long hash);

// Lazy string hash used by every name table: 31x multiply, upper-cased
// characters, with 0xFF meaning "keep -1".

static inline unsigned long HashName(const char* s)
{
    int i;
    unsigned long h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    for (i = 0; s[i] != 0; i++) {
        h = h * 31 + (unsigned char)toupper(s[i]);
    }
    h = h % 0xFFFFFFFF;
    return h;
}

// Fills in the cached name hashes and asset ids of the model, body-part and
// attachment name tables.
void u8020A650InitNameTables(void)
{
    u8020A650NameTables* t = &lbl_803BA9B8;
    u8020A650ModelName* m;
    u8020A650PartName* p;
    u8020A650AttachName* a;
    int i;

    for (i = 0; i < 3; i++) {
        t->models[i].hash = StrHash(t->models[i].name);
    }
    p = t->parts;
    for (i = 0; i < 0x26; i++, p++) {
        p->hash = HashName(p->name);
        p->asset = AssetFind(0xF1754BF2, p->hash);
        p->hash2 = HashName(p->name2);
        p->asset2 = AssetFind(0xF1754BF2, p->hash2);
    }
    a = t->attach;
    for (i = 0; i < 0x20; i++, a++) {
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
extern int lbl_804D4B18;

extern "C" float fabsf(float);
extern "C" float sqrtf(float);

// SDK vector helpers (plain C linkage in the retail binary).
extern "C" void PSQUATNormalize(Quat* q, Quat* out);

// Model / skeleton helpers.
void ModelAnimate(int modelId, char* frameBuf, int mode);
void ModelSetMatrix(int modelId, Mtx34* mtx, char* bones);
void BoneGetMatrix(Mtx34* mtx, void* rot);
void QuatRotateVecNorm(Vec3* out, float* q, Vec3* in);

// Actor-part helpers, still only known by address.
extern "C" void* fn_801D54E8(void);
extern "C" void* fn_801D5B38(float value, int zero, int asset, int id,
                              unsigned long hash, void* ctx, Vec3* out, int again);
extern "C" void fn_801D52E0(void* cb);
extern "C" void fn_801D5298(void* cb, Vec3* v);
extern "C" void fn_801D5260(void* cb, Vec3* v);
extern "C" void fn_801D556C(void* cb);
extern "C" void* fn_801D4C34(void);
extern "C" void fn_801D4B5C(void* ctx, void* cb, Vec3* v);
extern "C" void fn_801D4824(void* ctx, void* cbA, void* cbB, Vec3* v1,
                             Vec3* v2, int count, Mtx34* mtx);
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

    owner->entries[0].source = 0;
    owner->entries[0].rest.rot = anims[0].rot;
    owner->entries[0].rest.pos = anims[0].pos;

    numParts = lbl_803BA9B8.models[owner->mode].numParts;

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

                    if (lbl_804D4B18 == 0) {
                        hash = HashName("RED OAK");
                    } else {
                        hash = 0xBF4C00C0;
                    }
                    handle = fn_801D5B38(scale, 0, part->asset, part->id, hash,
                                         ctx, &axis, 0);
                    owner->entries[slot].source = handle;

                    if (owner->entries[slot].source != 0) {
                        u8020A650Entry* e = &owner->entries[slot];
                        void* cb = e->source;
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
                        PSQUATMultiply(&frames[slot].rot, &q, &e->rest.rot);
                        e->rest.rot.x = -e->rest.rot.x;
                        e->rest.rot.y = -e->rest.rot.y;
                        e->rest.rot.z = -e->rest.rot.z;

                        fn_801D52E0(cb);
                        zero.x = 0.0f;
                        zero.y = 0.0f;
                        zero.z = 0.0f;
                        fn_801D5298(cb, &zero);
                        fn_801D5260(cb, &zero);
                        fn_801D5110(cb, 0);
                        fn_801D556C(cb);
                    }
                    owner->entries[slot].nameHash = part->hash;
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
            if (owner->entries[slot].source != 0) {
                continue;
            }
            slot2 = fn_80037930(owner->modelId);
            if (slot2 == -1) {
                continue;
            }
            slot2 = slotMap[slot2];
            owner->entries[slot].index = slot2;
            delta.x = frames[slot].pos.x - frames[slot2].pos.x;
            delta.y = frames[slot].pos.y - frames[slot2].pos.y;
            delta.z = frames[slot].pos.z - frames[slot2].pos.z;
            owner->entries[slot].rest.pos = delta;
            QuatRotateVec(&owner->entries[slot].rest.pos, &frames[slot2].rot.x,
                          &owner->entries[slot].rest.pos);
            inv.x = -frames[slot2].rot.x;
            inv.y = -frames[slot2].rot.y;
            inv.z = -frames[slot2].rot.z;
            inv.w = frames[slot2].rot.w;
            PSQUATMultiply(&frames[slot].rot, &inv, &owner->entries[slot].rest.rot);
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
        if (owner->entries[slotA].source == 0) {
            continue;
        }
        if (owner->entries[slotB].source == 0) {
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

            fn_801D4B5C(owner->ctx, owner->entries[slotA].source, &frames[slotA].pos);
            if (owner->entries[slotA].source != 0) {
                Mtx34 boneMtx;

                BoneGetMatrix(&boneMtx, &frames[slotA].rot);
                fn_800A0168(&boneMtx, &boneMtx);
                fn_800A0030(&v1, &boneMtx, &v1);
                fn_800A0030(&v2, &boneMtx, &v2);
            } else {
                inv.x = -owner->entries[slotA].rest.rot.x;
                inv.y = -owner->entries[slotA].rest.rot.y;
                inv.z = -owner->entries[slotA].rest.rot.z;
                inv.w = owner->entries[slotA].rest.rot.w;
                QuatRotateVecNorm(&v1, &inv.x, &v1);
                QuatRotateVecNorm(&v2, &inv.x, &v2);
            }

            attachMtx.m[0][0] = a->f24;
            attachMtx.m[0][1] = a->f28;
            attachMtx.m[0][2] = lbl_804D939C;
            attachMtx.m[0][3] = 0.0f;
            attachMtx.m[1][0] = lbl_804D93A0;
            attachMtx.m[1][1] = lbl_804D93A4;
            attachMtx.m[1][2] = lbl_804D939C;
            attachMtx.m[1][3] = 0.0f;
            attachMtx.m[2][0] = a->f30;
            attachMtx.m[2][1] = a->f34;
            attachMtx.m[2][2] = lbl_804D939C;
            attachMtx.m[2][3] = 0.0f;
            fn_801D4824(owner->ctx, owner->entries[slotA].source,
                        owner->entries[slotB].source, &v1, &v2, 3, &attachMtx);
            break;
        }
        case 1: {
            v1.x = a->f0C;
            v1.y = a->f10;
            v1.z = a->f14;

            if (owner->entries[slotA].source != 0) {
                Mtx34 boneMtx;

                BoneGetMatrix(&boneMtx, &frames[slotA].rot);
                fn_800A0168(&boneMtx, &boneMtx);
                fn_800A0030(&v1, &boneMtx, &v1);
            } else {
                inv.x = -owner->entries[slotA].rest.rot.x;
                inv.y = -owner->entries[slotA].rest.rot.y;
                inv.z = -owner->entries[slotA].rest.rot.z;
                inv.w = owner->entries[slotA].rest.rot.w;
                QuatRotateVecNorm(&v1, &inv.x, &v1);
            }

            if (owner->entries[slotA].source != 0) {
                fn_801D513C(owner->entries[slotA].source, tmp);
                fn_801D4A70(owner->ctx, owner->entries[slotA].source,
                            owner->entries[slotB].source, tmp + 0x24, &v1, &a->f24);
            } else {
                fn_801D4A70(owner->ctx, &inv, owner->entries[slotB].source,
                            &owner->entries[slotA].rest.pos, &v1, &a->f24);
            }
            break;
        }
        }
    }

    for (i = 1; i < owner->count; i++) {
        int slot;
        Vec3 delta;
        Quat inv;

        if (owner->entries[i].source != 0) {
            continue;
        }
        slot = owner->entries[i].index;
        delta.x = anims[i].pos.x - anims[slot].pos.x;
        delta.y = anims[i].pos.y - anims[slot].pos.y;
        delta.z = anims[i].pos.z - anims[slot].pos.z;
        owner->entries[i].rest.pos = delta;
        QuatRotateVec(&owner->entries[i].rest.pos, &anims[slot].rot.x,
                      &owner->entries[i].rest.pos);
        inv.x = -anims[slot].rot.x;
        inv.y = -anims[slot].rot.y;
        inv.z = -anims[slot].rot.z;
        inv.w = anims[slot].rot.w;
        PSQUATMultiply(&anims[i].rot, &inv, &owner->entries[i].rest.rot);
    }

    return 1;
}

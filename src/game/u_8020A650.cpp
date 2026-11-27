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

void* VAlloc(int size);
void VFree(void* p);

// Base of the objects in the active-object registry (u_801E0EDC.cpp; vtable 0x803ADE00, whose slots after
// the destructor are all empty). Its destructor is inline: ~ActiveNode stores its vtable directly.
// Class and member names are guesses.
class ActiveObject {
public:
    static void operator delete(void* p) { VFree(p); }
    virtual ~ActiveObject() {}
    virtual int Setup(int model, u32 modeHash, u8* data, Mtx34* mtx, u8020A650Anim* anim) = 0; // 0x0C
    virtual void TickTimer(float amount, int flag) = 0;                                       // 0x10
    virtual void BuildSourceMatrices(int arg2, int arg3, u8020A650Anim* out) = 0;             // 0x14
    virtual void ApplySourceMode(int mode) = 0;                                               // 0x18
    virtual int SourcesFinished() = 0;                                                        // 0x1C
    virtual void DrawTriggerBoxes() = 0;                                                      // 0x20
    virtual void Teardown() = 0;                                                              // 0x24
    virtual void StopAll(int flag) = 0;                                                       // 0x28
    virtual void TriggerEntry(u32 hash, void* param) = 0;                                     // 0x2C
    virtual void TriggerModeSounds(float seek, Vec3* pos, Vec3* vec, int mode) = 0;           // 0x30
    virtual void Unk34() = 0;                                                                 // 0x34
};

// The body/sound-source owner (vtable 0x803BB8CC; it fills slots 0x08-0x30 of the ActiveObject
// vtable): a countdown timer and one 0x28-byte entry per model bone. Some functions walk the entry
// array with a byte offset (entryBytes). Class name is a guess.
class ActiveNode : public ActiveObject {
public:
    virtual ~ActiveNode();
    virtual int Setup(int model, u32 modeHash, u8* data, Mtx34* mtx, u8020A650Anim* anim);
    virtual void TickTimer(float amount, int flag);
    virtual void BuildSourceMatrices(int arg2, int arg3, u8020A650Anim* out);
    virtual void ApplySourceMode(int mode);
    virtual int SourcesFinished();
    virtual void DrawTriggerBoxes();
    virtual void Teardown();
    virtual void StopAll(int flag);
    virtual void TriggerEntry(u32 hash, void* param);
    virtual void TriggerModeSounds(float seek, Vec3* pos, Vec3* vec, int mode);

    short modelId;                  // 0x04
    u16 count;                      // 0x06: bone/entry count
    s32 mode;                       // 0x08: index into the model name table
    union {
        u8020A650Entry* entries;    // 0x0C
        u8* entryBytes;
    };
    void* ctx;                      // 0x10: secondary source
    u8* slotMap;                    // 0x14: per-mode buffer
    u8* boneSets;                   // 0x18: bone-set record of the mode
    float timer;                    // 0x1C: retrigger delay written on every trigger
};

// Upper-case table used by the name hashes.
extern unsigned char __upper_map[256];
static inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

extern "C" void PSQUATMultiply(Quat* a, Quat* b, Quat* out);
void Mtx34StoreTranslation(Mtx34* mtx, const Vec3* v);
void QuatRotateVec(Vec3* out, float* q, Vec3* in);
extern "C" void fn_801D5110(void* cb, int value);
extern "C" void fn_801D513C(void* cb, void* out);
extern "C" void fn_801D5448(void* cb);
int u8020A650BuildModel(ActiveNode* owner, int mode, u8* data,
                        const u8020A650Anim* anim, Mtx34* mtx);


// From a shared header: the ghost actions (unused here).
struct GhostAction {
    const char* name;
    int unk4;
    int time;
    float range;
    float range2;
    int hostOnly;
};

static GhostAction sGhostActions[] = {
    { "POSSESS", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "GHOSTACTION", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "SLIP", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "HIJACK", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "HOSTACTION", 0, 1000, 1600.0f, 4033.0f, 1 },
    { "EXCHANGE", 0, 1000, 1600.0f, 3600.0f, 1 },
    { "H_HIJACK", 0, 1000, 1600.0f, 3600.0f, 1 },
    { "TALK", 0, 1000, 1120.0f, 3600.0f, 1 },
    { "SCAN", 0, 1000, 1120.0f, 3600.0f, 1 },
};

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
    float f2C;                      // 0x2C
    float f30;                      // 0x30
    float f34;                      // 0x34
    float f38;                      // 0x38
    char* name;                     // 0x3C
    char* name2;                    // 0x40
};

static const char* sModeNames[] = {
    "AUTO", "FULL BODY", "PELVIS", "SPINE1", "NECK", "LTHIGH", "LCALF", "RTHIGH", "RCALF", "LUPPERARM", "LFOREARM", "RUPPERARM", "RFOREARM",
};

static u8020A650ModelName sModels[3] = {
    { 0, 12, 0, 10, 0, "RD_HUMAN_MALE_TYPE1" },
    { 0, 14, 12, 12, 10, "RD_ANIMAL_DOG_TYPE1" },
    { 0, 12, 26, 10, 22, "RD_STATUE_TYPE1" },
};

static u8020A650PartName sParts[0x26] = {
    { 0, 0, 0, 0, 0xEF17C6D3, 25.0f, 10.0f, "BIPBODY", "" },
    { 0, 0, 0, 0, 0xEF17C6D3, 60.0f, 10.0f, "PELVIS", "SPINE1" },
    { 0, 0, 0, 0, 0xEF17C6D3, 60.0f, 10.0f, "SPINE1", "NECK" },
    { 0, 0, 0, 0, 0xEF17C6D3, 45.0f, 30.0f, "NECK", "HEAD" },
    { 0, 0, 0, 0, 0xEF17C6D3, 35.0f, 110.0f, "LTHIGH", "LCALF" },
    { 0, 0, 0, 0, 0xEF17C6D3, 30.0f, 170.0f, "LCALF", "LFOOT" },
    { 0, 0, 0, 0, 0xEF17C6D3, 35.0f, 110.0f, "RTHIGH", "RCALF" },
    { 0, 0, 0, 0, 0xEF17C6D3, 30.0f, 170.0f, "RCALF", "RFOOT" },
    { 0, 0, 0, 0, 0xEF17C6D3, 25.0f, 70.0f, "LUPPERARM", "LFOREARM" },
    { 0, 0, 0, 0, 0xEF17C6D3, 25.0f, 90.0f, "LFOREARM", "LHAND" },
    { 0, 0, 0, 0, 0xEF17C6D3, 25.0f, 70.0f, "RUPPERARM", "RFOREARM" },
    { 0, 0, 0, 0, 0xEF17C6D3, 25.0f, 90.0f, "RFOREARM", "RHAND" },
    { 0, 0, 0, 0, 0xEF17C6D3, 25.0f, 10.0f, "BIPBODY", "" },
    { 0, 0, 0, 0, 0xEF17C6D3, 60.0f, 10.0f, "PELVIS", "SPINE1" },
    { 0, 0, 0, 0, 0xEF17C6D3, 60.0f, 10.0f, "SPINE1", "NECK" },
    { 0, 0, 0, 0, 0xEF17C6D3, 45.0f, 30.0f, "NECK1", "HEAD" },
    { 0, 0, 0, 0, 0xEF17C6D3, 40.0f, 30.0f, "LTHIGH", "LCALF" },
    { 0, 0, 0, 0, 0xEF17C6D3, 20.0f, 40.0f, "LCALF", "LFOOT" },
    { 0, 0, 0, 0, 0xEF17C6D3, 20.0f, 70.0f, "LFOOT", "LTOE01" },
    { 0, 0, 0, 0, 0xEF17C6D3, 40.0f, 30.0f, "RTHIGH", "RCALF" },
    { 0, 0, 0, 0, 0xEF17C6D3, 20.0f, 40.0f, "RCALF", "RFOOT" },
    { 0, 0, 0, 0, 0xEF17C6D3, 15.0f, 70.0f, "RFOOT", "RTOE01" },
    { 0, 0, 0, 0, 0xEF17C6D3, 25.0f, 40.0f, "LUPPERARM", "LFOREARM" },
    { 0, 0, 0, 0, 0xEF17C6D3, 20.0f, 120.0f, "LFOREARM", "LFINGER0" },
    { 0, 0, 0, 0, 0xEF17C6D3, 25.0f, 40.0f, "RUPPERARM", "RFOREARM" },
    { 0, 0, 0, 0, 0xEF17C6D3, 20.0f, 120.0f, "RFOREARM", "RFINGER0" },
    { 0, 0, 0, 0, 0xEF17C6D3, 200.0f, 40.0f, "BIPBODY", "" },
    { 0, 0, 0, 0, 0xEF17C6D3, 480.0f, 315.0f, "PELVIS", "SPINE" },
    { 0, 0, 0, 0, 0xEF17C6D3, 480.0f, 1428.0f, "SPINE", "NECK" },
    { 0, 0, 0, 0, 0xEF17C6D3, 360.0f, 205.0f, "NECK", "HEAD" },
    { 0, 0, 0, 0, 0xEF17C6D3, 280.0f, 1434.0f, "LTHIGH", "LCALF" },
    { 0, 0, 0, 0, 0xEF17C6D3, 240.0f, 1257.0f, "LCALF", "LFOOT" },
    { 0, 0, 0, 0, 0xEF17C6D3, 280.0f, 1434.0f, "RTHIGH", "RCALF" },
    { 0, 0, 0, 0, 0xEF17C6D3, 240.0f, 1257.0f, "RCALF", "RFOOT" },
    { 0, 0, 0, 0, 0xEF17C6D3, 200.0f, 1022.0f, "LUPPERARM", "LFOREARM" },
    { 0, 0, 0, 0, 0xEF17C6D3, 200.0f, 492.0f, "LFOREARM", "LHAND" },
    { 0, 0, 0, 0, 0xEF17C6D3, 200.0f, 1022.0f, "RUPPERARM", "RFOREARM" },
    { 0, 0, 0, 0, 0xEF17C6D3, 200.0f, 492.0f, "RFOREARM", "RHAND" },
};

static u8020A650AttachName sAttach[0x20] = {
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -10.0f, 60.0f, 0.0f, 0.0f, 25.0f, 0.0f, "LTHIGH", "PELVIS" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -70.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, "LCALF", "LTHIGH" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -10.0f, 60.0f, 0.0f, 0.0f, 25.0f, 0.0f, "RTHIGH", "PELVIS" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -70.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, "RCALF", "RTHIGH" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -10.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, "NECK", "SPINE1" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -10.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, "SPINE1", "PELVIS" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -70.0f, 20.0f, 0.0f, -80.0f, 80.0f, 0.0f, "LUPPERARM", "SPINE1" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -45.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, "LFOREARM", "LUPPERARM" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -70.0f, 20.0f, 0.0f, -80.0f, 80.0f, 0.0f, "RUPPERARM", "SPINE1" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -45.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, "RFOREARM", "RUPPERARM" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -30.0f, 60.0f, 0.0f, -5.0f, 70.0f, 0.0f, "LTHIGH", "PELVIS" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, "LCALF", "LTHIGH" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -40.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, "LFOOT", "LCALF" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -30.0f, 60.0f, 0.0f, -5.0f, 70.0f, 0.0f, "RTHIGH", "PELVIS" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, "RCALF", "RTHIGH" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -40.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, "RFOOT", "RCALF" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -10.0f, 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, "NECK1", "SPINE1" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -10.0f, 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, "SPINE1", "PELVIS" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -30.0f, 30.0f, 0.0f, -70.0f, 5.0f, 0.0f, "LUPPERARM", "SPINE1" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -5.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, "LFOREARM", "LUPPERARM" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -30.0f, 30.0f, 0.0f, -70.0f, 5.0f, 0.0f, "RUPPERARM", "SPINE1" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -5.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, "RFOREARM", "RUPPERARM" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -10.0f, 60.0f, 0.0f, 0.0f, 25.0f, 0.0f, "LTHIGH", "PELVIS" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -70.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, "LCALF", "LTHIGH" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -10.0f, 60.0f, 0.0f, 0.0f, 25.0f, 0.0f, "RTHIGH", "PELVIS" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -70.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, "RCALF", "RTHIGH" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -10.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, "NECK", "SPINE" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -10.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, "SPINE", "PELVIS" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -70.0f, 20.0f, 0.0f, -80.0f, 80.0f, 0.0f, "LUPPERARM", "SPINE" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -45.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, "LFOREARM", "LUPPERARM" },
    { 0, 0, 0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -70.0f, 20.0f, 0.0f, -80.0f, 80.0f, 0.0f, "RUPPERARM", "SPINE" },
    { 0, 0, 1, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -45.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, "RFOREARM", "RUPPERARM" },
};

static Vec3 sTriggerPos[0x14];  // recent trigger positions
static int sTriggerCount;       // number of recent trigger positions

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

void ActiveNode::TriggerModeSounds(float seek, Vec3* pos, Vec3* vec, int mode)
{
    Vec3 local;

    if (entryBytes == 0)
        return;

    Vec3Init(&local, vec->x, vec->y, vec->z);

    if (fn_801D60A0(1) != 0 && sTriggerCount < 0x14)
        Vec3Init(&sTriggerPos[sTriggerCount++], pos->x, pos->y, pos->z);

    if (mode == 0)
    {
        int hits = 0;
        int i;

        for (i = 1; i < count; i++)
        {
            if (entries[i].source != 0 &&
                fn_801D4738(entries[i].source, pos, seek) != 0)
            {
                hits++;
                fn_801D5010(entries[i].source, &local);
            }
        }

        if (hits == 0)
        {
            local.x *= 0.03125f;
            local.y *= 0.03125f;
            local.z *= 0.03125f;
            int i;

            for (i = 1; i < count; i++)
            {
                if (entries[i].source != 0)
                    fn_801D5010(entries[i].source, &local);
                }
        }
    }
    else if (mode == 1)
    {
        local.x *= 0.03125f;
        local.y *= 0.03125f;
        local.z *= 0.03125f;
        int i;

        for (i = 1; i < count; i++)
        {
            if (entries[i].source != 0)
                fn_801D5010(entries[i].source, vec);
        }
    }
    else
    {
        u32 hash = StrHash(sModeNames[mode]);
        int i;
        int off;

        for (i = 1, off = 0x28; i < count; i++)
        {
            u8020A650Entry* entry = (u8020A650Entry*)(entryBytes + off);

            if (entry->source != 0 && hash == entry->nameHash)
                fn_801D5010(entry->source, vec);
            off += 0x28;
        }
    }

    timer = 10000.0f;
}

// Finds the sound-list entry registered under the given name hash and hands its
// source object to the trigger routine; if nothing matches, restarts the delay.
void ActiveNode::TriggerEntry(u32 hash, void* param)
{
    u8020A650Entry* list = entries;

    if (list != 0)
    {
        for (int i = 1; i < count; i++)
        {
            if (this->entries[i].source != 0 &&
                hash == this->entries[i].nameHash)
            {
                fn_801D5010(list[i].source, param);
                return;
            }
        }
        timer = 10000.0f;
    }
}

// Releases every live source held by the list.
void ActiveNode::StopAll(int flag)
{
    if (entryBytes != 0)
    {
        int off;
        int i;

        for (i = 1, off = 0x28; i < count; i++)
        {
            void* source = *(void**)(entryBytes + off + 0x24);

            if (source != 0)
                fn_801D5110(source, 1);
            off += 0x28;
        }
    }
}

// Releases every live source held by the list, then frees the entry array, the
// secondary source and the extra buffer, if they exist.
void ActiveNode::Teardown()
{
    if (entryBytes != 0)
    {
        int off;
        int i;

        for (i = 1, off = 0x28; i < count; i++)
        {
            void* source = *(void**)(entryBytes + off + 0x24);

            if (source != 0)
                fn_801D55CC(source);
            off += 0x28;
        }

        VFree(entryBytes);
        fn_801D4C0C(ctx);
    }

    if (slotMap != 0)
        VFree(slotMap);
}

// Returns non-zero when none of the live sources of the list is still marked
// active; a single active source makes the whole list report "not finished".
int ActiveNode::SourcesFinished()
{
    int off;
    int i;

    if (entryBytes == 0)
        return 1;

    i = 1;
    off = 0x28;
    for (; i < count; i++)
    {
        void* source = *(void**)(entryBytes + off + 0x24);

        if (source != 0 && fn_801D549C(source) != 0)
            return 0;
        off += 0x28;
    }

    return 1;
}

extern "C" void fn_801D53D4(void* source);
extern "C" void fn_801D550C(void* source);



void BoneSetMatrix(void* bone, Mtx34* matrix);
void Mtx34GetTranslation(Vec3* out, const Mtx34* matrix);
void QuatRotateVec(Vec3* out, float* quat, Vec3* vec);


// Updates every live source of the list: when 'mode' is non-zero the sources are
// paused, otherwise they are resumed and re-registered with their source object.
void ActiveNode::ApplySourceMode(int mode)
{
    if (entryBytes == 0)
        return;

    int off;
    int i;

    for (i = 1, off = 0x28; i < count; i++)
    {
        void* source = *(void**)(entryBytes + off + 0x24);

        if (source != 0)
        {
            if (mode != 0)
            {
                fn_801D53D4(source);
            }
            else
            {
                fn_801D550C(source);
                fn_801D5448(*(void**)(entryBytes + off + 0x24));
            }
        }

        off += 0x28;
    }
}

// Builds the per-source transforms of the list into 'out': entry 0 supplies the
// base transform, every following entry either takes its live bone matrix or is
// rotated by the transform of the entry its index points at.
// Conjugate (inverse rotation) of a unit quaternion.
static inline void QuatConjugate(Quat* out, const Quat* q)
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

void ActiveNode::BuildSourceMatrices(int arg2, int arg3, u8020A650Anim* frames)
{
    Quat quat;
    Mtx34 matrix;

    if (entryBytes == 0)
        return;

    frames->pos = entries->rest.pos;
    frames->rot = entries->rest.rot;

    int off;
    u8020A650Anim* cur;
    int i;
    for (i = 1, off = 0x28, cur = frames + 1; i < count; i++)
    {
        if (((u8020A650Entry*)(entryBytes + off))->source != 0)
        {
            fn_801D513C(((u8020A650Entry*)(entryBytes + off))->source, &matrix);
            BoneSetMatrix(cur, &matrix);
            PSQUATMultiply(&cur->rot, &((u8020A650Entry*)(entryBytes + off))->rest.rot, &cur->rot);

            cur->rot.x = -cur->rot.x;
            cur->rot.y = -cur->rot.y;
            cur->rot.z = -cur->rot.z;

            Mtx34GetTranslation(&cur->pos, &matrix);
        }
        else
        {
            PSQUATMultiply(&((u8020A650Entry*)(entryBytes + off))->rest.rot, &frames[((u8020A650Entry*)(entryBytes + off))->index].rot, &cur->rot);
            QuatConjugate(&quat, &frames[((u8020A650Entry*)(entryBytes + off))->index].rot);
            QuatRotateVec(&cur->pos, (float*)&quat, &((u8020A650Entry*)(entryBytes + off))->rest.pos);
            Vec3AddTo(&cur->pos, &frames[((u8020A650Entry*)(entryBytes + off))->index].pos);
        }

        off += 0x28;
        cur++;
    }

    frames->rot = frames[1].rot;
    frames->pos = frames[1].pos;
}


int ModelBoneCount(int model);
extern "C" void* memset(void* dest, int value, u32 count);

// Sets up one of the owner's Batsqueeks modes: allocates the per-bone entry array
// and the mode buffer, then lets the model builder fill them in. Returns 0 when
// the mode hash is unknown or an allocation or the build fails.
int ActiveNode::Setup(int model, u32 modeHash, u8* data, Mtx34* mtx, u8020A650Anim* anim)
{
    if (model < -1)
        return 0;

    if (entries != 0)
        Teardown();

    int mode;
    for (mode = 0; mode < 3; mode++)
    {
        if (modeHash == sModels[mode].hash)
            break;
    }

    if (mode == 3)
        return 0;

    this->mode = mode;
    count = (u16)ModelBoneCount(model);
    entries = (u8020A650Entry*)VAlloc(count * 0x28);
    if (entries == 0)
        return 0;

    memset(entries, 0, count * 0x28);

    slotMap = (u8*)VAlloc(sModels[mode].numParts);
    if (slotMap == 0)
    {
        VFree(entries);
        entries = 0;
        return 0;
    }

    memset(slotMap, 0, sModels[mode].numParts);
    modelId = (short)model;
    boneSets = (u8*)&sParts[sModels[mode].partSet];
    timer = 10000.0f;

    if (u8020A650BuildModel(this, mode, data, anim, mtx) == 0)
    {
        VFree(entries);
        entries = 0;
        VFree(slotMap);
        slotMap = 0;
        return 0;
    }

    StopAll(0);
    return 1;
}

// Mtx34 plus context, initialized by the runtime before every debug draw.
struct BatsqueekDebugLink {
    Mtx34 matrix;               // 0x00: translation of the drawn box
    u8 pad30[0x08];
};

void LinkInfoInit(void* link);
void DebugDrawBoneBox(Vec3* box, void* link, int color, int a, int b);


// Draws a debug box at every position currently recorded by the sound trigger
// list, then clears the recorded position count.
void ActiveNode::DrawTriggerBoxes()
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

    for (int i = 0; i < sTriggerCount; i++)
    {
        Mtx34StoreTranslation(&link.matrix, &sTriggerPos[i]);
        DebugDrawBoneBox(box, &link, 0xff, 0, 0);
    }

    sTriggerCount = 0;
}


// Ticks the timer down; once it runs out, releases every listed entry.
void ActiveNode::TickTimer(float amount, int flag)
{
    if (timer > 0.0f) {
        timer -= amount;
        if (timer <= 0.0f && entries != 0) {
            for (int i = 1; i < count; i++) {
                if (entries[i].source != 0) {
                    fn_801D5448(entries[i].source);
                }
            }
        }
    }
}

// Tears the object down through the (virtual) Teardown; the inline ActiveObject
// destructor then restores the base vtable.
ActiveNode::~ActiveNode()
{
    Teardown();
}



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
    u8020A650ModelName* m;
    u8020A650PartName* p;
    u8020A650AttachName* a;
    int i;

    for (i = 0; i < 3; i++) {
        sModels[i].hash = StrHash(sModels[i].name);
    }
    p = sParts;
    for (i = 0; i < 0x26; i++, p++) {
        p->hash = HashName(p->name);
        p->asset = AssetFind(0xF1754BF2, p->hash);
        p->hash2 = HashName(p->name2);
        p->asset2 = AssetFind(0xF1754BF2, p->hash2);
    }
    a = sAttach;
    for (i = 0; i < 0x20; i++, a++) {
        a->asset = AssetFind(0xF1754BF2, HashName(a->name));
        a->asset2 = AssetFind(0xF1754BF2, HashName(a->name2));
    }
    sTriggerCount = 0;
}

// Constants and globals shared with the rest of the game code.
extern int lbl_804D33B8;        // non-zero: hash the part material name

// SDK vector helpers (plain C linkage in the retail binary).
extern "C" void PSQUATNormalize(Quat* q, Quat* out);
extern "C" float sqrtf(float);

// Model / skeleton helpers.
void ModelAnimate(int modelId, char* frameBuf, int mode);
void ModelSetMatrix(int modelId, Mtx34* mtx, char* bones);
void BoneGetMatrix(Mtx34* mtx, void* rot);
void QuatRotateVecNorm(Vec3* out, float* q, Vec3* in);

// Actor-part helpers, still only known by address.
extern "C" void* fn_801D54E8(void);
extern "C" void* fn_801D5B38(int handle, unsigned long asset, int id, unsigned long hash, void* ctx,
                              Vec3* offset, int flag, float x, float y, float z);
extern "C" void fn_801D52E0(void* cb, Mtx34* mtx);
extern "C" void fn_801D5298(void* cb, Vec3* v);
extern "C" void fn_801D5260(void* cb, Vec3* v);
extern "C" void fn_801D556C(void* cb);
extern "C" void* fn_801D4C34(void);
extern "C" void fn_801D4B5C(void* ctx, void* cbA, void* cbB, Vec3* v);
extern "C" void fn_801D4824(void* ctx, void* cbA, void* cbB, Vec3* v1,
                             Vec3* v2, int count, Mtx34* mtx);
extern "C" void fn_801D4A70(void* ctx, void* a, void* b, Vec3* pos, Vec3* v,
                             float* extra);
extern "C" void fn_800A0168(Mtx34* mtx, Mtx34* out);
extern "C" void fn_800A0030(Vec3* v, Mtx34* mtx, Vec3* out);
extern "C" unsigned short* fn_80037B6C(int modelId);
extern "C" int fn_80037930(int modelId, int bone);

// C stand-ins for the retail paired-single helpers (VecNormalize in include/game/vecnorm.h and the
// FastSqrt block); retail's versions may only be used once the whole unit matches.
float __fabsf(float x);

inline float FastSqrt(float x) {
    float r;
    if (x <= 0.0f) {
        r = 0.0f;
    } else {
        r = sqrtf(x);
    }
    return r;
}

inline float VecNormalize(Vec3* dst, const Vec3* src) {
    float len;
    float az = __fabsf(src->z);
    float ay = __fabsf(src->y);
    float ax = __fabsf(src->x);
    if ((ax > 1.1920929e-7f) + (ay > 1.1920929e-7f) + (az > 1.1920929e-7f) == 0) {
        dst->x = 0.0f;
        dst->y = 0.0f;
        dst->z = 0.0f;
    } else {
        float inv = 1.0f / sqrtf(src->x * src->x + src->y * src->y + src->z * src->z);
        dst->x = src->x * inv;
        dst->y = src->y * inv;
        dst->z = src->z * inv;
        len = 1.0f / inv;
    }
    return len;
}

static inline void VecCross(Vec3* out, const Vec3* a, const Vec3* b)
{
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = a->x * b->y - a->y * b->x;
}

static inline void Vec3Sub(Vec3* out, const Vec3* a, const Vec3* b)
{
    float x = a->x, y = a->y, z = a->z;
    x -= b->x;
    y -= b->y;
    z -= b->z;
    out->x = x;
    out->y = y;
    out->z = z;
}

// Builds the runtime representation of one body: allocates a callback per
// body part, places it on the animated skeleton and links the attachments.
int u8020A650BuildModel(ActiveNode* owner, int setIndex, u8* map,
                        const u8020A650Anim* anims, Mtx34* mtx)
{
    unsigned short numParts;
    u8020A650ModelName* set = &sModels[setIndex];
    u8020A650PartName* parts = &sParts[set->partSet];
    u8020A650AttachName* attach;
    char animBuf[0x1C00];
    void* ctx;
    unsigned short* list;
    int n;
    int i;

    owner->entries[0].source = 0;
    owner->entries[0].rest.pos = anims[0].pos;
    owner->entries[0].rest.rot = anims[0].rot;

    numParts = sModels[owner->mode].numParts;

    ModelAnimate(owner->modelId, animBuf, 1);
    ((u8020A650Anim*)animBuf)[0].pos.x = 0.0f;
    ((u8020A650Anim*)animBuf)[0].pos.y = 0.0f;
    ((u8020A650Anim*)animBuf)[0].pos.z = 0.0f;
    ModelSetMatrix(owner->modelId, mtx, animBuf);
    ctx = fn_801D54E8();

    for (int i = 1; i < numParts; i++) {
        u8020A650PartName* part = &parts[i];
        int slot = map[part->asset];
        int slot2 = map[part->asset2];
        Vec3 size;
        Vec3 axis;
        Vec3 delta;
        Vec3 zero;
        Quat q;
        Mtx34 partMtx;

        if (slot == 0xFF || slot2 == 0xFF)
            continue;

        owner->slotMap[i] = slot;
        size.x = 2.0f * part->f14;
        size.y = 2.0f * part->f14;
        size.z = part->f18;
        delta.x = ((u8020A650Anim*)animBuf)[slot2].pos.x - ((u8020A650Anim*)animBuf)[slot].pos.x;
        delta.y = ((u8020A650Anim*)animBuf)[slot2].pos.y - ((u8020A650Anim*)animBuf)[slot].pos.y;
        delta.z = ((u8020A650Anim*)animBuf)[slot2].pos.z - ((u8020A650Anim*)animBuf)[slot].pos.z;
        u8020A650Anim* a = &((u8020A650Anim*)animBuf)[slot];
        axis.x = 0.0f;
        axis.y = 0.0f;
        axis.z = 0.5f * (2.0f * part->f14 + part->f18);
        if (__fabsf(size.x) < 1.1920929e-7f || __fabsf(size.y) < 1.1920929e-7f ||
            __fabsf(size.z) < 1.1920929e-7f)
            continue;

        if (lbl_804D33B8 == 0)
            owner->entries[slot].source = fn_801D5B38(0, part->asset, part->id, StrHash("RED OAK"), ctx,
                                                      &axis, 0, size.x, size.y, size.z);
        else
            owner->entries[slot].source = fn_801D5B38(0, part->asset, part->id, 0xBF4C00C0, ctx,
                                                      &axis, 0, size.x, size.y, size.z);

        if (owner->entries[slot].source != 0) {
            float s;
            Vec3 up;
            Vec3 c;

            VecNormalize(&delta, &delta);
            up.x = 0.0f;
            up.y = 0.0f;
            up.z = 1.0f;
            VecCross(&c, &up, &delta);
            s = FastSqrt(2.0f * delta.z + 2.0f);
            if (s != 0.0f) {
                if (s == 0.0f) {
                    q.x = 0.0f;
                    q.y = 0.0f;
                    q.z = 0.0f;
                } else {
                    float inv = 1.0f / s;
                    q.x = c.x * inv;
                    q.y = c.y * inv;
                    q.z = c.z * inv;
                }
                q.w = 0.5f * s;
                PSQUATNormalize(&q, &q);
            } else {
                q.x = 0.0f;
                q.y = 0.0f;
                q.z = 0.0f;
                q.w = 1.0f;
            }

            BoneGetMatrix(&partMtx, &q);
            Mtx34StoreTranslation(&partMtx, &a->pos);
            PSQUATMultiply(&a->rot, &q, &owner->entries[slot].rest.rot);
            {
                Quat* r = &owner->entries[slot].rest.rot;
                r->x = -r->x;
                r->y = -r->y;
                r->z = -r->z;
            }
            fn_801D52E0(owner->entries[slot].source, &partMtx);
            zero.x = 0.0f;
            zero.y = 0.0f;
            zero.z = 0.0f;
            fn_801D5298(owner->entries[slot].source, &zero);
            fn_801D5260(owner->entries[slot].source, &zero);
            fn_801D5110(owner->entries[slot].source, 0);
            fn_801D556C(owner->entries[slot].source);
        }
        owner->entries[slot].nameHash = part->hash;
    }

    owner->ctx = fn_801D4C34();
    list = fn_80037B6C(owner->modelId);
    for (int i = 0; i < owner->count; i++) {
        int slot = map[list[i]];
        int parent;
        Quat inv;

        if (slot == -1)
            continue;
        if (owner->entries[slot].source != 0)
            continue;
        parent = fn_80037930(owner->modelId, list[i]);
        if (parent == -1)
            continue;
        parent = map[parent];
        u8020A650Anim* a = &((u8020A650Anim*)animBuf)[slot];
        u8020A650Anim* b = &((u8020A650Anim*)animBuf)[parent];
        owner->entries[slot].index = parent;
        {
            Vec3* d = &owner->entries[slot].rest.pos;
            d->x = a->pos.x - b->pos.x;
            d->y = a->pos.y - b->pos.y;
            d->z = a->pos.z - b->pos.z;
        }
        QuatRotateVec(&owner->entries[slot].rest.pos, &b->rot.x, &owner->entries[slot].rest.pos);
        inv.x = -b->rot.x;
        inv.y = -b->rot.y;
        inv.z = -b->rot.z;
        inv.w = b->rot.w;
        PSQUATMultiply(&a->rot, &inv, &owner->entries[slot].rest.rot);
    }

    {
        void* scratch = VAlloc(0x2000);
        if (scratch == 0)
            return 0;
        VFree(scratch);
    }

    attach = &sAttach[set->attachIndex];
    n = set->attachCount;
    for (int i = 0; i < n; i++) {
        u8020A650AttachName* at = &attach[i];
        int slotA = map[at->asset];
        int slotB = map[at->asset2];

        if (slotA == -1 || slotB == -1)
            continue;
        if (owner->entries[slotA].source == 0)
            continue;
        if (owner->entries[slotB].source == 0)
            continue;

        switch (at->type) {
        case 0: {
            Vec3 v1;
            Vec3 v2;
            Quat inv;
            Mtx34 attachMtx;
            Mtx34 boneMtx;
            u8020A650Anim* a = &((u8020A650Anim*)animBuf)[slotA];

            fn_801D4B5C(owner->ctx, owner->entries[slotA].source, owner->entries[slotB].source, &a->pos);
            v1.x = at->f0C;
            v1.y = at->f10;
            v1.z = at->f14;
            v2.x = at->f18;
            v2.y = at->f1C;
            v2.z = at->f20;
            if (owner->entries[slotA].source != 0) {
                BoneGetMatrix(&boneMtx, a);
                fn_800A0168(&boneMtx, &boneMtx);
                fn_800A0030(&v1, &boneMtx, &v1);
                fn_800A0030(&v2, &boneMtx, &v2);
            } else {
                QuatConjugate(&inv, &owner->entries[slotA].rest.rot);
                QuatRotateVecNorm(&v1, &inv.x, &v1);
                QuatRotateVecNorm(&v2, &inv.x, &v2);
            }

            attachMtx.m[0][0] = at->f24;
            attachMtx.m[0][1] = at->f28;
            attachMtx.m[0][2] = 50.0f;
            attachMtx.m[0][3] = 0.0f;
            attachMtx.m[1][0] = -0.57f;
            attachMtx.m[1][1] = 0.57f;
            attachMtx.m[1][2] = 50.0f;
            attachMtx.m[1][3] = 0.0f;
            attachMtx.m[2][0] = at->f30;
            attachMtx.m[2][1] = at->f34;
            attachMtx.m[2][2] = 50.0f;
            attachMtx.m[2][3] = 0.0f;
            fn_801D4824(owner->ctx, owner->entries[slotA].source,
                        owner->entries[slotB].source, &v1, &v2, 3, &attachMtx);
            break;
        }
        case 1: {
            Quat inv;
            Vec3 v1;
            Mtx34 boneMtx;
            PlaceF place;

            v1.x = at->f0C;
            v1.y = at->f10;
            v1.z = at->f14;
            if (owner->entries[slotA].source != 0) {
                BoneGetMatrix(&boneMtx, &((u8020A650Anim*)animBuf)[slotA]);
                fn_800A0168(&boneMtx, &boneMtx);
                fn_800A0030(&v1, &boneMtx, &v1);
            } else {
                QuatConjugate(&inv, &owner->entries[slotA].rest.rot);
                QuatRotateVecNorm(&v1, &inv.x, &v1);
            }

            if (owner->entries[slotA].source != 0) {
                fn_801D513C(owner->entries[slotA].source, &place);
                fn_801D4A70(owner->ctx, owner->entries[slotA].source,
                            owner->entries[slotB].source, &place.pos, &v1, &at->f24);
            } else {
                fn_801D4A70(owner->ctx, owner->entries[slotA].source,
                            owner->entries[slotB].source, &owner->entries[slotA].rest.pos, &v1, &at->f24);
            }
            break;
        }
        }
    }

    for (int i = 1; i < numParts; i++) {
        u8020A650PartName* part = &parts[i];
        int slot = map[part->asset];
        int slot2 = map[part->asset2];
        Vec3 delta;
        Quat q;
        Mtx34 m;

        if (slot == -1 || slot2 == -1)
            continue;
        delta.x = anims[slot2].pos.x - anims[slot].pos.x;
        delta.y = anims[slot2].pos.y - anims[slot].pos.y;
        delta.z = anims[slot2].pos.z - anims[slot].pos.z;
        if (owner->entries[slot].source == 0)
            continue;
        {
            float s;
            Vec3 up;
            Vec3 c;

            VecNormalize(&delta, &delta);
            up.x = 0.0f;
            up.y = 0.0f;
            up.z = 1.0f;
            VecCross(&c, &up, &delta);
            s = FastSqrt(2.0f * delta.z + 2.0f);
            if (s != 0.0f) {
                if (s == 0.0f) {
                    q.x = 0.0f;
                    q.y = 0.0f;
                    q.z = 0.0f;
                } else {
                    float inv = 1.0f / s;
                    q.x = c.x * inv;
                    q.y = c.y * inv;
                    q.z = c.z * inv;
                }
                q.w = 0.5f * s;
                PSQUATNormalize(&q, &q);
            } else {
                q.x = 0.0f;
                q.y = 0.0f;
                q.z = 0.0f;
                q.w = 1.0f;
            }
        }
        BoneGetMatrix(&m, &q);
        Mtx34StoreTranslation(&m, (Vec3*)&anims[slot].pos);
        fn_801D52E0(owner->entries[slot].source, &m);
    }

    for (i = 1; i < owner->count; i++) {
        Quat inv;

        if (owner->entries[i].source != 0)
            continue;
        Vec3Sub(&owner->entries[i].rest.pos, &anims[i].pos, &anims[owner->entries[i].index].pos);
        QuatRotateVec(&owner->entries[i].rest.pos, (float*)&anims[owner->entries[i].index].rot.x,
                      &owner->entries[i].rest.pos);
        QuatConjugate(&inv, &anims[owner->entries[i].index].rot);
        PSQUATMultiply((Quat*)&anims[i].rot, &inv, &owner->entries[i].rest.rot);
    }

    return 1;
}

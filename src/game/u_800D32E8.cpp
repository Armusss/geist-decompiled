
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned long u32;

struct Actor;
struct EmitterBank;
struct EmitterDesc;
typedef EmitterDesc EmitterSub;

// Emitter transform matrix: rows 0-2 are the rotation basis, row 3 holds the
// translation.
struct Mtx34 {
    float m[4][3];
};

// Placement form of a bone transform (same 4x3 layout as a matrix).
struct PlaceF {
    float m[4][3];
};

// Fixed-point (24.8) cull box of an emitter actor, 0x18 bytes.
struct CullBox {
    int minX;               // 0x00
    int minY;               // 0x04
    int minZ;               // 0x08
    int maxX;               // 0x0C
    int maxY;               // 0x10
    int maxZ;               // 0x14
};

struct Vec3 {
    float x;
    float y;
    float z;
};

// Axis-aligned box in world space (0x18 bytes).
struct Bounds {
    Vec3 min;
    Vec3 max;
};

// One spawn group of an emitter sub-descriptor (8 bytes).
struct EmitterGroup {
    int id;                 // 0x00
    u8 byte04;              // 0x04
    u8 pad05[3];
};

// One entry of the global emitter table (0xC0 bytes).
struct Emitter {
    u8 pad00[0x24];
    int posX;               // 0x24, 24.8 fixed point
    int posY;               // 0x28
    int posZ;               // 0x2C
    u8 pad30[8];
    int field38;            // 0x38
    u8 pad3C[0x10];
    int field4C;            // 0x4C
    CullBox bounds;         // 0x50
    u8 matrix[0x30];        // 0x68, Mtx34
    int field98;            // 0x98
    int handle;             // 0x9C
    int state;              // 0xA0
    EmitterBank* bank;      // 0xA4
    float fieldA8;          // 0xA8
    u32 flags;              // 0xAC
    float fieldB0;          // 0xB0
    float fieldB4;          // 0xB4
    float fieldB8;          // 0xB8
    float fieldBC;          // 0xBC
};

// Data reached through Emitter::bank.
struct EmitterBank {
    EmitterSub* sub;        // 0x00
    void* field04;          // 0x04
    void* field08;          // 0x08
    int field0C;            // 0x0C
    int field10;            // 0x10
    float field14;          // 0x14
    int field18;            // 0x18, rotating spawn-group cursor
    u8 pad1C[4];
};

// One 0x2C-byte spawn descriptor inside an emitter sub-descriptor.
struct EmitterParticle {
    float posX;             // 0x00
    float posY;             // 0x04
    float posZ;             // 0x08
    float rot[4];           // 0x0C
    int field1C;            // 0x1C, spawn type
    float field20;          // 0x20
    float field24;          // 0x24
    float field28;          // 0x28
};

// Spawn data (0x28 bytes) computed for one new particle.
struct EmitterSpawnData {
    Vec3 pos;               // 0x00
    Vec3 dir;               // 0x0C
    Vec3 field18;           // 0x18
    float field24;          // 0x24
};

// Live particle record (0x2C bytes): the spawn clock and its spawn data.
struct EmitterParticleState {
    float clock;            // 0x00
    EmitterSpawnData data;  // 0x04
};

// Per-particle render slot (0x14 bytes).
struct EmitterSlotData {
    int id;                 // 0x00
    int field04;            // 0x04, truncated fade value
    int field08;            // 0x08, truncated fade value
    u8 byte0C;              // 0x0C
    u8 byte0D;
    u8 byte0E;
    u8 byte0F;
    u8 byte10;
    u8 byte11;
    u8 byte12;
    u8 byte13;
};

// One emitter instance descriptor; the pool at lbl_804D495C holds 0x100 of
// these (0xEC bytes each).
struct EmitterDesc {
    float posX;             // 0x00
    float posY;             // 0x04
    float posZ;             // 0x08
    float rot[4];           // 0x0C
    u8 pad1C[4];            // 0x1C
    int state;              // 0x20
    float field24;          // 0x24
    int field28;            // 0x28, number of spawn groups
    EmitterGroup groups[8]; // 0x2C
    float field6C;          // 0x6C
    float field70;          // 0x70
    float field74;          // 0x74
    float field78;          // 0x78
    int field7C;            // 0x7C
    float field80;          // 0x80
    float field84;          // 0x84
    u32 field88;            // 0x88
    float field8C;          // 0x8C
    float field90;          // 0x90
    float field94;          // 0x94
    int field98;            // 0x98
    int count9C;            // 0x9C
    EmitterParticle* listA0; // 0xA0
    int countA4;            // 0xA4
    EmitterParticle* listA8; // 0xA8
    float fieldAC;          // 0xAC
    float fieldB0;          // 0xB0
    float fieldB4;          // 0xB4
    float fieldB8;          // 0xB8
    float fieldBC;          // 0xBC
    float fieldC0;          // 0xC0
    float fieldC4;          // 0xC4
    float fieldC8;          // 0xC8
    u8 byteCC;              // 0xCC
    u8 byteCD;              // 0xCD
    u8 byteCE;              // 0xCE
    u8 byteCF;              // 0xCF
    u8 byteD0;              // 0xD0
    u8 byteD1;              // 0xD1
    u8 byteD2;              // 0xD2
    u8 byteD3;              // 0xD3
    u8 padD4[8];            // 0xD4
    int fieldDC;            // 0xDC
    int fieldE0;            // 0xE0
    int fieldE4;            // 0xE4
    int fieldE8;            // 0xE8
};

// 0x20-byte emitter attachment record pool.
struct EmitterSlot {
    u8 pad00[0xC];
    int field0C;            // 0x0C
    u8 pad10[0x10];
};

// Source list of emitter descriptors the instance pools are seeded from.
struct EmitterSubList {
    int magic;              // 0x00, format tag (0x0D)
    int field04;            // 0x04
    int count;              // 0x08
    EmitterSub* subs;       // 0x0C
    int countA;             // 0x10
    EmitterParticle* listA; // 0x14
    int countB;             // 0x18
    EmitterParticle* listB; // 0x1C
};

// Message 0x800 payload: the emitter being released sits at +4.
struct EmitterReleaseMsg {
    int unk00;              // 0x00
    Emitter* emitter;       // 0x04
};

// Message block sent by EmitterAttach (0x40 bytes).
struct EmitterAttachMsg {
    u8 pad00[4];
    int value;              // 0x04
    int asset;              // 0x08
    u8 pad0C[0x34];
};

extern "C" Emitter* lbl_804D4960;
extern "C" int lbl_804D4964;
extern "C" EmitterBank* lbl_804D4954;
extern "C" int lbl_804D4958;
extern "C" EmitterSub* lbl_804D495C;
extern "C" EmitterSubList* lbl_804D4950;
extern "C" int lbl_804D4968;
extern "C" int lbl_804D496C;
extern "C" int lbl_804D4970;
extern "C" int lbl_804D4974;
extern "C" int lbl_804D4978;
extern "C" int lbl_804D497C;
extern "C" int lbl_804D4980;
extern "C" u8 __upper_map[];

extern "C" void* memset(void* dst, int c, u32 n);
extern "C" void SendMsg(int to, int msg, void* data);
extern "C" int fn_8021284C(int handle);
extern "C" int fn_80212814(int handle, int a, int b, int c, int d, int e);
extern "C" int fn_802127A0(int handle, int a, int b, int* c, int d, int e,
                           int f);
extern "C" void fn_8000F550(short* ids, int count);
void TextureLoad(short* ids, int count);
void ActorRefreshHostState(Actor* actor, int state);
int AssetFind(u32 id, u32 hash);
void ActorFn80022D98(Actor* actor, int value);
void ActorUnlinkOwnerObjects(Actor* actor);
void VFree(void* p);
void* VAlloc(int size);

extern "C" float lbl_804D7058;
extern "C" float lbl_804D705C;
extern "C" float lbl_804D7060;
extern "C" float lbl_804D7064;
extern "C" float lbl_804D7068;
extern "C" float lbl_804D7078;
extern "C" float lbl_804D707C;
extern "C" float lbl_804D7080;
extern "C" float lbl_804D7084;
extern "C" float lbl_804D7088;

extern "C" int fn_8000FECC(int handle);
extern "C" void fn_800A0168(void* dst, const void* src);
extern "C" void fn_800D72CC(void* em);
extern "C" int lbl_804D2BD0;
extern "C" float fn_802A2080(float t);
extern "C" void fn_800D4754(Emitter* em, float t);
extern "C" void fn_800D4A58(Emitter* em, float t);

void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void DeferAdd(int bank, Vec3* pos, void (*callback)(void*), void* data);

extern "C" void fn_800A0030(Vec3* out, const Mtx34* mtx, const Vec3* in);
extern "C" void fn_800A025C(Vec3* out, const Mtx34* mtx, int mode);
extern "C" void C_QUATSlerp(float t, const float* from, const float* to,
                            float* out);
extern "C" int fn_80026680(int id, void (*callback)(void), int arg);
extern "C" int fn_80026660(int id, void (*callback)(void), int arg);
extern "C" void fn_800D8664(u32 param_1, int* param_2, int param_3);
extern "C" int lbl_804D2BD4;
extern "C" double lbl_804D7050;
extern "C" float lbl_804D70A8;
extern "C" float lbl_804D70B0;
extern "C" float lbl_804D70B4;
extern "C" float lbl_804D70B8;
extern "C" void fn_80019A0C(void);
extern "C" int fn_800C64AC(void);
extern "C" float fn_800097B4(void);
extern "C" u32 fn_80010000(u32 handle);
extern "C" int fn_8000F904(u32 handle);
extern "C" void fn_8013AC68(u32 handle, int mode);
extern "C" void fn_8013A834(float a, float b, Vec3* pos, u8 c, u8 d, u8 e,
                            u8 f);
extern "C" void fn_8013A3D4(void);
extern "C" void fn_8013B6F8(float alpha, u32 handle, Mtx34* mtx);
extern "C" void fn_8013B608(Vec3* pos, Vec3* dir, u8 a, u8 b, u8 c, u8 d,
                            u8 e, u8 f, u8 g, u32 h);
extern "C" void fn_8013B5D4(void);
extern "C" void fn_8013B918(float alpha, u32 handle, Mtx34* mtx);
extern "C" void fn_8013B884(Vec3* pos, u8 a, u8 b, u8 c, u8 d);
extern "C" void fn_8013B850(void);
extern "C" void fn_8013A30C(u32 handle, Mtx34* mtx);
extern "C" void fn_8013A2E8(void);
extern "C" void fn_8009FC6C(Mtx34* out, Mtx34* a, Mtx34* b);
extern "C" void FileFlushBuffer(void* buf, u32 size);
extern "C" void fn_800070C0(void* p);
extern "C" void GXBegin(int prim, int fmt, int n);
void Vec3Set(Vec3* out, float x, float y, float z);
void MtxSetEuler(Mtx34* out, Vec3* euler);
int EmitterSubLookup(int handle, int arg, Emitter** out, int flag);
int EmitterResolveHandle(int arg, Emitter** out);
extern "C" int fn_800C5210(u32 handle);
extern "C" int fn_80084540(u32 handle, int arg, Emitter** out, int* result);
int EmitterMsgHandler(u32 arg, int msg, int* data);
void EmitterSubScaleParams(EmitterSub* sub);
Emitter* EmitterBuildFromSub(EmitterDesc* sub);

void PlacementToMatrix(Mtx34* out, PlaceF* place);
void ActorFn8002378C(Actor* actor, int a, int b, int c, int d, int e, int f);
void ActorFn800231A8(Actor* actor);
int SelectionSetTest(u32 handle);
void ObjSendC00(void* object, int value);
extern "C" float lbl_8039BD20[];

extern "C" float lbl_804D708C;
extern "C" float lbl_804D7090;
extern "C" float lbl_804D7094;
extern "C" float lbl_804D7098;
extern "C" float lbl_804D709C;
extern "C" float lbl_804D70AC;
extern "C" Mtx34 lbl_80451F00[];

extern "C" int rand(void);
extern "C" float sqrtf(float);
extern "C" float fabsf(float);
extern "C" double exp(double);
extern "C" float lbl_804D70A0;
extern "C" void* memcpy(void* dst, const void* src, u32 n);
int Rand(void);

extern "C" float fn_800A1418(float angle);
extern "C" float fn_800A1394(float angle);
extern "C" void PSQUATMultiply(const float* a, const float* b, float* out);
void BoneGetMatrix(Mtx34* mtx, void* quat);
void LinkInfoInit(void* info);
void ActorRelinkOwners(Actor* actor);
extern "C" void fn_8016DD6C(void* bounds, const Vec3* pos, int count);
extern "C" void fn_8016D724(void* bounds, const void* box);
extern "C" void EmitterParticleSpawnCommit(Emitter* em, Mtx34* mtx,
                                           EmitterParticleState* particle,
                                           EmitterSlotData* spare, float t);
extern "C" int EmitterParticleStep(Emitter* em, EmitterParticleState* particle,
                                   EmitterSlotData* spare, float t);
extern "C" void EmitterSpawnAccumForce(EmitterSpawnData* data, EmitterParticle* descs,
                            Mtx34* basis, int count);
void EmitterSlotRefresh(Emitter* em, EmitterParticleState* particle,
                        EmitterSlotData* spare);
void EmitterSpawnAccumDir(EmitterSpawnData* data, EmitterParticle* descs,
                          Mtx34* xform, int count);
void EmitterParticleUpdate(Emitter* em, float t);
void EmitterSpawnSample(float t, Emitter* em, EmitterBank* bank, EmitterSub* sub,
                        EmitterGroup* group, EmitterParticleState* particle,
                        EmitterSlotData* spare, Mtx34* basis);

// Uniform random value in [0, 1).
static inline float RandUnit(void)
{
    return (float)rand() / lbl_804D708C;
}

// Random offset in [-range/2, range/2) stepping by one.
static inline float RandRange(float range)
{
    int n = (int)fabsf(range);
    float v = lbl_804D705C;

    if (n != 0) {
        v = (float)(Rand() % n - n / 2);
    }
    return v;
}

// Look an emitter up by its handle; 0 when the handle is invalid or the
// entry is not active.
static inline Emitter* EmitterFromHandle(int handle)
{
    Emitter* em;

    if (handle != -1 && (int)(handle & 0xFFFFFF) >= 0 &&
        (int)(handle & 0xFFFFFF) < lbl_804D4964) {
        em = &lbl_804D4960[handle & 0xFFFFFF];
        if (em->field98 == 0) {
            em = 0;
        }
    } else {
        em = 0;
    }
    return em;
}

// Start an emitter at its current position, deferred to the end of the frame.
void EmitterDeferStart(int handle)
{
    Emitter* em = EmitterFromHandle(handle);

    if (em == 0) {
        return;
    }
    if (em != 0 && em->field98 != 0) {
        em->flags |= 4;
        if (em->state < 2 && em->state >= 0) {
            EmitterBank* bank = em->bank;
            int parent = 1;
            Vec3 pos;

            pos.x = (float)em->posX;
            pos.y = (float)em->posY;
            pos.z = (float)em->posZ;
            if (bank != 0 && bank->field0C != 0 && bank->sub != 0 &&
                bank->sub->field28 != 0) {
                parent = fn_8000FECC(bank->sub->groups[0].id);
            }
            DeferAdd(parent, &pos, (void (*)(void*))fn_800D72CC, em);
        }
    }
}

// Enable or disable an emitter. Returns 0 for a bad handle, 1 otherwise.
int EmitterSetEnabled(u32 handle, int on)
{
    int result;
    Emitter* em = EmitterFromHandle(handle);

    if (em == 0) {
        result = 0;
    } else {
        if (em != 0 && em->field98 != 0 && em->state < 2 && em->state >= 0 &&
            em != 0 && em->bank != 0 && em->bank->sub != 0) {
            if (on == 0) {
                em->flags &= ~1;
                em->fieldA8 = lbl_804D705C;
            } else {
                EmitterSub* sub = em->bank->sub;
                float f;

                em->flags |= 1;
                if (sub->field98 == 0) {
                    f = sub->field94;
                } else {
                    f = lbl_804D705C;
                    if (sub->field90 > lbl_804D7058) {
                        f = (float)sub->field7C / sub->field90;
                    }
                }
                f *= em->fieldB4;
                em->bank->field14 =
                    (f > lbl_804D7058) ? lbl_804D7060 / f : lbl_804D7064;
                em->fieldBC = lbl_804D705C;
                {
                    Mtx34 matrix;

                    ActorGetMatrix(&matrix, (Actor*)em);
                    fn_800A0168(em->matrix, &matrix);
                }
            }
        }
        result = 1;
    }
    return result;
}

// Attach an emitter asset (looked up by the hashed name) to an actor via
// message 0x415. The message block carries the owner id and the asset.
int EmitterAttach(int handle, int value, const char* name)
{
    EmitterAttachMsg msg;
    u32 hash;

    memset(&msg, 0, sizeof(msg));
    msg.value = value;
    if (name == 0) {
        hash = 0;
    } else {
        const s8* p = (const s8*)name;

        hash = 0;
        for (; *p != 0; p++) {
            s8 c = *p;
            u32 v;

            if (c == -1) {
                v = 0xFFFFFFFF;
            } else {
                v = __upper_map[(u8)c];
            }
            hash = hash * 0x1F + (v & 0xFF);
        }
        hash = hash % 0xFFFFFFFF;
    }
    msg.asset = AssetFind(0xF1754BF2, hash);
    SendMsg(handle, 0x415, &msg);
    return 0;
}

// Tear down a live emitter: release its bank objects and clear its slots in
// the three global emitter pools.
void EmitterDestroy(int handle)
{
    Emitter* em;
    EmitterBank* bank;
    int idx;

    if (handle == -1) {
        return;
    }
    if ((int)(handle & 0xFFFFFF) < 0) {
        return;
    }
    if ((int)(handle & 0xFFFFFF) >= lbl_804D4964) {
        return;
    }
    em = &lbl_804D4960[handle & 0xFFFFFF];
    if (em->field98 == 0) {
        return;
    }
    if (em->state != 1) {
        return;
    }
    bank = em->bank;
    fn_8021284C(em->field38);
    ActorFn80022D98((Actor*)em, 0);
    ActorUnlinkOwnerObjects((Actor*)em);
    VFree(bank->field04);
    bank->field04 = 0;
    VFree(bank->field08);
    bank->field08 = 0;
    idx = bank - lbl_804D4954;
    if (idx >= 0 && idx < lbl_804D4958) {
        lbl_804D4954[idx].field0C = 0;
    }
    idx = em - lbl_804D4960;
    if (idx >= 0 && idx < lbl_804D4964) {
        lbl_804D4960[idx].field98 = 0;
    }
    idx = bank->sub - lbl_804D495C;
    if (idx >= 0 && idx < 0x100) {
        lbl_804D495C[idx].field88 &= ~1u;
    }
}

// Blend the live emitter transform (bank->sub) between two descriptor
// snapshots: from at t=0 and to at t<max>. t is clamped to the configured
// range, the position is placed through the actor matrix and the remaining
// fields are lerped component-wise.
void EmitterDescBlend(float t, u32 handle, Actor* actor, EmitterDesc* from,
                      EmitterDesc* to)
{
    Emitter* em;
    EmitterSub* sub;
    Mtx34 mtx;
    Vec3 v;
    float tt;
    float s;

    if (handle == 0xFFFFFFFF) {
        return;
    }
    if ((handle >> 24) != 0x14) {
        return;
    }
    if ((int)(handle & 0xFFFFFF) >= lbl_804D4964) {
        return;
    }
    em = &lbl_804D4960[handle & 0xFFFFFF];
    if (em->field98 == 0) {
        return;
    }
    if (em->state != 0 && em->state != 1) {
        return;
    }

    sub = em->bank->sub;
    ActorGetMatrix(&mtx, actor);

    tt = lbl_804D705C;
    if (t >= lbl_804D705C) {
        tt = lbl_804D7068;
        if (t <= lbl_804D7068) {
            tt = t;
        }
    }
    s = lbl_804D7068 - tt;

    v.x = from->posX * s + to->posX * tt;
    v.y = from->posY * s + to->posY * tt;
    v.z = from->posZ * s + to->posZ * tt;
    fn_800A0030(&v, &mtx, &v);
    sub->posX = v.x + mtx.m[3][0];
    sub->posY = v.y + mtx.m[3][1];
    sub->posZ = v.z + mtx.m[3][2];

    C_QUATSlerp(tt, from->rot, to->rot, sub->rot);

    sub->field24 = from->field24 * s + to->field24 * tt;
    sub->field6C = from->field6C * s + to->field6C * tt;
    sub->field70 = from->field70 * s + to->field70 * tt;
    sub->field74 = from->field74 * s + to->field74 * tt;
    sub->field78 = from->field78 * s + to->field78 * tt;
    sub->field7C = (int)(from->field7C * s + to->field7C * tt);
    sub->field80 = from->field80 * s + to->field80 * tt;
    sub->field84 = from->field84 * s + to->field84 * tt;
    sub->field8C = from->field8C * s + to->field8C * tt;
    sub->field90 = from->field90 * s + to->field90 * tt;
    sub->field94 = from->field94 * s + to->field94 * tt;
    sub->fieldAC = from->fieldAC * s + to->fieldAC * tt;
    sub->fieldB0 = from->fieldB0 * s + to->fieldB0 * tt;
    sub->fieldB4 = from->fieldB4 * s + to->fieldB4 * tt;
    sub->fieldB8 = from->fieldB8 * s + to->fieldB8 * tt;
    sub->fieldBC = from->fieldBC * s + to->fieldBC * tt;
    sub->fieldC0 = from->fieldC0 * s + to->fieldC0 * tt;
    sub->fieldC4 = from->fieldC4 * s + to->fieldC4 * tt;
    sub->fieldC8 = from->fieldC8 * s + to->fieldC8 * tt;
    sub->byteCC = (u8)(from->byteCC * s + to->byteCC * tt);
    sub->byteCD = (u8)(from->byteCD * s + to->byteCD * tt);
    sub->byteCE = (u8)(from->byteCE * s + to->byteCE * tt);
    sub->byteCF = (u8)(from->byteCF * s + to->byteCF * tt);
    sub->byteD0 = (u8)(from->byteD0 * s + to->byteD0 * tt);
    sub->byteD1 = (u8)(from->byteD1 * s + to->byteD1 * tt);
    sub->byteD2 = (u8)(from->byteD2 * s + to->byteD2 * tt);
    sub->byteD3 = (u8)(from->byteD3 * s + to->byteD3 * tt);

    EmitterSubScaleParams(sub);
}

// Allocate a free slot in the 0x100-entry emitter pool, copy the descriptor
// into it, build the runtime emitter and return its handle (-1 on failure).
int EmitterCreate(EmitterDesc* desc)
{
    EmitterSub* slot = 0;
    EmitterSub* p;
    Emitter* em;
    int i;

    if (lbl_804D495C != 0) {
        i = 0;
        p = lbl_804D495C;
        for (; i < 0x100; i++, p++) {
            if ((p->field88 & 1) == 0) {
                memset(&lbl_804D495C[i], 0, sizeof(EmitterSub));
                lbl_804D495C[i].field88 |= 1;
                slot = &lbl_804D495C[i];
                break;
            }
        }
    }
    if (slot != 0) {
        *slot = *desc;
        slot->count9C = desc->count9C;
        slot->listA0 = desc->listA0;
        slot->countA4 = desc->countA4;
        slot->listA8 = desc->listA8;
        EmitterSubScaleParams(slot);
        em = EmitterBuildFromSub(slot);
        if (em != 0) {
            em->state = 1;
            return em->handle;
        }
        i = slot - lbl_804D495C;
        if (i >= 0 && i < 0x100) {
            lbl_804D495C[i].field88 &= ~1u;
        }
    }
    return -1;
}

// Destroy every live emitter and release the three global emitter pools.
void EmitterDestroyAll(void)
{
    int offset;
    int i;
    Emitter* em;

    offset = 0;
    for (i = 0; i < lbl_804D4964; i++) {
        em = (Emitter*)((u8*)lbl_804D4960 + offset);
        if (em->field98 != 0) {
            if (em->state == 1) {
                EmitterBank* bank = em->bank;

                if (em->state == 1) {
                    int idx;

                    fn_8021284C(em->field38);
                    ActorFn80022D98((Actor*)em, 0);
                    ActorUnlinkOwnerObjects((Actor*)em);
                    VFree(bank->field04);
                    bank->field04 = 0;
                    VFree(bank->field08);
                    bank->field08 = 0;
                    idx = bank - lbl_804D4954;
                    if (idx >= 0 && idx < lbl_804D4958) {
                        lbl_804D4954[idx].field0C = 0;
                    }
                    idx = em - lbl_804D4960;
                    if (idx >= 0 && idx < lbl_804D4964) {
                        lbl_804D4960[idx].field98 = 0;
                    }
                    idx = bank->sub - lbl_804D495C;
                    if (idx >= 0 && idx < 0x100) {
                        lbl_804D495C[idx].field88 &= ~1u;
                    }
                }
            } else if (em->state < 1 && em->state >= 0) {
                EmitterBank* bank = em->bank;
                int idx;

                fn_8021284C(em->field38);
                ActorFn80022D98((Actor*)em, 0);
                ActorUnlinkOwnerObjects((Actor*)em);
                VFree(bank->field04);
                bank->field04 = 0;
                VFree(bank->field08);
                bank->field08 = 0;
                idx = bank - lbl_804D4954;
                if (idx >= 0 && idx < lbl_804D4958) {
                    lbl_804D4954[idx].field0C = 0;
                }
                idx = em - lbl_804D4960;
                if (idx >= 0 && idx < lbl_804D4964) {
                    lbl_804D4960[idx].field98 = 0;
                }
            }
        }
        offset += 0xC0;
    }
    if (lbl_804D4954 != 0) {
        VFree(lbl_804D4954);
    }
    lbl_804D4954 = 0;
    lbl_804D4958 = 0;
    if (lbl_804D495C != 0) {
        VFree(lbl_804D495C);
    }
    lbl_804D495C = 0;
    if (lbl_804D4960 != 0) {
        VFree(lbl_804D4960);
    }
    lbl_804D4960 = 0;
    lbl_804D4964 = 0;
}

// Per-frame emitter update: advance every live emitter by the frame delta.
void EmitterUpdate(float t)
{
    int offset;
    int i;

    offset = 0;
    for (i = 0; i < lbl_804D4964; i++) {
        Emitter* em = (Emitter*)((u8*)lbl_804D4960 + offset);

        if (em->field98 != 0) {
            u32 flags = em->field4C;

            if ((flags & 0x400) != 0) {
                em->field4C = flags & ~0x400u;
            } else if (lbl_804D2BD0 != 0) {
                if ((flags & 0x20000) == 0) {
                    t = t * fn_802A2080(t);
                }
                if (em->state < 2 && em->state >= 0) {
                    fn_800D4754(em, t);
                }
            }
        }
        offset += 0xC0;
    }
}

// Tell every live emitter's owner (message 0x800) that the emitter table is
// being released; the payload carries the emitter itself at +4.
void EmitterNotifyRelease(void)
{
    EmitterReleaseMsg msg;
    int offset;
    int i;

    offset = 0;
    for (i = 0; i < lbl_804D4964; i++) {
        Emitter* em = (Emitter*)((u8*)lbl_804D4960 + offset);

        msg.emitter = em;
        SendMsg(em->handle, 0x800, &msg);
        offset += 0xC0;
    }
}

// Reset the emitter system: clear the bookkeeping counters, allocate the bank,
// descriptor and instance pools (plus the 0xEC00-byte sub-descriptor pool) and
// build every emitter configured in lbl_804D4950.
void EmitterSystemInit(void)
{
    EmitterSubList* list;
    int count;
    int offset;
    int i;

    lbl_804D4968 = 0;
    lbl_804D496C = 0;
    lbl_804D4970 = 0;
    lbl_804D4974 = 0;
    lbl_804D4980 = 0;
    lbl_804D4978 = 0;
    lbl_804D497C = 0;

    list = lbl_804D4950;
    count = (list != 0) ? list->count : 0;
    if ((lbl_804D4958 = count + 0x100) != 0) {
        lbl_804D4954 = (EmitterBank*)VAlloc(lbl_804D4958 * 0x20);
        memset(lbl_804D4954, 0, lbl_804D4958 << 5);
    } else {
        lbl_804D4954 = 0;
    }
    lbl_804D495C = (EmitterSub*)VAlloc(0xEC00);
    memset(lbl_804D495C, 0, 0xEC00);
    lbl_804D4964 = lbl_804D4958;
    if (lbl_804D4958 != 0) {
        lbl_804D4960 = (Emitter*)VAlloc(lbl_804D4958 * 0xC0);
        memset(lbl_804D4960, 0, lbl_804D4964 * 0xC0);
    } else {
        lbl_804D4960 = 0;
    }
    if (list != 0) {
        offset = 0;
        for (i = 0; i < list->count; i++) {
            EmitterSubScaleParams((EmitterSub*)((u8*)list->subs + offset));
            EmitterBuildFromSub((EmitterSub*)((u8*)list->subs + offset));
            offset += 0xEC;
        }
    }
}

// Scale one emitter sub-descriptor's parameters by the frame factors and mark
// it active.
void EmitterSubScaleParams(EmitterSub* sub)
{
    int i;

    sub->field70 = sub->field70 * lbl_804D7078;
    sub->field6C = sub->field6C * lbl_804D7078;

    if (sub->state != 1) {
        if (sub->state < 1) {
            if (sub->state >= 0) {
                sub->field80 = (sub->field80 * sub->field78) / lbl_804D707C;
                sub->field80 = sub->field80 * lbl_804D7078;
            }
        } else if (sub->state < 3) {
            sub->field80 = sub->field80 * lbl_804D7078;
        }
    }

    sub->fieldAC = sub->fieldAC * lbl_804D7078;
    sub->fieldB0 = sub->fieldB0 * lbl_804D7078;
    sub->fieldB4 = sub->fieldB4 * lbl_804D7060;
    sub->fieldB8 = sub->fieldB8 * lbl_804D7060;
    sub->fieldC4 = sub->fieldC4 * lbl_804D7060;
    sub->fieldC8 = sub->fieldC8 * lbl_804D7060;
    sub->field8C = sub->field8C * lbl_804D7060;
    sub->field78 = sub->field78 * lbl_804D7080;
    sub->field74 = sub->field74 * lbl_804D7080;

    for (i = 0; i < sub->countA4; i++) {
        EmitterParticle* p = &sub->listA8[i];

        p->field24 = p->field24 * lbl_804D7078;
        p->field20 = p->field20 * lbl_804D7078;
    }
    for (i = 0; i < sub->count9C; i++) {
        sub->listA0[i].field20 = sub->listA0[i].field20 * lbl_804D7084;
    }

    sub->field88 |= 1;
}

// Release the emitter system's global state and unregister its callbacks.
void EmitterSystemShutdown(void)
{
    lbl_804D4954 = 0;
    lbl_804D4958 = 0;
    lbl_804D495C = 0;
    lbl_804D4960 = 0;
    lbl_804D4964 = 0;
    lbl_804D4950 = 0;

    fn_80026680(0x14, (void (*)(void))fn_800D8664, 0);
    fn_80026660(0x14, (void (*)(void))EmitterSubLookup, 0);
}

// Step one emitter's animation clocks: grow or release the particle banks when
// the emitter enters or leaves the active state, advance the lifetime counters
// and emit the particles that have come due.
void fn_800D4754(Emitter* em, float t)
{
    EmitterBank* bank = em->bank;
    EmitterSub* sub;
    int grew = 0;

    if (bank->field0C == 0) {
        return;
    }
    if (em != 0 &&
        ((em->field4C & 0x40) != 0 || (em->field4C & 1) != 0 ||
         (em->field38 >> 24) == 0xC)) {
        if ((em->flags & 0x10) != 0) {
            if ((em->flags & 1) != 0) {
                if (bank->field10 == 0) {
                    sub = bank->sub;
                    bank->field10 = sub->field7C;
                    bank->field04 = VAlloc(sub->field7C * 0x2C);
                    if (bank->field04 == 0) {
                        bank->field10 = 0;
                    } else {
                        bank->field08 = VAlloc(sub->field7C * 0x14);
                        if (bank->field08 == 0) {
                            VFree(bank->field04);
                            bank->field04 = 0;
                            bank->field10 = 0;
                        }
                    }
                    if (bank->field10 > 0) {
                        grew = 1;
                    }
                }
            } else if (bank->field10 > 0) {
                em->fieldA8 = em->fieldA8 + t;
                if (lbl_804D7060 * bank->sub->field90 < em->fieldA8 &&
                    em->state < 2 && em->state >= 0 && bank->field10 > 0) {
                    bank->field10 = 0;
                    if (bank->field04 != 0) {
                        VFree(bank->field04);
                        bank->field04 = 0;
                    }
                    if (bank->field08 != 0) {
                        VFree(bank->field08);
                        bank->field08 = 0;
                    }
                }
            }
        } else if (bank->field10 > 0 && em->state < 2 && em->state >= 0) {
            bank->field10 = 0;
            if (bank->field04 != 0) {
                VFree(bank->field04);
                bank->field04 = 0;
            }
            if (bank->field08 != 0) {
                VFree(bank->field08);
                bank->field08 = 0;
            }
        }
    } else if (bank->field10 > 0 && em->state < 2 && em->state >= 0) {
        bank->field10 = 0;
        if (bank->field04 != 0) {
            VFree(bank->field04);
            bank->field04 = 0;
        }
        if (bank->field08 != 0) {
            VFree(bank->field08);
            bank->field08 = 0;
        }
    }

    em->flags &= ~4u;
    if (bank->field10 == 0) {
        em->fieldBC = em->fieldBC + t;
    } else {
        em->fieldB8 = t;
        if (lbl_804D7058 <= em->fieldBC ||
            lbl_804D705C < bank->sub->field8C) {
            if (bank->sub->field8C <= em->fieldBC + t) {
                if (grew) {
                    float next = em->fieldBC + t;

                    em->fieldBC = next;
                    t = lbl_804D7060 * bank->sub->field90;
                } else {
                    em->fieldBC = em->fieldBC + t;
                }
            } else {
                em->fieldBC = em->fieldBC + t;
                t = lbl_804D705C;
            }
        } else {
            float absValue = bank->sub->field8C;

            em->fieldBC = t;
            if (absValue < 0.0f) {
                absValue = -absValue;
            }
            t = t + absValue;
        }
        fn_800D4A58(em, t);
    }
}

// Per-emitter particle step: build the spawn transforms of the descriptor
// lists, advance the live particles, spawn the ones that are due and refresh
// the emitter matrix and bounds.
void EmitterParticleUpdate(Emitter* em, float t)
{
    EmitterBank* bank = em->bank;
    EmitterSub* sub;
    EmitterParticleState* particles;
    EmitterSlotData* spare;
    Mtx34 basis[16];
    Mtx34 mtx;
    Mtx34 subMtx;
    Bounds bounds;
    Vec3 delta;
    Vec3 pos;
    float invRot[4];
    float quat[4];
    float interval;
    float life;
    float fade;
    int spawned;
    int spawnIndex;
    int i;
    int g;

    if (t < lbl_804D7058) {
        return;
    }
    if (bank->field0C == 0) {
        return;
    }
    sub = bank->sub;
    spawned = 0;
    spawnIndex = 0;
    if (sub->field28 > 0) {
        particles = (EmitterParticleState*)bank->field04;
        spare = (EmitterSlotData*)bank->field08;
        interval = lbl_804D7060 * sub->field90;
        bounds.min.x = lbl_804D7088;
        bounds.min.y = lbl_804D7088;
        bounds.min.z = lbl_804D7088;
        bounds.max.x = lbl_804D7068;
        bounds.max.y = lbl_804D7068;
        bounds.max.z = lbl_804D7068;

        if ((sub->field88 & 0x40) == 0) {
            ActorGetMatrix(&mtx, (Actor*)em);
        } else {
            LinkInfoInit(&mtx);
        }

        if (sub->field98 == 0) {
            fade = sub->field94;
        } else if (sub->field90 > lbl_804D7058) {
            fade = (float)sub->field7C / sub->field90;
        } else {
            fade = lbl_804D705C;
        }
        fade = fade * em->fieldB4;
        life = (fade > lbl_804D7058) ? lbl_804D7060 / fade : lbl_804D7064;

        for (i = 0; i < sub->count9C; i++) {
            EmitterParticle* desc = &sub->listA0[i];

            invRot[0] = -sub->rot[0];
            invRot[1] = -sub->rot[1];
            invRot[2] = -sub->rot[2];
            invRot[3] = sub->rot[3];
            PSQUATMultiply(desc->rot, invRot, quat);
            BoneGetMatrix(&basis[i], quat);
            delta.x = desc->posX - sub->posX;
            delta.y = desc->posY - sub->posY;
            delta.z = desc->posZ - sub->posZ;
            BoneGetMatrix(&subMtx, sub->rot);
            fn_800A0030((Vec3*)basis[i].m[3], &subMtx, &delta);
        }

        for (i = 0; i < sub->countA4; i++) {
            EmitterParticle* desc = &sub->listA8[i];

            invRot[0] = -sub->rot[0];
            invRot[1] = -sub->rot[1];
            invRot[2] = -sub->rot[2];
            invRot[3] = sub->rot[3];
            PSQUATMultiply(desc->rot, invRot, quat);
            BoneGetMatrix(&lbl_80451F00[i], quat);
            delta.x = desc->posX - sub->posX;
            delta.y = desc->posY - sub->posY;
            delta.z = desc->posZ - sub->posZ;
            BoneGetMatrix(&subMtx, sub->rot);
            fn_800A0030((Vec3*)lbl_80451F00[i].m[3], &subMtx, &delta);
        }

        for (i = 0; i < bank->field10; i++) {
            if (particles[i].clock > lbl_804D7058) {
                EmitterSlotRefresh(em, &particles[i], &spare[i]);
                EmitterSpawnAccumForce(&particles[i].data, sub->listA0, basis, sub->count9C);
                spawned++;
            }
        }

        if ((em->flags & 8) != 0) {
            em->flags &= ~8u;
        }
        if ((em->flags & 1) != 0 && bank->field14 >= life) {
            int n;
            int groups[8];
            int cursor;
            int chunk;
            int start;
            float timer;

            n = sub->field28;
            if (n == 0) {
                n = 1;
            }
            for (i = 0; i < n; i++) {
                groups[i] = 0;
            }
            cursor = (sub->field28 != 0) ? bank->field18 : 0;
            timer = bank->field14;
            while (timer > lbl_804D7058) {
                groups[cursor]++;
                cursor = (sub->field28 != 0) ? (cursor + 1) % n : 0;
                timer -= life;
            }
            chunk = bank->field10 / n;
            if (chunk < 1) {
                chunk = 1;
            }
            start = 0;
            for (g = 0; g < n; g++) {
                int end = bank->field10;
                int todo = groups[g];
                int p = start;

                if (start + chunk < end) {
                    end = start + chunk;
                }
                for (; p < end && todo > 0; p++) {
                    if (particles[p].clock > lbl_804D7058) {
                        continue;
                    }
                    EmitterSpawnSample(interval, em, bank, sub, &sub->groups[g],
                                       &particles[p], &spare[p], basis);
                    EmitterSlotRefresh(em, &particles[p], &spare[p]);
                    EmitterSpawnAccumForce(&particles[p].data, sub->listA0, basis,
                                sub->count9C);
                    EmitterParticleSpawnCommit(em, &mtx, &particles[p],
                                               &spare[p],
                                               life * (float)spawnIndex);
                    spawned++;
                    spawnIndex++;
                    if (sub->field28 != 0) {
                        bank->field18 = (bank->field18 + 1) % sub->field28;
                    } else {
                        bank->field18 = -2;
                    }
                    todo--;
                    bank->field14 -= life;
                }
                start += chunk;
            }
        }

        {
            float limit = life * (float)bank->field10;

            if (bank->field14 > limit) {
                bank->field14 = limit;
            }
        }

        if (spawned != 0) {
            for (i = 0; i < bank->field10; i++) {
                if (particles[i].clock <= lbl_804D705C) {
                    continue;
                }
                EmitterParticleSpawnCommit(em, &mtx, &particles[i], &spare[i],
                                           t);
                pos.x = (float)em->posX;
                pos.y = (float)em->posY;
                pos.z = (float)em->posZ;
                if ((sub->field88 & 0x40) != 0) {
                    Mtx34 world;

                    ActorGetMatrix(&world, (Actor*)em);
                    pos.x = particles[i].data.pos.x - pos.x;
                    pos.y = particles[i].data.pos.y - pos.y;
                    pos.z = particles[i].data.pos.z - pos.z;
                    fn_800A0030(&pos, &world, &pos);
                } else {
                    pos = particles[i].data.pos;
                }
                fn_8016DD6C(&bounds, &pos, 1);
            }

            {
                float radius = sub->field80;

                if (radius < sub->fieldB0) {
                    radius = sub->fieldB0;
                }
                if (radius < sub->fieldAC) {
                    radius = sub->fieldAC;
                }
                bounds.min.x -= radius;
                bounds.min.y -= radius;
                bounds.min.z -= radius;
                bounds.max.x += radius;
                bounds.max.y += radius;
                bounds.max.z += radius;
            }
            fn_8016D724(&em->bounds, &bounds);
        }
    }

    ActorRelinkOwners((Actor*)em);
    if ((em->flags & 1) != 0) {
        Mtx34 world;

        ActorGetMatrix(&world, (Actor*)em);
        fn_800A0168(em->matrix, &world);
    }
    bank->field14 += t;
}

// Sample the spawn position, direction and render-slot data of one particle.
void EmitterSpawnSample(float t, Emitter* em, EmitterBank* bank, EmitterSub* sub,
                        EmitterGroup* group, EmitterParticleState* particle,
                        EmitterSlotData* spare, Mtx34* basis)
{
    EmitterSpawnData data;
    Mtx34 matrix;
    Vec3 dir;
    Vec3 off;

    if (sub->field88 & 0x20) {
        float radius = sub->field70;
        float z = lbl_804D7090 * RandUnit() - lbl_804D7068;
        float theta = lbl_804D7098 * (lbl_804D7094 * RandUnit());
        float r = lbl_804D705C;

        if (lbl_804D7068 - z * z > lbl_804D705C) {
            r = sqrtf(lbl_804D7068 - z * z);
        }
        dir.x = r * fn_800A1418(theta) * radius;
        dir.y = r * fn_800A1394(theta) * radius;
        dir.z = z * radius;
    } else {
        dir.x = RandRange(sub->field70);
        dir.y = RandRange(sub->field6C);
        dir.z = lbl_804D705C + RandRange(lbl_804D705C);
    }

    if (sub->field88 & 0x20) {
        if (((fabsf(dir.x) > lbl_804D7058) + (fabsf(dir.y) > lbl_804D7058) +
             (fabsf(dir.z) > lbl_804D7058)) == 0) {
            off.x = lbl_804D705C;
            off.y = lbl_804D705C;
            off.z = lbl_804D705C;
        } else {
            float inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y +
                                     dir.z * dir.z);

            off.x = dir.x * inv;
            off.y = dir.y * inv;
            off.z = dir.z * inv;
        }
        off.x *= em->fieldB0;
        off.y *= em->fieldB0;
        off.z *= em->fieldB0;
    } else {
        float spread = lbl_804D7090 * sub->field74;

        off.x = RandRange(spread);
        off.y = RandRange(spread);
        off.z = -em->fieldB0 + RandRange(spread);
    }

    memset(&data, 0, sizeof(data));
    data.pos = dir;
    data.dir = off;
    EmitterSpawnAccumDir(&data, sub->listA0, basis, sub->count9C);

    if (sub->field88 & 0x10) {
        data.field24 = lbl_804D709C * RandUnit();
    } else {
        data.field24 = lbl_804D705C;
    }

    if (sub->field88 & 0x40) {
        ActorGetMatrix(&matrix, (Actor*)em);
        fn_800A0168(&matrix, &matrix);
        fn_800A0030(&data.pos, &matrix, &data.pos);
        data.pos.x += matrix.m[3][0];
        data.pos.y += matrix.m[3][1];
        data.pos.z += matrix.m[3][2];
        fn_800A0030(&data.dir, &matrix, &data.dir);
        fn_800A0030(&data.field18, &matrix, &data.field18);
    }

    if (particle != 0) {
        particle->clock = t;
        particle->data = data;
    }
    if (spare != 0) {
        spare->id = group->id;
        spare->field04 = sub->field80;
        spare->field08 = sub->field80;
        spare->byte0C = sub->byteCC;
        spare->byte0D = sub->byteCD;
        spare->byte0E = sub->byteCE;
        spare->byte0F = group->byte04;
    }
    if (sub->state == 0) {
        spare->field04 = sub->field84;
        spare->byte10 = sub->byteD0;
        spare->byte11 = sub->byteD1;
        spare->byte12 = sub->byteD2;
        spare->byte13 = sub->byteD3;
    }
}

// Accumulates, over every spawn descriptor, the force this emitter applies to
// the point at data->pos: the descriptor's direction (either derived from the
// transform translation or supplied by the descriptor) scaled by its decay and
// by an exponential distance falloff.  The sum lands in data->field18.
extern "C" void EmitterSpawnAccumForce(EmitterSpawnData* data, EmitterParticle* descs,
                            Mtx34* basis, int count)
{
    float sx = lbl_804D705C;
    float sy = sx;
    float sz = sx;
    int i;

    for (i = 0; i < count; i++) {
        float decay = descs->field20;
        float dist = descs->field24;
        float len = lbl_804D705C;
        Vec3 dir;

        if (dist < lbl_804D705C) {
            dist = lbl_804D705C;
        }
        dir.x = lbl_804D705C;
        dir.y = lbl_804D705C;
        dir.z = lbl_804D705C;

        switch (descs->field1C) {
        case 1:
            dir.x = (basis->m[3][0] - data->pos.x) * lbl_804D70A0;
            dir.y = (basis->m[3][1] - data->pos.y) * lbl_804D70A0;
            dir.z = (basis->m[3][2] - data->pos.z) * lbl_804D70A0;
            if (fabsf(dir.x) > lbl_804D7058 ||
                fabsf(dir.y) > lbl_804D7058 ||
                fabsf(dir.z) > lbl_804D7058) {
                float inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y +
                                         dir.z * dir.z);

                dir.x *= inv;
                dir.y *= inv;
                dir.z *= inv;
                len = 1.0f / inv;
            } else {
                dir.x = lbl_804D705C;
                dir.y = lbl_804D705C;
                dir.z = lbl_804D705C;
            }
            if (dist > lbl_804D7058) {
                decay *= exp(-dist * len);
            }
            dir.x *= decay;
            dir.y *= decay;
            dir.z *= decay;
            break;
        case 0:
            fn_800A025C(&dir, basis, 2);
            if (dist > lbl_804D7058) {
                decay *= exp(-dist *
                             (dir.y * (data->pos.y - basis->m[3][1]) *
                                  lbl_804D70A0 +
                              dir.x * (data->pos.x - basis->m[3][0]) *
                                  lbl_804D70A0 +
                              dir.z * (data->pos.z - basis->m[3][2]) *
                                  lbl_804D70A0));
            }
            dir.x *= decay;
            dir.y *= decay;
            dir.z *= decay;
            break;
        }

        descs++;
        basis++;
        sx += dir.x;
        sy += dir.y;
        sz += dir.z;
    }

    data->field18.x = sx;
    data->field18.y = sy;
    data->field18.z = sz;
}

// Commits one spawned particle record.  The transform is not needed here: the
// per-particle work is done by EmitterParticleStep.
extern "C" void EmitterParticleSpawnCommit(Emitter* em, Mtx34* mtx,
                                           EmitterParticleState* particle,
                                           EmitterSlotData* spare, float t)
{
    EmitterParticleStep(em, particle, spare, t);
}

int EmitterSampleSpawnOffset(Emitter* em, EmitterParticleState* particle,
                             EmitterSpawnData* src, u32 flags, float t);

// Steps a live particle's spawn clock when a spare render slot is free.
extern "C" int EmitterParticleAdvance(Emitter* em, EmitterParticleState* particle,
                                      float t);

// Returns -1 with no spare slot, 0 while the particle's clock is still empty,
// and otherwise advances the clock one step.
extern "C" int EmitterParticleStep(Emitter* em, EmitterParticleState* particle,
                                   EmitterSlotData* spare, float t)
{
    if (spare == 0)
        return -1;
    if (particle->clock <= lbl_804D705C)
        return 0;
    return EmitterParticleAdvance(em, particle, t);
}

extern "C" float lbl_804D70A4;

// Advances one particle spawn clock by t and samples its spawn offset.
extern "C" int EmitterParticleAdvance(Emitter* em, EmitterParticleState* particle,
                                      float t)
{
    if (particle->clock > lbl_804D705C) {
        if (particle->clock != lbl_804D70A4)
            particle->clock -= t;
        EmitterSampleSpawnOffset(em, particle, &particle->data, 0, t);
        if (particle->clock > lbl_804D705C)
            return 1;
    }
    return 0;
}

extern "C" float lbl_804D70A8;

void EmitterSpawnOffset(float t, EmitterSpawnData* src, Vec3* pos, Vec3* dir,
                        float* scale, Emitter* em, float* out);

// Builds one particle's spawn position and direction from a spawn record.
int EmitterComputeSpawnOffset(Emitter* em, EmitterParticleState* particle,
                              EmitterSpawnData* src, int flags, float t);

// Scales the time step and forwards to the spawn-position builder.
int EmitterSampleSpawnOffset(Emitter* em, EmitterParticleState* particle,
                             EmitterSpawnData* src, u32 flags, float t)
{
    return EmitterComputeSpawnOffset(em, particle, src, flags,
                                     t / lbl_804D7060);
}

// Builds one particle's spawn position and direction from a spawn record.
int EmitterComputeSpawnOffset(Emitter* em, EmitterParticleState* particle,
                              EmitterSpawnData* src, int flags, float t)
{
    float scale;
    Vec3 dir;
    Vec3 pos;

    if (src == 0)
        return 0;

    if ((flags & 2) == 0) {
        dir.x = src->field18.x * t + src->dir.x;
        dir.y = src->field18.y * t + src->dir.y;
        dir.z = src->field18.z * t + src->dir.z;
    } else {
        dir.x = src->dir.x;
        dir.y = src->dir.y;
        dir.z = src->dir.z;
    }

    if ((flags & 4) == 0) {
        if ((flags & 2) == 0) {
            float s = t * lbl_804D70A8;

            pos.x = s * (src->dir.x + dir.x) + src->pos.x;
            pos.y = s * (src->dir.y + dir.y) + src->pos.y;
            pos.z = s * (src->dir.z + dir.z) + src->pos.z;
        } else {
            pos.x = src->dir.x * t + src->pos.x;
            pos.y = src->dir.y * t + src->pos.y;
            pos.z = src->dir.z * t + src->pos.z;
        }
    } else {
        pos.x = src->pos.x;
        pos.y = src->pos.y;
        pos.z = src->pos.z;
    }

    scale = src->field24;
    EmitterSpawnOffset(t, src, &pos, &dir, &scale, em, (float*)particle);
    return 1;
}

// Fades one per-particle parameter over the emitter's lifetime: it ramps from
// start to mid over the first `rise` units, holds mid, then ramps from mid to
// end over the last `fall` units of the total lifetime.
static float EmitterRampParam(float t, float total, float rise, float fall,
                              float start, float mid, float end)
{
    if (t < rise) {
        float s = lbl_804D7068;

        if (rise > lbl_804D7058) {
            s = t / rise;
        }
        return s * (mid - start) + start;
    }
    if (total - fall < t) {
        float s = lbl_804D7068;

        if (fall > lbl_804D7058) {
            s = (t - (total - fall)) / fall;
        }
        return s * (end - mid) + mid;
    }
    return mid;
}

// Refreshes a spare render slot from the emitter sub-descriptor: the slot's
// fade bytes and its two parameter values are ramped over the particle's
// remaining lifetime.
void EmitterSlotRefresh(Emitter* em, EmitterParticleState* particle,
                        EmitterSlotData* spare)
{
    EmitterBank* bank = em->bank;
    EmitterSub* sub;
    float total;
    float t;
    float value;
    int i;

    if (bank->field0C == 0) {
        return;
    }
    sub = bank->sub;
    total = lbl_804D7060 * sub->field90;
    t = total - particle->clock;

    if ((em->flags & 8) != 0) {
        spare->byte0C = sub->byteCC;
        spare->byte0D = sub->byteCD;
        spare->byte0E = sub->byteCE;
        spare->byte0F = sub->byteCF;
    }

    value = lbl_804D7068;
    for (i = 0; i < sub->field28; i++) {
        if (sub->groups[i].id == spare->id) {
            value = (float)sub->groups[i].byte04 / lbl_804D70AC;
        }
    }
    value = EmitterRampParam(t, total, sub->fieldC4, sub->fieldC8,
                             sub->fieldBC, value, sub->fieldC0);
    spare->byte0F = (u8)(int)(lbl_804D70AC * value);

    if (sub->state == 0) {
        value = (float)sub->byteD3;
        if (lbl_804D70AC == value) {
            for (i = 0; i < sub->field28; i++) {
                if (sub->groups[i].id == spare->id) {
                    value = (float)sub->groups[i].byte04;
                }
            }
        }
        value /= lbl_804D70AC;
        value = EmitterRampParam(t, total, sub->fieldC4, sub->fieldC8,
                                 sub->fieldBC, value, sub->fieldC0);
        spare->byte13 = (u8)(int)(lbl_804D70AC * value);
        spare->field08 = (int)EmitterRampParam(t, total, sub->fieldB4,
                                               sub->fieldB8, sub->fieldAC,
                                               sub->field80, sub->fieldB0);
        spare->field04 = (int)EmitterRampParam(t, total, sub->fieldB4,
                                               sub->fieldB8, sub->fieldAC,
                                               sub->field84, sub->fieldB0);
    } else {
        value = EmitterRampParam(t, total, sub->fieldB4, sub->fieldB8,
                                 sub->fieldAC, sub->field80, sub->fieldB0);
        spare->field08 = (int)value;
        spare->field04 = (int)value;
    }
}

void EmitterSpawnTracePick(float t, EmitterSpawnData* data, Vec3* pos,
                           Vec3* dir, float* speed, Emitter* em, float* out);

// Validates a spawn record against its live emitter and hands it to the spawn
// trace picker.
void EmitterSpawnOffset(float t, EmitterSpawnData* src, Vec3* pos, Vec3* dir,
                        float* scale, Emitter* em, float* out)
{
    if (src != 0 && em != 0 && em->field98 != 0 && em->state < 2 &&
        em->state >= 0) {
        EmitterSpawnTracePick(t, src, pos, dir, scale, em, out);
    }
}

extern "C" void qsort(void* base, u32 count, u32 size,
                      int (*compare)(const void*, const void*));
int ModelBoneHitFlush(Vec3** corner, Vec3* pos, Vec3* dir, float* t, float* u, float* v);  // 0x800187F4
int ActorPlayAnim(int object, u32 anim, int flags);                                      // 0x800C5654

// The sub-descriptor's particle list stores a float at 0x1C where the render
// list stores the spawn type, so the trace pass views the same 0x2C-byte
// records through this layout.
struct EmitterSpawnParticle {
    float posX;             // 0x00
    float posY;             // 0x04
    float posZ;             // 0x08
    float rot[4];           // 0x0C
    float field1C;          // 0x1C, distance scale
    float field20;          // 0x20
    float field24;          // 0x24
    float field28;          // 0x28
};

// Cached spawn-particle transform, the spawn quad and the point/ray the
// particle is tested against (0xBC bytes).
struct EmitterTraceSlot {
    Mtx34 basis;            // 0x00
    Mtx34 basisCopy;        // 0x30
    Vec3 corners[4];        // 0x60
    Vec3* corner[4];        // 0x90
    Vec3 pos;               // 0xA0
    Vec3 dir;               // 0xAC
    float speed;            // 0xB8, maximum hit distance
};

// One accepted trace hit, sorted by hit distance (0x10 bytes).
struct EmitterTraceHit {
    float t;                // 0x00
    float u;                // 0x04
    float v;                // 0x08
    int index;              // 0x0C, spawn-particle index
};

// qsort comparator for the accepted hits: ascending hit distance.
int EmitterHitParamCompare(const void* a, const void* b)
{
    return (int)(*(const float*)a - *(const float*)b);
}

// Builds the spawn quad of every particle of the sub-descriptor's second list,
// casts the spawn point against them and derives the spawn position,
// direction and speed from the nearest hit. When nothing is hit the spawn
// record is reset to the caller's defaults.
void EmitterSpawnTracePick(float t, EmitterSpawnData* data, Vec3* pos,
                           Vec3* dir, float* speed, Emitter* em, float* out)
{
    EmitterSub* sub = em->bank->sub;
    EmitterSpawnParticle* descs = (EmitterSpawnParticle*)sub->listA8;
    EmitterTraceSlot slots[8];
    EmitterTraceHit found[8];
    int count = sub->countA4;
    int hits = 0;
    int i;
    int j;

    if (count < 0) {
        count = 0;
    } else if (count > 8) {
        count = 8;
    }

    // Cache each particle's basis, its copy and the four corners of the quad
    // it describes.
    for (i = 0; i < count; i++) {
        EmitterTraceSlot* slot = &slots[i];
        float a;
        float b;

        slot->basis = lbl_80451F00[i];
        fn_800A0168(&slot->basisCopy, &slot->basis);
        slot->basisCopy.m[3][0] = slot->basis.m[3][0];
        slot->basisCopy.m[3][1] = slot->basis.m[3][1];
        slot->basisCopy.m[3][2] = slot->basis.m[3][2];
        a = lbl_804D70A8 * descs[i].field20;
        b = lbl_804D70A8 * descs[i].field24;
        slot->corners[0].x = -a;
        slot->corners[0].y = -b;
        slot->corners[0].z = lbl_804D705C;
        slot->corners[1].x = -a;
        slot->corners[1].y = b;
        slot->corners[1].z = lbl_804D705C;
        slot->corners[2].x = a;
        slot->corners[2].y = b;
        slot->corners[2].z = lbl_804D705C;
        slot->corners[3].x = a;
        slot->corners[3].y = -b;
        slot->corners[3].z = lbl_804D705C;
        slot->corner[0] = &slot->corners[0];
        slot->corner[1] = &slot->corners[1];
        slot->corner[2] = &slot->corners[2];
        slot->corner[3] = &slot->corners[3];
    }

    // Place the spawn point into each particle's space and carry the spawn
    // direction and the maximum hit distance with it.
    for (i = 0; i < count; i++) {
        EmitterTraceSlot* slot = &slots[i];
        float len;

        slot->pos.x = data->pos.x - slot->basis.m[3][0];
        slot->pos.y = data->pos.y - slot->basis.m[3][1];
        slot->pos.z = data->pos.z - slot->basis.m[3][2];
        fn_800A0030(&slot->pos, &slot->basis, &slot->pos);
        if (fabsf(data->dir.x) > lbl_804D7058 ||
            fabsf(data->dir.y) > lbl_804D7058 ||
            fabsf(data->dir.z) > lbl_804D7058) {
            float inv = 1.0f / sqrtf(data->dir.x * data->dir.x +
                                     data->dir.y * data->dir.y +
                                     data->dir.z * data->dir.z);

            slot->dir.x = data->dir.x * inv;
            slot->dir.y = data->dir.y * inv;
            slot->dir.z = data->dir.z * inv;
            len = 1.0f / inv;
        } else {
            slot->dir.x = lbl_804D705C;
            slot->dir.y = lbl_804D705C;
            slot->dir.z = lbl_804D705C;
            len = lbl_804D705C;
        }
        slot->speed = t * len;
        fn_800A0030(&slot->dir, &slot->basis, &slot->dir);
    }

    // Cast against each quad's two leading corners.
    for (i = 0; i < count; i++) {
        EmitterTraceSlot* slot = &slots[i];

        for (j = 0; j < 2; j++) {
            int hit = ModelBoneHitFlush(&slot->corner[j], &slot->pos,
                                            &slot->dir, &found[hits].t,
                                            &found[hits].u, &found[hits].v);

            if (hit != 0 && found[hits].t > lbl_804D7058 &&
                found[hits].t <= slot->speed) {
                found[hits].index = i;
                hits++;
            }
        }
    }

    if (hits > 1) {
        qsort(found, hits, sizeof(EmitterTraceHit), EmitterHitParamCompare);
    }

    if (hits < 1) {
        // Nothing was hit: fall back to the caller's defaults.
        data->dir.x = dir->x;
        data->dir.y = dir->y;
        data->dir.z = dir->z;
        data->pos.x = pos->x;
        data->pos.y = pos->y;
        data->pos.z = pos->z;
        data->field24 = *speed;
    } else {
        int idx = found[0].index;
        EmitterTraceSlot* slot = &slots[idx];
        Vec3 v;
        Vec3 w;
        Vec3 origin;
        float len;

        // Direction towards the hit.
        if (fabsf(data->dir.x) > lbl_804D7058 ||
            fabsf(data->dir.y) > lbl_804D7058 ||
            fabsf(data->dir.z) > lbl_804D7058) {
            float inv = 1.0f / sqrtf(data->dir.x * data->dir.x +
                                     data->dir.y * data->dir.y +
                                     data->dir.z * data->dir.z);

            v.x = data->dir.x * inv;
            v.y = data->dir.y * inv;
            v.z = data->dir.z * inv;
            len = 1.0f / inv;
        } else {
            v.x = lbl_804D705C;
            v.y = lbl_804D705C;
            v.z = lbl_804D705C;
            len = lbl_804D705C;
        }

        w.x = v.x * found[0].t;
        w.y = v.y * found[0].t;
        w.z = v.z * found[0].t;
        origin.x = data->pos.x + w.x;
        origin.y = data->pos.y + w.y;
        origin.z = data->pos.z + w.z;

        fn_800A0030(&w, &slot->basis, &data->dir);
        w.z *= lbl_804D7088;
        w.x *= descs[idx].field1C;
        w.y *= descs[idx].field1C;
        w.z *= descs[idx].field1C;
        fn_800A0030(&data->dir, &slot->basisCopy, &w);

        // Re-normalise the resulting direction.
        if (fabsf(data->dir.x) > lbl_804D7058 ||
            fabsf(data->dir.y) > lbl_804D7058 ||
            fabsf(data->dir.z) > lbl_804D7058) {
            float inv = 1.0f / sqrtf(data->dir.x * data->dir.x +
                                     data->dir.y * data->dir.y +
                                     data->dir.z * data->dir.z);

            v.x = data->dir.x * inv;
            v.y = data->dir.y * inv;
            v.z = data->dir.z * inv;
            len = 1.0f / inv;
        } else {
            v.x = lbl_804D705C;
            v.y = lbl_804D705C;
            v.z = lbl_804D705C;
            len = lbl_804D705C;
        }

        // Push the spawn point the remaining distance along the hit direction.
        if (len > lbl_804D7058) {
            float k = slot->speed - found[0].t;

            w.x = v.x * k;
            w.y = v.y * k;
            w.z = v.z * k;
            data->pos.x = origin.x + w.x;
            data->pos.y = origin.y + w.y;
            data->pos.z = origin.z + w.z;
        }

        data->field24 = *speed;
        ActorPlayAnim(em->handle, 0xF94BF500u, em->handle);
        if (descs[idx].field1C <= lbl_804D7058) {
            *out = lbl_804D705C;
        }
    }
}

// Resolve the handle of the emitter stored in *out and look its emitter
// sub-descriptor up.
int EmitterResolveHandle(int arg, Emitter** out)
{
    int result;

    if (*out == 0) {
        result = 0;
    } else {
        result = EmitterSubLookup((*out)->handle, arg, out, 0);
    }
    return result;
}

// Look the emitter-sub resource of a 0x14-type handle up in the global emitter
// table and run its load chain.
int EmitterSubLookup(int handle, int arg, Emitter** out, int flag)
{
    int result = 0;
    int index = handle & 0xFFFFFF;
    u32 id;
    Emitter* entry;

    if (index < 0 || index >= lbl_804D4964) {
        return 0;
    }
    entry = &lbl_804D4960[index];
    if (entry->field98 == 0) {
        return 0;
    }
    *out = entry;
    id = index | 0x14000000;
    result = fn_800C5210(id);
    if (result == 0) {
        result = fn_80084540(id, arg, out, &result);
        if (result == 0) {
            result = EmitterMsgHandler(id, arg, (int*)out);
        }
    }
    return result;
}

// Emitter message handler.  data is the message block; data[0] is the
// emitter the message is about and the remaining words are the payload.
int EmitterMsgHandler(u32 arg, int msg, int* data)
{
    Emitter* em;
    EmitterSub* sub;

    switch (msg) {
    case 0x415:
        em = (Emitter*)data[0];
        if (data[1] == -1) {
            return fn_8021284C(em->field38);
        }
        if (data[3] == 0) {
            return fn_80212814(em->field38, data[1], data[2], 1, 0, 0);
        }
        return fn_802127A0(em->field38, data[1], data[2], data + 4, 1, 0, 0);

    case 0x414:
        em = (Emitter*)data[0];
        {
            float t = *(float*)(data + 1);

            if (lbl_804D2BD0 != 0) {
                if ((em->field4C & 0x20000) == 0) {
                    t = t * fn_802A2080(t);
                }
                if (em->state < 2 && em->state >= 0) {
                    fn_800D4754(em, t);
                }
            }
            em->field4C |= 0x400;
        }
        return 1;

    case 0x413:
        em = (Emitter*)data[0];
        if (em == 0 || em->field98 == 0) {
            return 0;
        }
        memcpy(em, (const void*)data[1], 0x30);
        return 1;

    case 0x411:
    case 0x412:
        em = (Emitter*)data[0];
        if (em == 0 || em->field98 == 0) {
            return 0;
        }
        memcpy(data + 4, em, 0x40);
        return 1;

    case 0x400:
        if (arg != 0xFFFFFFFF) {
            u32 index = arg & 0xFFFFFF;

            if ((int)index >= 0 && (int)index < lbl_804D4964) {
                data[1] = (int)arg;
                data[3] = lbl_804D4960[index].field98;
                ((float*)data)[4] = lbl_804D7068;
                ((float*)data)[5] = lbl_804D7068;
                data[6] = 0;
                data[7] = 0;
            }
        }
        return 1;

    case 0:
        em = (Emitter*)data[0];
        if (em != 0 && em->field98 != 0) {
            em->flags |= 4;
            if (em->state < 2 && em->state >= 0) {
                EmitterBank* bank = em->bank;
                int parent = 1;
                Vec3 pos;

                pos.x = (float)em->posX;
                pos.y = (float)em->posY;
                pos.z = (float)em->posZ;
                if (bank != 0 && bank->field0C != 0 && bank->sub != 0 &&
                    bank->sub->field28 != 0) {
                    parent = fn_8000FECC(bank->sub->groups[0].id);
                }
                DeferAdd(parent, &pos, fn_800D72CC, em);
            }
        }
        return 1;

    case 5:
        return 1;

    case 6:
        em = (Emitter*)data[0];
        if (em->state < 2 && em->state >= 0) {
            short ids[8];
            int n = 0;
            int i;

            for (i = 0; i < em->bank->sub->field28; i++) {
                int id = em->bank->sub->groups[i].id;

                if (id >= 0) {
                    ids[n] = (short)id;
                    n++;
                }
            }
            if (data[1] == 0) {
                em->flags &= ~0x10u;
                fn_8000F550(ids, n);
            } else {
                em->flags |= 0x10;
                TextureLoad(ids, n);
            }
        }
        return 1;

    case 0x41a:
        em = (Emitter*)data[0];
        if (em != 0 && em->field98 != 0) {
            data[1] = AssetFind(0xFFFFFFFF, em->bank->sub->fieldDC);
        }
        return 1;

    case 0x41e:
        em = (Emitter*)data[0];
        if (data[1] == 0x7C18FE9E) {
            data[3] = AssetFind(0xFFFFFFFF, em->bank->sub->fieldDC);
            return 1;
        }
        if (data[1] == 0xCD52CF56) {
            data[3] = em->field4C & 0x20000;
            return 1;
        }
        return 0;

    case 0x41d:
        em = (Emitter*)data[0];
        if (data[1] == 0xCD52CF56) {
            if (data[3] != 0) {
                em->field4C |= 0x20000;
            } else {
                em->field4C &= ~0x20000;
            }
            return 1;
        }
        return 0;

    case 0xc00:
        em = (Emitter*)data[0];
        if (em != 0 && em->field98 != 0 && em->state < 2 && em->state >= 0) {
            if (em == 0) {
                return 0;
            }
            if (em->bank == 0) {
                return 0;
            }
            sub = em->bank->sub;
            if (sub == 0) {
                return 0;
            }
            if (data[1] != 0) {
                float f;

                em->flags |= 1;
                if (sub->field98 == 0) {
                    f = sub->field94;
                } else if (sub->field90 > lbl_804D7058) {
                    f = (float)sub->field7C / sub->field90;
                } else {
                    f = lbl_804D705C;
                }
                f *= em->fieldB4;
                em->bank->field14 =
                    (f > lbl_804D7058) ? lbl_804D7060 / f : lbl_804D7064;
                em->fieldBC = lbl_804D705C;
                {
                    Mtx34 mtx;

                    ActorGetMatrix(&mtx, (Actor*)em);
                    fn_800A0168(em->matrix, &mtx);
                }
            } else {
                em->flags &= ~1u;
                em->fieldA8 = lbl_804D705C;
            }
            return 1;
        }
        return 0;

    case 0xc15:
        em = (Emitter*)data[0];
        if (em->state < 2 && em->state >= 0) {
            sub = em->bank->sub;
            sub->byteCC = ((u8*)data)[0x0C];
            sub->byteCD = ((u8*)data)[0x0D];
            sub->byteCE = ((u8*)data)[0x0E];
            sub->byteCF = ((u8*)data)[0x0F];
            if (data[1] != -1) {
                em->flags |= 8;
            }
        }
        return 1;

    case 0x7203:
        ActorRefreshHostState((Actor*)data[0], data[1]);
        return 1;

    default:
        return 0;
    }
}

// Spawn and animate one emitter's particles: refresh the world matrix, scale
// the spawn rate by the distance to the camera, walk the spawn groups and hand
// every particle whose clock has expired to the active particle renderer.
extern "C" void fn_800D72CC(void* param)
{
    Emitter* em = (Emitter*)param;
    EmitterBank* bank;
    EmitterSub* sub;
    EmitterParticleState* states;
    EmitterSlotData* slots;
    EmitterGroup* group;
    Mtx34 mtx;
    Mtx34 basis;
    Mtx34 eulerMtx;
    Vec3 rel;
    Vec3 origin;
    Vec3 pos;
    Vec3 dir;
    Vec3 euler;
    int cam;
    int count;
    int groupCount;
    int perGroup;
    int state;
    int start;
    int end;
    int g;
    int i;
    int n;
    int k;
    int idx;
    u32 id;
    u32 flags;
    u32 handle;
    float dist;
    float denom;
    float scaled;
    float fade;
    float speed;
    float alpha;
    float alphaMax;
    float step;
    float life;
    float scale;
    float mag;
    float inv;
    u32 alphaByte;

    if (lbl_804D2BD4 == 0 || em == 0) {
        return;
    }
    bank = em->bank;
    if (bank == 0 || bank->field0C == 0) {
        return;
    }
    states = (EmitterParticleState*)bank->field04;
    sub = bank->sub;
    slots = (EmitterSlotData*)bank->field08;

    fn_80019A0C();
    ActorGetMatrix(&mtx, (Actor*)em);
    cam = fn_800C64AC();
    rel.x = mtx.m[3][0] - *(float*)(cam + 0x24);
    rel.y = mtx.m[3][1] - *(float*)(cam + 0x28);
    rel.z = mtx.m[3][2] - *(float*)(cam + 0x2C);
    cam = fn_800C64AC();
    fn_800A0030(&rel, (const Mtx34*)cam, &rel);

    dist = fn_800097B4();
    denom = dist;
    if (dist < rel.y) {
        denom = rel.y;
    }
    scaled = lbl_804D70B0 * (dist / denom);
    fade = lbl_804D705C;
    if (fade <= scaled) {
        fade = scaled;
        if (lbl_804D7068 < scaled) {
            fade = lbl_804D7068;
        }
    }

    if (sub->field88 & 0x40) {
        LinkInfoInit(&mtx);
    }

    count = bank->field10;
    groupCount = sub->field28;
    if (groupCount == 0) {
        groupCount = 1;
    }
    perGroup = count / groupCount;
    state = sub->state;

    if (state == 2) {
        if ((sub->field88 & 2) == 0 && (sub->field88 & 8) == 0) {
            fn_800A0168(&basis, &mtx);
            start = 0;
            origin.x = mtx.m[3][0];
            origin.y = mtx.m[3][1];
            origin.z = mtx.m[3][2];
            speed = lbl_804D7060;
            group = sub->groups;
            for (g = 0; g < groupCount; g++) {
                end = count;
                if (start + perGroup < count) {
                    end = start + perGroup;
                }
                if (sub->field28 == 0) {
                    id = 0xFFFFFFFDu;
                } else {
                    id = (u32)group->id;
                }
                flags = fn_80010000(id) & 0x18;
                n = fn_8000F904(id);
                step = (speed * sub->field90) /
                       (float)((double)n - lbl_804D7050);
                if (flags == 0) {
                    fn_8013AC68(id, 0);
                }
                for (i = start; i < end; i++) {
                    if (lbl_804D705C < states[i].clock) {
                        fn_800A0030(&pos, &basis, &states[i].data.pos);
                        pos.x += origin.x;
                        pos.y += origin.y;
                        pos.z += origin.z;
                        life = states[i].data.field24;
                        scale = lbl_804D70A8 *
                                (((float*)&slots[i])[1] +
                                 ((float*)&slots[i])[2]);
                        handle = id;
                        if (flags != 0) {
                            idx = (int)(states[i].clock / step);
                            k = (n - idx) - 1;
                            if (k < 0) {
                                k = 0;
                            } else if (n < k) {
                                k = n;
                            }
                            if ((handle & 0xFFF) != 0xFFF) {
                                handle &= 0xFFF;
                            }
                            handle = ((u32)k << 12) | handle;
                            fn_8013AC68(handle, 0);
                        }
                        fn_8013A834(scale, life, &pos, slots[i].byte0C,
                                    slots[i].byte0D, slots[i].byte0E,
                                    slots[i].byte0F);
                        if (flags != 0) {
                            fn_8013A3D4();
                        }
                    }
                }
                if (flags == 0) {
                    fn_8013A3D4();
                }
                start += perGroup;
                group++;
            }
        } else {
            alphaMax = lbl_804D70B4;
            alpha = lbl_804D70A0 * (fade * sub->field80);
            if (alpha < alphaMax) {
                alphaMax = alpha;
            }
            start = 0;
            speed = lbl_804D7060;
            group = sub->groups;
            for (g = 0; g < groupCount; g++) {
                end = count;
                if (start + perGroup < count) {
                    end = start + perGroup;
                }
                if (sub->field28 == 0) {
                    id = 0xFFFFFFFDu;
                } else {
                    id = (u32)group->id;
                }
                flags = fn_80010000(id) & 0x18;
                n = fn_8000F904(id);
                step = (speed * sub->field90) /
                       (float)((double)n - lbl_804D7050);
                if (flags == 0) {
                    fn_8013B6F8(alphaMax, id, &mtx);
                }
                for (i = start; i < end; i++) {
                    if (lbl_804D705C < states[i].clock) {
                        if ((sub->field88 & 8) == 0) {
                            if ((sub->field88 & 2) == 0) {
                                fn_800A025C(&dir, &mtx, 2);
                                dir.x = -dir.x;
                                dir.y = -dir.y;
                                dir.z = -dir.z;
                                if ((fabsf(dir.x) > lbl_804D7058) +
                                    (fabsf(dir.y) > lbl_804D7058) +
                                    (fabsf(dir.z) > lbl_804D7058) == 0) {
                                    dir.x = lbl_804D705C;
                                    dir.y = lbl_804D705C;
                                    dir.z = lbl_804D705C;
                                } else {
                                    inv = 1.0f / sqrtf(dir.x * dir.x +
                                                       dir.y * dir.y +
                                                       dir.z * dir.z);
                                    dir.x *= inv;
                                    dir.y *= inv;
                                    dir.z *= inv;
                                }
                                scale = -((float*)&slots[i])[2];
                                dir.x *= scale;
                                dir.y *= scale;
                                dir.z *= scale;
                            } else {
                                dir.x = states[i].data.dir.x;
                                dir.y = states[i].data.dir.y;
                                dir.z = states[i].data.dir.z;
                                if ((fabsf(dir.x) > lbl_804D7058) +
                                    (fabsf(dir.y) > lbl_804D7058) +
                                    (fabsf(dir.z) > lbl_804D7058) == 0) {
                                    dir.x = lbl_804D705C;
                                    dir.y = lbl_804D705C;
                                    dir.z = lbl_804D705C;
                                } else {
                                    inv = 1.0f / sqrtf(dir.x * dir.x +
                                                       dir.y * dir.y +
                                                       dir.z * dir.z);
                                    dir.x *= inv;
                                    dir.y *= inv;
                                    dir.z *= inv;
                                }
                                scale = -((float*)&slots[i])[2];
                                dir.x *= scale;
                                dir.y *= scale;
                                dir.z *= scale;
                            }
                        } else {
                            dir.x = states[i].data.dir.x;
                            dir.y = states[i].data.dir.y;
                            dir.z = states[i].data.dir.z;
                            if ((fabsf(dir.x) > lbl_804D7058) +
                                (fabsf(dir.y) > lbl_804D7058) +
                                (fabsf(dir.z) > lbl_804D7058) == 0) {
                                dir.x = lbl_804D705C;
                                dir.y = lbl_804D705C;
                                dir.z = lbl_804D705C;
                                mag = lbl_804D705C;
                            } else {
                                inv = 1.0f / sqrtf(dir.x * dir.x +
                                                   dir.y * dir.y +
                                                   dir.z * dir.z);
                                dir.x *= inv;
                                dir.y *= inv;
                                dir.z *= inv;
                                mag = 1.0f / inv;
                            }
                            scale = -sub->field24 * (mag / lbl_804D70B8);
                            dir.x *= scale;
                            dir.y *= scale;
                            dir.z *= scale;
                        }
                        pos.x = states[i].data.pos.x + dir.x;
                        pos.y = states[i].data.pos.y + dir.y;
                        pos.z = states[i].data.pos.z + dir.z;
                        handle = id;
                        if (flags != 0) {
                            idx = (int)(states[i].clock / step);
                            k = (n - idx) - 1;
                            if (k < 0) {
                                k = 0;
                            } else if (n < k) {
                                k = n;
                            }
                            if ((handle & 0xFFF) != 0xFFF) {
                                handle &= 0xFFF;
                            }
                            handle = ((u32)k << 12) | handle;
                            fn_8013B6F8(alphaMax, handle, &mtx);
                        }
                        alphaByte = 0xFF;
                        if (slots[i].byte13 < 0x100) {
                            alphaByte = slots[i].byte13;
                        }
                        fn_8013B608(&states[i].data.pos, &pos,
                                    slots[i].byte0C, slots[i].byte0D,
                                    slots[i].byte0E, slots[i].byte0F,
                                    slots[i].byte10, slots[i].byte11,
                                    slots[i].byte12, alphaByte);
                        if (flags != 0) {
                            fn_8013B5D4();
                        }
                    }
                }
                if (flags == 0) {
                    fn_8013B5D4();
                }
                start += perGroup;
                group++;
            }
        }
    } else if (state < 2) {
        if (state == 0) {
            alphaMax = lbl_804D70B4;
            alpha = fade * sub->field84;
            if (alpha < alphaMax) {
                alphaMax = alpha;
            }
            start = 0;
            speed = lbl_804D7060;
            group = sub->groups;
            for (g = 0; g < groupCount; g++) {
                end = count;
                if (start + perGroup < count) {
                    end = start + perGroup;
                }
                if (sub->field28 == 0) {
                    id = 0xFFFFFFFDu;
                } else {
                    id = (u32)group->id;
                }
                flags = fn_80010000(id) & 0x18;
                n = fn_8000F904(id);
                step = (speed * sub->field90) /
                       (float)((double)n - lbl_804D7050);
                if (flags == 0) {
                    fn_8013B6F8(alphaMax, id, &mtx);
                }
                for (i = start; i < end; i++) {
                    if (lbl_804D705C < states[i].clock) {
                        if ((sub->field88 & 8) == 0) {
                            if ((sub->field88 & 2) == 0) {
                                fn_800A025C(&dir, &mtx, 2);
                                dir.x = -dir.x;
                                dir.y = -dir.y;
                                dir.z = -dir.z;
                                if ((fabsf(dir.x) > lbl_804D7058) +
                                    (fabsf(dir.y) > lbl_804D7058) +
                                    (fabsf(dir.z) > lbl_804D7058) == 0) {
                                    dir.x = lbl_804D705C;
                                    dir.y = lbl_804D705C;
                                    dir.z = lbl_804D705C;
                                } else {
                                    inv = 1.0f / sqrtf(dir.x * dir.x +
                                                       dir.y * dir.y +
                                                       dir.z * dir.z);
                                    dir.x *= inv;
                                    dir.y *= inv;
                                    dir.z *= inv;
                                }
                                scale = -((float*)&slots[i])[2];
                                dir.x *= scale;
                                dir.y *= scale;
                                dir.z *= scale;
                            } else {
                                dir.x = states[i].data.dir.x;
                                dir.y = states[i].data.dir.y;
                                dir.z = states[i].data.dir.z;
                                if ((fabsf(dir.x) > lbl_804D7058) +
                                    (fabsf(dir.y) > lbl_804D7058) +
                                    (fabsf(dir.z) > lbl_804D7058) == 0) {
                                    dir.x = lbl_804D705C;
                                    dir.y = lbl_804D705C;
                                    dir.z = lbl_804D705C;
                                } else {
                                    inv = 1.0f / sqrtf(dir.x * dir.x +
                                                       dir.y * dir.y +
                                                       dir.z * dir.z);
                                    dir.x *= inv;
                                    dir.y *= inv;
                                    dir.z *= inv;
                                }
                                scale = -((float*)&slots[i])[2];
                                dir.x *= scale;
                                dir.y *= scale;
                                dir.z *= scale;
                            }
                        } else {
                            dir.x = states[i].data.dir.x;
                            dir.y = states[i].data.dir.y;
                            dir.z = states[i].data.dir.z;
                            if ((fabsf(dir.x) > lbl_804D7058) +
                                (fabsf(dir.y) > lbl_804D7058) +
                                (fabsf(dir.z) > lbl_804D7058) == 0) {
                                dir.x = lbl_804D705C;
                                dir.y = lbl_804D705C;
                                dir.z = lbl_804D705C;
                                mag = lbl_804D705C;
                            } else {
                                inv = 1.0f / sqrtf(dir.x * dir.x +
                                                   dir.y * dir.y +
                                                   dir.z * dir.z);
                                dir.x *= inv;
                                dir.y *= inv;
                                dir.z *= inv;
                                mag = 1.0f / inv;
                            }
                            scale = -sub->field24 * (mag / lbl_804D70B8);
                            dir.x *= scale;
                            dir.y *= scale;
                            dir.z *= scale;
                        }
                        pos.x = states[i].data.pos.x + dir.x;
                        pos.y = states[i].data.pos.y + dir.y;
                        pos.z = states[i].data.pos.z + dir.z;
                        handle = id;
                        if (flags != 0) {
                            idx = (int)(states[i].clock / step);
                            k = (n - idx) - 1;
                            if (k < 0) {
                                k = 0;
                            } else if (n < k) {
                                k = n;
                            }
                            if ((handle & 0xFFF) != 0xFFF) {
                                handle &= 0xFFF;
                            }
                            handle = ((u32)k << 12) | handle;
                            fn_8013B6F8(alphaMax, handle, &mtx);
                        }
                        alphaByte = 0xFF;
                        if (slots[i].byte13 < 0x100) {
                            alphaByte = slots[i].byte13;
                        }
                        fn_8013B608(&states[i].data.pos, &pos,
                                    slots[i].byte0C, slots[i].byte0D,
                                    slots[i].byte0E, slots[i].byte0F,
                                    slots[i].byte10, slots[i].byte11,
                                    slots[i].byte12, alphaByte);
                        if (flags != 0) {
                            fn_8013B5D4();
                        }
                    }
                }
                if (flags == 0) {
                    fn_8013B5D4();
                }
                start += perGroup;
                group++;
            }
        } else if (state >= 0) {
            alphaMax = lbl_804D70B4;
            alpha = fade * sub->field80;
            if (alpha < alphaMax) {
                alphaMax = alpha;
            }
            start = 0;
            speed = lbl_804D7060;
            group = sub->groups;
            for (g = 0; g < groupCount; g++) {
                end = count;
                if (start + perGroup < count) {
                    end = start + perGroup;
                }
                if (sub->field28 == 0) {
                    id = 0xFFFFFFFDu;
                } else {
                    id = (u32)group->id;
                }
                flags = fn_80010000(id) & 0x18;
                n = fn_8000F904(id);
                step = (speed * sub->field90) /
                       (float)((double)n - lbl_804D7050);
                if (flags == 0) {
                    fn_8013B918(alphaMax, id, &mtx);
                }
                for (i = start; i < end; i++) {
                    if (lbl_804D705C < states[i].clock) {
                        if ((sub->field88 & 8) == 0) {
                            if ((sub->field88 & 2) == 0) {
                                fn_800A025C(&dir, &mtx, 2);
                                dir.x = -dir.x;
                                dir.y = -dir.y;
                                dir.z = -dir.z;
                                if ((fabsf(dir.x) > lbl_804D7058) +
                                    (fabsf(dir.y) > lbl_804D7058) +
                                    (fabsf(dir.z) > lbl_804D7058) == 0) {
                                    dir.x = lbl_804D705C;
                                    dir.y = lbl_804D705C;
                                    dir.z = lbl_804D705C;
                                } else {
                                    inv = 1.0f / sqrtf(dir.x * dir.x +
                                                       dir.y * dir.y +
                                                       dir.z * dir.z);
                                    dir.x *= inv;
                                    dir.y *= inv;
                                    dir.z *= inv;
                                }
                                scale = -((float*)&slots[i])[2];
                                dir.x *= scale;
                                dir.y *= scale;
                                dir.z *= scale;
                            } else {
                                dir.x = states[i].data.dir.x;
                                dir.y = states[i].data.dir.y;
                                dir.z = states[i].data.dir.z;
                                if ((fabsf(dir.x) > lbl_804D7058) +
                                    (fabsf(dir.y) > lbl_804D7058) +
                                    (fabsf(dir.z) > lbl_804D7058) == 0) {
                                    dir.x = lbl_804D705C;
                                    dir.y = lbl_804D705C;
                                    dir.z = lbl_804D705C;
                                } else {
                                    inv = 1.0f / sqrtf(dir.x * dir.x +
                                                       dir.y * dir.y +
                                                       dir.z * dir.z);
                                    dir.x *= inv;
                                    dir.y *= inv;
                                    dir.z *= inv;
                                }
                                scale = -((float*)&slots[i])[2];
                                dir.x *= scale;
                                dir.y *= scale;
                                dir.z *= scale;
                            }
                        } else {
                            dir.x = states[i].data.dir.x;
                            dir.y = states[i].data.dir.y;
                            dir.z = states[i].data.dir.z;
                            if ((fabsf(dir.x) > lbl_804D7058) +
                                (fabsf(dir.y) > lbl_804D7058) +
                                (fabsf(dir.z) > lbl_804D7058) == 0) {
                                dir.x = lbl_804D705C;
                                dir.y = lbl_804D705C;
                                dir.z = lbl_804D705C;
                                mag = lbl_804D705C;
                            } else {
                                inv = 1.0f / sqrtf(dir.x * dir.x +
                                                   dir.y * dir.y +
                                                   dir.z * dir.z);
                                dir.x *= inv;
                                dir.y *= inv;
                                dir.z *= inv;
                                mag = 1.0f / inv;
                            }
                            scale = -sub->field24 * (mag / lbl_804D70B8);
                            dir.x *= scale;
                            dir.y *= scale;
                            dir.z *= scale;
                        }
                        pos.x = states[i].data.pos.x + dir.x;
                        pos.y = states[i].data.pos.y + dir.y;
                        pos.z = states[i].data.pos.z + dir.z;
                        handle = id;
                        if (flags != 0) {
                            idx = (int)(states[i].clock / step);
                            k = (n - idx) - 1;
                            if (k < 0) {
                                k = 0;
                            } else if (n < k) {
                                k = n;
                            }
                            if ((handle & 0xFFF) != 0xFFF) {
                                handle &= 0xFFF;
                            }
                            handle = ((u32)k << 12) | handle;
                            fn_8013B918(alphaMax, handle, &mtx);
                        }
                        alphaByte = 0xFF;
                        if (slots[i].byte13 < 0x100) {
                            alphaByte = slots[i].byte13;
                        }
                        fn_8013B884(&states[i].data.pos, slots[i].byte0C,
                                    slots[i].byte0D, slots[i].byte0E,
                                    slots[i].byte0F);
                        if (flags != 0) {
                            fn_8013B850();
                        }
                    }
                }
                if (flags == 0) {
                    fn_8013B850();
                }
                start += perGroup;
                group++;
            }
        }
    } else if (state < 4) {
        start = 0;
        speed = lbl_804D7060;
        group = sub->groups;
        for (g = 0; g < groupCount; g++) {
            end = count;
            if (start + perGroup < count) {
                end = start + perGroup;
            }
            if (sub->field28 == 0) {
                id = 0xFFFFFFFDu;
            } else {
                id = (u32)group->id;
            }
            flags = fn_80010000(id);
            n = fn_8000F904(id);
            step = (speed * sub->field90) /
                   (float)((double)n - lbl_804D7050);
            for (i = start; i < end; i++) {
                if (lbl_804D705C < states[i].clock) {
                    handle = id;
                    if ((flags & 0x18) != 0) {
                        idx = (int)(states[i].clock / step);
                        k = (n - idx) - 1;
                        if (k < 0) {
                            k = 0;
                        } else if (n < k) {
                            k = n;
                        }
                        if ((handle & 0xFFF) != 0xFFF) {
                            handle &= 0xFFF;
                        }
                        handle = ((u32)k << 12) | handle;
                    }
                    if ((sub->field88 & 0x10) == 0) {
                        fn_8013A30C(handle, &mtx);
                    } else {
                        Vec3Set(&euler, states[i].data.field24,
                                lbl_804D705C, lbl_804D705C);
                        MtxSetEuler(&eulerMtx, &euler);
                        fn_8009FC6C(&eulerMtx, &eulerMtx, &mtx);
                        eulerMtx.m[3][0] = mtx.m[3][0];
                        eulerMtx.m[3][1] = mtx.m[3][1];
                        eulerMtx.m[3][2] = mtx.m[3][2];
                        fn_8013A30C(handle, &eulerMtx);
                    }
                    GXBegin(0x98, 2, 4);
                    *(volatile float*)0xCC008000 = lbl_804D705C;
                    fn_8013A2E8();
                }
            }
            start += perGroup;
            group++;
        }
    }
}

// Parse the emitter sub-descriptor pool out of a loaded asset chunk: validate
// the header, carve the descriptor and spawn-list arrays out of the raw buffer
// and point every descriptor at its own slice of the two lists.
extern "C" void fn_800D8664(u32 param_1, int* param_2, int param_3)
{
    EmitterSubList* hdr;
    EmitterSub* sub;
    EmitterParticle* cursorA;
    EmitterParticle* cursorB;
    int baseSize;
    int remaining;
    int i;
    int j;

    if (param_3 > 0) {
        lbl_804D4950 = (EmitterSubList*)param_2;
    } else {
        lbl_804D4950 = 0;
    }
    param_2 += 8;
    baseSize = param_3 - 0x20;
    FileFlushBuffer(lbl_804D4950, 0x20);
    if (lbl_804D4950->magic != 0xD) {
        lbl_804D4950 = 0;
        return;
    }
    hdr = lbl_804D4950;
    if (baseSize > 0 && hdr->count * 0xEC > 0) {
        hdr->subs = (EmitterSub*)param_2;
    } else {
        hdr->subs = 0;
    }
    remaining = baseSize - hdr->count * 0xEC;
    if (remaining > 0 && hdr->countA * 0x2C > 0) {
        hdr->listA = (EmitterParticle*)(param_2 + hdr->count * 0x3B);
    } else {
        hdr->listA = 0;
    }
    FileFlushBuffer(hdr->listA, hdr->countA * 0x2C);
    if (remaining - hdr->countA * 0x2C > 0 && hdr->countB * 0x2C > 0) {
        hdr->listB = (EmitterParticle*)(param_2 + hdr->count * 0x3B +
                                        hdr->countA * 0xB);
    } else {
        hdr->listB = 0;
    }
    FileFlushBuffer(hdr->listB, hdr->countB * 0x2C);

    cursorA = hdr->listA;
    cursorB = hdr->listB;
    for (i = 0; i < hdr->count; i++) {
        sub = hdr->subs + i;
        FileFlushBuffer(sub, 0xEC);
        for (j = 0; j < 8; j++) {
            fn_800070C0(&sub->groups[j].byte04);
        }
        fn_800070C0(&sub->byteCC);
        fn_800070C0(&sub->byteD0);
        sub->listA0 = cursorA;
        sub->listA8 = cursorB;
        cursorA += sub->count9C;
        cursorB += sub->countA4;
    }
}

// Builds a live emitter from one emitter sub-descriptor: claims a bank slot
// and an instance slot from the two global pools, seeds the runtime state from
// the descriptor, places and registers the actor and looks up its assets.
// Returns 0 when either pool is full (the bank slot is then released).
Emitter* EmitterBuildFromSub(EmitterDesc* sub)
{
    EmitterBank* bank = 0;
    Emitter* em = 0;
    float fade;
    Mtx34 matrix;
    Mtx34 placement;
    int half;
    int first;
    int second;
    int i;

    if (lbl_804D4954 != 0) {
        for (i = 0; i < lbl_804D4958; i++) {
            if (lbl_804D4954[i].field0C == 0) {
                memset(&lbl_804D4954[i], 0, 0x20);
                lbl_804D4954[i].field0C = 1;
                bank = &lbl_804D4954[i];
                break;
            }
        }
    }
    if (bank == 0) {
        return 0;
    }
    if (lbl_804D4960 != 0) {
        for (i = 0; i < lbl_804D4964; i++) {
            if (lbl_804D4960[i].field98 == 0) {
                memset(&lbl_804D4960[i], 0, 0xC0);
                lbl_804D4960[i].field98 = 1;
                em = &lbl_804D4960[i];
                break;
            }
        }
    }
    if (em == 0) {
        int index = bank - lbl_804D4954;

        if (index >= 0 && index < lbl_804D4958) {
            lbl_804D4954[index].field0C = 0;
        }
        return 0;
    }

    bank->sub = sub;
    em->bank = bank;
    em->fieldB0 = sub->field78;
    em->fieldB4 = lbl_804D7068;
    if (sub->field98 == 0) {
        fade = sub->field94;
    } else {
        fade = lbl_804D705C;
        if (sub->field90 > lbl_804D7058) {
            fade = (float)sub->field7C / sub->field90;
        }
    }
    fade *= em->fieldB4;
    bank->field14 =
        (fade > lbl_804D7058) ? lbl_804D7060 / fade : lbl_804D7064;
    em->fieldA8 = lbl_804D705C;

    BoneGetMatrix(&matrix, sub->rot);
    matrix.m[3][0] = sub->posX;
    matrix.m[3][1] = sub->posY;
    matrix.m[3][2] = sub->posZ;
    PlacementToMatrix(&placement, (PlaceF*)&matrix);
    fn_800A0168(em->matrix, &matrix);

    half = (int)(em->bank->sub->field90 *
                 (em->bank->sub->field78 + em->bank->sub->field74));
    em->bounds.minX = -half;
    em->bounds.minY = -half;
    em->bounds.minZ = -half;
    em->bounds.maxX = half;
    em->bounds.maxY = half;
    em->bounds.maxZ = half;
    em->handle = ((u32)(em - lbl_804D4960) & 0xFFFFFF) | 0x14000000;

    ActorFn8002378C((Actor*)em, em->handle, (int)&placement,
                    (int)EmitterResolveHandle, (int)&em->bounds,
                    (int)&em->bounds, 0);
    ActorFn800231A8((Actor*)em);
    ActorFn80022D98((Actor*)em, 1);
    em->flags |= 1;
    first = AssetFind(0xFFFFFFFF, em->bank->sub->fieldE0);
    second = AssetFind(0xF1754BF2, em->bank->sub->fieldE4);
    fn_80212814(em->field38, first, second, 1, 0, 0);
    if (SelectionSetTest(bank->sub->fieldE8) == 0 &&
        (int)(em->field38 & 0xFFFFFF) < lbl_804D4950->count) {
        ActorRefreshHostState((Actor*)em, 0);
        ObjSendC00(em, 0);
    }
    return em;
}

// Stores one emitter channel weight in the global channel table.
void EmitterSetChannelWeight(int index, float value)
{
    lbl_8039BD20[index] = value;
}

// Stores the global emitter mode.
extern "C" int lbl_804D499C;

void EmitterSetMode(int mode)
{
    lbl_804D499C = mode;
}

// Feature query: this build leaves the feature disabled.
int EmitterFeatureEnabled(void)
{
    return 0;
}

// Per-emitter curve parameters (0x8C bytes); lbl_80452560 holds four of them.
struct EmitterChannel {
    int active;             // 0x00
    u8 pad04[0x24];
    int field28;            // 0x28
    int field2C;            // 0x2C
    u8 pad30[0x2C];
    int field5C;            // 0x5C
    int field60;            // 0x60
    float field64;          // 0x64
    float field68;          // 0x68
    float field6C;          // 0x6C
    float field70;          // 0x70
    u8 pad74[8];
    float field7C;          // 0x7C
    float field80;          // 0x80
    u8 pad84[8];
};

// One 0x60-byte curve table handed to fn_800DD714.
struct EmitterCurveTable {
    float value[24];
};

extern "C" EmitterChannel lbl_80452560[4];
extern "C" EmitterCurveTable lbl_8039BA20[];
extern "C" float lbl_80452500[];
extern "C" u32 lbl_804D2BD8;
extern "C" float lbl_804D70C0;
extern "C" float lbl_804D70C4;
extern "C" float lbl_804D70C8;
extern "C" float lbl_804D70CC;
extern "C" float lbl_804D70D0;
extern "C" float lbl_804D70D4;
extern "C" float lbl_804D70D8;

float ShapeRampA(float x);
float ShapeRampB(float x);
float ShapeRampC(float x);
float SampleEmitterCurve(float a, float b, EmitterCurveTable* table, int idx,
                         int arg);
extern "C" float fn_800D9E44(float x);

// A zero stays zero; anything else passes through.
static inline float ZeroOr(float v)
{
    return (v == lbl_804D70C4) ? lbl_804D70C4 : v;
}

// sin(k*x) squared, keeping the sign of x.
static inline float ShapeSineSquare(float x)
{
    float v;

    if (x > lbl_804D70C4) {
        v = fn_800A1394(lbl_804D70D0 * x);
        v = v * v;
    } else {
        v = fn_800A1394(lbl_804D70D0 * -x);
        v = -v * v;
    }
    return v;
}

// sin(k*x) cubed, keeping the sign of x.
static inline float ShapeSineCube(float x)
{
    float v;

    if (x > lbl_804D70C4) {
        v = fn_800A1394(lbl_804D70D0 * x);
        v = v * (v * v);
    } else {
        v = fn_800A1394(lbl_804D70D0 * -x);
        v = v * (-v * v);
    }
    return v;
}

// Rescale a shaped value around zero, keeping its sign.
static inline float ShapeGain(float v)
{
    float sign = (v >= lbl_804D70C4) ? lbl_804D70CC : lbl_804D70C8;
    float t = (fabsf(v) - lbl_804D70D4) / lbl_804D70D8;
    float lo = lbl_804D70C4;

    if (lbl_804D70C4 <= t) {
        lo = t;
    }
    return sign * lo;
}

// Reads the curve value of emitter channel `idx` for parameter `mode` and
// value `type`.  Modes 2, 3 and 4 blend two of the channel's fields through
// one of the shared curve tables; the other modes shape a single field.
extern "C" float fn_800D8BE4(int idx, int mode, int type, int arg)
{
    EmitterChannel* ch;
    int active;

    if (idx < 4) {
        active = *(int*)((u8*)lbl_80452560 + idx * 0x8C);
    } else {
        active = 0;
    }
    if (active == 0) {
        return lbl_804D70C0;
    }
    if ((lbl_804D2BD8 & (1 << idx)) == 0) {
        return lbl_804D70C4;
    }
    ch = &lbl_80452560[idx];

    switch (mode) {
    case 1:
        switch (type) {
        case 0:
            return ShapeGain(ShapeSineSquare(ZeroOr(ch->field68)));
        case 1:
            return ShapeGain(ShapeSineSquare(ZeroOr(ch->field6C)));
        case 2:
            return ShapeGain(ShapeSineSquare(ZeroOr(ch->field70)));
        case 3:
            return ShapeGain(ShapeSineCube(ZeroOr(ch->field64)));
        case 4:
            return ShapeSineSquare(ch->field80);
        case 5:
            return ShapeSineSquare(ch->field7C);
        }
        break;
    case 2:
    case 4:
        switch (type) {
        case 0:
            return SampleEmitterCurve(ch->field64, ch->field68, &lbl_8039BA20[1],
                               idx, arg);
        case 1:
            return SampleEmitterCurve(ch->field6C, ch->field70, &lbl_8039BA20[2],
                               idx, arg);
        case 2:
            return SampleEmitterCurve(ch->field68, ch->field64, &lbl_8039BA20[0],
                               idx, arg);
        case 3:
            return SampleEmitterCurve(ch->field70, ch->field6C, &lbl_8039BA20[3],
                               idx, arg);
        case 4:
            return ShapeRampB(lbl_804D70C4);
        }
        break;
    case 3:
        switch (type) {
        case 0:
            return SampleEmitterCurve(ch->field6C, ch->field70, &lbl_8039BA20[5],
                               idx, arg);
        case 1:
            return SampleEmitterCurve(ch->field64, ch->field68, &lbl_8039BA20[6],
                               idx, arg);
        case 2:
            return SampleEmitterCurve(ch->field68, ch->field64, &lbl_8039BA20[4],
                               idx, arg);
        case 3:
            return SampleEmitterCurve(ch->field70, ch->field6C, &lbl_8039BA20[7],
                               idx, arg);
        case 4:
            return ShapeRampB(lbl_804D70C4);
        }
        break;
    case 5:
        switch (type) {
        case 0:
            return ShapeRampC(ZeroOr(ch->field6C));
        case 1:
            return ShapeRampB(ZeroOr(ch->field64));
        case 2:
            return ShapeRampA(ZeroOr(ch->field68));
        case 3:
            return fn_800D9E44(ZeroOr(ch->field70));
        case 4:
            return ShapeRampB(lbl_804D70C4);
        }
        break;
    case 6:
        switch (type) {
        case 0:
            return ShapeRampC(ZeroOr(ch->field6C));
        case 1:
            return ShapeRampB(ZeroOr(ch->field64));
        case 2:
            return ShapeRampA(ZeroOr(ch->field68));
        case 3:
            return fn_800D9E44(ZeroOr(ch->field70));
        case 4:
            return ShapeRampB(lbl_804D70C4);
        }
        break;
    case 7:
        switch (type) {
        case 0:
            return fn_800D9E44(lbl_804D70C4);
        case 1:
            return ShapeRampB(ZeroOr(ch->field64));
        case 2:
            return ShapeRampA(ZeroOr(ch->field68));
        case 4:
            return ShapeRampB(lbl_804D70C4);
        }
        break;
    case 8:
        switch (type) {
        case 0:
            return ShapeRampC(lbl_804D70C4);
        case 1:
            return ShapeRampB(ZeroOr(ch->field64));
        case 2:
            return ShapeRampA(ZeroOr(ch->field68));
        case 3:
            return fn_800D9E44(lbl_804D70C4);
        case 4:
            return ShapeRampB(lbl_804D70C4);
        }
        break;
    case 9:
        switch (type) {
        case 0:
            if (ch->field28 != 0) {
                return ShapeRampC(lbl_804D70C8);
            }
            if (ch->field2C != 0) {
                return ShapeRampC(lbl_804D70CC);
            }
            return lbl_804D70C4;
        case 1:
            return ShapeRampB((ch->field64 == lbl_804D70C4) ? ch->field6C
                                                             : ch->field64);
        case 2:
            return ShapeRampA(ZeroOr(ch->field68));
        case 3:
            return fn_800D9E44(ZeroOr(ch->field70));
        case 4:
            return ShapeRampB(lbl_804D70C4);
        }
        break;
    case 10:
        switch (type) {
        case 0:
            if (ch->field60 != 0) {
                return ShapeRampC(lbl_804D70C8);
            }
            if (ch->field5C != 0) {
                return ShapeRampC(lbl_804D70CC);
            }
            return ShapeRampC(ZeroOr(ch->field6C));
        case 1:
            return ShapeRampB(ZeroOr(ch->field64));
        case 2:
            return ShapeRampA(ZeroOr(ch->field68));
        case 3:
            return fn_800D9E44(ZeroOr(ch->field70));
        case 4:
            return ShapeRampB(lbl_804D70C4);
        }
        break;
    case 11:
        switch (type) {
        case 0:
            return ShapeRampC(lbl_804D70C4);
        case 1:
            return ShapeRampB(ZeroOr(ch->field64));
        case 2:
            return ShapeRampA(ZeroOr(ch->field68));
        case 3:
            return fn_800D9E44(ZeroOr(ch->field70));
        case 4:
            return ShapeRampB(ch->field6C);
        }
        break;
    case 12:
        switch (type) {
        case 0:
            return ShapeGain(ShapeSineCube(ZeroOr(ch->field68)));
        case 1:
            return ShapeGain(ShapeSineCube(ZeroOr(ch->field64)));
        case 2:
            return ShapeSineCube(ch->field80);
        case 3:
            return ShapeSineCube(ch->field7C);
        case 4:
            return ShapeGain(ShapeSineCube(ZeroOr(ch->field70)));
        case 5:
            return ShapeGain(ShapeSineCube(ZeroOr(ch->field6C)));
        }
        break;
    case 0:
    default:
        break;
    }

    switch (type) {
    case 0:
        return ShapeGain(ShapeSineSquare(ZeroOr(ch->field6C)));
    case 1:
        return ShapeGain(ShapeSineSquare(ZeroOr(ch->field64)));
    case 2:
        return ShapeGain(ShapeSineSquare(ZeroOr(ch->field68)));
    case 3:
        return ShapeGain(ShapeSineSquare(ZeroOr(ch->field70)));
    }
    return lbl_804D70C0;
}

// Piecewise-linear curve through the breakpoints of lbl_80452500: it stays 0
// up to the first breakpoint (0x48), ramps through 0x4C/0x50 and 0x54/0x58 and
// saturates at one from 0x5C on.  The curve is odd-symmetric around zero.
extern "C" float fn_800D9E44(float x)
{
    float a = (x > lbl_804D70C4) ? x : -x;
    float r;

    if (a <= lbl_80452500[18]) {
        r = lbl_804D70C4;
    } else if (a <= lbl_80452500[19]) {
        r = lbl_80452500[20] * ((a - lbl_80452500[18]) /
                                (lbl_80452500[19] - lbl_80452500[18]));
    } else if (a <= lbl_80452500[21]) {
        r = (lbl_80452500[22] - lbl_80452500[20]) *
                ((a - lbl_80452500[19]) /
                 (lbl_80452500[21] - lbl_80452500[19])) +
            lbl_80452500[20];
    } else if (a <= lbl_80452500[23]) {
        r = (lbl_804D70CC - lbl_80452500[22]) *
                ((a - lbl_80452500[21]) /
                 (lbl_80452500[23] - lbl_80452500[21])) +
            lbl_80452500[22];
    } else {
        r = lbl_804D70CC;
    }
    return (x > lbl_804D70C4) ? r : -r;
}

// Piecewise-linear curve through the first breakpoints of lbl_80452500 (0x00
// to 0x14): flat at zero until 0x00, ramps through 0x04/0x08 and 0x0C/0x10 and
// saturates at one past 0x14.  The curve is odd-symmetric around zero.
float ShapeRampA(float x)
{
    float a = (x > lbl_804D70C4) ? x : -x;
    float r;

    if (a <= lbl_80452500[0]) {
        r = lbl_804D70C4;
    } else if (a <= lbl_80452500[1]) {
        r = lbl_80452500[2] * ((a - lbl_80452500[0]) /
                               (lbl_80452500[1] - lbl_80452500[0]));
    } else if (a <= lbl_80452500[3]) {
        r = (lbl_80452500[4] - lbl_80452500[2]) *
                ((a - lbl_80452500[1]) /
                 (lbl_80452500[3] - lbl_80452500[1])) +
            lbl_80452500[2];
    } else if (a <= lbl_80452500[5]) {
        r = (lbl_804D70CC - lbl_80452500[4]) *
                ((a - lbl_80452500[3]) /
                 (lbl_80452500[5] - lbl_80452500[3])) +
            lbl_80452500[4];
    } else {
        r = lbl_804D70CC;
    }
    return (x > lbl_804D70C4) ? r : -r;
}

// Piecewise-linear curve through the second group of breakpoints of
// lbl_80452500 (0x18 to 0x2C): flat at zero until 0x18, ramps through 0x1C/0x20
// and 0x24/0x28 and saturates at one past 0x2C.  Odd-symmetric around zero.
float ShapeRampB(float x)
{
    float a = (x > lbl_804D70C4) ? x : -x;
    float r;

    if (a <= lbl_80452500[6]) {
        r = lbl_804D70C4;
    } else if (a <= lbl_80452500[7]) {
        r = lbl_80452500[8] * ((a - lbl_80452500[6]) /
                               (lbl_80452500[7] - lbl_80452500[6]));
    } else if (a <= lbl_80452500[9]) {
        r = (lbl_80452500[10] - lbl_80452500[8]) *
                ((a - lbl_80452500[7]) /
                 (lbl_80452500[9] - lbl_80452500[7])) +
            lbl_80452500[8];
    } else if (a <= lbl_80452500[11]) {
        r = (lbl_804D70CC - lbl_80452500[10]) *
                ((a - lbl_80452500[9]) /
                 (lbl_80452500[11] - lbl_80452500[9])) +
            lbl_80452500[10];
    } else {
        r = lbl_804D70CC;
    }
    return (x > lbl_804D70C4) ? r : -r;
}

// Piecewise-linear curve through the third group of breakpoints of
// lbl_80452500 (0x30 to 0x44): flat at zero until 0x30, ramps through 0x34/0x38
// and 0x3C/0x40 and saturates at one past 0x44.  Odd-symmetric around zero.
float ShapeRampC(float x)
{
    float a = (x > lbl_804D70C4) ? x : -x;
    float r;

    if (a <= lbl_80452500[12]) {
        r = lbl_804D70C4;
    } else if (a <= lbl_80452500[13]) {
        r = lbl_80452500[14] * ((a - lbl_80452500[12]) /
                                (lbl_80452500[13] - lbl_80452500[12]));
    } else if (a <= lbl_80452500[15]) {
        r = (lbl_80452500[16] - lbl_80452500[14]) *
                ((a - lbl_80452500[13]) /
                 (lbl_80452500[15] - lbl_80452500[13])) +
            lbl_80452500[14];
    } else if (a <= lbl_80452500[17]) {
        r = (lbl_804D70CC - lbl_80452500[16]) *
                ((a - lbl_80452500[15]) /
                 (lbl_80452500[17] - lbl_80452500[15])) +
            lbl_80452500[16];
    } else {
        r = lbl_804D70CC;
    }
    return (x > lbl_804D70C4) ? r : -r;
}

// Reads one button of a channel's 0x60-byte button table.  The record of the
// channel starts at lbl_80452560 + channel * 0x8C; its first word is the
// channel state and the button masks follow it.
int PadButtonState(int player, int button);

// Prefers the primary button and falls back to a second mask when the pad has
// no such primary button.
static inline int PadButtonOr(int player, int primary, int fallback)
{
    if (PadButtonState(player, primary)) {
        return PadButtonState(player, primary);
    }
    return PadButtonState(player, fallback);
}

// Maps a logical action id onto the button mask of one pad channel.  The
// mapping depends on the pad mode; unknown actions yield -99.  lbl_804D2BD8
// marks the channels that are live this frame.
int PadButtonForAction(int player, int mode, int action)
{
    int state;
    int v;

    if (player < 4) {
        state = *(int*)((u8*)lbl_80452560 + player * 0x8C);
    } else {
        state = 0;
    }
    if (state == 0) {
        v = -99;
    } else {
        if ((lbl_804D2BD8 & (1 << player)) == 0) {
            return 0;
        }
        switch (mode) {
        case 0:
        default:
        generic:
            switch (action) {
            case 0:  v = PadButtonOr(player, 3, 0x11); break;
            case 1:  v = PadButtonOr(player, 2, 0x10); break;
            case 2:  v = PadButtonOr(player, 0xC, 0x18); break;
            case 3:  v = PadButtonOr(player, 8, 0x18); break;
            case 4:  v = PadButtonOr(player, 9, 0x18); break;
            case 5:  v = PadButtonOr(player, 0, 0xE); break;
            case 6:  v = PadButtonOr(player, 1, 0xF); break;
            case 7:  v = PadButtonState(player, 2); break;
            case 8:  v = PadButtonOr(player, 0xB, 0x18); break;
            case 9:  v = PadButtonOr(player, 10, 0x18); break;
            case 10: v = PadButtonOr(player, 6, 0x18); break;
            case 11: v = PadButtonOr(player, 5, 0x18); break;
            case 12: v = PadButtonState(player, 3); break;
            case 13: v = PadButtonState(player, 2); break;
            case 14: v = PadButtonState(player, 0); break;
            case 15: v = PadButtonState(player, 1); break;
            case 16: v = PadButtonState(player, 4); break;
            case 17: v = PadButtonOr(player, 4, 0x18); break;
            case 18: v = PadButtonState(player, 10); break;
            case 19: v = PadButtonState(player, 0xB); break;
            default: v = -99; break;
            }
            break;
        case 1:
            switch (action) {
            case 0:  v = PadButtonState(player, 3); break;
            case 1:  v = PadButtonState(player, 2); break;
            case 2:  v = PadButtonState(player, 0); break;
            case 3:  v = PadButtonState(player, 1); break;
            case 4:  v = PadButtonOr(player, 0xB, 0x18); break;
            case 5:  v = PadButtonState(player, 0x17); break;
            case 6:  v = PadButtonState(player, 0x16); break;
            case 7:  v = PadButtonState(player, 8); break;
            case 8:  v = PadButtonState(player, 9); break;
            case 9:  v = PadButtonState(player, 10); break;
            case 10: v = PadButtonState(player, 2); break;
            case 11: v = PadButtonState(player, 2); break;
            case 12: v = PadButtonState(player, 0x18); break;
            case 13: v = PadButtonState(player, 0x18); break;
            case 14: v = PadButtonState(player, 4); break;
            default: goto generic;
            }
            break;
        case 2:
            switch (action) {
            case 0:  v = PadButtonOr(player, 0x11, 0x18); break;
            case 1:  v = PadButtonOr(player, 0x10, 0x18); break;
            case 2:  v = PadButtonOr(player, 0x12, 0x18); break;
            case 3:  v = PadButtonOr(player, 0x13, 0x18); break;
            case 4:  v = PadButtonOr(player, 0xE, 0x18); break;
            case 5:  v = PadButtonOr(player, 0xF, 0x18); break;
            case 6:  v = PadButtonOr(player, 0x15, 0x18); break;
            case 7:  v = PadButtonOr(player, 0x14, 0x18); break;
            case 8:  v = PadButtonOr(player, 0x16, 0x18); break;
            case 9:  v = PadButtonOr(player, 4, 0x18); break;
            case 10: v = PadButtonOr(player, 8, 0x18); break;
            case 11: v = PadButtonOr(player, 0xB, 0x18); break;
            case 12: v = PadButtonOr(player, 0x18, 0x18); break;
            case 13: v = PadButtonOr(player, 0x17, 0x18); break;
            case 14: v = PadButtonOr(player, 8, 0x18); break;
            case 15: v = PadButtonOr(player, 0x18, 0x18); break;
            case 16: v = PadButtonOr(player, 0x18, 0x18); break;
            case 17: v = PadButtonOr(player, 0x18, 0x18); break;
            case 0x13: v = 0; break;
            case 0x16: v = PadButtonState(player, 6); break;
            case 0x17: v = PadButtonState(player, 9); break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            default: v = 0; break;
            }
            break;
        case 3:
            switch (action) {
            case 0:  v = PadButtonOr(player, 0x11, 0x18); break;
            case 1:  v = PadButtonOr(player, 0x10, 0x18); break;
            case 2:  v = PadButtonOr(player, 0xE, 0x18); break;
            case 3:  v = PadButtonOr(player, 0xF, 0x18); break;
            case 4:  v = PadButtonOr(player, 0x12, 0x18); break;
            case 5:  v = PadButtonOr(player, 0x13, 0x18); break;
            case 6:  v = PadButtonOr(player, 0x15, 0x18); break;
            case 7:  v = PadButtonOr(player, 0x14, 0x18); break;
            case 8:  v = PadButtonOr(player, 0x16, 0x18); break;
            case 9:  v = PadButtonOr(player, 4, 0x18); break;
            case 10: v = PadButtonOr(player, 8, 0x18); break;
            case 11: v = PadButtonOr(player, 0xB, 0x18); break;
            case 12: v = PadButtonOr(player, 0x18, 0x18); break;
            case 13: v = PadButtonOr(player, 0x17, 0x18); break;
            case 14: v = PadButtonOr(player, 9, 0x18); break;
            case 15: v = PadButtonOr(player, 0x18, 0x18); break;
            case 16: v = PadButtonOr(player, 0x18, 0x18); break;
            case 17: v = PadButtonOr(player, 0x18, 0x18); break;
            case 0x13: v = 0; break;
            case 0x16: v = PadButtonState(player, 6); break;
            case 0x17: v = PadButtonState(player, 9); break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            default: v = 0; break;
            }
            break;
        case 4:
            switch (action) {
            case 0:  v = PadButtonOr(player, 0x11, 0x18); break;
            case 1:  v = PadButtonOr(player, 0x10, 0x18); break;
            case 2:  v = PadButtonOr(player, 0x12, 0x18); break;
            case 3:  v = PadButtonOr(player, 0x13, 0x18); break;
            case 4:  v = PadButtonOr(player, 0xE, 0x18); break;
            case 5:  v = PadButtonOr(player, 0xF, 0x18); break;
            case 6:  v = PadButtonOr(player, 0x15, 0x18); break;
            case 7:  v = PadButtonOr(player, 0x14, 0x18); break;
            case 8:  v = PadButtonOr(player, 0x16, 0x18); break;
            case 9:  v = PadButtonOr(player, 4, 0x18); break;
            case 10: v = PadButtonOr(player, 8, 0x18); break;
            case 11: v = PadButtonOr(player, 0xB, 0x18); break;
            case 12: v = PadButtonOr(player, 10, 0x18); break;
            case 13: v = PadButtonOr(player, 0x17, 0x18); break;
            case 14: v = PadButtonOr(player, 9, 0x18); break;
            case 15: v = PadButtonOr(player, 0x18, 0x18); break;
            case 16: v = PadButtonOr(player, 0x18, 0x18); break;
            case 17: v = PadButtonOr(player, 0x18, 0x18); break;
            case 0x13: v = 0; break;
            case 0x17: v = PadButtonState(player, 9); break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            default: v = 0; break;
            }
            break;
        case 5:
            switch (action) {
            case 0:  v = PadButtonOr(player, 0x11, 0x18); break;
            case 1:  v = PadButtonOr(player, 0x10, 0x18); break;
            case 2:  v = PadButtonOr(player, 0xE, 0x18); break;
            case 3:  v = PadButtonOr(player, 0xF, 0x18); break;
            case 4:  v = PadButtonOr(player, 0x12, 0x18); break;
            case 5:  v = PadButtonOr(player, 0x13, 0x18); break;
            case 6:  v = PadButtonOr(player, 0x15, 0x18); break;
            case 7:  v = PadButtonOr(player, 0x14, 0x18); break;
            case 8:  v = PadButtonOr(player, 0x16, 0x18); break;
            case 9:  v = PadButtonOr(player, 4, 0x18); break;
            case 10: v = PadButtonOr(player, 8, 0x18); break;
            case 11: v = PadButtonOr(player, 0xB, 0x18); break;
            case 12: v = PadButtonOr(player, 10, 0x18); break;
            case 13: v = PadButtonOr(player, 0x17, 0x18); break;
            case 14: v = PadButtonOr(player, 9, 0x18); break;
            case 15: v = PadButtonOr(player, 0x18, 0x18); break;
            case 16: v = PadButtonOr(player, 0x18, 0x18); break;
            case 17: v = PadButtonOr(player, 0x18, 0x18); break;
            case 0x13: v = 0; break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            default: v = 0; break;
            }
            break;
        case 6:
            switch (action) {
            case 0:  v = PadButtonOr(player, 0x11, 0x18); break;
            case 1:  v = PadButtonOr(player, 0x10, 0x18); break;
            case 2:  v = PadButtonOr(player, 0xE, 0x18); break;
            case 3:  v = PadButtonOr(player, 0xF, 0x18); break;
            case 4:  v = PadButtonOr(player, 0x12, 0x18); break;
            case 5:  v = PadButtonOr(player, 0x13, 0x18); break;
            case 6:  v = PadButtonOr(player, 0x15, 0x18); break;
            case 7:  v = PadButtonOr(player, 0x14, 0x18); break;
            case 8:  v = PadButtonOr(player, 0x16, 0x18); break;
            case 9:  v = PadButtonOr(player, 0x17, 0x18); break;
            case 10: v = PadButtonOr(player, 8, 0x18); break;
            case 11: v = PadButtonOr(player, 4, 0x18); break;
            case 12: v = PadButtonOr(player, 0x18, 0x18); break;
            case 13: v = PadButtonOr(player, 0xB, 0x18); break;
            case 14: v = PadButtonOr(player, 8, 0x18); break;
            case 15: v = PadButtonOr(player, 0x18, 0x18); break;
            case 16: v = PadButtonOr(player, 0x18, 0x18); break;
            case 17: v = PadButtonOr(player, 0x18, 0x18); break;
            case 18: v = PadButtonOr(player, 3, 0); break;
            case 19: v = 0; break;
            case 20: v = PadButtonState(player, 5); break;
            case 21: v = PadButtonState(player, 6); break;
            case 22: v = PadButtonState(player, 6); break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            case 0x1C: v = PadButtonState(player, 0x18); break;
            case 0x1D: v = PadButtonState(player, 0x18); break;
            case 0x1E: v = PadButtonState(player, 0x18); break;
            case 0x1F: v = PadButtonState(player, 0x18); break;
            case 0x20: v = PadButtonState(player, 0x18); break;
            case 0x21: v = PadButtonState(player, 0x18); break;
            case 0x22: v = PadButtonState(player, 0x18); break;
            case 0x23: v = PadButtonState(player, 0x18); break;
            case 0x24: v = PadButtonState(player, 0x18); break;
            case 0x25: v = PadButtonState(player, 0x18); break;
            default: v = 0; break;
            }
            break;
        case 7:
            switch (action) {
            case 8:  v = PadButtonOr(player, 8, 0x18); break;
            case 9:  v = PadButtonOr(player, 9, 0x18); break;
            case 10: v = PadButtonOr(player, 0x16, 0x18); break;
            case 11: v = PadButtonOr(player, 10, 0x18); break;
            case 13: v = PadButtonOr(player, 0xB, 0x18); break;
            case 14: v = PadButtonOr(player, 4, 0x18); break;
            case 15: v = PadButtonOr(player, 0x18, 0x18); break;
            case 16: v = PadButtonOr(player, 0x17, 0x18); break;
            case 0x13: v = 0; break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            default: v = 0; break;
            }
            break;
        case 8:
            switch (action) {
            case 8:  v = PadButtonOr(player, 8, 0x18); break;
            case 9:  v = PadButtonOr(player, 4, 0x18); break;
            case 10: v = PadButtonOr(player, 10, 0x18); break;
            case 11: v = PadButtonOr(player, 0xB, 0x18); break;
            case 12: v = PadButtonOr(player, 0x18, 0x18); break;
            case 13: v = PadButtonOr(player, 9, 0x18); break;
            case 14: v = PadButtonOr(player, 10, 0x18); break;
            case 15: v = PadButtonOr(player, 0x16, 0x18); break;
            case 16: v = PadButtonOr(player, 0x18, 0x18); break;
            case 17: v = PadButtonOr(player, 0x17, 0x18); break;
            case 0x13: v = 0; break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            default: v = 0; break;
            }
            break;
        case 9:
            switch (action) {
            case 4:  v = PadButtonOr(player, 0x18, 0x18); break;
            case 5:  v = PadButtonOr(player, 0x18, 0x18); break;
            case 8:  v = PadButtonOr(player, 0x16, 0x18); break;
            case 9:  v = PadButtonOr(player, 0x17, 0x18); break;
            case 10: v = PadButtonOr(player, 8, 0x18); break;
            case 11: v = PadButtonOr(player, 4, 0x18); break;
            case 13: v = PadButtonOr(player, 0xB, 0x18); break;
            case 14: v = PadButtonOr(player, 8, 0x18); break;
            case 15: v = PadButtonOr(player, 0x18, 0x18); break;
            case 16: v = PadButtonOr(player, 0x18, 0x18); break;
            case 0x13: v = 0; break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            default: v = 0; break;
            }
            break;
        case 10:
            switch (action) {
            case 8:  v = PadButtonOr(player, 8, 0x18); break;
            case 9:  v = PadButtonOr(player, 9, 0x18); break;
            case 10: v = PadButtonOr(player, 0xB, 0x18); break;
            case 11: v = PadButtonOr(player, 4, 0x18); break;
            case 12: v = PadButtonOr(player, 0x18, 0x18); break;
            case 13: v = PadButtonOr(player, 10, 0x18); break;
            case 14: v = PadButtonOr(player, 0xB, 0x18); break;
            case 15: v = PadButtonOr(player, 0x18, 0x18); break;
            case 16: v = PadButtonOr(player, 0x18, 0x18); break;
            case 17: v = PadButtonOr(player, 0x18, 0x18); break;
            case 0x13: v = 0; break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            default: v = 0; break;
            }
            break;
        case 11:
            switch (action) {
            case 8:  v = PadButtonOr(player, 8, 0x18); break;
            case 9:  v = PadButtonOr(player, 10, 0x18); break;
            case 10: v = PadButtonOr(player, 9, 0x18); break;
            case 11: v = PadButtonOr(player, 4, 0x18); break;
            case 12: v = PadButtonOr(player, 0x18, 0x18); break;
            case 13: v = PadButtonOr(player, 0xB, 0x18); break;
            case 14: v = PadButtonOr(player, 9, 0x18); break;
            case 15: v = PadButtonOr(player, 0x16, 0x18); break;
            case 16: v = PadButtonOr(player, 0x18, 0x18); break;
            case 17: v = PadButtonOr(player, 0x18, 0x18); break;
            case 0x13: v = PadButtonOr(player, 0x17, 0x18); break;
            case 0x18: v = PadButtonState(player, 1); break;
            case 0x19: v = PadButtonState(player, 3); break;
            case 0x1A: v = PadButtonState(player, 0); break;
            case 0x1B: v = PadButtonState(player, 2); break;
            default: v = 0; break;
            }
            break;
        case 12:
            if (action == 2) {
                v = PadButtonState(player, 0xC);
            } else if (action < 2) {
                if (action == 0) {
                    v = PadButtonState(player, 8);
                    return v;
                }
                if (action >= 0) {
                    v = PadButtonState(player, 9);
                    return v;
                }
            } else if (action < 4) {
                v = PadButtonState(player, 4);
                return v;
            } else {
                v = 0;
            }
            break;
        }
    }
    return v;
}

// Returns one button mask of a pad channel's button table; out-of-range
// buttons read as not pressed.
int PadButtonState(int player, int button)
{
    if (button < 0x18) {
        return *(int*)((u8*)lbl_80452560 + player * 0x8C + 4 + button * 4);
    }
    return 0;
}

// Returns the pressed-button state for an action, clamped to at most 2.
int PadPressed(int player, int mode, int action)
{
    int result = PadButtonForAction(player, mode, action);

    if (result > 2) {
        result = 2;
    }
    return result;
}

// Finds the first pad that reports "pressed" for an action, or -1.
int PadFindPressedPlayer(int mode, int action)
{
    int i = 0;

    do {
        int state = PadButtonForAction(i, mode, action);

        if (state == 1) {
            return i;
        }
        i++;
    } while (i < 4);
    return -1;
}

// Scans the button masks of every live pad channel for the first mask whose
// value is exactly 1 (pressed this frame).  The channel index sits in the high
// word of the result and the scan position inside the channel in the low word;
// the high word is -1 when no channel reports a pressed button.
unsigned long long PadFirstPressedChannel(void)
{
    int pos = 0;
    int i;

    for (i = 0; i < 4; i++) {
        int state;

        if (i < 4) {
            state = *(int*)((u8*)lbl_80452560 + i * 0x8C);
        } else {
            state = 0;
        }
        if (state != 0) {
            int g;

            pos = 0;
            for (g = 0; g < 3; g++) {
                int b;

                for (b = 0; b < 8; b++) {
                    if (*(int*)((u8*)lbl_80452560 + i * 0x8C + 4 +
                                (g * 8 + b) * 4) == 1) {
                        return ((unsigned long long)i << 32) | (u32)pos;
                    }
                }
                pos += 7;
            }
        }
    }
    return ((unsigned long long)0xFFFFFFFFu << 32) | (u32)pos;
}

// Bitmask of the pad channels that are live this frame: bit N is set for each
// channel whose state word is non-zero.
u32 PadActiveChannelMask(void)
{
    u32 mask = 0;
    int i;

    for (i = 0; i < 4; i++) {
        int state;

        if (i < 4) {
            state = *(int*)((u8*)lbl_80452560 + i * 0x8C);
        } else {
            state = 0;
        }
        if (state != 0) {
            mask |= 1 << i;
        }
    }
    return mask;
}

// ---------------------------------------------------------------------------
// Pad manager frame update.  The connection snapshot in lbl_80452790 is copied
// from the live block (current -> previous) and the live block is reset before
// the four channels are serviced.  lbl_804D4988, when installed, hears about
// every channel whose connection state changed this frame.
// ---------------------------------------------------------------------------

// Two 0x40-byte connection blocks: live state, then last frame's copy.
struct PadStateBlock {
    u8 current[0x40];   // 0x00
    u8 previous[0x40];  // 0x40
};

extern "C" PadStateBlock lbl_80452790;
extern "C" void fn_80005878(void* state);
void PadChannelUpdate(int channel, float value);
extern "C" void (*lbl_804D4988)(int channel, int state, void* data);
extern "C" void* lbl_804D498C;
extern "C" int lbl_804D4990;
extern "C" int lbl_804D4998;

// Pad state word left behind by the frame update (guess).
int PadGetState(void)
{
    return lbl_804D4998;
}

// Runs one frame of the pad manager.
void PadUpdateAll(float value)
{
    int i;

    memcpy(lbl_80452790.previous, lbl_80452790.current, 0x40);
    fn_80005878(lbl_80452790.current);
    for (i = 0; i < 4; i++) {
        PadChannelUpdate(i, value);
    }
    if (lbl_804D4988 != 0) {
        int mask = 0;

        for (i = 0; i < 4; i++) {
            int state;

            if (i < 4) {
                state = *(int*)((u8*)lbl_80452560 + i * 0x8C);
            } else {
                state = 0;
            }
            if (state != 0) {
                mask |= 1 << i;
            }
        }
        if (mask != lbl_804D4990) {
            for (i = 0; i < 4; i++) {
                int bit = 1 << i;

                if ((mask & bit) != (lbl_804D4990 & bit)) {
                    lbl_804D4988(i, (mask >> i) & 1, lbl_804D498C);
                }
            }
            lbl_804D4990 = mask;
        }
    }
}

// ---------------------------------------------------------------------------
// Pad manager: per-channel state.  lbl_80452790 holds this frame's status
// snapshot, one 0x10-byte entry per channel; lbl_80452560 holds the running
// state of the four channels.
// ---------------------------------------------------------------------------

// One channel of the pad status snapshot.
struct PadStatusEntry {
    int connected;              // 0x00
    unsigned short buttons;     // 0x04
    signed char sticks[4];      // 0x06  x, y, sub x, sub y
    u8 triggerL;                // 0x0A
    u8 triggerR;                // 0x0B
    u8 pad0C[4];                // 0x0C
};

// Per-channel pad runtime state (0x8C bytes per channel).
struct PadRuntimeChannel {
    int active;                 // 0x00
    int hold[7];                // 0x04  face buttons
    int field20;                // 0x20
    int hold2[6];               // 0x24  shoulder buttons
    int repeat[8];              // 0x3C  two auto-repeat counters per axis
    int triggerCount[2];        // 0x5C
    float stickX;               // 0x64
    float stickY;               // 0x68
    float subStickX;            // 0x6C
    float subStickY;            // 0x70
    u8 pad74[8];                // 0x74
    float triggerR;             // 0x7C
    float triggerL;             // 0x80
    u8 trigA[2];                // 0x84
    u8 pad86[2];                // 0x86
    u8 trigB[2];                // 0x88
    u8 pad8A[2];                // 0x8A
};

extern "C" float lbl_804D70DC;
extern "C" float lbl_804D70E0;
extern "C" float lbl_804D70E4;
extern "C" float lbl_804D70E8;
extern "C" float lbl_804D70EC;
extern "C" float lbl_804D70F0;
extern "C" float lbl_804D70F4;
extern "C" float lbl_804D70F8;
extern "C" float lbl_804D70FC;
extern "C" float lbl_804D7100;
extern "C" float lbl_804D7104;
extern "C" float lbl_804D7108;
extern "C" float lbl_804D710C;
extern "C" float lbl_804D7110;
extern "C" float lbl_804D7114;
extern "C" float lbl_804D7118;
extern "C" float lbl_804D711C;
extern "C" int lbl_804D4994;
extern "C" float fn_800A1128(void);
void ScreenPrintf(const char* fmt, ...);

// Resets the pad curve table to its default breakpoints and clears the
// connection-change callback.
void PadManagerInit(void)
{
    lbl_80452500[0] = lbl_804D70DC;
    lbl_804D4988 = 0;
    lbl_804D498C = 0;
    lbl_804D4994 = 1;
    lbl_804D4998 = 0;
    lbl_80452500[1] = lbl_804D70CC;
    lbl_80452500[2] = lbl_804D70CC;
    lbl_80452500[3] = lbl_804D70CC;
    lbl_80452500[4] = lbl_804D70CC;
    lbl_80452500[5] = lbl_804D70CC;
    lbl_80452500[6] = lbl_804D70E0;
    lbl_80452500[7] = lbl_804D70E4;
    lbl_80452500[8] = lbl_804D70E8;
    lbl_80452500[9] = lbl_804D70EC;
    lbl_80452500[10] = lbl_804D70F0;
    lbl_80452500[11] = lbl_804D70CC;
    lbl_80452500[12] = lbl_804D70F4;
    lbl_80452500[13] = lbl_804D70F8;
    lbl_80452500[14] = lbl_804D70FC;
    lbl_80452500[15] = lbl_804D70CC;
    lbl_80452500[16] = lbl_804D70CC;
    lbl_80452500[17] = lbl_804D70CC;
    lbl_80452500[18] = lbl_804D70DC;
    lbl_80452500[19] = lbl_804D70E4;
    lbl_80452500[20] = lbl_804D70E8;
    lbl_80452500[21] = lbl_804D70EC;
    lbl_80452500[22] = lbl_804D70F0;
    lbl_80452500[23] = lbl_804D70CC;
}

// Updates one pad channel from this frame's status snapshot: the button hold
// counters, the analogue stick vectors (or their digital fallback), the
// trigger levels and the stick auto-repeat counters.
void PadChannelUpdate(int channel, float value)
{
    PadStatusEntry* status =
        (PadStatusEntry*)((u8*)&lbl_80452790 + channel * 0x10);
    PadRuntimeChannel* ch = (PadRuntimeChannel*)&lbl_80452560[channel];
    float* sticks = (float*)&ch->stickX;
    float magnitude;
    int i;
    int j;

    (void)value;

    if (status->connected == 0) {
        ch->active = 0;
        return;
    }
    ch->active = 1;

    // Face buttons: a button that stays down keeps counting up, a fresh press
    // starts the counter at one.
    for (i = 0; i < 7; i++) {
        ch->hold[i] = (status->buttons & (1 << i))
                          ? (ch->hold[i] != 0 ? ch->hold[i] + 1 : 1)
                          : 0;
    }

    // Triggers: only restart the counter ten units above the recorded peak and
    // while the value has not dropped twenty units below the recorded trough.
    for (i = 0; i < 2; i++) {
        u8 v = (i == 0) ? status->triggerL : status->triggerR;
        int prev = ch->triggerCount[i];
        int next;

        if (v != 0 && ch->trigB[i] + 10 < v &&
            (int)ch->trigA[i] - 20 <= (int)v) {
            next = (prev != 0) ? prev + 1 : 1;
            if (prev == 0) {
                ch->trigA[i] = v;
            } else if (ch->trigA[i] < v) {
                ch->trigA[i] = v;
            }
        } else {
            next = 0;
            if (prev == 0) {
                if (v < ch->trigB[i]) {
                    ch->trigB[i] = v;
                }
            } else {
                ch->trigB[i] = v;
                ch->trigA[i] = 0;
            }
        }
        ch->triggerCount[i] = next;
    }

    // Shoulder buttons.
    for (i = 0; i < 6; i++) {
        ch->hold2[i] = (status->buttons & (1 << (i + 8)))
                           ? (ch->hold2[i] != 0 ? ch->hold2[i] + 1 : 1)
                           : 0;
    }

    if (lbl_804D4994 == 1) {
        // Analogue mode: scale each axis pair by its combined magnitude,
        // clamped to the unit range and jittered by a random factor.
        for (i = 0; i < 4; i += 2) {
            float divisor = (i == 0) ? lbl_804D7100 : lbl_804D7108;

            magnitude = (fabsf(sticks[i]) + fabsf(sticks[i + 1])) / divisor;
            for (j = 0; j < 2; j++) {
                float sign =
                    (sticks[i + j] < lbl_804D70C4) ? lbl_804D70C8 : lbl_804D70CC;
                float random = fn_800A1128();
                float amount = magnitude;
                float ratio;
                float scale;

                if (amount > lbl_804D70CC) {
                    amount = lbl_804D70CC;
                }
                if (amount < lbl_804D70C8) {
                    amount = lbl_804D70C8;
                }
                ratio = fn_800A1418(random) / fn_800A1418(lbl_804D7104);
                scale = (ratio < lbl_804D70CC) ? ratio : lbl_804D70CC;
                sticks[i + j] = (sign * amount) * scale;
            }
        }
    } else {
        // Digital mode: the raw axis values scaled into -1..1.
        for (i = 0; i < 4; i++) {
            float divisor = (i < 2) ? lbl_804D710C : lbl_804D7110;
            float v = (float)status->sticks[i] / divisor;

            if (v < lbl_804D70C8) {
                v = lbl_804D70C8;
            } else if (v > lbl_804D70CC) {
                v = lbl_804D70CC;
            }
            sticks[i] = v;
        }
    }

    if (lbl_804D4998 != 0) {
        ScreenPrintf("%d %d %d %d\n", (int)status->sticks[0],
                     (int)status->sticks[1], (int)status->sticks[2],
                     (int)status->sticks[3]);
        ScreenPrintf("%.2f %.2f %.2f %.2f\n", sticks[0], sticks[1], sticks[2],
                     sticks[3]);
    }

    ch->triggerR = (float)status->triggerR / lbl_804D7114;
    ch->triggerL = (float)status->triggerL / lbl_804D7114;

    // Two auto-repeat counters per axis: one counts while the axis is below
    // the positive threshold, the other while it is above the negative one.
    for (i = 0; i < 4; i++) {
        float v = sticks[i];

        ch->repeat[i * 2] =
            (v >= lbl_804D711C)
                ? 0
                : (ch->repeat[i * 2] != 0 ? ch->repeat[i * 2] + 1 : 1);
        ch->repeat[i * 2 + 1] =
            (lbl_804D7118 < v)
                ? (ch->repeat[i * 2 + 1] != 0 ? ch->repeat[i * 2 + 1] + 1 : 1)
                : 0;
    }
}

extern "C" float lbl_804D7130;

// Bilinear sample of a 0x60-byte emitter curve table: four x knots, four y
// knots and a 4x4 grid of values. Two of the eight tables mirror the result,
// and when the debug scale table is live the result picks up a global scale.
float SampleEmitterCurve(float x, float y, EmitterCurveTable* table, int idx,
                         int mirrored)
{
    float scale = lbl_804D70CC;

    if (mirrored == 0) {
        if ((table == &lbl_8039BA20[2] || table == &lbl_8039BA20[6]) &&
            lbl_804D499C != 0) {
            scale = lbl_8039BD20[idx];
        }
    } else {
        if ((table == &lbl_8039BA20[1] || table == &lbl_8039BA20[5]) &&
            lbl_804D499C != 0) {
            scale = lbl_8039BD20[idx];
        }
    }

    float ax = (x < lbl_804D70C4) ? -x : x;
    if (y < lbl_804D70C4) {
        y = -y;
    }

    if (ax <= table->value[0]) {
        return lbl_804D70C4;
    }
    if (ax >= table->value[3]) {
        if (x <= lbl_804D70C4) {
            return lbl_804D70C8 * scale;
        }
        return scale;
    }

    int xi = 0;
    int x0 = 0;
    int yi = 0;
    int y0 = 0;

    for (; xi < 4 && table->value[xi] <= ax; xi++) {
        x0 = xi;
    }
    for (; yi < 4 && table->value[yi + 4] <= y; yi++) {
        y0 = yi;
    }

    float frac;
    int xa;
    int xb;
    if (x0 == 3) {
        xa = 3;
        xb = 3;
        frac = lbl_804D70C4;
    } else {
        xb = x0 + 1;
        frac = (ax - table->value[x0]) / (table->value[xb] - table->value[x0]);
        xa = x0;
    }

    int ya;
    int yb;
    if (y0 == 3) {
        ya = 3;
        yb = 3;
    } else {
        ya = y0;
        yb = y0 + 1;
    }

    float v00 = table->value[xa * 4 + ya + 8];
    float lo = frac * (table->value[xa * 4 + yb + 8] - v00) + v00;
    float v10 = table->value[xb * 4 + ya + 8];
    float hi = frac * (table->value[xb * 4 + yb + 8] - v10) + v10;
    float result = frac * (hi - lo) + lo;

    if (x <= lbl_804D70C4) {
        result = -result;
    }
    if (ax > lbl_804D7130) {
        result *= scale;
    }
    return result;
}

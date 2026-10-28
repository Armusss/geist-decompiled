// u_800776FC.cpp: placed light / glow effects ("LightFx", 0x800776FC-0x8007C868).
// Two actor kinds loaded from level chunks 6 and 9/10: lights (0x174 bytes each, with a matrix, a
// flash/fade state and a particle-ish sub-state) and a second effect kind (0x64 bytes each) with a
// large message handler.  Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct PlaceF {
    float v[12];
};

// Fixed point (Q12) 3x4 matrix as 12 ints.
struct MtxQ12 {
    int m[12];
};

struct Actor;

typedef int (*ActorHandler)(int msg, void* msgData);

// Common actor header; the effect actors below embed it at offset 0.
struct Actor {
    u8 pad00[0x30];
    ActorHandler handler;   // 0x30
    u8 pad34[4];
    u32 id;                 // 0x38
    u8 pad3C[0x10];
    u32 flags4C;            // 0x4C
};

struct Msg {
    Actor* actor;           // 0x00
    int arg1;               // 0x04
    int arg2;               // 0x08
    int arg3;               // 0x0C
    int arg4;               // 0x10
    int arg5;               // 0x14
    int arg6;               // 0x18
    int arg7;               // 0x1C
};

// Functions called from other units under their placeholder names.
struct Particle;
extern "C" void fn_8007902C(void);
extern "C" void fn_8007AE7C(void);
extern "C" void fn_8007AFBC(void);
extern "C" int fn_8007C5C8(double dt, Particle* p, u32 mask);

// One light placed in a level (0x3C bytes in the chunk).
struct LightDef {
    Vec3 pos;               // 0x00
    Vec3 rot;               // 0x0C
    float f18;              // 0x18
    float f1C;              // 0x1C
    float f20;              // 0x20
    float f24;              // 0x24
    u8 r;                   // 0x28
    u8 g;                   // 0x29
    u8 b;                   // 0x2A
    u8 pad2B;
    u32 hash2C;             // 0x2C
    u32 hash30;             // 0x30
    u32 hash34;             // 0x34
    u32 select38;           // 0x38
};

struct LightHeader {
    int magic;              // 0x00 (6)
    int count;              // 0x04
    float f08;              // 0x08
    float f0C;              // 0x0C
    float f10;              // 0x10
    float f14;              // 0x14
    u8 r;                   // 0x18
    u8 g;                   // 0x19
    u8 b;                   // 0x1A
    u8 pad1B[5];
};

struct AnimPose {
    float q[4];
    Vec3 pos;
};

// Light actor (0x174 bytes).
struct LightInst {
    Actor base;             // 0x000
    u8 pad50[0x30];         // 0x050: matrix copy (12 words), pos at 0x74
    u32 flags80;            // 0x080
    float f84;              // 0x084
    float f88;              // 0x088
    float f8C;              // 0x08C
    float f90;              // 0x090
    float f94;              // 0x094
    float f98;              // 0x098
    int i9C;                // 0x09C
    int iA0;                // 0x0A0
    int iA4;                // 0x0A4
    int animA8;             // 0x0A8
    int boundsB0[6];        // 0x0B0
    Mtx34 mtx;              // 0x0C8 (pos at 0xEC)
    Mtx34 savedMtx;         // 0x0F8
    int handle128;          // 0x128
    u32 hash12C;            // 0x12C
    u32 hash130;            // 0x130
    u32 hash134;            // 0x134
    AnimPose pose138;       // 0x138 (position at 0x148)
    int fxKind;             // 0x154
    int fxTime;             // 0x158
    int fxTotal;            // 0x15C
    int fxStep;             // 0x160
    int fxAccum;            // 0x164
    int fxFrames;           // 0x168
    int fxElapsed;          // 0x16C
    int fxSpare;            // 0x170
};

extern LightHeader* lbl_804D4560;
extern LightDef* lbl_804D4564;
extern LightInst* lbl_804D4568;
extern int lbl_804D2998;
extern int (*lbl_80394A3C[])(void*, void*);

extern "C" {
int fn_800A1624(int);
int fn_800A1688(int);
void fn_8009F76C(int* m);
void fn_800913EC(double dt, int handle, void* out);
int fn_8008EA9C(int handle, int flag);
void fn_800A0168(void* a, void* b);
void fn_800A0030(void* out, void* a, void* b);
void fn_8009FC6C(void* out, void* a, void* b);
void fn_8009ED70(void* dst, void* src);
void fn_80022E8C(void* actor);
int rand(void);
void fn_8009F5B0(int* m);
void fn_8009F50C(int* out, int* in);
void fn_8009F63C(int* out, int* in);
void fn_8009F44C(int* out, int* a, int* b);
void fn_8021284C(int h);
int fn_800C6558(void);
void fn_800C654C(float a, float b);
void fn_800C653C(int r, int g, int b);
void fn_800C6534(float a);
int fn_800C69B8(void* a, void* update, void* cb);
void fn_800A025C(void* out, void* m, int axis);
void fn_800A02C0(void* m, void* v, int axis);
int fn_8007EB34(u32 a, u32 b);
void fn_80212814(u32 a, int b, int c, int d, int e, int f);
int SendMsg(int to, int msg, void* data);
void* memset(void* p, int c, u32 n);
void fn_80026680(int kind, void* cb, int arg);
void fn_80026660(int kind, void* cb, int arg);
void fn_800070C0(void* p);
void FileFlushBuffer(void* p, int n);
void fn_800070E0(void* p, int n);
}

void MtxMulQ12(int*, int*, int*);
void* VAlloc(int size);
void VFree(void* p);
void MtxSetEuler(Mtx34* m, Vec3* rot);
void PlacementToMatrix(Mtx34* m, PlaceF* p);
void ActorFn8002378C(Actor* a, int id, int placement, int cb, int p1, int p2, int p3);
void ActorFn800231A8(Actor* a);
int SelectionSetTest(u32 sel);
void ActorRefreshHostState(Actor* a, int v);
int AnimApply(int handle, char* out, int a, int b);
void BoneGetMatrix(Mtx34* m, void* q);
void ObjSendC00(void* o, int v);
int LightFxMsg(int msg, Msg* m);
void LightFxUpdate(double dt, void* sub, int unused);
void LightFxSetFlash(int sub, int kind, int frames, int amount, int force);
int LightFxSetPlace(Msg* m);
void LightFxBuild(int index);
void LightFxLoad(int unused, void* data);
void LightFxMsgThunk(int index, int msg, Msg* m);

// Rotates a placement by a pseudo-random angle derived from a seed, scaled by the distance.
void LightFxJitterMatrix(void* desc, int* m) {
    struct Desc {
        u8 pad[8];
        int amount;     // 0x08
        u8 padC[8];
        u32 seed;       // 0x14
    };
    Desc* d = (Desc*)desc;
    int sinA = fn_800A1624((d->seed & 0x1FF) << 3);
    int cosA = fn_800A1688((d->seed & 0x1FF) << 3);
    int sinB = fn_800A1624((d->seed * 4 + 0x800) & 0xFFF);
    int dist = 0x101 - d->amount / 4096;
    int tmp[12];
    int src[12];
    int i;
    fn_8009F76C(tmp);
    tmp[0] = 0x1000 - (sinA << 2) / dist;
    tmp[8] = (sinB << 2) / dist + 0x1000;
    tmp[4] = (cosA << 1) / dist + 0x1000;
    for (i = 0; i < 12; i++)
        src[i] = m[i];
    MtxMulQ12(m, tmp, src);
}

void LightFxApplyOffset(void* desc, int* m) {
    struct Desc {
        u8 pad[8];
        int amount;     // 0x08
        u8 padC[4];
        u32 flags;      // 0x10
    };
    Desc* d = (Desc*)desc;
    int v[3];
    int t[3];
    if ((d->flags & 1) == 0) {
        v[0] = d->amount / 0x8000;
    } else {
        v[0] = -d->amount / 0x8000;
    }
    v[1] = d->amount / 4096;
    v[2] = -d->amount / 0x2000;
    fn_8009F5B0(m);
    fn_8009F50C(t, v);
    m[9] += t[0];
    m[10] += t[1];
    m[11] += t[2];
}

void LightFxRandomOffset(void* desc, int* m) {
    struct Desc {
        u8 pad[8];
        int amount;     // 0x08
    };
    Desc* d = (Desc*)desc;
    int v[3];
    int t[3];
    int rot[12];
    int amt;
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
    amt = d->amount / 4096;
    if (amt > 0) {
        u32 r = rand();
        v[0] = -amt;
        if (r & 1)
            v[0] = amt;
        r = rand();
        v[2] = amt;
        if (!(r & 1))
            v[2] = -amt;
    }
    fn_8009F63C(rot, m);
    fn_8009F44C(t, rot, v);
    m[9] += t[0];
    m[10] += t[1];
    m[11] += t[2];
}

void LightFxFreeAll(void) {
    if (lbl_804D4568 != NULL) {
        int i;
        for (i = 0; i < lbl_804D4560->count; i++)
            fn_8021284C(lbl_804D4568[i].base.id);
        VFree(lbl_804D4568);
    }
}

void LightFxSpawnAll(void) {
    if (lbl_804D4560 != NULL) {
        int i;
        if (lbl_804D4560->f08 != 0.0f) {
            fn_800C6558();
            fn_800C654C(lbl_804D4560->f10, lbl_804D4560->f14);
            fn_800C653C(lbl_804D4560->r, lbl_804D4560->g, lbl_804D4560->b);
            fn_800C6534(lbl_804D4560->f0C);
        }
        if (lbl_804D4560->count > 0) {
            lbl_804D4568 = (LightInst*)VAlloc(lbl_804D4560->count * sizeof(LightInst));
            if (lbl_804D4568 != NULL) {
                memset(lbl_804D4568, 0, lbl_804D4560->count * sizeof(LightInst));
                for (i = 0; i < lbl_804D4560->count; i++)
                    LightFxBuild(i);
            }
        }
    }
}

// Builds light actor `index` from its level definition.
void LightFxBuild(int index) {
    LightInst* inst = &lbl_804D4568[index];
    LightDef* def = &lbl_804D4564[index];
    int* sub = (int*)((u8*)inst + 0x50);
    int* mtxWords = (int*)&inst->mtx;
    u32 id;
    Mtx34 outMtx;
    Vec3 axisA;
    Vec3 axisB;
    float tmp[3];
    int pos[3];
    int i;

    inst->handle128 = fn_800C69B8(sub, (void*)LightFxUpdate, (void*)LightFxSetFlash);
    MtxSetEuler(&inst->mtx, &def->rot);
    inst->mtx.m[2][1] = def->pos.x;
    inst->mtx.m[2][2] = def->pos.y;
    inst->mtx.m[2][3] = def->pos.z;
    for (i = 0; i < 12; i++)
        sub[i] = mtxWords[i];
    fn_800A025C(&axisA, sub, 1);
    fn_800A025C(&axisB, sub, 2);
    fn_800A02C0(sub, &axisA, 2);
    axisB.x = -axisB.x;
    axisB.y = -axisB.y;
    axisB.z = -axisB.z;
    fn_800A02C0(sub, &axisB, 1);
    inst->f84 = def->f18;
    inst->f90 = def->f1C;
    inst->f94 = def->f20;
    inst->f98 = def->f24;
    inst->i9C = def->r;
    inst->iA0 = def->g;
    inst->iA4 = def->b;
    inst->hash12C = def->hash2C;
    inst->hash130 = def->hash30;
    inst->hash134 = def->hash34;
    inst->boundsB0[0] = -32;
    inst->boundsB0[1] = -32;
    inst->boundsB0[2] = -32;
    inst->boundsB0[3] = 32;
    inst->boundsB0[4] = 32;
    inst->boundsB0[5] = 32;
    PlacementToMatrix(&outMtx, (PlaceF*)sub);
    id = (index & 0xFFFFFF) | 0x9000000;
    pos[0] = (int)((float*)sub)[0x24 / 4];
    pos[1] = (int)((float*)sub)[0x28 / 4];
    pos[2] = (int)((float*)sub)[0x2C / 4];
    ActorFn8002378C(&inst->base, id, (int)&outMtx, (int)LightFxMsg, (int)inst->boundsB0, (int)inst->boundsB0, 0);
    ActorFn800231A8(&inst->base);
    {
        int a = fn_8007EB34(0xFFFFFFFF, inst->hash130);
        int b = fn_8007EB34(0xF1754BF2, inst->hash134);
        fn_80212814(inst->base.id, a, b, 1, 0, 0);
    }
    SendMsg(id, 0x800, tmp);
    if (SelectionSetTest(def->select38) == 0) {
        ActorRefreshHostState(&inst->base, 0);
        ObjSendC00(inst, 0);
    }
}

// Level chunk 9 / 6 hooks: first the setup that clears the globals.
void LightFxInit(void) {
    lbl_804D4560 = NULL;
    lbl_804D4564 = NULL;
    lbl_804D4568 = NULL;
    fn_80026680(9, (void*)LightFxLoad, 0);
    fn_80026660(9, (void*)LightFxMsgThunk, 0);
    memset(&lbl_804D2998, 0, 4);
}

// Starts a timed flash effect on a light (kinds 1..3); ignored if one is running unless forced.
void LightFxSetFlash(int sub, int kind, int frames, int amount, int force) {
    LightInst* inst;
    if (frames == 0)
        return;
    if (kind < 1)
        return;
    if (kind > 3)
        return;
    if (sub == 0)
        return;
    if (sub == 0)
        inst = NULL;
    else
        inst = (LightInst*)(sub - 0x50);
    if (inst->fxKind != 0 && force == 0)
        return;
    inst->fxKind = kind;
    inst->fxTime = frames;
    inst->fxTotal = amount << 12;
    inst->fxStep = inst->fxTotal / frames;
    inst->fxAccum = 0;
    inst->fxFrames = 0;
}

// Per-frame update of a light: follows its animation, fades, runs the flash effect.
void LightFxUpdate(double dt, void* subp, int unused) {
    LightInst* inst;
    bool active;
    u32 flags;
    Mtx34 tmpMtx;
    Mtx34 m1;
    Mtx34 m2;
    Vec3 axA;
    Vec3 axB;
    float anim[3];
    Mtx34 outMtx;

    if (subp == NULL)
        inst = NULL;
    else
        inst = (LightInst*)((u8*)subp - 0x50);
    flags = inst->base.flags4C;
    if ((flags & 0x400) != 0) {
        inst->base.flags4C = flags & ~0x400;
        return;
    }
    active = false;
    if (inst != NULL && ((flags & 0x40) != 0 || (flags & 1) != 0 || (inst->base.id >> 24) == 0xC))
        active = true;
    if (!active)
        return;
    if (inst->animA8 != -1) {
        fn_800913EC(dt, inst->animA8, anim);
        if (fn_8008EA9C(inst->animA8, 0) != 0) {
            if ((inst->flags80 & 0x1000) == 0) {
                AnimApply(inst->animA8, (char*)&inst->pose138, 0, 1);
                BoneGetMatrix(&inst->mtx, &inst->pose138);
                inst->mtx.m[2][1] = inst->pose138.pos.x;
                inst->mtx.m[2][2] = inst->pose138.pos.y;
                inst->mtx.m[2][3] = inst->pose138.pos.z;
            } else {
                int w[12];
                int i;
                int* saved = (int*)&inst->savedMtx;
                AnimApply(inst->animA8, (char*)&inst->pose138, 0, 1);
                for (i = 0; i < 12; i++)
                    w[i] = saved[i];
                fn_800A025C(&m1, w, 1);
                fn_800A025C(&axA, w, 2);
                fn_800A02C0(w, &m1, 2);
                axA.x = -axA.x;
                axA.y = -axA.y;
                axA.z = -axA.z;
                fn_800A02C0(w, &axA, 1);
                fn_800A0168(w, w);
                fn_800A0030(&inst->mtx.m[2][1], w, &inst->pose138.pos);
                inst->mtx.m[2][1] += inst->savedMtx.m[2][1];
                inst->mtx.m[2][2] += inst->savedMtx.m[2][2];
                inst->mtx.m[2][3] += inst->savedMtx.m[2][3];
                BoneGetMatrix(&tmpMtx, &inst->pose138);
                for (i = 0; i < 12; i++)
                    w[i] = saved[i];
                fn_800A025C(&m2, w, 1);
                fn_800A025C(&axB, w, 2);
                fn_800A02C0(w, &m2, 2);
                axB.x = -axB.x;
                axB.y = -axB.y;
                axB.z = -axB.z;
                fn_800A02C0(w, &axB, 1);
                fn_8009FC6C(&inst->mtx, &tmpMtx, w);
            }
        }
    }
    if ((inst->flags80 & 0x4000) != 0) {
        if (dt < (double)inst->f8C) {
            inst->f84 = (inst->f88 - inst->f84) * (float)(dt / (double)inst->f8C) + inst->f84;
            inst->f8C = (float)((double)inst->f8C - dt);
        } else {
            inst->f84 = inst->f88;
            inst->flags80 &= ~0x4000;
        }
    }
    {
        int i;
        int* dst = (int*)subp;
        int* src = (int*)&inst->mtx;
        for (i = 0; i < 12; i++)
            dst[i] = src[i];
    }
    fn_800A025C(&m1, subp, 1);
    fn_800A025C(&axA, subp, 2);
    fn_800A02C0(subp, &m1, 2);
    axA.x = -axA.x;
    axA.y = -axA.y;
    axA.z = -axA.z;
    fn_800A02C0(subp, &axA, 1);
    if (inst->fxKind != 0) {
        int step;
        PlacementToMatrix(&outMtx, (PlaceF*)subp);
        lbl_80394A3C[inst->fxKind](&inst->fxKind, &outMtx);
        step = (int)dt;
        inst->fxTime -= step;
        if (inst->fxTime < 0) {
            inst->fxKind = 0;
        } else {
            inst->fxTotal -= inst->fxStep * step;
            if (inst->fxTotal < 0)
                inst->fxKind = 0;
            inst->fxFrames += 1;
            inst->fxElapsed += step;
        }
        fn_8009ED70(subp, &outMtx);
    }
    PlacementToMatrix((Mtx34*)inst, (PlaceF*)subp);
    fn_80022E8C(inst);
}

// Level chunk 6 loader: byte-swaps the header and the light definitions in place.
void LightFxLoad(int unused, void* data) {
    int* hdr = (int*)data;
    int i;
    lbl_804D4560 = (LightHeader*)data;
    fn_800070C0(&hdr[0]);
    fn_800070C0(&hdr[1]);
    fn_800070C0(&hdr[2]);
    fn_800070C0(&hdr[3]);
    fn_800070C0(&hdr[4]);
    fn_800070C0(&hdr[5]);
    if (lbl_804D4560->magic == 6) {
        if (lbl_804D4560->count > 0) {
            lbl_804D4564 = (LightDef*)(hdr + 8);
            for (i = 0; i < lbl_804D4560->count; i++) {
                LightDef* d = &lbl_804D4564[i];
                FileFlushBuffer(&d->pos, 0xC);
                FileFlushBuffer(&d->rot, 0xC);
                fn_800070C0(&d->f18);
                fn_800070C0(&d->f1C);
                fn_800070C0(&d->f20);
                fn_800070C0(&d->f24);
                fn_800070C0(&d->hash2C);
                fn_800070C0(&d->hash30);
                fn_800070C0(&d->hash34);
            }
        }
    } else {
        lbl_804D4560 = NULL;
    }
}

void LightFxMsgThunk(int index, int msg, Msg* m) {
    m->actor = (Actor*)&lbl_804D4568[index];
    LightFxMsg(msg, m);
}

extern "C" {
int fn_800C5210(u32 id, int msg, Msg* m);
int fn_80084540(u32 id, int msg, Msg* m, void* buf);
void fn_80024128(double a, void* b, void* c);
void fn_8009EC5C(void* m, void* v);
void fn_800C6664(int h);
void fn_800C6834(int h);
void fn_800C6560(int h, int v);
void fn_800C6760(int h, int v);
int fn_80090EA4(u32 id, int v);
void fn_8008F378(double a, int h);
void fn_80090778(double a, int h, int b, int c, int d, int e, int f);
void fn_80090974(double a, int h, int b, int c, int d, int e, int f);
}
void VecSwapXY(IVec3* out, IVec3* in);
void FixedToRad(Vec3* out, IVec3* in);

// Message handler of the light actors.
int LightFxMsg(int msg, Msg* m) {
    u32 id = m->actor->id;
    u32 index = id & 0xFFFFFF;
    LightInst* inst;
    int r;
    char buf[4];

    if (msg == 0x414) {
        LightFxSetPlace(m);
        m->actor->flags4C |= 0x400;
        return 1;
    }
    r = fn_800C5210(id, msg, m);
    if (r != 0)
        return r;
    r = fn_80084540(id, msg, m, buf);
    if (r != 0)
        return r;
    inst = &lbl_804D4568[index];
    switch (msg) {
    case 0:
        if (lbl_804D2998 != 0) {
            int w[12];
            Vec3 a;
            Vec3 b;
            int i;
            int* src = (int*)&inst->mtx;
            for (i = 0; i < 12; i++)
                w[i] = src[i];
            fn_800A025C(&a, w, 1);
            fn_800A025C(&b, w, 2);
            fn_800A02C0(w, &a, 2);
            b.x = -b.x;
            b.y = -b.y;
            b.z = -b.z;
            fn_800A02C0(w, &b, 1);
            fn_80024128(128.0f, &w[9], w);
        }
        return 1;
    case 0x400:
        m->arg1 = index | 0x9000000;
        return 1;
    case 0x41A:
        m->arg1 = fn_8007EB34(0xFFFFFFFF, inst->hash12C);
        return 1;
    case 0x411:
    case 0x412:
        PlacementToMatrix((Mtx34*)&m->arg4, (PlaceF*)&inst->mtx);
        return 1;
    case 0x413:
        fn_8009ED70(&inst->mtx, (void*)m->arg1);
        return 1;
    case 0x41E:
        if (m->arg1 == 0x4F498E60)
            return (int)inst->f8C;
        if (m->arg1 == 0x110AD)
            return (int)inst->f84;
        if (m->arg1 == 0x7C18FE9E) {
            m->arg3 = fn_8007EB34(0xFFFFFFFF, inst->hash12C);
            return 1;
        }
        return 0;
    case 0x41D:
        if (m->arg1 == 0x110AD) {
            inst->f88 = (float)m->arg3;
            if ((inst->flags80 & 0x4000) == 0) {
                inst->f8C = 0.0f;
                inst->flags80 |= 0x4000;
            }
            return 1;
        }
        if (m->arg1 == -0x6617C20E) {
            inst->f8C = (float)m->arg3;
            inst->flags80 |= 0x4000;
            return 1;
        }
        return 0;
    case 0xC00:
        if (m->arg1 == 0)
            fn_800C6664(inst->handle128);
        else
            fn_800C6834(inst->handle128);
        return 1;
    case 0xC18:
        if (m->arg1 == 0)
            fn_800C6560(inst->handle128, m->arg3);
        else
            fn_800C6760(inst->handle128, m->arg3);
        return 1;
    case 0xC07:
        if ((m->arg1 & 2) != 0) {
            IVec3 swapped;
            Vec3 euler;
            Vec3 axA;
            Vec3 axB;
            VecSwapXY(&swapped, (IVec3*)&m->arg7);
            FixedToRad(&euler, &swapped);
            MtxSetEuler(&inst->mtx, &euler);
            fn_800A025C(&axA, &inst->mtx, 1);
            fn_800A025C(&axB, &inst->mtx, 2);
            axA.x = -axA.x;
            axA.y = -axA.y;
            axA.z = -axA.z;
            fn_800A02C0(&inst->mtx, &axB, 1);
            fn_800A02C0(&inst->mtx, &axA, 2);
        }
        if ((m->arg1 & 1) != 0) {
            inst->mtx.m[2][1] = (float)m->arg4;
            inst->mtx.m[2][2] = (float)m->arg5;
            inst->mtx.m[2][3] = (float)m->arg6;
        }
        return 1;
    case 0xC04:
    case 0xC09:
    case 0xC0D:
    case 0xC10:
    case 0xC11:
    case 0xC12: {
        float rate;
        if (inst->animA8 == -1) {
            inst->animA8 = fn_80090EA4(index | 0x9000000, -2);
            if (inst->animA8 == -1)
                return 0;
        }
        inst->flags80 &= ~0x3000;
        if (msg == 0xC0D || msg == 0xC10) {
            int i;
            int* dst = (int*)&inst->savedMtx;
            int* src = (int*)&inst->mtx;
            inst->flags80 |= 0x1000;
            for (i = 0; i < 12; i++)
                dst[i] = src[i];
        } else if (msg == 0xC09 || msg == 0xC11) {
            inst->flags80 |= 0x2000;
        }
        rate = 1.0f;
        if (m->arg4 != 0)
            rate = (float)m->arg4 / 4096.0f;
        fn_8008F378(rate, inst->animA8);
        switch (msg) {
        case 0xC04:
        case 0xC09:
        case 0xC0D:
            fn_80090778(rate, inst->animA8, 0, m->arg1, m->arg3, m->arg5, m->arg6);
            break;
        case 0xC10:
        case 0xC11:
        case 0xC12:
            fn_80090974(rate, inst->animA8, 0, m->arg1, m->arg3, m->arg5, m->arg6);
            break;
        }
        return 1;
    }
    case 0x1801: {
        int found[3];
        if (SendMsg(m->arg1, 0x400, found) != 0 && found[0] != 0) {
            Mtx34 mt;
            IVec3 delta;
            Vec3 axA;
            Vec3 axB;
            Actor* other = (Actor*)found[0];
            PlacementToMatrix(&mt, (PlaceF*)&inst->mtx);
            delta.x = ((int*)other)[0x24 / 4] - (int)mt.m[2][1];
            delta.y = ((int*)other)[0x28 / 4] - (int)mt.m[2][2];
            delta.z = ((int*)other)[0x2C / 4] - (int)mt.m[2][3];
            fn_8009EC5C(&mt, &delta);
            fn_8009ED70(&inst->mtx, &mt);
            fn_800A025C(&axA, &inst->mtx, 1);
            fn_800A025C(&axB, &inst->mtx, 2);
            fn_800A02C0(&inst->mtx, &axB, 1);
            axA.x = -axA.x;
            axA.y = -axA.y;
            axA.z = -axA.z;
            fn_800A02C0(&inst->mtx, &axA, 2);
        }
        return 1;
    }
    case 0x7203:
        ActorRefreshHostState(m->actor, m->arg1);
        return 1;
    }
    return 0;
}

// Message 0x414: copies a placement into the light and applies it.
int LightFxSetPlace(Msg* m) {
    int* src = (int*)m->arg3;
    LightInst* inst = (LightInst*)m->actor;
    Vec3 a;
    Vec3 b;
    if (src != NULL) {
        int i;
        int* dst = (int*)&inst->mtx;
        for (i = 0; i < 12; i++)
            dst[i] = src[i];
        fn_800A025C(&a, &inst->mtx, 1);
        fn_800A025C(&b, &inst->mtx, 2);
        a.x = -a.x;
        a.y = -a.y;
        a.z = -a.z;
        fn_800A02C0(&inst->mtx, &a, 2);
        fn_800A02C0(&inst->mtx, &b, 1);
    }
    LightFxUpdate((double)(float)m->arg1, (u8*)inst + 0x50, 0);
    return 1;
}

void fn_8007902C(void) {}

// ---- Second effect kind (level chunk 9/10): "spirit trail" markers, 0x64 bytes per instance ----

// Definition in the level chunk (0x4C bytes).
struct FxDef {
    Vec3 pos;               // 0x00
    Vec3 rot;               // 0x0C
    int bounds[6];          // 0x18
    u32 hash30;             // 0x30
    u32 hash34;             // 0x34
    u32 hash38;             // 0x38
    u32 hash3C;             // 0x3C
    u32 kind40;             // 0x40
    u32 hash44;             // 0x44
    u32 select48;           // 0x48
};

struct FxInst {
    Actor base;             // 0x00
    u8 pad50[4];
    u32 kind50;             // 0x50
    u8 rgba[4];             // 0x54
    float f58;              // 0x58
    int h5C;                // 0x5C
    int h60;                // 0x60
};

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    int pad04;
    int size;               // 0x08
    int* data;              // 0x0C
};

class FxList : public Buffer {
public:
    virtual ~FxList();
};

extern int lbl_804D4570;
extern FxInst* lbl_804D4574;
extern int lbl_804D4578;
extern FxList* lbl_804D457C;

extern "C" {
int fn_80022D80(void* actor);
void fn_80022D98(void* actor, int v);
void fn_8009F028(void* m, void* euler);
}
int FxMsg(int msg, Msg* m);
void FxListLoad(int unused, void* data);
void FxMsgThunk(int index, int msg, Msg* m);
double __fabs(double);
int SaveIsBusy(void);
void ActorFn80022D98(Actor* a, int v);

void FxListFree(void) {
    if (lbl_804D4574 != NULL)
        VFree(lbl_804D4574);
    if (lbl_804D457C != NULL)
        delete lbl_804D457C;
}

FxList::~FxList() {}

// Per-frame update of every instance: sends the 0x801 update message and fades f58 toward zero.
int FxListUpdate(double dt) {
    int i;
    for (i = 0; i < lbl_804D4578; i++) {
        FxInst* inst = &lbl_804D4574[i];
        bool active = false;
        if (inst != NULL &&
            ((inst->base.flags4C & 0x40) != 0 || (inst->base.flags4C & 1) != 0 || (inst->base.id >> 24) == 0xC))
            active = true;
        if (!active)
            continue;
        if ((inst->base.flags4C & 0x400) == 0) {
            if (fn_80022D80(inst) != 0) {
                struct { Actor* a; int unused; float dt; } msg;
                msg.dt = (float)dt;
                msg.unused = 0;
                msg.a = &inst->base;
                inst->base.handler(0x801, &msg);
                if (0.0f != inst->f58) {
                    if (dt < __fabs((double)inst->f58)) {
                        float sign = 1.0f;
                        if (inst->f58 < 0.0f)
                            sign = -1.0f;
                        inst->f58 = -(float)(dt * sign - inst->f58);
                    } else {
                        inst->f58 = 0.0f;
                    }
                }
                fn_80022E8C(inst);
            }
        } else {
            inst->base.flags4C &= ~0x400;
        }
    }
    return 1;
}

void FxListBroadcast(void) {
    int i;
    int msg = 0x800;
    if (SaveIsBusy() != 0)
        msg = 0x803;
    for (i = 0; i < lbl_804D4578; i++) {
        struct { Actor* a; } data;
        data.a = &lbl_804D4574[i].base;
        data.a->handler(msg, &data);
    }
}

int FxMsgCallback(void* a, void* b);
int FxStateTick(void* a);

void FxListSpawnAll(void) {
    int i;
    if (lbl_804D4578 != 0)
        lbl_804D4574 = (FxInst*)VAlloc(lbl_804D4578 * sizeof(FxInst));
    for (i = 0; i < lbl_804D4578; i++) {
        FxDef* def = &((FxDef*)lbl_804D4570)[i];
        FxInst* inst = &lbl_804D4574[i];
        Mtx34 mt;
        fn_8009F028(&mt, &def->rot);
        mt.m[2][1] = def->pos.x;
        mt.m[2][2] = def->pos.y;
        mt.m[2][3] = def->pos.z;
        inst->kind50 = 0x101AB;
        inst->rgba[0] = 0xFF;
        inst->rgba[1] = 0xFF;
        inst->rgba[2] = 0xFF;
        inst->rgba[3] = 0xFF;
        inst->f58 = 0.0f;
        inst->h60 = -1;
        ActorFn8002378C(&inst->base, (i & 0xFFFFFF) | 0xA000000, (int)&mt, (int)FxMsg, (int)def->bounds,
                        (int)def->bounds, def->hash3C);
        ActorFn800231A8(&inst->base);
        if ((inst->base.flags4C & 0x10000) != 0)
            ActorFn80022D98(&inst->base, 0);
        {
            int a = fn_8007EB34(0xFFFFFFFF, def->hash34);
            int b = fn_8007EB34(0xF1754BF2, def->hash38);
            fn_80212814(inst->base.id, a, b, 1, 0, 0);
        }
        if (def->kind40 == 0x1F85A947)
            inst->h5C = fn_8007EB34(0x92234D44, def->hash44);
        else
            inst->h5C = -1;
        if (SelectionSetTest(def->select48) == 0) {
            ActorRefreshHostState(&inst->base, 0);
            ObjSendC00(inst, 0);
        }
    }
}

int FxListInit(void) {
    FxList* list;
    lbl_804D4570 = 0;
    lbl_804D4574 = NULL;
    lbl_804D4578 = 0;
    list = new FxList;
    lbl_804D457C = list;
    fn_80026680(10, (void*)FxListLoad, 0);
    fn_80026660(10, (void*)FxMsgThunk, 0);
    return 1;
}

// Level chunk 10 loader: byte-swaps the definitions in place.
void FxListLoad(int unused, void* data) {
    short* hdr = (short*)data;
    int i;
    fn_800070E0(hdr, 0x10);
    if (hdr[0] == 9) {
        lbl_804D4570 = (int)(hdr + 16);
        lbl_804D4578 = hdr[1];
        for (i = 0; i < lbl_804D4578; i++) {
            FxDef* d = &((FxDef*)lbl_804D4570)[i];
            FileFlushBuffer(d->bounds, 0x18);
            FileFlushBuffer(&d->rot, 0xC);
            FileFlushBuffer(&d->pos, 0xC);
            fn_800070C0(&d->hash30);
            fn_800070C0(&d->hash34);
            fn_800070C0(&d->hash38);
            fn_800070C0(&d->hash3C);
            fn_800070C0(&d->kind40);
            fn_800070C0(&d->hash44);
            fn_800070C0(&d->select48);
        }
    }
}

void FxMsgThunk(int index, int msg, Msg* m) {
    m->actor = &lbl_804D4574[index].base;
    FxMsg(msg, m);
}

// STUB (not drafted yet, 0x1248 bytes at 0x80079618): message handler of the FxInst actors.
int FxMsg(int msg, Msg* m) {
    return 0;
}

// STUB (not drafted yet, 0x424 bytes at 0x8007A860): spawns the spirit-trail effects when an actor is touched.
void FxTouch(Actor* fx, FxDef* def, Actor* other) {}

// ---- Flicker lights (level chunk 5) and the pool of running flicker patterns ----

struct FadeTask {
    int fx;                 // 0x00
    int level;              // 0x04
    int fadeIn;             // 0x08
};

extern int lbl_804D29A0;
extern int lbl_804D29A4;

extern "C" {
void fn_8016EEF4(int fx);
void fn_8016F018(int fx, int level);
void fn_8028BC2C(void* p);
void fn_8028BC28(void* a, void* b, u32 c);
int FxStart(int a, int b, int c);
int fn_8016F390(int a, int b, int c);
int fn_8003A1C0(int v);
int fn_8003A18C(int v);
int fn_8003ADD4(void* m, int a, int b, int c, int d);
int fn_8003E620(int a, int b, int c, int d, void* e, void* f);
void fn_8003E5AC(void);
void fn_8003E5B0(void);
void fn_8003D8F4(int v);
int ModelBoneCount(int v);
int fn_802F6B10(const char* a, const char* b);
double fmod(double a, double b);
void* memcpy(void* d, const void* s, u32 n);
}
void ActorGetMatrix(Mtx34* m, Actor* a);
int TimerAdd(int a, float (*cb)(void*), void* arg, float delay);

// Fades a music/fx handle in or out each tick; returns the delay until the next call.
float FadeTaskTick(void* p) {
    FadeTask* t = (FadeTask*)p;
    int v;
    if (t->fadeIn == 0) {
        t->level -= 400;
        v = lbl_804D29A4;
        if (lbl_804D29A4 < t->level)
            v = t->level;
        t->level = v;
        if (t->level == 0) {
            fn_8016EEF4(t->fx);
            VFree(t);
            return 0.0f;
        }
        fn_8016F018(t->fx, t->level);
        return 100.0f;
    }
    t->level += 400;
    v = lbl_804D29A0;
    if (t->level < lbl_804D29A0)
        v = t->level;
    t->level = v;
    fn_8016F018(t->fx, t->level);
    if (t->level == 0x1000) {
        VFree(t);
        return 0.0f;
    }
    return 100.0f;
}

int ReturnZero(void) {
    return 0;
}

struct ListObj {
    u8 pad[4];
};

void FlickerBufClear(void* begin, void* end);
void FlickerBufNop(void* begin, void* end);

void FlickerBufConstruct(void* self, void* begin, u32 size) {
    fn_8028BC2C(begin);
    FlickerBufClear(begin, (u8*)begin + (size & ~7));
}

void FlickerBufDestruct(void* self, void* begin, u32 size) {
    FlickerBufNop(begin, (u8*)begin + (size & ~7));
    fn_8028BC28(self, begin, size);
}

int ReturnEight(void) {
    return 8;
}

void FlickerBufClear(void* begin, void* end) {
    int* p = (int*)begin;
    int* e = (int*)end;
    if (p >= e)
        return;
    do {
        if (p != NULL) {
            p[0] = 0;
            p[1] = 0;
        }
        p += 2;
    } while (p < e);
}

void FlickerBufNop(void* begin, void* end) {
    u32 b = (u32)begin;
    u32 e = (u32)end;
    if (e <= b)
        return;
    do {
    } while (--e > b + 7 - b);
}

extern int lbl_804D4584;
extern u8* lbl_804D4580;
extern u32* lbl_804D4588;
extern int lbl_804D458C;
extern int lbl_804D4590;

void FlickerResetCount(void) {
    lbl_804D4584 = 0;
}

// Draws the shadow casters of every flicker light queued since the last frame.
void fn_8007AE7C(void) {
    if (lbl_804D4590 != 0) {
        int w;
        int base = 0;
        for (w = 0; w < lbl_804D458C; w++) {
            u32 bits = lbl_804D4588[w];
            if (bits != 0) {
                int bit;
                u8* ent = lbl_804D4580 + base * 0x90;
                for (bit = 0; bit < 32; bit++, ent += 0x90) {
                    if ((1 << bit) & bits) {
                        int* e = (int*)ent;
                        int p1[3];
                        int p2[3];
                        Mtx34 mt;
                        int model;
                        int r;
                        p1[0] = e[0x6C / 4];
                        p1[1] = e[0x70 / 4];
                        p1[2] = e[0x74 / 4];
                        p2[0] = e[0x7C / 4];
                        p2[1] = e[0x84 / 4];
                        p2[2] = e[0x80 / 4];
                        ActorGetMatrix(&mt, (Actor*)ent);
                        model = ModelBoneCount(e[0x5C / 4]);
                        r = fn_8003ADD4(&mt, e[0x5C / 4], 0, 0, model);
                        fn_8003E620(e[0x5C / 4], r, model, e[0x64 / 4], p1, p2);
                    }
                }
                lbl_804D4588[w] = 0;
            }
            base += 32;
        }
        lbl_804D4590 = 0;
    }
    fn_8003E5AC();
}

void fn_8007AFBC(void) {
    fn_8003E5B0();
}

// One running pattern (0x44 bytes) in the flicker pool.
struct FlickerEntry {
    int key;                // 0x00
    char* buf;              // 0x04
    int pos;                // 0x08
    int len;                // 0x0C
    int params[6];          // 0x10
    int (*cb)(int, int, FlickerEntry*); // 0x28
    FlickerEntry* cbArg;    // 0x2C
    int target;             // 0x30
    int cbData;             // 0x34
    int next;               // 0x38
    int prev;               // 0x3C
    int owner;              // 0x40
};

struct FlickerPattern {
    const char* name;
    const char* data;
    int len;
};

extern FlickerEntry lbl_80423870[16];
extern char lbl_80423CB0[16][700];
extern FlickerPattern lbl_80395154[11];
extern int lbl_804D4594;
extern int lbl_804D4598;

int FlickerEntryRun(double dt, FlickerEntry* e);

int FlickerUpdateAll(double dt) {
    int i = lbl_804D4598;
    while (i != -1) {
        FlickerEntry* e = &lbl_80423870[i];
        int next = e->next;
        if (FlickerEntryRun(dt, e) == 0) {
            e->buf = NULL;
            if (lbl_804D4598 == i)
                lbl_804D4598 = e->next;
            else
                lbl_80423870[e->prev].next = e->next;
            if (e->next != -1)
                lbl_80423870[e->next].prev = e->prev;
            e->next = lbl_804D4594;
            lbl_804D4594 = i;
        }
        i = next;
    }
    return 1;
}

// Flicker light definition in the level chunk (0x90 bytes).
struct FlickerDef {
    Actor base;             // 0x00 (the actor is constructed in place)
    u8 pad50[0x3C];
    int model5C;            // 0x5C
    int unk60;              // 0x60
    u8 pad64[0x2C];
};

int FlickerLightMsg(int msg, Msg* m);
void FlickerLoad(int unused, void* data);
int FlickerMsgThunk(int index, int msg, Msg* m);
int FlickerStart(Msg* m, void* def, int (*cb)(int, int, FlickerEntry*), FlickerEntry* cbArg);
int FlickerNotify(int a, int b, FlickerEntry* e);

// (Re)creates every flicker light actor from the chunk.
void FlickerSpawnAll(void) {
    int i;
    int off = 0;
    for (i = 0; i < lbl_804D4584; i++, off += 0x90) {
        int* d = (int*)(lbl_804D4580 + off);
        int w[20];
        int k;
        for (k = 0; k < 14; k++)
            w[k] = d[k];
        w[14] = d[0xE];
        w[15] = fn_8003A1C0(d[0x17]);
        w[16] = fn_8003A18C(d[0x17]);
        w[19] = d[0x13];
        ActorFn8002378C((Actor*)d, (i & 0xFFFFFF) | 0x6000000, (int)w, (int)FlickerLightMsg, w[15], w[16], w[19]);
        ActorFn800231A8((Actor*)d);
        ActorFn80022D98((Actor*)d, d[0x18]);
        if (SelectionSetTest(d[0x23]) == 0) {
            ActorRefreshHostState((Actor*)d, 0);
            ObjSendC00(d, 0);
        }
    }
    if (lbl_804D4588 != NULL) {
        int k;
        for (k = 0; k < lbl_804D458C; k++)
            lbl_804D4588[k] = 0;
    }
    lbl_804D4590 = 0;
}

int FlickerInit(void) {
    int i;
    lbl_804D4584 = 0;
    lbl_804D458C = 0;
    lbl_804D4580 = NULL;
    lbl_804D4588 = NULL;
    fn_80026680(6, (void*)FlickerLoad, 0);
    fn_80026660(6, (void*)FlickerMsgThunk, 0);
    lbl_804D4590 = 0;
    memset(lbl_80423870, 0, sizeof(lbl_80423870));
    for (i = 0; i < 15; i++) {
        lbl_80423870[i].next = i + 1;
        lbl_80423870[i].prev = i - 1;
    }
    lbl_80423870[15].next = -1;
    lbl_804D4594 = 0;
    lbl_804D4598 = -1;
    return 1;
}

void FlickerLoad(int unused, void* data) {
    short* hdr = (short*)data;
    int i;
    fn_800070E0(hdr, 0x10);
    if (hdr[0] == 5) {
        int off = 0;
        lbl_804D4580 = (u8*)(hdr + 16);
        lbl_804D4584 = (u16)hdr[1];
        for (i = 0; i < lbl_804D4584; i++, off += 0x90) {
            FileFlushBuffer(lbl_804D4580 + off, 0x50);
            fn_800070C0(lbl_804D4580 + off + 0x5C);
            FileFlushBuffer(lbl_804D4580 + off + 0x50, 0xC);
            FileFlushBuffer(lbl_804D4580 + off + 0x60, 0x20);
        }
        lbl_804D4588 = (u32*)(hdr + lbl_804D4584 * 0x90 + 16);
        lbl_804D458C = lbl_804D4584 / 32 + 1;
        FileFlushBuffer(lbl_804D4588, lbl_804D458C * 4);
    }
}

// Unlinks pattern `i` from the active list and returns it to the free list.
static inline void FlickerRelease(int i) {
    FlickerEntry* e = &lbl_80423870[i];
    e->buf = NULL;
    if (lbl_804D4598 == i)
        lbl_804D4598 = e->next;
    else
        lbl_80423870[e->prev].next = e->next;
    if (e->next != -1)
        lbl_80423870[e->next].prev = e->prev;
    e->next = lbl_804D4594;
    lbl_804D4594 = i;
}

int FlickerLightMsg(int msg, Msg* m);

int FlickerMsgThunk(int index, int msg, Msg* m) {
    int* w = (int*)m;
    if (msg == 0x6001) {
        FlickerRelease(w[1]);
    } else if (msg > 0x6000 || msg < 0x6000) {
        m->actor = (Actor*)(lbl_804D4580 + index * 0x90);
        return FlickerLightMsg(msg, m);
    } else if (w[1] == -1) {
        w[12] = FlickerStart(m, lbl_804D4580 + index * 0x90, NULL, NULL);
    } else {
        w[12] = FlickerStart(m, lbl_804D4580 + index * 0x90, FlickerNotify, NULL);
        lbl_80423870[w[12]].cbArg = &lbl_80423870[w[12]];
        lbl_80423870[w[12]].cbData = w[2];
    }
    return 1;
}

// Message handler shared by the flicker light actors.
int FlickerLightMsg(int msg, Msg* m) {
    u8* a = (u8*)m->actor;
    u32* flags = &m->actor->flags4C;
    if (msg == 0xC00) {
        *(int*)(a + 0x60) = m->arg1;
        ActorFn80022D98(m->actor, *(int*)(a + 0x60));
        return 1;
    }
    if (msg == 6) {
        fn_8003D8F4(*(int*)(a + 0x5C));
        return 1;
    }
    if (msg == 0) {
        int idx = ((int)(a - lbl_804D4580)) / 0x90;
        lbl_804D4588[idx / 32] |= 1 << (idx % 32);
        lbl_804D4590++;
        return 1;
    }
    if (msg == 0x3001) {
        *flags &= ~1;
        return 1;
    }
    if (msg == 0x3000) {
        *flags |= 1;
        return 1;
    }
    return 0;
}

// Notifies the owning actor that a pattern finished or advanced.
int FlickerNotify(int a, int b, FlickerEntry* e) {
    struct { int z; int a; int b; int data; int out; } n;
    n.z = 0;
    n.data = e->cbData;
    n.out = 0;
    n.a = a;
    n.b = b;
    SendMsg(e->target, 0x6002, &n);
    return n.out;
}

// Starts a pattern described by message `m`; returns the pool index or -1.
int FlickerStart(Msg* m, void* def, int (*cb)(int, int, FlickerEntry*), FlickerEntry* cbArg) {
    int* w = (int*)m;
    u32 k;
    int idx;
    FlickerEntry* e;
    int cur;
    int prev;

    for (k = 0; k < 11; k++) {
        if (fn_802F6B10((const char*)w[5], lbl_80395154[k].name) == 0)
            break;
    }
    if (k == 11)
        return -1;
    idx = lbl_804D4594;
    if (idx == -1)
        return -1;
    e = &lbl_80423870[idx];
    lbl_804D4594 = e->next;
    e->buf = lbl_80423CB0[idx];
    e->pos = 0;
    e->key = w[3];
    memcpy(e->buf, lbl_80395154[k].data, lbl_80395154[k].len);
    e->len = lbl_80395154[k].len;
    e->params[0] = w[6];
    e->params[1] = w[7];
    e->params[2] = w[8];
    e->params[3] = w[9];
    e->params[4] = w[10];
    e->params[5] = w[11];
    e->cb = cb;
    e->cbArg = cbArg;
    e->target = w[1];
    e->owner = (int)def;
    cur = lbl_804D4598;
    if (cur == -1) {
        lbl_804D4598 = idx;
        lbl_80423870[idx].prev = -1;
        lbl_80423870[idx].next = -1;
        return idx;
    }
    for (;;) {
        if (lbl_80423870[cur].key < w[3]) {
            lbl_80423870[idx].prev = lbl_80423870[cur].prev;
            if (lbl_80423870[idx].prev == -1)
                lbl_804D4598 = idx;
            lbl_80423870[cur].prev = idx;
            lbl_80423870[idx].next = cur;
            return idx;
        }
        prev = cur;
        cur = lbl_80423870[cur].next;
        if (cur == -1)
            break;
    }
    lbl_80423870[prev].next = idx;
    lbl_80423870[idx].prev = prev;
    lbl_80423870[idx].next = -1;
    return idx;
}

// STUB (not drafted yet, 0xB1C bytes at 0x8007BAAC): advances one flicker pattern by dt, returns 0 when finished.
int FlickerEntryRun(double dt, FlickerEntry* e) {
    return 0;
}

struct Particle;
int ParticleStep(double dt, Particle* p, u32 mask);

struct Particle {
    Vec3 pos;               // 0x00
    Vec3 rot;               // 0x0C
    Vec3 vel;               // 0x18
    Vec3 spin;              // 0x24
    void (*custom)(double dt, Particle* p, Vec3* newPos, Vec3* newVel, Vec3* newRot); // 0x3C
};

int fn_8007C5C8(double dt, Particle* p, u32 mask) {
    return ParticleStep(dt / 1000.0f, p, mask);
}

// Integrates a particle for dt; mask bit 1 keeps rotation, 2 keeps velocity, 4 keeps position.
int ParticleStep(double dt, Particle* p, u32 mask) {
    Vec3 pos;
    Vec3 vel;
    Vec3 rot;
    if (p == NULL)
        return 0;
    if ((mask & 2) == 0) {
        vel.x = (float)((double)p->spin.x * dt + (double)p->vel.x);
        vel.y = (float)((double)p->spin.y * dt + (double)p->vel.y);
        vel.z = (float)((double)p->spin.z * dt + (double)p->vel.z);
    } else {
        vel = p->vel;
    }
    if ((mask & 4) == 0) {
        if ((mask & 2) == 0) {
            float half = (float)(dt * 0.5f);
            pos.x = half * (p->vel.x + vel.x) + p->pos.x;
            pos.y = half * (p->vel.y + vel.y) + p->pos.y;
            pos.z = half * (p->vel.z + vel.z) + p->pos.z;
        } else {
            pos.x = (float)((double)p->vel.x * dt + (double)p->pos.x);
            pos.y = (float)((double)p->vel.y * dt + (double)p->pos.y);
            pos.z = (float)((double)p->vel.z * dt + (double)p->pos.z);
        }
    } else {
        pos = p->pos;
    }
    if ((mask & 1) == 0) {
        rot.x = (float)((double)((float*)p)[9] * dt + (double)p->rot.x);
        rot.y = (float)((double)((float*)p)[10] * dt + (double)p->rot.y);
        rot.z = (float)((double)((float*)p)[11] * dt + (double)p->rot.z);
        rot.x = (float)fmod((double)(2048.0f + rot.x), 5.5) - 2048.0f;
        rot.y = (float)fmod((double)(2048.0f + rot.y), 5.5) - 2048.0f;
        rot.z = (float)fmod((double)(2048.0f + rot.z), 5.5) - 2048.0f;
    } else {
        rot = p->rot;
    }
    if (p->custom == NULL) {
        p->vel = vel;
        p->pos = pos;
        p->rot = rot;
    } else {
        p->custom(dt, p, &pos, &vel, &rot);
    }
    return 1;
}


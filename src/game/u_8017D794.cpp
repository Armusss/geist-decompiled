// u_8017D794.cpp (0x8017D794-0x8017EC24), split from u_8017D794.
// First piece of the old u_8017D794. Ends before 0x8017EC24: fn_8017EC24.. read their own copy of
// 1.0f (0x804D8274 vs 0x804D8240) and belong to the HydraSpit type (descriptor 0x803A84A8). Owns .data 0x803A83B8
// (0xF0 table), .bss 0x80494A80/0x80494B00, .sdata 0x804D3010, .sbss 0x804D50A8-0x804D50C8, .sdata2 0x804D8228-0x804D8270.
// Electric beams between two model bones: 32 slots, a 128-entry noise table that drifts over time, and the
// message handler 0x0E (create 0x7700, enable 0x7701, destroy 0x7801, tune 0x41D).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include "game/types.h"

float __fabsf(float x);

// One draw pass of a beam (0x0C bytes).
struct BeamPass {
    int texture;          // 0x00
    float width;          // 0x04
    u8 r, g, b, a;        // 0x08
};

// Beam style (0x50 bytes), copied from the style table.
struct BeamDef {
    BeamPass pass[5];     // 0x00
    int passes;           // 0x3C
    float amplitude;      // 0x40
    float density;        // 0x44
    float jitter;         // 0x48
    int step;             // 0x4C
};

struct Beam {
    BeamDef def;          // 0x00
    Placement xf;         // 0x50
    int enabled;          // 0x80
    int segments;         // 0x84
    float segLength;      // 0x88
    int id;               // 0x8C
    int layer;            // 0x90
};

// Beam object allocated per slot (0x114 bytes).
struct BeamObj {
    u8 pad00[0x38];
    u32 handle;           // 0x38
    u8 pad3C[0x14];
    Beam beam;            // 0x50
    int modelA;           // 0xE4
    int modelB;           // 0xE8
    int boneA;            // 0xEC
    int boneB;            // 0xF0
    u8* nodesA;           // 0xF4
    u8* nodesB;           // 0xF8
    Vec3 start;           // 0xFC
    Vec3 end;             // 0x108
};

// Message 0x7700: create a beam.
struct BeamCreateMsg {
    BeamObj* result;      // 0x00
    int fromObj;          // 0x04
    u32 fromBone;         // 0x08
    int toObj;            // 0x0C
    u32 toBone;           // 0x10
    int style;            // 0x14
};

// Message 0x41D / 0x7701 payload.
struct BeamCmdMsg {
    int unk00;            // 0x00
    u32 cmd;              // 0x04
    int unk08;            // 0x08
    int arg;              // 0x0C
};

// Reply of message 0x410 (model query).
struct ModelReply {
    int unk00;            // 0x00
    int model;            // 0x04
    u8* nodes;            // 0x08
    int unk0C;            // 0x0C
};

struct TrailBuffer {
    float* points;        // 0x00
    int count;            // 0x04
    u8 pad08[0x20];
};

void* VAlloc(int size);
void VFree(void* p);
int BitsetFirstSet(unsigned long* bits, int value, int count);
void BitClear(unsigned long* bits, int i);
void BitsetClear(void* bits, int count);
void LinkInfoInit(void* p);
int AssetFind(u32 group, u32 name);
void DeferAdd(int layer, Vec3* pos, void (*fn)(void*), void* arg);
void* ScratchAAlloc(int size);
void TrailBufferInit(TrailBuffer* tb, float* points, unsigned long count, unsigned long a, unsigned long b,
                     unsigned long c, unsigned long d, unsigned long e, unsigned char r, unsigned char g,
                     unsigned char bl, unsigned char al);
void Draw3DBeginLine(int texture, void* xf, float width);
void DrawTrailRing(TrailBuffer* tb, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void DrawDisableTexOffsets();
void Mtx34StoreTranslation(Mtx34* m, const Vec3* pos);
unsigned char* ModelAnimate(int model, char* bones, int a);
float fn_80017084(float x);

extern "C" {
int SendMsg(int to, int msg, void* data);
int rand();
float fn_800A1394(float x);
void fn_800A02C0(void* m, Vec3* v, int axis);
int fn_800388A4(int model, int node);
void fn_80026660(int id, int (*fn)(u8, int, void*), int arg);
}

extern "C" {

BeamDef lbl_803A83B8[3] = {
    {{{-3, 8.0f, 0xC8, 0xE1, 0xFF, 0x7F}, {-4, 2.0f, 0x64, 0x64, 0x64, 0xFF}}, 2, 1.0f, 0.04f, 1000.0f, 1},
    {{{-5, 24.0f, 0x0A, 0x0A, 0x0A, 0xFF},
      {-5, 12.0f, 0x0C, 0x0C, 0x0C, 0xFF},
      {-3, 12.0f, 0x00, 0x1D, 0x40, 0x64},
      {-4, 8.0f, 0xE1, 0xC8, 0xFF, 0x64},
      {-4, 2.0f, 0xE1, 0xC8, 0xFF, 0x9B}},
     5, 0.5f, 0.04f, 1000.0f, 1},
    {{{-4, 20.0f, 0x79, 0x99, 0xFF, 0x10},
      {-4, 14.0f, 0xAF, 0xC2, 0xFF, 0x10},
      {-4, 8.0f, 0xAF, 0xC2, 0xFF, 0x50},
      {-4, 2.0f, 0xFF, 0xFF, 0xFF, 0xFF}},
     4, 1.0f, 0.05f, 1000.0f, 1},
};

BeamObj* lbl_80494A80[32];
Vec3 lbl_80494B00[128];
Vec3 lbl_80495100[128];
int lbl_804D3010 = 1;
unsigned long lbl_804D50A8[1];
unsigned long lbl_804D50AC[1];
int lbl_804D50B0;
int lbl_804D50B4;

inline void BitSet(unsigned long* bits, int i) {
    bits[i / 32] |= 1 << (i & 31);
}

inline int BitTest(unsigned long* bits, int i) {
    return (1 << (i & 31)) & bits[i / 32];
}

inline int QueryModel(int obj, int* model, u8** nodes) {
    ModelReply r;

    if (SendMsg(obj, 0x410, &r) != 0) {
        *model = r.model;
        *nodes = r.nodes;
        return 1;
    }
    return 0;
}

inline void BeamInit(Beam* b, int style) {
    if (b != 0) {
        b->def = lbl_803A83B8[style];
        LinkInfoInit(&b->xf);
        b->enabled = 0;
        b->segments = 0;
        b->segLength = 0.0f;
        b->id = lbl_804D50B4++;
        b->layer = 1;
        lbl_804D50B0 = 1;
    }
}

inline int BeamTune(BeamObj* obj, BeamCmdMsg* m) {
    switch (m->cmd) {
    case 0x82A0A6E8:
        if (m->arg >= 0) {
            obj->beam.def.amplitude = m->arg / 100.0f;
            return 1;
        }
        break;
    case 0x875820C8:
        if (m->arg >= 0) {
            obj->beam.def.density = m->arg / 100.0f;
            return 1;
        }
        break;
    case 0xE0E8CE40:
        if (m->arg >= 0) {
            obj->beam.def.jitter = m->arg;
            return 1;
        }
        break;
    case 0xD004D547:
        if (m->arg > 0) {
            obj->beam.def.step = m->arg;
            return 1;
        }
        break;
    }
    return 0;
}

int fn_8017D794(u8 slot, int msg, void* data) {
    switch (msg) {
    case 0x7700: {
        BeamCreateMsg* m = (BeamCreateMsg*)data;
        int i;
        BeamObj* obj;

        i = BitsetFirstSet(lbl_804D50A8, 1, 32);
        BitClear(lbl_804D50A8, i);
        BitSet(lbl_804D50AC, i);
        if (i < 0) {
            m->result = 0;
            return 1;
        }
        obj = (BeamObj*)VAlloc(0x114);
        if (obj == 0) {
            m->result = 0;
            return 1;
        }
        m->result = obj;
        obj->handle = (((lbl_804D3010++ & 0xFFFF) << 8 | i) & 0xFFFFFF) | 0x0E000000;
        BeamInit(&obj->beam, m->style);
        obj->beam.enabled = 1;
        if (QueryModel(m->fromObj, &obj->modelA, &obj->nodesA) == 0 || obj->nodesA == 0) {
            return 0;
        }
        if (QueryModel(m->toObj, &obj->modelB, &obj->nodesB) == 0 || obj->nodesB == 0) {
            return 0;
        }
        obj->boneA = AssetFind(0xF1754BF2, m->fromBone);
        obj->boneB = AssetFind(0xF1754BF2, m->toBone);
        lbl_80494A80[i] = obj;
        return 1;
    }
    case 0x41D:
        if (BeamTune(lbl_80494A80[slot], (BeamCmdMsg*)data)) {
            return 1;
        }
        break;
    case 0x7701: {
        BeamObj* obj = lbl_80494A80[slot];

        if (obj != 0) {
            if (((BeamCmdMsg*)data)->cmd != 0) {
                obj->beam.enabled = 1;
            } else {
                obj->beam.enabled = 0;
            }
        }
        return 1;
    }
    case 0x7801:
        if (lbl_80494A80[slot] != 0) {
            VFree(lbl_80494A80[slot]);
        }
        lbl_80494A80[slot] = 0;
        BitSet(lbl_804D50A8, slot);
        BitClear(lbl_804D50AC, slot);
        return 1;
    }
    return 0;
}

void fn_8017DCBC(void* p);

inline void VecScale(Vec3* out, Vec3* v, float k) {
    out->x = v->x * k;
    out->y = v->y * k;
    out->z = v->z * k;
}

inline void VecAdd(Vec3* out, Vec3* a, Vec3* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

inline void VecLerp(Vec3* out, Vec3* a, Vec3* b, float t) {
    float u = 1.0f - t;
    out->x = a->x * u + b->x * t;
    out->y = a->y * u + b->y * t;
    out->z = a->z * u + b->z * t;
}

void fn_8017DC64(Beam* b) {
    if (b != 0 && b->enabled != 0 && b->segments > 0) {
        DeferAdd(b->layer, &((PlaceF*)&b->xf)->pos, fn_8017DCBC, b);
        lbl_804D50B0 = 1;
    }
}

// Draws one beam: a jagged line along the local Y axis, then one ring pass per style pass.
void fn_8017DCBC(void* p) {
    Beam* b = (Beam*)p;
    int n;
    float amp;
    float len;
    float jitter;
    float arc;
    Vec3* pt;
    Vec3* pts;
    int i;
    int k;
    int step;
    BeamPass* pass;
    TrailBuffer tb;

    n = b->segments;
    arc = 180.0f / n;
    k = (b->id * 11) & 0x7F;
    jitter = 3.0f * b->def.jitter;
    amp = b->def.amplitude;
    len = b->segLength;
    step = b->def.step;
    pts = (Vec3*)ScratchAAlloc(n * 12);
    if (pts == 0) {
        return;
    }
    pt = pts;
    for (i = 0; i < n; i++) {
        Vec3 a;
        Vec3 t;
        Vec3 c;
        float w;

        a.x = 0.0f;
        a.y = len * i;
        a.z = 0.0f;
        VecScale(&t, &lbl_80494B00[k], jitter);
        VecAdd(&c, &t, &a);
        w = amp * fn_800A1394(arc * i);
        k = (k + step) & 0x7F;
        VecLerp(pt, &a, &c, w);
        pt++;
    }
    TrailBufferInit(&tb, (float*)pts, n, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    tb.count = n;
    pass = b->def.pass;
    for (i = 0; i < b->def.passes; i++) {
        Draw3DBeginLine(pass->texture, &b->xf, pass->width);
        DrawTrailRing(&tb, pass->r, pass->g, pass->b, pass->a);
        DrawDisableTexOffsets();
        pass++;
    }
}

// C stand-in for the shared VecNormalize block (include/game/vecnorm.h), which may only be used once the
// whole unit matches: retail does the length math with ps_rsqrte/ps_res here.
inline float VecNormalizeC(Vec3* dst, const Vec3* src) {
    float len;
    float az = __fabsf(src->z);
    float ay = __fabsf(src->y);
    float ax = __fabsf(src->x);
    if ((ax > 1.1920929e-7f) + (ay > 1.1920929e-7f) + (az > 1.1920929e-7f) == 0) {
        dst->x = 0.0f;
        dst->y = 0.0f;
        dst->z = 0.0f;
        len = 0.0f;
    } else {
        float x = src->x;
        float y = src->y;
        float z = src->z;
        float r = __frsqrte(x * x + y * y + z * z);
        dst->x = x * r;
        dst->y = y * r;
        dst->z = z * r;
        len = r;
    }
    return len;
}

inline void Cross(Vec3* out, Vec3* a, Vec3* b) {
    float x = a->y * b->z - a->z * b->y;
    float y = a->z * b->x - a->x * b->z;
    float z = a->x * b->y - a->y * b->x;
    out->x = x;
    out->y = y;
    out->z = z;
}

// Aims the beam from `from` to `to` and splits it into segments.
void fn_8017DF34(Beam* b, Vec3* from, Vec3* to, float dt) {
    Vec3 dir;
    Vec3 side;
    Vec3 up;
    float len;

    if (b != 0 && from != 0 && to != 0 && b->enabled != 0) {
        dir.x = to->x - from->x;
        dir.y = to->y - from->y;
        dir.z = to->z - from->z;
        len = VecNormalizeC(&dir, &dir);
        b->segments = len * b->def.density;
        if (b->segments <= 1) {
            b->segments = 0;
            return;
        }
        b->segLength = len / (b->segments - 1);
        up.x = 0.0f;
        up.y = 0.0f;
        up.z = 1.0f;
        Cross(&side, &dir, &up);
        Cross(&up, &side, &dir);
        fn_800A02C0(&b->xf, &side, 0);
        fn_800A02C0(&b->xf, &dir, 1);
        fn_800A02C0(&b->xf, &up, 2);
        Mtx34StoreTranslation((Mtx34*)&b->xf, from);
    }
}

void fn_8017E15C(Beam* b, int on) {
    if (b != 0) {
        b->enabled = on;
        lbl_804D50B0 = 1;
    }
}

void fn_8017E174(Beam* b, int style) {
    BeamInit(b, style);
}

void fn_8017EAB0(BeamObj* obj, float dt);

inline void VecMix(Vec3* out, Vec3* a, float ka, Vec3* b, float kb) {
    out->x = a->x * ka + b->x * kb;
    out->y = a->y * ka + b->y * kb;
    out->z = a->z * ka + b->z * kb;
}

inline void NoiseRandomize(Vec3* v) {
    int i;

    for (i = 0; i < 128; i++) {
        v->x = 2.0f * (rand() / 32767.0f) - 1.0f;
        v->y = 0.2f * (2.0f * (rand() / 32767.0f) - 1.0f);
        v->z = 2.0f * (rand() / 32767.0f) - 1.0f;
        v++;
    }
}

// Advances all beams and lets the noise table drift toward a new random target in 16 ms steps.
void fn_8017E288(float dt) {
    static u32 sTime = 0;
    static int sPending = 0;
    Vec3* base;

    if (lbl_804D50B0 != 0) {
        int t = dt;
        int k;

        lbl_804D50B0 = 0;
        sTime += t;
        sPending += t;
        for (k = 0; k < 32; k++) {
            if (BitTest(lbl_804D50AC, k)) {
                fn_8017EAB0(lbl_80494A80[k], dt);
            }
        }
        base = lbl_80495100;
        NoiseRandomize(base);
        while (sPending > 16) {
            float r;
            float s;
            Vec3* cur;
            Vec3* tgt;
            int j;

            sPending -= 16;
            s = 0.25f * (0.5f * fn_80017084((float)sTime + (float)sPending) + 0.5f);
            r = 1.0f - s;
            tgt = base;
            cur = lbl_80494B00;
            for (j = 0; j < 128; j++) {
                Vec3* prev = &lbl_80494B00[(j - 1) & 0x7F];
                Vec3* next = &lbl_80494B00[(j + 1) & 0x7F];
                Vec3 a;
                Vec3 m;

                VecMix(&a, cur, 0.75f, tgt, 0.25f);
                VecMix(&m, prev, 0.5f, next, 0.5f);
                VecMix(cur, &m, r, &a, s);
                tgt++;
                cur++;
            }
        }
    }
}

// Queues the visible beams for drawing.
void fn_8017E5BC() {
    int i;

    for (i = 0; i < 32; i++) {
        if (BitTest(lbl_804D50AC, i)) {
            if (fn_800388A4(lbl_80494A80[i]->modelA, lbl_80494A80[i]->boneA) != 0 ||
                fn_800388A4(lbl_80494A80[i]->modelB, lbl_80494A80[i]->boneB) != 0) {
                fn_8017DC64(&lbl_80494A80[i]->beam);
            }
        }
    }
}

void fn_8017E69C() {
    int i;

    for (i = 0; i < 32; i++) {
        if (lbl_80494A80[i] != 0) {
            VFree(lbl_80494A80[i]);
            lbl_80494A80[i] = 0;
            BitSet(lbl_804D50A8, i);
        }
        BitsetClear(lbl_804D50AC, 32);
    }
}

void fn_8017E728() {
}

void fn_8017E72C() {
    Vec3* tgt;
    int i;

    lbl_804D50B0 = 0;
    lbl_804D50B4 = 0;
    lbl_804D3010 = 0;
    BitsetClear(lbl_804D50A8, 32);
    BitsetClear(lbl_804D50AC, 32);
    tgt = lbl_80495100;
    NoiseRandomize(tgt);
    for (i = 0; i < 128; i++) {
        lbl_80494B00[i] = *tgt;
        tgt++;
    }
    fn_8017E288(48.0f);
    for (i = 0; i < 32; i++) {
        lbl_80494A80[i] = 0;
        BitSet(lbl_804D50A8, i);
    }
    fn_80026660(0xE, fn_8017D794, 0);
}

struct ModelNode {
    u8 pad00[0x10];
    Vec3 pos;             // 0x10
};

// Follows the two bones and re-aims the beam.
void fn_8017EAB0(BeamObj* obj, float dt) {
    if (obj->modelA == obj->modelB) {
        unsigned char* map = ModelAnimate(obj->modelA, 0, 0);

        obj->start = ((ModelNode*)obj->nodesA)[map[obj->boneA]].pos;
        obj->end = ((ModelNode*)obj->nodesB)[map[obj->boneB]].pos;
    } else {
        int a = 0;
        int b = 0;
        unsigned char* map;

        map = ModelAnimate(obj->modelA, 0, 0);
        if (obj->boneA != -1) {
            a = map[obj->boneA];
        }
        obj->start = ((ModelNode*)obj->nodesA)[a].pos;
        map = ModelAnimate(obj->modelB, 0, 0);
        if (obj->boneB != -1) {
            b = map[obj->boneB];
        }
        obj->end = ((ModelNode*)obj->nodesB)[b].pos;
    }
    fn_8017DF34(&obj->beam, &obj->start, &obj->end, dt);
}
}

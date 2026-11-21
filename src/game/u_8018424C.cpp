// u_8018424C.cpp (0x8018424C-0x80184CD4), split from u_8017D794.
// Helper file between two behaviour groups: own double 0x804D8370 and its own 0.0f/1.19e-7 copies;
// .sdata 0x804D3020 is touched only here. sdata2 0x804D8350-0x804D8390.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include "game/types.h"

struct BehDesc;

struct BehSlot {
    u8 pad00[0x44];
    BehDesc* desc;        // 0x44
};

// The host object (only the fields used here).
struct BehHost {
    u8 pad00[0xF4];
    BehSlot* slot;        // 0xF4
};

extern BehDesc lbl_803A8600;

extern "C" int rand();
int AssetFind(u32 group, u32 name);
void TextureLoad(short* ids, int count);

extern "C" {
void fn_800E0E9C(IVec3* out, IVec3* in, int length);
void fn_800E1098(Vec3* out, Vec3* in, float length);
int fn_800E0CB8(IVec3* out, const IVec3* in);
void fn_802285D8(float size, Vec3* pos, Vec3* dir, u8 r, u8 g, u8 b);
void fn_80227DD8(float size, float b, float c, float d, Vec3* pos, Vec3* dir, u8 r, u8 g, u8 bl, s16 tex);

// Spark textures, loaded by fn_801849DC.
s16 lbl_804D3020[4] = {-1};

// Fixed-point (20.12) cross product.
inline void ICross(IVec3* out, IVec3* a, IVec3* b) {
    int x = (a->y * b->z + 0x800 - a->z * b->y) / 4096;
    int y = (a->z * b->x + 0x800 - a->x * b->z) / 4096;
    int z = (a->x * b->y + 0x800 - a->y * b->x) / 4096;
    out->x = x;
    out->y = y;
    out->z = z;
}

void fn_8018424C(IVec3* origin, IVec3* dir, int size, int kind) {
    Vec3 fpos;
    Vec3 fdir;
    IVec3 up;
    IVec3 side;
    IVec3 step;
    int z, y, x;
    int r, g, b;
    int jitter;

    fdir.x = (1.0f / 4096.0f) * dir->x;
    fdir.y = (1.0f / 4096.0f) * dir->y;
    fdir.z = (1.0f / 4096.0f) * dir->z;
    x = rand() % 8192 - 4096;
    y = rand() % 8192 - 4096;
    up.x = x;
    up.y = 0;
    up.z = y;
    ICross(&side, dir, &up);
    ICross(&up, &side, dir);
    fn_800E0E9C(&step, dir, 5);
    x = origin->x + step.x;
    y = origin->y + step.y;
    z = origin->z + step.z;
    fn_800E0E9C(&step, &side, rand() % 250 - 125);
    x += step.x;
    y += step.y;
    z += step.z;
    fn_800E0E9C(&step, &up, rand() % 250 - 125);
    x += step.x;
    y += step.y;
    z += step.z;
    jitter = rand() % 20 - 10;
    fpos.x = x;
    fpos.y = y;
    fpos.z = z;
    switch (kind) {
    case 0:
        r = rand() % 24 + 0xAE;
        g = rand() % 24 + 0xE4;
        b = 0x2D;
        break;
    case 2:
        r = 63.0f + 149.0f * (rand() / 32767.0f);
        g = 111.0f + 120.0f * (rand() / 32767.0f);
        b = 37.0f * (rand() / 32767.0f);
        break;
    case 3:
        r = rand() % 24 + 0x58;
        g = 0x3B;
        b = 0x13;
        break;
    default:
        r = rand() % 24 + 0xB3;
        g = 0;
        b = 0x10;
        break;
    }
    fn_802285D8(size + jitter, &fpos, &fdir, r, g, b);
}

void fn_80184704(IVec3* pos, IVec3* dir, u32 flags, BehHost* host) {
    Vec3 fpos;
    Vec3 fdir;
    Vec3 step;
    int n;
    int r, g, b;
    s16 tex;

    fpos.x = pos->x;
    fpos.y = pos->y;
    fpos.z = pos->z;
    fdir.x = (1.0f / 4096.0f) * dir->x;
    fdir.y = (1.0f / 4096.0f) * dir->y;
    fdir.z = (1.0f / 4096.0f) * dir->z;
    fn_800E1098(&step, &fdir, 5.0f);
    fpos.x += step.x;
    fpos.y += step.y;
    fpos.z += step.z;
    if (host->slot->desc == &lbl_803A8600) {
        n = 0x200;
    } else {
        n = rand() % 8 + 16;
    }
    if (flags & 0x100) {
        return;
    }
    switch (flags & 0xFF) {
    case 0x86:
    case 0x87:
    case 0x88:
        r = g = b = rand() % 16 + 8;
        tex = lbl_804D3020[0];
        break;
    case 0x84:
    case 0x85:
        r = g = b = rand() % 16 + 8;
        tex = lbl_804D3020[1];
        break;
    case 0x8B:
        r = g = b = rand() % 16 + 8;
        tex = lbl_804D3020[2];
        break;
    case 0x89:
        r = g = b = rand() % 16 + 8;
        tex = lbl_804D3020[3];
        break;
    case 0x8A:
        return;
    default:
        r = g = b = rand() % 16 + 8;
        tex = lbl_804D3020[1];
        break;
    }
    fn_80227DD8(n, 100.0f, 0.0f, 0.0f, &fpos, &fdir, r, g, b, tex);
}

void fn_801849DC() {
    lbl_804D3020[0] = AssetFind(0xF2E2BEB8, 0xF78176BA);
    lbl_804D3020[1] = AssetFind(0xF2E2BEB8, 0x362017F7);
    lbl_804D3020[2] = AssetFind(0xF2E2BEB8, 0x6D43D66F);
    lbl_804D3020[3] = AssetFind(0xF2E2BEB8, 0x362017F7);
    TextureLoad(lbl_804D3020, 4);
}

float __fabsf(float x);

// C stand-in for the shared VecNormalize block (include/game/vecnorm.h), which may only be used once the
// whole unit matches: retail does the length math with ps_rsqrte/ps_res here.
inline void VecNormalizeC(Vec3* dst, const Vec3* src) {
    float az = __fabsf(src->z);
    float ay = __fabsf(src->y);
    float ax = __fabsf(src->x);
    if ((ax > 1.1920929e-7f) + (ay > 1.1920929e-7f) + (az > 1.1920929e-7f) == 0) {
        dst->x = 0.0f;
        dst->y = 0.0f;
        dst->z = 0.0f;
    } else {
        float x = src->x;
        float y = src->y;
        float z = src->z;
        float r = __frsqrte(x * x + y * y + z * z);
        dst->x = x * r;
        dst->y = y * r;
        dst->z = z * r;
    }
}

inline float VecDot(Vec3* a, Vec3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

inline int IDot(IVec3* a, IVec3* b) {
    return a->x * b->x / 4096 + a->y * b->y / 4096 + a->z * b->z / 4096;
}

// Reflects the normalized direction off a surface with the given normal when it points into it.
void fn_80184A70(Vec3* dir, Vec3* normal, Vec3* out) {
    Vec3 n;
    Vec3 v;
    Vec3 t;
    float d;

    v = *dir;
    VecNormalizeC(&n, &v);
    d = VecDot(normal, &n);
    if (d < 0.0f) {
        t.x = d * normal->x / 0.5f;
        t.y = d * normal->y / 0.5f;
        t.z = d * normal->z / 0.5f;
        out->x = -(t.x - n.x);
        out->y = -(t.y - n.y);
        out->z = -(t.z - n.z);
    } else {
        out->x = n.x;
        out->y = n.y;
        out->z = n.z;
    }
}

// Fixed-point version of fn_80184A70.
void fn_80184BCC(IVec3* dir, IVec3* normal, IVec3* out) {
    IVec3 n;
    IVec3 v;
    IVec3 t;
    int d;

    v = *dir;
    fn_800E0CB8(&n, &v);
    d = IDot(normal, &n);
    if (d < 0) {
        t.x = d * normal->x / 2048;
        t.y = d * normal->y / 2048;
        t.z = d * normal->z / 2048;
        out->x = -(t.x - n.x);
        out->y = -(t.y - n.y);
        out->z = -(t.z - n.z);
    } else {
        out->x = n.x;
        out->y = n.y;
        out->z = n.z;
    }
}
}

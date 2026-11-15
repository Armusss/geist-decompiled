// Vector length / normalize helpers (0x800E0CB8-0x800E1144), for integer and float 3-vectors.
// The square root uses paired-single estimate instructions (ps_rsqrte, then ps_res).
// Integer vectors are normalized to a length of 4096 (fixed point).
// Names are descriptive guesses; the code is matched against the retail DOL.

struct IVec3 {
    int x, y, z;
};

struct Vec3 {
    float x, y, z;
};

inline float FastSqrt(float x) {
    float r;
    if (x <= 0.0f) {
        r = 0.0f;
    } else {
        float y;
        register float t;
        asm {
            lfs t, x
            ps_rsqrte t, t
            ps_res t, t
            stfs t, y
        }
        r = y;
    }
    return r;
}

extern "C" int fn_800E0CB8(IVec3* out, const IVec3* in) {
    float x, y, z;
    float sq;
    float len;
    float scale;

    if ((in->x | in->y | in->z) == 0) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return 0;
    }
    x = in->x;
    y = in->y;
    z = in->z;
    sq = x * x + y * y + z * z;
    len = FastSqrt(sq);
    scale = 4096.0f / len;
    x *= scale;
    y *= scale;
    z *= scale;
    out->x = x;
    out->y = y;
    out->z = z;
    return len;
}

extern "C" int fn_800E0DBC(const IVec3* v) {
    if ((v->x | v->y | v->z) == 0) {
        return 0;
    }
    float sq = (float)v->x * (float)v->x + (float)v->y * (float)v->y + (float)v->z * (float)v->z;
    return FastSqrt(sq);
}

extern "C" void fn_800E0E9C(IVec3* out, const IVec3* in, int length) {
    if ((in->x | in->y | in->z) == 0) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return;
    }
    float sq = (float)in->x * (float)in->x + (float)in->y * (float)in->y + (float)in->z * (float)in->z;
    float scale = (float)length / FastSqrt(sq);
    out->x = (float)in->x * scale;
    out->y = (float)in->y * scale;
    out->z = (float)in->z * scale;
}

extern "C" float fn_800E1020(const Vec3* v) {
    if (((*(const unsigned long*)&v->x | *(const unsigned long*)&v->y | *(const unsigned long*)&v->z) & 0x7FFFFFFF) == 0) {
        return 0.0f;
    }
    float sq = v->x * v->x + v->y * v->y + v->z * v->z;
    return FastSqrt(sq);
}

extern "C" void fn_800E1098(Vec3* out, const Vec3* in, float length) {
    if (((*(const unsigned long*)&in->x | *(const unsigned long*)&in->y | *(const unsigned long*)&in->z) & 0x7FFFFFFF) == 0) {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
        return;
    }
    float sq = in->x * in->x + in->y * in->y + in->z * in->z;
    float scale = length / FastSqrt(sq);
    out->x = in->x * scale;
    out->y = in->y * scale;
    out->z = in->z * scale;
}

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
    float r = 0.0f;
    if (!(x <= 0.0f)) {
        float y;
        asm {
            lfs f0, x
            ps_rsqrte f0, f0
            ps_res f0, f0
            stfs f0, y
        }
        r = y;
    }
    return r;
}

int IVecNormalize(IVec3* out, IVec3* in) {
    float x, y, z;
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
    len = FastSqrt(x * x + y * y + z * z);
    scale = 4096.0f / len;
    x *= scale;
    y *= scale;
    z *= scale;
    out->x = x;
    out->y = y;
    out->z = z;
    return len;
}

int IVecLength(IVec3* v) {
    if ((v->x | v->y | v->z) == 0) {
        return 0;
    }
    return FastSqrt((float)v->x * (float)v->x + (float)v->y * (float)v->y + (float)v->z * (float)v->z);
}

void IVecSetLength(IVec3* out, IVec3* in, int length) {
    float len;
    float scale;

    if ((in->x | in->y | in->z) == 0) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return;
    }
    len = FastSqrt((float)in->x * (float)in->x + (float)in->y * (float)in->y + (float)in->z * (float)in->z);
    scale = (float)length / len;
    out->x = (float)in->x * scale;
    out->y = (float)in->y * scale;
    out->z = (float)in->z * scale;
}

float VecLength(Vec3* v) {
    if (((*(unsigned long*)&v->x | *(unsigned long*)&v->y | *(unsigned long*)&v->z) & 0x7FFFFFFF) == 0) {
        return 0.0f;
    }
    return FastSqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

void VecSetLength(Vec3* out, Vec3* in, float length) {
    float scale;

    if (((*(unsigned long*)&in->x | *(unsigned long*)&in->y | *(unsigned long*)&in->z) & 0x7FFFFFFF) == 0) {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
        return;
    }
    scale = length / FastSqrt(in->x * in->x + in->y * in->y + in->z * in->z);
    out->x = in->x * scale;
    out->y = in->y * scale;
    out->z = in->z * scale;
}

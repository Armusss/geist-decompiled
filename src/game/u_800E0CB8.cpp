// Vector length / normalize helpers (0x800E0CB8-0x800E1144), for integer and float 3-vectors.
// The square root uses paired-single estimate instructions (ps_rsqrte, then ps_res).
// Names are descriptive guesses; the code is matched against the retail DOL.

struct IVec3 {
    int x, y, z;
};

struct Vec3 {
    float x, y, z;
};

inline float FastSqrt(register float x) {
    float r = 0.0f;
    if (x > 0.0f) {
        register float y;
        asm {
            ps_rsqrte y, x
            ps_res y, y
        }
        r = y;
    }
    return r;
}

float VecLength(Vec3* v) {
    if (((*(unsigned long*)&v->x | *(unsigned long*)&v->y | *(unsigned long*)&v->z) & 0x7FFFFFFF) == 0) {
        return 0.0f;
    }
    return FastSqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

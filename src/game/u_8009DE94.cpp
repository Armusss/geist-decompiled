// u_8009DE94.cpp: matrix/euler helpers and the AI agent ("NeoAgent") table code (0x8009DE94-0x800A6F18).
// Names are descriptive guesses; the code is matched against the retail DOL.

struct Mtx34 { float m[12]; };
struct Vec3 { float x, y, z; };
struct PlaceF;
struct Bone;
struct Actor;
struct Owner;

// Fixed-point (Q12) 3x4 matrix stored at the head of an Actor.
struct ActorMtx { int m[12]; };

extern "C" void fn_800E0CB8(int* out, int* in = 0);

extern const float lbl_804D6AA0;  // Q12 fixed-point -> float scale
extern const float lbl_804D6AA4;  // float -> Q12 fixed-point scale (4096.0f)
extern const float lbl_804D6AB0;  // epsilon
extern const float lbl_804D6AB4;  // 0.0f
extern const float lbl_804D6AB8;  // 1.0f
extern Vec3 lbl_80398628[3];  // default orthonormal right/up/forward basis
extern int lbl_80449038[12];  // shared global Q12 3x4 transform

extern "C" float fabsf(float);
extern "C" float sqrtf(float);

// 3x4 affine matrix inverse; the source row-vector basis is copied to a local
// first when dst and src alias.
void MtxInvert(Mtx34* dst, Mtx34* src)
{
    Mtx34 local;
    Mtx34* p;
    float det;
    float c;

    p = src;
    if (dst == src) {
        local = *src;
        p = &local;
    }
    c = p->m[4] * p->m[8] - p->m[5] * p->m[7];
    det = p->m[2] * (p->m[3] * p->m[7] - p->m[4] * p->m[6]) +
          (p->m[0] * c - p->m[1] * (p->m[3] * p->m[8] - p->m[5] * p->m[6]));
    if (lbl_804D6AB0 <= fabsf(det)) {
        dst->m[0] = c / det;
        dst->m[1] = -(p->m[1] * p->m[8] - p->m[7] * p->m[2]) / det;
        dst->m[2] = (p->m[1] * p->m[5] - p->m[4] * p->m[2]) / det;
        dst->m[3] = -(p->m[3] * p->m[8] - p->m[5] * p->m[6]) / det;
        dst->m[4] = (p->m[0] * p->m[8] - p->m[2] * p->m[6]) / det;
        dst->m[5] = -(p->m[0] * p->m[5] - p->m[2] * p->m[3]) / det;
        dst->m[6] = (p->m[3] * p->m[7] - p->m[4] * p->m[6]) / det;
        dst->m[7] = -(p->m[0] * p->m[7] - p->m[1] * p->m[6]) / det;
        dst->m[8] = (p->m[0] * p->m[4] - p->m[1] * p->m[3]) / det;
        dst->m[9] = -(p->m[11] * dst->m[2] + p->m[9] * dst->m[0] +
                      p->m[10] * dst->m[1]);
        dst->m[10] = -(p->m[11] * dst->m[5] + p->m[9] * dst->m[3] +
                       p->m[10] * dst->m[4]);
        dst->m[11] = -(p->m[11] * dst->m[8] + p->m[9] * dst->m[6] +
                       p->m[10] * dst->m[7]);
    } else {
        *dst = *src;
    }
}

static inline void NormalizeVec3(Vec3* v)
{
    if (fabsf(v->x) <= lbl_804D6AB0 && fabsf(v->y) <= lbl_804D6AB0 &&
        fabsf(v->z) <= lbl_804D6AB0) {
        v->x = lbl_804D6AB4;
        v->y = lbl_804D6AB4;
        v->z = lbl_804D6AB4;
    } else {
        float s = 1.0f / sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
        v->x = v->x * s;
        v->y = v->y * s;
        v->z = v->z * s;
    }
}

// Build an orthonormal right/up/forward basis from any subset of the three
// input vectors; missing axes come from the default global basis.
void BasisFromVecs(float* out, Vec3* right, Vec3* up, Vec3* fwd)
{
    Vec3 a;
    Vec3 b;
    Vec3 c;

    if (up == 0) {
        if (fwd == 0) {
            if (right == 0) {
                a = lbl_80398628[0];
                b = lbl_80398628[1];
                c = lbl_80398628[2];
            } else {
                a = *right;
                NormalizeVec3(&a);
                if (fabsf(a.x * lbl_80398628[2].x + a.y * lbl_80398628[2].y +
                          a.z * lbl_80398628[2].z) <= lbl_804D6AB8) {
                    b.x = -a.y;
                    b.y = a.x;
                    b.z = lbl_804D6AB4;
                } else {
                    b = lbl_80398628[1];
                }
                NormalizeVec3(&b);
                c.x = a.y * b.z - a.z * b.y;
                c.y = a.z * b.x - a.x * b.z;
                c.z = a.x * b.y - a.y * b.x;
                b.x = c.y * a.z - c.z * a.y;
                b.y = c.z * a.x - c.x * a.z;
                b.z = c.x * a.y - c.y * a.x;
            }
        } else {
            c = *fwd;
            NormalizeVec3(&c);
            if (right == 0) {
                if (fabsf(c.x * lbl_80398628[2].x + c.y * lbl_80398628[2].y +
                          c.z * lbl_80398628[2].z) <= lbl_804D6AB8) {
                    a.x = c.y;
                    a.y = -c.x;
                    a.z = lbl_804D6AB4;
                } else {
                    a = lbl_80398628[0];
                }
                NormalizeVec3(&a);
            } else {
                a = *right;
                NormalizeVec3(&a);
            }
            b.x = c.y * a.z - c.z * a.y;
            b.y = c.z * a.x - c.x * a.z;
            b.z = c.x * a.y - c.y * a.x;
            a.x = b.y * c.z - b.z * c.y;
            a.y = b.z * c.x - b.x * c.z;
            a.z = b.x * c.y - b.y * c.x;
        }
    } else {
        b = *up;
        NormalizeVec3(&b);
        if (fwd == 0) {
            if (right == 0) {
                if (fabsf(b.x * lbl_80398628[2].x + b.y * lbl_80398628[2].y +
                          b.z * lbl_80398628[2].z) <= lbl_804D6AB8) {
                    a.x = b.y;
                    a.y = -b.x;
                    a.z = lbl_804D6AB4;
                } else {
                    a = lbl_80398628[0];
                }
                NormalizeVec3(&a);
            } else {
                a = *right;
                NormalizeVec3(&a);
            }
            c.x = a.y * b.z - a.z * b.y;
            c.y = a.z * b.x - a.x * b.z;
            c.z = a.x * b.y - a.y * b.x;
            a.x = b.y * c.z - b.z * c.y;
            a.y = b.z * c.x - b.x * c.z;
            a.z = b.x * c.y - b.y * c.x;
        } else {
            c = *fwd;
            NormalizeVec3(&c);
            if (right == 0) {
                if (fabsf(c.x * lbl_80398628[2].x + c.y * lbl_80398628[2].y +
                          c.z * lbl_80398628[2].z) <= lbl_804D6AB8) {
                    a.x = c.y;
                    a.y = -c.x;
                    a.z = lbl_804D6AB4;
                } else {
                    a = lbl_80398628[0];
                }
                NormalizeVec3(&a);
            } else {
                a = *right;
                NormalizeVec3(&a);
            }
            b.x = c.y * a.z - c.z * a.y;
            b.y = c.z * a.x - c.x * a.z;
            b.z = c.x * a.y - c.y * a.x;
            a.x = b.y * c.z - b.z * c.y;
            a.y = b.z * c.x - b.x * c.z;
            a.z = b.x * c.y - b.y * c.x;
            c.x = a.y * b.z - a.z * b.y;
            c.y = a.z * b.x - a.x * b.z;
            c.z = a.x * b.y - a.y * b.x;
        }
    }
    out[0] = a.x;
    out[1] = a.y;
    out[2] = a.z;
    out[3] = b.x;
    out[4] = b.y;
    out[5] = b.z;
    out[6] = c.x;
    out[7] = c.y;
    out[8] = c.z;
}

// Fill a 9-word Q12 basis from one input axis: the axis itself in words 3..5,
// a perpendicular axis in words 0..2 and their cross product in words 6..8.
// fn_8009EC5C
extern "C" void fn_8009EC5C(int* out)
{
    int a[3];
    int b[3];

    fn_800E0CB8(a);
    out[3] = a[0];
    out[4] = a[1];
    out[5] = a[2];
    if (a[1] == 0 && a[0] == 0) {
        b[0] = 0x1000;
        b[1] = 0;
        b[2] = 0;
    } else {
        b[0] = a[1];
        b[1] = -a[0];
        b[2] = 0;
        fn_800E0CB8(b, b);
    }
    out[0] = b[0];
    out[1] = b[1];
    out[2] = b[2];
    out[6] = (b[1] * a[2] + 0x800 - b[2] * a[1]) / 0x1000;
    out[7] = (b[2] * a[0] + 0x800 - b[0] * a[2]) / 0x1000;
    out[8] = (b[0] * a[1] + 0x800 - b[1] * a[0]) / 0x1000;
}

// Convert an actor's fixed-point 3x4 matrix to floating point. The rotation
// part is scaled out of Q12; the translation row is already integral.
void ActorGetMatrix(Mtx34* mtx, Actor* actor)
{
    ActorMtx* src;
    int i;

    src = (ActorMtx*)actor;
    for (i = 0; i < 9; i++)
        mtx->m[i] = (float)src->m[i] * lbl_804D6AA0;
    for (; i < 12; i++)
        mtx->m[i] = (float)src->m[i];
}

// Convert a placement's floating-point 3x4 transform into the Q12 fixed-point
// form used by actors: the rotation part is scaled by 4096, the translation row
// is already integral and copied through.
void PlacementToMatrix(Mtx34* mtx, PlaceF* place)
{
    int* dst;
    float* src;
    int i;

    dst = (int*)mtx;
    src = (float*)place;
    for (i = 0; i < 9; i++)
        dst[i] = (int)(lbl_804D6AA4 * src[i]);
    for (; i < 12; i++)
        dst[i] = (int)src[i];
}

// Q12 sine/cosine lookups of the Euler angle unit.
// fn_800A1688
extern "C" int fn_800A1688(int angle);
// fn_800A1624
extern "C" int fn_800A1624(int angle);

// Build a Q12 3x3 rotation matrix from the three Euler angles (x, y, z), all
// in the fixed-point angle unit. The result is nine words of Q12.
// fn_8009F028
extern "C" void fn_8009F028(int* out, int* angles)
{
    int cosZ, sinZ, cosX, sinX, cosY, sinY;
    int t1, t2, t3, t4;

    cosZ = fn_800A1688(angles[2]);
    sinZ = fn_800A1624(angles[2]);
    cosX = fn_800A1688(angles[0]);
    sinX = fn_800A1624(angles[0]);
    cosY = fn_800A1688(angles[1]);
    sinY = fn_800A1624(angles[1]);
    fn_800A1688(0);
    fn_800A1624(0);

    t1 = sinZ * sinY / 0x1000;
    t2 = cosZ * sinY / 0x1000;
    t3 = sinZ * cosY / 0x1000;
    t4 = cosZ * cosY / 0x1000;

    out[0] = (t4 * 0x1000 - t1 * sinX) / 0x1000;
    out[1] = (t3 * 0x1000 + t2 * sinX) / 0x1000;
    out[2] = -cosX * sinY / 0x1000;
    out[3] = -sinZ * cosX / 0x1000;
    out[4] = cosZ * cosX / 0x1000;
    out[5] = sinX;
    out[6] = (t2 * 0x1000 + t3 * sinX) / 0x1000;
    out[7] = (t1 * 0x1000 - t4 * sinX) / 0x1000;
    out[8] = cosX * cosY / 0x1000;
}

// fn_800A1558
extern "C" int fn_800A1558(int y, int x);
extern "C" int fn_800A1494(int value);

// Extract the three Euler angles (Q12 angle units) from a Q12 3x3 rotation
// matrix stored row-major in the actor's fixed-point matrix layout. The middle
// column of the second row is the sine of the x angle; the two gimbal cases
// pin x to +/-90 degrees and derive y from the first row instead.
// fn_8009F190
extern "C" void fn_8009F190(int* angles, int* mtx)
{
    if (mtx[5] < 0x1000) {
        if (mtx[5] < -0xFFF) {
            angles[2] = 0;
            angles[0] = -0x400;
            angles[1] = fn_800A1558(-mtx[1], mtx[0]);
        } else {
            angles[2] = fn_800A1558(-mtx[3], mtx[4]);
            angles[0] = fn_800A1494(mtx[5]);
            angles[1] = fn_800A1558(-mtx[2], mtx[8]);
        }
    } else {
        angles[2] = 0;
        angles[0] = 0x400;
        angles[1] = fn_800A1558(-mtx[1], mtx[0]);
    }
}

// Multiply two Q12 3x3 rotation matrices (row-major) with rounded division by
// 4096. The destination's translation words are read first and written back
// unchanged, so the rotation part can be rebuilt in place.
void MtxMulQ12(int* out, int* a, int* b)
{
    int a0 = a[0], a1 = a[1], a2 = a[2];
    int a3 = a[3], a4 = a[4], a5 = a[5];
    int a6 = a[6], a7 = a[7], a8 = a[8];
    int b0 = b[0], b1 = b[1], b2 = b[2];
    int b3 = b[3], b4 = b[4], b5 = b[5];
    int b6 = b[6], b7 = b[7], b8 = b[8];
    int m9 = out[9], m10 = out[10], m11 = out[11];

    out[0] = (a0 * b0 + a1 * b3 + a2 * b6 + 0x800) / 0x1000;
    out[1] = (a0 * b1 + a1 * b4 + a2 * b7 + 0x800) / 0x1000;
    out[2] = (a0 * b2 + a1 * b5 + a2 * b8 + 0x800) / 0x1000;
    out[3] = (a3 * b0 + a4 * b3 + a5 * b6 + 0x800) / 0x1000;
    out[4] = (a3 * b1 + a4 * b4 + a5 * b7 + 0x800) / 0x1000;
    out[5] = (a3 * b2 + a4 * b5 + a5 * b8 + 0x800) / 0x1000;
    out[6] = (a6 * b0 + a7 * b3 + a8 * b6 + 0x800) / 0x1000;
    out[7] = (a6 * b1 + a7 * b4 + a8 * b7 + 0x800) / 0x1000;
    out[8] = (a6 * b2 + a7 * b5 + a8 * b8 + 0x800) / 0x1000;
    out[9] = m9;
    out[10] = m10;
    out[11] = m11;
}

// Transform a Q12 fixed-point point by a Q12 fixed-point 3x4 matrix (the
// rotation rows only), rounding each result back to whole units.
// fn_8009F44C
extern "C" void fn_8009F44C(int* out, int* mtx, int* vec)
{
    int x = vec[0];
    int y = vec[1];
    int z = vec[2];

    out[0] = (mtx[0] * x + mtx[1] * y + mtx[2] * z + 0x800) / 0x1000;
    out[1] = (mtx[3] * x + mtx[4] * y + mtx[5] * z + 0x800) / 0x1000;
    out[2] = (mtx[6] * x + mtx[7] * y + mtx[8] * z + 0x800) / 0x1000;
}

// Same as fn_8009F44C, but the matrix is the shared global Q12
// transform, not a caller-supplied one.
// fn_8009F50C
extern "C" void fn_8009F50C(int* out, int* vec)
{
    int x = vec[0];
    int y = vec[1];
    int z = vec[2];
    int* m = lbl_80449038;

    out[0] = (m[0] * x + m[1] * y + m[2] * z + 0x800) / 0x1000;
    out[1] = (m[3] * x + m[4] * y + m[5] * z + 0x800) / 0x1000;
    out[2] = (m[6] * x + m[7] * y + m[8] * z + 0x800) / 0x1000;
}

// Load the shared world transform from a Q12 3x4 matrix. The matrix is stored
// row-vector in the source and column-vector in the shared transform, so the
// 3x3 rotation part is transposed; the three translation words pass through.
// fn_8009F5B0
extern "C" void fn_8009F5B0(int* src)
{
    int* d = lbl_80449038;

    d[0] = src[0];
    d[3] = src[1];
    d[6] = src[2];
    d[1] = src[3];
    d[4] = src[4];
    d[7] = src[5];
    d[2] = src[6];
    d[5] = src[7];
    d[8] = src[8];
    d[9] = src[9];
    d[10] = src[10];
    d[11] = src[11];
}

// Copy the 3x3 Q12 rotation of a 3x4 matrix into a 3x3 destination, transposing
// it. The whole 12-word source is staged in a local so an aliasing caller is
// handled; the destination's trailing words are left alone.
// fn_8009F63C
extern "C" void fn_8009F63C(int* dst, int* src)
{
    int tmp[12];

    tmp[0] = src[0];
    tmp[1] = src[1];
    tmp[2] = src[2];
    tmp[3] = src[3];
    tmp[4] = src[4];
    tmp[5] = src[5];
    tmp[6] = src[6];
    tmp[7] = src[7];
    tmp[8] = src[8];
    tmp[9] = src[9];
    tmp[10] = src[10];
    tmp[11] = src[11];

    dst[0] = tmp[0];
    dst[3] = tmp[1];
    dst[6] = tmp[2];
    dst[1] = tmp[3];
    dst[4] = tmp[4];
    dst[7] = tmp[5];
    dst[2] = tmp[6];
    dst[5] = tmp[7];
    dst[8] = tmp[8];
}

// Copy the translation row (words 9..11) of an actor's fixed-point 3x4 matrix
// into a vector.
// fn_8009F6E4
extern "C" void fn_8009F6E4(int* out, Actor* actor)
{
    int* src = (int*)actor;

    out[0] = src[9];
    out[1] = src[10];
    out[2] = src[11];
}

// Copy one 3-word row of an actor's fixed-point 3x4 matrix into a vector. The
// row index is in words (three) per step of twelve bytes.
void ActorAim(int* out, Actor* actor, int row)
{
    ActorMtx* m = (ActorMtx*)actor;

    out[0] = m->m[row * 3];
    out[1] = m->m[row * 3 + 1];
    out[2] = m->m[row * 3 + 2];
}

// Copies three words of v into column `col` of the 3x3 integer matrix m.
// fn_8009F724
extern "C" void fn_8009F724(int* m, int* v, int col)
{
    m[col] = v[0];
    m[col + 3] = v[1];
    m[col + 6] = v[2];
}

// Copies three words of v into row `row` of the 3x3 integer matrix m.
void Mtx3SetRow(int* m, int* v, int row)
{
    int* d = m + row * 3;

    d[0] = v[0];
    d[1] = v[1];
    d[2] = v[2];
}

// Set a 3x4 fixed-point (Q12) matrix to identity.
// fn_8009F76C
extern "C" void fn_8009F76C(int* m)
{
    m[0] = 0x1000;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0x1000;
    m[5] = 0;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0x1000;
    m[9] = 0;
    m[10] = 0;
    m[11] = 0;
}

extern const float lbl_804D6ABC;  // 1.0f

// Non-zero when any component of v is larger in magnitude than the epsilon.
static inline int Vec3NonZero(const Vec3* v)
{
    return (lbl_804D6AB0 < fabsf(v->x)) + (lbl_804D6AB0 < fabsf(v->y)) +
           (lbl_804D6AB0 < fabsf(v->z));
}

// Build an orthonormal float basis from a forward direction. The direction is
// normalised into the second row of the matrix, the first row is a
// perpendicular in the XY plane and the third row is their cross product.
// fn_8009F7A8
extern "C" void fn_8009F7A8(Mtx34* m, Vec3* dir)
{
    Vec3 fwd;
    Vec3 right;
    float s;

    fwd = *dir;
    if (Vec3NonZero(&fwd)) {
        s = 1.0f / sqrtf(fwd.x * fwd.x + fwd.y * fwd.y + fwd.z * fwd.z);
        fwd.x = fwd.x * s;
        fwd.y = fwd.y * s;
        fwd.z = fwd.z * s;
    } else {
        fwd.x = lbl_804D6AB4;
        fwd.y = lbl_804D6AB4;
        fwd.z = lbl_804D6AB4;
    }

    m->m[3] = fwd.x;
    m->m[4] = fwd.y;
    m->m[5] = fwd.z;

    if (fwd.y == lbl_804D6AB4 && fwd.x == lbl_804D6AB4) {
        right.x = lbl_804D6ABC;
        right.y = lbl_804D6AB4;
        right.z = lbl_804D6AB4;
    } else {
        right.x = fwd.y;
        right.y = -fwd.x;
        right.z = lbl_804D6AB4;
        if (Vec3NonZero(&right)) {
            s = 1.0f / sqrtf(right.x * right.x + right.y * right.y +
                             right.z * right.z);
            right.x = right.x * s;
            right.y = right.y * s;
            right.z = right.z * s;
        } else {
            right.x = lbl_804D6AB4;
            right.y = lbl_804D6AB4;
            right.z = lbl_804D6AB4;
        }
    }

    m->m[0] = right.x;
    m->m[1] = right.y;
    m->m[2] = right.z;
    m->m[6] = right.y * fwd.z - right.z * fwd.y;
    m->m[7] = right.z * fwd.x - right.x * fwd.z;
    m->m[8] = right.x * fwd.y - right.y * fwd.x;
}

extern "C" float fn_800A1418(float angle);
extern "C" float fn_800A1394(float angle);

// Rotation matrix of `angle` about the direction `axis` (Rodrigues' formula).
// The translation row is cleared; only the 3x3 rotation part is written.
void MtxSetAxisAngle(Mtx34* m, Vec3* axis, float angle)
{
    float c = fn_800A1418(angle);
    float s = fn_800A1394(angle);
    float x = axis->x;
    float y = axis->y;
    float z = axis->z;
    float k = lbl_804D6ABC - c;

    m->m[0] = k * x * x + c;
    m->m[4] = k * y * y + c;
    m->m[8] = k * z * z + c;
    m->m[3] = k * x * y + z * s;
    m->m[1] = k * x * y - z * s;
    m->m[6] = k * x * z - y * s;
    m->m[2] = k * x * z + y * s;
    m->m[7] = k * y * z + x * s;
    m->m[5] = k * y * z - x * s;
    m->m[9] = lbl_804D6AB4;
    m->m[10] = lbl_804D6AB4;
    m->m[11] = lbl_804D6AB4;
}

extern "C" void fn_800A0FC4(float* out, float* in);

// Build a rotation matrix from the (x, y, z) Euler angles of `euler`. Each pair
// of angles is turned into sines and cosines by one call to fn_800A0FC4: the
// first two words hold the angles on entry and become their sines, followed by
// their cosines. The (z, x) pair is evaluated together with the (y, 0) pair.
void MtxSetEuler(Mtx34* m, Vec3* euler)
{
    float zx[4];
    float y0[4];
    float sz, sx, cz, cx, sy, cy;

    zx[0] = euler->z;
    zx[1] = euler->x;
    y0[0] = euler->y;
    y0[1] = lbl_804D6AB4;

    fn_800A0FC4(zx, zx);
    fn_800A0FC4(y0, y0);

    sz = zx[0];
    sx = zx[1];
    cz = zx[2];
    cx = zx[3];
    sy = y0[0];
    cy = y0[2];

    m->m[0] = -(sz * sy * sx - cz * cy);
    m->m[1] = cz * sy * sx + sz * cy;
    m->m[2] = -cx * sy;
    m->m[3] = -sz * cx;
    m->m[4] = cz * cx;
    m->m[5] = sx;
    m->m[6] = sz * cy * sx + cz * sy;
    m->m[7] = -(cz * cy * sx - sz * sy);
    m->m[8] = cx * cy;
}

extern const float lbl_804D6AC0;  // -1.0f
extern const float lbl_804D6AC4;  // pi / 2
extern const float lbl_804D6AC8;  // -pi / 2
extern "C" float fn_800A109C(float x);           // acos
extern "C" float fn_800A1128(float y, float x);  // atan2

// Extract the Euler angles (x = pitch, y = yaw, z = roll) from a row-major 3x4
// float rotation matrix. m[5] is the sine of the pitch; the two gimbal cases
// pin z and derive y from the first row instead.
void MtxGetEuler(Vec3* angles, Mtx34* mtx)
{
    if (lbl_804D6ABC <= mtx->m[5]) {
        angles->z = lbl_804D6AB4;
        angles->x = lbl_804D6AC4;
        angles->y = fn_800A1128(-mtx->m[1], mtx->m[0]);
    } else if (mtx->m[5] <= lbl_804D6AC0) {
        angles->z = lbl_804D6AB4;
        angles->x = lbl_804D6AC8;
        angles->y = fn_800A1128(mtx->m[1], mtx->m[0]);
    } else {
        angles->z = fn_800A1128(-mtx->m[3], mtx->m[4]);
        angles->x = lbl_804D6AC4 - fn_800A109C(mtx->m[5]);
        angles->y = fn_800A1128(-mtx->m[2], mtx->m[8]);
    }
}

// Concatenate two 3x3 rotations: dst's rotation becomes a * b, while the
// translation column already in dst is kept.
// fn_8009FC6C
extern "C" void fn_8009FC6C(Mtx34* dst, Mtx34* a, Mtx34* b)
{
    Mtx34 out;

    out.m[0] = a->m[0] * b->m[0] + a->m[1] * b->m[3] + a->m[2] * b->m[6];
    out.m[1] = a->m[0] * b->m[1] + a->m[1] * b->m[4] + a->m[2] * b->m[7];
    out.m[2] = a->m[0] * b->m[2] + a->m[1] * b->m[5] + a->m[2] * b->m[8];
    out.m[3] = a->m[3] * b->m[0] + a->m[4] * b->m[3] + a->m[5] * b->m[6];
    out.m[4] = a->m[3] * b->m[1] + a->m[4] * b->m[4] + a->m[5] * b->m[7];
    out.m[5] = a->m[3] * b->m[2] + a->m[4] * b->m[5] + a->m[5] * b->m[8];
    out.m[6] = a->m[6] * b->m[0] + a->m[7] * b->m[3] + a->m[8] * b->m[6];
    out.m[7] = a->m[6] * b->m[1] + a->m[7] * b->m[4] + a->m[8] * b->m[7];
    out.m[8] = a->m[6] * b->m[2] + a->m[7] * b->m[5] + a->m[8] * b->m[8];
    out.m[9] = dst->m[9];
    out.m[10] = dst->m[10];
    out.m[11] = dst->m[11];
    *dst = out;
}

// Concatenate two 3x4 affine transforms: out = a * b. The product is built in a
// local first so out may alias either input.
// fn_8009FE18
extern "C" void fn_8009FE18(Mtx34* out, Mtx34* a, Mtx34* b)
{
    Mtx34 local;

    local.m[0] = a->m[0] * b->m[0] + a->m[1] * b->m[3] + a->m[2] * b->m[6];
    local.m[1] = a->m[0] * b->m[1] + a->m[1] * b->m[4] + a->m[2] * b->m[7];
    local.m[2] = a->m[0] * b->m[2] + a->m[1] * b->m[5] + a->m[2] * b->m[8];
    local.m[3] = a->m[3] * b->m[0] + a->m[4] * b->m[3] + a->m[5] * b->m[6];
    local.m[4] = a->m[3] * b->m[1] + a->m[4] * b->m[4] + a->m[5] * b->m[7];
    local.m[5] = a->m[3] * b->m[2] + a->m[4] * b->m[5] + a->m[5] * b->m[8];
    local.m[6] = a->m[6] * b->m[0] + a->m[7] * b->m[3] + a->m[8] * b->m[6];
    local.m[7] = a->m[6] * b->m[1] + a->m[7] * b->m[4] + a->m[8] * b->m[7];
    local.m[8] = a->m[6] * b->m[2] + a->m[7] * b->m[5] + a->m[8] * b->m[8];
    local.m[9] = a->m[9] + a->m[0] * b->m[9] + a->m[1] * b->m[10] + a->m[2] * b->m[11];
    local.m[10] = a->m[10] + a->m[3] * b->m[9] + a->m[4] * b->m[10] + a->m[5] * b->m[11];
    local.m[11] = a->m[11] + a->m[6] * b->m[9] + a->m[7] * b->m[10] + a->m[8] * b->m[11];

    *out = local;
}

// Rotate a vector by the 3x3 part of a 3x4 transform (translation ignored).
// fn_800A0030
extern "C" void fn_800A0030(Vec3* out, Mtx34* m, Vec3* v)
{
    Vec3 t;

    t.x = m->m[0] * v->x + m->m[1] * v->y + m->m[2] * v->z;
    t.y = m->m[3] * v->x + m->m[4] * v->y + m->m[5] * v->z;
    t.z = m->m[6] * v->x + m->m[7] * v->y + m->m[8] * v->z;
    *out = t;
}

// Shared global basis matrix (rows: right/up/forward), written by
// fn_800A0100 and read by fn_800A0094 and friends.
extern Mtx34 lbl_80449068;

// Row-vector transform: out = v * M (3x3 part only, translation ignored).
// fn_800A0094
extern "C" void fn_800A0094(Vec3* out, Vec3* v)
{
    out->x = lbl_80449068.m[1] * v->y + lbl_80449068.m[0] * v->x +
             lbl_80449068.m[2] * v->z;
    out->y = lbl_80449068.m[4] * v->y + lbl_80449068.m[3] * v->x +
             lbl_80449068.m[5] * v->z;
    out->z = lbl_80449068.m[7] * v->y + lbl_80449068.m[6] * v->x +
             lbl_80449068.m[8] * v->z;
}

// Store a 3x4 matrix into the shared global, transposing the 3x3 basis part
// (the global is kept in column form for the row-vector helpers).
// fn_800A0100
extern "C" void fn_800A0100(Mtx34* m)
{
    lbl_80449068.m[0] = m->m[0];
    lbl_80449068.m[3] = m->m[1];
    lbl_80449068.m[6] = m->m[2];
    lbl_80449068.m[1] = m->m[3];
    lbl_80449068.m[4] = m->m[4];
    lbl_80449068.m[7] = m->m[5];
    lbl_80449068.m[2] = m->m[6];
    lbl_80449068.m[5] = m->m[7];
    lbl_80449068.m[8] = m->m[8];
    lbl_80449068.m[9] = m->m[9];
    lbl_80449068.m[10] = m->m[10];
    lbl_80449068.m[11] = m->m[11];
}

// Copy the 3x3 basis of a 3x4 matrix with its rows and columns swapped. The
// source is copied to a local first so dst may alias src; the translation
// column is not written.
// fn_800A0168
extern "C" void fn_800A0168(Mtx34* dst, Mtx34* src)
{
    Mtx34 t;

    t = *src;
    dst->m[0] = t.m[0];
    dst->m[3] = t.m[1];
    dst->m[6] = t.m[2];
    dst->m[1] = t.m[3];
    dst->m[4] = t.m[4];
    dst->m[7] = t.m[5];
    dst->m[2] = t.m[6];
    dst->m[5] = t.m[7];
    dst->m[8] = t.m[8];
}

// Extract the translation column (element 9..11) of a 3x4 matrix.
void Mtx34GetTranslation(Vec3* out, Mtx34* m)
{
    out->x = m->m[9];
    out->y = m->m[10];
    out->z = m->m[11];
}

// Copy column `col` of a row-major 3x4 float matrix into a vector.
void MtxGetColumn(Vec3* out, Mtx34* m, int col)
{
    out->x = m->m[col];
    out->y = m->m[col + 3];
    out->z = m->m[col + 6];
}

// Copy element `index` of an array of Vec3 into a vector.
// fn_800A025C
extern "C" void fn_800A025C(Vec3* out, Vec3* arr, int index)
{
    *out = arr[index];
}

// Store a vector into the translation column (elements 9..11) of a row-major
// 3x4 float matrix.
void Mtx34StoreTranslation(Mtx34* m, Vec3* v)
{
    m->m[9] = v->x;
    m->m[10] = v->y;
    m->m[11] = v->z;
}

// Store a vector into column `col` of a row-major 3x4 float matrix.
void Mtx34StoreColumn(Mtx34* m, Vec3* v, int col)
{
    m->m[col] = v->x;
    m->m[col + 3] = v->y;
    m->m[col + 6] = v->z;
}

// Store a vector into element `index` of an array of Vec3.
// fn_800A02C0
extern "C" void fn_800A02C0(Vec3* arr, Vec3* v, int index)
{
    arr[index] = *v;
}

// Build a 3x4 matrix whose columns are the given translation with a zeroed
// rotation basis.
void Mtx34SetTranslation(Mtx34* m, Vec3* v)
{
    float zero = lbl_804D6AB4;

    m->m[0] = v->x;
    m->m[1] = zero;
    m->m[2] = zero;
    m->m[3] = zero;
    m->m[4] = v->y;
    m->m[5] = zero;
    m->m[6] = zero;
    m->m[7] = zero;
    m->m[8] = v->z;
    m->m[9] = zero;
    m->m[10] = zero;
    m->m[11] = zero;
}

// Set a 3x4 float matrix to identity.
void LinkInfoInit(void* p)
{
    Mtx34* m = (Mtx34*)p;

    m->m[0] = lbl_804D6ABC;
    m->m[1] = lbl_804D6AB4;
    m->m[2] = lbl_804D6AB4;
    m->m[3] = lbl_804D6AB4;
    m->m[4] = lbl_804D6ABC;
    m->m[5] = lbl_804D6AB4;
    m->m[6] = lbl_804D6AB4;
    m->m[7] = lbl_804D6AB4;
    m->m[8] = lbl_804D6ABC;
    m->m[9] = lbl_804D6AB4;
    m->m[10] = lbl_804D6AB4;
    m->m[11] = lbl_804D6AB4;
}

// Clear a 3x4 float matrix to all zeroes.
void LinkInfoClear(void* p)
{
    Mtx34* m = (Mtx34*)p;

    m->m[0] = lbl_804D6AB4;
    m->m[1] = lbl_804D6AB4;
    m->m[2] = lbl_804D6AB4;
    m->m[3] = lbl_804D6AB4;
    m->m[4] = lbl_804D6AB4;
    m->m[5] = lbl_804D6AB4;
    m->m[6] = lbl_804D6AB4;
    m->m[7] = lbl_804D6AB4;
    m->m[8] = lbl_804D6AB4;
    m->m[9] = lbl_804D6AB4;
    m->m[10] = lbl_804D6AB4;
    m->m[11] = lbl_804D6AB4;
}

extern const float lbl_804D6AD0;  // 0.5f: half-angle factor
extern const float lbl_804D6AD4;  // 0.0f
extern "C" void fn_800A0FC4(float* out, float* in);

// Build a quaternion from an axis and an angle: xyz = axis * sin(a/2), w = cos(a/2).
// fn_800A039C
extern "C" void fn_800A039C(float angle, float* out, Vec3* axis)
{
    float sc[3];  // in: half angle; out: sc[0] = sin, sc[2] = cos

    sc[0] = lbl_804D6AD0 * angle;
    sc[1] = lbl_804D6AD4;
    fn_800A0FC4(sc, sc);
    out[0] = axis->x * sc[0];
    out[1] = axis->y * sc[0];
    out[2] = axis->z * sc[0];
    out[3] = sc[2];
}

extern const float lbl_804D6AE0;  // 1/4096: Q12 fixed-point -> float
extern const float lbl_804D6AE4;  // 4096.0f: float -> Q12 fixed-point
extern "C" void PSQUATNormalize(float* dst, float* src);
extern "C" double sqrt(double);

// Convert a Q12 fixed-point 3x3 rotation matrix (in[0..8]) into a normalized
// Q12 quaternion (out[0..3]).  Standard "pick the largest diagonal" method:
// the largest of the four candidate components is solved from the square root
// of the trace, the other three are derived from sums/differences of the
// off-diagonal terms.
// fn_800A0420
extern "C" void fn_800A0420(int* out, int* in)
{
    int trace;
    int n;
    int len;
    int scale;
    int axis;
    float q[4];

    trace = in[0] + in[4] + in[8];
    if (trace >= 0) {
        n = (trace + 0x1000) * 0x1000;
        len = 0;
        if (n > 0) {
            len = (int)sqrt((double)n);
        }
        scale = 0x800000 / len;
        out[3] = len / 2;
        out[0] = (scale * (in[7] - in[5])) / 4096;
        out[1] = (scale * (in[2] - in[6])) / 4096;
        out[2] = (scale * (in[3] - in[1])) / 4096;
    } else {
        axis = (in[0] < in[4]);
        if (in[axis * 4] < in[8]) {
            axis = 2;
        }
        switch (axis) {
        case 0:
            n = (in[0] + 0x1000 - in[4] - in[8]) * 0x1000;
            len = 0;
            if (n > 0) {
                len = (int)sqrt((double)n);
            }
            scale = 0x800000 / len;
            out[0] = len / 2;
            out[1] = (scale * (in[1] + in[3])) / 4096;
            out[2] = (scale * (in[2] + in[6])) / 4096;
            out[3] = (scale * (in[7] - in[5])) / 4096;
            break;
        case 1:
            n = (in[4] + 0x1000 - in[8] - in[0]) * 0x1000;
            len = 0;
            if (n > 0) {
                len = (int)sqrt((double)n);
            }
            scale = 0x800000 / len;
            out[1] = len / 2;
            out[2] = (scale * (in[7] + in[5])) / 4096;
            out[0] = (scale * (in[1] + in[3])) / 4096;
            out[3] = (scale * (in[2] - in[6])) / 4096;
            break;
        case 2:
            n = (in[8] + 0x1000 - in[0] - in[4]) * 0x1000;
            len = 0;
            if (n > 0) {
                len = (int)sqrt((double)n);
            }
            scale = 0x800000 / len;
            out[2] = len / 2;
            out[0] = (scale * (in[2] + in[6])) / 4096;
            out[1] = (scale * (in[5] + in[7])) / 4096;
            out[3] = (scale * (in[3] - in[1])) / 4096;
            break;
        }
    }
    q[0] = lbl_804D6AE0 * (float)out[0];
    q[1] = lbl_804D6AE0 * (float)out[1];
    q[2] = lbl_804D6AE0 * (float)out[2];
    q[3] = lbl_804D6AE0 * (float)out[3];
    PSQUATNormalize(q, q);
    out[0] = (int)(lbl_804D6AE4 * q[0]);
    out[1] = (int)(lbl_804D6AE4 * q[1]);
    out[2] = (int)(lbl_804D6AE4 * q[2]);
    out[3] = (int)(lbl_804D6AE4 * q[3]);
}

extern const float lbl_804D6AD8;  // 1.0f
extern const float lbl_804D6ADC;  // 2.0f

// Build a Q12 fixed-point 3x3 rotation matrix (row-major, nine words) from a
// Q12 quaternion (x, y, z, w). The products are Q24, so the unit term is
// 0x1000000 and the result is divided back down by 4096.
// fn_800A089C
extern "C" void fn_800A089C(int* out, int* q)
{
    int x = q[0];
    int y = q[1];
    int z = q[2];
    int w = q[3];
    int xx = x * x;
    int yy = y * y;
    int zz = z * z;

    out[0] = (0x1000000 - 2 * (yy + zz)) / 0x1000;
    out[1] = 2 * (x * y - z * w) / 0x1000;
    out[2] = 2 * (x * z + y * w) / 0x1000;
    out[3] = 2 * (x * y + z * w) / 0x1000;
    out[4] = (0x1000000 - 2 * (xx + zz)) / 0x1000;
    out[5] = 2 * (y * z - x * w) / 0x1000;
    out[6] = 2 * (x * z - y * w) / 0x1000;
    out[7] = 2 * (y * z + x * w) / 0x1000;
    out[8] = (0x1000000 - 2 * (xx + yy)) / 0x1000;
}

// Build the 3x4 world matrix of a bone from its orientation quaternion and
// position: the 3x3 rotation part comes from the quaternion, the translation
// row is copied straight from the position.
void BoneWorldMatrix(Mtx34* m, Bone* bone, float* pos)
{
    float* q = (float*)bone;
    float x = q[0];
    float y = q[1];
    float z = q[2];
    float w = q[3];

    m->m[0] = -(lbl_804D6ADC * (y * y + z * z) - lbl_804D6AD8);
    m->m[1] = lbl_804D6ADC * (x * y - z * w);
    m->m[2] = lbl_804D6ADC * (x * z + y * w);
    m->m[3] = lbl_804D6ADC * (x * y + z * w);
    m->m[4] = -(lbl_804D6ADC * (x * x + z * z) - lbl_804D6AD8);
    m->m[5] = lbl_804D6ADC * (y * z - x * w);
    m->m[6] = lbl_804D6ADC * (x * z - y * w);
    m->m[7] = lbl_804D6ADC * (y * z + x * w);
    m->m[8] = -(lbl_804D6ADC * (x * x + y * y) - lbl_804D6AD8);
    m->m[9] = pos[0];
    m->m[10] = pos[1];
    m->m[11] = pos[2];
}

// Convert the 3x3 rotation part of a row-major 3x4 matrix into a normalized
// quaternion (x, y, z, w). The component with the largest squared magnitude is
// solved from the trace (positive trace) or the corresponding diagonal, and the
// other three come from sums and differences of the off-diagonal terms.
void BoneSetMatrix(void* outQuat, Mtx34* m)
{
    float* q = (float*)outQuat;
    float m0 = m->m[0];
    float m4 = m->m[4];
    float m8 = m->m[8];
    float trace = m8 + m0 + m4;
    float n;
    float len;
    float s;
    int axis;

    if (trace > lbl_804D6AD4) {
        n = lbl_804D6AD8 + trace;
        len = lbl_804D6AD4;
        if (lbl_804D6AD4 < n) {
            len = sqrtf(n);
        }
        s = lbl_804D6AD0 / len;
        q[3] = lbl_804D6AD0 * len;
        q[0] = s * (m->m[7] - m->m[5]);
        q[1] = s * (m->m[2] - m->m[6]);
        q[2] = s * (m->m[3] - m->m[1]);
    } else {
        axis = (m0 < m4);
        if (m->m[axis * 4] < m8) {
            axis = 2;
        }
        switch (axis) {
        case 0:
            n = lbl_804D6AD8 + m0 - m4 - m8;
            len = lbl_804D6AD4;
            if (lbl_804D6AD4 < n) {
                len = sqrtf(n);
            }
            s = lbl_804D6AD0 / len;
            q[0] = lbl_804D6AD0 * len;
            q[1] = s * (m->m[3] + m->m[1]);
            q[2] = s * (m->m[6] + m->m[2]);
            q[3] = s * (m->m[7] - m->m[5]);
            break;
        case 1:
            n = lbl_804D6AD8 + m4 - m8 - m0;
            len = lbl_804D6AD4;
            if (lbl_804D6AD4 < n) {
                len = sqrtf(n);
            }
            s = lbl_804D6AD0 / len;
            q[1] = lbl_804D6AD0 * len;
            q[2] = s * (m->m[7] + m->m[5]);
            q[0] = s * (m->m[1] + m->m[3]);
            q[3] = s * (m->m[2] - m->m[6]);
            break;
        case 2:
            n = lbl_804D6AD8 + m8 - m0 - m4;
            len = lbl_804D6AD4;
            if (lbl_804D6AD4 < n) {
                len = sqrtf(n);
            }
            s = lbl_804D6AD0 / len;
            q[2] = lbl_804D6AD0 * len;
            q[0] = s * (m->m[2] + m->m[6]);
            q[1] = s * (m->m[5] + m->m[7]);
            q[3] = s * (m->m[3] - m->m[1]);
            break;
        }
    }
    PSQUATNormalize(q, q);
}

extern "C" void PSQUATMultiply(float* qa, float* qb, float* qr);

// Rotate a vector by a quaternion: out = q * (v, 0) * conj(q).
void QuatRotateVec(Vec3* out, float* q, Vec3* v)
{
    float conj[4];
    float p[4];
    float tmp[4];

    conj[0] = -q[0];
    conj[1] = -q[1];
    conj[2] = -q[2];
    conj[3] = q[3];
    p[0] = v->x;
    p[1] = v->y;
    p[2] = v->z;
    p[3] = lbl_804D6AD4;

    PSQUATMultiply(p, conj, tmp);
    PSQUATMultiply(q, tmp, conj);

    out->x = conj[0];
    out->y = conj[1];
    out->z = conj[2];
}

// Rotate a vector by a quaternion whose conjugate is normalized first:
// out = q * (v, 0) * conj(q / |q|).
void QuatRotateVecNorm(Vec3* out, float* q, Vec3* v)
{
    float conj[4];
    float p[4];
    float tmp[4];

    conj[0] = -q[0];
    conj[1] = -q[1];
    conj[2] = -q[2];
    conj[3] = q[3];
    PSQUATNormalize(conj, conj);
    p[0] = v->x;
    p[1] = v->y;
    p[2] = v->z;
    p[3] = lbl_804D6AD4;

    PSQUATMultiply(p, conj, tmp);
    PSQUATMultiply(q, tmp, conj);

    out->x = conj[0];
    out->y = conj[1];
    out->z = conj[2];
}

// Build only the 3x3 rotation part of a matrix from a unit quaternion.
void BoneGetMatrix(Mtx34* m, void* qv)
{
    float* q = (float*)qv;
    float x = q[0];
    float y = q[1];
    float z = q[2];
    float w = q[3];

    m->m[0] = -(lbl_804D6ADC * (y * y + z * z) - lbl_804D6AD8);
    m->m[1] = lbl_804D6ADC * (x * y - z * w);
    m->m[2] = lbl_804D6ADC * (x * z + y * w);
    m->m[3] = lbl_804D6ADC * (x * y + z * w);
    m->m[4] = -(lbl_804D6ADC * (x * x + z * z) - lbl_804D6AD8);
    m->m[5] = lbl_804D6ADC * (y * z - x * w);
    m->m[6] = lbl_804D6ADC * (x * z - y * w);
    m->m[7] = lbl_804D6ADC * (y * z + x * w);
    m->m[8] = -(lbl_804D6ADC * (x * x + y * y) - lbl_804D6AD8);
}

// Angles in this file are degrees. The cosine is evaluated as the odd Taylor
// series of sin(2*pi*u) at a quarter-revolution offset, since
// sin(2*pi*(0.25 - phase)) == cos(2*pi*phase); the revolution phase is wrapped
// to [-0.5, 0.5] by rounding through the 1.5*2^23 magic constant.
extern const float lbl_804D6AF0;  // 1/360: degrees -> revolutions
extern const float lbl_804D6AF4;  // 90 degrees
extern const float lbl_804D6AF8;  // 2.0
extern const float lbl_804D6AFC;  // 1.5*2^23: round-to-nearest magic
extern const float lbl_804D6B00;  // 0.25
extern const float lbl_804D6B04;  // 2*pi
extern const float lbl_804D6B08;  // -(2*pi)^3/3!
extern const float lbl_804D6B0C;  // (2*pi)^5/5!
extern const float lbl_804D6B10;  // -(2*pi)^7/7!
extern const float lbl_804D6B14;  // (2*pi)^9/9!

static inline float CosSeries(float deg)
{
    float p;
    float wrapped;
    float u;
    float u2;
    float u3;
    float u5;
    float u7;
    float u9;

    p = fabsf(deg) * lbl_804D6AF0;
    wrapped = p - ((p + lbl_804D6AFC) - lbl_804D6AFC);
    u = fabsf(wrapped - lbl_804D6AF8 * lbl_804D6B00) - lbl_804D6B00;
    u2 = u * u;
    u3 = u * u2;
    u5 = u3 * u2;
    u7 = u5 * u2;
    u9 = u7 * u2;
    return u9 * lbl_804D6B14 + u7 * lbl_804D6B10 + u5 * lbl_804D6B0C
         + u3 * lbl_804D6B08 + u * lbl_804D6B04;
}

// out[0..1] become the sines of the two angles, out[2..3] their cosines.
extern "C" void fn_800A0FC4(float* out, float* in)
{
    float a0 = in[0];
    float a1 = in[1];

    out[0] = CosSeries(a0 - lbl_804D6AF4);
    out[1] = CosSeries(a1 - lbl_804D6AF4);
    out[2] = CosSeries(a0);
    out[3] = CosSeries(a1);
}

extern const float lbl_804D6B18;  // -1.0f
extern const float lbl_804D6B1C;  // 180.0f
extern const float lbl_804D6B20;  // 1.0f
extern const float lbl_804D6B24;  // 0.0f
extern const float lbl_804D6B28;  // 89.99613f
extern const float lbl_804D6B2C;  // -12.15326f
extern const float lbl_804D6B30;  // 4.254842f
extern const float lbl_804D6B34;  // -1.073110f

// Inverse cosine in degrees: sqrt(1 - x) times a cubic fit on [-1, 1], folded
// through 180 degrees for negative inputs and saturated outside the domain.
extern "C" float fn_800A109C(float x)
{
    bool neg;
    float x2;
    float ax;

    neg = false;
    if (x < lbl_804D6B18)
        return lbl_804D6B1C;
    if (x > lbl_804D6B20)
        return lbl_804D6B24;
    if (x < lbl_804D6B24) {
        x = -x;
        neg = true;
    }
    x2 = x * x;
    ax = lbl_804D6B34 * x * x2 + lbl_804D6B30 * x2 + lbl_804D6B2C * x
       + lbl_804D6B28;
    ax = ax * sqrtf(lbl_804D6B20 - x);
    if (neg)
        return lbl_804D6B1C - ax;
    return ax;
}

extern const float lbl_804D6B38;   // -90.0f
extern const float lbl_804D6B3C;   // pi
extern const float lbl_804D6B40;   // -pi
extern double lbl_804D6B48;  // 0.0 (double)
extern const float lbl_804D6B50;   // tan(3*pi/8) = 2.41421366f
extern const float lbl_804D6B54;   // pi/2
extern const float lbl_804D6B58;   // tan(pi/8) = 0.41421357f
extern const float lbl_804D6B5C;   // pi/4
extern const float lbl_804D6B60;   // arctangent polynomial, constant term
extern const float lbl_804D6B64;   // arctangent polynomial, s^3 coefficient
extern const float lbl_804D6B68;   // arctangent polynomial, s^2 coefficient
extern const float lbl_804D6B6C;   // arctangent polynomial, s coefficient
extern const float lbl_804D6B70;   // 180/pi: radians -> degrees
extern const float lbl_804D6B74;   // pi/180: degrees -> radians
extern const float lbl_804D6B78;   // 4/pi: radians -> quarter turns
extern double lbl_804D6B80;  // 1.0 (double)
extern const float lbl_804D6B88;   // pi/4 split, high part
extern const float lbl_804D6B8C;   // pi/4 split, middle part
extern const float lbl_804D6B90;   // pi/4 split, low part
extern double lbl_804D6B98;  // 1e-4: below this tan(x) == x
extern const float lbl_804D6BA0;   // tangent polynomial coefficients
extern const float lbl_804D6BA4;
extern const float lbl_804D6BA8;
extern const float lbl_804D6BAC;
extern const float lbl_804D6BB0;
extern const float lbl_804D6BB4;
extern double lbl_804D6BB8;  // -1.0 (double)
extern double lbl_804D6BC0;  // 2^52 + 2^31: int -> float conversion magic

// atan2(y, x) in degrees. The ratio is folded into one of three ranges
// (|v| <= tan(pi/8), <= tan(3*pi/8), > tan(3*pi/8)), evaluated with the
// usual minimax arctangent polynomial, and then rotated into its quadrant.
extern "C" float fn_800A1128(float y, float x)
{
    int quadrant = 0;
    if (x < lbl_804D6B24)
        quadrant = 2;
    if (y < lbl_804D6B24)
        quadrant |= 1;
    if (x == lbl_804D6B24) {
        if (quadrant & 1)
            return lbl_804D6B38;
        if (y != lbl_804D6B24)
            return lbl_804D6AF4;
        return lbl_804D6B24;
    }
    if (y == lbl_804D6B24) {
        if ((quadrant & 2) == 0)
            return lbl_804D6B24;
        return lbl_804D6B1C;
    }
    float offset;
    if (quadrant == 2)
        offset = lbl_804D6B3C;
    else if (quadrant == 3)
        offset = lbl_804D6B40;
    else
        offset = lbl_804D6B24;
    float v = y / x;
    int neg = 0;
    if (v < lbl_804D6B48) {
        neg = -1;
        v = -v;
    }
    float angle;
    if (v > lbl_804D6B50) {
        angle = lbl_804D6B54;
        v = -(lbl_804D6B20 / v);
    } else if (v > lbl_804D6B58) {
        angle = lbl_804D6B5C;
        v = (v - lbl_804D6B20) / (lbl_804D6B20 + v);
    } else {
        angle = lbl_804D6B24;
    }
    float s = v * v;
    float p = lbl_804D6B64 * s - lbl_804D6B68;
    p = p * s + lbl_804D6B60;
    p = s * p - lbl_804D6B6C;
    angle = angle + (v * (s * p) + v);
    if (neg < 0)
        angle = -angle;
    angle = angle + offset;
    return lbl_804D6B70 * angle;
}

// Tangent of an angle in degrees. The angle is reduced to |r| <= pi/4 with a
// three-part Cody-Waite split of pi/4; the tangent polynomial is only needed
// above 1e-4, and every other quarter turn inverts the result.
extern "C" float fn_800A127C(float degrees)
{
    int neg = 0;
    float rad = lbl_804D6B74 * degrees;
    float a = rad;
    if (rad < lbl_804D6B48) {
        a = -rad;
        neg = -1;
    }
    int n = (int)(lbl_804D6B78 * a);
    float nf = (float)n;
    if (n & 1) {
        n++;
        nf = nf + lbl_804D6B80;
    }
    float r = a - lbl_804D6B88 * nf - lbl_804D6B8C * nf - lbl_804D6B90 * nf;
    float s = r * r;
    float t;
    if (a > lbl_804D6B98) {
        t = r + r * s
          * (((((lbl_804D6BA0 * s + lbl_804D6BA4) * s + lbl_804D6BA8) * s
               + lbl_804D6BAC) * s + lbl_804D6BB0) * s + lbl_804D6BB4);
    } else {
        t = r;
    }
    if (n & 2)
        t = lbl_804D6BB8 / t;
    if (neg < 0)
        t = -t;
    return t;
}

// Sine of an angle in degrees, evaluated as cos(angle - 90).
extern "C" float fn_800A1394(float angle)
{
    return CosSeries(angle - lbl_804D6AF4);
}

// Cosine of an angle in degrees.
extern "C" float fn_800A1418(float angle)
{
    return CosSeries(angle);
}

// Q12 sine table, offset by 0x400 so that entries cover [0, 0x800]. Entries
// just below the base address continue the same curve for negative sines.
extern "C" short lbl_80399650[];

// Arc sine of a Q12 sine value, returned in Q12 angle units (0x400 = 90
// degrees). The argument is clamped to the table's range.
extern "C" int fn_800A1494(int value)
{
    if (value < 0) {
        if (value < -0x1000) {
            value = -0x1000;
        }
        return lbl_80399650[0x1000 + value] - 0x400;
    }
    if (value > 0) {
        if (value > 0x1000) {
            value = 0x1000;
        }
        return 0x400 - lbl_80399650[0x1000 - value];
    }
    return 0;
}

// Arc cosine of a Q12 cosine value, returned in Q12 angle units (0x400 = 90
// degrees). Same clamped table lookup as the arc sine above.
extern "C" int fn_800A14F8(int value)
{
    if (value < 0) {
        if (value < -0x1000) {
            value = -0x1000;
        }
        return 0x800 - lbl_80399650[0x1000 + value];
    }
    if (value > 0) {
        if (value > 0x1000) {
            value = 0x1000;
        }
        return lbl_80399650[0x1000 - value];
    }
    return 0x400;
}

extern short lbl_80398650[];  // Q12 arctangent table, 0x400 entries

// Arctangent of y/x in the Q12 angle unit. The magnitudes of both inputs are
// reduced to the first octant against the lookup table and the quadrant signs
// are re-applied afterwards.
// fn_800A1558
extern "C" int fn_800A1558(int y, int x)
{
    int yNeg = 0;
    int xNeg = 0;
    int angle;

    if (y == 0 && x == 0)
        return 0;
    if (y < 0) {
        y = -y;
        yNeg = 1;
    }
    if (x < 0) {
        x = -x;
        xNeg = 1;
    }
    if (y < x) {
        if (y > 0x1FFFFF)
            y = 0x1FFFFF;
        angle = 0x400 - lbl_80398650[(y << 10) / x];
    } else if (x < y) {
        if (x > 0x1FFFFF)
            x = 0x1FFFFF;
        angle = lbl_80398650[(x << 10) / y];
    } else {
        angle = 0x200;
    }
    if (yNeg)
        angle = 0x800 - angle;
    if (xNeg)
        return -angle;
    return angle;
}

extern short lbl_80398E50[];  // Q12 sine table, 0x800 entries

// Sine of a Q12 angle. The angle is folded into 0..0x400 first; the table
// holds the first quadrant and the other quadrants are derived by mirroring.
// fn_800A1624
extern "C" int fn_800A1624(int angle)
{
    int a = angle - 0x400;
    int u;

    u = ((a >> 31) ^ a) - (a >> 31);
    u &= 0xFFF;
    if (u > 0x800)
        u = 0x1000 - u;
    if (u > 0x400)
        return -lbl_80398E50[0x800 - u];
    if (u < 0x400)
        return lbl_80398E50[u];
    return 0;
}

// Cosine of a Q12 angle. Same table as fn_800A1624, but without the 0x400 phase
// offset: the angle is folded into the first quadrant and mirrored.
// fn_800A1688
extern "C" int fn_800A1688(int angle)
{
    int u = ((angle >> 31) ^ angle) - (angle >> 31);

    u &= 0xFFF;
    if (u > 0x800)
        u = 0x1000 - u;
    if (u > 0x400)
        return -lbl_80398E50[0x800 - u];
    if (u < 0x400)
        return lbl_80398E50[u];
    return 0;
}

// Empty Q12 helper, kept as a real function by the original code.
void Q12Noop(void)
{
}

// Predicate callback: always reports success.
int AlwaysTrue(void)
{
    return 1;
}

extern int FileAddHandler(int id, int (*handler)(void*, void*), void* data);
extern struct AnimBank* lbl_804D4790;

int AnimBankInit(short* data, int size);

// Register this unit's file handler (id 0x2c) and clear its state word.
int ActorFileHandlerInit(void)
{
    lbl_804D4790 = 0;
    FileAddHandler(0x2c, (int (*)(void*, void*))AnimBankInit, 0);
    return 1;
}

// Animation bank tree: the root table (lbl_804D4790) holds groups, each group
// holds subgroups, each subgroup holds sets, and each set holds leaf entries
// with an animation index. All nodes are 8-byte counted arrays.
struct AnimEntry {
    short index;
    short flags;
    short anim;
    short pad;
};

struct AnimSet {
    unsigned short id;
    short count;
    AnimEntry* items;
};

struct AnimSub {
    unsigned short id;
    short count;
    AnimSet* items;
};

struct AnimGroup {
    unsigned short id;
    short count;
    AnimSub* items;
};

struct AnimBank {
    short unk0;
    short count;
    AnimGroup* items;
};

extern AnimBank* lbl_804D4790;

extern void AnimUnref(int anim);
extern void AnimRef(int anim);

// Release every animation referenced by bank `bankId`, group `groupId`.
// Returns 0 if the bank or group is unknown, 1 if the entries were released.
int AnimBankUnref(unsigned short bankId, unsigned short groupId)
{
    AnimGroup* group;
    AnimSub* sub;
    AnimSet* set;
    int i;
    int j;

    group = 0;
    for (i = 0; i < lbl_804D4790->count; i++) {
        if (bankId == lbl_804D4790->items[i].id) {
            group = &lbl_804D4790->items[i];
            break;
        }
    }
    if (group == 0)
        return 0;

    sub = 0;
    for (i = 0; i < group->count; i++) {
        if (groupId == group->items[i].id) {
            sub = &group->items[i];
            break;
        }
    }
    if (sub == 0)
        return 0;

    for (i = 0; i < sub->count; i++) {
        set = &sub->items[i];
        if (set) {
            for (j = 0; j < set->count; j++)
                AnimUnref(set->items[j].anim);
        }
    }
    return 1;
}

// Reference every animation in bank `bankId`, group `groupId` (same walk as
// AnimBankUnref). Returns 0 if the bank or group is unknown, 1 otherwise.
// fn_800A1860
extern "C" int fn_800A1860(unsigned short bankId, unsigned short groupId)
{
    AnimGroup* group;
    AnimSub* sub;
    AnimSet* set;
    int i;
    int j;

    group = 0;
    for (i = 0; i < lbl_804D4790->count; i++) {
        if (bankId == lbl_804D4790->items[i].id) {
            group = &lbl_804D4790->items[i];
            break;
        }
    }
    if (group == 0)
        return 0;

    sub = 0;
    for (i = 0; i < group->count; i++) {
        if (groupId == group->items[i].id) {
            sub = &group->items[i];
            break;
        }
    }
    if (sub == 0)
        return 0;

    for (i = 0; i < sub->count; i++) {
        set = &sub->items[i];
        if (set) {
            for (j = 0; j < set->count; j++)
                AnimRef(set->items[j].anim);
        }
    }
    return 1;
}

extern "C" void fn_80093478(int anim, int flag);

// Enable every animation referenced by bank `bankId`, group `groupId`.
// Returns 0 if the bank or group is unknown, 1 if the entries were flagged.
int AnimBankActivate(unsigned short bankId, unsigned short groupId)
{
    AnimGroup* group;
    AnimSub* sub;
    AnimSet* set;
    int i;
    int j;

    group = 0;
    for (i = 0; i < lbl_804D4790->count; i++) {
        if (bankId == lbl_804D4790->items[i].id) {
            group = &lbl_804D4790->items[i];
            break;
        }
    }
    if (group == 0)
        return 0;

    sub = 0;
    for (i = 0; i < group->count; i++) {
        if (groupId == group->items[i].id) {
            sub = &group->items[i];
            break;
        }
    }
    if (sub == 0)
        return 0;

    for (i = 0; i < sub->count; i++) {
        set = &sub->items[i];
        if (set) {
            for (j = 0; j < set->count; j++)
                fn_80093478(set->items[j].anim, 1);
        }
    }
    return 1;
}

// Look up bank `bankId`, group `groupId`, set `setId` and return the number of
// animation entries in that set, or 0 if any level is missing.
int AnimBankGetSetCount(unsigned short bankId, unsigned short groupId,
                        unsigned short setId)
{
    AnimGroup* group;
    AnimSub* sub;
    AnimSet* set;
    int i;

    group = 0;
    for (i = 0; i < lbl_804D4790->count; i++) {
        if (bankId == lbl_804D4790->items[i].id) {
            group = &lbl_804D4790->items[i];
            break;
        }
    }
    if (group == 0)
        return 0;

    sub = 0;
    for (i = 0; i < group->count; i++) {
        if (groupId == group->items[i].id) {
            sub = &group->items[i];
            break;
        }
    }
    if (sub == 0)
        return 0;

    set = 0;
    for (i = 0; i < sub->count; i++) {
        if (setId == sub->items[i].id) {
            set = &sub->items[i];
            break;
        }
    }
    if (set == 0)
        return 0;

    return set->count;
}

// Look up the animation entry at `index` for bank `bankId`, group `groupId`.
// When `out` is non-null the entry's pad word is stored there; the entry's
// animation index is returned, or -1 if any level of the tree is missing.
// fn_800A1BC8
extern "C" int fn_800A1BC8(unsigned short bankId, unsigned short groupId,
                      unsigned short setId, int index, unsigned int* out)
{
    AnimGroup* group;
    AnimSub* sub;
    AnimSet* set;
    int i;

    if (out != 0)
        *out = 0;

    group = 0;
    for (i = 0; i < lbl_804D4790->count; i++) {
        if (bankId == lbl_804D4790->items[i].id) {
            group = &lbl_804D4790->items[i];
            break;
        }
    }
    if (group == 0)
        return -1;

    sub = 0;
    for (i = 0; i < group->count; i++) {
        if (groupId == group->items[i].id) {
            sub = &group->items[i];
            break;
        }
    }
    if (sub == 0)
        return -1;

    set = 0;
    for (i = 0; i < sub->count; i++) {
        if (setId == sub->items[i].id) {
            set = &sub->items[i];
            break;
        }
    }
    if (set == 0)
        return -1;

    if (set->count <= 0)
        return -1;

    index = index % set->count;
    if (out != 0)
        *out = (unsigned short)set->items[index].pad;
    return set->items[index].anim;
}

extern "C" void fn_800070E0(void);

// Read an animation bank file image: an 8-byte header followed by a four-level
// tree of 8-byte counted arrays (groups, subgroups, sets, entries). Every node
// stores the byte size its children will occupy. Returns 1 when the image size
// accounted for the whole tree exactly, else 0.
int AnimBankInit(short* data, int size)
{
    AnimBank* bank;
    AnimGroup* group;
    AnimSub* sub;
    AnimSet* set;
    short* p;
    int rem;
    int count;
    int i;
    int j;
    int k;
    bool valid;

    fn_800070E0();

    bank = (size > 0) ? (AnimBank*)data : 0;
    lbl_804D4790 = bank;
    p = data + 4;
    rem = size - 8;
    if (bank->unk0 != 0xf)
        return 0;

    count = bank->count;
    valid = false;
    if (rem > 0 && count * 8 > 0)
        valid = true;
    bank->items = valid ? (AnimGroup*)p : 0;
    p += count * 4;
    rem -= count * 8;

    for (i = 0; i < lbl_804D4790->count; i++) {
        count = lbl_804D4790->items[i].count;
        valid = false;
        if (rem > 0 && count * 8 > 0)
            valid = true;
        lbl_804D4790->items[i].items = valid ? (AnimSub*)p : 0;
        p += count * 4;
        rem -= count * 8;
    }

    for (i = 0; i < lbl_804D4790->count; i++) {
        group = &lbl_804D4790->items[i];
        for (j = 0; j < group->count; j++) {
            count = group->items[j].count;
            valid = false;
            if (rem > 0 && count * 8 > 0)
                valid = true;
            group->items[j].items = valid ? (AnimSet*)p : 0;
            p += count * 4;
            rem -= count * 8;
        }
    }

    for (i = 0; i < lbl_804D4790->count; i++) {
        group = &lbl_804D4790->items[i];
        for (j = 0; j < group->count; j++) {
            sub = &group->items[j];
            for (k = 0; k < sub->count; k++) {
                count = sub->items[k].count;
                valid = false;
                if (rem > 0 && count * 8 > 0)
                    valid = true;
                sub->items[k].items = valid ? (AnimEntry*)p : 0;
                p += count * 4;
                rem -= count * 8;
            }
        }
    }

    return rem == 0;
}

// Live-agent query for a 24-bit agent handle: 0 when the handle is out of the
// active range (or outside the allocated bitmap), otherwise nonzero unless the
// agent's state field says it is dead (state 7).
extern int lbl_804D47C4;             // first active agent slot
extern void* lbl_804D47D0;           // agent slot base
extern unsigned int lbl_804D47E0[1]; // per-slot allocated bitmap

// fn_800A1F94
extern "C" int fn_800A1F94(int id)
{
    id &= 0xFFFFFF;
    if (lbl_804D47D0 == 0) {
        return 0;
    }
    if (id < 0 || id > lbl_804D47C4 + 0x10) {
        return 0;
    }
    if (id >= lbl_804D47C4) {
        int bit = id - lbl_804D47C4;
        if (!(lbl_804D47E0[bit / 32] & (1 << (bit & 31)))) {
            return 0;
        }
    }
    return *(int*)((char*)lbl_804D47D0 + id * 0x928 + 0x8F8) != 7;
}

// ---------------------------------------------------------------------------
// AINeoAgent save loader.
//
// The reader object used by the save/load code: [0] the agent being built,
// [1] bytes left in the parent block, [2] the cursor and [3] the running
// chunk budget that each read advances.
struct NeoAgentStream {
    int* owner;
    int remaining;
    unsigned char* cur;
    int end;
    int error;        // cleared by the save writer whenever it runs out of room
};

// Header of the "AINeoAgentSAVE" chunk: static slot count, first slot index
// and the number of dynamic agents whose RTLP chunks follow.
struct NeoAgentSaveHeader {
    int version;
    int staticCount;
    int dynamicCount;
};

// Serialised AINeoAgentDATA chunk (0x11C bytes).
struct NeoAgentRecord {
    int unk00;
    int unk04;
    int unk08;
    int x;
    int y;
    int z;
    int unk18;
    int unk1c;
    int state;
    int unk24;
    unsigned char rest[0xBC];
    int fE4;
    int fE8;
    int fEC;
    int fF0;
    int fF4;
    int fF8;
    int fFC;
    int f100;
    int f104;
    int f108;
    int f10C;
    int f110;
    int f114;
    int f118;
};

// The in-memory AINeoAgent slot (0x928 bytes).
struct NeoAgent {
    int unk00[9];
    int f24;
    int f28;
    int f2c;
    unsigned char unk30[0x54];
    float f84;
    float f88;
    float f8c;
    unsigned char unk90[0x90];
    void* f120;
    void* f124;
    int f128;
    int f12c;
    unsigned char unk128[0xE4];
    unsigned char f214[0x2C];
    unsigned char f240[0x30];
    unsigned char f270[0x50];
    unsigned char unk2c0[0x1FC];
    int f4bc;
    int f4c0;
    int f4c4;
    int f4c8;
    int f4cc;
    int f4d0;
    int f4d4;
    int f4d8;
    unsigned char unk4dc[0x3B8];
    int f894;
    int f898;
    int f89c;
    int f8a0;
    unsigned char unk8a4[0x48];
    int f8ec;
    unsigned char unk8f0[8];
    int state;
    unsigned char unk8fc[8];
    float f904;
};

extern unsigned char __upper_map[256];
extern "C" int fn_800B365C(void);
extern "C" int fn_800B4690(void* stream);
extern "C" int fn_800F7D28(int, int);
extern "C" void fn_800F825C(int, int, int);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void* memcpy(void* dst, const void* src, unsigned long n);
// fn_800A2DE8
extern "C" int fn_800A2DE8(int* def, unsigned int* time);
extern "C" void fn_800A6198(int slot, void* data);
extern "C" int fn_800B3818(void);
extern "C" int fn_800B47EC(void* stream);
extern "C" int fn_800F81EC(int handle);
extern "C" int fn_800F8130(int handle, int index);
extern "C" int fn_800F8AF4(int entry);
extern "C" void fn_8016D68C(void* dst, void* src);
extern unsigned char* lbl_804D47D4;   // AI agent definition table
extern int lbl_804D47CC;              // number of definition records
extern int lbl_804D47C8;              // number of allocated dynamic slots
extern unsigned char lbl_804490AC[];  // per-slot agent save records
int BitsetFirstClear(unsigned long* set, int start, int count);
int ActorIsActive(Actor* actor);
// fn_8009F028
extern "C" void fn_8009F028(int* mtx, int* euler);
void ActorRelinkOwners(Actor* actor);
void ActorFn80022D98(Actor* actor, int value);
int OwnerGetInfo(unsigned long id);
int Fn800F8A98(unsigned long id);

// Upper-case-map name hash used by the chunk tags.
static inline unsigned int AINeoNameHash(const char* name)
{
    unsigned int hash;
    const char* p;
    const char* q;

    hash = 0;
    if (name != 0) {
        q = name;
        for (p = name; *q != '\0'; q++) {
            unsigned int c;
            if (*p == -1) {
                c = 0xFFFFFFFF;
            } else {
                c = __upper_map[(unsigned char)*p];
            }
            hash = hash * 31;
            hash = hash + (c & 0xFF);
            p++;
        }
    }
    return hash % 0xFFFFFFFF;
}

// Begins a chunk: checks that the block is at least `minSize` bytes, reads its
// four-byte tag and compares it with the hashed name.
static inline int AINeoChunkTag(NeoAgentStream* s, const char* name, int minSize)
{
    unsigned int tag;

    if (s->end < minSize) {
        return 0;
    }
    if (s->end < 4) {
        return 0;
    }
    tag = *(unsigned int*)s->cur;
    s->end += 4;
    s->remaining -= 4;
    s->cur += 4;
    return tag == AINeoNameHash(name);
}

int AINeoAgentSaveLoad(NeoAgentStream* s)
{
    typedef int (*ReadChunkFn)(void*, void*, int, int);
    NeoAgentSaveHeader* hdr;
    NeoAgentRecord* rec;
    NeoAgent* agent;
    unsigned char* chunk;
    unsigned int id;
    int limit;
    int count;
    int diff;
    int offset;
    int i;
    int total;
    int name[2];
    unsigned char msg[0x24];

    if (fn_800B365C() == 0) {
        return 0;
    }
    if (fn_800B4690(s) == 0) {
        return 0;
    }
    if (!AINeoChunkTag(s, "AINeoAgentSAVE", 0x10)) {
        return 1;
    }
    hdr = (NeoAgentSaveHeader*)s->cur;
    s->end += 0xC;
    s->remaining -= 0xC;
    s->cur += 0xC;
    if (hdr->staticCount != lbl_804D47C4) {
        return 1;
    }
    for (i = 0; i < hdr->dynamicCount; i++) {
        if (!AINeoChunkTag(s, "AINeoAgentRTLP", 0x38)) {
            return 1;
        }
        chunk = s->cur;
        s->end += 0x34;
        s->remaining -= 0x34;
        s->cur += 0x34;
        limit = 1000;
        if (((ReadChunkFn)&fn_800A2DE8)(chunk, &limit, 0, 0) == 0) {
            return 1;
        }
    }
    total = hdr->staticCount + hdr->dynamicCount;
    offset = 0;
    for (i = 0; i < total; i++) {
        if (!AINeoChunkTag(s, "AINeoAgentDATA", 0x120)) {
            return 1;
        }
        rec = (NeoAgentRecord*)s->cur;
        s->end += 0x11C;
        s->remaining -= 0x11C;
        s->cur += 0x11C;
        agent = (NeoAgent*)((unsigned char*)lbl_804D47D0 + offset);
        fn_8009F028((int*)agent, (int*)rec);
        agent->f24 = rec->x;
        agent->f28 = rec->y;
        agent->f2c = rec->z;
        agent->f84 = (float)rec->x;
        agent->f88 = (float)rec->y;
        agent->f8c = (float)rec->z;
        ActorRelinkOwners((Actor*)agent);
        ActorFn80022D98((Actor*)agent, rec->f104);
        agent->state = rec->state;
        agent->f904 = (float)rec->unk24;
        if (agent->f8ec != -1) {
            for (count = 0; count < rec->f108; count++) {
                memcpy(name, s->cur, 8);
                s->end -= 8;
                s->cur += 8;
                id = Fn800F8A98(name[0]);
                diff = fn_800F7D28(agent->f8ec, id);
                if (name[1] < diff) {
                    fn_800F825C(agent->f8ec, id, diff - name[1]);
                } else if (diff < name[1]) {
                    fn_800F825C(agent->f8ec, id, name[1] - diff);
                }
            }
        }
        agent->f894 = rec->f10C;
        agent->f898 = rec->f110;
        agent->f89c = rec->f114;
        agent->f8a0 = rec->f118;
        if (agent->f120 != 0) {
            s->owner = (int*)agent;
            ((void (*)(int, void*))agent->f120)(0x7103, s);
        }
        if (agent->f124 != 0) {
            s->owner = (int*)agent;
            ((void (*)(int, void*))agent->f124)(0x7103, s);
        }
        memcpy(agent->f214, rec->rest, 0x2C);
        memcpy(agent->f240, rec->rest + 0x2C, 0x30);
        memcpy(agent->f270, rec->rest + 0x5C, 0x50);
        agent->f4bc = rec->fE4;
        agent->f4c0 = rec->fE8;
        agent->f4c4 = rec->fEC;
        agent->f4c8 = rec->fF0;
        agent->f4cc = rec->fF4;
        agent->f4d0 = rec->fF8;
        agent->f4d4 = rec->fFC;
        agent->f4d8 = rec->f100;
        if (*(int*)(agent->f214 + 0x0C) == 2) {
            if (*(int*)(agent->f214 + 0x18) == -1) {
                *(int*)(agent->f214 + 0x18) = 0;
            } else {
                SendMsg(*(int*)(agent->f214 + 0x18), 0x400, msg);
                *(int*)(agent->f214 + 0x18) = *(int*)msg;
            }
        }
        if (*(int*)(agent->f270 + 0x20) == 2) {
            if (*(int*)(agent->f270 + 0x44) == -1) {
                *(int*)(agent->f270 + 0x44) = 0;
            } else {
                SendMsg(*(int*)(agent->f270 + 0x44), 0x400, msg);
                *(int*)(agent->f270 + 0x44) = *(int*)msg;
            }
        }
        if (agent->f4c8 == 2) {
            if (agent->f4bc == -1) {
                agent->f4bc = 0;
            } else {
                agent->f4bc = OwnerGetInfo((unsigned long)agent->f4bc);
            }
        }
        offset += 0x928;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// AINeoAgent save writer: the mirror image of the loader above.  It writes an
// "AINeoAgentSAVE" header, one "AINeoAgentRTLP" block for every dynamic agent
// slot and one "AINeoAgentDATA" record for every live agent.  Returns 1; when
// the output buffer cannot hold the next block the stream's error flag is
// cleared and the function bails out early.

static inline void AINeoStreamWrite(NeoAgentStream* s, const void* src, int size)
{
    memcpy(s->cur, src, size);
    s->end += size;
    s->remaining -= size;
    s->cur += size;
}

int AINeoAgentSaveWrite(NeoAgentStream* s)
{
    NeoAgentSaveHeader* hdr;
    NeoAgentRecord* rec;
    NeoAgent* agent;
    unsigned int tag;
    int name[2];
    int entry;
    int count;
    int bit;
    int i;

    if (fn_800B3818() == 0) {
        s->error = 0;
        return 1;
    }
    if (fn_800B47EC(s) == 0) {
        s->error = 0;
        return 1;
    }
    if (s->remaining < 0x10) {
        s->error = 0;
        return 1;
    }

    tag = AINeoNameHash("AINeoAgentSAVE");
    AINeoStreamWrite(s, &tag, 4);
    hdr = (NeoAgentSaveHeader*)s->cur;
    s->end += 0xC;
    s->remaining -= 0xC;
    s->cur += 0xC;
    hdr->version = 2;
    hdr->staticCount = lbl_804D47C4;
    hdr->dynamicCount = 0;

    for (i = 0; i < 16; i++) {
        if (!(lbl_804D47E0[0] & (1 << i))) {
            continue;
        }
        agent = (NeoAgent*)((unsigned char*)lbl_804D47D0 +
                            (i + lbl_804D47C4) * 0x928);
        hdr->dynamicCount = hdr->dynamicCount + 1;
        if (s->remaining < 0x38) {
            s->error = 0;
            return 1;
        }
        tag = AINeoNameHash("AINeoAgentRTLP");
        AINeoStreamWrite(s, &tag, 4);
        AINeoStreamWrite(s, (unsigned char*)agent + 0xC8, 0x34);
    }

    for (i = 0; i < lbl_804D47C4 + 0x10; i++) {
        if (i >= lbl_804D47C4) {
            bit = i - lbl_804D47C4;
            if (!(lbl_804D47E0[bit / 32] & (1 << (bit & 31)))) {
                continue;
            }
        }
        agent = (NeoAgent*)((unsigned char*)lbl_804D47D0 + i * 0x928);
        if (s->remaining < 0x120) {
            s->error = 0;
            return 1;
        }
        tag = AINeoNameHash("AINeoAgentDATA");
        AINeoStreamWrite(s, &tag, 4);
        rec = (NeoAgentRecord*)s->cur;
        s->end += 0x11C;
        s->remaining -= 0x11C;
        s->cur += 0x11C;

        fn_8009F190((int*)rec, (int*)agent);
        rec->x = agent->f24;
        rec->y = agent->f28;
        rec->z = agent->f2c;
        rec->unk18 = agent->f128;
        rec->unk1c = agent->f12c;
        rec->state = agent->state;
        rec->unk24 = (int)agent->f904;
        rec->f104 = ActorIsActive((Actor*)agent);

        memcpy(rec->rest, agent->f214, 0x2C);
        memcpy(rec->rest + 0x2C, agent->f240, 0x30);
        memcpy(rec->rest + 0x5C, agent->f270, 0x50);
        memcpy(rec->rest + 0xAC, (unsigned char*)agent + 0x5A0, 0x10);

        rec->fE4 = agent->f4bc;
        rec->fE8 = agent->f4c0;
        rec->fEC = agent->f4c4;
        rec->fF0 = agent->f4c8;
        rec->fF4 = agent->f4cc;
        rec->fF8 = agent->f4d0;
        rec->fFC = agent->f4d4;
        rec->f100 = agent->f4d8;

        if (agent->f8ec != -1) {
            count = fn_800F81EC(agent->f8ec);
            rec->f108 = count;
            for (entry = 0; entry < count; entry++) {
                int handle;
                handle = fn_800F8130(agent->f8ec, entry);
                name[0] = fn_800F8AF4(handle);
                name[1] = fn_800F7D28(agent->f8ec, handle);
                AINeoStreamWrite(s, name, 8);
            }
        }

        rec->f10C = agent->f894;
        rec->f110 = agent->f898;
        rec->f114 = agent->f89c;
        rec->f118 = agent->f8a0;

        if (agent->f120 != 0) {
            s->owner = (int*)agent;
            ((void (*)(int, void*))agent->f120)(0x7102, s);
        }
        if (agent->f124 != 0) {
            s->owner = (int*)agent;
            ((void (*)(int, void*))agent->f124)(0x7102, s);
        }

        // A type-2 sub-record holds either -1 or an owner object; the owner is
        // asked for the handle that the loader later resolves again.
        if (*(int*)(rec->rest + 0x0C) == 2) {
            int owner = *(int*)(rec->rest + 0x18);
            if (owner == 0) {
                *(int*)(rec->rest + 0x18) = -1;
            } else {
                int msg[2];
                msg[0] = owner;
                ((void (*)(int, int*))(*(void**)((unsigned char*)(unsigned long)owner + 0x30)))(0x400, msg);
                *(int*)(rec->rest + 0x18) = msg[1];
            }
        }
        if (*(int*)(rec->rest + 0x7C) == 2) {
            int owner = *(int*)(rec->rest + 0xA0);
            if (owner == 0) {
                *(int*)(rec->rest + 0xA0) = -1;
            } else {
                int msg[2];
                msg[0] = owner;
                ((void (*)(int, int*))(*(void**)((unsigned char*)(unsigned long)owner + 0x30)))(0x400, msg);
                *(int*)(rec->rest + 0xA0) = msg[1];
            }
        }
        if (rec->fF0 == 2) {
            if (rec->fE4 == 0) {
                rec->fE4 = -1;
            } else {
                rec->fE4 = *(int*)((unsigned char*)(unsigned long)rec->fE4 + 0x38);
            }
        }
    }
    return 1;
}

// Definition record of one AI agent as it arrives from the agent table.
// The three angles at 0x18 are single-precision floats; the 0x10-byte tail at
// 0x24 is copied verbatim into the runtime slot.
struct NeoAgentDef {
    int unk00;
    int unk04;
    int unk08;
    int unk0c;
    int unk10;
    int unk14;
    float f18;
    float f1c;
    float f20;
    unsigned char ext[0x10];
};

// Per-slot save record kept for every dynamically allocated agent.
struct NeoAgentSlot {
    int unk00;
    int unk04;
    int unk08;
    int unk0c;
    int unk10;
    int unk14;
    float f18;
    float f1c;
    float f20;
    unsigned char ext[0x10];
};

// Allocates a dynamic agent slot for the definition record `def`, fills the
// per-slot save record and hands the new agent to the agent system.  `time`
// (when given) overrides the agent's timestamp.  Returns the agent's 24-bit
// handle when it is in the "active" state, or -1.
// fn_800A2DE8
extern "C" int fn_800A2DE8(int* def, unsigned int* time)
{
    NeoAgentDef* record;
    NeoAgentSlot* slot;
    NeoAgent* agent;
    unsigned char* table;
    unsigned int state;
    int slotId;
    int found;
    int i;

    if (def == 0) {
        return 0xFFFFFFFF;
    }
    record = (NeoAgentDef*)def;

    table = lbl_804D47D4;
    found = 0;
    for (i = 0; i < lbl_804D47CC; i++) {
        if (record->unk00 == *(int*)table) {
            found = 1;
            break;
        }
        table += 0x178;
    }
    if (!found) {
        return 0xFFFFFFFF;
    }

    slotId = BitsetFirstClear((unsigned long*)lbl_804D47E0, 1, 0x10);
    if (slotId == -1) {
        return 0xFFFFFFFF;
    }
    lbl_804D47C8 = lbl_804D47C8 + 1;
    lbl_804D47E0[slotId / 32] |= 1 << (slotId & 31);

    slot = (NeoAgentSlot*)(lbl_804490AC + slotId * 0x34);
    slot->unk00 = record->unk00;
    slot->unk04 = record->unk04;
    slot->unk08 = record->unk08;
    slot->unk0c = record->unk0c;
    slot->f18 = record->f18;
    slot->f1c = record->f1c;
    slot->f20 = record->f20;
    fn_8016D68C(slot->ext, record->ext);
    ((void (*)(int, void*))fn_800A6198)(lbl_804D47C4 + slotId, slot);

    agent = (NeoAgent*)((unsigned char*)lbl_804D47D0 +
                        (lbl_804D47C4 + slotId) * 0x928);
    if (time != 0) {
        agent->f904 = (float)*time;
    }

    // Only agents whose handle is in the "active" state (0x13) and still live
    // are reported back to the caller.
    state = *(unsigned int*)((unsigned char*)agent + 0x38);
    if ((state >> 24) != 0x13) {
        return 0xFFFFFFFF;
    }
    if (!fn_800A1F94(state & 0xFFFFFF)) {
        return 0xFFFFFFFF;
    }
    return state;
}

void BitClear(unsigned long* set, int bit);
void NeoAgentRelease(void* agent);

// Release one dynamic agent slot: tear the agent down, clear its bit in the
// dynamic-agent bitmap and drop the live-agent count. Returns 1.
// fn_800A31A0
extern "C" int fn_800A31A0(NeoAgent* agent)
{
    int slot;

    NeoAgentRelease(agent);
    slot = ((int)agent - (int)lbl_804D47D0) / 0x928;
    BitClear((unsigned long*)lbl_804D47E0, slot - lbl_804D47C4);
    lbl_804D47C8--;
    return 1;
}

extern "C" int fn_802A1D04(void);
extern "C" void fn_801D55CC(void* p);
extern "C" void* memset(void* dst, int value, unsigned long size);
extern const float lbl_804D6BD0;
void VFree(void* p);
void DebugPrintf(const char* fmt, ...);
void ActorPlayAnim(int id, unsigned long anim, int flag);

// Replace a dead agent: when the agent is still active, in state 4 or 6, and its
// definition record is still resident, a new agent is created from that
// definition and every per-agent field is copied over to it. The original slot
// is then released and every owner reference to it is invalidated. Returns 1.
// fn_800A3210
extern "C" int fn_800A3210(NeoAgent* agent)
{
    NeoAgentDef def;
    NeoAgent* dst;
    unsigned int id;
    int handle;
    int i;
    int offset;
    int msg[4];
    int* s;
    int* d;
    float* sf;
    float* df;

    if (fn_802A1D04() == 0 && ActorIsActive((Actor*)agent) != 0 &&
        (*(int*)((char*)agent + 0x8F8) == 4 ||
         *(int*)((char*)agent + 0x8F8) == 6)) {
        memset(&def, 0, 0x34);
        def.unk00 = *(int*)(*(int**)((char*)agent + 0xC8));
        def.f18 = *(float*)((char*)agent + 0x84);
        def.f1c = *(float*)((char*)agent + 0x88);
        def.f20 = *(float*)((char*)agent + 0x8C);
        handle = fn_800A2DE8((int*)&def, 0);
        if (handle != -1) {
            id = (unsigned int)handle & 0xFFFFFF;
            dst = fn_800A1F94((int)id)
                      ? (NeoAgent*)((char*)lbl_804D47D0 + id * 0x928)
                      : 0;
            s = (int*)agent;
            d = (int*)dst;
            sf = (float*)agent;
            df = (float*)dst;

            d[0x23E] = s[0x23E];
            d[0x00] = s[0x00];
            d[0x01] = s[0x01];
            d[0x02] = s[0x02];
            d[0x03] = s[0x03];
            d[0x04] = s[0x04];
            d[0x05] = s[0x05];
            d[0x06] = s[0x06];
            d[0x07] = s[0x07];
            d[0x08] = s[0x08];
            d[0x09] = s[0x09];
            d[0x0A] = s[0x0A];
            d[0x0B] = s[0x0B];
            d[0x90] = s[0x90];
            d[0x91] = s[0x91];
            d[0x92] = s[0x92];
            d[0x93] = s[0x93];
            d[0x94] = s[0x94];
            d[0x95] = s[0x95];
            df[0x96] = sf[0x96];
            df[0x97] = sf[0x97];
            df[0x98] = sf[0x98];
            df[0x99] = sf[0x99];
            d[0x9A] = s[0x9A];
            d[0x9B] = s[0x9B];
            d[0x9C] = s[0x9C];
            d[0x9D] = s[0x9D];
            d[0x9E] = s[0x9E];
            d[0x9F] = s[0x9F];
            d[0xA0] = s[0xA0];
            d[0xA1] = s[0xA1];
            d[0xA2] = s[0xA2];
            df[0xA3] = sf[0xA3];
            d[0xA4] = s[0xA4];
            d[0xA5] = s[0xA5];
            d[0xA6] = s[0xA6];
            d[0xA7] = s[0xA7];
            d[0xA8] = s[0xA8];
            d[0xA9] = s[0xA9];
            df[0xAA] = sf[0xAA];
            df[0xAB] = sf[0xAB];
            df[0xAC] = sf[0xAC];
            d[0xAD] = s[0xAD];
            d[0xAE] = s[0xAE];
            d[0xAF] = s[0xAF];
            df[0x23F] = sf[0x23F];
            df[0x241] = lbl_804D6BD0;

            if (*(int*)((char*)dst + 0xF0) != 0) {
                VFree(*(void**)((char*)dst + 0xF0));
            }
            *(int*)((char*)dst + 0xF0) = *(int*)((char*)agent + 0xF0);
            *(int*)((char*)agent + 0xF0) = 0;
            *(int*)((char*)dst + 0xD0) = *(int*)((char*)agent + 0xD0);

            handle = *(int*)((char*)dst + 0xCC);
            *(int*)((char*)dst + 0xCC) = *(int*)((char*)agent + 0xCC);
            *(int*)((char*)agent + 0xCC) = handle;

            if (*(int*)((char*)dst + 0x5F4) != 0) {
                fn_801D55CC(*(void**)((char*)dst + 0x5F4));
                *(int*)((char*)dst + 0x5F4) = 0;
            }
            if (*(int*)((char*)dst + 0x5F0) != 0 &&
                *(int*)((char*)agent + 0x5F0) != 0) {
                int* pd;
                int* ps;

                pd = *(int**)((char*)dst + 0x5F0);
                ps = pd;
                *(int**)((char*)dst + 0x5F0) =
                    *(int**)((char*)agent + 0x5F0);
                *(int**)((char*)agent + 0x5F0) = ps;

                pd = *(int**)((char*)dst + 0x5F0);
                pd[0] = *(int*)((char*)dst + 0x38);
                pd = *(int**)((char*)dst + 0x5F0);
                pd[4] = *(int*)((char*)dst + 0xF0);
                pd = *(int**)((char*)dst + 0x5F0);
                pd[1] = *(int*)((char*)dst + 0xCC);

                ps = *(int**)((char*)agent + 0x5F0);
                ps[0] = *(int*)((char*)agent + 0x38);
                ps = *(int**)((char*)agent + 0x5F0);
                ps[1] = *(int*)((char*)agent + 0xCC);
                ps = *(int**)((char*)agent + 0x5F0);
                ps[2] = 0;
                ps = *(int**)((char*)agent + 0x5F0);
                ps[4] = 0;
                ps = *(int**)((char*)agent + 0x5F0);
                ps[6] = 0;
            }

            DebugPrintf("copying dead guy %08x to %08x ",
                        *(int*)((char*)agent + 0x38),
                        *(int*)((char*)dst + 0x38));
        }
    }

    NeoAgentRelease(agent);
    fn_800A6198(((int)agent - (int)lbl_804D47D0) / 0x928,
                *(void**)((char*)agent + 0xC8));

    offset = 0;
    for (i = 0; i < lbl_804D47C4 + 0x10; i++) {
        if (fn_800A1F94(i | 0x13000000)) {
            int* owners;

            owners = (int*)((char*)lbl_804D47D0 + offset + 0x1E4);
            if (*(int*)((char*)agent + 0x38) == owners[0])
                owners[0] = -1;
            if (*(int*)((char*)agent + 0x38) == owners[3])
                owners[3] = -1;
            if (*(int*)((char*)agent + 0x38) == owners[6])
                owners[6] = -1;
            if (*(int*)((char*)agent + 0x38) == owners[9])
                owners[9] = -1;
        }
        offset += 0x928;
    }

    msg[1] = 0;
    msg[2] = *(int*)((char*)agent + 0x38);
    msg[3] = 0x6C345908;
    SendMsg(0x4B000000, 0xC14, msg);
    ActorPlayAnim(*(int*)((char*)agent + 0x38), 0xD3EE4649,
                  *(int*)((char*)agent + 0x38));
    return 1;
}

extern "C" int fn_800BA7FC(int id);
extern unsigned int lbl_804D47DC;    // id of the currently controlled agent slot

// Whether the given agent is the one the game currently controls: the slot
// must match lbl_804D47DC, and the game must report a valid controlled state.
int NeoAgentIsCurrent(int agent)
{
    int slot;

    if (fn_800BA7FC(0x33) != 0) {
        slot = (agent - (int)lbl_804D47D0) / 0x928;
        return slot == lbl_804D47DC;
    }
    return 1;
}

// Whether the given agent slot is the currently controlled one.
int NeoAgentIsCurrentSlot(int agent)
{
    int slot;

    slot = (agent - (int)lbl_804D47D0) / 0x928;
    return slot == lbl_804D47DC;
}

// Searches the global AI agent definition table for the record whose leading
// key word matches `key`.  Each record is 0x178 bytes.  Returns the matching
// record or null when the key is not registered.
unsigned char* NeoAgentFindRecord(void* key)
{
    unsigned char* record = lbl_804D47D4;
    int i = 0;
    int count = lbl_804D47CC;

    if (0 < count) {
        do {
            if (key == *(void**)record) {
                return lbl_804D47D4 + i * 0x178;
            }
            count--;
            record += 0x178;
            i++;
        } while (count != 0);
    }
    return 0;
}

// Returns the pointer kept at offset 0xC4 of `obj` (callers treat it as the
// per-object host/attachment record).
// fn_800A384C
extern "C" void* fn_800A384C(void* obj)
{
    return *(void**)((unsigned char*)obj + 0xC4);
}

// Number of dynamic agent slots currently allocated; the static slots below
// lbl_804D47C4 are not counted.
// fn_800A3854
extern "C" int fn_800A3854()
{
    return lbl_804D47C8;
}

extern unsigned int GameEvent(int from, int event);

// Scans the agent slots for a live agent whose game-event record (queried with
// GameEvent(-1, slot)) equals `event`.  Each slot's id word at 0x38 carries the
// handle in its low 24 bits, tagged 0x13 in the top byte.  Returns the slot's id
// word, or -1 when no live slot reports that event.
unsigned int AINeoAgentFindByEvent(unsigned int event)
{
    int i = 0;
    int offset = 0;

    while (i < lbl_804D47C4) {
        unsigned int slot = *(unsigned int*)((char*)lbl_804D47D0 + offset + 0x38);
        int ok;

        if ((slot >> 24) != 0x13) {
            ok = 0;
        } else {
            int id = slot & 0xFFFFFF;
            if (lbl_804D47D0 == 0) {
                ok = 0;
            } else if (id < 0 || id > lbl_804D47C4 + 0x10) {
                ok = 0;
            } else if (id < lbl_804D47C4 ||
                       (lbl_804D47E0[(id - lbl_804D47C4) / 32] &
                        (1 << ((id - lbl_804D47C4) & 31)))) {
                ok = *(int*)((char*)lbl_804D47D0 + id * 0x928 + 0x8F8) != 7;
            } else {
                ok = 0;
            }
        }
        if (!ok)
            slot = 0xFFFFFFFF;
        if (GameEvent(-1, slot) == event)
            return slot;
        offset += 0x928;
        i++;
    }
    return 0xFFFFFFFF;
}

// Typed view of the AI state word at offset 0x8F8 of an agent slot.
struct SlotStateView {
    unsigned char pad[0x8F8];
    int state;
};

// Returns the agent slot pointer for a 24-bit agent handle when the slot is
// live, or null when the handle is out of range or the agent is dead.
// fn_800A3990
extern "C" void* fn_800A3990(int handle)
{
    int id = handle & 0xFFFFFF;
    int live;

    if (lbl_804D47D0 == 0) {
        live = 0;
    } else if (id < 0 || id > lbl_804D47C4 + 0x10) {
        live = 0;
    } else if (id >= lbl_804D47C4 &&
               !(lbl_804D47E0[(id - lbl_804D47C4) / 32] &
                 (1 << ((id - lbl_804D47C4) & 31)))) {
        live = 0;
    } else if (((SlotStateView*)((char*)lbl_804D47D0 + id * 0x928))->state == 7) {
        live = 0;
    } else {
        live = 1;
    }
    if (!live) {
        return 0;
    }
    return (char*)lbl_804D47D0 + id * 0x928;
}

// Returns the 24-bit agent handle stored in an owner's id word at offset 0x38,
// or -1 when the word does not name a live agent.
unsigned int OwnerId(Owner* owner)
{
    unsigned int slot = *(unsigned int*)((unsigned char*)owner + 0x38);
    int id;
    int live;

    if ((slot >> 24) != 0x13) {
        return 0xFFFFFFFF;
    }
    id = slot & 0xFFFFFF;
    if (lbl_804D47D0 == 0) {
        live = 0;
    } else if (id < 0 || id > lbl_804D47C4 + 0x10) {
        live = 0;
    } else if (id < lbl_804D47C4 ||
               (lbl_804D47E0[(id - lbl_804D47C4) / 32] &
                (1 << ((id - lbl_804D47C4) & 31)))) {
        live = *(int*)((char*)lbl_804D47D0 + id * 0x928 + 0x8F8) != 7;
    } else {
        live = 0;
    }
    if (!live) {
        return 0xFFFFFFFF;
    }
    return slot;
}

// ---------------------------------------------------------------------------
// Per-frame AI agent update.
//
// The agent slot as seen by the update: the flag word at 0x4C selects the
// branch (0x400 marks a freed slot), 0x84/0x88/0x8C and 0x24C/0x250/0x254 are
// the two position triples reported by the debug keys, 0x13C is the flatline
// timer, 0x8F8 the AI state and 0x914 a per-frame scratch word.
struct AgentSlot {
    unsigned char pad00[0x24];
    int ix;                        // 0x24
    int iy;                        // 0x28
    int iz;                        // 0x2C
    unsigned char pad30[0x08];
    unsigned int id;               // 0x38
    unsigned char pad3C[0x10];
    unsigned int flags;            // 0x4C
    unsigned char pad50[0x34];
    float posX;                    // 0x84
    float posY;                    // 0x88
    float posZ;                    // 0x8C
    unsigned char pad90[0x3C];
    int fCC;                       // 0xCC
    int fD0;                       // 0xD0
    unsigned char padD4[0x18];
    int fEC;                       // 0xEC
    int fF0;                       // 0xF0
    unsigned char padF4[0x08];
    void (*cbFC)(AgentSlot*);      // 0xFC
    unsigned char pad100[0x04];
    void (*cb104)(AgentSlot*);     // 0x104
    unsigned char pad108[0x08];
    void (*cb110)(AgentSlot*);     // 0x110
    void (*cb114)(AgentSlot*);     // 0x114
    unsigned char pad118[0x10];
    int f128;                      // 0x128
    unsigned char pad12C[0x0C];
    float f138;                    // 0x138
    float f13C;                    // 0x13C
    unsigned char pad140[0xE0];
    int f220;                      // 0x220
    unsigned char pad224[0x1C];
    float velX;                    // 0x240
    float velY;                    // 0x244
    float velZ;                    // 0x248
    float f24C;                    // 0x24C
    float f250;                    // 0x250
    float f254;                    // 0x254
    unsigned char pad258[0x18];
    unsigned char sub270[0x50];    // 0x270
    unsigned char pad2C0[0x330];
    void* extra;                   // 0x5F0
    unsigned char pad5F4[0x304];
    int state;                     // 0x8F8
    float f8FC;                    // 0x8FC
    unsigned char pad900[0x14];
    int f914;                      // 0x914
    float f91C;                    // 0x91C
};

// Default state tallies, copied into a local each frame.
struct AgentStateCounts {
    int v[8];
};

extern AgentStateCounts lbl_803443A0;
extern const char lbl_803443C0[];   // state names, short form (8 pointers)
extern const char lbl_803443E0[];   // state names, print form (8 pointers)
extern const char lbl_803444D1[];   // "%0.02f %0.02f %0.02f"
extern const char lbl_803444E7[];   // "%s:  %i"
extern const char lbl_803444EB[];   // "%s:  %i"

extern int lbl_804D4798;            // global AI detail level
extern int lbl_804D479C;            // detail level at the last switch
extern int lbl_804D47C8;            // number of dynamic agents
extern unsigned int lbl_804D47DC;   // round-robin agent cursor
extern int lbl_80449098[];          // per-agent allocated bitmap
extern const float lbl_804D6BD4;          // flatline threshold

extern const float lbl_804D6BC8;          // 2^31, constant used by the raw hits
extern const float lbl_804D6BD8;          // initial flatline timer
extern const float lbl_804D6BDC;          // frame speed divisor
extern const float lbl_804D6BE0;          // flatline timer after an RTLP message
extern const float lbl_804D6BE4;          // damage amount for the ragdoll push
extern const float lbl_804D6BE8;          // damage scale when the target is an agent
extern char lbl_80393D7C[];         // RTLP message header marker

extern "C" int fn_800BA7F4(void);
extern "C" int fn_800BA7FC(int event);
extern "C" int fn_800BA84C(int event);
extern "C" void fn_800C5654(int a, int b, int c);
extern "C" void fn_801EA9F0(const char* fmt, ...);

// Agents, animation sub-objects and the RTLP message helpers used by the
// per-agent update. The latter two take their float argument through the
// variable part of the list, hence the double parameter.
extern "C" int fn_800A7B78(void* sub, int id);
extern "C" int fn_800A7AC4(void* sub, int id);
extern "C" void fn_800A7A54(void* sub, int id, int value);
extern "C" void fn_800A8500(AgentSlot* agent);
extern "C" void fn_800C99E8(double amount, void* agent, int a, int b, int c,
                            int d);
extern "C" float fn_802A2080(void);
extern "C" int fn_802A1D04(void);
extern "C" int fn_8020B97C(void);
extern "C" void* fn_80049948(void);
extern "C" int fn_80049940(void);
extern "C" int fn_80212730(unsigned int id);
extern "C" void fn_80230B1C(double value, ...);
extern "C" void fn_80230F08(void* extra);
extern "C" int fn_80230450(void* extra = 0);
extern "C" void fn_8028C500(void* msg);
extern "C" void fn_8028B9C0(void* msg, int value);
extern "C" void fn_800265D4(unsigned int id, int type, void* msg);
extern "C" void fn_80025CFC(void* bitmap, int index);
extern "C" int fn_80022D80(AgentSlot* agent);
extern "C" void fn_80022D98(AgentSlot* agent, int value);
extern "C" void fn_80022E8C(AgentSlot* agent);
extern "C" int fn_800B26B8(AgentSlot* agent);
extern "C" int fn_800B22F8(AgentSlot* agent);
extern "C" void fn_80082118(unsigned int handle, int a, int b, int c);
extern "C" void* fn_8003AA9C(int a, int b, int c);
extern "C" void* fn_8003A1F0(int a, int b, void* c);
extern "C" void fn_800FC764(unsigned int handle, int value);

// Per-state activation used by AI states 1 and 2; the body lives further down
// the unit and is not written yet.
void NeoAgentStateActivate(AgentSlot* agent);

int NeoAgentUpdate(AgentSlot* agent, float dt);

// Turn an agent handle word into the handle to report, or -1 when it does not
// name a live agent.
static inline unsigned int AgentLiveHandle(unsigned int handle)
{
    unsigned int n;
    bool live;

    if ((handle >> 24) != 0x13)
        return 0xFFFFFFFF;
    n = handle & 0xFFFFFF;
    if (lbl_804D47D0 == 0) {
        live = false;
    } else if ((int)n > lbl_804D47C4 + 0x10) {
        live = false;
    } else if ((int)n < lbl_804D47C4 ||
               (lbl_804D47E0[(n - lbl_804D47C4) / 32] &
                (1u << ((n - lbl_804D47C4) & 31)))) {
        live = ((AgentSlot*)((char*)lbl_804D47D0 + n * 0x928))->state != 7;
    } else {
        live = false;
    }
    return live ? handle : 0xFFFFFFFF;
}

// One frame of the AI system: update every allocated agent, tally the agents
// per AI state, report the selected agent's position and state when the debug
// keys ask for it, advance the round-robin cursor and keep the global detail
// level's screen filter in sync.
void NeoAgentFrameUpdate(float dt)
{
    AgentStateCounts counts;
    AgentSlot* agent;
    int i;
    int index;
    unsigned int next;

    counts = lbl_803443A0;
    lbl_804D4798 = 4;
    if (!fn_800BA7F4())
        return;
    if (lbl_804D47C8 > 0x90)
        return;

    index = 0;
    for (i = 0; i < lbl_804D47C4 + 0x10; i++) {
        agent = (AgentSlot*)((char*)lbl_804D47D0 + index);
        agent->f914 = 0;
        if ((agent->flags & 0x400) == 0) {
            NeoAgentUpdate(agent, dt);
            if (fn_800BA7FC(0x34)) {
                int state = agent->state;
                if (state == 2) {
                    if (lbl_804D6BD4 <= agent->f13C)
                        counts.v[2]++;
                    else
                        counts.v[1]++;
                } else if ((unsigned int)state < 8) {
                    counts.v[state]++;
                }
            }
        } else {
            agent->flags &= ~0x400u;
        }
        index += 0x928;
    }

    index = fn_800BA84C(4);
    if (index >= 0 && index < lbl_804D47C4 + 0x10) {
        agent = (AgentSlot*)((char*)lbl_804D47D0 + index * 0x928);
        if (fn_800BA7FC(0x38)) {
            fn_801EA9F0(lbl_803444D1, (double)agent->posX, (double)agent->posY,
                        (double)agent->posZ);
        }
        if (fn_800BA7FC(0x39)) {
            fn_801EA9F0(lbl_803444D1, (double)agent->f24C, (double)agent->f254,
                        (double)agent->f250);
        }
        if (fn_800BA7FC(0x3A)) {
            fn_801EA9F0(lbl_803444E7, &lbl_803443C0[agent->state * 4],
                        agent->state);
        }
    }

    next = lbl_804D47DC;
    if (lbl_804D47C8 > 0) {
        int count = lbl_804D47C4 + 0x10;
        unsigned int cur = (lbl_804D47DC + 1) % count;
        while (cur != lbl_804D47DC) {
            next = cur;
            if (lbl_80449098[(int)cur / 32] & (1 << ((int)cur & 31)))
                break;
            cur = (cur + 1) % count;
        }
    }
    lbl_804D47DC = next;

    if (lbl_804D4798 < 2 && lbl_804D479C > 1)
        fn_800C5654(0x2000000, 0xd4025a1b, 0x30000000);
    if (lbl_804D4798 != lbl_804D479C) {
        lbl_804D479C = lbl_804D4798;
        switch (lbl_804D4798) {
        case 0:
            fn_800C5654(0x23000000, 0xb0f4705f, 0x13000000);
            break;
        case 1:
            fn_800C5654(0x23000000, 0x34b5171a, 0x13000000);
            break;
        case 2:
            fn_800C5654(0x23000000, 0x3e2ebe98, 0x13000000);
            break;
        case 3:
            fn_800C5654(0x23000000, 0x82640e55, 0x13000000);
            break;
        case 4:
            fn_800C5654(0x23000000, 0x6af713df, 0x13000000);
            break;
        }
    }

    if (fn_800BA7FC(0x34)) {
        for (i = 0; i < 8; i++)
            fn_801EA9F0(lbl_803444EB, &lbl_803443E0[i * 4], counts.v[i]);
    }
}

// One agent's per-frame AI update. Returns 1 when the agent was processed, 0
// when its slot is not live or the debug filter excludes it.
int NeoAgentUpdate(AgentSlot* agent, float dt)
{
    int index;
    bool active;
    bool dying;
    float delta;
    float speed;
    float cap;
    void* obj;
    void* place;
    unsigned int handle;
    int iv;

    if (!fn_800BA7F4())
        return 0;
    index = ((char*)agent - (char*)lbl_804D47D0) / 0x928;
    if (index < 0)
        return 0;
    if (index >= lbl_804D47C4) {
        unsigned int bit = (unsigned int)(index - lbl_804D47C4);
        if (!(lbl_804D47E0[bit / 32] & (1u << (bit & 31))))
            return 0;
    }

    active = false;
    if (agent != 0) {
        if ((agent->flags & 0x40) != 0 || (agent->flags & 1) != 0 ||
            (agent->id >> 24) == 0xC) {
            active = true;
        }
    }

    if (active) {
        if (fn_800BA7FC(0x37) &&
            (agent->id & 0xFFFFFF) != (unsigned int)fn_800BA84C(4)) {
            return 0;
        }
        if (fn_800A7B78(agent->sub270, 0x29))
            fn_800C99E8((double)lbl_804D6BE4, agent, 0x19, 100, 5, 5);

        if ((agent->flags & 1) == 0) {
            delta = dt;
            if ((agent->flags & 0x20000) == 0)
                delta = dt * fn_802A2080();
            speed = delta * agent->f91C;
            if (fn_802A1D04() == 0) {
                fn_80049948();
                if (fn_8020B97C() != 0) {
                    obj = fn_80049948();
                    if (*(int*)((char*)obj + 0x934) != (int)agent->id)
                        speed = speed * lbl_804D6BE8;
                }
            }

            if (agent->fEC == 10 && agent->f128 == 0x276198) {
                unsigned int msgOut[5];
                int msgHdr[3];
                void* sent;

                handle = agent->id;
                fn_8028C500(msgOut);
                msgOut[0] = 0;
                msgHdr[0] = 0xdc13f8ab;
                msgHdr[1] = 0;
                fn_800265D4(handle, 0x41e, msgHdr);
                sent = (void*)msgOut[0];
                msgOut[0] = (unsigned int)lbl_80393D7C;
                fn_8028B9C0(msgOut, 0);
                if (sent == 0)
                    agent->f138 = speed;
                else
                    agent->f138 = lbl_804D6BE0;
            } else {
                agent->f138 = speed;
            }
            agent->f13C = agent->f13C + delta;

            if ((unsigned int)agent->state < 8) {
                switch ((unsigned int)agent->state) {
                case 0:
                    fn_80025CFC(lbl_80449098, index);
                    if (fn_80022D80(agent))
                        fn_80022D98(agent, 0);
                    break;
                case 1:
                    lbl_80449098[index / 32] |= 1 << (index & 31);
                    NeoAgentStateActivate(agent);
                    iv = fn_800B26B8(agent);
                    if (iv < 4 && (iv = fn_800B22F8(agent)) < lbl_804D4798)
                        lbl_804D4798 = iv;
                    break;
                case 2:
                    iv = fn_800B26B8(agent);
                    if (agent->f13C < lbl_804D6BD4 || iv != 4) {
                        NeoAgentStateActivate(agent);
                        dying = true;
                    } else {
                        if (agent->cbFC != 0)
                            agent->cbFC(agent);
                        fn_80022E8C(agent);
                        dying = false;
                    }
                    if (dying) {
                        lbl_80449098[index / 32] |= 1 << (index & 31);
                        iv = fn_800B26B8(agent);
                        if (iv < 4 && (iv = fn_800B22F8(agent)) < lbl_804D4798)
                            lbl_804D4798 = iv;
                    } else {
                        fn_80025CFC(lbl_80449098, index);
                    }
                    break;
                case 3:
                    if (agent->cb110 != 0)
                        agent->cb110(agent);
                    if (agent->extra != 0)
                        fn_80230B1C((double)agent->f138);
                    if (fn_800A7AC4(agent->sub270, 0x1B) == 0)
                        fn_800A8500(agent);
                    fn_80022E8C(agent);
                    fn_80025CFC(lbl_80449098, index);
                    break;
                case 4:
                    agent->f8FC = agent->f8FC + delta;
                    if (agent->cb104 != 0)
                        agent->cb104(agent);
                    if (agent->cb110 != 0)
                        agent->cb110(agent);
                    if (agent->extra == 0 || fn_80212730(agent->id) != -1) {
                        speed = agent->f138 / lbl_804D6BDC;
                        agent->posX = agent->velX * speed + agent->posX;
                        agent->posY = agent->velY * speed + agent->posY;
                        agent->posZ = agent->velZ * speed + agent->posZ;
                        agent->ix = (int)agent->posX;
                        agent->iy = (int)agent->posY;
                        agent->iz = (int)agent->posZ;
                    } else {
                        fn_80230B1C((double)agent->f138, agent->extra);
                        agent->posX = (float)agent->ix;
                        agent->posY = (float)agent->iy;
                        agent->posZ = (float)agent->iz;
                        if (fn_802A1D04() == 0 &&
                            fn_80230450(agent->extra) == 1) {
                            cap = lbl_804D6BDC;
                            if (agent->f8FC < lbl_804D6BDC)
                                cap = agent->f8FC;
                            agent->f8FC = cap;
                        }
                    }
                    if ((float)(unsigned int)fn_800BA84C(7) <= agent->f8FC) {
                        agent->f8FC = lbl_804D6BE0;
                        agent->state = 6;
                        if (fn_800A7AC4(agent->sub270, 10) == 0) {
                            fn_800A7A54(agent->sub270, 10, 1);
                            fn_800C5654(agent->id, 0xdc04e3d5, agent->id);
                        }
                    } else if ((float)(unsigned int)fn_800BA84C(8) <=
                               agent->f13C) {
                        agent->f8FC = lbl_804D6BE0;
                        agent->state = 6;
                        if (fn_800A7AC4(agent->sub270, 10) == 0) {
                            fn_800A7A54(agent->sub270, 10, 1);
                            fn_800C5654(agent->id, 0xdc04e3d5, agent->id);
                        }
                    }
                    if (fn_800A7AC4(agent->sub270, 0x1B) == 0)
                        fn_800A8500(agent);
                    fn_80022E8C(agent);
                    fn_80025CFC(lbl_80449098, index);
                    break;
                case 5:
                    if (agent->cb114 != 0)
                        agent->cb114(agent);
                    fn_80025CFC(lbl_80449098, index);
                    break;
                case 6:
                    agent->f8FC = agent->f8FC + delta;
                    if ((float)(unsigned int)fn_800BA84C(9) <= agent->f8FC) {
                        if (fn_80022D80(agent)) {
                            fn_80022D98(agent, 0);
                            if (agent->extra != 0 &&
                                fn_80230450() != 0) {
                                fn_80230F08(agent->extra);
                            }
                            if (agent->id != 0xFFFFFFFF) {
                                int msg[3];
                                msg[0] = 0;
                                msg[1] = (int)agent->id;
                                msg[2] = 0xfcfca566;
                                fn_800265D4(agent->id, 0xc14, msg);
                            }
                            iv = fn_802A1D04();
                            if (iv == 0) {
                                if (lbl_804D47C4 <=
                                    (int)(agent->id & 0xFFFFFF)) {
                                    NeoAgentRelease(agent);
                                    iv = ((char*)agent -
                                          (char*)lbl_804D47D0) / 0x928;
                                    fn_80025CFC((int*)lbl_804D47E0,
                                                iv - lbl_804D47C4);
                                    lbl_804D47C8--;
                                }
                            } else if (iv > -1 && iv < 4) {
                                fn_800FC764(AgentLiveHandle(agent->id), 0);
                            }
                        }
                    } else if (agent->extra != 0 &&
                               fn_80212730(agent->id) == -1) {
                        fn_80230B1C((double)agent->f138, agent->extra);
                        agent->posX = (float)agent->ix;
                        agent->posY = (float)agent->iy;
                        agent->posZ = (float)agent->iz;
                    }
                    fn_80025CFC(lbl_80449098, index);
                    break;
                case 7:
                    fn_80025CFC(lbl_80449098, index);
                    break;
                }
            } else {
                fn_80025CFC(lbl_80449098, index);
            }
            if (fn_80049940() != 0)
                agent->fEC = 10;
            return 1;
        }

        // Reset path: rebuild the placement from the agent's integer position.
        agent->f138 = dt;
        handle = AgentLiveHandle(agent->id);
        fn_80082118(handle, agent->fCC, agent->fD0, agent->fF0);
        place = fn_8003AA9C(agent->fCC, 0, 0);
        place = fn_8003A1F0(agent->fCC, agent->fF0, place);
        PlacementToMatrix((Mtx34*)(void*)agent, (PlaceF*)place);
        if (fn_800A7AC4(agent->sub270, 0x1B) == 0)
            fn_800A8500(agent);
        fn_800A7A54(agent->sub270, 0xC, 0);
        fn_800A7A54(agent->sub270, 0xD, 0);
        agent->f220 = 0;
        fn_80022E8C(agent);
        return 1;
    }

    if (agent->state == 4) {
        agent->f8FC = lbl_804D6BD8;
        if (agent->cb104 != 0)
            agent->cb104(agent);
        if (agent->cb110 != 0)
            agent->cb110(agent);
        if (agent->extra == 0 || fn_80212730(agent->id) != -1) {
            speed = agent->f138 / lbl_804D6BDC;
            agent->posX = agent->velX * speed + agent->posX;
            agent->posY = agent->velY * speed + agent->posY;
            agent->posZ = agent->velZ * speed + agent->posZ;
            agent->ix = (int)agent->posX;
            agent->iy = (int)agent->posY;
            agent->iz = (int)agent->posZ;
        } else {
            fn_80230B1C((double)agent->f138, agent->extra);
            agent->posX = (float)agent->ix;
            agent->posY = (float)agent->iy;
            agent->posZ = (float)agent->iz;
            if (fn_802A1D04() == 0 && fn_80230450(agent->extra) == 1) {
                cap = lbl_804D6BDC;
                if (agent->f8FC < lbl_804D6BDC)
                    cap = agent->f8FC;
                agent->f8FC = cap;
            }
        }
        if ((float)(unsigned int)fn_800BA84C(7) <= agent->f8FC) {
            agent->f8FC = lbl_804D6BE0;
            agent->state = 6;
            if (fn_800A7AC4(agent->sub270, 10) == 0) {
                fn_800A7A54(agent->sub270, 10, 1);
                fn_800C5654(agent->id, 0xdc04e3d5, agent->id);
            }
        } else if ((float)(unsigned int)fn_800BA84C(8) <= agent->f13C) {
            agent->f8FC = lbl_804D6BE0;
            agent->state = 6;
            if (fn_800A7AC4(agent->sub270, 10) == 0) {
                fn_800A7A54(agent->sub270, 10, 1);
                fn_800C5654(agent->id, 0xdc04e3d5, agent->id);
            }
        }
        if (fn_800A7AC4(agent->sub270, 0x1B) == 0)
            fn_800A8500(agent);
        fn_80022E8C(agent);
        agent->f8FC = lbl_804D6BD8;
        fn_80022D98(agent, 1);
        if ((float)(unsigned int)fn_800BA84C(9) <= agent->f8FC) {
            if (fn_80022D80(agent)) {
                fn_80022D98(agent, 0);
                if (agent->extra != 0 && fn_80230450() != 0)
                    fn_80230F08(agent->extra);
                if (agent->id != 0xFFFFFFFF) {
                    int msg[3];
                    msg[0] = 0;
                    msg[1] = (int)agent->id;
                    msg[2] = 0xfcfca566;
                    fn_800265D4(agent->id, 0xc14, msg);
                }
                iv = fn_802A1D04();
                if (iv == 0) {
                    if (lbl_804D47C4 <= (int)(agent->id & 0xFFFFFF)) {
                        NeoAgentRelease(agent);
                        iv = ((char*)agent - (char*)lbl_804D47D0) / 0x928;
                        fn_80025CFC((int*)lbl_804D47E0,
                                    iv - lbl_804D47C4);
                        lbl_804D47C8--;
                    }
                } else if (iv > -1 && iv < 4) {
                    fn_800FC764(AgentLiveHandle(agent->id), 0);
                }
            }
        } else if (agent->extra != 0 && fn_80212730(agent->id) == -1) {
            fn_80230B1C((double)agent->f138, agent->extra);
            agent->posX = (float)agent->ix;
            agent->posY = (float)agent->iy;
            agent->posZ = (float)agent->iz;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Dynamic-agent slots: teardown and pool shutdown.
//
// lbl_804D47D0 is the base of the dynamic agent slot array (0x928 bytes per
// slot); the first lbl_804D47C4 slots are static and the following sixteen are
// claimed through the allocated bitmap lbl_804D47E0.
extern int lbl_804D47D8;  // static agent slot count (0 when there are none)

extern "C" int fn_8021284C(void* handle);
extern "C" void fn_801BDDA4(void* agent);
extern "C" void fn_801BE9FC(void* agent);
extern "C" void fn_801626A0(void* agent, void* data);
extern "C" void fn_80117D40(void* agent);
extern "C" void fn_800C9B68(void* agent);
extern "C" void fn_800390F4(void* p);
extern "C" void fn_80090D6C(void* p);
extern "C" void fn_800F86A0(void* p);
extern "C" void fn_8028B35C(void* p, void* q, int n);
extern "C" void fn_800B6EDC(unsigned int id, int flag);
struct ExtraHolder;
void ExtraTrailClear(ExtraHolder* holder);
int ActorFn80022FD8(Actor* actor);
void ActorUnlinkOwnerObjects(Actor* actor);

void NeoAgentRelease(void* agent);

// Release every allocated dynamic agent slot, then free the slot array.
void NeoAgentPoolShutdown(void)
{
    int offset;
    int i;

    if (lbl_804D47D0 != 0) {
        offset = 0;
        for (i = 0; i < lbl_804D47C4 + 0x10; i++) {
            if (i < lbl_804D47C4 ||
                (lbl_804D47E0[(i - lbl_804D47C4) / 32] &
                 (1 << ((i - lbl_804D47C4) & 31)))) {
                NeoAgentRelease((char*)lbl_804D47D0 + offset);
            }
            offset += 0x928;
        }
        VFree(lbl_804D47D0);
    }
    lbl_804D47D0 = 0;
    lbl_804D47C8 = 0;
    lbl_804D47D8 = 0;
    lbl_804D47D4 = 0;
}

// Tear down one dynamic agent slot: its streams, live handle and every owned
// object, then the slot allocator.  Dead agents (state 7) are left alone.
void NeoAgentRelease(void* agent)
{
    NeoAgent* a;
    int* list;
    unsigned int count;
    unsigned int id;

    if (agent == 0) {
        return;
    }
    a = (NeoAgent*)agent;
    fn_8021284C(*(void**)((char*)a + 0x38));
    if (a->state == 7) {
        return;
    }

    if (*(void**)((char*)a + 0xF0) != 0) {
        VFree(*(void**)((char*)a + 0xF0));
        *(void**)((char*)a + 0xF0) = 0;
    }
    fn_801BDDA4(a);
    fn_801BE9FC(a);
    fn_801626A0(a, (char*)a + 0x894);
    fn_80117D40(a);
    fn_800C9B68(a);
    fn_800390F4(*(void**)((char*)a + 0xCC));
    fn_80090D6C(*(void**)((char*)a + 0xD4));
    fn_800F86A0(*(void**)((char*)a + 0x8EC));

    list = *(int**)((char*)a + 0x90C);
    count = (unsigned int)list[2];
    if (count >> 2) {
        fn_8028B35C(list, (void*)list[3], ((int)count / 4) * 4);
    }
    list = *(int**)((char*)a + 0x90C);
    if (list != 0) {
        void** vtbl = *(void***)list;

        ((void (*)(void*, int))vtbl[2])(list, 1);
    }

    id = *(unsigned int*)((char*)a + 0x238);
    if (id != 0xFFFFFFFF && (id >> 24) == 3) {
        fn_800B6EDC(id, 0);
        *(int*)((char*)a + 0x238) = -1;
    }

    if (*(void**)((char*)a + 0x5F0) != 0) {
        ExtraTrailClear((ExtraHolder*)*(void**)((char*)a + 0x5F0));
        VFree(*(void**)((char*)a + 0x5F0));
        *(void**)((char*)a + 0x5F0) = 0;
    }
    if (*(void**)((char*)a + 0x5F4) != 0) {
        fn_801D55CC(*(void**)((char*)a + 0x5F4));
        *(void**)((char*)a + 0x5F4) = 0;
    }
    if (ActorFn80022FD8((Actor*)a) != 0) {
        ActorUnlinkOwnerObjects((Actor*)a);
    }
}

// Heap object whose head is a Buffer: releasing it resets the virtual table,
// runs the base cleanup, destroys the embedded Buffer and, when the caller
// asks for it, returns the storage to VFree.
struct Buffer {
    ~Buffer();
};

struct BufferObject {
    void** vtable;
};

extern void* lbl_8039B67C[];   // virtual table of a buffer-derived object
extern "C" void fn_8028B960(void* p);

void* BufferObjectRelease(void* p, short freeIt)
{
    if (p != 0) {
        ((BufferObject*)p)->vtable = lbl_8039B67C;
        fn_8028B960(p);
        ((Buffer*)p)->~Buffer();
        if (freeIt > 0) {
            VFree(p);
        }
    }
    return p;
}

struct AgentSaveMsg {
    void* slot;
    int value;
};

int SaveIsBusy(void);

// Tells every static agent slot that a save is starting (0x803) or that the
// save is over (0x800), handing it the address of its own slot.
void NeoAgentSaveNotify(void)
{
    AgentSaveMsg msg;
    int offset;
    int type;
    int i;

    type = SaveIsBusy() ? 0x803 : 0x800;
    offset = 0;
    for (i = 0; i < lbl_804D47C4; i++) {
        msg.value = 0;
        msg.slot = (char*)lbl_804D47D0 + offset;
        SendMsg((i & 0xFFFFFF) | 0x13000000, type, &msg);
        offset += 0x928;
    }
}

// ---------------------------------------------------------------------------
// Agent-system start-up state.  These globals live in the shared AI data
// section; this unit only clears and fills them.
extern int lbl_804D47A0;
extern int lbl_804D47A4;
extern int lbl_804D47A8;
extern int lbl_804D47AC;
extern int lbl_804D47B0;
extern int lbl_804D47B4;
extern int lbl_804D47B8;
extern int lbl_804D47BC;
extern int lbl_804D47C0;
extern int lbl_804D47E4;
extern int lbl_804D47E8;
extern int lbl_804D47EC;
extern int lbl_804D47F0;
extern int lbl_804D47F4;
extern int lbl_804D47F8;
extern int lbl_804D47FC;
extern int lbl_804D4800;

// One 0x178-byte AI agent definition record.  The scalar members are copied
// one by one, the two interior arrays with a word-at-a-time block copy.
struct AINeoAgentDef {
    int key;                 // 0x00
    int field04;             // 0x04
    float field08;           // 0x08
    int field0C[11];         // 0x0C
    float field38;           // 0x38
    unsigned int data1[33];  // 0x3C
    float fieldC0;           // 0xC0
    float fieldC4;           // 0xC4
    int fieldC8;             // 0xC8
    int fieldCC;             // 0xCC
    float fieldD0;           // 0xD0
    float fieldD4;           // 0xD4
    float fieldD8;           // 0xD8
    float fieldDC;           // 0xDC
    float fieldE0;           // 0xE0
    int fieldE4;             // 0xE4
    int fieldE8;             // 0xE8
    int fieldEC;             // 0xEC
    int fieldF0;             // 0xF0
    int fieldF4;             // 0xF4
    int fieldF8;             // 0xF8
    int fieldFC;             // 0xFC
    int field100;            // 0x100
    int field104;            // 0x104
    float field108;          // 0x108
    unsigned int data2[27];  // 0x10C
};

// Data object whose second word receives the value looked up in the agent
// definition object list.
struct AINeoDefListObject {
    void* vtable;   // 0x00
    void* value;    // 0x04
};

extern AINeoAgentDef lbl_804493EC;       // copy of the first definition record
extern AINeoDefListObject lbl_8039B650;  // definition-list singleton

void* VAlloc(int size);
void* OLTFindValueByKey(unsigned long key, unsigned long value);

// Clears the agent-system state, allocates the agent pool, snapshots the first
// AI agent definition record and rebuilds the static agent slots.
void NeoAgentSystemCreate(void)
{
    int offset;
    int i;

    lbl_804D47A0 = 0;
    lbl_804D47A4 = 0;
    lbl_804D47AC = 0;
    lbl_804D47E4 = 0;
    lbl_804D47A8 = 0;
    lbl_804D47E8 = 0;
    lbl_804D47EC = 0;
    lbl_804D47F0 = 0;
    lbl_804D47F4 = 0;
    lbl_804D47F8 = 0;
    lbl_804D47FC = 0;
    lbl_804D4800 = 0;
    lbl_804D47B0 = 0;
    lbl_804D47B4 = 0;
    lbl_804D47B8 = 0;
    lbl_804D47BC = 0;
    lbl_804D47C0 = 0;

    if (lbl_804D47C4 > 0) {
        lbl_804D47D0 = VAlloc((lbl_804D47C4 + 0x10) * 0x928);
    } else {
        lbl_804D47D0 = VAlloc(0x9280);
    }

    lbl_80449098[0] = 0;
    lbl_80449098[1] = 0;
    lbl_80449098[2] = 0;
    lbl_80449098[3] = 0;
    lbl_80449098[4] = 0;

    if (lbl_804D47CC > 0) {
        lbl_8039B650.value =
            OLTFindValueByKey(0xC0A77187, *(unsigned long*)lbl_804D47D4);
        lbl_804493EC = *(AINeoAgentDef*)lbl_804D47D4;
    }

    offset = 0;
    for (i = 0; i < lbl_804D47C4; i++) {
        fn_800A6198(i, (void*)(lbl_804D47D8 + offset));
        offset += 0x34;
    }
}

extern "C" void fn_80026680(int id, void (*handler)(void), int arg);
extern "C" void fn_80026660(int id, void (*handler)(void), int arg);
extern "C" void fn_800A6054(void);
extern "C" void fn_800AB2B0(void);

// Resets the agent system and registers the two callbacks that live in this
// unit, then restores the default detail level.
void NeoAgentSystemInit(void)
{
    lbl_804D47D0 = 0;
    lbl_804D47D8 = 0;
    lbl_804D47CC = 0;
    lbl_804D47D4 = 0;
    lbl_804D47C8 = 0;
    lbl_804D47C4 = 0;
    lbl_804D47DC = 0;
    lbl_804D47E0[0] = 0;
    fn_80026680(0x13, fn_800A6054, 0);
    fn_80026660(0x13, fn_800AB2B0, 0);
    lbl_804D479C = 4;
}

// TODO fn_800A5558 (0x458 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800A5558(void) {}

// TODO fn_800A59B0 (0x6A4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800A59B0(void) {}

// TODO fn_800A6054 (0x144 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800A6054(void) {}

// TODO fn_800A6198 (0xD80 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800A6198(int slot, void* data) {}

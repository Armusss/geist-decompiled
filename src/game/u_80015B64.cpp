// u_80015B64.cpp: view-frustum construction and bounding-volume tests (0x80015B64-0x800190D4),
// followed by model bone lookup helpers. A frustum is a 16-byte header holding the plane count,
// then that many 4-float planes. Names are descriptive guesses; the code is matched against the
// retail DOL.

#define fabs(x) __fabs(x)
#define fabsf(x) __fabsf(x)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef float f32;

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

extern "C" {
double tan(double x);
double cos(double x);
double pow(double x, double y);
void fn_8009F44C(int* out, const void* mtx, const int* in);   // 0x8009F44C
void fn_8009F5B0(const void* mtx);                            // 0x8009F5B0
void fn_8009F50C(int* out, const int* in);                    // 0x8009F50C
void fn_800E1020(float* sums);                                // 0x800E1020
int lbl_803DAA70[6];                                          // 0x803DAA70
}

// Non-zero test and approximate normalize shared by the frustum builders.
inline int VecNonZero(const Vec3* v) {
    return (fabs(v->x) > 1.1920929e-7f) + (fabs(v->y) > 1.1920929e-7f) + (fabs(v->z) > 1.1920929e-7f);
}

inline float VecNormalize(Vec3* v) {
    double x, y, z, len;
    float inv;

    if (VecNonZero(v) == 0) {
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 0.0f;
        return 0.0f;
    }
    x = (double)v->x;
    y = (double)v->y;
    z = (double)v->z;
    {
        double xx = x * x;
        double yy = y * y;
        double zz = z * z;

        len = xx + yy + zz;
    }
    inv = __frsqrte((float)len);
    v->x = (float)(x * inv);
    v->y = (float)(y * inv);
    v->z = (float)(z * inv);
    return __fres(inv);
}

// Transforms a point into the space of mtx and checks it against the 6 bounds words (min xyz,
// max xyz).
s32 ViewFrustumBoxTest(IVec3* bounds, void* mtx, IVec3* point) {
    int local[3];
    int diff;

    local[0] = point->x - *(int*)((u8*)mtx + 0x24);
    local[1] = point->y - *(int*)((u8*)mtx + 0x28);
    local[2] = point->z - *(int*)((u8*)mtx + 0x2C);
    fn_8009F44C(local, mtx, local);
    diff = (local[0] - bounds[0].x) | (local[1] - bounds[0].y) | (local[2] - bounds[0].z) |
           (bounds[1].x - local[0]) | (bounds[1].y - local[1]) | (bounds[1].z - local[2]);
    return (u8)((u32)__cntlzw(diff >> 31) >> 5);
}

// For each of count points, classifies it against planes [first, planes[0]) of the volume and
// stores one bit per plane. Returns the OR of every bit written.
u32 ViewFrustumClassifyPoints(const int* planes, const Vec3* pts, u32* out, int count, int first) {
    u32 any = 0;

    while (count > 0) {
        const float* p = (const float*)planes + first * 4;
        int i;

        *out = 0;
        for (i = first; i < planes[0]; i++, p += 4) {
            float d = p[4] * pts->x + p[5] * pts->y + p[6] * pts->z + p[7];
            int side;
            u32 v;

            if (d > 1.0e-4f) {
                side = 1;
            } else if (d < -1.0e-4f) {
                side = 2;
            } else {
                side = 0;
            }
            v = *out | ((u32)(side & 1) << i);
            *out = v;
            any |= v;
        }
        out++;
        pts++;
        count--;
    }
    return any;
}

// Tests the 8 faces of an octahedron built from the 6 vertex values in src against the volume's
// planes. Returns -1 if some plane set accepts all 8 faces, otherwise a "no plane touched" flag.
u32 ViewFrustumTestOctahedron(const int* planes, const int* src) {
    int v[24];
    const float* p = (const float*)planes + planes[0] * 4;

    v[0] = src[0];
    v[1] = src[1];
    v[2] = src[2];
    v[3] = src[3];
    v[4] = src[1];
    v[5] = src[2];
    v[6] = src[3];
    v[7] = src[4];
    v[8] = src[2];
    v[9] = src[0];
    v[10] = src[4];
    v[11] = src[2];
    v[12] = src[0];
    v[13] = src[1];
    v[14] = src[5];
    v[15] = src[3];
    v[16] = src[1];
    v[17] = src[5];
    v[18] = src[3];
    v[19] = src[4];
    v[20] = src[5];
    v[21] = src[0];
    v[22] = src[4];
    v[23] = src[5];
    u32 any = 0;
    int n = planes[0];

    while (n != 0) {
        u32 all = 0xFFFFFFFF;
        int k = 8;
        int j = 0;

        n--;
        do {
            f32 fv[3];
            f32 d;
            int bits;
            u32 bit;

            fv[0] = (f32)v[j + 0];
            fv[1] = (f32)v[j + 1];
            fv[2] = (f32)v[j + 2];
            d = p[0] * fv[0] + p[1] * fv[1] + p[2] * fv[2] + p[3];
            bits = *(int*)&d;
            bit = (u32)((1 - (bits >> 31)) & 1) << n;
            any |= bit;
            all &= bit;
            j += 3;
            k--;
        } while (k != 0);
        p -= 4;
        if (all != 0) {
            return 0xFFFFFFFF;
        }
    }
    return __cntlzw(any) >> 5;
}

// Builds the plane through eye and the two edge points a and b (normal (a-eye) x (b-eye)).
inline void FrustumPlane(float* out, const Vec3* eye, const Vec3* a, const Vec3* b) {
    Vec3 n;

    n.x = (a->y - eye->y) * (b->z - eye->z) - (a->z - eye->z) * (b->y - eye->y);
    n.y = (a->z - eye->z) * (b->x - eye->x) - (a->x - eye->x) * (b->z - eye->z);
    n.z = (a->x - eye->x) * (b->y - eye->y) - (a->y - eye->y) * (b->x - eye->x);
    VecNormalize(&n);
    out[0] = n.x;
    out[1] = n.y;
    out[2] = n.z;
    out[3] = -(n.z * eye->z + n.x * eye->x + n.y * eye->y);
}

extern float gAspectScale;   // 0x804D2520

void fn_800167EC(float* out, const Vec3* origin, const Vec3* side, const Vec3* fwd,
                 float angle, float aspect);

// Builds a frustum with 4 side planes from the eye, four edge points, a forward normal and the
// near/far distances along it.
void ViewFrustumBuild(float* out, const Vec3* eye, const Vec3* fwd, const Vec3* c0,
                      const Vec3* c1, const Vec3* c2, const Vec3* c3, float nearD, float farD) {
    Vec3 n;
    Vec3 f;

    FrustumPlane(out + 4, eye, c0, c1);
    FrustumPlane(out + 8, eye, c1, c3);
    FrustumPlane(out + 0xc, eye, c3, c2);
    FrustumPlane(out + 0x10, eye, c2, c0);

    f = *fwd;
    VecNormalize(&f);
    n.x = -f.x;
    n.y = -f.y;
    n.z = -f.z;
    out[0x14] = n.x;
    out[0x15] = n.y;
    out[0x16] = n.z;
    out[0x17] = -(n.z * (eye->z + f.z * nearD) + n.x * (eye->x + f.x * nearD) +
                  n.y * (eye->y + f.y * nearD));
    out[0x18] = f.x;
    out[0x19] = f.y;
    out[0x1a] = f.z;
    out[0x1b] = -(f.z * (eye->z + f.z * farD) + f.x * (eye->x + f.x * farD) +
                  f.y * (eye->y + f.y * farD));
    ((int*)out)[0] = 6;
}

// Rebuilds the 4 side planes from an angle, then replaces the near/far planes along fwd.
void ViewFrustumUpdate(float* out, const Vec3* eye, const Vec3* side, const Vec3* fwd,
                       float angle, float fov, float nearD, float farD) {
    Vec3 n;

    fn_800167EC(out, eye, side, fwd, angle, gAspectScale);
    ((int*)out)[0] += 2;
    n = *fwd;
    VecNormalize(&n);
    out[0x14] = -n.x;
    out[0x15] = -n.y;
    out[0x16] = -n.z;
    out[0x17] = -((-n.z) * (eye->z + n.z * nearD) + (-n.x) * (eye->x + n.x * nearD) +
                  (-n.y) * (eye->y + n.y * nearD));
    out[0x18] = n.x;
    out[0x19] = n.y;
    out[0x1a] = n.z;
    out[0x1b] = -(n.z * (eye->z + n.z * farD) + n.x * (eye->x + n.x * farD) +
                  n.y * (eye->y + n.y * farD));
}

// Builds the 4 side planes of a symmetric frustum. fwd is the forward axis, side the horizontal
// axis and angle the vertical field of view in degrees.
void fn_800167EC(float* out, const Vec3* origin, const Vec3* side, const Vec3* fwd, float angle,
                 float aspect) {
    Vec3 n;
    Vec3 c0, c1, c2, c3;
    float half;
    float ta, tb;
    float nta, ntb;
    float bx, by, bz;

    ((int*)out)[0] = 4;
    half = 3.14159274f * angle / 180.0f;
    ta = (float)tan(0.5 * (double)(half * gAspectScale));
    tb = (float)tan(0.5 * (double)half);
    nta = -ta;
    ntb = -tb;

    n.x = fwd->z * side->y - fwd->y * side->z;
    n.y = fwd->x * side->z - fwd->z * side->x;
    n.z = fwd->y * side->x - fwd->x * side->y;
    VecNormalize(&n);

    bx = origin->x + fwd->x;
    by = origin->y + fwd->y;
    bz = origin->z + fwd->z;
    c0.x = n.x * ta + side->x * tb + bx;
    c0.y = n.y * ta + side->y * tb + by;
    c0.z = n.z * ta + side->z * tb + bz;
    c1.x = n.x * nta + side->x * tb + bx;
    c1.y = n.y * nta + side->y * tb + by;
    c1.z = n.z * nta + side->z * tb + bz;
    c2.x = n.x * nta + side->x * ntb + bx;
    c2.y = n.y * nta + side->y * ntb + by;
    c2.z = n.z * nta + side->z * ntb + bz;
    c3.x = n.x * ta + side->x * ntb + bx;
    c3.y = n.y * ta + side->y * ntb + by;
    c3.z = n.z * ta + side->z * ntb + bz;

    FrustumPlane(out + 4, origin, &c0, &c1);
    FrustumPlane(out + 8, origin, &c1, &c2);
    FrustumPlane(out + 0xc, origin, &c2, &c3);
    FrustumPlane(out + 0x10, origin, &c3, &c0);
}

inline u32 NoiseSeed(int i) {
    u32 u = (u32)i;

    return (u << 13) ^ u;
}

inline float NoiseValue(u32 seed) {
    u32 h = (seed * (seed * seed * 0x3d73 + 0xc0ae5) + 0x5208dd0d) & 0x7fffffff;

    return 1.0f - (float)h / 1073741824.0f;
}

float fn_8001715C(float angle);

// Applies the power-of-two gain pair to the angle noise (both exponents are zero here).
float fn_80017084(float angle) {
    double e = (double)(u32)0;
    double a = pow(2.0, e);
    double b = pow(0.5, e);
    float r = 0.0f;

    r += (float)b * fn_8001715C(angle * (float)a);
    return r;
}

// Smooth (raised-cosine) interpolation of hashed values around the integer part of angle.
float fn_8001715C(float angle) {
    int i = (int)angle;
    float t = 0.5f * (1.0f - (float)cos(3.14159274f * (angle - (float)i)));
    float nm1 = NoiseValue(NoiseSeed(i - 1));
    float n0 = NoiseValue(NoiseSeed(i));
    float np1 = NoiseValue(NoiseSeed(i + 1));
    float np2 = NoiseValue(NoiseSeed(i + 2));

    return (0.25f * np1 + 0.5f * n0 + 0.25f * nm1) * (1.0f - t) +
           (0.25f * np2 + 0.5f * np1 + 0.25f * n0) * t;
}

// Stores a plane (normal n, offset d = -n.p) at out.
void ViewPlaneFromNormal(float* out, const Vec3* n, const Vec3* p) {
    float d = n->x * p->x + n->y * p->y + n->z * p->z;
    out[0] = n->x;
    out[1] = n->y;
    out[2] = n->z;
    out[3] = -d;
}

// Transforms the 8 corners of the octahedron given by 6 vertex values into world space and
// returns the min/max bounding box of the result (min xyz, max xyz).
int* ViewOctahedronBounds(int* verts, const void* mtx) {
    IVec3 pts[8];
    int i;

    pts[0].x = verts[0];
    pts[0].y = verts[1];
    pts[0].z = verts[2];
    pts[1].x = verts[3];
    pts[1].y = verts[1];
    pts[1].z = verts[2];
    pts[2].x = verts[3];
    pts[2].y = verts[4];
    pts[2].z = verts[2];
    pts[3].x = verts[0];
    pts[3].y = verts[4];
    pts[3].z = verts[2];
    pts[4].x = verts[0];
    pts[4].y = verts[1];
    pts[4].z = verts[5];
    pts[5].x = verts[3];
    pts[5].y = verts[1];
    pts[5].z = verts[5];
    pts[6].x = verts[3];
    pts[6].y = verts[4];
    pts[6].z = verts[5];
    pts[7].x = verts[0];
    pts[7].y = verts[4];
    pts[7].z = verts[5];

    fn_8009F5B0(mtx);
    fn_8009F50C(&pts[0].x, &pts[0].x);
    pts[0].x += *(int*)((u8*)mtx + 0x24);
    pts[0].y += *(int*)((u8*)mtx + 0x28);
    pts[0].z += *(int*)((u8*)mtx + 0x2C);
    lbl_803DAA70[0] = pts[0].x;
    lbl_803DAA70[1] = pts[0].y;
    lbl_803DAA70[2] = pts[0].z;
    lbl_803DAA70[3] = pts[0].x;
    lbl_803DAA70[4] = pts[0].y;
    lbl_803DAA70[5] = pts[0].z;

    for (i = 1; i < 8; i++) {
        fn_8009F50C(&pts[i].x, &pts[i].x);
        pts[i].x += *(int*)((u8*)mtx + 0x24);
        pts[i].y += *(int*)((u8*)mtx + 0x28);
        pts[i].z += *(int*)((u8*)mtx + 0x2C);
        lbl_803DAA70[0] = (pts[i].x < lbl_803DAA70[0]) ? pts[i].x : lbl_803DAA70[0];
        lbl_803DAA70[1] = (pts[i].y < lbl_803DAA70[1]) ? pts[i].y : lbl_803DAA70[1];
        lbl_803DAA70[2] = (pts[i].z < lbl_803DAA70[2]) ? pts[i].z : lbl_803DAA70[2];
        lbl_803DAA70[3] = (pts[i].x > lbl_803DAA70[3]) ? pts[i].x : lbl_803DAA70[3];
        lbl_803DAA70[4] = (pts[i].y > lbl_803DAA70[4]) ? pts[i].y : lbl_803DAA70[4];
        lbl_803DAA70[5] = (pts[i].z > lbl_803DAA70[5]) ? pts[i].z : lbl_803DAA70[5];
    }
    return lbl_803DAA70;
}

// Sweeps the parameter of a Bezier control polygon from 0 to 1 in 1000 steps, accumulating the
// per-axis motion of the resulting curve point, then hands the sums to fn_800E1020.
void ViewBezierSweepLen(int* path) {
    Vec3 pts[33];
    Vec3 tmp[33];
    float acc[3];
    Vec3* src = (Vec3*)path[0];
    int n = path[1];
    int i, j, k;

    acc[0] = 0.0f;
    acc[1] = 0.0f;
    acc[2] = 0.0f;
    for (i = 0; i < n; i++) {
        pts[i] = src[i];
    }

    i = 1;
    for (;;) {
        if (n - 1 < i) {
            float px = pts[0].x;
            float py = pts[0].y;
            float pz = pts[0].z;
            u32 u;

            for (u = 1;; u++) {
                float t = (float)u / 1000.0f;
                float s = 1.0f - t;
                float dx, dy, dz;

                for (j = 0; j < n; j++) {
                    tmp[j] = src[j];
                }
                for (j = 1; j <= n - 1; j++) {
                    for (k = 0; k < n - j; k++) {
                        tmp[k].x = s * tmp[k].x + t * tmp[k + 1].x;
                        tmp[k].y = s * tmp[k].y + t * tmp[k + 1].y;
                        tmp[k].z = s * tmp[k].z + t * tmp[k + 1].z;
                    }
                }

                dx = tmp[0].x - px;
                dy = tmp[0].y - py;
                dz = tmp[0].z - pz;
                if (dx < 0.0f) {
                    dx = -dx;
                }
                if (dy < 0.0f) {
                    dy = -dy;
                }
                if (dz < 0.0f) {
                    dz = -dz;
                }
                acc[0] += dx;
                acc[2] += dz;
                acc[1] += dy;
                px = tmp[0].x;
                py = tmp[0].y;
                pz = tmp[0].z;
                if ((int)u > 999) {
                    fn_800E1020(acc);
                    return;
                }
            }
        }

        for (j = 0; j < n - i; j++) {
            pts[j].x = 1.0f * pts[j].x + 0.0f * pts[j + 1].x;
            pts[j].y = 1.0f * pts[j].y + 0.0f * pts[j + 1].y;
            pts[j].z = 1.0f * pts[j].z + 0.0f * pts[j + 1].z;
        }
        i++;
    }
}

// Evaluates the Bezier curve defined by the control polygon at parameter t (de Casteljau).
void ViewBezierPoint(double t, int* path, Vec3* out) {
    Vec3 pts[33];
    Vec3* src = (Vec3*)path[0];
    int n = path[1];
    float s = (float)(1.0 - t);
    int i, j, k;

    for (i = 0; i < n; i++) {
        pts[i] = src[i];
    }
    for (j = 1;; j++) {
        if (n - 1 < j) {
            *out = pts[0];
            return;
        }
        for (k = 0; k < n - j; k++) {
            pts[k].x = s * pts[k].x + (float)(t * (double)pts[k + 1].x);
            pts[k].y = s * pts[k].y + (float)(t * (double)pts[k + 1].y);
            pts[k].z = s * pts[k].z + (float)(t * (double)pts[k + 1].z);
        }
    }
}

// ---------------------------------------------------------------------------
// Model bone queries. The model handle is an index into a global table; the
// bone data pointers come from the model system in other files.
// ---------------------------------------------------------------------------

struct Vec4 {
    float x, y, z, w;
};

struct AnimBone {
    float q[4];
    float pos[3];
};

int ModelBoneCount(int model);
void* ModelBonePositions(int model);
void ModelAnimate(int model, char* buf, int flag);
void ModelNodeSetup(int model, int node, int a, int b);
int FileAddHandler(int id, int (*fn)(void*, int, void*), void* arg);

extern "C" {
void fn_80008E34(int mode);
void fn_80008DFC(int mode);
int fn_8003226C(int model, int i, int a, int b, int c, void* pts, int* o1, int* o2,
                int* o3, int* o4, float* dist);
int fn_80032D34(int a, int b, void* pts, int c, int count, int* out, void* o1, void* o2);
int fn_80032E54(int model, int a, float b, int c);
int fn_80032FD4(int a, int b, const void* p, int c);
int fn_800331F8(int a, int b, const void* p, int c, int* o1, int* o2, float* dist);
int fn_800333E0(void);
int fn_80033450(int a, int b, int c);
int fn_800345BC(void);
int fn_80037230(int model, int node);
int fn_80037930(int model, int node);
void* fn_80037B6C(int model);
void* fn_80037BAC(int model);
int fn_80037C20(int model);
int fn_80038230(int model, int node);
int fn_800388A4(int model, int node);
void* fn_80039478(int model);
void* fn_80039638(int model);
int fn_800398A8(int name);
void fn_8003AA9C(int model, char* buf, int flag);
int fn_8003B30C(void);
int fn_8003B400(void);
int fn_8003B4A4(void);
int fn_80032014(void* data, int msg, void* arg);
int fn_80031FAC(void* data, int msg, void* arg);
int fn_80032094(void);
int fn_800320F4(void);
int fn_80032200(void);
void Vec3Copy(Vec3* dst, const Vec3* src);
}

inline unsigned int ModelBoneLookup(int model, int bone) {
    unsigned char* map = (unsigned char*)fn_80039478(model);
    unsigned int v;

    fn_80037B6C(model);
    if (map != 0 && bone >= 0 && (v = map[bone]) != 0xff) {
        return v;
    }
    return 0xFFFFFFFF;
}

// Returns the two per-model bone tables: the name->bone map and the bone node list.
struct ModelBoneMaps {
    unsigned char* map;
    unsigned short* nodes;
};

inline ModelBoneMaps ModelGetBoneMaps(int model) {
    ModelBoneMaps m;

    m.map = (unsigned char*)fn_80039478(model);
    m.nodes = (unsigned short*)fn_80037B6C(model);
    return m;
}

struct ModelNodeMaps {
    unsigned char* map;
    unsigned short* nodes;
};

inline ModelNodeMaps ModelGetNodeMaps(int model) {
    ModelNodeMaps m;

    m.map = (unsigned char*)fn_80039638(model);
    m.nodes = (unsigned short*)fn_80037BAC(model);
    return m;
}

// Checks the model's bones against the per-bone shape data; on a hit returns 1 and
// stores the bone's shape data.
int ModelBoneHitTest(int model, char* shapes, int a, int b) {
    fn_80008E34(9);
    int found = 0;
    int count = ModelBoneCount(model);
    Vec3* pos = (Vec3*)ModelBonePositions(model);
    unsigned short* nodes = (unsigned short*)fn_80037B6C(model);
    int i;

    for (i = 0; i < count; i++) {
        int flags = fn_80037230(model, *nodes);

        if ((flags & 0x1000) == 0 && fn_80038230(model, *nodes) != 0) {
            Vec3 p;
            int hit;

            Vec3Copy(&p, pos);
            fn_80008E34(9);
            hit = fn_80032FD4(a, b, &p, (int)shapes);
            fn_80008DFC(9);
            if (hit != 0) {
                found = 1;
                break;
            }
        }
        nodes++;
        pos++;
        shapes += 0x30;
    }
    fn_80008DFC(9);
    return found;
}

// Candidate/selected hit record shared by the nearest-hit and sweep searches.
struct BoneHitResult {
    Vec3 pos;
    Vec3 normal;
    int flags;
    int unk0;
    int unk1;
    int bone;
};

// Finds the closest bone hit. The two out vectors receive the hit position and normal,
// outBone the bone index, shape the shape data and model the hit model.
int ModelBoneHitNearest(int a, int b, int model, int shapes, int* outBone, Vec3* outPos,
                        Vec3* outNormal, unsigned int* shape) {
    BoneHitResult best;
    BoneHitResult cand;
    float dist;
    Vec3 p;
    unsigned short model16 = (unsigned short)model;
    unsigned short* nodeBase;
    unsigned short* nodes;
    Vec3* pos;
    int count;
    float bestDist = 3.4e38f;
    int found = 0;
    int i;

    fn_80008E34(9);
    count = ModelBoneCount(model);
    pos = (Vec3*)ModelBonePositions(model);
    nodeBase = (unsigned short*)fn_80037B6C(model);
    nodes = nodeBase;
    for (i = 0; i < count; i++) {
        int flags = fn_80037230(model, *nodes);
        int hit;

        if (flags & 0x1000) {
            goto next;
        }
        if ((flags & 0x800) && !(*shape & 0x800)) {
            goto next;
        }
        if ((flags & 0x2000) && (*shape & 0x2000)) {
            goto next;
        }
        if ((flags & 0x8000) && (*shape & 0x8000)) {
            goto next;
        }
        if ((flags & 0x200) && (*shape & 0x200)) {
            goto next;
        }
        if (fn_80038230(model, *nodes) == 0) {
            goto next;
        }
        Vec3Copy(&p, pos);
        cand.flags = flags;
        hit = fn_800331F8(a, b, &p, shapes, (int*)&cand.pos, (int*)&cand.normal, &dist);
        if (hit != 0) {
            found = 1;
            cand.bone = i;
            if (dist < bestDist) {
                bestDist = dist;
                best = cand;
            }
        }
    next:
        nodes++;
        pos++;
        shapes += 0x30;
    }
    if (found) {
        *outBone = best.bone;
        *outPos = best.pos;
        *outNormal = best.normal;
        *shape = fn_80037230(model16, nodeBase[best.bone]);
    }
    fn_80008DFC(9);
    return found;
}

// Tests the model's bones against the shape list and returns the closest hit.
int ModelBoneHitSweep(int a, int b, int model, int shapes, int* outBone, int* outNode,
                      Vec3* outPos, Vec3* outNormal, unsigned int* shape, int* outModel) {
    int nodeCount = fn_80037C20(model);
    Vec3* pos = (Vec3*)ModelBonePositions(model);
    int count = ModelBoneCount(model);
    Vec3 pts[256];
    unsigned short* nodes = (unsigned short*)fn_80037BAC((unsigned short)model);
    float bestDist = 1.0e30f;
    int bestFound = 0;
    int bestBone = 0;
    int bestNode = 0;
    unsigned int bestShape = *shape;
    unsigned int bestAux = 0;
    Vec3 bestPos;
    Vec3 bestNormal;
    int i;

    fn_80008E34(9);
    for (i = 0; i < count; i++) {
        Vec3Copy(&pts[i], pos);
        pos++;
    }
    for (i = 0; i < nodeCount; i++) {
        if (fn_800388A4(model, *nodes) != 0) {
            float dist;
            Vec3 hitPos;
            Vec3 hitNormal;
            unsigned int shapeCopy = *shape;
            unsigned int aux;
            int hit;

            hit = fn_8003226C(model, i, a, b, shapes, pts, (int*)&hitPos,
                              (int*)&hitNormal, (int*)&shapeCopy, (int*)&aux, &dist);
            if (hit != 0) {
                bestFound = 1;
                bestNode = i;
                if (dist < bestDist) {
                    bestDist = dist;
                    bestBone = i;
                    bestPos = hitPos;
                    bestNormal = hitNormal;
                    bestShape = shapeCopy;
                    bestAux = aux;
                }
            }
        }
        nodes++;
    }
    if (bestFound != 0) {
        int o1[3];
        int o2[3];
        int r;

        *outNode = bestBone;
        r = fn_80032D34(a, b, pts, shapes, count, outBone, o1, o2);
        if (r == 0) {
            *outBone = 0;
        }
        *outPos = bestPos;
        *outNormal = bestNormal;
        *shape = bestShape;
        *outModel = bestAux;
    }
    fn_80008DFC(9);
    return bestFound;
}

int ModelBoneHitCapsule(int model, int a, float b, int c) {
    int result;

    fn_80008E34(9);
    result = fn_80032E54(model, a, b, c);
    fn_80008DFC(9);
    return result;
}

int ModelBoneHitBox(int a, int b, int c, int d) {
    int result;

    fn_80008E34(9);
    result = fn_80032FD4(a, b, (const void*)c, d);
    fn_80008DFC(9);
    return result;
}

int ModelBoneHitReset(void) {
    return fn_800333E0();
}

int ModelBoneHitCylinder(int a, int b, int c) {
    int result;

    fn_80008E34(9);
    result = fn_80033450(a, b, c);
    fn_80008DFC(9);
    return result;
}

int ModelBoneHitSphere(int a, int b, int c, int d, int e, int f) {
    float dist;
    int result;

    fn_80008E34(9);
    result = fn_800331F8(a, b, (const void*)c, d, (int*)e, (int*)f, &dist);
    fn_80008DFC(9);
    return result;
}

int ModelBoneHitFlush(void) {
    return fn_800345BC();
}

// Walks the node hierarchy from start and returns the position of the first of the
// count names found in the chain, or -1.
int ModelBoneNodeChain(int model, int start, int* names, int count) {
    int table[256];
    int* p;
    int i;

    start = ModelBoneLookup(model, start);
    if (start == -1) {
        return -1;
    }
    p = names;
    for (i = 0; i < count; i++) {
        table[i] = ModelBoneLookup(model, *p);
        p++;
    }
    while (start != -1) {
        for (i = 0; i < count; i++) {
            if (start == table[i]) {
                return i;
            }
        }
        {
            u16* nodes;
            int node;
            int idx;

            fn_80039478(model);
            nodes = (u16*)fn_80037B6C(model);
            if (nodes != 0 && start >= 0 && start < ModelBoneCount(model)) {
                node = nodes[start];
            } else {
                node = -1;
            }
            idx = fn_80037930(model, node);
            start = ModelBoneLookup(model, idx);
        }
    }
    return -1;
}

// Sets up the node whose name is at the given index of the node name table.
void ModelNodeSetupByName(int model, int node, int a, int b) {
    unsigned char* map = (unsigned char*)fn_80039638(model);
    unsigned short* nodes = (unsigned short*)fn_80037BAC(model);
    int idx;

    if (nodes != 0 && node >= 0 && node < fn_80037C20(model)) {
        idx = nodes[node];
    } else {
        idx = -1;
    }
    ModelNodeSetup(model, idx, a, b);
}

// Returns the Z component of the first bone's animated position.
float ModelBoneMotionZ(int model) {
    char anim[0x1bc8];

    ModelAnimate(model, anim, 1);
    return *(float*)(anim + 0x18);
}

// Sets up the node with the given bone name.
int ModelNodeSetupByNameIndex(int model, int name, int a, int b) {
    int idx = fn_800398A8(name);

    if (idx != -1) {
        ModelNodeSetup(model, idx, a, b);
    }
    return 1;
}

// Returns the bone index for a bone name index, or -1.
int ModelFindBone(int model, int bone) {
    unsigned char* map = (unsigned char*)fn_80039478(model);
    unsigned short* nodes = (unsigned short*)fn_80037B6C(model);
    unsigned int v;

    if (map != 0 && bone >= 0 && (v = map[bone]) != 0xff) {
        return v;
    }
    return 0xFFFFFFFF;
}

// Returns the node index for a bone index, or -1.
int ModelBoneName(int model, int bone) {
    unsigned char* map = (unsigned char*)fn_80039478(model);
    unsigned short* nodes = (unsigned short*)fn_80037B6C(model);
    unsigned int v;

    if (nodes != 0 && bone >= 0 && bone < ModelBoneCount(model)) {
        return nodes[bone];
    }
    return 0xFFFFFFFF;
}

// Returns the node index for a node name index, or -1.
int ModelNodeLookup(int model, int node) {
    unsigned char* map = (unsigned char*)fn_80039638(model);
    unsigned short* nodes = (unsigned short*)fn_80037BAC(model);
    unsigned int v;

    if (nodes != 0 && node >= 0 && node < fn_80037C20(model)) {
        return nodes[node];
    }
    return 0xFFFFFFFF;
}

// Returns the bone index for a bone name index, or -1.
int ModelNodeIndex(int model, int node) {
    unsigned char* map = (unsigned char*)fn_80039638(model);
    unsigned short* nodes = (unsigned short*)fn_80037BAC(model);
    unsigned int v;

    if (map != 0 && node >= 0 && (v = map[node]) != 0xff) {
        return v;
    }
    return 0xFFFFFFFF;
}

// Copies the animated rotation of every bone into out (4 floats per bone).
void ModelBoneRotations(int model, float* out) {
    char anim[0x1bc8];
    int count;
    int i;

    ModelAnimate(model, anim, 1);
    count = ModelBoneCount(model);
    for (i = 0; i < count; i++) {
        AnimBone* b = (AnimBone*)(anim + i * 0x1c);

        out[i * 4 + 0] = b->q[0];
        out[i * 4 + 1] = b->q[1];
        out[i * 4 + 2] = b->q[2];
        out[i * 4 + 3] = b->q[3];
    }
}

// Copies the animated position of every bone into out (3 floats per bone).
void ModelBoneTranslations(int model, float* out) {
    char anim[0x1bc8];
    int count;
    int i;

    ModelAnimate(model, anim, 1);
    count = ModelBoneCount(model);
    for (i = 0; i < count; i++) {
        AnimBone* b = (AnimBone*)(anim + i * 0x1c);

        out[i * 3 + 0] = b->pos[0];
        out[i * 3 + 1] = b->pos[1];
        out[i * 3 + 2] = b->pos[2];
    }
}

void ModelPhysicsReset(void) {
    fn_80032094();
}

int ModelPhysicsStart(void) {
    fn_8003B400();
    return 1;
}

void ModelPhysicsShutdown(void) {
    fn_8003B30C();
    fn_800320F4();
}

int ModelPhysicsOk(void) {
    return 1;
}

int ModelSystemInit(void) {
    FileAddHandler(0x2a, fn_80032014, 0);
    FileAddHandler(0x2b, fn_80031FAC, 0);
    fn_80032200();
    fn_8003B4A4();
    return 1;
}

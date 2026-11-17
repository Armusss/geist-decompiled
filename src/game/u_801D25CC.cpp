// u_801D25CC.cpp: 0x801D25CC-0x801D4738, 12 functions. The level-geometry collision class: user geom callbacks
// (AABB test, destructor, AABB, collider lookup) and the colliders of spheres, boxes and capsules against the
// room's triangles, plus a debug display of the triangles and contact normals that were hit.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   0x801D25CC: the four user-class callbacks (registered by u_801D4738's fn_801D7CD0) work on the same
//     UserGeomGetId data as the colliders; RoomGeomColliderFor (0x801D2634) returns the colliders below
//     (moved here from u_801D0C18, whose Fire code ends at 0x801D25CC; approved by the Lead)
//   .sdata2 0x804D8BD0-0x804D8C20 (0.0 at 0x804D8BE4 read from 0x801D2694 to 0x801D426C; the next file
//     has its own 0.00238 copy at 0x804D8C20, so the cut is exactly at 0x801D4738)
//   .bss 0x80496DD8 (0x2400), 0x804991D8 (0x1800); .sbss 0x804D5198/0x804D519C (debug triangle/normal lists)
// The colliders use the ODE dColliderFn signature (o1, o2, flags, contacts, skip); positions are scaled from
// world units by 419.84 and back by 1/419.84.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"

typedef unsigned long u32;
typedef unsigned char u8;

struct Geom;
struct UserGeom;

// The user data of the level-geometry geom: its bounding box and the room it collides against.
struct RoomGeomData {
    float aabb[6];      // 0x00
    short room;         // 0x18
};

// One room triangle as returned by fn_80035330.
struct Tri {
    Vec3 v[3];
};

// A plane (normal and distance).
struct Plane {
    float x, y, z, d;
};

// ODE's dContactGeom. While contacts are merged, g1 counts the merged contacts and g2 holds the triangle.
struct ContactGeom {
    float pos[4];       // 0x00
    float normal[4];    // 0x10
    float depth;        // 0x20
    union {
        Geom* g1;       // 0x24
        int count;
    };
    union {
        Geom* g2;       // 0x28
        int tri;
    };
};

// A polygon of up to eight points (clipped against the box planes).
struct Poly {
    float p[8][4];      // 0x00
    int n;              // 0x80
};

RoomGeomData* UserGeomGetId(UserGeom* g);                  // 0x8026A374
int RoomPortalOther(int room);                              // 0x8001A01C
float* GeomGetPosition(Geom* g);                            // 0x8026AC58
float* GeomGetRotation(Geom* g);                            // 0x8026AC50
float SphereGetRadius(Geom* g);                             // 0x80271C90
void BoxGetLengths(Geom* g, float* result);                 // 0x80271AB4
void CapsuleGetParams(Geom* g, float* radius, float* length);  // 0x802718A0
void MtxGetColumn(Vec3* out, Mtx34* m, int col);            // 0x800A0238
void Mtx34StoreTranslation(Mtx34* m, Vec3* t);              // 0x800A0280

extern "C" {
void* memcpy(void* dst, const void* src, u32 n);
void fn_800090BC(Vec3* a, Vec3* b, int r, int g, int bl);   // 0x800090BC: debug line
int fn_8002FE64(Vec3* start, Vec3* dir, Vec3* v0, Vec3* e1, Vec3* e2, float* s, float* u, float* v);
int fn_80034E30(int room, float* box, int a);               // 0x80034E30: gather the triangles in a box
int fn_80035330(int i, Tri* tri, float* surface);           // 0x80035330: one gathered triangle
float fn_800E1020(void* v);                                 // 0x800E1020: vector length
void fn_8027914C(float* v, ...);                            // 0x8027914C: normalize
int fn_801D60A0(int flag);                                  // 0x801D60A0: debug flag
void fn_801D7F54(float* out, Vec3* in);                     // 0x801D7F54
void fn_801D7F78(Mtx34* out, float* rot);                   // 0x801D7F78
}

int RoomGeomCapsuleCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contacts, int skip);
int RoomGeomNoCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contacts, int skip);
int RoomGeomSphereCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contacts, int skip);
int RoomGeomBoxCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contacts, int skip);
void RoomGeomBoxCorners(float* center, float* half, Mtx34* m, float* out);
void RoomGeomClipPoly(float d, float* plane, Poly* poly);

Vec3 gRoomGeomDebugTris[0x300];
Vec3 gRoomGeomDebugNormals[0x200];
int gRoomGeomDebugTriCount;
int gRoomGeomDebugNormalCount;

extern "C" int fn_801D25CC() { return 1; }

extern "C" void fn_801D25D4() {}

extern "C" void fn_801D25D8(UserGeom* g, float* aabb) {
    RoomGeomData* d = UserGeomGetId(g);

    aabb[0] = d->aabb[0];
    aabb[1] = d->aabb[1];
    aabb[2] = d->aabb[2];
    aabb[3] = d->aabb[3];
    aabb[4] = d->aabb[4];
    aabb[5] = d->aabb[5];
}

extern "C" void* fn_801D2634(int pass) {
    switch (pass) {
    case 0:
        return (void*)RoomGeomSphereCollide;
    case 1:
        return (void*)RoomGeomBoxCollide;
    case 5:
        return (void*)RoomGeomNoCollide;
    case 2:
        return (void*)RoomGeomCapsuleCollide;
    }
    return 0;
}

int RoomGeomCapsuleCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contacts, int skip) {
    RoomGeomData* data;
    float* p;
    float cx, cy, cz;
    float radius;
    float length;
    float e;
    float box[6];
    Vec3 start;
    Vec3 dir;
    Tri tri;
    Vec3 e1;
    Vec3 e2;
    Plane n;
    float surface;
    float s;
    float u;
    float v;
    float r2;
    Mtx34 m;
    int count;
    int off;
    int i;
    int ntris;

    data = UserGeomGetId((UserGeom*)o1);
    if (!(RoomPortalOther(data->room) & 0x10)) {
        return 0;
    }
    p = GeomGetPosition(o2);
    cx = p[0] * 419.84f;
    cy = p[1] * 419.84f;
    cz = p[2] * 419.84f;
    fn_801D7F78(&m, GeomGetRotation(o2));
    CapsuleGetParams(o2, &radius, &length);
    length *= 209.92f;
    e = length * 0.125f + length;
    radius *= 419.84f;
    {
        float r = radius * 0.125f + radius;
        float ex = r + __fabsf(m.m[0][2] * e);
        float ey = r + __fabsf(m.m[1][2] * e);
        float ez = r + __fabsf(m.m[2][2] * e);

        box[0] = cx - ex;
        box[1] = cy - ey;
        box[2] = cz - ez;
        box[3] = cx + ex;
        box[4] = cy + ey;
        box[5] = cz + ez;
    }
    ntris = fn_80034E30(data->room, box, 0);
    if (ntris == 0) {
        return 0;
    }
    count = 0;
    off = 0;
    start.x = cx - m.m[0][2] * length;
    start.y = cy - m.m[1][2] * length;
    start.z = cz - m.m[2][2] * length;
    dir.x = m.m[0][2] * length * 2.0f;
    dir.y = m.m[1][2] * length * 2.0f;
    dir.z = m.m[2][2] * length * 2.0f;
    r2 = radius * radius;
    for (i = 0; i < ntris; i++) {
        float len;
        float d2;

        if (fn_80035330(i, &tri, &surface) == 0) {
            continue;
        }
        e1.x = tri.v[1].x - tri.v[0].x;
        e1.y = tri.v[1].y - tri.v[0].y;
        e1.z = tri.v[1].z - tri.v[0].z;
        e2.x = tri.v[2].x - tri.v[0].x;
        e2.y = tri.v[2].y - tri.v[0].y;
        e2.z = tri.v[2].z - tri.v[0].z;
        n.x = e1.y * e2.z - e1.z * e2.y;
        n.y = e1.z * e2.x - e1.x * e2.z;
        n.z = e1.x * e2.y - e1.y * e2.x;
        n.d = n.x * tri.v[0].x + n.y * tri.v[0].y + n.z * tri.v[0].z;
        len = fn_800E1020(&n);
        n.x /= len;
        n.y /= len;
        n.z /= len;
        n.d /= len;
        d2 = fn_8002FE64(&start, &dir, &tri.v[0], &e1, &e2, &s, &u, &v);
        if (d2 <= r2) {
            ContactGeom* c = (ContactGeom*)((u8*)contacts + off);
            float d;
            int j;
            int k;

            c->pos[0] = 0.0023818625f * (tri.v[0].x + e2.x * v + e1.x * u);
            c->pos[1] = 0.0023818625f * (tri.v[0].y + e2.y * v + e1.y * u);
            c->pos[2] = 0.0023818625f * (tri.v[0].z + e2.z * v + e1.z * u);
            c->pos[3] = surface;
            c->normal[0] = -n.x;
            c->normal[1] = -n.y;
            c->normal[2] = -n.z;
            c->normal[3] = 0.0f;
            d = 0.0f;
            if (d2 > 0.0f) {
                d = 1.0f / __frsqrte(d2);
            }
            c->depth = radius - d;
            if (c->depth < -10.0f) {
                c->depth = -10.0f;
            } else if (c->depth > 10.0f) {
                c->depth = 10.0f;
            }
            c->depth *= 0.0023818625f;
            c->count = 1;
            c->tri = i;
            k = 0;
            for (j = 0; j < count; j++) {
                ContactGeom* o = (ContactGeom*)((u8*)contacts + k);

                if (o->normal[0] * c->normal[0] + o->normal[1] * c->normal[1] + o->normal[2] * c->normal[2] >
                    0.9999f) {
                    break;
                }
                k += skip;
            }
            if (j == count) {
                off += skip;
                count++;
            } else {
                ContactGeom* o = (ContactGeom*)((u8*)contacts + j * skip);

                o->pos[0] += c->pos[0];
                o->pos[1] += c->pos[1];
                o->pos[2] += c->pos[2];
                o->depth += c->depth;
                o->count++;
            }
            if (count >= flags) {
                break;
            }
        }
    }
    for (i = 0; i < count; i++) {
        ContactGeom* c = (ContactGeom*)((u8*)contacts + i * skip);
        float f = 1.0f / (float)c->count;

        c->pos[0] *= f;
        c->pos[1] *= f;
        c->pos[2] *= f;
        c->depth *= f;
        c->g1 = o1;
        c->g2 = o2;
    }
    return count;
}

int RoomGeomNoCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contacts, int skip) {
    return 0;
}

int RoomGeomPointInTri(Vec3* p, Plane* n, Tri* t);

int RoomGeomSphereCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contacts, int skip) {
    RoomGeomData* data;
    float* p;
    float cx, cy, cz;
    float r;
    float e;
    float lo;
    float hi;
    float box[6];
    Tri tri;
    Vec3 e1;
    Vec3 e2;
    Plane n;
    Vec3 q;
    float surface;
    int count;
    int off;
    int i;
    int ntris;

    data = UserGeomGetId((UserGeom*)o1);
    if (!(RoomPortalOther(data->room) & 0x10)) {
        return 0;
    }
    p = GeomGetPosition(o2);
    cx = p[0] * 419.84f;
    cy = p[1] * 419.84f;
    cz = p[2] * 419.84f;
    r = SphereGetRadius(o2) * 419.84f;
    e = r * 0.125f + r;
    box[0] = cx - e;
    box[1] = cy - e;
    box[2] = cz - e;
    box[3] = cx + e;
    box[4] = cy + e;
    box[5] = cz + e;
    ntris = fn_80034E30(data->room, box, 0);
    if (ntris == 0) {
        return 0;
    }
    count = 0;
    off = 0;
    lo = 3.0f * -r;
    hi = 2.0f * r;
    for (i = 0; i < ntris; i++) {
        float len;
        float d;

        if (fn_80035330(i, &tri, &surface) == 0) {
            continue;
        }
        e1.x = tri.v[1].x - tri.v[0].x;
        e1.y = tri.v[1].y - tri.v[0].y;
        e1.z = tri.v[1].z - tri.v[0].z;
        e2.x = tri.v[2].x - tri.v[0].x;
        e2.y = tri.v[2].y - tri.v[0].y;
        e2.z = tri.v[2].z - tri.v[0].z;
        n.x = e1.y * e2.z - e1.z * e2.y;
        n.y = e1.z * e2.x - e1.x * e2.z;
        n.z = e1.x * e2.y - e1.y * e2.x;
        n.d = n.x * tri.v[0].x + n.y * tri.v[0].y + n.z * tri.v[0].z;
        len = fn_800E1020(&n);
        n.y /= len;
        n.x /= len;
        n.z /= len;
        n.d /= len;
        d = r + (n.d - (cx * n.x + cy * n.y + cz * n.z));
        if (lo <= d && d <= hi) {
            q.x = cx - n.x * r;
            q.y = cy - n.y * r;
            q.z = cz - n.z * r;
            if (RoomGeomPointInTri(&q, &n, &tri)) {
                ContactGeom* c = (ContactGeom*)((u8*)contacts + off);

                count++;
                off += skip;
                c->pos[0] = 0.0023818625f * q.x;
                c->pos[1] = 0.0023818625f * q.y;
                c->pos[2] = 0.0023818625f * q.z;
                c->pos[3] = surface;
                c->normal[0] = -n.x;
                c->normal[1] = -n.y;
                c->normal[2] = -n.z;
                c->normal[3] = 0.0f;
                c->depth = 0.0023818625f * d;
                c->g1 = o1;
                c->g2 = o2;
                if (count >= flags) {
                    break;
                }
            }
        }
    }
    return count;
}

// Is the point (on the triangle's plane) inside the triangle? The three edge tests must agree in sign.
// n . (a x b)
inline float TripleDot(Plane* n, Vec3* a, Vec3* b) {
    return n->x * (a->y * b->z - a->z * b->y) + n->y * (a->z * b->x - a->x * b->z) + n->z * (a->x * b->y - a->y * b->x);
}

int RoomGeomPointInTri(Vec3* p, Plane* n, Tri* t) {
    Vec3 a;
    Vec3 b;
    Vec3 c;
    float d0, d1, d2;

    a.x = p->x - t->v[0].x;
    a.y = p->y - t->v[0].y;
    a.z = p->z - t->v[0].z;
    b.x = p->x - t->v[1].x;
    b.y = p->y - t->v[1].y;
    b.z = p->z - t->v[1].z;
    c.x = p->x - t->v[2].x;
    c.y = p->y - t->v[2].y;
    c.z = p->z - t->v[2].z;
    d0 = TripleDot(n, &a, &b);
    d1 = TripleDot(n, &b, &c);
    d2 = TripleDot(n, &c, &a);

    if ((d0 >= 0.0f && d1 >= 0.0f && d2 >= 0.0f) || (d0 <= 0.0f && d1 <= 0.0f && d2 <= 0.0f)) {
        return 1;
    }
    return 0;
}

int RoomGeomBoxCollide(Geom* o1, Geom* o2, int flags, ContactGeom* contacts, int skip) {
    RoomGeomData* data;
    float* p;
    Vec3 pos;
    float half[3];
    float box[6];
    Mtx34 rot;
    Mtx34 m;
    Vec3 axes[3];
    Plane planes[6];
    float corners[24];
    Tri tri;
    float surface;
    Poly poly;
    Plane n;
    int count;
    int off;
    int i;
    int ntris;

    data = UserGeomGetId((UserGeom*)o1);
    if (!(RoomPortalOther(data->room) & 0x10)) {
        return 0;
    }
    p = GeomGetPosition(o2);
    pos.x = p[0] * 419.84f;
    pos.y = p[1] * 419.84f;
    pos.z = p[2] * 419.84f;
    BoxGetLengths(o2, half);
    half[0] *= 209.92f;
    half[1] *= 209.92f;
    half[2] *= 209.92f;
    fn_801D7F78(&rot, GeomGetRotation(o2));
    m = rot;
    Mtx34StoreTranslation(&m, &pos);
    {
        float e = half[1];

        if (e < half[0]) {
            e = half[0];
        }
        if (e < half[2]) {
            e = half[2];
        }
        e *= 2.0f;
        box[0] = pos.x - e;
        box[1] = pos.y - e;
        box[2] = pos.z - e;
        box[3] = pos.x + e;
        box[4] = pos.y + e;
        box[5] = pos.z + e;
    }
    ntris = fn_80034E30(data->room, box, 0);
    if (ntris == 0) {
        return 0;
    }
    MtxGetColumn(&axes[0], &rot, 0);
    MtxGetColumn(&axes[2], &rot, 2);
    MtxGetColumn(&axes[1], &rot, 1);
    for (i = 0; i < 3; i++) {
        float h = -1.0f * half[i];

        planes[i].x = axes[i].x * -1.0f;
        planes[i].y = axes[i].y * -1.0f;
        planes[i].z = axes[i].z * -1.0f;
        planes[i].d = -1.0f * (axes[i].x * -(h * axes[i].x - pos.x) + axes[i].y * -(h * axes[i].y - pos.y) +
                               axes[i].z * -(h * axes[i].z - pos.z));
    }
    for (i = 0; i < 3; i++) {
        float h = 1.0f * half[i];

        planes[i + 3].x = axes[i].x * 1.0f;
        planes[i + 3].y = axes[i].y * 1.0f;
        planes[i + 3].z = axes[i].z * 1.0f;
        planes[i + 3].d = 1.0f * (axes[i].x * -(h * axes[i].x - pos.x) + axes[i].y * -(h * axes[i].y - pos.y) +
                                  axes[i].z * -(h * axes[i].z - pos.z));
    }
    count = 0;
    off = 0;
    for (i = 0; i < ntris; i++) {
        int j;

        if (fn_80035330(i, &tri, &surface) == 0) {
            continue;
        }
        if (fn_801D60A0(2) && gRoomGeomDebugTriCount < 0x100) {
            float nx = (tri.v[1].y - tri.v[0].y) * (tri.v[2].z - tri.v[0].z) -
                       (tri.v[1].z - tri.v[0].z) * (tri.v[2].y - tri.v[0].y);
            float ny = (tri.v[1].z - tri.v[0].z) * (tri.v[2].x - tri.v[0].x) -
                       (tri.v[1].x - tri.v[0].x) * (tri.v[2].z - tri.v[0].z);
            float nz = (tri.v[1].x - tri.v[0].x) * (tri.v[2].y - tri.v[0].y) -
                       (tri.v[1].y - tri.v[0].y) * (tri.v[2].x - tri.v[0].x);
            int k;

            if ((__fabsf(nx) > 1.1920929e-7f) + (__fabsf(ny) > 1.1920929e-7f) + (__fabsf(nz) > 1.1920929e-7f) == 0) {
                nx = ny = nz = 0.0f;
            } else {
                float f = __frsqrte(nx * nx + ny * ny + nz * nz);

                nx *= f;
                ny *= f;
                nz *= f;
            }
            nx *= 8.0f;
            ny *= 8.0f;
            nz *= 8.0f;
            k = gRoomGeomDebugTriCount * 3;
            gRoomGeomDebugTriCount++;
            gRoomGeomDebugTris[k].x = tri.v[0].x + nx;
            gRoomGeomDebugTris[k].y = tri.v[0].y + ny;
            gRoomGeomDebugTris[k].z = tri.v[0].z + nz;
            gRoomGeomDebugTris[k + 1].x = tri.v[1].x + nx;
            gRoomGeomDebugTris[k + 1].y = tri.v[1].y + ny;
            gRoomGeomDebugTris[k + 1].z = tri.v[1].z + nz;
            gRoomGeomDebugTris[k + 2].x = tri.v[2].x + nx;
            gRoomGeomDebugTris[k + 2].y = tri.v[2].y + ny;
            gRoomGeomDebugTris[k + 2].z = tri.v[2].z + nz;
        }
        poly.n = 3;
        fn_801D7F54(poly.p[0], &tri.v[0]);
        fn_801D7F54(poly.p[1], &tri.v[1]);
        fn_801D7F54(poly.p[2], &tri.v[2]);
        for (j = 0; j < 6; j++) {
            RoomGeomClipPoly(planes[j].d, &planes[j].x, &poly);
        }
        if (poly.n > 0) {
            Vec3 e1;
            Vec3 e2;
            float len;
            float depth;
            float* c;

            e1.x = tri.v[1].x - tri.v[0].x;
            e1.y = tri.v[1].y - tri.v[0].y;
            e1.z = tri.v[1].z - tri.v[0].z;
            e2.x = tri.v[2].x - tri.v[0].x;
            e2.y = tri.v[2].y - tri.v[0].y;
            e2.z = tri.v[2].z - tri.v[0].z;
            n.x = e1.y * e2.z - e1.z * e2.y;
            n.y = e1.z * e2.x - e1.x * e2.z;
            n.z = e1.x * e2.y - e1.y * e2.x;
            n.d = n.x * tri.v[0].x + n.y * tri.v[0].y + n.z * tri.v[0].z;
            len = fn_800E1020(&n);
            n.x /= len;
            n.y /= len;
            n.z /= len;
            n.d /= len;
            RoomGeomBoxCorners(&pos.x, half, &m, corners);
            depth = 0.0f;
            c = corners;
            for (j = 0; j < 4; j++) {
                float d = n.x * c[0] + n.y * c[1] + n.z * c[2] - n.d;

                if (d < 1e-5f && -d > depth) {
                    depth = -d;
                }
                d = n.x * c[3] + n.y * c[4] + n.z * c[5] - n.d;
                if (d < 1e-5f && -d > depth) {
                    depth = -d;
                }
                c += 6;
            }
            if (depth < 0.0f) {
                depth = 0.0f;
            }
            for (j = 0; j < poly.n && count < flags; j++) {
                ContactGeom* g = (ContactGeom*)((u8*)contacts + off);
                int k;
                int o;

                g->pos[0] = poly.p[j][0];
                g->pos[1] = poly.p[j][1];
                g->pos[2] = poly.p[j][2];
                g->pos[3] = surface;
                g->normal[0] = -n.x;
                g->normal[1] = -n.y;
                g->normal[2] = -n.z;
                g->normal[3] = 0.0f;
                g->depth = depth;
                g->count = 1;
                g->tri = i;
                o = 0;
                for (k = 0; k < count; k++) {
                    ContactGeom* h = (ContactGeom*)((u8*)contacts + o);
                    float dx = h->pos[0] - g->pos[0];
                    float dy = h->pos[1] - g->pos[1];
                    float dz = h->pos[2] - g->pos[2];

                    if (dx * dx + dy * dy + dz * dz < 0.001f) {
                        break;
                    }
                    o += skip;
                }
                if (k == count) {
                    off += skip;
                    count++;
                } else {
                    ContactGeom* h = (ContactGeom*)((u8*)contacts + k * skip);

                    h->normal[0] += g->normal[0];
                    h->normal[1] += g->normal[1];
                    h->normal[2] += g->normal[2];
                    h->normal[3] += g->normal[3];
                    h->depth += g->depth;
                    h->count++;
                }
            }
        }
    }
    for (i = 0; i < count; i++) {
        ContactGeom* c = (ContactGeom*)((u8*)contacts + i * skip);

        fn_8027914C(c->normal);
        c->depth /= (float)c->count;
        if (c->depth < -10.0f) {
            c->depth = -10.0f;
        } else if (c->depth > 10.0f) {
            c->depth = 10.0f;
        }
    }
    for (i = 0; i < count; i++) {
        ContactGeom* c = (ContactGeom*)((u8*)contacts + i * skip);

        if (fn_801D60A0(2) && gRoomGeomDebugNormalCount < 0x100) {
            int k = gRoomGeomDebugNormalCount * 2;

            gRoomGeomDebugNormalCount++;
            gRoomGeomDebugNormals[k].x = c->pos[0];
            gRoomGeomDebugNormals[k].y = c->pos[1];
            gRoomGeomDebugNormals[k].z = c->pos[2];
            gRoomGeomDebugNormals[k + 1].x = c->pos[0] + c->normal[0] * -128.0f;
            gRoomGeomDebugNormals[k + 1].y = c->pos[1] + c->normal[1] * -128.0f;
            gRoomGeomDebugNormals[k + 1].z = c->pos[2] + c->normal[2] * -128.0f;
        }
        c->pos[0] *= 0.0023818625f;
        c->pos[1] *= 0.0023818625f;
        c->pos[2] *= 0.0023818625f;
        c->depth *= 0.0023818625f;
        c->g1 = o1;
        c->g2 = o2;
    }
    return count;
}

// Draws the debug triangles (blue) and contact normals (red), then empties both lists.
void RoomGeomDebugDraw() {
    int i;
    int k;

    k = 0;
    for (i = 0; i < gRoomGeomDebugTriCount; i++) {
        fn_800090BC(&gRoomGeomDebugTris[k], &gRoomGeomDebugTris[k + 1], 0, 0xFF, 0);
        fn_800090BC(&gRoomGeomDebugTris[k + 1], &gRoomGeomDebugTris[k + 2], 0, 0xFF, 0);
        fn_800090BC(&gRoomGeomDebugTris[k + 2], &gRoomGeomDebugTris[k], 0, 0xFF, 0);
        k += 3;
    }
    k = 0;
    for (i = 0; i < gRoomGeomDebugNormalCount; i++) {
        fn_800090BC(&gRoomGeomDebugNormals[k], &gRoomGeomDebugNormals[k + 1], 0xFF, 0, 0);
        k += 2;
    }
    gRoomGeomDebugNormalCount = 0;
    gRoomGeomDebugTriCount = 0;
}

// The eight corners of a box (center, half lengths, rotation).
void RoomGeomBoxCorners(float* center, float* half, Mtx34* m, float* out) {
    Vec3 ax;
    Vec3 ay;
    Vec3 az;
    Vec3 dx;
    Vec3 dy;
    Vec3 dz;
    float l0, l1, l2;
    float h0, h1, h2;
    float x0, x1, x2;
    float y0, y1, y2;

    MtxGetColumn(&ax, m, 0);
    MtxGetColumn(&az, m, 2);
    MtxGetColumn(&ay, m, 1);
    dx.x = ax.x * half[0];
    dx.y = ax.y * half[0];
    dx.z = ax.z * half[0];
    dy.x = ay.x * half[1];
    dy.y = ay.y * half[1];
    dy.z = ay.z * half[1];
    dz.x = az.x * half[2];
    dz.y = az.y * half[2];
    dz.z = az.z * half[2];
    l0 = center[0] - dx.x;
    l1 = center[1] - dx.y;
    l2 = center[2] - dx.z;
    h0 = center[0] + dx.x;
    h1 = center[1] + dx.y;
    h2 = center[2] + dx.z;
    x0 = dy.x + l0;
    x1 = dy.y + l1;
    x2 = dy.z + l2;
    y0 = dy.x + h0;
    y1 = dy.y + h1;
    y2 = dy.z + h2;
    out[0] = dz.x + x0;
    out[1] = dz.y + x1;
    out[2] = dz.z + x2;
    out[3] = dz.x + y0;
    out[4] = dz.y + y1;
    out[5] = dz.z + y2;
    out[6] = dz.x + (h0 - dy.x);
    out[7] = dz.y + (h1 - dy.y);
    out[8] = dz.z + (h2 - dy.z);
    out[9] = dz.x + (l0 - dy.x);
    out[10] = dz.y + (l1 - dy.y);
    out[11] = dz.z + (l2 - dy.z);
    out[12] = x0 - dz.x;
    out[13] = x1 - dz.y;
    out[14] = x2 - dz.z;
    out[15] = y0 - dz.x;
    out[16] = y1 - dz.y;
    out[17] = y2 - dz.z;
    out[18] = (h0 - dy.x) - dz.x;
    out[19] = (h1 - dy.y) - dz.y;
    out[20] = (h2 - dy.z) - dz.z;
    out[21] = (l0 - dy.x) - dz.x;
    out[22] = (l1 - dy.y) - dz.y;
    out[23] = (l2 - dy.z) - dz.z;
}

// Clips a polygon against a plane (keeps the side where n.p - d >= 0).
void RoomGeomClipPoly(float d, float* plane, Poly* poly) {
    float dist[8];
    Poly out;
    int first = -1;
    int in = 0;
    int outside = 0;
    int i;
    int n = poly->n;
    int k;

    for (i = 0; i < n; i++) {
        dist[i] = 1e-8f + (plane[0] * poly->p[i][0] + plane[1] * poly->p[i][1] + plane[2] * poly->p[i][2] - d);
        if (dist[i] >= 0.0f) {
            in++;
            if (first < 0) {
                first = i;
            }
        } else {
            outside++;
        }
    }
    if (in <= 0) {
        poly->n = 0;
        return;
    }
    if (outside <= 0) {
        return;
    }
    k = 0;
    if (first > 0) {
        float t;
        int j = first;

        t = dist[j] / (dist[j] - dist[j - 1]);
        out.p[0][0] = t * (poly->p[j - 1][0] - poly->p[j][0]) + poly->p[j][0];
        out.p[0][1] = t * (poly->p[j - 1][1] - poly->p[j][1]) + poly->p[j][1];
        out.p[0][2] = t * (poly->p[j - 1][2] - poly->p[j][2]) + poly->p[j][2];
        k = 1;
        for (; j < n && dist[j] >= 0.0f; j++) {
            out.p[k][0] = poly->p[j][0];
            out.p[k][1] = poly->p[j][1];
            out.p[k][2] = poly->p[j][2];
            k++;
        }
        if (j >= n) {
            j = 0;
            i = n - 1;
        } else {
            i = j - 1;
        }
        t = dist[j] / (dist[j] - dist[i]);
        out.p[k][0] = t * (poly->p[i][0] - poly->p[j][0]) + poly->p[j][0];
        out.p[k][1] = t * (poly->p[i][1] - poly->p[j][1]) + poly->p[j][1];
        out.p[k][2] = t * (poly->p[i][2] - poly->p[j][2]) + poly->p[j][2];
        k++;
    } else {
        float t;
        int j;

        for (j = 0; j < n && dist[j] >= 0.0f; j++) {
            out.p[k][0] = poly->p[j][0];
            out.p[k][1] = poly->p[j][1];
            out.p[k][2] = poly->p[j][2];
            k++;
        }
        t = dist[j] / (dist[j] - dist[j - 1]);
        out.p[k][0] = t * (poly->p[j - 1][0] - poly->p[j][0]) + poly->p[j][0];
        out.p[k][1] = t * (poly->p[j - 1][1] - poly->p[j][1]) + poly->p[j][1];
        out.p[k][2] = t * (poly->p[j - 1][2] - poly->p[j][2]) + poly->p[j][2];
        k++;
        for (; j < n && dist[j] < 0.0f; j++) {
        }
        if (j < n) {
            t = dist[j] / (dist[j] - dist[j - 1]);
            out.p[k][0] = t * (poly->p[j - 1][0] - poly->p[j][0]) + poly->p[j][0];
            out.p[k][1] = t * (poly->p[j - 1][1] - poly->p[j][1]) + poly->p[j][1];
            out.p[k][2] = t * (poly->p[j - 1][2] - poly->p[j][2]) + poly->p[j][2];
            k++;
            for (; j < n && dist[j] >= 0.0f; j++) {
                out.p[k][0] = poly->p[j][0];
                out.p[k][1] = poly->p[j][1];
                out.p[k][2] = poly->p[j][2];
                k++;
            }
        } else {
            t = dist[0] / (dist[0] - dist[n - 1]);
            out.p[k][0] = t * (poly->p[n - 1][0] - poly->p[0][0]) + poly->p[0][0];
            out.p[k][1] = t * (poly->p[n - 1][1] - poly->p[0][1]) + poly->p[0][1];
            out.p[k][2] = t * (poly->p[n - 1][2] - poly->p[0][2]) + poly->p[0][2];
            k++;
        }
    }
    memcpy(poly, &out, k << 4);
    poly->n = k;
}

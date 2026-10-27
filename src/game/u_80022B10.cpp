// Actor support: owner/object links, visibility flags, collision bitsets,
// debug drawing, sphere meshes, CRC32 and allocation pools.
// Unit u_80022B10 (.text 0x80022B10-0x800265C8).

#pragma auto_inline off

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef float f32;

struct Actor;

// Node of both the owner-object lists and the per-actor object list.
// word 1: high byte = owner id, low 24 bits = next node index (signed).
struct ObjNode {
    Actor* actor;
    s32 owner : 8;
    s32 next : 24;
};

struct Owner {
    u8 pad0[0x20];
    u8 shape[0x28];   // 0x20, passed to fn_80031E48
    s32 head[3];      // 0x48
    u8 pad54[0x14];
};

struct Actor {
    u8 pad00[0x30];
    void (*msgFn)(int, void*);   // 0x30
    s8 layer;                    // 0x34
    u8 pad35[3];
    u32 id;                      // 0x38
    void* model;                 // 0x3C
    void* ptr40;                 // 0x40
    s32 objIndex;                // 0x44
    s32 objIndex2;               // 0x48
    u32 flags;                   // 0x4C
};

struct Msg {
    Actor* actor;
    s32 arg;
    s32 arg2;
    s32 arg3;
};

extern "C" {
extern ObjNode* lbl_804D41EC;   // node array
extern Owner* lbl_804D41D4;     // owner array
extern s32 lbl_804D41D0;        // owner count
extern s32 lbl_804D41E8;        // free node head
extern u32 lbl_803DCF7C[];      // owner id bit table
extern s32 lbl_80393C50[];      // triangle corner table (8 * 3)

void fn_8009F5B0(Actor*);
void fn_8009F50C(s32* out, const s32* in);
void fn_8009F6E4(s32* out, Actor*);
int fn_80031E48(void* box, void* shape);
void fn_8001CAAC(void* a, void* b, s32 nbits, Actor* actor);
int ActorPlayAnim(u32 id, u32 anim, u32 arg);
}

// ---------------------------------------------------------------------------

u32 ActorVisibleFlag(Actor* a) {
    return a->flags & 0x80;
}

int ActorRefreshHostState(Actor* a, int host) {
    s32 type = a->id >> 24;
    if (type == 1 || type == 2 || type == 0x13 || type == 0x14 ||
        type == 6 || type == 9 || type == 0xa || type == 0xc || type == 8) {
        int found = 0;
        for (s32 i = a->objIndex; i != -1; i = lbl_804D41EC[i].next) {
            s32 o = lbl_804D41EC[i].owner;
            if (lbl_803DCF7C[o / 32] & (1 << (o & 31))) {
                found = 1;
                break;
            }
        }
        if (host == -1) {
            a->flags |= 0x20;
            if (found) {
                if (a->msgFn) {
                    Msg m;
                    m.actor = a;
                    m.arg = 1;
                    if (!(a->flags & 0x40)) {
                        a->msgFn(6, &m);
                        ActorPlayAnim(a->id, 0xd29ded53, 0x24000000);
                    }
                    a->flags |= 0x40;
                }
            } else {
                if (a->msgFn) {
                    Msg m;
                    m.actor = a;
                    m.arg = 0;
                    if (a->flags & 0x40) {
                        a->msgFn(6, &m);
                        ActorPlayAnim(a->id, 0xb22a9c2c, 0x24000000);
                    }
                    a->flags &= ~0x40;
                }
            }
        } else {
            a->flags &= ~0x20;
            if (host == 1) {
                if (a->msgFn) {
                    Msg m;
                    m.actor = a;
                    m.arg = 1;
                    if (!(a->flags & 0x40)) {
                        a->msgFn(6, &m);
                        ActorPlayAnim(a->id, 0xd29ded53, 0x24000000);
                    }
                    a->flags |= 0x40;
                }
            } else {
                if (a->msgFn) {
                    Msg m;
                    m.actor = a;
                    m.arg = 0;
                    if (a->flags & 0x40) {
                        a->msgFn(6, &m);
                        ActorPlayAnim(a->id, 0xb22a9c2c, 0x24000000);
                    }
                    a->flags &= ~0x40;
                }
            }
        }
    }
    return 1;
}

int ActorIsActive(Actor* a) {
    return (a->flags & 0x1c) != 0;
}

int ActorFn80022D98(Actor* a, int arg) {
    a->flags &= ~0x101c;
    if (arg != 0) {
        s32 type = a->id >> 24;
        u32 bits = 0;
        if (a->ptr40 != 0) {
            if (type == 1 || type == 2 || type == 0x13 || type == 9 || type == 0xa)
                bits |= 8;
        }
        if (type == 1 || type == 2 || type == 0x13 || type == 0x14 ||
            type == 6 || type == 9 || type == 0x4a ||
            type == 0x57 || type == 0x58 || type == 0x5a || type == 0x5b)
            bits |= 4;
        if (a->ptr40 != 0) {
            if (type == 1 || type == 2 || type == 0x13 || type == 0xa || type == 9)
                bits |= 0x10;
        }
        if (type == 0x13)
            bits |= 0x1000;
        a->flags |= bits;
    }
    return 1;
}

void BitsetClear(void* bits, int nbits);

inline void FillOwnerBits(u32* bits, Actor* a) {
    if ((s32)(a->id >> 24) == 0xc)
        return;
    for (s32 i = a->objIndex; i != -1; i = lbl_804D41EC[i].next) {
        s32 o = lbl_804D41EC[i].owner;
        bits[o / 32] |= 1 << (o & 31);
    }
}

void ActorDetachOwnerObjects(Actor* a);
int ActorAttachOwnerObjects(Actor* a);

int ActorRelinkOwners(Actor* a) {
    if (a->model == 0)
        return 0;
    u32 before[32];
    u32 after[32];
    BitsetClear(before, 0x400);
    FillOwnerBits(before, a);
    ActorDetachOwnerObjects(a);
    ActorAttachOwnerObjects(a);
    BitsetClear(after, 0x400);
    FillOwnerBits(after, a);
    fn_8001CAAC(before, after, 0x400, a);
    return 1;
}

int ActorFn80022FD8(Actor* a) {
    return a->objIndex != -1;
}

void ActorUnlinkOwnerObjects(Actor* a) {
    u32 bits[32];
    u32 empty[32];
    BitsetClear(bits, 0x400);
    FillOwnerBits(bits, a);
    ActorDetachOwnerObjects(a);
    BitsetClear(empty, 0x400);
    fn_8001CAAC(bits, empty, 0x400, a);
    a->flags &= ~0x80;
}

inline s32& OwnerHead(Owner* o, int layer) {
    if (layer < 0 || layer >= 3)
        return o->head[0];
    return o->head[layer];
}

int ActorSetOwnerLayer(Actor* a, int layer) {
    if (layer >= 0 && layer < 3) {
        a->layer = (s8)layer;
        if (ActorAttachOwnerObjects(a)) {
            u32 empty[32];
            u32 bits[32];
            BitsetClear(empty, 0x400);
            BitsetClear(bits, 0x400);
            FillOwnerBits(bits, a);
            fn_8001CAAC(empty, bits, 0x400, a);
            return 1;
        }
    }
    return 0;
}

void ActorNoOp234AC() {
}

int ActorAttachOwnerObjects(Actor* a) {
    if (a == 0 || a->model == 0)
        return 0;
    if (a->objIndex != -1)
        ActorUnlinkOwnerObjects(a);
    a->objIndex = -1;
    a->objIndex2 = -1;
    int base = (int)a->model;
    s32 minx = 0x2000, miny = 0x2000, minz = 0x2000;
    s32 maxx = -0x2000, maxy = -0x2000, maxz = -0x2000;
    fn_8009F5B0(a);
    for (int i = 0; i < 8; i++) {
        s32 v[3];
        s32 out[3];
        v[0] = *(s32*)(base + lbl_80393C50[i * 3 + 0] * 4);
        v[1] = *(s32*)(base + lbl_80393C50[i * 3 + 1] * 4);
        v[2] = *(s32*)(base + lbl_80393C50[i * 3 + 2] * 4);
        fn_8009F50C(out, v);
        if (out[0] < minx) minx = out[0];
        if (out[1] < miny) miny = out[1];
        if (out[2] < minz) minz = out[2];
        if (out[0] > maxx) maxx = out[0];
        if (out[1] > maxy) maxy = out[1];
        if (out[2] > maxz) maxz = out[2];
    }
    s32 box[6];
    box[0] = minx;
    box[1] = miny;
    box[2] = minz;
    box[3] = maxx;
    box[4] = maxy;
    box[5] = maxz;
    s32 off[3];
    fn_8009F6E4(off, a);
    box[0] += off[0];
    box[1] += off[1];
    box[2] += off[2];
    box[3] += off[0];
    box[4] += off[1];
    box[5] += off[2];
    for (int i = 1; i < lbl_804D41D0; i++) {
        Owner* o = &lbl_804D41D4[i];
        if (fn_80031E48(box, o->shape)) {
            s32 idx = lbl_804D41E8;
            if (idx != -1) {
                ObjNode* n = &lbl_804D41EC[idx];
                lbl_804D41E8 = n->next;
                n->actor = a;
                n->owner = 1;
                n->next = OwnerHead(o, a->layer);
                OwnerHead(o, a->layer) = idx;
            }
            idx = lbl_804D41E8;
            if (idx != -1) {
                ObjNode* n = &lbl_804D41EC[idx];
                lbl_804D41E8 = n->next;
                n->next = a->objIndex;
                n->owner = 1;
                a->objIndex = idx;
            }
        }
    }
    return 1;
}

int ActorSetOwnerLayer(Actor* a, int layer);

int ActorFn800231A8(Actor* a) {
    if ((s32)(a->id >> 24) == 0xc)
        return ActorSetOwnerLayer(a,1);
    return ActorSetOwnerLayer(a,0);
}

void ActorDetachOwnerObjects(Actor* a) {
    s32 i = a->objIndex;
    if (i >= 0 && i < 0x1600) {
        while (i != -1) {
            ObjNode* n = &lbl_804D41EC[i];
            s8 owner = (s8)n->owner;
            if (owner == -1)
                break;
            s32 next = n->next;
            if (next == i)
                break;
            s8 layer = a->layer;
            i = next;
            Owner* o = &lbl_804D41D4[owner];
            s32 h = OwnerHead(o, layer);
            if (lbl_804D41EC[h].actor == a) {
                OwnerHead(o, layer) = lbl_804D41EC[h].next;
                lbl_804D41EC[h].actor = 0;
                lbl_804D41EC[h].next = lbl_804D41E8;
                lbl_804D41EC[h].owner = 0xff;
                lbl_804D41E8 = h;
            } else {
                s32 prev = h;
                s32 cur = lbl_804D41EC[prev].next;
                while (cur != -1) {
                    if (lbl_804D41EC[cur].actor == a) {
                        lbl_804D41EC[prev].next = lbl_804D41EC[cur].next;
                        lbl_804D41EC[cur].actor = 0;
                        lbl_804D41EC[cur].next = lbl_804D41E8;
                        lbl_804D41EC[cur].owner = 0;
                        lbl_804D41E8 = cur;
                        break;
                    }
                    prev = cur;
                    cur = lbl_804D41EC[cur].next;
                }
            }
            s32 j = a->objIndex;
            if ((s8)lbl_804D41EC[j].owner == owner) {
                a->objIndex = lbl_804D41EC[j].next;
                lbl_804D41EC[j].actor = 0;
                lbl_804D41EC[j].next = lbl_804D41E8;
                lbl_804D41EC[j].owner = 0xff;
                lbl_804D41E8 = j;
            } else {
                s32 prev = j;
                s32 cur = lbl_804D41EC[prev].next;
                while (cur != -1) {
                    if ((s8)lbl_804D41EC[cur].owner == owner) {
                        lbl_804D41EC[prev].next = lbl_804D41EC[cur].next;
                        lbl_804D41EC[cur].actor = 0;
                        lbl_804D41EC[cur].next = lbl_804D41E8;
                        lbl_804D41EC[cur].owner = 0;
                        lbl_804D41E8 = cur;
                        break;
                    }
                    prev = cur;
                    cur = lbl_804D41EC[cur].next;
                }
            }
        }
    }
    a->objIndex = -1;
    a->objIndex2 = -1;
}

struct Mtx34 {
    f32 m[12];
};

struct Vec3 {
    f32 x, y, z;
};

extern "C" void memset(void*, int, u32);
extern "C" void* memcpy(void*, const void*, u32);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void fn_8009F76C(void*);
extern "C" int fn_800090BC(Vec3*, Vec3*, u8, u8, u8);
void DebugDrawBoneBox(Vec3*, void*, int, int, int);
extern "C" void* fn_80037B6C(int);
extern "C" u32 fn_80037230(int, u16);
Vec3* ModelBonePositions(int);
void Vec3Copy(Vec3*, Vec3*);

int ActorFn8002378C(Actor* a, int id, int mtx, int msgFn, int model,
                               int ptr40, int p6) {
    memset(a, 0, 0x50);
    if ((u32)mtx != 0)
        *(Mtx34*)a = *(Mtx34*)mtx;
    else
        fn_8009F76C(a);
    a->msgFn = (void (*)(int, void*))msgFn;
    a->id = (u32)id;
    a->ptr40 = (void*)ptr40;
    a->model = (void*)model;
    a->objIndex = -1;
    a->objIndex2 = -1;
    a->flags = 0;
    ActorFn80022D98(a, 1);
    return ActorRefreshHostState(a, -1);
}

int ActorReturnOne() {
    return 1;
}

f32 DistPointLineSq(const f32* a, const f32* b, const f32* dir,
                           f32* tOut, f32* pOut) {
    f32 dy = a[1] - b[1];
    f32 dx = a[0] - b[0];
    f32 dz = a[2] - b[2];
    f32 dir0sq = dir[0] * dir[0];
    f32 dir1sq = dir[1] * dir[1];
    f32 dir2sq = dir[2] * dir[2];
    f32 den = dir0sq + dir1sq + dir2sq;
    f32 num = dir[0] * dx + dir[1] * dy + dir[2] * dz;
    f32 t = num / den;
    f32 px = dir[0] * t;
    f32 py = dir[1] * t;
    f32 pz = dir[2] * t;
    px += b[0];
    py += b[1];
    pz += b[2];
    f32 ex = a[0] - px;
    f32 ey = a[1] - py;
    f32 ez = a[2] - pz;
    if (tOut)
        *tOut = t;
    if (pOut) {
        pOut[0] = px;
        pOut[1] = py;
        pOut[2] = pz;
    }
    return ey * ey + ex * ex + ez * ez;
}

u32 OwnerGetInfo(u32 owner) {
    u32 info[6];
    info[0] = 0;
    SendMsg(owner, 0x416, info);
    return info[0];
}

void DebugDrawModelBones(int model, int count, u8* colors, int filter) {
    Vec3* pos = ModelBonePositions(model);
    u16* idx = (u16*)fn_80037B6C(model);
    for (int i = 0; i < count; i++, pos++, idx++, colors += 0x30) {
        if (filter != -1 && filter != i)
            continue;
        Vec3 v;
        Vec3Copy(&v, pos);
        u32 f = fn_80037230(model, *idx);
        if (f & 0x1000)
            DebugDrawBoneBox(&v, colors, 0xff, 0xff, 0);
        else if (f & 0x800)
            DebugDrawBoneBox(&v, colors, 0, 0xff, 0);
        else if (f & 0x2000)
            DebugDrawBoneBox(&v, colors, 0, 0, 0xff);
        else
            DebugDrawBoneBox(&v, colors, 0xff, 0xff, 0xff);
    }
}

extern "C" const float lbl_80342208[54];
extern "C" const s32 lbl_803422E0[96];

void DebugDrawSphereWire(int cr, int cg, int cb, f32 cx, f32 cy, f32 cz,
                            f32 rad) {
    Vec3 v[20];
    Vec3* p = v;
    const f32* src = lbl_80342208;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            p[j].x = cx + rad * (0.01f * src[j * 3 + 0]);
            p[j].y = cy + rad * (0.01f * src[j * 3 + 1]);
            p[j].z = cz + rad * (0.01f * src[j * 3 + 2]);
        }
        src += 15;
        p += 5;
    }
    const s32* tri = lbl_803422E0;
    for (int i = 0; i < 32; i++, tri += 3) {
        int a = tri[0];
        int b = tri[1];
        int c = tri[2];
        fn_800090BC(&v[a], &v[b], (u8)cr, (u8)cg, (u8)cb);
        fn_800090BC(&v[b], &v[c], (u8)cr, (u8)cg, (u8)cb);
        fn_800090BC(&v[c], &v[a], (u8)cr, (u8)cg, (u8)cb);
    }
}

int ActorRefreshHostState(Actor* a, int host);

struct DbgVertex {
    Vec3 p;
    f32 zero0;
    f32 zero1;
    u8 r, g, b, a;
};

struct DbgTri {
    s32 type;
    s32 unk;
    u16 i0, i1, i2;
    u16 pad;
};

struct DbgList {
    s32 field0;
    void* verts;
    void* tris;
    u16 nverts;
    u16 ntris;
};

extern "C" void fn_80044948(DbgList*);
extern "C" void fn_800A0100(void*);
extern "C" void fn_800A0094(Vec3*, Vec3*);

void DebugDrawSphere(int r, int g, int b, int a, f32 cx, f32 cy, f32 cz,
                     f32 rad) {
    DbgTri tris[32];
    DbgVertex verts[20];
    const f32* src = lbl_80342208;
    for (int i = 0; i < 20; i++) {
        verts[i].r = (u8)r;
        verts[i].g = (u8)g;
        verts[i].b = (u8)b;
        verts[i].a = (u8)a;
        verts[i].p.x = cx + rad * (0.01f * src[i * 3 + 0]);
        verts[i].p.y = cy + rad * (0.01f * src[i * 3 + 1]);
        verts[i].p.z = cz + rad * (0.01f * src[i * 3 + 2]);
        verts[i].zero0 = 0.0f;
        verts[i].zero1 = 0.0f;
    }
    for (int i = 0; i < 32; i++) {
        tris[i].unk = 0;
        tris[i].type = -3;
        tris[i].i0 = (u16)lbl_803422E0[i * 3 + 0];
        tris[i].i1 = (u16)lbl_803422E0[i * 3 + 1];
        tris[i].i2 = (u16)lbl_803422E0[i * 3 + 2];
    }
    DbgList list;
    list.tris = tris;
    list.ntris = 32;
    list.verts = verts;
    list.nverts = 20;
    list.field0 = 0;
    fn_80044948(&list);
}

void DebugDrawBoneBox(Vec3* p, void* bonep, int cr, int cg, int cb) {
    u8* bone = (u8*)bonep;
    Vec3 v[8];
    v[0].x = p[0].x;
    v[0].y = p[1].y;
    v[0].z = p[1].z;
    v[1].x = p[1].x;
    v[1].y = p[1].y;
    v[1].z = p[1].z;
    v[2].x = p[0].x;
    v[2].y = p[1].y;
    v[2].z = p[0].z;
    v[3].x = p[1].x;
    v[3].y = p[1].y;
    v[3].z = p[0].z;
    v[4].x = p[0].x;
    v[4].y = p[0].y;
    v[4].z = p[1].z;
    v[5].x = p[1].x;
    v[5].y = p[0].y;
    v[5].z = p[1].z;
    v[6].x = p[0].x;
    v[6].y = p[0].y;
    v[6].z = p[0].z;
    v[7].x = p[1].x;
    v[7].y = p[0].y;
    v[7].z = p[0].z;
    fn_800A0100(bone);
    for (int i = 0; i < 8; i++) {
        fn_800A0094(&v[i], &v[i]);
        v[i].x += *(f32*)(bone + 0x24);
        v[i].y += *(f32*)(bone + 0x28);
        v[i].z += *(f32*)(bone + 0x2c);
    }
    fn_800090BC(&v[0], &v[1], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[0], &v[2], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[0], &v[4], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[3], &v[1], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[3], &v[2], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[3], &v[7], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[5], &v[1], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[5], &v[4], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[5], &v[7], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[6], &v[2], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[6], &v[4], (u8)cr, (u8)cg, (u8)cb);
    fn_800090BC(&v[6], &v[7], (u8)cr, (u8)cg, (u8)cb);
}

// ---------------------------------------------------------------------------
// Debug axes, snapshot buffers, CRC32, bit pools and message handlers.

extern "C" void fn_800A025C(Vec3*, void*, int);
extern "C" f32 fn_800A1394(f32);
extern "C" f32 fn_800A109C(f32);
extern "C" f32 fn_800A127C(f32);
extern "C" int fn_800A1494(int);
extern "C" int fn_800A14F8(int);
extern "C" int fn_800A1624(int);
extern "C" int fn_800A1688(int);
void* VAlloc(int);
void VFree(void*);
extern "C" void fn_80026660(int, void*, int);
extern "C" int fn_80088C10(int, int, int, int, int, int, int, int);
extern "C" int fn_80088BA0(int, int, int, int, int, int, int);
extern "C" int fn_80088FB0(int, int);
extern "C" int fn_80088F0C(int, int);
extern "C" int fn_80088E68(int, int);
int Rand();
extern "C" int fn_800E0DBC(int*);
void ActorGetMatrix(Mtx34*, Actor*);
extern "C" void fn_8016D864(void*, void*);
extern "C" int fn_80018674(void*, Mtx34*, void*, Mtx34*);
int ActorAim(int*, Actor*, int);
extern "C" int stricmp(const char*, const char*);
void DebugPrintf(const char*, ...);

inline void DrawAxis(Vec3* p, void* a, int axis, f32 s, Vec3* start, int cr,
                     int cg, int cb) {
    Vec3 dir;
    fn_800A025C(&dir, a, axis);
    dir.x *= s;
    dir.y *= s;
    dir.z *= s;
    Vec3 end = *p;
    end.x += dir.x;
    end.y += dir.y;
    end.z += dir.z;
    fn_800090BC(start, &end, (u8)cr, (u8)cg, (u8)cb);
}

void DebugDrawAxes(Vec3* p, void* a, f32 s) {
    Vec3 start = *p;
    DrawAxis(p, a, 0, s, &start, 255, 0, 0);
    DrawAxis(p, a, 1, s, &start, 0, 255, 0);
    DrawAxis(p, a, 2, s, &start, 0, 0, 255);
}

struct SkinBone {
    u8 pad0[0x10];
    Vec3 vec;        // 0x10
    u8 pad1[0x1c];
    s32 field38;     // 0x38
};

struct Skin {
    u32 f00;            // 0x00
    u8 cr, cg, cb, ca;  // 0x04
    s32 count;          // 0x08
    u32 f0C, f10, f14, f18, f1C;
    SkinBone bones[8];  // 0x20
};

u8 lbl_803DD598[0x400];
Skin lbl_803DD998;
s32 lbl_804D2658[2] = {1, 0};

Skin* SkinBlendTint(Skin* src, int r, int g, int b, f32 t) {
    if (!src)
        return 0;
    t = fn_800A1394(90.0f * t);
    lbl_803DD998 = *src;
    f32 inv = 1.0f - t;
    Skin* d = &lbl_803DD998;
    d->cr = (u8)((f32)d->cr * inv + (f32)r * t);
    d->cg = (u8)((f32)d->cg * inv + (f32)g * t);
    d->cb = (u8)((f32)d->cb * inv + (f32)b * t);
    for (int i = 0; i < d->count; i++) {
        d->bones[i].vec.x *= inv;
        d->bones[i].vec.y *= inv;
        d->bones[i].vec.z *= inv;
        d->bones[i].field38 = -1;
    }
    return d;
}

void ActorNoOp24514() {}
void ActorNoOp24518() {}

int AuxMsgHandler32(void*, int, int*);
int AuxMsgHandler6300(int, int, int*);
int AuxAimAngle(int*);

void RegisterAuxMsgHandlers() {
    fn_80026660(0x32, (void*)AuxMsgHandler32, 0);
    fn_80026660(0x3f, (void*)AuxMsgHandler6300, 0);
}

u32 Crc32Compute(u8* data, int len) {
    if (lbl_804D2658[0] != 0) {
        for (u32 i = 0; i < 256; i++) {
            u32 c = i;
            for (int j = 0; j < 8; j++)
                c = (c & 1) ? ((c >> 1) ^ 0xEDB88320) : (c >> 1);
            ((u32*)lbl_803DD598)[i] = c;
        }
        lbl_804D2658[0] = 0;
    }
    u32 crc = 0xffffffff;
    while (len != 0) {
        s32 b = (s32)(f32)*data;
        crc = (crc >> 8) ^ ((u32*)lbl_803DD598)[(crc ^ b) & 0xff];
        data++;
        len--;
    }
    return crc ^ 0xFFFFFFFF;
}

void ListRemoveFree(void* p, void** list, u32* bits, int count) {
    if (!p)
        return;
    if (list) {
        for (int i = 0; i < count; i++) {
            if (list[i] == p) {
                bits[i / 32] &= ~(1u << (i & 31));
                VFree(p);
                list[i] = 0;
            }
        }
    } else {
        VFree(p);
    }
}

inline s32 FindFreeBit(u32* bits, s32 max) {
    s32 words = max / 32 + (max % 32 != 0);
    s32 found = -1;
    for (s32 i = 0; i < words; i++) {
        u32 w = bits[i];
        if (w == 0xffffffff)
            continue;
        for (s32 j = 0; j < 32; j++) {
            if (!(w & (1u << j))) {
                s32 idx = j + (i << 5);
                if (idx < max)
                    found = idx;
                else
                    found = -1;
                goto done;
            }
        }
    }
done:
    return found;
}

void* ListAlloc(void** arr, int size, u32* bits, int max) {
    if (arr) {
        s32 words = max / 32 + (max % 32 != 0);
        s32 found;
        u32* p = bits;
        for (s32 i = 0; i < words; i++, p++) {
            u32 w = *p;
            if (w == 0xffffffff)
                continue;
            for (s32 j = 0; j < 32; j++) {
                if (!(w & (1u << j))) {
                    s32 idx = j + (i << 5);
                    if (idx < max)
                        found = idx;
                    else
                        found = -1;
                    goto done;
                }
            }
        }
        found = -1;
    done:
        if (found == -1)
            return 0;
        arr[found] = VAlloc(size);
        if (arr[found]) {
            bits[found / 32] |= 1u << (found & 31);
            return arr[found];
        }
        return 0;
    }
    return VAlloc(size);
}

int PoolFree(void* base, void* ptr, int stride,
                                  void* bitsv) {
    u32* bits = (u32*)bitsv;
    int idx = ((u8*)base - (u8*)ptr) / stride;
    bits[idx / 32] &= ~(1u << (idx & 31));
    return 1;
}

void* PoolAlloc(void* pool, int stride, void* bitsv,
                                    int max) {
    u32* bits = (u32*)bitsv;
    s32 idx = FindFreeBit(bits, max);
    if (idx == -1)
        return 0;
    bits[idx / 32] |= 1u << (idx & 31);
    return (u8*)pool + idx * stride;
}

int BitsetSubset(u32* a, u32* b, int n) {
    int words = (n + 31) / 32;
    u32* pa = a;
    u32* pb = b;
    for (int i = 0; i < words; i++) {
        if ((*pa | *pb) != *pa)
            return 0;
        pa++;
        pb++;
    }
    return 1;
}

int BitsetEqual(u32* a, u32* b, int n) {
    int words = (n + 31) / 32;
    for (int i = 0; i < words; i++)
        if (a[i] != b[i])
            return 0;
    return 1;
}

void BitsetNot(u32* dst, u32* src, int n) {
    int i;
    int words = (n + 31) / 32;
    for (i = 0; i < words; i++)
        dst[i] = ~src[i];
}

static inline void BitsetZero(u32* p, int nbits) {
    int i;
    int words = (nbits + 31) / 32;
    for (i = 0; i < words; i++)
        p[i] = 0;
}

void BitsetFromIndices(u32* bits, int n, const s32* idx, int count) {
    BitsetZero(bits, n);
    for (int i = 0; i < count; i++) {
        s32 v = idx[i];
        bits[v / 32] |= 1u << (v & 31);
    }
}

s32 BitsetToIndices(s32* out, int max, u32* bits, int n) {
    s32 count = 0;
    s32 base = 0;
    int words = (n + 31) / 32;
    for (int i = 0; i < words; i++) {
        if (*bits) {
            s32 idx = base;
            for (int j = 0; j < 32; j++) {
                if (*bits & (1u << j)) {
                    out[count] = idx;
                    if (++count >= max)
                        return count;
                }
                idx++;
            }
        }
        bits++;
        base += 32;
    }
    return count;
}

u32 BitsetAndAcc(u32* dst, u32* a, u32* b, int n) {
    int i;
    u32 acc = 0;
    int words = (n + 31) / 32;
    if (dst) {
        for (i = 0; i < words; i++) {
            dst[i] = a[i] & b[i];
            acc |= dst[i];
        }
    } else {
        for (i = 0; i < words; i++)
            acc |= a[i] & b[i];
    }
    return acc;
}

void BitsetOr(u32* dst, u32* a, u32* b, int n) {
    int i;
    int words = (n + 31) / 32;
    for (i = 0; i < words; i++)
        dst[i] = a[i] | b[i];
}

u32 BitsetXorAcc(u32* dst, u32* a, u32* b, int n) {
    int i;
    u32 acc = 0;
    int words = (n + 31) / 32;
    if (dst) {
        for (i = 0; i < words; i++) {
            dst[i] = a[i] ^ b[i];
            acc |= dst[i];
        }
    } else {
        for (i = 0; i < words; i++)
            acc |= a[i] ^ b[i];
    }
    return acc;
}

void BitsetNandClear(u32* a, u32* b, int n) {
    int words = (n + 31) / 32;
    for (int i = 0; i < words; i++) {
        u32 av = a[i];
        u32 bv = b[i];
        u32 t = ~(av & bv);
        a[i] = av & t;
        b[i] = bv & t;
    }
}

void BitsetClear(void* bitsv, int nbits) {
    u32* p = (u32*)bitsv;
    int i;
    int words = (nbits + 31) / 32;
    for (i = 0; i < words; i++)
        p[i] = 0;
}

void BitsetSetAll(void* bitsv, int nbits) {
    int i;
    u32* p = (u32*)bitsv;
    int words = (nbits + 31) / 32;
    for (i = 0; i < words; i++)
        p[i] = 0xffffffff;
}

void BitsetAllocIndices(u32* bits, int n, s32* out, int count) {
    int words = (n - 1) / 32 + 1;
    s32 base = 0;
    for (int i = 0; i < words; i++, bits++, base += 32) {
        u32 w = *bits;
        if (w == 0xffffffff)
            continue;
        s32 idx = base;
        for (int j = 0; j < 32; j++) {
            if (!(w & (1u << j))) {
                *bits |= 1u << j;
                *out++ = idx;
                if (--count == 0)
                    return;
            }
            idx++;
        }
    }
}

s32 BitsetCount(u32* bits, int n, int flag) {
    int words = (n + 31) / 32;
    int remaining = n;
    s32 count = 0;
    for (int i = 0; i < words; i++) {
        int nb = remaining < 32 ? remaining : 32;
        u32 w = bits[i];
        if (w != 0) {
            for (int j = 0; j < nb; j++)
                count += (w >> j) & 1;
        }
        remaining -= 32;
    }
    return flag ? count : n - count;
}

s32 BitFind(u32* bits, int n, int flag) {
    int i;
    int j;
    s32 w;
    int words;
    s32 idx;

    if (flag)
        flag = 1;
    words = n / 32 + 1;
    for (i = 0; i < words; i++) {
        w = bits[i];
        if (flag != 1 || w == 0) {
            if (w == 0xffffffff)
                continue;
        }
        for (j = 0; j < 32; j++) {
            if ((s32)(w & (1u << j)) == flag) {
                idx = i * 32 + j;
                if (idx >= n)
                    return -2;
                return idx;
            }
        }
    }
    return -1;
}

s32 BitsetFirstSet(u32* bits, int words, int n) {
    int i, j;
    s32 w;
    for (i = 0; i < words; i++) {
        w = bits[i];
        if (w != 0) {
            for (j = 0; j < 32; j++) {
                if (w & (1u << j)) {
                    s32 idx = i * 32 + j;
                    if (idx >= n)
                        return -1;
                    return idx;
                }
            }
        }
    }
    return -1;
}

s32 BitsetFirstClear(u32* bits, int words, int n) {
    int i, j;
    u32 w;
    for (i = 0; i < words; i++) {
        w = bits[i];
        if (w == 0xffffffff)
            continue;
        for (j = 0; j < 32; j++) {
            if (!(w & (1u << j))) {
                s32 idx = i * 32 + j;
                if (idx >= n)
                    return -1;
                return idx;
            }
        }
    }
    return -1;
}

void BitToggle(u32* bits, int idx) {
    int word = idx / 32;
    u32 m = 1u << (idx & 31);
    if (bits[word] & m)
        bits[word] &= ~m;
    else
        bits[word] |= m;
}

int BitTestAndClear(u32* bits, int idx) {
    int word = idx / 32;
    u32 m = 1u << (idx & 31);
    if (bits[word] & m) {
        bits[word] &= ~m;
        return 1;
    }
    return 0;
}

void BitClear(u32* bits, int idx) {
    bits[idx / 32] &= ~(1u << (idx & 31));
}

void BitsetCopy(void* dstv, void* srcv, int n) {
    u32* dst = (u32*)dstv;
    u32* src = (u32*)srcv;
    int i;
    int words = (n + 31) / 32;
    for (i = 0; i < words; i++)
        dst[i] = src[i];
}

int AuxMsgHandler6300(int unused, int msg, int* a) {
    switch (msg) {
    case 0x6300:
        fn_80088C10(a[1], a[2], a[4], a[5], a[6], a[8], a[9], a[10]);
        break;
    case 0x6301:
        fn_80088BA0(a[1], a[4], a[5], a[6], a[8], a[9], a[10]);
        break;
    case 0x6302:
        fn_80088FB0(a[1], a[2]);
        break;
    case 0x6303:
        fn_80088F0C(a[1], a[2]);
        break;
    case 0x6304:
        fn_80088E68(a[1], a[2]);
        break;
    default:
        return 0;
    }
    return 1;
}

struct Buf402 {
    s32 f0;          // 0x00
    s16 f4;          // 0x04
    u8 pad0[0xa];    // 0x06
    void* actor;     // 0x10
    u8 pad1[0x20];   // 0x14
    s32 px, py, pz;  // 0x34
};

int AuxMsgHandler32(void* self, int msg, int* arg) {
    switch (msg) {
    case 0x4000: {
        int lo = arg[1];
        int hi = arg[2];
        arg[3] = lo + Rand() % (hi - lo);
        return 1;
    }
    case 0x4001: {
        int buf[2];
        buf[0] = arg[1];
        ((Actor*)arg[1])->msgFn(0x400, buf);
        arg[2] = buf[1];
        return 1;
    }
    case 0x4003: {
        Buf402 a;
        Buf402 b;
        a.f0 = 0;
        a.f4 = 0;
        b.f0 = 0;
        b.f4 = 0;
        if (!SendMsg(arg[1], 0x402, &a))
            return 0;
        if (!SendMsg(arg[2], 0x402, &b))
            return 0;
        int d[3];
        d[0] = a.px - b.px;
        d[1] = a.py - b.py;
        d[2] = a.pz - b.pz;
        arg[3] = fn_800E0DBC(d);
        return 1;
    }
    case 0x4004:
        return AuxAimAngle(arg);
    case 0x4005:
        switch (arg[1]) {
        case 0x140b8:
            arg[3] = fn_800A1624(arg[2]);
            return 1;
        case 0x10567:
            arg[3] = fn_800A1688(arg[2]);
            return 1;
        case 0x14381:
            arg[3] = (int)(4096.0f * fn_800A127C(0.087890625f * (f32)arg[2]));
            return 1;
        case 0x738e3d46:
            arg[3] = fn_800A1494(arg[2]);
            return 1;
        case 0x738e01f5:
            arg[3] = fn_800A14F8(arg[2]);
            return 1;
        }
        arg[3] = 0;
        return 0;
    case 0x4007: {
        int a = 0;
        int b = 0;
        if (!SendMsg(arg[1], 0x400, &a)) {
            arg[3] = 0;
            return 0;
        }
        if (!SendMsg(arg[2], 0x400, &b)) {
            arg[3] = 0;
            return 0;
        }
        Mtx34 m1, m2;
        ActorGetMatrix(&m1, (Actor*)a);
        ActorGetMatrix(&m2, (Actor*)b);
        u8 v1[0x18], v2[0x18];
        fn_8016D864(v1, ((Actor*)a)->ptr40);
        fn_8016D864(v2, ((Actor*)b)->ptr40);
        if (fn_80018674(v1, &m1, v2, &m2)) {
            arg[3] = 1;
            return 1;
        }
        arg[3] = 0;
        return 0;
    }
    }
    return 0;
}

inline f32 NormVec(Vec3& v) {
    if ((__fabs(v.z) > 1.1920929e-07f) + (__fabs(v.x) > 1.1920929e-07f) +
            (__fabs(v.y) > 1.1920929e-07f) ==
        0) {
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 0.0f;
        return 0.0f;
    }
    f32 len = v.x * v.x + v.y * v.y + v.z * v.z;
    f32 inv = __frsqrte(len);
    v.x *= inv;
    v.y *= inv;
    v.z *= inv;
    return __fres(len);
}

int AuxAimAngle(int* arg) {
    Buf402 a, b;
    a.f0 = 0;
    a.f4 = 0;
    b.f0 = 0;
    b.f4 = 0;
    if (!SendMsg(arg[1], 0x402, &a))
        return 0;
    if (!SendMsg(arg[2], 0x402, &b))
        return 0;
    int aim[3];
    ActorAim(aim, (Actor*)a.actor, 1);
    Vec3 v1;
    v1.x = (f32)aim[0];
    v1.y = (f32)aim[1];
    v1.z = (f32)aim[2];
    v1.z = 0.0f;
    Vec3 v2;
    v2.x = (f32)(b.px - a.px);
    v2.y = (f32)(b.py - a.py);
    v2.z = (f32)(b.pz - a.pz);
    v2.z = 0.0f;
    NormVec(v1);
    NormVec(v2);
    f32 dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    f32 cross = v1.x * v2.y - v1.y * v2.x;
    f32 ang = fn_800A109C(dot);
    if (!(cross > 0.0f))
        ang = -ang;
    arg[3] = (int)ang;
    return 1;
}

u32 lbl_804D427C;

int SelectionSetTest(u32 mask) {
    if (lbl_804D427C == 0)
        return 1;
    return (lbl_804D427C & mask) != 0;
}

struct SelSetEntry {
    char* name;
    u8 pad[0xc];
};

struct SelSetTable {
    u8 pad0[8];
    s32 count;              // 0x08
    SelSetEntry* entries;   // 0x0C
};

extern SelSetTable* lbl_804D4278;

void SelectionSetAdd(char* name) {
    for (s32 i = 0; i != lbl_804D4278->count; i++) {
        if (stricmp(lbl_804D4278->entries[i].name, name) == 0) {
            lbl_804D427C |= 1u << i;
            return;
        }
    }
    DebugPrintf("SelectionSet %s not found\n", name);
}

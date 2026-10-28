// u_80231550.cpp: shot pool and glass shatter effect (0x80231550-0x80234AD8).
// Two pointer arrays (live shots, live glass shards), the glass shatter effect (shard physics,
// drawing, the per-id message handler) and the GX async-draw thread helpers at the end.  The
// array class members (constructors, destructors, range helpers at 0x802315C0-0x802317C4,
// 0x802338B0 and 0x8023462C-0x80234764) and the three large functions (ShotFire 0x80231984,
// 0x80233070, 0x80233E88) are not drafted yet.  Names are descriptive guesses; unmatched
// against retail so far.

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

// Growable pointer array (Buffer-derived container with a virtual destructor).
class PtrArray {
public:
    virtual ~PtrArray();    // vtable pointer at 0x00
    int unk04;              // 0x04
    u32 bytes;              // 0x08: used size in bytes
    char* data;             // 0x0C
    int unk10;              // 0x10
};

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[12];
};

struct Actor {
    u8 pad00[0x38];
    int handle;             // 0x38
};

int ShardClip(char* shard, float* rect);

// One glass shard (0xEC bytes).
struct Shard {
    float rot[12];          // 0x00 orientation matrix (3x4)
    float verts[8][3];      // 0x30 (up to 8 outline points)
    float vel[3];           // 0xE0? (see ShardUpdate)
};

extern "C" {
void* VAlloc(int size);
void VFree(void* p);
int AssetFind(u32 a, u32 b);
extern unsigned char __upper_map[256];
void fn_8028B35C(PtrArray* a, void* it, int bytes);
void fn_8028BA9C(PtrArray* a);
float fn_802A2080(void);
void fn_80026660(int msg, void* fn, int arg);
int fn_800F1138(int a, void* pos, void* fn, void* arg);
void ActorGetMatrix(void* m, void* actor);
void fn_80122370(void);
void fn_8021284C(int id);
int fn_80170834(const char* name);
void fn_8000F874(int tex);
void fn_801123E4(int a, int b);
void fn_8013A30C(int a, void* m);
void GXBegin(int prim, int fmt, int n);
void fn_8013A2E8(void);
}

extern PtrArray* gShotArray;    // 0x804D5440
extern PtrArray* gShardArray;   // 0x804D544C
extern int gShardInit;          // 0x804D5448
extern float gShardZero;        // 0x804D9848
extern float gShardOne;         // 0x804D9838
extern int gGlassTexId;         // 0x804D349C (short)
extern int gGlassFxId;          // 0x804D3498
extern int gGfxDisabled;        // 0x804D4084

inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    u32 h = 0;
    int i;
    for (i = 0; s[i] != 0; i++) {
        h *= 31;
        h += (unsigned char)toupper(s[i]);
    }
    h = h % 0xFFFFFFFF;
    return h;
}

// Functions called from other units under their placeholder names (names in comments).
extern "C" void fn_80233848(void);
extern "C" void fn_80233928(void);
extern "C" void fn_80233A20(float dt);
extern "C" void fn_80233B60(void);
extern "C" void fn_80233C30(void);

// Empties and deletes the shot array.
void ShotArrayShutdown(void) {
    u32 bytes = gShotArray->bytes;
    if (bytes >> 2 != 0) {
        char* begin = gShotArray->data;
        fn_8028B35C(gShotArray, begin, (int)((void**)(begin + bytes) - (void**)begin) * 4);
    }
    delete gShotArray;
    gShotArray = NULL;
}

// Creates the shot array.
void ShotArrayInit(void) {
    PtrArray* a = (PtrArray*)VAlloc(0x14);
    if (a != NULL) {
        fn_8028BA9C(a);
    }
    gShotArray = a;
}

// Message handler for a shot: answers the position query (0x400), hands out the shot's
// placement matrix (0x411/0x412) and takes a new one (0x413).
int ShotMsg(int msg, int* m) {
    float one = gShardOne;
    int* shot = (int*)m[0];
    if (shot == NULL) {
        return 0;
    }
    if (msg < 0x411) {
        if (msg == 0x400) {
            m[1] = shot[0xE];
            m[3] = 1;
            ((float*)m)[4] = one;
            ((float*)m)[5] = one;
            m[6] = 0;
            m[7] = 0;
            return 1;
        }
    } else {
        if (msg == 0x413) {
            int* src = (int*)m[1];
            int i;
            if (shot == NULL) {
                return 0;
            }
            shot[0x17] = src[9] - shot[9];
            shot[0x18] = src[10] - shot[10];
            shot[0x19] = src[11] - shot[11];
            for (i = 0; i < 12; i++) {
                shot[i] = src[i];
            }
            return 1;
        }
        if (msg < 0x413) {
            int i;
            if (shot == NULL) {
                return 0;
            }
            for (i = 0; i < 12; i++) {
                m[4 + i] = shot[i];
            }
            return 1;
        }
    }
    return 0;
}

extern int gShardRings;             // 0x804D34C4 (5)
extern int gShardSpokes;            // 0x804D34B4 (12)
extern int gShardSegs;              // 0x804D34BC (7)
extern int gShardEdge;              // 0x804D34C8
extern float gShardRadius0;         // 0x804D34AC (30.0)
extern float gShardRadiusGrow;      // 0x804D34B0 (1.5)
extern float gShardJitter;          // 0x804D34A0
extern float gShardPlaneW;          // 0x804D34A4
extern float gShardAngle;           // 0x804D34A8
extern float gShardUvScale;         // 0x804D34C0
extern float gShardUvBias;          // 0x804D34B8
extern float gShardNan;             // 0x804D3E40
extern int gShardCounter;           // 0x804D5448 (shared with GlassInit)
extern "C" {
int fn_8003CC90(int a, int b, float* u0, float* v0, float* u1, float* v1);
u32 fn_80010000(void);
void ViewPlaneFromNormal(float* plane, float* n, float* p);         // 0x8001734C
void fn_8016F120(int fx, int* pos);
int fn_80121D60(float a, float b, float c, float d, float e, float f, float g, int h, int handle, float* pos, u32 hash);
void ActorFn8002378C(void* a, u32 id, void* info, void* fn, int b, int c, int d);
void PlacementToMatrix(void* a, void* m);
void fn_80212814(u32 id, u32 owner, int a, int b, int c, int d);
int fn_800201E4(int a, float* seg, u32 mask, void* fn, float* out);
double cos(double x);
double sin(double x);
float fn_800A1394(float x);
float fn_800A1418(float x);
float DistPointLineSq(float* p, float* a, float* b, float* t, float* u);
void fn_800A0328(void* m);
void fn_800A0168(void* out, void* m);
void fn_800A0030(float* out, void* m, float* v);
void* fn_8028B3BC(PtrArray* a, void* where, int n);
int fn_802A1D04(void);
int rand(void);
}

// Spawns a glass shatter: builds a pool of shard triangles fanned out around the impact point
// `hit` (rings of fracture lines), clips them to the pane rectangle `box` and registers the
// pool as an actor.
int ShotFire(Actor* actor, int texA, int texB, float fA, float fB, Vec3* box, Mtx34* mtx,
             Vec3* hit, Vec3* dir) {
    int rings = gShardRings;
    float radius = gShardRadius0;
    u32 attached = 0;
    int unit = 0;
    int spokes = gShardSpokes;
    float uvA, uvB, uvC, uvD;
    int tex;
    u32 flags;
    float planes[4][4];
    float n[3];
    float pt[3];
    float lo[3];
    float hi[3];
    float d[3];
    float p[3];
    char matBuf[0x30];
    int ipos[3];
    int count;
    char* pool;
    u32 handle;
    int segs = gShardSegs;
    int i;
    int j;
    float spanX;
    float spanY;
    float spanZ;

    tex = fn_8003CC90(texA, texB, &uvA, &uvB, &uvC, &uvD);
    flags = fn_80010000();
    if ((flags & 0x20000) != 0) {
        attached = 1;
        if (2.0 == (double)dir->x) {
            unit = 1;
            dir->x = 0.0f;
        }
    }
    n[0] = 0.0f; n[1] = 0.0f; n[2] = -1.0f;
    pt[0] = 0.0f; pt[1] = 0.0f; pt[2] = box->z;
    ViewPlaneFromNormal(planes[0], n, pt);
    n[0] = -1.0f; n[1] = 0.0f; n[2] = 0.0f;
    pt[0] = ((float*)box)[3]; pt[1] = 0.0f; pt[2] = 0.0f;
    ViewPlaneFromNormal(planes[1], n, pt);
    n[0] = 0.0f; n[1] = 0.0f; n[2] = 1.0f;
    pt[0] = 0.0f; pt[1] = 0.0f; pt[2] = ((float*)box)[2];
    ViewPlaneFromNormal(planes[2], n, pt);
    n[0] = 1.0f; n[1] = 0.0f; n[2] = 0.0f;
    pt[0] = ((float*)box)[0]; pt[1] = 0.0f; pt[2] = 0.0f;
    ViewPlaneFromNormal(planes[3], n, pt);
    ipos[0] = (int)mtx->m[9];
    ipos[1] = (int)mtx->m[10];
    ipos[2] = (int)mtx->m[11];
    fn_8016F120(gGlassFxId, ipos);

    lo[0] = ((float*)box)[0];
    lo[1] = ((float*)box)[1];
    lo[2] = ((float*)box)[2];
    hi[0] = ((float*)box)[3];
    hi[1] = ((float*)box)[4];
    hi[2] = ((float*)box)[5];
    if (hi[0] <= lo[0]) hi[0] = 1.0f + lo[0];
    if (hi[1] <= lo[1]) hi[1] = 1.0f + lo[1];
    if (hi[2] <= lo[2]) hi[2] = 1.0f + lo[2];

    if (mtx == NULL) {
        fn_800A0328(matBuf);
    } else {
        fn_800A0168(matBuf, mtx);
    }
    spanX = hi[0] - lo[0];
    spanY = hi[1] - lo[1];
    spanZ = hi[2] - lo[2];

    // impact point and direction in the pane's local space
    p[0] = hit->x - mtx->m[9];
    p[1] = hit->y - mtx->m[10];
    p[2] = hit->z - mtx->m[11];
    fn_800A0030(p, mtx, p);
    d[0] = dir->x;
    d[1] = dir->y;
    d[2] = dir->z;
    fn_800A0030(d, mtx, d);
    p[0] += d[0] * 500.0f;
    p[2] += d[2] * 500.0f;

    count = rings * segs + (spokes - 1) * segs * 2;
    pool = (char*)VAlloc(count * 0xEC + 0x78);
    if (pool == NULL) {
        return 0;
    }
    *(char**)fn_8028B3BC(gShardArray, gShardArray->data + gShardArray->bytes, 4) = pool;
    handle = (gShardInit & 0xFFFFFF) | 0xD000000;
    gShardInit++;
    if (fn_802A1D04() == 0) {
        *(int*)(pool + 0x74) = -1;
    } else {
        *(int*)(pool + 0x74) = fn_80121D60(14000.0f, 14000.0f, 100.0f, 0.6f, 0.75f, 0.4f, 500.0f, 0,
                                           actor->handle, p, 0xB062628C);
    }
    ActorFn8002378C(pool, handle, matBuf, (void*)ShotMsg, 0, 0, 0);
    PlacementToMatrix(pool, mtx);
    *(u32*)(pool + 0x4C) |= attached;
    *(u32*)(pool + 0x4C) &= ~0x20000;
    fn_80212814(handle, actor->handle, -1, 0, 0, 0);
    *(float*)(pool + 0x50) = mtx->m[9];
    *(float*)(pool + 0x54) = mtx->m[10];
    *(float*)(pool + 0x58) = mtx->m[11];
    *(int*)(pool + 0x68) = count;
    *(float*)(pool + 0x6C) = 3000.0f;
    *(float*)(pool + 0x70) = -3.4028235e38f;

    // rings of fracture lines around the impact point, each ring `segs` points
    {
        float ring[2][7][3];
        int cur = 0;
        char* sh = pool + 0x78;
        for (i = 0; i < spokes; i++) {
            float ang0 = gShardJitter * ((float)rand() / 32767.0f) - 0.5f * gShardJitter;
            float step = 360.0f / (float)segs;
            for (j = 0; j < segs; j++) {
                float a = ang0 + step * j;
                ring[cur][j][0] = radius * (float)fn_800A1394(a) + p[0];
                ring[cur][j][1] = 0.0f;
                ring[cur][j][2] = radius * (float)fn_800A1418(a) + p[2];
            }
            // shard triangles between this ring and the previous one (or the hit point)
            for (j = 0; j < segs; j++) {
                int k = (j + 1 < segs) ? j + 1 : 0;
                int shardIdx;
                for (shardIdx = 0; shardIdx < ((i == 0) ? rings : 2); shardIdx++) {
                    float* v = (float*)sh;
                    v[0xC] = p[0];
                    v[0xD] = p[1];
                    v[0xE] = p[2];
                    v[0xF] = ring[cur][j][0];
                    v[0x10] = ring[cur][j][1];
                    v[0x11] = ring[cur][j][2];
                    v[0x12] = ring[cur][k][0];
                    v[0x13] = ring[cur][k][1];
                    v[0x14] = ring[cur][k][2];
                    {
                        float r1 = (float)rand() / 32767.0f;
                        float r2 = (float)rand() / 32767.0f;
                        v[0x38] = (r1 * 2.0f - 1.0f) * 0.2f;
                        v[0x39] = (unit != 0) ? r2 * 8.0f * 0.2f : (r2 * 2.0f - 1.0f) * 0.2f;
                        v[0x3A] = (r2 * 2.0f - 1.0f) * 0.2f;
                    }
                    sh[0xD4] = (char)0x90;
                    sh[0xD5] = 3;
                    sh[0xD6] = (char)i;
                    sh[0xD7] = (char)((i == 0) ? 0 : 1);
                    v[0x33] = 0.0f;
                    v[0x34] = 3000.0f;
                    {
                        int w;
                        for (w = 0; w < 12; w++) {
                            v[w] = mtx->m[w];
                        }
                    }
                    sh += 0xEC;
                }
            }
            radius *= gShardRadiusGrow;
            cur ^= 1;
        }
    }

    // fit every shard to the pane: clip it and compute its texture coordinates
    {
        char* sh = pool + 0x78;
        float du = uvC - uvA;
        float dv = uvD - uvB;
        u32 maxRing = 0;
        for (i = 0; i < count; i++) {
            *(int*)(sh + 0xDC) = tex;
            if (ShardClip(sh, &lo[0]) != 0) {
                *(float*)(sh + 0xCC) = -(400.0f * ((float)rand() / 32767.0f) - *(float*)(sh + 0xCC));
            }
            if (sh[0xD5] != 0 && maxRing < (u8)sh[0xD6]) {
                maxRing = (u8)sh[0xD6];
            }
            for (j = 0; j < (u8)sh[0xD5]; j++) {
                float* v = (float*)(sh + 0x30 + j * 0xC);
                *(float*)(sh + 0x78 + j * 8) = ((v[0] - lo[0]) / spanX) * du + uvA;
                *(float*)(sh + 0x7C + j * 8) = -(((v[2] - lo[2]) / spanZ) * dv - uvD);
            }
            sh += 0xEC;
        }
        (void)spanY;
        (void)maxRing;
    }
    (void)unit;
    (void)gShardEdge;
    return 1;
}

// One vertex of a clip polygon.
struct ClipVert {
    float x, y, z;
};

extern float gClipPlanes[4][4];     // 0x8049F410: the four clip planes (normal, distance)
extern "C" void DebugPrintf(const char* fmt, ...);      // 0x8000781C

// Outcode of vertex `v` against the rectangle {minX, minZ, ?, maxX, ?, maxZ}.
static inline u32 ClipOutcode(ClipVert* v, float* rect) {
    u32 code = 0;
    if (rect[0] <= v->x) {
        if (rect[3] < v->x) {
            code |= 2;
        }
    } else {
        code |= 8;
    }
    if (rect[2] <= v->z) {
        if (rect[5] < v->z) {
            code |= 1;
        }
    } else {
        code |= 4;
    }
    return code;
}

// Clips the shard's triangle against the screen rectangle: returns 1 when the clipped polygon
// replaced it (type 0xA0, up to 6 vertices), 0 when it needs no clipping or is outside.
int ShardClip(char* shard, float* rect) {
    u32 any = 0;
    u32 all = 15;
    int cur = 0;
    ClipVert buf[2][7];
    u32 codes[6];
    int n = (u8)shard[0xD5];
    int i;
    if (n == 0 || n > 6) {
        DebugPrintf("ACK!!! vert ct : %d \n", n);
    }
    if (n != 3) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        buf[0][i + 1] = *(ClipVert*)(shard + 0x30 + i * 0xC);
    }
    buf[0][0] = *(ClipVert*)(shard + 0x30 + (n - 1) * 0xC);
    for (i = 0; i < n; i++) {
        codes[i] = ClipOutcode(&buf[0][i], rect);
        any |= codes[i];
        all &= codes[i];
    }
    if (any == 0) {
        return 0;
    }
    if (all != 0) {
        shard[0xD5] = 0;
        return 0;
    }
    {
        int plane;
        int m = n;
        for (plane = 0; plane < 4; plane++) {
            ClipVert* in = buf[cur];
            ClipVert* out = buf[cur ^ 1];
            u32 bit = 1 << plane;
            int outCount = 0;
            for (i = 0; i < m; i++) {
                codes[i] = ClipOutcode(&in[i], rect);
            }
            for (i = 0; i < m; i++) {
                int next = i + 1;
                if (next == m) {
                    next = 0;
                }
                {
                    u32 a = bit & codes[i];
                    u32 b = bit & codes[next];
                    if (a == 0 && b == 0) {
                        out[++outCount] = in[next];
                    } else if (a == 0 || b == 0) {
                        float* pl = gClipPlanes[plane];
                        float d0 = pl[2] * in[i].z + pl[0] * in[i].x + pl[1] * in[i].y;
                        float d1 = pl[2] * in[next].z + pl[0] * in[next].x + pl[1] * in[next].y;
                        float t = 0.0f;
                        ClipVert p;
                        if (d0 != d1) {
                            t = (-pl[3] - d0) / (d1 - d0);
                        }
                        p.x = in[i].x + (in[next].x - in[i].x) * t;
                        p.y = in[i].y + (in[next].y - in[i].y) * t;
                        p.z = in[i].z + (in[next].z - in[i].z) * t;
                        if (a == 0 && b != 0) {
                            out[++outCount] = p;
                        } else {
                            out[++outCount] = p;
                            out[++outCount] = in[next];
                        }
                        if (outCount > 6) {
                            DebugPrintf("NOT ENOUGH ROOM! : %d \n", outCount);
                            shard[0xD5] = 0;
                            return 0;
                        }
                    }
                }
            }
            out[0] = out[outCount];
            cur ^= 1;
            m = outCount;
        }
        for (i = 0; i < m; i++) {
            *(ClipVert*)(shard + 0x30 + i * 0xC) = buf[cur][i + 1];
        }
        shard[0xD5] = (char)m;
        shard[0xD4] = (char)0xA0;
        return 1;
    }
}

// Deletes every shard and then the shard array.
// ShardArrayShutdown
void fn_80233848(void) {
    while (gShardArray->bytes >> 2 != 0) {
        VFree(*(void**)gShardArray->data);
        fn_8028B35C(gShardArray, gShardArray->data, 4);
    }
    delete gShardArray;
}

struct ShardPool {
    u8 pad00[0x4C];
    u32 flags;              // 0x4C
    u8 pad50[0x68 - 0x50];
    int count;              // 0x68
    float life;             // 0x6C
    float step;             // 0x70
    int unk74;              // 0x74
    char shards[1];         // 0x78
};

int ShardUpdate(float dt, float floorZ, void* shards, int count, int n, void* owner, u32 flags,
                u32* hit);
void ShardRender(void* shards, int stride, int count, Mtx34* m, int r, int g, int b, int a, u32 flags);
void ShardDrawFn(void* pool);
int ShardMsg(u32 id, int msg, int* m);

// Draws the live glass pools: those not yet shattered are deferred, the rest drawn at once.
// ShardPoolsDraw
void fn_80233928(void) {
    void** it;
    float zero = gShardZero;
    for (it = (void**)gShardArray->data; it != (void**)(gShardArray->data + gShardArray->bytes);
         it++) {
        ShardPool* p = (ShardPool*)*it;
        if (zero < p->life && (gGfxDisabled == 0 || (p->flags & 1) == 0)) {
            if ((p->flags & 1) == 0) {
                fn_800F1138(1, (char*)p + 0x50, (void*)ShardDrawFn, p);
            } else if (gGfxDisabled == 0) {
                Mtx34 m;
                ActorGetMatrix(&m, p);
                ShardRender(p->shards, 0xEC, p->count, &m, 0xFF, 0xFF, 0xFF, 0xFF, p->flags);
            }
        }
    }
}

// Ages every glass pool by dt and frees the ones that ran out or finished.
// ShardPoolsUpdate
void fn_80233A20(float dt) {
    void** it = (void**)gShardArray->data;
    float scaled = dt * fn_802A2080();
    float zero = gShardZero;
    while (it != (void**)(gShardArray->data + gShardArray->bytes)) {
        ShardPool* p = (ShardPool*)*it;
        float step = scaled;
        if ((p->flags & 0x20000) != 0) {
            step = dt;
        }
        p->life = p->life - step;
        if (p->life <= zero ||
            ShardUpdate(step, p->step, p->shards, 0xEC, p->count, p, p->flags,
                        (u32*)((char*)p + 0x5C)) == 0) {
            if (((ShardPool*)*it)->unk74 != -1) {
                fn_80122370();
            }
            fn_8021284C(*(int*)((char*)p + 0x38));
            VFree(*it);
            fn_8028B35C(gShardArray, it, 4);
        } else {
            it++;
        }
    }
}

// Loads the shatter texture and effect.
// GlassInit
void fn_80233B60(void) {
    u32 h = StrHash("P:\\Fear\\Data\\Effects\\GlassShatter.tga");
    gGlassTexId = (short)AssetFind(0xF2E2BEB8, h);
    gGlassFxId = fn_80170834("glassshatter");
    fn_8000F874((short)gGlassTexId);
    gShardInit = 0;
}

// Creates the shard array and registers its message handler.
// ShardArrayInit
void fn_80233C30(void) {
    PtrArray* a = (PtrArray*)VAlloc(0x14);
    if (a != NULL) {
        fn_8028BA9C(a);
    }
    gShardArray = a;
    fn_80026660(0xD, (void*)ShardMsg, 0);
}

// Message handler registered for type 0xD: finds the pool whose handle matches `id` and
// answers like ShotMsg.
int ShardMsg(u32 id, int msg, int* m) {
    void** it = (void**)gShardArray->data;
    void** end = (void**)(gShardArray->data + gShardArray->bytes);
    int* shot;
    float one;
    int found = 0;
    while (it != end) {
        found = (int)*it;
        if ((*(u32*)(found + 0x38) & 0xFFFFFF) == id) {
            break;
        }
        it++;
        found = 0;
    }
    if (found == 0) {
        return 0;
    }
    m[0] = found;
    one = gShardOne;
    shot = (int*)m[0];
    if (shot == NULL) {
        return 0;
    }
    if (msg < 0x411) {
        if (msg == 0x400) {
            m[1] = shot[0xE];
            m[3] = 1;
            ((float*)m)[4] = one;
            ((float*)m)[5] = one;
            m[6] = 0;
            m[7] = 0;
            return 1;
        }
    } else {
        if (msg == 0x413) {
            int* src = (int*)m[1];
            int i;
            if (shot == NULL) {
                return 0;
            }
            shot[0x17] = src[9] - shot[9];
            shot[0x18] = src[10] - shot[10];
            shot[0x19] = src[11] - shot[11];
            for (i = 0; i < 12; i++) {
                shot[i] = src[i];
            }
            return 1;
        }
        if (msg < 0x413) {
            int i;
            if (shot == NULL) {
                return 0;
            }
            for (i = 0; i < 12; i++) {
                m[4 + i] = shot[i];
            }
            return 1;
        }
    }
    return 0;
}

extern float gShardGravity[3];      // 0x803BDA28
extern float gShardSpread;          // 0x804D34CC
extern "C" {
void C_QUATSlerp(float* a, float* b, float t, float* out);          // 0x8030D4D0
void PSQUATMultiply(float* a, float* b, float* out);                // 0x8030D420
void fn_800A0030(float* out, void* m, float* v);
void fn_800A025C(float* out, void* m, int col);
void fn_800A0AC4(float* a, void* m);
void fn_800A039C(float f, float* out, float* axis);
void fn_800A0168(void* out, void* m);
void fn_800A0EF0(void* m, float* q);
int rand(void);
}

// Advances `count` shards (stride `stride`) by dt: gravity, spin, bounce on the floor `floorZ`
// and merge-back when the pool is `flags & 1` (attached).  Returns the number still alive.
int ShardUpdate(float dt, float floorZ, void* shards, int stride, int count, void* owner,
                u32 flags, u32* hit) {
    float zero = gShardZero;
    char* sh = (char*)shards;
    int alive = 0;
    int k;
    if (shards == NULL) {
        return 0;
    }
    {
        float damp = -(0.35f * ((float)rand() / 32767.0f));
        float m[12];
        ActorGetMatrix(m, owner);
        for (k = 0; k < count; k++) {
            float* f = (float*)sh;
            if (zero < f[0x34]) {
                f[0x33] += dt;
                if (zero < f[0x33] && (sh[0xD7] & 8) == 0) {
                    if ((flags & 1) == 0) {
                        if ((sh[0xD7] & 1) != 0 && zero <= f[0x33]) {
                            int i;
                            int best = 0;
                            float low;
                            float off[3];
                            float add[12];
                            f[0x38] += (float)(int)hit[0] * gShardSpread;
                            f[0x39] += (float)(int)hit[1] * gShardSpread;
                            f[0x3A] += (float)(int)hit[2] * gShardSpread;
                            for (i = 0; i < 12; i++) {
                                f[i] = m[i];
                            }
                            sh[0xD7] &= ~1;
                            low = f[0xE];
                            for (i = 1; i < (u8)sh[0xD5]; i++) {
                                if (f[0xE + i * 3] < low) {
                                    best = i;
                                    low = f[0xE + i * 3];
                                }
                            }
                            off[0] = f[0xC + best * 3];
                            off[1] = f[0xD + best * 3];
                            off[2] = f[0xE + best * 3];
                            for (i = 0; i < (u8)sh[0xD5]; i++) {
                                f[0xC + i * 3] -= off[0];
                                f[0xD + i * 3] -= off[1];
                                f[0xE + i * 3] -= off[2];
                            }
                            fn_800A0168(add, f);
                            fn_800A0030(off, add, off);
                            f[9] += off[0];
                            f[10] += off[1];
                            f[11] += off[2];
                        }
                        if ((sh[0xD7] & 2) != 0) {
                            float q[4];
                            C_QUATSlerp(&f[0x2A], &f[0x2E], f[0x32], q);
                            f[0x32] += dt / 1000.0f;
                            fn_800A0EF0(f, q);
                            if (gShardOne <= f[0x32]) {
                                sh[0xD7] &= ~2;
                                sh[0xD7] |= 4;
                            }
                        }
                    }
                    f[0x34] -= dt;
                    f[0x38] += gShardGravity[0] * dt;
                    f[0x39] += gShardGravity[1] * dt;
                    f[0x3A] += gShardGravity[2] * dt;
                    if ((flags & 1) == 0) {
                        f[9] += f[0x38] * dt;
                        f[10] += f[0x39] * dt;
                        f[11] += f[0x3A] * dt;
                        if (f[11] < floorZ) {
                            f[11] = floorZ;
                            if ((sh[0xD7] & 6) == 0) {
                                float rel[3];
                                float axis[3];
                                float q[4];
                                float side;
                                sh[0xD7] |= 2;
                                rel[0] = f[9] - m[9];
                                rel[1] = f[10] - m[10];
                                rel[2] = f[11] - m[11];
                                fn_800A0030(axis, f, rel);
                                side = (axis[1] <= zero) ? -1.0f : 1.0f;
                                f[0x32] = zero;
                                fn_800A025C(axis, f, 0);
                                fn_800A0AC4(&f[0x2A], f);
                                fn_800A039C(zero, q, axis);
                                fn_800A039C(90.0f * side, q, axis);
                                PSQUATMultiply(q, &f[0x2A], &f[0x2E]);
                            }
                            f[0x38] *= 0.85f;
                            f[0x39] *= 0.85f;
                            f[0x3A] *= damp;
                        }
                    } else {
                        int i;
                        float* v = f;
                        for (i = 0; i < (u8)sh[0xD5]; i++) {
                            v[0xC] += f[0x38] * dt;
                            v[0xD] += f[0x39] * dt;
                            v[0xE] += f[0x3A] * dt;
                            v += 3;
                        }
                    }
                }
                alive++;
            }
            sh += stride;
        }
    }
    return alive;
}

// Deferred draw callback for a pool.
void ShardDrawFn(void* pool) {
    ShardPool* p = (ShardPool*)pool;
    Mtx34 m;
    ActorGetMatrix(&m, p);
    ShardRender(p->shards, 0xEC, p->count, &m, 0xFF, 0xFF, 0xFF, 0xFF, p->flags);
}

// Draws `count` shards (stride `stride`) as triangle fans with the given matrix.
void ShardRender(void* shards, int stride, int count, Mtx34* m, int r, int g, int b, int a,
                 u32 flags) {
    float zero = gShardZero;
    char* s = (char*)shards;
    int i;
    if (shards != NULL) {
        fn_801123E4(0, 0);
        for (i = 0; i < count; i++) {
            Mtx34* use = (Mtx34*)s;
            if ((s[0xD7] & 1) != 0 && (flags & 1) == 0) {
                use = m;
            }
            if (zero < *(float*)(s + 0xD0) && (u8)s[0xD5] > 2 &&
                ((u8)s[0xD4] == 0x90 || (u8)s[0xD4] == 0xA0)) {
                int k;
                fn_8013A30C(*(int*)((char*)shards + 0xDC), use);
                GXBegin((u8)s[0xD4], 2, (u8)s[0xD5]);
                for (k = 0; k < (int)(u8)s[0xD5]; k++) {
                    *(volatile float*)0xCC008000 = zero;
                }
                fn_8013A2E8();
            }
            s += stride;
        }
    }
}

// Returns the pool handle's low 24 bits as a float id.
int ShardGetId(ShardPool* p, float* out) {
    *out = (float)*(int*)((char*)p + 0x38);
    return 1;
}

extern int gAsyncThread;        // 0x804D5450
extern int gAsyncActive;        // 0x804D545C
extern int gAsyncDone;          // 0x804D5460
extern int gAsyncRequest;       // 0x804D5464
extern int gAsyncBusy;          // 0x804D5468
extern float gAsyncTimeout;     // 0x804D98E0
extern char gAsyncThreadObj[];  // 0x804A0180
extern "C" {
int OSGetTick(void);
void GXSetCurrentGXThread(void);
void OSYieldThread(void);
void OSResumeThread(void* t);
}

// Waits for the async draw thread to finish; returns the elapsed ticks (1 on timeout).
int AsyncDrawWait(void) {
    int start;
    int r;
    if (gAsyncThread == 0 || gAsyncActive == 0 || gGfxDisabled != 0) {
        r = 0;
    } else {
        float timeout = gAsyncTimeout;
        start = OSGetTick();
        while (gAsyncDone == 0) {
            int now = OSGetTick();
            if (timeout < (float)((u32)(now - start) / (*(u32*)0x800000F8 / 4000))) {
                GXSetCurrentGXThread();
                r = 1;
                gAsyncActive = 0;
                return r;
            }
            OSYieldThread();
        }
        GXSetCurrentGXThread();
        gAsyncActive = 0;
        r = OSGetTick() - start;
    }
    return r;
}

// Kicks the async draw thread.
void AsyncDrawKick(void) {
    if (gAsyncThread != 0 && gAsyncActive != 0 && gGfxDisabled == 0) {
        if (gAsyncBusy != 0) {
            OSResumeThread(gAsyncThreadObj);
        } else {
            gAsyncRequest = 1;
        }
    }
}

struct GfxSnapshot {
    float view[12];         // 0x804A0060
    float proj[2];          // 0x804A0090: fov scales (filled from +0x34 / +0x40 of the camera)
    float aspect;           // 0x804A0094
};

struct DrawJob {
    int valid;              // 0x00
    int data[11];           // 0x04
};

extern DrawJob gDrawJobs[0x40];     // 0x8049F460
extern u32 gAsyncSnapView[12];      // 0x804A0060
extern float gAsyncFovA;            // 0x804A0090
extern float gAsyncFovB;            // 0x804A0094
extern u8 gAsyncProj[0x1C];         // 0x804A009C
extern u8 gAsyncView[0x18];         // 0x804A00B8
extern u8 gAsyncMisc[0x18];         // 0x804A00D0
extern void* gAsyncFifo;            // 0x804D5458
extern "C" {
u32* fn_800C5DD8(void);
void fn_80008D7C(void);
void GXFlush(void);
void* GXGetGPFifo(void);
void GXGetFifoPtrs(void* fifo, void** read, void** write);
void fn_80009E44(int a, int b);
void fn_80009398(float f, int n);
void fn_800098B8(void* m);
void fn_800092CC(void);
void GXGetProjectionv(void* p);
void GXGetViewportv(void* v);
void fn_80008C78(void* p);
void fn_80235114(DrawJob* j);
void GXSetBreakPtCallback(void* cb);
void GXEnableBreakPt(void* p);
void fn_80234F88(void);
}

// Starts an async draw pass: snapshots the camera and replays the queued draw jobs.
void fn_80234900(void) {
    DrawJob* j = gDrawJobs;
    int i;
    void* rd;
    void* wr;
    if (gAsyncThread == 0 || gGfxDisabled != 0) {
        return;
    }
    gAsyncActive = 0;
    for (i = 0x40; i != 0; i--, j++) {
        if (j->valid != 0) {
            u32* cam;
            gAsyncDone = 0;
            gAsyncBusy = 0;
            gAsyncRequest = 0;
            gAsyncActive = 1;
            cam = fn_800C5DD8();
            for (i = 0; i < 12; i++) {
                gAsyncSnapView[i] = cam[i];
            }
            gAsyncFovB = ((float*)cam)[0x10];
            gAsyncFovA = ((float*)cam)[0xD];
            fn_80008D7C();
            GXFlush();
            gAsyncFifo = GXGetGPFifo();
            GXGetFifoPtrs(gAsyncFifo, &rd, &wr);
            if (rd == wr) {
                fn_80008D7C();
                fn_80009E44(0, 1);
                fn_80009398(gAsyncFovA, 2);
                fn_80009398(gAsyncFovB, 3);
                fn_800098B8(gAsyncSnapView);
                fn_800092CC();
                GXGetProjectionv(gAsyncProj);
                GXGetViewportv(gAsyncView);
                fn_80008C78(gAsyncMisc);
                for (i = 0; i < 0x40; i++) {
                    if (gDrawJobs[i].valid != 0) {
                        fn_80235114(&gDrawJobs[i]);
                    }
                }
                fn_80008D7C();
                gAsyncActive = 0;
                return;
            }
            GXSetBreakPtCallback((void*)fn_80234F88);
            GXEnableBreakPt(wr);
            return;
        }
    }
}

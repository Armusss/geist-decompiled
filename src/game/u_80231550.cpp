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

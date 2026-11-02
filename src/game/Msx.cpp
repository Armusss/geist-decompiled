// Msx.cpp: 0x8016D440-0x80175D60. Starts with Euler-angle / box math helpers (angles in degrees), followed by
// level load/unload and effect lookup (FxFind, LevelLoaded), the music/sound system glue (snd* calls), and
// debug text output.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/os.h>
#include <musyx/musyx.h>

typedef short s16;

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Box {
    Vec3 min, max;
};

struct IBox {
    IVec3 min, max;
};

struct Mtx34 {
    float rot[9];
    Vec3 pos;
};

struct Actor;

struct SBox {
    s16 v[6];
};

extern "C" {
float fn_800A1128(float y, float x);   // atan2 in degrees
void fn_800A0FC4(float* in, float* out);   // sin/cos of two angles
}

// Square root through the paired-single estimate instructions.
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

// EulerFromDirection: yaw (z) and pitch (x) angles of a direction vector, in degrees.
extern "C" void fn_8016D440(Vec3* out, Vec3* v) {
    float len;
    len = FastSqrt(v->x * v->x + v->y * v->y);
    out->z = fn_800A1128(-v->x, v->y);
    out->x = fn_800A1128(v->z, len);
    out->y = 0.0f;
}

// EulerToDirection: unit direction vector of a (z, x) angle pair.
extern "C" void fn_8016D4EC(Vec3* out, Vec3* v) {
    float a[4];

    float x = v->x;
    float z = v->z;
    a[1] = x;
    a[0] = z;
    fn_800A0FC4(a, a);
    out->x = -a[0] * a[3];
    out->y = a[2] * a[3];
    out->z = a[1];
}

// Wraps each angle into [-180, 180).
void EulerNormalize(Vec3* out, Vec3* in) {
    float a = in->z;
    if (a >= 180.0f) {
        out->z = a - 360.0f;
    } else if (a < -180.0f) {
        out->z = 360.0f + a;
    }
    a = in->x;
    if (a >= 180.0f) {
        out->x = a - 360.0f;
    } else if (a < -180.0f) {
        out->x = 360.0f + a;
    }
    a = in->y;
    if (a >= 180.0f) {
        out->y = a - 360.0f;
    } else if (a < -180.0f) {
        out->y = 360.0f + a;
    }
}

// Vec3Sub
extern "C" void fn_8016D614(Vec3* out, Vec3* a, Vec3* b) {
    float ay = a->y;
    float by = b->y;
    float az = a->z;
    float bz = b->z;
    out->x = a->x - b->x;
    out->y = ay - by;
    out->z = az - bz;
}

// Vec3Add
extern "C" void fn_8016D648(Vec3* out, Vec3* a, Vec3* b) {
    float ay = a->y;
    float by = b->y;
    float az = a->z;
    float bz = b->z;
    out->x = a->x + b->x;
    out->y = ay + by;
    out->z = az + bz;
}

// Stores its three arguments in z, x, y order.
void Vec3Set(Vec3* v, float a, float b, float c) {
    v->z = a;
    v->x = b;
    v->y = c;
}

// Vec3Copy
extern "C" void fn_8016D68C(Vec3* dst, Vec3* src) {
    float y = src->y;
    dst->x = src->x;
    float z = src->z;
    dst->y = y;
    dst->z = z;
}

// Vec3Zero
extern "C" void fn_8016D6A8(Vec3* v) {
    v->z = 0.0f;
    v->x = 0.0f;
    v->y = 0.0f;
}

// BoxToCenterExtents: center in out[0..2], half size in out[3..5].
extern "C" void fn_8016D6BC(Vec3* out, Box* b) {
    float h = 0.5f;
    float x1 = b->max.x;
    float x0 = b->min.x;
    float y1 = b->max.y;
    float y0 = b->min.y;
    float z1 = b->max.z;
    float z0 = b->min.z;
    float hy = h * (y1 - y0);
    out[1].x = h * (x1 - x0);
    out[1].y = hy;
    out[1].z = h * (z1 - z0);
    out[0].x = h * (x1 + x0);
    out[0].y = h * (y1 + y0);
    out[0].z = h * (z1 + z0);
}

// BoxToIntBox: truncates a float box to integers.
extern "C" void fn_8016D724(IBox* out, Box* b) {
    float y0 = b->min.y;
    float z0 = b->min.z;
    float x1 = b->max.x;
    float y1 = b->max.y;
    float z1 = b->max.z;
    out->min.x = (int)b->min.x;
    out->min.y = (int)y0;
    out->min.z = (int)z0;
    out->max.x = (int)x1;
    out->max.y = (int)y1;
    out->max.z = (int)z1;
}

// Despite the name in symbols.txt this converts a box of 16-bit integers to a float box.
void Vec3Copy(Vec3* out, Vec3* in) {
    SBox* s = (SBox*)in;
    Box* b = (Box*)out;
    b->min.x = s->v[0];
    b->min.y = s->v[1];
    b->min.z = s->v[2];
    b->max.x = s->v[3];
    b->max.y = s->v[4];
    b->max.z = s->v[5];
}

// IntBoxToBox: converts a box of 32-bit integers to a float box.
extern "C" void fn_8016D864(Box* out, IBox* in) {
    out->min.x = in->min.x;
    out->min.y = in->min.y;
    out->min.z = in->min.z;
    out->max.x = in->max.x;
    out->max.y = in->max.y;
    out->max.z = in->max.z;
}

// BoxCorner: corner i (0-7) of a box; returns 0 for an invalid index.
extern "C" int fn_8016D920(Vec3* out, Box* b, int i) {
    switch (i) {
    case 0:
        out->x = b->min.x;
        out->y = b->min.y;
        out->z = b->min.z;
        break;
    case 1:
        out->x = b->min.x;
        out->y = b->min.y;
        out->z = b->max.z;
        break;
    case 2:
        out->x = b->min.x;
        out->y = b->max.y;
        out->z = b->min.z;
        break;
    case 3:
        out->x = b->min.x;
        out->y = b->max.y;
        out->z = b->max.z;
        break;
    case 4:
        out->x = b->max.x;
        out->y = b->min.y;
        out->z = b->min.z;
        break;
    case 5:
        out->x = b->max.x;
        out->y = b->min.y;
        out->z = b->max.z;
        break;
    case 6:
        out->x = b->max.x;
        out->y = b->max.y;
        out->z = b->min.z;
        break;
    case 7:
        out->x = b->max.x;
        out->y = b->max.y;
        out->z = b->max.z;
        break;
    default:
        return 0;
    }
    return 1;
}

// BoxGrow: grows a box by a margin on each side.
extern "C" void fn_8016DD14(Box* b, Vec3* m) {
    float z;
    float y;
    float x;
    x = m->x;
    y = m->y;
    z = m->z;
    b->min.x -= x;
    b->min.y -= y;
    b->min.z -= z;
    b->max.x += x;
    b->max.y += y;
    b->max.z += z;
}

static inline void BoxAddPoint(Box* b, Vec3* p) {
    float v;
    v = p->x;
    b->min.x = (b->min.x < v) ? b->min.x : v;
    v = p->y;
    b->min.y = (b->min.y < v) ? b->min.y : v;
    v = p->z;
    b->min.z = (b->min.z < v) ? b->min.z : v;
    v = p->x;
    b->max.x = (b->max.x > v) ? b->max.x : v;
    v = p->y;
    b->max.y = (b->max.y > v) ? b->max.y : v;
    v = p->z;
    b->max.z = (b->max.z > v) ? b->max.z : v;
}

static inline void IBoxAddPoint(IBox* b, IVec3* p) {
    int v;
    v = p->x;
    b->min.x = (b->min.x < v) ? b->min.x : v;
    v = p->y;
    b->min.y = (b->min.y < v) ? b->min.y : v;
    v = p->z;
    b->min.z = (b->min.z < v) ? b->min.z : v;
    v = p->x;
    b->max.x = (b->max.x > v) ? b->max.x : v;
    v = p->y;
    b->max.y = (b->max.y > v) ? b->max.y : v;
    v = p->z;
    b->max.z = (b->max.z > v) ? b->max.z : v;
}

static inline void BoxCorners(Vec3* c, Box* b) {
    float z0 = b->min.z;
    float y0 = b->min.y;
    float x0 = b->min.x;
    float z1 = b->max.z;
    float y1 = b->max.y;
    float x1 = b->max.x;
    c[0].x = x0;
    c[0].y = y0;
    c[0].z = z0;
    c[1].x = x0;
    c[1].y = y0;
    c[1].z = z1;
    c[2].x = x0;
    c[2].y = y1;
    c[2].z = z0;
    c[3].x = x0;
    c[3].y = y1;
    c[3].z = z1;
    c[4].x = x1;
    c[4].y = y0;
    c[4].z = z0;
    c[5].x = x1;
    c[5].y = y0;
    c[5].z = z1;
    c[6].x = x1;
    c[6].y = y1;
    c[6].z = z0;
    c[7].x = x1;
    c[7].y = y1;
    c[7].z = z1;
}

static inline void IBoxCorners(IVec3* c, IBox* b) {
    int z0 = b->min.z;
    int y0 = b->min.y;
    int x0 = b->min.x;
    int z1 = b->max.z;
    int y1 = b->max.y;
    int x1 = b->max.x;
    c[0].x = x0;
    c[0].y = y0;
    c[0].z = z0;
    c[1].x = x0;
    c[1].y = y0;
    c[1].z = z1;
    c[2].x = x0;
    c[2].y = y1;
    c[2].z = z0;
    c[3].x = x0;
    c[3].y = y1;
    c[3].z = z1;
    c[4].x = x1;
    c[4].y = y0;
    c[4].z = z0;
    c[5].x = x1;
    c[5].y = y0;
    c[5].z = z1;
    c[6].x = x1;
    c[6].y = y1;
    c[6].z = z0;
    c[7].x = x1;
    c[7].y = y1;
    c[7].z = z1;
}

// BoxAddPoints: grows a box to include n points.
extern "C" void fn_8016DD6C(Box* b, Vec3* pts, int n) {
    int i;

    for (i = 0; i < n; i++) {
        BoxAddPoint(b, pts);
        pts++;
    }
}

extern "C" {
void fn_800A0100(void* mtx);               // selects the matrix used by fn_800A0094
void fn_800A0094(Vec3* out, Vec3* in);     // transforms a vector by the selected matrix
void fn_8009F5B0(void* mtx);               // integer variant of fn_800A0100
void fn_8009F50C(IVec3* out, IVec3* in);   // integer variant of fn_800A0094
}

// BoxTransformBounds: bounds of a box transformed by a matrix.
extern "C" void fn_8016DA30(Box* out, Box* in, void* mtx) {
    Vec3 c[8];
    Vec3* p;
    int i;

    BoxCorners(c, in);
    fn_800A0100(mtx);
    out->min.x = 3.4028235e38f;
    out->min.y = 3.4028235e38f;
    out->min.z = 3.4028235e38f;
    out->max.x = -3.4028235e38f;
    out->max.y = -3.4028235e38f;
    out->max.z = -3.4028235e38f;
    p = c;
    i = 0;
    do {
        fn_800A0094(p, p);
        BoxAddPoint(out, p);
        i++;
        p++;
    } while (i < 8);
}

// BoxExtendByBox: grows a box to include the corners of another box.
extern "C" void fn_8016DBD4(Box* out, Box* in) {
    Vec3 c[8];
    Vec3* p;
    int i;

    BoxCorners(c, in);
    p = c;
    i = 8;
    do {
        BoxAddPoint(out, p);
        p++;
        i--;
    } while (i != 0);
}

// IBoxTransformBounds: integer version of BoxTransformBounds.
extern "C" void fn_8016DE2C(IBox* out, IBox* in, void* mtx) {
    IVec3 c[8];
    IVec3* p;
    int i;

    IBoxCorners(c, in);
    fn_8009F5B0(mtx);
    p = c;
    i = 0;
    out->min.x = 0x7FFFFFFF;
    out->min.y = 0x7FFFFFFF;
    out->min.z = 0x7FFFFFFF;
    out->max.x = -0x7FFFFFFF;
    out->max.y = -0x7FFFFFFF;
    out->max.z = -0x7FFFFFFF;
    do {
        fn_8009F50C(p, p);
        IBoxAddPoint(out, p);
        i++;
        p++;
    } while (i < 8);
}

// IBoxExtendByBox
extern "C" void fn_8016DFBC(IBox* out, IBox* in) {
    IVec3 c[8];
    IVec3* p;
    int i;

    IBoxCorners(c, in);
    p = c;
    i = 8;
    do {
        IBoxAddPoint(out, p);
        p++;
        i--;
    } while (i != 0);
}

// IBoxGrow
extern "C" void fn_8016E0E4(IBox* b, IVec3* m) {
    int x = m->x;
    int y = m->y;
    int z = m->z;
    b->min.x -= x;
    b->min.y -= y;
    b->min.z -= z;
    b->max.x += x;
    b->max.y += y;
    b->max.z += z;
}

// IBoxAddPoints
extern "C" void fn_8016E13C(IBox* b, IVec3* pts, int n) {
    int i;

    for (i = 0; i < n; i++) {
        IBoxAddPoint(b, pts);
        pts++;
    }
}

// IBoxReset: an empty box.
extern "C" void fn_8016E1E4(IBox* b) {
    b->min.x = 0x7FFFFFFF;
    b->min.y = 0x7FFFFFFF;
    b->min.z = 0x7FFFFFFF;
    b->max.x = 0x80000001;
    b->max.y = 0x80000001;
    b->max.z = 0x80000001;
}

extern "C" {
void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);
}
void DebugPrintf(const char* fmt, ...);

// Streamed music playback: two mono ADPCM-decoded streams (left/right) fed from double buffers.
extern u32 lbl_804D4FB0;            // sample rate of the streams
extern SND_STREAMID lbl_804D5000;   // left stream
extern SND_STREAMID lbl_804D5004;   // right stream
extern void* lbl_804D5008;          // left buffer
extern void* lbl_804D500C;          // right buffer
extern u32 (*lbl_804D5010)(void* left, void* right, u32 n);   // decoder callback
extern u32 lbl_804D5014;
extern int lbl_804D5018;
extern int lbl_804D501C;
extern int lbl_804D5020;
extern int lbl_804D5024;
extern int lbl_804D5028;
extern int lbl_804D502C;
extern int lbl_804D5030;
extern int lbl_804D5034;
extern int lbl_804D5038;
extern int lbl_804D503C;
extern int lbl_804D2FC0;
extern int lbl_804D2FC4;
extern u32 lbl_804D2FCC;

// StreamSetPaused: a paused stream plays at sample rate 0.
extern "C" void fn_8016E20C(int playing) {
    u32 frq = 0;

    if (playing == 0) {
        frq = lbl_804D4FB0;
    }
    if (lbl_804D5008 != 0) {
        sndStreamFrq(lbl_804D5000, frq);
    }
    if (lbl_804D500C != 0) {
        sndStreamFrq(lbl_804D5004, frq);
    }
}

// StreamShutdown: frees both streams and their buffers.
extern "C" void fn_8016E270(void) {
    if (lbl_804D5000 != -1) {
        sndStreamFree(lbl_804D5000);
        lbl_804D5000 = -1;
    }
    if (lbl_804D5004 != -1) {
        sndStreamFree(lbl_804D5004);
        lbl_804D5004 = -1;
    }
    if (lbl_804D5008 != 0) {
        MemFree(lbl_804D5008, "Msx.cpp", 0x1119);
        lbl_804D5008 = 0;
    }
    if (lbl_804D500C != 0) {
        MemFree(lbl_804D500C, "Msx.cpp", 0x111F);
        lbl_804D500C = 0;
    }
}

static inline void StreamAllocBuffers(void) {
    u32 size = sndStreamAllocLength(0x1B90, 1);

    if (lbl_804D5008 == 0) {
        lbl_804D5008 = MemAlloc(size, "Msx.cpp", 0x10D5);
        if (lbl_804D5008 != 0) {
            lbl_804D500C = MemAlloc(size, "Msx.cpp", 0x10D8);
            if (lbl_804D500C == 0) {
                MemFree(lbl_804D5008, "Msx.cpp", 0x10DB);
                lbl_804D5008 = 0;
            }
        }
    }
    lbl_804D5000 = -1;
    lbl_804D5004 = -1;
}

u32 StreamUpdateLeft(void* b1, u32 n1, void* b2, u32 n2, u32 user);
u32 StreamUpdateRight(void* b1, u32 n1, void* b2, u32 n2, u32 user);

// StreamStart: allocates the buffers if needed and starts both streams. left/right: rate followed by ADPCM info.
extern "C" void fn_8016E314(int* left, int* right, u32 (*decode)(void*, void*, u32)) {
    sndStreamAllocLength(0x1B90, 1);
    lbl_804D5010 = decode;
    if (lbl_804D5008 == 0) {
        StreamAllocBuffers();
    }
    if (lbl_804D5008 != 0 && lbl_804D500C != 0) {
        lbl_804D4FB0 = *left;
        lbl_804D5000 = sndStreamAllocEx(0xFF, lbl_804D5008, 0x1B90, *left, lbl_804D2FCC, 0, 0, 0, 0, 0, 1,
                                        StreamUpdateLeft, 0, (SND_ADPCMSTREAM_INFO*)(left + 1));
        lbl_804D5004 = sndStreamAllocEx(0xFF, lbl_804D500C, 0x1B90, *right, lbl_804D2FCC, 0x7F, 0, 0, 0, 0, 1,
                                        StreamUpdateRight, 0, (SND_ADPCMSTREAM_INFO*)(right + 1));
    }
}

// StreamAlloc: allocates the stream buffers.
extern "C" void fn_8016E4C8(void) { StreamAllocBuffers(); }

// StreamUpdateRight: records the buffer halves for the right stream.
u32 StreamUpdateRight(void* b1, u32 n1, void* b2, u32 n2, u32 user) {
    u32 r;

    lbl_804D5030 = (int)b1;
    lbl_804D5034 = (int)b2;
    lbl_804D5038 = n1;
    lbl_804D503C = n2;
    r = lbl_804D5014;
    if (n1 + n2 < lbl_804D5014) {
        r = n1 + n2;
    }
    return r;
}

// StreamUpdateLeft: decodes both halves of the left stream's buffer through the decoder callback.
u32 StreamUpdateLeft(void* b1, u32 n1, void* b2, u32 n2, u32 user) {
    lbl_804D5028 = 0;
    lbl_804D502C = 0;
    lbl_804D5018 = (int)b1;
    lbl_804D501C = (int)b2;
    lbl_804D5020 = n1;
    lbl_804D5024 = n2;
    if (lbl_804D5010((char*)lbl_804D5008 + ((char*)b1 - (char*)lbl_804D5008),
                     (char*)lbl_804D500C + ((char*)b1 - (char*)lbl_804D5008), (n1 * 8) / 14) == 0) {
        lbl_804D5014 = 0;
        lbl_804D5028 = 1;
    } else {
        if (n2 != 0) {
            if (lbl_804D5010((char*)lbl_804D5008 + ((char*)b2 - (char*)lbl_804D5008),
                             (char*)lbl_804D500C + ((char*)b2 - (char*)lbl_804D5008), (n2 * 8) / 14) == 0) {
                lbl_804D502C = 1;
                n2 = 0;
                DebugPrintf("*StreamUpdate : split case failed\n");
            }
        }
        lbl_804D5014 = n1 + n2;
    }
    return lbl_804D5014;
}

// SoundSetOutputMode: 0 mono, 1 stereo, 2 surround (the OS stays in stereo for surround).
extern "C" void fn_8016E6D8(int mode) {
    lbl_804D2FC0 = mode;
    lbl_804D2FC4 = mode;
    switch (mode) {
    case 0:
        sndOutputMode(SND_OUTPUTMODE_MONO);
        OSSetSoundMode(0);
        break;
    case 1:
        sndOutputMode(SND_OUTPUTMODE_STEREO);
        OSSetSoundMode(1);
        break;
    case 2:
        sndOutputMode(SND_OUTPUTMODE_SURROUND);
        OSSetSoundMode(1);
        break;
    }
}

// SoundGetOutputMode
extern "C" int fn_8016E758(void) { return lbl_804D2FC0; }

// Empty in retail.
void LevelSetSky(char* name) {}

extern "C" void fn_8016E764(void) {}

void ActorGetMatrix(Mtx34* out, Actor* actor);

// A playing sound (0xA0 bytes); handles are (generation << 24) | slot index.
struct SoundSlot {
    int kind;         // 0x00: 0 free, 1 positional emitter
    int gen;          // 0x04
    int def;          // 0x08: index into the sound definitions (0x24 bytes each)
    int music;        // 0x0C
    u32 volume;       // 0x10
    int voice;        // 0x14
    SND_EMITTER emitter;   // 0x18
    u8 pad[0x88 - 0x18 - sizeof(SND_EMITTER)];
    Vec3 pos;         // 0x88
    Vec3 vel;         // 0x94: fed through fn_80175A90 (offset 0x98 as passed)
    u8 pad9c[0xA0 - 0xA0 + 0];
};

struct SoundDef {
    int volume;       // 0x00
    u8 pad4[0x14 - 4];
    int baseVolume;   // 0x14
    int emitterFlag;  // 0x18
    u8 pad1c[0x24 - 0x1C];
};

extern SoundSlot* lbl_804D4FBC;   // table of 256 slots
extern SoundDef* lbl_804D4F68;    // sound definitions
extern SND_LISTENER lbl_8046F798;
extern SND_FVECTOR lbl_803A810C;
extern SND_FVECTOR lbl_803A8118;
extern SND_FVECTOR lbl_803A8124;
extern SND_FVECTOR lbl_803A8130;
extern Mtx34 lbl_8046F828[4];
extern int lbl_804D4FB8;
extern int lbl_804D4F8C;

extern "C" void fn_80171F40(void* p);
extern "C" void fn_80175A90(Vec3* out, Vec3* a, Vec3* b);

static inline int SoundSlotIndex(int h) {
    int idx;

    if (h != -1) {
        idx = h & 0xFFFFFF;
        if ((int)((u32)h >> 24) == lbl_804D4FBC[idx].gen) {
        } else {
            idx = -1;
        }
    } else {
        idx = -1;
    }
    return idx;
}

// ListenerPush: remembers up to four camera matrices.
extern "C" int fn_8016ECB8(Actor* camera) {
    if (lbl_804D4FB8 < 4) {
        ActorGetMatrix(&lbl_8046F828[lbl_804D4FB8], camera);
        lbl_804D4FB8++;
    }
    return 1;
}

// ListenerSet: sets up the sound listener from the camera.
extern "C" int fn_8016ED08(Actor* camera) {
    fn_80171F40((char*)camera + 0x24);
    ActorGetMatrix(&lbl_8046F828[0], camera);
    lbl_804D4FB8 = 1;
    if (lbl_804D4F8C != 0) {
        sndUpdateListener(&lbl_8046F798, &lbl_803A810C, &lbl_803A8118, &lbl_803A8124, &lbl_803A8130, 0x7F, 0);
    } else {
        sndAddListener(&lbl_8046F798, &lbl_803A810C, &lbl_803A8118, &lbl_803A8124, &lbl_803A8130, 350.0f, 350.0f,
                       0.015f, 0, 0x7F, 0);
        lbl_804D4F8C = 1;
    }
    return 1;
}

extern "C" void fn_8016EDD4(void) {}

// SoundSetPosition: moves a playing sound; returns 1 if the handle was valid and the sound is not music.
extern "C" int fn_8016EDD8(int handle, Vec3* pos) {
    int idx = SoundSlotIndex(handle);

    if (idx > -1 && idx < 0x100) {
        SoundSlot* slot = &lbl_804D4FBC[idx];
        if (slot->kind != 0 && slot->music == 0) {
            Vec3 dir = {0.0f, 0.0f, 0.0f};
            Vec3 p;
            slot->pos = *pos;
            if (lbl_804D4FBC[idx].kind == 1) {
                fn_80175A90(&p, pos, &lbl_804D4FBC[idx].vel);
                sndUpdateEmitter(&lbl_804D4FBC[idx].emitter, (SND_FVECTOR*)&p, (SND_FVECTOR*)&dir,
                                 (u8)lbl_804D4FBC[idx].volume, 0);
            }
            return 1;
        }
    }
    return 0;
}

extern "C" int fn_8016F010(void) { return 0; }

// SoundSetVolume: volume is 12-bit fixed point (0x1000 = full).
extern "C" int fn_8016F018(int handle, int volume) {
    int idx = SoundSlotIndex(handle);

    if (idx > -1 && idx < 0x100 && lbl_804D4F68 != 0) {
        SoundSlot* slot = &lbl_804D4FBC[idx];
        SoundDef* def = &lbl_804D4F68[slot->def];
        if (slot->music == 0) {
            int v = def->baseVolume;
            if (v > 0x7F) {
                v = 0x7F;
            }
            slot->volume = (v * volume) / 0x1000;
        } else {
            int v = def->volume;
            if (v > 0x7F) {
                v = 0x7F;
            }
            slot->volume = (v * volume) / 0x1000;
            sndFXCtrl(lbl_804D4FBC[idx].voice, 7, (u8)lbl_804D4FBC[idx].volume);
        }
    }
    return 0;
}

extern "C" int fn_8016F118(void) { return 0; }

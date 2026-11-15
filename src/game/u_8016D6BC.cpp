// u_8016D6BC.cpp: box helpers (0x8016D6BC-0x8016E20C, .sdata2 0x804D8050-0x804D8068): center/extents,
// int/float/short box conversion, transforms and bounds. Split from Msx.cpp: it has its own copy of the
// int-to-float constant, 8-aligned after padding. Built with GC/2.0: retail loads through the const input
// pointers before the stack frame is set up (STATUS G4).
// Names are descriptive guesses; the code is matched against the retail DOL.

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

struct SBox {
    s16 v[6];
};

static inline void V3Set(Vec3* v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

// BoxToCenterExtents: center in out[0..2], half size in out[3..5].
extern "C" void fn_8016D6BC(Vec3* out, const Box* b) {
    float dx = (b->max.x - b->min.x) * 0.5f;
    float sx = (b->max.x + b->min.x) * 0.5f;
    float sy = (b->max.y + b->min.y) * 0.5f;
    float dy = (b->max.y - b->min.y) * 0.5f;
    float sz = (b->max.z + b->min.z) * 0.5f;
    float dz = (b->max.z - b->min.z) * 0.5f;
    out[1].x = dx;
    out[1].y = dy;
    out[1].z = dz;
    out[0].x = sx;
    out[0].y = sy;
    out[0].z = sz;
}

// BoxToIntBox: truncates a float box to integers.
extern "C" void fn_8016D724(IBox* out, const Box* b) {
    out->min.x = (int)b->min.x;
    out->min.y = (int)b->min.y;
    out->min.z = (int)b->min.z;
    out->max.x = (int)b->max.x;
    out->max.y = (int)b->max.y;
    out->max.z = (int)b->max.z;
}

// Despite the name in symbols.txt this converts a box of 16-bit integers to a float box.
void Vec3Copy(Vec3* out, const Vec3* in) {
    const SBox* s = (const SBox*)in;
    Box* b = (Box*)out;
    b->min.x = s->v[0];
    b->min.y = s->v[1];
    b->min.z = s->v[2];
    b->max.x = s->v[3];
    b->max.y = s->v[4];
    b->max.z = s->v[5];
}

// IntBoxToBox: converts a box of 32-bit integers to a float box.
extern "C" void fn_8016D864(Box* out, const IBox* in) {
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
        V3Set(out, b->min.x, b->min.y, b->min.z);
        break;
    case 1:
        V3Set(out, b->min.x, b->min.y, b->max.z);
        break;
    case 2:
        V3Set(out, b->min.x, b->max.y, b->min.z);
        break;
    case 3:
        V3Set(out, b->min.x, b->max.y, b->max.z);
        break;
    case 4:
        V3Set(out, b->max.x, b->min.y, b->min.z);
        break;
    case 5:
        V3Set(out, b->max.x, b->min.y, b->max.z);
        break;
    case 6:
        V3Set(out, b->max.x, b->max.y, b->min.z);
        break;
    case 7:
        V3Set(out, b->max.x, b->max.y, b->max.z);
        break;
    default:
        return 0;
    }
    return 1;
}

// BoxGrow: grows a box by a margin on each side.
extern "C" void fn_8016DD14(Box* b, const Vec3* m) {
    b->min.x -= m->x;
    b->min.y -= m->y;
    b->min.z -= m->z;
    b->max.x += m->x;
    b->max.y += m->y;
    b->max.z += m->z;
}

static inline float MinF(float a, float b) { return (a < b) ? a : b; }
static inline float MaxF(float a, float b) { return (a > b) ? a : b; }
static inline void BoxAddPoint(Box* b, const Vec3* p) {
    b->min.x = MinF(b->min.x, p->x);
    b->min.y = MinF(b->min.y, p->y);
    b->min.z = MinF(b->min.z, p->z);
    b->max.x = MaxF(b->max.x, p->x);
    b->max.y = MaxF(b->max.y, p->y);
    b->max.z = MaxF(b->max.z, p->z);
}

static inline int MinI(int a, int b) { return (a < b) ? a : b; }
static inline int MaxI(int a, int b) { return (a > b) ? a : b; }
static inline void IBoxAddPoint(IBox* b, const IVec3* p) {
    b->min.x = MinI(b->min.x, p->x);
    b->min.y = MinI(b->min.y, p->y);
    b->min.z = MinI(b->min.z, p->z);
    b->max.x = MaxI(b->max.x, p->x);
    b->max.y = MaxI(b->max.y, p->y);
    b->max.z = MaxI(b->max.z, p->z);
}

static inline void BoxCorners(Vec3* c, const Box* b) {
    float y1;
    float z1;
    float z0;
    float y0;
    float x0;
    float x1;
    z0 = b->min.z;
    y0 = b->min.y;
    x0 = b->min.x;
    z1 = b->max.z;
    y1 = b->max.y;
    x1 = b->max.x;
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

static inline void IBoxCorners(IVec3* c, const IBox* b) {
    int y1;
    int z1;
    int z0;
    int y0;
    int x0;
    int x1;
    z0 = b->min.z;
    y0 = b->min.y;
    x0 = b->min.x;
    z1 = b->max.z;
    y1 = b->max.y;
    x1 = b->max.x;
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
extern "C" void fn_8016DD6C(Box* b, const Vec3* pts, int n) {
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
extern "C" void fn_8016DA30(Box* out, const Box* in, void* mtx) {
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
extern "C" void fn_8016DBD4(Box* out, const Box* in) {
    Vec3 c[8];
    Vec3* p;
    int i;

    BoxCorners(c, in);
    for (i = 0; i < 8; i++) {
        BoxAddPoint(out, &c[i]);
    }
}

// IBoxTransformBounds: integer version of BoxTransformBounds.
extern "C" void fn_8016DE2C(IBox* out, const IBox* in, void* mtx) {
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
extern "C" void fn_8016DFBC(IBox* out, const IBox* in) {
    IVec3 c[8];
    IVec3* p;
    int i;

    IBoxCorners(c, in);
    for (i = 0; i < 8; i++) {
        IBoxAddPoint(out, &c[i]);
    }
}

// IBoxGrow
extern "C" void fn_8016E0E4(IBox* b, const IVec3* m) {
    b->min.x -= m->x;
    b->min.y -= m->y;
    b->min.z -= m->z;
    b->max.x += m->x;
    b->max.y += m->y;
    b->max.z += m->z;
}

// IBoxAddPoints
extern "C" void fn_8016E13C(IBox* b, const IVec3* pts, int n) {
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

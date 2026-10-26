// u_80100EB0.cpp: spline, item-colour and bone-rotation helpers (0x80100EB0-0x80104134).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct Quat {
    float x, y, z, w;
};

#define fabsf(x) __fabsf(x)

// Helpers from other units (still placeholder names in symbols.txt).
extern "C" float* fn_800FD3FC(int index);
extern "C" void fn_80037EC0(int handle, int asset, int hasColor, int r, int g, int b, int a);
extern "C" float fn_800A127C(float x);

int AssetFind(u32 group, u32 id);
void BoneGetMatrix(Mtx34* out, void* bone);
void MtxGetEuler(Vec3* out, Mtx34* mtx);
void MtxSetEuler(Mtx34* mtx, Vec3* euler);
void BoneSetMatrix(void* bone, Mtx34* mtx);
void EulerNormalize(Vec3* out, Vec3* in);

// Shared constants and tables owned by other units.
extern float lbl_804D7548;
extern float lbl_804D754C;
extern float lbl_804D7550;
extern float lbl_804D7554;
extern float lbl_804D7558;
extern float lbl_804D755C;
extern float lbl_804D7560;
extern float lbl_804D7564;
extern float lbl_804D7568;
extern float lbl_804D756C;
extern float lbl_804D7570;
extern float lbl_804D7580;
extern float lbl_804D7584;
extern float lbl_804D7588;
extern float lbl_804D758C;
extern float lbl_804D7590;
extern float lbl_8039EA48;
extern float lbl_8039EA54;
extern int lbl_8039EA60[5];
extern int lbl_804D2DA0;
extern int lbl_8039EB70[32];
extern int lbl_8039F6B0[25];
extern int lbl_804D4C08;

struct SplineState {
    int unk00;
    float unk04;
    float unk08;
    int count;
    int entries[16];
    int unk50;
};
extern SplineState lbl_8045E768;

extern float lbl_804D75B4;
extern float lbl_804D75B8;
extern double lbl_804D75C0;
extern "C" int BitsetFirstClear(u32* bits, int words, int n);
extern "C" void* memset(void* dst, int c, u32 n);

// Growable character buffer and string (same declarations as Credits.cpp; the code lives at
// 0x8028B9C0-0x8028C5xx and is shared).
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();
    String(const char* s);
    ~String() {}
    virtual void Release(char* p);
};

// Query payload: a hash, a spare word and a String whose first word the receiver overwrites
// with its answer.
struct SplineQueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    String result;
};

// Generic option message: hash plus a value word.
struct SplineOptMsg {
    int unk0;
    u32 hash;
    int unk8;
    int value;
};

extern int lbl_8039F6B0[25];       // spline configuration table
extern int lbl_804D4C0C;
extern int lbl_804D4C10;
extern int lbl_804D4C14;
extern int lbl_804D4C18;
extern int lbl_804D4C1C;
extern int lbl_803A59A8[8];
extern float lbl_804D7598;
extern float lbl_804D759C;
extern float lbl_804D75A0;
extern float lbl_804D75A4;
extern float lbl_804D75A8;
extern float lbl_804D75AC;
extern float lbl_804D75B0;
extern float lbl_804D75C8;
extern float lbl_804D75CC;

extern "C" void fn_8000519C(void* dst, void* src, int n);
extern "C" void fn_800F8ED0();
extern "C" void fn_800B969C();
extern "C" void fn_800FC764(int a, int b);
extern "C" int fn_800A3854();
extern "C" void fn_8028C500(void* str);
extern "C" void fn_8028B9C0(void* str, int a);
extern "C" u32 fn_80086364(u32 hash);
extern "C" void fn_80120C80(float a, int b, int c, int d);
extern "C" int fn_800FD720(int id);
extern "C" int fn_800FD68C(int player = 0);
extern "C" float fn_80120C14(int b, int c, int d);
extern "C" float* fn_800FD424(int a);
extern "C" void fn_8007E1E8(int a);
extern "C" void fn_8007E570(int a, int b, int c);
extern "C" void fn_8010FE64(int actor, int hash);
extern "C" void fn_8010FE38(int actor, int a);
extern "C" int fn_802A2024(u32 id);
extern "C" void Fn80140778();
extern "C" void Fn801408C8();
extern "C" void Fn801ED0C8();
extern "C" void Fn801ED2A0(int a);
extern "C" void Fn801ED1E4(int r, int g, int b, int a);
extern "C" void Fn801ED1A0(int a);
extern "C" void Fn801ED3FC(int a, int b, int c, int d);
extern "C" void Fn801EB188(const char* s, ...);
extern "C" int fn_80116DC4();
extern "C" void fn_8012E5A8(int a);
extern "C" void fn_8012071C(int a, int b, int* out, int c, int d);
extern "C" int fn_800FD334(int a);
extern "C" int fn_80170834(const char* s);
extern "C" void fn_8016F390(int a, int b, int c);
extern "C" int fn_8007EB34(int a, int b);
extern "C" void fn_8013E998(u32 hash, void* out);
extern "C" void fn_800B949C(u32 hash, void* out, int a, int b);
extern "C" int fn_800A3990();
extern "C" int fn_8007EA84(int a, int b);
extern "C" void fn_80120D28(float a, int b, int c, int d, int e, int f);
extern "C" void fn_80120AC0(float a, int b, int c, int d);
extern "C" void fn_802F09EC(void* out, int a, int b, int c, int d);

// Quaternion helpers from the SDK and neighbours.
extern "C" void PSQUATMultiply(const Quat* p, const Quat* q, Quat* out);
extern "C" void fn_800A039C(float angle, Quat* out, const float* axis);
extern "C" int fn_800C6448();
extern "C" void fn_8013AC68(int handle, int value);
extern "C" void fn_8013A400(int handle, int r, int g, int b, int a, float time, float scale);
extern "C" void fn_8013A834(int handle, int r, int g, int b, int a, float time, float scale);
extern "C" void fn_8013A3D4();
extern "C" u32 fn_800FD6EC(int index = 0);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void* memcpy(void* dst, const void* src, u32 n);
extern "C" int fn_80127274(int flag, void* msg);
extern "C" void fn_8013FCF0(int a0, int a1, int a2);
extern "C" void fn_800FEF2C(float a, float b, float c, float d, int handle, ...);
extern "C" void fn_801403D0(float time, int a, int b);
extern int lbl_804D2DA8;

// Evaluates a cubic Hermite segment at t: p0 and p1 are the end points, m0 and m1 their tangents.
// The three components are blended into out; always returns 1.
int SplineHermiteEval(float t, Vec3* p0, Vec3* m0, Vec3* p1, Vec3* m1, Vec3* out) {
    float h00 = lbl_804D7548 + (lbl_804D754C * t * t * t - lbl_804D7550 * t * t);
    float h10 = t + (t * t * t - lbl_804D754C * t * t);
    float h01 = lbl_804D7554 * t * t * t + lbl_804D7550 * t * t;
    float h11 = t * t * t - t * t;

    out->x = p1->x * h01 + p0->x * h00 + m0->x * h10 + m1->x * h11;
    out->y = p1->y * h01 + p0->y * h00 + m0->y * h10 + m1->y * h11;
    out->z = p1->z * h01 + p0->z * h00 + m0->z * h10 + m1->z * h11;
    return 1;
}

// Tints the models attached to two item handles with the RGBA colour at colorIndex (a negative
// index clears the tint). The first handle covers the five table entries in lbl_8039EA60, the
// second the extra asset id in lbl_804D2DA0.
void ItemColorApply(int handle1, int handle2, int colorIndex) {
    int hasColor;
    int r, g, b, a;
    int i;
    int asset;

    if (colorIndex < 0) {
        hasColor = 0;
        r = 0xff;
        g = r;
        b = r;
        a = r;
    } else {
        float* color = fn_800FD3FC(colorIndex);

        hasColor = 1;
        r = (int)(lbl_804D7558 * color[0]) & 0xff;
        g = (int)(lbl_804D7558 * color[1]) & 0xff;
        b = (int)(lbl_804D7558 * color[2]) & 0xff;
        a = (int)(lbl_804D7558 * color[3]) & 0xff;
    }

    if (handle1 != -1) {
        for (i = 0; i < 5; i++) {
            asset = AssetFind(0xd9044cbb, lbl_8039EA60[i]);
            if (asset != -1) {
                fn_80037EC0(handle1, asset, hasColor, r, g, b, a);
            }
        }
    }

    if (handle2 != -1) {
        asset = AssetFind(0xd9044cbb, lbl_804D2DA0);
        if (asset != -1) {
            fn_80037EC0(handle2, asset, hasColor, r, g, b, a);
        }
    }
}

// Rotates bone `index` of the skeleton at `bones` (0x1c bytes per bone): adds angle to its X euler
// angle, scales the roll by a factor derived from the angle and normalises the result.
void BoneAddRotX(void* bones, int index, float angle) {
    Vec3 euler;
    Mtx34 mtx;
    void* bone;
    float d;

    if (index < 0) {
        return;
    }
    bone = (char*)bones + index * 0x1c;
    BoneGetMatrix(&mtx, bone);
    MtxGetEuler(&euler, &mtx);
    euler.x += angle;
    d = lbl_804D755C * fabsf(angle);
    if (d >= lbl_804D7560) {
        d = lbl_804D7560;
    }
    d = fn_800A127C(d);
    d = lbl_804D7548 - d;
    if (d < lbl_804D7564) {
        d = lbl_804D7564;
    } else if (d > lbl_804D7568) {
        d = lbl_804D7568;
    }
    euler.z *= d;
    EulerNormalize(&euler, &euler);
    MtxSetEuler(&mtx, &euler);
    BoneSetMatrix(bone, &mtx);
}

// Applies a single rotation of `t` (clamped and divided by the span) to the quaternion keys
// start..end of the 7-float-stride array at quats. The keys outside the range are used to build
// the local rotations, so only the selected slice is rewritten. `type` selects the blend preset:
// a handful of hashes are rejected outright and type 0x1091c shifts the range by two keys.
void SplineQuatRotateRange(float t, float* quats, int start, int end, u32 type) {
    Quat conj;
    Quat axis;
    Quat cum[32];
    Quat neg[32];
    float a;
    float step;
    int i;

    if (type == 0xBEA8CA88u || type == 0xF63E7769u || type == 0x8FA80B9Eu ||
        type == 0x8FA05C1Au || type == 0x04624485u || type == 0x00013C05u ||
        type == 0xE0AB9B4Fu || type == 0x8C3E2D96u) {
        return;
    }
    if (start < 0 || start == 0xff) {
        return;
    }
    if (end < 0 || end == 0xff || end >= 0x20) {
        return;
    }

    a = lbl_804D756C;
    if (t >= a) {
        a = lbl_804D7570;
        if (t <= a) {
            a = t;
        }
    }
    step = a / (float)(end - start + 1);
    if (type == 0x1091c) {
        start += 2;
    }

    for (i = 0; i <= end; i++) {
        neg[i].x = -quats[i * 7 + 0];
        neg[i].y = -quats[i * 7 + 1];
        neg[i].z = -quats[i * 7 + 2];
        neg[i].w = quats[i * 7 + 3];
    }

    cum[0] = neg[0];
    for (i = 1; i <= end; i++) {
        PSQUATMultiply(&cum[i - 1], &neg[i], &cum[i]);
    }

    for (i = start; i <= end; i++) {
        fn_800A039C(step, &axis, &lbl_8039EA54);
        PSQUATMultiply(&axis, &cum[i], &cum[i]);
    }

    for (i = 1; i <= end; i++) {
        conj.x = -cum[i - 1].x;
        conj.y = -cum[i - 1].y;
        conj.z = -cum[i - 1].z;
        conj.w = cum[i - 1].w;
        PSQUATMultiply(&conj, &cum[i], &neg[i]);
    }

    for (i = start; i <= end; i++) {
        quats[i * 7 + 0] = -neg[i].x;
        quats[i * 7 + 1] = -neg[i].y;
        quats[i * 7 + 2] = -neg[i].z;
        quats[i * 7 + 3] = neg[i].w;
    }
}

// Same as SplineQuatRotateRange, but the rotation applied to each key grows linearly: key i gets
// step * (i + 1) instead of a flat step. Uses the other axis preset (lbl_8039EA48) and its own
// clamp range.
void SplineQuatRotateRamp(float t, float* quats, int start, int end, u32 type) {
    Quat conj;
    Quat axis;
    Quat cum[32];
    Quat neg[32];
    float a;
    float step;
    int i;

    if (type == 0xBEA8CA88u || type == 0xF63E7769u || type == 0x8FA80B9Eu ||
        type == 0x8FA05C1Au || type == 0x04624485u || type == 0x00013C05u ||
        type == 0xE0AB9B4Fu || type == 0x8C3E2D96u) {
        return;
    }
    if (start < 0 || start == 0xff) {
        return;
    }
    if (end < 0 || end == 0xff || end >= 0x20) {
        return;
    }

    a = lbl_804D7580;
    if (t >= a) {
        a = lbl_804D7584;
        if (t <= a) {
            a = t;
        }
    }
    step = a / (float)(end - start + 1);
    if (type == 0x1091c) {
        start += 2;
    }

    for (i = 0; i <= end; i++) {
        neg[i].x = -quats[i * 7 + 0];
        neg[i].y = -quats[i * 7 + 1];
        neg[i].z = -quats[i * 7 + 2];
        neg[i].w = quats[i * 7 + 3];
    }

    cum[0] = neg[0];
    for (i = 1; i <= end; i++) {
        PSQUATMultiply(&cum[i - 1], &neg[i], &cum[i]);
    }

    for (i = start; i <= end; i++) {
        fn_800A039C(step * (float)(i + 1), &axis, &lbl_8039EA48);
        PSQUATMultiply(&axis, &cum[i], &cum[i]);
    }

    for (i = 1; i <= end; i++) {
        conj.x = -cum[i - 1].x;
        conj.y = -cum[i - 1].y;
        conj.z = -cum[i - 1].z;
        conj.w = cum[i - 1].w;
        PSQUATMultiply(&conj, &cum[i], &neg[i]);
    }

    for (i = start; i <= end; i++) {
        quats[i * 7 + 0] = -neg[i].x;
        quats[i * 7 + 1] = -neg[i].y;
        quats[i * 7 + 2] = -neg[i].z;
        quats[i * 7 + 3] = neg[i].w;
    }
}

// Draws one colour swatch for the item-colour menu: binds the current render state to `value`,
// emits a textured quad tinted with the RGBA colour at color (scaled to bytes) and sized from the
// start/count range, then tears the state down again.
void ItemColorDrawQuad(int owner, int value, int start, int count, float* color) {
    int r, g, b, a;

    fn_8013AC68(owner, fn_800C6448());
    r = (int)(lbl_804D7558 * color[0]) & 0xff;
    g = (int)(lbl_804D7558 * color[1]) & 0xff;
    b = (int)(lbl_804D7558 * color[2]) & 0xff;
    a = (int)(lbl_804D7558 * color[3]) & 0xff;
    fn_8013A400(value, r, g, b, a, (float)(start + count) * lbl_804D7588, lbl_804D758C);
    fn_8013A3D4();
}

// Draws one item-colour swatch sized from an explicit start/count range rather than a table
// entry, then tears the draw state down again.
void ItemColorDrawQuadRange(int owner, int value, int start, int count, float* color) {
    int r, g, b, a;

    fn_8013AC68(owner, fn_800C6448());
    r = (int)(lbl_804D7558 * color[0]) & 0xff;
    g = (int)(lbl_804D7558 * color[1]) & 0xff;
    b = (int)(lbl_804D7558 * color[2]) & 0xff;
    a = (int)(lbl_804D7558 * color[3]) & 0xff;
    fn_8013A834(value, r, g, b, a, (float)(start + count) * lbl_804D7588, lbl_804D7590);
    fn_8013A3D4();
}

// Payload of the item-colour draw-state message 0x40d: a leading word the handler ignores, the
// menu mode and the number of entries to draw.
struct ItemColorDrawMsg {
    int unk00;
    int mode;
    int count;
};

// Puts the item-colour menu into mode 0 with two entries.
void ItemColorSendMode0(void) {
    ItemColorDrawMsg msg;

    msg.mode = 0;
    msg.count = 2;
    SendMsg(fn_800FD6EC(), 0x40d, &msg);
}

// Puts the item-colour menu into mode 1 with two entries.
void ItemColorSendMode1(void) {
    ItemColorDrawMsg msg;

    msg.mode = 1;
    msg.count = 2;
    SendMsg(fn_800FD6EC(), 0x40d, &msg);
}

// One 0x10-byte name entry: asset group, cached name hash and resolved asset id.
struct SplineNameEntry {
    char* name;
    u32 group;
    u32 hash;
    int asset;
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

void BitClear(u32* bits, int bit);

// Clears the cached name hash of `count` entries.
void SplineClearNameHashes(SplineNameEntry* entries, int count) {
    int i;

    for (i = 0; i < count; i++) {
        entries[i].hash = 0;
    }
}

// Resolves entry `index` of the name table: hashes its name (31x rolling hash over upper-cased
// characters, modulo 0xffffffff) and looks the hash up in the entry's asset group. The hash and
// the resolved asset id are cached in the entry. Returns -1 when the index is out of range.
int SplineAssetResolve(SplineNameEntry* entries, int count, int index) {
    SplineNameEntry* entry;
    char* p;
    u32 h;

    if (index < 0 || count <= index) {
        return -1;
    }
    entry = &entries[index];
    if (entry->hash == 0) {
        p = entry->name;
        if (p == 0) {
            h = 0;
        } else {
            h = 0;
            for (; *p != 0; p++) {
                h = h * 31 + (unsigned char)toupper(*p);
            }
            h = h % 0xFFFFFFFF;
        }
        entry->hash = h;
        entry->asset = AssetFind(entry->group, entry->hash);
    }
    return entry->asset;
}

// Clears the bit at (start - base) / step in the bitset.
void SplineBitClearSpan(int start, int base, int step, u32* bits) {
    BitClear(bits, (start - base) / step);
}

// Allocates a free bit of the bitset, marks it used and returns the matching entry of the array
// at base (each entry stride bytes apart). Returns null when the bitset is full.
void* SplineSlotAlloc(void* base, int stride, u32* bits, int count) {
    int slot;
    int words;

    words = count / 32 + (count % 32 != 0);
    slot = BitsetFirstClear(bits, words, count);
    if (slot == -1) {
        return 0;
    }
    bits[slot / 32] |= 1 << (slot & 31);
    return (char*)base + slot * stride;
}

// The spline race group handed to the per-frame entry points: participant count, the
// four-byte actor handles from +0x24 on, and a few flags further out.
struct SplineGroup {
    char pad00[0x1c];
    int count;
    int unk20;
    int ids[0x28];
    int unkC4;
    char padC8[0x40];
    int unk108;
    char pad10C[0x1c];
    int unk128;
};

// A command message: an unused first word followed by three payload words.
struct SplineCmdMsg {
    int unk0;
    int a;
    int b;
    int c;
};

extern int lbl_8045E7BC[8];
extern int lbl_8045E7DC[8];
extern char lbl_80345737[];
extern "C" int fn_8002394C();
extern "C" void fn_800FF4E4(int a);

void SplineSceneUnload(SplineGroup* g);
void SplineEntryUpdate(SplineGroup* g, int index);
int SplineEntryLink(int owner, int a, int b);
int SplineEntryAttach(int owner, int a, int b);

// Spline subsystem message handler.
int SplineMsg(int owner, int msg, void* p) {
    SplineCmdMsg* m = (SplineCmdMsg*)p;
    SplineQueryMsg q;
    int n;
    int a;
    int b;
    int i;

    if (msg == 0x420) {
        if (m->a != 0x381b348d) {
            return 0;
        }
        n = m->b;
        if (n > lbl_8045E768.count) {
            n = lbl_8045E768.count;
        }
        m->c = n;
        fn_8000519C((void*)m->c, lbl_8045E768.entries, n * 4);
        return 1;
    }
    if (msg < 0x420) {
        if (msg == 0x41e) {
            if (m->a == 0x56cf46f2) {
                m->c = lbl_804D2DA8;
                return 1;
            }
            if (m->a < 0x56cf46f2 && m->a == 0xc9991e05) {
                m->c = lbl_8045E768.unk50;
                return 1;
            }
            return fn_80127274(1, p);
        }
        if (msg > 0x41c && msg < 0x41e) {
            if (m->a != 0x56cf46f2) {
                return fn_80127274(0, p);
            }
            lbl_804D2DA8 = m->c;
            if (lbl_8045E768.unk50 != -1) {
                SplineCmdMsg out;
                out.a = 0;
                out.b = 0x4b000000;
                out.c = 0x56cf46f2;
                SendMsg(lbl_8045E768.unk50, 0xc14, &out);
            }
            return 1;
        }
        return 0;
    }
    if (msg != 0xc14) {
        return 0;
    }
    n = m->c;
    a = m->a;
    b = m->b;
    if (n == 0x53d74347) {
        int p1 = fn_800FD720(a);
        int p2 = fn_800FD720(b);
        int h = fn_800FD6EC(a);
        if (p2 == -1) {
            return 0;
        }
        if (p1 == -1) {
            fn_8013FCF0(owner, -1, p2);
        } else {
            u32 t = fn_80086364(0xceffe6b);
            fn_800FEF2C(lbl_804D7598, lbl_804D7598, lbl_804D7598, lbl_804D759C, p2, t);
        }
        fn_80120C80(lbl_804D75A0, 0xe2d6a9c0, 0, p2);
        fn_80120C80(lbl_804D75A0, 0xe2d6a9c0, 1, h);
        return 1;
    }
    if (n < 0x53d74347) {
        if (n != 0x901eefa2) {
            return 0;
        }
        if (a == 0x5e5bce3e) {
            lbl_8045E768.unk50 = b;
            return 1;
        }
        if (a < 0x5e5bce3e && a == 0x1af27550) {
            if (lbl_8045E768.count < 0x10) {
                lbl_8045E768.entries[lbl_8045E768.count] = b;
                lbl_8045E768.count = lbl_8045E768.count + 1;
            }
            return 1;
        }
        return 0;
    }
    if (n != 0x7c3f4e3e) {
        return 0;
    }
    if ((u32)a >> 24 != 1) {
        q.hash = 0xfbfd67c2;
        q.unk8 = 0;
        *(int*)&q.result = 0;
        SendMsg(0x4b000000, 0x41e, &q);
        if (*(int*)&q.result == 0) {
            return 0;
        }
        return SplineEntryLink(owner, m->a, m->b);
    }
    if (b == lbl_803A59A8[1] || b == lbl_803A59A8[3] || b == lbl_803A59A8[5] ||
        b == lbl_803A59A8[7]) {
        return 0;
    }
    for (i = 0; i < lbl_8045E768.count; i++) {
        if (m->a == lbl_8045E768.entries[i]) {
            return SplineEntryAttach(owner, m->a, m->b);
        }
    }
    return 0;
}

// Per-frame mode logic of the spline race.
void SplineModeUpdate(SplineGroup* g, int msg) {
    int i;
    int t;
    SplineQueryMsg q;

    if (lbl_804D4C08 == 0 || msg != -1) {
        return;
    }
    if (lbl_8045E768.unk00 == 2) {
        for (i = 0; i < g->count; i++) {
            if ((g->ids[i] >> 24) == 2 && *(int*)(fn_8002394C() + 0x358) == 1 &&
                g->ids[i] != -1) {
                SplineCmdMsg out;
                out.a = 1;
                out.b = 0x4b000000;
                out.c = 0xf8629c97;
                SendMsg(g->ids[i], 0xc14, &out);
            }
        }
        if (lbl_804D4C1C == 0) {
            if (lbl_804D4C14 == 1) {
                lbl_804D4C14 = 0;
                for (i = 0; i < g->count; i++) {
                    int target = fn_800FD6EC(i);
                    q.hash = 0xde3eaad7;
                    q.unk8 = 0;
                    *(int*)&q.result = 1;
                    SendMsg(target, 0x41d, &q);
                }
            }
            lbl_804D4C1C = 1;
        }
        if (lbl_8039F6B0[1] != 0) {
            fn_801403D0(lbl_8045E768.unk04, 0, 0);
        }
    } else if (lbl_8045E768.unk00 < 2) {
        if (lbl_8045E768.unk00 == 0) {
            if (lbl_804D4C10 == 0) {
                lbl_804D4C10 = 1;
                for (i = 0; i < g->count; i++) {
                    int target = fn_800FD6EC(i);
                    q.hash = 0xde3eaad7;
                    q.unk8 = 0;
                    *(int*)&q.result = 0;
                    SendMsg(target, 0x41d, &q);
                }
            }
            Fn801ED0C8();
            Fn801ED2A0(0x14);
            Fn801ED1E4(0xff, 0xff, 0xff, 0xff);
            Fn801ED1A0(2);
            Fn801ED3FC(0, 0x280, 0, 0x150);
            fn_80086364(0xbaac06c9);
            Fn801EB188(0);
        } else if (lbl_8045E768.unk00 >= 0) {
            if (lbl_804D4C18 == 0) {
                if (lbl_804D4C10 == 1) {
                    lbl_804D4C10 = 0;
                }
                lbl_804D4C18 = 1;
            }
            if (lbl_804D4C14 == 0) {
                lbl_804D4C14 = 1;
            }
            Fn801ED0C8();
            Fn801ED2A0(0x14);
            t = (int)(lbl_804D75A4 * (lbl_8045E768.unk08 / lbl_804D75A8));
            Fn801ED1E4(0xff, 0xff, 0xff, t & 0xff);
            Fn801ED1A0(2);
            Fn801ED3FC(0, 0x280, 0, 0x150);
            fn_80086364(0xbaac06c9);
            Fn801EB188(0);
        }
    } else if (lbl_8045E768.unk00 < 4) {
        if (lbl_804D4C0C == 1) {
            lbl_804D4C0C = 0;
        }
        lbl_804D4C18 = 0;
        lbl_804D4C1C = 0;
    }
    for (i = 0; i < g->count; i++) {
        if ((u32)fn_800FD6EC(i) >> 24 == 2) {
            SplineEntryUpdate(g, i);
        }
    }
}

// Advances the spline race timers; when the countdown expires it moves to the next mode.
void SplineTick(float time, SplineGroup* g) {
    int i;
    int done;

    if (lbl_804D4C08 == 0) {
        return;
    }
    if (lbl_8045E768.unk00 == 2) {
        done = 0;
        if (lbl_8039F6B0[1] != 0) {
            lbl_8045E768.unk04 = lbl_8045E768.unk04 - time;
            if (lbl_8045E768.unk04 <= lbl_804D75AC) {
                lbl_8045E768.unk04 = lbl_804D75AC;
                done = 1;
            }
        }
        if (lbl_8039F6B0[0] > 0) {
            for (i = 0; i < 8; i++) {
                if ((g->unk128 & (4 << i)) != 0) {
                    if (lbl_8039F6B0[0] <= (int)fn_80120C14(0xe2d6a9c0, 1, i)) {
                        done = 1;
                    }
                }
            }
        }
        if (done) {
            SplineQueryMsg q;
            SplineCmdMsg out;
            for (i = 0; i < g->count; i++) {
                ItemColorSendMode1();
                q.hash = 0x640d1f4d;
                q.unk8 = 0;
                *(int*)&q.result = 0;
                SendMsg(g->ids[i], 0x41d, &q);
            }
            fn_800FF4E4(0xffffffff);
            lbl_8045E768.unk00 = 3;
            lbl_8045E768.unk08 = lbl_804D75B0;
            out.a = 1;
            out.b = 0x4b000000;
            out.c = 0xf8629c97;
            SendMsg(0x20000000, 0xc14, &out);
        }
    } else {
        if (lbl_8045E768.unk00 < 2) {
            if (lbl_8045E768.unk00 == 0) {
                if (lbl_804D75AC < lbl_8045E768.unk08 - time) {
                    lbl_8045E768.unk08 = lbl_8045E768.unk08 - time;
                    return;
                }
                lbl_8045E768.unk00 = 1;
                lbl_8045E768.unk08 = lbl_804D75A8;
                return;
            }
            if (lbl_8045E768.unk00 >= 0) {
                if (lbl_804D75AC < lbl_8045E768.unk08 - time) {
                    lbl_8045E768.unk08 = lbl_8045E768.unk08 - time;
                    return;
                }
                lbl_8045E768.unk08 = lbl_804D75AC;
                lbl_8045E768.unk00 = 2;
                for (i = 0; i < g->count; i++) {
                    ItemColorSendMode0();
                }
                return;
            }
        } else if (lbl_8045E768.unk00 < 4) {
            SplineSceneUnload(g);
            return;
        }
        lbl_8045E768.unk00 = 2;
    }
}

// Frees the spline slot flags and resets the sub-state flags.
void SplineShutdown(void) {
    int i;

    if (lbl_804D4C08 == 0) {
        return;
    }
    lbl_804D4C08 = 0;
    for (i = 0; i < 8; i++) {
        if (lbl_8039EB70[0x10 + i] == 1) {
            lbl_8039EB70[0x10 + i] = 0;
        }
        if (lbl_8039EB70[0x08 + i] == 1) {
            lbl_8039EB70[0x08 + i] = 0;
        }
        if (lbl_8039EB70[0x18 + i] == 1) {
            lbl_8039EB70[0x18 + i] = 0;
        }
    }
    if (lbl_804D4C10 == 1) {
        lbl_804D4C10 = 0;
    }
    if (lbl_804D4C14 == 1) {
        lbl_804D4C14 = 0;
    }
    if (lbl_804D4C0C == 1) {
        lbl_804D4C0C = 0;
    }
    Fn80140778();
    Fn801408C8();
}

extern "C" void fn_80127D7C();
extern "C" void Fn80140918();

// Sets up the spline race from a configuration block (same object layout as SplineGroup).
void SplineSetup(int* cfg) {
    SplineGroup* g = (SplineGroup*)cfg;
    SplineQueryMsg q;
    SplineOptMsg s;
    int i;
    int v;

    if (lbl_804D4C08 == 0) {
        return;
    }
    q.hash = 0x31809f6f;
    q.unk8 = 2;
    *(int*)&q.result = 0xf34bfda9;
    SendMsg(0x4b000000, 0x41d, &q);

    q.hash = 0x02d7303a;
    q.unk8 = 0;
    *(int*)&q.result = lbl_8045E768.count;
    SendMsg(0x4b000000, 0x41d, &q);

    q.hash = 0xa8f9c0ee;
    q.unk8 = 0;
    *(int*)&q.result = 0x12c;
    SendMsg(0x4b000000, 0x41d, &q);

    q.hash = 0xf90653ca;
    q.unk8 = 0;
    *(int*)&q.result = 0x8;
    SendMsg(0x4b000000, 0x41d, &q);

    if (cfg[2] != 0) {
        v = cfg[5];
        if (v != 0) {
            lbl_8039F6B0[2] = v * 0x3c;
        }
        lbl_8039F6B0[1] = (v != 0);
        lbl_8039F6B0[0] = 99;
        if (cfg[4] != 0) {
            lbl_8039F6B0[0] = cfg[4];
        }
        s.hash = 0xf0fb92df;
        s.value = cfg[6];
        fn_80127274(0, &s);
        for (i = 0; i < 0x15; i++) {
            switch (i) {
            case 2: {
                SplineQueryMsg qm;
                qm.hash = 0x0e84b88b;
                qm.unk8 = 0;
                *(int*)&qm.result = (cfg[3] >> 2) & 1;
                SendMsg(0x4b000000, 0x41d, &qm);
                break;
            }
            case 3:
                lbl_8039F6B0[16] = (cfg[3] >> 3) & 1;
                break;
            case 9: {
                SplineOptMsg om;
                om.value = (cfg[3] >> 9) & 1;
                om.hash = 0x9c3699ce;
                fn_80127274(0, &om);
                om.hash = 0x6e9f5fb8;
                fn_80127274(0, &om);
                break;
            }
            case 10: {
                SplineOptMsg om;
                om.hash = 0xe90b5145;
                om.value = (cfg[3] >> 10) & 1;
                fn_80127274(0, &om);
                break;
            }
            case 11: {
                SplineOptMsg om;
                om.hash = 0x78f38f77;
                om.value = (cfg[3] >> 11) & 1;
                fn_80127274(0, &om);
                break;
            }
            case 12:
                lbl_8039F6B0[12] = (cfg[3] >> 12) & 1;
                break;
            case 13:
                lbl_8039F6B0[14] = (cfg[3] >> 13) & 1;
                break;
            case 14:
                lbl_8039F6B0[15] = (cfg[3] >> 14) & 1;
                break;
            }
        }
    }
    {
        int b1[9];
        int b2[2];
        fn_80127D7C();
        Fn80140918();
        for (i = 0; i < g->count; i++) {
            fn_80120D28(lbl_804D75AC, 0x7bfb940d, 0, i, 0, 0);
            fn_80120D28(lbl_804D75AC, 0xe2d6a9c0, 0, i, 0, 0);
        }
        fn_800B949C(0x5a9b315c, b1, 1, 1);
        for (i = 0; i < g->count; i++) {
            if ((u32)g->ids[i] >> 24 == 0x13) {
                int a = fn_800A3990();
                int r = fn_8007EA84(-1, b1[0]);
                *(int*)(*(int*)(a + 200) + 0xc) = r;
            }
        }
        fn_800B949C(0x5a9b315c, b2, 1, 1);
        for (i = 0; i < g->count; i++) {
            if ((u32)g->ids[i] >> 24 == 0x13) {
                int a = fn_800A3990();
                int r = fn_8007EA84(-1, b2[0]);
                *(int*)(*(int*)(a + 200) + 0xc) = r;
            }
        }
        for (i = 0; i < fn_800A3854(); i++) {
            fn_80120D28(lbl_804D75AC, 0x7bfb940d, 3, i, 0, 0);
        }
    }
    for (i = 0; i < 8; i++) {
        if ((g->unk128 & (4 << i)) != 0) {
            fn_80120D28(lbl_804D75AC, 0xe2d6a9c0, 1, i, 0, 0);
        }
    }
}

// Resets the spline subsystem: clears the state block, derives the length scale from the
// configuration table and frees the eight slot flags.
void SplineResetState(void) {
    int i;

    lbl_804D4C08 = 1;
    memset(&lbl_8045E768, 0, sizeof(lbl_8045E768));
    lbl_8045E768.unk00 = 0;
    lbl_8045E768.unk08 = lbl_804D75B4;
    lbl_8045E768.unk50 = -1;
    lbl_8045E768.unk04 = lbl_804D75B8 * (float)lbl_8039F6B0[2];
    for (i = 0; i < 8; i++) {
        lbl_8039EB70[i] = 0;
    }
}

// Tears the spline race down: resets the state block, releases the actors and clears the
// slot tables.
void SplineSceneUnload(SplineGroup* g) {
    int i;
    SplineQueryMsg q;
    SplineCmdMsg cmd;

    lbl_8045E768.unk00 = 0;
    lbl_8045E768.unk08 = lbl_804D75B4;
    lbl_8045E768.unk04 = lbl_804D75B8 * (float)lbl_8039F6B0[2];
    fn_800F8ED0();
    fn_800B969C();
    fn_800FC764(lbl_8045E768.unk50, 0);
    for (i = 0; i < fn_800A3854(); i++) {
        fn_800FC764((i & 0xffffff) | 0x13000000, 0);
    }
    for (i = 0; i < g->count; i++) {
        fn_80120AC0(lbl_804D75AC, 0x7bfb940d, 0, i);
        fn_80120AC0(lbl_804D75AC, 0xe2d6a9c0, 0, i);
        if ((u32)g->ids[i] >> 24 == 0x13) {
            cmd.a = 1;
            SendMsg(g->ids[i], 0xc00, &cmd);
        } else {
            int a = fn_802A2024(g->ids[i] & 0xffffff);
            fn_800FC764(g->ids[i], 0);
            fn_8010FE64(a, 0x2d3432c);
            fn_8010FE38(a, 1);
        }
        q.hash = 0x7c18fe9e;
        q.unk8 = 0;
        *(int*)&q.result = 0;
        SendMsg(g->ids[i], 0x41e, &q);
        q.hash = 0x0c343772;
        q.unk8 = 0;
        *(int*)&q.result = g->unk108;
        SendMsg(g->ids[i], 0x41d, &q);
    }
    q.hash = 0xf90653ca;
    q.unk8 = 0;
    *(int*)&q.result = g->unkC4;
    SendMsg(0x4b000000, 0x41d, &q);
    if (lbl_8045E768.count < g->unkC4 && lbl_8039F6B0[3] == 0) {
        q.hash = 0xe90b5145;
        q.unk8 = 0;
        *(int*)&q.result = 1;
        SendMsg(0x4b000000, 0x41d, &q);
    }
    for (i = 0; i < 8; i++) {
        lbl_8039EB70[i] = 0;
        if (lbl_8039EB70[0x10 + i] == 1) {
            lbl_8039EB70[0x10 + i] = 0;
        }
        if (lbl_8039EB70[0x08 + i] == 1) {
            lbl_8039EB70[0x08 + i] = 0;
        }
        if (lbl_8039EB70[0x18 + i] == 1) {
            lbl_8039EB70[0x18 + i] = 0;
        }
        lbl_8045E7BC[i] = 0;
        lbl_8045E7DC[i] = 0;
    }
    if (lbl_804D4C10 == 1) {
        lbl_804D4C10 = 0;
    }
    if (lbl_804D4C14 == 1) {
        lbl_804D4C14 = 0;
    }
    if (lbl_804D4C0C == 1) {
        lbl_804D4C0C = 0;
    }
    lbl_804D4C18 = 0;
    lbl_804D4C1C = 0;
    Fn80140778();
}

// Draws the per-player spline HUD entry: timer, position and score.
void SplineEntryUpdate(SplineGroup* g, int index) {
    int team;
    int score;
    int text[4];
    float* color;

    if (lbl_8039EB70[index] != 0) {
        lbl_8039EB70[index] = 0;
    }
    team = fn_800FD68C(index);
    color = fn_800FD424(team);
    score = (int)fn_80120C14(0xe2d6a9c0, 1, team);
    if (index == 2) {
        fn_8013E998(0x1d65ccec, text);
    } else if (index < 2) {
        if (index == 0) {
            fn_8013E998(0x4da3732e, text);
        } else if (index >= 0) {
            fn_8013E998(0xb584a00d, text);
        }
    } else if (index < 4) {
        fn_8013E998(0x8546f9cb, text);
    }
    if (lbl_8039EB70[0x10 + index] == 0) {
        lbl_8039EB70[0x10 + index] = 1;
        lbl_8045E7BC[index] = score;
    }
    Fn801ED0C8();
    Fn801ED1E4((int)(lbl_804D75A4 * color[0]) & 0xff, (int)(lbl_804D75A4 * color[1]) & 0xff,
               (int)(lbl_804D75A4 * color[2]) & 0xff, 0xff);
    fn_8007E1E8(text[3]);
    fn_8007E570(text[0], text[1], 2);
    Fn801EB188(lbl_80345737 + 8, score);
    if (score != lbl_8045E7BC[index]) {
        lbl_8045E7BC[index] = score;
    }
    if (lbl_8039EB70[0x08 + index] == 0) {
        lbl_8039EB70[0x08 + index] = 1;
    }
    if (lbl_8039F6B0[4] != 0) {
        int s = (int)fn_80120C14(0x7bfb940d, 0, index);
        if (s > 0) {
            if (lbl_8039EB70[0x18 + index] == 0) {
                lbl_8039EB70[0x18 + index] = 1;
                lbl_8045E7DC[index] = s;
            }
            if (s != lbl_8045E7DC[index]) {
                lbl_8045E7DC[index] = s;
            }
            Fn801ED1E4((int)(lbl_804D75A4 * color[0]) & 0xff,
                       (int)(lbl_804D75A4 * color[1]) & 0xff,
                       (int)(lbl_804D75A4 * color[2]) & 0xff, 0xff);
            Fn801EB188(lbl_80345737 + 0xe, s);
        }
        if (s == 0) {
            lbl_8045E7DC[index] = 0;
        }
    }
}

// Marks slot `index` of the spline slot table as used.
void SplineSlotSetUsed(int index) {
    lbl_8039EB70[index] = 1;
}

extern "C" int fn_80153B28();

// Links two players into the spline race and starts their timers.
int SplineEntryLink(int owner, int a, int b) {
    float time = lbl_804D7598;
    int p1;
    int p2;
    int t1;
    int t2;

    p1 = fn_800FD720(a);
    t1 = fn_800FD68C(p1);
    p2 = fn_800FD720(b);
    t2 = fn_800FD68C(p2);
    if (p1 == -1 || p2 == -1) {
        return 0;
    }
    if (lbl_8039F6B0[12] == 0 && lbl_8039F6B0[5] != 0) {
        int out[2];
        fn_8012071C(0xe2d6a9c0, 0, out, 1, 1);
        if (fn_80120C14(0xe2d6a9c0, 0, p1) == fn_80120C14(0xe2d6a9c0, 0, out[0])) {
            time = lbl_804D75C8;
        }
    }
    fn_8013FCF0(owner, p2, p1);
    if (t1 == t2) {
        if (lbl_8039F6B0[10] != 0) {
            int v = lbl_8039F6B0[10];
            if (lbl_8039F6B0[12] != 0) {
                int s = v >> 31;
                int den = (s ^ v) - s;
                v = (v / den) * (1 << den);
            }
            time = (float)v;
            fn_80120C80(time, 0xe2d6a9c0, 1, t2);
            fn_80120C80(time, 0xe2d6a9c0, 0, p2);
        }
        return 1;
    }
    if (lbl_8039F6B0[12] == 0) {
        if (fn_80120C14(0x7bfb940d, 0, p2) >= lbl_804D75CC) {
            time = lbl_804D75AC;
        }
    } else {
        float d = fn_80120C14(0x7bfb940d, 0, p2);
        time = lbl_804D7598;
        if (lbl_804D7598 <= d && lbl_804D75CC <= d) {
            time = lbl_804D75AC;
        }
    }
    {
        u32 id = fn_800FD6EC(p2);
        if (id >> 24 == 0x13) {
            fn_8002394C();
            int x = fn_80116DC4();
            if (x == 0) {
                id = 0xffffffff;
            } else {
                id = *(u32*)(x + 0x38);
            }
        } else {
            id = fn_80153B28();
        }
        if (id >> 24 == 0x13) {
            fn_80120C80(time, 0x7bfb940d, 0, p2);
        }
    }
    return 1;
}

// Attaches one player to the running spline race and awards the collected score.
int SplineEntryAttach(int owner, int a, int b) {
    int h;
    int team;
    int add;
    int cur;
    float* color;
    SplineQueryMsg q;
    char buf[0x100];
    int i;

    h = fn_800FD720(b);
    fn_8012E5A8(b);
    team = fn_800FD68C(h);
    if (lbl_8039F6B0[4] == 0 || lbl_8039F6B0[7] == 0 ||
        fn_80120C14(0x7bfb940d, 0, h) != lbl_804D75AC) {
        add = (lbl_8039F6B0[8] != 0);
        if (h == -1) {
            fn_80120C80((float)add, 0xe2d6a9c0, 1, team);
            q.hash = 0x372c8784;
            q.unk8 = 0;
            *(int*)&q.result = 1;
            SendMsg(owner, 0x41d, &q);
        } else {
            if (lbl_8039F6B0[4] != 0) {
                add = add + (int)fn_80120C14(0x7bfb940d, 0, h);
            }
            if (team != fn_800FD68C(h) && lbl_8039F6B0[3] == 0) {
                return 0;
            }
            cur = (int)fn_80120C14(0xe2d6a9c0, 1, team);
            if (lbl_8039F6B0[0] < cur + add) {
                add = lbl_8039F6B0[0] - cur;
            }
            fn_80120C80((float)add, 0xe2d6a9c0, 0, h);
            fn_80120AC0(lbl_804D75AC, 0x7bfb940d, 0, h);
            fn_80120C80((float)add, 0xe2d6a9c0, 1, team);
            q.hash = 0x372c8784;
            q.unk8 = 0;
            *(int*)&q.result = 1;
            SendMsg(owner, 0x41d, &q);
            if (add > 0) {
                int fl;
                int key;
                int ent;
                int text;
                int mag = add < 0 ? -add : add;

                color = fn_800FD424(team);
                key = fn_80086364(0x2c49feb0);
                ent = fn_800FD334(h);
                fl = mag > 1 ? 0x73 : 0;
                fn_802F09EC(buf, key, ent, add, fl);
                text = fn_80170834(lbl_80345737 + 0x12);
                fn_8016F390(text, 0, 0);
                for (i = 0; i < ((SplineGroup*)owner)->count; i++) {
                    if (fn_800FD6EC(i) >> 24 == 2) {
                        fn_800FEF2C(color[0], color[1], color[2], lbl_804D759C, i, buf);
                    }
                }
                text = fn_8007EB34(0x92234d44, 0xbbd48fcc);
                fn_8016F390(text, 0, 0);
            }
        }
        return 1;
    }
    return 0;
}

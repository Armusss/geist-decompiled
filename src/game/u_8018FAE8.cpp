// u_8018FAE8.cpp: ghost hint and input helpers (0x8018FAE8-0x80191C6C).
// Names are descriptive guesses; the code is matched against the retail DOL.

// u_8018FAE8.cpp: ghost hint / tutorial message system.
// Names are descriptive guesses based on observed behaviour.

struct Vec3 { float x, y, z; };

struct Actor;

// Object updated by the hint system (field names are descriptive guesses).
struct GhostHintObject {
    unsigned char pad00[0x24];
    Vec3 offset;              // 0x24: position offset applied to the model
    unsigned char pad30[0x04];
    float alpha;              // 0x34
    unsigned char pad38[0x08];
    float colorR;             // 0x40
    float colorG;             // 0x44
    float colorB;             // 0x48
    int mode;                 // 0x4C
    int blend;                // 0x50
    int priority;             // 0x54
};

// 3x4 column-major matrix (4 columns of 3 floats) used by the model code.
struct Mtx34 { float m[4][3]; };

// One bone transform filled in by ModelAnimate (layout is a guess).
struct ModelBone {
    Vec3 pos;
    Vec3 rot;
    float scale;
};

// Draw parameters handed to the model draw helper (field names are guesses).
struct GhostHintDrawParams {
    int flags;                  // 0x00
    unsigned char colorTop[4];  // 0x04
    int blend;                  // 0x08
    int mode;                   // 0x0c
    unsigned char colorBot[4];  // 0x10
    Vec3 offset;                // 0x14
    int unk20;                  // 0x20
    float scale;                // 0x24
    int unk28;                  // 0x28
    int unk2c;                  // 0x2c
    float colorA[4];            // 0x30
    float colorB[3];            // 0x40
    Vec3 offsetEnd;             // 0x4c
    int priority;               // 0x58
};

extern "C" {
void fn_800FF530(void);
void fn_8009F7A8(void* obj, void* vec);
void fn_800C6900(int handle);
void ModelFreeIds(short* ids, int count);
int fn_800C6AE8(void* obj, int a, int b, int c);
void fn_8016D6A8(Vec3* angle);
void fn_80190018(int modelId, unsigned int frame = 0);
int fn_8003A1F0(int modelId, void* bones, int anim);
void fn_8003F15C(int modelId, int node, int boneCount, void* params, int flags,
                 float time);
void fn_800C6834(int handle);
void fn_800C5E9C(int arg);
void fn_800C6664(int handle);
void fn_80009E10(void);
void fn_80008F64(void);
}

// Model interface (definitions live in the model unit).
int ModelBoneCount(int modelId);
void MtxSetEuler(Mtx34* mtx, Vec3* euler);
int ModelAnimate(int modelId, char* bones, int arg);
void ModelSetMatrix(int modelId, Mtx34* mtx, char* bones);
void ModelLoadIds(short* ids, int count);
int PadPressed(int pad, int a, int b);
void DeferFlush3(void);

const char* StrFindKey(unsigned long key, unsigned long* index);
void TextSetStyle(int a, int b, int c, int d, int e, int f, int g, int h,
                  int i, int j, int k, int l);
void TextSetBox(int a, int b, int c, int d);
void TextSetSize(int size);
void TextSetFlags(int a, int b);
void TextDraw(const char* str, unsigned long index);
void DebugText(const char* fmt, ...);

void GhostActionSetActive(int active);
void EulerNormalize(Vec3* src, Vec3* dst);
const char* StrGetIndex(int table, unsigned long* index);
Actor* GetPlayer(void);
int PlayerHud(Actor* player);
void HudMessageN(int arg0, const char* str, unsigned long index,
                 float a, float b, float c, float d);

extern int lbl_804D5124;      // state
extern float lbl_804D5128;    // hold timer
extern int lbl_804D512C;      // hint string table
extern float lbl_804D3060;    // blend factor
extern float lbl_804D8598;    // blend rate (in)
extern float lbl_804D859C;    // blend minimum
extern float lbl_804D85A0;    // message colour
extern float lbl_804D85A4;    // message alpha
extern float lbl_804D85A8;    // hold timer end
extern float lbl_804D85AC;    // blend rate (out)
extern float lbl_804D85B0;    // blend maximum
extern float lbl_804D85B4;    // spin rate

extern Vec3 lbl_803470F8;     // offset scale
extern Vec3 lbl_803A8F68;     // offset origin
extern Vec3 lbl_80495B40;     // hint angle
extern GhostHintObject lbl_80495AE0;
extern char lbl_80347110[];   // data block holding debug text at +0x47

extern float lbl_804D85B8;    // hint object alpha
extern float lbl_804D85C0;    // hint object colour (green)
extern float lbl_804D85C4;    // hint angle x
extern float lbl_804D85C8;    // hint colour
extern float lbl_804D85CC;    // hold timer start
extern float lbl_804D85D0;    // blend value after reset
extern float lbl_804D50F0;    // hint colour
extern float lbl_804D510C;    // hint blend value
extern int lbl_804D50F4;      // number of hint pages
extern int lbl_804D50F8;      // current hint page
extern int lbl_804959E0[];    // string id per hint page
extern int lbl_80495A20[];    // string table per hint page

extern float lbl_804D85BC;    // hint object colour (red)
extern float lbl_804D85E0;    // hint model scale
extern float lbl_804D85E4;    // hint model frame period
extern float lbl_804D85E8;    // hint page timer rate
extern int lbl_804D50FC;      // hint model bone count
extern int lbl_804D5104;      // hint page state
extern float lbl_804D5108;    // hint page timer

extern double lbl_804D85D8;   // 2^52: unsigned-int to float conversion bias
extern Vec3 lbl_80347104;     // hint model offset scale
extern float lbl_80495AA0[];  // per-page vertical offset

// Per-frame update of the ghost hint display.
void GhostHintUpdate(float dt)
{
    switch (lbl_804D5124) {
    case 1:
        lbl_804D3060 -= lbl_804D8598 * dt;
        if (lbl_804D3060 <= lbl_804D859C) {
            lbl_804D3060 = lbl_804D859C;
            lbl_804D5124 = 2;
            unsigned long index = 0;
            const char* str = StrGetIndex(lbl_804D512C, &index);
            HudMessageN(PlayerHud(GetPlayer()), str, index, lbl_804D85A0,
                        lbl_804D85A0, lbl_804D85A0, lbl_804D85A4);
        }
        break;
    case 2:
        lbl_804D5128 -= dt;
        fn_800FF530();
        if (lbl_804D5128 <= lbl_804D85A8) {
            lbl_804D5128 = lbl_804D85A8;
            lbl_804D5124 = 3;
        }
        break;
    case 3:
        lbl_804D3060 += lbl_804D85AC * dt;
        fn_800FF530();
        if (lbl_804D3060 >= lbl_804D85B0) {
            lbl_804D3060 = lbl_804D85B0;
            lbl_804D5124 = 0;
            GhostActionSetActive(1);
        }
        break;
    }

    lbl_80495B40.y += lbl_804D85B4 * dt;
    EulerNormalize(&lbl_80495B40, &lbl_80495B40);

    Vec3 v = lbl_803470F8;
    lbl_80495AE0.offset.x = v.x * lbl_804D3060 + lbl_803A8F68.x;
    lbl_80495AE0.offset.y = v.y * lbl_804D3060 + lbl_803A8F68.y;
    lbl_80495AE0.offset.z = v.z * lbl_804D3060 + lbl_803A8F68.z;
    v.x = -v.x;
    v.y = -v.y;
    v.z = -v.z;
    fn_8009F7A8(&lbl_80495AE0, &v);
}

extern short lbl_804D5122;   // hint model id
extern int lbl_804D5100;      // hint model handle

// Releases the hint model resources.
void GhostHintShutdown(void)
{
    ModelFreeIds(&lbl_804D5122, 1);
    fn_800C6900(lbl_804D5100);
}

// Resets the hint display: loads the hint model, creates its render object and
// puts the blend state back to its start value.
void GhostHintReset(void)
{
    ModelLoadIds(&lbl_804D5122, 1);
    lbl_804D5100 = fn_800C6AE8(&lbl_80495AE0, 0, 0, 0);
    lbl_80495AE0.alpha = lbl_804D85B8;
    lbl_80495AE0.colorR = lbl_804D85BC;
    lbl_80495AE0.colorG = lbl_804D85C0;
    lbl_80495AE0.colorB = lbl_804D85BC;
    lbl_80495AE0.mode = 0;
    lbl_80495AE0.blend = 8;
    lbl_80495AE0.priority = 0x10;
    fn_8016D6A8(&lbl_80495B40);
    lbl_803A8F68.x = lbl_804D85A8;
    lbl_803A8F68.y = lbl_804D85A8;
    lbl_803A8F68.z = lbl_804D85A8;

    Vec3 v = lbl_803470F8;
    float sx = v.x * lbl_804D859C + lbl_804D85A8;
    float sy = v.y * lbl_804D859C + lbl_804D85A8;
    float sz = v.z * lbl_804D859C + lbl_804D85A8;

    lbl_80495B40.x = lbl_804D85C4;
    lbl_80495B40.z = lbl_804D85A8;
    lbl_804D3060 = lbl_804D859C;
    lbl_804D50F0 = lbl_804D85C8;
    lbl_804D510C = lbl_804D859C;
    lbl_804D5104 = 0;
    lbl_804D5108 = lbl_804D85A8;
    lbl_80495AE0.offset.x = sx;
    lbl_80495AE0.offset.y = sy;
    lbl_80495AE0.offset.z = sz;
    v.x = -v.x;
    v.y = -v.y;
    v.z = -v.z;
    fn_8009F7A8(&lbl_80495AE0, &v);
    lbl_804D5124 = 1;
    lbl_804D5128 = lbl_804D85CC;
    lbl_804D3060 = lbl_804D85D0;
}

// Draws the two text blocks shown for the current hint page.
int GhostHintDrawText(int unnamed, unsigned long frame)
{
    unsigned long index;
    float t = (float)frame - lbl_804D85D8;

    if (t <= lbl_804D5108) {
        fn_80190018(lbl_804959E0[lbl_804D50F8]);
    } else {
        fn_80190018(lbl_804959E0[lbl_804D50F8], (int)lbl_804D5108);
    }

    TextSetStyle(0xff, 0xff, 0xff, frame & 0xff, 0x18, 4, 0, 0, 2, 0, 0, 0);
    TextSetBox(0, 0x280, 0, 0x82);
    TextSetSize(0x14);
    TextSetFlags(2, 0);

    const char* str = StrGetIndex(lbl_80495A20[lbl_804D50F8], &index);
    if (str != 0) {
        TextDraw(str, index);
    }
    DebugText(&lbl_80347110[0x47]);

    TextSetStyle(0xff, 0xff, 0xff, frame & 0xff, 0x18, 4, 0, 0, 2, 0, 0, 0);
    TextSetBox(0, 0x280, 0, 0x5a);
    TextSetSize(0xd);
    TextSetFlags(2, 0);

    str = StrFindKey(0x97fd6efd, &index);
    if (str != 0 && lbl_804D50F4 > 1) {
        TextDraw(str, index);
    }
    return 1;
}

// fn_80190018 (GhostHintModelDraw): draws the hint model of the current page,
// animating it at the given frame time. Other units call it by this name.
extern "C" void fn_80190018(int modelId, unsigned int frame)
{
    Vec3 offset = lbl_80347104;
    int boneCount;
    int node;

    boneCount = ModelBoneCount(modelId);
    lbl_804D50FC = boneCount;
    if (boneCount < 0) {
        node = 0;
    } else {
        ModelBone bones[16];
        Mtx34 mtx;
        int anim;

        MtxSetEuler(&mtx, &lbl_80495B40);
        mtx.m[3][0] = lbl_804D85A8;
        mtx.m[3][1] = -lbl_80495AA0[lbl_804D50F8];
        mtx.m[3][2] = lbl_804D85A8;
        anim = ModelAnimate(modelId, (char*)bones, 1);
        ModelSetMatrix(modelId, &mtx, (char*)bones);
        node = fn_8003A1F0(modelId, bones, anim);
    }
    if (node != 0) {
        GhostHintDrawParams params;

        params.flags = 1;
        params.colorTop[0] = 0x20;
        params.colorTop[1] = 0x20;
        params.colorTop[2] = 0x20;
        params.colorTop[3] = 0xff;
        params.blend = 1;
        params.mode = 1;
        params.colorBot[0] = 0xff;
        params.colorBot[1] = 0xff;
        params.colorBot[2] = 0xff;
        params.colorBot[3] = 0xff;
        params.offset = offset;
        params.unk20 = 0;
        params.scale = lbl_804D85E0;
        params.unk2c = 0;
        params.colorA[0] = lbl_804D85A0;
        params.colorA[1] = lbl_804D85A0;
        params.colorA[2] = lbl_804D85A0;
        params.colorA[3] = lbl_804D85A0;
        params.colorB[0] = lbl_804D85A8;
        params.colorB[1] = lbl_804D85A8;
        params.colorB[2] = lbl_804D85A8;
        params.offsetEnd = offset;
        params.priority = -0x20;

        fn_800C6834(lbl_804D5100);
        fn_800C5E9C(0);
        fn_8003F15C(modelId, node, lbl_804D50FC, &params, 0,
                    (float)frame / lbl_804D85E4);
        DeferFlush3();
        fn_80009E10();
        fn_80008F64();
        fn_800C6664(lbl_804D5100);
    }
}

void GhostHintUpdateModel(int pad, float dt);

// GhostHintInput: pads through the hint pages and steps the page timer.
// (Descriptive name based on observed behaviour.)
extern "C" void GhostHintInput(int pad, float dt)
{
    if (PadPressed(pad, 0, 4) == 1) {
        GhostActionSetActive(0);
        return;
    }
    if (PadPressed(pad, 0, 3) == 1 && lbl_804D50F4 > 1) {
        lbl_804D5104 = 2;
    }
    if (lbl_804D5104 == 2) {
        if (lbl_804D5108 <= lbl_804D85A8) {
            lbl_804D50F8++;
            lbl_804D5108 = lbl_804D85A8;
            if (lbl_804D50F8 >= lbl_804D50F4) {
                lbl_804D50F8 = 0;
            }
            fn_8016D6A8(&lbl_80495B40);
            lbl_803A8F68.x = lbl_804D85A8;
            lbl_803A8F68.y = lbl_804D85A8;
            lbl_803A8F68.z = lbl_804D85A8;
            lbl_80495B40.x = lbl_804D85C4;
            lbl_80495B40.z = lbl_804D85A8;
            lbl_804D3060 = lbl_804D859C;
            lbl_804D50F0 = lbl_804D85C8;
            lbl_804D510C = lbl_804D859C;
            lbl_804D5104 = 0;
            lbl_804D5108 = lbl_804D85A8;
        } else {
            lbl_804D5108 -= lbl_804D85E8 * dt;
        }
    } else if (lbl_804D5104 == 0) {
        if (lbl_804D5108 >= lbl_804D85E4) {
            lbl_804D5108 = lbl_804D85E4;
            lbl_804D5104 = 1;
        } else {
            lbl_804D5108 += lbl_804D85E8 * dt;
        }
    }
    GhostHintUpdateModel(pad, dt);
}

// Releases the model ids of every loaded hint page and the hint render object.
extern "C" void GhostHintFreeModelIds(void)
{
    short id;

    for (int i = 0; i < lbl_804D50F4; i++) {
        id = (short)lbl_804959E0[i];
        ModelFreeIds(&id, 1);
    }
    fn_800C6900(lbl_804D5100);
}

// Buffer/String interfaces from the container unit (layouts are guesses).
// The retailer calls Buffer's destructor for a String local, i.e. String has
// no destructor of its own; the buffer pointer is the first field.
class Buffer {
public:
    char* mData;              // 0x00
    int mLen;                 // 0x04
    int mCapacity;            // 0x08
    ~Buffer();
};

class String : public Buffer {
public:
    int mIndex;               // 0x0c
    int mUnk10;               // 0x10
    String();
};

// Query block filled in by the hint-name message (layout taken from retail).
struct GhostHintNameQuery {
    int status;               // 0x00
    unsigned long key;        // 0x04
    int zero;                 // 0x08
    String name;              // 0x0c
};

extern int lbl_80495A60[];    // string table per hint page
extern "C" int SendMsg(int to, int msg, void* data);

void* ActorGet(int index);
int fn_800F81EC(const char* list);
int fn_800F8130(const char* list, int index);
int fn_800F7A90(void);
unsigned int fn_800F8AF4(int entry);
int fn_800F7994(int entry, unsigned long key);
int fn_800DEF3C(int entry, unsigned long key);
double fn_800DEDF8(int entry, unsigned long key);
int fn_8007EB34(unsigned long a, unsigned long b);

// Loads the hint page list from the hint-name message and creates the hint
// render object; falls back to a single default page when none is authored.
extern "C" void GhostHintInit(int index)
{
    GhostHintNameQuery query;
    const char* list;
    void* target;
    int pageCount;
    int i;

    target = (void*)*(int*)((char*)ActorGet(index) + 0x38);
    query.status = 0;
    query.key = 0x2da8293c;
    query.zero = 0;
    SendMsg((int)target, 0x41e, &query);
    list = query.name.mData;

    lbl_804D50F4 = 0;
    lbl_804D50F8 = 0;
    pageCount = fn_800F81EC(list);
    lbl_804D50F0 = lbl_804D85A8;
    if (pageCount > 0) {
        for (i = 0; i < pageCount; i++) {
            int entry = fn_800F8130(list, i);
            int type = fn_800F7A90();

            if (fn_800F8AF4(entry) != 0xAA5212D1u) {
                if (type == 0x8CA7FE7Cu) {
                    lbl_804959E0[lbl_804D50F4] =
                        fn_800F7994(entry, 0x7cce3f4d);
                    lbl_80495A20[lbl_804D50F4] =
                        fn_800F7994(entry, 0x5edd9dc7);
                    lbl_80495A60[lbl_804D50F4] =
                        fn_800F7994(entry, 0x94001120);
                    lbl_80495AA0[lbl_804D50F4] = lbl_804D85A8;
                    lbl_804D50F4 = lbl_804D50F4 + 1;
                } else if ((type == 0x9860B61Cu || type == 0x276198) &&
                           fn_800F8AF4(entry) != 0x3BAD43EFu) {
                    int sub = fn_800F7994(entry, 0xc1e64319);

                    lbl_804959E0[lbl_804D50F4] =
                        fn_800DEF3C(sub, 0x7cce3f4d);
                    lbl_80495A20[lbl_804D50F4] =
                        fn_800DEF3C(sub, 0x5edd9dc7);
                    lbl_80495A60[lbl_804D50F4] =
                        fn_800DEF3C(sub, 0x94001120);
                    lbl_80495AA0[lbl_804D50F4] =
                        (float)fn_800DEDF8(sub, 0x2e53de90);
                    lbl_804D50F4 = lbl_804D50F4 + 1;
                }
            }
        }
    }
    if (lbl_804D50F4 < 1) {
        lbl_80495A20[0] = fn_8007EB34(0xbaa22e62, 0x3e7efa10);
        lbl_80495A60[0] = fn_8007EB34(0xbaa22e62, 0x9ad9d6f5);
        lbl_804959E0[0] = -1;
        lbl_804D50F4 = 1;
    } else {
        for (i = 0; i < lbl_804D50F4; i++) {
            short id = (short)lbl_804959E0[i];

            ModelLoadIds(&id, 1);
        }
    }

    lbl_804D5100 = fn_800C6AE8(&lbl_80495AE0, 0, 0, 0);
    lbl_80495AE0.alpha = lbl_804D85B8;
    lbl_80495AE0.colorR = lbl_804D85BC;
    lbl_80495AE0.colorG = lbl_804D85C0;
    lbl_80495AE0.colorB = lbl_804D85BC;
    lbl_80495AE0.mode = 0;
    lbl_80495AE0.blend = 8;
    lbl_80495AE0.priority = 0x10;
    fn_8016D6A8(&lbl_80495B40);
    lbl_803A8F68.x = lbl_804D85A8;
    lbl_803A8F68.y = lbl_804D85A8;
    lbl_803A8F68.z = lbl_804D85A8;

    Vec3 neg;

    lbl_80495AE0.offset.x = lbl_803470F8.x * lbl_804D859C + lbl_804D85A8;
    neg.x = -lbl_803470F8.x;
    lbl_80495AE0.offset.y = lbl_803470F8.y * lbl_804D859C + lbl_804D85A8;
    neg.y = -lbl_803470F8.y;
    lbl_80495B40.x = lbl_804D85C4;
    lbl_80495AE0.offset.z = lbl_803470F8.z * lbl_804D859C + lbl_804D85A8;
    neg.z = -lbl_803470F8.z;
    lbl_80495B40.z = lbl_804D85A8;
    lbl_804D3060 = lbl_804D859C;
    lbl_804D50F0 = lbl_804D85C8;
    lbl_804D510C = lbl_804D859C;
    lbl_804D5104 = 0;
    lbl_804D5108 = lbl_804D85A8;
    fn_8009F7A8(&lbl_80495AE0, &neg);
}

// --- ghost hint model tracking (writes the shared hint state above) ---------
extern char lbl_804D5118;     // angle defaults installed
extern char lbl_804D5120;     // blend defaults installed
extern float lbl_804D5114;    // time step of the last update
extern int lbl_804D511C;      // hint is pinned in place
extern int lbl_804D5110;      // hint origin was moved this frame
extern float lbl_804D85EC;    // "no input" axis value
extern float lbl_804D85F0;    // blend upper bound
extern float lbl_804D85F4;    // turn rate
extern float lbl_804D85F8;    // turn divisor
extern float lbl_804D85FC;    // angle lower bound
extern float lbl_804D8600;    // angle upper bound
extern float lbl_804D8604;    // blend lower bound
extern float lbl_804D8608;    // camera follow factor
extern double lbl_804D8610;   // blend return factor
extern Vec3 lbl_80495B70;     // movement target (same block as lbl_80495B40)
extern Vec3 lbl_80495B88;     // previous position (same block as lbl_80495B40)

extern "C" {
float fn_800D8BE4(int pad, int group, int axis, int unused);
float fn_8013E988(void);
int fn_802A1D04(void);
int fn_800FD3DC(void);
int fn_800FD3EC(void);
void fn_800A0168(Mtx34* mtx, const void* obj);
void fn_800A0030(Vec3* out, const void* mtx, const Vec3* in);
void fn_800A0100(void* obj);
void fn_800A0094(Vec3* a, Vec3* b);
void fn_800A025C(Vec3* out, const void* obj, int index);
void* fn_8003A18C(int modelId);
void fn_801406B0(char* out);
int sprintf(char* buf, const char* fmt, ...);
}

void ScoreBoardDrawTeams(void);
void ScoreBoardDrawPlayers(void);

void ViewFrustumUpdate(float* planes, const Vec3* a, const Vec3* b,
                       const Vec3* c, float f1, float f2, float f3, float f4);
int ViewFrustumTestOctahedron(const int* planes, const int* shape);

// Per-frame update of the ghost hint model: reads the pad axes, turns and
// blends the ghost, keeps it anchored and submits its draw data.
void GhostHintUpdateModel(int pad, float dt)
{
    if (lbl_804D5118 == 0) {
        lbl_804D5118 = 1;
        lbl_804D5114 = lbl_804D85A8;
    }
    if (lbl_804D5120 == 0) {
        lbl_804D511C = 0;
        lbl_804D5120 = 1;
    }

    Vec3 origin = lbl_803A8F68;
    int forced = 0;

    float a0 = fn_800D8BE4(pad, 0xc, 0, 0);
    if (a0 == lbl_804D85EC) {
        return;
    }
    float a1 = fn_800D8BE4(pad, 0xc, 1, 0);
    float a3 = fn_800D8BE4(pad, 0xc, 3, 0);
    float a2 = fn_800D8BE4(pad, 0xc, 2, 0);
    float a5 = fn_800D8BE4(pad, 0xc, 5, 0);
    float a4 = fn_800D8BE4(pad, 0xc, 4, 0);

    float rate = lbl_804D3060 / lbl_804D85F0;
    if (rate < lbl_804D85A0) {
        rate = lbl_804D85A0;
    }

    if (a0 != lbl_804D85A8) {
        float v = lbl_80495B40.x + (lbl_804D85F4 * a0 * dt) / lbl_804D85F8;
        lbl_80495B40.x = v;
        if (v < lbl_804D85FC) {
            lbl_80495B40.x = lbl_804D85FC;
        } else if (v > lbl_804D8600) {
            lbl_80495B40.x = lbl_804D8600;
        }
    }
    if (a1 != lbl_804D85A8) {
        float v = lbl_80495B40.z + (lbl_804D85F4 * a1 * dt) / lbl_804D85F8;
        lbl_80495B40.z = v;
        if (v < lbl_804D85FC) {
            lbl_80495B40.z = lbl_804D85FC;
        } else if (v > lbl_804D8600) {
            lbl_80495B40.z = lbl_804D8600;
        }
    }
    if (a3 != lbl_804D85A8 || a2 != lbl_804D85A8) {
        lbl_804D3060 += rate * ((lbl_804D85C4 * a3 * dt) / lbl_804D85F8);
        lbl_804D3060 -= rate * ((lbl_804D85C4 * a2 * dt) / lbl_804D85F8);
        if (lbl_804D3060 < lbl_804D8604) {
            lbl_804D3060 = lbl_804D8604;
        } else if (lbl_804D3060 > lbl_804D85F0) {
            lbl_804D3060 = lbl_804D85F0;
        }
    }

    if (PadPressed(pad, 0xc, 3)) {
        lbl_804D5114 = lbl_804D85A8;
        lbl_80495B88 = lbl_80495B70;
        lbl_804D511C = 1;
        forced = 1;
    }

    int pan = forced != 0;
    if (!pan) {
        if (a5 == lbl_804D85A8 && a4 == lbl_804D85A8 && a0 == lbl_804D85A8 &&
            a1 == lbl_804D85A8 && a3 == lbl_804D85A8 && a2 == lbl_804D85A8) {
            pan = 1;
        } else {
            lbl_804D511C = 0;
            lbl_804D5114 = lbl_804D85D0;
            if (a5 != lbl_804D85A8 || a4 != lbl_804D85A8) {
                Mtx34 mtx;
                Vec3 v;

                v.x = a5;
                v.y = lbl_804D85A8;
                v.z = a4;
                fn_800A0168(&mtx, &lbl_80495AE0);
                fn_800A0030(&v, &mtx, &v);
                float k = lbl_804D85C8 * (rate * (dt / lbl_804D85F8));
                lbl_804D5110 = 1;
                v.x *= k;
                v.y *= k;
                v.z *= k;
                lbl_803A8F68.x += v.x;
                lbl_803A8F68.y += v.y;
                lbl_803A8F68.z += v.z;
                v.x = lbl_803A8F68.x - lbl_80495B88.x;
                v.y = lbl_803A8F68.y - lbl_80495B88.y;
                v.z = lbl_803A8F68.z - lbl_80495B88.z;
            }
        }
    }

    if (pan) {
        lbl_804D5114 = -dt;
        if (-dt < lbl_804D85A8) {
            Vec3 t;
            Vec3 u;
            Vec3 w;

            t.x = lbl_80495AE0.offset.x - lbl_803A8F68.x;
            t.y = lbl_80495AE0.offset.y - lbl_803A8F68.y;
            t.z = lbl_80495AE0.offset.z - lbl_803A8F68.z;
            fn_800A0030(&u, &lbl_80495AE0, &t);
            t.x = lbl_80495AE0.offset.x - lbl_80495B88.x;
            t.y = lbl_80495AE0.offset.y - lbl_80495B88.y;
            t.z = lbl_80495AE0.offset.z - lbl_80495B88.z;
            fn_800A0030(&w, &lbl_80495AE0, &t);
            lbl_804D3060 -= lbl_804D8608 * (w.y - u.y);
            if (lbl_804D3060 < lbl_804D8604) {
                lbl_804D3060 = lbl_804D8604;
            } else if (lbl_804D3060 > lbl_804D85F0) {
                lbl_804D3060 = lbl_804D85F0;
            }

            Vec3 d;
            d.x = w.x - u.x;
            d.y = w.y - u.y;
            d.z = w.z - u.z;
            fn_800A0100(&lbl_80495AE0);
            fn_800A0094(&d, &d);
            lbl_803A8F68.x -= lbl_804D8608 * d.x;
            lbl_803A8F68.y -= lbl_804D8608 * d.y;
            lbl_803A8F68.z -= lbl_804D8608 * d.z;
        }
        if (lbl_804D511C != 0) {
            lbl_80495B40.z -= lbl_804D8608 * lbl_80495B40.z;
            lbl_80495B40.x += lbl_804D8608 * (lbl_804D85C4 - lbl_80495B40.x);
            lbl_804D3060 = (float)(lbl_804D8610 * (lbl_804D859C - lbl_804D3060) +
                                   lbl_804D3060);
        }
        lbl_804D5110 = 0;
    }

    Vec3 dir = lbl_803470F8;
    lbl_80495AE0.offset.x = lbl_803A8F68.x + dir.x * lbl_804D3060;
    lbl_80495AE0.offset.y = lbl_803A8F68.y + dir.y * lbl_804D3060;
    lbl_80495AE0.offset.z = lbl_803A8F68.z + dir.z * lbl_804D3060;
    Vec3 neg;
    neg.x = -dir.x;
    neg.y = -dir.y;
    neg.z = -dir.z;
    fn_8009F7A8(&lbl_80495AE0, &neg);

    Vec3 v1;
    Vec3 v2;
    fn_800A025C(&v1, &lbl_80495AE0, 1);
    fn_800A025C(&v2, &lbl_80495AE0, 2);

    float frustum[29];
    ViewFrustumUpdate(frustum, &lbl_80495AE0.offset, &v2, &v1,
                      lbl_80495AE0.alpha, lbl_804D85A8, lbl_804D85A8,
                      lbl_80495AE0.colorR);

    void* node = fn_8003A18C(lbl_804959E0[lbl_804D50F8]);
    if (node == 0) {
        return;
    }
    if (ViewFrustumTestOctahedron((const int*)frustum, (const int*)node) == 1) {
        return;
    }

    Vec3 dir2 = lbl_803470F8;
    lbl_80495AE0.offset.x = origin.x + dir2.x * lbl_804D3060;
    lbl_80495AE0.offset.y = origin.y + dir2.y * lbl_804D3060;
    lbl_80495AE0.offset.z = origin.z + dir2.z * lbl_804D3060;
    lbl_803A8F68 = origin;
    Vec3 neg2;
    neg2.x = -dir2.x;
    neg2.y = -dir2.y;
    neg2.z = -dir2.z;
    fn_8009F7A8(&lbl_80495AE0, &neg2);
}

// --- debug overlay for the ghost hint --------------------------------------
const char* StrFindKeySkip(unsigned long key);
void TextBegin(void);
void TextSetMode(int mode);
void TextSetColor(int r, int g, int b, int a);
float StatGet(int key, int a, int b);

// Draws the game-mode line and the hint page / statistic values as debug text.
int GhostHintDebugDraw(void)
{
    int mode = fn_802A1D04();

    switch (mode) {
    case 1:
    case 2:
    case 3: {
        int a = fn_800FD3DC();
        int b = fn_800FD3EC();

        if (fn_802A1D04() == 3 || b != a) {
            ScoreBoardDrawTeams();
        } else {
            ScoreBoardDrawPlayers();
        }
        break;
    }
    case 6: {
        char buf[40];
        float frame = lbl_804D85E4 * fn_8013E988();
        const char* a = StrFindKeySkip(0xb125b682);
        const char* b = StrFindKeySkip(0x2725d0e7);

        sprintf(buf, &lbl_80347110[0x49], b, a);
        TextBegin();
        TextSetMode(2);
        TextSetSize(0x18);
        TextSetColor(0xff, 0xff, 0xff, (int)frame & 0xff);
        TextSetBox(0x40, 0x240, 0x1e, 0x1c2);
        DebugText(buf);

        float stat = StatGet(0xe2d6a9c0, 0, 0);
        const char* c = StrFindKeySkip(0xc09f5ac4);
        sprintf(buf, &lbl_80347110[0x50], c, (int)stat);
        TextSetSize(0x14);
        TextSetBox(0x40, 0x240, 0x1e, 0x168);
        DebugText(buf);
        break;
    }
    case 7: {
        char buf[40];
        float frame = lbl_804D85E4 * fn_8013E988();
        const char* a = StrFindKeySkip(0xb125b682);
        const char* b = StrFindKeySkip(0xee96818a);

        sprintf(buf, &lbl_80347110[0x49], b, a);
        TextBegin();
        TextSetMode(2);
        TextSetSize(0x18);
        TextSetColor(0xff, 0xff, 0xff, (int)frame & 0xff);
        TextSetBox(0x40, 0x240, 0x1e, 0x1c2);
        DebugText(buf);

        StatGet(0xe2d6a9c0, 0, 0);
        char name[40];
        fn_801406B0(name);
        const char* c = StrFindKeySkip(0x3a057d58);
        sprintf(buf, &lbl_80347110[0x58], c, name);
        TextSetSize(0x14);
        TextSetBox(0x40, 0x240, 0x1e, 0x168);
        DebugText(buf);
        break;
    }
    case 8: {
        char buf[40];
        float frame = lbl_804D85E4 * fn_8013E988();
        const char* a = StrFindKeySkip(0xb125b682);
        const char* b = StrFindKeySkip(0xbd92e05b);

        sprintf(buf, &lbl_80347110[0x49], b, a);
        TextBegin();
        TextSetMode(2);
        TextSetSize(0x18);
        TextSetColor(0xff, 0xff, 0xff, (int)frame & 0xff);
        TextSetBox(0x40, 0x240, 0x1e, 0x1c2);
        DebugText(buf);

        float stat = StatGet(0xe2d6a9c0, 0, 0);
        const char* c = StrFindKeySkip(0xc09f5ac4);
        sprintf(buf, &lbl_80347110[0x50], c, (int)stat);
        TextSetSize(0x14);
        TextSetBox(0x40, 0x240, 0x1e, 0x168);
        DebugText(buf);
        break;
    }
    default:
        break;
    }
    return 1;
}

// --- score board debug draws ------------------------------------------------
// Both functions below format the per-team / per-player score lines of the
// debug score board and draw them with the debug text system.  Names are
// descriptive guesses based on the strings they format ("STR_%s_TEAM_WINS")
// and the score-system statistics they read.
extern "C" {
Vec3* fn_800FD424(int id);
const char* fn_800FD334(int id);
const char* fn_800FD44C(int id);
const char* fn_800FD5C4(int id);
unsigned char __upper_map[];
char* strcpy(char* dst, const char* src);
char* strcat(char* dst, const char* src);
}

int PlayerTeam(int id);
void StatRank(int table, int which, int* out, int count, int one);
void TextSetMode(int mode);
void TextSetAlign(int align);
void TextSetColor(int r, int g, int b, int a);

// Lazy name hash used to look up a localised string: characters are folded
// through the upper-case map and accumulated base 31, then folded mod 2^32-1.
static inline unsigned long ScoreBoardNameHash(const char* name)
{
    unsigned long hash = 0;

    if (name != 0) {
        char c;

        while ((c = *name++) != 0) {
            hash *= 31;
            hash += (unsigned char)(c == -1 ? -1 : __upper_map[(unsigned char)c]);
        }
        hash %= 0xffffffffu;
    }
    return hash;
}

void ScoreBoardDrawTeams(void)
{
    int players[8];
    int teams[8];
    char buf[256];
    int teamCount;
    int playerCount;
    int alpha;
    int i;
    int j;

    teamCount = fn_800FD3DC();
    playerCount = fn_800FD3EC();
    alpha = (int)(lbl_804D85E4 * (lbl_804D85A0 - fn_8013E988()));

    StatRank(0xe2d6a9c0, 1, teams, 8, 1);
    StatRank(0xe2d6a9c0, 0, players, 8, 1);

    int first = (int)StatGet(0xe2d6a9c0, 1, teams[0]);
    for (i = 1; i < teamCount; i++) {
        if ((float)first != StatGet(0xe2d6a9c0, 1, teams[i])) {
            break;
        }
    }

    if (i == 1) {
        sprintf(buf, &lbl_80347110[0x60], fn_800FD44C(teams[0]));
        strcpy(buf, StrFindKeySkip(ScoreBoardNameHash(buf)));
    } else {
        sprintf(buf, &lbl_80347110[0x71], StrFindKeySkip(0x4532b0cf));
        for (j = 0; j < i; j++) {
            if (j != 0) {
                strcat(buf, &lbl_80347110[0x75]);
            }
            strcat(buf, fn_800FD5C4(teams[j]));
        }
    }

    TextBegin();
    TextSetMode(2);
    TextSetAlign(4);
    TextSetSize(0x16);
    TextSetColor(0xff, 0xff, 0xff, alpha & 0xff);
    TextSetBox(0x40, 0x240, 0x1e, 0x1cc);
    DebugText(buf);

    int yMin = 0x17c;
    int rowY = 0;
    int colY = 0;

    for (i = 0; i < teamCount; i++) {
        Vec3* pos = fn_800FD424(teams[i]);
        int x;
        int w;
        int score;

        if ((i & 1) == 0) {
            x = 0x40;
            w = 0x130;
            colY = yMin;
            rowY = yMin;
        } else {
            x = 0x150;
            w = 0x240;
            rowY = colY;
        }

        sprintf(buf, &lbl_80347110[0x78], fn_800FD44C(teams[i]));
        sprintf(buf, &lbl_80347110[0x84],
                StrFindKeySkip(ScoreBoardNameHash(buf)));
        TextSetMode(0);
        TextSetSize(0x12);
        TextSetColor((int)(lbl_804D85E4 * pos->x) & 0xff,
                     (int)(lbl_804D85E4 * pos->y) & 0xff,
                     (int)(lbl_804D85E4 * pos->z) & 0xff, alpha & 0xff);
        TextSetBox(x, w, 0x1e, rowY);
        DebugText(buf);

        score = (int)StatGet(0xe2d6a9c0, 1, teams[i]);
        sprintf(buf, &lbl_80347110[0x87], score);
        TextSetColor(0xff, 0xff, 0xff, alpha & 0xff);
        TextSetMode(1);
        TextSetBox(x, w, 0x1e, rowY);
        DebugText(buf);
        rowY -= 0x19;

        for (j = 0; j < playerCount; j++) {
            if (PlayerTeam(players[j]) == teams[i]) {
                sprintf(buf, &lbl_80347110[0x8a],
                        fn_800FD334(players[j]));
                TextSetColor((int)(lbl_804D85E4 * pos->x) & 0xff,
                             (int)(lbl_804D85E4 * pos->y) & 0xff,
                             (int)(lbl_804D85E4 * pos->z) & 0xff,
                             alpha & 0xff);
                TextSetMode(0);
                TextSetSize(0xe);
                TextSetBox(x, w, 0x1e, rowY);
                DebugText(buf);

                score = (int)StatGet(0xe2d6a9c0, 0, players[j]);
                sprintf(buf, &lbl_80347110[0x87], score);
                TextSetColor(0xff, 0xff, 0xff, alpha & 0xff);
                TextSetMode(1);
                TextSetBox(x, w, 0x1e, rowY);
                DebugText(buf);
                rowY -= 0x14;
            }
        }

        if (rowY - 0xc < yMin) {
            yMin = rowY - 0xc;
        }
    }
}

// Draws the per-player score board: one column of player names and scores,
// grouped by the team each player belongs to.
void ScoreBoardDrawPlayers(void)
{
    int players[8];
    char buf[256];
    int teamCount;
    int playerCount;
    int alpha;
    int i;
    int j;
    int rowY;

    teamCount = fn_800FD3DC();
    playerCount = fn_800FD3EC();
    alpha = (int)(lbl_804D85E4 * (lbl_804D85A0 - fn_8013E988()));

    StatRank(0xe2d6a9c0, 0, players, 8, 1);

    int first = (int)StatGet(0xe2d6a9c0, 0, players[0]);
    for (i = 1; i < playerCount; i++) {
        if ((float)first != StatGet(0xe2d6a9c0, 0, players[i])) {
            break;
        }
    }

    if (i == 1) {
        sprintf(buf, StrFindKeySkip(0xcb5e990e), fn_800FD334(players[0]));
    } else {
        sprintf(buf, &lbl_80347110[0x71], StrFindKeySkip(0x4532b0cf));
        for (j = 0; j < i; j++) {
            if (j != 0) {
                strcat(buf, &lbl_80347110[0x75]);
            }
            strcat(buf, fn_800FD334(players[j]));
        }
    }

    TextBegin();
    TextSetMode(2);
    TextSetAlign(4);
    TextSetSize(0x16);
    TextSetColor(0xff, 0xff, 0xff, alpha & 0xff);
    TextSetBox(0x40, 0x240, 0x1e, 0x1cc);
    DebugText(buf);

    rowY = 0x16d;
    for (i = 0; i < teamCount; i++) {
        Vec3* pos = fn_800FD424(PlayerTeam(players[i]));
        int score;

        sprintf(buf, &lbl_80347110[0x84], fn_800FD334(players[i]));
        TextSetMode(0);
        TextSetSize(0x12);
        TextSetColor((int)(lbl_804D85E4 * pos->x) & 0xff,
                     (int)(lbl_804D85E4 * pos->y) & 0xff,
                     (int)(lbl_804D85E4 * pos->z) & 0xff, alpha & 0xff);
        TextSetBox(0xc8, 0x1b8, 0x1e, rowY);
        DebugText(buf);

        score = (int)StatGet(0xe2d6a9c0, 0, players[i]);
        sprintf(buf, &lbl_80347110[0x87], score);
        TextSetMode(1);
        TextSetColor(0xff, 0xff, 0xff, alpha & 0xff);
        TextSetBox(0xc8, 0x1b8, 0x1e, rowY);
        DebugText(buf);
        rowY -= 0x19;
    }
}

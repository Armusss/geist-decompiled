// u_800F9458.cpp (0x800F9458-0x800FA004): the camera table (field of view, activation, easing, shakes, camera messages).
// Split from the old u_800F73C8 (0x800F73C8-0x800FDDE4). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary evidence (split 2026-11-22): own .sdata2 0x804D7490-0x804D74A0 (0x4330 double, -1.0f, 0.0f; the team
// base file has its own copies at 0x804D74B0 / 0x804D74B8 / 0x804D74A0), the camera table .bss 0x8045C770,
// .sbss 0x804D4B9C, .data 0x8039CF58, .rodata 0x80345378 and .sdata 0x804D2D70.


#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

struct StreamNode;
struct EffectKind;
struct EffectInst;
struct Emitter;
struct Vector;
struct CamObj;
struct Shake;
struct GameType;
struct Minigame;

// Functions called from other units under their placeholder names.
extern "C" float fn_800F7920(u32 row, u32 key);
extern "C" int fn_800F7994(u32 row, u32 key);
extern "C" void fn_800F7A90(u32 row);
extern "C" int fn_800F7AF0(u32 emitter, u32 row);
extern "C" int fn_800F7C64(u32 emitter, int row);
extern "C" int fn_800F7D28(u32 emitter, u32 row);
extern "C" int fn_800F80DC(u32 handle);
extern "C" int fn_800F8130(u32 handle, int i);
extern "C" int fn_800F81EC(u32 handle);
extern "C" int fn_800F825C(u32 handle, u32 row, int n);
extern "C" void fn_800F86A0(u32 handle);
extern "C" u32 fn_800F8A30(int user);
extern "C" u32 fn_800F8AF4(u32 row);
extern "C" void fn_800F8ED0(void);
extern "C" void fn_800F9468(int on);
extern "C" int fn_800F94C8(u32 handle);
extern "C" void fn_800F94F8(u32 handle, short v);
extern "C" void fn_800F9578(u32 handle, int v);
extern "C" float fn_800F95E4(u32 handle);
extern "C" int fn_800F9614(u32 handle);
extern "C" void fn_800F96E0(u32 handle);
extern "C" u32 fn_800F9724(int owner, int id);
extern "C" u32 fn_800FC3B8(int id);
extern "C" int fn_800FC764(u32 who, int arg);
extern "C" char* fn_800FD334(int i);
extern "C" int fn_800FD3DC(void);
extern "C" int fn_800FD3EC(void);
extern "C" char* fn_800FD3FC(int i);
extern "C" char* fn_800FD424(int i);
extern "C" const char* fn_800FD44C(int c);
extern "C" const char* fn_800FD5C4(int c);
extern "C" int fn_800FD6EC(int i);
extern "C" void fn_800FD924(int arg);
extern "C" void fn_800FD998(void);
extern "C" void fn_800FDA10(void);
extern "C" void fn_800FDA90(void);
extern "C" void fn_800FDC48(void);
extern "C" void fn_800FDC78(void);

extern float gZeroF;            // 0x804D7488

extern "C" int SendMsg(int to, int msg, void* data);

static inline void SetVarInt(int target, u32 hash, int value) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    SendMsg(target, 0x41D, &m);
}

static inline int GetVarInt(int target, u32 hash) {
    QueryMsg m;
    *(int*)&m.result = 0;
    m.hash = hash;
    m.unk8 = 0;
    SendMsg(target, 0x41E, &m);
    return *(int*)&m.result;
}

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

// ---- cameras -----------------------------------------------------------------------------
// 16 camera objects (0x124 bytes each) behind handles 0x16xxxxxx; the objects wrap the
// camera controller (+0x60) and an optional shake controller (+0x64).

struct CamTriple {
    int v[3];
};

struct CamObj {
    u8 pad00[0x30];
    int f30;                // 0x30
    float fov;              // 0x34
    u8 pad38[0x40 - 0x38];
    float f40;              // 0x40
    float f44;              // 0x44
    float f48;              // 0x48
    CamTriple t4C;          // 0x4C
    u8 pad58[0xC0 - 0x58];
    int ctl;                // 0xC0
    int ctl2;               // 0xC4
    int unkC8;              // 0xC8
    u8 padCC[0xF8 - 0xCC];
    int ownerId;            // 0xF8
    int flagA;              // 0xFC
    int flagB;              // 0x100
    int unk104;             // 0x104
    u8 pad108[0x124 - 0x108];
};

extern CamObj gCams[16];        // 0x8045C770
extern int gCamLetterbox;       // 0x804D4B98
extern int gCamCount;           // 0x804D4B9C
extern float gCamNeg1;          // 0x804D7498
extern float gCamZero2;         // 0x804D749C

extern "C" {
int fn_802A1D04(void);
void fn_802A1EBC(int mode);
void fn_800C6E88(int handle);  // empty in retail; CameraSetFov passes the masked handle in r3
void fn_800C6560(int ctl);
void fn_800C6760(int ctl);
void fn_800C6834(int ctl);
int fn_800C6AE8(void* obj, void* a, void* b, int c);
void fn_80026660(int msg, void* fn, int arg);
}

void CamObjUpdate(float dt, CamObj* c);
void CamObjEase(CamObj* c, int a, int b, int d, int e);
int CamMsg(u32 handle, int msg, int* data);

static inline int CamIdx(u32 handle) {
    return (int)(handle & 0xFFFFFF);
}

// Base of the camera array plus 8 (the first object's second field).
void* CameraTable(void) {
    return (char*)gCams + 0xC8;
}

// Turns the camera letterbox on or off.
void fn_800F9468(int on) {
    if (fn_802A1D04() == 0) {
        if (on != 0) {
            gCamLetterbox = 1;
            fn_802A1EBC(9);
        } else {
            gCamLetterbox = 0;
            fn_802A1EBC(0);
        }
    }
}

// Controller handle of camera `handle`, or -1.
int fn_800F94C8(u32 handle) {
    if (CamIdx(handle) >= 0 && CamIdx(handle) < 16) {
        return gCams[handle & 0xFFFFFF].ctl;
    }
    return -1;
}

void fn_800F94F8(u32 handle, short v) {
    if (CamIdx(handle) >= 0 && CamIdx(handle) < 16) {
        gCams[handle & 0xFFFFFF].flagB = v;
    }
}

// Starts an ease on a camera object (does nothing when one is running unless `force`).
void CamObjEase(CamObj* c, int a, int b, int d, int force) {
    if (c == NULL || b == 0) {
        return;
    }
    if (c->unk104 != 0 && force == 0) {
        return;
    }
    c->unk104 = a;
    *(int*)((char*)c + 0x108) = b;
    *(int*)((char*)c + 0x10C) = d << 12;
    *(int*)((char*)c + 0x110) = *(int*)((char*)c + 0x10C) / b;
    *(int*)((char*)c + 0x114) = 0;
    *(int*)((char*)c + 0x118) = 0;
}

void fn_800F9578(u32 handle, int v) {
    if (CamIdx(handle) >= 0 && CamIdx(handle) < 16) {
        gCams[handle & 0xFFFFFF].flagA = v;
    }
}

// Sets the field of view of camera `handle`.
void CameraSetFov(int handle, float fov) {
    handle &= 0xFFFFFF;
    if (handle >= 0 && handle < 16) {
        gCams[handle].fov = fov;
    }
    fn_800C6E88(handle);
}

float fn_800F95E4(u32 handle) {
    if (CamIdx(handle) >= 0 && CamIdx(handle) < 16) {
        return gCams[handle & 0xFFFFFF].fov;
    }
    return -1.0f;
}

int fn_800F9614(u32 handle) {
    int r;
    if (CamIdx(handle) >= 0 && CamIdx(handle) < 16) {
        r = (int)gCams[handle & 0xFFFFFF].fov;
    } else {
        r = -1;
    }
    return r;
}

void CameraActivate(u32 handle) {
    if (CamIdx(handle) >= 0 && CamIdx(handle) < 16) {
        fn_800C6560(gCams[handle & 0xFFFFFF].ctl);
    }
}

void CameraDeactivate(u32 handle) {
    if (CamIdx(handle) >= 0 && CamIdx(handle) < 16) {
        fn_800C6760(gCams[handle & 0xFFFFFF].ctl);
    }
}

void fn_800F96E0(u32 handle) {
    if (CamIdx(handle) >= 0 && CamIdx(handle) < 16) {
        fn_800C6834(gCams[handle & 0xFFFFFF].ctl);
    }
}

// Creates a camera object and returns its handle (-1 when all 16 are used).
u32 fn_800F9724(int owner, int id) {
    u32 idx = gCamCount;
    if ((int)idx < 0x10) {
        CamObj* c = &gCams[idx];
        gCamCount++;
        c->ctl = fn_800C6AE8(c, (void*)CamObjUpdate, (void*)CamObjEase, id);
        if (fn_802A1D04() == 0) {
            c->ctl2 = fn_800C6AE8((char*)c + 0x60, NULL, NULL, id + 1);
            fn_800C6834(c->ctl2);
        } else {
            c->ctl2 = -1;
        }
        c->ownerId = owner;
        c->flagA = 0;
        c->flagB = 0;
        c->unk104 = 0;
        CamObjUpdate(0.0f, c);
        return idx & 0xFFFFFF | 0x16000000;
    }
    return 0xFFFFFFFF;
}

// Registers the camera message handler (type 0x16) and clears the count.
void CameraSystemInit(void) {
    fn_80026660(0x16, (void*)CamMsg, 0);
    gCamCount = 0;
}

struct IMtx {
    int m[12];
};

extern "C" {
int fn_800A1624(int angle);
int fn_800A1688(int angle);
void fn_8009F76C(int* m);
void MtxMulQ12(int* out, int* a, int* b);                   // 0x8009F258
void fn_8009F5B0(int* m);
void fn_8009F50C(int* out, int* in);
int rand(void);
void fn_8009F63C(int* m, int* actor);
void fn_8009F44C(int* out, int* m, int* in);
void PlacementToMatrix(int* m, int* p);                     // 0x8009EF04
void ActorGetMatrix(int* m, int* actor);
void MakeVec(int* v, int x, int y, int z);                  // 0x8016D430
void fn_8009F028(int* out, int* v);
void fn_8009FC6C(int* out, int* a, int* b);
void MtxSetEuler(int* m, float* e);                         // 0x8009FAC0
}

// Shake record (inside the camera object): +8 amplitude, +0x10 flags, +0x14 phase.
struct Shake {
    u8 pad00[8];
    int amp;                // 0x08
    u8 pad0C[4];
    u32 flags;              // 0x10
    int phase;              // 0x14
};

// Sine-wave roll shake applied to the camera matrix.
// A 3x4 fixed-point (Q12) matrix, copied as a whole.
struct RotQ12 {
    int m[9];
};
struct PosQ12 {
    int x, y, z;
};
struct MtxQ12 {
    RotQ12 rot;
    PosQ12 pos;
};

void ShakeRoll(Shake* s, int* m) {
    int div;
    int sinA;
    int cosA;
    int sinB;
    MtxQ12 rot;
    MtxQ12 tmp;
    sinA = fn_800A1624((s->phase & 0x1FF) << 3) * 4;
    cosA = fn_800A1688((s->phase & 0x1FF) << 3) * 2;
    sinB = fn_800A1624(s->phase * 4 + 0x800 & 0xFFF) * 4;
    div = 0x101 - s->amp / 0x1000;
    fn_8009F76C(rot.rot.m);
    rot.rot.m[0] = 0x1000 - sinA / div;
    rot.rot.m[8] = sinB / div + 0x1000;
    rot.rot.m[4] = cosA / div + 0x1000;
    tmp = *(MtxQ12*)m;
    MtxMulQ12(m, rot.rot.m, tmp.rot.m);
}

// Slides the camera along its view axis by the shake amplitude.
void ShakeSlide(Shake* s, int* pos) {
    int v[3];
    int out[3];
    if ((s->flags & 1) != 0) {
        v[0] = -s->amp / 0x8000;
    } else {
        v[0] = s->amp / 0x8000;
    }
    v[1] = s->amp / 0x1000;
    v[2] = -s->amp / 0x2000;
    fn_8009F5B0(pos);
    fn_8009F50C(out, v);
    pos[9] += out[0];
    pos[10] += out[1];
    pos[11] += out[2];
}

// Random jitter shake: offsets the position by +-amplitude on x and z.
void ShakeJitter(Shake* s, int* pos) {
    int v[3];
    int out[3];
    int mtx[12];
    int amp;
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
    amp = s->amp / 0x1000;
    if (amp > 0) {
        v[0] = rand() % 2 ? amp : -amp;
        if (rand() % 2) {
            v[2] = amp;
        } else {
            v[2] = -amp;
        }
    }
    fn_8009F63C(mtx, pos);
    fn_8009F44C(out, mtx, v);
    pos[9] += out[0];
    pos[10] += out[1];
    pos[11] += out[2];
}

extern void (*gShakeFns[])(void* shake, int* m);    // 0x8039CF54 (indexed by shake kind)
extern float gShakeAngle;       // 0x804D7490 (double bias)
extern int gCamTilt;            // 0x804D2D70
extern float gTiltRow[2];       // 0x80345378

// Per-frame update of a camera object: copies the controller's matrix, applies the offset,
// shake and roll, and mirrors it for the split view.
void CamObjUpdate(float dt, CamObj* c) {
    int* cm = (int*)c;
    int* src = (int*)cm[0x3E];
    float* off;
    int i;
    for (i = 0; i < 12; i++) {
        cm[i] = src[i];
    }
    off = (float*)cm[0x3F];
    if (off != NULL) {
        ((float*)cm)[9] += off[0];
        ((float*)cm)[10] += off[1];
        ((float*)cm)[11] += off[2];
    }
    if (cm[0x41] != 0) {
        int m[12];
        PlacementToMatrix(m, cm);
        gShakeFns[cm[0x41]](cm + 0x41, m);
        {
            int step = (int)dt;
            cm[0x42] -= step;
            if (cm[0x42] < 0) {
                cm[0x41] = 0;
            } else {
                cm[0x43] -= cm[0x44] * step;
                if (cm[0x43] < 0) {
                    cm[0x41] = 0;
                }
                cm[0x45] += 1;
                cm[0x46] += step;
            }
        }
        ActorGetMatrix(cm, m);
    }
    if (cm[0x40] != 0) {
        int v[3];
        int a[12];
        int b[12];
        v[1] = 0;
        v[2] = 0;
        v[0] = cm[0x40];
        MakeVec(v, 0, 0, 0);
        fn_8009F028(a, v);
        ActorGetMatrix(b, a);
        fn_8009FC6C(cm, b, cm);
    }
    for (i = 0; i < 12; i++) {
        cm[0x32 + i] = cm[i];
    }
    if (gCamLetterbox != 0) {
        float tilt[3];
        float rot[12];
        tilt[0] = gTiltRow[0];
        tilt[1] = gTiltRow[1];
        for (i = 0; i < 24; i++) {
            cm[0x18 + i] = cm[i];
        }
        tilt[2] = (float)(-gCamTilt);
        MtxSetEuler((int*)rot, tilt);
        fn_8009FC6C(cm + 0x18, (int*)rot, cm + 0x32);
        tilt[2] = (float)gCamTilt;
        MtxSetEuler((int*)rot, tilt);
        fn_8009FC6C(cm, (int*)rot, cm + 0x32);
    }
}

// Camera message handler (type 0x16): message 0x1C01 copies the requested fields out of the
// camera object.
// Reply block of camera message 0x1C01: `mask` selects the fields to fill.
struct CamQuery {
    int unk0;              // 0x00
    u32 mask;              // 0x04
    int f30;               // 0x08
    float fov;             // 0x0C
    float f40;             // 0x10
    float f44;             // 0x14
    float f48;             // 0x18
    CamTriple t4C;         // 0x1C
    int ctl;               // 0x28
};

int CamMsg(u32 handle, int msg, int* data) {
    CamQuery* q = (CamQuery*)data;
    switch (msg) {
    case 0x1C01:
        if ((q->mask & 1) != 0) {
            q->f30 = gCams[handle & 0xFFFFFF].f30;
        }
        if ((q->mask & 2) != 0) {
            q->fov = gCams[handle & 0xFFFFFF].fov;
        }
        if ((q->mask & 4) != 0) {
            q->f40 = gCams[handle & 0xFFFFFF].f40;
        }
        if ((q->mask & 8) != 0) {
            q->f44 = gCams[handle & 0xFFFFFF].f44;
        }
        if ((q->mask & 0x10) != 0) {
            q->f48 = gCams[handle & 0xFFFFFF].f48;
        }
        if ((q->mask & 0x20) != 0) {
            q->t4C.v[0] = gCams[handle & 0xFFFFFF].t4C.v[0];
            q->t4C.v[1] = gCams[handle & 0xFFFFFF].t4C.v[1];
            q->t4C.v[2] = gCams[handle & 0xFFFFFF].t4C.v[2];
        }
        if ((q->mask & 0x40) != 0) {
            q->ctl = gCams[handle & 0xFFFFFF].ctl;
        }
        return 1;
    }
    return 0;
}

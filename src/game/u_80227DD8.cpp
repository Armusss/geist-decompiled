// u_80227DD8.cpp (0x80227DD8-0x8022A180), split from the old u_80226DE8 range: a texture-plotting overlay
// (TurfPlotPixel, GXInitTexObj, a 0x8049EF60 texture block) and the actor list at .sbss 0x804D5418.
// Boundary evidence:
// - .sdata2: own pool 0x804D9678-0x804D96F0 (0.0f at 0x804D967C, int-to-float doubles 0x804D96B0/0x804D96B8);
//   the previous file has its own 0.0f at 0x804D9660 and the next its own at 0x804D96F0. fn_80227DD8 is the
//   first reader, fn_8022A170 the last.
// - .rodata 0x80349CB8-0x80349CC8 (a 16-byte zero initialiser read by fn_80227DD8), .sbss 0x804D5418-0x804D542C.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include "game/types.h"

extern int lbl_804D541C;
extern int lbl_804D5420;
extern int lbl_804D5424;
extern int lbl_804D5428;

// TODO (not drafted yet, 0x800 bytes at 0x80227DD8)
// fn_80227DD8
//@@802285D8
extern "C" void fn_80227DD8(int a, int b, int c, int d, int e, float f, float x, float y, float z, int g);
extern "C" void fn_802285D8(int a, int b, int c, int d, int e, float f);
void fn_802285D8(int a, int b, int c, int d, int e, float f) {
    fn_80227DD8(a, b, c, d, e, f, 0.0f, 0.0f, 0.0f, -1);
}
//@@end
// TODO (not drafted yet, 0x80 bytes at 0x80228608)
// fn_80228608
// TODO (not drafted yet, 0x384 bytes at 0x80228688)
// fn_80228688
// TODO (not drafted yet, 0xC0 bytes at 0x80228A0C)
// fn_80228A0C
//@@80228ACC
extern "C" void fn_80228ACC(void);
void fn_80228ACC(void) {
    lbl_804D541C = 0;
    lbl_804D5420 = 0;
    lbl_804D5424 = 0;
    lbl_804D5428 = 0;
}
//@@end
// TODO (not drafted yet, 0x228 bytes at 0x80228AE4)
// fn_80228AE4
// TODO (not drafted yet, 0x208 bytes at 0x80228D0C)
// fn_80228D0C
//@@80228F14
struct PlotActor {
    u8 pad00[0x38];
    u32 handle;    // 0x38
    u8 pad3C[0x50 - 0x3C];
    u32 flags;     // 0x50
};

extern PlotActor** lbl_804D5418;  // 64 slots, indexed by the low 24 bits of the handle

extern "C" int fn_80228F14(int mode, PlotActor** pp);
int fn_80228F14(int mode, PlotActor** pp) {
    int id = (*pp)->handle & 0xFFFFFF;
    switch (mode) {
    case 0:
        if (id >= 0 && id < 64) {
            lbl_804D5418[id]->flags |= 1;
            return 1;
        }
    }
    return 0;
}
//@@end
// TODO (not drafted yet, 0x47C bytes at 0x80228F64)
// fn_80228F64
//@@802293E0
extern "C" void fn_802293E0(Vec3* v);
void fn_802293E0(Vec3* v) {
    v->z = v->y = v->x = 0.0f;
}
//@@end
// TODO (not drafted yet, 0x444 bytes at 0x802293F4)
// fn_802293F4
// TODO (not drafted yet, 0x30C bytes at 0x80229838)
// fn_80229838
// TODO (not drafted yet, 0x610 bytes at 0x80229B44)
// fn_80229B44
//@@8022A154
extern "C" void fn_8022A154(Quat* q);
void fn_8022A154(Quat* q) {
    q->x = q->y = q->z = 0.0f;
    q->w = 1.0f;
}
//@@end
//@@8022A170
struct Vec2 {
    float x, y;
};

extern "C" void fn_8022A170(Vec2* v);
void fn_8022A170(Vec2* v) {
    v->y = v->x = 0.0f;
}
//@@end

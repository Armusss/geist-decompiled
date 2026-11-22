// u_801BE008.cpp (0x801BE008-0x801BEC4C): a second type registry (5-word entries) plus a timing helper; its static initializer is in .ctors.
// Split from the old u_801AE50C (0x801AE50C-0x801BEC4C). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary evidence (split 2026-11-22): own .sdata2 0x804D8A00-0x804D8A20 (0x4330 double at 0x804D8A08, 0.0f at
// 0x804D8A18), .sdata 0x804D3208-0x804D3218, .sbss 0x804D5188, .rodata 0x803486A8-0x803486C8 (an index table and the
// empty params constant), .data 0x803AB9D8-0x803ABBA8, and the .ctors entry 0x8034122C (fn_801BEC38, the static
// initializer).

#include "game/string.h"

typedef unsigned char u8;
typedef unsigned long u32;

// TODO (not drafted yet, 0x4B0 bytes at 0x801BE008): game logic
extern "C" void fn_801BE008();

// The object a registered type runs on (descriptive guess; only the fields used here).
struct RegTypeInfo {
    u8 pad00[0x10];
    u32 type;            // 0x10: registry hash of the object's type
    int anim14;          // 0x14
    u8 pad18[4];
    int arg1C;           // 0x1C: passed to the type's enter callback
};

struct RegObj {
    u8 pad00[0xC4];
    RegTypeInfo* info;   // 0xC4
    u8 padC8[0xD4 - 0xC8];
    int animCtl;         // 0xD4
    u8 padD8[0xF4 - 0xD8];
    float blend;         // 0xF4
    float timer;         // 0xF8
    u8 padFC[0x110 - 0xFC];
    int unk110;          // 0x110
    u8 pad114[0x124 - 0x114];
    u32 update;          // 0x124: the current type's update word
    u8 pad128[4];
    u32 type;            // 0x12C: current registered type (-1: none)
    u8 pad130[4];
    int unk134;          // 0x134
    float dt;            // 0x138
    u8 pad13C[0x270 - 0x13C];
    u8 conditions[0x18]; // 0x270
};

// One registered type (5 words): hash, enter/leave callbacks, update word and name.
struct RegType {
    u32 hash;                              // 0x00
    void (*enter)(int arg, RegObj* o);     // 0x04
    void (*leave)(RegObj* o);              // 0x08
    u32 update;                            // 0x0C
    const char* name;                      // 0x10
};

extern RegType lbl_803AB9D8[];   // the type table
extern int lbl_804D3214;         // number of entries (23)

extern float lbl_804D3208;       // blend-in limit
extern float lbl_804D320C;
extern float lbl_804D3210;       // timer threshold
extern float lbl_804D5188;       // blend rate

extern "C" int fn_8008EA9C(int ctl, int layer);
extern "C" void fn_8008F4F0(int ctl, int layer, float w);
extern "C" void fn_80090390(int ctl, int layer);
extern "C" int fn_800A1BC8(int a, int b, u32 hash, int c, int d);
extern "C" int fn_8008E7D0(int anim);
extern "C" void fn_80090974(int ctl, int layer, int anim, int c, int d, int e, float w);
void ConditionSetBit(void* object, int bit, int value);

static inline float Min(float a, float b) {
    return a < b ? a : b;
}

static inline int RegFind(u32 hash) {
    int i;
    for (i = 0; i < lbl_804D3214; i++) {
        if (hash == lbl_803AB9D8[i].hash) {
            return i;
        }
    }
    return -1;
}

static inline void RegLeave(RegObj* o) {
    if (o != 0) {
        int i = RegFind(o->type);
        if (i != -1) {
            if (lbl_803AB9D8[i].leave) {
                lbl_803AB9D8[i].leave(o);
            }
            o->unk110 = 0;
            o->update = 0;
            o->unk134 = 0;
            o->type = -1;
        } else {
            o->type = -1;
        }
    }
}

static inline void RegEnter(u32 hash, int arg, RegObj* o) {
    if (o != 0) {
        int i = RegFind(hash);
        if (i != -1) {
            RegLeave(o);
            ConditionSetBit(o->conditions, 1, 0);
            o->unk110 = 0;
            o->update = lbl_803AB9D8[i].update;
            o->type = hash;
            o->unk134 = 0;
            if (lbl_803AB9D8[i].enter) {
                lbl_803AB9D8[i].enter(arg, o);
            }
        }
    }
}

// Blends layer 1 of the animation in (state 1) or out (state 2) and advances the timer.
extern "C" void fn_801BE4B8(RegObj* o) {
    o->timer += 0.001 * o->dt;
    if (fn_8008EA9C(o->animCtl, 1) == 1 && o->blend < lbl_804D3208) {
        o->blend += 0.001 * o->dt * lbl_804D5188;
        o->blend = Min(o->blend, lbl_804D3208);
        fn_8008F4F0(o->animCtl, 1, o->blend);
    } else if (fn_8008EA9C(o->animCtl, 1) == 2) {
        o->blend += 0.001 * o->dt * -lbl_804D5188;
        if (o->blend <= 0.0f) {
            o->blend = 0.0f;
            fn_80090390(o->animCtl, 1);
        } else {
            fn_8008F4F0(o->animCtl, 1, o->blend);
        }
    }
}

// Starts the idle animation on layer 1 once the timer has run out.
extern "C" void fn_801BE5D4(RegObj* o) {
    int anim;

    if (o == 0) {
        return;
    }
    if (o->timer < lbl_804D3210) {
        return;
    }
    anim = fn_800A1BC8(o->info->anim14, o->info->arg1C, 0x806E6EB5, 0, 0);
    if (fn_8008E7D0(anim) != 0) {
        ConditionSetBit(o->conditions, 0x1C, 0);
        fn_8008F4F0(o->animCtl, 1, o->blend);
        fn_80090974(o->animCtl, 1, anim, 0x10, 0, 0, 1.0f);
        o->timer = 0.0f;
    }
}

// Switches an object to the type named in its type info.
extern "C" void fn_801BE690(RegObj* o) {
    if (o != 0) {
        RegEnter(o->info->type, o->info->arg1C, o);
    }
}

// Switches an object to a registered type.
extern "C" void fn_801BE7FC(u32 hash, int arg, RegObj* o) {
    RegEnter(hash, arg, o);
}

// The name of a registered type ("" if unknown).
extern "C" String fn_801BE974(u32 hash) {
    int i = RegFind(hash);
    if (i != -1) {
        return String(lbl_803AB9D8[i].name);
    }
    return String("");
}

// Leaves the object's current type.
extern "C" void fn_801BE9FC(RegObj* o) {
    RegLeave(o);
}

// Switches type without the condition reset and bookkeeping of fn_801BE7FC.
extern "C" void fn_801BEABC(u32 hash, int arg, RegObj* o) {
    int i;

    if (o == 0) {
        return;
    }
    if (o->type != 0xFFFFFFFF) {
        RegLeave(o);
    }
    i = RegFind(hash);
    if (i != -1) {
        if (lbl_803AB9D8[i].enter) {
            lbl_803AB9D8[i].enter(arg, o);
        }
        o->update = lbl_803AB9D8[i].update;
        o->type = hash;
    }
}

float lbl_804D5188 = lbl_804D3208 / lbl_804D320C;

// u_80037230.cpp (0x80037230-0x8003B98C), split from ModInfo.cpp.
// The model database (lbl_804D4300): node lookup by name hash, bone counts/positions, draw slots, animation.
// Boundaries: fn_80037230 is the first function using the model database .sbss 0x804D4300-0x804D4320, which
// nothing before it touches; its .sdata2 pool 0x804D61D0-0x804D61F0 has its own FLT_MAX (0x804D61D0; the
// previous file's is 0x804D6184), 1.0f, 0.0f and int-to-float double. .bss 0x803DEA58-0x803F9A00
// (draw slots 0x803E4998) and .rodata 0x80342490 are touched only from this range.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef float f32;

// Model database (lbl_804D4300) and its 0x48-byte node records; layouts are partial.
struct ModelNode {
    /*0x00*/ u8 unk00[0x18 - 0x00];
    /*0x18*/ u8 field18[0x34 - 0x18];
    /*0x34*/ u16 field34;
    /*0x36*/ u16 field36;
    /*0x38*/ u8 unk38[0x40 - 0x38];
    /*0x40*/ u16 boneCount;
    /*0x42*/ u16 field42;
    /*0x44*/ u8 unk44[0x46 - 0x44];
    /*0x46*/ u16 positionIndex;
};

struct ModelDb {
    /*0x00*/ u8 unk00[0x04 - 0x00];
    /*0x04*/ int count;
    /*0x08*/ u8 unk08[0x28 - 0x08];
    /*0x28*/ ModelNode* nodes;
    /*0x2C*/ u8 unk2C[0x34 - 0x2C];
    /*0x34*/ Vec3* positions;
    /*0x38*/ u8 unk38[0x44 - 0x38];
    /*0x44*/ s16* array44;
    /*0x48*/ s16* array48;
};

// Per-model draw slot (lbl_803E4998, 0x70 bytes each, 0x300 slots).
struct ModelSlot {
    /*0x00*/ u8 unk00[4];
    /*0x04*/ int field04;
    /*0x08*/ u8 color[4];
    /*0x0C*/ u8 unk0C[0x70 - 0x0C];
};

extern "C" {
extern ModelDb* lbl_804D4300;
extern ModelSlot lbl_803E4998[];
}

// 0x80037B6C (called from other units under this placeholder name)
extern "C" s16* fn_80037B6C(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return lbl_804D4300->array48 + lbl_804D4300->nodes[idx].field42;
    return 0;
}

// 0x80037BAC (called from another unit under this placeholder name; ModelArray44)
extern "C" s16* fn_80037BAC(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return lbl_804D4300->array44 + lbl_804D4300->nodes[idx].field36;
    return 0;
}

// 0x80037BEC
int ModelBoneCount(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return *(u16*)((u8*)lbl_804D4300->nodes + idx * 0x48 + 0x40);
    return -1;
}

// 0x80037C20 (called from other units under this placeholder name)
extern "C" int fn_80037C20(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return *(u16*)((u8*)lbl_804D4300->nodes + idx * 0x48 + 0x34);
    return -1;
}

// 0x800381F8 (called from other units under this placeholder name)
extern "C" void fn_800381F8(u32 handle, int value, u8 r, u8 g, u8 b, u8 a) {
    int idx = handle >> 16;
    if (idx > 0 && idx < 0x300) {
        lbl_803E4998[idx].field04 = value;
        lbl_803E4998[idx].color[0] = r;
        lbl_803E4998[idx].color[1] = g;
        lbl_803E4998[idx].color[2] = b;
        lbl_803E4998[idx].color[3] = a;
    }
}

// 0x800390D8 (called from other units under this placeholder name)
extern "C" int fn_800390D8(int handle, int index) {
    if (index < 0)
        return -1;
    return (handle & 0xFFFF0000) | (index & 0xFFFF);
}

// 0x8003A14C
Vec3* ModelBonePositions(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return &lbl_804D4300->positions[*(u16*)((u8*)lbl_804D4300->nodes + idx * 0x48 + 0x46)];
    return 0;
}

// 0x8003A18C (called from other units under this placeholder name)
extern "C" void* fn_8003A18C(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return lbl_804D4300->nodes[idx].field18;
    return 0;
}

// 0x8003A1C0 (called from other units under this placeholder name)
extern "C" ModelNode* fn_8003A1C0(int model) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return (ModelNode*)((u8*)lbl_804D4300->nodes + idx * 0x48);
    return 0;
}

// ---- Functions not drafted yet ----
// TODO (not drafted yet, 0x234 bytes at 0x80037230)
extern "C" void fn_80037230();
// TODO (not drafted yet, 0x28C bytes at 0x80037464): find a node of a model by name hash
int ModelNodeFind(int model, int hash);
// TODO (not drafted yet, 0x240 bytes at 0x800376F0)
extern "C" void fn_800376F0();
// TODO (not drafted yet, 0x23C bytes at 0x80037930)
extern "C" void fn_80037930();
// TODO (not drafted yet, 0x26C bytes at 0x80037C54)
extern "C" void fn_80037C54();
// TODO (not drafted yet, 0x2DC bytes at 0x80037EC0)
extern "C" void fn_80037EC0();
// TODO (not drafted yet, 0x5C bytes at 0x8003819C)
extern "C" void fn_8003819C();
// TODO (not drafted yet, 0x244 bytes at 0x80038230)
extern "C" void fn_80038230();
// TODO (not drafted yet, 0x430 bytes at 0x80038474)
extern "C" void fn_80038474();
// TODO (not drafted yet, 0x244 bytes at 0x800388A4)
extern "C" void fn_800388A4();
// TODO (not drafted yet, 0x5F0 bytes at 0x80038AE8): set up a model's node
int ModelNodeSetup(int a, int b, int c, int d);
// TODO (not drafted yet, 0xE8 bytes at 0x800390F4)
extern "C" void fn_800390F4();
// TODO (not drafted yet, 0x29C bytes at 0x800391DC)
extern "C" void fn_800391DC();
// TODO (not drafted yet, 0x1C0 bytes at 0x80039478)
extern "C" void fn_80039478();
// TODO (not drafted yet, 0x1C0 bytes at 0x80039638)
extern "C" void fn_80039638();
// TODO (not drafted yet, 0xB0 bytes at 0x800397F8)
extern "C" void fn_800397F8();
// TODO (not drafted yet, 0xB0 bytes at 0x800398A8)
extern "C" void fn_800398A8();
// TODO (not drafted yet, 0x7F4 bytes at 0x80039958)
extern "C" void fn_80039958();
// TODO (not drafted yet, 0x200 bytes at 0x8003A1F0)
extern "C" void fn_8003A1F0();
// TODO (not drafted yet, 0x27C bytes at 0x8003A3F0)
extern "C" void fn_8003A3F0();
// TODO (not drafted yet, 0x430 bytes at 0x8003A66C): set a model's matrix
void ModelSetMatrix(int model, Mtx34* m, char* name);
// TODO (not drafted yet, 0x338 bytes at 0x8003AA9C): animate a model
void ModelAnimate(int model, char* name, int frame);
// TODO (not drafted yet, 0x80 bytes at 0x8003ADD4)
extern "C" void fn_8003ADD4();
// TODO (not drafted yet, 0x4B8 bytes at 0x8003AE54)
extern "C" void fn_8003AE54();
// TODO (not drafted yet, 0xF4 bytes at 0x8003B30C)
extern "C" void fn_8003B30C();
// TODO (not drafted yet, 0xA4 bytes at 0x8003B400)
extern "C" void fn_8003B400();
// TODO (not drafted yet, 0x94 bytes at 0x8003B4A4)
extern "C" void fn_8003B4A4();
// TODO (not drafted yet, 0x454 bytes at 0x8003B538)
extern "C" void fn_8003B538();

// ModInfo.cpp (0x80037230-0x8003B98C): the real ModInfo.cpp. Its MemAlloc/MemFree calls pass __FILE__
// "ModInfo.cpp" (.rodata 0x80342490) with lines 348-394 (fn_8003B400, fn_8003B30C), so this end of the old
// ModInfo range is the file of that name; the code before 0x80037230 is u_800265C8.cpp and u_8002A274.cpp.
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
    /*0x38*/ u8 unk38[0x3E - 0x38];
    /*0x3E*/ u16 field3E;
    /*0x40*/ u16 boneCount;
    /*0x42*/ u16 field42;
    /*0x44*/ u8 unk44[0x46 - 0x44];
    /*0x46*/ u16 positionIndex;
};

struct ModelDb {
    /*0x00*/ u8 unk00[0x04 - 0x00];
    /*0x04*/ int count;
    /*0x08*/ u8 unk08[0x0C - 0x08];
    /*0x0C*/ int field0C;
    /*0x10*/ u8 unk10[0x18 - 0x10];
    /*0x18*/ int field18;
    /*0x1C*/ u8 unk1C[0x28 - 0x1C];
    /*0x28*/ ModelNode* nodes;
    /*0x2C*/ u8 unk2C[0x30 - 0x2C];
    /*0x30*/ Vec3* array30;
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
    /*0x0C*/ u8 unk0C[0x6C - 0x0C];
    /*0x6C*/ void* data;
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    u32 h;
    if (s == 0) {
        h = 0;
    } else {
        int i;
        h = 0;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
    }
    return h;
}

extern "C" {
extern ModelDb* lbl_804D4300;
extern ModelSlot lbl_803E4998[];
extern u8 lbl_803E19F8[];
extern u32 lbl_803F9998[];
extern void* gModInfoBits0;
extern int gModInfoBits0Size;
extern int lbl_804D4310;
extern void* gModInfoBits1;
extern int gModInfoBits1Size;
extern int lbl_804D431C;
void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);
void* memset(void* dst, int c, unsigned long n);
void VFree(void* p);
int fn_8003B538(void* data, int size, void* ctx);
}

typedef int (*FileHandlerFn)(void*, int, void*);
void FileAddHandler(int type, FileHandlerFn fn, void* ctx);

extern "C" {
int AssetFind(u32 group, u32 name);
void fn_8003AE54(int count, Vec3* v, void* a, void* b, void* c, void* out);
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

// 0x8003819C: read the colour of a draw slot; 0 if the slot is unused
extern "C" int fn_8003819C(u32 handle, u8* color) {
    int idx = handle >> 16;
    if (idx > 0 && idx < 0x300) {
        if (lbl_803E4998[idx].field04 != 0) {
            color[0] = lbl_803E4998[idx].color[0];
            color[1] = lbl_803E4998[idx].color[1];
            color[2] = lbl_803E4998[idx].color[2];
            color[3] = lbl_803E4998[idx].color[3];
            return 1;
        }
    }
    return 0;
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

// 0x800397F8: asset of a name in group 0xF1754BF2
extern "C" int fn_800397F8(const char* name) {
    if (name != 0) {
        int r = AssetFind(0xF1754BF2, StrHash(name));
        return (r == -1) ? -1 : r;
    }
    return -1;
}

// 0x800398A8: asset of a name in group 0xD9044CBB
extern "C" int fn_800398A8(const char* name) {
    if (name != 0) {
        int r = AssetFind(0xD9044CBB, StrHash(name));
        return (r == -1) ? -1 : r;
    }
    return -1;
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
extern "C" ModelNode* fn_8003A1C0(u32 model) {
    int idx = model & 0xFFFF;
    if (idx >= 0 && idx < lbl_804D4300->count)
        return &lbl_804D4300->nodes[idx];
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
// TODO (not drafted yet, 0xB0 bytes at 0x800398A8)
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
// TODO (not drafted yet, 0x4B8 bytes at 0x8003AE54)
// TODO (not drafted yet, 0xF4 bytes at 0x8003B30C)
// TODO (not drafted yet, 0xA4 bytes at 0x8003B400)
// TODO (not drafted yet, 0x94 bytes at 0x8003B4A4)
// TODO (not drafted yet, 0x454 bytes at 0x8003B538)


// 0x8003ADD4: build the bone matrices of a model into the shared buffer
extern "C" void* fn_8003ADD4(void* a, int model, void* b, void* c) {
    int idx = (u16)model;
    if (idx >= 0 && idx < lbl_804D4300->count) {
        ModelNode* n = &lbl_804D4300->nodes[idx];
        fn_8003AE54(n->boneCount, &lbl_804D4300->array30[n->field3E], a, b, c, lbl_803E19F8);
        return lbl_803E19F8;
    }
    return 0;
}

// 0x8003B30C: free the data of every used draw slot and both bit tables
extern "C" void fn_8003B30C() {
    int i;
    for (i = 0; i < 0x300; i++) {
        if (lbl_803F9998[i / 32] & (1 << (i & 31))) {
            if (lbl_803E4998[i].data != 0)
                VFree(lbl_803E4998[i].data);
        }
    }
    if (gModInfoBits0 != 0)
        MemFree(gModInfoBits0, "ModInfo.cpp", 387);
    gModInfoBits0 = 0;
    gModInfoBits0Size = 0;
    lbl_804D4310 = -1;
    if (gModInfoBits1 != 0)
        MemFree(gModInfoBits1, "ModInfo.cpp", 394);
    gModInfoBits1 = 0;
    gModInfoBits1Size = 0;
    lbl_804D431C = -1;
}

// 0x8003B400: allocate the two bit tables, sized from the model database counts
extern "C" void fn_8003B400() {
    if (lbl_804D4300->field0C > 0) {
        gModInfoBits0Size = lbl_804D4300->field0C;
        gModInfoBits0Size = (gModInfoBits0Size + 7) & ~7;
        gModInfoBits0 = MemAlloc(gModInfoBits0Size, "ModInfo.cpp", 348);
        gModInfoBits0Size = gModInfoBits0Size / 8;
    }
    if (lbl_804D4300->field18 > 0) {
        gModInfoBits1Size = lbl_804D4300->field18;
        gModInfoBits1Size = (gModInfoBits1Size + 7) & ~7;
        gModInfoBits1 = MemAlloc(gModInfoBits1Size, "ModInfo.cpp", 360);
        gModInfoBits1Size = gModInfoBits1Size / 8;
    }
}

// 0x8003B4A4: reset the model database and the draw slots; register the chunk 6 loader
extern "C" void fn_8003B4A4() {
    lbl_804D4300 = 0;
    gModInfoBits0 = 0;
    gModInfoBits0Size = 0;
    lbl_804D4310 = -1;
    gModInfoBits1 = 0;
    gModInfoBits1Size = 0;
    lbl_804D431C = -1;
    memset(lbl_803E4998, 0, 0x15000);
    memset(lbl_803F9998, 0, 100);
    lbl_803F9998[0] |= 1;
    FileAddHandler(6, fn_8003B538, 0);
}
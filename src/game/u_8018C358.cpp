// u_8018C358.cpp: the "SPLINE GUN" effect type (0x8018C358-0x8018D2D4).
// Only the create function is written so far. Names are descriptive guesses; the code is
// matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    if (s == 0) {
        return 0;
    } else {
        u32 h = 0;
        int i;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
        return h;
    }
}

struct EffectType {
    const char* name;
    u32 hash;
    int unk8[3];
    void* (*create)(int a, int id);
    void* destroy;
    void* msg;
    void* query;
    void* update;
    void* reset;
    int unk2C;
};

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}

struct SplineGunDef {
    int unk0;
    int unk4;
    int unk8;
    u8 pad0C[0x64 - 0x0C];
    int unk64;
    int unk68;
};

struct SplineGun {
    int unk0;
    float unk4;
    int emitter;
    SplineGunDef* def;
    int unk10;
    float unk14;
    int unk18;
    float unk1C;
    int pad20[2];
    int unk28;
    u8 pad2C[0x94 - 0x2C];
};

extern EffectType lbl_803A8F38;  // the "SPLINE GUN" type descriptor

void* SplineGunCreate(int a, int id) {
    EffectType* type = &lbl_803A8F38;
    SplineGun* g;

    EffectTypeHash(type);
    g = (SplineGun*)VAlloc(sizeof(SplineGun));
    memset(g, 0, sizeof(SplineGun));
    if (g == 0) {
        return 0;
    }
    g->def = (SplineGunDef*)fn_800EB21C(id);
    g->unk18 = -1;
    g->unk14 = 0.0f;
    g->unk28 = 0;
    g->unk1C = 100.0f;
    g->unk4 = 0.0f;
    g->unk0 = 0;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->unk10, g->def->unk8);
    Fn800F840C(g->emitter, g->def->unk4, 1);
    Fn800F840C(g->emitter, g->def->unk64, g->def->unk68);
    return g;
}

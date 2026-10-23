// u_80226DE8.cpp: the "MCG" effect type. Only the create function is written so far; names are
// descriptive guesses and the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_800F8A98(u32);
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
struct McgDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
    u8 pad0C[0x58];
    int unk64;
};

struct Mcg {
    int unk00;
    float unk04;
    float unk08;
    float unk0C;
    float unk10;
    float unk14;
    int emitter;
    McgDef* def;
    int unk20;
    int unk24;
    float unk28;
    float unk2C;
    int unk30;
    int unk34;
    int unk38;
    u8 pad3C[0x68];
};

extern EffectType lbl_803BD1C8;  // the "MCG" type descriptor

void* McgCreate(int a, int id) {
    EffectType* type = &lbl_803BD1C8;
    Mcg* g;

    EffectTypeHash(type);
    g = (Mcg*)VAlloc(sizeof(Mcg));
    memset(g, 0, sizeof(Mcg));
    if (g == 0) {
        return 0;
    }
    g->def = (McgDef*)fn_800EB21C(id);
    g->unk24 = fn_800F8A98(0xB1F8B047);
    g->unk28 = 0.0f;
    g->unk2C = 0.0f;
    g->unk34 = 0;
    g->unk38 = 0;
    g->unk04 = 0.0f;
    g->unk08 = 0.0f;
    g->unk30 = -1;
    g->unk0C = 0.0f;
    g->unk10 = 0.0f;
    g->unk14 = 0.0f;
    g->unk20 = 1;
    g->unk00 = 0;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->unk24, g->def->unk08);
    Fn800F840C(g->emitter, g->def->unk04, 1);
    Fn800F840C(g->emitter, g->def->unk64, 1);
    return g;
}


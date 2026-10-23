// u_801BF578.cpp: the "ASHEAVY" effect type. Only the create function is written so far; names are
// descriptive guesses and the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_80019A04();
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);
extern int lbl_804D5190;

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
struct AsHeavyDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
    u8 pad0C[0x58];
    int unk64;
};

struct AsHeavy {
    int unk00;
    float unk04;
    float unk08;
    int emitter;
    AsHeavyDef* def;
    int unk14;
    int unk18;
    float unk1C;
    float unk20;
    float unk24;
    u8 pad28[0x68];
};

extern EffectType lbl_803ACAD8;  // the "ASHEAVY" type descriptor

void* AsHeavyCreate(int a, int id) {
    EffectType* type = &lbl_803ACAD8;
    AsHeavy* g;

    EffectTypeHash(type);
    g = (AsHeavy*)VAlloc(sizeof(AsHeavy));
    memset(g, 0, sizeof(AsHeavy));
    if (g == 0) {
        return 0;
    }
    g->def = (AsHeavyDef*)fn_800EB21C(id);
    g->unk04 = 0.0f;
    g->unk08 = 0.0f;
    g->unk18 = -1;
    g->unk1C = 0.0f;
    g->unk20 = 0.0f;
    g->unk24 = 0.0f;
    g->unk14 = 1;
    g->unk00 = 0;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    lbl_804D5190 = fn_80019A04();
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    Fn800F840C(g->emitter, g->def->unk64, 1);
    return g;
}


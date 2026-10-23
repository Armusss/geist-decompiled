// u_800E1B5C.cpp: the "Pistol" effect type. Only the create function is written so far; names are
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
extern int lbl_804D4A18;

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
struct PistolDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
};

struct Pistol {
    int emitter;
    PistolDef* def;
    int unk08;
    u8 pad0C[0x4];
    int unk10;
    float unk14;
    float unk18;
    float unk1C;
};

extern EffectType lbl_8039C428;  // the "Pistol" type descriptor

void* PistolCreate(int a, int id) {
    EffectType* type = &lbl_8039C428;
    Pistol* g;

    EffectTypeHash(type);
    g = (Pistol*)VAlloc(sizeof(Pistol));
    memset(g, 0, sizeof(Pistol));
    if (g == 0) {
        return 0;
    }
    g->def = (PistolDef*)fn_800EB21C(id);
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    g->unk08 = 1;
    g->unk10 = -1;
    g->unk14 = 0.0f;
    g->unk18 = 0.0f;
    g->unk1C = 0.0f;
    lbl_804D4A18 = fn_80019A04();
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    return g;
}


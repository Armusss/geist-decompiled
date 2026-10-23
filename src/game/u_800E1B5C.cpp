// u_800E1B5C.cpp: the "AUG" and "Pistol" effect types. Only the create functions are written so far;
// names are descriptive guesses and the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_80019A04();
int fn_800F8AF4(int id);
void fn_80175EE4(void* p);
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);
extern int lbl_804D4A10;
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
struct AugDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
};

struct Aug {
    int unk00;
    float unk04;
    float unk08;
    int emitter;
    AugDef* def;
    int unk14;
    u8 pad18[0x68];
    int unk80;
    int unk84;
    float unk88;
    float unk8C;
    float unk90;
    int unk94;
    u8 pad98[0x38];
};

extern EffectType lbl_8039C3C8;  // the "AUG" type descriptor

void* AugCreate(int a, int id) {
    EffectType* type = &lbl_8039C3C8;
    Aug* g;

    EffectTypeHash(type);
    g = (Aug*)VAlloc(sizeof(Aug));
    memset(g, 0, sizeof(Aug));
    if (g == 0) {
        return 0;
    }
    g->def = (AugDef*)fn_800EB21C(id);
    g->unk08 = 100.0f;
    g->unk00 = 0;
    g->unk04 = 0.0f;
    g->unk14 = 1;
    g->unk80 = fn_800F8AF4(id);
    g->unk84 = -1;
    g->unk88 = 0.0f;
    g->unk8C = 0.0f;
    g->unk90 = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    lbl_804D4A10 = fn_80019A04();
    fn_80175EE4(&g->unk94);
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    return g;
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


// u_801E241C.cpp: the "Sniper" effect type. Only the create function is written so far; names are
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
extern int lbl_804D5280;

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
struct SniperDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
};

struct Sniper {
    int unk00;
    float unk04;
    float unk08;
    float unk0C;
    int emitter;
    SniperDef* def;
    u8 pad18[0x18];
    int unk30;
    int unk34;
};

extern EffectType lbl_803B3B00;  // the "Sniper" type descriptor

void* SniperCreate(int a, int id) {
    EffectType* type = &lbl_803B3B00;
    Sniper* g;

    EffectTypeHash(type);
    g = (Sniper*)VAlloc(sizeof(Sniper));
    memset(g, 0, sizeof(Sniper));
    if (g == 0) {
        return 0;
    }
    g->def = (SniperDef*)fn_800EB21C(id);
    g->unk00 = 0;
    g->unk30 = -1;
    g->unk34 = -1;
    g->unk04 = 0.0f;
    g->unk08 = 0.0f;
    g->unk0C = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    lbl_804D5280 = fn_80019A04();
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    return g;
}


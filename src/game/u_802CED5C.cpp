// u_802CED5C.cpp: the "STATUE BEAM" effect type. Only the create function is written so far; names are
// descriptive guesses and the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_800F8AF4(int id);
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
struct StatueBeamDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
    u8 pad0C[0x58];
    int unk64;
    int unk68;
};

struct StatueBeam {
    int unk00;
    int emitter;
    int unk08;
    StatueBeamDef* def;
    u8 pad10[0x6C];
    int unk7C;
};

extern EffectType lbl_803C5380;  // the "STATUE BEAM" type descriptor

void* StatueBeamCreate(int a, int id) {
    EffectType* type = &lbl_803C5380;
    StatueBeam* g;

    EffectTypeHash(type);
    g = (StatueBeam*)VAlloc(sizeof(StatueBeam));
    memset(g, 0, sizeof(StatueBeam));
    if (g == 0) {
        return 0;
    }
    g->def = (StatueBeamDef*)fn_800EB21C(id);
    g->unk00 = 0;
    g->unk7C = fn_800F8AF4(id);
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->unk08, g->def->unk08);
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    Fn800F840C(g->emitter, g->def->unk64, g->def->unk68);
    return g;
}


// u_8013CA38.cpp: the "ASR RIFLE" effect type. Only the create function is written so far; names are
// descriptive guesses and the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_80019A04();
int fn_800F8A98(u32);
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);
extern int lbl_804D4D28;

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
struct AsrRifleDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
    u8 pad0C[0x58];
    int unk64;
    int unk68;
};

struct AsrRifle {
    int unk00;
    float unk04;
    u8 pad08[0x4];
    float unk0C;
    float unk10;
    AsrRifleDef* def;
    int emitter;
    int unk1C;
    int unk20;
    float unk24;
    int unk28;
    u8 pad2C[0x68];
    int unk94;
};

extern EffectType lbl_803A0F50;  // the "ASR RIFLE" type descriptor

void* AsrRifleCreate(int a, int id) {
    EffectType* type = &lbl_803A0F50;
    AsrRifle* g;

    EffectTypeHash(type);
    g = (AsrRifle*)VAlloc(sizeof(AsrRifle));
    memset(g, 0, sizeof(AsrRifle));
    if (g == 0) {
        return 0;
    }
    g->def = (AsrRifleDef*)fn_800EB21C(id);
    g->unk20 = fn_800F8A98(0xB1F8B047);
    g->unk04 = 0.0f;
    g->unk28 = -1;
    g->unk24 = 0.0f;
    g->unk1C = 1;
    g->unk00 = 0;
    g->unk94 = -1;
    g->unk0C = 0.0f;
    g->unk10 = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    lbl_804D4D28 = fn_80019A04();
    Fn800F840C(g->emitter, g->unk20, g->def->unk08);
    Fn800F840C(g->emitter, g->def->unk04, 1);
    Fn800F840C(g->emitter, g->def->unk64, g->def->unk68);
    return g;
}


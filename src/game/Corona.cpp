// Corona.cpp: the "SELF EXPLODE" effect type. Only the create function is written so far; names are
// descriptive guesses and the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_800F7994(int, u32);
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
struct SelfExplodeDef {
};

struct SelfExplode {
    int unk00;
    int unk04;
};

extern EffectType lbl_803BE1A8;  // the "SELF EXPLODE" type descriptor

void* SelfExplodeCreate(int a, int id) {
    EffectType* type = &lbl_803BE1A8;
    SelfExplode* g;

    EffectTypeHash(type);
    g = (SelfExplode*)VAlloc(sizeof(SelfExplode));
    memset(g, 0, sizeof(SelfExplode));
    if (g == 0) {
        return 0;
    }
    g->unk04 = fn_800F7994(id, 0x96205479);
    g->unk00 = 0;
    return g;
}


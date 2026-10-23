// u_8025643C.cpp: the "GUNWITHGRENADE" effect type. Only the create function is written so far; names are
// descriptive guesses and the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_80019A04();
int fn_800F8AF4(int id);
void fn_80175D90(void* p);
void fn_80175EE4(void* p);
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);
extern int lbl_804D54D0;

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
struct GunWithGrenadeDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
    u8 pad0C[0x58];
    int unk64;
    int unk68;
};

struct GunWithGrenade {
    int unk00;
    float unk04;
    float unk08;
    float unk0C;
    float unk10;
    float unk14;
    int emitter;
    GunWithGrenadeDef* def;
    int unk20;
    u8 pad24[0x68];
    int unk8C;
    int unk90;
    int unk94;
    u8 pad98[0x38];
    int unkD0;
    u8 padD4[0x70];
};

extern EffectType lbl_803BF770;  // the "GUNWITHGRENADE" type descriptor

void* GunWithGrenadeCreate(int a, int id) {
    EffectType* type = &lbl_803BF770;
    GunWithGrenade* g;

    EffectTypeHash(type);
    g = (GunWithGrenade*)VAlloc(sizeof(GunWithGrenade));
    memset(g, 0, sizeof(GunWithGrenade));
    if (g == 0) {
        return 0;
    }
    g->def = (GunWithGrenadeDef*)fn_800EB21C(id);
    g->unk20 = 1;
    g->unk8C = fn_800F8AF4(id);
    g->unk00 = 0;
    g->unk04 = 0.0f;
    g->unk08 = 200.0f;
    g->unk90 = -1;
    g->unk0C = 0.0f;
    g->unk10 = 0.0f;
    g->unk14 = 0.0f;
    fn_80175EE4(&g->unk94);
    fn_80175D90(&g->unkD0);
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    lbl_804D54D0 = fn_80019A04();
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    Fn800F840C(g->emitter, g->def->unk64, g->def->unk68);
    return g;
}


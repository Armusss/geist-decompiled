// u_80294548.cpp: the "SpitAndSqueal" effect type. Only the create function is written so far; names are
// descriptive guesses and the code is matched against the retail DOL.

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
struct SpitAndSquealDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
    u8 pad0C[0x58];
    int unk64;
    int unk68;
};

struct SpitAndSqueal {
    int unk00;
    float unk04;
    float unk08;
    int emitter;
    SpitAndSquealDef* def;
    u8 pad14[0x14];
};

extern EffectType lbl_803C30B0;  // the "SpitAndSqueal" type descriptor

void* SpitAndSquealCreate(int a, int id) {
    EffectType* type = &lbl_803C30B0;
    SpitAndSqueal* g;

    EffectTypeHash(type);
    g = (SpitAndSqueal*)VAlloc(sizeof(SpitAndSqueal));
    memset(g, 0, sizeof(SpitAndSqueal));
    if (g == 0) {
        return 0;
    }
    g->def = (SpitAndSquealDef*)fn_800EB21C(id);
    g->unk08 = 200.0f;
    g->unk00 = 0;
    g->unk04 = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    Fn800F840C(g->emitter, g->def->unk64, g->def->unk68);
    return g;
}


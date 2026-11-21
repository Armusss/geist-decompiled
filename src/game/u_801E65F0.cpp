// u_801E65F0.cpp (0x801E65F0-0x801E72E0): the "Sniper" effect type, split from the old u_801E241C range.
// Boundary evidence:
// - .data: the type descriptor 0x803B3B00 lists reset 0x801E65F0, update 0x801E6600, query 0x801E6814,
//   msg 0x801E69AC, destroy 0x801E7160 and create 0x801E7198: exactly this range.
// - .rodata: pool 0x80348F18-0x80348F68, the ghost-action name table followed by "Sniper"; the next file has its
//   own table at 0x80348F68. .sdata2: own pool 0x804D8ED8-0x804D8EF0 (0.0f 0x804D8ED8, double 0x804D8EE0).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include "game/effect.h"

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

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}
// TODO (not drafted yet, 0x10 bytes at 0x801E65F0)
// fn_801E65F0
// TODO (not drafted yet, 0x214 bytes at 0x801E6600)
// fn_801E6600
// TODO (not drafted yet, 0x198 bytes at 0x801E6814)
// fn_801E6814
// TODO (not drafted yet, 0x7B4 bytes at 0x801E69AC)
// fn_801E69AC
// TODO (not drafted yet, 0x38 bytes at 0x801E7160)
// fn_801E7160

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


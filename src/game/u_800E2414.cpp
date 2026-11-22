// u_800E2414.cpp (0x800E2414-0x800E39F4): the "AUG" effect type, split from the old u_800E1B5C range.
// Boundary evidence: the descriptor 0x8039C3C8 lists reset 0x800E2414, update 0x800E244C, query 0x800E288C,
// msg 0x800E2A24, destroy 0x800E3820, create 0x800E3860 and 0x800E39D0 (slot +0x0C); .rodata pool
// 0x80344E58-0x80344EA8 (ghost-action table + "AUG"); .sdata2 0x804D71A0-0x804D71C0 (own 0.0f and double).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include "game/effect.h"

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

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

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

extern EffectType gAugType;  // the "AUG" type descriptor

// TODO (not drafted yet, 0x38 bytes at 0x800E2414)
// fn_800E2414
// TODO (not drafted yet, 0x440 bytes at 0x800E244C)
// fn_800E244C
// TODO (not drafted yet, 0x198 bytes at 0x800E288C)
// fn_800E288C
// TODO (not drafted yet, 0xDFC bytes at 0x800E2A24)
// fn_800E2A24
// TODO (not drafted yet, 0x40 bytes at 0x800E3820)
// fn_800E3820

void* AugCreate(int a, int id) {
    EffectType* type = &gAugType;
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

// TODO (not drafted yet, 0x24 bytes at 0x800E39D0)
// fn_800E39D0

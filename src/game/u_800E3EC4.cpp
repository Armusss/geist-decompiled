// u_800E3EC4.cpp (0x800E3EC4-0x800E48A0): the "Pistol" effect type, split from the old u_800E1B5C range.
// Boundary evidence: descriptor 0x8039C428 lists reset 0x800E3EC4, update 0x800E3EC8, query 0x800E4030,
// msg 0x800E4154, destroy 0x800E4724 and create 0x800E475C: exactly this range; .rodata pool
// 0x80344EF8-0x80344F48 (ghost-action table + "Pistol"); .sdata2 0x804D71D8-0x804D71F0 (own 0.0f and double).
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

extern EffectType gPistolType;  // the "Pistol" type descriptor

//@@800E3EC4
extern "C" void fn_800E3EC4(EffectObj* obj);
void fn_800E3EC4(EffectObj* obj) {}
//@@end
// TODO (not drafted yet, 0x168 bytes at 0x800E3EC8)
// fn_800E3EC8
// TODO (not drafted yet, 0x124 bytes at 0x800E4030)
// fn_800E4030
// TODO (not drafted yet, 0x5D0 bytes at 0x800E4154)
// fn_800E4154
//@@800E4724
extern "C" void fn_800F86A0(int emitter);
void VFree(void* p);
extern "C" void fn_800E4724(EffectObj* obj);
void fn_800E4724(EffectObj* obj) {
    Pistol* g = (Pistol*)obj->data;
    fn_800F86A0(g->emitter);
    VFree(g);
}
//@@end

void* PistolCreate(int a, int id) {
    EffectType* type = &gPistolType;
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

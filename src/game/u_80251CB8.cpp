// u_80251CB8.cpp: the "IMP FIRE" effect type. Only the create function is written so far; names are
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
struct EffectObj {
    int unk0;
    int unk4;
    int emitter;
    void* data;
};

extern "C" {
int fn_800F7D28(int emitter, int id);
void fn_800F86A0(int emitter);
}

void VFree(void* p);

struct ImpFireDef {
    u8 pad00[0x4];
    int unk04;
    int unk08;
};

struct ImpFire {
    int unk00;
    float unk04;
    int emitter;
    ImpFireDef* def;
    u8 pad10[0x78];
};

extern EffectType lbl_803BEDE8;  // the "IMP FIRE" type descriptor

void ImpFireReset(EffectObj* obj) {
    ImpFire* g = (ImpFire*)obj->data;

    g->unk00 = 0;
    g->unk04 = 0.0f;
}

u32 ImpFireQuery(EffectObj* obj, int msg) {
    ImpFire* g = (ImpFire*)obj->data;
    ImpFireDef* def = g->def;

    switch (msg) {
    case 0x0020EE36:
        return g->unk00 != 0 ? 0x3DA260F7 : 0x04A3E183;
    case 0xF829DB93:
        if (g->unk00 != 0) {
            return 0x3DA260F7;
        }
        if (fn_800F7D28(g->emitter, def->unk04)) {
            return 0x04A3E183;
        }
        return fn_800F7D28(obj->emitter, def->unk04) != 0 ? 0xC0BECF7E : 0x11D2C025;
    case 0x9791AF51:
    case 0x968CBE3D:
    case 0x96224FCC:
    case 0xE6B71BCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        return 0x3DA260F7;
    }
    return 0x3DA260F7;
}

void ImpFireDestroy(EffectObj* obj) {
    ImpFire* g = (ImpFire*)obj->data;

    fn_800F86A0(g->emitter);
    VFree(g);
}

void* ImpFireCreate(int a, int id) {
    EffectType* type = &lbl_803BEDE8;
    ImpFire* g;

    EffectTypeHash(type);
    g = (ImpFire*)VAlloc(sizeof(ImpFire));
    memset(g, 0, sizeof(ImpFire));
    if (g == 0) {
        return 0;
    }
    g->def = (ImpFireDef*)fn_800EB21C(id);
    g->unk00 = 0;
    g->unk04 = 0.0f;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    return g;
}


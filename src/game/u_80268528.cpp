// u_80268528.cpp: the "HERMIT SPIT" effect type (0x80268528-0x802690C8): reset, update, query, message,
// destroy and create, plus the gHermitSpitType descriptor that points at all six. One .sdata2 pool
// (200.0f, 0.0f). A plain shot also starts the "crapElectricity" effect on a possessed target. Names are descriptive guesses; the code is matched
// against the retail DOL.

#include <dolphin/types.h>

#include <string.h>
#include "game/string.h"
#include "game/msg.h"
#include "game/effect.h"

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_800F8AF4(int id);
int fn_800F7D28(int emitter, int id);
void fn_800F825C(int emitter, int id, int count);
void fn_800F86A0(int emitter);
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
int FxStart(void* fx, int a, int b);
void fn_8016EEF4(int handle);
void fn_8000F550(short* ids, int count);
void fn_80163824(int player, u32 id, int a, int b, int c);
void fn_80163854(int player, int a, int b);
int SendMsg(int target, int msg, void* data);
}

void* VAlloc(int size);
void VFree(void* p);
void Fn800F840C(int emitter, int a, int b);
void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
int Fn80175D60(unsigned long arg);
void* FxFind(const char* name);
int AssetFind(u32 table, u32 key);
int PlayerGetPossessor(int arg);
void TextureLoad(short* ids, int count);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);

int lbl_804D3770 = 1;  // .sdata flag: start the definition's hit effect on the next big shot

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

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

struct ShotHit {
    u8 pad[0x30];
    float x, y, z;
    u8 pad3C[0x24];
};

struct SpitInfo {
    ShotHit* hit;
    float speed;
    float unk8;
    int unkC;
    int unk10;
    int unk14;
};

// The big shot's request carries two more words (unused here).
struct BigSpitInfo {
    ShotHit* hit;
    float speed;
    float unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
};

// State block reached through EffectObj's pointer field at 0xC.
struct HermitSpitState {
    int state;          // 0x00
    unsigned char pad04[0x7C];
    int timer;          // 0x80, -1 when no timer is running
    float elapsed;      // 0x84
};

// Query view of the same state block: mode flag, two ids, and a pointer to
// the per-spawn parameter block.
struct HermitSpitParams {
    int mode;               // 0x00
    int idA;                // 0x04
    int idB;                // 0x08
    unsigned char pad0C[0x58];
    int idC;                // 0x64
};

struct HermitSpitQueryState {
    int enabled;                // 0x00
    int idA;                    // 0x04
    int idB;                    // 0x08
    HermitSpitParams* params;   // 0x0C
};

struct HermitSpitDef {
    int unk00;
    int id;
    int max;
    float speed;
    u8 pad10[0x48 - 0x10];
    int model2;
    u8 pad4C[0x64 - 0x4C];
    int id2;
    int max2;
    float speed2;
    float unk70;
    u8 pad74[0x90 - 0x74];
    void* hitFx;
    u8 pad94[0x98 - 0x94];
    int model;
    u8 pad9C[0xBC - 0x9C];
    int unkBC;
};

struct HermitSpit {
    int state;
    int emitter;
    int unk08;
    HermitSpitDef* def;
    int unk10;
    int arg;
    int pad18;
    ShotHit shot;
    u32 ammo;
    int fx;
    float unk84;
};

void HermitSpitReset(EffectObj* obj) {
    ((HermitSpitState*)obj->data)->state = 0;
}

void HermitSpitUpdate(EffectObj* obj, float dt) {
    HermitSpitState* s = (HermitSpitState*)obj->data;

    lbl_804D3770 = 1;
    if (s->timer != -1) {
        s->elapsed += dt;
        if (s->elapsed > 200.0f) {
            fn_8016EEF4(s->timer);
            s->timer = -1;
        }
    }
}

// Message hashes handled by the query.
static const int kMsgSpitFire = -0x07D6246D;   // 0xF829DB93
static const int kMsgSpitFire2 = 0x42E50A3D;
static const int kMsgSpitIdle = -0x686E50AF;   // 0x9791AF51
static const int kMsgSpitIdle2 = 0x0D19E338;
static const int kMsgSpitHit = -0x697341C3;    // 0x968CBE3D
static const int kMsgSpitHit2 = -0x1948E434;   // 0xE6B71BCC
static const int kMsgSpitCheck = -0x69DDB034;  // 0x96224FCC
static const int kMsgSpitCheck2 = 0x0020EE36;

// Answer hashes returned by the query.
static const int kHermitSpitAllowed = 0x11D2C025;
static const int kHermitSpitBusy = 0x04A3E183;
static const int kHermitSpitBlocked = (int)0xC0BECF7E;
static const int kHermitSpitIgnore = 0x4848CA1E;
static const int kHermitSpitDeny = 0x3DA260F7;

u32 HermitSpitQuery(EffectObj* obj, int msg) {
    HermitSpitQueryState* s = (HermitSpitQueryState*)obj->data;
    HermitSpitParams* p = s->params;

    switch (msg) {
    case kMsgSpitCheck:
    case kMsgSpitCheck2:
        if (fn_800F7D28(s->idA, s->idB) != 0)
            return kHermitSpitBusy;
        if (fn_800F7D28(obj->emitter, p->idA) != 0)
            return kHermitSpitBlocked;
        return kHermitSpitAllowed;
    case kMsgSpitFire:
    case kMsgSpitFire2:
        if (fn_800F7D28(s->idA, p->idC) != 0)
            return kHermitSpitBusy;
        if (fn_800F7D28(obj->emitter, p->idC) != 0)
            return kHermitSpitBlocked;
        return kHermitSpitAllowed;
    case kMsgSpitIdle:
    case kMsgSpitIdle2:
        if (s->enabled == 1)
            return kHermitSpitBusy;
        return kHermitSpitDeny;
    case kMsgSpitHit:
    case kMsgSpitHit2:
        if (p->idB == fn_800F7D28(s->idA, s->idB))
            return kHermitSpitIgnore;
        if (fn_800F7D28(obj->emitter, p->idA) != 0)
            return kHermitSpitBusy;
        return kHermitSpitAllowed;
    }
    return kHermitSpitDeny;
}

int HermitSpitMsg(EffectObj* obj, int msg, int arg, void* data, float dt);
void HermitSpitDestroy(EffectObj* obj);
void* HermitSpitCreate(int a, int id);

EffectType gHermitSpitType = {
    "HERMIT SPIT", 0, {0, 0, 0}, HermitSpitCreate, HermitSpitDestroy, HermitSpitMsg, HermitSpitQuery,
    HermitSpitUpdate, HermitSpitReset, 0,
};

int HermitSpitMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    HermitSpit* s = (HermitSpit*)obj->data;
    HermitSpitDef* def = s->def;
    int busy = Fn80175D60(arg);
    BigSpitInfo info2;
    SpitInfo info1;
    short texs[10];
    int pos[3];
    short tex;
    short model;

    s->arg = arg;
    switch (msg) {
    case 0x0020EE36:
        if (s->state == 1) {
            memcpy(&s->shot, data, 0x60);
        } else {
            if (!fn_800F7D28(s->emitter, def->id)) {
                return 0;
            }
            info1.hit = (ShotHit*)data;
            info1.speed = def->speed;
            info1.unk14 = 1;
            info1.unk10 = 0;
            Fn800F7F5C(s->emitter, def->id, 0x14967, arg, &info1, dt);
            {
                QueryMsg q;
                q.hash = 0x5A32795A;
                *(int*)&q.result = 0;
                q.unk8 = 0;
                SendMsg(arg, 0x41E, &q);
            }
            if (s->fx == -1 && ((arg >> 24) & 0xFF) == 2) {
                s->fx = FxStart(FxFind("crapElectricity"), 0, 0);
            }
            s->unk84 = 0.0f;
        }
        return 1;
    case 0x96224FCC:
        if (s->state != 1) {
            s->state = 1;
        }
        memcpy(&s->shot, data, 0x60);
        return 1;
    case 0x0D19E338:
        s->state = 0;
        return 1;
    case 0xF829DB93:
        if (s->state == 1) {
            memcpy(&s->shot, data, 0x60);
            s->arg = arg;
        } else {
            float* p;
            void* fx;

            if (!fn_800F7D28(s->emitter, def->id2)) {
                return 0;
            }
            info2.hit = (ShotHit*)data;
            info2.speed = def->speed2;
            info2.unk8 = def->unk70;
            info2.unkC = 0;
            info2.unk10 = 0;
            Fn800F7F5C(s->emitter, def->id2, 0x14967, arg, &info2, dt);
            fx = def->hitFx;
            p = &info2.hit->x;
            if (lbl_804D3770) {
                if (p) {
                    pos[0] = p[0];
                    pos[1] = p[1];
                    pos[2] = p[2];
                    fn_8016F118(fn_8016F120(fx, pos), 0, 100000);
                } else {
                    FxStart(fx, 0, 0);
                }
                lbl_804D3770 = 0;
            }
            {
                QueryMsg q;
                q.hash = 0x5A32795A;
                *(int*)&q.result = 0;
                q.unk8 = 0;
                SendMsg(arg, 0x41E, &q);
            }
        }
        return 1;
    case 0x42E50A3D:
        if (s->state != 1) {
            s->state = 1;
        }
        memcpy(&s->shot, data, 0x60);
        s->arg = arg;
        return 1;
    case 0x24725EAA:
        memcpy(&s->shot, data, 0x60);
        s->arg = arg;
        return 1;
    case 0x9791AF51:
        s->state = 0;
        return 1;
    case 0xE6B71BCC: {
        int n;
        int m;
        int r;

        n = fn_800F7D28(s->emitter, def->id);
        if (n == def->max) {
            return 0;
        }
        m = fn_800F7D28(obj->emitter, def->id);
        if (m == 0) {
            return 0;
        }
        n = (m < def->max - n) ? m : def->max - n;
        {
            QueryMsg q;
            q.hash = 0x5A32795A;
            *(int*)&q.result = 0;
            q.unk8 = 0;
            SendMsg(arg, 0x41E, &q);
            r = *(int*)&q.result;
        }
        if (r == 0) {
            fn_800F825C(obj->emitter, def->id, n);
        }
        Fn800F840C(s->emitter, def->id, n);
        return 1;
    }
    case 0x315A1A7F:
        memcpy(&s->shot, data, 0x60);
        return 1;
    case 0xE452286F:
    case 0x141109BD:
        tex = AssetFind(0xF2E2BEB8, 0x2184AD04);
        if ((int)data == 1) {
            TextureLoad(&tex, 1);
        } else {
            fn_8000F550(&tex, 1);
        }
        model = def->model;
        tex = AssetFind(0xF2E2BEB8, 0x5F678088);
        if ((int)data == 1) {
            ModelLoadIds(&model, 1);
            TextureLoad(&tex, 1);
        } else {
            ModelFreeIds(&model, 1);
            fn_8000F550(&tex, 1);
        }
        model = def->model2;
        texs[0] = AssetFind(0xF2E2BEB8, 0x5541EDD2);
        texs[1] = AssetFind(0xF2E2BEB8, 0x56F6C671);
        texs[2] = AssetFind(0xF2E2BEB8, 0x58AB9F10);
        texs[3] = AssetFind(0xF2E2BEB8, 0x5A6077AF);
        texs[4] = AssetFind(0xF2E2BEB8, 0x2E20AEE7);
        texs[5] = AssetFind(0xF2E2BEB8, 0x961C77AB);
        texs[6] = AssetFind(0xF2E2BEB8, 0x42F9014E);
        texs[7] = AssetFind(0xF2E2BEB8, 0x5C1B2515);
        if ((int)data == 1) {
            ModelLoadIds(&model, 1);
            TextureLoad(texs, 8);
        } else {
            ModelFreeIds(&model, 1);
            fn_8000F550(texs, 8);
        }
        return 1;
    case 0x0B59E861:
        if (busy == 0) {
            int player = PlayerGetPossessor(arg);
            int a = fn_800F7D28(obj->emitter, def->id);
            fn_80163824(player, 0x878DC2AD, a, fn_800F7D28(s->emitter, def->id), def->max);
            fn_80163854(player, player + 0x1508, def->unkBC);
        }
        return 1;
    case 0x7FAB1247:
        s->unk10 = 1;
        return 1;
    }
    return 0;
}

void HermitSpitDestroy(EffectObj* obj) {
    HermitSpit* s = (HermitSpit*)obj->data;

    if (s->fx != -1) {
        fn_8016EEF4(s->fx);
        s->fx = -1;
    }
    fn_800F86A0(s->emitter);
    VFree(s);
}

void* HermitSpitCreate(int a, int id) {
    EffectType* type = &gHermitSpitType;
    HermitSpit* g;

    EffectTypeHash(type);
    g = (HermitSpit*)VAlloc(sizeof(HermitSpit));
    memset(g, 0, sizeof(HermitSpit));
    if (g == 0) {
        return 0;
    }
    g->def = (HermitSpitDef*)fn_800EB21C(id);
    g->state = 0;
    g->ammo = fn_800F8AF4(id);
    g->fx = -1;
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(g->emitter, g->unk08, g->def->max);
    Fn800F840C(g->emitter, g->def->id, g->def->max);
    Fn800F840C(g->emitter, g->def->id2, g->def->max2);
    return g;
}

// ---------------------------------------------------------------------------------------------------

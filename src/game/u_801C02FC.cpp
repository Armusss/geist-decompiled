// u_801C02FC.cpp: 0x801C02FC-0x801C8B6C, 172 functions. A large character behaviour file (161 actions).
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   string pool 0x80348718 (POSSESS..SCAN, "[FASSVE]", "\n")
//   action table 0x803ABBC0 (0xF18 = 161 entries of 0x18 bytes, every callback is in this range)
//   .sdata2 0x804D8A48-0x804D8AD0 (one 0.0 at 0x804D8A64 read from 0x801C0C14 to 0x801C8934),
//   .bss 0x80496D18 (asset handles filled by fn_801C11F0), .sdata 0x804D3218/0x804D321C
// The same kind of component as u_801BF578 (SimpleHost: one action) and u_802B2680, with 161 actions: each
// table entry is {animation hash, enter, leave(next), choose next, event, name}. 113 of the functions are
// "enter action N" setters of one shape (current/next action, animation variant, ConditionSetBit).
// Every function keeps its fn_ placeholder name with C linkage.
// Names in comments are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;

float __fabsf(float x);

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// Per-actor action state (VAlloc'd, 0x54 bytes).
struct HostBeh {
    int prev;           // 0x00
    int cur;            // 0x04
    int next;           // 0x08
    int owner;          // 0x0C
    int anim;           // 0x10
    int animVar;        // 0x14
    u32 animOverride;   // 0x18
    int f1C;            // 0x1C
    int f20;            // 0x20
    int f24;            // 0x24
    int f28;            // 0x28
    float timer;        // 0x2C
    float f30;          // 0x30
    float f34;          // 0x34
    float f38;          // 0x38
    Vec3 motion;        // 0x3C
    char pad48[9];      // 0x48
    unsigned char variant;  // 0x51
    short f52;          // 0x52
};

// The host's per-type definition data (at actor+0xC4).
struct HostData {
    u32 type;           // 0x00
    char pad4[8];
    u32 kind;           // 0x0C
    char pad10[4];
    int i14;            // 0x14
    int i18;            // 0x18
    int i1C;            // 0x1C
};

struct ExtraHolder;
struct Owner;

struct Actor {
    char pad00[0x24];
    int x;              // 0x24
    int y;              // 0x28
    int z;              // 0x2C
    int (*msgFn)(int msg, void* data);  // 0x30
    char pad34[4];
    int id;             // 0x38
    char pad3C[0x48];
    Vec3 pos;           // 0x84
    char pad90[0x34];
    HostData* data;     // 0xC4
    char padC8[4];
    int model;          // 0xCC
    char padD0[4];
    int anim;           // 0xD4
    char padD8[0x34];
    void* cb10C;        // 0x10C
    void* cb110;        // 0x110
    char pad114[4];
    void* cb118;        // 0x118
    char pad11C[0x18];
    HostBeh* beh;       // 0x134
    float dt;           // 0x138
    char pad13C[0xD8];
    int hitBone;        // 0x214
    char pad218[8];
    int unk220;         // 0x220
    char pad224[0x14];
    u32 handle238;      // 0x238
    char pad23C[4];
    Vec3 v240;          // 0x240
    char pad24C[0x20];
    int unk26C;         // 0x26C
    char cond[0xC];     // 0x270
    Vec3 v27C;          // 0x27C: turn (z at 0x284)
    int unk288;         // 0x288
    char pad28C[0x364];
    ExtraHolder* extra; // 0x5F0
    void* obj5F4;       // 0x5F4
    char pad5F8[0x29C];
    char beh894[0x70];  // 0x894
    float f904;         // 0x904
    char pad908[0x14];
    float f91C;         // 0x91C
};

typedef int (*ActEnter)(Actor* a);
typedef int (*ActLeave)(Actor* a, int next);
typedef int (*ActNext)(Actor* a);
typedef int (*ActEvent)(int msg, Actor* a);

// One entry of the action table (0x18 bytes).
struct HostAction {
    u32 anim;           // 0x00
    ActEnter enter;     // 0x04
    ActLeave leave;     // 0x08
    ActNext next;       // 0x0C
    ActEvent event;     // 0x10
    const char* name;   // 0x14
};

extern HostAction gHostActions[161];

int ConditionSetBit(void* cond, int bit, int set);
int ConditionUpdateBit(void* cond, int bit, int set);
int MemFlagIsSet(void* bits, int index);                        // 0x800A7B78
int ZoneClassifyDirection(void* self, Vec3* v);                 // 0x800A82D0
void MemCopyVec3(float* dst, float* src);                       // 0x800A7BBC
void RangeResetPending(void* range);                            // 0x800A7028
void RangeResetFields(void* range);                             // 0x800A7068
void ExtraTrailAttach(ExtraHolder* h);                          // 0x80230FA8
int ExtraGetId(ExtraHolder* h);                                 // 0x80230450
void ExtraTrailRelease(ExtraHolder* h);                         // 0x80230F08
int ModelBoneName(int model, int bone);                         // 0x80018B44
int ModelBoneNodeChain(int model, int name, int* chain, int count); // 0x80018814
int OwnerId(Owner* o);                                          // 0x800A3A40
int ActorPlayAnim(int owner, u32 anim, int arg);                // 0x800C5654
int AnimBankGetSetCount(int a, int b, u32 anim);                // 0x800A1AC4
Actor* GetPlayer(void);                                         // 0x80049948
int ActorAnimState(int owner, u32 anim);                        // 0x800C5028
int AssetFind(u32 bank, u32 name);                              // 0x8007EB34
void TextureLoad(short* tex, int a);                            // 0x8000F738
int HasCondition(void* cond, int which);                        // 0x800A7AC4
void VFree(void* p);                                            // 0x8009DA3C
void* VAlloc(int size);                                         // 0x8009DA68

void Vec3Set(Vec3* v, float x, float y, float z);              // 0x8016D67C

extern "C" {
void fn_801C8934(Actor* a, int variant);
void fn_801C7D78(Actor* a);
void fn_80162610(Actor* a, void* beh, int msg);
void fn_800CB8BC(Actor* a, HostData* data, int arg, int c);
int fn_80212730(int id);
int fn_8016325C(void* beh);
void fn_800B42E0(Vec3* pos, int id, int a, int b, float r);
void fn_801BE5D4(Actor* a);
int fn_801BE008(Actor* a);
float fn_802A2080(void);
void fn_800B5004(Vec3* v, Vec3* to);
void fn_800BBCA8(Actor* a);
void fn_800B6EDC(u32 handle, int a);
void fn_800C6F58(Actor* a);
void fn_800902F0(int anim);
void fn_801D55CC(void* obj);
void fn_8008DE4C(int anim, int a, int b, int c);
int fn_8000DDD0(void);
int fn_802A1D04(void);
int fn_8020B97C(void);
int fn_801C0E7C(void* p);
int fn_8008EA9C(int anim, int a);
int fn_8008E964(int anim);
void fn_80090390(int anim, int a);
void fn_800B35D0(Actor* a);
int fn_800CDD10();
void fn_80090634(int anim, int a, int b, int c);
void fn_8008F4E8(int anim, int a);
void fn_8008F378(int anim);
void fn_8008F4EC(int anim, int a);
int fn_800A1BC8(int a, int b, u32 anim, int variant, int c);
int fn_8008E7D0(int h);
void fn_8008F230(int anim, float f, int a, int b);
void fn_80090974(int anim, int a, int h, float f, int b, ActEvent cb, Actor* owner);

int gHostAssets[16];                                            // 0x80496D18
float sDeathTime = 3000.0f;                                     // 0x804D3218
int sHostTexture = -1;                                          // 0x804D321C

// Stops the host: clears its motion and turn state and drops what it holds.
static inline void HostHalt(Actor* a, Vec3* zero) {
    a->unk288 = 0;
    a->unk26C = 0;
    fn_800B5004(&a->v240, zero);
    MemCopyVec3((float*)a->cond, (float*)zero);
    fn_800BBCA8(a);
    RangeResetPending(a->cond);
    {
        u32 h = a->handle238;
        if ((int)h != -1 && (int)(h >> 24) == 3) {
            fn_800B6EDC(h, 0);
            a->handle238 = 0xFFFFFFFF;
        }
    }
    fn_800C6F58(a);
}
int rand(void);

// 0x801C0950: message 0x41D (keyed commands) of the host.
int fn_801C0950(Msg41D* m) {
    u32 cmd = m->arg;
    Actor* a = (Actor*)m->actor;
    HostBeh* b = a->beh;
    switch (cmd) {
    case 0xEE14BF26:
        b->f34 = -1.0f;
        return 1;
    case 0x672F35C2:
        b->prev = b->cur;
        gHostActions[b->cur].leave(a, 0x11);
        gHostActions[0x11].enter(a);
        return 0;
    case 0x39CB0CB0:
        ConditionSetBit(a->cond, 0xB, 0);
        ConditionSetBit(a->cond, 0xD, 0);
        ConditionSetBit(a->cond, 0xC, 0);
        if (HasCondition(a->cond, 0xC)) {
            ConditionSetBit(a->cond, 0xC, 0);
            fn_80090390(a->anim, 0);
        }
        fn_800B35D0(a);
        if (b->cur != 0x13) {
            ConditionSetBit(a->cond, 0xE, 1);
            fn_80090390(a->anim, 0);
            fn_80090390(a->anim, 1);
            b->prev = b->cur;
            gHostActions[b->cur].leave(a, 0x10);
            gHostActions[0x10].enter(a);
        }
        ConditionUpdateBit(a->cond, 5, 0);
        ConditionUpdateBit(a->cond, 0xB, 0);
        b->f38 = 5000.0f;
        return 0;
    case 0x7DF38296:
        gHostActions[b->cur].leave(a, 0x29);
        gHostActions[0x29].enter(a);
        return 0;
    case 0x2EFC66CA: {
        u32 kind = a->data->kind;
        if (kind == 0x906E8474 || kind == 0xEA993A8F) {
            gHostActions[b->cur].leave(a, 0x13);
            gHostActions[0x13].enter(a);
        }
        return 0;
    }
    }
    return 0;
}

// 0x801C0E7C: per-frame update.
int fn_801C0E7C(void* p) {
    Actor* a;
    HostBeh* b;
    if (p == 0) {
        return 0;
    }
    a = (Actor*)p;
    b = a->beh;
    if (b->timer > 0.0f) {
        b->timer -= a->dt;
    }
    if (a->data->type == 0x8BD84799) {
        if (a->f91C > 1.5f) {
            b->f52 = 1500;
        } else {
            b->f52 = 0;
        }
    }
    if (HasCondition(a->cond, 9) && b->cur != 0x3F && b->cur != 0x40) {
        b->prev = b->cur;
        gHostActions[b->cur].leave(a, 0x3F);
        gHostActions[0x3F].enter(a);
    }
    if (HasCondition(a->cond, 0x1C) || fn_8008EA9C(a->anim, 0) == 0 || fn_8008EA9C(a->anim, 0) == 2) {
        if (!HasCondition(a->cond, 0xD) && !HasCondition(a->cond, 9) && !HasCondition(a->cond, 10) &&
            !HasCondition(a->cond, 0xB)) {
            gHostActions[b->cur].enter(a);
        }
        if (HasCondition(a->cond, 0x1C) && fn_8008E964(a->anim)) {
            ActorPlayAnim(a->id, 0xB2CB0AA4, a->id);
        }
        if (HasCondition(a->cond, 0x1C) && HasCondition(a->cond, 0xD) && fn_8008E964(a->anim)) {
            ActorPlayAnim(a->id, 0xB2CB0AA4, a->id);
            ConditionSetBit(a->cond, 0x1C, 0);
            ConditionSetBit(a->cond, 0x1D, 1);
        }
    }
    {
        int next = gHostActions[b->cur].next(a);
        if (next != b->cur && gHostActions[b->cur].leave(a, next)) {
            b->prev = b->cur;
            gHostActions[next].enter(a);
        }
    }
    if (!HasCondition(a->cond, 0xB) && b->f38 > 0.0f) {
        b->f38 -= a->dt;
        fn_800B42E0(&a->pos, a->id, 4, 0x12, 101.0f);
    }
    return 1;
}

// 0x801C11B0
void fn_801C11B0(Actor* a) {
    a->cb110 = 0;
    VFree(a->beh);
    a->beh = 0;
}

// 0x801C11F0: sets the behaviour up (also run from PlayerInitGunState).
void fn_801C11F0(int owner, void* p) {
    if (p != 0) {
        HostBeh* b;
        Actor* a = (Actor*)p;
        a->cb110 = (void*)fn_801C0E7C;
        a->cb118 = (void*)fn_800CDD10;
        b = (HostBeh*)VAlloc(0x54);
        if (b == 0) {
            a->beh = 0;
        } else {
            b->owner = owner;
            b->f30 = 30000.0f * ((float)rand() / 32767.0f) + 8500.0f;
            b->animOverride = 0;
            b->f38 = 0.0f;
            b->f52 = 0;
            a->beh = b;
            if (HasCondition(a->cond, 0xB)) {
                b->cur = 8;
                b->next = 8;
            } else if (HasCondition(a->cond, 10)) {
                b->cur = 0x3F;
                b->next = 0x40;
            } else if (a->f904 > 0.0f) {
                b->cur = 0x21;
                b->next = 0x21;
            } else {
                b->cur = 0x3F;
                b->next = 0x40;
            }
            gHostAssets[0] = AssetFind(0xF1754BF2, 0x21C5E0);
            gHostAssets[1] = AssetFind(0xF1754BF2, 0x24805F);
            gHostAssets[2] = AssetFind(0xF1754BF2, 0x101844CC);
            gHostAssets[3] = AssetFind(0xF1754BF2, 0x48874F6E);
            gHostAssets[4] = AssetFind(0xF1754BF2, 0x4A53FC1);
            gHostAssets[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
            gHostAssets[6] = AssetFind(0xF1754BF2, 0xD9404234);
            gHostAssets[7] = AssetFind(0xF1754BF2, 0x450B2BB);
            gHostAssets[8] = AssetFind(0xF1754BF2, 0x922BF04E);
            gHostAssets[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
            gHostAssets[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
            gHostAssets[11] = AssetFind(0xF1754BF2, 0x4A2F9AA);
            gHostAssets[12] = AssetFind(0xF1754BF2, 0x4A48BC0);
            gHostAssets[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
            gHostAssets[14] = AssetFind(0xF1754BF2, 0x44E6CA4);
            gHostAssets[15] = AssetFind(0xF1754BF2, 0x44FFEBA);
            sHostTexture = AssetFind(0xF2E2BEB8, 0x154CDE5C);
            {
                short tex = sHostTexture;
                TextureLoad(&tex, 2);
            }
            fn_80090634(a->anim, 0, 0, 0);
            ConditionSetBit(a->cond, 0xE, 0);
            gHostActions[b->cur].enter(a);
        }
    }
}

// 0x801C15A8: event handler of most actions.
int fn_801C15A8(int msg, void* p) {
    HostBeh* b;
    Actor* a;
    if (p == 0) {
        return 0;
    }
    a = (Actor*)p;
    b = a->beh;
    switch (msg) {
    case 0x1B83833F:
        b->prev = b->cur;
        gHostActions[b->cur].leave(a, b->next);
        gHostActions[b->next].enter(a);
        break;
    case (int)0xEAC45581:
        fn_80162610((Actor*)p, ((Actor*)p)->beh894, 0x20EE36);
        break;
    case 0x04A5C9FC:
    case 0x00239807:
    case 0x46341C3A:
        fn_800CB8BC((Actor*)p, ((Actor*)p)->data, b->f1C, ((Actor*)p)->unk26C);
        break;
    }
    return 1;
}

// 0x801C16D8: event handler of the attack actions.
int fn_801C16D8(int msg, Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    switch (msg) {
    case 0x1B83833F:
        b->prev = b->cur;
        gHostActions[b->cur].leave(a, b->next);
        gHostActions[b->next].enter(a);
        if (fn_8016325C(a->beh894) != 0) {
            fn_80162610(a, a->beh894, 0x1B83833F);
        }
        break;
    case (int)0xF829DB93:
    case 0x20EE36:
        if (fn_802A1D04() == 0) {
            GetPlayer();
            if (fn_8020B97C() != 0) {
                return 1;
            }
        }
        if (fn_8016325C(a->beh894) == 4) {
            fn_80162610(a, a->beh894, 0x20EE36);
        } else if (fn_8016325C(a->beh894) == 5) {
            fn_80162610(a, a->beh894, (int)0xF829DB93);
        }
        break;
    case (int)0xEAC45581:
        fn_80162610(a, a->beh894, 0x20EE36);
        break;
    case 0x04A5C9FC:
    case 0x00239807:
    case 0x46341C3A:
        fn_800CB8BC(a, a->data, b->f1C, a->unk26C);
        break;
    }
    return 1;
}

// 0x801C18C4
int fn_801C18C4(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    if (next == 0x40 || next == 0x60 || next == 0x80 || next == 0xA0 || next == 8) {
        return 1;
    }
    b->next = next;
    ConditionSetBit(a->cond, 0, 0);
    ConditionSetBit(a->cond, 4, 0);
    ConditionSetBit(a->cond, 0x2C, 0);
    if (b->cur == 7) {
        ConditionUpdateBit(a->cond, 0xB, 0);
    }
    if ((b->cur == 4 || b->cur == 5) && next == 0x3E) {
        b->cur = 0x21;
        return 1;
    }
    return 0;
}

// 0x801C19C0
int fn_801C19C0(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    if (next == 0x40 || next == 0x60 || next == 0x80 || next == 0xA0 || next == 8) {
        return 1;
    }
    b->next = next;
    ConditionSetBit(a->cond, 0, 0);
    ConditionSetBit(a->cond, 0, 0);
    ConditionSetBit(a->cond, 4, 0);
    if (b->cur == 7) {
        ConditionUpdateBit(a->cond, 0xB, 0);
    }
    if ((b->cur == 4 || b->cur == 5) && next == 0x3E) {
        b->cur = 0x21;
        return 1;
    }
    return 0;
}

// 0x801C1ABC
int fn_801C1ABC(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->next = next;
    ConditionSetBit(a->cond, 0, 0);
    {
        int cur = b->cur;
        if (cur == 8 || cur == 9) {
            if (next != 0x12) {
                ConditionSetBit(a->cond, 0xB, 0);
            }
        } else if (cur == 0xD) {
            ConditionUpdateBit(a->cond, 0x12, 0);
        } else if (cur == 0xE) {
            ConditionUpdateBit(a->cond, 0x13, 0);
        } else if (cur == 0xF) {
            ConditionUpdateBit(a->cond, 0x14, 0);
        } else if (cur == 1) {
            ConditionSetBit(a->cond, 0x12, 0);
        }
    }
    return 1;
}

// 0x801C1BBC
int fn_801C1BBC(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    a->beh->next = next;
    ConditionSetBit(a->cond, 0, 0);
    return 1;
}

// 0x801C1C04
int fn_801C1C04(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 0);
    a->v240.x = 0.0f;
    a->v240.y = 0.0f;
    a->v240.z = 0.0f;
    if ((b->cur - 0x21) % 32 == 0x1D && (next - 0x21) % 32 != 0x1D) {
        b->next = next;
    }
    ConditionSetBit(a->cond, 4, 0);
    return 1;
}

// 0x801C1CCC
int fn_801C1CCC(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 0);
    a->v240.x = 0.0f;
    a->v240.y = 0.0f;
    a->v240.z = 0.0f;
    if ((next - 0x21) % 32 != 0x1D) {
        b->next = next;
    }
    ConditionSetBit(a->cond, 4, 0);
    return 0;
}

// 0x801C1D70
int fn_801C1D70(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 0);
    if (fn_8016325C(a->beh894) == 6) {
        fn_80162610(a, a->beh894, 0xE6B71BCC);
    } else if (fn_8016325C(a->beh894) == 7) {
        fn_80162610(a, a->beh894, 0x968CBE3D);
    }
    b->next = next;
    ConditionSetBit(a->cond, 0, 0);
    return 1;
}

// 0x801C1E30
int fn_801C1E30(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->f30 = 30000.0f * ((float)rand() / 32767.0f) + 8500.0f;
    ConditionSetBit(a->cond, 0, 0);
    return 1;
}

// 0x801C1EBC
int fn_801C1EBC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 != 4) {
        b->prev = b->cur;
        gHostActions[b->cur].leave(a, 0x21);
        gHostActions[0x21].enter(a);
        fn_801C7D78(a);
    }
    return b->cur;
}

// 0x801C1F60
int fn_801C1F60(Actor* a) {
    HostBeh* b = a->beh;
    if (a->extra == 0 || fn_80212730(a->id) != -1) {
        fn_801C7D78(a);
    }
    return b->cur;
}

// 0x801C1FBC
int fn_801C1FBC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    Vec3Set(&a->v27C, 0.0f, 0.0f, 0.0f);
    if (b->f34 < 0.0f) {
        return 9;
    }
    return b->cur;
}

// 0x801C2024
int fn_801C2024(Actor* a) {
    if (a == 0) {
        return 0;
    }
    Vec3 p;
    HostBeh* b = a->beh;
    p.x = (float)a->x;
    p.y = (float)a->y;
    p.z = (float)a->z;
    Vec3Set(&a->v27C, 0.0f, 0.0f, 0.0f);
    fn_800B42E0(&p, a->id, 3, 10, 101.0f);
    if (a->extra == 0 || fn_80212730(a->id) != -1) {
        fn_801C7D78(a);
        a->pos.x = (float)a->x;
        a->pos.y = (float)a->y;
        a->pos.z = (float)a->z;
    }
    return b->cur;
}

// The action's timer ran out: drop condition 4 and run the shared timeout handler.
static inline void HostTimeout(Actor* a, HostBeh* b) {
    if (HasCondition(a->cond, 4) && !HasCondition(a->cond, 9) && !HasCondition(a->cond, 10)) {
        if (b->timer <= 0.0f) {
            ConditionSetBit(a->cond, 4, 0);
            fn_801BE5D4(a);
        }
    }
}

// 0x801C2170
int fn_801C2170(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    return b->cur;
}

// 0x801C21CC
int fn_801C21CC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    HostTimeout(a, b);
    fn_801C7D78(a);
    return b->cur;
}

// 0x801C2280
int fn_801C2280(Actor* a) {
    if (a == 0) {
        return 0;
    }
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    HostBeh* b = a->beh;
    int dir = ZoneClassifyDirection(a, &a->v240);
    if (MemFlagIsSet(a->cond, 0x17) == 0) {
        dir = 0;
    }
    if (HasCondition(a->cond, 4) && !HasCondition(a->cond, 9) && !HasCondition(a->cond, 10) &&
        b->timer <= 0.0f) {
        ConditionSetBit(a->cond, 4, 0);
        fn_801BE5D4(a);
    }
    if (b->f28 != a->unk26C || b->f24 != dir) {
        int stance = a->unk26C;
        switch (b->cur) {
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x85:
        case 0x86:
        case 0x87:
            return 0x29 + (stance << 5) + dir;
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x95:
        case 0x96:
        case 0x97:
            return 0x21 + (stance << 5) + dir;
        }
    }
    return b->cur;
}

// 0x801C2458
int fn_801C2458(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    HostTimeout(a, b);
    return b->cur;
}

// 0x801C251C
int fn_801C251C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    return b->cur;
}

// 0x801C2578
int fn_801C2578(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    int dir = ZoneClassifyDirection(a, &a->v240);
    if (MemFlagIsSet(a->cond, 0x17) == 0) {
        dir = 0;
    }
    HostTimeout(a, b);
    if (fn_8016325C(a->beh894) == 4) {
        return 0x2D + (a->unk288 << 5) + dir;
    }
    if (fn_8016325C(a->beh894) == 5) {
        return 0x31 + (a->unk288 << 5) + dir;
    }
    if (fn_8016325C(a->beh894) == 6 || fn_8016325C(a->beh894) == 7) {
        return 0x39 + (a->unk288 << 5) + dir;
    }
    if (MemFlagIsSet(a->cond, 3) == 0) {
        return 0x35 + (a->unk26C << 5) + dir;
    }
    if (HasCondition(a->cond, 0x2A)) {
        return 0x14;
    }
    if (HasCondition(a->cond, 0x2B) && MemFlagIsSet(a->cond, 6)) {
        int leap = fn_801BE008(a);
        ConditionSetBit(a->cond, 0x2B, 0);
        if (leap >= 0) {
            return leap + 0x1A;
        }
        ConditionSetBit(a->cond, 0x2C, 0);
    }
    if (b->f28 != a->unk26C) {
        return 0x29 + (a->unk26C << 5) + dir;
    }
    if (b->f24 != dir) {
        return 0x29 + (a->unk26C << 5) + dir;
    }
    return b->cur;
}

// 0x801C27D8
int fn_801C27D8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    int dir = ZoneClassifyDirection(a, &a->v240);
    if (MemFlagIsSet(a->cond, 0x17) == 0) {
        dir = 0;
    }
    HostTimeout(a, b);
    if (MemFlagIsSet(a->cond, 3) != 0) {
        return 0x25 + (a->unk26C << 5) + dir;
    }
    if (HasCondition(a->cond, 0x2A)) {
        return 0x14;
    }
    if (HasCondition(a->cond, 0x2B) && MemFlagIsSet(a->cond, 6)) {
        int leap = fn_801BE008(a);
        ConditionSetBit(a->cond, 0x2B, 0);
        if (leap >= 0) {
            return leap + 0x1A;
        }
        ConditionSetBit(a->cond, 0x2C, 0);
    }
    if (b->f28 != a->unk26C) {
        return 0x21 + (a->unk26C << 5) + dir;
    }
    if (b->f24 != dir) {
        return 0x21 + (a->unk26C << 5) + dir;
    }
    return b->cur;
}

// 0x801C29BC
int fn_801C29BC(Actor* a) {
    if (a == 0) {
        return 0x21;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    HostTimeout(a, b);
    if (HasCondition(a->cond, 0x1E)) {
        return 4;
    }
    {
        float turn = a->v27C.z;
        if (turn < 0.0f) {
            turn = -turn;
        }
        if (turn > 35.0f) {
            return 0x3D;
        }
    }
    if (HasCondition(a->cond, 0x2A)) {
        return 0x14;
    }
    if (HasCondition(a->cond, 0x2B) && MemFlagIsSet(a->cond, 6)) {
        int leap = fn_801BE008(a);
        ConditionSetBit(a->cond, 0x2B, 0);
        if (leap >= 0) {
            return leap + 0x1A;
        }
        ConditionSetBit(a->cond, 0x2C, 0);
    }
    if (a->unk26C != b->f28) {
        return 0x21 + (a->unk26C << 5);
    }
    if (MemFlagIsSet(a->cond, 3) != 0) {
        return 0x25 + (a->unk26C << 5);
    }
    if (MemFlagIsSet(a->cond, 9) != 0) {
        return b->cur;
    }
    return 0x21;
}

// 0x801C2BA8
int fn_801C2BA8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    int dir = ZoneClassifyDirection(a, &a->v240);
    if (MemFlagIsSet(a->cond, 0x17) == 0) {
        dir = 0;
    }
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    HostTimeout(a, b);
    if (__fabsf(a->v27C.z) < 1.0f) {
        return b->next + (a->unk26C << 5) + dir;
    }
    if (a->unk288 != 0) {
        return 0x3D + (a->unk26C << 5);
    }
    if (HasCondition(a->cond, 0x2A)) {
        return 0x14;
    }
    if (HasCondition(a->cond, 0x2B) && MemFlagIsSet(a->cond, 6)) {
        int leap = fn_801BE008(a);
        ConditionSetBit(a->cond, 0x2B, 0);
        if (leap >= 0) {
            return leap + 0x1A;
        }
        ConditionSetBit(a->cond, 0x2C, 0);
    }
    return b->cur;
}

// 0x801C2D68
int fn_801C2D68(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    int dir = ZoneClassifyDirection(a, &a->v240);
    if (MemFlagIsSet(a->cond, 0x17) == 0) {
        dir = 0;
    }
    HostTimeout(a, b);
    if (fn_8016325C(a->beh894) == 6 || fn_8016325C(a->beh894) == 7) {
        return 0x39 + (a->unk288 << 5) + dir;
    }
    if (HasCondition(a->cond, 0x2A)) {
        return 0x14;
    }
    if (HasCondition(a->cond, 0x2B) && MemFlagIsSet(a->cond, 6)) {
        int leap = fn_801BE008(a);
        ConditionSetBit(a->cond, 0x2B, 0);
        if (leap >= 0) {
            return leap + 0x1A;
        }
        ConditionSetBit(a->cond, 0x2C, 0);
    }
    if (fn_8016325C(a->beh894) != 5) {
        return 0x29 + (a->unk288 << 5) + dir;
    }
    if (b->f28 != a->unk26C || b->f24 != dir) {
        return 0x31 + (a->unk26C << 5) + dir;
    }
    {
        float turn = a->v27C.z;
        if (turn < 0.0f) {
        turn = -turn;
        }
        if (turn > 35.0f) {
        return 0x3D;
        }
    }
    return b->cur;
}

// 0x801C2F94
int fn_801C2F94(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    int dir = ZoneClassifyDirection(a, &a->v240);
    if (MemFlagIsSet(a->cond, 0x17) == 0) {
        dir = 0;
    }
    HostTimeout(a, b);
    if (fn_8016325C(a->beh894) == 6 || fn_8016325C(a->beh894) == 7) {
        return 0x39 + (a->unk288 << 5) + dir;
    }
    if (fn_8016325C(a->beh894) != 4) {
        return 0x29 + (a->unk288 << 5) + dir;
    }
    if (HasCondition(a->cond, 0x2A)) {
        return 0x14;
    }
    if (HasCondition(a->cond, 0x2B) && MemFlagIsSet(a->cond, 6)) {
        int leap = fn_801BE008(a);
        ConditionSetBit(a->cond, 0x2B, 0);
        if (leap >= 0) {
            return leap + 0x1A;
        }
        ConditionSetBit(a->cond, 0x2C, 0);
    }
    if (b->f28 != a->unk26C) {
        return 0x2D + (a->unk26C << 5) + dir;
    }
    if (b->f24 != dir) {
        return 0x2D + (a->unk26C << 5) + dir;
    }
    {
        float turn = a->v27C.z;
        if (turn < 0.0f) {
            turn = -turn;
        }
        if (turn > 35.0f) {
            return 0x3D;
        }
    }
    return b->cur;
}

// 0x801C31D0
int fn_801C31D0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    int dir = ZoneClassifyDirection(a, &a->v240);
    if (MemFlagIsSet(a->cond, 0x17) == 0) {
        dir = 0;
    }
    HostTimeout(a, b);
    if (fn_8016325C(a->beh894) == 6 || fn_8016325C(a->beh894) == 7) {
        return 0x39 + (a->unk288 << 5) + dir;
    }
    if (HasCondition(a->cond, 0x2A)) {
        return 0x14;
    }
    if (HasCondition(a->cond, 0x2B) && MemFlagIsSet(a->cond, 6)) {
        int leap = fn_801BE008(a);
        ConditionSetBit(a->cond, 0x2B, 0);
        if (leap >= 0) {
            return leap + 0x1A;
        }
        ConditionSetBit(a->cond, 0x2C, 0);
    }
    if (fn_8016325C(a->beh894) != 5) {
        return 0x29 + (a->unk26C << 5) + dir;
    }
    if (a->unk26C != 0) {
        return 0x31 + (a->unk26C << 5) + dir;
    }
    {
        float turn = a->v27C.z;
        if (turn < 0.0f) {
            turn = -turn;
        }
        if (turn > 35.0f) {
            return 0x3D;
        }
    }
    return b->cur;
}

// 0x801C33EC
int fn_801C33EC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    int dir = ZoneClassifyDirection(a, &a->v240);
    if (MemFlagIsSet(a->cond, 0x17) == 0) {
        dir = 0;
    }
    HostTimeout(a, b);
    if (HasCondition(a->cond, 0x2A)) {
        return 0x14;
    }
    if (HasCondition(a->cond, 0x2B) && MemFlagIsSet(a->cond, 6)) {
        int leap = fn_801BE008(a);
        ConditionSetBit(a->cond, 0x2B, 0);
        if (leap >= 0) {
            return leap + 0x1A;
        }
        ConditionSetBit(a->cond, 0x2C, 0);
    }
    if (a->unk26C != 0) {
        return 0x2D + (a->unk26C << 5) + dir;
    }
    if (fn_8016325C(a->beh894) == 6 || fn_8016325C(a->beh894) == 7) {
        return 0x39 + (a->unk288 << 5) + dir;
    }
    if (fn_8016325C(a->beh894) != 4) {
        return 0x29 + (a->unk26C << 5) + dir;
    }
    {
        float turn = a->v27C.z;
        if (turn < 0.0f) {
            turn = -turn;
        }
        if (turn > 35.0f) {
            return 0x3D;
        }
    }
    return b->cur;
}

// 0x801C3608
int fn_801C3608(Actor* a) {
    if (a == 0) {
        return 0x21;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    HostTimeout(a, b);
    int dir = ZoneClassifyDirection(a, &a->v240);
    if (MemFlagIsSet(a->cond, 0x17) == 0) {
        dir = 0;
    }
    if (fn_8016325C(a->beh894) == 4) {
        return 0x2D + (a->unk288 << 5) + dir;
    }
    if (fn_8016325C(a->beh894) == 5) {
        return 0x31 + (a->unk288 << 5) + dir;
    }
    if (fn_8016325C(a->beh894) == 6 || fn_8016325C(a->beh894) == 7) {
        return 0x39 + (a->unk288 << 5) + dir;
    }
    if (MemFlagIsSet(a->cond, 3) == 0) {
        return 0x35 + (a->unk26C << 5) + dir;
    }
    if (HasCondition(a->cond, 0x2A)) {
        return 0x14;
    }
    if (HasCondition(a->cond, 0x2B) && MemFlagIsSet(a->cond, 6)) {
        int leap = fn_801BE008(a);
        ConditionSetBit(a->cond, 0x2B, 0);
        if (leap >= 0) {
            return leap + 0x1A;
        }
        ConditionSetBit(a->cond, 0x2C, 0);
    }
    if (a->unk26C != 0) {
        return 0x29 + (a->unk26C << 5) + dir;
    }
    {
        float turn = a->v27C.z;
        if (turn < 0.0f) {
            turn = -turn;
        }
        if (turn > 35.0f) {
            return 0x3D;
        }
    }
    if (b->cur != 0xD && MemFlagIsSet(a->cond, 0x12) != 0) {
        return 0xD;
    }
    if (b->cur != 0xE && MemFlagIsSet(a->cond, 0x13) != 0) {
        return 0xE;
    }
    if (b->cur != 0xF && MemFlagIsSet(a->cond, 0x14) != 0) {
        return 0xF;
    }
    return b->cur;
}

// 0x801C38E8
int fn_801C38E8(Actor* a) {
    float step = a->dt / a->f91C / fn_802A2080();
    if (a == 0) {
        return 0x13;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    b->f30 -= step;
    if (b->f30 <= 0.0f) {
        b->f30 = 30000.0f * ((float)rand() / 32767.0f) + 8500.0f;
        return 0x21;
    }
    return b->cur;
}

// 0x801C39BC
int fn_801C39BC(Actor* a) {
    return 0x12;
}

// 0x801C39C4
int fn_801C39C4(Actor* a) {
    if (a == 0) {
        return 0;
    }
    return a->beh->cur;
}

// 0x801C3E34
int fn_801C3E34(Actor* a) {
    if (a == 0) {
        return 0x21;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    if (HasCondition(a->cond, 0x2A) == 0) {
        return 0x19;
    }
    HostTimeout(a, b);
    ZoneClassifyDirection(a, &a->v240);
    MemFlagIsSet(a->cond, 0x17);
    if (fn_8016325C(a->beh894) == 4) {
        return 0x16;
    }
    if (fn_8016325C(a->beh894) == 5) {
        return 0x17;
    }
    if (fn_8016325C(a->beh894) == 6 || fn_8016325C(a->beh894) == 7) {
        return 0x15;
    }
    return b->cur;
}

// 0x801C3F84
int fn_801C3F84(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    ZoneClassifyDirection(a, &a->v240);
    MemFlagIsSet(a->cond, 0x17);
    HostTimeout(a, b);
    if (fn_8016325C(a->beh894) == 6 || fn_8016325C(a->beh894) == 7) {
        return 0x15;
    }
    if (fn_8016325C(a->beh894) != 4) {
        return 0x18;
    }
    if (HasCondition(a->cond, 0x2A) != 0) {
        return b->cur;
    }
    return 0x19;
}

// 0x801C40BC
int fn_801C40BC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    HostBeh* b = a->beh;
    HostTimeout(a, b);
    return b->cur;
}

// 0x801C4180
int fn_801C4180(Actor* a) {
    if (a == 0) {
        return 0;
    }
    fn_801C7D78(a);
    if (a->unk220 == 4) {
        return 0x20;
    }
    HostBeh* b = a->beh;
    HostTimeout(a, b);
    return b->cur;
}

// 0x801C4244
int fn_801C4244(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 31;
    b->next = 41;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 1);
    return 1;
}

// 0x801C42C0
int fn_801C42C0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 30;
    b->next = 41;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C433C
int fn_801C433C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 29;
    b->next = 41;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 1);
    return 1;
}

// 0x801C43B8
int fn_801C43B8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 28;
    b->next = 41;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C4434
int fn_801C4434(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 27;
    b->next = 41;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 1);
    return 1;
}

// 0x801C44B0
int fn_801C44B0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 26;
    b->next = 41;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C452C
int fn_801C452C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 25;
    b->next = 33;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C45A8
int fn_801C45A8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 24;
    b->next = 24;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C4620
int fn_801C4620(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 23;
    b->next = 23;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C4698
int fn_801C4698(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 22;
    b->next = 22;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C4710
int fn_801C4710(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 21;
    b->next = 24;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C478C
int fn_801C478C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 20;
    b->next = 24;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C4808
int fn_801C4808(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 15;
    b->next = 33;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, -1);
    return 1;
}

// 0x801C4860
int fn_801C4860(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 14;
    b->next = 41;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C48DC
int fn_801C48DC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 13;
    b->next = 33;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C4934
int fn_801C4934(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 156;
    b->next = 140;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 3;
    return 1;
}

// 0x801C49B0
int fn_801C49B0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 155;
    b->next = 139;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 2;
    return 1;
}

// 0x801C4A30
int fn_801C4A30(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 154;
    b->next = 138;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 1;
    return 1;
}

// 0x801C4AB0
int fn_801C4AB0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 124;
    b->next = 108;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 3;
    return 1;
}

// 0x801C4B30
int fn_801C4B30(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 123;
    b->next = 107;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 2;
    return 1;
}

// 0x801C4BAC
int fn_801C4BAC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 122;
    b->next = 106;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 1;
    return 1;
}

// 0x801C4C2C
int fn_801C4C2C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 92;
    b->next = 76;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 3;
    return 1;
}

// 0x801C4CAC
int fn_801C4CAC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 91;
    b->next = 75;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 2;
    return 1;
}

// 0x801C4D2C
int fn_801C4D2C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 90;
    b->next = 74;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 1;
    return 1;
}

// 0x801C4DA8
int fn_801C4DA8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 140;
    b->next = 140;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 3;
    return 1;
}

// 0x801C4E04
int fn_801C4E04(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 139;
    b->next = 139;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 2;
    return 1;
}

// 0x801C4E64
int fn_801C4E64(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 138;
    b->next = 138;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 1;
    return 1;
}

// 0x801C4EC4
int fn_801C4EC4(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 108;
    b->next = 108;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 3;
    return 1;
}

// 0x801C4F24
int fn_801C4F24(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 107;
    b->next = 107;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 2;
    return 1;
}

// 0x801C4F80
int fn_801C4F80(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 106;
    b->next = 106;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 1;
    return 1;
}

// 0x801C4FE0
int fn_801C4FE0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 76;
    b->next = 76;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 3;
    return 1;
}

// 0x801C5040
int fn_801C5040(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 75;
    b->next = 75;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 2;
    return 1;
}

// 0x801C50A0
int fn_801C50A0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 74;
    b->next = 74;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 1;
    return 1;
}

// 0x801C50FC
int fn_801C50FC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 152;
    b->next = 132;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 3;
    return 1;
}

// 0x801C515C
int fn_801C515C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 151;
    b->next = 131;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 2;
    return 1;
}

// 0x801C51C0
int fn_801C51C0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 150;
    b->next = 130;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 1;
    return 1;
}

// 0x801C5224
int fn_801C5224(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 148;
    b->next = 148;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 3;
    return 1;
}

// 0x801C5280
int fn_801C5280(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 147;
    b->next = 147;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 2;
    return 1;
}

// 0x801C52E0
int fn_801C52E0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 146;
    b->next = 146;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 1;
    return 1;
}

// 0x801C5340
int fn_801C5340(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 144;
    b->next = 144;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 3;
    return 1;
}

// 0x801C539C
int fn_801C539C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 143;
    b->next = 143;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 2;
    return 1;
}

// 0x801C53FC
int fn_801C53FC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 142;
    b->next = 142;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 1;
    return 1;
}

// 0x801C545C
int fn_801C545C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 136;
    b->next = 140;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 3;
    return 1;
}

// 0x801C54BC
int fn_801C54BC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 135;
    b->next = 139;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 2;
    return 1;
}

// 0x801C5520
int fn_801C5520(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 134;
    b->next = 138;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 1;
    return 1;
}

// 0x801C5584
int fn_801C5584(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 120;
    b->next = 100;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 3;
    return 1;
}

// 0x801C55E8
int fn_801C55E8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 119;
    b->next = 99;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 2;
    return 1;
}

// 0x801C5648
int fn_801C5648(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 118;
    b->next = 98;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 1;
    return 1;
}

// 0x801C56AC
int fn_801C56AC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 116;
    b->next = 116;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 3;
    return 1;
}

// 0x801C570C
int fn_801C570C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 115;
    b->next = 115;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 2;
    return 1;
}

// 0x801C5768
int fn_801C5768(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 114;
    b->next = 114;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 1;
    return 1;
}

// 0x801C57C8
int fn_801C57C8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 112;
    b->next = 112;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 3;
    return 1;
}

// 0x801C5828
int fn_801C5828(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 111;
    b->next = 111;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 2;
    return 1;
}

// 0x801C5884
int fn_801C5884(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 110;
    b->next = 110;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 1;
    return 1;
}

// 0x801C58E4
int fn_801C58E4(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 104;
    b->next = 108;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 3;
    return 1;
}

// 0x801C5948
int fn_801C5948(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 103;
    b->next = 107;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 2;
    return 1;
}

// 0x801C59A8
int fn_801C59A8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 102;
    b->next = 106;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 1;
    return 1;
}

// 0x801C5A0C
int fn_801C5A0C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 88;
    b->next = 68;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 3;
    return 1;
}

// 0x801C5A70
int fn_801C5A70(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 87;
    b->next = 67;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 2;
    return 1;
}

// 0x801C5AD4
int fn_801C5AD4(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 86;
    b->next = 66;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 1;
    return 1;
}

// 0x801C5B34
int fn_801C5B34(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 84;
    b->next = 84;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 3;
    return 1;
}

// 0x801C5B94
int fn_801C5B94(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 83;
    b->next = 83;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 2;
    return 1;
}

// 0x801C5BF4
int fn_801C5BF4(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 82;
    b->next = 82;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 1;
    return 1;
}

// 0x801C5C50
int fn_801C5C50(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 80;
    b->next = 80;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 3;
    return 1;
}

// 0x801C5CB0
int fn_801C5CB0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 79;
    b->next = 79;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 2;
    return 1;
}

// 0x801C5D10
int fn_801C5D10(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 78;
    b->next = 78;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 1;
    return 1;
}

// 0x801C5D6C
int fn_801C5D6C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 72;
    b->next = 76;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 3;
    return 1;
}

// 0x801C5DD0
int fn_801C5DD0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 71;
    b->next = 75;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 2;
    return 1;
}

// 0x801C5E34
int fn_801C5E34(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 70;
    b->next = 74;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 1;
    return 1;
}

// 0x801C5E94
int fn_801C5E94(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 132;
    b->next = 132;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 3;
    return 1;
}

// 0x801C5EF0
int fn_801C5EF0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 131;
    b->next = 131;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 2;
    return 1;
}

// 0x801C5F50
int fn_801C5F50(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 130;
    b->next = 130;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 1;
    return 1;
}

// 0x801C5FB0
int fn_801C5FB0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 100;
    b->next = 100;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 3;
    return 1;
}

// 0x801C6010
int fn_801C6010(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 99;
    b->next = 99;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 2;
    return 1;
}

// 0x801C606C
int fn_801C606C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 98;
    b->next = 98;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 1;
    return 1;
}

// 0x801C60CC
int fn_801C60CC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 68;
    b->next = 68;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 3;
    return 1;
}

// 0x801C612C
int fn_801C612C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 67;
    b->next = 67;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 2;
    return 1;
}

// 0x801C618C
int fn_801C618C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 66;
    b->next = 66;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 1;
    return 1;
}

// 0x801C61E8
int fn_801C61E8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 6;
    b->next = 33;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, -1);
    return 1;
}

// 0x801C6264
int fn_801C6264(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 5;
    b->next = 5;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, -1);
    return 1;
}

// 0x801C62DC
int fn_801C62DC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 4;
    b->next = 5;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, -1);
    return 1;
}

// 0x801C6358
int fn_801C6358(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 3;
    b->next = 3;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, -1);
    return 1;
}

// 0x801C63D0
int fn_801C63D0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 2;
    b->next = 3;
    b->f28 = 0;
    b->f24 = 0;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C645C
int fn_801C645C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 1;
    b->next = 33;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C64D8
int fn_801C64D8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 12;
    b->next = 33;
    b->f24 = 0;
    ConditionSetBit(a->cond, 11, 0);
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C6548
int fn_801C6548(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 11;
    b->next = 12;
    b->f24 = 0;
    b->f34 = 10000.0f;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C65A4
int fn_801C65A4(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 10;
    b->next = 11;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C65F8
int fn_801C65F8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 9;
    b->next = 33;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C664C
int fn_801C664C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 8;
    b->next = 8;
    b->f24 = 0;
    b->f34 = 10000.0f;
    return 1;
}

// 0x801C6684
int fn_801C6684(Actor* a) {
    if (a == 0) {
        return 0;
    }
    Vec3 zero;
    HostBeh* b = a->beh;
    b->cur = 7;
    b->next = 8;
    b->f24 = 0;
    b->f30 = 0.0f;
    a->unk220 = 0;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    HostHalt(a, &zero);
    fn_801C7D78(a);
    if (a->extra != 0) {
        fn_800902F0(a->anim);
        ExtraTrailAttach(a->extra);
        if (a->obj5F4 != 0) {
            fn_801D55CC(a->obj5F4);
            a->obj5F4 = 0;
        }
    } else {
        fn_801C8934(a, 0);
    }
    ConditionSetBit(a->cond, 0xB, 1);
    return 1;
}

// 0x801C67AC
int fn_801C67AC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 32;
    b->next = 32;
    b->f24 = 0;
    return 1;
}

// 0x801C67DC
int fn_801C67DC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    Vec3 zero;
    HostBeh* b = a->beh;
    if (b->cur != 0x13) {
        b->f30 = 6000.0f;
    }
    b->cur = 0x13;
    b->next = 0x13;
    b->f24 = 0;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    HostHalt(a, &zero);
    ConditionSetBit(a->cond, 0xE, 0);
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C68CC
int fn_801C68CC(Actor* a) {
    Vec3 zero;
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 0x12;
    b->next = 0x12;
    b->f24 = 0;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    HostHalt(a, &zero);
    return 1;
}

// 0x801C698C
int fn_801C698C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    Vec3 zero;
    HostBeh* b = a->beh;
    b->cur = 0x11;
    b->next = 0x12;
    b->f24 = 0;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    HostHalt(a, &zero);
    if (a->extra != 0 && ExtraGetId(a->extra) != 0) {
        ExtraTrailRelease(a->extra);
    }
    fn_801C8934(a, HasCondition(a->cond, 0xB) != 0);
    fn_801C7D78(a);
    return 1;
}

// 0x801C6A98
int fn_801C6A98(Actor* a) {
    if (a == 0) {
        return 0;
    }
    Vec3 zero;
    HostBeh* b = a->beh;
    b->cur = 0x10;
    b->next = 0x21;
    b->f24 = 0;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    HostHalt(a, &zero);
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C6B68
int fn_801C6B68(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    a->cb10C = (void*)RangeResetFields;
    b->cur = 0x40;
    b->next = 0x40;
    b->f30 = sDeathTime;
    if (a->extra == 0 || fn_80212730(a->id) != -1) {
        fn_8008DE4C(a->anim, 0, b->anim, 0x18);
    }
    if (HasCondition(a->cond, 10) == 0) {
        ConditionSetBit(a->cond, 10, 1);
        ActorPlayAnim(a->id, 0xDC04E3D5, a->id);
    }
    return 1;
}

// 0x801C6C34
int fn_801C6C34(Actor* a) {
    float zero = 0.0f;
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 0x3F;
    b->next = 0x40;
    b->f30 = zero;
    a->unk220 = 0;
    ConditionSetBit(a->cond, 0, 1);
    ConditionSetBit(a->cond, 9, 1);
    if (a->extra == 0 || fn_80212730(a->id) != -1) {
        int v;
        if (a->hitBone == -1) {
            v = 8;
        } else {
            int c = ModelBoneNodeChain(a->model, ModelBoneName(a->model, a->hitBone), gHostAssets, 0x10);
            if (c == -1) {
                v = 8;
            } else {
                v = c;
            }
        }
        fn_801C8934(a, v);
    }
    return 1;
}

// 0x801C6D1C
int fn_801C6D1C(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    if (b->cur != 0x3E) {
        b->next = b->cur;
        b->cur = 0x3E;
    }
    b->timer = 4000.0f;
    ConditionSetBit(a->cond, 0, 1);
    {
        int v;
        if (a->hitBone == -1) {
            v = 8;
        } else {
            int c = ModelBoneNodeChain(a->model, ModelBoneName(a->model, a->hitBone), gHostAssets, 0x10);
            if (c == -1) {
                v = 8;
            } else {
                v = c;
            }
        }
        fn_801C8934(a, v);
    }
    ActorPlayAnim(OwnerId((Owner*)a), 0xA464660E, OwnerId((Owner*)a));
    return 1;
}

// 0x801C6E04
int fn_801C6E04(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 137;
    b->next = 137;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 0;
    return 1;
}

// 0x801C6E64
int fn_801C6E64(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 105;
    b->next = 105;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 0;
    return 1;
}

// 0x801C6EC4
int fn_801C6EC4(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 73;
    b->next = 73;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 0;
    return 1;
}

// 0x801C6F24
int fn_801C6F24(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 129;
    b->next = 129;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 0;
    return 1;
}

// 0x801C6F84
int fn_801C6F84(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 97;
    b->next = 97;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 0;
    return 1;
}

// 0x801C6FE4
int fn_801C6FE4(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 65;
    b->next = 65;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 0;
    return 1;
}

// 0x801C7044
int fn_801C7044(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 149;
    b->next = 129;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 0;
    return 1;
}

// 0x801C70A8
int fn_801C70A8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 145;
    b->next = 145;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 0;
    return 1;
}

// 0x801C7108
int fn_801C7108(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 141;
    b->next = 141;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 0;
    return 1;
}

// 0x801C7168
int fn_801C7168(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 133;
    b->next = 137;
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 0;
    return 1;
}

// 0x801C71CC
int fn_801C71CC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 117;
    b->next = 97;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 0;
    return 1;
}

// 0x801C7230
int fn_801C7230(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 113;
    b->next = 113;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 0;
    return 1;
}

// 0x801C7290
int fn_801C7290(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 109;
    b->next = 109;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 0;
    return 1;
}

// 0x801C72F0
int fn_801C72F0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 101;
    b->next = 105;
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 0;
    return 1;
}

// 0x801C7354
int fn_801C7354(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 85;
    b->next = 65;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 0;
    return 1;
}

// 0x801C73B8
int fn_801C73B8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 81;
    b->next = 81;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 0;
    return 1;
}

// 0x801C7418
int fn_801C7418(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 77;
    b->next = 77;
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 0;
    return 1;
}

// 0x801C7478
int fn_801C7478(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 69;
    b->next = 73;
    b->f24 = 0;
    fn_801C8934(a, 0);
    b->f28 = 1;
    return 1;
}

// 0x801C74DC
int fn_801C74DC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 53;
    b->next = 33;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C7554
int fn_801C7554(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 37;
    b->next = 41;
    b->f24 = 0;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C75CC
int fn_801C75CC(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 0);
    if (MemFlagIsSet(a->cond, 3) != 0) {
        b->next = 0x29;
    } else {
        b->next = 0x21;
    }
    b->cur = 0x3D + (a->unk26C << 5);
    b->f24 = 0;
    if (a->v27C.z > 0.0f) {
        if (MemFlagIsSet(a->cond, 3) != 0) {
            b->animOverride = 0x22B02743;
        } else {
            b->animOverride = 0xFF45A5A5;
        }
    } else {
        if (MemFlagIsSet(a->cond, 3) != 0) {
            b->animOverride = 0xA1B7BA66;
        } else {
            b->animOverride = 0xDAB4FC08;
        }
    }
    fn_801C8934(a, 0);
    return 1;
}

// Picks a new random animation variant for a repeated action (never the same one twice in a row when
// the action repeats).
static inline void HostPickVariant(Actor* a, HostBeh* b, int action) {
    int n = AnimBankGetSetCount(a->data->i14, a->data->i1C, gHostActions[b->cur].anim);
    if (n > 1) {
        if (b->cur != action) {
            b->variant = fn_8000DDD0() % n;
        } else if ((float)rand() / 32767.0f > 0.9f) {
            int v = fn_8000DDD0() % (n - 1);
            if (v >= b->variant) {
                v = (v + 1) % n;
            }
            b->variant = v;
        }
    } else {
        b->variant = 0;
    }
}

// 0x801C76E0
int fn_801C76E0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 153;
    b->next = 137;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 3;
    b->f24 = 0;
    return 1;
}

// 0x801C7760
int fn_801C7760(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 121;
    b->next = 105;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 2;
    b->f24 = 0;
    return 1;
}

// 0x801C77E0
int fn_801C77E0(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 89;
    b->next = 73;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    b->f28 = 1;
    b->f24 = 0;
    return 1;
}

// 0x801C7860
int fn_801C7860(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 57;
    b->next = 41;
    b->f28 = 0;
    ConditionSetBit(a->cond, 0, 1);
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C78E8
int fn_801C78E8(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    HostPickVariant(a, b, 0x31);
    b->cur = 0x31;
    b->next = 0x31;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, b->variant);
    return 1;
}

// 0x801C7A30
int fn_801C7A30(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    HostPickVariant(a, b, 0x2D);
    b->cur = 0x2D;
    b->next = 0x2D;
    b->f28 = 0;
    b->f24 = 0;
    fn_801C8934(a, b->variant);
    return 1;
}

// Enters the idle action (0x21).
static inline int HostEnterIdle(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 0x21;
    b->next = 0x21;
    b->f24 = 0;
    b->f28 = 0;
    b->animOverride = 0xA8BE217B;
    fn_801C8934(a, 0);
    return 1;
}

// 0x801C7B78
int fn_801C7B78(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    ConditionSetBit(a->cond, 0, 1);
    b->cur = 41;
    b->next = 41;
    b->f28 = 0;
    fn_801C8934(a, 0);
    b->f24 = 0;
    return 1;
}

// 0x801C7BF4
int fn_801C7BF4(Actor* a) {
    if (a == 0) {
        return 0;
    }
    HostBeh* b = a->beh;
    b->cur = 0;
    b->next = 0x21;
    b->f28 = 0;
    ConditionSetBit(a->cond, 0, 1);
    if (AnimBankGetSetCount(a->data->i14, a->data->i1C, 0x2237D4) != 0) {
        b->animOverride = 0x2237D4;
        fn_801C8934(a, -1);
    } else {
        HostEnterIdle(a);
    }
    b->f24 = 0;
    return 1;
}

// 0x801C7CF4
int fn_801C7CF4(Actor* a) {
    return HostEnterIdle(a);
}

// 0x801C8934: plays the animation of the current action (variant < 0: a random one).
void fn_801C8934(Actor* a, int variant) {
    float zero = 0.0f;
    if (a != 0) {
        HostBeh* b = a->beh;
        u32 anim;
        int h;
        b->motion.x = zero;
        b->motion.y = zero;
        b->motion.z = zero;
        if (HasCondition(a->cond, 0xC)) {
            ConditionSetBit(a->cond, 0xC, 0);
            ConditionSetBit(a->cond, 0xE, 1);
            fn_8008F4E8(a->anim, 0);
            fn_8008F378(a->anim);
            fn_8008F4EC(a->anim, 0);
        }
        if (b->animOverride != 0) {
            anim = b->animOverride;
            b->animOverride = 0;
        } else {
            anim = gHostActions[b->cur].anim;
        }
        if (variant < 0) {
            int n = AnimBankGetSetCount(a->data->i14, a->data->i1C, anim);
            if (n == 0) {
                return;
            }
            variant = rand() % n;
        }
        h = fn_800A1BC8(a->data->i14, a->data->i1C, anim, variant, 0);
        if (b->cur == 0x11 && ActorAnimState(a->id, 0x2A371672) > 0) {
            int r = ActorPlayAnim(a->id, 0x2A371672, a->id);
            if (r != -1) {
                h = AssetFind(0x2FA2416F, r);
            }
        }
        if (fn_8008E7D0(h)) {
            ConditionSetBit(a->cond, 0x1C, 0);
            if (HasCondition(a->cond, 0xE)) {
                ConditionSetBit(a->cond, 0xE, 0);
                fn_8008F230(a->anim, 0.0f, 0, 0);
            } else {
                fn_8008F230(a->anim, 150.0f, 0, 1);
            }
            fn_80090974(a->anim, 0, h, 1.0f, 9, gHostActions[b->cur].event, a);
            b->animVar = b->f28;
            b->anim = h;
        }
    }
}

}  // extern "C"

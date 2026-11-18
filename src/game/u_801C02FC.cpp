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
    int animOverride;   // 0x18
    int f1C;            // 0x1C
    int f20;            // 0x20
    int f24;            // 0x24
    int f28;            // 0x28
    float timer;        // 0x2C
    float f30;          // 0x30
    float f34;          // 0x34
    float f38;          // 0x38
    Vec3 motion;        // 0x3C
    char pad48[0xA];    // 0x48
    short f52;          // 0x52
};

// The host's per-type definition data (at actor+0xC4).
struct HostData {
    u32 type;           // 0x00
    char pad4[0x10];
    int i14;            // 0x14
    int i18;            // 0x18
    int i1C;            // 0x1C
};

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
    char padC8[0xC];
    int anim;           // 0xD4
    char padD8[0x38];
    void* cb110;        // 0x110
    char pad114[4];
    void* cb118;        // 0x118
    char pad11C[0x18];
    HostBeh* beh;       // 0x134
    float dt;           // 0x138
    char pad13C[0xE4];
    int unk220;         // 0x220
    char pad224[0x1C];
    Vec3 v240;          // 0x240
    char pad24C[0x20];
    int unk26C;         // 0x26C
    char cond[0xC];     // 0x270
    Vec3 v27C;          // 0x27C: turn (z at 0x284)
    int unk288;         // 0x288
    char pad28C[0x364];
    void* unk5F0;       // 0x5F0
    char pad5F4[0x2A0];
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
int rand(void);

// 0x801C11B0
void fn_801C11B0(Actor* a) {
    a->cb110 = 0;
    VFree(a->beh);
    a->beh = 0;
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
    if (a->unk5F0 == 0 || fn_80212730(a->id) != -1) {
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
    if (a->unk5F0 == 0 || fn_80212730(a->id) != -1) {
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
    {
        int stance = a->unk26C;
        if (b->f28 == stance && b->f24 == dir) {
            return b->cur;
        }
        return 0x29 + (stance << 5) + dir;
    }
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
    {
        int stance = a->unk26C;
        if (b->f28 == stance && b->f24 == dir) {
            return b->cur;
        }
        return 0x21 + (stance << 5) + dir;
    }
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

// 0x801C7CF4
int fn_801C7CF4(Actor* a) {
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

}  // extern "C"

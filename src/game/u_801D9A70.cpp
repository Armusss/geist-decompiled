// u_801D9A70.cpp: a generic creature AI (state table at 0x803ADBF8) plus trail effect helpers
// (0x801D9A70-0x801E0EDC).  Per-state enter / can-leave / update callbacks, message handling,
// attack and approach logic, and the trail create / destroy wrappers.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct Creature;

// AI state record (0x1C bytes) at Creature::ai.
struct CreatureAi {
    int state;              // 0x00
    int pending;            // 0x04
    float time;             // 0x08
    u32 flags;              // 0x0C
    float t10;              // 0x10
    float t14;              // 0x14
    float health18;         // 0x18
};

// One row of the state table (16 bytes).
struct CreatureOps {
    int (*enter)(Creature* c);              // 0x00
    int (*canLeave)(Creature* c);           // 0x04
    int (*update)(Creature* c);             // 0x08
    void* unk0C;                            // 0x0C
};

struct Creature {
    u8 pad000[0xFC];
    void* fnFC;             // 0x0FC
    u8 pad100[4];
    void* update104;        // 0x104
    u8 pad108[0x28];
    CreatureAi* ai;         // 0x130
    u8 pad134[4];
    float dt;               // 0x138
    u8 pad13C[0xE4];
    int unk220;             // 0x220
    u8 pad224[0x44];
    int hp;                 // 0x268
    int unk26C;             // 0x26C
    u8 flags270[0x18];      // 0x270
    int unk288;             // 0x288
    u8 pad28C[0x38];
    int maxHp;              // 0x2C4
    u8 pad2C8[0x334];
    void* fn5FC;            // 0x5FC
    void* fn600;            // 0x600
    void* fn604;            // 0x604
    u8 pad608[0x28C];
    u8 sub894[0x70];        // 0x894
    float health;           // 0x904
};

extern CreatureOps lbl_803ADBF8[];
extern float lbl_804D8CA8;
extern float lbl_804D8CD0;
extern float lbl_804D8CD4;
extern int lbl_804D3240;

extern "C" {
int fn_800A7AC4(void* set, int i);
void fn_800A7A54(void* set, int i, int v);
void fn_800A7B08(void* set, int i, int v);
void fn_800A7028(void* set);
int fn_800A7A18(void* set);
int fn_800BEF9C(int v);
int fn_800BAF40(Creature* c);
void fn_80162658(Creature* c, void* p);
int fn_801DDEF0(Creature* c);
void VFree(void* p);
void* VAlloc(int size);
int fn_8007EB34(u32 a, u32 b);
void fn_800CE96C(void);
void fn_800CF064(void);
void fn_800CC984(void);
int fn_800B26B8(Creature* c);
int fn_800B1EF4(Creature* c, int a);
void fn_800B261C(Creature* c, int a, int* out);
int fn_800B22F8(Creature* c, int a);
}



// Per-frame update: runs the current state's update, switches state when allowed.
int CreatureUpdate(Creature* c) {
    CreatureAi* ai;
    int changed;
    if (c == NULL)
        return 0;
    ai = c->ai;
    if (fn_800A7AC4(&c->flags270, 9) == 0 && fn_800A7AC4(&c->flags270, 10) == 0 && fn_800A7AC4(&c->flags270, 0xB) == 0) {
        if (fn_800BEF9C(c->maxHp) < c->hp)
            c->hp = fn_800BEF9C(c->maxHp);
        changed = lbl_803ADBF8[ai->state].update(c);
        if (fn_800BAF40(c) == -1) {
            c->unk288 = 0;
            c->unk26C = 0;
        }
        ai->time += c->dt;
        fn_80162658(c, c->sub894);
        if (changed != 0 || (ai->flags & 1) != 0) {
            int next;
            ai->flags &= ~1;
            next = fn_801DDEF0(c);
            if (next != ai->state && lbl_803ADBF8[ai->state].canLeave(c) != 0) {
                if (lbl_803ADBF8[next].enter(c) == 0)
                    lbl_803ADBF8[ai->state].enter(c);
                ai->time = lbl_804D8CA8;
                fn_800A7A54(&c->flags270, 0x40, 0);
                fn_800A7A54(&c->flags270, 0x41, 0);
                fn_800A7A54(&c->flags270, 0x43, 0);
                fn_800A7A54(&c->flags270, 0x42, 0);
            }
        }
        return 1;
    }
    fn_80162658(c, c->sub894);
    return 1;
}

void CreatureFree(Creature* c) {
    c->update104 = NULL;
    VFree(c->ai);
    c->ai = NULL;
}

void CreatureInit(Creature* c) {
    CreatureAi* ai;
    c->update104 = (void*)CreatureUpdate;
    ai = (CreatureAi*)VAlloc(sizeof(CreatureAi));
    if (ai == NULL) {
        c->ai = NULL;
    } else {
        c->ai = ai;
        c->fnFC = (void*)fn_800CE96C;
        c->fn604 = (void*)fn_800CF064;
        c->fn5FC = (void*)fn_800CC984;
        c->fn600 = (void*)fn_800CC984;
        ai = c->ai;
        ai->state = 0;
        ai->pending = 0;
        ai->flags = 0;
        ai->t10 = lbl_804D8CA8;
        ai->t14 = lbl_804D8CA8;
        ai->time = lbl_804D8CA8;
        ai->health18 = c->health;
        fn_800A7B08(&c->flags270, 6, 1);
        lbl_803ADBF8[ai->state].enter(c);
        lbl_804D3240 = fn_8007EB34(0xF1754BF2, 0x21C5E0);
    }
}

// Decides whether the creature may leave its idle state to attack.
int CreatureWantsAttack(Creature* c) {
    int pick[1];
    pick[0] = 0;
    if (c == NULL)
        return 0;
    {
        CreatureAi* ai = c->ai;
        if (c->unk220 == 0 || ai->time <= lbl_804D8CD0) {
            int slot = fn_800B26B8(c);
            if (slot != 4 && fn_800B1EF4(c, slot) == 0) {
                fn_800B261C(c, slot, pick);
                if (pick[0] != fn_800A7A18(&c->flags270) && pick[0] != 0)
                    return 1;
                if (fn_800B22F8(c, slot) < 2)
                    return 1;
                if (lbl_804D8CD4 < ai->time)
                    return 1;
            }
            if (slot < 3 || (fn_800A7028(&c->flags270), ai->time <= lbl_804D8CD4))
                return 0;
            return 1;
        }
        return 1;
    }
}

bool CreatureClearFlags(Creature* c) {
    if (c != NULL) {
        fn_800A7028(&c->flags270);
        fn_800A7B08(&c->flags270, 4, 0);
        fn_800A7B08(&c->flags270, 9, 0);
        fn_800A7B08(&c->flags270, 0x16, 0);
    }
    return c != NULL;
}

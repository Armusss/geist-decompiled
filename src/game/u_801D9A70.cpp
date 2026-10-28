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
struct TrailDesc;

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
void* fn_800A7A18(void* set);
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
void fn_800B261C(Creature* c, int a, void** out);
int fn_800B22F8(Creature* c, int a);
}



extern "C" {
void fn_800B1F20(Creature* c, int slot);
void fn_800A78A0(void* set, void* target);
int fn_800C5654(int to, u32 msg, int from);
void fn_800B1AB8(double a, Creature* c, int slot, int n);
void fn_800B1DCC(Creature* c, int slot, void* cb, void* target, int n);
int SendMsg(int to, int msg, void* data);
}
extern float lbl_804D8CD8;
int CreatureChargeCallback(Creature* c);

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    virtual void Release(char* p);
    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();
    String(const char* s);
    ~String() {}
    virtual void Release(char* p);
};

struct QueryMsg {
    int a;
    u32 hash;
    int c;
    String text;
};

extern "C" {
int fn_800A87A0(Creature* c);
u32 fn_800B1ED4(Creature* c, int slot);
double fn_800AA050(Creature* c, void* target);
int fn_800A7B78(void* set, int i);
void fn_800BBCA8(Creature* c);
void fn_800BC56C(Creature* c, void* target);
}
extern float lbl_804D8CDC;
int CreatureShootCallback(Creature* c);

extern float lbl_804D8CB4;

extern float lbl_804D8CAC;
extern float lbl_804D8CB8;
extern "C" {
void fn_800B2418(double a, Creature* c, int slot);
int fn_800A8CDC(double a, Creature* c, void* target);
void fn_800DDA24(int actor, int v);
void fn_800BC5A8(Creature* c, Vec3* at);
}
int CreatureSeekCallback(Creature* c);

// TODO (not drafted yet, 0xBD0 bytes at 0x801D9A70)
extern "C" void fn_801D9A70(void);

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
    void* pick[1];
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

int CreatureClearFlags(Creature* c) {
    if (c == NULL)
        return 0;
    fn_800A7028(&c->flags270);
    fn_800A7B08(&c->flags270, 4, 0);
    fn_800A7B08(&c->flags270, 9, 0);
    fn_800A7B08(&c->flags270, 0x16, 0);
    return 1;
}

// Starts the retreat state when the target slot has someone to flee from.
int CreatureStartRetreat(Creature* c) {
    CreatureAi* ai;
    int slot;
    void* who[1];
    if (c == NULL)
        return 0;
    ai = c->ai;
    slot = fn_800B26B8(c);
    if (slot == 4)
        return 0;
    fn_800B261C(c, slot, who);
    if (who[0] == 0)
        return 0;
    fn_800B1F20(c, slot);
    fn_800A78A0(&c->flags270, who[0]);
    c->unk288 = 0;
    c->hp = 3;
    ai->state = 15;
    ai->t10 = lbl_804D8CA8;
    ai->time = lbl_804D8CA8;
    fn_800A7B08(&c->flags270, 4, 1);
    fn_800A7B08(&c->flags270, 9, 1);
    fn_800A7B08(&c->flags270, 0x16, 1);
    return 1;
}

bool CreatureShouldLeave(Creature* c) {
    CreatureAi* ai;
    int slot;
    void* who[1];
    if (c == NULL)
        return false;
    ai = c->ai;
    slot = fn_800B26B8(c);
    if (slot != 4) {
        fn_800B261C(c, slot, who);
        if (who[0] != fn_800A7A18(&c->flags270))
            return true;
        if (fn_800B22F8(c, slot) < 3)
            return true;
    }
    return lbl_804D8CD4 < ai->time;
}

int CreatureClearFlags2(Creature* c) {
    if (c == NULL)
        return 0;
    fn_800A7028(&c->flags270);
    fn_800A7B08(&c->flags270, 4, 0);
    fn_800A7B08(&c->flags270, 9, 0);
    return 1;
}

// Starts the charge attack against the nearest target in range.
int CreatureStartCharge(Creature* c) {
    CreatureAi* ai;
    int slot;
    void* target;
    if (c == NULL)
        return 0;
    ai = c->ai;
    slot = fn_800B26B8(c);
    if (slot == 4)
        return 0;
    {
        void* who[1];
        fn_800B261C(c, slot, who);
        target = (int*)who[0];
    }
    if (target == NULL)
        return 0;
    fn_800B1F20(c, slot);
    if (fn_800C5654(*(int*)((u8*)c + 0x38), 0xB1C7633C, *(int*)((u8*)target + 0x38)) != 0)
        return 0;
    fn_800B1AB8(lbl_804D8CD8, c, slot, 3);
    fn_800A78A0(&c->flags270, target);
    c->unk288 = 0;
    c->hp = 3;
    ai->state = 11;
    ai->t10 = lbl_804D8CA8;
    fn_800A7B08(&c->flags270, 4, 1);
    fn_800A7B08(&c->flags270, 9, 1);
    {
        QueryMsg q;
        q.a = 0;
        q.hash = 0xF9D0235B;
        q.c = 0;
        SendMsg(*(int*)((u8*)c + 0x38), 0x41D, &q);
    }
    fn_800B1DCC(c, slot, (void*)CreatureChargeCallback, target, 0x28);
    if (c->unk220 == 4)
        c->unk220 = 0;
    return 1;
}

u32 CreatureCanChase(Creature* c) {
    CreatureAi* ai;
    int slot;
    void* who[1];
    u32 r;
    if (c == NULL)
        return 0;
    ai = c->ai;
    slot = fn_800B26B8(c);
    if (slot == 4)
        return 1;
    fn_800B261C(c, slot, who);
    if (who[0] == fn_800A7A18(&c->flags270)) {
        if (fn_800B22F8(c, slot) < 3)
            return 1;
        r = lbl_804D8CD4 < ai->time;
    } else {
        r = 1;
    }
    return r;
}

int CreatureClearFlags3(Creature* c) {
    if (c == NULL)
        return 0;
    fn_800A7028(&c->flags270);
    fn_800A7B08(&c->flags270, 4, 0);
    fn_800A7B08(&c->flags270, 9, 0);
    fn_800A7B08(&c->flags270, 0x16, 0);
    return 1;
}

// Starts the ranged attack state.
int CreatureStartShoot(Creature* c) {
    CreatureAi* ai;
    int slot;
    void* who[1];
    if (c == NULL)
        return 0;
    ai = c->ai;
    slot = fn_800B26B8(c);
    if (slot == 4)
        return 0;
    fn_800B261C(c, slot, who);
    if (who[0] == 0)
        return 0;
    if (fn_800A87A0(c) == 0)
        return 0;
    fn_800B1F20(c, slot);
    if (fn_800C5654(*(int*)((u8*)c + 0x38), 0x7C794AB6, *(int*)((u8*)who[0] + 0x38)) != 0)
        return 0;
    fn_800B1AB8(lbl_804D8CD8, c, slot, 3);
    fn_800A78A0(&c->flags270, who[0]);
    c->unk288 = 0;
    c->hp = 3;
    ai->state = 10;
    ai->t10 = lbl_804D8CA8;
    fn_800A7B08(&c->flags270, 4, 1);
    fn_800A7B08(&c->flags270, 0x16, 1);
    fn_800A7B08(&c->flags270, 9, 1);
    fn_800B1DCC(c, slot, (void*)CreatureShootCallback, who[0], 0x3C);
    if (c->unk220 == 4)
        c->unk220 = 0;
    return 1;
}

// Per-frame shooting update; returns nonzero when the state should end.
int CreatureShootUpdate(Creature* c) {
    CreatureAi* ai;
    int slot;
    void* who[1];
    if (c == NULL)
        return 0;
    ai = c->ai;
    slot = fn_800B26B8(c);
    if (slot != 4) {
        fn_800A7A18(&c->flags270);
        fn_800B261C(c, slot, who);
        if (who[0] != 0 && who[0] != fn_800A7A18(&c->flags270))
            return 1;
        if (fn_800B22F8(c, slot) < 3 && (fn_800B1ED4(c, slot) & 8) == 0)
            return 1;
    }
    who[0] = fn_800A7A18(&c->flags270);
    if (who[0] == 0)
        return 1;
    fn_800B1F20(c, slot);
    {
        double d = fn_800AA050(c, who[0]);
        if (d < lbl_804D8CDC || (ai->flags & 4) != 0) {
            if (c->unk288 == 0) {
                if ((ai->flags & 4) == 0) {
                    fn_800A7B08(&c->flags270, 4, 0);
                    if (fn_800C5654(*(int*)((u8*)c + 0x38), 0xFA658DD0, *(int*)((u8*)who[0] + 0x38)) != 0)
                        return 1;
                    fn_800A7B08(&c->flags270, 0x14, 1);
                    ai->flags |= 4;
                } else if (fn_800A7B78(&c->flags270, 0x14) == 0) {
                    return 1;
                }
            } else {
                fn_800BBCA8(c);
                c->unk288 = 0;
            }
        } else if (fn_800BAF40(c) == -1) {
            c->unk288 = 0;
        } else {
            c->unk288 = 2;
        }
    }
    return 0;
}

int CreatureStopChase(Creature* c) {
    if (c == NULL)
        return 0;
    CreatureAi* ai = c->ai;
    fn_800BBCA8(c);
    fn_800A7028(&c->flags270);
    fn_800A7B08(&c->flags270, 4, 0);
    c->unk288 = 0;
    ai->flags &= ~4;
    return 1;
}

int CreatureStartApproach(Creature* c) {
    CreatureAi* ai;
    void* target;
    if (c == NULL)
        return 0;
    ai = c->ai;
    target = fn_800A7A18(&c->flags270);
    if (target == 0)
        return 0;
    c->unk288 = 0;
    ai->state = 14;
    ai->flags &= ~4;
    fn_800A7B08(&c->flags270, 4, 1);
    fn_800BC56C(c, target);
    if (c->unk220 == 4)
        c->unk220 = 0;
    return 1;
}

// TODO (not drafted yet, 0x330 bytes at 0x801DB48C)
extern "C" void fn_801DB48C(void);

int CreatureStopShoot(Creature* c) {
    if (c == NULL)
        return 0;
    CreatureAi* ai = c->ai;
    fn_800BBCA8(c);
    fn_800A7028(&c->flags270);
    fn_800A7B08(&c->flags270, 4, 0);
    fn_800A7B08(&c->flags270, 0x16, 0);
    c->unk288 = 0;
    ai->flags &= ~4;
    return 1;
}

int CreatureStartGuard(Creature* c) {
    CreatureAi* ai;
    void* target;
    if (c == NULL)
        return 0;
    ai = c->ai;
    target = fn_800A7A18(&c->flags270);
    if (target == 0)
        return 0;
    c->unk288 = 0;
    ai->state = 12;
    ai->flags &= ~4;
    fn_800A7B08(&c->flags270, 4, 1);
    fn_800A7B08(&c->flags270, 3, 0);
    fn_800A7B08(&c->flags270, 0x16, 1);
    if (c->unk220 == 4)
        c->unk220 = 0;
    return 1;
}

// TODO (not drafted yet, 0x210 bytes at 0x801DB8FC)
extern "C" void fn_801DB8FC(void);

int CreatureEndGuard(Creature* c) {
    if (c == NULL)
        return 0;
    {
        u32 f = c->ai->flags;
        if ((f & 0x20) == 0)
            return 0;
        c->ai->flags = f & ~0x20;
        fn_800A7028(&c->flags270);
        fn_800A7B08(&c->flags270, 4, 0);
        fn_800A7B08(&c->flags270, 0x16, 0);
        return 1;
    }
}

int CreatureStartHold(Creature* c) {
    CreatureAi* ai;
    if (c == NULL)
        return 0;
    ai = c->ai;
    if (fn_800A7A18(&c->flags270) == 0)
        return 0;
    c->unk288 = 0;
    ai->state = 9;
    ai->flags &= ~0x20;
    ai->time = lbl_804D8CA8;
    fn_800A7B08(&c->flags270, 4, 1);
    fn_800A7B08(&c->flags270, 0x16, 1);
    fn_800A7B08(&c->flags270, 3, 0);
    if (c->unk220 == 4)
        c->unk220 = 0;
    return 1;
}

int CreatureHoldUpdate(Creature* c) {
    CreatureAi* ai;
    int slot;
    void* who[1];
    if (c == NULL)
        return 0;
    ai = c->ai;
    slot = fn_800B26B8(c);
    if (slot == 4)
        return 1;
    fn_800B261C(c, slot, who);
    if (who[0] == fn_800A7A18(&c->flags270) && fn_800B22F8(c, slot) > 2 && (fn_800B1ED4(c, slot) & 0x10) == 0 &&
        (ai->flags & 0x10) == 0) {
        if (lbl_804D8CB4 < ai->time) {
            fn_800B1AB8(lbl_804D8CD8, c, slot, 1);
            return 1;
        }
        return 0;
    }
    return 1;
}

int CreatureStopHold(Creature* c) {
    if (c == NULL)
        return 0;
    fn_800A7B08(&c->flags270, 4, 0);
    fn_800A7B08(&c->flags270, 3, 0);
    fn_800A7B08(&c->flags270, 0x16, 0);
    fn_800A7028(&c->flags270);
    return 1;
}

int CreatureStartSeek(Creature* c) {
    CreatureAi* ai;
    int slot;
    void* who[1];
    if (c == NULL)
        return 0;
    ai = c->ai;
    slot = fn_800B26B8(c);
    if (slot == 4)
        return 0;
    fn_800B261C(c, slot, who);
    if (who[0] == 0)
        return 0;
    if (fn_800C5654(*(int*)((u8*)c + 0x38), 0x88A615F1, *(int*)((u8*)who[0] + 0x38)) != 0) {
        ai->flags |= 0x10;
        return 0;
    }
    c->unk288 = 0;
    c->hp = 3;
    ai->state = 13;
    ai->t10 = lbl_804D8CA8;
    fn_800A7B08(&c->flags270, 4, 1);
    fn_800A7B08(&c->flags270, 3, 1);
    fn_800A7B08(&c->flags270, 0x16, 1);
    fn_800A78A0(&c->flags270, who[0]);
    fn_800B1DCC(c, slot, (void*)CreatureSeekCallback, who[0], 0x50);
    fn_800B1AB8(lbl_804D8CD8, c, slot, 3);
    fn_800DDA24(*(int*)((u8*)c + 0x38), 4);
    if (c->unk220 == 4)
        c->unk220 = 0;
    return 1;
}

int CreatureSeekUpdate(Creature* c) {
    CreatureAi* ai;
    void* target;
    int slot;
    void* who[1];
    if (c == NULL)
        return 0;
    ai = c->ai;
    target = fn_800A7A18(&c->flags270);
    slot = fn_800B26B8(c);
    if (target == 0) {
        fn_800A7028(&c->flags270);
        fn_800A7B08(&c->flags270, 4, 0);
        return 1;
    }
    if (slot < 2) {
        fn_800B22F8(c, slot);
        if (fn_800B1EF4(c, slot) == 0) {
            fn_800B261C(c, slot, who);
            if (who[0] != 0 && who[0] != target) {
                fn_800B1F20(c, slot);
                fn_800B2418(lbl_804D8CB8, c, slot);
                fn_800B1AB8(lbl_804D8CD8, c, slot, 1);
            }
        }
    } else if (fn_800B1EF4(c, slot) == 0) {
        return 1;
    }
    if (fn_800A8CDC(lbl_804D8CAC, c, target) != 0)
        return 1;
    if (fn_800BAF40(c) == -1) {
        fn_800BC56C(c, target);
        c->unk288 = 3;
        ai->time = lbl_804D8CA8;
    } else if (lbl_804D8CB8 < ai->time) {
        fn_800BC56C(c, target);
        c->unk288 = 3;
        ai->time = lbl_804D8CA8;
    }
    return 0;
}

int CreatureStartWatch(Creature* c) {
    int slot;
    void* who[1];
    if (c == NULL)
        return 0;
    c->ai->state = 8;
    fn_800A7B08(&c->flags270, 3, 1);
    slot = fn_800B26B8(c);
    if (slot == 4)
        return 0;
    fn_800B1F20(c, slot);
    fn_800B1AB8(lbl_804D8CD8, c, slot, 1);
    fn_800B261C(c, slot, who);
    if (who[0] == 0) {
        fn_800A7028(&c->flags270);
        fn_800BBCA8(c);
        c->unk288 = 0;
    } else {
        fn_800A78A0(&c->flags270, who[0]);
        fn_800BC56C(c, who[0]);
        c->unk288 = 3;
    }
    if (c->unk220 == 4)
        c->unk220 = 0;
    return 1;
}

// TODO (not drafted yet, 0x190 bytes at 0x801DC1C4)
extern "C" void fn_801DC1C4(void);

// TODO (not drafted yet, 0x34 bytes at 0x801DC354)
extern "C" void fn_801DC354(void);

// TODO (not drafted yet, 0x110 bytes at 0x801DC388)
extern "C" void fn_801DC388(void);

// TODO (not drafted yet, 0x42C bytes at 0x801DC498)
extern "C" void fn_801DC498(void);

// TODO (not drafted yet, 0x6C bytes at 0x801DC8C4)
extern "C" void fn_801DC8C4(void);

// TODO (not drafted yet, 0x108 bytes at 0x801DC930)
extern "C" void fn_801DC930(void);

// TODO (not drafted yet, 0x5C4 bytes at 0x801DCA38)
extern "C" void fn_801DCA38(void);

// TODO (not drafted yet, 0x60 bytes at 0x801DCFFC)
extern "C" void fn_801DCFFC(void);

// TODO (not drafted yet, 0x158 bytes at 0x801DD05C)
extern "C" void fn_801DD05C(void);

// TODO (not drafted yet, 0xFC bytes at 0x801DD1B4)
extern "C" void fn_801DD1B4(void);

// TODO (not drafted yet, 0xFC bytes at 0x801DD2B0)
extern "C" void fn_801DD2B0(void);

// TODO (not drafted yet, 0x138 bytes at 0x801DD3AC)
extern "C" void fn_801DD3AC(void);

// TODO (not drafted yet, 0x44 bytes at 0x801DD4E4)
extern "C" void fn_801DD4E4(void);

// TODO (not drafted yet, 0xF4 bytes at 0x801DD528)
extern "C" void fn_801DD528(void);

// TODO (not drafted yet, 0x4B0 bytes at 0x801DD61C)
extern "C" void fn_801DD61C(void);

// TODO (not drafted yet, 0xA8 bytes at 0x801DDACC)
extern "C" void fn_801DDACC(void);

// TODO (not drafted yet, 0x4C bytes at 0x801DDB74)
extern "C" void fn_801DDB74(void);

// TODO (not drafted yet, 0x1F0 bytes at 0x801DDBC0)
extern "C" void fn_801DDBC0(void);

// TODO (not drafted yet, 0x58 bytes at 0x801DDDB0)
extern "C" void fn_801DDDB0(void);

// TODO (not drafted yet, 0xE8 bytes at 0x801DDE08)
extern "C" void fn_801DDE08(void);

// TODO (not drafted yet, 0x3C4 bytes at 0x801DDEF0)
extern "C" int fn_801DDEF0(Creature* c);

// TODO (not drafted yet, 0x64 bytes at 0x801DE2B4)
int CreatureSeekCallback(Creature* c);

// TODO (not drafted yet, 0xCC bytes at 0x801DE318)
int CreatureChargeCallback(Creature* c);

// TODO (not drafted yet, 0xCC bytes at 0x801DE3E4)
int CreatureShootCallback(Creature* c);

// TODO (not drafted yet, 0xC bytes at 0x801DE4B0)
extern "C" void fn_801DE4B0(void);

// TODO (not drafted yet, 0x2E4 bytes at 0x801DE4BC)
extern "C" void fn_801DE4BC(void);

// TODO (not drafted yet, 0x3C bytes at 0x801DE7A0)
extern "C" void fn_801DE7A0(void);

// TODO (not drafted yet, 0x128 bytes at 0x801DE7DC)
extern "C" void fn_801DE7DC(void);

// TODO (not drafted yet, 0x5C bytes at 0x801DE904)
void TrailDestroy(int id);

// TODO (not drafted yet, 0x30 bytes at 0x801DE960)
int TrailCreate(TrailDesc* d);

// TODO (not drafted yet, 0x1DC bytes at 0x801DE990)
extern "C" void fn_801DE990(void);

// TODO (not drafted yet, 0xC4 bytes at 0x801DEB6C)
extern "C" void fn_801DEB6C(void);

// TODO (not drafted yet, 0x78 bytes at 0x801DEC30)
extern "C" void fn_801DEC30(void);

// TODO (not drafted yet, 0xEC bytes at 0x801DECA8)
extern "C" void fn_801DECA8(void);

// TODO (not drafted yet, 0x178 bytes at 0x801DED94)
extern "C" void fn_801DED94(void);

// TODO (not drafted yet, 0x28C bytes at 0x801DEF0C)
extern "C" void fn_801DEF0C(void);

// TODO (not drafted yet, 0x6A8 bytes at 0x801DF198)
extern "C" void fn_801DF198(void);

// TODO (not drafted yet, 0x108 bytes at 0x801DF840)
extern "C" void fn_801DF840(void);

// TODO (not drafted yet, 0x778 bytes at 0x801DF948)
extern "C" void fn_801DF948(void);

// TODO (not drafted yet, 0x3BC bytes at 0x801E00C0)
extern "C" void fn_801E00C0(void);

// TODO (not drafted yet, 0x27C bytes at 0x801E047C)
extern "C" void fn_801E047C(void);

// TODO (not drafted yet, 0x68 bytes at 0x801E06F8)
extern "C" void fn_801E06F8(void);

// TODO (not drafted yet, 0x48 bytes at 0x801E0760)
extern "C" void fn_801E0760(void);

// TODO (not drafted yet, 0x60 bytes at 0x801E07A8)
extern "C" void fn_801E07A8(void);

// TODO (not drafted yet, 0x8 bytes at 0x801E0808)
extern "C" void fn_801E0808(void);

// TODO (not drafted yet, 0x20 bytes at 0x801E0810)
extern "C" void fn_801E0810(void);

// TODO (not drafted yet, 0x20 bytes at 0x801E0830)
extern "C" void fn_801E0830(void);

// TODO (not drafted yet, 0x30 bytes at 0x801E0850)
extern "C" void fn_801E0850(void);

// TODO (not drafted yet, 0x4C bytes at 0x801E0880)
extern "C" void fn_801E0880(void);

// TODO (not drafted yet, 0x30 bytes at 0x801E08CC)
extern "C" void fn_801E08CC(void);

// TODO (not drafted yet, 0x30 bytes at 0x801E08FC)
extern "C" void fn_801E08FC(void);

// TODO (not drafted yet, 0x9C bytes at 0x801E092C)
extern "C" void fn_801E092C(void);

// TODO (not drafted yet, 0x2B0 bytes at 0x801E09C8)
extern "C" void fn_801E09C8(void);

// TODO (not drafted yet, 0xF8 bytes at 0x801E0C78)
extern "C" void fn_801E0C78(void);

// TODO (not drafted yet, 0x58 bytes at 0x801E0D70)
extern "C" void fn_801E0D70(void);

// TODO (not drafted yet, 0x114 bytes at 0x801E0DC8)
extern "C" void fn_801E0DC8(void);

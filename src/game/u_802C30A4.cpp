// u_802C30A4.cpp: the final boss actor (0x802C30A4-0x802C8900).
// Message handler, damage handling ("finalboss_hearthit" / "parthit" / "stunhit"), attack states and
// part animation of the final boss.  Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;

#define NULL 0

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

struct Boss;

class BossBuf : public Buffer {
public:
    BossBuf() {}
    virtual ~BossBuf() {}
    virtual void Release(char* p);
};

inline void* operator new(unsigned long, void* p) { return p; }

// Per-boss behaviour state (reached through Boss::state).
struct BossState {
    int type;               // 0x00
    int next;               // 0x04
    int anim;               // 0x08
    float partHp[4];        // 0x0C: arm / leg part health
    float heartHp;          // 0x1C
    float partFlash[4];     // 0x20
    float heartFlash;       // 0x30
    int hitPart;            // 0x34
    float hitTimer;         // 0x38
    float fade;             // 0x3C
    float fade2;            // 0x40

    int mode;               // 0x44
    int slots[4];           // 0x48
    int phaseAngle;         // 0x58
    BossBuf list;           // 0x5C
    float maxFloat70;       // 0x70
    float shotDelay;        // 0x74
    int shotState;          // 0x78
    float stunHp;           // 0x7C
    int attackA;            // 0x80
    int attackB;            // 0x84
    float attackC;          // 0x88
    int hitIds[4];          // 0x8C
    float hitTimers[4];     // 0x9C
};

// One row of the boss state table (24 bytes).
struct BossStateOps {
    void (*enter)(Boss* b);                 // 0x00
    int (*canLeave)(Boss* b, int next);     // 0x04
    int (*update)(Boss* b);                 // 0x08
    void* unk0C;                            // 0x0C
    const char* name;                       // 0x10
    void* unk14;                            // 0x14
};

struct BossCfg {
    u8 pad00[0x30];
    int maxHealth;          // 0x30
};

// The boss actor.
struct Boss {
    u8 pad00[0xC4];
    BossCfg* cfg;           // 0x0C4
    u8 pad0C8[4];
    int model;              // 0x0CC
    int skin;               // 0x0D0
    u8 pad0D4[0x1C];
    int bone;               // 0x0F0
    u8 pad0F4[0x40];
    BossState* state;       // 0x134
    u8 pad138[4];
    float lastDamage;       // 0x13C
    u8 pad140[0x130];
    u8 flags270[0x688];     // 0x270: condition bits
    int phase;              // 0x8F8
    float phaseTimer;       // 0x8FC
    u8 pad900[4];
    float health;           // 0x904
};

struct BossMsg {
    Boss* boss;             // 0x00
    int* arg1;              // 0x04
    int flags;              // 0x08
    int dmg;                // 0x0C
    int hitIndex;           // 0x10
    int p14;                // 0x14
    int p18;                // 0x18
    int model1C;            // 0x1C
    int other20;            // 0x20
    u8 pad24[0x1C];
    int hitNode;            // 0x40
};

extern int lbl_803C4DF0[28];   // node / bone ids: 0x803C4DF0 .. 0x803C4E5C
extern int lbl_804D3AB4;
extern int lbl_804D3AC0;
extern int lbl_804D3ACC;
extern int lbl_804D57B4;
extern int lbl_804D3A10;
extern float lbl_804D3A14;
extern float lbl_804D3A08;
extern float lbl_804D39F4;
extern float lbl_804D39F8;
extern float lbl_804D39FC;

extern "C" {
int fn_800A7AC4(void* set, int i);
int fn_800A7B78(void* set, int i);
void fn_800A7A54(void* set, int i, int v);
int fn_8003AA9C(int model, int a, int b);
int fn_8003A1F0(int a, int b, int c);
int fn_800183A8(float* a, float* b, int model, int c, int* d, int* e, float* f, float* g, void* h, int* i);
int fn_800AFD20(Boss* b);
void* fn_8002431C(double a, void* b, int c, int d, int e);
int fn_800BA84C(int i);
void fn_8003F15C(double a, int b, int c, int d, void* e, int f);
int fn_8007EB34(u32 a, u32 b);
void fn_8000F550(void* ids, int n);
void fn_8000F738(void* ids, int n);
void fn_8003D84C(void* ids, int n);
void fn_800225C8(void* out, String* a, String* b);
void fn_8028C188(void* out, int a, int b);
int fn_8007EF48(int a, int b, const char* c, int d, int e, int f, int g, int h, int i, int j);
int fn_80018BBC(int model, int node);
int fn_80170834(const char* name);
int FxStart(int a, int b, int c);
int fn_80049948(void);
int SendMsg(int to, int msg, void* data);
void fn_80038AE8(int model, int id, int a, int b);
void* memset(void* p, int c, u32 n);
}
extern const char* lbl_803C4E74[];
extern "C" double __fabs(double);
extern "C" float sqrtf(float);
extern char lbl_8034BEE0[];
int BossSetupHit(BossMsg* m);
void BossPartInit(Boss* b);
void BossHitFeedback(BossMsg* m);


extern "C" {
void fn_8009F7A8(Mtx34* out, Vec3* dir);
void MtxGetEuler(Vec3* out, Mtx34* m);
void fn_800E1098(double scale, Vec3* in, Vec3* out);
int fn_800C7E80(Vec3* pos);
}

void VFree(void* p);
extern "C" {
void fn_8016EEF4(int fx);
void fn_800A7B08(void* set, int i, int v);
void fn_8006DB74(double v);
}
extern BossStateOps lbl_803C4E64[];
extern float lbl_804D3A48;
extern int lbl_804D3A54;
extern int lbl_804D3A58;
extern int lbl_804D3A5C;
extern int lbl_804D3A60;

extern int lbl_804D3A9C, lbl_804D3AA0, lbl_804D3AA4, lbl_804D3AA8, lbl_804D3AAC, lbl_804D3AB0;
extern int lbl_804D3AB8, lbl_804D3ABC, lbl_804D3AC4, lbl_804D3AC8;
extern int lbl_804D57B0, lbl_804D57B8, lbl_804D57BC, lbl_804D57C0;
extern float lbl_804D3A64, lbl_804D3A68;
extern "C" {
void* VAlloc(int size);
void fn_80090634(int anim, int a, int b, int c);
int rand(void);
int fn_8008E5CC(int a, int b);
void fn_80037EC0(int model, int node, int a, int r, int g, int b, int al);
void fn_800381F8(int model, int a, int r, int g, int b, int al);
void fn_800CDD10(void);
}
extern int lbl_804D39F0;
void BossRefreshStates(Boss* b);
void BossUpdateParts(Boss* b, BossState* s);
void BossPartInit(Boss* b);

extern "C" {
double fn_800A127C(double a);
double fn_800A1418(double a);
int fn_80018AD4(int model, int node);
}
extern float lbl_804D3A04;
extern float lbl_804D3A18;

static inline int ClampByte(int v) {
    if (v < 0)
        return 0;
    if (v < 0x100)
        return v;
    return 0xFF;
}

// Flash fraction for a hit-flash timer (0 = no flash, 1 = full).
static inline float FlashFrac(float t) {
    float f;
    if (lbl_804D3A18 <= t) {
        f = 1.0f;
        if (lbl_804D3A14 - lbl_804D3A18 < t)
            f = (lbl_804D3A14 - t) / lbl_804D3A18;
    } else {
        f = t / lbl_804D3A18;
    }
    return f;
}

extern "C" {
void fn_80090974(double a, int anim, int b, int c, int d, int e, int f);
int fn_8016325C(void* p);
int fn_8008DE4C(int anim, int a, int b, int c);
void fn_800A7068(void* p);
int fn_800C5654(int to, u32 msg, int from);
void fn_8008F230(double a, int anim, int b, int c);
int fn_800A1BC8(int a, int b, int c, int d, int e);
void fn_800A7028(void* set);
void fn_800913EC(double a, int anim, void* b);
void fn_80211A84(void);
}
extern float lbl_804DAEA4;
extern float lbl_804DAEA8;
extern int lbl_803C4E60[];

extern "C" {
void fn_8008F4E8(int anim, int a);
void fn_8008F4EC(int anim, int a);
void fn_8008F378(double a, int anim);
int fn_800A1AC4(int a, int b, int c);
int Rand(void);
int fn_8008E7D0(int anim);
}
extern float lbl_804DAE6C;
extern float lbl_804D3A00;

// Starts the current state's animation, picking variant `pick` (or a random one if out of range).
static inline void BossPlayStateAnim(Boss* b, int pick, bool randomize) {
    if (b != NULL) {
        BossState* s = b->state;
        int anim = *(int*)((u8*)b + 0xD4);
        int count;
        if (fn_800A7AC4(&b->flags270, 0xC) != 0) {
            fn_800A7A54(&b->flags270, 0xC, 0);
            fn_800A7A54(&b->flags270, 0xE, 1);
            fn_8008F4E8(anim, 0);
            fn_8008F378(0.0f, anim);
            fn_8008F4EC(anim, 0);
        }
        count = fn_800A1AC4(*(int*)((u8*)b->cfg + 0x14), *(int*)((u8*)b->cfg + 0x1C), lbl_803C4E60[s->type * 6]);
        if (count != 0) {
            int idx;
            if (randomize || pick < 0 || count <= pick) {
                int r = Rand();
                pick = r - (r / count) * count;
            }
            idx = fn_800A1BC8(*(int*)((u8*)b->cfg + 0x14), *(int*)((u8*)b->cfg + 0x1C), lbl_803C4E60[s->type * 6], pick, 0);
            if (fn_8008E7D0(anim) != 0) {
                fn_800A7A54(&b->flags270, 0x1C, 0);
                if (fn_800A7AC4(&b->flags270, 0xE) == 0) {
                    fn_8008F230(lbl_804DAE6C, anim, 0, 1);
                } else {
                    fn_800A7A54(&b->flags270, 0xE, 0);
                    fn_8008F230(0.0f, anim, 0, 0);
                }
                fn_80090974(1.0f, anim, 0, idx, 9, (int)lbl_803C4E64[s->type].unk0C, (int)b);
                s->anim = idx;
            }
        }
    }
}


extern "C" void fn_8028B35C(BossBuf* list, void* where, int n);
extern float lbl_804D3A0C;
extern float lbl_804D3A44;
extern float lbl_804DAEAC;
extern float lbl_804DAEB0;
extern float lbl_804DAEB4;

extern "C" {
int fn_802D1640(double a, double b, double c, double d, double e, double f, double g, double h, Boss* owner, unsigned* mask,
                u32 hash, unsigned flag, int one);
void* fn_8028B3BC(BossBuf* list, void* where, int n);
}
extern int lbl_804D3A3C;
extern int lbl_804D3A40;
extern float lbl_804D3A1C, lbl_804D3A20, lbl_804D3A24, lbl_804D3A28, lbl_804D3A2C, lbl_804D3A30, lbl_804D3A34;
extern float lbl_804D3A4C, lbl_804D3A50;

struct PlaceF {
    float v[12];
};

extern "C" {
void fn_801BE4B8(Boss* b);
int fn_8008E964(int anim);
int fn_8008EA9C(int anim, int a);
double fn_800CC2D0(Boss* b, int a, int c, int d);
void fn_800A0168(void* a, void* b);
void fn_800A0030(void* out, void* a, void* b);
void ActorGetMatrix(Mtx34* m, Boss* a);
int AnimApply(int anim, char* bone, int a, int b);
void EulerNormalize(Vec3* a, Vec3* b);
void Vec3Set(Vec3* v, float x, float y, float z);
void MtxGetEuler(Vec3* out, Mtx34* m);
void MtxSetEuler(Mtx34* m, Vec3* rot);
void PlacementToMatrix(Mtx34* m, PlaceF* p);
void ModelSetMatrix(int model, Mtx34* m, char* bone);
void BoneGetMatrix(Mtx34* m, void* b);
}
extern float lbl_804DAEC0;

// Message handler of the final boss actor.
int BossMsgHandler(int msg, BossMsg* m) {
    int r;
    if (msg == 0x407)
        return BossSetupHit(m);
    if (msg < 0x407) {
        if (msg == 4)
            return 0xFFF0BDC1;
        if (msg < 4) {
            if (msg == 1) {
                Boss* b = m->boss;
                if (b->state->type == 5) {
                    if ((m->flags & 0x8000000) == 0) {
                        if (fn_800A7AC4(&b->flags270, 2) == 0) {
                            if (fn_800A7AC4(&b->flags270, 3) == 0 || m->hitNode == 0 || (*(u32*)(m->hitNode + 0x38) >> 24) != 0x13) {
                                m->model1C = b->model;
                                if ((m->flags & 0x20000000) == 0) {
                                    int a = fn_8003AA9C(b->model, 0, 0);
                                    int c = fn_8003A1F0(b->model, b->bone, a);
                                    int* box = m->arg1;
                                    float lo[3];
                                    float hi[3];
                                    float out[6];
                                    char tmp[4];
                                    lo[0] = (float)box[0];
                                    lo[1] = (float)box[1];
                                    lo[2] = (float)box[2];
                                    hi[0] = (float)box[3];
                                    hi[1] = (float)box[4];
                                    hi[2] = (float)box[5];
                                    if (fn_800183A8(lo, hi, b->model, c, &m->p14, &m->p18, out, out + 3, tmp, &m->hitIndex) == 0)
                                        return 0xFFF0BDC1;
                                    ((float*)m)[12] = out[0];
                                    ((float*)m)[13] = out[1];
                                    ((float*)m)[14] = out[2];
                                    ((float*)m)[15] = 4096.0f * out[3];
                                    ((float*)m)[16] = 4096.0f * out[4];
                                    ((float*)m)[17] = 4096.0f * out[5];
                                    return 1;
                                }
                                return 1;
                            }
                            return 0xFFF0BDC1;
                        }
                        return 0xFFF0BDC1;
                    }
                    return 0xFFF0BDC1;
                }
                return 0;
            }
            if (msg == 0) {
                Boss* b = m->boss;
                BossState* s = b->state;
                int model;
                int a;
                int* p;
                b->lastDamage = 0.0f;
                model = fn_800AFD20(b);
                a = fn_8003AA9C(b->model, 0, 0);
                a = fn_8003A1F0(model, b->bone, a);
                p = (int*)((u8*)m + 0x10);
                if ((s->mode & 1) != 0)
                    p = (int*)fn_8002431C(1.0, p, 0, 0, 0);
                if (b->phase == 6) {
                    float total = (float)fn_800BA84C(9);
                    float t = (total - b->phaseTimer) / total;
                    float v = 0.0f;
                    if (t >= 0.0f) {
                        v = t;
                        if (v > 1.0f)
                            v = 1.0f;
                    }
                    if (v == 0.0f)
                        return 1;
                    fn_8003F15C(v, model, a, b->skin, p, 0);
                    return 1;
                }
                fn_8003F15C(1.0f, model, a, b->skin, p, 0);
                return 1;
            }
        } else if (msg == 6) {
            s16 ids[2];
            s16 one[1];
            ids[0] = fn_8007EB34(0xF2E2BEB8, 0x415E24EA);
            ids[1] = fn_8007EB34(0xF2E2BEB8, 0x929869B8);
            one[0] = (s16)lbl_804D3AC0;
            if (m->arg1 == 0) {
                fn_8000F550(ids, 2);
                fn_8003D84C(one, 1);
            } else {
                fn_8000F738(ids, 2);
                fn_8003D84C(one, 1);
            }
            return 0;
        }
    } else {
        if (msg == 0xC00) {
            if (m->arg1 != 0)
                BossPartInit(m->boss);
            return 0;
        }
        if (msg < 0xC00) {
            if (msg == 0x41E) {
                if ((int)m->arg1 == -0x527A8906) {
                    String a(lbl_803C4E74[m->boss->state->type * 6]);
                    String bs(lbl_8034BEE0);
                    String c;
                    fn_800225C8(&c, &a, &bs);
                    fn_8028C188((int*)m + 3, 0, 0);
                    return 0;
                }
                return 0;
            }
            if (msg < 0x41E && msg > 0x41C) {
                BossState* s = m->boss->state;
                if ((int)m->arg1 == 0x42E653B) {
                    if (m->dmg == -1) {
                        memset(s->slots, 0xFF, 0x10);
                    } else {
                        int i;
                        for (i = 0; i < 4; i++) {
                            if (s->slots[i] == -1) {
                                s->slots[i] = m->dmg;
                                s->phaseAngle = 0;
                                break;
                            }
                        }
                    }
                    return 1;
                }
                return 0;
            }
        } else if (msg == 0xC14) {
            if (m->dmg == 0x7BDA2B92) {
                fn_8007EF48(-1, 0, lbl_8034BEE0 + 2, 0, 0, 0, 0, 0, 0, 0);
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

// Applies a hit message to the boss: heart, parts and stun damage; plays hit sounds.
int BossSetupHit(BossMsg* m) {
    Boss* b = m->boss;
    BossState* s = b->state;
    int node = fn_80018BBC(b->model, m->hitIndex);
    int r;
    if (m->p14 == 0x403548D0) {
        if (fn_800A7B78(&b->flags270, 10) == 0) {
            if (node == lbl_804D3AB4) {
                if (s->partHp[0] <= 0.0f && s->partHp[1] <= 0.0f && s->partHp[2] <= 0.0f && s->partHp[3] <= 0.0f &&
                    0.0f < s->heartHp) {
                    s->heartHp -= 1.0f;
                    m->dmg = (int)((0.34f * (float)b->cfg->maxHealth) / (float)lbl_804D3A10);
                    m->dmg = (int)((float)m->dmg * 0.01f * (float)fn_800BA84C(6));
                    b->health -= (float)m->dmg;
                    s->heartFlash = lbl_804D3A14;
                    FxStart(fn_80170834("finalboss_hearthit"), 0, 0);
                    if (s->heartHp <= 0.0f || b->health <= 0.0f) {
                        fn_800A7A54(&b->flags270, 4, 1);
                        if (b->health <= 0.0f) {
                            if (b->phase != 4 && b->phase != 6 && fn_800A7AC4(&b->flags270, 9) == 0 &&
                                fn_800A7AC4(&b->flags270, 10) == 0)
                                fn_800A7A54(&b->flags270, 9, 1);
                            {
                                int player = fn_80049948();
                                struct {
                                    int a;
                                    int hash;
                                    int c;
                                    String name;
                                } q;
                                q.a = 0;
                                q.hash = 0xAA24D309;
                                SendMsg(*(int*)(player + 0x38), 0x41D, &q);
                            }
                        }
                    }
                    BossHitFeedback(m);
                }
            } else {
                int i;
                for (i = 0; i < 4; i++) {
                    if (node == lbl_803C4DF0[8 + i]) {
                        if (s->partHp[i] <= 0.0f)
                            return 1;
                        FxStart(fn_80170834("finalboss_parthit"), 0, 0);
                        m->dmg = (int)((float)m->dmg * 0.01f * (float)fn_800BA84C(6));
                        s->partHp[i] -= (float)m->dmg;
                        s->partFlash[i] = lbl_804D3A14;
                        s->hitPart = i;
                        if (0.4f * (float)b->cfg->maxHealth <= b->health) {
                            if (0.7f * (float)b->cfg->maxHealth <= b->health)
                                s->hitTimer = lbl_804D39F4;
                            else
                                s->hitTimer = lbl_804D39F8;
                        } else {
                            s->hitTimer = lbl_804D39FC;
                        }
                        if (s->partHp[i] <= 0.0f) {
                            bool any = false;
                            fn_800A7A54(&b->flags270, 4, 1);
                            if (s->partHp[0] > 0.0f || s->partHp[1] > 0.0f || s->partHp[2] > 0.0f || s->partHp[3] > 0.0f)
                                any = true;
                            if (!any) {
                                fn_80038AE8(b->model, fn_8007EB34(0xD9044CBB, 0xFE64CE60), 0, 1);
                                fn_80038AE8(b->model, fn_8007EB34(0xD9044CBB, 0xFE64CF1A), 0, 1);
                                fn_80038AE8(b->model, fn_8007EB34(0xD9044CBB, 0x20FA84FF), 0, 1);
                                fn_80038AE8(b->model, fn_8007EB34(0xD9044CBB, 0x20FA85B9), 0, 1);
                                if (lbl_804D3ACC == -1)
                                    lbl_804D3ACC = FxStart(lbl_804D57B4, 0, 0);
                            }
                        }
                        BossHitFeedback(m);
                        return 1;
                    }
                }
            }
        } else {
            s->stunHp -= (float)m->dmg;
            s->mode = 6;
            FxStart(fn_80170834("finalboss_stunhit"), 0, 0);
            BossHitFeedback(m);
            if (s->stunHp <= 0.0f) {
                s->stunHp = lbl_804D3A08;
                fn_800A7A54(&b->flags270, 5, 1);
                {
                    int player = fn_80049948();
                    struct {
                        int a;
                        int hash;
                        int c;
                        String name;
                    } q;
                    q.a = 0;
                    q.hash = 0xAA24D309;
                    SendMsg(*(int*)(player + 0x38), 0x41D, &q);
                }
                s->attackA = 0;
            }
        }
    }
    return 1;
}

// Spawns the hit spark for the hit in `m`, and records it in the boss's hit-effect slots.
void BossHitFeedback(BossMsg* m) {
    int* other = (int*)m->arg1;
    BossState* s = m->boss->state;
    int slot = -1;
    Vec3 dir;
    Vec3 pos;
    Vec3 euler;
    Mtx34 look;
    int spawned;
    float best;

    dir.z = (float)(other[0x2C / 4] - ((int*)m)[8]);
    dir.x = (float)(other[0x24 / 4] - ((int*)m)[6]);
    dir.y = (float)(other[0x28 / 4] - ((int*)m)[7]);
    best = 999989.0f;
    if (((1.1920929e-07f < __fabs(dir.x)) ? 1 : 0) + ((1.1920929e-07f < __fabs(dir.y)) ? 1 : 0) + ((1.1920929e-07f < __fabs(dir.z)) ? 1 : 0) == 0) {
        dir.x = 0.0f;
        dir.y = 0.0f;
        dir.z = 0.0f;
    } else {
        float inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        dir.x *= inv;
        dir.y *= inv;
        dir.z *= inv;
    }
    fn_8009F7A8(&look, &dir);
    MtxGetEuler(&euler, &look);
    dir.x = -dir.x;
    dir.y = -dir.y;
    dir.z = -dir.z;
    pos.x = (float)((int*)m)[6];
    pos.y = (float)((int*)m)[7];
    pos.z = (float)((int*)m)[8];
    fn_800E1098(2000.0f, &dir, &dir);
    pos.x += dir.x;
    pos.y += dir.y;
    pos.z += dir.z;
    spawned = fn_800C7E80(&pos);
    if (spawned != 0) {
        String s0;
        struct {
            int a;
            int hash;
            int c;
        } q;
        q.hash = 0x6220F224;
        q.c = 0;
        SendMsg(*(int*)(spawned + 0x38), 0x41D, &q);
        {
            float* timers = s->hitTimers;
            if (timers[0] < best) {
                slot = 0;
                best = timers[0];
            }
            if (timers[1] < best) {
                slot = 1;
                best = timers[1];
            }
            if (timers[2] < best) {
                slot = 2;
                best = timers[2];
            }
            if (timers[3] < best)
                slot = 3;
        }
        if (slot != -1) {
            s->hitIds[slot] = *(int*)(spawned + 0x38);
            s->hitTimers[slot] = 800.0f;
        }
    }
}

// STUB (not drafted yet, 0x10BC bytes at 0x802C4078): per-frame boss update; runs the state table callbacks,
// the hit flash timers and the periodic attack scheduling.
int BossUpdate(Boss* b);

// Frees the boss's behaviour state.
void BossFreeState(Boss* b) {
    *(int*)((u8*)b + 0x110) = 0;
    VFree(b->state);
    b->state = NULL;
}

// Installs the boss behaviour on actor `b` and resolves all the model node / effect ids it uses.
void BossInit(int unused, Boss* b) {
    BossState* s;
    if (b == NULL)
        return;
    *(void**)((u8*)b + 0x110) = (void*)BossMsgHandler;
    *(void**)((u8*)b + 0x118) = (void*)fn_800CDD10;
    s = (BossState*)VAlloc(sizeof(BossState));
    if (s == NULL) {
        b->state = NULL;
    } else {
        new (&s->list) BossBuf;
        s->type = 0;
        s->next = 0;
        s->shotDelay = (lbl_804D3A68 - lbl_804D3A64) * ((float)rand() / 32767.0f) + lbl_804D3A64;
        s->stunHp = lbl_804D3A08;
        s->attackA = 0;
        s->attackC = 0.0f;
        s->attackB = 0;
        s->heartFlash = 0.0f;
        s->hitIds[0] = -1;
        s->hitTimers[0] = -1.0f;
        s->hitIds[1] = -1;
        s->hitTimers[1] = -1.0f;
        s->hitIds[2] = -1;
        s->hitTimers[2] = -1.0f;
        s->hitIds[3] = -1;
        s->hitTimers[3] = -1.0f;
        memset(s->slots, 0xFF, 0x10);
        lbl_804D3A9C = fn_8007EB34(0xF1754BF2, 0x450B2BB);
        lbl_804D3AA0 = fn_8007EB34(0xF1754BF2, 0x4A53FC1);
        lbl_804D3AA4 = fn_8007EB34(0xF1754BF2, 0x930D5E58);
        lbl_804D3AA8 = fn_8007EB34(0xF1754BF2, 0x8C73DFA9);
        lbl_804D3AAC = fn_8007EB34(0xD9044CBB, 0xBF47ABB4);
        lbl_803C4DF0[0] = fn_8007EB34(0xD9044CBB, 0x25357BEE);
        lbl_803C4DF0[1] = fn_8007EB34(0xD9044CBB, 0x7E9E82);
        lbl_803C4DF0[2] = fn_8007EB34(0xD9044CBB, 0xB49CC4C1);
        lbl_803C4DF0[3] = fn_8007EB34(0xD9044CBB, 0x824BB112);
        lbl_803C4DF0[4] = fn_8007EB34(0xF1754BF2, 0x25357BEE);
        lbl_803C4DF0[5] = fn_8007EB34(0xF1754BF2, 0x7E9E82);
        lbl_803C4DF0[6] = fn_8007EB34(0xF1754BF2, 0xB49CC4C1);
        lbl_803C4DF0[7] = fn_8007EB34(0xF1754BF2, 0x824BB112);
        lbl_803C4DF0[8] = fn_8007EB34(0xD9044CBB, 0x25357BEE);
        lbl_803C4DF0[9] = fn_8007EB34(0xD9044CBB, 0x7E9E82);
        lbl_803C4DF0[10] = fn_8007EB34(0xD9044CBB, 0xB49CC4C1);
        lbl_803C4DF0[11] = fn_8007EB34(0xD9044CBB, 0xB6135425);
        lbl_804D3AB0 = fn_8007EB34(0xD9044CBB, 0x8FAF0157);
        lbl_804D3AB4 = fn_8007EB34(0xD9044CBB, 0x3D36D22A);
        {
            int i;
            for (i = 0; i < 9; i++)
                lbl_803C4DF0[12 + i] = fn_8007EB34(0xD9044CBB, 0x81F567 + i);
            for (i = 0; i < 7; i++)
                lbl_803C4DF0[21 + i] = fn_8007EB34(0xD9044CBB, 0x81F585 + i);
        }
        lbl_804D3AC4 = fn_8007EB34(0xD9044CBB, 0x2DF40732);
        lbl_804D3AC0 = fn_8007EB34(0x87DE5E6A, 0x1159B855);
        lbl_804D3AB8 = fn_80170834("demon_fire");
        lbl_804D3ABC = fn_80170834("demon_death");
        lbl_804D57B0 = fn_80170834("finalboss_wind");
        lbl_804D57B4 = fn_80170834("finalboss_heart");
        lbl_804D57B8 = fn_80170834("finalboss_phaseout");
        lbl_804D57BC = fn_80170834("finalboss_phasein");
        lbl_804D57C0 = fn_80170834("finalboss_pain");
        lbl_804D3AC8 = -1;
        lbl_804D3ACC = -1;
        b->state = s;
        BossPartInit(b);
        fn_800A7B08(&b->flags270, 10, 1);
        fn_80090634(*(int*)((u8*)b + 0xD4), 0, 0, 0);
        lbl_803C4E64[s->type].enter(b);
    }
}

// Hurt-type messages: picks the attack set from the remaining health.
int BossOnHurt(int msg, Boss* b) {
    BossState* s;
    if (b == NULL)
        return 0;
    s = b->state;
    if (msg == 0x1B83833F) {
        lbl_803C4E64[s->type].canLeave(b, s->next);
        lbl_803C4E64[s->next].enter(b);
    } else {
        if (msg < 0x1B83833F) {
            if (msg != 0x20EE36) {
                if (msg < 0x20EE36) {
                    if (msg != -0xD44A5E9)
                        return 1;
                } else if (msg != 0x26E045) {
                    return 1;
                }
            }
        } else if (msg != 0x3961ACD5) {
            if (0x3961ACD4 < msg)
                return 1;
            if (msg != 0x395E188C)
                return 1;
        }
        if (s->attackA == -1) {
            float max = (float)b->cfg->maxHealth;
            if (b->health <= 0.8f * max) {
                if (b->health <= 0.5f * max) {
                    s->attackA = lbl_804D3A5C;
                    s->attackB = lbl_804D3A60;
                    s->attackC = 0.0f;
                } else {
                    s->attackA = lbl_804D3A58;
                    s->attackB = 0;
                    s->attackC = 0.0f;
                }
            } else {
                s->attackA = lbl_804D3A54;
                s->attackB = 0;
                s->attackC = 0.0f;
            }
        }
    }
    return 1;
}

int BossEnterState(Boss* b, int next) {
    BossState* s = b->state;
    s->next = next;
    if (s->type == 6) {
        fn_800A7B08(&b->flags270, 10, 1);
        fn_80049948();
        fn_8006DB74(lbl_804D3A48);
        s->stunHp = lbl_804D3A08;
    }
    return 0;
}

int BossSetNext(Boss* b, int next) {
    b->state->next = next;
    fn_800A7A54(&b->flags270, 0, 0);
    return 1;
}

int BossReturnZero(void) {
    return 0;
}

// Per-frame update for the "dying" state: fades the body parts out.
int BossFadeTick(Boss* b) {
    BossState* s = b->state;
    BossRefreshStates(b);
    if (s->type == 10) {
        double t = fn_8008E5CC(*(int*)((u8*)b + 0xD4), 0);
        if (0.1f <= t) {
            if (t <= 0.8f) {
                s->fade = 1.0f;
                fn_80037EC0(b->model, lbl_804D3AAC, 1, 0xFF, 0xFF, 0xFF, 0xFF);
            } else {
                float v = 5.0f * (float)(t - 0.8f);
                float c = 0.0f;
                int a;
                int i;
                if (0.0f <= v) {
                    c = v;
                    if (1.0f < v)
                        c = 1.0f;
                }
                s->fade = 1.0f - c;
                a = (int)(255.0f * s->fade);
                if (a < 0)
                    a = 0;
                else if (a >= 0x100)
                    a = 0xFF;
                for (i = 0; i < 4; i++)
                    fn_80037EC0(b->model, lbl_803C4DF0[i], 0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_80037EC0(b->model, lbl_804D3AB0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_80037EC0(b->model, lbl_804D3AAC, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_800381F8(b->model, 1, 0xFF, 0xFF, 0xFF, a & 0xFF);
            }
        } else {
            float v = (float)(10.0 * t);
            int a;
            float c = 0.0f;
            if (0.0f <= v) {
                c = v;
                if (1.0f < v)
                    c = 1.0f;
            }
            s->fade = c;
            a = (int)(s->fade * (float)(0xFF - lbl_804D39F0));
            if (a < 0)
                a = 0;
            else if (a >= 0x100)
                a = 0xFF;
            fn_80037EC0(b->model, lbl_804D3AAC, 1, 0xFF, 0xFF, 0xFF, a & 0xFF);
            BossPartInit(b);
        }
    }
    b->phaseTimer = 0.0f;
    return s->type;
}

int BossUpdateAndReturn(Boss* b) {
    BossState* s = b->state;
    BossRefreshStates(b);
    BossUpdateParts(b, s);
    return s->type;
}

// Fade-out tick (state 6): hides the body as the animation advances.
int BossStunTick(Boss* b) {
    BossState* s = b->state;
    BossRefreshStates(b);
    if (s->type == 6) {
        double t = fn_8008E5CC(*(int*)((u8*)b + 0xD4), 0);
        int a;
        s->fade = (float)(1.0 - t);
        a = (int)-(s->fade * (float)(0xFF - lbl_804D39F0) - 255.0f);
        if (a < 0)
            a = 0;
        else if (a >= 0x100)
            a = 0xFF;
        fn_80037EC0(b->model, lbl_804D3AAC, 1, 0xFF, 0xFF, 0xFF, a & 0xFF);
    }
    BossUpdateParts(b, s);
    return s->type;
}

// Chooses the next state while the boss is idle between attacks.
int BossIdleNext(Boss* b) {
    BossState* s = b->state;
    BossRefreshStates(b);
    if (fn_800A7AC4(&b->flags270, 4) == 0) {
        if (0.0f < s->heartHp) {
            if (fn_800A7B78(&b->flags270, 10) == 0)
                s->hitTimer -= *(float*)((u8*)b + 0x138);
            if (0.0f < s->hitTimer) {
                BossUpdateParts(b, s);
                return s->type;
            }
            return 6;
        }
        return 6;
    }
    fn_800A7A54(&b->flags270, 4, 0);
    if (0.0f < b->health) {
        if (0.0f < s->heartHp) {
            bool any = false;
            if (s->partHp[0] <= 0.0f) {
                if (s->partHp[1] <= 0.0f) {
                    if (s->partHp[2] <= 0.0f) {
                        if (0.0f < s->partHp[3])
                            any = true;
                    } else {
                        any = true;
                    }
                } else {
                    any = true;
                }
            } else {
                any = true;
            }
            if (any)
                return 7;
            return 8;
        }
        return 9;
    }
    return 10;
}

int BossHitTick(Boss* b) {
    BossState* s = b->state;
    BossRefreshStates(b);
    if (s->type == 4) {
        double t = fn_8008E5CC(*(int*)((u8*)b + 0xD4), 0);
        int a;
        s->fade = (float)t;
        a = (int)-(s->fade * (float)(0xFF - lbl_804D39F0) - 255.0f);
        if (a < 0)
            a = 0;
        else if (a >= 0x100)
            a = 0xFF;
        fn_80037EC0(b->model, lbl_804D3AAC, 1, 0xFF, 0xFF, 0xFF, a & 0xFF);
    }
    BossUpdateParts(b, s);
    if (0.4f * (float)b->cfg->maxHealth <= b->health) {
        if (0.7f * (float)b->cfg->maxHealth <= b->health)
            s->hitTimer = lbl_804D39F4;
        else
            s->hitTimer = lbl_804D39F8;
    } else {
        s->hitTimer = lbl_804D39FC;
    }
    return s->type;
}

// Tints the four parts and the heart: pulsing while armoured, red flash when hit.
void BossUpdateParts(Boss* b, BossState* s) {
    bool anyPart = false;
    float pulseScale = 1.0f;
    int pulse;
    int i;
    if (s->hitTimer < lbl_804D3A04) {
        double w = fn_800A127C(45.0f * ((lbl_804D3A04 - s->hitTimer) / lbl_804D3A04));
        pulseScale = (float)(14.0 * w + 1.0);
    }
    s->fade2 = 360.0f * 0.001f * *(float*)((u8*)b + 0x138) * pulseScale + s->fade2;
    pulse = ClampByte((int)(127.0f * (float)(1.0 + fn_800A1418(s->fade2))));
    s->phaseAngle = (int)(10.0f * 360.0f * 0.001f * *(float*)((u8*)b + 0x138) + (float)s->phaseAngle);
    for (i = 0; i < 4; i++) {
        int node = fn_80018AD4(b->model, lbl_803C4DF0[4 + i]);
        bool active = false;
        int r;
        int g;
        int bl;
        if (s->slots[0] == node)
            active = true;
        else if (s->slots[1] == node)
            active = true;
        else if (s->slots[2] == node)
            active = true;
        else if (s->slots[3] == node)
            active = true;
        if (active) {
            r = ClampByte((int)(127.0f * (float)(1.0 + fn_800A1418((float)s->phaseAngle))));
            g = 0;
            bl = 0;
        } else {
            int base;
            float t;
            if (s->partHp[i] <= 0.0f) {
                base = 0;
            } else {
                anyPart = true;
                base = pulse;
            }
            r = base;
            g = base;
            bl = base;
            t = s->partFlash[i];
            if (0.0f <= t) {
                float f = FlashFrac(t);
                float inv = 1.0f - f;
                r = ClampByte((int)(255.0f * f + (float)base * inv));
                g = ClampByte((int)((float)base * inv));
                bl = ClampByte((int)((float)base * inv));
            }
        }
        fn_80037EC0(b->model, lbl_803C4DF0[i], 1, r & 0xFF, g & 0xFF, bl & 0xFF, 0xFF);
    }
    if (anyPart) {
        fn_80037EC0(b->model, lbl_804D3AB0, 1, 200, 200, 200, 0xFF);
    } else {
        int base = pulse;
        int r;
        int g;
        int bl;
        float t = s->heartFlash;
        if (s->heartHp <= 0.0f)
            base = 0;
        r = base;
        g = base;
        bl = base;
        if (0.0f <= t) {
            float f = FlashFrac(t);
            float inv = 1.0f - f;
            r = ClampByte((int)(255.0f * f + (float)base * inv));
            g = ClampByte((int)((float)base * inv));
            bl = ClampByte((int)((float)base * inv));
        }
        fn_80037EC0(b->model, lbl_804D3AB0, 1, r & 0xFF, g & 0xFF, bl & 0xFF, 0xFF);
    }
}

int BossUpdateIdle(Boss* b) {
    BossState* s = b->state;
    BossRefreshStates(b);
    return s->type;
}

int BossUpdateWalk(Boss* b) {
    BossState* s = b->state;
    BossRefreshStates(b);
    if (__fabs(*(float*)((u8*)b + 0x284)) >= 5.0f)
        return s->type;
    return 0;
}

// Picks the locomotion state from the input flags.
int BossPickMove(Boss* b) {
    BossState* s = b->state;
    BossRefreshStates(b);
    if (fn_800A7AC4(&b->flags270, 0x31) == 0) {
        if (__fabs(*(float*)((u8*)b + 0x284)) <= lbl_804DAEA4) {
            if (fn_800A7AC4(&b->flags270, 0x2E) == 0) {
                if (fn_8016325C((u8*)b + 0x894) == 4)
                    return 2;
                return s->type;
            }
            return 4;
        }
        return 1;
    }
    return 3;
}

int BossEnterDeath(Boss* b) {
    BossState* s = b->state;
    s->type = 11;
    s->next = 11;
    b->phaseTimer = lbl_804DAEA8;
    fn_8008DE4C(*(int*)((u8*)b + 0xD4), 0, s->anim, 0x18);
    *(void**)((u8*)b + 0x10C) = (void*)fn_800A7068;
    b->phase = 6;
    if (fn_800A7AC4(&b->flags270, 10) == 0) {
        fn_800A7A54(&b->flags270, 10, 1);
        fn_800C5654(*(int*)((u8*)b + 0x38), 0xDC04E3D5, *(int*)((u8*)b + 0x38));
    }
    return 1;
}

int BossEnterDie(Boss* b) {
    BossState* s = b->state;
    int anim;
    s->type = 10;
    s->next = 11;
    fn_800A7A54(&b->flags270, 0xE, 1);
    fn_8008F230(0.0f, *(int*)((u8*)b + 0xD4), 0, 1);
    *(int*)((u8*)b + 0x220) = 4;
    fn_800A7A54(&b->flags270, 0xD, 1);
    anim = fn_800A1BC8(*(int*)((u8*)b->cfg + 0x14), *(int*)((u8*)b->cfg + 0x1C), lbl_803C4E60[s->type * 6], 0, 0);
    fn_80090974(1.0f, *(int*)((u8*)b + 0xD4), 0, anim, 0x58, (int)lbl_803C4E64[s->type].unk0C, (int)b);
    s->anim = anim;
    fn_800A7028(&b->flags270);
    fn_800A7A54(&b->flags270, 0xC, 1);
    fn_800A7A54(&b->flags270, 0xD, 1);
    fn_800913EC(0.0f, *(int*)((u8*)b + 0xD4), 0);
    if (lbl_804D3AC8 != -1) {
        fn_8016EEF4(lbl_804D3AC8);
        lbl_804D3AC8 = -1;
    }
    if (lbl_804D3ACC != -1) {
        fn_8016EEF4(lbl_804D3ACC);
        lbl_804D3ACC = -1;
    }
    FxStart(lbl_804D3ABC, 0, 0);
    return 1;
}

int BossEnterHurt(Boss* b) {
    BossState* s = b->state;
    s->type = 9;
    if (b->health <= 0.0f)
        s->next = 10;
    else
        s->next = 5;
    BossPlayStateAnim(b, 0, true);
    if (lbl_804D3ACC != -1) {
        fn_8016EEF4(lbl_804D3ACC);
        lbl_804D3ACC = -1;
    }
    FxStart(lbl_804D57C0, 0, 0);
    return 1;
}

int BossEnterAttackB(Boss* b) {
    BossState* s = b->state;
    int pick;
    s->type = 8;
    s->next = 5;
    pick = s->hitPart;
    BossPlayStateAnim(b, pick, false);
    s->hitTimer = lbl_804D3A00;
    return 1;
}

int BossEnterAttackA(Boss* b) {
    BossState* s = b->state;
    int pick;
    s->type = 7;
    s->next = 5;
    pick = s->hitPart;
    BossPlayStateAnim(b, pick, false);
    if (0.4f * (float)b->cfg->maxHealth <= b->health) {
        if (0.7f * (float)b->cfg->maxHealth <= b->health)
            s->hitTimer = lbl_804D39F4;
        else
            s->hitTimer = lbl_804D39F8;
    } else {
        s->hitTimer = lbl_804D39FC;
    }
    return 1;
}

int BossEnterStun(Boss* b) {
    BossState* s = b->state;
    s->type = 6;
    s->next = 0;
    BossPlayStateAnim(b, 0, true);
    fn_800A7A54(&b->flags270, 0x2F, 0);
    BossPartInit(b);
    FxStart(lbl_804D57BC, 0, 0);
    if (lbl_804D3ACC != -1) {
        fn_8016EEF4(lbl_804D3ACC);
        lbl_804D3ACC = -1;
    }
    return 1;
}

int BossEnterIdle(Boss* b) {
    BossState* s = b->state;
    if (s->type != 5)
        fn_800C5654(*(int*)((u8*)b + 0x38), 0xBAC4B348, *(int*)((u8*)b + 0x38));
    s->type = 5;
    s->next = 5;
    BossPlayStateAnim(b, 0, true);
    s->fade = 1.0f;
    fn_80037EC0(b->model, lbl_804D3AAC, 1, 0xFF, 0xFF, 0xFF, lbl_804D39F0 & 0xFF);
    return 1;
}

int BossEnterPhaseOut(Boss* b) {
    BossState* s = b->state;
    int i;
    s->type = 4;
    s->next = 5;
    BossPlayStateAnim(b, 0, true);
    fn_800A7A54(&b->flags270, 0x2F, 1);
    fn_800A7A54(&b->flags270, 0x2E, 0);
    fn_800A7A54(&b->flags270, 4, 0);
    fn_800A7B08(&b->flags270, 10, 0);
    s->partHp[0] = lbl_804D3A0C;
    s->partFlash[0] = -1.0f;
    s->partHp[1] = lbl_804D3A0C;
    s->partFlash[1] = -1.0f;
    s->partHp[2] = lbl_804D3A0C;
    s->partFlash[2] = -1.0f;
    s->partHp[3] = lbl_804D3A0C;
    s->partFlash[3] = -1.0f;
    s->heartHp = (float)lbl_804D3A10;
    s->heartFlash = -1.0f;
    s->fade2 = 0.0f;
    while ((u32)s->list.mLength >> 2 != 0) {
        int target = *(int*)s->list.mUnk0C;
        struct {
            int a;
            int hash;
            int c;
        } q;
        String name;
        q.hash = 0x1FF464;
        q.c = 0;
        SendMsg(target, 0x41D, &q);
        fn_8028B35C(&s->list, (void*)s->list.mUnk0C, 4);
    }
    fn_80049948();
    fn_8006DB74(lbl_804D3A44);
    s->maxFloat70 = lbl_804DAEAC;
    fn_80038AE8(b->model, fn_8007EB34(0xD9044CBB, 0xFE64CE60), 1, 1);
    fn_80038AE8(b->model, fn_8007EB34(0xD9044CBB, 0xFE64CF1A), 1, 1);
    fn_80038AE8(b->model, fn_8007EB34(0xD9044CBB, 0x20FA84FF), 1, 1);
    fn_80038AE8(b->model, fn_8007EB34(0xD9044CBB, 0x20FA85B9), 1, 1);
    FxStart(lbl_804D57B8, 0, 0);
    return 1;
}

int BossEnterMove(Boss* b) {
    BossState* s = b->state;
    int pick;
    s->type = 3;
    s->next = 0;
    if (lbl_804DAEB0 * (float)b->cfg->maxHealth <= b->health) {
        if (lbl_804DAEB4 * (float)b->cfg->maxHealth <= b->health)
            pick = 0;
        else
            pick = 1;
    } else {
        pick = 2;
    }
    s->attackA = -1;
    BossPlayStateAnim(b, pick, false);
    return 1;
}

int BossEnterWalk(Boss* b) {
    BossState* s = b->state;
    s->type = 2;
    s->next = 0;
    BossPlayStateAnim(b, 0, true);
    return 1;
}

int BossEnterRun(Boss* b) {
    if (b != NULL) {
        BossState* s = b->state;
        s->type = 1;
        s->next = 1;
        BossPlayStateAnim(b, 0, true);
        return 1;
    }
    return 0;
}

int BossEnterIdleBase(Boss* b) {
    if (b != NULL) {
        BossState* s = b->state;
        s->type = 0;
        s->next = 0;
        BossPlayStateAnim(b, 0, true);
        s->fade = 0.0f;
        fn_80037EC0(b->model, lbl_804D3AAC, 0, 0xFF, 0xFF, 0xFF, 0xFF);
        return 1;
    }
    return 0;
}

// Rebuilds the boss's ring of orbiting effects (one set per heart segment).
void BossPartInit(Boss* b) {
    BossState* s = b->state;
    int i;
    while ((u32)s->list.mLength >> 2 != 0) {
        int target = *(int*)s->list.mUnk0C;
        struct {
            int a;
            int hash;
            int c;
        } q;
        String name;
        q.hash = 0x1FF464;
        q.c = 0;
        SendMsg(target, 0x41D, &q);
        fn_8028B35C(&s->list, (void*)s->list.mUnk0C, 4);
    }
    for (i = 0; i < lbl_804D3A40; i++) {
        unsigned mask[3];
        float centerY;
        int r;
        int k;
        double speed;
        int fx;
        mask[0] = 0;
        mask[1] = *(unsigned*)((u8*)b + 0x84);
        mask[2] = *(unsigned*)((u8*)b + 0x88);
        centerY = *(float*)((u8*)b + 0x8C) + lbl_804D3A2C * (float)i + lbl_804D3A1C;
        r = Rand();
        for (k = 0; k < lbl_804D3A3C; k++) {
            int bit = (r % 24 + k) % 24;
            mask[bit >> 5] |= 1 << (bit & 31);
        }
        speed = (lbl_804D3A50 - lbl_804D3A4C) * ((float)rand() / 32767.0f) + lbl_804D3A4C;
        if (Rand() & 1)
            speed = (float)(speed * -1.0);
        fx = fn_802D1640(lbl_804D3A20, lbl_804D3A24, 3400.0, lbl_804D3A34, 0.0, lbl_804D3A28, speed, lbl_804D3A30, b, mask + 1,
                         0xF62EAC61, mask[0], 1);
        *(int*)fn_8028B3BC(&s->list, (void*)(s->list.mUnk0C + s->list.mLength), 4) = fx;
        {
            struct {
                int a;
                int hash;
                int c;
            } q2;
            String name2;
            q2.hash = 0xCD52CF56;
            q2.c = 0;
            SendMsg(((int*)s->list.mUnk0C)[i], 0x41D, &q2);
        }
    }
    s->maxFloat70 = 1000.0f * (lbl_804D3A2C / lbl_804D3A30);
}

// Moves the boss from the root-motion of its current animation and its pad input, and refreshes
// the model matrix.
void BossRefreshStates(Boss* b) {
    int* bone = *(int**)((u8*)b + 0xF0);
    float dt = *(float*)((u8*)b + 0x138);
    int anim = *(int*)((u8*)b + 0xD4);
    int model = b->model;
    float delta[3];
    float* d = delta;
    Mtx34 rot;
    Mtx34 rot2;
    Vec3 e;
    Mtx34 place;
    float* pitch = (float*)((u8*)b + 0x254);
    float* yaw = (float*)((u8*)b + 0x24C);

    if (bone == NULL)
        return;
    if (fn_8008E964(anim) == 0 && fn_800A7AC4(&b->flags270, 0xC) != 0)
        return;
    fn_801BE4B8(b);
    fn_800913EC(dt, anim, delta);
    {
        int pose = fn_8003AA9C(model, (int)bone, 1);
        AnimApply(anim, (char*)bone, pose, b->skin);
    }
    if (fn_800A7AC4(&b->flags270, 0xD) == 0 && fn_800A7AC4(&b->flags270, 0xC) == 0) {
        double ground = fn_800CC2D0(b, 0, 0, 0);
        float in1 = *(float*)((u8*)b + 0x284);
        float in2 = *(float*)((u8*)b + 0x27C);
        float turn = *(float*)((u8*)b->cfg + 0xC4);
        float maxPitch = *(float*)((u8*)b->cfg + 8);
        if (-999999.0 != ground)
            *(float*)((u8*)b + 0x8C) = (float)(ground + ((float*)bone)[0x18 / 4]);
        *(int*)((u8*)b + 0x2C) = (int)*(float*)((u8*)b + 0x8C);
        ((float*)bone)[0x10 / 4] = 0.0f;
        ((float*)bone)[0x14 / 4] = 0.0f;
        ((float*)bone)[0x18 / 4] = 0.0f;
        if (in1 <= 1.1920929e-07f) {
            if (in1 < -1.0f) {
                float lim = -((turn * dt) / 1000.0f);
                float v = in1;
                if (in1 < lim)
                    v = lim;
                *pitch += v;
            }
        } else {
            float lim = (turn * dt) / 1000.0f;
            float v = in1;
            if (lim < in1)
                v = lim;
            *pitch += v;
        }
        if (in2 <= 1.1920929e-07f) {
            if (in2 < -1.0f) {
                float lim = -((0.4f * turn * dt) / 1000.0f);
                float v = in2;
                if (v < lim)
                    v = lim;
                *yaw += v;
            }
        } else {
            float lim = (0.4f * turn * dt) / 1000.0f;
            float v = in2;
            if (lim < in2)
                v = lim;
            *yaw += v;
        }
        {
            float c = *yaw;
            float lo = -maxPitch;
            float r = lo;
            if (lo <= c) {
                r = c;
                if (maxPitch < c)
                    r = maxPitch;
            }
            *yaw = r;
        }
        if (1.1920929e-07f < __fabs(in1))
            *(int*)((u8*)b + 0x258) = *(int*)((u8*)b + 0x2B0);
        if (fn_800A7AC4(&b->flags270, 0) != 0) {
            ActorGetMatrix(&rot, b);
            fn_800A0168(&rot, &rot);
            fn_800A0030(delta, &rot, delta);
            if (dt <= 0.0f) {
                ((float*)b)[0x240 / 4] = 0.0f;
                ((float*)b)[0x244 / 4] = 0.0f;
                ((float*)b)[0x248 / 4] = 0.0f;
            } else {
                float inv = 1000.0f / dt;
                ((float*)b)[0x240 / 4] = d[0] * inv;
                ((float*)b)[0x244 / 4] = d[1] * inv;
                ((float*)b)[0x248 / 4] = 0.0f;
            }
        }
        EulerNormalize((Vec3*)yaw, (Vec3*)yaw);
        Vec3Set(&e, *pitch, 0.0f, 0.0f);
        EulerNormalize(&e, &e);
        MtxSetEuler(&place, &e);
        place.m[2][1] = (float)*(int*)((u8*)b + 0x24);
        place.m[2][2] = (float)*(int*)((u8*)b + 0x28);
        place.m[2][3] = (float)*(int*)((u8*)b + 0x2C);
        PlacementToMatrix((Mtx34*)b, (PlaceF*)&place);
        ActorGetMatrix(&rot2, b);
        fn_800A0168(&rot2, &rot2);
        d[1] = 0.0f;
        d[2] = 0.0f;
        fn_800A0030(delta, &rot2, delta);
        place.m[2][1] += d[0];
        place.m[2][2] += d[1];
        place.m[2][3] += d[2];
        ModelSetMatrix(model, &place, (char*)bone);
    } else if (fn_800A7AC4(&b->flags270, 0xC) == 0) {
        *yaw = 0.0f;
        {
            double ground = fn_800CC2D0(b, 0, 0, 0);
            if (-999999.0 != ground) {
                *(float*)((u8*)b + 0x8C) = (float)(ground + ((float*)bone)[0x18 / 4]);
                ((float*)bone)[0x18 / 4] = 0.0f;
            }
        }
        *(int*)((u8*)b + 0x2C) = (int)*(float*)((u8*)b + 0x8C);
        ActorGetMatrix(&rot, b);
        fn_800A0168(&rot, &rot);
        fn_800A0030(delta, &rot, delta);
        if (dt <= 0.0f) {
            ((float*)b)[0x240 / 4] = 0.0f;
            ((float*)b)[0x244 / 4] = 0.0f;
            ((float*)b)[0x248 / 4] = 0.0f;
        } else {
            float inv = 1000.0f / dt;
            ((float*)b)[0x240 / 4] = d[0] * inv;
            ((float*)b)[0x244 / 4] = d[1] * inv;
            ((float*)b)[0x248 / 4] = 0.0f;
        }
        Vec3Set(&e, *pitch, 0.0f, 0.0f);
        EulerNormalize(&e, &e);
        MtxSetEuler(&place, &e);
        place.m[2][1] = (float)*(int*)((u8*)b + 0x24);
        place.m[2][2] = (float)*(int*)((u8*)b + 0x28);
        place.m[2][3] = (float)*(int*)((u8*)b + 0x2C);
        PlacementToMatrix((Mtx34*)b, (PlaceF*)&place);
        ModelSetMatrix(model, &place, (char*)bone);
    } else {
        fn_800CC2D0(b, 0, 0, 0);
        if (fn_8008EA9C(anim, 0) != 0) {
            if (fn_800A7AC4(&b->flags270, 0xC) == 0) {
                Vec3Set(&e, *pitch, 0.0f, 0.0f);
                EulerNormalize(&e, &e);
                MtxSetEuler(&place, &e);
                place.m[2][1] = (float)*(int*)((u8*)b + 0x24);
                place.m[2][2] = (float)*(int*)((u8*)b + 0x28);
                place.m[2][3] = (float)*(int*)((u8*)b + 0x2C);
                PlacementToMatrix((Mtx34*)b, (PlaceF*)&place);
            } else {
                BoneGetMatrix(&place, bone);
                MtxGetEuler((Vec3*)yaw, &place);
                *pitch -= lbl_804DAEC0;
                EulerNormalize((Vec3*)yaw, (Vec3*)yaw);
                *(float*)((u8*)b + 0x258) = 0.0f;
                PlacementToMatrix((Mtx34*)b, (PlaceF*)&place);
                *(int*)((u8*)b + 0x84) = bone[0x10 / 4];
                *(int*)((u8*)b + 0x88) = bone[0x14 / 4];
                *(int*)((u8*)b + 0x8C) = bone[0x18 / 4];
                *(int*)((u8*)b + 0x24) = (int)((float*)bone)[0x10 / 4];
                *(int*)((u8*)b + 0x28) = (int)((float*)bone)[0x14 / 4];
                *(int*)((u8*)b + 0x2C) = (int)((float*)bone)[0x18 / 4];
            }
        }
        ModelSetMatrix(model, NULL, (char*)bone);
    }
}

void BossForward88E0(void) {
    fn_80211A84();
}

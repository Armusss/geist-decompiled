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

// Per-boss behaviour state (reached through Boss::state).
struct BossState {
    int type;               // 0x00
    u8 pad04[8];
    float partHp[4];        // 0x0C: arm / leg part health
    float heartHp;          // 0x1C
    float partFlash[4];     // 0x20
    float heartFlash;       // 0x30
    int hitPart;            // 0x34
    float hitTimer;         // 0x38
    u8 pad3C[8];
    int mode;               // 0x44
    int slots[4];           // 0x48
    int slotCount;          // 0x58
    u8 pad5C[0x20];
    float stunHp;           // 0x7C
    int stunState;          // 0x80
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

extern int lbl_803C4E10[4];
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
extern char lbl_8034BEE0[];
int BossSetupHit(BossMsg* m);
void BossHitFeedback(BossMsg* m);
void BossShutdown(Boss* b);

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
                fn_8003F15C(1.0, model, a, b->skin, p, 0);
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
                BossShutdown(m->boss);
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
                                s->slotCount = 0;
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
                    if (node == lbl_803C4E10[i]) {
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
                s->stunState = 0;
            }
        }
    }
    return 1;
}

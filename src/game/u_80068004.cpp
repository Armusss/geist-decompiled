// u_80068004.cpp: player stamina, push-away and terrain/target helpers (0x80068004-0x8006C604).
// Tuning setters for the player's movement curves, the stamina meters at Player+0x5E8..0x600, the
// actor repel push, the weapon/aim target search and the player pad-driven camera nudges.
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

struct Actor;

// The player actor, as far as this unit looks into it.
struct Player {
    u8 pad000[0x50];
    u32 flags50;            // 0x050
    u8 pad054[0x2A0];
    float negScale;         // 0x2F4
    u8 pad2F8[0x2C0];
    float pushX;            // 0x5B8
    float pushY;            // 0x5BC
    float pushZ;            // 0x5C0
    u8 pad5C4[0xC];
    u32 pushed;             // 0x5D0
    float pushDist;         // 0x5D4
    int fxA;                // 0x5D8
    int fxB;                // 0x5DC
    u8 pad5E0[8];
    float stamina;          // 0x5E8
    float meter;            // 0x5EC
    u32 unk5F0;             // 0x5F0
    float unk5F4;           // 0x5F4
    float unk5F8;           // 0x5F8
    float ratio;            // 0x5FC
    float charge;           // 0x600
};

// Functions called from other units under their placeholder names.
extern "C" void fn_8006811C(Player* p, float dt);
extern "C" void fn_800699A8(Player* p);
extern "C" float fn_800699B8(Player* p);
extern "C" void fn_8006B360(u32* s);

extern float lbl_804D2880;
extern float lbl_804D2884;
extern float lbl_804D2888;
extern float lbl_804D288C;
extern int lbl_804D44F4;
extern float lbl_804D44F8;
extern float lbl_804D44FC;
extern float lbl_804D28A8;

extern "C" {
double fn_800A1418(double x);
int fn_80049840(void);
int fn_80049940(void);
double fn_800E1020(Vec3* v);
void fn_800E1098(double scale, Vec3* in, Vec3* out);
int fn_8001F928(Actor* a, int* box, int mask, void* cb, int arg);
int rand(void);
void WeatherSetB0(int v);
void fn_8016F018(int fx, int level);
void fn_80163918(double a, double b, double c, double d, double e, Player* p, void* at);
}
void ActorGetMatrix(Mtx34* m, Actor* a);
extern "C" void fn_800A0030(void* out, void* m, void* v);
int TargetSlotUsable(int** slot, int* arg);

int RepelPush(float strength, float swirl, float lift, float inner, float outer, Player* self, Vec3* pos, int arg);

// ---- Pad-driven aim / panic input records ----

extern int lbl_804D4500;
extern int lbl_804D4504;
extern "C" int fn_80170834(const char* name);

// Sets the walk-bob amplitude curve.
void PlayerSetBobAmplitude(float x) {
    float v;
    if (x < 0.0f)
        v = 0.866f;
    else
        v = fn_800A1418(x);
    lbl_804D288C = v;
}

void PlayerSetTuneC(float x) {
    if (x < 0.0f)
        x = 0.2f;
    lbl_804D2888 = x;
}

void PlayerSetTuneB(float x) {
    if (x < 0.0f)
        x = 0.25f;
    lbl_804D2884 = x;
}

void PlayerSetTuneA(float x) {
    if (x < 0.0f)
        x = 0.65f;
    lbl_804D2880 = x;
}

void PlayerSetUnk5F8(Player* p, int v) {
    p->unk5F8 = (float)v;
}

void PlayerSetUnk5F4(Player* p, int v) {
    p->unk5F4 = (float)v;
}

void PlayerSetUnk5F0(Player* p, u32 v) {
    p->unk5F0 = v;
}

// 1 when the meter holds at least 180000.
u32 PlayerMeterFull(Player* p) {
    return 180000.0f <= p->meter;
}

// STUB (not drafted yet, 0x1410 bytes at 0x8006811C): per-frame player stamina / sprint update.
void fn_8006811C(Player* p, float dt) {
}

// Per-state look-up of a tuning value kept in the player record.
u32 PlayerStateValue(int unused, int state) {
    int p = fn_80049840();
    if (state == 0xE)
        return *(u32*)(p + 0x53C);
    if (state < 0xE) {
        if (state == 0xC)
            return *(u32*)(p + 0x538);
        if (state > 0xB)
            return *(u32*)(p + 0x540);
    } else if (state < 0x10) {
        return *(u32*)(p + 0x534);
    }
    return 1;
}

// Pushes `self` away from the position of `other`.
int ActorRepel(Actor* self, Actor* other, float strength, float swirl, float lift, float inner, float outer) {
    if (other == NULL)
        return 0;
    {
        Vec3 pos;
        pos.x = (float)((int*)other)[0x24 / 4];
        pos.y = (float)((int*)other)[0x28 / 4];
        pos.z = (float)((int*)other)[0x2C / 4];
        return RepelPush(strength, swirl, lift, inner, outer, (Player*)self, &pos, 0);
    }
}

// Pushes the player away from `pos` within the outer radius; sound and weather react to the push.
int RepelPush(float strength, float swirl, float lift, float inner, float outer, Player* self, Vec3* pos, int arg) {
    Vec3 delta;
    Vec3 pushed;
    int box[6];
    Mtx34 mtx;
    double dist;
    int hit;
    delta.x = (float)((int*)self)[0x24 / 4] - pos->x;
    delta.y = (float)((int*)self)[0x28 / 4] - pos->y;
    delta.z = (float)((int*)self)[0x2C / 4] - pos->z;
    dist = fn_800E1020(&delta);
    if (dist <= outer) {
        box[0] = ((int*)self)[0x24 / 4];
        box[1] = ((int*)self)[0x28 / 4];
        box[2] = ((int*)self)[0x2C / 4];
        box[3] = (int)pos->x;
        box[4] = (int)pos->y;
        box[5] = (int)pos->z;
        hit = fn_8001F928((Actor*)self, box, 0x12000000, (void*)TargetSlotUsable, arg);
        if (strength < 0.0f)
            lbl_804D44F4 |= 4;
        if (hit == 0) {
            float scale;
            float amp;
            float fall;
            self->pushDist = (float)dist;
            self->pushed = 1;
            if (dist < inner) {
                scale = strength;
                if (strength < 0.0f)
                    scale = 0.0f;
            } else {
                scale = (1.0f - (float)(dist - inner) / (outer - inner)) * strength;
            }
            pushed = delta;
            fn_800E1098(scale, &pushed, &pushed);
            ActorGetMatrix(&mtx, (Actor*)self);
            fn_800A0030(&pushed, &mtx, &pushed);
            amp = (float)(2.0f * swirl) * ((float)rand() / 32767.0f) - swirl + 1.0f;
            fall = (float)rand() / 32767.0f;
            self->pushZ = (float)(pushed.y * amp + self->pushZ);
            self->pushX = (float)(pushed.x * amp + self->pushX);
            self->pushY = (float)(scale * ((2.0f * lift) * fall - lift) + self->pushY) * 0.5f;
            {
                int vol = (int)(2048.0f * (1.0f - self->pushDist / outer)) + 0x800;
                if (0.0f != strength) {
                    WeatherSetB0(1);
                    if (self->fxA != -1)
                        fn_8016F018(self->fxA, vol);
                    if (self->fxB != -1)
                        fn_8016F018(self->fxB, vol);
                }
            }
            return 1;
        }
    }
    return 0;
}

void fn_800699A8(Player* p) {
    p->negScale = -lbl_804D44F8;
}

// Stamina as a fraction: negative stamina scales by one constant, positive by another.
float fn_800699B8(Player* p) {
    double r;
    if (0.0f <= p->stamina) {
        if (fn_80049940() == 0) {
            double t = p->stamina / lbl_804D44FC;
            r = 1.0f;
            if (t < r)
                r = t;
        } else {
            r = -0.5f;
        }
    } else {
        r = p->stamina / lbl_804D44F8;
    }
    return r;
}

// Charge-up meter: grows while a hostile possessed target is near, and fires the ready effect.
void PlayerChargeTick(double dt, Player* p) {
    bool near = false;
    Actor* possessed = *(Actor**)((u8*)p + 0x26E4);
    Actor* target = *(Actor**)((u8*)p + 0x2794);
    u32 typePossessed = possessed ? (*(u32*)((u8*)possessed + 0x38) >> 24) : 0;
    u32 typeTarget = target ? (*(u32*)((u8*)target + 0x38) >> 24) : 0;
    if (possessed != NULL && typePossessed == 2) {
        near = true;
    } else if (target != NULL && typeTarget == 2) {
        near = true;
    } else if (target != NULL && typeTarget == 0x13) {
        near = true;
    } else if (possessed != NULL && typePossessed == 0x13 &&
               (*(int*)((u8*)possessed + 0x8F8) == 1 || *(int*)((u8*)possessed + 0x8F8) == 2)) {
        near = true;
    }
    if (near) {
        if (p->charge < lbl_804D28A8) {
            p->charge = (float)(p->charge + dt);
            if (lbl_804D28A8 <= p->charge) {
                p->charge = lbl_804D28A8;
                if ((p->flags50 & 0x100000) == 0)
                    fn_80163918(1.0, 1.0, 1.0, 0.8f, 800.0f, p, (u8*)p + 0x1508);
            }
        }
        p->ratio = p->charge / lbl_804D28A8;
    } else {
        p->charge = 0.0f;
        p->ratio = 0.0f;
    }
}

// STUB (not drafted yet, 0x17E8 bytes at 0x80069B78): player weapon-aim target search and lock-on.
void PlayerAimUpdate(Player* p, float dt) {
}

// Clears one 0x30-word input sample.
void fn_8006B360(u32* s) {
    int i;
    for (i = 0; i < 0x26; i++)
        s[i] = 0;
    for (i = 0; i < 3; i++)
        s[0x23 + i] = 0;
    s[0x26] = 0;
    s[0x27] = 0;
    s[0x28] = 0;
    s[0x29] = 0;
    s[0x2A] = 0;
    s[0x2B] = 0;
}

void AimNoop(void) {
}

// Resets the whole aim input block (0x310 bytes) and caches the "panic" effect ids.
void AimInputInit(float* a, int mode) {
    int i;
    for (i = 0; i < 0x26; i++) {
        a[i] = 0.0f;
        a[0x2C + i] = 99999.0f;
        a[0x52 + i] = 0.0f;
        ((int*)a)[0x78 + i] = 1;
    }
    for (i = 0; i < 6; i++) {
        a[0x26 + i] = 0.0f;
        a[0xB1 + i] = 0.0f;
        ((int*)a)[0x9E + i] = 1;
    }
    ((int*)a)[0xB7] = 0;
    a[0xB8] = 9999999.0f;
    ((int*)a)[0xB9] = -1;
    ((int*)a)[0xBA] = -1;
    a[0xBB] = 0.0f;
    ((int*)a)[0xBC] = 1;
    ((int*)a)[0xA4] = 0;
    ((int*)a)[0xA6] = mode;
    a[0xA7] = 0.0f;
    a[0xA8] = 0.0f;
    a[0xA9] = 0.0f;
    a[0xBE] = 0.0f;
    ((int*)a)[0xBF] = -1;
    a[0xC0] = 700.0f;
    a[0xC1] = 800.0f;
    a[0xC2] = 0.0f;
    a[0xC3] = 0.0f;
    a[0xBD] = -lbl_804D44F8;
    a[0xAD] = 0.0f;
    a[0xAE] = 0.0f;
    a[0xAF] = 0.0f;
    a[0xB0] = 2000.0f;
    lbl_804D4500 = fn_80170834("panic_1");
    lbl_804D4504 = fn_80170834("panic_2");
    lbl_804D44F8 = 250.0f;
    lbl_804D44FC = 800.0f;
    lbl_804D2880 = 0.65f;
    lbl_804D2884 = 0.25f;
    lbl_804D2888 = 0.2f;
    lbl_804D288C = 0.866f;
}

// STUB (not drafted yet, 0x658 bytes at 0x8006B648): fills the aim input block from the pad.
void AimInputFromPad(float* a, Player* p, Actor* target) {
}

// STUB (not drafted yet, 0x964 bytes at 0x8006BCA0): per-frame aim assist update.
void AimAssistUpdate(float* a, Player* p, Actor* target) {
}

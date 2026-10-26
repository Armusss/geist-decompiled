// u_802B3430.cpp: clock actor (possession clock): update, init, messages, hands and tick (0x802B3430-0x802B4600).
// Names are descriptive guesses; the code is matched against the retail DOL.
// Function names are descriptive guesses based on what the code does.

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned int u32;
typedef signed int s32;

extern u8 __upper_map[];

extern void* lbl_804D3900;
extern void* lbl_804D3904;
extern void* lbl_804D3908;
extern float lbl_804DAAF8;
extern float lbl_804DAAFC;
extern float lbl_804DAB00;
extern float lbl_804DAB04;
extern float lbl_804D5780;
extern float lbl_804DAB10;
extern float lbl_804DAB14;
extern float lbl_804DAB18;
extern float lbl_804DAB1C;
extern float lbl_804DAB20;
extern float lbl_804DAB24;
extern float lbl_804DAB28;
extern float lbl_804DAB2C;
extern float lbl_804DAB30;

#include <math.h>

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct Quat {
    float x, y, z, w;
};

// Allocation owned by the clock actor; layout inferred from the code.
struct ClockWork {
    u8 pad_00[0x0C];
    s32 field_0C;
    u8 pad_10[0x20];
    s32 field_30;
    s32 field_34;
    float field_38;
    s32 field_3C;
    s32 field_40;
    s32 field_44;
    s32 field_48;
    s32 field_4C;
};

class Actor {
public:
    u8 pad_00[0x38];
    s32 field_38;  // 0x38
    u8 pad_3C[0x48];
    u8* field_84;  // 0x84: per-actor model/FX table
    u8 pad_88[0x08];
    s32 field_90;  // 0x90
    s32 field_94;  // 0x94
    u8 pad_98[0x04];
    char* field_9C;    // 0x9C
    u8 pad_A0[0x04];
    ClockWork* clock;  // 0xA4
    u8 pad_A8[0x38];
    Mtx34 field_E0;  // 0xE0
    s32 field_110;   // 0x110
};

// The player object the clock hangs from while it is being carried.
class Player {
public:
    u8 pad_00[0x38];
    s32 field_38;  // 0x38
    u8 pad_3C[0x18];
    s32 field_54;  // 0x54
    s32 field_58;  // 0x58
    u8 pad_5C[0x20];
    s32 field_7C;  // 0x7C
    s32 field_80;  // 0x80: model currently shown
    u8 pad_84[0x50];
    s32 field_D4;  // 0xD4
    u8 pad_D8[0x244];
    s32 field_31C;  // 0x31C
    u8 pad_320[0x34];
    s32 field_354;  // 0x354
    s32 field_358;  // 0x358
    s32 field_35C;  // 0x35C
    s32 field_360;  // 0x360
    u8 pad_364[0x28];
    float field_38C;  // 0x38C
    float field_390;  // 0x390
    u8 pad_394[0x1F4];
    s32 field_588;  // 0x588
    u8 pad_58C[0xE74];
    char* field_1400;  // 0x1400
    u8 pad_1404[0x08];
    char* field_140C;  // 0x140C
    u8 pad_1410[0x40];
    Mtx34 field_1450;  // 0x1450
    u8 pad_1480[0x12C0];
    s32 field_2740;  // 0x2740
};

void PossessableUpdate(Actor* self, float dt);
int PossessableInit(Actor* self);
int PossessableMsg(Actor* self, int msg, void* data, int arg);
void* FxFind(const char* name);
void* VAlloc(int size);
s32 AssetFind(u32 a, u32 b);

void* GetPlayer();
void PlayerApplyHeldSlotState(void* player);

u8* ModelAnimate(int model, char* anim, int mode);
int ModelBoneCount(int model);
void AnimApply(int model, char* anim, int frame, int bones);
void ModelSetMatrix(int model, Mtx34* mtx, char* anim);
void ActorGetMatrix(Mtx34* out, Actor* actor);
void MtxSetEuler(Mtx34* mtx, Vec3* euler);
void BoneSetMatrix(void* out, Mtx34* mtx);
extern "C" void PSQUATMultiply(void* a, void* b, void* out);

// Still placeholder symbols in symbols.txt.
extern "C" void fn_8021284C(int handle);
extern "C" int fn_80212730(int handle);
extern "C" void fn_80212814(int a, int b, int c, int d, int e, int f);

void ClockActorUpdateHands(Actor* self);

int PlayerInHost(Actor* actor);
float fn_8008E5CC(int handle, int flag);
void fn_8008E824(float value, int handle, int flag);
void fn_8008F230(float value, int handle, int a, int b);
void Fn8006CDE8(Player* player, int a, int b);
void Fn8006CE5C(Player* player, int a, int b, int c, int d, int e, int f);
void Fn8006CD84(Player* player, int a, float value);
int ActorPlayAnim(int model, unsigned long anim, int a);
extern "C" void FxStart(void* fx, int a, int b);

void ClockActorTick(Actor* self, float dt);

// Hash a name through the upper-case map with the engine's lazy hash.
static inline u32 ClockNameHash(const char* name) {
    u32 hash = 0;
    while (*name != '\0') {
        s32 c = (s8)*name;
        u32 v;
        if (c == -1) {
            v = 0xFFFFFFFF;
        } else {
            v = __upper_map[(u8)c];
        }
        hash = hash * 31 + (v & 0xFF);
        name++;
    }
    return hash % 0xFFFFFFFF;
}

void ClockActorUpdate(Actor* self, float dt) {
    ClockWork* work = self->clock;
    if (work->field_0C != -1 && work->field_30 == 0) {
        work->field_30 = 1;
    } else if (work->field_0C == -1 && work->field_30 == 1) {
        work->field_30 = 0;
    }
    PossessableUpdate(self, dt);
    ClockActorTick(self, dt);
    ClockActorUpdateHands(self);
}

int ClockActorInit(Actor* self) {
    ClockWork* work;

    lbl_804D3900 = FxFind("Clock_WindByHand");
    lbl_804D3904 = FxFind("GhostlyChime");
    lbl_804D3908 = FxFind("ClockChime");

    work = self->clock;
    if (work == 0) {
        work = (ClockWork*)VAlloc(0x50);
        self->clock = work;
    }

    if (!PossessableInit(self)) {
        return 0;
    }

    work->field_30 = 0;
    work->field_34 = 0;
    work->field_38 = lbl_804DAAF8;
    work->field_3C = 10;
    work->field_40 = 5;
    work->field_4C = 0;
    work->field_44 = AssetFind(0xF1754BF2, ClockNameHash("HR_HAND"));
    work->field_48 = AssetFind(0xF1754BF2, ClockNameHash("MIN_HAND"));
    return 1;
}

// Payload of the clock messages: word 0 is the actor that owns the clock,
// word 1 selects the hand, word 3 carries the hand position.
struct ClockMsgData {
    Actor* target;  // 0x00
    u32 hash;       // 0x04
    u32 pad_08;     // 0x08
    s32 value;      // 0x0C
};

// Message handler of the clock possessable; anything it does not consume
// goes on to the shared possessable handler.
int ClockActorMsg(Actor* self, int msg, void* data, int arg) {
    ClockMsgData* m = (ClockMsgData*)data;
    int handled = 0;

    switch (msg) {
    case 0x41D:
        if (m->hash == 0x4B3F7540) {
            m->target->clock->field_3C = m->value;
            handled = 1;
        } else if (m->hash == 0x8168BA70) {
            m->target->clock->field_40 = m->value;
            handled = 1;
        } else {
            handled = 0;
        }
        break;
    case 0x41E:
        if (m->hash == 0x4B3F7540) {
            m->value = m->target->clock->field_34;
            handled = 1;
        } else if (m->hash == 0x8168BA70) {
            m->value = (int)m->target->clock->field_38;
            handled = 1;
        } else {
            handled = 0;
        }
        break;
    case 0xC14: {
        void* player = GetPlayer();
        if (m->value == 0x39CB0CB0) {
            fn_8021284C(*(int*)((u8*)m->target + 0x38));
            PlayerApplyHeldSlotState(player);
        }
        handled = 0;
        break;
    }
    }
    if (handled == 0) {
        handled = PossessableMsg(self, msg, data, arg);
    }
    return handled;
}

// Drive the hour and minute hands of the clock: build their rotation from
// the stored hour/minute and attach them to the holder's models.
void ClockActorUpdateHands(Actor* self) {
    ClockWork* work = self->clock;
    Player* player = (Player*)GetPlayer();
    Mtx34 mtx;
    Mtx34 actorMtx;
    Quat hourBone;
    Quat minBone;
    Vec3 hourEuler;
    Vec3 minEuler;

    hourEuler.x = lbl_804DAAF8;
    hourEuler.y = -(lbl_804DAAFC * (work->field_38 / lbl_804DAB00)) -
                  lbl_804DAAFC * (float)work->field_34;
    hourEuler.z = lbl_804DAAF8;
    MtxSetEuler(&mtx, &hourEuler);
    BoneSetMatrix(&hourBone, &mtx);

    minEuler.x = lbl_804DAAF8;
    minEuler.y = -(lbl_804DAB04 * work->field_38 / lbl_804DAB00);
    minEuler.z = lbl_804DAAF8;
    MtxSetEuler(&mtx, &minEuler);
    BoneSetMatrix(&minBone, &mtx);

    if (work->field_0C == -1) {
        u8* anim = ModelAnimate(self->field_90, self->field_9C, 1);
        AnimApply(self->field_110, self->field_9C, (int)anim, self->field_94);
        char* base = self->field_9C;
        u8 idx = anim[work->field_44];
        PSQUATMultiply(base + idx * 0x1C, &hourBone, base + idx * 0x1C);
        idx = anim[work->field_48];
        PSQUATMultiply(base + idx * 0x1C, &minBone, base + idx * 0x1C);
        ModelSetMatrix(self->field_90, &self->field_E0, self->field_9C);
    } else {
        u8* hourAnim = ModelAnimate(player->field_58, 0, 0);
        u8* minAnim = ModelAnimate(player->field_54, 0, 0);
        if (hourAnim[work->field_44] != 0xFF &&
            minAnim[work->field_44] != 0xFF &&
            hourAnim[work->field_48] != 0xFF &&
            minAnim[work->field_48] != 0xFF) {
            if (fn_80212730(self->field_38) != player->field_38) {
                fn_80212814(self->field_38, player->field_38, -1, 1, 0, 0);
            }
            u8* anim = ModelAnimate(player->field_58, player->field_140C, 1);
            AnimApply(player->field_D4, player->field_140C, (int)anim,
                      ModelBoneCount(player->field_58));
            char* base = player->field_140C;
            u8 idx = anim[work->field_44];
            PSQUATMultiply(base + idx * 0x1C, &hourBone, base + idx * 0x1C);
            idx = anim[work->field_48];
            PSQUATMultiply(base + idx * 0x1C, &minBone, base + idx * 0x1C);

            anim = ModelAnimate(player->field_54, player->field_1400, 1);
            AnimApply(player->field_7C, player->field_1400, (int)anim,
                      ModelBoneCount(player->field_54));
            base = player->field_1400;
            idx = anim[work->field_44];
            PSQUATMultiply(base + idx * 0x1C, &hourBone, base + idx * 0x1C);
            idx = anim[work->field_48];
            PSQUATMultiply(base + idx * 0x1C, &minBone, base + idx * 0x1C);

            *(float*)(player->field_140C + 0x10) = lbl_804DAAF8;
            *(float*)(player->field_140C + 0x14) = lbl_804DAAF8;
            *(float*)(player->field_140C + 0x18) = lbl_804DAAF8;
            ActorGetMatrix(&actorMtx, (Actor*)player);
            ModelSetMatrix(player->field_58, &actorMtx, player->field_140C);

            *(float*)(player->field_1400 + 0x10) = lbl_804DAAF8;
            *(float*)(player->field_1400 + 0x14) = lbl_804DAAF8;
            *(float*)(player->field_1400 + 0x18) = lbl_804DAAF8;
            ModelSetMatrix(player->field_54, &player->field_1450,
                           player->field_1400);
        }
    }
}

// Spin the two hand models the clock drives on the player: the model named by
// hand (0x94/0x98 of the actor's model table) is rotated by amount unless the
// player is already showing that model, in which case the blend is faded out.
static inline void ClockSpinHands(Actor* self, Player* player, float dt, int hand,
                                  float amount, float base, int fadeWhenEqual) {
    int target = *(s32*)(self->field_84 + hand);

    if (player->field_80 == target) {
        float speed = amount / ((lbl_804DAB18 * dt) / lbl_804DAB1C);
        fn_8008E824(speed, player->field_7C, 0);
        fn_8008E824(speed, player->field_D4, 0);
        Fn8006CDE8(player, 0, 1);
        if (fadeWhenEqual) {
            fn_8008F230(lbl_804DAAF8, player->field_7C, 0, 0);
            fn_8008F230(lbl_804DAAF8, player->field_D4, 0, 0);
        }
    } else {
        float limit;

        Fn8006CE5C(player, target, 0, 0,
                   (int)(lbl_804DAB28 *
                         (amount / ((lbl_804DAB18 * dt) / lbl_804DAB1C))), 0, 0);
        limit = lbl_804DAB20 - base;
        if (limit < lbl_804DAAF8) {
            limit = lbl_804DAAF8;
        } else if (limit > lbl_804DAB2C) {
            limit = lbl_804DAB2C;
        }
        Fn8006CD84(player, 0, limit);
        Fn8006CDE8(player, 0, 1);
        if (!fadeWhenEqual) {
            fn_8008F230(lbl_804DAAF8, player->field_7C, 0, 0);
            fn_8008F230(lbl_804DAAF8, player->field_D4, 0, 0);
        }
    }
}

// Return the clock to its idle state once a hand has completed a full turn.
static inline void ClockIdleState(Actor* self, ClockWork* work) {
    if (work->field_4C != 0) {
        ActorPlayAnim(self->field_38, 0xA4DBBCA0, self->field_38);
        FxStart(*(void**)(self->field_84 + 0xD8), 0, 0);
        work->field_30 = 3;
    } else if (work->field_0C != -1) {
        ActorPlayAnim(self->field_38, 0xA738DF81, self->field_38);
        FxStart(*(void**)(self->field_84 + 0xDC), 0, 0);
        work->field_30 = 1;
    } else {
        ActorPlayAnim(self->field_38, 0xA738DF81, self->field_38);
        FxStart(*(void**)(self->field_84 + 0xDC), 0, 0);
        work->field_30 = 0;
    }
}

// Wind the clock by hand while the holder turns, and advance its hands.
void ClockActorTick(Actor* self, float dt) {
    Player* player = (Player*)GetPlayer();
    ClockWork* work;
    float amount;
    float base;

    if (!PlayerInHost((Actor*)player)) {
        return;
    }

    work = self->clock;
    lbl_804D5780 = lbl_804D5780 + dt;
    base = fn_8008E5CC(player->field_7C, 0);
    amount = lbl_804DAAF8;

    if (work->field_30 == 1) {
        if (player->field_588 == 2) {
            if (lbl_804DAB10 < fabs(player->field_38C)) {
                if (lbl_804DAB14 < lbl_804D5780) {
                    lbl_804D5780 = lbl_804DAAF8;
                    FxStart(lbl_804D3900, 0, 0);
                }
                amount = -((lbl_804DAB18 * player->field_38C) * dt / lbl_804DAB1C);
            }
        } else {
            if (lbl_804DAB10 < fabs(player->field_390)) {
                if (lbl_804DAB14 < lbl_804D5780) {
                    lbl_804D5780 = lbl_804DAAF8;
                    FxStart(lbl_804D3900, 0, 0);
                }
                amount = -((lbl_804DAB18 * player->field_390) * dt / lbl_804DAB1C);
            }
        }

        if (player->field_31C == 1 && player->field_2740 == 0 &&
            player->field_354 == 0 && player->field_358 == 0 &&
            player->field_35C == 0 && player->field_360 == 0 &&
            ActorPlayAnim(self->field_38, 0xEBE74F6A, self->field_38) == 0) {
            if (work->field_34 != work->field_3C ||
                lbl_804DAB20 <= fabs(work->field_38 - (float)work->field_40)) {
                ActorPlayAnim(self->field_38, 0xA738DF81, self->field_38);
                FxStart(lbl_804D3908, 0, 0);
                work->field_4C = 0;
                work->field_30 = 2;
            } else {
                work->field_4C = 1;
                ActorPlayAnim(self->field_38, 0xA4DBBCA0, self->field_38);
                FxStart(lbl_804D3904, 0, 0);
                work->field_30 = 3;
            }
        } else if (amount == lbl_804DAAF8) {
            Fn8006CDE8(player, 0, 0);
        } else if ((amount < lbl_804DAAF8 ? lbl_804DAB24 : lbl_804DAB20) <
                   lbl_804DAAF8) {
            ClockSpinHands(self, player, dt, 0x94, -amount, base, 0);

            {
                float v = work->field_38 + amount;
                if (v < lbl_804DAAF8) {
                    work->field_38 = lbl_804DAB00 + v;
                    work->field_34 = (work->field_34 + 11) % 12;
                } else {
                    work->field_38 = v;
                }
            }
        } else {
            ClockSpinHands(self, player, dt, 0x98, amount, base, 0);

            {
                float v = work->field_38 + amount;
                if (v > lbl_804DAB00) {
                    work->field_38 = v - lbl_804DAB00;
                    work->field_34 = (work->field_34 + 1) % 12;
                } else {
                    work->field_38 = v;
                }
            }
        }
    } else if (work->field_30 == 2) {
        float step = (lbl_804DAB30 * dt) / lbl_804DAB1C;

        if (work->field_34 < 6 ||
            (work->field_34 == 6 && work->field_38 <= lbl_804DAAFC)) {
            step = step * lbl_804DAB24;
        }
        if (step == lbl_804DAAF8) {
            Fn8006CDE8(player, 0, 0);
        } else if ((step < lbl_804DAAF8 ? lbl_804DAB24 : lbl_804DAB20) >
                   lbl_804DAAF8) {
            float v = work->field_38 + step;

            if (v > lbl_804DAB00) {
                work->field_38 = v - lbl_804DAB00;
                work->field_34 = (work->field_34 + 1) % 12;
                ClockSpinHands(self, player, dt, 0x94, step, base, 0);
                if (work->field_34 == 0) {
                    work->field_38 = lbl_804DAAF8;
                    ClockIdleState(self, work);
                    Fn8006CDE8(player, 0, 0);
                }
            } else {
                work->field_38 = v;
            }
        } else {
            float v = work->field_38 + step;

            if (v < lbl_804DAAF8) {
                work->field_38 = lbl_804DAB00 + v;
                work->field_34 = (work->field_34 + 11) % 12;
                ClockSpinHands(self, player, dt, 0x98, -step, base, 1);
                if (work->field_34 == 11) {
                    work->field_34 = 0;
                    work->field_38 = lbl_804DAAF8;
                    ClockIdleState(self, work);
                    Fn8006CDE8(player, 0, 0);
                }
            } else {
                work->field_38 = v;
            }
        }
    }
}

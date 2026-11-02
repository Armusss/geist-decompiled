// u_802B3430.cpp: clock actor (possession clock): update, init, messages, hands and tick (0x802B3430-0x802B4600).
// Names are descriptive guesses; the code is matched against the retail DOL.
// Function names are descriptive guesses based on what the code does.

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned long u32;
typedef signed int s32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// Display name of the actor (its descriptor lives in retail data, unused here).
static const char* sClockName = "Possessable Clock";

extern u8 __upper_map[];

extern void* lbl_804D3900;
extern void* lbl_804D3904;
extern void* lbl_804D3908;
extern float lbl_804D5780;

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

inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    int i;
    u32 h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    for (i = 0; s[i] != 0; i++) {
        h = h * 31 + (u8)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
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

    if (self->clock == 0) {
        work = (ClockWork*)VAlloc(0x50);
        self->clock = work;
    } else {
        work = self->clock;
    }

    if (!PossessableInit(self)) {
        return 0;
    }

    work->field_30 = 0;
    work->field_34 = 0;
    work->field_38 = 0.0f;
    work->field_3C = 10;
    work->field_40 = 5;
    work->field_4C = 0;
    work->field_44 = AssetFind(0xF1754BF2, StrHash("HR_HAND"));
    work->field_48 = AssetFind(0xF1754BF2, StrHash("MIN_HAND"));
    return 1;
}

// Payload of the clock messages: word 0 is the actor that owns the clock,
// word 1 selects the hand, word 3 carries the hand position.
struct ClockMsgData {
    Actor* target;  // 0x00
    s32 hash;       // 0x04
    u32 pad_08;     // 0x08
    s32 value;      // 0x0C
};

static inline int ClockSetParam(ClockMsgData* m) {
    ClockWork* w = m->target->clock;
    switch (m->hash) {
    case 0x4B3F7540:
        w->field_3C = m->value;
        return 1;
    case (s32)0x8167BA70:
        w->field_40 = m->value;
        return 1;
    }
    return 0;
}

static inline int ClockGetParam(ClockMsgData* m) {
    ClockWork* w = m->target->clock;
    switch (m->hash) {
    case 0x4B3F7540:
        m->value = w->field_34;
        return 1;
    case (s32)0x8167BA70:
        m->value = (int)w->field_38;
        return 1;
    }
    return 0;
}

// Message handler of the clock possessable; anything it does not consume
// goes on to the shared possessable handler.
int ClockActorMsg(Actor* self, int msg, void* data, int arg) {
    ClockMsgData* m = (ClockMsgData*)data;
    int handled = 0;

    switch (msg) {
    case 0x41D:
        handled = ClockSetParam(m);
        break;
    case 0x41E:
        handled = ClockGetParam(m);
        break;
    case 0xC14: {
        void* player = GetPlayer();
        Actor* t = m->target;
        switch (m->value) {
        case 0x39CB0CB0:
            fn_8021284C(*(int*)((u8*)t + 0x38));
            PlayerApplyHeldSlotState(player);
            break;
        }
        handled = 0;
        break;
    }
    }
    if (handled != 0) {
        return handled;
    }
    return PossessableMsg(self, msg, data, arg);
}

// Drive the hour and minute hands of the clock: build their rotation from
// the stored hour/minute and attach them to the holder's models.
void ClockActorUpdateHands(Actor* self) {
    Player* player;
    ClockWork* work = self->clock;
    u8* hourAnim;
    u8* minAnim;
    Mtx34 mtx;
    Mtx34 actorMtx;
    Vec3 hourEuler;
    Vec3 minEuler;
    Quat hourBone;
    Quat minBone;

    player = (Player*)GetPlayer();
    hourEuler.z = 0.0f;
    hourEuler.x = 0.0f;
    hourEuler.y = -(30.0f * (float)work->field_34) -
                  30.0f * (work->field_38 / 60.0f);
    MtxSetEuler(&mtx, &hourEuler);
    BoneSetMatrix(&hourBone, &mtx);

    minEuler.z = 0.0f;
    minEuler.x = 0.0f;
    minEuler.y = -(360.0f * work->field_38 / 60.0f);
    MtxSetEuler(&mtx, &minEuler);
    BoneSetMatrix(&minBone, &mtx);

    if (work->field_0C != -1) {
        hourAnim = ModelAnimate(player->field_58, 0, 0);
        minAnim = ModelAnimate(player->field_54, 0, 0);
        if (hourAnim[work->field_44] == 0xFF || minAnim[work->field_44] == 0xFF ||
            hourAnim[work->field_48] == 0xFF || minAnim[work->field_48] == 0xFF) {
            return;
        }
        if (fn_80212730(self->field_38) != player->field_38) {
            fn_80212814(self->field_38, player->field_38, -1, 1, 0, 0);
        }
        hourAnim = ModelAnimate(player->field_58, player->field_140C, 1);
        AnimApply(player->field_D4, player->field_140C, (int)hourAnim,
                  ModelBoneCount(player->field_58));
        PSQUATMultiply(player->field_140C + hourAnim[work->field_44] * 0x1C, &hourBone,
                       player->field_140C + hourAnim[work->field_44] * 0x1C);
        PSQUATMultiply(player->field_140C + hourAnim[work->field_48] * 0x1C, &minBone,
                       player->field_140C + hourAnim[work->field_48] * 0x1C);

        minAnim = ModelAnimate(player->field_54, player->field_1400, 1);
        AnimApply(player->field_7C, player->field_1400, (int)minAnim,
                  ModelBoneCount(player->field_54));
        PSQUATMultiply(player->field_1400 + minAnim[work->field_44] * 0x1C, &hourBone,
                       player->field_1400 + minAnim[work->field_44] * 0x1C);
        PSQUATMultiply(player->field_1400 + minAnim[work->field_48] * 0x1C, &minBone,
                       player->field_1400 + minAnim[work->field_48] * 0x1C);

        float* fp = (float*)player->field_140C;
        fp[4] = 0.0f;
        fp[5] = 0.0f;
        fp[6] = 0.0f;
        ActorGetMatrix(&actorMtx, (Actor*)player);
        ModelSetMatrix(player->field_58, &actorMtx, player->field_140C);

        fp = (float*)player->field_1400;
        fp[4] = 0.0f;
        fp[5] = 0.0f;
        fp[6] = 0.0f;
        ModelSetMatrix(player->field_54, &player->field_1450,
                       player->field_1400);
    } else {
        hourAnim = ModelAnimate(self->field_90, self->field_9C, 1);
        AnimApply(self->field_110, self->field_9C, (int)hourAnim, self->field_94);
        PSQUATMultiply(self->field_9C + hourAnim[work->field_44] * 0x1C, &hourBone,
                       self->field_9C + hourAnim[work->field_44] * 0x1C);
        PSQUATMultiply(self->field_9C + hourAnim[work->field_48] * 0x1C, &minBone,
                       self->field_9C + hourAnim[work->field_48] * 0x1C);
        ModelSetMatrix(self->field_90, &self->field_E0, self->field_9C);
    }
}

// Spin the two hand models the clock drives on the player: the model named by
// hand (0x94/0x98 of the actor's model table) is rotated by amount unless the
// player is already showing that model, in which case the blend is faded out.
static inline void ClockSpinHands(Actor* self, Player* player, float dt, int hand,
                                  float amount, float base, int fadeWhenEqual) {
    int target = *(s32*)(self->field_84 + hand);

    if (player->field_80 == target) {
        float speed = amount / ((45.0f * dt) / 1000.0f);
        fn_8008E824(speed, player->field_7C, 0);
        fn_8008E824(speed, player->field_D4, 0);
        Fn8006CDE8(player, 0, 1);
        if (fadeWhenEqual) {
            fn_8008F230(0.0f, player->field_7C, 0, 0);
            fn_8008F230(0.0f, player->field_D4, 0, 0);
        }
    } else {
        float limit;

        Fn8006CE5C(player, target, 0, 0,
                   (int)(4096.0f *
                         (amount / ((45.0f * dt) / 1000.0f))), 0, 0);
        limit = 1.0f - base;
        if (limit < 0.0f) {
            limit = 0.0f;
        } else if (limit > 0.99999988f) {
            limit = 0.99999988f;
        }
        Fn8006CD84(player, 0, limit);
        Fn8006CDE8(player, 0, 1);
        if (!fadeWhenEqual) {
            fn_8008F230(0.0f, player->field_7C, 0, 0);
            fn_8008F230(0.0f, player->field_D4, 0, 0);
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
    amount = 0.0f;

    if (work->field_30 == 1) {
        if (player->field_588 == 2) {
            if (1.1920929e-7f < fabs(player->field_38C)) {
                if (2000.0f < lbl_804D5780) {
                    lbl_804D5780 = 0.0f;
                    FxStart(lbl_804D3900, 0, 0);
                }
                amount = -((45.0f * player->field_38C) * dt / 1000.0f);
            }
        } else {
            if (1.1920929e-7f < fabs(player->field_390)) {
                if (2000.0f < lbl_804D5780) {
                    lbl_804D5780 = 0.0f;
                    FxStart(lbl_804D3900, 0, 0);
                }
                amount = -((45.0f * player->field_390) * dt / 1000.0f);
            }
        }

        if (player->field_31C == 1 && player->field_2740 == 0 &&
            player->field_354 == 0 && player->field_358 == 0 &&
            player->field_35C == 0 && player->field_360 == 0 &&
            ActorPlayAnim(self->field_38, 0xEBE74F6A, self->field_38) == 0) {
            if (work->field_34 != work->field_3C ||
                1.0f <= fabs(work->field_38 - (float)work->field_40)) {
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
        } else if (amount == 0.0f) {
            Fn8006CDE8(player, 0, 0);
        } else if ((amount < 0.0f ? -1.0f : 1.0f) <
                   0.0f) {
            ClockSpinHands(self, player, dt, 0x94, -amount, base, 0);

            {
                float v = work->field_38 + amount;
                if (v < 0.0f) {
                    work->field_38 = 60.0f + v;
                    work->field_34 = (work->field_34 + 11) % 12;
                } else {
                    work->field_38 = v;
                }
            }
        } else {
            ClockSpinHands(self, player, dt, 0x98, amount, base, 0);

            {
                float v = work->field_38 + amount;
                if (v > 60.0f) {
                    work->field_38 = v - 60.0f;
                    work->field_34 = (work->field_34 + 1) % 12;
                } else {
                    work->field_38 = v;
                }
            }
        }
    } else if (work->field_30 == 2) {
        float step = (90.0f * dt) / 1000.0f;

        if (work->field_34 < 6 ||
            (work->field_34 == 6 && work->field_38 <= 30.0f)) {
            step = step * -1.0f;
        }
        if (step == 0.0f) {
            Fn8006CDE8(player, 0, 0);
        } else if ((step < 0.0f ? -1.0f : 1.0f) >
                   0.0f) {
            float v = work->field_38 + step;

            if (v > 60.0f) {
                work->field_38 = v - 60.0f;
                work->field_34 = (work->field_34 + 1) % 12;
                ClockSpinHands(self, player, dt, 0x94, step, base, 0);
                if (work->field_34 == 0) {
                    work->field_38 = 0.0f;
                    ClockIdleState(self, work);
                    Fn8006CDE8(player, 0, 0);
                }
            } else {
                work->field_38 = v;
            }
        } else {
            float v = work->field_38 + step;

            if (v < 0.0f) {
                work->field_38 = 60.0f + v;
                work->field_34 = (work->field_34 + 11) % 12;
                ClockSpinHands(self, player, dt, 0x98, -step, base, 1);
                if (work->field_34 == 11) {
                    work->field_34 = 0;
                    work->field_38 = 0.0f;
                    ClockIdleState(self, work);
                    Fn8006CDE8(player, 0, 0);
                }
            } else {
                work->field_38 = v;
            }
        }
    }
}

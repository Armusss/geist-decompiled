// Clock.cpp: the "Clock" actor type (0x802881E4-0x8028865C).
// A wall clock whose hands follow the console's real-time clock; it also answers queries for the
// current hour, minute and second.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/os.h>

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct Actor;

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

struct ActorType {
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void (*update)(Actor* actor, float dt);
    int (*init)(Actor* actor);
    void* unk14;
    void (*destroy)(Actor* actor);
    void* unk1C[5];
};

struct Actor {
    char pad0[0x90];
    int model;
    int boneCount;
    char pad98[4];
    char* bones;
    char padA0[4];
    float secondAngle;
    char padA8[0x68];
    int anim;
};

int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
int ActorInitBase(Actor* a);                              // 0x800E6444
int AssetFind(u32 bank, u32 name);                        // 0x8007EB34
void ActorGetMatrix(Mtx34* m, Actor* a);                  // 0x8009ED70
int ModelAnimate(int model, char* bones, int a);          // 0x8003AA9C
void AnimApply(int anim, char* bones, int a, int count);  // 0x8008F630
void ModelSetMatrix(int model, Mtx34* m, char* bones);    // 0x8003A66C
int ModelFindBone(int model, int name);                   // 0x80018AD4
void BoneGetMatrix(Mtx34* m, void* bone);                 // 0x800A0EF0
void MtxGetEuler(Vec3* rot, Mtx34* m);                    // 0x8009FB94
void EulerNormalize(Vec3* in, Vec3* out);                 // 0x8016D55C
void MtxSetEuler(Mtx34* m, Vec3* rot);                    // 0x8009FAC0
void BoneSetMatrix(void* bone, Mtx34* m);                 // 0x800A0AC4

int ClockMsg(Actor* a, int msg, void* data, int arg);
void ClockUpdate(Actor* a, float dt);
int ClockInit(Actor* a);

ActorType gClockType = { 0, "Clock", ClockMsg, ClockUpdate, ClockInit };

static int sClockSecondHand;
static int sClockMinuteHand;
static int sClockHourHand;

int ClockInit(Actor* a) {
    sClockSecondHand = AssetFind(0xF1754BF2, 0x91E3615A);
    sClockMinuteHand = AssetFind(0xF1754BF2, 0xF2FF03BA);
    sClockHourHand = AssetFind(0xF1754BF2, 0xE37791EA);
    return ActorInitBase(a);
}

void ClockUpdate(Actor* a, float dt) {
    Vec3 secRot;
    Vec3 minRot;
    Vec3 hourRot;
    Mtx34 m;
    OSCalendarTime t;
    int hour;
    int min;
    int sec;
    int bone;
    int r;

    OSTicksToCalendarTime(OSGetTime(), &t);
    hour = t.hour;
    min = t.min;
    sec = t.sec;
    ActorGetMatrix(&m, a);
    r = ModelAnimate(a->model, a->bones, 1);
    AnimApply(a->anim, a->bones, r, a->boneCount);
    ModelSetMatrix(a->model, &m, a->bones);

    bone = ModelFindBone(a->model, sClockSecondHand);
    if (bone >= 0 && bone < a->boneCount) {
        float base;
        a->secondAngle += 0.006f * dt;
        if (a->secondAngle >= 180.0f) {
            a->secondAngle -= 360.0f;
        }
        base = 6.0f * (int)(a->secondAngle / 6.0f);
        dt = base;
        if (a->secondAngle > 5.0f + base) {
            dt = 6.0f * (a->secondAngle - (int)a->secondAngle) + base;
        }
        bone *= 28;
        BoneGetMatrix(&m, a->bones + bone);
        MtxGetEuler(&secRot, &m);
        secRot.y -= dt;
        EulerNormalize(&secRot, &secRot);
        MtxSetEuler(&m, &secRot);
        BoneSetMatrix(a->bones + bone, &m);
    }

    bone = ModelFindBone(a->model, sClockMinuteHand);
    if (bone >= 0 && bone < a->boneCount) {
        bone *= 28;
        BoneGetMatrix(&m, a->bones + bone);
        MtxGetEuler(&minRot, &m);
        minRot.y -= 6.0f * ((float)min + (float)sec / 60.0f);
        EulerNormalize(&minRot, &minRot);
        MtxSetEuler(&m, &minRot);
        BoneSetMatrix(a->bones + bone, &m);
    }

    bone = ModelFindBone(a->model, sClockHourHand);
    if (bone >= 0 && bone < a->boneCount) {
        bone *= 28;
        BoneGetMatrix(&m, a->bones + bone);
        MtxGetEuler(&hourRot, &m);
        hourRot.y -= 30.0f * ((float)hour + (float)min / 60.0f);
        EulerNormalize(&hourRot, &hourRot);
        MtxSetEuler(&m, &hourRot);
        BoneSetMatrix(a->bones + bone, &m);
    }
}

int ClockMsg(Actor* a, int msg, void* data, int arg) {
    switch (msg) {
    case 0x41E: {
        OSCalendarTime t;
        int hour, min, sec;
        OSTicksToCalendarTime(OSGetTime(), &t);
        hour = t.hour;
        min = t.min;
        sec = t.sec;
        switch (((int*)data)[1]) {
        case 0x041BCEEF:
            ((int*)data)[3] = hour;
            return 1;
        case 0x6A44A8BF:
            ((int*)data)[3] = min;
            return 1;
        case 0xA038D65F:
            ((int*)data)[3] = sec;
            return 1;
        }
    }
    }
    return ActorDefaultMsg(a, msg, data, arg);
}

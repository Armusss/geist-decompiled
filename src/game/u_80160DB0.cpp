// u_80160DB0.cpp: "Health Powerup" effect type (0x80160DB0-0x80160F40): the pickup message id and
// give-pickup handlers and their descriptor gHealthPowerupType. The behaviour state machine that drives
// the pickup is u_80160F40.cpp (its own copies of 1.0f/0.3f show it is a separate file).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;
typedef unsigned short u16;

struct Actor;

struct Owner;
struct Beh;
struct BehData;

struct BehData {
    int state;                                 // 0x00
    int next;                                  // 0x04
    u32 flags;                                 // 0x08
    int handle;                                // 0x0C
    float timer1;                              // 0x10
    float timer2;                              // 0x14
};

struct Beh {
    char pad0[0x10];
    int state;                                 // 0x10
    char pad14[0x14];
    u16 unk28;                                 // 0x28
    u16 unk2A;                                 // 0x2A
    u32 flags;                                 // 0x2C
    char pad30[0x24];
    BehData* data;                             // 0x54
};

struct Owner {
    char pad0[0x2C];
    u32 flags2C;                               // 0x2C
    char pad30[0x8];
    int handle38;                              // 0x38
    char pad3C[0x48];
    float posX;                                // 0x84
    float posY;                                // 0x88
    float posZ;                                // 0x8C
    char pad90[0x3C];
    int model;                                 // 0xCC
    char padD0[0x8];
    int boneId1;                               // 0xD8
    int boneId2;                               // 0xDC
    char padE0[0x4];
    int nodeE4;                                // 0xE4
    char padE8[0x8];
    int nodeF0;                                // 0xF0
    char padF4[0x44];
    float unk138;                              // 0x138
    char pad13C[0x134];
    char cond[0x4];                            // 0x270
    char pad274[0x678];
    int unk8EC;                                // 0x8EC
    int unk8F0;                                // 0x8F0
};

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

// Message structs.
struct PickupValueMsg {
    int unk0;
    float value;
    int result;
    int unkC;
};

struct PickupSpawnMsg {
    int unk0;
    int handle;
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
    Vec3 pos;
};

// Shared engine functions (C linkage ones as plain names).
extern "C" int SendMsg(int to, int msg, void* data);            // 0x800265D4
extern "C" void FxStart(void* fx, int a, int b);                // 0x8016F390
extern "C" int rand(void);                                      // 0x802F2CFC
void* FxFind(const char* name);                                 // 0x80170834

void BehUpdate(Owner* owner, Beh* beh);                         // 0x801632A8
void BehDone(Beh* beh);                                         // 0x80162EBC
void BehReset(Beh* beh);                                        // 0x801633D4
void Fn8016295C(Owner* owner, Beh* beh);                        // 0x8016295C
int OwnerId(Owner* owner);                                      // 0x800A3A40
int ModelFindBone(int model, int bone);                         // 0x80018AD4
int ModelAnimate(int model, char* name, int a);                 // 0x8003AA9C
void ModelNodeSetup(int a, int b, int c, int d);                // 0x80038AE8
void ActorGetMatrix(Mtx34* out, Actor* actor);                  // 0x8009ED70
int HasCondition(void* owner, int id);                          // 0x800A7AC4
void* VAlloc(int size);                                         // 0x8009DA68
void VFree(void* p);                                            // 0x8009DA3C

int Fn800F7DEC(int a, int b, int msg, int who, void* data);     // 0x800F7DEC
void Fn800F7F5C(int a, int b, int msg, int who, void* data, float f); // 0x800F7F5C
int Fn80161FF4(Owner* owner, void* data, float f);              // 0x80161FF4

// Not named yet: small engine helpers.
extern "C" int fn_800F8AF4(int id);                             // 0x800F8AF4
extern "C" int fn_800BA7FC(int id);                             // 0x800BA7FC
extern "C" int fn_8003A3F0(int model, int node, int anim);      // 0x8003A3F0
extern "C" void fn_80027690(int handle);                        // 0x80027690
extern "C" void fn_800277B0(int handle, void* pos, void* col, void* delay); // 0x800277B0
extern "C" int fn_80027A58(int type, void* pos, void* col, int delay, int a); // 0x80027A58

extern BehState gHostBehStates[8];

// 0x80160DB0
u32 PickupBehMsgId(int type, int id) {
    if (id == 0x14967) {
        return 0x4A3E183;
    }
    if (id == 0x2026EF) {
        return 0x4A3E183;
    }
    return 0x3DA260F7;
}

int PickupBehGivePickup(int* owner, int id, int target, float* pos);

// Effect/pickup type descriptor (0x803A7830), registered with the effect-type list at startup
// (0x800EB834). Same layout as the other effect types (Homing, Foam): name, cached hash, then
// the create/destroy/msg/query/update/reset callbacks.
struct PickupType {
    const char* name;
    u32 hash;
    int unk8[3];
    void* create;
    void* destroy;
    int (*msg)(int* owner, int id, int target, float* pos);
    u32 (*query)(int type, int id);
    void* update;
    void* reset;
    int unk2C;
};

PickupType gHealthPowerupType = {
    "Health Powerup", 0, {0, 0, 0}, 0, 0, PickupBehGivePickup, PickupBehMsgId, 0, 0, 0,
};

// 0x80160DE8
int PickupBehGivePickup(int* owner, int id, int target, float* pos) {
    PickupSpawnMsg spawn;
    PickupValueMsg value;

    if (id == 0x14967) {
        int hash = fn_800F8AF4(*owner);
        switch (hash) {
        case (int)0xECA3D57B:
            value.value = 0.1f;
            break;
        case (int)0xECA3D5B9:
            value.value = 0.3f;
            break;
        case (int)0xECA3D5F7:
            value.value = 0.5f;
            break;
        case (int)0xA7D6DA15:
            value.value = 1.0f;
            break;
        default:
            return 0;
        }
        SendMsg(target, 0xC0A, &value);
        if (value.result == 1) {
            return 0;
        }
        FxStart(FxFind("multi_genericpickup"), 0, 0);
        return 1;
    }
    if (id == 0x2026EF) {
        spawn.handle = fn_800F8AF4(*owner);
        spawn.pos.x = pos[0];
        spawn.pos.y = pos[1];
        spawn.pos.z = pos[2];
        return SendMsg(0x28000000, 0x1500, &spawn);
    }
    return 0;
}

// RobotArm.cpp: the "Possessable Robot Arm" actor type (0x80259448-0x8025951C). A thin wrapper around the
// generic possessable actor: it only allocates its data block and forwards update and messages.
// Split from the GUNWITHGRENADE unit: it has its own string pool (the action-name table followed by its name)
// and its own actor type descriptor (0x803BF7A0).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

void* VAlloc(int size);  // 0x8007D4E0

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Actor;

struct ArmData {
    u8 pad00[0x30];
    int unk30;
    u8 pad34[0x40 - 0x34];
};

struct ArmActor {
    char pad0[0xA4];
    ArmData* data;
    char padA8[4];
    unsigned long flags;
};

void PossessableUpdate(Actor* a, float dt);                  // 0x8020D6C0
int PossessableInit(Actor* a);                               // 0x8020D554
int PossessableMsg(Actor* a, int msg, void* data, int arg);  // 0x8020D738
void PossessableDestroy(Actor* a);                           // 0x8020D4F0

void RobotArmUpdate(ArmActor* a, float dt);
int RobotArmInit(ArmActor* a);
int RobotArmMsg(ArmActor* a, int msg, void* data, int arg);

// The actor type descriptor (0x803BF7A0), laid out like the other possessable actor types.
struct ArmActorType {
    int unk0;
    const char* name;
    int (*msg)(ArmActor* a, int msg, void* data, int arg);
    void (*update)(ArmActor* a, float dt);
    int (*init)(ArmActor* a);
    void* unk14;
    void (*destroy)(Actor* a);
    void* unk1C[5];
};

ArmActorType gRobotArmType = {
    0, "Possessable Robot Arm", RobotArmMsg, RobotArmUpdate, RobotArmInit, 0, PossessableDestroy,
};

void RobotArmUpdate(ArmActor* a, float dt) {
    PossessableUpdate((Actor*)a, dt);
}

int RobotArmInit(ArmActor* a) {
    ArmData* d;

    if (a->data == 0) {
        d = (ArmData*)VAlloc(sizeof(ArmData));
        a->data = d;
    } else {
        d = a->data;
    }
    memset(d, 0, sizeof(ArmData));
    if (!PossessableInit((Actor*)a)) {
        a->flags |= 0x01000000;
        return 0;
    }
    d->unk30 = 0;
    return 1;
}

int RobotArmMsg(ArmActor* a, int msg, void* data, int arg) {
    return PossessableMsg((Actor*)a, msg, data, arg);
}

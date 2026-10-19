// u_8022ED88.cpp: the "Ghost Bee" / "Ghost Bee Swarm" spin actors (0x8022ED88-0x802301E0; merged with the former
// u_8022EEEC: one string pool and two descriptors at 0x803BD988/0x803BD9B8).
// It spins its model and emits three motion trails (the "SPIN" effect). Its messages answer a query
// key; the rest of the actor (update, setup) lives in the next file.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

struct Actor {
    char pad0[0xA4];
    int* trails;
};

void TrailDestroy(int trail);                                     // 0x801DE904
void VFree(void* p);                                              // 0x8009DA3C
int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg);  // 0x800E4C64
int ActorInitBase(Actor* a);                                      // 0x800E6444
int SpinSetup(Actor* a);                                          // 0x8022FF6C

void SpinDestroy(Actor* a) {
    int* t = a->trails;
    if (t) {
        TrailDestroy(t[0]);
        TrailDestroy(t[1]);
        TrailDestroy(t[2]);
        VFree(a->trails);
        a->trails = 0;
    }
}

int SpinInit(Actor* a) {
    if (!ActorInitBase(a)) {
        return 0;
    }
    SpinSetup(a);
    return 1;
}

static inline int SpinCheckKey(void* data) {
    switch (((int*)data)[1]) {
    case 0x530BE0C7:
        return 1;
    }
    return 0;
}

int SpinMsg(Actor* a, int msg, void* data, int arg) {
    int ret = 0;
    switch (msg) {
    case 0x41E:
        ret = SpinCheckKey(data);
        break;
    case 0x41D:
        ret = SpinCheckKey(data);
        break;
    }
    if (ret == 0) return ActorDefaultMsg(a, msg, data, arg);
    return ret == -999999 ? 0 : ret;
}

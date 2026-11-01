// u_80161FF4.cpp: the character behaviour framework and HUD helpers (0x80161FF4-0x8016A774): attaching
// and detaching behaviours from the behaviour type table at 0x803A78C0, behaviour accessors, HUD quads and
// text. Only the small accessors are drafted so far; every larger function is declared with a TODO.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned char u8;

struct Owner;
struct Beh;

typedef int (*BehFunc)(Owner* owner, Beh* beh, int arg);

// Behaviour slot, embedded in the owner (Owner + 0x894).
struct Beh {
    float unk00[4];        // 0x00
    int state;             // 0x10
    u8 pad14[0x2C - 0x14]; // 0x14
    u32 flags;             // 0x2C
    int targetKind;        // 0x30 (1 = position, 2 = actor id)
    union {
        int id;
        float pos[3];
    } target;              // 0x34
    int type;              // 0x40 (behaviour type id, -1 = none)
    BehFunc fnA;           // 0x44
    BehFunc fnB;           // 0x48
    int argC;              // 0x4C
    BehFunc fnC;           // 0x50
    int unk54;             // 0x54
};

struct Owner {
    u8 pad00[0x270];       // 0x00
    u8 cond[0x894 - 0x270];  // 0x270 condition bits
    Beh beh;               // 0x894
};

struct Actor {
    u8 pad00[0x38];  // 0x00
    int id;          // 0x38
};

int HasCondition(void* cond, int bit);                          // 0x800A7AC4
Actor* FollowGetTargetObj(void* cond);                          // 0x800A7A18

// Sets the argument slot of a behaviour.
extern "C" int fn_801625AC(Owner* owner, Beh* beh, int value);
int fn_801625AC(Owner* owner, Beh* beh, int value) {
    if (beh == 0) {
        return 0;
    }
    beh->argC = value;
    return 1;
}

// Calls the behaviour's C hook.
extern "C" int fn_801625C8(Owner* owner, Beh* beh, int arg);
int fn_801625C8(Owner* owner, Beh* beh, int arg) {
    int r;
    if (beh == 0) {
        r = 0;
    } else if (beh->fnC != 0) {
        r = beh->fnC(owner, beh, arg);
    } else {
        r = 0;
    }
    return r;
}

// Calls the behaviour's B hook (used all over the game, the state change callback).
extern "C" int fn_80162610(Owner* owner, Beh* beh, int arg);
int fn_80162610(Owner* owner, Beh* beh, int arg) {
    int r;
    if (beh == 0) {
        r = 0;
    } else if (beh->fnB != 0) {
        r = beh->fnB(owner, beh, arg);
    } else {
        r = 0;
    }
    return r;
}

// Calls the behaviour's A hook.
extern "C" int fn_80162658(Owner* owner, Beh* beh, int arg);
int fn_80162658(Owner* owner, Beh* beh, int arg) {
    int r;
    if (beh == 0) {
        r = 0;
    } else if (beh->fnA != 0) {
        r = beh->fnA(owner, beh, arg);
    } else {
        r = 0;
    }
    return r;
}

// TODO (not drafted yet, 0x104 bytes at 0x801626A0): detaches the behaviour type: looks the type up in the behaviour table and calls its cleanup
extern "C" int fn_801626A0(Owner* owner, Beh* beh);
// TODO (not drafted yet, 0x164 bytes at 0x801627A4): attaches a behaviour type: table lookup, hook install and create call
extern "C" int fn_801627A4(Owner* owner, Beh* beh, int type);

extern "C" int fn_80162908(Owner* owner, Beh* beh);
// Clears the behaviour type and hooks.
int fn_80162908(Owner* owner, Beh* beh) {
    if (beh == 0) {
        return 0;
    }
    beh->type = -1;
    beh->fnA = 0;
    beh->fnC = 0;
    beh->fnB = 0;
    beh->argC = 0;
    beh->unk54 = 0;
    return 1;
}

// Tests behaviour flags.
extern "C" u32 fn_80162940(Beh* beh, u32 mask);
u32 fn_80162940(Beh* beh, u32 mask) {
    if (beh == 0) {
        return 0;
    }
    return beh->flags & mask;
}

// Clears the flags word; named in symbols.txt.
int BehDone(Beh* beh) {
    if (beh == 0) {
        return 0;
    }
    beh->flags = 0;
    return 1;
}

// Returns the state word.
extern "C" int fn_8016325C(Beh* beh);
int fn_8016325C(Beh* beh) {
    if (beh == 0) {
        return 0;
    }
    return beh->state;
}

// Clears the leading vector and state.
int BehClearHead(Beh* beh) {
    if (beh == 0) {
        return 0;
    }
    beh->unk00[0] = 0.0f;
    beh->unk00[1] = 0.0f;
    beh->unk00[2] = 0.0f;
    beh->unk00[3] = 0.0f;
    beh->state = 0;
    return 1;
}

// Picks up the owner's follow target as the behaviour target.
void BehUpdate(Owner* owner, Beh* beh) {
    if (beh != 0) {
        beh->targetKind = 0;
        beh->target.id = -1;
    }
    if (HasCondition(owner->cond, 9) == 0 && HasCondition(owner->cond, 10) == 0 &&
        HasCondition(owner->cond, 11) == 0) {
        Actor* t = FollowGetTargetObj(owner->cond);
        if (t != 0) {
            beh->targetKind = 2;
            beh->target.id = t->id;
        }
    }
}

// Returns the target actor id (or -1 when the target is not an actor).
int BehTarget(Beh* beh) {
    if (beh == 0) {
        return 0;
    }
    if (beh->targetKind != 2) {
        return -1;
    }
    return beh->target.id;
}

// Copies the target position out when the target is a position.
extern "C" int fn_80163378(Beh* beh, float* out);
int fn_80163378(Beh* beh, float* out) {
    if (beh == 0) {
        return 0;
    }
    if (beh->targetKind != 1) {
        return 0;
    }
    out[0] = beh->target.pos[0];
    out[1] = beh->target.pos[1];
    out[2] = beh->target.pos[2];
    return 1;
}

// Returns the target kind.
extern "C" int fn_801633BC(Beh* beh);
int fn_801633BC(Beh* beh) {
    if (beh == 0) {
        return 0;
    }
    return beh->targetKind;
}

// Clears the target.
int BehReset(Beh* beh) {
    if (beh == 0) {
        return 0;
    }
    beh->targetKind = 0;
    beh->target.id = -1;
    return 1;
}

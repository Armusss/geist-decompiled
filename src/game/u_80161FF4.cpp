// u_80161FF4.cpp: the character behaviour framework and HUD helpers (0x80161FF4-0x8016A774): attaching
// and detaching behaviours from the behaviour type table at 0x803A78C0, behaviour accessors, HUD quads and
// text. Only the small accessors are drafted so far; every larger function is declared with a TODO.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned char u8;

struct Owner;
struct Beh;

struct Actor {
    u8 pad00[0x38];  // 0x00
    int id;          // 0x38
};

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


// HUD owner block (only the fields used here).
struct Hud {
    u8 pad0000[0x1500];       // 0x0000
    float unk1500;            // 0x1500
    u8 pad1504[0x1510 - 0x1504];  // 0x1504
    int unk1510;              // 0x1510
    u8 pad1514[0x1518 - 0x1514];  // 0x1514
    float unk1518;            // 0x1518
    u8 pad151C[0x1564 - 0x151C];  // 0x151C
    float unk1564;            // 0x1564
    u8 pad1568[0x1590 - 0x1568];  // 0x1568
    int unk1590[4];           // 0x1590
    int unk15A0[4];           // 0x15A0
    u8 pad15B0[0x15B4 - 0x15B0];  // 0x15B0
    int unk15B4;              // 0x15B4
    u8 pad15B8[0x26E4 - 0x15B8];  // 0x15B8
    Actor* unk26E4;           // 0x26E4
    u8 pad26E8[0x2730 - 0x26E8];  // 0x26E8
    float unk2730;            // 0x2730
    float unk2734;            // 0x2734
};

// HUD element (only the fields used here).
struct HudElem {
    u8 pad00[0x10];           // 0x00
    float unk10;              // 0x10
    float unk14[4];           // 0x14
    float unk24;              // 0x24
    float unk28;              // 0x28
    float unk2C;              // 0x2C
    float unk30;              // 0x30
    float unk34;              // 0x34
    float unk38;              // 0x38
    u8 pad3C[0x50 - 0x3C];    // 0x3C
    float unk50;              // 0x50
    u8 pad54[0x58 - 0x54];    // 0x54
    float unk58;              // 0x58
    float unk5C;              // 0x5C
    u8 pad60[0x84 - 0x60];    // 0x60
    int unk84;                // 0x84
};

int HasCondition(void* cond, int bit);                          // 0x800A7AC4
Actor* FollowGetTargetObj(void* cond);                          // 0x800A7A18
int PlayerInHost(Actor* a);                                     // 0x80049750
struct SplineNameEntry;
int SplineAssetResolve(SplineNameEntry* names, int a, int b);   // 0x80101FC8
extern "C" void fn_8000F60C(int id);
extern "C" void fn_8000F874(int id);
extern SplineNameEntry lbl_803A7B58[];
extern int lbl_804D2F90;
extern "C" void fn_8016A3D0(int a, int b, int c, int d, int e);

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

// Reads the HUD word at +0x1510.
int HudGetUnk1510(Hud* hud) {
    return hud->unk1510;
}

// Writes the HUD word at +0x1510.
void HudSetUnk1510(Hud* hud, int value) {
    hud->unk1510 = value;
}

// TODO (not drafted yet, 0x38 bytes at 0x80163554): calls Fn8008E33C(lbl_8046DFC8, 0) when the id is valid (that call passes no float)

// Shows one of two HUD prompts.
extern "C" void fn_8016359C(int on);
void fn_8016359C(int on) {
    if (on != 0) {
        fn_8016A3D0(0x12, 0xFF, -1, 0, 0);
    } else {
        fn_8016A3D0(0x11, 0xFF, -1, 0, 0);
    }
}

// Sets four HUD words at +0x15A0.
extern "C" void fn_80163810(Hud* hud, int a, int b, int c, int d);
void fn_80163810(Hud* hud, int a, int b, int c, int d) {
    hud->unk15A0[0] = a;
    hud->unk15A0[1] = b;
    hud->unk15A0[2] = c;
    hud->unk15A0[3] = d;
}

// Sets four HUD words at +0x1590.
extern "C" void fn_80163824(Hud* hud, int a, int b, int c, int d);
void fn_80163824(Hud* hud, int a, int b, int c, int d) {
    hud->unk1590[0] = a;
    hud->unk1590[1] = b;
    hud->unk1590[2] = c;
    hud->unk1590[3] = d;
}

// Resets the element's timer value.
void HudElemSetTimer(Hud* hud, HudElem* e) {
    if (hud == 0) {
        return;
    }
    if (e == 0) {
        return;
    }
    e->unk50 = 1500.0f;
}

// Switches the element's texture; -2 picks the default arrow texture.
extern "C" void fn_80163854(Hud* hud, HudElem* e, int tex);
void fn_80163854(Hud* hud, HudElem* e, int tex) {
    if (hud != 0 && e != 0) {
        if (tex == -2) {
            tex = SplineAssetResolve(lbl_803A7B58, 2, 1);
        }
        if (e->unk84 != tex) {
            if (e->unk84 != -1) {
                fn_8000F60C(e->unk84);
                e->unk84 = -1;
            }
            e->unk84 = tex;
            fn_8000F874(e->unk84);
        }
    }
}

// Sets the element's position and default scale/alpha.
void HudElemSetPos(float x, float y, float z, Hud* hud, HudElem* e) {
    if (hud == 0) {
        return;
    }
    if (e == 0) {
        return;
    }
    e->unk24 = x;
    e->unk28 = y;
    e->unk2C = z;
    e->unk30 = 0.2f;
    e->unk34 = 0.2f;
    e->unk38 = 500.0f;
}

// Sets the element's position and size from five values.
extern "C" void fn_80163918(float a, float b, float c, float d, float e, Hud* hud, HudElem* el);
void fn_80163918(float a, float b, float c, float d, float e, Hud* hud, HudElem* el) {
    if (hud == 0) {
        return;
    }
    if (el == 0) {
        return;
    }
    el->unk24 = a;
    el->unk28 = b;
    el->unk2C = c;
    el->unk30 = d;
    el->unk34 = d;
    el->unk38 = e;
}

// Sets one of the element's array slots to the default value.
void HudElemSetSlot(Hud* hud, HudElem* e, int index) {
    if (hud == 0) {
        return;
    }
    if (e == 0) {
        return;
    }
    e->unk14[index] = 1000.0f;
}

// Picks the element's base value from the HUD owner state.
void HudElemInit(Hud* hud, HudElem* e) {
    if (hud != 0 && e != 0) {
        if (lbl_804D2F90 != 0) {
            if (PlayerInHost((Actor*)hud) != 0) {
                if (hud->unk2734 > 0.0f) {
                    e->unk10 = hud->unk2730;
                } else {
                    e->unk10 = 100.0f;
                }
            } else {
                e->unk10 = hud->unk1500;
            }
        } else if (PlayerInHost((Actor*)hud) != 0 && (int)((u32)hud->unk26E4->id >> 24) != 1) {
            e->unk10 = hud->unk2730;
        } else {
            e->unk10 = hud->unk1500;
        }
        e->unk5C = e->unk10;
        e->unk58 = 0.0f;
    }
}

// Returns -1 without a block, 1 when both words are set, else 0.
int HudBlockReady(void* unused, int* block) {
    if (block == 0) {
        return -1;
    }
    u8 r = 0;
    if (block[0] != 0 && block[1] != 0) {
        r = 1;
    }
    return r;
}

// Stores a word into the block.
void HudBlockSet(void* unused, int* block, int value) {
    if (block == 0) {
        return;
    }
    block[0] = value;
}

// Clears the HUD flag at +0x15B4 (called from another unit).
extern "C" void fn_80165D9C(Hud* hud);
void fn_80165D9C(Hud* hud) {
    hud->unk15B4 = 0;
}

// Sets the HUD flag at +0x15B4 (called from another unit).
extern "C" void fn_80165DA8(Hud* hud);
void fn_80165DA8(Hud* hud) {
    hud->unk15B4 = 1;
}

// Lowers two HUD gauges by the given amount.
void HudGaugesSubtract(Hud* hud, float amount) {
    if (hud == 0) {
        return;
    }
    hud->unk1564 = hud->unk1564 - amount;
    hud->unk1518 = hud->unk1518 - amount;
}

// TODO (not drafted yet, 0x5C bytes at 0x80163FB0): clears six HUD words, calls fn_80111404 and resets two fields
extern "C" void fn_80163FB0(Hud* hud);
// TODO (not drafted yet, 0x48 bytes at 0x80168B70): stores five values into globals and the table at 0x803A7BFC
extern "C" void fn_80168B70(int a, int b, int c, int d, int e, int f);

// TODO (not drafted yet, 0x5B8 bytes at 0x80161FF4): behaviour spawn helper
void Fn80161FF4(Owner* owner, void* data, float t);
// TODO (not drafted yet, 0x560 bytes at 0x8016295C): behaviour state update
void Fn8016295C(Owner* owner, Beh* beh);
// TODO (not drafted yet, 0x380 bytes at 0x80162EDC): resets a behaviour slot
extern "C" int fn_80162EDC(Beh* beh);
// TODO (not drafted yet, 0x158 bytes at 0x801633FC): HUD marker sprite by player kind
extern "C" void fn_801633FC(u32 id, float* color, int arg);
// TODO (not drafted yet, 0x21C bytes at 0x801635F4): draws a HUD frame from six quads
extern "C" void fn_801635F4();
// TODO (not drafted yet, 0x530 bytes at 0x80163A80): HUD init
extern "C" void fn_80163A80();
// TODO (not drafted yet, 0x12C bytes at 0x8016400C)
extern "C" void fn_8016400C();
// TODO (not drafted yet, 0xB80 bytes at 0x80164138): HUD update
extern "C" void fn_80164138();
// TODO (not drafted yet, 0x584 bytes at 0x80164CB8)
extern "C" void fn_80164CB8();
// TODO (not drafted yet, 0x4A8 bytes at 0x8016523C)
extern "C" void fn_8016523C();
// TODO (not drafted yet, 0x6B8 bytes at 0x801656E4)
extern "C" void fn_801656E4();
// TODO (not drafted yet, 0xCBC bytes at 0x80165DB4)
extern "C" void fn_80165DB4();
// TODO (not drafted yet, 0x470 bytes at 0x80166A70)
extern "C" void fn_80166A70();
// TODO (not drafted yet, 0x398 bytes at 0x80166EE0)
extern "C" void fn_80166EE0();
// TODO (not drafted yet, 0x53C bytes at 0x80167278)
extern "C" void fn_80167278();
// TODO (not drafted yet, 0x550 bytes at 0x801677B4)
extern "C" void fn_801677B4();
// TODO (not drafted yet, 0x198 bytes at 0x80167D04): host_action message
extern "C" void fn_80167D04();
// TODO (not drafted yet, 0x5EC bytes at 0x80167E9C)
extern "C" void fn_80167E9C();
// TODO (not drafted yet, 0x4DC bytes at 0x80168488)
extern "C" void fn_80168488();
// TODO (not drafted yet, 0x1E8 bytes at 0x80168988)
extern "C" void fn_80168988();
// TODO (not drafted yet, 0x1438 bytes at 0x80168BB8)
extern "C" void fn_80168BB8();
// TODO (not drafted yet, 0x168 bytes at 0x80169FF0)
extern "C" void fn_80169FF0();
// TODO (not drafted yet, 0x278 bytes at 0x8016A158)
extern "C" void fn_8016A158();
// TODO (not drafted yet, 0x3A4 bytes at 0x8016A3D0): HUD message printer

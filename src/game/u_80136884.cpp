// u_80136884.cpp: player gun (held weapon) message handling (0x80136884-0x8013A3D4).
// The gun action handler queries the weapon's two linked names through message 0x41E,
// then dispatches on the action hash: it starts the "gun_empty" effect when the weapon
// runs dry, switches the held-slot/action modes, and forwards the two names to the
// motion system. The per-type table callback starts a type's message handler.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;

struct PlayerRef;
struct GunOwner;

// Fields of the player actor used by this unit (full layout lives in its own unit).
struct Actor {
    u8 pad00[0x38];               // 0x0000
    int handle;                   // 0x0038
    u8 pad3C[0x50 - 0x3C];        // 0x003C
    u32 field50;                  // 0x0050
    u8 pad54[0x58 - 0x54];        // 0x0054
    int field58;                  // 0x0058 model
    u8 pad5C[0x78 - 0x5C];        // 0x005C
    int field78;                  // 0x0078
    int field7C;                  // 0x007C
    u8 pad80[0x8C - 0x80];        // 0x0080
    int field8C;                  // 0x008C
    u8 pad90[0x98 - 0x90];        // 0x0090
    int field98;                  // 0x0098
    u8 pad9C[0xA4 - 0x9C];        // 0x009C
    int fieldA4;                  // 0x00A4
    u8 padA8[0xC4 - 0xA8];        // 0x00A8
    int fieldC4;                  // 0x00C4
    u8 padC8[0xE4 - 0xC8];        // 0x00C8
    int fieldE4;                  // 0x00E4 state callback
    u8 padE8[0xF0 - 0xE8];        // 0x00E8
    int fieldF0;                  // 0x00F0
    u8 padF4[0xFC - 0xF4];        // 0x00F4
    int fieldFC;                  // 0x00FC
    u8 pad100[0x11C - 0x100];     // 0x0100
    int field11C;                 // 0x011C
    u8 pad120[0x2F4 - 0x120];     // 0x0120
    u8 field2F4[0x314 - 0x2F4];   // 0x02F4 weapon gauge timer block
    int field314;                 // 0x0314
    int field318;                 // 0x0318
    u8 pad31C[0x320 - 0x31C];     // 0x031C
    u32 field320;                 // 0x0320
    u32 field324;                 // 0x0324
    int field328;                 // 0x0328
    u8 pad32C[0x38C - 0x32C];     // 0x032C
    float field38C;               // 0x038C
    float field390;               // 0x0390
    float field394;               // 0x0394
    float field398;               // 0x0398
    u8 pad39C[0x588 - 0x39C];     // 0x039C
    int field588;                 // 0x0588 held-gun action state
    u8 pad58C[0x5E8 - 0x58C];     // 0x058C
    float field5E8;               // 0x05E8 gauge charge
    u8 pad5EC[0x638 - 0x5EC];     // 0x05EC
    int field638;                 // 0x0638
    u8 pad63C[0x678 - 0x63C];     // 0x063C
    u32 field678;                 // 0x0678
    u8 pad67C[0x73C - 0x67C];     // 0x067C
    int field73C;                 // 0x073C
    u8 pad740[0x7A4 - 0x740];     // 0x0740
    int type;                     // 0x07A4
    int nextType;                 // 0x07A8
    u8 pad7AC[0x7B4 - 0x7AC];     // 0x07AC
    u32 flags;                    // 0x07B4
    u8 pad7B8[0x7D0 - 0x7B8];     // 0x07B8
    int field7D0;                 // 0x07D0
    int field7D4;                 // 0x07D4 random variant flag
    float field7D8;               // 0x07D8
    int field7DC;                 // 0x07DC
    u8 pad7E0[0x14FC - 0x7E0];    // 0x07E0
    float speed;                  // 0x14FC
    u8 pad1500[0x1D04 - 0x1500];  // 0x1500
    char buf[96];                 // 0x1D04
    u8 pad1D64[0x26B0 - 0x1D64];  // 0x1D64
    int field26B0;                // 0x26B0 model/bone id, -1 when unset
    u8 pad26B4[0x26C4 - 0x26B4];  // 0x26B4
    PlayerRef* refs[7];           // 0x26C4 player reference, one per slot
    u8 field26E0[0x26E4 - 0x26E0];// 0x26E0
    GunOwner* field26E4;          // 0x26E4 weapon owner
    int field26E8;                // 0x26E8
    u8 pad26EC[0x279C - 0x26EC];  // 0x26EC
    int field279C;                // 0x279C
};

// Weapon/owner state reached through Actor::field26E8.
struct GunOwner {
    u8 pad00[0x38];               // 0x0000
    u32 flags38;                  // 0x0038
    u8 pad3C[0x270 - 0x3C];       // 0x003C
    u8 cond[4];                   // 0x0270
};

// The player reference returned by fn_800A384C.
struct PlayerRef {
    u8 pad00[0x14];               // 0x0000
    int field14;                  // 0x0014
    int field18;                  // 0x0018
    int field1C;                  // 0x001C
};

// Per-type dispatch table entry (base lbl_803A00D0, stride 0x48).
struct TypeEntry {
    void (*cb00)(Actor*);         // 0x00
    void (*cb04)(Actor*);         // 0x04
    void (*msg)(Actor*);          // 0x08
    u8 pad0C[0x48 - 0x0C];        // 0x0C
};

extern TypeEntry lbl_803A00D0[];

// Payload of the 0xC06 "can I act" query.
struct MsgQuery {
    int result;                   // 0x00
    u32 hash;                     // 0x04
    int pad08;                    // 0x08
};

// Payload of the 0x41E name request; the name object follows it in memory.
struct NameMsg {
    int pad00;                    // 0x00
    u32 nameHash;                 // 0x04
    int pad08;                    // 0x08
};

// Local view of the String object the receiver writes the queried name into.
class String {
public:
    String();
    ~String();
    int field_0;                  // 0x00
};

extern "C" {
void fn_800769A0(Actor* a, float f);                          // 0x800769A0
void* fn_800A384C(void* p);                                   // 0x800A384C
int fn_800A1BC8(int a, int b, int c, int d, int e);           // 0x800A1BC8
int fn_800C5210(int to, int msg, void* data);                 // 0x800C5210
int fn_800498D0(Actor* a);                                    // 0x800498D0
void fn_80048BA8(Actor* a, void* buf, int mode);              // 0x80048BA8
int SendMsg(int to, int msg, void* data);                     // 0x800265D4
void FxStart(void* fx, int a, int b);                         // 0x8016F390
int fn_80175EFC(void);                                        // 0x80175EFC
void fn_80155A84(void* p, int a, int b);                      // 0x80155A84
int rand(void);
}
int PlayerGunActionFallback(void* p, float f);                // 0x80139670
int PlayerGunGaugeState(void* p);                             // 0x801399A8
int PlayerGunActionMsg(int action, void* p);                  // 0x80139C48
extern "C" {
void fn_800F7AF0(int a, int b);                                // 0x800F7AF0
void fn_80050EB0(Actor* a);                                    // 0x80050EB0
}
int ModelBoneName(int model, int bone);                        // 0x80018B44
int ModelBoneNodeChain(int model, int name, int* table, int count); // 0x80018814
extern int lbl_8046D270[16];                                   // 0x8046D270
extern float lbl_804D7A58;                                     // 0x804D7A58
extern float lbl_804D7A5C;                                     // 0x804D7A5C
extern float lbl_804D7A60;                                     // 0x804D7A60
extern float lbl_804D7A70;                                     // 0x804D7A70
extern float lbl_804D7A74;                                     // 0x804D7A74
float fn_800699B8(void* p);                                    // 0x800699B8
void fn_800699A8(void* p);                                     // 0x800699A8
void fn_8008E824(int a, int b);                                // 0x8008E824
float fabsf(float f);
void PlayerSetActionMode(void* owner, int mode);
void PlayerSetHeldSlotMode(void* owner, int mode);
void PlayerSetHeldSlotIndex(void* owner, int index);          // 0x8006CAC0
void PlayerSelectCollisionUpdate(void* p, float f);            // 0x80072058
void PlayerSetValue4(void* p, int v);                          // 0x8006D17C
void PlayerResetGunTimer(void* p);                             // 0x8006D76C
u32 AssetFind(u32 group, u32 hash);                            // 0x8007EB34
extern u32 lbl_8046D230[16];                                   // 0x8046D230
extern u8 lbl_803A07D8[];                                      // 0x803A07D8
extern u32 lbl_804D4D00;                                       // 0x804D4D00
extern char lbl_804D4D04;                                      // 0x804D4D04
extern void* lbl_804D4D20;                                     // 0x804D4D20
extern "C" {
void* fn_801D89A4(void* p);                                    // 0x801D89A4
void fn_8004320C(void* cam, void* buf);                        // 0x8004320C
void fn_80009324(void* buf, int mode);                         // 0x80009324
void fn_800092CC(void);                                        // 0x800092CC
int fn_80010448(int view, int mode);                           // 0x80010448
void fn_80010330(int view, int mode);                          // 0x80010330
void GXSetAlphaUpdate(int enable);                             // 0x8030B538
void GXSetColorUpdate(int enable);                             // 0x8030B50C
}
void GfxSetVtxDesc(int desc);
void GfxSetCullMode(int mode);
int PlayerInHost(Actor* a);                                    // 0x80049750
int HasCondition(void* p, int v);                              // 0x800A7AC4
int PlayerGetHeldSlotIndex(void* p);                           // 0x8006CAB8
void DashUpdate(Actor* a);                                     // 0x80243DC8
void ActorPlayAnim(int handle, u32 anim, int value);           // 0x800C5654
int Fn800F7DEC(int a, int b, int c, int d, void* e);          // 0x800F7DEC
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f); // 0x800F7F5C
void* FxFind(const char* name);                               // 0x80170834

// Requests a name through message 0x41E; the receiver leaves it in the trailing object.
static inline int NameQuery(int target, int msg, u32 hash) {
    NameMsg m;
    String name;

    name.field_0 = 0;
    m.nameHash = hash;
    m.pad08 = 0;
    SendMsg(target, msg, &m);
    return name.field_0;
}

// 0x80136884: player gun action handler.
int PlayerGunAction(int action, void* p) {
    Actor* actor = (Actor*)p;
    MsgQuery q0;
    int handle = actor->handle;
    int r1;
    int r2;

    q0.hash = action;
    if (fn_800C5210(handle, 0xC06, &q0) == 1) {
        return 0;
    }
    r1 = NameQuery(actor->handle, 0x41E, 0x2DA8293C);
    r2 = NameQuery(actor->handle, 0x41E, 0xDFF3097A);

    switch (action) {
    case 0x1B83833F:
        if (actor->type == 5) {
            int r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(actor), 0);
            if (r == 0x04A3E183) {
                PlayerSetActionMode(actor, actor->nextType);
            } else {
                if (r == 0xC0BECF7E || r == 0x11D2C025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                PlayerSetHeldSlotMode(actor, 3);
            }
        } else if (actor->type == 8) {
            int r = Fn800F7DEC(r1, r2, 0xF829DB93, fn_800498D0(actor), 0);
            if (r == 0x04A3E183) {
                PlayerSetActionMode(actor, actor->nextType);
            } else {
                if (r == 0xC0BECF7E || r == 0x11D2C025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                PlayerSetHeldSlotMode(actor, 3);
            }
        } else if (actor->type == 0xD) {
            actor->flags |= 0x20000;
        } else if (actor->type == 0xA) {
            int r = Fn800F7DEC(r1, r2, 0xE6B71BCC, fn_800498D0(actor), 0);
            if (r == 0x04A3E183) {
                PlayerSetActionMode(actor, 10);
            } else {
                PlayerSetHeldSlotMode(actor, 15);
            }
        } else {
            PlayerSetActionMode(actor, actor->nextType);
        }
        break;
    case 0x9791AF51:
    case 0x96224FCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        fn_80048BA8(actor, actor->buf, 0);
        Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        break;
    case 0xF829DB93:
        fn_80048BA8(actor, actor->buf, 0);
        Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        break;
    case 0xE6B71BCC:
        fn_80048BA8(actor, actor->buf, 0);
        Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        break;
    case 0x968CBE3D:
        fn_80048BA8(actor, actor->buf, 0);
        Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        break;
    case 0x0020EE36:
        fn_80048BA8(actor, actor->buf, 0);
        Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        break;
    }
    return 0;
}

// 0x80136D00: runs the current type's message callback, if it has one.
void PlayerGunTypeMsg(void* p) {
    Actor* actor = (Actor*)p;

    if (lbl_803A00D0[actor->type].msg) {
        lbl_803A00D0[actor->type].msg(actor);
    }
}

// 0x80136D44: runs the current type's 0x04 callback, if it has one.
void PlayerGunTypeCallback04(void* p) {
    Actor* actor = (Actor*)p;

    if (lbl_803A00D0[actor->type].cb04) {
        lbl_803A00D0[actor->type].cb04(actor);
    }
}

// 0x80136D88: runs the current type's 0x00 callback, if it has one.
void PlayerGunTypeCallback00(void* p) {
    Actor* actor = (Actor*)p;

    if (lbl_803A00D0[actor->type].cb00) {
        lbl_803A00D0[actor->type].cb00(actor);
    }
}

// 0x80136DC8: resolves the sixteen assets used by the gun code into the table
// that starts at 0x8046D230.
// fn_80136DC8
extern "C" void fn_80136DC8(void) {
    lbl_8046D230[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_8046D230[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_8046D230[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_8046D230[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_8046D230[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_8046D230[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_8046D230[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_8046D230[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_8046D230[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_8046D230[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_8046D230[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_8046D230[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_8046D230[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_8046D230[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_8046D230[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_8046D230[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

// 0x80136FE4: drops the gun's active flag and picks the follow-up state from
// the player's weapon slot.
void PlayerGunLeaveState(void* p, float f) {
    Actor* a = (Actor*)p;

    a->field638 = 0;
    a->flags &= ~1;
    fn_800769A0(a, f);
    PlayerSelectCollisionUpdate(a, f);
    if (a->fieldC4 == -1) {
        if (a->field7D0 == 0) {
            PlayerSetValue4(&a->type, 1);
        } else {
            PlayerSetValue4(&a->type, 3);
        }
    } else {
        PlayerSetValue4(&a->type, 0x16);
    }
}

// 0x80137080: clears the gun's aim state and copies the weapon slot and the
// follow-up state into their active fields.
void PlayerGunResetState(void* p) {
    Actor* a = (Actor*)p;

    a->field8C = 0;
    if (a->fieldC4 != -1) {
        a->field98 = a->fieldC4;
    }
    if (a->field11C == -1) {
        return;
    }
    a->fieldF0 = a->field11C;
}

// 0x801370AC: re-arms the gun after the player fires or switches weapons.
void PlayerGunReactivate(void* p) {
    Actor* a = (Actor*)p;

    PlayerResetGunTimer(a);
    a->field638 = 1;
    a->flags &= ~1;
    PlayerSetValue4(&a->type, 0x15);
    if (a->field279C == -1 || fn_80175EFC() != 0) {
        PlayerSetValue4(&a->type, 3);
        fn_80155A84(&a->field26E0, a->field26E8, 1);
    }
}

// 0x8013712C: starts the gun action. The two linked names become the action and
// follow-up values, the state callback is the name-intro handler, and the slot
// selector is armed.
void PlayerGunStartAction(void* p) {
    Actor* a = (Actor*)p;
    GunOwner* g = (GunOwner*)a->field26E8;
    PlayerRef* ref;
    int flag = 0;
    int kind;
    int* types;

    if ((g->flags38 >> 24) == 0x13) {
        ref = (PlayerRef*)fn_800A384C(g);
        if (HasCondition(g->cond, 0xB) != 0) {
            flag = 1;
        }
    }
    PlayerInHost(a);
    kind = a->field73C;
    types = (int*)(lbl_803A07D8 + 0x5F4);
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, types[kind], flag, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 0x19;
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, types[kind], flag, 0);
    a->fieldFC = 0x19;
    PlayerSetValue4(&a->type, 0x15);
}

// 0x80137228: per-frame gun update; re-arms the timer, clears the active flag and
// the first flag bit, then picks the slot mode from the held slot.
void PlayerGunSelectUpdate(void* p, float f) {
    Actor* a = (Actor*)p;

    PlayerResetGunTimer(a);
    a->field638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, f);
    PlayerSelectCollisionUpdate(a, f);
    if (PlayerGetHeldSlotIndex(a) != 0) {
        PlayerSetValue4(&a->type, 2);
    } else {
        PlayerSetValue4(&a->type, 1);
    }
    PlayerInHost(a);
    DashUpdate(a);
}

// 0x801372C8: starts the gun action for the current weapon slot. The slot's two
// linked names become the action and follow-up values, the name-intro handler is
// installed as the state callback, and the held-slot and action modes are chosen
// from the queried weapon name.
void PlayerGunStartActionSlot(void* p) {
    Actor* a = (Actor*)p;
    PlayerRef* ref;
    int slot;
    int kind;
    int* types;
    int name;

    slot = PlayerInHost(a);
    kind = a->field73C;
    a->field78 = 0;
    a->field50 &= ~0x20000u;
    ref = a->refs[slot];
    types = (int*)(lbl_803A07D8 + 0x5AC) + kind;
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, *types, 0, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 1;
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, *types, 0, 0);
    a->fieldFC = 1;
    name = NameQuery(a->field26E4->flags38, 0x41E, 0x08783452);
    if (name != 0) {
        PlayerSetHeldSlotIndex(a, 0);
        if (a->field98 == -1) {
            PlayerSetActionMode(a, 1);
        }
    } else {
        PlayerSetHeldSlotIndex(a, 1);
        if (a->field98 == -1) {
            PlayerSetActionMode(a, 3);
        }
    }
}

// 0x80137438: re-arms the gun timer; unless the gun is already leaving (bit 0 of
// the flags), it sets the active flag and the pending-state field.
void PlayerGunResumeState(void* p) {
    Actor* a = (Actor*)p;

    PlayerResetGunTimer(a);
    a->field638 = 1;
    if (a->flags & 1) {
        a->flags &= ~1;
    } else {
        a->flags |= 0x20000;
        a->field78 = 1;
    }
}

// 0x80137494: starts the gun action from the player's hold state. The weapon's
// two linked names become the action and follow-up values and the name-intro
// handler is installed as the state callback; the held-slot mode is chosen from
// the queried weapon name.
void PlayerGunStartActionWeapon(void* p) {
    Actor* a = (Actor*)p;
    GunOwner* g = (GunOwner*)a->field26E8;
    PlayerRef* ref;
    int flag = 0;
    int slot;
    int kind;
    int* types;
    int name;

    if ((g->flags38 >> 24) == 0x13) {
        ref = (PlayerRef*)fn_800A384C(g);
        if (HasCondition(g->cond, 0xB) != 0) {
            flag = 1;
        }
    }
    slot = PlayerInHost(a);
    kind = a->field73C;
    types = (int*)(lbl_803A07D8 + 0x564) + kind;
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, *types, flag, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 0x19;
    a->fieldF0 = fn_800A1BC8(a->refs[slot]->field14, a->refs[slot]->field1C,
                             *types, 0, 0);
    a->fieldFC = 0x19;
    name = NameQuery(g->flags38, 0x41E, 0x08783452);
    if (name != 0) {
        PlayerSetHeldSlotIndex(a, 0);
    } else {
        PlayerSetHeldSlotIndex(a, 1);
    }
}

// 0x801375F4: hands the gun's hold state back to its owner.
void PlayerGunHoldReset(void* p) {
    Actor* a = (Actor*)p;

    fn_80155A84(&a->field26E0, a->field26E8, 1);
}

// 0x80137620: reset the gun's action state after the player switches weapon.
void PlayerGunReset(Actor* actor) {
    PlayerResetGunTimer(actor);
    actor->field638 = 1;
    actor->flags &= ~1u;
    actor->field50 |= 0x20000;
    actor->field78 = 1;
    PlayerSetValue4(&actor->type, 0x13);
}

// 0x8013767C: set up the gun's two linked state slots, the gun action callback
// and the held-slot index from the weapon's action-name entries.
void PlayerGunSetup(Actor* actor) {
    int slot = PlayerInHost(actor);
    GunOwner* owner = (GunOwner*)actor->field26E8;
    int idx = actor->field73C;
    PlayerRef* ref = 0;
    int flag = 0;
    u32 name;

    if (owner->flags38 >> 24 == 0x13) {
        ref = (PlayerRef*)fn_800A384C(owner);
        if (HasCondition(owner->cond, 0xB) != 0) {
            flag = 1;
        }
    }
    actor->field98 = fn_800A1BC8(ref->field14, ref->field18,
                                 ((u32*)(lbl_803A07D8 + 0x51C))[idx], flag, 0);
    actor->field8C = (int)PlayerGunActionMsg;
    actor->fieldA4 = 0x59;
    ref = actor->refs[slot];
    actor->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C,
                                 ((u32*)(lbl_803A07D8 + 0x51C))[idx], 0, 0);
    actor->fieldFC = 0x59;
    name = (u32)NameQuery(owner->flags38, 0x41E, 0x08783452);
    PlayerSetHeldSlotIndex(actor, name == 0 ? 1 : 0);
}

// 0x801377D8: empty placeholder, kept to preserve the unit's function order.
void PlayerGunNoop(void) {}

// 0x801377DC: follow-state gun update. Re-queries the weapon's two linked names;
// when the follow-up action hash no longer matches the stored one it forces action
// mode 0x11. Then it clears the active state, runs the shared update, queries the
// names again and either starts the slot-0x11 name intro or rearms the gun timer.
void PlayerGunFollowUpdate(Actor* actor, float dt) {
    int r1 = NameQuery(actor->handle, 0x41E, 0x2DA8293C);
    int r2 = NameQuery(actor->handle, 0x41E, 0xDFF3097A);

    if (Fn800F7DEC(r1, r2, 0x6FAB23FE, fn_800498D0(actor), 0) != actor->field7DC) {
        PlayerSetActionMode(actor, 0x11);
    }
    actor->field638 = 1;
    actor->flags &= ~1;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);
    DashUpdate(actor);
    r1 = NameQuery(actor->handle, 0x41E, 0x2DA8293C);
    r2 = NameQuery(actor->handle, 0x41E, 0xDFF3097A);
    if (actor->field314 == 0) {
        PlayerSetValue4(&actor->type, 3);
        if (actor->type == 0x11) {
            fn_80048BA8(actor, actor->buf, 1);
            Fn800F7F5C(r1, r2, 0x0D19E338, fn_800498D0(actor), actor->buf,
                       actor->speed);
            PlayerSetHeldSlotMode(actor, 5);
        }
    } else {
        PlayerResetGunTimer(actor);
    }
}

// 0x80137A94: sets up the gun action from the two linked weapon names: the
// name-intro handler becomes the state callback, the action hash selects the
// name-intro mode, and the per-slot table provides the two motion names.
void PlayerGunSetupNames(void* p) {
    Actor* a = (Actor*)p;
    int r1;
    int r2;
    int h;
    int mode;
    int slot;
    int kind;
    PlayerRef* ref;
    u32* types;

    r1 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
    r2 = NameQuery(a->handle, 0x41E, 0xDFF3097A);
    h = Fn800F7DEC(r1, r2, 0x6FAB23FE, fn_800498D0(a), 0);
    a->field7DC = h;
    switch (h) {
    case 0x7183F329:
        mode = 0;
        break;
    case -0x410777B3:
        mode = 2;
        break;
    case 0x7183F5A1:
        mode = 1;
        break;
    }
    slot = PlayerInHost(a);
    kind = a->field73C;
    ref = a->refs[slot];
    types = (u32*)(lbl_803A07D8 + 0x4D4);
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, types[kind], mode, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 1;
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, types[kind], 0, 0);
    a->fieldFC = 1;
}

// 0x80137C74: clears the gun's "action pending" bit.
void PlayerGunClearPendingFlag(void* p) {
    Actor* a = (Actor*)p;

    a->field50 &= ~0x40000u;
}

// 0x80137C84: resets the gun timer and the "action pending" bit, then runs the
// player's collision update for this frame with the frame delta.
void PlayerGunResetCollision(void* p, float dt) {
    Actor* a = (Actor*)p;

    PlayerResetGunTimer(a);
    a->field638 = 1;
    a->flags &= ~1u;
    PlayerSelectCollisionUpdate(a, dt);
}

// 0x80137CDC: sets the gun action up from the weapon's two linked names. The
// names are forwarded to the motion system, the per-slot/per-type motion names
// become the gun's two state slots, and the bone name of the weapon model picks
// the second slot's variant (8 when the model has no such bone).
void PlayerGunSetupState(void* p) {
    Actor* a = (Actor*)p;
    int r1;
    int r2;
    int slot;
    int kind;
    int bone;
    PlayerRef* ref;
    u32* types;

    r1 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
    r2 = NameQuery(a->handle, 0x41E, 0xDFF3097A);
    PlayerResetGunTimer(a);
    a->field50 |= 0x40000;
    fn_800F7AF0(r1, r2);
    fn_80050EB0(a);

    slot = PlayerInHost(a);
    kind = a->field73C;
    ref = a->refs[slot];
    types = (u32*)(lbl_803A07D8 + 0x3B4);
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, types[kind], 0, 0);
    a->fieldA4 = 0x50;
    if (a->field26B0 == -1) {
        bone = 8;
    } else {
        bone = ModelBoneName(a->field58, a->field26B0);
        bone = ModelBoneNodeChain(a->field58, bone, lbl_8046D270, 0x10);
        if (bone == -1) {
            bone = 8;
        }
    }
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, types[kind], bone << 1, 0);
    a->fieldE4 = (int)PlayerGunActionMsg;
    a->fieldFC = 0x50;
}

// 0x80137EC4: re-arms the gun for the follow state, runs the shared frame update
// (collision + dash) and switches the action state to 3.
void PlayerGunReengage(void* p, float dt) {
    Actor* a = (Actor*)p;

    a->field638 = 1;
    a->flags &= ~1u;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(&a->type, 3);
}

// 0x80137F30: follow-state gun update. Forwards the weapon's two linked names to
// the motion system, then sets up the gun's two state slots from the per-slot and
// per-type motion tables. The second slot keeps the type's first variant (1) when
// it equals the motion name, and uses the follow-up variant (0x11) otherwise.
void PlayerGunFollowSetup(Actor* a) {
    int name1 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
    int name2 = NameQuery(a->handle, 0x41E, 0xDFF3097A);
    PlayerRef* ref;
    int* types;
    int slot;
    int kind;
    int motion;

    fn_80048BA8(a, a->buf, 1);
    Fn800F7F5C(name1, name2, 0xBC78F550, fn_800498D0(a), a->buf, a->speed);
    slot = PlayerInHost(a);
    kind = a->field73C;
    ref = a->refs[slot];
    types = (int*)(lbl_803A07D8 + 0x324) + kind;
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, *types, 0, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 0x11;
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, *types, 0, 0);
    motion = fn_800A1BC8(ref->field14, ref->field1C,
                         ((int*)(lbl_803A07D8 + 0xE4))[kind], 0, 0);
    if (a->fieldF0 == motion) {
        a->fieldFC = 1;
    } else {
        a->fieldFC = 0x11;
    }
}

// 0x80138128: re-arms the gun for this state, runs the shared frame update
// (collision + dash) and switches the action state to 3.
void PlayerGunResume(void* p, float dt) {
    Actor* a = (Actor*)p;

    a->field638 = 1;
    a->flags &= ~1u;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(&a->type, 3);
}

// 0x80138194: ready-state gun setup. Forwards the weapon's two linked names to
// the motion system, then fills the gun's two state slots from the per-slot and
// per-type motion tables. The second slot keeps the type's first variant (1) when
// it equals the motion name, and uses the follow-up variant (0x11) otherwise.
void PlayerGunReadySetup(Actor* a) {
    int name1 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
    int name2 = NameQuery(a->handle, 0x41E, 0xDFF3097A);
    PlayerRef* ref;
    int* types;
    int slot;
    int kind;
    int motion;

    fn_80048BA8(a, a->buf, 1);
    Fn800F7F5C(name1, name2, 0x8FDD7599, fn_800498D0(a), a->buf, a->speed);
    slot = PlayerInHost(a);
    kind = a->field73C;
    ref = a->refs[slot];
    types = (int*)(lbl_803A07D8 + 0x2DC);
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, types[kind], 0, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 0x11;
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, types[kind], 0, 0);
    motion = fn_800A1BC8(ref->field14, ref->field1C,
                         ((int*)(lbl_803A07D8 + 0xE4))[kind], 0, 0);
    if (a->fieldF0 == motion) {
        a->fieldFC = 1;
    } else {
        a->fieldFC = 0x11;
    }
}

// 0x8013838C: plays the held gun's idle animation on the owner's handle.
void PlayerGunPlayIdleAnim(void* p) {
    ActorPlayAnim(0x02000000, 0xD39F69B4, fn_800498D0((Actor*)p));
}

// 0x801383C0: re-arms the gun for the ready state, runs the shared frame update
// (collision + dash) and switches the action state to 1.
void PlayerGunEnterState(void* p, float dt) {
    Actor* a = (Actor*)p;

    a->field638 = 1;
    a->flags &= ~1u;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
    PlayerSetValue4(&a->type, 1);
}

extern "C" int fn_800F7994(int name, u32 hash);   // 0x800F7994 effect lookup by name id
extern int lbl_804D2E74;                          // 0x804D2E74 active gun effect

// 0x8013842C: starts the gun's idle effect variant. The weapon's second linked
// name picks the effect, and the per-slot motion names fill the two state slots.
void PlayerGunIdleEffectStart(void* p) {
    Actor* a = (Actor*)p;
    PlayerRef* ref;
    int* types;
    int slot;
    int kind;
    int name;

    name = NameQuery(a->handle, 0x41E, 0xDFF3097A);
    slot = PlayerInHost(a);
    kind = a->field73C;
    ref = a->refs[slot];
    types = (int*)(lbl_803A07D8 + 0x294);
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, types[kind], 0, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 0x10;
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, types[kind], 0, 0);
    a->fieldFC = 0x10;
    lbl_804D2E74 = fn_800F7994(name, 0x9D1C245C);
    FxStart((void*)lbl_804D2E74, 0, 0);
}

// 0x8013856C: gun fire-state update. Re-arms the timer, clears the leaving flag
// and the first flag bit, runs the shared frame update, switches the action state
// to 8, and then either falls to state 3 or rearms the gun timer.
void PlayerGunFireUpdate(void* p, float f) {
    Actor* a = (Actor*)p;

    a->field638 = 1;
    a->flags &= ~1u;
    fn_800769A0(a, f);
    PlayerSelectCollisionUpdate(a, f);
    PlayerSetValue4(&a->type, 8);
    DashUpdate(a);
    if (a->field318 == 0) {
        PlayerSetValue4(&a->type, 3);
    } else {
        PlayerResetGunTimer(a);
    }
}

// 0x801385FC: starts the gun's action for the first slot variant. Both motion
// slots come from the per-slot motion table, the state callback is the
// name-intro handler and both slot modes are 1.
void PlayerGunSetupActionSlot1(Actor* a) {
    PlayerRef* ref;
    int slot;
    int kind;
    u32* types;

    slot = PlayerInHost(a);
    kind = a->field73C;
    ref = a->refs[slot];
    types = (u32*)(lbl_803A07D8 + 0x24C);
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, types[kind], 0, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 1;
    ref = a->refs[slot];
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, types[kind], 0, 0);
    a->fieldFC = 1;
}

// 0x801386B0: dash-state gun update. Re-arms the timer, clears the leaving flag
// and the first flag bit, then runs the shared frame update (pre-update,
// collision and dash).
void PlayerGunDashStateUpdate(Actor* a, float dt) {
    a->field638 = 1;
    a->flags &= ~1u;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    DashUpdate(a);
}

// 0x80138710: state-entry variant for the name-intro state that reuses the action
// hash stored in field7DC. The stored hash picks the first slot's mode; both state
// slots come from the motion table at lbl_803A07D8 + 0x174 for the current weapon
// slot, and the name-intro handler becomes the state callback.
void PlayerGunStoredHashIntro(void* p) {
    Actor* a = (Actor*)p;
    PlayerRef* ref;
    u32* types;
    int slot;
    int kind;
    int mode;

    if (a->field7DC == 0x7183F329) {
        mode = 0;
    } else if (a->field7DC < 0x7183F329) {
        if (a->field7DC == (int)0xBEF8884D) {
            mode = 2;
        }
    } else if (a->field7DC == 0x7183F5A1) {
        mode = 1;
    }
    slot = PlayerInHost(a);
    kind = a->field73C;
    ref = a->refs[slot];
    types = (u32*)(lbl_803A07D8 + 0x174);
    a->field98 = fn_800A1BC8(ref->field14, ref->field18, types[kind], mode, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 1;
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, types[kind], 0, 0);
    a->fieldFC = 1;
}

// 0x80138818: follow-state gun update. It clears the leaving flag, runs the shared
// pre-update and collision step, refreshes the aim speed when the horizontal speed
// changed, then dispatches on the weapon's linked names: the resulting action hash
// selects a held-slot mode and may commit a new state. When no branch committed a
// state it falls through to the common follow update.
void PlayerGunFollowNameCheck(void* p, float f) {
    Actor* a = (Actor*)p;
    float speed;
    int r1;
    int r2;
    int h;
    int handled;

    a->field638 = 1;
    a->flags &= ~1u;
    fn_800769A0(a, f);
    PlayerSelectCollisionUpdate(a, f);
    a->field7D0 = 1;
    speed = fabsf(a->field394);
    if (speed < fabsf(a->field38C)) {
        speed = fabsf(a->field38C);
    }
    if (speed != lbl_804D7A58) {
        fn_8008E824(a->field7C, 0);
    }
    if (lbl_804D7A58 == a->field7D8) {
        PlayerSetHeldSlotMode(a, 0);
        return;
    }
    if (a->field678 & 0x80) {
        a->field678 &= ~0x80u;
        PlayerSetActionMode(a, 3);
        return;
    }
    PlayerSetValue4(&a->type, 3);
    DashUpdate(a);
    if (PlayerGunGaugeState(a) != 0) {
        return;
    }
    r1 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
    r2 = NameQuery(a->handle, 0x41E, 0xDFF3097A);
    handled = 0;
    if (a->field318 < 1) {
        if (a->type == 8) {
            PlayerSetHeldSlotMode(a, 3);
        }
    } else if (a->type == 1) {
        PlayerSetHeldSlotMode(a, 2);
        handled = 1;
    } else {
        PlayerResetGunTimer(a);
        h = Fn800F7DEC(r1, r2, 0xF829DB93, fn_800498D0(a), 0);
        switch (h) {
        case 0x11D2C025:
            if (a->type == 8) {
                PlayerSetHeldSlotMode(a, 3);
            }
            break;
        case 0x3DA260F7:
            if (a->type == 8) {
                PlayerSetHeldSlotMode(a, 3);
            }
            break;
        case 0x04A3E183:
            if (a->type != 8) {
                PlayerSetHeldSlotMode(a, 8);
                handled = 1;
            }
            break;
        case 0xC0BECF7E:
            if (Fn800F7DEC(r1, r2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                PlayerSetHeldSlotMode(a, 0xB);
                handled = 1;
            }
            break;
        }
    }
    if (!handled) {
        PlayerGunActionFallback(a, f);
    }
}

// 0x80138B6C: starts the gun's action for the second slot variant. Both motion
// slots come from the per-type second motion table, the state callback is the
// name-intro handler and both slot modes are 1.
void PlayerGunSetupActionSlot2(void* p) {
    Actor* a = (Actor*)p;
    int slot = PlayerInHost(a);
    int kind = a->field73C;
    u32* types = (u32*)(lbl_803A07D8 + 0xE4);

    a->field98 = fn_800A1BC8(a->refs[slot]->field14, a->refs[slot]->field18,
                             types[kind], 0, 0);
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 1;
    a->fieldF0 = fn_800A1BC8(a->refs[slot]->field14, a->refs[slot]->field1C,
                             types[kind], 0, 0);
    a->fieldFC = 1;
}

// 0x80138C20: plays the held gun's second idle animation on the owner's handle.
void PlayerGunPlayHeldIdleAnim(void* p) {
    ActorPlayAnim(0x02000000, 0x8B3A27A3, fn_800498D0((Actor*)p));
}

// 0x80138C54: drops the held gun's follow flag, then runs the shared actor
// update and selects the gun's own collision slot.
void PlayerGunLeaveTarget(void* p, float f) {
    Actor* a = (Actor*)p;

    a->field638 = 1;
    a->flags &= ~1u;
    fn_800769A0(a, f);
    PlayerSelectCollisionUpdate(a, f);
    DashUpdate(a);
    PlayerSetValue4(&a->type, 3);
}

extern int lbl_804D2E70;                                     // 0x804D2E70 active gun effect

// 0x80138CC0: starts a weapon's effect and resolves its two linked name assets
// into the actor's 0x98 and 0xF0 slots.
void PlayerGunStartWeaponFx(void* p) {
    Actor* a = (Actor*)p;
    int handle = a->handle;
    u32 name = NameQuery(handle, 0x41E, 0xDFF3097A);
    int slot = PlayerInHost(a);
    PlayerRef* ref = a->refs[slot];
    u32* entry = (u32*)(lbl_803A07D8 + slot * 4);

    a->field98 = fn_800A1BC8(ref->field14, ref->field18, entry[0x27], 0, 0);
    a->fieldA4 = 0x10;
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldF0 = fn_800A1BC8(ref->field14, ref->field1C, entry[0x27], 0, 0);
    a->fieldFC = 0x10;
    lbl_804D2E70 = fn_800F7994(name, 0xAFEF562B);
    FxStart((void*)lbl_804D2E70, 0, 0);
}

// 0x80138E00: gun state update. Refreshes the weapon state, requests the two
// linked names and picks the held-slot mode from the action hash.
void PlayerGunActionUpdate(void* p, float f) {
    Actor* a = (Actor*)p;
    int name0;
    int name1;
    int handled = 0;
    int r;

    a->field638 = 1;
    a->flags &= ~1u;
    fn_800769A0(a, f);
    PlayerSelectCollisionUpdate(a, f);
    if (a->field678 & 0x80) {
        a->field678 &= ~0x80u;
        PlayerSetActionMode(a, 1);
    }
    PlayerSetValue4(&a->type, 1);
    a->field7D0 = 0;
    DashUpdate(a);
    if (PlayerGunGaugeState(a) != 0) {
        return;
    }
    name0 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
    name1 = NameQuery(a->handle, 0x41E, 0xDFF3097A);

    if (a->field318 < 1) {
        if (a->type == 8) {
            PlayerSetHeldSlotMode(a, 3);
        }
    } else if (a->type == 1) {
        PlayerSetHeldSlotMode(a, 2);
        handled = 1;
    } else {
        PlayerResetGunTimer(a);
        r = Fn800F7DEC(name0, name1, 0xF829DB93, fn_800498D0((Actor*)a), 0);
        if (r == 0x11D2C025) {
            if (a->type == 8) {
                PlayerSetHeldSlotMode(a, 3);
            }
        } else if (r < 0x11D2C025) {
            if (r == 0x04A3E183) {
                if (a->type != 8) {
                    PlayerSetHeldSlotMode(a, 8);
                    handled = 1;
                }
            } else if (r < 0x04A3E183) {
                if (r == 0xC0BECF7E) {
                    if (Fn800F7DEC(name0, name1, 0x968CBE3D,
                                    fn_800498D0((Actor*)a), 0) == 0x04A3E183) {
                        PlayerSetHeldSlotMode(a, 0xB);
                        handled = 1;
                    }
                }
            }
        } else if (r == 0x3DA260F7 && a->type == 8) {
            PlayerSetHeldSlotMode(a, 3);
        }
    }
    if (!handled && (a->field320 & 1)) {
        if (a->field7D0 == 0) {
            PlayerSetHeldSlotMode(a, 2);
        } else {
            PlayerSetHeldSlotMode(a, 9);
        }
    }
}

// 0x80139118: starts the held gun's slot animations for the current slot type.
// Both motion slots come from the per-slot name table and the state callback is
// the name-intro handler.
void PlayerGunStartSlotState(void* p) {
    Actor* a = (Actor*)p;
    int slot = PlayerInHost(a);
    int kind = a->field73C;
    u32* names = (u32*)(lbl_803A07D8 + kind * 4);

    a->field98 = fn_800A1BC8(a->refs[slot]->field14, a->refs[slot]->field18,
                             names[0x15], 0, 0);
    a->fieldA4 = 1;
    a->fieldF0 = fn_800A1BC8(a->refs[slot]->field14, a->refs[slot]->field1C,
                             names[0x15], 0, 0);
    a->fieldE4 = (int)PlayerGunActionMsg;
    a->fieldFC = 1;
    a->field7D0 = 0;
}

// 0x801391D4: state callback that only re-arms the gun timer of the actor it is
// handed (the argument is passed straight through).
void PlayerGunRearmTimerCb(void* p) {
    PlayerResetGunTimer(p);
}

// 0x801391F4: follow-state gun update. Commits the pending follow state (3) and
// runs the shared actor update; when the follow query says the gun has nothing to
// do, it re-queries the weapon's two linked names and picks the held-slot mode
// from the resulting action hash, falling through to the shared follow update when
// no branch committed a state. Finally it forces action mode 3 when the actor's
// motion fields are no longer at rest.
void PlayerGunFollowCommitUpdate(void* p, float f) {
    Actor* a = (Actor*)p;
    int name0;
    int name1;
    int h;
    int handled = 0;

    a->field638 = 1;
    a->flags &= ~1u;
    a->field7D0 = 1;
    PlayerSetValue4(&a->type, 3);
    DashUpdate(a);
    if (PlayerGunGaugeState(a) == 0) {
        name0 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
        name1 = NameQuery(a->handle, 0x41E, 0xDFF3097A);
        if (a->field318 < 1) {
            if (a->type == 8) {
                PlayerSetHeldSlotMode(a, 3);
            }
        } else if (a->type == 1) {
            PlayerSetHeldSlotMode(a, 2);
            handled = 1;
        } else {
            PlayerResetGunTimer(a);
            h = Fn800F7DEC(name0, name1, 0xF829DB93, fn_800498D0(a), 0);
            if (h == 0x11D2C025) {
                if (a->type == 8) {
                    PlayerSetHeldSlotMode(a, 3);
                }
            } else if (h < 0x11D2C025) {
                if (h == 0x04A3E183) {
                    if (a->type != 8) {
                        PlayerSetHeldSlotMode(a, 8);
                        handled = 1;
                    }
                } else if (h < 0x04A3E183) {
                    if (h == 0xC0BECF7E) {
                        if (Fn800F7DEC(name0, name1, 0x968CBE3D,
                                        fn_800498D0(a), 0) == 0x04A3E183) {
                            PlayerSetHeldSlotMode(a, 0xB);
                            handled = 1;
                        }
                    }
                }
            } else if (h == 0x3DA260F7) {
                if (a->type == 8) {
                    PlayerSetHeldSlotMode(a, 3);
                }
            }
        }
        if (!handled) {
            PlayerGunActionFallback(a, f);
        }
    }
    if (lbl_804D7A58 != a->field394 || lbl_804D7A58 != a->field38C ||
        lbl_804D7A58 != a->field390 || lbl_804D7A58 != a->field398 ||
        a->field328 != 0) {
        PlayerSetActionMode(a, 3);
    }
}

// 0x80139504: sets up the gun action's two motion slots. The variant flag is
// picked at random (a chance out of rand()/scale) and both slot requests retry
// once with the flag cleared when the motion is not found.
void PlayerGunSetupActionRandSlot(void* p) {
    Actor* a = (Actor*)p;
    u32 r = rand();
    u32* types;
    int slot;

    if ((float)r / lbl_804D7A5C < lbl_804D7A60) {
        a->field7D4 = 1;
    } else {
        a->field7D4 = 0;
    }
    slot = PlayerInHost(a);
    types = (u32*)lbl_803A07D8;

    a->field98 = fn_800A1BC8(a->refs[slot]->field14, a->refs[slot]->field18,
                             types[3], a->field7D4, 0);
    if (a->field98 == -1) {
        a->field7D4 = 0;
        a->field98 = fn_800A1BC8(a->refs[slot]->field14, a->refs[slot]->field18,
                                 types[3], a->field7D4, 0);
    }
    a->field8C = (int)PlayerGunActionMsg;
    a->fieldA4 = 1;
    a->fieldF0 = fn_800A1BC8(a->refs[slot]->field14, a->refs[slot]->field1C,
                             types[3], a->field7D4, 0);
    if (a->fieldF0 == -1) {
        a->field7D4 = 0;
        a->fieldF0 = fn_800A1BC8(a->refs[slot]->field14, a->refs[slot]->field1C,
                                 types[3], a->field7D4, 0);
    }
    a->fieldE4 = (int)PlayerGunActionMsg;
    a->fieldFC = 1;
}

// 0x80139670: gun action handler for the actions without a dedicated case in
// PlayerGunAction. It fetches the weapon's two linked names (messages 0x41E)
// once, then matches them against the action hashes and switches the held-slot
// mode; the timer branch also charges/ticks the weapon's gauge.
int PlayerGunActionFallback(void* p, float f) {
    Actor* a = (Actor*)p;
    int name1 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
    int name2 = NameQuery(a->handle, 0x41E, 0xDFF3097A);
    float t;

    if (a->field588 == 5 || a->field588 == 4) {
        if (a->field320 & 1) {
            PlayerResetGunTimer(a);
            if (a->field7D0 == 0) {
                PlayerSetHeldSlotMode(a, 2);
            } else {
                PlayerSetHeldSlotMode(a, 9);
            }
            return 1;
        }
        if (a->field324 & 1) {
            PlayerResetGunTimer(a);
            if (Fn800F7DEC(name1, name2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                PlayerSetHeldSlotMode(a, 10);
                return 1;
            }
            if (Fn800F7DEC(name1, name2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                PlayerSetHeldSlotMode(a, 0xB);
                return 1;
            }
        }
    } else if (a->field320 == 0) {
        t = fn_800699B8(a);
        if (lbl_804D7A74 < t && t < lbl_804D7A58) {
            if (Fn800F7DEC(name1, name2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                PlayerSetHeldSlotMode(a, 10);
                fn_800699A8(&a->field2F4);
                return 1;
            }
            if (Fn800F7DEC(name1, name2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                PlayerSetHeldSlotMode(a, 0xB);
                fn_800699A8(&a->field2F4);
                return 1;
            }
        }
        fn_800699A8(&a->field2F4);
    } else {
        PlayerResetGunTimer(a);
        a->field5E8 += f;
        if (fn_800699B8(a) == lbl_804D7A70) {
            if (a->field7D0 == 0) {
                PlayerSetHeldSlotMode(a, 2);
            } else {
                PlayerSetHeldSlotMode(a, 9);
            }
            fn_800699A8(&a->field2F4);
            return 1;
        }
    }
    return 0;
}

// Payload of the gun's 0xC06 "can I act" query; the action hash and the
// weapon handle are filled in by the caller.
struct ActQuery {
    int result;                   // 0x00
    int action;                   // 0x04
    int pad08;                    // 0x08
    int handle;                   // 0x0C
};

// 0x801399A8: gun gauge/action state. Queries the weapon's two names, then
// checks the gauge timer and the held type; on a matching action pair it starts
// the empty-fire effect and moves the slot mode to 0x11.
int PlayerGunGaugeState(void* p) {
    Actor* a = (Actor*)p;
    int name0;
    int name1;
    int hash;

    name0 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
    name1 = NameQuery(a->handle, 0x41E, 0xDFF3097A);
    if (a->field314 < 1) {
        if (a->type == 5) {
            PlayerSetHeldSlotMode(a, 3);
        }
        return 0;
    }
    if (a->type == 1) {
        PlayerSetHeldSlotMode(a, 2);
        return 1;
    }
    PlayerResetGunTimer(a);
    hash = Fn800F7DEC(name0, name1, 0x20EE36, fn_800498D0(a), 0);
    if (hash == 0x11D2C025) {
        if (a->type == 5) {
            PlayerSetHeldSlotMode(a, 3);
        }
    } else if (hash < 0x11D2C025) {
        if (hash == 0x04A3E183) {
            if (a->type != 0x11) {
                fn_80048BA8(a, a->buf, 1);
                Fn800F7F5C(name0, name1, 0x96224FCC, fn_800498D0(a), a->buf, a->speed);
                a->field7DC = 0;
                PlayerSetHeldSlotMode(a, 0x11);
                return 1;
            }
        } else if (hash < 0x04A3E183 && hash == -0x3F413082) {
            hash = Fn800F7DEC(name0, name1, 0xE6B71BCC, fn_800498D0(a), 0);
            if (hash == 0x04A3E183) {
                PlayerSetHeldSlotMode(a, 0xA);
                return 1;
            }
        }
    } else if (hash == 0x3DA260F7 && a->type == 5) {
        PlayerSetHeldSlotMode(a, 3);
    }
    return 0;
}

// 0x80139C48: gun action message handler. Asks the weapon's 0xC06 query, then
// dispatches on the action hash: most actions forward the two queried names to
// the motion system, 0x1B83833F updates the held-type action state.
int PlayerGunActionMsg(int action, void* p) {
    Actor* a = (Actor*)p;
    ActQuery q;
    int name0;
    int name1;

    q.handle = a->handle;
    q.action = action;
    if (fn_800C5210(q.handle, 0xC06, &q) == 1) {
        return 0;
    }
    name0 = NameQuery(a->handle, 0x41E, 0x2DA8293C);
    name1 = NameQuery(a->handle, 0x41E, 0xDFF3097A);
    switch (action) {
    case 0x20EE36:
    case 0x96224FCC:
    case 0x9791AF51:
    case 0x968CBE3D:
    case 0xE6B71BCC:
    case 0x42E50A3D:
    case 0x0D19E338:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(name0, name1, action, fn_800498D0(a), a->buf, a->speed);
        break;
    case 0xF829DB93:
        fn_80048BA8(a, a->buf, 0);
        Fn800F7F5C(name0, name1, action, fn_800498D0(a), a->buf, a->speed);
        break;
    case 0x1B83833F:
        switch (a->type) {
        case 5:
            PlayerSetActionMode(a, 3);
            break;
        case 8:
            if (Fn800F7DEC(name0, name1, 0xF829DB93, fn_800498D0(a), 0) == 0x04A3E183) {
                PlayerSetActionMode(a, a->nextType);
            } else {
                PlayerSetHeldSlotMode(a, 3);
            }
            break;
        case 0xD:
            a->flags |= 0x20000;
            break;
        case 0x11:
            break;
        default:
            PlayerSetActionMode(a, a->nextType);
            break;
        }
        break;
    }
    return 0;
}

// Per-held-type entry in the dispatch table at lbl_803A07D8 (stride 0x48).
struct HeldTypeEntry {
    void (*stop)(void*);          // 0x00
    void (*begin)(void*);         // 0x04
    void (*msg)(void*);           // 0x08
    u8 pad0C[0x44 - 0x0C];        // 0x0C
    int value;                    // 0x44
};

// 0x80139FCC: returns the current held type's value from the 0x44 table field.
int PlayerGunHeldTypeValue(Actor* a) {
    return ((HeldTypeEntry*)lbl_803A07D8)[a->type].value;
}

// 0x80139FE8: runs the current held type's message handler, if it has one.
void PlayerGunHeldTypeCallback(void* p) {
    HeldTypeEntry* e = &((HeldTypeEntry*)lbl_803A07D8)[((Actor*)p)->type];

    if (e->msg) {
        e->msg(p);
    }
}

// 0x8013A02C: one-time init, then runs the held type's "start" callback (table +0x04).
void PlayerGunHeldTypeBegin(void* p) {
    HeldTypeEntry* e;

    if (lbl_804D4D04 == 0) {
        lbl_804D4D00 = 0;
        lbl_804D4D04 = 1;
    }
    e = &((HeldTypeEntry*)lbl_803A07D8)[((Actor*)p)->type];
    if (e->begin) {
        e->begin(p);
    }
}

// 0x8013A08C: runs the held type's "stop" callback (table +0x00), if it has one.
void PlayerGunHeldTypeEnd(void* p) {
    HeldTypeEntry* e = &((HeldTypeEntry*)lbl_803A07D8)[((Actor*)p)->type];

    if (e->stop) {
        e->stop(p);
    }
}

// 0x8013A0CC: resolves the sixteen bone-name assets used by the gun model into
// the table that starts at 0x8046D270.
// fn_8013A0CC
extern "C" void fn_8013A0CC(void) {
    lbl_8046D270[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_8046D270[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_8046D270[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_8046D270[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_8046D270[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_8046D270[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_8046D270[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_8046D270[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_8046D270[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_8046D270[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_8046D270[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_8046D270[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_8046D270[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_8046D270[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_8046D270[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_8046D270[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

// 0x8013A2E8: tears down the global render object used by the held gun.
// fn_8013A2E8
extern "C" void fn_8013A2E8(void) {
    fn_801D89A4(lbl_804D4D20);
}

// 0x8013A30C: sets up the render state for the held gun. A non-null camera
// object is converted into the stack render buffer; the view query result then
// decides whether the view is resumed (2), reset (0/8) or left untouched.
// fn_8013A30C
extern "C" void fn_8013A30C(int view, void* cam) {
    u8 buf[0x34];
    int mode;

    if (cam) {
        fn_8004320C(cam, buf);
        fn_80009324(buf, 0);
    } else {
        fn_800092CC();
    }
    GfxSetVtxDesc(2);
    lbl_804D4D20 = fn_801D89A4(0);
    GXSetAlphaUpdate(1);
    GXSetColorUpdate(1);
    GfxSetCullMode(0);
    mode = fn_80010448(view, 1);
    if (mode == 2) {
        fn_80010330(view, 1);
    } else if (mode == 0 || mode == 8) {
        fn_80010448(-3, 1);
    }
}

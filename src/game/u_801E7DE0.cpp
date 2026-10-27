// u_801E7DE0.cpp: player weapon, aim and collision-view states (0x801E7DE0-0x801EAACC).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned int u32;

// Reference-counted byte buffer and the String built on top of it. Both are
// part of the shared runtime (defined in u_8028A77C.cpp).
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
};

class String : public Buffer {
public:
    String();
    int data;   // 0x004 resolved character data
};

// Per-local-player view data block (pointed to by Actor::views).
struct PlayerView {
    u8  unk000[0x14];           // 0x000
    int field14;                // 0x014
    int field18;                // 0x018
    int field1C;                // 0x01C
    u8  unk020[0x104 - 0x20];   // 0x020
    u32 fov;                    // 0x104
};

struct Actor {
    u8  unk000[0x38];             // 0x000
    void* host38;                 // 0x038
    u8  unk03C[0x50 - 0x3C];      // 0x03C
    u32 flags50;                  // 0x050
    u8  unk054[0x58 - 0x54];      // 0x054
    int model;                    // 0x058
    u8  unk05C[0x7C - 0x5C];      // 0x05C
    void* field7C;                // 0x07C
    u8 pad80[0x8C - 0x80];
    int (*field8C)(int, Actor*);  // 0x08C
    u8  unk090[0x98 - 0x90];      // 0x090
    int field98;                  // 0x098
    u8  unk09C[0xA4 - 0x9C];      // 0x09C
    int fieldA4;                  // 0x0A4
    u8  unk0A8[0xE4 - 0xA8];      // 0x0A8
    int (*updateE4)(int, Actor*); // 0x0E4
    u8 padE8[0xF0 - 0xE8];
    int fieldF0;                  // 0x0F0
    u8  unk0F4[0xFC - 0xF4];      // 0x0F4
    int fieldFC;                  // 0x0FC
    u8  unk100[0x184 - 0x100];    // 0x100
    int cameraIndex;              // 0x184
    u8  unk188[0x1F0 - 0x188];    // 0x188
    float field1F0;               // 0x1F0
    u8  unk1F4[0x1F8 - 0x1F4];    // 0x1F4
    float field1F8;               // 0x1F8
    u8  unk1FC[0x248 - 0x1FC];    // 0x1FC
    int field248;                 // 0x248
    u8  unk24C[0x314 - 0x24C];    // 0x24C
    int field314;                 // 0x314
    int field318;                 // 0x318
    int field31C;                 // 0x31C
    u32 flags320;                 // 0x320
    u32 flags324;                 // 0x324
    u32 flags328;                 // 0x328
    u8  unk32C[0x38C - 0x32C];    // 0x32C
    float field38C;               // 0x38C
    float field390;               // 0x390
    float field394;               // 0x394
    float field398;               // 0x398
    u8  unk39C[0x588 - 0x39C];    // 0x39C
    int field588;                 // 0x588
    u8  unk58C[0x5E8 - 0x58C];    // 0x58C
    float field5E8;               // 0x5E8
    u8  unk5EC[0x638 - 0x5EC];    // 0x5EC
    int field638;                 // 0x638
    u8  unk63C[0x678 - 0x63C];    // 0x63C
    int field678;                 // 0x678
    u8  unk67C[0x73C - 0x67C];    // 0x67C
    int weaponSlot;               // 0x73C
    u8  unk740[0x7A4 - 0x740];    // 0x740
    int state7A4;                 // 0x7A4
    int field7A8;                 // 0x7A8
    u8  unk7AC[0x7B4 - 0x7AC];    // 0x7AC
    u32 flags7B4;                 // 0x7B4
    u8  unk7B8[0x7D0 - 0x7B8];    // 0x7B8
    int field7D0;                 // 0x7D0
    int field7D4;                 // 0x7D4
    float field7D8;               // 0x7D8
    u8  unk7DC[0x7E0 - 0x7DC];    // 0x7DC
    int field7E0;                 // 0x7E0
    u8  unk7E4[0x7E8 - 0x7E4];    // 0x7E4
    float field7E8;               // 0x7E8
    float field7EC;               // 0x7EC
    u8  unk7F0[0x14FC - 0x7F0];   // 0x7F0
    float field14FC;              // 0x14FC
    u8  unk1500[0x15B4 - 0x1500]; // 0x1500
    int field15B4;                // 0x15B4
    u8  unk15B8[0x1D04 - 0x15B8]; // 0x15B8
    u8  field1D04[0x26B0 - 0x1D04]; // 0x1D04
    int field26B0;                // 0x26B0
    u8  unk26B4[0x26C4 - 0x26B4]; // 0x26B4
    PlayerView* views[4];         // 0x26C4 (per local player; element 1 is at 0x26C8)
};

// Global table holding per-weapon ids; the arrays used here live at 0x0E4,
// 0x2DC and 0x3B4.
struct WeaponTable {
    u8  unk000[0x00C];      // 0x000
    int globalWeaponId;     // 0x00C weapon id used by the timer variant
    u8  unk010[0x09C - 0x10]; // 0x010
    int ids9C[0x12];        // 0x09C
    int idsE4[0x24];        // 0x0E4
    int ids174[0x48];       // 0x174
    int ids294[0x12];       // 0x294
    int ids2DC[0x36];       // 0x2DC
    int weapons[0xD5];      // 0x3B4
};

extern "C" {
extern WeaponTable lbl_803B3B60;
extern int lbl_8049C1E8[16];
extern int lbl_803B3B6C;
extern void fn_80050EB0(void*);
extern int fn_800A1BC8(int, int, int, int, int);
extern void SendMsg(void* to, int msg, void* data);
extern void fn_80048BA8(void* actor, void* stream, int flag);
extern int fn_800498D0(void* actor);
extern int fn_80049750(void* actor);
extern void fn_800769A0(void*, float);
extern int fn_80049940(void);
extern int fn_802A1D04(void);
extern int fn_800F60B8(int, int);
extern void fn_800F60E4(int, int, int);
extern void fn_80073E80(void*);
extern void fn_8008E824(void*, int);
extern int fn_801E9C30(void*);
extern int fn_801E9F08(int, Actor*);
extern float fn_800699B8(void*);
extern void fn_800699A8(void*);
extern float fn_800F95E4(int);
extern void fn_80165D9C(void*);
extern void fn_8000F60C(void);
extern void fn_8000F7E4(int, int);
extern int fn_800C5210(void*, int, int*);
extern void fn_80165DA8(void*);
extern float lbl_804D8F0C;
extern float lbl_804D8F10;
extern float lbl_804D8F14;
extern float lbl_804D32F8;
extern float lbl_804D32FC;
extern float lbl_804D3300;
extern float lbl_804D3304;
extern int lbl_804D330C;
extern int fn_800F7994(int, u32);
extern void FxStart(int, int, int);
extern int rand(void);
extern float lbl_804D8F04;
extern float lbl_804D8F08;
}

int FxFind(const char* name);
int AssetFind(unsigned long, unsigned long);

int PlayerZoomLevelUpdate(Actor* actor, int slotA, int slotB);
int PlayerZoomStateUpdate(Actor* actor, float dt);

void PlayerResetGunTimer(void*);
extern int PlayerInHost(Actor*);
extern int ModelBoneName(int, int);
extern int ModelBoneNodeChain(int, int, int*, int);
extern void CameraSetFov(int, float);
extern void PlayerSelectCollisionUpdate(void*, float);
extern void PlayerSetValue4(void*, int);
extern void Fn800F7F5C(int, int, int, int, void*, float);
extern int Fn800F7DEC(int, int, int, int, void*);
extern float lbl_804D8F00;
extern void PlayerSetHeldSlotMode(void*, int);
extern void PlayerSetActionMode(void*, int);
extern int ActorPlayAnim(int, u32, int);

// Player first-person view setup: creates the two weapon handles, installs the
// per-frame update callback and syncs the camera field of view.
void PlayerFirstPersonSetup(Actor* actor)
{
    PlayerResetGunTimer(actor);
    actor->flags50 |= 0x40000;
    fn_80050EB0(actor);

    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;
    PlayerView* view = actor->views[localPlayer];

    actor->field98 = fn_800A1BC8(view->field14, view->field18, lbl_803B3B60.weapons[slot], 0, 0);
    actor->fieldA4 = 0x50;

    int bone;
    if (actor->field26B0 == -1) {
        bone = 8;
    } else {
        bone = ModelBoneNodeChain(actor->model, ModelBoneName(actor->model, actor->field26B0),
                                  lbl_8049C1E8, 0x10);
        if (bone == -1) {
            bone = 8;
        }
    }

    actor->fieldF0 = fn_800A1BC8(view->field14, view->field1C, lbl_803B3B60.weapons[slot], bone << 1, 0);
    actor->updateE4 = fn_801E9F08;
    actor->fieldFC = 0x50;
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)(u32)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
}

// Player per-frame update for the first-person view state.
void PlayerFirstPersonUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);

    if (fn_80049940()) {
        if (fn_802A1D04() == 0) {
            PlayerZoomLevelUpdate(actor, 0xD, 0x16);
        } else if (actor->flags328 & 1) {
            if (PlayerInHost(actor) != 0) {
                if (!(actor->flags7B4 & 0x40000) && !(actor->flags7B4 & 0x80000)) {
                    fn_80073E80(actor);
                }
            }
        }
    }
    PlayerSetValue4(&actor->state7A4, 3);
}

// Host query block for message 0x41E: the host fills the trailing String with
// the name handle it resolves for `hash`.
struct NameQuery {
    int    unk00;   // 0x00 filled in by the host
    u32    hash;    // 0x04
    int    unk08;   // 0x08
    String name;    // 0x0C
};

// Asks the actor's host for the name handle selected by `hash`.
static int PlayerWeaponQueryName(Actor* actor, u32 hash)
{
    NameQuery query;

    query.hash = hash;
    query.unk08 = 0;
    SendMsg(actor->host38, 0x41e, &query);
    return query.name.data;
}

// First-person "weapon held" view setup: resolves the model and animation
// names from the host, records the view FOV, and installs the per-frame
// update callback for this state.
void PlayerWeaponFirstPersonInit(Actor* actor)
{
    actor->field15B4 = 0;

    int modelName = PlayerWeaponQueryName(actor, 0x2da8293c);
    int animName = PlayerWeaponQueryName(actor, 0xdff3097a);

    fn_80048BA8(actor, actor->field1D04, 1);
    int anim = fn_800498D0(actor);
    Fn800F7F5C(modelName, animName, 0x8fdd7599, anim, actor->field1D04, actor->field14FC);

    PlayerView* view = actor->views[PlayerInHost(actor)];
    int slot = actor->weaponSlot;

    actor->field98 = fn_800A1BC8(view->field14, view->field18, lbl_803B3B60.ids2DC[slot], 0, 0);
    actor->field8C = fn_801E9F08;
    actor->fieldA4 = 0x11;
    actor->fieldF0 = fn_800A1BC8(view->field14, view->field1C, lbl_803B3B60.ids2DC[slot], 0, 0);
    if (actor->fieldF0 == lbl_803B3B60.idsE4[slot]) {
        actor->fieldFC = 1;
    } else {
        actor->fieldFC = 0x11;
    }
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)(u32)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
}

// Plays the weapon-ready animation entry of the actor's current loadout.
void PlayerWeaponRaiseAnim(Actor* actor)
{
    ActorPlayAnim(0x2000000, 0xd39f69b4, fn_800498D0(actor));
}

// Player per-frame update used while the first-person collision view is up:
// refreshes the local-player view, advances collision and, if the host asks
// for it, re-enters the weapon-select state.
void PlayerCollisionViewUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);

    if (fn_802A1D04() == 0) {
        PlayerZoomLevelUpdate(actor, 0xD, 0x16);
    } else if (actor->flags328 & 1) {
        if (PlayerInHost(actor) != 0) {
            if (!(actor->flags7B4 & 0x40000) && !(actor->flags7B4 & 0x80000)) {
                fn_80073E80(actor);
            }
        }
    }
    PlayerSetValue4(&actor->state7A4, 1);
}

// First-person weapon view setup for the state entered from the host: resolves
// the held weapon's name handle, swaps in the view model's two draw handles and
// starts the weapon-tied effect.
void PlayerWeaponViewSetup(Actor* actor)
{
    int name = PlayerWeaponQueryName(actor, 0xdff3097a);

    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;
    PlayerView* view = actor->views[localPlayer];

    actor->field98 = fn_800A1BC8(view->field14, view->field18, lbl_803B3B60.ids294[slot], 0, 0);
    actor->field8C = fn_801E9F08;
    actor->fieldA4 = 0x10;
    actor->fieldF0 = fn_800A1BC8(view->field14, view->field1C, lbl_803B3B60.ids294[slot], 0, 0);
    actor->fieldFC = 0x10;

    lbl_804D330C = fn_800F7994(name, 0x9d1c245c);
    FxStart(lbl_804D330C, 0, 0);
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)(u32)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
}

// Player per-frame update for the weapon-held first-person state: refreshes the
// local-player collision view, re-enters the weapon-select state when the host
// asks for it, and re-targets the weapon model handle between the two loaded
// variants before handing the state back to the base update.
void PlayerWeaponStateUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);
    PlayerSetValue4(&actor->state7A4, 5);

    if (fn_802A1D04() == 0) {
        PlayerZoomLevelUpdate(actor, 0xD, 0x16);
    } else if (actor->flags328 & 1) {
        if (PlayerInHost(actor) != 0) {
            if (!(actor->flags7B4 & 0x40000) && !(actor->flags7B4 & 0x80000)) {
                fn_80073E80(actor);
            }
        }
    }

    if (fn_802A1D04() == 0) {
        if (actor->field318 == 1 || actor->field318 == 3) {
            if (fn_800F60B8(actor->field248, 1) == 0x24a738) {
                fn_800F60E4(actor->field248, 1, 0x9b286e6f);
            } else {
                fn_800F60E4(actor->field248, 1, 0x24a738);
            }
        }
    } else {
        PlayerZoomLevelUpdate(actor, 9, 0x15);
    }

    if (actor->field314 == 0) {
        PlayerSetValue4(&actor->state7A4, 3);
    } else {
        PlayerResetGunTimer(actor);
    }
}

// Installs the two draw handles of the held weapon view model (one for the
// world pass, one for the view-model pass) from the local player's view data.
void PlayerWeaponHandlesInit(Actor* actor)
{
    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;
    PlayerView* view = actor->views[localPlayer];

    actor->field98 = fn_800A1BC8(view->field14, view->field18, lbl_803B3B60.ids174[slot], 0, 0);
    actor->field8C = fn_801E9F08;
    actor->fieldA4 = 1;
    actor->fieldF0 = fn_800A1BC8(view->field14, view->field1C, lbl_803B3B60.ids174[slot], 0, 0);
    actor->fieldFC = 1;
}

// Weapon-held idle state: re-asks the host for both weapon name handles,
// refreshes collision, keeps the view FOV in sync with the larger of the two
// view angles, and runs the shared "leave to weapon select" checks.
void PlayerWeaponIdleStateUpdate(Actor* actor, float dt)
{
    PlayerWeaponQueryName(actor, 0x2da8293c);
    PlayerWeaponQueryName(actor, 0xdff3097a);

    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);
    actor->field7D0 = 1;

    float fovAbs = __fabsf(actor->field394);
    if (fovAbs < __fabsf(actor->field38C)) {
        fovAbs = __fabsf(actor->field38C);
    }
    if (lbl_804D8F00 != fovAbs) {
        fn_8008E824(actor->field7C, 0);
    }

    if (lbl_804D8F00 == actor->field7D8) {
        PlayerSetHeldSlotMode(actor, 0);
    } else if (!(actor->field678 & 0x80)) {
        PlayerSetValue4(&actor->state7A4, 3);

        if (fn_802A1D04() == 0) {
            if (actor->field318 == 1 || actor->field318 == 3) {
                if (fn_800F60B8(actor->field248, 1) == 0x24a738) {
                    fn_800F60E4(actor->field248, 1, 0x9b286e6f);
                } else {
                    fn_800F60E4(actor->field248, 1, 0x24a738);
                }
            }
        } else {
            PlayerZoomLevelUpdate(actor, 9, 0x15);
        }

        if (fn_801E9C30(actor) == 0) {
            int reentered;
            if (fn_802A1D04() == 0) {
                reentered = PlayerZoomLevelUpdate(actor, 0xD, 0x16);
            } else if (!(actor->flags328 & 1)) {
                reentered = 0;
            } else if (PlayerInHost(actor) == 0) {
                reentered = 0;
            } else if (actor->flags7B4 & 0x40000) {
                reentered = 0;
            } else if (actor->flags7B4 & 0x80000) {
                reentered = 0;
            } else {
                fn_80073E80(actor);
                reentered = 1;
            }
            if (reentered == 0) {
                PlayerZoomStateUpdate(actor, dt);
            }
        }
    } else {
        actor->field678 &= ~0x80;
        PlayerSetActionMode(actor, 3);
    }
}

// Weapon-held entry state: resolves the two host name handles, matches the
// weapon animation set, retries the draw handles until the host returns a
// valid one, and asks for the weapon-select state when the player is able.
void PlayerWeaponEnterState(Actor* actor)
{
    int nameA = PlayerWeaponQueryName(actor, 0x2da8293c);
    int nameB = PlayerWeaponQueryName(actor, 0xdff3097a);

    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;
    int result = Fn800F7DEC(nameA, nameB, 0x20ee36, fn_800498D0(actor), 0);

    int matched = 0;
    if (result == -0x3f413082 || result == 0x11d2c025) {
        matched = 1;
    }

    actor->field98 = -1;
    while (actor->field98 == -1) {
        PlayerView* view = actor->views[localPlayer];
        actor->field98 = fn_800A1BC8(view->field14, view->field18,
                                     lbl_803B3B60.idsE4[slot], matched, 0);
        actor->field8C = fn_801E9F08;
        actor->fieldA4 = 1;
        if (actor->field98 == -1 && matched != 0) {
            matched = 0;
        }
    }

    PlayerView* view = actor->views[localPlayer];
    actor->fieldF0 = fn_800A1BC8(view->field14, view->field1C,
                                 lbl_803B3B60.idsE4[slot], 0, 0);
    actor->fieldFC = 1;

    if (fn_802A1D04() == 0) {
        PlayerZoomLevelUpdate(actor, 0xD, 0x16);
    } else if (actor->flags328 & 1) {
        if (PlayerInHost(actor) != 0) {
            if (!(actor->flags7B4 & 0x40000) && !(actor->flags7B4 & 0x80000)) {
                fn_80073E80(actor);
            }
        }
    }
}

// Plays the weapon idle animation entry of the actor's current loadout.
void PlayerWeaponIdleAnim(Actor* actor)
{
    ActorPlayAnim(0x2000000, 0x8b3a27a3, fn_800498D0(actor));
}

// Player per-frame update for the collision-view state that hands the player
// back to the weapon-select state.
void PlayerCollisionViewStateUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);

    if (fn_802A1D04() == 0) {
        PlayerZoomLevelUpdate(actor, 0xD, 0x16);
    } else if (actor->flags328 & 1) {
        if (PlayerInHost(actor) != 0) {
            if (!(actor->flags7B4 & 0x40000) && !(actor->flags7B4 & 0x80000)) {
                fn_80073E80(actor);
            }
        }
    }
    PlayerSetValue4(&actor->state7A4, 3);
}

extern "C" {
extern int lbl_804D3308;
}

// First-person "weapon drawn" view setup: asks the host for the weapon name
// handle, installs both draw handles from the 0x9C weapon-id table, starts the
// weapon-tied effect and syncs the camera field of view.
void PlayerWeaponDrawnSetup(Actor* actor)
{
    int name = PlayerWeaponQueryName(actor, 0xdff3097a);

    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;
    PlayerView* view = actor->views[localPlayer];

    actor->field98 = fn_800A1BC8(view->field14, view->field18, lbl_803B3B60.ids9C[slot], 0, 0);
    actor->field8C = fn_801E9F08;
    actor->fieldA4 = 0x10;
    actor->fieldF0 = fn_800A1BC8(view->field14, view->field1C, lbl_803B3B60.ids9C[slot], 0, 0);
    actor->fieldFC = 0x10;

    lbl_804D3308 = fn_800F7994(name, 0xafef562b);
    FxStart(lbl_804D3308, 0, 0);
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)(u32)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
}

// Player per-frame update for the first-person collision-view state: clears
// the one-shot state flags, keeps collision in sync, updates the equipped
// weapon ids and hands the player back to the weapon-select state.
void PlayerCollisionViewEnterUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    fn_800769A0(actor, dt);
    PlayerSelectCollisionUpdate(actor, dt);

    if (actor->field678 & 0x80) {
        actor->field678 &= ~0x80;
        PlayerSetActionMode(actor, 1);
    }

    PlayerSetValue4(&actor->state7A4, 1);
    actor->field7D0 = 0;

    if (fn_802A1D04() == 0) {
        if (actor->field318 == 1 || actor->field318 == 3) {
            if (fn_800F60B8(actor->field248, 1) == 0x24a738) {
                fn_800F60E4(actor->field248, 1, 0x9b286e6f);
            } else {
                fn_800F60E4(actor->field248, 1, 0x24a738);
            }
        }
    } else {
        PlayerZoomLevelUpdate(actor, 9, 0x15);
    }

    if (fn_801E9C30(actor) != 0) {
        return;
    }

    int reentered;
    if (fn_802A1D04() == 0) {
        reentered = PlayerZoomLevelUpdate(actor, 0xD, 0x16);
    } else if (!(actor->flags328 & 1)) {
        reentered = 0;
    } else if (PlayerInHost(actor) == 0) {
        reentered = 0;
    } else if (actor->flags7B4 & 0x40000) {
        reentered = 0;
    } else if (actor->flags7B4 & 0x80000) {
        reentered = 0;
    } else {
        fn_80073E80(actor);
        reentered = 1;
    }

    if (reentered == 0 && (actor->flags320 & 1)) {
        if (actor->field7D0 == 0) {
            PlayerSetHeldSlotMode(actor, 2);
        } else {
            PlayerSetHeldSlotMode(actor, 9);
        }
    }
}

// Weapon-state setup for the timer variant: installs both view draw handles from
// the "weapons" id table, arms the per-frame update callback and clears the
// weapon-timer counter.
void PlayerWeaponTimerStateSetup(Actor* actor)
{
    int localPlayer = PlayerInHost(actor);
    int slot = actor->weaponSlot;
    PlayerView* view = actor->views[localPlayer];

    actor->field98 = fn_800A1BC8(view->field14, view->field18, lbl_803B3B60.weapons[slot], 0, 0);
    actor->fieldA4 = 1;
    actor->fieldF0 = fn_800A1BC8(view->field14, view->field1C, lbl_803B3B60.weapons[slot], 0, 0);
    actor->updateE4 = fn_801E9F08;
    actor->fieldFC = 1;
    actor->field7D0 = 0;
}

// Resets the held-weapon timer (state-entry helper shared with the base setup).
void PlayerWeaponResetTimer(Actor* actor)
{
    PlayerResetGunTimer(actor);
}

// First-person aim state update: marks the state active, clears the one-shot
// flag, runs the shared "return to weapon select" checks and leaves the aim
// view when the aim offsets are not neutral or the aim flag is set.
void PlayerAimStateUpdate(Actor* actor, float dt)
{
    actor->field638 = 1;
    actor->flags7B4 &= ~1u;
    actor->field7D0 = 1;
    PlayerSetValue4(&actor->state7A4, 3);

    if (fn_802A1D04() == 0) {
        PlayerZoomLevelUpdate(actor, 0xD, 0x16);
    } else if (actor->flags328 & 1) {
        if (PlayerInHost(actor) != 0) {
            if (!(actor->flags7B4 & 0x40000) && !(actor->flags7B4 & 0x80000)) {
                fn_80073E80(actor);
            }
        }
    }

    if (fn_801E9C30(actor) == 0) {
        int reentered;
        if (fn_802A1D04() == 0) {
            if (actor->field318 == 1 || actor->field318 == 3) {
                if (fn_800F60B8(actor->field248, 1) == 0x24a738) {
                    fn_800F60E4(actor->field248, 1, 0x9b286e6f);
                } else {
                    fn_800F60E4(actor->field248, 1, 0x24a738);
                }
                reentered = 1;
            } else {
                reentered = 0;
            }
        } else {
            reentered = PlayerZoomLevelUpdate(actor, 9, 0x15);
        }
        if (reentered == 0) {
            PlayerZoomStateUpdate(actor, dt);
        }
    }

    if (lbl_804D8F00 != actor->field394 ||
        lbl_804D8F00 != actor->field38C ||
        lbl_804D8F00 != actor->field390 ||
        lbl_804D8F00 != actor->field398 ||
        actor->flags328 != 0) {
        PlayerSetActionMode(actor, 3);
    }
}

// Weapon-timer state entry: resolves the two host weapon name handles, matches
// the current weapon animation set, picks the timer variant (coin flip when the
// weapon set does not force one) and installs both view draw handles, retrying
// once with the un-matched variant when the host returns no handle.
void PlayerWeaponTimerEnter(Actor* actor)
{
    int nameA = PlayerWeaponQueryName(actor, 0x2da8293c);
    int nameB = PlayerWeaponQueryName(actor, 0xdff3097a);
    int anim = fn_800498D0(actor);
    int result = Fn800F7DEC(nameA, nameB, 0x20ee36, anim, 0);

    if (result == -0x3f413082 || result == 0x11d2c025) {
        actor->field7D4 = 2;
    } else if ((float)rand() / lbl_804D8F04 < lbl_804D8F08) {
        actor->field7D4 = 1;
    } else {
        actor->field7D4 = 0;
    }

    PlayerView* view = actor->views[fn_80049750(actor)];

    actor->field98 = fn_800A1BC8(view->field14, view->field18, lbl_803B3B6C, actor->field7D4, 0);
    if (actor->field98 == -1) {
        actor->field7D4 = 0;
        actor->field98 = fn_800A1BC8(view->field14, view->field18, lbl_803B3B6C, actor->field7D4, 0);
    }
    actor->field8C = fn_801E9F08;
    actor->fieldA4 = 1;

    actor->fieldF0 = fn_800A1BC8(view->field14, view->field1C, lbl_803B3B6C, actor->field7D4, 0);
    if (actor->fieldF0 == -1) {
        actor->field7D4 = 0;
        actor->fieldF0 = fn_800A1BC8(view->field14, view->field1C, lbl_803B3B6C, actor->field7D4, 0);
    }
    actor->updateE4 = fn_801E9F08;
    actor->fieldFC = 1;
}

// Per-slot zoom state table (element size 4) at 0x2F4. Its entries overlap
// fields other states read directly, so it is reached from the object base.
static inline int* ZoomStateTable(Actor* actor)
{
    return (int*)((u8*)actor + 0x2F4);
}

// Zoom/aim state update for the first-person weapon states: asks the host for
// both weapon name handles and either re-enters the zoomed weapon view or
// advances the hold timer.
int PlayerZoomStateUpdate(Actor* actor, float dt)
{
    int nameA = PlayerWeaponQueryName(actor, 0x2da8293c);
    int nameB = PlayerWeaponQueryName(actor, 0xdff3097a);

    if (actor->field588 == 5 || actor->field588 == 4) {
        if (actor->flags320 & 1) {
            PlayerResetGunTimer(actor);
            if (actor->field7D0 == 0) {
                PlayerSetHeldSlotMode(actor, 2);
            } else {
                PlayerSetHeldSlotMode(actor, 9);
            }
            return 1;
        }
        if (actor->flags324 & 1) {
            PlayerResetGunTimer(actor);

            int anim = fn_800498D0(actor);
            int result = Fn800F7DEC(nameA, nameB, 0xe6b71bcc, anim, 0);
            if (result == 0x4a3e183) {
                actor->field15B4 = 0;
                PlayerSetHeldSlotMode(actor, 10);
                return 1;
            }

            anim = fn_800498D0(actor);
            result = Fn800F7DEC(nameA, nameB, 0x968cbe3d, anim, 0);
            if (result == 0x4a3e183) {
                PlayerSetHeldSlotMode(actor, 11);
                return 1;
            }
        }
    } else if (actor->flags320 == 0) {
        float level = fn_800699B8(actor);
        if (lbl_804D8F10 < level && level < lbl_804D8F00) {
            int anim = fn_800498D0(actor);
            int result = Fn800F7DEC(nameA, nameB, 0xe6b71bcc, anim, 0);
            if (result == 0x4a3e183) {
                PlayerSetHeldSlotMode(actor, 10);
                fn_800699A8(ZoomStateTable(actor));
                return 1;
            }

            anim = fn_800498D0(actor);
            result = Fn800F7DEC(nameA, nameB, 0x968cbe3d, anim, 0);
            if (result == 0x4a3e183) {
                PlayerSetHeldSlotMode(actor, 11);
                fn_800699A8(ZoomStateTable(actor));
                return 1;
            }
        }
        fn_800699A8(ZoomStateTable(actor));
    } else {
        PlayerResetGunTimer(actor);
        actor->field5E8 += dt;

        if (fn_800699B8(actor) == lbl_804D8F0C) {
            if (actor->field7D0 == 0) {
                PlayerSetHeldSlotMode(actor, 2);
            } else {
                PlayerSetHeldSlotMode(actor, 9);
            }
            fn_800699A8(ZoomStateTable(actor));
            return 1;
        }
    }
    return 0;
}

// Per-frame zoom-level update: selects the target zoom level from the two slot
// state entries, drives the view FOV towards it and notifies the effect system
// when the level changed.
int PlayerZoomLevelUpdate(Actor* actor, int slotA, int slotB)
{
    int oldLevel = actor->field7E0;

    if (fn_80049940() == 0) {
        if (ZoomStateTable(actor)[slotB] == 0) {
            if (ZoomStateTable(actor)[slotA] == 0) {
                actor->field7E8 = lbl_804D8F00;
                actor->field7E0 = 0;
            } else {
                if (actor->state7A4 == 1) {
                    PlayerSetHeldSlotMode(actor, 2);
                    return 1;
                }
                actor->field7E8 = (lbl_804D8F14 - fn_800F95E4(actor->cameraIndex)) / lbl_804D3300;
                actor->field7EC = lbl_804D8F14;
                actor->field7E0 = 1;
            }
        } else {
            if (actor->state7A4 == 1) {
                PlayerSetHeldSlotMode(actor, 2);
                return 1;
            }
            actor->field7E8 = (lbl_804D8F0C - fn_800F95E4(actor->cameraIndex)) / lbl_804D3304;
            actor->field7EC = lbl_804D8F0C;
            actor->field7E0 = 2;
        }
    } else {
        int slotState = ZoomStateTable(actor)[slotA];
        if (slotState == 1 || slotState == 3) {
            if (actor->state7A4 == 1) {
                PlayerSetHeldSlotMode(actor, 2);
                return 1;
            }
            int level = actor->field7E0;
            if (level == 1) {
                actor->field7E8 = (lbl_804D8F0C - fn_800F95E4(actor->cameraIndex)) / lbl_804D32FC;
                actor->field7EC = lbl_804D8F0C;
                actor->field7E0 = 2;
            } else if (level < 1) {
                if (level >= 0) {
                    actor->field7E8 = (lbl_804D8F14 - fn_800F95E4(actor->cameraIndex)) / lbl_804D32F8;
                    actor->field7EC = lbl_804D8F14;
                    actor->field7E0 = 1;
                }
            } else if (level < 3) {
                actor->field7E8 = lbl_804D8F00;
                actor->field7E0 = 0;
            }
        }
    }

    if (actor->field7E0 == 0) {
        actor->flags7B4 &= ~0x01000000;
        fn_80165D9C(actor);
    } else {
        PlayerResetGunTimer(actor);
        actor->flags7B4 |= 0x01000000;
        fn_80165DA8(actor);
    }

    if (lbl_804D8F00 == actor->field7E8) {
        int state = actor->field7E0;
        if (state == 1) {
            CameraSetFov(actor->cameraIndex, lbl_804D8F14);
        } else if (state < 1) {
            if (state >= 0) {
                CameraSetFov(actor->cameraIndex, (float)(u32)actor->views[1]->fov);
            }
        } else if (state < 3) {
            CameraSetFov(actor->cameraIndex, lbl_804D8F0C);
        }
    } else {
        float fov = fn_800F95E4(actor->cameraIndex);
        float step = actor->field7E8 * actor->field14FC;
        float next;

        if (fn_80049940() == 0) {
            float target = actor->field7EC;
            if (fov > target) {
                next = fov + step;
                if (next < target) {
                    actor->field7E8 = lbl_804D8F00;
                    next = target;
                }
            } else if (fov < target) {
                next = fov + step;
                if (next > target) {
                    actor->field7E8 = lbl_804D8F00;
                    next = target;
                }
            }
        } else {
            next = fov + step;
            if (next < actor->field7EC) {
                actor->field7E8 = lbl_804D8F00;
                next = actor->field7EC;
            }
        }
        CameraSetFov(actor->cameraIndex, next);
    }

    int level = actor->field7E0;
    if (oldLevel != level) {
        if (oldLevel < level) {
            FxStart(FxFind("zoom_out"), 0, 0);
        } else if (level == 1) {
            FxStart(FxFind("zoom_level1"), 0, 0);
        } else if (level == 2) {
            FxStart(FxFind("zoom_level2"), 0, 0);
        }
    }
    return 0;
}

// Checks the held-slot weapon action against the nameless action pair and
// switches the held-slot mode when the queried action resolves. Returns 1 when
// a mode change was made.
int PlayerHeldSlotActionCheck(Actor* actor)
{
    int nameA = PlayerWeaponQueryName(actor, 0x2da8293c);
    int nameB = PlayerWeaponQueryName(actor, 0xdff3097a);

    if (actor->field314 < 1) {
        if (actor->state7A4 == 5) {
            PlayerSetHeldSlotMode(actor, 3);
        }
    } else {
        if (actor->state7A4 == 1) {
            PlayerSetHeldSlotMode(actor, 2);
            return 1;
        }

        PlayerResetGunTimer(actor);
        int anim = fn_800498D0(actor);
        int result = Fn800F7DEC(nameA, nameB, 0x20ee36, anim, 0);

        if (result == 0x11d2c025) {
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                NameQuery query;
                query.hash = 0x7ffcfd4f;
                query.name.data = 0x91a0cef7;
                query.unk08 = 0;
                SendMsg(actor->host38, 0x41d, &query);
            }
            if (actor->state7A4 == 5) {
                PlayerSetHeldSlotMode(actor, 3);
            }
        } else if (result < 0x11d2c025) {
            if (result == 0x4a3e183) {
                PlayerSetHeldSlotMode(actor, 5);
                return 1;
            }
            if (result < 0x4a3e183 && result == -0x3f413082) {
                int anim2 = fn_800498D0(actor);
                int second = Fn800F7DEC(nameA, nameB, 0xe6b71bcc, anim2, 0);
                actor->field15B4 = 0;
                if (second == 0x4a3e183) {
                    PlayerSetHeldSlotMode(actor, 10);
                    return 1;
                }
            }
        } else if (result == 0x3da260f7 && actor->state7A4 == 5) {
            PlayerSetHeldSlotMode(actor, 3);
        }
    }
    return 0;
}

// Per-frame weapon action message handler. Resolves the two action names once,
// then dispatches on the message id: either plays the matching animation or
// switches the held-slot mode (playing the "gun_empty" effect when the weapon
// has no ammo left).
int PlayerGunActionMessage(int msg, Actor* actor)
{
    int handle;
    if (fn_800C5210(actor->host38, 0xc06, &handle) == 1) {
        return 0;
    }

    int nameA = PlayerWeaponQueryName(actor, 0x2da8293c);
    int nameB = PlayerWeaponQueryName(actor, 0xdff3097a);

    switch (msg) {
    case 0x20ee36:
    case -0x697341c3:   // 0x968cbe3d
    case -0x1948e434:   // 0xe6b71bcc
    case -0x69ddb034:   // 0x96224fcc
    case 0xd19e338: {
        fn_80048BA8(actor, actor->field1D04, 1);
        int anim = fn_800498D0(actor);
        Fn800F7F5C(nameA, nameB, msg, anim, actor->field1D04, actor->field14FC);
        break;
    }
    case 0x1b83833f: {
        int state = actor->state7A4;
        if (state == 5) {
            int anim = fn_800498D0(actor);
            int result = Fn800F7DEC(nameA, nameB, 0x20ee36, anim, 0);
            if (result != 0x4a3e183) {
                if (result == -0x3f413082 || result == 0x11d2c025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                PlayerSetHeldSlotMode(actor, 3);
                return 0;
            }
            PlayerSetActionMode(actor, actor->field7A8);
            return 0;
        }
        if (state != 8) {
            if (state == 0xd) {
                actor->flags7B4 |= 0x20000;
                return 0;
            }
            PlayerSetActionMode(actor, actor->field7A8);
            return 0;
        }
        int anim = fn_800498D0(actor);
        int result = Fn800F7DEC(nameA, nameB, 0xf829db93, anim, 0);
        if (result != 0x4a3e183) {
            if (result == -0x3f413082 || result == 0x11d2c025) {
                FxStart(FxFind("gun_empty"), 0, 0);
            }
            PlayerSetHeldSlotMode(actor, 3);
            return 0;
        }
        PlayerSetActionMode(actor, actor->field7A8);
        return 0;
    }
    }
    return 0;
}

// Leaves the zoomed first-person view through the weapon-effect variant:
// clears the zoom level, restores the camera field of view, drops the zoom flag
// and releases the zoom assets before the shared zoom-exit helper runs.
void PlayerWeaponZoomExitFx(Actor* actor)
{
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)(u32)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
    AssetFind(0xf2e2beb8, 0xdefd6fa5);
    fn_8000F60C();
    fn_80165D9C(actor);
}

// Leaves the zoomed first-person view: clears the zoom level, restores the
// camera field of view, drops the zoom flag and runs the shared zoom-exit
// helper.
void PlayerWeaponZoomExit(Actor* actor)
{
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)(u32)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
    fn_80165D9C(actor);
}

// Leaves the zoomed first-person view through the weapon-effect asset: clears
// the zoom level, restores the camera field of view, drops the zoom flag and
// hands the effect asset to the asset release helper before the shared
// zoom-exit helper runs.
void PlayerWeaponZoomExitFxAsset(Actor* actor)
{
    actor->field7E0 = 0;
    CameraSetFov(actor->cameraIndex, (float)(u32)actor->views[1]->fov);
    actor->flags7B4 &= ~0x01000000;
    fn_8000F7E4(AssetFind(0xf2e2beb8, 0xdefd6fa5), 1);
    fn_80165D9C(actor);
}

// One entry of the 0x48-byte weapon-state table at lbl_803B3B60; the weapon id
// belonging to that state lives at +0x44.
struct WeaponSlotEntry {
    u8  unk000[0x44];   // 0x000
    int field44;        // 0x044
};

// Returns the weapon id assigned to the actor's current weapon state.
int PlayerWeaponStateId(Actor* actor)
{
    WeaponSlotEntry* table = (WeaponSlotEntry*)&lbl_803B3B60;
    return table[actor->state7A4].field44;
}

extern "C" {
extern signed char lbl_804D528C;
extern float lbl_804D5288;
extern float lbl_804D8F18;
extern float lbl_804D8F1C;
extern float lbl_804D8F20;
extern float lbl_804D8F24;
extern float lbl_804D8F28;
}

float fn_80017084(float);

// One entry of the 0x48-byte per-weapon-state callback table at lbl_803B3B60.
// +0x04 is the per-frame update, +0x08 the plain state notification.
struct WeaponStateEntry {
    u8  unk000[0x04];                                 // 0x000
    void (*update)(float dt, Actor* actor, int arg);  // 0x004
    void (*notify)(Actor* actor);                     // 0x008
    u8  unk00C[0x48 - 0x0C];                          // 0x00C
};

// Notifies the actor's current weapon state through the +0x08 callback.
void PlayerWeaponStateNotify(Actor* actor)
{
    WeaponStateEntry* table = (WeaponStateEntry*)&lbl_803B3B60;
    if (table[actor->state7A4].notify != 0) {
        table[actor->state7A4].notify(actor);
    }
}

// Shared per-frame effect for the first-person weapon states: advances the
// global view-bob phase, offsets the view position by the two sine terms and
// re-arms the gun timer, then runs the state's +0x04 frame callback.
void PlayerWeaponStateFrameUpdate(float dt, Actor* actor, int arg)
{
    if (lbl_804D528C == 0) {
        lbl_804D528C = 1;
        lbl_804D5288 = lbl_804D8F00;
    }

    if (actor->state7A4 == 3 || actor->state7A4 == 5) {
        if (actor->field7E0 == 1) {
            lbl_804D5288 = lbl_804D5288 + dt;
            actor->field1F0 += fn_80017084(lbl_804D8F18 * lbl_804D5288) * lbl_804D8F1C;
            actor->field1F8 += fn_80017084(lbl_804D8F18 * (lbl_804D8F20 + lbl_804D5288)) * lbl_804D8F24;
            PlayerResetGunTimer(actor);
        } else if (actor->field7E0 == 2) {
            lbl_804D5288 = lbl_804D5288 + dt;
            actor->field1F0 += fn_80017084(lbl_804D8F18 * lbl_804D5288) * lbl_804D8F24;
            actor->field1F8 += fn_80017084(lbl_804D8F18 * (lbl_804D8F20 + lbl_804D5288)) * lbl_804D8F28;
            PlayerResetGunTimer(actor);
        }
    }

    WeaponStateEntry* entry = (WeaponStateEntry*)&lbl_803B3B60 + actor->state7A4;
    if (entry->update != 0) {
        entry->update(dt, actor, arg);
    }
}

// Leading callback of a per-state entry. The table at 0x803B3B60 is walked
// with a 0x48-byte stride here, the same layout the weapon-state helpers use.
struct StateEnterEntry {
    void (*enter)(Actor* actor);  // 0x000
    u8 unk004[0x48 - 0x04];       // 0x004
};

// Runs the enter handler of the actor's current weapon state, if any.
void Ea620StateEnter(Actor* actor)
{
    StateEnterEntry* entry = (StateEnterEntry*)&lbl_803B3B60 + actor->state7A4;
    if (entry->enter != 0) {
        entry->enter(actor);
    }
}

// Loads the sixteen asset handles used by the first-person weapon view; they
// are all tagged with the same group hash and cached in the 0x8049C1E8 block.
// fn_801EA660
extern "C" void fn_801EA660(void)
{
    lbl_8049C1E8[0] = AssetFind(0xf1754bf2, 0x21c5e0);
    lbl_8049C1E8[1] = AssetFind(0xf1754bf2, 0x24805f);
    lbl_8049C1E8[2] = AssetFind(0xf1754bf2, 0x101844cc);
    lbl_8049C1E8[3] = AssetFind(0xf1754bf2, 0x48874f6e);
    lbl_8049C1E8[4] = AssetFind(0xf1754bf2, 0x4a53fc1);
    lbl_8049C1E8[5] = AssetFind(0xf1754bf2, 0x967daac6);
    lbl_8049C1E8[6] = AssetFind(0xf1754bf2, 0xd9404234);
    lbl_8049C1E8[7] = AssetFind(0xf1754bf2, 0x450b2bb);
    lbl_8049C1E8[8] = AssetFind(0xf1754bf2, 0x922bf04e);
    lbl_8049C1E8[9] = AssetFind(0xf1754bf2, 0x8c73dfa9);
    lbl_8049C1E8[10] = AssetFind(0xf1754bf2, 0x90aeeee4);
    lbl_8049C1E8[11] = AssetFind(0xf1754bf2, 0x4a2f9aa);
    lbl_8049C1E8[12] = AssetFind(0xf1754bf2, 0x4a48bc0);
    lbl_8049C1E8[13] = AssetFind(0xf1754bf2, 0x8671db2a);
    lbl_8049C1E8[14] = AssetFind(0xf1754bf2, 0x44e6ca4);
    lbl_8049C1E8[15] = AssetFind(0xf1754bf2, 0x44ffeba);
}

// Empty hook, kept so the screen-printf call sites have a stable target.
// (Descriptive name: DebugTextNoOp. Kept as fn_801EA87C because other units
// already call it by that placeholder.)
extern "C" void fn_801EA87C(void) {}

// Debug display-list buffer and its write cursor. The buffer is a 16 KB
// scratch block the debug text renderer appends packets to.
extern u8 lbl_803B4284[];
extern u8* lbl_804D3318;

// Builds one 9-byte text packet from the colour/rectangle arguments and then
// draws the formatted string. (Descriptive name: DebugTextDraw. Kept as
// fn_801EA880 because other units already call it by that placeholder.)
void TextSetStyle(int, int, int, int, int, int, int, int, int, int, int, int);
void DebugText(const char* fmt, ...);

extern "C" void fn_801EA880(const char* fmt, u8* color, int x, int y, int w,
                            int h, int flags)
{
    if ((int)lbl_804D3318 - (int)lbl_803B4284 + 9 < 0x4000) {
        u8* pos = lbl_804D3318;

        *pos++ = 9;
        *(unsigned short*)pos = (unsigned short)(x < 0 ? 0 : (x < 0x281 ? x : 0x280));
        pos += 2;
        *(unsigned short*)pos = 0x280;
        pos += 2;
        *(unsigned short*)pos = 0;
        pos += 2;
        *(unsigned short*)pos = (unsigned short)(y < 0 ? 0 : (y < 0x1e1 ? y : 0x1e0));
        pos += 2;
        lbl_804D3318 = pos;
        *pos = 0;
    }

    if (flags == 0) {
        TextSetStyle(color[0], color[1], color[2], color[3], w, 0, 0, 0,
                     0, 0, 0, 0);
    } else {
        TextSetStyle(color[0], color[1], color[2], color[3], w, 2,
                     h / 16 - w, 0, 0, 0, 0, 0);
    }
    DebugText(fmt);
}

// Debug-text state word shared with the screen-printf renderer. It lives in
// another unit's small-data section, so it keeps its placeholder name here.
extern int lbl_804D52A4;

// Reads the debug-text state word.
// (Descriptive name: GetDebugTextState. Was fn_801EA9DC.)
int GetDebugTextState(void)
{
    return lbl_804D52A4;
}

// Clears the debug-text state word.
// (Descriptive name: ResetDebugTextState. Was fn_801EA9E4.)
void ResetDebugTextState(void)
{
    lbl_804D52A4 = 0;
}

#include <stdarg.h>

extern "C" {
extern int vsprintf(char* buf, const char* fmt, va_list args);
extern unsigned long strlen(const char* s);
extern char* strcat(char* dst, const char* src);
extern char lbl_803B8284[];
}

void* ScratchAAlloc(int size);

// Formats a debug message into a scratch block and appends it to the debug
// text buffer when the combined text still fits in the 0x1000-byte block.
void ScreenPrintf(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    if (lbl_804D52A4 != 0) {
        char* buf = (char*)ScratchAAlloc(0x40000);
        int len = vsprintf(buf, fmt, args);

        if ((u32)(len + strlen(lbl_803B8284)) < 0x1000) {
            strcat(lbl_803B8284, buf);
        }
    }
}

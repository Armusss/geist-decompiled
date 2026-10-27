// u_80208238.cpp: player "Batsqueeks" host mode (per-frame update, setup, mode id tables) and a sound list
// (0x80208238-0x8020A650). Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

struct Actor;

// One possessed host; the player keeps a per-slot array of these.
struct HostSlot {
    u8 pad00[0x14];
    void* field14;      // 0x14
    void* field18;      // 0x18
    void* field1C;      // 0x1C
};

// Host animation object hung off the player; its live sub-state lives elsewhere.
struct HostAnim {
    u8 pad000[0x14];
    void* field14;              // 0x14
    void* field18;              // 0x18
    void* field1C;              // 0x1C
    u8 pad020[0x18];
    u32 field38;                // 0x38: type tag (top byte)
    u8 pad03C[0x234];
    u8 field270[1];             // 0x270: condition list
};

// Sub-state of the host animation object that owns the motion sources.
struct HostState {
    u8 pad00[0x14];
    void* field14;              // 0x14
    void* field18;              // 0x18
    void* field1C;              // 0x1C
};

struct Player {
    u8 pad00[0x38];
    void* field38;              // 0x38: message peer
    u8 pad3C[0x14];
    u32 field50;                // 0x50: player state flags
    u8 pad54[0x04];
    s32 field58;                // 0x58: host motion/model object
    u8 pad5C[0x1C];
    s32 field78;                // 0x78
    void* field7C;              // 0x7C: model object
    u8 pad80[0x0C];
    void* field08C;             // 0x8C: per-mode update callback
    u8 pad090[0x08];
    void* field098;             // 0x98
    u8 pad09C[0x08];
    s32 field0A4;               // 0xA4
    u8 pad0A8[0x1C];
    s32 field0C4;               // 0xC4: saved motion handle
    u8 pad0C8[0x1C];
    void* field0E4;             // 0xE4: second per-mode update callback
    u8 pad0E8[0x08];
    void* field0F0;             // 0xF0
    u8 pad0F4[0x08];
    s32 field0FC;               // 0xFC
    u8 pad100[0x1C];
    s32 field11C;               // 0x11C: saved motion handle
    u8 pad120[0x1FC];
    u32 flags31C;               // 0x31C
    u8 pad320[0x08];
    u32 flags328;               // 0x328
    u8 pad32C[0x60];
    float field38C;             // 0x38C: motion amplitude
    float field390;             // 0x390: motion amplitude
    float field394;             // 0x394: motion amplitude
    float field398;             // 0x398: motion amplitude
    u8 pad39C[0x29C];
    s32 field638;               // 0x638
    u8 pad63C[0x3C];
    u32 flags678;               // 0x678
    u8 pad67C[0xC0];
    s32 field73C;               // 0x73C
    u8 pad740[0x64];
    s32 field7A4;               // 0x7A4: action mode
    s32 field7A8;               // 0x7A8: saved action mode
    u8 pad7AC[0x08];
    u32 flags7B4;               // 0x7B4
    u8 pad7B8[0x18];
    s32 field7D0;               // 0x7D0
    s32 field7D4;               // 0x7D4: host mode motion variant
    float field7D8;             // 0x7D8: motion seek target
    u8 pad7DC[0x1ED4];
    s32 field26B0;              // 0x26B0: host bone index (-1: none)
    u8 pad26B4[0x10];
    HostSlot* slots26C4[1];     // 0x26C4: hosts, indexed by player slot
    u8 pad26C8[0x18];
    void* field26E0;            // 0x26E0: animation/state object (opaque)
    void* field26E4;            // 0x26E4: peer/owner object
    HostAnim* field26E8;        // 0x26E8: host animation object
    u8 pad26EC[0xB0];
    s32 field279C;              // 0x279C
};

// Shared data blob; the Batsqueeks mode id table starts at +0x684.
struct BatsqueekIdTable {
    u8 pad000[0x0C];
    s32 modeId0C;               // 0x0C: host mode motion id
    u8 pad010[0x44];
    s32 ids54[1];               // 0x54: slot motion ids
    u8 pad058[0x8C];
    s32 idsE4[1];               // 0xE4: host slot motion ids
    u8 pad0E8[0x2CC];
    s32 ids3B4[1];              // 0x3B4: weapon setup motion ids
    u8 pad3B8[0x164];
    s32 ids51C[1];              // 0x51C: carried-state motion ids
    u8 pad520[0x44];
    s32 ids564[1];              // 0x564: host mode motion ids
    u8 pad568[0x44];
    s32 slotIds[1];             // 0x5AC: per-slot weapon ids
    u8 pad5B0[0x44];
    s32 ids5F4[1];              // 0x5F4: host motion ids
    u8 pad5F8[0x8C];
    s32 ids[1];                 // 0x684: mode id table
};
extern BatsqueekIdTable lbl_803BA2B0;
extern float lbl_804D9360;      // sdata2 zero float shared by the player code
extern float lbl_804D9364;      // sdata2 scale for the random motion variant pick
extern float lbl_804D9368;      // sdata2 threshold for the random motion variant pick
extern "C" double fabs(double);
extern "C" int rand(void);

void* FxFind(const char* name);
extern "C" void* FxStart(void* fx, int a, int b);

int PlayerInHost(Actor* actor);
void PlayerUpdateMotionSeek(float seek, void* player);
void PlayerSelectCollisionUpdate(void* player, float seek);
void PlayerApplyFrameOffset(Player* player);
void PlayerSetActionMode(void* player, int mode);
void PlayerSetHeldSlotMode(void* player, int mode);
void PlayerSetValue4(void* field, int value);
void PlayerResetGunTimer(void* player);
int HasCondition(void* list, int id);

extern "C" void* fn_800A384C(void* host);
extern "C" int fn_80175EFC(int id);
extern "C" void fn_80155A84(void* anim, void* param, int flag);

extern "C" void fn_800769A0(void* player, float seek);
extern "C" int fn_802A1D04();
extern "C" void fn_8004C9C8(void* p);
extern "C" void* fn_800A1BC8(void* a, void* b, int id, int c, int d);
extern "C" int fn_800498D0(void* p);
extern "C" int fn_800C5210(void* peer, int msg, void* data);
int Fn800F7DEC(int first, int second, int query, int owner, void* param);
int PlayerBatsqueekModeCallback(int msg, void* p);
extern "C" void fn_80050EB0(void* p);
extern "C" void fn_8008E824(void* obj, int flag);
int ModelBoneName(int obj, int index);
int ModelBoneNodeChain(int obj, int name, int* chain, int count);
extern int lbl_8049E0F0[];
extern "C" int SendMsg(void* target, int msg, void* data);

void PlayerSetHeldSlotIndex(void* player, int index);

// Engine string type; the constructor and destructor live in the runtime unit.
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
};

// String's destructor is compiler-generated here (retail inlines it: it stores
// __vt__6String and calls __dt__6BufferFv directly).
class String : public Buffer {
public:
    String();
};

// Query block handed to the peer when the host mode asks which slot it holds.
struct NameQuery {
    s32 result;                 // 0x00
    u32 hash;                   // 0x04
    u32 param;                  // 0x08
    String name;                // 0x0C
};

// Query block handed to the peer when a host mode asks whether the player already
// holds a slot; the peer fills the two result words on success.
struct BatsqueekMsgQuery {
    s32 result;                 // 0x00
    s32 msg;                    // 0x04
    u32 param;                  // 0x08
    void* peer;                 // 0x0C
};

// Object hanging off the player at 0x26E4; +0x38 is the message peer.
struct PlayerPeer {
    u8 pad00[0x38];
    void* peer38;               // 0x38
};

// Per-frame update while the player is in the Batsqueeks host mode.
void PlayerBatsqueekUpdate(Player* player, float seek)
{
    player->field638 = 1;
    fn_800769A0(player, seek);
    PlayerUpdateMotionSeek(seek, player);

    if ((player->flags31C & 1) != 0 && PlayerInHost((Actor*)player) != 0)
    {
        if (fn_802A1D04() != 0)
            fn_8004C9C8(player);
        PlayerApplyFrameOffset(player);
        if (player->field7A4 != 0x17)
            PlayerSetActionMode(player, 0x17);
        else
            PlayerSetValue4(&player->field7A4, 0x17);
    }
    else if ((player->flags328 & 1) != 0)
    {
        FxStart(FxFind("Batsqueeks"), 0, 0);
    }
}

// Sets up the Batsqueeks host mode: one motion object per player slot, one per state.
void PlayerBatsqueekSetup(Player* player)
{
    int slot = PlayerInHost((Actor*)player);

    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field18,
                                   lbl_803BA2B0.ids[player->field73C], 0, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 1;

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field1C,
                                   lbl_803BA2B0.ids[player->field73C], 0, 0);
    player->field0FC = 1;
    player->field7D0 = 0;
    if (player->field7D0 == 0)
        PlayerSetValue4(&player->field7A4, 1);
    else
        PlayerSetValue4(&player->field7A4, 3);
}

// Per-frame update while the player is leaving the Batsqueeks host mode.
void PlayerBatsqueekExitUpdate(Player* player, float seek)
{
    player->field638 = 0;
    player->flags7B4 &= ~1u;
    fn_800769A0(player, seek);
    PlayerSelectCollisionUpdate(player, seek);

    if (player->field0C4 == -1)
    {
        if (player->field7D0 != 0)
            PlayerSetValue4(&player->field7A4, 3);
        else
            PlayerSetValue4(&player->field7A4, 1);
    }
    else
    {
        PlayerSetValue4(&player->field7A4, 0x16);
    }
}

// Clears the mode update callback and restores the saved motion handles.
void PlayerBatsqueekTeardown(Player* player)
{
    player->field08C = 0;

    if (player->field0C4 != -1)
        player->field098 = (void*)player->field0C4;

    if (player->field11C == -1)
        return;

    player->field0F0 = (void*)player->field11C;
}

// Resets the player to the Batsqueeks idle mode after the host mode ends.
void PlayerBatsqueekResetMode(Player* player)
{
    PlayerResetGunTimer(player);
    player->field638 = 1;
    player->flags7B4 &= ~1u;
    PlayerSetValue4(&player->field7A4, 0x15);

    if (player->field279C == -1 || fn_80175EFC(player->field279C) != 0)
    {
        PlayerSetValue4(&player->field7A4, 3);
        fn_80155A84((void*)&player->field26E0, player->field26E8, 1);
    }
}

// Sets up the Batsqueeks host mode: one motion object per player slot, one for the
// action and one for the carried state.
void PlayerBatsqueekHostSetup(Player* player)
{
    HostAnim* anim = player->field26E8;
    HostState* state;
    int used = 0;

    if ((anim->field38 >> 24) == 0x13)
    {
        state = (HostState*)fn_800A384C(anim);
        if (HasCondition(anim->field270, 0xb) != 0)
            used = 1;
    }

    PlayerInHost((Actor*)player);

    player->field098 = fn_800A1BC8(state->field14, state->field18,
                                   lbl_803BA2B0.ids5F4[player->field73C], used, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 0x19;

    player->field0F0 = fn_800A1BC8(state->field14, state->field1C,
                                   lbl_803BA2B0.ids5F4[player->field73C], used, 0);
    player->field0FC = 0x19;

    PlayerSetValue4(&player->field7A4, 0x15);
}

// Per-frame update while the player is in the Batsqueeks host mode: resets the gun
// timer, drives the motion seek and switches the player into the bite mode.
void PlayerBatsqueekFrameUpdate(Player* player, float seek)
{
    PlayerResetGunTimer(player);
    player->field638 = 1;
    player->flags7B4 &= ~1u;
    fn_800769A0(player, seek);
    PlayerUpdateMotionSeek(seek, player);
    PlayerSetValue4(&player->field7A4, 3);
    PlayerInHost((Actor*)player);

    if ((player->flags31C & 1) == 0 || PlayerInHost((Actor*)player) == 0)
    {
        if ((player->flags328 & 1) != 0)
            FxStart(FxFind("Batsqueeks"), 0, 0);
        return;
    }

    if (fn_802A1D04() != 0)
        fn_8004C9C8(player);

    PlayerApplyFrameOffset(player);
    if (player->field7A4 != 0x17)
        PlayerSetActionMode(player, 0x17);
    else
        PlayerSetValue4(&player->field7A4, 0x17);
}

// Hosts the Batsqueeks mode: builds the per-slot motion objects and asks the peer
// whether the player already holds a slot, which picks the follow-up action mode.
void PlayerBatsqueekGunSetup(Player* player)
{
    int slot = PlayerInHost((Actor*)player);
    NameQuery query;

    player->field78 = 0;
    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field18,
                                   lbl_803BA2B0.slotIds[player->field73C], 0, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 1;

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field1C,
                                   lbl_803BA2B0.slotIds[player->field73C], 0, 0);
    player->field0FC = 1;

    query.hash = 0x8783452;
    SendMsg(((PlayerPeer*)player->field26E4)->peer38, 0x41e, &query);

    if (query.result == 0)
    {
        PlayerSetHeldSlotIndex(player, 1);
        if ((s32)player->field098 == -1)
            PlayerSetActionMode(player, 3);
    }
    else
    {
        PlayerSetHeldSlotIndex(player, 0);
        if ((s32)player->field098 == -1)
            PlayerSetActionMode(player, 1);
    }
}

// Clears the Batsqueeks mode bit on the player (or arms the flag when it was not
// set) and marks the player as active in the host mode.
void PlayerBatsqueekResetGunState(Player* player)
{
    PlayerResetGunTimer(player);
    player->field638 = 1;
    if ((player->flags7B4 & 1) == 0)
    {
        player->field50 |= 0x20000;
        player->field78 = 1;
    }
    else
    {
        player->flags7B4 &= ~1u;
    }
}

// Sets up the two motion objects of the Batsqueeks host mode and asks the peer
// whether the player already holds a host slot.
void PlayerBatsqueekHostSlotSetup(Player* player)
{
    int used = 0;
    HostAnim* anim = player->field26E8;
    HostState* state;
    int slot;
    HostSlot* host;

    if ((anim->field38 >> 24) == 0x13)
    {
        state = (HostState*)fn_800A384C(anim);
        if (HasCondition(anim->field270, 0xb) != 0)
            used = 1;
    }

    slot = PlayerInHost((Actor*)player);

    player->field098 = fn_800A1BC8(state->field14, state->field18,
                                   lbl_803BA2B0.ids564[player->field73C], used, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 0x19;

    host = player->slots26C4[slot];
    player->field0F0 = fn_800A1BC8(host->field14, host->field1C,
                                   lbl_803BA2B0.ids564[player->field73C], 0, 0);
    player->field0FC = 0x19;

    NameQuery query;
    query.hash = 0x8783452;
    SendMsg((void*)anim->field38, 0x41e, &query);

    // The peer leaves the query's String block cleared while no slot is held.
    if (*(void**)&query.name == 0)
        PlayerSetHeldSlotIndex(player, 1);
    else
        PlayerSetHeldSlotIndex(player, 0);
}

// Restart the host animation object on the player's current state object.
void BatsqueekHostAnimStart(Player* player)
{
    fn_80155A84((void*)&player->field26E0, player->field26E8, 1);
}

// Put the player into the Batsqueeks gun mode: reset the gun timer, clear the
// "gun mode" bit and set the state flags and action mode.
void BatsqueekHostEnterGunMode(Player* player)
{
    PlayerResetGunTimer(player);
    player->field638 = 1;
    player->flags7B4 &= ~1u;
    player->field50 |= 0x20000;
    player->field78 = 1;
    PlayerSetValue4(&player->field7A4, 0x13);
}

// Hosts the carried state: builds the two carried-state motion objects and asks
// the peer whether the player already holds a host slot.
void PlayerBatsqueekCarryStateSetup(Player* player)
{
    int slot = PlayerInHost((Actor*)player);
    HostAnim* anim = player->field26E8;
    HostState* state;
    int index = player->field73C;
    int used = 0;

    if ((anim->field38 >> 24) == 0x13)
    {
        state = (HostState*)fn_800A384C(anim);
        if (HasCondition(anim->field270, 0xb) != 0)
            used = 1;
    }

    s32* id = &lbl_803BA2B0.ids51C[index];

    player->field098 = fn_800A1BC8(state->field14, state->field18, *id, used, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 0x59;

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field1C,
                                   *id, 0, 0);
    player->field0FC = 0x59;

    NameQuery query;
    query.hash = 0x8783452;
    SendMsg((void*)anim->field38, 0x41e, &query);

    if (*(void**)&query.name == 0)
        PlayerSetHeldSlotIndex(player, 1);
    else
        PlayerSetHeldSlotIndex(player, 0);
}

// Clears the host-mode flag (0x40000) in the player state flags.
void PlayerBatsqueekClearCarryBit(void* p)
{
    Player* player = (Player*)p;
    player->field50 &= ~0x40000u;
}

// Per-frame host-mode update that re-seeks the held weapon motion.
void PlayerBatsqueekSeekUpdate(Player* player, float seek)
{
    PlayerResetGunTimer(player);
    player->field638 = 1;
    player->flags7B4 &= ~1u;
    PlayerUpdateMotionSeek(seek, player);
}

// Sets up the host-mode motions for the held weapon: one for the host, one per bone chain.
void PlayerBatsqueekWeaponMotionSetup(Player* player)
{
    int slot;
    int index;
    int bone;

    PlayerResetGunTimer(player);
    player->field50 |= 0x40000;
    fn_80050EB0(player);

    slot = PlayerInHost((Actor*)player);
    index = player->field73C;

    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field18,
                                   lbl_803BA2B0.ids3B4[index], 0, 0);
    player->field0A4 = 0x50;

    if (player->field26B0 == -1)
    {
        bone = 8;
    }
    else
    {
        bone = ModelBoneNodeChain(player->field58,
                                  ModelBoneName(player->field58, player->field26B0),
                                  lbl_8049E0F0, 0x10);
        if (bone == -1)
            bone = 8;
    }

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field1C,
                                   lbl_803BA2B0.ids3B4[index], bone << 1, 0);
    player->field0E4 = (void*)PlayerBatsqueekModeCallback;
    player->field0FC = 0x50;
}

// Asks the player's message peer for a mode handle. The peer answers by writing
// the handle into the query block's trailing word, which is cleared beforehand.
static inline s32 BatsqueekQueryPeer(Player* player, u32 hash)
{
    NameQuery query;
    *(s32*)&query.name = 0;
    query.hash = hash;
    query.param = 0;
    SendMsg(player->field38, 0x41e, &query);
    return *(s32*)&query.name;
}

// Per-frame update used while the player is still picking a host mode: asks the
// peer for the two mode handles, then drives the motion and the action mode.
void PlayerBatsqueekQueryUpdate(Player* player, float seek)
{
    BatsqueekQueryPeer(player, 0x2da8293c);
    BatsqueekQueryPeer(player, 0xdff3097a);

    player->field638 = 1;
    player->flags7B4 &= ~1u;
    fn_800769A0(player, seek);
    PlayerUpdateMotionSeek(seek, player);
    player->field7D0 = 1;

    float speed = fabs(player->field394);
    if (speed < fabs(player->field38C))
        speed = fabs(player->field38C);

    if (lbl_804D9360 != speed)
        fn_8008E824(player->field7C, 0);

    if (lbl_804D9360 == player->field7D8)
    {
        PlayerSetHeldSlotMode(player, 0);
        return;
    }

    if ((player->flags678 & 0x80) != 0)
    {
        player->flags678 &= ~0x80u;
        PlayerSetActionMode(player, 3);
        return;
    }

    PlayerSetValue4(&player->field7A4, 3);

    if ((player->flags31C & 1) != 0 && PlayerInHost((Actor*)player) != 0)
    {
        if (fn_802A1D04() != 0)
            fn_8004C9C8(player);

        PlayerApplyFrameOffset(player);
        if (player->field7A4 != 0x17)
            PlayerSetActionMode(player, 0x17);
        else
            PlayerSetValue4(&player->field7A4, 0x17);
    }
    else if ((player->flags328 & 1) != 0)
    {
        FxStart(FxFind("Batsqueeks"), 0, 0);
    }
}

// Sets up the host-mode motion of the player's current slot: asks the peer for
// the two mode handles, validates them and builds the slot's motion objects.
void PlayerBatsqueekSlotHandleSetup(Player* player)
{
    s32 first = BatsqueekQueryPeer(player, 0x2da8293c);
    s32 second = BatsqueekQueryPeer(player, 0xdff3097a);

    int host = PlayerInHost((Actor*)player);
    int index = player->field73C;
    int owner = fn_800498D0(player);

    int found = Fn800F7DEC(first, second, 0x20ee36, owner, 0);
    int used = 0;
    if (found == -0x3F413082 || found == 0x11D2C025)
        used = 1;

    player->field098 = (void*)-1;
    while ((s32)player->field098 == -1)
    {
        HostSlot* slot = player->slots26C4[host];
        player->field098 = fn_800A1BC8(slot->field14, slot->field18,
                                       lbl_803BA2B0.idsE4[index], used, 0);
        player->field08C = (void*)PlayerBatsqueekModeCallback;
        player->field0A4 = 1;

        if ((s32)player->field098 == -1 && used != 0)
            used = 0;
    }

    HostSlot* slot = player->slots26C4[host];
    player->field0F0 = fn_800A1BC8(slot->field14, slot->field1C,
                                   lbl_803BA2B0.idsE4[index], 0, 0);
    player->field0FC = 1;
}

// Per-frame update while the Batsqueeks host mode winds down: resets the action
// mode, then either re-enters the bite mode or fires the effect when unhosted.
void PlayerBatsqueekExitMotionUpdate(Player* player, float seek)
{
    player->field638 = 1;
    player->flags7B4 &= ~1u;
    fn_800769A0(player, seek);
    PlayerUpdateMotionSeek(seek, player);

    if ((player->flags678 & 0x80) != 0)
    {
        player->flags678 &= ~0x80u;
        PlayerSetActionMode(player, 1);
    }

    PlayerSetValue4(&player->field7A4, 1);
    player->field7D0 = 0;

    if ((player->flags31C & 1) == 0 || PlayerInHost((Actor*)player) == 0)
    {
        if ((player->flags328 & 1) != 0)
            FxStart(FxFind("Batsqueeks"), 0, 0);
        return;
    }

    if (fn_802A1D04() != 0)
        fn_8004C9C8(player);

    PlayerApplyFrameOffset(player);
    if (player->field7A4 != 0x17)
        PlayerSetActionMode(player, 0x17);
    else
        PlayerSetValue4(&player->field7A4, 0x17);
}

// Sets up the two slot motion objects of the Batsqueeks host mode and stores the
// follow-up action mode.
void PlayerBatsqueekSlotMotionSetup(Player* player)
{
    int slot = PlayerInHost((Actor*)player);
    int index = player->field73C;
    HostSlot* host = player->slots26C4[slot];

    player->field098 = fn_800A1BC8(host->field14, host->field18,
                                   lbl_803BA2B0.ids54[index], 0, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 1;

    player->field0F0 = fn_800A1BC8(host->field14, host->field1C,
                                   lbl_803BA2B0.ids54[index], 0, 0);
    player->field0FC = 1;
    player->field7D0 = 0;
}

// Resets the possessed host's gun timer while the Batsqueeks host mode runs.
void PlayerBatsqueekResetGunTimer(void* player)
{
    PlayerResetGunTimer(player);
}

// Leaves the Batsqueeks host mode: clears the mode flags and falls back to the
// default action when no host motion is left active.
void PlayerBatsqueekExitHost(Player* player)
{
    float zero = 0.0f;

    player->field638 = 1;
    player->flags7B4 &= ~1;
    player->field7D0 = 1;
    PlayerSetValue4(&player->field7A4, 3);
    if ((player->flags31C & 1) != 0 && PlayerInHost((Actor*)player) != 0)
    {
        if (fn_802A1D04() != 0)
            fn_8004C9C8(player);
        PlayerApplyFrameOffset(player);
        if (player->field7A4 != 0x17)
            PlayerSetActionMode(player, 0x17);
        else
            PlayerSetValue4(&player->field7A4, 0x17);
    }
    else if ((player->flags328 & 1) != 0)
    {
        FxStart(FxFind("Batsqueeks"), 0, 0);
    }

    if (zero != player->field394 || zero != player->field38C ||
        zero != player->field390 || zero != player->field398 ||
        player->flags328 != 0)
        PlayerSetActionMode(player, 3);
}

// Sets up the current slot's two motion objects after asking the peer for the mode
// handles; the peer's answer (or a random draw) picks the motion variant at 0x7D4.
void PlayerBatsqueekSlotModeSetup(Player* player)
{
    s32 first = BatsqueekQueryPeer(player, 0x2da8293c);
    s32 second = BatsqueekQueryPeer(player, 0xdff3097a);

    int owner = fn_800498D0(player);
    int found = Fn800F7DEC(first, second, 0x20ee36, owner, 0);

    if (found == -0x3F413082 || found == 0x11D2C025)
        player->field7D4 = 2;
    else if ((float)rand() / lbl_804D9364 < lbl_804D9368)
        player->field7D4 = 1;
    else
        player->field7D4 = 0;

    int slot = PlayerInHost((Actor*)player);

    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field18,
                                   lbl_803BA2B0.modeId0C, player->field7D4, 0);
    if ((s32)player->field098 == -1)
    {
        player->field7D4 = 0;
        player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                       player->slots26C4[slot]->field18,
                                       lbl_803BA2B0.modeId0C, player->field7D4, 0);
    }
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 1;

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field1C,
                                   lbl_803BA2B0.modeId0C, player->field7D4, 0);
    if ((s32)player->field0F0 == -1)
    {
        player->field7D4 = 0;
        player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                       player->slots26C4[slot]->field1C,
                                       lbl_803BA2B0.modeId0C, player->field7D4, 0);
    }
    player->field0E4 = (void*)PlayerBatsqueekModeCallback;
    player->field0FC = 1;
}

// Per-mode callback: asks the player's peer for the two mode handles, then reacts to
// the two mode change messages (re-arm the bite flag / restart the swing effect).
int PlayerBatsqueekModeCallback(int msg, void* p)
{
    Player* player = (Player*)p;

    BatsqueekMsgQuery query;
    query.msg = msg;
    query.peer = player->field38;

    if (fn_800C5210(player->field38, 0xc06, &query) == 1)
        return 0;

    BatsqueekQueryPeer(player, 0x2da8293c);
    BatsqueekQueryPeer(player, 0xdff3097a);

    if (msg == 0x1b83833f)
    {
        if (player->field7A4 == 0xd)
            player->flags7B4 |= 0x20000;
        else
            PlayerSetActionMode(player, player->field7A8);
    }
    else if (msg == 0x28a80b)
    {
        FxStart(FxFind("Batwing"), 0, 0);
    }

    return 0;
}

// One entry of the per-action-mode table overlaid on lbl_803BA2B0 (0x48 bytes each).
struct BatsqueekModeEntry {
    u8 pad000[0x08];
    void (*callback)(Player* player);   // 0x08: per-mode callback
    u8 pad00C[0x38];
    s32 motionId;                       // 0x44: per-mode motion id
};

// Return the motion id registered for the player's current action mode.
s32 BatsqueekGetModeMotionId(Player* player) {
    BatsqueekModeEntry* table = (BatsqueekModeEntry*)&lbl_803BA2B0;
    return table[player->field7A4].motionId;
}

// Run the per-action-mode callback for the player's current mode, if one is set.
void BatsqueekRunModeCallback(Player* player) {
    BatsqueekModeEntry* table = (BatsqueekModeEntry*)&lbl_803BA2B0;
    if (table[player->field7A4].callback != 0) {
        table[player->field7A4].callback(player);
    }
}

// Function pointers stored at the start of each 0x48-byte per-action-mode table entry.
struct BatsqueekModeHooks {
    void (*hook00)(Player* player);     // 0x00
    void (*hook04)(Player* player);     // 0x04
    u8 pad008[0x40];
};

// Run the per-action-mode function pointer stored at table entry offset 0x04, if set.
void BatsqueekRunModeHook04(Player* player) {
    BatsqueekModeHooks* entry = &((BatsqueekModeHooks*)&lbl_803BA2B0)[player->field7A4];
    if (entry->hook04 != 0) {
        entry->hook04(player);
    }
}

// Run the per-action-mode function pointer stored at table entry offset 0x00, if set.
void BatsqueekRunModeHook00(Player* player) {
    BatsqueekModeHooks* entry = &((BatsqueekModeHooks*)&lbl_803BA2B0)[player->field7A4];
    if (entry->hook00 != 0) {
        entry->hook00(player);
    }
}

// Asset lookups for one of the Batsqueeks modes; fills the shared handle table.
unsigned long AssetFind(unsigned long id, unsigned long hash);

// Loads the Batsqueeks asset handles of this mode into the shared table.
// Still called as fn_80209A14 from u_8006C604, so the placeholder name stays.
extern "C" void fn_80209A14(void)
{
    lbl_8049E0F0[0] = AssetFind(0xf1754bf2, 0x21c5e0);
    lbl_8049E0F0[1] = AssetFind(0xf1754bf2, 0x24805f);
    lbl_8049E0F0[2] = AssetFind(0xf1754bf2, 0x101844cc);
    lbl_8049E0F0[3] = AssetFind(0xf1754bf2, 0x48874f6e);
    lbl_8049E0F0[4] = AssetFind(0xf1754bf2, 0x4a53fc1);
    lbl_8049E0F0[5] = AssetFind(0xf1754bf2, 0x967daac6);
    lbl_8049E0F0[6] = AssetFind(0xf1754bf2, 0xd9404234);
    lbl_8049E0F0[7] = AssetFind(0xf1754bf2, 0x450b2bb);
    lbl_8049E0F0[8] = AssetFind(0xf1754bf2, 0x922bf04e);
    lbl_8049E0F0[9] = AssetFind(0xf1754bf2, 0x8c73dfa9);
    lbl_8049E0F0[10] = AssetFind(0xf1754bf2, 0x90aeeee4);
    lbl_8049E0F0[11] = AssetFind(0xf1754bf2, 0x4a2f9aa);
    lbl_8049E0F0[12] = AssetFind(0xf1754bf2, 0x4a48bc0);
    lbl_8049E0F0[13] = AssetFind(0xf1754bf2, 0x8671db2a);
    lbl_8049E0F0[14] = AssetFind(0xf1754bf2, 0x44e6ca4);
    lbl_8049E0F0[15] = AssetFind(0xf1754bf2, 0x44ffeba);
}

struct Vec3 {
    float x;
    float y;
    float z;
};

// List of per-mode effect sources. The entry stride is 0x28 and entry 0 is
// skipped; each entry keeps the name hash it was registered under at +0x04 and
// the source object at +0x24.
struct SoundList {
    u8 pad00[0x06];
    unsigned short count;       // 0x06: entry count
    u8 pad08[0x04];
    u8* entries;                // 0x0C: entry array
    void* field10;              // 0x10: secondary source
    void* field14;              // 0x14: extra buffer
    u8 pad18[0x04];
    float timer;                // 0x1C: retrigger delay written on every trigger
};

extern Vec3 lbl_8049E130[];     // 0x14 recent trigger positions
extern int lbl_804D53A0;        // number of recent trigger positions
extern float lbl_804D9378;      // trigger range scale
extern float lbl_804D937C;      // retrigger delay
extern u8* lbl_803BAA90[];      // per-mode name strings
extern u8 __upper_map[];        // ASCII upper-case map used by the name hash

extern "C" int fn_801D60A0(int flag);
extern "C" int fn_801D4738(void* source, Vec3* pos, float seek);
extern "C" void fn_801D5010(void* source, void* param);
extern "C" void fn_801D5110(void* source, int flag);
extern "C" void fn_801D55CC(void* source);
extern "C" void fn_801D4C0C(void* source);
extern "C" int fn_801D549C(void* source);
void VFree(void* p);

// Triggers the sources of a list around the given position: mode 0 tests each
// entry for range, mode 1 scales the working vector, and any other mode filters
// the entries by the hash of that mode's name.
void BatsqueekTriggerModeSounds(SoundList* list, float seek, Vec3* pos, Vec3* vec, int mode)
{
    Vec3 local;

    if (list->entries == 0)
        return;

    local.x = vec->x;
    local.y = vec->y;
    local.z = vec->z;

    if (fn_801D60A0(1) != 0 && lbl_804D53A0 < 0x14)
    {
        int index = lbl_804D53A0;
        lbl_804D53A0 = index + 1;
        lbl_8049E130[index].x = pos->x;
        lbl_8049E130[index].y = pos->y;
        lbl_8049E130[index].z = pos->z;
    }

    if (mode == 0)
    {
        int hits = 0;
        int off = 0x28;

        for (int i = 1; i < list->count; i++)
        {
            void* source = *(void**)(list->entries + off + 0x24);

            if (source != 0 && fn_801D4738(source, pos, seek) != 0)
            {
                hits++;
                fn_801D5010(source, &local);
            }
            off += 0x28;
        }

        if (hits == 0)
        {
            local.x *= lbl_804D9378;
            local.y *= lbl_804D9378;
            local.z *= lbl_804D9378;

            off = 0x28;
            for (int i = 1; i < list->count; i++)
            {
                void* source = *(void**)(list->entries + off + 0x24);

                if (source != 0)
                    fn_801D5010(source, &local);
                off += 0x28;
            }
        }
    }
    else if (mode == 1)
    {
        local.x *= lbl_804D9378;
        local.y *= lbl_804D9378;
        local.z *= lbl_804D9378;

        int off = 0x28;
        for (int i = 1; i < list->count; i++)
        {
            void* source = *(void**)(list->entries + off + 0x24);

            if (source != 0)
                fn_801D5010(source, vec);
            off += 0x28;
        }
    }
    else
    {
        u8* name = lbl_803BAA90[mode];
        u32 hash = 0;

        if (name != 0)
        {
            signed char c;

            while ((c = (signed char)*name) != 0)
            {
                u32 v = (c == -1) ? 0xffffffffu : (u32)__upper_map[(u8)c];
                hash = hash * 31 + (v & 0xff);
                name++;
            }
            hash = hash % 0xffffffffu;
        }

        int off = 0x28;
        for (int i = 1; i < list->count; i++)
        {
            u8* entry = list->entries + off;
            void* source = *(void**)(entry + 0x24);

            if (source != 0 && hash == *(u32*)(entry + 4))
                fn_801D5010(source, vec);
            off += 0x28;
        }
    }

    list->timer = lbl_804D937C;
}

// Finds the sound-list entry registered under the given name hash and hands its
// source object to the trigger routine; if nothing matches, restarts the delay.
void SoundListTriggerEntry(SoundList* list, u32 hash, void* param)
{
    if (list->entries != 0)
    {
        int off = 0x28;

        for (int i = 1; i < list->count; i++)
        {
            u8* entry = list->entries + off;
            void* source = *(void**)(entry + 0x24);

            if (source != 0 && hash == *(u32*)(entry + 4))
            {
                fn_801D5010(source, param);
                return;
            }
            off += 0x28;
        }
        list->timer = lbl_804D937C;
    }
}

// Releases every live source held by the list.
void SoundListStopAll(SoundList* list)
{
    if (list->entries != 0)
    {
        int off;
        int i;

        for (i = 1, off = 0x28; i < list->count; i++)
        {
            void* source = *(void**)(list->entries + off + 0x24);

            if (source != 0)
                fn_801D5110(source, 1);
            off += 0x28;
        }
    }
}

// Releases every live source held by the list, then frees the entry array, the
// secondary source and the extra buffer, if they exist.
void SoundListTeardown(SoundList* list)
{
    if (list->entries != 0)
    {
        int off;
        int i;

        for (i = 1, off = 0x28; i < list->count; i++)
        {
            void* source = *(void**)(list->entries + off + 0x24);

            if (source != 0)
                fn_801D55CC(source);
            off += 0x28;
        }

        VFree(list->entries);
        fn_801D4C0C(list->field10);
    }

    if (list->field14 != 0)
        VFree(list->field14);
}

// Returns non-zero when none of the live sources of the list is still marked
// active; a single active source makes the whole list report "not finished".
int SoundListSourcesFinished(SoundList* list)
{
    if (list->entries != 0)
    {
        int off = 0x28;

        for (int i = 1; i < list->count; i++)
        {
            void* source = *(void**)(list->entries + off + 0x24);

            if (source != 0 && fn_801D549C(source) != 0)
                return 0;
            off += 0x28;
        }
    }

    return 1;
}

extern "C" void fn_801D513C(void* source, void* matrix);
extern "C" void fn_801D53D4(void* source);
extern "C" void fn_801D5448(void* source);
extern "C" void fn_801D550C(void* source);

// Quaternion of a bone transform.
struct Quat {
    float x;
    float y;
    float z;
    float w;
};

// Affine transform stored as a quaternion followed by a translation.
struct Mtx34 {
    Quat q;                     // 0x00: rotation
    Vec3 t;                     // 0x10: translation
};

void BoneSetMatrix(void* bone, Mtx34* matrix);
void Mtx34GetTranslation(Vec3* out, Mtx34* matrix);
void QuatRotateVec(Vec3* out, float* quat, Vec3* vec);
extern "C" void PSQUATMultiply(void* out, void* a, void* b);

// One 0x28-byte entry of a sound-source list: the source's rest transform and the
// index of the parent transform it hangs off.
struct SoundSourceEntry {
    s32 index;                  // 0x00: parent transform index in the output array
    u32 nameHash;               // 0x04
    Mtx34 matrix;               // 0x08: rest transform
    void* source;               // 0x24: live source, 0 for an unused entry
};

// Updates every live source of the list: when 'mode' is non-zero the sources are
// paused, otherwise they are resumed and re-registered with their source object.
void SoundListApplySourceMode(SoundList* list, int mode)
{
    if (list->entries == 0)
        return;

    int off;
    int i;

    for (i = 1, off = 0x28; i < list->count; i++)
    {
        void* source = *(void**)(list->entries + off + 0x24);

        if (source != 0)
        {
            if (mode != 0)
            {
                fn_801D53D4(source);
            }
            else
            {
                fn_801D550C(source);
                fn_801D5448(*(void**)(list->entries + off + 0x24));
            }
        }

        off += 0x28;
    }
}

// Builds the per-source transforms of the list into 'out': entry 0 supplies the
// base transform, every following entry either takes its live bone matrix or is
// rotated by the transform of the entry its index points at.
void SoundListBuildSourceMatrices(SoundList* list, int arg2, int arg3, Mtx34* out)
{
    Quat quat;
    Mtx34 matrix;

    if (list->entries == 0)
        return;

    int off = 0x28;
    Mtx34* cur = out + 1;

    out->t = ((SoundSourceEntry*)list->entries)->matrix.t;
    out->q = ((SoundSourceEntry*)list->entries)->matrix.q;

    for (int i = 1; i < list->count; i++)
    {
        SoundSourceEntry* entry = (SoundSourceEntry*)(list->entries + off);

        if (entry->source != 0)
        {
            fn_801D513C(entry->source, &matrix);
            BoneSetMatrix(cur, &matrix);
            PSQUATMultiply(&cur->q, &entry->matrix.q, &cur->q);

            cur->q.x = -cur->q.x;
            cur->q.y = -cur->q.y;
            cur->q.z = -cur->q.z;

            Mtx34GetTranslation(&cur->t, &matrix);
        }
        else
        {
            PSQUATMultiply(&entry->matrix.q, &out[entry->index].q, &cur->q);

            quat.x = -out[entry->index].q.x;
            quat.y = -out[entry->index].q.y;
            quat.z = -out[entry->index].q.z;
            quat.w = out[entry->index].q.w;

            QuatRotateVec(&cur->t, (float*)&quat, &entry->matrix.t);

            cur->t.x += out[entry->index].t.x;
            cur->t.y += out[entry->index].t.y;
            cur->t.z += out[entry->index].t.z;
        }

        off += 0x28;
        cur++;
    }

    out->q = out[1].q;
    out->t = out[1].t;
}

typedef unsigned short u16;

// Per-mode setup record of the owner: the mode hash the caller passes, the size
// of the mode buffer and the index of the bone-set table this mode uses.
struct BatsqueekOwnerMode {
    u32 hash;                   // 0x00
    u16 bufferSize;             // 0x04
    u16 boneSetIndex;           // 0x06
    u8 pad08[0x08];
};
extern BatsqueekOwnerMode lbl_803BAAC4[];   // three records

// 0x24-byte bone-set records referenced by the table above.
struct BatsqueekOwnerBoneSet {
    u8 pad00[0x24];
};
extern BatsqueekOwnerBoneSet lbl_803BAAF4[];

// The owner object is built and owned by another unit; only the two virtual
// slots this file calls and the fields the setup writes are modelled here.
struct u8020A650Owner;
struct u8020A650OwnerVtable {
    u8 pad00[0x24];
    void (*stop)(u8020A650Owner* owner);                    // 0x24
    void (*activate)(u8020A650Owner* owner, int flag);      // 0x28
};

struct u8020A650Anim {
    u8 pad00[0x04];
};

struct u8020A650Owner {
    u8020A650OwnerVtable* vtable;   // 0x00
    u16 field04;                    // 0x04: model index
    u16 boneCount;                  // 0x06
    s32 mode;                       // 0x08
    void* entries;                  // 0x0C: one 0x28-byte entry per bone
    void* field10;                  // 0x10
    void* buffer;                   // 0x14: per-mode buffer
    u8* boneSets;                   // 0x18: bone-set record of the mode
    float timer;                    // 0x1C
};

int ModelBoneCount(int model);
void* VAlloc(int size);
extern "C" void* memset(void* dest, int value, u32 count);
int u8020A650BuildModel(u8020A650Owner* owner, int mode, u8* data,
                        u8020A650Anim* anim, Mtx34* mtx);

// Sets up one of the owner's Batsqueeks modes: allocates the per-bone entry array
// and the mode buffer, then lets the model builder fill them in. Returns 0 when
// the mode hash is unknown or an allocation or the build fails.
int BatsqueekOwnerSetup(u8020A650Owner* owner, int model, u32 modeHash, u8* data,
                        Mtx34* mtx, u8020A650Anim* anim)
{
    if (model < -1)
        return 0;

    if (owner->entries != 0)
        owner->vtable->stop(owner);

    int mode;
    if (modeHash == lbl_803BAAC4[0].hash)
        mode = 0;
    else if (modeHash == lbl_803BAAC4[1].hash)
        mode = 1;
    else if (modeHash == lbl_803BAAC4[2].hash)
        mode = 2;
    else
        mode = 3;

    if (mode == 3)
        return 0;

    owner->mode = mode;
    owner->boneCount = (u16)ModelBoneCount(model);
    owner->entries = VAlloc(owner->boneCount * 0x28);
    if (owner->entries == 0)
        return 0;

    memset(owner->entries, 0, owner->boneCount * 0x28);

    owner->buffer = VAlloc(lbl_803BAAC4[mode].bufferSize);
    if (owner->buffer == 0)
    {
        VFree(owner->entries);
        owner->entries = 0;
        return 0;
    }

    memset(owner->buffer, 0, lbl_803BAAC4[mode].bufferSize);
    owner->field04 = (u16)model;
    owner->boneSets = (u8*)&lbl_803BAAF4[lbl_803BAAC4[mode].boneSetIndex];
    owner->timer = lbl_804D937C;

    if (u8020A650BuildModel(owner, mode, data, anim, mtx) == 0)
    {
        VFree(owner->entries);
        owner->entries = 0;
        VFree(owner->buffer);
        owner->buffer = 0;
        return 0;
    }

    owner->vtable->activate(owner, 0);
    return 1;
}

// Mtx34 plus context, initialized by the runtime before every debug draw.
struct BatsqueekDebugLink {
    Mtx34 matrix;               // 0x00: translation of the drawn box
    u8 pad1C[0x1C];
};

void LinkInfoInit(void* link);
void Mtx34StoreTranslation(Mtx34* matrix, Vec3* pos);
void DebugDrawBoneBox(Vec3* box, void* link, int color, int a, int b);

extern float lbl_804D9380;
extern float lbl_804D9384;

// Draws a debug box at every position currently recorded by the sound trigger
// list, then clears the recorded position count.
void BatsqueekDrawTriggerBoxes(void)
{
    Vec3 box[2];
    BatsqueekDebugLink link;

    box[0].x = -24.0f;
    box[0].y = -24.0f;
    box[0].z = -24.0f;
    box[1].x = 24.0f;
    box[1].y = 24.0f;
    box[1].z = 24.0f;

    LinkInfoInit(&link);

    for (int i = 0; i < lbl_804D53A0; i++)
    {
        Mtx34StoreTranslation(&link.matrix, &lbl_8049E130[i]);
        DebugDrawBoneBox(box, &link, 0xff, 0, 0);
    }

    lbl_804D53A0 = 0;
}

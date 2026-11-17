// u_80208238.cpp: player "Batsqueeks" host mode (per-frame update, setup, mode id tables)
// (0x80208238-0x80209C30). Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

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

// Per-action-mode table (0x48 bytes per row): three hooks, then one motion id per character variant.
typedef void (*BatsqueekHook)(Player* player);
typedef void (*BatsqueekHook04)(Player* player, int arg);
struct AnimRow {
    BatsqueekHook hook00;                       // 0x00
    BatsqueekHook04 hook04;                     // 0x04
    BatsqueekHook callback;                     // 0x08
    s32 byVariant[15];                          // 0x0C
};
extern AnimRow gBatsqueekModeTable[];
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
int gBatsqueekAssets[16];       // asset handles loaded by fn_80209A14
extern "C" int SendMsg(void* target, int msg, void* data);

void PlayerSetHeldSlotIndex(void* player, int index);

// Message 0xC06: asks the peer whether it handles the given mode message itself.
struct NodeQuery {
    s32 unk0;                   // 0x00
    s32 msg;                    // 0x04
    s32 unk8;                   // 0x08
    void* target;               // 0x0C
    s32 unk10[4];               // 0x10
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
    int idx = player->field73C;

    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field18,
                                   gBatsqueekModeTable[23].byVariant[idx], 0, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 1;

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field1C,
                                   gBatsqueekModeTable[23].byVariant[idx], 0, 0);
    player->field0FC = 1;
    player->field7D0 = 0;
    if (player->field7D0 != 0)
        PlayerSetValue4(&player->field7A4, 3);
    else
        PlayerSetValue4(&player->field7A4, 1);
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

    if ((int)(anim->field38 >> 24) == 0x13)
    {
        state = (HostState*)fn_800A384C(anim);
        if (HasCondition(anim->field270, 0xb) != 0)
            used = 1;
    }

    int idx;

    PlayerInHost((Actor*)player);
    idx = player->field73C;

    player->field098 = fn_800A1BC8(state->field14, state->field18,
                                   gBatsqueekModeTable[21].byVariant[idx], used, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 0x19;

    player->field0F0 = fn_800A1BC8(state->field14, state->field1C,
                                   gBatsqueekModeTable[21].byVariant[idx], used, 0);
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

// Hosts the Batsqueeks mode: builds the per-slot motion objects and asks the peer
// whether the player already holds a slot, which picks the follow-up action mode.
void PlayerBatsqueekGunSetup(Player* player)
{
    int slot = PlayerInHost((Actor*)player);
    int idx = player->field73C;

    player->field78 = 0;
    player->field50 &= ~0x20000u;
    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field18,
                                   gBatsqueekModeTable[20].byVariant[idx], 0, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 1;
    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field1C,
                                   gBatsqueekModeTable[20].byVariant[idx], 0, 0);
    player->field0FC = 1;

    QueryMsg query;
    query.hash = 0x8783452;
    SendMsg(((PlayerPeer*)player->field26E4)->peer38, 0x41e, &query);
    if (*(int*)&query.result != 0)
    {
        PlayerSetHeldSlotIndex(player, 0);
        if ((s32)player->field098 == -1)
            PlayerSetActionMode(player, 1);
    }
    else
    {
        PlayerSetHeldSlotIndex(player, 1);
        if ((s32)player->field098 == -1)
            PlayerSetActionMode(player, 3);
    }
}

// Clears the Batsqueeks mode bit on the player (or arms the flag when it was not
// set) and marks the player as active in the host mode.
void PlayerBatsqueekResetGunState(Player* player)
{
    PlayerResetGunTimer(player);
    player->field638 = 1;
    if ((player->flags7B4 & 1) != 0)
    {
        player->flags7B4 &= ~1u;
    }
    else
    {
        player->field50 |= 0x20000;
        player->field78 = 1;
    }
}

// The host animation object the player is attached to.
static inline HostAnim* PlayerHeldAnim(Player* player) { return player->field26E8; }

// Sets up the two motion objects of the Batsqueeks host mode and asks the peer
// whether the player already holds a host slot.
void PlayerBatsqueekHostSlotSetup(Player* player)
{
    int slot;
    HostState* state;
    int used = 0;
    int idx;
    HostAnim* anim = PlayerHeldAnim(player);
    if ((int)(anim->field38 >> 24) == 0x13)
    {
        state = (HostState*)fn_800A384C(anim);
        if (HasCondition(anim->field270, 0xb) != 0)
            used = 1;
    }
    slot = PlayerInHost((Actor*)player);
    idx = player->field73C;
    player->field098 = fn_800A1BC8(state->field14, state->field18, gBatsqueekModeTable[19].byVariant[idx], used, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 0x19;
    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field1C,
                                   gBatsqueekModeTable[19].byVariant[idx], 0, 0);
    player->field0FC = 0x19;

    QueryMsg query;
    query.hash = 0x8783452;
    SendMsg((void*)player->field26E8->field38, 0x41e, &query);
    if (*(int*)&query.result != 0)
        PlayerSetHeldSlotIndex(player, 0);
    else
        PlayerSetHeldSlotIndex(player, 1);
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
    HostState* state;
    int slot = PlayerInHost((Actor*)player);
    HostAnim* anim = player->field26E8;
    int idx = player->field73C;
    int used = 0;

    if ((int)(anim->field38 >> 24) == 0x13)
    {
        state = (HostState*)fn_800A384C(anim);
        if (HasCondition(anim->field270, 0xb) != 0)
            used = 1;
    }
    player->field098 = fn_800A1BC8(state->field14, state->field18, gBatsqueekModeTable[18].byVariant[idx], used, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 0x59;
    HostSlot* host = player->slots26C4[slot];
    player->field0F0 = fn_800A1BC8(host->field14, host->field1C, gBatsqueekModeTable[18].byVariant[idx], 0, 0);
    player->field0FC = 0x59;

    QueryMsg query;
    query.hash = 0x8783452;
    SendMsg((void*)player->field26E8->field38, 0x41e, &query);
    if (*(int*)&query.result != 0)
        PlayerSetHeldSlotIndex(player, 0);
    else
        PlayerSetHeldSlotIndex(player, 1);
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
                                   gBatsqueekModeTable[13].byVariant[index], 0, 0);
    player->field0A4 = 0x50;

    if (player->field26B0 == -1)
    {
        bone = 8;
    }
    else
    {
        bone = ModelBoneNodeChain(player->field58,
                                  ModelBoneName(player->field58, player->field26B0),
                                  gBatsqueekAssets, 0x10);
        if (bone == -1)
            bone = 8;
    }

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field1C,
                                   gBatsqueekModeTable[13].byVariant[index], bone << 1, 0);
    player->field0E4 = (void*)PlayerBatsqueekModeCallback;
    player->field0FC = 0x50;
}

// Asks the player's message peer for a mode handle. The peer answers by writing
// the handle into the query block's trailing word, which is cleared beforehand.
static inline void QueryGet(s32* out, void* target, u32 hash)
{
    QueryMsg query;
    query.hash = hash;
    *(s32*)&query.result = 0;
    query.unk8 = 0;
    SendMsg(target, 0x41e, &query);
    *out = *(s32*)&query.result;
}

// Per-frame update used while the player is still picking a host mode: asks the
// peer for the two mode handles, then drives the motion and the action mode.
void PlayerBatsqueekQueryUpdate(Player* player, float seek)
{
    s32 first;
    s32 second;

    QueryGet(&first, player->field38, 0x2da8293c);
    QueryGet(&second, player->field38, 0xdff3097a);

    player->field638 = 1;
    player->flags7B4 &= ~1u;
    fn_800769A0(player, seek);
    PlayerUpdateMotionSeek(seek, player);
    player->field7D0 = 1;

    float speed;
    {
        float x = player->field394;
        float z = player->field38C;
        speed = __fabsf(z) > __fabsf(x) ? __fabsf(z) : __fabsf(x);
    }

    if (0.0f != speed)
        fn_8008E824(player->field7C, 0);

    if (0.0f == player->field7D8)
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
    int index;
    s32 first;
    s32 second;

    QueryGet(&first, player->field38, 0x2da8293c);
    QueryGet(&second, player->field38, 0xdff3097a);

    int host = PlayerInHost((Actor*)player);
    index = player->field73C;
    int found = Fn800F7DEC(first, second, 0x20ee36, fn_800498D0(player), 0);
    int used = 0;

    if (found == -0x3F413082 || found == 0x11D2C025)
        used = 1;

    player->field098 = (void*)-1;
    while ((s32)player->field098 == -1)
    {
        player->field098 = fn_800A1BC8(player->slots26C4[host]->field14, player->slots26C4[host]->field18,
                                       gBatsqueekModeTable[3].byVariant[index], used, 0);
        player->field08C = (void*)PlayerBatsqueekModeCallback;
        player->field0A4 = 1;
        if ((s32)player->field098 == -1 && used)
            used = 0;
    }

    HostSlot* slot = player->slots26C4[host];
    player->field0F0 = fn_800A1BC8(slot->field14, slot->field1C, gBatsqueekModeTable[3].byVariant[index], 0, 0);
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

// Sets up the two slot motion objects of the Batsqueeks host mode and stores the
// follow-up action mode.
void PlayerBatsqueekSlotMotionSetup(Player* player)
{
    int slot = PlayerInHost((Actor*)player);
    int index = player->field73C;

    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field18,
                                   gBatsqueekModeTable[1].byVariant[index], 0, 0);
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 1;

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field1C,
                                   gBatsqueekModeTable[1].byVariant[index], 0, 0);
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
        (int)player->flags328 != 0)
        PlayerSetActionMode(player, 3);
}

// Sets up the current slot's two motion objects after asking the peer for the mode
// handles; the peer's answer (or a random draw) picks the motion variant at 0x7D4.
void PlayerBatsqueekSlotModeSetup(Player* player)
{
    s32 first;
    s32 second;

    QueryGet(&first, player->field38, 0x2da8293c);
    QueryGet(&second, player->field38, 0xdff3097a);

    int found = Fn800F7DEC(first, second, 0x20ee36, fn_800498D0(player), 0);

    if (found == -0x3F413082 || found == 0x11D2C025)
        player->field7D4 = 2;
    else if ((float)rand() / 32767.0f >= 0.5f)
        player->field7D4 = 0;
    else
        player->field7D4 = 1;

    int slot = PlayerInHost((Actor*)player);

    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field18,
                                   gBatsqueekModeTable[0].byVariant[0], player->field7D4, 0);
    if ((s32)player->field098 == -1)
    {
        player->field7D4 = 0;
        player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field18,
                                       gBatsqueekModeTable[0].byVariant[0], player->field7D4, 0);
    }
    player->field08C = (void*)PlayerBatsqueekModeCallback;
    player->field0A4 = 1;

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field1C,
                                   gBatsqueekModeTable[0].byVariant[0], player->field7D4, 0);
    if ((s32)player->field0F0 == -1)
    {
        player->field7D4 = 0;
        player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field1C,
                                       gBatsqueekModeTable[0].byVariant[0], player->field7D4, 0);
    }
    player->field0E4 = (void*)PlayerBatsqueekModeCallback;
    player->field0FC = 1;
}

// Per-mode callback: asks the player's peer for the two mode handles, then reacts to
// the two mode change messages (re-arm the bite flag / restart the swing effect).
int PlayerBatsqueekModeCallback(int msg, void* p)
{
    Player* player = (Player*)p;
    NodeQuery query;

    query.target = player->field38;
    query.msg = msg;
    if (fn_800C5210(player->field38, 0xc06, &query) == 1)
        return 0;

    s32 first;
    s32 second;
    QueryGet(&first, player->field38, 0x2da8293c);
    QueryGet(&second, player->field38, 0xdff3097a);

    switch (msg)
    {
    case 0x1b83833f:
        if (player->field7A4 == 0xd)
            player->flags7B4 |= 0x20000;
        else
            PlayerSetActionMode(player, player->field7A8);
        break;
    case 0x28a80b:
        FxStart(FxFind("Batwing"), 0, 0);
        break;
    }

    return 0;
}

struct Actor;
struct LargeActor;
void PlayerDashStart(Actor* actor);
void PlayerDashCheck(Actor* actor);
void LargeActorResetFlags(LargeActor* actor);

// The per-action-mode table: setup and update hooks, an optional callback and
// the motion ids of each character variant, one row per action mode.
AnimRow gBatsqueekModeTable[25] = {
    { (BatsqueekHook)PlayerBatsqueekSlotModeSetup, (BatsqueekHook04)PlayerBatsqueekExitHost, (BatsqueekHook)PlayerBatsqueekResetGunTimer,
      { 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0 } },
    { (BatsqueekHook)PlayerBatsqueekSlotMotionSetup, (BatsqueekHook04)PlayerBatsqueekExitMotionUpdate, 0,
      { 0xa8be217b, 0x744b56b9, 0x90a41c8e, 0x29d70f30, 0xa81c49f3, 0xa798111, 0x35fc8e1c, 0xa9d72f66, 0x8254aa67, 0xd3193f08, 0x81548f05, 0x40ec2f58, 0xb1bec6a3, 0x11042941, 0 } },
    { 0, 0, 0,
      { 0x34cd59aa, 0xd84584ac, 0x23823037, 0x30b56155, 0x454c4232, 0x58fcaf54, 0x83ae54e9, 0xb0b5625f, 0x230c033e, 0x9101d67d, 0x4c4e12e0, 0xffe79c2d, 0x140f6f82, 0x1f644124, 0 } },
    { (BatsqueekHook)PlayerBatsqueekSlotHandleSetup, (BatsqueekHook04)PlayerBatsqueekQueryUpdate, 0,
      { 0x2a1a099, 0xc5d70d57, 0x3be1446c, 0x5514778e, 0xd9df8711, 0xc1bbedaf, 0xe424c17a, 0x2feb2d44, 0x3bdd7485, 0xdc50bd66, 0x1c29e8a3, 0x5bc381b6, 0x1a596fc1, 0xa32e1df, 0 } },
    { 0, 0, 0,
      { 0x6db3b0e5, 0x3940e623, 0x5599abf8, 0xeecc9e9a, 0x6d11d95d, 0xcf6f107b, 0xfaf21d86, 0x6eccbed0, 0x474a39d1, 0x980ece72, 0x464a1e6f, 0x5e1bec2, 0x76b4560d, 0xd5f9b8ab, 0 } },
    { 0, 0, 0,
      { 0xce83f460, 0x50aef7e2, 0xf9a70a2d, 0x5d8feacb, 0xcd335ee8, 0x1f00ac8a, 0xf98b35f, 0x2ad0a655, 0x7dc7f2f4, 0xcd4789f3, 0x2a2d8316, 0x4d42fba3, 0xb681a038, 0xcec2725a, 0 } },
    { 0, 0, 0,
      { 0x48c2836e, 0xeedbf5f0, 0x5d9beabb, 0xeac74c59, 0x352969f6, 0x1e731698, 0x6351aaed, 0x6acb32e3, 0xa1fd1802, 0x69cb1781, 0x82f9c724, 0xb6563131, 0x5fd68346, 0xe93d7468, 0 } },
    { 0, 0, 0,
      { 0x21820da8, 0xa708e7aa, 0x659ed5b5, 0x557847d3, 0x8fc97230, 0x4d5a7e52, 0xd4e55167, 0xe41cb3dd, 0x1669cd3c, 0x22b5e8fb, 0xaaf73bde, 0x78e156ab, 0xc3957780, 0x8f04a822, 0 } },
    { 0, 0, 0,
      { 0x48c24dc9, 0xeedbc04b, 0x5d9bb516, 0xeac716b4, 0x35293451, 0x1e72e0f3, 0x63517548, 0x6acafd3e, 0xa1fce25d, 0x69cae1dc, 0x82f9917f, 0xb655fb8c, 0x5fd64da1, 0xe93d3ec3, 0 } },
    { 0, 0, 0,
      { 0x64ddf349, 0x306b2887, 0x4cc3ee5c, 0xe5f6e0fe, 0x643c1bc1, 0xc69952df, 0xf21c5fea, 0x65f70134, 0x3e747c35, 0x8f3910d6, 0x3d7460d3, 0xfd0c0126, 0x6dde9871, 0xcd23fb0f, 0 } },
    { 0, 0, 0,
      { 0x51979943, 0xf50fc445, 0x404c6fd0, 0x4d7fa0ee, 0x621681cb, 0x75c6eeed, 0xa0789482, 0xcd7fa1f8, 0x3fd642d7, 0xadcc1616, 0x69185279, 0x1cb1dbc6, 0x30d9af1b, 0x3c2e80bd, 0 } },
    { 0, 0, 0,
      { 0x1de2c92c, 0xa369a32e, 0x61ff9139, 0x51d90357, 0x8c2a2db4, 0x49bb39d6, 0xd1460ceb, 0xe07d6f61, 0x12ca88c0, 0x1f16a47f, 0xa757f762, 0x7542122f, 0xbff63304, 0x8b6563a6, 0 } },
    { (BatsqueekHook)PlayerDashStart, (BatsqueekHook04)PlayerDashCheck, (BatsqueekHook)LargeActorResetFlags,
      { 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0 } },
    { (BatsqueekHook)PlayerBatsqueekWeaponMotionSetup, (BatsqueekHook04)PlayerBatsqueekSeekUpdate, (BatsqueekHook)PlayerBatsqueekClearCarryBit,
      { 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0 } },
    { 0, 0, 0,
      { 0xf8b4bd22, 0x15c99ae0, 0x7f083f75, 0x97491197, 0x992aa79a, 0xd9168f38, 0x300a8583, 0x17c1fc4d, 0xc6ccbb0e, 0xf8beab6f, 0x565f02c, 0x3d96c7bf, 0xc420b84a, 0x6797eb68, 0 } },
    { 0, 0, 0,
      { 0x1de2fed1, 0xa369d8d3, 0x61ffc6de, 0x51d938fc, 0x8c2a6359, 0x49bb6f7b, 0xd1464290, 0xe07da506, 0x12cabe65, 0x1f16da24, 0xa7582d07, 0x754247d4, 0xbff668a9, 0x8b65994b, 0 } },
    { 0, 0, 0,
      { 0x3fe4c227, 0x5cf99fe5, 0xc638447a, 0xde79169c, 0xe05aac9f, 0x2046943d, 0x773a8a88, 0x5ef20152, 0xdfcc013, 0x3feeb074, 0x4c95f531, 0x84c6ccc4, 0xb50bd4f, 0xaec7f06d, 0 } },
    { 0, 0, 0,
      { 0x38243a5e, 0xdb9c6560, 0x26d910eb, 0x340c4209, 0x48a322e6, 0x5c539008, 0x8705359d, 0xb40c4313, 0x2662e3f2, 0x9458b731, 0x4fa4f394, 0x33e7ce1, 0x17665036, 0x22bb21d8, 0 } },
    { (BatsqueekHook)PlayerBatsqueekCarryStateSetup, (BatsqueekHook04)BatsqueekHostEnterGunMode, (BatsqueekHook)BatsqueekHostAnimStart,
      { 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0 } },
    { (BatsqueekHook)PlayerBatsqueekHostSlotSetup, (BatsqueekHook04)PlayerBatsqueekResetGunState, 0,
      { 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0 } },
    { (BatsqueekHook)PlayerBatsqueekGunSetup, (BatsqueekHook04)PlayerBatsqueekFrameUpdate, 0,
      { 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0 } },
    { (BatsqueekHook)PlayerBatsqueekHostSetup, (BatsqueekHook04)PlayerBatsqueekResetMode, 0,
      { 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0 } },
    { (BatsqueekHook)PlayerBatsqueekTeardown, (BatsqueekHook04)PlayerBatsqueekExitUpdate, 0,
      { 0x2a1a099, 0xc5d70d57, 0x3be1446c, 0x5514778e, 0xd9df8711, 0xc1bbedaf, 0xe424c17a, 0x2feb2d44, 0x3bdd7485, 0xdc50bd66, 0x1c29e8a3, 0x5bc381b6, 0x1a596fc1, 0xa32e1df, 0 } },
    { (BatsqueekHook)PlayerBatsqueekSetup, (BatsqueekHook04)PlayerBatsqueekUpdate, 0,
      { 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0xd9df8711, 0 } },
    { 0, 0, 0,
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
};

// One entry of the per-action-mode table overlaid on gBatsqueekModeTable (0x48 bytes each).
struct BatsqueekModeEntry {
    u8 pad000[0x08];
    void (*callback)(Player* player);   // 0x08: per-mode callback
    u8 pad00C[0x38];
    s32 motionId;                       // 0x44: per-mode motion id
};

// Return the motion id registered for the player's current action mode.
s32 BatsqueekGetModeMotionId(Player* player) {
    BatsqueekModeEntry* table = (BatsqueekModeEntry*)gBatsqueekModeTable;
    return table[player->field7A4].motionId;
}

// Run the per-action-mode callback for the player's current mode, if one is set.
void BatsqueekRunModeCallback(Player* player) {
    BatsqueekModeEntry* table = (BatsqueekModeEntry*)gBatsqueekModeTable;
    if (table[player->field7A4].callback != 0) {
        table[player->field7A4].callback(player);
    }
}

// Function pointers stored at the start of each 0x48-byte per-action-mode table entry.
struct BatsqueekModeHooks {
    void (*hook00)(Player* player);     // 0x00
    void (*hook04)(Player* player, int arg); // 0x04
    u8 pad008[0x40];
};

// Run the per-action-mode function pointer stored at table entry offset 0x04, if set.
void BatsqueekRunModeHook04(Player* player, int arg) {
    BatsqueekModeHooks* table = (BatsqueekModeHooks*)gBatsqueekModeTable;
    void (*fn)(Player*, int) = table[player->field7A4].hook04;
    if (fn != 0) {
        fn(player, arg);
    }
}

// Run the per-action-mode function pointer stored at table entry offset 0x00, if set.
void BatsqueekRunModeHook00(Player* player) {
    BatsqueekModeHooks* entry = &((BatsqueekModeHooks*)gBatsqueekModeTable)[player->field7A4];
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
    gBatsqueekAssets[0] = AssetFind(0xf1754bf2, 0x21c5e0);
    gBatsqueekAssets[1] = AssetFind(0xf1754bf2, 0x24805f);
    gBatsqueekAssets[2] = AssetFind(0xf1754bf2, 0x101844cc);
    gBatsqueekAssets[3] = AssetFind(0xf1754bf2, 0x48874f6e);
    gBatsqueekAssets[4] = AssetFind(0xf1754bf2, 0x4a53fc1);
    gBatsqueekAssets[5] = AssetFind(0xf1754bf2, 0x967daac6);
    gBatsqueekAssets[6] = AssetFind(0xf1754bf2, 0xd9404234);
    gBatsqueekAssets[7] = AssetFind(0xf1754bf2, 0x450b2bb);
    gBatsqueekAssets[8] = AssetFind(0xf1754bf2, 0x922bf04e);
    gBatsqueekAssets[9] = AssetFind(0xf1754bf2, 0x8c73dfa9);
    gBatsqueekAssets[10] = AssetFind(0xf1754bf2, 0x90aeeee4);
    gBatsqueekAssets[11] = AssetFind(0xf1754bf2, 0x4a2f9aa);
    gBatsqueekAssets[12] = AssetFind(0xf1754bf2, 0x4a48bc0);
    gBatsqueekAssets[13] = AssetFind(0xf1754bf2, 0x8671db2a);
    gBatsqueekAssets[14] = AssetFind(0xf1754bf2, 0x44e6ca4);
    gBatsqueekAssets[15] = AssetFind(0xf1754bf2, 0x44ffeba);
}

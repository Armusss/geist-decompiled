// u_801EDE74.cpp: player held-slot, gun aim and item message states (0x801EDE74-0x801F1798).
// Names are descriptive guesses; the code is matched against the retail DOL.
// Player gun/zoom per-frame update and held-item state setup. Names are
// descriptive guesses; field offsets come from the retail instructions.

typedef unsigned char u8;
typedef signed int s32;
typedef unsigned int u32;

class Actor;

// Player actor fields touched by this unit (offsets from retail loads/stores).
struct PlayerObj {
    u8 unk000[0x38];
    void* field38;                      // 0x38, item-name message target
    u8 unk03c[0x50 - 0x3c];
    u32 flags50;                        // 0x50
    u8 unk054[0x58 - 0x54];
    void* field58;                      // 0x58, model object
    u8 unk05c[0x78 - 0x5c];
    s32 field78;                        // 0x78
    void* field7c;                      // 0x7c, collision/animation object
    u8 unk080[0x8c - 0x80];
    void* field8c;                      // 0x8c
    u8 unk090[0x98 - 0x90];
    s32 field98;                        // 0x98
    u8 unk09c[0xa4 - 0x9c];
    s32 fielda4;                        // 0xa4
    u8 unk0a8[0xe4 - 0xa8];
    void (*fielde4)(void);              // 0xe4, per-frame held-item callback
    u8 unk0e8[0xf0 - 0xe8];
    s32 fieldf0;                        // 0xf0
    u8 unk0f4[0xfc - 0xf4];
    s32 fieldfc;                        // 0xfc
    u8 unk100[0x184 - 0x100];
    s32 field184;                       // 0x184
    u8 unk188[0x2f4 - 0x188];
    u8 field2f4[4];                     // 0x2f4, aim/timer state block
    u8 unk2f8[0x314 - 0x2f8];
    s32 field314;                       // 0x314
    u8 unk318[0x320 - 0x318];
    u32 field320;                       // 0x320, player action flags
    s32 field324;                       // 0x324, player action flags
    s32 field328;                       // 0x328
    u8 unk32c[0x38c - 0x32c];
    float field38c;                     // 0x38c
    float field390;                     // 0x390
    float field394;                     // 0x394
    float field398;                     // 0x398
    u8 unk39c[0x588 - 0x39c];
    s32 field588;                       // 0x588, held-item state id
    u8 unk58c[0x5e8 - 0x58c];
    float field5e8;                     // 0x5e8, accumulated aim timer
    u8 unk5ec[0x638 - 0x5ec];
    s32 field638;                       // 0x638
    u8 unk63c[0x678 - 0x63c];
    u32 field678;                       // 0x678, player flags
    u8 unk67c[0x73c - 0x67c];
    s32 field73c;                       // 0x73c
    u8 unk740[0x7a4 - 0x740];
    s32 field7a4;                       // 0x7a4
    s32 field7a8;                       // 0x7a8, action/animation id
    u8 unk7ac[0x7b4 - 0x7ac];
    u32 field7b4;                       // 0x7b4
    u8 unk7b8[0x7d0 - 0x7b8];
    s32 field7d0;                       // 0x7d0
    s32 field7d4;                       // 0x7d4, held-item variant id
    float field7d8;                     // 0x7d8
    u8 unk7dc[0x7e0 - 0x7dc];
    s32 field7e0;                       // 0x7e0
    u8 unk7e4[0x800 - 0x7e4];
    s32 field800;                       // 0x800, held-item effect id
    u8 unk804[0x14fc - 0x804];
    float field14fc;                    // 0x14fc
    u8 unk1500[0x1d04 - 0x1500];
    u8 field1d04[0x1d54 - 0x1d04];      // 0x1d04, held-object effect state
    s32 field1d54;                      // 0x1d54, effect id copied from 0x800
    u8 unk1d58[0x26b0 - 0x1d58];
    s32 field26b0;                      // 0x26b0, model bone id or -1
    u8 unk26b4[0x26c4 - 0x26b4];
    void* field26c4;                    // 0x26c4, array indexed by host index
    void* field26c8;                    // 0x26c8
    u8 unk26cc[0x26e0 - 0x26cc];
    void* field26e0;                    // 0x26e0, message block passed to the zoom helper
    void* field26e4;                    // 0x26e4
    void* field26e8;                    // 0x26e8, owner object queried by the zoom state
};

struct ZoomSource {
    u8 unk000[0x104];
    s32 field104;                       // 0x104
};

struct HostSound {
    u8 unk000[0x14];
    s32 field14;                        // 0x14
    s32 field18;                        // 0x18
    s32 field1c;                        // 0x1c
};

struct OwnerObject {
    u8 unk000[0x38];
    void* target;                       // 0x38
};

// FOV applied while the zoom state is active (retail: lbl_804D8F70@sda21;
// the exact literal still has to be read out of the sdata2 pool).
static const float kZoomInFov = 1.0f;

// Share of the zoom factor below which the aim update is skipped (retail:
// lbl_804D8F80@sda21; read as 0.0f for now, the literal has to be confirmed).
static const float kZeroF = 0.0f;

// Small message payload: retail writes only the trailing hash.
struct PlayerMsgArg {
    int unk0;                           // 0x0 (left untouched by retail)
    u32 hash;                           // 0x4
};

// Minimal String model: Buffer declares the virtual destructor; String's
// implicit destructor stores __vt__6String and calls __dt__6BufferFv.
class Buffer {
public:
    virtual ~Buffer();
};

class String : public Buffer {
public:
    String();
};

// Message payload for the item-name query (msg 0x41e): a three-word header
// followed by the String the target fills in.
struct ItemNameMsg {
    int unk0;                           // 0x0
    u32 hash;                           // 0x4
    int unk8;                           // 0x8
    String name;                        // 0xc
};

extern "C" void fn_800769A0(void*, float);
extern "C" void fn_80155A84(void*, void*, int);
extern "C" int fn_80049940(void);
extern "C" int fn_800A1BC8(int, int, int, int, int);
extern "C" void fn_80048BA8(void*, void*, int);
extern "C" int fn_800498D0(void*);
void Fn800F7F5C(int, int, int, int, void*, float);
extern "C" void fn_80050EB0(void*);
extern "C" int lbl_8049D1C8[];
int ModelBoneName(int, int);
int ModelBoneNodeChain(int, int, int*, int);
extern "C" void* fn_800A384C(void*);
int HasCondition(void*, int);
extern "C" void fn_801F0ED8(void);
extern "C" void FxStart(void*, int, int);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" unsigned char lbl_803B9378[];
extern "C" void fn_8008E824(void*, int);
extern "C" float fabsf(float);
extern "C" int fn_801F0994(void*, float);
int PlayerHeldItemSlotCheck(void*, float);
extern "C" int rand(void);
extern "C" float fn_800699B8(void*);
extern "C" void fn_800699A8(void*);
extern "C" double lbl_804D8F78;
extern "C" float lbl_804D8F80;
extern "C" float lbl_804D8F84;
extern "C" float lbl_804D8F88;
extern "C" float lbl_804D8F8C;
extern "C" float lbl_804D8F90;
extern "C" float lbl_804D8F94;
extern "C" float lbl_804D52A8;
extern "C" int fn_802A1D04(void);
extern "C" int fn_800C5210(int, int, void*);

void PlayerResetGunTimer(void*);
void PlayerSelectCollisionUpdate(void*, float);
void PlayerSetValue4(void*, int);
void PlayerSetHeldSlotMode(void*, int);
void PlayerSetHeldSlotIndex(void*, int);
void PlayerSetActionMode(void*, int);
int PlayerGetHeldSlotIndex(void*);
int PlayerInHost(Actor*);
void CameraSetFov(int, float);
void ActorPlayAnim(int, unsigned long, int);
void* FxFind(const char*);

// 0x801EDE74: per-frame gun/zoom state update. r3 is the player, f1 the
// aim/zoom factor carried across both helper calls.
void PlayerGunZoomUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    int prevMode;
    int mode;

    PlayerResetGunTimer(p);
    p->field638 = 1;
    p->field7b4 &= ~1u;
    fn_800769A0(p, f);
    PlayerSelectCollisionUpdate(p, f);

    if (fn_80049940() != 0) {
        PlayerSetValue4(&p->field7a4, 2);
    } else if (PlayerGetHeldSlotIndex(p) != 0) {
        PlayerSetValue4(&p->field7a4, 2);
    } else {
        PlayerSetValue4(&p->field7a4, 1);
    }

    PlayerInHost((Actor*)p);
    prevMode = p->field7e0;

    if ((p->field328 == 1 || p->field328 == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            goto done;
        }
        PlayerResetGunTimer(p);
        if (p->field7e0 == 0)
            p->field7e0 = 3;
        else
            p->field7e0 = 0;
    }

    mode = p->field7e0;
    if (mode == 3) {
        CameraSetFov(p->field184, kZoomInFov);
    } else if (mode == 0) {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }

    if (p->field7e0 != prevMode) {
        if (prevMode < p->field7e0)
            FxStart(FxFind("zoom_out"), 0, 0);
        else
            FxStart(FxFind("zoom_level1"), 0, 0);
    }
done:
    ;
}

// 0x801EE050: (re)initialise the held-item state: start the two held sounds,
// install the 0x801F0ED8 callback and query the owner about the item name.
void PlayerBeginHeldItemState(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    void* nameData;
    int hostIndex;
    int slot;
    PlayerMsgArg arg;

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    p->field78 = 0;
    p->flags50 &= 0xffffbfffu;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0x16b + slot], 0, 0);
    p->field8c = (void*)fn_801F0ED8;
    p->fielda4 = 1;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[0x16b + slot], 0, 0);
    p->fieldfc = 1;

    String name;
    arg.hash = 0x08783452;
    SendMsg((int)(((OwnerObject*)p->field26e4)->target), 0x41e, &arg);

    // Retail tests the String object's first word (0x14(r1)).
    nameData = *(void**)&name;
    if (nameData != 0) {
        PlayerSetHeldSlotIndex(p, 0);
        if (p->field98 == -1)
            PlayerSetActionMode(p, 1);
    } else {
        PlayerSetHeldSlotIndex(p, 1);
        if (p->field98 == -1)
            PlayerSetActionMode(p, 3);
    }
}

// 0x801EE1C0: start the gun timer and toggle the zoom flag: if the zoom bit
// was already set it is cleared, otherwise the player is marked zooming.
void PlayerGunZoomEnter(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;

    PlayerResetGunTimer(p);
    p->field638 = 1;
    if (p->field7b4 & 1) {
        p->field7b4 &= ~1u;
    } else {
        p->flags50 |= 0x20000;
        p->field78 = 1;
    }
}

// 0x801EE21C: enter the zoom state: build the two held-item sounds, install
// the zoom callback, ask the owner for the item name and pick the held slot.
void PlayerBeginZoomState(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    OwnerObject* owner;
    HostSound* host;
    HostSound* held;
    void* nameData;
    int hostIndex;
    int slot;
    int flag;
    PlayerMsgArg arg;

    flag = 0;
    owner = (OwnerObject*)p->field26e8;
    if (((u32)owner->target >> 24) == 0x13) {
        host = (HostSound*)fn_800A384C(owner);
        if (HasCondition((char*)owner + 0x270, 0xb) != 0)
            flag = 1;
    }

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0x159 + slot], flag, 0);
    p->field8c = (void*)fn_801F0ED8;
    p->fielda4 = 0x19;
    held = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(held->field14, held->field1c,
                             ((s32*)lbl_803B9378)[0x159 + slot], 0, 0);
    p->fieldfc = 0x19;

    String name;
    arg.hash = 0x08783452;
    SendMsg((int)owner->target, 0x41e, &arg);

    nameData = *(void**)&name;
    if (nameData != 0)
        PlayerSetHeldSlotIndex(p, 0);
    else
        PlayerSetHeldSlotIndex(p, 1);
}

// 0x801EE37C: hand the message block at 0x26e0 and its owner object (0x26e8)
// to the zoom helper with a flag of 1.
void PlayerZoomOwnerNotify(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;

    fn_80155A84(&p->field26e0, p->field26e8, 1);
}

// 0x801EE3A8: reset the gun timer, clear the zoom bit, mark the player as
// aiming and push the held-slot mode value 0x13.
void PlayerGunZoomSetup(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;

    PlayerResetGunTimer(p);
    p->field638 = 1;
    p->field7b4 &= ~1u;
    p->flags50 |= 0x20000;
    p->field78 = 1;
    PlayerSetValue4(&p->field7a4, 0x13);
}

// 0x801EE404: held-item setup variant: starts the two held sounds with aim id
// 0x59, installs the 0x801F0ED8 callback, asks the owner object for the item
// name and selects the held slot.
void PlayerBeginHeldItemStateAlt(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    OwnerObject* owner;
    HostSound* host;
    HostSound* held;
    void* nameData;
    int hostIndex;
    int slot;
    int flag;
    PlayerMsgArg arg;

    hostIndex = PlayerInHost((Actor*)p);
    flag = 0;
    owner = (OwnerObject*)p->field26e8;
    slot = p->field73c;
    if (((u32)owner->target >> 24) == 0x13) {
        host = (HostSound*)fn_800A384C(owner);
        if (HasCondition((char*)owner + 0x270, 0xb) != 0)
            flag = 1;
    }

    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0x147 + slot], flag, 0);
    p->field8c = (void*)fn_801F0ED8;
    p->fielda4 = 0x59;
    held = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(held->field14, held->field1c,
                             ((s32*)lbl_803B9378)[0x147 + slot], 0, 0);
    p->fieldfc = 0x59;

    String name;
    arg.hash = 0x08783452;
    SendMsg((int)owner->target, 0x41e, &arg);

    nameData = *(void**)&name;
    if (nameData != 0)
        PlayerSetHeldSlotIndex(p, 0);
    else
        PlayerSetHeldSlotIndex(p, 1);
}

// 0x801EE560: clear the 0x40000 state bit on the player.
void PlayerClearState40000(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;

    p->flags50 &= ~0x40000u;
}

// 0x801EE570: clear the gun zoom state and rerun the collision update with the
// same aim factor.
void PlayerHeldSlotSelectUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    PlayerResetGunTimer(p);
    p->field638 = 1;
    p->field7b4 &= ~1u;
    PlayerSelectCollisionUpdate(p, f);
}

// 0x801EE5C8: (re)build the held-item state: two positional sounds from the
// host's sound block, the per-frame callback and the camera FOV.
void PlayerSetupHeldItemState(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    ZoomSource* zoom;
    int hostIndex;
    int slot;
    int bone;

    PlayerResetGunTimer(p);
    p->flags50 |= 0x40000;
    fn_80050EB0(p);
    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0xed + slot], 0, 0);
    p->fielda4 = 0x50;
    if (p->field26b0 == -1) {
        bone = 8;
    } else {
        bone = ModelBoneName((int)p->field58, p->field26b0);
        if (ModelBoneNodeChain((int)p->field58, bone, lbl_8049D1C8, 0x10) == -1)
            bone = 8;
    }
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[0xed + slot], bone << 1, 0);
    p->fielde4 = fn_801F0ED8;
    p->fieldfc = 0x50;
    p->field7e0 = 0;
    zoom = (ZoomSource*)p->field26c8;
    CameraSetFov(p->field184, (float)zoom->field104);
    p->field7b4 &= 0xfeffffffu;
}

// 0x801EE718: per-frame gun/zoom mode update. Toggles the zoom state while the
// player is in an unarmed/aiming stance, applies the FOV for the new state and
// plays zoom_out / zoom_level1 when the state changed this frame.
void PlayerGunZoomModeUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    int prevMode;
    int mode;

    p->field638 = 1;
    p->field7b4 &= ~1u;
    fn_800769A0(p, f);
    PlayerSelectCollisionUpdate(p, f);

    prevMode = p->field7e0;
    if ((p->field328 == 1 || p->field328 == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            goto done;
        }
        PlayerResetGunTimer(p);
        if (p->field7e0 == 0)
            p->field7e0 = 3;
        else
            p->field7e0 = 0;
    }

    mode = p->field7e0;
    if (mode == 3) {
        CameraSetFov(p->field184, kZoomInFov);
    } else if (mode == 0) {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }

    if (p->field7e0 != prevMode) {
        if (prevMode < p->field7e0)
            FxStart(FxFind("zoom_out"), 0, 0);
        else
            FxStart(FxFind("zoom_level1"), 0, 0);
    }
done:
    PlayerSetValue4(&p->field7a4, 3);
}

// 0x801EE8A4: gun/held-item state setup. Sends the two hashed item-name
// queries to the player's message target, runs the held-object effect update,
// then builds both held sounds from the host's sound block and the slot table.
void PlayerSetupGunItemState(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    void* nameA;
    void* nameB;
    int hostIndex;
    int slot;
    int alt;

    {
        ItemNameMsg msg;
        msg.hash = 0x2da8293c;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameA = *(void**)&msg.name;
    }
    {
        ItemNameMsg msg;
        msg.hash = 0xdff3097a;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameB = *(void**)&msg.name;
    }

    fn_80048BA8(p, p->field1d04, 1);
    Fn800F7F5C((int)nameA, (int)nameB, 0xbc78f550, fn_800498D0(p),
               p->field1d04, p->field14fc);

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0xc9 + slot], 0, 0);
    p->field8c = (void*)fn_801F0ED8;
    p->fielda4 = 0x11;
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[0xc9 + slot], 0, 0);
    alt = fn_800A1BC8(host->field14, host->field1c,
                      ((s32*)lbl_803B9378)[0x39 + slot], 0, 0);
    if (p->fieldf0 == alt)
        p->fieldfc = 1;
    else
        p->fieldfc = 0x11;
    p->field7e0 = 0;
    CameraSetFov(p->field184,
                 (float)((ZoomSource*)p->field26c8)->field104);
    p->field7b4 &= 0xfeffffffu;
}

// 0x801EEADC: gun/zoom per-frame update variant without the initial timer
// reset. Toggles the zoom state while the player is in an aiming stance,
// applies the FOV for the new state, plays zoom_out / zoom_level1 when the
// state changed this frame and finally pushes held-slot mode 3.
void PlayerGunZoomSwitchUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    int prevMode;
    int mode;

    p->field638 = 1;
    p->field7b4 &= ~1u;
    fn_800769A0(p, f);
    PlayerSelectCollisionUpdate(p, f);

    prevMode = p->field7e0;

    if ((p->field328 == 1 || p->field328 == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            goto done;
        }
        PlayerResetGunTimer(p);
        if (p->field7e0 == 0)
            p->field7e0 = 3;
        else
            p->field7e0 = 0;
    }

    mode = p->field7e0;
    if (mode == 3) {
        CameraSetFov(p->field184, kZoomInFov);
    } else if (mode == 0) {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }

    if (p->field7e0 != prevMode) {
        if (prevMode < p->field7e0)
            FxStart(FxFind("zoom_out"), 0, 0);
        else
            FxStart(FxFind("zoom_level1"), 0, 0);
    }
done:
    PlayerSetValue4(&p->field7a4, 3);
}

// 0x801EEC68: gun/held-item state setup for the item-in-hand mode. Sends two
// hashed item-name queries to the player's message target, builds the held
// object's effect block, then creates both held sounds from the host sound
// block and the slot table and applies the zoom FOV.
void PlayerSetupGunItemStateAlt(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    void* nameA;
    void* nameB;
    HostSound* host;
    int hostIndex;
    int slot;
    int alt;

    {
        ItemNameMsg msg;
        msg.hash = 0x2da8293c;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameA = *(void**)&msg.name;
    }
    {
        ItemNameMsg msg;
        msg.hash = 0xdff3097a;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameB = *(void**)&msg.name;
    }

    fn_80048BA8(p, p->field1d04, 1);
    Fn800F7F5C((int)nameA, (int)nameB, (int)0x8fdd7599, fn_800498D0(p),
               p->field1d04, p->field14fc);

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0xb7 + slot], 0, 0);
    p->field8c = (void*)fn_801F0ED8;
    p->fielda4 = 0x11;
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[0xb7 + slot], 0, 0);
    alt = fn_800A1BC8(host->field14, host->field1c,
                      ((s32*)lbl_803B9378)[0x39 + slot], 0, 0);
    if (p->fieldf0 == alt)
        p->fieldfc = 1;
    else
        p->fieldfc = 0x11;

    p->field7e0 = 0;
    CameraSetFov(p->field184, (float)((ZoomSource*)p->field26c8)->field104);
    p->field7b4 &= 0xfeffffffu;
}

// 0x801EEEA0: start the zoom/aim animation on the player's model. The helper
// hands back the model id it picked for the current held item.
void PlayerZoomIntroAnim(void* pv)
{
    int model = fn_800498D0(pv);

    ActorPlayAnim(0x02000000, 0xd39f69b4u, model);
}

// 0x801EEED4: per-frame gun/zoom mode update whose held-slot flag ends at 1.
// Starts from the same unarmed/aiming test as 0x801EE718, but on the early exit
// it also drops the zoom back to the base FOV and clears the zoom-active bit.
void PlayerGunZoomAnimUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    int prevMode;

    p->field638 = 1;
    p->field7b4 &= ~1u;
    fn_800769A0(p, f);
    PlayerSelectCollisionUpdate(p, f);

    prevMode = p->field7e0;
    if ((p->field328 == 1 || p->field328 == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            goto done;
        }
        PlayerResetGunTimer(p);
        if (p->field7e0 == 0)
            p->field7e0 = 3;
        else
            p->field7e0 = 0;
    }

    if (p->field7e0 == 3) {
        CameraSetFov(p->field184, kZoomInFov);
    } else if (p->field7e0 == 0) {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }

    if (p->field7e0 != prevMode) {
        if (prevMode < p->field7e0)
            FxStart(FxFind("zoom_out"), 0, 0);
        else
            FxStart(FxFind("zoom_level1"), 0, 0);
    }
done:
    if (p->field7e0 != 0) {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        p->field7e0 = 0;
        CameraSetFov(p->field184, (float)z->field104);
        p->field7b4 &= 0xfeffffffu;
    }
    PlayerSetValue4(&p->field7a4, 1);
}

// 0x801EF0AC: held-item state variant (aim id 0x10): starts the two held
// sounds, installs the 0x801F0ED8 callback, resolves the effect stored in the
// unit's name slot, resets the zoom mode and applies the current FOV.
extern "C" void* fn_800F7994(void* name, u32 hash);
extern "C" void* lbl_804D332C;

void PlayerBeginHeldItemStateAim(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    void* nameData;
    int hostIndex;
    int slot;

    {
        String name;
        PlayerMsgArg arg;
        arg.hash = 0xdff3097a;
        SendMsg((int)p->field38, 0x41e, &arg);
        nameData = *(void**)&name;
    }

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0xa5 + slot], 0, 0);
    p->field8c = (void*)fn_801F0ED8;
    p->fielda4 = 0x10;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[0xa5 + slot], 0, 0);
    p->fieldfc = 0x10;

    lbl_804D332C = fn_800F7994(nameData, 0x9d1c245c);
    FxStart(lbl_804D332C, 0, 0);

    p->field7e0 = 0;
    {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }
    p->field7b4 &= ~0x01000000u;
}

// 0x801EF22C: per-frame gun/zoom update variant: keeps the aim factor across
// the timer and collision calls, pushes held-slot value 3, toggles the zoom
// mode and starts the matching zoom sound when the mode changes.
void PlayerGunZoomUpdateAlt(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    int prevMode;
    int mode;

    p->field638 = 1;
    p->field7b4 &= ~1u;
    fn_800769A0(p, f);
    PlayerSelectCollisionUpdate(p, f);
    PlayerSetValue4(&p->field7a4, 3);

    prevMode = p->field7e0;
    if ((p->field328 == 1 || p->field328 == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            goto done;
        }
        PlayerResetGunTimer(p);
        if (p->field7e0 == 0)
            p->field7e0 = 3;
        else
            p->field7e0 = 0;
    }

    mode = p->field7e0;
    if (mode == 3) {
        CameraSetFov(p->field184, kZoomInFov);
    } else if (mode == 0) {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }

    if (p->field7e0 != prevMode) {
        if (prevMode < p->field7e0)
            FxStart(FxFind("zoom_out"), 0, 0);
        else
            FxStart(FxFind("zoom_level1"), 0, 0);
    }
done:
    ;
}

// 0x801EF3B8: held-item state setup (aim id 0x11): builds the two held sounds
// from the host sound block and the slot table and installs the 0x801F0ED8
// callback.
void PlayerBeginHeldItemStateId11(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    int hostIndex;
    int slot;
    int id;

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    id = ((s32*)lbl_803B9378)[0x93 + slot];
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18, id, 0, 0);
    p->field8c = (void*)fn_801F0ED8;
    p->fielda4 = 0x11;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c, id, 0, 0);
    p->fieldfc = 0x11;
}

// 0x801EF46C: per-frame gun/zoom mode update with the held-slot value 5 pushed
// before the state test. Toggles the zoom state while the player is in an
// unarmed/aiming stance, applies the FOV for the new state, plays zoom_out /
// zoom_level1 when the state changed this frame and finally either pushes
// held-slot value 3 or restarts the gun timer.
void PlayerGunZoomSetupUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    int prevMode;
    int mode;

    p->field638 = 1;
    p->field7b4 &= ~1u;
    fn_800769A0(p, f);
    PlayerSelectCollisionUpdate(p, f);
    PlayerSetValue4(&p->field7a4, 5);

    prevMode = p->field7e0;
    if ((p->field328 == 1 || p->field328 == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            goto done;
        }
        PlayerResetGunTimer(p);
        if (p->field7e0 == 0)
            p->field7e0 = 3;
        else
            p->field7e0 = 0;
    }

    mode = p->field7e0;
    if (mode == 3) {
        CameraSetFov(p->field184, kZoomInFov);
    } else if (mode == 0) {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }

    if (p->field7e0 != prevMode) {
        if (prevMode < p->field7e0)
            FxStart(FxFind("zoom_out"), 0, 0);
        else
            FxStart(FxFind("zoom_level1"), 0, 0);
    }
done:
    if (p->field314 == 0)
        PlayerSetValue4(&p->field7a4, 3);
    else
        PlayerResetGunTimer(p);
}

// 0x801EF61C: held-item state variant (aim id 1): starts the two held sounds
// from the host sound block and the slot table and installs the 0x801F0ED8
// callback. Names are descriptive guesses.
void PlayerBeginHeldItemStateAim1(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    int hostIndex;
    int slot;

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0x5d + slot], 0, 0);
    p->field8c = (void*)fn_801F0ED8;
    p->fielda4 = 1;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[0x5d + slot], 0, 0);
    p->fieldfc = 1;
}

// 0x801EF6D0: per-frame gun/zoom state update for the item-query states.
// Queries both item-name hashes, then updates the zoom mode, the FOV and the
// zoom sounds, and finally runs the follow-up helper unless the held slot was
// switched. Names are descriptive guesses.
void PlayerGunZoomItemQueryUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    float maxAbs;
    int prevMode;
    bool handled;

    {
        ItemNameMsg msg;
        msg.hash = 0x2da8293c;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
    }
    {
        ItemNameMsg msg;
        msg.hash = 0xdff3097a;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
    }

    p->field638 = 1;
    p->field7b4 &= ~1u;
    fn_800769A0(p, f);
    PlayerSelectCollisionUpdate(p, f);
    p->field7d0 = 1;

    maxAbs = fabsf(p->field394);
    if (maxAbs < fabsf(p->field38c))
        maxAbs = fabsf(p->field38c);
    if (kZeroF != maxAbs)
        fn_8008E824(p->field7c, 0);

    if (kZeroF == p->field7d8) {
        PlayerSetHeldSlotMode(p, 0);
        goto done;
    }
    if (p->field678 & 0x80) {
        p->field678 &= ~0x80u;
        PlayerSetActionMode(p, 3);
        goto done;
    }
    PlayerSetValue4(&p->field7a4, 3);
    if (fn_801F0994(p, f) != 0)
        goto done;

    prevMode = p->field7e0;
    handled = false;
    if ((p->field328 == 1 || p->field328 == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            handled = true;
        } else {
            PlayerResetGunTimer(p);
            if (p->field7e0 == 0)
                p->field7e0 = 3;
            else
                p->field7e0 = 0;
        }
    }
    if (!handled) {
        if (p->field7e0 == 3) {
            CameraSetFov(p->field184, kZoomInFov);
        } else if (p->field7e0 == 0) {
            ZoomSource* z = (ZoomSource*)p->field26c8;
            CameraSetFov(p->field184, (float)z->field104);
        }
        if (p->field7e0 != prevMode) {
            if (prevMode < p->field7e0)
                FxStart(FxFind("zoom_out"), 0, 0);
            else
                FxStart(FxFind("zoom_level1"), 0, 0);
        }
        PlayerHeldItemSlotCheck(p, f);
    }
done:
    ;
}

int Fn800F7DEC(int, int, int, int, void*);

// 0x801EF9D8: held-item sound follow-up setup. Queries both item-name hashes,
// asks the follow-up resolver whether this item is handled, then (re)builds the
// held sound, retrying once without the flag if the first lookup failed.
void PlayerSetupGunItemQueryFollow(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    void* nameA;
    void* nameB;
    int hostIndex;
    int slot;
    int flag;
    u32 ret;

    {
        ItemNameMsg msg;
        msg.hash = 0x2da8293c;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameA = *(void**)&msg.name;
    }
    {
        ItemNameMsg msg;
        msg.hash = 0xdff3097a;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameB = *(void**)&msg.name;
    }

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    ret = (u32)Fn800F7DEC((int)nameA, (int)nameB, 0x20ee36,
                          fn_800498D0(p), 0);
    flag = 0;
    if (ret == 0xC0BECF7Eu || ret == 0x11D2C025u)
        flag = 1;

    p->field98 = -1;
    while (p->field98 == -1) {
        host = (HostSound*)(&p->field26c4)[hostIndex];
        p->field98 = fn_800A1BC8(host->field14, host->field18,
                                 ((s32*)lbl_803B9378)[0x39 + slot], flag, 0);
        p->field8c = (void*)fn_801F0ED8;
        p->fielda4 = 1;
        if (p->field98 == -1 && flag != 0)
            flag = 0;
    }

    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[0x39 + slot], 0, 0);
    p->fieldfc = 1;
}

// 0x801EFBCC: restart the held-item animation on the player's host model,
// keyed by the actor's current state value.
void PlayerPlayHeldItemAnim(void* pv)
{
    ActorPlayAnim(0x2000000, 0x8b3a27a3u, fn_800498D0(pv));
}

extern "C" void* fn_800F7994(void* name, u32 hash);
extern "C" void* lbl_804D3328;

// 0x801EFC00: per-frame gun/zoom state update without the initial timer reset.
// Keeps the aim factor across the update and the collision call, toggles the
// zoom state while the player is in an unarmed/aiming stance, applies the FOV
// for the new state, plays zoom_out / zoom_level1 when the state changed this
// frame and finally pushes held-slot value 3. Names are descriptive guesses.
void PlayerGunZoomStateUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    int prevMode;
    int mode;

    p->field638 = 1;
    p->field7b4 &= ~1u;
    fn_800769A0(p, f);
    PlayerSelectCollisionUpdate(p, f);

    prevMode = p->field7e0;
    if ((p->field328 == 1 || p->field328 == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            goto done;
        }
        PlayerResetGunTimer(p);
        if (p->field7e0 == 0)
            p->field7e0 = 3;
        else
            p->field7e0 = 0;
    }

    mode = p->field7e0;
    if (mode == 3) {
        CameraSetFov(p->field184, kZoomInFov);
    } else if (mode == 0) {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }

    if (p->field7e0 != prevMode) {
        if (prevMode < p->field7e0)
            FxStart(FxFind("zoom_out"), 0, 0);
        else
            FxStart(FxFind("zoom_level1"), 0, 0);
    }
done:
    PlayerSetValue4(&p->field7a4, 3);
}

// 0x801EFD8C: held-item zoom state setup. Sends the item-name query to the
// player's message target, builds the two held sounds from the host sound
// block and the slot table, starts the effect stored in the message reply and
// applies the current zoom FOV. Names are descriptive guesses.
void PlayerBeginZoomHeldItemState(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    void* nameData;
    int hostIndex;
    int slot;

    ItemNameMsg msg;
    msg.hash = 0xdff3097a;
    msg.unk8 = 0;
    *(void**)&msg.name = 0;
    SendMsg((int)p->field38, 0x41e, &msg);
    nameData = *(void**)&msg.name;

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0x27 + slot], 0, 0);
    p->fielda4 = 0x10;
    p->field8c = (void*)fn_801F0ED8;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[0x27 + slot], 0, 0);
    p->fieldfc = 0x10;

    lbl_804D3328 = fn_800F7994(nameData, 0xafef562b);
    FxStart(lbl_804D3328, 0, 0);

    p->field7e0 = 0;
    {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }
    p->field7b4 &= ~0x01000000u;
}

// 0x801EFF0C: per-frame zoom tick that also services the held-item callback:
// refresh the gun/zoom flags, let the held-item state update run, then pick
// the zoom FOV and the matching cue.
void PlayerZoomStateTick(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    int prevMode;
    int mode;
    int skip;

    p->field638 = 1;
    p->field7b4 &= ~1u;
    fn_800769A0(p, f);
    PlayerSelectCollisionUpdate(p, f);

    if (p->field678 & 0x80) {
        p->field678 &= ~0x80u;
        PlayerSetActionMode(p, 1);
    }
    PlayerSetValue4(&p->field7a4, 1);
    p->field7d0 = 0;
    if (fn_801F0994(p, f) != 0)
        return;

    mode = p->field328;
    prevMode = p->field7e0;
    skip = 0;
    if ((mode == 1 || mode == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            skip = 1;
        } else {
            PlayerResetGunTimer(p);
            if (p->field7e0 == 0)
                p->field7e0 = 3;
            else
                p->field7e0 = 0;
        }
    }

    if (!skip) {
        mode = p->field7e0;
        if (mode == 3) {
            CameraSetFov(p->field184, kZoomInFov);
        } else if (mode == 0) {
            ZoomSource* z = (ZoomSource*)p->field26c8;
            CameraSetFov(p->field184, (float)z->field104);
        }
        if (p->field7e0 != prevMode) {
            if (prevMode < p->field7e0)
                FxStart(FxFind("zoom_out"), 0, 0);
            else
                FxStart(FxFind("zoom_level1"), 0, 0);
        }
    }

    if (!skip && (p->field320 & 1)) {
        if (p->field7d0 == 0)
            PlayerSetHeldSlotMode(p, 2);
        else
            PlayerSetHeldSlotMode(p, 9);
    }
}

// 0x801F0118: initialise the held-item sounds for the current host and slot,
// install the 0x801F0ED8 per-frame callback and reselect the held slot.
void PlayerStartHeldItemZoom(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    int hostIndex;
    int slot;

    hostIndex = PlayerInHost((Actor*)p);
    slot = p->field73c;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[0x15 + slot], 0, 0);
    p->fielda4 = 1;
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[0x16 + slot], 0, 0);
    p->fielde4 = fn_801F0ED8;
    p->fieldfc = 1;
    p->field7d0 = 0;
}

// 0x801F01D4: restart the gun timer for the player passed in.
void PlayerGunTimerResetState(void* pv)
{
    PlayerResetGunTimer(pv);
}

// 0x801F01F4: per-frame zoom/action update that first forces the held-slot
// mode to 3, then runs the shared zoom state machine (mode toggle, FOV, zoom
// sound) and finally the aim/follow helpers and the action-mode check.
void PlayerGunZoomActionUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    int prevMode;

    p->field638 = 1;
    p->field7b4 &= ~1u;
    p->field7d0 = 1;
    PlayerSetValue4(&p->field7a4, 3);

    prevMode = p->field7e0;

    if ((p->field328 == 1 || p->field328 == 3) &&
        p->field314 != 2 && p->field314 != 1) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            goto done;
        }
        PlayerResetGunTimer(p);
        if (p->field7e0 == 0)
            p->field7e0 = 3;
        else
            p->field7e0 = 0;
    }

    if (p->field7e0 == 3) {
        CameraSetFov(p->field184, kZoomInFov);
    } else if (p->field7e0 == 0) {
        ZoomSource* z = (ZoomSource*)p->field26c8;
        CameraSetFov(p->field184, (float)z->field104);
    }

    if (p->field7e0 != prevMode) {
        if (prevMode < p->field7e0)
            FxStart(FxFind("zoom_out"), 0, 0);
        else
            FxStart(FxFind("zoom_level1"), 0, 0);
    }
done:
    if (fn_801F0994(p, f) == 0)
        PlayerHeldItemSlotCheck(p, f);
    if (kZeroF != p->field394 || kZeroF != p->field38c ||
        kZeroF != p->field390 || kZeroF != p->field398 ||
        p->field328 != 0)
        PlayerSetActionMode(p, 3);
}

// 0x801F03E0: item-name query setup for the held-item state. Resolves the two
// item-name hashes; a recognised item forces variant 2, otherwise the variant
// is picked at random. It then rebuilds both held sounds from the host sound
// block and the shared id table (retrying once with variant 0 if a lookup
// fails) and installs the per-frame held-item callbacks.
void PlayerSetupHeldItemRandomState(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    HostSound* host;
    void* nameA;
    void* nameB;
    int hostIndex;
    u32 ret;

    {
        ItemNameMsg msg;
        msg.hash = 0x2da8293c;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameA = *(void**)&msg.name;
    }
    {
        ItemNameMsg msg;
        msg.hash = 0xdff3097a;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameB = *(void**)&msg.name;
    }

    ret = (u32)Fn800F7DEC((int)nameA, (int)nameB, 0x20ee36,
                          fn_800498D0(p), 0);
    if (ret == 0xC0BECF7Eu || ret == 0x11D2C025u)
        p->field7d4 = 2;
    else if ((float)rand() / lbl_804D8F84 < lbl_804D8F88)
        p->field7d4 = 1;
    else
        p->field7d4 = 0;

    hostIndex = PlayerInHost((Actor*)p);
    host = (HostSound*)(&p->field26c4)[hostIndex];
    p->field98 = fn_800A1BC8(host->field14, host->field18,
                             ((s32*)lbl_803B9378)[3], p->field7d4, 0);
    if (p->field98 == -1) {
        p->field7d4 = 0;
        p->field98 = fn_800A1BC8(host->field14, host->field18,
                                 ((s32*)lbl_803B9378)[3], p->field7d4, 0);
    }
    p->field8c = (void*)fn_801F0ED8;
    p->fielda4 = 1;
    p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                             ((s32*)lbl_803B9378)[3], p->field7d4, 0);
    if (p->fieldf0 == -1) {
        p->field7d4 = 0;
        p->fieldf0 = fn_800A1BC8(host->field14, host->field1c,
                                 ((s32*)lbl_803B9378)[3], p->field7d4, 0);
    }
    p->fielde4 = fn_801F0ED8;
    p->fieldfc = 1;
}

// 0x801F065C: held-item slot check run every frame while the player holds an
// item. It resolves the two item-name hashes, then either reacts to the attack
// button (held-item states 4/5), runs the aim-timer path, or picks the held
// slot mode from the item hash. Returns non-zero when it handled the frame.
int PlayerHeldItemSlotCheck(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    void* nameA;
    void* nameB;
    float v;

    {
        ItemNameMsg msg;
        msg.hash = 0x2da8293c;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameA = *(void**)&msg.name;
    }
    {
        ItemNameMsg msg;
        msg.hash = 0xdff3097a;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameB = *(void**)&msg.name;
    }

    if (p->field588 == 5 || p->field588 == 4) {
        if (p->field320 & 1) {
            PlayerResetGunTimer(p);
            if (p->field7d0 == 0)
                PlayerSetHeldSlotMode(p, 2);
            else
                PlayerSetHeldSlotMode(p, 9);
            return 1;
        }
        if (p->field324 & 1) {
            PlayerResetGunTimer(p);
            if ((u32)Fn800F7DEC((int)nameA, (int)nameB, 0xe6b71bcc,
                                fn_800498D0(p), 0) == 0x4A3E183u) {
                PlayerSetHeldSlotMode(p, 10);
                return 1;
            }
            if ((u32)Fn800F7DEC((int)nameA, (int)nameB, 0x968cbe3d,
                                fn_800498D0(p), 0) == 0x4A3E183u) {
                PlayerSetHeldSlotMode(p, 11);
                return 1;
            }
        }
    } else if (p->field320 == 0) {
        v = fn_800699B8(p);
        if (v > lbl_804D8F90 && v < lbl_804D8F80) {
            if ((u32)Fn800F7DEC((int)nameA, (int)nameB, 0xe6b71bcc,
                                fn_800498D0(p), 0) == 0x4A3E183u) {
                PlayerSetHeldSlotMode(p, 10);
                fn_800699A8(p->field2f4);
                return 1;
            }
            if ((u32)Fn800F7DEC((int)nameA, (int)nameB, 0x968cbe3d,
                                fn_800498D0(p), 0) == 0x4A3E183u) {
                PlayerSetHeldSlotMode(p, 11);
                fn_800699A8(p->field2f4);
                return 1;
            }
        }
        fn_800699A8(p->field2f4);
    } else {
        PlayerResetGunTimer(p);
        p->field5e8 += f;
        if (fn_800699B8(p) == lbl_804D8F8C) {
            if (p->field7d0 == 0)
                PlayerSetHeldSlotMode(p, 2);
            else
                PlayerSetHeldSlotMode(p, 9);
            fn_800699A8(p->field2f4);
            return 1;
        }
    }
    return 0;
}

// 0x801F0994: held-item zoom/aim state machine. Resolves the two item-name
// hashes, accumulates the aim timer while the player is charging, then picks
// the held-slot mode from the item the query returns. Returns non-zero when
// the frame was handled.
int PlayerGunAimStateUpdate(void* pv, float f)
{
    PlayerObj* p = (PlayerObj*)pv;
    void* nameA;
    void* nameB;
    int handled;
    int ret;
    int mode;

    {
        ItemNameMsg msg;
        msg.hash = 0x2da8293c;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameA = *(void**)&msg.name;
    }
    {
        ItemNameMsg msg;
        msg.hash = 0xdff3097a;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameB = *(void**)&msg.name;
    }

    if (p->field314 > 0)
        lbl_804D52A8 += f;

    if (p->field7e0 == 0) {
        handled = 0;
        if (fn_80049940() != 0) {
            if (p->field314 != 0)
                handled = 1;
        } else if (p->field314 != 0) {
            handled = 1;
        }
        if (handled == 1) {
            if (p->field7a4 == 1) {
                PlayerSetHeldSlotMode(p, 2);
                return 1;
            }
            lbl_804D52A8 = lbl_804D8F80;
            PlayerResetGunTimer(p);
            ret = Fn800F7DEC((int)nameA, (int)nameB, 0x20ee36,
                             fn_800498D0(p), 0);
            if (ret == 0x11d2c025) {
                if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                    ItemNameMsg msg;
                    msg.hash = 0x7ffcfd4f;
                    msg.unk8 = 0;
                    *(u32*)&msg.name = 0x91a0cef7;
                    SendMsg((int)p->field38, 0x41d, &msg);
                }
                if (p->field7a4 == 5)
                    PlayerSetHeldSlotMode(p, 3);
            } else if (ret < 0x11d2c025) {
                if (ret == 0x4a3e183) {
                    PlayerSetHeldSlotMode(p, 5);
                    return 1;
                }
                if (ret < 0x4a3e183 && ret == -0x3f413082) {
                    mode = Fn800F7DEC((int)nameA, (int)nameB, 0xe6b71bcc,
                                      fn_800498D0(p), 0);
                    if (mode == 0x4a3e183) {
                        PlayerSetHeldSlotMode(p, 10);
                        return 1;
                    }
                }
            } else if (ret == 0x3da260f7 && p->field7a4 == 5) {
                PlayerSetHeldSlotMode(p, 3);
            }
        } else if (p->field7a4 == 5) {
            lbl_804D52A8 = lbl_804D8F80;
            PlayerSetHeldSlotMode(p, 3);
        }
    } else if ((p->field314 & 1) != 0 || lbl_804D8F94 < lbl_804D52A8) {
        if (p->field7a4 == 1) {
            PlayerSetHeldSlotMode(p, 2);
            return 1;
        }
        lbl_804D52A8 = lbl_804D8F80;
        PlayerResetGunTimer(p);
        ret = Fn800F7DEC((int)nameA, (int)nameB, 0xf829db93,
                         fn_800498D0(p), 0);
        if (ret == 0x11d2c025) {
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                ItemNameMsg msg;
                msg.hash = 0x7ffcfd4f;
                msg.unk8 = 0;
                *(u32*)&msg.name = 0x91a0cef7;
                SendMsg((int)p->field38, 0x41d, &msg);
            }
            if (p->field7a4 == 8)
                PlayerSetHeldSlotMode(p, 3);
        } else if (ret < 0x11d2c025) {
            if (ret == 0x4a3e183) {
                if (p->field7a4 != 8) {
                    PlayerSetHeldSlotMode(p, 8);
                    return 1;
                }
            } else if (ret < 0x4a3e183 && ret == -0x3f413082) {
                mode = Fn800F7DEC((int)nameA, (int)nameB, 0x968cbe3d,
                                  fn_800498D0(p), 0);
                if (mode == 0x4a3e183) {
                    PlayerSetHeldSlotMode(p, 11);
                    return 1;
                }
            }
        } else if (ret == 0x3da260f7 && p->field7a4 == 8) {
            PlayerSetHeldSlotMode(p, 3);
        }
    } else if (p->field7a4 == 8) {
        lbl_804D52A8 = lbl_804D8F80;
        PlayerSetHeldSlotMode(p, 3);
    }

    return 0;
}

// 0x801F0ED8: held-item message handler. `param` is the incoming item hash and
// pv the player. It ignores the message while the player already runs the
// 0xc06 action, resolves the two item-name hashes, then starts the held-item
// effect sequence that matches the hash.
int PlayerItemMessageHandler(int param, void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    void* nameA;
    void* nameB;
    void* fx;
    int out;
    int ret;
    int mode;

    if (fn_800C5210((int)p->field38, 0xc06, &out) == 1)
        return 0;

    {
        ItemNameMsg msg;
        msg.hash = 0x2da8293c;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameA = *(void**)&msg.name;
    }
    {
        ItemNameMsg msg;
        msg.hash = 0xdff3097a;
        msg.unk8 = 0;
        SendMsg((int)p->field38, 0x41e, &msg);
        nameB = *(void**)&msg.name;
    }

    switch (param) {
    case 0xf829db93:
        fn_80048BA8(p, p->field1d04, 0);
        Fn800F7F5C((int)nameA, (int)nameB, 0xf829db93, fn_800498D0(p),
                   p->field1d04, p->field14fc);
        break;
    case 0xe6b71bcc:
        fn_80048BA8(p, p->field1d04, 1);
        Fn800F7F5C((int)nameA, (int)nameB, 0xe6b71bcc, fn_800498D0(p),
                   p->field1d04, p->field14fc);
        break;
    case 0x968cbe3d:
        fn_80048BA8(p, p->field1d04, 1);
        Fn800F7F5C((int)nameA, (int)nameB, 0x968cbe3d, fn_800498D0(p),
                   p->field1d04, p->field14fc);
        break;
    case 0x20ee36:
        if ((u32)Fn800F7DEC((int)nameA, (int)nameB, 0x20ee36,
                            fn_800498D0(p), 0) != 0x4a3e183u)
            break;
        fn_80048BA8(p, p->field1d04, 1);
        p->field1d54 = p->field800;
        Fn800F7F5C((int)nameA, (int)nameB, 0x20ee36, fn_800498D0(p),
                   p->field1d04, p->field14fc);
        break;
    case 0x1b83833f:
        mode = p->field7a4;
        if (mode == 5) {
            ret = Fn800F7DEC((int)nameA, (int)nameB, 0x20ee36,
                             fn_800498D0(p), 0);
            if (ret == 0x4a3e183) {
                PlayerSetActionMode(p, p->field7a8);
                break;
            }
            if (ret == -0x3f413082 || ret == 0x11d2c025) {
                ActorPlayAnim((int)p->field38, 0xa7d124de, (int)p->field38);
                fx = FxFind("gun_empty");
                FxStart(fx, 0, 0);
            }
            PlayerSetHeldSlotMode(p, 3);
            break;
        }
        if (mode != 8) {
            if (mode == 0xd) {
                p->field7b4 |= 0x20000;
                break;
            }
            PlayerSetActionMode(p, p->field7a8);
            break;
        }
        ret = Fn800F7DEC((int)nameA, (int)nameB, 0xf829db93,
                         fn_800498D0(p), 0);
        if (ret == 0x4a3e183) {
            PlayerSetActionMode(p, p->field7a8);
            break;
        }
        if (ret == -0x3f413082 || ret == 0x11d2c025) {
            fx = FxFind("gun_empty");
            FxStart(fx, 0, 0);
        }
        PlayerSetHeldSlotMode(p, 3);
        break;
    default:
        fn_80048BA8(p, p->field1d04, 1);
        Fn800F7F5C((int)nameA, (int)nameB, param, fn_800498D0(p),
                   p->field1d04, p->field14fc);
        break;
    }

    return 0;
}

// 0x801F1348: clear the zoom mode, restore the base FOV and drop the
// zoom-active bit. Names are descriptive guesses.
void PlayerResetZoomFovState(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    ZoomSource* zoom;

    p->field7e0 = 0;
    zoom = (ZoomSource*)p->field26c8;
    CameraSetFov(p->field184, (float)zoom->field104);
    p->field7b4 &= 0xfeffffffu;
}

// 0x801F13B0: same reset as 0x801F1348 on a second state entry. Names are
// descriptive guesses.
void PlayerResetZoomFovStateAlt(void* pv)
{
    PlayerObj* p = (PlayerObj*)pv;
    ZoomSource* zoom;

    p->field7e0 = 0;
    zoom = (ZoomSource*)p->field26c8;
    CameraSetFov(p->field184, (float)zoom->field104);
    p->field7b4 &= 0xfeffffffu;
}

// 0x801F1418: reads the 32-bit field at 0x44 of the 0x48-byte table entry
// selected by the player's held-slot state at 0x7a4. Names are descriptive
// guesses.
struct PlayerSlotEntry {
    u8 unk0[0x8];                       // 0x0
    void (*callback)(void);             // 0x8
    u8 unkC[0x44 - 0xc];                // 0xc
    s32 value44;                        // 0x44
};

s32 PlayerHeldSlotField44(PlayerObj* p)
{
    PlayerSlotEntry* table = (PlayerSlotEntry*)lbl_803B9378;
    return table[p->field7a4].value44;
}

// 0x801F1434: calls the callback at 0x8 of the same table entry, if any.
void PlayerHeldSlotDispatchCallback(PlayerObj* p)
{
    PlayerSlotEntry* entry = &((PlayerSlotEntry*)lbl_803B9378)[p->field7a4];
    void (*callback)(void);

    callback = entry->callback;
    if (callback != 0)
        callback();
}

// 0x801F1478: advances the held-item effect value at 0x800 by the frame delta,
// clamping it between the two state thresholds, then runs the state's update
// callback. Names and constants are descriptive guesses.
extern "C" float lbl_804D8F98;
extern "C" float lbl_804D8F9C;

struct PlayerSlotFxEntry {
    void (*callback0)(void);            // 0x0
    void (*callback4)(void);            // 0x4
    u8 unk8[0x48 - 0x8];                // 0x8
};

void PlayerUpdateHeldEffectDelta(PlayerObj* p, float dt)
{
    float* value = (float*)&p->field800;
    PlayerSlotFxEntry* entry;
    void (*callback)(void);
    float v;

    if (p->field7a4 == 5) {
        if (*value < lbl_804D8F8C)
            *value = lbl_804D8F98 * dt + *value;
        v = *value;
        *value = (v < lbl_804D8F8C) ? v : lbl_804D8F8C;
    } else {
        if (*value > lbl_804D8F80)
            *value = *value - lbl_804D8F9C * dt;
        v = *value;
        *value = (v <= lbl_804D8F80) ? lbl_804D8F80 : v;
    }

    entry = &((PlayerSlotFxEntry*)lbl_803B9378)[p->field7a4];
    callback = entry->callback4;
    if (callback != 0)
        callback();
}

// 0x801F153C: runs the state's first callback from the same 0x48-byte table
// slot, if there is one. Names are descriptive guesses.
void PlayerHeldSlotCallback0(PlayerObj* p)
{
    PlayerSlotFxEntry* entry = &((PlayerSlotFxEntry*)lbl_803B9378)[p->field7a4];
    void (*callback)(void);

    callback = entry->callback0;
    if (callback != 0)
        callback();
}

// Resolves the 16 asset handles the player gun code looks up by name hash.
// (Called from u_8006C604.cpp under the placeholder name, so keep C linkage.)
unsigned long AssetFind(unsigned long group, unsigned long hash);

// LoadPlayerGunAssets: fills the global asset-handle table.
extern "C" void fn_801F157C(void) {
    lbl_8049D1C8[0]  = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_8049D1C8[1]  = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_8049D1C8[2]  = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_8049D1C8[3]  = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_8049D1C8[4]  = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_8049D1C8[5]  = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_8049D1C8[6]  = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_8049D1C8[7]  = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_8049D1C8[8]  = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_8049D1C8[9]  = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_8049D1C8[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_8049D1C8[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_8049D1C8[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_8049D1C8[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_8049D1C8[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_8049D1C8[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

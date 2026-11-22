// u_801BD5A4.cpp (0x801BD5A4-0x801BE008): a registry of behaviour types (6-word entries: hash, init, free, update, msg, params).
// Split from the old u_801AE50C (0x801AE50C-0x801BEC4C). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary evidence (split 2026-11-22): the registry table .data 0x803AB708-0x803AB9D8 with its own empty 8-byte params
// constant (.rodata 0x803486A0), .sdata 0x804D3200-0x804D3208; no float constants. Its .rodata const sits after the
// previous file's string pool (CW puts a file's string pool after its other .rodata), so it is not part of that file.
// The next file has a second registry with a different entry layout (5 words) and its own empty params constant
// (0x803486C0).
// Matching (2026-11-22): OwnerIsFollowing (0x801BD694) has C++ linkage (CW emits an extern "C" function that needs
// implicit destructors only after the next C++ function), and fn_801BD5A4 / fn_801BD62C (no callers) are in
// config.yml force_active.

#include "game/string.h"
#include "game/msg.h"
#include "game/owner.h"

typedef unsigned char u8;
typedef unsigned long u32;

extern "C" int SendMsg(int to, int msg, void* data);
void OwnerSlotRemove(Owner* o);

// The character a registered behaviour type runs on (only the fields used here; see game/owner.h).
struct SveInfo {
    u8 pad00[0xC];
    u32 type;                  // 0x0C: the type the character starts in
};

struct SveOwner {
    u8 pad00[0x38];
    int id;                    // 0x038
    u8 pad3C[0xC4 - 0x3C];
    SveInfo* info;             // 0x0C4
    u8 padC8[0x104 - 0xC8];
    void* update;              // 0x104: the current type's update callback
    u8 pad108[0x120 - 0x108];
    void* msg;                 // 0x120: the current type's message handler
    u8 pad124[4];
    u32 type;                  // 0x128: current registered type (-1: none)
    u8 pad12C[4];
    void* state;               // 0x130
    u8 pad134[0x908 - 0x134];
    u32 variant;               // 0x908
};

// One registered behaviour type (6 words).
struct SveType {
    u32 hash;                          // 0x00
    void (*enter)(SveOwner* o);        // 0x04
    void (*leave)(SveOwner* o);        // 0x08
    void* update;                      // 0x0C
    void* msg;                         // 0x10
    const char* name;                  // 0x14
};

void AiNoOp_801BE004();
void AiNoOp_801BE000();
int AiReturnFalse_801BDFF8();
int AiReturnFalse_801BDFF0();

// Callbacks of the registered types (other units).
struct Actor; struct Creature; struct EventOwner; struct HostObj; struct MsgArg; struct MsgData; struct Rico;
extern "C" {
void fn_801D9A70();
void fn_8029F0BC();
void fn_8029EDCC();
void fn_8029E714();
void fn_800CFB78();
void fn_800CFB38();
void fn_800CF914();
void fn_800CF4B4();
void fn_8011FCEC();
void fn_801B455C();
void fn_801B44E4();
void fn_801B3DE8();
void fn_801B3610();
void fn_8025AD90();
void fn_80259AB0();
void fn_802C0564();
void fn_802C01E8();
void fn_802C9D1C();
void fn_802C9818();
}
void CreatureInit(Creature*);
void CreatureFree(Creature*);
void CreatureUpdate(Creature*);
void FSHSVEBlockInit(Actor*);
void FSHSVEBlockFree(Actor*);
void UpdateFSHSVE(Actor*);
int HandleFSHSVEMessages(int, int*);
void HostRideJInit(Actor*);
void HostRideJFree(Actor*);
void HostRideJTick(void*);
int HostRideJMsg(int, void*);
void HostRideBInit(Actor*);
void HostRideBFree(Actor*);
void HostRideBUpdate(void*);
int HostRideBMsg(int, void*);
void NpcSveInitActor(Owner*);
void NpcSveFreeEvent(Owner*);
void NpcSveUpdateActor(Owner*);
int NpcSveHandleMsg(int, int*);
void HostLeaveRide(HostObj*);
void ActorStateInit(Owner*);
void ActorReleaseState(Owner*);
void UpdateActorState(Owner*);
int HandleQueryActorMsg(int, MsgData*);
void ItemCreate(void*);
void ItemDestroy(void*);
void ItemUpdate(EventOwner*);
void FollowBehaviourInit(void*);
void FollowBehaviourRelease(void*);
void FollowBehaviourTick(void*);
int FollowBehaviourMessage(int, void*);
void AllyCreate(Actor*);
void AllyDestroy(Actor*);
void AllyUpdate(Actor*);
int AllyMsg(int, int*);
void HostInitBehaviour(void*);
void HostDestroyBehaviour(void*);
void HostUpdateBehaviour(void*);
int HostHandleMessage(int, void*);
void ImpInit(void*);
void ImpFree(Actor*);
void ImpTick(void*);
int ImpMsg(int, void*);
void GeistA28A4DogFetchInit(int);
void GeistA28A4StateDetach(int);
void u801A2D90Update(int);
int u801A28A4MsgHandler(int, int*);
void GeistA28A4StateAttach(int);
void FoeCreate(Actor*);
void FoeDestroy(Actor*);
void FoeUpdate(Actor*);
int FoeMsg(int, void*);
void FshSveCreateBehaviour(Actor*);
void FshSveReleaseBehaviour(Actor*);
void fn_802BB8E0(Actor*);
int fn_802BB1F4(int, MsgArg*);
void HostRideDInit(void*);
void HostRideDFree(Actor*);
void HostRideDTick(void*);
int HostRideDMsg(int, int*);
void RicoInit(Rico*);
void RicoDestroy(Rico*);
void ActorInitCollection802B8308(Actor*);
void ActorFreeExtra802B82C8(Actor*);
void ActorStateTick802B7FD0(Actor*);
int ActorMessageHandler802B7120(int, int*);
void RideInit(void*);
void RideFree(Actor*);
void RideUpdate(void*);
int RideMsg(int, void*);
void PossessedStatusInitState(Owner*);
void PossessedStatusFreeState(Owner*);
void HostBehInit(Actor*);
void HostBehFree(Actor*);
void HostBehUpdate(void*);
int HostBehMsg(int, void*);
void FollowInit(Actor*);
void StateBlockFree(Actor*);
void HostRideInit(Actor*);
void HostRideFree(Actor*);
void HostRideTick(void*);
int HostRideMsg(int, void*);
void HostRideCInit(void*);
void HostRideCFree(Actor*);
void HostRideCTick(void*);
int HostRideCMsg(int, void*);
void HostRideAInit(void*);
void HostRideAFree(Actor*);
void HostRideAUpdate(void*);
int HostRideAMsg(int, void*);
void CollectInit(Actor*);
void CollectRelease(Actor*);
void CollectTick(void*);
int CollectMsg(int, void*);

typedef void (*SveFn)(SveOwner*);
// The type table and its entry count.
SveType lbl_803AB708[30] = {
    { 0xE6858762, (SveFn)CreatureInit, (SveFn)CreatureFree, (void*)CreatureUpdate, (void*)fn_801D9A70, "" },
    { 0xEA993A8F, (SveFn)FSHSVEBlockInit, (SveFn)FSHSVEBlockFree, (void*)UpdateFSHSVE, (void*)HandleFSHSVEMessages, "" },
    { 0x045D77C6, (SveFn)HostRideJInit, (SveFn)HostRideJFree, (void*)HostRideJTick, (void*)HostRideJMsg, "" },
    { 0x03C46091, (SveFn)HostRideBInit, (SveFn)HostRideBFree, (void*)HostRideBUpdate, (void*)HostRideBMsg, "" },
    { 0x00012EC1, (SveFn)NpcSveInitActor, (SveFn)NpcSveFreeEvent, (void*)NpcSveUpdateActor, (void*)NpcSveHandleMsg, "" },
    { 0xB1C30D62, (SveFn)fn_8029F0BC, (SveFn)HostLeaveRide, (void*)fn_8029EDCC, (void*)fn_8029E714, "" },
    { 0xC8422A5B, (SveFn)ActorStateInit, (SveFn)ActorReleaseState, (void*)UpdateActorState, (void*)HandleQueryActorMsg, "" },
    { 0x00276198, (SveFn)fn_800CFB78, (SveFn)fn_800CFB38, (void*)fn_800CF914, (void*)fn_800CF4B4, "" },
    { 0x6B9EBFFE, (SveFn)ItemCreate, (SveFn)ItemDestroy, (void*)ItemUpdate, (void*)fn_8011FCEC, "" },
    { 0x518F8F9A, (SveFn)fn_801B455C, (SveFn)fn_801B44E4, (void*)fn_801B3DE8, (void*)fn_801B3610, "" },
    { 0x78562990, (SveFn)FollowBehaviourInit, (SveFn)FollowBehaviourRelease, (void*)FollowBehaviourTick, (void*)FollowBehaviourMessage, "" },
    { 0xAF7258AD, (SveFn)AllyCreate, (SveFn)AllyDestroy, (void*)AllyUpdate, (void*)AllyMsg, "" },
    { 0x4BD66AD8, (SveFn)HostInitBehaviour, (SveFn)HostDestroyBehaviour, (void*)HostUpdateBehaviour, (void*)HostHandleMessage, "" },
    { 0x0D028AC2, (SveFn)ImpInit, (SveFn)ImpFree, (void*)ImpTick, (void*)ImpMsg, "" },
    { 0x0001091C, (SveFn)GeistA28A4DogFetchInit, (SveFn)GeistA28A4StateDetach, (void*)u801A2D90Update, (void*)u801A28A4MsgHandler, "" },
    { 0xA7F71344, (SveFn)GeistA28A4StateAttach, (SveFn)GeistA28A4StateDetach, (void*)u801A2D90Update, (void*)u801A28A4MsgHandler, "" },
    { 0x001FA79E, (SveFn)FoeCreate, (SveFn)FoeDestroy, (void*)FoeUpdate, (void*)FoeMsg, "" },
    { 0x906E8474, (SveFn)FshSveCreateBehaviour, (SveFn)FshSveReleaseBehaviour, (void*)fn_802BB8E0, (void*)fn_802BB1F4, "" },
    { 0x296DB72C, (SveFn)HostRideDInit, (SveFn)HostRideDFree, (void*)HostRideDTick, (void*)HostRideDMsg, "" },
    { 0x7ECFB323, (SveFn)RicoInit, (SveFn)RicoDestroy, (void*)fn_8025AD90, (void*)fn_80259AB0, "" },
    { 0x2C3FD46C, (SveFn)ActorInitCollection802B8308, (SveFn)ActorFreeExtra802B82C8, (void*)ActorStateTick802B7FD0, (void*)ActorMessageHandler802B7120, "" },
    { 0x0E6D87D7, (SveFn)RideInit, (SveFn)RideFree, (void*)RideUpdate, (void*)RideMsg, "" },
    { 0xB79B4F12, (SveFn)PossessedStatusInitState, (SveFn)PossessedStatusFreeState, (void*)fn_802C0564, (void*)fn_802C01E8, "" },
    { 0x208C8734, (SveFn)HostBehInit, (SveFn)HostBehFree, (void*)HostBehUpdate, (void*)HostBehMsg, "" },
    { 0x3DCD3AD9, (SveFn)FollowInit, (SveFn)StateBlockFree, (void*)fn_802C9D1C, (void*)fn_802C9818, "" },
    { 0x7DE7AEAE, (SveFn)HostRideInit, (SveFn)HostRideFree, (void*)HostRideTick, (void*)HostRideMsg, "" },
    { 0xDB3A08C6, (SveFn)HostRideCInit, (SveFn)HostRideCFree, (void*)HostRideCTick, (void*)HostRideCMsg, "" },
    { 0xDB630C2B, (SveFn)HostRideAInit, (SveFn)HostRideAFree, (void*)HostRideAUpdate, (void*)HostRideAMsg, "" },
    { 0x8CFF50E3, (SveFn)CollectInit, (SveFn)CollectRelease, (void*)CollectTick, (void*)CollectMsg, "" },
    { 0x0024A738, 0, 0, 0, 0, "" },
};
int lbl_804D3200 = 30;

static inline int SveFind(u32 hash) {
    int i;
    for (i = 0; i < lbl_804D3200; i++) {
        if (hash == lbl_803AB708[i].hash) {
            return i;
        }
    }
    return -1;
}

static inline void SveLeave(SveOwner* o) {
    if (o != 0) {
        int i = SveFind(o->type);
        if (i != -1) {
            if (lbl_803AB708[i].leave) {
                lbl_803AB708[i].leave(o);
            }
            OwnerSlotRemove((Owner*)o);
            o->update = 0;
            o->state = 0;
            o->msg = 0;
            o->type = -1;
        } else {
            o->type = -1;
        }
    }
}

static inline void SveEnter(u32 hash, void* p) {
    if (p != 0) {
        SveOwner* o = (SveOwner*)p;
        int i = SveFind(hash);
        if (i != -1) {
            SveLeave(o);
            o->update = lbl_803AB708[i].update;
            o->msg = lbl_803AB708[i].msg;
            o->type = hash;
            o->state = 0;
            if (lbl_803AB708[i].enter) {
                lbl_803AB708[i].enter(o);
            }
        }
    }
}

// Makes a registered type do nothing.
extern "C" void fn_801BD5A4(u32 hash) {
    int i = SveFind(hash);
    if (i >= 0) {
        lbl_803AB708[i].enter = (void (*)(SveOwner*))AiNoOp_801BE004;
        lbl_803AB708[i].leave = (void (*)(SveOwner*))AiNoOp_801BE000;
        lbl_803AB708[i].update = (void*)AiReturnFalse_801BDFF8;
        lbl_803AB708[i].msg = (void*)AiReturnFalse_801BDFF0;
    }
}

// Replaces the callbacks of a registered type.
extern "C" void fn_801BD62C(u32 hash, void (*enter)(SveOwner*), void (*leave)(SveOwner*), void* update,
                            void* msg) {
    int i = SveFind(hash);
    if (i >= 0) {
        lbl_803AB708[i].enter = enter;
        lbl_803AB708[i].leave = leave;
        lbl_803AB708[i].update = update;
        lbl_803AB708[i].msg = msg;
    }
}

// Asks a character for its 0x8132F1B1 value (0 if it does not answer): the follow behaviours (u_8023824C)
// answer 1, hence OwnerIsFollowing below.
inline int QuerySend(int* out, int target, u32 hash) {
    QueryMsg m;
    int sent;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    sent = SendMsg(target, 0x41E, &m);
    *out = *(int*)&m.result;
    return sent;
}

int OwnerIsFollowing(void* p) {
    SveOwner* o = (SveOwner*)p;
    int value;
    int sent;

    if (o == 0) {
        return 0;
    }
    sent = QuerySend(&value, o->id, 0x8132F1B1);
    return sent != 0 ? value : 0;
}

// The name of a registered type ("" if unknown).
extern "C" String fn_801BD74C(u32 hash) {
    int i = SveFind(hash);
    if (i != -1) {
        return String(lbl_803AB708[i].name);
    }
    return String("");
}

// Starts the character's own type. Type 0x12EC1 starts 0xC8422A5B or 0xB1C30D62 instead (by the variant
// word), unless the character already runs one of the three.
extern "C" void fn_801BD7D4(SveOwner* o) {
    u32 type;

    if (o == 0) {
        return;
    }
    type = o->info->type;
    if (type == 0x12EC1 && o->type != 0x12EC1 && o->type != 0xB1C30D62 && o->type != 0xC8422A5B) {
        if (o->variant == 0x8CD18EE1) {
            SveEnter(0xC8422A5B, o);
        } else {
            SveEnter(0xB1C30D62, o);
        }
        return;
    }
    SveEnter(type, o);
}

// True when the character does not run its own type.
extern "C" int fn_801BDC0C(SveOwner* o) {
    if (o == 0) {
        return 0;
    }
    return o->type != o->info->type;
}

// Switches a character to a registered type.
extern "C" void fn_801BDC3C(u32 hash, SveOwner* o) {
    SveEnter(hash, o);
}

// Leaves the character's current type.
extern "C" void fn_801BDDA4(SveOwner* o) {
    SveLeave(o);
}

// Switches type without resetting the state word.
extern "C" void fn_801BDE6C(u32 hash, SveOwner* o) {
    int i;

    if (o == 0) {
        return;
    }
    if (o->type != 0xFFFFFFFF) {
        SveLeave(o);
        o->type = -1;
    }
    i = SveFind(hash);
    if (i != -1) {
        if (lbl_803AB708[i].enter) {
            lbl_803AB708[i].enter(o);
        }
        o->update = lbl_803AB708[i].update;
        o->msg = lbl_803AB708[i].msg;
        o->type = hash;
    }
}

int AiReturnFalse_801BDFF0() {
    return 0;
}

int AiReturnFalse_801BDFF8() {
    return 0;
}

void AiNoOp_801BE000() {
}

void AiNoOp_801BE004() {
}

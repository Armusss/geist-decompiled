// u_80192AC4.cpp (0x80192AC4-0x801965FC): the imp actor's state machine. Split from u_801920B8.
// Evidence: u_801920B8's own .sdata2 pool (0.0f at 0x804D8638, int->float double at 0x804D8648) is used only
// up to the IMP MELEE effect type (descriptor lbl_803A8FA8 -> 0x801920B8..0x80192994); this file has its own
// copies (0.0f at 0x804D8658, double at 0x804D8668). Its 0x708-byte state table lbl_803A8FD8 (25 states of
// 0x48 bytes, the layout of u_80187370's gPlayerStates) points only into 0x80192AC4-0x80195A5C, and the
// behaviour-node hooks fn_801962FC..fn_801963A0 read it too. fn_801963E0 fills .bss lbl_80495CA8.
// Most functions are byte-for-byte twins of u_80187370's player-state functions; shared types come from there.
// Names are descriptive guesses; the code is matched against the retail DOL.


#include <dolphin/types.h>
#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

struct Actor;

struct HostModel {
    u8 pad00[0x14];
    int model;            // 0x14
    int animSet;          // 0x18
    int animSet2;         // 0x1C
};

// A world object the player is attached to (only the fields used here).
struct LinkObj {
    u8 pad00[0x38];
    u32 handle;           // 0x38
    u8 pad3C[0x48];
    struct { u8 pad[0x9C]; int anim; }* info;  // 0x84
    u8 pad88[0x1E8];
    u8 conds[4];          // 0x270
};

// The player object (only the fields used here).
struct Player {
    u8 pad00[0x38];
    int handle;           // 0x38
    u8 pad3C[0x14];
    u32 flags50;          // 0x50
    u8 pad54[0x4];
    int model;            // 0x58
    u8 pad5C[0x1C];
    int unk78;            // 0x78
    u8 pad7C[0x10];
    void* animDone;       // 0x8C
    u8 pad90[0x8];
    int anim;             // 0x98
    u8 pad9C[0x8];
    int animLoop;         // 0xA4
    u8 padA8[0x1C];
    int animC4;           // 0xC4
    u8 padC8[0x1C];
    void* animDone2;      // 0xE4
    u8 padE8[0x8];
    int anim2;            // 0xF0
    u8 padF4[0x8];
    int animLoop2;        // 0xFC
    u8 pad100[0x1C];
    int anim11C;          // 0x11C
    u8 pad120[0x128];
    int unk248;           // 0x248
    u8 pad24C[0xC8];
    int unk314;           // 0x314
    int unk318;           // 0x318
    u8 pad31C[0xC];
    int unk328;           // 0x328
    u8 pad32C[0x60];
    float unk38C;         // 0x38C
    float unk390;         // 0x390
    float unk394;         // 0x394
    float unk398;         // 0x398
    u8 pad39C[0x29C];
    int unk638;           // 0x638
    u8 pad63C[0x3C];
    int unk678;           // 0x678
    u8 pad67C[0x8];
    float unk684;         // 0x684
    u8 pad688[0xB4];
    int animVariant;      // 0x73C
    u8 pad740[0xC];
    int unk74C;           // 0x74C
    int unk750;           // 0x750
    u8 pad754[0x50];
    int state;            // 0x7A4
    int unk7A8;           // 0x7A8
    u8 pad7AC[0x8];
    u32 flags7B4;         // 0x7B4
    u8 pad7B8[0x18];
    int unk7D0;           // 0x7D0
    int unk7D4;           // 0x7D4
    float unk7D8;         // 0x7D8
    int unk7DC;           // 0x7DC
    u8 pad7E0[0x10];
    int unk7F0;           // 0x7F0
    u8 pad7F4[0x14];
    int unk808;           // 0x808
    u8 pad80C[0x1D0];
    int unk9DC;           // 0x9DC
    u8 pad9E0[0xB1C];
    float unk14FC;        // 0x14FC
    u8 pad1500[0x7D4];
    float unk1CD4;        // 0x1CD4
    u8 pad1CD8[0x2C];
    u8 unk1D04[4];        // 0x1D04
    u8 pad1D08[0x964];
    float unk266C;        // 0x266C
    u8 pad2670[0x40];
    int bone;             // 0x26B0
    u8 pad26B4[0x10];
    HostModel* hosts[2];  // 0x26C4
    u8 pad26CC[0x14];
    u8 unk26E0[4];        // 0x26E0
    LinkObj* unk26E4;     // 0x26E4
    LinkObj* link;        // 0x26E8
    u8 pad26EC[0xB0];
    int unk279C;          // 0x279C
};

struct PlayerState {
    void (*enter)(Player* p);                 // 0x00
    void (*update)(Player* p, int arg);       // 0x04 (the hook slot type of 0x80394168: (object, int))
    void (*exit)(Player* p);                  // 0x08
    u32 anims[14];                            // 0x0C
    u32 unk44;                                // 0x44
};

void PlayerResetGunTimer(void* p);
void PlayerSelectCollisionUpdate(void* p, float dt);
void PlayerSetValue4(void* p, int v);
void PlayerUpdateMotionInput(float dt, void* p);
int PlayerInHost(Actor* a);
void HostAction(Actor* a);
int ModelBoneName(int model, int bone);
int ModelBoneNodeChain(int model, int name, int* out, int max);
int HasCondition(void* conds, int id);
void ActorPlayAnim(int handle, unsigned long anim, int a);
int AssetFind(unsigned long bank, unsigned long name);
void PlayerSetActionMode(void* p, int mode);
void PlayerSetHeldSlotIndex(void* p, int i);
void WeatherSetRainB(float rain);
void HostTimerReset();
void HostCreateEmitters(Actor* a);
int PlayerGetHeldSlotIndex(void* p);
void PlayerSetHeldSlotMode(void* p, int mode);
int Fn800F7DEC(int a, int b, int msg, int who, void* data);
void WeatherSetByTypeId(int id, int a, int type);
void PlayerApplyFrameOffset(Player* p);
void PlayerStartGunCharge(void* p);
void Fn800F7F5C(int a, int b, int msg, int who, void* data, float f);

extern "C" {
void fn_80155A84(void* a, int b, int c);
int fn_800498D0(Player* p);
void fn_800769A0(Player* p, float dt);
void fn_80074D68(Player* p, float dt);
int fn_800A1BC8(int model, int set, u32 anim, int a, int b);
int fn_80175EFC(int slot);
int fn_80195F5C(int msg, void* data);  // the imp action-message handler (defined below)
int fn_800C5210(int handle, int msg, void* data);
HostModel* fn_800A384C(LinkObj* obj);
int SendMsg(int to, int msg, void* data);
int fn_802A1D04();
int fn_80049940();
void fn_80050EB0(Player* p);
void fn_800F7AF0(int a, int b);
int rand();
int fn_800F7994(int a, u32 b);
void FxStart(int fx, int a, int b);
extern int lbl_804D3050;
extern int lbl_804D3054;
extern float lbl_804D50E4;
int fn_80212730(int handle);
void fn_80048BA8(Player* p, void* buf, int a);

extern PlayerState lbl_803A8FD8[25];  // the imp state table (25 states of 0x48 bytes)
extern int lbl_804D3070;
extern int lbl_80495CA8[16];
extern int lbl_804D3074;

// Sends a 0x41E name query to the target (the reply is not used).
inline void QuerySend(int target, unsigned long hash) {
    QueryMsg q;

    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    SendMsg(target, 0x41E, &q);
}

// Movement step shared by the state updates.
inline void PlayerMove(Player* p, float dt) {
    fn_800769A0(p, dt);
    if (p->unk750 == 1) {
        fn_80074D68(p, dt);
    } else if (p->unk74C == 1) {
        PlayerUpdateMotionInput(dt, p);
    } else {
        PlayerSelectCollisionUpdate(p, dt);
    }
}

// Fire / charge input shared by the state updates.
inline void PlayerGunInput(Player* p) {
    if (fn_802A1D04() == 0) {
        if (p->unk750 != 1) {
            if (p->unk74C == 1) {
                if (p->unk328 & 1) {
                    PlayerApplyFrameOffset(p);
                }
            } else if ((p->unk328 & 1) && !(p->flags7B4 & 0x08000000) && !(p->flags7B4 & 0x40000) &&
                       !(p->flags7B4 & 0x80000) && fn_80212730(p->handle) == -1) {
                PlayerStartGunCharge(p);
            }
        }
    } else if (p->unk328 != 0 && !(p->flags7B4 & 0x100000) && p->unk266C > 1250.0f) {
        if (p->flags7B4 & 0x800000) {
            p->flags7B4 &= ~0x100000;
        } else {
            p->flags7B4 |= 0x100000;
            PlayerSetActionMode(p, 0x10);
        }
    } else if (p->unk266C <= 0.0f || p->unk328 == 0) {
        p->flags7B4 &= ~0x100000;
    }
}

// Asks the target's 0x41E handler for the value stored under a name hash.
inline void QueryNameOut(int* out, int target, unsigned long hash) {
    QueryMsg q;

    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    SendMsg(target, 0x41E, &q);
    *out = *(int*)&q.result;
}

inline u32* PlayerStateAnim(Player* p, int state) {
    return &lbl_803A8FD8[state].anims[p->animVariant];
}




// Twin of linkObjSys HostRideStateA (Player fields).
void fn_80192AC4(Player* p, float dt) {
    p->unk638 = 0;
    p->flags7B4 &= ~1;
    fn_800769A0(p, dt);
    PlayerSelectCollisionUpdate(p, dt);
    if (p->animC4 == -1) {
        if (p->unk7D0 != 0) {
            PlayerSetValue4(&p->state, 3);
        } else {
            PlayerSetValue4(&p->state, 1);
        }
    } else {
        PlayerSetValue4(&p->state, 22);
    }
}

// ported from u_80187370 fn_8018766C; renamed: 
void fn_80192B60(Player* p) {
    p->animDone = 0;
    if (p->animC4 != -1) {
        p->anim = p->animC4;
    }
    if (p->anim11C != -1) {
        p->anim2 = p->anim11C;
    }
}





// Twin of linkObjSys HostRideStateB (Player fields).
void fn_80192B8C(Player* p) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerSetValue4(&p->state, 21);
    if (p->unk279C == -1 || fn_80175EFC(p->unk279C) != 0) {
        PlayerSetValue4(&p->state, 3);
        fn_80155A84(p->unk26E0, (int)p->link, 1);
    }
}

// Twin of linkObjSys HostRideEnterA (Player fields).
void fn_80192C0C(Player* p) {
    LinkObj* link = p->link;
    HostModel* m;
    int blend = 0;
    u32* anim;

    if ((int)(link->handle >> 24) == 0x13) {
        m = fn_800A384C(link);
        if (HasCondition(link->conds, 11)) {
            blend = 1;
        }
    }
    PlayerInHost((Actor*)p);
    anim = PlayerStateAnim(p, 21);
    p->anim = fn_800A1BC8(m->model, m->animSet, *anim, blend, 0);
    p->animDone = (void*)fn_80195F5C;
    p->animLoop = 0x19;
    p->anim2 = fn_800A1BC8(m->model, m->animSet2, *anim, blend, 0);
    p->animLoop2 = 0x19;
    PlayerSetValue4(&p->state, 21);
}

// TODO (not drafted yet, 0xD0 bytes at 0x80192D08)

// ported from u_80127D80 AiState8012928C__FP5Actor; renamed: lbl_8039F7C8->lbl_803A8FD8, fn_8012B4C8->fn_80195F5C
void fn_80192DD8(Player* p) {
    int idx = PlayerInHost((Actor*)p);
    int v = p->animVariant;
    p->unk78 = 0;
    p->flags50 &= ~0x20000;
    p->anim = fn_800A1BC8(p->hosts[idx]->model, p->hosts[idx]->animSet, lbl_803A8FD8[20].anims[v], 0, 0);
    p->animDone = (void*)fn_80195F5C;
    p->animLoop = 1;
    p->anim2 = fn_800A1BC8(p->hosts[idx]->model, p->hosts[idx]->animSet2, lbl_803A8FD8[20].anims[v], 0, 0);
    p->animLoop2 = 1;
    QueryMsg m;
    m.hash = 0x8783452;
    SendMsg(p->unk26E4->handle, 0x41E, &m);
    if (*(int*)&m.result != 0) {
        PlayerSetHeldSlotIndex(p, 0);
        if (p->anim == -1) {
            PlayerSetActionMode(p, 1);
        }
    } else {
        PlayerSetHeldSlotIndex(p, 1);
        if (p->anim == -1) {
            PlayerSetActionMode(p, 3);
        }
    }
}

// ported from u_80187370 fn_80187E24; renamed: 
void fn_80192F48(Player* p) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    if (p->flags7B4 & 1) {
        p->flags7B4 &= ~1;
    } else {
        p->flags50 |= 0x20000;
        p->unk78 = 1;
    }
}



// ported from u_80187370 fn_80187E80; renamed: gPlayerStates->lbl_803A8FD8, fn_8018ABFC->fn_80195F5C
void fn_80192FA4(Player* p) {
    int blend = 0;
    LinkObj* link = p->link;
    HostModel* m;
    int host;
    u32* anim;

    if ((int)(link->handle >> 24) == 0x13) {
        m = fn_800A384C(link);
        if (HasCondition(link->conds, 11)) {
            blend = 1;
        }
    }
    host = PlayerInHost((Actor*)p);
    anim = PlayerStateAnim(p, 19);
    p->anim = fn_800A1BC8(m->model, m->animSet, *anim, blend, 0);
    p->animDone = (void*)fn_80195F5C;
    p->animLoop = 0x19;
    p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
    p->animLoop2 = 0x19;
    {
        QueryMsg q;
        q.hash = 0x08783452;
        SendMsg(p->link->handle, 0x41E, &q);
        if (*(int*)&q.result != 0) {
            PlayerSetHeldSlotIndex(p, 0);
        } else {
            PlayerSetHeldSlotIndex(p, 1);
        }
    }
}

// ported from u_80187370 fn_80187FE0; renamed: 
void fn_80193104(Player* p) {
    fn_80155A84(p->unk26E0, (int)p->link, 1);
}

// ported from u_80187370 fn_8018800C; renamed: 
void fn_80193130(Player* p) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    p->flags50 |= 0x20000;
    p->unk78 = 1;
    PlayerSetValue4(&p->state, 0x13);
}



// ported from u_80187370 fn_80188068; renamed: gPlayerStates->lbl_803A8FD8, fn_8018ABFC->fn_80195F5C
void fn_8019318C(Player* p) {
    int host = PlayerInHost((Actor*)p);
    LinkObj* link = p->link;
    int variant = p->animVariant;
    int blend = 0;
    HostModel* m;
    u32* anim;

    if ((int)(link->handle >> 24) == 0x13) {
        m = fn_800A384C(link);
        if (HasCondition(link->conds, 11)) {
            blend = 1;
        }
    }
    anim = &lbl_803A8FD8[18].anims[variant];
    p->anim = fn_800A1BC8(m->model, m->animSet, *anim, blend, 0);
    p->animDone = (void*)fn_80195F5C;
    p->animLoop = 0x59;
    p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
    p->animLoop2 = 0x59;
    {
        QueryMsg q;
        q.hash = 0x08783452;
        SendMsg(p->link->handle, 0x41E, &q);
        if (*(int*)&q.result != 0) {
            PlayerSetHeldSlotIndex(p, 0);
        } else {
            PlayerSetHeldSlotIndex(p, 1);
        }
    }
}

// ported from u_80187370 fn_80187370; renamed: 
void fn_801932E8(Player* p) {
    p->flags50 &= ~0x40000;
}

// ported from u_80187370 fn_80187380; renamed: 
void fn_801932F8(Player* p, float dt) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(p, dt);
}

// TODO (not drafted yet, 0x1EC bytes at 0x80193350)

// TODO (not drafted yet, 0x9C bytes at 0x8019353C)

// TODO (not drafted yet, 0x1B8 bytes at 0x801935D8)

// TODO (not drafted yet, 0x9C bytes at 0x80193790)

// TODO (not drafted yet, 0x1B8 bytes at 0x8019382C)

// ported from u_80187370 fn_801890F0; renamed: 
void fn_801939E4(Player* p) {
    ActorPlayAnim(0x02000000, 0xD39F69B4, fn_800498D0(p));
}

// TODO (not drafted yet, 0x9C bytes at 0x80193A18)

// ported from u_80127D80 AiState80129F88__FP5Actor; renamed: lbl_8039F7C8->lbl_803A8FD8, fn_8012B4C8->fn_80195F5C, lbl_804D2E44->lbl_804D3074
void fn_80193AB4(Player* p) {
    int id;
    QueryNameOut(&id, p->handle, 0xDFF3097A);
    int idx = PlayerInHost((Actor*)p);
    int v = p->animVariant;
    p->anim = fn_800A1BC8(p->hosts[idx]->model, p->hosts[idx]->animSet, lbl_803A8FD8[9].anims[v], 0, 0);
    p->animDone = (void*)fn_80195F5C;
    p->animLoop = 0x10;
    p->anim2 = fn_800A1BC8(p->hosts[idx]->model, p->hosts[idx]->animSet2, lbl_803A8FD8[9].anims[v], 0, 0);
    p->animLoop2 = 0x10;
    lbl_804D3074 = fn_800F7994(id, 0x9D1C245C);
    FxStart(lbl_804D3074, 0, 0);
}

// TODO (not drafted yet, 0x128 bytes at 0x80193BF4)

// TODO (not drafted yet, 0x144 bytes at 0x80193D1C)

// TODO (not drafted yet, 0x1A8 bytes at 0x80193E60)

// TODO (not drafted yet, 0x9C bytes at 0x80194008)

// TODO (not drafted yet, 0x1A4 bytes at 0x801940A4)

// TODO (not drafted yet, 0x1FC bytes at 0x80194248)

// ported from u_80127D80 AiState801356E8__FP5Actor; renamed: lbl_803A00D0->lbl_803A8FD8, PlayerGunAction->fn_80195F5C
void fn_80194444(Player* p) {
    int idx = PlayerInHost((Actor*)p);
    int v = p->animVariant;
    p->anim = fn_800A1BC8(p->hosts[idx]->model, p->hosts[idx]->animSet, lbl_803A8FD8[5].anims[v], 0, 0);
    p->animDone = (void*)fn_80195F5C;
    p->animLoop = 1;
    p->anim2 = fn_800A1BC8(p->hosts[idx]->model, p->hosts[idx]->animSet2, lbl_803A8FD8[5].anims[v], 0, 0);
    p->animLoop2 = 1;
}

// TODO (not drafted yet, 0x138 bytes at 0x801944F8)

// TODO (not drafted yet, 0x9C bytes at 0x80194630)

// TODO (not drafted yet, 0xB4 bytes at 0x801946CC)

// TODO (not drafted yet, 0x604 bytes at 0x80194780)

// ported from u_801CDFBC PlayerRatSlotHandleSetup__FP6Player; renamed: lbl_803ACC68->lbl_803A8FD8, PlayerRatModeCallback->fn_80195F5C
void fn_80194D84(Player* p) {
    int index;
    int first;
    int second;

    QueryNameOut(&first, p->handle, 0x2da8293c);
    QueryNameOut(&second, p->handle, 0xdff3097a);

    int host = PlayerInHost((Actor*)p);
    index = p->animVariant;
    int found = Fn800F7DEC(first, second, 0x20ee36, fn_800498D0(p), 0);
    int used = 0;

    if (found == -0x3F413082 || found == 0x11D2C025)
        used = 1;

    p->anim = -1;
    while (p->anim == -1)
    {
        p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet,
                                       lbl_803A8FD8[3].anims[index], used, 0);
        p->animDone = (void*)fn_80195F5C;
        p->animLoop = 1;
        if (p->anim == -1 && used)
            used = 0;
    }

    HostModel* slot = p->hosts[host];
    p->anim2 = fn_800A1BC8(slot->model, slot->animSet2, lbl_803A8FD8[3].anims[index], 0, 0);
    p->animLoop2 = 1;
}

// ported from u_80187370 fn_80189F0C; renamed: 
void fn_80194F78(Player* p) {
    ActorPlayAnim(0x02000000, 0x8B3A27A3, fn_800498D0(p));
}

// TODO (not drafted yet, 0x9C bytes at 0x80194FAC)

// ported from u_80127D80 AiState8012A6E4__FP5Actor; renamed: lbl_8039F7C8->lbl_803A8FD8, fn_8012B4C8->fn_80195F5C, lbl_804D2E40->lbl_804D3070
void fn_80195048(Player* p) {
    int id;
    QueryNameOut(&id, p->handle, 0xDFF3097A);
    int idx = PlayerInHost((Actor*)p);
    int v = p->animVariant;
    p->anim = fn_800A1BC8(p->hosts[idx]->model, p->hosts[idx]->animSet, lbl_803A8FD8[2].anims[v], 0, 0);
    p->animLoop = 0x10;
    p->animDone = (void*)fn_80195F5C;
    p->anim2 = fn_800A1BC8(p->hosts[idx]->model, p->hosts[idx]->animSet2, lbl_803A8FD8[2].anims[v], 0, 0);
    p->animLoop2 = 0x10;
    lbl_804D3070 = fn_800F7994(id, 0xAFEF562B);
    FxStart(lbl_804D3070, 0, 0);
}

// TODO (not drafted yet, 0x34C bytes at 0x80195188)

// ported from u_80136884 PlayerGunStartSlotState__FPv; renamed: lbl_803A07D8->lbl_803A8FD8, PlayerGunActionMsg->fn_80195F5C
void fn_801954D4(Player* p) {
    int slot = PlayerInHost((Actor*)p);
    int kind = p->animVariant;

    p->anim = fn_800A1BC8(p->hosts[slot]->model, p->hosts[slot]->animSet,
                             lbl_803A8FD8[1].anims[kind], 0, 0);
    p->animLoop = 1;
    p->anim2 = fn_800A1BC8(p->hosts[slot]->model, p->hosts[slot]->animSet2,
                             lbl_803A8FD8[1].anims[kind], 0, 0);
    p->animDone2 = (void*)fn_80195F5C;
    p->animLoop2 = 1;
    p->unk7D0 = 0;
}

// ported from u_80187370 fn_8018A6C0; renamed: 
void fn_80195590(Player* p) {
    PlayerResetGunTimer(p);
}

// TODO (not drafted yet, 0x4AC bytes at 0x801955B0)

// TODO (not drafted yet, 0x284 bytes at 0x80195A5C)

// TODO (not drafted yet, 0x27C bytes at 0x80195CE0)

// TODO (not drafted yet, 0x3A0 bytes at 0x80195F5C)

// ported from u_80187370 fn_8018B000; renamed: gPlayerStates->lbl_803A8FD8
u32 fn_801962FC(Player* p) {
    return lbl_803A8FD8[p->state].unk44;
}

// ported from u_80187370 fn_8018B01C; renamed: gPlayerStates->lbl_803A8FD8
void fn_80196318(Player* p) {
    if (lbl_803A8FD8[p->state].exit != 0) {
        lbl_803A8FD8[p->state].exit(p);
    }
}

// ported from u_80160188 WeaponNodeFn1__FP6Weaponi; renamed: sWeaponNodes->lbl_803A8FD8
void fn_8019635C(Player* p, int arg) {
    if (lbl_803A8FD8[p->state].update) {
        lbl_803A8FD8[p->state].update(p, arg);
    }
}

// ported from u_80187370 fn_8018B0C0; renamed: gPlayerStates->lbl_803A8FD8
void fn_801963A0(Player* p) {
    if (lbl_803A8FD8[p->state].enter != 0) {
        lbl_803A8FD8[p->state].enter(p);
    }
}

// ported from u_80187370 fn_8018B100; renamed: lbl_80495890->lbl_80495CA8
void fn_801963E0() {
    lbl_80495CA8[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_80495CA8[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_80495CA8[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_80495CA8[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_80495CA8[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_80495CA8[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_80495CA8[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_80495CA8[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_80495CA8[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_80495CA8[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_80495CA8[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_80495CA8[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_80495CA8[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_80495CA8[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_80495CA8[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_80495CA8[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

} // extern "C"

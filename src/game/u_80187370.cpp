// u_80187370.cpp (0x80187370-0x8018B31C), split from u_8017D794.
// Large behaviour table file: 0x803A8800 (0x708 bytes of state functions/hashes) points only into
// 0x80187370-0x8018B31C; .bss 0x80495890, .sdata 0x804D3050/0x804D3054, .sbss 0x804D50E0-0x804D50F0,
// sdata2 0x804D8418-0x804D8440 (own double 0x804D8438).
// Names are descriptive guesses; the code is matched against the retail DOL.
//
// The table at 0x803A8800 is 25 player states of 0x48 bytes: {enter, update, exit, 14 animation name hashes,
// a final word}. fn_8018B0C0 / fn_8018B060 / fn_8018B01C run the enter / update / exit hook of the current
// state (+0x7A4), fn_8018B000 returns the final word.

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
    u8 pad32C[0x30C];
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
    u8 pad7A8[0xC];
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
    u8 pad80C[0xCF0];
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
void fn_8018ABFC();
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

extern PlayerState lbl_803A8800[25];
float lbl_804D50E0;
int lbl_80495890[16];

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
    return &lbl_803A8800[state].anims[p->animVariant];
}

void fn_80187370(Player* p) {
    p->flags50 &= ~0x40000;
}

void fn_80187380(Player* p, float dt) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(p, dt);
}

void fn_801873D8(Player* p) {
    int host;
    u32* anim;
    int chain;
    QuerySend(p->handle, 0x2DA8293C);
    QuerySend(p->handle, 0xDFF3097A);
    PlayerResetGunTimer(p);
    HostAction((Actor*)p);
    host = PlayerInHost((Actor*)p);
    anim = PlayerStateAnim(p, 24);
    p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
    p->animLoop = 0x50;
    if (p->bone == -1) {
        chain = 8;
    } else {
        chain = ModelBoneNodeChain(p->model, ModelBoneName(p->model, p->bone), lbl_80495890, 16);
        if (chain == -1) {
            chain = 8;
        }
    }
    p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, chain * 2, 0);
    p->animDone2 = (void*)fn_8018ABFC;
    p->animLoop2 = 0x50;
}

void fn_80187598(Player* p, float dt) {
    p->unk638 = 0;
    p->flags7B4 &= ~1;
    fn_800769A0(p, dt);
    if (p->unk750 == 1) {
        fn_80074D68(p, dt);
    } else if (p->unk74C == 1) {
        PlayerUpdateMotionInput(dt, p);
    } else {
        PlayerSelectCollisionUpdate(p, dt);
    }
    if (p->animC4 == -1) {
        if (p->unk7D0 != 0) {
            PlayerSetValue4(&p->state, 3);
        } else {
            PlayerSetValue4(&p->state, 1);
        }
    } else {
        PlayerSetValue4(&p->state, 0x16);
    }
}

void fn_8018766C(Player* p) {
    p->animDone = 0;
    if (p->animC4 != -1) {
        p->anim = p->animC4;
    }
    if (p->anim11C != -1) {
        p->anim2 = p->anim11C;
    }
}

void fn_80187698(Player* p, float dt) {
    AssetFind(0xD9044CBB, 0x50EC64DA);
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    p->unk7D0 = 1;
    PlayerGunInput(p);
    if (!(p->flags7B4 & 0x100000)) {
        PlayerSetActionMode(p, 3);
    } else {
        PlayerSetValue4(&p->state, 0x10);
    }
}

void fn_80187878(Player* p) {
    int host = PlayerInHost((Actor*)p);
    u32* anim = PlayerStateAnim(p, 16);
    p->unk1CD4 = 500.0f;
    p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
    p->animDone = (void*)fn_8018ABFC;
    p->animLoop = 1;
    p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
    p->animLoop2 = 1;
}

void fn_80187934(Player* p) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerSetValue4(&p->state, 0x15);
    if (p->unk279C == -1 || fn_80175EFC(p->unk279C) != 0) {
        PlayerSetValue4(&p->state, 3);
        p->unk684 = lbl_804D50E0;
        fn_80155A84(p->unk26E0, (int)p->link, 1);
    }
}

void fn_801879BC(Player* p) {
    int blend = 0;
    LinkObj* link;
    lbl_804D50E0 = p->unk684;
    link = p->link;
    switch (link->handle >> 24) {
    case 0x13:
        if ((int)(link->handle >> 24) == 0x13) {
            HostModel* m = fn_800A384C(link);
            u32* anim;
            if (HasCondition(link->conds, 11)) {
                blend = 1;
            }
            anim = PlayerStateAnim(p, 21);
            p->anim = fn_800A1BC8(m->model, m->animSet, *anim, blend, 0);
            p->anim2 = fn_800A1BC8(m->model, m->animSet2, *anim, blend, 0);
        }
        break;
    case 1:
        p->anim = link->info->anim;
        p->anim2 = link->info->anim;
        break;
    }
    p->animDone = (void*)fn_8018ABFC;
    p->animLoop = 0x59;
    p->animLoop2 = 0x59;
    PlayerSetValue4(&p->state, 0x15);
}

void fn_80187AE0(Player* p, float dt) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    if (PlayerGetHeldSlotIndex(p)) {
        PlayerSetValue4(&p->state, 2);
    } else {
        PlayerSetValue4(&p->state, 1);
    }
    PlayerInHost((Actor*)p);
    PlayerGunInput(p);
}

void fn_80187CB4(Player* p) {
    int host = PlayerInHost((Actor*)p);
    u32* anim = PlayerStateAnim(p, 20);
    p->unk78 = 0;
    p->flags50 &= ~0x20000;
    p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
    p->animDone = (void*)fn_8018ABFC;
    p->animLoop = 1;
    p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
    p->animLoop2 = 1;
    {
        QueryMsg q;
        q.hash = 0x08783452;
        SendMsg(p->unk26E4->handle, 0x41E, &q);
        if (*(int*)&q.result != 0) {
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
}

void fn_80187E24(Player* p) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    if (p->flags7B4 & 1) {
        p->flags7B4 &= ~1;
    } else {
        p->flags50 |= 0x20000;
        p->unk78 = 1;
    }
}

void fn_80187E80(Player* p) {
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
    p->animDone = (void*)fn_8018ABFC;
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

void fn_80187FE0(Player* p) {
    fn_80155A84(p->unk26E0, (int)p->link, 1);
}

void fn_8018800C(Player* p) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    p->flags50 |= 0x20000;
    p->unk78 = 1;
    PlayerSetValue4(&p->state, 0x13);
}

void fn_80188068(Player* p) {
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
    anim = &lbl_803A8800[18].anims[variant];
    p->anim = fn_800A1BC8(m->model, m->animSet, *anim, blend, 0);
    p->animDone = (void*)fn_8018ABFC;
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

void fn_801881C4(Player* p) {
}

void fn_801881C8(Player* p, float dt) {
    int a;
    int b;
    int c;
    int d;
    QueryNameOut(&a, p->handle, 0x2DA8293C);
    QueryNameOut(&b, p->handle, 0xDFF3097A);
    if (Fn800F7DEC(a, b, 0x6FAB23FE, fn_800498D0(p), 0) != p->unk7DC) {
        PlayerSetActionMode(p, 0x11);
    }
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    PlayerGunInput(p);
    QueryNameOut(&c, p->handle, 0x2DA8293C);
    QueryNameOut(&d, p->handle, 0xDFF3097A);
    if (p->unk318 == 0) {
        PlayerSetValue4(&p->state, 3);
        if (p->state == 0x11) {
            fn_80048BA8(p, p->unk1D04, 1);
            Fn800F7F5C(c, d, 0x9791AF51, fn_800498D0(p), p->unk1D04, p->unk14FC);
            PlayerSetHeldSlotMode(p, 8);
        }
    } else {
        PlayerResetGunTimer(p);
    }
}

void fn_801885AC(Player* p) {
    int a;
    int b;
    int blend;
    QueryNameOut(&a, p->handle, 0x2DA8293C);
    QueryNameOut(&b, p->handle, 0xDFF3097A);
    p->unk7DC = Fn800F7DEC(a, b, 0x6FAB23FE, fn_800498D0(p), 0);
    switch (p->unk7DC) {
    case 0x7183F329:
        blend = 0;
        break;
    case 0x7183F5A1:
        blend = 1;
        break;
    case 0xBEF8884D:
        blend = 2;
        break;
    }
    {
        int host = PlayerInHost((Actor*)p);
        u32* anim = PlayerStateAnim(p, 17);
        p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, blend, 0);
        p->animDone = (void*)fn_8018ABFC;
        p->animLoop = 1;
        p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
        p->animLoop2 = 1;
    }
}

void fn_8018878C(Player* p) {
    p->flags50 &= ~0x40000;
}

void fn_8018879C(Player* p, float dt) {
    PlayerResetGunTimer(p);
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    if (p->unk750 == 1) {
        fn_80074D68(p, dt);
    } else if (p->unk74C == 1) {
        PlayerUpdateMotionInput(dt, p);
    } else {
        PlayerSelectCollisionUpdate(p, dt);
    }
    if (lbl_804D50E4 < 20.0f) {
        lbl_804D50E4 += 0.02f * dt;
        lbl_804D50E4 = lbl_804D50E4 < 20.0f ? lbl_804D50E4 : 20.0f;
    }
    WeatherSetRainB(lbl_804D50E4);
}

void fn_8018886C(Player* p) {
    int a;
    int b;
    QueryNameOut(&a, p->handle, 0x2DA8293C);
    QueryNameOut(&b, p->handle, 0xDFF3097A);
    PlayerResetGunTimer(p);
    p->flags50 |= 0x40000;
    fn_800F7AF0(a, b);
    fn_80050EB0(p);
    {
        int host = PlayerInHost((Actor*)p);
        u32* anim = PlayerStateAnim(p, 13);
        p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
        p->animLoop = 0x50;
        p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 1, 0);
        p->animDone2 = (void*)fn_8018ABFC;
        p->animLoop2 = 0x50;
    }
    if (!fn_80049940()) {
        lbl_804D50E4 = 0.0f;
        WeatherSetRainB(lbl_804D50E4);
        WeatherSetByTypeId(p->unk248, 0, 0x5EAC46DA);
    }
}

void fn_80188A40(Player* p, float dt) {
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    PlayerGunInput(p);
    PlayerSetValue4(&p->state, 3);
}

void fn_80188BE0(Player* p) {
    int a;
    int b;
    QueryNameOut(&a, p->handle, 0x2DA8293C);
    QueryNameOut(&b, p->handle, 0xDFF3097A);
    fn_80048BA8(p, p->unk1D04, 1);
    Fn800F7F5C(a, b, 0xBC78F550, fn_800498D0(p), p->unk1D04, p->unk14FC);
    {
        int host = PlayerInHost((Actor*)p);
        u32* anim = PlayerStateAnim(p, 11);
        p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
        p->animDone = (void*)fn_8018ABFC;
        p->animLoop = 0x11;
        p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
        p->animLoop2 = 0x11;
    }
}

void fn_80188D98(Player* p, float dt) {
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    PlayerGunInput(p);
    PlayerSetValue4(&p->state, 3);
}

void fn_80188F38(Player* p) {
    int a;
    int b;
    QueryNameOut(&a, p->handle, 0x2DA8293C);
    QueryNameOut(&b, p->handle, 0xDFF3097A);
    fn_80048BA8(p, p->unk1D04, 1);
    Fn800F7F5C(a, b, 0x8FDD7599, fn_800498D0(p), p->unk1D04, p->unk14FC);
    {
        int host = PlayerInHost((Actor*)p);
        u32* anim = PlayerStateAnim(p, 10);
        p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
        p->animDone = (void*)fn_8018ABFC;
        p->animLoop = 0x11;
        p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
        p->animLoop2 = 0x11;
    }
}

void fn_801890F0(Player* p) {
    ActorPlayAnim(0x02000000, 0xD39F69B4, fn_800498D0(p));
}

void fn_80189124(Player* p, float dt) {
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    PlayerGunInput(p);
    PlayerSetValue4(&p->state, 1);
}

void fn_801892C4(Player* p) {
    int v;
    QueryNameOut(&v, p->handle, 0xDFF3097A);
    {
        int host = PlayerInHost((Actor*)p);
        u32* anim = PlayerStateAnim(p, 9);
        p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
        p->animDone = (void*)fn_8018ABFC;
        p->animLoop = 0x10;
        p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
        p->animLoop2 = 0x10;
    }
    lbl_804D3054 = fn_800F7994(v, 0x9D1C245C);
    FxStart(lbl_804D3054, 0, 0);
}

void fn_80189404(Player* p, float dt) {
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    PlayerGunInput(p);
}

void fn_80189598(Player* p) {
    int blend;
    switch (p->unk7DC) {
    case 0x7183F329:
        blend = 0;
        break;
    case 0x7183F5A1:
        blend = 1;
        break;
    case 0xBEF8884D:
        blend = 2;
        break;
    }
    {
        int host = PlayerInHost((Actor*)p);
        u32* anim = PlayerStateAnim(p, 8);
        p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, blend, 0);
        p->animDone = (void*)fn_8018ABFC;
        p->animLoop = 1;
        p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
        p->animLoop2 = 1;
    }
}

void fn_801896A0(Player* p, float dt) {
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    if (p->unk808 >= 5) {
        PlayerSetValue4(&p->state, 3);
    } else {
        PlayerSetValue4(&p->state, 5);
    }
    PlayerGunInput(p);
    if (p->unk314 == 0) {
        PlayerSetValue4(&p->state, 3);
    } else {
        PlayerResetGunTimer(p);
    }
}

void fn_80189880(Player* p) {
    int host = PlayerInHost((Actor*)p);
    u32* anim = PlayerStateAnim(p, 5);
    p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
    p->animDone = (void*)fn_8018ABFC;
    p->animLoop = 1;
    p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
    p->animLoop2 = 1;
    p->unk808++;
}

void fn_80189940(Player* p, float dt) {
    int b;
    int a;

    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerResetGunTimer(p);
    PlayerMove(p, dt);
    p->unk7D0 = 1;
    if (p->unk7D8 == 0.0f) {
        PlayerSetHeldSlotMode(p, 0);
        return;
    }
    if (p->unk678 & 0x80) {
        p->unk678 &= ~0x80;
        PlayerSetActionMode(p, 3);
        return;
    }
    PlayerSetValue4(&p->state, 3);
    PlayerGunInput(p);
    QueryNameOut(&a, p->handle, 0x2DA8293C);
    QueryNameOut(&b, p->handle, 0xDFF3097A);
    if ((p->unk314 & ~2) == 1 && p->unk7F0 == 1) {
        PlayerResetGunTimer(p);
        switch (Fn800F7DEC(a, b, 0x0020EE36, fn_800498D0(p), 0)) {
        case 0x04A3E183:
            if (p->state != 5) {
                PlayerSetHeldSlotMode(p, 5);
            }
            break;
        case 0xC0BECF7E:
            if (Fn800F7DEC(a, b, 0xE6B71BCC, fn_800498D0(p), 0) == 0x04A3E183) {
                PlayerSetHeldSlotMode(p, 0xA);
            }
            break;
        case 0x11D2C025:
            if (p->state == 5) {
                PlayerSetHeldSlotMode(p, 3);
            }
            break;
        case 0x3DA260F7:
            if (p->state == 5) {
                PlayerSetHeldSlotMode(p, 3);
            }
            break;
        }
    } else if (p->state == 5) {
        PlayerSetHeldSlotMode(p, 3);
    }
}

void fn_80189D54(Player* p) {
    int host = PlayerInHost((Actor*)p);
    u32* anim = PlayerStateAnim(p, 3);
    p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
    p->animDone = (void*)fn_8018ABFC;
    p->animLoop = 1;
    p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
    p->animLoop2 = 1;
    PlayerGunInput(p);
    p->unk808 = 0;
}

void fn_80189F0C(Player* p) {
    ActorPlayAnim(0x02000000, 0x8B3A27A3, fn_800498D0(p));
}

void fn_80189F40(Player* p, float dt) {
    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    PlayerGunInput(p);
    PlayerSetValue4(&p->state, 3);
}

void fn_8018A0E0(Player* p) {
    int v;
    QueryNameOut(&v, p->handle, 0xDFF3097A);
    {
        int host = PlayerInHost((Actor*)p);
        u32* anim = PlayerStateAnim(p, 2);
        p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
        p->animLoop = 0x10;
        p->animDone = (void*)fn_8018ABFC;
        p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
        p->animLoop2 = 0x10;
    }
    lbl_804D3050 = fn_800F7994(v, 0xAFEF562B);
    FxStart(lbl_804D3050, 0, 0);
}

void fn_8018A220(Player* p, float dt) {
    int b;
    int a;

    p->unk638 = 1;
    p->flags7B4 &= ~1;
    PlayerMove(p, dt);
    if (p->unk678 & 0x80) {
        p->unk678 &= ~0x80;
        PlayerSetActionMode(p, 1);
    }
    PlayerSetValue4(&p->state, 1);
    p->unk7D0 = 0;
    PlayerGunInput(p);
    QueryNameOut(&a, p->handle, 0x2DA8293C);
    QueryNameOut(&b, p->handle, 0xDFF3097A);
    if ((p->unk314 & ~2) == 1 && p->unk7F0 == 1) {
        PlayerResetGunTimer(p);
        switch (Fn800F7DEC(a, b, 0x0020EE36, fn_800498D0(p), 0)) {
        case 0x04A3E183:
            if (p->state != 5) {
                PlayerSetHeldSlotMode(p, 5);
            }
            break;
        case 0xC0BECF7E:
            if (Fn800F7DEC(a, b, 0xE6B71BCC, fn_800498D0(p), 0) == 0x04A3E183) {
                PlayerSetHeldSlotMode(p, 0xA);
            }
            break;
        case 0x11D2C025:
            if (p->state == 5) {
                PlayerSetHeldSlotMode(p, 3);
            }
            break;
        case 0x3DA260F7:
            if (p->state == 5) {
                PlayerSetHeldSlotMode(p, 3);
            }
            break;
        }
    } else if (p->state == 5) {
        PlayerSetHeldSlotMode(p, 3);
    }
}

void fn_8018A604(Player* p) {
    int host = PlayerInHost((Actor*)p);
    u32* anim = PlayerStateAnim(p, 1);
    p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, *anim, 0, 0);
    p->animLoop = 1;
    p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, *anim, 0, 0);
    p->animDone2 = (void*)fn_8018ABFC;
    p->animLoop2 = 1;
    p->unk7D0 = 0;
}

void fn_8018A6C0(Player* p) {
    PlayerResetGunTimer(p);
}

void fn_8018AA90(Player* p) {
    int host;

    if ((float)rand() / 32767.0f >= 0.5f) {
        p->unk7D4 = 0;
    } else {
        p->unk7D4 = 1;
    }
    host = PlayerInHost((Actor*)p);
    p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, lbl_803A8800[0].anims[0], p->unk7D4, 0);
    if (p->anim == -1) {
        p->unk7D4 = 0;
        p->anim = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet, lbl_803A8800[0].anims[0], p->unk7D4, 0);
    }
    p->animDone = (void*)fn_8018ABFC;
    p->animLoop = 1;
    p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, lbl_803A8800[0].anims[0], p->unk7D4, 0);
    if (p->anim2 == -1) {
        p->unk7D4 = 0;
        p->anim2 = fn_800A1BC8(p->hosts[host]->model, p->hosts[host]->animSet2, lbl_803A8800[0].anims[0], p->unk7D4, 0);
    }
    p->animDone2 = (void*)fn_8018ABFC;
    p->animLoop2 = 1;
}

u32 fn_8018B000(Player* p) {
    return lbl_803A8800[p->state].unk44;
}

void fn_8018B01C(Player* p) {
    if (lbl_803A8800[p->state].exit != 0) {
        lbl_803A8800[p->state].exit(p);
    }
}

struct StateClock {
    StateClock() : time(0) {}
    int time;
};

void fn_8018B060(Player* p, int arg) {
    static StateClock sClock;
    if (lbl_803A8800[p->state].update != 0) {
        lbl_803A8800[p->state].update(p, arg);
    }
}

void fn_8018B0C0(Player* p) {
    if (lbl_803A8800[p->state].enter != 0) {
        lbl_803A8800[p->state].enter(p);
    }
}

// Looks up the 16 shared player effect assets.
void fn_8018B100() {
    lbl_80495890[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_80495890[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_80495890[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_80495890[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_80495890[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_80495890[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_80495890[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_80495890[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_80495890[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_80495890[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_80495890[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_80495890[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_80495890[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_80495890[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_80495890[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_80495890[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

}

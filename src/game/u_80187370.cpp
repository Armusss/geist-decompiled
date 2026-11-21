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
    u8 pad120[0x518];
    int unk638;           // 0x638
    u8 pad63C[0x48];
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
    u8 pad7D4[0x34];
    int unk808;           // 0x808
    u8 pad80C[0x14C8];
    float unk1CD4;        // 0x1CD4
    u8 pad1CD8[0x9D8];
    int bone;             // 0x26B0
    u8 pad26B4[0x10];
    HostModel* hosts[2];  // 0x26C4
    u8 pad26CC[0x14];
    u8 unk26E0[8];        // 0x26E0
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

extern "C" {
void fn_80155A84(void* a, int b, int c);
int fn_800498D0();
void fn_800769A0(Player* p);
void fn_80074D68(Player* p, float dt);
int fn_800A1BC8(int model, int set, u32 anim, int a, int b);
int fn_80175EFC(int slot);
void fn_8018ABFC();
HostModel* fn_800A384C(LinkObj* obj);
int SendMsg(int to, int msg, void* data);

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
    fn_800769A0(p);
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

void fn_801881C4(Player* p) {
}

void fn_8018878C(Player* p) {
    p->flags50 &= ~0x40000;
}

void fn_801890F0(Player* p) {
    ActorPlayAnim(0x02000000, 0xD39F69B4, fn_800498D0());
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

void fn_80189F0C(Player* p) {
    ActorPlayAnim(0x02000000, 0x8B3A27A3, fn_800498D0());
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

}

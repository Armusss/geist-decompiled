// u_80160F40.cpp: "Health Powerup" host-behaviour state machine (0x80160F40-0x80161F40).
// A character behaviour registered in the behaviour table (entry at 0x803A78C0) whose three-word
// state table (enter/exit/update, 0x803A7860) drives a health pickup effect: it computes a random
// pickup value, spawns/updates the "multi_genericpickup" effect, and answers the owner's health
// query messages. Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"
#include "game/owner.h"

typedef unsigned long u32;
typedef unsigned short u16;

struct Actor;

struct Owner;
struct Beh;
struct BehData;

struct BehData {
    int state;                                 // 0x00
    int next;                                  // 0x04
    u32 flags;                                 // 0x08
    int handle;                                // 0x0C
    float timer1;                              // 0x10
    float timer2;                              // 0x14
};

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

// Message structs.
struct PickupValueMsg {
    int unk0;
    float value;
    int result;
    int unkC;
};

struct PickupSpawnMsg {
    int unk0;
    int handle;
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
    Vec3 pos;
};

// Shared engine functions (C linkage ones as plain names).
extern "C" int SendMsg(int to, int msg, void* data);            // 0x800265D4
extern "C" void FxStart(void* fx, int a, int b);                // 0x8016F390
extern "C" int rand(void);                                      // 0x802F2CFC
void* FxFind(const char* name);                                 // 0x80170834

void BehUpdate(Owner* owner, Beh* beh);                         // 0x801632A8
void BehDone(Beh* beh);                                         // 0x80162EBC
void BehReset(Beh* beh);                                        // 0x801633D4
void Fn8016295C(Owner* owner, Beh* beh);                        // 0x8016295C
int OwnerId(Owner* owner);                                      // 0x800A3A40
int ModelFindBone(int model, int bone);                         // 0x80018AD4
int ModelAnimate(int model, char* name, int a);                 // 0x8003AA9C
void ModelNodeSetup(int a, int b, int c, int d);                // 0x80038AE8
void ActorGetMatrix(Mtx34* out, Actor* actor);                  // 0x8009ED70
int HasCondition(void* owner, int id);                          // 0x800A7AC4
void* VAlloc(int size);                                         // 0x8009DA68
void VFree(void* p);                                            // 0x8009DA3C

int Fn800F7DEC(int a, int b, int msg, int who, void* data);     // 0x800F7DEC
void Fn800F7F5C(int a, int b, int msg, int who, void* data, float f); // 0x800F7F5C
int Fn80161FF4(Owner* owner, void* data, float f);              // 0x80161FF4

// Not named yet: small engine helpers.
extern "C" int fn_800F8AF4(int id);                             // 0x800F8AF4
extern "C" int fn_800BA7FC(int id);                             // 0x800BA7FC
extern "C" int fn_8003A3F0(int model, int node, int anim);      // 0x8003A3F0
extern "C" void fn_80027690(int handle);                        // 0x80027690
extern "C" void fn_800277B0(int handle, void* pos, void* col, void* delay); // 0x800277B0
extern "C" int fn_80027A58(int type, void* pos, void* col, int delay, int a); // 0x80027A58

extern BehState gHostBehStates[8];

// 0x80160F40
void PickupBehState7Update(void* owner, void* beh) {
    if (beh) {
        BehData* d = ((BehData*)((Beh*)beh)->data);
        if ((d->flags & 0x20) == 0) {
            return;
        }
        if ((d->flags & 0x10) == 0) {
            return;
        }
        d->next = 0;
    }
}

// 0x80160F6C
void PickupBehState6Update(void* owner, void* beh) {
    if (beh) {
        BehData* d = ((BehData*)((Beh*)beh)->data);
        if ((d->flags & 0x20) == 0) {
            return;
        }
        if ((d->flags & 0x10) == 0) {
            return;
        }
        d->next = 0;
    }
}

// 0x80160F98
void PickupBehState5Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehData* d = ((BehData*)b->data);
        int delay;
        Vec4 col;
        Vec3 pos;

        BehUpdate(o, b);
        Fn8016295C(o, b);
        b->flags &= ~2;
        b->flags &= ~8;
        if (d->flags & 1) {
            if (fn_800BA7FC(0x29) == 0 && (d->flags & 0x100) == 0) {
                int bone = ModelFindBone(o->model, o->boneId1);
                if (bone == -1) {
                    bone = ModelFindBone(o->model, o->boneId2);
                }
                if (bone != -1) {
                    int base = fn_8003A3F0(o->model, o->nodeF0, ModelAnimate(o->model, 0, 0));
                    Mtx34 mtx;
                    ActorGetMatrix(&mtx, (Actor*)(base + bone * 0x30));
                    pos.x = mtx.m[2][1];
                    pos.y = mtx.m[2][2];
                    pos.z = mtx.m[2][3];
                } else {
                    pos.x = o->pos.x;
                    pos.y = o->pos.y;
                    pos.z = o->pos.z;
                }
                float t = (float)rand() / 32767.0f;
                delay = (int)(250.0f * t + 800.0f);
                float r = (float)rand() / 32767.0f;
                col.x = 1.0f;
                col.y = 0.3f * r + 0.3f;
                col.z = 0.0f;
                col.w = 1.0f;
                if (d->handle == -1) {
                    d->handle = fn_80027A58(1, &pos, &col, delay, 9);
                    b->flags |= 0x100000;
                } else {
                    fn_800277B0(d->handle, &pos, &col, &delay);
                    b->flags &= ~0x100000;
                    fn_80027690(d->handle);
                    d->handle = -1;
                    d->flags |= 0x100;
                }
            }
            if (d->flags & 0x10) {
                switch (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, (int)0xF829DB93, OwnerId(o), 0)) {
                case (int)0xC0BECF7E: {
                    int handle = o->id;
                    int sent;
                    int ans;
                    {
                        QueryMsg m;
                        m.hash = 0xBDC9494E;
                        *(int*)&m.result = 0;
                        m.unk8 = 0;
                        sent = SendMsg(handle, 0x41E, &m);
                        ans = *(int*)&m.result;
                    }
                    if (sent == 0 || ans != 0) {
                        d->next = 7;
                    }
                    break;
                }
                case 0x4A3E183:
                    if ((b->flags & 8) == 0) {
                        d->next = 0;
                    }
                    break;
                case 0x11D2C025:
                case (int)0x3DA260F7:
                    break;
                }
            }
        }
    }
}

// 0x801612E0
void PickupBehState4Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehData* d = ((BehData*)b->data);
        int delay;
        Vec4 col;
        Vec3 pos;

        BehUpdate(o, b);
        Fn8016295C(o, b);
        b->flags &= ~2;
        b->flags &= ~8;
        if (d->flags & 1) {
            if (fn_800BA7FC(0x29) == 0 && (d->flags & 0x100) == 0) {
                int bone = ModelFindBone(o->model, o->boneId1);
                if (bone == -1) {
                    bone = ModelFindBone(o->model, o->boneId2);
                }
                if (bone != -1) {
                    int base = fn_8003A3F0(o->model, o->nodeF0, ModelAnimate(o->model, 0, 0));
                    Mtx34 mtx;
                    ActorGetMatrix(&mtx, (Actor*)(base + bone * 0x30));
                    pos.x = mtx.m[2][1];
                    pos.y = mtx.m[2][2];
                    pos.z = mtx.m[2][3];
                } else {
                    pos.x = o->pos.x;
                    pos.y = o->pos.y;
                    pos.z = o->pos.z;
                }
                float t = (float)rand() / 32767.0f;
                delay = (int)(250.0f * t + 800.0f);
                float r = (float)rand() / 32767.0f;
                col.x = 1.0f;
                col.y = 0.3f * r + 0.3f;
                col.z = 0.0f;
                col.w = 1.0f;
                if (d->handle == -1) {
                    d->handle = fn_80027A58(1, &pos, &col, delay, 9);
                    b->flags |= 0x100000;
                } else {
                    fn_800277B0(d->handle, &pos, &col, &delay);
                    b->flags &= ~0x100000;
                    fn_80027690(d->handle);
                    d->handle = -1;
                    d->flags |= 0x100;
                }
            }
            if (d->flags & 0x10) {
                switch (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, (int)0x0020EE36, OwnerId(o), 0)) {
                case (int)0xC0BECF7E: {
                    int handle = o->id;
                    int sent;
                    int ans;
                    {
                        QueryMsg m;
                        m.hash = 0xBDC9494E;
                        *(int*)&m.result = 0;
                        m.unk8 = 0;
                        sent = SendMsg(handle, 0x41E, &m);
                        ans = *(int*)&m.result;
                    }
                    if (sent == 0 || ans != 0) {
                        d->next = 6;
                    }
                    break;
                }
                case 0x4A3E183:
                    if ((b->flags & 4) == 0) {
                        d->next = 0;
                    }
                    break;
                case 0x11D2C025:
                case (int)0x3DA260F7:
                    break;
                }
            }
        }
    }
}

// 0x80161628
void PickupBehState0Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehUpdate(o, b);
        Fn8016295C(o, b);
        b->flags &= ~2;
        b->flags &= ~8;
    }
}

// 0x8016168C
void PickupBehState7Exit(void* owner, void* beh) {
}

// 0x80161690
void PickupBehState6Exit(void* owner, void* beh) {
}

// 0x80161694
void PickupBehState5Exit(void* owner, void* beh) {
    if (beh) {
        BehData* d = ((BehData*)((Beh*)beh)->data);
        if (d->handle != -1) {
            fn_80027690(d->handle);
            d->handle = -1;
        }
    }
}

// 0x801616DC
void PickupBehState4Exit(void* owner, void* beh) {
    if (beh) {
        BehData* d = ((BehData*)((Beh*)beh)->data);
        if (d->handle != -1) {
            fn_80027690(d->handle);
            d->handle = -1;
        }
    }
}

// 0x80161724
void PickupBehState0Exit(void* owner, void* beh) {
}

// 0x80161728
void PickupBehState7Enter(void* owner, void* beh) {
    if (beh) {
        ((BehData*)((Beh*)beh)->data)->flags = 0;
        BehDone((Beh*)beh);
    }
}

// 0x80161760
void PickupBehState6Enter(void* owner, void* beh) {
    if (beh) {
        ((BehData*)((Beh*)beh)->data)->flags = 0;
        BehDone((Beh*)beh);
    }
}

// 0x80161798
void PickupBehState5Enter(void* owner, void* beh) {
    if (beh) {
        ((BehData*)((Beh*)beh)->data)->flags = 0;
        BehDone((Beh*)beh);
    }
}

// 0x801617D0
void PickupBehState4Enter(void* owner, void* beh) {
    if (beh) {
        ((BehData*)((Beh*)beh)->data)->flags = 0;
        BehDone((Beh*)beh);
    }
}

// 0x80161808
void PickupBehState0Enter(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        ((BehData*)b->data)->flags = 0;
        BehDone(b);
        if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, (int)0xE6B71BCC, OwnerId(o), 0) == 0x4A3E183) {
            b->flags |= 0x10;
        }
    }
}

// 0x80161894
int PickupBehQuery(void* owner, void* beh) {
    return 0;
}

// Behaviour message handler (the table at 0x803A78C0 registers this).
int PickupBehMsg(void* owner, void* beh, int msg) {
    Owner* o;
    Beh* b;

    if (beh == 0) {
        return 0;
    }
    o = (Owner*)owner;
    b = (Beh*)beh;
    {
        BehData* d = ((BehData*)b->data);

        switch (msg) {
        case 0:
            d->next = 0;
            break;
        case 1:
            if (d->state == 0) {
                d->next = 4;
            }
            break;
        case 2:
            if (d->state == 0) {
                d->next = 5;
            }
            break;
        case 3:
            d->next = 6;
            break;
        case 4:
            if (d->state == 0) {
                d->next = 7;
            }
            break;
        case 0x1B83833F:
            d->flags |= 0x10;
            break;
        case 0x20EE36:
            if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, msg, OwnerId(o), 0) == 0x4A3E183) {
                if (d->timer1 >= (float)b->delay) {
                    char buf[0x60];
                    d->flags |= 1;
                    d->timer1 = 0.0f;
                    if (Fn80161FF4(o, buf, 0.0f) == 0) {
                        return 0;
                    }
                    Fn800F7F5C(o->beh.unk58, o->beh.unk5C, msg, OwnerId(o), buf, o->dt);
                    d->flags &= ~0x100;
                }
            }
            break;
        case (int)0xF829DB93:
            if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, msg, OwnerId(o), 0) == 0x4A3E183) {
                if (d->timer2 >= (float)b->unk2A) {
                    char buf[0x60];
                    d->flags |= 1;
                    d->timer2 = 0.0f;
                    if (Fn80161FF4(o, buf, 0.0f) == 0) {
                        return 0;
                    }
                    Fn800F7F5C(o->beh.unk58, o->beh.unk5C, msg, OwnerId(o), buf, o->dt);
                    d->flags &= ~0x100;
                }
            }
            break;
        case (int)0xEAC45581:
            d->next = 4;
            if (gHostBehStates[d->state].exit) {
                gHostBehStates[d->state].exit(o, b);
            }
            d->state = d->next;
            b->state = d->next;
            if (gHostBehStates[d->state].enter) {
                gHostBehStates[d->state].enter(o, b);
            }
            d->flags |= 1;
            d->flags &= ~0x100;
            break;
        case (int)0xE6B71BCC:
        case (int)0x968CBE3D:
            if ((d->flags & 0x20) == 0) {
                d->flags |= 0x20;
                Fn800F7F5C(o->beh.unk58, o->beh.unk5C, msg, OwnerId(o), 0, o->dt);
            }
            break;
        }
    }
    return 1;
}

// 0x80161C44
int PickupBehUpdate(void* owner_, void* beh_) {
    Beh* b = (Beh*)beh_;
    BehData* d;
    Owner* o = (Owner*)owner_;

    if (b == 0) {
        return 0;
    }
    d = ((BehData*)b->data);
    {

        if (HasCondition(o->conditions, 9) != 0 ||
            HasCondition(o->conditions, 10) != 0 ||
            HasCondition(o->conditions, 11) != 0) {
            if (d->handle != -1) {
                fn_80027690(d->handle);
                d->handle = -1;
            }
            ModelNodeSetup(o->model, o->nodeE4, 0, 1);
            d->flags = 0;
            BehReset(b);
            BehDone(b);
        } else {
            d->timer1 += (float)(u16)(int)o->dt;
            d->timer2 += (float)(u16)(int)o->dt;
            if (gHostBehStates[d->state].update) {
                gHostBehStates[d->state].update(o, b);
            }
            if (d->next != d->state) {
                if (gHostBehStates[d->state].exit) {
                    gHostBehStates[d->state].exit(o, b);
                }
                d->state = d->next;
                b->state = d->next;
                if (gHostBehStates[d->state].enter) {
                    gHostBehStates[d->state].enter(o, b);
                }
            }
        }
    }
    return 1;
}

// 0x80161E28
int PickupBehFree(void* owner_, void* beh_) {
    Owner* owner;
    Beh* beh;

    if (beh_ == 0) {
        return 0;
    }
    owner = (Owner*)owner_;
    beh = (Beh*)beh_;
    {
        BehData* d = ((BehData*)beh->data);
        if (d->handle != -1) {
            fn_80027690(d->handle);
            d->handle = -1;
        }
        VFree(d);
    }
    return 1;
}

// 0x80161E84
int PickupBehInit(void* owner_, void* beh_) {
    Beh* beh;

    if (beh_ == 0) {
        return 0;
    }
    beh = (Beh*)beh_;
    {
        BehData* d = (BehData*)VAlloc(0x18);
        if (d == 0) {
            beh->data = 0;
            return 0;
        }
        beh->data = d;
        beh->state = 0;
        d->state = 0;
        d->next = 0;
        d->handle = -1;
        d->flags = 0;
        d->timer1 = 0.0f;
        d->timer2 = 0.0f;
        if (gHostBehStates[d->state].enter) {
            gHostBehStates[d->state].enter(owner_, beh_);
        }
    }
    return 1;
}

// State table at 0x803A7860: enter / exit / update for states 0-7.
BehState gHostBehStates[8] = {
    { PickupBehState0Enter, PickupBehState0Exit, PickupBehState0Update },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { PickupBehState4Enter, PickupBehState4Exit, PickupBehState4Update },
    { PickupBehState5Enter, PickupBehState5Exit, PickupBehState5Update },
    { PickupBehState6Enter, PickupBehState6Exit, PickupBehState6Update },
    { PickupBehState7Enter, PickupBehState7Exit, PickupBehState7Update },
};

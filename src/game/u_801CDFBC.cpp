// u_801CDFBC.cpp: 0x801CDFBC-0x801D0C18, 47 functions. The "rat" character file.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   behaviour state table 0x803ACC08 (8 states x enter/exit/update, fn_801CDFBC-fn_801CF038; read by
//   fn_801CF3A4/fn_801CF57C), player mode table 0x803ACC68 (25 rows of 0x48 bytes, fn_801CF62C-fn_801D07B4)
//   string pool 0x80348888 (POSSESS..SCAN, "rat_jump")
//   .sdata2 0x804D8B68-0x804D8BA0 (its own 0.0/1.0/32767.0 copies), .bss 0x80496D98 (asset preload
//   fn_801D09FC at the end, called from PlayerInitGunState)
// Two halves, like u_80160F40 (a host behaviour) and u_80208238 (the Batsqueeks player mode): the rat's
// behaviour state machine, then the player's rat host mode.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"
#include "game/owner.h"
#include "game/vecnorm.h"

typedef unsigned char u8;
typedef int s32;
typedef unsigned long u32;
typedef unsigned short u16;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Actor;

// ---------------------------------------------------------------------------------------------------------
// The rat's behaviour (Beh::data, VAlloc'd, 0x14 bytes).
struct RatBehData {
    int state;                                 // 0x00
    int next;                                  // 0x04
    u32 flags;                                 // 0x08
    int handle;                                // 0x0C: light handle
    float timer;                               // 0x10
};

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

extern "C" int SendMsg(int to, int msg, void* data);            // 0x800265D4
extern "C" void* FxStart(void* fx, int a, int b);               // 0x8016F390
extern "C" int rand(void);                                      // 0x802F2CFC
void* FxFind(const char* name);                                 // 0x80170834

void BehDone(Beh* beh);                                         // 0x80162EBC
void BehReset(Beh* beh);                                        // 0x801633D4
void Fn8016295C(Owner* owner, Beh* beh);                        // 0x8016295C
int OwnerId(Owner* owner);                                      // 0x800A3A40
float OwnerValue(Owner* o, void* value);                        // 0x800AA050
void* FollowGetTargetObj(void* range);                          // 0x800A7A18
void* GetPlayer();                                              // 0x80049948
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

extern "C" int fn_800BA7FC(int id);                             // 0x800BA7FC
extern "C" int fn_8003A3F0(int model, int node, int anim);      // 0x8003A3F0
extern "C" void fn_80027690(int handle);                        // 0x80027690
extern "C" void fn_800277B0(int handle, void* pos, void* col, void* delay); // 0x800277B0
extern "C" int fn_80027A58(int type, void* pos, void* col, int delay, int a); // 0x80027A58
extern "C" void fn_800A0168(Mtx34* out, Mtx34* in);             // 0x800A0168
extern "C" void fn_800A0030(Vec3* out, Mtx34* m, Vec3* in);     // 0x800A0030

extern BehState gRatBehStates[8];

// 0x801CDFBC
void RatBehState7Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        RatBehData* d = ((RatBehData*)b->data);
        Fn8016295C(o, b);
        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}

// 0x801CE00C
void RatBehState6Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        RatBehData* d = ((RatBehData*)b->data);
        Fn8016295C(o, b);
        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}

// 0x801CE05C
void RatBehState5Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        RatBehData* d = ((RatBehData*)b->data);
        int delay;
        Vec4 col;
        Vec3 pos;

        Fn8016295C(o, b);
        if (d->handle != -1) {
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
            fn_800277B0(d->handle, &pos, &col, &delay);
        }
        if ((d->flags & 0x40) && (d->flags & 0x10)) {
            switch (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, (int)0xF829DB93, OwnerId(o), 0)) {
            case (int)0xC0BECF7E:
                d->next = 7;
                break;
            case 0x4A3E183:
                d->next = 0;
                break;
            case 0x11D2C025:
            case (int)0x3DA260F7:
                break;
            }
        }
    }
}

// 0x801CE284
void RatBehState4Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        RatBehData* d = ((RatBehData*)b->data);
        int delay;
        Vec4 col;
        Vec3 pos;

        Fn8016295C(o, b);
        if (d->handle != -1) {
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
            fn_800277B0(d->handle, &pos, &col, &delay);
        }
        if (d->flags & 2) {
            switch (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, 0x20EE36, OwnerId(o), 0)) {
            case (int)0xC0BECF7E:
                d->next = 6;
                break;
            case 0x4A3E183:
                d->next = 0;
                break;
            case 0x11D2C025:
            case (int)0x3DA260F7:
                break;
            }
        }
    }
}

// 0x801CE4A4
void RatBehState1Update(void* owner, void* beh) {
    Owner* o;
    Beh* b;
    RatBehData* d;
    int done = 0;
    if (beh) {
        o = (Owner*)owner;
        b = (Beh*)beh;
        d = ((RatBehData*)b->data);
        char buf[0x60];

        Fn8016295C(o, b);
        if (Fn80161FF4(o, buf, 0.0f)) {
            Fn800F7F5C(o->beh.unk58, o->beh.unk5C, 0x315A1A7F, OwnerId(o), buf, o->dt);
        }
        d->timer -= o->dt;
        if (d->timer < 0.0f) {
            done = 1;
        }
        if (done == 1) {
            if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, 0xD19E338, OwnerId(o), 0) == 0x4A3E183) {
                char buf2[0x60];
                d->next = 4;
                if (Fn80161FF4(o, buf2, 0.0f)) {
                    Fn800F7F5C(o->beh.unk58, o->beh.unk5C, 0xD19E338, OwnerId(o), buf2, o->dt);
                }
            }
        }
    }
}

// 0x801CE5D4
void RatBehState0Update(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;
        Owner* t;

        BehDone(b);
        BehReset(b);
        Fn8016295C((Owner*)owner, b);
        t = (Owner*)FollowGetTargetObj(((Owner*)owner)->conditions);
        if (t) {
            Vec3 fwd;
            Vec3 dir;
            Mtx34 m;

            b->targetKind = 2;
            b->target = t->id;
            fwd.x = 0.0f;
            fwd.y = 1.0f;
            fwd.z = 0.0f;
            float ty = (float)t->place.pos.y;
            float oy = (float)((Owner*)owner)->place.pos.y;
            float z = (float)((Owner*)owner)->place.pos.z;
            float tx = (float)t->place.pos.x;
            float ox = (float)((Owner*)owner)->place.pos.x;
            dir.x = tx - ox; dir.y = ty - oy; dir.z = z - z;
            ActorGetMatrix(&m, (Actor*)owner);
            fn_800A0168(&m, &m);
            fn_800A0030(&fwd, &m, &fwd);
            VecNormalize(&fwd, &fwd);
            VecNormalize(&dir, &dir);
            if (fwd.x * dir.x + fwd.y * dir.y + fwd.z * dir.z > 0.5f) {
                float v = OwnerValue((Owner*)owner, t);
                if (v <= b->hi1 && v >= b->lo1) {
                    b->flags |= 1;
                    b->flags |= 4;
                }
                if (v <= b->hi2 && v >= b->lo2) {
                    b->flags |= 2;
                    b->flags |= 8;
                }
                if ((b->flags & 4) == 0 && (b->flags & 8) == 0) {
                    b->flags |= 0x80;
                }
            }
        }
    }
}

// 0x801CE9E0
void RatBehState7Exit(void* owner, void* beh) {
}

// 0x801CE9E4
void RatBehState6Exit(void* owner, void* beh) {
}

// 0x801CE9E8
void RatBehState5Exit(void* owner, void* beh) {
    if (beh) {
        RatBehData* d = ((RatBehData*)((Beh*)beh)->data);
        if (d->handle != -1) {
            fn_80027690(d->handle);
            d->handle = -1;
        }
    }
}

// 0x801CEA30
void RatBehState4Exit(void* owner, void* beh) {
    if (beh) {
        RatBehData* d = ((RatBehData*)((Beh*)beh)->data);
        if (d->handle != -1) {
            fn_80027690(d->handle);
            d->handle = -1;
        }
    }
}

// 0x801CEA78
void RatBehState1Exit(void* owner, void* beh) {
}

// 0x801CEA7C
void RatBehState0Exit(void* owner, void* beh) {
}

// 0x801CEA80
void RatBehState7Enter(void* owner, void* beh) {
    if (beh) {
        ((RatBehData*)((Beh*)beh)->data)->flags = 0;
        BehDone((Beh*)beh);
    }
}

// 0x801CEAB8
void RatBehState6Enter(void* owner, void* beh) {
    if (beh) {
        ((RatBehData*)((Beh*)beh)->data)->flags = 0;
        BehDone((Beh*)beh);
    }
}

// Starts the rat's light at its head bone (or its position) when both the rat and the player can see it.
static inline void RatLightStart(Owner* o, RatBehData* d) {
    if (fn_800BA7FC(0x29) == 0 && d->handle == -1) {
        Msg400 player;
        Msg400 self;

        self.actor = (Actor*)o;
        player.actor = (Actor*)GetPlayer();
        ((Owner*)self.actor)->handler(0x400, &self);
        ((Owner*)player.actor)->handler(0x400, &player);
        if (self.flags & player.flags) {
            Vec3 pos;
            Vec4 col;
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
            float r = (float)rand() / 32767.0f;
            float t = (float)rand() / 32767.0f;
            col.x = 1.0f;
            col.y = 0.3f * r + 0.3f;
            col.z = 0.0f;
            col.w = 1.0f;
            d->handle = fn_80027A58(1, &pos, &col, (int)(250.0f * t + 800.0f), 9);
        }
    }
}

// 0x801CEAF0
void RatBehState5Enter(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        RatBehData* d = ((RatBehData*)b->data);
        d->flags = 0;
        BehDone(b);
        RatLightStart(o, d);
    }
}

// 0x801CECF4
void RatBehState4Enter(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        RatBehData* d = ((RatBehData*)b->data);
        d->flags = 0;
        BehDone(b);
        RatLightStart(o, d);
        if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, 0x20EE36, OwnerId(o), 0) == 0x4A3E183) {
            char buf[0x60];
            d->flags |= 2;
            if (Fn80161FF4(o, buf, 0.0f)) {
                Fn800F7F5C(o->beh.unk58, o->beh.unk5C, 0x20EE36, OwnerId(o), buf, o->dt);
            }
        }
    }
}

// 0x801CEF74
void RatBehState1Enter(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        RatBehData* d = ((RatBehData*)b->data);
        char buf[0x60];
        d->flags = 0;
        BehDone(b);
        d->timer = 600.0f;
        if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, (int)0x96224FCC, OwnerId(o), 0) == 0x4A3E183 &&
            Fn80161FF4(o, buf, 0.0f)) {
            Fn800F7F5C(o->beh.unk58, o->beh.unk5C, (int)0x96224FCC, OwnerId(o), buf, o->dt);
        }
    }
}

// 0x801CF038
void RatBehState0Enter(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        ((RatBehData*)b->data)->flags = 0;
        BehDone(b);
        if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, (int)0xE6B71BCC, OwnerId(o), 0) == 0x4A3E183) {
            b->flags |= 0x10;
        }
        if (Fn800F7DEC(o->beh.unk58, o->beh.unk5C, (int)0x968CBE3D, OwnerId(o), 0) == 0x4A3E183) {
            b->flags |= 0x20;
        }
    }
}

// 0x801CF100 (the five behaviour type callbacks keep their placeholder names: gBehTypes in u_80161F40 uses them)
extern "C" int fn_801CF100(void* owner, void* beh) {
    return 0;
}

// 0x801CF108: behaviour message handler.
extern "C" int fn_801CF108(void* owner, void* beh, int msg) {
    Owner* o;
    Beh* b;

    if (beh == 0) {
        return 0;
    }
    b = (Beh*)beh;
    o = (Owner*)owner;
    {
        RatBehData* d = ((RatBehData*)b->data);

        if (HasCondition(o->conditions, 9) || HasCondition(o->conditions, 10) ||
            HasCondition(o->conditions, 11)) {
            if (d->handle != -1) {
                fn_80027690(d->handle);
                d->handle = -1;
            }
            d->flags = 0;
            BehReset(b);
            BehDone(b);
            return 1;
        }

        switch (msg) {
        case 1:
            if (d->state == 0) {
                d->next = 1;
            }
            break;
        case 2:
            if (d->state == 0) {
                d->next = 5;
            }
            break;
        case 3:
            if (d->state == 0) {
                d->next = 6;
            }
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
            break;
        case (int)0xF829DB93: {
            char buf[0x60];
            if ((d->flags & 0x40) == 0 &&
                Fn800F7DEC(o->beh.unk58, o->beh.unk5C, (int)0xF829DB93, OwnerId(o), 0) == 0x4A3E183) {
                d->flags |= 0x40;
                if (Fn80161FF4(o, buf, 300.0f) == 0) {
                    return 0;
                }
                Fn800F7F5C(o->beh.unk58, o->beh.unk5C, (int)0xF829DB93, OwnerId(o), buf, o->dt);
            }
            break;
        }
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

// 0x801CF3A4
extern "C" int fn_801CF3A4(void* owner_, void* beh_) {
    Beh* b = (Beh*)beh_;
    RatBehData* d;
    Owner* o = (Owner*)owner_;

    if (b == 0) {
        return 0;
    }
    d = ((RatBehData*)b->data);
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
            if (gRatBehStates[d->state].update) {
                gRatBehStates[d->state].update(o, b);
            }
            if (d->next != d->state) {
                if (gRatBehStates[d->state].exit) {
                    gRatBehStates[d->state].exit(o, b);
                }
                d->state = d->next;
                b->state = d->next;
                if (gRatBehStates[d->state].enter) {
                    gRatBehStates[d->state].enter(o, b);
                }
            }
        }
    }
    return 1;
}

// 0x801CF520
extern "C" int fn_801CF520(void* owner_, void* beh_) {
    Owner* owner;
    Beh* beh;

    if (beh_ == 0) {
        return 0;
    }
    owner = (Owner*)owner_;
    beh = (Beh*)beh_;
    {
        RatBehData* d = ((RatBehData*)beh->data);
        if (d->handle != -1) {
            fn_80027690(d->handle);
            d->handle = -1;
        }
        VFree(d);
    }
    return 1;
}

// 0x801CF57C
extern "C" int fn_801CF57C(void* owner_, void* beh_) {
    Beh* beh;

    if (beh_ == 0) {
        return 0;
    }
    beh = (Beh*)beh_;
    {
        RatBehData* d = (RatBehData*)VAlloc(0x14);
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
        if (gRatBehStates[d->state].enter) {
            gRatBehStates[d->state].enter(owner_, beh_);
        }
    }
    return 1;
}

// State table at 0x803ACC08: enter / exit / update for states 0-7.
BehState gRatBehStates[8] = {
    { RatBehState0Enter, RatBehState0Exit, RatBehState0Update },
    { RatBehState1Enter, RatBehState1Exit, RatBehState1Update },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { RatBehState4Enter, RatBehState4Exit, RatBehState4Update },
    { RatBehState5Enter, RatBehState5Exit, RatBehState5Update },
    { RatBehState6Enter, RatBehState6Exit, RatBehState6Update },
    { RatBehState7Enter, RatBehState7Exit, RatBehState7Update },
};

// ---------------------------------------------------------------------------------------------------------
// The player's rat host mode.

// One possessed host; the player keeps a per-slot array of these.
struct HostSlot {
    u8 pad00[0x14];
    void* field14;      // 0x14
    void* field18;      // 0x18
    void* field1C;      // 0x1C
};

// Host animation object hung off the player.
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
    u8 pad120[0x208];
    u32 flags328;               // 0x328
    u8 pad32C[0x30C];
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
    u8 pad7D4[0x1EDC];
    s32 field26B0;              // 0x26B0: host bone index (-1: none)
    u8 pad26B4[0x10];
    HostSlot* slots26C4[1];     // 0x26C4: hosts, indexed by player slot
    s32* field26C8;             // 0x26C8: the host's type hash
    u8 pad26CC[0x14];
    void* field26E0;            // 0x26E0: animation/state object (opaque)
    void* field26E4;            // 0x26E4: peer/owner object
    HostAnim* field26E8;        // 0x26E8: host animation object
    u8 pad26EC[0xB0];
    s32 field279C;              // 0x279C
};

// Per-action-mode table (0x48 bytes per row): three hooks, then one motion id per character variant.
typedef void (*RatHook)(Player* player);
typedef void (*RatHook04)(Player* player, int arg);
struct AnimRow {
    RatHook hook00;                             // 0x00
    RatHook04 hook04;                           // 0x04
    RatHook callback;                           // 0x08
    s32 byVariant[15];                          // 0x0C
};
extern AnimRow gRatModeTable[];

int PlayerInHost(Actor* actor);
void PlayerSelectCollisionUpdate(void* player, float seek);
void PlayerSetActionMode(void* player, int mode);
void PlayerSetValue4(void* field, int value);
void PlayerResetGunTimer(void* player);
void PlayerSetHeldSlotIndex(void* player, int index);
void DashUpdate(Actor* a);
int ModelBoneName(int obj, int index);
int ModelBoneNodeChain(int obj, int name, int* chain, int count);

extern "C" void* fn_800A384C(void* host);
extern "C" int fn_80175EFC(int id);
extern "C" void fn_80155A84(void* anim, void* param, int flag);
extern "C" void fn_800769A0(void* player, float seek);
extern "C" void* fn_800A1BC8(void* a, void* b, int id, int c, int d);
extern "C" int fn_800498D0(void* p);
extern "C" int fn_800C5210(void* peer, int msg, void* data);
extern "C" void fn_80050EB0(void* p);

int PlayerRatModeCallback(int msg, void* p);
int gRatAssets[16];             // asset handles loaded by fn_801D09FC

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

// Asks the player's message peer for a mode handle.
static inline void QueryGet(s32* out, void* target, u32 hash)
{
    QueryMsg query;
    query.hash = hash;
    *(s32*)&query.result = 0;
    query.unk8 = 0;
    SendMsg((int)target, 0x41e, &query);
    *out = *(s32*)&query.result;
}

// Landing check shared by the rat's movement updates: small hosts set the "landed" flag, the jumping
// ones play the jump sound, everything else runs the dash update.
static inline void RatLandCheck(Player* player)
{
    switch (*player->field26C8) {
    case (int)0xF7327F84:
    case (int)0x87D85819:
    case (int)0x812D6542:
    case (int)0xBDE5BEC5:
    case (int)0xA3FFBC62:
    case (int)0xA3FFBC63:
    case (int)0xA3FFBC64:
    case 0x001F8BC6:
    case 0x52A6F4D8:
        if ((s32)player->flags328 != 0 && (player->flags7B4 & 0x100000) == 0)
            player->flags7B4 |= 0x200000;
        else if ((s32)player->flags328 == 0)
            player->flags7B4 &= ~0x200000;
        break;
    case 0x04624485:
    case 0x00013C05:
        if (player->flags328 & 1)
            FxStart(FxFind("rat_jump"), 0, 0);
        break;
    default:
        DashUpdate((Actor*)player);
        break;
    }
}

// 0x801CF62C: per-frame update while the player is leaving the rat host mode.
void PlayerRatExitUpdate(Player* player, float seek)
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

// 0x801CF6C8: clears the mode update callback and restores the saved motion handles.
void PlayerRatTeardown(Player* player)
{
    player->field08C = 0;

    if (player->field0C4 != -1)
        player->field098 = (void*)player->field0C4;

    if (player->field11C == -1)
        return;

    player->field0F0 = (void*)player->field11C;
}

// 0x801CF6F4
void PlayerRatResetMode(Player* player)
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

// 0x801CF774
void PlayerRatHostSetup(Player* player)
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
                                   gRatModeTable[21].byVariant[idx], used, 0);
    player->field08C = (void*)PlayerRatModeCallback;
    player->field0A4 = 0x19;

    player->field0F0 = fn_800A1BC8(state->field14, state->field1C,
                                   gRatModeTable[21].byVariant[idx], used, 0);
    player->field0FC = 0x19;

    PlayerSetValue4(&player->field7A4, 0x15);
}

// 0x801CF870
void PlayerRatFrameUpdate(Player* player, float seek)
{
    PlayerResetGunTimer(player);
    player->field638 = 1;
    player->flags7B4 &= ~1u;
    fn_800769A0(player, seek);
    PlayerSelectCollisionUpdate(player, seek);
    PlayerSetValue4(&player->field7A4, 1);
    PlayerInHost((Actor*)player);
    RatLandCheck(player);
}

// 0x801CFA20
void PlayerRatGunSetup(Player* player)
{
    int slot = PlayerInHost((Actor*)player);
    int idx = player->field73C;

    player->field78 = 0;
    player->field50 &= ~0x20000u;
    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field18,
                                   gRatModeTable[20].byVariant[idx], 0, 0);
    player->field08C = (void*)PlayerRatModeCallback;
    player->field0A4 = 1;
    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field1C,
                                   gRatModeTable[20].byVariant[idx], 0, 0);
    player->field0FC = 1;

    QueryMsg query;
    query.hash = 0x8783452;
    SendMsg((int)((PlayerPeer*)player->field26E4)->peer38, 0x41e, &query);
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

// 0x801CFB90
void PlayerRatResetGunState(Player* player)
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

// 0x801CFBEC
void PlayerRatHostSlotSetup(Player* player)
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
    player->field098 = fn_800A1BC8(state->field14, state->field18, gRatModeTable[19].byVariant[idx], used, 0);
    player->field08C = (void*)PlayerRatModeCallback;
    player->field0A4 = 0x19;
    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field1C,
                                   gRatModeTable[19].byVariant[idx], 0, 0);
    player->field0FC = 0x19;

    QueryMsg query;
    query.hash = 0x8783452;
    SendMsg((int)player->field26E8->field38, 0x41e, &query);
    if (*(int*)&query.result != 0)
        PlayerSetHeldSlotIndex(player, 0);
    else
        PlayerSetHeldSlotIndex(player, 1);
}

// 0x801CFD4C
void RatHostAnimStart(Player* player)
{
    fn_80155A84((void*)&player->field26E0, player->field26E8, 1);
}

// 0x801CFD78
void RatHostEnterGunMode(Player* player)
{
    PlayerResetGunTimer(player);
    player->field638 = 1;
    player->flags7B4 &= ~1u;
    player->field50 |= 0x20000;
    player->field78 = 1;
    PlayerSetValue4(&player->field7A4, 0x13);
}

// 0x801CFDD4
void PlayerRatCarryStateSetup(Player* player)
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
    player->field098 = fn_800A1BC8(state->field14, state->field18, gRatModeTable[18].byVariant[idx], used, 0);
    player->field08C = (void*)PlayerRatModeCallback;
    player->field0A4 = 0x59;
    HostSlot* host = player->slots26C4[slot];
    player->field0F0 = fn_800A1BC8(host->field14, host->field1C, gRatModeTable[18].byVariant[idx], 0, 0);
    player->field0FC = 0x59;

    QueryMsg query;
    query.hash = 0x8783452;
    SendMsg((int)player->field26E8->field38, 0x41e, &query);
    if (*(int*)&query.result != 0)
        PlayerSetHeldSlotIndex(player, 0);
    else
        PlayerSetHeldSlotIndex(player, 1);
}

// 0x801CFF30
void PlayerRatClearCarryBit(void* p)
{
    Player* player = (Player*)p;
    player->field50 &= ~0x40000u;
}

// 0x801CFF40
void PlayerRatSeekUpdate(Player* player, float seek)
{
    PlayerResetGunTimer(player);
    player->field638 = 1;
    player->flags7B4 &= ~1u;
    PlayerSelectCollisionUpdate(player, seek);
}

// 0x801CFF98
void PlayerRatWeaponMotionSetup(Player* player)
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
                                   gRatModeTable[13].byVariant[index], 0, 0);
    player->field0A4 = 0x50;

    if (player->field26B0 == -1)
    {
        bone = 8;
    }
    else
    {
        bone = ModelBoneNodeChain(player->field58,
                                  ModelBoneName(player->field58, player->field26B0),
                                  gRatAssets, 0x10);
        if (bone == -1)
            bone = 8;
    }

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14,
                                   player->slots26C4[slot]->field1C,
                                   gRatModeTable[13].byVariant[index], bone << 1, 0);
    player->field0E4 = (void*)PlayerRatModeCallback;
    player->field0FC = 0x50;
}

// 0x801D00A8
void PlayerRatQueryUpdate(Player* player, float seek)
{
    s32 first;
    s32 second;

    QueryGet(&first, player->field38, 0x2da8293c);
    QueryGet(&second, player->field38, 0xdff3097a);

    player->field638 = 1;
    player->flags7B4 &= ~1u;
    fn_800769A0(player, seek);
    PlayerSelectCollisionUpdate(player, seek);
    player->field7D0 = 1;

    if ((player->flags678 & 0x80) != 0)
    {
        player->flags678 &= ~0x80u;
        PlayerSetActionMode(player, 3);
        return;
    }

    PlayerSetValue4(&player->field7A4, 3);
    RatLandCheck(player);
}

// 0x801D0340
void PlayerRatSlotHandleSetup(Player* player)
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
                                       gRatModeTable[3].byVariant[index], used, 0);
        player->field08C = (void*)PlayerRatModeCallback;
        player->field0A4 = 1;
        if ((s32)player->field098 == -1 && used)
            used = 0;
    }

    HostSlot* slot = player->slots26C4[host];
    player->field0F0 = fn_800A1BC8(slot->field14, slot->field1C, gRatModeTable[3].byVariant[index], 0, 0);
    player->field0FC = 1;
}

// 0x801D0534
void PlayerRatExitMotionUpdate(Player* player, float seek)
{
    player->field638 = 1;
    player->flags7B4 &= ~1u;
    fn_800769A0(player, seek);
    PlayerSelectCollisionUpdate(player, seek);

    if ((player->flags678 & 0x80) != 0)
    {
        player->flags678 &= ~0x80u;
        PlayerSetActionMode(player, 1);
    }

    PlayerSetValue4(&player->field7A4, 1);
    player->field7D0 = 0;
    RatLandCheck(player);
}

// 0x801D06F8
void PlayerRatSlotMotionSetup(Player* player)
{
    int slot = PlayerInHost((Actor*)player);
    int index = player->field73C;

    player->field098 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field18,
                                   gRatModeTable[1].byVariant[index], 0, 0);
    player->field0A4 = 1;

    player->field0F0 = fn_800A1BC8(player->slots26C4[slot]->field14, player->slots26C4[slot]->field1C,
                                   gRatModeTable[1].byVariant[index], 0, 0);
    player->field0E4 = (void*)PlayerRatModeCallback;
    player->field0FC = 1;
    player->field7D0 = 0;
}

// 0x801D07B4: per-mode callback.
int PlayerRatModeCallback(int msg, void* p)
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
    }

    return 0;
}

struct LargeActor;
void PlayerDashStart(Actor* actor);
void PlayerDashCheck(Actor* actor);
void LargeActorResetFlags(LargeActor* actor);

// The per-action-mode table at 0x803ACC68: setup and update hooks, an optional callback and the motion
// ids of each character variant, one row per action mode.
AnimRow gRatModeTable[25] = {
    { 0, 0, 0, { 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x2237d4, 0x0 } },
    { (RatHook)PlayerRatSlotMotionSetup, (RatHook04)PlayerRatExitMotionUpdate, 0, { 0xa8be217b, 0x744b56b9, 0x90a41c8e, 0x29d70f30, 0xa81c49f3, 0xa798111, 0x35fc8e1c, 0xa9d72f66, 0x8254aa67, 0xd3193f08, 0x81548f05, 0x40ec2f58, 0xb1bec6a3, 0x11042941, 0x0 } },
    { 0, 0, 0, { 0x34cd59aa, 0xd84584ac, 0x23823037, 0x30b56155, 0x454c4232, 0x58fcaf54, 0x83ae54e9, 0xb0b5625f, 0x230c033e, 0x9101d67d, 0x4c4e12e0, 0xffe79c2d, 0x140f6f82, 0x1f644124, 0x0 } },
    { (RatHook)PlayerRatSlotHandleSetup, (RatHook04)PlayerRatQueryUpdate, 0, { 0x2a1a099, 0xc5d70d57, 0x3be1446c, 0x5514778e, 0xd9df8711, 0xc1bbedaf, 0xe424c17a, 0x2feb2d44, 0x3bdd7485, 0xdc50bd66, 0x1c29e8a3, 0x5bc381b6, 0x1a596fc1, 0xa32e1df, 0x0 } },
    { 0, 0, 0, { 0x6db3b0e5, 0x3940e623, 0x5599abf8, 0xeecc9e9a, 0x6d11d95d, 0xcf6f107b, 0xfaf21d86, 0x6eccbed0, 0x474a39d1, 0x980ece72, 0x464a1e6f, 0x5e1bec2, 0x76b4560d, 0xd5f9b8ab, 0x0 } },
    { 0, 0, 0, { 0xce83f460, 0x50aef7e2, 0xf9a70a2d, 0x5d8feacb, 0xcd335ee8, 0x1f00ac8a, 0xf98b35f, 0x2ad0a655, 0x7dc7f2f4, 0xcd4789f3, 0x2a2d8316, 0x4d42fba3, 0xb681a038, 0xcec2725a, 0x0 } },
    { 0, 0, 0, { 0x48c2836e, 0xeedbf5f0, 0x5d9beabb, 0xeac74c59, 0x352969f6, 0x1e731698, 0x6351aaed, 0x6acb32e3, 0xa1fd1802, 0x69cb1781, 0x82f9c724, 0xb6563131, 0x5fd68346, 0xe93d7468, 0x0 } },
    { 0, 0, 0, { 0x21820da8, 0xa708e7aa, 0x659ed5b5, 0x557847d3, 0x8fc97230, 0x4d5a7e52, 0xd4e55167, 0xe41cb3dd, 0x1669cd3c, 0x22b5e8fb, 0xaaf73bde, 0x78e156ab, 0xc3957780, 0x8f04a822, 0x0 } },
    { 0, 0, 0, { 0x48c24dc9, 0xeedbc04b, 0x5d9bb516, 0xeac716b4, 0x35293451, 0x1e72e0f3, 0x63517548, 0x6acafd3e, 0xa1fce25d, 0x69cae1dc, 0x82f9917f, 0xb655fb8c, 0x5fd64da1, 0xe93d3ec3, 0x0 } },
    { 0, 0, 0, { 0x64ddf349, 0x306b2887, 0x4cc3ee5c, 0xe5f6e0fe, 0x643c1bc1, 0xc69952df, 0xf21c5fea, 0x65f70134, 0x3e747c35, 0x8f3910d6, 0x3d7460d3, 0xfd0c0126, 0x6dde9871, 0xcd23fb0f, 0x0 } },
    { 0, 0, 0, { 0x51979943, 0xf50fc445, 0x404c6fd0, 0x4d7fa0ee, 0x621681cb, 0x75c6eeed, 0xa0789482, 0xcd7fa1f8, 0x3fd642d7, 0xadcc1616, 0x69185279, 0x1cb1dbc6, 0x30d9af1b, 0x3c2e80bd, 0x0 } },
    { 0, 0, 0, { 0x1de2c92c, 0xa369a32e, 0x61ff9139, 0x51d90357, 0x8c2a2db4, 0x49bb39d6, 0xd1460ceb, 0xe07d6f61, 0x12ca88c0, 0x1f16a47f, 0xa757f762, 0x7542122f, 0xbff63304, 0x8b6563a6, 0x0 } },
    { (RatHook)PlayerDashStart, (RatHook04)PlayerDashCheck, (RatHook)LargeActorResetFlags, { 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x72c27236, 0x0 } },
    { (RatHook)PlayerRatWeaponMotionSetup, (RatHook04)PlayerRatSeekUpdate, (RatHook)PlayerRatClearCarryBit, { 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x1f6f94f6, 0x0 } },
    { 0, 0, 0, { 0xf8b4bd22, 0x15c99ae0, 0x7f083f75, 0x97491197, 0x992aa79a, 0xd9168f38, 0x300a8583, 0x17c1fc4d, 0xc6ccbb0e, 0xf8beab6f, 0x565f02c, 0x3d96c7bf, 0xc420b84a, 0x6797eb68, 0x0 } },
    { 0, 0, 0, { 0x1de2fed1, 0xa369d8d3, 0x61ffc6de, 0x51d938fc, 0x8c2a6359, 0x49bb6f7b, 0xd1464290, 0xe07da506, 0x12cabe65, 0x1f16da24, 0xa7582d07, 0x754247d4, 0xbff668a9, 0x8b65994b, 0x0 } },
    { 0, 0, 0, { 0x3fe4c227, 0x5cf99fe5, 0xc638447a, 0xde79169c, 0xe05aac9f, 0x2046943d, 0x773a8a88, 0x5ef20152, 0xdfcc013, 0x3feeb074, 0x4c95f531, 0x84c6ccc4, 0xb50bd4f, 0xaec7f06d, 0x0 } },
    { 0, 0, 0, { 0x38243a5e, 0xdb9c6560, 0x26d910eb, 0x340c4209, 0x48a322e6, 0x5c539008, 0x8705359d, 0xb40c4313, 0x2662e3f2, 0x9458b731, 0x4fa4f394, 0x33e7ce1, 0x17665036, 0x22bb21d8, 0x0 } },
    { (RatHook)PlayerRatCarryStateSetup, (RatHook04)RatHostEnterGunMode, (RatHook)RatHostAnimStart, { 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x672f359c, 0x0 } },
    { (RatHook)PlayerRatHostSlotSetup, (RatHook04)PlayerRatResetGunState, 0, { 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x1379c466, 0x0 } },
    { (RatHook)PlayerRatGunSetup, (RatHook04)PlayerRatFrameUpdate, 0, { 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x1c1a4b85, 0x0 } },
    { (RatHook)PlayerRatHostSetup, (RatHook04)PlayerRatResetMode, 0, { 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x672f1d89, 0x0 } },
    { (RatHook)PlayerRatTeardown, (RatHook04)PlayerRatExitUpdate, 0, { 0x2a1a099, 0xc5d70d57, 0x3be1446c, 0x5514778e, 0xd9df8711, 0xc1bbedaf, 0xe424c17a, 0x2feb2d44, 0x3bdd7485, 0xdc50bd66, 0x1c29e8a3, 0x5bc381b6, 0x1a596fc1, 0xa32e1df, 0x0 } },
    { 0, 0, 0, { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } },
    { 0, 0, 0, { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } },
};

// One entry of the per-action-mode table overlaid on gRatModeTable (0x48 bytes each).
struct RatModeEntry {
    u8 pad000[0x08];
    void (*callback)(Player* player);   // 0x08: per-mode callback
    u8 pad00C[0x38];
    s32 motionId;                       // 0x44: per-mode motion id
};

// 0x801D0918
s32 RatGetModeMotionId(Player* player) {
    RatModeEntry* table = (RatModeEntry*)gRatModeTable;
    return table[player->field7A4].motionId;
}

// 0x801D0934
void RatRunModeCallback(Player* player) {
    RatModeEntry* table = (RatModeEntry*)gRatModeTable;
    if (table[player->field7A4].callback != 0) {
        table[player->field7A4].callback(player);
    }
}

// Function pointers stored at the start of each 0x48-byte per-action-mode table entry.
struct RatModeHooks {
    void (*hook00)(Player* player);     // 0x00
    void (*hook04)(Player* player, int arg); // 0x04
    u8 pad008[0x40];
};

// 0x801D0978
void RatRunModeHook04(Player* player, int arg) {
    RatModeHooks* table = (RatModeHooks*)gRatModeTable;
    void (*fn)(Player*, int) = table[player->field7A4].hook04;
    if (fn != 0) {
        fn(player, arg);
    }
}

// 0x801D09BC
void RatRunModeHook00(Player* player) {
    RatModeHooks* entry = &((RatModeHooks*)gRatModeTable)[player->field7A4];
    if (entry->hook00 != 0) {
        entry->hook00(player);
    }
}

unsigned long AssetFind(unsigned long id, unsigned long hash);

// 0x801D09FC: loads the rat's asset handles. Still called as fn_801D09FC from PlayerInitGunState.
extern "C" void fn_801D09FC(void)
{
    gRatAssets[0] = AssetFind(0xf1754bf2, 0x21c5e0);
    gRatAssets[1] = AssetFind(0xf1754bf2, 0x24805f);
    gRatAssets[2] = AssetFind(0xf1754bf2, 0x101844cc);
    gRatAssets[3] = AssetFind(0xf1754bf2, 0x48874f6e);
    gRatAssets[4] = AssetFind(0xf1754bf2, 0x4a53fc1);
    gRatAssets[5] = AssetFind(0xf1754bf2, 0x967daac6);
    gRatAssets[6] = AssetFind(0xf1754bf2, 0xd9404234);
    gRatAssets[7] = AssetFind(0xf1754bf2, 0x450b2bb);
    gRatAssets[8] = AssetFind(0xf1754bf2, 0x922bf04e);
    gRatAssets[9] = AssetFind(0xf1754bf2, 0x8c73dfa9);
    gRatAssets[10] = AssetFind(0xf1754bf2, 0x90aeeee4);
    gRatAssets[11] = AssetFind(0xf1754bf2, 0x4a2f9aa);
    gRatAssets[12] = AssetFind(0xf1754bf2, 0x4a48bc0);
    gRatAssets[13] = AssetFind(0xf1754bf2, 0x8671db2a);
    gRatAssets[14] = AssetFind(0xf1754bf2, 0x44e6ca4);
    gRatAssets[15] = AssetFind(0xf1754bf2, 0x44ffeba);
}

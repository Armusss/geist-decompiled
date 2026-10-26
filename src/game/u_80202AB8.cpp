// u_80202AB8.cpp: "host" actor possession state machine (0x80202AB8-0x802052B4).
// Names are descriptive guesses; the code is matched against the retail DOL.
//
// HostActorBehaviourUpdate is the actor behaviour callback: it runs a small state
// machine that starts/restarts an aim timer, detaches attached actors, and finally
// plays the "grab"/"release" animations. HostActorAimTimerCheck is the delayed
// timer callback that checks whether the aim target is still in front, in range
// and of the expected type, then sets one of the behaviour flag bits.

#include "dolphin/types.h"

struct Vec3i {
    s32 x;                          // 0x00
    s32 y;                          // 0x04
    s32 z;                          // 0x08
};

struct Vec3 {
    f32 x;                          // 0x00
    f32 y;                          // 0x04
    f32 z;                          // 0x08
};

struct ActorModel {
    u8 unk00[0x34];
    f32 unk34;                      // 0x34
    f32 unk38;                      // 0x38
    f32 unk3c;                      // 0x3c
    f32 unk40;                      // 0x40
    u8 unk44[0x94 - 0x44];
    s32 unk94;                      // 0x94
    s32 unk98;                      // 0x98
    s32 unk9c;                      // 0x9c
    s32 unkA0;                      // 0xa0
    u8 unkA4[0xac - 0xa4];
    u32 unkAC;                      // 0xac
    u8 unkB0[0xb8 - 0xb0];
    u32 unkB8;                      // 0xb8
    u8 unkBC[0xc4 - 0xbc];
    s32 unkC4;                      // 0xc4
    s32 unkC8;                      // 0xc8
    u8 unkCC[0xd4 - 0xcc];
    u32 unkD4;                      // 0xd4
    u32 unkD8;                      // 0xd8
};

struct HostBehaviour {
    s32 state;                      // 0x00
    u32 flags;                      // 0x04
    f32 unk08;                      // 0x08
    s32 unk0c;                      // 0x0c
    s32 unk10;                      // 0x10
    void* unk14;                    // 0x14
};

struct AttachSlot {
    s32 unk0;                       // 0x00
    void* unk4;                     // 0x04
};

struct Actor {
    u8 unk00[0x24];
    Vec3i unk24;                    // 0x24
    u8 unk30[0x38 - 0x30];
    u32 unk38;                      // 0x38
    u8 unk3c[0x40 - 0x3c];
    void* unk40;                    // 0x40
    u8 unk44[0x50 - 0x44];
    u8 unk50[0x5c - 0x50];          // 0x50
    u8 unk5c[0x84 - 0x5c];
    ActorModel* model;              // 0x84
    u8 unk88[0x8c - 0x88];
    u32 unk8c;                      // 0x8c
    u32 unk90;                      // 0x90
    u8 unk94[0x9c - 0x94];
    u32 unk9c;                      // 0x9c
    u8 unkA0[0xa4 - 0xa0];
    HostBehaviour* behaviour;       // 0xa4
    u8 unkA8[0xac - 0xa8];
    u32 unkAC;                      // 0xac
    u8 unkB0[0xc4 - 0xb0];
    void* unkC4;                    // 0xc4
    u8 unkC8[0x110 - 0xc8];
    s32 unk110;                     // 0x110
    u8 unk114[0x124 - 0x114];
    s32 unk124;                     // 0x124
    AttachSlot* unk128;             // 0x128
    u8 unk12c[0x14f8 - 0x12c];
    s32 unk14f8;
    u8 unk14fc[0x26c8 - 0x14fc];
    void* unk26c8;                  // 0x26c8
    u8 unk26cc[0x2740 - 0x26cc];
    s32 unk2740;
};

struct Mtx34 {
    f32 m[12];                      // 0x00
};

void ActorGetMatrix(Mtx34* out, Actor* actor);
int ModelBoneHitBox(int a, int b, int c, int d);
int ModelBoneHitNearest(int a, int b, int handle, int asset, int* out, Vec3* v0, Vec3* v1,
                        unsigned int* flags);
int ModelAnimate(int handle, char* name, int flag);
int ActorInitBase(Actor* actor);
void* VAlloc(int size);
void FearObjAdd(void* obj);
extern "C" int fn_80019BA8(void* link, Actor* actor);
void* memset(void* dst, int value, unsigned int size);

// reply buffer of the 0x400 "where is actor X" message
struct HostMsg400 {
    Actor* obj;                     // 0x00
    s32 unk04;                      // 0x04
    s32 unk08;                      // 0x08
    s32 flag;                       // 0x0c
};

// request/reply buffer of the 0x41e "current type-name actor" message
struct TypeNameMsg {
    s32 id;                         // 0x00
    u32 hash;                       // 0x04
    s32 unk08;                      // 0x08
};

// payload of the 0x40b "play this grabbed-animation" message
struct HostQueryAnimMsg {
    s32 unk00;                      // 0x00
    s32 unk04;                      // 0x04
    s32 unk08;                      // 0x08
    s32 asset;                      // 0x0c
};

class Buffer {
public:
    ~Buffer();
};

class String : public Buffer {
public:
    String();
    String(const char* text);
    virtual ~String();

    const char* text;               // 0x04
    s32 length;                     // 0x08
    u8 unk0c[0x14 - 0x0c];          // 0x0c
};

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int fn_8001ED98(void* seq, int frames, int (*cb)(Actor*, Actor*), Actor* owner,
                           int mode);
extern "C" int fn_8001EAF8(void* seq, int frames, int (*cb)(Actor*, Actor*, float), Actor* owner,
                           int mode);
extern "C" void fn_8001BA6C(void* seq);
extern "C" void fn_8001BA30(void* handle, int value);
extern "C" void fn_801D556C(void* link);
extern "C" int fn_802A1D04();
extern "C" int fn_80007138();
extern "C" void fn_80090974(int a, int b, f32 c, int d, int e, int f, int g);
extern "C" void fn_80090390(int handle, int value);
extern "C" u32 fn_800e4c64(int from, int msg, int* data, int arg);
extern "C" u8* fn_80049948();
extern "C" u32 fn_8003aa9c(u32 handle, int a, int b);
extern "C" u32 fn_8003a1f0(u32 handle, u32 unk, u32 asset);
extern "C" void fn_8016d864(void* out, void* src);
extern "C" void fn_8009ed70(void* out, void* actor);
extern "C" u32 fn_8002d4e0(u32 handle, void* mat, void* info, u32 asset, int a, int b, int c,
                           int d, int e, u32 flags, int f, int g);
extern "C" s32 fn_8007eb34(u32 id, u32 kind);
extern "C" u8 __upper_map[];
extern "C" u32 fn_8028C400(String* out, const char* text);
extern "C" void fn_800225C8(String* out, String* a, String* b);
extern "C" void fn_8028C188(int* out, const char* text, s32 length);
extern "C" const char* lbl_803BA130[];
extern "C" char lbl_80349501[];
extern "C" int fn_80019F88(int* data, Actor* actor);
extern "C" void fn_800E48A0(f32 value, Actor* actor);

extern void ActorUpdate(Actor* actor, float dt);
extern int ActorPlayAnim(int id, u32 anim, int flag);
extern void ActorAim(int* out, Actor* actor, int angle);
extern Actor* GetPlayer();
extern Actor* PlayerInHost(Actor* actor);
extern void Fn8008E33C(int a, int b, float c);

// other functions of this unit
void HostActorReset(Actor* actor);          // 0x80204804
void HostActorBegin(Actor* actor);          // 0x80204e1c
int HostActorTimerDone(Actor* host, Actor* actor);  // 0x80203910
int HostActorAimTimerCheck(Actor* host, Actor* actor, float dist);

// still under their placeholder names (drafted below)
u32 HostActorStateQueryMsg(int* data);      // 0x80203f00
u32 HostActorStateSetMsg(int* data);        // 0x80204118
u32 HostActorHitTest(int* data);            // 0x80204448
extern "C" void fn_80204A6C(Actor* actor, u32 id);  // 0x80204a6c

extern s32 lbl_804D3398;
extern s32 lbl_804D5390;
extern s32 lbl_804D5394;
extern s32 lbl_804D5398;
extern f64 lbl_804D9238;
extern f32 lbl_804D9240;
extern f32 lbl_804D9244;
extern f32 lbl_804D9248;
extern f32 lbl_804D924C;
extern f32 lbl_804D9250;

extern "C" float fabsf(float x);
extern "C" float sqrtf(float x);

// Releases every actor attached to this one and clears the "linked" bit.
static inline void DetachHostLinks(Actor* actor)
{
    if ((actor->behaviour->flags & 0x400) == 0 && actor != 0 && actor->unk128 != 0) {
        HostBehaviour* beh = actor->behaviour;
        int ofs = 0;

        for (int i = 0; i < actor->unk124; i++) {
            fn_801D556C(*(void**)((u8*)actor->unk128 + ofs + 4));
            ofs += 8;
            beh->flags &= ~2;
        }
    }
}

// (Re)starts the aim/range timer for the current hold duration.
static inline void StartHostAimTimer(Actor* actor, HostBehaviour* beh)
{
    f32 delay = (f32)beh->unk10;

    if (delay <= actor->model->unk34)
        delay = actor->model->unk34;
    beh->flags &= ~0x700;
    fn_8001EAF8(&actor->unk24, (int)delay, HostActorAimTimerCheck, actor, 0);
}

// Normalizes a movement direction; the direction helpers of this game work on the
// horizontal (x/y) plane only, so z is pinned to zero first.
static inline void NormalizePlaneDir(Vec3* v)
{
    if (fabsf(v->x) > lbl_804D9248 || fabsf(v->y) > lbl_804D9248 ||
        fabsf(v->z) > lbl_804D9248) {
        f32 inv = 1.0f / sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
        v->x *= inv;
        v->y *= inv;
        v->z *= inv;
    } else {
        v->x = lbl_804D9244;
        v->y = lbl_804D9244;
        v->z = lbl_804D9244;
    }
}

static inline f32 PlaneDot(const Vec3* a, const Vec3* b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void HostActorBehaviourUpdate(Actor* actor, float dt)
{
    HostBehaviour* beh = actor->behaviour;
    bool done = false;

    switch (beh->state) {
    case 0:
        if ((beh->flags & 4) == 0 && beh->unk10 > 0)
            fn_8001ED98(&actor->unk24, beh->unk10, HostActorTimerDone, actor, 0);
        if (beh->flags & 2) {
            StartHostAimTimer(actor, beh);
            DetachHostLinks(actor);
        }
        if (lbl_804D3398 == (s32)actor->unk38) {
            lbl_804D3398 = -1;
            lbl_804D5394 = 0;
        }
        break;

    case 3:
        if ((beh->flags & 8) == 0) {
            StartHostAimTimer(actor, beh);
            if ((beh->flags & 0x100) == 0 && (beh->flags & 0x200) == 0) {
                HostActorReset(actor);
            } else if ((beh->flags & 4) == 0 && (beh->flags & 0x200) == 0 &&
                       lbl_804D5398 == 1) {
                HostActorReset(actor);
            } else if (beh->flags & 2) {
                DetachHostLinks(actor);
            }
        } else if (beh->flags & 2) {
            StartHostAimTimer(actor, beh);
            DetachHostLinks(actor);
        }
        break;

    case 6:
        if (lbl_804D5390 == (s32)actor->unk38 && lbl_804D5394 == 0) {
            beh->state = 7;
            fn_8001BA6C(&actor->unk24);
        }
        break;

    case 8:
        if (lbl_804D5390 == (s32)actor->unk38) {
            if (lbl_804D5394 == 0 || fn_802A1D04() != 0) {
                beh->state = 9;
                fn_8001BA6C(&actor->unk24);
            }
        } else {
            HostActorReset(actor);
        }
        break;

    case 9:
        if (lbl_804D5390 == (s32)actor->unk38) {
            if (fn_80007138() == 0 || fn_802A1D04() != 0) {
                beh->state = 10;
                HostActorBegin(actor);
                lbl_804D5394 = 1;
                lbl_804D3398 = (s32)actor->unk38;
            }
        } else {
            HostActorReset(actor);
            lbl_804D5394 = 0;
            lbl_804D3398 = (s32)actor->unk38;
        }
        break;

    case 11:
        if (lbl_804D5390 == (s32)actor->unk38 || fn_802A1D04() != 0) {
            if ((beh->flags & 8) == 0) {
                StartHostAimTimer(actor, beh);
                if ((beh->flags & 0x100) == 0 && (beh->flags & 0x200) == 0) {
                    HostActorReset(actor);
                } else if ((beh->flags & 4) == 0 && (beh->flags & 0x200) == 0 &&
                           lbl_804D5398 == 1) {
                    HostActorReset(actor);
                } else if (beh->flags & 2) {
                    DetachHostLinks(actor);
                }
            } else if (beh->flags & 2) {
                StartHostAimTimer(actor, beh);
                DetachHostLinks(actor);
            }
        } else {
            HostActorReset(actor);
        }
        break;

    default:
        break;
    }

    if (beh->state != 0 && (beh->flags & 8) == 0 && fn_802A1D04() == 0) {
        if (PlayerInHost(GetPlayer()) == 0 || GetPlayer()->unk2740 != 0)
            done = true;
    }
    if (beh->state != 0 && (beh->flags & 8) == 0 && fn_802A1D04() == 0) {
        if (GetPlayer()->unk14f8 > 0)
            done = true;
    }

    if (done) {
        int anim;

        beh->state = 0;
        anim = actor->model->unkC8;
        if (anim != -1)
            fn_80090974(actor->unk110, 0, lbl_804D9240, anim, 0x50, 0, 0);
        else
            fn_80090974(actor->unk110, 0, lbl_804D9240, actor->model->unk98, 0x50, 0, 0);
        Fn8008E33C(actor->unk110, 0, lbl_804D9240);
        ActorUpdate(actor, dt);
        fn_80090390(actor->unk110, 0);
        actor->unkAC |= 0x80000000;
        ActorPlayAnim((int)actor->unk38, 0xC62BD5D6, beh->unk0c);
        ActorPlayAnim((int)actor->unk38, 0xA34BBC0B, beh->unk0c);
        fn_8001BA30(beh->unk14, 0);
        if (lbl_804D5390 == (s32)actor->unk38) {
            fn_8001BA6C(0);
            lbl_804D5390 = 0;
            lbl_804D5394 = 0;
            lbl_804D3398 = (s32)actor->unk38;
        }
    }

    ActorUpdate(actor, dt);
}

// Delayed aim/range timer callback. `host` is the actor that started the timer,
// `actor` is the owner of the behaviour being ticked.
int HostActorAimTimerCheck(Actor* host, Actor* actor, float dist)
{
    HostBehaviour* beh;
    HostMsg400 pos1;
    HostMsg400 pos2;
    TypeNameMsg query;
    Vec3 dir;
    Vec3 v1;
    Vec3 v2;
    int aim[3];
    f32 maxd;
    bool visible = true;

    if (host == 0)
        return 0;

    beh = actor->behaviour;
    if (SendMsg((int)host->unk38, 0x400, &pos1) == 0)
        return 0;
    if (pos1.obj == 0)
        return 0;

    if ((host->unk38 >> 24) == 0x13) {
        maxd = (f32)beh->unk10;
    } else if ((host->unk38 >> 24) == 2) {
        int angle = (int)actor->model->unk40;

        ActorAim(aim, actor, angle);
        dir.x = (f32)aim[0];
        dir.y = (f32)aim[1];
        dir.z = (f32)aim[2];
        dir.z = lbl_804D9244;
        NormalizePlaneDir(&dir);

        v1.x = (f32)(pos1.obj->unk24.x - actor->unk24.x);
        v1.y = (f32)(pos1.obj->unk24.y - actor->unk24.y);
        v1.z = (f32)(pos1.obj->unk24.z - actor->unk24.z);
        v1.z = lbl_804D9244;
        NormalizePlaneDir(&v1);

        {
            String name;  // name buffer owned by the 0x41e message

            query.id = -1;
            query.hash = 0x497F770D;
            query.unk08 = 0;
            if (SendMsg((int)host->unk38, 0x41e, &query) != 0 && query.id != -1) {
                if (SendMsg(query.id, 0x400, &pos2) != 0 && pos2.flag == 1 && pos2.obj != 0) {
                    v2.x = (f32)(pos2.obj->unk24.x - actor->unk24.x);
                    v2.y = (f32)(pos2.obj->unk24.y - actor->unk24.y);
                    v2.z = (f32)(pos2.obj->unk24.z - actor->unk24.z);
                    v2.z = lbl_804D9244;
                    NormalizePlaneDir(&v2);
                    if (PlaneDot(&dir, &v1) * PlaneDot(&dir, &v2) < lbl_804D9244)
                        visible = false;
                }
            }
        }

        if (PlaneDot(&dir, &v1) < lbl_804D9244) {
            if (beh->flags & 1)
                maxd = actor->model->unk34;
            else
                maxd = actor->model->unk38;
        } else {
            if (beh->flags & 1)
                maxd = actor->model->unk38;
            else
                maxd = actor->model->unk34;
        }
    } else {
        return 0;
    }

    maxd *= maxd;
    if (dist > maxd) {
        if (visible)
            return 0;
    }

    if ((host->unk38 >> 24) == 2) {
        beh->flags |= 0x100;
    } else if ((host->unk38 >> 24) == 0x13) {
        if (pos1.flag != 0)
            beh->flags |= 0x200;
        else
            beh->flags |= 0x400;
    }
    return 0;
}

// Delayed "grab" timer callback: re-queries the host's aim target and aborts the
// hold when the 0x41e type-name query says the target is no longer the one grabbed.
int HostActorTimerDone(Actor* host, Actor* actor)
{
    HostBehaviour* beh = actor->behaviour;

    if (host != 0 && (host->unk38 >> 24) == 0x13) {
        HostMsg400 pos;

        if (SendMsg((int)host->unk38, 0x400, &pos) != 0 && pos.flag == 1 && pos.obj != 0) {
            if ((beh->flags & 0x80) == 0) {
                String name;
                TypeNameMsg query;

                query.hash = 0x88B4A252;
                query.unk08 = 0;
                if (SendMsg((int)host->unk38, 0x41e, &query) != 0 && query.id == 0xD028AC2)
                    return 1;
            }
            fn_80204A6C(actor, host->unk38);
        }
    }
    return 1;
}

// Folds the upper-cased name into the running hash used by the message system.
static inline u32 HostNameHash(const char* name, u32 hash)
{
    if (name == 0)
        return 0;

    const char* p = name;

    while (*p != 0) {
        u32 c = (*p == (char)0xff) ? 0xffffffff : __upper_map[(u8)*p];

        hash *= 31;
        hash += c & 0xff;
        p++;
    }
    return hash;
}

// Actor message handler of the host behaviour: dispatches the ids the host owns and
// hands everything else to the shared actor message handler.
u32 HostActorMessage(int from, int msg, int* data, int arg)
{
    u32 result = 0;

    if (msg == 0x41e) {
        result = HostActorStateQueryMsg(data);
    } else if (msg < 0x41e) {
        if (msg == 0x409) {
            Actor* actor = (Actor*)data[0];
            HostBehaviour* beh = actor->behaviour;

            if (((u32)data[2] >> 24) == 2 && (beh->flags & 4) != 0) {
                ActorPlayAnim((int)actor->unk38, 0xB2CC0DA9, data[2]);
            } else {
                fn_80204A6C(actor, (u32)data[2]);
                beh->flags |= 0x10;
            }
            result = 1;
        } else if (msg < 0x409) {
            if (msg == 4) {
                u8* state;

                if (fn_802A1D04() == 0) {
                    state = fn_80049948();
                    if (*(s32*)(state + 0x2740) != 0) {
                        Actor* actor = (Actor*)data[0];
                        u32 flags = (u32)data[6];

                        if ((actor->unkAC & 0x20000000) != 0)
                            return 0;
                        {
                            u32 handle = actor->unk90;
                            u8 mat[24];
                            u8 info[60];
                            u32 asset = fn_8003aa9c(handle, 0, 0);

                            asset = fn_8003a1f0(handle, actor->unk9c, asset);
                            fn_8016d864(mat, actor->unk50);
                            fn_8009ed70(info, actor);
                            return fn_8002d4e0(handle, mat, info, asset, data[1], data[2],
                                              data[3], data[4], data[5],
                                              (flags & 0xEFFFFFFF) | 0x20000000, data[0xb],
                                              data[0xc]);
                        }
                    }
                }
            } else if (msg == 1) {
                result = HostActorHitTest(data);
            }
        } else if (msg < 0x41d) {
            if (msg < 0x40b) {
                Actor* actor = (Actor*)data[0];
                int target = data[1];
                HostBehaviour* beh = actor->behaviour;
                String name;
                TypeNameMsg query;

                query.hash = 0x947E97;
                query.unk08 = 0;
                SendMsg(target, 0x41e, &query);

                if (ActorPlayAnim((int)actor->unk38, HostNameHash("QUERY", (u32)data[2]),
                                  target) != 0) {
                    result = 1;
                } else if (beh->state == 0 && (beh->flags & 0x20) == 0 &&
                           data[2] == 0x7C44EC3D && query.id == 1) {
                    HostQueryAnimMsg animMsg;
                    s32 asset;

                    animMsg.unk04 = 0;
                    data[3] = (s32)0xF21581DE;
                    animMsg.unk08 = data[3];
                    animMsg.asset = -1;
                    if ((beh->flags & 4) == 0)
                        asset = fn_8007eb34(0xC0A77187, 0x251E4A);
                    else
                        asset = fn_8007eb34(0xC0A77187, 0x8629270A);
                    animMsg.asset = asset;
                    SendMsg(target, 0x40b, &animMsg);
                    result = 1;
                } else {
                    result = 1;
                }
            }
        } else {
            result = HostActorStateSetMsg(data);
        }
    } else if (msg == 0x3000) {
        Actor* actor = (Actor*)data[0];

        fn_8001BA30(actor->behaviour->unk14, 1);
        result = 0;
    } else if (msg < 0x3000) {
        if (msg == 0xc14) {
            Actor* actor = (Actor*)data[0];
            HostBehaviour* beh = actor->behaviour;
            s32 id = data[3];

            if (id == 0x2072074C) {
                fn_80204A6C(actor, (u32)data[2]);
                beh->flags |= 0x10;
            }
            result = (id == 0x2072074C);
        } else if (msg < 0xc14) {
            if (msg == 0xc00) {
                Actor* actor = (Actor*)data[0];
                HostBehaviour* beh = actor->behaviour;

                if (data[1] == 0) {
                    fn_8001BA30(beh->unk14, 1);
                } else if (beh->state == 0) {
                    fn_8001BA30(beh->unk14, 0);
                } else {
                    fn_8001BA30(beh->unk14, 1);
                }
                result = 0;
            }
        }
    } else if (msg == 0x3001 || msg == 0x300b) {
        Actor* actor = (Actor*)data[0];
        HostBehaviour* beh = actor->behaviour;

        if (beh->state == 0)
            fn_8001BA30(beh->unk14, 0);
        else
            fn_8001BA30(beh->unk14, 1);
        result = 0;
    }

    if (result == 0)
        result = fn_800e4c64(from, msg, data, arg);
    return result;
}

// 0x80203f00: the 0x41e "state query" handler of the host behaviour. The caller
// passes an actor in data[0], a query hash in data[1] and reads the answer from
// data[3]. The 0xAD8576FA query answers the hash of the current state name.
u32 HostActorStateQueryMsg(int* data)
{
    Actor* actor = (Actor*)data[0];
    HostBehaviour* beh = actor->behaviour;
    u32 query = (u32)data[1];

    switch (query) {
    case 0xAD8576FA: {
        String stateName;
        String separator;
        String joined;

        fn_8028C400(&stateName, lbl_803BA130[beh->state]);
        fn_8028C400(&separator, lbl_80349501);
        fn_800225C8(&joined, &stateName, &separator);
        fn_8028C188(&data[3], joined.text, joined.length);
        return 1;
    }
    case 0x8AB5B526:
        data[3] = (beh->flags & 0x20) != 0;
        return 1;
    case 0x8629270A:
        data[3] = (beh->flags & 4) != 0;
        return 1;
    case 0x914B98A8:
        data[3] = (s32)actor->model->unk40;
        return 1;
    case 0x4707BB10:
        data[3] = beh->unk10;
        return 1;
    case 0x251E4A: {
        s32 state = beh->state;

        if (state == 2 || (u32)(state - 3) < 2 || (u32)(state - 10) < 2 || state == 12)
            data[3] = 1;
        else
            data[3] = 0;
        return 1;
    }
    default:
        return 0;
    }
}

// 0x80204118: the setter counterpart of the 0x41e handler: data[0] is the actor,
// data[1] the request hash and data[3] the new value. It toggles the behaviour
// flag bits, stores the hold duration and runs the full release sequence for the
// 0x438A0843 request.
u32 HostActorStateSetMsg(int* data)
{
    Actor* actor = (Actor*)data[0];
    HostBehaviour* beh = actor->behaviour;
    u32 query;

    if (fn_80019F88(data, actor) != 0)
        return 1;

    query = (u32)data[1];
    switch (query) {
    case 0x251E4A:
        if (data[3] == 0) {
            beh->flags &= ~8;
            HostActorReset(actor);
        } else {
            beh->flags |= 8;
            beh->flags |= 0x10;
            fn_80204A6C(actor, 0xffffffff);
        }
        break;
    case 0x8AB5B526:
        if (data[3] == 0)
            beh->flags &= ~0x20;
        else
            beh->flags |= 0x20;
        return 1;
    case 0x8629270A:
        if (data[3] == 0)
            beh->flags &= ~4;
        else
            beh->flags |= 4;
        return 1;
    case 0xEA8A9C98:
        if (data[3] == 0)
            beh->flags &= ~0x80;
        else
            beh->flags |= 0x80;
        return 1;
    case 0x4707BB10:
        beh->unk10 = data[3];
        return 1;
    case 0x438A0843: {
        s32 anim;

        beh->state = 0;
        anim = actor->model->unkC8;
        if (anim != -1)
            fn_80090974(actor->unk110, 0, lbl_804D9240, anim, 0x50, 0, 0);
        else
            fn_80090974(actor->unk110, 0, lbl_804D9240, actor->model->unk98, 0x50, 0, 0);
        Fn8008E33C(actor->unk110, 0, lbl_804D9240);
        fn_800E48A0(lbl_804D924C, actor);
        fn_80090390(actor->unk110, 0);
        actor->unkAC |= 0x800000;
        ActorPlayAnim((int)actor->unk38, 0xC62BD5D6, beh->unk0c);
        ActorPlayAnim((int)actor->unk38, 0xA34BBC0B, beh->unk0c);
        fn_8001BA30(beh->unk14, 0);
        if (lbl_804D5390 == (s32)actor->unk38) {
            fn_8001BA6C(0);
            lbl_804D5390 = 0;
            lbl_804D5394 = 0;
            lbl_804D3398 = (s32)actor->unk38;
        }
        return 1;
    }
    case 0x4F7F0EE6:
        if (data[3] == 0) {
            beh->flags |= 0x40;
            beh->flags &= ~8;
            HostActorReset(actor);
        } else {
            beh->flags |= 0x40;
            beh->flags |= 8;
            beh->flags |= 0x10;
            fn_80204A6C(actor, 0xffffffff);
        }
        break;
    default:
        break;
    }
    return 0;
}

// 0x80204448: "grabbed actor hit" message handler (message 0x1). Rejects the
// message when the owner is in the released state, optionally separates the two
// actors when their bone hit boxes overlap, then queries the nearest bone hit
// point of the grabbed actor and stores the result in the message buffer.
u32 HostActorHitTest(int* data)
{
    Actor* actor = (Actor*)data[0];
    HostBehaviour* beh = actor->behaviour;

    if ((actor->unkAC & 0x20000000) != 0)
        return 0xFFF0BDC1;

    if (actor->model->unk3c != lbl_804D9244 && (beh->flags & 4) == 0) {
        Actor* target = (Actor*)data[8];

        if (target != 0 && (data[2] & 0x4000000) != 0) {
            void* type = 0;
            u32 kind = target->unk38 >> 24;

            if (kind == 2)
                type = target->unk26c8;
            else if (kind == 0x13)
                type = target->unkC4;

            if (type != 0) {
                u32 id = *(u32*)type;

                if (id != 0x04624485 && id != 0x00013C05 && id != 0xE0AB9B4F &&
                    id != 0x0000FFF5) {
                    u8 hitSelf[0x18];
                    u8 hitTarget[0x18];
                    Mtx34 mtxSelf;
                    Mtx34 mtxTarget;

                    fn_8016d864(hitSelf, actor->unk40);
                    fn_8016d864(hitTarget, target->unk40);
                    ActorGetMatrix(&mtxSelf, actor);
                    ActorGetMatrix(&mtxTarget, target);
                    if (ModelBoneHitBox((int)hitSelf, (int)&mtxSelf, (int)hitTarget,
                                        (int)&mtxTarget) != 0)
                        fn_80204A6C(actor, target->unk38);
                }
            }
        }
    }

    data[7] = (int)actor->unk90;
    {
        s32* src = (s32*)data[1];
        Vec3 pos;
        Vec3 dir;
        Vec3 point;
        Vec3 range;
        s32 asset = ModelAnimate((int)actor->unk90, 0, 0);

        asset = (s32)fn_8003a1f0(actor->unk90, actor->unk9c, (u32)asset);

        pos.x = (f32)src[0];
        pos.y = (f32)src[1];
        pos.z = (f32)src[2];
        dir.x = (f32)src[3];
        dir.y = (f32)src[4];
        dir.z = (f32)src[5];

        if (ModelBoneHitNearest((int)&pos, (int)&dir, (int)actor->unk90, asset, &data[5],
                                &point, &range, (unsigned int*)&data[3]) == 0)
            return 0xFFF0BDC1;

        data[0xc] = (int)point.x;
        data[0xd] = (int)point.y;
        data[0xe] = (int)point.z;
        data[0xf] = (int)(lbl_804D9250 * range.x);
        data[0x10] = (int)(lbl_804D9250 * range.y);
        data[0x11] = (int)(lbl_804D9250 * range.z);
    }
    return 1;
}

// 0x80204730: registers the host actor: allocates its behaviour block, attaches
// it to the actor and adds the actor to the fear system.
int HostActorInit(Actor* actor)
{
    HostBehaviour* beh;

    lbl_804D5390 = 0;
    lbl_804D5394 = 0;
    lbl_804D3398 = (s32)actor->unk38;

    if (ActorInitBase(actor) == 0) {
        actor->unkAC |= 0x1000000;
        return 0;
    }

    beh = (HostBehaviour*)VAlloc(0x30);
    memset(beh, 0, 0x30);
    beh->state = 0;
    beh->flags = 0;
    beh->unk08 = lbl_804D9244;
    beh->unk0c = -1;
    beh->unk10 = 0;
    actor->behaviour = beh;
    actor->unkAC |= 0x100000;

    beh->unk14 = (void*)fn_80019BA8(actor->unk50, actor);
    fn_8001BA30(beh->unk14, 0);
    FearObjAdd(actor);
    return 1;
}

extern "C" void fn_8016f120(u32 handle, int pos);
extern "C" void fn_800265d4(u32 handle, int msg, void* data);
extern "C" void fn_801D550C(void* link);
extern "C" void fn_800E0CB8(int* v, int* out);
void HostActorHoldTimerDone(int from, Actor* actor);

// request block of the 0xc12 "restart the hold timer" message
struct HostTimerReq {
    s32 unk00;                      // 0x00
    s32 hold;                       // 0x04
    s32 unk08;                      // 0x08
    s32 unk0c;                      // 0x0c
    s32 unk10;                      // 0x10
    void (*callback)(int, Actor*);  // 0x14
    Actor* owner;                   // 0x18
};

// 0x80204804: aborts or restarts the current hold. It runs from the behaviour
// state machine and from the message handlers when a hold has to be abandoned
// (the target is gone, the host was released, the grabbed actor was hit, ...).
void HostActorReset(Actor* actor)
{
    HostBehaviour* beh = actor->behaviour;
    s32 hold;
    u32 sound;

    if (beh->state == 0 || (beh->flags & 8) != 0)
        return;

    if (beh->state == 0xb || (u32)(beh->state - 2) < 2 || beh->state == 10) {
        beh->state = (u32)(beh->state - 2) < 2 ? 4 : 0xc;
        if ((beh->flags & 1) == 0 || (hold = actor->model->unkA0) == -1)
            hold = actor->model->unk98;
        ActorPlayAnim((int)actor->unk38, 0xC62BD5D6, beh->unk0c);
        sound = actor->model->unkB8;
    } else {
        beh->state = 5;
        hold = actor->model->unkC8;
        sound = actor->model->unkD8;
        fn_8001BA30(beh->unk14, 0);
    }

    if (hold < 0) {
        beh = actor->behaviour;
        if (beh->state == 5) {
            beh->state = 0;
            ActorPlayAnim((int)actor->unk38, 0xA34BBC0B, beh->unk0c);
        } else if (beh->state < 5) {
            if (beh->state > 3) {
                beh->state = 5;
                HostActorReset(actor);
            }
        } else if (beh->state == 0xc) {
            fn_8001BA6C(0);
            lbl_804D5394 = 0;
            lbl_804D3398 = (s32)actor->unk38;
            beh->state = 5;
            HostActorReset(actor);
        }
    } else {
        HostTimerReq req;

        req.unk00 = 0;
        req.hold = hold;
        req.unk08 = 0;
        req.unk0c = 0x10;
        req.unk10 = 0x1000;
        req.callback = HostActorHoldTimerDone;
        req.owner = actor;

        beh = actor->behaviour;
        if (actor != 0 && actor->unk128 != 0) {
            int ofs = 0;

            for (int i = 0; i < actor->unk124; i++) {
                fn_801D550C(*(void**)((u8*)actor->unk128 + ofs + 4));
                ofs += 8;
                beh->flags |= 2;
            }
        }

        fn_800265d4(actor->unk8c, 0xc12, &req);

        if (beh->state == 4 || beh->state == 0xc) {
            if ((beh->flags & 0x40) == 0)
                fn_8016f120(sound, (int)&actor->unk24);
            beh->flags &= ~0x40;
        }
    }
}

extern u8* OwnerGetInfo(u32 id);
extern "C" void fn_800A0168(void* dst, void* src);
extern "C" void fn_800A0030(void* out, void* mtx, void* in);
extern "C" int fn_8001e9d4(void* start, void* out, int count);
extern f32 lbl_804D9254;

// 0x80204a6c: starts a hold on `id`. It verifies that the owner is still the
// host's current target, checks the line of sight between the two collision
// volumes, then switches the behaviour into the grab state (6 for a possessed
// actor, 1 otherwise), runs the begin sequence and plays the grab animation.
extern "C" void fn_80204A6C(Actor* actor, u32 id)
{
    u8* info = OwnerGetInfo(id);
    HostBehaviour* beh = actor->behaviour;

    if ((beh->flags & 8) == 0 && PlayerInHost(GetPlayer()) == 0)
        return;

    if (beh->state != 0)
        return;

    if ((beh->unk0c >> 24) == 2)
        lbl_804D5398 = 1;

    if ((beh->unk0c >> 24) == 2 && lbl_804D5394 == 1 && fn_802A1D04() == 0)
        return;

    if (info != 0 && (*(u32*)(info + 0x38) >> 24) == 2) {
        Mtx34 mtx;
        Vec3 tmp;
        Vec3i hits[8];
        s32 start[3];
        s32 end[3];
        u8* vol;
        f32 px;
        f32 py;
        f32 pz;
        int count;
        int first;

        ActorGetMatrix(&mtx, (Actor*)info);
        fn_800A0168(&mtx, &mtx);
        px = (f32)*(s32*)(info + 0x24);
        py = (f32)*(s32*)(info + 0x28);
        pz = (f32)*(s32*)(info + 0x2c);
        fn_800A0030(&tmp, &mtx, &tmp);

        vol = *(u8**)(info + 0x40);
        start[0] = (s32)(px + (f32)*(s32*)(vol + 0xc));
        start[1] = (s32)(py + (f32)*(s32*)(vol + 0x10));
        start[2] = (s32)(pz + (f32)*(s32*)(vol + 0x14));

        count = fn_8001e9d4(start, hits, 8);
        if (count < 1)
            return;
        first = hits[0].x;

        ActorGetMatrix(&mtx, (Actor*)info);
        fn_800A0168(&mtx, &mtx);
        px = (f32)*(s32*)(info + 0x24);
        py = (f32)*(s32*)(info + 0x28);
        pz = (f32)*(s32*)(info + 0x2c);
        fn_800A0030(&tmp, &mtx, &tmp);

        vol = *(u8**)(info + 0x40);
        end[0] = (s32)(px + (f32)*(s32*)(vol + 0));
        end[1] = (s32)(py + (f32)*(s32*)(vol + 4));
        end[2] = (s32)(pz + (f32)*(s32*)(vol + 8) + lbl_804D9254);

        count = fn_8001e9d4(end, hits, 8);
        if (count < 1 || hits[0].x != first)
            return;
    }

    beh->unk0c = (s32)id;
    if ((beh->unk0c >> 24) == 2)
        beh->state = 6;
    else
        beh->state = 1;

    HostActorBegin(actor);
    ActorPlayAnim((int)actor->unk38, 0x473ACE79, beh->unk0c);
}

void HostActorBeginTimerDone(int from, Actor* actor);

// Re-attaches the actors already linked to this one (used when a new hold begins).
static inline void AttachHostLinks(Actor* actor)
{
    HostBehaviour* beh = actor->behaviour;

    if (actor != 0 && actor->unk128 != 0) {
        int ofs = 0;

        for (int i = 0; i < actor->unk124; i++) {
            fn_801D550C(*(void**)((u8*)actor->unk128 + ofs + 4));
            ofs += 8;
            beh->flags |= 2;
        }
    }
}

// 0x80204e1c: starts the hold sequence for the target stored in the behaviour:
// picks the hold length and sound, re-attaches the linked actors, restarts the
// hold timer (0xc12) and walks the state machine through its begin states.
void HostActorBegin(Actor* actor)
{
    HostBehaviour* beh = actor->behaviour;
    s32 hold;
    u32 sound;

    if (beh->state == 6 || beh->state == 1) {
        hold = actor->model->unkC4;
        sound = actor->model->unkD4;
        if ((beh->unk0c >> 24) == 2) {
            lbl_804D5390 = (s32)actor->unk38;
            lbl_804D5398 = 0;
        }
    } else {
        beh->flags &= ~1;
        if (beh->unk0c != -1) {
            HostMsg400 pos;

            if (SendMsg(beh->unk0c, 0x400, &pos) != 0 && pos.obj != 0) {
                int aim[3];
                Vec3i d;
                int angle = (int)actor->model->unk40;

                ActorAim(aim, actor, angle);
                aim[2] = 0;
                fn_800E0CB8(aim, aim);

                d.x = pos.obj->unk24.x - actor->unk24.x;
                d.y = pos.obj->unk24.y - actor->unk24.y;
                d.z = pos.obj->unk24.z - actor->unk24.z;
                fn_800E0CB8((int*)&d, (int*)&d);

                if (aim[0] * d.x + aim[1] * d.y + aim[2] * d.z < 0)
                    beh->flags |= 1;
            }
        }
        if ((beh->flags & 1) == 0 || (hold = actor->model->unk9c) == -1)
            hold = actor->model->unk94;
        sound = actor->model->unkAC;
        AttachHostLinks(actor);
        fn_8001BA30(beh->unk14, 1);
    }

    if (hold < 0) {
        HostBehaviour* b = actor->behaviour;

        switch (b->state) {
        case 1:
            b->state = 2;
            HostActorBegin(actor);
            break;
        case 2:
            b->state = 3;
            ActorPlayAnim((int)actor->unk38, 0xB7F9A308, b->unk0c);
            break;
        case 6:
            b->state = 8;
            break;
        case 7:
            b->state = 9;
            break;
        case 10:
            b->state = 0xb;
            ActorPlayAnim((int)actor->unk38, 0xB7F9A308, b->unk0c);
            break;
        }
    } else {
        HostTimerReq req;

        req.hold = hold;
        req.unk08 = 0;
        req.unk0c = 0x10;
        req.unk10 = 0x1000;
        req.callback = HostActorBeginTimerDone;
        req.owner = actor;
        SendMsg(actor->unk8c, 0xc12, &req);

        if ((beh->flags & 0x40) == 0)
            fn_8016f120(sound, (int)&actor->unk24);
        else if (beh->state == 2 || beh->state == 10)
            beh->flags &= ~0x40;
    }
}

// 0x80205124: callback of the 0xc12 "restart hold timer" message. When the
// timer expires it advances the hold state machine and, in the states where the
// hold cannot continue, releases the target through HostActorReset.
void HostActorHoldTimerDone(int from, Actor* actor)
{
    HostBehaviour* beh = actor->behaviour;

    if (from == 0x1B83833F) {
        s32 state = beh->state;

        if (state == 5) {
            beh->state = 0;
            ActorPlayAnim((int)actor->unk38, 0xA34BBC0B, beh->unk0c);
        } else if (state < 5) {
            if (state > 3) {
                beh->state = 5;
                HostActorReset(actor);
            }
        } else if (state == 0xc) {
            fn_8001BA6C(0);
            lbl_804D5394 = 0;
            lbl_804D3398 = (s32)actor->unk38;
            beh->state = 5;
            HostActorReset(actor);
        }
    }
}

// 0x802051f0: callback of the 0xc12 "restart hold timer" message that
// HostActorBegin starts for a held target. On expiry it advances the hold state
// machine by one step and plays the grab animation in the states that need it.
void HostActorBeginTimerDone(int from, Actor* actor)
{
    HostBehaviour* beh = actor->behaviour;

    if (from == 0x1B83833F) {
        switch (beh->state) {
        case 1:
            beh->state = 2;
            HostActorBegin(actor);
            break;
        case 2:
            beh->state = 3;
            ActorPlayAnim((int)actor->unk38, 0xB7F9A308, beh->unk0c);
            break;
        case 6:
            beh->state = 8;
            break;
        case 7:
            beh->state = 9;
            break;
        case 10:
            beh->state = 0xb;
            ActorPlayAnim((int)actor->unk38, 0xB7F9A308, beh->unk0c);
            break;
        }
    }
}

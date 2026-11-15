// u_802C8900.cpp: actor FX emitter: update, init, message handler and spawn helpers (0x802C8900-0x802C9818).
// Names are descriptive guesses; the code is matched against the retail DOL.

// Actor helpers for the effect-emitter state kept in Actor::field_0xA4.
// Names are descriptive guesses.

typedef unsigned char u8;
typedef unsigned long u32;
typedef float f32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};


class Actor;
struct Mtx34 { f32 m[12]; };

class Buffer {
public:
    virtual ~Buffer();

    char* data;
    int length;
    int unk0C;
    int owned;
};

class String : public Buffer {
public:
    String();
    ~String() {}
    void Format(const char* fmt, ...);
};

class Skin;

extern "C" {






    extern f32 lbl_804D3AD0;
    extern f32 lbl_804D3AD4;
    extern f32 lbl_804D57C8;
    extern int lbl_804D57CC;
    extern int lbl_804D3408;
    extern u8  lbl_803C4FE0[];

    u32  fn_800E7890(Actor* actor, int* out);
    u32  fn_800390D8(u32 model, u32 a);
    u32  fn_8003A1F0(u32 a, u32 b, u32 c);
    int  fn_80158AFC(u32 id, void* data);
    f32  fn_80115F68(u32 id);
    void fn_801FAD18(Actor* actor, int flag);
    void fn_8003F15C(u32 a, u32 b, u32 c, void* d, void* e, float alpha);

    void* memset(void* dst, int val, unsigned long size);
    int SendMsg(int to, int msg, void* data);

    int  fn_80212730(u32 x);
    void fn_8021284C(u32 x);
    void fn_801D55CC(u32 handle);
    u32  fn_801D5B38(f32 x, f32 y, f32 z, u32 a, int b, u32 c, u32 d, u32 e, void* view, int f);
    void fn_801D52E0(u32 fx, void* m);
    void fn_801D5298(u32 fx, void* v);
    void fn_801D5260(u32 fx, void* v);
    void fn_801D5110(u32 fx, int v);
    void fn_801D556C(u32 fx);
    void fn_800A0168(void* a, void* b);
    void fn_8016D864(void* out, u32 v);
    void fn_8016D6BC(void* out, void* in);
    void fn_801D902C(void* mtx, u32 target, Actor* self, void* view);
    void fn_8028B9C0(void* s, int v);
}

void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void Fn80211AA4(Actor* actor, float dt);
int ActorVisibleFlag(Actor* actor);
u32 VarFindHash(u32 a, u32 b, u32 c);

int Fn802120A0(Actor* self, int msg, void* data, int arg4);
int FxEmitterUpdateState(void* data, int mode);
Skin* SkinBlendTint(Skin* skin, int a, int b, int c, float f);
int ModelAnimate(int model, char* name, int flags);
Actor* GetPlayer();
int PlayerInHost(Actor* actor);
int Fn80080FA8();

extern "C" void fn_8028C188(void* dst, const char* src, int len);
int FxEmitterSpawnFromMsg(int* d);

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

// Actor type descriptor of the effect emitter (retail .data 0x803C501C).
struct ActorType {
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void (*update)(Actor* actor, float dt);
    int (*init)(Actor* actor);
    void* unk14;
    void (*destroy)();
    void* unk1C[5];
};

int FxEmitterHandleMessage(Actor* self, int msg, void* data, int arg4);
void ActorFxEmitterUpdate(Actor* self, f32 dt);
int ActorFxEmitterInit(Actor* self);
void BossForward88E0();

ActorType gDestroyableType = { 0, "Destroyable", FxEmitterHandleMessage, ActorFxEmitterUpdate, ActorFxEmitterInit, 0, BossForward88E0 };

// Answer slot of the 0x41D query: the receiver writes its result here.
struct QueryResult {
    String name;
};

// 0x41D "query" payload, answered by the receiving actor.
struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

// Answer to the 0x400 query: the parent actor and its state.
struct ParentQuery {
    Actor* actor;
    u32 unk4;
    u32 unk8;
    int unkC;
    u32 unk10[4];
};

void ActorFxEmitterUpdate(Actor* self, f32 dt)
{
    u8* base = (u8*)self;
    f32* data = *(f32**)(base + 0xA4);
    ParentQuery info;
    Mtx34 mtx;

    if (data[0x10] > 0.0f)
        data[0x10] -= dt;

    if (fn_80212730(*(u32*)(base + 0x38)) != -1) {
        memset(&info, 0, sizeof(info));
        if ((SendMsg(*(u32*)(base + 0xA8), 0x400, &info) == 0 || info.actor == 0 || info.unkC == 0) &&
            lbl_804D57CC == 0) {
            if (info.actor != 0)
                ActorGetMatrix(&mtx, info.actor);
            else
                return;

            if (*(u32*)(*(u32*)(base + 0x84) + 0x24) != 0x24A738) {
                u32 id = *(u32*)(base + 0x38);
                QueryMsg q;

                q.hash = 0xDCC384C1;
                *(int*)&q.result = id;
                q.unk8 = 0;
                SendMsg(id, 0x41D, &q);
            } else {
                u32 h1;
                u32 h2;
                u8 view[0x30];
                u8 rot[0x18];

                if (*(u32*)(base + 0x11C) != 0) {
                    fn_801D55CC(*(u32*)(base + 0x11C));
                    *(u32*)(base + 0x11C) = 0;
                }

                {
                    u32 key = *(u32*)(*(u32*)(base + 0x84) + 0x28);
                    h1 = VarFindHash(0xF3BEEEDF, key, 0x4B35781);
                    h2 = VarFindHash(0xF3BEEEDF, key, 0x40795527);
                }

                fn_8016D864(rot, *(u32*)(base + 0x40));
                fn_8016D6BC(rot, rot);

                *(u32*)(base + 0x11C) = fn_801D5B38(2.0f * *(f32*)(rot + 0x0C),
                                     2.0f * *(f32*)(rot + 0x10),
                                     2.0f * *(f32*)(rot + 0x14),
                                     *(u32*)(base + 0x38), -1, h1, h2, 0x1FFFFFFF, rot, 0);

                if (*(u32*)(base + 0x11C) != 0) {
                    Mtx34 m;
                    f32 v[3];

                    ActorGetMatrix(&m, self);
                    fn_800A0168(&m, &m);
                    fn_801D52E0(*(u32*)(base + 0x11C), &m);
                    v[0] = 0.0f;
                    v[1] = 0.0f;
                    v[2] = 0.0f;
                    fn_801D5298(*(u32*)(base + 0x11C), v);
                    fn_801D5260(*(u32*)(base + 0x11C), v);
                    fn_801D5110(*(u32*)(base + 0x11C), 1);
                    if (ActorVisibleFlag(self) != 0)
                        fn_801D556C(*(u32*)(base + 0x11C));
                }

                *(f32*)(view + 0x08) = 360.0f;
                *(f32*)(view + 0x0C) = 1000.0f;
                *(f32*)(view + 0x10) = 2000.0f;
                *(f32*)(view + 0x14) = 2000.0f;
                *(f32*)(view + 0x18) = 0.0f;
                *(f32*)(view + 0x1C) = 0.0f;
                *(f32*)(view + 0x20) = 0.0f;
                *(f32*)(view + 0x24) = 0.0f;
                *(f32*)(view + 0x28) = 0.0f;
                *(f32*)(view + 0x2C) = 0.0f;

                fn_801D902C(&mtx, (u32)info.actor, self, view);
            }

            fn_8021284C(*(u32*)(base + 0x38));
            data[4] = 0.0f;
            lbl_804D57CC = 1;
        }
    }

    Fn80211AA4(self, dt);
}

void* VAlloc(int size);
int Fn80211F78(Actor* actor);

int ActorFxEmitterInit(Actor* self)
{
    u8* base = (u8*)self;
    u32 h2;
    u32 h1;
    u8* p;
    u8 view[0x18];

    if (*(u8**)(base + 0xA4) == 0) {
        p = (u8*)VAlloc(0x44);
        *(u8**)(base + 0xA4) = p;
    } else {
        p = *(u8**)(base + 0xA4);
    }
    memset(p, 0, 0x44);

    if (Fn80211F78(self) == 0) {
        *(u32*)(base + 0xAC) |= 0x1000000;
        return 0;
    }

    *(u32*)(p + 0x18) &= ~0x08000000;
    *(u32*)(p + 0x18) &= ~0x8;
    *(u32*)(p + 0x18) &= ~0x6;
    *(f32*)(p + 0x34) = 400.0f;
    *(f32*)(p + 0x38) = 1000.0f;
    if (*(u32*)(base + 0x84) == 0 || *(u32*)(*(u32*)(base + 0x84) + 0x24) == 0x24A738)
        *(u32*)(p + 0x18) |= 0x30000000;

    if (*(u32*)(base + 0x11C) != 0) {
        fn_801D55CC(*(u32*)(base + 0x11C));
        *(u32*)(base + 0x11C) = 0;
    }

    {
        u32 key = *(u32*)(*(u32*)(base + 0x84) + 0x28);
        h1 = VarFindHash(0xF3BEEEDF, key, 0x4B35781);
        h2 = VarFindHash(0xF3BEEEDF, key, 0x40795527);

        fn_8016D864(view, *(u32*)(base + 0x40));
        fn_8016D6BC(view, view);

        *(u32*)(base + 0x11C) = fn_801D5B38(2.0f * *(f32*)(view + 0x0C),
                             2.0f * *(f32*)(view + 0x10),
                             2.0f * *(f32*)(view + 0x14),
                             *(u32*)(base + 0x38), -1, h1, h2, 0x1FFFFFFF, view, 1);

        if (*(u32*)(base + 0x11C) != 0) {
            u8 m[0x30];

            ActorGetMatrix((Mtx34*)m, self);
            fn_800A0168(m, m);
            fn_801D52E0(*(u32*)(base + 0x11C), m);
            if (ActorVisibleFlag(self) != 0)
                fn_801D556C(*(u32*)(base + 0x11C));
        }
    }

    return 1;
}

// Message handler of the effect-emitter actor.  0x41E carries a parent name
// that is formatted into the emitter state; 0x407 and 0x7300/0 react on the
// emitter; anything else goes to the default actor handler.
int FxEmitterHandleMessage(Actor* self, int msg, void* data, int arg4)
{
    u32* d = (u32*)data;
    int result = 0;

    switch (msg) {
    case 0x407:
        result = FxEmitterSpawnFromMsg((int*)d);
        break;
    case 0:
        result = FxEmitterUpdateState(d, 0);
        break;
    case 0x7300:
        result = FxEmitterUpdateState(d, 1);
        break;
    case 0x41E: {
        u32 owner = d[0];
        switch (d[1]) {
        case 0xAD8576FA:
            {
                String s;

                s.Format("Parent: 0x%08x\n", fn_80212730(*(u32*)(owner + 0x38)));
                fn_8028C188(d + 3, s.data, s.length);
            }
            result = 0;
            break;
        default:
            result = 0;
        }
        break;
    }
    }

    if (result == -999999)
        return 0;
    if (result != 0)
        return result;
    return Fn802120A0(self, msg, data, arg4);
}

// Per-frame update of the effect emitter.  `data` is the actor message buffer
// whose first word is the owning actor; the emitter state lives at actor+0xA4.
// Builds a tinted skin from the state's fade value and draws the effect.
int FxEmitterUpdateState(void* data, int mode)
{
    Actor* actor = *(Actor**)data;
    u8* st = *(u8**)((u8*)actor + 0xA4);
    int local;
    u32 shape;
    u32 anim;
    Actor* player;
    u32 id;
    Skin* skin;

    if (lbl_804D57CC != 0)
        lbl_804D57CC = 0;

    if (*(u32*)(st + 0x18) & 0x10)
        return 1;

    if (mode == 0 && fn_80158AFC(*(u32*)((u8*)actor + 0x38), data) != 0)
        return 1;

    id = fn_800390D8(*(u32*)((u8*)actor + 0x90), fn_800E7890(actor, &local));
    anim = ModelAnimate(*(int*)((u8*)actor + 0x90), 0, 0);
    shape = fn_8003A1F0(id, *(u32*)((u8*)actor + 0x9C), anim);
    player = GetPlayer();

    if (*(f32*)(st + 0x40) > 0.0f) {
        f32 v = *(f32*)(st + 0x40);
        f32 t;

        if (v > lbl_804D3AD0 - lbl_804D57C8)
            t = (lbl_804D3AD0 - v) / lbl_804D57C8;
        else if (v < lbl_804D3AD4)
            t = v / lbl_804D3AD4;
        else
            t = 1.0f;

        skin = SkinBlendTint((Skin*)((u8*)data + 0x10), 0xFF, 0, 0, t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t));
    } else {
        skin = (Skin*)((u8*)data + 0x10);
    }

    if (!(PlayerInHost(player) != 0 && lbl_804D3408 == 0) && Fn80080FA8() == 0 &&
        *(f32*)(st + 0x10) > 0.0f &&
        *((int**)((u8*)player + 0x26C4))[PlayerInHost(player)] != 0x07C48821 &&
        local < 6) {
        f32 v = fn_80115F68(*(u32*)((u8*)actor + 0x38));

        fn_8003F15C(id, shape, *(u32*)((u8*)actor + 0x94), skin, 0, 1.0f);

        if (v <= 0.0f) {
            u32 idx = *(u32*)(*(u32*)((u8*)actor + 0x84) + 0xA8);

            if ((int)idx >= 0) {
                u32 shape2 = fn_8003A1F0(idx, *(u32*)((u8*)actor + 0x9C), anim);

                fn_8003F15C(idx, shape2, *(u32*)((u8*)actor + 0x94), 0, lbl_803C4FE0, 1.0f);
            }
            fn_801FAD18(actor, 0);
        }
    } else {
        fn_8003F15C(id, shape, *(u32*)((u8*)actor + 0x94), skin, 0, 1.0f);
    }

    return 1;
}

// Effect/render parameters handed to the fx system (0x30 bytes).  The first
// two words are not written here; the fx system fills them in.
struct FxRenderInfo {
    u32 unk0;
    u32 unk4;
    f32 f0;
    f32 f1;
    f32 f2;
    f32 f3;
    f32 f4;
    f32 f5;
    f32 f6;
    f32 f7;
    f32 f8;
    f32 f9;
};

// Payload of message 0x41D: the receiver looks the parent actor up by id and
// answers with its display name.
struct FxNameMsg {
    u32 unk0;
    u32 magic;
    u32 unk8;
    u32 parentId;
};

extern "C" {



    float fabsf(float x);
    void fn_8009F7A8(void* mtx, f32* v);
    void fn_800E1098(f32* dst, f32* src, f32 scale);
    void fn_8016F120(int fx, void* pos);
}

int FxFind(const char* name);

// Worker for actor message 0x407 on the effect-emitter actor.  `d` is the
// message payload: d[0] is the owning actor, d[5] the effect variant (a name
// hash), d[12..14] a rotation and d[6..8] a position.  The emitter state lives
// at actor+0xA4.  Returns 1 when the message was consumed.
int FxEmitterSpawnFromMsg(int* d)
{
    Actor* actor = (Actor*)d[0];
    u8* st = *(u8**)((u8*)actor + 0xA4);
    u32* model = *(u32**)((u8*)actor + 0x84);
    int id = fn_80212730(*(u32*)((u8*)actor + 0x38));

    if (*(u32*)(st + 0x18) & 8) {
        d[0x16] = 1;
        return 1;
    }

    {
        int attached[2];

        if (id != -1 && SendMsg(id, 0xC17, attached) != 0 && attached[1] != 0) {
            d[0x16] = 1;
            return 1;
        }
    }

    if (d[2] == id && d[5] != 0x6CA5F172)
        return 1;

    if (*(f32*)(st + 0x10) <= 0.0f) {
        // Idle emitter: build a one-shot fx straight from the payload.
        f32 scale = *(f32*)((u8*)d + 0x4C);
        f32 alpha;
        FxRenderInfo info;
        f32 pos[3];
        Mtx34 mtx;

        if (fabsf(scale) < 1.1920929e-07f)
            return 1;

        switch (d[5]) {
        case 0xB9FC42F5:
            alpha = 3000.0f;
            break;
        case 0x12DD2E2A:
        case 0xC8769B80:
        case 0x0021EE36:
        case 0x6CA5F172:
        case 0x672F35C2:
            return 1;
        case 0x75715902:
        default:
            alpha = 1000.0f;
            break;
        }
        alpha *= scale;

        info.f0 = 360.0f;
        info.f1 = 1500.0f;
        info.f2 = alpha;
        info.f3 = alpha;
        info.f4 = 0.0f;
        info.f5 = 0.0f;
        info.f6 = 0.0f;
        info.f7 = 0.0f;
        info.f8 = 0.0f;
        info.f9 = 0.0f;

        pos[0] = (f32)d[12];
        pos[1] = (f32)d[13];
        pos[2] = (f32)d[14];

        fn_8009F7A8(&mtx, pos);
        mtx.m[9] = (f32)d[6];
        mtx.m[10] = (f32)d[7];
        mtx.m[11] = (f32)d[8];

        if (*(f32*)((u8*)d + 0x50) > 1.1920929e-07f) {
            fn_800E1098(pos, pos, 1.0f);
            if (pos[2] <= *(f32*)((u8*)d + 0x50))
                pos[2] = *(f32*)((u8*)d + 0x50);
        }
        fn_800E1098(pos, pos, alpha);

        fn_801D902C(&mtx, d[1], actor, &info);
        return 1;
    }

    // Live emitter: age it, then respawn the fx it carries.
    switch (d[5]) {
    case 0x403548D0:
    case 0x00011A6B:
    case 0xA349664A:
        break;
    case 0x6CA5F172:
        *(f32*)(st + 0x10) = -1.0f;
        break;
    default:
        d[2] = *(int*)((u8*)actor + 0x38);
        d[1] = (int)actor;
        SendMsg(id, 0x407, d);
        if (*(f32*)(st + 0x40) <= 0.0f)
            *(f32*)(st + 0x40) = lbl_804D3AD0;
        return 1;
    }

    if (*(f32*)(st + 0x10) > 0.0f)
        return 1;

    {
        u32 found[8];
        Mtx34 parentMtx;

        memset(found, 0, sizeof(found));

        id = fn_80212730(*(u32*)((u8*)actor + 0x38));
        if (id == -1 || SendMsg(id, 0x400, found) == 0)
            return 1;

        ActorGetMatrix(&parentMtx, (Actor*)found[0]);

        if (*(u32*)((u8*)model + 0x24) != 0x24A738) {
            FxNameMsg msg;
            // The retail code also constructs a String here and keeps the
            // parent id in its first word; the original source shape for that
            // is still unknown, so only the plain payload is modelled.
            String name;

            msg.magic = 0xDCC384C1;
            msg.unk8 = 0;
            msg.parentId = d[2];
            (void)name;
            SendMsg(*(u32*)((u8*)actor + 0x38), 0x41D, &msg);
        } else {
            FxRenderInfo info;
            Mtx34 rot;
            Mtx34 fxMtx;
            f32 v[3];
            u32 h1;
            u32 h2;

            if (*(u32*)((u8*)actor + 0x11C) != 0) {
                fn_801D55CC(*(u32*)((u8*)actor + 0x11C));
                *(u32*)((u8*)actor + 0x11C) = 0;
            }

            h1 = VarFindHash(0xF3BEEEDF, *(u32*)((u8*)model + 0x28), 0x4B35781);
            h2 = VarFindHash(0xF3BEEEDF, *(u32*)((u8*)model + 0x28), 0x40795527);

            fn_8016D864(rot.m, *(u32*)((u8*)actor + 0x40));
            fn_8016D6BC(rot.m, rot.m);

            *(u32*)((u8*)actor + 0x11C) =
                fn_801D5B38(2.0f * rot.m[3], 2.0f * rot.m[4],
                            2.0f * rot.m[5], *(u32*)((u8*)actor + 0x38),
                            -1, h1, h2, 0x1FFFFFFF, rot.m, 0);

            if (*(u32*)((u8*)actor + 0x11C) != 0) {
                ActorGetMatrix(&fxMtx, actor);
                fn_800A0168(&fxMtx, &fxMtx);
                fn_801D52E0(*(u32*)((u8*)actor + 0x11C), &fxMtx);

                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = 0.0f;
                fn_801D5298(*(u32*)((u8*)actor + 0x11C), v);
                fn_801D5260(*(u32*)((u8*)actor + 0x11C), v);
                fn_801D5110(*(u32*)((u8*)actor + 0x11C), 1);
                if (ActorVisibleFlag(actor) != 0)
                    fn_801D556C(*(u32*)((u8*)actor + 0x11C));
            }

            info.f0 = 360.0f;
            info.f1 = 1000.0f;
            info.f2 = 2000.0f;
            info.f3 = 2000.0f;
            info.f4 = 0.0f;
            info.f5 = 0.0f;
            info.f6 = 0.0f;
            info.f7 = 0.0f;
            info.f8 = 0.0f;
            info.f9 = 0.0f;

            fn_801D902C(&parentMtx, found[0], actor, &info);
        }

        fn_8016F120(FxFind("BOSS_BREAK"), (u8*)actor + 0x24);
        fn_8021284C(*(u32*)((u8*)actor + 0x38));

        if (id != -1) {
            u32 stateMsg[4];

            stateMsg[2] = *(u32*)((u8*)actor + 0x38);
            stateMsg[3] = 0x90F5B327;
            stateMsg[1] = 0;
            SendMsg(id, 0xC14, stateMsg);
        }
    }
    return 1;
}

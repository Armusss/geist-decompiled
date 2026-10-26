// u_8020B77C.cpp: FX actor emitter helpers (text-out, spawn and update) (0x8020B77C-0x8020D47C).
// Names are descriptive guesses; the code is matched against the retail DOL.

extern "C" void* memset(void* dst, int val, unsigned long size);
extern "C" int SendMsg(int to, int msg, void* data);
int FxEmitterAimUpdate(float value, struct FxActor* actor);
extern "C" float lbl_804D93A8;

void* AssetFind(unsigned long, unsigned long);
int ActorPlayAnim(int, unsigned long, int);

struct Actor;
int PlayerInHost(Actor* actor);
int FxFind(const char* name);

// State block at actor+0x92C (0x34 bytes).
struct FxCfg {
    int handle;             // 0x92C
    unsigned long nameIdx;  // 0x930
    int unk08;              // 0x934
    float unk938;           // 0x938
    float unk93C;           // 0x93C
    float x;                // 0x940
    float y;                // 0x944
    float z;                // 0x948
    float unk94C;           // 0x94C
    short state;            // 0x950
    short state2;           // 0x952
    short unk954;           // 0x954
    short unk2A;            // 0x956
    short unk958;           // 0x958
    short unk95A;           // 0x95A
    short unk95C;           // 0x95C
    short flags;            // 0x95E
};

struct FxActor;

struct Vec3 {
    float x;
    float y;
    float z;
};

// 3x4 transform matrix.
struct Mtx34 {
    float m[12];
};

// Engine placement block the actor starts with.
struct PlaceF;

struct FxHost {
    char unk00[0x38];
    int unk38;              // 0x038
};

struct FxPlayer {
    char unk00[0x38];
    unsigned int unk38;     // 0x038
    char unk3C[0x48];
    void* unk84;            // 0x084
};

struct FxActor {
    char unk00[0x24];
    int unk24;              // 0x024
    int unk28;              // 0x028
    int unk2c;              // 0x02C
    char unk30[0x08];
    int unk38;              // 0x038
    char unk3C[0x14];
    unsigned int flags;     // 0x050
    char unk54[0x130];
    int unk184;             // 0x184
    char unk188[0x38];
    char unk1C0[0x24];      // 0x1C0
    float posX;             // 0x1E4
    float posY;             // 0x1E8
    float posZ;             // 0x1EC
    float unk1F0;           // 0x1F0
    char unk1F4[0x6C];
    float unk260;           // 0x260
    char unk264[0x328];     // 0x264
    int unk58C;             // 0x58C
    char unk590[0xF8];      // 0x590
    Vec3 unk688;            // 0x688
    char unk694[0xA4];      // 0x694
    void* unk738;           // 0x738
    char unk73C[0x1F0];     // 0x73C
    FxCfg cfg;              // 0x92C
    int unk960;             // 0x960
    int unk964;             // 0x964
    char unk968[0x7C];      // 0x968
    int unk9E4;             // 0x9E4
    char unk9E8[0xB14];     // 0x9E8
    float unk14FC;          // 0x14FC
    char unk1500[0x868];
    FxHost* unk1D68;        // 0x1D68
    char unk1D6C[0x30];
    int unk1D9C;            // 0x1D9C
    char unk1DA0[0x944];
    FxPlayer* unk26E4;      // 0x26E4
    char unk26E8[0x0C];
    char unk2F4[0x100];     // 0x2F4
};

struct FxMsg {
    void* unk00;
    void* asset;
    char unk08[0x2C];
    int x;                  // 0x34
    int y;                  // 0x38
    int z;                  // 0x3C
};

// Set up the effect emitter on the actor and broadcast its spawn message.
void FxEmitterStart(FxActor* actor, int owner)
{
    FxMsg msg;

    memset(&actor->cfg, 0, sizeof(actor->cfg));
    actor->cfg.flags |= 0x400;
    actor->cfg.unk08 = owner;
    actor->cfg.unk2A = 0;
    actor->flags |= 2;
    msg.asset = AssetFind(0xF1754BF2, 0x21C5E0);
    SendMsg(owner, 0x411, &msg);
    actor->cfg.x = (float)msg.x;
    actor->cfg.y = (float)msg.y;
    actor->cfg.z = (float)msg.z;
    actor->cfg.flags |= 0x800;
    actor->flags |= 2;
    actor->unk260 = lbl_804D93A8;
}

// Per-frame emitter update: fires the end-of-life animation once the timer expires.
int FxEmitterUpdate(FxActor* actor)
{
    if (!(actor->cfg.flags & 0x400)) {
        return 0;
    }
    if ((actor->cfg.flags & 0x800) && FxEmitterAimUpdate(actor->unk14FC, actor)) {
        actor->cfg.flags &= ~0x800;
        actor->flags &= ~2;
        actor->cfg.flags &= ~0x400;
        ActorPlayAnim(actor->cfg.unk08, 0xCFE9C6DC, actor->unk38);
    }
    return 1;
}

// Enable or disable bit 0x100 of the effect state flags.
int SetFxFlag100(FxActor* actor, int enable)
{
    if (enable) {
        actor->cfg.flags |= 0x100;
    } else {
        actor->cfg.flags &= ~0x100;
    }
    return 1;
}

// Enable or disable bit 0x80 of the effect state flags.
int SetFxFlag80(FxActor* actor, int enable)
{
    if (enable) {
        actor->cfg.flags &= ~0x80;
    } else {
        actor->cfg.flags |= 0x80;
    }
    return 1;
}

// True when bit 0 of the effect state flags is set.
// fn_8020B97C
extern "C" int fn_8020B97C(FxActor* actor)
{
    return actor->cfg.flags & 1;
}

// Engine string object (constructor/destructor live in another unit).
class Buffer {
public:
    ~Buffer();
    void* data;             // 0x00
    int unk04;
    int unk08;
    int unk0C;
};

class String : public Buffer {
public:
    String();
    int unk10;
};

// Message sent to the owner to ask for the asset name; the name is filled in
// by the handler through the trailing String.
struct FxNameMsg {
    int result;             // 0x08
    unsigned long hash;     // 0x0C
    int unused;             // 0x10
    String name;            // 0x14
};

int StrGetIndex(int asset, unsigned long* out);
extern "C" void FxStart(void* name, int arg1, int arg2);
extern "C" void fn_8016F120(void* name, void* data);

// Looks up the effect asset for the given id and starts it on the actor.
void FxEmitterSetAsset(FxActor* actor, unsigned long id)
{
    FxNameMsg msg;
    void* name;
    int started;
    int out;

    actor->cfg.unk2A = 0;
    if (id == 0xBFD420DA) {
        return;
    }
    if (id == 0) {
        actor->cfg.handle = 0;
    } else {
        actor->cfg.handle = StrGetIndex((int)AssetFind(0xBAA22E62, id), &actor->cfg.nameIdx);
    }
    if (actor->cfg.handle == 0) {
        actor->cfg.handle = StrGetIndex((int)AssetFind(0xBAA22E62, 0x1A347905), &actor->cfg.nameIdx);
    }
    msg.hash = 0x17F428F8;
    msg.unused = 0;
    started = SendMsg(actor->cfg.unk08, 0x41E, &msg);
    name = msg.name.data;
    if (started != 0 && name != (void*)-1) {
        if (SendMsg(actor->cfg.unk08, 0x400, &out) == 0 || out == 0) {
            FxStart(name, 0, 0);
        } else {
            fn_8016F120(name, (char*)out + 0x24);
        }
    }
    actor->cfg.flags |= 2;
}

// Sends the "text_out" request to the host actor, switches the emitter to
// state 0x41 and starts the text effect. (fn_8020BAF4)
void FxEmitterStartTextOut(FxActor* actor)
{
    FxNameMsg msg;
    void* name;

    actor->cfg.flags &= ~0x2;
    actor->cfg.flags |= 0x40;
    msg.name.data = 0;
    msg.hash = 0x18E1630C;
    msg.unused = 0;
    SendMsg(actor->cfg.unk08, 0x41E, &msg);
    name = msg.name.data;
    if (name != 0) {
        actor->cfg.flags |= 0x1000;
    }
    if (PlayerInHost((Actor*)actor) == 0) {
        actor->cfg.state = 0x41;
    } else {
        actor->cfg.state = 0x41;
    }
    FxStart((void*)FxFind("text_out"), 0, 0);
}

// ---------------------------------------------------------------------------
// Helpers shared by the emitter spawn/draw functions below.
// ---------------------------------------------------------------------------
extern "C" int fn_800F9614(int handle);
extern "C" void fn_8006B360(void* dst);
extern "C" int fn_800498D0(void* actor);
extern "C" void fn_800A0168(void* dst, void* src);
extern "C" void fn_800A0030(float* vec, void* mtx, float* out);
extern "C" float fn_800E1020(float* vec);
extern "C" float fn_800A109C(float value);
extern "C" unsigned char __upper_map[];

void PlayerResetGunMotion(void* actor);

void Draw2DBegin();
void Draw2DEnd();
void DrawTexQuad(int tex, float x, float y, float w, float h,
                 unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void TextSetRect(int, int, int, int, int, int, int, int);
void TextSetStyle(int, int, int, int, int, int, int, int, int, int, int, int);
void TextSetBox(int, int, int, int);
void TextSetFlags(int, int);
void TextSetMode(int);
void TextSetAlign(int);
void TextSetColor(int, int, int, int);
void TextSetSize(int);
void TextDraw(const char* text, unsigned long id);
const char* StrFindKey(unsigned long key, unsigned long* out);
void FxTextRectUpdate(short start, int line, int lines, int unused6, int page, int unused8,
                      int unused9, FxActor* actor);

extern "C" float lbl_804D93B8;
extern "C" float lbl_804D93BC;
extern "C" float lbl_804D93C0;
extern "C" float lbl_804D93C4;
extern "C" float lbl_804D33C0;
extern "C" float lbl_804D33F4;
extern "C" int lbl_804D3400;
extern "C" int lbl_804D33D0;
extern "C" int lbl_804D33D4;
extern "C" int lbl_804D33D8;
extern "C" int lbl_804D33DC;
extern "C" int lbl_804D33E0;
extern "C" int lbl_804D33E4;
extern "C" int lbl_804D33E8;
extern "C" int lbl_804D33EC;
extern "C" int lbl_804D33F0;

// Type descriptor the engine keeps for the effect class; the readable name
// lives 0x50 bytes into it.
struct FxTypeDesc {
    char pad00[0x50];
    char name[0x20];        // 0x50
};

extern FxTypeDesc lbl_80349748;

struct FxVec {
    float x;
    float y;
    float z;
};

// Object the 0x411 asset reply points back at; only its 0x2C word is used.
struct FxMsgSrc {
    char pad00[0x2C];
    int unk2C;
};

// Message broadcast with id 0x409 (six words long).
struct FxMsg409 {
    void* host;             // 0x00
    void* actor;            // 0x04
    int unk08;              // 0x08
    int unk0C;              // 0x0C
    int unk10;              // 0x10
    int unk14;              // 0x14
};

// Player view record reached through the actor's view pointer.
struct FxViewRec {
    char pad00[0x1C];
    int unk1C;
};

// Full emitter setup: resets the effect state, asks the owner for the asset
// name and places the spawn points. (fn_8020BBF8)
void FxEmitterSetup(FxActor* actor, int owner)
{
    FxVec from;
    FxVec to;
    FxMsg409 msg409;
    FxMsg msg;
    float mtx[16];
    FxTypeDesc* type;
    const char* name;
    unsigned long hash;
    int view;
    int z;
    int srcZ;

    if (actor->unk1D68 == 0) {
        return;
    }
    actor->cfg.handle = 0;
    actor->cfg.nameIdx = 0;
    actor->cfg.unk08 = owner;
    actor->cfg.unk954 = 0;
    actor->cfg.unk2A = 0;
    actor->cfg.unk958 = 0;
    actor->cfg.unk95A = 0;
    actor->cfg.unk95C = 0;
    actor->cfg.unk938 = lbl_804D93A8;
    actor->cfg.state = fn_800F9614(actor->unk184);

    if (((unsigned int)owner >> 24) == 2 || ((unsigned int)owner >> 24) == 4 ||
        ((unsigned int)owner >> 24) == 0x13) {
        hash = 0;
        type = &lbl_80349748;
        name = type->name;
        if (name != 0) {
            for (const char* p = name; *p != 0; p++) {
                int c = *p;
                unsigned long v;

                if (c == -1) {
                    v = 0xFFFFFFFF;
                } else {
                    v = __upper_map[c & 0xFF];
                }
                hash *= 31;
                hash += v & 0xFF;
            }
            hash %= 0xFFFFFFFF;
        }
        msg.asset = AssetFind(0xF1754BF2, hash);
        SendMsg(owner, 0x411, &msg);
        from.x = (float)msg.x;
        from.y = (float)msg.y;
        from.z = (float)msg.z;
        to.x = from.x;
        to.y = from.y;
        to.z = from.z;
        to.z = from.z + (float)lbl_804D3400;
        from.x -= actor->posX;
        from.y -= actor->posY;
        from.z -= actor->posZ;
        to.x -= actor->posX;
        to.y -= actor->posY;
        to.z -= actor->posZ;
        fn_800A0168(mtx, actor->unk1C0);
        fn_800A0030(&to.x, mtx, &to.x);
        fn_800A0030(&from.x, mtx, &from.x);
        {
            float lenTo = fn_800E1020(&to.x);
            float lenFrom = fn_800E1020(&from.x);
            float dot = from.x * to.x + from.y * to.y + from.z * to.z;
            float angle = fn_800A109C(dot / (lenTo * lenFrom));

            if (angle <= lbl_804D93B8) {
                angle = lbl_804D93B8;
            }
            actor->cfg.state2 = (short)angle;
        }
    } else {
        actor->cfg.state2 = fn_800F9614(actor->unk184);
    }
    actor->cfg.unk94C = lbl_804D93BC;

    if ((actor->cfg.flags & 1) == 0) {
        actor->cfg.unk93C = lbl_804D93A8;
        actor->cfg.flags &= 0x180;
        actor->cfg.flags |= 1;
    }
    actor->cfg.flags &= ~0x40;
    actor->cfg.flags |= 0x20;

    actor->flags |= 2;
    actor->flags |= 0x1000000;
    actor->flags |= 0x800000;
    fn_8006B360(actor->unk2F4);

    msg409.host = actor->unk1D68;
    msg409.actor = actor;
    msg409.unk0C = actor->unk1D9C;
    msg409.unk14 = 0;
    msg409.unk08 = fn_800498D0(actor);
    msg409.unk10 = msg409.unk08;
    SendMsg(((FxHost*)msg409.host)->unk38, 0x409, &msg409);

    if (PlayerInHost((Actor*)actor) != 0) {
        if ((actor->unk26E4->unk38 >> 24) == 1) {
            view = ((FxViewRec*)actor->unk26E4->unk84)->unk1C;
            if (view == 0 || view == 0xA82 || view == 0x942B64F0) {
                actor->cfg.flags |= 0x2000;
            }
        }
    }

    if ((actor->cfg.flags & 0x2000) == 0) {
        SendMsg(owner, 0x400, &actor);
        if (((unsigned int)owner >> 24) == 2 || ((unsigned int)owner >> 24) == 4 ||
            ((unsigned int)owner >> 24) == 0x13 || ((unsigned int)owner >> 24) == 1) {
            msg.asset = AssetFind(0xF1754BF2, 0x21C5E0);
            SendMsg(owner, 0x411, &msg);
            srcZ = ((FxMsgSrc*)msg.unk00)->unk2C;
            z = (int)(actor->posZ - (float)actor->unk2c);
            actor->cfg.x = (float)(z - (msg.z - srcZ));
            actor->cfg.y = (float)msg.y;
            actor->cfg.z = (float)msg.z + lbl_804D33C0 * (float)msg.x;
        } else {
            actor->cfg.x = (float)actor->unk24;
            actor->cfg.y = (float)actor->unk28;
            actor->cfg.z = (float)actor->unk2c;
        }
        actor->flags |= 2;
        actor->flags |= 0x1000000;
        actor->flags |= 0x800000;
        actor->unk260 = lbl_804D93A8;
        PlayerResetGunMotion(actor);
    }
}

// Draws the emitter's timing bar and the pop-up text that goes with it.
// (fn_8020C178)
void FxEmitterDrawText(FxActor* actor)
{
    if (actor->cfg.flags & 1) {
        float progress = actor->cfg.unk93C / lbl_804D33F4;

        if (progress > lbl_804D93A8) {
            float bar = lbl_804D93C0 - (float)lbl_804D33EC * progress;
            float fill = (float)lbl_804D33F0 * progress;

            Draw2DBegin();
            DrawTexQuad(-3, lbl_804D93A8, lbl_804D93C0, lbl_804D93C4, bar, 0, 0, 0, 0xff);
            DrawTexQuad(-3, lbl_804D93A8, fill, lbl_804D93C4, lbl_804D93A8, 0, 0, 0, 0xff);
            Draw2DEnd();
        }
    }

    if ((actor->cfg.flags & 1) && (actor->cfg.flags & 2) && actor->cfg.handle != 0 &&
        (actor->cfg.flags & 0x60) == 0) {
        int height = lbl_804D33E4 - lbl_804D33E0;
        int half = lbl_804D33D4 / 2;
        int left = (0x140 - half) + lbl_804D33D8;
        int right = half + lbl_804D33D8 + 0x140;

        TextSetRect(left, right, height, lbl_804D33E4, actor->cfg.unk958, actor->cfg.unk95A,
                    (int)FxTextRectUpdate, (int)actor);
        TextSetStyle(0xff, 0xff, 0xff, 0xff, lbl_804D33D0, 0, 0, 0, 0, 0, lbl_804D33E8, 0);
        TextSetFlags(1, 0);
        TextDraw((const char*)actor->cfg.handle, actor->cfg.nameIdx);
        if (actor->cfg.flags & 0x10) {
            unsigned long key = 0;

            TextSetBox(lbl_804D33DC + (left - lbl_804D33D8),
                       lbl_804D33DC + (right - lbl_804D33D8), 0, height);
            TextSetMode(1);
            TextSetAlign(1);
            TextSetColor(0xff, 0xff, 0xff, 0xff);
            TextSetSize(0x1b);
            TextSetFlags(1, 0);
            if (actor->cfg.flags & 4) {
                TextDraw(StrFindKey(0x136FAAE3, &key), key);
            } else {
                TextDraw(StrFindKey(0x136B9310, &key), key);
            }
            TextSetSize(0);
        }
    }
}

// ---------------------------------------------------------------------------
// Per-frame emitter state machine: the low bits of cfg.flags select the
// lifetime phases (spawn placement, fade-out, text roll-out and restart).
// (fn_8020C424)
// ---------------------------------------------------------------------------
extern "C" void fn_800F95A0(float value, int handle);
extern "C" float fn_800A1418(float value);
void FxEmitterRollText(float dt, FxActor* actor);
extern "C" float lbl_804D93C8;
extern "C" float lbl_804D93CC;
extern "C" float lbl_804D93D0;
extern "C" float lbl_804D93D4;
extern "C" float lbl_804D93D8;
extern "C" int lbl_804D33C4;
extern "C" int lbl_804D33C8;

int FxEmitterStateUpdate(FxActor* actor)
{
    FxMsg msg;
    unsigned short state;
    int owner;
    unsigned int kind;
    int srcZ;
    int z;

    state = actor->cfg.flags;
    if (!(state & 1)) {
        return 0;
    }
    if (actor->unk9E4 != 0 || actor->unk960 != 0 || actor->unk964 != 0) {
        actor->flags &= ~2;
        actor->flags &= ~0x1000000;
        actor->flags &= ~0x800000;
        actor->cfg.flags &= ~1;
        ActorPlayAnim(actor->cfg.unk08, 0x60096007, actor->unk38);
        return 1;
    }

    if ((state & 0x100) && !(state & 0x2000)) {
        owner = actor->cfg.unk08;
        SendMsg(owner, 0x400, &actor);
        kind = (unsigned int)owner >> 24;
        if (kind == 2 || kind == 4 || kind == 0x13 || kind == 1) {
            msg.asset = AssetFind(0xF1754BF2, 0x21C5E0);
            SendMsg(owner, 0x411, &msg);
            srcZ = ((FxMsgSrc*)msg.unk00)->unk2C;
            z = (int)(actor->posZ - (float)actor->unk2c);
            actor->cfg.x = (float)(z - (msg.z - srcZ));
            actor->cfg.y = (float)msg.y;
            actor->cfg.z = (float)msg.z + lbl_804D33C0 * (float)msg.x;
        } else {
            actor->cfg.x = (float)actor->unk24;
            actor->cfg.y = (float)actor->unk28;
            actor->cfg.z = (float)actor->unk2c;
        }
        actor->flags |= 2;
        actor->flags |= 0x1000000;
        actor->flags |= 0x800000;
        actor->unk260 = lbl_804D93A8;
        PlayerResetGunMotion(actor);
    }

    state = actor->cfg.flags;
    if ((state & 0x20) == 0) {
        if (state & 0x40) {
            int done1;
            int done2;

            actor->cfg.unk93C -= actor->unk14FC;
            if (actor->cfg.unk93C < lbl_804D93A8) {
                actor->cfg.unk93C = lbl_804D93A8;
            }
            done1 = actor->cfg.unk93C == lbl_804D93A8;
            if (!(actor->cfg.flags & 0x2000)) {
                actor->cfg.unk938 -= actor->unk14FC;
                if (actor->cfg.unk938 < lbl_804D93A8) {
                    actor->cfg.unk938 = lbl_804D93A8;
                }
                if (actor->cfg.unk938 == lbl_804D93A8) {
                    fn_800F95A0((float)actor->cfg.state, actor->unk184);
                    done2 = 1;
                } else {
                    float t = -(lbl_804D93CC *
                                fn_800A1418(lbl_804D93D0 *
                                            (actor->cfg.unk938 / lbl_804D33F4)) -
                                lbl_804D93CC);
                    int v = (int)((float)actor->cfg.state * (lbl_804D93B8 - t) +
                                  (float)actor->cfg.state2 * t);

                    fn_800F95A0((float)v, actor->unk184);
                    done2 = 0;
                }
            } else {
                done2 = 1;
                actor->cfg.unk938 = lbl_804D93A8;
            }
            if (!(done1 && done2)) {
                return 1;
            }
            actor->flags &= ~2;
            actor->flags &= ~0x1000000;
            actor->flags &= ~0x800000;
            actor->cfg.flags &= ~1;
            ActorPlayAnim(actor->cfg.unk08, 0x60096007, actor->unk38);
            return 1;
        }

        if (state & 2) {
            int inc;

            fn_800F95A0((float)actor->cfg.state2, actor->unk184);
            inc = (int)(lbl_804D93D4 * lbl_804D93D8 * actor->unk14FC *
                        (float)lbl_804D33C8);
            if (inc == 0) {
                inc = lbl_804D33C8;
            }
            lbl_804D33C4 += inc;
            if (lbl_804D33C4 < 0xFF) {
                if (lbl_804D33C4 < 0x9C) {
                    lbl_804D33C4 = 0x9B;
                    lbl_804D33C8 = 1;
                }
            } else {
                lbl_804D33C4 = 0xFF;
                lbl_804D33C8 = -1;
            }
            FxEmitterRollText(actor->unk14FC, actor);
            FxEmitterAimUpdate(actor->unk14FC, actor);
            return 1;
        }

        actor->cfg.unk2A += (short)(int)actor->unk14FC;
        fn_800F95A0((float)actor->cfg.state2, actor->unk184);
        FxEmitterAimUpdate(actor->unk14FC, actor);
        if (actor->cfg.unk2A < 0x2711) {
            return 1;
        }
        actor->cfg.unk2A = 0;
        actor->cfg.handle = 0;
        if (actor->cfg.handle == 0) {
            actor->cfg.handle =
                StrGetIndex((int)AssetFind(0xBAA22E62, 0x1A347905), &actor->cfg.nameIdx);
        }
        {
            FxNameMsg nmsg;
            void* name;
            int started;

            nmsg.hash = 0x17F428F8;
            nmsg.unused = 0;
            started = SendMsg(actor->cfg.unk08, 0x41E, &nmsg);
            name = nmsg.name.data;
            if (started != 0 && name != (void*)-1) {
                int out;

                if (SendMsg(actor->cfg.unk08, 0x400, &out) == 0 || out == 0) {
                    FxStart(name, 0, 0);
                } else {
                    fn_8016F120(name, (char*)out + 0x24);
                }
            }
        }
        actor->cfg.flags |= 2;
        return 1;
    }

    {
        int alive = FxEmitterAimUpdate(actor->unk14FC, actor);
        int full;
        int done;

        actor->cfg.unk93C += actor->unk14FC;
        if (actor->cfg.unk93C > lbl_804D33F4) {
            actor->cfg.unk93C = lbl_804D33F4;
        }
        full = lbl_804D33F4 <= actor->cfg.unk93C;
        if (lbl_804D93C8 <= actor->cfg.unk94C) {
            done = 0;
        } else {
            if (!(actor->cfg.flags & 0x80)) {
                actor->cfg.unk938 += actor->unk14FC;
                if (actor->cfg.unk938 > lbl_804D33F4) {
                    actor->cfg.unk938 = lbl_804D33F4;
                }
                if (lbl_804D33F4 <= actor->cfg.unk938) {
                    fn_800F95A0((float)actor->cfg.state2, actor->unk184);
                    done = 1;
                } else {
                    float t = -(lbl_804D93CC *
                                fn_800A1418(lbl_804D93D0 *
                                            (actor->cfg.unk938 / lbl_804D33F4)) -
                                lbl_804D93CC);
                    int v = (int)((float)actor->cfg.state * (lbl_804D93B8 - t) +
                                  (float)actor->cfg.state2 * t);

                    fn_800F95A0((float)v, actor->unk184);
                    done = 0;
                }
            } else {
                done = 1;
            }
            done = alive & 1 & full & done;
        }
        if (done != 0) {
            actor->cfg.flags &= ~0x20;
        }
        return 1;
    }
}

// Empty effect callback, stored in a state table as a no-op. (fn_8020CC14)
void FxEmitterNoOp(void)
{
}

// Clears the effect state block at actor+0x92C. (fn_8020CC18)
void FxEmitterClearState(FxActor* actor)
{
    memset(&actor->cfg, 0, sizeof(actor->cfg));
}

int PadPressed(int pad, int button, int port);
extern "C" int lbl_804D33CC;

// Small message the effect actor posts to itself when it starts an animation.
struct FxAnimMsg {
    int unk00;
    int unk04;
    int handle;             // 0x08
    int hash;               // 0x0C
};

// Per-frame roll-out of the effect text: pad handling, the countdown timer and
// the page change. (fn_8020CC44)
void FxEmitterRollText(float dt, FxActor* actor)
{
    short flags = actor->cfg.flags;

    if ((flags & 0x20) || (flags & 0x40)) {
        return;
    }
    actor->cfg.flags = flags & ~0x10;

    flags = actor->cfg.flags;
    if (flags & 8) {
        if (PadPressed(actor->unk58C, 0, 3) == 1) {
            actor->cfg.unk95A = 0x7fff;
        } else {
            actor->cfg.unk2A -= (short)dt;
            while (actor->cfg.unk2A < 1) {
                actor->cfg.unk2A += lbl_804D33CC;
                actor->cfg.unk95A++;
            }
        }
    } else if (flags & 4) {
        if (actor->cfg.unk95C == 0) {
            actor->cfg.flags |= 0x10;
        } else {
            actor->cfg.unk95C--;
            actor->cfg.unk2A = 0;
        }
    } else {
        actor->cfg.flags |= 0x10;
    }

    if ((actor->cfg.flags & 0x10) && PadPressed(actor->unk58C, 0, 3) == 1) {
        if (!(actor->cfg.flags & 4)) {
            FxAnimMsg amsg;

            ActorPlayAnim(actor->cfg.unk08, 0xCFE9C6DC, actor->unk38);
            if (actor->unk38 != -1) {
                amsg.unk04 = 0;
                amsg.handle = actor->cfg.unk08;
                amsg.hash = 0x268BA0FF;
                SendMsg(actor->unk38, 0xC14, &amsg);
            }
            if (!(actor->cfg.flags & 0x20)) {
                if (PlayerInHost((Actor*)actor) == 0) {
                    actor->cfg.state = 0x41;
                } else {
                    actor->cfg.state = 0x41;
                }
                actor->cfg.flags |= 0x40;
                FxStart((void*)FxFind("text_out"), 0, 0);
                actor->cfg.flags |= 0x40;
                {
                    FxNameMsg msg;

                    SendMsg(actor->cfg.unk08, 0x41E, &msg);
                    if (msg.name.data != 0) {
                        actor->cfg.flags |= 0x1000;
                    }
                }
            }
        } else {
            actor->cfg.unk958 += actor->cfg.unk954;
            FxStart((void*)FxFind("text_page"), 0, 0);
            ActorPlayAnim(actor->cfg.unk08, 0x4D04426C, actor->unk38);
        }
    }
}

// Text rectangle scroll callback: keeps the visible line range and the page
// counter bits of the effect state in sync with the text rectangle.
// (fn_8020CEF0)
void FxTextRectUpdate(short start, int line, int lines, int unused6, int page, int unused8,
                      int unused9, FxActor* actor)
{
    if (line < lines) {
        actor->cfg.unk954 = (short)(line - start);
        actor->cfg.flags |= 4;
    } else {
        actor->cfg.flags &= ~4;
    }
    if (actor->cfg.unk95A < page) {
        actor->cfg.flags |= 8;
    } else {
        actor->cfg.flags &= ~8;
    }
}

// ---------------------------------------------------------------------------
// Aim blending: turns the emitter towards the direction it was placed from and
// reports a white-out (0) when the blended orientation drifted away from it.
// (fn_8020CF48)
// ---------------------------------------------------------------------------
struct Quat {
    float x;
    float y;
    float z;
    float w;
};

extern "C" float fabsf(float value);
extern "C" float sqrtf(float value);
extern "C" void Vec3Set(Vec3* out, float x, float y, float z);
extern "C" void fn_8009F7A8(Mtx34* out, Vec3* dir);
extern "C" void fn_800A025C(Vec3* out, Mtx34* mtx, int arg);
extern "C" float fn_800A1394(float value);
extern "C" void C_QUATSlerp(Quat* a, Quat* b, Quat* out, float t);
extern "C" float lbl_804D93DC;
extern "C" float lbl_804D93D8;
extern "C" float lbl_804D93E0;
extern "C" float lbl_804D33F8;
extern "C" float lbl_804D33FC;

void MtxSetEuler(Mtx34* mtx, Vec3* euler);
void MtxGetEuler(Vec3* euler, Mtx34* mtx);
void BoneSetMatrix(void* out, Mtx34* mtx);
void BoneGetMatrix(Mtx34* mtx, void* in);
void PlacementToMatrix(PlaceF* place, Mtx34* mtx);
void WhiteOutEffectQuery(int arg);

// Normalises the vector, or clears it when it has no usable direction.
static inline void NormalizeFxVector(Vec3* v)
{
    if (fabsf(v->x) > lbl_804D93DC || fabsf(v->y) > lbl_804D93DC ||
        fabsf(v->z) > lbl_804D93DC) {
        float inv = 1.0f / sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);

        v->x *= inv;
        v->y *= inv;
        v->z *= inv;
    } else {
        v->x = lbl_804D93A8;
        v->y = lbl_804D93A8;
        v->z = lbl_804D93A8;
    }
}

int FxEmitterAimUpdate(float dt, FxActor* actor)
{
    Vec3 dir;
    Vec3 euler;
    Vec3 axis;
    Vec3 source;
    Vec3 cross;
    Mtx34 mtx;
    Quat dirQuat;
    Quat baseQuat;
    Quat blended;
    float threshold;
    float angle;
    float progress;
    float t;
    float dot;

    if ((actor->cfg.flags & 0x80) || (actor->cfg.flags & 0x2000)) {
        actor->cfg.unk94C = lbl_804D93A8;
        return 1;
    }

    if (actor->unk26E4 != 0 && (actor->unk26E4->unk38 >> 24) == 1) {
        threshold = (float)(int)*(float*)((char*)actor->unk26E4->unk84 + 0x34);
        if (threshold < lbl_804D93A8) {
            threshold = (float)(int)*(float*)((char*)actor->unk738 + 0x3C);
        }
    } else {
        threshold = (float)(int)*(float*)((char*)actor->unk738 + 0x3C);
    }

    dir.x = actor->cfg.x - actor->posX;
    dir.y = actor->cfg.y - actor->posY;
    dir.z = actor->cfg.z - actor->posZ;
    NormalizeFxVector(&dir);

    fn_8009F7A8(&mtx, &dir);
    MtxGetEuler(&euler, &mtx);
    BoneSetMatrix(&dirQuat, &mtx);

    if (fabsf(euler.x) > threshold) {
        actor->cfg.unk94C = lbl_804D93A8;
        return 1;
    }

    Vec3Set(&source, actor->unk688.z, actor->unk1F0, lbl_804D93A8);
    MtxSetEuler(&mtx, &source);
    fn_800A025C(&axis, &mtx, 1);
    BoneSetMatrix(&baseQuat, &mtx);

    dot = dir.x * axis.x + dir.y * axis.y + dir.z * axis.z;
    if (dot < lbl_804D93E0) {
        dot = lbl_804D93E0;
    } else if (dot > lbl_804D93B8) {
        dot = lbl_804D93B8;
    }
    angle = fn_800A109C(dot);
    actor->cfg.unk94C = angle;

    cross.x = dir.y * axis.z - dir.z * axis.y;
    cross.y = dir.z * axis.x - dir.x * axis.z;
    cross.z = dir.x * axis.y - dir.y * axis.x;
    NormalizeFxVector(&cross);

    progress = lbl_804D33F8 * fn_800A1394(angle * lbl_804D93CC) + lbl_804D33FC;
    progress *= dt;
    if (progress >= angle) {
        t = lbl_804D93B8;
    } else {
        t = lbl_804D93B8 - (angle - progress) / angle;
    }
    if (t < lbl_804D93A8) {
        t = lbl_804D93A8;
    } else if (t > lbl_804D93B8) {
        t = lbl_804D93B8;
    }

    C_QUATSlerp(&baseQuat, &dirQuat, &blended, t);

    BoneGetMatrix(&mtx, &blended);
    MtxGetEuler(&source, &mtx);
    actor->unk1F0 = source.x;
    actor->unk688.z = source.z;

    MtxSetEuler(&mtx, &actor->unk688);
    mtx.m[9] = (float)actor->unk24;
    mtx.m[10] = (float)actor->unk28;
    mtx.m[11] = (float)actor->unk2c;
    PlacementToMatrix((PlaceF*)actor, &mtx);

    if (fabsf(dirQuat.x - blended.x + dirQuat.y - blended.y +
              dirQuat.z - blended.z + dirQuat.w - blended.w) > lbl_804D93D8) {
        WhiteOutEffectQuery(actor->unk38);
        return 0;
    }
    return 1;
}

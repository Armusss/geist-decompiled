// u_8024E5D0.cpp: host-ride behaviour state machine (0x8024E5D0-0x80251CB8).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;

class Actor;

class String {
public:
    String();
    String(const char* text);
    ~String();
    void* vtbl;      // 0x00
    int   unk04;     // 0x04
    int   unk08;     // 0x08
    u8    unk0c[8];  // 0x0c
};

struct HostBehaviour;

// state table entry, stride 0x18
struct HostStateEntry {
    int  unk00;                  // 0x00
    void (*enter)(void*);        // 0x04
    int  (*check)(void*, int);   // 0x08
    int  (*update)(void*);       // 0x0c
    void (*unk10)(void*);        // 0x10
    const char* name;            // 0x14
};

extern HostStateEntry lbl_803BEB88[14];

struct HostBehaviour {
    int   unk00;      // 0x00
    int   unk04;      // 0x04
    int   state;      // 0x08
    int   unk0c;      // 0x0c
    float unk10;      // 0x10
    int   unk14;      // 0x14
    float unk18;      // 0x18
    float unk1c;      // 0x1c
    float unk20;      // 0x20
    int   unk24;      // 0x24
    int   unk28;      // 0x28
    int   stream;     // 0x2c
    char  unk30;      // 0x30
    u8    unk31;      // 0x31
    u8    unk32[2];   // 0x32
};

struct Vec3 {
    float x;  // 0x000
    float y;  // 0x004
    float z;  // 0x008
};

struct HostAssetSet {
    u8  unk000[0x8];           // 0x000
    float unk008;              // 0x008
    u8  unk00c[0x14 - 0x0c];   // 0x00c
    int unk014;                // 0x014
    u8  unk018[0x1c - 0x18];   // 0x018
    int unk01c;                // 0x01c
    u8  unk020[0xc0 - 0x20];   // 0x020
    float unk0c0;              // 0x0c0
    u8  unk0c4[0x124 - 0xc4];  // 0x0c4
    unsigned int unk124;       // 0x124
    unsigned int unk128;       // 0x128
    unsigned int unk12c;       // 0x12c
};

// Integer copy of the ride anchor placement embedded near the start of the
// actor (descriptive layout).
struct HostPlaceEmbed {
    int x;         // 0x00 (actor + 0x24)
    int y;         // 0x04 (actor + 0x28)
    int z;         // 0x08 (actor + 0x2c)
    u8  unk0c[8];  // 0x0c (actor + 0x30)
};

struct HostActor {
    u8  unk000[0x24];           // 0x000
    HostPlaceEmbed unk024;      // 0x024
    int unk038;                 // 0x038
    u8  unk03c[0x84 - 0x3c];    // 0x03c
    Vec3 pos;                   // 0x084
    u8  unk090[0xc4 - 0x90];    // 0x090
    HostAssetSet* unk0c4;       // 0x0c4
    int unk0c8;                 // 0x0c8
    int unk0cc;                 // 0x0cc
    int unk0d0;                 // 0x0d0
    int unk0d4;                 // 0x0d4
    u8  unk0d8[0xf0 - 0xd8];    // 0x0d8
    void* unk0f0;               // 0x0f0
    u8  unk0f4[0x110 - 0xf4];   // 0x0f4
    void* unk110;               // 0x110
    u8  unk114[0x118 - 0x114];  // 0x114
    void (*unk118)();           // 0x118
    u8  unk11c[0x134 - 0x11c];  // 0x11c
    HostBehaviour* beh;         // 0x134
    float unk138;               // 0x138
    float unk13c;               // 0x13c
    u8  unk140[0x240 - 0x140];  // 0x140
    Vec3 unk240;                // 0x240
    Vec3 unk24c;                // 0x24c
    u8  unk258[0x26c - 0x258];  // 0x258
    int unk26c;                 // 0x26c
    u8  cond[0x27c - 0x270];    // 0x270
    float unk27c;               // 0x27c
    float unk280;               // 0x280
    float unk284;               // 0x284
    int unk288;                 // 0x288
    u8  unk28c[0x894 - 0x28c];  // 0x28c
    u8  unk894[0x8f8 - 0x894];  // 0x894
    int unk8f8;                 // 0x8f8
    float unk8fc;               // 0x8fc
    u8  unk900[0x904 - 0x900];  // 0x900
    float unk904;               // 0x904
    u8  unk908[0x918 - 0x908];  // 0x908
    float unk918;               // 0x918
};

union HostValue {
    float f;  // 0x0c
    int   i;  // 0x0c
};

struct HostMsg {
    HostActor* actor;  // 0x00
    int hash;          // 0x04
    int unk08;         // 0x08
    HostValue value;   // 0x0c
    int unk10;         // 0x10
    int hash2;         // 0x14
};

struct HostMsg41eOut {
    int unk00;  // 0x00
    int unk04;  // 0x04
    int unk08;  // 0x08
    int hash;   // 0x0c
};

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int fn_8008EA9C(int handle, int mode);
extern "C" void fn_800225C8(void* out, const void* a, const void* b);
extern "C" void fn_8028C188(void* out, int a, int b);
void HostRideEffectUpdate(void* actor);
int HostRideSkinUpdate(void* msg, int arg);

int HasCondition(void* cond, int id);
void ActorRender(Actor* actor);

extern float lbl_804D35D0;
extern float lbl_804D35D4;
extern float lbl_804D35D8;
extern float lbl_804D35DC;
extern float lbl_804D9D70;
extern float lbl_804D9D74;
extern const char lbl_8034A628[];

int HostStateMsgHandler(int msg, void* data)
{
    HostMsg* p = (HostMsg*)data;
    HostActor* a = p->actor;
    HostBehaviour* b = a->beh;

    if (msg == 0x41e) {
        if (p->hash != (int)0xAD856AFA) {
            return 0;
        }
        {
            String name(lbl_803BEB88[b->state].name);
            String suffix(lbl_8034A628);
            String text;
            fn_800225C8(&text, &name, &suffix);
            fn_8028C188(&p->value, text.unk04, text.unk08);
        }
        return 0;
    }
    if (msg < 0x41e) {
        if (msg == 0x407) {
            int h = p->hash2;

            if (h == 0x6d57da0a) {
                p->value.f *= lbl_804D35D4;
            } else if (h < 0x6d57da0a) {
                if (h == (int)0xCF0262AA) {
                    p->value.f *= lbl_804D35D8;
                } else if (h < (int)0xCF0262AA && h == (int)0xC8769B80) {
                    p->value.f *= lbl_804D35D0;
                }
            } else if (h == 0x75715902) {
                p->value.f *= lbl_804D35DC;
            }

            if (a->unk904 <= p->value.f) {
                int state = b->state;
                if (state == 5 || state == 2) {
                    p->value.f = a->unk904 - lbl_804D9D70;
                }
                if (a->unk904 > lbl_804D9D74 && a->unk904 <= p->value.f) {
                    HostMsg41eOut out;
                    out.unk04 = a->unk038;
                    out.unk08 = p->unk08;
                    out.hash = 0x7c3f4e3e;
                    SendMsg(0x4B000000, 0xc14, &out);
                }
            }
            return 0;
        }
        if (msg < 0x407) {
            if (msg == 0) {
                return HostRideSkinUpdate(data, 0);
            }
        } else if (msg > 0x41c) {
            if (p->hash != 0x56d8c010) {
                return 0;
            }
            lbl_803BEB88[b->state].check(a, b->unk14);
            lbl_803BEB88[7].enter(a);
            b->unk31 = 100;
            return 1;
        }
    } else {
        if (msg == 0x7300) {
            return HostRideSkinUpdate(data, 1);
        }
        if (msg < 0x7300 && msg == 0xc14) {
            if (p->value.i != (int)0xFCFCA566) {
                return 0;
            }
            if (b->stream != -1) {
                int buf[12];
                SendMsg(b->stream, 0x400, buf);
                ActorRender((Actor*)buf[0]);
                b->stream = -1;
            }
            return 1;
        }
    }
    return 0;
}

int HostStateMachineTick(void* actorArg)
{
    HostActor* a = (HostActor*)actorArg;
    HostBehaviour* b = a->beh;
    int next;

    if ((fn_8008EA9C(a->unk0d4, 0) == 0 || fn_8008EA9C(a->unk0d4, 0) == 2) &&
        HasCondition(a->cond, 0xd) == 0 && HasCondition(a->cond, 0xa) == 0) {
        lbl_803BEB88[b->state].enter(a);
    } else if (HasCondition(a->cond, 0xd) != 0) {
        HostRideEffectUpdate(a);
        return 1;
    }

    next = lbl_803BEB88[b->state].update(a);
    if (next != b->state && lbl_803BEB88[b->state].check(a, next) != 0) {
        lbl_803BEB88[next].enter(a);
    }
    return 1;
}

extern "C" int rand();
extern "C" void fn_800CDD10();
extern "C" void fn_800381F8(int handle, int a, int b, int c, int d, int e);
void* VAlloc(int size);
void VFree(void* p);
int FxFind(const char* name);
int AssetFind(unsigned long id, unsigned long sub);

extern int lbl_804D35E0;
extern int lbl_804D35E4;
extern int lbl_804D35E8;
extern int lbl_804D35EC;
extern int lbl_804D35F0;
extern int lbl_804D35F4;
extern int lbl_804D35F8;
extern int lbl_804D35FC;

// Releases the host-ride behaviour of an actor (descriptive name).
void HostBehaviourDestroy(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;

    if (actor != 0) {
        VFree(actor->beh);
        actor->unk110 = 0;
        actor->beh = 0;
    }
}

// Allocates and initialises the host-ride behaviour of an actor (descriptive name).
void HostBehaviourCreate(int handle, HostActor* actor)
{
    HostBehaviour* beh;

    if (actor == 0) {
        return;
    }
    beh = (HostBehaviour*)VAlloc(0x34);
    if (beh == 0) {
        actor->beh = 0;
        return;
    }
    actor->beh = beh;
    actor->unk110 = (void*)HostStateMachineTick;
    actor->unk118 = fn_800CDD10;
    beh->unk00 = handle;
    beh->unk28 = -1;
    beh->unk24 = 0;
    beh->unk04 = -1;
    beh->unk0c = 0;
    beh->unk10 = lbl_804D9D74;
    beh->unk18 = lbl_804D9D74;
    beh->unk1c = lbl_804D9D74;
    beh->unk20 = lbl_804D9D74;
    beh->stream = -1;
    beh->unk30 = rand() % 256;
    lbl_804D35EC = FxFind("imp_pants");
    lbl_804D35F0 = FxFind("trib_attack");
    lbl_804D35F4 = FxFind("trib_idle");
    lbl_804D35F8 = FxFind("imp_explode");
    lbl_804D35FC = AssetFind(0xD9044CBB, 0x1F31A2);
    lbl_804D35E0 = AssetFind(0x87DE5E6A, actor->unk0c4->unk124);
    lbl_804D35E4 = AssetFind(0x2FA2416F, actor->unk0c4->unk128);
    lbl_804D35E8 = AssetFind(0xF2E2BEB8, actor->unk0c4->unk12c);
    beh->state = 0;
    beh->unk14 = 0;
    fn_800381F8(actor->unk0cc, 1, 0x32, 0x32, 200, 0xff);
}

extern "C" int fn_800C8198();
extern "C" void fn_8000F550(void* id, int flag);
extern "C" void* fn_800C7E80(Vec3* pos);
extern "C" void fn_800CB8BC(void* actor, HostAssetSet* set, int a, int b);
extern "C" void* memset(void* dst, int value, unsigned long size);
extern unsigned char __upper_map[];
extern float lbl_804D9D78;
extern float lbl_804D9D7C;
extern float lbl_804D9D80;
extern float lbl_804D9D84;
extern float lbl_804D9D9C;
extern float lbl_804D9DA0;

extern "C" void fn_8008F4E8(int handle, int arg);
extern "C" void fn_8008F378(int handle);
extern "C" void fn_8008F4EC(int handle, int arg);
extern "C" void fn_8008F230(int handle, int a, int b, float value);
extern "C" void fn_80090974(int handle, int a, int anim, int mode,
                             void (*callback)(void*), void* actor, float time);
extern "C" int fn_800A1BC8(unsigned short a, unsigned short b,
                            unsigned short c, int index, int arg);
int AnimBankGetSetCount(unsigned short a, unsigned short b, unsigned short c);

void Vec3Set(Vec3* v, float x, float y, float z);
void ModelNodeSetup(int node, int asset, int a, int b);

// Query block filled in for a ground/effect search (descriptive layout).
struct SpawnQuery {
    Vec3 pos;    // 0x00
    Vec3 unk0c;  // 0x0c
    int  unk18;  // 0x18
    int  unk1c;  // 0x1c
    int  unk20;  // 0x20
    int  unk24;  // 0x24
    int  unk28;  // 0x28
    int  unk2c;  // 0x2c
    int  unk30;  // 0x30
    int  unk34;  // 0x34
};

struct SpawnNode {
    u8  unk00[0x38];  // 0x00
    int unk38;        // 0x38
};

struct SpawnMsg {
    int unk00;        // 0x00
    int unk04;        // 0x04
    int unk08;        // 0x08
    int unk0c;        // 0x0c
    u8  unk10[0x10];  // 0x10
};

// Effect/asset message handler of the host-ride actor (descriptive name).
int HostRideEffectMsgHandler(int msg, void* data)
{
    HostActor* actor = (HostActor*)data;
    HostBehaviour* beh = actor->beh;

    switch (msg) {
    case 0x1B83833F:
        {
            short id = lbl_804D35E8;
            fn_8000F550(&id, 1);
        }
        lbl_803BEB88[beh->state].check(actor, beh->unk14);
        lbl_803BEB88[beh->unk14].enter(actor);
        break;
    case (int)0x8E6AAEC8:
        {
            unsigned int hash = 0;
            const char* name = "BODY";
            const char* p = name;

            while (*p != 0) {
                unsigned int c;
                if ((signed char)*name == -1) {
                    c = 0xFFFFFFFF;
                } else {
                    c = __upper_map[(unsigned char)*name];
                }
                hash = hash * 31 + (c & 0xFF);
                p++;
                name++;
            }
            ModelNodeSetup(actor->unk0cc, AssetFind(0xD9044CBB, hash % 0xFFFFFFFFu), 0, 1);
        }
        break;
    case (int)0xB9FC42F5:
        {
            SpawnQuery q;
            SpawnNode* node;

            if (beh->stream != -1) {
                return 1;
            }
            if (fn_800C8198() == 0) {
                return 1;
            }
            memset(&q, 0, sizeof(q));
            q.unk1c = lbl_804D35E0;
            q.unk20 = 0;
            q.unk28 = lbl_804D35E4;
            {
                unsigned int r = (unsigned int)rand();
                float t = (float)r;
                Vec3Set(&q.unk0c, lbl_804D9D7C * (t / lbl_804D9D78) - lbl_804D9D80, lbl_804D9D74,
                        lbl_804D9D74);
            }
            q.pos = actor->pos;
            q.pos.z = lbl_804D9D84 + actor->unk918;
            node = (SpawnNode*)fn_800C7E80(&q.pos);
            if (node != 0) {
                SpawnMsg m;

                memset(&m, 0, sizeof(m));
                m.unk0c = 0x18;
                m.unk04 = q.unk28;
                SendMsg(node->unk38, 0xc04, &m);
            }
            beh->stream = node->unk38;
        }
        break;
    case 0x04A5C9FC:
    case 0x239807:
    case 0x46341C3A:
        fn_800CB8BC(actor, actor->unk0c4, beh->unk24, actor->unk26c);
        break;
    }
    return 1;
}

extern "C" int fn_802A1D04();
extern "C" int fn_8020B97C(Actor* player);
// Called with two and with three arguments in this unit.
extern "C" int fn_80162610(void* obj, void* target, ...);
Actor* GetPlayer();
void ConditionUpdateBit(void* cond, int bit, int value);
void ConditionSetBit(void* cond, int bit, int value);

// Hit/damage message handler of the host-ride actor (descriptive name).
int HostRideHitMsgHandler(int msg, void* data)
{
    HostActor* actor = (HostActor*)data;
    HostBehaviour* beh = actor->beh;

    switch (msg) {
    case 0x1B83833F:
        fn_80162610(actor, actor->unk894);
        lbl_803BEB88[beh->state].check(actor, beh->unk14);
        lbl_803BEB88[beh->unk14].enter(actor);
        break;
    case 0x20EE36:
    case (int)0xF82ADB93:
        if (fn_802A1D04() == 0 && fn_8020B97C(GetPlayer()) != 0) {
            return 1;
        }
        if (beh->unk31 != 0) {
            u8 step = (beh->unk31 > 20) ? 20 : beh->unk31;

            beh->unk31 = beh->unk31 - step;
            fn_80162610(actor, actor->unk894, 0xF82ADB93);
        }
        if (beh->unk31 != 0) {
            return 1;
        }
        ConditionUpdateBit(actor->cond, 3, 0);
        ConditionUpdateBit(actor->cond, 0, 0);
        ConditionSetBit(actor->cond, 5, 1);
        break;
    case 0x04A5C9FC:
    case 0x239807:
    case 0x46341C3A:
        fn_800CB8BC(actor, actor->unk0c4, beh->unk24, actor->unk26c);
        break;
    }
    return 1;
}

// Forces the host-ride behaviour's target state, clearing condition bits 0 and 4 (descriptive name).
int HostRideSetTargetState(void* actorArg, int target)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;

    ConditionSetBit(actor->cond, 0, 0);
    ConditionSetBit(actor->cond, 4, 0);
    beh->unk14 = target;
    return 0;
}

// Requests a new target state, refusing while either state is one of the two locked
// kick states 2/5; returns 1 when the request was applied (descriptive name).
int HostRideTrySetTargetState(void* actorArg, int target)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;

    if ((beh->state == 2 || beh->state == 5) && (beh->unk14 == 2 || beh->unk14 == 5)) {
        return 0;
    }
    ConditionSetBit(actor->cond, 0, 0);
    ConditionSetBit(actor->cond, 4, 0);
    beh->unk14 = target;
    return 1;
}

extern float lbl_804D9D90;
extern float lbl_804D9D94;

// Stores the requested target state and a randomised delay for it (descriptive name).
int HostRideStateRandomWait(void* actorArg, int target)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;

    beh->unk14 = target;
    beh->unk1c = lbl_804D9D94 * ((float)rand() / lbl_804D9D78) + lbl_804D9D90;
    return 1;
}

// Stores the requested target state without a random delay (descriptive name).
int HostRideStateSetTarget(void* actorArg, int target)
{
    HostActor* actor = (HostActor*)actorArg;

    actor->beh->unk14 = target;
    return 1;
}

int HostRideGetState(HostActor* actor)
{
    if (actor != 0) {
        return actor->beh->state;
    }
    return 0;
}

int HostRideUpdateState(HostActor* actor)
{
    HostBehaviour* b = actor->beh;
    HostRideEffectUpdate(actor);
    return b->state;
}

extern "C" int fn_8016325C(void* area);
extern "C" int fn_8016F120(int handle, void* pos);
int MemFlagIsSet(void* cond, int bit);
extern float lbl_804D9D98;

// Chooses the next host-ride state from the actor's condition bits and target state,
// resetting the behaviour timer with a fresh random delay when it expires (descriptive name).
int HostRideUpdateConditionalState(HostActor* actor)
{
    HostBehaviour* beh = actor->beh;
    int result;
    int target;

    HostRideEffectUpdate(actor);
    if (HasCondition(actor->cond, 9)) {
        result = 0xc;
    } else if (HasCondition(actor->cond, 4)) {
        ConditionSetBit(actor->cond, 4, 0);
        result = 2;
    } else if (MemFlagIsSet(actor->cond, 3)) {
        result = 6;
    } else if (fn_8016325C(actor->unk894) == 4) {
        result = 6;
    } else if (fn_8016325C(actor->unk894) == 5) {
        result = 8;
    } else {
        target = actor->unk288;
        if (beh->unk0c != target) {
            if (target == 3) {
                return 10;
            }
            if (target < 3) {
                if (target == 0) {
                    return 3;
                }
                if (target >= 0) {
                    return 9;
                }
            }
        }
        {
            float f = beh->unk20 - actor->unk138;

            beh->unk20 = f;
            if (f <= lbl_804D9D74) {
                beh->unk20 = lbl_804D9D98 * ((float)rand() / lbl_804D9D78) + lbl_804D9D90;
                fn_8016F120(lbl_804D35F4, &actor->unk024);
            }
        }
        result = beh->state;
    }
    return result;
}

// Returns the behaviour state unless condition 9 blocks the update (descriptive name).
int HostRideUpdateStateChecked(HostActor* actor)
{
    HostBehaviour* beh = actor->beh;

    HostRideEffectUpdate(actor);
    if (HasCondition(actor->cond, 9)) {
        return 0xc;
    }
    return beh->state;
}

// Returns the behaviour state, or 0xc when condition 9 is set, but only while the
// actor's area reports type 5; otherwise the state 3 is returned (descriptive name).
int HostRideUpdateTypeCheckedState(HostActor* actor)
{
    HostBehaviour* beh = actor->beh;

    HostRideEffectUpdate(actor);
    if (fn_8016325C(actor->unk894) != 5) {
        return 3;
    }
    if (HasCondition(actor->cond, 9)) {
        return 0xc;
    }
    return beh->state;
}

// Advances the charge timer of the host-ride behaviour while the actor stands on the
// charge pad, clamping the displayed percentage and switching states when full
// (descriptive name).
int HostRideUpdateChargeState(HostActor* actor)
{
    HostBehaviour* beh = actor->beh;

    HostRideEffectUpdate(actor);
    if (HasCondition(actor->cond, 9)) {
        beh->unk31 = 0;
        return 0xc;
    }
    if (!MemFlagIsSet(actor->cond, 3)) {
        beh->unk31 = 0;
        return 4;
    }
    if (beh->state == 7) {
        beh->unk1c = beh->unk1c + actor->unk138;
        {
            int percent = (int)(lbl_804D9D84 * (beh->unk1c / lbl_804D9D90));

            if (percent < 0) {
                beh->unk31 = 0;
            } else if (percent < 0x65) {
                beh->unk31 = (u8)percent;
            } else {
                beh->unk31 = 100;
            }
        }
        if (beh->unk1c >= lbl_804D9D90) {
            ConditionUpdateBit(actor->cond, 3, 0);
            ConditionUpdateBit(actor->cond, 0, 1);
            return 4;
        }
    }
    return beh->state;
}

extern "C" void fn_8008DE4C(int handle, int a, int b, int c);
extern "C" void fn_8016EEF4(int handle);
extern "C" void fn_800FC764(int handle, int arg);
void ActorPlayAnim(int handle, unsigned long anim, int arg);

// Chooses the next host-ride state from the actor's condition bits, the requested
// target state and the two behaviour timers (descriptive name).
int HostRideUpdateTimerState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    int result;

    HostRideEffectUpdate(actor);
    if (HasCondition(actor->cond, 9)) {
        return 0xc;
    }
    if (beh->unk0c != actor->unk288) {
        if (actor->unk288 == 3) {
            return 10;
        }
        if (actor->unk288 < 3) {
            if (actor->unk288 == 0) {
                return 3;
            }
            if (actor->unk288 >= 0) {
                return 9;
            }
        }
    }
    if (MemFlagIsSet(actor->cond, 3)) {
        return 6;
    }
    if (fn_8016325C(actor->unk894) == 4) {
        return 6;
    }
    if (fn_8016325C(actor->unk894) == 5) {
        return 8;
    }
    if (beh->state < 3 && MemFlagIsSet(actor->cond, 3)) {
        return 3;
    }
    if (!HasCondition(actor->cond, 4) || HasCondition(actor->cond, 9) ||
        HasCondition(actor->cond, 10)) {
        float f = beh->unk20 - actor->unk138;

        beh->unk20 = f;
        if (f <= lbl_804D9D74) {
            beh->unk20 = lbl_804D9D98 * ((float)(unsigned int)rand() / lbl_804D9D78) + lbl_804D9D90;
            fn_8016F120(lbl_804D35F4, &actor->unk024);
        }
    } else {
        ConditionSetBit(actor->cond, 4, 0);
        return 2;
    }
    {
        float f = beh->unk1c - actor->unk138;

        beh->unk1c = f;
        if (lbl_804D9D74 < f) {
            return beh->state;
        }
        result = 1;
        if (beh->state > 2) {
            result = 4;
        }
    }
    return result;
}

// Enters the host-ride "riding" state of the behaviour and starts its animation
// (descriptive name).
int HostRideEnterRideState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;

    beh->state = 0xd;
    beh->unk14 = 0xd;
    fn_8008DE4C(actor->unk0d4, 0, beh->unk04, 0x18);
    if (!HasCondition(actor->cond, 10)) {
        ConditionSetBit(actor->cond, 10, 1);
        ActorPlayAnim(actor->unk038, 0xdc04e3d5, actor->unk038);
    }
    if (beh->unk28 != -1) {
        fn_8016EEF4(beh->unk28);
        beh->unk28 = -1;
    }
    if (fn_802A1D04() == 6) {
        fn_800FC764(actor->unk038, 0);
    }
    return 1;
}

// Enters the pulse/attack state of the host-ride behaviour: resets the state, picks a
// random animation from the state's bank and restarts the effect (descriptive name).
int HostBehaviourEnterPulseState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->state = 0xc;
    beh->unk14 = 0xd;
    beh->unk0c = 0;
    ConditionSetBit(actor->cond, 0xe, 1);
    beh = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh->state].unk00);
    if (count != 0) {
        oldAnim = beh->unk04;
        {
            int r = rand();

            beh->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                     lbl_803BEB88[beh->state].unk00,
                                     r - (r / count) * count, 0);
        }
        if (oldAnim == beh->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh->unk04, 0x18,
                    lbl_803BEB88[beh->state].unk10, actor, lbl_804D9D70);
    }
    HostRideEffectMsgHandler(0xb9fc42f5, actor);
    HostRideEffectMsgHandler(0x8e69aec8, actor);
    if (beh->unk28 != -1) {
        fn_8016EEF4(beh->unk28);
        beh->unk28 = -1;
    }
    fn_8016F120(lbl_804D35F8, &actor->unk024);
    return 1;
}

// Enters the wait state of the host-ride behaviour: resets the state, resets the charge
// value and picks a random animation from the state's bank (descriptive name).
int HostBehaviourEnterWaitState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->state = 0xb;
    beh->unk14 = 0xd;
    beh->unk0c = 0;
    actor->unk904 = lbl_804D9DA0;
    beh = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh->state].unk00);
    if (count != 0) {
        oldAnim = beh->unk04;
        {
            int r = rand();

            beh->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                     lbl_803BEB88[beh->state].unk00,
                                     r - (r / count) * count, 0);
        }
        if (oldAnim == beh->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh->unk04, 0x18,
                    lbl_803BEB88[beh->state].unk10, actor, lbl_804D9D70);
    }
    if (beh->unk28 != -1) {
        fn_8016EEF4(beh->unk28);
        beh->unk28 = -1;
    }
    fn_8016F120(lbl_804D35F8, &actor->unk024);
    return 1;
}

// Enters the attack state of the host-ride behaviour: resets the state and picks a
// random animation from the state's bank (descriptive name).
int HostBehaviourEnterAttackState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->state = 10;
    beh->unk14 = 10;
    beh->unk0c = 3;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 0, lbl_803BEB88[beh2->state].unk10,
                    actor, lbl_804D9D70);
    }
    if (beh->unk30 == 0) {
        if (beh->unk28 == -1) {
            beh->unk28 = fn_8016F120(lbl_804D35EC, &actor->unk024);
        }
    } else {
        beh->unk30 = beh->unk30 - 1;
    }
    return 1;
}

// Enters the search state of the host-ride behaviour: resets the state and picks a
// random animation from the state's bank (descriptive name).
int HostBehaviourEnterSearchState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->state = 9;
    beh->unk14 = 9;
    beh->unk0c = 2;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 0, lbl_803BEB88[beh2->state].unk10,
                    actor, lbl_804D9D70);
    }
    if (beh->unk30 == 0) {
        if (beh->unk28 == -1) {
            beh->unk28 = fn_8016F120(lbl_804D35EC, &actor->unk024);
        }
    } else {
        beh->unk30 = beh->unk30 - 1;
    }
    return 1;
}

// Enters the flinch/hurt state of the host-ride behaviour: remembers the previous
// state, picks a random animation from the state's bank, restarts the effect and
// counts the hurt timer down, clearing condition bits 0 and 3 when it expires
// (descriptive name).
int HostBehaviourEnterFlinchState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostBehaviour* beh3;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    if (beh->state != 8) {
        beh->unk14 = beh->state;
    }
    beh->state = 8;
    beh->unk0c = 0;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 9,
                    lbl_803BEB88[beh2->state].unk10, actor, lbl_804D9D70);
    }
    if (beh->unk28 != -1) {
        fn_8016EEF4(beh->unk28);
        beh->unk28 = -1;
    }
    beh3 = actor->beh;
    if (fn_802A1D04() == 0) {
        if (fn_8020B97C(GetPlayer()) != 0) {
            return 1;
        }
    }
    if (beh3->unk31 != 0) {
        u8 step = (beh3->unk31 > 20) ? 20 : beh3->unk31;

        beh3->unk31 = beh3->unk31 - step;
        fn_80162610(actor, actor->unk894, 0xF829DB93);
    }
    if (beh3->unk31 == 0) {
        ConditionUpdateBit(actor->cond, 3, 0);
        ConditionUpdateBit(actor->cond, 0, 0);
        ConditionSetBit(actor->cond, 5, 1);
    }
    return 1;
}

// Enters the idle/rest state of the host-ride behaviour: resets the state, the wait
// value and picks a random animation from the state's bank (descriptive name).
int HostBehaviourEnterIdleState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    if (beh->state != 7) {
        beh->unk1c = lbl_804D9D74;
    }
    beh->state = 7;
    beh->unk14 = 7;
    beh->unk0c = 0;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 9,
                    lbl_803BEB88[beh2->state].unk10, actor, lbl_804D9D70);
    }
    if (beh->unk28 != -1) {
        fn_8016EEF4(beh->unk28);
        beh->unk28 = -1;
    }
    return 1;
}

extern "C" int fn_800C5654(int arg0, unsigned int hash, int arg2);

// Enters state 6 of the host-ride behaviour: resets the state, picks a random animation
// from the state's bank, restarts the effect and forwards a message (descriptive name).
int HostBehaviourEnterState6(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->state = 6;
    beh->unk14 = 7;
    beh->unk0c = 0;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 0x18,
                    lbl_803BEB88[beh2->state].unk10, actor, lbl_804D9D70);
    }
    fn_8016F120(lbl_804D35F0, &actor->unk024);
    if (beh->unk30 == 0) {
        if (beh->unk28 == -1) {
            beh->unk28 = fn_8016F120(lbl_804D35EC, &actor->unk024);
        }
    } else {
        beh->unk30 = beh->unk30 - 1;
    }
    fn_800C5654(actor->unk038, 0xa30cfd53, actor->unk038);
    return 1;
}

extern float lbl_804D9DA4;

// Enters state 5 of the host-ride behaviour: resets the state, picks a random animation
// from the state's bank and restarts the effect (descriptive name).
int HostBehaviourEnterState5(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->unk18 = lbl_804D9DA4;
    beh->state = 5;
    beh->unk14 = 3;
    beh->unk0c = 0;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 8,
                    lbl_803BEB88[beh2->state].unk10, actor, lbl_804D9D70);
    }
    if (beh->unk30 == 0) {
        if (beh->unk28 == -1) {
            beh->unk28 = fn_8016F120(lbl_804D35EC, &actor->unk024);
        }
    } else {
        beh->unk30 = beh->unk30 - 1;
    }
    return 1;
}

// Enters the mount state of the host-ride behaviour: keeps the previous state as the
// return state, resets the flags and restarts the effect with a random animation
// (descriptive name).
int HostBehaviourEnterMountState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->unk14 = beh->state;
    beh->state = 4;
    beh->unk0c = 0;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 9, lbl_803BEB88[beh2->state].unk10,
                    actor, lbl_804D9D70);
    }
    if (beh->unk30 == 0) {
        if (beh->unk28 == -1) {
            beh->unk28 = fn_8016F120(lbl_804D35EC, &actor->unk024);
        }
    } else {
        beh->unk30 = beh->unk30 - 1;
    }
    return 1;
}

// Enters the chase state of the host-ride behaviour: resets the state flags and
// restarts the effect with a random animation from the state's bank (descriptive name).
int HostBehaviourEnterChaseState(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->state = 3;
    beh->unk14 = 3;
    beh->unk0c = 0;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 9, lbl_803BEB88[beh2->state].unk10,
                    actor, lbl_804D9D70);
    }
    if (beh->unk30 == 0) {
        if (beh->unk28 == -1) {
            beh->unk28 = fn_8016F120(lbl_804D35EC, &actor->unk024);
        }
    } else {
        beh->unk30 = beh->unk30 - 1;
    }
    return 1;
}

// Enters state 2 of the host-ride behaviour: resets the state, picks a random animation
// from the state's bank and restarts the effect (descriptive name).
int HostBehaviourEnterState2(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->unk18 = lbl_804D9DA4;
    beh->state = 2;
    beh->unk14 = 3;
    beh->unk0c = 0;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 8,
                    lbl_803BEB88[beh2->state].unk10, actor, lbl_804D9D70);
    }
    if (beh->unk30 == 0) {
        if (beh->unk28 == -1) {
            beh->unk28 = fn_8016F120(lbl_804D35EC, &actor->unk024);
        }
    } else {
        beh->unk30 = beh->unk30 - 1;
    }
    return 1;
}

// Enters state 1 of the host-ride behaviour: remembers the previous state, picks a
// random animation from the state's bank and restarts the effect (descriptive name).
int HostBehaviourEnterState1(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->unk14 = beh->state;
    beh->state = 1;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (oldAnim == beh2->unk04) {
            same = 1;
        }
        if (HasCondition(actor->cond, 0xe) != 0 || same != 0) {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        } else {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 9,
                    lbl_803BEB88[beh2->state].unk10, actor, lbl_804D9D70);
    }
    beh->unk0c = 0;
    if (beh->unk30 == 0) {
        if (beh->unk28 == -1) {
            beh->unk28 = fn_8016F120(lbl_804D35EC, &actor->unk024);
        }
    } else {
        beh->unk30 = beh->unk30 - 1;
    }
    return 1;
}

// Float placement matrix, 3x3 rotation followed by a translation (descriptive layout).
struct Mtx34 {
    float m[9];  // 0x00
    Vec3  pos;   // 0x24
};

// Float placement as the render/matrix helpers see it (descriptive layout).
struct PlaceF {
    float m[9];  // 0x00
    Vec3  pos;   // 0x24
};

// Placement of the bone the ride effect is attached to (descriptive layout).
struct HostPlace {
    u8    unk00[0x10];  // 0x00
    float x;            // 0x10
    float y;            // 0x14
    float z;            // 0x18
};

void PlacementToMatrix(Mtx34* mtx, PlaceF* place);
void ModelSetMatrix(int model, Mtx34* mtx, char* bone);
int ModelAnimate(int model, char* bone, int flag);
void AnimApply(int handle, char* bone, int anim, int arg);
void BoneGetMatrix(Mtx34* mtx, void* bone);
void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void MtxGetEuler(Vec3* euler, Mtx34* mtx);
void MtxSetEuler(Mtx34* mtx, Vec3* euler);
void EulerNormalize(Vec3* a, Vec3* b);
extern "C" void fn_800913EC(int handle, float value, Vec3* out);
extern "C" float fn_800CC308(void* actor, int a, int b, int c, int d);
extern "C" void fn_800A0168(void* a, void* b);
extern "C" void fn_800A0030(void* out, void* a, void* b);
extern "C" float fn_800E1020(void* p);
extern "C" void fn_800E1098(void* out, void* in, float value);
extern "C" void fn_8016D4EC(void* a, void* b);
extern float lbl_804D9DA8;
extern float lbl_804D9DAC;
extern float lbl_804D9D94;
extern float lbl_804D9DB0;
extern float lbl_804D9DB4;

// Per-frame update of host-ride state 0: resets the state, picks a random animation
// from the state's bank and restarts the effect (descriptive name).
int HostBehaviourUpdateState0(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostBehaviour* beh2;
    HostAssetSet* set;
    int count;
    int oldAnim;
    int same = 0;

    beh->state = 0;
    beh->unk14 = 0;
    beh2 = actor->beh;
    ConditionSetBit(actor->cond, 0xd, 0);
    if (HasCondition(actor->cond, 0xc) != 0) {
        ConditionSetBit(actor->cond, 0xc, 0);
        ConditionSetBit(actor->cond, 0xe, 1);
        fn_8008F4E8(actor->unk0d4, 0);
        fn_8008F378(actor->unk0d4);
        fn_8008F4EC(actor->unk0d4, 0);
    }
    set = actor->unk0c4;
    count = AnimBankGetSetCount(set->unk014, set->unk01c, lbl_803BEB88[beh2->state].unk00);
    if (count != 0) {
        oldAnim = beh2->unk04;
        {
            int r = rand();

            beh2->unk04 = fn_800A1BC8(set->unk014, set->unk01c,
                                      lbl_803BEB88[beh2->state].unk00,
                                      r - (r / count) * count, 0);
        }
        if (HasCondition(actor->cond, 0xe) == 0 && oldAnim != beh2->unk04) {
            fn_8008F230(actor->unk0d4, 0, 1, lbl_804D9D9C);
        } else {
            fn_8008F230(actor->unk0d4, 0, 0, lbl_804D9D74);
            ConditionSetBit(actor->cond, 0xe, 0);
        }
        ConditionSetBit(actor->cond, 0, 1);
        fn_80090974(actor->unk0d4, 0, beh2->unk04, 9,
                    lbl_803BEB88[beh2->state].unk10, actor, lbl_804D9D70);
    }
    beh->unk0c = 0;
    beh->unk20 = lbl_804D9D98 * ((float)rand() / lbl_804D9D78) + lbl_804D9D90;
    if (beh->unk30 == 0) {
        if (beh->unk28 == -1) {
            beh->unk28 = fn_8016F120(lbl_804D35EC, &actor->unk024);
        }
    } else {
        beh->unk30 = beh->unk30 - 1;
    }
    return 1;
}

class Skin;

// Glow message sent to the ride effect stream (descriptive layout).
struct HostGlowMsg {
    int unk00;  // 0x00
    int unk04;  // 0x04
    int unk08;  // 0x08
    u8  unk0c;  // 0x0c
    u8  unk0d;  // 0x0d
    u8  unk0e;  // 0x0e
    u8  unk0f;  // 0x0f
};

extern float lbl_804D9DB8;
extern float lbl_804D9DBC;
extern float lbl_804D9DC0;
extern "C" int fn_800AFD20(void* actor);
extern "C" int fn_8003A1F0(int anim, void* bone, int index);
extern "C" void fn_8003F15C(float value, int anim, int blend, int handle,
                             void* skin, int arg);
extern "C" int fn_800BA84C(int id);
int SkinBlendTint(Skin* skin, int a, int b, int c, float tint);

// Per-frame update of the rider attachment: animates the ride bone, keeps the
// actor placement in sync and writes the model matrix (descriptive name).
void HostRideEffectUpdate(void* actorArg)
{
    HostActor* actor = (HostActor*)actorArg;
    HostBehaviour* beh = actor->beh;
    HostPlace* bone = (HostPlace*)actor->unk0f0;
    Vec3 vec;
    Vec3 euler;
    Mtx34 mat0;
    Mtx34 mat1;
    Mtx34 mtx;
    int anim;

    fn_800913EC(actor->unk0d4, actor->unk138, &vec);
    anim = ModelAnimate(actor->unk0cc, (char*)actor->unk0f0, 1);
    AnimApply(actor->unk0d4, (char*)actor->unk0f0, anim, actor->unk0d0);

    if (HasCondition(actor->cond, 0xc) != 0) {
        if (fn_8008EA9C(actor->unk0d4, 0) != 0) {
            if (HasCondition(actor->cond, 0xc) != 0) {
                BoneGetMatrix(&mtx, actor->unk0f0);
                MtxGetEuler(&actor->unk24c, &mtx);
                // Retail passes the actor itself as the placement source; the
                // shared prototype keeps the matrix type first.
                PlacementToMatrix((Mtx34*)actor, (PlaceF*)&mtx);
                actor->pos.x = bone->x;
                actor->pos.y = bone->y;
                actor->pos.z = bone->z;
                actor->unk024.x = (int)bone->x;
                actor->unk024.y = (int)bone->y;
                actor->unk024.z = (int)bone->z;
            } else {
                Vec3Set(&euler, actor->unk24c.z, lbl_804D9D74, lbl_804D9D74);
                EulerNormalize(&euler, &euler);
                MtxSetEuler(&mtx, &euler);
                mtx.pos.x = (float)actor->unk024.x;
                mtx.pos.y = (float)actor->unk024.y;
                mtx.pos.z = (float)actor->unk024.z;
                PlacementToMatrix((Mtx34*)actor, (PlaceF*)&mtx);
            }
            ModelSetMatrix(actor->unk0cc, &mtx, (char*)actor->unk0f0);
        }
    } else if (HasCondition(actor->cond, 0xd) != 0) {
        float resp = fn_800CC308(actor, 100, 1, 0, 0);
        float scale;

        if (lbl_804D9DA8 != resp) {
            float target = (actor->pos.z - beh->unk10) -
                           lbl_804D9DAC * lbl_804D9D94 * actor->unk138;
            float step = (resp > target) ? resp : target;

            actor->pos.z = bone->z + step;
            if (target > resp) {
                actor->unk26c = 0;
            }
            beh->unk10 = bone->z;
        }
        ActorGetMatrix(&mat1, (Actor*)actor);
        fn_800A0168(&mat1, &mat1);
        fn_800A0030(&vec, &mat1, &vec);
        actor->unk240 = vec;
        actor->unk240.z = lbl_804D9D74;
        scale = lbl_804D9DAC * actor->unk138;
        actor->unk240.x *= scale;
        actor->unk240.y *= scale;
        actor->unk240.z *= scale;
        Vec3Set(&euler, actor->unk24c.z, lbl_804D9D74, lbl_804D9D74);
        EulerNormalize(&euler, &euler);
        MtxSetEuler(&mtx, &euler);
        mtx.pos.x = (float)actor->unk024.x;
        mtx.pos.y = (float)actor->unk024.y;
        mtx.pos.z = (float)actor->unk024.z;
        PlacementToMatrix((Mtx34*)actor, (PlaceF*)&mtx);
        bone->x = lbl_804D9D74;
        bone->y = lbl_804D9D74;
        bone->z = lbl_804D9D74;
        ModelSetMatrix(actor->unk0cc, &mtx, (char*)actor->unk0f0);
    } else {
        float limit = actor->unk0c4->unk008;
        float fx = actor->unk24c.x;
        float resp;
        float scale;
        float v284;
        float v27c;

        if (fx < -limit) {
            fx = -limit;
        } else if (fx > limit) {
            fx = limit;
        }
        actor->unk24c.x = fx;

        resp = fn_800CC308(actor, 100, 1, 0, 0);
        if (lbl_804D9DA8 != resp) {
            float target = (actor->pos.z - beh->unk10) -
                           lbl_804D9DAC * lbl_804D9D94 * actor->unk138;
            float step = (resp > target) ? resp : target;

            actor->pos.z = bone->z + step;
            if (target > resp) {
                actor->unk26c = 0;
            }
            beh->unk10 = bone->z;
        }
        actor->unk024.z = (int)actor->pos.z;
        bone->x = lbl_804D9D74;
        bone->y = lbl_804D9D74;
        bone->z = lbl_804D9D74;

        v284 = actor->unk284;
        v27c = actor->unk27c;
        if (v284 > lbl_804D9DB0) {
            float step = lbl_804D9DAC * actor->unk0c4->unk0c0 * actor->unk138;

            if (step >= v284) {
                step = v284;
            }
            actor->unk24c.z += step;
        } else if (v284 < lbl_804D9DB4) {
            float step = -(lbl_804D9DAC * actor->unk0c4->unk0c0 * actor->unk138);

            if (step <= v284) {
                step = v284;
            }
            actor->unk24c.z += step;
        }
        if (v27c > lbl_804D9DB0) {
            float step = lbl_804D9DAC * actor->unk0c4->unk0c0 * actor->unk138;

            if (step >= v27c) {
                step = v27c;
            }
            actor->unk24c.x += step;
        } else if (v27c < lbl_804D9DB4) {
            float step = -(lbl_804D9DAC * actor->unk0c4->unk0c0 * actor->unk138);

            if (step <= v27c) {
                step = v27c;
            }
            actor->unk24c.x += step;
        }

        EulerNormalize(&actor->unk24c, &actor->unk24c);
        if (HasCondition(actor->cond, 0) != 0) {
            ActorGetMatrix(&mat0, (Actor*)actor);
            fn_800A0168(&mat0, &mat0);
            fn_800A0030(&vec, &mat0, &vec);
            actor->unk240 = vec;
            actor->unk240.z = lbl_804D9D74;
            scale = lbl_804D9DAC * actor->unk138;
            actor->unk240.x *= scale;
            actor->unk240.y *= scale;
            actor->unk240.z *= scale;
        }
        if (HasCondition(actor->cond, 0) == 0 &&
            MemFlagIsSet(actor->cond, 4) == 0) {
            float len = fn_800E1020(&actor->unk240);

            fn_8016D4EC(&actor->unk240, &actor->unk24c);
            fn_800E1098(&actor->unk240, &actor->unk240, len);
        }
        Vec3Set(&euler, actor->unk24c.z, lbl_804D9D74, lbl_804D9D74);
        MtxSetEuler(&mtx, &euler);
        mtx.pos.x = (float)actor->unk024.x;
        mtx.pos.y = (float)actor->unk024.y;
        mtx.pos.z = (float)actor->unk024.z;
        PlacementToMatrix((Mtx34*)actor, (PlaceF*)&mtx);
        ModelSetMatrix(actor->unk0cc, &mtx, (char*)actor->unk0f0);
    }
}

// Per-frame skin tint and ride-glow update of the host-ride behaviour
// (descriptive name).
int HostRideSkinUpdate(void* msgArg, int arg)
{
    HostMsg* msg = (HostMsg*)msgArg;
    HostActor* actor = msg->actor;
    HostBehaviour* beh = actor->beh;
    HostGlowMsg glow;
    Skin* skin;
    int anim;
    int blend;
    float alpha = lbl_804D9D70;

    if (actor->unk13c > lbl_804D9DB8) {
        ActorPlayAnim(actor->unk038, 0xDBE2CAE6, actor->unk038);
    }
    actor->unk13c = lbl_804D9D74;
    anim = fn_800AFD20(actor);
    blend = fn_8003A1F0(anim, actor->unk0f0, ModelAnimate(actor->unk0cc, 0, 0));
    skin = (Skin*)&msg->unk10;
    if (beh->unk31 != 0) {
        float t = (float)beh->unk31 / lbl_804D9D84 * lbl_804D9DBC;

        if (t < lbl_804D9D74) {
            t = lbl_804D9D74;
        } else if (t > lbl_804D9D70) {
            t = lbl_804D9D70;
        }
        blend = SkinBlendTint(skin, 0, 0, 0xff, t);
    }
    if (actor->unk8f8 == 6) {
        int total = fn_800BA84C(9);
        float ratio = ((float)total - actor->unk8fc) / (float)total;

        if (ratio < lbl_804D9D74) {
            ratio = lbl_804D9D74;
        } else if (ratio > lbl_804D9D70) {
            ratio = lbl_804D9D70;
        }
        alpha = ratio;
        if (ratio == lbl_804D9D74) {
            return 1;
        }
        if (beh->stream != -1) {
            glow.unk04 = -1;
            glow.unk08 = 1;
            glow.unk0c = 0xff;
            glow.unk0d = 0xff;
            glow.unk0e = 0xff;
            glow.unk0f = (u8)(int)(lbl_804D9DC0 * ratio);
            SendMsg(beh->stream, 0xc15, &glow);
        }
    }
    fn_8003F15C(alpha, anim, blend, actor->unk0d0, skin, 0);
    return 1;
}

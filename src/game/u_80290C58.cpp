// u_80290C58.cpp: host follow, offset and carrier states (0x80290C58-0x80294548).
// Names are descriptive guesses; the code is matched against the retail DOL.

// u_80290C58.cpp: host actor behaviour-state dispatch and message handling.
// Type and function names are descriptive guesses based on the code.

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct Host;
struct HostBehaviour;
struct HostAnim;

// Entry of the host's behaviour-state table (lbl_803C2B38, 13 * 0x10 bytes).
struct HostStateEntry {
    int (*action)(void* actor);   // 0x00
    int (*check)(void* actor);    // 0x04
    int (*update)(void* actor);   // 0x08
    void* typeName;               // 0x0c  (state name string)
};

struct HostBehaviour {
    s32 state;                    // 0x00
    s32 field04;                  // 0x04
    s32 field08;                  // 0x08
    s32 target;                   // 0x0c
    void* weapon;                 // 0x10
    float field14;                // 0x14
    s32 field18;                  // 0x18
    float field1c;                // 0x1c
    float field20;                // 0x20
    s32 field24;                  // 0x24
    float timer;                  // 0x28
    s32 field2c;                  // 0x2c
    s32 mode;                     // 0x30
    float field34;                // 0x34
    u32 flags;                    // 0x38
};

struct HostAnim {
    u8  unk00[0x0c];              // 0x00
    u32 field0c;                  // 0x0c (model/type id hash)
    u8  unk10[0x14 - 0x10];       // 0x10
    s32 bank;                     // 0x14
    s32 anim;                     // 0x18
    u8  unk1c[0x30 - 0x1c];       // 0x1c
    s32 unk30;                    // 0x30
    u8  unk34[0x120 - 0x34];      // 0x34
    s32 field120;                 // 0x120
    u8  unk124[0x174 - 0x124];    // 0x124
    s32 field174;                 // 0x174
};

struct Host {
    u8  unk000[0x24];             // 0x000
    s32 i24;                      // 0x024
    s32 i28;                      // 0x028
    s32 i2c;                      // 0x02c
    u8  unk030[0x38 - 0x30];      // 0x030
    u32 field38;                  // 0x038 (class/tag byte in the top 8 bits)
    u8  unk3c[0x84 - 0x3c];       // 0x03c
    float x84;                    // 0x084
    float y88;                    // 0x088
    float z8c;                    // 0x08c
    u8  unk090[0xc4 - 0x90];      // 0x090
    HostAnim* anim;               // 0x0c4
    u8  unk0c8[0xcc - 0x0c8];     // 0x0c8
    s32 iCC;                      // 0x0cc
    u8  unk0d0[0x0fc - 0x0d0];    // 0x0d0
    void (*fieldFC)(void*);       // 0x0fc
    void (*field100)(void*);      // 0x100
    int  (*field104)(void*);      // 0x104
    u32 field108;                 // 0x108
    u32 field10c;                 // 0x10c
    void (*field110)(void*);      // 0x110
    u8 pad114[0x118 - 0x114];
    void (*field118)(void*);      // 0x118
    u8 pad11C[0x120 - 0x11C];
    void (*field120)(void*);      // 0x120
    void (*field124)(void*);      // 0x124
    u8  unk128[0x130 - 0x128];    // 0x128
    HostBehaviour* behaviour;     // 0x130
    u8  unk134[0x138 - 0x134];    // 0x134
    float field138;               // 0x138
    u8  unk13c[0x21c - 0x13c];    // 0x13c
    s32 i21c;                     // 0x21c
    s32 i220;                     // 0x220
    u8  unk224[0x22c - 0x224];    // 0x224
    s32 i22c;                     // 0x22c
    u8  unk230[0x240 - 0x230];    // 0x230
    float f240;                   // 0x240
    float f244;                   // 0x244
    float f248;                   // 0x248
    u8  unk24c[0x268 - 0x24c];    // 0x24c
    s32 i268;                     // 0x268
    s32 i26c;                     // 0x26c
    u8  field270[0x288 - 0x270];  // 0x270 (condition state)
    s32 i288;                     // 0x288
    float f28c;                   // 0x28c
    u8  unk290[0x2c4 - 0x290];    // 0x290
    s32 i2c4;                     // 0x2c4
    u8  unk2c8[0x5c8 - 0x2c8];    // 0x2c8
    float f5c8;                   // 0x5c8
    u8  unk5cc[0x604 - 0x5cc];    // 0x5cc
    void (*field604)(void*);      // 0x604
    u8  unk608[0x894 - 0x608];    // 0x608
    u8  field894[0x900 - 0x894];  // 0x894
    float f900;                   // 0x900
    float field904;               // 0x904
    u8  unk908[0x91c - 0x908];    // 0x908
    float field91c;               // 0x91c
};

extern HostStateEntry lbl_803C2B38[];

// 0x80290C58: run the current behaviour state's predicate and, when it
// succeeds, the shared follow-up action of state 3.
void HostUpdateBehaviourState(void* owner)
{
    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    if (behaviour->state != 3 && lbl_803C2B38[behaviour->state].check(owner) != 0) {
        lbl_803C2B38[3].action(owner);
    }
}

// Payload of the messages this handler receives.
struct HostMsg {
    Host* host;                   // 0x00
    s32 hash;                     // 0x04
    s32 unk08;                    // 0x08
    s32 value;                    // 0x0c
    float f10;                    // 0x10
    float f14;                    // 0x14
    s32 i18;                      // 0x18
    s32 i1c;                      // 0x1c
};

// Payload forwarded to the weapon in the "firing" message.
struct HostFireMsg {
    Host* host;                   // 0x00
    s32 unk04;                    // 0x04
    s32 unk08;                    // 0x08
    s32 unk0c;                    // 0x0c
};

// Two-word notification payload.
struct HostNotify {
    void* object;                 // 0x00
    s32 value;                    // 0x04
};

struct Owner;

// String objects are 0x14 bytes: Buffer's vptr plus four words of storage.
class Buffer {
public:
    virtual ~Buffer();
    u8* data;                     // 0x04
    u32 length;                   // 0x08
    u32 capacity;                 // 0x0c
    u32 flags;                    // 0x10
};

class String : public Buffer {
public:
    String();
    String(const char* text);
};

String operator+(const String& left, const String& right);

// Shared functions that are still fn_/lbl_ placeholders in symbols.txt.
extern "C" int  fn_80019F88(void* data);
extern "C" void fn_802997B0(void* actor, int event);
extern "C" void fn_800A1860(int bank, int anim);
extern "C" void fn_8028C188(void* dest, const char* text, u32 length);
extern "C" void fn_8028C258(void* dest, const char* text);
extern "C" int  SendMsg(int to, int msg, void* data);

int OwnerId(Owner* owner);
void AnimBankUnref(unsigned short bank, unsigned short anim);

extern s32 lbl_804D3804;
extern s32 lbl_804D3808;

extern float lbl_804DA5F0;
extern float lbl_804DA600;
extern float lbl_804DA604;
extern float lbl_804DA608;
extern float lbl_804DA638;

int HasCondition(void* state, int condition);
extern char lbl_8034B5D8[];

// 0x80290CCC: message handler of the host actor.
int HostHandleMessage(int msg, void* payload)
{
    HostMsg* data = (HostMsg*)payload;
    Host* host = data->host;
    HostBehaviour* behaviour = host->behaviour;

    if (msg == 0x41d) {
        if (fn_80019F88(payload) != 0) {
            return 1;
        }
        if (data->hash == -0xa5b8d44) {
            if (data->value == 0) {
                behaviour->flags &= ~0x80;
            } else {
                behaviour->flags |= 0x40;
                behaviour->flags |= 0x80;
                if ((behaviour->flags & 0x40) != 0 && (behaviour->flags & 1) != 0) {
                    behaviour->flags &= ~1;
                    behaviour->timer = 0.0f;
                    fn_802997B0(host, lbl_804D3808);
                }
            }
            return 1;
        }
        if (data->hash == -0x274ecbf0) {
            if (data->value == 0) {
                behaviour->flags &= ~0x40;
                if ((behaviour->flags & 1) == 0 && (behaviour->flags & 0x80) == 0) {
                    behaviour->flags |= 1;
                    behaviour->timer = 0.0f;
                    fn_802997B0(host, lbl_804D3804);
                }
            } else {
                behaviour->flags |= 0x40;
            }
            return 1;
        }
        if (data->hash == 0x34bccf8d) {
            if (data->value == 0) {
                if ((behaviour->flags & 1) == 0 && (behaviour->flags & 0x80) == 0) {
                    behaviour->flags |= 1;
                    behaviour->timer = 0.0f;
                    fn_802997B0(host, lbl_804D3804);
                }
            } else if ((behaviour->flags & 0x40) != 0 && (behaviour->flags & 1) != 0) {
                behaviour->flags &= ~1;
                behaviour->timer = 0.0f;
                fn_802997B0(host, lbl_804D3808);
            }
            return 1;
        }
        return 0;
    }

    if (msg < 0x41d) {
        if (msg == 0x400) {
            data->hash = OwnerId((Owner*)host);
            data->value = 0.0f < host->field904;
            bool firing = ((behaviour->flags & 1) != 0) || ((behaviour->flags & 4) != 0);
            data->f10 = (float)firing;
            data->f14 = lbl_804DA5F0;
            data->i18 = 2;
            data->i1c = 0;
            return 1;
        }
        if (msg < 0x400) {
            if (msg == 6) {
                HostFireMsg fire;
                fire.host = host;
                fire.unk04 = data->hash;
                fire.unk08 = data->unk08;
                fire.unk0c = data->value;
                if (behaviour->weapon != 0) {
                    SendMsg(*(int*)((u8*)behaviour->weapon + 0x38), 6, &fire);
                }
                if (fire.unk04 == 0) {
                    AnimBankUnref(host->anim->bank, host->anim->anim);
                } else {
                    fn_800A1860(host->anim->bank, host->anim->anim);
                }
                return 0;
            }
        } else if (msg < 0x402) {
            data->f10 = host->field904;
            data->f14 = (float)host->anim->unk30;
            return 1;
        }
    } else {
        if (msg == 0xc00) {
            if ((behaviour->flags & 4) != 0) {
                HostNotify notify;
                notify.object = behaviour->weapon;
                notify.value = data->hash;
                SendMsg(behaviour->target, 0xc00, &notify);
            }
            return 0;
        }
        if (msg < 0x41f) {
            if (data->hash != -0x527a8906) {
                return 0;
            }
            // The retail code keeps this String alive across the append below.
            String name;
            {
                String stateName((const char*)lbl_803C2B38[behaviour->state].typeName);
                String prefix(&lbl_8034B5D8[0x48]);
                String text = stateName + prefix;
                fn_8028C188((u8*)data + 0xc, (const char*)text.data, text.length);
            }
            if (behaviour->mode == 2) {
                fn_8028C258((u8*)data + 0xc, &lbl_8034B5D8[0x4a]);
            } else if (behaviour->mode > 0 && behaviour->mode < 2) {
                fn_8028C258((u8*)data + 0xc, &lbl_8034B5D8[0x5a]);
            }
            return 0;
        }
    }
    return 0;
}

// Shared helpers that are still fn_/lbl_ placeholders in symbols.txt.
extern "C" int  fn_80162658(void* object, void* anchor);
int HostPickBehaviourState(void* owner);
void ConditionSetBit(void* state, int bit, int value);

// 0x802911AC: per-frame update of the host's behaviour state.
int HostUpdateBehaviour(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;
    void* weapon = behaviour->weapon;

    behaviour->field20 = behaviour->field1c - host->field904;
    behaviour->field1c = host->field904;

    if (HasCondition(host->field270, 10) == 0 &&
        HasCondition(host->field270, 9) == 0 &&
        HasCondition(host->field270, 0xb) == 0) {
        fn_80162658(host, host->field894);
        if ((behaviour->flags & 4) != 0) {
            fn_80162658(weapon, (u8*)weapon + 0x894);
        }
        if (0.0f != host->field904) {
            host->f5c8 = lbl_804DA600 / (lbl_804DA604 * host->field904);
            if (lbl_804DA608 < host->f5c8) {
                host->f5c8 = lbl_804DA608;
            }
        }
        behaviour->timer = behaviour->timer + host->field138;
        if (lbl_803C2B38[behaviour->state].update(host) != 0) {
            int index = HostPickBehaviourState(host);
            if (lbl_803C2B38[behaviour->state].check(host) != 0) {
                lbl_803C2B38[index].action(host);
                ConditionSetBit(host->field270, 0x40, 0);
                ConditionSetBit(host->field270, 0x41, 0);
                ConditionSetBit(host->field270, 0x43, 0);
                ConditionSetBit(host->field270, 0x42, 0);
            }
        }
        return 1;
    }

    host->i288 = 0;
    host->i26c = 0;
    host->f28c = 0.0f;
    host->f240 = 0.0f;
    host->f244 = 0.0f;
    host->f248 = 0.0f;
    return 1;
}

extern "C" void fn_801626A0(void* object, void* anchor);
void VFree(void* memory);

// 0x802913A4: tear down the host's behaviour state.
void HostDestroyBehaviour(void* owner)
{
    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;
    void* weapon = behaviour->weapon;

    host->field104 = 0;
    if (weapon != 0) {
        fn_801626A0(weapon, (u8*)weapon + 0x894);
    }
    fn_801626A0(host, host->field894);
    VFree(behaviour);
    host->behaviour = 0;
}

// Shared helpers that are still fn_/lbl_ placeholders in symbols.txt.
extern "C" void* VAlloc(int size);
extern "C" void  fn_80290B80(void* owner);
extern "C" void  fn_800CF064(void* owner);
extern "C" int   fn_800A2DE8(void* params, int a, int b, int c);
extern "C" void* fn_800A3990(void);
extern "C" void  fn_8016D6A8(void* transform);
extern "C" void  fn_801627A4(void* actor, void* anchor, int anim);
extern "C" int   fn_8029B070(void* owner, u32 message);
extern "C" void  fn_800BBCA8(void* owner);
extern "C" void  fn_800B6D04(void* owner, void (*callback)(void));
extern "C" void  fn_800B5028(void);
float HostAccumLinkDirection(void* owner, float* out);
extern "C" void  fn_80298550(void* owner);
extern "C" void  fn_8029509C(void* owner);
extern "C" void  fn_80295094(void* owner);
extern "C" int   rand(void);

int FxFind(const char* name);
void ConditionUpdateBit(void* state, int bit, int value);

extern s32    lbl_804D3800;
extern float  lbl_804DA60C;
extern double lbl_804DA5F8;
extern float  lbl_804DA610;
extern float  lbl_804DA614;
extern float  lbl_804DA618;

// Scratch transform handed to the effect helper.
struct FxTransform {
    float x;                      // 0x00
    float y;                      // 0x04
    float z;                      // 0x08
    float w;                      // 0x0c
    float scale;                  // 0x10
};

// 0x80291414: create the host's behaviour state and, for the two carrier
// variants, its demon weapon actor and attack effect.
void HostInitBehaviour(void* owner)
{
    Host* host = (Host*)owner;

    host->field104 = HostUpdateBehaviour;
    host->fieldFC = fn_80290B80;
    host->field604 = fn_800CF064;

    HostBehaviour* behaviour = (HostBehaviour*)VAlloc(0x3c);
    if (behaviour == 0) {
        host->behaviour = 0;
        return;
    }

    host->behaviour = behaviour;
    behaviour->field08 = 0;
    behaviour->state = 0;
    behaviour->field18 = 0;
    // Retail stores zero to 0x08 a second time here.
    behaviour->field08 = 0;
    behaviour->field24 = -1;
    behaviour->timer = 0.0f;
    behaviour->field20 = 0.0f;
    behaviour->field1c = host->field904;
    behaviour->field2c = 0.0f;
    behaviour->mode = 0;
    behaviour->field14 = 0.0f;
    behaviour->field04 = -1;
    behaviour->target = -1;
    behaviour->weapon = 0;
    behaviour->flags = 0;
    behaviour->field34 = lbl_804DA60C;
    behaviour->flags |= 0x40;

    ConditionUpdateBit(host->field270, 6, 1);
    fn_800B6D04(host, (void (*)(void))HostAccumLinkDirection);

    if (lbl_804D3800 == -1) {
        lbl_804D3800 = FxFind(&lbl_8034B5D8[0x6d]);
    }
    if (lbl_804D3804 == -1) {
        lbl_804D3804 = FxFind(&lbl_8034B5D8[0x81]);
    }
    if (lbl_804D3808 == -1) {
        lbl_804D3808 = FxFind(&lbl_8034B5D8[0x95]);
    }

    s32 params[6];
    params[2] = 0;
    params[1] = 0;
    params[3] = 0;
    params[0] = host->anim->field120;

    if (params[0] == -0x5ae969f2 || params[0] == 0x11e66fec) {
        behaviour->flags |= 4;

        // The converted host coordinates are cleared again before the
        // scratch transform is initialised (as in retail).
        float px = (float)host->i24;
        float py = (float)host->i28;
        float pz = (float)host->i2c;
        FxTransform transform;
        px = 0.0f;
        py = 0.0f;
        pz = 0.0f;
        fn_8016D6A8(&transform);

        behaviour->target = fn_800A2DE8(params, 0, 0, 0);
        if (behaviour->target == -1) {
            behaviour->flags |= 4;
            behaviour->weapon = 0;
            behaviour->flags |= 1;
        } else {
            Host* weapon = (Host*)fn_800A3990();
            behaviour->weapon = weapon;
            if (weapon != 0) {
                weapon->field110 = fn_80298550;
                weapon->field124 = fn_8029509C;
                weapon->field104 = 0;
                weapon->field120 = fn_8029509C;
                weapon->fieldFC = 0;
                weapon->field108 = 0;
                weapon->field10c = 0;
                weapon->field100 = 0;
                weapon->field118 = fn_80295094;
            }
            fn_801627A4(weapon, (u8*)weapon + 0x894, weapon->anim->field174);
            ConditionSetBit((u8*)weapon + 0x270, 3, 1);
            ConditionSetBit((u8*)weapon + 0x270, 1, 1);
        }
    } else {
        behaviour->weapon = 0;
        behaviour->flags |= 1;
    }
}

// 0x80291728: randomised reappearance update of the carrier's behaviour.
int HostAppearUpdate(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    float random = (float)rand();
    behaviour->field34 = lbl_804DA618 * (random / lbl_804DA610) + lbl_804DA614;

    fn_8029B070(host, 0x766a16b4);
    fn_800BBCA8(host);
    ConditionUpdateBit(host->field270, 4, 0);
    if (behaviour->state == 10) {
        fn_800B6D04(host, fn_800B5028);
    }
    return 1;
}

// Helpers used by the carrier's lunge state (still fn_/lbl_ placeholders).
extern "C" int  fn_800BAF40(void* owner);
extern "C" int  fn_800BB218(float speed, void* owner, int filter, float* origin,
                            float* direction, void* context, float* outScale,
                            int* outA, int* outB);
extern "C" void fn_800BC5A8(void* owner, float* position);
extern "C" void fn_800E1098(float length, float* direction, float* out);
extern "C" void FollowGetTargetPos(void* state, float* out);
extern "C" void FollowSetTargetObj(void* state, void* target);
extern "C" int  fn_8016325C(void* object);

extern float lbl_804DA61C;
extern float lbl_804DA620;

// 0x802917E8: lunge update of the carrier's behaviour. Returns whether the
// state may keep running.
int HostLungeUpdate(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    if (host->i220 != 0) {
        FollowSetTargetObj(host->field270, (void*)host->i22c);
        return 1;
    }

    if (HasCondition(host->field270, 0x2e) != 0) {
        fn_800BBCA8(host);
        host->i288 = 0;
        return 0;
    }

    if (HasCondition(host->field270, 0x2f) != 0) {
        if (fn_800BAF40(host) == -1) {
            if ((behaviour->flags & 0x20) == 0) {
                float target[3];
                float direction[3];
                float scale;
                int outA;
                int outB;

                FollowGetTargetPos(host->field270, target);
                direction[0] = target[0] - host->x84;
                direction[1] = target[1] - host->y88;
                direction[2] = target[2] - host->z8c;
                fn_800E1098(lbl_804DA61C, direction, direction);
                target[0] = target[0] + direction[0];
                target[1] = target[1] + direction[1];
                target[2] = target[2] + direction[2];
                ConditionUpdateBit(host->field270, 4, 0);
                if (fn_800BB218(host->f900, host, host->i2c4, &host->x84,
                                target, host, &scale, &outA, &outB) != 0) {
                    scale = lbl_804DA620 * scale;
                    target[0] = (target[0] - host->x84) * scale + host->x84;
                    target[1] = (target[1] - host->y88) * scale + host->y88;
                    target[2] = (target[2] - host->z8c) * scale + host->z8c;
                }
                fn_800BC5A8(host, target);
                host->i288 = 3;
                behaviour->flags |= 0x20;
            } else {
                host->i288 = 0;
            }
        }
        return 0;
    }

    if (HasCondition(host->field270, 5) != 0) {
        fn_800BBCA8(host);
        host->i288 = 0;
        return 0;
    }

    return 1;
}

// 0x80291A38: check callback of the carrier's lunge state.
bool HostLungeCheck(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;

    if (host->i220 != 0) {
        FollowSetTargetObj(host->field270, (void*)host->i22c);
        return 1;
    }

    return fn_8016325C(host->field894) == 0;
}

// Helpers used by the carrier's state actions (still fn_/lbl_ placeholders).
extern "C" void fn_800B6CAC(void* owner, void (*callback)(void));
extern "C" int  fn_8029B3B0(void* owner, u32 message);
extern "C" int  fn_80162610(void* object, void* anchor, int anim);

// 0x80291A94: enter action of the carrier's lunge state (state 10).
int HostLungeAction(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    behaviour->state = 10;
    fn_800B6CAC(host, fn_800B5028);

    HostBehaviour* current = host->behaviour;
    u32 flags = current->flags;
    if ((flags & 1) == 0 && (flags & 0x80) == 0) {
        current->flags = flags | 1;
        current->timer = 0.0f;
        fn_802997B0(host, lbl_804D3804);
    }

    fn_8029B3B0(host, 0x766a16b4);
    ConditionUpdateBit(host->field270, 4, 1);
    host->i288 = 0;
    fn_800BBCA8(host);
    ConditionSetBit(host->field270, 0x2e, 1);
    behaviour->mode = 1;
    behaviour->flags &= ~0x20;
    fn_802997B0(host, lbl_804D3800);
    return 1;
}

// 0x80291B84: enter action of the carrier's approach state (state 9).
int HostApproachAction(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    behaviour->state = 9;

    HostBehaviour* current = host->behaviour;
    u32 flags = current->flags;
    if ((flags & 1) == 0 && (flags & 0x80) == 0) {
        current->flags = flags | 1;
        current->timer = 0.0f;
        fn_802997B0(host, lbl_804D3804);
    }

    ConditionUpdateBit(host->field270, 4, 1);
    host->i288 = 0;
    fn_800BBCA8(host);
    fn_80162610(host, host->field894, 2);
    behaviour->mode = 1;
    return 1;
}

// 0x80291C3C: enter action of the carrier's wait state (state 8).
int HostCarryWaitAction(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    behaviour->state = 8;

    HostBehaviour* current = host->behaviour;
    u32 flags = current->flags;
    if ((flags & 1) == 0 && (flags & 0x80) == 0) {
        current->flags = flags | 1;
        current->timer = 0.0f;
        fn_802997B0(host, lbl_804D3804);
    }

    ConditionUpdateBit(host->field270, 4, 1);
    host->i288 = 0;
    fn_800BBCA8(host);
    fn_80162610(host, host->field894, 1);
    behaviour->mode = 1;
    return 1;
}

// Helpers used by the carrier's target tracking update.
extern "C" int  fn_800B26B8(void);
extern "C" int  fn_800B22F8(void* owner, int type);
extern "C" void fn_800B261C(void* owner, int type, void** out);
extern "C" void fn_800B1F20(void* owner, int type);
void* FollowGetTargetObj(void* state);

// 0x80291CF4: keep the carrier's follow target in sync with the target the
// actor's target query helpers report.
int HostTrackTargetUpdate(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;
    void* weapon = behaviour->weapon;

    if (host->i220 != 0) {
        FollowSetTargetObj(host->field270, (void*)host->i22c);
        return 1;
    }

    void* target = 0;
    int type = fn_800B26B8();
    if (type != 4) {
        int previous = fn_800B22F8(host, type);
        fn_800B261C(host, type, &target);
        if (FollowGetTargetObj(host->field270) != target) {
            FollowSetTargetObj(host->field270, target);
            if ((behaviour->flags & 4) != 0) {
                FollowSetTargetObj((u8*)weapon + 0x270, target);
            }
        }
        fn_800B1F20(host, type);
        if (previous != 4) {
            host->i268 = previous;
            return 1;
        }
    }

    if (target == 0 && behaviour->field04 == -1) {
        if (fn_800BAF40(host) == -1) {
            return 1;
        }
        fn_800BBCA8(host);
        return 0;
    }
    return 1;
}

// 0x80291E20: leave the carrier's state, clearing the movement condition.
bool HostCarryCancelState(void* owner)
{
    if (owner == 0) {
        return false;
    }

    Host* host = (Host*)owner;
    fn_800BBCA8(host);
    ConditionUpdateBit(host->field270, 4, 0);
    return true;
}

// 0x80291E6C: leave the carrier's state and return to behaviour state 0.
bool HostCarryResetState(void* owner)
{
    if (owner == 0) {
        return false;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;
    ConditionUpdateBit(host->field270, 0, 0);
    behaviour->state = 0;
    host->i288 = 0;
    return true;
}

// Shared helpers that are still fn_/lbl_ placeholders in symbols.txt.
extern "C" void fn_800CCFE8(void* owner, void* data, int value);

// 0x80291ECC: countdown update of a timed behaviour state. When the timer
// runs out the state randomly switches to one of two follow-up states.
int HostTimerCountdownUpdate(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    if (fn_8016325C(host->field894) != 0) {
        fn_80162610(host, host->field894, 0x1b83833f);
        behaviour->mode = 1;
    }

    fn_800CCFE8(host, &behaviour->field08, 500);
    behaviour->field14 = behaviour->field14 - host->field138 / host->field91c;
    if (behaviour->field14 <= 0.0f) {
        if (rand() % 1000 < 500) {
            if (lbl_803C2B38[behaviour->state].check(host) != 0) {
                lbl_803C2B38[5].action(host);
            }
        } else {
            if (lbl_803C2B38[behaviour->state].check(host) != 0) {
                lbl_803C2B38[7].action(host);
            }
        }
    }
    return 0;
}

// Integer and float three-component vectors used by the drop trace.
struct Vec3i {
    s32 x;                        // 0x00
    s32 y;                        // 0x04
    s32 z;                        // 0x08
};

struct Vec3f {
    float x;                      // 0x00
    float y;                      // 0x04
    float z;                      // 0x08
};

// Vertical segment handed to the ground trace helper.
struct HostTraceBox {
    s32 topX;                     // 0x00
    s32 topY;                     // 0x04
    s32 topZ;                     // 0x08
    s32 bottomX;                  // 0x0c
    s32 bottomY;                  // 0x10
    s32 bottomZ;                  // 0x14
};

// Shared helpers that are still fn_/lbl_ placeholders in symbols.txt.
extern "C" void  fn_8009F44C(void* out, const void* a, const void* b);
extern "C" void  fn_8009F63C(void* out, const void* in);
extern "C" void  fn_8009EC5C(void* owner, void* value);
extern "C" float fn_80018A40(int id);
extern "C" int   fn_800BF2D0(const void* position);
extern "C" int   fn_800A8A74(void* owner, void* position);
extern "C" void  fn_8016D440(void* dest, const void* src);
extern "C" void  fn_8016D68C(void* dest, const void* src);
extern "C" void  fn_8001F928(void* owner, void* box, u32 flags,
                             void (*callback)(void*), void* out);
extern "C" void  fn_80294548(void*);

// 0x80292044: drop the carried object onto the ground. The actor is moved to
// the traced floor height and the fall is reported to the effect helper.
int HostCarrierDropUpdate(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    void* target = FollowGetTargetObj(host->field270);
    ConditionUpdateBit(host->field270, 4, 1);
    ConditionUpdateBit(host->field270, 0, 0);
    behaviour->state = 7;
    host->i288 = 2;
    behaviour->field14 = lbl_804DA618;

    Vec3i position;
    fn_8009F44C(&position, target, (u8*)target + 0x24);

    int value = rand();
    int drop = value % 2000 + 0x5dc + (int)host->f900;
    if (rand() % 1000 < 100) {
        drop = -drop;
    }
    position.y = position.y + drop;

    Vec3i scratch;
    fn_8009F63C(&scratch, target);
    fn_8009F44C(&position, &scratch, &position);

    HostTraceBox box;
    box.topX = position.x;
    box.topY = position.y;
    box.topZ = position.z + 300;
    box.bottomX = position.x;
    box.bottomY = position.y;
    box.bottomZ = position.z - 0x25e4;

    s32 hit = -0x7fffffff - 1;
    fn_8001F928(host, &box, 0x20000000, fn_80294548, &hit);

    float height = fn_80018A40(host->iCC);
    position.z = hit + (int)height;

    Vec3f point;
    point.x = (float)position.x;
    point.y = (float)position.y;
    point.z = (float)position.z;

    if (fn_800BF2D0(&point) != -1 && fn_800A8A74(host, &point) != 0) {
        Vec3i delta;
        delta.x = *(s32*)((u8*)target + 0x24) - position.x;
        delta.y = *(s32*)((u8*)target + 0x28) - position.y;
        delta.z = *(s32*)((u8*)target + 0x2c) - position.z;

        Vec3f direction;
        direction.x = (float)delta.x;
        direction.y = (float)delta.y;
        direction.z = (float)delta.z;

        fn_8016D440((u8*)host + 0x24c, &direction);
        fn_8016D68C((u8*)host + 0x27c, (u8*)host + 0x24c);
        fn_8009EC5C(host, &delta);

        host->i24 = position.x;
        host->i28 = position.y;
        host->i2c = position.z;
        host->x84 = (float)host->i24;
        host->y88 = (float)host->i28;
        host->z8c = (float)host->i2c;
    }
    return 1;
}

float HostFollowTargetDirection(void* owner, float* out);

// 0x80292384: leave the flee state. The movement callback is unregistered,
// the carried-object bit is cleared and the behaviour timer is reset.
int HostFleeExitState(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    fn_800B6CAC(host, (void (*)(void))HostFollowTargetDirection);
    fn_800BBCA8(host);
    ConditionUpdateBit(host->field270, 4, 0);

    HostBehaviour* current = host->behaviour;
    u32 flags = current->flags;
    if ((flags & 0x40) != 0 && (flags & 1) != 0) {
        current->flags = flags & 0xfffffffe;
        current->timer = 0.0f;
        fn_802997B0(host, lbl_804D3808);
    }
    behaviour->timer = 0.0f;
    return 1;
}

// 0x80292428: enter the flee state. The condition bits are reset, state 6 is
// selected, the behaviour timer gets a random value and the movement callback
// is registered.
int HostFleeEnterState(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    ConditionUpdateBit(host->field270, 4, 1);
    ConditionUpdateBit(host->field270, 0, 0);
    behaviour->state = 6;
    host->i288 = 1;

    float random = (float)rand();
    behaviour->field14 = lbl_804DA614 * (random / lbl_804DA610) + lbl_804DA614;
    behaviour->timer = 0.0f;

    fn_800BBCA8(host);

    HostBehaviour* current = host->behaviour;
    u32 flags = current->flags;
    if ((flags & 0x40) != 0 && (flags & 1) != 0) {
        current->flags = flags & 0xfffffffe;
        current->timer = 0.0f;
        fn_802997B0(host, lbl_804D3808);
    }
    behaviour->field2c = 0.0f;

    fn_800B6D04(host, (void (*)(void))HostFollowTargetDirection);
    return 1;
}

// Helpers used by the carrier's follow-line state (still fn_/lbl_ placeholders).
extern "C" int  fn_800B1EF4(void* owner, int type);
extern "C" void fn_800B2418(float value, void* owner, int type);
extern "C" int  fn_800B2454(void* owner, int type);
extern "C" void fn_800B1AB8(float value, void* owner, int type, int flag);

void RangeResetPending(void* state);
void FollowLineCheck(void* state, void* target);
int  FollowLineCheckActorDist(void* state, void* target, float distance);
float OwnerValue(Owner* owner, void* target);

extern float lbl_804DA624;
extern float lbl_804DA628;

// 0x8029252C: follow-line check update of the carrier's behaviour. Keeps the
// actor's follow target in sync and runs the line-of-sight check while the
// behaviour's 0x34 timer has not run out.
int HostFollowLineUpdate(void* owner)
{
    Host* host = (Host*)owner;

    if (host == 0) {
        return 0;
    }

    HostBehaviour* behaviour = host->behaviour;
    int type = fn_800B26B8();
    void* target;

    if (type == 4) {
        RangeResetPending(host->field270);
        return 1;
    }

    if (fn_800B1EF4(host, type) == 0) {
        fn_800B22F8(host, type);
        fn_800B261C(host, type, &target);
        if (target == 0 || fn_800B22F8(host, type) > 1) {
            RangeResetPending(host->field270);
            return 1;
        }
        fn_800B2418(lbl_804DA624, host, type);
        fn_800B1F20(host, type);
        fn_800B1AB8(lbl_804DA628, host, type, 1);
    }

    if (fn_800B2454(host, type) == 0) {
        fn_800B2418(lbl_804DA624, host, type);
    }

    target = FollowGetTargetObj(host->field270);
    if (target == 0) {
        return 1;
    }

    float value = OwnerValue((Owner*)host, target);
    behaviour->field34 = behaviour->field34 - host->field138 / host->field91c;
    if (behaviour->field34 < 0.0f) {
        if (FollowLineCheckActorDist(host, target, lbl_804DA5F0) != 0) {
            return 1;
        }
    }
    if (lbl_804DA61C < value) {
        FollowLineCheck(host, target);
        behaviour->field14 = behaviour->field14 - host->field138 / host->field91c;
        return behaviour->field14 <= 0.0f;
    }
    host->f240 = 0.0f;
    host->f244 = 0.0f;
    host->f248 = 0.0f;
    return 1;
}

// 0x80292738: leave the carrier's follow-line state. The movement callback is
// unregistered and the movement condition bit is cleared.
bool HostFollowLineLeave(void* owner)
{
    Host* host = (Host*)owner;

    if (host == 0) {
        return false;
    }

    fn_800BBCA8(host);
    ConditionUpdateBit(host->field270, 4, 0);
    return true;
}

// Shared helpers used by the follow-line states (still fn_/lbl_ placeholders).
extern "C" void fn_800BC56C(void* owner, void* target);
extern "C" int  fn_80162940(void* object, int index);
extern "C" int  fn_8000DDD0(void);

extern float lbl_804DA62C;
extern float lbl_804DA630;

// 0x80292784: enter action of the follow-line behaviour state (state 5).
bool HostFollowLineAction(void* owner)
{
    if (owner == 0) {
        return false;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    ConditionUpdateBit(host->field270, 4, 1);
    ConditionUpdateBit(host->field270, 0, 1);
    behaviour->state = 5;
    host->i288 = 3;
    behaviour->field14 = lbl_804DA62C;
    fn_800BBCA8(host);

    void* target = FollowGetTargetObj(host->field270);
    fn_800BC56C(host, target);
    return true;
}

// 0x80292820: per-frame check of the follow-line behaviour state. Tracks the
// follow target, counts down the follow-line timer and drives the weapon's
// attack mode.
int HostFollowLineCheckUpdate(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;
    Host* weapon = (Host*)behaviour->weapon;
    void* target;

    int type = fn_800B26B8();
    if (type == 4) {
        RangeResetPending(host->field270);
        return 1;
    }

    if (fn_800B1EF4(host, type) == 0) {
        fn_800B22F8(host, type);
        fn_800B261C(host, type, &target);
        if (target == 0 || fn_800B22F8(host, type) > 1) {
            RangeResetPending(host->field270);
            return 1;
        }
        fn_800B2418(lbl_804DA624, host, type);
        fn_800B1F20(host, type);
        fn_800B1AB8(lbl_804DA628, host, type, 1);
    }

    if (fn_800B2454(host, type) == 0) {
        fn_800B2418(lbl_804DA624, host, type);
    }

    target = FollowGetTargetObj(host->field270);
    if (target == 0) {
        return 1;
    }

    if ((behaviour->flags & 4) == 0) {
        behaviour->field34 = behaviour->field34 - host->field138 / host->field91c;
        if (behaviour->field34 < 0.0f &&
            FollowLineCheckActorDist(host, target, lbl_804DA5F0) != 0) {
            return 1;
        }
    } else {
        if (FollowLineCheckActorDist(host, target, lbl_804DA5F0) != 0) {
            if (lbl_804DA630 <= (float)rand() / lbl_804DA610 ||
                fn_80162940((u8*)weapon + 0x894, 2) == 0 ||
                fn_80162940((u8*)weapon + 0x894, 8) == 0) {
                if (fn_80162940((u8*)weapon + 0x894, 1) != 0 &&
                    fn_80162940((u8*)weapon + 0x894, 4) != 0) {
                    fn_80162610(weapon, (u8*)weapon + 0x894, 1);
                    behaviour->mode = 2;
                }
            } else {
                fn_80162610(weapon, (u8*)weapon + 0x894, 2);
                behaviour->mode = 2;
            }
        }
    }

    if (OwnerValue((Owner*)host, target) < lbl_804DA62C) {
        return 1;
    }

    if ((behaviour->flags & 0x10) == 0) {
        if (fn_800BAF40(host) == -1) {
            fn_800BC56C(host, target);
            if (fn_800BAF40(host) == -1) {
                return 1;
            }
            host->i288 = 1;
            if ((behaviour->flags & 4) == 0) {
                host->i288 = 2;
            }
        }
        behaviour->field14 = behaviour->field14 - host->field138 / host->field91c;
        if (behaviour->field14 <= 0.0f) {
            behaviour->field14 = 500.0f;
            behaviour->flags |= 0x10;
        }
    } else {
        fn_800CCFE8(host, &behaviour->field08, 500);
        behaviour->field14 = behaviour->field14 - host->field138 / host->field91c;
        if (behaviour->field14 <= 0.0f ||
            FollowLineCheckActorDist(host, target, lbl_804DA5F0) != 0) {
            fn_800BC56C(host, target);
            host->i288 = 1;
            if ((behaviour->flags & 4) == 0) {
                host->i288 = 2;
            }
            behaviour->field08 = 0;
            behaviour->flags &= ~0x10;
            behaviour->field14 = 500.0f;
        }
    }

    if ((behaviour->flags & 4) == 0) {
        HostBehaviour* current = host->behaviour;
        u32 flags = current->flags;
        if ((flags & 0x40) != 0 && (flags & 1) != 0) {
            current->flags = flags & ~1;
            current->timer = 0.0f;
            fn_802997B0(host, lbl_804D3808);
        }
    }

    if (0.0f < behaviour->field20 && (behaviour->flags & 4) == 0 &&
        fn_8000DDD0() % 1000 < 300) {
        if (lbl_803C2B38[behaviour->state].check(host) != 0) {
            lbl_803C2B38[5].action(host);
        }
    }
    return 0;
}

extern float lbl_804DA638;

// 0x80292CC0: exit action of the carrier's chase state. The behaviour's
// approach timer is scaled down, the movement condition is cleared and, when
// the state is not armed yet, the follow-up event is fired.
int HostChaseExit(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    fn_800BBCA8(host);
    ConditionUpdateBit(host->field270, 4, 0);
    behaviour->field34 = behaviour->field34 * lbl_804DA638;

    if ((behaviour->flags & 4) == 0) {
        HostBehaviour* current = host->behaviour;
        u32 flags = current->flags;
        if ((flags & 1) == 0 && (flags & 0x80) == 0) {
            current->flags = flags | 1;
            current->timer = 0.0f;
            fn_802997B0(host, lbl_804D3804);
        }
    }
    return 1;
}

// 0x80292D6C: enter action of the carrier's chase state (state 4). The
// movement condition is set, the state is selected and the follow target is
// handed to the movement helper.
int HostChaseEnter(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    behaviour->state = 4;
    behaviour->field14 = 500.0f;
    ConditionUpdateBit(host->field270, 4, 1);

    host->i288 = 1;
    if ((behaviour->flags & 4) == 0) {
        host->i288 = 2;
    }

    void* target = FollowGetTargetObj(host->field270);
    if (target != 0) {
        fn_800BC56C(host, target);
    }
    return 1;
}

// 0x80292E08: non-zero when the host's behaviour sub-mode is 2.
bool HostBehaviourIsSubModeTwo(Host* host)
{
    if (host == 0) {
        return 0;
    }
    return host->behaviour->field18 == 2;
}

// 0x80292E30: cancel the host's movement state: the movement helper is
// released and the movement condition bit is cleared.
bool HostBehaviourCancelMove(void* owner)
{
    if (owner == 0) {
        return false;
    }

    Host* host = (Host*)owner;
    fn_800BBCA8(host);
    ConditionUpdateBit(host->field270, 4, 0);
    return true;
}

extern float lbl_804DA63C;

// 0x80292E7C: activate the behaviour in follow state 3. Resets two condition
// bits, selects animation bank 1 and reports the state-change event once.
int HostBehaviourActivate(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    ConditionUpdateBit(host->field270, 0, 0);
    ConditionUpdateBit(host->field270, 3, 0);
    behaviour->state = 3;
    host->i288 = 0;
    behaviour->field18 = 1;
    if ((behaviour->flags & 1) == 0 && (behaviour->flags & 0x80) == 0) {
        behaviour->flags |= 1;
        behaviour->timer = 0.0f;
        fn_802997B0(owner, lbl_804D3804);
    }
    return 1;
}

int Rand();

// 0x80292F30: per-frame check of the target-engagement behaviour state. Tracks
// the follow target, applies the line-of-sight test, drives the weapon's attack
// animation and re-runs the state table once the random delay expires.
int HostEngageTargetUpdate(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;
    Host* weapon = (Host*)behaviour->weapon;
    void* target;

    int type = fn_800B26B8();
    if (type == 4) {
        RangeResetPending(host->field270);
        return 1;
    }

    if (fn_800B1EF4(host, type) == 0) {
        fn_800B22F8(host, type);
        fn_800B261C(host, type, &target);
        if (target == 0 || fn_800B22F8(host, type) > 1) {
            RangeResetPending(host->field270);
            return 1;
        }
        fn_800B2418(lbl_804DA624, host, type);
        fn_800B1F20(host, type);
        fn_800B1AB8(lbl_804DA628, host, type, 1);
    }

    if (fn_800B2454(host, type) == 0) {
        fn_800B2418(lbl_804DA624, host, type);
    }

    target = FollowGetTargetObj(host->field270);
    if (target == 0) {
        return 1;
    }

    if (behaviour->flags & 4) {
        if (FollowLineCheckActorDist(host, target, lbl_804DA5F0) != 0) {
            if (lbl_804DA630 <= (float)rand() / lbl_804DA610 ||
                fn_80162940((u8*)weapon + 0x894, 2) == 0 ||
                fn_80162940((u8*)weapon + 0x894, 8) == 0) {
                if (fn_80162940((u8*)weapon + 0x894, 1) != 0 &&
                    fn_80162940((u8*)weapon + 0x894, 4) != 0) {
                    fn_80162610(weapon, (u8*)weapon + 0x894, 1);
                    behaviour->mode = 2;
                }
            } else {
                fn_80162610(weapon, (u8*)weapon + 0x894, 2);
                behaviour->mode = 2;
            }
        }
    } else {
        behaviour->field34 = behaviour->field34 - host->field138 / host->field91c;
        if (behaviour->field34 < 0.0f &&
            FollowLineCheckActorDist(host, target, lbl_804DA5F0) != 0) {
            return 1;
        }
    }

    if (OwnerValue((Owner*)host, target) < lbl_804DA63C) {
        return 1;
    }

    behaviour->field14 = behaviour->field14 - host->field138 / host->field91c;
    if (behaviour->field14 <= 0.0f) {
        return 1;
    }

    if (fn_800BAF40(host) == -1) {
        fn_800BC56C(host, target);
        if (fn_800BAF40(host) == -1) {
            return 1;
        }
        host->i288 = 1;
        if ((behaviour->flags & 4) == 0) {
            host->i288 = 2;
        }
    }

    if (behaviour->field20 > 0.0f && (behaviour->flags & 4) == 0) {
        if (Rand() % 1000 < 400) {
            if (lbl_803C2B38[behaviour->state].check(host) != 0) {
                lbl_803C2B38[5].action(host);
            }
            return 0;
        }
    }
    return 0;
}

// 0x802932DC: behaviour-state action. Damps the attack timing counter and,
// when the behaviour is idle, re-arms the attack-ready event.
int HostActionAttackResume(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    behaviour->field34 = behaviour->field34 * lbl_804DA638;
    fn_800BBCA8(host);
    ConditionUpdateBit(host->field270, 4, 0);

    if ((behaviour->flags & 4) == 0) {
        behaviour = host->behaviour;
        u32 flags = behaviour->flags;
        if ((flags & 1) == 0 && (flags & 0x80) == 0) {
            behaviour->flags = flags | 1;
            behaviour->timer = 0.0f;
            fn_802997B0(host, lbl_804D3804);
        }
    }
    return 1;
}

// 0x80293388: behaviour-state action. Switches the host into the follow state,
// drops the attack-ready flag and notifies the weapon of the new target.
int HostActionFollowStart(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;
    void* target;

    behaviour->state = 1;
    behaviour->field14 = 500.0f;
    ConditionUpdateBit(host->field270, 4, 1);
    host->i288 = 1;

    if ((behaviour->flags & 4) == 0) {
        host->i288 = 2;
        behaviour = host->behaviour;
        u32 flags = behaviour->flags;
        if ((flags & 0x40) != 0 && (flags & 1) != 0) {
            behaviour->flags = flags & ~1u;
            behaviour->timer = 0.0f;
            fn_802997B0(host, lbl_804D3808);
        }
    }

    target = FollowGetTargetObj(host->field270);
    if (target != 0) {
        fn_800BC56C(host, target);
    }
    return 1;
}

// 0x80293458: follow-state check.  Drops the follow flag when there is no
// target (or when the host's i220 flag is clear) and re-issues the follow
// animation when the actor is close enough, sometimes with a random delay.
int HostFollowStateCheck(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;
    void* target = FollowGetTargetObj(host->field270);
    void* object = behaviour->weapon;

    if (target == 0) {
        behaviour->flags &= ~2u;
        return 1;
    }

    if (FollowLineCheckActorDist(host, target, lbl_804DA5F0) != 0) {
        int result = 0;
        int mode;

        if ((behaviour->flags & 4) == 0) {
            mode = 1;
            object = host;
        } else {
            mode = 2;
        }

        if (mode == 2) {
            float random = (float)rand();
            if (random / lbl_804DA610 < lbl_804DA630 &&
                (result = fn_80162610(object, (u8*)object + 0x894, 2)) != 0) {
                behaviour->mode = 2;
            }
        }

        if (result == 0) {
            fn_80162610(object, (u8*)object + 0x894, 1);
            behaviour->mode = mode;
        }
    }

    if (host->i220 == 0) {
        behaviour->flags &= ~2u;
        return 1;
    }
    return 0;
}

// 0x80293588: follow-state teardown.  Resets the actor's condition bit and
// reports whether it was actually called with an actor.
int HostFollowStateExit(void* owner)
{
    if (owner == 0) {
        return 0;
    }
    fn_800BBCA8(owner);
    ConditionUpdateBit(((Host*)owner)->field270, 4, 0);
    return 1;
}

// 0x802935D4: enter the follow state.  Marks the behaviour active, sets the
// movement condition and clears the pending follow target.
int HostFollowBeginState(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    host->behaviour->state = 0xb;
    ConditionUpdateBit(host->field270, 4, 1);
    host->i288 = 0;
    return 1;
}

// Helpers used by the host's chase state update.
int  MemFlagIsSet(void* state, int flag);
int  FollowLineCheckPos(void* owner, float* position);
void OwnerSlotGetPosition(int owner, int slot, float* out);
extern "C" void fn_800C748C(void);

// 0x80293630: chase-state update.  Picks up the current target reported by the
// target query helpers, retargets the follow state and, while the host is in
// chase mode, keeps the stored follow target and callback in sync.
int HostChaseStateUpdate(void* owner)
{
    void* target;
    Host* host = (Host*)owner;

    if (host == 0) {
        return 0;
    }

    HostBehaviour* behaviour = host->behaviour;
    int type = fn_800B26B8();

    if ((behaviour->flags & 2) != 0) {
        return 1;
    }

    if (type != 4) {
        int previous = fn_800B22F8(host, type);
        fn_800B261C(host, type, &target);
        if (previous != 4) {
            int ok = 1;
            if (type == 3) {
                float position[5];
                OwnerSlotGetPosition((int)host, type, position);
                if (FollowLineCheckPos(host, position) == 0) {
                    ok = 0;
                }
            }
            if (ok) {
                host->i268 = previous;
                if (MemFlagIsSet(host->field270, 5) == 0) {
                    FollowSetTargetObj(host->field270, target);
                    host->i220 = 0;
                    return 1;
                }
            }
        }
    }

    if (host->i220 == 0 && MemFlagIsSet(host->field270, 5) == 0) {
        return 1;
    }

    if (host->i220 == 2) {
        host->field10c = (u32)fn_800C748C;
        void* current = FollowGetTargetObj(host->field270);
        if (current != (void*)host->i22c) {
            FollowSetTargetObj(host->field270, (void*)host->i22c);
            if (host->i21c != 0) {
                host->i288 = host->i21c;
                fn_800BC56C(host, (void*)host->i22c);
            }
        }
        if (fn_800BAF40(host) == -1 && host->i21c != 0) {
            FollowSetTargetObj(host->field270, 0);
            host->i288 = 0;
        }
    }
    return 0;
}

void FollowOffsetUpdate(void* owner);

// 0x802937E8: follow-offset state entry.  Installs the follow-offset update
// callback, drops the stale follow state and clears the pending condition.
int HostFollowOffsetStateEnter(void* owner)
{
    Host* host;

    if (owner == 0) {
        return 0;
    }

    host = (Host*)owner;
    host->field10c = (u32)FollowOffsetUpdate;
    if (host->i220 == 4) {
        host->i220 = 0;
    }
    if (HasCondition(host->field270, 0xd) != 0) {
        ConditionSetBit(host->field270, 0xd, 0);
    }
    return 1;
}

// 0x80293864: leave the current behaviour state and restart the range check
// with the pending-range reset.
int HostFollowOffsetResetEnter(void* owner)
{
    Host* host;

    if (owner == 0) {
        return 0;
    }

    host = (Host*)owner;
    host->behaviour->state = 0xc;
    RangeResetPending(host->field270);
    return 1;
}

// Shared helpers used by the follow-line carry update (still fn_ placeholders).
extern "C" void  fn_800E0E9C(void* out, const void* in, int value);
extern "C" float fn_800E1020(const void* vec);
extern "C" void  fn_800BAFF8(void* owner, float* vec, int outA, int outB,
                             float* position, float scale, float amount);

// 0x802938A8: follow-line check update of the carrier's behaviour. Tracks the
// follow target, drives the weapon's attack mode and, once the movement helper
// reports a free path, traces the ground below the carried object and moves the
// host to it.
int HostFollowLineCarryUpdate(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;
    Host* weapon = (Host*)behaviour->weapon;
    void* target;

    int type = fn_800B26B8();
    if (type == 4) {
        RangeResetPending(host->field270);
        return 1;
    }

    if (fn_800B1EF4(host, type) == 0) {
        fn_800B22F8(host, type);
        fn_800B261C(host, type, &target);
        if (target == 0 || fn_800B22F8(host, type) > 1) {
            RangeResetPending(host->field270);
            return 1;
        }
        fn_800B2418(lbl_804DA624, host, type);
        fn_800B1F20(host, type);
        fn_800B1AB8(lbl_804DA628, host, type, 1);
    }

    if (fn_800B2454(host, type) == 0) {
        fn_800B2418(lbl_804DA624, host, type);
    }

    target = FollowGetTargetObj(host->field270);
    if (target == 0) {
        return 1;
    }

    if ((behaviour->flags & 4) == 0) {
        behaviour->field34 = behaviour->field34 - host->field138 / host->field91c;
        if (behaviour->field34 < 0.0f &&
            FollowLineCheckActorDist(host, target, lbl_804DA5F0) != 0) {
            return 1;
        }
    } else {
        if (FollowLineCheckActorDist(host, target, lbl_804DA5F0) != 0) {
            if (lbl_804DA630 <= (float)rand() / lbl_804DA610 ||
                fn_80162940((u8*)weapon + 0x894, 2) == 0 ||
                fn_80162940((u8*)weapon + 0x894, 8) == 0) {
                if (fn_80162940((u8*)weapon + 0x894, 1) != 0 &&
                    fn_80162940((u8*)weapon + 0x894, 4) != 0) {
                    fn_80162610(weapon, (u8*)weapon + 0x894, 1);
                    behaviour->mode = 2;
                }
            } else {
                fn_80162610(weapon, (u8*)weapon + 0x894, 2);
                behaviour->mode = 2;
            }
        }
    }

    if (fn_800BAF40(host) == -1) {
        Vec3i delta;
        delta.x = host->i24 - *(s32*)((u8*)target + 0x24);
        delta.y = host->i28 - *(s32*)((u8*)target + 0x28);
        delta.z = host->i2c - *(s32*)((u8*)target + 0x2c);
        fn_800E0E9C(&delta, &delta, 500);

        Vec3f offset;
        offset.x = (float)delta.x;
        offset.y = (float)delta.y;
        offset.z = (float)delta.z;

        Vec3f position;
        position.x = offset.x + host->x84;
        position.y = offset.y + host->y88;
        position.z = offset.z + host->z8c;

        float scale;
        int outA;
        int outB;
        if (fn_800BB218(host->f900, host, host->i2c4, &host->x84,
                        (float*)&position, host, &scale, &outA, &outB) != 0) {
            float norm = fn_800E1020(&offset);
            fn_800BAFF8(host, (float*)&offset, outA, outB, (float*)&position,
                        scale, norm * (lbl_804DA5F0 - scale));
        }

        int x = (int)position.x;
        int y = (int)position.y;
        int z = (int)position.z;

        HostTraceBox box;
        box.topX = x;
        box.topY = y;
        box.topZ = z + 100;
        box.bottomX = x;
        box.bottomY = y;
        box.bottomZ = z - 0x26ac;

        s32 hit = -0x7fffffff - 1;
        fn_8001F928(host, &box, 0x20000000, fn_80294548, &hit);

        position.z = (float)hit;
        fn_800BC5A8(host, (float*)&position);
        if (fn_800BAF40(host) == -1) {
            return 1;
        }
    } else {
        if (fn_800BAF40(host) != -2) {
            host->i288 = 2;
        }
    }

    if ((behaviour->flags & 4) == 0) {
        HostBehaviour* current = host->behaviour;
        u32 flags = current->flags;
        if ((flags & 0x40) != 0 && (flags & 1) != 0) {
            current->flags = flags & ~1u;
            current->timer = 0.0f;
            fn_802997B0(host, lbl_804D3808);
        }
    }

    behaviour->field14 = behaviour->field14 - host->field138 / host->field91c;
    if (behaviour->field14 <= 0.0f) {
        return 1;
    }
    return OwnerValue((Owner*)host, target) < lbl_804DA624;
}

// 0x80293DB0: leave the carrier's drop state. Releases the movement helper,
// clears the movement condition and re-arms the idle event when the behaviour
// is not busy.
int HostCarrierReleaseState(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    fn_800BBCA8(host);
    ConditionUpdateBit(host->field270, 4, 0);

    if ((behaviour->flags & 4) == 0) {
        HostBehaviour* current = host->behaviour;
        u32 flags = current->flags;
        if ((flags & 1) == 0 && (flags & 0x80) == 0) {
            current->flags = flags | 1;
            current->timer = 0.0f;
            fn_802997B0(host, lbl_804D3804);
        }
    }
    return 1;
}

// 0x80293E4C: reaction action. Enters behaviour state 2, picks a random
// recovery delay, then traces the ground under the follow target and moves the
// actor to the hit height.
int HostRecoilAction(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    behaviour->state = 2;
    behaviour->field14 = lbl_804DA61C * ((float)rand() / lbl_804DA610) + lbl_804DA63C;

    fn_800BBCA8(host);
    host->i288 = 1;

    Host* target = (Host*)FollowGetTargetObj(host->field270);
    if (target != 0) {
        Vec3i delta;
        delta.x = host->i24 - target->i24;
        delta.y = host->i28 - target->i28;
        delta.z = host->i2c - target->i2c;
        fn_800E0E9C(&delta, &delta, 500);

        Vec3f offset;
        offset.x = (float)delta.x;
        offset.y = (float)delta.y;
        offset.z = (float)delta.z;

        Vec3f position;
        position.x = offset.x + host->x84;
        position.y = offset.y + host->y88;
        position.z = offset.z + host->z8c;

        float scale;
        int outA;
        int outB;
        if (fn_800BB218(host->f900, host, host->i2c4, &host->x84,
                        (float*)&position, host, &scale, &outA, &outB) != 0) {
            float norm = fn_800E1020(&offset);
            fn_800BAFF8(host, (float*)&offset, outA, outB, (float*)&position,
                        scale, norm * (lbl_804DA5F0 - scale));
        }

        int x = (int)position.x;
        int y = (int)position.y;
        int z = (int)position.z;

        HostTraceBox box;
        box.topX = x;
        box.topY = y;
        box.topZ = z + 100;
        box.bottomX = x;
        box.bottomY = y;
        box.bottomZ = z - 0x26ac;

        s32 hit = -0x7fffffff - 1;
        fn_8001F928(host, &box, 0x20000000, fn_80294548, &hit);

        position.z = (float)hit;
        fn_800BC5A8(host, (float*)&position);
    }

    ConditionUpdateBit(host->field270, 4, 1);
    return 1;
}

extern float lbl_804DA640;
extern float lbl_804DA644;

// 0x802940CC: pick the behaviour state that follows the target value check.
int HostPickBehaviourState(void* owner)
{
    if (owner == 0) {
        return 0;
    }

    Host* host = (Host*)owner;
    HostBehaviour* behaviour = host->behaviour;

    void* target = FollowGetTargetObj(host->field270);
    if (target == 0) {
        return 0;
    }

    float value = OwnerValue((Owner*)host, target);

    if (host->i220 != 0) {
        return 12 - ((behaviour->flags >> 1) & 1);
    }

    if ((behaviour->flags & 4) == 0 && behaviour->state != 8 &&
        behaviour->state != 9 && behaviour->state != 10 &&
        behaviour->field34 < 0.0f) {
        if (lbl_804DA640 < value) {
            return 8;
        }
        if (lbl_804DA618 < value) {
            return 9;
        }
        if (fn_8029B3B0(host, 0x766a16b4) != 0) {
            return 10;
        }
    }

    if (value < lbl_804DA63C) {
        return 2;
    }
    if (lbl_804DA624 < value) {
        return 1;
    }
    if (behaviour->state != 4) {
        return 4;
    }

    if (lbl_804DA644 * ((float)rand() / lbl_804DA610) < value - lbl_804DA63C) {
        return 1;
    }
    return 2;
}

extern "C" void fn_8001ED98(void* position, u32 flags,
                            int (*callback)(void*, void*), void* context,
                            int arg);

// Accumulator filled by HostAddLinkOffset for HostAccumLinkDirection.
struct HostLinkOffset {
    void* owner;                  // 0x00  (the host being queried)
    float x;                      // 0x04
    float y;                      // 0x08
    float z;                      // 0x0c
};

int HostAddLinkOffset(void* object, void* data);

// 0x80294268: sum the direction towards every linked host of the target class
// into an out vector and normalise it; 0 on failure, 1 on success.
float HostAccumLinkDirection(void* owner, float* out)
{
    if (owner == 0 || out == 0) {
        return 0.0f;
    }

    Host* host = (Host*)owner;
    if (host == 0) {
        return 0.0f;
    }

    HostLinkOffset offset;
    offset.owner = host;
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = 0.0f;

    FollowGetTargetObj(host->field270);
    fn_8001ED98(&host->i24, 0x300, HostAddLinkOffset, &offset, 0);

    *out = offset.x;
    out[1] = offset.y;
    out[2] = offset.z;
    fn_800E1098(180.0f, out, out);
    return lbl_804DA5F0;
}

// 0x80294324: link-search callback. Adds the normalised direction from the
// querying host to one other host of the same class into the accumulator.
int HostAddLinkOffset(void* object, void* data)
{
    Host* host = (Host*)object;
    HostLinkOffset* offset = (HostLinkOffset*)data;

    if ((host->field38 >> 24) != 0x13) {
        return 0;
    }
    if (host->anim->field0c != 0x4bd66ad8) {
        return 0;
    }

    Host* other = (Host*)offset->owner;
    if (other == host) {
        return 0;
    }

    Vec3f delta;
    delta.x = (float)(other->i24 - host->i24);
    delta.y = (float)(other->i28 - host->i28);
    delta.z = (float)(other->i2c - host->i2c);
    fn_800E1098(lbl_804DA5F0, (float*)&delta, (float*)&delta);

    offset->x += delta.x;
    offset->y += delta.y;
    offset->z += delta.z;
    return 1;
}

// 0x80294450: write the direction from the follow target to the owner into an
// out vector and normalise it; returns the "no direction" sentinel on failure.
float HostFollowTargetDirection(void* owner, float* out)
{
    if (owner == 0 || out == 0) {
        return 0.0f;
    }

    Host* host = (Host*)owner;
    if (host == 0) {
        return 0.0f;
    }

    Host* target = (Host*)FollowGetTargetObj(host->field270);
    if (target == 0) {
        return 0.0f;
    }

    out[0] = (float)(host->i24 - target->i24);
    out[1] = (float)(host->i28 - target->i28);
    out[2] = (float)(host->i2c - target->i2c);
    fn_800E1098(180.0f, out, out);
    return lbl_804DA5F0;
}

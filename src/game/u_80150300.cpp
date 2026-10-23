// u_80150300.cpp: possession host-type dispatch tables (0x80150300-0x80153C10).
// Per-host-type records (id, four callbacks, separator) and the state machine that
// drives them. Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN", "",
};

struct Actor;

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

// Per-actor state block (VAlloc'd, 0x58 bytes). The first word is the index into
// the host-type table; the three trail handles live at 0x4C.
struct HostState {
    int index;          // 0x00
    int sub;            // 0x04
    int owner;          // 0x08
    int unk0C;          // 0x0C
    int unk10;          // 0x10
    int unk14;          // 0x14
    float f18;          // 0x18
    int unk1C;          // 0x1C
    float f20;          // 0x20
    float f24;          // 0x24
    float f28;          // 0x28
    int unk2C;          // 0x2C
    int unk30;          // 0x30
    int unk34;          // 0x34
    int unk38;          // 0x38
    float f3C;          // 0x3C
    float f40;          // 0x40
    float f44;          // 0x44
    float f48;          // 0x48
    int trail[3];       // 0x4C
};

typedef int (*HostMsgFunc)(int msg, Actor* actor);
typedef int (*HostEnterFunc)(Actor* actor);
typedef int (*HostCheckFunc)(Actor* actor, int next);
typedef int (*HostUpdateFunc)(Actor* actor);

struct Actor {
    char pad00[0x30];    // 0x00
    int msgFn;           // 0x30
    char pad34[4];       // 0x34
    int i38;             // 0x38
    char pad3C[0x88];    // 0x3C
    int iC4;             // 0xC4
    char padC8[4];       // 0xC8
    int iCC;             // 0xCC
    char padD0[4];       // 0xD0
    int iD4;             // 0xD4
    char padD8[0x34];    // 0xD8
    void (*fn10C)();     // 0x10C
    HostUpdateFunc fn110;// 0x110
    char pad114[4];      // 0x114
    void (*fn118)();     // 0x118
    char pad11C[0x18];   // 0x11C
    HostState* state;    // 0x134
    float dt;            // 0x138
    char pad13C[0xD8];   // 0x13C
    int i214;            // 0x214
    char pad218[8];      // 0x218
    int i220;            // 0x220
    char pad224[0x1C];   // 0x224
    float pos[3];        // 0x240
    char pad24C[0x20];   // 0x24C
    int i26C;            // 0x26C
    int i270;            // 0x270
    char pad274[0x10];   // 0x274
    float f284;          // 0x284
    int i288;            // 0x288
    char pad28C[0x364];  // 0x28C
    void* p5F0;          // 0x5F0
    char pad5F4[0x2A0];  // 0x5F4
    char obj894[0x70];   // 0x894
    float f904;          // 0x904
};

// Trail description passed to TrailCreate.
struct TrailDesc {
    int unk0;            // 0x00
    int owner;           // 0x04
    int unk8;            // 0x08
    int segments;        // 0x0C
    float width;         // 0x10
    float unk14;         // 0x14
    char pad18[8];       // 0x18
    float unk20;         // 0x20
    float unk24;         // 0x24
    float unk28;         // 0x28
    float unk2C;         // 0x2C
    int texture;         // 0x30
    unsigned char color[3]; // 0x34
    char pad37[9];       // 0x37
};

// One entry of the host-type table at 0x803A5488 (0x18 bytes).
struct HostType {
    u32 id;                 // 0x00
    HostEnterFunc enter;    // 0x04
    HostCheckFunc check;    // 0x08
    HostUpdateFunc update;  // 0x0C
    HostMsgFunc msg;        // 0x10
    const char* sep;        // 0x14
};

extern HostType lbl_803A5488[];

struct MsgHdr {
    Actor* actor;   // 0x00
    int cmd;        // 0x04
    int unk08;      // 0x08
};

struct Msg41E {
    Actor* actor;   // 0x00
    int cmd;        // 0x04
    int unk08;      // 0x08
    Buffer result;  // 0x0C
};

struct MsgC14 {
    int unk00;      // 0x00
    int unk04;      // 0x04
    int unk08;      // 0x08
    int hash;       // 0x0C
};

// Query message used by the host-type check callback (msg 0x400).
struct HostQuery {
    Actor* actor;   // 0x00
    int value;      // 0x04
    int pad08[4];   // 0x08
    int flags;      // 0x18
};

typedef void (*HostQueryFunc)(int msg, HostQuery* query);

// A pair of actors registered in the global host list.
struct HostEntry {
    Actor* lead;    // 0x00
    Actor* trail;   // 0x04
};

// Mangled C++ callees.
int HasCondition(void* conditions, int which);          // 0x800A7AC4
void* VAlloc(int size);                                 // 0x8009DA68
void VFree(void* p);                                    // 0x8009DA3C
int AssetFind(u32 table, u32 name);                     // 0x8007EB34
int ModelFindBone(int model, int id);                   // 0x80018AD4
int ModelBoneName(int model);                           // 0x80018B44
int OwnerId(Actor* a);                                  // 0x800A3A40
int TrailCreate(TrailDesc* desc);                       // 0x801DE960
void TrailDestroy(int trail);                           // 0x801DE904
void ActorPlayAnim(int actor, u32 anim, int a);         // 0x800C5654
void TalkPlay(int actor, int id);                       // 0x800DDA24

extern "C" {
String fn_800225C8(const String& a, const String& b);   // 0x800225C8
void fn_8028C188(Buffer* b, char* data, int len);       // 0x8028C188
int fn_80116E08(Actor* actor);                          // 0x80116E08
int fn_80116DC4(Actor* actor);                          // 0x80116DC4
void fn_800CB8BC(Actor* actor, int a, int b, int c);    // 0x800CB8BC
int fn_8008EA9C(int a, int b);                          // 0x8008EA9C
void fn_80152990(Actor* actor);                         // 0x80152990
void fn_801537E8(Actor* actor, int arg);                // 0x801537E8
void fn_800A7A54(void* cond, int id, int value);        // 0x800A7A54
int fn_800A82D0(Actor* actor, void* pos);               // 0x800A82D0
void fn_80162610(Actor* actor, void* obj, u32 hash);    // 0x80162610
int fn_8016325C(void* obj);                             // 0x8016325C
int fn_80212730(int id);                                // 0x80212730
void fn_8008DE4C(int a, int b, int c, int d);           // 0x8008DE4C
int fn_80018814(int model, int bone, void* table, int n);// 0x80018814
void SendMsg(int to, int msg, void* data);              // 0x800265D4
int fn_800A1BC8(int a, int b, u32 hash, int c, int d);  // 0x800A1BC8
void fn_80090974(int id, int a, int b, float t, int c,
                 HostMsgFunc cb, Actor* actor);         // 0x80090974
void fn_80120AC0(double t, u32 hash, int n, int id);    // 0x80120AC0
void fn_800FC764(int actor, int flag);                  // 0x800FC764
int fn_802A1D04(void);                                  // 0x802A1D04
void fn_800A7068(void);                                 // 0x800A7068
void fn_800CDD10(void);                                 // 0x800CDD10
void fn_8008F230(int actor, int id, int on, float value);// 0x8008F230
int fn_80153A64(int* arg, int* out);                    // 0x80153A64
int fn_80153B28(int id);                                // 0x80153B28
int fn_80153B9C(int id);                                // 0x80153B9C
int rand(void);                                         // 0x802F2CFC
extern int lbl_8046DDA8[16];
extern float lbl_804D2F10;
extern HostEntry* lbl_803A5968[];
extern int lbl_804D4E20;
}

// Local (this unit) functions.
int PossessHostMsg(int msg, void* data);                   // 0x80150300
int PossessHostUpdate(Actor* a);                           // 0x80150494
void HostTypeTrails(Actor* a);                          // 0x80150614
void HostTypeFree(Actor* a);                            // 0x8015074C
void HostTypeInit(int owner, Actor* a);                 // 0x801507D4

int HostTypeMsgA(int msg, Actor* a);                    // 0x80150B18
int HostTypeMsgB(int msg, Actor* a);                    // 0x80150C14
int HostTypeCheckA(Actor* a, int next);                 // 0x80150D94
int HostTypeCheckB(Actor* a, int next);                 // 0x80150DB4
int HostTypeCheckC(Actor* a, int next);                 // 0x80150E84
int HostTypeCheckD(Actor* a, int next);                 // 0x80150F30
int HostTypeCheckE(Actor* a, int next);                 // 0x80151004
int HostTypeUpdateC(Actor* a);                          // 0x80151068
int HostTypeUpdateD(Actor* a);                          // 0x801510D0
int HostTypeUpdateE(Actor* a);                          // 0x8015115C
int HostTypeUpdateF(Actor* a);                          // 0x801511B8
int HostTypeUpdateG(Actor* a);                          // 0x8015136C
int HostTypeUpdateH(Actor* a);                          // 0x801514CC
int HostTypeUpdateI(Actor* a);                          // 0x80151674
int HostTypeUpdateJ(Actor* a);                          // 0x80151828
int HostTypeUpdateK(Actor* a);                          // 0x801519EC

int HostTypeEnter2F(Actor* a);                          // 0x80151B38
int HostTypeEnter2E(Actor* a);                          // 0x80151B9C
int HostTypeEnter22(Actor* a);                          // 0x80151C00
int HostTypeEnter21(Actor* a);                          // 0x80151C60
int HostTypeEnter15(Actor* a);                          // 0x80151CC4
int HostTypeEnter14(Actor* a);                          // 0x80151D28
int HostTypeEnter29(Actor* a);                          // 0x80151D88
int HostTypeEnter28(Actor* a);                          // 0x80151DE8
int HostTypeEnter1C(Actor* a);                          // 0x80151E48
int HostTypeEnter1B(Actor* a);                          // 0x80151EA4
int HostTypeEnter0F(Actor* a);                          // 0x80151F04
int HostTypeEnter0E(Actor* a);                          // 0x80151F64
int HostTypeEnter2C(Actor* a);                          // 0x80151FC0
int HostTypeEnter2B(Actor* a);                          // 0x80152020
int HostTypeEnter1F(Actor* a);                          // 0x80152080
int HostTypeEnter1E(Actor* a);                          // 0x801520DC
int HostTypeEnter12(Actor* a);                          // 0x8015213C
int HostTypeEnter11(Actor* a);                          // 0x8015219C
int HostTypeEnter0C(Actor* a);                          // 0x801521F8
int HostTypeEnter0B(void* p);                           // 0x801522AC
int HostTypeEnter0A(Actor* a);                          // 0x801523D8
int HostTypeEnter27(Actor* a);                          // 0x80152488
int HostTypeEnter1A(Actor* a);                          // 0x801524E0
int HostTypeEnter0D(Actor* a);                          // 0x80152538
int HostTypeEnter2A(Actor* a);                          // 0x80152590
int HostTypeEnter1D(Actor* a);                          // 0x801525E8
int HostTypeEnter10(Actor* a);                          // 0x80152640
int HostTypeEnter09(Actor* a);                          // 0x80152698
int HostTypeEnter2D(Actor* a);                          // 0x80152780
int HostTypeEnter20(Actor* a);                          // 0x801527DC
int HostTypeEnter13(Actor* a);                          // 0x80152838
int HostTypeEnter06(Actor* a);                          // 0x80152894
int HostTypeEnter03(Actor* a);                          // 0x801528E8
int HostTypeEnter00(Actor* a);                          // 0x80152938

static inline int PossessHostCheck(void* data) {
    Actor* a = ((MsgHdr*)data)->actor;

    if (a == 0) {
        return 1;
    }
    if (fn_80116E08(a) == 0) {
        return 1;
    }
    return 0;
}

int PossessHostMsg(int msg, void* data) {
    MsgHdr* m = (MsgHdr*)data;
    Actor* a = *(Actor**)data;
    HostState* s = a->state;

    switch (msg) {
    case 0x3300:
        return PossessHostCheck(data);
    case 0xC06:
        if (m->cmd == 0x46341C3A || m->cmd == 0x239807 || m->cmd == 0x4A5C9FC) {
            fn_800CB8BC(a, a->iC4, s->unk2C, a->i26C);
        }
        return 0;
    case 0x41E: {
        Msg41E* q = (Msg41E*)data;

        switch (q->cmd) {
        case 0xAD8576FA: {
            String name(lbl_803A5488[s->index].sep);
            String sep("\n");
            const String& text = fn_800225C8(name, sep);

            fn_8028C188(&q->result, text.mData, text.mLength);
            return 0;
        }
        }
        return 0;
    }
    }
    return 0;
}

int PossessHostUpdate(Actor* a) {
    int next;
    HostState* s;
    int ok;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    if (s->f20 > 0.0f) {
        s->f20 = s->f20 - a->dt;
    }
    if ((fn_8008EA9C(a->iD4, 0) == 0 || fn_8008EA9C(a->iD4, 0) == 2) &&
        HasCondition(&a->i270, 13) == 0 && HasCondition(&a->i270, 10) == 0) {
        lbl_803A5488[s->index].enter(a);
    }
    if (a->i220 == 4) {
        fn_80152990(a);
        next = s->index;
    } else {
        next = lbl_803A5488[s->index].update(a);
    }
    if (next != s->index) {
        ok = lbl_803A5488[s->index].check(a, next);
        if (ok != 0) {
            lbl_803A5488[next].enter(a);
        }
    }
    HostTypeTrails(a);
    return 1;
}

void HostTypeTrails(Actor* a) {
    HostState* s;
    int* trail;
    int i;

    s = a->state;
    if (fn_80116E08(a) != 0) {
        trail = s->trail;
        for (i = 0; i != 3; i++) {
            if (*trail >= 0) {
                TrailDestroy(*trail);
                *trail = -1;
            }
            trail++;
        }
    } else if (s->trail[0] < 0 && s->trail[1] < 0 && s->trail[2] < 0) {
        TrailDesc d;

        d.owner = a->i38;
        d.width = 49.0f;
        d.unk14 = -4.0f;
        d.unk20 = 0.01f;
        d.unk24 = -5.0f;
        d.unk28 = 31.0f;
        d.unk2C = 0.3f;
        d.texture = AssetFind(0xF2E2BEB8, 0x98909881);
        d.segments = 500;
        d.color[0] = 0xC3;
        d.color[1] = 0xFC;
        d.color[2] = 0xC8;
        d.unk8 = ModelFindBone(a->iCC, lbl_8046DDA8[0]);
        s->trail[0] = TrailCreate(&d);
        s->trail[1] = -1;
        s->trail[2] = -1;
    }
}

void HostTypeFree(Actor* a) {
    HostState* s;
    int i;

    s = a->state;
    for (i = 0; i != 3; i++) {
        if (s->trail[i] >= 0) {
            TrailDestroy(s->trail[i]);
            s->trail[i] = -1;
        }
    }
    a->fn110 = 0;
    VFree(a->state);
    a->state = 0;
}

void HostTypeInit(int owner, Actor* a) {
    HostState* s;

    if (a != 0) {
        a->fn110 = PossessHostUpdate;
        a->fn118 = fn_800CDD10;
        s = (HostState*)VAlloc(0x58);
        if (s == 0) {
            a->state = 0;
        } else {
            s->owner = owner;
            s->unk0C = -1;
            s->unk10 = -1;
            s->unk38 = 0;
            s->unk14 = 0;
            s->f28 = -1.0f;
            a->state = s;
            s->f18 = 0.0f;
            s->unk1C = 0;
            s->f24 = 3000.0f * ((float)rand() / 32767.0f) + 3000.0f;
            s->f20 = 0.0f;
            s->unk2C = 0;
            s->unk30 = -1;
            s->unk34 = 1;
            s->f48 = 0.0f;
            s->f3C = 0.0f;
            s->f40 = 0.0f;
            s->f44 = 0.0f;
            if (a->f904 <= 0.0f) {
                s->index = 0xB;
                s->sub = 0xC;
            } else {
                s->index = 0;
                s->sub = 0;
            }
            lbl_8046DDA8[0] = AssetFind(0xF1754BF2, 0x21C5E0);
            lbl_8046DDA8[1] = AssetFind(0xF1754BF2, 0x24805F);
            lbl_8046DDA8[2] = AssetFind(0xF1754BF2, 0x101844CC);
            lbl_8046DDA8[3] = AssetFind(0xF1754BF2, 0x48874F6E);
            lbl_8046DDA8[4] = AssetFind(0xF1754BF2, 0x4A53FC1);
            lbl_8046DDA8[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
            lbl_8046DDA8[6] = AssetFind(0xF1754BF2, 0xD9404234);
            lbl_8046DDA8[7] = AssetFind(0xF1754BF2, 0x450B2BB);
            lbl_8046DDA8[8] = AssetFind(0xF1754BF2, 0x922BF04E);
            lbl_8046DDA8[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
            lbl_8046DDA8[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
            lbl_8046DDA8[11] = AssetFind(0xF1754BF2, 0x4A2F9AA);
            lbl_8046DDA8[12] = AssetFind(0xF1754BF2, 0x4A48BC0);
            lbl_8046DDA8[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
            lbl_8046DDA8[14] = AssetFind(0xF1754BF2, 0x44E6CA4);
            lbl_8046DDA8[15] = AssetFind(0xF1754BF2, 0x44FFEBA);
            s->trail[0] = -1;
            s->trail[1] = -1;
            s->trail[2] = -1;
        }
    }
}

int HostTypeMsgA(int msg, Actor* a) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    switch (msg) {
    case 0x1B83833F:
        lbl_803A5488[s->index].check(a, s->sub);
        lbl_803A5488[s->sub].enter(a);
        break;
    case 0x46341C3A:
    case 0x239807:
    case 0x4A5C9FC:
        fn_800CB8BC(a, a->iC4, s->unk2C, a->i26C);
        break;
    }
    return 1;
}

int HostTypeMsgB(int msg, Actor* a) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    switch (msg) {
    case 0x4A5C9FC:
    case 0x46341C3A:
        fn_800CB8BC(a, a->iC4, s->unk2C, a->i26C);
        break;
    case 0x239807:
        if (fn_8016325C(a->obj894) == 4) {
            fn_80162610(a, a->obj894, 0x20EE36);
        } else if (fn_8016325C(a->obj894) == 5) {
            fn_80162610(a, a->obj894, 0xF829DB93);
        }
        break;
    case 0x1B83833F:
        lbl_803A5488[s->index].check(a, s->sub);
        lbl_803A5488[s->sub].enter(a);
        if (fn_8016325C(a->obj894) != 0) {
            fn_80162610(a, a->obj894, 0x1B83833F);
        }
        break;
    }
    return 1;
}

int HostTypeCheckA(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    a->state->sub = next;
    return 1;
}

int HostTypeCheckB(Actor* a, int next) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    fn_800A7A54(&a->i270, 0, 0);
    a->pos[0] = 0.0f;
    a->pos[1] = 0.0f;
    a->pos[2] = 0.0f;
    if (s->index % 13 == 10 && next % 13 != 10) {
        s->sub = next;
    }
    fn_800A7A54(&a->i270, 4, 0);
    return 1;
}

int HostTypeCheckC(Actor* a, int next) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    fn_800A7A54(&a->i270, 0, 0);
    a->pos[0] = 0.0f;
    a->pos[1] = 0.0f;
    a->pos[2] = 0.0f;
    if (next % 13 != 10) {
        s->sub = next;
    }
    fn_800A7A54(&a->i270, 4, 0);
    return 0;
}

int HostTypeCheckD(Actor* a, int next) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    if (fn_8016325C(a->obj894) == 6) {
        fn_80162610(a, a->obj894, 0xE6B71BCC);
    } else if (fn_8016325C(a->obj894) == 7) {
        fn_80162610(a, a->obj894, 0x968CBE3D);
    }
    s->sub = next;
    fn_800A7A54(&a->i270, 0, 0);
    if (fn_8016325C(a->obj894) != 0) {
        fn_80162610(a, a->obj894, 0x1B83833F);
    }
    return 1;
}

int HostTypeCheckE(Actor* a, int next) {
    if (a == 0) {
        return 0;
    }
    a->state->sub = next;
    if (fn_8016325C(a->obj894) != 0) {
        fn_80162610(a, a->obj894, 0x1B83833F);
    }
    return 1;
}

int HostTypeUpdateC(Actor* a) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    if (a->p5F0 == 0 || fn_80212730(a->i38) != -1) {
        fn_80152990(a);
    }
    return s->index;
}

int HostTypeUpdateD(Actor* a) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    fn_80152990(a);
    if (HasCondition(&a->i270, 9) != 0 && HasCondition(&a->i270, 10) == 0 &&
        HasCondition(&a->i270, 11) == 0) {
        return 0xB;
    }
    return s->index;
}

int HostTypeUpdateE(Actor* a) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    fn_80152990(a);
    fn_80162610(a, a->obj894, 0x1B83833F);
    return s->sub;
}

// Index of a host-type record row: 13 sub-states per host, plus an offset.
static inline int SeqIndex(int row, int base, int off) {
    return row * 13 + base + off;
}

int HostTypeUpdateF(Actor* a) {
    HostState* s;
    int iVar1;
    int iVar2;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    fn_80152990(a);
    iVar1 = HasCondition(&a->i270, 9);
    if (iVar1 != 0 && HasCondition(&a->i270, 10) == 0 &&
        HasCondition(&a->i270, 11) == 0) {
        return 0xB;
    }
    iVar1 = HasCondition(&a->i270, 4);
    if ((iVar1 != 0) && HasCondition(&a->i270, 9) == 0 &&
        (HasCondition(&a->i270, 10) == 0 && s->f20 <= 0.0f)) {
        fn_800A7A54(&a->i270, 4, 0);
        return 10;
    }
    iVar1 = fn_800A82D0(a, a->pos);
    if (iVar1 == 3) {
        iVar1 = 0;
    }
    iVar2 = fn_8016325C(a->obj894);
    if (iVar2 == 4 || fn_8016325C(a->obj894) == 5) {
        iVar1 = a->i288 * 13 + iVar1 + 3;
    } else {
        iVar2 = fn_8016325C(a->obj894);
        if (iVar2 == 6 || fn_8016325C(a->obj894) == 7) {
            iVar1 = a->i288 * 13 + iVar1 + 6;
        } else {
            iVar2 = a->i26C;
            if (s->unk38 == iVar2) {
                if (s->unk34 == iVar1) {
                    iVar1 = s->index;
                } else {
                    iVar1 = iVar1 + iVar2 * 13;
                }
            } else {
                iVar1 = iVar1 + iVar2 * 13;
            }
        }
    }
    return iVar1;
}

int HostTypeUpdateG(Actor* a) {
    HostState* s;
    int iVar1;
    int iVar2;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    iVar1 = fn_800A82D0(a, a->pos);
    if (iVar1 == 3) {
        iVar1 = 0;
    }
    fn_80152990(a);
    iVar2 = HasCondition(&a->i270, 9);
    if (iVar2 != 0 && HasCondition(&a->i270, 10) == 0 &&
        HasCondition(&a->i270, 11) == 0) {
        return 0xB;
    }
    iVar2 = HasCondition(&a->i270, 4);
    if ((iVar2 != 0) && HasCondition(&a->i270, 9) == 0 &&
        (HasCondition(&a->i270, 10) == 0 && s->f20 <= 0.0f)) {
        fn_800A7A54(&a->i270, 4, 0);
        return 10;
    }
    if (fabsf(a->f284) < 1.0f) {
        return a->i26C * 13 + iVar1 + s->sub;
    }
    if (a->i288 != 0) {
        return a->i26C * 13 + 9;
    }
    return s->index;
}

int HostTypeUpdateH(Actor* a) {
    HostState* s;
    int iVar1;
    int iVar2;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    fn_80152990(a);
    iVar1 = fn_800A82D0(a, a->pos);
    if (iVar1 == 3) {
        iVar1 = 0;
    }
    iVar2 = HasCondition(&a->i270, 9);
    if (iVar2 != 0 && HasCondition(&a->i270, 10) == 0 &&
        HasCondition(&a->i270, 11) == 0) {
        return 0xB;
    }
    iVar2 = HasCondition(&a->i270, 4);
    if ((iVar2 != 0) && HasCondition(&a->i270, 9) == 0 &&
        (HasCondition(&a->i270, 10) == 0 && s->f20 <= 0.0f)) {
        fn_800A7A54(&a->i270, 4, 0);
        return 10;
    }
    iVar2 = a->i26C;
    if (s->unk38 != iVar2) {
        iVar1 += iVar2 * 13;
        return iVar1 + 3;
    } else {
        if (s->unk34 != iVar1) {
            iVar1 += iVar2 * 13;
            return iVar1 + 3;
        } else {
            iVar2 = fn_8016325C(a->obj894);
            if (iVar2 == 6 || fn_8016325C(a->obj894) == 7) {
                return iVar1 + a->i288 * 13 + 6;
            } else {
                iVar2 = fn_8016325C(a->obj894);
                if (iVar2 == 0) {
                    return iVar1 + a->i288 * 13;
                } else {
                    iVar1 = s->index;
                }
            }
        }
    }
    return iVar1;
}

int HostTypeUpdateI(Actor* a) {
    HostState* s;
    float f;
    int iVar2;
    int iVar3;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    iVar2 = fn_800A82D0(a, a->pos);
    if (iVar2 == 3) {
        iVar2 = 0;
    }
    fn_80152990(a);
    iVar3 = HasCondition(&a->i270, 9);
    if (iVar3 != 0 && HasCondition(&a->i270, 10) == 0 &&
        HasCondition(&a->i270, 11) == 0) {
        return 0xB;
    }
    iVar3 = HasCondition(&a->i270, 4);
    if ((iVar3 != 0) && HasCondition(&a->i270, 9) == 0 &&
        (HasCondition(&a->i270, 10) == 0 && s->f20 <= 0.0f)) {
        fn_800A7A54(&a->i270, 4, 0);
        return 10;
    }
    if (s->unk38 != a->i26C) {
        return iVar2 + a->i26C * 13 + 3;
    } else {
        iVar3 = fn_8016325C(a->obj894);
        if (iVar3 == 6 || fn_8016325C(a->obj894) == 7) {
            return a->i288 * 13 + iVar2 + 6;
        } else {
            iVar3 = fn_8016325C(a->obj894);
            if (iVar3 == 0) {
                return iVar2 + a->i288 * 13;
            } else {
                f = a->f284;
                if (f < 0.0f) {
                    f = -f;
                }
                if (f > 35.0f) {
                    return 9;
                } else {
                    return s->index;
                }
            }
        }
    }
    return iVar2;
}

int HostTypeUpdateJ(Actor* a) {
    HostState* s;
    float f;
    int iVar2;
    int iVar3;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    fn_80152990(a);
    iVar2 = HasCondition(&a->i270, 9);
    if (iVar2 != 0 && HasCondition(&a->i270, 10) == 0 &&
        HasCondition(&a->i270, 11) == 0) {
        return 0xB;
    }
    iVar2 = HasCondition(&a->i270, 4);
    if ((iVar2 != 0) && HasCondition(&a->i270, 9) == 0 &&
        (HasCondition(&a->i270, 10) == 0 && s->f20 <= 0.0f)) {
        fn_800A7A54(&a->i270, 4, 0);
        return 10;
    }
    iVar2 = fn_800A82D0(a, a->pos);
    if (iVar2 == 3) {
        iVar2 = 0;
    }
    iVar3 = fn_8016325C(a->obj894);
    if (iVar3 == 4 || fn_8016325C(a->obj894) == 5) {
        return a->i288 * 13 + iVar2 + 3;
    } else {
        iVar3 = fn_8016325C(a->obj894);
        if (iVar3 == 6 || fn_8016325C(a->obj894) == 7) {
            return a->i288 * 13 + iVar2 + 6;
        } else if (s->unk38 != a->i26C) {
            return iVar2 + a->i26C * 13;
        } else {
            f = a->f284;
            if (f < 0.0f) {
                f = -f;
            }
            if (f > 35.0f) {
                return 9;
            } else {
                return s->index;
            }
        }
    }
    return iVar2;
}

int HostTypeUpdateK(Actor* a) {
    HostState* s;
    int result;
    String str;
    MsgC14 msg;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    s->f24 = s->f24 - a->dt;
    if (s->f24 <= 0.0f) {
        str.mData = 0;
        msg.unk04 = 0;
        msg.unk08 = 0;
        msg.hash = 0x1512C7;
        SendMsg(0x4B000000, 0x41E, &msg);
        if (str.mData != 0) {
            int id = OwnerId(a);
            fn_80120AC0(0.0, 0x7BFB940D, 3, id & 0xFFFFFF);
        }
        int obj = fn_80116E08(a);
        if (obj != 0 && (obj = fn_80116DC4(a), obj != 0) && ((u32)(*(int*)(obj + 0x38)) >> 24) == 0x13) {
            fn_800FC764(*(int*)(obj + 0x38), 0);
        }
        if (fn_802A1D04() != 3) {
            int id = OwnerId(a);
            fn_800FC764(id, 0);
        }
    }
    return s->index;
}

#define ENTER_HEAD   HostState* s;
#define ENTER_BEGIN  if (a == 0) { return 0; } s = a->state;

int HostTypeEnter2F(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x2F;
    s->sub = 0x29;
    fn_801537E8(a, -1);
    s->unk38 = 3;
    s->unk34 = 2;
    return 1;
}

int HostTypeEnter2E(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x2E;
    s->sub = 0x28;
    fn_801537E8(a, -1);
    s->unk38 = 3;
    s->unk34 = 1;
    return 1;
}

int HostTypeEnter22(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x22;
    s->sub = 0x1C;
    fn_801537E8(a, -1);
    s->unk38 = 2;
    s->unk34 = 2;
    return 1;
}

int HostTypeEnter21(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x21;
    s->sub = 0x1B;
    fn_801537E8(a, -1);
    s->unk38 = 2;
    s->unk34 = 1;
    return 1;
}

int HostTypeEnter15(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x15;
    s->sub = 0x0F;
    fn_801537E8(a, -1);
    s->unk38 = 1;
    s->unk34 = 2;
    return 1;
}

int HostTypeEnter14(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x14;
    s->sub = 0x0E;
    fn_801537E8(a, -1);
    s->unk38 = 1;
    s->unk34 = 1;
    return 1;
}

int HostTypeEnter29(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x29;
    s->sub = 0x29;
    fn_801537E8(a, -1);
    s->unk38 = 3;
    s->unk34 = 2;
    return 1;
}

int HostTypeEnter28(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x28;
    s->sub = 0x28;
    fn_801537E8(a, -1);
    s->unk38 = 3;
    s->unk34 = 1;
    return 1;
}

int HostTypeEnter1C(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x1C;
    s->sub = 0x1C;
    fn_801537E8(a, -1);
    s->unk38 = 2;
    s->unk34 = 2;
    return 1;
}

int HostTypeEnter1B(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x1B;
    s->sub = 0x1B;
    fn_801537E8(a, -1);
    s->unk38 = 2;
    s->unk34 = 1;
    return 1;
}

int HostTypeEnter0F(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x0F;
    s->sub = 0x0F;
    fn_801537E8(a, -1);
    s->unk38 = 1;
    s->unk34 = 2;
    return 1;
}

int HostTypeEnter0E(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x0E;
    s->sub = 0x0E;
    fn_801537E8(a, -1);
    s->unk38 = 1;
    s->unk34 = 1;
    return 1;
}

int HostTypeEnter2C(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x2C;
    s->sub = 0x2C;
    fn_801537E8(a, -1);
    s->unk38 = 3;
    s->unk34 = 2;
    return 1;
}

int HostTypeEnter2B(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x2B;
    s->sub = 0x2B;
    fn_801537E8(a, -1);
    s->unk38 = 3;
    s->unk34 = 1;
    return 1;
}

int HostTypeEnter1F(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x1F;
    s->sub = 0x1F;
    fn_801537E8(a, -1);
    s->unk38 = 2;
    s->unk34 = 2;
    return 1;
}

int HostTypeEnter1E(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x1E;
    s->sub = 0x1E;
    fn_801537E8(a, -1);
    s->unk38 = 2;
    s->unk34 = 1;
    return 1;
}

int HostTypeEnter12(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x12;
    s->sub = 0x12;
    fn_801537E8(a, -1);
    s->unk38 = 1;
    s->unk34 = 2;
    return 1;
}

int HostTypeEnter11(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x11;
    s->sub = 0x11;
    fn_801537E8(a, -1);
    s->unk38 = 1;
    s->unk34 = 1;
    return 1;
}

int HostTypeEnter0C(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    a->fn10C = fn_800A7068;
    s->index = 0x0C;
    s->sub = 0x0C;
    s->unk38 = 0;
    s->unk34 = 0;
    s->f24 = lbl_804D2F10;
    fn_8008DE4C(a->iD4, 0, s->unk0C, 0x18);
    if (HasCondition(&a->i270, 10) == 0) {
        fn_800A7A54(&a->i270, 10, 1);
        ActorPlayAnim(a->i38, 0xDC04E3D5, a->i38);
    }
    return 1;
}

// Bone index used for a host's possession effect.
static inline int HostBoneIndex(Actor* a) {
    int n = a->i214;

    if (n == -1) {
        return 8;
    }
    n = fn_80018814(a->iCC, ModelBoneName(a->iCC), lbl_8046DDA8, 0x10);
    if (n == -1) {
        return 8;
    }
    return n;
}

int HostTypeEnter0B(void* p) {
    HostState* s;
    MsgC14 msg;
    int id;
    Actor* a = (Actor*)p;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    s->index = 0x0B;
    s->sub = 0x0C;
    s->unk38 = 0;
    s->unk34 = 0;
    fn_800A7A54(&a->i270, 0, 1);
    if (a->p5F0 == 0 || fn_80212730(a->i38) != -1) {
        fn_801537E8(a, HostBoneIndex(a));
    }
    id = OwnerId(a);
    int obj = OwnerId(a);
    if (obj != -1) {
        msg.unk08 = id;
        msg.hash = 0x274B01CB;
        msg.unk04 = 0;
        SendMsg(obj, 0xC14, &msg);
    }
    TalkPlay(OwnerId(a), 8);
    return 1;
}

int HostTypeEnter0A(Actor* a) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    s->sub = s->index;
    s->index = 10;
    s->f20 = 1500.0f;
    s->unk38 = 0;
    s->unk34 = 0;
    fn_801537E8(a, HostBoneIndex(a));
    return 1;
}

int HostTypeEnter27(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x27;
    s->sub = 0x27;
    s->unk38 = 3;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter1A(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x1A;
    s->sub = 0x1A;
    s->unk38 = 2;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter0D(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x0D;
    s->sub = 0x0D;
    s->unk38 = 1;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter2A(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x2A;
    s->sub = 0x2A;
    s->unk38 = 3;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter1D(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x1D;
    s->sub = 0x1D;
    s->unk38 = 2;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter10(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x10;
    s->sub = 0x10;
    s->unk38 = 1;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter09(Actor* a) {
    HostState* s;

    if (a == 0) {
        return 0;
    }
    s = a->state;
    s->sub = 0;
    s->index = 9;
    s->unk38 = 0;
    s->unk34 = 0;
    if (a->f284 > 0.0f) {
        s->unk0C = s->unk0C = fn_800A1BC8(*(int*)(a->iC4 + 0x14), *(int*)(a->iC4 + 0x1C), 0x22B02743, 0, 0);
    } else {
        s->unk0C = s->unk0C = fn_800A1BC8(*(int*)(a->iC4 + 0x14), *(int*)(a->iC4 + 0x1C), 0xA1B7BA66, 0, 0);
    }
    fn_80090974(a->iD4, 0, s->unk0C, 1.0f, 0x11, lbl_803A5488[9].msg, a);
    return 1;
}

int HostTypeEnter2D(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x2D;
    s->sub = 0x27;
    s->unk38 = 3;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter20(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x20;
    s->sub = 0x1A;
    s->unk38 = 2;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter13(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0x13;
    s->sub = 0x0D;
    s->unk38 = 1;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter06(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 6;
    s->sub = 0;
    s->unk38 = 0;
    s->unk34 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter03(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 3;
    s->sub = 3;
    s->unk38 = 0;
    fn_801537E8(a, -1);
    return 1;
}

int HostTypeEnter00(Actor* a) {
    ENTER_HEAD
    ENTER_BEGIN
    s->index = 0;
    s->sub = 0;
    fn_801537E8(a, -1);
    s->unk38 = 0;
    return 1;
}

// Enter a host-type state: allocate the type's action id and prime the state block.
void fn_801537E8(Actor* a, int arg) {
    HostState* s;
    if (a == 0) {
        return;
    }
    s = a->state;
    s->f3C = 0.0f;
    s->f40 = 0.0f;
    s->f44 = 0.0f;
    fn_800A7A54(&a->i270, 0xC, 0);
    if (arg != -1) {
        s->unk0C = fn_800A1BC8(*(int*)(a->iC4 + 0x14), *(int*)(a->iC4 + 0x1C),
                               lbl_803A5488[s->index].id, arg, 0);
    } else {
        s->unk0C = fn_800A1BC8(*(int*)(a->iC4 + 0x14), *(int*)(a->iC4 + 0x1C),
                               lbl_803A5488[s->index].id, 0, 0);
    }
    fn_80090974(a->iD4, 0, s->unk0C, 1.0f, 9, lbl_803A5488[s->index].msg, a);
    if (arg != -1) {
        if (HasCondition(&a->i270, 0xE) != 0) {
            fn_8008F230(a->iD4, 0, 0, 0.0f);
            fn_8008F230(a->iD4, 1, 0, 0.0f);
            fn_800A7A54(&a->i270, 0xE, 0);
        } else if (s->f28 >= 0.0f) {
            fn_8008F230(a->iD4, 0, 1, s->f28);
            s->f28 = -1.0f;
        } else if (s->unk10 == s->unk0C) {
            fn_8008F230(a->iD4, 0, 0, 0.0f);
            fn_8008F230(a->iD4, 1, 0, 0.0f);
        } else if (a->i288 != 0 && a->i26C != 0) {
            if (s->unk14 == a->i288) {
                fn_8008F230(a->iD4, 0, 0, 0.0f);
                fn_8008F230(a->iD4, 1, 0, 0.0f);
            } else {
                fn_8008F230(a->iD4, 0, 1, 100.0f);
                fn_8008F230(a->iD4, 1, 1, 100.0f);
            }
        } else {
            fn_8008F230(a->iD4, 0, 1, 150.0f);
            fn_8008F230(a->iD4, 1, 1, 150.0f);
        }
    }
    s->unk14 = a->i288;
    s->unk10 = s->unk0C;
}

// Check callback for the host descriptor: query the target actor (msg 0x400).
int fn_80153A64(int* arg, int* out) {
    HostQuery msg;
    Actor* a = (Actor*)arg[0];
    int type;
    if (a != 0) {
        msg.actor = a;
        ((HostQueryFunc)msg.actor->msgFn)(0x400, &msg);
        type = (int)(*(u32*)(arg[0] + 0x38) >> 24);
        if ((type == 0x13 || type == 2) && (msg.flags & 2) == 0) {
            return 0;
        }
        out[0] = arg[0xE];
        out[1] = arg[3];
        out[2] = msg.value;
        return 1;
    } else {
        out[0] = arg[0xE];
        out[1] = arg[3];
        out[2] = -1;
        return 1;
    }
}

// Find the partner actor whose lead slot carries the given host id.
int fn_80153B28(int id) {
    int n = lbl_804D4E20;
    HostEntry* e;
    for (int i = 0; i < n; i++) {
        e = lbl_803A5968[i];
        if (e->lead != 0 && id == e->lead->i38 && e->trail != 0) {
            return lbl_803A5968[i]->trail->i38;
        }
    }
    return -1;
}

// Find the partner actor whose trail slot carries the given host id.
int fn_80153B9C(int id) {
    int n = lbl_804D4E20;
    HostEntry* e;
    for (int i = 0; i < n; i++) {
        e = lbl_803A5968[i];
        if (e->trail != 0 && id == e->trail->i38 && e->lead != 0) {
            return lbl_803A5968[i]->lead->i38;
        }
    }
    return -1;
}

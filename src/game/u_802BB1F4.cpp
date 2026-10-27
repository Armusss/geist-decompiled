// u_802BB1F4.cpp: "FshSve" actor states: queries, follow, zone checks and frame update (0x802BB1F4-0x802BF114).
// Names are descriptive guesses; the code is matched against the retail DOL.
// FSHSVE actor: behaviour state machine, scale/gravity handling and the
// possession message protocol. Names are descriptive guesses.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef float f32;
typedef double f64;

class Owner;

// per-instance behaviour data, hung off Actor::beh (0x130)
struct BehData {
    s32 state;   // 0x00
    f32 unk04;   // 0x04
    s32 unk08;   // 0x08
    f32 unk0C;   // 0x0c
    f32 unk10;   // 0x10
    f32 unk14;   // 0x14
    s32 unk18;   // 0x18
    f32 unk1C;   // 0x1c
    f32 unk20;   // 0x20
    f32 unk24;   // 0x24
    s32 unk28;   // 0x28
    f32 unk2C;   // 0x2c
    s32 unk30;   // 0x30
    f32 unk34;   // 0x34
    s32 unk38;   // 0x38
    u32 flags3C; // 0x3c
    f32 unk40;   // 0x40
    u32 unk44;   // 0x44
    s32 unk48;   // 0x48
    s32 unk4C;   // 0x4c
    s32 unk50;   // 0x50
};

struct C4Data {
    u8  unk00[0x30];         // 0x00
    u32 unk30;               // 0x30
    u8  unk34[0xC8 - 0x34];  // 0x34
    s32 unkC8;               // 0xc8
};

struct Actor {
    u8      unk00[0x24];           // 0x00
    u8      unk24[0x14];           // 0x24
    void*   unk38;                 // 0x38
    u8      unk3C[0x4C - 0x3C];    // 0x3c
    u32     flags4C;               // 0x4c
    u8      unk50[0xC4 - 0x50];    // 0x50
    C4Data* unkC4;                 // 0xc4
    u8      unkC8[0xFC - 0xC8];    // 0xc8
    void*   unkFC;                 // 0xfc
    u8      unk100[0x104 - 0x100]; // 0x100
    void*   unk104;                // 0x104
    void*   unk108;                // 0x108
    void*   unk10C;                // 0x10c
    u8      unk110[0x130 - 0x110]; // 0x110
    BehData* beh;                  // 0x130
    f32     unk138;                // 0x138
    u8      unk13C[0x21C - 0x13C]; // 0x13c
    s32     unk21C;                // 0x21c
    s32     unk220;                // 0x220
    u8      unk224[0x22C - 0x224]; // 0x224
    s32     unk22C;                // 0x22c
    u8      unk230[0x268 - 0x230]; // 0x230
    s32     unk268;                // 0x268
    s32     unk26C;                // 0x26c
    u8      unk270[0x288 - 0x270]; // 0x270
    s32     unk288;                // 0x288
    u8      unk28C[0x2C4 - 0x28C]; // 0x28c
    void*   unk2C4;                // 0x2c4
    u8      unk2C8[0x5B8 - 0x2C8]; // 0x2c8
    f32     unk5B8;                // 0x5b8
    u8      unk5BC[0x604 - 0x5BC]; // 0x5bc
    void*   unk604;                // 0x604
    u8      unk608[0x894 - 0x608]; // 0x608
    u8      unk894[0x904 - 0x894]; // 0x894
    f32     unk904;                // 0x904
    u8      unk908[0x91C - 0x908]; // 0x908
    f32     unk91C;                // 0x91c
};

// generic message argument: the same slot is read as int, float or pointer
union MsgArg {
    s32 i;
    u32 u;
    f32 f;
    void* p;
};

struct GeistString {
    void* data;  // 0x00
    u32   unk04; // 0x04
    u32   unk08; // 0x08
    u32   unk0C; // 0x0c
    u32   unk10; // 0x10
};

extern "C" {
extern f32 lbl_804DACE0;
extern f32 lbl_804DACE4;
extern f64 lbl_804DACE8;
extern f32 lbl_804DACF0;
extern f32 lbl_804DACF4;
extern f32 lbl_804DACF8;
extern f32 lbl_804DACFC;
extern s32 lbl_804D3970;
extern u32 lbl_804D3974;
extern u32 lbl_804D3978;
extern f32 lbl_804D397C;
extern f32 lbl_804D3980;
extern f32 lbl_804D3984;
extern s32 lbl_804D3988;
extern s32 lbl_804D398C;
extern s32 lbl_804D3990;
extern s32 lbl_804D3994;
extern s32 lbl_804D3998;
extern u32 lbl_804D39B4;
extern u32 lbl_804D39B8;
extern s32 lbl_804D39BC;
extern f32 lbl_804D39C0;
extern f32 lbl_804D39C4;

extern void* PTR_DAT_803C4B54[];
extern void* PTR_FUN_803C4B48[];
extern void* PTR_FUN_803C4B4C[];
extern void* PTR_FUN_803C4B50[];
extern char  lbl_8034BDDD[];
extern char  lbl_8034BDD4[];
extern char  lbl_80393D7C[];
extern u8    __upper_map[];

extern u32 fn_80049940(void);
extern s32 fn_800C5654(u32 a, s32 b, s32 c);
extern void fn_800265D4(s32 a, s32 b, void* c);
extern u32 fn_80019F88(MsgArg* args, Actor* self);
extern void fn_8028C400(void* out, void* src);
extern void fn_8028C188(void* out, u32 a, u32 b);
extern void fn_8028B9C0(void* str, s32 flag);
extern void fn_800225C8(void* out, void* a, void* b);
extern void fn_800C99E8(Actor* self, u32 a, u32 b, s32 c, s32 d);
extern f32 fn_802A2080(void);
extern u32 fn_8016F120(u32 a, void* b);
extern s32 fn_800A7AC4(void* obj, s32 a);
extern s32 fn_800BEF9C(void* p);
extern s32 fn_800BAF40(Actor* self);
extern void fn_80162658(Actor* self, void* p);
extern void fn_800A7A54(void* obj, s32 a, s32 b);
}

extern int OwnerId(Owner* owner);
void OwnerUpdateSlots(Owner* owner);

// follow helpers, already named in symbols.txt
void* FollowGetTargetObj(void* obj);                               // FollowGetTargetObj__FPv
int   HasCondition(void* obj, int cond);                           // HasCondition__FPvi
int   FollowLineCheckActorDist(void* obj, void* target, f32 dist); // FollowLineCheckActorDist__FPvPvf
void  FollowOffsetUpdate(void* obj);                               // FollowOffsetUpdate__FPv
void  RangeResetPending(void* obj);                                // RangeResetPending__FPv
int   MemFlagIsSet(void* obj, int flag);                           // MemFlagIsSet__FPvi

// callback stored at Actor::0x604: (self, target id)
typedef int (*ActorCheckFn)(void* self, void* target);

// game helpers still under placeholder names in symbols.txt (C linkage)
extern "C" {
s32   fn_8029B3B0(Actor* self, u32 id);
void  fn_80162610(Actor* self, void* list, s32 mode);
s32   fn_80162940(void* list, s32 index);
s32   fn_800B9104(u32 hash, s32 a, void* b, s32 c);
// some call sites pass only the actor; the retail code leaves r4 untouched there,
// so the optional parameter keeps those calls one-argument.
void  fn_800BC56C(Actor* self, void* target = 0);
void  fn_800BBCA8(void);
void  fn_8029B070(Actor* self, u32 id);
void  fn_800B2E34(Actor* self, s32 a, s32 b);
void  fn_800B3164(Actor* self, s32 a, s32 b);
void  fn_800CECCC(Actor* self);
int   rand(void);
void* memset(void* dst, int value, unsigned long size);
int   SendMsg(int to, int msg, void* data);
}

// C++-linkage SDK helpers (mangled names in symbols.txt)
void  VFree(void* p);
void* VAlloc(int size);
int   ConditionUpdateBit(void* obj, int bit, int value);
int   AssetFind(u32 id, u32 flags);
int   FxFind(const char* name);

extern "C" void fn_800C9B68(void);
extern "C" void fn_800C9C68(Actor* self);
extern "C" void fn_800C748C(void);

// defined later in this file
u32 FshSveQueryResponse(MsgArg* args);
u32 FshSveFrameUpdate(MsgArg* args);
u32 FshSveNextState(Actor* self);
u32 FshSveUpdatePossession(Actor* self);
u32 FshSveCheckTargetId(void* self, u32 targetId);

// message handler / query entry point
u32 fn_802BB1F4(int msg, MsgArg* args)
{
    Actor* self = (Actor*)args[0].p;
    BehData* bd;
    MsgArg send[3];
    GeistString s1;
    GeistString s2;
    GeistString s3;
    f32 limit;
    u32 hash;
    u8* p;
    s32 r;
    s32 state;

    if (msg == 0x40A)
        return FshSveQueryResponse(args);

    if (msg < 0x40A) {
        if (msg == 0x400) {
            args[1].i = OwnerId((Owner*)self);
            args[3].i = (lbl_804DACE0 < self->unk904);
            args[4].f = lbl_804DACE4;
            args[5].f = lbl_804DACE4;
            args[6].i = 3 - ((__cntlzw(fn_80049940()) >> 5) & 1);
            args[7].i = 0;
            return 1;
        }
        if (msg < 0x400) {
            if (msg == 0)
                return FshSveFrameUpdate(args);
            return 0;
        }
        if (msg == 0x408)
            return 0;
        if (msg > 0x408) {
            // 0x409: possession request
            r = fn_800C5654((u32)OwnerId((Owner*)self), args[3].i, args[2].i);
            if (r != 0)
                return 1;
            if (args[3].i != 0x272CAC)
                return 0;
            send[0].u = (u32)fn_800C5654((u32)self->unk38, 0xDC35206A, args[2].i);
            send[1].p = self->unk38;
            if (args[2].i != -1) {
                send[2].u = 0xAF699ED0;
                fn_800265D4(args[2].i, 0xC14, send);
            }
            return 1;
        }
        // 0x407: scale pulse
        bd = self->beh;
        if ((u32)args[2].i >> 24 != 2)
            return 1;
        if (args[5].u == 0x2EFC66CA) {
            bd->unk40 = lbl_804DACE0;
            self->unk91C = lbl_804DACE4;
        } else if (args[5].u < 0x2EFC66CA && args[5].u == 0xB9FC42F5 &&
                   bd->unk50 == 0) {
            args[3].i = (s32)(lbl_804D39C0 * (f32)self->unkC4->unk30);
        }
        if (args[5].u != 0x403548D0)
            bd->unk2C = bd->unk2C + (f32)args[3].i;
        limit = lbl_804D39C4 * (f32)self->unkC4->unk30;
        if (limit < bd->unk2C) {
            bd->unk2C = bd->unk2C - limit;
            bd->unk30 = 1;
        }
        bd->unk50 = 0;
        return 0;
    }

    if (msg == 0xC14) {
        bd = self->beh;
        if (args[3].u == 0x39CB0CB0) {
            if ((u32)args[2].i >> 24 == 0x13)
                bd->unk50 = 1;
            return 0;
        }
        if (args[3].u < 0x39CB0CB0 && args[3].u == 0x2EFC66CA) {
            bd->unk14 = lbl_804DACE0;
            self->unk91C = lbl_804DACE4;
            bd->unk18 = 1;
            return 0;
        }
        return 0;
    }

    if (msg < 0xC14) {
        if (msg == 0x41E) {
            if (args[1].u == 0xAD8576FA) {
                fn_8028C400(&s1, PTR_DAT_803C4B54[self->beh->state]);
                fn_8028C400(&s2, lbl_8034BDDD);
                fn_800225C8(&s3, &s1, &s2);
                fn_8028C188(&args[3], s3.unk04, s3.unk08);
                s3.data = lbl_80393D7C;
                fn_8028B9C0(&s3, 0);
                s2.data = lbl_80393D7C;
                fn_8028B9C0(&s2, 0);
                s1.data = lbl_80393D7C;
                fn_8028B9C0(&s1, 0);
                return 0;
            }
            if (args[1].u < 0xAD8576FA) {
                if (args[1].u == 0x832DF50A) {
                    args[3].i = lbl_804D3970;
                    return 1;
                }
            } else if (args[1].u == 0x2EAA5F62) {
                args[3].i = (lbl_804DACE4 < self->unk91C);
                return 1;
            }
            return 0;
        }
        if (msg > 0x41C) {
            r = fn_80019F88(args, self);
            return r != 0;
        }
        return 0;
    }

    if (msg == 0x7102) {
        bd = self->beh;
        if (bd != 0) {
            if (args[1].i < 0x58) {
                args[4].i = 0;
                return 1;
            }
            if (args[1].i < 4)
                return 1;
            {
                u32* value = (u32*)args[2].p;
                args[3].i += 4;
                args[1].i -= 4;
                args[2].p = (u8*)args[2].p + 4;
                hash = 0;
                for (p = (u8*)lbl_8034BDD4; *p != 0; p++) {
                    u32 c = (*p == 0xFF) ? 0xFFFFFFFF : (u8)__upper_map[*p];
                    hash = hash * 0x1F + (c & 0xFF);
                }
                if (*value != hash)
                    return 1;
            }
        }
        return 1;
    }

    // 0x7103
    bd = self->beh;
    if (bd != 0) {
        state = bd->state;
        args[4].i = state;
    }
    return 1;
}

// per-frame behaviour update
u32 fn_802BB8E0(Actor* self)
{
    BehData* bd;
    f32 delta;
    f32 scale;
    f32 value;
    s32 r;
    s32 next;

    if (self == 0)
        return 0;

    bd = self->beh;
    lbl_804D3988 = (s32)((f32)(lbl_804D398C - lbl_804D3990) *
                             (self->unk904 / (f32)self->unkC4->unk30) +
                         (f32)lbl_804D3990);
    fn_800C99E8(self, lbl_804D39B8 & 0xFF, lbl_804D39B4 & 0xFF, 1,
                (s32)(lbl_804DACF0 * lbl_804D397C));

    // growth per frame, relative to the current scale
    delta = self->unk138 / self->unk91C;
    delta = delta / fn_802A2080();

    if (bd->unk14 < lbl_804D3984) {
        bd->unk40 = lbl_804DACE0;
        bd->unk14 = bd->unk14 + delta;
    }

    // normalise the current scale against the shrink/grow limits
    value = (fn_802A2080() < lbl_804DACE4)
                ? (self->unk91C - lbl_804DACE4) / (lbl_804D397C - lbl_804DACE4)
                : (self->unk91C - lbl_804DACE4) / (lbl_804D3980 - lbl_804DACE4);

    if ((bd->flags3C & 2) == 0) {
        if (bd->unk40 < lbl_804DACE4) {
            bd->unk40 = bd->unk40 + self->unk138 / (f32)lbl_804D3988;
            if (bd->unk40 > lbl_804DACE4)
                bd->unk40 = lbl_804DACE4;
        }
    } else {
        bd->unk40 = bd->unk40 - self->unk138 / (f32)lbl_804D3988;
        if (bd->unk40 <= lbl_804DACE0 && bd->unk48 == 0) {
            if ((bd->flags3C & 2) != 0) {
                bd->flags3C &= ~2u;
                if (lbl_804D39BC != 0)
                    self->flags4C |= 0x1000;
                bd->unk44 = fn_8016F120(lbl_804D3978, self->unk24);
            }
        }
    }

    if ((bd->flags3C & 2) == 0) {
        if (lbl_804DACE0 < value) {
            value = value - delta / (f32)lbl_804D3998;
            if (value < lbl_804DACE0)
                value = lbl_804DACE0;
        }
    } else {
        if (value < lbl_804DACE4) {
            value = value + delta / (f32)lbl_804D3994;
            if (value > lbl_804DACE4)
                value = lbl_804DACE4;
        }
    }
    if (value < lbl_804DACE0)
        value = lbl_804DACE0;
    else if (value > lbl_804DACE4)
        value = lbl_804DACE4;

    scale = fn_802A2080();
    if (scale < lbl_804DACE4)
        self->unk91C = value * (lbl_804D397C - lbl_804DACE4) + lbl_804DACE4;
    else
        self->unk91C = value * (lbl_804D3980 - lbl_804DACE4) + lbl_804DACE4;

    r = fn_800A7AC4(self->unk270, 9);
    if (r == 0) {
        r = fn_800A7AC4(self->unk270, 10);
        if (r == 0) {
            r = fn_800A7AC4(self->unk270, 11);
            if (r == 0) {
                r = fn_800BEF9C(self->unk2C4);
                if (r < self->unk268)
                    self->unk268 = fn_800BEF9C(self->unk2C4);
                r = fn_800BAF40(self);
                if (r == -1) {
                    self->unk288 = 0;
                    self->unk26C = 0;
                }
                r = ((s32 (*)(Actor*))PTR_FUN_803C4B50[bd->state])(self);
                fn_80162658(self, self->unk894);
                if (r != 0) {
                    next = FshSveNextState(self);
                    if (next != bd->state) {
                        if (((s32 (*)(Actor*))PTR_FUN_803C4B4C[bd->state])(self) != 0) {
                            ((void (*)(Actor*))PTR_FUN_803C4B48[next])(self);
                            fn_800A7A54(self->unk270, 0x40, 0);
                            fn_800A7A54(self->unk270, 0x41, 0);
                            fn_800A7A54(self->unk270, 0x43, 0);
                            fn_800A7A54(self->unk270, 0x42, 0);
                        }
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}

// Releases the per-instance behaviour data; the actor drops back to its
// "no behaviour" state.
void FshSveReleaseBehaviour(Actor* self)
{
    fn_800C9B68();
    VFree(self->beh);
    self->beh = 0;
    self->unk104 = 0;
}

// Installs the behaviour update callback and allocates/fills the 0x54-byte
// per-instance behaviour data. Fx handles and the shared asset are looked up
// once, on the first creation.
void FshSveCreateBehaviour(Actor* self)
{
    BehData* b;

    self->unk104 = (void*)fn_802BB8E0;
    b = (BehData*)VAlloc(0x54);
    if (b == 0) {
        self->beh = 0;
    } else {
        self->beh = b;
        self->unkFC = (void*)FshSveUpdatePossession;
        self->unk604 = (void*)FshSveCheckTargetId;
        self->unk91C = lbl_804DACE4;
        b->state = 0;
        b->unk04 = lbl_804DACE0;
        b->unk08 = 0;
        b->unk0C = lbl_804DACE0;
        b->unk10 = lbl_804DACE0;
        b->flags3C = 0;
        b->unk40 = lbl_804DACE0;
        b->unk44 = 0xFFFFFFFF;
        b->unk14 = lbl_804DACF4;
        b->unk24 = (f32)(u32)lbl_804D3988;
        b->unk48 = 0;
        b->unk4C = 0;
        b->unk18 = 0;
        b->unk28 = -1;
        b->unk50 = 0;
        b->unk2C = lbl_804DACE0;
        b->unk30 = 0;
        b->unk34 = lbl_804DACE0;
        b->unk38 = -1;
        ConditionUpdateBit((u8*)self + 0x270, 6, 1);
        ConditionUpdateBit((u8*)self + 0x270, 0x22, 1);
        lbl_804D3970 = AssetFind(0xF1754BF2, 0x21C5E0);
        lbl_804D3974 = FxFind("dashin");
        lbl_804D3978 = FxFind("dashout");
        fn_800C9C68(self);
    }
}

// Per-frame follow-state update. Applies the per-frame shrink to the follow
// offset, verifies the possessed target, maintains the follow condition bits
// and runs the 0x400 "acquire target" handshake.
int FshSveFollowStateUpdate(Actor* self)
{
    BehData* bd;
    Actor* target;
    BehData* b;
    s32 id;
    s32 r;
    u32 buf[8];

    if (self == 0)
        return 0;

    bd = self->beh;
    bd->unk34 = bd->unk34 - self->unk138 / self->unk91C;
    if (bd->unk34 <= lbl_804DACE0)
        return 1;

    target = (Actor*)FollowGetTargetObj((u8*)self + 0x270);
    if (target == 0)
        return 1;
    if (self->unk604 != 0 &&
        ((ActorCheckFn)self->unk604)(self, target->unk38) == 0)
        return 1;

    if ((self->beh->flags3C & 2) == 0 &&
        HasCondition((u8*)self + 0x270, 9) == 0 &&
        HasCondition((u8*)self + 0x270, 10) == 0) {
        bd->unk48 = 1;
        b = self->beh;
        if ((b->flags3C & 2) == 0) {
            b->flags3C |= 2;
            if (lbl_804D39BC != 0)
                self->flags4C &= ~0x80000u;
            b->unk44 = fn_8016F120(lbl_804D3974, (u8*)self + 0x24);
        }
    }

    target = (Actor*)FollowGetTargetObj((u8*)self + 0x270);
    r = FollowLineCheckActorDist(self, target, lbl_804DACF8);
    if (r == 0 || fn_8029B3B0(self, 0x73B1E248) == 0) {
        fn_80162610(self, (u8*)self + 0x894, 0);
        ConditionUpdateBit((u8*)self + 0x270, 0, 0);
    } else {
        ConditionUpdateBit((u8*)self + 0x270, 0, 1);
        ConditionUpdateBit((u8*)self + 0x270, 3, 1);
        ConditionUpdateBit((u8*)self + 0x270, 4, 1);
        if (fn_80162940((u8*)self + 0x894, 2) != 0 &&
            fn_80162940((u8*)self + 0x894, 8) != 0) {
            if (self->unk5B8 > (f32)rand() / lbl_804DACFC) {
                fn_80162610(self, (u8*)self + 0x894, 2);
                goto done;
            }
        }
        if (fn_80162940((u8*)self + 0x894, 1) != 0 &&
            fn_80162940((u8*)self + 0x894, 4) != 0)
            fn_80162610(self, (u8*)self + 0x894, 1);
    }

done:
    if (fn_800BAF40(self) == -1) {
        if (target == 0)
            return 1;
        if (self == 0 || target == 0)
            id = -1;
        else
            id = fn_800B9104(0x4CCF05A5, 0, target->unk38, 0);
        if (id == -1)
            return 1;
        memset(buf, 0, 0x20);
        if (SendMsg(id, 0x400, buf) == 0 || buf[0] == 0)
            return 1;
        if (id != bd->unk38) {
            fn_800BC56C(self);
            self->unk288 = 3;
            bd->unk38 = id;
            if (fn_800BAF40(self) == -1)
                return 1;
        }
    }
    return 0;
}

// Starts a follow: installs the offset-update callback, resets the follow
// state fields and clears the follow condition bit.
int FshSveFollowBegin(Actor* self)
{
    BehData* bd;

    if (self == 0)
        return 0;

    bd = self->beh;
    if (bd == 0)
        return 0;

    self->unk10C = (void*)FollowOffsetUpdate;
    self->unk288 = 0;
    self->unk21C = 0;
    self->unk220 = 0;
    fn_800BBCA8();
    bd->unk38 = -1;
    ConditionUpdateBit((u8*)self + 0x270, 0x16, 0);
    fn_8029B070(self, 0x73B1E248);
    return 1;
}

// ---- possession/follow helpers -------------------------------------------

struct Vec3 {
    f32 x; // 0x00
    f32 y; // 0x04
    f32 z; // 0x08
};

// 4x3 transform written by ActorGetMatrix; the translation occupies the last
// three floats (0x24).
struct Mtx34 {
    f32 m[9]; // 0x00
    f32 tx;   // 0x24
    f32 ty;   // 0x28
    f32 tz;   // 0x2c
};

extern Vec3 lbl_8034BD80;

extern "C" {
extern f32 lbl_804D399C;
extern f32 lbl_804D39A0;
extern f32 lbl_804D39A4;
extern f32 lbl_804D39A8;
extern f32 lbl_804D39AC;
extern f32 lbl_804D39B0;
extern f32 lbl_804D39C8;
extern f32 lbl_804D39CC;
extern f32 lbl_804DAD00;
extern f32 lbl_804DAD04;
extern f32 lbl_804DAD08;

extern s32  fn_800B26B8(Actor* self);
extern s32  fn_800B1EF4(Actor* self, s32 mode);
extern u32  fn_800B1ED4(Actor* self, s32 mode);
extern void fn_800B261C(Actor* self, s32 mode, void* target);
extern s32  fn_800B22F8(Actor* self, s32 mode);
extern void fn_800B2418(f32 value, Actor* self, s32 mode);
extern void fn_800B1F20(Actor* self, s32 mode);
extern void fn_800B1AB8(f32 value, Actor* self, s32 mode, s32 flag);
extern s32  fn_800B2454(Actor* self, s32 mode);
extern void fn_800BB8B8(Actor* self, void* out);
extern void fn_800A025C(void* out, void* mtx, s32 flag);
extern void fn_8016D440(void* out, void* vec);
extern void fn_8016D4EC(void* out, void* euler);
extern void fn_800E1098(void* out, void* in, f32 angle);
extern s32  fn_800BF2D0(void* vec);
extern void fn_800BC5A8(Actor* self, void* vec);
}

extern void FollowSetTargetObj(void* obj, void* target);
extern f32  ActorAimAngleToPos(void* target, Vec3* pos);
extern void ActorGetMatrix(Mtx34* mtx, Actor* actor);
extern void EulerNormalize(Vec3* out, Vec3* in);
extern "C" double fabs(double x);

// Starts the possession state. The state is switched to 8, the follow-distance
// scale is derived from the actor's current scale, the follow condition bits
// are raised, and the follow target's owner is asked to acknowledge the
// possession with message 0x400.
int FshSvePossessionBegin(Actor* self)
{
    BehData* bd;
    void* target;
    s32 id;
    s32 r;
    u32 buf[8];

    if (self == 0)
        return 0;

    bd = self->beh;
    bd->state = 8;
    bd->unk30 = 0;
    bd->unk34 = (lbl_804D39CC - lbl_804D39C8) *
                    (lbl_804DACE4 - self->unk904 / (f32)self->unkC4->unk30) +
                lbl_804D39C8;
    ConditionUpdateBit((u8*)self + 0x270, 4, 1);
    ConditionUpdateBit((u8*)self + 0x270, 0x16, 1);
    ConditionUpdateBit((u8*)self + 0x270, 0x17, 1);

    target = FollowGetTargetObj((u8*)self + 0x270);
    if (target != 0) {
        if (self == 0 || target == 0)
            id = -1;
        else
            id = fn_800B9104(0x4CCF05A5, 0, ((Actor*)target)->unk38, 0);
        if (id != -1) {
            memset(buf, 0, 0x20);
            r = SendMsg(id, 0x400, buf);
            if (r == 0 || buf[0] == 0)
                return 1;
            fn_800BC56C(self);
            self->unk288 = 3;
            bd->unk38 = id;
        }
    }
    return 1;
}

// Per-frame target/aim update. Keeps the follow target handshake in sync,
// drops the aim when the target leaves the allowed angle, and otherwise picks
// a random yaw offset, validates it against the world and commits the
// resulting position with fn_800BC5A8.
int FshSveFollowAimUpdate(Actor* self)
{
    BehData* bd;
    BehData* b;
    void* target;
    s32 mode;
    f32 angle;
    f32 spread;
    f32 turn;
    Vec3 dir;
    Vec3 euler;
    Vec3 aim;
    MsgArg args[5];
    Mtx34 mtx;

    if (self == 0)
        return 1;

    bd = self->beh;
    if (self->unk220 != 0)
        return 1;

    target = FollowGetTargetObj((u8*)self + 0x270);
    mode = fn_800B26B8(self);
    if (mode != 4) {
        if (fn_800B1EF4(self, mode) == 0) {
            if (fn_800B1ED4(self, mode) & 0x200) {
                bd->unk40 = lbl_804DACE4;
                bd->unk48 = 1;
                b = self->beh;
                if ((b->flags3C & 2) == 0) {
                    b->flags3C |= 2;
                    if (lbl_804D39BC != 0)
                        self->flags4C &= ~0x1000u;
                    b->unk44 = fn_8016F120(lbl_804D3974, (u8*)self + 0x24);
                }
            }
            fn_800B261C(self, mode, &target);
            fn_800B22F8(self, mode);
            if (target == 0)
                return 1;
            fn_800B2418(lbl_804DAD00, self, mode);
            fn_800B1F20(self, mode);
            fn_800B1AB8(lbl_804DAD04, self, mode, 1);
            FollowSetTargetObj((u8*)self + 0x270, target);
        } else {
            if (fn_800B2454(self, mode) != 0)
                return 1;
            fn_800B2418(lbl_804DAD00, self, mode);
        }
    }

    if (fn_800BAF40(self) == -1) {
        angle = ActorAimAngleToPos(target, (Vec3*)((u8*)self + 0x84));
        if (fabs(angle) > lbl_804D399C) {
            bd->unk40 = lbl_804DACE0;
            b = self->beh;
            if (b->flags3C & 2) {
                b->flags3C &= ~2u;
                if (lbl_804D39BC != 0)
                    self->flags4C |= 0x1000;
                b->unk44 = fn_8016F120(lbl_804D3978, (u8*)self + 0x24);
            }
            return 1;
        }
        bd->unk20 = lbl_804DACE0;
    }

    // Ask who currently possesses the follow target; the reply is a single
    // flag in the argument slot following the message hash.
    args[1].u = 0x2EAA5F62;
    args[2].u = 0;
    SendMsg((s32)((Actor*)target)->unk38, 0x41E, args);
    if (args[3].i != 0)
        return 1;

    bd->unk20 = bd->unk20 - self->unk138;
    if (bd->unk20 <= lbl_804DACE0) {
        bd->unk20 = lbl_804D39B0;
        fn_800BB8B8(self, &aim);
        angle = ActorAimAngleToPos(target, &aim);
        if (fabs(angle) < lbl_804D399C) {
            dir = lbl_8034BD80;
            dir.z = angle;
            ActorGetMatrix(&mtx, (Actor*)target);
            fn_800A025C(&aim, &mtx, 1);
            fn_8016D440(&euler, &aim);
            EulerNormalize(&dir, &dir);

            spread = (lbl_804D39A4 - lbl_804D39A0) *
                         ((f32)rand() / lbl_804DACFC) + lbl_804D39A0;
            if (bd->unk4C == 0) {
                if (dir.z < lbl_804DACE0)
                    spread = spread * lbl_804DAD04;
            } else {
                spread = spread * (f32)bd->unk4C;
            }
            turn = (lbl_804D39AC - lbl_804D39A8) *
                       ((f32)rand() / lbl_804DACFC) + lbl_804D39A8;
            euler.z = euler.z + spread + dir.z;
            fn_8016D4EC(&aim, &euler);
            fn_800E1098(&aim, &aim, turn);
            aim.x += mtx.tx;
            aim.y += mtx.ty;
            aim.z += mtx.tz;
            if (fn_800BF2D0(&aim) == -1) {
                if (bd->unk4C == 0) {
                    spread = spread * lbl_804DAD08;
                    euler.z = euler.z + spread;
                    fn_8016D4EC(&aim, &euler);
                    fn_800E1098(&aim, &aim, turn);
                    aim.x += mtx.tx;
                    aim.y += mtx.ty;
                    aim.z += mtx.tz;
                    if (fn_800BF2D0(&aim) != -1)
                        goto accepted;
                }
                b = self->beh;
                if (b->flags3C & 2) {
                    b->flags3C &= ~2u;
                    if (lbl_804D39BC != 0)
                        self->flags4C |= 0x1000;
                    b->unk44 = fn_8016F120(lbl_804D3978, (u8*)self + 0x24);
                }
                bd->unk40 = lbl_804DACE0;
                return 1;
            }
        accepted:
            if (spread < lbl_804DACE0)
                bd->unk4C = -1;
            else
                bd->unk4C = 1;
            fn_800BC5A8(self, &aim);
            self->unk288 = 3;
        }
    }
    return 0;
}

// State-entry check callback: this behaviour accepts entry unconditionally.
int FshSveCheckEnter(void)
{
    return 1;
}

// Prepares the follow state: switches to state 7, clears the force counter and
// the follow condition bits, re-acquires the follow target and resets the aim
// timer that drives the next attack.
int FshSvePrepareFollow(Actor* self)
{
    BehData* bd;
    void* target;
    s32 mode;

    if (self == 0)
        return 0;

    bd = self->beh;
    bd->state = 7;
    self->unk288 = 0;
    bd->unk4C = 0;
    fn_800BBCA8();
    ConditionUpdateBit((u8*)self + 0x270, 4, 1);
    ConditionUpdateBit((u8*)self + 0x270, 3, 0);
    mode = fn_800B26B8(self);
    if (mode == 4)
        return 0;
    fn_800B261C(self, mode, &target);
    if (target == 0)
        return 0;
    FollowSetTargetObj((u8*)self + 0x270, target);
    bd->unk20 = lbl_804DACE0;
    return 1;
}

// Follow-state update while the follow target is being validated: keeps the
// follow conditions in sync, republishes the follow FX when the target is
// grabbed and drops the follow when the target is out of range.
int FshSveFollowTargetUpdate(Actor* self)
{
    BehData* bd;
    BehData* b;
    void* target;
    void* cur;
    s32 mode;
    s32 ok;

    if (self == 0)
        return 0;

    bd = self->beh;
    target = FollowGetTargetObj((u8*)self + 0x270);
    mode = fn_800B26B8(self);
    if (target == 0) {
        ConditionUpdateBit((u8*)self + 0x270, 4, 0);
        return 1;
    }

    if (mode < 2) {
        fn_800B22F8(self, mode);
        if (fn_800B1EF4(self, mode) == 0) {
            fn_800B261C(self, mode, &cur);
            if (fn_800B1ED4(self, mode) & 0x200) {
                bd->unk40 = lbl_804DACE4;
                bd->unk48 = 1;
                b = self->beh;
                if ((b->flags3C & 2) == 0) {
                    b->flags3C |= 2;
                    if (lbl_804D39BC != 0)
                        self->flags4C &= ~0x1000u;
                    b->unk44 = fn_8016F120(lbl_804D3974, (u8*)self + 0x24);
                }
            }
            if (target != 0 && target != cur) {
                fn_800B1F20(self, mode);
                fn_800B2418(lbl_804DAD00, self, mode);
                fn_800B1AB8(lbl_804DAD04, self, mode, 1);
            }
        }
    } else {
        if (fn_800B1EF4(self, mode) == 0)
            return 1;
    }

    if ((self->beh->flags3C & 2) == 0 &&
        HasCondition((u8*)self + 0x270, 9) == 0 &&
        HasCondition((u8*)self + 0x270, 10) == 0)
        ok = 1;
    else
        ok = 0;

    if (ok) {
        bd->unk48 = 0;
        b = self->beh;
        if ((b->flags3C & 2) == 0) {
            b->flags3C |= 2;
            if (lbl_804D39BC != 0)
                self->flags4C &= ~0x1000u;
            b->unk44 = fn_8016F120(lbl_804D3974, (u8*)self + 0x24);
        }
    }

    if (FollowLineCheckActorDist(self, target, lbl_804DACE4) != 0)
        return 1;

    if (fn_800BAF40(self) == -1) {
        fn_800BC56C(self, target);
        self->unk288 = 3;
    }
    return 0;
}

// Cancels the follow: clears the follow state and reports success.
int FshSveFollowCancel(void)
{
    fn_800BBCA8();
    return 1;
}

// Switches the behaviour to state 6 and re-targets the current follow target:
// the follow condition bits are raised, the possession target is committed
// with fn_800BC56C and the possession state is set to 3. Returns 0 when the
// actor follows nothing or the mode is 4.
int FshSveRetargetState(Actor* self)
{
    BehData* bd;
    void* target;
    s32 mode;

    if (self == 0)
        return 0;

    bd = self->beh;
    bd->state = 6;
    ConditionUpdateBit((u8*)self + 0x270, 3, 1);
    mode = fn_800B26B8(self);
    if (mode == 4)
        return 0;
    fn_800B261C(self, mode, &target);
    if (target == 0)
        return 0;
    FollowSetTargetObj((u8*)self + 0x270, target);
    fn_800BC56C(self, target);
    self->unk288 = 3;
    return 1;
}

// ---- main per-frame update -----------------------------------------------

struct ZoneList;

extern "C" {
extern f32 lbl_804DAD0C;
extern f32 lbl_804DAD10;
extern f32 lbl_804DAD14;
extern f32 lbl_804DAD18;
extern f32 lbl_804DAD1C;
extern f32 lbl_804DAD20;

void TalkPlay(s32 owner, s32 id);
f32  OwnerValue(Owner* owner, void* target);
void ZoneWalkNodes(ZoneList* list, u32 (*cb)(u32, f32, s32), s32 a, s32 b);
s32  fn_800BB218(f32 scale, Actor* self, void* unk2c4, Vec3* pos, Vec3* target,
                 Actor* owner, f32* angle, s32* out1, s32* out2);
f32  fn_800E1020(Vec3* vec);
void fn_800BAFF8(f32 a, f32 b, Actor* self, Vec3* dir, s32 c, s32 d, Vec3* out);
void fn_800CCA88(Actor* self, void* data);
}

u32 FshSveZoneCheckOwner(void* node, Owner* owner, s32* out);

// True while the actor may take possession control: the behaviour bit is clear
// and neither follow condition 9 nor 10 is raised. Inlined at every use.
static inline int FshSveCanPossess(Actor* self)
{
    BehData* b = self->beh;

    if ((b->flags3C & 2) != 0)
        return 0;
    if (HasCondition((u8*)self + 0x270, 9) != 0)
        return 0;
    if (HasCondition((u8*)self + 0x270, 10) != 0)
        return 0;
    return 1;
}

// Raises the possession bit on the behaviour data, drops the follow exclusion
// flag and re-registers the owner id against the first owner slot.
static inline void FshSvePossessOn(Actor* self)
{
    BehData* b = self->beh;

    if ((b->flags3C & 2) == 0) {
        b->flags3C |= 2;
        if (lbl_804D39BC != 0)
            self->flags4C &= ~0x1000u;
        b->unk44 = fn_8016F120(lbl_804D3974, (u8*)self + 0x24);
    }
}

// Clears the possession bit, raises the follow exclusion flag and re-registers
// the owner id against the second owner slot.
static inline void FshSvePossessOff(Actor* self)
{
    BehData* b = self->beh;

    if ((b->flags3C & 2) != 0) {
        b->flags3C &= ~2u;
        if (lbl_804D39BC != 0)
            self->flags4C |= 0x1000u;
        b->unk44 = fn_8016F120(lbl_804D3978, (u8*)self + 0x24);
    }
}

// Main per-frame update of the possessed actor: keeps the follow handshake in
// sync, runs the 0x400/0x41E possession handshake, starts and stops the
// "being possessed" behaviour bit, drives the in/out movement towards the
// target and feeds the idle/approach timers with random values. Returns 1
// when the state machine must not fall through this frame, 0 otherwise.
int FshSveMainUpdate(Actor* self)
{
    BehData* bd;
    BehData* b;
    void* target;
    void* reply;
    s32 mode;
    f32 f31;
    f32 f30;
    f32 dist;
    f32 angle;
    s32 out1;
    s32 out2;
    Vec3 dir;
    Vec3 pos;
    u32 buf[8];

    if (self == 0)
        return 0;

    bd = self->beh;
    mode = fn_800B26B8(self);
    if (mode != 4) {
        fn_800B261C(self, mode, &target);
        fn_800B22F8(self, mode);
        if (fn_800B1EF4(self, mode) == 0) {
            if (fn_800B1ED4(self, mode) & 0x200) {
                bd->unk40 = lbl_804DACE4;
                bd->unk48 = 1;
                FshSvePossessOn(self);
            }
            if (target != 0 && fn_800B22F8(self, mode) < 2) {
                fn_800B2418(lbl_804DAD00, self, mode);
                fn_800B1F20(self, mode);
                fn_800B1AB8(lbl_804DAD04, self, mode, 1);
                FollowSetTargetObj((u8*)self + 0x270, target);
            }
        } else {
            if (fn_800B2454(self, mode) != 0)
                return 1;
            fn_800B2418(lbl_804DAD00, self, mode);
        }
        if (FollowGetTargetObj((u8*)self + 0x270) != target)
            FollowSetTargetObj((u8*)self + 0x270, target);
    }

    target = FollowGetTargetObj((u8*)self + 0x270);
    if (target == 0 ||
        (self->unk604 != 0 &&
         ((ActorCheckFn)self->unk604)(self, ((Actor*)target)->unk38) == 0)) {
        RangeResetPending((u8*)self + 0x270);
        return 1;
    }

    SendMsg((int)((Actor*)target)->unk38, 0x400, buf);
    if ((buf[6] & 2) && self != 0) {
        b = self->beh;
        b->unk04 = b->unk04 - self->unk138;
        if (b->unk04 <= lbl_804DACE0) {
            s32 died = 0;

            ZoneWalkNodes((ZoneList*)((u8*)self + 0x658),
                          (u32 (*)(u32, f32, s32))FshSveZoneCheckOwner,
                          (s32)&died, 0);
            if (died == 0)
                TalkPlay(OwnerId((Owner*)self), 5);
            else
                TalkPlay(OwnerId((Owner*)self), 6);
            b->unk04 = lbl_804DACE0;
        }
    }

    dist = OwnerValue((Owner*)self, target);
    if ((f32)self->unkC4->unkC8 < dist)
        return 1;
    if (FollowLineCheckActorDist(self, target, lbl_804DACF8) == 0)
        return 1;

    if ((buf[6] & 2) == 0) {
        f31 = lbl_804DACE4;
        f30 = lbl_804DAD04;
        if (FshSveCanPossess(self)) {
            bd->unk48 = 1;
            FshSvePossessOn(self);
        }
    } else {
        // message 0x41E: ask the possession target whether it accepts the link
        struct Msg41E {
            u32 unk00;       // 0x00
            u32 hash;        // 0x04
            u32 unk08;       // 0x08
            GeistString str; // 0x0c
        } msg;

        msg.str.data = 0;
        msg.hash = 0x2EAA5F62;
        msg.unk08 = 0;
        SendMsg((int)((Actor*)target)->unk38, 0x41E, &msg);
        reply = msg.str.data;
        msg.str.data = lbl_80393D7C;
        fn_8028B9C0(&msg.str, 0);

        if (reply == 0) {
            u32 possessed = bd->flags3C & 2;

            if (possessed == 0 || bd->unk48 == 0) {
                if (possessed != 0 || bd->unk30 != 0)
                    return 1;
            } else {
                bd->unk40 = lbl_804DACE0;
                FshSvePossessOff(self);
            }
            f31 = lbl_804DACE4;
            f30 = lbl_804DAD04;
        } else if ((bd->flags3C & 2) == 0) {
            if (FshSveCanPossess(self)) {
                bd->unk48 = 1;
                FshSvePossessOn(self);
            }
        } else {
            f31 = lbl_804DACE4;
            f30 = lbl_804DAD04;
        }
    }

    if (bd->unk40 == lbl_804DACE4) {
        if (FshSveCanPossess(self)) {
            bd->unk48 = 0;
            FshSvePossessOn(self);
        }
    }

    if (f31 <= lbl_804DACE0)
        fn_8029B070(self, 0x73B1E248);
    if (f30 <= lbl_804DACE0)
        fn_8029B070(self, 0x86A69297);

    bd->unk1C = bd->unk1C - self->unk138;

    if (fn_80162940(self->unk894, 2) != 0 &&
        fn_80162940(self->unk894, 8) != 0 &&
        (f32)rand() / lbl_804DACFC <= f30 &&
        fn_8029B3B0(self, 0x86A69297) != 0) {
        angle = ActorAimAngleToPos(target, (Vec3*)((u8*)self + 0x84));
        if (fabs(angle) < lbl_804DAD0C ||
            (bd->unk1C = bd->unk1C - self->unk138) <= lbl_804DACE0) {
            if (fn_8029B3B0(self, 0x86A69297) != 0) {
                bd->unk1C = lbl_804DAD10;
                fn_80162610(self, self->unk894, 2);
            }
        }
    } else if (fn_80162940(self->unk894, 1) != 0 &&
               fn_80162940(self->unk894, 4) != 0 &&
               (f32)rand() / lbl_804DACFC <= f31 &&
               fn_8029B3B0(self, 0x73B1E248) != 0) {
        fn_80162610(self, self->unk894, 1);
    }

    if (fn_80162940(self->unk894, 0x800) != 0) {
        ConditionUpdateBit((u8*)self + 0x270, 0x1D, 1);
        fn_800BC56C(self, target);
        self->unk288 = 3;
    } else if (fn_80162940(self->unk894, 0x400) != 0) {
        // move to the possession point in front of the target
        dir.x = (f32)*(s32*)((u8*)target + 0x24);
        dir.y = (f32)*(s32*)((u8*)target + 0x28);
        dir.z = (f32)*(s32*)((u8*)target + 0x2C);
        dir.x = *(f32*)((u8*)self + 0x84) - dir.x;
        dir.y = *(f32*)((u8*)self + 0x88) - dir.y;
        dir.z = *(f32*)((u8*)self + 0x8C) - dir.z;
        fn_800E1098(&dir, &dir, lbl_804DAD14);
        pos.x = dir.x + *(f32*)((u8*)self + 0x84);
        pos.y = dir.y + *(f32*)((u8*)self + 0x88);
        pos.z = dir.z + *(f32*)((u8*)self + 0x8C);
        if (fn_800BB218(*(f32*)((u8*)self + 0x900), self, self->unk2C4,
                        (Vec3*)((u8*)self + 0x84), &pos, self, &angle, &out1,
                        &out2) != 0) {
            f32 t = fn_800E1020(&dir);

            fn_800BAFF8(angle, t * (lbl_804DACE4 - angle), self, &dir, out1,
                        out2, &pos);
        }
        if (fn_800BF2D0((u8*)self + 0x84) == fn_800BF2D0(&pos)) {
            ConditionUpdateBit((u8*)self + 0x270, 0x1D, 1);
            fn_800BC5A8(self, &pos);
            self->unk288 = 3;
        }
    } else if (fn_800BAF40(self) != -1) {
        dist = OwnerValue((Owner*)self, target);
        if (MemFlagIsSet((u8*)self + 0x270, 0x1D) != 0 &&
            dist > (f32)*(u16*)((u8*)self + 0x8B2) &&
            dist < (f32)*(u16*)((u8*)self + 0x8B4)) {
            ConditionUpdateBit((u8*)self + 0x270, 0x1D, 0);
            fn_800BBCA8();
        }
    } else {
        angle = ActorAimAngleToPos(target, (Vec3*)((u8*)self + 0x84));
        if (fabs(angle) < lbl_804DAD18) {
            bd->unk10 = bd->unk10 - self->unk138;
            if (bd->unk10 < lbl_804DACE0) {
                bd->unk08 = 0;
                fn_800CCA88(self, &bd->unk08);
                bd->unk10 = lbl_804DAD20 * ((f32)rand() / lbl_804DACFC) +
                            lbl_804DAD1C;
            }
        }
    }
    return 0;
}

// Drop whatever this actor is holding: clear the carry conditions, empty the
// attached object list and unregister the two status behaviours.
bool FshSveClearHeld(Actor* self)
{
    if (self != 0) {
        ConditionUpdateBit((u8*)self + 0x270, 0, 0);
        ConditionUpdateBit((u8*)self + 0x270, 4, 0);
        fn_80162610(self, (u8*)self + 0x894, 0);
        fn_8029B070(self, 0x73b1e248);
        fn_8029B070(self, 0x86a69297);
    }
    return self != 0;
}

// Begin the throw/drop state: fix the behaviour state at 5, seed the two
// timers, and hand the current follow target to the slot's physics mode.
int FshSveThrowBegin(Actor* self)
{
    BehData* bd;
    BehData* b;
    void* target;
    s32 mode;

    if (self == 0)
        return 0;

    bd = self->beh;
    bd->state = 5;
    bd->unk04 = lbl_804DACE0;
    bd->unk10 = lbl_804DAD20 * ((f32)rand() / lbl_804DACFC) + lbl_804DAD1C;
    bd->unk1C = lbl_804DAD10;
    ConditionUpdateBit((u8*)self + 0x270, 4, 1);
    ConditionUpdateBit((u8*)self + 0x270, 3, 1);
    self->unk288 = 0;

    mode = fn_800B26B8(self);
    if (mode < 4 && fn_800B1EF4(self, mode) == 0) {
        if (fn_800B1ED4(self, mode) & 0x200) {
            bd->unk40 = lbl_804DACE4;
            bd->unk48 = 1;
            b = self->beh;
            if ((b->flags3C & 2) == 0) {
                b->flags3C |= 2;
                if (lbl_804D39BC != 0)
                    self->flags4C &= ~0x1000u;
                b->unk44 = fn_8016F120(lbl_804D3974, (u8*)self + 0x24);
            }
        }
        fn_800B1F20(self, mode);
        fn_800B1AB8(lbl_804DAD04, self, mode, 1);
        fn_800B261C(self, mode, &target);
        if (target == 0)
            return 0;
        FollowSetTargetObj((u8*)self + 0x270, target);
    }
    return 1;
}

// Releases whatever the actor currently holds through the shared follow
// helper and reports that the transition succeeded.
int FshSveReleaseSignal(Actor* self)
{
    fn_800BBCA8();
    return 1;
}

// Puts the actor into its ranged state: state 3 on the actor, state 4 on the
// behaviour record, condition bit 3 set and the range tracker reset.
int FshSveEnterRangedState(Actor* self)
{
    BehData* bd;

    if (self == 0)
        return 0;

    bd = self->beh;
    self->unk288 = 3;
    bd->state = 4;
    ConditionUpdateBit((u8*)self + 0x270, 3, 1);
    RangeResetPending((u8*)self + 0x270);
    return 1;
}

// State-leave check: releases the follow helpers and reports success.
bool FshSveCheckLeave(void)
{
    fn_800BBCA8();
    return true;
}

// Puts the actor into its quiet range state: actor status 3, behaviour state 3,
// condition bit 3 raised and the range tracker reset.
bool FshSveEnterIdleState(void* p)
{
    if (p == 0)
        return false;
    *(s32*)((u8*)p + 0x288) = 3;
    *(*(s32**)((u8*)p + 0x130)) = 3;
    ConditionUpdateBit((u8*)p + 0x270, 3, 1);
    RangeResetPending((u8*)p + 0x270);
    return true;
}

// Follow/range helpers used by the update below (C++ linkage, mangled in
// symbols.txt).
int  RangeIsIdle(void* obj);
void FollowSetTargetVec(void* obj, Vec3* vec);
void OwnerSlotGetPosition(s32 self, s32 mode, f32* out);
int  FollowLineCheckSweep(void* obj, f32* out);
f32  OwnerValue(Owner* owner, Vec3* target);

// Per-frame follow update of the possessed actor. Keeps the possession handshake
// in sync, steers the follow range towards the owner slot position, and pushes
// the actor towards state 4 when it gets close enough. Returns 1 when the state
// machine must not fall through this frame, 0 otherwise.
int FshSveFollowUpdate(Actor* self)
{
    BehData* bd;
    s32 mode;
    s32 r;
    s32 range;
    Vec3 pos;

    if (self == 0)
        return 0;

    bd = self->beh;
    mode = fn_800B26B8(self);
    if (mode == 4) {
        self->unk268 = 4;
        return 1;
    }

    if (fn_800B1EF4(self, mode) == 0 || fn_800B2454(self, mode) == 0) {
        if (fn_800B1ED4(self, mode) & 0x200) {
            bd->unk40 = lbl_804DACE4;
            bd->unk48 = 1;
            FshSvePossessOn(self);
        }

        if (RangeIsIdle((u8*)self + 0x270) != 0) {
            fn_800B1F20(self, mode);
            OwnerSlotGetPosition((s32)self, mode, &pos.x);
            FollowSetTargetVec((u8*)self + 0x270, &pos);
            fn_800BC5A8(self, &pos);
        } else {
            if (fn_800B1EF4(self, mode) == 0) {
                RangeResetPending((u8*)self + 0x270);
                self->unk268 = fn_800B22F8(self, mode);
                return 1;
            }
            r = fn_800B22F8(self, mode);
            if (r < 2) {
                self->unk268 = r;
                return 1;
            }
        }
    }

    r = fn_800BAF40(self);
    if (r == -1) {
        self->unk288 = 0;
    } else {
        self->unk288 = (bd->state == 2) ? 2 : 3;
        if (FollowLineCheckSweep(self, (f32*)((u8*)self + 0x2b4)) != 0) {
            range = 8000;
            if (self->unkC4->unkC8 < range)
                range = self->unkC4->unkC8;
            if (OwnerValue((Owner*)self, (Vec3*)((u8*)self + 0x2b4)) <=
                (f32)range) {
                self->unk268 = 4;
                fn_800B2418(lbl_804DACE4, self, mode);
                return 1;
            }
        }
        fn_800B2418(lbl_804DAD00, self, mode);
    }
    return 0;
}

// Clears the follow registration and reports success; used as a small state
// handler.
int FshSveFollowExit(void)
{
    fn_800BBCA8();
    return 1;
}

// State-entry handler for the grab state: actor status 2, behaviour state 2,
// condition bit 3 raised and the range tracker reset.
bool FshSveEnterGrabState(void* p)
{
    Actor* self = (Actor*)p;
    BehData* bd;

    if (self == 0)
        return false;
    bd = self->beh;
    *(s32*)((u8*)self + 0x288) = 2;
    bd->state = 2;
    ConditionUpdateBit((u8*)self + 0x270, 3, 1);
    RangeResetPending((u8*)self + 0x270);
    return true;
}

// Per-frame possession follow process: refreshes the target slot from the
// current physics mode, raises the "being held" bit when the mode reports it,
// keeps the follow target and the possession state consistent and drops the
// possession when the target object goes away. Returns 1 when the state
// machine must not fall through this frame.
int FshSveFollowStateProcess(Actor* self)
{
    BehData* bd;
    s32 mode;

    if (self == 0)
        return 0;

    bd = self->beh;
    mode = fn_800B26B8(self);
    if (mode != 4) {
        self->unk268 = fn_800B22F8(self, mode);
        if (fn_800B1ED4(self, mode) & 0x200) {
            bd->unk40 = lbl_804DACE4;
            bd->unk48 = 1;
            bd = self->beh;
            if ((bd->flags3C & 2) == 0) {
                bd->flags3C |= 2;
                if (lbl_804D39BC != 0)
                    self->flags4C &= ~0x1000u;
                bd->unk44 = fn_8016F120(lbl_804D3974, (u8*)self + 0x24);
            }
        }
        if (MemFlagIsSet((u8*)self + 0x270, 5) == 0)
            return 1;
    }

    if (self->unk220 == 0 && MemFlagIsSet((u8*)self + 0x270, 5) == 0)
        return 1;

    if (self->unk220 == 2) {
        self->unk10C = (void*)fn_800C748C;
        if (FollowGetTargetObj((u8*)self + 0x270) != (void*)self->unk22C) {
            FollowSetTargetObj((u8*)self + 0x270, (void*)self->unk22C);
            if (self->unk21C != 0) {
                self->unk288 = self->unk21C;
                fn_800BC56C(self, (void*)self->unk22C);
            }
        }
        if (fn_800BAF40(self) == -1 && self->unk21C != 0) {
            FollowSetTargetObj((u8*)self + 0x270, 0);
            self->unk288 = 0;
        }
    } else {
        self->unk288 = 0;
    }
    return 0;
}

void ConditionSetBit(void* obj, int bit, int value); // ConditionSetBit__FPvii

// Reinstalls the follow offset-update callback and clears the follow-block
// condition bit; the "held" state (4) falls back to idle.
int FshSveFollowOffsetResume(Actor* self)
{
    if (self == 0)
        return 0;

    self->unk10C = (void*)FollowOffsetUpdate;
    if (self->unk220 == 4)
        self->unk220 = 0;
    if (HasCondition((u8*)self + 0x270, 0xd) != 0)
        ConditionSetBit((u8*)self + 0x270, 0xd, 0);
    return 1;
}

// State callback: marks the behaviour as entered (state 1).
int FshSveEnterReturnState(Actor* self)
{
    if (self == 0)
        return 0;

    self->beh->state = 1;
    return 1;
}

// Checks the current behaviour state: on the "detach" flag it clears the
// attach state and caches the follow target, then reports whether the
// actor is still busy (state 4 or a pending possession request).
int FshSveBehCheckDetach(Actor* self)
{
    BehData* beh;
    int state;
    int keep[3];

    if (self == 0) {
        return 0;
    }
    beh = self->beh;
    state = fn_800B26B8(self);
    if (state != 4) {
        fn_800B261C(self, state, keep);
        if (fn_800B1ED4(self, state) & 0x200) {
            beh->unk40 = lbl_804DACE4;
            beh->unk48 = 1;
            beh = self->beh;
            if (!(beh->flags3C & 2)) {
                beh->flags3C |= 2;
                if (lbl_804D39BC != 0) {
                    self->flags4C &= ~0x1000;
                }
                beh->unk44 = fn_8016F120(lbl_804D3974, self->unk24);
            }
        }
        if (keep[0] != 0) {
            self->unk268 = fn_800B22F8(self, state);
            return 1;
        }
    }
    return self->unk220 != 0;
}

// Query callback that always reports "yes".
int FshSveAlwaysTrue(void)
{
    return 1;
}

// Clears the follow target slot and the possession status of a valid actor;
// reports whether the pointer was non-null.
bool FshSveFollowTargetReset(Actor* self)
{
    if (self != 0) {
        self->beh->state = 0;
        self->unk288 = 0;
        fn_800BBCA8();
    }
    return self != 0;
}

// Picks the state that follows the current one. The follow-line flag at
// BehData::0x3c and the current follow mode decide whether the actor keeps
// following, asks the target with message 0x41E, or drops back to an idle
// state.
u32 FshSveNextState(Actor* self)
{
    struct Msg41E {
        u32 unk00;       // 0x00
        u32 hash;        // 0x04
        u32 unk08;       // 0x08
        GeistString str; // 0x0c
    } msg;
    BehData* bd;
    void* target;
    u32 id;
    u32 reply;
    u32 state;
    s32 mode;
    s32 limit;
    s32 count;
    f32 dist;

    if (self == 0)
        return 0;

    bd = self->beh;
    count = self->unk268;
    mode = fn_800B26B8(self);
    target = FollowGetTargetObj((u8*)self + 0x270);

    if ((bd->flags3C & 1) != 0 && target != 0) {
        if (FollowLineCheckActorDist(self, target, lbl_804DACE4) != 0) {
            limit = 8000;
            if (self->unkC4->unkC8 < limit)
                limit = self->unkC4->unkC8;
            dist = OwnerValue((Owner*)self, target);
            state = (dist <= (f32)limit) ? 2 : 3;
        } else {
            state = 3;
        }
    } else if (MemFlagIsSet((u8*)self + 0x270, 5) != 0) {
        state = 4;
    } else if (count > 1 && (bd->flags3C & 1) == 0) {
        state = (count <= 3) ? 1 : 0;
    } else if (mode == 4) {
        state = 0;
    } else {
        fn_800B261C(self, mode, &target);
        if (target == 0) {
            state = 1;
        } else if (FollowLineCheckActorDist(self, target, lbl_804DACE4) != 0) {
            limit = 8000;
            if (self->unkC4->unkC8 < limit)
                limit = self->unkC4->unkC8;
            dist = OwnerValue((Owner*)self, target);
            state = (dist <= (f32)limit) ? 2 : 3;
        } else {
            state = 3;
        }
    }

    if (state == 2) {
        id = *(u32*)((u8*)target + 0x38);
        msg.str.data = 0;
        msg.hash = 0x2EAA5F62;
        msg.unk08 = 0;
        SendMsg((int)id, 0x41E, &msg);
        reply = (u32)msg.str.data;
        msg.str.data = lbl_80393D7C;
        fn_8028B9C0(&msg.str, 0);
        if (bd->unk30 == 0) {
            if (reply != 0)
                state = 5;
            else
                state = (bd->flags3C & 2) ? 7 : 5;
        } else {
            state = 8;
        }
    } else if (state < 2) {
        if (state == 0)
            state = (self->unk220 != 0);
        else if (count == 1)
            state = 4;
        else
            state = (count == 2) ? 3 : 0;
    } else if (state == 4) {
        state = 1;
    } else if (state < 4) {
        state = 6;
    } else {
        state = 0;
    }
    return state;
}

// argument block for the 0x401 owner query: the callee fills in the float
struct FshPossessQuery {
    s32 owner;  // 0x00
    u32 unk04;  // 0x04
    u32 unk08;  // 0x08
    u32 unk0C;  // 0x0c
    f32 result; // 0x10
};

// object receiving the query, with its message entry at 0x30
struct FshOwnerObj {
    u8   unk00[0x30];                // 0x00
    void (*msg)(s32 id, void* args); // 0x30
};

// Zone-walk callback (0x802BE5C8): ask one candidate owner whether possession
// succeeded and report it through the walker's user word. Return 1 to continue.
u32 FshSveZoneCheckOwner(void* node, Owner* owner, s32* out)
{
    FshPossessQuery q;

    if (OwnerId(owner) != -1) {
        q.owner = (s32)owner;
        ((FshOwnerObj*)owner)->msg(0x401, &q);
        if (q.result > lbl_804DACE0)
            *out = 1;
    }
    return 1;
}

// Per-frame possession update (0x802BE640): refresh the owner slots, then let
// the first slot that requests possession take the behaviour over.
u32 FshSveUpdatePossession(Actor* self)
{
    BehData* bd = self->beh;
    s32 i;
    u8* slot;

    if (MemFlagIsSet((u8*)self + 0x270, 5) == 0) {
        fn_800B2E34(self, 0, 1);
        fn_800B3164(self, 0, 1);
        fn_800CECCC(self);
    } else {
        OwnerUpdateSlots((Owner*)self);
    }

    slot = (u8*)self;
    for (i = 0; i < 4; i++) {
        u32 flags = *(u32*)(slot + 0x144);

        if ((flags & 1) != 0) {
            if ((flags & 4) == 0) {
                if ((*(u32*)(slot + 0x158) & 0x200) != 0) {
                    BehData* b;

                    bd->unk40 = lbl_804DACE4;
                    bd->unk48 = 1;
                    b = self->beh;
                    if ((b->flags3C & 2) == 0) {
                        b->flags3C |= 2;
                        if (lbl_804D39BC != 0)
                            self->flags4C &= ~0x1000u;
                        b->unk44 = fn_8016F120(lbl_804D3974, self->unk24);
                    }
                }
            }
        }
        slot += 0x28;
    }
    return 1;
}

// Argument block for the 0x400 target query, filled by the queried actor.
struct FshTargetQuery {
    void* obj;                // 0x00
    u8    unk04[0x0C - 0x04]; // 0x04
    s32   alive;              // 0x0c
    u8    unk10[0x18 - 0x10]; // 0x10
    u32   flags;              // 0x18
    u32   unk1C;              // 0x1c
};

// Target-check callback stored at Actor::0x604 (0x802BE740): asks the target
// actor's 0x400 query for its state and decides whether the possession link is
// still valid for this target type (target id high byte).
u32 FshSveCheckTargetId(void*, u32 targetId)
{
    FshTargetQuery q;
    u32 type;

    memset(&q, 0, sizeof(q));
    if (SendMsg((int)targetId, 0x400, &q) == 0 || q.obj == 0)
        return 0;
    type = targetId >> 24;
    switch (type) {
    case 0x13:
        if (HasCondition((u8*)q.obj + 0x270, 0xb) != 0)
            return 0;
        if (q.alive == 0)
            return 0;
        if ((q.flags & 1) != 0)
            return 0;
        return (q.flags >> 1) & 1;
    case 2:
        if (q.alive == 0 || (q.flags & 2) == 0)
            return 0;
        return 1;
    default:
        return 1;
    }
}

extern "C" {
extern char lbl_8034BD8C[];
}

// Prefix of the player object used by the 0x40A query: the host table at
// 0x26c4 holds, per player index, the object the player currently possesses.
struct FshPlayerObj {
    u8    unk00[0x26C4]; // 0x00
    void* hosts[8];      // 0x26c4
    void* unk26E4;       // 0x26e4
};

// Argument block for the 0x400 actor query: the actor and its owner id.
struct FshActorQuery {
    Actor* actor; // 0x00
    s32    owner; // 0x04
};

// Argument block for the 0x40B scene query. The first word is left for the
// receiver.
struct FshSceneQuery {
    u32 unk00; // 0x00
    u32 unk04; // 0x04
    u32 hash;  // 0x08
    s32 unk0C; // 0x0c
};

int           PlayerInHost(Actor* self);
FshPlayerObj* GetPlayer(void);
int           ActorAnimState(int handle, u32 id);
int           ActorPlayAnim(int owner, u32 anim, int arg);

// Id of the object the player currently hosts.
static inline u32 FshSveHostId(FshPlayerObj* player)
{
    return *(u32*)player->hosts[PlayerInHost((Actor*)player)];
}

// Message 0x40A handler (0x802BE828): validates an animation query against the
// actor, its owner, the current player host and the follow conditions, then
// asks the actor to play the animation named "QUERY".
u32 FshSveQueryResponse(MsgArg* args)
{
    FshActorQuery q;
    FshSceneQuery r;
    Actor* actor = (Actor*)args[0].p;
    BehData* beh = actor->beh;
    FshPlayerObj* player;
    const char* name;
    const char* cp;
    u32 hash;
    s32 mode;

    q.actor = actor;
    q.owner = OwnerId((Owner*)actor);
    ((FshOwnerObj*)actor)->msg(0x400, &q);

    hash = args[2].u;
    name = lbl_8034BD8C + 0x62; // "QUERY"
    if (name != 0) {
        for (cp = name; *cp != 0; cp++) {
            char c = *name;
            u32 v = (c == '\xFF') ? 0xFFFFFFFFu : (u32)__upper_map[(u8)c];
            hash = hash * 0x1F + (v & 0xFF);
            name++;
        }
        hash = hash % 0xFFFFFFFFu;
    } else {
        hash = 0;
    }

    if (ActorPlayAnim(OwnerId((Owner*)actor), hash, args[1].i) != 0) {
        args[3].u = 0;
        return 1;
    }
    if (fn_80049940() != 0) {
        if (args[2].u == 0x7C44EC3D) {
            args[3].u = 0xA53F7E23;
            return 1;
        }
        return 0;
    }
    if (MemFlagIsSet((u8*)actor + 0x270, 0xb) != 0)
        return 1;
    if (beh->unk14 < lbl_804D3984)
        return 1;
    if (HasCondition((u8*)actor + 0x270, 10) != 0 ||
        HasCondition((u8*)actor + 0x270, 9) != 0)
        return 0;
    if (args[2].u == 0x3CA34BD6 &&
        MemFlagIsSet((u8*)actor + 0x270, 0x20) == 0) {
        args[3].u = 0x1379C466;
        return 1;
    }
    if (ActorAnimState((int)actor->unk38, 0xDC35206A) == 0)
        return 0;
    if (HasCondition((u8*)actor + 0x270, 0xb) != 0)
        return 0;
    player = GetPlayer();
    if (player == 0)
        return 0;
    if (player->unk26E4 != 0 &&
        (*(u32*)((u8*)player->unk26E4 + 0x38) >> 24) == 1)
        return 0;
    if (FshSveHostId(player) == 0x04624485 ||
        FshSveHostId(player) == 0x00013C05 ||
        FshSveHostId(player) == 0xE0AB9B4F ||
        FshSveHostId(player) == 0x0000FFF5)
        return 0;
    if (MemFlagIsSet((u8*)actor + 0x270, 0x1e) != 0)
        return 0;
    if (args[2].u == 0x7C44EC3D) {
        mode = fn_800B26B8(actor);
        if (mode != 4 && fn_800B22F8(actor, mode) < 2)
            return 0;
    }
    args[3].u = 0x272CAC;
    r.unk04 = 0;
    r.unk0C = -1;
    r.hash = args[3].u;
    SendMsg(args[1].i, 0x40B, &r);
    return 1;
}

// One 0x3c-byte entry of the actor's tint table; the three colour words are
// what the blend helper reads/writes.
struct FshTint {
    u8  unk00[0x10];        // 0x00
    u32 unk10;              // 0x10
    u32 unk14;              // 0x14
    u32 unk18;              // 0x18
    u8  unk1C[0x3C - 0x1C]; // 0x1c
};

// Tint table at 0x803C4A70; entries 6/7/8 are the fade colours.
struct FshTintSet {
    u8      unk000[0x168]; // 0x000
    FshTint unk168;        // 0x168
    FshTint unk1A4;        // 0x1a4
    FshTint unk1E0;        // 0x1e0
};

extern FshTintSet lbl_803C4A70;

extern s32 lbl_804D579C;

extern f32 lbl_804DAD30;
extern f32 lbl_804DAD34;
extern f32 lbl_804DAD38;
extern f32 lbl_804DAD3C;

extern "C" {
void* fn_800AFD48(void* actor, s32* out, s32 mode);
void* fn_8003A1F0(void* handle, u32 id, int shape);
void  fn_8003F15C(void* a, void* b, u32 c, void* d, void* e, f32 f);
void  fn_800C9970(Actor* actor);
int   ModelAnimate(int a, char* b, int c);
int   SkinBlendTint(void* skin, int a, int b, int c, f32 d);
void  HazardDestroy(int id);
int   HazardCreate(Actor* owner, int id, void* vec, int a, int b,
                   f32 c, f32 d, f32 e, f32 f);
void  fn_80158C3C(void* obj, u32 hash, s32 a, s32 b, s32 c, s32 d, s32 e);
f32   fn_80115F68(u32 id);
u32   fn_800BA84C(int a);
int   fn_800BA7FC(int a);
s32   fn_80049660(void* player);
s32   fn_800CC21C(void);
s32   Fn80080FA8(void);
}

// Per-frame update (message 0). Asks the follow target whether it is still
// linked, plays the release animation, blends the skin tint from the fade
// timer, keeps the possession hazard in sync and finally tests the actor's
// conditions before refreshing the possession host outline.
u32 FshSveFrameUpdate(MsgArg* args)
{
    Actor* actor = (Actor*)args[0].p;
    BehData* beh = actor->beh;
    MsgArg* pos = args + 4;
    FshTintSet* tints = &lbl_803C4A70;
    void* reply = 0;
    void* target = FollowGetTargetObj((u8*)actor + 0x270);

    if (target != 0) {
        // message 0x41E: ask the follow target for its link state
        struct Msg41E {
            u32 unk00;       // 0x00
            u32 hash;        // 0x04
            u32 unk08;       // 0x08
            GeistString str; // 0x0c
        } msg;

        msg.str.data = 0;
        msg.hash = 0x2EAA5F62;
        msg.unk08 = 0;
        SendMsg((int)*(u32*)((u8*)target + 0x38), 0x41E, &msg);
        reply = msg.str.data;
        msg.str.data = lbl_80393D7C;
        fn_8028B9C0(&msg.str, 0);
    }

    if (*(f32*)((u8*)actor + 0x13C) > lbl_804DAD30)
        ActorPlayAnim((int)(u32)actor->unk38, 0xDBE2CAE6,
                      (int)(u32)actor->unk38);
    *(f32*)((u8*)actor + 0x13C) = lbl_804DACE0;

    s32 animState;
    void* handle = fn_800AFD48(actor, &animState, 0);
    int anim = ModelAnimate((int)*(u32*)((u8*)actor + 0xCC), 0, 0);
    void* shape = fn_8003A1F0(handle, *(u32*)((u8*)actor + 0xF0), anim);
    fn_800C9970(actor);

    void* skin;
    f32 fade = beh->unk14;
    if (fade < lbl_804D3984) {
        f32 t;
        if (fade < lbl_804DAD34)
            t = fade / lbl_804DAD34;
        else if (fade > lbl_804D3984 - lbl_804DAD34)
            t = (lbl_804D3984 - fade) / lbl_804DAD34;
        else
            t = lbl_804DACE4;

        if (beh->unk18 != 0) {
            fn_80158C3C((u8*)actor + 0x84, 0x39FD698D, -1, -1, -1, -1, -1);
            if (beh->unk28 != -1) {
                HazardDestroy(beh->unk28);
                beh->unk28 = -1;
            }
            beh->unk28 = HazardCreate(0, (int)(u32)actor->unk38,
                                      (u8*)actor + 0x84, 0x403548D0,
                                      (int)(u32)actor->unk38, lbl_804DACE0,
                                      lbl_804DAD38, lbl_804DAD3C, lbl_804DAD3C);
            beh->unk18 = 0;
        }
        skin = (void*)SkinBlendTint(pos, 0, 0xFF, 0, t);
    } else {
        skin = pos;
        if ((beh->flags3C & 2) != 0 && reply == 0) {
            // the fade value is computed but only used by the state-6 branch
            f32 range = lbl_804D3980;
            f32 raw;
            f32 c;
            if (range >= lbl_804D397C)
                range = lbl_804D397C;
            raw = (*(f32*)((u8*)actor + 0x91C) - lbl_804DACE4) / range;
            c = lbl_804DACE0;
            if (raw >= lbl_804DACE0) {
                c = lbl_804DACE4;
                if (raw <= lbl_804DACE4)
                    c = raw;
            }
            if (c < lbl_804DACE0)
                skin = pos;
        }
    }

    if (beh->unk28 != -1 && beh->unk14 >= lbl_804D3984) {
        HazardDestroy(beh->unk28);
        beh->unk28 = -1;
    }

    if (*(s32*)((u8*)actor + 0x8F8) == 6) {
        f32 total = (f32)fn_800BA84C(9);
        f32 raw = (total - *(f32*)((u8*)actor + 0x8FC)) / total;
        f32 c = lbl_804DACE0;
        if (raw >= lbl_804DACE0) {
            c = lbl_804DACE4;
            if (raw <= lbl_804DACE4)
                c = raw;
        }
        if (c > lbl_804DACE0)
            fn_8003F15C(handle, shape, *(u32*)((u8*)actor + 0xD0), pos, 0, c);
    } else {
        if ((beh->flags3C & 2) != 0 && reply == 0) {
            if (lbl_804D579C != 0) {
                fn_8003F15C(handle, shape, *(u32*)((u8*)actor + 0xD0), pos,
                            &tints->unk168, lbl_804DACE4);
                fn_8003F15C(handle, shape, *(u32*)((u8*)actor + 0xD0), pos,
                            &tints->unk1A4, lbl_804DACE4);
            } else {
                f32 v = (f32)lbl_804D39B4;
                fn_8003F15C(handle, shape, *(u32*)((u8*)actor + 0xD0), pos, 0, v);
            }
        } else {
            fn_8003F15C(handle, shape, *(u32*)((u8*)actor + 0xD0), skin, 0,
                        lbl_804DACE4);
        }
    }

    if (HasCondition((u8*)actor + 0x270, 0x22) != 0)
        return 1;
    if (HasCondition((u8*)actor + 0x270, 0x21) == 0) {
        if (animState >= 6)
            return 1;
        if (Fn80080FA8() != 0)
            return 1;
        if (HasCondition((u8*)actor + 0x270, 9) != 0)
            return 1;
        if (HasCondition((u8*)actor + 0x270, 10) != 0)
            return 1;
        if (fn_80049660(GetPlayer()) != 0 && fn_800BA7FC(0x35) == 0)
            return 1;
    }

    if (fn_80115F68(*(u32*)((u8*)actor + 0x38)) <= lbl_804DACE0) {
        tints->unk1E0.unk14 = 0;
        tints->unk1E0.unk10 = 0xFF;
        tints->unk1E0.unk18 = 0;

        u8* host = *(u8**)((u8*)actor + 0xE8);
        s32 id = *(s32*)(host + 0x18);
        if (id >= 0) {
            FshPlayerObj* player = GetPlayer();
            u32 st = 0;
            if (PlayerInHost((Actor*)player) == 0 ||
                (player->unk26E4 != 0 &&
                 ((st = *(u32*)((u8*)player->unk26E4 + 0x38) >> 24) == 1 ||
                  (st == 0x13 && fn_800CC21C() != 0)))) {
                int anim2 = ModelAnimate((int)*(u32*)((u8*)actor + 0xCC), 0, 0);
                void* outline =
                    fn_8003A1F0((void*)id, *(u32*)((u8*)actor + 0xF0), anim2);
                fn_8003F15C((void*)id, outline, *(u32*)((u8*)actor + 0xD0), 0,
                            &tints->unk1E0, lbl_804DACE4);
            }
        }
    }
    return 1;
}

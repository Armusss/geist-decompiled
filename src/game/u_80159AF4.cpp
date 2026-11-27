// u_80159AF4.cpp (0x80159AF4-0x8015AA24): pooled 2D/3D overlay effects (ResArray lbl_804D4EA8).
// Split from u_80157730. Evidence: own Buffer-template vtables (lbl_803A6FE8/7008) and own copies of the
// template methods fn_8015A934..fn_8015AA04 at the end of the file (one copy per file, see u_80158C3C);
// it alone uses .sbss lbl_804D4EA8 and .sdata2 0x804D7D40-0x804D7D90. fn_80159AF4 (colour setter, called from
// SplineGun) opens it: it follows the previous file's template block.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef unsigned int uint;
typedef float f32;
typedef double f64;

struct Actor;
struct Vec3;


// --- engine functions with C++ linkage (mangled names in symbols.txt) ---
int ReflectUpdateCamera();
int ReflectRender(int);
void ReflectEnd();
void HostExtra(Actor*);
int CreditsTipVisible();
void ActorAim(int*, Actor*, int);
void ActorGetMatrix(Mtx34*, Actor*);
void ActorPlayAnim(int, u32, int);
void* AssetFind(u32, u32);
void BoneGetMatrix(Mtx34*, void*);
void DeferAdd(int, Vec3*, void* (*)(void*), void*);
void DeferFlush3();
void DeferFlush4();
void DeferFlush5();
void EmitterAttach(int, int, const char*);
void* Fn800F8A98(u32);
int fn_8007EE6C(int, int, int, int, int, int);
int fn_8007EF48(int, int, const char*, int, int, int, int, int, int, int);
void fn_8007F064(int, int);
Actor* GetPlayer();
void GhostShotDrawAll();
void GhostShotUpdateAll(f32);
void GrenadeInit();
void GrenadeLoadTexture();
void GrenadeNop(f32);
void GrenadeRemoveAll();
void LinkInfoInit(void*);
f32 LoadingTime();
int PlayerInHost(Actor*);
int Rand();
void SltAssetInit();
void SltEffectCleanup();
void SltEffectKillAll();
void SltEffectUpdate(f32);
void SystemPoll();
void TextureLoad(short*, int);
void* VAlloc(int);
void VFree(void*);
void* fn_8000AA20Probe();
void fn_8013A3D4();
void fn_8013A834(f32, f32, void*, u8, u8, u8, u8);
void fn_8013AC68(int, int);
void fn_8013ADF4(f64, f64, f64, f64, f64, int, int, int, int, int, int, int, int, int);
void fn_8013B5D4();
void fn_8013B608(void*, void*, u8, u8, u8, u8, u8, u8, u8, u8);
void fn_8013B6F8(f32, int, void*);
void fn_801EA880(void*, void*, int, int, int, int, int);
void fn_801EAEAC();
void fn_80210D80(int, int, int, int, int, int, int);
void fn_80210FC8(f32, f64, f64, int, int, int, int, void*, void*, void*);
void fn_802113F0(f32, int, int, int, void*);
void fn_80211470(f32, int, int, int);
void fn_80211638(f64, int, f64, f64, int, void*, void*, int, int);
void fn_8028B2D4(void*);
int fn_800391DC(int);
void fn_8028B35C(void*, void*, int);
void* fn_8028B3BC(void*, void*, int);
void fn_8028B960(void*);
void fn_8028B9C0(void*, int);
void fn_8028BA9C(void*);
// Base Commit/Append (0x8028BC2C / 0x8028BC28) of the Buffer classes in u_8028A8D4.cpp, which the
// array overrides here call directly. Class and member names are guesses.
class ScreenEffectBuffer {
public:
    void Commit(int start, unsigned long size);
    void Append(unsigned char* data, unsigned long size);
};
void fn_800A0094(void*, void*);
void fn_800A0100(void*);
f32 fn_800A1394(f32);
void fn_800A1418();

extern "C" {
void memset(void*, int, u32);
int SendMsg(int to, int msg, void* data);
void rand();

int fn_800059D0(int);
void fn_80005A64();
void fn_80008F58();
void fn_80008F64();
void fn_80009E10();
void fn_80009E44(int, int);
void fn_8000BA8C(f64, f64, f64, f64, f64, f64);
void fn_8000DDD0();
void fn_8000F738(void*);
void fn_8001CE28();
int fn_80049750();
void fn_80049948();
void fn_8007CF04();
void fn_800D35B0(int, int, const char*);
void fn_800F1138(int, int, void (*)(int*));
void fn_8018D2D4();
void fn_8018D79C(f32);
void fn_801A001C();
void fn_8001D040();
void fn_8001D258();
void fn_8001D858();
void fn_8002756C();
void fn_80027690(int);
int fn_80027A58(int, void*, void*, int, int);
void fn_800390F4(int);
int fn_800391DC(int);
void fn_8003F15C(f32, int, void*, int, int, int);
int fn_80045524(int, int, int);
void fn_800499E8(f64, int);
void fn_8007902C();
void fn_8007AE7C();
void fn_8007AFBC();
u32 fn_8007EB34(u32, u32);
void fn_800857A8();
void fn_80093F88(int, int, int, int, int);
void fn_80096C74(f64, f64);
void fn_80096DD0(int, int, int, int);
void fn_80097DD0(f64, int);
void fn_8009851C(u32, u32, u32);
void fn_8009EC5C(void*, void*);
void fn_8009ED70(void*, void*);
void fn_8009F700(void*, void*, int);
void fn_8009F7A8(void*, void*);
void fn_800B9520();
void fn_800BB3A0();
void fn_800C5654(u32, u32, u32);
void fn_800C5E5C();
void fn_800C5E9C(int);
int fn_800C6448();
void fn_800DE0C4(f64);
int SaveCommand(int, int);
void fn_800DE9D8();
void fn_800EBBB8(f64, int, int);
void fn_800EC614(int, int, int, int);
void fn_800EC80C(int, int, int, int);
void fn_800EC9DC(int, int, int, int);
void fn_800ECB18();
void fn_800F0D24();
void fn_800F0DFC();
void fn_800F0ED4();
void fn_800F39E0();
void fn_800F45F8();
void fn_800F4854();
void fn_800F4ACC();
void fn_800F4EE0();
void fn_800F4F0C();
void fn_800F6264(f32);
void fn_800F671C();
void fn_800F6804();
u32 fn_800F7994(int, u32);
int fn_800FD3EC();
void fn_800FD924(int);
void fn_800FD998(f32);
void fn_800FDA10();
void fn_800FDA90();
void fn_800FDC48();
void fn_800FDC78();
void fn_800FF294(int);
void fn_800FF530(f32);
void fn_800FF66C();
void fn_80100EB0(f64, void*, void*, void*, void*, void*);
void fn_801116E4();
void fn_80120D60();
void fn_80122590();
void fn_8013ECCC();
void fn_80153C10(f32);
void fn_80156AA0();
void fn_80156AA4();
void fn_80160B3C(f32);
void fn_80160B40();
void fn_80160B80();
void fn_8016BEF8(f32);
void fn_8016C120();
void fn_8016C198();
void fn_8016F120(int, void*);
void fn_8016F8EC();
void fn_801764F0();
void fn_8017B164();
void fn_8017B488(f32);
void fn_8017B6C0();
void fn_8017B7A0();
void fn_8017B7A4();
void fn_8017E288(f32);
void fn_8017E5BC();
void fn_8017E69C();
void fn_8017E728();
void fn_8017E72C();
void fn_801849DC();
void fn_80185EE4(f32);
void fn_80186314();
void fn_80186754();
void fn_801868AC();
int fn_801B829C(int);
void fn_801D60B4(f32);
void fn_801E092C();
void fn_801E4850();
void fn_801EA87C();
void fn_801FE008();
void fn_801FE00C();
void fn_801FE068();
void fn_80200630();
void fn_80200650(f32);
void fn_802154B0(int);
void fn_80215A24();
int fn_80215B40(int);
void fn_8021696C();
void fn_802177F4();
void fn_80217AFC();
int fn_80220A38();
void fn_80228688();
void fn_80228A0C(f32);
void fn_80233848();
void fn_80233928();
void fn_80233A20(f32);
void fn_80233B60();
void fn_80233C30();
void fn_80234900();
void fn_802698D0();
void fn_80269944();
void fn_8027E76C();
void fn_8027E8F8();
void fn_802845CC();
void fn_802846F0();
void fn_80284CB0();
void fn_802876F0();
void fn_80287764();
void fn_802886B4();
void fn_8028A388();
void fn_8028A4BC();
void fn_8028A554(f32);
void fn_8028A6B8();
void fn_8028A728();
void fn_8028A8D4();
int fn_802A1D04();
void fn_802A20A4();
void fn_802A214C();
void fn_802A3AB8();
void fn_802A3BEC();
void fn_802A3BF0(f32);
void fn_802A3CE0();
void fn_802A3D50();
void fn_802A815C();
void fn_802A8204();
void fn_802C2D20();
void fn_802C2D5C();
void fn_802C2E5C();
void fn_802C2F5C(f32);
void fn_802C301C();
void fn_802C30A0();
void fn_802CEAAC(f32);
void fn_802CEBAC();
void fn_802CECE8();
}

// Defined later in this file.
extern "C" {
void fn_801582AC();
void fn_801584F8();
void fn_8015860C();
void fn_80158744();
void fn_801587B8(int);
void fn_80158834();
void fn_80158990(int, int);
void fn_80158AAC(int, int, int);
int fn_80158AFC(int);
void fn_80158B4C(int);
void fn_80158B90(int, int, int);
int fn_80158BE0();
int fn_80158C3C(f32* pos, int kind, int a, int b, int c, int d, int e);
void fn_80158F90();
void* fn_8015902C(void*, short);
void fn_801590A4();
void fn_80159118(f32);
void fn_801592E8();
void fn_801593A4();
void* fn_801593FC(void*, short);
void fn_80159464(int*);
void fn_80159554(f32, f32, f32, f32, f32, f32, f32, int, int, f32*, f32*);
void fn_80159A04(void*, int, unsigned);
void fn_80159A4C(void*, char*, int);
int fn_80159AAC();
void fn_80159AB4(int*, int*);
void fn_80159AD4(int*, int*);
void fn_80159AF4(u32, u8, u8, u8);
void fn_80159B2C();
void* fn_80159B94(void*, short);
void fn_80159C0C();
void fn_80159D48(f32*);
void fn_80159F88(f32);
void fn_80159FEC();
void fn_8015A050();
void* fn_8015A0A4(void*, short);
void fn_8015A10C(void*);
void fn_8015A1A0(f32, int);
void fn_8015A4AC(f32, f32, void*, void*, int, int, void*);
int fn_8015A570(f32, f32, u8, u8, u8, u8, u8, u8, u8, u8, int, int);
int fn_8015A78C(u8, u8, u8, u8, u8, u8, u8, u8);
void fn_8015A934(void*, int, unsigned);
void fn_8015A97C(void*, int, unsigned);
int fn_8015A9DC();
void fn_8015A9E4(int*, int*);
void fn_8015AA04(int*, int*);
void fn_8015AA24();
void* fn_8015AAC4(void*, short);
void fn_8015AB3C();
void fn_8015AB90(f32);
void fn_8015ABB0();
void fn_8015ABB4();
void* fn_8015AC08(void*, short);
void fn_8015AC70();
void fn_8015AF74();
int fn_8015B2A0(f32, void*, u8, u8, u8, u8, u8, u8);
int fn_8015B428(f32, void*, int, u8, u8, u8, u8, u8, u8);
void fn_8015B5B4(int);
void fn_8015BA5C(f32);
void fn_8015BF90(void*, int, unsigned);
void fn_8015BFD8(void*, int, unsigned);
int fn_8015C038();
void fn_8015C040(int*, int*);
void fn_8015C060(int*, int*);
}

// ---- shared per-player HUD/effect pools (three 4-entry arrays in .bss) ----
struct FxPoolEntry {
    s32 state;      // 0x00
    s32 alpha;      // 0x04
    f32 timer;      // 0x08
    void* objA;     // 0x0C
    void* objB;     // 0x10
};

struct FocusFx {
    s32 state;      // 0x00
    s32 alpha;      // 0x04
    f32 timer;      // 0x08
    f32 textAlpha;  // 0x0C
    s32 sndA;       // 0x10
    s32 sndB;       // 0x14
    f32 fade;       // 0x18
    void* obj;      // 0x1C
};

extern FxPoolEntry* lbl_804D4E34;
extern FxPoolEntry* lbl_804D4E44;
extern FocusFx* lbl_804D4E48;

// A 2D draw request posted to an object with message 0x7300.
struct HudDraw {
    void* obj;       // 0x00
    s32 unk04;       // 0x04
    s32 unk08;       // 0x08
    s32 unk0C;       // 0x0C
    s32 unk10;       // 0x10
    s32 unk14;       // 0x14
    s32 unk18;       // 0x18
    s32 unk1C;       // 0x1C
    u8 color[4];     // 0x20
    f32 srcX;        // 0x24
    s32 srcYi;       // 0x28
    f32 srcZ;        // 0x2C
    s32 unk30;       // 0x30
    s32 unk34;       // 0x34
    s32 unk38;       // 0x38
    s32 unk3C;       // 0x3C
    f32 colR;        // 0x40
    f32 colG;        // 0x44
    f32 colB;        // 0x48
    f32 colA;        // 0x4C
    u8 pad50[12];    // 0x50
    f32 dstX;        // 0x5C
    s32 dstYi;       // 0x60
    f32 dstZ;        // 0x64
    s32 unk68;       // 0x68
    u8 pad6C[0x210 - 0x6C];
};

// --- data ---
extern f32 lbl_804D4E30;
extern s32 lbl_804D4E5C, lbl_804D4E60, lbl_804D4E64, lbl_804D4E68, lbl_804D4E6C, lbl_804D4E70;
extern s32 lbl_804D4E74, lbl_804D4E78, lbl_804D4E7C, lbl_804D4E80, lbl_804D4E84, lbl_804D4E88;
extern s32 lbl_804D4E8C, lbl_804D4E90, lbl_804D4E94, lbl_804D4E98;
extern s32 gSaveLoaded;

extern s32 lbl_804D2F28;
extern f32 lbl_804D2F2C;
extern f32 lbl_804D2F30;
extern f32 lbl_804D2F34;
extern s32 lbl_804D2F38;
extern s32 lbl_804D2F3C;
extern f32 lbl_804D2F40;
extern f32 lbl_804D2F44;
extern u32 lbl_804D2F48;
extern f32 lbl_804D2F50;

extern u32 lbl_804D7CE8;

extern u32 lbl_803A59C8[];
extern u32 lbl_803A6FA8[8];
extern u32 lbl_803A6FC8[8];
extern u32 lbl_803A6FE8[8];
extern u32 lbl_803A7008[8];
extern u32 lbl_803A7028[8];
extern u32 lbl_803A7048[8];

// Resizable pointer array used by the effect pools.
class ResArray {
public:
    virtual ~ResArray();
    s32 unk04;     // 0x04
    s32 size;      // 0x08 (bytes)
    void** data;   // 0x0C
};

extern ResArray* lbl_804D4EA0;
extern ResArray* lbl_804D4EA8;
extern ResArray* lbl_804D4EB0;
extern f32 lbl_804D4EB4;

extern u8 lbl_80346630[16];

extern "C" {


// 0x80159AF4: set an effect's tint colours.
void fn_80159AF4(u32 p, u8 r, u8 g, u8 b) {
    if (p == 0) {
        return;
    }
    if (p == 0) {
        return;
    }
    *(u8*)(p + 0x34) = r;
    *(u8*)(p + 0x35) = g;
    *(u8*)(p + 0x36) = b;
    *(u8*)(p + 0x37) = 0xff;
    *(u8*)(p + 0x38) = r;
    *(u8*)(p + 0x39) = g;
    *(u8*)(p + 0x3a) = b;
    *(u8*)(p + 0x3b) = 0;
}

// 0x80159B2C: destroy the grenade HUD pool.
void fn_80159B2C() {
    while ((u32)lbl_804D4EA8->size >> 2 != 0) {
        VFree(*(void**)lbl_804D4EA8->data);
        fn_8028B35C(lbl_804D4EA8, lbl_804D4EA8->data, 4);
    }
    delete lbl_804D4EA8;
}

// 0x80159B94: destroy part of the grenade HUD pool object.
void* fn_80159B94(void* p, short freeIt) {
    if (p != 0) {
        *(void**)p = &lbl_803A7008;
        if (p != 0) {
            *(void**)p = &lbl_803A6FE8;
            fn_8028B960(p);
            fn_8028B9C0(p, 0);
        }
        if (freeIt > 0) {
            VFree(p);
        }
    }
    return p;
}
void fn_80159C0C() {
    int** p;

    for (p = (int**)lbl_804D4EA8->data;
         p != (int**)((u8*)lbl_804D4EA8->data + lbl_804D4EA8->size); p++) {
        int obj = (int)*p;
        int base;
        int passA;
        f32 alpha;

        fn_80159D48((f32*)obj);
        if (*(int*)(obj + 0x48) != 0 && *(int*)(obj + 0x6c) == 0) {
            base = (int)Fn800F8A98(0xfea88c51);
            passA = fn_800F7994(base, 0x9fe04b6c);
            base = fn_800F7994(base, 0x28fb247e);
            fn_8013AC68(passA, 0);
            fn_8013A834(*(f32*)(obj + 0x54), 0.0f, (void*)(obj + 0x60),
                        *(u8*)(obj + 0x34), *(u8*)(obj + 0x35), *(u8*)(obj + 0x36), 0x80);
            fn_8013A3D4();
            fn_8013AC68(base, 0);
            alpha = fn_800A1394(*(f32*)(obj + 0x5c));
            fn_8013A834(*(f32*)(obj + 0x54), 0.0f, (void*)(obj + 0x60), 0xff, 0xff, 0xff,
                        (u8)(s32)(alpha * 64.0f + 128.0f));
            fn_8013A3D4();
        }
    }
}
void fn_80159D48(f32* p) {
    if (p != 0) {
        if (*(int*)(p + 0x1b) == 0) {
            f32* e;
            f32 col[3];
            f32 pos[3];
            int i;

            fn_8013B6F8(3.0f, -4, p);
            for (i = 0, e = p; i < 0x40; i++, e += 5) {
                if (*(int*)(e + 0x1c) != 0) {
                    f32 s = p[0xf];
                    f32 k = e[0x20] * s;

                    col[0] = 0.0f;
                    col[1] = 0.0f;
                    col[2] = 0.0f;
                    pos[0] = e[0x1d] * s * k;
                    pos[1] = e[0x1e] * s * k;
                    pos[2] = e[0x1f] * s * k;
                    pos[0] += e[0x1d] * s;
                    pos[1] += e[0x1e] * s;
                    pos[2] += e[0x1f] * s;
                    fn_8013B608(col, pos, *(u8*)(p + 0xd), *(u8*)((u8*)p + 0x35),
                                *(u8*)((u8*)p + 0x36), *(u8*)((u8*)p + 0x37),
                                *(u8*)(p + 0xe), *(u8*)((u8*)p + 0x39),
                                *(u8*)((u8*)p + 0x3a), *(u8*)((u8*)p + 0x3b));
                }
            }
            fn_8013B5D4();
        } else if (*(int*)(p + 0x1b) == 1) {
            f32 m[9];
            f32 t;
            int i;

            for (i = 0; i < 9; i++) {
                m[i] = p[i];
            }
            t = p[0x16] / (1.0f / p[0x10]);
            if (t > 1.0f) {
                t = 1.0f;
            }
            t = p[0xf] * t;
            for (i = 0; i < 9; i++) {
                m[i] *= t;
            }
            fn_800A0100(m);
            fn_8003F15C(1.0f, *(int*)(p + 0x14), m, 1, 0, 0);
        }
    }
}

// 0x80159F88: update every grenade HUD entry.
void fn_80159F88(f32 dt) {
    void** p;

    p = lbl_804D4EA8->data;
    while (p != (void**)((u8*)lbl_804D4EA8->data + lbl_804D4EA8->size)) {
        fn_8015A1A0(dt, (int)*p);
        p = p + 1;
    }
}

// 0x80159FEC: look up the grenade HUD textures.
void fn_80159FEC() {
    u16 ids[2];
    int h;

    h = (int)Fn800F8A98(0xfea88c51);
    ids[0] = (u16)fn_800F7994(h, 0x9fe04b6c);
    ids[1] = (u16)fn_800F7994(h, 0x28fb247e);
    TextureLoad((short*)ids, 2);
}

// 0x8015A050: create the grenade HUD pool.
void fn_8015A050() {
    void* p = VAlloc(0x14);

    if (p != 0) {
        fn_8028BA9C(p);
        *(void**)p = &lbl_803A6FE8;
        *(void**)p = &lbl_803A7008;
    }
    lbl_804D4EA8 = (ResArray*)p;
}

// 0x8015A0A4: destroy the grenade HUD pool object.
void* fn_8015A0A4(void* p, short freeIt) {
    if (p != 0) {
        *(void**)p = &lbl_803A6FE8;
        fn_8028B960(p);
        fn_8028B9C0(p, 0);
        if (freeIt > 0) {
            VFree(p);
        }
    }
    return p;
}

// 0x8015A10C: remove one grenade HUD entry.
void fn_8015A10C(void* entry) {
    void** p = lbl_804D4EA8->data;
    if (entry != 0) {
        void** end = (void**)((u8*)lbl_804D4EA8->data + lbl_804D4EA8->size);
        for (; p != end; p++) {
            if (entry == *p) {
                if (((s32*)entry)[0x1b] == 1) {
                    fn_800390F4(((s32*)entry)[0x14]);
                }
                VFree(entry);
                fn_8028B35C(lbl_804D4EA8, p, 4);
                return;
            }
        }
    }
}
// TODO (not drafted yet, 0x30C bytes at 0x8015A1A0)

// 0x8015A4AC: copy a HUD/effect description into a new entry.
void fn_8015A4AC(f32 a, f32 b, void* dst, void* src, int active, int arg, void* vec) {
    if (dst == 0) {
        return;
    }
    if (dst == 0) {
        return;
    }
    *(u32*)((u8*)dst + 0x00) = *(u32*)((u8*)src + 0x00);
    *(u32*)((u8*)dst + 0x04) = *(u32*)((u8*)src + 0x04);
    *(u32*)((u8*)dst + 0x08) = *(u32*)((u8*)src + 0x08);
    *(u32*)((u8*)dst + 0x0c) = *(u32*)((u8*)src + 0x0c);
    *(u32*)((u8*)dst + 0x10) = *(u32*)((u8*)src + 0x10);
    *(u32*)((u8*)dst + 0x14) = *(u32*)((u8*)src + 0x14);
    *(u32*)((u8*)dst + 0x18) = *(u32*)((u8*)src + 0x18);
    *(u32*)((u8*)dst + 0x1c) = *(u32*)((u8*)src + 0x1c);
    *(u32*)((u8*)dst + 0x20) = *(u32*)((u8*)src + 0x20);
    *(u32*)((u8*)dst + 0x24) = *(u32*)((u8*)src + 0x24);
    *(u32*)((u8*)dst + 0x28) = *(u32*)((u8*)src + 0x28);
    *(u32*)((u8*)dst + 0x2c) = *(u32*)((u8*)src + 0x2c);
    if (active != 0) {
        *(u32*)((u8*)dst + 0x48) = 1;
        *(u32*)((u8*)dst + 0x4c) = arg;
        *(f32*)((u8*)dst + 0x54) = b;
        *(u32*)((u8*)dst + 0x60) = *(u32*)((u8*)vec + 0x00);
        *(u32*)((u8*)dst + 0x64) = *(u32*)((u8*)vec + 0x04);
        *(u32*)((u8*)dst + 0x68) = *(u32*)((u8*)vec + 0x08);
        return;
    }
    *(u32*)((u8*)dst + 0x48) = 0;
    *(u32*)((u8*)dst + 0x4c) = 0;
    *(f32*)((u8*)dst + 0x54) = 0.0f;
    *(u32*)((u8*)dst + 0x60) = 0.0f;
    *(u32*)((u8*)dst + 0x64) = 0.0f;
    *(u32*)((u8*)dst + 0x68) = 0.0f;
}
int fn_8015A570(f32 a, f32 b, u8 p3, u8 p4, u8 p5, u8 p6, u8 p7, u8 p8, u8 p9, u8 p10,
                int kind, int arg) {
    int* obj;

    while ((u32)lbl_804D4EA8->size >> 2 >= 8) {
        VFree(*(void**)lbl_804D4EA8->data);
        fn_8028B35C(lbl_804D4EA8, lbl_804D4EA8->data, 4);
    }
    obj = (int*)VAlloc(0x570);
    if (obj != 0) {
        *(int*)fn_8028B3BC(lbl_804D4EA8,
                           (u8*)lbl_804D4EA8->data + lbl_804D4EA8->size, 4) = (int)obj;
        LinkInfoInit(obj);
        *(int*)((u8*)obj + 0x30) = 0x10;
        *(u8*)((u8*)obj + 0x34) = p3;
        *(u8*)((u8*)obj + 0x35) = p4;
        *(u8*)((u8*)obj + 0x36) = p5;
        *(u8*)((u8*)obj + 0x37) = p6;
        *(u8*)((u8*)obj + 0x38) = p7;
        *(u8*)((u8*)obj + 0x39) = p8;
        *(u8*)((u8*)obj + 0x3a) = p9;
        *(u8*)((u8*)obj + 0x3b) = p10;
        *(f32*)((u8*)obj + 0x3c) = b;
        *(f32*)((u8*)obj + 0x40) = a;
        *(f32*)((u8*)obj + 0x44) = 0.3f;
        *(int*)((u8*)obj + 0x48) = 0;
        *(f32*)((u8*)obj + 0x5c) = 235.0f;
        *(int*)((u8*)obj + 0x6c) = kind;
        *(f32*)((u8*)obj + 0x58) = 0.0f;
        if (kind == 0) {
            u8* e = (u8*)obj + 0x70;
            int n = 2;
            int i;

            do {
                for (i = 0; i < 32; i++) {
                    *(int*)(e + i * 0x14) = 0;
                }
                e += 0x280;
            } while (--n != 0);
            *(int*)((u8*)obj + 0x50) = -1;
        } else if (kind == 1) {
            int h = fn_800391DC(arg);

            *(int*)((u8*)obj + 0x50) = h;
            if (h == arg || h == -1) {
                fn_8028B2D4(lbl_804D4EA8);
                VFree(obj);
                obj = 0;
            }
        }
    }
    return (int)obj;
}
int fn_8015A78C(u8 p1, u8 p2, u8 p3, u8 p4, u8 p5, u8 p6, u8 p7, u8 p8) {
    int* obj;

    while ((u32)lbl_804D4EA8->size >> 2 >= 8) {
        VFree(*(void**)lbl_804D4EA8->data);
        fn_8028B35C(lbl_804D4EA8, lbl_804D4EA8->data, 4);
    }
    obj = (int*)VAlloc(0x570);
    if (obj != 0) {
        u8* e = (u8*)obj + 0x70;
        int n;
        int i;

        *(int*)fn_8028B3BC(lbl_804D4EA8,
                           (u8*)lbl_804D4EA8->data + lbl_804D4EA8->size, 4) = (int)obj;
        LinkInfoInit(obj);
        *(int*)((u8*)obj + 0x30) = 0x10;
        *(u8*)((u8*)obj + 0x34) = p1;
        *(u8*)((u8*)obj + 0x35) = p2;
        *(u8*)((u8*)obj + 0x36) = p3;
        *(u8*)((u8*)obj + 0x37) = p4;
        *(u8*)((u8*)obj + 0x38) = p5;
        *(u8*)((u8*)obj + 0x39) = p6;
        *(u8*)((u8*)obj + 0x3a) = p7;
        *(u8*)((u8*)obj + 0x3b) = p8;
        *(f32*)((u8*)obj + 0x3c) = 12.0f;
        *(f32*)((u8*)obj + 0x40) = 2.0f;
        *(f32*)((u8*)obj + 0x44) = 0.3f;
        *(int*)((u8*)obj + 0x48) = 0;
        *(f32*)((u8*)obj + 0x5c) = 235.0f;
        *(int*)((u8*)obj + 0x6c) = 0;
        *(f32*)((u8*)obj + 0x58) = 0.0f;
        n = 2;
        do {
            for (i = 0; i < 32; i++) {
                *(int*)(e + i * 0x14) = 0;
            }
            e += 0x280;
        } while (--n != 0);
        *(int*)((u8*)obj + 0x50) = -1;
    }
    return (int)obj;
}

// 0x8015A934 / 0x8015A97C and siblings: grenade pool element array helpers.
void fn_8015A934(void* pool, int start, unsigned size) {
    ((ScreenEffectBuffer*)pool)->Commit((int)start, size);
    fn_8015A9E4((int*)start, (int*)(start + (size & 0xfffffffc)));
}

void fn_8015A97C(void* pool, int start, unsigned size) {
    unsigned long p = (unsigned long)start;

    fn_8015AA04((int*)p, (int*)(p + (size & 0xfffffffc)));
    ((ScreenEffectBuffer*)pool)->Append((unsigned char*)start, size);
}

int fn_8015A9DC() {
    return 4;
}

void fn_8015A9E4(int* p, int* end) {
    while (p < end) {
        p++;
    }
}

void fn_8015AA04(int* p, int* end) {
    while (p < end) {
        p++;
    }
}
} // extern "C"

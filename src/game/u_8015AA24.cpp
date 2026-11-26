// u_8015AA24.cpp (0x8015AA24-0x8015C080): pooled timed effects (ResArray lbl_804D4EB0, timer lbl_804D4EB4).
// Split from u_80157730. Evidence: own Buffer-template vtables (lbl_803A7028/7048) and own copies of the
// template methods fn_8015BF90..fn_8015C060 closing the file; it alone uses .sbss 0x804D4EB0-0x804D4EB8,
// .sdata lbl_804D2F50 and .sdata2 0x804D7D90-0x804D7DD4.
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
void GrenadeNop();
void GrenadeRemoveAll();
void LinkInfoInit(void*);
f32 LoadingTime();
void PlayerInHost(Actor*);
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
void fn_8015A10C(int);
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
extern s32 lbl_804D4B98;

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


// 0x8015AA24: destroy the emitter pool.
void fn_8015AA24() {
    while ((u32)lbl_804D4EB0->size >> 2 != 0) {
        int e = *(int*)lbl_804D4EB0->data;
        if (*(s32*)(e + 4) == 2 || *(s32*)(e + 4) == 3) {
            if (*(u32*)(e + 0xc) != 0) {
                VFree(*(void**)(e + 0xc));
                *(s32*)(e + 0xc) = 0;
            }
        }
        VFree((void*)e);
        fn_8028B35C(lbl_804D4EB0, lbl_804D4EB0->data, 4);
    }
    delete lbl_804D4EB0;
}

// 0x8015AAC4: destroy part of the emitter pool object.
void* fn_8015AAC4(void* p, short freeIt) {
    if (p != 0) {
        *(void**)p = &lbl_803A7048;
        if (p != 0) {
            *(void**)p = &lbl_803A7028;
            fn_8028B960(p);
            fn_8028B9C0(p, 0);
        }
        if (freeIt > 0) {
            VFree(p);
        }
    }
    return p;
}

// 0x8015AB3C: update every emitter.
void fn_8015AB3C() {
    void** p;

    p = lbl_804D4EB0->data;
    while (p != (void**)((u8*)lbl_804D4EB0->data + lbl_804D4EB0->size)) {
        fn_8015B5B4((int)*p);
        p = p + 1;
    }
}

// 0x8015AB90: update all emitter effects.
void fn_8015AB90(f32 dt) {
    fn_8015BA5C(dt);
}

// 0x8015ABB0: empty hook.
void fn_8015ABB0() {
}

// 0x8015ABB4: create the emitter pool.
void fn_8015ABB4() {
    void* p = VAlloc(0x14);

    if (p != 0) {
        fn_8028BA9C(p);
        *(void**)p = &lbl_803A7028;
        *(void**)p = &lbl_803A7048;
    }
    lbl_804D4EB0 = (ResArray*)p;
}

// 0x8015AC08: destroy the emitter pool object.
void* fn_8015AC08(void* p, short freeIt) {
    if (p != 0) {
        *(void**)p = &lbl_803A7028;
        fn_8028B960(p);
        fn_8028B9C0(p, 0);
        if (freeIt > 0) {
            VFree(p);
        }
    }
    return p;
}
// TODO (not drafted yet, 0x304 bytes at 0x8015AC70)
// TODO (not drafted yet, 0x32C bytes at 0x8015AF74)
int fn_8015B2A0(f32 timer, void* posRaw, u8 a, u8 b, u8 c, u8 d, u8 e, u8 f) {
    int* pos = (int*)posRaw;
    u32* p;
    int i;

    while ((u32)lbl_804D4EB0->size >> 2 >= 8) {
        int* obj = *(int**)lbl_804D4EB0->data;

        if ((obj[1] == 2 || obj[1] == 3) && obj[3] != 0) {
            VFree((void*)obj[3]);
            obj[3] = 0;
        }
        VFree(obj);
        fn_8028B35C(lbl_804D4EB0, lbl_804D4EB0->data, 4);
    }
    p = (u32*)VAlloc(0x4c);
    if (p != 0) {
        u32* slot = (u32*)fn_8028B3BC(lbl_804D4EB0,
                                      (u8*)lbl_804D4EB0->data + lbl_804D4EB0->size, 4);

        *slot = (u32)p;
        p[0] = 1;
        p[1] = 0;
        for (i = 0; i < 12; i++) {
            p[3 + i] = pos[i];
        }
        *(f32*)(p + 2) = lbl_804D4EB4;
        *(f32*)(p + 0xf) = timer;
        *(u8*)((u8*)p + 0x44) = a;
        *(u8*)((u8*)p + 0x45) = b;
        *(u8*)((u8*)p + 0x46) = c;
        *(u8*)((u8*)p + 0x47) = d;
        *(u8*)((u8*)p + 0x48) = e;
        *(u8*)((u8*)p + 0x49) = f;
    }
    return p != 0;
}
int fn_8015B428(f32 timer, void* posRaw, int arg, u8 a, u8 b, u8 c, u8 d, u8 e, u8 f) {
    int* pos = (int*)posRaw;
    u32* p;
    int i;

    while ((u32)lbl_804D4EB0->size >> 2 >= 8) {
        int* obj = *(int**)lbl_804D4EB0->data;

        if ((obj[1] == 2 || obj[1] == 3) && obj[3] != 0) {
            VFree((void*)obj[3]);
            obj[3] = 0;
        }
        VFree(obj);
        fn_8028B35C(lbl_804D4EB0, lbl_804D4EB0->data, 4);
    }
    p = (u32*)VAlloc(0x4c);
    if (p != 0) {
        u32* slot = (u32*)fn_8028B3BC(lbl_804D4EB0,
                                      (u8*)lbl_804D4EB0->data + lbl_804D4EB0->size, 4);

        *slot = (u32)p;
        p[0] = 1;
        p[1] = 1;
        for (i = 0; i < 12; i++) {
            p[3 + i] = pos[i];
        }
        *(f32*)(p + 2) = lbl_804D4EB4;
        *(f32*)(p + 0xf) = timer;
        p[0x10] = arg;
        *(u8*)((u8*)p + 0x44) = a;
        *(u8*)((u8*)p + 0x45) = b;
        *(u8*)((u8*)p + 0x46) = c;
        *(u8*)((u8*)p + 0x47) = d;
        *(u8*)((u8*)p + 0x48) = e;
        *(u8*)((u8*)p + 0x49) = f;
    }
    return p != 0;
}
// TODO (not drafted yet, 0x4A8 bytes at 0x8015B5B4)
// TODO (not drafted yet, 0x534 bytes at 0x8015BA5C)

// 0x8015BF90 / 0x8015BFD8 and siblings: emitter pool element array helpers.
void fn_8015BF90(void* pool, int start, unsigned size) {
    ((ScreenEffectBuffer*)pool)->Commit((int)start, size);
    fn_8015C040((int*)start, (int*)(start + (size & 0xfffffffc)));
}

void fn_8015BFD8(void* pool, int start, unsigned size) {
    unsigned long p = (unsigned long)start;

    fn_8015C060((int*)p, (int*)(p + (size & 0xfffffffc)));
    ((ScreenEffectBuffer*)pool)->Append((unsigned char*)start, size);
}

int fn_8015C038() {
    return 4;
}

void fn_8015C040(int* p, int* end) {
    while (p < end) {
        p++;
    }
}

void fn_8015C060(int* p, int* end) {
    while (p < end) {
        p++;
    }
}
} // extern "C"

// u_80157730.cpp: the master game file (0x80157730-0x8015C080). It boots and tears down every
// game subsystem, runs the per-frame update and draw passes, shows the save-file prompt and
// manages the shared per-player HUD/effect pools (small handle arrays in .sbss).
// Names are descriptive guesses; the code is matched against the retail DOL.

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
struct Mtx34;
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
void fn_8013A834(f64, f64, void*, u8, u8, u8, u8);
void fn_8013AC68(int, int);
void fn_8013ADF4(f64, f64, f64, f64, f64, int, int, int, int, int, int, int, int, int);
void fn_8013B5D4();
void fn_8013B608(void*, void*, u8, u8, u8, u8, u8, u8, u8, u8);
void fn_8013B6F8(f64, int, void*);
void fn_801EA880(void*, void*, int, int, int, int, int);
void fn_801EAEAC();
void fn_80210D80(int, int, int, int, int, int, int);
void fn_80210FC8(f64, f64, f64, int, int, int, int, void*, void*, void*);
void fn_802113F0(f64, int, int, int, void*);
void fn_80211470(f64, int, int, int);
void fn_80211638(f64, int, f64, f64, int, void*, void*, int, int);
void fn_8028B2D4(void*);
void fn_8028B35C(void*, void*, int);
void* fn_8028B3BC(void*, int, int);
void fn_8028B960(void*);
void fn_8028B9C0(void*, int);
void fn_8028BA9C(void*);
void fn_8028BC28(void*, void*, unsigned);
void fn_8028BC2C(void*);
void fn_800A0094(void*, void*);
void fn_800A0100(void*);
void fn_800A1394();
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
void fn_80027690();
void fn_80027690();
void fn_80027A58(int, void*, void*, int, int);
void fn_800390F4(int);
int fn_800391DC(int);
void fn_8003F15C(f64, f32, void*, int, int, int);
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
int fn_800DE2F8(int, int);
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
void fn_80233A20(f64);
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
void fn_80158C3C();
void fn_80158F90();
void* fn_8015902C(void*, short);
void fn_801590A4();
void fn_80159118(f32);
void fn_801592E8();
void* fn_801593A4();
void* fn_801593FC(void*, short);
void fn_80159464(int*);
void fn_80159554();
void fn_80159A04(void*, int, unsigned);
void fn_80159A4C(void*, char*, int);
int fn_80159AAC();
void fn_80159AB4(int*, int*);
void fn_80159AD4(int*, int*);
void fn_80159AF4(int, u8, u8, u8);
void fn_80159B2C();
void* fn_80159B94(void*, short);
void fn_80159C0C();
void fn_80159D48(f32*);
void fn_80159F88(f32);
void fn_80159FEC();
void* fn_8015A050();
void* fn_8015A0A4(void*, short);
void fn_8015A10C(int);
void fn_8015A1A0(f32, int);
void fn_8015A4AC(f32, f32, void*, void*, int, int, void*);
int fn_8015A570(f32, f64, u8, u8, u8, u8, u8, u8, u8, u8, int, int);
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
void* fn_8015ABB4();
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

extern FxPoolEntry lbl_8046DDE8[4];
extern FxPoolEntry lbl_8046DE38[4];
extern FocusFx lbl_8046DE88[4];

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
extern s32 lbl_804D49C4;

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
extern f32 lbl_804D7CEC;
extern f32 lbl_804D7CF0;
extern f32 lbl_804D7CF4;
extern f32 lbl_804D7CF8;
extern f32 lbl_804D7CFC;
extern f32 lbl_804D7D00;
extern f32 lbl_804D7D04;
extern f32 lbl_804D7D08;
extern f32 lbl_804D7D0C;
extern f32 lbl_804D7D10;
extern f32 lbl_804D7D18;
extern f32 lbl_804D7D1C;
extern f32 lbl_804D7D20;
extern f32 lbl_804D7D24;
extern f32 lbl_804D7D28;
extern f32 lbl_804D7D2C;
extern f32 lbl_804D7D30;
extern f64 lbl_804D7D38;
extern f32 lbl_804D7D40;
extern f32 lbl_804D7D44;
extern f32 lbl_804D7D48;
extern f32 lbl_804D7D4C;
extern f32 lbl_804D7D50;
extern f32 lbl_804D7D54;
extern f32 lbl_804D7D58;
extern f32 lbl_804D7D5C;
extern f32 lbl_804D7D60;
extern f32 lbl_804D7D64;
extern f32 lbl_804D7D68;
extern f32 lbl_804D7D6C;
extern f32 lbl_804D7D70;
extern f64 lbl_804D7D78;
extern f32 lbl_804D7D80;
extern f32 lbl_804D7D84;
extern f32 lbl_804D7D88;
extern f32 lbl_804D7D90;
extern f32 lbl_804D7D94;
extern f64 lbl_804D7D98;
extern f32 lbl_804D7DA0;
extern f32 lbl_804D7DA4;
extern f32 lbl_804D7DA8;
extern f32 lbl_804D7DAC;
extern f32 lbl_804D7DB0;
extern f32 lbl_804D7DB4;
extern f32 lbl_804D7DB8;
extern f32 lbl_804D7DBC;
extern f32 lbl_804D7DC0;
extern f64 lbl_804D7DC8;
extern f32 lbl_804D7DD0;

extern u32 lbl_803A59C8;
extern u32 lbl_803A6FA8;
extern u32 lbl_803A6FC8;
extern u32 lbl_803A6FE8;
extern u32 lbl_803A7008;
extern u32 lbl_803A7028;
extern u32 lbl_803A7048;

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

// 0x80157730: release everything the game session allocated.
void fn_80157730() {
    fn_8027E76C();
    fn_800FDA10();
    fn_80186754();
    fn_8028A388();
    fn_802C2D20();
    fn_802A3AB8();
    fn_80158F90();
    fn_80159B2C();
    fn_8015AA24();
    GrenadeRemoveAll();
    SltEffectKillAll();
    fn_8017B6C0();
    fn_8017E69C();
    fn_802698D0();
    fn_802876F0();
    fn_802A20A4();
    fn_802A815C();
    fn_802CEBAC();
    fn_801FE00C();
    fn_8021696C();
}

// 0x8015779C: the main in-game draw pass: 3D scene (including the mirror pass) plus the
// per-player HUD effect pools. dt is the frame time, mode selects the front-end/in-game path,
// view is the player/view index (-1 for the unused view) and flag suppresses the loading screen.
void fn_8015779C(f32 dt, int mode, int view, int flag) {
    int i;
    int n;

    if (fn_801B829C(view) == 0) {
        if (fn_80215B40(view) == 0) {
            lbl_804D4E34 = 0;
            if (view >= 0) {
                lbl_804D4E34 = &lbl_8046DDE8[view];
                if (lbl_804D4E34->state != 0) {
                    lbl_804D4E34->timer = lbl_804D4E34->timer + dt;
                    if (lbl_804D4E34->timer <= lbl_804D7CEC) {
                        i = (int)(lbl_804D7CF0 * (lbl_804D4E34->timer / lbl_804D7CEC));
                        lbl_804D4E34->alpha = 0xff - i;
                    } else {
                        lbl_804D4E34->state = 2;
                    }
                }
            }
            lbl_804D4E44 = 0;
            if (view >= 0) {
                lbl_804D4E44 = &lbl_8046DE38[view];
                if (lbl_804D4E44->state != 0) {
                    lbl_804D4E44->timer = lbl_804D4E44->timer + dt;
                    if (lbl_804D4E44->timer <= lbl_804D7CEC) {
                        i = (int)(lbl_804D7CF0 * (lbl_804D4E44->timer / lbl_804D7CEC));
                        lbl_804D4E44->alpha = 0xff - i;
                    } else {
                        lbl_804D4E44->state = 2;
                    }
                }
            }
            lbl_804D4E48 = 0;
            if (view >= 0) {
                lbl_804D4E48 = &lbl_8046DE88[view];
                if (lbl_804D4E48->state != 0) {
                    lbl_804D4E48->timer = lbl_804D4E48->timer + dt;
                    if (lbl_804D4E48->timer <= lbl_804D7CEC) {
                        i = (int)(lbl_804D7CF0 * (lbl_804D4E48->timer / lbl_804D7CEC));
                        lbl_804D4E48->alpha = 0xff - i;
                    } else {
                        lbl_804D4E48->state = 2;
                    }
                    if (lbl_804D7CF4 <= lbl_804D4E48->timer) {
                        lbl_804D4E48->textAlpha = lbl_804D7CF8;
                    } else {
                        lbl_804D4E48->textAlpha =
                            (0.0f - lbl_804D2F40) * (lbl_804D4E48->timer / lbl_804D2F44) +
                            lbl_804D2F40;
                    }
                    if (lbl_804D7CF8 < lbl_804D4E48->fade) {
                        lbl_804D4E48->fade = -(lbl_804D7CFC * dt - lbl_804D4E48->fade);
                        if (lbl_804D4E48->fade <= lbl_804D7CF8) {
                            lbl_804D4E48->fade = lbl_804D7CF8;
                            HostExtra((Actor*)lbl_804D4E48->obj);
                        }
                    }
                    if (lbl_804D4E48->sndA != -1) {
                        fn_8007EE6C(lbl_804D4E48->sndA, 0, 0,
                                    (int)(lbl_804D7CF0 * lbl_804D4E48->fade), 0, 0);
                    }
                    if (lbl_804D4E48->sndB != -1) {
                        fn_8007EE6C(lbl_804D4E48->sndB, (int)(lbl_804D7D00 * lbl_804D4E48->fade),
                                    0, 0, 0, 0);
                    }
                }
            }
            if (mode == 1) {
                if (lbl_804D2F28 != 0) {
                    if (lbl_804D4B98 != 0) {
                        fn_800FF294(0);
                        fn_800FD924(0);
                        fn_800499E8(dt, 0);
                        fn_8007F064(2, 0);
                    }
                    fn_800499E8(dt, 0xffffffff);
                }
                if (CreditsTipVisible() != 0) {
                    fn_8000BA8C(lbl_804D7CF8, lbl_804D7CF8, lbl_804D7D08, lbl_804D7D0C,
                                lbl_804D7D08, lbl_804D7D08);
                }
                fn_800FD924(0xffffffff);
                fn_801EAEAC();
                if (CreditsTipVisible() != 0) {
                    fn_80009E44(0, 0);
                }
                fn_8007F064(0, 0xffffffff);
                fn_801E4850();
                fn_801EAEAC();
            } else if (mode < 1 && -1 < mode) {
                if (lbl_804D4E48 == 0 || lbl_804D4E48->state == 0) {
                    if (lbl_804D4E44 == 0 || lbl_804D4E44->state == 0) {
                        if (lbl_804D4E34 == 0 || lbl_804D4E34->state == 0) {
                            n = ReflectUpdateCamera();
                            for (i = 0; i < n; i++) {
                                fn_80009E10();
                                if (ReflectRender(i) != 0) {
                                    fn_800857A8();
                                    fn_80009E10();
                                    fn_8001D858();
                                    fn_8001D258();
                                    fn_8001D040();
                                    fn_8007AE7C();
                                    fn_802A3BEC();
                                    fn_801E092C();
                                    fn_8017E5BC();
                                    fn_801590A4();
                                    fn_80009E10();
                                    fn_80159C0C();
                                    fn_8015AB3C();
                                    fn_80122590();
                                    fn_8007AE7C();
                                    fn_80009E10();
                                    fn_8007AFBC();
                                    fn_800F0ED4();
                                    fn_80009E10();
                                    fn_801A001C();
                                }
                            }
                            fn_800C5E9C(0);
                            fn_8001D858();
                            if (flag == 0) {
                                fn_8001CE28();
                            }
                            fn_800857A8();
                            fn_80009E10();
                            fn_800C5E9C(0);
                            fn_8001D258();
                            fn_80228688();
                            fn_8028A4BC();
                            fn_802C2E5C();
                            if (flag == 0) {
                                fn_802886B4();
                                fn_800C5E9C(0);
                            }
                            fn_8001D040();
                            fn_8007AE7C();
                            fn_802C2D5C();
                            fn_801E092C();
                            fn_8017E5BC();
                            fn_801590A4();
                            fn_80009E10();
                            fn_80159C0C();
                            fn_8015AB3C();
                            fn_80122590();
                            fn_8018D2D4();
                            fn_8007AE7C();
                            fn_80234900();
                            fn_80200630();
                            fn_801D60B4(dt);
                            fn_8002756C();
                            fn_8007902C();
                            fn_8007CF04();
                            fn_8016F8EC();
                            fn_800BB3A0();
                            fn_800B9520();
                            fn_80009E10();
                            fn_8007AFBC();
                            if (view != -1) {
                                fn_80008F64();
                            }
                        } else {
                            HudDraw hud;
                            int obj = fn_800C6448();

                            fn_80093F88(0xfffffffd, 0, 0, 0, 0xff);
                            fn_802154B0(0xff);
                            memset(&hud, 0, 0x210);
                            hud.unk04 = 1;
                            hud.srcX = *(f32*)(obj + 0xc) + lbl_804D7D04;
                            hud.srcYi = *(s32*)(obj + 0x10);
                            hud.srcZ = *(f32*)(obj + 0x14) - lbl_804D7D04;
                            hud.unk10 = 1;
                            hud.unk18 = 1;
                            hud.unk1C = 1;
                            hud.color[0] = 0xff;
                            hud.color[1] = 0xff;
                            hud.color[2] = 0xff;
                            hud.color[3] = 0xff;
                            hud.unk30 = 0;
                            hud.unk3C = 0;
                            hud.colR = lbl_804D7D08;
                            hud.colG = lbl_804D7D08;
                            hud.colB = lbl_804D7D08;
                            hud.colA = lbl_804D7D08;
                            hud.unk68 = -32;
                            hud.dstX = hud.srcX;
                            hud.dstYi = hud.srcYi;
                            hud.dstZ = hud.srcZ;
                            fn_800C5E9C(0);
                            hud.obj = lbl_804D4E34->objA;
                            SendMsg(*(s32*)((u8*)hud.obj + 0x38), 0x7300, &hud);
                            hud.obj = lbl_804D4E34->objB;
                            SendMsg(*(s32*)((u8*)hud.obj + 0x38), 0x7300, &hud);
                            fn_80009E10();
                            fn_800F0ED4();
                            fn_80096C74(lbl_804D2F2C, lbl_804D2F30);
                            for (i = 0; i < lbl_804D2F38; i++) {
                                fn_80097DD0(lbl_804D2F34, 0);
                            }
                            if (lbl_804D2F3C != 0) {
                                fn_80096DD0(0xff, 0xff, 0xff, 0xc0);
                            }
                            if (lbl_804D4E34->state == 1) {
                                u32 a = lbl_804D4E34->alpha & 0xff;
                                fn_8009851C(a, a, a);
                            }
                        }
                    } else {
                        HudDraw hud;
                        int obj = fn_800C6448();

                        memset(&hud, 0, 0x210);
                        hud.unk04 = 1;
                        hud.srcX = *(f32*)(obj + 0xc) + lbl_804D7D04;
                        hud.srcYi = *(s32*)(obj + 0x10);
                        hud.srcZ = *(f32*)(obj + 0x14) - lbl_804D7D04;
                        hud.unk10 = 1;
                        hud.unk18 = 1;
                        hud.unk1C = 1;
                        hud.color[0] = 0xff;
                        hud.color[1] = 0xff;
                        hud.color[2] = 0xff;
                        hud.color[3] = 0xff;
                        hud.unk30 = 0;
                        hud.unk3C = 0;
                        hud.colR = lbl_804D7D08;
                        hud.colG = lbl_804D7D08;
                        hud.colB = lbl_804D7D08;
                        hud.colA = lbl_804D7D08;
                        hud.unk68 = -32;
                        hud.dstX = hud.srcX;
                        hud.dstYi = hud.srcYi;
                        hud.dstZ = hud.srcZ;
                        fn_800C5E9C(0);
                        obj = (int)lbl_804D4E44->objB;
                        if (obj != 0) {
                            hud.obj = (void*)obj;
                            SendMsg(*(s32*)(obj + 0x38), 0, &hud);
                        }
                        obj = (int)lbl_804D4E44->objA;
                        if (obj != 0) {
                            hud.obj = (void*)obj;
                            SendMsg(*(s32*)(obj + 0x38), 0x7300, &hud);
                        }
                        fn_80009E10();
                        fn_800F0ED4();
                        if (lbl_804D4E44->state == 1) {
                            u32 a = lbl_804D4E44->alpha & 0xff;
                            fn_8009851C(a, a, a);
                        }
                    }
                } else {
                    HudDraw hud;
                    int obj = fn_800C6448();

                    if (view >= 0) {
                        lbl_804D4E48 = &lbl_8046DE88[view];
                        fn_80093F88(0xfffffffd, 0, 0, 0, 0xff);
                        fn_802154B0((int)lbl_804D4E48->textAlpha);
                    }
                    memset(&hud, 0, 0x210);
                    hud.unk04 = 1;
                    hud.srcX = *(f32*)(obj + 0xc) + lbl_804D7D04;
                    hud.srcYi = *(s32*)(obj + 0x10);
                    hud.srcZ = *(f32*)(obj + 0x14) - lbl_804D7D04;
                    hud.unk10 = 1;
                    hud.unk18 = 1;
                    hud.unk1C = 1;
                    hud.color[0] = 0xff;
                    hud.color[1] = 0xff;
                    hud.color[2] = 0xff;
                    hud.color[3] = 0xff;
                    hud.unk30 = 0;
                    hud.unk3C = 0;
                    hud.colR = lbl_804D7D08;
                    hud.colG = lbl_804D7D08;
                    hud.colB = lbl_804D7D08;
                    hud.colA = lbl_804D7D08;
                    hud.unk68 = -32;
                    hud.dstX = hud.srcX;
                    hud.dstYi = hud.srcYi;
                    hud.dstZ = hud.srcZ;
                    fn_800C5E9C(0);
                    obj = (int)lbl_804D4E48->obj;
                    if (obj != 0) {
                        hud.obj = (void*)obj;
                        SendMsg(*(s32*)(obj + 0x38), 0x7300, &hud);
                    }
                    fn_80009E10();
                    fn_800F0ED4();
                    if (lbl_804D4E48->state == 1) {
                        u32 a = lbl_804D4E48->alpha & 0xff;
                        fn_8009851C(a, a, a);
                    }
                    fn_801D60B4(dt);
                    if (view != -1) {
                        fn_80008F64();
                    }
                }
                fn_8007F064(1, view);
                if (lbl_804D4E34 == 0 || lbl_804D4E34->state == 0) {
                    fn_800F4F0C();
                    fn_80049948();
                    if (fn_80049750() == 0) {
                        fn_800F4854();
                        fn_8017B164();
                    }
                }
                fn_800C5E9C(0);
                fn_800F0ED4();
                fn_80009E10();
                if (lbl_804D4E34 == 0 || lbl_804D4E34->state == 0) {
                    fn_800F4EE0();
                }
                fn_800C5E9C(0);
                fn_800F0DFC();
                fn_80009E10();
                if (lbl_804D4E34 == 0 || lbl_804D4E34->state == 0) {
                    fn_800F4ACC();
                }
                fn_800C5E9C(0);
                fn_800F0D24();
                fn_80009E10();
                if (fn_80220A38() != 4) {
                    fn_80215A24();
                }
                if (lbl_804D2F28 != 0 && lbl_804D4B98 == 0) {
                    fn_800FF294(view);
                    fn_800FD924(view);
                    fn_800499E8(dt, view);
                    fn_8007F064(2, view);
                }
            }
            if (view == -1) {
                fn_80008F58();
            }
        } else if (view == -1) {
            fn_8007F064(0, 0xffffffff);
        }
    }
}

// 0x801581DC: per-frame update of every game subsystem.
void fn_801581DC(f32 dt) {
    lbl_804D4E30 = dt;
    fn_800F39E0();
    fn_800F6264(dt);
    fn_80185EE4(dt);
    fn_8028A554(dt);
    fn_802C2F5C(dt);
    fn_80228A0C(dt);
    fn_802A3BF0(dt);
    fn_80159118(dt);
    fn_80159F88(dt);
    fn_8015AB90(dt);
    fn_80160B3C(dt);
    fn_8016BEF8(dt);
    fn_8017B488(dt);
    fn_8017E288(dt);
    fn_8018D79C(dt);
    fn_802CEAAC(dt);
    fn_800FF530(dt);
    fn_80200650(dt);
    fn_800FD998(dt);
    fn_80153C10(dt);
    fn_800DE0C4(dt);
}

// 0x801582AC: load the ghost body-part models.
void fn_801582AC() {
    int i;
    int n;
    int model;
    int part;

    fn_802845CC();
    fn_800FDA90();
    fn_800F45F8();
    fn_800F671C();
    model = fn_8007EB34(0xffffffff, 0x1ddb1c20);
    if (model != -1) {
        part = fn_8007EB34(0xffffffff, 0x4a41da93);
        fn_800D35B0(part, model, "Shadow_ghost_move");
        part = fn_8007EB34(0xffffffff, 0xdb039652);
        fn_800D35B0(part, model, "Ghost_head");
        part = fn_8007EB34(0xffffffff, 0x7344bd62);
        fn_800D35B0(part, model, "Ghost_torso01");
        part = fn_8007EB34(0xffffffff, 0x7344bd63);
        fn_800D35B0(part, model, "Ghost_torso02");
        part = fn_8007EB34(0xffffffff, 0x2faf3396);
        fn_800D35B0(part, model, "Ghost_lleg02");
        part = fn_8007EB34(0xffffffff, 0x39ec4750);
        fn_800D35B0(part, model, "Ghost_rleg02");
        part = fn_8007EB34(0xffffffff, 0x2f1a3064);
        fn_800D35B0(part, model, "Ghost_larm02");
        part = fn_8007EB34(0xffffffff, 0x3957441e);
        fn_800D35B0(part, model, "Ghost_rarm02");
        part = fn_8007EB34(0xffffffff, 0x7344bd62);
        fn_800D35B0(part, model, "Ghost_torso01");
        part = fn_8007EB34(0xffffffff, 0x2faf3395);
        fn_800D35B0(part, model, "Ghost_lleg01");
        part = fn_8007EB34(0xffffffff, 0x39ec474f);
        fn_800D35B0(part, model, "Ghost_rleg01");
        part = fn_8007EB34(0xffffffff, 0x2f1a3063);
        fn_800D35B0(part, model, "Ghost_larm01");
        part = fn_8007EB34(0xffffffff, 0x3957441d);
        fn_800D35B0(part, model, "Ghost_rarm01");
    }
    n = fn_800FD3EC();
    for (i = 0; i != n; i++) {
        fn_800C5E5C();
    }
}

// 0x801584F8: initialise the game session.
void fn_801584F8() {
    int r;

    lbl_804D4E5C = 0;
    lbl_804D4E60 = 0;
    lbl_804D4E64 = 0;
    lbl_804D4E68 = 0;
    lbl_804D4E98 = 0;
    lbl_804D4E6C = 0;
    lbl_804D4E88 = 0;
    lbl_804D4E74 = 0;
    lbl_804D4E7C = 0;
    lbl_804D4E70 = 0;
    lbl_804D4E84 = 0;
    lbl_804D4E94 = 0;
    lbl_804D4E8C = 0;
    lbl_804D4E90 = 0;
    lbl_804D4E78 = 0;
    lbl_804D4E80 = 0;
    fn_802846F0();
    fn_8028A6B8();
    fn_802C301C();
    fn_802A3CE0();
    fn_801592E8();
    GrenadeLoadTexture();
    fn_80159FEC();
    fn_8015ABB0();
    SltAssetInit();
    fn_80156AA0();
    fn_80186314();
    fn_800FDC48();
    fn_8017B7A0();
    fn_8017E728();
    fn_801849DC();
    fn_801FE008();
    fn_8013ECCC();
    fn_802177F4();
    r = fn_802A1D04();
    if (r == 0 && (r = fn_800DE2F8(5, 0)) >= 0 && (r = fn_800DE2F8(9, r)) != 0) {
        fn_800DE2F8(3, 1);
    }
    fn_800DE9D8();
    fn_800C5654(0x2000000, 0x5d46c0a8, 0x2000000);
    fn_800C5654(0x4e000000, 0x5d46c0a8, 0x4e000000);
}

// 0x8015860C: shut the game session down.
void fn_8015860C() {
    int i;

    fn_801EA87C();
    fn_80284CB0();
    fn_801116E4();
    fn_8028A8D4();
    fn_80120D60();
    fn_801868AC();
    fn_800FF66C();
    fn_800FDC78();
    fn_80156AA4();
    fn_801764F0();
    fn_8028A728();
    fn_802C30A0();
    fn_802A3D50();
    fn_801593A4();
    fn_8015A050();
    fn_8015ABB4();
    fn_80160B80();
    fn_8016C198();
    fn_800F6804();
    fn_8017B7A4();
    fn_8017E72C();
    fn_80269944();
    fn_80287764();
    fn_802A214C();
    fn_802A8204();
    fn_802CECE8();
    for (i = 0; i < 4; i++) {
        lbl_8046DDE8[i].state = 0;
        lbl_8046DDE8[i].objA = 0;
        lbl_8046DDE8[i].objB = 0;
    }
    for (i = 0; i < 4; i++) {
        lbl_8046DE38[i].state = 0;
        lbl_8046DE38[i].objA = 0;
        lbl_8046DE38[i].objB = 0;
    }
    lbl_804D4E34 = 0;
    lbl_804D4E44 = 0;
    for (i = 0; i < 4; i++) {
        lbl_8046DE88[i].state = 0;
        lbl_8046DE88[i].obj = 0;
    }
    lbl_804D4E48 = 0;
    fn_801FE068();
    fn_80217AFC();
}

// 0x80158744: check for a save file and create one if needed.
void fn_80158744() {
    lbl_804D49C4 = fn_80045524(0x420, 1, 0x38);
    if (lbl_804D49C4 == 1) {
        fn_80158834();
    } else if (lbl_804D49C4 > 0 && lbl_804D49C4 < 3 && (fn_800DE2F8(7, 0x30), lbl_804D49C4 == 1)) {
        fn_80158834();
    }
    fn_8027E8F8();
}

// 0x801587B8: the same check with a caller-supplied mode.
void fn_801587B8(int mode) {
    lbl_804D49C4 = fn_80045524(0x420, 1, mode | 8);
    if (lbl_804D49C4 == 1) {
        fn_80158834();
    } else if (lbl_804D49C4 > 0 && lbl_804D49C4 < 3 && (fn_800DE2F8(7, mode), lbl_804D49C4 == 1)) {
        fn_80158834();
    }
}

// 0x80158834: "no save file" prompt; waits for a button before creating one.
void fn_80158834() {
    f32 wait;
    f32 elapsed;
    u32 msg;
    int r;
    int r2;

    elapsed = lbl_804D7D10;
    msg = lbl_804D7CE8;
    fn_800EC80C(0x32, 0x96, 0x21c, 0x7d);
    LoadingTime();
    do {
        float now = LoadingTime();
        elapsed = elapsed - now;
        fn_80005A64();
        fn_800ECB18();
        fn_800EC9DC(0x32, 0x96, 0x21c, 0x7d);
        {
            u32 arg = msg;
            fn_801EA880((void*)lbl_804D2F48, &arg, 0x50, 0x82, 0xe, 0x10, 0);
        }
        fn_800EBBB8(now, 0x104, 0x45);
        fn_801EAEAC();
        fn_80005A64();
        if (elapsed <= lbl_804D7CF8) {
            break;
        }
        r = fn_800059D0(0x15);
        if (r == 1) {
            break;
        }
        r2 = fn_800059D0(0x14);
    } while (r2 != 1);
    fn_800EC614(0x32, 0x96, 0x21c, 0x7d);
    fn_800ECB18();
    fn_800DE2F8(1, 0);
    r = fn_800DE2F8(6, 0);
    lbl_804D49C4 = (r == 1) ? 2 : 0;
}

// 0x80158990: start the focus/lighten HUD effect on a player.
void fn_80158990(int player, int obj) {
    if (player < 0) {
        return;
    }
    lbl_804D4E48 = &lbl_8046DE88[player];
    if (lbl_804D4E48->state != 0) {
        return;
    }
    lbl_804D4E48->state = 1;
    lbl_804D4E48->timer = lbl_804D7CF8;
    lbl_804D4E48->obj = (void*)obj;
    lbl_804D4E48->textAlpha = lbl_804D7CF0;
    if (lbl_804D4E48->sndA != -1) {
        lbl_804D4E48->sndA = fn_8007EF48(player, 1, "Lighten Loop", 0, 0, 0, 0, 0, 0, 0);
    }
    if (lbl_804D4E48->sndB != -1) {
        lbl_804D4E48->sndB = fn_8007EF48(player, 1, "Focus Loop", 0, 0, 0, 0, 0, 0, 0);
    }
    lbl_804D4E48->fade = lbl_804D7D08;
}

// 0x80158AAC: start the second HUD effect pair for a player.
void fn_80158AAC(int player, int a, int b) {
    if (player < 0) {
        return;
    }
    lbl_804D4E44 = &lbl_8046DE38[player];
    if (lbl_804D4E44->state != 0) {
        return;
    }
    int s = 1;
    f32 t = lbl_804D7CF8;
    lbl_804D4E44->state = s;
    lbl_804D4E44->timer = t;
    lbl_804D4E44->objA = (void*)a;
    lbl_804D4E44->objB = (void*)b;
}

// 0x80158AFC: is this object id one of the current HUD effect objects?
int fn_80158AFC(int id) {
    if (lbl_804D4E34 != 0 && lbl_804D4E34->state != 0) {
        if (id == *(s32*)((u8*)lbl_804D4E34->objA + 0x38)) {
            return 1;
        }
        if (id == *(s32*)((u8*)lbl_804D4E34->objB + 0x38)) {
            return 1;
        }
    }
    return 0;
}

// 0x80158B4C: stop the first HUD effect for a player.
void fn_80158B4C(int player) {
    if (player < 0) {
        return;
    }
    lbl_804D4E34 = &lbl_8046DDE8[player];
    if (lbl_804D4E34->state == 0) {
        return;
    }
    lbl_804D4E34->state = 0;
    lbl_804D4E34->objA = 0;
    lbl_804D4E34->objB = 0;
}

// 0x80158B90: start the first HUD effect pair for a player.
void fn_80158B90(int player, int a, int b) {
    if (player < 0) {
        return;
    }
    lbl_804D4E34 = &lbl_8046DDE8[player];
    if (lbl_804D4E34->state != 0) {
        return;
    }
    lbl_804D4E34->state = 1;
    lbl_804D4E34->timer = lbl_804D7CF8;
    lbl_804D4E34->objA = (void*)a;
    lbl_804D4E34->objB = (void*)b;
}

// 0x80158BE0: is any player HUD effect running?
int fn_80158BE0() {
    FxPoolEntry* p = lbl_8046DDE8;

    if ((p++)->state != 0) {
        return 1;
    }
    if ((p++)->state != 0) {
        return 1;
    }
    if ((p++)->state != 0) {
        return 1;
    }
    if (p->state != 0) {
        return 1;
    }
    return 0;
}

// ---- effect pool handle arrays ----
// 0x80158F90: destroy the focus effect pool.
void fn_80158F90() {
    while ((u32)lbl_804D4EA0->size >> 2 != 0) {
        if (*(u32*)((u8*)*lbl_804D4EA0->data + 0x2c) != 0) {
            VFree(*(void**)((u8*)*lbl_804D4EA0->data + 0x2c));
            *(s32*)((u8*)*lbl_804D4EA0->data + 0x2c) = 0;
        }
        VFree(*lbl_804D4EA0->data);
        fn_8028B35C(lbl_804D4EA0, lbl_804D4EA0->data, 4);
    }
    delete lbl_804D4EA0;
    fn_80233848();
}

// 0x8015902C: destroy part of the focus effect pool object.
void* fn_8015902C(void* p, short freeIt) {
    if (p != 0) {
        *(void**)p = &lbl_803A6FC8;
        if (p != 0) {
            *(void**)p = &lbl_803A6FA8;
            fn_8028B960(p);
            fn_8028B9C0(p, 0);
        }
        if (freeIt > 0) {
            VFree(p);
        }
    }
    return p;
}

// 0x801590A4: update every focus effect.
void fn_801590A4() {
    void** p;

    p = lbl_804D4EA0->data;
    while (p != (void**)((u8*)lbl_804D4EA0->data + lbl_804D4EA0->size)) {
        DeferAdd(2, (Vec3*)(*(int*)p + 4), (void* (*)(void*))fn_80159464, *p);
        p = p + 1;
    }
    fn_80233928();
}

// 0x801592E8: look up the effect type/part hashes.
void fn_801592E8() {
    u16 list[14];
    u8* rec;
    int i;
    int j;

    rec = (u8*)&lbl_803A59C8;
    for (i = 0; i < 8; i++) {
        *(s32*)(rec + 8) = (s32)fn_8007EB34(0x92234D44, *(u32*)(rec + 4));
        *(u16*)(rec + 0xe) = 0;
        {
            u8* part = rec;
            u16* out = list;
            for (j = 0; j < *(u16*)(rec + 0xc); j++) {
                *(s32*)(part + 0x98) = (s32)fn_8007EB34(0xF2E2BEB8, *(u32*)(part + 0x94));
                {
                    s16 total = *(s16*)(rec + 0xe);
                    s32 count = *(s32*)(part + 0x14);
                    s32 hash = *(s32*)(part + 0x98);
                    part += 0x88;
                    *out = (u16)hash;
                    out++;
                    *(s16*)(rec + 0xe) = total + (s16)count;
                }
            }
        }
        fn_8000F738(list);
        rec += 0x2bc;
    }
    fn_80233B60();
}

// 0x801593A4: create the focus effect pool.
void* fn_801593A4() {
    void* p = VAlloc(0x14);

    if (p != 0) {
        fn_8028BA9C(p);
        *(void**)p = &lbl_803A6FA8;
        *(void**)p = &lbl_803A6FC8;
    }
    lbl_804D4EA0 = (ResArray*)p;
    fn_80233C30();
    return p;
}

// 0x801593FC: destroy the focus effect pool object.
void* fn_801593FC(void* p, short freeIt) {
    if (p != 0) {
        *(void**)p = &lbl_803A6FA8;
        fn_8028B960(p);
        fn_8028B9C0(p, 0);
        if (freeIt > 0) {
            VFree(p);
        }
    }
    return p;
}

// 0x80159A04 / 0x80159A4C and siblings: pool element array helpers.
void fn_80159A04(void* pool, int start, unsigned size) {
    fn_8028BC2C(pool);
    fn_80159AB4((int*)start, (int*)(start + (size & 0xfffffffc)));
}

void fn_80159A4C(void* pool, char* start, int size) {
    fn_80159AD4((int*)start, (int*)(start + (size & 0xfffffffc)));
    fn_8028BC28(pool, start, size);
}

int fn_80159AAC() {
    return 4;
}

void fn_80159AB4(int* p, int* end) {
    while (p < end) {
        p++;
    }
}

void fn_80159AD4(int* p, int* end) {
    while (p < end) {
        p++;
    }
}

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
void* fn_8015A050() {
    void* p = VAlloc(0x14);

    if (p != 0) {
        fn_8028BA9C(p);
        *(void**)p = &lbl_803A6FE8;
        *(void**)p = &lbl_803A7008;
    }
    lbl_804D4EA8 = (ResArray*)p;
    return p;
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
void fn_8015A10C(int entry) {
    void** p;
    void** end;

    p = lbl_804D4EA8->data;
    if (entry != 0) {
        end = (void**)((u8*)lbl_804D4EA8->data + lbl_804D4EA8->size);
        for (; p != end; p = p + 1) {
            if ((void*)entry == *p) {
                if (*(s32*)(entry + 0x6c) == 1) {
                    fn_800390F4(*(s32*)(entry + 0x50));
                }
                VFree((void*)entry);
                fn_8028B35C(lbl_804D4EA8, p, 4);
                return;
            }
        }
    }
}

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
    *(f32*)((u8*)dst + 0x54) = lbl_804D7D40;
    *(u32*)((u8*)dst + 0x60) = lbl_804D7D40;
    *(u32*)((u8*)dst + 0x64) = lbl_804D7D40;
    *(u32*)((u8*)dst + 0x68) = lbl_804D7D40;
}

// 0x8015A934 / 0x8015A97C and siblings: grenade pool element array helpers.
void fn_8015A934(void* pool, int start, unsigned size) {
    fn_8028BC2C(pool);
    fn_8015A9E4((int*)start, (int*)(start + (size & 0xfffffffc)));
}

void fn_8015A97C(void* pool, int start, unsigned size) {
    fn_8015AA04((int*)start, (int*)(start + (size & 0xfffffffc)));
    fn_8028BC28(pool, (void*)start, size);
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
void* fn_8015ABB4() {
    void* p = VAlloc(0x14);

    if (p != 0) {
        fn_8028BA9C(p);
        *(void**)p = &lbl_803A7028;
        *(void**)p = &lbl_803A7048;
    }
    lbl_804D4EB0 = (ResArray*)p;
    return p;
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

// 0x8015BF90 / 0x8015BFD8 and siblings: emitter pool element array helpers.
void fn_8015BF90(void* pool, int start, unsigned size) {
    fn_8028BC2C(pool);
    fn_8015C040((int*)start, (int*)(start + (size & 0xfffffffc)));
}

void fn_8015BFD8(void* pool, int start, unsigned size) {
    fn_8015C060((int*)start, (int*)(start + (size & 0xfffffffc)));
    fn_8028BC28(pool, (void*)start, size);
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

// ---- not yet reconstructed ----
void fn_80159464(int*) {}
void fn_80158C3C() {}
void fn_80159118(f32) {}
void fn_80159554() {}
void fn_80159C0C() {}
void fn_80159D48(f32*) {}
void fn_8015A1A0(f32, int) {}
int fn_8015A570(f32, f64, u8, u8, u8, u8, u8, u8, u8, u8, int, int) { return 0; }
int fn_8015A78C(u8, u8, u8, u8, u8, u8, u8, u8) { return 0; }
void fn_8015AC70() {}
void fn_8015AF74() {}
int fn_8015B2A0(f32, void*, u8, u8, u8, u8, u8, u8) { return 0; }
int fn_8015B428(f32, void*, int, u8, u8, u8, u8, u8, u8) { return 0; }
void fn_8015B5B4(int) {}
void fn_8015BA5C(f32) {}

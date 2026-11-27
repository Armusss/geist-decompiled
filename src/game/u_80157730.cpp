// u_80157730.cpp: the master game file (0x80157730-0x80158C3C). It boots and tears down every
// game subsystem, runs the per-frame update and draw passes, shows the save-file prompt and
// manages the shared per-player HUD/effect pools (small handle arrays in .sbss).
// Names are descriptive guesses; the code is matched against the retail DOL.
// StoreCheck__FiPCcUl keeps its symbols.txt prototype; the callers here pass 1 as its second argument.

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

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// --- engine functions with C++ linkage (mangled names in symbols.txt) ---
int ReflectUpdateCamera();
int SaveCommand(int cmd, int arg);
void RoomsFrame();
int StoreCheck(int mode, const char* name, unsigned long flags);
void SaveUpdate(f32);
void SaveRestoreBlocks();
void WeatherInit();
void WeatherLatchInit();
void WeatherBuildTable();
void WeatherReset();
void StatReset();
void HazardDebugDrawSpheres();
void GhostsTick(f32);
void GhostSystemInit();
void GhostSystemReset();
void EffectListFreeAll();
void EffectListUpdate(f32);
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

extern "C" {
void AudioNop();
void fn_801EA880(void*, void*, int, int, int, int, int);
void fn_801EAEAC();
void memset(void*, int, u32);
int SendMsg(int to, int msg, void* data);
void rand();

int fn_800059D0(int);
void fn_80005A64();
void fn_80008F58();
void fn_80008F64();
void fn_80009E10();
void fn_80009E44(int, int);
void fn_8000BA8C(f32, f32, f32, f32, f32, f32);
void fn_8001CE28();
void fn_8001D040();
void fn_8001D258();
void fn_8002756C();
void fn_80027690(int);
int fn_80027A58(int, void*, void*, int, int);
void fn_800390F4(int);
int fn_800391DC(int);
void fn_8003F15C(f32, int, void*, int, int, int);
void fn_800499E8(f64, int);
void fn_8007902C();
void fn_8007AE7C();
void fn_8007AFBC();
void fn_800857A8();
void fn_80093F88(int, u8, u8, u8, u8);
void fn_80096C74(f64, f64);
void fn_80096DD0(int, int, int, int);
void fn_80097DD0(f64, int);
void fn_8009851C(u32, u32, u32);
void fn_8009EC5C(void*, void*);
void fn_8009F7A8(void*, void*);
void fn_800B9520();
void fn_800BB3A0();
void fn_800C5E5C();
void fn_800C5E9C(int);
int fn_800C6448();
void fn_800EBBB8(f64, int, int);
void fn_800EC614(int, int, int, int);
void fn_800EC80C(int, int, int, int);
void fn_800EC9DC(int, int, int, int);
void fn_800ECB18();
void fn_800F39E0();
void fn_800F4ACC();
void fn_800F4EE0();
void fn_800F4F0C();
void fn_800F6264(f32);
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
void fn_801116E4();
void fn_8013ECCC();
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
void fn_802A3BEC();
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

// Defined here: the compiler lays the three pools out contiguously and addresses them from one base.
FxPoolEntry lbl_8046DDE8[4];
FxPoolEntry lbl_8046DE38[4];
FocusFx lbl_8046DE88[4];


// The object a HUD draw request is sent to (its message id at 0x38).
struct HudObj {
    u8 pad00[0x38];
    s32 id;          // 0x38
};

// The current 2D placement returned by fn_800C6448 (position at 0x0C).
struct HudPlace {
    u8 pad00[0x0C];
    f32 x, y, z;     // 0x0C
};

// A 2D draw request posted to an object with message 0x7300.
struct HudDraw {
    HudObj* obj;     // 0x00
    s32 unk04;       // 0x04
    s32 unk08;       // 0x08
    s32 unk0C;       // 0x0C
    s32 unk10;       // 0x10
    s32 unk14;       // 0x14
    s32 unk18;       // 0x18
    s32 unk1C;       // 0x1C
    u8 color[4];     // 0x20
    f32 srcX;        // 0x24
    f32 srcY;        // 0x28
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
    f32 dstY;        // 0x60
    f32 dstZ;        // 0x64
    s32 unk68;       // 0x68
    u8 pad6C[0x210 - 0x6C];
};

static inline void HudDrawInit(HudDraw* hud, HudPlace* place) {
    f32 z, y, x;

    memset(hud, 0, sizeof(HudDraw));
    hud->unk04 = 1;
    x = place->x;
    y = place->y;
    z = place->z;
    x += 0.2f;
    z -= 0.2f;
    hud->unk10 = 1;
    hud->unk18 = 1;
    hud->unk1C = 1;
    hud->color[0] = 0xff;
    hud->color[1] = 0xff;
    hud->color[2] = 0xff;
    hud->color[3] = 0xff;
    hud->srcX = x;
    hud->srcY = y;
    hud->srcZ = z;
    hud->unk30 = 0;
    hud->unk3C = 0;
    hud->colR = 1.0f;
    hud->colG = 1.0f;
    hud->colB = 1.0f;
    hud->colA = 1.0f;
    hud->dstX = x;
    hud->dstY = y;
    hud->dstZ = z;
    hud->unk68 = -32;
}

// --- data ---
// .sdata
s32 lbl_804D2F28 = 1;
f32 lbl_804D2F2C = 1.2f;
f32 lbl_804D2F30 = 0.5f;
f32 lbl_804D2F34 = 0.4f;
s32 lbl_804D2F38 = 1;
s32 lbl_804D2F3C = 1;
f32 lbl_804D2F40 = 255.0f;
f32 lbl_804D2F44 = 4000.0f;
// The save-file prompt text.
const char* lbl_804D2F48 =
    "NO GEIST SAVE FILE FOUND.  A NEW\nGEIST SAVE FILE WILL NOW BE CREATED.\n                  OK";

// .sbss
f32 lbl_804D4E30;
FxPoolEntry* lbl_804D4E34;
s32 lbl_804D4E38, lbl_804D4E3C, lbl_804D4E40;
FxPoolEntry* lbl_804D4E44;
FocusFx* lbl_804D4E48;
s32 lbl_804D4E4C, lbl_804D4E50, lbl_804D4E54;
f32 lbl_804D4E58;
s32 lbl_804D4E5C, lbl_804D4E60, lbl_804D4E64, lbl_804D4E68, lbl_804D4E6C, lbl_804D4E70;
s32 lbl_804D4E74, lbl_804D4E78, lbl_804D4E7C, lbl_804D4E80, lbl_804D4E84, lbl_804D4E88;
s32 lbl_804D4E8C, lbl_804D4E90, lbl_804D4E94, lbl_804D4E98;

extern s32 gSaveLoaded;
extern s32 gCamLetterbox;

// The prompt text colour (.sdata2, ahead of the literal pool).
struct PromptColor {
    u32 rgba;
};
const PromptColor lbl_804D7CE8 = {0xC8C8C8FF};

extern "C" {


// 0x80157730: release everything the game session allocated.
void fn_80157730() {
    fn_8027E76C();
    fn_800FDA10();
    fn_80186754();
    fn_8028A388();
    fn_802C2D20();
    EffectListFreeAll();
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
    if (fn_801B829C(view) != 0) {
        return;
    }
    if (fn_80215B40(view) != 0) {
        if (view == -1) {
            fn_8007F064(0, -1);
        }
        return;
    }
    lbl_804D4E34 = 0;
    if (view >= 0) {
        lbl_804D4E34 = &lbl_8046DDE8[view];
        if (lbl_804D4E34->state != 0) {
            lbl_804D4E34->timer += dt;
            if (lbl_804D4E34->timer > 333.0f) {
                lbl_804D4E34->state = 2;
            } else {
                lbl_804D4E34->alpha = 0xff - (int)(255.0f * (lbl_804D4E34->timer / 333.0f));
            }
        }
    }
    lbl_804D4E44 = 0;
    if (view >= 0) {
        lbl_804D4E44 = &lbl_8046DE38[view];
        if (lbl_804D4E44->state != 0) {
            lbl_804D4E44->timer += dt;
            if (lbl_804D4E44->timer > 333.0f) {
                lbl_804D4E44->state = 2;
            } else {
                lbl_804D4E44->alpha = 0xff - (int)(255.0f * (lbl_804D4E44->timer / 333.0f));
            }
        }
    }
    lbl_804D4E48 = 0;
    if (view >= 0) {
        lbl_804D4E48 = &lbl_8046DE88[view];
        if (lbl_804D4E48->state != 0) {
            lbl_804D4E48->timer += dt;
            if (lbl_804D4E48->timer > 333.0f) {
                lbl_804D4E48->state = 2;
            } else {
                lbl_804D4E48->alpha = 0xff - (int)(255.0f * (lbl_804D4E48->timer / 333.0f));
            }
            if (lbl_804D4E48->timer < 4000.0f) {
                lbl_804D4E48->textAlpha =
                    (lbl_804D4E58 - lbl_804D2F40) * (lbl_804D4E48->timer / lbl_804D2F44) + lbl_804D2F40;
            } else {
                lbl_804D4E48->textAlpha = lbl_804D4E58;
            }
            if (lbl_804D4E48->fade > 0.0f) {
                lbl_804D4E48->fade -= 0.0015f * dt;
                if (lbl_804D4E48->fade <= 0.0f) {
                    lbl_804D4E48->fade = 0.0f;
                    HostExtra((Actor*)lbl_804D4E48->obj);
                }
            }
            if (lbl_804D4E48->sndA != -1) {
                fn_8007EE6C(lbl_804D4E48->sndA, 0, 0, (int)(255.0f * lbl_804D4E48->fade), 0, 0);
            }
            if (lbl_804D4E48->sndB != -1) {
                fn_8007EE6C(lbl_804D4E48->sndB, (int)(20.0f * lbl_804D4E48->fade), 0, 0, 0, 0);
            }
        }
    }
    switch (mode) {
    case 0:
        if (lbl_804D4E48 != 0 && lbl_804D4E48->state != 0) {
            HudDraw hud;
            HudPlace* place = (HudPlace*)fn_800C6448();

            if (view >= 0) {
                lbl_804D4E48 = &lbl_8046DE88[view];
                fn_80093F88(-3, lbl_804D4E4C, lbl_804D4E50, lbl_804D4E54, 0xff);
                fn_802154B0((int)lbl_804D4E48->textAlpha);
            }
            HudDrawInit(&hud, place);
            fn_800C5E9C(0);
            if (lbl_804D4E48->obj != 0) {
                hud.obj = (HudObj*)lbl_804D4E48->obj;
                SendMsg(hud.obj->id, 0x7300, &hud);
            }
            fn_80009E10();
            DeferFlush3();
            if (lbl_804D4E48->state == 1) {
                u8 a = lbl_804D4E48->alpha;
                fn_8009851C(a, a, a);
            }
            fn_801D60B4(dt);
            if (view != -1) {
                fn_80008F64();
            }
        } else if (lbl_804D4E44 != 0 && lbl_804D4E44->state != 0) {
            HudDraw hud;
            HudPlace* place = (HudPlace*)fn_800C6448();

            HudDrawInit(&hud, place);
            fn_800C5E9C(0);
            if (lbl_804D4E44->objB != 0) {
                hud.obj = (HudObj*)lbl_804D4E44->objB;
                SendMsg(hud.obj->id, 0, &hud);
            }
            if (lbl_804D4E44->objA != 0) {
                hud.obj = (HudObj*)lbl_804D4E44->objA;
                SendMsg(hud.obj->id, 0x7300, &hud);
            }
            fn_80009E10();
            DeferFlush3();
            if (lbl_804D4E44->state == 1) {
                u8 a = lbl_804D4E44->alpha;
                fn_8009851C(a, a, a);
            }
        } else if (lbl_804D4E34 != 0 && lbl_804D4E34->state != 0) {
            HudDraw hud;
            HudPlace* place = (HudPlace*)fn_800C6448();
            int i;

            fn_80093F88(-3, lbl_804D4E38, lbl_804D4E3C, lbl_804D4E40, 0xff);
            fn_802154B0(0xff);
            HudDrawInit(&hud, place);
            fn_800C5E9C(0);
            hud.obj = (HudObj*)lbl_804D4E34->objA;
            SendMsg(hud.obj->id, 0x7300, &hud);
            hud.obj = (HudObj*)lbl_804D4E34->objB;
            SendMsg(hud.obj->id, 0x7300, &hud);
            fn_80009E10();
            DeferFlush3();
            fn_80096C74(lbl_804D2F2C, lbl_804D2F30);
            for (i = 0; i < lbl_804D2F38; i++) {
                fn_80097DD0(lbl_804D2F34, 0);
            }
            if (lbl_804D2F3C != 0) {
                fn_80096DD0(0xff, 0xff, 0xff, 0xc0);
            }
            if (lbl_804D4E34->state == 1) {
                u8 a = lbl_804D4E34->alpha;
                fn_8009851C(a, a, a);
            }
        } else {
            int n = ReflectUpdateCamera();
            int i;

            for (i = 0; i < n; i++) {
                fn_80009E10();
                if (ReflectRender(i) != 0) {
                    fn_800857A8();
                    fn_80009E10();
                    RoomsFrame();
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
                    HazardDebugDrawSpheres();
                    fn_8007AE7C();
                    fn_80009E10();
                    fn_8007AFBC();
                    DeferFlush3();
                    fn_80009E10();
                    ReflectEnd();
                }
            }
            fn_800C5E9C(0);
            RoomsFrame();
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
            HazardDebugDrawSpheres();
            GhostShotDrawAll();
            fn_8007AE7C();
            fn_80234900();
            fn_80200630();
            fn_801D60B4(dt);
            fn_8002756C();
            fn_8007902C();
            AudioNop();
            fn_8016F8EC();
            fn_800BB3A0();
            fn_800B9520();
            fn_80009E10();
            fn_8007AFBC();
            if (view != -1) {
                fn_80008F64();
            }
        }
        fn_8007F064(1, view);
        if (lbl_804D4E34 == 0 || lbl_804D4E34->state == 0) {
            fn_800F4F0C();
            if (PlayerInHost(GetPlayer()) == 0) {
                WeatherLatchInit();
                fn_8017B164();
            }
        }
        fn_800C5E9C(0);
        DeferFlush3();
        fn_80009E10();
        if (lbl_804D4E34 == 0 || lbl_804D4E34->state == 0) {
            fn_800F4EE0();
        }
        fn_800C5E9C(0);
        DeferFlush4();
        fn_80009E10();
        if (lbl_804D4E34 == 0 || lbl_804D4E34->state == 0) {
            fn_800F4ACC();
        }
        fn_800C5E9C(0);
        DeferFlush5();
        fn_80009E10();
        if (fn_80220A38() != 4) {
            fn_80215A24();
        }
        if (lbl_804D2F28 != 0 && gCamLetterbox == 0) {
            fn_800FF294(view);
            fn_800FD924(view);
            fn_800499E8(dt, view);
            fn_8007F064(2, view);
        }
        break;
    case 1:
        if (lbl_804D2F28 != 0) {
            if (gCamLetterbox != 0) {
                fn_800FF294(0);
                fn_800FD924(0);
                fn_800499E8(dt, 0);
                fn_8007F064(2, 0);
            }
            fn_800499E8(dt, -1);
        }
        if (CreditsTipVisible() != 0) {
            fn_8000BA8C(0.0f, 0.0f, 1.0f, 0.754f, 1.0f, 1.0f);
        }
        fn_800FD924(-1);
        fn_801EAEAC();
        if (CreditsTipVisible() != 0) {
            fn_80009E44(0, 0);
        }
        fn_8007F064(0, -1);
        fn_801E4850();
        fn_801EAEAC();
        break;
    }
    if (view == -1) {
        fn_80008F58();
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
    EffectListUpdate(dt);
    fn_80159118(dt);
    fn_80159F88(dt);
    fn_8015AB90(dt);
    GrenadeNop(dt);
    SltEffectUpdate(dt);
    fn_8017B488(dt);
    fn_8017E288(dt);
    GhostShotUpdateAll(dt);
    fn_802CEAAC(dt);
    fn_800FF530(dt);
    fn_80200650(dt);
    fn_800FD998(dt);
    GhostsTick(dt);
    SaveUpdate(dt);
}

// 0x801582AC: load the ghost body-part models.
void fn_801582AC() {
    int i;
    int n;
    int model;
    int part;

    fn_802845CC();
    fn_800FDA90();
    WeatherInit();
    WeatherBuildTable();
    model = (int)AssetFind(0xffffffff, 0x1ddb1c20);
    if (model != -1) {
        part = (int)AssetFind(0xffffffff, 0x4a41da93);
        EmitterAttach(part, model, "Shadow_ghost_move");
        part = (int)AssetFind(0xffffffff, 0xdb039652);
        EmitterAttach(part, model, "Ghost_head");
        part = (int)AssetFind(0xffffffff, 0x7344bd62);
        EmitterAttach(part, model, "Ghost_torso01");
        part = (int)AssetFind(0xffffffff, 0x7344bd63);
        EmitterAttach(part, model, "Ghost_torso02");
        part = (int)AssetFind(0xffffffff, 0x2faf3396);
        EmitterAttach(part, model, "Ghost_lleg02");
        part = (int)AssetFind(0xffffffff, 0x39ec4750);
        EmitterAttach(part, model, "Ghost_rleg02");
        part = (int)AssetFind(0xffffffff, 0x2f1a3064);
        EmitterAttach(part, model, "Ghost_larm02");
        part = (int)AssetFind(0xffffffff, 0x3957441e);
        EmitterAttach(part, model, "Ghost_rarm02");
        part = (int)AssetFind(0xffffffff, 0x7344bd62);
        EmitterAttach(part, model, "Ghost_torso01");
        part = (int)AssetFind(0xffffffff, 0x2faf3395);
        EmitterAttach(part, model, "Ghost_lleg01");
        part = (int)AssetFind(0xffffffff, 0x39ec474f);
        EmitterAttach(part, model, "Ghost_rleg01");
        part = (int)AssetFind(0xffffffff, 0x2f1a3063);
        EmitterAttach(part, model, "Ghost_larm01");
        part = (int)AssetFind(0xffffffff, 0x3957441d);
        EmitterAttach(part, model, "Ghost_rarm01");
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
    GhostSystemInit();
    fn_80186314();
    fn_800FDC48();
    fn_8017B7A0();
    fn_8017E728();
    fn_801849DC();
    fn_801FE008();
    fn_8013ECCC();
    fn_802177F4();
    r = fn_802A1D04();
    if (r == 0 && (r = SaveCommand(5, 0)) >= 0 && (r = SaveCommand(9, r)) != 0) {
        SaveCommand(3, 1);
    }
    SaveRestoreBlocks();
    ActorPlayAnim(0x2000000, 0x5d46c0a8, 0x2000000);
    ActorPlayAnim(0x4e000000, 0x5d46c0a8, 0x4e000000);
}

// 0x8015860C: shut the game session down.
void fn_8015860C() {
    int i;

    fn_801EA87C();
    fn_80284CB0();
    fn_801116E4();
    fn_8028A8D4();
    StatReset();
    fn_801868AC();
    fn_800FF66C();
    fn_800FDC78();
    GhostSystemReset();
    fn_801764F0();
    fn_8028A728();
    fn_802C30A0();
    fn_802A3D50();
    fn_801593A4();
    fn_8015A050();
    fn_8015ABB4();
    GrenadeInit();
    SltEffectCleanup();
    WeatherReset();
    fn_8017B7A4();
    fn_8017E72C();
    fn_80269944();
    fn_80287764();
    fn_802A214C();
    fn_802A8204();
    fn_802CECE8();
    lbl_804D4E34 = 0;
    for (i = 0; i < 4; i++) {
        lbl_8046DDE8[i].state = 0;
        lbl_8046DDE8[i].objA = 0;
        lbl_8046DDE8[i].objB = 0;
    }
    lbl_804D4E44 = 0;
    for (i = 0; i < 4; i++) {
        lbl_8046DE38[i].state = 0;
        lbl_8046DE38[i].objA = 0;
        lbl_8046DE38[i].objB = 0;
    }
    lbl_804D4E48 = 0;
    for (i = 0; i < 4; i++) {
        lbl_8046DE88[i].state = 0;
        lbl_8046DE88[i].obj = 0;
    }
    fn_801FE068();
    fn_80217AFC();
}

// 0x80158744: check for a save file and create one if needed.
void fn_80158744() {
    gSaveLoaded = StoreCheck(0x420, (const char*)1, 0x38);
    switch (gSaveLoaded) {
    case 0:
        break;
    case 1:
        fn_80158834();
        break;
    case 2:
        SaveCommand(7, 0x30);
        if (gSaveLoaded == 1) {
            fn_80158834();
        }
        break;
    }
    fn_8027E8F8();
}

// 0x801587B8: the same check with a caller-supplied mode.
void fn_801587B8(int mode) {
    gSaveLoaded = StoreCheck(0x420, (const char*)1, mode | 8);
    switch (gSaveLoaded) {
    case 0:
        break;
    case 1:
        fn_80158834();
        break;
    case 2:
        SaveCommand(7, mode);
        if (gSaveLoaded == 1) {
            fn_80158834();
        }
        break;
    }
}

// 0x80158834: "no save file" prompt; waits for a button before creating one.
void fn_80158834() {
    f32 now;
    f32 elapsed;
    PromptColor msg;
    int r;
    int r2;

    elapsed = 30000.0f;
    msg = lbl_804D7CE8;
    fn_800EC80C(0x32, 0x96, 0x21c, 0x7d);
    LoadingTime();
    do {
        now = LoadingTime();
        elapsed = elapsed - now;
        fn_80005A64();
        fn_800ECB18();
        fn_800EC9DC(0x32, 0x96, 0x21c, 0x7d);
        {
            PromptColor arg = msg;
            fn_801EA880((void*)lbl_804D2F48, &arg, 0x50, 0x82, 0xe, 0x10, 0);
        }
        fn_800EBBB8(now, 0x104, 0x45);
        fn_801EAEAC();
        SystemPoll();
        if (elapsed <= 0.0f) {
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
    SaveCommand(1, 0);
    r = SaveCommand(6, 0);
    gSaveLoaded = (r == 1) ? 2 : 0;
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
    lbl_804D4E48->timer = 0.0f;
    lbl_804D4E48->obj = (void*)obj;
    lbl_804D4E48->textAlpha = 255.0f;
    if (lbl_804D4E48->sndA != -1) {
        lbl_804D4E48->sndA = fn_8007EF48(player, 1, "Lighten Loop", 0, 0, 0, 0, 0, 0, 0);
    }
    if (lbl_804D4E48->sndB != -1) {
        lbl_804D4E48->sndB = fn_8007EF48(player, 1, "Focus Loop", 0, 0, 0, 0, 0, 0, 0);
    }
    lbl_804D4E48->fade = 1.0f;
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
    lbl_804D4E44->state = 1;
    lbl_804D4E44->timer = 0.0f;
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
    lbl_804D4E34->timer = 0.0f;
    lbl_804D4E34->objA = (void*)a;
    lbl_804D4E34->objB = (void*)b;
}

// 0x80158BE0: is any player HUD effect running?
int fn_80158BE0() {
    int i;

    for (i = 0; i < 4; i++) {
        if (lbl_8046DDE8[i].state != 0) {
            return 1;
        }
    }
    return 0;
}
} // extern "C"

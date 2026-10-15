// main.cpp: the program entry (0x8000DC24-0x8000F2F0).
// main() reads the reset code, sets up the paired-single quantizers and the subsystems, shows the
// logos (or the progressive-scan question on a restart), then runs levels: each AppMain call returns
// the next level name, which is looked up in "00Index.TXT". A debug level-select menu lists the index.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <dolphin/vi.h>

extern "C" {
char* strcpy(char* dst, const char* src);
int strcmp(const char* a, const char* b);
int sprintf(char* s, const char* format, ...);
double tan(double x);
int rand(void);
void srand(unsigned int seed);
long time(long* t);
extern unsigned char __upper_map[];

// Callees in other files, not named yet.
void fn_8000ADD0(int stage);
void fn_8000B034(int arg);
void fn_80005CEC(void);
void fn_800DCD44(void);
void fn_801EB060(int arg);
void fn_80170C28(void);
void fn_80005A64(float dt);
void fn_800DCBB0(float dt);
int fn_800DCAE8(void);
void fn_80170AA4(void);
void fn_8000B604(int arg);
void fn_80115EE0(void);
void fn_80288C9C(void);
void fn_800E0808(void);
void fn_80088574(void);
void fn_80170B14(int arg);
void fn_8000879C(const char* name);
void fn_8000B000(void);
void fn_80006558(int arg);
void fn_8013C7D4(void* a, void* b, int c, int d, int e, int f, int g, int h, int i, int j, int k);
void fn_8013C76C(void* a, void* pos);
void fn_8013C6F0(void);
void fn_8013BF58(int a, float b);
void fn_8013BA9C(void* a, int r, int g, int b, int alpha);
void fn_8013BA70(void);
void fn_8013C6C4(void);
int fn_800DC95C(int pad, int button);
void fn_800DE2F8(int command, int arg);
void fn_800087D8(int a, int b, int c);
int fn_800087C8(int arg);
void* fn_802869DC(int size);
void fn_80286980(void);
int fn_80005974(void);
int fn_800059D0(int button);
void fn_800EC80C(int x, int y, int w, int h);
void fn_800ECB18(void);
void fn_800EC9DC(int x, int y, int w, int h);
void fn_801EA880(const char* text, GXColor color, int x, int y, int w, int h, int flags);
void fn_800EBBB8(int x, int y, float dt);
void fn_801EAEAC(void);
void fn_800EC614(int x, int y, int w, int h);
float fn_800A1418(float x);
float fn_800A1394(float x);
const char* fn_80086B9C(const char* level, int* arg);
void fn_800880D0(void);
}

struct ImageInfo;
struct TPLPalette;

void DebugPrintf(const char* format, ...);
void ScreenPrintf(const char* format, ...);
void MemPrintStatus(void);
void MemFree(void* p, const char* file, int line);
void* FileLoad(const char* name, int* outSize, int unused);
float LoadingTime(void);
void SystemPoll(void);
void SystemIdle(void);
void GfxFrameEnd(void);
void GfxFrameSwap(void);
void GfxFrameBegin(void);
void ImageSetPixelFmtRGB8(void);
void ImageSetPixelFmtRGBA6(void);
void ImageDrawLoading(ImageInfo* info, void* data, ImageInfo* iconInfo, void* iconData, int frame, int alpha);
void ImageDrawRaw(ImageInfo* info, void* data, u8 a, u8 r, u8 g, u8 b, f32 x0, f32 x1, f32 y0, f32 y1);
void ImageDrawTPL(TPLPalette* pal, u8 alpha);
int ImageReadTPL(TPLPalette** pal, const char* name);

extern int lbl_804D4470;
extern ImageInfo lbl_8034F220;
extern u8 lbl_8034F240[];
extern ImageInfo lbl_803623C0;
extern u8 lbl_803623E0[];
extern ImageInfo lbl_803837E0;
extern u8 lbl_80383800[];
extern ImageInfo lbl_80387100;
extern u8 lbl_80387120[];

struct Vec3 {
    float x, y, z;
};

const char* sGameName = "Fear";
const char* sFirstLevel = "mainmenu";
ImageInfo* sLogoInfo0 = &lbl_8034F220;
void* sLogoData0 = lbl_8034F240;
ImageInfo* sLogoInfo1 = &lbl_80387100;
void* sLogoData1 = lbl_80387120;
ImageInfo* sLoadInfo = &lbl_803623C0;
void* sLoadData = lbl_803623E0;
ImageInfo* sLoadIconInfo = &lbl_803837E0;
void* sLoadIconData = lbl_80383800;
const char* sProgressiveAsk = "Do you want to display in\nProgressive Mode?";
const char* sProgressiveOn = "Screen has been set to Progressive Mode";
const char* sProgressiveOff = "  Progressive Mode has been turned off";

static char sLevelUpper[0x40];
static char sLevelPath[0x100];
static char sLevelName[0x90];

int sProgressive;
static void (*sTickHook)(int code);
static u32 sTickStart;
static u32 sTickTime;
u32 sResetCode;
int* sCheckpoint;
const char* (*sAppMain)(const char* level, int* arg);
void (*sAppInit)(void);

void TickTimer(int code) {
    if ((u16)code == 0xABC) {
        sTickStart = OSGetTick();
    } else if ((u16)code == 0x123) {
        sTickTime = OSGetTick() - sTickStart;
    }
    if (sTickHook) {
        sTickHook(code);
    }
}

struct Rect {
    float x0, y0, x1, y1;
};

int RectUnion(const Rect* a, const Rect* b, Rect* out) {
    float x0 = a->x0 < b->x0 ? a->x0 : b->x0;
    float y0 = a->y0 < b->y0 ? a->y0 : b->y0;
    float x1 = a->x1 > b->x1 ? a->x1 : b->x1;
    float y1 = a->y1 > b->y1 ? a->y1 : b->y1;
    out->x0 = x0;
    out->y0 = y0;
    out->x1 = x1;
    out->y1 = y1;
    return 1;
}

int RectIntersect(const Rect* a, const Rect* b, Rect* out) {
    float x0, y0, x1, y1;
    if (a->x1 < b->x0 || a->x0 > b->x1 || a->y1 < b->y0 || a->y0 > b->y1) {
        return 0;
    }
    x0 = a->x0 > b->x0 ? a->x0 : b->x0;
    y0 = a->y0 > b->y0 ? a->y0 : b->y0;
    x1 = a->x1 < b->x1 ? a->x1 : b->x1;
    y1 = a->y1 < b->y1 ? a->y1 : b->y1;
    out->x0 = x0;
    out->y0 = y0;
    out->x1 = x1;
    out->y1 = y1;
    return 1;
}

float Tan(float x) { return tan(x); }

int Rand(void) { return rand(); }

void MainNop(void) {}

const char* LevelFind(const char* name);
void LogoShow(int arg);
int ProgressiveSelect(void);

inline void InitFastCast(void) {
    asm {
        li r3, 4
        oris r3, r3, 4
        mtspr GQR2, r3
        li r3, 5
        oris r3, r3, 5
        mtspr GQR3, r3
        li r3, 6
        oris r3, r3, 6
        mtspr GQR4, r3
        li r3, 7
        oris r3, r3, 7
        mtspr GQR5, r3
    }
}

int main(int argc, char** argv) {
    int restart = 0;
    int arg = -1;
    const char* level;
    int n;

    sCheckpoint = &lbl_804D4470;
    sAppMain = fn_80086B9C;
    sAppInit = fn_800880D0;
    if (argc) {
        DebugPrintf("Arg: '%s'\n", argv[1]);
    }
    sResetCode = OSGetResetCode();
    if (sResetCode & 0x80000000) {
        sResetCode &= 0x7FFFFFFF;
        fn_8000ADD0(sResetCode);
        sResetCode |= 0x80000000;
    }
    DebugPrintf("Reset code=%x\n", sResetCode);
    InitFastCast();
    fn_8000B034(0);
    fn_80005CEC();
    fn_800DCD44();
    fn_801EB060(2);
    fn_80170C28();
    fn_80005A64(1.0f);
    fn_800DCBB0(1.0f);
    for (n = fn_800DCAE8(); n != 0; n >>= 1) {
    }
    if ((sResetCode & 0x7FFFFFFF) < 3) {
        DebugPrintf("Showing Logos\n");
        LogoShow(1);
        fn_80170AA4();
    } else {
        DebugPrintf("Skipping Logos\n");
        if (ProgressiveSelect()) {
            DebugPrintf("Calling CubeReInit for progressive mode\n");
            fn_8000B604(0);
        }
        restart = 1;
    }
    fn_80115EE0();
    fn_80288C9C();
    fn_800E0808();
    sAppInit();
    fn_80088574();
    if (restart) {
        fn_80170B14(0);
    }
    fn_8000879C(sFirstLevel);
    level = LevelFind(sFirstLevel);
    while (level) {
        DebugPrintf("Before AppMain ");
        MemPrintStatus();
        srand(time(0));
        level = sAppMain(level, &arg);
        DebugPrintf("After AppMain ");
        MemPrintStatus();
        level = LevelFind(level);
    }
    fn_8000B000();
    return 0;
}

const char* LevelSelect(const char* name, char** names, char** titles, char** paths, int count);

const char* LevelFind(const char* name) {
    char* names[64];
    char* titles[64];
    char* paths[64];
    int size = 0;
    char* buf;
    char* p;
    int count;

    p = (char*)FileLoad("00Index.TXT", &size, 0);
    buf = p;
    if (p && size) {
        count = 0;
        while (size) {
            names[count] = p;
            while (size && *p != '\t') {
                p++;
                size--;
            }
            if (size) {
                *p++ = 0;
                if (--size) {
                    paths[count] = p;
                    while (size && *p != '\t') {
                        p++;
                        size--;
                    }
                    if (size) {
                        *p++ = 0;
                        if (--size) {
                            titles[count] = p;
                            while (size && *p != '\n') {
                                p++;
                                size--;
                            }
                            if (size) {
                                *p++ = 0;
                                size--;
                                count++;
                            }
                        }
                    }
                }
            }
        }
        strcpy(sLevelName, LevelSelect(name, names, titles, paths, count));
        MemFree(buf, "main.cpp", 0x486);
        return sLevelName;
    }
    DebugPrintf("\n00Index.txt NOT FOUND\n\n");
    return 0;
}

inline int ToUpper(int c) { return c == -1 ? -1 : __upper_map[c & 0xFF]; }

inline void StrUpper(char* dst, const char* src) {
    int i;
    for (i = 0; src[i]; i++) {
        dst[i] = ToUpper(src[i]);
    }
    dst[i] = 0;
}

const char* LevelSelect(const char* name, char** names, char** titles, char** paths, int count) {
    int sel = 0;
    int slot = 0;
    int i;
    int j;
    u32 time;
    char* path;
    u8 obj[0x24];
    u8 view[0x180];

    if (name) {
        StrUpper(sLevelUpper, name);
        sprintf(sLevelPath, "/%s/%s.OLC", sLevelUpper, sLevelUpper);
        path = sLevelPath;
        DebugPrintf("Seeking '%s'\n", path);
        for (j = 0; j < count; j++) {
            if (strcmp(paths[j], path) == 0) {
                DebugPrintf("Matched '%s'\n", paths[j]);
                return paths[j];
            }
        }
        DebugPrintf("Level '%s' not found\n", name);
    }

    fn_80006558(0);
    time = 0;
    fn_8013C7D4(obj, view, 32, 0, 155, 255, 255, 0, 100, 255, 0);
    for (;;) {
        float dt = LoadingTime();
        float t;
        float a;
        float b;
        float x;
        float y;

        fn_80005A64(dt);
        fn_800DCBB0(dt);
        ScreenPrintf("%s                            Checkpoint (X/Y) %d\n", sGameName, *sCheckpoint);
        if (slot < 3) {
            ScreenPrintf("%s                            Save slot (Z) %d\n\n", sGameName, slot);
        } else {
            ScreenPrintf("%s                            Save slot (Z) %s\n\n", sGameName, "Disabled");
        }
        for (i = 0; i < count; i++) {
            ScreenPrintf("%s", sel == i ? " >" : "  ");
            ScreenPrintf("%s\n", names[i]);
        }
        GfxFrameEnd();
        GfxFrameSwap();
        time += (int)dt;
        t = (float)time / 50.0f;
        a = 0.75 + 0.25f * fn_800A1418(t);
        b = 0.75 + 0.25f * fn_800A1394(t);
        x = 25.0f * (a * fn_800A1418((float)time / 5.0f)) + 580.0f;
        y = 25.0f * (b * fn_800A1394((float)time / 7.0f)) + 60.0f;
        Vec3 pos = {0.0f, 0.0f, 0.0f};
        pos.x = x;
        pos.y = y;
        fn_8013C76C(obj, &pos);
        fn_8013C6F0();
        fn_8013BF58(-3, 5.0f);
        fn_8013BA9C(obj, 255, 255, 255, 255);
        fn_8013BA70();
        fn_8013C6C4();
        GfxFrameBegin();
        if (fn_800DC95C(0, 0x13) != -1) {
            (*sCheckpoint)--;
            if (*sCheckpoint < 0) {
                *sCheckpoint = 0;
            }
        }
        if (fn_800DC95C(0, 0x12) != -1) {
            (*sCheckpoint)++;
        }
        if (fn_800DC95C(0, 0x11) != -1) {
            slot = (slot + 1) % 4;
        }
        if (fn_800DC95C(0, 3) != -1 || fn_800DC95C(0, 2) != -1) {
            break;
        }
        if (fn_800DC95C(0, 0) != -1) {
            sel--;
        }
        if (fn_800DC95C(0, 1) != -1) {
            sel++;
        }
        if (sel < 0) {
            sel = 0;
        }
        if (sel > count - 1) {
            sel = count - 1;
        }
    }
    ScreenPrintf("  %s\n", titles[sel]);
    GfxFrameEnd();
    GfxFrameSwap();
    GfxFrameBegin();
    ScreenPrintf("  %s\n", titles[sel]);
    GfxFrameEnd();
    GfxFrameSwap();
    GfxFrameBegin();
    if (slot < 3) {
        fn_800DE2F8(4, slot);
        fn_800DE2F8(11, slot);
    } else {
        fn_800DE2F8(4, -1);
    }
    return paths[sel];
}

void LogoShow(int arg) {
    TPLPalette* pal;
    int ok;
    int old;
    int i;

    ImageSetPixelFmtRGB8();
    fn_800087D8(0, 0, 0);
    if (ProgressiveSelect()) {
        DebugPrintf("Calling CubeReInit for progressive mode\n");
        fn_8000B604(0);
    }
    fn_8000ADD0(2);
    LoadingTime();
    old = fn_800087C8(0);
    for (i = 0; i < 60; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 30; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        ImageDrawLoading(sLoadInfo, sLoadData, 0, 0, 0, (u8)(8.5f * i));
        GfxFrameBegin();
        SystemIdle();
        SystemPoll();
    }
    pal = (TPLPalette*)fn_802869DC(0x80000);
    ok = ImageReadTPL(&pal, "nspace_logo.tpl");
    fn_80170B14(1);
    for (i = 0; i < 3600; i++) {
        GfxFrameEnd();
        fn_80005A64(1.0f);
        GfxFrameSwap();
        if (i > 90) {
            ImageDrawLoading(sLoadInfo, sLoadData, sLoadIconInfo, sLoadIconData, i - 90, 255);
        } else {
            ImageDrawLoading(sLoadInfo, sLoadData, 0, 0, i, 255);
        }
        GfxFrameBegin();
        if (i > 90 && fn_80005974()) {
            break;
        }
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 30; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        ImageDrawLoading(sLoadInfo, sLoadData, 0, 0, 0, (u8)(8.5f * (30 - i)));
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 60; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 30; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        ImageDrawRaw(sLogoInfo0, sLogoData0, (int)(8.5f * i), 220, 0, 0, 133.0f, 509.0f, 170.0f, 274.0f);
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 60; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        ImageDrawRaw(sLogoInfo0, sLogoData0, 255, 220, 0, 0, 133.0f, 509.0f, 170.0f, 274.0f);
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 30; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        ImageDrawRaw(sLogoInfo0, sLogoData0, (int)(8.5f * (30 - i)), 220, 0, 0, 133.0f, 509.0f, 170.0f, 274.0f);
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 5; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 30; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        ImageDrawRaw(sLogoInfo1, sLogoData1, (int)(8.5f * i), 255, 255, 255, 205.0f, 437.0f, 166.0f, 278.0f);
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 60; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        ImageDrawRaw(sLogoInfo1, sLogoData1, 255, 255, 255, 255, 205.0f, 437.0f, 166.0f, 278.0f);
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 30; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        ImageDrawRaw(sLogoInfo1, sLogoData1, (int)(8.5f * (30 - i)), 255, 255, 255, 205.0f, 437.0f, 166.0f, 278.0f);
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    for (i = 0; i < 5; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    if (ok) {
        for (i = 0; i < 30; i++) {
            GfxFrameEnd();
            GfxFrameSwap();
            ImageDrawTPL(pal, (int)(8.5f * i));
            GfxFrameBegin();
            SystemPoll();
            SystemIdle();
        }
        for (i = 0; i < 60; i++) {
            GfxFrameEnd();
            GfxFrameSwap();
            ImageDrawTPL(pal, 255);
            GfxFrameBegin();
            SystemPoll();
            SystemIdle();
        }
        for (i = 0; i < 30; i++) {
            GfxFrameEnd();
            GfxFrameSwap();
            ImageDrawTPL(pal, (int)(8.5f * (30 - i)));
            GfxFrameBegin();
            SystemPoll();
            SystemIdle();
        }
        fn_80286980();
    }
    for (i = 0; i < 5; i++) {
        GfxFrameEnd();
        GfxFrameSwap();
        GfxFrameBegin();
        SystemPoll();
        SystemIdle();
    }
    SystemPoll();
    SystemIdle();
    fn_800087C8(old);
    ImageSetPixelFmtRGBA6();
    fn_8000ADD0(3);
}

int ProgressiveSelect(void) {
    u32 mode = OSGetProgressiveMode();
    int digital;

    sProgressive = 0;
    digital = VIGetDTVStatus();
    DebugPrintf("hasDigital==%d\n", digital);
    if (digital == 0) {
        DebugPrintf("No digital, stay in interlace\n");
        OSSetProgressiveMode(0);
        return 0;
    }
    fn_80005A64(16.66f);
    if (sResetCode & 0x80000000) {
        int stage = sResetCode & 0x7FFFFFFF;
        DebugPrintf("Restart==TRUE, stage=%d\n", stage);
        if (stage >= 2) {
            DebugPrintf("Keeping saved progressive setting==%d\n", mode);
            sProgressive = mode;
            return mode;
        }
    }
    DebugPrintf(" Showing progressive select screen\n");
    if (VIGetTvFormat() == VI_NTSC && digital && (mode == 1 || fn_800059D0(0x13))) {
        int choice = 1;
        GXColor color = {200, 200, 200, 255};
        float timer = 10000.0f;
        float timer2;

        fn_800EC80C(50, 90, 540, 65);
        LoadingTime();
        for (;;) {
            float dt = LoadingTime();
            fn_80005A64(dt);
            timer -= dt;
            fn_800ECB18();
            fn_800EC9DC(50, 90, 540, 65);
            fn_801EA880(sProgressiveAsk, color, 80, 78, 14, 16, 0);
            fn_801EA880("Yes", color, 440, 78, 14, 16, 0);
            fn_801EA880("No", color, 530, 78, 14, 16, 0);
            fn_800EBBB8((choice ? 440 : 530) - 20, 68, dt);
            fn_801EAEAC();
            SystemPoll();
            SystemIdle();
            if (timer <= 0.0f || fn_800059D0(0x15) == 1 || fn_800059D0(0x14) == 1) {
                timer2 = 10000.0f;
                choice = (choice != 0) + 1;
                break;
            }
            if (fn_800059D0(0xF) == 1 && choice == 0) {
                choice = 1;
                timer = 10000.0f;
            }
            if (fn_800059D0(0x10) == 1 && choice == 1) {
                timer = 10000.0f;
                choice = 0;
            }
        }
        fn_800EC614(50, 90, 540, 65);
        fn_800ECB18();
        fn_800ECB18();
        if (choice == 2) {
            if ((int)mode == 0) {
                OSSetProgressiveMode(1);
            }
            sProgressive = 1;
            fn_8000B604(0);
        } else if ((int)mode == 1) {
            OSSetProgressiveMode(0);
        }
        fn_800EC80C(50, 90, 540, 65);
        LoadingTime();
        for (;;) {
            float dt = LoadingTime();
            fn_80005A64(dt);
            timer2 -= dt;
            fn_800ECB18();
            fn_800EC9DC(50, 90, 540, 65);
            switch (choice) {
            case 1:
                fn_801EA880(sProgressiveOff, color, 80, 78, 14, 16, 0);
                break;
            case 2:
                fn_801EA880(sProgressiveOn, color, 80, 78, 14, 16, 0);
                break;
            }
            fn_801EAEAC();
            SystemPoll();
            SystemIdle();
            if (timer2 <= 0.0f) {
                break;
            }
            if (timer2 <= 9500.0f && fn_80005974()) {
                break;
            }
        }
        fn_800EC614(50, 90, 540, 65);
        fn_800ECB18();
        fn_800ECB18();
    }
    return 0;
}

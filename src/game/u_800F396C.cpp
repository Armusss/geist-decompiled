// u_800F396C.cpp: weather / sky effect controller (0x800F396C-0x800F73C8).
// Keeps the current weather type per zone slot (clear, cloud, rain, ...), ramps the effect
// parameters, and drives the ambient stream volume.  Names are descriptive guesses; unmatched
// against retail so far.  Only the small helpers are drafted; the large update and draw
// functions (fn_800F39E0, fn_800F4ACC, fn_800F4F0C, fn_800F6264, fn_800F68AC, ...) are not.

typedef unsigned long u32;
typedef unsigned char u8;

extern "C" {
void fn_80088C10(int id, const char* name, int a, int b, int c, int d, int e, int f);
void fn_80088FB0(int id, int on);
void fn_80088F0C(int id, int on);
void fn_8009425C(void);
void fn_800F9468(int on);
float fn_80017084(float x);
float fn_800A1394(float x);
void AISetStreamVolLeft(u8 v);
void AISetStreamVolRight(u8 v);
}
extern int AssetFind(u32 a, u32 b);

extern int sWeatherMode;        // 0x804D4A84
extern int sWeatherParamA;      // 0x804D4A88
extern int sWeatherFlag;        // 0x804D4A8C
extern float sWeatherFade;      // 0x804D4A90
extern int sWeatherArgA;        // 0x804D4A94
extern int sWeatherArgB;        // 0x804D4A98
extern int sWeatherAssetA;      // 0x804D4A68
extern int sWeatherAssetB;      // 0x804D4A6C
extern int sWeatherAssetC;      // 0x804D4A70
extern int sWeatherAssetD;      // 0x804D4A74
extern int sWeatherAssetE;      // 0x804D4A78
extern int sWeatherAssetF;      // 0x804D4A7C
extern int sWeatherZero;        // 0x804D4A80
extern int sWeatherSlotA[][3];     // 0x80457BEC: current type per [zone*3 + slot]
extern int sWeatherSlotB[][3];     // 0x80457C1C: previous type
extern int sWeatherZoneMode[];  // 0x80457BDC
extern int sWeatherTypeIds[];   // 0x8039CDC8
extern int sWeatherCur;         // 0x804D4AAC
extern int sWeatherUnkA4;       // 0x804D4AA4
extern int sWeatherUnk4190;     // 0x804D4190
extern float sWeatherLevel;     // 0x804D4ADC
extern float sWeatherRainA;     // 0x804D4B2C
extern float sWeatherRainB;     // 0x804D4B30
extern float sWeatherF00;       // 0x804D4B00
extern float sWeatherF98;       // 0x804D4AF8
extern float sWeatherF90;       // 0x804D4AF0
extern float sWeatherF8C;       // 0x804D4AEC
extern float sWeatherF9C;       // 0x804D4AFC
extern float sWeatherF94;       // 0x804D4AF4
extern float sWeatherSq;        // 0x804D2D18
extern float sWeatherBase;      // 0x804D2CB8
extern int sWeatherB0;          // 0x804D4AB0
extern int sWeatherAssetG;      // 0x804D4B38
extern int sWeatherTable[128];  // 0x80457C4C
extern int sWeatherInit;        // 0x804D4B4C
extern char sWeatherInitDone;    // 0x804D4B50
extern int sWeatherEnabled;     // 0x804D2C74

extern int sStreamState;        // 0x804D4B64
extern int sStreamUnk68;        // 0x804D4B68
extern int sStreamUnk6C;        // 0x804D4B6C
extern int sStreamRate;         // 0x804D4B70
extern u8 sStreamVolL;          // 0x804D4B74
extern u8 sStreamVolR;          // 0x804D4B75
extern int sStreamUnk58;        // 0x804D4B58
extern int sStreamUnk5C;        // 0x804D4B5C
extern int sStreamUnk60;        // 0x804D4B60

// 0x800F396C: switch to the clear-sky fade.
void WeatherSetClear(int a, int b) {
    if (sWeatherMode == 3) {
        return;
    }
    sWeatherMode = 3;
    sWeatherFade = 1000.0f;
    sWeatherArgA = a;
    sWeatherArgB = b;
}

// 0x800F3994: switch to the fast fade.
void WeatherSetFast(void) {
    sWeatherParamA = 0x1000;
    sWeatherMode = 4;
    sWeatherFade = 150.0f;
    sWeatherFlag = 1;
}

// 0x800F39B8: switch to the default fade.
void WeatherSetDefault(void) {
    if (sWeatherMode == 1) {
        return;
    }
    sWeatherMode = 1;
    sWeatherFade = 1000.0f;
    sWeatherParamA = 0;
}

// 0x800F45F8: register the sky layers.
void WeatherInit(void) {
    sWeatherAssetA = AssetFind(0xF2E2BEB8, 0x909F25F5);
    sWeatherAssetB = AssetFind(0xF2E2BEB8, 0x897D991A);
    sWeatherAssetC = AssetFind(0xF2E2BEB8, 0xCAC810DD);
    sWeatherAssetD = AssetFind(0xF2E2BEB8, 0xF733FFE9);
    sWeatherAssetE = AssetFind(0xF2E2BEB8, 0x19528D48);
    sWeatherAssetF = AssetFind(0xF2E2BEB8, 0x981B2243);
    sWeatherParamA = 0x1000;
    sWeatherMode = 2;
    sWeatherZero = 0;
    sWeatherFade = 0.0f;
    sWeatherArgA = 0;
    sWeatherArgB = 0;
    fn_80088C10(sWeatherAssetA, "NODRAW", 0, 0, 0, 0x40, 0, 0);
    fn_80088C10(sWeatherAssetB, "CLOUD", 18000, 0, sWeatherParamA, 1, 1, sWeatherAssetF);
    fn_80088C10(sWeatherAssetC, "CLOUD", 0x4844, 0, sWeatherParamA, -1, -1, sWeatherAssetF);
    fn_80088C10(sWeatherAssetD, "TDRIL", 8000, 14000, sWeatherParamA, 4, 1, 0x1000);
    fn_80088FB0(sWeatherAssetA, 0);
    fn_80088FB0(sWeatherAssetB, 1);
    fn_80088FB0(sWeatherAssetC, 1);
    fn_80088FB0(sWeatherAssetD, 1);
    fn_80088F0C(sWeatherAssetA, 1);
    fn_80088F0C(sWeatherAssetB, 1);
    fn_80088F0C(sWeatherAssetC, 1);
    fn_80088F0C(sWeatherAssetD, 1);
}

// 0x800F47D8 .. 0x800F484C: plain setters.
void WeatherSetRainA(float v) { sWeatherRainA = v; }
void WeatherSetRainB(float v) { sWeatherRainB = v; }
void WeatherResetF00(void) { sWeatherF00 = 1.0f; }
void WeatherSetF98(float v) { sWeatherF98 = v; }
void WeatherSetF90(float v) { sWeatherF90 = v; }
void WeatherSetF8C(float v) { sWeatherF8C = v; }

// 0x800F480C: squared fog factor.
void WeatherSetSq(float v) {
    float t = fn_800A1394(90.0f * v);
    sWeatherSq = t * t;
}

void WeatherSetBase(float v) { sWeatherBase = v; }
void WeatherSetB0(int v) { sWeatherB0 = v; }

// 0x800F4854: one-time init of the flag.
void WeatherLatchInit(void) {
    if (sWeatherInitDone == 0) {
        sWeatherInit = 0;
        sWeatherInitDone = 1;
    }
    if (sWeatherEnabled != 0) {
        sWeatherInit = 0;
    }
}

// 0x800F60B8: type id of the current weather in a zone slot.
int WeatherGetTypeId(int zone, int slot) {
    return sWeatherTypeIds[sWeatherSlotA[zone][slot]];
}

// 0x800F60E4: set the weather of a zone slot by type id.
void WeatherSetByTypeId(int zone, int slot, int id) {
    int i = 0;
    int* p = sWeatherTypeIds;
    int n = 14;
    sWeatherUnkA4 = 0;
    sWeatherUnk4190 = 0;
    do {
        if (id == p[0]) {
            sWeatherSlotB[zone][slot] = sWeatherSlotA[zone][slot];
            sWeatherSlotA[zone][slot] = i;
            if (zone == 0 && slot == 0) {
                sWeatherCur = i;
            }
        }
        if (id == p[1]) {
            sWeatherSlotB[zone][slot] = sWeatherSlotA[zone][slot];
            sWeatherSlotA[zone][slot] = i + 1;
            if (zone == 0 && slot == 0) {
                sWeatherCur = i + 1;
            }
        }
        if (id == p[2]) {
            sWeatherSlotB[zone][slot] = sWeatherSlotA[zone][slot];
            sWeatherSlotA[zone][slot] = i + 2;
            if (zone == 0 && slot == 0) {
                sWeatherCur = i + 2;
            }
        }
        p += 3;
        i += 3;
        n--;
    } while (n != 0);
    if (id == sWeatherTypeIds[42]) {
        sWeatherLevel = 1000.0f;
    }
    fn_8009425C();
    if (sWeatherSlotA[zone][slot] == 0x17) {
        fn_800F9468(1);
    } else {
        fn_800F9468(0);
    }
}

// 0x800F6228: change the mode of a zone.
void WeatherSetZoneMode(int zone, int mode) {
    if (mode != sWeatherZoneMode[zone]) {
        sWeatherZoneMode[zone] = mode;
        fn_8009425C();
    }
}

// 0x800F671C: build the 128-entry ripple table.
void WeatherBuildTable(void) {
    int i;
    int* p = sWeatherTable;
    u32 ang = 0;
    for (i = 0; i < 0x80; i++) {
        float s = fn_80017084(0.0f * (float)(int)ang);
        *p = (int)(0.0f * s);
        p++;
        ang += 0x10;
    }
    sWeatherAssetG = AssetFind(0xF2E2BEB8, 0xCB11C7AF);
}

// 0x800F6804: clear all weather slots.
void WeatherReset(void) {
    int i;
    for (i = 0; i < 4; i++) {
        sWeatherSlotA[i][0] = 0;
        sWeatherSlotB[i][0] = 0;
        sWeatherSlotA[i][1] = 0;
        sWeatherSlotB[i][1] = 0;
        sWeatherSlotA[i][2] = 0;
        sWeatherSlotB[i][2] = 0;
    }
    for (i = 0; i < 4; i++) {
        sWeatherZoneMode[i] = 0;
    }
    sWeatherF9C = 0.0f;
    sWeatherF98 = 0.0f;
    sWeatherF94 = 0.0f;
    sWeatherF90 = 0.0f;
    sWeatherCur = 0;
    sWeatherBase = 100.0f;
}

// 0x800F6CFC: set the stream volumes.
void StreamSetVolumes(u8 left, int right) {
    sStreamVolR = (u8)right;
    sStreamVolL = left;
    if (sStreamState == 1) {
        AISetStreamVolLeft(left);
        AISetStreamVolRight((u8)right);
    }
}

int StreamGetUnk58(void) { return sStreamUnk58; }
int StreamGetState(void) { return sStreamState; }
void StreamSetUnk68(int v) { sStreamUnk68 = v; }

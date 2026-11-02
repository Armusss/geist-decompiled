// u_80142E88.cpp: level progress bookkeeping: zone and unlock tables (the "Z_BASE",
// "Z_Office" ... names), the host unlock count, and a few state accessors.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef signed long s32;

extern "C" {
int stricmp(const char* a, const char* b);
int strcmp(const char* a, const char* b);
char* strupr(char* s);
}

// A zone name and the progress flags set for it (lbl_803A5008, 22 entries).
struct ZoneFlag {
    const char* name;  // 0x00
    u32 mask;          // 0x04
};

// One unlock step (lbl_803A50B8, 17 entries).
struct UnlockEntry {
    const char* name;  // 0x00  zone it opens
    u32 flags;         // 0x04  progress flags to add
    u32 threshold;     // 0x08  count at which it unlocks
    u32 key;           // 0x0c  string key announced to the player
    int unk10;         // 0x10
};

// A level name entry (lbl_803A466C, 14 entries).
struct LevelName {
    const char* name;  // 0x00
    int unk04[4];      // 0x04
};

enum { ZONE_FLAG_COUNT = 22, UNLOCK_COUNT = 17, LEVEL_NAME_COUNT = 14 };

extern ZoneFlag lbl_803A5008[ZONE_FLAG_COUNT];
extern UnlockEntry lbl_803A50B8[UNLOCK_COUNT];
extern LevelName lbl_803A466C[LEVEL_NAME_COUNT];
extern u32 lbl_804D4DDC;  // host unlock progress count
extern int lbl_804D4DA4;  // when set every zone counts as unlocked
extern int lbl_804D2ED8;
extern int lbl_804D2EBC;
extern u32 lbl_8046DAFC;      // mask of valid slots
extern int lbl_8046DB00[4][4];  // four value banks

u32 StrFindKey(u32 key, u32* out);

// Option getters and setters (neighbouring units).
extern "C" {
int fn_8016E758();
void fn_8016E6D8(int value);
int fn_80170C30();
void fn_80170DD4(int value);
int fn_80170CBC();
void fn_80170D48(int value);
float fn_800071A4();
void fn_8000719C(float value);
}
extern int lbl_804D2ED0;
extern int gWideScreen;  // 0x804D4008

// 0x80142E88
extern "C" void fn_80142E88()
{
    lbl_804D2ED8 = 3;
}

// 0x80142E94
extern "C" int fn_80142E94()
{
    return lbl_804D2ED8;
}

// 0x80145B24: steps a three-way option (wraps around)
void OptionStep3(int delta)
{
    int v = fn_8016E758() + delta;

    if (v < 0) {
        v = 2;
    } else if (v > 2) {
        v = 0;
    }
    fn_8016E6D8(v);
}

// 0x80145B70: reads the three-way option
int OptionGet3()
{
    return fn_8016E758();
}

// 0x80145B90: changes a volume by steps of four (0-100)
void VolumeAdjustA(int steps)
{
    int v = steps * 4 + fn_80170C30();
    int c;

    if (v < 0) {
        c = 0;
    } else {
        c = 100;
        if (v <= 100) {
            c = v;
        }
    }
    fn_80170DD4(c);
}

// 0x80145BE4: reads the first volume
int VolumeGetA()
{
    return fn_80170C30();
}

// 0x80145C04: changes the second volume by steps of four (0-100)
void VolumeAdjustB(int steps)
{
    int v = steps * 4 + fn_80170CBC();
    int c;

    if (v < 0) {
        c = 0;
    } else {
        c = 100;
        if (v <= 100) {
            c = v;
        }
    }
    fn_80170D48(c);
}

// 0x80145C58: reads the second volume
int VolumeGetB()
{
    return fn_80170CBC();
}

// 0x80145DF4: toggles a two-state option
void OptionToggle(int delta)
{
    lbl_804D2ED0 = (lbl_804D2ED0 + delta) & 1;
}

// 0x80145E08: reads the two-state option
int OptionGet()
{
    return lbl_804D2ED0;
}

// 0x80145E10: changes the brightness by steps of 0.05, limited to +-0.5
void BrightnessAdjust(int steps)
{
    float v = 0.05f * (float)steps + fn_800071A4();
    float c = 0.5f;

    if (v <= c) {
        c = v;
        if (v < -0.5f) {
            c = -0.5f;
        }
    }
    fn_8000719C(c);
}

// 0x80145E88: brightness as a percentage
int BrightnessGet()
{
    return (int)(100.0f * (0.5f + fn_800071A4()));
}

// 0x80145EC4: steps the widescreen option (0-1)
void WideScreenStep(int delta)
{
    gWideScreen = gWideScreen + delta;
    if (gWideScreen < 0) {
        gWideScreen = 0;
    } else if (gWideScreen > 1) {
        gWideScreen = 1;
    }
}

// 0x80145EEC: is widescreen on?
int WideScreenOn()
{
    return gWideScreen != 0;
}

// TODO (not drafted yet, 0x7CC bytes at 0x80142E9C): progress state update (14 calls)
extern "C" void fn_80142E9C();

// 0x8014FCB4: does a zone have all the given progress flags?
int ZoneHasFlags(const char* name, u32 mask)
{
    ZoneFlag* z;
    u32 i;

    if (lbl_804D4DA4 != 0) {
        return 1;
    }
    z = lbl_803A5008;
    for (i = 0; i < ZONE_FLAG_COUNT; i++, z++) {
        if (stricmp(name, z->name) == 0 && (mask & z->mask) != 0) {
            return 1;
        }
    }
    return 0;
}

// The first unlock step that the current count has not reached yet (null when all are done).
static inline UnlockEntry* UnlockFind()
{
    UnlockEntry* e = lbl_803A50B8;
    int i;

    for (i = 0; i < UNLOCK_COUNT; i++, e++) {
        if (lbl_804D4DDC < e->threshold) {
            return &lbl_803A50B8[i];
        }
    }
    return 0;
}

// 0x8014FD54: how many more pickups until the next unlock (HostUnlockCount)
int HostUnlockCount()
{
    UnlockEntry* e = lbl_803A50B8;
    int i;

    for (i = 0; i < 16; i++, e++) {
        if (lbl_804D4DDC < e->threshold) {
            return lbl_803A50B8[i].threshold - lbl_804D4DDC;
        }
    }
    return 0;
}

// 0x8014FDA8: advances the count and applies the unlocks passed (HostUnlockName)
u32 HostUnlockName(int delta)
{
    UnlockEntry* before = UnlockFind();
    UnlockEntry* after;
    UnlockEntry* e;
    u32 buf[4];

    lbl_804D4DDC += delta;
    after = UnlockFind();
    if (before == 0 || after == 0 || after == before) {
        return 0;
    }
    for (e = before; e != after; e++) {
        ZoneFlag* z = lbl_803A5008;
        u32 i;

        for (i = 0; i < ZONE_FLAG_COUNT; i++, z++) {
            if (stricmp(z->name, e->name) == 0) {
                z->mask |= e->flags;
            }
        }
    }
    return StrFindKey(before->key, buf);
}

// 0x8014FEE4: unlock entry by index
extern "C" UnlockEntry* fn_8014FEE4(int index)
{
    if (index < 0 || index > UNLOCK_COUNT - 1) {
        return 0;
    }
    return &lbl_803A50B8[index];
}

// 0x8014FF10: index of a level name, or 99
extern "C" u32 fn_8014FF10(char* name)
{
    u32 i;
    LevelName* l;

    strupr(name);
    i = 0;
    l = lbl_803A466C;
    do {
        if (strcmp(l->name, name) == 0) {
            return i;
        }
        i++;
        l++;
    } while (i < LEVEL_NAME_COUNT);
    return 99;
}

// 0x8014FF88: clears a selection
void ClearSelection()
{
    lbl_804D2EBC = -1;
}

// 0x8014FF94: stores a value in a bank for a valid slot
extern "C" void fn_8014FF94(int bank, int slot, int value)
{
    if ((lbl_8046DAFC & (1 << slot)) == 0) {
        return;
    }
    if (bank == 2) {
        lbl_8046DB00[2][slot] = value;
        return;
    }
    if (bank > 1) {
        if (bank > 3) {
            return;
        }
        lbl_8046DB00[3][slot] = value;
        return;
    }
    if (bank != 0) {
        if (bank < 0) {
            return;
        }
        lbl_8046DB00[1][slot] = value;
        return;
    }
    lbl_8046DB00[0][slot] = value;
}

// TODO (not drafted yet, 0x16C bytes at 0x80143668): level progress / game state routine
extern "C" void fn_80143668();

// TODO (not drafted yet, 0xE60 bytes at 0x801437D4): level progress / game state routine
extern "C" void fn_801437D4();

// TODO (not drafted yet, 0x458 bytes at 0x80144634): level progress / game state routine
extern "C" void fn_80144634();

// TODO (not drafted yet, 0x300 bytes at 0x80144A8C): level progress / game state routine
extern "C" void fn_80144A8C();

// TODO (not drafted yet, 0x5EC bytes at 0x80144D8C): level progress / game state routine
extern "C" void fn_80144D8C();

// TODO (not drafted yet, 0x7AC bytes at 0x80145378): level progress / game state routine
extern "C" void fn_80145378();







// TODO (not drafted yet, 0xF0 bytes at 0x80145C78): level progress / game state routine
extern "C" void fn_80145C78();

// TODO (not drafted yet, 0x8C bytes at 0x80145D68): level progress / game state routine
extern "C" void fn_80145D68();







// TODO (not drafted yet, 0x418 bytes at 0x80145F00): level progress / game state routine
extern "C" void fn_80145F00();

// TODO (not drafted yet, 0x420 bytes at 0x80146318): level progress / game state routine
extern "C" void fn_80146318();

// TODO (not drafted yet, 0x418 bytes at 0x80146738): level progress / game state routine
extern "C" void fn_80146738();

// TODO (not drafted yet, 0xFC8 bytes at 0x80146B50): level progress / game state routine
extern "C" void fn_80146B50();

// TODO (not drafted yet, 0x244 bytes at 0x80147B18): level progress / game state routine
extern "C" void fn_80147B18();

// TODO (not drafted yet, 0x184 bytes at 0x80147D5C): level progress / game state routine
extern "C" void fn_80147D5C();

// TODO (not drafted yet, 0x338 bytes at 0x80147EE0): level progress / game state routine
extern "C" void fn_80147EE0();

// TODO (not drafted yet, 0x29C bytes at 0x80148218): level progress / game state routine
extern "C" void fn_80148218();

// TODO (not drafted yet, 0x13C bytes at 0x801484B4): level progress / game state routine
extern "C" void fn_801484B4();

// TODO (not drafted yet, 0x598 bytes at 0x801485F0): level progress / game state routine
extern "C" void fn_801485F0();

// TODO (not drafted yet, 0x488 bytes at 0x80148B88): level progress / game state routine
extern "C" void fn_80148B88();

// TODO (not drafted yet, 0xAB0 bytes at 0x80149010): level progress / game state routine
extern "C" void fn_80149010();

// TODO (not drafted yet, 0x7BC bytes at 0x80149AC0): level progress / game state routine
extern "C" void fn_80149AC0();

// TODO (not drafted yet, 0x3CC bytes at 0x8014A27C): level progress / game state routine
extern "C" void fn_8014A27C();

// TODO (not drafted yet, 0x44C bytes at 0x8014A648): level progress / game state routine
extern "C" void fn_8014A648();

// TODO (not drafted yet, 0x3F4 bytes at 0x8014AA94): level progress / game state routine
extern "C" void fn_8014AA94();

// TODO (not drafted yet, 0x5E4 bytes at 0x8014AE88): level progress / game state routine
extern "C" void fn_8014AE88();

// TODO (not drafted yet, 0x3B8 bytes at 0x8014B46C): level progress / game state routine
extern "C" void fn_8014B46C();

// TODO (not drafted yet, 0x2FC bytes at 0x8014B824): level progress / game state routine
extern "C" void fn_8014B824();

// TODO (not drafted yet, 0x23C bytes at 0x8014BB20): level progress / game state routine
extern "C" void fn_8014BB20();

// TODO (not drafted yet, 0x364 bytes at 0x8014BD5C): level progress / game state routine
extern "C" void fn_8014BD5C();

// TODO (not drafted yet, 0x3FC bytes at 0x8014C0C0): level progress / game state routine
extern "C" void fn_8014C0C0();

// TODO (not drafted yet, 0x39C bytes at 0x8014C4BC): level progress / game state routine
extern "C" void fn_8014C4BC();

// TODO (not drafted yet, 0x3B8 bytes at 0x8014C858): level progress / game state routine
extern "C" void fn_8014C858();

// TODO (not drafted yet, 0xA4 bytes at 0x8014CC10): level progress / game state routine
extern "C" void fn_8014CC10();

// TODO (not drafted yet, 0xF4 bytes at 0x8014CCB4): level progress / game state routine
extern "C" void fn_8014CCB4();

// TODO (not drafted yet, 0x134 bytes at 0x8014CDA8): level progress / game state routine
extern "C" void fn_8014CDA8();

// TODO (not drafted yet, 0x80 bytes at 0x8014CEDC): level progress / game state routine
extern "C" void fn_8014CEDC();

// TODO (not drafted yet, 0xD70 bytes at 0x8014CF5C): level progress / game state routine
extern "C" void fn_8014CF5C();

// TODO (not drafted yet, 0x7A0 bytes at 0x8014DCCC): level progress / game state routine
extern "C" void fn_8014DCCC();

// TODO (not drafted yet, 0x25C bytes at 0x8014E46C): level progress / game state routine
extern "C" void fn_8014E46C();

// TODO (not drafted yet, 0x314 bytes at 0x8014E6C8): level progress / game state routine
extern "C" void fn_8014E6C8();

// TODO (not drafted yet, 0x1094 bytes at 0x8014E9DC): level progress / game state routine
extern "C" void fn_8014E9DC();

// TODO (not drafted yet, 0x244 bytes at 0x8014FA70): level progress / game state routine
extern "C" void fn_8014FA70();

// TODO (not drafted yet, 0x130 bytes at 0x80150018): level progress / game state routine
extern "C" void fn_80150018();

// u_8028009C.cpp: extras / collectible menus (0x8028009C-0x80285014).
// Unlock bitsets (cheats, movies, concept art, music, ...) with their lookup, unlock and
// save-notify helpers, the extras menu update, and the per-page menu screens.  Only the small
// bitset helpers and the page dispatcher are drafted; the large page functions (0x8028009C,
// 0x802814BC, 0x80281894, 0x80281EE8, 0x8028253C, 0x80282BA4, 0x80283310, 0x80283FF8,
// 0x8028448C, 0x802845CC, 0x802846F0, 0x80284CE0) are not.  Names are descriptive guesses;
// unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned char u8;

#define NULL 0

extern "C" {
void TextBegin(void);                               // 0x801ED0C8
void TextSetMode(int m);                            // 0x801ED1A0
void TextSetColor(int r, int g, int b, int a);      // 0x801ED1E4
void TextSetSize(int n);                            // 0x801ED2A0
void TextSetBox(int a, int b, int c, int d);        // 0x801ED3FC
void TextDraw(const char* s, u32 len);              // 0x801EB11C
const char* StrFindKey(u32 key, u32* len);          // 0x80086234
const char* StrGetIndex(int idx, u32* len);         // 0x800862F0
int* fn_8014FEE4(int i);
int VarObjKey(u32 table, int index);                // 0x800DF160
int fn_80007138(void);
void fn_800C6834(int ctl);
void fn_800C5FB4(float f, int a);
void DeferFlush3(void);                             // 0x800F0ED4
void DeferFlush4(void);                             // 0x800F0DFC
void fn_80009E10(void);
void fn_80008F64(void);
void fn_800C6664(int ctl);
int SendMsg(int to, int msg, void* data);
int Fn800DF814(u32 key, int value);                 // 0x800DF814: index of a record by name
int Fn800DF3B8(u32 key, int index, u32 field);      // 0x800DF3B8: record field
char* VarObjData(u32 key, int index);               // 0x800DF8A4
int VarGetHash(u32 key, int index, u32 field);      // 0x800DF1D8
int GameEvent(int a, int b);                        // 0x8007EA84
int AssetFind(u32 a, u32 b);                        // 0x8007EB34
int SaveCommand(int a, int b);                      // 0x800DE2F8
int fn_802A1FA0(void);
int BitsetCount(u32* bits, int a, int b);           // 0x800259C8
int PadPressed(int a, int b, int c);                // 0x800DC930
int FxStart(int a, int b, int c);                   // 0x8016F390
void GhostActionSetActive(int a);                   // 0x80215390
int fn_80026660(int a, void* fn, int b);
void fn_8027E954(void);
void fn_8028253C(float dt);
void fn_80281EE8(float dt);
void fn_80281894(float dt);
void fn_80282BA4(float dt);
void fn_80283310(float dt);
void fn_80284CE0(void);
}

extern int gMovieCount;         // 0x804D55BC
extern int gMovieSel;           // 0x804D55C4
extern int gMovieBusy;          // 0x804D55C8
extern int gExtrasModel;        // 0x804D55CC
extern int gExtrasCam;          // 0x804D55EC
extern int gHudLines;           // 0x804D7BD0
extern float gExtrasZoom;       // 0x804D3788
extern int gMusicSel;           // 0x804D55AC
extern int gMusicBusy;          // 0x804D55B0
extern int gMusicModel;         // 0x804D55B4
extern int gArtSel;             // 0x804D5594
extern int gArtBusy;            // 0x804D5598
extern int gArtModel;           // 0x804D559C
extern int gMenuPage;           // 0x804D55F0
extern int gMenuInPage;         // 0x804D55F4
extern int gMenuFxMove;         // 0x804D5604
extern int gMenuFxSelect;       // 0x804D5608
extern int gMenuFxBack;         // 0x804D560C
extern int gPadDown;            // 0x803C0CB0
extern int gPadUp;              // 0x803C0CB4

extern u32* gCheatBits;         // 0x804D55E4
extern u32* gMovieBits;         // 0x804D55B8
extern u32* gMusicBits;         // 0x804D55A0
extern int gMusicTotal;         // 0x804D55A8
extern int gMusicCount;         // 0x804D55A4

extern u32* gArtBitsA;          // 0x804D558C
extern int gArtCountA;          // 0x804D5590
extern u32* gArtBitsB;          // 0x804D5580
extern int gArtCountB;          // 0x804D5584
extern u32* gBonusBits;         // 0x804D5558
extern int gBonusCount;         // 0x804D555C
extern u32* gBonusBits2;        // 0x804D556C
extern int gBonusCount2;        // 0x804D5570
extern u32* gSecretBits;        // 0x804D55D0
extern int gSecretCount;        // 0x804D55D4

// Functions called from other units under their placeholder names (names in comments).
extern "C" u32 fn_802810BC(int key);
extern "C" void fn_80281124(int key);
extern "C" u32 fn_802837C4(int event);
extern "C" void fn_80283864(u32 event);
extern "C" void fn_80283A44(u32 event);
extern "C" void fn_80283DC4(u32 value);
extern "C" void fn_80284CB0(void);
extern "C" void fn_802845CC(void);

void ExtrasModelShow(int model, u8 alpha);

static inline int BitTest(u32* bits, int i) {
    return (1 << (i & 0x1F)) & bits[i / 32];
}

static inline void BitSet(u32* bits, int i) {
    bits[i / 32] |= 1 << (i & 0x1F);
}

// Page dispatcher of the extras menu: pad up/down moves between pages, A enters one.
void ExtrasMenuUpdate(float dt) {
    int old = gMenuPage;
    int page;
    fn_80284CE0();
    if (gMenuInPage == 0) {
        if (gPadUp == 1) {
            gMenuPage++;
        }
        if (gPadDown == 1) {
            gMenuPage--;
        }
        if (gMenuPage < 0) {
            page = 0;
        } else {
            page = 4;
            if (gMenuPage < 5) {
                page = gMenuPage;
            }
        }
        gMenuPage = page;
        if (old != page) {
            FxStart(gMenuFxMove, 0, 0);
        }
        if (PadPressed(0, 0, 3) == 1) {
            gMenuInPage = 1;
            FxStart(gMenuFxSelect, 0, 0);
        }
        if (PadPressed(0, 0, 4) == 1) {
            GhostActionSetActive(0);
            FxStart(gMenuFxBack, 0, 0);
        }
    } else if (gMenuPage == 0) {
        fn_80282BA4(dt);
    } else if (gMenuPage == 1) {
        fn_8028253C(dt);
    } else if (gMenuPage == 2) {
        fn_80281EE8(dt);
    } else if (gMenuPage == 3) {
        fn_80281894(dt);
    } else if (gMenuPage == 4) {
        fn_80283310(dt);
    } else {
        gMenuInPage = 0;
    }
}

// Is the cheat named `key` unlocked?
// CheatIsUnlocked
u32 fn_802810BC(int key) {
    int idx = Fn800DF814(0x872E838C, key);
    if (idx == -1 || gCheatBits == NULL) {
        return 0;
    }
    return BitTest(gCheatBits, idx);
}

// Unlocks the cheat named `key` and queues a save.
// CheatUnlock
void fn_80281124(int key) {
    int idx = Fn800DF814(0x872E838C, key);
    if (idx != -1 && gCheatBits != NULL) {
        BitSet(gCheatBits, idx);
        SaveCommand(1, fn_802A1FA0());
        SaveCommand(6, 1);
    }
}

// Fraction (0..1+) of the music tracks heard.
double MusicProgress(void) {
    float base = 1.0f;
    if (gMusicBits != NULL) {
        u32 pct;
        if (gMusicBits == NULL || gMusicTotal == 0) {
            pct = 0;
        } else {
            pct = (BitsetCount(gMusicBits, gMusicCount, 1) * 100) / gMusicTotal;
        }
        base = base + (float)(int)pct / 100.0f;
    }
    return base;
}

// Is movie `key` unlocked?
int MovieIsUnlocked(int key) {
    u32 idx;
    if (gMovieBits != NULL && (idx = Fn800DF814(0xD8E68AB7, key)) != 0xFFFFFFFF &&
        BitTest(gMovieBits, idx) != 0) {
        return 1;
    }
    return 0;
}

// Plays the movie or reports the lock (movie list).
void MoviePlay(u32 idx) {
    if (BitTest(gMovieBits, idx) != 0) {
        Fn800DF3B8(0xD8E68AB7, idx, 0x27873A);
    } else {
        AssetFind(0xF2E2BEB8, 0x52BE59C3);
    }
}

// Plays the music track or reports the lock (music list).
void MusicPlay(u32 idx) {
    if (BitTest(gMusicBits, idx) != 0) {
        Fn800DF3B8(0x258124CE, idx, 0x27873A);
    } else {
        AssetFind(0xF2E2BEB8, 0x52BE59C3);
    }
}

// Marks movie `key` unlocked if its record says it starts unlocked.
void MovieMarkUnlocked(int key) {
    if (gMovieBits != NULL) {
        int idx = Fn800DF814(0xD8E68AB7, key);
        char* data = VarObjData(0xD8E68AB7, idx);
        if (idx != -1) {
            if (data != NULL && *data != 0) {
                BitSet(gMovieBits, idx);
            }
        }
    }
}

// Same for the music list.
void MusicMarkUnlocked(int key) {
    if (gMusicBits != NULL) {
        int idx = Fn800DF814(0x258124CE, key);
        char* data = VarObjData(0x258124CE, idx);
        if (idx != -1) {
            if (data != NULL && *data != 0) {
                BitSet(gMusicBits, idx);
            }
        }
    }
}

// Draws the movie / music hint list in the corner and, while a movie is selected, its title.
void ExtrasHudDraw(int alpha) {
    static int sFade = 0;               // 0x804D561C
    int y = 0x17C;
    int played;
    if (gMovieBits == NULL || gMovieCount == 0) {
        played = 0;
    } else {
        played = BitsetCount(gMovieBits, gMovieCount, 1);
    }
    if (gMovieBits == NULL) {
        gMenuInPage = 0;
    } else if (gMovieBusy == 0) {
        int i;
        int fade = sFade + 1;
        sFade = 0xFF;
        if (fade < 0x100) {
            sFade = fade;
        }
        for (i = 0; i < gHudLines - 1; i++) {
            int* line = fn_8014FEE4(i);
            u32 len = 0;
            int left = line[2] - played;
            if (line == NULL) {
                break;
            }
            if (left < 3) {
                int a;
                TextBegin();
                TextSetMode(0);
                a = (sFade * 0xFF) / 0xFF;
                TextSetColor(0xFF, 0xFF, 0xFF, a & 0xFF);
                TextSetSize(0xD);
                TextSetBox(0xA5, 0x28A, 0, y);
                if (left == 1) {
                    TextSetColor(0xFF, 0xFF, 0, ((sFade * 0xFF) / 0xFF) & 0xFF);
                    TextDraw(StrFindKey(0x4B2687A6, &len), len);
                    y -= 0x14;
                    TextBegin();
                    TextSetMode(0);
                    TextSetColor(0x28, 0x9D, 0x5A, ((sFade * 0xFF) / 0xFF) & 0xFF);
                    TextSetSize(0xD);
                    TextSetBox(0xA5, 0x28A, 0, y);
                } else if (left == 2) {
                    TextSetColor(0xFF, 0xFF, 0, ((sFade * 0xFF) / 0xFF) & 0xFF);
                    TextDraw(StrFindKey(0xDA18CB28, &len), len);
                    y -= 0x14;
                    TextBegin();
                    TextSetMode(0);
                    TextSetColor(0x28, 0x9D, 0x5A, ((sFade * 0xFF) / 0xFF) & 0xFF);
                    TextSetSize(0xD);
                    TextSetBox(0xA5, 0x28A, 0, y);
                }
                TextDraw(StrFindKey(line[4], &len), len);
                y -= 0x14;
            }
        }
    } else {
        u32 len = 0;
        int title = Fn800DF3B8(0xD8E68AB7, gMovieSel, 0x273D2D);
        sFade = 0;
        if (title >= 0) {
            TextBegin();
            TextSetMode(0);
            TextSetColor(0xFF, 0xFF, 0xFF, ((alpha * 0xFF) / 0xFF) & 0xFF);
            TextSetSize(0xD);
            TextSetBox(400, 0x262, 0x50, 0x168);
            TextDraw(StrGetIndex(title, &len), len);
        }
        ExtrasModelShow(gExtrasModel, alpha);
    }
}

struct LevelGate {
    u32 event;              // game event that unlocks the entry
    int level;              // first level whose progress is needed
};

extern LevelGate gLevelGates[13];       // 0x8034B0D8
extern int gProgressLevel;              // 0x804D55F8
extern int gRepeatTimer;                // 0x804D5600
extern int gPadLeftEdge;                // 0x803C0CA8
extern int gPadRightEdge;               // 0x803C0CAC

extern "C" {
void Vec3Set(float* v, float x, float y, float z);                  // 0x8016D67C
int fn_800C7E80(float* spawn);
void fn_800C7B14(int model);
void ActorRender(void);                                             // 0x800C7CE8
float VarGetFloat(u32 table, int index, u32 key);                   // 0x800DF2C8
}

// Is list entry `i` of `table` hidden (no data record)?
static inline int ExtrasHidden(u32 table, int i) {
    const char* d = VarObjData(table, i);
    return d == NULL || *d == 0;
}

// Finds the first level whose gate matches the entry's game event: returns the gate index or -1.
static inline int ExtrasGateFor(u32 table, int i) {
    int ev = GameEvent(0xBAA22E62, Fn800DF3B8(table, i, 0x273D2D));
    int k;
    for (k = 0; k < 13; k++) {
        if (ev == (int)gLevelGates[k].event && k < gProgressLevel + 1) {
            return k;
        }
    }
    return -1;
}

// One page of the extras list menu (movies, music, concept art): moves the highlight with the
// pad skipping locked rows, starts the preview model of the chosen entry and shows it.
static inline void ExtrasListPage(float dt, u32 table, u32 evTable, u32 nameKey, u32* bits, int count,
                                  int& sel, int& busy, int& model) {
    int old = sel;
    if (bits == NULL) {
        gMenuInPage = 0;
        return;
    }
    if (busy == 0) {
        if (gPadLeftEdge == 1) {
            if (sel == -1) {
                return;
            }
            sel--;
            if (ExtrasHidden(table, sel)) {
                sel--;
            }
        }
        if (gPadRightEdge == 1) {
            if (sel == -1) {
                return;
            }
            sel++;
            if (sel < count && ExtrasHidden(table, sel)) {
                int i = sel;
                int found = -1;
                for (; i >= 0; i--) {
                    if (ExtrasHidden(table, i) && ExtrasGateFor(table, i) != -1) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    sel--;
                } else {
                    sel++;
                }
            }
        }
        if (PadPressed(0, 0, 0xB) == 1 && gRepeatTimer > 10) {
            int i;
            gRepeatTimer = 0;
            for (i = sel; i < count - 1; i++) {
                if (ExtrasHidden(table, i)) {
                    break;
                }
            }
            sel = ExtrasGateFor(table, i) == -1 ? i - 1 : i + 1;
        }
        if (PadPressed(0, 0, 10) == 1 && gRepeatTimer > 10) {
            int i;
            int seen = 0;
            gRepeatTimer = 0;
            for (i = sel; i >= 0; i--) {
                if (ExtrasHidden(table, i)) {
                    if (seen) {
                        sel = i + 1;
                        break;
                    }
                    seen = 1;
                }
            }
        }
        gRepeatTimer++;
        if (sel < 1) {
            sel = 1;
        } else if (sel > count - 1) {
            sel = count - 1;
        }
        if (PadPressed(0, 0, 3) == 1 && sel != -1 && BitTest(bits, sel) != 0) {
            int ev = GameEvent(evTable, Fn800DF3B8(table, sel, nameKey));
            if (ev == 0) {
                model = 0;
            } else {
                float spawn[12];
                float pos[3];
                spawn[0] = 0.0f;
                spawn[1] = 0.0f;
                *(int*)&spawn[2] = ev;
                pos[2] = VarGetFloat(table, sel, 0x5A);
                pos[1] = VarGetFloat(table, sel, 0x59);
                pos[0] = VarGetFloat(table, sel, 0x58);
                Vec3Set(&spawn[6], VarGetFloat(table, sel, 0x5A3309E2), VarGetFloat(table, sel, 0x489CB20), 0.0f);
                {
                    int act = fn_800C7E80(pos);
                    if (act == 0) {
                        model = 0;
                    } else {
                        SendMsg(*(int*)(act + 0x38), 6, spawn);
                        model = *(int*)(act + 0x38);
                    }
                }
            }
            if (model != 0) {
                busy = 1;
                FxStart(gMenuFxSelect, 0, 0);
            }
        }
        if (PadPressed(0, 0, 4) == 1) {
            FxStart(gMenuFxBack, 0, 0);
            gMenuInPage = 0;
        }
    } else {
        if (PadPressed(0, 0, 4) == 1) {
            busy = 0;
            FxStart(gMenuFxBack, 0, 0);
            if (model != 0) {
                int m = model;
                int msg[2];
                msg[1] = 0;
                SendMsg(model, 6, msg);
                fn_800C7B14(m);
                ActorRender();
            }
            model = 0;
        }
    }
    if (old != sel) {
        FxStart(gMenuFxMove, 0, 0);
    }
    if (model != 0) {
        float msg[2];
        msg[1] = dt;
        SendMsg(model, 0x414, msg);
    }
}

// Movie list page.
void ExtrasMoviePage(float dt) {
    ExtrasListPage(dt, 0xD8E68AB7, 0x2F9DF01F, 0xD83EE6B4, gMovieBits, gMovieCount, gMovieSel, gMovieBusy,
                   gExtrasModel);
}

// Music list page.
void ExtrasMusicPage(float dt) {
    ExtrasListPage(dt, 0x258124CE, 0x2F9DF01F, 0xD83EE6B4, gMusicBits, gMusicCount, gMusicSel, gMusicBusy,
                   gMusicModel);
}

// Concept art list page.
void ExtrasArtPage(float dt) {
    ExtrasListPage(dt, 0x80EDD05C, 0x2F9DF01F, 0xD83EE6B4, gArtBitsB, gArtCountB, gArtSel, gArtBusy,
                   gArtModel);
}

// Is the concept-art entry with game event `event` unlocked? (1 when it is not in the list.)
// ArtIsUnlocked
u32 fn_802837C4(int event) {
    u32 i = 0;
    while (true) {
        if (gArtCountA <= (int)i) {
            return 1;
        }
        if (event == GameEvent(0x2F9DF01F, Fn800DF3B8(0x80EDD05C, i, 0xD83EE6B4))) {
            break;
        }
        i++;
    }
    return BitTest(gArtBitsA, i);
}

// Unlocks every art entry whose game event is `event` and has the "starts unlocked" flag;
// queues a save when none was found.
// ArtMarkUnlocked
void fn_80283864(u32 event) {
    u32 i;
    int field;
    char* data;
    u32 found = -1;
    for (i = 0; (int)i < gArtCountA; i++) {
        field = Fn800DF3B8(0x80EDD05C, i, 0xD83EE6B4);
        data = VarObjData(0x80EDD05C, i);
        if (event == GameEvent(0x2F9DF01F, field)) {
            if (data != NULL && *data != 0) {
                BitSet(gArtBitsA, i);
                found = event;
            }
        }
    }
    if (found != event) {
        SaveCommand(1, fn_802A1FA0());
        SaveCommand(6, 1);
    }
}

// Is there an art entry for game event `event`?
int ArtExists(int event) {
    int i = 0;
    while (true) {
        if (gArtCountA <= i) {
            return 0;
        }
        if (event == GameEvent(0x2F9DF01F, Fn800DF3B8(0x80EDD05C, i, 0xD83EE6B4))) {
            break;
        }
        i++;
    }
    return 1;
}

// Shows art entry `idx` or reports the lock.
void ArtShow(u32 idx) {
    if (BitTest(gArtBitsB, idx) != 0) {
        Fn800DF3B8(0x80EDD05C, idx, 0x27873A);
    } else {
        AssetFind(0xF2E2BEB8, 0x52BE59C3);
    }
}

// ArtMarkUnlockedB
void fn_80283A44(u32 event) {
    u32 i;
    int field;
    char* data;
    u32 found = -1;
    for (i = 0; (int)i < gArtCountB; i++) {
        field = Fn800DF3B8(0x80EDD05C, i, 0xD83EE6B4);
        data = VarObjData(0x80EDD05C, i);
        if (event == GameEvent(0x2F9DF01F, field)) {
            if (data != NULL && *data != 0) {
                BitSet(gArtBitsB, i);
                found = event;
            }
        }
    }
    if (found != event) {
        SaveCommand(1, fn_802A1FA0());
        SaveCommand(6, 1);
    }
}

// Returns the image field of art entry `event`, or 0.
int ArtImage(int event) {
    int i = 0;
    while (true) {
        if (gArtCountA <= i) {
            return 0;
        }
        if (event == GameEvent(0x2F9DF01F, Fn800DF3B8(0x80EDD05C, i, 0xD83EE6B4))) {
            break;
        }
        i++;
    }
    return Fn800DF3B8(0x80EDD05C, i, 0x198917DC);
}

void BonusShow(u32 idx) {
    if (BitTest(gBonusBits, idx) != 0) {
        Fn800DF3B8(0x5E130113, idx, 0x27873A);
    } else {
        AssetFind(0xF2E2BEB8, 0x52BE59C3);
    }
}

u32 Bonus2IsUnlocked(int event) {
    u32 i = 0;
    while (true) {
        if (gBonusCount2 <= (int)i) {
            return 0;
        }
        if (event == GameEvent(0x78198479, Fn800DF3B8(0x5E130113, i, 0xDF714172))) {
            break;
        }
        i++;
    }
    return BitTest(gBonusBits2, i);
}

void Bonus2MarkUnlocked(u32 event) {
    u32 i;
    int field;
    char* data;
    u32 found = -1;
    for (i = 0; (int)i < gBonusCount2; i++) {
        field = Fn800DF3B8(0x5E130113, i, 0xDF714172);
        data = VarObjData(0x5E130113, i);
        if (event == GameEvent(0x78198479, field)) {
            if (data != NULL && *data != 0) {
                BitSet(gBonusBits2, i);
                found = event;
            }
        }
    }
    if (found != event) {
        SaveCommand(1, fn_802A1FA0());
        SaveCommand(6, 1);
    }
}

// BonusMarkUnlocked
void fn_80283DC4(u32 value) {
    u32 i;
    int field;
    char* data;
    u32 found = -1;
    for (i = 0; (int)i < gBonusCount; i++) {
        field = VarGetHash(0x5E130113, i, 0xDF714172);
        data = VarObjData(0x5E130113, i);
        if (field == value) {
            if (data != NULL && *data != 0) {
                BitSet(gBonusBits, i);
                found = value;
            }
        }
    }
    if (found != value) {
        SaveCommand(1, fn_802A1FA0());
        SaveCommand(6, 1);
    }
}

void SecretMarkUnlocked(u32 event) {
    u32 i;
    int field;
    int ev;
    char* data;
    u32 found = -1;
    for (i = 0; (int)i < gSecretCount; i++) {
        field = Fn800DF3B8(0x5EB7A22E, i, 0x273D2D);
        ev = GameEvent(0xBAA22E62, field);
        data = VarObjData(0x5EB7A22E, i);
        if (ev == event) {
            if (data != NULL && *data != 0) {
                BitSet(gSecretBits, i);
                found = event;
            }
        }
    }
    if (found != -1) {
        SaveCommand(1, fn_802A1FA0());
        SaveCommand(6, 1);
    }
}

void SecretShow(u32 idx) {
    if (BitTest(gSecretBits, idx) != 0) {
        AssetFind(0xF2E2BEB8, 0x6983CAD1);
    } else {
        AssetFind(0xF2E2BEB8, 0x52BE59C3);
    }
}

extern "C" {
void Draw2DBegin(void);                                             // 0x8013C6F0
void Draw2DEnd(void);                                               // 0x8013C6C4
void DrawTexQuad(int tex, float x0, float y0, float x1, float y1, u8 r, u8 g, u8 b, u8 a);  // 0x8013C308
void GhostShotDrawSprite(float a, float b, float c, float d, int tex, int x, int y, int w, int h,
                         u8 r, u8 g, u8 bl, u8 al);                 // 0x8018F880
void TextDrawStr(const char* s);                                    // 0x801EB188
int sprintf(char* s, const char* fmt, ...);
}
extern int gArrowTexL;          // 0x804A3278
extern int gArrowTexR;          // 0x804A327C

// Draws one window of a list page: the title of the current entry, up to six rows with their
// icons (`iconFn`), the "This Level (n of m)" counter and the scroll arrows.
void ExtrasListDraw(u32 sel, int count, int (*iconFn)(u32), int alpha, u32 table, u32 nameKey,
                    u32* bits) {
    u32 first = 0xFFFFFFFF;
    int k;
    if (iconFn == NULL) {
        return;
    }
    for (k = sel; k >= 0; k--) {
        if (ExtrasHidden(table, k)) {
            if (ExtrasGateFor(table, k) != -1) {
                first = k;
            }
            break;
        }
    }
    {
        u32 len = 0;
        int total = 0;
        int have = 0;
        if (first != 0xFFFFFFFF) {
            int more = 0;
            u32 i;
            u32 start;
            u32 end = count;
            u32 a8;
            Draw2DBegin();
            TextBegin();
            TextSetMode(0);
            TextSetColor(0xFF, 0xFF, 0xFF, 0xFF);
            TextSetSize(0xD);
            TextSetBox(0xA5, 0xFF, 0x1A4, 0x1C4);
            TextDraw(StrGetIndex(Fn800DF3B8(table, first, nameKey), &len), len);
            start = first + 1;
            i = start;
            while ((int)i < count && !ExtrasHidden(table, i)) {
                total++;
                if (BitTest(bits, i) != 0) {
                    have++;
                }
                i++;
            }
            if ((int)(sel - start) > 4 && total > 5) {
                start = sel - 2;
            }
            if ((int)(start + 6) < (int)end) {
                end = start + 6;
            }
            a8 = (u32)(((alpha * 0xFF) / 0xFF)) & 0xFF;
            for (i = start; (int)i < (int)end; i++) {
                int y = (i - start) * 0x28;
                if (ExtrasHidden(table, i)) {
                    break;
                }
                if (i == sel) {
                    DrawTexQuad(-3, (float)(y + 0x13E), 418.0f, (float)(y + 0x162), 454.0f, 0xFF, 0xFF, 0xFF, a8);
                }
                DrawTexQuad(iconFn(i), (float)(y + 0x140), 420.0f, (float)(y + 0x160), 452.0f, 0xFF, 0xFF,
                            0xFF, a8);
            }
            if (total > 0) {
                char buf[68];
                sprintf(buf, "This Level (%d of %d)    Item Number %d", have, total, sel - first);
                TextBegin();
                TextSetMode(0);
                TextSetColor(0xFF, 0xFF, 0xFF, 0xFF);
                TextSetSize(0xD);
                TextSetBox(0xA5, 600, 0x17C, 400);
                TextDrawStr(buf);
            }
            if ((int)i < (int)end) {
                int ev = GameEvent(0xBAA22E62, Fn800DF3B8(table, i, nameKey));
                if (ev != (int)gLevelGates[gProgressLevel + 1].event) {
                    more = 1;
                }
            }
            Draw2DEnd();
            if (more) {
                GhostShotDrawSprite(0.0f, 0.0f, 0.0f, 0.0f, gArrowTexL, 600, 0x1B8, 0x2E, 0x1B, 0xFF, 0xFF,
                                    0xFF, 0xFF);
            }
            if (first != 0) {
                GhostShotDrawSprite(0.0f, 0.0f, 0.0f, 0.0f, gArrowTexR, 0x118, 0x1B8, 0x2E, 0x1B, 0xFF, 0xFF,
                                    0xFF, 0xFF);
            }
        }
    }
}

// Shows the 3D model of the selected extra with the given alpha.
void ExtrasModelShow(int model, u8 alpha) {
    if (fn_80007138() == 0 && model != 0) {
        int msg[0x88];
        fn_800C6834(gExtrasCam);
        fn_800C5FB4(gExtrasZoom, 0);
        SendMsg(model, 0, msg);
        DeferFlush3();
        DeferFlush4();
        fn_80009E10();
        fn_80008F64();
        fn_800C6664(gExtrasCam);
    }
}

// Tells every unlocked music track and movie object to appear (message 0xC00).
void fn_802845CC(void) {
    int i;
    if (gMusicBits != NULL) {
        for (i = 0; i < gMusicCount; i++) {
            if (BitTest(gMusicBits, i) != 0) {
                int key = VarObjKey(0x258124CE, i);
                int h = AssetFind(0x110A6, key);
                int msg[2];
                msg[1] = 0;
                SendMsg(h, 0xC00, msg);
            }
        }
    }
    if (gMovieBits != NULL) {
        for (i = 0; i < gMovieCount; i++) {
            if (BitTest(gMovieBits, i) != 0) {
                int key = VarObjKey(0xD8E68AB7, i);
                int h = AssetFind(0x110A6, key);
                int msg[2];
                msg[1] = 0;
                SendMsg(h, 0xC00, msg);
            }
        }
    }
}

// Registers the extras message handler (type 99).
// ExtrasMsgRegister
void fn_80284CB0(void) {
    fn_80026660(99, (void*)fn_8027E954, 0);
}

extern "C" int FxFind(const char* name);        // 0x80170834

// Looks up the menu sounds.
void ExtrasMenuFxInit(void) {
    gMenuFxMove = FxFind("Navigate");
    gMenuFxSelect = FxFind("Select");
    gMenuFxBack = FxFind("Back");
}

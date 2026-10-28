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

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(u32 h, const char* s) {
    const char* p;
    for (p = s; *p != 0; p++) {
        h = h * 31 + (unsigned char)toupper(*p);
    }
    h = h % 0xFFFFFFFF;
    return h;
}

extern "C" {
void* memset(void* p, int c, u32 n);
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
extern int gSecretSel;          // 0x804D55DC
extern int gSecretBusy;         // 0x804D55E0
extern int gBonusSel;           // 0x804D5574
extern int gBonusBusy;          // 0x804D5578
extern int gBonusModel;         // 0x804D557C
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
extern "C" void fn_802846F0(void);

void ExtrasModelShow(int model, u8 alpha);

static inline int BitTest(u32* bits, int i) {
    return (1 << (i & 0x1F)) & bits[i / 32];
}

static inline void BitSet(u32* bits, int i) {
    bits[i / 32] |= 1 << (i & 0x1F);
}

extern "C" {
void GhostShotDrawBeam(float a, float b, float c, float d, float e, int tex, int x0, int y0, int x1,
                       int y1, u8 r, u8 g, u8 bl, u8 al);           // 0x8018F56C
const char* StrFindKeySkip(u32 key);                                // 0x80086364
int sprintf(char* s, const char* fmt, ...);
void TextDrawStr(const char* s);                                    // 0x801EB188
void Draw2DBegin(void);                                             // 0x8013C6F0
void Draw2DEnd(void);                                               // 0x8013C6C4
void DrawTexQuad(int tex, float x0, float y0, float x1, float y1, u8 r, u8 g, u8 b, u8 a);  // 0x8013C308
}
void BonusShow(u32 idx);
void ArtShow(u32 idx);
void MusicPlay(u32 idx);
void MoviePlay(u32 idx);
void SecretShow(u32 idx);
void ExtrasListDraw(u32 sel, int count, int (*iconFn)(u32), int alpha, u32 table, u32 nameKey,
                    u32* bits);
void ExtrasHudDraw(int alpha);
extern int gMenuReady;          // 0x804D5550
extern int gWideScreen;         // 0x804D4008
extern int gBonusUnlockCount;   // 0x804D5560
extern int gArtHidden;          // 0x804D5588
extern int gMusicVisible;       // 0x804D55A8
extern int gMovieVisible;       // 0x804D55C0
extern int gSecretVisible;      // 0x804D55D8
extern int gArrowTexUp;         // 0x804A3280
extern int gArrowTexDown;       // 0x804A3284
extern float gOne2;             // 0x804DA418
extern float gZero2;            // 0x804DA41C

// Draws the extras menu: the arrows and the five category percentages on the left, the
// highlight bar for the current page and then the page itself.
int ExtrasMenuDraw(int unused, int alpha) {
    static int sRowX = 10;              // 0x804D5624
    static int sRowY = 0x94;            // 0x804D562C
    static int sFade = 0;               // 0x804D5634
    static int sPageFlags = 0;          // 0x804D563C
    static u8 sTitleFade = 0;           // 0x804D5641
    char buf[256];
    char cbuf[64];
    int pct;
    int selBonus = gBonusBusy;
    int selArt = gArtBusy;
    int selMusic = gMusicBusy;
    int selMovie = gMovieBusy;
    int selSecret = gSecretBusy;
    signed char fade;

    if (fn_80007138() == 0) {
        gMenuReady = 1;
    } else if (gMenuReady == 0) {
        sTitleFade = 10;
        return 1;
    }
    fade = sTitleFade + 3;
    sTitleFade = 0xFF;
    if (fade > 4) {
        sTitleFade = fade;
    }
    if (gWideScreen == 0) {
        GhostShotDrawBeam(gOne2, gZero2, gZero2, gZero2, gZero2, gArrowTexUp, 400, 0x1C2, 0x1DD, 0x5A,
                          0xFF, 0xFF, 0xFF, sTitleFade);
        GhostShotDrawBeam(gOne2, gZero2, gZero2, gZero2, gZero2, gArrowTexDown, 0x50, 0xF0, 0x8E, 0x1E0,
                          0xFF, 0xFF, 0xFF, sTitleFade);
    } else {
        GhostShotDrawBeam(gOne2, gZero2, gZero2, gZero2, gZero2, gArrowTexUp, 400, 0x1C2, 0x26C, 0x5A,
                          0xFF, 0xFF, 0xFF, sTitleFade);
        GhostShotDrawBeam(gOne2, gZero2, gZero2, gZero2, gZero2, gArrowTexDown, 0x50, 0xF0, 200, 0x1E0,
                          0xFF, 0xFF, 0xFF, sTitleFade);
    }
    TextBegin();
    TextSetMode(2);
    TextSetSize(0xD);
    TextSetColor(0xFF, 0xFF, 0xFF, sTitleFade);
    TextSetBox(10, 0xA0, 0x15E, 0x1A4);
    pct = (gBonusBits == NULL) ? 0 : (BitsetCount(gBonusBits, gBonusCount, 1) * 100) / gBonusUnlockCount;
    sprintf(buf, "%s\n%d%", StrFindKeySkip(0x98280222), pct);
    TextDrawStr(buf);
    TextSetBox(10, 0xA0, 0x10E, 0x154);
    pct = (gArtBitsB == NULL) ? 0 : (BitsetCount(gArtBitsB, gArtCountB, 1) * 100) / gArtHidden;
    sprintf(buf, "%s\n%d%", StrFindKeySkip(0x89C6C9AB), pct);
    TextDrawStr(buf);
    TextSetBox(10, 0xA0, 0xBE, 0x104);
    pct = (gMusicBits == NULL || gMusicTotal == 0) ? 0 : (BitsetCount(gMusicBits, gMusicCount, 1) * 100) / gMusicTotal;
    sprintf(buf, "%s\n%d%", StrFindKeySkip(0x1AA0E630), pct);
    TextDrawStr(buf);
    TextSetBox(10, 0xA0, 0x6E, 0xB4);
    pct = (gMovieBits == NULL || gMovieVisible == 0) ? 0 : (BitsetCount(gMovieBits, gMovieCount, 1) * 100) / gMovieVisible;
    sprintf(buf, "%s\n%d%", StrFindKeySkip(0x695DB0A5), pct);
    TextDrawStr(buf);
    TextSetBox(10, 0xA0, 0x1E, 100);
    if (gSecretBits == NULL) {
        pct = 0;
    } else {
        pct = (int)(float)((BitsetCount(gSecretBits, gSecretCount, 1) * 100) / gSecretVisible);
    }
    sprintf(buf, "%s\n%d%", StrFindKeySkip(0x98CCA33D), pct);
    TextDrawStr(buf);
    Draw2DBegin();
    DrawTexQuad(-3, (float)sRowX, (float)(gMenuPage * -0x50 + 0x15E), (float)sRowY,
                (float)(gMenuPage * -0x50 + 0x160), 0x2C, 0x8F, 0x6D, sTitleFade);
    DrawTexQuad(-3, (float)sRowX, (float)(gMenuPage * -0x50 + 0x1A4), (float)sRowY,
                (float)(gMenuPage * -0x50 + 0x1A2), 0x2C, 0x8F, 0x6D, sTitleFade);
    DrawTexQuad(-3, (float)sRowY, (float)(gMenuPage * -0x50 + 0x15E), (float)(sRowY + 2),
                (float)(gMenuPage * -0x50 + 0x1A4), 0x2C, 0x8F, 0x6D, sTitleFade);
    Draw2DEnd();
    if (gMenuInPage == 0) {
        sFade = 0;
        sPageFlags = 0;
    } else if (gMenuPage == 0) {
        if (fn_80007138() != 0) {
            sFade = 0;
        }
        if (selBonus == 1) {
            if ((sPageFlags & 2) == 0) {
                sFade = 0;
                sPageFlags = 2;
            } else {
                int f = sFade + 1;
                sFade = 0xFF;
                if (f < 0x100) {
                    sFade = f;
                }
            }
        } else {
            sFade = 0;
        }
        ExtrasListDraw(gBonusSel, gBonusCount, (int (*)(u32))BonusShow, alpha, 0x5E130113, 0x198917DC,
                       gBonusBits);
        if (gBonusBits == NULL) {
            gMenuInPage = 0;
        } else if (gBonusBusy != 0) {
            u32 len = 0;
            int title = Fn800DF3B8(0x5E130113, gBonusSel, 0x198917DC);
            if (title >= 0) {
                TextBegin();
                TextSetMode(0);
                TextSetColor(0xFF, 0xFF, 0xFF, ((sFade * 0xFF) / 0xFF) & 0xFF);
                TextSetSize(0xD);
                TextSetBox(400, 0x262, 0x14, 0x168);
                TextDraw(StrGetIndex(title, &len), len);
            }
            ExtrasModelShow(gBonusModel, sFade);
        }
    } else if (gMenuPage == 1) {
        if (fn_80007138() != 0) {
            sFade = 0;
        }
        if (selArt == 1) {
            if ((sPageFlags & 4) == 0) {
                sFade = 0;
                sPageFlags = 4;
            } else {
                int f = sFade + 1;
                sFade = 0xFF;
                if (f < 0x100) {
                    sFade = f;
                }
            }
        } else {
            sFade = 0;
        }
        ExtrasListDraw(gArtSel, gArtCountB, (int (*)(u32))ArtShow, alpha, 0x80EDD05C, 0x198917DC, gArtBitsB);
        if (gArtBitsB == NULL) {
            gMenuInPage = 0;
        } else if (gArtBusy != 0) {
            u32 len = 0;
            int title = Fn800DF3B8(0x80EDD05C, gArtSel, 0x198917DC);
            if (title >= 0) {
                TextBegin();
                TextSetMode(0);
                TextSetColor(0xFF, 0xFF, 0xFF, ((sFade * 0xFF) / 0xFF) & 0xFF);
                TextSetSize(0xD);
                TextSetBox(400, 0x262, 0x50, 0x168);
                TextDraw(StrGetIndex(title, &len), len);
            }
            ExtrasModelShow(gArtModel, sFade);
        }
    } else if (gMenuPage == 2) {
        if (fn_80007138() != 0) {
            sFade = 0;
        }
        if (selMusic == 1) {
            if ((sPageFlags & 0x20) == 0) {
                sFade = 0;
                sPageFlags = 0x20;
            } else {
                int f = sFade + 1;
                sFade = 0xFF;
                if (f < 0x100) {
                    sFade = f;
                }
            }
        } else {
            sFade = 0;
        }
        ExtrasListDraw(gMusicSel, gMusicCount, (int (*)(u32))MusicPlay, alpha, 0x258124CE, 0x273D2D, gMusicBits);
        if (gMusicBits == NULL) {
            gMenuInPage = 0;
        } else if (gMusicBusy != 0) {
            u32 len = 0;
            int title = Fn800DF3B8(0x258124CE, gMusicSel, 0x273D2D);
            if (title >= 0) {
                u32 heard;
                if (gMusicBits == NULL || gMusicTotal == 0) {
                    heard = 0;
                } else {
                    heard = BitsetCount(gMusicBits, gMusicCount, 1);
                }
                sprintf(cbuf, "\n\n(%d of %d)", heard, gMusicTotal);
                TextBegin();
                TextSetMode(0);
                TextSetColor(0xFF, 0xFF, 0xFF, ((sFade * 0xFF) / 0xFF) & 0xFF);
                TextSetSize(0xD);
                TextSetBox(400, 0x26C, 0x50, 0x168);
                TextDraw(StrGetIndex(title, &len), len);
                TextDrawStr(cbuf);
            }
            ExtrasModelShow(gMusicModel, sFade);
        }
    } else if (gMenuPage == 3) {
        if (fn_80007138() != 0) {
            sFade = 0;
        }
        if (selMovie == 1) {
            if ((sPageFlags & 0x10) == 0) {
                sFade = 0;
                sPageFlags = 0x10;
            } else {
                int f = sFade + 1;
                sFade = 0xFF;
                if (f < 0x100) {
                    sFade = f;
                }
            }
        } else {
            sFade = 0;
        }
        ExtrasListDraw(gMovieSel, gMovieCount, (int (*)(u32))MoviePlay, alpha, 0xD8E68AB7, 0x273D2D, gMovieBits);
        ExtrasHudDraw(sFade);
    } else if (gMenuPage == 4) {
        if (fn_80007138() != 0) {
            sFade = 0;
        }
        if (selSecret == 1) {
            if ((sPageFlags & 8) == 0) {
                sFade = 0;
                sPageFlags = 8;
            } else {
                int f = sFade + 1;
                sFade = 0xFF;
                if (f < 0x100) {
                    sFade = f;
                }
            }
        } else {
            sFade = 0;
        }
        ExtrasListDraw(gSecretSel, gSecretCount, (int (*)(u32))SecretShow, alpha, 0x5EB7A22E, 0x273D2D,
                       gSecretBits);
        if (gSecretBits == NULL) {
            gMenuInPage = 0;
        } else if (gSecretBusy != 0) {
            u32 len = 0;
            int a = Fn800DF3B8(0x5EB7A22E, gSecretSel, 0x7EC7D68D);
            int b = Fn800DF3B8(0x5EB7A22E, gSecretSel, 0x273D2D);
            if (b >= 0 && a >= 0) {
                u8 al = ((sFade * 0xFF) / 0xFF) & 0xFF;
                TextBegin();
                TextSetMode(0);
                TextSetColor(0xFF, 0xFF, 0xFF, al);
                TextSetSize(0xD);
                TextSetBox(0xF0, 0x262, 100, 0x14A);
                TextDraw(StrGetIndex(a, &len), len);
                TextBegin();
                TextSetMode(0);
                TextSetColor(0xFF, 0xFF, 0xFF, al);
                TextSetSize(0xD);
                TextSetBox(0xF0, 0x262, 100, 0x12A);
                TextDraw(StrGetIndex(b, &len), len);
            }
        }
    }
    return 1;
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
void fn_80093424(void);
void fn_800934D0(int id);
}

// Is list entry `i` of `table` hidden (no data record)?
static inline int ExtrasHidden(u32 table, int i) {
    const char* d = VarObjData(table, i);
    return d == NULL || *d == 0;
}

// Finds the first level whose gate matches the entry's game event: returns the gate index or -1.
static inline int ExtrasGateFor(u32 table, u32 key, int i) {
    int ev = GameEvent(0xBAA22E62, Fn800DF3B8(table, i, key));
    int k;
    for (k = 0; k < 13; k++) {
        if (ev == (int)gLevelGates[k].event && k < gProgressLevel + 1) {
            return k;
        }
    }
    return -1;
}

// Pages of the extras list menu (movies, music, concept art).  Each moves the highlight with
// the pad skipping locked rows, starts the preview model of the chosen entry and shows it.

// Movie list page: moves the highlight with the pad skipping locked rows, starts the preview model of the chosen entry and shows it.
void ExtrasMoviePage(float dt) {
    int old = gMovieSel;
    if (gMovieBits == NULL) {
        gMenuInPage = 0;
        return;
    }
    if (gMovieBusy == 0) {
        if (gPadLeftEdge == 1) {
            if (gMovieSel == -1) {
                return;
            }
            gMovieSel--;
            if (ExtrasHidden(0xD8E68AB7, gMovieSel)) {
                gMovieSel--;
            }
        }
        if (gPadRightEdge == 1) {
            if (gMovieSel == -1) {
                return;
            }
            gMovieSel++;
            if (gMovieSel < gMovieCount && ExtrasHidden(0xD8E68AB7, gMovieSel)) {
                int i = gMovieSel;
                int found = -1;
                for (; i >= 0; i--) {
                    if (ExtrasHidden(0xD8E68AB7, i) && ExtrasGateFor(0xD8E68AB7, 0x273D2D, i) != -1) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    gMovieSel--;
                } else {
                    gMovieSel++;
                }
            }
        }
        if (PadPressed(0, 0, 0xB) == 1 && gRepeatTimer > 10) {
            int i;
            gRepeatTimer = 0;
            for (i = gMovieSel; i < gMovieCount - 1; i++) {
                if (ExtrasHidden(0xD8E68AB7, i)) {
                    break;
                }
            }
            gMovieSel = ExtrasGateFor(0xD8E68AB7, 0x273D2D, i) == -1 ? i - 1 : i + 1;
        }
        if (PadPressed(0, 0, 10) == 1 && gRepeatTimer > 10) {
            int i;
            int seen = 0;
            gRepeatTimer = 0;
            for (i = gMovieSel; i >= 0; i--) {
                if (ExtrasHidden(0xD8E68AB7, i)) {
                    if (seen) {
                        gMovieSel = i + 1;
                        break;
                    }
                    seen = 1;
                }
            }
        }
        gRepeatTimer++;
        if (gMovieSel < 1) {
            gMovieSel = 1;
        } else if (gMovieSel > gMovieCount - 1) {
            gMovieSel = gMovieCount - 1;
        }
        if (PadPressed(0, 0, 3) == 1 && gMovieSel != -1 && BitTest(gMovieBits, gMovieSel) != 0) {
            int ev = GameEvent(0x2F9DF01F, Fn800DF3B8(0xD8E68AB7, gMovieSel, 0xD83EE6B4));
            if (ev == 0) {
                gExtrasModel = 0;
            } else {
                float spawn[12];
                float pos[3];
                spawn[0] = 0.0f;
                spawn[1] = 0.0f;
                *(int*)&spawn[2] = ev;
                pos[2] = VarGetFloat(0xD8E68AB7, gMovieSel, 0x5A);
                pos[1] = VarGetFloat(0xD8E68AB7, gMovieSel, 0x59);
                pos[0] = VarGetFloat(0xD8E68AB7, gMovieSel, 0x58);
                Vec3Set(&spawn[6], VarGetFloat(0xD8E68AB7, gMovieSel, 0x5A3309E2), VarGetFloat(0xD8E68AB7, gMovieSel, 0x489CB20), 0.0f);
                {
                    int act = fn_800C7E80(pos);
                    if (act == 0) {
                        gExtrasModel = 0;
                    } else {
                        SendMsg(*(int*)(act + 0x38), 6, spawn);
                        gExtrasModel = *(int*)(act + 0x38);
                    }
                }
            }
            if (gExtrasModel != 0) {
                gMovieBusy = 1;
                FxStart(gMenuFxSelect, 0, 0);
            }
        }
        if (PadPressed(0, 0, 4) == 1) {
            FxStart(gMenuFxBack, 0, 0);
            gMenuInPage = 0;
        }
    } else {
        if (PadPressed(0, 0, 4) == 1) {
            gMovieBusy = 0;
            FxStart(gMenuFxBack, 0, 0);
            if (gExtrasModel != 0) {
                int m = gExtrasModel;
                int msg[2];
                msg[1] = 0;
                SendMsg(gExtrasModel, 6, msg);
                fn_800C7B14(m);
                ActorRender();
            }
            gExtrasModel = 0;
        }
    }
    if (old != gMovieSel) {
        FxStart(gMenuFxMove, 0, 0);
    }
    if (gExtrasModel != 0) {
        float msg[2];
        msg[1] = dt;
        SendMsg(gExtrasModel, 0x414, msg);
    }
}

// Music list page (same layout as the movie page).
void ExtrasMusicPage(float dt) {
    int old = gMusicSel;
    if (gMusicBits == NULL) {
        gMenuInPage = 0;
        return;
    }
    if (gMusicBusy == 0) {
        if (gPadLeftEdge == 1) {
            if (gMusicSel == -1) {
                return;
            }
            gMusicSel--;
            if (ExtrasHidden(0x258124CE, gMusicSel)) {
                gMusicSel--;
            }
        }
        if (gPadRightEdge == 1) {
            if (gMusicSel == -1) {
                return;
            }
            gMusicSel++;
            if (gMusicSel < gMusicCount && ExtrasHidden(0x258124CE, gMusicSel)) {
                int i = gMusicSel;
                int found = -1;
                for (; i >= 0; i--) {
                    if (ExtrasHidden(0x258124CE, i) && ExtrasGateFor(0x258124CE, 0x273D2D, i) != -1) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    gMusicSel--;
                } else {
                    gMusicSel++;
                }
            }
        }
        if (PadPressed(0, 0, 0xB) == 1 && gRepeatTimer > 10) {
            int i;
            gRepeatTimer = 0;
            for (i = gMusicSel; i < gMusicCount - 1; i++) {
                if (ExtrasHidden(0x258124CE, i)) {
                    break;
                }
            }
            gMusicSel = ExtrasGateFor(0x258124CE, 0x273D2D, i) == -1 ? i - 1 : i + 1;
        }
        if (PadPressed(0, 0, 10) == 1 && gRepeatTimer > 10) {
            int i;
            int seen = 0;
            gRepeatTimer = 0;
            for (i = gMusicSel; i >= 0; i--) {
                if (ExtrasHidden(0x258124CE, i)) {
                    if (seen) {
                        gMusicSel = i + 1;
                        break;
                    }
                    seen = 1;
                }
            }
        }
        gRepeatTimer++;
        if (gMusicSel < 1) {
            gMusicSel = 1;
        } else if (gMusicSel > gMusicCount - 1) {
            gMusicSel = gMusicCount - 1;
        }
        if (PadPressed(0, 0, 3) == 1 && gMusicSel != -1 && BitTest(gMusicBits, gMusicSel) != 0) {
            int ev = GameEvent(0x2F9DF01F, Fn800DF3B8(0x258124CE, gMusicSel, 0xD83EE6B4));
            if (ev == 0) {
                gMusicModel = 0;
            } else {
                float spawn[12];
                float pos[3];
                spawn[0] = 0.0f;
                spawn[1] = 0.0f;
                *(int*)&spawn[2] = ev;
                pos[2] = VarGetFloat(0x258124CE, gMusicSel, 0x5A);
                pos[1] = VarGetFloat(0x258124CE, gMusicSel, 0x59);
                pos[0] = VarGetFloat(0x258124CE, gMusicSel, 0x58);
                Vec3Set(&spawn[6], VarGetFloat(0x258124CE, gMusicSel, 0x5A3309E2), VarGetFloat(0x258124CE, gMusicSel, 0x489CB20), 0.0f);
                {
                    int act = fn_800C7E80(pos);
                    if (act == 0) {
                        gMusicModel = 0;
                    } else {
                        SendMsg(*(int*)(act + 0x38), 6, spawn);
                        gMusicModel = *(int*)(act + 0x38);
                    }
                }
            }
            if (gMusicModel != 0) {
                gMusicBusy = 1;
                FxStart(gMenuFxSelect, 0, 0);
            }
        }
        if (PadPressed(0, 0, 4) == 1) {
            FxStart(gMenuFxBack, 0, 0);
            gMenuInPage = 0;
        }
    } else {
        if (PadPressed(0, 0, 4) == 1) {
            gMusicBusy = 0;
            FxStart(gMenuFxBack, 0, 0);
            if (gMusicModel != 0) {
                int m = gMusicModel;
                int msg[2];
                msg[1] = 0;
                SendMsg(gMusicModel, 6, msg);
                fn_800C7B14(m);
                ActorRender();
            }
            gMusicModel = 0;
        }
    }
    if (old != gMusicSel) {
        FxStart(gMenuFxMove, 0, 0);
    }
    if (gMusicModel != 0) {
        float msg[2];
        msg[1] = dt;
        SendMsg(gMusicModel, 0x414, msg);
    }
}

// Concept art list page (same layout as the movie page).
void ExtrasArtPage(float dt) {
    int old = gArtSel;
    if (gArtBitsB == NULL) {
        gMenuInPage = 0;
        return;
    }
    if (gArtBusy == 0) {
        if (gPadLeftEdge == 1) {
            if (gArtSel == -1) {
                return;
            }
            gArtSel--;
            if (ExtrasHidden(0x80EDD05C, gArtSel)) {
                gArtSel--;
            }
        }
        if (gPadRightEdge == 1) {
            if (gArtSel == -1) {
                return;
            }
            gArtSel++;
            if (gArtSel < gArtCountB && ExtrasHidden(0x80EDD05C, gArtSel)) {
                int i = gArtSel;
                int found = -1;
                for (; i >= 0; i--) {
                    if (ExtrasHidden(0x80EDD05C, i) && ExtrasGateFor(0x80EDD05C, 0x198917DC, i) != -1) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    gArtSel--;
                } else {
                    gArtSel++;
                }
            }
        }
        if (PadPressed(0, 0, 0xB) == 1 && gRepeatTimer > 10) {
            int i;
            gRepeatTimer = 0;
            for (i = gArtSel; i < gArtCountB - 1; i++) {
                if (ExtrasHidden(0x80EDD05C, i)) {
                    break;
                }
            }
            gArtSel = ExtrasGateFor(0x80EDD05C, 0x198917DC, i) == -1 ? i - 1 : i + 1;
        }
        if (PadPressed(0, 0, 10) == 1 && gRepeatTimer > 10) {
            int i;
            int seen = 0;
            gRepeatTimer = 0;
            for (i = gArtSel; i >= 0; i--) {
                if (ExtrasHidden(0x80EDD05C, i)) {
                    if (seen) {
                        gArtSel = i + 1;
                        break;
                    }
                    seen = 1;
                }
            }
        }
        gRepeatTimer++;
        if (gArtSel < 1) {
            gArtSel = 1;
        } else if (gArtSel > gArtCountB - 1) {
            gArtSel = gArtCountB - 1;
        }
        if (PadPressed(0, 0, 3) == 1 && gArtSel != -1 && BitTest(gArtBitsB, gArtSel) != 0) {
            int ev = GameEvent(0x2F9DF01F, Fn800DF3B8(0x80EDD05C, gArtSel, 0xD83EE6B4));
            if (ev == 0) {
                gArtModel = 0;
            } else {
                float spawn[12];
                float pos[3];
                spawn[0] = 0.0f;
                spawn[1] = 0.0f;
                *(int*)&spawn[2] = ev;
                pos[2] = VarGetFloat(0x80EDD05C, gArtSel, 0x5A);
                pos[1] = VarGetFloat(0x80EDD05C, gArtSel, 0x59);
                pos[0] = VarGetFloat(0x80EDD05C, gArtSel, 0x58);
                Vec3Set(&spawn[6], VarGetFloat(0x80EDD05C, gArtSel, 0x5A3309E2), VarGetFloat(0x80EDD05C, gArtSel, 0x489CB20), 0.0f);
                {
                    int act = fn_800C7E80(pos);
                    if (act == 0) {
                        gArtModel = 0;
                    } else {
                        SendMsg(*(int*)(act + 0x38), 6, spawn);
                        gArtModel = *(int*)(act + 0x38);
                    }
                }
            }
            if (gArtModel != 0) {
                gArtBusy = 1;
                FxStart(gMenuFxSelect, 0, 0);
            }
        }
        if (PadPressed(0, 0, 4) == 1) {
            FxStart(gMenuFxBack, 0, 0);
            gMenuInPage = 0;
        }
    } else {
        if (PadPressed(0, 0, 4) == 1) {
            gArtBusy = 0;
            FxStart(gMenuFxBack, 0, 0);
            if (gArtModel != 0) {
                int m = gArtModel;
                int msg[2];
                msg[1] = 0;
                SendMsg(gArtModel, 6, msg);
                fn_800C7B14(m);
                ActorRender();
            }
            gArtModel = 0;
        }
    }
    if (old != gArtSel) {
        FxStart(gMenuFxMove, 0, 0);
    }
    if (gArtModel != 0) {
        float msg[2];
        msg[1] = dt;
        SendMsg(gArtModel, 0x414, msg);
    }
}

// Bonus (weapon) list page: like the movie page, but the preview is a weapon firing its muzzle flash.
void ExtrasBonusPage(float dt) {
    int old = gBonusSel;
    if (gBonusBits == NULL) {
        gMenuInPage = 0;
        return;
    }
    if (gBonusBusy == 0) {
        if (gPadLeftEdge == 1) {
            if (gBonusSel == -1) {
                return;
            }
            gBonusSel--;
            if (ExtrasHidden(0x5E130113, gBonusSel)) {
                gBonusSel--;
            }
        }
        if (gPadRightEdge == 1) {
            if (gBonusSel == -1) {
                return;
            }
            gBonusSel++;
            if (gBonusSel < gBonusCount && ExtrasHidden(0x5E130113, gBonusSel)) {
                int i = gBonusSel;
                int found = -1;
                for (; i >= 0; i--) {
                    if (ExtrasHidden(0x5E130113, i) && ExtrasGateFor(0x5E130113, 0x198917DC, i) != -1) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    gBonusSel--;
                } else {
                    gBonusSel++;
                }
            }
        }
        if (PadPressed(0, 0, 0xB) == 1 && gRepeatTimer > 10) {
            int i;
            gRepeatTimer = 0;
            for (i = gBonusSel; i < gBonusCount - 1; i++) {
                if (ExtrasHidden(0x5E130113, i)) {
                    break;
                }
            }
            gBonusSel = ExtrasGateFor(0x5E130113, 0x198917DC, i) == -1 ? i - 1 : i + 1;
        }
        if (PadPressed(0, 0, 10) == 1 && gRepeatTimer > 10) {
            int i;
            int seen = 0;
            gRepeatTimer = 0;
            for (i = gBonusSel; i >= 0; i--) {
                if (ExtrasHidden(0x5E130113, i)) {
                    if (seen) {
                        gBonusSel = i + 1;
                        break;
                    }
                    seen = 1;
                }
            }
        }
        gRepeatTimer++;
        if (gBonusSel < 1) {
            gBonusSel = 1;
        } else if (gBonusSel > gBonusCount - 1) {
            gBonusSel = gBonusCount - 1;
        }
        if (PadPressed(0, 0, 3) == 1 && gBonusSel != -1 && BitTest(gBonusBits, gBonusSel) != 0) {
            int sel = gBonusSel;
            float spawn[12];
            float pos[3];
            int act;
            int fx[8];
            int gun[2];
            spawn[0] = 0.0f;
            *(int*)&spawn[1] = Fn800DF3B8(0x5E130113, sel, 0x2D143A0B);
            spawn[2] = 0.0f;
            *(int*)&spawn[4] = Fn800DF3B8(0x5E130113, sel, 0x6F9E1166);
            pos[2] = VarGetFloat(0x5E130113, sel, 0x5A);
            pos[1] = VarGetFloat(0x5E130113, sel, 0x59);
            pos[0] = VarGetFloat(0x5E130113, sel, 0x58);
            Vec3Set(&spawn[8], VarGetFloat(0x5E130113, sel, 0x5A3309E2), VarGetFloat(0x5E130113, sel, 0x489CB20), 0.0f);
            act = fn_800C7E80(pos);
            if (act != 0) {
                SendMsg(*(int*)(act + 0x38), 6, gun);
            }
            fn_800934D0(*(int*)&spawn[4]);
            memset(fx, 0, 0x20);
            SendMsg(*(int*)(act + 0x38), 0xC04, fx);
            gun[0] = AssetFind(0xD9044CBB, StrHash(0, "MUZZLEFLASH"));
            SendMsg(*(int*)(act + 0x38), 0xC02, gun);
            gBonusModel = *(int*)(act + 0x38);
            if (gBonusModel != 0) {
                FxStart(gMenuFxSelect, 0, 0);
                gBonusBusy = 1;
            }
        }
        if (PadPressed(0, 0, 4) == 1) {
            FxStart(gMenuFxBack, 0, 0);
            gMenuInPage = 0;
        }
    } else {
        if (PadPressed(0, 0, 4) == 1) {
            gBonusBusy = 0;
            FxStart(gMenuFxBack, 0, 0);
            if (gBonusModel != 0) {
                int m = gBonusModel;
                int msg[2];
                msg[1] = 0;
                SendMsg(gBonusModel, 6, msg);
                Fn800DF3B8(0x5E130113, gBonusSel, 0x6F9E1166);
                fn_80093424();
                fn_800C7B14(m);
                ActorRender();
            }
            gBonusModel = 0;
        }
    }
    if (old != gBonusSel) {
        FxStart(gMenuFxMove, 0, 0);
    }
    if (gBonusModel != 0) {
        float msg[2];
        msg[1] = dt;
        SendMsg(gBonusModel, 0x414, msg);
    }
}

// Secrets page of the extras menu (same navigation as the list pages, no preview model).
void ExtrasSecretPage(void) {
    int old = gSecretSel;
    if (gSecretBits == NULL) {
        gMenuInPage = 0;
        return;
    }
    if (gSecretBusy != 0) {
        if (PadPressed(0, 0, 4) == 1) {
            FxStart(gMenuFxBack, 0, 0);
            gSecretBusy = 0;
        }
    } else {
        if (gPadLeftEdge == 1) {
            if (gSecretSel == -1) {
                return;
            }
            gSecretSel--;
            if (ExtrasHidden(0x5EB7A22E, gSecretSel)) {
                gSecretSel--;
            }
        }
        if (gPadRightEdge == 1) {
            if (gSecretSel == -1) {
                return;
            }
            gSecretSel++;
            if (gSecretSel < gSecretCount && ExtrasHidden(0x5EB7A22E, gSecretSel)) {
                int i = gSecretSel;
                int found = -1;
                for (; i >= 0; i--) {
                    if (ExtrasHidden(0x5EB7A22E, i) && ExtrasGateFor(0x5EB7A22E, 0x273D2D, i) != -1) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    gSecretSel--;
                } else {
                    gSecretSel++;
                }
            }
        }
        if (PadPressed(0, 0, 0xB) == 1 && gRepeatTimer > 10) {
            int i;
            gRepeatTimer = 0;
            for (i = gSecretSel; i < gSecretCount - 1; i++) {
                if (ExtrasHidden(0x5EB7A22E, i)) {
                    break;
                }
            }
            gSecretSel = ExtrasGateFor(0x5EB7A22E, 0x273D2D, i) == -1 ? i - 1 : i + 1;
        }
        if (PadPressed(0, 0, 10) == 1 && gRepeatTimer > 10) {
            int i;
            int seen = 0;
            gRepeatTimer = 0;
            for (i = gSecretSel; i >= 0; i--) {
                if (ExtrasHidden(0x5EB7A22E, i)) {
                    if (seen) {
                        gSecretSel = i + 1;
                        break;
                    }
                    seen = 1;
                }
            }
        }
        gRepeatTimer++;
        if (gSecretSel < 1) {
            gSecretSel = 1;
        } else if (gSecretSel > gSecretCount - 1) {
            gSecretSel = gSecretCount - 1;
        }
        if (PadPressed(0, 0, 3) == 1 && gSecretSel != -1 && BitTest(gSecretBits, gSecretSel) != 0) {
            FxStart(gMenuFxSelect, 0, 0);
            gSecretBusy = 1;
        }
        if (PadPressed(0, 0, 4) == 1) {
            FxStart(gMenuFxBack, 0, 0);
            gMenuInPage = 0;
        }
    }
    if (old != gSecretSel) {
        FxStart(gMenuFxMove, 0, 0);
    }
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
            if (ExtrasGateFor(table, nameKey, k) != -1) {
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

extern "C" {
int VarExists(u32 table, u32 a, u32 b);                             // 0x800DF080
int VarGroupObjCount(u32 table);                                    // 0x800DED98
void SaveGetOptions(void* p, int n);                                // 0x800DEC54
void* VAlloc(int size);
void* memset(void* p, int c, u32 n);
float fn_800D8BE4(int a, int b, int axis, int c);
int FxFind(const char* name);                                       // 0x80170834
}
extern int gArrowTexUp;         // 0x804A3280
extern int gArrowTexDown;       // 0x804A3284
extern int gBonusUnlockCount;   // 0x804D5560
extern u32* gBonusBitsB;        // 0x804D5564
extern int gBonusCountB;        // 0x804D5568
extern int gArtHidden;          // 0x804D5588
extern int gMusicVisible;       // 0x804D55A8 (same word as gMusicTotal)
extern int gMovieVisible;       // 0x804D55C0
extern int gSecretVisible;      // 0x804D55D8
extern int gCheatCount;         // 0x804D55E8

// Builds the unlock bitsets of the extras menu (bonus, art, music, movies, secrets, cheats)
// from the variable tables and looks up the menu sounds and arrow textures.
void fn_802846F0(void) {
    int bytes;
    int i;
    gMenuFxMove = FxFind("Navigate");
    gMenuFxSelect = FxFind("Select");
    gMenuFxBack = FxFind("Back");
    gArrowTexR = AssetFind(0xF2E2BEB8, 0x3FD6348D);
    gArrowTexL = AssetFind(0xF2E2BEB8, 0x1651D499);
    gArrowTexUp = AssetFind(0xF2E2BEB8, 0xD141D32E);
    gArrowTexDown = AssetFind(0xF2E2BEB8, 0x66979E2C);
    if (gBonusBits == NULL && VarExists(0x5E130113, 0, 0) != 0) {
        gBonusCount = VarGroupObjCount(0x5E130113);
        bytes = ((gBonusCount + 0x1F) / 32) * 4;
        gBonusCountB = gBonusCount;
        gBonusCount2 = gBonusCount;
        gBonusBits = (u32*)VAlloc(bytes);
        gBonusBitsB = (u32*)VAlloc(bytes);
        gBonusBits2 = (u32*)VAlloc(bytes);
        for (i = 0; i < gBonusCount; i++) {
            const char* d = VarObjData(0x5E130113, i);
            if (d != NULL && *d != 0) {
                gBonusUnlockCount++;
            }
        }
        if (gBonusBits != NULL) {
            memset(gBonusBits, 0, bytes);
        }
        if (gBonusBitsB != NULL) {
            memset(gBonusBitsB, 0, bytes);
        }
        if (gBonusBits2 != NULL) {
            memset(gBonusBits2, 0, bytes);
        }
    }
    if (gArtBitsB == NULL && VarExists(0x80EDD05C, 0, 0) != 0) {
        int skipped = 0;
        gArtCountB = VarGroupObjCount(0x80EDD05C);
        bytes = ((gArtCountB + 0x1F) / 32) * 4;
        gArtCountA = gArtCountB;
        gArtBitsB = (u32*)VAlloc(bytes);
        gArtBitsA = (u32*)VAlloc(bytes);
        for (i = 0; i < gArtCountB; i++) {
            const char* d = VarObjData(0x80EDD05C, i);
            if (d == NULL || *d == 0) {
                int ev = GameEvent(0xBAA22E62, Fn800DF3B8(0x80EDD05C, i, 0x198917DC));
                if (ev == -0x404C527A) {
                    skipped++;
                }
            } else if (skipped < 1) {
                gArtHidden++;
            } else {
                skipped++;
            }
        }
        gArtCountB = gArtCountB - skipped;
        if (gArtBitsB != NULL) {
            memset(gArtBitsB, 0, bytes);
        }
        if (gArtBitsA != NULL) {
            memset(gArtBitsA, 0, bytes);
        }
    }
    if (gMusicBits == NULL && VarExists(0x258124CE, 0, 0) != 0) {
        gMusicCount = VarGroupObjCount(0x258124CE);
        bytes = ((gMusicCount + 0x1F) / 32) * 4;
        gMusicBits = (u32*)VAlloc(bytes);
        for (i = 0; i < gMusicCount; i++) {
            const char* d = VarObjData(0x258124CE, i);
            if (d != NULL && *d != 0) {
                gMusicVisible++;
            }
        }
        if (gMusicBits != NULL) {
            memset(gMusicBits, 0, bytes);
        }
    }
    if (gMovieBits == NULL && VarExists(0xD8E68AB7, 0, 0) != 0) {
        gMovieCount = VarGroupObjCount(0xD8E68AB7);
        bytes = ((gMovieCount + 0x1F) / 32) * 4;
        gMovieBits = (u32*)VAlloc(bytes);
        for (i = 0; i < gMovieCount; i++) {
            const char* d = VarObjData(0xD8E68AB7, i);
            if (d != NULL && *d != 0) {
                gMovieVisible++;
            }
        }
        if (gMovieBits != NULL) {
            memset(gMovieBits, 0, bytes);
        }
        SaveGetOptions(gMovieBits, bytes);
    }
    if (gSecretBits == NULL && VarExists(0x5EB7A22E, 0, 0) != 0) {
        gSecretCount = VarGroupObjCount(0x5EB7A22E);
        bytes = ((gSecretCount + 0x1F) / 32) * 4;
        gSecretBits = (u32*)VAlloc(bytes);
        for (i = 0; i < gSecretCount; i++) {
            const char* d = VarObjData(0x5EB7A22E, i);
            if (d != NULL && *d != 0) {
                gSecretVisible++;
            }
        }
        if (gSecretBits != NULL) {
            memset(gSecretBits, 0, bytes);
        }
    }
    if (gCheatBits == NULL && VarExists(0x872E838C, 0, 0) != 0) {
        gCheatCount = VarGroupObjCount(0x872E838C);
        bytes = ((gCheatCount + 0x1F) / 32) * 4;
        gCheatBits = (u32*)VAlloc(bytes);
        if (gCheatBits != NULL) {
            memset(gCheatBits, 0, bytes);
        }
    }
}

// Registers the extras message handler (type 99).
// ExtrasMsgRegister
void fn_80284CB0(void) {
    fn_80026660(99, (void*)fn_8027E954, 0);
}

extern int gPadStickTimer;      // 0x804D5614 (function static)
extern int gPadDownEdge;        // 0x803C0CB0
extern int gPadUpEdge;          // 0x803C0CB4

// Turns the stick axes into edge-triggered up/down/left/right presses (0 idle, 1 pressed,
// 2 held) with a repeat delay.
void ExtrasPadUpdate(void) {
    static int sRepeat = 0;             // 0x804D5614
    float v;
    if (gPadLeftEdge == 0) {
        v = fn_800D8BE4(0, 0, 1, 0);
        if (v < -0.65f) {
            sRepeat = 0;
            gPadLeftEdge = 1;
        }
    } else if (gPadLeftEdge == 1) {
        v = fn_800D8BE4(0, 0, 1, 0);
        if (v <= -0.55f) {
            gPadLeftEdge = 2;
        } else {
            gPadLeftEdge = 0;
        }
    } else if (sRepeat > 10) {
        gPadLeftEdge = 0;
    }
    if (gPadRightEdge == 0) {
        v = fn_800D8BE4(0, 0, 1, 0);
        if (0.65f < v) {
            gPadRightEdge = 1;
            sRepeat = 0;
        }
    } else if (gPadRightEdge == 1) {
        v = fn_800D8BE4(0, 0, 1, 0);
        if (0.55f <= v) {
            gPadRightEdge = 2;
        } else {
            gPadRightEdge = 0;
        }
    } else if (sRepeat > 10) {
        gPadRightEdge = 0;
    }
    if (gPadUpEdge == 0) {
        v = fn_800D8BE4(0, 0, 2, 0);
        if (v < -0.65f) {
            gPadUpEdge = 1;
        }
    } else if (gPadUpEdge == 1) {
        v = fn_800D8BE4(0, 0, 2, 0);
        if (v <= -0.55f) {
            gPadUpEdge = 2;
        } else {
            gPadUpEdge = 0;
        }
    } else {
        v = fn_800D8BE4(0, 0, 2, 0);
        if (-0.55f < v) {
            gPadUpEdge = 0;
        }
    }
    if (gPadDownEdge == 0) {
        v = fn_800D8BE4(0, 0, 2, 0);
        if (0.65f < v) {
            gPadDownEdge = 1;
        }
    } else if (gPadDownEdge == 1) {
        v = fn_800D8BE4(0, 0, 2, 0);
        if (0.55f <= v) {
            gPadDownEdge = 2;
        } else {
            gPadDownEdge = 0;
        }
    } else {
        v = fn_800D8BE4(0, 0, 2, 0);
        if (v < 0.55f) {
            gPadDownEdge = 0;
        }
    }
    sRepeat++;
}

extern "C" int FxFind(const char* name);        // 0x80170834

// Looks up the menu sounds.
void ExtrasMenuFxInit(void) {
    gMenuFxMove = FxFind("Navigate");
    gMenuFxSelect = FxFind("Select");
    gMenuFxBack = FxFind("Back");
}

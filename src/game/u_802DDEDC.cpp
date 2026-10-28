// u_802DDEDC.cpp: memory card UI and card state machines (0x802DDEDC-0x802E2F08).
// MemCard.c: the "memory card needs to be formatted / saving / loading" message box, and the
// CARDGuide1 check-file, format (Fig2) and save-file state machines that talk to the SDK card
// functions and report progress with OSReport.  The later functions (0x802E0B1C on) are the
// save/load menu pages built on top of them.  Names are descriptive guesses; unmatched against
// retail so far.

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

struct Color4 {
    u8 r, g, b, a;
};

// SDK card types (only the parts used here).
struct CARDFileInfo {
    int chan;       // 0x00
    int fileNo;     // 0x04
    int offset;     // 0x08
    int length;     // 0x0C
    u16 iBlock;     // 0x10
};

struct CARDStat {
    char fileName[32];      // 0x00
    u32 length;             // 0x20
    u32 time;               // 0x24
    u8 gameName[4];         // 0x28
    u8 company[2];          // 0x2C
    u8 bannerFormat;        // 0x2E
    u8 pad2F;
    u32 iconAddr;           // 0x30
    u16 iconFormat;         // 0x34
    u16 iconSpeed;          // 0x36
    u32 commentAddr;        // 0x38
    u32 offsetBanner;       // 0x3C
    u32 offsetBannerTlut;   // 0x40
    u32 offsetIcon[8];      // 0x44
    u32 offsetIconTlut;     // 0x64
    u32 offsetData;         // 0x68
};

extern "C" {
int sprintf(char* s, const char* fmt, ...);
char* strcpy(char* dst, const char* src);
char* strstr(const char* s, const char* sub);
void* memset(void* dst, int c, u32 n);
void OSReport(const char* msg, ...);
void OSPanic(const char* file, int line, const char* msg, ...);
u16 OSGetFontEncode(void);
void DCFlushRange(void* addr, u32 nBytes);
void DCInvalidateRange(void* addr, u32 nBytes);
char* strncpy(char* dst, const char* src, u32 n);
void* memcpy(void* dst, const void* src, u32 n);
int CARDRead(CARDFileInfo* info, void* buf, int length, int offset);
int CARDProbe(int chan);
int CARDProbeEx(int chan, int* memSize, int* sectorSize);
int CARDMount(int chan, void* workArea, void* detachCallback);
int CARDUnmount(int chan);
int CARDCheck(int chan);
int CARDFormat(int chan);
int CARDUnformat(int chan);
int CARDOpen(int chan, const char* name, CARDFileInfo* info);
int CARDClose(CARDFileInfo* info);
int CARDCreate(int chan, const char* name, u32 size, CARDFileInfo* info);
int CARDWrite(CARDFileInfo* info, void* buf, int length, int offset);
int CARDFastDelete(int chan, int fileNo);
int CARDRename(int chan, const char* oldName, const char* newName);
int CARDGetStatus(int chan, int fileNo, CARDStat* stat);
int CARDSetStatus(int chan, int fileNo, CARDStat* stat);
int CARDFreeBlocks(int chan, int* bytesNotUsed, int* filesNotUsed);
}

float LoadingTime(void);
extern "C" {
int fn_800059D0(int button);
void fn_800EBED8(int style, int height);
void fn_800EC0C4(int style, int height);
void fn_800EC214(int style, int oldHeight, int newHeight);
void fn_800ECB18(void);
void fn_800EBBB8(float scale, int x, int y);
void fn_801EA880(const char* text, Color4* color, int x, int y, int font, int a, int b);
void fn_801EAEAC(void);
}

// One message of the card message table: text, a choice count and up to five
// (label, result) choices.  The table ends with an id of -0x80.
struct CardChoice {
    const char* label;      // 0x00
    int result;             // 0x04
};

struct CardMsg {
    int id;                 // 0x00
    const char* text;       // 0x04
    int count;              // 0x08: <0 timed message, else number of choices
    int defaultSel;         // 0x0C
    CardChoice choice[5];   // 0x10
};

extern CardMsg gCardMsgs[2][25];    // 0x803C57F0 (one table per font encoding)
extern char gCardVerb[0x20];        // 0x804A4A88
extern u32 gCardCrcTable[256];      // 0x804A4AA8
extern Color4 gCardTextColor;       // 0x804D3D90
extern int gCardTextFont;           // 0x804D3D94
extern int gCardTextX;              // 0x804D3D98
extern int gCardLineHeight;         // 0x804D3D9C
extern int gCardChoiceStep;         // 0x804D3DA0
extern int gCardBoxStyle;           // 0x804D3DA4
extern int gCardBoxMargin;          // 0x804D3DA8
extern int gCardCursorX;            // 0x804D3DAC
extern int gCardCursorY;            // 0x804D3DB0
extern int sCardBoxHeight;          // 0x804D5854
extern int sCardChoiceHeight;       // 0x804D5858
extern int sCardBoxMade;            // 0x804D585C
extern int sCardIsSaving;           // 0x804D5860
extern int sCardSel;                // 0x804D5864
extern int sCardOverwriteOk;        // 0x804D5828
extern float gCardInputDelay;       // 0x804DB29C

static inline CardMsg* CardMsgFind(int id) {
    CardMsg* tbl = gCardMsgs[OSGetFontEncode() & 0xFFFF];
    int i;
    for (i = 0; tbl[i].id != -0x80; i++) {
        if (id == tbl[i].id) {
            return &tbl[i];
        }
    }
    return NULL;
}

static inline int CardCountLines(const char* s) {
    int n = 0;
    for (; *s != 0; s++) {
        if (*s == '\n') {
            n++;
        }
    }
    return n;
}

// Shows message `msgId`; returns the chosen result (or 0 while undecided, 1 when a timed
// message has run out).
// CardMsgShow (called by other units under its placeholder name)
extern "C" int fn_802DDEDC(float dt, int msgId, int flag, int mode) {
    static int sShownId = 0;            // 0x804D5868
    static float sTimer = 0.0f;         // 0x804D5870
    static float sDelay = 0.0f;         // 0x804D5878
    bool fresh = false;
    u32 accept = 0;
    CardMsg* msg;
    int i;
    int count;
    char line[260];
    char fmt[256];

    if (sShownId != msgId) {
        if (sTimer > 0.0f) {
            while (sTimer > 0.0f) {
                msg = CardMsgFind(sShownId);
                if (msg == NULL) {
                    strcpy(line, "NO MESSAGE");
                } else {
                    if ((mode == 2 || mode == 1) && sCardIsSaving == 0) {
                        sprintf(gCardVerb, OSGetFontEncode() != 0 ? "\x83\x8d\x81[\x83h" : "loading");
                    } else {
                        sprintf(gCardVerb, OSGetFontEncode() != 0 ? "\x83Z\x81[\x83u" : "saving");
                    }
                    sprintf(fmt, "%s\n\n", msg->text);
                    sprintf(line, fmt, flag == 0 ? "A" : "B");
                }
                {
                    int y = (0x1E0 - (0x1E0 - sCardBoxHeight) / 2) - gCardBoxMargin;
                    Color4 color;
                    sTimer -= LoadingTime();
                    fn_800ECB18();
                    fn_800EC0C4(gCardBoxStyle, sCardBoxHeight);
                    color = gCardTextColor;
                    fn_801EA880(line, &color, gCardTextX, y, gCardTextFont, 0x10, 0);
                    fn_801EAEAC();
                }
            }
        }
        fresh = true;
        sShownId = msgId;
    }
    msg = CardMsgFind(msgId);
    if (msg == NULL) {
        count = 0;
        sCardSel = 0;
    } else {
        if (fresh) {
            sCardSel = msg->defaultSel;
            if ((mode == 2 || mode == 1) && sCardIsSaving == 0) {
                sprintf(gCardVerb, OSGetFontEncode() != 0 ? "\x83\x8d\x81[\x83h" : "LOADING");
            } else {
                sprintf(gCardVerb, OSGetFontEncode() != 0 ? "\x83Z\x81[\x83u" : "SAVING");
            }
        }
        sprintf(fmt, "%s\n\n", msg->text);
        sprintf(line, fmt, flag == 0 ? "A" : "B");
        count = msg->count;
        if (fresh) {
            const char* text = msg->text;
            if (sCardBoxMade == 0) {
                sCardBoxHeight = gCardBoxMargin * 2 + gCardLineHeight * (CardCountLines(line) + 2);
                sCardChoiceHeight = gCardLineHeight * (CardCountLines(text) + 2);
                fn_800EBED8(gCardBoxStyle, sCardBoxHeight);
                sCardBoxMade = 1;
            } else {
                int h = gCardBoxMargin * 2 + gCardLineHeight * (CardCountLines(line) + 2);
                sCardChoiceHeight = gCardLineHeight * (CardCountLines(text) + 2);
                if (h != sCardBoxHeight) {
                    fn_800EC214(gCardBoxStyle, sCardBoxHeight, h);
                    sCardBoxHeight = h;
                }
            }
            sDelay = gCardInputDelay;
            if (count < 0) {
                sTimer = (float)(-count);
            }
        }
        fn_800ECB18();
        fn_800EC0C4(gCardBoxStyle, sCardBoxHeight);
        {
            Color4 color = gCardTextColor;
            int y = (0x1E0 - (0x1E0 - sCardBoxHeight) / 2) - gCardBoxMargin;
            fn_801EA880(line, &color, gCardTextX, y, gCardTextFont, 0x10, 0);
            if (count > 0) {
                int font = gCardTextFont;
                for (i = 0; i < msg->count; i++) {
                    if (msg->choice[i].result == 5) {
                        font = 9;
                    }
                }
                y -= sCardChoiceHeight;
                for (i = 0; i < msg->count; i++) {
                    const char* label;
                    if (strstr(msg->choice[i].label, "%s") == 0) {
                        label = msg->choice[i].label;
                    } else {
                        sprintf(fmt, msg->choice[i].label, gCardVerb);
                        label = fmt;
                    }
                    color = gCardTextColor;
                    fn_801EA880(label, &color, gCardTextX + 0x1E, y, font, 0x10, 0);
                    if (i == sCardSel) {
                        fn_800EBBB8(dt, gCardCursorX + 100, y + gCardCursorY);
                    }
                    y -= gCardChoiceStep;
                }
            }
        }
        fn_801EAEAC();
    }
    if (count < 0) {
        sTimer -= dt;
        if (sTimer <= 0.0f) {
            return 1;
        }
    } else if (sDelay <= 0.0f) {
        if (fn_800059D0(0xD) == 1) {
            sCardSel--;
        }
        if (fn_800059D0(0xE) == 1) {
            sCardSel++;
        }
        if (sCardSel < 0) {
            sCardSel = count - 1;
        }
        sCardSel = sCardSel % count;
        accept = (fn_800059D0(0x14) == 1);
    } else {
        sDelay -= dt;
        accept = 0;
    }
    if (accept == 0 || msg == NULL) {
        return 0;
    }
    return msg->choice[sCardSel].result;
}

// Looks up message `msgId`: returns its choice count (1 for timed messages, with *out set to 1)
// or 0 when missing; a single-choice message stores its result in *out.
// CardMsgQuery (called by other units under its placeholder name)
extern "C" int fn_802DE64C(int msgId, int* out) {
    CardMsg* msg = CardMsgFind(msgId);
    int count;
    if (msg == NULL) {
        return 0;
    }
    count = msg->count;
    if (count < 0) {
        if (out != NULL) {
            *out = 1;
        }
        return 1;
    }
    if (count == 1 && out != NULL) {
        *out = msg->choice[msg->defaultSel].result;
    }
    return count;
}

// Debug trace of the CARDGuide1 check-file state.
void CardCheckPrintState(int state) {
    static int sPrev = (int)0xDEADF00D;
    if (state != sPrev) {
        sPrev = state;
        switch (state) {
        case 0xD0: OSReport("CARD_GUIDE1_CHECK_STAT_START\n"); break;
        case 0xD1: OSReport("CARD_GUIDE1_CHECK_STAT_OPEN_FILE\n"); break;
        case 0xD2: OSReport("CARD_GUIDE1_CHECK_STAT_OPEN_TEMP\n"); break;
        case 0xD3: OSReport("CARD_GUIDE1_CHECK_STAT_CHECK_TEMP\n"); break;
        case 0xD4: OSReport("CARD_GUIDE1_CHECK_STAT_DELETE_FILE\n"); break;
        case 0xD5: OSReport("CARD_GUIDE1_CHECK_STAT_RESTORE_TEMP\n"); break;
        case 0xD6: OSReport("CARD_GUIDE1_CHECK_STAT_DELETE_TEMP\n"); break;
        case 0xD7: OSReport("CARD_GUIDE1_CHECK_STAT_SPACE\n"); break;
        case 4: OSReport("CARD_GUIDE1_RTRN_REBOOT\n"); break;
        case 2: OSReport("CARD_GUIDE1_RTRN_CONTINUE\n"); break;
        case 1: OSReport("CARD_GUIDE1_RTRN_DONE\n"); break;
        case 0: OSReport("CARD_GUIDE1_RTRN_ERROR\n"); break;
        case -0x1A: OSReport("CARD_GUIDE1_CHECK_MESG_DELETE\n"); break;
        case -0x1B: OSReport("CARD_GUIDE1_CHECK_MESG_RESTORE\n"); break;
        case -0x1D: OSReport("CARD_GUIDE1_CHECK_MESG_SPACE_INIT\n"); break;
        case -0x1C: OSReport("CARD_GUIDE1_CHECK_MESG_SPACE\n"); break;
        default: OSPanic("MemCard.c", 0xE56, "DebugPrint case not supported"); break;
        }
    }
}

// Save description handed to the card state machines.
struct CardSaveDesc {
    char name[0x20];        // 0x00
    int size;               // 0x20
    u32* data;              // 0x24
    u8 pad28[0x36 - 0x28];
    u8 bannerFormat;        // 0x36
    u8 pad37;
    u32 iconAddr;           // 0x38
    u16 iconFormat;         // 0x3C
    u8 pad3E[0x40 - 0x3E];
    u32 commentAddr;        // 0x40
    int iconCount;          // 0x44
    u16 iconSpeed[8];       // 0x48
    u16 iconAnim;           // 0x58
    u8 saved;               // 0x5A
};

// State block of the check-file state machine (accessed as an int array in retail).
struct CardCheck {
    int chan;               // 0x00
    CardSaveDesc* desc;     // 0x04
    int mode;               // 0x08
    int state;              // 0x0C
    u8 pad10[0x2C - 0x10];
    char tmpName[0x20];     // 0x2C
    int tmpFileNo;          // 0x4C
    char name[0x20];        // 0x50
    int fileNo;             // 0x70
    int flagA;              // 0x74
    int flagB;              // 0x78
};

int CardGuide1CheckFile(CardCheck* c, int flagB) {
    int r;
    int bytesFree;
    int filesFree;
    CARDFileInfo info;
    CARDStat stat;
    int need;

    c->flagB = flagB;
    CardCheckPrintState(c->state);
    switch (c->state) {
    case 4:
        c->state = 0xD0;
        return 4;
    case -0x1A:
        if (c->mode == 1) {
            c->state = 0xD6;
        } else {
            c->state = -0x1A;
        }
        return -0x1A;
    case -0x1C:
        if (c->mode == 3) {
            c->state = 2;
        } else if (c->mode == 5) {
            c->state = 4;
        } else if (c->mode == 4) {
            c->state = 0xD0;
        } else {
            c->state = -0x1C;
        }
        return -0x1C;
    case -0x1B:
        if (c->mode == 1) {
            c->state = 0xD4;
        } else {
            c->state = -0x1B;
        }
        return -0x1B;
    case -0x1D:
        if (c->mode == 3) {
            c->state = 2;
        } else if (c->mode == 5) {
            c->state = 4;
        } else if (c->mode == 4) {
            c->state = 0xD0;
        } else {
            c->state = -0x1D;
        }
        return -0x1D;
    case 1:
        c->state = 0xD0;
        if (c->fileNo == -1) {
            return 3;
        }
        return 1;
    case 0:
        c->state = 0xD0;
        return 0;
    case 2:
        c->state = 0xD0;
        return 2;
    case 0xD4:
        if (c->fileNo == -1) {
            r = 0;
        } else {
            r = CARDFastDelete(c->chan, c->fileNo);
        }
        OSReport(" CARDFastDelete(): %d \n", r);
        if (r == 0) {
            c->state = 0xD5;
        } else if (r < 0 && r > -2) {
            c->state = 0xD4;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    case 0xD1:
        memset(&info, 0, sizeof(info));
        r = CARDOpen(c->chan, c->name, &info);
        if (r == 0) {
            c->fileNo = info.fileNo;
        } else {
            c->fileNo = -1;
        }
        OSReport(" CARDOpen(): %d \n", r);
        if (r == -1) {
            c->state = 0xD1;
            return -1;
        }
        if (r == -4 || (r > -2 && r < 1)) {
            c->state = 0xD2;
            return -1;
        }
        OSReport("Unexpected Error %d \n", r);
        c->state = 0;
        return -1;
    case 0xD3:
        r = CARDGetStatus(c->chan, c->tmpFileNo, &stat);
        OSReport(" CARDGetStatus(): %d \n", r);
        if (r == 0) {
            if (stat.length - 0x40U < stat.commentAddr) {
                c->state = 0xD6;
            } else {
                c->state = 0xD4;
            }
        } else if (r < 0 && r > -2) {
            c->state = 0xD3;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    case 0xD2:
        r = CARDOpen(c->chan, c->tmpName, &info);
        if (r == 0) {
            c->tmpFileNo = info.fileNo;
        } else {
            c->tmpFileNo = -1;
        }
        OSReport(" CARDOpen(): %d \n", r);
        if (r == -1) {
            c->state = 0xD2;
        } else if (r == -4) {
            if (c->tmpFileNo == -1 && c->fileNo == -1) {
                c->state = 0xD7;
                return -1;
            }
            c->state = 1;
            return -1;
        } else if (r > -2 && r < 1) {
            c->state = 0xD3;
            return -1;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    case 0xD0:
        strcpy(c->name, (const char*)c->desc);
        sprintf(c->tmpName, "%c%s", 0x21, c->name);
        c->state = 0xD1;
        return -1;
    case 0xD7:
        if (c->flagB == 0) {
            c->state = 1;
            return -1;
        }
        r = CARDFreeBlocks(c->chan, &bytesFree, &filesFree);
        need = c->desc->size;
        OSReport(" CARDFreeBlocks(): %d \n", r);
        if (r != -3 && r > -4) {
            if (r == 0) {
                if (filesFree > 0 && need <= bytesFree) {
                    c->state = 1;
                    return -1;
                }
                if (c->flagA != 0) {
                    c->state = -0x1C;
                    return -1;
                }
                c->state = -0x1D;
                return -1;
            }
            if (r < 0 && r > -2) {
                c->state = 0xD7;
                return -1;
            }
        }
        OSReport("Unexpected Error %d \n", r);
        c->state = 0;
        return -1;
    case 0xD5:
        r = CARDRename(c->chan, c->tmpName, c->name);
        if (r == 0) {
            c->fileNo = c->tmpFileNo;
        }
        OSReport(" CARDRename(): %d \n", r);
        if (r == 0) {
            c->state = 1;
        } else if (r < 0 && r > -2) {
            c->state = 0xD5;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    case 0xD6:
        if (c->tmpFileNo == -1) {
            r = 0;
        } else {
            r = CARDFastDelete(c->chan, c->tmpFileNo);
        }
        OSReport(" CARDFastDelete(): %d \n", r);
        if (r == 0) {
            c->state = 1;
        } else if (r < 0 && r > -2) {
            c->state = 0xD6;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    }
    OSPanic("MemCard.c", 0xC09, "Illegal state in CARDGuide1CheckFile\n");
    return 0;
}

// Debug trace of the format (Fig2) state.
void CardFig2PrintState(int state) {
    static int sPrev = (int)0xDEADF00D;
    if (state != sPrev) {
        sPrev = state;
        switch (state) {
        case 0xB0: OSReport("CARD_GUIDE1_FIG2_STAT_START\n"); break;
        case 0xB1: OSReport("CARD_GUIDE1_FIG2_STAT_PROBE\n"); break;
        case 0xB2: OSReport("CARD_GUIDE1_FIG2_STAT_MOUNT\n"); break;
        case 0xB3: OSReport("CARD_GUIDE1_FIG2_STAT_CHECK\n"); break;
        case 0xB4: OSReport("CARD_GUIDE1_FIG2_STAT_SECTOR\n"); break;
        case 0xB5: OSReport("CARD_GUIDE1_FIG2_STAT_FORMAT\n"); break;
        case 0xB6: OSReport("CARD_GUIDE1_FIG2_STAT_UNMOUNT\n"); break;
        case 1: OSReport("CARD_GUIDE1_RTRN_DONE\n"); break;
        case 2: OSReport("CARD_GUIDE1_RTRN_CONTINUE\n"); break;
        case -0x10: OSReport("CARD_GUIDE1_FIG2_MESG_CORRUPT\n"); break;
        case -0x13: OSReport("CARD_GUIDE1_FIG2_MESG_DAMAGED\n"); break;
        case -0x12: OSReport("CARD_GUIDE1_FIG2_MESG_NOSUPPORT\n"); break;
        case -0x0F: OSReport("CARD_GUIDE1_FIG2_MESG_WRONGDEVICE\n"); break;
        case -0x11: OSReport("CARD_GUIDE1_FIG2_MESG_NOCARD\n"); break;
        case -0x0E: OSReport("CARD_GUIDE1_FIG2_MESG_ENCODING\n"); break;
        case -0x0D: OSReport("CARD_GUIDE1_FIG2_MESG_FORMAT_SURE\n"); break;
        case -0x0C: OSReport("CARD_GUIDE1_FIG2_MESG_FORMAT_FAILED\n"); break;
        case -0x0B: OSReport("CARD_GUIDE1_FIG2_MESG_FORMAT_FAILED_NOCARD\n"); break;
        case -0x0A: OSReport("CARD_GUIDE1_FIG2_MESG_FORMAT_DONE\n"); break;
        default: OSPanic("MemCard.c", 0xB9F, "Illegal state in DEBUGPrintState\n"); break;
        }
    }
}

// State block of the format (Fig2) state machine.
struct CardFig2Ctx {
    int unk00;              // 0x00
    int unk04;              // 0x04
    int unk08;              // 0x08
    int cardState;          // 0x0C
    int* ext;               // 0x10 (+0x2C: format confirmed)
    int unk14;              // 0x14
    int chan;               // 0x18
    int mode;               // 0x1C
    int unk20;              // 0x20
    int state;              // 0x24
    void* workArea;         // 0x28
    int sectorSize;         // 0x2C
    int memSize;            // 0x30
};

int CardGuideFig2(CardFig2Ctx* c) {
    static int sFormatDelay = 0x1E;     // 0x804D583C
    int r;

    CardFig2PrintState(c->state);
    switch (c->state) {
    case 0:
        c->state = 0xB0;
        return 0;
    case -0xE:
        if (c->mode == 1) {
            c->state = -0xD;
        } else if (c->mode == 3) {
            c->state = 2;
        } else if (c->mode == 4) {
            c->state = 0xB0;
        } else {
            c->state = -0xE;
        }
        return -0xE;
    case -0x11:
        if (c->mode == 3) {
            c->state = 2;
        } else if (c->mode < 3 || c->mode > 4) {
            c->state = -0x11;
        } else {
            c->state = 0xB0;
        }
        return -0x11;
    case -0xF:
        if (c->mode == 3) {
            c->state = 2;
        } else if (c->mode < 3 || c->mode > 4) {
            c->state = -0xF;
        } else {
            c->state = 0xB0;
        }
        return -0xF;
    case -0x10:
        if (c->mode == 2) {
            c->state = 2;
        } else if (c->mode == 1) {
            c->state = -0xD;
        } else if (c->mode == 4) {
            c->state = 0xB0;
        } else {
            c->state = -0x10;
        }
        return -0x10;
    case -0x13:
        if (c->mode == 3) {
            c->state = 2;
        } else if (c->mode < 3 || c->mode > 4) {
            c->state = -0x13;
        } else {
            c->state = 0xB0;
        }
        return -0x13;
    case -0x12:
        if (c->mode == 3) {
            c->state = 2;
        } else if (c->mode < 3 || c->mode > 4) {
            c->state = -0x12;
        } else {
            c->state = 0xB0;
        }
        return -0x12;
    case -0xB:
        if (c->mode == 1) {
            c->state = 0xB0;
        } else {
            c->state = -0xB;
        }
        return -0xB;
    case -0xD:
        if (c->mode == 1) {
            c->state = 0xB5;
        } else if (c->mode < 1 || c->mode > 2) {
            c->state = -0xD;
        } else {
            c->state = 0xB0;
        }
        return -0xD;
    case -0xC:
        if (c->mode == 3) {
            c->state = 2;
        } else if (c->mode < 3 || c->mode > 4) {
            c->state = -0xC;
        } else if (c->ext[0x2C / 4] == 1) {
            c->state = 0xB0;
        } else {
            c->state = -0xC;
        }
        return -0xC;
    case -0xA:
        if (c->mode == 1) {
            c->state = 0xB0;
        } else {
            c->state = -0xA;
        }
        return -0xA;
    case 0xB0:
        c->state = 0xB1;
        return -1;
    case 0xB1:
        r = CARDProbeEx(c->chan, &c->memSize, &c->sectorSize);
        c->ext[0x2C / 4] = 0;
        OSReport(" CARDProbeEx(): %d \n", r);
        if (r == -1) {
            c->state = 0xB1;
        } else if (r == -3) {
            c->state = -0x11;
        } else if (r == -2) {
            c->state = -0xF;
        } else if (r == 0) {
            c->state = 0xB4;
        } else {
            OSReport("Unexpeted Error = %d \n", r);
            c->state = -0x13;
        }
        return -1;
    case 2:
        c->state = 0xB0;
        return 2;
    case 1:
        c->state = 0xB0;
        return 1;
    case 0xB2:
        r = CARDMount(c->chan, c->workArea, 0);
        OSReport(" CARDMount(): %d \n", r);
        if (r == -5 || r == -0x80) {
            c->state = -0x13;
        } else if (r == -0xD) {
            c->state = -0xE;
        } else if (r >= -6 && r < -0xD) {
            c->state = 0xB3;
        } else if (r == -1) {
            c->state = 0xB2;
        } else if (r == -3) {
            c->state = 0xB0;
        } else if (r == -2) {
            c->state = -0xF;
        } else if (r == 0) {
            c->state = 0xB3;
        } else {
            OSReport("Unexpeted Error = %d \n", r);
            c->state = -0x13;
        }
        return -1;
    case 0xB5:
        if (sFormatDelay > 0) {
            sFormatDelay--;
            r = -1;
        } else {
            r = CARDFormat(c->chan);
            OSReport(" CARDFormat(): %d \n", r);
        }
        if (r == -1) {
            c->state = 0xB5;
        } else if (r == -3) {
            c->state = -0xB;
            sFormatDelay = 0x1E;
            return -3;
        } else if (r == 0) {
            c->state = -0xA;
            sFormatDelay = 0x1E;
            return -3;
        } else {
            OSReport("Unexpeted Error = %d \n", r);
            c->state = -0xC;
            c->cardState = CARDProbe(c->unk00);
            sFormatDelay = 0x1E;
        }
        return -3;
    case 0xB4:
        if (c->sectorSize == 0x2000) {
            c->state = 0xB2;
        } else {
            c->state = -0x12;
        }
        return -1;
    case 0xB3:
        r = CARDCheck(c->chan);
        OSReport(" CARDCheck(): %d \n", r);
        switch (r) {
        case 0:
            c->state = 1;
            break;
        case -0xD:
            c->state = -0xE;
            break;
        case -6:
            c->state = -0x10;
            break;
        case -5:
            c->state = -0x13;
            break;
        case -3:
            c->state = 0xB0;
            break;
        case -1:
            c->state = 0xB3;
            break;
        default:
            OSReport("Unexpeted Error = %d \n", r);
            c->state = -0x13;
            break;
        }
        return -1;
    case 0xB6:
        r = CARDUnmount(c->chan);
        OSReport(" CARDUnFormat(): %d \n", r);
        if (r == 0) {
            c->state = -0x13;
        } else if (r < 0 && r > -2) {
            c->state = 0xB6;
        } else {
            OSReport("Unexpeted Error = %d \n", r);
            c->state = -0x13;
        }
        return -1;
    }
    OSPanic("MemCard.c", 0x8F0, "Illegal state in CARDGuideFig2\n");
    return 2;
}

// Debug trace of the save-file state.
void CardSavePrintState(int state) {
    static int sPrev = (int)0xDEADF00D;
    if (state != sPrev) {
        sPrev = state;
        switch (state) {
        case 0xE0: OSReport("CARD_GUIDE1_SAVE_STAT_START\n"); break;
        case 0xE1: OSReport("CARD_GUIDE1_SAVE_STAT_OPEN_FILE\n"); break;
        case 0xE2: OSReport("CARD_GUIDE1_SAVE_STAT_CREATE_TEMP\n"); break;
        case 0xE3: OSReport("CARD_GUIDE1_SAVE_STAT_GETSTATUS\n"); break;
        case 0xE4: OSReport("CARD_GUIDE1_SAVE_STAT_WRITE_TEMP\n"); break;
        case 0xE5: OSReport("CARD_GUIDE1_SAVE_STAT_SETSTATUS\n"); break;
        case 0xE6: OSReport("CARD_GUIDE1_SAVE_STAT_CLOSE_TEMP\n"); break;
        case 0xE7: OSReport("CARD_GUIDE1_SAVE_STAT_DELETE_FILE\n"); break;
        case 0xE8: OSReport("CARD_GUIDE1_SAVE_STAT_RENAME\n"); break;
        case 0xE9: OSReport("CARD_GUIDE1_SAVE_STAT_CLOSE_FILE\n"); break;
        case 0xEA: OSReport("CARD_GUIDE1_SAVE_STAT_WRITE_NOTEMP\n"); break;
        case 1: OSReport("CARD_GUIDE1_RTRN_DONE\n"); break;
        case 2: OSReport("CARD_GUIDE1_RTRN_CONTINUE\n"); break;
        case 0: OSReport("CARD_GUIDE1_RTRN_ERROR\n"); break;
        case 5: OSReport("CARD_GUIDE1_RTRN_NO_OVERWRITE\n"); break;
        case -0x3B: OSReport("CARD_GUIDE1_SAVE_MESG_OVERWRITE\n"); break;
        case -0x3A: OSReport("CARD_GUIDE1_SAVE_MESG_DONE\n"); break;
        case -0x39:
        case -0x38: OSReport("CARD_GUIDE1_SAVE_MESG_FAILED\n"); break;
        default: OSPanic("MemCard.c", 0x887, "DebugPrint case not supported"); break;
        }
    }
}

// State block of the save-file state machine, as the outer object.
struct CardSave {
    u8 pad00[0xA8];
    int unkA8;              // 0xA8
    u8 padAC[4];
    int chan;               // 0xB0
    int mode;               // 0xB4
    CardSaveDesc* desc;     // 0xB8
    int state;              // 0xBC
    char tmpName[0x20];     // 0xC0
    CARDFileInfo tmpInfo;   // 0xE0
    int tmpFileNo;          // 0xF4
    CARDStat stat;          // 0xF8
    char name[0x20];        // 0x164
    CARDFileInfo info;      // 0x184
    int pad198;
    int fileNo;             // 0x198
    int useTemp;            // 0x19C
};

static inline u32 CardCrc(u32* data, u32 len) {
    u32 crc = 0;
    u32 i;
    for (i = 0; i < len; i++) {
        crc = (crc << 8) ^ gCardCrcTable[((crc >> 24) ^ (int)((char*)data)[i]) & 0xFF];
    }
    return crc;
}

int CardGuide1SaveFile(CardSave* c) {
    int r;
    int i;
    int fileNo;
    u32* data;
    u32 len;

    CardSavePrintState(c->state);
    switch (c->state) {
    case 0xE0:
        strcpy(c->name, (const char*)c->desc);
        sprintf(c->tmpName, "%c%s", 0x21, c->name);
        c->fileNo = -1;
        c->tmpFileNo = -1;
        data = c->desc->data;
        len = c->desc->size;
        *data = 0;
        if (data[2] + data[1] <= len) {
            len = data[2] + data[1];
        }
        *data = CardCrc(data, len);
        DCFlushRange(data, len);
        c->state = 0xE1;
        c->useTemp = 1;
        return -1;
    case 0:
        c->state = 0xE0;
        return 0;
    case -0x39:
        if (c->mode == 1) {
            c->state = 2;
        } else {
            c->state = -0x39;
        }
        return -0x39;
    case -0x3B:
        if (c->mode == 1) {
            if (c->desc->saved == 0 && sCardOverwriteOk == 1) {
                c->state = 5;
            } else {
                c->state = 0xE2;
            }
        } else if (c->mode < 1 || c->mode > 2) {
            c->state = -0x3B;
        } else {
            c->state = 1;
        }
        return -0x3B;
    case -0x3A:
        if (c->mode == 1) {
            c->state = 1;
        } else {
            c->state = -0x3A;
        }
        return -0x3A;
    case -0x38:
        if (c->mode == 3) {
            c->state = 2;
        } else if (c->mode < 3 || c->mode > 4) {
            c->state = -0x38;
        } else {
            c->state = 0;
        }
        return -0x38;
    case 5:
        c->state = 0xE0;
        return 5;
    case 2:
        c->state = 0xE0;
        return 2;
    case 1:
        c->state = 0xE0;
        return 1;
    case 0xE6:
        r = CARDClose(&c->tmpInfo);
        OSReport(" CARDClose(): %d \n", r);
        if (r == 0) {
            c->state = 0xE7;
        } else if (r < 0 && r > -2) {
            c->state = 0xE6;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -2;
    case 0xE3:
        if (c->useTemp == 0) {
            r = CARDGetStatus(c->chan, c->fileNo, &c->stat);
        } else {
            r = CARDGetStatus(c->chan, c->tmpFileNo, &c->stat);
        }
        OSReport(" CARDGetStatus(): %d \n", r);
        if (r == 0) {
            if (c->useTemp == 0) {
                c->state = 0xEA;
            } else {
                c->state = 0xE4;
            }
        } else if (r < 0 && r > -2) {
            c->state = 0xE3;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -2;
    case 0xE5:
        c->stat.commentAddr = c->desc->commentAddr;
        c->stat.iconAddr = c->desc->iconAddr;
        c->stat.bannerFormat = (c->stat.bannerFormat & 0xFC) | c->desc->bannerFormat;
        c->stat.bannerFormat = (c->stat.bannerFormat & 0xFB) | (u8)c->desc->iconAnim;
        for (i = 0; (u32)i < (u32)c->desc->iconCount; i++) {
            u16 mask = ~(3 << (i * 2));
            c->stat.iconFormat = (c->stat.iconFormat & mask) | (c->desc->iconFormat << (i * 2));
            c->stat.iconSpeed = (c->stat.iconSpeed & mask) | (c->desc->iconSpeed[i] << (i * 2));
        }
        if ((u32)c->desc->iconCount < 8) {
            c->stat.iconFormat = c->stat.iconFormat & ~(3 << (c->desc->iconCount * 2));
            c->stat.iconSpeed = c->stat.iconSpeed & ~(3 << (c->desc->iconCount * 2));
        }
        if (c->useTemp == 0) {
            r = CARDSetStatus(c->chan, c->fileNo, &c->stat);
        } else {
            r = CARDSetStatus(c->chan, c->tmpFileNo, &c->stat);
        }
        if (r == 0) {
            if (c->useTemp == 0) {
                c->state = 0xE9;
            } else {
                c->state = 0xE6;
            }
        } else if (r < 0 && r > -2) {
            c->state = 0xE5;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -2;
    case 0xE4:
        r = CARDWrite(&c->tmpInfo, c->desc->data, c->desc->size, 0);
        OSReport(" CARDWrite(): %d \n", r);
        if (r == -1) {
            c->state = 0xE4;
        } else if (r == -5) {
            c->state = -0x38;
        } else if (r == 0) {
            c->state = 0xE5;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -2;
    case 0xE1:
        r = CARDOpen(c->chan, c->name, &c->info);
        OSReport(" CARDOpen(): %d \n", r);
        if (r == 0) {
            c->fileNo = c->info.fileNo;
        } else if (r == -4) {
            c->fileNo = -1;
        }
        if (r == -1) {
            c->state = 0xE1;
        } else if (r == -4) {
            c->state = 0xE2;
            return -1;
        } else if (r == 0) {
            if (c->desc->saved == 0) {
                c->state = -0x3B;
                return -1;
            }
            c->state = 0xE2;
            return -1;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    case 0xE2:
        r = CARDCreate(c->chan, c->tmpName, c->desc->size, &c->tmpInfo);
        OSReport(" CARDCreate(): %d \n", r);
        if (r == 0) {
            c->tmpFileNo = c->tmpInfo.fileNo;
        }
        if (r == -1) {
            c->state = 0xE2;
        } else if (r == 0) {
            c->state = 0xE3;
            return -2;
        } else if (r < -7 && r > -10) {
            c->useTemp = 0;
            c->state = 0xEA;
            return -2;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -2;
    case 0xE9:
        if (c->fileNo == -1) {
            r = 0;
        } else {
            r = CARDClose(&c->info);
        }
        OSReport(" CARDClose(): %d \n", r);
        if (r == 0) {
            c->desc->saved = 1;
            c->unkA8 = 0;
            c->state = -0x3A;
        } else if (r < 0 && r > -2) {
            c->state = 0xE9;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -2;
    case 0xE8:
        r = CARDRename(c->chan, c->tmpName, c->name);
        OSReport(" CARDRename(): %d \n", r);
        if (r == 0) {
            c->state = 0xE9;
        } else if (r < 0 && r > -2) {
            c->state = 0xE8;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -2;
    case 0xE7:
        if (c->fileNo == -1) {
            r = 0;
        } else {
            r = CARDFastDelete(c->chan, c->fileNo);
        }
        if (r == 0) {
            c->state = 0xE8;
        } else if (r < 0 && r > -2) {
            c->state = 0xE7;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -2;
    case 0xEA:
        r = CARDWrite(&c->info, c->desc->data, c->desc->size, 0);
        OSReport(" CARDWrite(): %d \n", r);
        if (r == -1) {
            c->state = 0xEA;
        } else if (r == -5) {
            c->state = -0x38;
        } else if (r == 0) {
            c->state = 0xE5;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -2;
    }
    OSPanic("MemCard.c", 0x56C, "Illegal state in CARDGuide1SaveFile\n");
    return 0;
}

// Debug trace of the read-file state.
void CardReadPrintState(int state) {
    static int sPrev = (int)0xDEADF00D;
    if (state != sPrev) {
        sPrev = state;
        switch (state) {
        case 0xC0: OSReport("CARD_GUIDE1_READ_STAT_START\n"); break;
        case 0xC1: OSReport("CARD_GUIDE1_READ_STAT_OPEN\n"); break;
        case 0xC2: OSReport("CARD_GUIDE1_READ_STAT_GETSTATUS\n"); break;
        case 0xC3: OSReport("CARD_GUIDE1_READ_STAT_READ\n"); break;
        case 0xC4: OSReport("CARD_GUIDE1_READ_STAT_CRC_CHECK\n"); break;
        case 0xC5: OSReport("CARD_GUIDE1_READ_STAT_CLOSE\n"); break;
        case 0xC6: OSReport("CARD_GUIDE1_READ_STAT_DELETE\n"); break;
        case 1: OSReport("CARD_GUIDE1_RTRN_DONE\n"); break;
        case 3: OSReport("CARD_GUIDE1_RTRN_NOFILE\n"); break;
        case 2: OSReport("CARD_GUIDE1_RTRN_CONTINUE\n"); break;
        case 0: OSReport("CARD_GUIDE1_RTRN_ERROR\n"); break;
        case -0x26: OSReport("CARD_GUIDE1_READ_MESG_CORRUPT_FILE\n"); break;
        case -0x27: OSReport("CARD_GUIDE1_READ_MESG_NOFILE\n"); break;
        case -0x25: OSReport("CARD_GUIDE1_READ_MESG_DONE\n"); break;
        case -0x24: OSReport("CARD_GUIDE1_READ_MESG_FAILED\n"); break;
        default: OSPanic("MemCard.c", 0x503, "DebugPrint case not supported"); break;
        }
    }
}

// State block of the read-file state machine.
struct CardRead {
    int chan;               // 0x00
    CardSaveDesc* desc;     // 0x04
    int mode;               // 0x08
    int state;              // 0x0C
    CARDFileInfo info;      // 0x10
    int pad24;
    CARDStat stat;          // 0x24
    int fileNo;             // 0x90
    int corrupt;            // 0x94
};

static inline int CardVerifyCrc(u32* data, u32 size) {
    u32 saved = *data;
    *data = 0;
    if ((int)data[1] < 1 || (int)data[2] < 1 || size < data[1] + data[2]) {
        return 0;
    }
    {
        u32 crc = CardCrc(data, data[1] + data[2]);
        if (saved == crc) {
            return 1;
        }
        OSReport("CRCVerifyCRC Error: CRCs doesn't match!! Saved CRC  = 0x%x Actual CRC = 0x%x \n", saved, crc);
        return 0;
    }
}

int CardGuide1ReadFile(CardRead* c) {
    int r;

    CardReadPrintState(c->state);
    switch (c->state) {
    case 3:
        c->state = 0xC0;
        return 3;
    case -0x24:
        if (c->mode == 1) {
            c->state = 2;
        } else {
            c->state = -0x24;
        }
        return -0x24;
    case -0x26:
        if (c->mode == 1) {
            c->corrupt = 1;
            c->state = 0xC6;
        } else {
            c->state = -0x26;
        }
        return -0x26;
    case -0x25:
        if (c->mode == 1) {
            c->state = 1;
        } else {
            c->state = -0x25;
        }
        return -0x25;
    case -0x27:
        if (c->mode == 1) {
            c->state = 3;
        } else if (c->mode < 1 || c->mode != 4) {
            c->state = -0x27;
        } else {
            c->state = 0xC0;
        }
        return -0x27;
    case 1:
        c->state = 0xC0;
        return 1;
    case 2:
        c->state = 0xC0;
        return 2;
    case 0:
        c->state = 0xC0;
        return 0;
    case 0xC3:
        r = CARDRead(&c->info, c->desc->data, c->desc->size, 0);
        OSReport(" CARDRead(): %d \n", r);
        if (r == 0) {
            DCInvalidateRange(c->desc->data, c->desc->size);
        }
        if (r == 0) {
            c->state = 0xC4;
        } else if (r < 0 && r > -2) {
            c->state = 0xC3;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    case 0xC1:
        r = CARDOpen(c->chan, c->desc->name, &c->info);
        OSReport(" CARDOpen(): %d \n", r);
        if (r == 0) {
            c->fileNo = c->info.fileNo;
        } else if (r == -4) {
            c->fileNo = -1;
        }
        if (r == -1) {
            c->state = 0xC1;
        } else if (r == -4) {
            c->state = -0x27;
            return -1;
        } else if (r == 0) {
            c->state = 0xC2;
            return -1;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    case 0xC2:
        r = CARDGetStatus(c->chan, c->fileNo, &c->stat);
        OSReport(" CARDGetStatus(): %d \n", r);
        if (r == 0) {
            c->state = 0xC3;
        } else if (r < 0 && r > -2) {
            c->state = 0xC2;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    case 0xC0:
        c->corrupt = 1;
        DCInvalidateRange(c->desc->data, c->desc->size);
        c->state = 0xC1;
        return -1;
    case 0xC6:
        r = CARDFastDelete(c->chan, c->fileNo);
        OSReport(" CARDFastDelete(): %d \n", r);
        if (r == 0) {
            if (c->corrupt == 1) {
                c->state = 3;
            } else {
                c->state = 1;
            }
        } else if (r < 0 && r > -2) {
            c->state = 0xC6;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    case 0xC4:
        if (CardVerifyCrc(c->desc->data, c->desc->size)) {
            c->state = 0xC5;
        } else {
            c->state = -0x26;
        }
        return -1;
    case 0xC5:
        r = CARDClose(&c->info);
        if (r == 0) {
            c->desc->saved = 1;
        }
        OSReport(" CARDClose(): %d \n", r);
        if (r == 0) {
            c->state = 1;
        } else if (r < 0 && r > -2) {
            c->state = 0xC5;
        } else {
            OSReport("Unexpected Error %d \n", r);
            c->state = 0;
        }
        return -1;
    }
    OSPanic("MemCard.c", 0x313, "Illegal state in CARDGuide1ReadFile\n");
    return 0;
}

// Fills a save buffer (title, subtitle, banner, icon and payload) and its description.
// CardSaveBufferInit (called by other units under its placeholder name)
extern "C" void fn_802E14B0(const char* fileName, const char* title, const char* subtitle, int size,
                        int* buf, int dataSize, const void* icon, const void* banner,
                        CardSaveDesc* desc) {
    *buf = 0;
    buf[2] = dataSize;
    buf[1] = 0x204C;
    strncpy((char*)(buf + 3), title, 0x20);
    strncpy((char*)(buf + 0xB), subtitle, 0x20);
    memcpy(buf + 0x613, icon, 0x800);
    memcpy(buf + 0x13, banner, 0x1800);
    strncpy(desc->name, fileName, 0x20);
    desc->size = size;
    desc->data = (u32*)buf;
    desc->bannerFormat = 2;
    desc->commentAddr = 0xC;
    desc->iconAddr = 0x4C;
    desc->iconFormat = 2;
    desc->iconCount = 1;
    desc->iconAnim = 4;
    desc->iconSpeed[0] = 3;
    desc->iconSpeed[1] = 0;
}

// State block of the card manager.  The sub-blocks are the per-operation contexts above.
struct CardMgr {
    int chan;               // 0x00
    int state;              // 0x04
    int result;             // 0x08
    int cardState;          // 0x0C
    int* ext;               // 0x10 (+0x2C: changed flag, +0x28: pointer to a flag word)
    int mode;               // 0x14
    u8 pad18[0x34 - 0x18];
    u8 check[0x70 - 0x34];  // 0x34: check-file context
    u8 pad70[0xA8 - 0x70];
    int unkA8;              // 0xA8
};

// Chooses the next state after the check-file pass.
void CardInitStatCheck(int* m) {
    int mode = m[0x14 / 4];
    if (mode == 2) {
        m[8 / 4] = CardGuide1CheckFile((CardCheck*)(m + 0x34 / 4), 0);
    } else {
        m[8 / 4] = CardGuide1CheckFile((CardCheck*)(m + 0x34 / 4), 1);
    }
    switch (m[8 / 4]) {
    case 2:
        m[4 / 4] = 2;
        break;
    case 0:
        m[4 / 4] = 0xA1;
        break;
    case 1:
        mode = m[0x14 / 4];
        if (mode == 2) {
            m[4 / 4] = 0xA4;
        } else if (mode >= 1 && mode < 2) {
            m[4 / 4] = 1;
        } else if (mode > 2 && mode < 4) {
            m[4 / 4] = 0xA3;
        } else {
            OSPanic("MemCard.c", 0x1F1, "Bad Command in FUNC_INIT_STAT_CHECK\n");
        }
        break;
    case 4:
        m[4 / 4] = 4;
        break;
    case 3:
        mode = m[0x14 / 4];
        if (mode == 2) {
            m[4 / 4] = 3;
        } else if (mode >= 1 && mode < 2) {
            m[4 / 4] = 3;
        } else if (mode > 2 && mode < 4) {
            m[4 / 4] = 0xA3;
        } else {
            OSPanic("MemCard.c", 0x208, "Bad Command in FUNC_INIT_STAT_CHECK\n");
        }
        break;
    default:
        if (m[8 / 4] < 0 && m[8 / 4] > -2) {
            m[4 / 4] = 0xA2;
        }
        break;
    }
}

// Top-level card state machine: dispatches to the Fig2 / check / save / read contexts.
// CardGuide1Init (called by other units under its placeholder name)
extern "C" int fn_802E1728(int* m, int flagB) {
    int r = CARDProbe(m[0]);
    int state;
    if (m[3] != (int)0xDEADF00D && r != m[3]) {
        m[3] = r;
        ((CardSaveDesc*)m[0x2E])->saved = 0;
        ((int*)m[4])[0x2C / 4] = 1;
        if (((int**)m[4])[0x28 / 4] != NULL) {
            *((int**)m[4])[0x28 / 4] = 1;
        }
    }
    m[7] = flagB;
    m[0xF] = flagB;
    m[0x6A] = flagB;
    m[0x2D] = flagB;
    state = m[1];
    switch (state) {
    case 5:
        m[1] = 0xA0;
        m[5] = 0;
        return 5;
    case 2:
        m[1] = 0xA1;
        m[5] = 0;
        return 2;
    case 3:
        m[1] = 0xA0;
        m[5] = 0;
        return 3;
    case 4:
        m[1] = 0xA0;
        m[5] = 0;
        return 4;
    case 0:
        m[1] = 0xA0;
        m[5] = 0;
        return 0;
    case 1:
        m[1] = 0xA0;
        m[5] = 0;
        return 1;
    case 0xA2:
        CardInitStatCheck(m);
        break;
    case 0xA0:
        m[9] = 0xB0;
        m[6] = m[0];
        m[0x10] = 0xD0;
        m[0xD] = m[0];
        m[0x2A] = 0;
        m[0xE] = m[4];
        m[0x2F] = 0xE0;
        m[0x2C] = m[0];
        m[0x2E] = m[4];
        m[0x6B] = 0xC0;
        m[0x68] = m[0];
        m[0x69] = m[4];
        m[2] = -1;
        if (m[5] == 2) {
            m[1] = 0xA4;
        } else if (m[5] == 0) {
            m[1] = 0xA0;
        } else if (m[5] >= 0 && m[5] < 2) {
            m[1] = 0xA1;
        } else if (m[5] < 4) {
            m[1] = 0xA3;
        }
        break;
    case 0xA1:
        m[2] = CardGuideFig2((CardFig2Ctx*)m);
        if (m[2] == 2) {
            m[1] = 2;
        } else if (m[2] > 0 && m[2] < 2) {
            m[3] = CARDProbe(m[0]);
            if (m[5] == 2) {
                m[1] = 0xA4;
            } else {
                m[1] = 0xA2;
            }
        }
        break;
    case 0xA4:
        m[2] = CardGuide1ReadFile((CardRead*)(m + 0x68));
        if (m[2] == 1) {
            m[1] = 1;
        } else if (m[2] == -1) {
            m[1] = 0xA4;
        } else if (m[2] < 1 && m[2] > -2) {
            m[1] = 0xA1;
        } else if (m[2] == 3) {
            m[1] = 3;
        } else if (m[2] < 3) {
            m[1] = 2;
        }
        break;
    case 0xA3:
        m[2] = CardGuide1SaveFile((CardSave*)m);
        if (m[2] == 2) {
            m[1] = 2;
        } else if (m[2] == 0) {
            m[1] = 0xA1;
        } else if (m[2] < 0 && m[2] > -2) {
            m[1] = 0xA3;
        } else if (m[2] > 0 && m[2] < 2) {
            m[1] = 1;
        } else if (m[2] == 5) {
            m[1] = 5;
        }
        break;
    default:
        OSPanic("MemCard.c", 0x164, "Illegal state in CARDGuide1Init\n");
        return 0;
    }
    r = m[2];
    if (r >= 0) {
        r = -1;
    }
    return r;
}

// Detects a card swap; returns 1 when the card state changed.
// CardChanged (called by other units under its placeholder name)
extern "C" int fn_802E1B38(int* m) {
    int r = CARDProbe(m[0]);
    if (m[3] == (int)0xDEADF00D || r == m[3]) {
        return 0;
    }
    m[3] = r;
    ((CardSaveDesc*)m[0x2E])->saved = 0;
    ((int*)m[4])[0x2C / 4] = 1;
    if (((int**)m[4])[0x28 / 4] != NULL) {
        *((int**)m[4])[0x28 / 4] = 1;
    }
    return 1;
}

// Starts an operation on an idle manager.
// CardStartOp (called by other units under its placeholder name)
extern "C" int fn_802E1BBC(int* m, int op) {
    if (m[0x14 / 4] != 0) {
        return 0;
    }
    if (m[4 / 4] != 0xA1) {
        m[4 / 4] = 0xA0;
    }
    m[0x14 / 4] = op;
    if (m[0x14 / 4] == 3 && m[4 / 4] != 0xA1) {
        m[4 / 4] = 0xA2;
    }
    return 1;
}

// CardSetBusy (called by other units under its placeholder name)
extern "C" void fn_802E1C10(int* m) {
    m[0xA8 / 4] = 1;
}

// Resets the state of the sub-machines.
// CardResetStates (called by other units under its placeholder name)
extern "C" void fn_802E1C1C(int* m) {
    m[4 / 4] = 0xA1;
    m[0x24 / 4] = 0xB0;
    m[0x40 / 4] = 0xD0;
    m[0xBC / 4] = 0xE0;
    m[0x1AC / 4] = 0xC0;
}

// Builds the CRC32 table and sets up the manager for `chan`.
// CardMgrInit (called by other units under its placeholder name)
extern "C" void fn_802E1C48(int* m, int chan, CardSaveDesc* desc, void* workArea) {
    u32 i;
    u32 v;
    u32* p = gCardCrcTable;
    for (i = 0; i < 256; i++, p++) {
        int bit;
        v = i << 24;
        for (bit = 0; bit < 8; bit++) {
            if (v & 0x80000000) {
                v = (v << 1) ^ 0x04C11DB7;
            } else {
                v = v << 1;
            }
        }
        *p = v;
    }
    m[1] = 0xA0;
    m[5] = 0;
    m[0] = chan;
    m[3] = 0xDEADF00D;
    m[4] = (int)desc;
    ((CardSaveDesc*)m[4])->saved = 0;
    memset(workArea, 0, 0xA000);
    m[10] = (int)workArea;
}

// ---- aim helper behaviour (0x802E1D84-0x802E2F08) ----------------------------------------
// A behaviour block that picks bone targets on a target actor and tells it about them.

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();
    String(const char* s);
    ~String() {}
    virtual void Release(char* p);
};

// Variable query/set message: the receiver reads or writes the first word of the string.
struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

extern "C" int SendMsg(int to, int msg, void* data);

inline void SetVar(int target, u32 hash, int value) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    SendMsg(target, 0x41D, &m);
}

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[12];
};

struct TargetInfo {
    u8 pad00[0x24];
    int x;                  // 0x24
    int y;                  // 0x28
    int z;                  // 0x2C
};

struct BehData {
    int cur;                // 0x00
    int next;               // 0x04
    int arg;                // 0x08
    int bone[4];            // 0x0C
    float timer;            // 0x1C
};

struct Beh {
    u8 pad00[0x10];
    int state;              // 0x10
    u16 nearMin;            // 0x14
    u16 nearMax;            // 0x16
    u16 farMin;             // 0x18
    u16 farMax;             // 0x1A
    u8 pad1C[0x2C - 0x1C];
    u32 flags;              // 0x2C
    u8 pad30[0x4C - 0x30];
    void (*onState)(void* owner, Beh* beh, int arg);    // 0x4C
    u8 pad50[4];
    BehData* data;          // 0x54
};

struct Actor {
    u8 pad00[0x38];
    int handle;             // 0x38
    u8 pad3C[0x84 - 0x3C];
    Vec3 pos;               // 0x84
    u8 pad90[0xCC - 0x90];
    int model;              // 0xCC
    u8 padD0[0xF0 - 0xD0];
    char* pose;             // 0xF0
    u8 padF4[0x138 - 0xF4];
    float dt;               // 0x138
    u8 pad13C[0x26C - 0x13C];
    int mode;               // 0x26C
    char conds[0x5C8 - 0x270]; // 0x270
    float unk5C8;           // 0x5C8
    u8 pad5CC[0x894 - 0x5CC];
    Beh beh;                // 0x894
    int idA;                // 0x8EC
    int idB;                // 0x8F0
};

struct AimPacket {
    u8 pad00[0x24];
    Vec3 pos;               // 0x24
    Vec3 pos2;              // 0x30
    Vec3 dir;               // 0x3C
    int target;             // 0x48
    float unk4C;            // 0x4C
    float unk50;            // 0x50
    float unk54;            // 0x54
    u8 pad58[4];
    int bone;               // 0x5C
};

int BehTarget(Beh* beh);
TargetInfo* OwnerGetInfo(u32 id);
float OwnerValue(Actor* a);
int OwnerId(Actor* a);
void BehUpdate(Actor* a, Beh* beh);
void BehDone(Beh* beh);
void BehReset(Beh* beh);
int HasCondition(void* flags, int bit);
int ModelFindBone(int model, int bone);
void* VAlloc(int size);
void VFree(void* p);
int AssetFind(u32 a, u32 b);
void ActorGetMatrix(Mtx34* m, void* actor);
void Mtx34StoreColumn(Mtx34* m, Vec3* v, int col);
extern "C" {
int rand(void);
float sqrtf(float x);
int fn_800F7C64(int a, u32 hash);
int fn_800FC3B8(int a);
int fn_8003AA9C(int model, int a, int b);
int fn_8003A3F0(int model, char* pose, int anim);
void fn_800A025C(Vec3* out, Mtx34* m, int col);
void fn_800A0168(AimPacket* p, Mtx34* m);
int Fn800F7DEC(int a, int b, u32 hash, int c, void* d);
void Fn800F7F5C(float f, int a, int b, u32 hash, int c, void* d);
}
extern u32 gAimBoneNames[4];        // 0x803C6318
struct AimState {
    void (*enter)(Actor* a, Beh* beh);
    void (*exit)(Actor* a, Beh* beh);
    int (*update)(Actor* a, void* b);
};
extern AimState gAimStates[8];      // 0x803C6328
extern float gAimDirFast;           // 0x804D3DB8
extern float gAimThreshA;           // 0x804D3DBC
extern float gAimThreshB;           // 0x804D3DC0
extern float gAimThreshC;           // 0x804D3DC4

static inline void AimNormalize(Vec3* v) {
    float ax = v->x < 0.0f ? -v->x : v->x;
    float ay = v->y < 0.0f ? -v->y : v->y;
    float az = v->z < 0.0f ? -v->z : v->z;
    if (!(ax > 1.1920929e-07f) && !(ay > 1.1920929e-07f) && !(az > 1.1920929e-07f)) {
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 0.0f;
    } else {
        float inv = 1.0f / sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
        v->x *= inv;
        v->y *= inv;
        v->z *= inv;
    }
}

// Aims `p` from the actor's bone at the target's position (offset up by `up`).
int AimPacketBuild(float up, Actor* a, AimPacket* p) {
    TargetInfo* t;
    Mtx34 m;
    Vec3 d;
    Vec3 rx;
    Vec3 rz;
    Vec3 up3;
    Vec3 fwd;
    char* bones;
    int pose;
    int tx, ty, tz, tzu;

    BehTarget(&a->beh);
    t = OwnerGetInfo(BehTarget(&a->beh));
    if (t == NULL) {
        return 0;
    }
    p->target = (int)t;
    p->unk4C = a->unk5C8;
    if (fn_800F7C64(a->idA, 0x20A5AA68) > 0 || fn_800FC3B8(0x12) != 0) {
        p->unk4C = p->unk4C * 1.5f;
    }
    tx = t->x;
    ty = t->y;
    tz = t->z;
    tzu = (int)((float)tz + 250.0f);
    pose = fn_8003A3F0(a->model, a->pose, fn_8003AA9C(a->model, 0, 0));
    ActorGetMatrix(&m, (void*)(pose + p->bone * 0x30));
    p->pos2.x = m.m[3];
    p->pos2.y = m.m[7];
    p->pos2.z = m.m[11];
    p->pos.x = m.m[3];
    p->pos.y = m.m[7];
    p->pos.z = m.m[11];
    {
        float* bp = (float*)(pose + p->bone * 0x30 + 0x24);
        d.x = (float)(tx - (int)bp[0]);
        d.y = (float)(ty - (int)bp[1]);
        d.z = (float)((tzu + (int)up) - (int)bp[2]);
    }
    AimNormalize(&d);
    ActorGetMatrix(&m, (void*)pose);
    fn_800A025C(&rx, &m, 0);
    fn_800A025C(&rz, &m, 2);
    rz.x = rx.y * d.z - rx.z * d.y;
    rz.y = rx.z * d.x - rx.x * d.z;
    rz.z = rx.x * d.y - rx.y * d.x;
    rx.x = d.y * rz.z - d.z * rz.y;
    rx.y = d.z * rz.x - d.x * rz.z;
    rx.z = d.x * rz.y - d.y * rz.x;
    Mtx34StoreColumn(&m, &rx, 0);
    Mtx34StoreColumn(&m, &d, 1);
    Mtx34StoreColumn(&m, &rz, 2);
    p->unk54 = 0.0f;
    switch (a->mode) {
    case 0:
        p->unk50 = 0.0f;
        break;
    case 1:
        p->unk50 = 0.33f;
        break;
    case 2:
        p->unk50 = 0.67f;
        break;
    case 3:
        p->unk50 = 1.0f;
        break;
    }
    p->dir = d;
    fn_800A0168(p, &m);
    return 1;
}

static inline void BehSelectRange(Actor* a, Beh* beh, int hasInfo) {
    float dist;
    beh->flags = 0;
    if (!HasCondition(a->conds, 9) || !HasCondition(a->conds, 10) || !HasCondition(a->conds, 0xB)) {
        if (beh->state == 0) {
            beh->flags |= 1;
            beh->flags |= 2;
        }
        if (BehTarget(beh) != -1) {
            BehTarget(beh);
            if (OwnerGetInfo(BehTarget(beh)) != NULL) {
                dist = OwnerValue(a);
                if ((float)beh->nearMin <= dist && dist <= (float)beh->nearMax) {
                    beh->flags |= 4;
                }
                if ((float)beh->farMin <= dist && dist <= (float)beh->farMax) {
                    beh->flags |= 8;
                }
                if (beh->flags == 0) {
                    if ((float)beh->nearMin <= dist) {
                        beh->flags = 0x80;
                    } else {
                        beh->flags = 0x40;
                    }
                }
            }
        }
    }
}

void AimBehUpdate(Actor* a, Beh* beh) {
    BehData* d;
    if (beh == NULL) {
        return;
    }
    d = beh->data;
    BehUpdate(a, beh);
    BehSelectRange(a, beh, 0);
    d->timer -= a->dt;
    if (d->timer <= 0.0f) {
        int id = OwnerId(a);
        if (Fn800F7DEC(a->idA, a->idB, 0xF829DB93, id, 0) == 0x4A3E183) {
            int i;
            for (i = 0; i < 4; i++) {
                int bone = d->bone[i];
                if (bone != -1) {
                    AimPacket pk;
                    d->bone[i] = -1;
                    pk.bone = bone;
                    if (AimPacketBuild(0.0f, a, &pk)) {
                        Fn800F7F5C(0.0f, a->idA, a->idB, 0xF829DB93, OwnerId(a), &pk);
                    }
                }
                SetVar(a->handle, 0x42E653B, -1);
                d->next = 0;
            }
        }
    }
}

void AimBehScan(Actor* a, Beh* beh) {
    if (beh == NULL) {
        return;
    }
    BehUpdate(a, beh);
    BehSelectRange(a, beh, 0);
}

void AimBehNop1(void) {
}

void AimBehNop2(void) {
}

void AimBehPick(Actor* a, Beh* beh) {
    BehData* d;
    Vec3 dir;
    int n;
    int i;
    int j;
    TargetInfo* t;
    if (beh == NULL) {
        return;
    }
    d = beh->data;
    BehDone(beh);
    BehTarget(&a->beh);
    t = OwnerGetInfo(BehTarget(&a->beh));
    if (t == NULL) {
        d->next = 0;
        return;
    }
    dir.x = (float)t->x - a->pos.x;
    dir.y = (float)t->y - a->pos.y;
    dir.z = (float)t->z - a->pos.z;
    AimNormalize(&dir);
    memset(d->bone, 0xFF, 0x10);
    {
        float r = (float)rand() / 32767.0f;
        if (r >= gAimThreshC) {
            if (r >= gAimThreshC + gAimThreshB) {
                if (r >= gAimThreshA + gAimThreshC + gAimThreshB) {
                    n = 1;
                } else {
                    n = 2;
                }
            } else {
                n = 3;
            }
        } else {
            n = 4;
        }
    }
    for (i = n; i-- > 0;) {
        float best = -1.1f;
        for (j = 0; j < 4; j++) {
            Mtx34 m;
            Vec3 col;
            int bone;
            float dot;
            QueryMsg q;
            q.unk8 = 0;
            SendMsg(a->handle, 0x411, &q);
            ActorGetMatrix(&m, &q);
            fn_800A025C(&col, &m, 1);
            dot = dir.z * col.z + dir.x * col.x + dir.y * col.y;
            bone = ModelFindBone(a->model, gAimBoneNames[j]);
            if (best < dot && d->bone[0] != bone && d->bone[1] != bone && d->bone[2] != bone &&
                d->bone[3] != bone) {
                d->bone[i] = bone;
                best = dot;
            }
        }
        SetVar(a->handle, 0x42E653B, d->bone[i]);
        d->timer = gAimDirFast;
    }
}

void AimBehReset(void* unused, Actor* a) {
    BehData* d;
    if (a != NULL) {
        d = ((Beh*)a)->data;
        d->next = 0;
        d->arg = 0;
        BehDone((Beh*)a);
    }
}

// AimBehFalse (called by other units under its placeholder name)
extern "C" int fn_802E29F0(void) {
    return 0;
}

static inline void AimEnter(Actor* a, BehData* d) {
    if (gAimStates[d->cur].enter != NULL) {
        gAimStates[d->cur].enter(a, &a->beh);
    }
}

// AimBehMsg (called by other units under its placeholder name)
extern "C" int fn_802E29F8(Actor* a, Beh* beh, int msg) {
    BehData* d;
    if (beh == NULL) {
        return 0;
    }
    d = beh->data;
    if (msg == 1) {
        d->next = 4;
        d->arg = 1;
        if (d->cur == 0) {
            if (gAimStates[0].exit != NULL) {
                gAimStates[0].exit(a, &a->beh);
            }
            d->cur = d->next;
            a->beh.state = d->next;
            if (a->beh.onState != NULL) {
                a->beh.onState(a, &a->beh, d->arg);
            }
            AimEnter(a, d);
        }
    } else if (msg < 1) {
        if (msg >= 0) {
            d->next = 0;
            d->arg = 0;
            if (gAimStates[d->cur].exit != NULL) {
                gAimStates[d->cur].exit(a, &a->beh);
            }
            d->cur = d->next;
            a->beh.state = d->next;
            if (a->beh.onState != NULL) {
                a->beh.onState(a, &a->beh, d->arg);
            }
            AimEnter(a, d);
        }
    } else if (msg == 0x1B83833F) {
        if (gAimStates[d->cur].exit != NULL) {
            gAimStates[d->cur].exit(a, &a->beh);
        }
        d->cur = d->next;
        a->beh.state = d->next;
        if (a->beh.onState != NULL) {
            a->beh.onState(a, &a->beh, d->arg);
        }
        AimEnter(a, d);
    }
    return 1;
}

// AimBehTick (called by other units under its placeholder name)
extern "C" int fn_802E2C30(Actor* a, Beh* beh) {
    BehData* d;
    if (beh == NULL) {
        return 0;
    }
    d = beh->data;
    if (!HasCondition(a->conds, 9) && !HasCondition(a->conds, 10) && !HasCondition(a->conds, 0xB)) {
        if (gAimStates[d->cur].update != NULL) {
            gAimStates[d->cur].update(a, beh);
        }
        if (d->next != d->cur) {
            if (gAimStates[d->cur].exit != NULL) {
                gAimStates[d->cur].exit(a, (Beh*)beh);
            }
            d->cur = d->next;
            beh->state = d->next;
            if (beh->onState != NULL) {
                beh->onState(a, beh, d->arg);
            }
            if (gAimStates[d->cur].enter != NULL) {
                gAimStates[d->cur].enter(a, (Beh*)beh);
            }
        }
    } else {
        BehReset(beh);
        BehDone(beh);
    }
    return 1;
}

// AimBehFree (called by other units under its placeholder name)
extern "C" int fn_802E2D98(void* unused, Beh* beh) {
    if (beh != NULL) {
        VFree(beh->data);
    }
    return beh != NULL;
}

// AimBehInit (called by other units under its placeholder name)
extern "C" int fn_802E2DD0(Actor* a, Beh* beh) {
    BehData* d;
    if (beh == NULL) {
        return 0;
    }
    if (a == NULL) {
        return 0;
    }
    d = (BehData*)VAlloc(0x20);
    if (d == NULL) {
        beh->data = NULL;
        return 0;
    }
    beh->data = d;
    beh->state = 0;
    d->cur = 0;
    d->next = 0;
    d->arg = 0;
    if (gAimStates[d->cur].enter != NULL) {
        gAimStates[d->cur].enter(a, beh);
    }
    gAimBoneNames[0] = AssetFind(0xF1754BF2, 0x25357BEE);
    gAimBoneNames[1] = AssetFind(0xF1754BF2, 0x7E9E82);
    gAimBoneNames[2] = AssetFind(0xF1754BF2, 0xB49CC4C1);
    gAimBoneNames[3] = AssetFind(0xF1754BF2, 0x824BB112);
    return 1;
}

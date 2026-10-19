// Credits.cpp: the credits screen (0x802E3674-0x802E3AE4): scrolls the STR_CREDITS_%03d lines; its screen
// descriptor is named "No Bot". The tip messages are in CreditsTip.cpp and the "No Bot" behaviour in NoBot.cpp
// (one constant/data pool per file in retail: STATUS G3). Names are descriptive guesses; matched against the DOL.

typedef unsigned long u32;
typedef unsigned short u16;

extern "C" {
int sprintf(char* s, const char* format, ...);
int strncmp(const char* a, const char* b, unsigned long n);
void* memset(void* dst, int val, unsigned long n);
}

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHashKey(const char* s) {
    const char* p;
    u32 h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    for (p = s; *p != 0; p++) {
        h = h * 31 + (unsigned char)toupper(*p);
    }
    h = h % 0xFFFFFFFF;
    return h;
}

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();                                  // 0x8028BA9C
    virtual ~Buffer();                         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

// "new"/"free" query: the receiver writes its answer over the first word of the string.
struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

struct ScreenMsg {
    int unk0;
    const char* name;
    int unk8;
    int unkC;
};

struct ScreenType {
    const char* name;
    void (*init)(void);
    void (*enter)(void);
    void (*exit)(void);
    void (*update)(void);
    void (*draw)(void* s, int pass);
    int (*msg)(void* s, int msg);
    void* unk1C;
};

struct CreditsData {
    const char* lines[250];
    u32 lengths[250];
    int count;
    int state;
    int scroll;
    int unk7DC;
    int top;
};

extern "C" int SendMsg(int to, int msg, void* data);                 // 0x800265D4
int AssetFind(u32 table, u32 key);                        // 0x8007EB34
const char* StrFindKey(u32 key, u32* len);                // 0x80086234
const char* StrGetIndex(int index, u32* len);             // 0x800862F0
int PadPressed(int pad, int a, int button);               // 0x800DC930
void FontRelease(int font);                               // 0x8007EEA4
int FontLoad(int a, const char* name, int b, int c, int d, int e); // 0x8007F004
void Fn8021E654(int on);                                  // 0x8021E654
void Fn80140778(void);                                    // 0x80140778
int Fn800816B0(void);                                     // 0x800816B0
void TextBegin(void);                                     // 0x801ED0C8
void TextSetMode(int mode);                               // 0x801ED1A0
void TextSetSize(int size);                               // 0x801ED2A0
void TextSetAlign(int align);                             // 0x801ED15C
void TextSetColor(int r, int g, int b, int a);            // 0x801ED1E4
void TextSetBox(int x0, int x1, int y0, int y1);          // 0x801ED3FC
void TextDraw(const char* text, u32 len);                 // 0x801EB11C
void TextSetRect(int x0, int x1, int y0, int y1, int a, int b, int c, int d); // 0x801ED2E4
void TextSetStyle(int r, int g, int b, int a, int e, int f, int g2, int h, int i, int j, int k, int l); // 0x801ED4E4
void TextSetFlags(int a, int b);                          // 0x801ED29C


CreditsData sCredits;

int gCreditsFontSize = 16;
int gCreditsLineHeight = 24;
int gLetterboxFont = -1;

inline int Query(int target, int msg, u32 hash) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    return *(int*)&m.result;
}

int CreditsMsg(void* s, int msg);
void CreditsDraw(void* s, int pass);
void CreditsUpdate(void);
void CreditsExit(void);
void CreditsEnter(void);
void CreditsInit(void);

ScreenType gCreditsScreen = {
    "No Bot", CreditsInit, CreditsEnter, CreditsExit, CreditsUpdate, CreditsDraw, CreditsMsg, 0,
};

int CreditsMsg(void* s, int msg) {
    switch (msg) {
    case 0x41D:
        return 0;
    case 0x41E:
        return 0;
    }
    return 0;
}

void CreditsDraw(void* s, int pass) {
    int count;
    int i;
    int y;
    int line;
    CreditsData* d;

    if (pass == 0) {
        TextBegin();
        TextSetMode(2);
        TextSetSize(gCreditsFontSize);
        TextSetAlign(4);
        TextSetColor(255, 255, 255, 255);
        d = &sCredits;
        i = 0;
        y = d->scroll;
        count = (d->scroll - d->unk7DC) / gCreditsLineHeight;
        for (; i < count; i++) {
            TextSetBox(0, 640, 0, y);
            line = d->top + i;
            if (line == d->count) {
                break;
            }
            TextSetColor(255, 255, 255, 255);
            TextDraw(d->lines[line], d->lengths[line]);
            y -= gCreditsLineHeight;
        }
    }
}

void CreditsUpdate(void) {
    ScreenMsg m;

    if (PadPressed(0, 0, 2) == 1 || PadPressed(1, 0, 2) == 1 || PadPressed(2, 0, 2) == 1 ||
        PadPressed(3, 0, 2) == 1) {
        sCredits.state = 2;
    }

    switch (sCredits.state) {
    case 0:
        if (++sCredits.scroll == 479) {
            sCredits.state = 1;
        }
        break;
    case 1:
        if (++sCredits.top < sCredits.count) {
            sCredits.scroll = sCredits.scroll - gCreditsLineHeight;
            sCredits.state = 0;
        } else {
            sCredits.state = 2;
        }
        break;
    case 2:
        m.name = "MainMenu";
        SendMsg(0x38000000, 0x5600, &m);
        break;
    }
}

void CreditsExit(void) {
    FontRelease(gLetterboxFont);
    Fn8021E654(0);
}

void CreditsEnter(void) {
    CreditsData* d;
    CreditsData* p;
    int i;
    char key[32];

    d = &sCredits;
    p = d;
    d->count = 0;
    for (i = 0; i < 250; i++) {
        sprintf(key, "STR_CREDITS_%03d", i);
        p->lines[0] = StrFindKey(StrHashKey(key), &p->lengths[0]);
        if (strncmp("ERROR", p->lines[0], 5) == 0) {
            break;
        }
        d->count++;
        p = (CreditsData*)&p->lines[1];
    }

    sCredits.scroll = 30;
    sCredits.unk7DC = 0;
    Query(0x02000000, 0x41D, 0x640D1F4D);

    Fn80140778();
    Fn8021E654(1);
    gLetterboxFont = FontLoad(0, "LBox On", 8, 0, 0, 0);
}

void CreditsInit(void) { memset(&sCredits, 0, sizeof(CreditsData)); }



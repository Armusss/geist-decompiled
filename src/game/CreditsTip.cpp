// CreditsTip.cpp: on-screen tip messages (0x802E3AE4-0x802E3EF8), shown for gTipTimeMax ms in a letterbox.
// Split from Credits.cpp (own .sdata/.sbss/.sdata2 in retail). Names are descriptive guesses.

typedef unsigned long u32;
typedef unsigned short u16;

extern "C" {
int sprintf(char* s, const char* format, ...);
int strncmp(const char* a, const char* b, unsigned long n);
void* memset(void* dst, int val, unsigned long n);
}


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


float gTipTimeMax = 5000.0f;
int gTipIndex = -1;
int gTipWidth = 500;
int gTipX = -13;
int gTipTop = 82;
int gTipBottom = 105;
int gTipStyle = 15;
int gTipStyle2 = -2;

float sTipTime;
int sTipActive;
int sLetterbox;
const char* sTipText;
u32 sTipLength;

inline int Query(int target, int msg, u32 hash) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    return *(int*)&m.result;
}

inline int TipShown(int r) {
    if (r == 0 || Fn800816B0() == -1) {
        return 0;
    }
    return 1;
}

void CreditsTipDraw(void) {
    int r;

    r = Query(0x4B000000, 0x41E, 0xC6D2473B);

    if (TipShown(r) && (int)sTipLength > 0) {
        TextSetRect(320 - gTipWidth / 2 + gTipX, 320 + gTipWidth / 2 + gTipX, gTipBottom - gTipTop, gTipBottom, 0,
                    sTipLength - 1, 0, 0);
        TextSetStyle(255, 255, 255, 255, gTipStyle, 0, 0, 0, 0, 0, gTipStyle2, 0);
        TextSetFlags(1, 0);
        TextDraw(sTipText, sTipLength);
    }
}

inline void TipReset(void) {
    sTipTime = 0.0f;
    gTipIndex = -1;
    sTipText = 0;
    sTipLength = 0;
}

inline float Clamp(float x, float lo, float hi) {
    float r;
    if (x < lo) {
        r = lo;
    } else if (x > hi) {
        r = hi;
    } else {
        r = x;
    }
    return r;
}

void CreditsTipUpdate(float dt) {
    if (sTipActive == 0 && Fn800816B0() > -1) {
        sTipActive = 1;
        TipReset();
    }

    if (Fn800816B0() > -1) {
        if (gTipIndex != -1) {
            sTipTime = sTipTime - dt;
            sTipTime = Clamp(sTipTime, 0.0f, gTipTimeMax);
            if (0.0f == sTipTime) {
                TipReset();
            }
        }
    } else {
        sTipActive = 0;
        TipReset();
    }
}

void CreditsTipClear(void) { TipReset(); }

void CreditsTipShow(int key) {
    if (key == (int)0xAA9CD166) {
        sLetterbox = 1;
    } else if (key == (int)0xA8FD5AA8) {
        sLetterbox = 0;
    } else if (key == 0) {
        TipReset();
    } else {
        gTipIndex = AssetFind(0xBAA22E62, key);
        if (gTipIndex != -1) {
            sTipText = StrGetIndex(gTipIndex, &sTipLength);
            sTipTime = gTipTimeMax;
        }
    }
}

void CreditsTipReset(void) { TipReset(); }

void CreditsTipNop(void) {}

int CreditsTipVisible(void) {
    int r;

    r = Query(0x4B000000, 0x41E, 0xC6D2473B);

    if (TipShown(r) && (gTipIndex != -1 || sLetterbox != 0)) {
        return 1;
    }
    return 0;
}


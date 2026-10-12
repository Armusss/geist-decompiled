// Credits.cpp: the credits screen, on-screen tip messages and the "No Bot" behaviour (0x802E3674-0x802E4858).
// - The credits screen scrolls the STR_CREDITS_000.. strings up the screen until "ERROR" (the string table's
//   answer for a missing key), then returns to "MainMenu"; any controller can skip it.
// - Tip messages: a string shown in a box for a while (CreditsTip*), plus a letterbox ("LBox On") flag.
// - A small state-machine behaviour with a per-state {enter, exit, update} table.
// Names are descriptive guesses; the code is matched against the retail DOL.

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

int SendMsg(int to, int msg, void* data);                 // 0x800265D4
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
        TextSetRect(gTipX + (320 - gTipWidth / 2), gTipX + gTipWidth / 2 + 320, gTipBottom - gTipTop, gTipBottom, 0,
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

// ---- "No Bot" behaviour ----

struct Owner;
struct Beh;

typedef void (*BehStateFunc)(Owner* o, Beh* b);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

struct NoBotData {
    int state;
    int next;
    u32 flags;
    float timer;
};

struct Beh {
    char pad0[0x10];
    int state;
    char pad14[0x14];
    u16 delay;
    char pad2A[2];
    u32 flags;
    char pad30[0x24];
    NoBotData* data;
};

struct Owner {
    char pad0[0x38];
    int target;
    char pad3C[0xFC];
    float speed;
    char pad13C[0x134];
    char conditions[0x67C];
    int unk8EC;
    int unk8F0;
};

void BehUpdate(Owner* o, Beh* b);                         // 0x801632A8
void BehDone(Beh* b);                                     // 0x80162EBC
void BehReset(Beh* b);                                    // 0x801633D4
int OwnerId(Owner* o);                                    // 0x800A3A40
int HasCondition(void* conditions, int which);            // 0x800A7AC4
int Fn800F7DEC(int a, int b, int c, int d, void* e);      // 0x800F7DEC
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f); // 0x800F7F5C
int Fn80161FF4(Owner* o, void* buf, float f);            // 0x80161FF4
void Fn8016295C(Owner* o, Beh* b);                        // 0x8016295C
void* VAlloc(int size);
void VFree(void* p);

extern BehState gNoBotStates[8];

void NoBotState7Update(Owner* o, Beh* b) {
    if (b) {
        NoBotData* d = b->data;
        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}

void NoBotState6Update(Owner* o, Beh* b) {
    if (b) {
        NoBotData* d = b->data;
        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}

void NoBotState4Update(Owner* o, Beh* b) {
    if (b) {
        NoBotData* d = b->data;
        BehUpdate(o, b);
        Fn8016295C(o, b);
        b->flags &= ~2;
        b->flags &= ~8;
        if ((d->flags & 1) && (d->flags & 0x10)) {
            switch (Fn800F7DEC(o->unk8EC, o->unk8F0, 0x0020EE36, OwnerId(o), 0)) {
            case 0xC0BECF7E: {
                QueryMsg m;
                int sent;
                int r;
                int target = o->target;
                m.hash = 0xBDC9494E;
                *(int*)&m.result = 0;
                m.unk8 = 0;
                sent = SendMsg(target, 0x41E, &m);
                r = *(int*)&m.result;
                if (sent == 0 || r != 0) {
                    d->next = 6;
                }
                break;
            }
            case 0x04A3E183:
                if (!(b->flags & 4)) {
                    d->next = 0;
                }
                break;
            case 0x11D2C025:
            case 0x3DA260F7:
                break;
            }
        }
    }
}

void NoBotState0Update(Owner* o, Beh* b) {
    if (b) {
        BehUpdate(o, b);
        Fn8016295C(o, b);
        b->flags &= ~2;
        b->flags &= ~8;
    }
}

void NoBotState7Exit(Owner* o, Beh* b) {}

void NoBotState6Exit(Owner* o, Beh* b) {}

void NoBotState4Exit(Owner* o, Beh* b) {
    char buf[96];

    if (b) {
        NoBotData* d = b->data;
        Fn800F7F5C(o->unk8EC, o->unk8F0, 0x0D19E338, OwnerId(o), buf, o->speed);
        d->next = 0;
    }
}

void NoBotState0Exit(Owner* o, Beh* b) {}

void NoBotState7Enter(Owner* o, Beh* b) {
    if (b) {
        b->data->flags = 0;
        BehDone(b);
    }
}

void NoBotState6Enter(Owner* o, Beh* b) {
    if (b) {
        b->data->flags = 0;
        BehDone(b);
    }
}

void NoBotState4Enter(Owner* o, Beh* b) {
    char buf[96];

    if (b) {
        b->data->flags = 0;
        BehDone(b);
        if (Fn800F7DEC(o->unk8EC, o->unk8F0, 0x96224FCC, OwnerId(o), 0) == 0x04A3E183) {
            if (Fn80161FF4(o, buf, 0.0f)) {
                Fn800F7F5C(o->unk8EC, o->unk8F0, 0x96224FCC, OwnerId(o), buf, o->speed);
            }
        }
    }
}

void NoBotState0Enter(Owner* o, Beh* b) {
    if (b) {
        b->data->flags = 0;
        BehDone(b);
        if (Fn800F7DEC(o->unk8EC, o->unk8F0, 0xE6B71BCC, OwnerId(o), 0) == 0x04A3E183) {
            b->flags |= 0x10;
        }
    }
}

int NoBotQuery(void) { return 0; }

int NoBotMsg(Owner* o, Beh* b, int msg) {
    NoBotData* d;
    char buf[96];

    if (!b) {
        return 0;
    }

    d = b->data;
    switch (msg) {
    case 0:
        d->next = 0;
        break;
    case 1:
        if (d->state == 0) {
            d->next = 4;
        }
        break;
    case 2:
        if (d->state == 0) {
            d->next = 5;
        }
        break;
    case 3:
        d->next = 6;
        break;
    case 4:
        if (d->state == 0) {
            d->next = 7;
        }
        break;
    case 0x1B838341:
        d->flags |= 0x10;
        break;
    case 0x0020EE36:
        if (Fn800F7DEC(o->unk8EC, o->unk8F0, msg, OwnerId(o), 0) == 0x04A3E183 && d->timer >= b->delay) {
            d->flags |= 1;
            d->timer = 0.0f;
            if (!Fn80161FF4(o, buf, 0.0f)) {
                return 0;
            }
            Fn800F7F5C(o->unk8EC, o->unk8F0, msg, OwnerId(o), buf, o->speed);
            d->flags &= ~0x100;
        }
        break;
    case 0xE6B71BCC:
    case 0x968CBE3D:
        if (!(d->flags & 0x20)) {
            d->flags |= 0x20;
            Fn800F7F5C(o->unk8EC, o->unk8F0, msg, OwnerId(o), 0, o->speed);
        }
        break;
    }
    return 1;
}

int NoBotUpdate(Owner* o, Beh* b) {
    NoBotData* d;

    if (!b) {
        return 0;
    }

    d = b->data;
    if (HasCondition(o->conditions, 9) || HasCondition(o->conditions, 10) || HasCondition(o->conditions, 11)) {
        d->flags = 0;
        BehReset(b);
        BehDone(b);
        if (d->state == 4) {
            d->state = 0;
            Fn800F7F5C(o->unk8EC, o->unk8F0, 0x0D19E338, OwnerId(o), 0, o->speed);
        }
    } else {
        d->timer = d->timer + (u16)o->speed;
        if (gNoBotStates[d->state].update) {
            gNoBotStates[d->state].update(o, b);
        }
        if (d->next != d->state) {
            if (gNoBotStates[d->state].exit) {
                gNoBotStates[d->state].exit(o, b);
            }
            d->state = d->next;
            b->state = d->next;
            if (gNoBotStates[d->state].enter) {
                gNoBotStates[d->state].enter(o, b);
            }
        }
    }
    return 1;
}

int NoBotFree(Owner* o, Beh* b) {
    if (!b) {
        return 0;
    }
    VFree(b->data);
    return 1;
}

int NoBotInit(Owner* o, Beh* b) {
    NoBotData* d;

    if (!b) {
        return 0;
    }

    d = (NoBotData*)VAlloc(sizeof(NoBotData));
    if (d == 0) {
        b->data = 0;
        return 0;
    }

    b->data = d;
    b->state = 0;
    d->state = 0;
    d->next = 0;
    d->flags = 0;
    d->timer = 0.0f;
    if (gNoBotStates[d->state].enter) {
        gNoBotStates[d->state].enter(o, b);
    }
    return 1;
}

BehState gNoBotStates[8] = {
    { NoBotState0Enter, NoBotState0Exit, NoBotState0Update },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { NoBotState4Enter, NoBotState4Exit, NoBotState4Update },
    { 0, 0, 0 },
    { NoBotState6Enter, NoBotState6Exit, NoBotState6Update },
    { NoBotState7Enter, NoBotState7Exit, NoBotState7Update },
};

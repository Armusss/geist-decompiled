// NoBot.cpp: the "No Bot" character behaviour (0x802E3EF8-0x802E4858), registered in gBehTypes (u_80161F40).
// Split from Credits.cpp (own .data/.sdata2 in retail). Names are descriptive guesses.

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


// ---- "No Bot" behaviour ----

struct Owner;
struct Beh;

typedef void (*BehStateFunc)(void* owner, void* beh);

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

void NoBotState7Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        NoBotData* d = b->data;
        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}

void NoBotState6Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        NoBotData* d = b->data;
        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}

// Sends a query and reports whether it was delivered; the answer comes back through out.
inline int QuerySend(int* out, int target, u32 hash) {
    QueryMsg m;
    int sent;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    sent = SendMsg(target, 0x41E, &m);
    *out = *(int*)&m.result;
    return sent;
}

void NoBotState4Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        NoBotData* d = b->data;
        BehUpdate(o, b);
        Fn8016295C(o, b);
        b->flags &= ~2;
        b->flags &= ~8;
        if ((d->flags & 1) && (d->flags & 0x10)) {
            switch (Fn800F7DEC(o->unk8EC, o->unk8F0, 0x0020EE36, OwnerId(o), 0)) {
            case 0xC0BECF7E: {
                int r;
                if (QuerySend(&r, o->target, 0xBDC9494E) == 0 || r != 0) {
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

void NoBotState0Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehUpdate(o, b);
        Fn8016295C(o, b);
        b->flags &= ~2;
        b->flags &= ~8;
    }
}

void NoBotState7Exit(void* owner, void* beh) {}

void NoBotState6Exit(void* owner, void* beh) {}

void NoBotState4Exit(void* owner, void* beh) {
    char buf[96];

    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        NoBotData* d = b->data;
        Fn800F7F5C(o->unk8EC, o->unk8F0, 0x0D19E338, OwnerId(o), buf, o->speed);
        d->next = 0;
    }
}

void NoBotState0Exit(void* owner, void* beh) {}

void NoBotState7Enter(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        b->data->flags = 0;
        BehDone(b);
    }
}

void NoBotState6Enter(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        b->data->flags = 0;
        BehDone(b);
    }
}

void NoBotState4Enter(void* owner, void* beh) {
    char buf[96];

    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        b->data->flags = 0;
        BehDone(b);
        if (Fn800F7DEC(o->unk8EC, o->unk8F0, 0x96224FCC, OwnerId(o), 0) == 0x04A3E183) {
            if (Fn80161FF4(o, buf, 0.0f)) {
                Fn800F7F5C(o->unk8EC, o->unk8F0, 0x96224FCC, OwnerId(o), buf, o->speed);
            }
        }
    }
}

void NoBotState0Enter(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        b->data->flags = 0;
        BehDone(b);
        if (Fn800F7DEC(o->unk8EC, o->unk8F0, 0xE6B71BCC, OwnerId(o), 0) == 0x04A3E183) {
            b->flags |= 0x10;
        }
    }
}

int NoBotQuery(void* owner, void* beh) { return 0; }

int NoBotMsg(void* owner, void* beh, int msg) {
    Owner* o;
    Beh* b;
    NoBotData* d;
    char buf[96];

    if (!beh) {
        return 0;
    }

    o = (Owner*)owner;
    b = (Beh*)beh;
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
    case 0x1B83833F:
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

int NoBotUpdate(void* owner, void* beh) {
    Owner* o;
    Beh* b;
    NoBotData* d;

    if (!beh) {
        return 0;
    }

    o = (Owner*)owner;
    b = (Beh*)beh;
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
            gNoBotStates[d->state].update(owner, beh);
        }
        if (d->next != d->state) {
            if (gNoBotStates[d->state].exit) {
                gNoBotStates[d->state].exit(owner, beh);
            }
            d->state = d->next;
            b->state = d->next;
            if (gNoBotStates[d->state].enter) {
                gNoBotStates[d->state].enter(owner, beh);
            }
        }
    }
    return 1;
}

int NoBotFree(void* owner, void* beh) {
    Beh* b;

    if (!beh) {
        return 0;
    }
    b = (Beh*)beh;
    VFree(b->data);
    return 1;
}

int NoBotInit(void* owner, void* beh) {
    Beh* b;
    NoBotData* d;

    if (!beh) {
        return 0;
    }

    b = (Beh*)beh;

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
        gNoBotStates[d->state].enter(owner, beh);
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

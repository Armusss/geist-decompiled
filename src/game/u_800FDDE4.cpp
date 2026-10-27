// u_800FDDE4.cpp: HUD message/notification system (0x800FDDE4-0x80100EB0).
// Message handlers read and write the shared HUD state block; queries answer command hashes
// with a value (handles, counts, floats) and setters update it.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef signed long s32;
typedef unsigned char u8;

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
int SendMsg(int to, int msg, void* data);                    // 0x800265D4
u32 fn_80128DE0(void* owner, int msg, void* data, int arg);  // 0x80128DE0
void fn_80140834(int arg);                                   // 0x80140834
u32 fn_8014CCB4(void* data);                                 // 0x8014CCB4
u32 fn_8014CDA8(void* data);                                 // 0x8014CDA8
u32 fn_80153B9C(void);                                       // 0x80153B9C
int fn_802A1D04(void);                                       // 0x802A1D04
void fn_802A1D0C(int mode);                                  // 0x802A1D0C
void* memcpy(void* dst, const void* src, unsigned long n);
void* memset(void* dst, int c, unsigned long n);
unsigned long strlen(const char* s);
char* strncat(char* dst, const char* src, unsigned long n);
float fn_8013E988(void);                                     // 0x8013E988
}

// Text laid out through the debug/2D text system (0x8007E1E8-0x801ED...).
void TextSetStyle(int, int, int, int, int, int, int, int, int, int, int, int);
void TextSizeScaled(int size);
void DebugTextPos(int x, int y, int mode);
void TextSetAlign(int align);
void TextSetFlags(int flags, int arg);
void DebugText(const char* text, ...);
void TextDraw(const char* text, u32 handle);

int ActorCount();                                      // ActorCount__Fv        0x80049930
const char* StrFindKeySkip(u32 hash);                  // StrFindKeySkip__FUl   0x80086364
int PlayerGetPossessor(int id);                        // PlayerGetPossessor__Fi 0x80049888

extern float lbl_804D2D8C;  // .sdata, 0x804D2D8C
extern int lbl_804D4BD8;    // .sbss,  0x804D4BD8

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
    ~String() {}
    virtual void Release(char* p);
};

// Reply block of a 0x41D query: the receiver writes its answer over the first word.
struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

// A message addressed to the HUD system: command hash, then an in/out value word.
struct HudMsg {
    int unk0;    // 0x00
    int cmd;     // 0x04
    int unk8;    // 0x08
    int value;   // 0x0C
};

// Interface object polled before a message is processed.
struct HudCheck {
    void* pad[6];                 // 0x00
    int (*run)(void* state);      // 0x18
};

// The shared HUD state block (lbl_8039E748, 0x160 bytes).
struct HudState {
    int mode;                     // 0x00
    int field04;                  // 0x04
    char pad08[0x14];             // 0x08
    int count;                    // 0x1C
    int field20;                  // 0x20
    int handles[8];               // 0x24
    char msgs[8][16];             // 0x44
    int fieldC4;                  // 0xC4
    int arrC8[8];                 // 0xC8
    int arrE8[8];                 // 0xE8
    int arr108[8];                // 0x108
    int field128;                 // 0x128
    int field12C;                 // 0x12C
    int field130;                 // 0x130
    int field134;                 // 0x134
    int field138;                 // 0x138
};

extern HudState lbl_8039E748;
extern HudCheck* lbl_8039E730[];

// One line entry of a HUD slot: the message text, a countdown, a colour, the text scale,
// the text handle (-1 = debug text) and the hash of the message.
struct HudSlotEntry {
    char text[0xB0];              // 0x00
    float timer;                  // 0xB0
    float colorR;                 // 0xB4
    float colorG;                 // 0xB8
    float colorB;                 // 0xBC
    float scale;                  // 0xC0
    char padC4[4];                // 0xC4
    int state;                    // 0xC8
    int textHandle;               // 0xCC
    u32 hash;                     // 0xD0
};

// A HUD slot holds four timing entries (4 * 0xD4 = 0x350 bytes).
struct HudSlot {
    HudSlotEntry entries[4];
};

extern HudSlot lbl_8045D9F0[4];  // .bss, 0x8045D9F0
extern int lbl_8045E730[5];      // .bss, 0x8045E730 (per-slot disable flags)

// The notification settings block (lbl_8045E740, 0x24 bytes): three countdown values, the
// "value disabled" bits, the notification owner handle and its cached values.
struct HudNotify {
    float f00;                    // 0x00  (8045E740)
    float f04;                    // 0x04
    float f08;                    // 0x08
    u32 flags;                    // 0x0C  (8045E74C)
    u32 handle;                   // 0x10  (8045E750)
    float f14;                    // 0x14  (8045E754)
    float f18;                    // 0x18  (8045E758)
    int f1C;                      // 0x1C  (8045E75C)
    u32 f20;                      // 0x20  (8045E760)
    int pad24;                    // 0x24
};

extern HudNotify lbl_8045E740;   // .bss, 0x8045E740
extern float lbl_804D4BE0;       // .sbss, 0x804D4BE0 (running text y)
extern signed char lbl_804D4BE4; // .sbss, 0x804D4BE4 (y initialised)
extern float lbl_804D7500;       // .sdata2, 0x804D7500 (0.0f)
extern float lbl_804D7504;       // .sdata2, 0x804D7504 (message lifetime)
extern float lbl_804D7508;       // .sdata2, 0x804D7508 (default message text scale)
extern float lbl_804D750C;       // .sdata2, 0x804D750C (first text y)
extern float lbl_804D7510;       // .sdata2, 0x804D7510 (lifetime divisor)
extern float lbl_804D7514;       // .sdata2, 0x804D7514 (colour/lifetime cap)
extern float lbl_804D7518;       // .sdata2, 0x804D7518 (0..1 -> 0..255)
extern float lbl_804D751C;       // .sdata2, 0x804D751C (text y scale)
extern float lbl_804D7520;       // .sdata2, 0x804D7520 (text y step)
extern float lbl_804D7528;       // .sdata2, 0x804D7528 (0.0f)
extern float lbl_804D752C;       // .sdata2, 0x804D752C
extern float lbl_804D7530;       // .sdata2, 0x804D7530
extern double lbl_804D7538;      // .sdata2, 0x804D7538 (2^52, int->double helper)
extern float lbl_804D7540;       // .sdata2, 0x804D7540

extern unsigned char __upper_map[];  // 0x803C6B08 (character case table)

int fn_800FD6EC(int arg);                       // 0x800FD6EC
void fn_801408A8(int a, int b);                 // 0x801408A8
void fn_801403D0(int a, int b);                 // 0x801403D0
void fn_8013EB40(u32 key, int* out);            // 0x8013EB40
void fn_8016359C(int mode);                     // 0x8016359C
void fn_801635F4(float, float, float, int, int, int, int, int, int, int, int,
                 int, int, int, int, int, int); // 0x801635F4
int ActorGet(int id);                           // ActorGet__Fi   0x802A2024
float fn_802A2080(void);                        // 0x802A2080
void fn_802A1DA4(void);                         // 0x802A1DA4
float fn_800A1394(void);                        // 0x800A1394
int PlayerInHost(void* actor);                  // PlayerInHost__FP5Actor 0x80049750
void ActorPlayAnim(int handle, u32 hash, int arg); // ActorPlayAnim__FiUli 0x800C5654
u32 StrFindKey(u32 hash, u32* out);             // StrFindKey__FUlPUl 0x80086234
void DebugTextSetKey(u32 key);

int HudMsgQuery(void* state, void* msg);
int HudMsgSet(void* state, void* msg);
void HudRefreshSlots(HudState* s);
int HudNotifySet(void* state, void* data);
int HudMenuUpdate(const char* title, float dt);

// Message hook of the HUD system: lets the active HUD interface pre-empt, then handles
// the notification messages and forwards the menu queries.
int HudMsgDispatch(void* owner, int msg, void* data, int arg) {
    int r;
    int idx = lbl_8039E748.mode;

    r = 0;
    if (idx >= 0 && (u32)idx < 6) {
        HudCheck* check = lbl_8039E730[idx];
        if (check != 0 && check->run != 0) {
            r = check->run(&lbl_8039E748);
        }
    }
    if (r != 0) {
        return 1;
    }
    if (fn_80128DE0(owner, msg, data, arg) == 1) {
        return 1;
    }
    switch (msg) {
    case 0xC14: {
        int i;
        int count;

        if (*(int*)((char*)data + 0xC) != 0x515632E0) {
            break;
        }
        count = ActorCount();
        lbl_8039E748.count = lbl_8039E748.field20 + count;
        for (i = 0; i < count; i++) {
            u32 handle;

            lbl_8039E748.handles[i] = ((u32)i & 0xFFFFFF) | 0x02000000;
            handle = lbl_8039E748.handles[i];
            {
                QueryMsg q;

                q.hash = 0x99E99508;
                *(int*)&q.result = i;
                q.unk8 = 0;
                SendMsg(handle, 0x41D, &q);
            }
            sprintf(lbl_8039E748.msgs[i], StrFindKeySkip(0x1B149A29), i + 1);
        }
        break;
    }
    case 0x41D:
        if (HudMsgSet(&lbl_8039E748, data) == 1) {
            return 1;
        }
        break;
    case 0x41E:
        if (HudMsgQuery(&lbl_8039E748, data) == 1) {
            return 1;
        }
        break;
    case 0x7104:
        return fn_8014CCB4(data);
    case 0x7105:
        return fn_8014CDA8(data);
    }
    return 0;
}

// Answers a HUD query command by writing the requested value into the message.
int HudMsgQuery(void* state, void* msg) {
    HudState* s = (HudState*)state;
    HudMsg* m = (HudMsg*)msg;

    switch (m->cmd) {
    case 0xC541822C:
        switch (s->mode) {
        case 1:
            m->value = 0xC00D5BD1;
            break;
        case 2:
            m->value = 0x14E49BD1;
            break;
        case 3:
            m->value = 0x220393;
            break;
        case 0:
        default:
            m->value = 0;
            break;
        }
        return 1;
    case 0xA267AD6C:
        m->value = lbl_8039E748.fieldC4;
        return 1;
    case 0x0C343772:
        m->value = s->field138;
        return 1;
    case 0x6B6D9FD6:
        m->value = lbl_8039E748.arr108[(u32)m->value >> 16];
        return 1;
    case 0x0E4043D9: {
        int v = m->value;
        int i;

        do {
            i = 0;
            for (int n = 0; n < lbl_8039E748.count; n++, i++) {
                if (lbl_8039E748.handles[n] == v) {
                    goto found;
                }
            }
        } while ((int)((u32)v >> 24) == 0x13 &&
                 ((v = fn_80153B9C()), (int)((u32)v >> 24) == 2) &&
                 (*(int*)(PlayerGetPossessor((int)v) + 0x2740) == 0));
        i = -1;
    found:
        m->value = lbl_8039E748.arr108[i];
        return 1;
    }
    case 0x67FFEA96:
        m->value = lbl_8039E748.field128;
        return 1;
    case 0x50F42175:
        m->value = lbl_8039E748.field12C;
        return 1;
    case 0x3777E06F:
        m->value = lbl_8039E748.field130;
        return 1;
    case 0x6A055115:
        *(float*)&m->value = lbl_804D2D8C;
        return 1;
    case 0xC6D2473B:
        m->value = lbl_8039E748.field04;
        return 1;
    case 0x0E83B88B:
        if (s->mode != 0 && s->mode >= 0 &&
            s->mode < 4) {
            m->value = lbl_804D4BD8;
            return 1;
        }
        m->value = 1;
        return 1;
    }
    return 0;
}

// Applies a HUD setting command: mode selection, per-slot values, and the handle tables.
int HudMsgSet(void* state, void* msg) {
    HudState* s = (HudState*)state;
    HudMsg* m = (HudMsg*)msg;

    switch (m->cmd) {
    case 0xC541822C:
        switch (m->value) {
        case 0xC00D5BD1:
            s->mode = 1;
            break;
        case 0x14E49BD1:
            s->mode = 2;
            break;
        case 0x220393:
            s->mode = 3;
            break;
        default:
            s->mode = 0;
            break;
        }
        fn_802A1D0C(s->mode);
        return 1;
    case 0xA267AD6C:
        lbl_8039E748.fieldC4 = m->value;
        HudRefreshSlots(&lbl_8039E748);
        return 1;
    case 0x4531C86F: {
        u32 v = (u32)m->value;

        lbl_8039E748.arrC8[v >> 16] = v & 0xF;
        lbl_8039E748.arrE8[v >> 16] = (v >> 4) & 0xF;
        return 1;
    }
    case 0x0C343772:
        s->field138 = m->value;
        return 1;
    case 0x6B6D9FD6:
        lbl_8039E748.arr108[(u32)m->value >> 16] = (u32)m->value & 0xFFFF;
        return 1;
    case 0xC6D2473B:
        lbl_8039E748.field04 = m->value;
        return 1;
    case 0x0E83B88B:
        lbl_804D4BD8 = m->value;
        return 1;
    }
    return 0;
}

// Re-seeds the HUD slot table after a mode change: picks the per-slot codes, clears the
// counters, asks the HUD manager how many extra records exist and marks their handles.
void HudRefreshSlots(HudState* s) {
    int n;

    if (s->count == 4 && s->arrE8[0] == 0 && s->arrE8[1] == 0 && s->arrE8[2] == 0 && s->arrE8[3] == 0 &&
        s->arrC8[0] == 0 && s->arrC8[1] == 0 && s->arrC8[2] == 0 && s->arrC8[3] == 0 &&
        s->field12C == 0 && s->field130 == 0) {
        if (fn_802A1D04() == 3) {
            s->field128 |= 0xC;
            s->arrE8[1] = 1;
            s->arrE8[3] = 1;
            s->arrC8[1] = 1;
            s->arrC8[3] = 1;
        } else {
            s->field128 |= 0x3C;
            s->arrC8[1] = 1;
            s->arrC8[2] = 2;
            s->arrC8[3] = 3;
        }
    }
    s->fieldC4 = 0;
    s->field128 = 0;
    s->field130 = 0;
    s->field12C = 0;
    {
        QueryMsg q;

        q.hash = 0xDE1D5536;
        *(int*)&q.result = 0;
        q.unk8 = 0;
        SendMsg(0x4B000000, 0x41E, &q);
        n = *(int*)&q.result;
    }
    n += ActorCount();
    for (int i = 0; i < n; i++) {
        int hi;
        u32 bit = 4 << s->arrC8[i];

        if ((s->field128 & bit) == 0) {
            s->field128 |= bit;
            s->fieldC4++;
            hi = (u32)s->handles[i] >> 24;
            if (hi == 2) {
                if ((s->field12C & bit) == 0) {
                    s->field12C |= bit;
                    continue;
                }
            }
            if (hi == 0x13) {
                if ((s->field130 & bit) == 0) {
                    s->field130 |= bit;
                }
            }
        }
    }
}

// Clears the four countdown timers of one HUD slot.
void HudTimerReset(int slot) {
    if (slot < 0) {
        return;
    }
    if (slot < 4) {
        lbl_8045D9F0[slot].entries[0].timer = lbl_804D7500;
        lbl_8045D9F0[slot].entries[1].timer = lbl_804D7500;
        lbl_8045D9F0[slot].entries[2].timer = lbl_804D7500;
        lbl_8045D9F0[slot].entries[3].timer = lbl_804D7500;
        return;
    }
}

// Restarts one timer of a HUD slot; when the timer runs out (value <= 0) the entry is
// marked done and the following entries are shifted down, leaving a zeroed tail entry.
// Drops one expired entry of a HUD slot: the entries below it move up and the last
// entry is zeroed.
static inline void HudExpireEntry(int slot, int i) {
    int j;

    if (slot >= 0 && slot < 4) {
        HudSlot* s = &lbl_8045D9F0[slot];

        if (s->entries[i].state == 1) {
            s->entries[i].state = 0;
        }
        for (j = i + 1; j < 4; j++) {
            memcpy(&s->entries[j - 1], &s->entries[j], sizeof(HudSlotEntry));
        }
        memset(&s->entries[j - 1], 0, sizeof(HudSlotEntry));
    }
}

void HudTimerSet(int slot, float value) {
    if (slot >= 0 && slot < 4) {
        float zero = lbl_804D7500;
        HudSlot* s = &lbl_8045D9F0[slot];

        for (int i = 0; i < 4; i++) {
            HudSlotEntry* e = &s->entries[i];

            if (e->timer > zero) {
                e->timer = value;
                if (value <= zero) {
                    HudExpireEntry(slot, i);
                }
            }
        }
    }
}

// Adds a message with an explicit character count to one HUD slot: hashes the text,
// reuses an entry whose timer expired, ignores a message whose hash is already shown,
// and shifts the entries down when none has expired (the new message then takes the
// last place). The count is kept in the entry so the draw code can pass a length.
void HudMessageN(int slot, const char* text, u32 len, float r, float g, float b, float scale) {
    u32 hash;
    int idx;
    int i;
    int n = (int)len;

    if (text == 0 || slot < 0 || slot >= 4) {
        return;
    }
    hash = 0;
    for (i = 0; i < n; i++) {
        hash = hash * 31 + (u8)text[i];
    }
    hash %= 0xFFFFFFFFu;

    idx = -1;
    {
        HudSlot* s = &lbl_8045D9F0[slot];

        for (i = 0; i < 4; i++) {
            HudSlotEntry* e = &s->entries[i];

            if (hash == e->hash) {
                idx = -1;
                break;
            }
            if (idx < 0 && e->timer <= lbl_804D7500) {
                idx = i;
            }
        }
        if (i == 4) {
            if (idx < 0) {
                if (s->entries[0].state == 1) {
                    s->entries[0].state = 0;
                }
                for (i = 1; i < 4; i++) {
                    memcpy(&s->entries[i - 1], &s->entries[i], sizeof(HudSlotEntry));
                }
                memset(&s->entries[3], 0, sizeof(HudSlotEntry));
                idx = 3;
            } else if (s->entries[idx].state == 1) {
                s->entries[idx].state = 0;
            }
        }
        if (idx < 0) {
            return;
        }
        memcpy(s->entries[idx].text, text, 0xAE);
        s->entries[idx].textHandle = (int)len;   // 0xCC: explicit character count
        s->entries[idx].timer = lbl_804D7504;
        s->entries[idx].colorR = r;
        s->entries[idx].colorG = g;
        s->entries[idx].colorB = b;
        s->entries[idx].scale = scale;
        s->entries[idx].hash = hash;
        s->entries[idx].state = 1;
    }
}

// Adds a null-terminated message to one HUD slot: same entry selection as HudMessageN,
// hashing the whole string; the entry keeps -1 (draw with the debug text routine) and
// the default text scale.
void HudMessage(int slot, const char* text, float r, float g, float b) {
    u32 hash;
    int idx;
    int i;
    int len;

    if (text == 0 || slot < 0 || slot >= 4) {
        return;
    }
    hash = 0;
    len = strlen(text);
    for (i = 0; i < len; i++) {
        hash = hash * 31 + (u8)text[i];
    }
    hash %= 0xFFFFFFFFu;

    idx = -1;
    {
        HudSlot* s = &lbl_8045D9F0[slot];

        for (i = 0; i < 4; i++) {
            HudSlotEntry* e = &s->entries[i];

            if (hash == e->hash) {
                idx = -1;
                break;
            }
            if (idx < 0 && e->timer <= lbl_804D7500) {
                idx = i;
            }
        }
        if (i == 4) {
            if (idx < 0) {
                if (s->entries[0].state == 1) {
                    s->entries[0].state = 0;
                }
                for (i = 1; i < 4; i++) {
                    memcpy(&s->entries[i - 1], &s->entries[i], sizeof(HudSlotEntry));
                }
                memset(&s->entries[3], 0, sizeof(HudSlotEntry));
                idx = 3;
            } else if (s->entries[idx].state == 1) {
                s->entries[idx].state = 0;
            }
        }
        if (idx < 0) {
            return;
        }
        {
            HudSlotEntry* e = &s->entries[idx];

            strncat(e->text, text, 0xAE);
            e->textHandle = -1;         // 0xCC: -1 draws the whole string
            e->timer = lbl_804D7504;
            e->colorR = r;
            e->colorG = g;
            e->colorB = b;
            e->scale = lbl_804D7508;
            e->hash = hash;
            e->state = 1;
        }
    }
}

// Adds a message to one HUD slot: hashes the text, reuses an entry whose timer expired,
// ignores a message whose hash is already shown, and shifts the entries down when none
// has expired (the new message then takes the last place).
void HudMessagePush(int slot, const char* text, float r, float g, float b, float scale) {
    u32 hash;
    int idx;
    int len;
    int i;

    if (text == 0 || slot < 0 || slot >= 4) {
        return;
    }
    hash = 0;
    len = strlen(text);
    for (i = 0; i < len; i++) {
        hash = hash * 31 + (u8)text[i];
    }
    hash %= 0xFFFFFFFFu;

    idx = -1;
    {
        HudSlot* s = &lbl_8045D9F0[slot];

        for (i = 0; i < 4; i++) {
            HudSlotEntry* e = &s->entries[i];

            if (hash == e->hash) {
                idx = -1;
                break;
            }
            if (idx < 0 && e->timer <= lbl_804D7500) {
                idx = i;
            }
        }
        if (i == 4) {
            if (idx < 0) {
                if (s->entries[0].state == 1) {
                    s->entries[0].state = 0;
                }
                for (i = 1; i < 4; i++) {
                    memcpy(&s->entries[i - 1], &s->entries[i], sizeof(HudSlotEntry));
                }
                memset(&s->entries[3], 0, sizeof(HudSlotEntry));
                idx = 3;
            } else if (s->entries[idx].state == 1) {
                s->entries[idx].state = 0;
            }
        }
        if (idx < 0) {
            return;
        }
        {
            HudSlotEntry* e = &s->entries[idx];

            strncat(e->text, text, 0xAE);
            e->textHandle = -1;
            e->timer = lbl_804D7504;
            e->colorR = r;
            e->colorG = g;
            e->colorB = b;
            e->scale = scale;
            e->hash = hash;
            e->state = 1;
        }
    }
}

// Draws the live message entries of one HUD slot through the 2D text system.
void HudDrawSlotMessages(int slot) {
    float zero;
    float step;
    float y;
    HudSlotEntry* e;

    if (slot < 0) {
        return;
    }
    if (slot > 4) {
        return;
    }
    if (lbl_8045E730[slot] != 0) {
        return;
    }
    if (!lbl_804D4BE4) {
        lbl_804D4BE4 = 1;
        lbl_804D4BE0 = lbl_804D750C;
    }
    step = fn_8013E988();
    y = lbl_804D4BE0;
    zero = lbl_804D7500;
    e = &lbl_8045D9F0[slot].entries[0];
    for (int i = 0; i < 4; i++) {
        if (e->timer > zero) {
            float a = e->timer / lbl_804D7510;

            if (a > lbl_804D7514) {
                a = lbl_804D7514;
            }
            TextSetStyle((int)(lbl_804D7518 * e->colorR) & 0xFF,
                         (int)(lbl_804D7518 * e->colorG) & 0xFF,
                         (int)(lbl_804D7518 * e->colorB) & 0xFF,
                         (int)(lbl_804D7518 * a * step) & 0xFF,
                         0, 0, 0, 0, 0, 4, 0, 0);
            TextSizeScaled((int)e->scale);
            DebugTextPos(0x140, (int)(lbl_804D751C * (lbl_804D7514 - y)), 2);
            TextSetAlign(4);
            TextSetFlags(2, 0);
            if (e->textHandle == -1) {
                DebugText(e->text);
            } else {
                TextDraw(e->text, e->textHandle);
            }
            y += lbl_804D7520;
        }
        e++;
    }
}

// Clears the per-slot disable flag; -1 clears the flags of the first four slots.
void HudSlotFlagClear(int slot) {
    if (slot == -1) {
        lbl_8045E730[0] = 0;
        lbl_8045E730[1] = 0;
        lbl_8045E730[2] = 0;
        lbl_8045E730[3] = 0;
        return;
    }
    if (slot <= 0) {
        return;
    }
    if (slot > 4) {
        return;
    }
    lbl_8045E730[slot] = 0;
}

// Sets the per-slot disable flag; -1 sets the flags of the first four slots.
void HudSlotFlagSet(int slot) {
    if (slot == -1) {
        lbl_8045E730[0] = 1;
        lbl_8045E730[1] = 1;
        lbl_8045E730[2] = 1;
        lbl_8045E730[3] = 1;
        return;
    }
    if (slot <= 0) {
        return;
    }
    if (slot > 4) {
        return;
    }
    lbl_8045E730[slot] = 1;
}

// Counts the timers of every enabled HUD slot down by delta; when a timer runs out the
// entry is marked done and the following entries are shifted down, leaving a zeroed tail.
void HudTimerTick(float delta) {
    for (int slot = 0; slot < 4; slot++) {
        if (lbl_8045E730[slot] == 0) {
            float zero = lbl_804D7500;
            HudSlot* s = &lbl_8045D9F0[slot];

            for (int i = 0; i < 4; i++) {
                HudSlotEntry* e = &s->entries[i];

                if (e->timer > zero) {
                    e->timer -= delta;
                    if (e->timer <= zero) {
                        HudExpireEntry(slot, i);
                    }
                }
            }
        }
    }
}

// Clears the whole HUD slot table and the per-slot disable flags.
void HudTimerClearAll(void) {
    memset(&lbl_8045D9F0, 0, sizeof(HudSlot) * 4);
    memset(lbl_8045E730, 0, 0x10);
}

// --- Main menu ------------------------------------------------------------
// A small menu drawn with the debug text system. It is a task callback kept
// alive by re-registering itself through fn_80088700 every frame.

extern "C" {
void fn_80005A64(float dt);                                   // 0x80005A64
void fn_8003BAD0(float dt);                                   // 0x8003BAD0
void fn_80173DD0(int id);                                     // 0x80173DD0
void fn_800E2300(int arg);                                    // 0x800E2300
const char* fn_802A1FA0(void);                                // 0x802A1FA0
void fn_800886F0(void);                                       // 0x800886F0
void* fn_800886C8(void);                                      // 0x800886C8
}
void PadUpdateAll(float dt);

void* GetPlayer(void);                                        // GetPlayer__Fv   0x80049948
void SAMUpdate(float dt);                                     // SAMUpdate__Ff   0x800E073C
void TextBegin(void);                                         // TextBegin__Fv   0x801ED0C8
void TextSetSize(int size);                                   // TextSetSize__Fi 0x801ED2A0
void TextSetColor(int r, int g, int b, int a);                // TextSetColor__Fiiii 0x801ED1E4
void TextSetBox(int x, int y, int w, int h);                  // TextSetBox__Fiiii   0x801ED3FC
int PadPressed(int pad, int button, int port);                // PadPressed__Fiii 0x800DC930
int SaveIsNewBest(void);                                      // SaveIsNewBest__Fv 0x800DE1E8
void LevelLoadNames(const char* level, int* a, char* b, char* out, char* c);  // 0x80015A44

typedef int (*HudMenuUpdateFn)(const char* title, float dt);

extern "C" void fn_80088700(HudMenuUpdateFn fn, const void* arg);  // 0x80088700

int HudMenuUpdate(const char* title, float dt);

// Starts the menu with the title string stored under the given key.
void HudMenuStartByKey(u32 key) {
    fn_80088700(HudMenuUpdate, StrFindKeySkip(key));
}

// Starts the menu with the default title.
void HudMenuStartDefault(void) {
    fn_80088700(HudMenuUpdate, StrFindKeySkip(0xd6a085a8));
}

// Per-frame menu update: draws the current page and handles the pad input.
int HudMenuUpdate(const char* title, float dt) {
    static s32 sOption = 0;         // 0x804D4BE8
    static s32 sPage = 0;           // 0x804D4BF0
    static s32 sChoice = 0;         // 0x804D4BF8
    static s32 sMission = 0x5600;   // 0x804D4C00

    u8* player = (u8*)GetPlayer();

    if (player != 0 && *(s32*)(player + 0x27fc) != 0) {
        // Someone else took over the screen: drop the menu and hand the frame
        // to whatever callback is currently installed.
        fn_800886F0();
        ((void (*)(int, float))fn_800886C8())(0, dt);
        fn_80088700(HudMenuUpdate, title);
        return 1;
    }
    if (title == 0) {
        return 0;
    }

    fn_80005A64(dt);
    PadUpdateAll(dt);
    fn_8003BAD0(dt);
    SAMUpdate(dt);
    TextBegin();
    TextSetSize(0x1a);
    TextSetColor(0xff, 0xff, 0xff, 0xff);
    TextSetAlign(4);
    DebugTextPos(0x140, 0x153, 2);
    DebugText(title);

    switch (sPage) {
    case 0:
        if (sOption == 0) {
            TextSetSize(0x10);
            TextSetColor(0xff, 0xff, 0, 0xff);
            DebugTextPos(0x140, 0xb4, 2);
        } else {
            TextSetSize(0xc);
            TextSetColor(0xff, 0xff, 0xff, 0xff);
            DebugTextPos(0x140, 0xb2, 2);
        }
        DebugText(StrFindKeySkip(0x8de5da7d));
        if (sOption == 1) {
            TextSetSize(0x10);
            TextSetColor(0xff, 0xff, 0, 0xff);
            DebugTextPos(0x140, 0x98, 2);
        } else {
            TextSetSize(0xc);
            TextSetColor(0xff, 0xff, 0xff, 0xff);
            DebugTextPos(0x140, 0x96, 2);
        }
        DebugText(StrFindKeySkip(0x4276e7ca));
        if (PadPressed(0, 0, 0) == 1 && sOption == 1) {
            sOption--;
        }
        if (PadPressed(0, 0, 1) == 1 && sOption == 0) {
            sOption++;
        }
        if (PadPressed(0, 0, 3) == 1) {
            sPage = 1;
            sChoice = 0;
        }
        break;

    case 1:
        TextSetSize(0x10);
        TextSetColor(0xff, 0xff, 0xff, 0xff);
        TextSetBox(0, 0x280, 0, 0x82);
        DebugText(StrFindKeySkip(SaveIsNewBest() ? 0xa6a8832c : 0x1f94e304));
        TextSetBox(0xf0, 0x140, 0, 0x48);
        if (sChoice == 0) {
            TextSetColor(0xff, 0xff, 0, 0xff);
        } else {
            TextSetColor(0x80, 0x80, 0x80, 0xff);
        }
        DebugText(StrFindKeySkip(0x9268a6af));
        TextSetBox(0x140, 0x190, 0, 0x48);
        if (sChoice == 1) {
            TextSetColor(0xff, 0xff, 0, 0xff);
        } else {
            TextSetColor(0x80, 0x80, 0x80, 0xff);
        }
        DebugText(StrFindKeySkip(0xbaac5799));
        if (PadPressed(0, 0, 5) == 1 || PadPressed(0, 0, 6) == 1) {
            sChoice ^= 1;
        }
        if (PadPressed(0, 0, 3) == 1) {
            if (sChoice == 1) {
                sPage = 3;
                sMission = 0x5600;
            } else {
                sPage = 2;
                sChoice = 0;
            }
        }
        break;

    case 2:
        TextSetSize(0x10);
        TextSetColor(0xff, 0xff, 0xff, 0xff);
        TextSetBox(0, 0x280, 0, 0x82);
        if (sOption == 0) {
            DebugText(StrFindKeySkip(0x9ce42c96));
        } else {
            DebugText(StrFindKeySkip(0xe7d3a1e6));
        }
        TextSetBox(0xf0, 0x140, 0, 0x48);
        if (sChoice == 0) {
            TextSetColor(0xff, 0xff, 0, 0xff);
        } else {
            TextSetColor(0x80, 0x80, 0x80, 0xff);
        }
        DebugText(StrFindKeySkip(0x9268a6af));
        TextSetBox(0x140, 0x190, 0, 0x48);
        if (sChoice == 1) {
            TextSetColor(0xff, 0xff, 0, 0xff);
        } else {
            TextSetColor(0x80, 0x80, 0x80, 0xff);
        }
        DebugText(StrFindKeySkip(0xbaac5799));
        if (PadPressed(0, 0, 5) == 1 || PadPressed(0, 0, 6) == 1) {
            sChoice ^= 1;
        }
        if (PadPressed(0, 0, 3) == 1) {
            if (sChoice == 1) {
                sPage = 3;
                sMission = 0x5601;
            } else {
                sPage = 0;
                sChoice = 0;
            }
        }
        break;

    case 3:
        {
            char levelName[0x100];
            struct MenuMsg {
                int kind;
                const char* text;
            } msg;

            if (sOption == 0) {
                LevelLoadNames(fn_802A1FA0(), 0, 0, levelName, 0);
            } else {
                sprintf(levelName, "MainMenu");
            }
            sPage = 0;
            sOption = 0;
            sChoice = 0;
            fn_80173DD0(0);
            fn_80173DD0(1);
            fn_80173DD0(2);
            fn_80173DD0(4);
            fn_80173DD0(6);
            fn_80173DD0(7);
            fn_80173DD0(8);
            fn_80173DD0(9);
            fn_80173DD0(0xb);
            fn_80173DD0(0xc);
            fn_80173DD0(0xd);
            fn_80173DD0(0xe);
            fn_80173DD0(0xf);
            fn_80173DD0(0x10);
            fn_80173DD0(0x11);
            fn_800E2300(0);
            msg.kind = 0;
            msg.text = levelName;
            SendMsg(0x38000000, sMission, &msg);
            fn_800886F0();
        }
        break;

    default:
        break;
    }
    return 1;
}

// Message hook of the notification settings block: answers the 0x41E queries with the
// stored notification values (or 0 when the value is disabled), and hands the 0x41D
// set messages to the notification setter.
int HudNotifyMsg(void* state, int msg, void* data) {
    HudMsg* m = (HudMsg*)data;

    switch (msg) {
    case 0x41D:
        return HudNotifySet(state, data);
    case 0x41E:
        switch (m->cmd) {
        case 0x7D8C4C13:
            if ((lbl_8045E740.flags & 1) == 0) {
                m->value = (int)lbl_8045E740.f00;
            } else {
                m->value = 0;
            }
            return 1;
        case 0x4BA99765:
            if ((lbl_8045E740.flags & 2) == 0) {
                m->value = (int)lbl_8045E740.f04;
            } else {
                m->value = 0;
            }
            return 1;
        case 0x4BA99766:
            if ((lbl_8045E740.flags & 4) == 0) {
                m->value = (int)lbl_8045E740.f08;
            } else {
                m->value = 0;
            }
            return 1;
        case 0xC6D2473B:
            m->value = ((HudState*)state)->field04;
            return 1;
        }
        return 0;
    }
    return 0;
}


// Hashes a key string onto an existing hash value: characters are mapped through the
// game's case table (0xFF maps to 0xFFFFFFFF) and the result is reduced mod 0xFFFFFFFF.
static inline u32 HudHashKey(u32 h, const char* s) {
    int i;

    if (s == 0) {
        return 0;
    }
    for (i = 0; s[i] != 0; i++) {
        signed char ch = s[i];
        u32 c;

        if (ch == -1) {
            c = 0xFFFFFFFF;
        } else {
            c = __upper_map[(u8)ch];
        }
        h *= 31;
        h += c & 0xFF;
    }
    return h % 0xFFFFFFFF;
}

void HudNotifyDraw(int mode);   // 0x8010015C
void HudNotifyBar(float dt);    // 0x80100BC4

// Asks the HUD manager for the two notification strings; when either exists, restarts the
// countdowns of the three notification kinds that are not disabled.
void HudNotifyCheck(int owner, int arg) {
    int buf[8];
    int found0;
    int found1;
    int target;

    if (arg != 0) {
        return;
    }
    target = fn_800FD6EC(arg);
    {
        QueryMsg q;

        q.hash = 0x640D1F4D;
        *(int*)&q.result = 0;
        q.unk8 = 0;
        SendMsg(target, 0x41E, &q);
        found0 = *(int*)&q.result;
    }
    target = fn_800FD6EC(arg);
    {
        QueryMsg q;

        q.hash = 0x2B9216E5;
        *(int*)&q.result = 0;
        q.unk8 = 0;
        SendMsg(target, 0x41E, &q);
        found1 = *(int*)&q.result;
    }
    if (found0 != 0 || found1 != 0) {
        float zero = lbl_804D7528;
        float* pf = &lbl_8045E740.f00;
        int i = 0;

        do {
            if (*pf > zero && (lbl_8045E740.flags & (1 << i)) == 0) {
                fn_801403D0(arg, i);
            }
            i++;
            pf++;
        } while (i < 3);
        SendMsg(*(int*)(owner + 0x24 + arg * 4), 0x400, buf);
        HudNotifyDraw(PlayerInHost((void*)buf[0]));
    }
}

// Draws the notification bar for the current notification owner: picks its position and
// colour from the owner's state and draws the label plus the remaining time.
void HudNotifyDraw(int mode) {
    if (lbl_8045E740.handle != 0) {
        QueryMsg q;
        int buf[8];
        int pos[4];
        u32 handle = lbl_8045E740.handle;
        float a;
        float b;
        float c;
        int found = 0;
        int arg = 0;

        fn_8016359C(mode);
        a = lbl_804D7528;
        if (handle >> 24 == 0x13) {
            int actor = ActorGet(0);
            int sub;

            SendMsg(handle, 0x400, buf);
            sub = *(int*)(buf[0] + 0x5EC);
            if (sub != 0 && *(int*)(sub + 0xBC) == *(int*)(actor + 0x38)) {
                a = *(float*)(sub + 8);
                b = *(float*)(sub + 0x0C);
                c = lbl_8045E740.f14;
                found = 1;
            }
        }
        if (!found) {
            SendMsg(handle, 0x401, &q);
            a = *(float*)&q;
            b = lbl_804D7528;
            c = lbl_8045E740.f14;
        }
        if (a < lbl_804D7528) {
            a = lbl_804D7528;
        }
        if (c < lbl_804D7528) {
            c = lbl_804D7528;
        }
        if (b != lbl_804D7528) {
            if (mode == 0) {
                fn_8013EB40(0xA054D63E, pos);
            } else {
                fn_8013EB40(0x413843F7, pos);
            }
            fn_801635F4(a, c, b, pos[0], pos[1], pos[2], pos[3], 0, 0xFF, 0, 0xFF, 0xFF, 0x36,
                         0xC0, 0x28, 0x28, 0xFF);
            if (mode == 0) {
                fn_8013EB40(0x0CF4C32D, pos);
            } else {
                fn_8013EB40(0xBDDE0114, pos);
            }
            TextBegin();
            TextSetSize(0x0E);
            DebugTextPos(pos[0] - pos[2] / 2, pos[1] + pos[3] / 2 + 7, 2);
            TextSetColor(0xFF, 0xFF, 0, 0xFF);
            TextSetAlign(1);
            TextSetFlags(2, 0);
            DebugTextSetKey(StrFindKey(lbl_8045E740.f20, (u32*)&arg));
        }
        {
            QueryMsg s;

            s.hash = 0x6B22F882;
            *(int*)&s.result = 0;
            s.unk8 = 0;
            SendMsg(handle, 0x41E, &s);
            a = (float)(u32)s.result.name.mData;
            s.hash = 0xB81E9CA6;
            *(int*)&s.result = 0;
            s.unk8 = 0;
            SendMsg(handle, 0x41E, &s);
            b = (float)(u32)s.result.name.mData;
        }
        if (a != lbl_804D7528) {
            float w = lbl_8045E740.f18;
            float h = b;

            if (lbl_8045E740.f1C == 0) {
                fn_801635F4(a, w, h, 0x140, 0x1A6, 0x126, 7, 0xFF, 0x80, 0, 0x80, 0x40, 0, 0x40,
                             0x20, 0, 0xFF);
            } else {
                float t;

                fn_802A1DA4();
                t = lbl_804D752C * fn_800A1394();
                fn_802A1DA4();
                fn_801635F4(a, w, h, 0x140, 0x1A6, 0x126, 7, (int)t + 0xC0 & 0xFF,
                             (int)(lbl_804D7530 * fn_800A1394()) + 0x60 & 0xFF, 0, 0x80, 0x40, 0,
                             0x40, 0x20, 0, 0xFF);
            }
            TextBegin();
            TextSetSize(10);
            DebugTextPos(0xA9, 0x1AE, 1);
            TextSetColor(0xFF, 0xFF, 0, 0xFF);
            TextSetAlign(1);
            DebugText("");
        }
    }
}

// Counts the three notification countdowns down; a countdown that reaches zero is cleared
// and its "finished" animation is played on the notification owner.
void HudNotifyTick(float dt) {
    float zero = lbl_804D7528;
    float* pf = &lbl_8045E740.f00;
    int i = 0;

    do {
        if (*pf > zero) {
            *pf -= dt * fn_802A2080();
            if (*pf <= lbl_804D7528) {
                u32 h;

                *pf = lbl_804D7528;
                fn_801408A8(0, i);
                h = HudHashKey(0, "onTimer1");
                h += i;
                h = HudHashKey(h, "Finished");
                ActorPlayAnim(0x02000000, h, 0x4B000000);
            }
        }
        i++;
        pf++;
    } while (i < 3);
    if (lbl_8045E740.f20 != 0) {
        HudNotifyBar(dt);
    }
}

// Small wrappers: reset the notification block's helper (0x80140834) and clear it.
void fn_8010080C(void) {
    fn_80140834(0);
}

void fn_80100830(void) {
}

void fn_80100834(void) {
    memset(&lbl_8045E740, 0, 0x24);
}

// Applies a notification setting command: the three countdown values, the "value disabled"
// bits, the notification owner handle and the menu start requests.
int HudNotifySet(void* state, void* data) {
    HudNotify* n = &lbl_8045E740;
    HudMsg* m = (HudMsg*)data;

    switch (m->cmd) {
    case 0xC6D2473B:
        ((HudState*)state)->field04 = m->value;
        return 1;
    case 0xA3D6988C:
        n->flags |= 4;
        return 1;
    case 0xA3335EB9:
        n->flags |= 1;
        return 1;
    case 0xB273F193: {
        u32 v = (u32)m->value;

        n->handle = v;
        n->f20 = 0x9AD44CDB;
        if (v == 0) {
            return 1;
        }
        {
            QueryMsg q;

            if (SendMsg((int)v, 0x401, &q) != 0) {
                n->f14 = *(float*)&q;
            }
        }
        {
            QueryMsg q;

            q.hash = 0x6B22F882;
            *(int*)&q.result = 0;
            q.unk8 = 0;
            if (SendMsg((int)v, 0x41E, &q) != 0) {
                n->f18 = (float)(u32)q.result.name.mData;
            }
        }
        return 1;
    }
    case 0xC53F2FC6:
        if (m->value == 0 || m->value == 1) {
            fn_80088700(HudMenuUpdate, StrFindKeySkip(0xD6A085A8));
        } else {
            fn_80088700(HudMenuUpdate, StrFindKeySkip(0));
        }
        return 1;
    case 0x4BA99766:
        n->f08 = (float)(u32)m->value;
        n->flags &= ~4;
        return 1;
    case 0x7D8C4C13:
        n->f00 = (float)(u32)m->value;
        n->flags &= ~1;
        return 1;
    case 0x5BC38C5A:
        if (m->value != 0) {
            n->f20 = m->value;
            return 1;
        }
        n->f20 = 0x9AD44CDB;
        return 1;
    case 0xE00D8842:
        n->f04 = (float)(u32)m->value;
        n->flags &= ~2;
        return 1;
    }
    return 1;
}

// Smoothly moves the notification position and length towards the values the HUD manager
// reports, and latches the "expanding" flag used by the draw code.
void HudNotifyBar(float dt) {
    int buf[8];
    int found = 0;
    QueryMsg q;
    float f0 = lbl_804D7528;

    if (lbl_8045E740.handle >> 24 == 0x13) {
        int actor = ActorGet(0);
        int sub;

        SendMsg(lbl_8045E740.handle, 0x400, buf);
        sub = *(int*)(buf[0] + 0x5EC);
        if (sub != 0 && *(int*)(sub + 0xBC) == *(int*)(actor + 0x38)) {
            float target = *(float*)(sub + 8) - lbl_8045E740.f14;
            float step = f0;

            if (f0 != target) {
                float mag = target < f0 ? -target : target;

                step = *(float*)(sub + 0x0C) / lbl_804D7540 * dt;
                if (mag < step) {
                    step = mag;
                }
                if (f0 != mag) {
                    step = step * (target / mag);
                }
            }
            found = 1;
            lbl_8045E740.f14 = lbl_8045E740.f14 + step;
        }
    }
    if (!found) {
        if (SendMsg(lbl_8045E740.handle, 0x401, &q) != 0) {
            float target = *(float*)&q - lbl_8045E740.f14;
            float step = f0;

            if (f0 != target) {
                float mag = target < f0 ? -target : target;

                step = (*(float*)((char*)&q + 4) / lbl_804D7540) * dt;
                if (mag < step) {
                    step = mag;
                }
                if (f0 != mag) {
                    step = step * (target / mag);
                }
            }
            lbl_8045E740.f14 = lbl_8045E740.f14 + step;
        }
    }
    {
        float v0;
        float v1;

        q.hash = 0x6B22F882;
        *(int*)&q.result = 0;
        q.unk8 = 0;
        SendMsg(lbl_8045E740.handle, 0x41E, &q);
        v0 = (float)(u32)q.result.name.mData;
        q.hash = 0xB81E9CA6;
        *(int*)&q.result = 0;
        q.unk8 = 0;
        SendMsg(lbl_8045E740.handle, 0x41E, &q);
        v1 = (float)(u32)q.result.name.mData;
        if (v1 != f0) {
            float old = lbl_8045E740.f18;
            float target = v0 - old;
            float step = f0;

            if (f0 != target) {
                float mag = target < f0 ? -target : target;

                step = v1 / lbl_804D7540 * dt;
                if (mag < step) {
                    step = mag;
                }
                if (f0 != mag) {
                    step = step * (target / mag);
                }
            }
            lbl_8045E740.f18 = old + step;
            if (old < v0 || v0 == f0) {
                lbl_8045E740.f1C = 1;
            } else {
                lbl_8045E740.f1C = 0;
            }
        }
    }
}

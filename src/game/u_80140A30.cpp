// u_80140A30.cpp: the "Hunt Bot" (FearHuntEvent) HUD screen.
// Split out of the 0x8013xxxx game code; the screen descriptor sits in .data next to the
// fear-action table. Names are descriptive guesses; code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

extern "C" {
int SendMsg(int target, int msg, void* data);                 // 0x800265D4
void* memset(void* dst, int val, unsigned long n);
}

int PlayerTeam(int player);                                   // 0x800FD68C
int PlayerGetPossessor(int player);                           // 0x800FD6EC? (see uses)
int HandleToPlayer(int handle);                               // 0x800FD720
void* OwnerGetInfo(u32 owner);                                // 0x8002394C
void* ActorGet(int id);                                       // 0x802A2024
int GameEvent(int a, int b);                                  // 0x8007EA84
void GhostNotify(int a, int b, int c);                        // 0x80154594
const char* StrFindKeySkip(u32 key);                          // 0x80086364
void DebugText(const char* format, ...);                      // 0x801EB188
void DebugTextPos(int x, int y, int flag);                    // 0x8007E570
int TextSizeScaled(int size);                                 // 0x8007E1E8
void TextBegin(void);                                         // 0x801ED0C8
void TextSetMode(int mode);                                   // 0x801ED1A0
void TextSetSize(int size);                                   // 0x801ED2A0
void TextSetColor(int r, int g, int b, int a);                // 0x801ED1E4
void TextSetBox(int x0, int x1, int y0, int y1);              // 0x801ED3FC
void Fn80140778(void);                                        // 0x80140778

void fn_800872BC(int level, const char* format, ...); // 0x800872BC
void fn_800A2DE8(int a);                                      // 0x800A2DE8
void* fn_800A3990(u32 handle);                                // 0x800A3990
void fn_800A31A0(int a);                                      // 0x800A31A0
int fn_800B949C(u32 hash, void* out, int count, int flag);    // 0x800B949C
void fn_800B969C(void);                                       // 0x800B969C
void fn_800F8ED0(void);                                       // 0x800F8ED0
void fn_800FC764(int a, int b);                               // 0x800FC764
int fn_800FD3EC(void);                                        // 0x800FD3EC
void* fn_800FD424(void);                                      // 0x800FD424
u32 fn_800FD6EC(int i);                                       // 0x800FD6EC
void fn_800FD424_alt(void);                                   // unused placeholder
void fn_800FF4E4(int a);                                      // 0x800FF4E4
void fn_80101F00(int a);                                      // 0x80101F00
void fn_8010FE38(int a);                                      // 0x8010FE38
void fn_8010FE64(int a);                                      // 0x8010FE64
void fn_80116E30(int a);                                      // 0x80116E30
float fn_80120C14(u32 hash, int a, int b);                    // 0x80120C14
int fn_80120C80(u32 hash, int a, int b);                      // 0x80120C80
void fn_80120D28(int a);                                      // 0x80120D28
void fn_80120D60(int a);                                      // 0x80120D60
void fn_8013E998(u32 hash, void* out);                        // 0x8013E998
void fn_8013EB40(u32 hash, void* out);                        // 0x8013EB40
int fn_8013ECD0(int a, void* data);                           // 0x8013ECD0
void fn_8013EFB8(void);                                       // 0x8013EFB8
int fn_8013FCF0(int a, int b, int c);                         // 0x8013FCF0
void fn_801403D0(int a, int b, float c);                      // 0x801403D0
void fn_801408C8(void);                                       // 0x801408C8
void fn_80140918(void);                                       // 0x80140918
void fn_8016D6A8(int a);                                      // 0x8016D6A8

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

struct Vec3 {
    float x, y, z;
};

struct HudPos {
    float x, y, z;
    int size;
};

struct HudAnchor {
    float x, y, z;
    int size;
    u8 color[4];
};

// Payload for message 0xC14 (fear-hunt events).
struct C14Msg {
    int unk0;
    int index;
    u32 handle;
    u32 key;
};

struct HuntState {
    int state;          // 0x00
    float unk4;         // 0x04
    float timer;        // 0x08
    int reg[8];         // 0x0C
    float scores[8];    // 0x2C
    int slot;           // 0x4C
};

struct HuntConf {
    int unk0;           // 0x00
    int unk4;           // 0x04
    int enabled;        // 0x08
    int startTime;      // 0x0C
    int limit;          // 0x10
};

struct HuntScreen {
    char pad0[8];
    int mode;           // 0x08
    u32 flags;          // 0x0C
    const char* title;  // 0x10
    int minutes;        // 0x14
    char pad18[4];
    int count;          // 0x1C
    char pad20[4];
    u32 handles[8];     // 0x24
    char pad44[0xA4];
    int unkE8;          // 0xE8
    char padEC[0x3C];
    u32 teamMask;       // 0x128
};

extern HuntState lbl_8046D5B8;     // 0x8046D5B8
extern HuntConf lbl_803A4028;      // 0x803A4028

// One entry of the fear-action table (0x803A41C0).
struct HuntEntry {
    const char* name;
    int unk4;
    int score;
    float unkC;
    float unk10;
    int flag;
};

static HuntEntry sEntries[9] = {
    { "POSSESS", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "GHOSTACTION", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "SLIP", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "HIJACK", 0, 1000, 1600.0f, 3600.0f, 0 },
    { "HOSTACTION", 0, 1000, 1600.0f, 4033.0f, 1 },
    { "EXCHANGE", 0, 1000, 1600.0f, 3600.0f, 1 },
    { "H_HIJACK", 0, 1000, 1600.0f, 3600.0f, 1 },
    { "TALK", 0, 1000, 1120.0f, 3600.0f, 1 },
    { "SCAN", 0, 1000, 1120.0f, 3600.0f, 1 },
};

struct HuntScreenType {
    const char* name;
    void (*init)(void);
    void (*enter)(void*);
    void (*exit)(void*);
    void (*update)(void*, float);
    void (*draw)(void*, int);
    int (*msg)(void*, int, int*);
    void (*reset)(void*);
};

int HuntBotMsg(void* owner, int msg, int* data);
void HuntBotDraw(void* screen, int pass);
void HuntBotUpdate(void* screen, float dt);
void HuntBotExit(void* screen);
void HuntBotEnter(void* screen);
void HuntBotInit(void);
void HuntBotReset(void* screen);
int HuntBotPick(void* obj, int* out, float radius);
void HuntBotSetFlag(int i);
u32 HuntBotPickable(u32 handle);
int HuntBotScore(void* screen, u32 handle, u32 index);
int HuntBotHit(void* obj, float* p);

static HuntScreenType sHuntBotScreen = {
    "Hunt Bot", HuntBotInit, HuntBotEnter, HuntBotExit, HuntBotUpdate, HuntBotDraw, HuntBotMsg, HuntBotReset,
};

// .data flags, in address order.
static int sFlagA[8];        // 0x803A42B8
static int sFlagB[8];        // 0x803A42D8
static int sFlagC[16];       // 0x803A42F8

// .bss
static int sShownScore[8];   // 0x8046D608
static u32 sSlots[16];       // 0x8046D628

// .sbss, in address order.
int lbl_804D4D48;            // active
float lbl_804D4D4C;          // accumulated time
int lbl_804D4D50;            // mask
int lbl_804D4D54;            // mask 2
int lbl_804D4D58;            // mask 3
int lbl_804D4D5C;
int lbl_804D4D60;
int lbl_804D4D64;
int lbl_804D4D68;
int lbl_804D4D6C;
int lbl_804D4D70;
int lbl_804D4D74;

static inline void SendQuery(int target, u32 hash, int value) {
    QueryMsg q;
    q.hash = hash;
    *(int*)&q.result = value;
    q.unk8 = 0;
    SendMsg(target, 0x41D, &q);
}

static inline void QueryValue(int target, u32 hash, int* out) {
    QueryMsg q;
    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    SendMsg(target, 0x41E, &q);
    *out = *(int*)&q.result;
}

int HuntBotMsg(void* owner, int msg, int* data) {
    switch (msg) {
    case 0xC14: {
        u32 handle;
        int index;
        index = data[1];
        handle = data[2];
        switch (data[3]) {
        case 0x7C3F4E3E:
            fn_800872BC(0x40000, "FearHuntEvent( SCOREPOINT, 0x%08X, 0x%08X )", handle, index);
            return HuntBotScore(owner, handle, index);
        case 0x6C345908: {
            int player = HandleToPlayer(handle);
            PlayerTeam(player);
            if (player < 0 || player > 7) {
                return 1;
            }
            if (lbl_804D4D50 & (1 << player)) {
                lbl_804D4D50 &= ~(1 << player);
            }
            return 1;
        }
        case 0x901EEFA2:
            fn_800872BC(0x40000, "FearHuntEvent( REGISTEROBJECT, 0x%08X, 0x%08X )", handle, index);
            return 0;
        case 0x0020A893:
            return lbl_8046D5B8.reg[index] = 1;
        }
        return 0;
    }
    case 0x41E:
        return fn_8013ECD0(1, data);
    case 0x41D:
        return fn_8013ECD0(0, data);
    }
    return 0;
}

void HuntBotDraw(void* screenp, int pass) {
    HuntScreen* screen = (HuntScreen*)screenp;

    if (!lbl_804D4D48) {
        return;
    }

    if (pass == -1) {
        switch (lbl_8046D5B8.state) {
        case 0:
            if (!lbl_804D4D64) {
                int i;
                lbl_804D4D64 = 1;
                for (i = 0; i < screen->count; i++) {
                    u32 h = fn_800FD6EC(i);
                    QueryMsg q;
                    q.hash = 0xDE3EAAD7;
                    *(int*)&q.result = 0;
                    q.unk8 = 0;
                    SendMsg(h, 0x41D, &q);
                }
            }
            TextBegin();
            TextSetSize(0x14);
            TextSetColor(0xFF, 0xFF, 0xFF, 0xFF);
            TextSetMode(2);
            TextSetBox(0, 0x280, 0, 0x150);
            DebugText(StrFindKeySkip(0xCE12257A));
            break;
        case 1:
            if (!lbl_804D4D6C) {
                if (lbl_804D4D64 == 1) {
                    lbl_804D4D64 = 0;
                }
                lbl_804D4D6C = 1;
            }
            if (!lbl_804D4D68) {
                lbl_804D4D68 = 1;
            }
            TextBegin();
            TextSetSize(0x14);
            TextSetColor(0xFF, 0xFF, 0xFF, (u8)(255.0f * (lbl_8046D5B8.timer / 1200.0f)));
            TextSetMode(2);
            TextSetBox(0, 0x280, 0, 0x150);
            DebugText(StrFindKeySkip(0xCE12257A));
            break;
        case 2:
            if (!lbl_804D4D70) {
                if (lbl_804D4D68 == 1) {
                    int i;
                    lbl_804D4D68 = 0;
                    for (i = 0; i < screen->count; i++) {
                        u32 h = fn_800FD6EC(i);
                        QueryMsg q;
                        q.hash = 0xDE3EAAD7;
                        *(int*)&q.result = 1;
                        q.unk8 = 0;
                        SendMsg(h, 0x41D, &q);
                    }
                }
                lbl_804D4D70 = 1;
            }
            if (lbl_803A4028.enabled != 0) {
                fn_801403D0(0, 0, lbl_8046D5B8.unk4);
            }
            break;
        case 3:
            if (lbl_804D4D5C == 1) {
                lbl_804D4D5C = 0;
            }
            lbl_804D4D6C = 0;
            lbl_804D4D70 = 0;
            break;
        }
        {
            int i;
            for (i = 0; i < screen->count; i++) {
                u32 h = fn_800FD6EC(i);
                if ((int)(h >> 24) == 2) {
                    int team = PlayerTeam(i);
                    void* pos = fn_800FD424();
                    int score = (int)fn_80120C14(0xE2D6A9C0, 1, team);
                    Vec3 color;
                    switch (i) {
                    case 0:
                        fn_8013E998(0x4DA3732E, &color);
                        break;
                    case 1:
                        fn_8013E998(0xB584A00D, &color);
                        break;
                    case 2:
                        fn_8013E998(0x1D65CCEC, &color);
                        break;
                    case 3:
                        fn_8013E998(0x8546F9CB, &color);
                        break;
                    }
                    if (sFlagC[0] == 0) {
                        sShownScore[0] = score;
                        sFlagC[0] = 1;
                    }
                    TextBegin();
                    TextSetColor((u8)(255.0f * ((float*)pos)[0]), (u8)(255.0f * ((float*)pos)[1]),
                                 (u8)(255.0f * ((float*)pos)[2]), 0xFF);
                    TextSizeScaled(((HudPos*)&color)->size);
                    DebugTextPos((int)((float*)pos)[0], (int)((float*)pos)[1], 2);
                    DebugText(" %02d", score);
                    if (sShownScore[0] != score) {
                        sShownScore[0] = score;
                    }
                    if (sFlagB[0] == 0) {
                        sFlagB[0] = 1;
                    }
                }
            }
        }
    } else {
        int r;
        u32 h = fn_800FD6EC(pass);
        QueryMsg q;
        q.hash = 0x640D1F4D;
        *(int*)&q.result = 0;
        q.unk8 = 0;
        SendMsg(h, 0x41E, &q);
        r = *(int*)&q.result;
        if (r != 0) {
            int team = PlayerTeam(pass);
            if (lbl_8046D5B8.scores[team] > 0.0f) {
                int limit = lbl_803A4028.limit;
                int total = (int)(1000.0f * (float)limit - lbl_8046D5B8.scores[team]);
                int minutes = total / 60;
                int sec = total / 1000;
                int min = sec / 60;
                int cs = total / 10;
                int secRem = sec - min * 60;
                int frac = cs - (cs / 100) * 100;
                void* pos = fn_800FD424();
                HudAnchor anchor;
                if (!lbl_804D4D60) {
                    fn_8013EB40(0x1A94D158, &anchor);
                    lbl_804D4D60 = 1;
                }
                TextBegin();
                TextSetColor((u8)(255.0f * ((float*)pos)[0]), (u8)(255.0f * ((float*)pos)[1]),
                             (u8)(255.0f * ((float*)pos)[2]), anchor.color[3]);
                TextSizeScaled(anchor.size);
                DebugTextPos((int)((float*)pos)[0], (int)((float*)pos)[1], 0);
                if (minutes > 0) {
                    DebugText("%d:%02d", minutes, secRem);
                } else {
                    DebugText("%d.%02d", secRem, frac);
                }
            }
        }
    }
}

void HuntBotUpdate(void* screenp, float dt) {
    HuntScreen* screen = (HuntScreen*)screenp;

    if (!lbl_804D4D48) {
        return;
    }

    lbl_804D4D4C += dt;
    switch (lbl_8046D5B8.state) {
    case 0: {
        float t = lbl_8046D5B8.timer - dt;
        lbl_8046D5B8.timer = t;
        if (t <= 0.0f) {
            lbl_8046D5B8.state = 1;
            lbl_8046D5B8.timer = 1200.0f;
        }
        break;
    }
    case 1: {
        float t = lbl_8046D5B8.timer - dt;
        lbl_8046D5B8.timer = t;
        if (t <= 0.0f) {
            int i;
            lbl_8046D5B8.timer = 0.0f;
            lbl_8046D5B8.state = 2;
            for (i = 0; i < screen->count; i++) {
                u32 h = screen->handles[i];
                if ((int)(h >> 24) == 2) {
                    struct {
                        int unk0;
                        int unk4;
                        int unk8;
                    } m;
                    m.unk4 = 0;
                    m.unk8 = 0x01000002;
                    SendMsg(h, 0x40D, &m);
                } else if ((int)(h >> 24) == 0x13) {
                    struct {
                        int unk0;
                        int unk4;
                    } m;
                    fn_800A3990(h);
                    m.unk4 = 1;
                    SendMsg(h, 0xC00, &m);
                }
            }
        }
        break;
    }
    case 2: {
        int i;
        int redraw = 0;
        int teams;
        u32 seen;
        int hosts;
        float threshold;
        for (i = 0; i < screen->count; i++) {
            u32 h = screen->handles[i];
            if ((int)(h >> 24) == 2 && *((int*)OwnerGetInfo(h) + 0x358 / 4) == 1 && h != 0xFFFFFFFF) {
                C14Msg m;
                m.index = 1;
                m.handle = 0x4B000000;
                m.key = 0xF8639C97;
                SendMsg(h, 0xC14, &m);
            }
        }
        if (lbl_803A4028.enabled != 0) {
            float t = lbl_8046D5B8.unk4 - dt;
            lbl_8046D5B8.unk4 = t;
            if (t <= 0.0f) {
                lbl_8046D5B8.unk4 = 0.0f;
                redraw = 1;
            }
        }
        teams = 0;
        seen = 0;
        hosts = 0;
        for (i = 0; i < screen->count; i++) {
            int team = PlayerTeam(i);
            if ((int)fn_80120C14(0xE2D6A9C0, 1, team) > 0) {
                if (screen->unkE8 != 0) {
                    seen |= 1;
                } else {
                    seen |= 2;
                }
                if ((int)(fn_800FD6EC(i) >> 24) == 2) {
                    hosts++;
                }
                {
                    u32 bit = 4 << PlayerTeam(HandleToPlayer(screen->handles[i]));
                    if ((seen & bit) == 0) {
                        seen |= bit;
                        teams++;
                    }
                }
            }
        }
        if (teams < 2) {
            redraw = 1;
        }
        if (hosts < 1) {
            redraw = 1;
        }
        if ((seen & 1) == 0 || (seen & 2) == 0) {
            redraw = 1;
        }
        threshold = 1000.0f * (float)lbl_803A4028.limit;
        for (i = 0; i < 8; i++) {
            if (screen->teamMask & (4 << i)) {
                if (lbl_8046D5B8.reg[i] != 0) {
                    lbl_8046D5B8.reg[i] = 0;
                    lbl_8046D5B8.scores[i] += dt;
                    if (lbl_8046D5B8.scores[i] >= threshold) {
                        lbl_8046D5B8.slot = i;
                        redraw = 1;
                    }
                } else {
                    lbl_8046D5B8.scores[i] = 0.0f;
                }
            }
        }
        if (redraw == 1) {
            for (i = 0; i < screen->count; i++) {
                u32 h;
                QueryMsg q;
                fn_80101F00(i);
                h = screen->handles[i];
                q.hash = 0x640D1F4D;
                *(int*)&q.result = 0;
                q.unk8 = 0;
                SendMsg(h, 0x41D, &q);
            }
            fn_800FF4E4(-1);
            lbl_8046D5B8.state = 3;
            lbl_8046D5B8.timer = 5000.0f;
            {
                C14Msg m;
                m.index = 1;
                m.handle = 0x4B000000;
                m.key = 0xF8639C97;
                SendMsg(0x20000000, 0xC14, &m);
            }
        }
        break;
    }
    case 3:
        HuntBotReset(screen);
        break;
    default:
        lbl_8046D5B8.state = 2;
        break;
    }
}

void HuntBotExit(void* screenp) {
    HuntScreen* screen = (HuntScreen*)screenp;
    int i;

    lbl_804D4D48 = 0;
    for (i = 0; i < screen->count; i++) {
        u32 h = screen->handles[i];
        int r;
        QueryValue(h, 0x00E321FA, &r);
        if (r != -1) {
            int buf[11];
            SendMsg(r, 0x400, buf);
            fn_800A31A0(buf[0]);
            SendQuery(h, 0x00E321FA, -1);
        }
    }
    for (i = 0; i < 16; i++) {
        sFlagC[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        sFlagB[i] = 0;
    }
    lbl_804D4D64 = 0;
    lbl_804D4D68 = 0;
    lbl_804D4D5C = 0;
    lbl_804D4D60 = 0;
    Fn80140778();
    fn_801408C8();
}

void HuntBotEnter(void* screenp) {
    HuntScreen* screen = (HuntScreen*)screenp;
    int i;

    if (!lbl_804D4D48) {
        return;
    }

    {
        QueryMsg q;
        q.hash = 0x31809F6F;
        *(int*)&q.result = 0xC00D5BD1;
        q.unk8 = 2;
        SendMsg(0x4B000000, 0x41D, &q);
    }
    SendQuery(0x4B000000, 0xA8F9C0EE, 0x12C);
    SendQuery(0x4B000000, 0x7C09D03F, 7);
    SendQuery(0x4B000000, 0x3C682FD8, 7);
    SendQuery(0x4B000000, 0xBECAF63A, 0xF);

    if (screen->mode != 0) {
        String s;
        if (screen->minutes == 0) {
            lbl_803A4028.enabled = 0;
        } else {
            lbl_803A4028.enabled = 1;
            lbl_803A4028.startTime = screen->minutes * 60;
        }
        {
            QueryMsg q;
            q.hash = 0x7C09D03F;
            *(int*)&q.result = (int)screen->title;
            fn_8013ECD0(0, &q);
            q.hash = 0x3C682FD8;
            fn_8013ECD0(0, &q);
        }
        for (i = 0; i < 0x15; i++) {
            switch (i) {
            case 2: {
                QueryMsg q;
                q.hash = 0x0E83B88B;
                *(int*)&q.result = (screen->flags >> 2) & 1;
                q.unk8 = 0;
                SendMsg(0x4B000000, 0x41D, &q);
                break;
            }
            }
        }
        (void)s;
    }

    fn_8013EFB8();
    fn_80140918();

    {
        int a;
        int b;
        fn_800B949C(0x0613B26A, &a, 1, 1);
        fn_800B949C(0xBAA51EE3, &b, 1, 1);
        for (i = 0; i < screen->count; i++) {
            u32 h = screen->handles[i];
            void* p;
            SendQuery(h, 0x00E321FA, -1);
            if ((int)(h >> 24) == 0x13) {
                int obj = (int)fn_800A3990(h);
                p = (screen->unkE8 == 1) ? (void*)&b : (void*)&a;
                *(int*)(*(int*)(obj + 0xC8) + 0xC) = GameEvent(-1, (int)p);
            } else if ((int)(h >> 24) == 2) {
                int buf[8];
                SendMsg(h, 0x400, buf);
                p = (screen->unkE8 == 1) ? (void*)&b : (void*)&a;
                SendQuery(h, 0x7C18FE9E, (int)p);
            }
        }
    }
}

void HuntBotInit(void) {
    lbl_804D4D48 = 1;
    memset(&lbl_8046D5B8, 0, 0x50);
    lbl_8046D5B8.unk4 = 1000.0f * (float)lbl_803A4028.startTime;
    sFlagA[0] = 0;
    lbl_8046D5B8.state = 0;
    lbl_8046D5B8.timer = 3000.0f;
    sFlagA[1] = 0;
    sFlagA[2] = 0;
    sFlagA[3] = 0;
    sFlagA[4] = 0;
    sFlagA[5] = 0;
    sFlagA[6] = 0;
    sFlagA[7] = 0;
}

void HuntBotReset(void* screenp) {
    HuntScreen* screen = (HuntScreen*)screenp;
    int i;

    lbl_804D4D54 = 0;
    lbl_804D4D58 = 0;
    lbl_8046D5B8.state = 0;
    lbl_8046D5B8.timer = 1000.0f * (float)lbl_803A4028.startTime;
    lbl_8046D5B8.unk4 = 3000.0f;
    fn_800F8ED0();
    fn_800B969C();
    lbl_804D4D74 = fn_800B949C(0x53FA489C, &sSlots[0], 0x10, 1);
    for (i = 0; i < lbl_804D4D74; i++) {
        C14Msg m;
        m.unk0 = 4;
        m.index = 0;
        m.handle = 0x2840341C;
        SendMsg(sSlots[i], 0xC14, &m);
    }
    for (i = 0; i < lbl_804D4D74; i++) {
        fn_80120D60(sSlots[i]);
        fn_80120D28(sSlots[i]);
    }
    for (i = 0; i < 16; i++) {
        sFlagC[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        sFlagB[i] = 0;
    }
    Fn80140778();
}

int HuntBotPick(void* obj, int* out, float radius) {
    float best;
    int i;
    int found;

    if (!obj || !out) {
        return 0;
    }
    best = radius * radius;
    found = 0;
    for (i = 0; i < 16; i++) {
        u32 h = sSlots[i];
        float v[8];
        int j;
        SendMsg(h, 0x400, v);
        for (j = 0; j < 3; j++) {
            float d = ((float*)obj)[j] - v[j];
            if (d * d < best) {
                best = d * d;
                out[0] = j;
                found = 1;
            }
        }
    }
    return found ? (int)best : 0;
}

void HuntBotSetFlag(int i) {
    sFlagA[i] = 1;
}

u32 HuntBotPickable(u32 handle) {
    u32 obj;
    int player = HandleToPlayer(handle);
    if (player == -1) {
        obj = 0;
        switch (handle >> 24) {
        case 2:
            obj = PlayerGetPossessor(handle) + 0x26E0;
            break;
        case 0x13: {
            u32 o = (u32)fn_800A3990(handle);
            u32 v = *(u32*)(o + 0x5EC);
            if (v != 0) {
                obj = v + 0x10;
            }
            break;
        }
        }
        if (obj != 0 && *(u32*)(obj + 4) != 0) {
            return ((1 << player) & HandleToPlayer(*(int*)(*(int*)(obj + 4) + 0x38))) != 0;
        }
        return 1;
    }
    return (lbl_804D4D54 & (1 << player)) != 0;
}

int HuntBotScore(void* screen, u32 handle, u32 index) {
    (void)screen;
    (void)handle;
    (void)index;
    return 1;
}

int HuntBotHit(void* obj, float* p) {
    (void)obj;
    (void)p;
    return 1;
}

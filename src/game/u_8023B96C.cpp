// u_8023B96C.cpp: match rules / scoreboard setup (0x8023B96C-0x8023E7E0).
// A table of match-rule variables (score limit, time limit, handicap, ...) is read and
// written through 0x41D/0x41E messages keyed by name hash, mirrored into a menu/HUD, and
// the match phases (countdown, play, results) are driven from RulesUpdate.  Names are
// descriptive guesses; the code is matched against the retail DOL.


typedef unsigned long u32;
typedef unsigned char u8;

extern "C" {
void* memcpy(void* dst, const void* src, unsigned long n);
void* memset(void* dst, int c, unsigned long n);
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

// Match rule variables (0x803BE310, defaults copy at 0x803BE354).
struct RulesVars {
    int scoreLimit;     // 0x00 0..100
    int timed;          // 0x04 0..1
    int timeLimit;      // 0x08 5..1200
    int opt0C;          // 0x0C 0..1
    int players;        // 0x10 1..8
    int handicap;       // 0x14 -5..5
    int opt18;          // 0x18 0..1
    int count;          // 0x1C 1..list size
    int opt20;          // 0x20 0..1
    int opt24;          // 0x24 0..1
    int opt28;          // 0x28 5..120
    int opt2C;          // 0x2C 0..1
    int opt30;          // 0x30 0..1
    int opt34;          // 0x34 0..1
    int opt38;          // 0x38 0..1
    int opt3C;          // 0x3C 0..1
    int opt40;          // 0x40 0..1
};

// Match phase state (0x804A28A0, 0x50 bytes).
struct RulesState {
    int phase;          // 0x00
    float timeLeft;     // 0x04
    float fade;         // 0x08
    int count;          // 0x0C
    int list[16];       // 0x10
};

// Per-slot flags (0x803BE4A0..).
struct RulesFlags {
    int a[8];           // 0x00
    int b[8];           // 0x20
    int c[8];           // 0x40
    int d[8];           // 0x60
};

// Game object being set up: handles of the players and a few settings.
struct Game {
    char pad00[8];
    int f08;                // 0x08
    int f0C;                // 0x0C
    int f10;                // 0x10
    int f14;                // 0x14
    char pad18[4];
    int count;              // 0x1C
    char pad20[4];
    u32 handle[0x28];       // 0x24
    int count2;             // 0xC4
    int team[16];           // 0xC8
    int ids[8];             // 0x108
    u32 mask;               // 0x128
};

// Message payload carrying one rule variable.  The value overlays the String's slot.
struct VarMsg {
    int unk00;              // 0x00
    u32 key;                // 0x04
    int unk08;              // 0x08
    union {
        int i;
        float f;
        String s;
    } v;                    // 0x0C
};

// 0xC14 message: player-to-player event.
struct EventMsg {
    int unk00;              // 0x00
    int arg;                // 0x04
    int from;               // 0x08
    u32 key;                // 0x0C
};

struct OwnerInfo {
    char pad00[0x358];
    int f358;               // 0x358
};

struct ActorInfo {
    char pad00[0xC8];
    struct { char pad00[0xC]; int f0C; }* c8;   // 0xC8
};

extern "C" {
int SendMsg(int to, int msg, void* data);
int fn_800FD3EC(void);
void fn_8013FCF0(int a, int b, int c);
void fn_800FEF2C(float a, float b, float c, float d, int handle, ...);
void fn_8013E998(u32 hash, int* out);
void fn_801403D0(float f, int a, int b);
void fn_800FF4E4(int a);
void fn_80140918(void);
void fn_801408C8(void);
void fn_800F8ED0(void);
void fn_800B969C(void);
void fn_800B949C(u32 hash, int* out, int a, int b);
int fn_800A3990(void);
int fn_800A3854(void);
void fn_800FC764(u32 a, int b);
void fn_8010FE64(int a, int hash);
void fn_8010FE38(int a, int b);
u32 fn_800FD6EC(int i);
float* fn_800FD424(void);
}

int HandleToPlayer(int handle);
int PlayerTeam(int player);
u32 StrFindKeySkip(u32 key);
void StatAdd(float v, int a, int b, int c);
float StatGet(int a, int b, int c);
void StatSet(float v, int a, int b, int c);
void StatRegister(float v, int a, int b, int c, void (*fn)(void*), void* p);
void TextBegin();
void TextSetSize(int s);
void TextSetColor(int r, int g, int b, int a);
void TextSetMode(int m);
void TextSetBox(int a, int b, int c, int d);
void TextSizeScaled(int s);
void DebugTextPos(int a, int b, int c);
void DebugText(const char* fmt, ...);
int ActorGet(int id);
OwnerInfo* OwnerGetInfo(u32 handle);
void ItemColorSendMode1(int i);
void ItemColorSendMode0(int i);
void Fn80140778();
int GameEvent(int a, int b);

// Data owned by other files.
extern RulesVars lbl_803BE310;
extern RulesVars lbl_803BE354;
extern u32 lbl_803BE398[4];
extern RulesFlags lbl_803BE4A0;
extern RulesState lbl_804A28A0;
extern int lbl_804D34D8;
extern int lbl_804D5478;
extern int lbl_804D5480;
extern int lbl_804D5484;
extern int lbl_804D5488;
extern int lbl_804D548C;
extern int lbl_804D5490;
extern int lbl_804D5494;
extern char lbl_8034A176[];

extern int lbl_804A28F0[16];
extern int lbl_804A2910[8];

// Send one rule variable to a target (set request, message 0x41D).
static inline void SendVar(int to, u32 key, int value) {
    VarMsg m;

    m.v.i = value;
    m.key = key;
    m.unk08 = 0;
    SendMsg(to, 0x41D, &m);
}

// Ask a target for one rule variable (get request, message 0x41E).
static inline int GetVar(int to, u32 key) {
    VarMsg m;

    m.key = key;
    m.unk08 = 0;
    SendMsg(to, 0x41E, &m);
    return m.v.i;
}

static inline void VarIO(int isGet, int* slot, int* msg, int lo, int hi) {
    if (isGet == 0) {
        int v = msg[3];

        if (v < lo) {
            *slot = lo;
        } else if (v > hi) {
            *slot = hi;
        } else {
            *slot = v;
        }
    } else {
        msg[3] = *slot;
    }
}

void RulesVarChanged(int key);
void RulesSyncMenu();
int RulesPlayerMsg(int from, u32 key, int arg1, int arg2);
void RulesEnd(Game* g);

// 0x8023B96C: get (isGet != 0) or set one rule variable from a message.
int RulesVarAccess(int isGet, int* msg) {
    int handled = 0;
    int key = msg[1];

    switch (key) {
    case 0xE905AA02:
        VarIO(isGet, &lbl_803BE310.scoreLimit, msg, 0, 100);
        handled = 1;
        break;
    case 0xC18DF518:
        VarIO(isGet, &lbl_803BE310.handicap, msg, -5, 5);
        handled = 1;
        break;
    case 0xA8F9C0EE:
        VarIO(isGet, &lbl_803BE310.timeLimit, msg, 5, 0x4B0);
        handled = 1;
        break;
    case 0xA887EF89:
        VarIO(isGet, &lbl_803BE310.timed, msg, 0, 1);
        handled = 1;
        break;
    case 0x896F8C7F:
        VarIO(isGet, &lbl_803BE310.opt3C, msg, 0, 1);
        handled = 1;
        break;
    case 0xBDEDC248:
        VarIO(isGet, &lbl_803BE310.opt0C, msg, 0, 1);
        handled = 1;
        break;
    case 0xD1DA12E7:
        if (isGet == 0) {
            float f = *(float*)&msg[3];

            lbl_804A28A0.timeLeft = 5000.0f;
            if (!(f < 5000.0f)) {
                lbl_804A28A0.timeLeft = f;
                if (f > 1200000.0f) {
                    lbl_804A28A0.timeLeft = 1200000.0f;
                }
            }
        } else {
            *(float*)&msg[3] = lbl_804A28A0.timeLeft;
        }
        handled = 1;
        break;
    case 0xD199C84A:
        VarIO(isGet, &lbl_803BE310.opt30, msg, 0, 1);
        handled = 1;
        break;
    case 0xC355E8EE:
        VarIO(isGet, &lbl_803BE310.opt38, msg, 0, 1);
        handled = 1;
        break;
    case 0xE2A324C4:
        VarIO(isGet, &lbl_803BE310.opt40, msg, 0, 1);
        handled = 1;
        break;
    case 0x02D7303A:
        if (isGet == 0) {
            lbl_803BE310.count = msg[3];
            if (lbl_803BE310.count < 1) {
                lbl_803BE310.count = 1;
            } else if (lbl_804A28A0.count < lbl_803BE310.count) {
                lbl_803BE310.count = lbl_804A28A0.count;
            }
        } else {
            msg[3] = lbl_803BE310.count;
        }
        handled = 1;
        break;
    case 0xF90653CA:
        VarIO(isGet, &lbl_803BE310.players, msg, 1, 8);
        handled = 1;
        break;
    case 0xF0FB92DF:
        VarIO(isGet, &lbl_803BE310.opt28, msg, 5, 0x78);
        handled = 1;
        break;
    case 0xE90B5145:
        VarIO(isGet, &lbl_803BE310.opt20, msg, 0, 1);
        handled = 1;
        break;
    case 0x001512C7:
        VarIO(isGet, &lbl_803BE310.opt18, msg, 0, 1);
        handled = 1;
        break;
    case 0x31809F6F:
        if (isGet == 0) {
            lbl_804D34D8 = msg[3];
        } else {
            msg[3] = lbl_804D34D8;
        }
        handled = 1;
        break;
    case 0x2D88B734:
        VarIO(isGet, &lbl_803BE310.opt34, msg, 0, 1);
        handled = 1;
        break;
    case 0x1783A79C:
        VarIO(isGet, &lbl_803BE310.opt2C, msg, 0, 1);
        handled = 1;
        break;
    case 0x78F28F77:
        VarIO(isGet, &lbl_803BE310.opt24, msg, 0, 1);
        handled = 1;
        break;
    }
    if (handled != 0 && isGet == 0) {
        RulesVarChanged(msg[1]);
    }
    return handled;
}

// 0x8023C134: empty hook.
void RulesNop() {
}

// 0x8023C138: react to a changed rule variable.
void RulesVarChanged(int key) {
    switch (key) {
    case 0x02D7303A:
        break;
    case 0xE90B5145:
        SendVar(0, 0xE90B5145, lbl_803BE310.opt20);
        break;
    case 0xA8F9C0EE:
        lbl_804A28A0.timeLeft = 1000.0f * (float)lbl_803BE310.timeLimit;
        break;
    case 0xF0FB92DF:
        SendVar(0, 0xF0FB92DF, lbl_803BE310.opt28);
        break;
    case 0x78F28F77:
        SendVar(0, 0x78F28F77, lbl_803BE310.opt24);
        break;
    case 0x31809F6F:
        RulesSyncMenu();
        return;
    }
    if (lbl_804D34D8 == 0x77297F71) {
        lbl_803BE354.scoreLimit = lbl_803BE310.scoreLimit;
        lbl_803BE354.timed = lbl_803BE310.timed;
        lbl_803BE354.timeLimit = lbl_803BE310.timeLimit;
        lbl_803BE354.opt0C = lbl_803BE310.opt0C;
        lbl_803BE354.players = lbl_803BE310.players;
        lbl_803BE354.handicap = lbl_803BE310.handicap;
        lbl_803BE354.opt18 = lbl_803BE310.opt18;
        lbl_803BE354.count = lbl_803BE310.count;
        lbl_803BE354.opt20 = lbl_803BE310.opt20;
        lbl_803BE354.opt24 = lbl_803BE310.opt24;
        lbl_803BE354.opt28 = lbl_803BE310.opt28;
        lbl_803BE354.opt2C = lbl_803BE310.opt2C;
        lbl_803BE354.opt30 = lbl_803BE310.opt30;
        lbl_803BE354.opt34 = lbl_803BE310.opt34;
        lbl_803BE354.opt38 = lbl_803BE310.opt38;
        lbl_803BE354.opt3C = lbl_803BE310.opt3C;
        lbl_803BE354.opt40 = lbl_803BE310.opt40;
    }
}

// The default rule set for the two preset modes.
static inline void SendDefaults() {
    SendVar(0x4B000000, 0xE905AA02, 15);
    SendVar(0x4B000000, 0xA887EF89, 1);
    SendVar(0x4B000000, 0xBDEDC248, 0);
    SendVar(0x4B000000, 0x6E9F5FB8, 0);
    SendVar(0x4B000000, 0x1B95702C, 0);
    SendVar(0x4B000000, 0xE90B5145, 0);
    SendVar(0x4B000000, 0xC18DF518, -1);
    SendVar(0x4B000000, 0x001512C7, 1);
    SendVar(0x4B000000, 0xC355E8EE, 1);
    SendVar(0x4B000000, 0x896F8C7F, 0);
    SendVar(0x4B000000, 0xE2A324C4, 0);
}

// 0x8023C41C: push the rules of the selected preset mode to the menu.
void RulesSyncMenu() {
    int mode = lbl_804D34D8;

    if (mode == lbl_803BE398[0]) {
        lbl_804D5478 = 0;
    } else if (mode == lbl_803BE398[1]) {
        lbl_804D5478 = 1;
    } else if (mode == lbl_803BE398[2]) {
        lbl_804D5478 = 2;
    }
    switch (mode) {
    case 0x77297F71:
        SendVar(0x4B000000, 0xE905AA02, lbl_803BE354.scoreLimit);
        SendVar(0x4B000000, 0xA887EF89, lbl_803BE354.timed);
        SendVar(0x4B000000, 0xBDEDC248, lbl_803BE354.opt0C);
        SendVar(0x4B000000, 0xA267AD6C, fn_800FD3EC());
        SendVar(0x4B000000, 0xE90B5145, lbl_803BE354.opt20);
        SendVar(0x4B000000, 0xF90653CA, lbl_803BE354.players);
        SendVar(0x4B000000, 0xC18DF518, lbl_803BE354.handicap);
        SendVar(0x4B000000, 0x001512C7, lbl_803BE354.opt18);
        SendVar(0x4B000000, 0xC355E8EE, lbl_803BE354.opt38);
        SendVar(0x4B000000, 0x896F8C7F, 0);
        SendVar(0x4B000000, 0xE2A324C4, 0);
        break;
    case 0xE2C7F0EF:
        SendDefaults();
        break;
    case 0x3D23AB6C:
        SendDefaults();
        break;
    }
}

// 0x8023D0BC: message handler of the rules object.
int RulesMsg(int to, int msg, int* data) {
    switch (msg) {
    case 0xC14:
        return RulesPlayerMsg(to, data[3], data[2], data[1]);
    case 0x41E:
        return RulesVarAccess(1, data);
    case 0x41D:
        return RulesVarAccess(0, data);
    case 0x420:
        switch (data[1]) {
        case 0x381B348D:
            data[3] = (data[2] < lbl_804A28A0.count) ? data[2] : lbl_804A28A0.count;
            memcpy((void*)data[4], lbl_804A28A0.list,
                   ((data[2] < lbl_804A28A0.count) ? data[2] : lbl_804A28A0.count) << 2);
            return 1;
        }
        return 0;
    }
    return 0;
}

// 0x8023D1B8: player-to-player event (0xC14).
int RulesPlayerMsg(int from, u32 key, int arg1, int arg2) {
    switch (key) {
    case 0x53D74347: {
        int pa = HandleToPlayer(arg2);
        int pb;
        int sc;

        PlayerTeam(pa);
        pb = HandleToPlayer(arg1);
        PlayerTeam(pb);
        if (pb == -1) {
            return 0;
        }
        if (pa == -1) {
            fn_8013FCF0(from, -1, pb);
        } else {
            u32 name = StrFindKeySkip(0xCEFFE6B);

            fn_800FEF2C(1.0f, 1.0f, 1.0f, 22.0f, pb, name);
        }
        sc = GetVar(arg1, 0x9BD4EF06);
        StatAdd(-1.0f, 0xE2D6A9C0, 0, pb);
        StatAdd(-1.0f, 0xE2D6A9C0, 1, sc);
        return 1;
    }
    case 0x901EEFA2:
        if (arg2 != 0x1AF27550) {
            return 0;
        }
        if (lbl_804A28A0.count < 16) {
            lbl_804A28A0.list[lbl_804A28A0.count++] = arg1;
        }
        return 1;
    case 0x7C3F4E3E: {
        int pa = HandleToPlayer(arg2);
        int ta = PlayerTeam(pa);
        int pb = HandleToPlayer(arg1);
        int tb = PlayerTeam(pb);
        int sc;

        if (pa == -1 || pb == -1) {
            return 0;
        }
        fn_8013FCF0(from, pb, pa);
        if (ta == tb && pa != pb) {
            if (lbl_803BE310.handicap != 0) {
                int h = lbl_803BE310.handicap;
                int x;

                if (lbl_803BE310.opt2C == 0) {
                    x = h;
                } else {
                    int a = h < 0 ? -h : h;

                    x = (h / a) * (1 << a);
                }
                StatAdd((float)x, 0xE2D6A9C0, 1, tb);
                StatAdd((float)x, 0xE2D6A9C0, 0, pb);
            }
            return 1;
        }
        sc = GetVar(arg1, 0x9BD4EF06);
        if (pa == pb) {
            StatAdd(-1.0f, 0xE2D6A9C0, 0, pb);
            StatAdd(-1.0f, 0xE2D6A9C0, 1, sc);
        } else {
            StatAdd(1.0f, 0xE2D6A9C0, 0, pb);
            StatAdd(1.0f, 0xE2D6A9C0, 1, sc);
        }
        return 1;
    }
    }
    return 0;
}

// 0x8023D5AC: draw the phase banner and the per-player scores.
void RulesDraw(Game* g, int sel) {
    int i;

    if (lbl_804D5480 != 0 && sel == -1) {
        switch (lbl_804A28A0.phase) {
        case 2:
            if (lbl_804D5494 == 0) {
                if (lbl_804D548C == 1) {
                    lbl_804D548C = 0;
                    for (i = 0; i < g->count; i++) {
                        SendVar(fn_800FD6EC(i), 0xDE3EAAD7, 1);
                    }
                }
                lbl_804D5494 = 1;
            }
            if (lbl_803BE310.timed != 0) {
                fn_801403D0(lbl_804A28A0.timeLeft, 0, 0);
            }
            break;
        case 0:
            if (lbl_804D5488 == 0) {
                lbl_804D5488 = 1;
                for (i = 0; i < g->count; i++) {
                    SendVar(fn_800FD6EC(i), 0xDE3EAAD7, 0);
                }
            }
            if (lbl_804D5488 == 1) {
                TextBegin();
                TextSetSize(0x18);
                TextSetColor(0xFF, 0xFF, 0xFF, 0xFF);
                TextSetMode(2);
                TextSetBox(0, 0x280, 0, 0x150);
                DebugText((const char*)StrFindKeySkip(0xBAAC35AB));
            }
            break;
        case 1:
            if (lbl_804D5490 == 0) {
                if (lbl_804D5488 == 1) {
                    lbl_804D5488 = 0;
                }
                lbl_804D5490 = 1;
            }
            if (lbl_804D548C == 0) {
                lbl_804D548C = 1;
            }
            if (lbl_804D548C == 1) {
                TextBegin();
                TextSetSize(0x18);
                TextSetColor(0xFF, 0xFF, 0xFF, (int)(255.0f * (lbl_804A28A0.fade / 1200.0f)) & 0xFF);
                TextSetMode(2);
                TextSetBox(0, 0x280, 0, 0x150);
                DebugText((const char*)StrFindKeySkip(0xBAAC35AB));
            }
            break;
        case 3:
            if (lbl_804D5484 == 1) {
                lbl_804D5484 = 0;
            }
            lbl_804D5490 = 0;
            lbl_804D5494 = 0;
            break;
        }
        for (i = 0; i < g->count; i++) {
            u32 h = fn_800FD6EC(i);

            if ((h >> 24) == 2) {
                int pos[3];
                int score;
                float* col;

                ActorGet(i);
                if (lbl_803BE4A0.a[i] != 0) {
                    lbl_803BE4A0.a[i] = 0;
                }
                {
                    int team = PlayerTeam(i);

                    col = fn_800FD424();
                    score = (int)StatGet(0xE2D6A9C0, 1, team);
                }
                switch (i) {
                case 2:
                    fn_8013E998(0x1D65CCEC, pos);
                    break;
                case 0:
                    fn_8013E998(0x4DA3732E, pos);
                    break;
                case 1:
                    fn_8013E998(0xB584A00D, pos);
                    break;
                case 3:
                    fn_8013E998(0x8546F9CB, pos);
                    break;
                }
                if (lbl_803BE4A0.c[i] == 0) {
                    lbl_804A28F0[i] = score;
                    lbl_803BE4A0.c[i] = 1;
                }
                if (score != lbl_804A28F0[i]) {
                    lbl_804A28F0[i] = score;
                }
                if (lbl_803BE4A0.b[i] == 0) {
                    lbl_803BE4A0.b[i] = 1;
                }
                TextBegin();
                TextSetColor((int)(255.0f * col[0]) & 0xFF, (int)(255.0f * col[1]) & 0xFF,
                             (int)(255.0f * col[2]) & 0xFF, 0xFF);
                TextSizeScaled(pos[2]);
                DebugTextPos(pos[0], pos[1], 2);
                DebugText("%02d", score);
            }
        }
    }
}

// 0x8023DAA4: per-frame match update (phases 0..3).
void RulesUpdate(float dt, Game* g) {
    int i;

    if (lbl_804D5480 != 0) {
        if (lbl_804A28A0.phase == 2) {
            int done = 0;

            for (i = 0; i < g->count; i++) {
                if ((g->handle[i] >> 24) == 2 && OwnerGetInfo(g->handle[i])->f358 == 1 && g->handle[i] != 0xFFFFFFFF) {
                    EventMsg m;

                    m.arg = 1;
                    m.from = 0x4B000000;
                    m.key = 0xF8629C97;
                    SendMsg(g->handle[i], 0xC14, &m);
                }
            }
            if (lbl_803BE310.timed != 0) {
                lbl_804A28A0.timeLeft -= dt;
                if (lbl_804A28A0.timeLeft <= 0.0f) {
                    lbl_804A28A0.timeLeft = 0.0f;
                    done = 1;
                }
            }
            if (lbl_803BE310.scoreLimit > 0) {
                for (i = 0; i < 8; i++) {
                    if ((g->mask & (4 << i)) != 0) {
                        if (lbl_803BE310.scoreLimit <= (int)StatGet(0xE2D6A9C0, 1, i)) {
                            done = 1;
                        }
                    }
                }
            }
            if (done) {
                EventMsg m;

                for (i = 0; i < g->count; i++) {
                    ItemColorSendMode1(i);
                    SendVar(g->handle[i], 0x640D1F4D, 0);
                }
                fn_800FF4E4(-1);
                lbl_804A28A0.fade = 5000.0f;
                lbl_804A28A0.phase = 3;
                m.arg = 0x1E0B3621;
                m.from = 0x4B000000;
                m.key = 0xF8629C97;
                SendMsg(0x20000000, 0xC14, &m);
            }
        } else {
            switch (lbl_804A28A0.phase) {
            case 0:
                lbl_804A28A0.fade -= dt;
                if (lbl_804A28A0.fade <= 0.0f) {
                    lbl_804A28A0.phase = 1;
                    lbl_804A28A0.fade = 1200.0f;
                }
                break;
            case 1:
                lbl_804A28A0.fade -= dt;
                if (lbl_804A28A0.fade <= 0.0f) {
                    lbl_804A28A0.fade = 0.0f;
                    lbl_804A28A0.phase = 2;
                    for (i = 0; i < g->count; i++) {
                        ItemColorSendMode0(i);
                    }
                }
                break;
            case 3:
                RulesEnd(g);
                break;
            default:
                lbl_804A28A0.phase = 2;
                break;
            }
        }
    }
}

// 0x8023DDAC: shut the rules object down.
void RulesShutdown() {
    int i;

    if (lbl_804D5480 != 0) {
        lbl_804D5480 = 0;
        for (i = 0; i < 8; i++) {
            if (lbl_803BE4A0.c[i] == 1) {
                lbl_803BE4A0.c[i] = 0;
            }
            if (lbl_803BE4A0.b[i] == 1) {
                lbl_803BE4A0.b[i] = 0;
            }
            if (lbl_803BE4A0.d[i] == 1) {
                lbl_803BE4A0.d[i] = 0;
            }
        }
        Fn80140778();
        fn_801408C8();
    }
}

// 0x8023DEF0: start a match from the game object's setup.
void RulesStart(Game* g) {
    int i;
    int c0, bc, c4, c8;

    if (lbl_804D5480 != 0) {
        SendVar(0x4B000000, 0x31809F6F, 0xE2C7F0EF);
        SendVar(0x4B000000, 0x02D7303A, lbl_804A28A0.count);
        SendVar(0x4B000000, 0xA8F9C0EE, 300);
        SendVar(0x4B000000, 0xF90653CA, 8);
        if (g->f08 != 0) {
            int t = g->f14;

            if (t != 0) {
                lbl_803BE310.timeLimit = t * 60;
            }
            lbl_803BE310.timed = (t != 0);
            lbl_803BE310.scoreLimit = g->f10;
            i = 0;
            do {
                if (i == 3) {
                    lbl_803BE310.opt3C = (g->f0C >> 3) & 1;
                } else if (i < 3 && i > 1) {
                    SendVar(0x4B000000, 0x0E83B88B, (g->f0C >> 2) & 1);
                }
                i++;
            } while (i < 0x15);
        }
        fn_80140918();
        RulesNop();
        for (i = 0; i < g->count; i++) {
            StatRegister(0.0f, 0xE2D6A9C0, 0, i, 0, 0);
        }
        i = 0;
        do {
            if ((g->mask & (4 << i)) != 0) {
                StatRegister(0.0f, 0xE2D6A9C0, 1, i, 0, 0);
            }
            i++;
        } while (i < 8);
        fn_800B949C(0xA4B6BB62, &c0, 1, 1);
        fn_800B949C(0xA2C244AB, &bc, 1, 1);
        fn_800B949C(0x5BE2B379, &c4, 1, 1);
        fn_800B949C(0x0D5B0588, &c8, 1, 1);
        for (i = 0; i < g->count; i++) {
            if ((g->handle[i] >> 24) == 0x13) {
                ActorInfo* a = (ActorInfo*)fn_800A3990();

                switch (g->team[i]) {
                case 2:
                    a->c8->f0C = GameEvent(-1, c4);
                    break;
                case 0:
                    a->c8->f0C = GameEvent(-1, c0);
                    break;
                case 1:
                    a->c8->f0C = GameEvent(-1, bc);
                    break;
                case 3:
                    a->c8->f0C = GameEvent(-1, c8);
                    break;
                }
            }
        }
    }
}

// 0x8023E334: reset the rules state.
void RulesInit() {
    lbl_804D5480 = 1;
    memset(&lbl_804A28A0, 0, 0x50);
    lbl_803BE4A0.a[0] = 0;
    lbl_804A28A0.phase = 0;
    lbl_804A28A0.fade = 3000.0f;
    lbl_803BE4A0.a[1] = 0;
    lbl_804A28A0.timeLeft = 1000.0f * (float)lbl_803BE310.timeLimit;
    lbl_803BE4A0.a[2] = 0;
    lbl_803BE4A0.a[3] = 0;
    lbl_803BE4A0.a[4] = 0;
    lbl_803BE4A0.a[5] = 0;
    lbl_803BE4A0.a[6] = 0;
    lbl_803BE4A0.a[7] = 0;
}

// 0x8023E3D8: end the match and clear the per-player state.
void RulesEnd(Game* g) {
    int i;
    int k;

    lbl_804A28A0.phase = 0;
    lbl_804A28A0.fade = 3000.0f;
    lbl_804A28A0.timeLeft = 1000.0f * (float)lbl_803BE310.timeLimit;
    fn_800F8ED0();
    fn_800B969C();
    for (i = 0; i < g->count2; i++) {
        StatSet(0.0f, 0xE2D6A9C0, 1, i);
    }
    for (u32 j = 0; (int)j < fn_800A3854(); j++) {
        fn_800FC764((j & 0xFFFFFF) | 0x13000000, 0);
    }
    for (i = 0; i < g->count; i++) {
        u32 h;

        StatSet(0.0f, 0x7BFB940D, 0, i);
        StatSet(0.0f, 0xE2D6A9C0, 0, i);
        h = g->handle[i];
        if ((h >> 24) == 0x13) {
            int m[2];

            m[1] = 1;
            SendMsg(h, 0xC00, m);
        } else {
            int a = ActorGet(h & 0xFFFFFF);

            fn_800FC764(g->handle[i], 0);
            fn_8010FE64(a, 0x2D3432C);
            fn_8010FE38(a, 1);
        }
        SendVar(g->handle[i], 0x0C343772, g->ids[i]);
    }
    for (i = lbl_804A28A0.count - 1; i >= 0; i--) {
        int m[2];

        m[1] = 0;
        SendMsg(lbl_804A28A0.list[i], 0xC00, m);
        for (k = i; k < lbl_804A28A0.count; k++) {
            lbl_804A28A0.list[k] = lbl_804A28A0.list[k + 1];
        }
        lbl_804A28A0.count--;
    }
    SendVar(0x4B000000, 0xF90653CA, g->count2);
    for (i = 0; i < 8; i++) {
        lbl_803BE4A0.a[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        if (lbl_803BE4A0.c[i] == 1) {
            lbl_803BE4A0.c[i] = 0;
        }
        if (lbl_803BE4A0.b[i] == 1) {
            lbl_803BE4A0.b[i] = 0;
        }
        if (lbl_803BE4A0.d[i] == 1) {
            lbl_803BE4A0.d[i] = 0;
        }
        lbl_804A28F0[i] = 0;
        lbl_804A2910[i] = 0;
    }
    if (lbl_804D5488 == 1) {
        lbl_804D5488 = 0;
    }
    if (lbl_804D548C == 1) {
        lbl_804D548C = 0;
    }
    if (lbl_804D5484 == 1) {
        lbl_804D5484 = 0;
    }
    lbl_804D5490 = 0;
    lbl_804D5494 = 0;
    Fn80140778();
}

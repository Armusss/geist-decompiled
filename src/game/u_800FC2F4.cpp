// u_800FC2F4.cpp (0x800FC2F4-0x800FDDE4): the minigame engine: rules, scratch, streaks, player teams and bots, colour names.
// Split from the old u_800F73C8 (0x800F73C8-0x800FDDE4). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary evidence (split 2026-11-22): own .sdata2 0x804D74F0-0x804D7500 (0x4330 double, 15.0), .sdata
// 0x804D2D88-0x804D2D9C, the minigame state .data 0x8039E730/0x8039E748 and the name tables 0x8039E8A8-0x8039EA28,
// .bss 0x8045D9B0, .rodata 0x80345410 (memory-check / colour strings).


#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

struct StreamNode;
struct EffectKind;
struct EffectInst;
struct Emitter;
struct Vector;
struct CamObj;
struct Shake;
struct GameType;
struct Minigame;

// Functions called from other units under their placeholder names.
extern "C" float fn_800F7920(u32 row, u32 key);
extern "C" int fn_800F7994(u32 row, u32 key);
extern "C" void fn_800F7A90(u32 row);
extern "C" int fn_800F7AF0(u32 emitter, u32 row);
extern "C" int fn_800F7C64(u32 emitter, int row);
extern "C" int fn_800F7D28(u32 emitter, u32 row);
extern "C" int fn_800F80DC(u32 handle);
extern "C" int fn_800F8130(u32 handle, int i);
extern "C" int fn_800F81EC(u32 handle);
extern "C" int fn_800F825C(u32 handle, u32 row, int n);
extern "C" void fn_800F86A0(u32 handle);
extern "C" u32 fn_800F8A30(int user);
extern "C" u32 fn_800F8AF4(u32 row);
extern "C" void fn_800F8ED0(void);
extern "C" void fn_800F9468(int on);
extern "C" int fn_800F94C8(u32 handle);
extern "C" void fn_800F94F8(u32 handle, short v);
extern "C" void fn_800F9578(u32 handle, int v);
extern "C" float fn_800F95E4(u32 handle);
extern "C" int fn_800F9614(u32 handle);
extern "C" void fn_800F96E0(u32 handle);
extern "C" u32 fn_800F9724(int owner, int id);
extern "C" u32 fn_800FC3B8(int id);
extern "C" int fn_800FC764(u32 who, int arg);
extern "C" char* fn_800FD334(int i);
extern "C" int fn_800FD3DC(void);
extern "C" int fn_800FD3EC(void);
extern "C" char* fn_800FD3FC(int i);
extern "C" char* fn_800FD424(int i);
extern "C" const char* fn_800FD44C(int c);
extern "C" const char* fn_800FD5C4(int c);
extern "C" int fn_800FD6EC(int i);
extern "C" void fn_800FD924(int arg);
extern "C" void fn_800FD998(void);
extern "C" void fn_800FDA10(void);
extern "C" void fn_800FDA90(void);
extern "C" void fn_800FDC48(void);
extern "C" void fn_800FDC78(void);

void fn_80026660(int msg, void* fn, int arg);
extern "C" int SendMsg(int to, int msg, void* data);

static inline void SetVarInt(int target, u32 hash, int value) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    SendMsg(target, 0x41D, &m);
}

static inline int GetVarInt(int target, u32 hash) {
    QueryMsg m;
    *(int*)&m.result = 0;
    m.hash = hash;
    m.unk8 = 0;
    SendMsg(target, 0x41E, &m);
    return *(int*)&m.result;
}

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

// ---- minigame engine / player teams ------------------------------------------------------

struct GameType {
    const char* name;       // 0x00
    void (*init)(void);     // 0x04
    void (*reset)(void);    // 0x08
    void (*term)(void);     // 0x0C
    void (*update)(void);   // 0x10
    void (*draw)(void* mg, int arg);    // 0x14
    u8 pad18[4];
    void (*restart)(void);  // 0x1C
};

struct Minigame {
    int type;               // 0x00 (0x8039E748)
    int unk04;
    int unk08;              // 0x08
    u32 flags;              // 0x0C
    int unk10;              // 0x10
    int unk14;              // 0x14
    int unk18;              // 0x18
    int count;              // 0x1C: number of players
    int bots;               // 0x20
    int handle[8];          // 0x24
    char name[8][16];       // 0x44
    int unkC4;              // 0xC4
    int team[8];            // 0xC8
    u8 padE8[0x12C - 0xE8];
    u32 flags12C;           // 0x12C
    u32 flags130;           // 0x130
    int state;              // 0x134
    u8 pad138[0x160 - 0x138];
};

extern Minigame gMinigame;              // 0x8039E748
extern GameType* gGameTypes[6];         // 0x8039E730
extern char gTeamColors[9][16];         // 0x803E9938
extern char gTeamColors2[9][16];        // 0x803E98A8
extern char gTeamNameBuf[0x40];         // 0x8045D9B0

extern "C" {
int fn_802A1FA0(void);
int strlen(const char* s);
char* strncpy(char* d, const char* s, u32 n);
int strncmp(const char* a, const char* b, u32 n);
int sprintf(char* s, const char* fmt, ...);
void fn_800872BC(int level, const char* msg);
int fn_80142E94(void);
int fn_800B705C(void);
u32 fn_80153B9C(void);
void* PlayerGetPossessor(int i);                            // 0x80049888
int Rand(void);                                             // 0x8000DDD0
void StatSet(int a, int b, int c, float v);                 // 0x80120AC0
const char* StrFindKeySkip(u32 key);                        // 0x80086364
int ActorCount(void);                                       // 0x80049930
void fn_80128C8C(void);
void fn_80140984(void);
void fn_801289D0(GameType* t);
void HudRefreshSlots(void* hud);                            // 0x800FE518
void fn_80128D7C(void);
void fn_800094D8(int a, int b);
void fn_802A1D0C(void);
void fn_802D1D7C(int a);
void fn_8027D3E4(float f);
void EmitterSetMode(int m);                                 // 0x800D8BD4
void fn_8014C858(void);
}

extern int gMinigameLevel;      // 0x804D2D88
extern int gStatThreshold;      // 0x804D2D90
extern int gStatMid;            // 0x804D2D94
extern int gStatHigh;           // 0x804D2D98
extern double gStatBias;        // 0x804D74F0
extern float gMinigameZoom;     // 0x804D74F8

extern "C" int fn_800FDDE4(u32 handle, int msg, int* data);   // message handler of the next unit
int MinigameMsgB(u32 handle, int msg, int* data);

// Stores/returns the minigame's scratch word.
void MinigameSetScratch(int v) {
    *(int*)((char*)&gMinigame + 8) = v;
}

int MinigameGetScratch(void) {
    return *(int*)((char*)&gMinigame + 8);
}

// Sets rule flag `id` (0..0x13 bits, 0x14.. dedicated words).
void MinigameSetRule(int id, int on) {
    if (id == 0x16) {
        gMinigame.unk14 = on;
        return;
    }
    if (id > 0x15) {
        if (id > 0x17) {
            return;
        }
        gMinigame.unk18 = on;
        return;
    }
    if (id > 0x14) {
        gMinigame.unk10 = on;
        return;
    }
    if (id < 0) {
        return;
    }
    if (on != 0) {
        gMinigame.flags |= 1 << id;
        return;
    }
    gMinigame.flags &= ~(1 << id);
}

u32 fn_800FC3B8(int id) {
    if (id == 0x16) {
        return gMinigame.unk14;
    }
    if (id < 0x16) {
        if (id > 0x14) {
            return gMinigame.unk10;
        }
        if (id >= 0) {
            return gMinigame.flags & 1 << id;
        }
    } else if (id < 0x18) {
        return gMinigame.unk18;
    }
    return 0;
}

// Loads the rule words from `p` (4 words).
void MinigameLoadRules(int* p) {
    if (p == NULL) {
        return;
    }
    gMinigame.flags = p[0];
    gMinigame.unk10 = p[1];
    gMinigame.unk14 = p[2];
    gMinigame.unk18 = p[3];
}

// Saves the rule words to `p` (4 words).
void MinigameSaveRules(int* p) {
    if (p == NULL) {
        return;
    }
    p[0] = gMinigame.flags;
    p[1] = gMinigame.unk10;
    p[2] = gMinigame.unk14;
    p[3] = gMinigame.unk18;
}

// Picks the game type from the name of the running level file.
int SysCheckMemory(void) {
    char name[0x401];
    char* path = (char*)fn_802A1FA0();
    int len = strlen(path);
    int slash = -1;
    int i;
    int start;
    int dot;
    int n;
    for (i = 0; i < len; i++) {
        if (path[i] == '\\' || path[i] == '/') {
            slash = i;
        }
    }
    start = slash + 1;
    dot = len;
    for (i = start; i < len; i++) {
        if (path[i] == '.') {
            dot = i;
            break;
        }
    }
    n = dot - slash - 1;
    strncpy(name, path + slash + 1, n);
    name[dot - slash - 1] = 0;
    for (i = 0; i < n; i++) {
        unsigned char c = name[i];
        name[i] = (c == 0xFF) ? 0xFF : __upper_map[c];
    }
    if (strncmp(name, "MAINMENU", 8) == 0) {
        fn_800872BC(0x40000, "Minigame : GameType FEAR_MAIN_MENU");
        return 4;
    }
    if (strncmp(name, "CREDITS", 7) == 0) {
        fn_800872BC(0x40000, "Minigame : GameType FEAR_CREDITS");
        return 5;
    }
    if (strncmp(name, "HUNT_", 5) == 0) {
        fn_800872BC(0x40000, "Minigame : GameType FEAR_HUNT");
        return 3;
    }
    if (strncmp(name, "EVAC_", 5) == 0) {
        fn_800872BC(0x40000, "Minigame : GameType FEAR_HUNT (evac detected)");
        return 3;
    }
    {
        int mode = fn_80142E94();
        if (mode == 2) {
            fn_800872BC(0x40000, "Default Minigame : GameType FEAR_STORY_MODE");
        } else {
            if (mode < 2) {
                if (mode == 0) {
                    fn_800872BC(0x40000, "Minigame : GameType FEAR_DM");
                    return 2;
                }
                if (mode >= 0) {
                    fn_800872BC(0x40000, "Minigame : GameType FEAR_CTH");
                    return 1;
                }
            } else if (mode == 4) {
                fn_800872BC(0x40000, "Default Minigame : GameType FEAR_STORY_MODE");
                return 0;
            }
            fn_800872BC(0x40000, "Minigame : GameType FEAR_STORY_MODE");
        }
    }
    return 0;
}

static inline int PlayerSlotOf(u32 handle) {
    int i;
    int n;
    int* p;
    do {
        i = 0;
        p = gMinigame.handle;
        n = gMinigame.count;
        if (n > 0) {
            do {
                if ((u32)*p == handle) {
                    return i;
                }
                p++;
                i++;
                n--;
            } while (n != 0);
        }
    } while (handle >> 24 == 0x13 && (handle = fn_80153B9C(), handle >> 24 == 2) &&
             *(int*)((char*)PlayerGetPossessor(0) + 0x2740) == 0);
    return -1;
}

void MinigameStreakUp(u32 handle);
void MinigameStreakDown(u32 handle);

extern "C" {
int fn_802A1D04(void);
int HuntBotPickable(u32 who);                                       // 0x80142864
u32 fn_800B8B64(int a, int b, int c, void* fn, int d);
u32 fn_800B8D68(u32 a, u32 b, int c);
void fn_8009F190(void* out, void* in);
void VecSwapXY2(void* a, void* b);                                  // 0x8016D18C
}
int MinigameIsArenaMap(void);

// Finds a target or spawn point for actor `who` and links the two (variable 0x7C18FE9E on
// both, spawn command 0xC07).  Returns 1 when one was found.
int fn_800FC764(u32 who, int arg) {
    u32 found = 0xFFFFFFFF;
    u32 pick;
    if (who == 0xFFFFFFFF) {
        return 0;
    }
    MinigameStreakDown(who);
    if (fn_802A1D04() == 3) {
        if (HuntBotPickable(who) != 0) {
            int msg[2];
            msg[0] = 1;
            msg[1] = 8;
            SendMsg(who, 0x40D, msg);
            return 1;
        }
        if (who != 0xFFFFFFFF) {
            int m[4];
            m[1] = arg;
            m[2] = 0x4B000000;
            m[3] = 0x6C345908;
            if (SendMsg(who, 0xC14, m) == 0) {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        if (who != 0xFFFFFFFF) {
            int m[4];
            m[1] = arg;
            m[2] = 0x4B000000;
            m[3] = 0x6C345908;
            if (SendMsg(who, 0xC14, m) == 0) {
                return 0;
            }
        } else {
            return 0;
        }
    }
    if (who >> 24 == 2 || who >> 24 == 0x13) {
        int mode = fn_802A1D04();
        if (mode == 1 || mode == 2) {
            int slot = PlayerSlotOf(who);
            int team;
            if (slot < 0 || gMinigame.count <= slot) {
                team = -1;
            } else {
                team = gMinigame.team[slot];
            }
            if (team < 8 && team >= 0) {
                found = fn_800B8B64(0, 0, 1, (void*)MinigameIsArenaMap, 0);
            }
        } else if (fn_802A1D04() == 3) {
            int info[8];
            SendMsg(who, 0x400, info);
            if ((info[6] & 2) == 0) {
                found = fn_800B8D68(0, 0x612B26A, 1);
            } else {
                found = fn_800B8D68(0, 0xBAA51EE3, 1);
            }
        }
    }
    pick = found;
    if (found == 0xFFFFFFFF) {
        int holder = GetVarInt(who, 0x7C18FE9E);
        int ok = 1;
        if (ok) {
            int back = GetVarInt(holder, 0x7C18FE9E);
            int mode = GetVarInt(holder, 0x040EFE5F);
            if (back == (int)who) {
                pick = holder;
            } else if (mode == 0) {
                int alt = GetVarInt(holder, 0x03D3B158);
                pick = found;
                if (alt != 0) {
                    pick = fn_800B8D68(0, alt, 1);
                }
            } else {
                pick = fn_800B8D68(mode, 0, 1);
            }
        }
    }
    if (pick == 0xFFFFFFFF) {
        return 0;
    }
    {
        char info[0x44];
        char mtx[0x24];
        float v[3];
        int msg[4];
        SendMsg(pick, 0x402, info);
        msg[3] = 3;
        fn_8009F190(v, mtx);
        VecSwapXY2(v, v);
        SendMsg(who, 0xC07, msg);
        SetVarInt(who, 0x7C18FE9E, pick);
        SetVarInt(pick, 0x7C18FE9E, who);
        SetVarInt(pick, 0x33032FA9, 1);
        MinigameStreakUp(who);
        return 1;
    }
}

// Is the current level one of the four deathmatch-style maps?
int MinigameIsArenaMap(void) {
    int id = fn_800B705C();
    if (id != 0xD5B0588) {
        if (id < 0xD5B0588) {
            if (id != -0x5B49449E && (id > -0x5B49449F || id != -0x5D3DBB55)) {
                return 0;
            }
        } else if (id != 0x5BE2B379) {
            return 0;
        }
    }
    return 1;
}

// Awards a "kill streak" step to the player `handle` when the match counters allow it.
void MinigameStreakUp(u32 handle) {
    if (handle >> 24 == 0x13) {
        if (PlayerSlotOf(handle) == -1) {
            int streak = GetVarInt(0x4B000000, 0x82A1DD29);
            if (streak < 3) {
                int enabled = GetVarInt(0x4B000000, 0xD199C84A);
                if (enabled != 0) {
                    int level = 0;
                    int r = Rand();
                    int q = r / 100 + (r >> 31);
                    if (r + (q - (q >> 31)) * -100 < gStatThreshold) {
                        int roll;
                        r = Rand();
                        q = r / 100 + (r >> 31);
                        roll = r + (q - (q >> 31)) * -100;
                        if (gStatHigh < roll) {
                            level = 9;
                        } else if (gStatMid < roll) {
                            level = 6;
                        } else {
                            level = 3;
                        }
                        SetVarInt(handle, 0x82A1DD29, level);
                        SetVarInt(0x4B000000, 0x82A1DD29, streak + 1);
                    }
                    StatSet(0x7BFB940D, 3, handle & 0xFFFFFF, (float)level);
                }
            }
        }
    }
}

// Takes back one streak step from the player `handle`.
void MinigameStreakDown(u32 handle) {
    if (handle >> 24 == 0x13) {
        if (PlayerSlotOf(handle) == -1) {
            int streak = GetVarInt(0x4B000000, 0x82A1DD29);
            if (streak != 0) {
                int enabled = GetVarInt(0x4B000000, 0xD199C84A);
                if (enabled != 0) {
                    int mine = GetVarInt(handle, 0x82A1DD29);
                    if (mine != 0) {
                        SetVarInt(0x4B000000, 0x82A1DD29, streak - 1);
                    }
                }
            }
        }
    }
}

// Name string for player slot `i`.
char* fn_800FD334(int i) {
    sprintf(gTeamNameBuf, "%s", gMinigame.name[i]);
    return gTeamNameBuf;
}

u32 MinigameFlagB(int i) {
    u32 v = gMinigame.flags130 & 4 << i;
    return (-v | v) >> 31;
}

u32 MinigameFlagA(int i) {
    u32 v = gMinigame.flags12C & 4 << i;
    return (-v | v) >> 31;
}

int fn_800FD3DC(void) {
    return gMinigame.unkC4;
}

int fn_800FD3EC(void) {
    return gMinigame.count;
}

char* fn_800FD3FC(int i) {
    if (i < 0 || i >= 8) {
        i = 8;
    }
    return gTeamColors[i];
}

char* fn_800FD424(int i) {
    if (i < 0 || i >= 8) {
        i = 8;
    }
    return gTeamColors2[i];
}

// English name of player colour `c`.
const char* fn_800FD44C(int c) {
    switch (c) {
    case 0:
        return "Blue";
    case 1:
        return "Red";
    case 2:
        return "Green";
    case 3:
        return "Yellow";
    case 4:
        return "Brown";
    case 5:
        return "Orange";
    case 6:
        return "Purple";
    case 7:
        return "Pink";
    default:
        return "INVALID";
    }
}

// Localised name of player colour `c` (the first four come from the string table).
const char* ColorNameLocal(int c) {
    const char* r;
    switch (c) {
    case 0:
        r = StrFindKeySkip(0x9AD441C8);
        break;
    case 1:
        r = StrFindKeySkip(0xBAAC3D43);
        break;
    case 2:
        r = StrFindKeySkip(0xBFFCEB35);
        break;
    case 3:
        r = StrFindKeySkip(0x5DA3CE62);
        break;
    case 4:
        r = "Brown";
        break;
    case 5:
        r = "Orange";
        break;
    case 6:
        r = "Purple";
        break;
    case 7:
        r = "Pink";
        break;
    default:
        r = "INVALID";
    }
    return r;
}

// Localised team name for colour `c`.
const char* fn_800FD5C4(int c) {
    const char* r;
    switch (c) {
    case 0:
        r = StrFindKeySkip(0x4F578E74);
        break;
    case 1:
        r = StrFindKeySkip(0xA02887D9);
        break;
    case 2:
        r = StrFindKeySkip(0x4030C127);
        break;
    case 3:
        r = StrFindKeySkip(0xE329D21A);
        break;
    case 4:
        r = "Brown Team";
        break;
    case 5:
        r = "Orange Team";
        break;
    case 6:
        r = "Purple Team";
        break;
    case 7:
        r = "Pink Team";
        break;
    default:
        r = "INVALID";
    }
    return r;
}

// Team of player slot `i` (-1 when out of range).
int PlayerTeam(int i) {
    if (i >= 0 && i < gMinigame.count) {
        return gMinigame.team[i];
    }
    return -1;
}

void MinigameSetTeam(int i, int team) {
    if (i < 0) {
        return;
    }
    if (gMinigame.count <= i) {
        return;
    }
    gMinigame.team[i] = team;
}

// Handle of player slot `i` (-1 when out of range).
int fn_800FD6EC(int i) {
    if (i >= 0 && i < gMinigame.count) {
        return gMinigame.handle[i];
    }
    return -1;
}

// Finds the slot of player `handle`; remaps a possessed host to its possessor.
int HandleToPlayer(int handle) {
    return PlayerSlotOf(handle);
}

// Removes the (non-local) player `handle` from the player list.
void MinigameRemovePlayer(int handle) {
    int i = 0;
    int slot = -1;
    int* p = gMinigame.handle;
    int n = gMinigame.count;
    if (n > 0) {
        do {
            slot = i;
            if (*p == handle) {
                break;
            }
            p++;
            i++;
            n--;
            slot = -1;
        } while (n != 0);
    }
    if (slot != -1 && ActorCount() <= slot) {
        gMinigame.count--;
        gMinigame.handle[slot] = -1;
    }
}

// Adds a bot player with handle `handle`; returns its slot (-1 when full).
int MinigameAddBot(int handle) {
    int slot;
    if (gMinigame.count < 8) {
        slot = gMinigame.bots + ActorCount();
        gMinigame.bots++;
        gMinigame.handle[slot] = handle;
        sprintf(gMinigame.name[slot], StrFindKeySkip(0x32405BBF), (slot - ActorCount()) + 1);
    } else {
        slot = -1;
    }
    return slot;
}

// Calls the current game type's update hook while the engine is running.
void fn_800FD924(int arg) {
    if (gMinigame.state == 1 && gMinigame.type > -1 && gMinigame.type < 6 &&
        gGameTypes[gMinigame.type] != NULL && gGameTypes[gMinigame.type]->draw != NULL) {
        gGameTypes[gMinigame.type]->draw(&gMinigame, arg);
    }
}

void fn_800FD998(void) {
    if (gMinigame.state == 1 && gMinigame.type > -1 && gMinigame.type < 6 &&
        gGameTypes[gMinigame.type] != NULL && gGameTypes[gMinigame.type]->update != NULL) {
        gGameTypes[gMinigame.type]->update();
    }
}

// Shuts the minigame engine down.
void fn_800FDA10(void) {
    fn_800872BC(0x40000, "Minigame : Terminating minigame engine");
    if (gMinigame.type > -1 && gMinigame.type < 6 && gGameTypes[gMinigame.type] != NULL &&
        gGameTypes[gMinigame.type]->term != NULL) {
        gGameTypes[gMinigame.type]->term();
    }
    fn_80128C8C();
    fn_80140984();
}

// Restarts the current game: rebuilds the player list and runs the type's reset / restart hooks.
void fn_800FDA90(void) {
    int n;
    int i;
    int* hp;
    char* np;
    gMinigame.state = 1;
    fn_801289D0(gGameTypes[gMinigame.type]);
    n = ActorCount();
    gMinigame.count = gMinigame.bots + n;
    hp = gMinigame.handle;
    np = gMinigame.name[0];
    for (i = 0; i < n; i++) {
        hp[i] = (i & 0xFFFFFF) | 0x2000000;
        SetVarInt(hp[i], 0x99E99508, i);
        sprintf(np, StrFindKeySkip(0x1B149A29), i + 1);
        np += 0x10;
    }
    HudRefreshSlots(&gMinigame);
    if (gMinigame.type > -1 && gMinigame.type < 6 && gGameTypes[gMinigame.type] != NULL &&
        gGameTypes[gMinigame.type]->reset != NULL) {
        gGameTypes[gMinigame.type]->reset();
    }
    fn_800872BC(0x40000, "Minigame : Reseting current game");
    if (gMinigame.type > -1 && gMinigame.type < 6 && gGameTypes[gMinigame.type] != NULL &&
        gGameTypes[gMinigame.type]->restart != NULL) {
        gGameTypes[gMinigame.type]->restart();
    }
}

void fn_800FDC48(void) {
    if (gMinigame.type == 4) {
        fn_8014C858();
    }
}

// Starts the minigame engine: registers the message handlers and picks the game type.
void fn_800FDC78(void) {
    fn_800872BC(0x40000, "Minigame : Minigame engine intializing");
    fn_80128D7C();
    fn_80026660(0x4B, (void*)fn_800FDDE4, 0);
    fn_80026660(0x52, (void*)MinigameMsgB, 0);
    fn_800094D8(0xB, gMinigameLevel);
    gMinigame.state = 2;
    gMinigame.bots = 0;
    gMinigame.count = 0;
    gMinigame.type = SysCheckMemory();
    fn_802A1D0C();
    if (gMinigame.type > -1 && gMinigame.type < 6 && gGameTypes[gMinigame.type] != NULL &&
        gGameTypes[gMinigame.type]->init != NULL) {
        gGameTypes[gMinigame.type]->init();
    }
    fn_802D1D7C(1);
    fn_8027D3E4(gMinigameZoom);
    if (gMinigame.type == 0) {
        EmitterSetMode(0);
    } else {
        EmitterSetMode(1);
    }
}

// Message handler (type 0x52): message 0x1002 with flag 1 tells the HUD to refresh.
int MinigameMsgB(u32 handle, int msg, int* data) {
    int reply[2];
    if (msg == 0x1002 && data[1] == 1) {
        reply[0] = 0;
        reply[1] = 0;
        SendMsg(0x38000000, 0x5600, reply);
    }
    return 0;
}

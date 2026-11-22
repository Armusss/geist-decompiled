// u_800FA004.cpp (0x800FA004-0x800FC2F4): the team base actors (coloured spawn pads).
// Split from the old u_800F73C8 (0x800F73C8-0x800FDDE4). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary evidence (split 2026-11-22): own .sdata2 0x804D74A0-0x804D74F0 (0.0f, 0x4330 double at 0x804D74B0, -1.0f),
// .sbss 0x804D4BA0-0x804D4BD8, .sdata 0x804D2D78-0x804D2D88, .data 0x8039CF68 / 0x8039CFE8 / 0x8039E6E8 / 0x8039E70C
// and .rodata 0x80345388. The minigame file after it has its own 0x4330 double (0x804D74F0).


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

// ---- team base actors (coloured spawn pads) ---------------------------------------------

struct TeamBaseData {
    float timer;            // 0x00: time until the next colour roll
    int team;               // 0x04: current team colour (-1 = neutral)
    int owner;              // 0x08
    int slot[6];            // 0x0C ids of the emitters attached
    u8 pad24[0x5AC - 0x24];
    int active;             // 0x5AC
    float cooldown;         // 0x5B0
};

struct TeamBase {
    u8 pad00[0x38];
    int handle;             // 0x38
    u8 pad3C[0x84 - 0x3C];
    int** desc;             // 0x84: descriptor (first word = placement hash)
    u8 pad88[4];
    int id;                 // 0x8C
    int model;              // 0x90
    u8 pad94[0x9C - 0x94];
    int bone;               // 0x9C
    u8 padA0[4];
    TeamBaseData* data;     // 0xA4
};

extern "C" int AssetFind(u32 a, u32 b);
extern "C" int Rand(void);
u32 MinigameGetRule(int id);
char* TeamColorB(int i);
int fn_800FBE88(TeamBase* a, int* m);
extern int gTeamNodeTop[8];     // 0x804D4BA0 .. 0x804D4BBC: model nodes of the team colours
extern int gTeamBaseAllowed;    // 0x804D4BC4
extern int gTeamBaseRandom;     // 0x804D4BC8
extern int gTeamRollMs;         // 0x804D2D80
extern float gTeamZero;         // 0x804D74A0
extern float gTeamRollScale;    // 0x804D74A4 (1000.0)
extern double gTeamColorScale;  // 0x804D74A8 (255.0)
extern float gTeamNone;         // 0x804D74B8 (-1.0)
extern char gTeamDefaultColor[16];  // 0x80345388
extern char gTeamNames[];       // 0x8039CF68 (name table, "PH_Blue", ...)
extern char gTeamSplineBase[];  // 0x8039CFE8
extern int gTeamSplineLen;      // 0x804D2D7C
extern "C" {
void EmitterDestroy(int id);                                        // 0x800D3694
int SplineAssetResolve(char* names, int n, int i);                  // 0x80101FC8
void SplineBitClearSpan(void* a, void* b, int c, void* d);          // 0x801020B4
void SplineClearNameHashes(char* names, int n);                     // 0x80101F3C
void* SplineSlotAlloc(void* base, int len, void* d, int n);        // 0x801020E0
void fn_800913EC(float dt, int a, void* out);
int ModelAnimate(int model, int bone, int a);                       // 0x8003AA9C
void AnimApply(int a, int b, int c, int d);                         // 0x8008F630
void ModelSetMatrix(int model, void* m, int bone);                  // 0x8003A66C
int ActorDefaultMsg(int a, int msg, void* data);                    // 0x800E4C64
int fn_800E6444(void* a);
void fn_80037EC0(int model, int node, int a, int r, int g, int b, int al);
void ModelNodeSetup(int model, int node, int a, int b);
void fn_800FB630(TeamBase* a);
void ActorUpdate(float dt, void* a);
}

// Hides the team colour nodes and clears the spline slots when the pad is destroyed.
void TeamBaseDestroy(TeamBase* a) {
    TeamBaseData* d = a->data;
    int i;
    if (d->active == 1) {
        for (i = 0; i < 6; i++) {
            EmitterDestroy(d->slot[i]);
            d->slot[i] = -1;
        }
        for (i = 0; i < 8; i++) {
            ModelNodeSetup(a->model, gTeamNodeTop[i], 0, 1);
        }
        for (i = 0; i < 8; i++) {
            if (i == d->team) {
                ModelNodeSetup(a->model, SplineAssetResolve(gTeamNames, 8, i), 1, 1);
            }
        }
    }
    SplineBitClearSpan(a->data, gTeamSplineBase, 0x5B4, &gTeamSplineLen);
    SplineClearNameHashes(gTeamNames, 8);
}

static inline void TeamBaseColorNodes(TeamBase* a, int team) {
    char* c = (team < 0) ? gTeamDefaultColor : TeamColorB(team);
    float* f = (float*)c;
    fn_80037EC0(a->model, AssetFind(0xD9044CBB, 0x10B6E46B), 1, (int)(255.0 * f[0]) & 0xFF,
                (int)(255.0 * f[1]) & 0xFF, (int)(255.0 * f[2]) & 0xFF, (int)(255.0 * f[3]) & 0xFF);
    fn_80037EC0(a->model, AssetFind(0xD9044CBB, 0xFF1A2296), 1, (int)(255.0 * f[0]) & 0xFF,
                (int)(255.0 * f[1]) & 0xFF, (int)(255.0 * f[2]) & 0xFF, (int)(255.0 * f[3]) & 0xFF);
}

// Per-frame update of a team pad: follows its bone, shows the active colour, and rolls a new
// colour when the timer runs out.
void TeamBaseUpdate(float dt, TeamBase* a) {
    TeamBaseData* d = a->data;
    int allowed = GetVarInt(0x4B000000, 0xF90653CA);
    int mask = GetVarInt(0x4B000000, 0x67FFEA96);
    char matBuf[0x34];
    float anim[3];
    int boneAnim;
    fn_800913EC(dt, 0, matBuf);
    boneAnim = ModelAnimate(a->model, a->bone, 1);
    ((float*)(&a->bone))[0] = ((float*)(&a->bone))[0];
    AnimApply(0, a->bone, boneAnim, a->id);
    if (d->active == 1) {
        int i;
        for (i = 4; i < 8; i++) {
            ModelNodeSetup(a->model, gTeamNodeTop[i], 1, 1);
        }
        d->cooldown -= dt;
        if (d->cooldown <= gTeamZero) {
            for (i = 0; i < 6; i++) {
                EmitterDestroy(d->slot[i]);
                d->slot[i] = -1;
            }
            for (i = 0; i < 8; i++) {
                ModelNodeSetup(a->model, gTeamNodeTop[i], 0, 1);
            }
            for (i = 0; i < 8; i++) {
                if (i == d->team) {
                    ModelNodeSetup(a->model, SplineAssetResolve(gTeamNames, 8, i), 1, 1);
                }
            }
            d->active = 0;
            d->owner = 0;
        }
    }
    if (gTeamBaseAllowed != 0 && d->owner == 0 && d->active == 0) {
        d->timer -= dt;
        if (d->timer <= gTeamZero) {
            d->timer = gTeamRollScale * (float)gTeamRollMs;
            if (gTeamBaseRandom == 0) {
                do {
                    d->team = (d->team + 1) & 7;
                } while ((mask & 4 << d->team) == 0);
            } else {
                int r = Rand();
                d->team = r % mask;
            }
            if (MinigameGetRule(10) == 0) {
                d->team = -1;
            } else {
                int team = d->team;
                TeamBaseData* dd = a->data;
                if (team < 0 || MinigameGetRule(10) == 0) {
                    team = -1;
                }
                TeamBaseColorNodes(a, team);
                dd->team = team;
                {
                    int reply[8];
                    SendMsg(a->id, 0x41A, reply);
                    if (reply[1] != -1) {
                        SetVarInt(reply[1], 0x9BD4EF06, d->team);
                    }
                }
            }
        }
    }
    ActorUpdate(dt, a);
}

// Message handler of a team pad: answers the team queries (0x41E) and the colour flags (0x400).
int TeamBaseMsg(int self, int msg, int* m) {
    TeamBase* a = (TeamBase*)m[0];
    if (msg == 0x41E) {
        int key = m[1];
        if (key == 0x5C8F66F9) {
            m[3] = 0x1AF27550;
            return 1;
        }
        if (key == -0x642B10FA) {
            m[3] = a->data->team;
            return 1;
        }
        if (key == 0x5ED5BF20) {
            switch (a->data->team) {
            case 0: m[3] = 0x188FEED7; break;
            case 1: m[3] = -0x1E62EE0; break;
            case 2: m[3] = 0x704EDEE; break;
            case 3: m[3] = 0x5FDBBEFD; break;
            case 4: m[3] = 0x891A39B; break;
            case 5: m[3] = -0x5E6FFE7D; break;
            case 6: m[3] = -0x3BE2174B; break;
            case 7: m[3] = -0x6B6410C5; break;
            default: m[3] = 0x74C335AA; break;
            }
            return 1;
        }
        return 0;
    }
    if (msg < 0x41E) {
        if (msg == 0x400) {
            TeamBaseData* d = a->data;
            ActorDefaultMsg(a->handle, 0x400, m);
            m[6] = 2;
            switch (d->team) {
            case 0: m[6] |= 4; break;
            case 1: m[6] |= 8; break;
            case 2: m[6] |= 0x10; break;
            case 3: m[6] |= 0x20; break;
            case 4: m[6] |= 0x40; break;
            case 5: m[6] |= 0x80; break;
            case 6: m[6] |= 0x100; break;
            case 7: m[6] |= 0x200; break;
            default: m[6] |= 0x200; break;
            }
            return 1;
        }
        if (msg < 0x400) {
            if (msg == 4) {
                return 0;
            }
        } else if (msg > 0x41C) {
            return fn_800FBE88(a, m);
        }
    } else {
        if (msg == 0xC14) {
            if (m[3] == 0x44A2E959) {
                int reply[4];
                reply[1] = a->id;
                reply[2] = m[1];
                reply[3] = 0x7C3F4E3E;
                return SendMsg(0x4B000000, 0xC14, reply);
            }
            if (m[3] == -0x14611E68) {
                a->data->owner = m[1];
                return 1;
            }
            return 0;
        }
        if (msg < 0xC14 && msg == 0xC00) {
            int reply[4];
            if (m[1] == 1) {
                reply[1] = a->id;
                reply[2] = 0x2840341C;
                reply[0] = 1;
                SendMsg(0x53000000, 0xC14, reply);
            } else {
                reply[1] = a->id;
                reply[2] = 0xEB4A0875;
                reply[0] = 1;
                SendMsg(0x53000000, 0xC14, reply);
            }
            return ActorDefaultMsg(a->handle, 0xC00, m);
        }
    }
    return ActorDefaultMsg(self, msg, m);
}

extern float gTeamInitTimer;    // 0x804D74B8

// Creates a team pad: allocates its data, hides the colour nodes, picks the team colour from
// the placement name and announces itself to the minigame.
int TeamBaseCreate(TeamBase* a) {
    static int sInit = 0;               // 0x804D4BCC
    TeamBaseData* d;
    if (sInit == 0) {
        SetVarInt(0x4B000000, 0xE90B5145, 1);
        SetVarInt(0x4B000000, 0x78F28F77, 0);
        SetVarInt(0x4B000000, 0xF0FB92DF, 7);
        sInit = 1;
    }
    if (fn_800E6444(a) == 0) {
        return 0;
    }
    d = (TeamBaseData*)SplineSlotAlloc(gTeamSplineBase, 0x5B4, &gTeamSplineLen, 4);
    if (d == NULL) {
        return 0;
    }
    d->timer = gTeamNone;
    a->data = d;
    ModelNodeSetup(a->model, AssetFind(0xD9044CBB, 0x8295241), 0, 1);
    ModelNodeSetup(a->model, AssetFind(0xD9044CBB, 0x8CA6B9AA), 0, 1);
    ModelNodeSetup(a->model, AssetFind(0xD9044CBB, 0xC9F6A49B), 0, 1);
    ModelNodeSetup(a->model, AssetFind(0xD9044CBB, 0xFD49E9DC), 0, 1);
    {
        int hash = **a->desc;
        int team = -2;
        if (hash == 0x2D9BAFB7) {
            team = 2;
        } else if (hash == 0x19D87129) {
            team = 1;
        } else if (hash == 0xC1D3654) {
            team = 3;
        } else if (hash == 0x74A54FEE) {
            team = 0;
        }
        if (team != -2) {
            TeamBaseData* dd = a->data;
            if (MinigameGetRule(10) == 0) {
                team = -1;
            }
            TeamBaseColorNodes(a, team);
            dd->team = team;
        }
    }
    fn_800FB630(a);
    d->cooldown = 0.0f;
    {
        int msg[4];
        msg[1] = a->id;
        msg[2] = 0x901EEFA2;
        msg[0] = 0x1AF27550;
        SendMsg(0x4B000000, 0xC14, msg);
        msg[1] = a->id;
        msg[2] = 0x2840341C;
        msg[0] = 1;
        SendMsg(0x53000000, 0xC14, msg);
    }
    return 1;
}

extern float gTeamActiveMs;     // 0x804D74BC (3000.0)
extern float gTeamAnimSpeed;    // 0x804D74C0 (1.0)
extern int gTeamAnimId;         // 0x804D4BC0
extern "C" {
void fn_80090390(int a, int b);
void fn_80090B74(float f, int a, int b, int c, int d);
int EmitterCreate(void* desc);                                      // 0x800D3D88
}

// Switches the pad on: lights the node of the current team colour and creates the six
// emitters that spray it.
void TeamBaseActivate(TeamBase* a) {
    TeamBaseData* d = a->data;
    int i;
    int team;
    char* e;
    for (i = 0; i < 8; i++) {
        if (i == d->team) {
            ModelNodeSetup(a->model, SplineAssetResolve(gTeamNames, 8, i), 0, 1);
        }
    }
    team = d->team;
    if (team == 2) {
        ModelNodeSetup(a->model, gTeamNodeTop[2], 1, 1);
        ModelNodeSetup(a->model, gTeamNodeTop[6], 1, 1);
    } else if (team < 2) {
        if (team == 0) {
            ModelNodeSetup(a->model, gTeamNodeTop[0], 1, 1);
            ModelNodeSetup(a->model, gTeamNodeTop[4], 1, 1);
        } else if (team >= 0) {
            ModelNodeSetup(a->model, gTeamNodeTop[1], 1, 1);
            ModelNodeSetup(a->model, gTeamNodeTop[5], 1, 1);
        } else {
            ModelNodeSetup(a->model, gTeamNodeTop[3], 1, 1);
            ModelNodeSetup(a->model, gTeamNodeTop[7], 1, 1);
        }
    } else {
        ModelNodeSetup(a->model, gTeamNodeTop[3], 1, 1);
        ModelNodeSetup(a->model, gTeamNodeTop[7], 1, 1);
    }
    d->cooldown = gTeamActiveMs;
    fn_80090390(*(int*)((char*)a + 0x110), 0);
    fn_80090B74(gTeamAnimSpeed, *(int*)((char*)a + 0x110), 0, gTeamAnimId, 0);
    e = (char*)d;
    for (i = 0; i < 6; i++) {
        switch (d->team) {
        case 2:
            e[0xF0] = 0x31;
            e[0xF1] = (char)0xE2;
            e[0xF2] = 0x49;
            break;
        case 0:
            e[0xF0] = 0x33;
            e[0xF1] = 0x36;
            e[0xF2] = (char)0xE2;
            break;
        case 1:
            e[0xF0] = (char)0xD5;
            e[0xF1] = 0x22;
            e[0xF2] = 0x23;
            break;
        case 3:
            e[0xF0] = (char)0xF0;
            e[0xF1] = (char)0xF3;
            e[0xF2] = 0x5F;
            break;
        }
        d->slot[i] = EmitterCreate(e + 0x24);
        e += 0xEC;
    }
}

// u_80222C08.cpp: rivet puzzle actor, camera tunables and player state callbacks
// (0x80222C08-0x80226DE8).
// The first function (0x80222C08) is the BeeBase deleting destructor of the Bees unit
// (u_80221100) and is only declared here (TODO: move the boundary); the array helpers that
// follow it (0x80222C70-0x80222D40), the huge 0x80222D60 and the unit's tail (0x80226E78 on) are
// not drafted.  Then the rivet actor (state, messages, variable set, create / init / destroy),
// the camera tunables (the table at 0x8049EE68 with its message setter and per-frame ramp update)
// and a set of small player state callbacks (enter / exit / update for the climb, hang and
// rivet states).  Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

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

struct Mtx34 {
    float m[12];
};

float sqrtf(float x);
extern "C" {
int SendMsg(int to, int msg, void* data);
void* memset(void* p, int c, u32 n);
void* VAlloc(int size);
void VFree(void* p);
int AssetFind(u32 a, u32 b);
int fn_80019F88(void* m, void* actor);
int PlayerInHost(void* player);
int ActorPlayAnim(int handle, u32 hash, int arg);
int ActorDefaultMsg(void* a, int msg, void* data, int arg);
void fn_802A2078(float f);
void fn_802A2054(float f);
void* ActorGet(int i);
void fn_802A209C(int mode);
int fn_80049940(void);
void PlayerResetGunTimer(void* p);
void PlayerSetValue4(void* p, int v);
void PlayerSetActionMode(void* p, int mode);
void PlayerSetHeldSlotIndex(void* p, int v);
int PlayerGetHeldSlotIndex(void* p);
void PlayerSetHeldSlotMode(void* p, int v);
void PlayerSelectCollisionUpdate(float dt, void* p);
void fn_800769A0(float dt, void* p);
int fn_800A384C(void* o);
int fn_800A1BC8(int a, int b, int c, int d, int e);
void fn_80155A84(void* a, void* b, int c);
int fn_80175EFC(void);
void fn_80050EB0(void* p);
void fn_80227594(void);
void fn_802272D4(void* p);
void fn_80226E78(void* p);
void fn_80212730(int id);
void MarkLookupEntryFlag(int id);
int OwnerGetInfo(u32 id);
void fn_8009F63C(void* m, int a);
void fn_8009F44C(void* out, void* m, void* in);
int fn_800E6444(void* a);
void ModelNodeSetup(int model, int node, int a, int b);
int ModelFindBone(int model, int bone);
void fn_80038474(int model, int node, int a, int b);
}

int HasCondition(void* conds, int bit);

// Variable query: the reply word is the first word of the string.
inline int QueryVar(int target, u32 hash) {
    QueryMsg m;
    *(int*)&m.result = 0;
    m.hash = hash;
    m.unk8 = 0;
    SendMsg(target, 0x41E, &m);
    return *(int*)&m.result;
}

// ---- BeeBase deleting destructor (belongs to the Bees unit, u_80221100) -------------------
// 0x80222C08: not drafted here.

// ---- rivet puzzle actor ------------------------------------------------------------------

// Rivet state (Actor + 0xA4, 0xF8 bytes).
struct RivetData {
    int placed[9];          // 0x00 required order of the rivet ids
    int count;              // 0x24
    int cursor;             // 0x28: index into `placed` being asked for
    int have[9];            // 0x2C per slot: bone exists
    float glow[9];          // 0x50 glow fade timers
    int mode;               // 0x74: selected rivet
    int result;             // 0x78: tick interval
    int state;              // 0x7C
    int unk80;              // 0x80: free-play flag
    float timer;            // 0x84
    int unk88;              // 0x88: player id
    float saved[12];        // 0x8C: start placement
    float euler[9];         // 0xBC: target rotation
    float vec[3];           // 0xE0: target position
    float duration;         // 0xEC
    float elapsed;          // 0xF0
    int outcome;            // 0xF4
};

struct RivetActor {
    u8 pad00[0x38];
    int handle;             // 0x38
    u8 pad3C[0x88 - 0x3C];
    int unk88;              // 0x88
    u8 pad8C[4];

    int model;              // 0x90
    u8 pad94[0xA4 - 0x94];
    RivetData* data;        // 0xA4
};

extern float gRivetZero;        // 0x804D95E0

extern int gRivetBusyFlag;      // 0x804D5400
extern float gRivetT0;          // 0x804D95E4 (1000.0)
extern float gRivetT1;          // 0x804D95E8 (500.0)
extern float gRivetOne;         // 0x804D95EC
extern float gRivetT2;          // 0x804D95F0 (1300.0)
extern float gRivetT3;          // 0x804D95F4 (600.0)
extern float gRivetT4;          // 0x804D95F8 (900.0)
extern float gRivetT5;          // 0x804D95FC (2000.0)
extern float gRivetFadeK;       // 0x804D9600 (-255.0)
extern "C" {
void* PlayerGetPossessor(int i);
int PadPressed(int pad, int kind, int button);
void PlayerSetHeldSlotMode(void* p, int v);
void PlayerResetGunTimer(void* p);
void fn_8006811C(void* p, u32 hash);
int TimerAdd(float t, void* fn, void* arg, float f);
int fn_800498D0(void* p);
void ActorUpdate(float dt, void* a);
void fn_800A0AC4(float* q, void* m);
void fn_800A0EF0(void* m, float* q);
void MtxGetEuler(float* e, void* m);
void C_QUATSlerp(float* a, float* b, float t, float* out);
void fn_8009FAC0(void* m, float* e);
int ActorPlayAnim(int handle, u32 hash, int arg);
int Fn800F7DEC(int a, int b, int c, int d, void* e);
void fn_80037EC0(int model, int node, int a, int r, int g, int b, int al);
void ModelNodeSetup(int model, int node, int a, int b);
int FxStart(int a, int b, int c);
void PlacementToMatrix(void* a, void* m);
void fn_8009ED70(void* out, void* in);
void fn_800A0328(void* m);
int fn_8001F0DC(void* a, int* pos, u32 mask, int b, int c, int d, int* out, int e);
void fn_8016D440(float* out, float* v);
}
double RivetPlayerBusy(void* player);
int RivetSelect(RivetData* d, RivetActor* a);

// Shows rivet node `i` hidden/lit-up setup used when a round ends.
static inline void RivetNodesReset(RivetData* d, RivetActor* a) {
    float zero = gRivetZero;
    int i;
    for (i = 0; i < 9; i++) {
        int node = AssetFind(0xD9044CBB, i + 0x732D446F);
        if (node >= 0) {
            fn_80037EC0(a->model, node, 1, 0xFF, 0xFF, 0xFF, 0xFF);
            ModelNodeSetup(a->model, node, 0, 0);
        }
        d->glow[i] = zero;
    }
}

// Releases the controlled player after a round: sets its hold mode and schedules the busy flag.
static inline void RivetReleasePlayer(char* pl) {
    PlayerSetHeldSlotMode(pl, 9);
    fn_8006811C(pl, 0x1F309F82);
    TimerAdd(gRivetT0, (void*)RivetPlayerBusy, pl, 0.0f);
}

// Eases the controlled player toward the target pose; returns 0 when finished.
static inline int RivetEasePlayer(RivetData* d, float dt) {
    float dur = d->duration;
    if (d->elapsed == dur) {
        return 0;
    } else {
        float t = d->elapsed + dt;
        char* pl;
        if (t < dur) {
            dur = t;
        }
        d->elapsed = dur;
        pl = (char*)PlayerGetPossessor(d->unk88);
        if (pl == NULL) {
            return 0;
        }
        {
            float k = gRivetOne;
            float qa[4];
            float qb[4];
            float qc[4];
            float m[12];
            float e[3];
            float r = d->elapsed / d->duration;
            float inv;
            if (r < k) {
                k = r;
            }
            *(float*)(pl + 0x650) = gRivetZero;
            *(float*)(pl + 0x654) = gRivetZero;
            *(float*)(pl + 0x658) = gRivetZero;
            inv = gRivetOne - k;
            fn_800A0AC4(qa, d->saved);
            fn_800A0AC4(qb, d->euler);
            C_QUATSlerp(qa, qb, k, qc);
            *(float*)(pl + 0x67C) = d->saved[9] * inv + d->vec[0] * k;
            *(float*)(pl + 0x680) = d->saved[10] * inv + d->vec[1] * k;
            *(float*)(pl + 0x684) = d->saved[11] * inv + d->vec[2] * k;
            fn_800A0EF0(m, qc);
            MtxGetEuler(e, m);
            *(float*)(pl + 0x1F0) = e[0];
            *(float*)(pl + 0x690) = e[2];
            fn_8009FAC0(m, (float*)(pl + 0x688));
            m[9] = *(float*)(pl + 0x67C);
            m[10] = *(float*)(pl + 0x680);
            m[11] = *(float*)(pl + 0x684);
            PlacementToMatrix(pl, m);
            return 1;
        }
    }
}

// Starts the pose ease toward rivet `idx` (state 1).
static inline void RivetStartEase(RivetData* d, RivetActor* a, char* pl, int idx, int fromNode,
                                  int toNode) {
    float from[12];
    float to[12];
    QueryMsg q;
    float delta[3];
    int fb[3];
    int hit;
    float fl;
    q.unk8 = 0;
    q.hash = AssetFind(0xF1754BF2, fromNode);
    if (SendMsg(a->handle, 0x411, &q) == 0) {
        fn_800A0328(from);
    } else {
        fn_8009ED70(from, &q);
    }
    q.hash = AssetFind(0xF1754BF2, toNode);
    if (SendMsg(a->handle, 0x411, &q) == 0) {
        fn_800A0328(to);
    } else {
        fn_8009ED70(to, &q);
    }
    fb[0] = (int)to[9];
    fb[1] = (int)to[10];
    fb[2] = (int)to[11];
    fl = (float)fn_8001F0DC(a, fb, 0x80000000, 0, 0, 0, &hit, 0);
    if (hit != 0) {
        to[11] = fl + *(float*)(pl + 0x64C) + (*(float*)(pl + 0x1EC) - (float)*(int*)(pl + 0x2C));
    }
    delta[0] = from[9] - to[9];
    delta[1] = from[10] - to[10];
    delta[2] = from[11] - to[11];
    fn_8016D440(to, delta);
    fn_8009FAC0(d->euler, to);
    d->vec[0] = to[9];
    d->vec[1] = to[10];
    d->vec[2] = to[11];
    d->duration = gRivetT1;
    d->elapsed = gRivetZero;
    {
        int i;
        for (i = 0; i < 12; i++) {
            d->saved[i] = *(float*)(pl + 0x1C0 + i * 4);
        }
        d->saved[9] = (float)*(int*)(pl + 0x24);
        d->saved[10] = (float)*(int*)(pl + 0x28);
        d->saved[11] = (float)*(int*)(pl + 0x2C);
    }
    (void)idx;
}

// Main update of the rivet puzzle: drives the round state machine (show the pattern, wait for
// the player's input, judge it) and fades the rivet glows.
void RivetUpdate(float dt, RivetActor* a) {
    RivetData* d = a->data;
    int i;
    if (d != NULL) {
        char* pl;
        switch (d->state) {
        case 1:
            pl = (char*)PlayerGetPossessor(d->unk88);
            if (pl != NULL) {
                fn_8006811C(pl, 0xDD028756);
                PlayerResetGunTimer(pl);
                *(int*)(pl + 0x26D8) = 0;
                if (*(int*)(pl + 0x7D0) == 0) {
                    PlayerSetHeldSlotMode(pl, 2);
                }
            }
            d->timer = gRivetZero;
            d->cursor = 0;
            d->state = 2;
            d->outcome = 0;
            if (d->have[4] == 0) {
                int pick = 0;
                if (d->have[0] == 0 && (pick = 1, d->have[1] == 0) && (pick = 2, d->have[2] == 0) &&
                    (pick = 3, d->have[3] == 0) && (pick = 5, d->have[5] == 0) &&
                    (pick = 6, d->have[6] == 0) && (pick = 7, d->have[7] == 0) &&
                    (pick = 8, d->have[8] == 0)) {
                    pick = 9;
                }
                if (pick == 9) {
                    pl = (char*)PlayerGetPossessor(d->unk88);
                    if (pl != NULL) {
                        RivetReleasePlayer(pl);
                    }
                    d->state = 0;
                } else {
                    char* p2 = (char*)PlayerGetPossessor(d->unk88);
                    if (p2 != NULL) {
                        d->mode = pick;
                        RivetStartEase(d, a, p2, pick, 0x429A14B6, pick + 0x144E4B52);
                    }
                }
            } else {
                pl = (char*)PlayerGetPossessor(d->unk88);
                if (pl != NULL) {
                    d->mode = 4;
                    RivetStartEase(d, a, pl, 4, 0x429A14B6, 0x144E4B56);
                }
            }
            break;
        case 2:
            if (!RivetEasePlayer(d, dt)) {
                d->timer = gRivetT2;
                d->state = 0xB;
            }
            break;
        case 3:
            d->timer += dt;
            if ((float)d->result < d->timer) {
                d->timer -= (float)d->result;
                ModelNodeSetup(a->model, AssetFind(0xD9044CBB, d->placed[d->cursor] + 0x5EDEF91D), 0, 0);
                ModelNodeSetup(a->model, AssetFind(0xD9044CBB, d->placed[d->cursor] - 0x29273EC3), 0, 0);
                d->cursor++;
                if (d->cursor < d->count) {
                    ModelNodeSetup(a->model, AssetFind(0xD9044CBB, d->placed[d->cursor] + 0x5EDEF91D), 1, 0);
                    ModelNodeSetup(a->model, AssetFind(0xD9044CBB, d->placed[d->cursor] - 0x29273EC3), 1, 0);
                    FxStart(AssetFind(0x92234D44, 0xF307998F), 0, 0);
                } else {
                    d->timer = gRivetZero;
                    d->cursor = 0;
                    d->state = 4;
                }
            }
            break;
        case 4:
            if (d->outcome == -1) {
                d->state = 7;
                break;
            }
            if (d->outcome == 1) {
                d->state = 6;
                break;
            }
            if (!RivetEasePlayer(d, dt)) {
                int handled = 0;
                pl = (char*)PlayerGetPossessor(d->unk88);
                if (pl != NULL) {
                    PlayerResetGunTimer(pl);
                    if (PadPressed(*(int*)(pl + 0x58C), 0, 10) == 1 ||
                        PadPressed(*(int*)(pl + 0x58C), 0, 11) == 1) {
                        int hA = QueryVar(*(int*)(pl + 0x38), 0x2DA8293C);
                        int hB = QueryVar(*(int*)(pl + 0x38), 0xDFF3097A);
                        if (Fn800F7DEC(hA, hB, 0xF829DB93, fn_800498D0(pl), 0) != 0x4A3E183) {
                            d->state = 0xC;
                            d->timer = gRivetZero;
                        } else {
                            PlayerSetHeldSlotMode(pl, 8);
                            d->state = 5;
                            handled = 1;
                            d->timer = gRivetT3;
                        }
                    }
                }
                if (!handled) {
                    RivetSelect(d, a);
                }
            }
            pl = (char*)PlayerGetPossessor(d->unk88);
            if (pl != NULL && PadPressed(*(int*)(pl + 0x58C), 0, 4) != 0) {
                SendMsg(d->unk88, 0x40C, NULL);
                pl = (char*)PlayerGetPossessor(d->unk88);
                if (pl != NULL) {
                    RivetReleasePlayer(pl);
                }
                if (d->unk80 == 0) {
                    RivetNodesReset(d, a);
                }
                d->state = 0;
            }
            break;
        case 5: {
            float t = d->timer - dt;
            float z = gRivetZero;
            if (z < t) {
                z = t;
            }
            d->timer = z;
            if (gRivetZero == d->timer) {
                if (d->unk80 == 0) {
                    if (d->mode + 0x144E4B52 == d->placed[d->cursor]) {
                        d->glow[d->mode] = gRivetT4;
                        d->cursor++;
                        if (d->cursor == d->count) {
                            d->outcome = 1;
                        }
                        ActorPlayAnim(a->handle, 0x4A601EE, a->handle);
                    } else {
                        d->outcome = -1;
                    }
                } else {
                    d->glow[d->mode] = gRivetT4;
                    ActorPlayAnim(a->handle, 0x4A601EE, a->handle);
                }
                d->state = 4;
                gRivetBusyFlag = 0;
            } else {
                gRivetBusyFlag = 1;
            }
            break;
        }
        case 6:
            d->state = 8;
            d->timer = gRivetT5;
            break;
        case 7:
            d->state = 9;
            d->timer = gRivetT5;
            break;
        case 8:
            d->timer -= dt;
            if (d->timer < gRivetZero) {
                pl = (char*)PlayerGetPossessor(d->unk88);
                if (pl != NULL) {
                    RivetReleasePlayer(pl);
                }
                d->state = 10;
                ActorPlayAnim(a->handle, 0xBB80CBE3, a->handle);
            }
            break;
        case 9:
            d->timer -= dt;
            if (d->timer < gRivetZero) {
                pl = (char*)PlayerGetPossessor(d->unk88);
                if (pl != NULL) {
                    RivetReleasePlayer(pl);
                }
                RivetNodesReset(d, a);
                d->state = 0;
                ActorPlayAnim(a->handle, 0x20CF1E, a->handle);
            }
            break;
        case 10:
            if (d->unk80 != 0) {
                d->state = 0;
            }
            break;
        case 0xB:
            d->timer -= dt;
            if (d->timer < gRivetZero) {
                if (d->unk80 == 0) {
                    d->timer = gRivetZero;
                    d->state = 3;
                    ModelNodeSetup(a->model, AssetFind(0xD9044CBB, d->placed[0] + 0x5EDEF91D), 1, 0);
                    ModelNodeSetup(a->model, AssetFind(0xD9044CBB, d->placed[0] - 0x29273EC3), 1, 0);
                    FxStart(AssetFind(0x92234D44, 0xF307998F), 0, 0);
                } else {
                    d->timer = gRivetZero;
                    d->cursor = 0;
                    d->state = 4;
                }
            }
            break;
        case 0xC: {
            char* p2 = (char*)PlayerGetPossessor(d->unk88);
            pl = (char*)PlayerGetPossessor(d->unk88);
            if (pl != NULL) {
                RivetReleasePlayer(pl);
            }
            RivetNodesReset(d, a);
            if (p2 != NULL) {
                PlayerSetHeldSlotMode(p2, 0xB);
            }
            d->state = 0;
            ActorPlayAnim(a->handle, 0xA6D68DCD, a->handle);
            break;
        }
        }
    }
    {
        float zero = gRivetZero;
        for (i = 0; i < 9; i++) {
            if (zero < d->glow[i]) {
                float v = d->glow[i] - dt;
                float c = zero;
                int node;
                if (zero < v) {
                    c = v;
                }
                node = AssetFind(0xD9044CBB, i + 0x732D446F);
                if (node >= 0) {
                    int lvl;
                    ModelNodeSetup(a->model, node, 1, 0);
                    lvl = (int)(gRivetFadeK * (c / gRivetT4));
                    fn_80037EC0(a->model, node, 1, 0xFF, (lvl + 0xFF) & 0xFF, (lvl + 0xFF) & 0xFF, 0xFF);
                }
                d->glow[i] = c;
            }
        }
    }
    ActorUpdate(dt, a);
}

// Sets the "busy" flag of a player object (0x26D8) and returns 0.
double RivetPlayerBusy(void* player) {
    *(int*)((char*)player + 0x26D8) = 1;
    return gRivetZero;
}

// Clears the rivet state and hides/queries the rivet nodes; returns 1 when the state exists.
static inline int RivetReset(RivetActor* a) {
    RivetData* d = a->data;
    int i;
    if (d == NULL) {
        return 0;
    }
    memset(d, 0, 0xF8);
    d->count = 0;
    d->mode = 4;
    d->unk80 = 0;
    d->state = 0;
    d->unk88 = 0;
    d->result = 0;
    d->duration = gRivetZero;
    d->elapsed = gRivetZero;
    for (i = 0; i < 9; i++) {
        int node = AssetFind(0xD9044CBB, i + 0x732D446F);
        if (node >= 0) {
            ModelNodeSetup(a->model, node, 0, 0);
        }
        node = AssetFind(0xD9044CBB, i - 0x14D8F371);
        if (node >= 0) {
            ModelNodeSetup(a->model, node, 0, 0);
        }
    }
    d = a->data;
    if (d != NULL) {
        int* have = d->have;
        for (i = 0; i < 9; i++) {
            int bone = ModelFindBone(a->model, AssetFind(0xF1754BF2, i + 0x144E4B52));
            if (bone == -1) {
                *have = 0;
            } else {
                *have = 1;
            }
            have++;
        }
    }
    return 1;
}

// Records rivet id `id` as placed (once).
static inline void RivetAdd(RivetData* d, int id) {
    int found = 0;
    int n = d->count;
    int* p = d->placed;
    for (; n > 0; n--, p++) {
        if (id == *p) {
            found = 1;
            break;
        }
    }
    if (!found) {
        d->placed[d->count] = id;
        d->count++;
    }
}

inline u32 StrHash(u32 h, const char* s) {
    const char* p;
    for (p = s; *p != 0; p++) {
        h = h * 31 + (unsigned char)toupper(*p);
    }
    h = h % 0xFFFFFFFF;
    return h;
}

extern "C" {
void TextBegin(void);
void TextSetSize(int n);
void TextSetColor(int r, int g, int b, int a);
void DebugTextPos(int x, int y, int z);
void DebugText(const char* s, ...);
}

int RivetVarSet(int* m);

extern float gRivetPos[9][3];   // 0x80349B34: rivet positions (relative to the panel)
extern double gRivetBias;       // 0x804D9608
extern float gRivetSpeed;       // 0x804D9610
extern "C" {
void* PlayerGetPossessor(int i);                                    // 0x80049888
int PadPressed(int pad, int kind, int button);                      // 0x800DC930
int fn_8001F0DC(void* a, int* pos, u32 mask, int b, int c, int d, int* out, int e);
void fn_800A0328(void* m);
void fn_8009ED70(void* out, void* in);
void fn_8016D440(float* out, float* v);
void fn_8009FAC0(void* m, float* e);
}

// Rivet selection: moves the cursor over the 3x3 rivet grid with the pad, eases the cursor
// model to the chosen rivet and returns 1 when the selection changed.
int RivetSelect(RivetData* d, RivetActor* a) {
    char* pl = (char*)PlayerGetPossessor(d->unk88);
    if (pl != NULL) {
        int sel = d->mode;
        u32 flip = *(u32*)(pl + 0x254) & 1;
        int pad = *(int*)(pl + 0x58C);
        int next;
        if ((PadPressed(pad, 0, flip) != 0 || PadPressed(pad, 2, flip + 6) != 0) && sel > 2) {
            sel -= 3;
        }
        if ((PadPressed(pad, 0, flip == 0) != 0 || PadPressed(pad, 2, (flip != 0) + 7) != 0) &&
            sel < 6) {
            sel += 3;
        }
        if ((PadPressed(pad, 0, 5) != 0 || PadPressed(pad, 2, 2) != 0) && sel != (sel / 3) * 3) {
            sel -= 1;
        }
        if ((PadPressed(pad, 0, 6) != 0 || PadPressed(pad, 2, 3) != 0) && sel % 3 != 2) {
            sel += 1;
        }
        if (sel < 0) {
            next = 0;
        } else {
            next = 9;
            if (sel < 10) {
                next = sel;
            }
        }
        if (d->have[next] != 0 && d->mode != next && next >= 0 && next < 9) {
            float pos[9][3];
            float dist2;
            float dist;
            int i;
            for (i = 0; i < 9; i++) {
                pos[i][0] = gRivetPos[i][0];
                pos[i][1] = gRivetPos[i][1];
                pos[i][2] = gRivetPos[i][2];
            }
            dist = gRivetZero;
            dist2 = (pos[d->mode][2] - pos[next][2]) * (pos[d->mode][2] - pos[next][2]) +
                    (pos[d->mode][0] - pos[next][0]) * (pos[d->mode][0] - pos[next][0]) +
                    (pos[d->mode][1] - pos[next][1]) * (pos[d->mode][1] - pos[next][1]);
            if (dist < dist2) {
                dist = sqrtf(dist2);
            }
            if (PlayerGetPossessor(d->unk88) != NULL) {
                char* p2 = pl;
                float from[12];
                float to[12];
                float delta[3];
                int fb[3];
                int hit;
                d->mode = next;
                {
                    QueryMsg q;
                    q.unk8 = 0;
                    q.hash = AssetFind(0xF1754BF2, 0x429A14B6);
                    if (SendMsg(a->handle, 0x411, &q) == 0) {
                        fn_800A0328(from);
                    } else {
                        fn_8009ED70(from, &q);
                    }
                    q.hash = AssetFind(0xF1754BF2, next + 0x144E4B52);
                    if (SendMsg(a->handle, 0x411, &q) == 0) {
                        fn_800A0328(to);
                    } else {
                        fn_8009ED70(to, &q);
                    }
                }
                fb[0] = (int)from[9];
                fb[1] = (int)from[10];
                fb[2] = (int)from[11];
                hit = fn_8001F0DC(a, fb, 0x80000000, 0, 0, 0, &hit, 0);
                if (hit != 0) {
                    to[11] = (float)hit + *(float*)(p2 + 0x64C) +
                             (*(float*)(p2 + 0x1EC) - (float)*(int*)(p2 + 0x2C));
                }
                delta[0] = to[9] - from[9];
                delta[1] = to[10] - from[10];
                delta[2] = to[11] - from[11];
                fn_8016D440(from, delta);
                fn_8009FAC0((char*)d + 0xBC, from);
                *(float*)((char*)d + 0xE0) = from[9];
                *(float*)((char*)d + 0xE4) = from[10];
                *(float*)((char*)d + 0xE8) = from[11];
                d->duration = (float)(gRivetSpeed * dist);
                d->elapsed = gRivetZero;
            }
            return 1;
        }
    }
    return 0;
}

// Message handler of the rivet actor: draws the status text (message 0), answers variable
// queries and the 0x409 "attach" command.
int RivetMsg(void* self, int msg, int* m, int arg) {
    RivetActor* a;
    RivetData* d;
    if (msg != 0x409) {
        if (msg < 0x409) {
            if (msg < 3) {
                if (msg == 0 && ((RivetActor*)m[0])->data != NULL) {
                    int state = ((RivetActor*)m[0])->data->state;
                    if (state == 9) {
                        TextBegin();
                        TextSetSize(0x18);
                        TextSetColor(0, 0xFF, 0, 0xFF);
                        DebugTextPos(0x140, 0xFC, 2);
                        DebugText("INCORRECT RIVET PLACEMENT");
                    } else if (state < 9) {
                        if (state > 4 && state > 7) {
                            TextBegin();
                            TextSetSize(0x18);
                            TextSetColor(0, 0xFF, 0, 0xFF);
                            DebugTextPos(0x140, 0xFC, 2);
                            DebugText("SUCCESS");
                        }
                    } else if (state == 0xB) {
                        TextBegin();
                        TextSetSize(0x18);
                        TextSetColor(0, 0xFF, 0, 0xFF);
                        DebugTextPos(0x140, 0xFC, 2);
                        DebugText("Ready");
                    }
                }
            } else if (msg < 5) {
                return 0;
            }
        } else if (msg == 0x41D) {
            if (RivetVarSet(m) != 0) {
                return 1;
            }
        } else if (msg < 0x41D && msg < 0x40B) {
            u32 h = StrHash(m[2], "QUERY");
            a = (RivetActor*)m[0];
            if (ActorPlayAnim(a->handle, h, m[1]) != 0) {
                return 1;
            }
            d = ((RivetActor*)m[0])->data;
            if (d != NULL && d->count != 0 && m[2] == 0x7C44EC3D && d->state == 0) {
                struct {
                    int unk0;
                    int unk4;
                    int unk8;
                    int unkC;
                } rq;
                rq.unk4 = 0;
                m[3] = -0xDEA7E22;
                rq.unk8 = m[3];
                rq.unkC = AssetFind(0xC0A77187, 0x4B8CC42);
                SendMsg(m[1], 0x40B, &rq);
                return 1;
            }
            return 0;
        }
        return ActorDefaultMsg(self, msg, m, arg);
    }
    a = (RivetActor*)m[0];
    if (ActorPlayAnim(a->handle, m[3], m[2]) != 0) {
        return 1;
    }
    d = ((RivetActor*)m[0])->data;
    if (d != NULL && d->state == 0) {
        d->state = 1;
        d->unk88 = m[2];
        return 1;
    }
    return 0;
}

// Variable set messages of the rivet actor: reset (0x4DF34DF1), place a rivet (0x4DF1917D),
// result (0x4ED697EB) and mode (-0x585189D8).
int RivetVarSet(int* m) {
    RivetActor* a = (RivetActor*)m[0];
    RivetData* d;
    if (fn_80019F88(m, a) != 0) {
        return 1;
    }
    switch (m[1]) {
    case 0x4DF34DF1:
        RivetReset(a);
        break;
    case 0x4DF1917D:
        d = a->data;
        if (d != NULL && d->count < 9) {
            int id = m[3] + 0x144E4B51;
            if (id > 0x144E4B51 && id < 0x144E4B5B && d->have[m[3] - 1] != 0) {
                RivetAdd(d, id);
            }
        }
        break;
    case -0x585189D8:
        if (a->data != NULL) {
            a->data->unk80 = m[3];
        }
        break;
    case 0x4ED697EB:
        if (a->data != NULL) {
            a->data->result = m[3];
        }
        break;
    default:
        return 0;
    }
    return 1;
}

// Frees the rivet state.
void RivetDestroy(RivetActor* a) {
    if (a->data != NULL) {
        VFree(a->data);
        a->data = NULL;
    }
}

extern "C" void fn_80038474(int model, int node, int a, int b);

// Creates the rivet state: resets it, pre-places the rivets whose bones exist and shows
// the base node.
int RivetCreate(RivetActor* a) {
    RivetData* d;
    int node;
    if (fn_800E6444(a) == 0) {
        a->unk88 = 0;
        return 0;
    }
    if (a->data == NULL) {
        a->data = (RivetData*)VAlloc(0xF8);
    }
    if (RivetReset(a)) {
        d = a->data;
        if (d != NULL && d->count < 9 && d->have[3] != 0) {
            RivetAdd(d, 0x144E4B55);
        }
        d = a->data;
        if (d != NULL && d->count < 9 && d->have[4] != 0) {
            RivetAdd(d, 0x144E4B56);
        }
        d = a->data;
        if (d != NULL && d->count < 9 && d->have[5] != 0) {
            RivetAdd(d, 0x144E4B57);
        }
        if (a->data != NULL) {
            a->data->result = 1000;
        }
    }
    node = AssetFind(0xF1754BF2, 0x429A14B6);
    fn_80038474(a->model, node, 0, 0);
    return 1;
}

// ---- camera tunables ---------------------------------------------------------------------
// The tunables live at 0x8049EE68: a mode word, an enable flag, the two current values
// (cur0, cur1) and four presets (walk, climb, ride, hang) of
// {value0, hold0, value1, hold1, ...}.  Each pair of ramps (value, delay, duration) eases
// cur0/cur1 from the old preset value to the new one.
struct CamTune {
    int mode;               // 0x00 (ee68)
    int active;             // 0x04 (ee6c)
    float cur0;             // 0x08 (ee70)
    float cur1;             // 0x0C (ee74)
    float a0;               // 0x10 (ee78)
    float a1;               // 0x14 (ee7c)
    float a2;               // 0x18 (ee80)
    float a3;               // 0x1C (ee84)
    float a4;               // 0x20 (ee88)
    float a5;               // 0x24 (ee8c)
    float a6;               // 0x28 (ee90)
    float a7;               // 0x2C (ee94)
    float b0;               // 0x30 (ee98)
    float b1;               // 0x34 (ee9c)
    float b2;               // 0x38 (eea0)
    float b3;               // 0x3C (eea4)
    float b4;               // 0x40 (eea8)
    float b5;               // 0x44 (eeac)
    float b6;               // 0x48 (eeb0)
    float b7;               // 0x4C (eeb4)
    float c0;               // 0x50 (eeb8)
    float c1;               // 0x54 (eebc)
    float c2;               // 0x58 (eec0)
    float c3;               // 0x5C (eec4)
    float c4;               // 0x60 (eec8)
    float c5;               // 0x64 (eecc)
    float c6;               // 0x68 (eed0)
    float c7;               // 0x6C (eed4)
    float d0;               // 0x70 (eed8)
    float d1;               // 0x74 (eedc)
    float d2;               // 0x78 (eee0)
    float d3;               // 0x7C (eee4)
    float d4;               // 0x80 (eee8)
    float d5;               // 0x84 (eeec)
    float d6;               // 0x88 (eef0)
    float d7;               // 0x8C (eef4)
    float delay0;           // 0x90 (eef8)
    float timer0;           // 0x94 (eefc)
    float delay1;           // 0x98 (ef00)
    float timer1;           // 0x9C (ef04)
};

extern CamTune gCamTune;        // 0x8049EE68
extern float gCamScale;         // 0x804D9618
extern float gCamZero;          // 0x804D9628
extern float gCamOne;           // 0x804D962C
extern float gCamP0;            // 0x804D9630
extern float gCamP1;            // 0x804D9634
extern float gCamP2;            // 0x804D9638
extern float gCamP3;            // 0x804D963C
extern float gCamP4;            // 0x804D9640
extern float gCamP5;            // 0x804D9644
extern float gCamP6;            // 0x804D9648
extern float gCamP7;            // 0x804D964C
extern float gCamP8;            // 0x804D9650
extern float gCamP9;            // 0x804D9654

struct CamMsg {
    void* actor;            // 0x00
    int hash;               // 0x04
    int unk08;
    int value;              // 0x0C
};

static inline float CamScaled(int v) {
    return gCamScale * (float)v;
}

// Camera tunable variable set messages (values arrive in 1/1000ths).
int CamTuneSet(CamMsg* m) {
    switch (m->hash) {
    case -0xE25C821:
        gCamTune.a5 = CamScaled(m->value);
        return 1;
    case -0x39B5EB82:
        gCamTune.b7 = CamScaled(m->value);
        return 1;
    case -0x49E55B69:
        gCamTune.b3 = CamScaled(m->value);
        return 1;
    case -0x71C7EEEA:
        gCamTune.a1 = CamScaled(m->value);
        return 1;
    case -0x739174D4:
        gCamTune.active = m->value;
        return 1;
    case -0x444AA7B3: {
        int host = PlayerInHost(m->actor);
        if (host != 0) {
            gCamTune.cur0 = gCamTune.b0;
            gCamTune.cur1 = gCamTune.b4;
            gCamTune.delay0 = gCamTune.b1;
            gCamTune.timer0 = gCamTune.b3;
            gCamTune.delay1 = gCamTune.b5;
            gCamTune.timer1 = gCamTune.b7;
        } else {
            gCamTune.cur0 = gCamTune.a0;
            gCamTune.cur1 = gCamTune.a4;
            gCamTune.delay0 = gCamTune.a1;
            gCamTune.timer0 = gCamTune.a3;
            gCamTune.delay1 = gCamTune.a5;
            gCamTune.timer1 = gCamTune.a7;
        }
        gCamTune.mode = (host != 0);
        return 1;
    }
    case -0x2689D5FB:
        gCamTune.b5 = CamScaled(m->value);
        return 1;
    case -0x2BC58D35:
        gCamTune.b4 = CamScaled(m->value);
        return 1;
    case -0x2FD42F28:
        gCamTune.a7 = CamScaled(m->value);
        return 1;
    case -0x24425CFC:
        gCamTune.b6 = CamScaled(m->value);
        return 1;
    case 0x5BE24230:
        gCamTune.b1 = CamScaled(m->value);
        return 1;
    case 0x19AA90DD:
        gCamTune.b2 = CamScaled(m->value);
        return 1;
    case 0x1142F4FD:
        gCamTune.a3 = CamScaled(m->value);
        return 1;
    case -0xBDE4F22:
        gCamTune.a6 = CamScaled(m->value);
        return 1;
    case 0x4C005FC3:
        gCamTune.a2 = CamScaled(m->value);
        return 1;
    case 0x63D27225:
        gCamTune.a4 = CamScaled(m->value);
        return 1;
    case 0x601ED99C:
        gCamTune.a0 = CamScaled(m->value);
        return 1;
    case 0x6C012C36:
        gCamTune.b0 = CamScaled(m->value);
        return 1;
    }
    return 0;
}

// Eases `cur` from `lo` toward `hi` while `timer` runs out (after `delay` ran out).
static inline void CamRamp(float dt, float& delay, float& timer, float& cur, float lo, float hi,
                           float dur) {
    float zero = gCamZero;
    if (delay <= zero) {
        if (zero < timer) {
            float t = timer - dt;
            if (zero < t) {
                zero = t;
            }
            timer = zero;
            cur = (hi - lo) * ((dur - zero) / dur) + lo;
        }
    } else {
        float t = delay - dt;
        if (zero < t) {
            zero = t;
        }
        delay = zero;
    }
}

static inline void CamLoad(float c0, float c1, float d0, float t0, float d1, float t1) {
    gCamTune.cur0 = c0;
    gCamTune.cur1 = c1;
    gCamTune.delay0 = d0;
    gCamTune.timer0 = t0;
    gCamTune.delay1 = d1;
    gCamTune.timer1 = t1;
}

// Per-frame camera tunable update: picks the preset for the player's state and eases toward it.
void CamTuneUpdate(float dt) {
    CamTune& g = gCamTune;
    char* p = (char*)ActorGet(0);
    fn_802A209C(g.active);
    if (g.active == 1 && p != NULL && fn_80049940() == 0 && (*(u32*)(p + 0x4C) & 1) == 0) {
        if (PlayerInHost(p) == 0) {
            if (g.mode == 1) {
                CamLoad(g.a0, g.a4, g.a1, g.a3, g.a5, g.a7);
                g.mode = 0;
            }
            if (g.mode == 2) {
                CamLoad(g.a2, g.a6, gCamZero, gCamZero, gCamZero, gCamZero);
                g.mode = 0;
            }
            if (g.mode == 3) {
                CamLoad(g.a2, g.a6, gCamZero, gCamZero, gCamZero, gCamZero);
                g.mode = 0;
            }
            CamRamp(dt, g.delay0, g.timer0, g.cur0, g.a0, g.a2, g.a3);
            CamRamp(dt, g.delay1, g.timer1, g.cur1, g.a4, g.a6, g.a7);
        } else if ((*(u32*)(p + 0x7B4) & 0x10000000) == 0) {
            if (*(int*)(p + 0x960) == 1 || *(int*)(p + 0x9E4) != 0) {
                if (g.mode != 4) {
                    CamLoad(g.d0, g.d4, g.d1, g.d3, g.d5, g.d7);
                    g.mode = 4;
                }
                CamRamp(dt, g.delay0, g.timer0, g.cur0, g.d0, g.d2, g.d3);
                CamRamp(dt, g.delay1, g.timer1, g.cur1, g.d4, g.d6, g.d7);
            } else {
                if (g.mode == 0) {
                    CamLoad(g.b0, g.b4, g.b1, g.b3, g.b5, g.b7);
                    g.mode = 1;
                }
                if (g.mode == 2) {
                    CamLoad(g.b2, g.b6, gCamZero, gCamZero, gCamZero, gCamZero);
                    g.mode = 1;
                }
                if (g.mode == 3) {
                    CamLoad(g.b2, g.b6, gCamZero, gCamZero, gCamZero, gCamZero);
                    g.mode = 1;
                }
                CamRamp(dt, g.delay0, g.timer0, g.cur0, g.b0, g.b2, g.b3);
                CamRamp(dt, g.delay1, g.timer1, g.cur1, g.b4, g.b6, g.b7);
            }
        } else {
            if (g.mode == 1) {
                CamLoad(g.c0, g.c4, g.c1, g.c3, g.c5, g.c7);
                g.mode = 2;
            }
            if (g.mode == 3) {
                CamLoad(g.c2, g.c6, gCamZero, gCamZero, gCamZero, gCamZero);
                g.mode = 2;
            }
            CamRamp(dt, g.delay0, g.timer0, g.cur0, g.a0, g.a2, g.a3);
            CamRamp(dt, g.delay1, g.timer1, g.cur1, g.a4, g.a6, g.a7);
        }
    } else {
        g.mode = 3;
        g.cur0 = gCamOne;
        g.cur1 = gCamOne;
    }
    fn_802A2078(g.cur0);
    fn_802A2054(g.cur1);
}

void CamTuneNop(void) {
}

// Resets the easing state to its idle values.
extern "C" void fn_80225CF8(void);

// CamTuneReset (called from another unit as fn_80225CF8)
void fn_80225CF8(void) {
    float one = gCamOne;
    float zero = gCamZero;
    gCamTune.cur0 = one;
    gCamTune.cur1 = one;
    gCamTune.delay0 = zero;
    gCamTune.timer0 = zero;
    gCamTune.delay1 = zero;
    gCamTune.timer1 = zero;
    fn_802A2078(gCamTune.cur0);
    fn_802A2054(gCamTune.cur1);
}

// Default tunables.
void CamTuneInit(void) {
    CamTune& g = gCamTune;
    g.active = 1;
    g.mode = 0;
    g.cur0 = gCamP0;
    g.cur1 = gCamOne;
    g.a0 = gCamP0;
    g.a1 = gCamP1;
    g.a2 = gCamP0;
    g.a3 = gCamP2;
    g.a4 = gCamP3;
    g.a5 = gCamP4;
    g.a6 = gCamOne;
    g.a7 = gCamP5;
    g.b0 = gCamP0;
    g.b1 = gCamP1;
    g.b2 = gCamOne;
    g.b3 = gCamP2;
    g.b4 = gCamOne;
    g.b5 = gCamP1;
    g.b6 = gCamOne;
    g.b7 = gCamP2;
    g.c0 = gCamP0;
    g.c1 = gCamZero;
    g.c2 = gCamP0;
    g.c3 = gCamZero;
    g.c4 = gCamP6;
    g.c5 = gCamZero;
    g.c6 = gCamP6;
    g.c7 = gCamZero;
    g.d0 = gCamP7;
    g.d1 = gCamP8;
    g.d2 = gCamP9;
    g.d3 = gCamP5;
    g.d4 = gCamP7;
    g.d5 = gCamZero;
    g.d6 = gCamOne;
    g.d7 = gCamZero;
    g.delay0 = gCamZero;
    g.timer0 = gCamZero;
    g.delay1 = gCamZero;
    g.timer1 = gCamZero;
}

// ---- player state callbacks --------------------------------------------------------------

struct HostInfo {
    int type;               // 0x00
    u8 pad04[0x14 - 0x04];
    int animA;              // 0x14
    int animB;              // 0x18
    int animC;              // 0x1C
};

struct Player {
    u8 pad00[0x24];
    int pos[3];             // 0x24
    int pad30[2];
    int handle;             // 0x38
    u8 pad3C[0x50 - 0x3C];
    u32 flags;              // 0x50
    u8 pad54[0x78 - 0x54];
    int unk78;              // 0x78
    u8 pad7C[0x8C - 0x7C];
    void* fn8C;             // 0x8C
    u8 pad90[0x98 - 0x90];
    int v98;                // 0x98
    u8 pad9C[0xA4 - 0x9C];
    int vA4;                // 0xA4
    u8 padA8[0xC4 - 0xA8];
    int vC4;                // 0xC4
    u8 padC8[0xF0 - 0xC8];
    int vF0;                // 0xF0
    u8 padF4[0xFC - 0xF4];
    int vFC;                // 0xFC
    u8 pad100[0x11C - 0x100];
    int v11C;               // 0x11C
    u8 pad120[0x314 - 0x120];
    int v314;               // 0x314
    u8 pad318[0x638 - 0x318];
    int v638;               // 0x638
    u8 pad63C[0x678 - 0x63C];
    u32 flags678;           // 0x678
    float x67C;             // 0x67C
    float y680;             // 0x680
    float z684;             // 0x684
    u8 pad688[0x73C - 0x688];
    int hostIndex;          // 0x73C
    u8 pad740[0x7A4 - 0x740];
    int sub7A4;             // 0x7A4: movement sub-state object
    u8 pad7A8[0x7B4 - 0x7A8];
    u32 flags7B4;           // 0x7B4
    u8 pad7B8[0x7D0 - 0x7B8];
    int v7D0;               // 0x7D0
    u8 pad7D4[0x26C4 - 0x7D4];
    HostInfo* hosts[2];     // 0x26C4 (indexed by PlayerInHost)
    u8 pad26CC[0x26E0 - 0x26CC];
    int obj26E0;            // 0x26E0
    Player* target;         // 0x26E4
    Player* held;           // 0x26E8
    u8 pad26EC[0x279C - 0x26EC];
    int v279C;              // 0x279C
};

extern int gHostAnimA[];        // 0x803BD054
extern int gHostAnimB[];        // 0x803BCFC4
extern int gHostAnimC[];        // 0x803BCF7C



// Exit of the hang state: back to the normal movement sub-state.
void PlayerHangExit(float dt, Player* p) {
    p->v638 = 0;
    p->flags7B4 &= ~1;
    fn_800769A0(dt, p);
    PlayerSelectCollisionUpdate(dt, p);
    if (p->vC4 == -1) {
        if (p->v7D0 != 0) {
            PlayerSetValue4(&p->sub7A4, 3);
        } else {
            PlayerSetValue4(&p->sub7A4, 1);
        }
    } else {
        PlayerSetValue4(&p->sub7A4, 0x16);
    }
}

void PlayerHangResetAnim(Player* p) {
    p->fn8C = NULL;
    if (p->vC4 != -1) {
        p->v98 = p->vC4;
    }
    if (p->v11C == -1) {
        return;
    }
    p->vF0 = p->v11C;
}

void PlayerHangEnter(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    PlayerSetValue4(&p->sub7A4, 0x15);
    if (p->v279C == -1 || fn_80175EFC() != 0) {
        PlayerSetValue4(&p->sub7A4, 3);
        fn_80155A84(&p->obj26E0, p->held, 1);
    }
}

// Is the held object a player-type actor that has condition bit 0xB set?  (Helper of the
// grab-state enters below.)
static inline int HeldIsSpecial(Player* held, int* info) {
    int r = 0;
    if ((u32)held->handle >> 24 == 0x13) {
        *info = fn_800A384C(held);
        if (HasCondition((char*)held + 0x270, 0xB) != 0) {
            r = 1;
        }
    }
    return r;
}

void PlayerGrabEnterA(Player* p) {
    int info;
    int special;
    int idx;
    int tbl;
    Player* held = p->held;
    special = HeldIsSpecial(held, &info);
    PlayerInHost(p);
    tbl = p->hostIndex;
    p->v98 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x18), gHostAnimA[tbl], special, 0);
    p->fn8C = (void*)fn_80227594;
    *(int*)((char*)p + 0xA4) = 0x19;
    p->vF0 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x1C), gHostAnimA[tbl], special, 0);
    p->vFC = 0x19;
    PlayerSetValue4(&p->sub7A4, 0x15);
}

void PlayerGrabUpdate(float dt, Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    fn_800769A0(dt, p);
    PlayerSelectCollisionUpdate(dt, p);
    if (PlayerGetHeldSlotIndex(p) != 0) {
        PlayerSetValue4(&p->sub7A4, 2);
    } else {
        PlayerSetValue4(&p->sub7A4, 1);
    }
    PlayerInHost(p);
    fn_80226E78(p);
}

void PlayerHostSync(Player* p) {
    int a;
    int b;
    PlayerInHost(p);
    p->unk78 = 0;
    p->flags &= ~0x20000;
    p->v98 = QueryVar(p->target->handle, 0x6A0D51C0);
    b = QueryVar(p->target->handle, 0x8783452);
    if (b == 0) {
        PlayerSetHeldSlotIndex(p, 1);
        if (p->v98 == -1) {
            PlayerSetActionMode(p, 3);
        }
    } else {
        PlayerSetHeldSlotIndex(p, 0);
        if (p->v98 == -1) {
            PlayerSetActionMode(p, 1);
        }
    }
}

void PlayerHangHold(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    if ((p->flags7B4 & 1) != 0) {
        p->flags7B4 &= ~1;
    } else {
        p->flags |= 0x20000;
        p->unk78 = 1;
    }
}

void PlayerGrabEnterB(Player* p) {
    int info = 0;
    int special = 0;
    int idx;
    int tbl;
    HostInfo* host;
    Player* held = p->held;
    special = HeldIsSpecial(held, &info);
    idx = PlayerInHost(p);
    tbl = p->hostIndex;
    p->v98 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x18), gHostAnimB[tbl], special, 0);
    p->fn8C = (void*)fn_80227594;
    *(int*)((char*)p + 0xA4) = 0x19;
    host = p->hosts[idx];
    p->vF0 = fn_800A1BC8(host->animA, host->animC, gHostAnimB[tbl], 0, 0);
    p->vFC = 0x19;
    PlayerSetHeldSlotIndex(p, QueryVar(p->held->handle, 0x8783452) == 0);
}

void PlayerReleaseHeld(Player* p) {
    fn_80155A84(&p->obj26E0, p->held, 1);
}

void PlayerHangEnterC(Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    p->flags |= 0x20000;
    p->unk78 = 1;
    PlayerSetValue4(&p->sub7A4, 0x13);
}

void PlayerGrabEnterC(Player* p) {
    int info = 0;
    int special;
    int idx;
    int tbl;
    HostInfo* host;
    idx = PlayerInHost(p);
    tbl = p->hostIndex;
    special = HeldIsSpecial(p->held, &info);
    p->v98 = fn_800A1BC8(*(int*)(info + 0x14), *(int*)(info + 0x18), gHostAnimC[tbl], special, 0);
    p->fn8C = (void*)fn_80227594;
    *(int*)((char*)p + 0xA4) = 0x59;
    host = p->hosts[idx];
    p->vF0 = fn_800A1BC8(host->animA, host->animC, gHostAnimC[tbl], 0, 0);
    p->vFC = 0x59;
    PlayerSetHeldSlotIndex(p, QueryVar(p->held->handle, 0x8783452) == 0);
}

void PlayerClearFlag40000(Player* p) {
    p->flags &= ~0x40000;
}

void PlayerIdleUpdate(float dt, Player* p) {
    PlayerResetGunTimer(p);
    p->v638 = 1;
    p->flags7B4 &= ~1;
    PlayerSelectCollisionUpdate(dt, p);
}

void PlayerRideEnter(Player* p) {
    PlayerResetGunTimer(p);
    p->flags |= 0x40000;
    fn_80050EB0(p);
    PlayerInHost(p);
    PlayerSetHeldSlotMode(p, 3);
}

extern const int gRivetOffset[3];       // 0x80349C28

// While riding on a rivet host: moves the player onto the host's seat position.
static inline void PlayerSnapToSeat(Player* p) {
    int target;
    int v[3];
    u8 m[0x30];
    fn_80212730(p->handle);
    target = OwnerGetInfo(p->handle);
    v[0] = gRivetOffset[0];
    v[1] = gRivetOffset[1];
    v[2] = gRivetOffset[2];
    fn_8009F63C(m, target);
    fn_8009F44C(v, m, v);
    v[0] += *(int*)(target + 0x24);
    v[1] += *(int*)(target + 0x28);
    v[2] += *(int*)(target + 0x2C);
    p->pos[0] = v[0];
    p->pos[1] = v[1];
    p->pos[2] = v[2];
    p->x67C = (float)v[0];
    p->y680 = (float)v[1];
    p->z684 = (float)v[2];
    MarkLookupEntryFlag(p->handle);
}

void PlayerSeatUpdate(float dt, Player* p) {
    p->v638 = 1;
    p->flags7B4 &= ~1;
    if (p->hosts[1]->type == 0x66B15822) {
        PlayerSnapToSeat(p);
    }
    if ((p->flags & 2) == 0 && (p->flags & 8) == 0) {
        fn_800769A0(dt, p);
        PlayerSelectCollisionUpdate(dt, p);
        PlayerSetValue4(&p->sub7A4, 5);
        if (p->v314 == 0) {
            PlayerSetValue4(&p->sub7A4, 3);
        } else {
            PlayerResetGunTimer(p);
        }
        fn_80226E78(p);
    } else {
        PlayerSetValue4(&p->sub7A4, 3);
    }
}

void PlayerSeatEnter(Player* p) {
    PlayerInHost(p);
    p->v98 = *(int*)(*(int*)((char*)p->target + 0x84) + 0x98);
    p->fn8C = (void*)fn_80227594;
    *(int*)((char*)p + 0xA4) = 1;
    p->vF0 = *(int*)(*(int*)((char*)p->target + 0x84) + 0x94);
    p->vFC = 1;
}

void PlayerSeatUpdateB(float dt, Player* p) {
    if (p->hosts[1]->type == 0x66B15822) {
        PlayerSnapToSeat(p);
    }
    QueryVar(p->handle, 0x2DA8293C);
    QueryVar(p->handle, 0xDFF3097A);
    p->v638 = 0;
    p->flags7B4 &= ~1;
    fn_800769A0(dt, p);
    PlayerSelectCollisionUpdate(dt, p);
    p->v7D0 = 1;
    if ((p->flags678 & 0x80) == 0) {
        PlayerSetValue4(&p->sub7A4, 3);
    } else {
        p->flags678 &= ~0x80;
        PlayerSetActionMode(p, 3);
    }
    fn_802272D4(p);
    fn_80226E78(p);
}

void PlayerHostLeave(Player* p) {
    int idx = PlayerInHost(p);
    int type = p->hosts[idx]->type;
    if (type == 0x54E34D35 || type == 0x6B6F4068) {
        p->v98 = -1;
    } else {
        p->v98 = QueryVar(p->target->handle, 0x6A0D51C0);
    }
}

void PlayerSeatUpdateC(float dt, Player* p) {
    if (p->hosts[1]->type == 0x66B15822) {
        PlayerSnapToSeat(p);
    }
    p->v638 = 1;
    p->flags7B4 &= ~1;
    fn_800769A0(dt, p);
    PlayerSelectCollisionUpdate(dt, p);
    if ((p->flags678 & 0x80) != 0) {
        p->flags678 &= ~0x80;
        PlayerSetActionMode(p, 1);
    }
    PlayerSetValue4(&p->sub7A4, 1);
    p->v7D0 = 0;
    PlayerSetActionMode(p, 3);
}

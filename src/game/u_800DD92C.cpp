// NPC talk (0x800DD92C-0x800DE0C4): nine voice categories ("See Threatening Solo", ..., "Die"). Each
// category owns a run of variant sounds in the level variables DB; a variant's key is the category key,
// hashed with " 2" for the second and then counted up. Timers per category, per NPC and per variant
// keep the same lines from repeating too often.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef signed short s16;
typedef unsigned char u8;

int Rand(void);                                       // 0x8000DDD0 (main.cpp)

extern "C" {
int rand(void);

// Callees in other files, not named yet.
struct TalkActor* fn_800A3990(void* owner);
void fn_8016EEF4(int sound);
int fn_8016F120(int sound, void* pos);
}
int MemFlagIsSet(void* bits, int index);

void* VAlloc(int size);
void VFree(void* p);
int VarExists(u32 group, u32 obj, u32 var);
int VarGroupObjCount(u32 key);
int Fn800DF3B8(u32 key, int index, u32 var);

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s, u32 key) {
    u32 h;
    int i;

    if (s == 0) {
        h = 0;
    } else {
        h = key;
        for (i = 0; s[i] != 0; i++) {
            h = h * 31 + (unsigned char)toupper(s[i]);
        }
        h = h % 0xFFFFFFFF;
    }
    return h;
}

#define TALK_GROUP 0xE6BCD867

struct TalkActor {
    u8 pad0[0x24];
    u8 pos[0x24C];
    u8 state[0x3AC];
    int sound;
    int talker;
    s16 kind;
    s16 variant;
};

struct TalkKind {
    u32 key;
    float npcMin;
    float npcMax;
    float kindMin;
    float kindMax;
    int start;
    int count;
};

static const char* sTalkNames[] = {
    "See Threatening Solo", "See Threatening Group", "Hear Threatening Solo",
    "Hear Threatening Group", "Lower Your Weapon", "Chatter Solo",
    "Chatter Group", "Take Damage", "Die",
};

TalkKind sTalkKinds[9] = {
    {0x9119500D, 40000.0f, 40000.0f, 15000.0f, 15000.0f, 0, 0},
    {0x916900F1, 40000.0f, 40000.0f, 15000.0f, 15000.0f, 0, 0},
    {0x360397B2, 40000.0f, 40000.0f, 15000.0f, 15000.0f, 0, 0},
    {0x89C7ADEC, 40000.0f, 40000.0f, 15000.0f, 15000.0f, 0, 0},
    {0x3123FBEA, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0},
    {0x76B2A4F6, 40000.0f, 35000.0f, 15000.0f, 13000.0f, 0, 0},
    {0x5EFA4928, 40000.0f, 35000.0f, 15000.0f, 13000.0f, 0, 0},
    {0x65F6E528, 20000.0f, 20000.0f, 15000.0f, 15000.0f, 0, 0},
    {0x00010860, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0},
};

float sTalkKindTimers[10] = {0};

static int sTalkNpcs;
static float* sTalkTimers;
static float sTalkTimer;
static int sTalkCount;
static int sTalkActive;

static TalkKind sTalkLast;

inline float TalkLerp(float lo, float hi, float t) { float d = hi - lo; return lo + d * t; }
#define TalkRand(lo, hi) TalkLerp(lo, hi, (float)rand() / 32767.0f)

const char* TalkName(int kind) {
    if (kind > 0 && kind < 9) {
        return sTalkNames[kind];
    }
    return "";
}

void TalkUpdate(float dt) {
    int i;

    sTalkTimer -= dt;
    for (i = 0; i < sTalkNpcs * sTalkCount; i++) {
        if (sTalkTimers[i] >= 0.0f) {
            sTalkTimers[i] -= dt;
        }
    }
    for (i = 0; i < 9; i++) {
        if (sTalkKindTimers[i] >= 0.0f) {
            sTalkKindTimers[i] -= dt;
        }
    }
}

int TalkPlay(void* owner, int kind) {
    TalkActor* actor;
    int index;
    u32 key;
    int i;
    int offset;

    actor = fn_800A3990(owner);
    if (actor == 0 || MemFlagIsSet(actor->state, 0x11) != 0 || actor->talker < 0 || sTalkKinds[kind].count == 0) {
        return 0;
    }
    if (kind == 8) {
        fn_8016EEF4(actor->sound);
        index = sTalkKinds[kind].start + Rand() % sTalkKinds[kind].count;
        key = sTalkKinds[kind].key;
        if (index > sTalkKinds[kind].start) {
            key = StrHash(" 1", key);
            key += index - sTalkKinds[kind].start;
        }
        actor->sound = fn_8016F120(Fn800DF3B8(TALK_GROUP, actor->talker, key), actor->pos);
        actor->kind = kind;
        actor->variant = index - sTalkKinds[kind].start;
        return 1;
    }
    if (sTalkTimer > 0.0f) {
        return 0;
    }
    if (sTalkKindTimers[kind] > 0.0f) {
        return 0;
    }
    if (sTalkTimers[actor->talker * sTalkCount + kind] > 0.0f) {
        return 0;
    }
    index = -1;
    offset = Rand() % sTalkKinds[kind].count;
    for (i = 0; i < sTalkKinds[kind].count; i++) {
        int slot = sTalkKinds[kind].start + (i + offset) % sTalkKinds[kind].count;
        if (sTalkTimers[actor->talker * sTalkCount + slot] <= 0.0f) {
            index = slot;
            break;
        }
    }
    if (index < 0) {
        return 0;
    }
    switch (kind) {
    case 0:
    case 1:
    case 2:
    case 3:
        sTalkKindTimers[5] = TalkRand(sTalkKinds[5].kindMin, sTalkKinds[5].kindMax);
        sTalkKindTimers[6] = TalkRand(sTalkKinds[6].kindMin, sTalkKinds[6].kindMax);
        break;
    }
    sTalkTimers[actor->talker * sTalkCount + index] = TalkRand(sTalkKinds[kind].npcMin, sTalkKinds[kind].npcMax);
    sTalkKindTimers[kind] = TalkRand(sTalkKinds[kind].kindMin, sTalkKinds[kind].kindMax);
    sTalkTimer = TalkRand(2000.0f, 3000.0f);
    key = sTalkKinds[kind].key;
    if (index > sTalkKinds[kind].start) {
        u32 unused = StrHash(" 1", key) + (index - sTalkKinds[kind].start);
    }
    actor->sound = fn_8016F120(Fn800DF3B8(TALK_GROUP, actor->talker, key), actor->pos);
    actor->kind = kind;
    actor->variant = index - sTalkKinds[kind].start;
    return 1;
}

void TalkFree(void) {
    sTalkNpcs = 0;
    if (sTalkTimers) {
        VFree(sTalkTimers);
        sTalkTimers = 0;
    }
}

void TalkInit(void) {
    int j;
    TalkKind* table = sTalkKinds;
    u32 key;
    int* count;
    TalkKind* kind;
    int i;
    sTalkCount = 0;
    for (kind = table, i = 0; i < 9; i++, kind++) {
        kind->start = sTalkCount;
        kind->count = 0;
        count = &kind->count;
        key = kind->key;
        while (VarExists(TALK_GROUP, 0, key)) {
            sTalkCount++;
            (*count)++;
            if (key == kind->key) {
                key = StrHash(" 2", key);
            } else {
                key++;
            }
        }
    }
    sTalkNpcs = VarGroupObjCount(TALK_GROUP);
    if (sTalkNpcs > 0) {
        sTalkTimers = (float*)VAlloc(sTalkNpcs * sTalkCount * sizeof(float));
        for (i = 0; i < sTalkNpcs * sTalkCount; i++) {
            sTalkTimers[i] = 0.0f;
        }
    } else {
        sTalkTimers = 0;
    }
    sTalkActive = 0;
    sTalkLast = table[0];
}

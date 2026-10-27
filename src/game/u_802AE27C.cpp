// u_802AE27C.cpp: actor behaviour-parameter ("BehParams") message handling and
// per-frame update.  Names and struct layouts are descriptive guesses.

struct BehSlot {
    float timerEnd;                     // 0x00
    float timer;                        // 0x04
    float unk08;                        // 0x08
    int   flag;                         // 0x0c
    int   emitter;                      // 0x10
};

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[12];
};

struct Place {
    unsigned char unk00[0x24];
    int   unk24;                        // 0x24
    int   unk28;                        // 0x28
    int   unk2c;                        // 0x2c
};

struct BehParams {
    int   state;                        // 0x00
    int   state2;                       // 0x04
    int   owner;                        // 0x08
    int   unk0c;                        // 0x0c
    int   unk10;                        // 0x10
    int   unk14;                        // 0x14
    int   unk18;                        // 0x18
    BehSlot slot0;                      // 0x1c
    BehSlot slot1;                      // 0x30
    float progress;                     // 0x44
    float unk48;                        // 0x48
    float fade;                         // 0x4c
    float unk50;                        // 0x50
    float unk54;                        // 0x54
    float unk58;                        // 0x58
    float unk5c;                        // 0x5c
    float unk60;                        // 0x60
    int   unk64;                        // 0x64
};

struct BehStateEntry {
    int   unk00;                        // 0x00
    void (*enter)(void*);               // 0x04
    int  (*change)(void*, int);         // 0x08
    int  (*update)(void*);              // 0x0c
    void* unk10;                        // 0x10
    const char* name;                   // 0x14
};

struct BehActor {
    unsigned char unk000[0x24 - 0x000];
    int   unk24;                        // 0x24
    int   unk28;                        // 0x28
    int   unk2c;                        // 0x2c
    unsigned char unk030[0x38 - 0x30];
    int   unk38;                        // 0x38
    unsigned char unk3c[0x4c - 0x3c];
    int   flags;                        // 0x4c
    unsigned char unk50[0x84 - 0x50];
    float unk84;                        // 0x84
    float unk88;                        // 0x88
    float unk8c;                        // 0x8c
    unsigned char unk090[0xc4 - 0x90];
    void* msgCtx;                       // 0x0c4
    unsigned char unk0c8[0xcc - 0x0c8];
    int   model;                        // 0x0cc
    int   unk0d0;                       // 0x0d0
    void* unk0d4;                       // 0x0d4
    unsigned char unk0d8[0xe0 - 0x0d8];
    int   unk0e0;                       // 0x0e0
    int   unk0e4;                       // 0x0e4
    unsigned char unk0e8[0xf0 - 0x0e8];
    void* unk0f0;                       // 0x0f0  animation node / placement
    unsigned char unk0f4[0x10c - 0x0f4];
    void (*unk10c)(void*);              // 0x10c
    int (*behUpdate)(void*);            // 0x110
    unsigned char unk114[0x118 - 0x114];
    void (*behDelete)(void*);           // 0x118
    unsigned char unk11c[0x134 - 0x11c];
    BehParams* beh;                     // 0x134
    float unk138;                       // 0x138
    float unk13c;                       // 0x13c
    unsigned char unk140[0x220 - 0x140];
    int mode;                           // 0x220
    unsigned char unk224[0x240 - 0x224];
    float unk240;                       // 0x240
    float unk244;                       // 0x244
    float unk248;                       // 0x248
    Vec3  angles;                       // 0x24c  euler angles (x = yaw, z = roll)
    unsigned char unk258[0x26c - 0x258];
    int   unk26c;                       // 0x26c
    unsigned char cond[0x27c - 0x270];  // 0x270
    Vec3  turn;                         // 0x27c  turn rates (z used as a scalar)
    int   unk288;                       // 0x288
    unsigned char unk28c[0x894 - 0x28c];
    unsigned char unk894[0x8f8 - 0x894]; // 0x894  serialisation stream context
    int   unk8f8;                       // 0x8f8
    float unk8fc;                       // 0x8fc
    unsigned char unk900[0x904 - 0x900];
    float unk904;                       // 0x904
    unsigned char unk908[0x91c - 0x908];
    float unk91c;                       // 0x91c
};

struct MsgStream {
    void* actor;                        // 0x00
    int   remaining;                    // 0x04
    unsigned char* cursor;              // 0x08
    int   written;                      // 0x0c
    int   status;                       // 0x10
};

struct StrObj {
    void* vtable;                       // 0x00
    char* data;                         // 0x04
    int   len;                          // 0x08
    int   unk0c;                        // 0x0c
    int   unk10;                        // 0x10
};

struct TypeNameBlob {
    unsigned char unk00[0x48];
    char name[9];                       // 0x48
    char msgName[2];                    // 0x51
    char dieName[10];                   // 0x53  "Ghost_Die"
};

extern "C" BehStateEntry lbl_803C4138[];
extern "C" TypeNameBlob lbl_8034BA64;
extern "C" void* __vt__6String[];
extern "C" unsigned char __upper_map[];
extern "C" float lbl_804DA9F0;
extern "C" float lbl_804DA9F4;
extern "C" float lbl_804DA9F8;
extern "C" float lbl_804DA9FC;
extern "C" float lbl_804DAA00;
extern "C" void* lbl_804D38F8;
extern "C" int rand(void);
extern "C" void fn_800CDD10(void* actor);
extern "C" void fn_80090634(void* handle, int a, int b, int c);
void* VAlloc(int size);
void VFree(void* p);
int Rand();
void ConditionSetBit(void* cond, int bit, int value);
void* FxFind(const char* name);
extern "C" void* memcpy(void* dst, const void* src, unsigned long n);
extern "C" int GhostActorStreamMsg(void* stream, int mode);
extern "C" void GhostActorFrameUpdate(void* actor);
extern "C" void fn_8028C400(void* self, const char* s);
extern "C" void fn_800225C8(void* dst, void* a, void* b);
extern "C" void fn_8028C188(void* out, void* a, int b);
extern "C" void fn_8028B9C0(void* self, int flag);
void EmitterDestroy(int id);
void EmitterSetEnabled(unsigned long id, int enabled);
int HasCondition(void* cond, int id);
int fn_8008EA9C(void* handle, int arg);
extern "C" float lbl_804DAA10;
extern "C" float lbl_804DAA14;
extern "C" float lbl_804DAA18;
int ZoneClassifyDirection(void* self, Vec3* dir);
float __fabsf(float f);
extern "C" void fn_8016D440(Vec3* out, Vec3* in);
extern "C" void fn_8016F120(void* cam, void* pos);
void* GetPlayer();
void EulerNormalize(Vec3* out, Vec3* in);
void MtxSetEuler(Mtx34* mtx, Vec3* vec);
void* OwnerGetInfo(unsigned long id);
void PlacementToMatrix(Place* place, Mtx34* mtx);

// Lazy name hash used to tag the serialised behaviour block: h = h * 31 + upper(c).
static inline unsigned int BehNameHash(const char* str)
{
    unsigned int h = 0;

    if (str != 0) {
        const char* p = str;
        const char* e = p;
        while (*e != 0) {
            unsigned int c;
            if (*p == (char)0xff) {
                c = 0xffffffff;
            } else {
                c = __upper_map[(unsigned char)*p];
            }
            h = h * 31 + (c & 0xff);
            p++;
            e++;
        }
        h = h % 0xffffffffu;
    }
    return h;
}

// Handles the behaviour-parameter messages: state entry (0x803), emitter reset
// (0xc14), the FASSVE string message (0x41e) and the two 0x7102/0x7103 stream
// directions that serialise the 0x68-byte behaviour block under a name hash.
int BehParamsMsgHandler(int msg, void* data)
{
    MsgStream* m = (MsgStream*)data;
    BehParams* beh;
    unsigned int h;
    unsigned int ch;
    unsigned int word;
    unsigned char* s;
    unsigned char* e;
    StrObj strA;
    StrObj strB;
    StrObj strC;
    int ok;

    switch (msg) {
    case 0:
        return GhostActorStreamMsg(m, 0);

    case 0x41e:
        beh = *(BehParams**)((unsigned char*)m->actor + 0x134);
        if (m->remaining != (int)0xAD8576FA) {
            return 0;
        }
        fn_8028C400(&strC, lbl_803C4138[beh->state].name);
        fn_8028C400(&strB, lbl_8034BA64.msgName);
        fn_800225C8(&strA, &strC, &strB);
        fn_8028C188(&m->written, strA.data, strA.len);
        strA.vtable = __vt__6String;
        fn_8028B9C0(&strA, 0);
        strB.vtable = __vt__6String;
        fn_8028B9C0(&strB, 0);
        strC.vtable = __vt__6String;
        fn_8028B9C0(&strC, 0);
        return 0;

    case 0x803:
        beh = *(BehParams**)((unsigned char*)m->actor + 0x134);
        if (beh != 0) {
            lbl_803C4138[beh->state].enter(m->actor);
        }
        return 0;

    case 0xc14:
        beh = *(BehParams**)((unsigned char*)m->actor + 0x134);
        if (m->written != (int)0xFCFCA566) {
            return 0;
        }
        if (beh->slot0.emitter != -1) {
            EmitterDestroy(beh->slot0.emitter);
            beh->slot0.emitter = -1;
        }
        if (beh->slot1.emitter != -1) {
            EmitterDestroy(beh->slot1.emitter);
            beh->slot1.emitter = -1;
        }
        return 0;

    case 0x3300:
        return 1;

    case 0x7102:
        beh = *(BehParams**)((unsigned char*)m->actor + 0x134);
        if (beh == 0) {
            return 1;
        }
        if (m->remaining < 0x6c) {
            m->status = 0;
            return 1;
        }
        h = BehNameHash(lbl_8034BA64.name);
        memcpy(m->cursor, &h, 4);
        m->written += 4;
        m->remaining -= 4;
        m->cursor += 4;
        memcpy(m->cursor, beh, 0x68);
        m->written += 0x68;
        m->remaining -= 0x68;
        m->cursor += 0x68;
        return 1;

    case 0x7103:
        beh = *(BehParams**)((unsigned char*)m->actor + 0x134);
        if (beh == 0) {
            return 1;
        }
        if (m->written < 0x6c) {
            m->status = 0;
            return 1;
        }
        ok = 0;
        if (m->written >= 4) {
            word = *(unsigned int*)m->cursor;
            m->written += 4;
            m->remaining -= 4;
            m->cursor += 4;
            ok = (word == BehNameHash(lbl_8034BA64.name));
        }
        if (!ok) {
            return 1;
        }
        memcpy(beh, m->cursor, 0x68);
        m->written -= 0x68;
        m->cursor += 0x68;
        return 1;

    case 0x7300:
        return GhostActorStreamMsg(m, 1);
    }
    return 0;
}

// Per-frame behaviour-parameter update: emitter timers, the two timed slots, the
// 0x8fc pulse value and the state machine step through lbl_803C413C.
int BehParamsUpdate(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    BehSlot* slot;
    int i;
    int next;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    if (HasCondition(a->cond, 9) && *(int*)a->msgCtx == (int)0xFE14BFB5) {
        a->unk13c = lbl_804DA9F0;
    }
    if (beh->fade > lbl_804DA9F0) {
        beh->fade = beh->fade - a->unk138;
    }
    if (HasCondition(a->cond, 9)) {
        beh->progress = beh->progress + a->unk138 / a->unk91c;
    }
    for (i = 0; i < 2; i++) {
        slot = &beh->slot0 + i;
        if (slot->emitter != -1 && slot->flag != 0) {
            if (slot->timer > lbl_804DA9F0) {
                slot->timer = slot->timer - a->unk138 / a->unk91c;
                if (slot->timer <= lbl_804DA9F0) {
                    EmitterSetEnabled(slot->emitter, 0);
                }
            }
            if (slot->timerEnd > lbl_804DA9F0) {
                slot->timerEnd = slot->timerEnd - a->unk138 / a->unk91c;
                if (slot->timerEnd <= lbl_804DA9F0) {
                    EmitterDestroy(slot->emitter);
                    slot->emitter = -1;
                }
            }
        }
    }
    if (a->unk8fc >= lbl_804DA9F4 && HasCondition(a->cond, 9) && !HasCondition(a->cond, 10)) {
        if (beh->slot0.emitter == -1 && beh->slot1.emitter == -1) {
            a->unk8fc = lbl_804DA9F8;
        } else {
            a->unk8fc = lbl_804DA9F4;
        }
    }
    next = fn_8008EA9C(a->unk0d4, 0);
    if ((next == 0 || fn_8008EA9C(a->unk0d4, 0) == 2) &&
        !HasCondition(a->cond, 0xd) && !HasCondition(a->cond, 10)) {
        lbl_803C4138[beh->state].enter(a);
    }
    if (a->mode == 4) {
        if (!HasCondition(a->cond, 9) || *(int*)a->msgCtx != (int)0xFE14BFB5) {
            GhostActorFrameUpdate(a);
            next = beh->state;
        } else {
            next = lbl_803C4138[beh->state].update(a);
        }
    } else {
        next = lbl_803C4138[beh->state].update(a);
    }
    if (next != beh->state && lbl_803C4138[beh->state].change(a, next) != 0) {
        lbl_803C4138[next].enter(a);
    }
    return 1;
}

// Destroys an actor's behaviour block: any live emitters go first, then the
// per-frame callback is cleared and the block itself is freed.
void BehParamsFree(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh = a->beh;

    if (beh->slot0.emitter != -1) {
        EmitterDestroy(beh->slot0.emitter);
        beh->slot0.emitter = -1;
    }
    if (beh->slot1.emitter != -1) {
        EmitterDestroy(beh->slot1.emitter);
        beh->slot1.emitter = -1;
    }
    a->behUpdate = 0;
    VFree(beh);
}

// Allocates and initialises the 0x68-byte behaviour block of an actor and hooks
// the update/delete callbacks into it.
void BehParamsInit(int id, void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;

    if (a == 0) {
        return;
    }
    beh = (BehParams*)VAlloc(0x68);
    if (beh == 0) {
        a->beh = 0;
        return;
    }
    a->beh = beh;
    a->behUpdate = BehParamsUpdate;
    a->behDelete = fn_800CDD10;
    beh->owner = id;
    beh->unk0c = -1;
    beh->unk10 = -1;
    beh->unk18 = 0;
    beh->unk14 = -1;
    beh->unk60 = lbl_804DA9F0;
    beh->unk5c = lbl_804DA9F0;
    beh->unk58 = lbl_804DA9F0;
    beh->unk50 = lbl_804DAA00 * ((float)rand() / lbl_804DA9FC) + lbl_804DAA00;
    beh->unk54 = (float)(Rand() % 1000);
    beh->unk48 = lbl_804DA9F0;
    beh->fade = lbl_804DA9F0;
    beh->unk64 = 0;
    beh->slot0.emitter = -1;
    beh->slot1.emitter = -1;
    beh->progress = lbl_804DA9F0;
    if (*(int*)a->msgCtx == (int)0xFC5A4B84 || *(int*)a->msgCtx == (int)0xFE14BFB5) {
        beh->unk64 = 0x5dc;
    }
    if (a->unk904 <= lbl_804DA9F0) {
        beh->state = 0xb;
        beh->state2 = 0xc;
    } else {
        beh->state = 0;
        beh->state2 = 0;
    }
    ConditionSetBit((int*)a->cond, 0, 0);
    fn_80090634(a->unk0d4, 0, 0, 0);
    lbl_804D38F8 = FxFind(lbl_8034BA64.dieName);
}

extern "C" int fn_8016325C(unsigned char* stream);
extern "C" void fn_80162610(void* owner, unsigned char* stream, int msg);

// Message 0x1b83833f: ask the current behaviour state to switch to the queued
// state, then enter that state.
int BehParamsStateChangeMsg(int msg, void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;

    if (a == 0) {
        return 0;
    }
    switch (msg) {
    case 0x1B83833F:
        beh = a->beh;
        lbl_803C4138[beh->state].change(a, beh->state2);
        lbl_803C4138[beh->state2].enter(a);
        break;
    }
    return 1;
}

// The same state transition, plus re-arming the serialisation stream: once the
// transition is done a pending stream is asked to re-send 0x1b83833f, while
// message 0x20ee36 turns the pending states 4 and 5 into 0x20ee36 / 0xf829db93.
int BehParamsStreamStateMsg(int msg, void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    int state;

    if (a == 0) {
        return 0;
    }
    switch (msg) {
    case 0x1B83833F:
        beh = a->beh;
        lbl_803C4138[beh->state].change(a, beh->state2);
        lbl_803C4138[beh->state2].enter(a);
        if (fn_8016325C(a->unk894) != 0) {
            fn_80162610(a, a->unk894, 0x1B83833F);
        }
        break;

    case 0x20EE36:
        state = fn_8016325C(a->unk894);
        if (state == 4) {
            fn_80162610(a, a->unk894, 0x20EE36);
        } else if (fn_8016325C(a->unk894) == 5) {
            fn_80162610(a, a->unk894, 0xF829DB93);
        }
        break;
    }
    return 1;
}

// Clears the actor's 0x240/0x244/0x248 float triple (motion/mirror values) and
// condition bit 0; returns whether it had an actor to work on.
bool ResetActorVelocityAndCondition(void* selfPtr)
{
    BehActor* a = (BehActor*)selfPtr;

    if (a == 0) {
        return false;
    }
    a->unk240 = 0.0f;
    a->unk244 = 0.0f;
    a->unk248 = 0.0f;
    ConditionSetBit(a->cond, 0, 0);
    return true;
}

// Stores a value into the secondary state field (0x04) of the actor's behaviour
// block; returns whether the actor was non-null.
bool SetBehSecondaryState(void* selfPtr, int value)
{
    BehActor* a = (BehActor*)selfPtr;

    if (a == 0) {
        return false;
    }
    a->beh->state2 = value;
    return true;
}

// Spawns the actor's ghost at the player-relative position for each free timed
// slot, then returns the behaviour block's main state.
int BehParamsSpawnGhost(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    BehSlot* slot;
    int i;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    if (*(int*)a->msgCtx == (int)0xFE14BFB5 && HasCondition(a->cond, 9) &&
        !HasCondition(a->cond, 10) && !HasCondition(a->cond, 0xb) && beh->state != 0xb) {
        return 0xb;
    }
    if (beh->state != 0xc) {
        GhostActorFrameUpdate(a);
    }
    for (i = 0; i < 2; i++) {
        slot = &beh->slot0 + i;
        if (slot->emitter != -1 && slot->flag == 0) {
            if (slot->unk08 <= beh->progress) {
                void* player = GetPlayer();
                Vec3 delta;
                Vec3 euler;
                Mtx34 mtx;
                Place* info;

                delta.x = *(float*)((unsigned char*)player + 0x67c) - a->unk84;
                delta.y = *(float*)((unsigned char*)player + 0x680) - a->unk88;
                delta.z = *(float*)((unsigned char*)player + 0x684) - a->unk8c;
                fn_8016D440(&euler, &delta);
                euler.x = euler.x + lbl_804DAA10;
                EulerNormalize(&euler, &euler);
                MtxSetEuler(&mtx, &euler);
                info = (Place*)OwnerGetInfo(slot->emitter);
                if (info != 0) {
                    PlacementToMatrix(info, &mtx);
                    info->unk24 = a->unk24;
                    info->unk28 = a->unk28;
                    info->unk2c = a->unk2c;
                }
                if (i == 0) {
                    fn_8016F120(lbl_804D38F8, &a->unk24);
                }
                EmitterSetEnabled(slot->emitter, 1);
                slot->flag = 1;
            }
        }
    }
    return beh->state;
}

// Clears the actor's behaviour bookkeeping and reports whether the ghost may
// enter its spawn state (0xb) unless one of the guard conditions is set.
int BehParamsGhostStateCheck(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;

    if (a == 0) {
        return 0;
    }
    GhostActorFrameUpdate(a);
    beh = a->beh;
    if (!HasCondition(a->cond, 9) || HasCondition(a->cond, 10) ||
        HasCondition(a->cond, 0xb)) {
        return beh->state;
    }
    return 0xb;
}

// Picks the ghost's resting behaviour state once it has lost the player: 0xb
// while the sight conditions are all clear, 0xd when the kill condition is up,
// 6 when the player is in range but not yet seen, otherwise 3, 4 or the
// current state depending on the possession flag and the turn rate.
int BehParamsGhostIdleState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;

    if (a == 0) {
        return 0;
    }
    GhostActorFrameUpdate(a);
    beh = a->beh;
    if (!HasCondition(a->cond, 9) || HasCondition(a->cond, 10) ||
        HasCondition(a->cond, 0xb)) {
        if (HasCondition(a->cond, 0x19)) {
            return 0xd;
        }
        if (HasCondition(a->cond, 4) && !HasCondition(a->cond, 9) &&
            !HasCondition(a->cond, 10) && beh->fade <= lbl_804DA9F0) {
            return 6;
        }
        if (a->unk26c == 0) {
            return 3;
        }
        if (lbl_804DAA14 <= __fabsf(a->turn.z)) {
            return beh->state;
        }
        return 4;
    }
    return 0xb;
}

// Picks the ghost's hunting behaviour state: 5 when the target direction
// changed, 1 without possession, 4 when the direction query says "no target",
// 7 when the target sits more than lbl_804DAA18 off the facing, else the
// current state.
int BehParamsGhostChaseState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    float turn;
    int dir;

    if (a == 0) {
        return 5;
    }
    GhostActorFrameUpdate(a);
    beh = a->beh;
    if (!HasCondition(a->cond, 9) || HasCondition(a->cond, 10) ||
        HasCondition(a->cond, 0xb)) {
        if (HasCondition(a->cond, 0x19)) {
            return 0xd;
        }
        if (HasCondition(a->cond, 4) && !HasCondition(a->cond, 9) &&
            !HasCondition(a->cond, 10) && beh->fade <= lbl_804DA9F0) {
            return 6;
        }
        if (a->unk26c == 0) {
            return 1;
        }
        if (fn_8016325C(a->unk894) == 0) {
            return 4;
        }
        dir = ZoneClassifyDirection(a, (Vec3*)&a->unk240);
        if (beh->unk18 != dir) {
            beh->unk18 = dir;
            return 5;
        }
        turn = a->turn.z;
        if (turn < lbl_804DA9F0) {
            turn = -turn;
        }
        if (turn <= lbl_804DAA18) {
            return beh->state;
        }
        return 7;
    }
    return 0xb;
}

// Picks the ghost's next state while its spawn payload is armed: 0xb when all
// the sight conditions are clear, 0xd on the kill condition, 6 when the player
// is in range but unseen, 10 without a payload, 5 while the serialised context
// is in its reset states, 4 when the target direction changed, 7 when the
// facing is too far off the target, else the current state.
int BehParamsGhostSpawnState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    float turn;
    int dir;

    if (a == 0) {
        return 4;
    }
    GhostActorFrameUpdate(a);
    beh = a->beh;
    if (!HasCondition(a->cond, 9) || HasCondition(a->cond, 10) ||
        HasCondition(a->cond, 0xb)) {
        if (HasCondition(a->cond, 0x19)) {
            return 0xd;
        }
        if (HasCondition(a->cond, 4) && !HasCondition(a->cond, 9) &&
            !HasCondition(a->cond, 10) && beh->fade <= lbl_804DA9F0) {
            return 6;
        }
        if (a->unk288 == 0) {
            return 10;
        }
        if (fn_8016325C(a->unk894) == 4 || fn_8016325C(a->unk894) == 5) {
            return 5;
        }
        dir = ZoneClassifyDirection(a, (Vec3*)&a->unk240);
        if (beh->unk18 != dir) {
            beh->unk18 = dir;
            return 4;
        }
        turn = a->turn.z;
        if (turn < lbl_804DA9F0) {
            turn = -turn;
        }
        if (turn <= lbl_804DAA18) {
            return beh->state;
        }
        return 7;
    }
    return 0xb;
}

// Picks the ghost's next state while it waits on the player: 0xb when all the
// sight conditions are clear, 0xd on the kill condition, 2 when the player is in
// range but unseen, 7 when the payload is pending, otherwise 0 while the facing
// is inside the watch cone and the current state once it is not.
int BehParamsGhostWatchState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;

    if (a == 0) {
        return 0;
    }
    GhostActorFrameUpdate(a);
    beh = a->beh;
    if (!HasCondition(a->cond, 9) || HasCondition(a->cond, 10) ||
        HasCondition(a->cond, 0xb)) {
        if (HasCondition(a->cond, 0x19)) {
            return 0xd;
        }
        if (HasCondition(a->cond, 4) && !HasCondition(a->cond, 9) &&
            !HasCondition(a->cond, 10) && beh->fade <= lbl_804DA9F0) {
            return 2;
        }
        if (a->unk26c == 0) {
            if (lbl_804DAA14 <= __fabsf(a->turn.z)) {
                return beh->state;
            }
            return 0;
        }
        return 7;
    }
    return 0xb;
}

// Picks the ghost's next state while it tracks the player after losing sight:
// 0xb when the sight conditions are all clear, 0xd on the kill condition, 2 when
// the player is in range but unseen, 5 while the possession flag is up, 0 when
// the serialised context is unarmed, 3 when the facing is too far off target,
// else the current state.
int BehParamsGhostFollowState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    float turn;

    if (a == 0) {
        return 1;
    }
    GhostActorFrameUpdate(a);
    beh = a->beh;
    if (!HasCondition(a->cond, 9) || HasCondition(a->cond, 10) ||
        HasCondition(a->cond, 0xb)) {
        if (HasCondition(a->cond, 0x19)) {
            return 0xd;
        }
        if (HasCondition(a->cond, 4) && !HasCondition(a->cond, 9) &&
            !HasCondition(a->cond, 10) && beh->fade <= lbl_804DA9F0) {
            return 2;
        }
        if (a->unk26c != 0) {
            return 5;
        }
        if (fn_8016325C(a->unk894) == 0) {
            return 0;
        }
        turn = a->turn.z;
        if (turn < lbl_804DA9F0) {
            turn = -turn;
        }
        if (turn <= lbl_804DAA18) {
            return beh->state;
        }
        return 3;
    }
    return 0xb;
}

// Picks the ghost's next state while it waits on the player after the serialised
// context reset: 0xd on the kill condition, 2 when the player is in range but
// unseen, 9 while the possession flag is up, 1 while the serialised context is
// in its reset states, 3 when the facing is too far off target, 8 once the
// behaviour's 0x50 countdown has run out, else the current state.
int BehParamsGhostWaitState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    float turn;
    float timer;

    if (a == 0) {
        return 0;
    }
    GhostActorFrameUpdate(a);
    beh = a->beh;
    if (!HasCondition(a->cond, 9) || HasCondition(a->cond, 10) ||
        HasCondition(a->cond, 0xb)) {
        if (HasCondition(a->cond, 0x19)) {
            return 0xd;
        }
        if (HasCondition(a->cond, 4) && !HasCondition(a->cond, 9) &&
            !HasCondition(a->cond, 10) && beh->fade <= lbl_804DA9F0) {
            return 2;
        }
        if (a->unk26c != 0) {
            return 9;
        }
        if (fn_8016325C(a->unk894) == 4 || fn_8016325C(a->unk894) == 5) {
            return 1;
        }
        turn = a->turn.z;
        if (turn < lbl_804DA9F0) {
            turn = -turn;
        }
        if (turn > lbl_804DAA18) {
            return 3;
        }
        if (beh->state != 8) {
            timer = beh->unk50 - a->unk138;
            beh->unk50 = timer;
            if (timer <= lbl_804DA9F0) {
                return 8;
            }
        }
        return beh->state;
    }
    return 0xb;
}

struct MsgBuf {
    unsigned char unk00[0x14];
    void* unk14;                        // 0x14
    unsigned char unk18[0x1c - 0x18];
    void* unk1c;                        // 0x1c
};

extern "C" void fn_8008F4E8(void* handle, int arg);
extern "C" void fn_8008F378(void* handle);
extern "C" void fn_8008F4EC(void* handle, int arg);
extern "C" int fn_800A1BC8(void* a, void* b, int c, int d, int e);
extern "C" void fn_80090974(float f, void* handle, int a, int b, int c, void* cb, void* actor);
extern "C" void fn_8008F230(float f, void* handle, int a, int b);
extern "C" void fn_8008DE4C(void* handle, int a, int b, int c);
extern "C" int fn_802A1D04(void);
extern "C" void fn_800FC764(int id, int arg);
extern "C" float lbl_804DAA1C;
void RangeResetFields(void* self);
void ActorPlayAnim(int handle, unsigned long animId, int arg);
struct Owner;
int OwnerId(Owner* owner);

// Behaviour update for state 0xd: arms condition 0xc, resets the emitter and
// plays the state's effect callback with the object id from the state table.
int BehParamsGhostCueState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* buf;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 0xd;
    beh->state2 = 8;
    beh->unk10 = beh->unk0c;
    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        buf = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                  lbl_803C4138[beh->state].unk00, 0, 0);
        fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
    return 1;
}

// Behaviour update for state 0xc: installs the range reset callback, replays
// the reveal animation once and releases the owner's held state.
int BehParamsGhostRevealState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    int id;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 0xc;
    beh->state2 = 0xc;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    a->unk10c = RangeResetFields;
    a->unk8f8 = 6;
    fn_8008DE4C(a->unk0d4, 0, beh->unk0c, 0x18);
    if (!HasCondition(a->cond, 10)) {
        ConditionSetBit(a->cond, 10, 1);
        ActorPlayAnim(a->unk38, 0xDC04E3D5, a->unk38);
    }
    if (fn_802A1D04() != 0) {
        id = OwnerId((Owner*)a);
        fn_800FC764(id, 0);
    }
    return 1;
}

extern "C" float lbl_804DAA20;
extern "C" float lbl_804DAA24;
extern "C" float lbl_804DAA28;
extern "C" float lbl_804DAA2C;
extern "C" float lbl_804DAA30;
extern "C" float lbl_804DAA34;
extern "C" float lbl_804DAA38;
extern "C" float lbl_804DAA3C;
extern "C" float lbl_804DAA40;
extern "C" float lbl_804DAA44;
extern "C" float lbl_804DAA48;
extern "C" float lbl_804DAA4C;
extern "C" float lbl_804DAA50;
extern "C" float lbl_804DAA54;
extern "C" float lbl_804DAA58;
extern "C" float lbl_804DAA5C;
extern "C" float lbl_804DAA60;
extern "C" float lbl_804DAA64;
extern "C" float lbl_804DAA68;
extern "C" float lbl_804DAA6C;
extern "C" float lbl_804DAA70;
extern "C" float lbl_804DAA74;
extern "C" float lbl_804DAA78;
extern "C" float lbl_804DAA7C;
extern "C" float lbl_804DAA80;
extern "C" float lbl_804DAA84;

// Emitter creation descriptor; only the fields the game sets are named.  The
// gaps are left untouched by the original code, so they stay explicit.
struct EmitterDesc {
    float posX;                         // 0x00
    float posY;                         // 0x04
    float posZ;                         // 0x08
    float unk0c;                        // 0x0c
    float unk10;                        // 0x10
    float unk14;                        // 0x14
    float unk18;                        // 0x18
    int   unk1c;                        // 0x1c
    int   unk20;                        // 0x20
    float unk24;                        // 0x24
    int   unk28;                        // 0x28
    void* asset;                        // 0x2c
    unsigned char unk30;                // 0x30
    unsigned char unk31[0x6c - 0x31];
    float unk6c;                        // 0x6c
    float unk70;                        // 0x70
    float unk74;                        // 0x74
    float unk78;                        // 0x78
    int   unk7c;                        // 0x7c
    float unk80;                        // 0x80
    float unk84;                        // 0x84
    int   unk88;                        // 0x88
    float unk8c;                        // 0x8c
    float unk90;                        // 0x90
    float unk94;                        // 0x94
    int   unk98;                        // 0x98
    int   unk9c;                        // 0x9c
    void* unkA0;                        // 0xa0
    int   unkA4;                        // 0xa4
    float unkA8;                        // 0xa8
    float unkAC;                        // 0xac
    float unkB0;                        // 0xb0
    float unkB4;                        // 0xb4
    float unkB8;                        // 0xb8
    float unkBC;                        // 0xbc
    float unkC0;                        // 0xc0
    float unkC4;                        // 0xc4
    float unkC8;                        // 0xc8
    unsigned char colorCC[8];           // 0xcc
    int   unkD4;                        // 0xd4
    int   unkD8;                        // 0xd8
    int   unkDC;                        // 0xdc
    int   unkE0;                        // 0xe0
    int   unkE4;                        // 0xe4
};

// Shared emitter parameters the ghost points its descriptor at.
struct EmitterBase {
    float posX;                         // 0x00
    float posY;                         // 0x04
    float posZ;                         // 0x08
    float unk0c;                        // 0x0c
    float unk10;                        // 0x10
    float unk14;                        // 0x14
    float unk18;                        // 0x18
    int   unk1c;                        // 0x1c
    float unk20;                        // 0x20
    float unk24;                        // 0x24
};

extern "C" EmitterBase lbl_804A4950;
void* AssetFind(unsigned long id, unsigned long id2);
int EmitterCreate(EmitterDesc* desc);
void RangeResetPending(void* cond);
extern "C" void fn_800913EC(void* handle, float f, void* out);

// Behaviour update for state 0xb: builds the two ghost emitters once, installs
// the slot timing constants and switches the object into the 0xb state.
int BehParamsGhostEmitState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* ctx;
    EmitterDesc descA;
    EmitterDesc descB;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    if (beh->state != 0xb) {
        descA.posX = a->unk84;
        descA.posY = a->unk88;
        descA.posZ = a->unk8c;
        descA.unk0c = lbl_804DA9F0;
        descA.unk10 = lbl_804DAA20;
        descA.unk14 = lbl_804DAA20;
        descA.unk18 = lbl_804DA9F0;
        descA.unk1c = 1;
        descA.unk20 = 0;
        descA.unk24 = lbl_804DAA24;
        descA.unk28 = 1;
        descA.asset = AssetFind(0xF2E2BEB8, 0x60A26EDB);
        descA.unk30 = 0xfe;
        descA.unk6c = lbl_804DAA28;
        descA.unk70 = lbl_804DAA2C;
        descA.unk74 = lbl_804DA9F0;
        descA.unk78 = lbl_804DA9F0;
        descA.unk7c = 1000;
        descA.unk80 = lbl_804DAA30;
        descA.unk84 = lbl_804DAA14;
        descA.unk88 = 8;
        descA.unk8c = lbl_804DA9F0;
        descA.unk90 = lbl_804DAA14;
        descA.unk94 = lbl_804DA9F0;
        descA.unk98 = 1;
        descA.unk9c = 1;

        lbl_804A4950.posX = a->unk84;
        lbl_804A4950.posY = a->unk88;
        lbl_804A4950.posZ = a->unk8c;
        lbl_804A4950.unk0c = lbl_804DAA34;
        lbl_804A4950.unk10 = lbl_804DA9F0;
        lbl_804A4950.unk14 = lbl_804DAA20;
        lbl_804A4950.unk18 = lbl_804DA9F0;
        lbl_804A4950.unk1c = 1;
        lbl_804A4950.unk20 = lbl_804DAA38;
        lbl_804A4950.unk24 = lbl_804DA9F0;

        descA.unkA0 = &lbl_804A4950;
        descA.unkA4 = 0;
        descA.unkAC = lbl_804DA9F0;
        descA.unkB0 = lbl_804DA9F0;
        descA.unkB4 = lbl_804DAA3C;
        descA.unkB8 = lbl_804DA9F0;
        descA.unkBC = lbl_804DA9F0;
        descA.unkC0 = lbl_804DA9F0;
        descA.unkC4 = lbl_804DA9F0;
        descA.unkC8 = lbl_804DAA40;
        descA.colorCC[0] = 0xff;
        descA.colorCC[1] = 0xff;
        descA.colorCC[2] = 0xff;
        descA.colorCC[3] = 0xff;
        descA.colorCC[4] = 0xff;
        descA.colorCC[5] = 0xff;
        descA.colorCC[6] = 0xff;
        descA.colorCC[7] = 0xff;
        descA.unkD4 = -1;
        descA.unkD8 = -1;
        descA.unkDC = 0;
        descA.unkE0 = 0;
        descA.unkE4 = 0;

        beh->slot0.emitter = EmitterCreate(&descA);
        EmitterSetEnabled((unsigned long)beh->slot0.emitter, 0);
        ctx = (MsgBuf*)a->msgCtx;
        if (*(int*)ctx == (int)0xFE14BFB5) {
            beh->slot0.timer = lbl_804DAA44;
            beh->slot0.timerEnd = lbl_804DAA48;
            beh->slot0.unk08 = lbl_804DAA4C;
        } else {
            beh->slot0.timer = lbl_804DAA44;
            beh->slot0.timerEnd = lbl_804DAA48;
            beh->slot0.unk08 = lbl_804DAA50;
        }
        beh->slot0.flag = 0;

        descB.posX = a->unk84;
        descB.posY = a->unk88;
        descB.posZ = a->unk8c;
        descB.unk0c = lbl_804DAA34;
        descB.unk10 = lbl_804DA9F0;
        descB.unk14 = lbl_804DA9F0;
        descB.unk18 = lbl_804DAA34;
        descB.unk1c = 1;
        descB.unk20 = 2;
        descB.unk24 = lbl_804DAA54;
        descB.unk28 = 1;
        descB.asset = AssetFind(0xF2E2BEB8, 0x60A26EDB);
        descB.unk30 = 0xfe;
        descB.unk6c = lbl_804DAA58;
        descB.unk70 = lbl_804DAA5C;
        descB.unk74 = lbl_804DA9F0;
        descB.unk78 = lbl_804DAA60;
        descB.unk7c = 5;
        descB.unk80 = lbl_804DAA64;
        descB.unk84 = lbl_804DAA14;
        descB.unk88 = 0x10;
        descB.unk8c = lbl_804DA9F0;
        descB.unk90 = lbl_804DAA68;
        descB.unk94 = lbl_804DAA6C;
        descB.unk98 = 1;
        descB.unk9c = 0;
        descB.unkA4 = 0;
        descB.unkAC = lbl_804DAA70;
        descB.unkB0 = lbl_804DA9F0;
        descB.unkB4 = lbl_804DAA3C;
        descB.unkB8 = lbl_804DA9F0;
        descB.unkBC = lbl_804DA9F0;
        descB.unkC0 = lbl_804DA9F0;
        descB.unkC4 = lbl_804DA9F0;
        descB.unkC8 = lbl_804DAA3C;
        descB.colorCC[0] = 0xd6;
        descB.colorCC[1] = 0xe4;
        descB.colorCC[2] = 0x99;
        descB.colorCC[3] = 0xff;
        descB.colorCC[4] = 0xff;
        descB.colorCC[5] = 0xff;
        descB.colorCC[6] = 0xff;
        descB.colorCC[7] = 0xff;
        descB.unkD4 = -1;
        descB.unkD8 = -1;
        descB.unkDC = 0;
        descB.unkE0 = 0;
        descB.unkE4 = 0;

        beh->slot1.emitter = EmitterCreate(&descB);
        EmitterSetEnabled((unsigned long)beh->slot1.emitter, 0);
        if (*(int*)ctx == (int)0xFE14BFB5) {
            beh->slot1.timer = lbl_804DAA74;
            beh->slot1.timerEnd = lbl_804DAA78;
            beh->slot1.unk08 = lbl_804DAA7C;
        } else {
            beh->slot1.timer = lbl_804DAA74;
            beh->slot1.timerEnd = lbl_804DAA78;
            beh->slot1.unk08 = lbl_804DAA80;
        }
        beh->slot1.flag = 0;
    }

    beh->state = 0xb;
    beh->state2 = 0xb;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    ctx = (MsgBuf*)a->msgCtx;
    if (*(int*)ctx == (int)0xFE14BFB5) {
        beh->unk0c = fn_800A1BC8(ctx->unk14, ctx->unk1c,
                                  lbl_803C4138[0xb].unk00, 0, 0);
        ConditionSetBit(a->cond, 0xe, 1);
        fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 1);
        a->mode = 4;
        ConditionSetBit(a->cond, 0xd, 1);
        fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 0x58,
                    lbl_803C4138[0xb].unk10, a);
        RangeResetPending(a->cond);
        ConditionSetBit(a->cond, 0xc, 1);
        ConditionSetBit(a->cond, 0xd, 1);
        a->unk13c = lbl_804DA9F0;
        fn_800913EC(a->unk0d4, lbl_804DAA14, 0);
    } else {
        ConditionSetBit(a->cond, 9, 1);
        ConditionSetBit(a->cond, 0, 1);
        fn_800A1BC8(ctx->unk14, ctx->unk1c, lbl_803C4138[0xb].unk00, 0, 0);
        if (a != 0) {
            beh = a->beh;
            if (HasCondition(a->cond, 0xc)) {
                ConditionSetBit(a->cond, 0xc, 0);
                ConditionSetBit(a->cond, 0xe, 1);
                fn_8008F4E8(a->unk0d4, 0);
                fn_8008F378(a->unk0d4);
                fn_8008F4EC(a->unk0d4, 0);
            }
            beh->unk0c = fn_800A1BC8(ctx->unk14, ctx->unk1c,
                                      lbl_803C4138[beh->state].unk00, 0, 0);
            fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                        lbl_803C4138[beh->state].unk10, a);
            if (HasCondition(a->cond, 0xe)) {
                fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
                ConditionSetBit(a->cond, 0xe, 0);
            } else if (beh->unk10 == beh->unk0c) {
                fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            } else {
                fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
            }
        }
    }
    return 1;
}

// Behaviour update for state 0xa: resets the slot timing, replays the state
// cursor and starts the state's emitter cue.
int BehParamsGhostApproachState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* ctx;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 0xa;
    beh->state2 = 0;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        ctx = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(ctx->unk14, ctx->unk1c,
                                  lbl_803C4138[beh->state].unk00, 0, 0);
        fn_80090974(lbl_804DAA84, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
    return 1;
}

extern "C" int AnimBankGetSetCount(unsigned short a, unsigned short b, unsigned short c);
extern "C" float lbl_804DAA88;
extern "C" float lbl_804DAA8C;

// Behaviour update for state 9: plays the first animation set of the state's
// bank and primes the fade emitter, like the other ghost states.
int BehParamsGhostSearchState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* buf;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 9;
    beh->state2 = 4;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        buf = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                  lbl_803C4138[beh->state].unk00, 0, 0);
        fn_80090974(lbl_804DAA88, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
    return 1;
}

// Behaviour update for state 8: randomises the roam timer, then plays a random
// animation set of the state's bank, or falls back to state 0 if there is none.
int BehParamsGhostRoamState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* ctx;
    int setCount;
    int set;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 8;
    beh->state2 = 0;
    beh->unk50 = lbl_804DAA00 * ((float)(unsigned int)rand() / lbl_804DA9FC) + lbl_804DAA00;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    ctx = (MsgBuf*)a->msgCtx;
    setCount = AnimBankGetSetCount((int)ctx->unk14, (int)ctx->unk1c,
                                   lbl_803C4138[beh->state].unk00);
    if (setCount < 1) {
        if (a != 0) {
            beh = a->beh;
            beh->state = 0;
            beh->state2 = 0;
            beh->unk18 = 0;
            beh->unk10 = beh->unk0c;
            if (a != 0) {
                beh = a->beh;
                if (HasCondition(a->cond, 0xc)) {
                    ConditionSetBit(a->cond, 0xc, 0);
                    ConditionSetBit(a->cond, 0xe, 1);
                    fn_8008F4E8(a->unk0d4, 0);
                    fn_8008F378(a->unk0d4);
                    fn_8008F4EC(a->unk0d4, 0);
                }
                ctx = (MsgBuf*)a->msgCtx;
                beh->unk0c = fn_800A1BC8(ctx->unk14, ctx->unk1c,
                                          lbl_803C4138[beh->state].unk00, 0, 0);
                fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                            lbl_803C4138[beh->state].unk10, a);
                if (HasCondition(a->cond, 0xe)) {
                    fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
                    ConditionSetBit(a->cond, 0xe, 0);
                } else if (beh->unk10 == beh->unk0c) {
                    fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
                } else {
                    fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
                }
            }
        }
        return 1;
    }
    set = rand() % setCount;
    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        ctx = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(ctx->unk14, ctx->unk1c,
                                  lbl_803C4138[beh->state].unk00, set, 0);
        fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
    return 1;
}

// Behaviour update for state 7: switches the block to the 7 state and re-arms
// the state's effect, choosing between the two cue variants by the size of the
// actor's 0x284 turn value.
int BehParamsGhostTurnState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* buf;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 7;
    beh->state2 = 7;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    if (a->turn.z <= lbl_804DA9F0) {
        if (a != 0) {
            beh = a->beh;
            if (HasCondition(a->cond, 0xc)) {
                ConditionSetBit(a->cond, 0xc, 0);
                ConditionSetBit(a->cond, 0xe, 1);
                fn_8008F4E8(a->unk0d4, 0);
                fn_8008F378(a->unk0d4);
                fn_8008F4EC(a->unk0d4, 0);
            }
            buf = (MsgBuf*)a->msgCtx;
            beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                      lbl_803C4138[beh->state].unk00, 1, 0);
            fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                        lbl_803C4138[beh->state].unk10, a);
            if (HasCondition(a->cond, 0xe)) {
                fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
                ConditionSetBit(a->cond, 0xe, 0);
            } else if (beh->unk10 == beh->unk0c) {
                fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            } else {
                fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
            }
        }
    } else {
        if (a != 0) {
            beh = a->beh;
            if (HasCondition(a->cond, 0xc)) {
                ConditionSetBit(a->cond, 0xc, 0);
                ConditionSetBit(a->cond, 0xe, 1);
                fn_8008F4E8(a->unk0d4, 0);
                fn_8008F378(a->unk0d4);
                fn_8008F4EC(a->unk0d4, 0);
            }
            buf = (MsgBuf*)a->msgCtx;
            beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                      lbl_803C4138[beh->state].unk00, 0, 0);
            fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                        lbl_803C4138[beh->state].unk10, a);
            if (HasCondition(a->cond, 0xe)) {
                fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
                ConditionSetBit(a->cond, 0xe, 0);
            } else if (beh->unk10 == beh->unk0c) {
                fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            } else {
                fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
            }
        }
    }
    return 1;
}

// Behaviour update for state 6: keeps the previous state in the secondary field,
// starts the state's fade value, clears condition bit 4, flags the actor's 0x288
// word and re-arms the state's effect with the block's 0x18 value.
int BehParamsGhostPauseState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* buf;
    int arg;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state2 = beh->state;
    beh->state = 6;
    beh->fade = lbl_804DAA8C;
    beh->unk10 = beh->unk0c;
    a->unk288 = 1;
    ConditionSetBit(a->cond, 4, 0);
    arg = beh->unk18;
    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        buf = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                  lbl_803C4138[beh->state].unk00, arg, 0);
        fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
    return 1;
}

// Behaviour update for state 5: records 5 as both the current and the previous
// state, keeps the previous effect id and re-arms the state's effect.
int BehParamsGhostState5(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* buf;
    int arg;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 5;
    beh->state2 = 5;
    beh->unk10 = beh->unk0c;
    arg = beh->unk18;
    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        buf = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                  lbl_803C4138[beh->state].unk00, arg, 0);
        fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
    return 1;
}

// Behaviour update for state 4: same re-arm as state 5, recording 4 in both
// the current and the previous state slot.
int BehParamsGhostState4(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* buf;
    int arg;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 4;
    beh->state2 = 4;
    beh->unk10 = beh->unk0c;
    arg = beh->unk18;
    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        buf = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                  lbl_803C4138[beh->state].unk00, arg, 0);
        fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
    return 1;
}

// Starts a ghost behaviour state's emitter cue: clears the pending cue flag,
// arms the held flag, resets the emitter and plays the state's effect callback
// with the state's animation id.  `loop` selects a looping animation set.
static inline void GhostCueStart(void* actorPtr, int loop)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* buf;

    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        buf = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                  lbl_803C4138[beh->state].unk00, loop, 0);
        fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
}

// Behaviour update for state 3: enters the state and starts its emitter cue,
// looping the animation while the object's progress value is below the
// threshold and playing it once otherwise.
int BehParamsGhostState3(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 3;
    beh->state2 = 0;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    if (a->turn.z > lbl_804DA9F0) {
        GhostCueStart(a, 0);
    } else {
        GhostCueStart(a, 1);
    }
    return 1;
}

// Behaviour update for state 2: remembers the previous state, resets the fade,
// clears the "fading" condition and starts the state's emitter cue once.
int BehParamsGhostState2(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state2 = beh->state;
    beh->state = 2;
    beh->fade = lbl_804DAA8C;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    ConditionSetBit(a->cond, 4, 0);
    GhostCueStart(a, 0);
    return 1;
}

// Behaviour start callback for state 1: marks the behaviour active, resets the
// emitter, installs the state's effect callback and syncs the sound condition.
int BehParamsGhostActivateState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* buf;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 1;
    beh->state2 = 1;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        buf = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                  lbl_803C4138[beh->state].unk00, 0, 0);
        fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
    return 1;
}

// Behaviour start callback for state 0: the inactive counterpart of the state 1
// callback above, with the same emitter and sound-condition handling.
int BehParamsGhostDeactivateState(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh;
    MsgBuf* buf;

    if (a == 0) {
        return 0;
    }
    beh = a->beh;
    beh->state = 0;
    beh->state2 = 0;
    beh->unk18 = 0;
    beh->unk10 = beh->unk0c;
    if (a != 0) {
        beh = a->beh;
        if (HasCondition(a->cond, 0xc)) {
            ConditionSetBit(a->cond, 0xc, 0);
            ConditionSetBit(a->cond, 0xe, 1);
            fn_8008F4E8(a->unk0d4, 0);
            fn_8008F378(a->unk0d4);
            fn_8008F4EC(a->unk0d4, 0);
        }
        buf = (MsgBuf*)a->msgCtx;
        beh->unk0c = fn_800A1BC8(buf->unk14, buf->unk1c,
                                  lbl_803C4138[beh->state].unk00, 0, 0);
        fn_80090974(lbl_804DAA14, a->unk0d4, 0, beh->unk0c, 9,
                    lbl_803C4138[beh->state].unk10, a);
        if (HasCondition(a->cond, 0xe)) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
            ConditionSetBit(a->cond, 0xe, 0);
        } else if (beh->unk10 == beh->unk0c) {
            fn_8008F230(lbl_804DA9F0, a->unk0d4, 0, 0);
        } else {
            fn_8008F230(lbl_804DAA1C, a->unk0d4, 0, 1);
        }
    }
    return 1;
}

// ---- shared ghost frame-update helpers --------------------------------------
extern "C" float lbl_804DAA90;
extern "C" float lbl_804DAA94;
extern "C" float lbl_804DAA98;
extern "C" float lbl_804DAA9C;
extern "C" float lbl_804DAAA0;
extern "C" float lbl_804DAAA4;
extern "C" float lbl_804DAAA8;
extern "C" float lbl_804DAAAC;
extern "C" float lbl_804DAAB0;
extern "C" float lbl_804DAAB4;
extern "C" float lbl_804DAAB8;
extern "C" float lbl_804DAABC;
extern "C" float lbl_804DAAC0;
extern "C" float lbl_804D5778;
extern "C" float lbl_804D38F0;
extern "C" float lbl_804D38F4;

extern "C" int fn_8008E964(void* handle);
extern "C" int ModelAnimate(int model, char* node, int loop);
extern "C" void AnimApply(int handle, char* node, int anim, int arg);
extern "C" float fn_800CC308(void* actor, int a, int b, int c, void* out);
extern "C" void BoneGetMatrix(Mtx34* mtx, void* node);
extern "C" void MtxGetEuler(Vec3* out, Mtx34* mtx);
extern "C" void Vec3Set(Vec3* v, float x, float y, float z);
extern "C" void ModelSetMatrix(int model, Mtx34* mtx, char* node);
extern "C" void ActorGetMatrix(Mtx34* mtx, void* actor);
extern "C" void fn_800A0168(Mtx34* out, Mtx34* in);
extern "C" void fn_800A0030(Vec3* out, Mtx34* m, Vec3* v);  // MtxTransformVec3 (u_8009DE94)
extern "C" void fn_800A025C(Mtx34* out, Mtx34* in, int flag);
extern "C" void fn_8016D648(Vec3* out, Vec3* a, Vec3* b);
extern "C" float fn_800E1020(Vec3* v);
extern "C" void fn_8016D4EC(Vec3* out, Vec3* in);
extern "C" void fn_800E1098(float f, Vec3* out, Vec3* in);
extern "C" int MemFlagIsSet(void* cond, int bit);
extern "C" float fn_800A1394(float x);
extern "C" float fn_800A1418(float x);

// Builds the actor's placement matrix for the current euler angles (the roll
// stored in angles.z) and pushes it to the model node.
static inline void GhostPlaceMatrix(BehActor* a, Mtx34* mtx, Vec3* tmp)
{
    float* trans;

    Vec3Set(tmp, a->angles.z, lbl_804DA9F0, lbl_804DA9F0);
    EulerNormalize(tmp, tmp);
    MtxSetEuler(mtx, tmp);
    trans = (float*)((unsigned char*)mtx + 0x24);
    trans[0] = (float)a->unk24;
    trans[1] = (float)a->unk28;
    trans[2] = (float)a->unk2c;
    PlacementToMatrix((Place*)a, mtx);
    ModelSetMatrix(a->model, mtx, (char*)a->unk0f0);
}

// Per-frame ghost update: advances the behaviour timers, integrates the
// steering angles and rebuilds the actor's model matrix.  Descriptively named;
// the layout of the behaviour block is a guess from the accesses.
extern "C" void GhostActorFrameUpdate(void* actorPtr)
{
    BehActor* a = (BehActor*)actorPtr;
    BehParams* beh = a->beh;
    Vec3 rot;                           // scratch vector
    Vec3 tmp;
    Vec3 axis;
    Mtx34 mtx;
    Mtx34 mtxRot;
    Mtx34 mtxPlace;
    float limit;
    float f;
    float spd;
    int anim;

    if (fn_8008E964(a->unk0d4) == 0 && HasCondition(a->cond, 0xc) != 0) {
        return;
    }
    limit = a->unk138 / a->unk91c;
    if (*(int*)a->msgCtx == (int)0xFE14BFB5 && HasCondition(a->cond, 9) != 0) {
        fn_800913EC(a->unk0d4, limit, &rot);
    } else {
        fn_800913EC(a->unk0d4, a->unk138, &rot);
    }
    anim = ModelAnimate(a->model, (char*)a->unk0f0, 1);
    AnimApply((int)a->unk0d4, (char*)a->unk0f0, anim, a->unk0d0);

    if (HasCondition(a->cond, 0xd) != 0 || HasCondition(a->cond, 0xc) != 0) {
        f = fn_800CC308(a, beh->unk64, 1, 0, &beh->unk14);
        if (HasCondition(a->cond, 0xc) != 0) {
            if (fn_8008EA9C(a->unk0d4, 0) != 0) {
                if (HasCondition(a->cond, 0xc) != 0) {
                    BoneGetMatrix(&mtxPlace, a->unk0f0);
                    MtxGetEuler(&a->angles, &mtxPlace);
                    PlacementToMatrix((Place*)a, &mtxPlace);
                    a->unk84 = *(float*)((unsigned char*)a->unk0f0 + 0x10);
                    a->unk88 = *(float*)((unsigned char*)a->unk0f0 + 0x14);
                    a->unk8c = *(float*)((unsigned char*)a->unk0f0 + 0x18);
                    a->unk24 = (int)*(float*)((unsigned char*)a->unk0f0 + 0x10);
                    a->unk28 = (int)*(float*)((unsigned char*)a->unk0f0 + 0x14);
                    a->unk2c = (int)*(float*)((unsigned char*)a->unk0f0 + 0x18);
                } else {
                    GhostPlaceMatrix(a, &mtxPlace, &tmp);
                }
            }
            ModelSetMatrix(a->model, 0, (char*)a->unk0f0);
        } else {
            if (f != lbl_804DAA98) {
                a->unk8c = a->unk8c + *(float*)((unsigned char*)a->unk0f0 + 0x18);
                *(float*)((unsigned char*)a->unk0f0 + 0x18) = lbl_804DA9F0;
            }
            a->unk2c = (int)a->unk8c;
            *(float*)((unsigned char*)a->unk0f0 + 0x10) = lbl_804DA9F0;
            *(float*)((unsigned char*)a->unk0f0 + 0x14) = lbl_804DA9F0;
            *(float*)((unsigned char*)a->unk0f0 + 0x18) = lbl_804DA9F0;
            ActorGetMatrix(&mtx, a);
            fn_800A0168(&mtx, &mtx);
            fn_800A0030(&rot, &mtx, &rot);
            a->unk240 = rot.x;
            a->unk244 = rot.y;
            a->unk248 = rot.z;
            a->unk248 = lbl_804DA9F0;
            f = lbl_804DAA94 / a->unk138;
            a->unk240 = a->unk240 * f;
            a->unk244 = a->unk244 * f;
            a->unk248 = a->unk248 * f;
            GhostPlaceMatrix(a, &mtxPlace, &tmp);
        }
        return;
    }

    limit = *(float*)((unsigned char*)a->msgCtx + 8);
    if (a->angles.x < -limit) {
        a->angles.x = -limit;
    } else if (a->angles.x > limit) {
        a->angles.x = limit;
    }
    f = lbl_804DAA90;
    beh->unk54 = beh->unk54 + a->unk138;
    f = fn_800A1394(f * beh->unk54);
    a->unk8c = a->unk8c
             + (a->unk138 * (lbl_804DAA44 * f)) / lbl_804DAA94;
    f = fn_800CC308(a, beh->unk64, 1, 0, &beh->unk14);
    if (f != lbl_804DAA98) {
        float d = a->unk8c - f;

        if (d < lbl_804DAA9C) {
            a->unk8c = lbl_804DAA9C + f;
        } else if (d > lbl_804DAAA0) {
            a->unk8c = lbl_804DAAA0 + f;
        }
    }
    a->unk2c = (int)a->unk8c;
    *(float*)((unsigned char*)a->unk0f0 + 0x10) = lbl_804DA9F0;
    *(float*)((unsigned char*)a->unk0f0 + 0x14) = lbl_804DA9F0;
    *(float*)((unsigned char*)a->unk0f0 + 0x18) = lbl_804DA9F0;
    if (HasCondition(a->cond, 0) != 0) {
        ActorGetMatrix(&mtx, a);
        fn_800A0168(&mtx, &mtx);
        fn_800A0030(&rot, &mtx, &rot);
        a->unk240 = rot.x;
        a->unk244 = rot.y;
        a->unk248 = rot.z;
        a->unk248 = lbl_804DA9F0;
        f = lbl_804DAA94 / a->unk138;
        a->unk240 = a->unk240 * f;
        a->unk244 = a->unk244 * f;
        a->unk248 = a->unk248 * f;
    } else {
        float rate = a->turn.z;
        float yawRate = a->turn.x;
        float accel;
        float v;

        lbl_804D5778 = lbl_804D5778 + a->unk138;
        beh->unk5c = lbl_804DA9F0;
        accel = lbl_804DAAA8 * (*(float*)((unsigned char*)a->msgCtx + 0xc0) * a->unk138);
        if (rate > lbl_804DAAA4) {
            v = accel;
            if (v > rate) {
                v = rate;
            }
            a->angles.z = a->angles.z + v;
            v = lbl_804DAAB0 * rate;
            spd = (v < lbl_804DAAAC) ? v : lbl_804DAAAC;
        } else if (rate < lbl_804DAAB4) {
            v = -accel;
            if (v < rate) {
                v = rate;
            }
            a->angles.z = a->angles.z + v;
            v = lbl_804DAAB0 * rate;
            spd = (v > lbl_804DAAB8) ? v : lbl_804DAAB8;
        } else {
            spd = lbl_804DA9F0;
        }
        if (yawRate <= lbl_804DAAA4) {
            if (yawRate < lbl_804DAAB4) {
                v = -accel;
                if (yawRate < v) {
                    yawRate = v;
                }
                a->angles.x = a->angles.x + yawRate;
            }
        } else {
            v = accel;
            if (v < yawRate) {
                yawRate = v;
            }
            a->angles.x = a->angles.x + yawRate;
        }
        if (a->unk26c != 0) {
            beh->unk5c = lbl_804DAABC
                       * ((a->unk138
                           * (lbl_804D38F4
                              * fn_800A1394(lbl_804D5778 * lbl_804D38F0)))
                          / lbl_804DAA94);
            beh->unk60 = lbl_804DAABC
                       * ((a->unk138
                           * (lbl_804D38F4
                              * fn_800A1418(lbl_804D5778 * lbl_804D38F0)))
                          / lbl_804DAA94);
            v = lbl_804DAAA8 * (lbl_804DAAC0 * a->unk138);
            if (beh->unk58 < spd) {
                float diff = spd - beh->unk58;

                beh->unk58 = beh->unk58 + ((v < diff) ? v : diff);
            } else if (beh->unk58 > spd) {
                float diff = spd - beh->unk58;

                v = -v;
                beh->unk58 = beh->unk58 + ((v > diff) ? v : diff);
            }
        }
    }
    MtxSetEuler(&mtxRot, &a->angles);
    fn_800A025C(&mtx, &mtxRot, 1);
    fn_8016D648(&axis, &a->angles, &a->turn);
    MtxSetEuler(&mtxRot, &axis);
    fn_800A025C(&mtx, &mtxRot, 1);
    EulerNormalize(&a->angles, &a->angles);
    if (beh->unk18 == 0 && HasCondition(a->cond, 0) == 0 &&
        MemFlagIsSet(a->cond, 4) == 0) {
        f = fn_800E1020((Vec3*)&a->unk240);
        fn_8016D4EC((Vec3*)&a->unk240, &a->angles);
        fn_800E1098(f, (Vec3*)&a->unk240, (Vec3*)&a->unk240);
    }
    GhostPlaceMatrix(a, &mtxPlace, &tmp);
}

// ---- ghost model/effect stream update ---------------------------------------
// Effect descriptor copied from a shared template before spawning.
struct GhostFxDesc {
    unsigned char unk00[0x30];
    unsigned char colorA[4];            // 0x30
    unsigned char colorB[4];            // 0x34
    unsigned char unk38[4];             // 0x38
};

extern "C" GhostFxDesc lbl_8034BA28;
extern "C" float lbl_804DAAC4;
extern "C" float lbl_804DAAC8;
extern "C" float lbl_804DAACC;

extern "C" int fn_80158AFC(int id, void* stream);
extern "C" int fn_80116E08(void* actor);
extern "C" int fn_800AFD20(void* actor);
extern "C" int fn_80162940(void* ctx, int flag);
extern "C" void ModelNodeSetup(int model, int node, int a, int b);
extern "C" int ModelFindBone(int model, int bone);
extern "C" void Mtx34SetTranslation(Mtx34* mtx, Vec3* v);
extern "C" void fn_8009FC6C(void* node, void* node2, Mtx34* mtx);
extern "C" void fn_8003F15C(float f, int handle, int node, int arg,
                            int* stream, void* desc);
extern "C" int fn_8003A1F0(int handle, void* node, int anim);
extern "C" int HandleToPlayer(int handle);
extern "C" int PlayerTeam(int player);
extern "C" void* fn_800FD3FC(int team);

// Per-frame ghost model/effect update driven by the serialisation message
// stream; returns 1 when the message was handled.
extern "C" int GhostActorStreamMsg(void* streamPtr, int mode)
{
    int* stream = (int*)streamPtr;
    BehActor* a = (BehActor*)stream[0];
    BehParams* beh;
    Vec3 v;
    Vec3 rnd;
    Mtx34 mtx;
    Mtx34 mtxT;
    Place place;
    GhostFxDesc desc;
    float f;
    float f0;
    int anim;
    int node;
    int bone;
    int hnd;
    float* col;

    if (a == 0) {
        return 1;
    }
    beh = a->beh;
    if (mode == 0) {
        if (fn_80158AFC(OwnerId((Owner*)a), stream) != 0) {
            return 1;
        }
    }
    fn_80116E08(a);
    hnd = fn_800AFD20(a);
    Vec3Set(&v, a->angles.z + beh->unk5c, a->angles.x + beh->unk60,
            beh->unk58);
    EulerNormalize(&v, &v);
    MtxSetEuler(&mtx, &v);
    PlacementToMatrix(&place, &mtx);
    place.unk24 = a->unk24;
    place.unk28 = a->unk28;
    place.unk2c = a->unk2c;
    anim = ModelAnimate(a->model, 0, 0);
    node = fn_8003A1F0(hnd, a->unk0f0, anim);
    if ((a->flags & 1) == 0) {
        if (fn_80162940(a->unk894, 0x100000) != 0) {
            if (a->unk0e4 != -1) {
                ModelNodeSetup(a->model, a->unk0e4, 1, 1);
            }
            bone = ModelFindBone(a->model, a->unk0e0);
            rnd.x = lbl_804DAA88 - (float)(Rand() % 2048) / lbl_804DAAC4;
            rnd.y = lbl_804DAA88 - (float)(Rand() % 2048) / lbl_804DAAC4;
            rnd.z = lbl_804DAA88 - (float)(Rand() % 2048) / lbl_804DAAC4;
            Mtx34SetTranslation(&mtxT, &rnd);
            if (bone != -1) {
                char* dst = (char*)node + bone * 0x30;

                fn_8009FC6C(dst, dst, &mtxT);
            }
        } else {
            if (a->unk0e4 != -1) {
                ModelNodeSetup(a->model, a->unk0e4, 0, 1);
            }
        }
    }
    if (fn_80116E08(a) == 0) {
        f = lbl_804DAA14;
        if (HasCondition(a->cond, 0xa) != 0) {
            return 1;
        }
        if (HasCondition(a->cond, 9) != 0) {
            if (*(int*)a->msgCtx == (int)0xFE14BFB5) {
                f0 = beh->progress - lbl_804DAAC8;
                if (f0 < lbl_804DA9F0) {
                    f0 = lbl_804DA9F0;
                }
                f0 = f0 / lbl_804DAA94;
            } else {
                f0 = beh->progress / lbl_804DA9F4;
            }
            f0 = lbl_804DAA14 - f0;
            if (f0 < lbl_804DA9F0) {
                f = lbl_804DA9F0;
            } else if (f0 > lbl_804DAA14) {
                f = lbl_804DAA14;
            } else {
                f = f0;
            }
        }
        fn_8003F15C(f, hnd, node, a->unk0d0, (int*)((char*)streamPtr + 0x10),
                    0);
    } else {
        desc = lbl_8034BA28;
        col = (float*)fn_800FD3FC(PlayerTeam(HandleToPlayer(a->unk38)));
        desc.colorA[0] = (unsigned char)(int)(lbl_804DAACC * col[0]);
        desc.colorA[1] = (unsigned char)(int)(lbl_804DAACC * col[1]);
        desc.colorA[2] = (unsigned char)(int)(lbl_804DAACC * col[2]);
        desc.colorB[0] = (unsigned char)(int)(lbl_804DAACC * col[0]);
        desc.colorB[1] = (unsigned char)(int)(lbl_804DAACC * col[1]);
        desc.colorB[2] = (unsigned char)(int)(lbl_804DAACC * col[2]);
        fn_8003F15C(lbl_804DAA14, hnd, node, a->unk0d0,
                    (int*)((char*)streamPtr + 0x10), &desc);
    }
    a->unk13c = lbl_804DA9F0;
    return 1;
}

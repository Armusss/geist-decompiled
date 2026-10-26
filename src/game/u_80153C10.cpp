// u_80153C10.cpp: ghost possession, quick flash and ghost-record bookkeeping.

// Loose (unattached) ghosts: tick every active one that has an update callback.
struct GhostSlot {
    GhostSlot* unk00;            // 0x00
    GhostSlot* unk04;            // 0x04
    GhostSlot* unk08;            // 0x08
    int unk0C;                   // 0x0C
    char pad10[0x30 - 0x10];
    int (*handler)(int, void*);  // 0x30
    char pad34[0x38 - 0x34];
    unsigned long id;            // 0x38
    char pad3C[0x48 - 0x3C];
    int unk48;                   // 0x48
    float unk4C;                 // 0x4C
    float unk50;                 // 0x50
    float unk54;                 // 0x54
    int unk58;                   // 0x58
    int active;                  // 0x5C
    int unk60;                   // 0x60
    int unk64;                   // 0x64
    void* unk68;                 // 0x68
    int unk6C;                   // 0x6C
    int unk70;                   // 0x70
    int unk74;                   // 0x74
    int unk78;                   // 0x78
    int unk7C;                   // 0x7C
    int unk80;                   // 0x80
    void* unk84;                 // 0x84
    int unk88;                   // 0x88
    void (*unk8C)(GhostSlot*);   // 0x8C
    void (*unk90)(GhostSlot*);   // 0x90
    void (*unk94)(GhostSlot*);   // 0x94
    void (*unk98)(GhostSlot*);   // 0x98
    void (*update)(float);       // 0x9C
    char padA0[0x5EC - 0xA0];
    void* unk5EC;                // 0x5EC
    char pad5F0[0x924 - 0x5F0];
    int unk924;                  // 0x924
};

extern GhostSlot* lbl_803A5968[];
extern int lbl_804D4E20;
extern unsigned long lbl_803A59A8[8];   // four {ghost id, actor id} pairs
extern float lbl_804D7CA0;
extern float lbl_804D7CA4;
extern float lbl_804D7CA8;
extern double lbl_804D7CB0;

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int fn_802A1D04(void);
extern "C" float fn_80115F68(unsigned long id);
extern "C" GhostSlot* fn_80116DC4(GhostSlot* target);
extern "C" void fn_80116E30(GhostSlot* target);
extern "C" void fn_800F7AF0(int a, int b);
extern "C" int fn_801BD694(GhostSlot* slot);
int GhostFlashNotify(GhostSlot* self, int mode);
int GhostPossessEnd(GhostSlot* self);
int GhostPossessMenuNotify(unsigned int flags, int unused, GhostSlot* self);
extern "C" unsigned long fn_801554E0(GhostSlot* self, GhostSlot* target, int mode);
int ActorPlayAnim(int handle, unsigned long anim, int arg);
int fn_8007EFA4(int a, int b, const char* name, int c, int d, int e, int f);
extern "C" void fn_8015688C(void);
int HudMsgDispatch(void* owner, int msg, void* data, int arg);
int HandleToPlayer(int id);
float StatGet(int stat, int kind, int index);
void StatSet(int stat, int kind, int index, float value);

struct Actor;

// What PlayerGetPossessor() answers: the player record, with the ghost's
// possession meter at 0x1500.
struct PlayerData {
    char pad0000[0x1500];
    float unk1500;               // 0x1500
};
PlayerData* PlayerGetPossessor(int id);
void HudSlotFlagClear(int slot);

// Fires one of the two paired effects that link the ghost `self` to `target`.
void Fn800F7F5C(int effectTarget, int effectSource, int event, int id, void* flag,
                float value);
extern "C" void fn_800A7B08(void* owner, int kind, int flag);
void ActorRender(Actor* actor);

unsigned long GhostPossessCommit(GhostSlot* self, GhostSlot* target, int mode);
unsigned long GhostPossess(GhostSlot* self, GhostSlot* target, int mode);

struct Actor;
struct IVec3 { int x; int y; int z; };

int ActorAim(int* out, Actor* actor, int flag);
void VecToAngles(IVec3* out, IVec3* in);
void VecSwapXY2(IVec3* out, IVec3* in);

extern "C" void fn_800A3990(unsigned long value);
extern "C" int fn_800A384C(void);
extern "C" int fn_8020D4BC(GhostSlot* slot);
extern "C" void fn_80049940(void);
extern "C" void fn_8001BAA8(int value);
extern "C" void* fn_800886C8(void);
extern "C" void fn_800886F0(void);
int GhostFrameUpdate(float dt, void* p);
int GhostPossessTick(GhostSlot* self);

// Runtime string buffer; the 0x41E query messages receive a String answer.
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
    ~String() {}
    virtual void Release(char* p);
};

struct QueryResult {
    String name;
};

// Payload of the 0x41E "query by name hash" message: the handler writes the
// answer over result.name's first word (the String vtable slot is reused).
struct QueryMsg {
    int unk0;
    unsigned long hash;
    int unk8;
    QueryResult result;
};

static inline void Query(int* out, int target, int msg, unsigned long hash) {
    QueryMsg m;
    *(int*)&m.result = 0;
    m.hash = hash;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    *out = *(int*)&m.result;
}

// Payload of message 0xC14: the possessor's id and the request name hash.
struct MsgC14 {
    int unk0;
    void* arg;
    unsigned long value;
    unsigned long hash;
};

// Payload of message 0x40D (draw state): the actor and two mode words.
struct Msg40D {
    void* unk0;
    int mode;
    int count;
    int unk0C;
};

void GhostsTick(float dt) {
    GhostSlot* slot;
    int i;
    for (i = 0; i < lbl_804D4E20; i++) {
        slot = lbl_803A5968[i];
        if (slot->active != 0 && slot->update != 0) {
            slot->update(dt);
        }
    }
}

// 0x80153C94: try to begin a possession of `target` by the ghost `self`.
unsigned long GhostPossessBegin(GhostSlot* self, GhostSlot* target, int mode) {
    int answer;
    int ok;
    GhostSlot* slot;
    int i;
    for (i = 0; i < lbl_804D4E20; i++) {
        slot = lbl_803A5968[i];
        if (slot->active != 0 && (slot->unk04 == target || slot->unk08 == target)) {
            return 0;
        }
    }
    if (fn_802A1D04() == 8) {
        QueryMsg m;
        *(int*)&m.result = (int)target;
        m.hash = 0xA92756;
        m.unk8 = 0;
        SendMsg(0x4B000000, 0x41E, &m);
        if (*(int*)&m.result == 0) {
            return 0;
        }
    }
    if (target == 0) {
        ok = 0;
    } else {
        Query(&answer, self->unk00->id, 0x41E, 0xFA969430);
        for (int i = 0; i < lbl_804D4E20; i++) {
            GhostSlot* slot = lbl_803A5968[i];
            if (slot->active != 0 && (slot->unk04 == target || slot->unk08 == target)) {
                ok = 0;
                goto check_done;
            }
        }
        if (fn_80115F68(target->id) <= lbl_804D7CA0 || fn_802A1D04() != 0 || answer != 0) {
            ok = 1;
        } else {
            ok = 0;
        }
    }
check_done:
    if (ok) {
        self->unk58 = 1;
        if (self->unk60 == 0) {
            self->unk60 = 1;
            if ((int)(self->unk00->id >> 24) == 2) {
                if (self->unk04 != 0) {
                    ActorPlayAnim(self->unk04->id, 0xBCC6ABCD, self->unk00->id);
                }
                Msg40D msg;
                msg.unk0 = self->unk00;
                msg.mode = 1;
                msg.count = 0x1000002;
                self->unk00->handler(0x40D, &msg);
            }
            GhostPossessEnd(self);
            self->unk88 = 1;
        }
        if ((int)target->id != -1) {
            MsgC14 msg;
            msg.value = self->unk00->id;
            msg.hash = 0x672F35C2;
            msg.arg = 0;
            SendMsg(target->id, 0xC14, &msg);
        }
        return fn_801554E0(self, target, mode);
    }
    return 0;
}

// 0x80153F88: notify a ghost that `target` should be (re)possessed.  The
// target is either the ghost itself (kind 2, possess its unk04 partner) or an
// actor of kind 0x13 found through fn_80116DC4.
unsigned long GhostPossessNotify(GhostSlot* self, GhostSlot* target, int mode) {
    GhostSlot* slot = 0;
    if (fn_802A1D04() == 3) {
        return 0;
    }
    for (int i = 0; i < lbl_804D4E20; i++) {
        GhostSlot* s = lbl_803A5968[i];
        if (s->unk00 == target) {
            slot = s;
            break;
        }
    }
    if (slot != 0 && (slot->unk60 != 0 || slot->active != 0 || slot->unk08 != 0)) {
        return 0;
    }
    self->unk7C = 1;
    if (self->unk04 != 0 && self->unk60 == 0) {
        self->unk60 = 1;
        if ((self->unk00->id >> 24) == 2) {
            if (self->unk04 != 0) {
                ActorPlayAnim(self->unk04->id, 0xBCC6ABCD, self->unk00->id);
            }
            Msg40D msg;
            msg.unk0 = self->unk00;
            msg.mode = 1;
            msg.count = 0x1000002;
            self->unk00->handler(0x40D, &msg);
        }
        GhostPossessEnd(self);
        self->unk88 = 1;
    }
    unsigned long kind = target->id >> 24;
    if (kind != 2) {
        if (kind != 0x13) {
            return 1;
        }
        GhostSlot* other = fn_80116DC4(target);
        if (other != 0 && (other->id >> 24) == 0x13) {
            other->unk924 = 1;
        }
        for (int i = 0; i < 4; i++) {
            if (lbl_803A59A8[i * 2] == 0) {
                lbl_803A59A8[i * 2] = other->id;
                lbl_803A59A8[i * 2 + 1] = target->id;
                break;
            }
        }
        fn_80116E30(target);
        for (int i = 0; i < lbl_804D4E20; i++) {
            GhostSlot* s = lbl_803A5968[i];
            if (s->active != 0 && s->unk04 == other) {
                goto resolve;
            }
        }
        if (fn_802A1D04() == 8) {
            QueryMsg m;
            *(int*)&m.result = (int)other;
            m.hash = 0xA92756;
            m.unk8 = 0;
            SendMsg(0x4B000000, 0x41E, &m);
            if (*(int*)&m.result == 0) {
                goto resolve;
            }
        }
        fn_801554E0(self, other, mode);
    resolve:
        {
            int answer;
            Query(&answer, 0x4B000000, 0x41E, 0x896F8C7F);
            if (fn_802A1D04() == 0) {
                return 1;
            }
            if (answer != 1) {
                return 1;
            }
            MsgC14 msg;
            msg.value = self->unk00->id;
            if ((int)other->id != -1) {
                msg.hash = 0x672F35C2;
                msg.arg = 0;
                SendMsg(other->id, 0xC14, &msg);
            }
        }
        return 1;
    }
    GhostSlot* other = slot->unk04;
    self->unk58 = 1;
    for (int i = 0; i < 4; i++) {
        if (lbl_803A59A8[i * 2] == 0) {
            lbl_803A59A8[i * 2 + 1] = other->id;
            lbl_803A59A8[i * 2] = self->unk00->id;
            break;
        }
    }
    if (slot->unk60 == 0) {
        slot->unk60 = 1;
        if ((slot->unk00->id >> 24) == 2) {
            if (slot->unk04 != 0) {
                ActorPlayAnim(slot->unk04->id, 0xBCC6ABCD, slot->unk00->id);
            }
            Msg40D msg;
            msg.unk0 = slot->unk00;
            msg.mode = 1;
            msg.count = 0x1000002;
            slot->unk00->handler(0x40D, &msg);
        }
        if (mode == 0) {
            GhostPossessEnd(slot);
            slot->unk88 = 1;
        } else {
            slot->unk6C = fn_8007EFA4(slot->unk64, 1, "Red Flash", 0, 0, 0, 0);
            slot->unk70 = fn_8007EFA4(slot->unk64, 1, "Repossess", 0, 0, (int)GhostPossessMenuNotify, (int)slot);
            if (slot->unk70 == -1) {
                GhostPossessEnd(slot);
            }
            slot->unk74 = fn_8007EFA4(slot->unk64, 1, "Focus", 0, 0, 0, 0);
        }
    }
    for (int i = 0; i < lbl_804D4E20; i++) {
        GhostSlot* s = lbl_803A5968[i];
        if (s->active != 0 && s->unk04 == other) {
            goto done;
        }
    }
    if (fn_802A1D04() == 8) {
        QueryMsg m;
        *(int*)&m.result = (int)other;
        m.hash = 0xA92756;
        m.unk8 = 0;
        SendMsg(0x4B000000, 0x41E, &m);
        if (*(int*)&m.result == 0) {
            goto done;
        }
    }
    fn_801554E0(self, other, mode);
done:
    if (other != 0 && (other->id >> 24) == 0x13) {
        other->unk924 = 1;
    }
    return 1;
}

// 0x80154594: one-shot setup for a ghost's possession menu ("Red Flash",
// "Repossess", "Focus").  Returns 1 the first time it runs, 0 afterwards.
int GhostNotify(void* p, int mode, int unused) {
    GhostSlot* self = (GhostSlot*)p;
    if (self->unk60 == 0) {
        self->unk60 = 1;
        if ((int)(self->unk00->id >> 24) == 2) {
            if (self->unk04 != 0) {
                ActorPlayAnim(self->unk04->id, 0xBCC6ABCD, self->unk00->id);
            }
            Msg40D msg;
            msg.unk0 = self->unk00;
            msg.mode = 1;
            msg.count = 0x1000002;
            self->unk00->handler(0x40D, &msg);
        }
        if (mode != 0) {
            self->unk6C = fn_8007EFA4(self->unk64, 1, "Red Flash", 0, 0, 0, 0);
            self->unk70 = fn_8007EFA4(self->unk64, 1, "Repossess", 0, 0,
                                      (int)GhostPossessMenuNotify, (int)self);
            if (self->unk70 == -1) {
                GhostPossessEnd(self);
            }
            self->unk74 = fn_8007EFA4(self->unk64, 1, "Focus", 0, 0, 0, 0);
        } else {
            GhostPossessEnd(self);
            self->unk88 = 1;
        }
        return 1;
    }
    return 0;
}

// 0x80154700: menu callback.  Bit 0 of the flags means "chosen", which tears
// the menu down; otherwise the menu state is cleared.
int GhostPossessMenuNotify(unsigned int flags, int unused, GhostSlot* self) {
    if ((flags & 1) != 0) {
        GhostPossessEnd(self);
    } else {
        self->unk60 = 0;
        self->unk6C = -1;
        self->unk70 = -1;
        self->unk74 = -1;
    }
    return 1;
}

// Payload of message 0x407 (possession flash): the possessed actor, the
// possessing ghost and their ids, plus the flash strength and a name hash.
struct Msg407 {
    GhostSlot* actor;      // 0x00
    GhostSlot* ghost;      // 0x04
    unsigned long id;      // 0x08
    float unkC;            // 0x0C
    int unk10;             // 0x10
    unsigned long hash;    // 0x14
};

// 0x8015474C: release the ghost's possessed actor and clear the possession
// state.  Returns 0 when there is nothing attached.
int GhostPossessEnd(GhostSlot* self) {
    if (self->unk04 == 0) {
        return 0;
    }
    int name0;
    int name1;
    Query(&name0, self->unk04->id, 0x41E, 0x2DA8293C);
    Query(&name1, self->unk04->id, 0x41E, 0xDFF3097A);
    fn_800F7AF0(name0, name1);
    if ((int)self->unk04->id != -1) {
        MsgC14 msg;
        msg.arg = self;
        msg.value = self->unk00->id;
        msg.hash = 0x237655D3;
        SendMsg(self->unk04->id, 0xC14, &msg);
    }
    if (self->unk98 != 0) {
        self->unk98(self);
    }
    if ((int)self->unk04->id != -1) {
        MsgC14 msg;
        msg.arg = self;
        msg.value = self->unk00->id;
        msg.hash = 0x39CB0CB0;
        SendMsg(self->unk04->id, 0xC14, &msg);
    }

    Msg407 payload;
    payload.actor = self->unk04;
    payload.ghost = self->unk00;
    payload.id = self->unk00->id;

    int state = fn_802A1D04();
    if (state != 3) {
        if ((fn_802A1D04() == 2 || fn_802A1D04() == 1) &&
            (self->unk00->id >> 24) == 2) {
            QueryMsg m;
            *(int*)&m.result = (int)self->unk00->id;
            m.hash = 0x0E4043D9;
            m.unk8 = 0;
            HudMsgDispatch(0, 0x41E, &m, 0);
            float scale = lbl_804D7CA4 -
                lbl_804D7CA8 * (float)(*(int*)&m.result - 2);
            self->unk4C = self->unk4C * scale;
            self->unk50 = self->unk50 * scale;
        }
        payload.unkC = self->unk4C - self->unk50;
        payload.hash = 0x6CA5F172;
        payload.actor->handler(0x407, &payload);
    }
    payload.unkC = lbl_804D7CA0;
    payload.hash = 0x672F35C2;
    payload.actor->handler(0x407, &payload);

    if (fn_802A1D04() == 3) {
        if ((self->unk04->id >> 24) == 0x13) {
            QueryMsg m;
            *(int*)&m.result = -1;
            m.hash = 0x1C1A4CC9;
            m.unk8 = 0;
            SendMsg(self->unk04->id, 0x41D, &m);
        }
    } else {
        int found;
        Query(&found, 0x4B000000, 0x41E, 0x001512C7);
        if (found != 0) {
            int actorType = 3;
            int actorIdx = 0;
            switch (self->unk04->id >> 24) {
            case 2:
                actorType = 0;
                actorIdx = HandleToPlayer(self->unk04->id);
                break;
            case 0x13:
                if (fn_801BD694(self->unk04) != 0) {
                    actorType = 0;
                    actorIdx = HandleToPlayer(self->unk04->id);
                } else {
                    actorType = 3;
                    actorIdx = self->unk04->id & 0xFFFFFF;
                }
                break;
            }
            int ghostType = 3;
            int ghostIdx = 0;
            switch (self->unk00->id >> 24) {
            case 2:
                ghostType = 0;
                ghostIdx = HandleToPlayer(self->unk00->id);
                break;
            case 0x13:
                if (fn_801BD694(self->unk04) != 0) {
                    ghostType = 0;
                    ghostIdx = HandleToPlayer(self->unk00->id);
                } else {
                    ghostType = 3;
                    ghostIdx = self->unk00->id & 0xFFFFFF;
                }
                break;
            }
            float value = StatGet(0x7BFB940D, ghostType, ghostIdx);
            StatSet(0x7BFB940D, actorType, actorIdx, value);
            StatSet(0x7BFB940D, ghostType, ghostIdx, lbl_804D7CA0);
        }
    }

    if ((self->unk04->id >> 24) == 1 &&
        *(int*)((char*)self->unk04->unk84 + 4) == 0x2DF51005) {
        GhostFlashNotify(self, 5);
    } else if (lbl_804D7CA0 < self->unk50) {
        GhostFlashNotify(self, 2);
    } else {
        GhostFlashNotify(self, 4);
    }

    if ((int)self->unk04->id != -1) {
        MsgC14 msg;
        msg.arg = self;
        msg.value = self->unk00->id;
        msg.hash = 0xD5746CF2;
        SendMsg(self->unk04->id, 0xC14, &msg);
    }
    ActorPlayAnim(self->unk04->id, 0xC6144531, self->unk00->id);
    ActorPlayAnim(self->unk00->id, 0x2085A009, self->unk04->id);
    self->unk04 = 0;
    self->unk08 = 0;
    self->unk50 = lbl_804D7CA0;
    self->unk48 = -1;
    self->unk4C = lbl_804D7CA0;
    self->unk54 = lbl_804D7CA0;
    self->unk0C = 0;
    self->unk7C = 0;
    ActorPlayAnim(0x4E000000, 0xE0BD431C, self->unk00->id);
    ActorPlayAnim(0x38000000, 0xE0BD431C, self->unk00->id);
    return 1;
}

// Payload of message 0x401 (read aim/scale): the actor and two floats.
struct Msg401 {
    void* owner;   // 0x00
    int unk4;      // 0x04
    int unk8;      // 0x08
    int unkC;      // 0x0C
    float f10;     // 0x10
    float f14;     // 0x14
};

// Payload of message 0x402: a single 16-bit mode.
struct Msg402 {
    int unk0;              // 0x00
    unsigned short unk4;   // 0x04
};

// Payload of message 0x410: the actor itself.
struct Msg410 {
    void* owner;   // 0x00
};

// Payload of message 0x419: the actor and its returned value.
struct Msg419 {
    void* owner;   // 0x00
    int value;     // 0x04
};

// Payload of message 0x409: the actor plus a name hash.
struct Msg409 {
    GhostSlot* owner;      // 0x00
    int unk4;              // 0x04
    int unk8;              // 0x08
    unsigned long hash;    // 0x0C
};

// Output buffer filled by ActorAim; the vector used by the 0xC07 message
// lives at offset 0x24.
struct AimBuffer {
    int unk00[9];   // 0x00
    IVec3 vec;      // 0x24
};

// Payload of message 0xC07 (flash position).
struct MsgC07 {
    int unk0;   // 0x00
    int unk4;   // 0x04
    int unk8;   // 0x08
    int unkC;   // 0x0C
    int x;      // 0x10
    int y;      // 0x14
    int z;      // 0x18
};

// Payload of message 0xC00: the actor and a single flag.
struct MsgC00 {
    void* unk0;  // 0x00
    int value;   // 0x04
};

// 0x80154D90: per-frame possession update.  Recomputes the ghost's aim and
// scale, sends the HUD updates, then either opens the possession menu or
// cancels the possession.  Returns 1 when the possession was released.
int GhostPossessTick(GhostSlot* self) {
    GhostSlot* actor = self->unk04;
    if (actor == 0) {
        return 0;
    }
    self->unk08 = 0;
    Msg401 aimMsg;
    aimMsg.owner = actor;
    actor->handler(0x401, &aimMsg);
    self->unk54 = aimMsg.f14;
    self->unk4C = aimMsg.f10;
    self->unk50 = aimMsg.f10;

    if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
        if ((self->unk00->id >> 24) == 2) {
            QueryMsg m;
            *(int*)&m.result = (int)self->unk00->id;
            m.hash = 0x0E4043D9;
            m.unk8 = 0;
            HudMsgDispatch(0, 0x41E, &m, 0);
            float scale = lbl_804D7CA4 +
                lbl_804D7CA8 * (float)(*(int*)&m.result - 2);
            if (self->unk50 == self->unk54) {
                self->unk50 = self->unk50 * scale;
                self->unk4C = self->unk4C * scale;
            }
            self->unk54 = self->unk54 * scale;
        }
    }

    Msg402 m402;
    m402.unk4 = 0xFFFF;
    SendMsg(actor->id, 0x402, &m402);

    Msg410 m410;
    m410.owner = actor;
    actor->handler(0x410, &m410);

    GhostSlot* target = self->unk04;
    int result;
    unsigned long kind = target->id >> 24;
    if (kind == 0x13) {
        if (fn_801BD694(target) != 0) {
            fn_800A3990(*(unsigned long*)((char*)target->unk5EC + 0xBC));
        } else {
            fn_800A3990(target->id);
        }
        result = fn_800A384C();
    } else if (kind == 2) {
        Msg419 m419;
        m419.owner = target;
        target->handler(0x419, &m419);
        result = m419.value;
    } else if (kind == 1) {
        result = fn_8020D4BC(target);
    }
    self->unk0C = result;

    if (self->unk7C == 1) {
        if ((int)self->unk04->id != -1) {
            MsgC14 msg;
            msg.arg = self;
            msg.value = self->unk00->id;
            msg.hash = 0x672F35C2;
            SendMsg(self->unk04->id, 0xC14, &msg);
        }
    }
    if (self->unk94 != 0) {
        self->unk94(self);
    }

    AimBuffer aim;
    IVec3 pos;
    IVec3 angles;
    IVec3 swapped;
    ActorAim((int*)&pos, (Actor*)&aim, 1);
    pos.z = 0;
    VecToAngles(&angles, &pos);
    VecSwapXY2(&swapped, &angles);

    MsgC07 flash;
    flash.unk4 = 3;
    flash.x = aim.vec.x;
    flash.y = aim.vec.y;
    flash.z = aim.vec.z;
    SendMsg(self->unk00->id, 0xC07, &flash);

    if ((self->unk00->id >> 24) == 2 && (self->unk04->id >> 24) != 1) {
        fn_80049940();
    }

    Msg409 m409;
    m409.owner = actor;
    m409.hash = 0x1379C466;
    actor->handler(0x409, &m409);

    int found;
    Query(&found, 0x4B000000, 0x41E, 0x001512C7);
    if (found != 0) {
        int actorType = 3;
        int actorIdx = 0;
        unsigned long actorKind = self->unk04->id >> 24;
        if (actorKind == 2) {
            actorType = 0;
            actorIdx = HandleToPlayer(self->unk04->id);
        } else if (actorKind == 0x13) {
            if (fn_801BD694(self->unk04) != 0) {
                actorType = 0;
                actorIdx = HandleToPlayer(self->unk04->id);
            } else {
                actorType = 3;
                actorIdx = self->unk04->id & 0xFFFFFF;
            }
        }
        int ghostType = 3;
        int ghostIdx = 0;
        unsigned long ghostKind = self->unk00->id >> 24;
        if (ghostKind == 2) {
            ghostType = 0;
            ghostIdx = HandleToPlayer(self->unk00->id);
        } else if (ghostKind == 0x13) {
            if (fn_801BD694(self->unk04) != 0) {
                ghostType = 0;
                ghostIdx = HandleToPlayer(self->unk00->id);
            } else {
                ghostType = 3;
                ghostIdx = self->unk00->id & 0xFFFFFF;
            }
        }
        float value = StatGet(0x7BFB940D, actorType, actorIdx);
        StatSet(0x7BFB940D, ghostType, ghostIdx, value);
        StatSet(0x7BFB940D, actorType, actorIdx, lbl_804D7CA0);
    }

    if (lbl_804D7CA0 < self->unk50) {
        if ((int)self->unk04->id != -1) {
            MsgC14 msg;
            msg.arg = self;
            msg.value = self->unk00->id;
            msg.hash = 0x7BF91F20;
            SendMsg(self->unk04->id, 0xC14, &msg);
        }
        ActorPlayAnim(self->unk04->id, 0x24AFB4E1, self->unk00->id);
        ActorPlayAnim(0x4E000000, 0x3C9C44F6, self->unk00->id);
        ActorPlayAnim(0x38000000, 0x3C9C44F6, self->unk00->id);
        if (self->unk80 != -1) {
            MsgC00 m;
            m.value = 1;
            SendMsg(self->unk80, 0xC00, &m);
            QueryMsg q;
            *(int*)&q.result = (int)self->unk00->id;
            q.hash = 0x5CE116F5;
            q.unk8 = 0;
            SendMsg(self->unk80, 0x41D, &q);
            self->unk80 = -1;
        }
    } else {
        self->unk58 = 0;
        if (fn_800886C8() == (void*)GhostFrameUpdate) {
            fn_800886F0();
        }
        if (self->unk60 == 0) {
            self->unk60 = 1;
            if ((int)(self->unk00->id >> 24) == 2) {
                if (self->unk04 != 0) {
                    ActorPlayAnim(self->unk04->id, 0xBCC6ABCD, self->unk00->id);
                }
                Msg40D msg;
                msg.unk0 = self->unk00;
                msg.mode = 1;
                msg.count = 0x1000002;
                self->unk00->handler(0x40D, &msg);
            }
            self->unk6C = fn_8007EFA4(self->unk64, 1, "Red Flash", 0, 0, 0, 0);
            self->unk70 = fn_8007EFA4(self->unk64, 1, "Repossess", 0, 0,
                                      (int)GhostPossessMenuNotify, (int)self);
            if (self->unk70 == -1) {
                GhostPossessEnd(self);
            }
            self->unk74 = fn_8007EFA4(self->unk64, 1, "Focus", 0, 0, 0, 0);
        }
        return 0;
    }
    self->unk58 = 0;
    self->unk7C = 0;
    fn_8001BAA8(1);
    if (fn_800886C8() == (void*)GhostFrameUpdate) {
        fn_800886F0();
    }
    return 1;
}

// 0x801554E0: commit a possession.  Attaches `target` to the ghost, sends the
// target's per-kind enter message, then either opens the possession menu or
// finishes the possession immediately.  Returns 0 when it cannot start.
unsigned long fn_801554E0(GhostSlot* self, GhostSlot* target, int mode) {
    if (self->unk04 != 0) {
        return 0;
    }
    if (target == 0) {
        return 0;
    }
    int answer;
    Query(&answer, self->unk00->id, 0x41E, 0xFA969430);
    if (fn_80115F68(target->id) > lbl_804D7CA0 && fn_802A1D04() == 0 && answer == 0) {
        return 0;
    }
    if ((self->unk00->id >> 24) == 2) {
        if ((target->id >> 24) == 2 &&
            PlayerGetPossessor(target->id)->unk1500 < lbl_804D7CA4) {
            return 0;
        }
        HudSlotFlagClear(self->unk64);
        int nameA;
        int nameB;
        Query(&nameA, target->id, 0x41E, 0xDFF3097A);
        Query(&nameB, target->id, 0x41E, 0x2DA8293C);
        Fn800F7F5C(nameB, nameA, 0x7FAB1247, self->unk00->id, 0, lbl_804D7CA0);
        Fn800F7F5C(nameB, nameA, 0xE452286F, self->unk00->id, (void*)1, lbl_804D7CA0);
    }

    self->unk04 = target;
    self->active = 1;
    self->unk78 = mode;

    switch (target->id >> 24) {
    case 2: {
        Msg40D msg;
        msg.unk0 = target;
        msg.mode = 1;
        msg.count = 2;
        target->handler(0x40D, &msg);
        break;
    }
    case 1: {
        MsgC00 msg;
        msg.unk0 = target;
        msg.value = 0;
        target->handler(0xC00, &msg);
        break;
    }
    case 0x13:
        if (fn_801BD694(self->unk04) != 0) {
            QueryMsg m;
            *(int*)&m.result = (int)self->unk00->id;
            m.hash = 0x1C1A4CC9;
            m.unk8 = 0;
            SendMsg(self->unk04->id, 0x41D, &m);
        } else {
            MsgC00 msg;
            msg.unk0 = target;
            msg.value = 3;
            target->handler(0xC00, &msg);
            if (fn_802A1D04() != 0) {
                int sent;
                int queried;
                {
                    QueryMsg m;
                    *(int*)&m.result = 0;
                    m.hash = 0x7C18FE9E;
                    m.unk8 = 0;
                    sent = SendMsg(target->id, 0x41E, &m);
                    queried = *(int*)&m.result;
                }
                if (sent != 0) {
                    QueryMsg m2;
                    *(int*)&m2.result = (int)self->unk00->id;
                    m2.hash = 0x7C18FE9E;
                    m2.unk8 = 0;
                    SendMsg(queried, 0x41D, &m2);
                }
            }
        }
        fn_800A7B08((char*)self->unk04 + 0x270, 0xB, 1);
        break;
    default:
        return 0;
    }

    if (self->unk8C != 0) {
        self->unk8C(self);
    }
    if (mode != 0) {
        self->unk6C = fn_8007EFA4(self->unk64, 1, "Quick Flash", 0, 0, 0, 0);
        self->unk70 = fn_8007EFA4(self->unk64, 1, "Repossess", 0, 0,
                                  (int)fn_8015688C, (int)self);
        if (self->unk70 == -1) {
            if (self->unk90 != 0) {
                self->unk90(self);
            }
            GhostPossessTick(self);
        }
        self->unk74 = fn_8007EFA4(self->unk64, 1, "Focus", 0, 0, 0, 0);
        fn_8007EFA4(self->unk64, 1, "Red Flash 2", 0, 0, 0, 0);
    } else {
        if (self->unk90 != 0) {
            self->unk90(self);
        }
        GhostPossessTick(self);
        self->active = 0;
    }
    if (self->unk68 != 0) {
        ActorRender((Actor*)self->unk68);
        self->unk68 = 0;
    }
    return 1;
}

// 0x80155A84: public "possess this target" entry point.  Refuses when an
// active slot already possesses the target and, in the spirit world, when the
// target does not answer the possession query; otherwise commits the
// possession through fn_801554E0.
unsigned long fn_80155A84(GhostSlot* self, GhostSlot* target, int mode) {
    for (int i = 0; i < lbl_804D4E20; i++) {
        GhostSlot* s = lbl_803A5968[i];
        if (s->active != 0 && s->unk04 == target) {
            return 0;
        }
    }
    if (fn_802A1D04() == 8) {
        QueryMsg m;
        *(int*)&m.result = (int)target;
        m.hash = 0xA92756;
        m.unk8 = 0;
        SendMsg(0x4B000000, 0x41E, &m);
        if (*(int*)&m.result == 0) {
            return 0;
        }
    }
    return fn_801554E0(self, target, mode);
}

// The big ghost actor that a possession record belongs to; only the fields
// this unit reads or writes are named.
struct GhostActor {
    char pad00[0x30];
    int (*handler)(int, void*);      // 0x30
    char pad34[4];
    unsigned long id;                // 0x38
    char pad3C[0x50 - 0x3C];
    unsigned int flags;              // 0x50
    int unk54;                       // 0x54
    int unk58;                       // 0x58
    char pad5C[0x78 - 0x5C];
    int unk78;                       // 0x78
    int unk7C;                       // 0x7C
    char pad80[0x84 - 0x80];
    void* unk84;                     // 0x84
    char pad88[0xC4 - 0x88];
    void* unkC4;                     // 0xC4
    void* unkC8;                     // 0xC8
    char padCC[0xD4 - 0xCC];
    int unkD4;                       // 0xD4
    char padD8[0x14C - 0xD8];
    int unk14C;                      // 0x14C
    char pad150[0x1F0 - 0x150];
    float unk1F0;                    // 0x1F0
    char pad1F4[0x248 - 0x1F4];
    int unk248;                      // 0x248
    char pad24C[0x270 - 0x24C];
    char unk270[4];                  // 0x270
    char pad274[0x5EC - 0x274];
    void* unk5EC;                    // 0x5EC
    char pad5F0[0x650 - 0x5F0];
    float unk650;                    // 0x650
    float unk654;                    // 0x654
    float unk658;                    // 0x658
    char pad65C[0x12B4 - 0x65C];
    int unk12B4;                     // 0x12B4
    char pad12B8[0xC];
    int unk12C4;                     // 0x12C4
    char pad12C8[0x140C - 0x12C8];
    int unk140C;                     // 0x140C
    char pad1410[0x1BA0 - 0x1410];
    GhostActor* unk1BA0;             // 0x1BA0
    char pad1BA4[0x1C98 - 0x1BA4];
    int unk1C98;                     // 0x1C98
    char pad1C9C[0x1D68 - 0x1C9C];
    GhostActor* unk1D68;             // 0x1D68
    char pad1D6C[0x2668 - 0x1D6C];
    unsigned long unk2668;           // 0x2668
    char pad266C[0x26C4 - 0x266C];
    int** unk26C4;                   // 0x26C4
    int* unk26C8;                    // 0x26C8
    char pad26CC[0x26E0 - 0x26CC];
    GhostSlot* unk26E0;              // 0x26E0  (possessor of the embedded record)
    int unk26E4;                     // 0x26E4
    GhostActor* unk26E8;             // 0x26E8
    char pad26EC[0x273C - 0x26EC];
    int unk273C;                     // 0x273C
    char pad2740[0x279C - 0x2740];
    int unk279C;                     // 0x279C
};

extern "C" void fn_802A209C(int value);
extern "C" void fn_80088700(void* callback, int arg);
extern "C" void fn_800A1860(int id, int value);
extern "C" void fn_8003D7A0(void* data, int count);
extern "C" void fn_80007158(void);
extern "C" int fn_8007EB34(unsigned long a, unsigned long b);
extern "C" void fn_8016F390(int handle, int arg, int flag);
extern "C" void fn_80038AE8(int a, unsigned long b, int c, int d);
extern "C" void fn_80225CF8(void);
extern "C" void fn_80103ABC(int a);
extern "C" void fn_8014284C(int a);
extern "C" void fn_800FF4E4(int a);
extern "C" void fn_80090390(int a, int b);
void PlayerResetHeldSlots(void* owner);
extern "C" void fn_8008F230(double value, int a, int b, int c);
extern "C" void fn_800C64E4(void);
extern "C" int fn_800C6510(void);
extern "C" int fn_80176044(double a, double b, double c, int d, unsigned long e,
                           int f, void* callback, int g);
extern "C" void fn_80176000(int a, int b);
extern "C" int fn_8003AA9C(int a, int b, int c);
extern "C" int fn_80037BEC(int a);
extern "C" void fn_8008F630(int a, int b, int c, int d);
extern "C" void fn_8009ED70(void* dst, void* src);
extern "C" void fn_8003A66C(int a, void* b, int c);
extern "C" void fn_80101740(double value, int a, unsigned char b, unsigned char c, int d);
extern "C" void fn_80158B90(int a, int b, int c);

extern unsigned long lbl_804D7CB8;
extern float lbl_804D7CBC;
extern float lbl_804D7CC0;
extern float lbl_804D7CC4;

unsigned long GhostAimTargetFilter(int value, int owner);

// 0x80155BBC: per-frame possession attempt.  Looks at the ghost's current
// target, asks the world whether the ghost may take it, and on success
// initialises the possession state, the camera/aim hooks and the menu.
unsigned long GhostPossessAttempt(GhostSlot* self) {
    GhostActor* owner = (GhostActor*)self->unk00;
    GhostActor* target = owner->unk1D68;
    int ok;

    if ((target->id >> 24) == 0x13) {
        if (owner->unk26E4 != 0) {
            ok = 0;
        } else if (target == 0) {
            ok = 0;
        } else {
            int answer;
            Query(&answer, owner->unk26E0->id, 0x41E, 0xFA969430);
            for (int i = 0; i < lbl_804D4E20; i++) {
                GhostSlot* s = lbl_803A5968[i];
                if (s->active != 0 &&
                    (s->unk04 == (GhostSlot*)target || s->unk08 == (GhostSlot*)target)) {
                    ok = 0;
                    goto taken13;
                }
            }
            if (fn_80115F68(target->id) <= lbl_804D7CA0 || fn_802A1D04() != 0 || answer != 0) {
                ok = 1;
            } else {
                ok = 0;
            }
        }
    taken13:
        if (ok) {
            target = owner->unk1D68;
            if (*(int*)target->unkC8 == (int)0xAF7258AD ||
                *(int*)target->unkC8 == (int)0xFE14BFB5) {
                int id;
                Query(&id, target->id, 0x41E, 0x6C47D293);
                if (id == -1) {
                    return 1;
                }
                fn_80116E30((GhostSlot*)target);
                int buf[8];
                SendMsg(id, 0x400, buf);
                owner->unk1D68 = *(GhostActor**)buf;
                MsgC00 m;
                m.value = 3;
                SendMsg(target->id, 0xC00, &m);
                self->unk80 = target->id;
            }
        }
    }

    if (fn_802A1D04() == 0 && owner->unk1C98 != 1) {
        target = owner->unk1D68;
        unsigned long kind = target->id >> 24;
        if (kind == 0x13 || kind == 1) {
            if (owner->unk26E4 != 0) {
                ok = 0;
            } else if (target == 0) {
                ok = 0;
            } else {
                int answer;
                Query(&answer, owner->unk26E0->id, 0x41E, 0xFA969430);
                for (int i = 0; i < lbl_804D4E20; i++) {
                    GhostSlot* s = lbl_803A5968[i];
                    if (s->active != 0 &&
                        (s->unk04 == (GhostSlot*)target || s->unk08 == (GhostSlot*)target)) {
                        ok = 0;
                        goto taken1;
                    }
                }
                if (fn_80115F68(target->id) <= lbl_804D7CA0 || fn_802A1D04() != 0 ||
                    answer != 0) {
                    ok = 1;
                } else {
                    ok = 0;
                }
            }
        taken1:
            if (ok) {
                self->unk58 = 1;
                fn_802A209C(0);
                fn_8001BAA8(0);
                fn_80088700((void*)GhostFrameUpdate, (int)owner);
                int targetId = owner->unk1D68->id;
                if (targetId != -1) {
                    MsgC14 msg;
                    msg.arg = 0;
                    msg.value = owner->id;
                    msg.hash = 0x672F35C2;
                    SendMsg(targetId, 0xC14, &msg);
                }
                if (self->unk84 == 0) {
                    target = owner->unk1D68;
                    unsigned long k = target->id >> 24;
                    int* aim = 0;
                    if (k == 0x13) {
                        if (fn_801BD694((GhostSlot*)target) == 0) {
                            fn_800A3990(target->id);
                            aim = (int*)fn_800A384C();
                        } else {
                            fn_800A3990(*(unsigned long*)((char*)target->unk5EC + 0xBC));
                            aim = (int*)fn_800A384C();
                        }
                    } else if (k == 2) {
                        Msg419 m;
                        m.owner = target;
                        target->handler(0x419, &m);
                        aim = (int*)m.value;
                    } else if (k == 1) {
                        aim = (int*)fn_8020D4BC((GhostSlot*)target);
                    }
                    fn_800A1860(aim[5], aim[6]);
                    fn_800A1860(aim[5], aim[7]);
                    fn_800A1860(aim[5], aim[9]);
                    self->unk84 = (void*)1;
                }

                target = owner->unk1D68;
                unsigned long kind2 = target->id >> 24;
                if (kind2 == 0x13) {
                    int* src = (int*)fn_800A384C();
                    unsigned short table[12];
                    for (int i = 0; i < 11; i++) {
                        int v = *(int*)((char*)src + 0x3C + i * 12);
                        table[i] = (v < 0) ? 0xFFFF : (unsigned short)v;
                    }
                    fn_8003D7A0(table, 11);
                    fn_80007158();
                    fn_800A7B08((char*)target + 0x270, 5, 1);
                    fn_800A7B08((char*)target + 0x270, 0xB, 1);
                    int handle = fn_8007EB34(0x92234D44,
                                             *(unsigned long*)((char*)src + 0x14C));
                    if (handle != -1) {
                        fn_8016F390(handle, 0, 0);
                    }
                    unsigned long ev = fn_8007EB34(0xD9044CBB, 0x50EC64DA);
                    fn_80038AE8(owner->unk58, ev, 1, 1);
                } else if (kind2 == 1) {
                    int handle = *(int*)((char*)target->unk84 + 0xAC);
                    if (handle != -1) {
                        fn_8016F390(handle, 0, 0);
                    }
                }

                float zero = lbl_804D7CA0;
                owner->flags |= 0x1000002;
                owner->unk650 = zero;
                owner->unk654 = zero;
                owner->unk658 = zero;
                owner->unk26E8 = owner->unk1D68;
                fn_80225CF8();
                fn_80103ABC(owner->unk248);
                fn_8014284C(owner->unk248);
                fn_800FF4E4(owner->unk248);
                owner->unk1BA0 = owner->unk1D68;
                fn_80090390(owner->unk7C, 0);
                fn_80090390(owner->unkD4, 0);
                fn_80090390(owner->unkD4, 1);
                PlayerResetHeldSlots(owner);
                fn_8008F230((double)lbl_804D7CA0, owner->unk7C, 0, 1);
                fn_8008F230((double)lbl_804D7CA0, owner->unkD4, 0, 1);
                fn_8008F230((double)lbl_804D7CA0, owner->unkD4, 1, 1);
                owner->unk273C = 1;
                owner->unk2668 = lbl_804D7CB8;
                target = owner->unk1D68;
                unsigned long kind3 = target->id >> 24;
                if (kind3 == 1) {
                    fn_800C64E4();
                    int basis = fn_800C6510();
                    owner->unk279C = fn_80176044(
                        (double)lbl_804D7CBC, (double)lbl_804D7CC0,
                        (double)*(float*)((char*)target->unk84 + 0x50),
                        owner->unk248, owner->unk1D68->id, basis + 0x24,
                        (void*)GhostAimTargetFilter, (int)owner);
                } else {
                    int* sub = 0;
                    if (kind3 == 2) {
                        sub = target->unk26C8;
                    } else if (kind3 == 0x13) {
                        sub = (int*)target->unkC4;
                    }
                    if (sub != 0) {
                        int key = sub[0];
                        if (key == 0x4624485 || key == 0x13C05 || key == (int)0xE0AB9B4F ||
                            key == 0x1091C || key == (int)0xA7F71344 || key == 0x11BAC ||
                            key == 0xFFF5 || key == 0x4A86D09) {
                            fn_800C64E4();
                            int basis = fn_800C6510();
                            owner->unk279C = fn_80176044(
                                (double)lbl_804D7CC4, (double)lbl_804D7CA0,
                                (double)(float)sub[0x42], owner->unk248,
                                owner->unk1D68->id, basis + 0x24,
                                (void*)GhostAimTargetFilter, (int)owner);
                        } else {
                            fn_800C64E4();
                            int basis = fn_800C6510();
                            owner->unk279C = fn_80176044(
                                (double)lbl_804D7CC4, (double)lbl_804D7CC0,
                                (double)(float)sub[0x42], owner->unk248,
                                owner->unk1D68->id, basis + 0x24,
                                (void*)GhostAimTargetFilter, (int)owner);
                        }
                    }
                }
                fn_80176000(owner->unk279C, owner->unk248);
                owner->unk78 = 1;
                owner->unk1BA0 = owner->unk26E8;
                int draw = fn_8003AA9C(owner->unk58, owner->unk140C, 1);
                unsigned long arg = fn_80037BEC(owner->unk58);
                fn_8008F630(owner->unkD4, owner->unk140C, draw, arg);
                float zero2 = lbl_804D7CA0;
                int state = owner->unk78;
                if (state == 1) {
                    Msg402 m;
                    m.unk4 = 0;
                    SendMsg(owner->unk1BA0->id, 0x402, &m);
                    char matA[48];
                    char matB[48];
                    fn_8009ED70(matA, matB);
                    fn_8003A66C(owner->unk58, matA, owner->unk140C);
                } else if (state < 1 && -1 < state) {
                    int dst = owner->unk140C;
                    *(float*)(dst + 0x10) = lbl_804D7CA0;
                    *(float*)(dst + 0x14) = zero2;
                    *(float*)(dst + 0x18) = zero2;
                    fn_80101740((double)owner->unk1F0, owner->unk140C,
                                *(unsigned char*)(draw + owner->unk12C4),
                                *(unsigned char*)(draw + owner->unk12B4),
                                **owner->unk26C4);
                    char matC[56];
                    fn_8009ED70(matC, owner);
                    fn_8003A66C(owner->unk58, matC, owner->unk140C);
                }
                fn_80158B90(owner->unk248, (int)owner, (int)owner->unk26E8);
                return 1;
            }
        }
        return 0;
    }

    target = owner->unk1D68;
    if (owner->unk26E4 == 0) {
        if (target == 0) {
            ok = 0;
        } else {
            int answer;
            Query(&answer, owner->unk26E0->id, 0x41E, 0xFA969430);
            for (int i = 0; i < lbl_804D4E20; i++) {
                GhostSlot* s = lbl_803A5968[i];
                if (s->active != 0 &&
                    (s->unk04 == (GhostSlot*)target || s->unk08 == (GhostSlot*)target)) {
                    ok = 0;
                    goto taken3;
                }
            }
            if (fn_80115F68(target->id) <= lbl_804D7CA0 || fn_802A1D04() != 0 || answer != 0) {
                ok = 1;
            } else {
                ok = 0;
            }
        }
    } else {
        ok = 0;
    }
taken3:
    if (!ok) {
        return 0;
    }

    GhostActor* desired = owner->unk1D68;
    int result = -1;
    for (int i = 0; i < lbl_804D4E20; i++) {
        GhostSlot* s = lbl_803A5968[i];
        if (s->active != 0 && s->unk04 == (GhostSlot*)desired) {
            result = 0;
            goto attached;
        }
    }
    if (fn_802A1D04() == 8) {
        QueryMsg m;
        m.hash = 0xA92756;
        m.unk8 = 0;
        *(int*)&m.result = (int)desired;
        SendMsg(0x4B000000, 0x41E, &m);
        if (*(int*)&m.result == 0) {
            result = 0;
            goto attached;
        }
    }
    result = fn_801554E0((GhostSlot*)((char*)owner + 0x26E0), (GhostSlot*)desired, 1);
attached:
    if (result != 1) {
        return 0;
    }
    self->unk58 = 1;
    owner->flags |= 0x1000002;
    fn_8001BAA8(0);
    int attachedId = owner->unk1D68->id;
    if (attachedId != -1) {
        MsgC14 msg;
        msg.arg = 0;
        msg.value = owner->id;
        msg.hash = 0x672F35C2;
        SendMsg(attachedId, 0xC14, &msg);
    }
    fn_80225CF8();
    return 1;
}

// 0x8015670C: aim-target filter used when the possession builds its target
// list.  A null handle passes, the ghost itself is rejected, and every other
// handle is accepted unless it is the ghost's current possession target.
unsigned long GhostAimTargetFilter(int value, int owner) {
    if ((unsigned int)value == 0) {
        return 1;
    }
    if ((unsigned int)value == (unsigned int)owner) {
        return 0;
    }
    int target = (int)((GhostActor*)owner)->unk1D68;
    return (unsigned long)((unsigned int)(target - value) |
                           (unsigned int)(value - target)) >> 31;
}

// 0x80156744: may the possession record `self` (a GhostSlot view of a
// GhostActor's embedded record) take `target`?  No when something is already
// attached, when another active ghost possesses the target, or when the target
// fails the name query; yes when the target is in spirit range, the world is
// paused, or the query was answered.
int GhostSlotCanPossess(GhostSlot* self, GhostActor* target) {
    if (self->unk04 != 0) {
        return 0;
    }
    if (target == 0) {
        return 0;
    }
    int answer;
    Query(&answer, self->unk00->id, 0x41E, 0xFA969430);
    GhostSlot* slot;
    int i;
    for (i = 0; i < lbl_804D4E20; i++) {
        slot = lbl_803A5968[i];
        if (slot->active != 0 &&
            (slot->unk04 == (GhostSlot*)target || slot->unk08 == (GhostSlot*)target)) {
            return 0;
        }
    }
    if (fn_80115F68(target->id) <= lbl_804D7CA0 || fn_802A1D04() != 0 || answer != 0) {
        return 1;
    }
    return 0;
}

// 0x8015688C: "Quick Flash" possession-menu callback.  Bit 0 set means the
// menu answer was taken, so the slot's own hook runs and the possession keeps
// ticking; otherwise the menu state is cleared.
int GhostQuickFlashNotify(unsigned int flags, int unused, GhostSlot* self) {
    if ((flags & 1) != 0) {
        if (self->unk90 != 0) {
            self->unk90(self);
        }
        GhostPossessTick(self);
    } else {
        self->active = 0;
        self->unk6C = -1;
        self->unk70 = -1;
        self->unk74 = -1;
    }
    return 1;
}

extern float lbl_804D7CC8;
void FontRelease(int font);
extern "C" void* memset(void* dst, int value, unsigned long size);

// 0x801568FC: release the record's three font handles and reset its text,
// flash and menu state to the defaults.
void GhostRecordReset(GhostSlot* self) {
    if (self->unk6C != -1) {
        FontRelease(self->unk6C);
        self->unk6C = -1;
    }
    if (self->unk70 != -1) {
        FontRelease(self->unk70);
        self->unk70 = -1;
    }
    if (self->unk74 != -1) {
        FontRelease(self->unk74);
        self->unk74 = -1;
    }
    self->unk04 = 0;
    self->unk08 = 0;
    float flash = lbl_804D7CC8;
    float reset = lbl_804D7CA0;
    self->unk50 = reset;
    self->unk48 = -1;
    self->unk4C = reset;
    self->unk54 = flash;
    self->active = 0;
    self->unk60 = 0;
    self->unk58 = 0;
    self->unk80 = -1;
    self->unk88 = 0;
    self->unk84 = 0;
}

// 0x801569AC: register `self` in the ghost-slot table (16 entries at most),
// then initialise the record: attach it to `owner`, store the HUD slot and the
// behaviour/menu callbacks.
void GhostRecordInit(GhostSlot* self, GhostSlot* owner, int hudSlot,
                     void (*unk8C)(GhostSlot*), void (*unk90)(GhostSlot*),
                     void (*unk94)(GhostSlot*), void (*unk98)(GhostSlot*),
                     void (*update)(float)) {
    if (lbl_804D4E20 == 0x10) {
        return;
    }
    int i;
    for (i = 0; i < lbl_804D4E20; i++) {
        if (lbl_803A5968[i] == self) {
            break;
        }
    }
    if (i == lbl_804D4E20) {
        lbl_803A5968[lbl_804D4E20++] = self;
    }
    memset(self, 0, 0xAC);
    self->unk00 = owner;
    self->unk48 = -1;
    self->unk64 = hudSlot;
    self->unk80 = -1;
    self->unk88 = 0;
    self->unk8C = unk8C;
    self->unk90 = unk90;
    self->unk94 = unk94;
    self->unk98 = unk98;
    self->update = update;
    self->unk6C = -1;
    self->unk70 = -1;
    self->unk74 = -1;
    self->unk84 = 0;
}

// Ghost system init hook. All ghost state lives in static storage, so there is
// nothing to do at runtime.
void GhostSystemInit(void)
{
}

// Reset the ghost bookkeeping: the loose-slot table, the four {ghost id, actor
// id} pairs and the pending ghost id.
void GhostSystemReset(void)
{
    lbl_803A5968[0] = 0;
    lbl_803A5968[1] = 0;
    lbl_803A5968[2] = 0;
    lbl_803A5968[3] = 0;
    lbl_803A5968[4] = 0;
    lbl_803A5968[5] = 0;
    lbl_803A5968[6] = 0;
    lbl_803A5968[7] = 0;
    lbl_803A5968[8] = 0;
    lbl_803A5968[9] = 0;
    lbl_803A5968[10] = 0;
    lbl_803A5968[11] = 0;
    lbl_803A5968[12] = 0;
    lbl_803A5968[13] = 0;
    lbl_803A5968[14] = 0;
    lbl_803A5968[15] = 0;

    lbl_804D4E20 = 0;

    lbl_803A59A8[0] = 0;
    lbl_803A59A8[1] = 0;
    lbl_803A59A8[2] = 0;
    lbl_803A59A8[3] = 0;
    lbl_803A59A8[4] = 0;
    lbl_803A59A8[5] = 0;
    lbl_803A59A8[6] = 0;
    lbl_803A59A8[7] = 0;
}

// ---- flash-position helpers used by the possession flash (0x80156B1C) ----

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[12];
};

struct Player;
struct Place;

void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void MtxGetEuler(Vec3* euler, Mtx34* mtx);
void MtxSetEuler(Mtx34* mtx, Vec3* euler);
void EulerNormalize(Vec3* out, Vec3* in);
void RadToFixed(IVec3* out, Vec3* in);
void VecSwapXY2(IVec3* out, IVec3* in);
void PlayerBeginBlend(Player* player, Place* place, float value);
int BitsetFirstClear(unsigned long* bits, int start, int count);
extern "C" int rand(void);
extern "C" float sqrtf(float);
extern "C" float fabsf(float);
extern "C" void fn_8001ED98(int* dst, int size, void (*callback)(void), void* arg, int flag);
extern "C" int fn_8001ACD4(Actor* actor, Vec3* pos, Vec3* ax, Vec3* ay, Vec3* az,
                           Vec3* offset, unsigned long flags, void* callback,
                           GhostSlot* target);
extern "C" int fn_8001F0DC(Actor* actor, IVec3* pos, int a, int b, int c, int d,
                           int* flag, int e);
extern "C" void fn_8016D68C(Vec3* out, Vec3* euler);
extern "C" void fn_8016D4EC(Vec3* out, Vec3* euler);
extern "C" void fn_800A025C(Vec3* out, Mtx34* mtx, int index);
extern "C" void fn_800E1098(float scale, Vec3* dst, Vec3* src);
struct FlashSpotActor;
unsigned long GhostFlashSpotFilter(int* candidate, int target);
int GhostFlashSpotPlace(FlashSpotActor* other, FlashSpotActor* owner);

extern int lbl_804D4E24;
extern int lbl_804D4E28;
extern int lbl_804D2F20;
extern float lbl_804D2F18;
extern float lbl_804D2F1C;
extern float lbl_804D2F24;
extern float lbl_804D7CCC;
extern float lbl_804D7CD0;
extern float lbl_804D7CD4;
extern float lbl_804D7CD8;
extern float lbl_804D7CDC;

int GhostFlashFindSpot(Mtx34* out, GhostSlot* self, float angle);

// 0x80156B1C: build the ghost's flash matrix (asking the world for a free spot
// around the actor unless the mode pins it) and send the 0xC07 position message.
int GhostFlashNotify(GhostSlot* self, int mode) {
    int* bounds;
    int ok = 1;

    lbl_804D4E28 = 0;
    bounds = *(int**)((char*)self->unk00 + 0x40);
    fn_8001ED98((int*)((char*)self->unk00 + 0x24), bounds[4] - bounds[1],
                (void (*)(void))GhostFlashSpotPlace, self->unk00, 0);
    if (lbl_804D4E28 == 0) {
        if (lbl_804D4E24 != 0) {
            mode = lbl_804D4E24;
        }

        Mtx34 mtx;
        Mtx34 blend;
        MsgC07 msg;
        IVec3 swapped;
        Vec3 euler;
        IVec3 angles;
        switch (mode) {
        case 3:
            if (GhostFlashFindSpot(&mtx, self, lbl_804D7CA0) == 0 &&
                GhostFlashFindSpot(&mtx, self, lbl_804D7CCC) == 0) {
                ok = 0;
            }
            break;
        case 4:
            if (GhostFlashFindSpot(&mtx, self, lbl_804D7CCC) == 0 &&
                GhostFlashFindSpot(&mtx, self, lbl_804D7CA0) == 0) {
                ok = 0;
            }
            break;
        case 1:
            if (GhostFlashFindSpot(&mtx, self, lbl_804D7CA0) == 0) {
                ok = 0;
            }
            break;
        case 5:
            ActorGetMatrix(&mtx, (Actor*)self->unk00);
            ok = 0;
            mtx.m[11] = mtx.m[11] - lbl_804D7CD0;
            break;
        default:
            if (GhostFlashFindSpot(&mtx, self, lbl_804D7CCC) == 0) {
                ok = 0;
            }
            break;
        }

        MtxGetEuler(&euler, &mtx);
        if (ok) {
            euler.z = euler.z + lbl_804D7CCC;
        }
        EulerNormalize(&euler, &euler);

        if ((int)(self->unk00->id >> 24) == 2) {
            Actor* a = (Actor*)self->unk00;
            MtxSetEuler(&blend, &euler);
            blend.m[9] = *(float*)((char*)a + 0x1E4);
            blend.m[10] = *(float*)((char*)a + 0x1E8);
            blend.m[11] = *(float*)((char*)a + 0x1EC);
            PlayerBeginBlend((Player*)a, (Place*)&blend, lbl_804D2F24);
        }

        RadToFixed(&angles, &euler);
        VecSwapXY2(&swapped, &angles);

        msg.unk4 = 3;
        msg.x = (int)mtx.m[9];
        msg.y = (int)mtx.m[10];
        msg.z = (int)mtx.m[11];
        SendMsg(self->unk00->id, 0xC07, &msg);
    }
    return 1;
}

// 0x80156D90: pick a random free spot in the actor's bounds and build the
// flash matrix there.  Returns 0 (with the plain actor matrix) when every slot
// has already been tried.
int GhostFlashFindSpot(Mtx34* out, GhostSlot* self, float angle) {
    Mtx34 base;
    Vec3 euler;
    Vec3 dir;
    Vec3 pos;
    Vec3 vec;
    IVec3 fixed;
    Vec3 offset;
    Vec3 az;
    Vec3 ay;
    Vec3 ax;
    unsigned long used;
    int flag;
    int done;

    ActorGetMatrix(&base, (Actor*)self->unk00);
    MtxGetEuler(&euler, &base);
    used = 0;
    done = 0;
    euler.x = lbl_804D7CA0;
    euler.y = lbl_804D7CA0;

    do {
        if (done) {
            MtxSetEuler(out, &dir);
            out->m[9] = pos.x;
            out->m[10] = pos.y;
            out->m[11] = pos.z;
            return 1;
        }
        if (BitsetFirstClear(&used, 1, lbl_804D2F20) == -1) {
            *out = base;
            return 0;
        }
        int k = rand() % lbl_804D2F20;
        unsigned long mask = 1UL << (k & 31);
        if ((used & mask) == 0) {
            used |= mask;

            float spread = (float)k * (lbl_804D2F1C / (float)(lbl_804D2F20 - 1)) -
                           lbl_804D2F1C * lbl_804D7CD4;
            fn_8016D68C(&dir, &euler);
            dir.z = dir.z + (spread + angle);
            EulerNormalize(&dir, &dir);
            fn_8016D4EC(&vec, &dir);

            float inv;
            if (fabsf(vec.x) > lbl_804D7CD8 || fabsf(vec.y) > lbl_804D7CD8 ||
                fabsf(vec.z) > lbl_804D7CD8) {
                inv = 1.0f / sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
                vec.x = vec.x * inv;
                vec.y = vec.y * inv;
                vec.z = vec.z * inv;
                inv = 1.0f / inv;
            } else {
                vec.x = lbl_804D7CA0;
                vec.y = lbl_804D7CA0;
                vec.z = lbl_804D7CA0;
            }

            vec.x = vec.x * lbl_804D2F18;
            vec.y = vec.y * lbl_804D2F18;
            vec.z = vec.z * lbl_804D2F18;
            pos.x = vec.x + base.m[9];
            pos.y = vec.y + base.m[10];
            pos.z = vec.z + base.m[11];

            int* bounds = *(int**)((char*)self->unk00 + 0x40);
            float minHW = (float)(bounds[4] - bounds[1]);
            if ((float)(bounds[3] - bounds[0]) < minHW) {
                minHW = (float)(bounds[3] - bounds[0]);
            }
            float rowScale = lbl_804D7CD4 * minHW;
            fn_800A025C(&ax, &base, 0);
            fn_800E1098(rowScale, &ax, &ax);
            fn_800A025C(&ay, &base, 1);
            fn_800E1098(rowScale, &ay, &ay);
            fn_800A025C(&az, &base, 2);
            fn_800E1098(lbl_804D7CDC * (float)(bounds[5] - bounds[2]), &az, &az);

            offset.x = pos.x - base.m[9];
            offset.y = pos.y - base.m[10];
            offset.z = pos.z - base.m[11];

            int found = fn_8001ACD4((Actor*)self->unk00, (Vec3*)&base.m[9],
                                    &ax, &ay, &az, &offset, 0x10000000,
                                    (void*)GhostFlashSpotFilter, self->unk04);
            if (found == 0) {
                flag = 0;
                fixed.x = (int)pos.x;
                fixed.y = (int)pos.y;
                fixed.z = (int)pos.z;
                fn_8001F0DC((Actor*)self->unk00, &fixed, 0, 0, 0, 0, &flag, 0);
                if (flag == 0) {
                    found = 1;
                }
            }
            if (found == 0) {
                done = 1;
            }
        }
    } while (1);
}
// 0x80157288: candidate filter for the flash-position search.  Passes a
// candidate unless its id is the target id.
unsigned long GhostFlashSpotFilter(int* candidate, int target) {
    return ((unsigned int)(target - *candidate) |
            (unsigned int)(*candidate - target)) >> 31;
}

// The two actors the flash-spot search looks at: the candidate occupant
// (`other`) and the ghost's own actor (`owner`).
struct FlashSpotActor {
    char pad00[0x24];
    int x;                  // 0x24
    int y;                  // 0x28
    int z;                  // 0x2C
    char pad30[0x38 - 0x30];
    unsigned long id;       // 0x38
    char pad3C[0x40 - 0x3C];
    int* bounds;            // 0x40
    char pad44[0x67C - 0x44];
    float flashX;           // 0x67C
    float flashY;           // 0x680
    float flashZ;           // 0x684
};

extern float lbl_804D7CE0;
extern float lbl_804D7CE4;

// 0x801572A0: iterator callback of the flash-spot search.  Asks the candidate
// occupant for its aim index, projects it onto the ghost's aim ray and, when
// the projection falls inside the ghost's reach, moves the flash to the
// ghost's offset and sends the 0xC07 position message.
int GhostFlashSpotPlace(FlashSpotActor* other, FlashSpotActor* owner) {
    if (other == 0 || (int)(owner->id >> 24) != 2 ||
        (int)(other->id >> 24) != 1) {
        return 0;
    }

    int index;
    int sent;
    {
        QueryMsg m;
        *(int*)&m.result = 0;
        m.hash = 0x914B98A8;
        m.unk8 = 0;
        sent = SendMsg(other->id, 0x41E, &m);
        index = *(int*)&m.result;
    }
    if (sent == 0 || index < 0) {
        return 1;
    }

    IVec3 aim;
    ActorAim((int*)&aim, (Actor*)other, index);

    float scale = lbl_804D7CE0;
    float ax = scale * (float)aim.x;
    float ay = scale * (float)aim.y;
    float az = scale * (float)aim.z;
    float dx = (float)(owner->x - other->x);
    float dy = (float)(owner->y - other->y);
    float dz = (float)(owner->z - other->z);

    float t = (ax * dx + ay * dy + az * dz) / (ax * ax + ay * ay + az * az);

    Vec3 vec;
    vec.x = ax * t;
    vec.y = ay * t;
    vec.z = az * t;

    int* bounds = owner->bounds;
    float span = (float)(bounds[4] - bounds[1]);
    float radius;
    if (t < lbl_804D7CA0) {
        radius = lbl_804D7CE4 * (span - (float)bounds[index]);
    } else {
        radius = lbl_804D7CE4 * (span + (float)bounds[index + 3]);
    }
    if (vec.x * vec.x + vec.y * vec.y + vec.z * vec.z < radius * radius) {
        float px = owner->flashX - vec.x;
        float py = owner->flashY - vec.y;
        float pz = owner->flashZ - vec.z;
        fn_800E1098(radius, &vec, &vec);
        vec.x = vec.x + px;
        vec.y = vec.y + py;
        vec.z = vec.z + pz;

        MsgC07 msg;
        msg.unk4 = 1;
        msg.x = (int)vec.x;
        msg.y = (int)vec.y;
        msg.z = (int)vec.z;
        SendMsg(owner->id, 0xC07, &msg);
        lbl_804D4E28 = 1;
    }
    return 1;
}

// Payload of message 0x414 (frame delta): a zeroed 0x10-byte record holding
// the frame time at 0xC.
struct Msg414 {
    int unk00;    // 0x00
    float dt;     // 0x04
    int unk08;    // 0x08
    int unk0C;    // 0x0C
};

extern "C" void fn_8008910C(void);
extern "C" void fn_8004A254(float dt);
extern "C" void fn_801E0880(float dt);
extern "C" void fn_800C6C28(float dt);
extern "C" void fn_80005A64(float dt);
extern "C" void* fn_800C6414(int value);
extern "C" void fn_8016ED08(Mtx34* mtx);
extern "C" void AudioUpdate(float dt);
extern "C" void fn_8016F950(float dt);
void PlacementToMatrix(Mtx34* mtx, Place* place);
void fn_8007F0D4(float dt);

// 0x80157610: per-frame ghost-system update, installed as the callback of
// fn_80088700.  Runs the frame's audio/effect passes, ticks every active ghost
// slot, then refreshes the camera placement and the audio listener.
int GhostFrameUpdate(float dt, void* p) {
    GhostActor* owner = (GhostActor*)p;
    fn_8008910C();
    fn_8004A254(dt);
    if (owner != 0 && owner->unk26E8 != 0) {
        Msg414 msg;
        memset(&msg, 0, sizeof(msg));
        msg.dt = dt;
        SendMsg(owner->unk26E8->id, 0x414, &msg);
    }
    fn_801E0880(dt);
    fn_800C6C28(dt);
    fn_80005A64(dt);
    GhostSlot* slot;
    int i;
    for (i = 0; i < lbl_804D4E20; i++) {
        slot = lbl_803A5968[i];
        if (slot->active != 0 && slot->update != 0) {
            slot->update(dt);
        }
    }
    Mtx34 mtx;
    PlacementToMatrix(&mtx, (Place*)fn_800C6414(0));
    fn_8016ED08(&mtx);
    AudioUpdate(dt);
    fn_8016F950(dt);
    fn_8007F0D4(dt);
    return 1;
}

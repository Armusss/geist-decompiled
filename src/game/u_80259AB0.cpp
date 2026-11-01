// u_80259AB0.cpp: the "hyd_rico" character AI (0x80259AB0-0x8025ECF8): a state machine whose states are
// enter / update callbacks taking the actor, plus the actor's message handler and init/destroy.
// Only the smaller state callbacks are drafted so far; every larger function is declared with a TODO.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned char u8;

struct Vec3 {
    float x, y, z;
};

// Reply written by SendMsg 0xC14 (state change notification).
struct StateMsg {
    int unk0;
    int flag;     // +4
    int target;   // +8
    u32 hash;     // +0xC
};

// AI data block, allocated by RicoInit (0xAC bytes), at actor + 0x130.
struct RicoData {
    int state;         // 0x00
    float unk04;       // 0x04
    float unk08;       // 0x08
    float unk0C;       // 0x0C
    float unk10;       // 0x10
    float unk14;       // 0x14
    float unk18;       // 0x18
    float unk1C;       // 0x1C
    float unk20;       // 0x20
    int unk24;         // 0x24
    int unk28;         // 0x28
    int unk2C;         // 0x2C
    int unk30;         // 0x30
    int unk34;         // 0x34
    int unk38;         // 0x38
    int unk3C;         // 0x3C
    int unk40;         // 0x40
    int unk44;         // 0x44
    int unk48;         // 0x48
    int unk4C;         // 0x4C
    int unk50;         // 0x50
    int unk54;         // 0x54
    int unk58;         // 0x58
    int unk5C;         // 0x5C
    int unk60;         // 0x60
    int unk64;         // 0x64
    float unk68;       // 0x68
    float unk6C;       // 0x6C
    float unk70;       // 0x70
    float unk74;       // 0x74
    float unk78;       // 0x78
    float unk7C;       // 0x7C
    int unk80;         // 0x80
    float unk84;       // 0x84
    float unk88;       // 0x88
    float unk8C;       // 0x8C
    float unk90;       // 0x90
    float unk94;       // 0x94
    float unk98;       // 0x98
    u32 flags;         // 0x9C
    float unkA0;       // 0xA0
    float unkA4;       // 0xA4
    float unkA8;       // 0xA8
};

// Character definition block (actor + 0xC4); only the field used here.
struct RicoInfo {
    u8 pad00[0x30];  // 0x00
    int unk30;       // 0x30
};

// The actor (only the fields this file touches).
struct Rico {
    u8 pad00[0x38];            // 0x00
    int id;                    // 0x38
    u8 pad3C[0x84 - 0x3C];     // 0x3C
    float pos[3];              // 0x84
    u8 pad90[0xC4 - 0x90];     // 0x90
    RicoInfo* info;            // 0xC4
    u8 padC8[0xCC - 0xC8];     // 0xC8
    int unkCC;                 // 0xCC
    u8 padD0[0xFC - 0xD0];     // 0xD0
    void* unkFC;               // 0xFC
    u8 pad100[0x104 - 0x100];  // 0x100
    void* destroy;             // 0x104
    u8 pad108[0x10C - 0x108];  // 0x108
    void* msgHandler;          // 0x10C
    u8 pad110[0x130 - 0x110];  // 0x110
    RicoData* data;            // 0x130
    u8 pad134[0x138 - 0x134];  // 0x134
    float dt;                  // 0x138
    u8 pad13C[0x26C - 0x13C];  // 0x13C
    int unk26C;                // 0x26C
    u8 cond[0x288 - 0x270];    // 0x270 condition bits
    int unk288;                // 0x288
    u8 pad28C[0x2B0 - 0x28C];  // 0x28C
    float aimAngle;            // 0x2B0
    u8 pad2B4[0x5FC - 0x2B4];  // 0x2B4
    void* unk5FC;              // 0x5FC
    void* unk600;              // 0x600
    void* unk604;              // 0x604
    u8 pad608[0x894 - 0x608];  // 0x608
    u8 beh[0x904 - 0x894];     // 0x894
    float unk904;              // 0x904
};

int HasCondition(void* cond, int bit);                          // 0x800A7AC4
void ConditionSetBit(void* cond, int bit, int on);              // 0x800A7A54
void ConditionUpdateBit(void* cond, int bit, int on);           // 0x800A7B08
void* VAlloc(int size);                                         // 0x8009DA68
void VFree(void* p);                                            // 0x8009DA3C

extern "C" int SendMsg(int to, int msg, void* data);   // 0x800265D4
extern int lbl_804D54E0;
extern int lbl_804D3694;
extern int lbl_804D3698;

// TODO (not drafted yet, 0x688 bytes at 0x80259AB0): actor message handler
// TODO (not drafted yet, 0x284 bytes at 0x8025A138)
// TODO (not drafted yet, 0x9D4 bytes at 0x8025A3BC)
// TODO (not drafted yet, 0x44C bytes at 0x8025AD90): per-frame update

// Destroy: frees the AI data block.
void RicoDestroy(Rico* a) {
    a->destroy = 0;
    VFree(a->data);
    a->data = 0;
}

void ActorPlayAnim(int id, u32 hash, int arg);   // 0x800C5654
extern "C" void fn_80162610(void* actor, void* beh, int on);

// Condition bits (guessed meanings).
enum {
    COND_3 = 3,
    COND_5 = 5,
    COND_6 = 6
};

// Notifies the owner that the AI changed state.
static inline void NotifyState(Rico* a, int flag, u32 hash) {
    if (a->id != -1) {
        StateMsg m;
        m.target = a->id;
        m.hash = hash;
        m.flag = flag;
        SendMsg(a->id, 0xC14, &m);
    }
}

// State 0x33 check: leaves the stun once the condition clears.
int RicoCheckStun(Rico* a) {
    RicoData* d = a->data;
    if (HasCondition(a->cond, 0x33) == 0) {
        d->flags &= ~2;
        ConditionSetBit(a->cond, 0x24, 0);
        ConditionSetBit(a->cond, 0x25, 0);
        return 1;
    }
    return 0;
}


// Leaves the attack state: clears the beh, stops notifying the owner.
int RicoExitAttack(Rico* a) {
    RicoData* d = a->data;
    fn_80162610(a, a->beh, 0);
    ConditionUpdateBit(a->cond, 3, 0);
    d->unk5C = 0;
    d->unk28 = 0;
    d->flags &= ~4;
    d->unk4C = 0;
    NotifyState(a, 1, 0x665BF55F);
    d->unk58 = 0;
    ConditionSetBit(a->cond, 0x3A, 0);
    lbl_804D54E0 = 0;
    lbl_804D3694 = lbl_804D3698;
    return 1;
}

int RicoEnterState10(Rico* a) {
    RicoData* d = a->data;
    d->state = 10;
    a->unk288 = 0;
    ConditionUpdateBit(a->cond, 3, 0);
    ConditionSetBit(a->cond, 6, 1);
    if (HasCondition(a->cond, 0x24) == 0 && HasCondition(a->cond, 0x25) == 0) {
        d->unk8C = 0.0f;
    }
    return 1;
}

int RicoExitState9(Rico* a) {
    fn_80162610(a, a->beh, 0);
    ConditionUpdateBit(a->cond, 3, 0);
    return 1;
}

int RicoEnterState9(Rico* a) {
    a->data->state = 9;
    a->unk288 = 0;
    ConditionUpdateBit(a->cond, 3, 1);
    ConditionSetBit(a->cond, 6, 0);
    return 1;
}

int RicoClearMoveConds(Rico* a) {
    ConditionSetBit(a->cond, 0x24, 0);
    ConditionSetBit(a->cond, 0x25, 0);
    ConditionSetBit(a->cond, 6, 0);
    return 1;
}

int RicoEnterState8(Rico* a) {
    RicoData* d = a->data;
    d->state = 8;
    a->unk288 = 0;
    ConditionUpdateBit(a->cond, 4, 0);
    ConditionUpdateBit(a->cond, 3, 0);
    ConditionSetBit(a->cond, 6, 1);
    ConditionUpdateBit(a->cond, 0x25, 0);
    d->unk34 = 0;
    d->flags &= ~0x2000;
    if ((d->flags & 2) == 0) {
        d->flags |= 2;
        d->unkA0 = a->pos[0];
        d->unkA4 = a->pos[1];
        d->unkA8 = a->pos[2];
        NotifyState(a, 0, 0x1F1BB10B);
        ActorPlayAnim(a->id, 0xE49A6366, a->id);
    }
    if (HasCondition(a->cond, 0x24) == 0 && HasCondition(a->cond, 0x25) == 0) {
        d->unk8C = 0.0f;
    }
    return 1;
}

int RicoCheckRecover(Rico* a) {
    RicoData* d = a->data;
    if ((d->flags & 0x8000) == 0) {
        return 0;
    }
    if (HasCondition(a->cond, 0x2F) == 0) {
        d->flags &= ~0x800;
        d->flags &= ~0x8000;
        return 1;
    }
    d->unk74 = 0.0f;
    float scale;
    if (a->unk904 > 0.7f * (float)a->info->unk30) {
        scale = 0.2f;
    } else {
        scale = 0.1f;
    }
    if (d->unk78 >= (float)a->info->unk30 * scale) {
        d->flags |= 0x800;
        d->flags &= ~0x8000;
        ConditionSetBit(a->cond, 0x2F, 0);
        return 1;
    }
    return 0;
}

int RicoClearRecover(Rico* a) {
    RicoData* d = a->data;
    ConditionSetBit(a->cond, 0x2F, 0);
    d->unk28 = 0;
    return 1;
}

int RicoEnterState7(Rico* a) {
    RicoData* d = a->data;
    d->state = 7;
    d->flags &= ~0x1000;
    d->unk74 = 0.0f;
    a->unk288 = 0;
    ConditionUpdateBit(a->cond, 3, 0);
    ConditionSetBit(a->cond, 0x2F, 1);
    d->unk58 = 0;
    ConditionSetBit(a->cond, 0x3A, 0);
    NotifyState(a, 1, 0x665BF55F);
    lbl_804D3694 = lbl_804D3698;
    return 1;
}

int RicoExitState5(Rico* a) {
    RicoData* d = a->data;
    ConditionSetBit(a->cond, 5, 0);
    ConditionSetBit(a->cond, 6, 0);
    d->unk28 = 0;
    d->flags &= ~4;
    d->unk4C = 0;
    NotifyState(a, 1, 0x665BF55F);
    d->unk58 = 0;
    ConditionSetBit(a->cond, 0x3A, 0);
    lbl_804D54E0 = 0;
    lbl_804D3694 = lbl_804D3698;
    d->unk78 = 0.0f;
    return 1;
}

int RicoEnterState6(Rico* a) {
    RicoData* d = a->data;
    d->state = 6;
    d->flags |= 4;
    d->unk7C = a->unk904;
    a->unk288 = 0;
    NotifyState(a, 0, 0xA4FC335D);
    ConditionUpdateBit(a->cond, 3, 0);
    ConditionSetBit(a->cond, 5, 1);
    ConditionSetBit(a->cond, 6, 1);
    ConditionUpdateBit(a->cond, 0x25, 0);
    d->flags &= ~0x4000;
    d->flags &= ~0x2000;
    return 1;
}

bool RicoNotCond37(Rico* a) {
    return HasCondition(a->cond, 0x37) == 0;
}

int RicoExitState37(Rico* a) {
    RicoData* d = a->data;
    ConditionSetBit(a->cond, 0x37, 0);
    d->unk28 = 0;
    d->unk4C = 0;
    d->unk58 = 0;
    ConditionSetBit(a->cond, 0x3A, 0);
    lbl_804D3694 = lbl_804D3698;
    NotifyState(a, 1, 0x665BF55F);
    return 1;
}

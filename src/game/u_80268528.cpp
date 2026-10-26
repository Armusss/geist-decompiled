// u_80268528.cpp: Hermit Spit effect callbacks.
// Names are descriptive guesses based on what the code does.

// State block reached through EffectObj's pointer field at 0xC.
struct HermitSpitState {
    int state;          // 0x00
    unsigned char pad04[0x7C];
    int timer;          // 0x80, -1 when no timer is running
    float elapsed;      // 0x84
};

struct EffectObj {
    unsigned char pad00[0x8];
    int id;                 // 0x08
    HermitSpitState* state; // 0x0C
};

// Query view of the same state block: mode flag, two ids, and a pointer to
// the per-spawn parameter block.
struct HermitSpitParams {
    int mode;               // 0x00
    int idA;                // 0x04
    int idB;                // 0x08
    unsigned char pad0C[0x58];
    int idC;                // 0x64
};

struct HermitSpitQueryState {
    int enabled;                // 0x00
    int idA;                    // 0x04
    int idB;                    // 0x08
    HermitSpitParams* params;   // 0x0C
};

extern int lbl_804D3770;

extern "C" void fn_8016EEF4(int handle);

void HermitSpitReset(EffectObj* obj) {
    obj->state->state = 0;
}

void HermitSpitUpdate(EffectObj* obj, float dt) {
    HermitSpitState* s = obj->state;

    lbl_804D3770 = 1;
    if (s->timer != -1) {
        s->elapsed += dt;
        if (s->elapsed > 200.0f) {
            fn_8016EEF4(s->timer);
            s->timer = -1;
        }
    }
}

// Message hashes handled by the query.
static const int kMsgSpitFire = -0x07D6246D;   // 0xF829DB93
static const int kMsgSpitFire2 = 0x42E50A3D;
static const int kMsgSpitIdle = -0x686E50AF;   // 0x9791AF51
static const int kMsgSpitIdle2 = 0x0D19E338;
static const int kMsgSpitHit = -0x697341C3;    // 0x968CBE3D
static const int kMsgSpitHit2 = -0x1948E434;   // 0xE6B71BCC
static const int kMsgSpitCheck = -0x69DDB034;  // 0x96224FCC
static const int kMsgSpitCheck2 = 0x0020EE36;

// Answer hashes returned by the query.
static const int kHermitSpitAllowed = 0x11D2C025;
static const int kHermitSpitBusy = 0x04A3E183;
static const int kHermitSpitBlocked = (int)0xC0BECF7E;
static const int kHermitSpitIgnore = 0x4848CA1E;
static const int kHermitSpitDeny = 0x3DA260F7;

extern "C" int fn_800F7D28(int id, int other);

int HermitSpitQuery(EffectObj* obj, int msg) {
    HermitSpitQueryState* s = (HermitSpitQueryState*)obj->state;
    HermitSpitParams* p = s->params;

    switch (msg) {
    case kMsgSpitFire:
    case kMsgSpitFire2:
        if (fn_800F7D28(s->idA, p->idC) != 0)
            return kHermitSpitBusy;
        if (fn_800F7D28(obj->id, p->idC) != 0)
            return kHermitSpitBlocked;
        return kHermitSpitAllowed;
    case kMsgSpitIdle:
    case kMsgSpitIdle2:
        if (s->enabled == 1)
            return kHermitSpitBusy;
        return kHermitSpitDeny;
    case kMsgSpitHit:
    case kMsgSpitHit2:
        if (p->idB == fn_800F7D28(s->idA, s->idB))
            return kHermitSpitIgnore;
        if (fn_800F7D28(obj->id, p->idA) != 0)
            return kHermitSpitBusy;
        return kHermitSpitAllowed;
    case kMsgSpitCheck:
    case kMsgSpitCheck2:
        if (fn_800F7D28(s->idA, s->idB) != 0)
            return kHermitSpitBusy;
        if (fn_800F7D28(obj->id, p->idA) != 0)
            return kHermitSpitBlocked;
        return kHermitSpitAllowed;
    }
    return kHermitSpitDeny;
}

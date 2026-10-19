// u_802A770C.cpp: host possess-action handling (0x802A770C-0x802A7E3C).
// A host is an actor the player can possess. The action handler queries the host's two
// links ("new"/"free"), then dispatches on the action hash: it records link/path info
// into the host, fires a "gun_empty" effect when the host runs dry, and steps through the
// per-host-type {enter, update, msg, data} table. The sound bank is loaded once.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

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

// The plain query block handed to fn_800C5210 (no String member).
struct MsgQuery {
    int unk0;
    u32 hash;
    int unk8;
    int result;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
};

struct Actor;

// Per-host-type behaviour table (25 entries at 0x803C3980).
struct HostType {
    void (*enter)(Actor* actor);   // 0x00
    void (*update)(Actor* actor, int arg);  // 0x04
    void (*msg)(Actor* actor);     // 0x08
    u32 actions[14];               // 0x0C
    void* name;                    // 0x44
};

extern HostType gHostTypes[];

struct Actor {
    char pad0[0x38];
    int handle;           // 0x38
    char pad3C[0x768];
    int type;             // 0x7A4
    int nextType;         // 0x7A8
    char pad7AC[8];
    u32 flags;            // 0x7B4
    char pad7B8[0xD44];
    float speed;          // 0x14FC
    char pad1500[0x804];
    char buf[96];         // 0x1D04
};

extern "C" {
int fn_800C5210(int to, int msg, void* data);
int fn_800498D0(Actor* a);
void fn_80048BA8(Actor* a, void* buf, int mode);
void fn_8006D288(Actor* a, int state);
}

extern "C" int SendMsg(int to, int msg, void* data);                 // 0x800265D4
int Fn800F7DEC(int a, int b, int c, int d, void* e);      // 0x800F7DEC
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f); // 0x800F7F5C
int ActorPlayAnim(int actor, u32 anim, int arg);          // 0x800C5654
void* FxFind(const char* name);                           // 0x80170834
extern "C" void FxStart(void* fx, int a, int b);                     // 0x8016F390
int AssetFind(u32 table, u32 key);                        // 0x8007EB34

int gHostSounds[16];

// Out-parameter form (not a return value): with a returned value the two results outrank the actor
// in register allocation (they took r31/r30, retail keeps the actor in r31).
inline void Query(int* out, int target, int msg, u32 hash) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    *out = *(int*)&m.result;
}

// Message-handler style: the actor arrives as void* and is cast to a local (retail ranks it first).
int HostAction(int action, void* p) {
    Actor* actor = (Actor*)p;
    MsgQuery q0;
    q0.result = actor->handle;
    q0.hash = action;
    if (fn_800C5210(actor->handle, 0xc06, &q0) == 1) {
        return 0;
    }
    int r1;
    int r2;
    Query(&r1, actor->handle, 0x41E, 0x2DA8293C);
    Query(&r2, actor->handle, 0x41E, 0xDFF3097A);

    switch (action) {
    case 0x1B83833F:
        if (actor->type == 5) {
            int r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(actor), 0);
            if (r == 0xC0BECF7E || r == 0x11D2C025) {
                ActorPlayAnim(actor->handle, 0xA7D124DE, actor->handle);
                FxStart(FxFind("gun_empty"), 0, 0);
            }
            fn_8006D288(actor, actor->nextType);
        } else if (actor->type == 8) {
            int r = Fn800F7DEC(r1, r2, 0xF829DB93, fn_800498D0(actor), 0);
            if (r == 0xC0BECF7E || r == 0x11D2C025) {
                FxStart(FxFind("gun_empty"), 0, 0);
            }
            fn_8006D288(actor, actor->nextType);
        } else if (actor->type == 13) {
            actor->flags |= 0x20000;
        } else {
            fn_8006D288(actor, actor->nextType);
        }
        break;
    case 0x0020EE36:
        if (Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(actor), 0) == 0x04A3E183) {
            fn_80048BA8(actor, actor->buf, 1);
            Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        }
        break;
    case 0xF829DB93:
        fn_80048BA8(actor, actor->buf, 0);
        Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        break;
    case 0x9791AF51:
    case 0x96224FCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        fn_80048BA8(actor, actor->buf, 1);
        Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        break;
    case 0xE6B71BCC:
        fn_80048BA8(actor, actor->buf, 1);
        Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        break;
    case 0x968CBE3D:
        fn_80048BA8(actor, actor->buf, 1);
        Fn800F7F5C(r1, r2, action, fn_800498D0(actor), actor->buf, actor->speed);
        break;
    }
    return 0;
}

void* HostTypeData(Actor* actor) {
    return gHostTypes[actor->type].name;
}

void HostTypeMsg(Actor* actor) {
    if (gHostTypes[actor->type].msg) {
        gHostTypes[actor->type].msg(actor);
    }
}

void HostTypeUpdate(Actor* actor, int arg) {
    if (gHostTypes[actor->type].update) {
        gHostTypes[actor->type].update(actor, arg);
    }
}

void HostTypeEnter(Actor* actor) {
    if (gHostTypes[actor->type].enter) {
        gHostTypes[actor->type].enter(actor);
    }
}

void HostLoadSounds(void) {
    gHostSounds[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    gHostSounds[1] = AssetFind(0xF1754BF2, 0x0024805F);
    gHostSounds[2] = AssetFind(0xF1754BF2, 0x101844CC);
    gHostSounds[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    gHostSounds[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    gHostSounds[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    gHostSounds[6] = AssetFind(0xF1754BF2, 0xD9404234);
    gHostSounds[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    gHostSounds[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    gHostSounds[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    gHostSounds[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    gHostSounds[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    gHostSounds[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    gHostSounds[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    gHostSounds[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    gHostSounds[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

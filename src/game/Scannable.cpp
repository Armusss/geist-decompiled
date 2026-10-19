// Scannable.cpp: the "Scannable" actor type (0x8024E374-0x8024E5D0).
// Answers the ghost's scan: plays the "QUERY" animation and reports back to the scanner.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s, u32 h) {
    int i;

    if (s == 0) {
        return 0;
    }
    for (i = 0; s[i] != 0; i++) {
        h = h * 31 + (u8)toupper(s[i]);
    }
    return h % 0xFFFFFFFF;
}

struct Actor;

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

struct ActorType {
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void (*update)(Actor* actor, float dt);
    int (*init)(Actor* actor);
    void* unk14;
    void (*destroy)(Actor* actor);
    void* unk1C[5];
};

struct Actor {
    char pad0[0x38];
    int handle;
    char pad3C[0x4C];
    ActorType* type;
    int unk8C;
};

struct ScanMsg {
    Actor* actor;
    Actor* scanner;
    int anim2;
    u32 anim;
    float unk10;
    float unk14;
    int unk18;
    Actor* target;
};

struct AnimMsg {
    AnimMsg() : unk4(0), sound(-1) {}
    int unk0;
    int unk4;
    u32 anim;
    int sound;
};

struct ScanReply {
    int unk0;
    int result;
    Actor* actor;
    u32 anim;
};

int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
int ActorInitBase(Actor* a);                              // 0x800E6444
int ActorIsActive(Actor* a);                              // 0x80022D80
int ActorPlayAnim(int handle, u32 anim, int arg);         // 0x800C5654
extern "C" int SendMsg(int target, int msg, void* data);             // 0x800265D4
int AssetFind(u32 bank, u32 name);                        // 0x8007EB34

int ScannableMsg(Actor* a, int msg, void* data, int arg);
int ScannableInit(Actor* a);

ActorType gScannableType = { 0, "Scannable", ScannableMsg, 0, ScannableInit };

int ScannableMsg(Actor* a, int msg, void* data, int arg) {
    ScanMsg* m = (ScanMsg*)data;

    switch (msg) {
    case 0x40A: {
        if (ActorPlayAnim((int)a, StrHash("QUERY", m->anim2), (int)m->scanner)) return 1;
        AnimMsg am;
        m->anim = 0x0026BE7D;
        am.anim = m->anim;
        am.sound = AssetFind(0xC0A77187, 0x0026BE7D);
        SendMsg((int)m->scanner, 0x40B, &am);
        return 1;
    }
    case 0x409: {
        Actor* s = m->scanner;
        int r;
        if (ActorPlayAnim((int)a, m->anim, m->anim2)) return 1;
        r = ActorPlayAnim((int)a, 0xDC35206A, s->handle);
        if (s->handle != -1) {
            ScanReply reply;
            reply.result = r;
            reply.actor = a;
            reply.anim = 0xAF699ED0;
            SendMsg(s->handle, 0xC14, &reply);
        }
        return 1;
    }
    case 0x400: {
        Actor* t = m->actor;
        m->scanner = (Actor*)t->unk8C;
        m->anim = 1;
        m->unk10 = ActorIsActive(t) ? 1.0f : 0.0f;
        m->unk14 = 0.0f;
        m->target = t;
        m->anim2 = 0;
        m->unk18 = 3;
        return 1;
    }
    default:
        return ActorDefaultMsg(a, msg, data, arg);
    }
}

int ScannableInit(Actor* a) {
    if (!ActorInitBase(a)) {
        a->type = 0;
        return 0;
    }
    return 1;
}

// CollectionHost.cpp: the "Collection Host Unlock" actor type (0x802E2F08-0x802E32D0).
// Possessing it plays a "QUERY" animation, unlocks a host power ("Host_Power" effect) and shows a
// timed HUD message with the unlocked host's name.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

extern "C" {
int sprintf(char* s, const char* format, ...);
unsigned long strlen(const char* s);
}

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
        h = h * 31 + (unsigned char)toupper(s[i]);
    }
    return h % 0xFFFFFFFF;
}

struct ActorInfo {
    char pad0[0x94];
    int unlockId;
};

struct Actor {
    char pad0[0x38];
    u32 flags;
    char pad3C[0x48];
    ActorInfo* info;
    char pad88[4];
    int id;
    char pad90[0x14];
    int unkA4;
    char padA8[4];
    u32 flagsAC;
    char padB0[0x7C];
    float scanRange;
};

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

struct ActorType {
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void (*update)(Actor* actor, float dt);
    int (*init)(Actor* actor);
    void* unk14[7];
};

struct HostMsg {
    Actor* target;
    Actor* host;
    u32 anim;
    u32 key;
};

struct AnimMsg {
    int unk0;
    int unk4;
    u32 key;
    int asset;
    int unk10[4];
};

struct UnlockMsg {
    int unk0;
    int unk4;
    u32 flags;
    u32 key;
};

struct ShowMsg {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
};

struct InitMsg {
    int unk0;
    int id;
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
};

int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
void ActorUpdate(Actor* a, float dt);                     // 0x800E48A0
int ActorInitBase(Actor* a);                              // 0x800E6444
int SendMsg(int to, int msg, void* data);                 // 0x800265D4
Actor* GetPlayer(void);                                   // 0x80049948
int ActorPlayAnim(int actor, u32 anim, int arg);          // 0x800C5654
int AssetFind(u32 table, u32 key);                        // 0x8007EB34
const char* StrFindKey(u32 key, u32* out);                // 0x80086234
int PlayerInHost(Actor* player);                          // 0x80049750
int PlayerHud(Actor* player);                             // 0x80049928
void HudMessage(int hud, const char* text, float a, float b, float c); // 0x800FEBD8
void SoundPlay(u32 id);                                   // 0x8018FA30
void* FxFind(const char* name);                           // 0x80170834
void FxStart(void* fx, int a, int b);                     // 0x8016F390
void GameEvent(int event, int arg);                       // 0x8007EA84
void Fn8028138C(void);                                    // 0x8028138C
void TimerAdd(int id, float (*func)(void* user), void* user, float delay); // 0x8007DC5C
const char* HostUnlockName(int which);                    // 0x8014FDA8
int HostUnlockCount(void);                                // 0x8014FD54

float gHostScanNear = 0.2f;
float gHostScanFar = 0.8f;

int CollectionHostMsg(Actor* a, int msg, void* data, int arg);
int CollectionHostInit(Actor* a);
float CollectionHostShow(void* user);

ActorType gCollectionHostType = { 0, "Collection Host Unlock", CollectionHostMsg, ActorUpdate, CollectionHostInit };

int CollectionHostMsg(Actor* a, int msg, void* data, int arg) {
    AnimMsg am;
    ShowMsg sm;
    UnlockMsg um;
    HostMsg* m;
    Actor* host;
    Actor* t;

    switch (msg) {
    case 0: {
        Actor* self = *(Actor**)data;
        if (PlayerInHost(GetPlayer())) {
            self->scanRange = gHostScanFar;
        } else {
            self->scanRange = gHostScanNear;
        }
        break;
    }
    case 0x40A:
        m = (HostMsg*)data;
        if (ActorPlayAnim((int)a, StrHash("QUERY", m->anim), (int)m->host)) {
            return 0;
        }
        if (m->anim == 0x7C44EC3D) {
            am.unk4 = 0;
            am.asset = -1;
            m->key = 0xF21581DE;
            am.key = m->key;
            am.asset = AssetFind(0xC0A77187, 0x8CA7FE7C);
            SendMsg((int)m->host, 0x40B, &am);
        }
        return 1;
    case 0x409:
        m = (HostMsg*)data;
        host = m->host;
        t = m->target;
        if ((int)(host->flags >> 24) == 2) {
            SoundPlay(0x98EC6A3A);
            um.flags = host->flags;
            um.key = 0x1C19D386;
            um.unk4 = 1;
            SendMsg(0x20000000, 0xC14, &um);
            ActorPlayAnim(t->id, 0xB94AE51B, t->id);
        }
        FxStart(FxFind("Host_Power"), 0, 0);
        GameEvent(0x110A6, t->flags);
        Fn8028138C();
        TimerAdd(0, CollectionHostShow, t, 500.0f);
        sm.unk4 = 0;
        SendMsg(t->id, 0xC00, &sm);
        return 1;
    }
    return ActorDefaultMsg(a, msg, data, arg);
}

int CollectionHostInit(Actor* a) {
    int id;

    if (!ActorInitBase(a)) {
        return 0;
    }

    a->flagsAC |= 0x20000000;
    a->unkA4 = 0;
    id = a->info->unlockId;
    if (id != -1) {
        InitMsg m;
        m.id = id;
        m.unk8 = 0;
        m.unkC = 0;
        m.unk10 = 0x1000;
        m.unk14 = 0;
        m.unk18 = 0;
        SendMsg(a->id, 0xC04, &m);
    }
    return 1;
}

float CollectionHostShow(void* user) {
    u32 tmp;
    char text[92];
    const char* name;

    name = HostUnlockName(1);
    if (name != 0) {
        HudMessage(PlayerHud(GetPlayer()), name, 1.0f, 1.0f, 1.0f);
    } else {
        sprintf(text, "%s %i %s", StrFindKey(0x85129D98, &tmp), HostUnlockCount(), StrFindKey(0xFFD037CB, &tmp));
        strlen(text);
        HudMessage(PlayerHud(GetPlayer()), text, 1.0f, 1.0f, 1.0f);
    }
    return 0.0f;
}

// CollectionGhost.cpp: the "Collection Ghost Health" actor type (0x802E32D0-0x802E3674).
// A near copy of CollectionHost.cpp: possessing it raises the ghost's health ("Ghost_Power" effect) and
// shows a timed HUD message.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

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
    char pad130[0x13D0];
    float health;
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
extern "C" int SendMsg(int to, int msg, void* data);                 // 0x800265D4
Actor* GetPlayer(void);                                   // 0x80049948
int ActorPlayAnim(int actor, u32 anim, int arg);          // 0x800C5654
int AssetFind(u32 table, u32 key);                        // 0x8007EB34
const char* StrGetIndex(int index, u32* len);             // 0x800862F0
int PlayerInHost(Actor* player);                          // 0x80049750
int PlayerHud(Actor* player);                             // 0x80049928
void HudMessageN(int hud, const char* text, u32 len, float a, float b, float c, float d); // 0x800FE884
void SoundPlay(u32 id);                                   // 0x8018FA30
void* FxFind(const char* name);                           // 0x80170834
extern "C" void FxStart(void* fx, int a, int b);                     // 0x8016F390
int GameEvent(int event, int arg);                        // 0x8007EA84
void Fn80281424(void);                                    // 0x80281424
float Fn802811A4(void);                                   // 0x802811A4
void TimerAdd(int id, float (*func)(void* user), void* user, float delay); // 0x8007DC5C
int Fn800DF814(u32 key, int value);                       // 0x800DF814
int Fn800DF3B8(u32 key, int value, u32 key2);             // 0x800DF3B8

float gGhostScanNear = 0.3f;
float gGhostScanFar = 1.0f;

int CollectionGhostMsg(Actor* a, int msg, void* data, int arg);
int CollectionGhostInit(Actor* a);
float CollectionGhostShow(void* user);

ActorType gCollectionGhostType = { 0, "Collection Ghost Health", CollectionGhostMsg, ActorUpdate, CollectionGhostInit };

int CollectionGhostMsg(Actor* a, int msg, void* data, int arg) {
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
            self->scanRange = gGhostScanNear;
        } else {
            self->scanRange = gGhostScanFar;
        }
        break;
    }
    case 0x40A:
        m = (HostMsg*)data;
        if (ActorPlayAnim((int)a, StrHash("QUERY", m->anim), (int)m->host)) {
            return 0;
        }
        if (m->anim == 0x3CA34BD6) {
            am.unk4 = 0;
            am.asset = -1;
            m->key = 0x3D831565;
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
            SoundPlay(0x20B1076F);
            um.flags = host->flags;
            um.key = 0x1C19D386;
            um.unk4 = 1;
            SendMsg(0x20000000, 0xC14, &um);
        }
        FxStart(FxFind("Ghost_Power"), 0, 0);
        GameEvent(0x110A6, t->flags);
        Fn80281424();
        TimerAdd(0, CollectionGhostShow, t, 500.0f);
        sm.unk4 = 0;
        SendMsg(t->id, 0xC00, &sm);
        return 1;
    }
    return ActorDefaultMsg(a, msg, data, arg);
}

int CollectionGhostInit(Actor* a) {
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

float CollectionGhostShow(void* user) {
    Actor* a;
    int index;
    u32 len;
    Actor* player;

    a = (Actor*)user;
    player = GetPlayer();
    index = Fn800DF3B8(0x258124CE, Fn800DF814(0x258124CE, GameEvent(0x110A6, a->flags)), 0x8829DC51);
    len = 0;
    HudMessageN(PlayerHud(player), StrGetIndex(index, &len), len, 1.0f, 1.0f, 1.0f, 15.0f);
    player->health = 6800.0f * (1.0003f * Fn802811A4());
    return 0.0f;
}

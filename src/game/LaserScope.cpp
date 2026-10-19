// LaserScope.cpp: the "Possessable Laser Scope" actor type (0x8025951C-0x80259AB0).
// A possessable sniper scope: while possessed, the player cycles its zoom (50/30/15) and the view
// zooms smoothly towards it. It reserves and frees a target through "new"/"free" queries.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    int i;
    u32 h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    for (i = 0; s[i] != 0; i++) {
        h = h * 31 + (u8)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
}

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

struct LaserData {
    void* owner;
    char pad4[8];
    unsigned long flags;
    char pad10[0x20];
    int state;
    int target;
    float unk38;
    float zoomTarget;
    float zoom;
};

struct Actor {
    char pad0[0x38];
    int handle;
    char pad3C[0x68];
    LaserData* data;
    char padA8[4];
    unsigned long flags;
};

struct Player {
    char pad0[0x184];
    int camera;
    char pad188[0x1C4];
    int zoomPressed;
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

struct LinkMsg {
    int unk0;
    int handle;
    int unk8;
    int unkC;
    char info[0x30];
};

extern "C" int SendMsg(int target, int msg, void* data);            // 0x800265D4
void* VAlloc(int size);
extern "C" void* memset(void* dst, int val, unsigned long n);
void PossessableDestroy(Actor* a);                       // 0x8020D4E0
int PossessableInit(Actor* a);                           // 0x8020D554
void PossessableUpdate(Actor* a, float dt);              // 0x8020D6C0
int PossessableMsg(Actor* a, int msg, void* data, int arg); // 0x8020D738
Player* PlayerGetPossessor(int handle);                  // 0x80049888 (player whose handle matches)
void CameraSetFov(int camera, float fov);                // 0x800F95A0
void LinkInfoInit(void* info);                           // 0x800A0328

int LaserMsg(Actor* a, int msg, void* data, int arg);
void LaserUpdate(Actor* a, float dt);
int LaserInit(Actor* a);
void LaserDestroy(Actor* a);

ActorType gLaserScopeType = { 0, "Possessable Laser Scope", LaserMsg, LaserUpdate, LaserInit, 0, LaserDestroy };

void LaserDestroy(Actor* a) {
    LaserData* d = a->data;

    if (d->target) {
        QueryMsg m;
        m.hash = StrHash("free");
        SendMsg(d->target, 0x41D, &m);
    }
    PossessableDestroy(a);
}

void LaserUpdate(Actor* a, float dt) {
    LaserData* d = a->data;

    switch (d->state) {
    case 0:
        break;
    case 1:
        if ((int)(d->flags >> 24) == 2) {
            Player* p = PlayerGetPossessor(d->flags);
            if (p && d->owner) {
                if (p->zoomPressed == 1) {
                    if (50.0f == d->zoomTarget) {
                        d->zoomTarget = 30.0f;
                    } else if (30.0f == d->zoomTarget) {
                        d->zoomTarget = 15.0f;
                    } else if (15.0f == d->zoomTarget) {
                        d->zoomTarget = 50.0f;
                    }
                }
                {
                    float step = 0.001f * dt;
                    float zoom = d->zoom;
                    float target = d->zoomTarget;
                    if (zoom < target) {
                        zoom += 35.0f * step;
                        if (zoom > target) zoom = target;
                    } else if (zoom > target) {
                        zoom -= 2.0f * (35.0f * step);
                        if (zoom < target) zoom = target;
                    }
                    d->zoom = zoom;
                    CameraSetFov(p->camera, zoom);
                }
            }
        }
    }
    PossessableUpdate(a, dt);
}

int LaserInit(Actor* a) {
    QueryMsg m;
    LaserData* d;

    if (a->data == 0) {
        d = (LaserData*)VAlloc(sizeof(LaserData));
        a->data = d;
    } else {
        d = a->data;
    }
    memset(d, 0, sizeof(LaserData));

    if (!PossessableInit(a)) {
        a->flags |= 0x01000000;
        return 0;
    }

    m.hash = StrHash("new");
    SendMsg(0x60000000, 0x41E, &m);
    d->target = *(int*)&m.result;
    if (d->target) {
        LinkMsg lm;
        lm.handle = a->handle;
        lm.unk8 = 0;
        lm.unkC = 1;
        LinkInfoInit(lm.info);
        SendMsg(d->target, 0x415, &lm);
    }
    d->zoomTarget = 50.0f;
    d->zoom = 50.0f;
    return 1;
}

static inline int LaserOnFree(void* data) {
    LaserData* d = (*(Actor**)data)->data;

    int ret = 0;

    switch (((int*)data)[1]) {
    case 0x044DC6AB:
        ((int*)data)[1] = 0x7A281983;
        SendMsg(d->target, 0x41D, data);
        ret = 1;
    }
    return ret;
}

static inline int LaserOnTrigger(void* data) {
    Actor* actor = *(Actor**)data;
    LaserData* d = actor->data;

    switch (((int*)data)[3]) {
    case 0x672F35C2: {
        LinkMsg lm;
        d->state = 1;
        d->unk38 = 0.0f;
        lm.handle = ((int*)data)[2];
        lm.unk8 = 0;
        lm.unkC = 1;
        LinkInfoInit(lm.info);
        SendMsg(d->target, 0x415, &lm);
        break;
    }
    case 0x39CB0CB0: {
        LinkMsg lm;
        d->state = 0;
        d->unk38 = 0.0f;
        d->zoomTarget = 50.0f;
        d->zoom = 50.0f;
        lm.handle = actor->handle;
        lm.unk8 = 0;
        lm.unkC = 1;
        LinkInfoInit(lm.info);
        SendMsg(d->target, 0x415, &lm);
        break;
    }
    }
    return 0;
}

int LaserMsg(Actor* a, int msg, void* data, int arg) {
    int ret = 0;

    switch (msg) {
    case 0x41D:
        ret = LaserOnFree(data);
        break;
    case 0xC14:
        ret = LaserOnTrigger(data);
        break;
    }

    if (ret) return ret;
    return PossessableMsg(a, msg, data, arg);
}

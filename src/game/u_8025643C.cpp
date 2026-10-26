// u_8025643C.cpp: the gun-with-grenade host states and the "GUNWITHGRENADE" effect type (Update/Msg written,
// the effect half 0x8025808C-0x80259448 matches). Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_80019A04();
int fn_800F8AF4(int id);
void fn_80175D90(void* p);
void fn_80175EE4(void* p);
}

void* VAlloc(int size);
void Fn800F840C(int emitter, int a, int b);
extern int lbl_804D54D0;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    if (s == 0) {
        return 0;
    } else {
        u32 h = 0;
        int i;
        for (i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
        return h;
    }
}


// ---------------------------------------------------------------------------------------------------
// 0x8025643C-0x8025808C: the gun-with-grenade host states (aim, fire, reload, grenade throw) and the
// per-host-type table callbacks. Its own string pool ("gun_empty"), host table (0x803BF068) and
// .sdata2 constants (0x804D9E98-0x804D9EBC) suggest it is a file of its own.

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

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

struct Actor;

// Per-host-type behaviour table (25 entries at 0x803BF068).
struct HostType {
    void (*enter)(Actor* actor);
    void (*update)(Actor* actor, int arg, float dt);
    void (*msg)(Actor* actor);
    u32 actions[14];
    void* name;
};

extern HostType lbl_803BF068[];

// The host's per-instance animation block, reached through the actor's host array.
struct HostData {
    char pad00[0x14];
    int set;              // 0x14
    int aim;              // 0x18
    int walk;             // 0x1C
    int unk20;
    int run;              // 0x24
};

typedef int (*HostMsgFunc)(int msg, void* data);

struct Actor {
    char pad0[0x38];
    int handle;           // 0x38
    char pad3C[0x7C - 0x3C];
    int model;            // 0x7C
    char pad80[0x8C - 0x80];
    HostMsgFunc msgHandler;  // 0x8C
    char pad90[0x98 - 0x90];
    int anim;             // 0x98
    char pad9C[0xA4 - 0x9C];
    int animMode;         // 0xA4
    char padA8[0xE4 - 0xA8];
    HostMsgFunc msgHandler2;  // 0xE4
    char padE8[0xF0 - 0xE8];
    int anim2;            // 0xF0
    char padF4[0xFC - 0xF4];
    int animMode2;        // 0xFC
    char pad100[0x2F4 - 0x100];
    char timer[0x314 - 0x2F4];  // 0x2F4
    int ammo;             // 0x314
    int grenades;         // 0x318
    char pad31C[4];
    int pressed;          // 0x320
    u32 pressed2;         // 0x324
    int unk328;           // 0x328
    char pad32C[0x38C - 0x32C];
    float move[4];        // 0x38C
    char pad39C[0x588 - 0x39C];
    int mode;             // 0x588
    char pad58C[0x5E8 - 0x58C];
    float hold;           // 0x5E8
    char pad5EC[0x638 - 0x5EC];
    int unk638;           // 0x638
    char pad63C[0x678 - 0x63C];
    u32 flags678;         // 0x678
    char pad67C[0x73C - 0x67C];
    int weapon;           // 0x73C
    char pad740[0x7A4 - 0x740];
    int type;             // 0x7A4
    int nextType;         // 0x7A8
    char pad7AC[8];
    u32 flags;            // 0x7B4
    char pad7B8[0x7D0 - 0x7B8];
    int unk7D0;           // 0x7D0
    int variant;          // 0x7D4
    float unk7D8;         // 0x7D8
    char pad7DC[0x800 - 0x7DC];
    float aim;            // 0x800
    char pad804[0x14FC - 0x804];
    float speed;          // 0x14FC
    char pad1500[0x1D04 - 0x1500];
    char buf[0x50];       // 0x1D04
    float bufAim;         // 0x1D54
    char pad1D58[0x26C4 - 0x1D58];
    HostData* hosts[8];   // 0x26C4
};

extern "C" {
int fn_800498D0(Actor* a);
void fn_8006D184(Actor* a, int state);
void fn_8006D288(Actor* a, int state);
int fn_800A1BC8(int set, int anim, u32 action, int a, int b);
int fn_802A1D04(void);
float fn_800699B8(Actor* a);
void fn_800699A8(void* timer);
void fn_8008E824(int model, int a);
int fn_800F7994(int a, u32 b);
int fn_800C5210(int to, int msg, void* data);
void fn_80048BA8(Actor* a, void* buf, int mode);
int rand(void);
void fn_800769A0(Actor* a, float dt);
void fn_80072058(Actor* a, float dt);
void fn_8006D17C(int* type, int state);
void fn_8006D76C(Actor* a);
}

int ActorPlayAnim(int actor, u32 anim, int arg);
void DashUpdate(Actor* a);
int PlayerInHost(Actor* a);
extern "C" int SendMsg(int to, int msg, void* data);
int Fn800F7DEC(int a, int b, int c, int d, void* e);
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f);
void* FxFind(const char* name);
extern "C" int FxStart(void* fx, int a, int b);
extern int lbl_804D3620;

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

inline void Query(int* out, int target, int msg, u32 hash) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    *out = *(int*)&m.result;
}

inline void QueryValue(int target, u32 hash, int value) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    SendMsg(target, 0x41D, &m);
}

int GunHostCheckGrenade(Actor* a);
int GunHostCheckGun(Actor* a);
int GunHostCheckReload(Actor* a, float dt);
int GunHostMsg(int msg, void* data);

static inline float fabsf2(float x) { return __fabsf(x); }
inline float Max(float a, float b) { return a > b ? a : b; }
inline float MaxLt(float a, float b) { return a < b ? b : a; }
int AssetFind(u32 table, u32 key);

extern int lbl_804A2930[16];  // the host's sounds

void GunHostAimUpdate(Actor* a, float dt) {
    int r1;
    int r2;
    float m;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    a->unk7D0 = 1;
    m = __fabsf(a->move[0]) > __fabsf(a->move[2]) ? __fabsf(a->move[0]) : __fabsf(a->move[2]);
    if (m != 0.0f) {
        fn_8008E824(a->model, 0);
    }
    if (a->unk7D8 == 0.0f) {
        fn_8006D184(a, 0);
    } else if (a->flags678 & 0x80) {
        a->flags678 &= ~0x80;
        fn_8006D288(a, 3);
    } else {
        fn_8006D17C(&a->type, 3);
        DashUpdate(a);
        if (!GunHostCheckGrenade(a) && !GunHostCheckGun(a)) {
            GunHostCheckReload(a, dt);
        }
    }
}

void GunHostAimEnter(Actor* a) {
    int weapon;
    int r1;
    int r2;
    int host;
    int empty;
    int r;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    host = PlayerInHost(a);
    weapon = a->weapon;
    r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(a), 0);
    empty = 0;
    if (r == 0xC0BECF7E || r == 0x11D2C025) {
        empty = 1;
    }
    a->anim = -1;
    while (a->anim == -1) {
        a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[3].actions[weapon], empty, 0);
        a->msgHandler = GunHostMsg;
        a->animMode = 1;
        if (a->anim == -1 && empty) {
            empty = 0;
        }
    }
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[3].actions[weapon], 0, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[3].actions[weapon], 0, 0);
    }
    a->animMode2 = 1;
}

void GunHostSignal(Actor* a) {
    ActorPlayAnim(0x2000000, 0x8B3A27A3, fn_800498D0(a));
}

void GunHostIdleUpdate(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    DashUpdate(a);
    fn_8006D17C(&a->type, 3);
}

void GunHostFireEnter(Actor* a) {
    int r2;
    int host;
    int weapon;

    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    host = PlayerInHost(a);
    weapon = a->weapon;
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[2].actions[weapon], 0, 0);
    a->animMode = 16;
    a->msgHandler = GunHostMsg;
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[2].actions[weapon], 0, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[2].actions[weapon], 0, 0);
    }
    a->animMode2 = 16;
    lbl_804D3620 = fn_800F7994(r2, 0xAFEF562B);
    FxStart((void*)lbl_804D3620, 0, 0);
}

void GunHostFireUpdate(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    fn_800769A0(a, dt);
    fn_80072058(a, dt);
    if (a->flags678 & 0x80) {
        a->flags678 &= ~0x80;
        fn_8006D288(a, 1);
    }
    fn_8006D17C(&a->type, 1);
    a->unk7D0 = 0;
    DashUpdate(a);
    if (!GunHostCheckGun(a) && !GunHostCheckGrenade(a) && (a->pressed & 1)) {
        if (a->unk7D0 == 0) {
            fn_8006D184(a, 2);
        } else {
            fn_8006D184(a, 9);
        }
    }
}

void GunHostIdleEnter(Actor* a) {
    int host = PlayerInHost(a);
    int weapon = a->weapon;

    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[1].actions[weapon], 0, 0);
    a->animMode = 1;
    a->msgHandler = 0;
    if (a->flags & 0x80000) {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->run, lbl_803BF068[1].actions[weapon], 0, 0);
    } else {
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->walk, lbl_803BF068[1].actions[weapon], 0, 0);
    }
    a->msgHandler2 = GunHostMsg;
    a->animMode2 = 1;
    a->unk7D0 = 0;
}

void GunHostStop(Actor* a) {
    fn_8006D76C(a);
}

void GunHostWalkUpdate(Actor* a, float dt) {
    a->unk638 = 1;
    a->flags &= ~1;
    a->unk7D0 = 1;
    fn_8006D17C(&a->type, 3);
    DashUpdate(a);
    if (!GunHostCheckGun(a) && !GunHostCheckGrenade(a)) {
        GunHostCheckReload(a, dt);
    }
    if (a->move[2] != 0.0f || a->move[0] != 0.0f || a->move[1] != 0.0f || a->move[3] != 0.0f || a->unk328 != 0) {
        fn_8006D288(a, 3);
    }
}

void GunHostThrowEnter(Actor* a) {
    int r1;
    int r2;
    int host;
    int r;
    int walk;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(a), 0);
    if (r == 0xC0BECF7E || r == 0x11D2C025) {
        a->variant = 2;
    } else if (rand() / 32767.0f >= 0.5f) {
        a->variant = 0;
    } else {
        a->variant = 1;
    }
    host = PlayerInHost(a);
    a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[0].actions[0], a->variant, 0);
    if (a->anim == -1) {
        a->variant = 0;
        a->anim = fn_800A1BC8(a->hosts[host]->set, a->hosts[host]->aim, lbl_803BF068[0].actions[0], a->variant, 0);
    }
    a->msgHandler = GunHostMsg;
    a->animMode = 1;
    {
        HostData* h = a->hosts[host];
        int set = h->set;

        if (a->flags & 0x80000) {
            walk = h->run;
        } else {
            walk = h->walk;
        }
        a->anim2 = fn_800A1BC8(set, walk, lbl_803BF068[0].actions[0], a->variant, 0);
    }
    if (a->anim2 == -1) {
        a->variant = 0;
        a->anim2 = fn_800A1BC8(a->hosts[host]->set, walk, lbl_803BF068[0].actions[0], a->variant, 0);
    }
    a->msgHandler2 = GunHostMsg;
    a->animMode2 = 1;
}

int GunHostCheckReload(Actor* a, float dt) {
    int r1;
    int r2;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    if (a->mode == 5 || a->mode == 4) {
        if (a->pressed & 1) {
            fn_8006D76C(a);
            if (a->unk7D0 == 0) {
                fn_8006D184(a, 2);
            } else {
                fn_8006D184(a, 9);
            }
            return 1;
        }
        if (a->pressed2 & 1) {
            fn_8006D76C(a);
            if (Fn800F7DEC(r1, r2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 10);
                return 1;
            }
            if (Fn800F7DEC(r1, r2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 11);
                return 1;
            }
        }
    } else if (a->pressed != 0) {
        fn_8006D76C(a);
        a->hold += dt;
        if (fn_800699B8(a) == 1.0f) {
            if (a->unk7D0 == 0) {
                fn_8006D184(a, 2);
            } else {
                fn_8006D184(a, 9);
            }
            fn_800699A8(a->timer);
            return 1;
        }
    } else {
        float t = fn_800699B8(a);
        if (t > -1.0f && t < 0.0f) {
            if (Fn800F7DEC(r1, r2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 10);
                fn_800699A8(a->timer);
                return 1;
            }
            if (Fn800F7DEC(r1, r2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 11);
                fn_800699A8(a->timer);
                return 1;
            }
        }
        fn_800699A8(a->timer);
    }
    return 0;
}

int GunHostCheckGrenade(Actor* a) {
    int r1;
    int r2;
    int r;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    if (a->grenades >= 1) {
        if (a->type == 1) {
            fn_8006D184(a, 2);
            return 1;
        }
        fn_8006D76C(a);
        r = Fn800F7DEC(r1, r2, 0xF829DB93, fn_800498D0(a), 0);
        switch (r) {
        case 0x04A3E183:
            if (a->type != 8) {
                fn_8006D184(a, 8);
                return 1;
            }
            break;
        case 0xC0BECF7E:
            if (Fn800F7DEC(r1, r2, 0x968CBE3D, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 11);
                return 1;
            }
            break;
        case 0x11D2C025:
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                QueryValue(a->handle, 0x7FFCFD4F, 0x91A0CEF7);
            }
            if (a->type == 8) {
                fn_8006D184(a, 3);
            }
            break;
        case 0x3DA260F7:
            if (a->type == 8) {
                fn_8006D184(a, 3);
            }
            break;
        }
    } else if (a->type == 8) {
        fn_8006D184(a, 3);
    }
    return 0;
}

int GunHostCheckGun(Actor* a) {
    int r1;
    int r2;
    int r;

    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    if (a->ammo >= 1) {
        if (a->type == 1) {
            fn_8006D184(a, 2);
            return 1;
        }
        fn_8006D76C(a);
        r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(a), 0);
        switch (r) {
        case 0x04A3E183:
            fn_8006D184(a, 5);
            fn_8006D17C(&a->type, 5);
            return 1;
        case 0xC0BECF7E:
            if (Fn800F7DEC(r1, r2, 0xE6B71BCC, fn_800498D0(a), 0) == 0x04A3E183) {
                fn_8006D184(a, 10);
                return 1;
            }
            break;
        case 0x11D2C025:
            if (fn_802A1D04() == 2 || fn_802A1D04() == 1) {
                QueryValue(a->handle, 0x7FFCFD4F, 0x91A0CEF7);
            }
            if (a->type == 5) {
                fn_8006D184(a, 3);
            }
            break;
        case 0x3DA260F7:
            if (a->type == 5) {
                fn_8006D184(a, 3);
            }
            break;
        }
    } else if (a->type == 5) {
        fn_8006D184(a, 3);
    }
    return 0;
}

// Animation message handler of the gun states: passes fire/reload actions on to the host's weapon.
int GunHostMsg(int msg, void* data) {
    Actor* a = (Actor*)data;
    MsgQuery q0;
    int r1;
    int r2;
    int r;

    q0.result = a->handle;
    q0.hash = msg;
    if (fn_800C5210(a->handle, 0xC06, &q0) == 1) {
        return 0;
    }
    Query(&r1, a->handle, 0x41E, 0x2DA8293C);
    Query(&r2, a->handle, 0x41E, 0xDFF3097A);
    switch (msg) {
    case 0x1B83833F:
        if (a->type == 5) {
            r = Fn800F7DEC(r1, r2, 0x0020EE36, fn_800498D0(a), 0);
            if (r != 0x04A3E183) {
                if (r == 0xC0BECF7E || r == 0x11D2C025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                fn_8006D184(a, 3);
            } else {
                fn_8006D288(a, a->nextType);
            }
        } else if (a->type == 8) {
            r = Fn800F7DEC(r1, r2, 0xF829DB93, fn_800498D0(a), 0);
            if (r != 0x04A3E183) {
                if (r == 0xC0BECF7E || r == 0x11D2C025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                fn_8006D184(a, 3);
            } else {
                fn_8006D288(a, a->nextType);
            }
        } else if (a->type == 13) {
            a->flags |= 0x20000;
        } else {
            fn_8006D288(a, a->nextType);
        }
        break;
    case 0x0020EE36:
        fn_80048BA8(a, a->buf, 1);
        a->bufAim = a->aim;
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        break;
    case 0xF829DB93:
        fn_80048BA8(a, a->buf, 0);
        a->bufAim = a->aim;
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        return 1;
    case 0x9791AF51:
    case 0x96224FCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        break;
    case 0xE6B71BCC:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        break;
    case 0x968CBE3D:
        fn_80048BA8(a, a->buf, 1);
        Fn800F7F5C(r1, r2, msg, fn_800498D0(a), a->buf, a->speed);
        break;
    }
    return 0;
}


void* GunHostTypeData(Actor* actor) {
    return lbl_803BF068[actor->type].name;
}

void GunHostTypeMsg(Actor* actor) {
    if (lbl_803BF068[actor->type].msg) {
        lbl_803BF068[actor->type].msg(actor);
    }
}

// Raises the aim while in the aiming state (type 5), lowers it otherwise.
void GunHostTypeUpdate(Actor* actor, int arg, float dt) {
    if (actor->type == 5) {
        if (actor->aim < 1.0f) {
            actor->aim = 0.0005f * dt + actor->aim;
        }
        actor->aim = actor->aim < 1.0f ? actor->aim : 1.0f;
    } else {
        if (actor->aim > 0.0f) {
            actor->aim = -(0.001f * dt - actor->aim);
        }
        actor->aim = actor->aim > 0.0f ? actor->aim : 0.0f;
    }
    if (lbl_803BF068[actor->type].update) {
        lbl_803BF068[actor->type].update(actor, arg, dt);
    }
}

void GunHostTypeEnter(Actor* actor) {
    if (lbl_803BF068[actor->type].enter) {
        lbl_803BF068[actor->type].enter(actor);
    }
}

void GunHostLoadSounds(void) {
    lbl_804A2930[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_804A2930[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_804A2930[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_804A2930[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_804A2930[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_804A2930[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_804A2930[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_804A2930[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_804A2930[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_804A2930[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_804A2930[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_804A2930[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_804A2930[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_804A2930[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_804A2930[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_804A2930[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

// ---------------------------------------------------------------------------------------------------
// 0x8025808C-0x80259448: the "GUNWITHGRENADE" effect type.

struct EffectType {
    const char* name;
    u32 hash;
    int unk8[3];
    void* (*create)(int a, int id);
    void* destroy;
    void* msg;
    void* query;
    void* update;
    void* reset;
    int unk2C;
};

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}
struct Vec3 {
    float x, y, z;
};

struct Mtx34;
struct Player;

struct GunShot {
    u8 pad00[0x30];
    float x, y, z;
    Vec3 dir;
    u8 pad48[0x50 - 0x48];
    float spread;
    float scale;
    u8 pad58[0x60 - 0x58];
};

struct GunWithGrenadeDef {
    int unk00;
    int unk04;  // gun ammo id
    int unk08;  // gun ammo max
    float speed;
    int unk10;
    int unk14;
    int charge;
    int drain;
    int maxCharge;
    int spread;
    int unk28;
    int unk2C;
    void* fx;
    u8 pad34[0x3C - 0x34];
    void* hitFx;
    u8 pad40[0x4C - 0x40];
    float recoil;
    float recoilRate;
    float spreadMinA;
    float spreadMaxA;
    float spreadMinB;
    float spreadMaxB;
    int unk64;  // grenade ammo id
    int unk68;  // grenade ammo max
    float speed2;
    float unk70;
    u8 pad74[0x90 - 0x74];
    void* fx2;
    void* hitFx2;
    int model;
    u8 pad9C[0xBC - 0x9C];
    int hud;
};

struct GunWithGrenade {
    int unk00;  // state: 1 firing, 2 throwing
    float unk04;  // timer
    float unk08;  // rate
    float unk0C;  // charge
    float unk10;  // recoil
    float unk14;  // recoil timer
    int emitter;
    GunWithGrenadeDef* def;
    int unk20;
    int arg;
    int pad28;
    GunShot shot;
    int unk8C;
    int unk90;  // fx
    int unk94;
    u8 pad98[0x38];
    int unkD0;
    u8 padD4[0x70];
};

struct GunShotInfo {
    GunShot* hit;
    float speed;
    float unk8;
    int unkC;
    int charge;
    int unk14;
    int unk18;
    int unk1C;
    int unk20;
};

struct MsgResult {
    void* p;
    char pad[0x1C];
};

struct Target {
    u8 pad[0x24];
    int x, y, z;
};

struct GrabMsg {
    int unk0;
    int asset;
    u8 pad08[0x34 - 0x08];
    int x, y, z;
};

extern EffectType lbl_803BF770;  // the "GUNWITHGRENADE" type descriptor

struct EffectObj {
    int unk0;
    int unk4;
    int emitter;
    void* data;
};

extern "C" {
int fn_800F7D28(int emitter, int id);
void fn_800F86A0(int emitter);
void fn_80175DA8(void* p);
void fn_80175D80(void* p);
void fn_80175EA4(void* p);
void fn_80175D8C(void* p);
void fn_80175EF8(void);
void fn_80175DE8(void* p, float dt);
void fn_80175D84(void* p, float dt);
void fn_80175E40(void* p, float* pos, void* hit, int arg);
void fn_800F825C(int emitter, int id, int count);
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
void fn_8016EDD8(int handle, int* pos);
void fn_800E0E9C(int* out, int* in, int scale);
int fn_800F94C8(int a);
Mtx34* fn_800C6510(int id);
void fn_8016D440(Vec3* out, Vec3* in);
void fn_80163824(int player, u32 id, int a, int b, int c);
void fn_80163810(int player, u32 id, int a, int b, int c);
void fn_80163854(int player, int a, int b);
}

int Fn80175D60(unsigned long arg);
void ActorAim(int* out, Actor* a, int b);
void PlayerAnimOffUpdate(Player* p, float dt);
int PlayerGetPossessor(int arg);
void MtxGetEuler(Vec3* rot, Mtx34* m);
int GrenadeThrow(int a, Vec3* pos, Vec3* rot, int duration, float power);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);

inline float Min(float a, float b) { return a < b ? a : b; }

inline int FxStartAt(void* fx, float* p, int* pos) {
    int h;
    if (p) {
        pos[0] = p[0];
        pos[1] = p[1];
        pos[2] = p[2];
        h = fn_8016F120(fx, pos);
        fn_8016F118(h, 0, 100000);
    } else {
        h = FxStart(fx, 0, 0);
    }
    return h;
}

inline void GunTrack(int fx, int target) {
    MsgResult q;
    int pos[3];

    if (fx != -1 && target != -1 && SendMsg(target, 0x400, &q) && q.p) {
        ActorAim(pos, (Actor*)q.p, 1);
        fn_800E0E9C(pos, pos, 1000);
        pos[0] = pos[0] + ((Target*)q.p)->x;
        pos[1] = pos[1] + ((Target*)q.p)->y;
        pos[2] = pos[2] + ((Target*)q.p)->z;
        fn_8016EDD8(fx, pos);
    }
}

void VFree(void* p);

void GunWithGrenadeReset(EffectObj* obj) {
    GunWithGrenade* g = (GunWithGrenade*)obj->data;

    g->unk00 = 0;
    g->unk04 = 0.0f;
    fn_80175DA8(&g->unk94);
    fn_80175D80(&g->unkD0);
}

void GunWithGrenadeUpdate(EffectObj* obj, float dt) {
    GunWithGrenade* g = (GunWithGrenade*)obj->data;
    GunWithGrenadeDef* def = g->def;

    g->unk20 = 0;
    if (g->unk00 == 1) {
        g->unk04 -= dt;
        while (g->unk04 <= 0.0f) {
            GunShotInfo info;
            int pos[3];
            void* fx;

            g->unk04 += g->unk08;
            if (!fn_800F7D28(g->emitter, def->unk04)) {
                return;
            }
            g->unk0C += def->charge;
            g->unk0C = Min(g->unk0C, def->maxCharge);
            info.hit = &g->shot;
            info.speed = def->speed;
            info.unkC = def->unk14;
            info.charge = g->unk0C;
            info.unk18 = def->unk28;
            info.unk1C = def->unk2C;
            info.hit->spread += def->spread;
            Fn800F7F5C(g->emitter, def->unk04, 0x14967, g->arg, &info, dt);
            fx = def->fx;
            g->unk90 = FxStartAt(fx, &info.hit->x, pos);
        }
    } else if (g->unk00 == 2) {
        GunShotInfo info;
        int pos[3];
        void* fx;

        if (!fn_800F7D28(g->emitter, def->unk64)) {
            return;
        }
        info.hit = &g->shot;
        info.speed = def->speed2;
        info.unk8 = def->unk70;
        info.unkC = def->unk14;
        info.charge = g->unk0C;
        Fn800F7F5C(g->emitter, def->unk64, 0x14967, g->arg, &info, dt);
        fx = def->fx2;
        g->unk90 = FxStartAt(fx, &info.hit->x, pos);
        fn_800F825C(g->emitter, def->unk64, 1);
    }
    if (g->unk90 != -1) {
        GunTrack(g->unk90, g->arg);
    }
    if (g->unk0C > 0.0f) {
        g->unk0C -= 0.001f * dt * def->drain;
        g->unk0C = g->unk0C > 0.0f ? g->unk0C : 0.0f;
    }
    g->unk14 -= dt;
    g->unk14 = g->unk14 > 0.0f ? g->unk14 : 0.0f;
    if (g->unk10 > 0.0f && 0.0f == g->unk14) {
        float step = def->recoilRate * dt;
        if (step <= 0.0f) {
            g->unk10 = 0.0f;
        }
        if (g->unk10 > step) {
            if (((g->arg >> 24) & 0xFF) == 2) {
                MsgResult q;
                SendMsg(g->arg, 0x400, &q);
                PlayerAnimOffUpdate((Player*)q.p, -step);
            }
            g->unk10 -= step;
        } else {
            if (((g->arg >> 24) & 0xFF) == 2) {
                MsgResult q;
                SendMsg(g->arg, 0x400, &q);
                PlayerAnimOffUpdate((Player*)q.p, -g->unk10);
            }
            g->unk10 = 0.0f;
        }
    }
    fn_80175DE8(&g->unk94, dt);
    fn_80175D84(&g->unkD0, dt);
}

u32 GunWithGrenadeQuery(EffectObj* obj, int msg) {
    GunWithGrenade* g = (GunWithGrenade*)obj->data;
    GunWithGrenadeDef* def = g->def;

    switch (msg) {
    case 0x96224FCC:
    case 0x0020EE36:
        if (fn_800F7D28(g->emitter, def->unk04)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->unk04)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0x0D19E338:
        if (g->unk00 == 1) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
    case 0xE6B71BCC:
        if (def->unk08 == fn_800F7D28(g->emitter, def->unk04)) {
            return 0x4848CA1E;
        }
        if (fn_800F7D28(obj->emitter, def->unk04)) {
            return 0x04A3E183;
        }
        return 0x11D2C025;
    case 0xF829DB93:
    case 0x42E50A3D:
        if (fn_800F7D28(g->emitter, def->unk64)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->unk64)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0x9791AF51:
        if (g->unk00 == 2) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
    case 0x968CBE3D:
        if (def->unk68 == fn_800F7D28(g->emitter, def->unk64)) {
            return 0x4848CA1E;
        }
        if (fn_800F7D28(obj->emitter, def->unk64)) {
            return 0x04A3E183;
        }
        return 0x11D2C025;
    }
    return 0x3DA260F7;
}

int GunWithGrenadeMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    GunWithGrenade* g = (GunWithGrenade*)obj->data;
    GunWithGrenadeDef* def = g->def;
    int busy = Fn80175D60(arg);
    Vec3 rot;
    Vec3 pos;


    g->arg = arg;
    switch (msg) {
    case 0x0020EE36:
        if (g->unk00 == 1) {
            memcpy(&g->shot, data, 0x60);
        } else {
            GunShotInfo info;
            MsgResult q;
            int posA[3];
            int posB[3];
            void* fx;
            int kind;

            g->unk20 = 1;
            if (!fn_800F7D28(g->emitter, def->unk04)) {
                return 0;
            }
            g->unk0C += def->charge;
            g->unk0C = Min(g->unk0C, def->maxCharge);
            kind = (arg >> 24) & 0xFF;
            if (kind == 2) {
                SendMsg(arg, 0x400, &q);
                PlayerAnimOffUpdate((Player*)q.p, def->recoil);
                g->unk10 += def->recoil;
                g->unk14 = 300.0f;
            }
            info.hit = (GunShot*)data;
            info.speed = def->speed;
            info.unkC = def->unk14;
            info.charge = g->unk0C;
            info.unk18 = def->unk28;
            info.unk1C = def->unk2C;
            if (((g->arg >> 24) & 0xFF) == 2) {
                ((GunShot*)data)->spread =
                    ((GunShot*)data)->spread * (def->spreadMaxA - def->spreadMinA) + def->spreadMinA;
            } else {
                ((GunShot*)data)->spread =
                    ((GunShot*)data)->spread * (def->spreadMaxB - def->spreadMinB) + def->spreadMinB;
            }
            info.hit->spread *= info.hit->scale;
            Fn800F7F5C(g->emitter, def->unk04, 0x14967, arg, &info, dt);
            fx = def->fx;
            g->unk90 = FxStartAt(fx, &info.hit->x, posA);
            if (fn_80019A04() - lbl_804D54D0 >= 3 && kind == 2) {
                FxStartAt((void*)def->unk2C, &info.hit->x, posB);
                lbl_804D54D0 = fn_80019A04();
            }
            fn_80175E40(&g->unk94, &info.hit->x, info.hit, arg);
            fn_800F825C(g->emitter, def->unk04, 1);
        }
        return 1;
    case 0x96224FCC:
        if (g->unk00 != 1) {
            g->unk00 = 1;
            g->unk04 = g->unk08;
        }
        memcpy(&g->shot, data, 0x60);
        return 1;
    case 0x315A1A7F:
        memcpy(&g->shot, data, 0x60);
        return 1;
    case 0x0D19E338:
        g->unk00 = 0;
        return 1;
    case 0xE6B71BCC: {
        int n;
        int m;
        int r;

        g->unk20 = 1;
        n = fn_800F7D28(g->emitter, def->unk04);
        if (n == def->unk08) {
            return 0;
        }
        m = fn_800F7D28(obj->emitter, def->unk04);
        if (m == 0) {
            return 0;
        }
        n = (m < def->unk08 - n) ? m : def->unk08 - n;
        Query(&r, arg, 0x41E, 0x5A32795A);
        if (r == 0) {
            fn_800F825C(obj->emitter, def->unk04, n);
        }
        Fn800F840C(g->emitter, def->unk04, n);
        return 1;
    }
    case 0xF829DB93:
        if (g->unk00 == 2) {
            memcpy(&g->shot, data, 0x60);
        } else {
            GrabMsg grab;

            g->unk20 = 1;
            if (!fn_800F7D28(g->emitter, def->unk64)) {
                return 0;
            }
            grab.asset = AssetFind(0xF1754BF2, 0x0450B2BB);
            SendMsg(arg, 0x411, &grab);
            pos.x = grab.x;
            pos.y = grab.y;
            pos.z = grab.z;
            if (((arg >> 24) & 0xFF) == 2) {
                Mtx34* m = fn_800C6510(fn_800F94C8(*(int*)(PlayerGetPossessor(arg) + 0x184)));
                MtxGetEuler(&rot, m);
            } else {
                fn_8016D440(&rot, &((GunShot*)data)->dir);
            }
            GrenadeThrow(arg, &pos, &rot, 1, 4800.0f);
            fn_800F825C(g->emitter, def->unk64, 1);
        }
        return 1;
    case 0x42E50A3D:
        if (g->unk00 != 2) {
            g->unk00 = 2;
            g->unk04 = g->unk08;
        }
        memcpy(&g->shot, data, 0x60);
        return 1;
    case 0x9791AF51:
        g->unk00 = 0;
        return 1;
    case 0x968CBE3D: {
        int n;
        int m;
        int r;

        g->unk20 = 1;
        n = fn_800F7D28(g->emitter, def->unk64);
        if (n == def->unk68) {
            return 0;
        }
        m = fn_800F7D28(obj->emitter, def->unk64);
        if (m == 0) {
            return 0;
        }
        n = (m < def->unk68 - n) ? m : def->unk68 - n;
        Query(&r, arg, 0x41E, 0x5A32795A);
        Fn800F840C(g->emitter, def->unk64, n);
        return 1;
    }
    case 0x8FDD7599: {
        GunShot* hit;
        void* fx;
        int pos[3];

        g->unk20 = 1;
        fx = def->hitFx;
        hit = (GunShot*)data;
        g->unk90 = FxStartAt(fx, &hit->x, pos);
        return 1;
    }
    case 0xBC78F550: {
        GunShot* hit;
        void* fx;
        int pos[3];

        g->unk20 = 1;
        fx = def->hitFx2;
        hit = (GunShot*)data;
        g->unk90 = FxStartAt(fx, &hit->x, pos);
        return 1;
    }
    case 0x0B59E861:
        if (busy == 0) {
            int player = PlayerGetPossessor(arg);
            int a = fn_800F7D28(obj->emitter, def->unk04);
            int b = fn_800F7D28(g->emitter, def->unk04);
            int c = def->unk08;
            int d = fn_800F7D28(obj->emitter, def->unk64);
            int e = fn_800F7D28(g->emitter, def->unk64);
            int f = def->unk68;

            fn_80163824(player, 0x878DC2AD, a, b, c);
            fn_80163810(player, 0xAB3DC1F9, d, e, f);
            fn_80163854(player, player + 0x1508, def->hud);
        }
        return 1;
    case 0xE452286F: {
        int player = PlayerGetPossessor(arg);
        short ids[2];

        ids[0] = def->model;
        ids[1] = def->unk28;
        if ((int)data == 1) {
            ModelLoadIds(ids, 2);
        } else {
            ModelFreeIds(ids, 2);
        }
        fn_80163854(player, player + 0x1508, def->hud);
        return 1;
    }
    case 0x141109BD: {
        short ids[2];

        ids[0] = def->model;
        ids[1] = def->unk28;
        if ((int)data == 1) {
            ModelLoadIds(ids, 2);
        } else {
            ModelFreeIds(ids, 2);
        }
        return 1;
    }
    }
    return 0;
}

void GunWithGrenadeDestroy(EffectObj* obj) {
    GunWithGrenade* g = (GunWithGrenade*)obj->data;

    fn_800F86A0(g->emitter);
    fn_80175EA4(&g->unk94);
    fn_80175D8C(&g->unkD0);
    VFree(g);
}

void* GunWithGrenadeCreate(int a, int id) {
    EffectType* type = &lbl_803BF770;
    GunWithGrenade* g;

    EffectTypeHash(type);
    g = (GunWithGrenade*)VAlloc(sizeof(GunWithGrenade));
    memset(g, 0, sizeof(GunWithGrenade));
    if (g == 0) {
        return 0;
    }
    g->def = (GunWithGrenadeDef*)fn_800EB21C(id);
    g->unk20 = 1;
    g->unk8C = fn_800F8AF4(id);
    g->unk00 = 0;
    g->unk04 = 0.0f;
    g->unk08 = 200.0f;
    g->unk90 = -1;
    g->unk0C = 0.0f;
    g->unk10 = 0.0f;
    g->unk14 = 0.0f;
    fn_80175EE4(&g->unk94);
    fn_80175D90(&g->unkD0);
    g->emitter = fn_800F8A30(fn_800F80DC(a));
    lbl_804D54D0 = fn_80019A04();
    Fn800F840C(g->emitter, g->def->unk04, g->def->unk08);
    Fn800F840C(g->emitter, g->def->unk64, g->def->unk68);
    return g;
}


int GunWithGrenadeInit(void) {
    fn_80175EF8();
    return 1;
}

// u_80125DE4.cpp: pickup key actor: update, messages and hit probe (0x80125DE4-0x80127D80).
// Names are descriptive guesses; the code is matched against the retail DOL.

// The pickup "key" actor (0x80125DE4-0x80127D80): reports its position and time left on screen,
// resets after a timeout, and configures its model/colour when told which asset it represents.
typedef unsigned long u32;
typedef unsigned char u8;

extern "C" {
int SendMsg(int to, int msg, void* data);            // 0x800265D4
void FxStart(void* fx, int a, int b);                // 0x8016F390
void* memset(void* dst, int val, unsigned long n);
}

void* FxFind(const char* name);                      // 0x80170834
int Rand();                                          // 0x8000DDD0

// Growable character buffer (code at 0x8028B1E4-0x8028C5xx).
class Buffer {
public:
    Buffer();                                        // 0x8028BA9C
    virtual ~Buffer();                               // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                                        // 0x8028C500
    String(const char* s);                           // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

// Query reply: the receiver writes its answer over the first word of the string.
struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

inline int KeyQuerySend(int* out, int target, u32 hash) {
    QueryMsg m;
    int sent;

    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    sent = SendMsg(target, 0x41E, &m);
    *out = *(int*)&m.result;
    return sent;
}

struct Actor {
    char pad00[0x24];        // 0x00
    int posX;                // 0x24
    int posY;                // 0x28
    int posZ;                // 0x2c
    int (*send)(int msg, void* data); // 0x30
    char pad34[0x4c];        // 0x34
    float* pos;              // 0x80
    u8* unk84;               // 0x84
    char pad88[4];           // 0x88
    int unk8C;               // 0x8c
    int model;               // 0x90
    char pad94[0x10];        // 0x94
    void* unkA4;             // 0xa4
    char padA8[4];           // 0xa8
    u32 flags;               // 0xac
};

struct KeyData {
    u32 asset;               // 0x0
    int unk4;                // 0x4
    float timer;             // 0x8
    int unkC;                // 0xc
    int unk10;               // 0x10
    int unk14;               // 0x14
};

// Model/team selection message for the "configure" query (0x1501), 0x2c bytes.
struct KeyConfigMsg {
    void* object;            // 0x0
    int unk4;                // 0x4
    int team;                // 0x8
    int unkC;                // 0xc
    int unk10;               // 0x10
    int one;                 // 0x14
    int unk18;               // 0x18
    int unk1C;               // 0x1c
    float x;                 // 0x20
    float y;                 // 0x24
    float z;                 // 0x28
};

struct KeyResultMsg {
    void* object;            // 0x0
    int result;              // 0x4
};

struct ShowMsg {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
};

// Hit report sent to the actor itself after the key's downward probe (0xC07).
struct HitMsg {
    Actor* actor;            // 0x0
    int one;                 // 0x4
    int unk8;                // 0x8
    int unkC;                // 0xc
    int x;                   // 0x10
    int y;                   // 0x14
    int z;                   // 0x18
};

// Effect spawn request (0xC04).
struct FxSpawnMsg {
    int unk0;                // 0x0
    int id;                  // 0x4
    int unk8;                // 0x8
    int unkC;                // 0xc
    int unk10;               // 0x10
    int unk14;               // 0x14
    int unk18;               // 0x18
};

// Shared spline-slot table of the pickup keys; field 0xc is a player handle.
struct KeySlotTable {
    char pad00[0xc];         // 0x0
    int handle;              // 0xc
    char pad10[8];           // 0x10
};

struct MsgTarget {
    char pad00[0x30];        // 0x00
    int (*send)(int msg, void* data); // 0x30
    char pad34[4];           // 0x34
    u32 flags;               // 0x38
};

int ActorIsActive(Actor* a);                          // 0x80022D80
void ActorUpdate(Actor* a, float dt);                 // 0x800E48A0
int ActorDefaultMsg(Actor* a, int msg, void* data, int arg); // 0x800E4C64
u32 AssetFind(u32 table, u32 key);                    // 0x8007EB34
int ModelNodeSetup(int model, int asset, int a, int b); // 0x80038AE8
int HandleToPlayer(int handle);                       // 0x800FD720
int PlayerTeam(int player);                           // 0x800FD68C
void ScreenPrintf(const char* fmt, ...);              // 0x801EA9F0
int ActorInitBase(Actor* actor);                      // 0x800E6444
void* SplineSlotAlloc(void* slots, int size, u32* mask, int flag); // 0x801020E0
int PickupKeyQueryMsg(Actor* actor, int* data);       // 0x8012697C
int PickupKeySlotProbe(int* slot, int* out);          // 0x80127164
int PickupKeyCmdMsg(Actor* actor, int* data);         // 0x80126BD8

extern "C" {
int fn_800FC764(int a, int b);                        // 0x800FC764
int fn_800FD3DC();                                    // 0x800FD3DC
int fn_80019F88(int* data, Actor* actor);             // 0x80019F88
float* fn_800FD3FC(int id);                           // 0x800FD3FC
void fn_80037EC0(int a, int b, int c, int d, int e, int f, int g); // 0x80037EC0
int fn_8001F928(Actor* actor, int* probe, u32 mask, void (*cb)(), int* out); // 0x8001F928
int PickupKeyProbeHit(int* probe, int* hit);           // 0x80127240
}

extern "C" int lbl_804D2E34;                          // 0x804D2E34
extern "C" float lbl_804D7890;                        // 0x804D7890
extern "C" float lbl_804D7894;                        // 0x804D7894
extern "C" double lbl_804D78A0;                       // 0x804D78A0
extern "C" int lbl_8039F610[9];                       // 0x8039F610
extern "C" KeySlotTable lbl_8039F568;                 // 0x8039F568
extern "C" unsigned long lbl_804D2E30;                // 0x804D2E30
extern "C" int lbl_804D4CD8;                          // 0x804D4CD8
extern "C" signed char lbl_804D4CDC;                  // 0x804D4CDC

void PickupKeyUpdate(Actor* actor, float dt) {
    KeyData* d;
    int answer;

    d = (KeyData*)actor->unkA4;
    KeyQuerySend(&answer, 0x4B000000, 0x9A5A9E33);
    if (d != 0) {
        if (ActorIsActive(actor)) {
            d->timer += dt;
            if (d->timer >= lbl_804D7890 * (float)lbl_804D2E34) {
                if (fn_800FC764(actor->unk8C, 0) == 0) {
                    float* p = actor->pos;
                    actor->posX = (int)p[0];
                    actor->posY = (int)p[1];
                    actor->posZ = (int)p[2];
                }
                d->timer = lbl_804D7894;
            }
            if (answer != 0) {
                ScreenPrintf("Key is at %d %d %d", actor->posX, actor->posY, actor->posZ);
                ScreenPrintf("Key will reset in %.0f ms",
                             lbl_804D7890 * (float)lbl_804D2E34 - d->timer);
            }
        }
        ActorUpdate(actor, dt);
    }
}

// Message handler of the pickup key.
int PickupKeyMessage(Actor* a, int msg, void* dataPtr, int arg) {
    int* data = (int*)dataPtr;
    Actor* actor = (Actor*)data[0];
    KeyData* d = (KeyData*)actor->unkA4;
    KeyConfigMsg m;
    ShowMsg sm;
    int query;

    if (d == 0) {
        return 0;
    }
    switch (msg) {
    case 0x41E:
        if (PickupKeyQueryMsg(actor, data) == 1) {
            return 1;
        }
        break;
    case 0x405: {
        MsgTarget* target;
        KeyResultMsg q;

        if (d == 0) {
            return 0;
        }
        if (data[1] == 0) {
            return 0;
        }
        memset(&m, 0, sizeof(m));
        m.object = (void*)data[1];
        KeyQuerySend(&query, 0x4B000000, 0x56CF46F2);
        if (query == 0) {
            m.team = lbl_8039F610[d->asset];
        } else {
            m.team = lbl_8039F610[PlayerTeam(HandleToPlayer(*(int*)(data[1] + 0x38)))];
        }
        m.one = 1;
        m.x = (float)actor->posX;
        m.y = (float)actor->posY;
        m.z = (float)actor->posZ;
        target = (MsgTarget*)data[1];
        if (target->send(0x1501, &m) == 0) {
            data[4] = 0;
        } else {
            sm.unk4 = 0;
            SendMsg(actor->unk8C, 0xC00, &sm);
            d->timer = lbl_804D7894;
            d->unk10 = 1;
            q.object = (void*)data[1];
            target->send(0x400, &q);
            d->unkC = q.result;
            FxStart(FxFind("key_pickup"), 0, 0);
            if (target->flags >> 24 == 2) {
                d->unk14 = 1;
            }
        }
        return 1;
    }
    case 0x41D: {
        KeyData* dd;
        int handled;
        int value;

        handled = 1;
        if (fn_80019F88(data, actor) == 0) {
            if (data[1] == 0xB55C799) {
                lbl_804D2E34 = data[3];
            } else if (data[1] == 0x9BD4EF06) {
                dd = (KeyData*)actor->unkA4;
                value = data[3];
                if (dd != 0) {
                    KeyQuerySend(&query, 0x4B000000, 0x56CF46F2);
                    if (query == 0) {
                        int asset = AssetFind(0xD9044CBB, 0x3691C91A);
                        float* c;

                        ModelNodeSetup(actor->model, asset, 1, 1);
                        c = fn_800FD3FC(value);
                        fn_80037EC0(actor->model, asset, 1,
                                    (int)(float)(lbl_804D78A0 * c[0]) & 0xFF,
                                    (int)(float)(lbl_804D78A0 * c[1]) & 0xFF,
                                    (int)(float)(lbl_804D78A0 * c[2]) & 0xFF,
                                    (int)(float)(lbl_804D78A0 * c[3]) & 0xFF);
                        asset = AssetFind(0xD9044CBB, 0xD1EB3B71);
                        ModelNodeSetup(actor->model, asset, 0, 1);
                    } else {
                        int asset = AssetFind(0xD9044CBB, 0x3691C91A);

                        ModelNodeSetup(actor->model, asset, 0, 1);
                        asset = AssetFind(0xD9044CBB, 0xD1EB3B71);
                        ModelNodeSetup(actor->model, asset, 1, 1);
                    }
                    dd->asset = value;
                }
            } else {
                handled = 0;
            }
        }
        if (handled == 1) {
            return 1;
        }
        break;
    }
    case 0xC14:
        return PickupKeyCmdMsg(actor, data);
    }
    return ActorDefaultMsg(a, msg, data, arg);
}

// Actor init: publish the shared key timeout, take a spline slot, set up the model nodes
// and send the spawn/effect messages. Returns 1 when the actor was configured.
int PickupKeyInit(Actor* actor) {
    KeyData* d;
    KeyData* d2;
    QueryMsg q;
    int answer;
    int probe[6];
    int hit[3];
    int asset;
    float* c;
    ShowMsg m1;
    ShowMsg m2;
    HitMsg hm;
    FxSpawnMsg fm;

    if (!lbl_804D4CDC) {
        lbl_804D4CD8 = 0;
        lbl_804D4CDC = 1;
    }
    if (lbl_804D4CD8 == 0) {
        q.hash = 0x0B55C799;
        q.unk8 = 0;
        *(int*)&q.result = 0x14;
        SendMsg(0x4B000000, 0x41D, &q);
        lbl_804D4CD8 = 1;
    }
    if (ActorInitBase(actor) == 0) {
        return 0;
    }
    d = (KeyData*)SplineSlotAlloc(&lbl_8039F568, 0x18, &lbl_804D2E30, 1);
    if (d == 0) {
        return 0;
    }
    d->unk4 = (int)actor;
    d->timer = lbl_804D7894;
    d->unkC = -1;
    d->unk10 = 0;
    actor->unkA4 = d;
    if (actor->unk84 == 0) {
        return 0;
    }
    ModelNodeSetup(actor->model, AssetFind(0xD9044CBB, 0x368F6F11), 0, 1);
    ModelNodeSetup(actor->model, AssetFind(0xD9044CBB, 0x7DA1C2DA), 0, 1);
    ModelNodeSetup(actor->model, AssetFind(0xD9044CBB, 0x9BA7670C), 0, 1);
    ModelNodeSetup(actor->model, AssetFind(0xD9044CBB, 0xF748CD6B), 0, 1);
    if (*(int*)actor->unk84 != (int)0xBA60EF56) {
        return 0;
    }
    d2 = (KeyData*)actor->unkA4;
    if (d2 != 0) {
        KeyQuerySend(&answer, 0x4B000000, 0x56CF46F2);
        if (answer == 0) {
            asset = AssetFind(0xD9044CBB, 0x3691C91A);
            ModelNodeSetup(actor->model, asset, 1, 1);
            c = fn_800FD3FC(0);
            fn_80037EC0(actor->model, asset, 1,
                        (int)(float)(lbl_804D78A0 * c[0]) & 0xFF,
                        (int)(float)(lbl_804D78A0 * c[1]) & 0xFF,
                        (int)(float)(lbl_804D78A0 * c[2]) & 0xFF,
                        (int)(float)(lbl_804D78A0 * c[3]) & 0xFF);
            ModelNodeSetup(actor->model, AssetFind(0xD9044CBB, 0xD1EB3B71), 0, 1);
        } else {
            ModelNodeSetup(actor->model, AssetFind(0xD9044CBB, 0x3691C91A), 0, 1);
            ModelNodeSetup(actor->model, AssetFind(0xD9044CBB, 0xD1EB3B71), 1, 1);
        }
        d2->asset = 0;
    }
    actor->flags |= 0x20000000;
    m1.unk4 = 0x5E5BCE3E;
    m1.unk8 = actor->unk8C;
    m1.unkC = (int)0x901EEFA2;
    SendMsg(0x4B000000, 0xC14, &m1);
    m2.unk4 = 0;
    m2.unk8 = actor->unk8C;
    m2.unkC = 0x2840341C;
    SendMsg(0x4B000000, 0xC14, &m2);
    probe[0] = actor->posX;
    probe[1] = actor->posY;
    probe[2] = actor->posZ;
    probe[3] = actor->posX;
    probe[4] = actor->posY;
    probe[5] = actor->posZ - 0x7FFF;
    if (fn_8001F928(actor, probe, 0x80000000,
                    (void (*)())PickupKeyProbeHit, hit) != 0) {
        hm.actor = actor;
        hm.one = 1;
        hm.x = hit[0];
        hm.y = hit[1];
        hm.z = hit[2] + 0x190;
        actor->send(0xC07, &hm);
    }
    if (*(int*)(actor->unk84 + 0x94) != -1) {
        fm.id = *(int*)(actor->unk84 + 0x94);
        fm.unk8 = 0;
        fm.unkC = 0;
        fm.unk10 = 0x1000;
        fm.unk14 = 0;
        fm.unk18 = 0;
        SendMsg(actor->unk8C, 0xC04, &fm);
    }
    d->unk14 = 0;
    return 1;
}

// Query handler of the pickup key: answers the state/hash questions asked by other
// actors (current asset, key data pointer, owning player's team, ...). Returns 1 when
// the query was recognised.
int PickupKeyQueryMsg(Actor* actor, int* data) {
    KeyData* d;
    int answer;

    d = (KeyData*)actor->unkA4;
    if (data[1] == 0x12868306) {
        data[3] = d->unkC;
    } else if (data[1] < 0x12868306) {
        if (data[1] == 0x029AE71B) {
            data[3] = d->unk10;
            return 1;
        }
        if (data[1] < 0x029AE71B && data[1] == (int)0x9BD4EF06) {
            data[3] = (int)d;
            return 1;
        }
    } else {
        if (data[1] == 0x5ED5BF20) {
            if (ActorIsActive(actor) == 0 && lbl_8039F568.handle < 0) {
                return 0;
            }
            KeyQuerySend(&answer, 0x4B000000, 0x56CF46F2);
            if (answer == 0) {
                data[3] = (int)0xEAFE2C46;
            } else if (lbl_8039F568.handle < 0) {
                data[3] = (int)0xEAFE2C46;
            } else {
                switch (PlayerTeam(HandleToPlayer(lbl_8039F568.handle))) {
                case 0: data[3] = 0x114F1979; break;
                case 1: data[3] = 0x6B4B4A10; break;
                case 2: data[3] = 0x21425B82; break;
                case 3: data[3] = (int)0xFAD5AC93; break;
                case 4: data[3] = 0x6BA1BC35; break;
                case 5: data[3] = (int)0xDBEC014D; break;
                case 6: data[3] = (int)0xE54B6BDB; break;
                case 7: data[3] = 0x154F7C95; break;
                default: data[3] = (int)0xEAFE2C46; break;
                }
            }
            return 1;
        }
        if (data[1] < 0x5ED5BF20 && data[1] == 0x5C8F66F9) {
            data[3] = 0x5E5BCE3E;
            return 1;
        }
    }
    return 0;
}

// Sub-message payload of the key's 0xC14 message: `pos` is the requested drop point and
// `cmd` selects the sub-operation.
struct KeyEventMsg {
    int unk0;                // 0x0
    float* pos;              // 0x4
    int unk8;                // 0x8
    u32 cmd;                 // 0xc
};

// Sub-message handler of the pickup key (message 0xC14). 0x6C345908 re-probes the ground
// under a caller-supplied position and picks one of the available asset variants;
// 0x56CF46F2 refreshes the model colours from the currently selected asset.
int PickupKeyCmdMsg(Actor* actor, int* dataPtr) {
    KeyEventMsg* msg = (KeyEventMsg*)dataPtr;

    switch (msg->cmd) {
    case 0x6C345908: {
        float* pos = msg->pos;
        int count = fn_800FD3DC();
        KeyData* d = (KeyData*)actor->unkA4;
        HitMsg dropHit;
        HitMsg groundHit;
        ShowMsg show;
        int hitPos[3];
        int probe[6];
        int found;
        int handle;

        if (pos == 0) {
            return 1;
        }
        if (count == 0) {
            count = 1;
        }
        d->unk10 = 0;
        dropHit.actor = actor;
        dropHit.one = 1;
        dropHit.x = (int)pos[0];
        dropHit.y = (int)pos[1];
        dropHit.z = (int)pos[2];
        actor->send(0xC07, &dropHit);

        probe[0] = actor->posX;
        probe[1] = actor->posY;
        probe[2] = actor->posZ;
        probe[3] = actor->posX;
        probe[4] = actor->posY;
        probe[5] = actor->posZ - 0x7FFF;
        if (fn_8001F928(actor, probe, 0x80000000,
                        (void (*)())PickupKeyProbeHit, hitPos) != 0) {
            groundHit.actor = actor;
            groundHit.one = 1;
            groundHit.x = hitPos[0];
            groundHit.y = hitPos[1];
            groundHit.z = hitPos[2] + 0x190;
            actor->send(0xC07, &groundHit);
        }
        show.unk4 = 1;
        SendMsg(actor->unk8C, 0xC00, &show);
        d->unkC = -1;

        probe[0] = actor->posX;
        probe[1] = actor->posY;
        probe[2] = actor->posZ;
        probe[3] = actor->posX;
        probe[4] = actor->posY;
        probe[5] = actor->posZ - 0x7FFF;
        found = fn_8001F928(actor, probe, 0x40000000,
                            (void (*)())PickupKeySlotProbe, &handle);
        if (found != 0) {
            return 1;
        }
        if (*(int*)actor->unk84 == (int)0xBA60EF56) {
            int value = Rand() % count;
            KeyData* dd = (KeyData*)actor->unkA4;

            if (dd != 0) {
                int answer;
                int asset;
                float* c;

                KeyQuerySend(&answer, 0x4B000000, 0x56CF46F2);
                if (answer == 0) {
                    asset = AssetFind(0xD9044CBB, 0x3691C91A);
                    ModelNodeSetup(actor->model, asset, 1, 1);
                    c = fn_800FD3FC(value);
                    fn_80037EC0(actor->model, asset, 1,
                                (int)(lbl_804D78A0 * c[0]) & 0xFF,
                                (int)(lbl_804D78A0 * c[1]) & 0xFF,
                                (int)(lbl_804D78A0 * c[2]) & 0xFF,
                                (int)(lbl_804D78A0 * c[3]) & 0xFF);
                    ModelNodeSetup(actor->model,
                                   AssetFind(0xD9044CBB, 0xD1EB3B71), 0, 1);
                } else {
                    ModelNodeSetup(actor->model,
                                   AssetFind(0xD9044CBB, 0x3691C91A), 0, 1);
                    ModelNodeSetup(actor->model,
                                   AssetFind(0xD9044CBB, 0xD1EB3B71), 1, 1);
                }
                dd->asset = value;
            }
        }
        return 0;
    }
    case 0x56CF46F2: {
        KeyData* d = (KeyData*)actor->unkA4;

        if (d != 0) {
            int answer;
            int asset;
            float* c;

            KeyQuerySend(&answer, 0x4B000000, 0x56CF46F2);
            if (answer != 0) {
                ModelNodeSetup(actor->model,
                               AssetFind(0xD9044CBB, 0x3691C91A), 0, 1);
                ModelNodeSetup(actor->model,
                               AssetFind(0xD9044CBB, 0xD1EB3B71), 1, 1);
            } else {
                asset = AssetFind(0xD9044CBB, 0x3691C91A);
                ModelNodeSetup(actor->model, asset, 1, 1);
                c = fn_800FD3FC(0);
                fn_80037EC0(actor->model, asset, 1,
                            (int)(lbl_804D78A0 * c[0]) & 0xFF,
                            (int)(lbl_804D78A0 * c[1]) & 0xFF,
                            (int)(lbl_804D78A0 * c[2]) & 0xFF,
                            (int)(lbl_804D78A0 * c[3]) & 0xFF);
                ModelNodeSetup(actor->model,
                               AssetFind(0xD9044CBB, 0xD1EB3B71), 0, 1);
            }
            d->asset = 0;
        }
        break;
    }
    }
    return ActorDefaultMsg((Actor*)actor->unk8C, 0xC14, msg, 0);
}

// Spline-slot owner probe (0x80127164): asks the slot's holder who owns it (message 0x400)
// and, when the reply names a live player, writes that owner through `out` and returns 1.
int PickupKeySlotProbe(int* slot, int* out) {
    Actor* owner;
    int answer;
    int reply[2];

    owner = (Actor*)*slot;
    if (owner == 0) {
        return 0;
    }
    reply[0] = (int)owner;
    if (owner->send(0x400, reply) == 0) {
        return 0;
    }
    KeyQuerySend(&answer, reply[1], 0x5C8F66F9);
    if (answer != 0x1AF27550) {
        return 0;
    }
    *out = (int)owner;
    return 1;
}

// Collision-probe callback of the pickup key: accepts the probe result `probe` when its
// first word is clear and copies the three hit coordinates out through `hit`.
extern "C" int PickupKeyProbeHit(int* probe, int* hit) {
    if (*(unsigned int*)probe == 0) {
        hit[0] = probe[0xc];
        hit[1] = probe[0xd];
        hit[2] = probe[0xe];
        return 1;
    }
    return 0;
}

// ---- Option blocks of the settings message below (names are descriptive guesses) ----
// The 0x64-byte option block at 0x8039F6B0: 25 consecutive words, meanings unknown.
struct KeyOptions {
    int unk00;               // 0x00
    int unk04;               // 0x04
    int unk08;               // 0x08
    int unk0C;               // 0x0c
    int unk10;               // 0x10
    int unk14;               // 0x14
    int unk18;               // 0x18
    int unk1C;               // 0x1c
    int unk20;               // 0x20
    int unk24;               // 0x24
    int unk28;               // 0x28
    int unk2C;               // 0x2c
    int unk30;               // 0x30
    int unk34;               // 0x34
    int unk38;               // 0x38
    int unk3C;               // 0x3c
    int unk40;               // 0x40
    int unk44;               // 0x44
    int unk48;               // 0x48
    int unk4C;               // 0x4c
    int unk50;               // 0x50
    int unk54;               // 0x54
    int unk58;               // 0x58
    int unk5C;               // 0x5c
    int unk60;               // 0x60
};

// The 0x54-byte limit block at 0x8045E768; only +0x04 (float) and +0x0c (count) are used.
struct KeyLimits {
    char pad00[4];           // 0x00
    float unk04;             // 0x04
    char pad08[4];           // 0x08
    int maxKeys;             // 0x0c
    char pad10[0x44];        // 0x10
};

extern "C" KeyOptions lbl_8039F6B0;      // 0x8039F6B0
extern "C" KeyLimits lbl_8045E768;       // 0x8045E768
extern "C" int lbl_804D2E38;             // 0x804D2E38
extern "C" float lbl_804D78A8;           // 0x804D78A8
extern "C" float lbl_804D78AC;           // 0x804D78AC

// "changed" notification of the next unit, still an fn_ placeholder there.
extern "C" void fn_80127D80(int id);     // 0x80127D80

// One get/set request of the settings dispatcher: `id` names the option, `value` carries
// the number (an int, or a float for the camera-distance option).
union SettingValue {
    int i;
    float f;
};

struct SettingMsg {
    int unk0;                // 0x0
    int id;                  // 0x4
    int unk8;                // 0x8
    SettingValue value;      // 0xc
};

#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))

// Global option get/set dispatcher. With `mode` 0 the value carried by `msg` is clamped
// and stored into the global option block (then the change is announced with the option
// id); otherwise the stored value is copied back into the message. Returns 1 when the
// option id was recognised.
extern "C" int fn_80127274(int mode, SettingMsg* msg) {
    int handled = 0;
    int maxKeys = lbl_8045E768.maxKeys;

    switch (msg->id) {
    case -0x16F4AEBB: {                  // 0xE90B5145
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk50 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk50;
        }
        handled = 1;
        break;
    }
    case -0x3E720AE8: {                  // 0xC18DF518
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk28 = CLAMP(v, -5, 5);
        } else {
            msg->value.i = lbl_8039F6B0.unk28;
        }
        handled = 1;
        break;
    }
    case -0x57781077: {                  // 0xA887EF89
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk04 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk04;
        }
        handled = 1;
        break;
    }
    case -0x65A561CD: {                  // 0x9A5A9E33
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk60 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk60;
        }
        handled = 1;
        break;
    }
    case -0x76907381: {                  // 0x896F8C7F
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk40 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk40;
        }
        handled = 1;
        break;
    }
    case -0x63CA6632: {                  // 0x9C3599CE
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk18 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk18;
        }
        handled = 1;
        break;
    }
    case -0x42123DB8: {                  // 0xBDEDC248
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk14 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk14;
        }
        handled = 1;
        break;
    }
    case -0x57063F12: {                  // 0xA8F9C0EE
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk08 = CLAMP(v, 5, 0x4B0);
        } else {
            msg->value.i = lbl_8039F6B0.unk08;
        }
        handled = 1;
        break;
    }
    case -0x2E25ED19: {                  // 0xD1DA12E7
        if (mode == 0) {
            float v = msg->value.f;
            lbl_8045E768.unk04 = CLAMP(v, lbl_804D78A8, lbl_804D78AC);
        } else {
            msg->value.f = lbl_8045E768.unk04;
        }
        handled = 1;
        break;
    }
    case -0x3CAA1712: {                  // 0xC355E8EE
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk3C = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk3C;
        }
        handled = 1;
        break;
    }
    case -0x3D6E4CD3: {                  // 0xC291B32D
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk1C = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk1C;
        }
        handled = 1;
        break;
    }
    case -0x2E6637B6: {                  // 0xD199C84A
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk34 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk34;
        }
        handled = 1;
        break;
    }
    case -0x16FA55FE: {                  // 0xE905AA02
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk00 = CLAMP(v, 1, 0x32);
        } else {
            msg->value.i = lbl_8039F6B0.unk00;
        }
        handled = 1;
        break;
    }
    case -0x1D5CDB3C: {                  // 0xE2A324C4
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk44 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk44;
        }
        handled = 1;
        break;
    }
    case 0x0B55C799: {
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk5C = CLAMP(v, 1, 300);
        } else {
            msg->value.i = lbl_8039F6B0.unk5C;
        }
        handled = 1;
        break;
    }
    case -0x0402983E: {                  // 0xFBFD67C2
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk10 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk10;
        }
        handled = 1;
        break;
    }
    case -0x0F046D21: {                  // 0xF0FB92DF
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk58 = CLAMP(v, 1, 0x78);
        } else {
            msg->value.i = lbl_8039F6B0.unk58;
        }
        handled = 1;
        break;
    }
    case -0x167D1E20: {                  // 0xE982E1E0
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk4C = CLAMP(v, 0x400, 0x2000);
        } else {
            msg->value.i = lbl_8039F6B0.unk4C;
        }
        handled = 1;
        break;
    }
    case -0x06F9AC36: {                  // 0xF90653CA
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk24 = CLAMP(v, 1, 8);
        } else {
            msg->value.i = lbl_8039F6B0.unk24;
        }
        handled = 1;
        break;
    }
    case 0x02D7303A: {
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk48 = CLAMP(v, 1, maxKeys);
        } else {
            msg->value.i = lbl_8039F6B0.unk48;
        }
        handled = 1;
        break;
    }
    case 0x01512C7: {
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk2C = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk2C;
        }
        handled = 1;
        break;
    }
    case 0x31809F6F: {
        if (mode == 0) {
            lbl_804D2E38 = msg->value.i;
        } else {
            msg->value.i = lbl_804D2E38;
        }
        handled = 1;
        break;
    }
    case 0x1783A79C: {
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk30 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk30;
        }
        handled = 1;
        break;
    }
    case 0x0E1CEE7E: {
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk20 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk20;
        }
        handled = 1;
        break;
    }
    case 0x2D88B734: {
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk38 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk38;
        }
        handled = 1;
        break;
    }
    case 0x78F28F77: {
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk54 = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk54;
        }
        handled = 1;
        break;
    }
    case 0x6E9F5FB8: {
        if (mode == 0) {
            int v = msg->value.i;
            lbl_8039F6B0.unk0C = CLAMP(v, 0, 1);
        } else {
            msg->value.i = lbl_8039F6B0.unk0C;
        }
        handled = 1;
        break;
    }
    }

    if (handled != 0 && mode == 0) {
        fn_80127D80(msg->id);
    }
    return handled;
}

// PickupKeyNoop: empty trailing function of the unit (retail is a bare `blr`).
// u_80100EB0.cpp calls it under the placeholder name, so the symbol keeps it.
extern "C" void fn_80127D7C() {
}

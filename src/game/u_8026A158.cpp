// u_8026A158.cpp: "Ammo Refill" item message handler and two user-geometry helpers (0x8026A158-0x8026A37C).
// Names are descriptive guesses; the code is matched against the retail DOL.

class Actor;

class Buffer
{
public:
    virtual ~Buffer();
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer
{
public:
    String();
    ~String() {}
    virtual void Release(char* p);
};

// Message payload used by the "Ammo Refill" item handler.
struct AmmoQuery
{
    char* owner;            // +0x00: object owning the animation handle
    int anim;               // +0x04
    unsigned long query;    // +0x08: name hash of the requested animation
    int result;             // +0x0c
};

struct QueryResult
{
    String name;
};

struct QueryMsg
{
    int unk0;
    unsigned long hash;
    int unk8;
    QueryResult result;
};

struct AnimMsg
{
    int unk0;
    int unk4;
    unsigned long key;
    int asset;
    int unk10[4];
};

extern "C" int SendMsg(int to, int msg, void* data);
void* AssetFind(unsigned long table, unsigned long key);
extern unsigned char __upper_map[];
int ActorPlayAnim(int handle, unsigned long hash, int arg);
int ActorDefaultMsg(Actor* actor, int msg, void* data, int param);

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int param);

struct ActorType
{
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void (*update)(Actor* actor, float dt);
    int (*init)(Actor* actor);
    void* unk14[7];
};

int AmmoRefillMsg(Actor* actor, int msg, void* msgData, int param);

ActorType gAmmoRefillType = { 0, "Ammo Refill", AmmoRefillMsg };

inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline unsigned long StrHash(const char* s, unsigned long h)
{
    int i;

    if (s == 0)
        return 0;
    for (i = 0; s[i] != 0; i++)
        h = h * 31 + (unsigned char)toupper(s[i]);
    return h % 0xFFFFFFFF;
}

// Handler for messages sent to the "Ammo Refill" item.  msg 0x40a looks the
// requested animation up by its hashed name; msg 0x409 plays a fixed one.
int AmmoRefillMsg(Actor* actor, int msg, void* msgData, int param)
{
    AmmoQuery* data = (AmmoQuery*)msgData;

    switch (msg)
    {
    case 0x40a:
        if (ActorPlayAnim(*(int*)(data->owner + 0x38), StrHash("QUERY", data->query), data->anim) != 0)
            return 1;

        if (data->query == 0x7c44ec3d)
        {
            AnimMsg am;

            am.unk4 = 0;
            data->result = 0xf21581de;
            am.key = data->result;
            am.asset = (int)AssetFind(0xc0a77187, 0x208548c7);
            SendMsg(data->anim, 0x40b, &am);
            return 1;
        }
        return 0;

    case 0x409:
        if (ActorPlayAnim(*(int*)(data->owner + 0x38), data->result, data->query) == 0)
        {
            QueryMsg q;

            q.hash = 0xece3c8b7;
            SendMsg(data->query, 0x41d, &q);
        }
        return 1;
    }

    return ActorDefaultMsg(actor, msg, msgData, param);
}

extern int lbl_804D5518;
extern int lbl_804D551C;

// Clears the two item statics owned by this unit.
void AmmoRefillResetStatics(void)
{
    lbl_804D5518 = 0;
    lbl_804D551C = 0;
}

class UserGeom;

UserGeom* SolverAllocBlock(unsigned long size);
UserGeom* UserGeomCreate(UserGeom* geom, int type);

// Allocates a 0x50-byte user geometry and initialises it for the given type.
UserGeom* UserGeomAlloc(int type)
{
    UserGeom* geom = SolverAllocBlock(0x50);

    if (geom != 0)
        geom = UserGeomCreate(geom, type);
    return geom;
}

// Returns the solver handle stored in the geometry.
int UserGeomGetId(UserGeom* geom)
{
    return *(int*)((char*)geom + 0x4c);
}

// u_8026A158.cpp: "Ammo Refill" item message handler and two user-geometry helpers (0x8026A158-0x8026A37C).
// Names are descriptive guesses; the code is matched against the retail DOL.

class Actor;

class Buffer
{
public:
    virtual ~Buffer();
};

class String : public Buffer
{
public:
    String();
};

// Message payload used by the "Ammo Refill" item handler.
struct AmmoQuery
{
    char* owner;            // +0x00: object owning the animation handle
    int anim;               // +0x04
    unsigned long query;    // +0x08: name hash of the requested animation
    int result;             // +0x0c
};

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void* AssetFind(unsigned long, unsigned long);
extern unsigned char __upper_map[];
int ActorPlayAnim(int handle, unsigned long hash, int arg);
int ActorDefaultMsg(Actor* actor, int msg, void* data, int param);

// Handler for messages sent to the "Ammo Refill" item.  msg 0x40a looks the
// requested animation up by its hashed name; msg 0x409 plays a fixed one.
int AmmoRefillMsg(Actor* actor, int msg, void* msgData, int param)
{
    AmmoQuery* data = (AmmoQuery*)msgData;

    if (msg == 0x40a)
    {
        unsigned long hash = data->query;
        const char* name = "QUERY";

        if (name != 0)
        {
            const char* p;

            for (p = name; *p != 0; p++)
            {
                int c = (signed char)*p;
                int up = (c == -1) ? -1 : __upper_map[(unsigned char)c];

                hash = hash * 31;
                hash += (unsigned char)up;
            }
            hash = hash % 0xffffffffUL;
        }
        else
        {
            hash = 0;
        }

        if (ActorPlayAnim(*(int*)(data->owner + 0x38), hash, data->anim) != 0)
            return 1;

        if (data->query == 0x7c44ec3d)
        {
            int reply[4];

            reply[1] = 0;
            data->result = 0xf21581de;
            reply[2] = data->result;
            reply[3] = (int)AssetFind(0xc0a77187, 0x208548c7);
            SendMsg(data->anim, 0x40b, reply);
            return 1;
        }
        return 0;
    }

    if (msg < 0x40a && msg > 0x408)
    {
        if (ActorPlayAnim(*(int*)(data->owner + 0x38), data->result, data->query) == 0)
        {
            String str;
            int reply[3];

            reply[0] = 0;
            reply[1] = 0xece3c8b7;
            SendMsg(data->query, 0x41d, reply);
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

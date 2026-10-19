// Fuel.cpp: the "Fuel" item type (0x802A7E3C-0x802A815C).
// When picked up it asks the level for a free slot, spawns the fuel can at the owner and tells the
// ghost; when dropped it frees the slot again.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Vec3 {
    float x, y, z;
};

struct Mtx33 {
    float m[3][3];
};

struct Mtx34 {
    float m[3][4];
};

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

struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

struct ItemType {
    const char* name;
    int unk4[6];
    int (*msg)(int item, int msg, u32 owner, void* data);   // 0x1C
    u32 (*id)(void);                                         // 0x20
    void (*update)(void);                                    // 0x24
    int unk28[2];
};

struct SpawnMsg {
    int unk0;
    u32 owner;
    int asset;
    int unkC;
    Mtx33 rot;
    Vec3 pos;
};

struct OwnerInfo {
    char pad0[0xD8];
    int fuelAsset;
};

extern "C" int SendMsg(int target, int msg, void* data);            // 0x800265D4
int AssetFind(u32 bank, u32 name);                        // 0x8007EB34
void MtxSetEuler(Mtx34* m, Vec3* rot);                    // 0x8009FAC0
void Fn80175D60(u32 owner);                               // 0x80175D60
void Vec3Set(Vec3* v, float x, float y, float z);         // 0x8016D67C
OwnerInfo* OwnerGetInfo(u32 owner);                       // 0x8002394C

void FuelUpdate(void);
u32 FuelId(void);
int FuelMsg(int item, int msg, u32 owner, void* data);

ItemType gFuelType = { "Fuel", { 0 }, FuelMsg, FuelId, FuelUpdate };

void FuelUpdate(void) {}

u32 FuelId(void) { return 0x3DA260F7; }

static inline void SendQuery(int target, u32 hash, int value) {
    QueryMsg q;
    q.hash = hash;
    *(int*)&q.result = value;
    q.unk8 = 0;
    SendMsg(target, 0x41D, &q);
}

static inline void QueryValue(int target, u32 hash, int* out) {
    QueryMsg q;
    q.hash = hash;
    *(int*)&q.result = 0;
    q.unk8 = 0;
    SendMsg(target, 0x41E, &q);
    *out = *(int*)&q.result;
}

static inline int FuelPickup(u32 owner, void* data) {
    if (data) {
        if (*((int**)data)[1] == -1) {
            QueryMsg unused;
            SpawnMsg m;
            Vec3 rot;
            QueryValue(0x62000000, 0x00012D80, ((int**)data)[1]);
            m.owner = owner;
            if ((int)(owner >> 24) == 19) {
                m.asset = OwnerGetInfo(owner)->fuelAsset;
            } else {
                m.asset = AssetFind(0xF1754BF2, 0x7B91FFD5);
            }
            m.unkC = 1;
            Vec3Set(&rot, -90.0f, 0.0f, 0.0f);
            MtxSetEuler((Mtx34*)&m.rot, &rot);
            m.pos.x = 0.0f;
            m.pos.y = 0.0f;
            m.pos.z = 0.0f;
            SendMsg(*((int**)data)[1], 0x415, &m);
        }
        if (*((int**)data)[1]) {
            SendQuery(*((int**)data)[1], 0x7A281983, 1);
        }
    }
    return 1;
}

static inline int FuelDrop(void* data) {
    if (data) {
        if (*((int**)data)[1]) {
            SendQuery(*((int**)data)[1], 0x7A281983, 0);
            SendQuery(*((int**)data)[1], 0x00210E6C, 1);
            *((int**)data)[1] = -1;
        }
    }
    return 1;
}

int FuelMsg(int item, int msg, u32 owner, void* data) {
    Fn80175D60(owner);

    switch (msg) {
    case 0x96224FCC:
        return FuelPickup(owner, data);
    case 0x0D19E338:
        return FuelDrop(data);
    case 0x00014967:
        return 1;
    }
    return 0;
}

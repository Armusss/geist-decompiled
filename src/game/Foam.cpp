// Foam.cpp: the "Foam" item type (0x801965FC-0x801969D8). Picking it up spawns the WP_FIRE1 model at the
// owner and tells the level; dropping frees it. Split from the Extinguisher half: each file has its own
// string pool (starting with the action-name table) and its own .sdata2 constants.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include <string.h>

extern "C" {
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
int fn_800F8AF4(int id);
int fn_800F7D28(int emitter, int id);
void fn_800F825C(int emitter, int id, int count);
void fn_800F86A0(int emitter);
int fn_8016F120(void* fx, int* pos);
void fn_8016F118(int handle, int a, int b);
int FxStart(void* fx, int a, int b);
void fn_8016EDD8(int handle, int* pos);
void fn_8016EEF4(int handle);
void fn_800E1098(void* out, void* in, float length);
int fn_800201E4(void* player, void* seg, int a, int b, int c);
int fn_8001A30C(int target, void* pos, void* delta, int a, int (*cb)(void*, void*), void* out, float range);
int fn_802A1D04(void);
void fn_80163824(int player, u32 id, int a, int b, int c);
void fn_8000F550(short* ids, int count);
int SendMsg(int target, int msg, void* data);
}

void* VAlloc(int size);
void VFree(void* p);
void Fn800F840C(int emitter, int a, int b);
void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
int Fn80175D60(unsigned long arg);
int AssetFind(u32 table, u32 key);
int PlayerGetPossessor(int arg);
void TextureLoad(short* ids, int count);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);
struct Player;
Player* GetPlayer(void);
void PlayerAnimOffUpdate(Player* p, float dt);

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

struct Vec3 {
    float x, y, z;
};

struct Mtx33 {
    float m[3][3];
};

struct Mtx34 {
    float m[3][4];
};

void MtxSetEuler(Mtx34* m, Vec3* rot);
void Vec3Set(Vec3* v, float x, float y, float z);

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

struct MsgResult {
    void* p;
    int unk4;
    u8 pad8[0x18];
};

struct ItemType {
    const char* name;
    int unk4[6];
    int (*msg)(int item, int msg, u32 owner, void* data);
    u32 (*id)(void);
    void (*update)(void);
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

// ---- Foam ----

void FoamUpdate(void);
u32 FoamId(void);
int FoamMsg(int item, int msg, u32 owner, void* data);

ItemType gFoamType = { "Foam", { 0 }, FoamMsg, FoamId, FoamUpdate };

void FoamUpdate(void) {}

u32 FoamId(void) { return 0x3DA260F7; }

int FoamMsg(int item, int msg, u32 owner, void* data) {
    Fn80175D60(owner);
    AssetFind(0xF2E2BEB8, 0xFAC4DCB6);

    switch (msg) {
    case 0x96224FCC:
        if (data) {
            if (*((int**)data)[1] == -1) {
                QueryMsg q1;
                QueryMsg q2;
                SpawnMsg m;
                Vec3 rot;

                q1.hash = 0x00012D80;
                SendMsg(0x61000000, 0x41E, &q1);
                *((int**)data)[1] = *(int*)&q1.result;
                q2.hash = 0x03F08D2D;
                *(int*)&q2.result = 0;
                SendMsg(*((int**)data)[1], 0x41D, &q2);
                m.owner = owner;
                m.asset = AssetFind(0xF1754BF2, StrHash("WP_FIRE1"));
                m.unkC = 1;
                Vec3Set(&rot, -90.0f, 0.0f, 0.0f);
                MtxSetEuler((Mtx34*)&m.rot, &rot);
                m.pos.x = 0.0f;
                m.pos.y = 0.0f;
                m.pos.z = 0.0f;
                SendMsg(*((int**)data)[1], 0x415, &m);
            }
            if (*((int**)data)[1]) {
                QueryMsg q;
                q.hash = 0x7A281983;
                *(int*)&q.result = 1;
                SendMsg(*((int**)data)[1], 0x41D, &q);
            }
        }
        return 1;
    case 0x0D19E338:
        if (data) {
            if (*((int**)data)[1]) {
                QueryMsg q;
                q.hash = 0x7A281983;
                *(int*)&q.result = 0;
                SendMsg(*((int**)data)[1], 0x41D, &q);
                q.hash = 0x03F08D2D;
                *(int*)&q.result = 0;
                SendMsg(*((int**)data)[1], 0x41D, &q);
            }
        }
        return 1;
    case 0x00014967:
        if (*((int**)data)[1]) {
            QueryMsg q;
            q.hash = 0x03F08D2D;
            *(int*)&q.result = 1;
            SendMsg(*((int**)data)[1], 0x41D, &q);
        }
        return 1;
    }
    return 0;
}

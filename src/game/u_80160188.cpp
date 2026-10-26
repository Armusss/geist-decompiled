// u_80160188.cpp: message handler of the held-weapon actor and the four callbacks that forward to its
// per-weapon node table (0x80160188-0x801608A4).
// - The handler answers two "node" queries (0x41E) sent to the owner, then dispatches the message against
//   the two queried node ids: it either reports a node change (Fn800F7F5C) or, for the "switch node"
//   message, walks the six hand-node assets one step per message (ModelNodeSetup).
// - The four small functions are the weapon type's callbacks: they look up the current node entry in
//   sWeaponNodes and forward to its three function pointers (or return its last field).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

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

// Message 0xC06: ask the target whether it knows the node for a given message.
struct NodeQuery {
    int unk0;
    int msg;
    int unk8;
    int target;
    int unk10[4];
};

// One entry of the per-weapon node table (0x48 bytes each, 25 entries at 0x803A7128).
struct WeaponNode {
    void (*fn0)(int);
    void (*fn1)(int, int);
    void (*fn2)(int, WeaponNode*);
    int unk0C[14];
    int unk44;
};

// The held-weapon actor. Only the fields this file touches are named.
struct Weapon {
    char pad0[0x38];
    int target;                                // 0x38
    char pad3C[0x18];
    int model;                                 // 0x54
    char pad58[0x74C];
    int state;                                 // 0x7A4
    int unk7A8;                                // 0x7A8
    char pad7AC[8];
    unsigned long flags;                       // 0x7B4
    char pad7B8[0xD44];
    float speed;                               // 0x14FC
    char pad1500[0x804];
    char buf[0x100];                           // 0x1D04
};

extern WeaponNode sWeaponNodes[25];            // 0x803A7128

extern "C" int SendMsg(int to, int msg, void* data);                 // 0x800265D4
int AssetFind(u32 table, u32 key);                        // 0x8007EB34
void ModelNodeSetup(int model, int node, int a, int b);   // 0x80038AE8
void* FxFind(const char* name);                           // 0x80170834
extern "C" void FxStart(void* fx, int a, int b);                     // 0x8016F390
int Fn800F7DEC(int a, int b, int c, int d, void* e);      // 0x800F7DEC
void Fn800F7F5C(int a, int b, int c, int d, void* e, float f); // 0x800F7F5C

extern "C" {
int fn_800C5210(int target, int msg, void* data);         // 0x800C5210
void fn_80048BA8(Weapon* w, void* buf, int flag);         // 0x80048BA8
int fn_800498D0(Weapon* w);                               // 0x800498D0
}
void PlayerSetActionMode(void* owner, int mode);
void PlayerSetHeldSlotMode(void* owner, int mode);

// One message, two queried node ids and the target they belong to.
static int sNodeIndex;
static char sNodeIndexInit;

// Out-parameter form: a returned value outranks the weapon pointer in register allocation.
inline void Query(int* out, int target, int msg, u32 hash) {
    QueryMsg m;

    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    SendMsg(target, msg, &m);
    *out = *(int*)&m.result;
}

// Message-handler style: the weapon arrives as void* and is cast to a local (retail keeps it in r31).
int WeaponMsg(int msg, void* p) {
    Weapon* w = (Weapon*)p;
    NodeQuery nq;

    nq.target = w->target;
    nq.msg = msg;
    if (fn_800C5210(w->target, 0xC06, &nq) == 1) {
        return 0;
    }

    int a;
    int b;
    Query(&a, w->target, 0x41E, 0x2DA8293C);
    Query(&b, w->target, 0x41E, 0xDFF3097A);

    if (!sNodeIndexInit) {
        sNodeIndex = 0;
        sNodeIndexInit = 1;
    }

    switch (msg) {
    case 0x1B83833F:
        if (w->state == 5) {
            int r = Fn800F7DEC(a, b, 0x0020EE36, fn_800498D0(w), 0);

            if (r != 0x04A3E183) {
                if (r == 0xC0BECF7E || r == 0x11D2C025) {
                    FxStart(FxFind("gun_empty"), 0, 0);
                }
                PlayerSetHeldSlotMode(w, 3);
            } else {
                PlayerSetActionMode(w, w->unk7A8);
            }
        } else if (w->state == 8) {
            break;
        } else if (w->state == 0xD) {
            w->flags |= 0x20000;
        } else {
            PlayerSetActionMode(w, w->unk7A8);
        }
        break;
    case 0x0020EE36:
        fn_80048BA8(w, w->buf, 1);
        Fn800F7F5C(a, b, msg, fn_800498D0(w), w->buf, w->speed);
        break;
    case 0xF829DB93:
        fn_80048BA8(w, w->buf, 0);
        Fn800F7F5C(a, b, msg, fn_800498D0(w), w->buf, w->speed);
        break;
    case 0x9791AF51:
    case 0x96224FCC:
    case 0x0D19E338:
    case 0x42E50A3D:
        fn_80048BA8(w, w->buf, 1);
        Fn800F7F5C(a, b, msg, fn_800498D0(w), w->buf, w->speed);
        break;
    case 0xE6B71BCC:
        fn_80048BA8(w, w->buf, 1);
        Fn800F7F5C(a, b, msg, fn_800498D0(w), w->buf, w->speed);
        break;
    case 0x968CBE3D:
        fn_80048BA8(w, w->buf, 1);
        Fn800F7F5C(a, b, msg, fn_800498D0(w), w->buf, w->speed);
        break;
    case 0xDAB493AB: {
        int nodes[6] = {0};

        nodes[0] = AssetFind(0xD9044CBB, 0xDE7F2683);
        nodes[1] = AssetFind(0xD9044CBB, 0xDE7F2684);
        nodes[2] = AssetFind(0xD9044CBB, 0xDE7F2685);
        nodes[3] = AssetFind(0xD9044CBB, 0xDE7F2686);
        nodes[4] = AssetFind(0xD9044CBB, 0xDE7F2687);
        nodes[5] = AssetFind(0xD9044CBB, 0xDE7F2688);
        ModelNodeSetup(w->model, nodes[sNodeIndex], 0, 0);
        if (++sNodeIndex == 6) {
            sNodeIndex = 0;
        }
        break;
    }
    case 0x365FF2D0: {
        int nodes[6] = {0};

        nodes[0] = AssetFind(0xD9044CBB, 0xDE7F2683);
        nodes[1] = AssetFind(0xD9044CBB, 0xDE7F2684);
        nodes[2] = AssetFind(0xD9044CBB, 0xDE7F2685);
        nodes[3] = AssetFind(0xD9044CBB, 0xDE7F2686);
        nodes[4] = AssetFind(0xD9044CBB, 0xDE7F2687);
        nodes[5] = AssetFind(0xD9044CBB, 0xDE7F2688);
        ModelNodeSetup(w->model, nodes[0], 1, 0);
        ModelNodeSetup(w->model, nodes[1], 1, 0);
        ModelNodeSetup(w->model, nodes[2], 1, 0);
        ModelNodeSetup(w->model, nodes[3], 1, 0);
        ModelNodeSetup(w->model, nodes[4], 1, 0);
        ModelNodeSetup(w->model, nodes[5], 1, 0);
        sNodeIndex = 0;
        break;
    }
    }
    return 0;
}

// Weapon node callbacks: forward to the current node entry.
int WeaponNodeValue(Weapon* w) {
    return sWeaponNodes[w->state].unk44;
}

void WeaponNodeFn2(Weapon* w) {
    if (sWeaponNodes[w->state].fn2) {
        sWeaponNodes[w->state].fn2((int)w, &sWeaponNodes[w->state]);
    }
}

void WeaponNodeFn1(Weapon* w, int arg) {
    if (sWeaponNodes[w->state].fn1) {
        sWeaponNodes[w->state].fn1((int)w, arg);
    }
}

void WeaponNodeFn0(Weapon* w) {
    WeaponNode* n = &sWeaponNodes[w->state];

    if (n->fn0) {
        n->fn0((int)w);
    }
}

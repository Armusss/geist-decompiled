// u_80277600.cpp: scene node list helpers and actor state helpers (0x80277600-0x8027D3EC).
// A linked list of scene nodes with lock / pin flags and position / velocity vectors, plus the
// per-actor state functions of a large actor class.  Names are descriptive guesses; only the tiny
// accessors are drafted so far, the rest are placeholders waiting to be drafted.

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

struct Vec3 {
    float x, y, z;
};

struct ListLink {
    int value;              // 0x00
    int unk04;              // 0x04
    ListLink* next;         // 0x08
};

struct SceneNode {
    u8 pad00[0xC];
    u32 owner;              // 0x00C
    u8 pad10[4];
    ListLink* list;         // 0x014
    u32 flags;              // 0x018
    u8 pad1C[0xCC];
    Vec3 pos;               // 0x0E8
    u8 padF4[4];
    Vec3 vel;               // 0x0F8
    u8 pad104[4];
    Vec3 accel;             // 0x108
    u8 pad114[4];
    Vec3 rotAdd;            // 0x118
    u8 pad124[0x1C];
    float prevA;            // 0x140
    float prevB;            // 0x144
    float curA;             // 0x148
    float curB;             // 0x14C
};

struct ActorBig {
    u8 pad00[0x50];
    u32 flags50;            // 0x050
    u8 pad54[0x44];
    int target;             // 0x098
    u8 pad9C[0x5DC];
    u32 flags678;           // 0x678
    u8 pad67C[0x128];
    int stateIndex;         // 0x7A4
    u8 pad7A8[0x28];
    int timer;              // 0x7D0
};

extern int lbl_803C05B4[][18];
extern float lbl_804D3780;

// Functions called from other units under their placeholder names.
extern "C" void fn_8027D3E4(float v);

// TODO (not drafted yet, 0xD4 bytes at 0x80277600)
extern "C" void fn_80277600(void);

// TODO (not drafted yet, 0xA4 bytes at 0x802776D4)
extern "C" void fn_802776D4(void);

// TODO (not drafted yet, 0x44 bytes at 0x80277778)
extern "C" void fn_80277778(void);

// TODO (not drafted yet, 0x48 bytes at 0x802777BC)
extern "C" void fn_802777BC(void);

// TODO (not drafted yet, 0x154 bytes at 0x80277804)
extern "C" void fn_80277804(void);

// TODO (not drafted yet, 0x1E0 bytes at 0x80277958)
extern "C" void fn_80277958(void);

// TODO (not drafted yet, 0x200 bytes at 0x80277B38)
extern "C" void fn_80277B38(void);

// TODO (not drafted yet, 0x48 bytes at 0x80277D38)
extern "C" void fn_80277D38(void);

// TODO (not drafted yet, 0x148 bytes at 0x80277D80)
extern "C" void fn_80277D80(void);

// TODO (not drafted yet, 0x204 bytes at 0x80277EC8)
extern "C" void fn_80277EC8(void);

// TODO (not drafted yet, 0x148 bytes at 0x802780CC)
extern "C" void fn_802780CC(void);

// TODO (not drafted yet, 0x148 bytes at 0x80278214)
extern "C" void fn_80278214(void);

void SceneNodeSetPinned(SceneNode* n, int pinned) {
    if (pinned != 0) {
        n->flags &= ~8;
        return;
    }
    n->flags |= 8;
}

u8 SceneNodeNotLocked(SceneNode* n) {
    return (n->flags >> 2 & 1) ^ 1;
}

void SceneNodeLock(SceneNode* n) {
    n->flags |= 4;
}

void SceneNodeSnapPrev(SceneNode* n) {
    n->flags &= ~4;
    n->curB = n->prevB;
    n->curA = n->prevA;
}

int SceneNodeListGet(SceneNode* n, int index) {
    ListLink* l = n->list;
    int i = 0;
    for (; l != NULL; l = l->next, i++) {
        if (i == index)
            return l->value;
    }
    return 0;
}

int SceneNodeListCount(SceneNode* n) {
    int count = 0;
    ListLink* l;
    for (l = n->list; l != NULL; l = l->next)
        count++;
    return count;
}

// TODO (not drafted yet, 0x118 bytes at 0x8027841C)
extern "C" void fn_8027841C(void);

// TODO (not drafted yet, 0xC4 bytes at 0x80278534)
extern "C" void fn_80278534(void);

// TODO (not drafted yet, 0xAC bytes at 0x802785F8)
extern "C" void fn_802785F8(void);

// TODO (not drafted yet, 0x7C bytes at 0x802786A4)
extern "C" void fn_802786A4(void);

// TODO (not drafted yet, 0x70 bytes at 0x80278720)
extern "C" void fn_80278720(void);

// TODO (not drafted yet, 0x70 bytes at 0x80278790)
extern "C" void fn_80278790(void);

void SceneNodeAddRot(float x, float y, float z, SceneNode* n) {
    n->rotAdd.x += x;
    n->rotAdd.y += y;
    n->rotAdd.z += z;
}

void SceneNodeAddAccel(float x, float y, float z, SceneNode* n) {
    n->accel.x += x;
    n->accel.y += y;
    n->accel.z += z;
}

// TODO (not drafted yet, 0x64 bytes at 0x80278850)
extern "C" void fn_80278850(void);

Vec3* SceneNodeVelocityPtr(SceneNode* n) {
    return &n->vel;
}

Vec3* SceneNodePosPtr(SceneNode* n) {
    return &n->pos;
}

void SceneNodeSetVelocity(float x, float y, float z, SceneNode* n) {
    n->vel.x = x;
    n->vel.y = y;
    n->vel.z = z;
}

void SceneNodeSetPos(float x, float y, float z, SceneNode* n) {
    n->pos.x = x;
    n->pos.y = y;
    n->pos.z = z;
}

// TODO (not drafted yet, 0x88 bytes at 0x802788E4)
extern "C" void fn_802788E4(void);

// TODO (not drafted yet, 0x54 bytes at 0x8027896C)
extern "C" void fn_8027896C(void);

u32 SceneNodeGetOwner(SceneNode* n) {
    return n->owner;
}

void SceneNodeSetOwner(SceneNode* n, u32 owner) {
    n->owner = owner;
}

// TODO (not drafted yet, 0x1A0 bytes at 0x802789D0)
extern "C" void fn_802789D0(void);

// TODO (not drafted yet, 0x1BC bytes at 0x80278B70)
extern "C" void fn_80278B70(void);

// TODO (not drafted yet, 0x2B4 bytes at 0x80278D2C)
extern "C" void fn_80278D2C(void);

// TODO (not drafted yet, 0x16C bytes at 0x80278FE0)
extern "C" void fn_80278FE0(void);

// TODO (not drafted yet, 0x40C bytes at 0x8027914C)
extern "C" void fn_8027914C(void);

// TODO (not drafted yet, 0x1468 bytes at 0x80279558)
extern "C" void fn_80279558(void);

// TODO (not drafted yet, 0xB38 bytes at 0x8027A9C0)
extern "C" void fn_8027A9C0(void);

// TODO (not drafted yet, 0x78 bytes at 0x8027B4F8)
extern "C" void fn_8027B4F8(void);

// TODO (not drafted yet, 0x530 bytes at 0x8027B570)
extern "C" void fn_8027B570(void);

// TODO (not drafted yet, 0xA8 bytes at 0x8027BAA0)
extern "C" void fn_8027BAA0(void);

// TODO (not drafted yet, 0x78 bytes at 0x8027BB48)
extern "C" void fn_8027BB48(void);

// TODO (not drafted yet, 0x78 bytes at 0x8027BBC0)
extern "C" void fn_8027BBC0(void);

// TODO (not drafted yet, 0x74 bytes at 0x8027BC38)
extern "C" void fn_8027BC38(void);

// TODO (not drafted yet, 0x74 bytes at 0x8027BCAC)
extern "C" void fn_8027BCAC(void);

// TODO (not drafted yet, 0x3C bytes at 0x8027BD20)
extern "C" void fn_8027BD20(void);

// TODO (not drafted yet, 0x4E4 bytes at 0x8027BD5C)
extern "C" void fn_8027BD5C(void);

// TODO (not drafted yet, 0x410 bytes at 0x8027C240)
extern "C" void fn_8027C240(void);

// TODO (not drafted yet, 0x9C bytes at 0x8027C650)
extern "C" void fn_8027C650(void);

// TODO (not drafted yet, 0x2C bytes at 0x8027C6EC)
extern "C" void fn_8027C6EC(void);

// TODO (not drafted yet, 0x80 bytes at 0x8027C718)
extern "C" void fn_8027C718(void);

// TODO (not drafted yet, 0xFC bytes at 0x8027C798)
extern "C" void fn_8027C798(void);

// TODO (not drafted yet, 0x78 bytes at 0x8027C894)
extern "C" void fn_8027C894(void);

// TODO (not drafted yet, 0x170 bytes at 0x8027C90C)
extern "C" void fn_8027C90C(void);

// TODO (not drafted yet, 0x60 bytes at 0x8027CA7C)
extern "C" void fn_8027CA7C(void);

// TODO (not drafted yet, 0x160 bytes at 0x8027CADC)
extern "C" void fn_8027CADC(void);

// TODO (not drafted yet, 0x2C bytes at 0x8027CC3C)
extern "C" void fn_8027CC3C(void);

// TODO (not drafted yet, 0x60 bytes at 0x8027CC68)
extern "C" void fn_8027CC68(void);

// TODO (not drafted yet, 0x15C bytes at 0x8027CCC8)
extern "C" void fn_8027CCC8(void);

void ActorClearFlag50(ActorBig* a) {
    a->flags50 &= ~0x40000;
}

// TODO (not drafted yet, 0x58 bytes at 0x8027CE34)
extern "C" void fn_8027CE34(void);

// TODO (not drafted yet, 0x110 bytes at 0x8027CE8C)
extern "C" void fn_8027CE8C(void);

// TODO (not drafted yet, 0x1A0 bytes at 0x8027CF9C)
extern "C" void fn_8027CF9C(void);

void ActorResetTarget(ActorBig* a) {
    a->target = -1;
}

// TODO (not drafted yet, 0x8C bytes at 0x8027D148)
extern "C" void fn_8027D148(void);

void ActorResetTargetAndTimer(ActorBig* a) {
    a->target = -1;
    a->timer = 0;
}

// TODO (not drafted yet, 0x5C bytes at 0x8027D1E8)
extern "C" void fn_8027D1E8(void);

// TODO (not drafted yet, 0x9C bytes at 0x8027D244)
extern "C" void fn_8027D244(void);

void ActorClearFlag678(ActorBig* a) {
    a->flags678 &= ~0x200;
}

void ActorSetFlag678(ActorBig* a) {
    a->flags678 |= 0x200;
}

int ActorGetStateValue(ActorBig* a) {
    return lbl_803C05B4[a->stateIndex][0];
}

// TODO (not drafted yet, 0x44 bytes at 0x8027D31C)
extern "C" void fn_8027D31C(void);

// TODO (not drafted yet, 0x44 bytes at 0x8027D360)
extern "C" void fn_8027D360(void);

// TODO (not drafted yet, 0x40 bytes at 0x8027D3A4)
extern "C" void fn_8027D3A4(void);

void fn_8027D3E4(float v) {
    lbl_804D3780 = v;
}

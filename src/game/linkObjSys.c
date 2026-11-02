// linkObjSys.c: object link system (0x802128E0-0x8021427C).
// Actors can be linked to a parent actor (or to a bone of it). Links live in a tree of ChildNode records
// (id of the linked actor, bone, flags, orientation and offset relative to the parent). Requests to link,
// relink or unlink are queued (0x100 entries of 0x44 bytes) and applied once per frame by the update; the
// per-node passes then keep the relative transform up to date. The tail of the file holds the host-ride
// state callbacks of the player that use the host's animation tables.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;

#define fabs(x) __fabs(x)

struct Vec3 {
    float x, y, z;
};

struct Mtx33 {
    float m[3][3];
};

// Orientation followed by the position (a full 3x4 matrix).
struct Mtx34 {
    Mtx33 rot;
    union {
        Vec3 pos;
        int ipos[3];
    };
};

struct PlaceF;
struct Bone;

// Orientation (3x3) followed by the position.
struct Place {
    Mtx33 rot;
    Vec3 pos;
};

// A linked actor and the bone of the parent it hangs on.
struct LinkRef {
    int id;    // 0x00
    int bone;  // 0x04
};

// Orientation (quaternion) and offset of a linked actor relative to its parent.
struct LinkPose {
    float rot[4];  // 0x00
    float pos[3];  // 0x10
};

// A linked actor in the tree.
struct ChildNode {
    ChildNode* mParent;  // 0x00
    ChildNode* mFirst;   // 0x04
    ChildNode* mLast;    // 0x08
    ChildNode* mNext;    // 0x0C
    ChildNode* mPrev;    // 0x10
    LinkRef mRef;        // 0x14 linked actor and the bone of the parent it hangs on (-1 = none)
    u32 mFlags;          // 0x1C 1 = relink, 2 = remove, 4/8 = keep offset, 0x10 = moved
    LinkPose mPose;      // 0x20 orientation (quaternion) and offset relative to the parent
};

struct ChildNodeTree;
struct BlockList;

// One queued link request (0x44 bytes).
struct LinkRequest {
    int mType;     // 0x00 0 = link with offset, 1 = link, 2 = unlink
    int mChild;    // 0x04
    int mParent;   // 0x08
    int mBone;     // 0x0C
    u32 mFlags;    // 0x10
    Place mPlace;  // 0x14
};

// From a shared header (class bodies are in other files).
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
    String(const char* s);
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

// Animation slot of the player (two of them, 0x58 bytes each, from 0x8C).
struct AnimSlot {
    int (*mMsg)(int, void*);  // 0x00
    u8 mUnk04[0x0C - 0x04];   // 0x04
    int mAnim;                // 0x0C
    u8 mUnk10[0x18 - 0x10];   // 0x10
    int mArg;                 // 0x18
    u8 mUnk1C[0x38 - 0x1C];   // 0x1C
    int mTarget;              // 0x38
    u8 mUnk3C[0x58 - 0x3C];   // 0x3C
};

// One of the two animation sets selected by PlayerInHost().
struct PlayerAnimSet {
    u8 mUnk00[0x14];          // 0x00
    int mUnk14;               // 0x14
    int mUnk18;               // 0x18
    int mUnk1C;               // 0x1C
};

// What OwnerGetInfo() returns for a linked actor id.
struct ActorInfo {
    u8 mUnk00[0x38];  // 0x00
    u32 mId;          // 0x38
    u8 mUnk3C[0x4C - 0x3C];  // 0x3C
    u32 mFlags;       // 0x4C 0x40 / 1 = moves by itself, 0x800 = linked
};

// Message 0x414: asks a moving actor to update for this frame.
struct LinkMoveMsg {
    ActorInfo* mOwner;  // 0x00
    float mDt;          // 0x04
    int mUnk8;          // 0x08
    int mUnkC;          // 0x0C
};

// Message 0x413: hands an actor its new world matrix.
struct LinkRotMsg {
    int mUnk0;        // 0x00
    Mtx34* mMtx;      // 0x04
};

// Reply buffer of the 0x411 placement query.
struct PlaceQuery {
    int mUnk0;
    int mBone;
    int mPad[2];
    Mtx34 mPlace;
};

struct Actor {
    u8 mUnk00[0x38];            // 0x00
    u32 mId;                    // 0x38
    u8 mUnk3C[0x50 - 0x3C];     // 0x3C
    u32 mFlags;                 // 0x50
    u8 mUnk54[0x58 - 0x54];     // 0x54
    int mModel;                 // 0x58
    u8 mUnk5C[0x78 - 0x5C];     // 0x5C
    int mUnk78;                 // 0x78
    u8 mUnk7C[0x8C - 0x7C];     // 0x7C
    AnimSlot mSlot[2];          // 0x8C
    u8 mUnk13C[0x270 - 0x13C];  // 0x13C
    u8 mConditions[0x638 - 0x270];  // 0x270
    int mUnk638;                // 0x638
    u8 mUnk63C[0x73C - 0x63C];  // 0x63C
    int mHostKind;              // 0x73C
    u8 mUnk740[0x7A4 - 0x740];  // 0x740
    u8 mValue[0x7B4 - 0x7A4];   // 0x7A4
    u32 mState;                 // 0x7B4
    u8 mUnk7B8[0x7D0 - 0x7B8];  // 0x7B8
    int mUnk7D0;                // 0x7D0
    u8 mUnk7D4[0x26B0 - 0x7D4];  // 0x7D4
    int mUnk26B0;               // 0x26B0
    u8 mUnk26B4[0x26C4 - 0x26B4];  // 0x26B4
    PlayerAnimSet* mAnimSet[2];  // 0x26C4
    u8 mUnk26CC[0x26E0 - 0x26CC];  // 0x26CC
    u8 mHostLink[4];            // 0x26E0
    Actor* mUnk26E4;            // 0x26E4
    Actor* mHost;               // 0x26E8
    u8 mUnk26EC[0x279C - 0x26EC];  // 0x26EC
    int mUnk279C;               // 0x279C
};

#ifdef __cplusplus
extern "C" {
#endif
void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);
void* memset(void* p, int c, unsigned long n);
int SendMsg(int to, int msg, void* data);
void OSReport(const char* fmt, ...);
ChildNode* fn_80242BA4(ChildNodeTree* tree);
void fn_8024311C(ChildNode* root, u8 (*visit)(ChildNode*, void*), void* arg);
ChildNode* fn_80243448(ChildNode* root, u8 (*visit)(ChildNode*, void*), void* arg);
void fn_800769A0(Actor* a, float dt);
int fn_80175EFC(int arg);
void fn_80155A84(void* link, Actor* host, int arg);
int fn_800A384C(Actor* a);
int fn_800A1BC8(int a, int b, int c, int d, int e);
void fn_80050EB0(Actor* a);
int fn_80049940(void);
void fn_8009F63C(Mtx34* out, Mtx34* in);
void fn_8009F44C(void* out, Mtx34* m, void* in);
void fn_800A025C(Vec3* out, Mtx34* m, int row);
void fn_800A0030(void* out, Mtx34* m, void* in);
void fn_800A0168(Mtx34* out, Mtx34* in);
void fn_8009FC6C(Mtx34* out, Mtx34* a, Mtx34* b);
#ifdef __cplusplus
}
#endif

void* VAlloc(int size);
ChildNodeTree* ChildNodeTreeInit(ChildNodeTree* tree, int size, int count);
ChildNode* BlockListAlloc(BlockList* list);
int SetChildNodeRootIfEmpty(ChildNode** root, ChildNode* node);
ChildNode* DetachChildNode(ChildNode** root, ChildNode* node);
ChildNode* ReparentChildNodes(ChildNode** root, ChildNode* dst, ChildNode* src);
void BoneSetMatrix(void* dst, Mtx34* src);
void LinkInfoInit(void* info);
void ActorGetMatrix(Mtx34* out, Actor* in);
void BoneWorldMatrix(Mtx34* out, Bone* rot, float* pos);
void PlacementToMatrix(Mtx34* out, PlaceF* in);
void MtxMulQ12(int* out, int* a, int* b);
ChildNode* AppendChildNode(ChildNode** root, ChildNode* parent, ChildNode* node);
void DetachChildNodeAndNotify(ChildNodeTree* tree, ChildNode* node);
ChildNodeTree* FreeChildNodeTree(ChildNodeTree* tree, short freeTree);
u8 WalkChildNodes(ChildNode* node, u8 (*visit)(ChildNode*, void*), void* arg);
ActorInfo* OwnerGetInfo(u32 id);
int HasCondition(void* conditions, int cond);
int PlayerInHost(Actor* a);
void PlayerSelectCollisionUpdate(void* player, float dt);
void PlayerSetValue4(void* value, int v);
void PlayerResetGunTimer(void* player);
void PlayerSetActionMode(void* player, int mode);
void PlayerSetHeldSlotIndex(void* player, int slot);
int ModelBoneName(int model, int bone);
int ModelBoneNodeChain(int model, int bone, int* chain, int count);
int GhostActionMsg(int msg, void* data);

// Animation tables of the host kinds (one entry per host kind), all in one .data object (0x803BC148).
struct HostAnimTables {
    u8 mUnk000[0x3B4];  // 0x000
    int mA[0x168 / 4];  // 0x3B4
    int mB[0x48 / 4];   // 0x51C
    int mC[0x90 / 4];   // 0x564
    int mD[0x114 / 4];  // 0x5F4
};
extern HostAnimTables lbl_803BC148;

static inline int* HostAnimEntry(int* table, int kind)
{
    return &table[kind];
}
extern int lbl_8049E260[];
extern const float lbl_804D94A8;  // 0.0f
extern const float lbl_804D94AC;
extern const float lbl_804D94B0;
extern const float lbl_804D94B4;  // 1.0f       // 0x8049E260

static ChildNodeTree* sLinkTree;
static LinkRequest* sLinkQueue;
static int sLinkQueueCount;

u8 LinkNodeMatchId(ChildNode* node, void* arg);
u8 LinkNodeRefresh(ChildNode* node, void* arg);
u8 LinkNodeCheck(ChildNode* node, void* arg);
u8 LinkNodeRemove(ChildNode* node, void* arg);
void LinkQueueFlush(void);
u8 LinkNodeCheck(ChildNode* node, void* arg);

// Not referenced in Geist (dead-stripped), but its strings open this file's string pool.
void LinkNodePrint(ChildNode* parent, const char* name, const char* pad)
{
    if (parent->mParent == 0) {
        OSReport("%s is attached to Scene Root\n", name);
    } else {
        if (name == 0) {
            name = "unknown";
        }
        OSReport("   ");
        OSReport(pad ? pad : "Scene Root\n");
        OSReport("%s is attached to\n", name);
    }
}

void LinkObjSysUpdate(float dt)
{
    LinkQueueFlush();
    fn_8024311C(fn_80242BA4(sLinkTree), LinkNodeRefresh, 0);
    fn_8024311C(fn_80242BA4(sLinkTree), LinkNodeCheck, &dt);
    WalkChildNodes(fn_80242BA4(sLinkTree), LinkNodeRemove, 0);
}

void LinkObjSysShutdown(void)
{
    DetachChildNodeAndNotify(sLinkTree, fn_80242BA4(sLinkTree));
    FreeChildNodeTree(sLinkTree, 1);
    if (sLinkQueue) {
        MemFree(sLinkQueue, "linkObjSys.c", 798);
    }
    sLinkQueue = 0;
}

void LinkObjSysInit(void)
{
    ChildNode* root;
    void* mem = VAlloc(0x1C);
    if (mem) {
        mem = ChildNodeTreeInit((ChildNodeTree*)mem, 0x28, 0x40);
    }
    sLinkTree = (ChildNodeTree*)mem;
    root = BlockListAlloc((BlockList*)sLinkTree);
    SetChildNodeRootIfEmpty((ChildNode**)sLinkTree, root);
    root->mRef.id = -1;
    root->mRef.bone = -1;
    sLinkQueue = (LinkRequest*)MemAlloc(0x4400, "linkObjSys.c", 781);
    if (sLinkQueue) {
        memset(sLinkQueue, 0, 0x4400);
    }
    sLinkQueueCount = 0;
}

// Queues a request to link child to parent (or, with parent -1, to unlink child). Returns whether it was queued.
int LinkQueueAdd(int child, int parent, int bone, Place* place, u8 allowReverse, u8 keepRot, u8 keepPos)
{
    int id;
    int parentId;
    int childId;
    childId = child;
    parentId = parent;
    if (child == -1) {
        return 0;
    }
    if (parent == -1) {
        ActorInfo* info = OwnerGetInfo(child);
        if (info && (info->mFlags & 0x800)) {
            ChildNode* node = fn_80243448(fn_80242BA4(sLinkTree), LinkNodeMatchId, &childId);
            if (node && node->mParent && node->mParent->mParent) {
                id = childId;
                if (id != -1) {
                    if (fn_80243448(fn_80242BA4(sLinkTree), LinkNodeMatchId, &id)) {
                        if (sLinkQueueCount < 0x100) {
                            LinkRequest* r = &sLinkQueue[sLinkQueueCount];
                            r->mType = 2;
                            r->mChild = id;
                            r->mParent = -1;
                            r->mBone = -1;
                            sLinkQueueCount++;
                        }
                    }
                }
            }
        }
        return 0;
    } else {
        ChildNode* childNode = fn_80243448(fn_80242BA4(sLinkTree), LinkNodeMatchId, &childId);
        ChildNode* parentNode = fn_80243448(fn_80242BA4(sLinkTree), LinkNodeMatchId, &parentId);
        if (childNode && parentNode) {
            if (childNode->mParent == parentNode) {
                return 0;
            }
            if (!allowReverse && parentNode->mParent == childNode) {
                return 0;
            }
        }
        if (sLinkQueueCount < 0x100) {
            LinkRequest* r = &sLinkQueue[sLinkQueueCount];
            r->mChild = childId;
            r->mParent = parentId;
            r->mBone = bone;
            r->mFlags = 0;
            r->mFlags |= keepRot ? 4 : 0;
            r->mFlags |= keepPos ? 8 : 0;
            if (place) {
                r->mType = 0;
                r->mPlace = *place;
            } else {
                r->mType = 1;
            }
            sLinkQueueCount++;
            return 1;
        }
        return 0;
    }
}

// Creates a node for a linked actor and marks the actor as linked.
static inline ChildNode* LinkNodeCreate(int id, int bone)
{
    ChildNode* node = BlockListAlloc((BlockList*)sLinkTree);
    node->mRef.id = id;
    node->mRef.bone = bone;
    node->mFlags = 0;
    node->mPose.rot[0] = lbl_804D94A8;
    node->mPose.rot[1] = lbl_804D94A8;
    node->mPose.rot[2] = lbl_804D94A8;
    node->mPose.rot[3] = lbl_804D94AC;
    node->mPose.pos[0] = lbl_804D94A8;
    node->mPose.pos[1] = lbl_804D94A8;
    node->mPose.pos[2] = lbl_804D94A8;
    if (id != -1) {
        ActorInfo* info = OwnerGetInfo(id);
        if (info) {
            info->mFlags |= 0x800;
        }
    }
    return node;
}

// Applies one link request: finds or creates both nodes and (re)attaches the child below the parent.
static inline void LinkApply(LinkRequest* r, Place* place)
{
    u32 flags = r->mFlags;
    int bone = r->mBone;
    int parentId = r->mParent;
    int childId = r->mChild;
    ChildNode* childNode = fn_80243448(fn_80242BA4(sLinkTree), LinkNodeMatchId, &childId);
    ChildNode* parentNode = fn_80243448(fn_80242BA4(sLinkTree), LinkNodeMatchId, &parentId);
    if (parentNode == 0) {
        parentNode = LinkNodeCreate(parentId, -1);
        AppendChildNode((ChildNode**)sLinkTree, fn_80242BA4(sLinkTree), parentNode);
    }
    if (childNode == 0) {
        childNode = LinkNodeCreate(childId, bone);
    } else {
        childNode = DetachChildNode((ChildNode**)sLinkTree, childNode);
    }
    parentNode->mFlags = 1;
    childNode->mRef.bone = bone;
    childNode->mFlags = flags;
    if (place) {
        BoneSetMatrix(childNode->mPose.rot, (Mtx34*)place);
        childNode->mPose.pos[0] = place->pos.x;
        childNode->mPose.pos[1] = place->pos.y;
        childNode->mPose.pos[2] = place->pos.z;
        childNode->mFlags = 0;
    } else {
        childNode->mFlags |= 1;
    }
    AppendChildNode((ChildNode**)sLinkTree, parentNode, childNode);
}

// Moves a linked actor's node back below the root.
static inline void LinkUnlink(LinkRequest* r)
{
    int id = r->mChild;
    if (id != -1) {
        ChildNode* node = fn_80243448(fn_80242BA4(sLinkTree), LinkNodeMatchId, &id);
        if (node) {
            node = DetachChildNode((ChildNode**)sLinkTree, node);
            if (node) {
                AppendChildNode((ChildNode**)sLinkTree, fn_80242BA4(sLinkTree), node);
            }
        }
    }
}

// Applies the queued requests and empties the queue.
void LinkQueueFlush(void)
{
    int i;
    for (i = 0; i != sLinkQueueCount; i++) {
        LinkRequest* r = &sLinkQueue[i];
        switch (r->mType) {
        case 1:
            LinkApply(r, 0);
            break;
        case 0:
            LinkApply(r, &r->mPlace);
            break;
        case 2:
            LinkUnlink(r);
            break;
        }
    }
    sLinkQueueCount = 0;
}

u8 LinkNodeRemove(ChildNode* node, void* arg)
{
    if (node->mParent == 0) {
        return 0;
    }
    if (node->mFlags & 2) {
        ActorInfo* owner = OwnerGetInfo(node->mRef.id);
        if (owner) {
            owner->mFlags &= ~0x800;
        }
        node = DetachChildNode((ChildNode**)sLinkTree, node);
        if (node) {
            ReparentChildNodes((ChildNode**)sLinkTree, fn_80242BA4(sLinkTree), node);
            DetachChildNodeAndNotify(sLinkTree, node);
        }
    }
    return 0;
}

// Whether an actor moves by itself (so a link to it has to ask it for its update).
static inline u8 LinkOwnerIsDynamic(ActorInfo* owner)
{
    u8 result = 0;
    if (owner != 0 && ((owner->mFlags & 0x40) || (owner->mFlags & 1) || (int)(owner->mId >> 24) == 0xC)) {
        result = 1;
    }
    return result;
}

// Do the orientations of two matrices agree (each row's dot product is about 1)?
static inline u8 LinkMatricesAgree(Mtx34* a, Mtx34* b)
{
    int i;
    if (fabs(a->pos.x - b->pos.x) > lbl_804D94B0 || fabs(a->pos.y - b->pos.y) > lbl_804D94B0 ||
        fabs(a->pos.z - b->pos.z) > lbl_804D94B0) {
        return 0;
    }
    for (i = 0; i != 3; i++) {
        Vec3 rowA;
        Vec3 rowB;
        fn_800A025C(&rowA, a, i);
        fn_800A025C(&rowB, b, i);
        if (fabs(lbl_804D94AC - (rowA.x * rowB.x + rowA.y * rowB.y + rowA.z * rowB.z)) > lbl_804D94B4) {
            return 0;
        }
    }
    return 1;
}

// Per-node pass: asks the linked actor whether it may keep its link, and flags the node when the
// actor has moved relative to the pose we stored (so the next pass rebases it).
u8 LinkNodeCheck(ChildNode* node, void* arg)
{
    ChildNode* parent = node->mParent;
    ActorInfo* owner;
    LinkMoveMsg moveMsg;
    PlaceQuery q1;
    PlaceQuery q2;
    PlaceQuery q3;
    PlaceQuery q4;
    Mtx34 parentMtx;
    Mtx34 childMtx;
    Mtx34 worldMtx;
    Mtx34 inv;
    Mtx34 rel;
    Mtx34 before;
    Mtx34 after;
    LinkRotMsg rotMsg;
    if (parent == 0) {
        return 0;
    }
    if (node->mFirst == 0 && parent != 0 && parent->mParent == 0) {
        node->mFlags |= 2;
        return 0;
    }
    owner = OwnerGetInfo(node->mRef.id);
    if (owner == 0) {
        node->mFlags |= 2;
        return 0;
    }
    if (parent->mParent != 0) {
        q1.mUnk0 = 0;
        q1.mBone = node->mRef.bone;
        if (SendMsg(parent->mRef.id, 0x411, &q1)) {
            ActorGetMatrix(&parentMtx, (Actor*)&q1.mPlace);
        } else {
            LinkInfoInit(&parentMtx);
        }
        BoneWorldMatrix(&childMtx, (Bone*)node->mPose.rot, node->mPose.pos);
        PlacementToMatrix(&rel, (PlaceF*)&parentMtx);
        PlacementToMatrix(&inv, (PlaceF*)&childMtx);
        fn_8009F63C(&before, &rel);
        fn_8009F44C(&inv.pos, &before, &inv.pos);
        inv.ipos[0] += rel.ipos[0];
        inv.ipos[1] += rel.ipos[1];
        inv.ipos[2] += rel.ipos[2];
        MtxMulQ12((int*)&inv, (int*)&inv, (int*)&rel);
        ActorGetMatrix(&childMtx, (Actor*)&inv);
        PlacementToMatrix(&worldMtx, (PlaceF*)&childMtx);
        rotMsg.mMtx = &worldMtx;
        SendMsg(node->mRef.id, 0x413, &rotMsg);
    }
    if (parent->mParent != 0) {
        q2.mUnk0 = 0;
        q2.mBone = -1;
        if (SendMsg(node->mRef.id, 0x411, &q2)) {
            ActorGetMatrix(&before, (Actor*)&q2.mPlace);
        } else {
            LinkInfoInit(&before);
        }
    }
    if (owner != 0) {
        if (LinkOwnerIsDynamic(owner)) {
            moveMsg.mOwner = owner;
            moveMsg.mDt = *(float*)arg;
            moveMsg.mUnk8 = 0;
            moveMsg.mUnkC = 0;
            if (!SendMsg(owner->mId, 0x414, &moveMsg)) {
                node->mFlags |= 2;
                return 0;
            }
        }
    }
    if (parent->mParent != 0) {
        q3.mUnk0 = 0;
        q3.mBone = -1;
        if (SendMsg(node->mRef.id, 0x411, &q3)) {
            ActorGetMatrix(&after, (Actor*)&q3.mPlace);
        } else {
            LinkInfoInit(&after);
        }
    }
    if (parent->mParent != 0) {
        if (!LinkMatricesAgree(&before, &after)) {
            node->mFlags |= 0x10;
        }
    }
    return 0;
}

// Resets a node's relative pose to identity.
static inline void LinkNodeResetPose(ChildNode* node)
{
    node->mPose.rot[0] = lbl_804D94A8;
    node->mPose.rot[1] = lbl_804D94A8;
    node->mPose.rot[2] = lbl_804D94A8;
    node->mPose.rot[3] = lbl_804D94AC;
    node->mPose.pos[0] = lbl_804D94A8;
    node->mPose.pos[1] = lbl_804D94A8;
    node->mPose.pos[2] = lbl_804D94A8;
}

// Asks an actor for its placement (0x411 query) and converts it to a matrix.
static inline int LinkQueryMatrix(Mtx34* out, LinkRef* ref, int bone)
{
    PlaceQuery q;
    int ok = 0;
    if (ref) {
        q.mUnk0 = 0;
        q.mBone = bone;
        if (SendMsg(ref->id, 0x411, &q)) {
            ActorGetMatrix(out, (Actor*)&q.mPlace);
            ok = 1;
        } else {
            LinkInfoInit(out);
            ok = 0;
        }
    }
    return ok;
}

// Recomputes a node's pose relative to its parent from the current world poses of both actors.
static inline void LinkNodeRebase(ChildNode* node)
{
    Mtx34 world;
    Mtx34 parentMtx;
    Mtx34 inv;
    LinkRef* ref = &node->mRef;
    ChildNode* parent;
    if (!LinkQueryMatrix(&world, ref, -1)) {
        LinkNodeResetPose(node);
        return;
    }
    parent = node->mParent;
    if (parent == 0 || parent->mParent == 0) {
        LinkInfoInit(&parentMtx);
    } else if (!LinkQueryMatrix(&parentMtx, &parent->mRef, ref->bone)) {
        LinkInfoInit(&parentMtx);
    }
    node->mPose.pos[0] = world.pos.x - parentMtx.pos.x;
    node->mPose.pos[1] = world.pos.y - parentMtx.pos.y;
    node->mPose.pos[2] = world.pos.z - parentMtx.pos.z;
    fn_800A0030(node->mPose.pos, &parentMtx, node->mPose.pos);
    fn_800A0168(&inv, &parentMtx);
    fn_8009FC6C(&world, &world, &inv);
    BoneSetMatrix(node->mPose.rot, &world);
}

// Per-node pass: brings the relative pose of a linked actor up to date (new link, or the parent moved).
u8 LinkNodeRefresh(ChildNode* node, void* arg)
{
    ChildNode* parent = node->mParent;
    if (parent == 0) {
        return 0;
    }
    if (parent != 0 && (u32)parent->mRef.id != 0xFFFFFFFF) {
        if (node->mFlags & 1) {
            LinkNodeRebase(node);
        } else if (node->mFlags & 0x10) {
            LinkPose saved = node->mPose;
            LinkNodeRebase(node);
            if (node->mFlags & 4) {
                node->mPose.pos[0] = saved.pos[0];
                node->mPose.pos[1] = saved.pos[1];
                node->mPose.pos[2] = saved.pos[2];
            }
            if (node->mFlags & 8) {
                node->mPose.rot[0] = saved.rot[0];
                node->mPose.rot[1] = saved.rot[1];
                node->mPose.rot[2] = saved.rot[2];
                node->mPose.rot[3] = saved.rot[3];
            }
        }
    }
    node->mFlags &= ~1;
    node->mFlags &= ~0x10;
    return 0;
}

u8 LinkNodeMatchId(ChildNode* node, void* arg)
{
    u32 id = *(u32*)arg;
    if (node->mParent != 0 && (u32)node->mRef.id == id) {
        return 1;
    }
    return 0;
}

void HostRideStateA(Actor* a, float dt)
{
    a->mUnk638 = 0;
    a->mState &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    if (a->mSlot[0].mTarget == -1) {
        if (a->mUnk7D0 != 0) {
            PlayerSetValue4(a->mValue, 3);
        } else {
            PlayerSetValue4(a->mValue, 1);
        }
    } else {
        PlayerSetValue4(a->mValue, 22);
    }
}

void HostRideSlotsSync(Actor* a)
{
    a->mSlot[0].mMsg = 0;
    if (a->mSlot[0].mTarget != -1) {
        a->mSlot[0].mAnim = a->mSlot[0].mTarget;
    }
    if (a->mSlot[1].mTarget == -1) {
        return;
    }
    a->mSlot[1].mAnim = a->mSlot[1].mTarget;
}

void HostRideStateB(Actor* a)
{
    PlayerResetGunTimer(a);
    a->mUnk638 = 1;
    a->mState &= ~1;
    PlayerSetValue4(a->mValue, 21);
    if (a->mUnk279C == -1 || fn_80175EFC(a->mUnk279C) != 0) {
        PlayerSetValue4(a->mValue, 3);
        fn_80155A84(a->mHostLink, a->mHost, 1);
    }
}

void HostRideEnterA(Actor* a)
{
    Actor* host = a->mHost;
    PlayerAnimSet* set;
    int hasCond = 0;
    int* table;
    if ((int)(host->mId >> 24) == 0x13) {
        set = (PlayerAnimSet*)fn_800A384C(host);
        if (HasCondition(host->mConditions, 11)) {
            hasCond = 1;
        }
    }
    PlayerInHost(a);
    table = HostAnimEntry(lbl_803BC148.mD, a->mHostKind);
    a->mSlot[0].mAnim = fn_800A1BC8(set->mUnk14, set->mUnk18, *table, hasCond, 0);
    a->mSlot[0].mMsg = GhostActionMsg;
    a->mSlot[0].mArg = 0x19;
    a->mSlot[1].mAnim = fn_800A1BC8(set->mUnk14, set->mUnk1C, *table, hasCond, 0);
    a->mSlot[1].mArg = 0x19;
    PlayerSetValue4(a->mValue, 21);
}

void HostRideStateC(Actor* a, float dt)
{
    PlayerResetGunTimer(a);
    a->mUnk638 = 1;
    a->mState &= ~1;
    fn_800769A0(a, dt);
    PlayerSelectCollisionUpdate(a, dt);
    PlayerSetValue4(a->mValue, 1);
    PlayerInHost(a);
}

void HostRideEnterB(Actor* a)
{
    a->mSlot[0].mAnim = -1;
    a->mSlot[1].mAnim = -1;
    if (fn_80049940() != 0) {
        PlayerSetActionMode(a, 3);
    } else {
        PlayerSetActionMode(a, 1);
    }
    QueryMsg q;
    q.hash = 0x08783452;
    SendMsg(a->mUnk26E4->mId, 0x41E, &q);
    if (*(int*)&q.result != 0) {
        PlayerSetHeldSlotIndex(a, 0);
        if (a->mSlot[0].mAnim == -1) {
            PlayerSetActionMode(a, 1);
        }
    } else {
        PlayerSetHeldSlotIndex(a, 1);
        if (a->mSlot[0].mAnim == -1) {
            PlayerSetActionMode(a, 3);
        }
    }
}

void HostRideStateD(Actor* a)
{
    PlayerResetGunTimer(a);
    a->mUnk638 = 1;
    if (a->mState & 1) {
        a->mState &= ~1;
    } else {
        a->mFlags |= 0x20000;
        a->mUnk78 = 1;
    }
}

void HostRideEnterC(void* p)
{
    Actor* a = (Actor*)p;
    Actor* host = a->mHost;
    int* table;
    PlayerAnimSet* set;
    int hasCond = 0;
    int inHost;
    if ((int)(host->mId >> 24) == 0x13) {
        set = (PlayerAnimSet*)fn_800A384C(host);
        if (HasCondition(host->mConditions, 11)) {
            hasCond = 1;
        }
    }
    inHost = PlayerInHost(a);
    table = HostAnimEntry(lbl_803BC148.mC, a->mHostKind);
    a->mSlot[0].mAnim = fn_800A1BC8(set->mUnk14, set->mUnk18, *table, hasCond, 0);
    a->mSlot[0].mMsg = GhostActionMsg;
    a->mSlot[0].mArg = 0x19;
    a->mSlot[1].mAnim = fn_800A1BC8(a->mAnimSet[inHost]->mUnk14, a->mAnimSet[inHost]->mUnk1C, *table, 0, 0);
    a->mSlot[1].mArg = 0x19;
    QueryMsg q;
    q.hash = 0x08783452;
    SendMsg(a->mHost->mId, 0x41E, &q);
    if (*(int*)&q.result != 0) {
        PlayerSetHeldSlotIndex(a, 0);
    } else {
        PlayerSetHeldSlotIndex(a, 1);
    }
}

void HostRideHostLink(Actor* a)
{
    fn_80155A84(a->mHostLink, a->mHost, 1);
}

void HostRideStateE(Actor* a)
{
    PlayerResetGunTimer(a);
    a->mUnk638 = 1;
    a->mState &= ~1;
    a->mFlags |= 0x20000;
    a->mUnk78 = 1;
    PlayerSetValue4(a->mValue, 19);
}

void HostRideEnterD(void* p)
{
    Actor* a = (Actor*)p;
    int inHost = PlayerInHost(a);
    Actor* host = a->mHost;
    int kind = a->mHostKind;
    int hasCond = 0;
    PlayerAnimSet* set;
    int* table;
    if ((int)(host->mId >> 24) == 0x13) {
        set = (PlayerAnimSet*)fn_800A384C(host);
        if (HasCondition(host->mConditions, 11)) {
            hasCond = 1;
        }
    }
    table = HostAnimEntry(lbl_803BC148.mB, kind);
    a->mSlot[0].mAnim = fn_800A1BC8(set->mUnk14, set->mUnk18, *table, hasCond, 0);
    a->mSlot[0].mMsg = GhostActionMsg;
    a->mSlot[0].mArg = 0x59;
    a->mSlot[1].mAnim = fn_800A1BC8(a->mAnimSet[inHost]->mUnk14, a->mAnimSet[inHost]->mUnk1C, *table, 0, 0);
    a->mSlot[1].mArg = 0x59;
    QueryMsg q;
    q.hash = 0x08783452;
    SendMsg(a->mHost->mId, 0x41E, &q);
    if (*(int*)&q.result != 0) {
        PlayerSetHeldSlotIndex(a, 0);
    } else {
        PlayerSetHeldSlotIndex(a, 1);
    }
}

void HostRideClearFlag(Actor* a)
{
    a->mFlags &= ~0x40000;
}

void HostRideStateF(Actor* a, float dt)
{
    PlayerResetGunTimer(a);
    a->mUnk638 = 1;
    a->mState &= ~1;
    PlayerSelectCollisionUpdate(a, dt);
}

void HostRideEnterE(Actor* a)
{
    int inHost;
    int kind;
    PlayerAnimSet* set;
    int* table;
    int bone;
    PlayerResetGunTimer(a);
    a->mFlags |= 0x40000;
    fn_80050EB0(a);
    inHost = PlayerInHost(a);
    kind = a->mHostKind;
    set = a->mAnimSet[inHost];
    table = HostAnimEntry(lbl_803BC148.mA, kind);
    a->mSlot[0].mAnim = fn_800A1BC8(set->mUnk14, set->mUnk18, *table, 0, 0);
    a->mSlot[0].mArg = 0x50;
    if (a->mUnk26B0 == -1) {
        bone = 8;
    } else {
        bone = ModelBoneNodeChain(a->mModel, ModelBoneName(a->mModel, a->mUnk26B0), lbl_8049E260, 0x10);
        if (bone == -1) {
            bone = 8;
        }
    }
    set = a->mAnimSet[inHost];
    a->mSlot[1].mAnim = fn_800A1BC8(set->mUnk14, set->mUnk1C, *table, bone << 1, 0);
    a->mSlot[1].mMsg = GhostActionMsg;
    a->mSlot[1].mArg = 0x50;
}

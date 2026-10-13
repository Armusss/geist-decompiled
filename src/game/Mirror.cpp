// Mirror.cpp: the "Mirror" actor type (0x80285670-0x80285ACC).
// A possessable mirror: when triggered it casts a reflected shot from its model and notifies the
// possessing ghost.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct BoneXform {
    Mtx34 m;
    Vec3 scale;
};

struct BoneOrigin {
    Vec3 pos;
    Vec3 unkC;
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

struct MirrorData {
    char pad0[0xC];
    int ghost;
    char pad10[8];
    unsigned long flags;
    char pad1C[0x14];
};

struct Bone {
    char pad0[0x10];
    float x, y, z;
};

struct Actor {
    char pad0[0x38];
    int handle;
    char pad3C[0x54];
    int model;
    char pad94[8];
    Bone* bones;
    char padA0[4];
    MirrorData* data;
    char padA8[4];
    unsigned long flags;
};

struct NotifyMsg {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
};

struct MsgResult {
    char* p;
    char pad[0x1C];
};

// Shot description filled by ActorAim.
struct MirrorShot {
    float range;
    int unk4;
    u32 type;
    int pos[3];
    char pad18[0xC];
    int dir[3];
    char pad30[8];
    int bone;
    int flags;
    char pad40[0x10];
};

int SendMsg(int target, int msg, void* data);            // 0x800265D4
void* VAlloc(int size);
extern "C" void* memset(void* dst, int val, unsigned long n);
void PossessableDestroy(Actor* a);                       // 0x8020D4E0
int PossessableInit(Actor* a);                           // 0x8020D554
int PossessableMsg(Actor* a, int msg, void* data, int arg); // 0x8020D738
void ActorRender(Actor* a);                              // 0x800C7CE8
void ActorAim(int* dir, Actor* a, int a2);               // 0x8009F700
int ModelBoneCount(int model);                           // 0x80037BEC
int ModelBoneName(int model, int bone);                  // 0x80018B44
int ModelNodeFind(int model, int name);                  // 0x80037464
Vec3* ModelBonePositions(int model);                     // 0x8003A14C
void Vec3Copy(Vec3* out, Vec3* in);                      // 0x8016D7A8
void BoneWorldMatrix(Mtx34* out, Bone* bone, float* pos); // 0x800A09A8
void ModelNodeSetup(int model, int node, int a, int b);  // 0x80038AE8
void ShotFire(Actor* a, int model, int node, float speed, float size, Vec3* org, Mtx34* m, Vec3* pos, Vec3* dir); // 0x80231984
void GhostNotify(void* ghost, int a, int b);             // 0x80154594

int MirrorMsg(Actor* a, int msg, void* data, int arg);
void MirrorUpdate(Actor* a, float dt);
int MirrorInit(Actor* a);
void MirrorDestroy(Actor* a);

ActorType gMirrorType = { 0, "Mirror", MirrorMsg, MirrorUpdate, MirrorInit, 0, MirrorDestroy };

void MirrorDestroy(Actor* a) { PossessableDestroy(a); }

void MirrorUpdate(Actor* a, float dt) {
    if (a->data->flags & 0x20000000) {
        ActorRender(a);
    }
}

int MirrorInit(Actor* a) {
    MirrorData* d = a->data;
    if (d == 0) a->data = d = (MirrorData*)VAlloc(sizeof(MirrorData));
    memset(d, 0, sizeof(MirrorData));
    if (!PossessableInit(a)) {
        a->flags |= 0x01000000;
        return 0;
    }
    a->flags &= ~0x20000000;
    return 1;
}

void MirrorReflect(Actor* a);

static inline int MirrorOnTrigger(void* data) {
    Actor* actor = *(Actor**)data;

    switch (((int*)data)[3]) {
    case 0xCF0164E2:
        MirrorReflect(actor);
        return 1;
    }
    return 0;
}

int MirrorMsg(Actor* a, int msg, void* data, int arg) {
    int ret = 0;

    switch (msg) {
    case 0xC14:
        ret = MirrorOnTrigger(data);
        break;
    case 0x41D:
        ret = 0;
        break;
    }

    if (ret == -999999) return 0;
    if (ret) return ret;
    return PossessableMsg(a, msg, data, arg);
}

void MirrorReflect(Actor* a) {
    NotifyMsg n;
    Vec3 pos;
    Vec3 dir;
    BoneOrigin org;
    MsgResult res;
    MirrorShot shot;
    BoneXform m;
    MirrorData* d = a->data;
    int bone;
    int node;

    n.unk4 = 0;
    SendMsg(a->handle, 0xC00, &n);

    shot.type = 0x75715902;
    shot.range = 1000.0f;
    shot.flags = 0x200;
    shot.bone = 0;
    shot.pos[0] = a->bones->x;
    shot.pos[1] = a->bones->y;
    shot.pos[2] = a->bones->z;
    shot.pos[2] += 200;
    ActorAim(shot.dir, a, 1);
    shot.dir[0] = -shot.dir[0];
    shot.dir[1] = -shot.dir[1];
    shot.dir[2] = -shot.dir[2];
    shot.dir[0] = 0x2000;

    bone = shot.bone;
    if (bone >= 0 && bone < ModelBoneCount(a->model)) {
        int name = ModelBoneName(a->model, bone);
        node = ModelNodeFind(a->model, name);
        Vec3Copy(&org.pos, ModelBonePositions(a->model) + bone);
        BoneWorldMatrix(&m.m, a->bones + bone, &a->bones[bone].x);
        pos.x = shot.pos[0];
        pos.y = shot.pos[1];
        pos.z = shot.pos[2];
        dir.x = 0.000244140625f * shot.dir[0];
        dir.y = 0.000244140625f * shot.dir[1];
        dir.z = 0.000244140625f * shot.dir[2];
        ModelNodeSetup(a->model, node, 0, 1);
        ShotFire(a, a->model, node, 8.0f, 128.0f, &org.pos, &m.m, &pos, &dir);
    }

    if (d->ghost != -1 && (int)((u32)d->ghost >> 24) == 2) {
        SendMsg(d->ghost, 0x400, &res);
        GhostNotify(res.p + 0x26E0, 0, 0);
    }
    d->flags |= 0x20000000;
}

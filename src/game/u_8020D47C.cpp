// u_8020D47C.cpp: possession select states, possessable update/messages and shot firing (0x8020D47C-0x80210D80).
// Names are descriptive guesses; the code is matched against the retail DOL.
// Function and field names are descriptive guesses based on what the code does.

// Small math types used by the shot-firing path.
struct Vec3 {
    float x;                           // 0x00
    float y;                           // 0x04
    float z;                           // 0x08
};

struct Mtx34 {
    float m[12];                       // 0x00
};

struct Bone {
    unsigned char unk0[0x10];          // 0x00
    float unk10;                       // 0x10, start of the rotation
    unsigned char unk14[0x1C - 0x14];  // 0x14
};

// Engine text helper: String derives from Buffer. The constructor and the base
// destructor live in another unit; only the layout is mirrored here.
class Buffer {
public:
    virtual ~Buffer();                 // vptr at 0x00
};

class String : public Buffer {
public:
    String();
    int unk04;                         // 0x04
    int unk08;                         // 0x08
};

// Object that owns a possession state block; only the fields the handlers
// touch are named.
struct PossessOwner {
    int unk0;                          // 0x00
    unsigned char unk4[0x12C - 0x04];  // 0x04
    int unk12C;                        // 0x12C
    int unk130;                        // 0x130
};

// Possession record hung off an actor at 0xA4.
struct PossessionInfo {
    void* owner;                      // 0x00, freed with VFree on destroy
    int handle;                       // 0x04
    int unk8;                         // 0x08
    int unkC;                         // 0x0C
    float unk10;                      // 0x10
    float unk14;                      // 0x14
    unsigned int flags;               // 0x18
    int unk1C;                        // 0x1C
    int unk20;                        // 0x20
    float unk24;                      // 0x24
    float unk28;                      // 0x28
    int unk2C;                        // 0x2C
};

// Per-actor data block reached through Actor::data.
struct ActorData {
    int unk00;                         // 0x00
    unsigned char unk04[0x18 - 0x04];  // 0x04
    unsigned long unk18;               // 0x18
    void* unk1C;                       // 0x1C
    unsigned char unk20[0x30 - 0x20];  // 0x20
    float unk30;                       // 0x30
    unsigned char unk34[0x48 - 0x34];  // 0x34
    float unk48;                       // 0x48
    unsigned char unk4C[0x80 - 0x4C];  // 0x4C
    int unk80;                         // 0x80, model id
    int unk84;                         // 0x84
    int unk88;                         // 0x88
    int unk8C;                         // 0x8C
    int unk90;                         // 0x90
    int unk94;                         // 0x94
    int unk98;                         // 0x98
    unsigned char unk9C[0xA8 - 0x9C];  // 0x9C
    int unkA8;                         // 0xA8
    unsigned char unkAC[0xB0 - 0xAC];  // 0xAC
    int unkB0;                         // 0xB0
    int unkB4;                         // 0xB4
};

class Actor {
public:
    unsigned char unk0[0x38];          // 0x00
    int unk38;                         // 0x38
    unsigned char unk3C[0x4C - 0x3C];  // 0x3C
    unsigned int unk4C;                // 0x4C, possession flags
    unsigned char unk50[0x84 - 0x50];  // 0x50
    ActorData* data;                   // 0x84
    unsigned char unk88[0x90 - 0x88];  // 0x88
    int unk90;                         // 0x90, model handle
    int unk94;                         // 0x94, skin/handle id
    int unk98;                         // 0x98
    Bone* bones;                       // 0x9C, bone array (stride 0x1C)
    unsigned char unkA0[0xA4 - 0xA0];  // 0xA0
    PossessionInfo* possess;           // 0xA4
    unsigned char unkA8[0xAC - 0xA8];  // 0xA8
    unsigned int unkAC;                // 0xAC
};

extern "C" float lbl_804D93E8;
extern "C" float lbl_804D93EC;
extern "C" float lbl_804D93F0;
extern "C" float lbl_804D93F4;

extern "C" void* memset(void*, int, unsigned long);
void VFree(void* ptr);
void* VAlloc(int size);
int ActorInitBase(Actor* actor);
extern "C" void fn_800F86A0(int handle);
extern "C" int fn_800F8A30(int id);
unsigned long Fn800F8A98(unsigned long id);
int Fn800F840C(int handle, int id, int mode);
extern "C" int fn_800EB1B8(unsigned long id, int index);
void ActorUpdate(Actor* actor, float dt);

// Message payloads: every possession message starts with the receiving actor.
struct PossessMsg {
    Actor* actor;                      // 0x00
    int value;                         // 0x04
    unsigned int flags;                // 0x08
};

// 0x41D/0x41E: hash-keyed possession query. The second word is a message hash
// and the fourth carries the value in or out.
struct PossessQuery {
    Actor* actor;                      // 0x00
    unsigned int msg;                  // 0x04
    unsigned int unk08;                // 0x08
    unsigned int value;                // 0x0C
};

// 0x400: activation state.
struct PossessActiveMsg {
    Actor* actor;                      // 0x00
    int handle;                        // 0x04
    int unk08;                         // 0x08
    int active;                        // 0x0C
    float progress;                    // 0x10
    float maxProgress;                 // 0x14
    int mode;                          // 0x18
    Actor* newOwner;                   // 0x1C
};

// 0x401: current possession progress.
struct PossessProgressMsg {
    Actor* actor;                      // 0x00
    unsigned char unk04[0x10 - 0x04];
    float progress;                    // 0x10
    float maxProgress;                 // 0x14
    float unk18;                       // 0x18
    float unk1C;                       // 0x1C
};

// 0x407: bone-targeted possession shot.
struct PossessFireMsg {
    Actor* actor;                      // 0x00
    int unk04;                         // 0x04
    int anim;                          // 0x08
    float damage;                      // 0x0C
    float progress;                    // 0x10
    int type;                          // 0x14
    int unk18;                         // 0x18
    int unk1C;                         // 0x1C
    int unk20;                         // 0x20
    unsigned char unk24[0x30 - 0x24];
    int unk30;                         // 0x30
    int unk34;                         // 0x34
    int unk38;                         // 0x38
    unsigned char unk3C[0x44 - 0x3C];
    int bone;                          // 0x44
};

extern "C" float lbl_804D93F8;
extern "C" float lbl_804D93FC;
extern "C" float lbl_804D9400;

int ModelBoneName(int model, int bone);
int ModelBoneCount(int model);
int ModelNodeFind(int model, int name);
Vec3* ModelBonePositions(int model);
void Vec3Copy(Vec3* dst, Vec3* src);
void BoneWorldMatrix(Mtx34* dst, Bone* bone, float* rot);
void ModelNodeSetup(int model, int node, int a, int b);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);
void TextureLoad(short* ids, int count);
int ActorIsActive(Actor* actor);
int ActorPlayAnim(int handle, unsigned long id, int arg);
int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg);
void ShotFire(Actor* actor, int model, int node, float f1, float f2,
              Vec3* pos, Mtx34* mtx, Vec3* off1, Vec3* off2);
extern "C" int fn_80037230(int model, int name);
extern "C" void fn_8000F550(short* ids, int count);
extern "C" void fn_801FAD18(Actor* actor, int mode);

int PossessShotFire(void* data, int a, int b);
int PossessAnimQuery(void* data);
int PossessStateEvent(void* data);

// Still called from other units under their placeholder names, so the C names
// are kept for now.
extern "C" int fn_8020DD08(int* data);
extern "C" int fn_8020DF98(int* data);

int GameEvent(int type, int id);
extern "C" void fn_80283864(int state);
extern "C" void fn_80283A44(int state);
extern "C" void fn_80283DC4(int state);
extern "C" void fn_8028BF80(String* text, const char* format, ...);
extern "C" void fn_8028C188(unsigned int* out, int a, int b);

// fn_8020D47C: blend rate used while possession is active. Still called from
// other units under its placeholder name, so the C name is kept for now.
extern "C" float fn_8020D47C(Actor* actor) {
    if (actor == 0 || actor->possess == 0) {
        return lbl_804D93E8;
    }
    unsigned int flags = actor->possess->flags;
    if ((flags & 2) && (flags & 4)) {
        return lbl_804D93E8;
    }
    return lbl_804D93EC;
}

// fn_8020D4BC: id of the actor that currently possesses this one, or 0.
extern "C" int fn_8020D4BC(Actor* actor) {
    if (actor == 0 || actor->possess == 0) {
        return 0;
    }
    return (int)actor->possess->owner;
}

// Releases the possession record hung off the actor and the payload it owns.
void PossessableDestroy(Actor* actor) {
    PossessionInfo* info = actor->possess;
    if (info != 0) {
        fn_800F86A0(info->handle);
        info->handle = -1;
        if (info->owner != 0) {
            VFree(info->owner);
            info->owner = 0;
        }
        VFree(actor->possess);
        actor->possess = 0;
    }
}

// Allocates and fills the possession record used by the possession system.
int PossessableInit(Actor* actor) {
    if (ActorInitBase(actor) == 0) {
        actor->unkAC |= 0x1000000;
        return 0;
    }

    PossessionInfo* info = actor->possess;
    if (info == 0) {
        info = (PossessionInfo*)VAlloc(0x30);
        actor->possess = info;
        memset(info, 0, 0x30);
    }

    info->unk10 = actor->data->unk30;
    info->unkC = -1;
    info->unk14 = lbl_804D93F0;
    info->flags |= 0xA;
    info->flags &= ~0x10;
    info->unk1C = 0;
    info->unk20 = 0;
    if (actor->data->unk1C != 0) {
        info->flags |= 4;
    }

    info->handle = fn_800F8A30(actor->unk38);
    if (actor->data->unk18 != 0) {
        unsigned long id = Fn800F8A98(actor->data->unk18);
        if (Fn800F840C(info->handle, id, 1) != 0) {
            info->unk8 = id;
            int slot = fn_800EB1B8(id, 0);
            if (slot != -1) {
                Fn800F840C(info->handle, slot, 9999);
            }
            slot = fn_800EB1B8(id, 1);
            if (slot != -1) {
                Fn800F840C(info->handle, slot, 9999);
            }
        }
    }
    return 1;
}

// PossessableUpdate__FP5Actorf: advances the possession timers, then the base
// actor update.
void PossessableUpdate(Actor* actor, float dt) {
    PossessionInfo* info = actor->possess;
    if (info->unk14 <= 600.0f) {
        info->unk14 += dt;
    }
    float zero = 0.0f;
    if (zero != info->unk28) {
        info->unk24 += dt;
        if (info->unk24 >= info->unk28) {
            info->unk28 = zero;
            info->unk24 = zero;
        }
    }
    ActorUpdate(actor, dt);
}

// PossessableMsg__FP5ActoriPvi: possession message handler.
int PossessableMsg(Actor* actor, int msg, void* data, int arg) {
    PossessMsg* d = (PossessMsg*)data;
    Actor* owner = d->actor;
    PossessionInfo* info = owner->possess;
    unsigned int result = 0;

    switch (msg) {
    case 0x400: {
        PossessActiveMsg* m = (PossessActiveMsg*)data;
        m->handle = owner->unk38;
        m->active = (info->unk10 > 0.0f);
        m->progress = ActorIsActive(owner) ? lbl_804D93EC : lbl_804D93E8;
        m->maxProgress = lbl_804D93E8;
        m->newOwner = owner;
        m->unk08 = 0;
        m->mode = 2;
        result = 1;
        break;
    }
    case 0x401: {
        PossessProgressMsg* m = (PossessProgressMsg*)data;
        m->progress = info->unk10;
        m->maxProgress = owner->data->unk30;
        m->unk18 = lbl_804D93E8;
        m->unk1C = lbl_804D93E8;
        result = 1;
        break;
    }
    case 0x407: {
        PossessFireMsg* m = (PossessFireMsg*)data;
        if (m->type != 0x6CA5F172) {
            if (m->type == 0x403548D0) {
                info->flags |= 2;
                m->damage = lbl_804D93E8;
            } else {
                int bone = m->bone;
                if ((fn_80037230(owner->unk90, ModelBoneName(owner->unk90, bone)) & 0x200) != 0 &&
                    bone >= 0 && bone < ModelBoneCount(owner->unk90)) {
                    Vec3 pos;
                    Mtx34 mtx;
                    Vec3 offset1;
                    Vec3 offset2;
                    Bone* bonePtr = &owner->bones[bone];
                    int node = ModelNodeFind(owner->unk90, ModelBoneName(owner->unk90, bone));

                    Vec3Copy(&pos, ModelBonePositions(owner->unk90) + bone);
                    BoneWorldMatrix(&mtx, bonePtr, &bonePtr->unk10);
                    offset1.x = (float)m->unk18;
                    offset1.y = (float)m->unk1C;
                    offset1.z = (float)m->unk20;
                    offset2.x = lbl_804D93F8 * (float)m->unk30;
                    offset2.y = lbl_804D93F8 * (float)m->unk34;
                    offset2.z = lbl_804D93F8 * (float)m->unk38;
                    ModelNodeSetup(owner->unk90, node, 0, 1);
                    ShotFire(owner, owner->unk90, node, lbl_804D93FC, lbl_804D9400,
                             &pos, &mtx, &offset1, &offset2);
                }
            }
        }
        if (info->unk10 > 0.0f && (info->flags & 8) == 0) {
            info->unk10 -= m->damage;
            if (info->unk10 <= 0.0f) {
                ActorPlayAnim(owner->unk38, 0xDC04E3D5, m->anim);
            }
            float v = info->unk10;
            if (v < 0.0f) {
                v = lbl_804D93E8;
            } else if (v > owner->data->unk30) {
                v = owner->data->unk30;
            }
            info->unk10 = v;
            info->unk14 = lbl_804D93E8;
        }
        result = 1;
        break;
    }
    case 0x419: {
        PossessMsg* m = (PossessMsg*)data;
        if (owner == 0 || info == 0 || info->owner == 0) {
            result = 0;
        } else {
            m->value = (int)info->owner;
            result = 1;
        }
        break;
    }
    case 0x40a:
        result = PossessAnimQuery(data);
        break;
    case 0xc14:
        result = PossessStateEvent(data);
        break;
    case 0x41d:
        result = fn_8020DD08((int*)data);
        break;
    case 0x41e:
        result = fn_8020DF98((int*)data);
        break;
    case 0x0:
        result = PossessShotFire(data, 0, 0);
        if (info->unk2C == 0x2CAD3C2E) {
            fn_801FAD18(owner, 2);
        }
        break;
    case 0x1:
        result = -(((unsigned int)d->flags >> 27) & 1) & 0xFFF0BDC1;
        break;
    case 0x6: {
        PossessMsg* m = (PossessMsg*)data;
        ActorData* ad = owner->data;
        short modelIds[1];
        short texIds[5];

        modelIds[0] = (short)ad->unkA8;
        texIds[0] = (short)ad->unk80;
        texIds[1] = (short)ad->unk84;
        texIds[2] = (short)ad->unk88;
        texIds[3] = (short)ad->unk8C;
        texIds[4] = (short)ad->unk90;
        if (m->value != 0) {
            ModelLoadIds(modelIds, 1);
            TextureLoad(texIds, 5);
        } else {
            ModelFreeIds(modelIds, 1);
            fn_8000F550(texIds, 5);
        }
        result = 0;
        break;
    }
    case 0xa:
        result = PossessShotFire(data, 0, 1);
        break;
    case 0x7300:
        result = PossessShotFire(data, 1, 1);
        break;
    case 0xc16: {
        PossessMsg* m = (PossessMsg*)data;
        if (m->value != 0) {
            info->flags |= 8;
        } else {
            info->flags &= ~8;
        }
        result = 1;
        break;
    }
    case 0xc17: {
        PossessMsg* m = (PossessMsg*)data;
        m->value = (info->flags >> 3) & 1;
        result = 1;
        break;
    }
    default:
        break;
    }

    if (result == 0) {
        return ActorDefaultMsg(actor, msg, data, arg);
    }
    if (result == 0xFFF0BDC1) {
        result = 0;
    }
    return result;
}

// Hash-keyed possession query (message 0x41D): stores one piece of possession
// state into the query word. Returns 1 for a hash it knows.
extern "C" int fn_8020DD08(int* param) {
    PossessQuery* q = (PossessQuery*)param;
    Actor* actor = q->actor;
    PossessionInfo* info = actor->possess;

    switch (q->msg) {
    case 0x2CAD3C2E:
        info->unk2C = info->unk2C ? 0 : 0x2CAD3C2E;
        return 1;
    case 0x9CB281D0:
        fn_80283864(actor->data->unk00);
        return 1;
    case 0x98DE0283:
        info->unk20 = q->value;
        if (info->owner != 0) {
            PossessOwner* owner = (PossessOwner*)info->owner;
            if (info->unk20 == 0) {
                owner->unk12C = GameEvent(0xF2E2BEB8, actor->data->unkB4);
            } else {
                owner->unk12C = info->unk20;
            }
        }
        return 1;
    case 0x98D8F9CF:
        info->unk1C = q->value;
        if (info->owner != 0) {
            PossessOwner* owner = (PossessOwner*)info->owner;
            if (info->unk1C == 0) {
                owner->unk130 = GameEvent(0xF2E2BEB8, actor->data->unkB0);
            } else {
                owner->unk130 = info->unk1C;
            }
        }
        return 1;
    case 0x1C1A4CC9:
        info->unkC = q->value;
        return 1;
    case 0xC8AAF374:
        info->unk24 = lbl_804D93E8;
        info->unk28 = (float)q->value;
        return 1;
    case 0x7EB3AEC0:
        if (q->value == 0) {
            info->flags &= ~6;
        } else {
            info->flags |= 6;
        }
        return 1;
    case 0x4A1B522D:
        if (q->value == 0) {
            info->flags &= ~8;
        } else {
            info->flags |= 8;
        }
        return 1;
    case 0x776B88B9:
        fn_80283DC4(actor->data->unk00);
        return 1;
    case 0x7EC1614D:
        fn_80283A44(actor->data->unk00);
        return 1;
    default:
        return 0;
    }
}

// Hash-keyed possession query (message 0x41E): the fourth word receives the
// answer. Returns 1 for a known hash; the health hash writes a formatted
// string instead and returns 0.
extern "C" int fn_8020DF98(int* param) {
    PossessQuery* q = (PossessQuery*)param;
    Actor* actor = q->actor;
    PossessionInfo* info = actor->possess;

    switch (q->msg) {
    case 0x18E1630C:
        q->value = 1;
        return 1;
    case 0xAD8576FA: {
        String text;
        fn_8028BF80(&text, "Health: %d / %d", (int)info->unk10,
                    (int)actor->data->unk30);
        fn_8028C188(&q->value, text.unk04, text.unk08);
        return 0;
    }
    case 0x7D45BF64:
        if ((int)actor->data->unk48 == 0) {
            q->value = 1;
        } else {
            q->value = 0;
        }
        return 1;
    case 0x2DA8293C:
        q->value = info->handle;
        return 1;
    case 0x1C1A4CC9:
        q->value = info->unkC;
        return 1;
    case 0x7EB3AEC0:
        q->value = info->flags & 6;
        return 1;
    case 0x98DE0283:
        if (info->unk20 != 0) {
            q->value = info->unk20;
        } else {
            q->value = GameEvent(0xF2E2BEB8, actor->data->unkB4);
        }
        // falls through into the paired state query
    case 0x98D8F9CF:
        if (info->unk1C != 0) {
            q->value = info->unk1C;
        } else {
            q->value = GameEvent(0xF2E2BEB8, actor->data->unkB0);
        }
        if (info->owner != 0) {
            q->value = ((PossessOwner*)info->owner)->unk0;
        } else {
            q->value = 0;
        }
        return 1;
    default:
        return 0;
    }
}

// --- shot / possession path: skin, node and the globals it feeds ---
struct SkinBone {
    Vec3 pos;                           // 0x00
    unsigned char unk0C[0x28 - 0x0C];   // 0x0C
    int unk28;                          // 0x28
    unsigned char unk2C[0x3C - 0x2C];   // 0x2C
};

struct Skin {
    unsigned char unk00[0x08];          // 0x00
    int count;                          // 0x08
    unsigned char unk0C[0x30 - 0x0C];   // 0x0C
    SkinBone bones[1];                  // 0x30
};

// Possession shot message: receiving actor followed by the blended skin.
struct ShotMsg {
    Actor* actor;                       // 0x00
    int unk04;                          // 0x04
    int unk08;                          // 0x08
    int unk0C;                          // 0x0C
    Skin skin;                          // 0x10
};

// Node returned by the model helpers; position sits at 0x24.
struct ShotNode {
    unsigned char unk00[0x24];          // 0x00
    Vec3 pos;                           // 0x24
};

// Actor::data fields the shot path reads.
struct ShotData {
    int unk00;                          // 0x00
    int unk04;                          // 0x04
    int unk08;                          // 0x08
    int unk0C;                          // 0x0C
    int unk10;                          // 0x10
    int unk14;                          // 0x14
    int unk18;                          // 0x18
    int unk1C;                          // 0x1C
};

// Player block: host slot pointers at 0x26C4, host actor at 0x26E4.
struct ShotPlayer {
    unsigned char unk00[0x26C4];             // 0x00
    unsigned int* slots[1];                  // 0x26C4
    unsigned char unk26C8[0x26E4 - 0x26C8];  // 0x26C8
    void* unk26E4;                           // 0x26E4
};

// Shot statistics block the firing helper fills in at 0x1C.
struct ShotStats {
    unsigned char unk00[0x1C];          // 0x00
    int unk1C;                          // 0x1C
};

// Two-angle rotation used by the aim helpers.
struct Rot2 {
    float x;                            // 0x00
    float y;                            // 0x04
};

extern "C" float lbl_804D9418;
extern "C" float lbl_804D941C;
extern "C" float lbl_804D9420;
extern "C" float lbl_804D9424;
extern "C" float lbl_804D9428;
extern "C" float lbl_804D942C;
extern "C" int lbl_804D3408;
extern "C" float lbl_804D340C;
extern "C" int lbl_804D26B8;
extern "C" int lbl_804D26BC;
extern "C" ShotStats lbl_803BB968;
extern "C" ShotStats lbl_803BB9A4;

extern "C" float sqrtf(float);
extern "C" int fn_80158AFC(int, void*);
extern "C" int fn_80158BE0(void);
extern "C" int fn_802837C4(int);
extern "C" int fn_8003AA9C(int, int, int);
extern "C" ShotNode* fn_8003A1F0(int, Bone*, int);
extern "C" void fn_8003F15C(float, int, int, int, Skin*, ShotStats*);
extern "C" float fn_80115F68(int);
extern "C" int fn_800390D8(int, int);
extern "C" ShotNode* fn_800C6448(void);
extern "C" void fn_800A025C(Vec3*, void*, int);
extern "C" void fn_8016D440(Rot2*, Vec3*);
extern "C" void fn_8016D4EC(Vec3*, Rot2*);
extern "C" float fn_800A109C(float);
extern "C" float fn_800C63D8(void);
extern "C" float fn_800A1418(float);
extern "C" float fn_802F4B38(float, float);

Actor* GetPlayer();
int PlayerInHost(Actor* actor);
int Fn80080FA8();
int GhostActionIsActive();
int ModelAnimate(int model, char* name, int arg);
Skin* SkinBlendTint(Skin* skin, int a, int b, int c, float blend);

#define SHOT_ABS(v) ((v) < 0.0f ? -(v) : (v))

// Fires the possession shot for a possessed actor: updates the aim angle,
// the shot statistics block and the skin blend weights. Name is a guess.
int PossessShotFire(void* data, int a, int b)
{
    ShotMsg* msg = (ShotMsg*)data;
    Actor* actor = msg->actor;
    PossessionInfo* info = actor->possess;
    ShotData* adata = (ShotData*)actor->data;
    Actor* player;
    ShotPlayer* pl;
    Skin* skin;
    ShotNode* node;
    Vec3 dir;
    Vec3 aim;
    Rot2 rot;
    float len;
    float inv;
    float dot;
    float cube;
    float angle;
    float scale;
    float t;
    int model;
    int anim;
    int i;

    if ((info->flags & 0x10) != 0) {
        return 1;
    }
    if (a == 0 && fn_80158AFC(actor->unk38, data) != 0) {
        return 1;
    }

    model = actor->unk90;
    anim = ModelAnimate(model, 0, 0);
    node = fn_8003A1F0(model, actor->bones, anim);
    player = GetPlayer();
    pl = (ShotPlayer*)player;

    if (lbl_804D93F4 <= info->unk14) {
        skin = &msg->skin;
    } else {
        t = (lbl_804D93F4 - info->unk14) / lbl_804D93F4;
        if (t < lbl_804D93E8) {
            t = lbl_804D93E8;
        } else if (t > lbl_804D93EC) {
            t = lbl_804D93EC;
        }
        skin = SkinBlendTint(&msg->skin, 0xFF, 0, 0, t);
    }

    if (player == 0 ||
        (PlayerInHost(player) != 0 && lbl_804D3408 == 0) ||
        Fn80080FA8() != 0 ||
        (fn_80158BE0() != 0 || info->unk10 <= lbl_804D93E8) ||
        *(int*)pl->slots[PlayerInHost(player)] == 0x7C48821 ||
        (fn_802837C4(adata->unk00) == 0 || b != 0) ||
        GhostActionIsActive() != 0) {
        fn_8003F15C(lbl_804D93EC, model, (int)node, actor->unk94, skin, 0);
        return 1;
    }

    if (lbl_804D93E8 < fn_80115F68(actor->unk38)) {
        fn_8003F15C(lbl_804D93EC, model, (int)node, actor->unk94, skin, 0);
        return 1;
    }

    if (adata->unk10 >= 0) {
        if (PlayerInHost(player) == 0 ||
            (pl->unk26E4 != 0 &&
             (*(unsigned int*)((char*)pl->unk26E4 + 0x38) >> 24) == 0x13)) {
            if (adata->unk10 != -1) {
                int model2 = fn_800390D8(actor->unk90, adata->unk10);
                ShotNode* obj = fn_800C6448();

                fn_800A025C(&aim, obj, 1);
                dir.x = node->pos.x - obj->pos.x;
                dir.y = node->pos.y - obj->pos.y;
                dir.z = node->pos.z - obj->pos.z;
                if (SHOT_ABS(dir.x) > lbl_804D9418 ||
                    SHOT_ABS(dir.y) > lbl_804D9418 ||
                    SHOT_ABS(dir.z) > lbl_804D9418) {
                    inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
                    len = 1.0f / inv;
                    dir.x *= inv;
                    dir.y *= inv;
                    dir.z *= inv;
                } else {
                    dir.x = lbl_804D93E8;
                    dir.y = lbl_804D93E8;
                    dir.z = lbl_804D93E8;
                    len = lbl_804D93E8;
                }

                dot = dir.x * aim.x + dir.y * aim.y + dir.z * aim.z;
                cube = dot * dot * dot;
                fn_8016D440(&rot, &dir);
                rot.x = lbl_804D93E8;
                rot.y = lbl_804D93E8;
                fn_8016D4EC(&dir, &rot);
                fn_8016D440(&rot, &aim);
                rot.x = lbl_804D93E8;
                rot.y = lbl_804D93E8;
                fn_8016D4EC(&aim, &rot);

                if (SHOT_ABS(aim.x) > lbl_804D9418 ||
                    SHOT_ABS(aim.y) > lbl_804D9418 ||
                    SHOT_ABS(aim.z) > lbl_804D9418) {
                    inv = 1.0f / sqrtf(aim.x * aim.x + aim.y * aim.y + aim.z * aim.z);
                    aim.x *= inv;
                    aim.y *= inv;
                    aim.z *= inv;
                } else {
                    aim.x = lbl_804D93E8;
                    aim.y = lbl_804D93E8;
                    aim.z = lbl_804D93E8;
                }

                angle = fn_800A109C(dir.x * aim.x + dir.y * aim.y + dir.z * aim.z);
                scale = lbl_804D941C * (lbl_804D9420 / fn_800C63D8());
                angle *= scale;
                if (angle > lbl_804D9420) {
                    angle = lbl_804D9420;
                }
                if (angle < lbl_804D9424) {
                    angle = lbl_804D9424;
                }
                angle = fn_800A1418(angle);
                scale = fn_802F4B38(angle, cube);
                lbl_803BB968.unk1C = (int)(lbl_804D9428 * scale);

                if (adata->unk1C == (int)0x946B64F0) {
                    anim = fn_8003AA9C(model2, 0, 0);
                    node = fn_8003A1F0(adata->unk10, actor->bones, anim);
                }
                fn_8003F15C(lbl_804D93EC, model2, (int)node, actor->unk94, 0,
                            &lbl_803BB968);
                if (adata->unk1C == (int)0x946B64F0) {
                    anim = fn_8003AA9C(actor->unk90, 0, 0);
                    node = fn_8003A1F0(actor->unk90, actor->bones, anim);
                }

                t = (len - (float)lbl_804D26B8) /
                    (float)(lbl_804D26BC - lbl_804D26B8);
                if (t < lbl_804D93E8) {
                    t = lbl_804D93E8;
                } else if (t > lbl_804D93EC) {
                    t = lbl_804D93EC;
                }
                scale = lbl_804D93EC -
                        lbl_804D340C *
                            ((float)lbl_803BB968.unk1C / lbl_804D9428) *
                            (lbl_804D93EC - t);
                if (info->unk28 > lbl_804D93E8) {
                    scale *= info->unk24 / info->unk28;
                }
                for (i = 0; i < skin->count; i++) {
                    skin->bones[i].pos.x *= scale;
                    skin->bones[i].pos.y *= scale;
                    skin->bones[i].pos.z *= scale;
                    skin->bones[i].unk28 = -1;
                }
                fn_8003F15C(lbl_804D93EC, model, (int)node, actor->unk94, skin, 0);
            }
        } else {
            fn_8003F15C(lbl_804D93EC, model, (int)node, actor->unk94, skin, 0);
        }
    } else {
        if (PlayerInHost(player) == 0 ||
            (pl->unk26E4 != 0 &&
             (*(unsigned int*)((char*)pl->unk26E4 + 0x38) >> 24) == 0x13)) {
            ShotNode* obj;
            float inv2;

            fn_800390D8(actor->unk90, adata->unk10);
            obj = fn_800C6448();
            fn_800A025C(&aim, obj, 1);
            dir.x = node->pos.x - obj->pos.x;
            dir.y = node->pos.y - obj->pos.y;
            dir.z = node->pos.z - obj->pos.z;
            if (SHOT_ABS(dir.x) > lbl_804D9418 ||
                SHOT_ABS(dir.y) > lbl_804D9418 ||
                SHOT_ABS(dir.z) > lbl_804D9418) {
                inv2 = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
                len = 1.0f / inv2;
                dir.x *= inv2;
                dir.y *= inv2;
                dir.z *= inv2;
            } else {
                dir.x = lbl_804D93E8;
                dir.y = lbl_804D93E8;
                dir.z = lbl_804D93E8;
                len = lbl_804D93E8;
            }

            fn_8016D440(&rot, &dir);
            rot.x = lbl_804D93E8;
            rot.y = lbl_804D93E8;
            fn_8016D4EC(&dir, &rot);
            fn_8016D440(&rot, &aim);
            rot.x = lbl_804D93E8;
            rot.y = lbl_804D93E8;
            fn_8016D4EC(&aim, &rot);

            if (SHOT_ABS(aim.x) > lbl_804D9418 ||
                SHOT_ABS(aim.y) > lbl_804D9418 ||
                SHOT_ABS(aim.z) > lbl_804D9418) {
                inv2 = 1.0f / sqrtf(aim.x * aim.x + aim.y * aim.y + aim.z * aim.z);
                aim.x *= inv2;
                aim.y *= inv2;
                aim.z *= inv2;
            } else {
                aim.x = lbl_804D93E8;
                aim.y = lbl_804D93E8;
                aim.z = lbl_804D93E8;
            }

            angle = fn_800A109C(dir.x * aim.x + dir.y * aim.y + dir.z * aim.z);
            scale = lbl_804D941C * (lbl_804D9420 / fn_800C63D8());
            angle *= scale;
            if (angle > lbl_804D9420) {
                angle = lbl_804D9420;
            }
            if (angle < lbl_804D9424) {
                angle = lbl_804D9424;
            }
            angle = fn_800A1418(angle);
            lbl_803BB9A4.unk1C = (int)(lbl_804D9428 * angle);

            fn_8003F15C(lbl_804D93EC, actor->unk90, (int)node, actor->unk94, 0,
                        &lbl_803BB9A4);

            t = (len - (float)lbl_804D26B8) /
                (float)(lbl_804D26BC - lbl_804D26B8);
            if (t < lbl_804D93E8) {
                t = lbl_804D93E8;
            } else if (t > lbl_804D93EC) {
                t = lbl_804D93EC;
            }
            scale = lbl_804D93EC -
                    lbl_804D340C * ((float)lbl_803BB968.unk1C / lbl_804D9428) *
                        (lbl_804D93EC - t);
            if (info->unk28 > lbl_804D93E8) {
                scale *= info->unk24 / info->unk28;
            }
            for (i = 0; i < skin->count; i++) {
                skin->bones[i].pos.x *= scale;
                skin->bones[i].pos.y *= scale;
                skin->bones[i].pos.z *= scale;
                skin->bones[i].unk28 = -1;
            }
            fn_8003F15C(lbl_804D93EC, actor->unk90, (int)node, actor->unk94, skin, 0);
        } else {
            fn_8003F15C(lbl_804D93EC, model, (int)node, actor->unk94, skin, 0);
        }
    }

    return 1;
}

// --- Possession state block (0x178) allocated on the 0x672F35C2 event ---
struct PossessSlot {
    int bone;                          // 0x00
    float unk4;                        // 0x04
    float unk8;                        // 0x08
};

struct PossessState {
    int unk00;                          // 0x00
    int unk04;                          // 0x04
    int unk08;                          // 0x08
    int unk0C;                          // 0x0C
    int unk10;                          // 0x10
    unsigned char unk14[0x1C - 0x14];   // 0x14
    int unk1C;                          // 0x1C
    unsigned char unk20[0x30 - 0x20];   // 0x20
    int unk30;                          // 0x30
    unsigned char unk34[0x38 - 0x34];   // 0x34
    float unk38;                        // 0x38
    PossessSlot slots[11];              // 0x3C
    unsigned char unkB8[0x108 - 0xB8];  // 0xB8
    float unk108;                       // 0x108
    unsigned char unk10C[0x12C - 0x10C]; // 0x10C
    int unk12C;                         // 0x12C
    int unk130;                         // 0x130
    unsigned char unk134[0x14C - 0x134];// 0x134
    int unk14C;                         // 0x14C
    unsigned char unk150[0x174 - 0x150];// 0x150
    int unk174;                         // 0x174
};

// Source data block (Actor::data) fields this helper copies out.
struct PossessBoneRef {
    int id;                             // 0x00
    int value;                          // 0x04
};

struct PossessSrcData {
    int unk00;                          // 0x00
    int unk04;                          // 0x04
    PossessBoneRef bones[3];            // 0x08 (0x08, 0x10, 0x18)
    unsigned char unk20[0x30 - 0x20];   // 0x20
    float unk30;                        // 0x30
    unsigned char unk34[0x44 - 0x34];   // 0x34
    float unk44;                        // 0x44
    unsigned char unk48[0x50 - 0x48];   // 0x48
    float unk50;                        // 0x50
    unsigned char unk54[0xA4 - 0x54];   // 0x54
    int unkA4;                          // 0xA4
    unsigned char unkA8[0xAC - 0xA8];   // 0xA8
    int unkAC;                          // 0xAC
    int unkB0;                          // 0xB0
    int unkB4;                          // 0xB4
};

// 0x672F35C2 / 0x39CB0CB0 event payload.
struct PossessStateMsg {
    Actor* actor;                       // 0x00
    int unk04;                          // 0x04
    int unk08;                          // 0x08
    int msg;                            // 0x0C
};

extern "C" int fn_802A1D04(void);

// Allocates or releases the possession state block on the paired
// start/finish events. Name is a guess.
int PossessStateEvent(void* data)
{
    PossessStateMsg* msg = (PossessStateMsg*)data;
    Actor* actor = msg->actor;
    PossessionInfo* info = actor->possess;

    if (msg->msg == 0x672F35C2) {
        PossessSrcData* src = (PossessSrcData*)actor->data;
        PossessState* state;
        PossessSlot cur;
        int i;

        if (info->owner != 0) {
            VFree((void*)info->owner);
            info->owner = 0;
        }
        state = (PossessState*)VAlloc(0x178);
        info->owner = state;
        memset(state, 0, 0x178);

        state->unk00 = src->unk00;
        state->unk0C = src->unk04;
        state->unk04 = src->bones[2].value;
        state->unk174 = src->bones[2].id;
        state->unk30 = (int)src->unk30;
        state->unk38 = src->unk44;
        state->unk108 = src->unk50;

        if (info->unk1C != 0) {
            state->unk130 = info->unk1C;
        } else {
            state->unk130 = GameEvent(0xF2E2BEB8, src->unkB0);
        }
        if (info->unk20 != 0) {
            state->unk12C = info->unk20;
        } else {
            state->unk12C = GameEvent(0xBAA22E62, src->unkB4);
        }

        state->unk10 = 0xFF30E617;
        state->unk1C = 0;
        for (i = 0; i < 11; i++) {
            state->slots[i].bone = -1;
        }

        cur.bone = -1;
        cur.unk4 = lbl_804D942C;
        for (i = 0; i < 7; i++) {
            PossessSlot* slot = &state->slots[i + 4];

            if (i < 1) {
                PossessBoneRef* ref = &src->bones[i];

                if (ref->id != -1) {
                    slot->bone = ref->id;
                    slot->unk4 = (float)ref->value;
                    cur = *slot;
                    continue;
                }
            }
            *slot = cur;
        }

        state->slots[3].bone = src->unkA4;
        state->unk14C = src->unkAC;
        info->flags |= 1;
        info->unkC = msg->unk08;
    } else if (msg->msg < 0x672F35C2 && msg->msg == 0x39CB0CB0) {
        if (info->owner != 0 && fn_802A1D04() == 0) {
            VFree((void*)info->owner);
            info->owner = 0;
        }
        info->flags &= ~1u;
        info->unkC = -1;
    }
    return 0;
}

// MSL's toupper maps a signed char through the shared upper-case table.
extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

// Seeded name hash used by the possession message queries.
inline void StrHash(unsigned int& h, const char* s)
{
    if (s != 0) {
        for (int i = 0; s[i] != 0; i++) {
            h *= 31;
            h += (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
    } else {
        h = 0;
    }
}

// Payload of the animation query message: the caller supplies the actor, the
// animation argument and a hash seed; the answer is written to result.
struct AnimQueryMsg {
    Actor* actor;         // 0x00
    int arg;              // 0x04
    unsigned int hash;    // 0x08
    unsigned int result;  // 0x0C
};

// Reply block of the 0x40B message.
struct AnimQueryReply {
    int unk00;            // 0x00
    int unk04;            // 0x04
    unsigned int state;   // 0x08
    int asset;            // 0x0C
};

struct PlayerActor {
    unsigned char unk00[0x26E4];  // 0x00
    Actor* host;                  // 0x26E4, actor currently possessed
};

struct PlayerSelectObj {
    unsigned char unk00[0x8C];            // 0x00
    int unk8C;                            // 0x8C, state callback
    unsigned char unk90[0x98 - 0x90];     // 0x90
    int unk98;                            // 0x98, first endpoint value
    unsigned char unk9C[0xA4 - 0x9C];     // 0x9C
    int unkA4;                            // 0xA4, first value duration
    unsigned char unkA8[0xC4 - 0xA8];     // 0xA8
    int unkC4;                            // 0xC4
    unsigned char unkC8[0xF0 - 0xC8];     // 0xC8
    int unkF0;                            // 0xF0, second endpoint value
    unsigned char unkF4[0xFC - 0xF4];     // 0xF4
    int unkFC;                            // 0xFC, second value duration
    unsigned char unk100[0x328 - 0x100];  // 0x100
    int unk328;                           // 0x328, aim state
    unsigned char unk32C[0x638 - 0x32C];  // 0x32C
    int unk638;                           // 0x638
    unsigned char unk63C[0x73C - 0x63C];  // 0x63C
    int unk73C;                           // 0x73C, slot index
    unsigned char unk740[0x7A4 - 0x740];  // 0x740
    int unk7A4;                           // 0x7A4, value block
    unsigned char unk7A8[0x7B4 - 0x7A8];  // 0x7A8
    unsigned int unk7B4;                  // 0x7B4
    unsigned char unk7B8[0x7D0 - 0x7B8];  // 0x7B8
    int unk7D0;                           // 0x7D0
    unsigned char unk7D4[0x26C4 - 0x7D4]; // 0x7D4
    int unk26C4[7];                       // 0x26C4, per-player possession records
    int unk26E0;                          // 0x26E0
    int unk26E4;                          // 0x26E4
    int unk26E8;                          // 0x26E8
    unsigned char unk26EC[0x279C - 0x26EC]; // 0x26EC
    int unk279C;                          // 0x279C
};

unsigned long AssetFind(unsigned long id, unsigned long name);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void fn_800769A0(void* self, float dt);
void PlayerSelectCollisionUpdate(void* self, float dt);
void PlayerSetValue4(void* value, int v);

// Animation name query (message 0x40A): hashes the state name, starts the
// matching animation and reports the possession state selection.
int PossessAnimQuery(void* data)
{
    AnimQueryMsg* m = (AnimQueryMsg*)data;
    Actor* actor = m->actor;
    PossessionInfo* info = actor->possess;
    unsigned int hash = m->hash;

    StrHash(hash, "QUERY");
    if (ActorPlayAnim(actor->unk38, hash, m->arg) != 0) {
        m->result = 0;
        return 1;
    }
    if (info->unkC != -1)
        return 1;
    if ((actor->unk4C & 4) == 0) {
        m->result = 0;
        return 1;
    }
    if (info->unk10 <= lbl_804D93E8 || (info->flags & 2) == 0)
        return 0;

    {
        Actor* player = GetPlayer();
        PlayerActor* pinfo = (PlayerActor*)player;
        if (PlayerInHost(player) != 0 &&
            (pinfo->host == 0 || ((unsigned int)pinfo->host->unk38 >> 24) != 0x13))
            return 1;
    }

    if (fn_802837C4(actor->data->unk00) != 0) {
        if (PlayerInHost(GetPlayer()) != 0)
            return 0;
        if (m->hash == 0x3CA34BD6)
            m->result = 0x1379C466;
        return 1;
    }

    hash = m->hash;
    StrHash(hash, "QUERY");
    if (ActorPlayAnim(actor->unk38, hash, m->arg) != 0)
        return 1;

    m->result = 0x1379C466;
    {
        AnimQueryReply reply;
        reply.unk04 = 0;
        reply.state = 0x1379C466;
        reply.asset = AssetFind(0xC0A77187UL, 0x26BE7D);
        SendMsg(m->arg, 0x40B, &reply);
    }
    return 1;
}

// Player-select state update for the possession host: clears the state slot,
// refreshes the collision query and picks the idle/move/possessed sub-state.
void PossessSelectCollisionUpdate(void* self, float dt)
{
    PlayerSelectObj* obj = (PlayerSelectObj*)self;

    obj->unk638 = 0;
    obj->unk7B4 &= ~1u;
    fn_800769A0(self, dt);
    PlayerSelectCollisionUpdate(self, dt);
    if (obj->unkC4 == -1) {
        if (obj->unk7D0 != 0)
            PlayerSetValue4(&obj->unk7A4, 3);
        else
            PlayerSetValue4(&obj->unk7A4, 1);
    } else {
        PlayerSetValue4(&obj->unk7A4, 0x16);
    }
}

// --- possession select state: pending aim/selection values ---
// Holds a live slot and a pending slot for each of the two selections; the
// commit below copies the pending value across when one is queued. Name and
// field meaning are guesses.
struct PossessAimState {
    unsigned char unk00[0x8C];           // 0x00
    int unk8C;                           // 0x8C
    unsigned char unk90[0x98 - 0x90];    // 0x90
    int unk98;                           // 0x98, live value
    unsigned char unk9C[0xC4 - 0x9C];    // 0x9C
    int unkC4;                           // 0xC4, pending value, -1 = none
    unsigned char unkC8[0xF0 - 0xC8];    // 0xC8
    int unkF0;                           // 0xF0, live value
    unsigned char unkF4[0x11C - 0xF4];   // 0xF4
    int unk11C;                          // 0x11C, pending value, -1 = none
};

// Commits the queued aim/selection values into the live slots.
void PossessAimStateCommit(PossessAimState* state)
{
    state->unk8C = 0;
    if (state->unkC4 != -1) {
        state->unk98 = state->unkC4;
    }
    if (state->unk11C == -1) {
        return;
    }
    state->unkF0 = state->unk11C;
}

extern "C" void PlayerResetGunTimer(void* obj);
extern "C" int fn_80175EFC(int id);
extern "C" void fn_80155A84(void* self, void* data, int flag);

// Resets the gun timer, clears the select-state slot and picks the idle or
// possessed sub-state for the value block. Name is a guess.
void PossessSelectResetGun(PlayerSelectObj* obj)
{
    PlayerResetGunTimer(obj);
    obj->unk638 = 1;
    obj->unk7B4 &= ~1u;
    PlayerSetValue4(&obj->unk7A4, 0x15);
    if (obj->unk279C == -1 || fn_80175EFC(obj->unk279C) != 0) {
        PlayerSetValue4(&obj->unk7A4, 3);
        fn_80155A84(&obj->unk26E0, (void*)obj->unk26E8, 1);
    }
}

// Global data block owned by another unit. The possession-select code reads a
// table of per-slot values that starts at 0x5F4 inside it.
// Per-slot possession-select data; the table starts at the base of
// lbl_803BB9E0 and each entry is 0x48 bytes.
struct SelectSlotEntry {
    unsigned char unk00[0x8];          // 0x00
    void (*callback)(void*);           // 0x08, per-slot update callback
    unsigned char unk0C[0x38];         // 0x0C
    int value;                         // 0x44, per-slot value
};

struct SelectGlobalData {
    union {
        unsigned char unk000[0x51C];   // 0x000
        SelectSlotEntry slot[0x12];    // 0x000, per-slot select table
    };
    unsigned int selectValue[0x12];    // 0x51C, per-slot value, select state
    unsigned int hostValue[0x24];      // 0x564, per-slot value, possession host
    unsigned int slotValue[0x45];      // 0x5F4, per-slot value, select state
};

extern "C" SelectGlobalData lbl_803BB9E0;

// Data block returned for a possessed host actor; the two endpoint values are
// handed to fn_800A1BC8 together with the per-slot value.
struct HostPossessInfo {
    unsigned char unk00[0x14];         // 0x00
    int unk14;                         // 0x14
    int unk18;                         // 0x18
    int unk1C;                         // 0x1C
};

extern "C" void* fn_800A384C(void* host);
extern "C" int fn_800A1BC8(int, int, unsigned int, unsigned int, int);
int HasCondition(void* value, int id);
int PossessSelectEventDispatch(int event, void* self);

// Starts the possession-select aim state: takes the two endpoint values for
// the current slot from the possessed host's data and installs the follow-up
// callback and durations. Name and field meaning are guesses.
void PossessSelectEnterPossessed(PlayerSelectObj* obj)
{
    Actor* host = (Actor*)obj->unk26E8;
    HostPossessInfo* info;
    unsigned int flag = 0;

    if ((int)((unsigned int)host->unk38 >> 24) == 0x13) {
        info = (HostPossessInfo*)fn_800A384C(host);
        if (HasCondition((unsigned char*)host + 0x270, 0xB) != 0)
            flag = 1;
    }
    PlayerInHost((Actor*)obj);
    {
        int index = obj->unk73C;

        obj->unk98 = fn_800A1BC8(info->unk14, info->unk18,
                                  lbl_803BB9E0.slotValue[index], flag, 0);
        obj->unk8C = (int)PossessSelectEventDispatch;
        obj->unkA4 = 0x19;
        obj->unkF0 = fn_800A1BC8(info->unk14, info->unk1C,
                                  lbl_803BB9E0.slotValue[index], flag, 0);
        obj->unkFC = 0x19;
    }
    PlayerSetValue4(&obj->unk7A4, 0x15);
}

int PlayerGetHeldSlotIndex(void* self);

// Per-frame gun-select update: resets the gun timer, refreshes the collision
// query, picks the idle/held sub-state and toggles the aim flag. Name and
// field meaning are guesses.
void PossessSelectGunUpdate(PlayerSelectObj* obj, float dt)
{
    PlayerResetGunTimer(obj);
    obj->unk638 = 1;
    obj->unk7B4 &= ~1u;
    fn_800769A0(obj, dt);
    PlayerSelectCollisionUpdate(obj, dt);
    if (PlayerGetHeldSlotIndex(obj) != 0) {
        PlayerSetValue4(&obj->unk7A4, 2);
    } else {
        PlayerSetValue4(&obj->unk7A4, 1);
    }
    PlayerInHost((Actor*)obj);
    if (obj->unk328 == 1) {
        unsigned int flags = obj->unk7B4;

        if ((flags & 0x80000) != 0)
            obj->unk7B4 = flags & ~0x80000;
        else
            obj->unk7B4 = flags | 0x80000;
    }
}

// --- possession-select state handlers ---
// The player-select object starts with the shared actor header; only the
// fields these two callbacks touch are named. Field meaning is a guess.
struct PossessSelectActor {
    unsigned char unk00[0x50];             // 0x00
    unsigned int flags;                    // 0x50, actor flags
    unsigned char unk54[0x78 - 0x54];      // 0x54
    int unk78;                             // 0x78, action state
    unsigned char unk7C[0x98 - 0x7C];      // 0x7C
    int unk98;                             // 0x98, select value, -1 = none
    unsigned char unk9C[0x638 - 0x9C];     // 0x9C
    int unk638;                            // 0x638
    unsigned char unk63C[0x7A4 - 0x63C];   // 0x63C
    int unk7A4;                            // 0x7A4, value block
    unsigned char unk7A8[0x7B4 - 0x7A8];   // 0x7A8
    unsigned int unk7B4;                   // 0x7B4, select flags
    unsigned char unk7B8[0x26E4 - 0x7B8];  // 0x7B8
    void* unk26E4;                         // 0x26E4, possessed host actor
};

// Possessed host actor reached through 0x26E4; SendMsg addresses it by the id
// at 0x38.
struct PossessSelectHost {
    unsigned char unk00[0x38];             // 0x00
    int unk38;                             // 0x38
};

// Query payload for message 0x41E: the second word is the hash of the property
// being asked about; the receiver fills the embedded String with the answer.
struct PossessTextQuery {
    int unk00;                             // 0x00
    unsigned int hash;                     // 0x04
    int unk08;                             // 0x08
    String text;                           // 0x0C
};

void PlayerSetHeldSlotIndex(void* self, int index);
void PlayerSetActionMode(void* self, int mode);

// Possession-select state: asks the possessed host for the value behind the
// 0x08783452 property hash and picks the held-slot index and action mode from
// the answer. Name and field meaning are guesses.
void PossessSelectCheckHost(PossessSelectActor* obj)
{
    PossessTextQuery q;

    PlayerInHost((Actor*)obj);
    obj->unk78 = 0;
    obj->flags &= ~0x20000u;

    q.hash = 0x08783452;
    SendMsg(((PossessSelectHost*)obj->unk26E4)->unk38, 0x41E, &q);

    // The answer is empty when the host reports nothing to hold.
    if (q.text.unk04 == 0) {
        PlayerSetHeldSlotIndex(obj, 1);
        if (obj->unk98 == -1) {
            PlayerSetActionMode(obj, 3);
        }
    } else {
        PlayerSetHeldSlotIndex(obj, 0);
        if (obj->unk98 == -1) {
            PlayerSetActionMode(obj, 1);
        }
    }
}

// Possession-select state: restarts the gun timer and either drops the pending
// select flag or raises the "gun busy" flag and returns the actor to state 1.
// Name and field meaning are guesses.
void PossessSelectGunRecover(PossessSelectActor* obj)
{
    PlayerResetGunTimer(obj);
    obj->unk638 = 1;
    if ((obj->unk7B4 & 1) != 0) {
        obj->unk7B4 &= ~1u;
    } else {
        obj->flags |= 0x20000;
        obj->unk78 = 1;
    }
}

// Possession-select state: takes the two endpoint values for the current slot
// from the possession host's value table, installs the follow-up callback and
// durations, then asks the possessed host for the 0x08783452 property and picks
// the held-slot index from whether the answer is empty. Name and field meaning
// are guesses.
void PossessSelectEnterHostQuery(PlayerSelectObj* obj)
{
    Actor* host = (Actor*)obj->unk26E8;
    HostPossessInfo* info;
    unsigned int flag = 0;

    if (((unsigned int)host->unk38 >> 24) == 0x13) {
        info = (HostPossessInfo*)fn_800A384C(host);
        if (HasCondition((unsigned char*)host + 0x270, 0xB) != 0)
            flag = 1;
    }
    PlayerInHost((Actor*)obj);
    {
        int index = obj->unk73C;

        obj->unk98 = fn_800A1BC8(info->unk14, info->unk18,
                                  lbl_803BB9E0.hostValue[index], flag, 0);
        obj->unk8C = (int)PossessSelectEventDispatch;
        obj->unkA4 = 0x19;
        obj->unkF0 = fn_800A1BC8(info->unk14, info->unk1C,
                                  lbl_803BB9E0.hostValue[index], 0, 0);
        obj->unkFC = 0x19;
    }
    {
        PossessTextQuery q;

        q.hash = 0x08783452;
        SendMsg(((PossessSelectHost*)(void*)obj->unk26E8)->unk38, 0x41E, &q);
        if (q.text.unk04 == 0)
            PlayerSetHeldSlotIndex(obj, 1);
        else
            PlayerSetHeldSlotIndex(obj, 0);
    }
}

// Possession-select state: hands the possessed host actor and the object's
// state slot to the follow helper with the flag set. Name and field meaning are
// guesses.
void PossessSelectHostRelease(PlayerSelectObj* obj)
{
    fn_80155A84(&obj->unk26E0, (void*)obj->unk26E8, 1);
}

// Possession-select state: restarts the gun timer, marks the gun busy and
// returns the value block to the busy sub-state. Name and field meaning are
// guesses.
void PossessSelectGunBusy(PossessSelectActor* obj)
{
    PlayerResetGunTimer(obj);
    obj->unk638 = 1;
    obj->unk7B4 &= ~1u;
    obj->flags |= 0x20000;
    obj->unk78 = 1;
    PlayerSetValue4(&obj->unk7A4, 0x13);
}

// Possession-select state: takes the two endpoint values for the current slot
// from the possessed host and from the per-player possession record, installs
// the follow-up callback and durations, then asks the possessed host for the
// 0x08783452 property and picks the held-slot index from whether the answer is
// empty. Name and field meaning are guesses.
void PossessSelectEnterHostSlot(PlayerSelectObj* obj)
{
    Actor* host;
    HostPossessInfo* info;
    unsigned int flag = 0;
    int player;
    int index;

    player = PlayerInHost((Actor*)obj);
    host = (Actor*)obj->unk26E8;
    index = obj->unk73C;
    if (((unsigned int)host->unk38 >> 24) == 0x13) {
        info = (HostPossessInfo*)fn_800A384C(host);
        if (HasCondition((unsigned char*)host + 0x270, 0xB) != 0)
            flag = 1;
    }
    {
        HostPossessInfo* slotInfo = (HostPossessInfo*)obj->unk26C4[player];

        obj->unk98 = fn_800A1BC8(info->unk14, info->unk18,
                                  lbl_803BB9E0.selectValue[index], flag, 0);
        obj->unk8C = (int)PossessSelectEventDispatch;
        obj->unkA4 = 0x59;
        obj->unkF0 = fn_800A1BC8(slotInfo->unk14, slotInfo->unk1C,
                                  lbl_803BB9E0.selectValue[index], 0, 0);
        obj->unkFC = 0x59;
    }
    {
        PossessTextQuery q;

        q.hash = 0x08783452;
        SendMsg(((PossessSelectHost*)(void*)obj->unk26E8)->unk38, 0x41E, &q);
        if (q.text.unk04 == 0)
            PlayerSetHeldSlotIndex(obj, 1);
        else
            PlayerSetHeldSlotIndex(obj, 0);
    }
}

// Possession-select state: clears the 0x40000 aim flag in the shared actor
// header. Name and flag meaning are guesses.
void PossessSelectClearAimFlag(PossessSelectActor* obj)
{
    obj->flags &= ~0x40000u;
}

// Possession-select state: resets the gun timer, clears the select-state slot
// and refreshes the collision query. Name and field meaning are guesses.
void PossessSelectGunResetUpdate(void* self, float dt)
{
    PlayerSelectObj* obj = (PlayerSelectObj*)self;

    PlayerResetGunTimer(self);
    obj->unk638 = 1;
    obj->unk7B4 &= ~1u;
    PlayerSelectCollisionUpdate(self, dt);
}

// Player-select object as seen by the gun-value states: the shared actor flags,
// the gun mode word, the value block and its flags. Field meaning is a guess.
struct PossessGunSelect {
    unsigned char unk00[0x50];           // 0x00
    unsigned int flags;                  // 0x50, actor flags
    unsigned char unk54[0x314 - 0x54];   // 0x54
    int unk314;                          // 0x314, gun mode
    unsigned char unk318[0x638 - 0x318]; // 0x318
    int unk638;                          // 0x638
    unsigned char unk63C[0x7A4 - 0x63C]; // 0x63C
    int unk7A4;                          // 0x7A4, value block
    unsigned char unk7A8[0x7B4 - 0x7A8]; // 0x7A8
    unsigned int unk7B4;                 // 0x7B4
};

extern "C" void fn_80050EB0(void* obj);
void PlayerSetHeldSlotMode(void* obj, int mode);

// Resets the gun timer and returns the possession select to its held mode.
// Name and field meaning are guesses.
void PossessSelectGunHeld(PossessGunSelect* obj)
{
    PlayerResetGunTimer(obj);
    obj->flags |= 0x40000;
    fn_80050EB0(obj);
    PlayerInHost((Actor*)obj);
    PlayerSetHeldSlotMode(obj, 3);
}

// Per-frame gun-value update for the aiming sub-state: refreshes the collision
// query and picks the idle/move sub-state of the value block. Name and field
// meaning are guesses.
void PossessSelectGunAimUpdate(PossessGunSelect* obj, float dt)
{
    obj->unk638 = 1;
    obj->unk7B4 &= ~1u;
    fn_800769A0(obj, dt);
    PlayerSelectCollisionUpdate(obj, dt);
    PlayerSetValue4(&obj->unk7A4, 5);
    if (obj->unk314 == 0) {
        PlayerSetValue4(&obj->unk7A4, 3);
    } else {
        PlayerResetGunTimer(obj);
    }
}

// Possession-select state: copies the possessed host's two endpoint values
// into the live select slots with fixed one-step durations. Name is a guess.
void PossessSelectEnterHostValue(PlayerSelectObj* obj)
{
    Actor* host;

    PlayerInHost((Actor*)obj);
    host = (Actor*)obj->unk26E4;
    obj->unk98 = host->data->unk98;
    obj->unk8C = (int)PossessSelectEventDispatch;
    obj->unkA4 = 1;
    obj->unkF0 = host->data->unk94;
    obj->unkFC = 1;
}

// View of the player-select object used by the per-frame possession update.
struct PossessUpdateObj {
    unsigned char unk00[0x38];            // 0x00
    int unk38;                            // 0x38, message target id
    unsigned char unk3C[0x7C - 0x3C];     // 0x3C
    int unk7C;                            // 0x7C, effect handle
    unsigned char unk80[0x98 - 0x80];     // 0x80
    int unk98;                            // 0x98, select value, -1 = none
    unsigned char unk9C[0x314 - 0x9C];    // 0x9C
    int unk314;                           // 0x314, gun mode
    unsigned char unk318[0x38C - 0x318];  // 0x318
    float unk38C;                         // 0x38C
    unsigned char unk390[0x394 - 0x390];  // 0x390
    float unk394;                         // 0x394
    unsigned char unk398[0x58C - 0x398];  // 0x398
    int unk58C;                           // 0x58C, pad id
    unsigned char unk590[0x638 - 0x590];  // 0x590
    int unk638;                           // 0x638
    unsigned char unk63C[0x678 - 0x63C];  // 0x63C
    unsigned int unk678;                  // 0x678, actor flags
    unsigned char unk67C[0x7A4 - 0x67C];  // 0x67C
    int unk7A4;                           // 0x7A4, value block
    unsigned char unk7A8[0x7B4 - 0x7A8];  // 0x7A8
    unsigned int unk7B4;                  // 0x7B4, select flags
    unsigned char unk7B8[0x7D0 - 0x7B8];  // 0x7B8
    int unk7D0;                           // 0x7D0
    unsigned char unk7D4[0x26E4 - 0x7D4]; // 0x7D4
    void* unk26E4;                        // 0x26E4, possessed host actor
};

extern "C" float lbl_804D9430;
extern "C" void fn_8008E824(int handle, int arg);
extern "C" int fn_800498D0(void* self);
int PadPressed(int pad, int arg, int button);
int Fn800F7DEC(int, int, int, int, void*);

// Per-frame update for the possession-select state: posts the two host
// property queries, routes the pad to the host's animations, refreshes the
// selection value block and picks the sub-state from the host's answer.
// Name and field meaning are guesses.
void PossessSelectFrameUpdate(void* self, float dt)
{
    PossessUpdateObj* obj = (PossessUpdateObj*)self;
    int first;
    int second;
    int result;
    int id;
    float extent;

    {
        PossessTextQuery q;

        q.hash = 0x2DA8293C;
        q.unk08 = 0;
        *(int*)&q.text = 0;
        SendMsg(obj->unk38, 0x41E, &q);
    }
    {
        PossessTextQuery q;

        q.hash = 0xDFF3097A;
        q.unk08 = 0;
        *(int*)&q.text = 0;
        SendMsg(obj->unk38, 0x41E, &q);
    }

    obj->unk638 = 0;
    obj->unk7B4 &= ~1u;

    if (PadPressed(obj->unk58C, 0, 0) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x70A1506,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 1) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x6CD1384D,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 5) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x6CD4B3B2,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 6) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x2E1823B1,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 3) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x779385F3,
                      obj->unk38);
    } else if (PadPressed(obj->unk58C, 0, 3) == 2) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0xD5CB1BA8,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 4) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x7AE5FF65,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 10) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0xD53C01A6,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 11) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0xCE5964C7,
                      obj->unk38);
    }

    fn_800769A0(obj, dt);
    PlayerSelectCollisionUpdate(obj, dt);
    obj->unk7D0 = 1;

    extent = SHOT_ABS(obj->unk394);
    if (extent < SHOT_ABS(obj->unk38C)) {
        extent = SHOT_ABS(obj->unk38C);
    }
    if (lbl_804D9430 != extent) {
        fn_8008E824(obj->unk7C, 0);
    }

    if ((obj->unk678 & 0x80) == 0) {
        PlayerSetValue4(&obj->unk7A4, 3);
    } else {
        obj->unk678 &= ~0x80u;
        PlayerSetActionMode(obj, 3);
    }

    {
        PossessTextQuery q;

        q.hash = 0x2DA8293C;
        q.unk08 = 0;
        *(int*)&q.text = 0;
        SendMsg(obj->unk38, 0x41E, &q);
        first = *(int*)&q.text;
    }
    {
        PossessTextQuery q;

        q.hash = 0xDFF3097A;
        q.unk08 = 0;
        *(int*)&q.text = 0;
        SendMsg(obj->unk38, 0x41E, &q);
        second = *(int*)&q.text;
    }

    if (obj->unk314 < 1) {
        if (obj->unk7A4 == 5) {
            PlayerSetHeldSlotMode(obj, 3);
        }
    } else {
        PlayerResetGunTimer(obj);
        id = fn_800498D0(obj);
        result = Fn800F7DEC(first, second, 0x20EE36, id, 0);
        if (result == 0x11D2C025) {
            if (obj->unk7A4 == 5) {
                PlayerSetHeldSlotMode(obj, 3);
            }
        } else if (result < 0x11D2C025) {
            if (result == 0x4A3E183) {
                PlayerSetHeldSlotMode(obj, 5);
            }
        } else if (result == 0x3DA260F7 && obj->unk7A4 == 5) {
            PlayerSetHeldSlotMode(obj, 3);
        }
    }
}

// Posts the two host property queries out of the possession-select state and
// clears the live select value; the counterpart of PossessSelectEnterHostValue.
// Name and field meaning are guesses.
void PossessSelectResetHostValue(void* self)
{
    PossessUpdateObj* obj = (PossessUpdateObj*)self;

    {
        PossessTextQuery q;

        q.hash = 0x2DA8293C;
        q.unk08 = 0;
        *(int*)&q.text = 0;
        SendMsg(obj->unk38, 0x41E, &q);
    }
    {
        PossessTextQuery q;

        q.hash = 0xDFF3097A;
        q.unk08 = 0;
        *(int*)&q.text = 0;
        SendMsg(obj->unk38, 0x41E, &q);
    }

    PlayerInHost((Actor*)self);
    obj->unk98 = -1;
}

// Per-frame update while a possession target is selected: keeps the select
// flag set, routes the pad to the host's animations, then runs the base update
// and collision pass and resets the action mode and value block. Name and
// field meaning are guesses.
void PossessSelectFrameUpdateHost(void* self, float dt)
{
    PossessUpdateObj* obj = (PossessUpdateObj*)self;

    obj->unk638 = 1;
    obj->unk7B4 &= ~1u;

    if (PadPressed(obj->unk58C, 0, 0) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x70A1506,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 1) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x6CD1384D,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 5) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x6CD4B3B2,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 6) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x2E1823B1,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 3) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x779385F3,
                      obj->unk38);
    } else if (PadPressed(obj->unk58C, 0, 3) == 2) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0xD5CB1BA8,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 4) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0x7AE5FF65,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 10) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0xD53C01A6,
                      obj->unk38);
    }
    if (PadPressed(obj->unk58C, 0, 11) == 1) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, 0xCE5964C7,
                      obj->unk38);
    }

    fn_800769A0(obj, dt);
    PlayerSelectCollisionUpdate(obj, dt);

    if ((obj->unk678 & 0x80) != 0) {
        obj->unk678 &= ~0x80u;
        PlayerSetActionMode(obj, 1);
    }

    PlayerSetValue4(&obj->unk7A4, 1);
    obj->unk7D0 = 0;
    PlayerSetActionMode(obj, 3);
}

extern "C" int fn_800C5210(int target, int msg, void* out);
extern "C" void fn_80048BA8(void* self, void* state, int flag);
int Fn800F7F5C(int, int, int, int, void*, float);

// PossessSelectValueReset: clears the select value and the per-frame flag the
// possession event dispatch uses. Name and field meaning are guesses.
void PossessSelectValueReset(void* self)
{
    PlayerSelectObj* obj = (PlayerSelectObj*)self;

    obj->unk98 = -1;
    obj->unk7D0 = 0;
}

// Possession-select object as seen by the possession event dispatch: the
// message target, the value block and its flags, the state block at 0x1D04 and
// the possessed host actor. Field meaning is a guess.
struct PossessEventObj {
    unsigned char unk000[0x38];              // 0x000
    int unk038;                              // 0x038, message target id
    unsigned char unk03C[0x7A4 - 0x03C];     // 0x03C
    int unk7A4;                              // 0x7A4, select state
    int unk7A8;                              // 0x7A8, action mode
    unsigned char unk7AC[0x7B4 - 0x7AC];     // 0x7AC
    unsigned int unk7B4;                     // 0x7B4, select flags
    unsigned char unk7B8[0x14FC - 0x7B8];    // 0x7B8
    float unk14FC;                           // 0x14FC
    unsigned char unk1500[0x1D04 - 0x1500];  // 0x1500
    unsigned char unk1D04[0x26E4 - 0x1D04];  // 0x1D04, possession state block
    int unk26E4;                             // 0x26E4, possessed host actor
};

// Possession-select event dispatch: asks the possessed host for its handle and
// for its state-name value, then reports the possession event with those
// values and drives the value block. Name, signature and field meaning are
// guesses.
int PossessSelectEventDispatch(int event, void* self)
{
    PossessEventObj* obj = (PossessEventObj*)self;
    PossessSelectHost* host;
    int first;
    int second;
    int status;
    int state;

    if (fn_800C5210(obj->unk038, 0xC06, &status) == 1) {
        return 0;
    }

    {
        PossessTextQuery q;

        q.hash = 0x2DA8293C;
        q.unk08 = 0;
        *(int*)&q.text = 0;
        SendMsg(obj->unk038, 0x41E, &q);
        first = *(int*)&q.text;
    }
    {
        PossessTextQuery q;

        q.hash = 0xDFF3097A;
        q.unk08 = 0;
        *(int*)&q.text = 0;
        SendMsg(obj->unk038, 0x41E, &q);
        second = *(int*)&q.text;
    }

    switch (event) {
    case 0x1B83833F:
        state = obj->unk7A4;
        if (state == 5) {
            if (Fn800F7DEC(first, second, 0x20EE36, fn_800498D0(obj), 0) !=
                0x4A3E183) {
                PlayerSetHeldSlotMode(obj, 3);
                return 0;
            }
            PlayerSetActionMode(obj, obj->unk7A8);
            return 0;
        }
        if (state == 8) {
            if (Fn800F7DEC(first, second, 0xF829DB93, fn_800498D0(obj), 0) !=
                0x4A3E183) {
                PlayerSetHeldSlotMode(obj, 3);
                return 0;
            }
            PlayerSetActionMode(obj, obj->unk7A8);
            return 0;
        }
        if (state == 0xD) {
            obj->unk7B4 |= 0x20000;
            return 0;
        }
        PlayerSetActionMode(obj, obj->unk7A8);
        return 0;
    case 0x20EE36:
        fn_80048BA8(obj, obj->unk1D04, 1);
        Fn800F7F5C(first, second, event, fn_800498D0(obj), obj->unk1D04,
                   obj->unk14FC);
        host = (PossessSelectHost*)obj->unk26E4;
        if (host != 0) {
            ActorPlayAnim(host->unk38, 0x8B3AF095, obj->unk038);
        }
        break;
    case 0xF829DB93:
        fn_80048BA8(obj, obj->unk1D04, 0);
        Fn800F7F5C(first, second, event, fn_800498D0(obj), obj->unk1D04,
                   obj->unk14FC);
        break;
    case 0xE6B71BCC:
    case 0x968CBE3D:
    case 0x9791AF51:
    case 0x96224FCC:
    case 0xD19E338:
    case 0x42E50A3D:
        fn_80048BA8(obj, obj->unk1D04, 1);
        Fn800F7F5C(first, second, event, fn_800498D0(obj), obj->unk1D04,
                   obj->unk14FC);
        break;
    default:
        break;
    }
    return 0;
}

// Returns the per-slot value selected in the slot table for the object's
// current value-block index. Name and meaning are guesses.
int SelectSlotValue(PlayerSelectObj* obj)
{
    return lbl_803BB9E0.slot[obj->unk7A4].value;
}

// Calls the per-slot update callback for the object's current value-block
// index, if the slot has one. Name and meaning are guesses.
void SelectSlotUpdate(PlayerSelectObj* obj)
{
    SelectSlotEntry* table = lbl_803BB9E0.slot;

    if (table[obj->unk7A4].callback != 0) {
        table[obj->unk7A4].callback(obj);
    }
}

// Leading callback view of the per-slot select table at lbl_803BB9E0: each
// 0x48-byte entry starts with two handlers that take the player-select object.
struct SelectSlotHandlers {
    void (*enter)(void*);              // 0x00
    void (*update)(void*);             // 0x04
    unsigned char unk08[0x48 - 0x08];  // 0x08
};

// Player-select object as seen by the per-slot animation callback: the three
// aim extents, the host state and the possessed host actor. Field meaning is a
// guess.
struct PossessSlotAnimObj {
    unsigned char unk000[0x38C];         // 0x000
    float unk38C;                        // 0x38C, aim extent
    float unk390;                        // 0x390, aim extent
    float unk394;                        // 0x394, aim extent
    unsigned char unk398[0x588 - 0x398]; // 0x398
    int unk588;                          // 0x588, possession host state
    unsigned char unk58C[0x7A4 - 0x58C]; // 0x58C
    int unk7A4;                          // 0x7A4, slot index into the select table
    unsigned char unk7A8[0x26E4 - 0x7A8];// 0x7A8
    void* unk26E4;                       // 0x26E4, possessed host actor
};

extern "C" float lbl_804D9434;
extern "C" float lbl_804D9438;

// Plays the aim animation matching how far the extent is from zero: the low
// animation below the first threshold, the high one above the second.
static inline void PossessSlotAnim(PossessSlotAnimObj* obj, float value,
                                   unsigned long lowId, unsigned long highId)
{
    if (value < lbl_804D9434) {
        if (obj->unk26E4 != 0) {
            ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, lowId,
                          0x2000000);
        }
    } else if (lbl_804D9438 < value && obj->unk26E4 != 0) {
        ActorPlayAnim(((PossessSelectHost*)obj->unk26E4)->unk38, highId,
                      0x2000000);
    }
}

// Per-slot callback for the possession-select states: runs the slot's update
// handler and plays the two aim animations for the current host state. Name
// and field meaning are guesses.
void PossessSelectSlotAnimUpdate(void* self)
{
    PossessSlotAnimObj* obj = (PossessSlotAnimObj*)self;
    SelectSlotHandlers* entry =
        &((SelectSlotHandlers*)&lbl_803BB9E0)[obj->unk7A4];
    int state;

    if (entry->update != 0) {
        entry->update(self);
    }

    state = obj->unk588;
    if (state == 0xB || state == 3) {
        PossessSlotAnim(obj, obj->unk390, 0x15466978, 0x93DD26AB);
        PossessSlotAnim(obj, obj->unk394, 0xA1420B94, 0x1541CE58);
    } else if (state == 2) {
        PossessSlotAnim(obj, obj->unk38C, 0x15466978, 0x93DD26AB);
        PossessSlotAnim(obj, obj->unk394, 0xA1420B94, 0x1541CE58);
    }
}

// Per-slot callback for the possession-select states: runs the slot's primary
// handler on the player-select object. Name and field meaning are guesses.
void PossessSelectSlotEnter(void* self)
{
    PossessSlotAnimObj* obj = (PossessSlotAnimObj*)self;
    SelectSlotHandlers* entry =
        &((SelectSlotHandlers*)&lbl_803BB9E0)[obj->unk7A4];

    if (entry->enter != 0) {
        entry->enter(self);
    }
}

// Looks up the sixteen assets of the possession-select screen and caches
// their handles in the lbl_8049E220 table. Asset ids and the function name
// are guesses. Kept under its fn_ name because another unit calls it.
extern unsigned long lbl_8049E220[16];

extern "C" void fn_80210B64(void)
{
    lbl_8049E220[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    lbl_8049E220[1] = AssetFind(0xF1754BF2, 0x0024805F);
    lbl_8049E220[2] = AssetFind(0xF1754BF2, 0x101844CC);
    lbl_8049E220[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    lbl_8049E220[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    lbl_8049E220[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    lbl_8049E220[6] = AssetFind(0xF1754BF2, 0xD9404234);
    lbl_8049E220[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    lbl_8049E220[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    lbl_8049E220[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    lbl_8049E220[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    lbl_8049E220[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    lbl_8049E220[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    lbl_8049E220[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    lbl_8049E220[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    lbl_8049E220[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

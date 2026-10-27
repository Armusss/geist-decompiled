// u_802CBB44.cpp: "Stone Men" actor states, actor registry and collision splash effects (0x802CBB44-0x802CED5C).
// Names are descriptive guesses; the code is matched against the retail DOL.

// The names below are descriptive guesses; the actor this unit drives is not
// identified yet.  Struct layouts are partial and derived from the retail asm.

typedef unsigned char u8;
typedef unsigned int u32;

// String/Buffer: layout guessed from the call sites (sizeof(String) == 0x14).
class Buffer {
public:
    virtual ~Buffer();              // 0x00 vptr
    /*0x04*/ void* mData;
    /*0x08*/ unsigned long mSize;
    /*0x0C*/ unsigned long mCapacity;
    /*0x10*/ unsigned long mUnk10;
};

class String : public Buffer {
public:
    String(const char* str);
    String(const String& a, const String& b);
    ~String() {}
};

// Per-actor state machine block (actor + 0x134) and its 0x18-byte table entries.
struct StateControl {
    /*0x00*/ int state;
    /*0x04*/ int pending;
    /*0x08*/ int arg;
    /*0x0C*/ int field0C;
    /*0x10*/ float field10;
    /*0x14*/ int field14;
    /*0x18*/ int field18;
    /*0x1C*/ int field1C;
};

struct StateEntry {
    /*0x00*/ int field00;
    /*0x04*/ void (*fn04)(void*);
    /*0x08*/ int (*fn08)(void*, int);
    /*0x0C*/ int (*fn0C)(void*);
    /*0x10*/ void* fn10;
    /*0x14*/ const char* name;
};

// Animation owner block the actor points at (actor + 0xC4).
struct AnimOwner {
    /*0x00*/ int field00;
    /*0x04*/ u8 unk04[0x08 - 0x04];
    /*0x08*/ float field08;
    /*0x0C*/ u8 unk0C[0x14 - 0x0C];
    /*0x14*/ int field14;
    /*0x18*/ u8 unk18[0x1C - 0x18];
    /*0x1C*/ int field1C;
    /*0x20*/ u8 unk20[0xC4 - 0x20];
    /*0xC4*/ float fieldC4;
};

// Partial layout of the actor class (only the offsets used here).
struct Actor {
    /*0x000*/ u8 unk000[0x24];
    /*0x024*/ int field024;
    /*0x028*/ int field028;
    /*0x02C*/ int field02C;
    /*0x030*/ u8 unk030[0x38 - 0x30];
    /*0x038*/ int field038;
    /*0x03C*/ u8 unk03C[0x4C - 0x3C];
    /*0x04C*/ u32 field04C;
    /*0x050*/ u8 unk050[0x84 - 0x50];
    /*0x084*/ float field084;
    /*0x088*/ float field088;
    /*0x08C*/ float field08C;
    /*0x090*/ u8 field090[0xB8 - 0x90];
    /*0x0B8*/ int field0B8;
    /*0x0BC*/ u8 unk0BC[0xC4 - 0xBC];
    /*0x0C4*/ AnimOwner* field0C4;
    /*0x0C8*/ u8 unk0C8[0xCC - 0xC8];
    /*0x0CC*/ int field0CC;
    /*0x0D0*/ int field0D0;
    /*0x0D4*/ int field0D4;
    /*0x0D8*/ int field0D8;
    /*0x0DC*/ int field0DC;
    /*0x0E0*/ u8 unk0E0[0xF0 - 0xE0];
    /*0x0F0*/ int field0F0;
    /*0x0F4*/ u8 unk0F4[0x110 - 0xF4];
    /*0x110*/ int (*field110)(void*);
    /*0x114*/ u8 unk114[0x118 - 0x114];
    /*0x118*/ void (*field118)(void*);
    /*0x11C*/ u8 unk11C[0x134 - 0x11C];
    /*0x134*/ StateControl* control;
    /*0x138*/ float field138;
    /*0x13C*/ float field13C;
    /*0x140*/ u8 unk140[0x220 - 0x140];
    /*0x220*/ int field220;
    /*0x224*/ u8 unk224[0x240 - 0x224];
    /*0x240*/ float field240;
    /*0x244*/ float field244;
    /*0x248*/ float field248;
    /*0x24C*/ float field24C;
    /*0x250*/ float field250;
    /*0x254*/ float field254;
    /*0x258*/ float field258;
    /*0x25C*/ u8 unk25C[0x26C - 0x25C];
    /*0x26C*/ int field26C;
    /*0x270*/ u8 conditions[0xC];
    /*0x27C*/ float field27C;
    /*0x280*/ u8 unk280[0x284 - 0x280];
    /*0x284*/ float field284;
    /*0x288*/ u8 unk288[0x2B0 - 0x288];
    /*0x2B0*/ float field2B0;
    /*0x2B4*/ u8 unk2B4[0x2C4 - 0x2B4];
    /*0x2C4*/ int field2C4;
    /*0x2C8*/ u8 unk2C8[0x608 - 0x2C8];
    /*0x608*/ int field608;
    /*0x60C*/ u8 unk60C[0x894 - 0x60C];
    /*0x894*/ u8 field894[0x8F8 - 0x894];
    /*0x8F8*/ int field8F8;
    /*0x8FC*/ float field8FC;
    /*0x900*/ float field900;
    /*0x904*/ u8 unk904[0x994 - 0x904];
};

struct Vec3 {
    /*0x00*/ float x;
    /*0x04*/ float y;
    /*0x08*/ float z;
};

struct Vec3i {
    /*0x00*/ int x;
    /*0x04*/ int y;
    /*0x08*/ int z;
};

struct CollisionQuery {
    /*0x00*/ Vec3 pos;
    /*0x0C*/ Vec3 off;
};

struct Msg411 {
    /*0x00*/ int unk00;
    /*0x04*/ int id;
    /*0x08*/ u8 unk08[0x34 - 0x08];
    /*0x34*/ u32 x;
    /*0x38*/ u32 y;
    /*0x3C*/ u32 z;
};

// Reply to message 0x400 sent to a potential collision target.
struct CollisionReply400 {
    /*0x00*/ u8 unk00[0x18];
    /*0x18*/ u32 flags;
};

// Small reply filled in by message 0x400 in CollisionSplashFx.
struct Msg400Reply {
    /*0x00*/ u8 unk00[0x0C];
    /*0x0C*/ int hit;
    /*0x10*/ u8 unk10[0x10];
};

// Collision record handed to the query callback (only the used offsets).
struct CollisionRecord {
    /*0x00*/ Actor* actor;
    /*0x04*/ u8 unk04[0x0C - 0x04];
    /*0x0C*/ int field0C;
    /*0x10*/ int field10;
    /*0x14*/ int field14;
    /*0x18*/ int field18;
    /*0x1C*/ u8 unk1C[0x30 - 0x1C];
    /*0x30*/ int field30;
    /*0x34*/ int field34;
    /*0x38*/ int field38;
    /*0x3C*/ int field3C;
    /*0x40*/ int field40;
    /*0x44*/ int field44;
};

// Message 0x407 payload sent to the actor that was hit.
struct HitMessage407 {
    /*0x00*/ Actor* actor;
    /*0x04*/ Actor* target;
    /*0x08*/ int field08;
    /*0x0C*/ float field0C;
    /*0x10*/ u8 unk10[0x14 - 0x10];
    /*0x14*/ u32 hash;
    /*0x18*/ int field18;
    /*0x1C*/ int field1C;
    /*0x20*/ int field20;
    /*0x24*/ int field24;
    /*0x28*/ int field28;
    /*0x2C*/ int field2C;
    /*0x30*/ int dX;
    /*0x34*/ int dY;
    /*0x38*/ int dZ;
    /*0x3C*/ int field3C;
    /*0x40*/ int field40;
    /*0x44*/ int field44;
    /*0x48*/ int field48;
    /*0x4C*/ float field4C;
    /*0x50*/ u8 unk50[0x58 - 0x50];
    /*0x58*/ int field58;
};

// Partial view of the object released by ActorReleaseCollisionBlock.
struct CollisionHolder {
    /*0x000*/ u8 unk000[0x110];
    /*0x110*/ int field110;
    /*0x114*/ u8 unk114[0x134 - 0x114];
    /*0x134*/ void* block;
};

extern StateEntry lbl_803C51F8[];
extern const char lbl_8034C0A8[];   // TODO: 0x8034C0A8 has no symbol yet
extern float lbl_804DAF98;
extern float lbl_804DAF9C;
extern float lbl_804DAFA8;
extern float lbl_804DAFAC;
extern int lbl_804D3B74;
extern int lbl_804D3B78;
extern int lbl_804D3B84;
extern int lbl_804D3B88;
extern int lbl_804D3B8C;
extern int lbl_804D3B90;
extern int lbl_804D3BDC;

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void FxStart(int handle, int a, int b);
extern "C" int fn_80019F88(void* data);
typedef int (*CollisionCallback)(void* result, void* arg);
extern "C" int fn_800201E4(void* actor, void* query, unsigned long flags,
                            CollisionCallback callback, void* arg);
extern "C" void fn_8028C188(void* dest, void* src, unsigned long len);
extern "C" int fn_8008EA9C(int owner, int index);
extern "C" int fn_8008E964(int owner);
extern "C" void fn_8008DE4C(int owner, int a, int b, int c);
extern "C" void fn_8008F230(int owner, float value, int a, int b);
extern "C" void fn_80090974(int owner, float value, int a, int b, int c,
                            void* callback, void* arg);
extern "C" void fn_800913EC(int owner, void* out, float value);
extern "C" void fn_8009F44C(void* out, void* actor, void* in);
extern "C" void fn_8016E13C(void* weapon, void* vec, int flag);
extern "C" void fn_80163378(void* obj, void* out);
extern "C" int fn_801633BC(void* obj);
extern "C" int fn_800388A4(int handle, int id);
int StoneMenTick(void* data);
void StoneMenUpdate(void* owner);
void StoneMenApplyStateAnim(void* owner, int arg);
extern "C" void fn_8029B070(void* actor, int msg);
extern "C" void fn_8029B218(void* actor, int msg);
extern "C" int fn_800A1BC8(int a, int b, int id, int variant, int c);
extern "C" int fn_8008F054(void);
extern "C" void fn_8008EE1C(int handle, int frame, int a, Vec3* pos, void* work);
extern "C" void fn_8009F63C(void* out, void* actor);
extern "C" int fn_800BB218(double scale, void* a, int b, Vec3* localPos,
                           Vec3* worldPos, void* c, int* out1, int* out2,
                           int* out3);
extern float lbl_804DAFE0;
void VFree(void* p);

int ActorCollisionHitCallback(void* result, void* arg);
void ActorReleaseCollisionBlock(void* obj);

int HasCondition(void* conditions, int id);
int MemFlagIsSet(void* conditions, int id);
void ConditionSetBit(void* conditions, int id, int value);
void RangeResetPending(void* conditions);
void ActorUpdateBoneBounds(void* actor);
void ActorPlayAnim(int owner, unsigned long anim, int arg);
int Rand();

// Partial view of the object returned by GetPlayer().
struct Player {
    /*0x000*/ u8 unk000[0x248];
    /*0x248*/ int field248;
    /*0x24C*/ u8 unk24C[0x58C - 0x24C];
    /*0x58C*/ int field58C;
    /*0x590*/ u8 unk590[0x67C - 0x590];
    /*0x67C*/ float x;
    /*0x680*/ float y;
    /*0x684*/ float z;
};

// Walk-state tuning values (all loaded with lfs).
extern float lbl_804D3B58;
extern float lbl_804D3B5C;
extern float lbl_804D3B60;
extern float lbl_804D3B64;
extern float lbl_804D3B68;
extern float lbl_804D3B6C;
extern float lbl_804D3B70;
extern float lbl_804D3B94;
extern float lbl_804D3B98;
extern float lbl_804D3B9C;
extern float lbl_804D3BA0;
extern float lbl_804D3BA4;
extern float lbl_804D3BA8;
extern float lbl_804D3BAC;
extern float lbl_804D3BB0;
extern float lbl_804D3BB4;
extern float lbl_804D3BB8;
extern float lbl_804D3BBC;
extern float lbl_804D3BC0;
extern float lbl_804D3BC4;
extern float lbl_804D3BC8;
extern float lbl_804D3BCC;
extern float lbl_804D3BD0;
extern float lbl_804DAFB0;
extern float lbl_804DAFB4;
extern float lbl_804DAFB8;
extern float lbl_804DAFBC;
extern float lbl_804DAFC0;
extern float lbl_804DAFC4;
extern float lbl_804DAFC8;
extern float lbl_804DAFCC;
extern float lbl_804DAFD0;
extern float lbl_804DAFD4;
extern float lbl_804DAFD8;
extern float lbl_804DAFDC;

// Asset / effect ids and walk-state scalars.
extern int lbl_804D3B7C;
extern int lbl_804D3B80;
extern int lbl_804D3BD4;
extern int lbl_804D3BD8;
extern int lbl_804D3BE0;
extern int lbl_804D3BE4[];
extern int lbl_804D3BE8;
extern int lbl_804D3BEC;
extern int lbl_804D3BF0;
extern int lbl_804D3BF4;
extern int lbl_804D3BF8;
extern int lbl_804D3BFC;
extern int lbl_804D57D8;

void* VAlloc(int size);
int AssetFind(unsigned long asset, unsigned long name);
int FxFind(const char* name);
Player* GetPlayer();
void HazardCreateSpline(Actor* actor, int handle, Vec3* vec, int hash,
                        float f1, float f2, float f3, float f4,
                        float f5, float f6, float f7);

extern "C" void fn_80090634(int handle, int a, int b, int c);
extern "C" void fn_800CDD10(void* arg);
extern "C" void fn_8016F120(int handle, void* pos);
extern "C" int fn_8016325C(void* state);
extern "C" void fn_80162610(void* actor, void* state, int msg);
extern "C" float fn_800CC2D0(void* actor, int a, int b, int c);
extern "C" float fn_800E1020(void* vec);
extern "C" void fn_800C5E00(int handle, int a, int b, int c, int d);
extern "C" void fn_800E1C04(int handle, int a, int b, int c, int d, int e, int f);
extern "C" void fn_802D1640(void* actor, void* vec, int hash, int a, int b,
                            float f1, float f2, float f3, float f4,
                            float f5, float f6, float f7, float f8,
                            float s1, float s2, float s3);

// Handles the messages sent to this actor (0x41e, 0xc09, 0xc14 and tick 0).
int MessageHandler802CBB44(int msg, void* data)
{
    Actor* actor;
    StateControl* ctrl;
    int* d = (int*)data;

    switch (msg) {
    case 0:
        return StoneMenTick(data);

    case 0x41e:
        actor = (Actor*)d[0];
        ctrl = actor->control;
        if (fn_80019F88(data) != 0)
            return 1;
        if (d[1] != (int)0xad8576fa)
            return 0;
        {
            String name1(lbl_803C51F8[ctrl->state].name);
            String name2(lbl_8034C0A8);
            String name3(name1, name2);

            fn_8028C188((char*)data + 0xc, name3.mData, name3.mSize);
        }
        return 0;

    case 0xc09:
        actor = (Actor*)d[0];
        ctrl = actor->control;
        if (HasCondition(actor->conditions, 9) == 0)
            return 0;
        ctrl->pending = 8;
        lbl_803C51F8[ctrl->state].fn08(actor, ctrl->pending);
        lbl_803C51F8[ctrl->pending].fn04(actor);
        ctrl->arg = d[1];
        ConditionSetBit(actor->conditions, 0xe, 1);
        fn_8008F230(actor->field0D4, lbl_804DAF98, 0, 1);
        actor->field220 = 4;
        ConditionSetBit(actor->conditions, 0xd, 1);
        fn_80090974(actor->field0D4, lbl_804DAF9C, 0, ctrl->arg, 0x58,
                    lbl_803C51F8[ctrl->state].fn10, actor);
        RangeResetPending(actor->conditions);
        ConditionSetBit(actor->conditions, 0xc, 1);
        ConditionSetBit(actor->conditions, 0xd, 1);
        actor->field13C = lbl_804DAF98;
        fn_800913EC(actor->field0D4, 0, lbl_804DAF98);
        return 1;

    case 0xc14:
        actor = (Actor*)d[0];
        ctrl = actor->control;
        if (d[3] != (int)0xcad7de50)
            return 0;
        {
            int buf[16];

            if (ctrl->state == 2) {
                if (SendMsg(actor->field608, 0x400, buf) != 0 && buf[3] != 0) {
                    int lo = lbl_804D3B74;
                    int hi = lbl_804D3B78;

                    ctrl->field18 = lo + Rand() % (hi - lo);
                } else {
                    int lo = lbl_804D3B84;
                    int hi = lbl_804D3B88;

                    ctrl->field18 = lo + Rand() % (hi - lo);
                }
            } else {
                ctrl->field18 = 1;
                ctrl->field14 = 0;
            }
        }
        return 1;
    }
    return 0;
}

// Per-frame update: state machine tick, animation, and the collision probe.
int ActorThink802CBE94(void* owner)
{
    if (owner == 0)
        return 0;

    Actor* self = (Actor*)owner;
    StateControl* ctrl = self->control;

    if (HasCondition(self->conditions, 0x1c) != 0 ||
        fn_8008EA9C(self->field0D4, 0) == 0 ||
        fn_8008EA9C(self->field0D4, 0) == 2) {
        if (HasCondition(self->conditions, 0xd) == 0 &&
            HasCondition(self->conditions, 9) == 0 &&
            HasCondition(self->conditions, 0xa) == 0 &&
            HasCondition(self->conditions, 0xb) == 0) {
            lbl_803C51F8[ctrl->state].fn04(self);
        }
        if (HasCondition(self->conditions, 0x1c) != 0 &&
            fn_8008E964(self->field0D4) != 0) {
            ActorPlayAnim(self->field038, 0xb2cb0aa4, self->field038);
        }
        if (HasCondition(self->conditions, 0x1c) != 0 &&
            HasCondition(self->conditions, 0xd) != 0 &&
            fn_8008E964(self->field0D4) != 0) {
            ActorPlayAnim(self->field038, 0xb2cb0aa4, self->field038);
            ConditionSetBit(self->conditions, 0x1c, 0);
            ConditionSetBit(self->conditions, 0x1d, 1);
        }
    }

    int next = lbl_803C51F8[ctrl->state].fn0C(self);
    if (next != ctrl->state &&
        lbl_803C51F8[ctrl->state].fn08(self, next) != 0) {
        lbl_803C51F8[next].fn04(self);
    }

    ActorUpdateBoneBounds(self);

    if (fn_801633BC(self->field894) == 1) {
        Vec3 pos;
        Vec3i rel;
        CollisionQuery query;
        Msg411 msg;

        fn_80163378(self->field894, &pos);
        rel.x = (int)pos.x - self->field024;
        rel.y = (int)pos.y - self->field028;
        rel.z = (int)pos.z - self->field02C;
        fn_8009F44C(&rel, self, &rel);
        fn_8016E13C(self->field090, &rel, 1);

        fn_80163378(self->field894, &query.pos);
        msg.id = self->field0D8;
        SendMsg(self->field038, 0x411, &msg);
        query.off.x = (float)msg.x;
        query.off.y = (float)msg.y;
        query.off.z = (float)msg.z;
        if (fn_800201E4(self, &query, 0x1200a000, ActorCollisionHitCallback, self) == 0 &&
            fn_800388A4(self->field0CC, lbl_804D3BDC) != 0) {
            msg.id = self->field0DC;
            SendMsg(self->field038, 0x411, &msg);
            query.off.x = (float)msg.x;
            query.off.y = (float)msg.y;
            query.off.z = (float)msg.z;
            fn_800201E4(self, &query, 0x2200a000, ActorCollisionHitCallback, self);
        }
    }

    return 1;
}

// Collision query callback: if the hit actor accepts (message 0x400 reply
// flag bit 1), send it our relative position as message 0x407.
// Returns 1 when the hit was reported, 0 otherwise.
int ActorCollisionHitCallback(void* result, void* arg)
{
    CollisionRecord* hit;
    Actor* other;
    CollisionReply400 reply;
    HitMessage407 msg;

    if (result == 0)
        return 0;
    hit = (CollisionRecord*)result;
    if (hit->actor == 0)
        return 0;
    if (SendMsg(hit->actor->field038, 0x400, &reply) == 0)
        return 0;
    if ((reply.flags & 2) == 0)
        return 0;

    other = (Actor*)arg;
    msg.actor = hit->actor;
    msg.target = other;
    msg.field08 = other->field038;
    msg.field0C = lbl_804DAFA8 * lbl_804DAFAC * other->field138;
    msg.hash = 0x44dc6ab;
    msg.field4C = lbl_804DAF9C;
    msg.dX = hit->actor->field024 - other->field024;
    msg.dY = hit->actor->field028 - other->field028;
    msg.dZ = hit->actor->field02C - other->field02C;
    msg.field18 = hit->field30;
    msg.field1C = hit->field34;
    msg.field20 = hit->field38;
    msg.field24 = hit->field3C;
    msg.field28 = hit->field40;
    msg.field2C = hit->field44;
    msg.field3C = hit->field10;
    msg.field40 = hit->field18;
    msg.field44 = hit->field14;
    msg.field48 = hit->field0C;
    msg.field58 = 0;
    SendMsg(msg.actor->field038, 0x407, &msg);
    return 1;
}

// Releases the collision block referenced by the object and clears the flag.
void ActorReleaseCollisionBlock(void* obj)
{
    CollisionHolder* self = (CollisionHolder*)obj;

    self->field110 = 0;
    VFree(self->block);
    self->block = 0;
}

// Spawn/init entry for the stone men: installs the update and auxiliary
// callbacks, allocates the 0x20-byte walk state block, loads the walk assets
// and enters state 0.  The first argument is unused.
void StoneMenActorInit(void* unused, void* owner)
{
    Actor* self;
    StateControl* ctrl;
    int lo;
    int hi;
    int roll;

    if (!owner)
        return;
    self = (Actor*)owner;
    self->field110 = ActorThink802CBE94;
    self->field118 = fn_800CDD10;

    ctrl = (StateControl*)VAlloc(0x20);
    if (!ctrl) {
        self->control = 0;
        return;
    }
    ctrl->state = 0;
    ctrl->pending = 0;
    ctrl->field10 = lbl_804DAF98;
    ctrl->field14 = 0;
    lo = lbl_804D3B7C;
    roll = Rand();
    hi = lbl_804D3B80;
    ctrl->field18 = lo + roll % (hi - lo);
    ctrl->field1C = (int)((((u32)__cntlzw((u32)lbl_804D57D8)) >> 5) & 0xFF);
    lbl_804D57D8 = ctrl->field1C;
    self->control = ctrl;

    lbl_804D3BD4 = AssetFind(0xF1754BF2, 0x44FFEBA);
    lbl_804D3BD8 = AssetFind(0xF1754BF2, 0x4A48BC0);
    lbl_804D3BDC = AssetFind(0xD9044CBB, 0x9CA18DD0);
    lbl_804D3BE0 = AssetFind(0xF1754BF2, 0x4B71843);
    lbl_804D3BE4[0] = FxFind("StoneMen_StartWalk_01");
    lbl_804D3BE8 = FxFind("StoneMen_StartWalk_02");
    lbl_804D3BEC = FxFind("StoneMen_Footstep");
    lbl_804D3BF4 = FxFind("stat_stun");
    lbl_804D3BF0 = FxFind("stat_laserfire");
    lbl_804D3BF8 = FxFind("stat_fall2");
    lbl_804D3BFC = FxFind("stat_fall1");
    ConditionSetBit(self->conditions, 0x1B, 1);
    fn_80090634(self->field0D4, 0, 0, 0);
    lbl_803C51F8[ctrl->state].fn04(self);
}

// State message handler for the stone men: handles the walk/jump messages,
// the stun and laser messages, and the fall-damage message.
int StoneMenStateMessage(int msg, void* owner)
{
    Actor* self;
    StateControl* ctrl;
    Msg411 m;
    Vec3 vec;
    Vec3 rel;
    Player* player;
    float f31;
    float f30;
    float f29;
    float t;
    int flag;
    int a1;
    int a2;
    int b1;
    int b2;
    int b3;
    int b4;
    int b5;
    int i;

    if (!owner)
        return 0;
    self = (Actor*)owner;
    ctrl = self->control;

    switch (msg) {
    case 0x20EE36:
    case 0xF829DB93:
        if (fn_8016325C(self->field894) == 4) {
            for (i = 0; i < 8; i++)
                fn_80162610(self, self->field894, 0x20EE36);
        } else if (fn_8016325C(self->field894) == 5) {
            fn_80162610(self, self->field894, 0xF829DB93);
        }
        break;

    case 0x1B84833F:
        lbl_803C51F8[ctrl->state].fn08(self, ctrl->pending);
        lbl_803C51F8[ctrl->pending].fn04(self);
        break;

    case 0x395E188C:
    case 0xF2BB5A17:
    case 0xB8834E2B:
    case 0x583CD858:
        flag = 0;
        f31 = fn_800CC2D0(self, 1, 0, 0);
        if (msg == 0x395E188C) {
            f30 = lbl_804D3B58;
            m.x = lbl_804D3BD4;
            f29 = lbl_804D3B5C;
        } else if (msg == 0xF2BB5A17) {
            f30 = lbl_804D3B58;
            m.x = lbl_804D3BD8;
            f29 = lbl_804D3B5C;
        } else if (msg == 0xB8834E2B) {
            flag = 1;
            f30 = lbl_804D3B60;
            m.x = lbl_804D3BD4;
            f29 = lbl_804D3B64;
        } else if (msg == 0x583CD858) {
            flag = 1;
            f30 = lbl_804D3B60;
            m.x = lbl_804D3BD8;
            f29 = lbl_804D3B64;
        }
        SendMsg(self->field038, 0x411, &m);
        vec.x = (float)m.x;
        vec.y = (float)m.y;
        vec.z = (float)m.z;
        if (lbl_804DAFB0 != f31)
            vec.z = f31 + lbl_804D3B70;
        HazardCreateSpline(self, self->field038, &vec, 0xE0490B9B,
                           lbl_804DAFB4, lbl_804DAFB8, lbl_804DAFBC,
                           lbl_804DAFBC, lbl_804DAFC0, lbl_804DAFC4,
                           lbl_804DAFB4);
        fn_8016F120(lbl_804D3BEC, &self->field024);
        if (ctrl->field14 == 0 || flag) {
            fn_802D1640(self, &vec, 0xE0490B9B, 0, 0, f30, f29,
                        lbl_804D3B68, lbl_804D3B6C,
                        lbl_804DAF98, lbl_804DAF98, lbl_804DAF98,
                        lbl_804DAF98, lbl_804DAFC8, lbl_804DAFCC,
                        lbl_804DAFD0);
        }
        player = GetPlayer();
        rel.x = player->x - vec.x;
        rel.y = player->y - vec.y;
        rel.z = player->z - vec.z;
        t = fn_800E1020(&rel);
        t = lbl_804DAF9C - (t - lbl_804D3B94) / (lbl_804D3B98 - lbl_804D3B94);
        if (t < lbl_804DAF98) {
            t = lbl_804DAF98;
        } else if (t > lbl_804DAF9C) {
            t = lbl_804DAF9C;
        }
        a1 = (int)(t * (lbl_804D3BA0 - lbl_804D3B9C) + lbl_804D3B9C);
        a2 = (int)(t * (lbl_804D3BA8 - lbl_804D3BA4) + lbl_804D3BA4);
        fn_800C5E00(player->field248, 1, a1, a2, 0);
        b1 = (int)(t * (lbl_804D3BB0 - lbl_804D3BAC) + lbl_804D3BAC);
        b2 = (int)(t * (lbl_804D3BB8 - lbl_804D3BB4) + lbl_804D3BB4);
        b3 = (int)(t * (lbl_804D3BC0 - lbl_804D3BBC) + lbl_804D3BBC);
        b4 = (int)(t * (lbl_804D3BC8 - lbl_804D3BC4) + lbl_804D3BC4);
        b5 = (int)(t * (lbl_804D3BD0 - lbl_804D3BCC) + lbl_804D3BCC);
        fn_800E1C04(player->field58C, 3, b1, b2, b4, b3, b5);
        break;
    }
    return 1;
}

// Records the requested pending state in the control block, notifies the actor
// when it is currently in state 3, and reports whether the request is the jump
// state (8).
int StoneMenSetPendingState(Actor* actor, int pending)
{
    StateControl* ctrl;

    if (actor == 0)
        return 0;
    ctrl = actor->control;
    ctrl->pending = pending;
    if (ctrl->state == 3)
        fn_8029B070(actor, 0x31992E8D);
    return pending == 8;
}

// Records the requested pending state in the control block and clears
// condition bit 0 on the actor.
int StoneMenSetPendingResetCondition(Actor* actor, int pending)
{
    StateControl* ctrl;

    if (actor == 0)
        return 0;
    ctrl = actor->control;
    ctrl->pending = pending;
    ConditionSetBit(actor->conditions, 0, 0);
    return 1;
}

// Returns the actor's current state.
int StoneMenGetState(Actor* actor)
{
    return actor->control->state;
}

// Runs the state update and then returns the actor's state.
int StoneMenUpdateAndGetState(Actor* actor)
{
    StateControl* control = actor->control;

    StoneMenUpdate(actor);
    return control->state;
}

// Timed state update: counts the state timer down, and when the entry
// conditions hold it plays the stun replies and reports the current state.
// Otherwise it asks for state 8.
int StoneMenTimedStateUpdate(void* owner)
{
    Actor* self = (Actor*)owner;
    StateControl* ctrl = self->control;

    StoneMenUpdate(owner);
    ctrl->field10 -= self->field138;
    if (!HasCondition(self->conditions, 9) ||
        HasCondition(self->conditions, 10) ||
        HasCondition(self->conditions, 11)) {
        if (ctrl->field10 <= lbl_804DAF98 &&
            (fn_8016325C(self->field894) == 4 ||
             fn_8016325C(self->field894) == 5)) {
            fn_80162610(self, self->field894, 0xF829DB93);
            if (fn_800388A4(self->field0CC, lbl_804D3BDC) != 0)
                fn_80162610(self, self->field894, 0x20EE36);
            fn_80162610(self, self->field894, 0x1B83833F);
            fn_80162610(self, self->field894, 0);
        }
        return ctrl->state;
    }
    return 8;
}

// Plain state update: while the entry conditions do not hold, run the shared
// state tick and report the current state; otherwise ask for state 8.
int StoneMenStateUpdate(void* owner)
{
    Actor* self = (Actor*)owner;
    StateControl* ctrl = self->control;

    if (HasCondition(self->conditions, 9) &&
        !HasCondition(self->conditions, 10) &&
        !HasCondition(self->conditions, 11)) {
        return 8;
    }
    StoneMenUpdate(owner);
    return ctrl->state;
}

// Charge-state tick: while the entry conditions hold, clears the stun flag
// request and runs the shared state tick; otherwise asks for state 8.
int StoneMenChargeStateUpdate(void* owner)
{
    Actor* self = (Actor*)owner;
    StateControl* ctrl = self->control;

    if (!HasCondition(self->conditions, 9) ||
        HasCondition(self->conditions, 10) ||
        HasCondition(self->conditions, 11)) {
        if (!MemFlagIsSet(self->conditions, 0x24))
            ctrl->pending = 7;
        StoneMenUpdate(owner);
        return ctrl->state;
    }
    return 8;
}

// Charge-state tick with an early aim abort: requests state 3 when the stun
// flag is set, or when the actor has no target and is looking too far up or
// down; then runs the shared state tick.
int StoneMenChargeAimStateUpdate(void* owner)
{
    Actor* self = (Actor*)owner;
    StateControl* ctrl = self->control;

    if (!HasCondition(self->conditions, 9) ||
        HasCondition(self->conditions, 10) ||
        HasCondition(self->conditions, 11)) {
        if (MemFlagIsSet(self->conditions, 0x24))
            ctrl->pending = 3;
        if (self->field26C == 0 || __fabs(self->field284) > lbl_804DAFD4)
            ctrl->pending = 3;
        StoneMenUpdate(owner);
        return ctrl->state;
    }
    return 8;
}

// Stun/charge state update: once the entry conditions fail it checks how far
// the actor is aimed off-level and either requests the stun state (4), keeps
// the current state, or asks for state 8.
int StoneMenStunStateUpdate(void* owner)
{
    Actor* self = (Actor*)owner;
    StateControl* ctrl = self->control;

    StoneMenUpdate(owner);
    if (!HasCondition(self->conditions, 9) ||
        HasCondition(self->conditions, 10) ||
        HasCondition(self->conditions, 11)) {
        if (__fabs(self->field284) <= lbl_804DAFD8) {
            if (MemFlagIsSet(self->conditions, 0x24))
                return 5;
            if (self->field26C != 0)
                return 1;
            if (fn_8016325C(self->field894) == 4 ||
                fn_8016325C(self->field894) == 5) {
                fn_80162610(self, self->field894, 0xF829DB93);
                if (fn_800388A4(self->field0CC, lbl_804D3BDC) != 0)
                    fn_80162610(self, self->field894, 0x20EE36);
                fn_80162610(self, self->field894, 0x1B83833F);
                fn_80162610(self, self->field894, 0);
                fn_8016F120(lbl_804D3BF0, &self->field024);
            }
            return ctrl->state;
        }
        if (self->field284 <= lbl_804DAFDC)
            ctrl->field0C = 1;
        else
            ctrl->field0C = 0;
        return 4;
    }
    return 8;
}

// Jumps the actor straight into state 9 (the stun state) and starts the
// matching 0x18-sized state animation on its state owner.
int StoneMenSetStateNine(void* owner)
{
    if (owner == 0)
        return 0;

    Actor* self = (Actor*)owner;
    StateControl* ctrl = self->control;

    ctrl->state = 9;
    ctrl->pending = 9;
    fn_8008DE4C(self->field0D4, 0, ctrl->arg, 0x18);
    return 1;
}

// Sends collision message 0x400 to the actor's target and starts the splash
// effect that belongs to the reply.  Returns 0 without an owner.
int CollisionSplashFx(void* owner)
{
    Actor* self;
    StateControl* ctrl;
    Msg400Reply reply;
    int result;

    if (owner == 0)
        return 0;

    self = (Actor*)owner;
    ctrl = self->control;
    ctrl->state = 8;
    ctrl->pending = 9;
    result = SendMsg(self->field608, 0x400, &reply);
    if (result != 0 && reply.hit != 0)
        FxStart(lbl_804D3BF8, 0, 0);
    else
        FxStart(lbl_804D3BFC, 0, 0);
    return 1;
}

// Enters state 7 after a hit, clears the two hit conditions and hands the
// state argument on to the hit reaction.  Returns 0 without an owner.
int EnterHitState(void* owner)
{
    Actor* self;
    StateControl* ctrl;

    if (owner == 0)
        return 0;

    self = (Actor*)owner;
    ctrl = self->control;
    ctrl->state = 7;
    ctrl->pending = 0;
    ConditionSetBit(self->conditions, 0, 1);
    ConditionSetBit(self->conditions, 0x3b, 0);
    StoneMenApplyStateAnim(self, ctrl->field0C);
    return 1;
}

// Enters state 6, sets the two entry conditions and hands the state argument
// on.  Returns 0 without an owner.
int EnterStunState(void* owner)
{
    Actor* self;
    StateControl* ctrl;

    if (owner == 0)
        return 0;

    self = (Actor*)owner;
    ctrl = self->control;
    ctrl->state = 6;
    ctrl->pending = 6;
    ConditionSetBit(self->conditions, 0, 1);
    ConditionSetBit(self->conditions, 0x3b, 1);
    StoneMenApplyStateAnim(self, ctrl->field0C);
    return 1;
}

// Enters state 5, sets condition 0, hands the state argument on and notifies
// the stun fx with the actor's position.  Returns 0 without an owner.
int EnterStunFxState(void* owner)
{
    Actor* self;
    StateControl* ctrl;

    if (owner == 0)
        return 0;

    self = (Actor*)owner;
    ctrl = self->control;
    ctrl->state = 5;
    ctrl->pending = 6;
    ConditionSetBit(self->conditions, 0, 1);
    StoneMenApplyStateAnim(self, ctrl->field0C);
    fn_8016F120(lbl_804D3BF4, &self->field024);
    return 1;
}

// Enters state 4, clears the pending state and the state argument, and hands
// the (zero) state argument on to the shared state entry.  Returns 0 without
// an owner.
bool StoneMenEnterStateFour(void* owner)
{
    Actor* self;
    StateControl* ctrl;

    if (owner == 0)
        return false;

    self = (Actor*)owner;
    ctrl = self->control;
    ctrl->state = 4;
    ctrl->pending = 0;
    ConditionSetBit(self->conditions, 0, 1);
    StoneMenApplyStateAnim(self, ctrl->field0C);
    return true;
}

// Enters state 3, clears the pending state, the state argument and the
// reaction field, sets condition 0 and notifies the walk handler before handing
// the (zero) state argument to the shared state entry.  Returns 0 without an
// owner.
bool StoneMenEnterStateThree(void* owner)
{
    Actor* self;
    StateControl* ctrl;

    if (owner == 0)
        return false;

    self = (Actor*)owner;
    ctrl = self->control;
    ctrl->state = 3;
    ctrl->pending = 0;
    ctrl->field14 = 0;
    ConditionSetBit(self->conditions, 0, 1);
    fn_8029B218(self, 0x31992E8D);
    StoneMenApplyStateAnim(self, ctrl->field0C);
    return true;
}

// Walk-step probe used while the stone man is already walking: samples the
// walk animation at its last and first frame, projects the resulting foot
// displacement into the world and tests it against the floor.  A floor hit
// enters state 3; otherwise state 2 is entered and the delay until the next
// animation variant is re-rolled (different ranges depending on whether the
// optional target accepted our 0x400 query).
int StoneMenProbeWalkState(void* owner)
{
    Actor* self;
    StateControl* ctrl;
    Vec3 animPos;
    u8 work[0xFE0];
    float mtx[12];
    Vec3i rel;
    Vec3i sample;
    Vec3 localPos;
    Vec3 worldPos;
    int out1;
    int out2;
    int out3;
    int handle;
    int frames;

    if (owner == 0)
        return 0;

    self = (Actor*)owner;
    ctrl = self->control;
    handle = fn_800A1BC8(self->field0C4->field14, self->field0C4->field1C,
                         lbl_803C51F8[2].field00,
                         (ctrl->field0C + 1) % 2, 0);
    frames = fn_8008F054();
    fn_8008EE1C(handle, frames - 1, self->field0CC, &animPos, work);
    rel.x = (int)animPos.x;
    rel.y = (int)animPos.y;
    rel.z = (int)animPos.z;
    fn_8008EE1C(handle, 0, self->field0CC, &animPos, work);
    rel.x -= (int)animPos.x;
    rel.y -= (int)animPos.y;
    rel.z -= (int)animPos.z;

    fn_8009F63C(mtx, self);
    sample.x = self->field024;
    sample.y = self->field028;
    sample.z = self->field02C;
    localPos.x = (float)(u32)self->field024;
    localPos.y = (float)(u32)self->field028;
    localPos.z = (float)(u32)self->field02C;
    fn_8009F44C(&sample, mtx, &rel);
    worldPos.x = (float)(u32)sample.x * lbl_804DAFE0 + self->field084;
    worldPos.y = (float)(u32)sample.y * lbl_804DAFE0 + self->field088;
    worldPos.z = (float)(u32)sample.z * lbl_804DAFE0 + self->field08C;

    if (fn_800BB218(self->field900, self, self->field2C4, &localPos, &worldPos,
                    self, &out1, &out2, &out3) != 0) {
        // Enter state 3 (the shared entry helper re-tests its argument).
        if (owner == 0)
            return 0;
        self = (Actor*)owner;
        ctrl = self->control;
        ctrl->state = 3;
        ctrl->pending = 0;
        ctrl->field14 = 0;
        ConditionSetBit(self->conditions, 0, 1);
        fn_8029B218(self, 0x31992E8D);
        StoneMenApplyStateAnim(self, ctrl->field0C);
        return 1;
    }

    ctrl->state = 2;
    ctrl->pending = 2;
    {
        int reply[8];

        if (SendMsg(self->field608, 0x400, reply) != 0 && reply[3] != 0) {
            if (ctrl->field14 == 0) {
                if (--ctrl->field18 == 0) {
                    ctrl->field18 = lbl_804D3B74 +
                                    Rand() % (lbl_804D3B78 - lbl_804D3B74);
                    ctrl->field14 = 1;
                }
            } else {
                if (--ctrl->field18 == 0) {
                    ctrl->field18 = lbl_804D3B7C +
                                    Rand() % (lbl_804D3B80 - lbl_804D3B7C);
                    ctrl->field14 = 0;
                }
            }
        } else {
            if (ctrl->field14 == 0) {
                if (--ctrl->field18 == 0) {
                    ctrl->field18 = lbl_804D3B84 +
                                    Rand() % (lbl_804D3B88 - lbl_804D3B84);
                    ctrl->field14 = 1;
                }
            } else {
                if (--ctrl->field18 == 0) {
                    ctrl->field18 = lbl_804D3B8C +
                                    Rand() % (lbl_804D3B90 - lbl_804D3B8C);
                    ctrl->field14 = 0;
                }
            }
        }
    }
    ctrl->field0C++;
    if (ctrl->field0C > 1)
        ctrl->field0C = 0;
    ConditionSetBit(self->conditions, 0, 1);
    fn_8029B218(self, 0x31992E8D);
    StoneMenApplyStateAnim(self, ctrl->field0C);
    return 1;
}

// Start-walk entry: same animation/foot probe as the walk-step handler.  When
// no floor contact is found the walk-start fx for the current variant is
// played at the actor position and state 1 is entered.
int StoneMenBeginWalkState(void* owner)
{
    Actor* self;
    StateControl* ctrl;
    Vec3 animPos;
    u8 work[0xFE0];
    float mtx[12];
    Vec3i rel;
    Vec3i sample;
    Vec3 localPos;
    Vec3 worldPos;
    int out1;
    int out2;
    int out3;
    int handle;
    int frames;

    if (owner == 0)
        return 0;

    self = (Actor*)owner;
    ctrl = self->control;
    handle = fn_800A1BC8(self->field0C4->field14, self->field0C4->field1C,
                         lbl_803C51F8[1].field00, 0, 0);
    frames = fn_8008F054();
    fn_8008EE1C(handle, frames - 1, self->field0CC, &animPos, work);
    rel.x = (int)animPos.x;
    rel.y = (int)animPos.y;
    rel.z = (int)animPos.z;
    fn_8008EE1C(handle, 0, self->field0CC, &animPos, work);
    rel.x -= (int)animPos.x;
    rel.y -= (int)animPos.y;
    rel.z -= (int)animPos.z;

    fn_8009F63C(mtx, self);
    sample.x = self->field024;
    sample.y = self->field028;
    sample.z = self->field02C;
    localPos.x = (float)(u32)self->field024;
    localPos.y = (float)(u32)self->field028;
    localPos.z = (float)(u32)self->field02C;
    fn_8009F44C(&sample, mtx, &rel);
    worldPos.x = (float)(u32)sample.x * lbl_804DAFE0 + self->field084;
    worldPos.y = (float)(u32)sample.y * lbl_804DAFE0 + self->field088;
    worldPos.z = (float)(u32)sample.z * lbl_804DAFE0 + self->field08C;

    if (fn_800BB218(self->field900, self, self->field2C4, &localPos, &worldPos,
                    self, &out1, &out2, &out3) != 0)
        return 0;

    fn_8016F120(lbl_804D3BE4[ctrl->field1C], &self->field024);
    ctrl->state = 1;
    ctrl->pending = 2;
    ctrl->field0C = 0;
    ctrl->field14 = 0;
    ConditionSetBit(self->conditions, 0, 1);
    fn_8029B218(self, 0x31992E8D);
    StoneMenApplyStateAnim(self, 0);
    return 1;
}

// Clears the actor's state-control block (state, pending and argument) and
// restarts the behaviour machine.  Returns 1 when an actor was given.
int StoneMenClearState(void* owner)
{
    Actor* self = (Actor*)owner;
    StateControl* ctrl;

    if (self) {
        ctrl = self->control;
        ctrl->state = 0;
        ctrl->pending = 0;
        ctrl->field0C = 0;
        StoneMenApplyStateAnim(self, 0);
    }
    return self != 0;
}

extern float lbl_804DAFE4;
extern double lbl_804DAFA0;
extern float lbl_804DAFE8;
extern "C" void* fn_800AFD20(void* actor);
extern "C" void* fn_8003A1F0(void* a, int b, int c);
extern "C" void fn_8003F15C(float t, void* a, void* b, int c, void* pos, int e);
extern "C" unsigned int fn_800BA84C(int a);
int ModelAnimate(int handle, char* name, int a);
void Draw3DBeginLine(int a, void* b, float c);
void DrawLine(float* a, float* b, unsigned char c1, unsigned char c2,
              unsigned char c3, unsigned char c4, unsigned char c5,
              unsigned char c6, unsigned char c7, unsigned char c8);
void DrawDisableTexOffsets();

// Per-tick handler: plays the hurt animation when the timer runs down, drives
// the character model, and (in the debug view) draws a line from the tracked
// ground point to the position the actor reports back for message 0x411.
int StoneMenTick(void* data)
{
    int* d = (int*)data;
    Actor* self = (Actor*)d[0];
    void* anim;
    void* model;

    if (lbl_804DAFE4 < self->field13C)
        ActorPlayAnim(self->field038, 0xdbe2cae6, self->field038);
    self->field13C = lbl_804DAF98;

    anim = fn_800AFD20(self);
    model = fn_8003A1F0(anim, self->field0F0,
                        ModelAnimate(self->field0CC, 0, 0));

    if (self->field8F8 == 6) {
        unsigned int r = fn_800BA84C(9);
        float f = (float)r;
        float t;

        f = (f - self->field8FC) / f;
        t = lbl_804DAF98;
        if (f >= lbl_804DAF98) {
            t = lbl_804DAF9C;
            if (f <= lbl_804DAF9C)
                t = f;
        }
        if (t > lbl_804DAF98)
            fn_8003F15C(t, anim, model, self->field0D0, &d[4], 0);
    } else {
        fn_8003F15C(lbl_804DAF9C, anim, model, self->field0D0, &d[4], 0);
    }

    if (fn_801633BC(&self->field894) == 1) {
        Vec3 linePos;
        Vec3 tracked;
        Msg411 msg;

        fn_80163378(&self->field894, &tracked);
        msg.id = self->field0D8;
        SendMsg(self->field038, 0x411, &msg);
        linePos.x = (float)msg.x;
        linePos.y = (float)msg.y;
        linePos.z = (float)msg.z;
        Draw3DBeginLine(-3, 0, lbl_804DAFE8);
        DrawLine(&linePos.x, &tracked.x, 0xff, 100, 100, 0x32, 0xff, 0x14,
                 0x14, 0xff);
        DrawDisableTexOffsets();
        if (fn_800388A4(self->field0CC, lbl_804D3BDC) != 0) {
            msg.id = self->field0DC;
            SendMsg(self->field038, 0x411, &msg);
            linePos.x = (float)msg.x;
            linePos.y = (float)msg.y;
            linePos.z = (float)msg.z;
            Draw3DBeginLine(-3, 0, lbl_804DAFE8);
            DrawLine(&linePos.x, &tracked.x, 0xff, 100, 100, 0x32, 0xff,
                     0x14, 0x14, 0xff);
            DrawDisableTexOffsets();
        }
    }
    return 1;
}

extern float lbl_804DAFEC;
extern float lbl_804DAFF0;
extern float lbl_804DAFF4;
extern float lbl_804DAFF8;
extern float lbl_804DAFFC;
extern float lbl_804DB000;
extern float lbl_804DB004;
extern float lbl_804DB008;

struct Mtx34 {
    /*0x00*/ float m[12];
};

struct PlaceF {
    /*0x00*/ float x;
    /*0x04*/ float y;
    /*0x08*/ float z;
};

void ActorAim(int* out, Actor* actor, int arg);
int ModelFindBone(int model, int asset);
void BoneAddRotX(void* model, int bone, float angle);
void SplineQuatRotateRamp(float angle, float* out, int boneA, int boneB,
                          unsigned long model);
int AnimBankGetSetCount(unsigned short a, unsigned short b, unsigned short c);
void AnimApply(int handle, char* name, int a, int b);
void ModelSetMatrix(int model, Mtx34* mtx, char* name);
void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void MtxSetEuler(Mtx34* mtx, Vec3* euler);
void EulerNormalize(Vec3* out, Vec3* in);
void Vec3Set(Vec3* out, float x, float y, float z);
void PlacementToMatrix(Mtx34* mtx, PlaceF* place);
extern "C" void fn_800A0168(Mtx34* a, Mtx34* b);
extern "C" void fn_800A0030(Vec3* out, Mtx34* mtx, Vec3* in);
extern "C" void fn_800E1098(float scale, Vec3* out, Vec3* in);
extern "C" void fn_8008F4E8(int handle, int a);
extern "C" void fn_8008F378(int handle);
extern "C" void fn_8008F4EC(int handle, int a);
extern "C" int fn_8008E7D0(void);
extern "C" int rand(void);

// Shared per-tick state update: aims the actor, drives the animation and the
// model matrix, integrates the turn/lean rates of the current state and
// finally places the model.  Name is a guess from the state-machine callers.
void StoneMenUpdate(void* owner)
{
    Actor* self = (Actor*)owner;
    StateControl* ctrl = self->control;
    float* anim;
    AnimOwner* animOwner;

    if (self->field0F0 == 0)
        return;
    if (fn_8008E964(self->field0D4) == 0 &&
        HasCondition(self->conditions, 0xc) != 0)
        return;

    anim = (float*)self->field0F0;
    {
        int aim[3];
        Vec3 pos;
        Vec3 placement;

        ActorAim(aim, self, 1);
        pos.x = (float)aim[0];
        pos.y = (float)aim[1];
        pos.z = (float)aim[2];
        fn_800E1098(lbl_804DAF9C, &pos, &pos);
        fn_800913EC(self->field0D4, &placement, self->field138);

        AnimApply(self->field0D4, (char*)self->field0F0,
                  ModelAnimate(self->field0CC, (char*)self->field0F0, 1),
                  self->field0D0);
    }

    if (!HasCondition(self->conditions, 0xa) &&
        !HasCondition(self->conditions, 9) &&
        !HasCondition(self->conditions, 0xb) &&
        !HasCondition(self->conditions, 0xc)) {
        int boneA = ModelFindBone(self->field0CC,
                                  AssetFind(0xF1754BF2, 0x4B71843));
        int boneB = ModelFindBone(self->field0CC,
                                  AssetFind(0xF1754BF2, 0x21C5E0));

        BoneAddRotX((void*)self->field0F0, boneB, self->field258);
        SplineQuatRotateRamp(self->field24C, anim, boneA, boneB,
                             (unsigned long)self->field0C4->field00);
    }

    if (!HasCondition(self->conditions, 0xd) &&
        !HasCondition(self->conditions, 0xc)) {
        float ground = fn_800CC2D0(self, 0, 0, 0);

        if (ground != lbl_804DAFB0)
            self->field08C = ground + anim[6];
        self->field02C = (int)self->field08C;
        anim[4] = lbl_804DAF98;
        anim[5] = lbl_804DAF98;
        anim[6] = lbl_804DAF98;

        animOwner = self->field0C4;
        {
            float rate = animOwner->fieldC4;
            float pitch = self->field284;
            float turnRate = lbl_804DAFC4 * rate;
            float roll = self->field27C;

            if (ctrl->state != 4)
                rate = rate * lbl_804DAFEC;

            if (pitch > lbl_804DAFDC) {
                float t = (rate * self->field138) / lbl_804DAFF0;

                if (t >= pitch)
                    t = pitch;
                self->field254 += t;
            } else if (pitch < lbl_804DAFF4) {
                float t = -((rate * self->field138) / lbl_804DAFF0);

                if (t <= pitch)
                    t = pitch;
                self->field254 += t;
            }

            if (roll > lbl_804DAFDC) {
                float t = (turnRate * self->field138) / lbl_804DAFF0;

                if (t >= roll)
                    t = roll;
                self->field24C += t;
            } else if (roll < lbl_804DAFF4) {
                float t = -((turnRate * self->field138) / lbl_804DAFF0);

                if (t <= roll)
                    t = roll;
                self->field24C += t;
            }

            {
                float limit = animOwner->field08;

                if (self->field24C < -limit)
                    self->field24C = -limit;
                else if (self->field24C > limit)
                    self->field24C = limit;
            }

            {
                float step = lbl_804DAFFC *
                             (lbl_804DAFA8 * self->field138);

                if (__fabs(self->field2B0) > lbl_804DAFF8) {
                    if (self->field258 > lbl_804DAF98) {
                        self->field258 -= step;
                        if (self->field258 < lbl_804DAF98)
                            self->field258 = lbl_804DAF98;
                    } else if (self->field258 < lbl_804DAF98) {
                        self->field258 += step;
                        if (self->field258 > lbl_804DAF98)
                            self->field258 = lbl_804DAF98;
                    }
                } else {
                    if (self->field258 > self->field2B0) {
                        self->field258 -= step;
                        if (self->field258 < self->field2B0)
                            self->field258 = self->field2B0;
                    } else if (self->field258 < self->field2B0) {
                        self->field258 += step;
                        if (self->field258 > self->field2B0)
                            self->field258 = self->field2B0;
                    }
                }
            }
        }
    }

    if (HasCondition(self->conditions, 0) != 0) {
        Mtx34 mtx;
        Vec3 v;

        v.x = lbl_804DAF98;
        v.y = lbl_804DAF98;
        v.z = lbl_804DAF98;
        ActorGetMatrix(&mtx, self);
        fn_800A0168(&mtx, &mtx);
        fn_800A0030(&v, &mtx, &v);
        if (self->field138 > lbl_804DAF98) {
            float inv = lbl_804DAFF0 / self->field138;

            self->field240 = v.x;
            self->field244 = v.y;
            self->field248 = v.z;
            self->field248 = lbl_804DAF98;
            self->field240 *= inv;
            self->field244 *= inv;
            self->field248 *= inv;
        } else {
            self->field240 = lbl_804DAF98;
            self->field244 = lbl_804DAF98;
            self->field248 = lbl_804DAF98;
        }
    }

    EulerNormalize((Vec3*)&self->field24C, (Vec3*)&self->field24C);
    {
        Mtx34 modelMtx;
        Mtx34 actorMtx;
        Vec3 lean;
        Vec3 offset;
        PlaceF place;

        Vec3Set(&lean, self->field254, lbl_804DAF98, lbl_804DAF98);
        EulerNormalize(&lean, &lean);
        MtxSetEuler(&modelMtx, &lean);
        place.x = (float)self->field024;
        place.y = (float)self->field028;
        place.z = (float)self->field02C;
        PlacementToMatrix(&modelMtx, &place);
        ActorGetMatrix(&actorMtx, self);
        fn_800A0168(&actorMtx, &actorMtx);
        offset.x = lbl_804DAF98;
        offset.y = lbl_804DAF98;
        offset.z = lbl_804DAF98;
        fn_800A0030(&offset, &actorMtx, &offset);
        place.x += offset.x;
        place.y += offset.y;
        place.z += offset.z;
        ModelSetMatrix(self->field0CC, &modelMtx, (char*)self->field0F0);
    }
}

// Applies the state animation: picks the animation set for the actor's current
// state, starts it on the actor's state owner and records the resulting handle
// as the state argument.  Name is a guess from the state-entry callers.
void StoneMenApplyStateAnim(void* owner, int variant)
{
    Actor* self = (Actor*)owner;
    StateControl* ctrl;
    AnimOwner* animOwner;
    float scale = lbl_804DAF9C;
    int count;
    int handle;

    if (self == 0)
        return;

    ctrl = self->control;

    if (HasCondition(self->conditions, 0xc) != 0) {
        ConditionSetBit(self->conditions, 0xc, 0);
        ConditionSetBit(self->conditions, 0xe, 1);
        fn_8008F4E8(self->field0D4, 0);
        fn_8008F378(self->field0D4);
        fn_8008F4EC(self->field0D4, 0);
    }

    animOwner = self->field0C4;
    count = AnimBankGetSetCount((unsigned short)animOwner->field14,
                                (unsigned short)animOwner->field1C,
                                (unsigned short)lbl_803C51F8[ctrl->state].field00);
    if (count == 0)
        return;
    if (variant >= count)
        return;
    if (variant < 0)
        variant = rand() % count;

    if (ctrl->field14 == 1 &&
        (ctrl->state == 1 || ctrl->state == 2 || ctrl->state == 3)) {
        Msg400Reply reply;
        int result = SendMsg(self->field608, 0x400, &reply);

        if (result == 0 || reply.hit == 0)
            scale = lbl_804DB004;
        else
            scale = lbl_804DB000;
    }

    handle = fn_800A1BC8(animOwner->field14, animOwner->field1C,
                         lbl_803C51F8[ctrl->state].field00, variant, 0);
    if (fn_8008E7D0() != 0) {
        ConditionSetBit(self->conditions, 0x1c, 0);
        if (HasCondition(self->conditions, 0xe) != 0) {
            ConditionSetBit(self->conditions, 0xe, 0);
            fn_8008F230(self->field0D4, lbl_804DAF98, 0, 0);
        } else {
            fn_8008F230(self->field0D4, lbl_804DB008, 0, 1);
        }
        fn_80090974(self->field0D4, scale, 0, handle, 9,
                    lbl_803C51F8[ctrl->state].fn10, self);
        ctrl->arg = handle;
    }
}

// Global actor registry the two helpers below walk (lbl_804D57E0).  Only the
// fields they touch are modelled: 0x08 is the byte size of the 0x0C array of
// actor pointers; the class is polymorphic (the teardown uses delete).  This is
// the intermediate level of the container's polymorphic chain; its destructor
// runs the engine-side cleanup fn_8028B960 before the concrete level frees the
// storage.
extern "C" void fn_8028B960(void);

class ActorRegistry {
public:
    virtual ~ActorRegistry() { fn_8028B960(); }     // 0x00 vptr
    /*0x04*/ int field04;
    /*0x08*/ unsigned long byteSize;
    /*0x0C*/ Actor** items;
};

// Concrete 0x14-byte container the global handle points at (the trailing word
// is the extra field that brings it up to the allocation size).
class RegistryBuffer : public ActorRegistry {
public:
    void* operator new(unsigned long size) { return VAlloc((int)size); }
    virtual ~RegistryBuffer();                      // 0x00 vptr
    /*0x10*/ int field10;
};

extern RegistryBuffer* lbl_804D57E0;
extern "C" float fn_802A2080(Actor* actor);
extern "C" int fn_802CED5C(float value, Actor* actor);
extern "C" void fn_8028B35C(void* list, void* element, unsigned long size);
void FontRelease(int font);
void ActorUnlinkOwnerObjects(Actor* actor);

// Destroys every actor of the global registry for which fn_802CED5C rejects the
// given value (the value is scaled per actor unless the actor's 0x20000 flag is
// set) and shrinks the registry in place.  Names are guesses from the context.
// fn_802CEAAC
extern "C" void fn_802CEAAC(float value)
{
    Actor** it = lbl_804D57E0->items;

    while (it !=
           (Actor**)((u8*)lbl_804D57E0->items + lbl_804D57E0->byteSize)) {
        float scaled = value;

        if (((*it)->field04C & 0x20000) == 0)
            scaled = value * fn_802A2080(*it);
        if (fn_802CED5C(scaled, *it) != 0) {
            it++;
            continue;
        } else {
            Actor* actor = *it;
            Actor** found = lbl_804D57E0->items;
            Actor** end = (Actor**)((u8*)lbl_804D57E0->items +
                                    lbl_804D57E0->byteSize);

            for (; found != end; found++) {
                if (*found == actor) {
                    fn_8028B35C(lbl_804D57E0, found, sizeof(Actor*));
                    if (actor->field0B8 != -1) {
                        FontRelease(actor->field0B8);
                        actor->field0B8 = -1;
                    }
                    ActorUnlinkOwnerObjects(actor);
                    VFree(actor);
                    break;
                }
            }
        }
    }
}

// Tears the global actor registry down: destroys every registered actor (same
// body as the range helper above, always taking the first element) and then
// deletes the registry itself.  Name is a guess from the context.
// fn_802CEBAC
extern "C" void fn_802CEBAC(void)
{
    if (lbl_804D57E0 != 0) {
        while ((lbl_804D57E0->byteSize >> 2) != 0) {
            Actor* actor = lbl_804D57E0->items[0];
            Actor** it = lbl_804D57E0->items;
            Actor** end = (Actor**)((u8*)lbl_804D57E0->items +
                                    lbl_804D57E0->byteSize);

            for (; it != end; it++) {
                if (*it == actor) {
                    fn_8028B35C(lbl_804D57E0, it, sizeof(Actor*));
                    if (actor->field0B8 != -1) {
                        FontRelease(actor->field0B8);
                        actor->field0B8 = -1;
                    }
                    ActorUnlinkOwnerObjects(actor);
                    VFree(actor);
                    break;
                }
            }
        }
        delete lbl_804D57E0;
        lbl_804D57E0 = 0;
    }
}

// Destroys the global registry container.  The intermediate level's destructor
// (ActorRegistry) runs its engine-side cleanup, then this level releases the
// storage; the trailing flag the compiler threads through the destructor frees
// the object.  Name is a guess from the context.
RegistryBuffer::~RegistryBuffer()
{
}

extern "C" void fn_80026660(int slot, void (*callback)(void), void* data);
extern "C" void fn_802CF9AC(void);

// Registers the registry's update callback (slot 0x1a) and lazily creates the
// global registry container.  Names are guesses from the call sites.
// fn_802CECE8
extern "C" void fn_802CECE8(void)
{
    fn_80026660(0x1a, fn_802CF9AC, 0);

    if (lbl_804D57E0 == 0)
        lbl_804D57E0 = new RegistryBuffer;
}

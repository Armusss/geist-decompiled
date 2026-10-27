// u_802B7120.cpp: actor message handlers. Function names are descriptive guesses:
// the actor that owns the +0x130 state block has not been identified yet.

struct Owner;

struct ActorVitals {
    unsigned char pad00[0x30];
    int max;                    // 0x30
};

struct ExtraState {
    int unk00;                  // 0x00
    unsigned char pad04[0x08];  // 0x04
    float f0c;                  // 0x0c
    float f10;                  // 0x10
    float f14;                  // 0x14
    int total;                  // 0x18
    int remaining;              // 0x1c
    int selected;               // 0x20
    int unk24[2];               // 0x24
    int size;                   // 0x2c  (byte size of list)
    int* list;                  // 0x30
    unsigned char pad34[0x08];  // 0x34
    int f3c;                    // 0x3c  follow target acquired
    int slot;                   // 0x40
};

struct Actor {
    unsigned char pad00[0x24];
    unsigned char timer[0x14];  // 0x24  delayed-call timer object
    int id;                     // 0x38
    unsigned char pad3c[0x4c];
    int* i88;                   // 0x88  interaction source descriptor
    unsigned char pad8c[0x1c];
    int iA8;                    // 0xa8
    unsigned char padac[0x18];
    ActorVitals* vitals;        // 0xc4
    unsigned char padc8[0x04];
    int model;                  // 0xcc
    int d0;                     // 0xd0
    int d4;                     // 0xd4
    unsigned char padd8[0x18];
    int f0;                     // 0xf0
    unsigned char padf4[0x08];
    void (*cbFC)(void*);        // 0xfc
    int i100;                   // 0x100
    int (*cb104)(Actor*);       // 0x104
    void (*cb108)(void*);       // 0x108
    void (*cb10c)(void*);       // 0x10c
    unsigned char pad110[0x20]; // 0x110
    ExtraState* extra;          // 0x130
    unsigned char pad134[0x04];
    float f138;                 // 0x138
    float f13c;                 // 0x13c
    unsigned char pad140[0xd4];
    int i214;                   // 0x214
    int i218;                   // 0x218
    int i21c;                   // 0x21c
    int i220;                   // 0x220
    unsigned char pad224[0x08]; // 0x224
    void* p22c;                 // 0x22c  followed object
    unsigned char pad230[0x38]; // 0x230
    int i268;                   // 0x268
    unsigned char pad26c[0x04];
    unsigned char mem270[0x38c];// 0x270
    void (*cb5fc)(void*);       // 0x5fc
    void (*cb600)(void*);       // 0x600
    void (*cb604)(void*);       // 0x604
    unsigned char mem608[0x2f0];// 0x608
    int i8f8;                   // 0x8f8
    unsigned char pad8fc[0x04];
    float f900;                 // 0x900
    float health;               // 0x904
};

struct SendTarget {
    int unk00;
    int unk04;
    int unk08;
    int unk0c;
    int unk10;
    int id;                     // 0x14
};

// Payload of the reply message sent by the damage handler.
struct DamageReply {
    int unk00;
    Actor* owner;               // 0x04
    int id;                     // 0x08
    float factor;               // 0x0c
    int unk10;
    int hash;                   // 0x14
};

extern unsigned char lbl_8034BC18[];
extern SendTarget lbl_803C44F0[];
extern unsigned char __upper_map[];

extern float lbl_804DABE8;
extern float lbl_804DABEC;
extern float lbl_804DABF0;
extern float lbl_804DABF4;
extern float lbl_804DABF8;
extern double lbl_804DAC00;
extern float lbl_804D394C;
extern float lbl_804D3950;
extern float lbl_804D3954;

int OwnerId(Owner* owner);
int ModelAnimate(int model, char* anim, int flags);
int HasCondition(void* mem, int condition);
int MemFlagIsSet(void* mem, int flag);
void ConditionSetBit(void* mem, int condition, int value);
void FollowSetTargetObj(void* owner, void* target);
int ActorPlayAnim(int id, unsigned long anim, int arg);
int Rand();

extern "C" int fn_80019F88(int* data, void* owner);
extern "C" int fn_800AFD20(void* owner);
extern "C" int fn_8003A1F0(int a, int b, int c);
extern "C" void fn_8003F15C(float f, int a, int b, int c, int* out, int e);
typedef int (*ActorCallback)(int a, int b, void* owner);
extern "C" void fn_800BDA0C(void* owner, ActorCallback callback, void* arg);
extern "C" void fn_80090390(void* a, int b);
extern "C" void fn_8028B35C(void* dst, void* src, int size);
extern "C" unsigned int fn_800BA84C(int n);
extern "C" void* memcpy(void* dst, const void* src, unsigned long n);
extern "C" int SendMsg(int to, int msg, void* data);
int ActorQueryHandler802B79DC(Actor* a, int* data);
int ActorFindSendSlot802B7EFC(int want0, int want1, void* owner);

struct Vec3 {
    float x;                    // 0x00
    float y;                    // 0x04
    float z;                    // 0x08
};

// An object whose world position is stored as ints at +0x24.
struct ObjectPosition {
    unsigned char pad00[0x24];
    int x;                      // 0x24
    int y;                      // 0x28
    int z;                      // 0x2c
};

extern "C" void fn_800C748C(void* owner);
extern "C" void fn_800BC56C(void* owner, void* target);
extern "C" int fn_800BAF40(void* owner);
int FollowLineCheck(void* a, void* b);
float ActorAngleToPos(void* owner, Vec3* pos);
float OwnerValue(Owner* owner, void* value);

extern float lbl_804D3920;
extern float lbl_804D3924;
extern float lbl_804D3928;
extern int lbl_804D3958;
extern float lbl_804DAC28;
extern float lbl_804DAC2C;
extern float lbl_804DAC30;
extern float lbl_804DAC34;

int ActorDamageHandler802B7710(Actor* a, int* data);

// Lazy type-name hash: uppercase-folded characters, 31-base accumulate.
static inline unsigned long TypeNameHash(const char* name)
{
    unsigned long hash = 0;

    if (name) {
        const char* p = name;
        while (*p) {
            int c;
            if (*p == -1)
                c = -1;
            else
                c = __upper_map[(unsigned char)*p];
            hash *= 31;
            hash += c & 0xFF;
            p++;
        }
        hash %= 0xFFFFFFFF;
    }
    return hash;
}

int ActorMessageHandler802B7120(int msg, int* data)
{
    Actor* a = (Actor*)data[0];
    ExtraState* extra;
    int reply[9];

    switch (msg) {
    case 0x0: {
        int id;
        int anim;
        int arg;
        a->f13c = lbl_804DABE8;
        id = fn_800AFD20((void*)a);
        anim = ModelAnimate(a->model, 0, 0);
        arg = fn_8003A1F0(id, a->f0, anim);
        fn_8003F15C(lbl_804DABEC, id, arg, a->d0, data + 4, 0);
        return 0;
    }
    case 0x400:
        data[1] = OwnerId((Owner*)a);
        *(float*)(data + 4) = lbl_804DABEC;
        *(float*)(data + 5) = lbl_804DABEC;
        data[6] = 2;
        data[7] = 0;
        if (!HasCondition(a->mem270, 9) && !HasCondition(a->mem270, 10))
            data[3] = 1;
        else
            data[3] = 0;
        return 1;
    case 0x407:
        return ActorDamageHandler802B7710(a, data);
    case 0x41d:
        extra = a->extra;
        if (fn_80019F88(data, (void*)a) != 0)
            return 1;
        if (data[1] != 0x79e72438)
            return 0;
        extra->slot = -1;
        fn_800BDA0C((void*)a, ActorFindSendSlot802B7EFC, (void*)a);
        if (extra->slot != -1) {
            if (SendMsg(lbl_803C44F0[extra->slot].id, 0x400, reply) != 0 &&
                reply[0] != 0)
                FollowSetTargetObj(a->mem270, (void*)reply[0]);
        }
        return 1;
    case 0x41e:
        return ActorQueryHandler802B79DC(a, data);
    case 0xc14: {
        unsigned int n;
        int i;
        extra = a->extra;
        if (data[3] != (int)0x90F54CD9)
            return 0;
        extra->remaining--;
        n = (unsigned int)extra->size >> 2;
        for (i = 0; i < n; i++) {
            if (data[2] == extra->list[i]) {
                int* list = extra->list;
                int* end = (int*)((char*)list + extra->size);
                int* dst = list;
                int* p;
                for (p = list; p != end; p++) {
                    if (*p != extra->list[i]) {
                        *dst = *p;
                        dst++;
                    }
                }
                fn_8028B35C(extra->unk24, dst, (end - dst) * 4);
                break;
            }
        }
        if (extra->selected == data[2])
            extra->selected = -1;
        if (extra->remaining == 0 && a->i8f8 != 4 && a->i8f8 != 6 &&
            !HasCondition(a->mem270, 9) && !HasCondition(a->mem270, 10)) {
            if (HasCondition(a->mem270, 12)) {
                ConditionSetBit(a->mem270, 12, 0);
                fn_80090390((void*)a->d4, 0);
            }
            ConditionSetBit(a->mem270, 9, 1);
        }
        return 0;
    }
    case 0x7102: {
        const char* name;
        unsigned long hash;
        extra = a->extra;
        if (extra == 0)
            return 1;
        if (data[1] < 0x48) {
            data[4] = 0;
            return 1;
        }
        name = (const char*)lbl_8034BC18 + 0x48;
        hash = TypeNameHash(name);
        memcpy((void*)data[2], &hash, 4);
        data[3] += 4;
        data[1] -= 4;
        data[2] += 4;
        memcpy((void*)data[2], extra, 0x44);
        data[3] += 0x44;
        data[1] -= 0x44;
        data[2] += 0x44;
        return 1;
    }
    case 0x7103: {
        const char* name;
        int size;
        int ok;
        extra = a->extra;
        if (extra == 0)
            return 1;
        size = data[3];
        if (size < 0x48) {
            data[4] = 0;
            return 1;
        }
        if (size < 4) {
            ok = 0;
        } else {
            unsigned int* p = (unsigned int*)data[2];
            data[3] += 4;
            data[1] -= 4;
            data[2] += 4;
            name = (const char*)lbl_8034BC18 + 0x48;
            ok = (*p == TypeNameHash(name));
        }
        if (!ok)
            return 1;
        memcpy(extra, (void*)data[2], 0x44);
        data[3] -= 0x44;
        data[2] += 0x44;
        return 1;
    }
    default:
        return 0;
    }
}

int ActorDamageHandler802B7710(Actor* a, int* data)
{
    ExtraState* extra = a->extra;

    if (extra->remaining > 0) {
        if (MemFlagIsSet(a->mem270, 10)) {
            data[0x16] = 1;
        } else if (data[2] != a->id) {
            int hash = data[5];
            unsigned int rnd;
            unsigned int limit;
            float max;

            if (hash == (int)0xCF0262AA)
                *(float*)(data + 3) *= lbl_804D3954;
            else if (hash < (int)0xCF0262AA) {
                if (hash == -0x378A6480)
                    *(float*)(data + 3) *= lbl_804D394C;
            } else if (hash == 0x6D57DA0A) {
                *(float*)(data + 3) *= lbl_804D3950;
            }
            if (data[5] == -0x4603BD0B)
                *(float*)(data + 3) = (*(float*)(data + 3) < lbl_804DABF0)
                                          ? *(float*)(data + 3)
                                          : lbl_804DABF0;
            rnd = fn_800BA84C(6);
            *(float*)(data + 3) =
                *(float*)(data + 3) * (lbl_804DABF4 * (float)rnd);
            a->i214 = data[0x11];
            a->health -= *(float*)(data + 3);
            max = (float)(unsigned int)a->vitals->max;
            {
                float h = a->health;
                float out = lbl_804DABE8;
                if (h >= lbl_804DABE8) {
                    out = (h <= max) ? h : max;
                }
                a->health = out;
            }
            ConditionSetBit(a->mem270, 4, 1);
            if (ActorPlayAnim(a->id, data[5], data[2]) == 0)
                ActorPlayAnim(a->id, 0xA464660E, data[2]);
            if ((unsigned int)data[2] >> 24 == 1)
                extra->selected = data[2];
            limit = (unsigned int)(extra->remaining *
                                   (a->vitals->max / extra->total));
            if (a->health < (float)limit) {
                DamageReply reply;
                reply.owner = a;
                reply.id = a->id;
                reply.factor = lbl_804DABF8;
                reply.hash = 0x6CA5F172;
                if (extra->selected == -1) {
                    unsigned int r = Rand();
                    unsigned int count = (unsigned int)extra->size >> 2;
                    SendMsg(extra->list[r % count], 0x407, &reply);
                } else {
                    SendMsg(extra->selected, 0x407, &reply);
                }
            }
        }
    }
    return 1;
}

// --- Layouts and helpers used by the 0x41e query handler -------------------

// One entry of the per-state callback table (stride 0x10).  Each state owns a
// transition test, an exit test and an update callback, plus its name.
struct NameEntry {
    int (*unk00)(Actor*);       // 0x00
    int (*unk04)(Actor*);       // 0x04
    int (*unk08)(Actor*);       // 0x08
    const char* name;           // 0x0c
};

// One follower slot (stride 0x20) inside the actor's +0x270 block.
struct FollowEntry {
    Actor* target;              // 0x00
    unsigned char pad04[0x08];
    int state;                  // 0x0c
    unsigned char pad10[0x10];
};

// Overlay of the actor's +0x270 block (0x688 bytes).  The block is handed to
// the condition helpers as raw bytes, so it is cast where it is used here.
struct FollowBlock {
    unsigned char pad00[0x60];
    int index;                  // 0x2d0
    unsigned char pad64[0x08];
    FollowEntry entries[48];    // 0x2dc
    unsigned char padTail[0x1c];
};

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
};

class String : public Buffer {
public:
    String(const char* text);
    virtual ~String() {}
    int data;                   // 0x04
    int size;                   // 0x08
};

extern NameEntry lbl_803C49D0[];

extern float lbl_804D392C;
extern float lbl_804D3930;
extern float lbl_804D3934;
extern float lbl_804D3938;
extern float lbl_804D393C;
extern float lbl_804D3940;
extern float lbl_804D3944;
extern float lbl_804D3948;
extern float lbl_804D5788;
extern float lbl_804D578C;
extern float lbl_804D5790;

extern float lbl_804DAC08;
extern float lbl_804DAC0C;
extern float lbl_804DAC10;
extern float lbl_804DAC14;
extern float lbl_804DAC18;
extern float lbl_804DAC1C;
extern int lbl_804D395C;

extern "C" void fn_80162658(void* owner, void* mem);
extern "C" int fn_800BEF9C(int handle);
extern "C" void fn_80037EC0(int model, int id, int mode, unsigned char a,
                            unsigned char b, unsigned char c, unsigned char d);
extern "C" double pow(double base, double exponent);
float fn_80017084(float value);
void VFree(void* ptr);

extern "C" void fn_8028C188(int* out, int a, int b);
extern "C" String fn_800225C8(String* a, String* b);
extern "C" int fn_800BAA90(void* owner, int* a, int* b);
void* GetPlayer();
void* FollowGetTargetObj(void* p);

// Damage-level reply for the current health ratio.
static inline void SetDamageReply(Actor* a, int* data, float high, float mid, float low)
{
    if (a->health < (float)(unsigned int)a->vitals->max * lbl_804D393C * lbl_804DABF4)
        data[3] = (int)high;
    else if (a->health < (float)(unsigned int)a->vitals->max * lbl_804D392C * lbl_804DABF4)
        data[3] = (int)mid;
    else
        data[3] = (int)low;
}

int ActorQueryHandler802B79DC(Actor* a, int* data)
{
    ExtraState* extra = a->extra;
    FollowBlock* mem = (FollowBlock*)a->mem270;
    int hash = data[1];

    switch (hash) {
    case (int)0xBA8B6271: {
        void* player = GetPlayer();
        int valueA;
        int valueB;
        fn_800BAA90(player, &valueA, &valueB);
        if (fn_800BAA90(player, &valueA, &valueB) != 0) {
            data[3] = valueB;
            return 1;
        }
        data[3] = -1;
        return 0;
    }
    case (int)0xBA8B6270: {
        int valueA;
        int valueB;
        if (fn_800BAA90(GetPlayer(), &valueA, &valueB) != 0) {
            data[3] = valueA;
            return 1;
        }
        data[3] = -1;
        return 0;
    }
    case (int)0xA0F20BDF:
        SetDamageReply(a, data, lbl_804D3944, lbl_804D3934, lbl_804D578C);
        return 1;
    case (int)0x8D0EBDD3:
        SetDamageReply(a, data, lbl_804D3940, lbl_804D3930, lbl_804D5788);
        return 1;
    case (int)0xBECAC197:
        SetDamageReply(a, data, lbl_804D3948, lbl_804D3938, lbl_804D5790);
        return 1;
    case (int)0x929AD7A4:
        if (extra->slot != -1) {
            data[3] = lbl_803C44F0[extra->slot].unk08;
            return 1;
        }
        data[3] = 0;
        return 0;
    case (int)0xF086E6C4:
        if (extra->slot != -1) {
            data[3] = lbl_803C44F0[extra->slot].unk0c;
            return 1;
        }
        data[3] = 0;
        return 0;
    case (int)0xAD8576FA: {
        String name(lbl_803C49D0[extra->unk00].name);
        String suffix((const char*)lbl_8034BC18 + 0x52);
        String out = fn_800225C8(&name, &suffix);
        fn_8028C188(data + 3, out.data, out.size);
        return 0;
    }
    case (int)0x29FADCC0:
        if (mem->index != -1) {
            FollowEntry* e = &mem->entries[mem->index];
            if (e->state == 2) {
                data[3] = e->target->id;
                return 1;
            }
        }
        data[3] = -1;
        return 0;
    case (int)0x63F94A69: {
        Actor* target = (Actor*)FollowGetTargetObj(a->mem270);
        if (target != 0)
            data[3] = target->id;
        else
            data[3] = -1;
        return 1;
    }
    default:
        return 0;
    }
}

int ActorFindSendSlot802B7EFC(int want0, int want1, void* owner)
{
    Actor* actor = (Actor*)owner;
    ExtraState* extra = actor->extra;
    int i;

    for (i = 0; i < 0xd; i++) {
        int base = i * 4;
        if (want0 == lbl_803C44F0[base].unk10 && want1 == lbl_803C44F0[base].id) {
            extra->slot = base;
            return 1;
        }
        if (want0 == lbl_803C44F0[base + 1].unk10 && want1 == lbl_803C44F0[base + 1].id) {
            extra->slot = base + 1;
            return 1;
        }
        if (want0 == lbl_803C44F0[base + 2].unk10 && want1 == lbl_803C44F0[base + 2].id) {
            extra->slot = base + 2;
            return 1;
        }
        if (want0 == lbl_803C44F0[base + 3].unk10 && want1 == lbl_803C44F0[base + 3].id) {
            extra->slot = base + 3;
            return 1;
        }
    }
    return 0;
}

// Per-frame tick of the extra state block: runs the per-state callbacks,
// counts the fade timer down and fades the model colour with it.
int ActorStateTick802B7FD0(Actor* a)
{
    ExtraState* extra;
    int next;
    float gain;
    float ratio;
    float blend;
    float lo;
    float hi;
    float glow;
    int value;

    if (a == 0)
        return 0;

    extra = a->extra;
    extra->f0c += a->f138;
    if (extra->f10 > lbl_804DABE8)
        extra->f10 -= a->f138;

    if (HasCondition(a->mem270, 9) || HasCondition(a->mem270, 10) ||
        HasCondition(a->mem270, 11)) {
        fn_80162658(a, a->mem608 + 0x1c);
        return 1;
    }

    if (a->i268 > fn_800BEF9C(*(int*)(a->mem270 + 0x54)))
        a->i268 = fn_800BEF9C(*(int*)(a->mem270 + 0x54));

    next = lbl_803C49D0[extra->unk00].unk08(a);
    fn_80162658(a, a->mem608 + 0x1c);

    if (next != extra->unk00) {
        if (lbl_803C49D0[extra->unk00].unk04(a) != 0) {
            if (lbl_803C49D0[next].unk00(a) == 0)
                lbl_803C49D0[extra->unk00].unk00(a);
        }
    }

    if (extra->remaining < extra->total) {
        gain = fn_80017084(extra->f14);
        ratio = (float)extra->remaining / (float)extra->total;
        if (ratio < lbl_804DAC08)
            blend = lbl_804DAC08 + ratio;
        else
            blend = lbl_804DAC0C * (ratio - lbl_804DAC08) + lbl_804DABEC;
        lo = lbl_804DAC10 * (lbl_804DABEC - ratio) + ratio;
        hi = ratio + (lbl_804DABEC - ratio);
        glow = lbl_804DABEC - (float)pow(lbl_804DAC08 * (lbl_804DABEC + gain), blend);
        if (glow < lbl_804DABE8)
            glow = lbl_804DABE8;
        else if (glow > lbl_804DABEC)
            glow = lbl_804DABEC;
        value = (int)(lbl_804DAC14 * (glow * (hi - lo) + lo));
        if (value < 0)
            value = 0;
        else if (value > 0xff)
            value = 0xff;
        fn_80037EC0(a->model, lbl_804D395C, 1, 0xff, value, value, 0xff);
        extra->f14 = lbl_804DAC18 * (lbl_804DAC1C * a->f138) + extra->f14;
    }
    return 1;
}

// Releases the actor's state block.
void ActorFreeExtra802B82C8(Actor* a)
{
    a->cb104 = 0;
    VFree(a->extra);
    a->extra = 0;
}

// --- Collection state of the pickup actor ----------------------------------

// The collected-id list embedded at +0x24 of the state block.  The retail
// constructor writes two different vtables into the same slot, which is what
// an inlined two-level constructor chain produces.
class PickupListBase : public Buffer {
public:
    virtual ~PickupListBase();
    int unk04;                  // 0x04
    int unk08;                  // 0x08
    int unk0c;                  // 0x0c
    int unk10;                  // 0x10
    int unk14;                  // 0x14
    int unk18;                  // 0x18
};

class PickupIdList : public PickupListBase {
public:
    PickupIdList() {}
    virtual ~PickupIdList();
};

// The 0x44-byte state block allocated by ActorInitCollection802B8308.
struct PickupState {
    PickupState() {}
    int state;                  // 0x00
    float f04;                  // 0x04
    float f08;                  // 0x08
    float f0c;                  // 0x0c
    float f10;                  // 0x10
    float f14;                  // 0x14
    int count;                  // 0x18
    int added;                  // 0x1c
    int selected;               // 0x20
    PickupIdList list;          // 0x24
    int slot;                   // 0x40
};

inline void* operator new(unsigned long, void* where) { return where; }

typedef int (*CollectCheckFn)(void*, void*);

void* VAlloc(int size);
unsigned long AssetFind(unsigned long id, unsigned long group);
void ConditionUpdateBit(void* mem, int bit, int value);
extern "C" void fn_8001ED98(void* timer, int period, CollectCheckFn callback,
                            void* owner, int flags);
extern "C" void fn_800BAEE4(void* owner, int enable);
extern "C" void fn_800CE96C(void* owner);
extern "C" void fn_800CF064(void* owner);
extern "C" void fn_800CC984(void* owner);
extern "C" int* fn_8028B3BC(void* list, int offset, int size);
extern "C" int fn_80212730(int id);
extern "C" void fn_8021284C(int id);
extern "C" void fn_80212814(int id, int other, unsigned long asset,
                            int a, int b, int c);

extern float lbl_804DAC20;
extern float lbl_804DAC24;

int ActorShouldCollect802B84C8(void* playerArg, void* ownerArg);

// Allocates and initialises the pickup actor's collection state block.
void ActorInitCollection802B8308(Actor* a)
{
    PickupState* state;
    int i;

    a->cb104 = ActorStateTick802B7FD0;
    state = (PickupState*)VAlloc(0x44);
    if (state != 0)
        new (state) PickupState();
    if (state == 0) {
        a->extra = 0;
        return;
    }

    a->extra = (ExtraState*)state;
    a->cbFC = fn_800CE96C;
    a->cb604 = fn_800CF064;
    a->cb5fc = fn_800CC984;
    a->cb600 = fn_800CC984;

    state->state = 1;
    state->list.unk14 = 0;
    state->f04 = lbl_804DABE8;
    state->f08 = lbl_804DABE8;
    state->f0c = lbl_804DAC20;
    state->count = 0;
    state->added = 0;
    state->list.unk18 = 0;

    lbl_804D395C = (int)AssetFind(0xD9044CBB, 0x619DEF54);
    fn_8001ED98(a->timer, (int)(lbl_804DAC24 * a->f900),
                ActorShouldCollect802B84C8, a, 0);
    fn_800BAEE4(a, 1);
    ConditionSetBit(a->mem270, 0x2d, 1);
    ConditionUpdateBit(a->mem270, 0x1c, 1);
    lbl_803C49D0[state->state].unk00(a);

    for (i = 0; i < 0x34; i++) {
        lbl_803C44F0[i].unk10 = (int)AssetFind(0x57F77CF9, lbl_803C44F0[i].unk00);
        lbl_803C44F0[i].id = (int)AssetFind(0x57F77CF9, lbl_803C44F0[i].unk04);
    }
}

// Timer check of the pickup actor: once the interacting actor is the player
// and carries the pickup interaction source, publish the pickup state and
// record the player in the collected list.
int ActorShouldCollect802B84C8(void* playerArg, void* ownerArg)
{
    Actor* player = (Actor*)playerArg;
    Actor* owner = (Actor*)ownerArg;

    if ((int)((unsigned int)player->id >> 24) == 1 && player->i88 != 0 &&
        *player->i88 == 0x0A303C34) {
        PickupState* state = (PickupState*)owner->extra;
        unsigned long asset = AssetFind(0xF1754BF2, 0x619DEF54);
        int* entry;

        if (fn_80212730(player->id) != -1)
            fn_8021284C(player->id);
        fn_80212814(player->id, owner->id, asset, 1, 0, 0);
        player->iA8 = owner->id;
        state->count++;
        state->added++;
        entry = fn_8028B3BC(&state->list, state->list.unk0c + state->list.unk08, 4);
        *entry = player->id;
    }
    return 1;
}

// Returns the selected id stored at the head of the state block, unless the
// actor's +0x220 flag is set.
int ActorGetSelectedId802B85D0(Actor* a)
{
    ExtraState* e = a->extra;
    if (a->i220 != 0)
        return 0;
    return e->unk00;
}

// Constant predicate (always true).
int ActorCollectPredicate802B85F0(void)
{
    return 1;
}

// Starts the pickup collection state: marks the state block as active.
int ActorCollectStart802B85F8(Actor* a)
{
    a->extra->unk00 = 1;
    return 1;
}

// Per-frame follow behaviour: tracks the follow target, plays the pickup
// animation when the target faces the actor, and clears the acquired flag
// once the target has moved away again.
int ActorFollowUpdate802B860C(Actor* a)
{
    ExtraState* extra = a->extra;
    int result;

    if (a->i220 == 0 && MemFlagIsSet(a->mem270, 5) == 0)
        return 1;

    if (a->i220 == 2) {
        a->cb10c = fn_800C748C;
        if (FollowGetTargetObj(a->mem270) != a->p22c ||
            *(int*)(a->mem270 + 0x18) != a->i21c) {
            FollowSetTargetObj(a->mem270, a->p22c);
            if (a->i21c != 0) {
                *(int*)(a->mem270 + 0x18) = a->i21c;
                fn_800BC56C(a, a->p22c);
            }
        }
        if (fn_800BAF40(a) == -1 && a->i21c != 0) {
            FollowSetTargetObj(a->mem270, 0);
            *(int*)(a->mem270 + 0x18) = 0;
        }
        if (extra->f3c == 0) {
            Actor* player = (Actor*)GetPlayer();
            if (player != 0) {
                ObjectPosition* target = (ObjectPosition*)a->p22c;
                ObjectPosition* who = (ObjectPosition*)player;
                Vec3 targetPos;
                Vec3 playerPos;
                float diff;

                targetPos.x = (float)target->x;
                targetPos.y = (float)target->y;
                targetPos.z = (float)target->z;
                playerPos.x = (float)who->x;
                playerPos.y = (float)who->y;
                playerPos.z = (float)who->z;
                diff = ActorAngleToPos(a, &targetPos) -
                       ActorAngleToPos(a, &playerPos);
                while (diff < lbl_804DAC2C)
                    diff += lbl_804DAC28;
                while (lbl_804DAC30 < diff)
                    diff -= lbl_804DAC28;
                if ((diff < 0.0f ? -diff : diff) < lbl_804D3924 &&
                    (lbl_804D3958 == 0 || FollowLineCheck(a, player) != 0) &&
                    OwnerValue((Owner*)a, player) < lbl_804D3920) {
                    ActorPlayAnim(a->id, 0x14F0389D, player->id);
                    extra->f3c = 1;
                    extra->f10 = lbl_804D3928;
                }
            }
        } else {
            Actor* player = (Actor*)GetPlayer();
            if (player != 0 &&
                OwnerValue((Owner*)a, player) > lbl_804DAC34 * lbl_804D3920)
                extra->f3c = 0;
            if (extra->f10 <= lbl_804DABE8)
                extra->f3c = 0;
        }
    }
    result = extra->unk00;
    return result;
}

// Exit test of the pickup collection state: clears the active condition bit.
int ActorCollectExit802B88F0(Actor* a)
{
    ConditionSetBit(a->mem270, 0x24, 0);
    return 1;
}

// Enter test of the pickup collection state: drops the followed target and
// marks the state as active.
int ActorCollectEnter802B8920(Actor* a)
{
    a->extra->unk00 = 0;
    Actor* target = (Actor*)FollowGetTargetObj(a->mem270);
    fn_800BC56C(a, target);
    ConditionSetBit(a->mem270, 0x24, 1);
    return 1;
}

// Type-id hash of the state block handled by this unit's message hooks.
unsigned long OwnerStateTypeHash802B8980(void)
{
    return 0x3da260f7;
}

unsigned long Fn80175D60(unsigned long value);

void ImpThrowFireball802B8A24(int id, int phase, int* payload, void* owner, float scale);

// Message hook: resolves the value through Fn80175D60 and, for message
// 0x14967, forwards it to the state update routine. Returns true when the
// message was consumed.
bool OwnerStateMessage802B898C(int* owner, int msg, unsigned long value, int arg, float scale)
{
    unsigned long resolved = Fn80175D60(value);
    if (msg == 0x14967) {
        ImpThrowFireball802B8A24((int)value, (int)resolved, (int*)arg,
                                 (void*)*owner, scale);
        return true;
    }
    return false;
}

// --- Imp fire-ammo throw state (message 0x14967) ---------------------------

// Object the fireball is aimed at.  Only its integer world position is read.
struct ImpFireTarget {
    unsigned char pad00[0x24];
    int x;                      // 0x24
    int y;                      // 0x28
    int z;                      // 0x2c
};

// Aiming data of the firing actor.
struct ImpFireAim {
    unsigned char pad00[0x30];
    Vec3 pos;                   // 0x30
    Vec3 aim;                   // 0x3c
    ImpFireTarget* target;      // 0x48
};

// Payload of the throw-state message.
struct ImpFirePayload {
    ImpFireAim* aim;            // 0x00
    float angle;                // 0x04
    float droop;                // 0x08
    int unk0c;                  // 0x0c
    int unk10;                  // 0x10
};

// Projectile description handed to ImpFireBallSpawn (0x4c bytes on the stack).
struct ImpThrow {
    Vec3 pos;                   // 0x00
    Vec3 vec;                   // 0x0c
    Vec3 dir;                   // 0x18
    float f24;                  // 0x24
    float f28;                  // 0x28
    float f2c;                  // 0x2c
    float f30;                  // 0x30
    float f34;                  // 0x34
    float f38;                  // 0x38
    unsigned char pad3c[0x10];  // 0x3c
};

// Reply of the 0x400 weapon query.
struct GunModel {
    unsigned char pad00[0x248];
    int gun;                    // 0x248
    unsigned char pad24c[0x1a20];
    int f1c6c;                  // 0x1c6c
    int f1c70;                  // 0x1c70
    int f1c74;                  // 0x1c74
};

extern const char* lbl_803C49F0;
extern unsigned long lbl_803C49F4;

extern float lbl_804DAC38;
extern float lbl_804DAC3C;
extern float lbl_804DAC40;
extern float lbl_804DAC44;
extern float lbl_804DAC48;
extern float lbl_804DAC4C;
extern float lbl_804DAC50;
extern float lbl_804DAC54;
extern float lbl_804DAC58;
extern float lbl_804DAC5C;
extern float lbl_804DAC60;
extern float lbl_804DAC64;

extern float lbl_804D3960;
extern float lbl_804D3964;
extern float lbl_804D3968;
extern float lbl_804D396C;

extern "C" void fn_800E1098(float angle, Vec3* out, Vec3* in);
extern "C" void fn_8016D440(Vec3* out, Vec3* in);
extern "C" void fn_8016D4EC(Vec3* out, Vec3* in);
extern "C" float fn_800E1020(Vec3* vec);
extern "C" float fn_800F7920(void* owner, unsigned long hash);
extern "C" void fn_800C5E00(int handle, int a, int b, int c, int zero);
extern "C" void fn_800B4C50(float f1, float f2, Vec3* pos, int id, int one, int zero);
extern "C" int rand(void);
extern "C" float sqrtf(float value);
extern "C" float fabsf(float value);
extern "C" void* memset(void* dst, int value, unsigned long size);

Vec3* PlayerGetGunVec(void* owner);
float ActorAimAngleToPos(void* owner, Vec3* pos);
void ImpFireBallSpawn(int id, ImpThrow* throwState, int mode);

// Second half of the 0x14967 throw message: phase 0 launches a shot from the
// firing actor's aim, phase 1 adjusts it towards the aimed-at object.  The
// payload is passed through the message table as an untyped word, hence the
// pointer parameters arrive as plain ints.
void ImpThrowFireball802B8A24(int id, int phase, int* payload, void* owner, float scale)
{
    ImpFirePayload* data = (ImpFirePayload*)payload;
    ImpFireAim* aim = data->aim;
    ImpThrow shot;
    float power;
    float rnd1;
    float rnd2;
    float rnd3;

    if (lbl_803C49F4 == 0) {
        if (lbl_803C49F0 != 0)
            lbl_803C49F4 = TypeNameHash(lbl_803C49F0);
        else
            lbl_803C49F4 = 0;
    }

    if (phase == 0) {
        Vec3 aimDir;
        GunModel* gun;
        Vec3* gunVec;
        float spread;
        int reply[8];

        memset(&shot, 0, sizeof(shot));
        shot.f38 = -data->droop;
        shot.f30 = lbl_804DAC38;
        shot.f34 = lbl_804DAC38;
        fn_800E1098(data->angle, &shot.dir, &aim->aim);
        shot.pos = aim->pos;
        fn_8016D440(&aimDir, &aim->aim);
        shot.vec = aimDir;

        power = fn_800F7920(owner, 0x53BF9B3F);
        if (power != lbl_804DAC38) {
            shot.f24 = lbl_804DAC40 * (power * ((float)rand() / lbl_804DAC3C)) - power;
            shot.f28 = lbl_804DAC40 * (power * ((float)rand() / lbl_804DAC3C)) - power;
            shot.f2c = lbl_804DAC40 * (power * ((float)rand() / lbl_804DAC3C)) - power;
        }

        SendMsg(id, 0x400, reply);
        gun = (GunModel*)reply[0];
        gunVec = PlayerGetGunVec(gun);
        spread = lbl_804DAC48 * scale;
        shot.dir.x += gunVec->x * lbl_804DAC44;
        shot.dir.y += gunVec->y * lbl_804DAC44;
        shot.dir.z += gunVec->z * lbl_804DAC44;
        shot.pos.x += gunVec->x * lbl_804DAC44 * spread;
        shot.pos.y += gunVec->y * lbl_804DAC44 * spread;
        shot.pos.z += gunVec->z * lbl_804DAC44 * spread;

        if (gun->f1c6c == 0)
            fn_800C5E00(gun->gun, 1, data->unk0c, data->unk10, 0);
        else
            fn_800C5E00(gun->gun, 1, gun->f1c70, gun->f1c74, 0);

        if (reply[6] & 2)
            fn_800B4C50(lbl_804DAC44, lbl_804DAC4C, &aim->pos, id, 1, 0);
    }
    else if (phase == 1) {
        ImpFireTarget* target;
        Vec3 delta;
        Vec3 angles;
        float drop;
        float root;
        float length;
        float speed;
        float ratio;
        int reply[8];

        if (aim->target == 0)
            return;
        SendMsg(id, 0x400, reply);
        target = aim->target;

        memset(&shot, 0, sizeof(shot));
        shot.pos = aim->pos;
        shot.f38 = -data->droop;
        shot.f30 = lbl_804DAC38;
        shot.f34 = lbl_804DAC38;

        rnd1 = (float)rand() / lbl_804DAC3C;
        rnd2 = (float)rand() / lbl_804DAC3C;
        rnd3 = (float)rand() / lbl_804DAC3C;
        shot.dir.x = lbl_804DAC38;
        shot.dir.y = lbl_804DAC38;
        shot.f24 = lbl_804DAC50 * rnd3 - lbl_804DAC54;
        shot.dir.z = lbl_804DAC58;
        shot.f28 = lbl_804DAC50 * rnd2 - lbl_804DAC54;
        shot.f2c = lbl_804DAC50 * rnd1 - lbl_804DAC54;

        delta.x = (float)target->x - shot.pos.x;
        delta.y = (float)target->y - shot.pos.y;
        delta.z = (float)target->z - shot.pos.z;

        drop = -(lbl_804DAC58 / shot.f38);
        ratio = (delta.z - lbl_804DAC5C * lbl_804DAC58 * drop) / (lbl_804DAC5C * shot.f38);
        root = lbl_804DAC38;
        if (ratio > lbl_804DAC38)
            root = sqrtf(ratio);

        length = fn_800E1020(&delta);
        delta.z = lbl_804DAC38;
        speed = (length / (drop + root)) * lbl_804DAC60;
        fn_800E1098(speed, &delta, &delta);

        if (fabsf(ActorAimAngleToPos(target, (Vec3*)((char*)reply[0] + 0x84))) >
            lbl_804DAC64) {
            int sign;

            fn_8016D440(&angles, &delta);
            sign = ((float)rand() / lbl_804DAC3C > lbl_804DAC5C) ? 1 : -1;
            angles.z = ((lbl_804D3964 - lbl_804D3960) *
                            ((float)rand() / lbl_804DAC3C) +
                        lbl_804D3960) *
                           (float)sign +
                       angles.z;
            fn_8016D4EC(&delta, &angles);
            fn_800E1098(speed * lbl_804D3968, &delta, &delta);
            delta.z += lbl_804D396C;
        }

        shot.dir.x += delta.x;
        shot.dir.y += delta.y;
        shot.dir.z += delta.z;
    }

    ImpFireBallSpawn(id, &shot, 1);
}

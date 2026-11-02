// u_8029E714.cpp: game-wide world / actor-table services (0x8029E714-0x802A770C).
// A large unit of loosely related code; only the small accessors (frame counters, level-state
// flags, the actor-table lookup and the time-scale values) are drafted so far.  Every larger
// function is declared with a TODO comment.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned short u16;
struct Actor;

// Growable buffer base used by the list classes below (code at 0x8028B1E4-0x8028C5xx).
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

void* VAlloc(int size);                                         // 0x8009DA68
void VFree(void* p);                                            // 0x8009DA3C
u32 AssetFind(u32 table, u32 key);                              // 0x8007EB34
void TextureLoad(short* ids, int n);                            // 0x8000F738
int HasCondition(void* conditions, int which);                  // 0x800A7AC4
int ConditionSetBit(void* cond, int bit, int set);              // 0x800A7A54
int ActorPlayAnim(int actor, u32 anim, int arg);                // 0x800C5654
extern "C" void fn_8028B960(void* p);
extern "C" void fn_8028BC2C(void* self);
extern "C" void fn_8028BC28(void* self, void* begin, u32 size);
extern "C" int fn_800498D0(void);
extern "C" void fn_800BBCA8(void* o);
extern "C" void fn_8029B070(void* o, int v);
extern "C" void fn_800C9ED0(void* o);
extern "C" void fn_8016EDD4(int v);
extern "C" char* strcpy(char* dst, const char* src);
extern "C" char* strncpy(char* dst, const char* src, u32 n);

// Condition-carrying object (the condition block sits at +0x270).
struct CondObj {
    char pad00[0x270];
    char conditions[0xC];   // 0x270
};

// Host-like game object: the offsets the state helpers below touch.
struct HostObj {
    char pad0[0x24];
    int pos24; // 0x24
    int pos28; // 0x28
    int pos2C; // 0x2C
    char pad30[0x20];
    u32 flags50; // 0x50
    char pad54[0x24];
    int i78; // 0x78
    char pad7C[0x48];
    int iC4; // 0xC4
    char padC8[0x3C];
    int i104; // 0x104
    char pad108[0x4];
    void (*cb10C)(void*); // 0x10C
    char pad110[0x20];
    int* ptr130; // 0x130
    char pad134[0xE8];
    int i21C; // 0x21C
    int i220; // 0x220
    char pad224[0x4C];
    char conditions[0x10]; // 0x270
    char pad280[0x8];
    int i288; // 0x288
    char pad28C[0x3AC];
    int i638; // 0x638
    char pad63C[0x168];
    char sub7A4[0x10]; // 0x7A4
    u32 flags7B4; // 0x7B4
    char pad7B8[0x18];
    int i7D0; // 0x7D0
    char pad7D4[0xC0];
    char anim894[0x1E4C]; // 0x894
    char sub26E0[8]; // 0x26E0
    char pad26E4[0x4];
    int i26E8; // 0x26E8
    char pad26EC[0xB0];
    int i279C; // 0x279C
};

extern "C" {
void fn_80162610(void* obj, void* anim, u32 id);
int fn_80175EFC(void);
void fn_80155A84(void* a, int b, int c);
void fn_800769A0(void);
void fn_8003F15C(float scale, int model, void* mtx, int a, int b, int c);
}
void PlayerSelectCollisionUpdate(void* o, float dt);   // 0x80072058
void PlayerSetValue4(void* sub, int v);                 // 0x8006D17C
void PlayerResetGunTimer(void* p);                      // 0x8006D76C
void DashUpdate(Actor* a);                              // 0x80243DC8
void ActorGetMatrix(void* m, Actor* a);                 // 0x8009ED70
int ConditionUpdateBit(void* cond, int bit, int set);   // 0x800A7B08
extern float lbl_804DA8C4;
extern float lbl_804DA8E8;
extern float lbl_804DA7C0;
void* FollowGetTargetObj(void* f);                      // 0x800A7A18
void FollowSetTargetObj(void* f, void* t);              // 0x800A78A0
void FollowOffsetUpdate(void* f);                       // 0x800A70D0
int PlayerGetHeldSlotIndex(void* p);                    // 0x8006CAB8
void PlayerInHost(Actor* a);                            // 0x80049750
extern "C" int fn_80049940(void);
extern "C" int fn_8016325C(void* anim);
extern "C" void fn_8029B3B0(void* o, int v);
extern "C" void fn_800B26B8(void* o);
extern "C" void fn_800BC56C(void* o, void* t);

struct HostObjFx {
    char pad00[0xC8];
    int model;          // 0xC8
};

// Two stacked list classes, each with its own vtable (0x803C38F0 / 0x803C3910).
class EffectListBase : public Buffer {
public:
    EffectListBase() {}
    virtual ~EffectListBase() { fn_8028B960(this); }
    static void* operator new(unsigned long n) { return VAlloc(n); }
    static void operator delete(void* p) { VFree(p); }
};

class EffectList : public EffectListBase {
public:
    EffectList() {}
    virtual ~EffectList() {}
};
extern EffectList* lbl_804D5750;

extern int lbl_804D5728;
extern float lbl_804D5720;
extern int lbl_804D5724;
extern int lbl_804D572C;
extern int lbl_804D5730;
extern int lbl_804D5734;
extern int lbl_804A46F8;
extern int lbl_804A46F4[];
extern int lbl_804D5738;
extern int lbl_804D573C;
extern float lbl_804D3860;
extern float lbl_804D3864;
extern int lbl_804D3868;
extern char lbl_804A4598[];

// Functions called from other units under their placeholder names (C linkage).
extern "C" int fn_802A1D04(void);
extern "C" void fn_802A1D0C(int v);
extern "C" int fn_802A1D9C(void);
extern "C" float fn_802A1DA4(void);
extern "C" int fn_802A1DAC(void);
extern "C" int fn_802A1DB4(void);
extern "C" void fn_802A1D80(void);
extern "C" char* fn_802A1FA0(void);
extern "C" void fn_802A2048(int base, int valid);
extern "C" void fn_802A2054(float v);
extern "C" void fn_802A2078(float v);
extern "C" float fn_802A2080(void);
extern "C" void fn_802A209C(int v);
extern "C" void fn_802A3BEC();


int fn_802A1D04() {
    return lbl_804D5728;
}

void fn_802A1D0C(int v) {
    lbl_804D5728 = v;
}


void FrameCounterAdd(float dt) {
    lbl_804D5724++;
    lbl_804D5720 = (float)((double)lbl_804D5720 + (double)dt);
}

void FrameCounterReset() {
    lbl_804D5724 = 0;
    lbl_804D5720 = 0.0f;
}

void LevelFlagRestore() {
    lbl_804D572C = lbl_804D5730;
}

void LevelFlagsInit() {
    lbl_804D5730 = 1;
    lbl_804A46F8 = 1;
    lbl_804D572C = 1;
    lbl_804D5734 = 1;
}

int FrameCountGet() {
    return lbl_804D5724;
}

float fn_802A1DA4() {
    return lbl_804D5720;
}

int fn_802A1DAC() {
    return lbl_804D5730;
}

int LevelFlagCurrent() {
    return lbl_804D572C;
}

void LevelFlagPop() {
    lbl_804D5730 = lbl_804A46F4[lbl_804D5734];
    if (lbl_804D5734 < 2) {
        return;
    }
    lbl_804D5734--;
}


char* fn_802A1FA0() {
    return lbl_804A4598;
}


struct Actor;
Actor* ActorGet(int index) {
    if (lbl_804D573C == 0) {
        return 0;
    }
    return (Actor*)(lbl_804D5738 + index * 0x2800);
}

void ActorTableSet(int base, int valid) {
    lbl_804D5738 = base;
    lbl_804D573C = valid;
}

void fn_802A2054(float v) {
    lbl_804D3864 = v;
}

void fn_802A2078(float v) {
    lbl_804D3860 = v;
}

float fn_802A2080() {
    if (lbl_804D3868 != 0) {
        return lbl_804D3860;
    }
    return 1.0f;
}

void fn_802A209C(int on) {
    lbl_804D3868 = on;
}


// Empty in retail.
void fn_802A3BEC() {}

// TODO (not drafted yet, 0x6B8 bytes at 0x8029E714): not analysed
// TODO (not drafted yet, 0x260 bytes at 0x8029EDCC): not analysed
void HostLeaveRide(HostObj* o) {
    o->i104 = 0;
    ConditionUpdateBit(o->conditions, 4, 0);
    ConditionUpdateBit(o->conditions, 3, 0);
    ConditionUpdateBit(o->conditions, 0x17, 0);
    ConditionSetBit(o->conditions, 0x2A, 0);
    ConditionSetBit(o->conditions, 0x2B, 0);
    VFree(o->ptr130);
    o->ptr130 = 0;
}

// TODO (not drafted yet, 0x164 bytes at 0x8029F0BC): not analysed
// TODO (not drafted yet, 0x128 bytes at 0x8029F220): not analysed
bool HostResetFollowB(HostObj* o) {
    float zero = lbl_804DA7C0;
    if (o == 0) {
        return false;
    }
    o->cb10C = FollowOffsetUpdate;
    *(float*)(o->ptr130 + 5) = zero;
    o->i288 = 0;
    o->i21C = 0;
    o->i220 = 0;
    fn_800BBCA8(o);
    o->cb10C = FollowOffsetUpdate;
    ConditionUpdateBit(o->conditions, 0x16, 0);
    fn_8029B070(o, 0x73B1E248);
    fn_8029B070(o, 0x13E6B);
    return true;
}

// TODO (not drafted yet, 0xCC bytes at 0x8029F3EC): not analysed
// TODO (not drafted yet, 0x39C bytes at 0x8029F4B8): not analysed
// TODO (not drafted yet, 0xA4 bytes at 0x8029F854): not analysed
// TODO (not drafted yet, 0xE0 bytes at 0x8029F8F8): not analysed
bool HostRideDone(HostObj* o) {
    if (o == 0) {
        return false;
    }
    void* t = FollowGetTargetObj(o->conditions);
    fn_800B26B8(o);
    if (t == 0) {
        ConditionUpdateBit(o->conditions, 4, 0);
        return true;
    }
    return fn_8016325C(o->anim894) != 6;
}

bool CondObjClearFlag2A(CondObj* o) {
    if (o == 0) {
        return false;
    }
    ConditionSetBit(o->conditions, 0x2A, 0);
    fn_800BBCA8(o);
    return true;
}

// TODO (not drafted yet, 0xC0 bytes at 0x8029FAB0): not analysed
// TODO (not drafted yet, 0x16C bytes at 0x8029FB70): not analysed
bool HostResetFollow(HostObj* o) {
    float zero = lbl_804DA7C0;
    if (o == 0) {
        return false;
    }
    o->cb10C = FollowOffsetUpdate;
    *(float*)(o->ptr130 + 5) = zero;
    o->i288 = 0;
    o->i21C = 0;
    o->i220 = 0;
    fn_800BBCA8(o);
    o->cb10C = FollowOffsetUpdate;
    fn_8029B070(o, 0x73B1E248);
    fn_8029B070(o, 0x13E6B);
    return true;
}

// TODO (not drafted yet, 0xE4 bytes at 0x8029FD70): not analysed
// TODO (not drafted yet, 0x39C bytes at 0x8029FE54): not analysed
// TODO (not drafted yet, 0xA4 bytes at 0x802A01F0): not analysed
// TODO (not drafted yet, 0xE4 bytes at 0x802A0294): not analysed
// TODO (not drafted yet, 0x1C0 bytes at 0x802A0378): not analysed
bool CondObjActivate(CondObj* o) {
    if (o == 0) {
        return false;
    }
    fn_8029B070(o, 0x13E6B);
    fn_800BBCA8(o);
    return true;
}

int HostEnterRide(HostObj* o) {
    if (o == 0) {
        return 0;
    }
    *o->ptr130 = 2;
    ConditionUpdateBit(o->conditions, 3, 1);
    ConditionUpdateBit(o->conditions, 0x17, 0);
    fn_8029B3B0(o, 0x13E6B);
    void* t = FollowGetTargetObj(o->conditions);
    if (t != 0) {
        FollowSetTargetObj(o->conditions, t);
        fn_800BC56C(o, t);
    }
    if (o->i220 == 4) {
        o->i220 = 0;
    }
    return 1;
}

bool CondObjNotBusy(CondObj* o) {
    if (o == 0) {
        return false;
    }
    fn_800C9ED0(o);
    return HasCondition(o->conditions, 0x2C) == 0;
}

bool HostResetFloat(HostObj* o) {
    if (o == 0) {
        return false;
    }
    *(float*)(o->ptr130 + 4) = lbl_804DA7C0;
    ConditionUpdateBit(o->conditions, 4, 1);
    fn_80162610(o, o->anim894, 0);
    fn_8029B070(o, 0x73B1E248);
    return true;
}

// TODO (not drafted yet, 0x170 bytes at 0x802A0700): not analysed
// TODO (not drafted yet, 0x300 bytes at 0x802A0870): not analysed
bool HostResetFloatB(HostObj* o) {
    if (o == 0) {
        return false;
    }
    *(float*)(o->ptr130 + 4) = lbl_804DA7C0;
    ConditionUpdateBit(o->conditions, 4, 0);
    ConditionSetBit(o->conditions, 0x2A, 0);
    fn_80162610(o, o->anim894, 0);
    fn_8029B070(o, 0x73B1E248);
    return true;
}

// TODO (not drafted yet, 0x138 bytes at 0x802A0BF4): not analysed
// TODO (not drafted yet, 0x528 bytes at 0x802A0D2C): not analysed
bool HostReset(HostObj* o) {
    if (o == 0) {
        return false;
    }
    ConditionUpdateBit(o->conditions, 4, 0);
    fn_80162610(o, o->anim894, 0);
    fn_8029B070(o, 0x73B1E248);
    return true;
}

// TODO (not drafted yet, 0x15C bytes at 0x802A12BC): not analysed
// TODO (not drafted yet, 0xF8 bytes at 0x802A1418): not analysed
bool HostClearBits(HostObj* o) {
    if (o == 0) {
        return false;
    }
    ConditionUpdateBit(o->conditions, 4, 0);
    ConditionUpdateBit(o->conditions, 0x16, 0);
    return true;
}

// TODO (not drafted yet, 0xD4 bytes at 0x802A1568): not analysed
// TODO (not drafted yet, 0x6C8 bytes at 0x802A163C): not analysed
extern "C" void MusicPause();
extern "C" void fn_8016F6A0(int v);
extern "C" void fn_80170E60(int v);
void AudioPauseAll() {
    MusicPause();
    fn_8016F6A0(0);
    fn_80170E60(0);
}

extern "C" void MusicResume();
extern int lbl_804A46F8_arr[];

// Pushes a new level-state value (1..8) on the 5-deep stack and returns the previous top.
int LevelFlagPush(int v) {
    if (v > 0 && v < 9) {
        int n = lbl_804D5734 + 1;
        (&lbl_804A46F8)[lbl_804D5734] = lbl_804D572C;
        lbl_804D5730 = v;
        lbl_804D5734 = n < 5 ? n : 5;
        if (v == 3) {
            MusicResume();
            fn_8016F6A0(1);
            fn_80170E60(1);
        }
        return lbl_804A46F4[lbl_804D5734];
    }
    return 0;
}

extern void* lbl_804D3858;
void* LevelSlotCurrent() {
    return lbl_804D3858;
}

// TODO (not drafted yet, 0x2C bytes at 0x802A1E90): not analysed
extern "C" char lbl_803C32B8[];
extern "C" void* fn_802A1EBC(u32 index);
void* fn_802A1EBC(u32 index) {
    if ((int)index >= 0 && index < 10) {
        lbl_804D3858 = lbl_803C32B8 + index * 0x60;
        fn_8016EDD4(*(int*)lbl_804D3858);
    }
    return lbl_804D3858;
}

extern "C" char* strchr(const char* s, int c);
extern "C" char* strrchr(const char* s, int c);
extern char lbl_804A46B8[];

// Returns the file-name part of the current level path, without extension.
char* LevelFileName() {
    strcpy(lbl_804A46B8, lbl_804A4598);
    char* dot = strchr(lbl_804A46B8, '.');
    if (dot != 0) {
        *dot = 0;
    }
    char* slash = strrchr(lbl_804A46B8, '/');
    if (slash == 0) {
        slash = strrchr(lbl_804A46B8, '\\');
        if (slash == 0) {
            return lbl_804A46B8;
        }
        return slash + 1;
    }
    return slash + 1;
}

void LevelNameSet(const char* name) {
    strcpy(lbl_804A4598, name);
}

extern char lbl_804A4698[];
char* LevelNameBuffer2() {
    return lbl_804A4698;
}

void LevelName2Set(const char* name) {
    strncpy(lbl_804A4698, name, 0x1F);
    lbl_804A4698[0x1F] = 0;
}

extern float lbl_804DA820;
float TimeScaleValue() {
    if (lbl_804D3868 != 0) {
        return lbl_804D3864;
    }
    return lbl_804DA820;
}

struct ListNode {
    char pad00[0x38];
    int id;             // 0x38
    char pad3C[0xFB0 - 0x3C];
    ListNode* next;     // 0xFB0
    ListNode* prev;     // 0xFB4
};
extern ListNode* lbl_804D5740;
extern int lbl_804D5744;
extern "C" void fn_8000F60C(int id);
extern "C" void fn_8021284C(int id);
extern "C" void fn_802A2210(int a, int b, int* c, int d);
extern "C" void fn_80026660(int a, void (*fn)(int, int, int*, int), int b);
void ActorUnlinkOwnerObjects(Actor* a);                 // 0x80022FF0

// Frees every node of the global list.
static inline void ListFreeAll() {
    ListNode* n;
    while ((n = lbl_804D5740) != 0) {
        if (lbl_804D5740 != 0) {
            fn_8000F60C(AssetFind(0xF2E2BEB8, 0xFAC4DCB6));
            fn_8021284C(n->id);
            ActorUnlinkOwnerObjects((Actor*)n);
            if (n->next != 0) {
                n->next->prev = n->prev;
            }
            if (n->prev != 0) {
                n->prev->next = n->next;
            }
            if (lbl_804D5740 == n) {
                lbl_804D5740 = n->next;
            }
            VFree(n);
        }
    }
}

extern "C" void fn_802A20A4(void);
void fn_802A20A4(void) {
    ListFreeAll();
}

extern "C" void fn_802A214C(void);
void fn_802A214C(void) {
    ListFreeAll();
    lbl_804D5744 = 0;
    fn_80026660(0x61, fn_802A2210, 0);
}

// TODO (not drafted yet, 0x428 bytes at 0x802A2210): not analysed
// TODO (not drafted yet, 0x410 bytes at 0x802A2638): not analysed
void ActorMsgForward(int a, int* b) {
    fn_802A2210(*(int*)(*b + 0x38), a, b, 0);
}

// TODO (not drafted yet, 0x218 bytes at 0x802A2A7C): not analysed
// TODO (not drafted yet, 0x95C bytes at 0x802A2C94): not analysed
int StructCopy18(u32* src, u32* dst) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[7] = src[7];
    dst[8] = src[8];
    dst[9] = src[9];
    u32 a = src[11];
    dst[10] = src[10];
    dst[11] = a;
    u32 b = src[13];
    dst[12] = src[12];
    dst[13] = b;
    dst[14] = src[14];
    u32 c = src[16];
    dst[15] = src[15];
    dst[16] = c;
    dst[17] = src[17];
    return 1;
}

// TODO (not drafted yet, 0xF0 bytes at 0x802A3688): not analysed
// TODO (not drafted yet, 0xEC bytes at 0x802A3778): not analysed
// TODO (not drafted yet, 0x254 bytes at 0x802A3864): not analysed
void EmitterDestroy(int handle);                        // 0x800D3694
extern "C" void fn_8028B35C(void* buf, int at, int size);

struct FxRecItem {
    int pad00;
    int handle;         // 0x04
    char pad08[0x28];
};

// Effect record: time left, item count, then 0x30-byte items starting at +0x30.
struct FxRec {
    float time;         // 0x00
    int count;          // 0x04
    char pad08[0x28];
    FxRecItem items[1]; // 0x30
};

// Destroys every effect record in the list, then the list itself.
void EffectListFreeAll() {
    while (((u32)lbl_804D5750->mLength >> 2) != 0) {
        FxRec* rec = *(FxRec**)lbl_804D5750->mUnk0C;
        for (int i = 0; i != rec->count; i++) {
            if (rec->items[i].handle != -1) {
                EmitterDestroy(rec->items[i].handle);
            }
        }
        VFree(*(FxRec**)lbl_804D5750->mUnk0C);
        fn_8028B35C(lbl_804D5750, lbl_804D5750->mUnk0C, 4);
    }
    if (lbl_804D5750 != 0) {
        delete lbl_804D5750;
    }
}

// Ticks every effect record by dt and frees the expired ones.
void EffectListUpdate(float dt) {
    int* p = (int*)lbl_804D5750->mUnk0C;
    while (p != (int*)(lbl_804D5750->mUnk0C + lbl_804D5750->mLength)) {
        FxRec* rec = (FxRec*)*p;
        int r;
        if (dt <= rec->time) {
            r = 0;
            rec->time = rec->time - dt;
        } else {
            r = -1;
        }
        if (r == -1) {
            for (int i = 0; i != rec->count; i++) {
                if (rec->items[i].handle != -1) {
                    EmitterDestroy(rec->items[i].handle);
                }
            }
            VFree(rec);
            fn_8028B35C(lbl_804D5750, (int)p, 4);
        } else {
            p++;
        }
    }
}

struct TexKey {
    u32 key;
    short id;
    short pad;
};
extern "C" TexKey lbl_803C3678[4];
extern "C" void fn_802A3CE0(void);

// Resolves and loads the four effect textures.
void fn_802A3CE0(void) {
    u32 i = 0;
    do {
        lbl_803C3678[i].id = AssetFind(0xF2E2BEB8, lbl_803C3678[i].key);
        TextureLoad(&lbl_803C3678[i].id, 1);
        i++;
    } while (i != 4);
}

extern "C" void fn_802A3D50(void);
void fn_802A3D50(void) {
    lbl_804D5750 = new EffectList();
}

void EmptyRangeLoopA(void* begin, void* end);
void EmptyRangeLoopB(void* begin, void* end);

void EffectListArrayConstruct(void* self, void* begin, u32 size) {
    fn_8028BC2C(self);
    EmptyRangeLoopA(begin, (char*)begin + (size & ~3));
}

void EffectListArrayDestruct(void* self, void* begin, u32 size) {
    EmptyRangeLoopB(begin, (char*)begin + (size & ~3));
    fn_8028BC28(self, begin, size);
}

int ReturnFour() {
    return 4;
}

void EmptyRangeLoopA(void* begin, void* end) {
    for (char* p = (char*)begin; p < (char*)end; p += 4) {
    }
}

void EmptyRangeLoopB(void* begin, void* end) {
    for (char* p = (char*)begin; p < (char*)end; p += 4) {
    }
}

// TODO (not drafted yet, 0x84 bytes at 0x802A3EFC): not analysed
// TODO (not drafted yet, 0x250 bytes at 0x802A3F80): not analysed
extern "C" int rand(void);
extern float lbl_804DA8B8;
extern float lbl_804DA8BC;
extern float lbl_804DA8C0;

struct JitterObj {
    char pad00[0x8C];
    float f8C;          // 0x8C
    float f90;          // 0x90
};

// Randomises two offsets of the object.
int JitterObjRandomize(int unused, JitterObj* o) {
    o->f8C = lbl_804DA8BC * ((float)rand() / lbl_804DA8B8) - lbl_804DA8C0;
    o->f90 = lbl_804DA8BC * ((float)rand() / lbl_804DA8B8) - lbl_804DA8C0;
    return 1;
}

void HostFxA(int unused, HostObjFx* o) {
    if (o->model != -1) {
        char m[0x30];
        ActorGetMatrix(m, (Actor*)o);
        fn_8003F15C(lbl_804DA8C4, o->model, m, 1, 0, 0);
    }
}

// TODO (not drafted yet, 0x4DC bytes at 0x802A42C4): not analysed
void HostFxB(int unused, HostObjFx* o) {
    if (o->model != -1) {
        char m[0x30];
        ActorGetMatrix(m, (Actor*)o);
        fn_8003F15C(lbl_804DA8E8, o->model, m, 1, 0, 0);
    }
}

void HostStateEnterE(HostObj* o, float dt) {
    o->i638 = 0;
    o->flags7B4 &= 0xFFFFFFFE;
    fn_800769A0();
    PlayerSelectCollisionUpdate(o, dt);
    if (o->iC4 == -1) {
        if (o->i7D0 == 0) {
            PlayerSetValue4(o->sub7A4, 1);
        } else {
            PlayerSetValue4(o->sub7A4, 3);
        }
    } else {
        PlayerSetValue4(o->sub7A4, 0x16);
    }
}

struct ResetObj {
    char pad00[0x8C];
    int i8C;            // 0x8C
    char pad90[8];
    int i98;            // 0x98
    char pad9C[0x28];
    int iC4;            // 0xC4
    char padC8[0x28];
    int iF0;            // 0xF0
    char padF4[0x28];
    int i11C;           // 0x11C
};

void ObjRestoreSaved(ResetObj* o) {
    o->i8C = 0;
    if (o->iC4 != -1) {
        o->i98 = o->iC4;
    }
    if (o->i11C != -1) {
        o->iF0 = o->i11C;
    }
}

void HostStateEnterD(HostObj* o) {
    PlayerResetGunTimer(o);
    o->i638 = 1;
    o->flags7B4 &= 0xFFFFFFFE;
    PlayerSetValue4(o->sub7A4, 0x15);
    if (o->i279C == -1 || fn_80175EFC() != 0) {
        PlayerSetValue4(o->sub7A4, 3);
        fn_80155A84(o->sub26E0, o->i26E8, 1);
    }
}

// TODO (not drafted yet, 0xFC bytes at 0x802A4940): not analysed
void HostStateEnterF(HostObj* o, float dt) {
    PlayerResetGunTimer(o);
    o->i638 = 1;
    o->flags7B4 &= 0xFFFFFFFE;
    fn_800769A0();
    PlayerSelectCollisionUpdate(o, dt);
    if (fn_80049940() == 0) {
        if (PlayerGetHeldSlotIndex(o) == 0) {
            PlayerSetValue4(o->sub7A4, 1);
        } else {
            PlayerSetValue4(o->sub7A4, 2);
        }
    } else {
        PlayerSetValue4(o->sub7A4, 2);
    }
    PlayerInHost((Actor*)o);
    DashUpdate((Actor*)o);
}

// TODO (not drafted yet, 0x170 bytes at 0x802A4AF8): not analysed
void HostStateEnterB(HostObj* o) {
    PlayerResetGunTimer(o);
    o->i638 = 1;
    if ((o->flags7B4 & 1) == 0) {
        o->flags50 |= 0x20000;
        o->i78 = 1;
    } else {
        o->flags7B4 &= 0xFFFFFFFE;
    }
}

// TODO (not drafted yet, 0x160 bytes at 0x802A4CC4): not analysed
extern "C" void fn_80155A84(void* a, int b, int c);
struct BigObj {
    char pad00[0x26E0];
    char sub[8];        // 0x26E0
    int i26E8;          // 0x26E8
};

void BigObjSubInit(BigObj* o) {
    fn_80155A84(o->sub, o->i26E8, 1);
}

void HostStateEnterC(HostObj* o) {
    PlayerResetGunTimer(o);
    o->i638 = 1;
    o->flags7B4 &= 0xFFFFFFFE;
    o->flags50 |= 0x20000;
    o->i78 = 1;
    PlayerSetValue4(o->sub7A4, 0x13);
}

// TODO (not drafted yet, 0x15C bytes at 0x802A4EAC): not analysed
struct FlagObj {
    char pad00[0x50];
    u32 flags;          // 0x50
};

void ObjFlagsClear40000(FlagObj* o) {
    o->flags &= 0xFFFBFFFF;
}

void HostStateEnterA(HostObj* o, float dt) {
    PlayerResetGunTimer(o);
    o->i638 = 1;
    o->flags7B4 &= 0xFFFFFFFE;
    PlayerSelectCollisionUpdate(o, dt);
}

// TODO (not drafted yet, 0x110 bytes at 0x802A5070): not analysed
void HostStateEnter180(HostObj* o, float dt) {
    o->i638 = 1;
    o->flags7B4 &= 0xFFFFFFFE;
    fn_800769A0();
    PlayerSelectCollisionUpdate(o, dt);
    DashUpdate((Actor*)o);
    PlayerSetValue4(o->sub7A4, 3);
}

// TODO (not drafted yet, 0x1B8 bytes at 0x802A51EC): not analysed
void HostStateEnter3A4(HostObj* o, float dt) {
    o->i638 = 1;
    o->flags7B4 &= 0xFFFFFFFE;
    fn_800769A0();
    PlayerSelectCollisionUpdate(o, dt);
    DashUpdate((Actor*)o);
    PlayerSetValue4(o->sub7A4, 3);
}

// TODO (not drafted yet, 0x1B8 bytes at 0x802A5410): not analysed
void ActorPlayAnimD39F() {
    int h = fn_800498D0();
    ActorPlayAnim(0x2000000, 0xD39F69B4, h);
}

void HostStateEnter5FC(HostObj* o, float dt) {
    o->i638 = 1;
    o->flags7B4 &= 0xFFFFFFFE;
    fn_800769A0();
    PlayerSelectCollisionUpdate(o, dt);
    DashUpdate((Actor*)o);
    PlayerSetValue4(o->sub7A4, 1);
}

// TODO (not drafted yet, 0x140 bytes at 0x802A5668): not analysed
void HostStateEnter7A8(HostObj* o, float dt) {
    o->i638 = 1;
    o->flags7B4 &= 0xFFFFFFFE;
    fn_800769A0();
    PlayerSelectCollisionUpdate(o, dt);
    PlayerSetValue4(o->sub7A4, 3);
    DashUpdate((Actor*)o);
}

// TODO (not drafted yet, 0x10C bytes at 0x802A5814): not analysed
// TODO (not drafted yet, 0x288 bytes at 0x802A5920): not analysed
// TODO (not drafted yet, 0xB4 bytes at 0x802A5BA8): not analysed
// TODO (not drafted yet, 0x61C bytes at 0x802A5C5C): not analysed
// TODO (not drafted yet, 0x1F4 bytes at 0x802A6278): not analysed
void ActorPlayAnim8B3A() {
    int h = fn_800498D0();
    ActorPlayAnim(0x2000000, 0x8B3A27A3, h);
}

void HostStateEnter4A0(HostObj* o, float dt) {
    o->i638 = 1;
    o->flags7B4 &= 0xFFFFFFFE;
    fn_800769A0();
    PlayerSelectCollisionUpdate(o, dt);
    DashUpdate((Actor*)o);
    PlayerSetValue4(o->sub7A4, 3);
}

// TODO (not drafted yet, 0x140 bytes at 0x802A650C): not analysed
// TODO (not drafted yet, 0x520 bytes at 0x802A664C): not analysed
// TODO (not drafted yet, 0xBC bytes at 0x802A6B6C): not analysed
void PlayerResetGunTimer(void* p);
void GunTimerResetForward(void* p) {
    PlayerResetGunTimer(p);
}

// TODO (not drafted yet, 0x510 bytes at 0x802A6C48): not analysed
// TODO (not drafted yet, 0x27C bytes at 0x802A7158): not analysed
// TODO (not drafted yet, 0x338 bytes at 0x802A73D4): not analysed

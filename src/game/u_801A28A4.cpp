// u_801A28A4.cpp: a Geist possession helper unit: state bits, follow slots and the dog-fetch message check
// (0x801A28A4-0x801A6094). Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;
typedef signed int s32;

extern float lbl_804D8710;
extern float lbl_804D8714;
extern float lbl_804D8718;
extern u8 lbl_803C6B08[];
extern u8 lbl_80348390[];
extern char lbl_803483DA[];

// Runtime state-machine table: 16-byte records.
struct GeistStateEntry {
    void (*enter)(int actor);   // 0x0
    int (*leave)(int actor);    // 0x4
    int (*update)(int actor);   // 0x8
    const char* name;           // 0xc
};
extern GeistStateEntry lbl_803AAA08[];

int HasCondition(void* mem, int cond);
int MemFlagIsSet(void* mem, int flag);
void ConditionSetBit(void* mem, int bit, int value);
void ConditionUpdateBit(void* mem, int bit, int value);
extern "C" void fn_800BBCA8(int actor);

extern "C" int fn_80019F88(int* p);
extern "C" u32 fn_800A3A40(int actor);
extern "C" int fn_800C5654(u32, u32, int);
extern "C" int fn_800A7B78(int, int);
extern "C" int fn_80049940(void);
extern "C" void fn_800A7B08(int, int, int);
extern "C" void fn_800A7A54(int, int, int);
extern "C" void fn_800A78A0(int, int);
extern "C" void fn_800050B4(void*, int, int);
extern "C" int fn_800265D4(int, int, void*);
extern "C" void fn_800BC56C(int, int);
extern "C" void fn_8028C188(void*, int, int);
extern "C" void fn_800225C8(void*, void*, void*);
extern "C" void fn_80162658(int, int);
int GeistA28A4SplineDebugMsg(int* p);
u32 GeistA28A4DogFetchPickState(int actor);

class Buffer {
public:
    virtual ~Buffer();
};

class String : public Buffer {
public:
    String();
    String(const char* s);
    String(const String& a, const String& b) { fn_800225C8(this, (void*)&a, (void*)&b); }
    virtual ~String() {}
    int mData;                        // 0x4
    int mSize;                        // 0x8
    u8 unkC[0x14 - 0xC];              // 0xc
};

// Message handler for the actor this unit belongs to.
int u801A28A4MsgHandler(int msg, int* p)
{
    if (msg == 0x41D) {
        u8* state = (u8*)*(int*)(p[0] + 0x130);
        if (fn_80019F88(p) != 0)
            return 1;
        if (p[1] != 0x2DD2C39C) {
            if (p[1] < 0x2DD2C39C && p[1] == 0x1EFEB8) {
                if (p[3] == 0)
                    state[0x15] &= ~2;
                else
                    state[0x15] |= 2;
                state[0x15] |= 1;
                return 0;
            }
            return 0;
        }
        *(int*)(state + 0x10) = p[3];
        state[0x15] |= 1;
        return 1;
    }

    if (msg >= 0x41D) {
        if (msg == 0xC14)
            return GeistA28A4SplineDebugMsg(p);
        if (msg < 0xC14) {
            if (msg < 0x41F) {
                u8* state;
                if (p[1] != (int)0xAD8576FA)
                    return 0;
                state = (u8*)*(int*)(p[0] + 0x130);
                String first(lbl_803AAA08[state[0x14]].name);
                String second((const char*)(lbl_80348390 + 0x48));
                String joined(first, second);
                fn_8028C188(&p[3], joined.mData, joined.mSize);
                return 0;
            }
            return 0;
        }
        if (msg == 0x1800) {
            int actor = p[0];
            if (*(char*)(*(int*)(actor + 0x130) + 0x14) == 0x0A) {
                int buf[8];
                fn_800050B4(buf, 0, 0x20);
                if (fn_800265D4(p[1], 0x400, buf) != 0 && buf[0] != 0) {
                    fn_800A78A0(actor + 0x270, buf[0]);
                    fn_800BC56C(actor, buf[0]);
                    *(int*)(actor + 0x224) = p[3];
                    *(int*)(actor + 0x228) = p[4];
                }
                return 1;
            }
            return 0;
        }
        return 0;
    }

    if (msg == 0x408)
        return 0;
    if (msg >= 0x408) {
        if (msg == 0x40A) {
            int actor = p[0];
            int arg[2];
            int flags;
            u32 hash;
            char* name;
            char* end;
            u32 c;
            arg[0] = actor;
            arg[1] = fn_800A3A40(actor);
            (*(void (**)(int, void*))(*(int*)actor + 0x30))(0x400, arg);
            hash = p[2];
            name = lbl_803483DA;
            for (end = name; *end != 0; end++) {
                if (*name == (char)0xFF)
                    c = 0xFFFFFFFF;
                else
                    c = lbl_803C6B08[(u8)*name];
                hash *= 31;
                hash += c & 0xFF;
                name++;
            }
            if (fn_800C5654(fn_800A3A40(actor), hash, p[1]) != 0) {
                p[3] = 0;
                return 1;
            }
            if (fn_800A7B78(actor + 0x270, 0xB) != 0)
                return 1;
            if (lbl_804D8710 < *(float*)(actor + 0x904)) {
                flags = 0;
                if (p[2] == 0x3CA34BD6 && (flags & 1) == 0) {
                    p[3] = 0x1379C466;
                    return 1;
                }
                if (p[2] == 0x7C44EC3D && (flags & 1) == 0 && fn_80049940() != 0) {
                    p[3] = (int)0xA53F7E23;
                    return 1;
                }
            }
            return 0;
        }
        if (msg < 0x40A) {
            int actor = p[0];
            u8* state = (u8*)*(int*)(actor + 0x130);
            if (lbl_804D8710 <= *(float*)(state + 0xC)) {
                fn_800A7B08(actor + 0x270, 4, 0);
            } else {
                fn_800A7A54(actor + 0x270, 0x12, 1);
                fn_800A78A0(actor + 0x270, p[1]);
                fn_800A7B08(actor + 0x270, 4, 1);
                *(float*)(state + 0xC) = lbl_804D8718;
            }
            return 0;
        }
        return 0;
    }

    if (msg == 0x400) {
        int actor = p[0];
        float v;
        p[1] = *(int*)(actor + 0x38);
        p[3] = (lbl_804D8710 < *(float*)(actor + 0x904));
        v = lbl_804D8714;
        ((float*)p)[4] = v;
        ((float*)p)[5] = v;
        p[6] = 2;
        p[7] = 0;
        return 1;
    }
    if (msg > 0x406) {
        *(u8*)(*(int*)(p[0] + 0x130) + 0x15) |= 1;
        return 0;
    }
    return 0;
}

// Per-frame update: advances the state timer and runs the current state's
// update callback, switching states when the callback or a flag asks for it.
int u801A2D90Update(int actor)
{
    u8* state = (u8*)*(int*)(actor + 0x130);

    if (lbl_804D8710 < *(float*)(actor + 0x904) &&
        HasCondition((void*)(actor + 0x270), 10) == 0 &&
        HasCondition((void*)(actor + 0x270), 11) == 0) {
        u32 next;

        if (MemFlagIsSet((void*)(actor + 0x270), 5) != 0 && state[0x14] != 1)
            state[0x15] |= 1;
        *(float*)state = *(float*)state + *(float*)(actor + 0x138);
        if (lbl_803AAA08[state[0x14]].update(actor) != 0 || (state[0x15] & 1) != 0) {
            state[0x15] &= ~1;
            next = GeistA28A4DogFetchPickState(actor);
            if (state[0x14] != next && lbl_803AAA08[state[0x14]].leave(actor) != 0) {
                *(float*)state = lbl_804D8710;
                lbl_803AAA08[next].enter(actor);
                ConditionSetBit((void*)(actor + 0x270), 0x40, 0);
                ConditionSetBit((void*)(actor + 0x270), 0x41, 0);
                ConditionSetBit((void*)(actor + 0x270), 0x43, 0);
                ConditionSetBit((void*)(actor + 0x270), 0x42, 0);
            }
        }
        fn_80162658(actor, actor + 0x894);
    }
    return 1;
}

void VFree(void* p);
void* VAlloc(int size);
int AssetFind(u32 id, u32 index);
extern "C" void* memset(void* dst, int val, u32 n);
extern "C" void fn_800CC984();
u32 GeistA28A4DogFetchMsgCheck(int self, u32 msg);
extern "C" void fn_800B2E34(int actor, int a, int b);
extern "C" void fn_800B3164(int actor, int a, int b);
extern "C" void fn_800CECCC(int actor);
extern "C" int SendMsg(int to, int msg, void* data);
class Owner;
void OwnerUpdateSlots(Owner* owner);
int FxFind(const char* name);
int GeistA28A4DogFetchCheck(int actor);
extern int lbl_804D3090;
extern int lbl_804D3094;
extern int lbl_804D3098;

// Releases the state buffer at +0x130 and clears the handler slot at +0x104.
void GeistA28A4StateDetach(int self)
{
    *(int*)(self + 0x104) = 0;
    VFree(*(void**)(self + 0x130));
    *(int*)(self + 0x130) = 0;
}

// Allocates the 0x24-byte state buffer, installs the handlers and enters the
// initial state chosen by the actor query at 0x801A5E00.
void GeistA28A4StateAttach(int self)
{
    char* mem = (char*)VAlloc(0x24);

    if (mem == 0) {
        *(int*)(self + 0x130) = 0;
    } else {
        int filler = (int)&fn_800CC984;

        *(char**)(self + 0x130) = mem;
        *(int*)(self + 0xfc) = (int)&GeistA28A4DogFetchCheck;
        *(int*)(self + 0x5fc) = filler;
        *(int*)(self + 0x600) = filler;
        memset(mem, 0, 0x24);
        mem[0x16] = 1;
        *(float*)mem = lbl_804D8710;

        u8 idx = (u8)GeistA28A4DogFetchPickState(self);
        mem[0x14] = idx;
        lbl_803AAA08[idx].enter(self);

        lbl_804D3090 = AssetFind(0xF1754BF2, 0x46B8BB2);
        lbl_804D3094 = AssetFind(0xF1754BF2, 0x68539EA1);
    }
}

// Allocates the 0x24-byte dog-fetch state buffer, installs the state
// callbacks and enters the initial state chosen by fn_801A5E00.
void GeistA28A4DogFetchInit(int self)
{
    char* mem = (char*)VAlloc(0x24);

    if (mem == 0) {
        *(int*)(self + 0x130) = 0;
        return;
    }

    *(char**)(self + 0x130) = mem;
    *(int*)(self + 0xfc) = (int)&GeistA28A4DogFetchCheck;
    *(int*)(self + 0x604) = (int)&GeistA28A4DogFetchMsgCheck;
    *(int*)(self + 0x5fc) = (int)&fn_800CC984;
    *(int*)(self + 0x600) = (int)&fn_800CC984;

    *(float*)(mem + 0x0) = lbl_804D8710;
    *(float*)(mem + 0x8) = lbl_804D8710;
    *(float*)(mem + 0xc) = lbl_804D8710;
    *(int*)(mem + 0x10) = -1;

    mem[0x14] = (u8)GeistA28A4DogFetchPickState(self);
    mem[0x15] = 0;
    mem[0x16] = 0;
    mem[0x14] = (u8)GeistA28A4DogFetchPickState(self);

    lbl_803AAA08[(u8)mem[0x14]].enter(self);

    lbl_804D3090 = AssetFind(0xF1754BF2, 0x46B8BB2);
    lbl_804D3094 = AssetFind(0xF1754BF2, 0x68539EA1);
    lbl_804D3098 = FxFind((const char*)(lbl_80348390 + 0x50));
}

// Dog-fetch state check: while the actor's condition flag is clear it
// releases the actor's slots and broadcasts the fetch request for each of
// its four sub-objects; otherwise it updates the owner's slots normally.
int GeistA28A4DogFetchCheck(int actor)
{
    if (actor == 0)
        return 0;

    char* state = *(char**)(actor + 0x130);

    if (fn_800A7B78(actor + 0x270, 5) != 0) {
        OwnerUpdateSlots((Owner*)actor);
        return 1;
    }

    fn_800B2E34(actor, 0, 0);
    fn_800B3164(actor, 0, 0);

    char* slot = (char*)actor;
    for (int i = 0; i < 4; i++, slot += 0x28) {
        if (*(int*)(state + 0x10) == -1 && (*(u32*)(slot + 0x158) & 0x40) == 0 &&
            (*(u32*)(slot + 0x158) & 1) != 0) {
            int owner;
            String name;
            int msg[4];

            *(int*)(slot + 0x150) = 4;
            owner = *(int*)(actor + 0x38);
            msg[1] = (int)0xF9D0235B;
            msg[2] = 0;
            msg[3] = 1;
            SendMsg(owner, 0x41D, msg);
        }
    }

    fn_800CECCC(actor);
    return 1;
}

// Dog-fetch state message check: asks the target at +0x130's actor object
// whether the message can be accepted in the current state.
u32 GeistA28A4DogFetchMsgCheck(int self, u32 msg)
{
    int obj = *(int*)(self + 0x130);
    int buf[8];

    memset(buf, 0, 0x20);
    if (SendMsg(msg, 0x400, buf) == 0 || buf[0] == 0)
        return 0;
    if (*(int*)(obj + 0x10) != -1)
        return (msg >> 24) == 0x4a;
    switch (msg >> 24) {
    case 0x13:
        if (HasCondition((void*)(buf[0] + 0x270), 0xb) == 0 && buf[3] != 0)
            return 1;
        return 0;
    case 2:
        return (u32)(-buf[3] | buf[3]) >> 31;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// Dog-fetch state update (0x801A33B4).
extern float lbl_804D8730;
extern float lbl_804D8734;
extern float lbl_804D8738;
extern float lbl_804D873C;
extern float lbl_804D8740;
extern float lbl_804D8744;
extern float lbl_804D8748;
extern float lbl_804D874C;
extern float lbl_804D8750;
extern float lbl_804D8754;
extern float lbl_804D8758;
extern float lbl_804D875C;
extern float lbl_804D8760;
extern float lbl_804D8764;
extern float lbl_804D8768;
extern int lbl_804D5168;

float __fabsf(float f);
inline float fabsf(float x) { return __fabsf(x); }

extern "C" int fn_800B26B8(int actor);
extern "C" u32 fn_800B1ED4(int actor, int mode);
extern "C" int fn_800B1EF4(int actor, int mode);
extern "C" void fn_800B1F20(int actor, int mode);
extern "C" void fn_800B2418(int actor, int mode, float f);
extern "C" int fn_800B261C(int actor, int mode, void* out);
extern "C" int fn_800B912C(int a, u32 id, int b, int c, void* fn, void* actor);
extern "C" int fn_800B9360(int a, u32 id, int b, int c);
extern "C" int fn_800BAF40(int actor);
extern "C" void fn_800BBCA8(int actor);
extern "C" int FollowGetTargetObj(void* follow);
extern "C" int FollowGetTargetPos(void* follow, void* pos);
extern "C" void FollowSetTargetVec(void* follow, void* pos);
extern "C" void ActorAimOffset2D(void* actor, float* pos, float* yaw, float* pitch);
extern "C" void ActorGetMatrix(void* mtx, void* actor);
extern "C" int ActorPlayAnim(int actor, u32 anim, int arg);
extern "C" void fn_800A025C(float* out, void* mtx, int idx);
extern "C" void fn_800E1098(float* dst, float* src, float scale);
extern "C" void RangeResetPending(void* p);
int GeistA28A4FetchAimCheck(int targetId, int actor);
extern "C" void fn_8016F120(int a, int b);

// Dog-fetch state update: tracks the target, aims at it and runs the
// avoidance/approach state machine held in the +0x130 block.
u32 GeistA28A4DogFetchUpdate(int actor)
{
    u32 flags = 0;
    char* obj = *(char**)(actor + 0x130);
    int mode = fn_800B26B8(actor);
    float pos[3];
    float aimYaw;
    float aimPitch;
    float pos2[3];
    float aimYaw2;
    float aimPitch2;
    char* tgt;
    char* tgt2;
    u32 type;
    u8 fl;

    if (mode != 4)
        flags = fn_800B1ED4(actor, mode);

    tgt = (char*)FollowGetTargetObj((void*)(actor + 0x270));
    if (tgt != 0) {
        pos[0] = (float)*(int*)(tgt + 0x24);
        pos[1] = (float)*(int*)(tgt + 0x28);
        pos[2] = (float)*(int*)(tgt + 0x2c);
        ActorAimOffset2D((void*)actor, pos, &aimYaw, &aimPitch);
    } else if (FollowGetTargetPos((void*)(actor + 0x270), pos) != 0) {
        ActorAimOffset2D((void*)actor, pos, &aimYaw, &aimPitch);
    } else {
        aimPitch = lbl_804D8730;
        aimYaw = lbl_804D8710;
    }
    while (aimYaw < lbl_804D8738)
        aimYaw += lbl_804D8734;
    while (aimYaw > lbl_804D873C)
        aimYaw -= lbl_804D8734;

    int buf[8];
    if (SendMsg(*(int*)(obj + 0x10), 0x400, buf) == 0 || buf[0] == 0)
        return 1;

    tgt2 = (char*)buf[0];
    pos2[0] = (float)*(int*)(tgt2 + 0x24);
    pos2[1] = (float)*(int*)(tgt2 + 0x28);
    pos2[2] = (float)*(int*)(tgt2 + 0x2c);
    ActorAimOffset2D((void*)actor, pos2, &aimYaw2, &aimPitch2);
    while (aimYaw2 < lbl_804D8738)
        aimYaw2 += lbl_804D8734;
    while (aimYaw2 > lbl_804D873C)
        aimYaw2 -= lbl_804D8734;

    type = *(u32*)(tgt2 + 0x38);

    if (fn_800BAF40(actor) == -1 && *(int*)(actor + 0x224) != -1) {
        ActorPlayAnim(*(int*)(actor + 0x224), *(u32*)(actor + 0x228), *(int*)(actor + 0x38));
        *(int*)(actor + 0x224) = -1;
    }

    fl = *(u8*)(obj + 0x15);
    if (fl & 0x10) {
        if (aimPitch2 < lbl_804D8740) {
            *(u8*)(obj + 0x15) = fl & ~0x10;
            *(u8*)(obj + 0x15) = *(u8*)(obj + 0x15) | 0x40;
            fn_800BBCA8(actor);
            *(int*)(actor + 0x288) = 0;
            float mtx[12];
            float v[3];
            ActorGetMatrix(mtx, tgt2);
            fn_800A025C(v, mtx, 1);
            v[1] = lbl_804D8710;
            fn_800E1098(v, v, lbl_804D8744);
            v[0] += *(float*)(actor + 0x84);
            v[1] += *(float*)(actor + 0x88);
            v[2] += *(float*)(actor + 0x8c);
            FollowSetTargetVec((void*)(actor + 0x270), v);
            fn_800A7B08(actor + 0x270, 4, 1);
        } else if (fabsf(aimYaw2) > lbl_804D8748) {
            fn_800A7B08(actor + 0x270, 4, 1);
            fn_800A78A0(actor + 0x270, (int)tgt2);
        } else {
            *(u8*)(obj + 0x15) = fl & ~0x10;
            *(u8*)(obj + 0x15) = *(u8*)(obj + 0x15) | 0x20;
            fn_800BC56C(actor, (int)tgt2);
            *(int*)(actor + 0x288) = 2;
        }
    } else if (fl & 0x20) {
        float f = *(float*)(obj + 8) + *(float*)(actor + 0x138);
        *(float*)(obj + 8) = f;
        if (aimPitch2 < lbl_804D8740 || fn_800BAF40(actor) == -1 || f <= lbl_804D874C) {
            *(u8*)(obj + 0x15) = fl & ~0x20;
            *(u8*)(obj + 0x15) = *(u8*)(obj + 0x15) | 0x40;
            fn_800BBCA8(actor);
            *(int*)(actor + 0x288) = 0;
            float mtx[12];
            float v[3];
            ActorGetMatrix(mtx, tgt2);
            fn_800A025C(v, mtx, 1);
            v[1] = lbl_804D8710;
            fn_800E1098(v, v, lbl_804D8744);
            v[0] += *(float*)(actor + 0x84);
            v[1] += *(float*)(actor + 0x88);
            v[2] += *(float*)(actor + 0x8c);
            FollowSetTargetVec((void*)(actor + 0x270), v);
            fn_800A7B08(actor + 0x270, 4, 1);
        }
    } else if (fl & 0x40) {
        if (fabsf(aimYaw) < lbl_804D8748) {
            *(u8*)(obj + 0x15) = fl & ~0x40;
            fn_800A7B08(actor + 0x270, 4, 0);
            if (*(int*)(obj + 0x10) != -1) {
                int msgData[4];
                msgData[1] = 0;
                msgData[2] = *(int*)(actor + 0x38);
                msgData[3] = 0x21C664;
                SendMsg(*(int*)(obj + 0x10), 0xc14, msgData);
            }
        }
    } else if (fl & 4) {
        *(int*)(actor + 0x288) = 0;
        if (HasCondition((void*)(actor + 0x270), 0x29) == 0)
            *(u8*)(obj + 0x15) = fl & ~4;
    } else if (fl & 8) {
        if (fabsf(aimYaw) <= lbl_804D8748 || aimPitch < lbl_804D8750) {
            fn_800A7B08(actor + 0x270, 4, 0);
            fn_800A7A54(actor + 0x270, 0x29, 1);
            *(u8*)(obj + 0x15) = fl & ~8;
            *(u8*)(obj + 0x15) = *(u8*)(obj + 0x15) | 4;
        } else {
            fn_800A7B08(actor + 0x270, 4, 1);
        }
    } else if (tgt == 0 || (type >> 24) != 0x4a) {
        if (mode < 4 && (flags & 0x40) && (flags & 0x100) && fn_800B1EF4(actor, mode) == 0) {
            int picked;
            fn_800B1F20(actor, mode);
            fn_800B2418(actor, mode, lbl_804D8754);
            if (fn_800B261C(actor, mode, &picked) != 0) {
                *(float*)(obj + 4) = lbl_804D8758;
                fn_800A78A0(actor + 0x270, picked);
                fn_800BC56C(actor, picked);
                *(int*)(actor + 0x288) = 3;
                fn_8016F120(lbl_804D3098, actor + 0x24);
                goto done;
            }
        }
    }

    if (tgt != 0) {
        if ((type >> 24) == 0x4a) {
            if (fn_800BAF40(actor) != -1) {
                if (mode == 4) {
                    RangeResetPending((void*)(actor + 0x270));
                    *(int*)(actor + 0x288) = 0;
                }
                *(float*)(obj + 4) = *(float*)(obj + 4) - *(float*)(actor + 0x138);
                if (*(float*)(obj + 4) <= lbl_804D8710) {
                    fn_800BBCA8(actor);
                    RangeResetPending((void*)(actor + 0x270));
                    *(int*)(actor + 0x288) = 0;
                }
                if (aimPitch < lbl_804D875C) {
                    if (fabsf((lbl_804D8760 + (float)*(int*)(tgt + 0x2c)) - *(float*)(actor + 0x8c)) < lbl_804D8764) {
                        fn_800BBCA8(actor);
                        *(u8*)(obj + 0x15) = *(u8*)(obj + 0x15) | 8;
                        *(int*)(actor + 0x288) = 0;
                    }
                }
            } else {
                *(int*)(actor + 0x288) = 0;
                if (aimPitch < lbl_804D875C)
                    *(u8*)(obj + 0x15) = *(u8*)(obj + 0x15) | 8;
                else
                    RangeResetPending((void*)(actor + 0x270));
            }
        } else if ((type >> 24) != 2) {
            if (fn_800BAF40(actor) != -1) {
                *(int*)(actor + 0x288) = 3;
            } else {
                *(int*)(actor + 0x288) = 0;
                if (aimPitch >= lbl_804D8744)
                    RangeResetPending((void*)(actor + 0x270));
            }
        }
    } else {
        if (lbl_804D5168 != 0) {
            int found = fn_800B912C(0, 0xD1BE1DD, *(int*)(actor + 0x38), 1, (void*)&GeistA28A4FetchAimCheck, (void*)actor);
            if (found == -1)
                found = fn_800B9360(0, 0xD1BE1DD, *(int*)(actor + 0x38), 1);
            if (found != -1) {
                int msgBuf[8];
                memset(msgBuf, 0, 0x20);
                if (SendMsg(found, 0x400, msgBuf) != 0 && msgBuf[0] != 0) {
                    fn_800A78A0(actor + 0x270, msgBuf[0]);
                    fn_800BC56C(actor, msgBuf[0]);
                }
            }
        }
        if (fn_800BAF40(actor) == -1) {
            if (aimPitch2 > lbl_804D8768)
                fn_800A7A54(actor + 0x270, 0x3c, 1);
            else
                fn_800A7A54(actor + 0x270, 0x3c, 0);
        }
    }

done:
    if (HasCondition((void*)(actor + 0x270), 0x3c) != 0) {
        if ((*(u8*)(obj + 0x15) & 0x70) != 0 || (*(u8*)(obj + 0x15) & 0x0c) != 0 || fn_800BAF40(actor) != -1)
            fn_800A7A54(actor + 0x270, 0x3c, 0);
    }
    return 0;
}

extern "C" void fn_800B6CAC(int actor, void* fn);
extern "C" void fn_800B6D04(int actor, void* fn);
float GeistA28A4FetchAimProgress(int actor, float* dir);

// Dog-fetch state start (0x801A3C98): runs the actor wake-up, installs the
// state callback at +0xfc and clears the two fetch range bits.
int GeistA28A4DogFetchStart(int actor)
{
    fn_800BBCA8(actor);
    fn_800B6CAC(actor, (void*)&GeistA28A4FetchAimProgress);
    fn_800A7B08(actor + 0x270, 0x1c, 0);
    fn_800A7A54(actor + 0x270, 0x3c, 0);
    return 1;
}

// Dog-fetch state stop (0x801A3CF8): resets the fetch range, clears the state
// callback at +0xfc, drops the pending action and clears the two range bits.
int GeistA28A4DogFetchStop(int actor)
{
    u8* obj = *(u8**)(actor + 0x130);

    obj[0x14] = 10;
    obj[0x15] &= ~4;
    obj[0x15] &= ~8;
    RangeResetPending((void*)(actor + 0x270));
    fn_800BBCA8(actor);
    fn_800B6D04(actor, (void*)&GeistA28A4FetchAimProgress);
    *(int*)(actor + 0x288) = 0;
    fn_800A7B08(actor + 0x270, 0x1c, 1);
    return 1;
}

// Shared setup for the fetch behaviour's embedded helper, then success.
extern "C" int fn_80162610(int actor, int mem, int arg);

int GeistA28A4FetchCheckEnter(int actor)
{
    fn_80162610(actor, actor + 0x894, 0);
    return 1;
}

// Keeps the fetch condition bits in sync with the attached helper's state.
extern "C" int fn_80162940(int mem, int flag);
void ConditionUpdateBit(void* mem, int bit, int value);

int GeistA28A4FetchStateCheck(int actor)
{
    if (fn_80162940(actor + 0x894, 1) != 0) {
        ConditionUpdateBit((void*)(actor + 0x270), 0, 0);
        return 1;
    }
    if (HasCondition((void*)(actor + 0x270), 0x10) == 0)
        return 0;
    if (HasCondition((void*)(actor + 0x270), 4) == 0 &&
        lbl_804D8768 <= *(float*)(actor + 0x904))
        return 0;
    ConditionSetBit((void*)(actor + 0x270), 0x10, 0);
    ConditionUpdateBit((void*)(actor + 0x270), 0, 0);
    return 1;
}

// Fetch state enter (0x801A3E64): switches to the fetch sub-state, resets the
// state timer, drops the pending action and re-arms the helper condition bit.
int GeistA28A4FetchStateEnter(int actor)
{
    u8* obj = *(u8**)(actor + 0x130);

    obj[0x14] = 9;
    *(float*)(obj + 0xc) = lbl_804D8718;
    fn_800BBCA8(actor);
    *(int*)(actor + 0x288) = 0;
    fn_80162610(actor, actor + 0x894, 1);
    ConditionUpdateBit((void*)(actor + 0x270), 0, 1);
    return 1;
}

// Always-true state predicate (0x801A3ED0).
int GeistA28A4StateAlwaysTrue(void)
{
    return 1;
}

// Dog-fetch target tracking update (0x801A3ED8): picks the current target
// object or slot position from the actor's mode list, keeps the follow
// condition in sync and decides the next fetch step from the target value.
extern "C" void FollowSetTargetObj(void* follow, void* obj);
extern "C" int ConditionGetOwner(void* cond);
extern "C" void OwnerSlotGetPosition(int actor, int slot, float* pos);
class Owner;
float OwnerValue(Owner* owner, void* p);
extern float lbl_804D876C;

int GeistA28A4FetchTrackUpdate(int actor)
{
    int target = FollowGetTargetObj((void*)(actor + 0x270));
    int mode = fn_800B26B8(actor);
    int updated = 0;

    if (mode < 4 && (fn_800B1ED4(actor, mode) & 0x40) == 0) {
        int pending = fn_800B1EF4(actor, mode);
        int obj;

        fn_800B1F20(actor, mode);
        if (fn_800B261C(actor, mode, &obj) != 0) {
            FollowSetTargetObj((void*)(actor + 0x270), (void*)obj);
        } else {
            float pos[3];

            OwnerSlotGetPosition(actor, mode, pos);
            FollowSetTargetVec((void*)(actor + 0x270), pos);
        }
        if (pending == 0) {
            ConditionUpdateBit((void*)(actor + 0x270), 3, 1);
            updated = 1;
        }
    }
    if (updated) {
        if (ConditionGetOwner((void*)(actor + 0x270)) == 2) {
            target = FollowGetTargetObj((void*)(actor + 0x270));
            fn_800BC56C(actor, target);
        } else {
            FollowSetTargetObj((void*)(actor + 0x270), (void*)target);
        }
    }
    if (target == 0)
        return 1;
    {
        float value = OwnerValue((Owner*)actor, (void*)target);

        if (value < (float)*(u32*)(*(int*)(actor + 0xc4) + 0xc8))
            return 1;
        if (fn_800BAF40(actor) == -3) {
            if (value < lbl_804D876C)
                *(int*)(actor + 0x288) = 2;
            else
                *(int*)(actor + 0x288) = 3;
        } else {
            fn_800BC56C(actor, target);
            *(int*)(actor + 0x288) = 0;
        }
    }
    return 0;
}

// Fetch state start (0x801A40CC): switches the state block to fetch, resets
// the state timer, clears the fetch condition bit and clears the pending step.
int GeistA28A4FetchStateStart(int actor)
{
    u8* state = *(u8**)(actor + 0x130);

    state[0x14] = 8;
    *(float*)(state + 0xc) = lbl_804D8718;
    fn_800A7B08(actor + 0x270, 4, 0);
    fn_800BC56C(actor, FollowGetTargetObj((void*)(actor + 0x270)));
    *(int*)(actor + 0x288) = 3;
    return 1;
}

extern "C" int fn_800CDBE0(int actor);

// Follow behaviour enter (0x801A4138): installs the follow-state callback and
// clears the pending fetch step.
int GeistA28A4FollowStateEnter(int actor)
{
    fn_800B6CAC(actor, (void*)&fn_800CDBE0);
    *(int*)(actor + 0x288) = 0;
    return 1;
}

// Follow target range check (0x801A4178): refreshes the follow target from the
// actor's mode slot, then reports whether the target's value is still at least
// the actor's configured range.
int GeistA28A4FollowTargetRangeCheck(int actor)
{
    int target = FollowGetTargetObj((void*)(actor + 0x270));
    int mode = fn_800B26B8(actor);
    int updated = 0;

    if (mode < 4 && (fn_800B1ED4(actor, mode) & 0x40) == 0) {
        int pending = fn_800B1EF4(actor, mode);
        int obj;

        fn_800B1F20(actor, mode);
        if (fn_800B261C(actor, mode, &obj) != 0) {
            FollowSetTargetObj((void*)(actor + 0x270), (void*)obj);
        } else {
            float pos[3];

            OwnerSlotGetPosition(actor, mode, pos);
            FollowSetTargetVec((void*)(actor + 0x270), pos);
        }
        if (pending == 0) {
            ConditionUpdateBit((void*)(actor + 0x270), 3, 1);
            updated = 1;
        }
    }
    if (updated) {
        if (ConditionGetOwner((void*)(actor + 0x270)) == 2) {
            target = FollowGetTargetObj((void*)(actor + 0x270));
            fn_800BC56C(actor, target);
        } else {
            FollowSetTargetObj((void*)(actor + 0x270), (void*)target);
        }
    }
    if (target == 0)
        return 1;
    {
        float value = OwnerValue((Owner*)actor, (void*)target);
        float range = (float)*(u32*)(*(int*)(actor + 0xc4) + 0xcc);

        return value >= range;
    }
}

// Enter handler of behaviour state 7.
int GeistA28A4State7Enter(int actor)
{
    u8* state = *(u8**)(actor + 0x130);
    state[0x14] = 7;
    *(float*)(state + 0xC) = lbl_804D8718;
    ConditionUpdateBit((void*)(actor + 0x270), 4, 1);
    ConditionUpdateBit((void*)(actor + 0x270), 9, 1);
    fn_800BBCA8(actor);
    fn_800B6D04(actor, (void*)fn_800CDBE0);
    *(int*)(actor + 0x288) = 1;
    return 1;
}

// Leave handler of behaviour state 7.
int GeistA28A4State7Leave(int actor)
{
    ConditionUpdateBit((void*)(actor + 0x270), 4, 0);
    ConditionUpdateBit((void*)(actor + 0x270), 9, 0);
    return 1;
}

extern float lbl_804D8770;
extern float lbl_804D8774;

// Fetch range update (0x801A43D4): once the state timer has passed the
// configured start threshold, refreshes the follow target from the actor's
// mode slot and reports whether the target's value exceeds the fetch range.
int GeistA28A4FetchRangeUpdate(int actor)
{
    u8* state = *(u8**)(actor + 0x130);

    if (lbl_804D8770 <= *(float*)state) {
        int mode = fn_800B26B8(actor);

        if (mode < 4 && (fn_800B1ED4(actor, mode) & 0x40) == 0) {
            int pending = fn_800B1EF4(actor, mode);
            int obj;

            fn_800B1F20(actor, mode);
            if (fn_800B261C(actor, mode, &obj) != 0) {
                FollowSetTargetObj((void*)(actor + 0x270), (void*)obj);
            } else {
                float pos[3];

                OwnerSlotGetPosition(actor, mode, pos);
                FollowSetTargetVec((void*)(actor + 0x270), pos);
            }
            if (pending == 0)
                ConditionUpdateBit((void*)(actor + 0x270), 3, 1);
        }
        if (ConditionGetOwner((void*)(actor + 0x270)) != 0) {
            float pos[3];

            FollowGetTargetPos((void*)(actor + 0x270), pos);
            if (lbl_804D8774 < OwnerValue((Owner*)actor, pos))
                return 1;
        }
    }
    return 0;
}

// Enter handler of behaviour state 6 (0x801A4504): marks the state, sets the
// timer and the fetch condition bits, and clears the pending fetch step.
int GeistA28A4State6Enter(int actor)
{
    u8* state = *(u8**)(actor + 0x130);

    state[0x14] = 6;
    *(float*)(state + 0xc) = lbl_804D8718;
    ConditionUpdateBit((void*)(actor + 0x270), 4, 1);
    ConditionUpdateBit((void*)(actor + 0x270), 9, 1);
    fn_800BBCA8(actor);
    *(int*)(actor + 0x288) = 0;
    return 1;
}

// Fetch-state leave (0x801A4574): clears the two fetch condition bits and
// reports success.
int GeistA28A4FetchResetBits(int actor)
{
    ConditionUpdateBit((void*)(actor + 0x270), 8, 0);
    ConditionUpdateBit((void*)(actor + 0x270), 4, 0);
    return 1;
}

extern float lbl_804D8778;
extern float lbl_804D8720;
extern "C" void fn_800B4C50(float, float, int, int, int, int);

// Fetch-slot check (0x801A45C0): while the state timer is past the start
// threshold it refreshes the follow target, then reports whether the target
// value is outside the configured window.
int GeistA28A4FetchSlotCheck(int actor)
{
    float* state = *(float**)(actor + 0x130);

    if (*state < lbl_804D8770)
        return 0;

    int mode = fn_800B26B8(actor);
    int updated = 0;

    if (mode < 4 && (fn_800B1ED4(actor, mode) & 0x40) == 0) {
        int pending = fn_800B1EF4(actor, mode);
        int obj;

        fn_800B1F20(actor, mode);
        if (fn_800B261C(actor, mode, &obj) != 0) {
            FollowSetTargetObj((void*)(actor + 0x270), (void*)obj);
        } else {
            float pos[3];

            OwnerSlotGetPosition(actor, mode, pos);
            FollowSetTargetVec((void*)(actor + 0x270), pos);
        }
        if (pending == 0) {
            ConditionUpdateBit((void*)(actor + 0x270), 3, 1);
            updated = 1;
        }
    }

    if (updated > 0)
        fn_800B4C50(lbl_804D8778, lbl_804D874C, actor + 0x84, *(int*)(actor + 0x38), 2, 0x12);

    if (ConditionGetOwner((void*)(actor + 0x270)) == 0)
        return 1;

    {
        float pos[3];
        float value;

        FollowGetTargetPos((void*)(actor + 0x270), pos);
        value = OwnerValue((Owner*)actor, pos);
        if (value < lbl_804D8774 || value > lbl_804D8720)
            return 1;
    }
    return 0;
}

// Dog-fetch retreat enter (0x801A4734): plays the fetch animation at the
// current target, switches the state block to the retreat state, arms the
// fetch condition bits and clears the pending step.
int GeistA28A4DogFetchRetreat(int actor)
{
    char* state = *(char**)(actor + 0x130);

    if (state[0x14] != 5) {
        int target = FollowGetTargetObj((void*)(actor + 0x270));

        if (target != 0)
            ActorPlayAnim(*(int*)(actor + 0x38), 0x5B99AFD1, *(int*)(target + 0x38));
    }
    state[0x14] = 5;
    *(float*)(state + 0xC) = lbl_804D8718;
    ConditionUpdateBit((void*)(actor + 0x270), 4, 1);
    ConditionUpdateBit((void*)(actor + 0x270), 8, 1);
    fn_800BBCA8(actor);
    *(int*)(actor + 0x288) = 0;
    return 1;
}

// Always-true predicate of the dog-fetch retreat state (0x801A47DC).
int GeistA28A4DogFetchRetreatCheck(void)
{
    return 1;
}

extern float lbl_804D877C;
extern float lbl_804D8780;
extern "C" void fn_800BC5A8(int actor, void* pos);
extern "C" int fn_800BB218(float f, void* actor, int arg, float* origin,
                           float* target, void* owner, float* factor,
                           float* out1, float* out2);

// Dog-fetch retreat state update (0x801A47E4): refreshes the follow target
// from the actor's mode slot, then ends the retreat once the target leaves
// the configured range and picks the follow-up state.
int GeistA28A4DogFetchRetreatUpdate(int actor)
{
    float* state = *(float**)(actor + 0x130);
    int updated = 0;
    int mode = fn_800B26B8(actor);

    if (mode < 4 && (fn_800B1ED4(actor, mode) & 0x40) == 0) {
        int pending = fn_800B1EF4(actor, mode);
        int obj;

        fn_800B1F20(actor, mode);
        if (fn_800B261C(actor, mode, &obj) != 0) {
            FollowSetTargetObj((void*)(actor + 0x270), (void*)obj);
        } else {
            float pos[3];

            OwnerSlotGetPosition(actor, mode, pos);
            FollowSetTargetVec((void*)(actor + 0x270), pos);
        }
        if (pending == 0) {
            ConditionUpdateBit((void*)(actor + 0x270), 3, 1);
            updated = 1;
        }
    }
    if (updated > 0) {
        float pos[3];

        if (ConditionGetOwner((void*)(actor + 0x270)) == 2)
            return 1;
        FollowGetTargetPos((void*)(actor + 0x270), pos);
        fn_800BC5A8(actor, pos);
        *(int*)(actor + 0x288) = 2;
    }
    {
        float pos[3];
        float value;

        FollowGetTargetPos((void*)(actor + 0x270), pos);
        value = OwnerValue((Owner*)actor, pos);
        if (lbl_804D874C < *state || lbl_804D877C < value) {
            RangeResetPending((void*)(actor + 0x270));
            return 1;
        }
        if (value >= (float)*(int*)(*(int*)(actor + 0xc4) + 0xc8)) {
            int st = fn_800BAF40(actor);

            if (st == -3) {
                *(int*)(actor + 0x288) = 2;
            } else {
                st = fn_800BAF40(actor);
                if (st == -1)
                    *(int*)(actor + 0x288) = 0;
            }
            return 0;
        }
    }
    return 1;
}

// Dog-fetch approach state update (0x801A49C0): moves the follow target a
// fraction of the way from the actor's position to the target's position and
// switches the pending action to state 2.
int GeistA28A4DogFetchApproachUpdate(int actor)
{
    char* state = *(char**)(actor + 0x130);
    float tgtPos[3];
    float actorPos[3];
    float factor;
    float out1;
    float out2;

    FollowGetTargetPos((void*)(actor + 0x270), tgtPos);
    state[0x14] = 4;
    *(float*)(state + 0xc) = lbl_804D8718;
    actorPos[0] = (float)*(int*)(actor + 0x24);
    actorPos[1] = (float)*(int*)(actor + 0x28);
    actorPos[2] = (float)*(int*)(actor + 0x2c);
    if (fn_800BB218(*(float*)(actor + 0x900), (void*)actor,
                    *(int*)(actor + 0x2c4), actorPos, tgtPos, (void*)actor,
                    &factor, &out1, &out2) != 0) {
        factor *= lbl_804D8780;
        tgtPos[0] = actorPos[0] + (tgtPos[0] - actorPos[0]) * factor;
        tgtPos[1] = actorPos[1] + (tgtPos[1] - actorPos[1]) * factor;
        tgtPos[2] = actorPos[2] + (tgtPos[2] - actorPos[2]) * factor;
    }
    fn_800BC5A8(actor, tgtPos);
    *(int*)(actor + 0x288) = 2;
    return 1;
}

// Fetch-range slot leave (0x801A4B0C): clears the fetch condition bits and
// reports success.
int GeistA28A4FetchSlotClearBits(int actor)
{
    ConditionUpdateBit((void*)(actor + 0x270), 4, 0);
    ConditionUpdateBit((void*)(actor + 0x270), 0x10, 0);
    return 1;
}

extern float lbl_804D8784;

// Fetch-range slot check (0x801A4B58): while the state timer is past the start
// threshold it refreshes the follow target from the actor's mode slot, then
// reports whether the target is still inside the configured range.
int GeistA28A4FetchSlotRangeCheck(int actor)
{
    float* state = *(float**)(actor + 0x130);
    int updated = 0;
    int mode;

    if (*state < lbl_804D8770)
        return 0;

    mode = fn_800B26B8(actor);
    if (mode < 4 && (fn_800B1ED4(actor, mode) & 0x40) == 0) {
        int pending = fn_800B1EF4(actor, mode);
        int obj;

        fn_800B1F20(actor, mode);
        if (fn_800B261C(actor, mode, &obj) != 0) {
            FollowSetTargetObj((void*)(actor + 0x270), (void*)obj);
        } else {
            float pos[3];

            OwnerSlotGetPosition(actor, mode, pos);
            FollowSetTargetVec((void*)(actor + 0x270), pos);
        }
        if (pending == 0) {
            ConditionUpdateBit((void*)(actor + 0x270), 3, 1);
            updated = 1;
        }
    }

    if (!updated && *state > lbl_804D8784)
        return 1;

    if (ConditionGetOwner((void*)(actor + 0x270)) != 0) {
        float pos[3];
        float value;

        FollowGetTargetPos((void*)(actor + 0x270), pos);
        value = OwnerValue((Owner*)actor, pos);
        if (value < lbl_804D8720 || value > lbl_804D877C)
            return 1;
    }
    return 0;
}

// Activates the special-state block on the actor and enables the two
// condition bits that this state depends on.
int GeistA28A4EnableStateBits(int actor)
{
    u8* state = *(u8**)(actor + 0x130);
    state[0x14] = 3;
    *(float*)(state + 0xc) = lbl_804D8718;
    ConditionUpdateBit((void*)(actor + 0x270), 4, 1);
    ConditionUpdateBit((void*)(actor + 0x270), 0x10, 1);
    fn_800BBCA8(actor);
    *(u32*)(actor + 0x288) = 0;
    return 1;
}

// Deactivates the two condition bits that the previous state enabled.
int GeistA28A4DisableStateBits(int actor)
{
    ConditionUpdateBit((void*)(actor + 0x270), 0xe, 0);
    ConditionUpdateBit((void*)(actor + 0x270), 0xf, 0);
    return 1;
}

extern float lbl_804D871C;
extern float lbl_804D8788;

// Follow-slot range check (0x801A4D84): while the state timer is past the start
// threshold it refreshes the follow target from the actor's active mode slot,
// then toggles the follow condition bits according to the owner value.
int GeistA28A4FollowSlotRangeCheck(int actor)
{
    float* state = *(float**)(actor + 0x130);
    int updated = 0;

    if (*state < lbl_804D8770)
        return 0;

    {
        int mode = fn_800B26B8(actor);

        if (mode < 4 && (fn_800B1ED4(actor, mode) & 0x40) == 0) {
            int pending = fn_800B1EF4(actor, mode);
            int obj;

            fn_800B1F20(actor, mode);
            if (fn_800B261C(actor, mode, &obj) != 0) {
                FollowSetTargetObj((void*)(actor + 0x270), (void*)obj);
            } else {
                float pos[3];

                OwnerSlotGetPosition(actor, mode, pos);
                FollowSetTargetVec((void*)(actor + 0x270), pos);
            }
            if (pending == 0) {
                ConditionUpdateBit((void*)(actor + 0x270), 3, 1);
                updated = 1;
            }
        }
    }

    if (!updated && *state > lbl_804D8784)
        return 1;

    if (ConditionGetOwner((void*)(actor + 0x270)) != 0) {
        float pos[3];
        float value;

        FollowGetTargetPos((void*)(actor + 0x270), pos);
        value = OwnerValue((Owner*)actor, pos);
        if (value > lbl_804D871C || value < lbl_804D877C)
            return 1;
        if (value <= lbl_804D871C && value > lbl_804D8788) {
            ConditionUpdateBit((void*)(actor + 0x270), 0xe, 1);
            ConditionUpdateBit((void*)(actor + 0x270), 0xf, 0);
        } else {
            ConditionUpdateBit((void*)(actor + 0x270), 0xe, 0);
            ConditionUpdateBit((void*)(actor + 0x270), 0xf, 1);
        }
    }
    return 0;
}

// Follow-slot enter (0x801A4F50): parks the special block in mode 2, resets its
// timer, enables the follow condition bit and notifies the owner.
int GeistA28A4FollowSlotEnter(int actor)
{
    u8* state = *(u8**)(actor + 0x130);

    state[0x14] = 2;
    *(float*)(state + 0xc) = lbl_804D8718;
    ConditionUpdateBit((void*)(actor + 0x270), 0xe, 1);
    fn_800BBCA8(actor);
    *(u32*)(actor + 0x288) = 0;
    return 1;
}

// Follow-offset state update (0x801A4FB0): refreshes the follow target from the
// actor's active mode slot, then keeps the retire target and pending step in
// sync with the follow-offset state block.
void FollowOffsetUpdate(void* p);
extern "C" void fn_800C748C(int actor);

int GeistA28A4FollowOffsetStateUpdate(int actor)
{
    bool updated;
    int mode;
    int pending;
    int obj;

    if (actor == 0)
        return 0;

    mode = fn_800B26B8(actor);
    if (mode < 4 && (fn_800B1ED4(actor, mode) & 0x40) == 0) {
        pending = fn_800B1EF4(actor, mode);
        fn_800B1F20(actor, mode);
        if (fn_800B261C(actor, mode, &obj) != 0) {
            FollowSetTargetObj((void*)(actor + 0x270), (void*)obj);
        } else {
            float pos[3];

            OwnerSlotGetPosition(actor, mode, pos);
            FollowSetTargetVec((void*)(actor + 0x270), pos);
        }
        if (pending == 0) {
            ConditionUpdateBit((void*)(actor + 0x270), 3, 1);
            updated = true;
            goto done;
        }
    }
    updated = false;
done:
    if (updated)
        return 1;
    if (*(int*)(actor + 0x220) == 0 && MemFlagIsSet((void*)(actor + 0x270), 5) == 0)
        return 1;
    if (*(int*)(actor + 0x220) == 2) {
        *(int*)(actor + 0x10c) = (int)&fn_800C748C;
        if (FollowGetTargetObj((void*)(actor + 0x270)) != *(int*)(actor + 0x22c)) {
            FollowSetTargetObj((void*)(actor + 0x270), *(void**)(actor + 0x22c));
            if (*(int*)(actor + 0x21c) != 0) {
                *(int*)(actor + 0x288) = *(int*)(actor + 0x21c);
                fn_800BC56C(actor, *(int*)(actor + 0x22c));
            }
        }
        if (fn_800BAF40(actor) == -1 && *(int*)(actor + 0x21c) != 0) {
            FollowSetTargetObj((void*)(actor + 0x270), 0);
            *(int*)(actor + 0x288) = 0;
        }
    }
    return 0;
}

// Follow-offset state leave (0x801A515C): restores the follow-offset update
// callback and clears the state's pending retire mode and condition bit.
int GeistA28A4FollowOffsetStateLeave(int actor)
{
    if (actor == 0)
        return 0;

    *(int*)(actor + 0x10c) = (int)&FollowOffsetUpdate;
    if (*(int*)(actor + 0x220) == 4)
        *(int*)(actor + 0x220) = 0;
    if (HasCondition((void*)(actor + 0x270), 0xd) != 0)
        ConditionSetBit((void*)(actor + 0x270), 0xd, 0);
    return 1;
}

// Enter handler of behaviour state 1: marks the state block and reports
// success. A null owner is rejected.
int GeistA28A4State1Enter(void* p)
{
    if (p == 0)
        return 0;
    u8* state = *(u8**)((char*)p + 0x130);
    state[0x14] = 1;
    return 1;
}

// Update/query handler of behaviour state 1: always reports success.
int GeistA28A4State1Check(void)
{
    return 1;
}

// Slot-target state update (0x801A5204): while the state timer is past the
// start threshold it refreshes the follow target from the actor's active mode
// slot, then reports whether the follow owner or a pending retire mode is set.
int GeistA28A4SlotTargetCheck(int actor)
{
    float* state = *(float**)(actor + 0x130);

    if (!(lbl_804D8770 <= state[0]))
        return 0;

    state[3] = state[3] - *(float*)(actor + 0x138);
    if (HasCondition((void*)(actor + 0x270), 0x10) != 0)
        return 0;

    {
        int mode = fn_800B26B8(actor);

        if (mode < 4 && (fn_800B1ED4(actor, mode) & 0x40) == 0) {
            int pending = fn_800B1EF4(actor, mode);
            int obj;

            fn_800B1F20(actor, mode);
            if (fn_800B261C(actor, mode, &obj) != 0) {
                FollowSetTargetObj((void*)(actor + 0x270), (void*)obj);
            } else {
                float pos[3];

                OwnerSlotGetPosition(actor, mode, pos);
                FollowSetTargetVec((void*)(actor + 0x270), pos);
            }
            if (pending == 0)
                ConditionUpdateBit((void*)(actor + 0x270), 3, 1);
        }
    }

    if (ConditionGetOwner((void*)(actor + 0x270)) != 0)
        return 1;
    return *(u32*)(actor + 0x220) != 0;
}

// Enter handler of behaviour state 0 (0x801A534C): clears the state index,
// wakes the actor and clears the pending fetch step.
int GeistA28A4State0Enter(int actor)
{
    *(u8*)(*(int*)(actor + 0x130) + 0x14) = 0;
    fn_800BBCA8(actor);
    *(int*)(actor + 0x288) = 0;
    return 1;
}

// ---- Fetch aim helpers ----------------------------------------------------

struct Actor;

void ActorAim(int* out, Actor* actor, int mode);

extern "C" float fn_800E1020(float* v);
extern "C" void fn_800E1BD8(int a, int b, int c, int d, int e);

extern float lbl_804D878C;
extern float lbl_804D8790;
extern float lbl_804D8794;
extern float lbl_804D8798;
extern float lbl_804D879C;

// Reply block of the 0x411 "aim at target" query. The message system keeps
// the query token at +0x4 and writes the target direction at +0x34.
struct GeistA28A4AimReply {
    int unk0;                        // 0x0
    int token;                       // 0x4
    int unk8[11];                    // 0x8
    int x;                           // 0x34
    int y;                           // 0x38
    int z;                           // 0x3c
};

// Number of components farther from zero than the epsilon; used as the
// "is this vector usable" test before normalising.
static inline int GeistA28A4VecNonZero(float x, float y, float z)
{
    return (fabsf(x) > lbl_804D8750) + (fabsf(y) > lbl_804D8750) + (fabsf(z) > lbl_804D8750);
}

// Normalise in place, or collapse to the zero vector when every component
// lies within the epsilon band.
static inline void GeistA28A4VecNormalize(float* v)
{
    if (GeistA28A4VecNonZero(v[0], v[1], v[2])) {
        float d = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        float inv = __frsqrte(d);
        float len = __fres(inv);
        (void)len;
        v[0] *= inv;
        v[1] *= inv;
        v[2] *= inv;
    } else {
        v[0] = lbl_804D8710;
        v[1] = lbl_804D8710;
        v[2] = lbl_804D8710;
    }
}

// Fetch behaviour +0xfc callback (0x801A5390): samples the target direction
// twice through the 0x411 query and returns the normalised progress towards
// it, writing the filtered aim vector to dir.
float GeistA28A4FetchAimProgress(int actor, float* dir)
{
    int* state = (int*)*(int*)(actor + 0x130);
    int probe[8];
    if (SendMsg(state[4], 0x400, probe) == 0 || probe[0] == 0)
        return lbl_804D8710;

    GeistA28A4AimReply reply;
    reply.token = lbl_804D3090;
    SendMsg(*(int*)(actor + 0x38), 0x411, &reply);
    float px = (float)reply.x;
    float py = (float)reply.y;
    float pz = (float)reply.z;

    reply.token = lbl_804D3094;
    SendMsg(*(int*)(actor + 0x38), 0x411, &reply);
    float delta[3];
    delta[0] = (float)reply.x - px;
    delta[1] = (float)reply.y - py;
    delta[2] = (float)reply.z - pz;

    float len = fn_800E1020(delta);
    if (len <= lbl_804D876C)
        return lbl_804D8710;

    float t = (len - lbl_804D876C) / lbl_804D878C;
    if (t < lbl_804D8710)
        t = lbl_804D8710;
    else if (t > lbl_804D8714)
        t = lbl_804D8714;

    int target = probe[0];
    if (*(int*)(actor + 0x288) != 0 && *(float*)(actor + 0x2a8) > lbl_804D8710) {
        float scale = lbl_804D8790 * t + lbl_804D8714;
        float dist = fn_800E1020((float*)(actor + 0x240));
        int amount = (int)(scale * (dist / *(float*)(actor + 0x2a8)));
        fn_800E1BD8(*(int*)(target + 0x58c), 3, 200, amount, 0x28);
    }

    dir[0] = delta[0];
    dir[1] = delta[1];
    dir[2] = delta[2];
    dir[2] = lbl_804D8710;
    fn_800E1098(dir, dir, lbl_804D8794 * (*(float*)(actor + 0x2ac) * *(float*)(actor + 0x138)));
    return lbl_804D879C * t + lbl_804D8798;
}

// Fetch "aim check" callback (0x801A5610): true when the actor's aim vector
// points at the object reported by the 0x400 query.
int GeistA28A4FetchAimCheck(int targetId, int actor)
{
    int query[8];
    memset(query, 0, 0x20);
    if (SendMsg(targetId, 0x400, query) == 0 || query[0] == 0)
        return 0;

    int aim[3];
    ActorAim(aim, (Actor*)actor, 1);
    float aimDir[3];
    aimDir[0] = (float)aim[0];
    aimDir[1] = (float)aim[1];
    aimDir[2] = (float)aim[2];
    GeistA28A4VecNormalize(aimDir);

    int* obj = (int*)query[0];
    float toTarget[3];
    toTarget[0] = (float)obj[9] - *(float*)(actor + 0x84);
    toTarget[1] = (float)obj[10] - *(float*)(actor + 0x88);
    toTarget[2] = (float)obj[11] - *(float*)(actor + 0x8c);
    GeistA28A4VecNormalize(toTarget);

    return aimDir[0] * toTarget[0] + aimDir[1] * toTarget[1] + aimDir[2] * toTarget[2]
           >= lbl_804D8710;
}

// ---- Debug spline message handler -----------------------------------------
extern float lbl_804D87A0;
extern float lbl_804D87A4;
extern float lbl_804D87A8;
extern float lbl_804D87AC;
extern float lbl_804D87B0;
extern float lbl_804D87B4;
extern float lbl_804D87B8;
extern float lbl_804D87BC;
extern float lbl_804D87C0;
extern float lbl_804D87C4;
extern float lbl_804D87C8;

struct Vec3 {
    float x;                         // 0x0
    float y;                         // 0x4
    float z;                         // 0x8
};

void SplineHermiteEval(float t, Vec3* a, Vec3* b, Vec3* c, Vec3* d, Vec3* out);
void Draw3DBeginLine(int mode, void* arg, float v);
void DrawLine(float* from, float* to, u8, u8, u8, u8, u8, u8, u8, u8);
void DrawDisableTexOffsets(void);
void fn_8016D440(float* out, Vec3* v);
void fn_8016D4EC(Vec3* v, float* s);
float OwnerValue(Owner* owner, Vec3* v);

// Debug/interaction sub-message handler (0x801A5934), dispatched by the unit's
// 0xC14 message handler. 0x21C664 re-queries the fetch target and drops the
// approach flag; 0xF6C38C61 draws the current fetch spline as 32 segments.
int GeistA28A4SplineDebugMsg(int* p)
{
    int actor = p[0];
    int* state = *(int**)(actor + 0x130);

    if (p[3] == 0x21C664) {
        int buf[8];

        memset(buf, 0, 0x20);
        if (SendMsg(p[2], 0x400, buf) != 0 && buf[0] != 0) {
            *(float*)(state + 2) = lbl_804D8710;
            *(u8*)((char*)state + 0x15) |= 0x10;
            *(u8*)((char*)state + 0x15) &= 0x9F;
        }
        return 1;
    }
    if (p[3] != (int)0xF6C38C61)
        return 0;

    {
        GeistA28A4AimReply ra;
        GeistA28A4AimReply rb;
        Vec3 pa;
        Vec3 pb;
        Vec3 delta;
        Vec3 dir;
        Vec3 back;
        float len;
        float t;
        float scale;

        ra.token = lbl_804D3090;
        SendMsg(*(int*)(actor + 0x38), 0x411, &ra);
        pa.x = (float)ra.x;
        pa.y = (float)ra.y;
        pa.z = (float)ra.z;

        rb.token = lbl_804D3094;
        SendMsg(*(int*)(state + 4), 0x411, &rb);
        pb.x = (float)rb.x;
        pb.y = (float)rb.y;
        pb.z = (float)rb.z;

        delta.x = pa.x - pb.x;
        delta.y = pa.y - pb.y;
        delta.z = pa.z - pb.z;

        dir.x = delta.x;
        dir.y = delta.y;
        dir.z = delta.z;
        back.x = -delta.x;
        back.y = -delta.y;
        back.z = -delta.z;

        len = fn_800E1020(&delta.x);
        t = (len - lbl_804D8770) / lbl_804D87A0;
        if (t < lbl_804D8710)
            t = lbl_804D8710;
        else if (t > lbl_804D8714)
            t = lbl_804D8714;

        fn_8016D440(&scale, &dir);
        scale = scale + (lbl_804D87A8 * t + lbl_804D87A4);
        if (scale < lbl_804D87A4)
            scale = lbl_804D87A4;
        else if (scale > lbl_804D87AC)
            scale = lbl_804D87AC;
        fn_8016D4EC(&dir, &scale);

        fn_8016D440(&scale, &back);
        scale = scale - (lbl_804D87B4 * t + lbl_804D87B0);
        if (scale < lbl_804D87A4)
            scale = lbl_804D87A4;
        else if (scale > lbl_804D87AC)
            scale = lbl_804D87AC;
        fn_8016D4EC(&back, &scale);

        fn_800E1098(&dir.x, &dir.x, lbl_804D87B8 * t + lbl_804D876C);
        fn_800E1098(&back.x, &back.x, lbl_804D87BC * t + lbl_804D8770);

        {
            int i;

            for (i = 0; i < 0x20; i++) {
                Vec3 p0;
                Vec3 p1;
                float v;

                SplineHermiteEval(((float)i - lbl_804D8794) * lbl_804D87C4,
                                  &pb, &dir, &pa, &back, &p0);
                SplineHermiteEval((float)(i + 1) * lbl_804D87C4,
                                  &pb, &dir, &pa, &back, &p1);
                v = lbl_804D87C0 * ((float)i * lbl_804D87C4) + lbl_804D87B4;
                Draw3DBeginLine(-3, 0, v);
                DrawLine(&p0.x, &p1.x, 0x64, 0x14, 0x14, 0xFF, 0x64, 0x14, 0x14, 0xFF);
                DrawDisableTexOffsets();
                Draw3DBeginLine(-3, 0, lbl_804D87C8 * v);
                DrawLine(&p0.x, &p1.x, 0x14, 0x14, 0x14, 0xFF, 0x14, 0x14, 0x14, 0xFF);
                DrawDisableTexOffsets();
            }
        }
        return 1;
    }
}

// Dog-fetch state picker (0x801A5E00): returns the next sub-state of the
// dog-fetch behaviour from the current condition-owner distance.
u32 GeistA28A4DogFetchPickState(int actor)
{
    u8* state;
    int owner;
    float value;

    if (actor == 0)
        return 0;
    state = *(u8**)(actor + 0x130);
    if (state == 0)
        return 0;

    if (state[0x15] & 2) {
        state[0x15] &= ~2;
        return 5;
    }
    if (*(int*)(state + 0x10) != -1)
        return 10;
    if (MemFlagIsSet((void*)(actor + 0x270), 5) != 0)
        return 1;
    if (MemFlagIsSet((void*)(actor + 0x270), 0) != 0)
        return state[0x14];

    {
        int mode = fn_800B26B8(actor);

        if (mode != 4 && (fn_800B1ED4(actor, mode) & 0x40) != 0)
            mode = 4;
        if (*(int*)(actor + 0x220) != 0 && mode == 4)
            return 1;
    }

    owner = ConditionGetOwner((void*)(actor + 0x270));
    if (owner == 1) {
        float pos[3];

        FollowGetTargetPos((void*)(actor + 0x270), pos);
        value = OwnerValue((Owner*)actor, (Vec3*)pos);
        if (value < lbl_804D8774)
            return 6;
        if (value < lbl_804D8720)
            return 5;
        if (value < lbl_804D877C)
            return 4 - (state[0x16] == 0);
        if (value < lbl_804D871C)
            return 2;
        return 0;
    }
    if (owner < 1 || owner > 2)
        return 0;

    {
        int target = FollowGetTargetObj((void*)(actor + 0x270));

        value = OwnerValue((Owner*)actor, (void*)target);
        if (state[0x16] == 1) {
            if (state[0x14] == 9)
                return 0;
            if (value < (float)*(u32*)(*(int*)(actor + 0xc4) + 0xcc))
                return 7;
            if (value < (float)*(u32*)(*(int*)(actor + 0xc4) + 0xc8))
                return 9;
            return 8;
        }
        if (value < lbl_804D8774)
            return 6;
        if (value < lbl_804D8720)
            return 5;
        if (value < lbl_804D877C)
            return 3;
        if (value < lbl_804D871C)
            return 2;
        return 0;
    }
}

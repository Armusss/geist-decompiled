// u_80285ACC.cpp: the "DOG MELEE" effect type (0x80285ACC-0x80286580) and the ARAM VM page
// allocator / stream heap helpers (0x80286638-0x80286AE8).
// The effect locks a dog onto a target: it spawns the two definition effects and a third one while
// the dog charges, and answers queries about its state. The helpers below it scan the virtual page
// table looking for a free page and parcel out the streaming heap.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

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

struct EffectObj {
    int unk0;
    int unk4;
    int emitter;
    void* data;
};

struct EffectType {
    const char* name;
    u32 hash;
    int unk8[3];
    void* (*create)(int a, int id);
    void (*destroy)(EffectObj* obj);
    int (*msg)(EffectObj* obj, int msg, int arg, void* data, float dt);
    u32 (*query)(EffectObj* obj, int msg);
    void (*update)(EffectObj* obj, float dt);
    void (*reset)(EffectObj* obj);
    int unk2C;
};

inline float Min(float a, float b) { return a < b ? a : b; }
inline float Max(float a, float b) { return a > b ? a : b; }

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        const char* name = type->name;
        type->hash = StrHash(name);
    }
    return type->hash;
}

struct DogDef {
    u8 pad0[0x30];
    int fx1;            // 0x30
    int fx2;            // 0x34
    u8 pad38[0x58];
    int fx3;            // 0x90
};

struct DogMsgBlock {
    u8 pad0[0x30];
    float x, y, z;      // 0x48, 0x4C, 0x50
    u8 pad3C[0x24];
};

struct DogBeh {
    int state;          // 0x00
    float unk4;         // 0x04
    DogDef* def;        // 0x08
    int fx1;            // 0x0C
    int fx2;            // 0x10
    int arg;            // 0x14
    DogMsgBlock msg;    // 0x18
    float timer;        // 0x78
    int fx3;            // 0x7C
};

struct TargetPos {
    u8 pad0[0x24];
    int x, y, z;        // 0x24, 0x28, 0x2C
};

struct TargetMsg {
    void* target;
    int arg;
    int unk08[7];
};

inline void TargetToPos(float* pos, TargetPos* t) {
    pos[0] = (float)t->x;
    pos[1] = (float)t->y;
    pos[2] = (float)t->z;
}

struct Actor {
    u8 pad0[0x184];
    int unk184;
};

extern "C" {
int SendMsg(int target, int msg, void* data);                       // 0x800265D4
void DCInvalidateRange(void* addr, u32 len);
u32 OSGetTick(void);
void VMAllocPages(void* addr, u32 size);

void* fn_8016F120(int fx, int* pos);
void fn_8016F118(int handle, int a, int b);
void* fn_8016F25C(int fx, void* pos, void (*cb)(int*), int* handle);
void fn_8016EDD8(int handle, int* pos);
void fn_8016EEF4(int handle);
void* FxStart(int fx, int a, int b);
void fn_800B4C50(float* pos, int arg, float a, float b, int c, int d);
void fn_80122108(void* target, int arg, float* pos, u32 id, float a, float b, float c, float d,
                 float e, float f, float g, int h);
void* fn_800EB21C(int id);
int fn_800F94C8(int a);
int fn_800C64E4(void);

void* fn_803407D8(u32 page);
int fn_80340604(void);
int fn_8034062C(void* slot);
int fn_80340654(void* slot);
void fn_8034070C(void* slot, int arg);
u32 fn_802872F4(void);

// VM/stream-heap state shared with VM.c (.sdata/.sbss).
int lbl_804D3790 = 1;
int lbl_804D3794 = 1;
int lbl_804D5648;
s8 lbl_804D564C;
u32 lbl_804D5650;
extern u8* lbl_804D565C;
extern u32 lbl_804D5664;
extern u8* lbl_804D5678;
extern u8* lbl_804D567C;
extern u8* lbl_804D5680;
extern u32 lbl_804D5684;
extern u32 lbl_804D5688;
extern u32 lbl_804D568C;
extern s16* lbl_804D5690;
}

int Fn800816B0(void);
void* VAlloc(int size);
void VFree(void* p);
int Fn80175D60(u32 arg);
void PlayerGetPossessor(int arg);
void ActorPlayAnim(int actor, u32 anim, int arg);
Actor* GetPlayer(void);

extern EffectType gDogMeleeType;

int VMPageRef(u32 page);
void DogMeleeFxNotify(int* handle);

void DogMeleeReset(EffectObj* obj) {
    DogBeh* b = (DogBeh*)obj->data;

    if (b->fx1 != -1) {
        fn_8016EEF4(b->fx1);
        b->fx1 = -1;
    }
    if (b->fx2 != -1) {
        fn_8016EEF4(b->fx2);
        b->fx2 = -1;
    }
    b->state = 0;
    b->unk4 = 0.0f;
}

void DogMeleeUpdate(EffectObj* obj, float dt) {
    DogBeh* b = (DogBeh*)obj->data;
    float pos[3];
    TargetMsg q;
    TargetPos* t;
    float pct;

    if (b == 0) {
        return;
    }
    switch (b->state) {
    case 1:
        if (b->timer < 1500.0f) {
            b->timer += dt;
            b->timer = Min(b->timer, 1500.0f);
        }
        break;
    case 2: {
        q.arg = b->arg;
        q.target = 0;
        SendMsg(b->arg, 0x400, &q);
        t = (TargetPos*)q.target;
        TargetToPos(pos, t);
        {
            float rate = b->timer / 1500.0f;
            pct = (rate > 0.0f) ? rate : 0.0f;
        }
        fn_80122108(q.target, q.arg, pos, 0xB3082735, 1.0f, 1000.0f, 1000.0f * pct,
                    1000.0f * pct, 0.75f, 0.4f, 1.0f, q.arg);
    } break;
    case 0:
    case 3:
    default:
        break;
    }
}

u32 DogMeleeQuery(EffectObj* obj, int msg) {
    DogBeh* b = (DogBeh*)obj->data;

    switch (msg) {
    case 0xF829DB93:
    case 0x42E50A3D:
        if (b->state != 0) {
            return 0x3DA260F7;
        }
        return 0x04A3E183;
    case 0x9791AF51:
        if (b->state == 1) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
    case 0x0020EE36:
        if (b->state == 0 || b->state == 4 || b->state == 3) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
    case 0x953D50F2:
        if (b->state == 0 || b->state == 4 || b->state == 3) {
            return 0x04A3E183;
        }
        return 0x3DA260F7;
    case 0x6FAB23FE:
        return (int)b->timer;
    case 0x968CBE3D:
    case 0x96224FCC:
    case 0xE6B71BCC:
    case 0x0D19E338:
        return 0x3DA260F7;
    }
    return 0x3DA260F7;
}

int DogMeleeMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    DogBeh* b = (DogBeh*)obj->data;
    DogDef* def;
    int r;
    int pos1[3];
    int pos2[3];
    int pos3[3];
    int pos4[3];
    int pos5[3];

    def = b->def;
    b->arg = arg;
    r = Fn80175D60(b->arg);

    switch (msg) {
    case 0x1B83833F:
        b->state = 0;
        return 1;
    case 0x96224FCC: {
        float* m;
        int fx;
        int id;

        if (b->fx1 != -1) {
            fn_8016EEF4(b->fx1);
            b->fx1 = -1;
        }
        m = &b->msg.x;
        id = def->fx1;
        if (m) {
            pos5[0] = (int)b->msg.x;
            pos5[1] = (int)b->msg.y;
            pos5[2] = (int)b->msg.z;
            fx = (int)fn_8016F120(id, pos5);
            fn_8016F118(fx, 0, 100000);
        } else {
            fx = (int)FxStart(id, 0, 0);
        }
        b->fx1 = fx;
        b->state = 3;
    }
        return 1;
    case 0x0020EE36:
        if (data) {
            memcpy(&b->msg, data, 0x60);
        }
        return 1;
    case 0x315A1A7F:
        if (b->state == 3) {
            if (b->fx1 != -1) {
                pos1[0] = (int)b->msg.x;
                pos1[1] = (int)b->msg.y;
                pos1[2] = (int)b->msg.z;
                fn_8016EDD8(b->fx1, pos1);
            }
            fn_800B4C50(&b->msg.x, b->arg, 101.0f, 8000.0f, 3, 0x12);
        } else if (b->state == 0 && b->fx1 != -1) {
            fn_8016EEF4(b->fx1);
            b->fx1 = -1;
        }
        return 1;
    case 0x0D19E338:
        fn_8016EEF4(b->fx1);
        b->fx1 = -1;
        b->state = 0;
        return 1;
    case 0x4E424290: {
        float* m;
        int id;

        if (b->fx2 != -1) {
            fn_8016EEF4(b->fx2);
            b->fx2 = -1;
        }
        m = &b->msg.x;
        id = def->fx2;
        if (m) {
            b->fx2 = (int)fn_8016F25C(id, &b->msg.x, DogMeleeFxNotify, &b->fx2);
            fn_8016F118(b->fx2, 0, 100000);
        } else {
            b->fx2 = (int)FxStart(id, 0, 0);
        }
        b->state = 4;
    }
        return 1;
    case 0x953D50F2:
        if (data) {
            memcpy(&b->msg, data, 0x60);
        }
        return 1;
    case 0x7D38803B:
        if (b->state == 4) {
            if (b->fx2 != -1) {
                pos2[0] = (int)b->msg.x;
                pos2[1] = (int)b->msg.y;
                pos2[2] = (int)b->msg.z;
                fn_8016EDD8(b->fx2, pos2);
            }
            ActorPlayAnim(b->arg, 0x8B390117, b->arg);
            fn_800B4C50(&b->msg.x, b->arg, 101.0f, 8000.0f, 2, 0x12);
        } else if (b->state == 0 && b->fx2 != -1) {
            fn_8016EEF4(b->fx2);
            b->fx2 = -1;
        }
        return 1;
    case 0x238E87F4:
        fn_8016EEF4(b->fx2);
        b->fx2 = -1;
        b->state = 0;
        return 1;
    case 0x42E50A3D:
        if (b->state != 1) {
            b->state = 1;
            b->timer = 0.0f;
        }
        {
            pos3[0] = (int)b->msg.x;
            pos3[1] = (int)b->msg.y;
            pos3[2] = (int)b->msg.z;
            b->fx3 = (int)fn_8016F120(def->fx3, pos3);
        }
        return 1;
    case 0x24725EAA:
        if (data) {
            memcpy(&b->msg, data, 0x60);
        }
        if (b->fx3 != -1) {
            pos4[0] = (int)b->msg.x;
            pos4[1] = (int)b->msg.y;
            pos4[2] = (int)b->msg.z;
            fn_8016EDD8(b->fx3, pos4);
        }
        return 1;
    case 0xF829DB93:
        b->state = 0;
        b->timer = 0.0f;
        return 1;
    case 0x9791AF51:
        fn_8016EEF4(b->fx3);
        b->fx3 = -1;
        b->state = 2;
        return 1;
    case 0x0B59E861:
        if (r == 0) {
            PlayerGetPossessor(arg);
        }
        return 1;
    case 0x141109BD:
    case 0x8FDD7599:
    case 0x968CBE3D:
    case 0xBC78F550:
    case 0xE452286F:
    case 0xE6B71BCC:
        return 1;
    }
    return 0;
}

void DogMeleeDestroy(EffectObj* obj) { VFree(obj->data); }

void* DogMeleeCreate(int a, int id) {
    EffectType* type = &gDogMeleeType;
    EffectTypeHash(type);
    DogBeh* b = (DogBeh*)VAlloc(0x80);
    memset(b, 0, 0x80);
    if (b == 0) {
        return 0;
    }
    b->def = (DogDef*)fn_800EB21C(id);
    b->fx1 = -1;
    b->fx2 = -1;
    b->timer = 0.0f;
    b->fx3 = -1;
    b->state = 0;
    b->unk4 = 0.0f;
    return b;
}

void DogMeleeFxNotify(int* handle) {
    int h;

    if (lbl_804D564C == 0) {
        lbl_804D5648 = 0;
        lbl_804D564C = 1;
    }
    h = *handle;
    if (h != -1 && lbl_804D5648 == 0) {
        lbl_804D5648 = 1;
        if (Fn800816B0() > -1 || fn_800C64E4() != fn_800F94C8(GetPlayer()->unk184)) {
            fn_8016EEF4(h);
            *handle = -1;
        }
    } else {
        lbl_804D5648 = 0;
    }
}

u32 VMAllocPage(void) {
    u32 start = lbl_804D5650;
    u32 result = 0;
    int candA = -1;
    int candB = -1;
    int candC = -1;

    if (lbl_804D3790 == 0) {
        for (;;) {
            void* e = fn_803407D8(lbl_804D5650);
            int a;
            int c;

            if (e == 0 || fn_80340604() == 0) {
                break;
            }
            a = fn_8034062C(e);
            c = fn_80340654(e);
            if (a == 0 && c == 0 && VMPageRef(lbl_804D5650) == 0) {
                result = lbl_804D5650;
                goto done;
            }
            if (a == 0 && c != 0) {
                if (candA < 0 && VMPageRef(lbl_804D5650) == 0) {
                    candA = lbl_804D5650;
                }
            } else if (a != 0 && c == 0) {
                if (candB < 0 && VMPageRef(lbl_804D5650) == 0) {
                    candB = lbl_804D5650;
                }
            } else {
                if (candC < 0 && VMPageRef(lbl_804D5650) == 0) {
                    candC = lbl_804D5650;
                }
            }
            if (a != 0) {
                fn_8034070C(e, 0);
            }
            if (start == lbl_804D5650) {
                if (candA >= 0) {
                    result = candA;
                } else if (candB >= 0) {
                    result = candB;
                } else if (candC >= 0) {
                    result = candC;
                }
                goto done;
            }
            lbl_804D5650++;
            if (lbl_804D5650 >= fn_802872F4()) {
                lbl_804D5650 = 0;
            }
        }
        result = lbl_804D5650;
    } else {
        result = start;
    }
done:
    lbl_804D5650++;
    if (lbl_804D5650 >= fn_802872F4()) {
        lbl_804D3790 = 0;
        lbl_804D5650 = 0;
    }
    return result;
}

u32 VMAllocPageAny(void) {
    u32 page;

    if (lbl_804D3794 == 0) {
        return VMAllocPage();
    }
    if (lbl_804D3794 == 1) {
        for (;;) {
            if (lbl_804D3790 != 0) {
                page = lbl_804D5650;
                lbl_804D5650++;
                if (lbl_804D5650 >= fn_802872F4()) {
                    lbl_804D3790 = 0;
                    lbl_804D5650 = 0;
                }
            } else {
                u32 n = fn_802872F4();
                page = OSGetTick() % n;
            }
            if (VMPageRef(page) == 0) {
                return page;
            }
        }
    }
    for (;;) {
        page = lbl_804D5650;
        lbl_804D5650++;
        if (lbl_804D5650 >= fn_802872F4()) {
            lbl_804D3790 = 0;
            lbl_804D5650 = 0;
        }
        if (VMPageRef(page) == 0) {
            return page;
        }
    }
}

void CacheNotify(void* addr, int op) {}

void StreamHeapFlush(void) {
    u32 i;

    for (i = 0; i < lbl_804D5664; i++) {
        if (lbl_804D5690[i] == -1) {
            DCInvalidateRange(lbl_804D565C + i * 0x1000, 0x1000);
            lbl_804D5690[i] = 0;
        }
    }
}

static inline void VMResetRefs() {
    u32 end = ((u32)(lbl_804D5680 - lbl_804D5678) + 0xFFF) >> 12;
    u32 i;

    for (i = end; i < lbl_804D5664; i++) {
        lbl_804D5690[i] = 0;
    }
    lbl_804D568C = lbl_804D5664 - end;
    lbl_804D5688 = ((lbl_804D5664 - end) << 12) - 0x20000;
}

extern "C" void fn_80286980(void) {
    lbl_804D5684 = 0;
    lbl_804D5680 = lbl_804D5678;
    VMResetRefs();
}

extern "C" void* fn_802869DC(u32 size) {
    void* p = lbl_804D5680;

    VMAllocPages(p, size);
    lbl_804D5680 += size;
    lbl_804D5684 += size;
    lbl_804D5688 -= size;
    return p;
}

u32 StreamHeapFreeSize(void) { return lbl_804D568C << 12; }

extern "C" int VMIsVirtualAddr(void* p) {
    if (p >= (void*)lbl_804D5678 && p < (void*)lbl_804D567C) {
        return 1;
    }
    return 0;
}

int VMPageRef(u32 page) { return lbl_804D5690[page]; }

void StreamHeapRelease(void* p) {
    u32 end = ((u32)(lbl_804D5680 - lbl_804D5678) + 0xFFF) >> 12;
    u32 i;

    for (i = end; i < lbl_804D5664; i++) {
        lbl_804D5690[i] = 0;
    }
    lbl_804D568C = lbl_804D5664 - end;
    lbl_804D5688 = ((lbl_804D5664 - end) << 12) - 0x20000;
}

u32 StreamHeapSize(void) { return lbl_804D5688; }

u8* StreamHeapBase(void) { return lbl_804D5680; }

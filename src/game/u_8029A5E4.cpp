// u_8029A5E4.cpp (0x8029A5E4-0x8029B070), split from the old u_80294548 range: two descriptor-driven actor
// extras (descriptors 0x803C30E0 and 0x803C3104 in .data, hashes 0xC5733545 / 0xFE16F9C0) and the 12-byte
// element array class (vtable 0x803C3128) the first one keeps at holder +0xF0.
// Boundary evidence:
// - .sdata2: own pool 0x804DA728-0x804DA760 (0.0f at 0x804DA730 duplicates the previous file's 0x804DA720 and
//   the next file's 0x804DA760; its own int-to-float doubles 0x804DA740/0x804DA758).
// - .data 0x803C30E0-0x803C3148: the first descriptor lists functions from both halves of the range
//   (fn_8029AE58, fn_8029A914, fn_8029ADC4, fn_8029AC60), so fn_8029A5E4..fn_8029B04C is one file; the
//   array vtable 0x803C3128 holds fn_8029AEF0, fn_8029B020, fn_8029AF58, fn_8029AFB0. The next file's code
//   uses only the vtables from 0x803C3148 on.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include "game/types.h"

void* VAlloc(int size);
void VFree(void* p);
extern "C" void fn_8028B960(void* buffer);
extern "C" void fn_8028B2D4(void* slot);

// The Buffer class chain of u_8028A8D4.cpp (ScreenEffectSlot -> ScreenEffectBuffer -> Buffer; base names are
// guesses there). The array below overrides the element hooks and calls the base ones directly.
class ScreenEffectSlot {
public:
    virtual ~ScreenEffectSlot() {}
    virtual void Clear();
    virtual int GetAlignment();
    virtual void Commit(int start, unsigned long size) = 0;

    char* mStart; // 0x04
    int mSize;    // 0x08
    char* mStore; // 0x0C
    u32 mState;   // 0x10
};

class ScreenEffectBuffer : public ScreenEffectSlot {
public:
    virtual void Commit(int start, unsigned long size);
    virtual void Append(unsigned char* data, unsigned long size);
    virtual int GetSpan() = 0;
};

class Buffer : public ScreenEffectBuffer {
public:
    virtual ~Buffer();
    Buffer();
    virtual void Clear();
    virtual int GetSpan() { return 0; }
};

extern "C" char* fn_8028B3BC(Buffer* self, char* pos, int size);

// Construct / destroy hooks for a range of 12-byte elements (trivial: empty loops).
extern "C" void fn_8029B028(Vec3* first, Vec3* last);
extern "C" void fn_8029B04C(Vec3* first, Vec3* last);

// A growable array of Vec3 (vtable 0x803C3128: destructor, Buffer::Clear, GetAlignment, Commit, Append,
// Buffer::GetSpan). Class name is a guess.
class Vec3Array : public Buffer {
public:
    Vec3Array() {}
    virtual ~Vec3Array() { fn_8028B960(this); }
    virtual int GetAlignment();
    virtual void Commit(int start, unsigned long size);
    virtual void Append(unsigned char* data, unsigned long size);
    static void operator delete(void* p) { VFree(p); }
};

// A projectile (laid out like the rivets of u_801FCA4C.cpp; fields used here only).
struct Projectile {
    u8 pad00[0x68];
    Vec3 pos;           // 0x68
    u8 pad74[0x80 - 0x74];
    Vec3 vel;           // 0x80
    u8 pad8C[0xB4 - 0x8C];
    Vec3 lastPos;       // 0xB4
    u8 padC0[0xC8 - 0xC0];
    int model;          // 0xC8
    u32 owner;          // 0xCC: handle of the shooter
    u8 padD0[0xF0 - 0xD0];
    Vec3Array* trail;   // 0xF0
};

struct ProjHook {
    u8 pad00[0xC];
    int (*hit)(void* type, Projectile* p, void* info);  // 0x0C
};

// The projectile type descriptor passed to the callbacks (fields used here only).
struct ProjType {
    u8 pad00[4];
    int damage;         // 0x04
    int damage2;        // 0x08: used while fn_80049940() is set
    u8 pad0C[0x18 - 0x0C];
    float force;        // 0x18
    int kind;           // 0x1C
    u8 pad20[0x2C - 0x20];
    void* hitFx;        // 0x2C
    u8 pad30[0x44 - 0x30];
    ProjHook* hook;     // 0x44
};

// 0x48-byte hit record filled by the collision query callback (as in u_801FCA4C.cpp).
struct HitBlob {
    void* obj;   // 0x00
    int unk04;
    int unk08;
    int unk0C;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
    int unk20;
    int unk24;
    int unk28;
    int unk2C;
    int pos[3];  // 0x30
    int vec[3];  // 0x3C
};

struct Hit {
    u8 pad00[0x30];
    int (*msg)(int msg, void* data);  // 0x30
    u8 pad34[4];
    u32 handle;  // 0x38
};

// Message 0x407 (projectile hit) sent to the object that was hit.
struct HitMsg407 {
    Hit* target;        // 0x00
    Projectile* from;   // 0x04
    u32 owner;          // 0x08
    float damage;       // 0x0C
    int unk10;
    int kind;           // 0x14
    int pos[3];         // 0x18
    int normal[3];      // 0x24
    int vel[3];         // 0x30
    int unk3C;          // 0x3C
    int unk40;          // 0x40
    int unk44;          // 0x44
    int unk48;          // 0x48
    float force;        // 0x4C
    u8 pad50[0xC];
};

typedef int (*QueryCallback)(void*, void**);
extern "C" int fn_8001F928(void* actor, int* ints, u32 flags, QueryCallback cb, void** ctx);
extern "C" int fn_80183DD0(void* src, void** ctx);
extern "C" void fn_8016F120(void* fx, int* pos);
extern "C" void fn_80184BCC(int* delta, int* in, int* out);
extern "C" int fn_80049940(void);
extern "C" void fn_8028B35C(Buffer* self, char* pos, int size);

// TODO (not drafted yet, 0x2D8 bytes at 0x8029A5E4)
// fn_8029A5E4
//@@8029A8BC
struct Actor;
void ActorGetMatrix(Mtx34* m, Actor* a);
extern "C" void fn_8003F15C(int model, Mtx34* m, int a, int b, int c, float t);
extern "C" void fn_8029A8BC(void* self, Projectile* o);
void fn_8029A8BC(void* self, Projectile* o) {
    if (o->model != -1) {
        Mtx34 m;
        ActorGetMatrix(&m, (Actor*)o);
        fn_8003F15C(o->model, &m, 1, 0, 0, 1.0f);
    }
}
//@@end
//@@8029A914
static inline void Vec3Erase(Vec3Array* a, Vec3* first, Vec3* last) {
    fn_8028B35C(a, (char*)first, (last - first) * sizeof(Vec3));
}

extern "C" int fn_8029A914(void* type, Projectile* p);
int fn_8029A914(void* type, Projectile* p) {
    ProjType* self = (ProjType*)type;
    int ints[6];
    HitBlob hitInfo;
    void* ctx[2];
    int delta[3];
    Vec3 vtmp;
    Vec3Array* a = p->trail;
    Vec3* v;

    if (a == 0) {
        return 0;
    }
    v = (Vec3*)fn_8028B3BC(a, a->mStore + a->mSize, sizeof(Vec3));
    *v = p->pos;
    if (a->mSize / sizeof(Vec3) > 6) {
        Vec3Erase(a, (Vec3*)a->mStore, (Vec3*)a->mStore + 1);
    }
    ctx[0] = &hitInfo;
    ctx[1] = (void*)p->owner;
    ints[0] = (int)p->lastPos.x;
    ints[1] = (int)p->lastPos.y;
    ints[2] = (int)p->lastPos.z;
    ints[3] = (int)p->pos.x;
    ints[4] = (int)p->pos.y;
    ints[5] = (int)p->pos.z;
    if (fn_8001F928(p, ints, 0x12000000, fn_80183DD0, ctx) == 0) {
        return 1;
    }
    if (self->hook != 0 && self->hook->hit != 0) {
        return self->hook->hit(self, p, &hitInfo) == 0;
    }
    delta[0] = ints[3] - ints[0];
    delta[1] = ints[4] - ints[1];
    delta[2] = ints[5] - ints[2];
    fn_80184BCC(delta, hitInfo.vec, (int*)&vtmp);
    if (hitInfo.obj != 0) {
        HitMsg407 m;
        float damage;
        int kind = (int)(((Hit*)hitInfo.obj)->handle >> 24);

        if (kind == 2 || kind == 4) {
            fn_8016F120(self->hitFx, hitInfo.pos);
        }
        if (fn_80049940()) {
            damage = self->damage2;
        } else {
            damage = self->damage;
        }
        m.from = p;
        m.owner = p->owner;
        m.damage = damage;
        m.kind = self->kind;
        m.pos[0] = hitInfo.pos[0];
        m.pos[1] = hitInfo.pos[1];
        m.pos[2] = hitInfo.pos[2];
        m.normal[0] = hitInfo.vec[0];
        m.normal[1] = hitInfo.vec[1];
        m.normal[2] = hitInfo.vec[2];
        m.vel[0] = (int)p->vel.x;
        m.vel[1] = (int)p->vel.y;
        m.vel[2] = (int)p->vel.z;
        m.unk3C = hitInfo.unk10;
        m.unk40 = hitInfo.unk18;
        m.unk44 = hitInfo.unk14;
        m.unk48 = hitInfo.unk0C;
        m.force = self->force;
        m.target = (Hit*)hitInfo.obj;
        m.target->msg(0x407, &m);
    }
    return 0;
}
//@@end
//@@8029AC60
void Draw3DBeginLine(int mode, void* tex, float width);
void DrawLine(float* a, float* b, u8 r0, u8 g0, u8 b0, u8 a0, u8 r1, u8 g1, u8 b1, u8 a1);
void DrawDisableTexOffsets(void);
extern "C" void fn_8029AC60(void* self, Projectile* o);
void fn_8029AC60(void* self, Projectile* o) {
    int off;
    Vec3Array* a = o->trail;
    int i;
    float step;
    if (a == 0 || a->mSize / sizeof(Vec3) < 2) {
        return;
    }
    step = 255.0f / (a->mSize / sizeof(Vec3) - 1);
    Draw3DBeginLine(-3, 0, 2.0f);
    for (i = 0, off = 0; i < a->mSize / sizeof(Vec3) - 1; i++, off += sizeof(Vec3)) {
        DrawLine((float*)(a->mStore + off), (float*)(a->mStore + (i + 1) * sizeof(Vec3)), 255, 255, 255,
                 (u8)(i * step), 255, 255, 255, (u8)((i + 1) * step));
    }
    DrawDisableTexOffsets();
}
//@@end
//@@8029ADC4
extern "C" void fn_8029ADC4(void* self, Projectile* o);
void fn_8029ADC4(void* self, Projectile* o) {
    Vec3Array* a = o->trail;
    if (a == 0) {
        return;
    }
    while (a->mSize / sizeof(Vec3) != 0) {
        fn_8028B2D4(a);
    }
    if (a) {
        a->Vec3Array::~Vec3Array();
        VFree(a);
    }
}
//@@end
//@@8029AE58
extern "C" int fn_8029AE58(void* self, Projectile* o);
int fn_8029AE58(void* self, Projectile* o) {
    Vec3Array* a = new Vec3Array;
    Vec3* v;
    if (a == 0) {
        return 0;
    }
    o->trail = a;
    v = (Vec3*)fn_8028B3BC(a, a->mStore + a->mSize, sizeof(Vec3));
    *v = o->pos;
    return 1;
}
//@@end
// TODO (not drafted yet, 0x68 bytes at 0x8029AEF0)
// fn_8029AEF0
//@@8029AF58
void Vec3Array::Commit(int start, unsigned long size) {
    ScreenEffectBuffer::Commit(start, size);
    fn_8029B028((Vec3*)start, (Vec3*)start + size / sizeof(Vec3));
}
//@@end
//@@8029AFB0
static inline void Vec3Destroy(void* p, u32 bytes) {
    Vec3* first = (Vec3*)p;
    fn_8029B04C(first, first + bytes / sizeof(Vec3));
}

void Vec3Array::Append(unsigned char* data, unsigned long size) {
    Vec3Destroy(data, size);
    ScreenEffectBuffer::Append(data, size);
}
//@@end
//@@8029B020
int Vec3Array::GetAlignment() {
    return sizeof(Vec3);
}
//@@end
//@@8029B028
void fn_8029B028(Vec3* first, Vec3* last) {
    for (; first < last; first++) {
    }
}
//@@end
//@@8029B04C
void fn_8029B04C(Vec3* first, Vec3* last) {
    for (; first < last; first++) {
    }
}
//@@end

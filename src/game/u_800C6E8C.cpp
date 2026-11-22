// u_800C6E8C.cpp (0x800C6E8C-0x800C99E8): actor aim point / aim state helpers and the per-actor animation
// callbacks (.data 0x8039B770, .sdata2 0x804D6F10-0x804D6F30), then actor rendering, collision hooks and the
// actor type registry (.data 0x8039B780, .bss 0x80451000-0x80451780, .sbss 0x804D48D0-0x804D48F8, .sdata
// 0x804D2BA8-0x804D2BB8). Split from the old u_800C5028 range.
// Boundary evidence: start: own .sdata2 pool from 0x804D6F10 (int-to-float double, a duplicate of
// 0x804D6EE8) and ActorPlayAnim called out of line (see u_800C5028.cpp). End: the next file's pool starts
// at 0x804D6F30 with its own 1.0f (this file's 1.0f is 0x804D6F28) and int-to-float double 0x804D6F38.
// Tentative: the render / registry part (0x800C78F0-0x800C99E8) reads no floats and may be its own file;
// no evidence found yet (no out-of-line call to a small earlier function across it).
// Names are descriptive guesses; unmatched against retail unless noted.

typedef unsigned long u32;
typedef signed long s32;
typedef unsigned char u8;

extern "C" {
void* memset(void* dst, int c, unsigned long n);
void qsort(void* base, unsigned long n, unsigned long size, int (*cmp)(const void*, const void*));
void fn_80026660(int msg, void* handler, int arg);
void fn_800933D0(int anim);
void fn_8009337C(int anim);
int fn_80084540(int to, int msg, void* data, void* result);
}
void AnimUnref(int anim);
void AnimRef(int anim);


struct TargetPoint {
    int x;  // 0x24
    int y;  // 0x28
    int z;  // 0x2c
};
struct Actor {
    char pad0[0x84];
    float pos[3];       // 0x84
    char pad90[0x218 - 0x90];
    int unk218;         // 0x218
    int unk21C;         // 0x21c
    int targetMode;     // 0x220
    int unk224;         // 0x224
    int unk228;         // 0x228
    union {
        float point[3]; // 0x22c  mode 1: a point
        TargetPoint* target;  // mode 2: a target object (the point is at +0x24)
    };
    int unk238;         // 0x238
    int unk23C;         // 0x23c
};


// 0x800C6E8C: the point an actor is aiming at; true when it is a real target
int ActorAimPoint(Actor* a, float* out)
{
    if (a->targetMode == 1) {
        out[0] = a->point[0];
        out[1] = a->point[1];
        out[2] = a->point[2];
        return 1;
    } else if (a->targetMode == 2 && a->target != 0) {
        out[0] = (float)a->target->x;
        out[1] = (float)a->target->y;
        out[2] = (float)a->target->z;
        return 1;
    } else {
        out[0] = a->pos[0];
        out[1] = a->pos[1];
        out[2] = a->pos[2];
        return 0;
    }
}

// 0x800C6F58: clears an actor's aim state
extern "C" int fn_800C6F58(Actor* a)
{
    if (a == 0) {
        return 0;
    }
    a->targetMode = 0;
    a->unk238 = -1;
    a->unk23C = -1;
    return 1;
}

struct Mtx34;
void MineSlotsFree();
void MineSlotsClear();
void ActorGetMatrix(Mtx34* m, Actor* a);
extern int lbl_804D48F0;

// TODO (not drafted yet, 0xA8 bytes at 0x800C6F84)
// fn_800C6F84
// TODO (not drafted yet, 0x14C bytes at 0x800C702C)
// fn_800C702C
// TODO (not drafted yet, 0xE4 bytes at 0x800C7178)
// fn_800C7178
// TODO (not drafted yet, 0x230 bytes at 0x800C725C)
// fn_800C725C
// TODO (not drafted yet, 0x464 bytes at 0x800C748C)
// fn_800C748C
//@@800C78F0
extern "C" void fn_800C78F0(void) {
    MineSlotsFree();
}
//@@end
//@@800C7910
extern "C" void fn_800C7910(void) {
    MineSlotsClear();
}
//@@end
// TODO (not drafted yet, 0x1E4 bytes at 0x800C7930)
// fn_800C7930
// TODO (not drafted yet, 0x48 bytes at 0x800C7B14)
// fn_800C7B14
//@@800C7B5C
// Default colour callback: opaque black.
void ActorFn800C7B5C(Actor* a, void* out) {
    u8* c = (u8*)out;
    c[0] = 0;
    c[1] = 0;
    c[2] = 0;
    c[3] = 0xFF;
}
//@@end
// TODO (not drafted yet, 0x170 bytes at 0x800C7B78)
// fn_800C7B78
// TODO (not drafted yet, 0x198 bytes at 0x800C7CE8)
// ActorRender__FP5Actor
// TODO (not drafted yet, 0x318 bytes at 0x800C7E80)
// fn_800C7E80
// TODO (not drafted yet, 0x44 bytes at 0x800C8198)
// fn_800C8198
// TODO (not drafted yet, 0x1B8 bytes at 0x800C81DC)
// fn_800C81DC
// TODO (not drafted yet, 0x5C bytes at 0x800C8394)
// ActorCollide__Fv
// TODO (not drafted yet, 0x4C bytes at 0x800C83F0)
// ActorTypeRegister__FP9ActorType
// TODO (not drafted yet, 0x128 bytes at 0x800C843C)
// fn_800C843C
// TODO (not drafted yet, 0x3E0 bytes at 0x800C8564)
// fn_800C8564
// TODO (not drafted yet, 0x584 bytes at 0x800C8944)
// fn_800C8944
// TODO (not drafted yet, 0xA4 bytes at 0x800C8EC8)
// fn_800C8EC8
// TODO (not drafted yet, 0x4A8 bytes at 0x800C8F6C)
// fn_800C8F6C
// TODO (not drafted yet, 0x6C bytes at 0x800C9414)
// fn_800C9414
// TODO (not drafted yet, 0x64 bytes at 0x800C9480)
// fn_800C9480
// TODO (not drafted yet, 0x308 bytes at 0x800C94E4)
// fn_800C94E4
// TODO (not drafted yet, 0x44 bytes at 0x800C97EC)
// fn_800C97EC
// TODO (not drafted yet, 0x44 bytes at 0x800C9830)
// fn_800C9830
// TODO (not drafted yet, 0x98 bytes at 0x800C9874)
// fn_800C9874
//@@800C990C
extern "C" int fn_800C9968(int msg, int a, int* data);

// Registers the message 0x15 handler and clears the state word.
extern "C" void fn_800C990C(void) {
    fn_80026660(0x15, (void*)fn_800C9968, 0);
    lbl_804D48F0 = 0;
}
//@@end
//@@800C9944
// Fills the matrix with the world matrix of the actor stored at +0x64.
extern "C" void fn_800C9944(Mtx34* m) {
    ActorGetMatrix(m, *(Actor**)((u8*)m + 0x64));
}
//@@end
//@@800C9968
extern "C" int fn_800C9968(int msg, int a, int* data) {
    return 0;
}
//@@end
// TODO (not drafted yet, 0x78 bytes at 0x800C9970)
// fn_800C9970

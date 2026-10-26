// u_800A6F18.cpp: range synchronisation helpers and related game code (0x800A6F18-0x800B1834).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>
#include <stddef.h>
#include <string.h>

// Range synchronisation helpers, defined later in this file.
void RangeSyncWords(void* start, void* end);    // fn_800A6FC8
void RangeTouchWords(void* start, void* end);   // fn_800A6FE8

// Engine hooks implemented in another unit (still fn_ placeholders).
extern "C" void fn_8028BC2C(void* self, void* addr, unsigned long size);
extern "C" void fn_8028BC28(void* self, void* addr, unsigned long size);

// Sync a whole range through the engine hook, then close it out word by word.
void MemFlushRange(void* self, void* addr, unsigned long size)
{
    fn_8028BC2C(self, addr, size);
    RangeSyncWords(addr, (char*)addr + (size & ~3));
}

// Touch the range word by word, then report the whole span to the hook.
void MemStoreRange(void* self, void* addr, unsigned long size)
{
    RangeTouchWords(addr, (char*)addr + (size & ~3));
    fn_8028BC28(self, addr, size);
}

// Granularity, in bytes, of the ranges walked by the sync helpers below.
int RangeSyncUnit(void)
{
    return 4;
}

// Walk the word-aligned span [start, end) one word at a time, spending one
// sync slot per word.  An empty or inverted span costs nothing.
void RangeSyncWords(void* start, void* end)
{
    unsigned long count = ((unsigned long)end + 3 - (unsigned long)start) >> 2;

    if ((unsigned long)start >= (unsigned long)end) {
        return;
    }
    do {
        count -= 1;
    } while (count != 0);
}

// Walk the word-aligned span [start, end) one word at a time, spending one
// touch slot per word.  An empty or inverted span costs nothing.
void RangeTouchWords(void* start, void* end)
{
    unsigned long count = ((unsigned long)end + 3 - (unsigned long)start) >> 2;

    if ((unsigned long)start >= (unsigned long)end) {
        return;
    }
    do {
        count -= 1;
    } while (count != 0);
}

// A range is idle when there is nothing to touch: no object, or the pending
// count word at offset 0x20 has drained to zero.
int RangeIsIdle(void* range)
{
    if (range != 0) {
        return *(unsigned long*)((char*)range + 0x20) == 0;
    }
    return 1;
}

// Release the pending count word of a range, unless the range carries the
// busy flag at 0x24.  A null range is ignored.
void RangeResetPending(void* range)
{
    int busy;

    if (range == 0) {
        return;
    }
    if (range != 0) {
        if ((*(unsigned long*)((char*)range + 0x24) & 0x200000) != 0) {
            busy = 1;
        } else {
            busy = 0;
        }
    } else {
        busy = 0;
    }
    if (busy) {
        return;
    }
    *(unsigned long*)((char*)range + 0x20) = 0;
}

struct Vec3 {
    float x, y, z;
};

void Vec3Set(Vec3* v, float x, float y, float z);

extern float lbl_804D6C08;

// Reset the vector/colour fields of a range object to the shared default
// constant.  Returns false for a null object.
int RangeResetFields(void* range)
{
    float value;

    if (range == 0) {
        return 0;
    }
    value = lbl_804D6C08;
    *(float*)((char*)range + 0x270) = value;
    *(float*)((char*)range + 0x274) = value;
    *(float*)((char*)range + 0x278) = value;
    Vec3Set((Vec3*)((char*)range + 0x27C), value, value, value);
    value = lbl_804D6C08;
    *(float*)((char*)range + 0x2AC) = value;
    *(float*)((char*)range + 0x2A8) = value;
    *(float*)((char*)range + 0x28C) = value;
    return 1;
}

// Follow-target block reached through the owner pointer at +0x270.  The mode
// word at +0x20 says what the target union at +0x44 holds: nothing, a fixed
// offset vector, or another object.
union FollowTarget {
    Vec3  vec;
    void* obj;
};

struct FollowState {
    Vec3          offset;   // 0x00
    Vec3          rot;      // 0x0c
    int           index;    // 0x18
    float         scale;    // 0x1c
    int           mode;     // 0x20
    unsigned long flags;    // 0x24
    int           unk28;    // 0x28
    int           unk2C;    // 0x2c
    unsigned long flags30;  // 0x30
    int           unk34;    // 0x34
    float         unk38;    // 0x38
    float         unk3C;    // 0x3c
    float         angle;    // 0x40
    FollowTarget  target;   // 0x44
};

// A mode-2 target keeps integer coordinates at +0x24.
struct FollowTargetObj {
    char pad[0x24];
    int  pos[3];
};

struct Mtx34 {
    float m[3][4];
};

struct Actor;

void ActorGetMatrix(Mtx34* mtx, Actor* actor);
void MtxGetEuler(Vec3* out, Mtx34* mtx);
void EulerNormalize(Vec3* out, Vec3* in);
void* AssetFind(unsigned long, unsigned long);

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void fn_800BB8B8(void* obj, Vec3* out);
extern "C" void fn_8016D440(Vec3* out, Vec3* in);
extern "C" void fn_8016D68C(void* dst, void* src);
extern "C" float fn_800E1020(void* p);
float ActorAngleToPos(void* self, Vec3* pos);

extern float lbl_804D6C0C;
extern float lbl_804D6C10;
extern float lbl_804D6C14;
extern float lbl_804D6C18;
extern float lbl_804D6C1C;
extern float lbl_804D6C20;
extern float lbl_804D6C24;

// Query buffer shared by the asset/actor message round trips below.
struct AssetActorMsg {
    int   unk00;     // 0x00
    void* asset;     // 0x04
    int   unk08;     // 0x08
    int   unk0C;     // 0x0c
    void* actor;     // 0x10
    int   pad14[8];  // 0x14
    int   pos[3];    // 0x34
};

// Advance the follow target one frame; false for a null owner.
int FollowOffsetUpdate(void* self)
{
    FollowState* st;
    Vec3 base;
    Vec3 pos;
    Vec3 ang;
    Vec3 euler;
    Vec3 delta;
    Vec3 tmp;
    Mtx34 mtx;
    AssetActorMsg msg;
    void* desc;
    int mode;

    if (self == 0) {
        return 0;
    }

    base = *(Vec3*)((char*)self + 0x84);
    st = (FollowState*)((char*)self + 0x270);

    fn_800BB8B8(self, &pos);
    st->offset.x = pos.x - base.x;
    st->offset.y = pos.y - base.y;
    st->offset.z = pos.z - base.z;

    mode = st->mode;
    if (mode != 0 && (st->flags & 0x200000) != 0) {
        unsigned long type = *(unsigned long*)*(void**)((char*)self + 0xc4);

        if (type == 0x8FA05C1A || type == 0x00013C05 || type == 0xE0AB9B4F ||
            type == 0x0000FFF5 || type == 0x04624485 || type == 0x04A86D09) {
            st->angle = lbl_804D6C08;
        } else {
            if (mode == 1) {
                ang = st->target.vec;
            } else if (mode == 2 && st->target.obj != 0) {
                FollowTargetObj* t = (FollowTargetObj*)st->target.obj;
                ang.x = (float)t->pos[0];
                ang.y = (float)t->pos[1];
                ang.z = (float)t->pos[2];
            }

            st->angle = ActorAngleToPos(self, &ang);

            msg.asset = AssetFind(0xf1754bf2, 0x8c73dfa9);
            SendMsg(*(int*)((char*)self + 0x38), 0x411, &msg);

            ActorGetMatrix(&mtx, (Actor*)msg.actor);
            MtxGetEuler(&euler, &mtx);
            euler.y -= *(float*)((char*)self + 0x254);
            EulerNormalize(&euler, &euler);

            while (st->angle < lbl_804D6C10) {
                st->angle += lbl_804D6C0C;
            }
            while (st->angle >= lbl_804D6C14) {
                st->angle -= lbl_804D6C0C;
            }

            float lo = euler.y - lbl_804D6C18;
            float hi = euler.y + lbl_804D6C18;
            if (st->angle < lo) {
                st->angle = lo;
            } else if (st->angle > hi) {
                st->angle = hi;
            }

            void* target = 0;
            if (mode == 2) {
                target = st->target.obj;
            }
            if (target != 0) {
                int ax;
                int ay;
                int az;

                msg.asset = AssetFind(0xf1754bf2, 0x21c5e0);
                SendMsg(*(int*)((char*)self + 0x38), 0x411, &msg);

                ax = msg.pos[0];
                ay = msg.pos[1];
                az = msg.pos[2];
                SendMsg(*(int*)((char*)target + 0x38), 0x411, &msg);

                delta.x = (float)(msg.pos[0] - ax);
                delta.y = (float)(msg.pos[1] - ay);
                delta.z = (float)(msg.pos[2] - az);
                fn_8016D440(&euler, &delta);
                st->rot.x = euler.x;
            }
        }
    } else {
        st->angle = lbl_804D6C08;
    }

    if ((st->flags & 0x10) != 0) {
        if ((st->flags & 0x200000) == 0 ||
            fabs(st->angle - *(float*)((char*)self + 0x258)) < lbl_804D6C1C) {
            switch (mode) {
            case 1:
                pos = st->target.vec;
                break;
            case 2:
                if (st->target.obj != NULL) {
                    FollowTargetObj* t = (FollowTargetObj*)st->target.obj;
                    pos.x = (float)t->pos[0];
                    pos.y = (float)t->pos[1];
                    pos.z = (float)t->pos[2];
                }
                break;
            }
        }
    } else if (fn_800E1020((char*)self + 0x240) < lbl_804D6C20) {
        pos = base;
    }

    pos.x -= base.x;
    pos.y -= base.y;
    pos.z -= base.z;

    if (fabs(pos.z) > lbl_804D6C20 || fabs(pos.x) > lbl_804D6C20 ||
        fabs(pos.y) > lbl_804D6C20) {
        fn_8016D440(&tmp, &pos);
        Vec3Set(&st->rot, tmp.z - *(float*)((char*)self + 0x254),
                          tmp.x - *(float*)((char*)self + 0x24c), 0.0f);
        if ((st->flags30 & 0x02000000) != 0) {
            st->rot.z += lbl_804D6C14;
        }
        EulerNormalize(&st->rot, &st->rot);
    } else {
        Vec3Set(&st->rot, 0.0f, -*(float*)((char*)self + 0x24c), 0.0f);
    }

    if ((st->flags & 0x10) == 0 && (st->flags & 0x200000) == 0) {
        st->rot.x = -*(float*)((char*)self + 0x24c);
    }

    desc = *(void**)((char*)self + 0xc4);
    st->unk3C = *(float*)((char*)desc + st->index * 4 + 0xf4);
    st->unk38 = *(float*)((char*)desc + 0xf0);
    st->scale = *(float*)((char*)desc + st->index * 4 + 0xe4);

    if (*(int*)((char*)desc + 0xc) == 0x00012EC1) {
        if (st->flags & 0x10) {
            st->scale *= lbl_804D6C24;
        }
    }

    return 1;
}

// Reset the follow-target block: clear the target, copy the owner's integer
// position into the offset and keep the old offset as the fallback vector.
void FollowOffsetReset(void* self)
{
    FollowState* st;

    if (self == NULL) {
        return;
    }

    st = (FollowState*)((char*)self + 0x270);
    st->flags = 0;
    st->unk28 = 0;
    st->unk2C = 0;
    st->flags30 = 0;
    st->unk34 = 0;
    st->index = 0;
    st->unk38 = lbl_804D6C08;
    st->unk3C = lbl_804D6C08;
    st->target.obj = NULL;
    st->mode = 0;

    fn_8016D68C(&st->rot, (char*)self + 0x24c);

    st->target.vec = st->offset;
    st->offset.x = (float)*(int*)((char*)self + 0x24);
    st->offset.y = (float)*(int*)((char*)self + 0x28);
    st->offset.z = (float)*(int*)((char*)self + 0x2c);
}

// Point the follow target at another object (mode 2).  A null block, or one
// that carries the busy flag at +0x24, is left untouched.
void FollowSetTargetObj(void* range, void* obj)
{
    FollowState* st;
    int busy;

    if (range == 0) {
        return;
    }
    if (range != 0) {
        if ((*(unsigned long*)((char*)range + 0x24) & 0x200000) != 0) {
            busy = 1;
        } else {
            busy = 0;
        }
    } else {
        busy = 0;
    }
    if (busy) {
        return;
    }
    st = (FollowState*)range;
    st->mode = 2;
    st->target.obj = obj;
}

// Point the follow target at a fixed offset vector (mode 1).  A null block, a
// null vector, or a block carrying the busy flag at +0x24 is left untouched.
void FollowSetTargetVec(void* range, Vec3* vec)
{
    FollowState* st;
    int busy;

    if (range == 0) {
        return;
    }
    if (vec == 0) {
        return;
    }
    if (range == 0) {
        busy = 0;
    } else if ((*(unsigned long*)((char*)range + 0x24) & 0x200000) == 0) {
        busy = 0;
    } else {
        busy = 1;
    }
    if (busy) {
        return;
    }
    st = (FollowState*)range;
    st->mode = 1;
    st->target.vec = *vec;
}

// Fetch the follow target position into `out`: the fixed offset vector for
// mode 1, or the integer position of the follow object for mode 2.  Returns 0
// when the block is null, its mode is unknown, or it has no target.
int FollowGetTargetPos(void* range, Vec3* out)
{
    FollowState* st;
    FollowTargetObj* obj;

    if (range == 0) {
        return 0;
    }
    st = (FollowState*)range;
    if (st->mode == 0) {
        return 0;
    }
    if (st->mode == 1) {
        *out = st->target.vec;
        return 1;
    }
    if (st->mode == 2) {
        obj = (FollowTargetObj*)st->target.obj;
        if (obj != 0) {
            out->x = (float)obj->pos[0];
            out->y = (float)obj->pos[1];
            out->z = (float)obj->pos[2];
            return 1;
        }
    }
    return 0;
}

// The object a mode-2 follow target points at; null for any other case.
void* FollowGetTargetObj(void* range)
{
    FollowState* st;

    if (range != 0 && ((FollowState*)range)->mode == 2) {
        st = (FollowState*)range;
        return st->target.obj;
    }
    return 0;
}

// Bit-clear helper from another unit: clears bit `bit` of the word at `p`.
void BitClear(unsigned long* p, int bit);

// Owner stored at +0x20 of a condition, or null for a null condition.
int ConditionGetOwner(void* cond)
{
    if (cond == 0) {
        return 0;
    }
    return *(int*)((char*)cond + 0x20);
}

// Set or clear bit `bit` in the condition's flag block at +0x2c.
// Returns 0 for a null condition, 1 otherwise.
int ConditionSetBit(void* cond, int bit, int set)
{
    if (cond == 0) {
        return 0;
    }
    if (set == 0) {
        BitClear((unsigned long*)((char*)cond + 0x2c), bit);
        return 1;
    }
    *(unsigned long*)((char*)cond + 0x2c + (bit / 32) * 4) |= 1 << (bit & 0x1f);
    return 1;
}

// Test bit `bit` of the condition's flag block at +0x2c.  Returns 0 for a
// null condition, otherwise 1 when the bit is set.
int HasCondition(void* cond, int bit)
{
    unsigned long mask;

    if (cond == 0) {
        return 0;
    }
    mask = 1 << (bit & 0x1f) & *(unsigned long*)((char*)cond + 0x2c + (bit / 32) * 4);
    return (-mask | mask) >> 31;
}

// Clear or set bit `bit` in the condition's flag block at +0x24.
// Returns 0 for a null condition, 1 otherwise.
int ConditionUpdateBit(void* cond, int bit, int set)
{
    if (cond == 0) {
        return 0;
    }
    if (set == 0) {
        BitClear((unsigned long*)((char*)cond + 0x24), bit);
    } else {
        *(unsigned long*)((char*)cond + 0x24 + (bit / 32) * 4) |= 1 << (bit & 0x1f);
    }
    return 1;
}

// Test one bit of a 32-bit flag array: returns 1 when bit `index` is set,
// 0 when it is clear or the array is missing.
int MemFlagIsSet(void* bits, int index)
{
    unsigned int set;

    if (bits == 0) {
        return 0;
    }
    set = (unsigned int)(1 << (index & 31)) & *(unsigned int*)((char*)bits + 0x24 + (index / 32) * 4);
    return (-set | set) >> 31;
}

// Copy a three-float vector; returns the destination, or null when either
// pointer is missing.
float* MemCopyVec3(float* dst, float* src)
{
    if (dst == 0 || src == 0) {
        return 0;
    }
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    return dst;
}

// Model helpers from other units: bone lookup by index, animation stepping
// and the per-bone matrix base of the current animation.
int ModelFindBone(int model, int bone);
int ModelAnimate(int model, char* name, int mode);
extern "C" void* fn_8003A3F0(int model, int anim, int frame);

// Write the matrix of the bone named at +0xd8 of the owner's model into
// `mtx`; when the bone is missing, fall back to the actor's own matrix.
int FollowBoneMatrix(void* self, Mtx34* mtx, int flag)
{
    int bone;

    if (mtx == 0) {
        return 0;
    }
    if (flag != 0) {
        bone = ModelFindBone(*(int*)((char*)self + 0xcc), *(int*)((char*)self + 0xd8));
    } else {
        bone = ModelFindBone(*(int*)((char*)self + 0xcc), *(int*)((char*)self + 0xd8));
    }
    if (bone == -1) {
        ActorGetMatrix(mtx, (Actor*)self);
    } else {
        int frame = ModelAnimate(*(int*)((char*)self + 0xcc), 0, 0);
        void* base = fn_8003A3F0(*(int*)((char*)self + 0xcc),
                                 *(int*)((char*)self + 0xf0), frame);
        ActorGetMatrix(mtx, (Actor*)((char*)base + bone * 0x30));
    }
    return 1;
}

// Parameters of the zone walk below, laid out as the callback expects them:
// the owner, the two reference points, the distance limit at +0x900 and the
// running best hit.
struct ZoneQuery {
    void* self;     // 0x00
    Vec3  a;        // 0x04
    Vec3  b;        // 0x10
    float limit;    // 0x1c
    int   arg;      // 0x20
    float best;     // 0x24
    int   flag;     // 0x28
    float value;    // 0x2c
};

struct ZoneList;

typedef int (*ZoneWalkFn)(void* self, void* node, void* ctx);

void ZoneWalkNodes(ZoneList* list, ZoneWalkFn fn, void* ctx);

// Per-zone visitor used by the walk above (defined below).
int ZoneFindNearestVisit(void* self, void* node, void* ctx);

int ActorIsActive(Actor* actor);
extern "C" int fn_802A1D04(void);
extern "C" void fn_800A0168(Mtx34* mtx, Mtx34* src);
extern "C" void fn_800A0030(Vec3* out, Mtx34* mtx, Vec3* in);
extern "C" void fn_800E1098(Vec3* out, Vec3* v, float scale);

extern float lbl_804D6C30;
extern float lbl_804D6C34;
extern float lbl_804D6C38;
extern float lbl_804D6C3C;
extern float lbl_804D6C40;
extern float lbl_804D6C50;

// Unit forward/right basis vectors, three floats each.
extern Vec3 lbl_803445A0;
extern Vec3 lbl_803445AC;

// Walk the zone list under `self` looking for the nearest zone hit.  On a hit
// the distance/flag/value triple is copied to the out-parameters and 1 is
// returned; otherwise the defaults are written and 0 is returned.
int ZoneFindNearestHit(void* self, Vec3* a, Vec3* b, float* outBest,
                       int* outFlag, float* outValue, int arg)
{
    ZoneQuery q;

    q.self = self;
    q.limit = *(float*)((char*)self + 0x900);
    q.a = *a;
    q.b = *b;
    q.best = lbl_804D6C30;
    q.flag = 0;
    q.value = lbl_804D6C34;
    q.arg = arg;
    ZoneWalkNodes((ZoneList*)((char*)self + 0x658), ZoneFindNearestVisit, &q);

    if (q.best < lbl_804D6C38) {
        if (outBest != 0) {
            *outBest = q.best;
        }
        if (outFlag != 0) {
            *outFlag = q.flag;
        }
        if (outValue != 0) {
            *outValue = q.value;
        }
        return 1;
    }
    if (outBest != 0) {
        *outBest = lbl_804D6C30;
    }
    if (outFlag != 0) {
        *outFlag = 0;
    }
    if (outValue != 0) {
        *outValue = lbl_804D6C34;
    }
    return 0;
}

// Message 0x400 payload exchanged with an object's handler: the handler
// reports whether it takes part (word at +0x0c) and which collision layers
// the object belongs to (mask at +0x18).
struct Msg400Info {
    unsigned char pad_00[0x0c];
    int           present;   // 0x0c
    unsigned char pad_10[0x08];
    unsigned int  mask;      // 0x18
    unsigned char pad_1c[0x04];
};

// Visit one zone of the walk: ask both the query owner and the candidate for
// their message 0x400 payload, then test the segment between the query's two
// points against the candidate's reach.  The first hit inside the reach
// updates the running best triple (distance, object, radius).  Returns the
// walker's stop flag: 1 means the walk is over.
int ZoneFindNearestVisit(void* self, void* node, void* ctx)
{
    ZoneQuery* q;
    unsigned char* object;
    Msg400Info queryInfo;
    Msg400Info nodeInfo;
    Vec3 pos;
    Vec3 delta;
    Vec3 segment;
    Vec3 point;
    Vec3 toEnd;
    float reach;
    float radius;
    float length;
    float along;
    float gap;
    float root;
    float dist;
    float t;
    unsigned int type;
    int count;

    (void)self;
    q = (ZoneQuery*)ctx;
    object = (unsigned char*)node;

    SendMsg(*(int*)((char*)q->self + 0x38), 0x400, &queryInfo);
    if (SendMsg(*(int*)((char*)object + 0x38), 0x400, &nodeInfo) == 0) {
        return 1;
    }
    if (nodeInfo.present == 0) {
        return 1;
    }
    if ((nodeInfo.mask & queryInfo.mask) == 0) {
        return 1;
    }
    if (ActorIsActive((Actor*)object) == 0) {
        return 1;
    }
    if (*(int*)((char*)object + 0x38) == q->arg) {
        return 1;
    }

    // The high byte of the object id selects how the position and reach are
    // stored.
    type = *(unsigned int*)((char*)object + 0x38) >> 24;
    if (type == 2) {
        pos.x = (float)*(unsigned int*)((char*)object + 0x24);
        pos.y = (float)*(unsigned int*)((char*)object + 0x28);
        pos.z = (float)*(unsigned int*)((char*)object + 0x2c);
        reach = *(float*)(*(int*)((char*)object + 0x738) + 0x6c);
    } else if (type < 2) {
        if (type != 1) {
            return 1;
        }
        if (*(int*)((char*)object + 0x88) == 0 ||
            *(int*)*(int*)((char*)object + 0x88) != 0x4956805b ||
            (*(unsigned int*)((char*)object + 0xac) & 0x20000000) != 0) {
            return 1;
        }
        pos.x = (float)*(unsigned int*)((char*)object + 0x24);
        pos.y = (float)*(unsigned int*)((char*)object + 0x28);
        pos.z = (float)*(unsigned int*)((char*)object + 0x2c);
        reach = lbl_804D6C3C;
    } else {
        if (type != 0x13) {
            return 1;
        }
        if (HasCondition((char*)object + 0x270, 0x0b) != 0 ||
            HasCondition((char*)object + 0x270, 0x01) != 0 ||
            HasCondition((char*)object + 0x270, 0x03) != 0) {
            return 1;
        }
        if (fn_802A1D04() == 3) {
            return 1;
        }
        pos.x = *(float*)((char*)object + 0x84);
        pos.y = *(float*)((char*)object + 0x88);
        pos.z = *(float*)((char*)object + 0x8c);
        reach = *(float*)((char*)object + 0x900);
    }

    count = (fabsf(pos.x) > lbl_804D6C40) + (fabsf(pos.y) > lbl_804D6C40) +
            (fabsf(pos.z) > lbl_804D6C40);
    if (count == 0) {
        return 0;
    }
    if (!(reach > lbl_804D6C34)) {
        return 0;
    }

    delta.x = pos.x - q->a.x;
    delta.y = pos.y - q->a.y;
    delta.z = pos.z - q->a.z;
    segment.x = q->b.x - q->a.x;
    segment.y = q->b.y - q->a.y;
    segment.z = q->b.z - q->a.z;
    radius = (reach + q->limit) * (reach + q->limit);

    length = fn_800E1020(&segment);
    if (length < lbl_804D6C40) {
        // Degenerate segment: only the start point can be hit.
        if (delta.y * delta.y + delta.x * delta.x + delta.z * delta.z <= radius) {
            q->best = lbl_804D6C34;
            q->flag = (int)node;
            q->value = reach;
        }
        return 1;
    }

    along = (segment.y * delta.y + segment.x * delta.x + segment.z * delta.z) / length;
    if (along > length) {
        // The closest approach is past the end of the segment.
        toEnd.x = pos.x - q->b.x;
        toEnd.y = pos.y - q->b.y;
        toEnd.z = pos.z - q->b.z;
        if (toEnd.y * toEnd.y + toEnd.x * toEnd.x + toEnd.z * toEnd.z > radius) {
            return 1;
        }
    }
    if (along < lbl_804D6C34) {
        if (delta.y * delta.y + delta.x * delta.x + delta.z * delta.z > radius) {
            return 1;
        }
    }

    fn_800E1098(&point, &segment, along);
    point.x += q->a.x;
    point.y += q->a.y;
    point.z += q->a.z;
    {
        float dx = point.x - pos.x;
        float dy = point.y - pos.y;
        float dz = point.z - pos.z;
        dist = dy * dy + dx * dx + dz * dz;
    }
    if (dist >= radius) {
        return 1;
    }

    // Back the entry point off the sphere surface along the segment.
    gap = radius - dist;
    root = lbl_804D6C34;
    if (gap > lbl_804D6C34) {
        root = sqrtf(gap);
    }
    t = (along - root) / length;
    if (t < q->best) {
        q->best = t;
        q->flag = (int)node;
        q->value = reach;
        return 1;
    }
    return 0;
}

// True when every component of a vector is inside the zero tolerance.
static inline int Vec3IsZero(const Vec3* v)
{
    return ((fabsf(v->x) > lbl_804D6C40) + (fabsf(v->y) > lbl_804D6C40) +
            (fabsf(v->z) > lbl_804D6C40)) == 0;
}

// Scale a vector to unit length; a near-zero vector stays zero.  Returns the
// length that was removed (callers below ignore it).
static inline float Vec3Normalize(Vec3* out, const Vec3* v)
{
    float len;

    if (Vec3IsZero(v)) {
        out->x = lbl_804D6C34;
        out->y = lbl_804D6C34;
        out->z = lbl_804D6C34;
        return lbl_804D6C34;
    }
    len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    out->x = v->x / len;
    out->y = v->y / len;
    out->z = v->z / len;
    return len;
}

// Classify a direction against the actor's facing frame: 0 when it points
// forward, 3 when it points backward and 1/2 for the two sides.
int ZoneClassifyDirection(void* self, Vec3* dir)
{
    Vec3 forward;
    Vec3 right;
    Vec3 normal;
    Mtx34 mtx;
    float dot;

    forward = lbl_803445A0;
    right = lbl_803445AC;
    ActorGetMatrix(&mtx, (Actor*)self);
    fn_800A0168(&mtx, &mtx);
    fn_800A0030(&forward, &mtx, &forward);
    if (Vec3IsZero(dir)) {
        return 0;
    }
    Vec3Normalize(&normal, dir);
    dot = forward.y * normal.y + forward.x * normal.x + forward.z * normal.z;
    if (fabsf(dot) < lbl_804D6C50) {
        // Nearly perpendicular: fall back to the sideways axis.
        fn_800A0030(&right, &mtx, &right);
        if (right.y * normal.y + right.x * normal.x + right.z * normal.z >
            lbl_804D6C34) {
            return 2;
        }
        return 1;
    }
    if (dot < lbl_804D6C34) {
        return 3;
    }
    return 0;
}

// Bone helpers from another unit: the running animation's frame, the pose of
// that frame and the routine that folds a pose into a bounding box, plus the
// update of the secondary attach matrix.
extern "C" void* fn_8003A1F0(int model, int anim, int frame);
extern "C" void fn_80039958(int model, Mtx34* mtx, void* pose, void* boxA,
                            void* boxB);
extern "C" void fn_801D52E0(void* obj, Mtx34* mtx);

extern float lbl_804D6C54;

// Refresh the bone bounding box of an actor: while the animation is running
// (+0xf0) the box is derived from the pose matrix of the current frame,
// otherwise it collapses to the default -32..32 box held twice in a row.
// The attach object at +0x5f4, when present, gets the actor matrix as well.
void ActorUpdateBoneBounds(void* self)
{
    Mtx34 attachMtx;
    Mtx34 poseMtx;

    if (*(float*)((char*)self + 0x13c) <= lbl_804D6C54 &&
        *(int*)((char*)self + 0xec) != 10) {
        if (*(int*)((char*)self + 0xf0) == 0) {
            *(int*)((char*)self + 0x90) = -0x20;
            *(int*)((char*)self + 0x94) = -0x20;
            *(int*)((char*)self + 0x98) = -0x20;
            *(int*)((char*)self + 0x9c) = 0x20;
            *(int*)((char*)self + 0xa0) = 0x20;
            *(int*)((char*)self + 0xa4) = 0x20;
            *(int*)((char*)self + 0xa8) = -0x20;
            *(int*)((char*)self + 0xac) = -0x20;
            *(int*)((char*)self + 0xb0) = -0x20;
            *(int*)((char*)self + 0xb4) = 0x20;
            *(int*)((char*)self + 0xb8) = 0x20;
            *(int*)((char*)self + 0xbc) = 0x20;
        } else {
            int frame = ModelAnimate(*(int*)((char*)self + 0xcc), 0, 0);
            void* pose = fn_8003A1F0(*(int*)((char*)self + 0xcc),
                                     *(int*)((char*)self + 0xf0), frame);

            ActorGetMatrix(&poseMtx, (Actor*)self);
            fn_80039958(*(int*)((char*)self + 0xcc), &poseMtx, pose,
                        (char*)self + 0x90, (char*)self + 0xa8);
        }
        if (*(void**)((char*)self + 0x5f4) != 0) {
            ActorGetMatrix(&attachMtx, (Actor*)self);
            fn_801D52E0(*(void**)((char*)self + 0x5f4), &attachMtx);
        }
    }
}

// Closest approach of two segments p1->p2 and p3->p4.  The parameter of the
// closest point on the second segment is written to `outT` (0..1) and the
// squared distance between the two closest points is returned.  Parallel or
// degenerate segments pin the parameter to zero.
float SegmentClosestSquaredDist(float* p1, float* p2, float* p3, float* p4,
                                float* outT)
{
    float dz = p2[2] - p1[2];
    float duz = p4[2] - p3[2];
    float dx = p2[0] - p1[0];
    float dux = p4[0] - p3[0];
    float dy = p2[1] - p1[1];
    float duy = p4[1] - p3[1];
    float az = dz - duz;
    float ax = dx - dux;
    float ay = dy - duy;
    float t;
    float y;
    float x;
    float z;

    if (((fabsf(ax) > lbl_804D6C40) + (fabsf(ay) > lbl_804D6C40) +
         (fabsf(az) > lbl_804D6C40)) == 0) {
        *outT = lbl_804D6C34;
    } else {
        *outT = -((p1[1] - p3[1]) * ay + (p1[0] - p3[0]) * ax +
                  (p1[2] - p3[2]) * az);
        *outT = *outT / (ay * ay + ax * ax + az * az);
    }
    if (*outT > lbl_804D6C38) {
        *outT = lbl_804D6C38;
    } else if (*outT < lbl_804D6C34) {
        *outT = lbl_804D6C34;
    }

    t = *outT;
    y = (p3[1] + duy * t) - (p1[1] + dy * t);
    x = (p3[0] + dux * t) - (p1[0] + dx * t);
    z = (p3[2] + duz * t) - (p1[2] + dz * t);
    return z * z + x * x + y * y;
}

// Engine hooks used by the line-of-sight probes below.
extern "C" int fn_800BA7FC(int type);
extern "C" int fn_8001F928(void* self, void* from, int mask, void* callback, void* out);
extern "C" void fn_80009170(void* from, void* to, int a, int b, int c);
extern "C" void fn_800090BC(void* a, void* b, int x, int y, int z);
int ZoneProbeHitFilter(int self, float* out);
int SlotOwnerMatches(void** slot, void* owner);

extern float lbl_804D6C54;

// Probe the line between this owner and another actor.  A set "already
// handled" flag at +0x2f short-circuits to true.  The probe runs through the
// object's query callback (or the default one) and, when the engine has a
// collision world, through the hit-recording callback; the hit point is then
// re-probed along each axis with a small tolerance.  The condition bit at
// +0x1a is updated with the result.
int FollowLineCheck(void* self, void* target)
{
    AssetActorMsg msg;
    int from[3];
    int to[3];
    void* callback;
    int result;

    if (MemFlagIsSet((int*)((char*)self + 0x270), 0x2f) != 0) {
        return 1;
    }

    msg.asset = AssetFind(0xf1754bf2, 0x21c5e0);
    if (SendMsg(*(int*)((char*)self + 0x38), 0x411, &msg) != 0) {
        from[0] = msg.pos[0];
        from[1] = msg.pos[1];
        from[2] = msg.pos[2];
    } else {
        from[0] = *(int*)((char*)self + 0x24);
        from[1] = *(int*)((char*)self + 0x28);
        from[2] = *(int*)((char*)self + 0x2c);
    }

    msg.asset = AssetFind(0xf1754bf2, 0x922bf04e);
    if (SendMsg(*(int*)((char*)target + 0x38), 0x411, &msg) != 0) {
        to[0] = msg.pos[0];
        to[1] = msg.pos[1];
        to[2] = msg.pos[2];
    } else {
        to[0] = *(int*)((char*)target + 0x24);
        to[1] = *(int*)((char*)target + 0x28);
        to[2] = *(int*)((char*)target + 0x2c);
    }

    callback = *(void**)((char*)self + 0x600);
    if (callback == 0) {
        callback = (void*)SlotOwnerMatches;
    }

    if (fn_800BA7FC(0x20) == 0) {
        result = fn_8001F928(self, from, 0x2000a200, callback, target);
    } else {
        Vec3 hit;
        Vec3 a;
        Vec3 b;

        fn_80009170(from, to, 0, 0, 0xff);
        result = fn_8001F928(self, from, 0x2000a200, (void*)ZoneProbeHitFilter, &hit);
        if (result != 0) {
            a.x = hit.x + lbl_804D6C54;
            a.y = hit.y;
            a.z = hit.z;
            b.x = hit.x - lbl_804D6C54;
            b.y = hit.y;
            b.z = hit.z;
            fn_800090BC(&a, &b, 0xff, 0, 0);

            a.x = hit.x;
            a.y = hit.y + lbl_804D6C54;
            a.z = hit.z;
            b.x = hit.x;
            b.y = hit.y - lbl_804D6C54;
            b.z = hit.z;
            fn_800090BC(&a, &b, 0xff, 0, 0);

            a.x = hit.x;
            a.y = hit.y;
            a.z = hit.z + lbl_804D6C54;
            b.x = hit.x;
            b.y = hit.y;
            b.z = hit.z - lbl_804D6C54;
            fn_800090BC(&a, &b, 0xff, 0, 0);
        }
    }

    if (result == 0) {
        ConditionUpdateBit((char*)self + 0x270, 0x1a, 1);
        return 1;
    }
    ConditionUpdateBit((char*)self + 0x270, 0x1a, 0);
    return 0;
}

// Same probe as FollowLineCheck, but aimed at a world position instead of
// another actor.  The position is truncated to the engine's integer grid.
int FollowLineCheckPos(void* self, float* pos)
{
    int from[3];
    int to[3];
    void* callback;
    int result;

    if (MemFlagIsSet((int*)((char*)self + 0x270), 0x2f) != 0) {
        return 1;
    }

    from[0] = *(int*)((char*)self + 0x24);
    from[1] = *(int*)((char*)self + 0x28);
    from[2] = *(int*)((char*)self + 0x2c);
    to[0] = (int)pos[0];
    to[1] = (int)pos[1];
    to[2] = (int)pos[2];

    callback = *(void**)((char*)self + 0x600);
    if (callback == 0) {
        callback = (void*)SlotOwnerMatches;
    }

    if (fn_800BA7FC(0x20) == 0) {
        result = fn_8001F928(self, from, 0x2000a200, callback, 0);
    } else {
        Vec3 hit;
        Vec3 a;
        Vec3 b;

        fn_80009170(from, to, 0, 0, 0xff);
        result = fn_8001F928(self, from, 0x2000a200, (void*)ZoneProbeHitFilter, &hit);
        if (result != 0) {
            a.x = hit.x + lbl_804D6C54;
            a.y = hit.y;
            a.z = hit.z;
            b.x = hit.x - lbl_804D6C54;
            b.y = hit.y;
            b.z = hit.z;
            fn_800090BC(&a, &b, 0xff, 0, 0);

            a.x = hit.x;
            a.y = hit.y + lbl_804D6C54;
            a.z = hit.z;
            b.x = hit.x;
            b.y = hit.y - lbl_804D6C54;
            b.z = hit.z;
            fn_800090BC(&a, &b, 0xff, 0, 0);

            a.x = hit.x;
            a.y = hit.y;
            a.z = hit.z + lbl_804D6C54;
            b.x = hit.x;
            b.y = hit.y;
            b.z = hit.z - lbl_804D6C54;
            fn_800090BC(&a, &b, 0xff, 0, 0);
        }
    }

    if (result == 0) {
        ConditionUpdateBit((char*)self + 0x270, 0x1a, 1);
        return 1;
    }
    ConditionUpdateBit((char*)self + 0x270, 0x1a, 0);
    return 0;
}

// Engine helpers used by the line-of-sight probes below.
extern "C" void fn_800E0CB8(void* dst, void* src);
extern "C" void fn_8009F724(void* dst, void* src, int index);
extern "C" void fn_8009F50C(void* dst, void* src);
extern double lbl_804D6C48;
extern float lbl_804D6C58;
extern double lbl_804D6C60;
extern float lbl_804D6C68;
extern float lbl_804D6C6C;
extern unsigned long lbl_80449038[12];

void ActorAim(int* out, Actor* actor, int mode);

// Round an accumulated 12-bit fixed-point value back to an integer.
static inline int RoundFixed12(unsigned int v)
{
    return ((int)v >> 12) + (int)((int)v < 0 && (v & 0xfff) != 0);
}

// Cross term of the fixed-point basis build: (a * b + 0x800) - c * d,
// rounded back to an integer.
static inline int CrossTerm(int a, int b, int c, int d)
{
    unsigned int v = (unsigned int)(a * b + 0x800) - (unsigned int)(c * d);

    return RoundFixed12(v);
}

// Re-probe the three axis-aligned spans through a recorded hit point.
static inline void ProbeHitSweep(Vec3* hit)
{
    Vec3 a;
    Vec3 b;

    a.x = hit->x + lbl_804D6C54;
    a.y = hit->y;
    a.z = hit->z;
    b.x = hit->x - lbl_804D6C54;
    b.y = hit->y;
    b.z = hit->z;
    fn_800090BC(&a, &b, 0xff, 0, 0);

    a.x = hit->x;
    a.y = hit->y + lbl_804D6C54;
    a.z = hit->z;
    b.x = hit->x;
    b.y = hit->y - lbl_804D6C54;
    b.z = hit->z;
    fn_800090BC(&a, &b, 0xff, 0, 0);

    a.x = hit->x;
    a.y = hit->y;
    a.z = hit->z + lbl_804D6C54;
    b.x = hit->x;
    b.y = hit->y;
    b.z = hit->z - lbl_804D6C54;
    fn_800090BC(&a, &b, 0xff, 0, 0);
}

// Cast the probe line from `from` to `to` through the object's query callback,
// or through the hit-recording callback when the collision world is live.
// Returns the engine probe result; a hit is also swept on the three axes.
static inline int ProbeLine(void* self, int* from, int* to, void* callback,
                            void* target, Vec3* hit)
{
    int result;

    if (fn_800BA7FC(0x20) == 0) {
        result = fn_8001F928(self, from, 0x2000a200, callback, target);
    } else {
        fn_80009170(from, to, 0, 0, 0xff);
        result = fn_8001F928(self, from, 0x2000a200, (void*)ZoneProbeHitFilter, hit);
        if (result != 0) {
            ProbeHitSweep(hit);
        }
    }
    return result;
}

// Line-of-sight probe from `self` towards `target`, with the time budget
// `budget` (seconds * speed) limiting how far ahead the owner may move.
// Returns 1 when the way is blocked or the target is behind the aim cone.
int FollowLineCheckActorDist(void* self, void* target, float budget)
{
    AssetActorMsg msg;
    int from[3];
    int to[3];
    void* callback;
    int result;
    Vec3 hit;
    int aim[3];
    float dx;
    float dy;
    float dz;
    float dot;
    unsigned long mat[12];
    int dir[3];
    int side[3];
    int up[3];
    int offset[3];
    int savedFromZ;

    if (MemFlagIsSet((int*)((char*)self + 0x270), 0x2f) != 0) {
        return 1;
    }

    msg.asset = *(void**)((char*)self + 0xd8);
    if (SendMsg(*(int*)((char*)self + 0x38), 0x411, &msg) != 0) {
        from[0] = msg.pos[0];
        from[1] = msg.pos[1];
        from[2] = msg.pos[2];
    } else {
        from[0] = *(int*)((char*)self + 0x24);
        from[1] = *(int*)((char*)self + 0x28);
        from[2] = *(int*)((char*)self + 0x2c);
    }
    savedFromZ = from[2];

    msg.asset = AssetFind(0xf1754bf2, 0x922bf04e);
    if (SendMsg(*(int*)((char*)target + 0x38), 0x411, &msg) != 0) {
        to[0] = msg.pos[0];
        to[1] = msg.pos[1];
        to[2] = msg.pos[2];
    } else {
        to[0] = *(int*)((char*)target + 0x24);
        to[1] = *(int*)((char*)target + 0x28);
        to[2] = *(int*)((char*)target + 0x2c);
    }

    callback = *(void**)((char*)self + 0x600);
    if (callback == 0) {
        callback = (void*)SlotOwnerMatches;
    }
    result = ProbeLine(self, from, to, callback, target, &hit);

    if (result == 0) {
        if (MemFlagIsSet((int*)((char*)self + 0x270), 0x1a) != 0) {
            return 1;
        }
        if (budget < (double)lbl_804D6C40) {
            ConditionUpdateBit((char*)self + 0x270, 0x1a, 1);
            return 1;
        }

        ActorAim(aim, (Actor*)self, 1);
        dx = (float)to[0] - *(float*)((char*)self + 0x84);
        dy = (float)to[1] - *(float*)((char*)self + 0x88);
        dz = (float)to[2] - *(float*)((char*)self + 0x8c);
        if (((fabsf(dx) > lbl_804D6C40) + (fabsf(dy) > lbl_804D6C40) +
             (fabsf(dz) > lbl_804D6C40)) == 0) {
            dx = lbl_804D6C34;
            dy = lbl_804D6C34;
            dz = lbl_804D6C34;
        } else {
            float inv = 1.0f / sqrtf(dx * dx + dy * dy + dz * dz);

            dx *= inv;
            dy *= inv;
            dz *= inv;
        }
        dot = dz * lbl_804D6C58 * (float)aim[2] +
              dx * lbl_804D6C58 * (float)aim[0] +
              dy * lbl_804D6C58 * (float)aim[1];
        if (lbl_804D6C60 < (double)dot) {
            ConditionUpdateBit((char*)self + 0x270, 0x1a, 1);
            return 1;
        }

        from[0] = *(int*)((char*)self + 0x24);
        from[1] = *(int*)((char*)self + 0x28);
        from[2] = savedFromZ;
        callback = *(void**)((char*)self + 0x600);
        if (callback == 0) {
            callback = (void*)SlotOwnerMatches;
        }
        result = ProbeLine(self, from, to, callback, target, &hit);

        if (result == 0) {
            dir[0] = to[0] - from[0];
            dir[1] = to[1] - from[1];
            dir[2] = to[2] - from[2];
            fn_800E0CB8(dir, dir);
            ActorAim(aim, (Actor*)self, 2);

            side[0] = CrossTerm(dir[1], aim[2], dir[2], aim[1]);
            side[1] = CrossTerm(dir[2], aim[0], dir[0], aim[2]);
            side[2] = CrossTerm(dir[0], aim[1], dir[1], aim[0]);
            up[0] = CrossTerm(side[1], dir[2], side[2], dir[1]);
            up[1] = CrossTerm(side[2], dir[0], side[0], dir[2]);
            up[2] = CrossTerm(side[0], dir[1], side[1], dir[0]);

            fn_8009F724(mat, side, 0);
            fn_8009F724(mat, dir, 1);
            fn_8009F724(mat, up, 2);

            offset[0] = (int)((double)*(float*)((char*)self + 0x900) * budget);
            offset[1] = 0;
            offset[2] = 0;

            lbl_80449038[0] = mat[0];
            lbl_80449038[1] = mat[1];
            lbl_80449038[2] = mat[2];
            lbl_80449038[3] = mat[3];
            lbl_80449038[4] = mat[4];
            lbl_80449038[5] = mat[5];
            lbl_80449038[6] = mat[6];
            lbl_80449038[7] = mat[7];
            lbl_80449038[8] = mat[8];
            lbl_80449038[9] = mat[9];
            lbl_80449038[10] = mat[10];
            lbl_80449038[11] = mat[11];
            fn_8009F50C(offset, offset);

            from[0] = *(int*)((char*)self + 0x24) + offset[0];
            from[1] = *(int*)((char*)self + 0x28) + offset[1];
            from[2] = savedFromZ;
            callback = *(void**)((char*)self + 0x600);
            if (callback == 0) {
                callback = (void*)SlotOwnerMatches;
            }
            result = ProbeLine(self, from, to, callback, target, &hit);

            if (result == 0) {
                from[0] = *(int*)((char*)self + 0x24) - offset[0];
                from[1] = *(int*)((char*)self + 0x28) - offset[1];
                from[2] = savedFromZ;
                callback = *(void**)((char*)self + 0x600);
                if (callback == 0) {
                    callback = (void*)SlotOwnerMatches;
                }
                result = ProbeLine(self, from, to, callback, target, &hit);

                if (result == 0) {
                    ConditionUpdateBit((char*)self + 0x270, 0x1a, 1);
                    return 1;
                }
            }
        }
    }

    ConditionUpdateBit((char*)self + 0x270, 0x1a, 0);
    return 0;
}

// Line-of-sight probe from `self` towards a world position, repeated at the
// positive and negative ends of the swept offset.  Returns 0 when the way is
// clear and 1 when one of the probes is obstructed.
int FollowLineCheckSweep(void* self, float* pos)
{
    int from[3];
    int to[3];
    void* callback;
    int result;
    Vec3 hit;
    int aim[3];
    int dir[3];
    int side[3];
    int up[3];
    unsigned long mat[12];
    int offset[3];

    from[0] = *(int*)((char*)self + 0x24);
    from[1] = *(int*)((char*)self + 0x28);
    from[2] = *(int*)((char*)self + 0x2c);
    to[0] = (int)pos[0];
    to[1] = (int)pos[1];
    to[2] = (int)pos[2];

    if (MemFlagIsSet((int*)((char*)self + 0x270), 0x2f) != 0) {
        return 1;
    }

    callback = *(void**)((char*)self + 0x600);
    if (callback == 0) {
        callback = (void*)SlotOwnerMatches;
    }
    result = ProbeLine(self, from, to, callback, 0, &hit);
    if (result != 0) {
        return 0;
    }

    dir[0] = to[0] - from[0];
    dir[1] = to[1] - from[1];
    dir[2] = to[2] - from[2];
    fn_800E0CB8(dir, dir);
    ActorAim(aim, (Actor*)self, 2);

    side[0] = CrossTerm(dir[1], aim[2], dir[2], aim[1]);
    side[1] = CrossTerm(dir[2], aim[0], dir[0], aim[2]);
    side[2] = CrossTerm(dir[0], aim[1], dir[1], aim[0]);
    up[0] = CrossTerm(side[1], dir[2], side[2], dir[1]);
    up[1] = CrossTerm(side[2], dir[0], side[0], dir[2]);
    up[2] = CrossTerm(side[0], dir[1], side[1], dir[0]);

    fn_8009F724(mat, side, 0);
    fn_8009F724(mat, dir, 1);
    fn_8009F724(mat, up, 2);

    offset[0] = (int)(lbl_804D6C68 * *(float*)((char*)self + 0x900) * lbl_804D6C6C);
    offset[1] = 0;
    offset[2] = 0;

    lbl_80449038[0] = mat[0];
    lbl_80449038[1] = mat[1];
    lbl_80449038[2] = mat[2];
    lbl_80449038[3] = mat[3];
    lbl_80449038[4] = mat[4];
    lbl_80449038[5] = mat[5];
    lbl_80449038[6] = mat[6];
    lbl_80449038[7] = mat[7];
    lbl_80449038[8] = mat[8];
    lbl_80449038[9] = mat[9];
    lbl_80449038[10] = mat[10];
    lbl_80449038[11] = mat[11];
    fn_8009F50C(offset, offset);

    from[0] = *(int*)((char*)self + 0x24) + offset[0];
    from[1] = *(int*)((char*)self + 0x28) + offset[1];
    from[2] = *(int*)((char*)self + 0x2c) + offset[2];
    callback = *(void**)((char*)self + 0x600);
    if (callback == 0) {
        callback = (void*)SlotOwnerMatches;
    }
    result = ProbeLine(self, from, to, callback, 0, &hit);
    if (result != 0) {
        return 0;
    }

    from[0] = *(int*)((char*)self + 0x24) - offset[0];
    from[1] = *(int*)((char*)self + 0x28) - offset[1];
    from[2] = *(int*)((char*)self + 0x2c) - offset[2];
    callback = *(void**)((char*)self + 0x600);
    if (callback == 0) {
        callback = (void*)SlotOwnerMatches;
    }
    result = ProbeLine(self, from, to, callback, 0, &hit);
    if (result != 0) {
        return 0;
    }

    return 1;
}

// An owner's position is stored as three signed integers at +0x24..+0x2C.
struct Owner {
    char pad0[0x24];
    int x;      // 0x24
    int y;      // 0x28
    int z;      // 0x2C
};

// Distance between an owner's integer position and a float point.  Falls back
// to the shared zero constant when either side is null or the point sits on
// the owner.
static inline float OwnerValueDistance(Owner* owner, Vec3* vec)
{
    float dx;
    float dy;
    float dz;
    float dist;

    if (owner == 0 || vec == 0) {
        return lbl_804D6C34;
    }
    dx = vec->x - (float)owner->x;
    dy = vec->y - (float)owner->y;
    dz = vec->z - (float)owner->z;
    dist = dy * dy + dx * dx + dz * dz;
    if (dist > lbl_804D6C34) {
        return sqrtf(dist);
    }
    return lbl_804D6C34;
}

float OwnerValue(Owner* owner, Vec3* vec);

// Distance from an owner to a point whose position is three packed integers at
// +0x24, +0x28 and +0x2C.
float OwnerValue(Owner* owner, void* value)
{
    Vec3 vec;

    if (owner == 0 || value == 0) {
        return lbl_804D6C34;
    }
    vec.x = (float)*(int*)((char*)value + 0x24);
    vec.y = (float)*(int*)((char*)value + 0x28);
    vec.z = (float)*(int*)((char*)value + 0x2C);
    return OwnerValueDistance(owner, &vec);
}

// Distance from an owner's integer position to a float point.
float OwnerValue(Owner* owner, Vec3* vec)
{
    return OwnerValueDistance(owner, vec);
}

// Distance helper in another unit (still an fn_ placeholder).  It takes a
// three-component integer vector and returns an integer length.
extern "C" int fn_800E0DBC(int* delta);

// Distance between two integer positions held by the object at +0x40, with the
// vertical component flattened, scaled by the shared conversion constant.
float RangeValueDistance2D(void* self)
{
    int* pos = *(int**)((char*)self + 0x40);
    int delta[3];

    delta[0] = pos[3] - pos[0];
    delta[1] = pos[4] - pos[1];
    delta[2] = pos[5] - pos[2];
    delta[2] = 0;
    return lbl_804D6C6C * (float)fn_800E0DBC(delta);
}

// Engine RNG (mangled C++ name Rand__Fv), declared through its real prototype.
int Rand();

// Scatter a position by three random offsets in [-range/2, range/2).
void JitterPositionRange(float* pos, int range)
{
    if (range > 0) {
        int half = range / 2;

        pos[0] += (float)(Rand() % range - half);
        pos[1] += (float)(Rand() % range - half);
        pos[2] += (float)(Rand() % range - half);
    }
}

// Result block of the vertical probe (0x48 bytes, filled by fn_8001F928).
struct ProbeHit {
    int id;         // 0x00
    int pad04[2];   // 0x04
    int value;      // 0x0c
    int pad10[10];  // 0x10
    int height;     // 0x38
    int x;          // 0x3c
    int y;          // 0x40
    int z;          // 0x44
};

// Segment cast by the vertical probe: from 0x20 above the point down to
// 0x8000 below it.
struct ProbeSegment {
    int start[3];
    int end[3];
};

int ZoneProbeHitQuery(int* src, int* dst);

// Cast a vertical line through pos and return the height of the first hit; on
// a miss the bottom of the segment (pos[2] - 0x8000) is returned.  outPos,
// outValue and outId are optional; outColor receives 0, 0, 0, 0xff.
float ZoneCastVerticalColor(void* self, int* pos, int mask, Vec3* outPos,
                            int* outValue, int* outId, unsigned char* outColor)
{
    ProbeHit hit;
    ProbeSegment seg;
    float height;

    if (outColor != 0) {
        outColor[0] = 0;
        outColor[1] = 0;
        outColor[2] = 0;
        outColor[3] = 0xff;
    }
    memset(&hit, 0, sizeof(hit));
    seg.start[0] = pos[0];
    seg.start[1] = pos[1];
    seg.start[2] = pos[2] + 0x20;
    seg.end[0] = seg.start[0];
    seg.end[1] = seg.start[1];
    seg.end[2] = pos[2] - 0x8000;
    if (fn_8001F928(self, &seg, mask | 0x10000000, (void*)ZoneProbeHitQuery, &hit) != 0) {
        if (outPos != 0) {
            outPos->x = (float)hit.x;
            outPos->y = (float)hit.y;
            outPos->z = (float)hit.z;
        }
        if (outValue != 0) {
            *outValue = hit.value;
        }
        if (outId != 0) {
            *outId = hit.id;
        }
        height = (float)hit.height;
    } else {
        height = (float)(pos[2] - 0x8000);
    }
    return height;
}

// Same vertical cast as ZoneCastVerticalColor, without the colour output.
float ZoneCastVertical(void* self, int* pos, int mask, Vec3* outPos,
                       int* outValue, int* outId)
{
    ProbeHit hit;
    ProbeSegment seg;
    float height;

    memset(&hit, 0, sizeof(hit));
    seg.start[0] = pos[0];
    seg.start[1] = pos[1];
    seg.start[2] = pos[2] + 0x20;
    seg.end[0] = seg.start[0];
    seg.end[1] = seg.start[1];
    seg.end[2] = pos[2] - 0x8000;
    if (fn_8001F928(self, &seg, mask | 0x10000000, (void*)ZoneProbeHitQuery, &hit) != 0) {
        if (outPos != 0) {
            outPos->x = (float)hit.x;
            outPos->y = (float)hit.y;
            outPos->z = (float)hit.z;
        }
        if (outValue != 0) {
            *outValue = hit.value;
        }
        if (outId != 0) {
            *outId = hit.id;
        }
        height = (float)hit.height;
    } else {
        height = (float)(pos[2] - 0x8000);
    }
    return height;
}

// Angle range constants used to wrap a bearing into canonical form.
extern float lbl_804D6C70;
extern float lbl_804D6C74;
extern float lbl_804D6C78;
extern float lbl_804D6C7C;

// Game math helper: angle of the vector (x, y), declared in another unit.
float fn_800A1128(float y, float x);

// Bearing from an actor's aim direction to a world position.  The actor keeps
// integer coordinates at +0x24..+0x2c; the aim direction comes from the engine
// and is reduced to its Z euler term before being subtracted.  Returns zero for
// a null actor or a position already inside the tolerance.
float ActorAimAngleToPos(void* self, Vec3* pos)
{
    Vec3  dir;
    Vec3  aimVec;
    int   aim[3];
    Vec3  euler;
    float angle;
    float inv;

    if (self == 0) {
        return lbl_804D6C34;
    }

    dir.x = (float)*(int*)((char*)self + 0x24);
    dir.y = (float)*(int*)((char*)self + 0x28);
    dir.z = (float)*(int*)((char*)self + 0x2c);
    if (((fabsf(dir.x - pos->x) > lbl_804D6C40) +
         (fabsf(dir.y - pos->y) > lbl_804D6C40) +
         (fabsf(dir.z - pos->z) > lbl_804D6C40)) == 0) {
        return lbl_804D6C34;
    }

    dir.x = pos->x - dir.x;
    dir.y = pos->y - dir.y;
    dir.z = pos->z - dir.z;
    if (((fabsf(dir.x) > lbl_804D6C40) + (fabsf(dir.y) > lbl_804D6C40) +
         (fabsf(dir.z) > lbl_804D6C40)) == 0) {
        dir.x = lbl_804D6C34;
        dir.y = lbl_804D6C34;
        dir.z = lbl_804D6C34;
    } else {
        inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        dir.x *= inv;
        dir.y *= inv;
        dir.z *= inv;
    }

    angle = fn_800A1128(dir.y, dir.x) - lbl_804D6C70;

    ActorAim(aim, (Actor*)self, 1);
    aimVec.x = (float)aim[0];
    aimVec.y = (float)aim[1];
    aimVec.z = (float)aim[2];
    if (((fabsf(aimVec.x) > lbl_804D6C40) +
         (fabsf(aimVec.y) > lbl_804D6C40) +
         (fabsf(aimVec.z) > lbl_804D6C40)) == 0) {
        aimVec.x = lbl_804D6C34;
        aimVec.y = lbl_804D6C34;
        aimVec.z = lbl_804D6C34;
    } else {
        inv = 1.0f / sqrtf(aimVec.x * aimVec.x + aimVec.y * aimVec.y +
                          aimVec.z * aimVec.z);
        aimVec.x *= inv;
        aimVec.y *= inv;
        aimVec.z *= inv;
    }

    fn_8016D440(&euler, &aimVec);
    angle -= euler.z;

    if (angle < lbl_804D6C74) {
        angle += lbl_804D6C78;
    } else if (angle >= lbl_804D6C7C) {
        angle -= lbl_804D6C78;
    }
    return angle;
}

// Bearing from an actor to a world position, relative to the heading kept at
// +0x254.  The actor keeps integer coordinates at +0x24..+0x2c.  Returns zero
// when the position is already inside the tolerance.
float ActorAngleToPos(void* self, Vec3* pos)
{
    Vec3  dir;
    float angle;
    float inv;

    dir.x = (float)*(int*)((char*)self + 0x24);
    dir.y = (float)*(int*)((char*)self + 0x28);
    dir.z = (float)*(int*)((char*)self + 0x2c);
    if (((fabsf(dir.x - pos->x) > lbl_804D6C40) +
         (fabsf(dir.y - pos->y) > lbl_804D6C40) +
         (fabsf(dir.z - pos->z) > lbl_804D6C40)) == 0) {
        return lbl_804D6C34;
    }

    dir.x = pos->x - dir.x;
    dir.y = pos->y - dir.y;
    dir.z = pos->z - dir.z;
    if (((fabsf(dir.x) > lbl_804D6C40) + (fabsf(dir.y) > lbl_804D6C40) +
         (fabsf(dir.z) > lbl_804D6C40)) == 0) {
        dir.x = lbl_804D6C34;
        dir.y = lbl_804D6C34;
        dir.z = lbl_804D6C34;
    } else {
        inv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        dir.x *= inv;
        dir.y *= inv;
        dir.z *= inv;
    }

    angle = fn_800A1128(dir.y, dir.x) - lbl_804D6C70;
    angle -= *(float*)((char*)self + 0x254);

    if (angle < lbl_804D6C74) {
        angle += lbl_804D6C78;
    } else if (angle >= lbl_804D6C7C) {
        angle -= lbl_804D6C78;
    }
    return angle;
}

// Distance between two 2D points; the squared difference sum is offset by the
// shared zero constant and square-rooted only when it is positive.  Names are
// descriptive guesses.
float Distance2D(float* a, float* b)
{
    float dx = a[0] - b[0];
    float dy = a[1] - b[1];
    float dist = lbl_804D6C34 + (dx * dx + dy * dy);
    float result = lbl_804D6C34;

    if (dist > lbl_804D6C34) {
        result = sqrtf(dist);
    }
    return result;
}

// Same as Distance2D, over all three components.
float Distance3D(float* a, float* b)
{
    float dx = a[0] - b[0];
    float dy = a[1] - b[1];
    float dz = a[2] - b[2];
    float dist = dx * dx + dy * dy + dz * dz;
    float result = lbl_804D6C34;

    if (dist > lbl_804D6C34) {
        result = sqrtf(dist);
    }
    return result;
}

// Squared planar distance between two points, biased by a tuning constant.
// Name is a descriptive guess.
float Vec2DistSq(float* a, float* b)
{
    float dx = a[0] - b[0];
    float dy = a[1] - b[1];

    return dx * dx + dy * dy + lbl_804D6C34;
}

// Squared 3D distance between two points.  Name is a descriptive guess.
float Vec3DistSq(float* a, float* b)
{
    float dx = a[0] - b[0];
    float dy = a[1] - b[1];
    float dz = a[2] - b[2];

    return dx * dx + dy * dy + dz * dz;
}

extern float lbl_804D6C6C;

// 2D bearing and "corner" distance from an actor's integer position to `pos`.
// The bearing is taken relative to the heading kept at +0x254 and wrapped into
// [-pi, pi); the returned distance is the sum of the axis deltas less the
// smaller delta scaled by a shared factor.  Names are descriptive guesses.
void ActorAimOffset2D(void* self, float* pos, float* outAngle, float* outDist)
{
    float dx = (float)*(int*)((char*)self + 0x24) - pos[0];
    float dy = (float)*(int*)((char*)self + 0x28) - pos[1];
    float adx;
    float ady;
    float mn;

    if (outAngle != 0) {
        *outAngle = *(float*)((char*)self + 0x254) - fn_800A1128(dy, dx) -
                    lbl_804D6C70;
        while (*outAngle < lbl_804D6C74) {
            *outAngle += lbl_804D6C78;
        }
        while (*outAngle > lbl_804D6C7C) {
            *outAngle -= lbl_804D6C78;
        }
    }

    adx = (dx < lbl_804D6C34) ? -dx : dx;
    ady = (dy < lbl_804D6C34) ? -dy : dy;
    mn = (adx < ady) ? adx : ady;
    *outDist = (adx + ady) - mn * lbl_804D6C6C;
}

extern float lbl_804D6C80;

// 3D version of ActorAimOffset2D: the bearing is computed from the XY deltas,
// wrapped into [0, 2*pi] and then offset, while the distance combines all three
// axis deltas with the largest of the three scaled by a shared factor.  Names
// are descriptive guesses.
void ActorAimOffset3D(void* self, float* pos, float* outAngle, float* outDist)
{
    float dx = pos[0] - (float)*(int*)((char*)self + 0x24);
    float dy = pos[1] - (float)*(int*)((char*)self + 0x28);
    float dz = pos[2] - (float)*(int*)((char*)self + 0x2c);
    float angle;
    float adx;
    float ady;
    float adz;
    float mn;

    angle = *(float*)((char*)self + 0x254) -
            (fn_800A1128(dy, dx) + lbl_804D6C70);
    while (angle < lbl_804D6C34) {
        angle += lbl_804D6C78;
    }
    while (angle >= lbl_804D6C78) {
        angle -= lbl_804D6C78;
    }
    *outAngle = angle - lbl_804D6C7C;

    if (dx < lbl_804D6C34) {
        dx = -dx;
    }
    if (dy < lbl_804D6C34) {
        dy = -dy;
    }
    if (dz < lbl_804D6C34) {
        dz = -dz;
    }
    adx = dx;
    ady = dy;
    adz = dz;
    mn = (adx < ady) ? adx : ady;
    if (mn >= adz) {
        adz = mn;
    }
    *outDist = (adx + ady + adz) - adz / lbl_804D6C80;
}

// Probe callback for the vertical cast: let the hit object's query method
// (at +0x30, message 0x400) fill in its status, drop it when it reports state 2
// or 4, and copy the 0x48-byte hit record to the destination block.
struct ProbeQuery {
    int value;          // 0x00
    unsigned int status; // 0x04
    int pad[6];         // 0x08
};

typedef void (*ProbeQueryMethod)(int, ProbeQuery*);

int ZoneProbeHitQuery(int* src, int* dst)
{
    ProbeQuery q;
    void* object = (void*)src[0];
    int state;

    if (object != 0) {
        q.value = (int)object;
        (*(ProbeQueryMethod*)((char*)object + 0x30))(0x400, &q);
        state = (int)(q.status >> 24);
        if (state == 2 || state == 4) {
            return 0;
        }
    }
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
    dst[10] = src[10];
    dst[11] = src[11];
    dst[12] = src[12];
    dst[13] = src[13];
    dst[14] = src[14];
    dst[15] = src[15];
    dst[16] = src[16];
    dst[17] = src[17];
    return 1;
}

// Filter/transform callback: when the context block carries an optional test
// function at +0x0c, run it with the context value at +0x10 and reject the hit
// if it fails.  Otherwise convert the three fixed-point position words of the
// object at +0x30..+0x38 to floats.
int ZoneProbeHitFilter(int self, float* out)
{
    int (*test)(int, int);

    test = (int (*)(int, int))*(void**)((char*)out + 0xc);
    if (test == 0 || test(self, *(int*)((char*)out + 0x10)) != 0) {
        out[0] = (float)*(int*)(self + 0x30);
        out[1] = (float)*(int*)(self + 0x34);
        out[2] = (float)*(int*)(self + 0x38);
        return 1;
    }
    return 0;
}

// Engine string object: owns a heap buffer and is destroyed through its
// Buffer base.
class Buffer {
public:
    virtual ~Buffer();
};

class String : public Buffer {
public:
    String();
};

// Answer buffer for the owner query below.  The queried object reports the
// hash it resolved and the handle of the owner it belongs to.
struct OwnerQuery {
    int unk00;   // 0x00
    int hash;    // 0x04
    int unk08;   // 0x08
    int result;  // 0x0c
};

// False when the slot's object is the given owner, or when the owner query
// says both objects belong to the same owner.  True otherwise.
int SlotOwnerMatches(void** slot, void* owner)
{
    void* cur = *slot;

    if (cur == 0 || cur != owner) {
        if (owner != 0 && cur != 0) {
            String name;
            OwnerQuery query;
            int ok;

            query.hash = 0x717e6e02;
            query.unk08 = 0;
            query.result = 0;
            ok = SendMsg(*(int*)((char*)cur + 0x38), 0x41e, &query);
            (void)name;
            if (ok != 0 && query.result == *(int*)((char*)owner + 0x38)) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

// Handle table lookups implemented in another unit (still fn_ placeholders).
extern "C" int fn_800A1F94(unsigned int key);
extern "C" int fn_800A3990(unsigned int key);

typedef int (*HandleQueryMethod)(void* arg, int* out);

// Resolve a handle through the global tables, then dispatch its query method.
int HandleFindAndCall(unsigned int id, void* arg, int* out)
{
    unsigned int key = (id & 0xFFFFFF) | 0x13000000;
    HandleQueryMethod method;
    void* object;

    if (fn_800A1F94(key) == 0) {
        return 0;
    }
    *out = fn_800A3990(key);
    object = (void*)*out;
    if (object != 0) {
        method = *(HandleQueryMethod*)((char*)object + 0x30);
        return method(arg, out);
    }
    return 0;
}

// Dispatch a message to a game object: the tag checks come first, then the
// object's two scripted callback slots, then the generic message handler.
// Callbacks return -999999 to mean "handled, stop".
extern "C" int fn_800AB604(int msg, int* data);
extern "C" int fn_800AD314(int object, int* data);
extern "C" int fn_800BA84C(int type);

int ActorMessageDispatch(int msg, int* data)
{
    int object = *data;
    int result;

    if ((msg == 0 || msg == 9 || msg == 0x3300) &&
        fn_800BA7FC(0x36) != 0 &&
        (*(int*)((char*)object + 0x38) & 0xFFFFFF) != fn_800BA84C(4)) {
        return 1;
    }
    if (msg == 0x41e && data[1] == (int)0xAD8576FA) {
        return fn_800AD314(object, data);
    }
    result = 0;
    if (*(int (**)(int, int*))((char*)object + 0x120) != 0) {
        result = (*(int (**)(int, int*))((char*)object + 0x120))(msg, data);
        if (result == -999999) {
            return 0;
        }
    }
    if (result == 0 && *(int (**)(int, int*))((char*)object + 0x124) != 0) {
        result = (*(int (**)(int, int*))((char*)object + 0x124))(msg, data);
        if (result == -999999) {
            return 0;
        }
    }
    if (result == 0) {
        result = fn_800AB604(msg, data);
        if (result == -999999) {
            result = 0;
        }
    }
    return result;
}

// Resolve which object a message refers to: pick the handle from the kind hash
// in the payload, ask that object for its owner, and write the result back
// into the payload (handle at +4, owner at +0).  Other kinds are unanswered.
extern "C" int fn_800B9360(int, int, int, int);
extern "C" int fn_800B9104(int, int, int, int);
extern "C" int fn_800B8D68(int, int, int);
extern "C" int fn_8029B560(unsigned int, int);

int MsgObjectResolve(int, int msg, int* data)
{
    unsigned int info[8];

    switch (msg) {
    case 0x180D: {
        int handle = -1;

        switch (data[3]) {
        case 0x5E71DA39:
            handle = fn_800B9360(data[2], 0, data[4], data[5]);
            break;
        case 0x66B631DB:
            handle = fn_800B9104(data[2], 0, data[4], data[5]);
            break;
        case -0x705A273D:
            handle = fn_800B8D68(data[2], 0, data[5]);
            break;
        }
        if (SendMsg(handle, 0x400, info) != 0 && info[0] != 0) {
            data[1] = handle;
            *data = info[0];
            return 1;
        }
        data[1] = -1;
        return 0;
    }
    case 0x41D:
        switch (data[1]) {
        case 0x64BB5E72:
            fn_8029B560(0x73B1E248, data[3]);
            return 1;
        case -0x142BF57:
            fn_8029B560(0x86A69297, data[3]);
            return 1;
        }
        return 0;
    }
    return 0;
}

// TODO fn_800AB604 (0x1D10 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" int fn_800AB604(int msg, int* data) { return 0; }

// TODO fn_800AD314 (0xB90 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" int fn_800AD314(int object, int* data) { return 0; }

// TODO fn_800ADEA4 (0x204 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800ADEA4(void) {}

// TODO fn_800AE0A8 (0x8EC bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800AE0A8(void) {}

// TODO fn_800AE994 (0x1D4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800AE994(void) {}

// TODO fn_800AEB68 (0x374 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800AEB68(void) {}

// TODO fn_800AEEDC (0x2BC bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800AEEDC(void) {}

// TODO fn_800AF198 (0x438 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800AF198(void) {}

// TODO fn_800AF5D0 (0x750 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800AF5D0(void) {}

// TODO fn_800AFD20 (0x28 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800AFD20(void) {}

// TODO fn_800AFD48 (0x448 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800AFD48(void) {}

// TODO fn_800B0190 (0xCD8 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B0190(void) {}

// TODO fn_800B0E68 (0x690 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B0E68(void) {}

// TODO fn_800B14F8 (0x33C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_800B14F8(void) {}

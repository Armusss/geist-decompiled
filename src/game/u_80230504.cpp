// u_80230504.cpp: the "extra" body-trail system (0x80230504-0x80231550).
// An ExtraHolder can carry a short-lived copy of an actor's moving body: the owner sends it
// positions and motion each frame, they are collected in a four-slot ring buffer, and when the
// effect is switched on the averaged path is handed to the actor-system trail.
// .rodata holds the shared ghost action names and skeleton part names (unused here) plus the
// "RD_HUMAN_MALE_TYPE1" model name used as the effect's animation key.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Mtx34 {
    float m[3][4];
};

struct Actor {
    char pad0[0x3C];
    int f3C;
    int f40;
    char pad44[8];
    u32 flags;
};

struct OwnerInfo {
    char pad0[0x8F8];
    int f8F8;
    float f8FC;
};

struct ExtraRecord {
    s16 time;    // 0x00
    u8 count;    // 0x02
    u8 flag;     // 0x03
    Vec3 pos;    // 0x04
    Vec3 vel;    // 0x10
};

struct ExtraHolder {
    u32 handle;        // 0x00
    int model;         // 0x04
    void* actor;       // 0x08
    int hash;          // 0x0C
    char* name;        // 0x10
    float life;        // 0x14
    int state;         // 0x18
    ExtraRecord* recs; // 0x1C
};

struct ExtraMsg {
    char pad0[0x14];
    u32 type;     // 0x14
    IVec3 pos;    // 0x18
    char pad24[0xC];
    IVec3 vel;    // 0x30
    char pad3C[0x10];
    float dt;     // 0x4C
    float scale;  // 0x50
};

struct ActorMsg {
    void* actor;      // 0x00
    char pad04[0x1C]; // generic message payload
};

struct ExtraList {
    char pad0[8];
    u32 size;     // 0x08
    void** data;  // 0x0C
};

struct PlaceF;

// The holder list, shared with the neighbouring file that owns its class.
ExtraList* lbl_804D5440;
int lbl_804D5444;
extern float lbl_804D3488;
extern int lbl_804D348C;
extern float lbl_804D3490;

extern "C" int SendMsg(int to, int msg, void* data);
extern "C" void* memcpy(void* dst, const void* src, u32 n);
extern "C" void* memset(void* dst, int val, u32 n);

void* VAlloc(int size);
void VFree(void* p);
void* OwnerGetInfo(u32 handle);
int ModelAnimate(int model, char* name, int a);
void ActorGetMatrix(Mtx34* m, Actor* a);
void PlacementToMatrix(Mtx34* m, PlaceF* p);

extern "C" {
void fn_800E1098(float scale, Vec3* out, Vec3* in);              // 0x800E1098
int fn_8003A1F0(int model, char* name, int anim);                // 0x8003A1F0
void fn_80039958(int model, int a, int b, int c, int d);         // 0x80039958
void fn_801E0EDC(void* actor, Vec3* a, Vec3* b, int kind, float scale); // 0x801E0EDC
void fn_801E0FD0(void** actor);                                  // 0x801E0FD0
int fn_801E10D8(void* actor);                                    // 0x801E10D8
void fn_801E1150(void* actor, int flag);                         // 0x801E1150
void fn_801E11C0(void* actor, int anim, Mtx34* m, char* name);   // 0x801E11C0
void* fn_801E1230(int model, u32 hash, int anim, Mtx34* m, char* name, int extra); // 0x801E1230
void fn_8028B35C(void* list, void* pos, int size);               // 0x8028B35C
void* fn_8028B3BC(void* list, void* pos, int size);              // 0x8028B3BC
}

// From a shared header: names of the ghost actions and of the human skeleton parts (unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN", "AUTO",
};
static const char* sBodyPartNames[] = {
    "FULL BODY", "PELVIS", "SPINE1", "NECK", "LTHIGH", "LCALF",
    "RTHIGH", "RCALF", "LUPPERARM", "LFOREARM", "RUPPERARM", "RFOREARM",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    u32 h;
    if (s == 0) {
        h = 0;
    } else {
        int i;
        h = 0;
        for (i = 0; s[i] != 0; i++) {
            h = h * 31 + (unsigned char)toupper(s[i]);
        }
        h %= 0xFFFFFFFF;
    }
    return h;
}

// Removes the holder from the live list if present.
inline void ExtraListRemove(ExtraHolder* e) {
    ExtraHolder** p = (ExtraHolder**)lbl_804D5440->data;
    ExtraHolder** end = (ExtraHolder**)((char*)p + lbl_804D5440->size);
    for (; p != end; p++) {
        if (e == *p) {
            fn_8028B35C(lbl_804D5440, p, 4);
            break;
        }
    }
}

// Adds the holder to the live list if it is not there yet.
inline void ExtraListAdd(ExtraHolder* e) {
    ExtraHolder** p = (ExtraHolder**)lbl_804D5440->data;
    ExtraHolder** end = (ExtraHolder**)((char*)p + lbl_804D5440->size);
    int found = 0;
    for (; p != end; p++) {
        if (*p == e) {
            found = 1;
            break;
        }
    }
    if (!found) {
        *(ExtraHolder**)fn_8028B3BC(lbl_804D5440, end, 4) = e;
    }
}

// Appends the holder to the live list.
inline void ExtraListAppend(ExtraHolder* e) {
    ExtraHolder** end = (ExtraHolder**)((char*)lbl_804D5440->data + lbl_804D5440->size);
    *(ExtraHolder**)fn_8028B3BC(lbl_804D5440, end, 4) = e;
}

// Switches the trail off.
inline void ExtraTrailStop(ExtraHolder* e) {
    if (e && e->actor && e->state == 1) {
        fn_801E1150(e->actor, 0);
        e->state = 2;
        ExtraListRemove(e);
    }
}

// Ages one buffered sample; clears it once it is too old.
inline void ExtraTrailRecTick(ExtraRecord* r, float dt, int* any) {
    if (r->time >= 0 && r->time < 100) {
        r->time += (int)dt;
        if (r->time > 4) {
            r->time = -1;
        } else {
            *any = 1;
        }
    }
}

// Switches the trail on.
inline void ExtraTrailStart(ExtraHolder* e) {
    if (e && e->actor && e->state != 1) {
        fn_801E1150(e->actor, 1);
        e->state = 1;
        ExtraListAdd(e);
    }
}

void ExtraTrailApply(ExtraHolder* e, ExtraMsg* m) {
    ExtraRecord* recs;
    ExtraRecord* r;
    Vec3 pos;
    Vec3 vel;
    int i;
    int kind;
    float scale;
    float life;
    bool bright = false;

    if (!e) {
        return;
    }
    if (fabsf(m->dt) < 1.1920929e-7f) {
        return;
    }
    switch (m->type) {
    case 0x672F35C2:
    case 0x20EE36:
    case 0xC8759B80:
    case 0x12DD2E2A:
    case 0x6CA5F172:
        return;
    case 0xB9FC42F5:
    case 0x6A5089FD:
        life = lbl_804D3488;
        bright = true;
        kind = lbl_804D348C;
        scale = 48.0f;
        break;
    default:
    case 0x75715902:
        life = lbl_804D3490;
        kind = lbl_804D5444;
        scale = 18.0f;
        break;
    }
    if ((e->handle >> 24) == 0x13) {
        OwnerInfo* o = (OwnerInfo*)OwnerGetInfo(e->handle);
        if (o == 0) {
            return;
        }
        if (o->f8FC >= 1000.0f) {
            return;
        }
        if (o->f8F8 == 6) {
            return;
        }
    }
    pos.x = (float)m->pos.x;
    pos.y = (float)m->pos.y;
    pos.z = (float)m->pos.z;
    vel.x = (float)m->vel.x;
    vel.y = (float)m->vel.y;
    vel.z = (float)m->vel.z;
    life *= m->dt;
    if (fabsf(vel.x) > 1.1920929e-7f || fabsf(vel.y) > 1.1920929e-7f || fabsf(vel.z) > 1.1920929e-7f) {
        float d = 1.0f / sqrtf(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);
        vel.x *= d;
        vel.y *= d;
        vel.z *= d;
    } else {
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
    }
    if (m->scale > 1.1920929e-7f) {
        fn_800E1098(1.0f, &vel, &vel);
        if (vel.z < m->scale) {
            vel.z = m->scale;
        }
    }
    fn_800E1098(life, &vel, &vel);
    if (e->actor) {
        if (e->state != 1) {
            ExtraTrailStart(e);
        }
        fn_801E0EDC(e->actor, &pos, &vel, kind, scale);
    } else {
        recs = e->recs;
        if (!recs) {
            recs = (ExtraRecord*)VAlloc(0x70);
            e->recs = recs;
            if (!recs) {
                return;
            }
            recs[0].time = -1;
            recs[1].time = -1;
            recs[2].time = -1;
            recs[3].time = -1;
        }
        i = -1;
        {
            s16 last = 0;
            int j;
            for (j = 0; j < 4; j++) {
                s16 t = recs[j].time;
                if (t == 0) {
                    i = j;
                    break;
                }
                if (last >= 0) {
                    if (t < 0) {
                        i = j;
                        last = -1;
                    } else if (t > last) {
                        i = j;
                        last = t;
                    }
                }
            }
        }
        if (i >= 0) {
            r = &recs[i];
            if (r->time == 0) {
                r->count++;
                r->pos.x += pos.x;
                r->pos.y += pos.y;
                r->pos.z += pos.z;
                r->vel.x += vel.x;
                r->vel.y += vel.y;
                r->vel.z += vel.z;
                r->flag = bright;
            } else {
                r->count = 1;
                r->time = 0;
                r->pos = pos;
                r->vel = vel;
                if (bright) {
                    r->flag = bright;
                }
            }
        }
    }
}

int ExtraTrailUpdate(ExtraHolder* e, float dt) {
    ActorMsg msg;
    ExtraRecord* r;
    int n;
    int any;
    int i;
    Mtx34 mtx;
    int anim;

    if (!e) {
        return 0;
    }
    SendMsg(e->handle, 0x400, &msg);
    if (!msg.actor) {
        return 0;
    }
    e->life += dt;
    if (!e->actor) {
        r = e->recs;
        if (r) {
            any = 0;
            for (i = 0; i < 2; i++) {
                ExtraTrailRecTick(r++, dt, &any);
                ExtraTrailRecTick(r++, dt, &any);
            }
            if (!any) {
                VFree(e->recs);
                e->recs = 0;
            }
        }
        return 0;
    }
    if (e->state == 4) {
        if ((((Actor*)msg.actor)->flags & 0x40) && (((Actor*)msg.actor)->flags & 0x80)) {
            ExtraTrailStart(e);
            return 1;
        }
        return 0;
    }
    if (e->state != 1) {
        return 0;
    }
    if (!e->name || !msg.actor) {
        return 0;
    }
    ActorGetMatrix(&mtx, (Actor*)msg.actor);
    anim = ModelAnimate(e->model, e->name, 0);
    fn_801E11C0(e->actor, anim, &mtx, e->name);
    anim = fn_8003A1F0(e->model, e->name, anim);
    PlacementToMatrix(&mtx, (PlaceF*)anim);
    fn_80039958(e->model, anim, anim, ((Actor*)msg.actor)->f3C, ((Actor*)msg.actor)->f40);
    if (fn_801E10D8(e->actor)) {
        ExtraTrailStop(e);
        e->state = 2;
        return 0;
    }
    return 1;
}

void ExtraTrailHide(ExtraHolder* e, int hide) {
    if (!e || !e->actor) {
        return;
    }
    if (hide) {
        if (e->state == 3) {
            e->state = 4;
        }
        return;
    }
    ExtraTrailStop(e);
    e->state = 3;
}

// Frees the trail's model and record buffer.
void ExtraTrailRelease(ExtraHolder* e) {
    if (!e) {
        return;
    }
    if (e->recs) {
        VFree(e->recs);
        e->recs = 0;
    }
    if (e->actor) {
        ExtraListRemove(e);
        fn_801E0FD0(&e->actor);
        e->actor = 0;
    }
    e->state = 0;
}

void ExtraTrailAttach(ExtraHolder* e) {
    ActorMsg msg;
    Mtx34 mtx;
    int anim;
    Vec3 pos;
    Vec3 vel;
    int total;
    int i;
    int kind;
    float scale;
    ExtraHolder** p;
    ExtraHolder** end;
    ExtraHolder* other;
    ExtraHolder** victim;
    ExtraRecord* r;
    float best;

    if (!e) {
        return;
    }
    if (e->actor) {
        ExtraTrailRelease(e);
    }
    SendMsg(e->handle, 0x400, &msg);
    anim = ModelAnimate(e->model, e->name, 0);
    ActorGetMatrix(&mtx, (Actor*)msg.actor);
    if (e->hash == 0) {
        e->hash = StrHash("RD_HUMAN_MALE_TYPE1");
    }
    end = (ExtraHolder**)((char*)lbl_804D5440->data + lbl_804D5440->size);
    for (p = (ExtraHolder**)lbl_804D5440->data; p != end; p++) {
        other = *p;
        if (other->actor && other->model == e->model) {
            return;
        }
    }
    e->actor = fn_801E1230(e->model, e->hash, anim, &mtx, e->name, ((Actor*)msg.actor)->f40);
    if (!e->actor) {
        return;
    }
    if (e->recs) {
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
        total = 0;
        kind = lbl_804D5444;
        scale = 18.0f;
        r = e->recs;
        for (i = 0; i < 4; i++, r++) {
            if (r->time < 0 || r->time >= 100) {
                continue;
            }
            vel.x += r->vel.x;
            vel.y += r->vel.y;
            vel.z += r->vel.z;
            pos.x += r->pos.x;
            pos.y += r->pos.y;
            pos.z += r->pos.z;
            total += r->count;
            if (r->flag) {
                kind = lbl_804D348C;
                scale = 48.0f;
            }
        }
        if (total > 0) {
            float inv = 1.0f / (float)total;
            pos.x *= inv;
            pos.y *= inv;
            pos.z *= inv;
            fn_801E0EDC(e->actor, &pos, &vel, kind, scale);
        }
        VFree(e->recs);
        e->recs = 0;
    }
    if ((lbl_804D5440->size >> 2) >= 8) {
        best = -1.0f;
        end = (ExtraHolder**)((char*)lbl_804D5440->data + lbl_804D5440->size);
        for (p = (ExtraHolder**)lbl_804D5440->data; p != end; p++) {
            other = *p;
            if (other->life > best) {
                victim = p;
                best = other->life;
            }
        }
        other = *victim;
        fn_801E0FD0(&other->actor);
        other->actor = 0;
        other->state = 2;
        fn_8028B35C(lbl_804D5440, victim, 4);
    }
    ExtraListAppend(e);
    e->state = 1;
    e->life = 0.0f;
    if (!((((Actor*)msg.actor)->flags & 0x40) && (((Actor*)msg.actor)->flags & 0x80))) {
        if (e && e->actor) {
            ExtraTrailStop(e);
            e->state = 3;
        }
    }
}

void ExtraTrailClear(ExtraHolder* e) {
    if (!e) {
        return;
    }
    ExtraTrailRelease(e);
    e->handle = -1;
    e->model = -1;
    e->name = 0;
    e->recs = 0;
    e->life = 0.0f;
    e->hash = 0;
    e->state = 0;
}

void ExtraTrailInit(ExtraHolder* e, u32 handle, u32 model, char* name, u32 hash) {
    if (!e) {
        return;
    }
    memset(e, 0, 0x20);
    e->handle = handle;
    e->model = model;
    e->name = name;
    e->hash = hash;
    e->state = 0;
}

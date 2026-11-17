// u_801D0C18.cpp: 0x801D0C18-0x801D25CC, 12 functions. The "Fire" actor type.
// Split from u_801BF578 (0x801BF578-0x801D9A70). Evidence for the boundaries:
//   actor type descriptor 0x803AD370 {0, "Fire", fn_801D12D8, fn_801D1054, fn_801D0E10, 0, fn_801D0FCC, ...}
//   string pool 0x803488D8 (POSSESS..SCAN, "Fire", the firetwo/darksmoke texture paths, "imp_flame")
//   texture table 0x803AD3A0, tables 0x803AD420/0x803AD7D0, 0x803ADB80; .sdata2 0x804D8BA0-0x804D8BC8
//   fn_801D0C18 is called by ImpFireBallSpawn
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

#include <string.h>
#include "game/types.h"
#include "game/actor.h"
#include "game/msg.h"

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

// Fires close to each other share one light: the group (VAlloc'd, 0x14 bytes) holds the light and the
// handles of up to four fires.
struct FireGroup {
    int light;                                 // 0x00 (-1: none)
    int members[4];                            // 0x04 (-1: free)
};

// Per-actor state (VAlloc'd, 0x34 bytes, at actor +0xA4).
struct Fire {
    int id;                                    // 0x00: the fire's handle
    float life;                                // 0x04
    void (*onOut)(int arg);                    // 0x08: called when the fire goes out
    float outTime;                             // 0x0C: time since it went out (-1 while burning)
    int onOutArg;                              // 0x10
    int sound;                                 // 0x14
    int emitters[4];                           // 0x18
    FireGroup* group;                          // 0x28
    float fade;                                // 0x2C
    int light;                                 // 0x30
};

// Particle emitter description (0xEC bytes); one per emitter in .data.
struct FireEmitDesc {
    Vec3 pos;                                  // 0x00
    float f[56];                               // 0x0C
};

// The emitter descriptor handed to fn_800D3D88: the description plus a second copy of the colour keys.
struct FireEmitParams {
    FireEmitDesc desc;                         // 0x00
    char padEC[0x10];
    float keys[16];                            // 0xFC
};

struct TexEntry {
    const char* path;
    unsigned long bank;
    int unk8;
    int unkC;
};

extern "C" {
int SendMsg(int to, int msg, void* data);
int fn_800C8198(void* place);
int fn_800C7E80(IVec3* pos);
void fn_800C7CE8(Actor* actor);
int fn_8001F718(Actor* a, IVec3* ray, int b, int c, int (*cb)(Actor*, int*), int* out);
int fn_8001ED98(IVec3* center, int radius, int (*cb)(Actor*, FireGroup**), void* arg, int flag);
int fn_80170834(const char* name);
int fn_8016F120(int sound, IVec3* pos);
void fn_8016EEF4(int sound);
void fn_8016EDD8(int sound, IVec3* pos);
int fn_80121BB4(Actor* a, int handle, Vec3* pos, int type, float f1, float f2, float f3, float f4);
void fn_80122448(int light, int on);
void fn_80122370(int light);
void fn_801223E4(int light, Vec3* pos);
void fn_80101F3C(TexEntry* tex, int count);
unsigned short fn_80101FC8(TexEntry* tex, int count, int index);
void fn_800D3694(int emitter);
void fn_800D3430(int emitter, int on);
void fn_800D37FC(int emitter, Actor* a, FireEmitDesc* desc, void* fade, float t);
int fn_800D3D88(FireEmitParams* p);
void fn_800D35B0(int emitter, int id, int a);
void fn_80022E8C(Actor* a);
int fn_80022D80(Actor* a);
int fn_80022FD8(Actor* a);
void fn_80022FF0(Actor* a);
void fn_800231A8(Actor* a);
int fn_800E4C64(Actor* actor, int msg, void* data, int arg);
void fn_800E48A0(Actor* actor, float dt);
int fn_800E6444(Actor* actor);
void fn_80027690(int light);
int fn_80027A58(int a, Vec3* pos, Vec4* color, int radius, int b);
void fn_800277B0(int light, Vec3* pos, int a, int* radius);
int fn_80212730(int id);
int fn_80212814(int handle, int a, int b, int c, int d, int e);
int fn_802127A0(int handle, int a, int b, void* c, int d, int e, int f);
int fn_8021284C(int handle);
int fn_80049888();
int fn_800201E4(int a, Vec3* seg, int b, int c, int d);
void fn_8000F550(unsigned short* tex, int count);
void fn_8000F738(unsigned short* tex, int count);
}
void* VAlloc(int size);
void VFree(void* p);
int ActorPlayAnim(int handle, u32 hash, int arg);

int fn_801D12D8(Actor* actor, int msg, void* data, int arg);
void fn_801D1054(Actor* actor, float dt);
int fn_801D0E10(Actor* actor);
void fn_801D0FCC(Actor* actor);
void fn_801D0FC8(Actor* actor);
void fn_801D0FC4(Actor* actor);
void fn_801D1A48(Fire* f);
void fn_801D1D3C(Fire* f);
int fn_801D2114(Actor* actor, Fire* f);
extern "C" int fn_801D20BC(Actor* a, FireGroup** out);

ActorType lbl_803AD370 = {
    0, "Fire", fn_801D12D8, fn_801D1054, fn_801D0E10, 0, fn_801D0FCC,
    {(void*)fn_801D0FC8, (void*)fn_801D0FC4, 0, 0, 0},
};

TexEntry lbl_803AD3A0[8] = {
    {"p:\\fear\\data\\particles\\firetwo\\OrangeGlow.tga", 0xF2E2BEB8, 0, 0},
    {"p:\\fear\\data\\particles\\firetwo\\fire01.tga", 0xF2E2BEB8, 0, 0},
    {"p:\\fear\\data\\particles\\firetwo\\fire02.tga", 0xF2E2BEB8, 0, 0},
    {"p:\\fear\\data\\particles\\firetwo\\fire03.tga", 0xF2E2BEB8, 0, 0},
    {"p:\\fear\\data\\particles\\firetwo\\fire04.tga", 0xF2E2BEB8, 0, 0},
    {"p:\\fear\\data\\common models\\fire_collision_box\\darksmoke.tga", 0xF2E2BEB8, 0, 0},
    {"p:\\fear\\data\\common models\\fire_collision_box\\darksmoke2.tga", 0xF2E2BEB8, 0, 0},
    {"p:\\fear\\data\\common models\\fire_collision_box\\darksmoke3.tga", 0xF2E2BEB8, 0, 0},
};

// Emitter descriptions and fade curves (not written out yet: 0x803AD420 and 0x803AD7D0).
extern FireEmitDesc lbl_803AD420[4];
extern char lbl_803AD7D0[4][0xEC];

Vec4 lbl_803ADB80 = {1.0f, 0.4f, 0.0f, 1.0f};

inline void IVecToVec(Vec3* out, IVec3* in) {
    out->x = (float)in->x;
    out->y = (float)in->y;
    out->z = (float)in->z;
}

// Moves the group's light to the centre of its burning fires (or removes it).
inline void FireGroupLight(FireGroup* g) {
    int n = 0;
    if (g != 0) {
        Vec3 c;
        int i;
        c.x = 0.0f;
        c.y = 0.0f;
        c.z = 0.0f;
        for (i = 0; i < 4; i++) {
            MsgResult r;
            if (g->members[i] != -1 && SendMsg(g->members[i], 0x400, &r) && r.p != 0 &&
                fn_80022FD8((Actor*)r.p)) {
                Actor* a = (Actor*)r.p;
                n++;
                c.y += (float)a->place.pos.y;
                c.z += (float)a->place.pos.z;
                c.x += (float)a->place.pos.x;
            }
        }
        if (n < 1) {
            if (g->light != -1) {
                fn_80027690(g->light);
                g->light = -1;
            }
        } else {
            int radius;
            float inv;
            QueryMsg m;
            int light;
            if (g->light == -1) {
                g->light = fn_80027A58(1, &c, &lbl_803ADB80, 2000, 9);
            }
            radius = (n / 4) * 2000 + 2000;
            inv = (float)(1 / n);
            c.x *= inv;
            c.y *= inv;
            c.z *= inv;
            fn_800277B0(g->light, &c, 0, &radius);
            light = g->light;
            m.hash = 0xFCC8CDD8;
            *(int*)&m.result = 1;
            m.unk8 = 0;
            SendMsg(light, 0x41D, &m);
        }
    }
}

// Places a new fire on the ground under `pos`.
extern "C" int fn_801D0C18(IVec3* pos, void (*onOut)(int arg), int arg) {
    Fire* f;
    Actor* a;
    IVec3 spawn;
    IVec3 ray[2];
    int ground;
    PlaceF place;

    if (fn_800C8198(&place) == 0) {
        return 0;
    }
    f = (Fire*)VAlloc(sizeof(Fire));
    memset(f, 0, sizeof(Fire));
    spawn = *pos;
    a = (Actor*)fn_800C7E80(&spawn);
    if (a == 0) {
        VFree(f);
        return 0;
    }
    ray[0].x = a->place.pos.x;
    ray[0].y = a->place.pos.y;
    ray[0].z = a->place.pos.z + 200;
    ray[1].x = ray[0].x;
    ray[1].y = ray[0].y;
    ray[1].z = a->place.pos.z - 100000;
    if (fn_8001F718(a, ray, 0, 0, (int (*)(Actor*, int*))0, &ground)) {
        a->place.pos.z = ground;
    }
    a->data = f;
    a->type = &lbl_803AD370;
    f->id = a->id;
    f->life = 4800.0f;
    f->onOut = onOut;
    f->onOutArg = arg;
    f->outTime = -1.0f;
    f->group = 0;
    fn_801D1D3C(f);
    f->sound = fn_8016F120(fn_80170834("imp_flame"), &a->place.pos);
    f->light = fn_80121BB4(a, a->handle, (Vec3*)pos, 0x20EE36, 0.0f, 800.0f, 10.0f, 1500.0f);
    if (f->light >= 0) {
        fn_80122448(f->light, 0);
    }
    fn_80101F3C(lbl_803AD3A0, 8);
    if (fn_801D2114(a, f) == 0) {
        fn_800C7CE8(a);
        return 0;
    }
    return 1;
}

int fn_801D0E10(Actor* actor) {
    Fire* f;
    Vec3 pos;

    if (fn_800E6444(actor) == 0) {
        actor->type = 0;
        return 0;
    }
    f = (Fire*)VAlloc(sizeof(Fire));
    memset(f, 0, sizeof(Fire));
    actor->data = f;
    actor->type = &lbl_803AD370;
    f->id = actor->id;
    f->life = 4800.0f;
    f->onOut = 0;
    f->onOutArg = 0;
    f->outTime = -1.0f;
    IVecToVec(&pos, &actor->place.pos);
    f->group = 0;
    fn_801D1D3C(f);
    f->sound = fn_8016F120(fn_80170834("imp_flame"), &actor->place.pos);
    f->light = fn_80121BB4(actor, actor->handle, &pos, 0x20EE36, 0.0f, 800.0f, 1500.0f, 10.0f);
    if (f->light >= 0) {
        fn_80122448(f->light, 0);
    }
    fn_80101F3C(lbl_803AD3A0, 8);
    if (fn_801D2114(actor, f) == 0) {
        fn_800C7CE8(actor);
        return 0;
    }
    return 1;
}

// Ground probe callback of fn_801D0C18.
int fn_801D0FA4(Actor* a, int* out) {
    out[0] = a->handle;
    out[1] = ((int*)a)[3];
    out[2] = -1;
    return 1;
}

void fn_801D0FC4(Actor* actor) {}

void fn_801D0FC8(Actor* actor) {}

void fn_801D0FCC(Actor* actor) {
    Fire* f = (Fire*)actor->data;
    int i;

    for (i = 0; i < 4; i++) {
        fn_800D3694(f->emitters[i]);
    }
    if (f->sound != -1) {
        fn_8016EEF4(f->sound);
        f->sound = -1;
    }
    fn_80122370(f->light);
    fn_801D1A48(f);
    VFree(f);
}

inline int ActorIsHidden(Actor* a) {
    if (a != 0 && ((a->flags4C & 0x40) || (a->flags4C & 1) || (a->handle >> 24) == 0xC)) {
        return 1;
    }
    return 0;
}

void fn_801D1054(Actor* actor, float dt) {
    Fire* f = (Fire*)actor->data;
    int i;

    if (f->emitters[0] == -1) {
        fn_801D2114(actor, f);
    }
    if (f->life > 0.0f) {
        if (f->fade < 1.0f - f->life / 4800.0f) {
            float max;
            FireEmitDesc* desc;
            char* fade;
            f->fade = 0.001f * dt + f->fade;
            max = 1.0f - f->life / 4800.0f;
            if (f->fade < max) {
                max = f->fade;
            }
            f->fade = max;
            fade = lbl_803AD7D0[0];
            desc = lbl_803AD420;
            for (i = 0; i < 4; i++) {
                fn_800D37FC(f->emitters[i], actor, desc, fade, f->fade);
                desc++;
                fade += 0xEC;
            }
        }
        {
            Vec3 pos;
            IVecToVec(&pos, &actor->place.pos);
            fn_801223E4(f->light, &pos);
        }
    } else if (f->outTime < 0.0f) {
        f->outTime = 0.0f;
        for (i = 0; i < 4; i++) {
            fn_800D3430(f->emitters[i], 0);
        }
        if (f->sound != -1) {
            fn_8016EEF4(f->sound);
            f->sound = -1;
        }
        fn_80122370(f->light);
        fn_801D1A48(f);
        if (f->onOut != 0) {
            f->onOut(f->onOutArg);
        }
    } else {
        f->outTime += dt;
        if (f->outTime > 6000.0f) {
            for (i = 0; i < 4; i++) {
                fn_800D3694(f->emitters[i]);
            }
            fn_800C7CE8(actor);
            return;
        }
    }
    fn_80022E8C(actor);
    fn_800E48A0(actor, dt);
    if (!ActorIsHidden(actor) && f->light >= 0) {
        fn_80122448(f->light, 0);
    }
}

int fn_801D12D8(Actor* self, int msg, void* data, int arg) {
    int* d = (int*)data;
    Actor* actor = (Actor*)d[0];
    Fire* f = (Fire*)actor->data;
    int i;

    switch (msg) {
    case 0x415: {
        int r;
        if (d[1] != -1) {
            if (d[3] == 0) {
                r = fn_80212814(actor->handle, d[1], d[2], 1, 0, 0);
            } else {
                r = fn_802127A0(actor->handle, d[1], d[2], d + 4, 1, 0, 0);
            }
            fn_801D1A48(f);
            fn_801D1D3C(f);
            SendMsg(f->group->light, 0x415, data);
            return r;
        }
        return fn_8021284C(actor->handle);
    }
    case 0:
    case 1:
    case 2:
    case 5:
        return 0;
    case 6: {
        unsigned short tex[9];
        if (f->light >= 0) {
            if (d[1] == 1 && fn_80022D80(actor)) {
                fn_80122448(f->light, 1);
            } else {
                fn_80122448(f->light, 0);
            }
        }
        tex[0] = fn_80101FC8(lbl_803AD3A0, 8, 0);
        tex[1] = fn_80101FC8(lbl_803AD3A0, 8, 0);
        tex[2] = fn_80101FC8(lbl_803AD3A0, 8, 5);
        tex[3] = fn_80101FC8(lbl_803AD3A0, 8, 6);
        tex[4] = fn_80101FC8(lbl_803AD3A0, 8, 7);
        tex[5] = fn_80101FC8(lbl_803AD3A0, 8, 1);
        tex[6] = fn_80101FC8(lbl_803AD3A0, 8, 2);
        tex[7] = fn_80101FC8(lbl_803AD3A0, 8, 3);
        tex[8] = fn_80101FC8(lbl_803AD3A0, 8, 4);
        if (d[1] == 0) {
            fn_8000F550(tex, 9);
        } else {
            fn_8000F738(tex, 9);
        }
        break;
    }
    case 0x407: {
        Msg400* m = (Msg400*)data;
        if (d[5] != 0x11A6B) {
            return 1;
        }
        if ((unsigned long)d[2] >> 24 == 2) {
            Actor* from = (Actor*)fn_80049888();
            Vec3 seg[2];
            IVecToVec(&seg[0], &actor->place.pos);
            IVecToVec(&seg[1], &from->place.pos);
            if (fn_800201E4((int)from, seg, 0, 0, 0)) {
                return 1;
            }
        }
        if (f->life > 0.0f) {
            f->life -= *(float*)&d[3];
            if (f->life <= 0.0f) {
                ActorPlayAnim(f->id, 0xDC04E3D5, d[2]);
            }
        }
        (void)m;
        break;
    }
    case 0xC00:
        if (d[1] == 0) {
            for (i = 0; i < 4; i++) {
                fn_800D3430(f->emitters[i], d[1]);
            }
            if (f->sound != -1) {
                fn_8016EEF4(f->sound);
                f->sound = -1;
            }
            if (fn_80022FD8(actor)) {
                fn_80022FF0(actor);
            }
        } else {
            if (f->sound == -1) {
                f->sound = fn_8016F120(fn_80170834("imp_flame"), &actor->place.pos);
            }
            if (fn_80022FD8(actor) == 0) {
                fn_800231A8(actor);
            }
            for (i = 0; i < 4; i++) {
                fn_800D3694(f->emitters[i]);
                f->emitters[i] = -1;
            }
        }
        fn_80122448(f->light, d[1]);
        FireGroupLight(f->group);
        break;
    case 0xC07: {
        Vec3 pos;
        fn_800E4C64(self, msg, data, arg);
        if (f->sound != -1) {
            fn_8016EDD8(f->sound, &actor->place.pos);
        }
        fn_801D1A48(f);
        fn_801D1D3C(f);
        IVecToVec(&pos, &actor->place.pos);
        fn_801223E4(f->light, &pos);
        return 1;
    }
    }
    return fn_800E4C64(self, msg, data, arg);
}

// Leaves the fire's group; frees the group when it is empty.
void fn_801D1A48(Fire* f) {
    FireGroup* g = f->group;
    int m0, m1, m2, m3;

    if (g == 0) {
        return;
    }
    if (g->members[0] == f->id) {
        g->members[0] = -1;
    }
    m0 = g->members[0];
    if (g->members[1] == f->id) {
        g->members[1] = -1;
    }
    m1 = g->members[1];
    if (g->members[2] == f->id) {
        g->members[2] = -1;
    }
    m2 = g->members[2];
    if (g->members[3] == f->id) {
        g->members[3] = -1;
    }
    m3 = g->members[3];
    FireGroupLight(f->group);
    if (m0 == -1 && m1 == -1 && m2 == -1 && m3 == -1) {
        VFree(g);
    }
    f->group = 0;
}

// Joins a group of a fire nearby, or starts a new one.
void fn_801D1D3C(Fire* f) {
    FireGroup* g;

    if (f->group == 0 && fn_80212730(f->id) == -1) {
        MsgResult r;
        g = 0;
        if (SendMsg(f->id, 0x400, &r) && r.p != 0 &&
            fn_8001ED98(&((Actor*)r.p)->place.pos, 2000, fn_801D20BC, &g, 0) && g != 0) {
            if (g->members[0] == -1) {
                g->members[0] = f->id;
                f->group = g;
            }
            if (g->members[1] == -1) {
                g->members[1] = f->id;
                f->group = g;
            }
            if (g->members[2] == -1) {
                g->members[2] = f->id;
                f->group = g;
            }
            if (g->members[3] == -1) {
                g->members[3] = f->id;
                f->group = g;
            }
        }
    }
    if (f->group == 0) {
        g = (FireGroup*)VAlloc(sizeof(FireGroup));
        if (g == 0) {
            return;
        }
        memset(g, -1, sizeof(FireGroup));
        g->members[0] = f->id;
        f->group = g;
    }
    FireGroupLight(f->group);
}

// Search callback: finds another fire's group.
extern "C" int fn_801D20BC(Actor* a, FireGroup** out) {
    if (a != 0 && (int)(a->handle >> 24) == 1 && a->info != 0 && ((int*)a->info)[1] == 0x20EE36) {
        FireGroup* g = ((Fire*)a->data)->group;
        if (g != 0) {
            *out = g;
        }
        return 1;
    }
    return 0;
}

// Creates the four particle emitters at the actor's position.
int fn_801D2114(Actor* actor, Fire* f) {
    FireEmitDesc* desc;
    int i;
    float x, y, z;
    FireEmitParams p;

    f->emitters[3] = -1;
    f->emitters[2] = -1;
    f->emitters[1] = -1;
    f->emitters[0] = -1;
    ((short*)lbl_803AD420)[0x16] = fn_80101FC8(lbl_803AD3A0, 8, 0);
    ((short*)lbl_803AD420)[0x8C] = fn_80101FC8(lbl_803AD3A0, 8, 0);
    ((short*)lbl_803AD420)[0x102] = fn_80101FC8(lbl_803AD3A0, 8, 5);
    ((short*)lbl_803AD420)[0x106] = fn_80101FC8(lbl_803AD3A0, 8, 6);
    ((short*)lbl_803AD420)[0x10A] = fn_80101FC8(lbl_803AD3A0, 8, 7);
    ((short*)lbl_803AD420)[0x178] = fn_80101FC8(lbl_803AD3A0, 8, 1);
    ((short*)lbl_803AD420)[0x17C] = fn_80101FC8(lbl_803AD3A0, 8, 2);
    ((short*)lbl_803AD420)[0x180] = fn_80101FC8(lbl_803AD3A0, 8, 3);
    ((short*)lbl_803AD420)[0x184] = fn_80101FC8(lbl_803AD3A0, 8, 4);
    desc = lbl_803AD420;
    x = (float)actor->place.pos.x;
    y = (float)actor->place.pos.y;
    z = (float)actor->place.pos.z;
    for (i = 0; i < 4; i++) {
        int k;
        p.desc = *desc;
        p.desc.pos.x = desc->pos.x + x;
        p.desc.pos.y = desc->pos.y + y;
        p.desc.pos.z = desc->pos.z + z;
        for (k = 0; k < 16; k++) {
            p.keys[k] = desc->f[0x13 + k];
        }
        f->emitters[i] = fn_800D3D88(&p);
        if (f->emitters[i] == -1) {
            return 0;
        }
        fn_800D35B0(f->emitters[i], f->id, 0);
        desc++;
    }
    return 1;
}

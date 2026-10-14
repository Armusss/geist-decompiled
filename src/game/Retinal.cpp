// Retinal.cpp: the "Retinal Scanner" actor type (0x80267558-0x80268528).
// When a host looks into it, a scan line sweeps the screen (two 2D overlays drawn by the trigger
// callback), then it reports the result to the scanner and plays "retpart1/2/3" effects.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

// From a shared header: names of the ghost actions (the table itself is unused here).
static const char* sActionNames[] = {
    "POSSESS", "GHOSTACTION", "SLIP", "HIJACK", "HOSTACTION", "EXCHANGE", "H_HIJACK", "TALK", "SCAN",
};

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s, u32 h) {
    int i;

    if (s == 0) {
        return 0;
    }
    for (i = 0; s[i] != 0; i++) {
        h = h * 31 + (u8)toupper(s[i]);
    }
    return h % 0xFFFFFFFF;
}

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

class String : public Buffer {
public:
    String();                                  // 0x8028C500
    String(const char* s);                     // 0x8028C400
    ~String() {}
    virtual void Release(char* p);
};

struct Actor;

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

struct ActorType {
    int unk0;
    const char* name;
    ActorMsgFunc msg;
    void (*update)(Actor* actor, float dt);
    int (*init)(Actor* actor);
    void* unk14;
    void (*destroy)(Actor* actor);
    void* unk1C[5];
};

struct RetinalDef {
    char pad0[0x1C];
    int model;
};

struct RetinalInfo {
    char pad0[8];
    int model1;
    char padC[4];
    int model2;
    char pad14[4];
    int sound;
    char pad1C[0x64];
    int flashTexture;
    int scanTexture;
    int lineTexture;
    int glowTexture;
    char pad90[4];
    int anim1;
    int anim2;
};

struct RetinalData {
    int state;
    int scanner;
    unsigned long flags;
    int progress;
    int lastProgress;
    int delta;
};

struct Actor {
    char pad0[0x38];
    int handle;
    char pad3C[0x44];
    RetinalDef* def;
    RetinalInfo* info;
    char pad88[0x1C];
    RetinalData* data;
    int ghost;
};

struct EffectMsg {
    int unk0;
    int anim;
    int unk8;
    int unkC;
    int unk10;
    int (*callback)(u32 event, Actor* a);
    Actor* actor;
    int unk1C;
};

struct AnimMsg {
    int unk0;
    int sound;
    u32 anim;
    int asset;
    int unk10;
};

struct QueryResult {
    String name;
};

struct QueryMsg {
    int unk0;
    u32 hash;
    int unk8;
    QueryResult result;
};

struct MsgResult {
    void* p;
    char pad[0xC];
};

struct TriggerMsg {
    int unk0;
    int unk4;
    int scanner;
    u32 event;
};

extern "C" void* memset(void* dst, int val, unsigned long n);
void* VAlloc(int size);
void VFree(void* p);
int ActorDefaultMsg(Actor* actor, int msg, void* data, int arg); // 0x800E4C64
void ActorUpdate(Actor* a, float dt);                     // 0x800E48A0
int ActorInitBase(Actor* a);                              // 0x800E6444
int ActorPlayAnim(int handle, u32 anim, int arg);         // 0x800C5654
int ActorAnimState(int handle, u32 anim);                 // 0x800C5028
int SendMsg(int target, int msg, void* data);             // 0x800265D4
int AssetFind(u32 bank, u32 name);                        // 0x8007EB34
void* FxFind(const char* name);                           // 0x80170834
void FxStart(void* fx, int a, int b);                     // 0x8016F390
void ModelLoadIds(short* ids, int count);                 // 0x8003D84C
void ModelFreeIds(short* ids, int count);                 // 0x8003D66C
extern "C" void fn_8000F874(int tex);                    // texture acquire
extern "C" void fn_8000F60C(int tex);                    // texture release
#define TextureRef fn_8000F874
#define TextureUnref fn_8000F60C
void AnimRef(int anim);                                   // 0x800934D0
void AnimUnref(int anim);                                 // 0x80093424
void Draw2DBegin(void);                                   // 0x8013C6F0
void Draw2DEnd(void);                                     // 0x8013C6C4
void DrawTexQuad(int tex, float x, float y, float w, float h, u8 r, u8 g, u8 b, u8 a); // 0x8013C308
void DrawSetTexture(int tex);                             // 0x8013C074
void DrawVertex(u8 r, u8 g, u8 b, u8 a, float x, float y, float u, float v); // 0x8013C034
void DrawFlush(void);                                     // 0x8013C030

extern float gAspectScale;                                // 0x804D2520

int RetinalMsg(Actor* a, int msg, void* data, int arg);
void RetinalUpdate(Actor* a, float dt);
int RetinalInit(Actor* a);
void RetinalDestroy(Actor* a);
int RetinalOnDone(u32 event, Actor* a);
int RetinalOnStart(u32 event, Actor* a);

ActorType gRetinalType = { 0, "Retinal Scanner", RetinalMsg, RetinalUpdate, RetinalInit, 0, RetinalDestroy };

void RetinalUpdate(Actor* a, float dt) {
    RetinalData* d = a->data;

    switch (d->state) {
    case 1:
        break;
    case 3:
        break;
    case 2: {
        int delta;
        d->lastProgress = d->progress;
        delta = d->delta;
        if (delta < 0) {
            d->delta = delta + (int)(1040.0f * dt / 900.0f);
            if (d->delta > 0) d->delta = 0;
        } else if ((float)d->progress > 1040.0f) {
            d->delta = delta + (int)(1040.0f * dt / 900.0f);
        } else {
            d->progress = d->progress + (int)(1040.0f * dt / 900.0f);
        }
        if ((float)(d->progress + d->delta) > 1640.0f) {
            EffectMsg m;
            AnimMsg am;
            d->state = 3;
            FxStart(FxFind("retpart3"), 0, 0);
            memset(&m, 0, sizeof(m));
            m.anim = a->info->anim2;
            m.unkC = 16;
            m.callback = RetinalOnDone;
            m.actor = a;
            SendMsg(a->handle, 0xC12, &m);
            am.sound = a->info->sound;
            am.anim = 0xF21581DE;
            am.asset = AssetFind(0xC0A77187, 0);
            SendMsg(d->scanner, 0x40B, &am);
            d->flags |= 2;
        }
        break;
    }
    }
    ActorUpdate(a, dt);
}

static inline int RetinalOnQuery(void* data) {
    Actor* actor = ((Actor**)data)[0];
    RetinalData* d = actor->data;
    int ghost;
    int ok;
    AnimMsg am;

    if (ActorPlayAnim(actor->handle, StrHash("QUERY", ((u32*)data)[2]), ((int*)data)[1])) return 1;

    ghost = actor->ghost;
    if (ghost != -1) {
        {
            QueryMsg q;
            *(int*)&q.result = 0;
            q.hash = 0x00251E4A;
            q.unk8 = 0;
            ok = SendMsg(ghost, 0x41E, &q);
            ghost = *(int*)&q.result;
        }
        if (ok && ghost == 1) return 1;
    }

    if (((u32*)data)[2] == 0x7C44EC3D && d->state == 0) {
        ((u32*)data)[3] = 0xF21581DE;
        am.anim = ((u32*)data)[3];
        am.sound = actor->info->sound;
        am.asset = AssetFind(0xC0A77187, 0x0025A645);
        SendMsg(((int*)data)[1], 0x40B, &am);
        d->flags |= 2;
        return 1;
    }
    return 0;
}

static inline int RetinalOnAnim(void* data) {
    Actor* actor = ((Actor**)data)[0];
    RetinalData* d = actor->data;

    if (ActorPlayAnim(actor->handle, ((u32*)data)[3], ((int*)data)[2])) return 1;

    if (d->state == 0) {
        EffectMsg m;
        d->state = 1;
        d->scanner = ((int*)data)[2];
        d->lastProgress = -200;
        d->progress = -200;
        d->delta = -600;
        FxStart(FxFind("retpart1"), 0, 0);
        memset(&m, 0, sizeof(m));
        m.anim = actor->info->anim1;
        m.unkC = 16;
        m.callback = RetinalOnStart;
        m.actor = actor;
        SendMsg(actor->handle, 0xC12, &m);
    }
    return 1;
}

int RetinalTrigger(void* data);
int RetinalPrecache(void* data);

static inline int RetinalOnState(void* data) {
    RetinalData* d = (*(Actor**)data)->data;

    switch (((int*)data)[1]) {
    case 0x801EFD79:
        if (d->flags & 2) {
            if (d->state == 1) {
                ((int*)data)[3] = 0;
            } else if (d->state == 3) {
                ((int*)data)[3] = 2;
            }
            d->flags &= ~2;
        } else {
            ((int*)data)[3] = 1;
        }
        return 1;
    }
    return 0;
}

int RetinalMsg(Actor* a, int msg, void* data, int arg) {
    int ret = 0;

    switch (msg) {
    case 0x40A:
        ret = RetinalOnQuery(data);
        break;
    case 0x409:
        ret = RetinalOnAnim(data);
        break;
    case 0xC14:
        ret = RetinalTrigger(data);
        break;
    case 6:
        ret = RetinalPrecache(data);
        break;
    case 0x41E:
        ret = RetinalOnState(data);
        break;
    }

    if (ret == 0) return ActorDefaultMsg(a, msg, data, arg);
    return ret == -999999 ? 0 : ret;
}

void RetinalDestroy(Actor* a) {
    if (a && a->data) {
        VFree(a->data);
        a->data = 0;
    }
}

int RetinalInit(Actor* a) {
    if (!ActorInitBase(a)) return 0;
    if (a->data == 0) a->data = (RetinalData*)VAlloc(sizeof(RetinalData));
    if (a->data == 0 || a->info == 0) return 0;
    return 1;
}

int RetinalPrecache(void* data) {
    Actor* actor = *(Actor**)data;
    short ids[4];
    int n = 0;

    if (actor->def && actor->def->model >= 0) {
        ids[0] = actor->def->model;
        n = 1;
    }
    if (actor->info) {
        short* p = &ids[n];
        int id = actor->info->model1;
        if (id >= 0) {
            *p++ = id;
            n++;
        }
        id = actor->info->model2;
        if (id >= 0) {
            *p = id;
            n++;
        }
    }

    if (((int*)data)[1]) {
        ModelLoadIds(ids, n);
        TextureRef(actor->info->flashTexture);
        TextureRef(actor->info->scanTexture);
        TextureRef(actor->info->lineTexture);
        TextureRef(actor->info->glowTexture);
        AnimRef(actor->info->anim1);
        AnimRef(actor->info->anim2);
    } else {
        ModelFreeIds(ids, n);
        TextureUnref(actor->info->flashTexture);
        TextureUnref(actor->info->scanTexture);
        TextureUnref(actor->info->lineTexture);
        TextureUnref(actor->info->glowTexture);
        AnimUnref(actor->info->anim1);
        AnimUnref(actor->info->anim2);
    }
    return 1;
}

int RetinalTrigger(void* data) {
    Actor* actor = ((Actor**)data)[0];
    RetinalData* d = actor->data;

    if (((u32*)data)[3] == 0x640D1F4D) {
        float top, bottom, t;

        if (d->state == 2) {
        Draw2DBegin();
        if (d->progress > 160 && d->progress < 320) {
            int alpha = 255 - (int)(3.2f * (d->progress - 240));
            DrawTexQuad(actor->info->flashTexture, 0.0f, 0.0f, 640.0f, 480.0f, 255, 255, 255,
                        alpha < 0 ? 0 : (alpha > 255 ? 255 : alpha));
        }

        if (d->delta > 0) {
            t = 240.0f * d->delta / 600.0f;
            top = t;
            bottom = 480.0f - t;
        } else {
            t = 240.0f * d->delta / 600.0f;
            top = -t;
            bottom = 480.0f + t;
        }
        top = top < 0.0f ? 0.0f : (top > 480.0f ? 480.0f : top);
        bottom = bottom < 0.0f ? 0.0f : (bottom > 480.0f ? 480.0f : bottom);

        DrawSetTexture(actor->info->scanTexture);
        DrawVertex(255, 255, 255, 255, 320.0f, 240.0f, 0.0f, 0.5f);
        DrawVertex(255, 255, 255, 255, 320.0f, 240.0f, 0.0f, 0.5f);
        DrawVertex(255, 255, 255, 255, d->progress, top, 1.0f, 0.0f);
        DrawVertex(255, 255, 255, 255, d->progress, bottom, 1.0f, 1.0f);
        DrawFlush();

        if (d->progress + d->delta > 0) {
            float lo;
            float x = (d->progress - 320.0f) + d->delta;
            lo = -320.0f;
            if (x - lo > 150.0f) lo = x - 150.0f;
            if (lo < 320.0f) {
                int a1 = 255.0f * ((x - lo) / 150.0f);
                int a2 = 255.0f * (((float)(d->progress - 320) - x) / 150.0f);
                DrawSetTexture(actor->info->lineTexture);
                lo = 330.0f + lo;
                DrawVertex(a1, a1, a1, 255, lo, -30.0f, 0.0f, 0.99f);
                x = 330.0f + x;
                DrawVertex(a2, a2, a2, 255, x, -30.0f, 0.0f, 0.0f);
                DrawVertex(a1, a1, a1, 255, x, 510.0f, 0.0f, 0.0f);
                DrawVertex(a2, a2, a2, 255, lo, 510.0f, 0.0f, 0.99f);
                DrawFlush();
            }

            {
                float x2 = (d->progress - 320.0f) + d->delta;
                float lo2 = -320.0f;
                float hi2;
                float minX, maxX;
                int b1, b2, b3;
                float u1, u2;
                if (x2 - lo2 > 200.0f) lo2 = x2 - 200.0f;
                minX = -320.0f / gAspectScale;
                maxX = 320.0f / gAspectScale;
                if (lo2 < minX) {
                    lo2 = minX;
                } else if (lo2 > maxX) {
                    lo2 = maxX;
                }
                minX = x2 < minX ? minX : (x2 > maxX ? maxX : x2);
                hi2 = minX;
                b1 = 255 - (int)(255.0f * ((hi2 - lo2) / 600.0f));
                b2 = 255 - (int)(255.0f * (((float)(d->progress - 320) - hi2) / 600.0f));
                b3 = 255.0f * ((hi2 - lo2) / 200.0f);
                u1 = (lo2 + maxX) / (640.0f / gAspectScale);
                u2 = (hi2 + maxX) / (640.0f / gAspectScale);
                DrawSetTexture(actor->info->glowTexture);
                float l = 320.0f + lo2;
                DrawVertex(b1, b1, b1, 0, l, 0.0f, u1, 1.0f);
                float h = 320.0f + hi2;
                DrawVertex(b2, b2, b2, b3, h, 0.0f, u2, 1.0f);
                DrawVertex(b2, b2, b2, b3, h, 480.0f, u2, 0.0f);
                DrawVertex(b1, b1, b1, 0, l, 480.0f, u1, 0.0f);
                DrawFlush();
            }
        }
        Draw2DEnd();
        }
        return 1;
    } else if (((u32*)data)[3] == 0x05C57E60) {
        if (d->state != 3 && d->state != 0) {
            AnimMsg am;
            am.sound = actor->info->sound;
            am.anim = 0xF21581DE;
            am.asset = AssetFind(0xC0A77187, 0);
            SendMsg(d->scanner, 0x40B, &am);
        }
        return 1;
    }
    return 0;
}

static inline void RetinalNotify(int ghost, int scanner) {
    if (ghost != -1) {
        TriggerMsg m;
        m.scanner = scanner;
        m.event = 0x2072074C;
        m.unk4 = 0;
        SendMsg(ghost, 0xC14, &m);
    }
}

int RetinalOnDone(u32 event, Actor* a) {
    if (event == 0x1B83833F) {
        RetinalData* d = a->data;
        MsgResult res;
        if (d->state != 3) return 1;
        SendMsg(d->scanner, 0x40C, &res);
        d->state = 0;
        if (ActorAnimState(a->handle, 0x8B40C0DC) != 1 || ActorPlayAnim(a->handle, 0x8B40C0DC, d->scanner) == 1) {
            if (a->ghost != -1) RetinalNotify(a->ghost, d->scanner);
        }
    }
    return 1;
}

int RetinalOnStart(u32 event, Actor* a) {
    if (event == 0x1B83833F) {
        RetinalData* d = a->data;
        if (d->state != 1) return 1;
        FxStart(FxFind("retpart2"), 0, 0);
        d->state = 2;
    }
    return 1;
}

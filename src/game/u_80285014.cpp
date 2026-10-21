// u_80285014.cpp: the "Tool" effect type (0x80285014-0x80285670).
// A possessable/stealable tool: it fires the definition's effect out of the owner's emitter, sends a
// query message back to the new owner, and loads/frees the definition's model. Names are descriptive
// guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {

// Callees in other files, not named yet.
int fn_800F7D28(int emitter, int id);
void* fn_8016F120(void* fx, int* pos);
void fn_8016F118(void* handle, int a, int b);
void fn_800F86A0(int emitter);
void* fn_800EB21C(int id);
int fn_800F80DC(int a);
int fn_800F8A30(int a);
void fn_800F825C(int emitter, int id, int count);
}

void Fn800F7F5C(int emitter, int id, int msg, int arg, void* info, float dt);
extern "C" void FxStart(void* fx, int a, int b);
void* VAlloc(int size);
void VFree(void* p);
void Fn800F840C(int emitter, int a, int b);
void Fn80175D60(unsigned long arg);
extern "C" int SendMsg(int target, int msg, void* data);
void ModelLoadIds(short* ids, int count);
void ModelFreeIds(short* ids, int count);

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

inline u32 StrHash(const char* s) {
    if (s == 0) {
        return 0;
    } else {
        u32 h = 0;
        int i;
        for (i = 0; s[i] != 0; i++) {
            h = h * 31 + (unsigned char)toupper(s[i]);
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

inline u32 EffectTypeHash(EffectType* type) {
    if (type->hash == 0) {
        type->hash = StrHash(type->name);
    }
    return type->hash;
}

struct ToolDef {
    int unk0;
    int id;
    int unk8;
    float unkC;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
    int unk20;
    int unk24;
    int unk28;
    int unk2C;
    void* fx;
    int unk34;
    int unk38;
    void* fx2;
};

struct Tool {
    int unk0;
    float unk4;
    ToolDef* def;
    int unkC;
};

struct ShotHit {
    u8 pad[0x30];
    float x, y, z;
};

struct ShotInfo {
    ShotHit* hit;
    float speed;
    int unk8;
    int unkC;
    int charge;
    int unk14;
    int unk18;
    int unk1C;
};

class Buffer {
public:
    Buffer();                  // 0x8028BA9C
    virtual ~Buffer();         // 0x8028B9C0
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();                  // 0x8028C500
    ~String() {}
    virtual void Release(char* p);
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

extern EffectType gToolType;

void ToolReset(EffectObj* obj);
void ToolUpdate(EffectObj* obj, float dt);
u32 ToolQuery(EffectObj* obj, int msg);
int ToolMsg(EffectObj* obj, int msg, int arg, void* data, float dt);
void ToolDestroy(EffectObj* obj);
void* ToolCreate(int a, int id);

void ToolReset(EffectObj* obj) {}

void ToolUpdate(EffectObj* obj, float dt) {}

u32 ToolQuery(EffectObj* obj, int msg) {
    Tool* t = (Tool*)obj->data;
    ToolDef* def = t->def;

    switch (msg) {
    case 0xF829DB93:
    case 0x0020EE36:
        if (fn_800F7D28(t->unk0, def->id)) {
            return 0x04A3E183;
        }
        if (fn_800F7D28(obj->emitter, def->id)) {
            return 0xC0BECF7E;
        }
        return 0x11D2C025;
    case 0x968CBE3D:
    case 0xE6B71BCC:
        if (fn_800F7D28(t->unk0, def->id) == def->unk8) {
            return 0x4848CA1E;
        }
        if (fn_800F7D28(obj->emitter, def->id)) {
            return 0x04A3E183;
        }
        return 0x11D2C025;
    }
    return 0x3DA260F7;
}

int ToolMsg(EffectObj* obj, int msg, int arg, void* data, float dt) {
    Tool* t = (Tool*)obj->data;
    ToolDef* def = t->def;

    Fn80175D60(arg);
    switch (msg) {
    case 0xF829DB93:
    case 0x0020EE36: {
        int pos[3];
        ShotInfo info;
        float* p;
        void* fx;

        if (!fn_800F7D28(t->unk0, def->id)) {
            return 0;
        }
        t->unkC = 1;
        info.hit = (ShotHit*)data;
        info.speed = def->unkC;
        info.unkC = def->unk14;
        info.charge = t->unk4;
        info.unk18 = def->unk28;
        info.unk1C = def->unk2C;
        Fn800F7F5C(t->unk0, def->id, 0x14967, arg, &info, dt);
        fx = def->fx;
        p = &info.hit->x;
        if (p) {
            pos[0] = p[0];
            pos[1] = p[1];
            pos[2] = p[2];
            fn_8016F118(fn_8016F120(fx, pos), 0, 100000);
        } else {
            FxStart(fx, 0, 0);
        }
        fn_800F825C(t->unk0, def->id, 1);
        return 1;
    }
    case 0x968CBE3D:
    case 0xE6B71BCC: {
        int n;
        int m;
        int r;

        t->unkC = 1;
        if (data == 0) {
            return 0;
        }
        n = fn_800F7D28(t->unk0, def->id);
        if (n == def->unk8) {
            return 0;
        }
        m = fn_800F7D28(obj->emitter, def->id);
        if (m == 0) {
            return 0;
        }
        n = (m < def->unk8 - n) ? m : def->unk8 - n;
        {
            QueryMsg q;
            q.hash = 0x5A32795A;
            *(int*)&q.result = 0;
            q.unk8 = 0;
            SendMsg(arg, 0x41E, &q);
            r = *(int*)&q.result;
        }
        if (r == 0) {
            fn_800F825C(obj->emitter, def->id, n);
        }
        Fn800F840C(t->unk0, def->id, n);
        return 1;
    }
    case 0x8FDD7599: {
        int pos[3];
        ShotHit* hit;
        void* fx;

        t->unkC = 1;
        fx = def->fx2;
        hit = (ShotHit*)data;
        if (&hit->x) {
            pos[0] = hit->x;
            pos[1] = hit->y;
            pos[2] = hit->z;
            fn_8016F118(fn_8016F120(fx, pos), 0, 100000);
        } else {
            FxStart(fx, 0, 0);
        }
        return 1;
    }
    case 0x0B59E861:
        return 1;
    case 0xE452286F: {
        short id = def->unk28;
        if ((int)data == 1) {
            ModelLoadIds(&id, 1);
        } else {
            ModelFreeIds(&id, 1);
        }
        return 1;
    }
    case 0x141109BD: {
        short id = def->unk28;
        if ((int)data == 1) {
            ModelLoadIds(&id, 1);
        } else {
            ModelFreeIds(&id, 1);
        }
        return 1;
    }
    }
    return 0;
}

void ToolDestroy(EffectObj* obj) {
    Tool* t = (Tool*)obj->data;
    fn_800F86A0(t->unk0);
    VFree(t);
}

void* ToolCreate(int a, int id) {
    Tool* t;

    EffectTypeHash(&gToolType);
    t = (Tool*)VAlloc(sizeof(Tool));
    if (t == 0) {
        return 0;
    }
    t->def = (ToolDef*)fn_800EB21C(id);
    t->unk0 = fn_800F8A30(fn_800F80DC(a));
    Fn800F840C(t->unk0, t->def->id, t->def->unk8);
    return t;
}

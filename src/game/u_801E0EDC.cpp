// u_801E0EDC.cpp (0x801E0EDC-0x801E14E8): the active-object registry (a linked list of ActiveObject
// pointers kept in .sbss 0x804D5220) and its helpers, plus the ActiveObject base destructor.
// Boundary evidence (the cut at 0x801E14E8, between this file and u_801E14E8.cpp, the barrier actor):
// - .sdata2: 1.0f is stored twice, at 0x804D8D78 (read by fn_801E0EDC here) and at 0x804D8D80 (read by
//   BarrierActorMsg/BarrierActorUpdate). CodeWarrior merges identical literals within one translation unit,
//   so the two copies come from two files; this file's pool is 0x804D8D78-0x804D8D80 (1.0f plus padding).
// - .data: the ActiveObject vtable 0x803ADE00-0x803ADE38 (destructor fn_801E14A0) is used only here and by
//   the derived class in u_8020A650; the barrier's data (kind table, type descriptor) starts at 0x803ADE38.
// - .sbss 0x804D5220-0x804D5228 (registry head) is touched only by the functions in this file.
// - The barrier's .rodata pool 0x80348E08 starts with the shared ghost-action name strings followed by
//   "Ghost Barrier"; nothing in this file reads .rodata.
// Names are descriptive guesses; the code is matched against the retail DOL.
//
// These functions are still called by other units through their fn_
// placeholders (see u_80230504.cpp), so they keep C linkage and their
// original symbol names.

#include "game/types.h"

typedef unsigned long u32;

struct ActiveEntry {
    void* value;        // 0x0
    ActiveEntry* next;  // 0x4
};

ActiveEntry* lbl_804D5220;

extern "C" float fn_802A2080(void);
void VFree(void* p);

extern void* VAlloc(int size);

// Base class of the objects held in the active-object registry (vtable 0x803ADE00).
// Storage comes from the virtual-memory heap.
class ActiveObject {
public:
    static void* operator new(unsigned long size) { return VAlloc(size); }
    static void operator delete(void* p) { VFree(p); }
    ActiveObject() {}
    virtual ~ActiveObject();                                                      // 0x8
    virtual unsigned char Init(void* a, void* b, void* c, void* d, void* e, void* f) = 0; // 0xC
    virtual void Feed(float value, int flag) = 0;                                 // 0x10
    virtual void Animate(int anim, void* mtx, char* name) = 0;                    // 0x14
    virtual void Cleanup(int flag) = 0;                                           // 0x18
    virtual u32 Tick() = 0;                                                       // 0x1C
    virtual void Step() = 0;                                                      // 0x20
    virtual void Unk24() = 0;
    virtual void Unk28() = 0;
    virtual void Unk2C() = 0;
    virtual void Apply(Vec3* pos, float scale, Vec3* vel, int kind) = 0;          // 0x30
    virtual void Unk34() = 0;
};

static inline int ActiveIsRegistered(void* p) {
    if (p == 0) return 0;
    for (ActiveEntry* e = lbl_804D5220; e != 0; e = e->next) {
        if (e->value == p) return 1;
    }
    return 0;
}

// Scales a vector into another.
static inline void VecScaleTo(Vec3* out, const Vec3* in, float s) {
    float x = in->x * s;
    float y = in->y * s;
    float z = in->z * s;
    out->z = z;
    out->y = y;
    out->x = x;
}

// Applies a distance-scaled velocity to a registered active object.
extern "C" void fn_801E0EDC(void* actor, Vec3* a, Vec3* b, int kind, float scale) {
    if (ActiveIsRegistered(actor)) {
        Vec3 v;
        VecScaleTo(&v, b, 1.0f / fn_802A2080());
        ((ActiveObject*)actor)->Apply(a, scale, &v, kind);
    }
}

// Unregisters an active object and frees its registry entry.
extern "C" void fn_801E0FD0(void** slot) {
    if (slot == 0) return;
    void* obj = *slot;
    if (obj != 0 && ActiveIsRegistered(obj)) {
        delete (ActiveObject*)obj;
        ActiveEntry* cur = lbl_804D5220;
        if (obj == cur->value) {
            lbl_804D5220 = cur->next;
            VFree(cur);
        } else {
            ActiveEntry* prev = cur;
            cur = cur->next;
            while (cur != 0 && cur->value != obj) {
                prev = cur;
                cur = cur->next;
            }
            if (cur != 0) {
                prev->next = cur->next;
                VFree(cur);
            }
        }
        *slot = 0;
    }
}

// Ticks a registered active object; returns the hook's result, or 1 when the
// object is not registered.
extern "C" u32 fn_801E10D8(void* obj) {
    u32 result;
    if (ActiveIsRegistered(obj)) {
        result = ((ActiveObject*)obj)->Tick();
    } else {
        result = 1;
    }
    return result;
}

// Runs the cleanup hook of a registered active object.
extern "C" void fn_801E1150(void* obj, int flag) {
    if (ActiveIsRegistered(obj)) {
        ((ActiveObject*)obj)->Cleanup(flag);
    }
}

// Calls the update hook of a registered active object.
extern "C" void fn_801E11C0(void* obj, int anim, void* mtx, char* name) {
    if (ActiveIsRegistered(obj)) {
        ((ActiveObject*)obj)->Animate(anim, mtx, name);
    }
}

// The registered node type (vtable 0x803BB8CC, defined with u_8020A650).
class ActiveNode : public ActiveObject {
public:
    ActiveNode() : m0C(0) {}
    virtual ~ActiveNode();
    virtual unsigned char Init(void* a, void* b, void* c, void* d, void* e, void* f);
    virtual void Feed(float value, int flag);
    virtual void Animate(int anim, void* mtx, char* name);
    virtual void Cleanup(int flag);
    virtual u32 Tick();
    virtual void Step();
    virtual void Unk24();
    virtual void Unk28();
    virtual void Unk2C();
    virtual void Apply(Vec3* pos, float scale, Vec3* vel, int kind);
    virtual void Unk34();
    int m04;                                        // 0x4
    int m08;                                        // 0x8
    int m0C;                                        // 0xC
    int m10;                                        // 0x10
    int m14;                                        // 0x14
    int m18;                                        // 0x18
    int m1C;                                        // 0x1C
};

// Creates an active node, initialises it from the six arguments and registers
// it. Returns 0 (after releasing the half-built node) on any failure.
extern "C" void* fn_801E1230(void* a, void* b, void* c, void* d, void* e, void* f) {
    ActiveNode* node = new ActiveNode;
    if (node == 0) {
        return 0;
    }
    if (node->Init(a, b, c, d, e, f) == 0) {
        delete node;
        return 0;
    }
    ActiveEntry* entry = (ActiveEntry*)VAlloc(8);
    if (entry == 0) {
        delete node;
        return 0;
    }
    entry->value = node;
    entry->next = lbl_804D5220;
    lbl_804D5220 = entry;
    return node;
}

// Calls the per-object tick hook (vtable +0x20) of every registered object.
extern "C" void fn_801E1354(void) {
    for (ActiveEntry* e = lbl_804D5220; e != 0; e = e->next) {
        ((ActiveObject*)e->value)->Step();
    }
}

// Pushes a value to every registered object; the second argument flags it.
extern "C" void fn_801E13A0(float value) {
    for (ActiveEntry* e = lbl_804D5220; e != 0; e = e->next) {
        ((ActiveObject*)e->value)->Feed(value, 1);
    }
}

// Builds the name tables used by the active-object registry.
void u8020A650InitNameTables(void);

extern "C" void fn_801E1400(void) {
    u8020A650InitNameTables();
}

// Releases and frees every entry of the active-object registry.
extern "C" void fn_801E1420(void) {
    ActiveEntry* entry = lbl_804D5220;
    while (entry != 0) {
        ActiveEntry* next;
        delete (ActiveObject*)entry->value;
        next = entry->next;
        VFree(entry);
        entry = next;
    }
    lbl_804D5220 = 0;
}

// Clears the head of the active-object registry.
// Kept under its fn_ placeholder: u_801BF578 still calls it by that name.
extern "C" void fn_801E1494(void) {
    lbl_804D5220 = 0;
}

// Vtable installed on released active objects.
extern char lbl_803ADE00[];

// Resets an active object's vtable and optionally frees its storage.
// Kept under its fn_ placeholder (it is the ActiveObject destructor, slot 0x8 of vtable 0x803ADE00).
extern "C" void* fn_801E14A0(void* object, short freeNow) {
    if (object != 0) {
        *(void**)object = (void*)lbl_803ADE00;
        if (freeNow > 0) {
            VFree(object);
        }
    }
    return object;
}

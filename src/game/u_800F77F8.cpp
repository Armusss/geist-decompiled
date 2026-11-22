// u_800F77F8.cpp (0x800F77F8-0x800F9458): the effect kind table, effect rows and the emitter/instance pools, plus the two vector template
// instances (16- and 4-byte elements) whose vtables this file uses.
// Split from the old u_800F73C8 (0x800F73C8-0x800FDDE4). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary evidence (split 2026-11-22): own .sdata2 0.0f at 0x804D7488 (the camera file reads another at
// 0x804D749C), .sbss 0x804D4B88-0x804D4B98 and .bss 0x80457F70-0x8045C770 (emitter and instance pools), the vtables
// .data 0x8039CF18/0x8039CF38 that point at fn_800F9278..fn_800F9438. None of the stream state before it.


#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0

struct StreamNode;
struct EffectKind;
struct EffectInst;
struct Emitter;
struct Vector;
struct CamObj;
struct Shake;
struct GameType;
struct Minigame;

// Functions called from other units under their placeholder names.
extern "C" float fn_800F7920(u32 row, u32 key);
extern "C" int fn_800F7994(u32 row, u32 key);
extern "C" void fn_800F7A90(u32 row);
extern "C" int fn_800F7AF0(u32 emitter, u32 row);
extern "C" int fn_800F7C64(u32 emitter, int row);
extern "C" int fn_800F7D28(u32 emitter, u32 row);
extern "C" int fn_800F80DC(u32 handle);
extern "C" int fn_800F8130(u32 handle, int i);
extern "C" int fn_800F81EC(u32 handle);
extern "C" int fn_800F825C(u32 handle, u32 row, int n);
extern "C" void fn_800F86A0(u32 handle);
extern "C" u32 fn_800F8A30(int user);
extern "C" u32 fn_800F8AF4(u32 row);
extern "C" void fn_800F8ED0(void);
extern "C" void fn_800F9468(int on);
extern "C" int fn_800F94C8(u32 handle);
extern "C" void fn_800F94F8(u32 handle, short v);
extern "C" void fn_800F9578(u32 handle, int v);
extern "C" float fn_800F95E4(u32 handle);
extern "C" int fn_800F9614(u32 handle);
extern "C" void fn_800F96E0(u32 handle);
extern "C" u32 fn_800F9724(int owner, int id);
extern "C" u32 fn_800FC3B8(int id);
extern "C" int fn_800FC764(u32 who, int arg);
extern "C" char* fn_800FD334(int i);
extern "C" int fn_800FD3DC(void);
extern "C" int fn_800FD3EC(void);
extern "C" char* fn_800FD3FC(int i);
extern "C" char* fn_800FD424(int i);
extern "C" const char* fn_800FD44C(int c);
extern "C" const char* fn_800FD5C4(int c);
extern "C" int fn_800FD6EC(int i);
extern "C" void fn_800FD924(int arg);
extern "C" void fn_800FD998(void);
extern "C" void fn_800FDA10(void);
extern "C" void fn_800FDA90(void);
extern "C" void fn_800FDC48(void);
extern "C" void fn_800FDC78(void);

extern "C" int SendMsg(int to, int msg, void* data);

static inline void SetVarInt(int target, u32 hash, int value) {
    QueryMsg m;
    m.hash = hash;
    *(int*)&m.result = value;
    m.unk8 = 0;
    SendMsg(target, 0x41D, &m);
}

static inline int GetVarInt(int target, u32 hash) {
    QueryMsg m;
    *(int*)&m.result = 0;
    m.hash = hash;
    m.unk8 = 0;
    SendMsg(target, 0x41E, &m);
    return *(int*)&m.result;
}

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

// ---- effect table and emitters -----------------------------------------------------------
// gEffectKinds is a vector of effect type records (name hash at +4, handlers at +0x14..+0x28),
// gEffectRows a vector of 16-byte rows {id, ?, table, index} describing the effects, and each
// emitter (handle 0x4C......) owns a list of live instances of those rows.

struct EffectKind {
    const char* name;       // 0x00
    u32 hash;               // 0x04
    u8 pad08[0x14 - 0x08];
    int (*create)(u32 emitter, u32 row);                // 0x14
    void (*destroy)(void* data);                        // 0x18
    int (*update)(float dt, void* data, int a, int b, int c);   // 0x1C
    int (*message)(void* data, int a, int b, int c);    // 0x20
    u8 pad24[4];
    void (*onAdd)(void* data);                          // 0x28
};

struct EffectRow {
    u32 id;                 // 0x00
    int unk04;              // 0x04
    u32 table;              // 0x08
    u32 index;              // 0x0C
};

struct VectorBase {
    virtual ~VectorBase();
};

struct Vector : VectorBase {
    int unk4;
    u32 bytes;              // 0x08
    char* data;             // 0x0C
};

// One live instance in an emitter's list.
struct EffectInst {
    u8 pad00[8];
    int active;             // 0x08
    u32 row;                // 0x0C
    int count;              // 0x10
    u32 emitter;            // 0x14
    int data;               // 0x18
};

struct Emitter {
    u8 pad00[8];
    u8 list[8];             // 0x08
};

extern Vector* gEffectKinds;    // 0x804D4B90
extern Vector* gEffectRows;     // 0x804D4B94
extern Emitter gEmitters[];     // 0x80457F70
extern int gEmitterPool;        // 0x804D4B88
extern int gInstPool;           // 0x804D4B8C
extern float gZeroF;            // 0x804D7488

extern "C" {
void* fn_8028B3BC(Vector* v, void* where, int n);
u32 VarGetHash(u32 table, int index, u32 key);              // 0x800DF1D8
float VarGetFloat(u32 table, int index, u32 key);           // 0x800DF2C8
int Fn800DF3B8(u32 table, int index, u32 key);              // 0x800DF3B8
int VarFindHash(u32 a, u32 b, u32 c);                       // 0x800DF4A8
float VarFindFloat(u32 a, u32 b, u32 c);                    // 0x800DF5CC
EffectInst* fn_80086928(void* list, u32 key, void* pred);
int fn_800869CC(void* list);
EffectInst* fn_800869A0(void* list, int i);
void fn_800869EC(void* list, EffectInst* n);
void fn_80086B20(void* list, void* n);
void* fn_80086AC0(void* pool);
void fn_800EB744(void);
}

static inline Emitter* EmitterOf(u32 handle) {
    return (handle >> 24 == 0x4C) ? &gEmitters[handle & 0xFFFFFF] : NULL;
}

static inline int RowValid(u32 row) {
    return (int)row >= 0 && row < (gEffectRows->bytes >> 4);
}

static inline EffectKind* KindOfRow(u32 row) {
    int hash = VarFindHash(0x47F42363, ((EffectRow*)gEffectRows->data)[row].id, 0x3DC50993);
    u32 n = gEffectKinds->bytes >> 2;
    int off = 0;
    for (; n != 0; n--) {
        EffectKind* k = *(EffectKind**)(gEffectKinds->data + off);
        if (hash == (int)k->hash) {
            return k;
        }
        off += 4;
    }
    return NULL;
}

// Registers an effect kind: hashes its name and appends it to the kind vector.
void EffectKindRegister(EffectKind* k) {
    const unsigned char* p = (const unsigned char*)k->name;
    u32 h;
    if (p == NULL) {
        h = 0;
    } else {
        h = 0;
        while (true) {
            unsigned char c = *p;
            u32 u;
            if (c == 0) {
                break;
            }
            if (c == 0xFF) {
                u = 0xFFFFFFFF;
            } else {
                u = __upper_map[c];
            }
            p++;
            h = h * 31 + (u & 0xFF);
        }
        h = h % 0xFFFFFFFF;
    }
    k->hash = h;
    *(EffectKind**)fn_8028B3BC(gEffectKinds, gEffectKinds->data + gEffectKinds->bytes, 4) = k;
}

// HashMix: looks up key `key` in the table of effect row `row` (0 when out of range).
u32 HashMix(u32 row, u32 key) {
    if (RowValid(row)) {
        EffectRow* r = (EffectRow*)(gEffectRows->data + row * 0x10);
        return VarGetHash(r->table, r->index, key);
    }
    return 0;
}

float fn_800F7920(u32 row, u32 key) {
    if (RowValid(row)) {
        EffectRow* r = (EffectRow*)(gEffectRows->data + row * 0x10);
        return VarGetFloat(r->table, r->index, key);
    }
    return gZeroF;
}

int fn_800F7994(u32 row, u32 key) {
    if (RowValid(row)) {
        EffectRow* r = (EffectRow*)(gEffectRows->data + row * 0x10);
        return Fn800DF3B8(r->table, r->index, key);
    }
    return -1;
}

u32 EffectRowIndex(int row) {
    return ((EffectRow*)gEffectRows->data)[row].index;
}

int EffectRowClass(int row) {
    int h = VarFindHash(0x47F42363, ((EffectRow*)gEffectRows->data)[row].id, 0x799F7D1B);
    if (h == 0x39FEC2BD) {
        return 1;
    }
    return h == 0x79A06324 ? 2 : 3;
}

void fn_800F7A90(u32 row) {
    u32 id;
    if ((int)row < 0 || (gEffectRows->bytes >> 4) <= row) {
        id = 0;
    } else {
        id = ((EffectRow*)gEffectRows->data)[row].id;
    }
    VarFindHash(0x47F42363, id, 0x24C3F827);
}

static u32 InstRowDiffers(EffectInst* n, u32 row);
static u32 RowIdDiffers(EffectInst* n, int id);

// Calls the "added" handler of the kind of `row` for the instance in `emitter`.
int fn_800F7AF0(u32 emitter, u32 row) {
    Emitter* e;
    EffectInst* n;
    EffectKind* k;
    if (!RowValid(row)) {
        return 0;
    }
    e = EmitterOf(emitter);
    if (e == NULL) {
        return 0;
    }
    n = fn_80086928(e->list, row, InstRowDiffers);
    if (n == NULL) {
        return 0;
    }
    k = KindOfRow(row);
    if (k != NULL && k->onAdd != NULL) {
        k->onAdd(&n->row);
    }
    return 1;
}

// Compare callback: nonzero when the instance is not for `row`.
static u32 InstRowDiffers(EffectInst* n, u32 row) {
    return (u32)(row - n->row | n->row - row) >> 31;
}

// Returns the instance count of effect `row` in `emitter`, or 0.
int fn_800F7C64(u32 emitter, int row) {
    Emitter* e;
    EffectInst* n;
    if (row == 0) {
        return 0;
    }
    e = EmitterOf(emitter);
    if (e == NULL) {
        return 0;
    }
    n = fn_80086928(e->list, row, (void*)RowIdDiffers);
    if (n == NULL) {
        return 0;
    }
    return n->count;
}

// Compare callback: nonzero when the instance's row has a different id than `id`.
static u32 RowIdDiffers(EffectInst* n, int id) {
    int rowId = ((EffectRow*)gEffectRows->data)[n->row].id;
    return (u32)(rowId - id | id - rowId) >> 31;
}

// Count of effect `row` in `emitter` (checked).
int fn_800F7D28(u32 emitter, u32 row) {
    Emitter* e;
    EffectInst* n;
    if (!RowValid(row)) {
        return 0;
    }
    e = EmitterOf(emitter);
    if (e == NULL) {
        return 0;
    }
    n = fn_80086928(e->list, row, (void*)InstRowDiffers);
    if (n == NULL) {
        return 0;
    }
    return n->count;
}

// Sends a message to the instance of `row` in `emitter` through its kind's message handler.
int Fn800F7DEC(int emitter, int row, int a, int b, void* c) {
    Emitter* e;
    EffectInst* n;
    EffectKind* k;
    if (!RowValid(row)) {
        return 0;
    }
    e = EmitterOf(emitter);
    if (e == NULL) {
        return 0;
    }
    n = fn_80086928(e->list, row, (void*)InstRowDiffers);
    if (n == NULL) {
        return 0;
    }
    k = KindOfRow(row);
    if (k == NULL || k->message == NULL) {
        return 0;
    }
    return k->message(&n->row, a, b, (int)c);
}

// Updates the instance of `row` in `emitter` through its kind's update handler.
int Fn800F7F5C(int emitter, int row, int a, int b, void* c, float dt) {
    Emitter* e;
    EffectInst* n;
    EffectKind* k;
    if (!RowValid(row)) {
        return 0;
    }
    e = EmitterOf(emitter);
    if (e == NULL) {
        return 0;
    }
    n = fn_80086928(e->list, row, (void*)InstRowDiffers);
    if (n == NULL) {
        return 0;
    }
    k = KindOfRow(row);
    if (k == NULL || k->update == NULL) {
        return 1;
    }
    return k->update(dt, &n->row, a, b, (int)c);
}

// Returns the group-handle value stored for emitter `handle`, or -1.
int fn_800F80DC(u32 handle) {
    Emitter* e = EmitterOf(handle);
    if (e != NULL) {
        return *(int*)((char*)e + 0xC);
    }
    return -1;
}

// Returns the row of the instance at index `i` in `emitter`, or 0.
int fn_800F8130(u32 handle, int i) {
    Emitter* e = EmitterOf(handle);
    if (e == NULL) {
        return 0;
    }
    if (i < 0 || fn_800869CC(e->list) <= i) {
        return 0;
    }
    {
        EffectInst* n = fn_800869A0(e->list, i);
        if (n == NULL) {
            return 0;
        }
        return n->row;
    }
}

// Number of instances in `emitter`.
int fn_800F81EC(u32 handle) {
    Emitter* e = EmitterOf(handle);
    if (e == NULL) {
        return 0;
    }
    return fn_800869CC(e->list);
}

// Removes `n` copies of effect `row` from `emitter`; returns how many were removed.
int fn_800F825C(u32 handle, u32 row, int n) {
    Emitter* e;
    EffectInst* inst;
    EffectKind* k;
    if (!RowValid(row)) {
        return 0;
    }
    e = EmitterOf(handle);
    if (e == NULL) {
        return 0;
    }
    inst = fn_80086928(e->list, row, (void*)InstRowDiffers);
    if (inst == NULL) {
        return 0;
    }
    k = KindOfRow(row);
    inst->count -= n;
    if (inst->count < 1) {
        n = inst->count + n;
        if (k != NULL && k->destroy != NULL) {
            k->destroy(&inst->row);
        }
        fn_800869EC(e->list, inst);
        inst->active = 0;
        fn_80086B20(&gInstPool, inst);
    }
    return n;
}

// Adds `n` copies of effect `row` to `emitter`; returns how many were added.
int Fn800F840C(int handle, int row, int n) {
    Emitter* e;
    EffectInst* inst;
    EffectKind* k;
    int before;
    float limit;
    if (!RowValid(row)) {
        return 0;
    }
    e = EmitterOf(handle);
    if (e == NULL) {
        return 0;
    }
    inst = fn_80086928(e->list, row, (void*)InstRowDiffers);
    if (inst == NULL) {
        int off;
        inst = (EffectInst*)fn_80086AC0(&gInstPool);
        if (inst == NULL) {
            return 0;
        }
        inst->active = 1;
        off = row * 0x10;
        k = KindOfRow(row);
        if (k != NULL && k->create != NULL) {
            inst->data = k->create(handle, row);
            if (inst->data == -1) {
                inst->active = 0;
                fn_80086B20(&gInstPool, inst);
                return 0;
            }
        } else {
            inst->data = 0;
        }
        inst->row = row;
        inst->emitter = handle;
        inst->count = n;
        limit = VarFindFloat(0x47F42363, *(u32*)(gEffectRows->data + off), 0x61837036);
        if ((int)limit < inst->count) {
            limit = VarFindFloat(0x47F42363, *(u32*)(gEffectRows->data + off), 0x61837036);
            inst->count = (int)limit;
        }
        fn_80086B20(e->list, inst);
        return inst->count;
    }
    before = inst->count;
    limit = VarFindFloat(0x47F42363, ((EffectRow*)gEffectRows->data)[row].id, 0x61837036);
    inst->count += n;
    if ((int)limit < inst->count) {
        inst->count = (int)limit;
    }
    return inst->count - before;
}

// Removes every effect from `handle` and returns the emitter to the pool.
void fn_800F86A0(u32 handle) {
    Emitter* e = EmitterOf(handle);
    int n;
    int i;
    if (e == NULL) {
        return;
    }
    n = fn_800F81EC(handle);
    for (i = 0; i < n; i++) {
        u32 row = fn_800F8130(handle, 0);
        int count = fn_800F7D28(handle, row);
        fn_800F825C(handle, row, count);
    }
    if (EmitterOf(handle) != NULL) {
        fn_80086B20(&gEmitterPool, e);
    }
}

// Allocates an emitter and returns its handle (-1 when the pool is empty).
u32 fn_800F8A30(int user) {
    Emitter* e = (Emitter*)fn_80086AC0(&gEmitterPool);
    if (e == NULL) {
        return 0xFFFFFFFF;
    }
    *(int*)((char*)e + 8) = 0;
    *(int*)((char*)e + 0xC) = user;
    return ((((int)e - (int)gEmitters) >> 4) & 0xFFFFFF) | 0x4C000000;
}

// Index of the effect row whose id is `id` (-1 if none).
int Fn800F8A98(u32 id) {
    int i = 0;
    int off = 0;
    u32 n;
    if (id == 0) {
        return -1;
    }
    n = gEffectRows->bytes >> 4;
    while (true) {
        if (n == 0) {
            return -1;
        }
        if (id == *(u32*)(gEffectRows->data + off)) {
            break;
        }
        i++;
        off += 0x10;
        n--;
    }
    return i;
}

// Id of effect row `row`, or 0.
u32 fn_800F8AF4(u32 row) {
    if ((int)row >= 0 && row < (gEffectRows->bytes >> 4)) {
        return ((EffectRow*)gEffectRows->data)[row].id;
    }
    return 0;
}
// Shuts the emitter system down: clears every emitter, runs the kinds' shutdown handlers and
// empties and deletes the row and kind vectors.
extern "C" void fn_8028B35C(Vector* v, void* it, int bytes);
void EffectEmitterShutdown(void);
void fn_800F86A0(u32 handle);

// Calls one handler (by offset in the kind record) of every effect kind. The byte offset is a second
// induction variable in the source: retail steps the counter before the offset, which the compiler's own
// strength reduction never does.
static inline void EffectKindsCall(int handler) {
    u32 k;
    int off;
    for (k = 0, off = 0; k < gEffectKinds->bytes >> 2; k++, off += 4) {
        EffectKind* kind = *(EffectKind**)(gEffectKinds->data + off);
        if (kind != NULL && *(void (**)(void))((char*)kind + handler) != NULL) {
            (*(void (**)(void))((char*)kind + handler))();
        }
    }
}

// Vector clear: erase(begin, end) on the typed element range.
static inline void EffectRowsClear(Vector* v) {
    u32 bytes = v->bytes;
    if (bytes >> 4 != 0) {
        EffectRow* first = (EffectRow*)v->data;
        EffectRow* last = (EffectRow*)(v->data + bytes);
        fn_8028B35C(v, first, (last - first) * sizeof(EffectRow));
    }
}

static inline void EffectKindsClear(Vector* v) {
    u32 bytes = v->bytes;
    if (bytes >> 2 != 0) {
        EffectKind** first = (EffectKind**)v->data;
        EffectKind** last = (EffectKind**)(v->data + bytes);
        fn_8028B35C(v, first, (last - first) * sizeof(EffectKind*));
    }
}

void EffectEmitterShutdown(void) {
    u32 i;
    u32 bytes;
    fn_800EB744();
    i = 0;
    do {
        fn_800F86A0(i & 0xFFFFFF | 0x4C000000);
        i++;
    } while ((int)i < 0x100);
    EffectKindsCall(0x10);
    EffectRowsClear(gEffectRows);
    EffectKindsClear(gEffectKinds);
    delete gEffectKinds;
    delete gEffectRows;
}

extern EffectInst gInstances[0x200];    // 0x80458F70
extern "C" {
float fn_802A2080(void);
int SendMsg(int to, int msg, void* data);
void fn_80086B60(void* pool, void* n);
int VarGroupObjCount(u32 table);                    // 0x800DED98
int VarObjKey(u32 table, int index);                // 0x800DF160
int Fn800DF814(u32 table, int key);                 // 0x800DF814
int GameEvent(int a, int b);                        // 0x8007EA84
void* VAlloc(int size);
void fn_8028BA9C(void* v);
void fn_800EB834(void);
void* memset(void* p, int c, u32 n);
}

struct ActorBits {
    u8 pad00[0x4C];
    u32 flags;              // 0x4C
};

// Updates every live instance whose kind has an update handler (dt is scaled by the time
// scale unless the owner is exempt).
void EmittersUpdate(float dt) {
    int i;
    EffectInst* n = gInstances;
    for (i = 0; i < 0x200; i++, n++) {
        if (n->active != 0) {
            EffectKind* k = KindOfRow(n->row);
            if (k != NULL && *(int*)((char*)k + 0x24) != 0) {
                Emitter* e = EmitterOf(n->emitter);
                int handle = (e == NULL) ? -1 : *(int*)((char*)e + 0xC);
                struct {
                    ActorBits* actor;
                    int pad[9];
                } info;
                float step = dt;
                if (SendMsg(handle, 0x400, &info) != 0 && info.actor != NULL &&
                    (info.actor->flags & 0x20000) == 0) {
                    step = dt * fn_802A2080();
                }
                (*(void (**)(float, void*))((char*)k + 0x24))(step, &n->row);
            }
        }
    }
}

// Calls every live instance's "added" handler.
void fn_800F8ED0(void) {
    int i;
    EffectInst* n = gInstances;
    for (i = 0; i < 0x200; i++, n++) {
        if (n->active != 0) {
            EffectKind* k = KindOfRow(n->row);
            if (k != NULL && k->onAdd != NULL) {
                k->onAdd(&n->row);
            }
        }
    }
}

// Builds the effect row table from the "effects" variable group and runs each kind's
// post-load handler.
int EffectTablesLoad(void) {
    int count = VarGroupObjCount(0x47F42363);
    int off;
    u32 table;
    EffectKind* kind;
    u32 k;
    int i;
    for (i = 0; i < count; i++) {
        int key = VarObjKey(0x47F42363, i);
        int index;
        u32 hash;
        u32 j;
        int o;
        int ev;
        EffectRow* row;
        kind = NULL;
        hash = VarGetHash(0x47F42363, i, 0x3DC50993);
        for (j = 0; j < gEffectKinds->bytes >> 2; j++) {
            EffectKind* c = ((EffectKind**)gEffectKinds->data)[j];
            if (hash == c->hash) {
                kind = c;
                break;
            }
        }
        table = VarGetHash(0x47F42363, i, 0x7300AB12);
        ev = Fn800DF3B8(0x47F42363, i, 0x8DD0EEE7);
        index = Fn800DF814(table, GameEvent(0xC0A77187, ev));
        row = (EffectRow*)fn_8028B3BC(gEffectRows, gEffectRows->data + gEffectRows->bytes, 0x10);
        row->id = key;
        row->unk04 = (int)kind;
        row->table = table;
        row->index = index;
    }
    for (k = 0, off = 0; k < gEffectKinds->bytes >> 2; k++, off += 4) {
        EffectKind* kind = *(EffectKind**)(gEffectKinds->data + off);
        if (kind != NULL && *(void (**)(void))((char*)kind + 0xC) != NULL) {
            (*(void (**)(void))((char*)kind + 0xC))();
        }
    }
    return 1;
}

// Sets up the emitter and instance pools and the effect vectors.
int EffectEmitterInit(void) {
    int i;
    char* p;
    memset(gEmitters, 0, 0x1000);
    memset(gInstances, 0, 0x3800);
    gEmitterPool = 0;
    p = (char*)gEmitters;
    i = 0;
    gInstPool = 0;
    do {
        fn_80086B60(&gEmitterPool, p);
        i++;
        p += 0x10;
    } while (i < 0x100);
    i = 0;
    p = (char*)gInstances;
    do {
        fn_80086B60(&gInstPool, p);
        i++;
        p += 0x1C;
    } while (i < 0x200);
    {
        Vector* v = (Vector*)VAlloc(0x14);
        if (v != NULL) {
            fn_8028BA9C(v);
        }
        gEffectKinds = v;
        v = (Vector*)VAlloc(0x14);
        if (v != NULL) {
            fn_8028BA9C(v);
        }
        gEffectRows = v;
    }
    fn_800EB834();
    EffectKindsCall(8);
    return 1;
}

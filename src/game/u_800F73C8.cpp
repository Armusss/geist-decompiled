// u_800F73C8.cpp: streamed audio / stream list and the game variable tables (0x800F73C8-0x800FDDE4).
// A linked list of DVD-streamed music entries with the DVD stream callbacks (prepare, play,
// stop, error), the game variable table accessors (hash lookups by table row), camera field of
// view helpers, the memory checker and the player team / handle lookups.  Names are descriptive
// guesses; unmatched against retail so far.

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

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float m[12];
};


class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    virtual void Release(char* p);

    char* mData;
    int mLength;
    int mUnk0C;
    int mOwned;
};

class String : public Buffer {
public:
    String();
    String(const char* s);
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

extern "C" int SendMsg(int to, int msg, void* data);

extern unsigned char __upper_map[256];
inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

// ---- stream list -------------------------------------------------------------------------

struct DVDCommandBlock {
    u8 data[0x30];
};

struct StreamNode {
    u8 pad00[4];
    StreamNode* next;       // 0x04
    u8 pad08[4];
    u32 events;             // 0x0C: which events invoke the callback
    void (*callback)(void); // 0x10
    u8 file[1];             // 0x14: DVDFileInfo
};

extern "C" {
void AISetStreamPlayState(int state);
void AISetStreamTrigger(int t);
void AISetStreamVolLeft(int v);
void AISetStreamVolRight(int v);
void AIResetStreamSampleCount(void);
int DVDClose(void* file);
int DVDGetStreamPlayAddrAsync(DVDCommandBlock* b, void* cb);
int DVDGetStreamErrorStatusAsync(DVDCommandBlock* b, void* cb);
int DVDPrepareStreamAsync(void* file, int a, int b, void* cb);
int DVDStopStreamAtEndAsync(DVDCommandBlock* b, void* cb);
}

extern StreamNode* gStreamCur;          // 0x804D4B58
extern StreamNode* gStreamHead;         // 0x804D4B5C
extern int gStreamTail;                 // 0x804D4B60
extern int gStreamState;                // 0x804D4B64
extern int gStreamMode;                 // 0x804D4B68
extern int gStreamPos;                  // 0x804D4B6C
extern int gStreamTrigger;              // 0x804D4B70
extern u8 gStreamVolL;                  // 0x804D4B74
extern u8 gStreamVolR;                  // 0x804D4B75
extern int gStreamX;                    // 0x804D4B78
extern int gStreamEnded;                // 0x804D4B7C
extern void (*gStreamCleanup)(void);    // 0x804D4B80
extern DVDCommandBlock gStreamBlkStop;  // 0x80457E80
extern DVDCommandBlock gStreamBlkPlay;  // 0x80457E50
extern DVDCommandBlock gStreamBlkAddr;  // 0x80457EE0
extern DVDCommandBlock gStreamBlkErr;   // 0x80457EB0

void StreamPrepared(void);
void StreamErrorDone(unsigned char status);
void StreamAddrDone(u32 pos);

// Calls the current entry's callback when it asked for event `bit`.
static inline void StreamNotify(u32 bit) {
    if (gStreamCur != NULL) {
        if (gStreamCur->callback != NULL && (gStreamCur->events & bit) != 0) {
            gStreamCur->callback();
        }
    }
}

// Stream finished: notify (bit 2) and clear the state.
void StreamFinished(void) {
    StreamNotify(2);
    gStreamState = 0;
}

// Closes every stream entry and resets the list.
void MusicStreamShutdown(void) {
    StreamNode* n;
    AISetStreamPlayState(0);
    for (n = gStreamHead; n != NULL; n = n->next) {
        DVDClose(n->file);
    }
    gStreamHead = NULL;
    gStreamTail = 0;
    gStreamCur = NULL;
    gStreamState = 0;
    if (gStreamCleanup != NULL) {
        gStreamCleanup();
        gStreamCleanup = NULL;
    }
    gStreamState = 0;
    gStreamX = 0;
}

void StreamSetTrigger(int offset) {
    AISetStreamTrigger(offset + gStreamTrigger);
    if (gStreamCur != NULL) {
        DVDGetStreamPlayAddrAsync(&gStreamBlkAddr, (void*)StreamAddrDone);
    }
}

// Advances to the next entry (or stops/loops by mode) when the current one ends.
void StreamAddrDone(u32 pos) {
    gStreamPos = pos;
    if (gStreamEnded == 0) {
        DVDGetStreamErrorStatusAsync(&gStreamBlkErr, (void*)StreamErrorDone);
    } else {
        gStreamEnded = 0;
        StreamNotify(0x10);
        gStreamState = 3;
        if (gStreamMode == 1) {
            if (gStreamCur != NULL) {
                gStreamCur = gStreamCur->next;
                if (gStreamCur == NULL) {
                    gStreamCur = gStreamHead;
                    AISetStreamVolLeft(0);
                    AISetStreamVolRight(0);
                    AISetStreamPlayState(0);
                    DVDPrepareStreamAsync(gStreamCur->file, 0, 0, (void*)StreamPrepared);
                } else {
                    AISetStreamVolLeft(0);
                    AISetStreamVolRight(0);
                    AISetStreamPlayState(0);
                    DVDPrepareStreamAsync(gStreamCur->file, 0, 0, (void*)StreamPrepared);
                }
            }
        } else if (gStreamMode < 1) {
            if (gStreamMode >= 0 && gStreamCur != NULL) {
                gStreamCur = gStreamCur->next;
                if (gStreamCur == NULL) {
                    gStreamCur = gStreamHead;
                    AISetStreamVolLeft(0);
                    AISetStreamVolRight(0);
                    AISetStreamPlayState(0);
                    gStreamState = 0;
                } else {
                    AISetStreamVolLeft(0);
                    AISetStreamVolRight(0);
                    AISetStreamPlayState(0);
                    DVDPrepareStreamAsync(gStreamCur->file, 0, 0, (void*)StreamPrepared);
                }
            }
        } else if (gStreamMode < 3 && gStreamCur != NULL) {
            AISetStreamVolLeft(0);
            AISetStreamVolRight(0);
            AISetStreamPlayState(0);
            DVDPrepareStreamAsync(gStreamCur->file, 0, 0, (void*)StreamPrepared);
        }
    }
}

// Stream stopped: silence it and notify (bit 0x20).
void StreamStop(void) {
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    AISetStreamPlayState(0);
    DVDStopStreamAtEndAsync(&gStreamBlkStop, NULL);
    gStreamState = 2;
    StreamNotify(0x20);
}

// Stream prepared: start playback and notify (bit 1).
void StreamPrepared(void) {
    AISetStreamVolLeft(gStreamVolL);
    AISetStreamVolRight(gStreamVolR);
    AIResetStreamSampleCount();
    AISetStreamTrigger(gStreamTrigger);
    AISetStreamPlayState(1);
    DVDStopStreamAtEndAsync(&gStreamBlkPlay, NULL);
    gStreamState = 1;
    StreamNotify(1);
}

// DVD error status callback.
void StreamErrorDone(unsigned char status) {
    if (status != 0) {
        return;
    }
    gStreamEnded = 1;
    gStreamPos = 0;
}

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

struct Vector {
    u8 pad00[8];
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
    if ((int)row < 0 || gEffectRows->bytes >> 4 <= row) {
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

void EffectEmitterShutdown(void) {
    u32 i;
    u32 bytes;
    fn_800EB744();
    i = 0;
    do {
        fn_800F86A0(i & 0xFFFFFF | 0x4C000000);
        i++;
    } while ((int)i < 0x100);
    {
        u32 k;
        int off = 0;
        for (k = 0; k < gEffectKinds->bytes >> 2; k++) {
            EffectKind* kind = *(EffectKind**)(gEffectKinds->data + off);
            if (kind != NULL && *(void (**)(void))((char*)kind + 0x10) != NULL) {
                (*(void (**)(void))((char*)kind + 0x10))();
            }
            off += 4;
        }
    }
    bytes = gEffectRows->bytes;
    if (bytes >> 4 != 0) {
        fn_8028B35C(gEffectRows, gEffectRows->data, ((int)bytes / 16) * 16);
    }
    bytes = gEffectKinds->bytes;
    if (bytes >> 2 != 0) {
        fn_8028B35C(gEffectKinds, gEffectKinds->data, ((int)bytes / 4) * 4);
    }
    if (gEffectKinds != NULL) {
        ((void (*)(Vector*, int))(*(void***)gEffectKinds)[2])(gEffectKinds, 1);
    }
    if (gEffectRows != NULL) {
        ((void (*)(Vector*, int))(*(void***)gEffectRows)[2])(gEffectRows, 1);
    }
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
    int i;
    u32 k;
    int off;
    for (i = 0; i < count; i++) {
        int key = VarObjKey(0x47F42363, i);
        int hash = VarGetHash(0x47F42363, i, 0x3DC50993);
        EffectKind* kind = NULL;
        u32 n = gEffectKinds->bytes >> 2;
        int o = 0;
        u32 table;
        int index;
        int ev;
        EffectRow* row;
        while (true) {
            if (n == 0) {
                kind = NULL;
                break;
            }
            kind = *(EffectKind**)(gEffectKinds->data + o);
            if (hash == (int)kind->hash) {
                break;
            }
            o += 4;
            n--;
        }
        table = VarGetHash(0x47F42363, i, 0x7300AB12);
        ev = GameEvent(0xC0A77187, Fn800DF3B8(0x47F42363, i, 0x8DD0EEE7));
        index = Fn800DF814(table, ev);
        row = (EffectRow*)fn_8028B3BC(gEffectRows, gEffectRows->data + gEffectRows->bytes, 0x10);
        row->id = key;
        row->unk04 = (int)kind;
        row->table = table;
        row->index = index;
    }
    off = 0;
    for (k = 0; k < gEffectKinds->bytes >> 2; k++) {
        EffectKind* kind = *(EffectKind**)(gEffectKinds->data + off);
        if (kind != NULL && *(void (**)(void))((char*)kind + 0xC) != NULL) {
            (*(void (**)(void))((char*)kind + 0xC))();
        }
        off += 4;
    }
    return 1;
}

// Sets up the emitter and instance pools and the effect vectors.
int EffectEmitterInit(void) {
    u32 k;
    int i;
    char* p;
    int off;
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
    off = 0;
    for (k = 0; k < gEffectKinds->bytes >> 2; k++) {
        EffectKind* kind = *(EffectKind**)(gEffectKinds->data + off);
        if (kind != NULL && *(void (**)(void))((char*)kind + 8) != NULL) {
            (*(void (**)(void))((char*)kind + 8))();
        }
        off += 4;
    }
    return 1;
}

// ---- cameras -----------------------------------------------------------------------------
// 16 camera objects (0x124 bytes each) behind handles 0x16xxxxxx; the objects wrap the
// camera controller (+0x60) and an optional shake controller (+0x64).

struct CamObj {
    u8 pad00[0x34];
    float fov;              // 0x34
    u8 pad38[0x60 - 0x38];
    int ctl;                // 0x60
    int ctl2;               // 0x64
    int unk68;              // 0x68
    u8 pad6C[0xF8 - 0x6C];
    int ownerId;            // 0xF8
    int flagA;              // 0xFC
    int flagB;              // 0x100
    int unk104;             // 0x104
    u8 pad108[0x124 - 0x108];
};

extern CamObj gCams[16];        // 0x8045C7D0 - 0x60 ... (object base 0x8045C770)
extern int gCamLetterbox;       // 0x804D4B98
extern int gCamCount;           // 0x804D4B9C
extern float gCamNeg1;          // 0x804D7498
extern float gCamZero2;         // 0x804D749C

extern "C" {
int fn_802A1D04(void);
void fn_802A1EBC(int mode);
void fn_800C6E88(void);
void fn_800C6560(int ctl);
void fn_800C6760(int ctl);
void fn_800C6834(int ctl);
int fn_800C6AE8(void* obj, void* a, void* b, int c);
void fn_80026660(int msg, void* fn, int arg);
}

void CamObjUpdate(float dt, CamObj* c);
void CamObjEase(CamObj* c, int a, int b, int d, int e);
int CamMsg(u32 handle, int msg, int* data);

static inline int CamIndexOk(u32 handle) {
    return (handle & 0xFFFFFF) < 0x10;
}

// Base of the camera array plus 8 (the first object's second field).
void* CameraTable(void) {
    return (char*)gCams + 0xC8;
}

// Turns the camera letterbox on or off.
void fn_800F9468(int on) {
    if (fn_802A1D04() == 0) {
        if (on == 0) {
            gCamLetterbox = 0;
            fn_802A1EBC(0);
        } else {
            gCamLetterbox = 1;
            fn_802A1EBC(9);
        }
    }
}

// Controller handle of camera `handle`, or -1.
int fn_800F94C8(u32 handle) {
    if ((handle & 0xFFFFFF) > 0xF) {
        return -1;
    }
    return gCams[handle & 0xFFFFFF].ctl;
}

void fn_800F94F8(u32 handle, short v) {
    if ((handle & 0xFFFFFF) < 0x10) {
        gCams[handle & 0xFFFFFF].flagB = v;
    }
}

// Starts an ease on a camera object (does nothing when one is running unless `force`).
void CamObjEase(CamObj* c, int a, int b, int d, int force) {
    if (c == NULL) {
        return;
    }
    if (b == 0) {
        return;
    }
    if (c->unk104 != 0 && force == 0) {
        return;
    }
    c->unk104 = a;
    *(int*)((char*)c + 0x108) = b;
    *(int*)((char*)c + 0x10C) = d << 12;
    *(int*)((char*)c + 0x110) = *(int*)((char*)c + 0x10C) / b;
    *(int*)((char*)c + 0x114) = 0;
    *(int*)((char*)c + 0x118) = 0;
}

void fn_800F9578(u32 handle, int v) {
    if ((handle & 0xFFFFFF) < 0x10) {
        gCams[handle & 0xFFFFFF].flagA = v;
    }
}

// Sets the field of view of camera `handle`.
void CameraSetFov(int handle, float fov) {
    if (((u32)handle & 0xFFFFFF) < 0x10) {
        gCams[(u32)handle & 0xFFFFFF].fov = fov;
    }
    fn_800C6E88();
}

float fn_800F95E4(u32 handle) {
    if ((handle & 0xFFFFFF) > 0xF) {
        return gCamNeg1;
    }
    return gCams[handle & 0xFFFFFF].fov;
}

int fn_800F9614(u32 handle) {
    int r;
    if ((handle & 0xFFFFFF) < 0x10) {
        r = (int)gCams[handle & 0xFFFFFF].fov;
    } else {
        r = -1;
    }
    return r;
}

void CameraActivate(u32 handle) {
    if ((handle & 0xFFFFFF) < 0x10) {
        fn_800C6560(gCams[handle & 0xFFFFFF].ctl);
    }
}

void CameraDeactivate(u32 handle) {
    if ((handle & 0xFFFFFF) < 0x10) {
        fn_800C6760(gCams[handle & 0xFFFFFF].ctl);
    }
}

void fn_800F96E0(u32 handle) {
    if ((handle & 0xFFFFFF) < 0x10) {
        fn_800C6834(gCams[handle & 0xFFFFFF].ctl);
    }
}

// Creates a camera object and returns its handle (-1 when all 16 are used).
u32 fn_800F9724(int owner, int id) {
    u32 idx = gCamCount;
    if ((int)idx < 0x10) {
        CamObj* c = &gCams[idx];
        gCamCount++;
        c->ctl = fn_800C6AE8(c, (void*)CamObjUpdate, (void*)CamObjEase, id);
        if (fn_802A1D04() == 0) {
            c->ctl2 = fn_800C6AE8(&c->ctl2, NULL, NULL, id + 1);
            fn_800C6834(c->ctl2);
        } else {
            c->ctl2 = -1;
        }
        c->ownerId = owner;
        c->flagA = 0;
        c->flagB = 0;
        c->unk104 = 0;
        CamObjUpdate(gZeroF, c);
        return idx & 0xFFFFFF | 0x16000000;
    }
    return 0xFFFFFFFF;
}

// Registers the camera message handler (type 0x16) and clears the count.
void CameraSystemInit(void) {
    fn_80026660(0x16, (void*)CamMsg, 0);
    gCamCount = 0;
}

struct IMtx {
    int m[12];
};

extern "C" {
int fn_800A1624(int angle);
int fn_800A1688(int angle);
void fn_8009F76C(int* m);
void MtxMulQ12(int* out, int* a, int* b);                   // 0x8009F258
void fn_8009F5B0(int* m);
void fn_8009F50C(int* out, int* in);
int rand(void);
void fn_8009F63C(int* m, int* actor);
void fn_8009F44C(int* out, int* m, int* in);
void PlacementToMatrix(int* m, int* p);                     // 0x8009EF04
void ActorGetMatrix(int* m, int* actor);
void MakeVec(int* v, int x, int y, int z);                  // 0x8016D430
void fn_8009F028(int* out, int* v);
void fn_8009FC6C(int* out, int* a, int* b);
void MtxSetEuler(int* m, float* e);                         // 0x8009FAC0
}

// Shake record (inside the camera object): +8 amplitude, +0x10 flags, +0x14 phase.
struct Shake {
    u8 pad00[8];
    int amp;                // 0x08
    u8 pad0C[4];
    u32 flags;              // 0x10
    int phase;              // 0x14
};

// Sine-wave roll shake applied to the camera matrix.
void ShakeRoll(Shake* s, int* m) {
    int sinA = fn_800A1624((s->phase & 0x1FF) << 3);
    int cosA = fn_800A1688((s->phase & 0x1FF) << 3);
    int sinB = fn_800A1624(s->phase * 4 + 0x800 & 0xFFF);
    int div = 0x101 - s->amp / 0x1000;
    int rot[12];
    int tmp[12];
    int i;
    fn_8009F76C(rot);
    rot[0] = 0x1000 - (sinA << 2) / div;
    rot[11] = (sinB << 2) / div + 0x1000;
    rot[8] = (cosA << 1) / div + 0x1000;
    for (i = 0; i < 12; i++) {
        tmp[i] = m[i];
    }
    MtxMulQ12(m, rot, tmp);
}

// Slides the camera along its view axis by the shake amplitude.
void ShakeSlide(Shake* s, int* pos) {
    int v[3];
    int out[3];
    if ((s->flags & 1) == 0) {
        v[0] = s->amp / 0x8000;
    } else {
        v[0] = -s->amp / 0x8000;
    }
    v[1] = s->amp / 0x1000;
    v[2] = -s->amp / 0x2000;
    fn_8009F5B0(pos);
    fn_8009F50C(out, v);
    pos[9] += out[0];
    pos[10] += out[1];
    pos[11] += out[2];
}

// Random jitter shake: offsets the position by +-amplitude on x and z.
void ShakeJitter(Shake* s, int* pos) {
    int v[3];
    int out[3];
    int mtx[12];
    int amp;
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
    amp = s->amp / 0x1000;
    if (amp > 0) {
        u32 r = rand();
        v[0] = -amp;
        if ((r & 1 ^ r >> 31) != r >> 31) {
            v[0] = amp;
        }
        r = rand();
        v[2] = amp;
        if ((r & 1 ^ r >> 31) == r >> 31) {
            v[2] = -amp;
        }
    }
    fn_8009F63C(mtx, pos);
    fn_8009F44C(out, mtx, v);
    pos[9] += out[0];
    pos[10] += out[1];
    pos[11] += out[2];
}

extern void (*gShakeFns[])(void* shake, int* m);    // 0x8039CF54 (indexed by shake kind)
extern float gShakeAngle;       // 0x804D7490 (double bias)
extern int gCamTilt;            // 0x804D2D70
extern float gTiltRow[2];       // 0x80345378

// Per-frame update of a camera object: copies the controller's matrix, applies the offset,
// shake and roll, and mirrors it for the split view.
void CamObjUpdate(float dt, CamObj* c) {
    int* cm = (int*)c;
    int* src = (int*)cm[0x3E];
    float* off;
    int i;
    for (i = 0; i < 12; i++) {
        cm[i] = src[i];
    }
    off = (float*)cm[0x3F];
    if (off != NULL) {
        ((float*)cm)[9] += off[0];
        ((float*)cm)[10] += off[1];
        ((float*)cm)[11] += off[2];
    }
    if (cm[0x41] != 0) {
        int m[12];
        PlacementToMatrix(m, cm);
        gShakeFns[cm[0x41]](cm + 0x41, m);
        {
            int step = (int)dt;
            cm[0x42] -= step;
            if (cm[0x42] < 0) {
                cm[0x41] = 0;
            } else {
                cm[0x43] -= cm[0x44] * step;
                if (cm[0x43] < 0) {
                    cm[0x41] = 0;
                }
                cm[0x45] += 1;
                cm[0x46] += step;
            }
        }
        ActorGetMatrix(cm, m);
    }
    if (cm[0x40] != 0) {
        int v[3];
        int a[12];
        int b[12];
        v[1] = 0;
        v[2] = 0;
        v[0] = cm[0x40];
        MakeVec(v, 0, 0, 0);
        fn_8009F028(a, v);
        ActorGetMatrix(b, a);
        fn_8009FC6C(cm, b, cm);
    }
    for (i = 0; i < 12; i++) {
        cm[0x32 + i] = cm[i];
    }
    if (gCamLetterbox != 0) {
        float tilt[3];
        float rot[12];
        tilt[0] = gTiltRow[0];
        tilt[1] = gTiltRow[1];
        for (i = 0; i < 24; i++) {
            cm[0x18 + i] = cm[i];
        }
        tilt[2] = (float)(-gCamTilt);
        MtxSetEuler((int*)rot, tilt);
        fn_8009FC6C(cm + 0x18, (int*)rot, cm + 0x32);
        tilt[2] = (float)gCamTilt;
        MtxSetEuler((int*)rot, tilt);
        fn_8009FC6C(cm, (int*)rot, cm + 0x32);
    }
}

// Camera message handler (type 0x16): message 0x1C01 copies the requested fields out of the
// camera object.
int CamMsg(u32 handle, int msg, int* data) {
    CamObj* c;
    if (msg != 0x1C01) {
        return 0;
    }
    c = &gCams[handle & 0xFFFFFF];
    if ((data[1] & 1) != 0) {
        data[2] = *(int*)((char*)c + 0x30);
    }
    if ((data[1] & 2) != 0) {
        data[3] = *(int*)((char*)c + 0x34);
    }
    if ((data[1] & 4) != 0) {
        data[4] = *(int*)((char*)c + 0x40);
    }
    if ((data[1] & 8) != 0) {
        data[5] = *(int*)((char*)c + 0x44);
    }
    if ((data[1] & 0x10) != 0) {
        data[6] = *(int*)((char*)c + 0x48);
    }
    if ((data[1] & 0x20) != 0) {
        data[7] = *(int*)((char*)c + 0x4C);
        data[8] = *(int*)((char*)c + 0x50);
        data[9] = *(int*)((char*)c + 0x54);
    }
    if ((data[1] & 0x40) != 0) {
        data[10] = c->ctl;
    }
    return 1;
}

// ---- minigame engine / player teams ------------------------------------------------------

struct GameType {
    const char* name;       // 0x00
    void (*init)(void);     // 0x04
    void (*reset)(void);    // 0x08
    void (*term)(void);     // 0x0C
    void (*update)(void);   // 0x10
    void (*draw)(void* mg, int arg);    // 0x14
    u8 pad18[4];
    void (*restart)(void);  // 0x1C
};

struct Minigame {
    int type;               // 0x00 (0x8039E748)
    int unk04;
    int unk08;              // 0x08
    u32 flags;              // 0x0C
    int unk10;              // 0x10
    int unk14;              // 0x14
    int unk18;              // 0x18
    int count;              // 0x1C: number of players
    int bots;               // 0x20
    int handle[8];          // 0x24
    char name[8][16];       // 0x44
    int unkC4;              // 0xC4
    int team[8];            // 0xC8
    u8 padE8[0x12C - 0xE8];
    u32 flags12C;           // 0x12C
    u32 flags130;           // 0x130
    int state;              // 0x134
    u8 pad138[0x160 - 0x138];
};

extern Minigame gMinigame;              // 0x8039E748
extern GameType* gGameTypes[6];         // 0x8039E730
extern char gTeamColors[9][16];         // 0x803E9938
extern char gTeamColors2[9][16];        // 0x803E98A8
extern char gTeamNameBuf[0x40];         // 0x8045D9B0

extern "C" {
int fn_802A1FA0(void);
int strlen(const char* s);
char* strncpy(char* d, const char* s, u32 n);
int strncmp(const char* a, const char* b, u32 n);
int sprintf(char* s, const char* fmt, ...);
void fn_800872BC(int level, const char* msg);
int fn_80142E94(void);
int fn_800B705C(void);
u32 fn_80153B9C(void);
void* PlayerGetPossessor(int i);                            // 0x80049888
int Rand(void);                                             // 0x8000DDD0
void StatSet(int a, int b, int c, float v);                 // 0x80120AC0
const char* StrFindKeySkip(u32 key);                        // 0x80086364
int ActorCount(void);                                       // 0x80049930
void fn_80128C8C(void);
void fn_80140984(void);
void fn_801289D0(GameType* t);
void HudRefreshSlots(void* hud);                            // 0x800FE518
void fn_80128D7C(void);
void fn_800094D8(int a, int b);
void fn_802A1D0C(void);
void fn_802D1D7C(int a);
void fn_8027D3E4(float f);
void EmitterSetMode(int m);                                 // 0x800D8BD4
void fn_8014C858(void);
}

extern int gMinigameLevel;      // 0x804D2D88
extern int gStatThreshold;      // 0x804D2D90
extern int gStatMid;            // 0x804D2D94
extern int gStatHigh;           // 0x804D2D98
extern double gStatBias;        // 0x804D74F0
extern float gMinigameZoom;     // 0x804D74F8

extern "C" int fn_800FDDE4(u32 handle, int msg, int* data);   // message handler of the next unit
int MinigameMsgB(u32 handle, int msg, int* data);

// Stores/returns the minigame's scratch word.
void MinigameSetScratch(int v) {
    *(int*)((char*)&gMinigame + 8) = v;
}

int MinigameGetScratch(void) {
    return *(int*)((char*)&gMinigame + 8);
}

// Sets rule flag `id` (0..0x13 bits, 0x14.. dedicated words).
void MinigameSetRule(int id, int on) {
    if (id == 0x16) {
        gMinigame.unk14 = on;
        return;
    }
    if (id > 0x15) {
        if (id > 0x17) {
            return;
        }
        gMinigame.unk18 = on;
        return;
    }
    if (id > 0x14) {
        gMinigame.unk10 = on;
        return;
    }
    if (id < 0) {
        return;
    }
    if (on != 0) {
        gMinigame.flags |= 1 << id;
        return;
    }
    gMinigame.flags &= ~(1 << id);
}

u32 fn_800FC3B8(int id) {
    if (id == 0x16) {
        return gMinigame.unk14;
    }
    if (id < 0x16) {
        if (id > 0x14) {
            return gMinigame.unk10;
        }
        if (id >= 0) {
            return gMinigame.flags & 1 << id;
        }
    } else if (id < 0x18) {
        return gMinigame.unk18;
    }
    return 0;
}

// Loads the rule words from `p` (4 words).
void MinigameLoadRules(int* p) {
    if (p == NULL) {
        return;
    }
    gMinigame.flags = p[0];
    gMinigame.unk10 = p[1];
    gMinigame.unk14 = p[2];
    gMinigame.unk18 = p[3];
}

// Saves the rule words to `p` (4 words).
void MinigameSaveRules(int* p) {
    if (p == NULL) {
        return;
    }
    p[0] = gMinigame.flags;
    p[1] = gMinigame.unk10;
    p[2] = gMinigame.unk14;
    p[3] = gMinigame.unk18;
}

// Picks the game type from the name of the running level file.
int SysCheckMemory(void) {
    char name[0x401];
    char* path = (char*)fn_802A1FA0();
    int len = strlen(path);
    int slash = -1;
    int i;
    int start;
    int dot;
    int n;
    for (i = 0; i < len; i++) {
        if (path[i] == '\\' || path[i] == '/') {
            slash = i;
        }
    }
    start = slash + 1;
    dot = len;
    for (i = start; i < len; i++) {
        if (path[i] == '.') {
            dot = i;
            break;
        }
    }
    n = dot - slash - 1;
    strncpy(name, path + slash + 1, n);
    name[dot - slash - 1] = 0;
    for (i = 0; i < n; i++) {
        unsigned char c = name[i];
        name[i] = (c == 0xFF) ? 0xFF : __upper_map[c];
    }
    if (strncmp(name, "MAINMENU", 8) == 0) {
        fn_800872BC(0x40000, "Minigame : GameType FEAR_MAIN_MENU");
        return 4;
    }
    if (strncmp(name, "CREDITS", 7) == 0) {
        fn_800872BC(0x40000, "Minigame : GameType FEAR_CREDITS");
        return 5;
    }
    if (strncmp(name, "HUNT_", 5) == 0) {
        fn_800872BC(0x40000, "Minigame : GameType FEAR_HUNT");
        return 3;
    }
    if (strncmp(name, "EVAC_", 5) == 0) {
        fn_800872BC(0x40000, "Minigame : GameType FEAR_HUNT (evac detected)");
        return 3;
    }
    {
        int mode = fn_80142E94();
        if (mode == 2) {
            fn_800872BC(0x40000, "Default Minigame : GameType FEAR_STORY_MODE");
        } else {
            if (mode < 2) {
                if (mode == 0) {
                    fn_800872BC(0x40000, "Minigame : GameType FEAR_DM");
                    return 2;
                }
                if (mode >= 0) {
                    fn_800872BC(0x40000, "Minigame : GameType FEAR_CTH");
                    return 1;
                }
            } else if (mode == 4) {
                fn_800872BC(0x40000, "Default Minigame : GameType FEAR_STORY_MODE");
                return 0;
            }
            fn_800872BC(0x40000, "Minigame : GameType FEAR_STORY_MODE");
        }
    }
    return 0;
}

// Is the current level one of the four deathmatch-style maps?
int MinigameIsArenaMap(void) {
    int id = fn_800B705C();
    if (id != 0xD5B0588) {
        if (id < 0xD5B0588) {
            if (id != -0x5B49449E && (id > -0x5B49449F || id != -0x5D3DBB55)) {
                return 0;
            }
        } else if (id != 0x5BE2B379) {
            return 0;
        }
    }
    return 1;
}

static inline int PlayerSlotOf(u32 handle) {
    int i;
    int n;
    int* p;
    do {
        i = 0;
        p = gMinigame.handle;
        n = gMinigame.count;
        if (n > 0) {
            do {
                if ((u32)*p == handle) {
                    return i;
                }
                p++;
                i++;
                n--;
            } while (n != 0);
        }
    } while (handle >> 24 == 0x13 && (handle = fn_80153B9C(), handle >> 24 == 2) &&
             *(int*)((char*)PlayerGetPossessor(0) + 0x2740) == 0);
    return -1;
}

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

// Awards a "kill streak" step to the player `handle` when the match counters allow it.
void MinigameStreakUp(u32 handle) {
    if (handle >> 24 == 0x13) {
        if (PlayerSlotOf(handle) == -1) {
            int streak = GetVarInt(0x4B000000, 0x82A1DD29);
            if (streak < 3) {
                int enabled = GetVarInt(0x4B000000, 0xD199C84A);
                if (enabled != 0) {
                    int level = 0;
                    int r = Rand();
                    int q = r / 100 + (r >> 31);
                    if (r + (q - (q >> 31)) * -100 < gStatThreshold) {
                        int roll;
                        r = Rand();
                        q = r / 100 + (r >> 31);
                        roll = r + (q - (q >> 31)) * -100;
                        if (gStatHigh < roll) {
                            level = 9;
                        } else if (gStatMid < roll) {
                            level = 6;
                        } else {
                            level = 3;
                        }
                        SetVarInt(handle, 0x82A1DD29, level);
                        SetVarInt(0x4B000000, 0x82A1DD29, streak + 1);
                    }
                    StatSet(0x7BFB940D, 3, handle & 0xFFFFFF, (float)level);
                }
            }
        }
    }
}

// Takes back one streak step from the player `handle`.
void MinigameStreakDown(u32 handle) {
    if (handle >> 24 == 0x13) {
        if (PlayerSlotOf(handle) == -1) {
            int streak = GetVarInt(0x4B000000, 0x82A1DD29);
            if (streak != 0) {
                int enabled = GetVarInt(0x4B000000, 0xD199C84A);
                if (enabled != 0) {
                    int mine = GetVarInt(handle, 0x82A1DD29);
                    if (mine != 0) {
                        SetVarInt(0x4B000000, 0x82A1DD29, streak - 1);
                    }
                }
            }
        }
    }
}

// Name string for player slot `i`.
char* fn_800FD334(int i) {
    sprintf(gTeamNameBuf, "%s", gMinigame.name[i]);
    return gTeamNameBuf;
}

u32 MinigameFlagB(int i) {
    u32 v = gMinigame.flags130 & 4 << i;
    return (-v | v) >> 31;
}

u32 MinigameFlagA(int i) {
    u32 v = gMinigame.flags12C & 4 << i;
    return (-v | v) >> 31;
}

int fn_800FD3DC(void) {
    return gMinigame.unkC4;
}

int fn_800FD3EC(void) {
    return gMinigame.count;
}

char* fn_800FD3FC(int i) {
    if (i < 0 || i >= 8) {
        i = 8;
    }
    return gTeamColors[i];
}

char* fn_800FD424(int i) {
    if (i < 0 || i >= 8) {
        i = 8;
    }
    return gTeamColors2[i];
}

// English name of player colour `c`.
const char* fn_800FD44C(int c) {
    switch (c) {
    case 0:
        return "Blue";
    case 1:
        return "Red";
    case 2:
        return "Green";
    case 3:
        return "Yellow";
    case 4:
        return "Brown";
    case 5:
        return "Orange";
    case 6:
        return "Purple";
    case 7:
        return "Pink";
    default:
        return "INVALID";
    }
}

// Localised name of player colour `c` (the first four come from the string table).
const char* ColorNameLocal(int c) {
    const char* r;
    switch (c) {
    case 0:
        r = StrFindKeySkip(0x9AD441C8);
        break;
    case 1:
        r = StrFindKeySkip(0xBAAC3D43);
        break;
    case 2:
        r = StrFindKeySkip(0xBFFCEB35);
        break;
    case 3:
        r = StrFindKeySkip(0x5DA3CE62);
        break;
    case 4:
        r = "Brown";
        break;
    case 5:
        r = "Orange";
        break;
    case 6:
        r = "Purple";
        break;
    case 7:
        r = "Pink";
        break;
    default:
        r = "INVALID";
    }
    return r;
}

// Localised team name for colour `c`.
const char* fn_800FD5C4(int c) {
    const char* r;
    switch (c) {
    case 0:
        r = StrFindKeySkip(0x4F578E74);
        break;
    case 1:
        r = StrFindKeySkip(0xA02887D9);
        break;
    case 2:
        r = StrFindKeySkip(0x4030C127);
        break;
    case 3:
        r = StrFindKeySkip(0xE329D21A);
        break;
    case 4:
        r = "Brown Team";
        break;
    case 5:
        r = "Orange Team";
        break;
    case 6:
        r = "Purple Team";
        break;
    case 7:
        r = "Pink Team";
        break;
    default:
        r = "INVALID";
    }
    return r;
}

// Team of player slot `i` (-1 when out of range).
int PlayerTeam(int i) {
    if (i >= 0 && i < gMinigame.count) {
        return gMinigame.team[i];
    }
    return -1;
}

void MinigameSetTeam(int i, int team) {
    if (i < 0) {
        return;
    }
    if (gMinigame.count <= i) {
        return;
    }
    gMinigame.team[i] = team;
}

// Handle of player slot `i` (-1 when out of range).
int fn_800FD6EC(int i) {
    if (i >= 0 && i < gMinigame.count) {
        return gMinigame.handle[i];
    }
    return -1;
}

// Finds the slot of player `handle`; remaps a possessed host to its possessor.
int HandleToPlayer(int handle) {
    return PlayerSlotOf(handle);
}

// Removes the (non-local) player `handle` from the player list.
void MinigameRemovePlayer(int handle) {
    int i = 0;
    int slot = -1;
    int* p = gMinigame.handle;
    int n = gMinigame.count;
    if (n > 0) {
        do {
            slot = i;
            if (*p == handle) {
                break;
            }
            p++;
            i++;
            n--;
            slot = -1;
        } while (n != 0);
    }
    if (slot != -1 && ActorCount() <= slot) {
        gMinigame.count--;
        gMinigame.handle[slot] = -1;
    }
}

// Adds a bot player with handle `handle`; returns its slot (-1 when full).
int MinigameAddBot(int handle) {
    int slot;
    if (gMinigame.count < 8) {
        slot = gMinigame.bots + ActorCount();
        gMinigame.bots++;
        gMinigame.handle[slot] = handle;
        sprintf(gMinigame.name[slot], StrFindKeySkip(0x32405BBF), (slot - ActorCount()) + 1);
    } else {
        slot = -1;
    }
    return slot;
}

// Calls the current game type's update hook while the engine is running.
void fn_800FD924(int arg) {
    if (gMinigame.state == 1 && gMinigame.type > -1 && gMinigame.type < 6 &&
        gGameTypes[gMinigame.type] != NULL && gGameTypes[gMinigame.type]->draw != NULL) {
        gGameTypes[gMinigame.type]->draw(&gMinigame, arg);
    }
}

void fn_800FD998(void) {
    if (gMinigame.state == 1 && gMinigame.type > -1 && gMinigame.type < 6 &&
        gGameTypes[gMinigame.type] != NULL && gGameTypes[gMinigame.type]->update != NULL) {
        gGameTypes[gMinigame.type]->update();
    }
}

// Shuts the minigame engine down.
void fn_800FDA10(void) {
    fn_800872BC(0x40000, "Minigame : Terminating minigame engine");
    if (gMinigame.type > -1 && gMinigame.type < 6 && gGameTypes[gMinigame.type] != NULL &&
        gGameTypes[gMinigame.type]->term != NULL) {
        gGameTypes[gMinigame.type]->term();
    }
    fn_80128C8C();
    fn_80140984();
}

// Restarts the current game: rebuilds the player list and runs the type's reset / restart hooks.
void fn_800FDA90(void) {
    int n;
    int i;
    int* hp;
    char* np;
    gMinigame.state = 1;
    fn_801289D0(gGameTypes[gMinigame.type]);
    n = ActorCount();
    gMinigame.count = gMinigame.bots + n;
    hp = gMinigame.handle;
    np = gMinigame.name[0];
    for (i = 0; i < n; i++) {
        hp[i] = (i & 0xFFFFFF) | 0x2000000;
        SetVarInt(hp[i], 0x99E99508, i);
        sprintf(np, StrFindKeySkip(0x1B149A29), i + 1);
        np += 0x10;
    }
    HudRefreshSlots(&gMinigame);
    if (gMinigame.type > -1 && gMinigame.type < 6 && gGameTypes[gMinigame.type] != NULL &&
        gGameTypes[gMinigame.type]->reset != NULL) {
        gGameTypes[gMinigame.type]->reset();
    }
    fn_800872BC(0x40000, "Minigame : Reseting current game");
    if (gMinigame.type > -1 && gMinigame.type < 6 && gGameTypes[gMinigame.type] != NULL &&
        gGameTypes[gMinigame.type]->restart != NULL) {
        gGameTypes[gMinigame.type]->restart();
    }
}

void fn_800FDC48(void) {
    if (gMinigame.type == 4) {
        fn_8014C858();
    }
}

// Starts the minigame engine: registers the message handlers and picks the game type.
void fn_800FDC78(void) {
    fn_800872BC(0x40000, "Minigame : Minigame engine intializing");
    fn_80128D7C();
    fn_80026660(0x4B, (void*)fn_800FDDE4, 0);
    fn_80026660(0x52, (void*)MinigameMsgB, 0);
    fn_800094D8(0xB, gMinigameLevel);
    gMinigame.state = 2;
    gMinigame.bots = 0;
    gMinigame.count = 0;
    gMinigame.type = SysCheckMemory();
    fn_802A1D0C();
    if (gMinigame.type > -1 && gMinigame.type < 6 && gGameTypes[gMinigame.type] != NULL &&
        gGameTypes[gMinigame.type]->init != NULL) {
        gGameTypes[gMinigame.type]->init();
    }
    fn_802D1D7C(1);
    fn_8027D3E4(gMinigameZoom);
    if (gMinigame.type == 0) {
        EmitterSetMode(0);
    } else {
        EmitterSetMode(1);
    }
}

// Message handler (type 0x52): message 0x1002 with flag 1 tells the HUD to refresh.
int MinigameMsgB(u32 handle, int msg, int* data) {
    int reply[2];
    if (msg == 0x1002 && data[1] == 1) {
        reply[0] = 0;
        reply[1] = 0;
        SendMsg(0x38000000, 0x5600, reply);
    }
    return 0;
}

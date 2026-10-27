// u_800886C8.cpp: screen FX: screen records, ring and strip-wave effects, texture binding (0x800886C8-0x8008DCDC).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <math.h>
#include <dolphin/gx.h>

// Global stack of active entries: an index into the table below (negative when
// the stack is empty) and the table itself. Names are descriptive guesses.
extern int lbl_804D46F8;
extern float lbl_804D46FC;
extern void* lbl_80441598[];
extern void* lbl_804415B8[];

// Per-frame entry table (0x24-byte entries) and the two words reset alongside
// lbl_804482A0. Names are descriptive guesses.
extern u8 lbl_804416A0[];
extern int lbl_804482A0;
extern int lbl_804482A4;
extern int lbl_804482A8;

// Returns the entry selected by the stack index, or null if the stack is empty.
// fn_800886C8
extern "C" void* fn_800886C8(void)
{
    if (lbl_804D46F8 < 0) {
        return 0;
    }
    return lbl_80441598[lbl_804D46F8];
}

// Pops the most recent entry off the active-entry stack.
// fn_800886F0
extern "C" void fn_800886F0(void)
{
    lbl_804D46F8--;
}

// PushesActiveEntry: pushes a callback and its argument onto the active-entry
// stack. The stack holds at most 8 entries. Names are descriptive guesses.
// Kept under its fn_ placeholder name because other units call it.
extern "C" void fn_80088700(void* callback, void* arg)
{
    int i = ++lbl_804D46F8;
    if (i > 8) {
        lbl_804D46F8 = i - 1;
        return;
    }
    lbl_80441598[i] = callback;
    lbl_804415B8[i] = arg;
}

// Runs the callback on top of the active-entry stack, advancing its clock by
// dt first. Returns 0 when the stack is empty. Names are descriptive guesses.
int RunActiveStackEntry(float dt)
{
    int (*callback)(void*) = (int (*)(void*))lbl_80441598[lbl_804D46F8];
    if (callback == 0) {
        return 0;
    }
    lbl_804D46FC += dt;
    return callback(lbl_804415B8[lbl_804D46F8]);
}

// Configuration block that follows the two stacks above in .bss; the setters
// below fill it in. Field names are descriptive guesses.
struct ActiveEntryConfig {
    unsigned char unk00[0x04];        // 0x00
    void* ptr04;                      // 0x04
    void* ptr08;                      // 0x08
    void* ptr0C;                      // 0x0c
    void* ptr10;                      // 0x10
    void* ptr14;                      // 0x14
    void* ptr18;                      // 0x18
    unsigned char unk1C[0x28 - 0x1c]; // 0x1c
    short mode;                       // 0x28
};
extern ActiveEntryConfig lbl_804415D8;

// Resets the active-entry configuration mode to -1 ("none"). Names are
// descriptive guesses. Kept under its fn_ placeholder name because another
// unit calls it.
extern "C" void fn_800887A0(void)
{
    lbl_804415D8.mode = -1;
}

// Fills in the active-entry configuration block from the caller's arguments.
// Names are descriptive guesses. Kept under its fn_ placeholder name because
// another unit calls it.
extern "C" void fn_800887B4(short mode, void* p04, void* p08, void* p0C,
                            void* p10, void* p14, void* p18)
{
    lbl_804415D8.mode = mode;
    lbl_804415D8.ptr04 = p04;
    lbl_804415D8.ptr08 = p08;
    lbl_804415D8.ptr0C = p0C;
    lbl_804415D8.ptr10 = p10;
    lbl_804415D8.ptr14 = p14;
    lbl_804415D8.ptr18 = p18;
}

// Handler that receives an output buffer, the caller's key and a context
// object, and returns non-zero when it produced an entry.
typedef int (*ScrollHandler)(void* out, void* key, void* ctx);

// One entry of the scroll-name table: the four-character name and the handler
// that resolves it. Field names and layout are descriptive guesses (stride
// 0x10, 26 entries).
struct ScrollNameEntry {
    const char* name;        // 0x00
    unsigned int unk04;      // 0x04
    unsigned int unk08;      // 0x08
    ScrollHandler handler;   // 0x0c
};
extern ScrollNameEntry lbl_80398468[26];

extern "C" int fn_80049940(void);

// Looks a four-character scroll name up in the scroll-name table and returns
// its index (0..25), or -1 when the lookup is disabled or the name is unknown.
// Names are descriptive guesses.
int FxLookupScrollType(const unsigned char* name)
{
    union {
        unsigned char bytes[4];
        int word;
    } key;
    int i;

    if (fn_80049940() != 0) {
        return -1;
    }
    key.bytes[0] = name[0];
    key.bytes[1] = name[1];
    key.bytes[2] = name[2];
    key.bytes[3] = name[3];
    for (i = 0; i < 26; i++) {
        if (key.word == *(const int*)lbl_80398468[i].name) {
            return i;
        }
    }
    return -1;
}

// Stores the scroll-type index used by the rest of the scroll code. Names are
// descriptive guesses.
extern unsigned int lbl_804D4710;

void FxSetScrollType(unsigned int type)
{
    lbl_804D4710 = type;
}

// Proximity-fade constants (float pool) and the current fade strengths.
// Names are descriptive guesses.
extern float lbl_804D6898;
extern float lbl_804D689C;
extern float lbl_804D68A0;
extern float lbl_804D68A4;
extern float lbl_804D68A8;
extern float lbl_804D68AC;
extern float lbl_804D2A18;
extern float lbl_804D2A1C;
extern float lbl_804D2A20;

// Divides the caller's value by the stored proximity constant and keeps the
// result as the current proximity scale. Names are descriptive guesses.
void FxSetProximityScale(float value)
{
    lbl_804D2A20 = value / lbl_804D6898;
}

// Position the fade is measured from, kept in .bss right next to the constants
// above. Names are descriptive guesses.
extern float lbl_80441620[];

struct FadeSource {
    unsigned char unk00[0x24]; // 0x00
    float x;                   // 0x24
    float y;                   // 0x28
    float z;                   // 0x2c
};

extern "C" FadeSource* fn_800C6448(void);
extern "C" float fn_800E1020(float* v);

// Stores the caller's position, measures its distance from the current source
// point and turns that distance into a fade strength: full inside the near
// radius, linear fall-off up to the far radius, zero beyond it. Names are
// descriptive guesses.
void FxUpdateProximityFade(float* pos)
{
    float local[3];
    FadeSource* source;
    float dist;

    lbl_80441620[0] = pos[0];
    lbl_80441620[1] = pos[1];
    lbl_80441620[2] = pos[2];
    source = fn_800C6448();
    local[0] = pos[0] - source->x;
    local[1] = pos[1] - source->y;
    local[2] = pos[2] - source->z;
    dist = fn_800E1020(local);
    if (dist < lbl_804D689C) {
        lbl_804D2A18 = lbl_804D68A0;
    } else if (dist > lbl_804D68A4) {
        lbl_804D2A18 = lbl_804D68A8;
    } else {
        lbl_804D2A18 = (lbl_804D68AC - (dist - lbl_804D689C)) / lbl_804D68AC;
    }
    lbl_804D2A1C = lbl_804D2A18;
}

// Copies two 3-float points into the scroll point globals that the proximity
// fade code reads. Names are descriptive guesses.
extern float lbl_80441608[3];
extern float lbl_80441614[3];

void FxSetScrollPoints(float* from, float* to)
{
    lbl_80441608[0] = from[0];
    lbl_80441608[1] = from[1];
    lbl_80441608[2] = from[2];
    lbl_80441614[0] = to[0];
    lbl_80441614[1] = to[1];
    lbl_80441614[2] = to[2];
}

// A scroll source addressed through the low 12 bits of a key; only its type
// index and its "usable" flag are read here. Layout is a descriptive guess.
struct ScrollSource {
    unsigned int unk00;               // 0x00
    unsigned int unk04;               // 0x04
    unsigned int unk08;               // 0x08
    unsigned int unk0C;               // 0x0c
    unsigned int unk10;               // 0x10
    unsigned int unk14;               // 0x14
    unsigned int unk18;               // 0x18
    unsigned char unk1C[0x28 - 0x1c]; // 0x1c
    short type;                       // 0x28
    short next;                       // 0x2a
    short prev;                       // 0x2c
    short flags;                      // 0x2e
};

extern ScrollSource** lbl_804D4700;
extern int lbl_804D4704;
extern unsigned int lbl_80441670;

// Resolves the scroll entry named by the low 12 bits of the caller's key.
// Returns the key unchanged when no entry applies, the filled-in output buffer
// when a handler accepts it, and null when a handler rejects it. Names are
// descriptive guesses.
unsigned int* FxResolveScrollEntry(unsigned int* key)
{
    ScrollSource* source;
    ScrollHandler handler;
    int type;
    void* ctx;

    if (fn_80049940() != 0) {
        return key;
    }

    type = lbl_804415D8.mode;
    if (type == -1) {
        int index = *key & 0xfff;
        if (index < 0 || index >= lbl_804D4704) {
            return key;
        }
        source = lbl_804D4700[index];
        if (source == 0) {
            return key;
        }
        if ((source->flags & 2) == 0) {
            return key;
        }
        type = source->type;
        ctx = source;
    } else {
        ctx = &lbl_804415D8;
    }

    handler = lbl_80398468[type].handler;
    if (handler == 0) {
        return key;
    }
    if (handler(&lbl_80441670, key, ctx) != 0) {
        return &lbl_80441670;
    }
    return 0;
}

// Entry of the scroll-dispatch table as the dispatch path below sees it: the
// four-character name, two words of data, and the handler pointer at 0x08
// (retail loads it through lbl_80398470, i.e. lbl_80398468 + 8).
struct ScrollDispatchEntry {
    unsigned int name;      // 0x00
    unsigned int unk04;     // 0x04
    ScrollHandler handler;  // 0x08
    unsigned int unk0C;     // 0x0c
};

// Scroll entry output buffer filled in by the handlers (0x8044162C).
extern unsigned int lbl_8044162C;

// Resolves the scroll entry named by the low 12 bits of the caller's key and
// hands it to that entry's handler with the output buffer. Returns the key
// unchanged when no entry applies, the filled-in output buffer when a handler
// accepts it, and null when a handler rejects it. Names are descriptive guesses.
unsigned int* FxDispatchScrollSource(unsigned int* key)
{
    int index;
    ScrollSource* source;
    ScrollHandler handler;

    if (fn_80049940() != 0) {
        return key;
    }
    index = *key & 0xfff;
    if (index < 0 || index >= lbl_804D4704) {
        return key;
    }
    source = lbl_804D4700[index];
    if (source == 0) {
        return key;
    }
    if ((source->flags & 2) == 0) {
        return key;
    }
    handler = ((ScrollDispatchEntry*)lbl_80398468)[source->type].handler;
    if (handler == 0) {
        return key;
    }
    if (handler(&lbl_8044162C, key, source) != 0) {
        return &lbl_8044162C;
    }
    return 0;
}

// Stores the six configuration words into the scroll source selected by the
// low 12 bits of the caller's key. Names are descriptive guesses.
// fn_80088BA0
extern "C" void fn_80088BA0(unsigned int key, unsigned int v04, unsigned int v08,
                             unsigned int v0C, unsigned int v10, unsigned int v14,
                             unsigned int v18)
{
    int index;

    if (lbl_804D4704 == 0) {
        return;
    }
    index = key & 0xfff;
    if (index >= lbl_804D4704) {
        return;
    }
    if (lbl_804D4700[index] == 0) {
        return;
    }
    lbl_804D4700[index]->unk04 = v04;
    lbl_804D4700[index]->unk08 = v08;
    lbl_804D4700[index]->unk0C = v0C;
    lbl_804D4700[index]->unk10 = v10;
    lbl_804D4700[index]->unk14 = v14;
    lbl_804D4700[index]->unk18 = v18;
}

// Scroll-source list head: the entry index most recently linked in, or -1 when
// the list is empty. Names are descriptive guesses.
extern int lbl_804D4708;

// Allocator for the 0x30-byte per-entry scroll-source blocks.
void* VAlloc(int size);
// Shared debug output; retail calls it with the "Unknown shader '%s'" message.
void DebugPrintf(const char* fmt, ...);
extern "C" unsigned int fn_800102F8(unsigned int key);
extern "C" void fn_800102C8(unsigned int key, unsigned int flags);

// Fetches the scroll source selected by the low 12 bits of the caller's key, or
// null when scrolling is disabled or the key is out of range. Names are
// descriptive guesses.
static inline ScrollSource* FxGetScrollSource(unsigned int key)
{
    int index = key & 0xfff;

    if (fn_80049940() != 0) {
        return 0;
    }
    if (lbl_804D4704 == 0) {
        return 0;
    }
    if (index >= lbl_804D4704) {
        return 0;
    }
    return lbl_804D4700[index];
}

// Registers a scroll source: allocates its block if needed, unlinks it from the
// active list, clears its "usable" flag, and stores the shader type plus the six
// caller values. Unknown shader names are reported and nothing is stored. Names
// are descriptive guesses. Kept under its fn_ placeholder name because other
// units call it.
extern "C" void fn_80088C10(unsigned int key, const char* name, unsigned int v04,
                            unsigned int v08, unsigned int v0C, unsigned int v10,
                            unsigned int v14, unsigned int v18)
{
    ScrollSource* source;
    int index;
    int type;

    if (fn_80049940() != 0) {
        return;
    }
    if (lbl_804D4704 == 0) {
        return;
    }
    index = key & 0xfff;
    if (index >= lbl_804D4704) {
        return;
    }

    if (lbl_804D4700[index] == 0) {
        lbl_804D4700[index] = (ScrollSource*)VAlloc(0x30);
    }

    source = FxGetScrollSource(key);
    if (source != 0 && (source->flags & 1) != 0) {
        if (source->prev == -1) {
            lbl_804D4708 = source->next;
        } else {
            lbl_804D4700[source->prev]->next = source->next;
        }
        if (source->next != -1) {
            lbl_804D4700[source->next]->prev = source->prev;
        }
        source->flags &= ~1;
    }

    source = FxGetScrollSource(key);
    if (source != 0) {
        unsigned int flags;

        source->flags &= ~2;
        flags = fn_800102F8(key & 0xfff);
        fn_800102C8(key & 0xfff, flags & 0xfffbffffu);
    }

    // Look the four-character shader name up in the scroll-name table.
    type = 0;
    {
        int i;

        for (i = 0; i < 13; i++) {
            if (*(const unsigned int*)name == *(const unsigned int*)lbl_80398468[type].name) {
                break;
            }
            type++;
            if (*(const unsigned int*)name == *(const unsigned int*)lbl_80398468[type].name) {
                break;
            }
            type++;
        }
    }

    if (type < 26) {
        source = lbl_804D4700[index];
        source->type = (short)type;
        source->unk04 = v04;
        source->unk08 = v08;
        source->unk0C = v0C;
        source->unk10 = v10;
        source->unk14 = v14;
        source->unk18 = v18;
    } else {
        DebugPrintf("Unknown shader '%s'", name);
    }
}

// Handler invoked when a scroll source's value changes: (entry, argument).
typedef void (*ScrollEntryHandler)(ScrollSource*, int);

// Layout of a scroll-name table entry as the value-update path sees it: the
// four-character name at 0x00 and the handler at 0x04 (retail loads the handler
// through lbl_80398468 + type * 0x10 + 4). Names are descriptive guesses.
struct ScrollNameHandlerEntry {
    const char* name;            // 0x00
    ScrollEntryHandler handler;  // 0x04
    unsigned int unk08;          // 0x08
    unsigned int unk0C;          // 0x0c
};

// Stores the caller's value in the scroll source selected by the low 12 bits of
// the key, then notifies it through its handler with a zero argument. Names are
// descriptive guesses. Kept under its fn_ placeholder name because other units
// call it.
extern "C" void fn_80088E68(unsigned int key, unsigned int value)
{
    ScrollNameHandlerEntry* entries;
    ScrollSource* source;
    ScrollEntryHandler handler;
    int index;

    if (fn_80049940() != 0) {
        return;
    }
    if (lbl_804D4704 == 0) {
        return;
    }
    index = key & 0xfff;
    if (index >= lbl_804D4704) {
        return;
    }
    source = lbl_804D4700[index];
    if (source == 0) {
        return;
    }
    source->unk00 = value;
    entries = (ScrollNameHandlerEntry*)lbl_80398468;
    handler = entries[source->type].handler;
    if (handler != 0) {
        handler(source, 0);
    }
}

// FxSetScrollSourceUsable: sets or clears the "usable" flag of the scroll
// source selected by the low 12 bits of the caller's key, and updates the
// matching engine object flag. Names are descriptive guesses. Kept under its
// fn_ placeholder name because other units call it; the symbol stays in
// symbols.txt unchanged.
extern "C" void fn_80088F0C(unsigned int key, int usable)
{
    ScrollSource* source;
    unsigned int flags;
    unsigned int index;

    if (lbl_804D4704 == 0) {
        return;
    }
    index = key & 0xfff;
    if ((int)index >= lbl_804D4704) {
        return;
    }
    source = lbl_804D4700[index];
    if (source == 0) {
        return;
    }
    if (usable != 0) {
        source->flags |= 2;
        flags = fn_800102F8(index);
        fn_800102C8(index, flags | 0x40000);
    } else {
        source->flags &= ~2;
        flags = fn_800102F8(index);
        fn_800102C8(index, flags & 0xfffbffffu);
    }
}

// FxSetScrollSourceActive: links the scroll source selected by the low 12 bits
// of the caller's key into the active list, or unlinks it, depending on the
// caller's flag. Nothing happens while the scroll system is disabled or when
// the source has no handler. Names are descriptive guesses. Kept under its
// fn_ placeholder name because other units call it; the symbol stays in
// symbols.txt unchanged.
extern "C" void fn_80088FB0(unsigned int key, int active)
{
    ScrollSource* source;
    int index;

    if (fn_80049940() != 0) {
        return;
    }
    if (lbl_804D4704 == 0) {
        return;
    }
    index = key & 0xfff;
    if (index >= lbl_804D4704) {
        return;
    }
    source = lbl_804D4700[index];
    if (source == 0) {
        return;
    }
    if (active != 0) {
        if ((source->flags & 1) != 0) {
            return;
        }
        if (((ScrollNameHandlerEntry*)lbl_80398468)[source->type].handler == 0) {
            return;
        }
        source->next = (short)lbl_804D4708;
        lbl_804D4700[index]->prev = -1;
        if (lbl_804D4708 != -1) {
            lbl_804D4700[lbl_804D4708]->prev = (short)index;
        }
        lbl_804D4708 = index;
        lbl_804D4700[index]->flags |= 1;
    } else {
        if ((source->flags & 1) == 0) {
            return;
        }
        if (source->prev == -1) {
            lbl_804D4708 = source->next;
        } else {
            lbl_804D4700[source->prev]->next = source->next;
        }
        if (lbl_804D4700[index]->next != -1) {
            lbl_804D4700[lbl_804D4700[index]->next]->prev = lbl_804D4700[index]->prev;
        }
        lbl_804D4700[index]->flags &= ~1;
    }
}

// Accumulated scroll clock in milliseconds. Names are descriptive guesses.
extern int lbl_804D470C;

// Advances the scroll clock by the caller's elapsed time and runs the value
// handler of every active scroll source with that elapsed time in whole
// milliseconds. Names are descriptive guesses. Kept under its fn_ placeholder
// name because another unit calls it.
extern "C" void fn_8008910C(double dt)
{
    int elapsed = (int)dt;
    int index;

    if (fn_80049940() != 0) {
        return;
    }
    lbl_804D470C += elapsed;
    index = lbl_804D4708;
    if (lbl_804D4704 != 0) {
        for (; index >= 0; index = lbl_804D4700[index]->next) {
            ScrollSource* source = lbl_804D4700[index];
            ((ScrollNameHandlerEntry*)lbl_80398468)[source->type].handler(source, elapsed);
        }
    }
}

// Allocates the scroll-source table with room for the caller's entry count and
// starts with an empty active list. Does nothing when the table already exists.
// Names are descriptive guesses. Kept under its fn_ placeholder name because
// another unit calls it.
extern "C" void fn_800891C8(int count)
{
    if (lbl_804D4704 == 0) {
        lbl_804D4704 = count;
        if (count != 0) {
            lbl_804D4700 = (ScrollSource**)VAlloc(count << 2);
            lbl_804D4708 = -1;
        }
    }
}

// Frees the scroll-source table: every live entry is released, then the table
// itself, and the table pointer is cleared. Names are descriptive guesses.
void VFree(void* ptr);

void FxFreeScrollSources(void)
{
    int i;

    if (lbl_804D4700 != 0) {        for (i = 0; i < lbl_804D4704; i++) {
            if (lbl_804D4700[i] != 0) {
                VFree(lbl_804D4700[i]);
            }
        }
        VFree(lbl_804D4700);
        lbl_804D4700 = 0;
    }
}

// Resets the scroll system: clears the mode of the active-entry configuration,
// empties the entry table and the active list. Names are descriptive guesses.
void FxResetScrollSources(void)
{
    lbl_804415D8.mode = -1;
    lbl_804D4704 = 0;
    lbl_804D4708 = -1;
}

// One row of the scroll record below: fifteen words holding an integer flag,
// two copied floats, another flag, the item position, a scale, three more
// copied floats, a direction vector and a trailing integer. Names and layout
// are descriptive guesses.
struct ScrollItemRow {
    int unk00;      // 0x00
    float src1;     // 0x04
    float src2;     // 0x08
    int unk0C;      // 0x0c
    float x;        // 0x10
    float y;        // 0x14
    float z;        // 0x18
    float scale;    // 0x1c
    float src8;     // 0x20
    float src9;     // 0x24
    float srcA;     // 0x28
    float dirX;     // 0x2c
    float dirY;     // 0x30
    float dirZ;     // 0x34
    int unk38;      // 0x38
};

// Per-item scroll record (0x200 bytes) that the transform builder copies and
// fills in: a mode word, four flag bytes, the current row index, two words, the
// sine/cosine pair that drives the item, a fade value and eight rows. Names and
// layout are descriptive guesses.
struct ScrollItemAnim {
    int mode;               // 0x00
    unsigned char unk04;    // 0x04
    unsigned char unk05;    // 0x05
    unsigned char unk06;    // 0x06
    unsigned char unk07;    // 0x07
    int index;              // 0x08
    int unk0C;              // 0x0c
    int unk10;              // 0x10
    float sinAngle;         // 0x14
    float cosAngle;         // 0x18
    float fade;             // 0x1c
    ScrollItemRow rows[8];  // 0x20
};

extern "C" int fn_802A1D04(void);
extern "C" float fn_800A1394(float x);
extern "C" float fn_800A1418(float x);
extern "C" void fn_80112D84(void* record, int mode, void* out, float scale);
extern void GfxSetMaterial(void* buf);
extern float lbl_804D68B0;
extern float lbl_804D68B4;
extern float lbl_804D68B8;
extern float lbl_804D68BC;

// Builds the transform of one scroll item for the current scroll clock and
// combines it with the caller's 3x4 matrix. Returns 0 when the scroll system is
// disabled. Names are descriptive guesses.
int FxBuildScrollItemTransform(int* out, int* in, int* item)
{
    ScrollItemAnim anim;
    ScrollItemRow* row;
    float* dir;
    unsigned char buffer[0x300];
    float len;
    float inv;
    int count;
    int i;

    if (fn_802A1D04() != 0) {
        return 0;
    }
    for (i = 0; i < 12; i++) {
        out[i] = in[i];
    }
    if (lbl_804D4710 == 0) {
        return 1;
    }

    anim = *(ScrollItemAnim*)lbl_804D4710;
    anim.rows[anim.index - 1] = anim.rows[0];

    anim.mode = 1;
    anim.unk04 = 0x80;
    anim.unk05 = 0x80;
    anim.unk06 = 0x80;
    anim.unk07 = (unsigned char)(int)(lbl_804D68B0 * lbl_804D2A20);
    anim.unk0C = 1;
    anim.sinAngle = fn_800A1394((float)(lbl_804D470C * item[4]) / lbl_804D68B4);
    anim.cosAngle = fn_800A1418((float)(lbl_804D470C * item[4]) / lbl_804D68B4);
    anim.fade = lbl_804D68A8;
    anim.unk10 = -1;
    anim.index = 1;

    row = &anim.rows[0];
    row->unk00 = 0;
    row->unk0C = 1;
    row->x = (float)item[1];
    row->y = (float)item[2];
    row->z = (float)item[3];
    row->scale = lbl_804D68B8 * lbl_804D2A20;

    dir = &row->dirX;
    dir[0] = fn_800A1394((float)(lbl_804D470C * item[4]) / lbl_804D68B4);
    dir[1] = fn_800A1418((float)(lbl_804D470C * item[4]) / lbl_804D68B4);
    dir[2] = fn_800A1394((float)(lbl_804D470C * item[5]) / lbl_804D68B4);

    count = 0;
    if (fabsf(dir[2]) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dir[0]) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dir[1]) > lbl_804D68BC) {
        count++;
    }
    if (count == 0) {
        dir[0] = lbl_804D68A8;
        dir[1] = lbl_804D68A8;
        dir[2] = lbl_804D68A8;
    } else {
        len = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
        inv = 1.0f / sqrtf(len);
        dir[0] *= inv;
        dir[1] *= inv;
        dir[2] *= inv;
    }

    row->unk38 = -0x20;
    fn_80112D84(&anim, 0x107, buffer, lbl_804D68A0);
    GfxSetMaterial(buffer);
    return 1;
}

// Advances one scroll item's phase by the caller's delta and recomputes its
// sine and cosine offsets (0x1c and 0x20). Names are descriptive guesses.
void FxAdvanceScrollItem(int* item, int delta)
{
    item[0] = item[0] + delta;
    item[7] = (int)(lbl_804D68B4 * fn_800A1394((float)(item[0] * item[4]) / lbl_804D68B4));
    item[8] = (int)(lbl_804D68B4 * fn_800A1418((float)(item[0] * item[4]) / lbl_804D68B4));
}

// Copies a 12-word state block and hands a randomly scaled value (0..1 times
// lbl_804D68CC) to fn_80105188. Names are descriptive guesses.
extern double lbl_804D68C0;
extern float lbl_804D68C8;
extern float lbl_804D68CC;
extern "C" void fn_80105188(float value);
extern "C" unsigned int rand(void);

int CopyState12Randomize(unsigned int* dst, unsigned int* src)
{
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

    unsigned int r = rand();
    fn_80105188(lbl_804D68CC * ((float)r / lbl_804D68C8));
    return 1;
}

// Copies a 17-word state block and hands a randomly scaled value (0..1 times
// lbl_804D68CC) to fn_80105188. Names are descriptive guesses.
int CopyState17Randomize(unsigned int* dst, unsigned int* src)
{
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

    unsigned int r = rand();
    fn_80105188(lbl_804D68CC * ((float)r / lbl_804D68C8));
    return 1;
}

// Copies a 12-word state block and hands the current clock scaled by the
// caller's rate field (scale[1]) to fn_80105188. Names are descriptive guesses.
int CopyState12Scaled(unsigned int* dst, unsigned int* src, int* scale)
{
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

    fn_80105188(((float)lbl_804D470C / lbl_804D68B4) * (float)scale[1]);
    return 1;
}

// Copies a 17-word state block and hands the current clock scaled by the
// caller's rate field (scale[1]) to fn_80105188. Names are descriptive guesses.
int CopyState17Scaled(unsigned int* dst, unsigned int* src, int* scale)
{
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

    fn_80105188(((float)lbl_804D470C / lbl_804D68B4) * (float)scale[1]);
    return 1;
}

// Three-float vector as ViewPlaneFromNormal sees it.
struct Vec3 {
    float x; // 0x00
    float y; // 0x04
    float z; // 0x08
};

// Builds the plane (normal and distance) a scroll item is projected onto.
void ViewPlaneFromNormal(float* plane, const Vec3* normal, const Vec3* point);

// Blend and shrink factors applied to every scroll item.
extern float lbl_804D68D0;
extern float lbl_804D68D4;

// Builds the scroll-item animation record for the current scroll clock: with no
// active scroll table it is a single default item, otherwise it is a copy of the
// active table with every item blended toward the screen plane. The record is
// then handed to the material builder. Returns 1. Names are descriptive guesses.
int FxBuildScreenScrollItems(int* out, int* in, int* item)
{
    ScrollItemAnim anim;
    ScrollItemAnim* table = (ScrollItemAnim*)lbl_804D4710;
    ScrollItemRow* row;
    float plane[4];
    float px;
    float py;
    float pz;
    float d0;
    float d1;
    float t;
    unsigned char buffer[0x284];
    int i;

    for (i = 0; i < 12; i++) {
        out[i] = in[i];
    }

    if (table == 0 || ((int*)table)[0xb] != 1) {
        anim.mode = 1;
        anim.unk04 = 0;
        anim.unk05 = 0;
        anim.unk06 = 0;
        anim.unk07 = 0;
        anim.index = 1;
        anim.unk0C = 0;
        row = &anim.rows[0];
        row->unk00 = 0;
        row->unk0C = 1;
        row->x = (float)item[1];
        row->y = (float)item[2];
        row->z = (float)item[3];
        row->scale = lbl_804D68A0;
        row->dirX = fn_800A1394((float)(lbl_804D470C * item[4]) / lbl_804D68B4);
        row->dirY = fn_800A1418((float)(lbl_804D470C * item[4]) / lbl_804D68B4);
        row->dirZ = lbl_804D68A8;
        row->unk38 = -0x20;
    } else {
        anim = *table;
        for (i = 0; i < anim.index; i++) {
            float lerpX;
            float lerpY;
            float lerpZ;

            row = &anim.rows[i];
            if (row->unk00 == 2) {
                ViewPlaneFromNormal(plane, (const Vec3*)&row->dirX,
                                    (const Vec3*)lbl_80441620);
                t = row->src1;
                px = row->src8 + row->dirX * t;
                py = row->src9 + row->dirY * t;
                pz = row->srcA + row->dirZ * t;
                d0 = plane[2] * row->srcA + plane[0] * row->src8 + plane[1] * row->src9;
                d1 = plane[2] * pz + plane[0] * px + plane[1] * py;
                if (d0 != d1) {
                    t = (-plane[3] - d0) / (d1 - d0);
                } else {
                    t = lbl_804D68A8;
                }
                lerpX = row->src8 + (px - row->src8) * t;
                lerpY = row->src9 + (py - row->src9) * t;
                lerpZ = row->srcA + (pz - row->srcA) * t;
                row->src8 += (lerpX - row->src8) * lbl_804D68D0;
                row->src9 += (lerpY - row->src9) * lbl_804D68D0;
                row->srcA += (lerpZ - row->srcA) * lbl_804D68D0;
            }
            row->dirX *= lbl_804D68D4;
            row->dirY *= lbl_804D68D4;
            row->dirZ *= lbl_804D68D4;
            row->x = (float)item[1];
            row->y = (float)item[2];
            row->z = (float)item[3];
            row->scale = lbl_804D68A0;
            row->unk38 = -0x20;
        }
    }

    fn_80112D84(&anim, 0x105, buffer, lbl_804D68A0);
    GfxSetMaterial(buffer);
    return 1;
}

// Advances one screen scroll item's phase by the caller's delta and recomputes
// its sine and cosine offsets (0x1c and 0x20). Names are descriptive guesses.
void FxAdvanceScreenScrollItem(int* item, int delta)
{
    item[0] = item[0] + delta;
    item[7] = (int)(lbl_804D68B4 * fn_800A1394((float)(item[0] * item[4]) / lbl_804D68B4));
    item[8] = (int)(lbl_804D68B4 * fn_800A1418((float)(item[0] * item[4]) / lbl_804D68B4));
}

// Wave amplitude and sine helper used by the screen-scroll items below.
extern float lbl_804D68D8;
float fn_80017084(float x);

// Builds a screen-scroll item record whose direction starts as the difference
// of the two view-plane points, wobbles with the current scroll clock and is
// renormalised, then hands it to the material builder. Returns 1. Names are
// descriptive guesses.
int FxBuildScreenScrollItemWave(int* out, int* in, int* item)
{
    ScrollItemAnim anim;
    ScrollItemRow* row;
    float* dir;
    unsigned char buffer[0x280];
    float len;
    float inv;
    int count;
    int i;

    for (i = 0; i < 12; i++) {
        out[i] = in[i];
    }

    anim.mode = 1;
    anim.unk04 = 0;
    anim.unk05 = 0;
    anim.unk06 = 0;
    anim.unk07 = 0;
    anim.index = 1;
    anim.unk0C = 0;

    row = &anim.rows[0];
    row->unk00 = 0;
    row->unk0C = 1;
    row->x = (float)item[1];
    row->y = (float)item[2];
    row->z = (float)item[3];
    row->scale = lbl_804D68A0;

    dir = &row->dirX;
    dir[0] = lbl_80441620[0] - lbl_80441614[0];
    dir[1] = lbl_80441620[1] - lbl_80441614[1];
    dir[2] = lbl_80441620[2] - lbl_80441614[2];

    count = 0;
    if (fabsf(dir[2]) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dir[0]) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dir[1]) > lbl_804D68BC) {
        count++;
    }
    if (count == 0) {
        dir[0] = lbl_804D68A8;
        dir[1] = lbl_804D68A8;
        dir[2] = lbl_804D68A8;
    } else {
        len = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
        inv = 1.0f / sqrtf(len);
        dir[0] *= inv;
        dir[1] *= inv;
        dir[2] *= inv;
    }

    dir[0] = lbl_804D68D8 *
                 fn_80017084((float)(lbl_804D470C * item[4]) / lbl_804D68B4) + dir[0];
    dir[1] = lbl_804D68D8 *
                 fn_80017084((float)(lbl_804D470C * item[5]) / lbl_804D68B4) + dir[1];
    dir[2] = lbl_804D68D8 *
                 fn_80017084((float)(lbl_804D470C * item[6]) / lbl_804D68B4) + dir[2];

    count = 0;
    if (fabsf(dir[2]) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dir[0]) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dir[1]) > lbl_804D68BC) {
        count++;
    }
    if (count == 0) {
        dir[0] = lbl_804D68A8;
        dir[1] = lbl_804D68A8;
        dir[2] = lbl_804D68A8;
    } else {
        len = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
        inv = 1.0f / sqrtf(len);
        dir[0] *= inv;
        dir[1] *= inv;
        dir[2] *= inv;
    }

    row->unk38 = -0x20;
    fn_80112D84(&anim, 0x101, buffer, lbl_804D68A0);
    GfxSetMaterial(buffer);
    return 1;
}

// Advances one screen-scroll item's phase by the caller's delta and recomputes
// its three sine offsets (0x1c, 0x20 and 0x24). Names are descriptive guesses.
void FxAdvanceScreenScrollItemWave(int* item, int delta)
{
    item[0] = item[0] + delta;
    item[7] = (int)(lbl_804D68B4 * fn_80017084((float)(item[0] * item[4]) / lbl_804D68B4));
    item[8] = (int)(lbl_804D68B4 * fn_80017084((float)(item[0] * item[5]) / lbl_804D68B4));
    item[9] = (int)(lbl_804D68B4 * fn_80017084((float)(item[0] * item[6]) / lbl_804D68B4));
}

void GfxSetCullMode(int mode);
extern "C" void fn_80104AAC(int arg, unsigned int red, unsigned int green, unsigned int blue);

// Shared graphics-state setup: copies a 12-word state block into the
// destination, turns culling off and programs the modulate colour stored at
// 0x04/0x08/0x0c of the configuration record. Names are descriptive guesses.
int GfxApplyStateConfig12(int* dst, int* src, void* config)
{
    unsigned int* colour = (unsigned int*)config;

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

    GfxSetCullMode(0);
    fn_80104AAC(lbl_804D470C, colour[1] & 0xff, colour[2] & 0xff, colour[3] & 0xff);
    return 1;
}

// Same as GfxApplyStateConfig12, but for a 17-word state block (the larger
// 0x44-byte variant of the state record). Names are descriptive guesses.
int GfxApplyStateConfig17(int* dst, int* src, void* config)
{
    unsigned int* colour = (unsigned int*)config;

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

    GfxSetCullMode(0);
    fn_80104AAC(lbl_804D470C, colour[1] & 0xff, colour[2] & 0xff, colour[3] & 0xff);
    return 1;
}

extern "C" void fn_80105260(int arg);

// Copies a 12-word graphics-state block into the destination and re-programs
// the shared display configuration. Names are descriptive guesses.
int GfxApplyStateBlock12(int* dst, int* src)
{
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

    fn_80105260(lbl_804D470C);
    return 1;
}

// Same as GfxApplyStateBlock12, but copies the 17-word (0x44-byte) variant of
// the state record. Names are descriptive guesses.
int GfxApplyStateBlock17(int* dst, int* src)
{
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

    fn_80105260(lbl_804D470C);
    return 1;
}

// Copies a 12-word graphics-state block into the caller's destination, then
// binds the texture object held in the item's word 2 and re-programs the shared
// display: a render-target texture (type 2) has its previous contents released
// first. Names are descriptive guesses.
extern "C" int fn_80010448(unsigned int texture, int arg);
extern "C" void fn_80010330(unsigned int texture, int arg);

int GfxApplyTextureStateBlock12(int* dst, int* src, int* item)
{
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

    dst[0] = item[2];
    if (fn_80010448(item[2], 0) == 2) {
        fn_80010330(item[2], 0);
    }
    fn_80105260(lbl_804D470C);
    return 1;
}

// Binds the texture object held in the item's word 4 (loading the texture-unit
// state that goes with it) and then builds the screen-scroll item record for the
// current scroll clock. With no active scroll table it is a single default item
// whose direction is the difference of the two scroll points; otherwise it is a
// copy of the active table with every item blended toward the screen plane. The
// record is handed to the material builder and culling is turned off. Returns 1.
// Names are descriptive guesses.
extern "C" void* fn_8000F990(int texture);
extern "C" void fn_8010576C(int clock);
extern "C" void fn_80105D84(int clock);

int FxBuildScreenScrollItemsTex(int* out, int* in, int* item)
{
    ScrollItemAnim anim;
    ScrollItemAnim* table = (ScrollItemAnim*)lbl_804D4710;
    ScrollItemRow* row;
    float plane[4];
    float dx;
    float dy;
    float dz;
    float px;
    float py;
    float pz;
    float d0;
    float d1;
    float t;
    float len;
    float inv;
    unsigned char buffer[0x284];
    void* texture;
    int count;
    int i;

    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
    out[4] = in[4];
    out[5] = in[5];
    out[6] = in[6];
    out[7] = in[7];
    out[8] = in[8];
    out[9] = in[9];
    out[10] = in[10];
    out[11] = in[11];

    texture = fn_8000F990(item[4]);
    if (texture != 0) {
        GXLoadTexObj((GXTexObj*)texture, GX_TEXMAP1);
        fn_8010576C(lbl_804D470C);
    } else {
        fn_80105D84(lbl_804D470C);
    }

    if (table == 0 || ((int*)table)[0xb] != 1) {
        anim.mode = 1;
        anim.unk04 = 0;
        anim.unk05 = 0;
        anim.unk06 = 0;
        anim.unk07 = 0;
        anim.index = 1;
        anim.unk0C = 0;

        row = &anim.rows[0];
        row->unk00 = 0;
        row->unk0C = 1;
        row->x = (float)item[1];
        row->y = (float)item[2];
        row->z = (float)item[3];
        row->scale = lbl_804D2A20;

        dx = lbl_80441620[0] - lbl_80441614[0];
        dy = lbl_80441620[1] - lbl_80441614[1];
        dz = lbl_80441620[2] - lbl_80441614[2];

        count = 0;
        if (fabsf(dz) > lbl_804D68BC) {
            count++;
        }
        if (fabsf(dx) > lbl_804D68BC) {
            count++;
        }
        if (fabsf(dy) > lbl_804D68BC) {
            count++;
        }
        if (count == 0) {
            dx = lbl_804D68A8;
            dy = lbl_804D68A8;
            dz = lbl_804D68A8;
        } else {
            len = dx * dx + dy * dy + dz * dz;
            inv = 1.0f / sqrtf(len);
            dx *= inv;
            dy *= inv;
            dz *= inv;
        }
        row->dirX = dx * lbl_804D68D4;
        row->dirY = dy * lbl_804D68D4;
        row->dirZ = dz * lbl_804D68D4;
        row->unk38 = -0x20;
    } else {
        anim = *table;
        for (i = 0; i < anim.index; i++) {
            float lerpX;
            float lerpY;
            float lerpZ;

            row = &anim.rows[i];
            if (row->unk00 == 2) {
                ViewPlaneFromNormal(plane, (const Vec3*)&row->dirX,
                                    (const Vec3*)lbl_80441620);
                t = row->src1;
                px = row->src8 + row->dirX * t;
                py = row->src9 + row->dirY * t;
                pz = row->srcA + row->dirZ * t;
                d0 = plane[2] * row->srcA + plane[0] * row->src8 + plane[1] * row->src9;
                d1 = plane[2] * pz + plane[0] * px + plane[1] * py;
                if (d0 != d1) {
                    t = (-plane[3] - d0) / (d1 - d0);
                } else {
                    t = lbl_804D68A8;
                }
                lerpX = row->src8 + (px - row->src8) * t;
                lerpY = row->src9 + (py - row->src9) * t;
                lerpZ = row->srcA + (pz - row->srcA) * t;
                row->src8 += (lerpX - row->src8) * lbl_804D68D0;
                row->src9 += (lerpY - row->src9) * lbl_804D68D0;
                row->srcA += (lerpZ - row->srcA) * lbl_804D68D0;
            }
            row->dirX *= lbl_804D68D4;
            row->dirY *= lbl_804D68D4;
            row->dirZ *= lbl_804D68D4;
            row->x = (float)item[1];
            row->y = (float)item[2];
            row->z = (float)item[3];
            row->scale = lbl_804D2A20;
            row->unk38 = -0x20;
        }
    }

    fn_80112D84(&anim, 0x105, buffer, lbl_804D68A0);
    GfxSetMaterial(buffer);
    GfxSetCullMode(2);
    return 1;
}

// Twelve-word (0x30-byte) state block that the scroll-item builders copy into
// their output. Size and use are descriptive guesses.
struct ScrollItemState12 {
    int w[12];
};

// Builds one scroll-item row whose facing direction is the segment between the
// two global markers (lbl_80441614 -> lbl_80441620), appends it to a fresh
// animation record and submits that record for material setup. Names are
// descriptive guesses.
int FxBuildScrollItemAlongSegment(int* out, int* in, int* item)
{
    ScrollItemAnim anim;
    unsigned char buffer[0x280];
    float dx;
    float dy;
    float dz;
    float len;
    float inv;
    int count;

    *(ScrollItemState12*)out = *(ScrollItemState12*)in;
    fn_80105D84(lbl_804D470C);

    anim.mode = 1;
    anim.unk04 = 0;
    anim.unk05 = 0;
    anim.unk06 = 0;
    anim.unk07 = 0;
    anim.index = 0;
    anim.unk0C = 0;

    anim.rows[anim.index].unk00 = 0;
    anim.rows[anim.index].unk0C = 1;
    anim.rows[anim.index].x = (float)item[1];
    anim.rows[anim.index].y = (float)item[2];
    anim.rows[anim.index].z = (float)item[3];
    anim.rows[anim.index].scale = lbl_804D68A0;

    dz = lbl_80441620[2] - lbl_80441614[2];
    dx = lbl_80441620[0] - lbl_80441614[0];
    dy = lbl_80441620[1] - lbl_80441614[1];

    count = 0;
    if (fabsf(dz) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dx) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dy) > lbl_804D68BC) {
        count++;
    }
    if (count == 0) {
        anim.rows[anim.index].dirX = lbl_804D68A8;
        anim.rows[anim.index].dirY = lbl_804D68A8;
        anim.rows[anim.index].dirZ = lbl_804D68A8;
    } else {
        len = dx * dx + dy * dy + dz * dz;
        inv = 1.0f / sqrtf(len);
        anim.rows[anim.index].dirX = dx * inv;
        anim.rows[anim.index].dirY = dy * inv;
        anim.rows[anim.index].dirZ = dz * inv;
    }

    anim.rows[anim.index].dirX *= lbl_804D68D4;
    anim.rows[anim.index].dirY *= lbl_804D68D4;
    anim.rows[anim.index].dirZ *= lbl_804D68D4;
    anim.rows[anim.index].unk38 = -0x20;
    anim.index++;

    fn_80112D84(&anim, 0x105, buffer, lbl_804D68A0);
    GfxSetMaterial(buffer);
    GfxSetCullMode(2);
    return 1;
}

// Copies a twelve-word scroll-item state block into the caller's output and
// advances the material clock; always returns 1. Names are descriptive guesses.
int FxCopyScrollItemState(int* out, int* in)
{
    *(ScrollItemState12*)out = *(ScrollItemState12*)in;
    fn_80105D84(lbl_804D470C);
    return 1;
}

// Seventeen-word (0x44-byte) variant of the state block above. Size and use are
// descriptive guesses.
struct ScrollItemState17 {
    int w[17];
};

// Copies a seventeen-word scroll-item state block into the caller's output and
// advances the material clock; always returns 1. Names are descriptive guesses.
int FxCopyScrollItemState17(int* out, int* in)
{
    *(ScrollItemState17*)out = *(ScrollItemState17*)in;
    fn_80105D84(lbl_804D470C);
    return 1;
}

// Wave and phase constants used by the scroll-item builders below.
extern float lbl_804D68DC;
extern float lbl_804D68E0;
extern float lbl_804D68E4;
extern float lbl_804D68E8;
extern float lbl_804D68EC;
extern "C" void fn_801D8A68(int arg);

// Builds a wave-modulated scroll item record: the facing direction starts as
// the segment between the two global markers (lbl_80441614 -> lbl_80441620),
// is normalised, then wobbles with the current scroll clock using the wave
// constants; the result is normalised again and handed to the material builder.
// The item position is scaled by the stored proximity constant. Always returns
// 1. Names are descriptive guesses.
int FxBuildScrollItemWaveAtClock(int* out, int* in, int* item)
{
    ScrollItemAnim anim;
    ScrollItemRow* row;
    float dir[3];
    float* dst;
    unsigned char buffer[0x280];
    float phase;
    float len;
    float inv;
    int count;
    int i;

    for (i = 0; i < 12; i++) {
        out[i] = in[i];
    }

    anim.mode = 1;
    anim.unk04 = 0x60;
    anim.unk05 = 0x60;
    anim.unk06 = 0x60;
    anim.unk07 = 0x60;
    anim.index = 1;
    anim.unk0C = 0;

    row = &anim.rows[0];
    row->unk00 = 0;
    row->unk0C = 1;
    row->x = (float)item[1] / lbl_804D6898;
    row->y = (float)item[2] / lbl_804D6898;
    row->z = (float)item[3] / lbl_804D6898;
    row->scale = lbl_804D68A0;

    dst = &row->dirX;
    dir[0] = lbl_80441620[0] - lbl_80441614[0];
    dir[1] = lbl_80441620[1] - lbl_80441614[1];
    dir[2] = lbl_80441620[2] - lbl_80441614[2];

    count = 0;
    if (fabsf(dir[2]) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dir[0]) > lbl_804D68BC) {
        count++;
    }
    if (fabsf(dir[1]) > lbl_804D68BC) {
        count++;
    }
    if (count == 0) {
        dst[0] = lbl_804D68A8;
        dst[1] = lbl_804D68A8;
        dst[2] = lbl_804D68A8;
    } else {
        len = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
        inv = 1.0f / sqrtf(len);
        dst[0] = dir[0] * inv;
        dst[1] = dir[1] * inv;
        dst[2] = dir[2] * inv;
    }

    if (item[4] != 0 || item[5] != 0 || item[6] != 0) {
        phase = lbl_804D68CC *
                (float)(((int)(lbl_80441620[0] + lbl_80441620[1] + lbl_80441620[2]) +
                         lbl_804D470C) &
                        0xfff) /
                lbl_804D68DC;
        dst[0] = lbl_804D68E0 * fn_800A1394(phase) + dst[0];
        dst[1] = lbl_804D68E0 * fn_800A1394(lbl_804D68E8 * phase + lbl_804D68E4) + dst[1];
        dst[2] = lbl_804D68E0 * fn_800A1394(lbl_804D68D0 * phase + lbl_804D68EC) + dst[2];

        count = 0;
        if (fabsf(dst[2]) > lbl_804D68BC) {
            count++;
        }
        if (fabsf(dst[0]) > lbl_804D68BC) {
            count++;
        }
        if (fabsf(dst[1]) > lbl_804D68BC) {
            count++;
        }
        if (count == 0) {
            dst[0] = lbl_804D68A8;
            dst[1] = lbl_804D68A8;
            dst[2] = lbl_804D68A8;
        } else {
            len = dst[0] * dst[0] + dst[1] * dst[1] + dst[2] * dst[2];
            inv = 1.0f / sqrtf(len);
            dst[0] *= inv;
            dst[1] *= inv;
            dst[2] *= inv;
        }
    }

    row->unk38 = -0x20;
    dir[0] = -dir[0];
    dir[1] = -dir[1];
    dir[2] = -dir[2];

    fn_80112D84(&anim, 0x101, buffer, lbl_804D68A0);
    GfxSetMaterial(buffer);
    GfxSetCullMode(2);
    fn_801D8A68(8);
    return 1;
}

// Return a null/zero value to the caller. Name is a guess.
int NullAccessorB6DC() {
    return 0;
}

// Return a null/zero value to the caller. Name is a guess.
int NullAccessorB6E4() {
    return 0;
}

// ---- Strip renderer (0x8008B6EC) -----------------------------------------
// A 13-byte entry of the strip's vertex stream: a matrix index followed by six
// big-endian 16-bit indices into the position, normal, colour and
// texture-coordinate arrays. Names are descriptive guesses.
struct FxStripVertex {
    u8 mtxIndex;     // 0x00
    u8 posIndex[2];  // 0x01
    u8 vecIndex0[2]; // 0x03
    u8 vecIndex1[2]; // 0x05
    u8 vecIndex2[2]; // 0x07
    u8 colIndex[2];  // 0x09
    u8 texIndex[2];  // 0x0b
};

// The data a strip draw is built from: a vertex stream plus the shared
// position/normal/colour/texcoord arrays it indexes. Names are descriptive
// guesses.
struct FxStripDraw {
    unsigned char unk00[0x04];      // 0x00
    u32 flags;                      // 0x04
    u32 count;                      // 0x08
    unsigned char unk0C[0x14 - 0x0c]; // 0x0c
    u8* stream;                     // 0x14
    u8* positions;                  // 0x18
    u8* vectors;                    // 0x1c
    u8* colors;                     // 0x20
    u8* texcoords;                  // 0x24
};

void GfxSetVtxDesc(int desc);        // 0x80008DAC

extern float lbl_804D68F0;
extern float lbl_804D68F4;
extern float lbl_804D68F8;

// Reads an unaligned big-endian 16-bit index out of the vertex stream.
static inline u16 FxReadStreamIndex(const u8* p)
{
    return (u16)(((u32)p[0] << 8) | (u32)p[1]);
}

// Draws a triangle strip/fan from the strip's vertex stream: one byte, four
// vectors, one colour and one texcoord per vertex. When the proximity fade is
// at full strength the vectors are used as-is; otherwise a sine wave driven by
// the elapsed time displaces the first vector and modulates the alpha. Names
// are descriptive guesses.
int FxDrawStrip(void* owner, void* data)
{
    FxStripDraw* strip = (FxStripDraw*)data;
    int total = strip->count * 13;
    int i;

    GfxSetVtxDesc(1);
    GXBegin((strip->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT1,
            (u16)strip->count);
    if (lbl_804D68A8 == lbl_804D2A18) {
        for (i = 0; i < total; i += 13) {
            u8* rec = strip->stream + i + 0xb;
            f32* p0 = (f32*)(strip->positions + FxReadStreamIndex(rec + 1) * 0xc);
            f32* p1 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 3) * 0xc);
            f32* p2 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 5) * 0xc);
            f32* p3 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 7) * 0xc);
            u8* col = strip->colors + FxReadStreamIndex(rec + 9) * 4;
            s16* uv = (s16*)(strip->texcoords + FxReadStreamIndex(rec + 11) * 4);

            GXWGFifo.u8 = rec[0];
            GXPosition3f32(p0[0], p0[1], p0[2]);
            GXPosition3f32(p1[0], p1[1], p1[2]);
            GXPosition3f32(p2[0], p2[1], p2[2]);
            GXPosition3f32(p3[0], p3[1], p3[2]);
            GXColor4u8(col[0], col[1], col[2], 0x7f);
            GXTexCoord2s16(uv[0], uv[1]);
        }
    } else {
        float phase = lbl_804D68F0 * (float)((double)lbl_804D470C - lbl_804D68C0);

        for (i = 0; i < total; i += 13) {
            u8* rec = strip->stream + i + 0xb;
            f32* p0 = (f32*)(strip->positions + FxReadStreamIndex(rec + 1) * 0xc);
            f32* p1 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 3) * 0xc);
            f32* p2 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 5) * 0xc);
            f32* p3 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 7) * 0xc);
            u8* col = strip->colors + FxReadStreamIndex(rec + 9) * 4;
            s16* uv = (s16*)(strip->texcoords + FxReadStreamIndex(rec + 11) * 4);
            float d0;
            float d1;
            float d2;
            u8 alpha;

            d0 = lbl_804D2A18 * (lbl_804D68D0 *
                 fn_800A1394(lbl_804D68E4 * (lbl_804D68F4 * p0[0] + phase)));
            d1 = lbl_804D2A18 * (lbl_804D68D0 *
                 fn_800A1394(lbl_804D68E4 * (lbl_804D68F4 * p0[1] + phase)));
            d2 = lbl_804D2A18 * (lbl_804D68D0 *
                 fn_800A1394(lbl_804D68E4 * (lbl_804D68F4 * p0[2] + phase)));
            alpha = (u8)((int)(lbl_804D68F8 *
                    ((lbl_804D68E0 * d0 + lbl_804D68D8) *
                     (lbl_804D68E0 * d1 + lbl_804D68D8) *
                     (lbl_804D68E0 * d2 + lbl_804D68D8))) + 0x7f);

            GXWGFifo.u8 = rec[0];
            GXPosition3f32(p0[0] + d0, p0[1] + d1, p0[2] + d2);
            GXPosition3f32(p1[0], p1[1], p1[2]);
            GXPosition3f32(p2[0], p2[1], p2[2]);
            GXPosition3f32(p3[0], p3[1], p3[2]);
            GXColor4u8(col[0], col[1], col[2], alpha);
            GXTexCoord2s16(uv[0], uv[1]);
        }
    }
    GfxSetVtxDesc(7);
    return 0;
}

// ---- State-block copy with texture rebind (0x8008BB54) -------------------
// Something a 12-word state block can carry at +0x10: a texture handle, or -1
// when the block has none. Names are descriptive guesses.
struct FxTextureSource {
    unsigned char unk00[0x10]; // 0x00
    int handle;                // 0x10
};

// Copies a 12-word state block. If the source block carries a texture handle,
// releases/binds that texture and stores the handle in the copy's first word.
// Names are descriptive guesses.
int CopyState12BindTexture(void* dst, void* src, void* source)
{
    unsigned int* out = (unsigned int*)dst;
    unsigned int* in = (unsigned int*)src;
    FxTextureSource* tex = (FxTextureSource*)source;

    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
    out[4] = in[4];
    out[5] = in[5];
    out[6] = in[6];
    out[7] = in[7];
    out[8] = in[8];
    out[9] = in[9];
    out[10] = in[10];
    out[11] = in[11];

    if (tex->handle != -1) {
        fn_80010448(tex->handle, 0);
        out[0] = tex->handle;
    }
    return 1;
}

// Seventeen-word variant of CopyState12BindTexture: copies a 17-word (0x44-byte)
// state block and, when the source block carries a texture handle at +0x10,
// binds that texture and stores the handle in the copy's first two words. Names
// are descriptive guesses.
int CopyState17BindTexture(void* dst, void* src, void* source)
{
    unsigned int* out = (unsigned int*)dst;
    unsigned int* in = (unsigned int*)src;
    FxTextureSource* tex = (FxTextureSource*)source;

    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
    out[4] = in[4];
    out[5] = in[5];
    out[6] = in[6];
    out[7] = in[7];
    out[8] = in[8];
    out[9] = in[9];
    out[10] = in[10];
    out[11] = in[11];
    out[12] = in[12];
    out[13] = in[13];
    out[14] = in[14];
    out[15] = in[15];
    out[16] = in[16];

    if (tex->handle != -1) {
        fn_80010448(tex->handle, 0);
        out[0] = tex->handle;
        out[1] = in[1];
    }
    return 1;
}

// ---- Projected strip draw (0x8008BCF8) -----------------------------------
// Per-draw state of the projected strip variant: the alpha scale, the two
// integers that scale/offset the generated texture coordinates and the texture
// object handle (-1 when the draw has none). Names are descriptive guesses.
struct FxStripDrawState {
    unsigned char unk00[0x0c]; // 0x00
    int unk0C;                 // 0x0c
    int unk10;                 // 0x10
    int unk14;                 // 0x14
    int texture;               // 0x18
    int unk1C;                 // 0x1c
};

// Constant colour channels of the projected strip draw, and the texcoord scale.
extern u32 lbl_804D2A3C;
extern u32 lbl_804D2A40;
extern u32 lbl_804D2A44;
extern float lbl_804D68FC;

// Draws a triangle strip/fan from the strip's vertex stream with a constant
// colour whose alpha comes from the draw state, and texture coordinates
// projected out of the vertex position. Each vertex is one matrix index, three
// vectors and a position index at offsets 0x0b..0x13 of a 13-byte record; the
// fourth vector comes from the second and third record entries advanced by one
// and two vectors. Returns 0. Names are descriptive guesses.
int FxDrawStripProjected(void* owner, void* data, void* state)
{
    FxStripDraw* strip = (FxStripDraw*)data;
    FxStripDrawState* s = (FxStripDrawState*)state;
    u8 alpha = (u8)((s->unk0C * 0x28) / 0x1000);
    int i;

    if (s->texture >= 0) {
        fn_80010330(s->texture, 0);
    }
    GfxSetVtxDesc(1);
    GfxSetMaterial(0);
    GXBegin((strip->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT1,
            (u16)strip->count);

    for (i = 0; i < (int)strip->count; i++) {
        u8* rec = strip->stream + i * 13 + 0xb;
        f32* p0 = (f32*)(strip->positions + FxReadStreamIndex(rec + 1) * 0xc);
        f32* v0 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 3) * 0xc);
        f32* v1 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 5) + 1) * 0xc);
        f32* v2 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 7) + 2) * 0xc);
        int offset = s->unk1C * s->unk14;
        int u = (int)((float)s->unk10 * (lbl_804D68D0 * p0[0]) +
                      lbl_804D68FC * (float)offset);
        int v = (int)(lbl_804D68D0 * p0[2] + (float)offset);

        GXWGFifo.u8 = rec[0];
        GXPosition3f32(p0[0], p0[1], p0[2]);
        GXPosition3f32(v0[0], v0[1], v0[2]);
        GXPosition3f32(v1[0], v1[1], v1[2]);
        GXPosition3f32(v2[0], v2[1], v2[2]);
        GXColor4u8(lbl_804D2A3C, lbl_804D2A40, lbl_804D2A44, alpha);
        GXTexCoord2s16((s16)u, (s16)v);
    }

    GfxSetVtxDesc(7);
    return 0;
}

// Advances a phase accumulator by "delta" and recomputes the wrapped phase
// stored at +0x1c: the accumulator modulo the period at +0x04, scaled to
// 0x1000. Names are descriptive guesses.
void FxAdvanceRingPhase(int* ring, int delta)
{
    ring[0] += delta;
    ring[7] = (ring[0] % ring[1]) * 0x1000 / ring[1];
}

// Per-draw state of the screen-projected strip variant: the alpha scale at
// +0x0c, the texcoord scale at +0x18 and the two projection centres at +0x1c
// and +0x20. Names are descriptive guesses.
struct FxStripScreenTexState {
    unsigned char unk00[0x0c];        // 0x00
    int unk0C;                        // 0x0c
    unsigned char unk10[0x18 - 0x10]; // 0x10
    int unk18;                        // 0x18
    int unk1C;                        // 0x1c
    int unk20;                        // 0x20
};

void GfxSetCullMode(int mode);                      // 0x80008B44
extern "C" int fn_800A1624(int value);              // 0x800A1624
extern "C" int fn_800A1688(int value);              // 0x800A1688

extern int lbl_804D2A28;
extern int lbl_804D2A2C;
extern int lbl_804D2A30;
extern float lbl_804D2A34;
extern float lbl_804D2A38;
extern int lbl_804D4714;
extern float lbl_804D6900;

// Draws a triangle strip/fan from the strip's vertex stream like FxDrawStrip,
// but projects the vertex position into screen space with fn_800A1688 and
// fn_800A1624 to build the texture coordinates, and uses a constant colour
// whose alpha comes from the draw state. Returns 0. Names are descriptive
// guesses.
int FxDrawStripScreenTex(void* owner, void* data, void* state)
{
    FxStripDraw* strip = (FxStripDraw*)data;
    FxStripScreenTexState* s = (FxStripScreenTexState*)state;
    u8 alpha = (u8)((s->unk0C * 0x7f) / 0x1000);
    f32 scaleU = lbl_804D68A0 / lbl_804D2A34;
    f32 scaleV = lbl_804D68A0 / lbl_804D2A38;
    f32 biasU = (f32)lbl_804D2A28 / lbl_804D6900;
    f32 biasV = (f32)lbl_804D2A2C / lbl_804D6900;
    int i;

    GfxSetVtxDesc(1);
    GfxSetMaterial(0);
    GXBegin((strip->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT1,
            (u16)strip->count);

    for (i = 0; i < (int)strip->count; i++) {
        u8* rec = strip->stream + i * 13 + 0xb;
        f32* p0 = (f32*)(strip->positions + FxReadStreamIndex(rec + 1) * 0xc);
        f32* v0 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 3) * 0xc);
        f32* v1 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 5) + 1) * 0xc);
        f32* v2 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 7) + 2) * 0xc);
        f32 su = scaleU * p0[0];
        f32 sv = scaleV * p0[2];
        int cu = fn_800A1688(s->unk1C +
                             (int)(lbl_804D68D0 * (su - biasU) *
                                   (sv * (f32)lbl_804D2A28)));
        int cv = fn_800A1624(s->unk20 +
                             (int)(lbl_804D68D0 * (sv - biasV) *
                                   (su * (f32)lbl_804D2A2C)));
        s16 tu = (s16)(lbl_804D2A30 + s->unk18 * ((int)(f32)cu / 2) / 0x1000);
        s16 tv = (s16)(lbl_804D4714 + s->unk18 * ((int)(f32)cv / 2) / 0x1000);

        GXWGFifo.u8 = rec[0];
        GXPosition3f32(p0[0], p0[1], p0[2]);
        GXPosition3f32(v0[0], v0[1], v0[2]);
        GXPosition3f32(v1[0], v1[1], v1[2]);
        GXPosition3f32(v2[0], v2[1], v2[2]);
        GXColor4u8(lbl_804D2A3C, lbl_804D2A40, lbl_804D2A44, alpha);
        GXTexCoord2s16(tu, tv);
    }

    GfxSetVtxDesc(7);
    GfxSetCullMode(1);
    return 0;
}

// Enable flag of the ring phase accumulators below; the accumulators are
// frozen while it is clear. Name is a descriptive guess.
extern int lbl_804D2A24;

// Advances the counter at +0x00 of a ring by delta and recomputes the two
// wrapped phases at +0x1c and +0x20: the counter modulo the periods at +0x04
// and +0x08, scaled to 0x1000. Names are descriptive guesses.
void FxAdvanceRingPhases(int* ring, int delta)
{
    if (lbl_804D2A24 == 0) {
        return;
    }
    ring[0] += delta;
    ring[7] = (ring[0] % ring[1]) * 0x1000 / ring[1];
    ring[8] = (ring[0] % ring[2]) * 0x1000 / ring[2];
}

// Per-draw animation state of the wave strip variant: the frame counter at
// +0x00 that shifts the wave phase. Names are descriptive guesses.
struct FxStripWaveState {
    int unk00; // 0x00
};

extern float lbl_804D6920;
extern float lbl_804D6924;
extern float lbl_804D6928;
extern float lbl_804D692C;
extern float lbl_804D6930;

// Draws a triangle strip/fan like FxDrawStrip, but drives the vertex colours
// from a wave: the length of the (x, z) part of each position sets a sine whose
// cube, scaled by a cosine term taken from the draw state's frame counter,
// becomes the alpha channel. Returns 0. Names are descriptive guesses.
int FxDrawStripWave(void* owner, void* data, void* state)
{
    FxStripDraw* strip = (FxStripDraw*)data;
    FxStripWaveState* waveState = (FxStripWaveState*)state;
    int i;

    GfxSetVtxDesc(1);
    GfxSetMaterial(0);
    GXBegin((strip->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT1,
            (u16)strip->count);

    for (i = 0; i < (int)strip->count; i++) {
        u8* rec = strip->stream + i * 13 + 0xb;
        f32* p0 = (f32*)(strip->positions + FxReadStreamIndex(rec + 1) * 0xc);
        f32* v0 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 3) * 0xc);
        f32* v1 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 5) + 1) * 0xc);
        f32* v2 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 7) + 2) * 0xc);
        u8* col = strip->colors + FxReadStreamIndex(rec + 9) * 4;
        s16* uv = (s16*)(strip->texcoords + FxReadStreamIndex(rec + 11) * 4);
        f32 len = sqrtf(p0[0] * p0[0] + p0[2] * p0[2]);
        f32 osc = lbl_804D68A8;
        f32 sn;
        u8 alpha;

        if (len != lbl_804D68A8) {
            osc = lbl_804D6928 *
                  (f32)cos(len * lbl_804D6920 +
                           (f32)waveState->unk00 * lbl_804D6924);
        }

        sn = (f32)sin(lbl_804D692C * (len / lbl_804D6930));
        alpha = (u8)(int)((lbl_804D68D0 * osc + lbl_804D68B0) *
                          (sn * sn * sn));

        GXWGFifo.u8 = rec[0];
        GXPosition3f32(p0[0], p0[1], p0[2]);
        GXPosition3f32(v0[0], v0[1], v0[2]);
        GXPosition3f32(v1[0], v1[1], v1[2]);
        GXPosition3f32(v2[0], v2[1], v2[2]);
        GXColor4u8(col[0], col[1], col[2], alpha);
        GXTexCoord2s16(uv[0], uv[1]);
    }

    GfxSetVtxDesc(7);
    return 0;
}

// Advances p[0] by p[1] * n / 4096 (rounding toward zero). Names are
// descriptive guesses.
void ScaleAccumulator(int* p, int n)
{
    *p += (n * p[1]) / 4096;
}

// Per-draw state that drives the displaced strip: the frame counter whose
// cosine term offsets the first vertex and the alpha scale at +0x0c. Names are
// descriptive guesses.
struct FxStripDisplaceState {
    int time;   // 0x00
    int unk04;  // 0x04
    int unk08;  // 0x08
    int alpha;  // 0x0c
};

// Draws a triangle strip/fan like FxDrawStrip, but displaces each record's first
// position along the (x, z) direction of that position and modulates the alpha
// with the squared sine of the position's (x, z) length, scaled by the draw
// state's frame counter and alpha. Returns 0. Names are descriptive guesses.
int FxDrawStripDisplaced(void* owner, void* data, void* state)
{
    FxStripDraw* strip = (FxStripDraw*)data;
    FxStripDisplaceState* disp = (FxStripDisplaceState*)state;
    int i;

    GfxSetVtxDesc(1);
    GfxSetMaterial(0);
    GXBegin((strip->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT1,
            (u16)strip->count);

    for (i = 0; i < (int)strip->count; i++) {
        u8* rec = strip->stream + i * 13 + 0xb;
        f32* p0 = (f32*)(strip->positions + FxReadStreamIndex(rec + 1) * 0xc);
        f32* v0 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 3) * 0xc);
        f32* v1 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 5) + 1) * 0xc);
        f32* v2 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 7) + 2) * 0xc);
        u8* col = strip->colors + FxReadStreamIndex(rec + 9) * 4;
        s16* uv = (s16*)(strip->texcoords + FxReadStreamIndex(rec + 11) * 4);
        f32 len = sqrtf(p0[0] * p0[0] + p0[2] * p0[2]);
        f32 dx = lbl_804D68A8;
        f32 dy = lbl_804D68A8;
        f32 dz = lbl_804D68A8;
        f32 sn;
        u8 alpha;

        if (len != lbl_804D68A8) {
            f32 t = len * lbl_804D6920;
            f32 c = lbl_804D6928 *
                    (f32)cos((f32)disp->time * lbl_804D6924 + t);

            dx = p0[0] / len * c;
            dz = p0[2] / len * c;
            dy = lbl_804D6928 *
                 (f32)cos((f32)disp->time * lbl_804D6924 + t);
        }

        GXWGFifo.u8 = rec[0];
        GXPosition3f32(p0[0] + dx, p0[1] + dy, p0[2] + dz);
        GXPosition3f32(v0[0], v0[1], v0[2]);
        GXPosition3f32(v1[0], v1[1], v1[2]);
        GXPosition3f32(v2[0], v2[1], v2[2]);

        sn = (f32)sin(lbl_804D692C * (len / lbl_804D6930));
        alpha = (u8)((int)((lbl_804D68D0 * dy + lbl_804D68B0) * (sn * sn)) *
                     disp->alpha / 4096);

        GXColor4u8(col[0], col[1], col[2], alpha);
        GXTexCoord2s16(uv[0], uv[1]);
    }

    GfxSetVtxDesc(7);
    return 0;
}

// Adds n * the scale at +0x04 / 4096 to the accumulated value at +0x00,
// rounding toward zero. Same shape as ScaleAccumulator above, but a separate
// helper in retail. Name is a descriptive guess.
void AddScaledInt(int* p, int n)
{
    *p += (n * p[1]) / 4096;
}

// Per-draw state of the constant-colour strip variant: the three colour
// channels written for every vertex live at +0x10, +0x14 and +0x18; the alpha
// of each vertex comes from its entry in the colour array. Names are
// descriptive guesses.
struct FxStripConstColorState {
    unsigned char unk00[0x10]; // 0x00
    u32 red;                   // 0x10
    u32 green;                 // 0x14
    u32 blue;                  // 0x18
};

// Draws a triangle strip/fan from the strip's vertex stream like FxDrawStrip,
// but writes one constant colour for every vertex of the strip; the alpha comes
// from the fourth byte of the indexed vertex colour. No material state is set.
// Returns 0. Name is a descriptive guess.
int FxDrawStripConstColor(void* owner, void* data, void* state)
{
    FxStripDraw* strip = (FxStripDraw*)data;
    FxStripConstColorState* s = (FxStripConstColorState*)state;
    int i;

    GfxSetVtxDesc(1);
    GXBegin((strip->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT1,
            (u16)strip->count);

    for (i = 0; i < (int)strip->count; i++) {
        u8* rec = strip->stream + i * 13 + 0xb;
        f32* p0 = (f32*)(strip->positions + FxReadStreamIndex(rec + 1) * 0xc);
        f32* v0 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 3) * 0xc);
        f32* v1 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 5) + 1) * 0xc);
        f32* v2 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 7) + 2) * 0xc);
        u8* col = strip->colors + FxReadStreamIndex(rec + 9) * 4;
        s16* uv = (s16*)(strip->texcoords + FxReadStreamIndex(rec + 11) * 4);

        GXWGFifo.u8 = rec[0];
        GXPosition3f32(p0[0], p0[1], p0[2]);
        GXPosition3f32(v0[0], v0[1], v0[2]);
        GXPosition3f32(v1[0], v1[1], v1[2]);
        GXPosition3f32(v2[0], v2[1], v2[2]);
        GXColor4u8(s->red, s->green, s->blue, col[3]);
        GXTexCoord2s16(uv[0], uv[1]);
    }

    GfxSetVtxDesc(7);
    return 0;
}

// Per-draw state of the flat-shaded strip variants: only the vertex alpha at
// +0x10 is used. Layout is a descriptive guess.
struct FxStripVertexAlpha {
    unsigned char unk00[0x10]; // 0x00
    u32 alpha;                 // 0x10
};

// Draws a triangle strip/fan like FxDrawStripConstColor, but every vertex's
// alpha comes from the draw state at +0x10 instead of the record's colour.
// Returns 0. Name is a descriptive guess.
int FxDrawStripStateAlpha(void* owner, void* data, void* state)
{
    FxStripDraw* strip = (FxStripDraw*)data;
    FxStripVertexAlpha* s = (FxStripVertexAlpha*)state;
    int i;
    int offset = 0;

    GfxSetVtxDesc(1);
    GXBegin((strip->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT1,
            (u16)strip->count);

    for (i = 0; i < (int)strip->count; i++) {
        u8* rec = strip->stream + offset + 0xb;
        f32* p0 = (f32*)(strip->positions + FxReadStreamIndex(rec + 1) * 0xc);
        f32* v0 = (f32*)(strip->vectors + FxReadStreamIndex(rec + 3) * 0xc);
        f32* v1 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 5) + 1) * 0xc);
        f32* v2 = (f32*)(strip->vectors + (FxReadStreamIndex(rec + 7) + 2) * 0xc);
        u8* col = strip->colors + FxReadStreamIndex(rec + 9) * 4;
        s16* uv = (s16*)(strip->texcoords + FxReadStreamIndex(rec + 11) * 4);

        offset += 13;

        GXWGFifo.u8 = rec[0];
        GXPosition3f32(p0[0], p0[1], p0[2]);
        GXPosition3f32(v0[0], v0[1], v0[2]);
        GXPosition3f32(v1[0], v1[1], v1[2]);
        GXPosition3f32(v2[0], v2[1], v2[2]);
        GXColor4u8(col[0], col[1], col[2], s->alpha);
        GXTexCoord2s16(uv[0], uv[1]);
    }

    GfxSetVtxDesc(7);
    return 0;
}

// The data a dual-texcoord strip draw is built from: 10-byte records that
// index a position, a vector, a vertex colour and two texture coordinates.
// Names and layout are descriptive guesses.
struct FxStripDrawDual {
    unsigned char unk00[0x08];        // 0x00
    u32 count;                        // 0x08
    u32 flags;                        // 0x0c
    unsigned char unk10[0x2c - 0x10]; // 0x10
    u8* stream;                       // 0x2c
    unsigned char unk30[0x34 - 0x30]; // 0x30
    u8* positions;                    // 0x34
    u8* vectors;                      // 0x38
    u8* colors;                       // 0x3c
    u8* texcoords;                    // 0x40
};

// Draws a triangle strip/fan from a dual-texcoord vertex stream: one position,
// one vector, one colour (alpha from the draw state at +0x10) and two texture
// coordinates per vertex. Returns 0. Name is a descriptive guess.
int FxDrawStripDualTexcoord(void* owner, void* data, void* state)
{
    FxStripDrawDual* strip = (FxStripDrawDual*)data;
    FxStripVertexAlpha* s = (FxStripVertexAlpha*)state;
    int i;

    GfxSetVtxDesc(0);
    GXBegin((strip->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT0,
            (u16)strip->count);

    for (i = 0; i < (int)strip->count; i++) {
        u16* rec = (u16*)(strip->stream + i * 10 + 0xc);
        f32* p0 = (f32*)(strip->positions + rec[0] * 0xc);
        f32* v0 = (f32*)(strip->vectors + rec[1] * 0xc);
        u8* col = strip->colors + rec[2] * 4;
        s16* uv0 = (s16*)(strip->texcoords + rec[3] * 4);
        s16* uv1 = (s16*)(strip->texcoords + rec[4] * 4);

        GXPosition3f32(p0[0], p0[1], p0[2]);
        GXPosition3f32(v0[0], v0[1], v0[2]);
        GXColor4u8(col[0], col[1], col[2], s->alpha);
        GXTexCoord2s16(uv0[0], uv0[1]);
        GXTexCoord2s16(uv1[0], uv1[1]);
    }

    GfxSetVtxDesc(6);
    return 0;
}

// Per-draw state of the offset strip variant: the fade value at +0x10 and the
// projection height at +0x14. Layout is a descriptive guess.
struct FxStripOffsetState {
    unsigned char unk00[0x10]; // 0x00
    int fade;                  // 0x10
    int height;                // 0x14
};

// Draws a dual-texcoord strip like FxDrawStripDualTexcoord, but displaces each
// vertex along x/z by an amount looked up with fn_800A1624 from the projected
// x coordinate and scaled by the fade at +0x10 over the height at +0x14 of the
// draw state. Returns 0. Name is a descriptive guess.
int FxDrawStripWarp(void* owner, void* data, void* state)
{
    FxStripDrawDual* strip = (FxStripDrawDual*)data;
    FxStripOffsetState* s = (FxStripOffsetState*)state;
    int fade = 0xff - s->fade;
    int amp = 0x20;
    int i;
    int offset = 0;

    if (fade < 0x20) {
        amp = fade;
    }

    GfxSetVtxDesc(0);
    GXBegin((strip->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT0,
            (u16)strip->count);

    for (i = 0; i < (int)strip->count; i++) {
        u16* rec = (u16*)(strip->stream + offset + 0xc);
        f32* p0 = (f32*)(strip->positions + rec[0] * 0xc);
        f32* v0 = (f32*)(strip->vectors + rec[1] * 0xc);
        u8* col = strip->colors + rec[2] * 4;
        s16* uv0 = (s16*)(strip->texcoords + rec[3] * 4);
        s16* uv1 = (s16*)(strip->texcoords + rec[4] * 4);
        int tex = fn_800A1624(fade * 8 +
                              (s->height / 2 + (int)p0[0]) / 16 + 0x980) +
                  0x1000;
        u32 disp = tex * (s->height / 2 + (int)p0[0]) / s->height * amp / 32 * 2;

        offset += 10;

        GXPosition3f32(p0[0] - (f32)disp, p0[1], p0[2] + (f32)disp);
        GXPosition3f32(v0[0], v0[1], v0[2]);
        GXColor4u8(col[0], col[1], col[2], col[3]);
        GXTexCoord2s16(uv0[0], uv0[1]);
        GXTexCoord2s16(uv1[0], uv1[1]);
    }

    GfxSetVtxDesc(6);
    return 0;
}

// Adds value to the counter the pointer points at. Name is a descriptive guess.
void FxCounterAdd(int* counter, int value)
{
    *counter += value;
}

// A 12-word record copied verbatim by the two helpers below before they call
// into the screen-space setup code. Layout and names are descriptive guesses.
struct FxScreenRecord {
    int w00;                            // 0x00
    int w04;                            // 0x04
    int w08;                            // 0x08
    int w0C;                            // 0x0c
    int w10;                            // 0x10
    int w14;                            // 0x14
    int w18;                            // 0x18
    int w1C;                            // 0x1c
    int w20;                            // 0x20
    int w24;                            // 0x24
    int w28;                            // 0x28
    int w2C;                            // 0x2c
};

// Screen-space callback shared with another unit; still under its placeholder
// name there, so it is declared with C linkage.
extern "C" void fn_8010EB70(float x, float y, int mode);
extern float lbl_804D68B4;

// Copies the caller's record and forwards the position stored at cfg[1]/cfg[2],
// normalised by the scroll scale, to the screen-space callback. Name is a
// descriptive guess.
int FxScreenRecordCopy(FxScreenRecord* dst, const FxScreenRecord* src, int* cfg)
{
    *dst = *src;
    fn_8010EB70((float)cfg[1] / lbl_804D68B4, (float)cfg[2] / lbl_804D68B4, 0);
    return 1;
}

// Same as FxScreenRecordCopy, but picks the callback mode from cfg[3] and
// forwards the position at cfg[7]/cfg[8]. Name is a descriptive guess.
int FxScreenRecordCopyMode(FxScreenRecord* dst, const FxScreenRecord* src, int* cfg)
{
    int mode = 0;

    *dst = *src;
    if (cfg[3] == 1) {
        mode = 4;
    }
    fn_8010EB70((float)cfg[7] / lbl_804D68B4, (float)cfg[8] / lbl_804D68B4, mode);
    return 1;
}

// A 17-word (0x44-byte) record copied verbatim by the helper below. Layout and
// names are descriptive guesses.
struct FxScreenRecord17 {
    int w00; // 0x00
    int w04; // 0x04
    int w08; // 0x08
    int w0C; // 0x0c
    int w10; // 0x10
    int w14; // 0x14
    int w18; // 0x18
    int w1C; // 0x1c
    int w20; // 0x20
    int w24; // 0x24
    int w28; // 0x28
    int w2C; // 0x2c
    int w30; // 0x30
    int w34; // 0x34
    int w38; // 0x38
    int w3C; // 0x3c
    int w40; // 0x40
};

// Copies the caller's 17-word record and forwards the position stored at
// cfg[7]/cfg[8], normalised by the scroll scale, to the screen-space callback;
// the callback mode comes from cfg[3]. Name is a descriptive guess.
int FxScreenRecordCopy17(FxScreenRecord17* dst, const FxScreenRecord17* src,
                         int* cfg)
{
    int mode = 0;

    *dst = *src;
    if (cfg[3] == 1) {
        mode = 4;
    }
    fn_8010EB70((float)cfg[7] / lbl_804D68B4, (float)cfg[8] / lbl_804D68B4,
                mode);
    return 1;
}

// Frame-scale callback shared with another unit; still under its placeholder
// name there, so it is declared with C linkage.
extern "C" float fn_802A2080(void);

// Advances the record's counter at 0x00 by v scaled by the current frame scale,
// then recomputes the two fractional accumulators (0x1c/0x20) from the counters
// at 0x04/0x08 and the multipliers at 0x10/0x14. Layout and name are
// descriptive guesses.
void FxCounterAdvance(int* rec, u32 v)
{
    int d;

    *rec += (int)((float)v * fn_802A2080());
    rec[8] = 0;
    rec[7] = 0;
    d = rec[1];
    if (d != 0 || rec[4] != 0) {
        rec[7] = rec[4] * (((*rec % d) * 0x1000) / d);
    }
    d = rec[2];
    if (d != 0 || rec[5] != 0) {
        rec[8] = rec[5] * (((*rec % d) * 0x1000) / d);
    }
}

extern "C" float fn_802A2080(void); // 0x802A2080

// Advances a ring's counter by a random step (the returned random scale times
// the caller's delta) and recomputes its two wrapped phases at +0x1c and +0x20
// from the counter modulo the periods at +0x04 and +0x08, scaled to 0x1000.
// Names are descriptive guesses.
void FxAdvanceRingRandom(int* ring, u32 delta)
{
    float step = fn_802A2080();
    int period;
    int scale;

    ring[0] += (int)((float)delta * step);
    ring[7] = 0;
    ring[8] = ring[5];
    period = ring[1];
    if (period != 0 && ring[4] != 0) {
        scale = ring[4];
        ring[7] = scale * (((ring[0] % period) * 0x1000) / period);
    }
}

// A mesh drawn straight into the GX FIFO. The 10-byte records at +0x2c hold
// position, normal, colour and two texcoord indices (read at +0x0c..+0x14);
// the arrays they index live at +0x34 (positions), +0x38 (normals), +0x3c
// (colours) and +0x40 (texcoords). Field names are descriptive guesses.
struct FxTexMesh {
    unsigned char unk00[0x08];        // 0x00
    int count;                        // 0x08
    u32 flags;                        // 0x0c
    unsigned char unk10[0x2c - 0x10]; // 0x10
    u8* entries;                      // 0x2c
    unsigned char unk30[0x34 - 0x30]; // 0x30
    u8* positions;                    // 0x34
    u8* normals;                      // 0x38
    u8* colors;                       // 0x3c
    u8* texcoords;                    // 0x40
};

// The per-draw frame data the texmesh uses: the two texcoord biases at +0x1c
// and +0x20 and the projection base at +0x24. Names are descriptive guesses.
struct FxTexMeshFrame {
    unsigned char unk00[0x1c]; // 0x00
    int biasU;                 // 0x1c
    int biasV;                 // 0x20
    int base;                  // 0x24
};

// Draws a texmesh triangle strip/fan into the GX FIFO. Each record emits a
// position (projected with fn_800A1624 and fixed-point scaled), a normal, a
// colour, and two texcoords, the first biased by the frame data. Returns 0.
// Names are descriptive guesses.
int FxDrawTexMesh(void* owner, void* data, void* frameData)
{
    FxTexMesh* mesh = (FxTexMesh*)data;
    FxTexMeshFrame* frame = (FxTexMeshFrame*)frameData;
    int i;

    GfxSetVtxDesc(0);
    GXBegin((mesh->flags & 1) ? GX_TRIANGLES : GX_TRIANGLESTRIP, GX_VTXFMT1,
            (u16)mesh->count);

    for (i = 0; i < mesh->count; i++) {
        u8* rec = mesh->entries + i * 0xa;
        u16 posIdx = *(u16*)(rec + 0x0c);
        u16 nrmIdx = *(u16*)(rec + 0x0e);
        u16 colIdx = *(u16*)(rec + 0x10);
        u16 texIdx0 = *(u16*)(rec + 0x12);
        u16 texIdx1 = *(u16*)(rec + 0x14);
        f32* pos = (f32*)(mesh->positions + posIdx * 0xc);
        f32* nrm;
        u8* col;
        s16* uv0;
        s16* uv1;
        int key;
        f32 x;
        f32 y;
        f32 z;

        key = (int)(pos[2] + (pos[0] + pos[1]));
        z = (f32)(fn_800A1624(frame->base + key / 8) / 128) + pos[2];
        y = (f32)(fn_800A1624(frame->base + key * 8) / 256) + pos[1];
        x = (f32)(fn_800A1624(frame->base + key * 4) / 256) + pos[0];

        GXPosition3f32(x, y, z);
        nrm = (f32*)(mesh->normals + nrmIdx * 0xc);
        GXNormal3f32(nrm[0], nrm[1], nrm[2]);
        col = mesh->colors + colIdx * 4;
        GXColor4u8(col[0], col[1], col[2], col[3]);
        uv0 = (s16*)(mesh->texcoords + texIdx0 * 4);
        GXTexCoord2s16((s16)(uv0[0] + frame->biasU),
                       (s16)(uv0[1] + frame->biasV));
        uv1 = (s16*)(mesh->texcoords + texIdx1 * 4);
        GXTexCoord2s16(uv1[0], uv1[1]);
    }

    GfxSetVtxDesc(6);
    return 0;
}

// fn_8008D978: advances a rotating counter and recomputes three scaled
// remainder values from it. Names are descriptive guesses.
void AdvanceScaledCounter(int* counter, int delta)
{
    int divisor;

    *counter = *counter + delta;
    counter[8] = 0;
    counter[7] = 0;
    divisor = counter[1];
    if (divisor != 0 && counter[4] != 0)
    {
        counter[7] = counter[4] * (((*counter - (*counter / divisor) * divisor) * 0x1000) / divisor);
    }
    divisor = counter[2];
    if (divisor != 0 && counter[5] != 0)
    {
        counter[8] = counter[5] * (((*counter - (*counter / divisor) * divisor) * 0x1000) / divisor);
    }
    divisor = counter[3];
    if (divisor != 0 && counter[6] != 0)
    {
        counter[9] = counter[6] * (((*counter - (*counter / divisor) * divisor) * 0x1000) / divisor);
    }
}

// fn_8008DA3C: clears the low state bits of one entry of the 0x24-byte table
// at lbl_804416A0 and resets the two words after lbl_804482A0, returning the
// old value of lbl_804482A0. Names are descriptive guesses.
int ClearEntryFlags(unsigned int param)
{
    int idx;
    int old;
    u32* entry;

    idx = (int)(param >> 16) / 8;
    if (idx < 0 || idx >= 0x300) {
        return 0;
    }
    entry = (u32*)((u8*)lbl_804416A0 + idx * 0x24);
    lbl_804482A4 = 0;
    old = lbl_804482A0;
    lbl_804482A8 = -1;
    *entry &= ~0x2;
    *entry &= ~0x4;
    *entry &= ~0x8;
    *entry &= ~0x10;
    *entry &= ~0x20;
    *entry &= ~0x40;
    *entry &= ~0x80;
    return old;
}

// fn_8008DADC: reports whether the "enabled" bit (bit 0) of the 0x24-byte table
// entry selected by the caller's key is set. Names are descriptive guesses.
int IsEntryFlagSet(unsigned int param)
{
    int idx;
    u32* entry;

    idx = (int)(param >> 16) / 8;
    if (idx < 0 || idx >= 0x300) {
        return 0;
    }
    entry = (u32*)((u8*)lbl_804416A0 + idx * 0x24);
    if ((*entry & 1) != 0) {
        return 1;
    }
    return 0;
}

// fn_8008DB1C: applies the flags of the 0x24-byte table entry selected by the
// caller's key to the scroll sources it configures, then queues the caller's
// value for the next frame. Returns the entry index, 0 when the value is
// already queued, or -1 when the key selects no entry. Names are descriptive
// guesses.
unsigned int FxApplyEntryFlags(unsigned int key, int value)
{
    u32* entry;
    u32 flags;
    int index;
    int count;
    int* pending;

    index = (int)(key >> 16) / 8;
    if (index < 0 || index >= 0x300) {
        return -1;
    }
    if (lbl_804482A0 == key) {
        count = lbl_804482A4;
        if (count > 0) {
            pending = &lbl_804482A8;
            do {
                if (*pending == value) {
                    return 0;
                }
                pending++;
            } while (--count != 0);
        }
    }
    entry = (u32*)((u8*)lbl_804416A0 + index * 0x24);
    flags = *entry;
    if ((flags & 0x2) != 0) {
        fn_80088F0C(value, 1);
        *entry |= 1;
    } else if ((flags & 0x4) != 0) {
        fn_80088F0C(value, 0);
    }
    if ((flags & 0x8) != 0) {
        fn_80088FB0(value, 1);
    } else if ((flags & 0x10) != 0) {
        fn_80088FB0(value, 0);
    }
    if ((flags & 0x40) != 0) {
        fn_80088C10(value, (const char*)&entry[1], entry[2], entry[3], entry[4],
                    entry[5], entry[6], entry[7]);
    } else if ((flags & 0x80) != 0) {
        fn_80088BA0(value, entry[2], entry[3], entry[4], entry[5],
                                entry[6], entry[7]);
    }
    if ((flags & 0x20) != 0) {
        fn_80088E68(value, entry[8]);
    }
    if (lbl_804482A4 < 0x20) {
        (&lbl_804482A8)[lbl_804482A4] = value;
        lbl_804482A4++;
    }
    return (unsigned int)index;
}

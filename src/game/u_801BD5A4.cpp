// u_801BD5A4.cpp (0x801BD5A4-0x801BE008): a registry of behaviour types (6-word entries: hash, init, free, update, msg, params).
// Split from the old u_801AE50C (0x801AE50C-0x801BEC4C). Names are descriptive guesses; matched against retail
// unless noted.
// Boundary evidence (split 2026-11-22): the registry table .data 0x803AB708-0x803AB9D8 with its own empty 8-byte params
// constant (.rodata 0x803486A0), .sdata 0x804D3200-0x804D3208; no float constants. Its .rodata const sits after the
// previous file's string pool (CW puts a file's string pool after its other .rodata), so it is not part of that file.
// The next file has a second registry with a different entry layout (5 words) and its own empty params constant
// (0x803486C0).

#include "game/string.h"
#include "game/msg.h"
#include "game/owner.h"

typedef unsigned char u8;
typedef unsigned long u32;

extern "C" int SendMsg(int to, int msg, void* data);
void OwnerSlotRemove(Owner* o);

// The character a registered behaviour type runs on (only the fields used here; see game/owner.h).
struct SveInfo {
    u8 pad00[0xC];
    u32 type;                  // 0x0C: the type the character starts in
};

struct SveOwner {
    u8 pad00[0x38];
    int id;                    // 0x038
    u8 pad3C[0xC4 - 0x3C];
    SveInfo* info;             // 0x0C4
    u8 padC8[0x104 - 0xC8];
    void* update;              // 0x104: the current type's update callback
    u8 pad108[0x120 - 0x108];
    void* msg;                 // 0x120: the current type's message handler
    u8 pad124[4];
    u32 type;                  // 0x128: current registered type (-1: none)
    u8 pad12C[4];
    void* state;               // 0x130
    u8 pad134[0x908 - 0x134];
    u32 variant;               // 0x908
};

// One registered behaviour type (6 words).
struct SveType {
    u32 hash;                          // 0x00
    void (*enter)(SveOwner* o);        // 0x04
    void (*leave)(SveOwner* o);        // 0x08
    void* update;                      // 0x0C
    void* msg;                         // 0x10
    const char* name;                  // 0x14
};

extern SveType lbl_803AB708[];   // the type table
extern int lbl_804D3200;         // number of entries (30)

void AiNoOp_801BE004();
void AiNoOp_801BE000();
int AiReturnFalse_801BDFF8();
int AiReturnFalse_801BDFF0();

static inline int SveFind(u32 hash) {
    int i;
    for (i = 0; i < lbl_804D3200; i++) {
        if (hash == lbl_803AB708[i].hash) {
            return i;
        }
    }
    return -1;
}

static inline void SveLeave(SveOwner* o) {
    if (o != 0) {
        int i = SveFind(o->type);
        if (i != -1) {
            if (lbl_803AB708[i].leave) {
                lbl_803AB708[i].leave(o);
            }
            OwnerSlotRemove((Owner*)o);
            o->update = 0;
            o->state = 0;
            o->msg = 0;
            o->type = -1;
        } else {
            o->type = -1;
        }
    }
}

static inline void SveEnter(u32 hash, void* p) {
    if (p != 0) {
        SveOwner* o = (SveOwner*)p;
        int i = SveFind(hash);
        if (i != -1) {
            SveLeave(o);
            o->update = lbl_803AB708[i].update;
            o->msg = lbl_803AB708[i].msg;
            o->type = hash;
            o->state = 0;
            if (lbl_803AB708[i].enter) {
                lbl_803AB708[i].enter(o);
            }
        }
    }
}

// Makes a registered type do nothing.
extern "C" void fn_801BD5A4(u32 hash) {
    int i = SveFind(hash);
    if (i >= 0) {
        lbl_803AB708[i].enter = (void (*)(SveOwner*))AiNoOp_801BE004;
        lbl_803AB708[i].leave = (void (*)(SveOwner*))AiNoOp_801BE000;
        lbl_803AB708[i].update = (void*)AiReturnFalse_801BDFF8;
        lbl_803AB708[i].msg = (void*)AiReturnFalse_801BDFF0;
    }
}

// Replaces the callbacks of a registered type.
extern "C" void fn_801BD62C(u32 hash, void (*enter)(SveOwner*), void (*leave)(SveOwner*), void* update,
                            void* msg) {
    int i = SveFind(hash);
    if (i >= 0) {
        lbl_803AB708[i].enter = enter;
        lbl_803AB708[i].leave = leave;
        lbl_803AB708[i].update = update;
        lbl_803AB708[i].msg = msg;
    }
}

// Asks a character for its 0x8132F1B1 value (0 if it does not answer).
inline int QuerySend(int* out, int target, u32 hash) {
    QueryMsg m;
    int sent;
    m.hash = hash;
    *(int*)&m.result = 0;
    m.unk8 = 0;
    sent = SendMsg(target, 0x41E, &m);
    *out = *(int*)&m.result;
    return sent;
}

extern "C" int fn_801BD694(SveOwner* o) {
    int value;
    int sent;

    if (o == 0) {
        return 0;
    }
    sent = QuerySend(&value, o->id, 0x8132F1B1);
    return sent != 0 ? value : 0;
}

// The name of a registered type ("" if unknown).
extern "C" String fn_801BD74C(u32 hash) {
    int i = SveFind(hash);
    if (i != -1) {
        return String(lbl_803AB708[i].name);
    }
    return String("");
}

// Starts the character's own type. Type 0x12EC1 starts 0xC8422A5B or 0xB1C30D62 instead (by the variant
// word), unless the character already runs one of the three.
extern "C" void fn_801BD7D4(SveOwner* o) {
    u32 type;

    if (o == 0) {
        return;
    }
    type = o->info->type;
    if (type == 0x12EC1 && o->type != 0x12EC1 && o->type != 0xB1C30D62 && o->type != 0xC8422A5B) {
        if (o->variant == 0x8CD18EE1) {
            SveEnter(0xC8422A5B, o);
        } else {
            SveEnter(0xB1C30D62, o);
        }
        return;
    }
    SveEnter(type, o);
}

// True when the character does not run its own type.
extern "C" int fn_801BDC0C(SveOwner* o) {
    if (o == 0) {
        return 0;
    }
    return o->type != o->info->type;
}

// Switches a character to a registered type.
extern "C" void fn_801BDC3C(u32 hash, SveOwner* o) {
    SveEnter(hash, o);
}

// Leaves the character's current type.
extern "C" void fn_801BDDA4(SveOwner* o) {
    SveLeave(o);
}

// Switches type without resetting the state word.
extern "C" void fn_801BDE6C(u32 hash, SveOwner* o) {
    int i;

    if (o == 0) {
        return;
    }
    if (o->type != 0xFFFFFFFF) {
        SveLeave(o);
        o->type = -1;
    }
    i = SveFind(hash);
    if (i != -1) {
        if (lbl_803AB708[i].enter) {
            lbl_803AB708[i].enter(o);
        }
        o->update = lbl_803AB708[i].update;
        o->msg = lbl_803AB708[i].msg;
        o->type = hash;
    }
}

int AiReturnFalse_801BDFF0() {
    return 0;
}

int AiReturnFalse_801BDFF8() {
    return 0;
}

void AiNoOp_801BE000() {
}

void AiNoOp_801BE004() {
}

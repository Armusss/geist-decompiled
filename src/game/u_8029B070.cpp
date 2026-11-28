// u_8029B070.cpp (0x8029B070-0x8029C0D0), split from the old u_80294548 range: a manager over .sbss
// 0x804D5718 with two BufferArray instances (Slot, vtable 0x803C3148; Entry12, vtable 0x803C3168).
// Boundary evidence:
// - .sdata2: own pool 0x804DA760-0x804DA778 (0.0f at 0x804DA760 duplicates the previous file's 0x804DA730).
// - .data 0x803C3148-0x803C3188 (the two vtables) and .sbss 0x804D5718 are referenced only from this range;
//   the next unit's data starts at 0x803C3188 / 0x804DA778.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

#include "game/bufarray.h"
#include "game/owner.h"

int MemFlagIsSet(void* bits, int index);

// 12-byte element.
struct Entry12 {
    int a;            // owner id (-1: free)
    unsigned long b;  // owner +0x908
    float c;
};

// An array of Entry12 (vtable 0x803C3168).
typedef BufferArray<Entry12> EntryList;

// 0x2C-byte record holding an EntryList at +0x08.
struct Slot {
    u32 key;          // 0x00
    int unk04;        // 0x04
    EntryList list;   // 0x08
    float unk1C;      // 0x1C
    float unk20;      // 0x20
    float unk24;      // 0x24
    int unk28;        // 0x28
};

// An array of Slot (vtable 0x803C3148).
typedef BufferArray<Slot> SlotArray;

extern SlotArray* lbl_804D5718;

//@@8029B070
// Drop the owner's entry from the slot `key` (any slot for key 0).
extern "C" void fn_8029B070(Owner* owner, u32 key);
void fn_8029B070(Owner* owner, u32 key) {
    if (owner == 0) {
        return;
    }
    if (MemFlagIsSet(owner->conditions, 0x26) == 0 && key != 0x2237D4) {
        return;
    }
    for (Slot* p = lbl_804D5718->Begin(); p != lbl_804D5718->End(); p++) {
        if (key == p->key || key == 0) {
            for (Entry12* e = p->list.Begin(); e != p->list.End(); e++) {
                if (e->a == owner->id) {
                    p->list.Erase((char*)e, sizeof(Entry12));
                    return;
                }
            }
        }
    }
}
//@@end
//@@8029B140
extern "C" int fn_8029B140(Owner* owner, u32 key);
int fn_8029B140(Owner* owner, u32 key) {
    if (owner == 0) {
        return 0;
    }
    if (MemFlagIsSet(owner->conditions, 0x26) == 0 && key != 0x2237D4) {
        return 1;
    }
    for (Slot* p = lbl_804D5718->Begin(), *end = lbl_804D5718->End(); p != end; p++) {
        if (p->key == key) {
            for (Entry12* e = p->list.Begin(), *eend = p->list.End(); e != eend; e++) {
                if (e->a == owner->id) {
                    return 1;
                }
            }
            return 0;
        }
    }
    return 0;
}
//@@end
//@@8029B218
extern "C" int fn_8029B218(Owner* owner, u32 key);
int fn_8029B218(Owner* owner, u32 key) {
    if (owner == 0) {
        return 0;
    }
    if (MemFlagIsSet(owner->conditions, 0x26) == 0 && key != 0x2237D4) {
        return 1;
    }
    for (Slot* p = lbl_804D5718->Begin(); p != lbl_804D5718->End(); p++) {
        if (p->key == key) {
            Entry12* e = p->list.Begin();
            Entry12* oldest = 0;
            int count = 0;
            for (; e != p->list.End(); e++) {
                if (e->a == owner->id) {
                    return 1;
                }
                if (e->a != -1 && e->b == owner->unk908) {
                    count++;
                    if (oldest == 0 || oldest->c < e->c) {
                        oldest = e;
                    }
                }
            }
            if (count < p->unk04) {
                int id = owner->id;
                unsigned long tag = owner->unk908;
                p->unk24 = p->unk20;
                Entry12* n = (Entry12*)p->list.Insert((char*)p->list.End(), sizeof(Entry12));
                n->a = id;
                n->b = tag;
                n->c = 0.0f;
                return 1;
            }
            if (oldest != 0) {
                oldest->a = owner->id;
                oldest->b = owner->unk908;
                oldest->c = 0.0f;
                p->unk24 = p->unk20;
                p->unk28 = owner->id;
                return 1;
            }
        }
    }
    return 0;
}
//@@end
//@@8029B3B0
extern "C" int fn_8029B3B0(Owner* owner, u32 key);
int fn_8029B3B0(Owner* owner, u32 key) {
    if (owner == 0) {
        return 0;
    }
    if (MemFlagIsSet(owner->conditions, 0x26) == 0 && key != 0x2237D4) {
        return 1;
    }
    for (Slot* p = lbl_804D5718->Begin(); p != lbl_804D5718->End(); p++) {
        if (p->key == key) {
            Entry12* e = p->list.Begin();
            int count = 0;
            if (p->unk28 != -1 && p->unk28 != owner->id) {
                return 0;
            }
            for (; e != p->list.End(); e++) {
                if (e->a == owner->id) {
                    return 1;
                }
                if (e->a != -1 && e->b == owner->unk908) {
                    count++;
                }
            }
            if (count < p->unk04) {
                int id = owner->id;
                unsigned long tag = owner->unk908;
                p->unk24 = p->unk20;
                p->unk28 = owner->id;
                Entry12* n = (Entry12*)p->list.Insert((char*)p->list.End(), sizeof(Entry12));
                n->a = id;
                n->b = tag;
                n->c = 0.0f;
                return 1;
            }
            return 0;
        }
    }
    return 0;
}
//@@end
//@@8029B528
extern "C" void fn_8029B528(u32 key, float value);
void fn_8029B528(u32 key, float value) {
    Slot* end = lbl_804D5718->End();
    Slot* p = lbl_804D5718->Begin();

    for (; p != end; p++) {
        if (key == p->key) {
            p->unk1C = value;
            return;
        }
    }
}
//@@end
//@@8029B560
// Make sure `key` has a slot: copy the matching one out (the copy is dropped again), or append a default slot;
// then reset the last slot. (Draft: the shape is not matched yet.)
extern "C" void fn_8029B560(u32 key, int value);
void fn_8029B560(u32 key, int value) {
    Slot* found = 0;
    Slot* p;

    for (p = lbl_804D5718->Begin(); p != lbl_804D5718->End(); p++) {
        if (key == p->key) {
            Slot copy = *p;
            found = &copy;
            break;
        }
    }
    if (found == 0) {
        {
            Slot def;
            *(Slot*)lbl_804D5718->Insert((char*)lbl_804D5718->End(), sizeof(Slot)) = def;
        }
        Slot* last = (Slot*)(lbl_804D5718->mUnk0C + lbl_804D5718->mLength - sizeof(Slot));
        last->key = key;
        last->unk1C = 15000.0f;
        last->unk24 = 0.0f;
        last->unk20 = 200.0f;
        last->unk28 = -1;
        found = last;
    }
    found->unk04 = value;
    found->list.Erase((char*)found->list.Begin(), (found->list.End() - found->list.Begin()) * sizeof(Entry12));
}
//@@end
//@@8029B7D0
extern "C" void fn_8029B7D0(void);
void fn_8029B7D0(void) {
    if (lbl_804D5718->Count() != 0) {
        lbl_804D5718->Erase((char*)lbl_804D5718->Begin(), (lbl_804D5718->End() - lbl_804D5718->Begin()) * sizeof(Slot));
    }
    // Deleted as a Buffer: the array destructor is emitted with the template methods, not after this function.
    delete (Buffer*)lbl_804D5718;
    lbl_804D5718 = 0;
}
//@@end
//@@8029B85C
extern "C" void fn_8029B85C(float dt);
void fn_8029B85C(float dt) {
    for (Slot* p = lbl_804D5718->Begin(); p != lbl_804D5718->End(); p++) {
        Entry12* e = p->list.Begin();
        if (p->unk24 > 0.0f) {
            p->unk24 -= dt;
            if (p->unk24 <= 0.0f) {
                p->unk24 = 0.0f;
                p->unk28 = -1;
            }
        }
        for (; e != p->list.End(); e++) {
            if (e->a != -1) {
                e->c += dt;
                if (p->unk1C > 0.0f && e->c >= p->unk1C) {
                    e->a = -1;
                    e->c = -1.0f;
                }
            }
        }
    }
}
//@@end
//@@8029B920
// Create (or empty) the slot array and register the default slots.
extern "C" void fn_8029B920(void);
void fn_8029B920(void) {
    if (lbl_804D5718 == 0) {
        lbl_804D5718 = new SlotArray;
    }
    if (lbl_804D5718->Count() != 0) {
        lbl_804D5718->Erase((char*)lbl_804D5718->Begin(), (lbl_804D5718->End() - lbl_804D5718->Begin()) * sizeof(Slot));
    }
    fn_8029B560(0x73B1E248, 2);
    fn_8029B560(0x86A69297, 1);
    fn_8029B560(0x55F3E922, 1);
    fn_8029B528(0x55F3E922, 0.0f);
    fn_8029B560(0xB3D49C39, 1);
    fn_8029B528(0xB3D49C39, 0.0f);
    fn_8029B560(0xC1F6F975, 1);
    fn_8029B528(0xC1F6F975, 0.0f);
    fn_8029B560(0x31992E8D, 1);
    fn_8029B528(0x31992E8D, 0.0f);
    fn_8029B560(0x00013E6B, 11);
    fn_8029B528(0x00013E6B, 0.0f);
    fn_8029B560(0x03C72B83, 4);
    fn_8029B528(0x03C72B83, 0.0f);
    fn_8029B560(0x766A16B4, 1);
    fn_8029B528(0x766A16B4, 0.0f);
    fn_8029B560(0x002237D4, 1);
    fn_8029B528(0x002237D4, 8000.0f);
}
//@@end
//@@8029BC3C
char* Buffer::TextEnd() const {
    return mData + mLength;
}
//@@end
// TODO (not drafted yet, 0x68 bytes at 0x8029BC4C)
// fn_8029BC4C
// TODO (not drafted yet, 0x68 bytes at 0x8029BCB4)
// fn_8029BCB4

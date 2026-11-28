// u_8029B070.cpp (0x8029B070-0x8029C0D0), split from the old u_80294548 range: a manager over .sbss
// 0x804D5718 with two BufferArray instances (Slot, vtable 0x803C3148; Entry12, vtable 0x803C3168).
// Boundary evidence:
// - .sdata2: own pool 0x804DA760-0x804DA778 (0.0f at 0x804DA760 duplicates the previous file's 0x804DA730).
// - .data 0x803C3148-0x803C3188 (the two vtables) and .sbss 0x804D5718 are referenced only from this range;
//   the next unit's data starts at 0x803C3188 / 0x804DA778.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>

#include "game/bufarray.h"

// 12-byte element.
struct Entry12 {
    int a;
    int b;
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

// TODO (not drafted yet, 0xD0 bytes at 0x8029B070)
// fn_8029B070
// TODO (not drafted yet, 0xD8 bytes at 0x8029B140)
// fn_8029B140
// TODO (not drafted yet, 0x198 bytes at 0x8029B218)
// fn_8029B218
// TODO (not drafted yet, 0x178 bytes at 0x8029B3B0)
// fn_8029B3B0
//@@8029B528
extern SlotArray* lbl_804D5718;

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
    Slot* last;

    for (p = lbl_804D5718->Begin(); p != lbl_804D5718->End(); p++) {
        if (key == p->key) {
            Slot copy = *p;
            found = &copy;
            break;
        }
    }
    if (found != 0) {
        return;
    }
    {
        Slot def;
        *(Slot*)lbl_804D5718->Insert((char*)lbl_804D5718->End(), sizeof(Slot)) = def;
    }
    last = lbl_804D5718->End() - 1;
    last->key = key;
    last->unk1C = 15000.0f;
    last->unk24 = 0.0f;
    last->unk20 = 200.0f;
    last->unk28 = -1;
    last->unk04 = value;
    last->list.Erase((char*)last->list.Begin(), (last->list.End() - last->list.Begin()) * sizeof(Entry12));
}
//@@end
// TODO (not drafted yet, 0x8C bytes at 0x8029B7D0)
// fn_8029B7D0
// TODO (not drafted yet, 0xC4 bytes at 0x8029B85C)
// fn_8029B85C
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
extern "C" char* fn_8029BC3C(Buffer* b);
char* fn_8029BC3C(Buffer* b) {
    return b->mData + b->mLength;
}
//@@end
// TODO (not drafted yet, 0x68 bytes at 0x8029BC4C)
// fn_8029BC4C
// TODO (not drafted yet, 0x68 bytes at 0x8029BCB4)
// fn_8029BCB4

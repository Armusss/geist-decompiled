// u_800E39F4.cpp (0x800E39F4-0x800E3EC4): the "Bullet" effect type, split from the old u_800E1B5C range.
// Boundary evidence: descriptor 0x8039C3F8 (update 0x800E39F4, query 0x800E39F8, msg 0x800E3A04; fn_800E3A8C
// reads it); .rodata pool 0x80344EA8-0x80344EF8 (ghost-action table + "Bullet"); .sdata2 0x804D71C0-0x804D71D8
// (own int-to-float double 0x804D71D0 next to AUG's 0x804D71A8).
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include "game/effect.h"

int Fn80175D60(unsigned long arg);

//@@800E39F4
extern "C" void fn_800E39F4(EffectObj* obj, float dt);
void fn_800E39F4(EffectObj* obj, float dt) {}
//@@end
//@@800E39F8
extern "C" unsigned long fn_800E39F8(EffectObj* obj, int msg);
unsigned long fn_800E39F8(EffectObj* obj, int msg) {
    return 0x3DA260F7;
}
//@@end
//@@800E3A04
extern "C" void fn_800E3A8C(int arg, int busy, void* data, int owner);
extern "C" int fn_800E3A04(EffectObj* obj, int msg, int arg, void* data);
int fn_800E3A04(EffectObj* obj, int msg, int arg, void* data) {
    int busy = Fn80175D60(arg);

    switch (msg) {
    case 0x14967:
        fn_800E3A8C(arg, busy, data, obj->unk0);
        return 1;
    }
    return 0;
}
//@@end
// TODO (not drafted yet, 0x438 bytes at 0x800E3A8C)
// fn_800E3A8C

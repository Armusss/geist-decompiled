// u_80294548.cpp (0x80294548-0x80299F0C): the DemonCarrier host actor ("CarrierDemon_flame",
// "DemonCarrier_ShootSpikes", ..., bones BONE_RIGHTREARLEG01 ...) and its behaviour table. The old u_80294548
// range (0x80294548-0x8029C0AC) held four files; the other three are u_80299F0C.cpp (SpitAndSqueal effect),
// u_8029A5E4.cpp and u_8029B070.cpp. Boundary evidence for the end at 0x80299F0C:
// - .sdata2: this file's pool is 0x804DA650-0x804DA720 (0.0f at 0x804DA658, int-to-float double 0x804DA688);
//   the next file has its own 0.0f at 0x804DA720. fn_80299994..fn_80299E60 (the last functions here) read the behaviour table
//   0x803C3050, whose entries are this file's fn_802997DC..fn_8029994C.
// - .rodata: string pool 0x8034B688-0x8034B7C0 starting with the ghost-action name table (POSSESS..SCAN); the
//   next file's pool starts with its own copy of the table at 0x8034B7C0.
// - .data 0x803C2C08-0x803C30B0, .sdata 0x804D3810-0x804D3848, .sbss 0x804D5708-0x804D5718 and .bss
//   0x804A44F8-0x804A4598 are referenced only from this range.
// No duplicated constant inside the range, so it stays one file.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include "game/owner.h"

// The carrier's own state block at owner +0x134.
struct CarrierData {
    int state;          // 0x00: current state (index into the 0x18-byte state table 0x803C2CE0)
    int next;           // 0x04: state to return to
    u8 pad08[0x1C - 0x08];
    Owner* owner;       // 0x1C
    int unk20;          // 0x20
    int unk24;          // 0x24
    u8 pad28[0x2C - 0x28];
    float unk2C;        // 0x2C
    u8 pad30[0x48 - 0x30];
    int unk48;          // 0x48
    u8 pad4C[0x58 - 0x4C];
    struct CarrierList* list;  // 0x58
};

struct CarrierList {
    u8 pad00[8];
    u32 bytes;          // 0x08
};

extern "C" {
int fn_80297CDC(void* p);
void fn_80294564(CarrierData* d);
void fn_80298718(void* p, int a, int b, float t);
}
int HasCondition(void* conditions, int bit);
extern "C" void* fn_8016F120(void* fx, IVec3* pos);
extern "C" int fn_8016325C(Beh* b);
extern void* lbl_804D382C;
int Rand(void);
void* VAlloc(int size);
void VFree(void* p);

//@@80294548
// For-each callback: keep the highest owner id.
extern "C" int fn_80294548(Owner* o, int* best);
int fn_80294548(Owner* o, int* best) {
    if (*best < o->id) {
        *best = o->id;
    }
    return 1;
}
//@@end
// TODO (not drafted yet, 0x758 bytes at 0x80294564)
// fn_80294564
// TODO (not drafted yet, 0x3D8 bytes at 0x80294CBC)
// fn_80294CBC
//@@80295094
extern "C" float fn_80295094(void);
float fn_80295094(void) {
    return 0.0f;
}
//@@end
// TODO (not drafted yet, 0x2FC bytes at 0x8029509C)
// fn_8029509C
// TODO (not drafted yet, 0x29C bytes at 0x80295398)
// fn_80295398
// TODO (not drafted yet, 0x4E8 bytes at 0x80295634)
// fn_80295634
// TODO (not drafted yet, 0x198 bytes at 0x80295B1C)
// fn_80295B1C
// TODO (not drafted yet, 0x4B4 bytes at 0x80295CB4)
// fn_80295CB4
// TODO (not drafted yet, 0x194 bytes at 0x80296168)
// fn_80296168
// TODO (not drafted yet, 0x14C bytes at 0x802962FC)
// fn_802962FC
// TODO (not drafted yet, 0x328 bytes at 0x80296448)
// fn_80296448
// TODO (not drafted yet, 0x6C bytes at 0x80296770)
// fn_80296770
// TODO (not drafted yet, 0x98 bytes at 0x802967DC)
// fn_802967DC
// TODO (not drafted yet, 0x7C bytes at 0x80296874)
// fn_80296874
//@@802968F0
extern "C" int fn_802968F0(void* p);
int fn_802968F0(void* p) {
    if (p == 0) {
        return 0;
    }
    ((CarrierData*)((Owner*)p)->unk134)->unk2C = 0.0f;
    return 1;
}
//@@end
//@@80296914
extern "C" int fn_80296914(void* p);
int fn_80296914(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    fn_80297CDC(p);
    return d->state;
}
//@@end
//@@80296954
extern "C" int fn_80296954(void* p);
int fn_80296954(void* p) {
    Owner* o = (Owner*)p;
    CarrierData* d;
    if (o == 0) {
        return 0;
    }
    d = (CarrierData*)o->unk134;
    if (d->list->bytes / 8 == 0) {
        fn_80294564(d);
    }
    return d->state;
}
//@@end
//@@802969A8
extern "C" int fn_802969A8(void* p);
int fn_802969A8(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    if (HasCondition(((Owner*)p)->conditions, 9)) {
        return 2;
    }
    fn_80297CDC(p);
    return d->state;
}
//@@end
//@@80296A10
extern "C" int fn_80296A10(void* p);
int fn_80296A10(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    if (HasCondition(((Owner*)p)->conditions, 9)) {
        return 2;
    }
    fn_80297CDC(p);
    return d->state;
}
//@@end
// TODO (not drafted yet, 0x13C bytes at 0x80296A78)
// fn_80296A78
// TODO (not drafted yet, 0x10C bytes at 0x80296BB4)
// fn_80296BB4
// TODO (not drafted yet, 0x84 bytes at 0x80296CC0)
// fn_80296CC0
// TODO (not drafted yet, 0xB8 bytes at 0x80296D44)
// fn_80296D44
// TODO (not drafted yet, 0xB8 bytes at 0x80296DFC)
// fn_80296DFC
//@@80296EB4
extern "C" int fn_80296EB4(void* p);
int fn_80296EB4(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    fn_80297CDC(p);
    return d->state;
}
//@@end
// TODO (not drafted yet, 0x174 bytes at 0x80296EF4)
// fn_80296EF4
// TODO (not drafted yet, 0x1A0 bytes at 0x80297068)
// fn_80297068
// TODO (not drafted yet, 0x104 bytes at 0x80297208)
// fn_80297208
// TODO (not drafted yet, 0xD4 bytes at 0x8029730C)
// fn_8029730C
// TODO (not drafted yet, 0x9C bytes at 0x802973E0)
// fn_802973E0
// TODO (not drafted yet, 0x98 bytes at 0x8029747C)
// fn_8029747C
// TODO (not drafted yet, 0x94 bytes at 0x80297514)
// fn_80297514
//@@802975A8
extern "C" int fn_802975A8(void* p);
int fn_802975A8(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 9;
    d->next = 10;
    d->unk24 = 0;
    fn_80298718(p, 0, 0, 1.0f);
    fn_8016F120(lbl_804D382C, &((Owner*)p)->place.pos);
    return 1;
}
//@@end
//@@80297618
extern "C" int fn_80297618(void* p);
int fn_80297618(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 8;
    d->next = 12;
    d->unk24 = 0;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@80297674
extern "C" int fn_80297674(void* p);
int fn_80297674(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 7;
    d->next = 12;
    d->unk24 = 0;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@802976D0
extern "C" int fn_802976D0(void* p);
int fn_802976D0(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 6;
    d->next = 6;
    d->unk24 = 0;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@80297728
extern "C" int fn_80297728(void* p);
int fn_80297728(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 5;
    d->next = 6;
    d->unk24 = 0;
    fn_80298718(p, 0, 0, 1.0f);
    d->unk48 = Rand() % 2 + 4;
    return 1;
}
//@@end
//@@802977A8
extern "C" int fn_802977A8(void* p);
int fn_802977A8(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 20;
    d->next = 20;
    d->unk24 = 2;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@80297800
extern "C" int fn_80297800(void* p);
int fn_80297800(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 19;
    d->next = 19;
    d->unk24 = 1;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@80297858
extern "C" int fn_80297858(void* p);
int fn_80297858(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 21;
    d->next = 21;
    d->unk24 = 3;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@802978B0
extern "C" int fn_802978B0(void* p);
int fn_802978B0(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 18;
    d->next = 18;
    d->unk24 = 0;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@80297908
extern "C" int fn_80297908(void* p);
int fn_80297908(void* p) {
    CarrierData* d;
    Beh* b;
    int r;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    b = &d->owner->beh;
    d->state = 0x11;
    d->next = 0x11;
    r = fn_8016325C(b);
    if (r == 4 || r == 5) {
        fn_80298718(p, 0, 0, 1.0f);
    }
    return 1;
}
//@@end
//@@80297980
extern "C" int fn_80297980(void* p);
int fn_80297980(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 16;
    d->next = 16;
    d->unk20 = 1;
    d->unk24 = 3;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@802979E0
extern "C" int fn_802979E0(void* p);
int fn_802979E0(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 15;
    d->next = 15;
    d->unk20 = 1;
    d->unk24 = 2;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@80297A40
extern "C" int fn_80297A40(void* p);
int fn_80297A40(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 14;
    d->next = 14;
    d->unk20 = 1;
    d->unk24 = 1;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@80297A9C
extern "C" int fn_80297A9C(void* p);
int fn_80297A9C(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 13;
    d->next = 13;
    d->unk20 = 1;
    d->unk24 = 0;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
//@@80297AFC
extern "C" int fn_80297AFC(void* p);
int fn_80297AFC(void* p) {
    CarrierData* d;
    if (p == 0) {
        return 0;
    }
    d = (CarrierData*)((Owner*)p)->unk134;
    d->state = 12;
    d->next = 12;
    d->unk20 = 0;
    fn_80298718(p, 0, 0, 1.0f);
    return 1;
}
//@@end
// TODO (not drafted yet, 0xA0 bytes at 0x80297B54)
// fn_80297B54
// TODO (not drafted yet, 0xE8 bytes at 0x80297BF4)
// fn_80297BF4
// TODO (not drafted yet, 0x874 bytes at 0x80297CDC)
// fn_80297CDC
// TODO (not drafted yet, 0x1C8 bytes at 0x80298550)
// fn_80298550
// TODO (not drafted yet, 0x348 bytes at 0x80298718)
// fn_80298718
// TODO (not drafted yet, 0xB28 bytes at 0x80298A60)
// fn_80298A60
// TODO (not drafted yet, 0x228 bytes at 0x80299588)
// fn_80299588
// ---------------------------------------------------------------------------------------------------
// The carrier's sub-behaviour: an eight-entry enter/exit/update table (0x803C3050) driven by the generic
// behaviour runner, laid out like the imp's attack behaviour in u_80252F2C.cpp.

struct CarrierBehData {
    int state;   // 0x00
    int next;    // 0x04
    int unk8;    // 0x08
    u32 flags;   // 0x0C
    u8 pad10[0xC];
};

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

void BehUpdate(Owner* o, Beh* b);                         // 0x801632A8
void BehDone(Beh* b);                                     // 0x80162EBC
void BehReset(Beh* b);                                    // 0x801633D4
void Fn8016295C(Owner* o, Beh* b);                        // 0x8016295C

void CarrierBehState0Enter(void* owner, void* beh);
void CarrierBehState0Exit(void* owner, void* beh);
void CarrierBehState0Update(void* owner, void* beh);
void CarrierBehState4Enter(void* owner, void* beh);
void CarrierBehState4Exit(void* owner, void* beh);
void CarrierBehState4Update(void* owner, void* beh);
void CarrierBehState5Enter(void* owner, void* beh);
void CarrierBehState5Exit(void* owner, void* beh);
void CarrierBehState5Update(void* owner, void* beh);

BehState gCarrierBehStates[8] = {
    { CarrierBehState0Enter, CarrierBehState0Exit, CarrierBehState0Update },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { CarrierBehState4Enter, CarrierBehState4Exit, CarrierBehState4Update },
    { CarrierBehState5Enter, CarrierBehState5Exit, CarrierBehState5Update },
    { 0, 0, 0 },
    { 0, 0, 0 },
};

//@@802997B0
extern "C" void fn_802997B0(Owner* o, void* fx);
void fn_802997B0(Owner* o, void* fx) {
    fn_8016F120(fx, &o->place.pos);
}
//@@end
//@@802997DC
void CarrierBehState5Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}
//@@end
//@@80299828
void CarrierBehState4Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}
//@@end
//@@80299874
void CarrierBehState0Update(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}
//@@end
//@@802998C0
void CarrierBehState5Exit(void* owner, void* beh) {}
//@@end
//@@802998C4
void CarrierBehState4Exit(void* owner, void* beh) {}
//@@end
//@@802998C8
void CarrierBehState0Exit(void* owner, void* beh) {}
//@@end
//@@802998CC
void CarrierBehState5Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;
        CarrierBehData* d = ((CarrierBehData*)b->data);

        d->flags = 0;
        d->next = 0;
        d->unk8 = 0;
        BehDone(b);
    }
}
//@@end
//@@8029990C
void CarrierBehState4Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;
        CarrierBehData* d = ((CarrierBehData*)b->data);

        d->flags = 0;
        d->next = 0;
        d->unk8 = 0;
        BehDone(b);
    }
}
//@@end
//@@8029994C
void CarrierBehState0Enter(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;
        CarrierBehData* d = ((CarrierBehData*)b->data);

        d->flags = 0;
        d->next = 0;
        d->unk8 = 0;
        BehDone(b);
    }
}
//@@end
//@@8029998C
extern "C" int fn_8029998C(void* owner, void* beh);
int fn_8029998C(void* owner, void* beh) {
    return 0;
}
//@@end
// TODO (not drafted yet, 0x3BC bytes at 0x80299994)
// fn_80299994
//@@80299D50
extern "C" int fn_80299D50(void* owner, void* beh);
int fn_80299D50(void* owner, void* beh) {
    Beh* b = (Beh*)beh;
    CarrierBehData* d;
    Owner* o = (Owner*)owner;

    if (!b) {
        return 0;
    }
    d = ((CarrierBehData*)b->data);
    if (HasCondition(o->conditions, 9) || HasCondition(o->conditions, 10) || HasCondition(o->conditions, 11)) {
        d->flags = 0;
        BehReset(b);
        BehDone(b);
    } else {
        if (gCarrierBehStates[d->state].update) {
            gCarrierBehStates[d->state].update(o, b);
        }
    }
    return 1;
}
//@@end
//@@80299E28
extern "C" int fn_80299E28(void* owner, void* beh);
int fn_80299E28(void* owner, void* beh) {
    if (!beh) {
        return 0;
    }
    VFree(((CarrierBehData*)((Beh*)beh)->data));
    return 1;
}
//@@end
//@@80299E60
extern "C" int fn_80299E60(void* owner, void* beh);
int fn_80299E60(void* owner, void* beh) {
    Beh* b = (Beh*)beh;
    CarrierBehData* d;

    if (!b) {
        return 0;
    }
    d = (CarrierBehData*)VAlloc(sizeof(CarrierBehData));
    if (d == 0) {
        b->data = 0;
        return 0;
    }
    b->data = d;
    b->state = 0;
    d->state = 0;
    d->next = 0;
    d->unk8 = 0;
    d->flags = 0;
    if (gCarrierBehStates[d->state].enter) {
        gCarrierBehStates[d->state].enter(owner, b);
    }
    return 1;
}
//@@end

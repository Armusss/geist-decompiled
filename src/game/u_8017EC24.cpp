// u_8017EC24.cpp (0x8017EC24-0x8017F9C0), split from u_8017D794.
// "HydraSpit" effect type. Starts after fn_8017EAB0 (called only by fn_8017E288 of the previous file and
// using its constants); its descriptor 0x803A8508 (name "HydraSpit", rodata 0x80346F40 with its own copy of the
// POSSESS..SCAN action names) and vtable-like table 0x803A84A8 point only into 0x8017EC24-0x8017F9C0.
// sdata2 0x804D8270-0x804D8298, sbss 0x804D50C8.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>
#include "game/types.h"
#include "game/string.h"
#include "game/msg.h"
#include "game/owner.h"


void BehUpdate(Owner* owner, Beh* beh);                         // 0x801632A8
void BehDone(Beh* beh);                                         // 0x80162EBC
void Fn8016295C(Owner* owner, Beh* beh);                        // 0x8016295C
void VFree(void* p);                                            // 0x8009DA3C
void* VAlloc(int size);                                         // 0x8009DA68
void BehReset(Beh* beh);                                        // 0x801633D4
int HasCondition(void* owner, int id);                          // 0x800A7AC4
int AssetFind(u32 group, u32 name);                             // 0x8007EB34

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

extern BehState lbl_803A84A8[8];
static int sHydraSpitAssets[2];

// The behaviour's own state (Beh::data).
struct HydraSpitData {
    int state;            // 0x00: index into sHydraSpitStates
    int next;             // 0x04
    int unk08;            // 0x08
    int flags;            // 0x0C
    int target;           // 0x10: set by message 0x41D 0x2C4F0F0A
    int unk14;            // 0x14
    u8 unk18[0x10];       // 0x18
    u8 unk28[0x10];       // 0x28
};

extern "C" {

void fn_8017EF9C(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}

void fn_8017EFE8(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;
        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}

void fn_8017F034(Owner* owner, Beh* beh) {
}

void fn_8017F038(Owner* owner, Beh* beh) {
}

void fn_8017F03C(Owner* owner, Beh* beh) {
}

void fn_8017F040(Owner* owner, Beh* beh) {
    if (beh != 0) {
        HydraSpitData* d = (HydraSpitData*)beh->data;
        d->flags = 0;
        d->next = 0;
        d->unk08 = 0;
        BehDone(beh);
        d->unk14 = 0;
        memset(d->unk28, 0, sizeof(d->unk28));
        memset(d->unk18, 0, sizeof(d->unk18));
    }
}

void fn_8017F0B0(Owner* owner, Beh* beh) {
    if (beh != 0) {
        HydraSpitData* d = (HydraSpitData*)beh->data;
        d->flags = 0;
        d->next = 0;
        d->unk08 = 0;
        BehDone(beh);
    }
}

void fn_8017F0F0(Owner* owner, Beh* beh) {
    if (beh != 0) {
        HydraSpitData* d = (HydraSpitData*)beh->data;
        d->flags = 0;
        d->next = 0;
        d->unk08 = 0;
        BehDone(beh);
    }
}

int fn_8017F130(Owner* owner, Beh* beh, int msg, void* data) {
    HydraSpitData* d;

    if (beh == 0) {
        return 0;
    }
    d = (HydraSpitData*)beh->data;
    switch (msg) {
    case 0x41D:
        switch ((int)((Msg41D*)data)->cmd) {
        case 0x2C4F0F0A:
            d->target = ((Msg41D*)data)->arg;
            return 1;
        }
        return 0;
    }
    return 0;
}

int fn_8017F5DC(Owner* owner, Beh* beh) {
    if (beh == 0) {
        return 0;
    }
    VFree(beh->data);
    return 1;
}

u32 fn_8017F700() {
    return 0x3DA260F7;
}
int fn_8017F504(void* owner_, void* beh_) {
    Beh* b = (Beh*)beh_;
    HydraSpitData* d;
    Owner* o = (Owner*)owner_;

    if (b == 0) {
        return 0;
    }
    d = (HydraSpitData*)b->data;
    if (HasCondition(o->conditions, 9) != 0 ||
        HasCondition(o->conditions, 10) != 0 ||
        HasCondition(o->conditions, 11) != 0) {
        d->flags = 0;
        BehReset(b);
        BehDone(b);
    } else {
        if (lbl_803A84A8[d->state].update) {
            lbl_803A84A8[d->state].update(o, b);
        }
    }
    return 1;
}

int fn_8017F614(void* owner_, void* beh_) {
    Beh* beh;

    if (beh_ == 0) {
        return 0;
    }
    beh = (Beh*)beh_;
    {
        HydraSpitData* d = (HydraSpitData*)VAlloc(0x38);
        if (d == 0) {
            beh->data = 0;
            return 0;
        }
        beh->data = d;
        beh->state = 0;
        d->state = 0;
        d->next = 0;
        d->unk08 = 0;
        d->flags = 0;
        d->target = 0;
        sHydraSpitAssets[0] = AssetFind(0xF1754BF2, 0x9147B62C);
        sHydraSpitAssets[1] = AssetFind(0xF1754BF2, 0x870AA272);
        if (lbl_803A84A8[d->state].enter) {
            lbl_803A84A8[d->state].enter(owner_, beh_);
        }
    }
    return 1;
}
}

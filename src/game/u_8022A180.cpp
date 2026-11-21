// u_8022A180.cpp (0x8022A180-0x8022BA30), split from the old u_80226DE8 range: a follow/attack behaviour with
// its eight-entry enter/exit/update table 0x803BD168 (entries fn_8022AC74..fn_8022B1E8).
// Boundary evidence:
// - .sdata2: own pool 0x804D96F0-0x804D9720 (0.0f at 0x804D96F0, int-to-float double 0x804D9710); the previous
//   file has its own 0.0f at 0x804D967C, the next (MCG) its own 0.0f / double at 0x804D9728 / 0x804D9730.
// - .data 0x803BD168-0x803BD1C8 (the behaviour table); the next file starts with the MCG reset 0x8022BA30
//   listed in the MCG descriptor.
// Names are descriptive guesses; unmatched against retail unless noted.

#include <dolphin/types.h>
#include "game/owner.h"

void* VAlloc(int size);
void VFree(void* p);
void BehDone(Beh* b);                                     // 0x80162EBC
void BehUpdate(Owner* o, Beh* b);                         // 0x801632A8
void Fn8016295C(Owner* o, Beh* b);                        // 0x8016295C
extern "C" void fn_80027690(int handle);

// The behaviour's own state (Beh +0x54).
struct FollowBehData {
    int state;    // 0x00
    int next;     // 0x04
    u32 flags;    // 0x08
    int handle;   // 0x0C: -1 when unused
    int unk10;    // 0x10
    int unk14;    // 0x14
    float unk18;  // 0x18
    float unk1C;  // 0x1C
};

typedef void (*BehStateFunc)(void* owner, void* beh);

struct BehState {
    BehStateFunc enter;
    BehStateFunc exit;
    BehStateFunc update;
};

extern BehState gFollowBehStates[8];

// TODO (not drafted yet, 0xAF4 bytes at 0x8022A180)
// fn_8022A180
//@@8022AC74
extern "C" void fn_8022AC74(void* owner, void* beh);
void fn_8022AC74(void* owner, void* beh) {
    if (beh) {
        FollowBehData* d = (FollowBehData*)((Beh*)beh)->data;
        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}
//@@end
//@@8022ACA0
extern "C" void fn_8022ACA0(void* owner, void* beh);
void fn_8022ACA0(void* owner, void* beh) {
    if (beh) {
        FollowBehData* d = (FollowBehData*)((Beh*)beh)->data;
        if ((d->flags & 0x20) && (d->flags & 0x10)) {
            d->next = 0;
        }
    }
}
//@@end
// TODO (not drafted yet, 0x100 bytes at 0x8022ACCC)
// fn_8022ACCC
//@@8022ADCC
extern "C" void fn_8022ADCC(void* owner, void* beh);
void fn_8022ADCC(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}
//@@end
// TODO (not drafted yet, 0x144 bytes at 0x8022AE18)
// fn_8022AE18
//@@8022AF5C
extern "C" void fn_8022AF5C(void* owner, void* beh);
void fn_8022AF5C(void* owner, void* beh) {
    if (beh) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}
//@@end
//@@8022AFA8
extern "C" void fn_8022AFA8(void* owner, void* beh);
void fn_8022AFA8(void* owner, void* beh) {}
//@@end
//@@8022AFAC
extern "C" void fn_8022AFAC(void* owner, void* beh);
void fn_8022AFAC(void* owner, void* beh) {}
//@@end
//@@8022AFB0
extern "C" void fn_8022AFB0(void* owner, void* beh);
void fn_8022AFB0(void* owner, void* beh) {
    if (beh) {
        FollowBehData* d = (FollowBehData*)((Beh*)beh)->data;
        if (d->handle != -1) {
            fn_80027690(d->handle);
            d->handle = -1;
        }
    }
}
//@@end
//@@8022AFF8
extern "C" void fn_8022AFF8(void* owner, void* beh);
void fn_8022AFF8(void* owner, void* beh) {
    if (beh) {
    }
}
//@@end
//@@8022B000
extern "C" void fn_8022B000(void* owner, void* beh);
void fn_8022B000(void* owner, void* beh) {}
//@@end
//@@8022B004
extern "C" void fn_8022B004(void* owner, void* beh);
void fn_8022B004(void* owner, void* beh) {}
//@@end
//@@8022B008
extern "C" void fn_8022B008(void* owner, void* beh);
void fn_8022B008(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        ((FollowBehData*)b->data)->flags = 0;
        BehDone(b);
    }
}
//@@end
//@@8022B040
extern "C" void fn_8022B040(void* owner, void* beh);
void fn_8022B040(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        ((FollowBehData*)b->data)->flags = 0;
        BehDone(b);
    }
}
//@@end
//@@8022B078
extern "C" void fn_8022B078(void* owner, void* beh);
void fn_8022B078(void* owner, void* beh) {
    if (beh) {
        Beh* b = (Beh*)beh;

        ((FollowBehData*)b->data)->flags = 0;
        BehDone(b);
    }
}
//@@end
//@@8022B0B0
extern "C" void fn_8022B0B0(void* owner, void* beh);
void fn_8022B0B0(void* owner, void* beh) {
    if (beh) {
    }
}
//@@end
// TODO (not drafted yet, 0x130 bytes at 0x8022B0B8)
// fn_8022B0B8
// TODO (not drafted yet, 0x120 bytes at 0x8022B1E8)
// fn_8022B1E8
//@@8022B308
extern "C" int fn_8022B308(void* owner, void* beh);
int fn_8022B308(void* owner, void* beh) {
    return 0;
}
//@@end
// TODO (not drafted yet, 0x404 bytes at 0x8022B310)
// fn_8022B310
// TODO (not drafted yet, 0x1F4 bytes at 0x8022B714)
// fn_8022B714
//@@8022B908
extern "C" int fn_8022B908(void* owner, void* beh);
int fn_8022B908(void* owner, void* beh) {
    FollowBehData* d;
    if (!beh) {
        return 0;
    }
    d = (FollowBehData*)((Beh*)beh)->data;
    if (d->handle != -1) {
        fn_80027690(d->handle);
        d->handle = -1;
    }
    VFree(d);
    return 1;
}
//@@end
//@@8022B964
extern "C" int fn_8022B964(void* owner, void* beh);
int fn_8022B964(void* owner, void* beh) {
    Beh* b = (Beh*)beh;
    FollowBehData* d;

    if (!b) {
        return 0;
    }
    d = (FollowBehData*)VAlloc(sizeof(FollowBehData));
    if (d == 0) {
        b->data = 0;
        return 0;
    }
    b->data = d;
    b->state = 0;
    d->state = 0;
    d->next = 0;
    d->handle = -1;
    d->flags = 0;
    d->unk10 = 500;
    d->unk14 = 750;
    d->unk18 = 0.0f;
    d->unk1C = 0.0f;
    if (gFollowBehStates[d->state].enter) {
        gFollowBehStates[d->state].enter(owner, b);
    }
    return 1;
}
//@@end

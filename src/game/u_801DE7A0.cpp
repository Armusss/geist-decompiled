// u_801DE7A0.cpp (0x801DE7A0-0x801E0850): trail effects (TrailCreate / TrailDestroy, the trail list classes
// with vtables .data 0x803ADD90 / 0x803ADDB0, per-frame update and draw), split from the old u_801D9A70 range.
// Boundary evidence:
// - start 0x801DE7A0: .sdata2 pool 0x804D8D28-0x804D8D60 (0.0f at 0x804D8D28 first read by fn_801DE7DC,
//   own int-to-float double 0x804D8D38); fn_801DE7A0 preloads the same texture (AssetFind 0xF2E2BEB8 /
//   0x17C7B021) that fn_801DE7DC uses, and is the trail module's load hook in fn_801E08CC.
// - .rodata 0x80348DD0-0x80348DF8, .data 0x803ADD28-0x803ADDD0, .sdata 0x804D3250-0x804D3280, .sbss
//   0x804D51F8-0x804D5220.
// - end 0x801E0850: the list classes' inline virtuals (base destructor fn_801E06F8, Commit / Append /
//   GetAlignment and the element hooks, slots of vtables 0x803ADD90 / 0x803ADDB0) are emitted last, as in the
//   Bees file. The module hooks after them are another file (u_801E0850): see there.
// Names are descriptive guesses; unmatched against retail unless noted.

#include "game/types.h"
#include "game/string.h"

typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define NULL 0


struct TrailDesc;

int AssetFind(u32 group, u32 name);
class ScreenEffectBuffer {
public:
    void Commit(int start, unsigned long size);
    void Append(unsigned char* data, unsigned long size);
};
void CrouchNop();
void CrouchAdvancePhases(float dt);
void CrouchInit();
void CrouchResetPhases();
void TimerModuleReset();
void TimerModuleUpdate(float dt);
void TimerModuleLoad();
void TimerModuleUnload();
extern "C" {
void fn_8000F874(int tex);
void fn_801E3AD4(void);
void fn_801E5D5C(void);
void fn_801E5D60(float dt);
void fn_801E3BBC(float dt);
void fn_801E5DD4(void);
void fn_801E3A88(void);
void fn_801E3E14(void);
void fn_801E5E44(void);
void fn_801DECA8(float dt);
void fn_801E0810(u8* begin, u8* end);
void fn_801E0830(u8* begin, u8* end);
}

//@@801DE7A0
// Preloads the trail texture (the same asset fn_801DE7DC draws with); the trail module's load hook.
extern "C" void fn_801DE7A0(void) {
    int tex = AssetFind(0xF2E2BEB8, 0x17C7B021);
    if (tex != -1) {
        fn_8000F874(tex);
    }
}
//@@end

// TODO (not drafted yet, 0x128 bytes at 0x801DE7DC)
extern "C" void fn_801DE7DC(void);

// TODO (not drafted yet, 0x5C bytes at 0x801DE904)
void TrailDestroy(int id);

// TODO (not drafted yet, 0x30 bytes at 0x801DE960)
int TrailCreate(TrailDesc* d);

// TODO (not drafted yet, 0x1DC bytes at 0x801DE990)
extern "C" void fn_801DE990(void);

// TODO (not drafted yet, 0xC4 bytes at 0x801DEB6C)
extern "C" void fn_801DEB6C(void);

// TODO (not drafted yet, 0x78 bytes at 0x801DEC30)
extern "C" void fn_801DEC30(void);

// TODO (not drafted yet, 0xEC bytes at 0x801DECA8)
// (declared above)

// TODO (not drafted yet, 0x178 bytes at 0x801DED94)
extern "C" void fn_801DED94(void);

// TODO (not drafted yet, 0x28C bytes at 0x801DEF0C)
extern "C" void fn_801DEF0C(void);

// TODO (not drafted yet, 0x6A8 bytes at 0x801DF198)
extern "C" void fn_801DF198(void);

// TODO (not drafted yet, 0x108 bytes at 0x801DF840)
extern "C" void fn_801DF840(void);

// TODO (not drafted yet, 0x778 bytes at 0x801DF948)
extern "C" void fn_801DF948(void);

// TODO (not drafted yet, 0x3BC bytes at 0x801E00C0)
extern "C" void fn_801E00C0(void);

// TODO (not drafted yet, 0x27C bytes at 0x801E047C)
extern "C" void fn_801E047C(void);

// TODO (not drafted yet, 0x68 bytes at 0x801E06F8)
extern "C" void fn_801E06F8(void);

//@@801E0760
extern "C" void fn_801E0760(void* list, u8* start, u32 size) {
    ((ScreenEffectBuffer*)list)->Commit((int)start, size);
    fn_801E0810(start, start + (size & ~3u));
}
//@@end

//@@801E07A8
extern "C" void fn_801E07A8(void* list, u8* data, u32 size) {
    u32 start = (u32)data;
    fn_801E0830((u8*)start, (u8*)(start + (size & ~3u)));
    ((ScreenEffectBuffer*)list)->Append(data, size);
}
//@@end

//@@801E0808
// Trail list element hooks (slots 0x10-0x18 of the vtables 0x803ADD90 / 0x803ADDB0; 4-byte elements).
// Written as free functions like the matched u_800BA6FC array overrides.
extern "C" int fn_801E0808(void) {
    return 4;
}
//@@end

//@@801E0810
extern "C" void fn_801E0810(u8* begin, u8* end) {
    for (u8* p = begin; p < end; p += 4) {
    }
}
//@@end

//@@801E0830
extern "C" void fn_801E0830(u8* begin, u8* end) {
    for (u8* p = begin; p < end; p += 4) {
    }
}
//@@end


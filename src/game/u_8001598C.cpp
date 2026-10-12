// Frame timer and empty module hooks (0x8001598C-0x80015A44).
// LoadingTime returns the milliseconds since the previous call, clamped to 1..100.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef signed long s32;

extern "C" u32 OSGetTick(void);

#define BUS_CLOCK (*(u32*)0x800000F8)

static u32 sLastTick;

float LoadingTime(void) {
    u32 now = OSGetTick();
    float dt = (float)(s32)(now - sLastTick) / (float)((BUS_CLOCK / 4) / 1000);
    sLastTick = now;
    if (dt > 100.0f) dt = 100.0f;
    if (dt < 1.0f) dt = 1.0f;
    return dt;
}

void HookNop0(void) {}

void HookNop1(void) {}

int HookFalse0(void) { return 0; }

int HookFalse1(void) { return 0; }

void HookNop2(void) {}

typedef unsigned long u32;
typedef signed long s32;
extern "C" u32 OSGetTick(void);
static u32 lastTick;
#define BUS_CLOCK (*(u32*)0x800000F8)
float FrameSeconds(void) {
    u32 now = OSGetTick();
    float dt = (float)(s32)(now - lastTick) / (float)((BUS_CLOCK / 4) / 1000);
    lastTick = now;
    if (dt > 0.1f) dt = 0.1f;
    if (dt < 0.0001f) dt = 0.0001f;
    return dt;
}

// u_800B4F98.cpp: instance transform reset and a null-safe vector copy (0x800B4F98-0x800B5028). Split
// from u_800B1834.cpp: its two constants (0.0f, 100.0f at .sdata2 0x804D6DA0) sit between that file's
// pools. Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned long u32;

extern "C" void fn_8016D68C(void* dst, void* src);

// Reset the instance's transform and timers to their defaults.
extern "C" int fn_800B4F98(u8* object)
{
    if (object == 0) {
        return 0;
    }
    float rest = 0.0f;
    float idle = 100.0f;
    *(u32*)(object + 0x26c) = 0;
    *(float*)(object + 0x260) = idle;
    *(u32*)(object + 0x268) = 4;
    *(float*)(object + 0x264) = idle;
    *(float*)(object + 0x240) = rest;
    *(float*)(object + 0x244) = rest;
    *(float*)(object + 0x248) = rest;
    fn_8016D68C(object + 0x24c, (void*)(*(u32*)(object + 0xc8) + 0x24));
    return 1;
}

// Copy three floats from `src` unless it is null.
extern "C" void fn_800B5004(float* dst, float* src)
{
    if (src == 0) {
        return;
    }
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

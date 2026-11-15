// Msx.cpp: 0x8016E20C-0x80175D60 (the Euler/box helpers before it are u_8016D170.cpp and u_8016D6BC.cpp):
// level load/unload and effect lookup (FxFind, LevelLoaded), the music/sound system glue (snd* calls), and
// debug text output.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/os.h>
#include <musyx/musyx.h>

typedef short s16;

struct Vec3 {
    float x, y, z;
};

struct Mtx34 {
    float rot[9];
    Vec3 pos;
};

struct Actor;

extern "C" {
void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);
}
void DebugPrintf(const char* fmt, ...);

// Streamed music playback: two mono ADPCM-decoded streams (left/right) fed from double buffers.
extern u32 lbl_804D4FB0;            // sample rate of the streams
extern SND_STREAMID lbl_804D5000;   // left stream
extern SND_STREAMID lbl_804D5004;   // right stream
extern void* lbl_804D5008;          // left buffer
extern void* lbl_804D500C;          // right buffer
extern u32 (*lbl_804D5010)(void* left, void* right, u32 n);   // decoder callback
extern u32 lbl_804D5014;
extern int lbl_804D5018;
extern int lbl_804D501C;
extern int lbl_804D5020;
extern int lbl_804D5024;
extern int lbl_804D5028;
extern int lbl_804D502C;
extern int lbl_804D5030;
extern int lbl_804D5034;
extern int lbl_804D5038;
extern int lbl_804D503C;
extern int lbl_804D2FC0;
extern int lbl_804D2FC4;
extern u32 lbl_804D2FCC;

// StreamSetPaused: a paused stream plays at sample rate 0.
extern "C" void fn_8016E20C(int playing) {
    u32 frq = 0;

    if (playing == 0) {
        frq = lbl_804D4FB0;
    }
    if (lbl_804D5008 != 0) {
        sndStreamFrq(lbl_804D5000, frq);
    }
    if (lbl_804D500C != 0) {
        sndStreamFrq(lbl_804D5004, frq);
    }
}

// StreamShutdown: frees both streams and their buffers.
extern "C" void fn_8016E270(void) {
    if (lbl_804D5000 != -1) {
        sndStreamFree(lbl_804D5000);
        lbl_804D5000 = -1;
    }
    if (lbl_804D5004 != -1) {
        sndStreamFree(lbl_804D5004);
        lbl_804D5004 = -1;
    }
    if (lbl_804D5008 != 0) {
        MemFree(lbl_804D5008, "Msx.cpp", 0x1119);
        lbl_804D5008 = 0;
    }
    if (lbl_804D500C != 0) {
        MemFree(lbl_804D500C, "Msx.cpp", 0x111F);
        lbl_804D500C = 0;
    }
}

static inline void StreamAllocBuffers(void) {
    u32 size = sndStreamAllocLength(0x1B90, 1);

    if (lbl_804D5008 == 0) {
        lbl_804D5008 = MemAlloc(size, "Msx.cpp", 0x10D5);
        if (lbl_804D5008 != 0) {
            lbl_804D500C = MemAlloc(size, "Msx.cpp", 0x10D8);
            if (lbl_804D500C == 0) {
                MemFree(lbl_804D5008, "Msx.cpp", 0x10DB);
                lbl_804D5008 = 0;
            }
        }
    }
    lbl_804D5000 = -1;
    lbl_804D5004 = -1;
}

u32 StreamUpdateLeft(void* b1, u32 n1, void* b2, u32 n2, u32 user);
u32 StreamUpdateRight(void* b1, u32 n1, void* b2, u32 n2, u32 user);

// StreamStart: allocates the buffers if needed and starts both streams. left/right: rate followed by ADPCM info.
extern "C" void fn_8016E314(int* left, int* right, u32 (*decode)(void*, void*, u32)) {
    sndStreamAllocLength(0x1B90, 1);
    lbl_804D5010 = decode;
    if (lbl_804D5008 == 0) {
        StreamAllocBuffers();
    }
    if (lbl_804D5008 != 0 && lbl_804D500C != 0) {
        lbl_804D4FB0 = *left;
        lbl_804D5000 = sndStreamAllocEx(0xFF, lbl_804D5008, 0x1B90, *left, lbl_804D2FCC, 0, 0, 0, 0, 0, 1,
                                        StreamUpdateLeft, 0, (SND_ADPCMSTREAM_INFO*)(left + 1));
        lbl_804D5004 = sndStreamAllocEx(0xFF, lbl_804D500C, 0x1B90, *right, lbl_804D2FCC, 0x7F, 0, 0, 0, 0, 1,
                                        StreamUpdateRight, 0, (SND_ADPCMSTREAM_INFO*)(right + 1));
    }
}

// StreamAlloc: allocates the stream buffers.
extern "C" void fn_8016E4C8(void) { StreamAllocBuffers(); }

// StreamUpdateRight: records the buffer halves for the right stream.
u32 StreamUpdateRight(void* b1, u32 n1, void* b2, u32 n2, u32 user) {
    u32 r;

    lbl_804D5030 = (int)b1;
    lbl_804D5034 = (int)b2;
    lbl_804D5038 = n1;
    lbl_804D503C = n2;
    r = lbl_804D5014;
    if (n1 + n2 < lbl_804D5014) {
        r = n1 + n2;
    }
    return r;
}

// StreamUpdateLeft: decodes both halves of the left stream's buffer through the decoder callback.
u32 StreamUpdateLeft(void* b1, u32 n1, void* b2, u32 n2, u32 user) {
    lbl_804D5028 = 0;
    lbl_804D502C = 0;
    lbl_804D5018 = (int)b1;
    lbl_804D501C = (int)b2;
    lbl_804D5020 = n1;
    lbl_804D5024 = n2;
    if (lbl_804D5010((char*)lbl_804D5008 + ((char*)b1 - (char*)lbl_804D5008),
                     (char*)lbl_804D500C + ((char*)b1 - (char*)lbl_804D5008), (n1 * 8) / 14) == 0) {
        lbl_804D5014 = 0;
        lbl_804D5028 = 1;
    } else {
        if (n2 != 0) {
            if (lbl_804D5010((char*)lbl_804D5008 + ((char*)b2 - (char*)lbl_804D5008),
                             (char*)lbl_804D500C + ((char*)b2 - (char*)lbl_804D5008), (n2 * 8) / 14) == 0) {
                lbl_804D502C = 1;
                n2 = 0;
                DebugPrintf("*StreamUpdate : split case failed\n");
            }
        }
        lbl_804D5014 = n1 + n2;
    }
    return lbl_804D5014;
}

// SoundSetOutputMode: 0 mono, 1 stereo, 2 surround (the OS stays in stereo for surround).
extern "C" void fn_8016E6D8(int mode) {
    lbl_804D2FC0 = mode;
    lbl_804D2FC4 = mode;
    switch (mode) {
    case 0:
        sndOutputMode(SND_OUTPUTMODE_MONO);
        OSSetSoundMode(0);
        break;
    case 1:
        sndOutputMode(SND_OUTPUTMODE_STEREO);
        OSSetSoundMode(1);
        break;
    case 2:
        sndOutputMode(SND_OUTPUTMODE_SURROUND);
        OSSetSoundMode(1);
        break;
    }
}

// SoundGetOutputMode
extern "C" int fn_8016E758(void) { return lbl_804D2FC0; }

// Empty in retail.
void LevelSetSky(char* name) {}

extern "C" void fn_8016E764(void) {}

void ActorGetMatrix(Mtx34* out, Actor* actor);

// A playing sound (0xA0 bytes); handles are (generation << 24) | slot index.
struct SoundSlot {
    int kind;         // 0x00: 0 free, 1 positional emitter
    int gen;          // 0x04
    int def;          // 0x08: index into the sound definitions (0x24 bytes each)
    int music;        // 0x0C
    u32 volume;       // 0x10
    int voice;        // 0x14
    SND_EMITTER emitter;   // 0x18
    u8 pad[0x88 - 0x18 - sizeof(SND_EMITTER)];
    Vec3 pos;         // 0x88
    Vec3 vel;         // 0x94: fed through fn_80175A90 (offset 0x98 as passed)
    u8 pad9c[0xA0 - 0xA0 + 0];
};

struct SoundDef {
    int volume;       // 0x00
    u8 pad4[0x14 - 4];
    int baseVolume;   // 0x14
    int emitterFlag;  // 0x18
    u8 pad1c[0x24 - 0x1C];
};

extern SoundSlot* lbl_804D4FBC;   // table of 256 slots
extern SoundDef* lbl_804D4F68;    // sound definitions
extern SND_LISTENER lbl_8046F798;
extern SND_FVECTOR lbl_803A810C;
extern SND_FVECTOR lbl_803A8118;
extern SND_FVECTOR lbl_803A8124;
extern SND_FVECTOR lbl_803A8130;
extern Mtx34 lbl_8046F828[4];
extern int lbl_804D4FB8;
extern int lbl_804D4F8C;

extern "C" void fn_80171F40(void* p);
extern "C" void fn_80175A90(Vec3* out, Vec3* a, Vec3* b);

static inline int SoundSlotIndex(int h) {
    int idx;

    if (h != -1) {
        idx = h & 0xFFFFFF;
        if ((int)((u32)h >> 24) == lbl_804D4FBC[idx].gen) {
        } else {
            idx = -1;
        }
    } else {
        idx = -1;
    }
    return idx;
}

// ListenerPush: remembers up to four camera matrices.
extern "C" int fn_8016ECB8(Actor* camera) {
    if (lbl_804D4FB8 < 4) {
        ActorGetMatrix(&lbl_8046F828[lbl_804D4FB8], camera);
        lbl_804D4FB8++;
    }
    return 1;
}

// ListenerSet: sets up the sound listener from the camera.
extern "C" int fn_8016ED08(Actor* camera) {
    fn_80171F40((char*)camera + 0x24);
    ActorGetMatrix(&lbl_8046F828[0], camera);
    lbl_804D4FB8 = 1;
    if (lbl_804D4F8C != 0) {
        sndUpdateListener(&lbl_8046F798, &lbl_803A810C, &lbl_803A8118, &lbl_803A8124, &lbl_803A8130, 0x7F, 0);
    } else {
        sndAddListener(&lbl_8046F798, &lbl_803A810C, &lbl_803A8118, &lbl_803A8124, &lbl_803A8130, 350.0f, 350.0f,
                       0.015f, 0, 0x7F, 0);
        lbl_804D4F8C = 1;
    }
    return 1;
}

extern "C" void fn_8016EDD4(void) {}

// SoundSetPosition: moves a playing sound; returns 1 if the handle was valid and the sound is not music.
extern "C" int fn_8016EDD8(int handle, Vec3* pos) {
    int idx = SoundSlotIndex(handle);

    if (idx > -1 && idx < 0x100) {
        SoundSlot* slot = &lbl_804D4FBC[idx];
        if (slot->kind != 0 && slot->music == 0) {
            Vec3 dir = {0.0f, 0.0f, 0.0f};
            Vec3 p;
            slot->pos = *pos;
            if (lbl_804D4FBC[idx].kind == 1) {
                fn_80175A90(&p, pos, &lbl_804D4FBC[idx].vel);
                sndUpdateEmitter(&lbl_804D4FBC[idx].emitter, (SND_FVECTOR*)&p, (SND_FVECTOR*)&dir,
                                 (u8)lbl_804D4FBC[idx].volume, 0);
            }
            return 1;
        }
    }
    return 0;
}

extern "C" int fn_8016F010(void) { return 0; }

// SoundSetVolume: volume is 12-bit fixed point (0x1000 = full).
extern "C" int fn_8016F018(int handle, int volume) {
    int idx = SoundSlotIndex(handle);

    if (idx > -1 && idx < 0x100 && lbl_804D4F68 != 0) {
        SoundSlot* slot = &lbl_804D4FBC[idx];
        SoundDef* def = &lbl_804D4F68[slot->def];
        if (slot->music == 0) {
            int v = def->baseVolume;
            if (v > 0x7F) {
                v = 0x7F;
            }
            slot->volume = (v * volume) / 0x1000;
        } else {
            int v = def->volume;
            if (v > 0x7F) {
                v = 0x7F;
            }
            slot->volume = (v * volume) / 0x1000;
            sndFXCtrl(lbl_804D4FBC[idx].voice, 7, (u8)lbl_804D4FBC[idx].volume);
        }
    }
    return 0;
}

extern "C" int fn_8016F118(void) { return 0; }

// Functions not drafted yet (symbol names are still the placeholders):
// TODO (not drafted yet, 0x2C bytes at 0x8016E578): fn_8016E578
// TODO (not drafted yet, 0x134 bytes at 0x8016E5A4): fn_8016E5A4
// TODO (not drafted yet, 0x1A8 bytes at 0x8016E768): fn_8016E768
// TODO (not drafted yet, 0x188 bytes at 0x8016E910): fn_8016E910
// TODO (not drafted yet, 0x220 bytes at 0x8016EA98): fn_8016EA98
// TODO (not drafted yet, 0x11C bytes at 0x8016EEF4): fn_8016EEF4
// TODO (not drafted yet, 0x13C bytes at 0x8016F120): fn_8016F120
// TODO (not drafted yet, 0x134 bytes at 0x8016F25C): fn_8016F25C
// TODO (not drafted yet, 0xEC bytes at 0x8016F390): FxStart
// TODO (not drafted yet, 0x60 bytes at 0x8016F47C): fn_8016F47C
// TODO (not drafted yet, 0x1C4 bytes at 0x8016F4DC): fn_8016F4DC
// TODO (not drafted yet, 0x178 bytes at 0x8016F6A0): fn_8016F6A0
// TODO (not drafted yet, 0xD4 bytes at 0x8016F818): fn_8016F818
// TODO (not drafted yet, 0x64 bytes at 0x8016F8EC): fn_8016F8EC
// TODO (not drafted yet, 0x864 bytes at 0x8016F950): fn_8016F950
// TODO (not drafted yet, 0x60 bytes at 0x801701B4): fn_801701B4
// TODO (not drafted yet, 0x134 bytes at 0x80170214): fn_80170214
// TODO (not drafted yet, 0x4EC bytes at 0x80170348): fn_80170348
// TODO (not drafted yet, 0x7C bytes at 0x80170834): FxFind__FPCc
// TODO (not drafted yet, 0xF4 bytes at 0x801708B0): LevelUnload__Fv
// TODO (not drafted yet, 0x100 bytes at 0x801709A4): LevelLoaded__FPCc
// TODO (not drafted yet, 0x70 bytes at 0x80170AA4): fn_80170AA4
// TODO (not drafted yet, 0x114 bytes at 0x80170B14): fn_80170B14
// TODO (not drafted yet, 0x8 bytes at 0x80170C28): fn_80170C28
// TODO (not drafted yet, 0x8C bytes at 0x80170C30): fn_80170C30
// TODO (not drafted yet, 0x8C bytes at 0x80170CBC): fn_80170CBC
// TODO (not drafted yet, 0x8C bytes at 0x80170D48): fn_80170D48
// TODO (not drafted yet, 0x8C bytes at 0x80170DD4): fn_80170DD4
// TODO (not drafted yet, 0x9C bytes at 0x80170E60): fn_80170E60
// TODO (not drafted yet, 0x68 bytes at 0x80170EFC): fn_80170EFC
// TODO (not drafted yet, 0x180 bytes at 0x80170F64): fn_80170F64
// TODO (not drafted yet, 0x16C bytes at 0x801710E4): fn_801710E4
// TODO (not drafted yet, 0x3C8 bytes at 0x80171250): fn_80171250
// TODO (not drafted yet, 0x3A4 bytes at 0x80171618): fn_80171618
// TODO (not drafted yet, 0x344 bytes at 0x801719BC): fn_801719BC
// TODO (not drafted yet, 0x240 bytes at 0x80171D00): fn_80171D00
// TODO (not drafted yet, 0x478 bytes at 0x80171F40): fn_80171F40
// TODO (not drafted yet, 0x2A8 bytes at 0x801723B8): fn_801723B8
// TODO (not drafted yet, 0x1278 bytes at 0x80172660): fn_80172660
// TODO (not drafted yet, 0x484 bytes at 0x801738D8): fn_801738D8
// TODO (not drafted yet, 0x74 bytes at 0x80173D5C): fn_80173D5C
// TODO (not drafted yet, 0x28 bytes at 0x80173DD0): fn_80173DD0
// TODO (not drafted yet, 0x28 bytes at 0x80173DF8): fn_80173DF8
// TODO (not drafted yet, 0x2CC bytes at 0x80173E20): fn_80173E20
// TODO (not drafted yet, 0x294 bytes at 0x801740EC): fn_801740EC
// TODO (not drafted yet, 0x524 bytes at 0x80174380): fn_80174380
// TODO (not drafted yet, 0x2CC bytes at 0x801748A4): fn_801748A4
// TODO (not drafted yet, 0x418 bytes at 0x80174B70): fn_80174B70
// TODO (not drafted yet, 0x2C bytes at 0x80174F88): fn_80174F88
// TODO (not drafted yet, 0x1C bytes at 0x80174FB4): fn_80174FB4
// TODO (not drafted yet, 0x30 bytes at 0x80174FD0): fn_80174FD0
// TODO (not drafted yet, 0x30 bytes at 0x80175000): fn_80175000
// TODO (not drafted yet, 0x72C bytes at 0x80175030): fn_80175030
// TODO (not drafted yet, 0x334 bytes at 0x8017575C): fn_8017575C
// TODO (not drafted yet, 0x2A8 bytes at 0x80175A90): fn_80175A90
// TODO (not drafted yet, 0x28 bytes at 0x80175D38): fn_80175D38

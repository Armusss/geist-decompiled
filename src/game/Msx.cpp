// Msx.cpp: 0x8016E20C-0x80175D60 (the Euler/box helpers before it are u_8016D170.cpp and u_8016D6BC.cpp):
// level load/unload and effect lookup (FxFind, LevelLoaded), the music/sound system glue (snd* calls), and
// debug text output.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/os.h>
// The game was built against the MusyX 2.0.3 headers its library comes from (u16 SND_PARAMETER.ctrl:
// retail stores the controller numbers with sth).
#define MUSY_VERSION_MAJOR 2
#define MUSY_VERSION_MINOR 0
#define MUSY_VERSION_PATCH 3
#include <musyx/musyx.h>
#include "game/types.h"

typedef short s16;

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
extern int lbl_804D2FCC;   // music volume (0..127)
extern int lbl_804D2FC8;   // effects volume (0..127)

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

extern "C" u32 fn_8016E5A4(void* b1, u32 n1, void* b2, u32 n2, u32 user);
extern "C" u32 fn_8016E578(void* b1, u32 n1, void* b2, u32 n2, u32 user);

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
                                        fn_8016E5A4, 0, (SND_ADPCMSTREAM_INFO*)(left + 1));
        lbl_804D5004 = sndStreamAllocEx(0xFF, lbl_804D500C, 0x1B90, *right, lbl_804D2FCC, 0x7F, 0, 0, 0, 0, 1,
                                        fn_8016E578, 0, (SND_ADPCMSTREAM_INFO*)(right + 1));
    }
}

// StreamAlloc: allocates the stream buffers.
extern "C" void fn_8016E4C8(void) { StreamAllocBuffers(); }

// fn_8016E578: records the buffer halves for the right stream.
extern "C" u32 fn_8016E578(void* b1, u32 n1, void* b2, u32 n2, u32 user) {
    lbl_804D5030 = (int)b1;
    lbl_804D5034 = (int)b2;
    lbl_804D5038 = n1;
    lbl_804D503C = n2;
    return lbl_804D5014 > n1 + n2 ? n1 + n2 : lbl_804D5014;
}

// fn_8016E5A4: decodes both halves of the left stream's buffer through the decoder callback.
extern "C" u32 fn_8016E5A4(void* b1, u32 n1, void* b2, u32 n2, u32 user) {
    int off;
    lbl_804D5018 = (int)b1;
    lbl_804D501C = (int)b2;
    lbl_804D5020 = n1;
    lbl_804D5024 = n2;
    lbl_804D5028 = 0;
    lbl_804D502C = 0;
    off = (char*)b1 - (char*)lbl_804D5008;
    if ((int)lbl_804D5010((char*)lbl_804D5008 + off, (char*)lbl_804D500C + off, (n1 * 8) / 14) == 0) {
        lbl_804D5014 = 0;
        lbl_804D5028 = 1;
        return 0;
    }
    if (n2 != 0) {
        off = (char*)b2 - (char*)lbl_804D5008;
        if ((int)lbl_804D5010((char*)lbl_804D5008 + off, (char*)lbl_804D500C + off, (n2 * 8) / 14) == 0) {
            lbl_804D502C = 1;
            n2 = 0;
            DebugPrintf("*StreamUpdate : split case failed\n");
        }
    }
    return lbl_804D5014 = n1 + n2;
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
    u8 pad[0x78 - 0x18 - sizeof(SND_EMITTER)];
    SND_PARAMETER_INFO params;   // 0x78: reverb send (0x5B) and filter (0x80) controllers
    void (*onStop)(void* arg);   // 0x80: called when the sound is stopped
    void* stopArg;               // 0x84
    Vec3 pos;         // 0x88
    Vec3 vel;         // 0x94: fed through fn_80175A90 (offset 0x98 as passed)
};

struct SoundDef {
    int volume;       // 0x00
    u8 pad4[0x14 - 4];
    int baseVolume;   // 0x14
    int reverb;       // 0x18: MIDI controller 0x5B (reverb send) value
    u8 pad1c[0x24 - 0x1C];
};

extern SoundSlot* lbl_804D4FBC;   // table of 256 slots
extern SoundDef* lbl_804D4F68;    // sound definitions
extern SND_LISTENER lbl_8046F798;
// Listener position, velocity, heading and up vectors: this file's own .data (retail addresses them from the
// unit's data base lbl_803A7CA8 + 0x464..0x488); symbols.txt lists them as separate objects.
extern SND_FVECTOR lbl_803A810C;
extern SND_FVECTOR lbl_803A8118;
extern SND_FVECTOR lbl_803A8124;
extern SND_FVECTOR lbl_803A8130;
extern Mtx34 gListenerMatrices[4];
extern int lbl_804D4FB8;
extern int lbl_804D4F8C;

extern "C" void fn_80171F40(void* p);
extern "C" void fn_80175A90(Vec3* out, Vec3* a, Vec3* b);

static inline int SoundSlotIndex(int h) {
    int idx;

    if (h != -1) {
        idx = h & 0xFFFFFF;
        if ((int)((u32)h >> 24) != lbl_804D4FBC[idx].gen) {
            return -1;
        }
        return idx;
    }
    return -1;
}

extern "C" bool sndFXAddFilterSetting2ParameterInfo(SND_PARAMETER_INFO* info, u8 filter, int param);
extern "C" bool sndFXSetFilter(SND_VOICEID vid, u8 filter, int param);

extern int lbl_804D2FE4;   // filter cutoff controller value (0x2000 = centre)
extern int lbl_804D2FE8;
extern int lbl_804D4FA4;   // filter enabled
extern int lbl_804D4FA8;
extern int lbl_804D2FD4;   // filter parameter
extern int lbl_804D2FDC;

// ---- Freeverb (Jezar at Dreampoint, public domain), built into this file: comb.hpp, allpass.hpp, revmodel.
// The tuning values and the class layouts are Freeverb's; this port's constructors also clear the members the
// original leaves alone, and its setters do not call update().

#define undenormalise(sample) if (((*(unsigned int*)&(sample)) & 0x7f800000) == 0) (sample) = 0.0f

const int numcombs = 8;
const int numallpasses = 4;
const float muted = 0;
const float fixedgain = 0.015f;
const float scalewet = 3;
const float scaledry = 2;
const float scaledamp = 0.4f;
const float scaleroom = 0.28f;
const float offsetroom = 0.7f;
const float initialroom = 0.5f;
const float initialdamp = 0.5f;
const float initialwet = 1 / scalewet;
const float initialdry = 0;
const float initialwidth = 1;
const float initialmode = 0;
const float freezemode = 0.5f;
const int stereospread = 23;

const int combtuningL1 = 1116;
const int combtuningR1 = 1116 + stereospread;
const int combtuningL2 = 1188;
const int combtuningR2 = 1188 + stereospread;
const int combtuningL3 = 1277;
const int combtuningR3 = 1277 + stereospread;
const int combtuningL4 = 1356;
const int combtuningR4 = 1356 + stereospread;
const int combtuningL5 = 1422;
const int combtuningR5 = 1422 + stereospread;
const int combtuningL6 = 1491;
const int combtuningR6 = 1491 + stereospread;
const int combtuningL7 = 1557;
const int combtuningR7 = 1557 + stereospread;
const int combtuningL8 = 1617;
const int combtuningR8 = 1617 + stereospread;
const int allpasstuningL1 = 556;
const int allpasstuningR1 = 556 + stereospread;
const int allpasstuningL2 = 441;
const int allpasstuningR2 = 441 + stereospread;
const int allpasstuningL3 = 341;
const int allpasstuningR3 = 341 + stereospread;
const int allpasstuningL4 = 225;
const int allpasstuningR4 = 225 + stereospread;

class comb {
public:
    comb();
    void setbuffer(float* buf, int size) {
        buffer = buf;
        bufsize = size;
    }
    inline float process(float inp);
    void mute() {
        for (int i = 0; i < bufsize; i++) {
            buffer[i] = 0;
        }
    }
    void setdamp(float val) {
        damp1 = val;
        damp2 = 1 - val;
    }
    void setfeedback(float val) { feedback = val; }

    float feedback;
    float filterstore;
    float damp1;
    float damp2;
    float* buffer;
    int bufsize;
    int bufidx;
};

inline float comb::process(float input) {
    float output;

    output = buffer[bufidx];
    undenormalise(output);
    filterstore = (output * damp2) + (filterstore * damp1);
    undenormalise(filterstore);
    buffer[bufidx] = input + (filterstore * feedback);
    if (++bufidx >= bufsize) {
        bufidx = 0;
    }
    return output;
}

class allpass {
public:
    allpass();
    void setbuffer(float* buf, int size) {
        buffer = buf;
        bufsize = size;
    }
    inline float process(float inp);
    void mute() {
        for (int i = 0; i < bufsize; i++) {
            buffer[i] = 0;
        }
    }
    void setfeedback(float val) { feedback = val; }

    float feedback;
    float* buffer;
    int bufsize;
    int bufidx;
};

inline float allpass::process(float input) {
    float output;
    float bufout;

    bufout = buffer[bufidx];
    undenormalise(bufout);
    output = -input + bufout;
    buffer[bufidx] = input + (bufout * feedback);
    if (++bufidx >= bufsize) {
        bufidx = 0;
    }
    return output;
}

class revmodel {
public:
    revmodel();
    void mute();
    void processreplace(float* inputL, float* inputR, float* outputL, float* outputR, long numsamples, int skip);
    void update();
    void setroomsize(float value) { roomsize = (value * scaleroom) + offsetroom; }
    void setdamp(float value) { damp = value * scaledamp; }
    void setwet(float value) { wet = value * scalewet; }
    void setdry(float value) { dry = value * scaledry; }
    void setwidth(float value) { width = value; }
    void setmode(float value) { mode = value; }
    float getmode() {
        if (mode >= freezemode) {
            return 1;
        } else {
            return 0;
        }
    }

    float gain;        // 0x00
    float roomsize, roomsize1;
    float damp, damp1;
    float wet, wet1, wet2;
    float dry;
    float width;
    float mode;        // 0x28
    comb combL[numcombs];          // 0x2C
    comb combR[numcombs];          // 0x10C
    allpass allpassL[numallpasses];   // 0x1EC
    allpass allpassR[numallpasses];   // 0x22C
    float bufcombL1[combtuningL1];    // 0x26C
    float bufcombR1[combtuningR1];
    float bufcombL2[combtuningL2];
    float bufcombR2[combtuningR2];
    float bufcombL3[combtuningL3];
    float bufcombR3[combtuningR3];
    float bufcombL4[combtuningL4];
    float bufcombR4[combtuningR4];
    float bufcombL5[combtuningL5];
    float bufcombR5[combtuningR5];
    float bufcombL6[combtuningL6];
    float bufcombR6[combtuningR6];
    float bufcombL7[combtuningL7];
    float bufcombR7[combtuningR7];
    float bufcombL8[combtuningL8];
    float bufcombR8[combtuningR8];
    float bufallpassL1[allpasstuningL1];
    float bufallpassR1[allpasstuningR1];
    float bufallpassL2[allpasstuningL2];
    float bufallpassR2[allpasstuningR2];
    float bufallpassL3[allpasstuningL3];
    float bufallpassR3[allpasstuningR3];
    float bufallpassL4[allpasstuningL4];
    float bufallpassR4[allpasstuningR4];
};

inline void revmodel::mute() {
    int i;

    if (getmode() >= freezemode) {
        return;
    }
    for (i = 0; i < numcombs; i++) {
        combL[i].mute();
        combR[i].mute();
    }
    for (i = 0; i < numallpasses; i++) {
        allpassL[i].mute();
        allpassR[i].mute();
    }
}

inline void revmodel::update() {
    int i;

    wet1 = wet * (width / 2 + 0.5f);
    wet2 = wet * ((1 - width) / 2);
    if (mode >= freezemode) {
        roomsize1 = 1;
        damp1 = 0;
        gain = muted;
    } else {
        roomsize1 = roomsize;
        damp1 = damp;
        gain = fixedgain;
    }
    for (i = 0; i < numcombs; i++) {
        combL[i].setfeedback(roomsize1);
        combR[i].setfeedback(roomsize1);
    }
    for (i = 0; i < numcombs; i++) {
        combL[i].setdamp(damp1);
        combR[i].setdamp(damp1);
    }
}

// A reverb preset (Freeverb parameters).
struct ReverbPreset {
    float room;
    float damp;
    float wet;
    float dry;
    float width;
};

extern revmodel lbl_8046F8FC;              // the reverb (defined at the end of the file)
extern ReverbPreset lbl_8046F8E8;          // current preset
extern ReverbPreset lbl_803A7CEC[3][17];   // presets per output mode
extern int lbl_804D4FC8;                   // current preset index
extern int lbl_804D4FCC;                   // current output mode

// SoundSlotSetParams: fills a positional sound's controller list (reverb send, filter value); true when the
// filter is on for this sound.
static inline int SoundSlotSetParams(SoundSlot* slot) {
    SND_PARAMETER_INFO* info = &slot->params;
    SoundDef* def = &lbl_804D4F68[slot->def];

    info->numPara = 2;
    info->paraArray[0].ctrl = 0x5B;
    info->paraArray[0].paraData.value7 = def->reverb;
    info->paraArray[1].ctrl = 0x80;
    info->paraArray[1].paraData.value14 = lbl_804D2FE4;
    if (lbl_804D4FA4 != 0 && def->reverb > 0) {
        sndFXAddFilterSetting2ParameterInfo(info, 1, lbl_804D2FD4);
        return 1;
    }
    return 0;
}

// SoundSetFilterLevel: level in [-1, 1] moves the filter controller around 0x2000 for every playing sound.
extern "C" void fn_8016E768(float level, int on) {
    int value;
    int off;
    int i;

    if (level > 1.0f) {
        level = 1.0f;
    } else if (level < -1.0f) {
        level = -1.0f;
    }
    if (on != 0) {
        value = (int)(8191.0f * level) + 0x2000;
    } else {
        value = 0x2000;
    }
    if (lbl_804D2FE4 != value) {
        lbl_804D2FE4 = value;
        lbl_804D2FE8 = value;
        for (i = 0, off = 0; i < 0x100; i++, off += sizeof(SoundSlot)) {
            SoundSlot* slot = (SoundSlot*)((char*)lbl_804D4FBC + off);
            if (slot->kind != 0) {
                if (slot->music == 0) {
                    if (SoundSlotSetParams(slot)) {
                        SND_VOICEID vid = sndEmitterVoiceID(&((SoundSlot*)((char*)lbl_804D4FBC + off))->emitter);
                        if (vid != -1) {
                            sndFXCtrl14(vid, 0x80, lbl_804D2FE4);
                        }
                    }
                } else if (lbl_804D4F68[slot->def].reverb > 0) {
                    sndFXCtrl14(slot->voice, 0x80, lbl_804D2FE4);
                }
            }
        }
    }
}

// SoundSetFilter: turns the filter on or off with a parameter for every playing sound.
extern "C" void fn_8016E910(int on, int param) {
    int enable;
    int off;
    int i;

    if (lbl_804D4FA4 != on || lbl_804D2FD4 != param) {
        lbl_804D4FA4 = on;
        lbl_804D4FA8 = on;
        enable = on != 0;
        lbl_804D2FD4 = param;
        lbl_804D2FDC = param;
        for (i = 0, off = 0; i < 0x100; i++, off += sizeof(SoundSlot)) {
            SoundSlot* slot = (SoundSlot*)((char*)lbl_804D4FBC + off);
            if (slot->kind != 0) {
                if (slot->music == 0) {
                    if (SoundSlotSetParams(slot)) {
                        SND_VOICEID vid = sndEmitterVoiceID(&((SoundSlot*)((char*)lbl_804D4FBC + off))->emitter);
                        if (vid != -1) {
                            sndFXSetFilter(vid, enable, param);
                        }
                    }
                } else if (lbl_804D4F68[slot->def].reverb > 0) {
                    sndFXSetFilter(slot->voice, enable, param);
                }
            }
        }
    }
}

// ReverbSetMode: re-applies the current preset for output mode `mode` (0 mono, 1 stereo, 2 surround).
extern "C" void fn_8016EA98(int mode) {
    if (mode > -1 && mode < 3) {
        int preset = lbl_804D4FC8;

        lbl_804D4FCC = mode;
        lbl_804D4FC8 = -1;
        if (preset != -1 && preset >= 0 && (u32)preset < 17) {
            ReverbPreset* p = &lbl_803A7CEC[mode][preset];

            lbl_804D4FC8 = preset;
            lbl_8046F8E8 = *p;
            lbl_8046F8FC.setroomsize(p->room);
            lbl_8046F8FC.setdamp(p->damp);
            lbl_8046F8FC.setwet(p->wet);
            lbl_8046F8FC.setdry(p->dry);
            lbl_8046F8FC.setwidth(p->width);
            lbl_8046F8FC.update();
        }
    }
}

// ListenerPush: remembers up to four camera matrices.
extern "C" int fn_8016ECB8(Actor* camera) {
    if (lbl_804D4FB8 < 4) {
        ActorGetMatrix(&gListenerMatrices[lbl_804D4FB8], camera);
        lbl_804D4FB8++;
    }
    return 1;
}

// ListenerSet: sets up the sound listener from the camera.
extern "C" int fn_8016ED08(Actor* camera) {
    fn_80171F40((char*)camera + 0x24);
    ActorGetMatrix(&gListenerMatrices[0], camera);
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

extern char** lbl_804D4F3C;   // sound names, by definition index
extern int lbl_804D4F70;      // sound debug output on

// SoundStop: stops a playing sound and runs its stop callback; returns 1 if the handle was valid.
extern "C" int fn_8016EEF4(int handle) {
    int idx = SoundSlotIndex(handle);

    if (idx > -1 && idx < 0x100) {
        if (lbl_804D4F70 != 0) {
            DebugPrintf("SndStop %d (%s)\n", handle, lbl_804D4F3C[lbl_804D4FBC[idx].def]);
        }
        if (lbl_804D4FBC[idx].kind == 1) {
            if (lbl_804D4FBC[idx].music == 0) {
                sndRemoveEmitter(&lbl_804D4FBC[idx].emitter);
            } else {
                sndFXKeyOff(lbl_804D4FBC[idx].voice);
            }
        }
        lbl_804D4FBC[idx].kind = 0;
        if (lbl_804D4FBC[idx].onStop != 0) {
            lbl_804D4FBC[idx].onStop(lbl_804D4FBC[idx].stopArg);
        }
        return 1;
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

// SoundGetKind: 0 when the handle is stale or the slot is free.
extern "C" int fn_8016F47C(int handle) {
    int idx = SoundSlotIndex(handle);

    if (idx >= 0 && idx < 0x100) {
        return lbl_804D4FBC[idx].kind;
    }
    return 0;
}

extern "C" void fn_801723B8(void);
void ScreenPrintf(const char* fmt, ...);
extern int lbl_804D4F80;   // show the playing-sounds overlay
extern int lbl_804D4F84;   // show the reverb overlay
extern char* lbl_803A7CA8[17];   // reverb preset names

// SoundDebugDraw
extern "C" void fn_8016F8EC(void) {
    if (lbl_804D4F80 != 0) {
        fn_801723B8();
    }
    if (lbl_804D4F84 != 0) {
        ScreenPrintf("Reverb %s (%d)\n", lbl_803A7CA8[lbl_804D4FC8], lbl_804D4FCC);
    }
}

extern "C" int stricmp(const char* a, const char* b);
extern int lbl_804D4F4C;   // number of sound definitions

// FxFind: sound definition index by name, tagged with the top bit; -1 if not found.
int FxFind(const char* name) {
    int i;

    for (i = 0; i < lbl_804D4F4C; i++) {
        if (stricmp(name, lbl_804D4F3C[i]) == 0) {
            return i | 0x80000000;
        }
    }
    return -1;
}

extern int lbl_804D4FB4;   // the loader thread is running
extern OSThread lbl_8046E480;
void GfxFrameSwap(void);
void GfxFrameBegin(void);

// SoundLoadWait: waits for the loader thread, keeping the screen alive.
extern "C" void fn_80170AA4(void) {
    void* result;

    if (lbl_804D4FB4 != 0) {
        while (OSIsThreadTerminated(&lbl_8046E480) == 0) {
            OSYieldThread();
            GfxFrameSwap();
            GfxFrameBegin();
        }
        OSJoinThread(&lbl_8046E480, &result);
        lbl_804D4FB4 = 0;
    }
}

extern "C" double modf(double x, double* ip);

// RoundToInt: rounds half up through modf.
static inline float RoundF(float v) {
    double ip;

    if ((float)modf(v, &ip) >= 0.5f) {
        ip += 1.0;
    }
    return ip;
}

// MusicVolumeGet / EffectsVolumeGet (percent)
extern "C" int fn_80170C30(void) { return RoundF(100.0f * (lbl_804D2FCC / 127.0f)); }

extern "C" int fn_80170CBC(void) { return RoundF(100.0f * (lbl_804D2FC8 / 127.0f)); }

// EffectsVolumeSet / MusicVolumeSet (percent)
extern "C" void fn_80170D48(int percent) { lbl_804D2FC8 = RoundF((127.0f * percent) / 100.0f); }

extern "C" void fn_80170DD4(int percent) { lbl_804D2FCC = RoundF((127.0f * percent) / 100.0f); }

// A loaded song (0xC bytes).
struct Song {
    int id;          // 0x00
    SND_SEQID seq;   // 0x04: -1 when not playing
    int pad8;
};

extern Song* lbl_804D4F6C;   // songs
extern int lbl_804D4F54;     // number of songs
extern int lbl_804D3000;     // current song (-1 none)
extern int lbl_804D4FFC;     // music pause depth

// MusicPause: nested pause/resume of the current song.
extern "C" void fn_80170E60(int pause) {
    if (pause != 0) {
        if (lbl_804D4FFC == 0 && lbl_804D3000 != -1) {
            sndSeqPause(lbl_804D4F6C[lbl_804D3000].seq);
        }
        lbl_804D4FFC++;
    } else {
        if (--lbl_804D4FFC < 1) {
            if (lbl_804D3000 != -1) {
                sndSeqContinue(lbl_804D4F6C[lbl_804D3000].seq);
            }
            lbl_804D4FFC = 0;
        }
    }
}

// SongStop
extern "C" void fn_80170EFC(int song) {
    if (song > -1 && song < lbl_804D4F54) {
        if (lbl_804D4F6C[song].seq != -1) {
            sndSeqStop(lbl_804D4F6C[song].seq);
            lbl_804D4F6C[song].seq = -1;
        }
    }
}

int FileReadSync(void* file, void* dst, int size, int offset);
extern "C" int fn_8000786C(void* file, void* dst, int size, int offset);
extern u8 lbl_80488F64[0x3C];   // the sound data file
extern void* lbl_804D4F30;      // sound data read buffer
extern int lbl_804D4F34;        // bytes read so far

// SoundDataRead: reads the next chunk of the sound data file (async while the loader thread runs).
extern "C" void* fn_80173D5C(int offset, int size) {
    if (lbl_804D4FB4 == 0) {
        FileReadSync(lbl_80488F64, lbl_804D4F30, size, offset);
    } else {
        fn_8000786C(lbl_80488F64, lbl_804D4F30, size, offset);
    }
    lbl_804D4F34 += size;
    return lbl_804D4F30;
}

// MemFreeCallback / MemAllocCallback: the memory hooks handed out through the hook table in .sdata2.
extern "C" void fn_80174FD0(void* p) { MemFree(p, "Msx.cpp", 0x41A); }

extern "C" void fn_80175000(int size) { MemAlloc(size, "Msx.cpp", 0x413); }

// Functions not drafted yet (symbol names are still the placeholders):
// TODO (not drafted yet, 0x2C bytes at 0x8016E578): fn_8016E578
// TODO (not drafted yet, 0x134 bytes at 0x8016E5A4): fn_8016E5A4
// TODO (not drafted yet, 0x13C bytes at 0x8016F120): fn_8016F120
// TODO (not drafted yet, 0x134 bytes at 0x8016F25C): fn_8016F25C
// TODO (not drafted yet, 0xEC bytes at 0x8016F390): FxStart
// TODO (not drafted yet, 0x1C4 bytes at 0x8016F4DC): fn_8016F4DC
// TODO (not drafted yet, 0x178 bytes at 0x8016F6A0): fn_8016F6A0
// TODO (not drafted yet, 0xD4 bytes at 0x8016F818): fn_8016F818
// TODO (not drafted yet, 0x864 bytes at 0x8016F950): fn_8016F950
// TODO (not drafted yet, 0x134 bytes at 0x80170214): fn_80170214
// TODO (not drafted yet, 0x4EC bytes at 0x80170348): fn_80170348
// TODO (not drafted yet, 0xF4 bytes at 0x801708B0): LevelUnload__Fv
// TODO (not drafted yet, 0x100 bytes at 0x801709A4): LevelLoaded__FPCc
// TODO (not drafted yet, 0x114 bytes at 0x80170B14): fn_80170B14
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
// TODO (not drafted yet, 0x2CC bytes at 0x80173E20): fn_80173E20
// TODO (not drafted yet, 0x294 bytes at 0x801740EC): fn_801740EC
extern float lbl_80488910[2][SND_AUX_BLOCKSIZE];   // reverb work buffers (left, right)

extern int lbl_804D4F38;   // sound group stack depth
extern int lbl_804D4F94;   // ARAM used by the pushed groups
extern int lbl_804D4F98;   // ARAM size of the top group

// SoundPopGroup
extern "C" void fn_801701B4(void) {
    if (lbl_804D4F38 > 0) {
        sndPopGroup();
        lbl_804D4F94 -= lbl_804D4F98;
        lbl_804D4F38--;
    } else {
        DebugPrintf("Tried to pop a group, but the sound stack is empty\n");
    }
}

extern "C" int fn_80170C28(void) { return 1; }

extern "C" int fn_8016F4DC(int pause, int group);

// SoundResumeGroup / SoundPauseGroup
extern "C" void fn_80173DD0(int group) { fn_8016F4DC(0, group); }

extern "C" void fn_80173DF8(int group) { fn_8016F4DC(1, group); }

// ReverbAuxCallback: runs one MusyX aux block through the Freeverb model (user is the revmodel).
extern "C" void fn_80174380(u8 reason, SND_AUX_INFO* info, void* user) {
    int i;

    if (reason == SND_AUX_REASON_BUFFERUPDATE) {
        for (i = 0; i < SND_AUX_BLOCKSIZE; i++) {
            lbl_80488910[0][i] = info->data.bufferUpdate.left[i] / 16777215.0;
            lbl_80488910[1][i] = info->data.bufferUpdate.right[i] / 16777215.0;
        }
        ((revmodel*)user)->processreplace(lbl_80488910[0], lbl_80488910[1], lbl_80488910[0], lbl_80488910[1],
                                          SND_AUX_BLOCKSIZE, 1);
        for (i = 0; i < SND_AUX_BLOCKSIZE; i++) {
            info->data.bufferUpdate.left[i] = (s32)(16777215.0 * lbl_80488910[0][i]);
            info->data.bufferUpdate.right[i] = (s32)(16777215.0 * lbl_80488910[1][i]);
        }
    }
}

void revmodel::processreplace(float* inputL, float* inputR, float* outputL, float* outputR, long numsamples,
                              int skip) {
    float outL, outR, input;
    int i;

    while (numsamples-- > 0) {
        outL = outR = 0;
        input = (*inputL + *inputR) * gain;
        // Accumulate comb filters in parallel
        for (i = 0; i < numcombs; i++) {
            outL += combL[i].process(input);
            outR += combR[i].process(input);
        }
        // Feed through allpasses in series
        for (i = 0; i < numallpasses; i++) {
            outL = allpassL[i].process(outL);
            outR = allpassR[i].process(outR);
        }
        // Calculate output REPLACING anything already there
        *outputL = outL * wet1 + outR * wet2 + *inputL * dry;
        *outputR = outR * wet1 + outL * wet2 + *inputR * dry;
        // Increment sample pointers, allowing for interleave (if any)
        inputL += skip;
        inputR += skip;
        outputL += skip;
        outputR += skip;
    }
}

revmodel::revmodel() {
    // Tie the components to their buffers
    combL[0].setbuffer(bufcombL1, combtuningL1);
    combR[0].setbuffer(bufcombR1, combtuningR1);
    combL[1].setbuffer(bufcombL2, combtuningL2);
    combR[1].setbuffer(bufcombR2, combtuningR2);
    combL[2].setbuffer(bufcombL3, combtuningL3);
    combR[2].setbuffer(bufcombR3, combtuningR3);
    combL[3].setbuffer(bufcombL4, combtuningL4);
    combR[3].setbuffer(bufcombR4, combtuningR4);
    combL[4].setbuffer(bufcombL5, combtuningL5);
    combR[4].setbuffer(bufcombR5, combtuningR5);
    combL[5].setbuffer(bufcombL6, combtuningL6);
    combR[5].setbuffer(bufcombR6, combtuningR6);
    combL[6].setbuffer(bufcombL7, combtuningL7);
    combR[6].setbuffer(bufcombR7, combtuningR7);
    combL[7].setbuffer(bufcombL8, combtuningL8);
    combR[7].setbuffer(bufcombR8, combtuningR8);
    allpassL[0].setbuffer(bufallpassL1, allpasstuningL1);
    allpassR[0].setbuffer(bufallpassR1, allpasstuningR1);
    allpassL[1].setbuffer(bufallpassL2, allpasstuningL2);
    allpassR[1].setbuffer(bufallpassR2, allpasstuningR2);
    allpassL[2].setbuffer(bufallpassL3, allpasstuningL3);
    allpassR[2].setbuffer(bufallpassR3, allpasstuningR3);
    allpassL[3].setbuffer(bufallpassL4, allpasstuningL4);
    allpassR[3].setbuffer(bufallpassR4, allpasstuningR4);

    // Set default values
    allpassL[0].setfeedback(0.5f);
    allpassR[0].setfeedback(0.5f);
    allpassL[1].setfeedback(0.5f);
    allpassR[1].setfeedback(0.5f);
    allpassL[2].setfeedback(0.5f);
    allpassR[2].setfeedback(0.5f);
    allpassL[3].setfeedback(0.5f);
    allpassR[3].setfeedback(0.5f);
    setwet(initialwet);
    setroomsize(initialroom);
    setdry(initialdry);
    setdamp(initialdamp);
    setwidth(initialwidth);
    setmode(initialmode);

    // Buffer will be full of rubbish - so we MUST mute them
    mute();
}

comb::comb() {
    filterstore = 0;
    bufidx = 0;
    feedback = 0;
    damp1 = 0;
    damp2 = 1;
    buffer = 0;
    bufsize = 0;
}

allpass::allpass() {
    bufidx = 0;
    feedback = 0;
    buffer = 0;
    bufsize = 0;
}

// TODO (not drafted yet, 0x72C bytes at 0x80175030): fn_80175030
// TODO (not drafted yet, 0x334 bytes at 0x8017575C): fn_8017575C
// TODO (not drafted yet, 0x2A8 bytes at 0x80175A90): fn_80175A90
revmodel lbl_8046F8FC;   // its constructor runs from __sinit_Msx_cpp

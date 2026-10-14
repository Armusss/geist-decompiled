// Audio.c: sound sets, volume fades and music control (0x8007CC04-0x8007DAC8).
// - A sound set is three positional sound slots owned by one object; sets are allocated per entry of
//   the level's sound table (loaded through a file handler) and driven by messages 0x2400-0x241B.
// - Two tables of 16 fades ramp a sound's volume (or a second parameter) toward a target over time.
// - The audio object (id 0x23) handles effect/music messages 0x2400-0x241A and fades the music volume.
// Compiled as C++ like the rest of the game.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

extern "C" {
// Callees in other files, not named yet.
void fn_8016EEF4(int handle);                // stop a sound
void fn_8016EDD8(int handle, int* pos);      // move a sound
int fn_8016F120(int id, int* pos);           // start a positional sound
void fn_8016F018(int handle, int volume);
void fn_8016F010(int handle, int value);
int fn_80084540(int to, int msg, void* data, int* result);
int fn_800C5210(int to, int msg, void* data);
void fn_80026680(int type, void* func, int arg);
void fn_80026660(int type, void* func, int arg);
void fn_800070E0(void* data, int size);
int fn_80171250(int a);
void fn_801710E4(int a, int b);
void fn_80170EFC(int a);
void fn_80170F64(int a, int b, int c, int d);
void fn_80170214(int a);
void fn_80170E60(int a);
void fn_801701B4(void);
void fn_8016F4DC(int a, int b);
void fn_8016EA98(int a);
void fn_8016E910(int a, int b);
void fn_8016E768(int a, float b);
}

int SendMsg(int to, int msg, void* data);
void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);
void FileFlushBuffer(void* p, int size);
int FxStart(void* fx, int a, int b);
int MusicPlay(const char* name, void (*done)(void*), void* user);
int MusicFree(int id);
void MusicReset(void);
void MusicHold(void);
void MusicStop(void);
void MusicResume(void);
void MusicPause(void);
void MusicSetMode(int mode);
void MusicSetVolume(float left, float right);
void MusicGetVolume(float* left, float* right);
void MusicInit(void);
void MusicShutdown(void);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct Vec3 {
    float x, y, z;
};

struct SoundSlot {
    int id;
    int handle;
    int active;
    int unkC;
};

struct SoundSet {
    SoundSlot slots[3];
};

struct SoundDef {
    Vec3 pos;
    u8 padC[0x10];
};

struct Fade {
    int handle;
    int value;
    int rate;
    int target;
};

struct SoundMsg {
    int unk0;
    int id;
    int slot;
    int volume;
};

struct AudioMsg {
    int unk0;
    void* a;
    void* b;
    void* c;
    void* d;
};

struct NotifyMsg {
    int unk0;
    int arg;
};

struct SetMsg {
    int unk0;
    SoundSet* set;
};

struct SoundFile {
    s16 type;
    s16 count;
    u8 pad4[0xC];
    SoundDef defs[1];
};

static Fade sFadesA[16];
static Fade sFadesB[16];

SoundDef* sSoundDefs;
SoundSet* sSoundSets;
int sSoundSetCount;
static int sFadeCountA;
static int sFadeCountB;
static int sMusicFading;
static float sMusicFrom;
static float sMusicTo;
static float sMusicStep;
static int sMusicTarget;
static void (*sMusicDone)(void* user);
static void* sMusicUser;

void SoundSetStopAll(SoundSet* set) {
    int i;

    for (i = 0; i < 3; i++) {
        if (set->slots[i].handle != -1) {
            fn_8016EEF4(set->slots[i].handle);
        }
    }
    for (i = 0; i < 3; i++) {
        set->slots[i].id = -1;
        set->slots[i].handle = -1;
        set->slots[i].active = 0;
    }
}

void SoundSetStop(SoundSet* set, int i) {
    if (i >= 0 && i < 3) {
        SoundSlot* s = &set->slots[i];
        if (s->handle != -1) {
            fn_8016EEF4(s->handle);
        }
        s->id = -1;
        s->handle = -1;
        s->active = 0;
    }
}

void SoundSetMove(SoundSet* set, Vec3* pos) {
    int i;

    for (i = 0; i < 3; i++) {
        int p[3];
        p[0] = pos->x;
        p[1] = pos->y;
        p[2] = pos->z;
        if (set->slots[i].handle != -1 && set->slots[i].active) {
            fn_8016EDD8(set->slots[i].handle, p);
        }
    }
}

void SoundSetVolume(SoundSet* set, int i, int volume) {
    if (volume > 127) {
        volume = 127;
    }
    volume = (volume << 12) / 127;
    if (i >= 0 && i < 3) {
        SoundSlot* s = &set->slots[i];
        if (s->handle != -1) {
            fn_8016F018(s->handle, volume);
        }
    }
}

void SoundSetPlay(SoundSet* set, int i, int id, Vec3* pos) {
    if (i >= 0 && i < 3) {
        SoundSlot* s = &set->slots[i];
        int p[3];
        p[0] = pos->x;
        p[1] = pos->y;
        p[2] = pos->z;
        s->id = id;
        s->handle = fn_8016F120(id, p);
        s->active = 1;
    }
}

void Fn8007CEA4(void* p) {
    SoundSet* set = (SoundSet*)p;
    int i;

    for (i = 0; i < 3; i++) {
        set->slots[i].id = -1;
        set->slots[i].handle = -1;
        set->slots[i].active = 0;
    }
}

void MusicNotify(void* user) {
    NotifyMsg m;
    m.arg = (int)user;
    SendMsg(sMusicTarget, 0x240E, &m);
}

void AudioNop(void) {}

void AudioUpdate(float dt) {
    int i;

    if (sFadeCountA) {
        for (i = 0; i < 16; i++) {
            if (sFadesA[i].handle != -1) {
                int done = 0;
                if (sFadesA[i].target < sFadesA[i].value) {
                    sFadesA[i].value = sFadesA[i].value - (int)(dt * sFadesA[i].rate / 1000.0f);
                    if (sFadesA[i].value < sFadesA[i].target) {
                        sFadesA[i].value = sFadesA[i].target;
                        done = 1;
                    }
                } else {
                    sFadesA[i].value = sFadesA[i].value + (int)(dt * sFadesA[i].rate / 1000.0f);
                    if (sFadesA[i].value > sFadesA[i].target) {
                        sFadesA[i].value = sFadesA[i].target;
                        done = 1;
                    }
                }
                fn_8016F010(sFadesA[i].handle, sFadesA[i].value);
                if (done) {
                    sFadesA[i].handle = -1;
                    sFadeCountA--;
                }
            }
        }
    }
    if (sFadeCountB) {
        for (i = 0; i < 16; i++) {
            if (sFadesB[i].handle != -1) {
                int done = 0;
                if (sFadesB[i].target < sFadesB[i].value) {
                    sFadesB[i].value = sFadesB[i].value - (int)(dt * sFadesB[i].rate / 1000.0f);
                    if (sFadesB[i].value < sFadesB[i].target) {
                        sFadesB[i].value = sFadesB[i].target;
                        done = 1;
                    }
                } else {
                    sFadesB[i].value = sFadesB[i].value + (int)(dt * sFadesB[i].rate / 1000.0f);
                    if (sFadesB[i].value > sFadesB[i].target) {
                        sFadesB[i].value = sFadesB[i].target;
                        done = 1;
                    }
                }
                fn_8016F018(sFadesB[i].handle, sFadesB[i].value);
                if (done) {
                    sFadesB[i].handle = -1;
                    sFadeCountB--;
                }
            }
        }
    }
    if (sMusicFading) {
        float vol;
        MusicGetVolume(&vol, 0);
        vol = sMusicStep * dt + vol;
        if (sMusicTo > sMusicFrom) {
            if (vol >= sMusicTo) {
                sMusicFading = 0;
                vol = sMusicTo;
                if (sMusicDone) {
                    sMusicDone(sMusicUser);
                }
            }
        } else {
            if (vol <= sMusicTo) {
                sMusicFading = 0;
                vol = sMusicTo;
                if (sMusicDone) {
                    sMusicDone(sMusicUser);
                }
            }
        }
        MusicSetVolume(vol, vol);
    }
}

void AudioShutdown(void) {
    if (sSoundSets) {
        MemFree(sSoundSets, "Audio.c", 520);
    }
    MusicShutdown();
}

void SoundSetsAlloc(void) {
    int i;

    if (sSoundSetCount) {
        sSoundSets = (SoundSet*)MemAlloc(sSoundSetCount * sizeof(SoundSet), "Audio.c", 498);
        for (i = 0; i < sSoundSetCount; i++) {
            SetMsg m;
            Fn8007CEA4(&sSoundSets[i]);
            m.set = &sSoundSets[i];
            SendMsg((i & 0xFFFFFF) | 0x0F000000, 0x800, &m);
        }
    }
}

int AudioMsgHandler(int id, int msg, AudioMsg* data);
int SoundSetMsg(int id, int msg, SoundMsg* data);
void SoundFileLoad(int arg, SoundFile* file);

int AudioInit(void) {
    int i;

    sSoundSetCount = 0;
    sSoundDefs = 0;
    sSoundSets = 0;
    fn_80026680(0xF, (void*)SoundFileLoad, 0);
    fn_80026660(0x23, (void*)AudioMsgHandler, 0);
    fn_80026660(0xF, (void*)SoundSetMsg, 0);
    sFadeCountA = 0;
    sFadeCountB = 0;
    for (i = 0; i < 16; i++) {
        sFadesA[i].handle = -1;
        sFadesB[i].handle = -1;
    }
    MusicInit();
    return 1;
}

int AudioMsgHandler(int id, int msg, AudioMsg* data) {
    if (!fn_80084540((id & 0xFFFFFF) | 0x23000000, msg, data, 0)) {
        switch (msg) {
        case 0x2400:
            if (data->c) {
                data->d = (void*)fn_8016F120((int)data->a, (int*)data->c);
            } else {
                data->d = (void*)FxStart(data->a, 0, 0);
            }
            return 1;
        case 0x2401:
            fn_8016EEF4((int)data->a);
            return 1;
        case 0x2402:
            fn_8016EDD8((int)data->a, (int*)data->b);
            return 1;
        case 0x2403:
            sMusicTarget = (int)data->b;
            return MusicPlay((const char*)data->a, MusicNotify, data->c);
        case 0x2404:
            return MusicFree((int)data->a);
        case 0x2405:
            MusicReset();
            return 1;
        case 0x2406:
            MusicHold();
            return 1;
        case 0x2407:
            MusicStop();
            return 1;
        case 0x2408:
            MusicResume();
            return 1;
        case 0x2409:
            MusicPause();
            return 1;
        case 0x240B:
            MusicSetMode(1);
            return 1;
        case 0x240C:
            MusicSetMode(2);
            return 1;
        case 0x240D:
            MusicSetMode(0);
            return 1;
        case 0x240A: {
            void (*done)(void*);
            float vol;
            float time;
            void* user;

            sMusicTarget = (int)data->c;
            done = sMusicDone;
            time = *(float*)&data->b;
            user = data->d;
            vol = *(float*)&data->a;
            if (time <= 0.0f) {
                MusicSetVolume(vol, vol);
            } else {
                sMusicFading = 1;
                MusicGetVolume(&sMusicFrom, 0);
                sMusicTo = MIN(1.0f, MAX(0.0f, vol));
                sMusicStep = (sMusicTo - sMusicFrom) / time;
                sMusicDone = done;
                sMusicUser = user;
            }
            return 1;
        }
        case 0x2410:
            data->b = (void*)fn_80171250((int)data->a);
            break;
        case 0x2411:
            fn_801710E4((int)data->a, (int)data->b);
            break;
        case 0x2412:
            fn_80170EFC((int)data->a);
            break;
        case 0x2419:
            fn_80170E60((int)data->a);
            break;
        case 0x2413:
            fn_80170F64((int)data->a, (int)data->b, (int)data->c, (int)data->d);
            break;
        case 0x2414:
            fn_80170214((int)data->a);
            break;
        case 0x241A:
            fn_801701B4();
            break;
        case 0x2415:
            fn_8016F4DC((int)data->a, (int)data->b);
            break;
        case 0x2416:
            fn_8016EA98((int)data->a);
            break;
        case 0x2417:
            fn_8016E910((int)data->a, (int)data->b);
            break;
        case 0x2418:
            fn_8016E768((int)data->a, *(float*)&data->b);
            break;
        case 0x3005: {
            NotifyMsg m;
            m.arg = (int)data->a;
            SendMsg(sMusicTarget, 0x240E, &m);
            return 1;
        }
        }
    }
    return 0;
}

int SoundSetMsg(int id, int msg, SoundMsg* data) {
    int to = (id & 0xFFFFFF) | 0x0F000000;
    int result = 0;

    if (!fn_80084540(to, msg, data, &result) && !fn_800C5210(to, msg, data)) {
        switch (msg) {
        case 0x2400:
            if (id >= 0 && id < sSoundSetCount) {
                SoundSetStop(&sSoundSets[id], data->slot);
                SoundSetPlay(&sSoundSets[id], data->slot, data->id, &sSoundDefs[id].pos);
            }
            return 1;
        case 0x2401:
            if (id >= 0 && id < sSoundSetCount) {
                SoundSetStop(&sSoundSets[id], data->slot);
            }
            return 1;
        case 0x241B:
            if (id >= 0 && id < sSoundSetCount) {
                SoundSetVolume(&sSoundSets[id], data->slot, data->volume);
            }
            return 1;
        }
    }
    return result;
}

void SoundFileLoad(int arg, SoundFile* file) {
    fn_800070E0(file, 0x10);
    if (file->type == 4) {
        sSoundDefs = file->defs;
        FileFlushBuffer(sSoundDefs, (sSoundSetCount = file->count) * sizeof(SoundDef));
    }
}

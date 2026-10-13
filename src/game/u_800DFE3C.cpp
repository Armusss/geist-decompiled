// Music streams (0x800DFE3C-0x800E0554): plays "Audio/<name>.adp" streams through a stream player in
// another file, with a left/right volume, a play state and 16 stream slots kept on a free list.
// Stream handles are the bitwise complement of the slot pointer.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

extern "C" {
void* memset(void* dst, int val, u32 n);
int sprintf(char* s, const char* format, ...);

// Stream player (another file, not named yet).
void fn_800F6CFC(int left, int right);
void fn_800F6F34(int mode);
u32 fn_800F6D4C(void);
void fn_800F6D54(int state);
void fn_800F6F3C(int arg);
u32 fn_800F6FFC(void* slot);
u32 fn_800F7130(const char* path, void* slot, int arg, void (*done)(void));
void fn_800F7350(void);
struct MusicSlot* fn_800F6D44(void);
}

void LevelLoadNames(const char* path, int* drive, char* dir, char* name, char* ext);

struct MusicSlot {
    u8 pad[0x50];
    char path[0x40];
    void (*done)(void* arg);
    MusicSlot* next;
};

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

float sMusicVolumeL = 1.0f;
float sMusicVolumeR = 1.0f;

static int sMusicState;
static int sMusicMode;
static MusicSlot* sMusicFree;

static MusicSlot sMusicSlots[16];

inline void MusicSetState(int state) {
    if (sMusicState != state) {
        while (fn_800F6D4C() == 3) {
        }
        fn_800F6D54(state);
        sMusicState = state;
    }
}

inline void MusicInitSlots(void) {
    int i;

    memset(sMusicSlots, 0, sizeof(sMusicSlots));
    sMusicFree = 0;
    for (i = 0; i < 16; i++) {
        sMusicSlots[i].next = sMusicFree;
        sMusicFree = &sMusicSlots[i];
    }
}

void MusicGetVolume(float* left, float* right) {
    if (left) {
        *left = sMusicVolumeL;
    }
    if (right) {
        *right = sMusicVolumeR;
    }
}

void MusicSetVolume(float left, float right) {
    sMusicVolumeL = MIN(1.0f, MAX(0.0f, left));
    sMusicVolumeR = MIN(1.0f, MAX(0.0f, right));
    fn_800F6CFC(70.0f * sMusicVolumeL, 70.0f * sMusicVolumeR);
}

inline void MusicApplyMode(int mode) {
    switch (sMusicMode = mode) {
    case 0:
        fn_800F6F34(0);
        break;
    case 1:
        fn_800F6F34(2);
        break;
    case 2:
        fn_800F6F34(1);
        break;
    }
}

void MusicSetMode(int mode) { MusicApplyMode(mode); }

void MusicPause(void) {
    if (sMusicState == 2) {
        MusicSetState(1);
    }
}

void MusicResume(void) {
    if (sMusicState == 1) {
        u32 status = fn_800F6D4C();
        while (status - 3 <= 1) {
            status = fn_800F6D4C();
        }
        fn_800F6D54(2);
        sMusicState = 2;
    }
}

void MusicStop(void) { MusicSetState(0); }

void MusicHold(void) { MusicSetState(1); }

void MusicReset(void) {
    MusicSetState(0);
    MusicInitSlots();
    fn_800F6F3C(0);
}

int MusicFree(int handle) {
    MusicSlot* slot = (MusicSlot*)~handle;

    if (fn_800F6FFC(slot) == 0) {
        memset(slot, 0, sizeof(MusicSlot));
        slot->next = sMusicFree;
        sMusicFree = slot;
        return 1;
    }
    return 0;
}

void MusicDone(void);

int MusicPlay(const char* name, void (*done)(void*), void* arg) {
    char ext[0x100];
    char base[0x100];
    char dir[0x100];
    int drive;
    MusicSlot* slot;
    int result = 0;

    if ((slot = sMusicFree) != 0) {
        sMusicFree = slot->next;
        slot->next = 0;
        LevelLoadNames(name, &drive, dir, base, ext);
        sprintf(slot->path, "Audio/%s.adp", base);
        slot->done = done;
        slot->next = (MusicSlot*)arg;
        if (fn_800F7130(slot->path, slot, 0x10, MusicDone) == 0) {
            result = ~(int)slot;
        } else {
            slot->next = sMusicFree;
            sMusicFree = slot;
        }
    }
    return result;
}

void MusicShutdown(void) {
    MusicSetState(0);
    MusicInitSlots();
    fn_800F6F3C(0);
}

void MusicInit(void) {
    int left;
    int right;

    sMusicState = 0;
    left = 70.0f * sMusicVolumeL;
    right = 70.0f * sMusicVolumeR;
    fn_800F7350();
    fn_800F6CFC(left, right);
    MusicApplyMode(0);
    MusicInitSlots();
}

void MusicDone(void) {
    MusicSlot* slot = fn_800F6D44();
    if (slot->done) {
        slot->done(slot->next);
    }
}

// SAM.cpp: streamed ADPCM audio (0x800E051C-0x800E0CB8).
// A track is read through a file stream in 0x800-byte blocks. The first two blocks carry the 0x40-byte
// DSP ADPCM headers of the left and right channel; the sample data of each channel goes into a
// 0x4000-byte ring buffer that the voice callback (SAMFillCallback) drains. Playback starts after
// eight blocks are buffered.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

extern "C" {
void* memcpy(void* dst, const void* src, u32 n);
void* memset(void* dst, int val, u32 n);

// Callees in other files, not named yet.
void fn_8016E20C(void);
void fn_8016E270(void);
void fn_8016E4C8(void);
void fn_8016E314(void* left, void* right, int (*fill)(u8*, u8*, int), int rate);
void fn_8016B998(int arg);
void fn_8003BA00(short id);
void fn_80080714(u32 stream);
void fn_80080D88(u32 stream);
void fn_80080A34(u32 stream, int count, int (*cb)(u8*, int), int arg);
u32 fn_80080E24(void* buffer, int size, int count, int arg);
}

void* MemAlloc(int size, const char* file, int line);

struct SAMHeader {
    u32 a;
    u32 data[14];
    u32 b;
};

u8* sRingBuf[2] = {0, 0};
int sRingWrite[2] = {0, 0};
int sRingRead[2] = {0, 0};
int sRingCount[2] = {0, 0};
u32 sStream = 0xFFFFFFFF;
short sTrack = -1;
int sDoneArg = -1;
int sSampleRate[2] = {3500, 3500};
short sPendingTrack = -1;

static int sBlockCount;
static int sPlaying;
static float sStartDelay;
static float sDelayLeft;
static int sState;

static u8 sStreamBuffer[0x2000];
static SAMHeader sHeaderL;
static SAMHeader sHeaderR;

int SAMReadCallback(u8* data, int size);
int SAMFillCallback(u8* left, u8* right, int size);

inline void SAMCopyHeader(SAMHeader* dst, SAMHeader* src) { *dst = *src; }

void SAMSetStartDelay(float delay) { sStartDelay = delay; }

u32 SAMGetStream(void) { return sStream; }

void SAMStreamEvent(int stream, int event) {
    if (event == 1) {
        sState = 3;
    }
}

void SAMPause(void) {
    if (sTrack != -1) {
        fn_8016E20C();
    }
}

void SAMNop(void) {}

void SAMReset(void) {
    fn_80080714(sStream);
    sTrack = -1;
    fn_8016E270();
    sPendingTrack = -1;
}

int SAMPlay(short track, int doneArg, int preload) {
    sPlaying = 0;
    fn_8016E4C8();
    sState = 1;

    if (preload != 0 || sPendingTrack != track) {
        sBlockCount = 0;
        sRingWrite[0] = 0;
        sRingWrite[1] = 0;
        sRingRead[0] = 0;
        sRingRead[1] = 0;
        sRingCount[0] = 0;
        sRingCount[1] = 0;
        sTrack = track;
        sDoneArg = doneArg;
        if (preload != 0) {
            sPendingTrack = track;
        } else {
            sPendingTrack = -1;
        }
    } else {
        sPendingTrack = -1;
        if (sBlockCount >= 8) {
            fn_8016E314(&sHeaderL, &sHeaderR, SAMFillCallback, sSampleRate[0]);
            fn_8003BA00(track);
            sPlaying = 1;
            sState = 2;
        }
    }
    return 1;
}

void SAMStop(void) {
    fn_8016E270();
    sState = 0;
    sTrack = -1;
    sPendingTrack = -1;
    if (sStream != 0xFFFFFFFF) {
        fn_80080D88(sStream);
        sStream = -1;
    }
}

void SAMUpdate(float dt) {
    if (sTrack != -1) {
        if (sState == 2 || sState == 1) {
            if (sDelayLeft != 0.0f) {
                sDelayLeft -= dt;
                if (sDelayLeft <= 0.0f) {
                    sDelayLeft = 0.0f;
                    fn_8003BA00(sTrack);
                }
            }
            fn_80080A34(sStream, 4, SAMReadCallback, 0);
        } else if (sState == 3) {
            fn_80080A34(sStream, 4, SAMReadCallback, 0);
        } else if (sState == 4) {
            sTrack = -1;
            fn_8016E270();
            sState = 0;
            fn_8016B998(sDoneArg);
        }
    }
}

void SAMAllocBuffers(void) {
    sRingBuf[0] = (u8*)MemAlloc(0x4000, "SAM.cpp", 278);
    sRingBuf[1] = (u8*)MemAlloc(0x4000, "SAM.cpp", 279);
}

void SAMInit(void) {
    sStream = fn_80080E24(sStreamBuffer, 0x800, 4, 1);
    sTrack = -1;
    sDoneArg = -1;
    sPendingTrack = -1;
    sStartDelay = 0.0f;
}

inline void SAMRingPut(int ch, u8* src, int n) {
    if (n + sRingWrite[ch] > 0x4000) {
        int first = 0x4000 - sRingWrite[ch];
        n -= first;
        if (first != 0) {
            memcpy(sRingBuf[ch] + sRingWrite[ch], src, first);
            sRingCount[ch] += first;
        }
        memcpy(sRingBuf[ch], src + first, n);
        sRingWrite[ch] = n;
        sRingCount[ch] += n;
    } else {
        memcpy(sRingBuf[ch] + sRingWrite[ch], src, n);
        sRingWrite[ch] += n;
        sRingCount[ch] += n;
    }
}

int SAMReadCallback(u8* data, int size) {
    int ch = (sBlockCount & 1);
    u8* block = data + 0x40;
    int n = size - 0x40;
    u8* src = block;

    if (n + sRingCount[ch] >= 0x4000) {
        return 0;
    }

    if (sBlockCount == 0 || sBlockCount == 1) {
        src += 0x40;
        n -= 0x40;
    }

    SAMRingPut(ch, src, n);

    if (sBlockCount == 0) {
        SAMCopyHeader(&sHeaderL, (SAMHeader*)block);
    }
    if (sBlockCount == 1) {
        SAMCopyHeader(&sHeaderR, (SAMHeader*)block);
    }
    if (sBlockCount == 8 && sPlaying == 0 && sPendingTrack == -1) {
        fn_8016E314(&sHeaderL, &sHeaderR, SAMFillCallback, sSampleRate[0]);
        sDelayLeft = sStartDelay;
        if (0.0f == sDelayLeft) {
            fn_8003BA00(sTrack);
        }
        sPlaying = 1;
        sState = 2;
    }
    sBlockCount++;
    return 1;
}

int SAMFillCallback(u8* left, u8* right, int size) {
    u8* dst = left;
    int i;

    for (i = 0; i < 2; i++) {
        int n;
        int want = size;
        n = (sRingCount[i] < size) ? sRingCount[i] : size;
        int room = 0x4000 - sRingRead[i];

        if (n > room) {
            memcpy(dst, sRingBuf[i] + sRingRead[i], room);
            want -= room;
            dst += room;
            sRingCount[i] -= room;
            sRingRead[i] = 0;
            n = (sRingCount[i] < want) ? sRingCount[i] : want;
        }

        if (n != 0) {
            memcpy(dst, sRingBuf[i] + sRingRead[i], n);
            dst += n;
            sRingRead[i] += n;
            sRingCount[i] -= n;
            if (sRingRead[i] == 0x4000) {
                sRingRead[i] = 0;
            }
        }

        if (n == 0 && sState >= 3 && sState < 4) {
            sState = 4;
        }

        if (want - n > 0) {
            int rest = want - n;
            if (rest != 0) {
                memset(dst, 0, rest);
            }
        }
        dst = right;
    }
    return 1;
}

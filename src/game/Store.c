// Store.c: saving and loading (0x800451D8-0x80045EBC).
// Two backends: the memory card (through a card manager in another file, with the "Geist" save file
// and the banner/icons from card.tpl) and a 16 KB ARAM block used as a quick store, copied through a
// 32-byte aligned bounce buffer. Also builds a CRC-32 table (reflected, polynomial 0x04C11DB7).
// Compiled as C++ like the rest of the game.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/os.h>
#include <dolphin/ar.h>

extern "C" {


// Card manager and other callees in other files, not named yet.
int fn_80115A34(int a, int b, int c);
void fn_800ECB10(int arg);
void fn_8000AA14(void);
void fn_8000AA08(void);
int fn_8000A9E0(void);
void fn_80005A64(float dt);
void fn_802E1BBC(void* card, int op);
void fn_802E14B0(const char* game, const char* company, const char* comment, int size, void* banner, int code,
                 void* icons, void* bannerImage, void* file);
void fn_802E1C48(void* card, int arg, void* file, void* buffer);
int fn_802E1728(void* card, int arg);
void fn_802E1C1C(void* card);
void fn_802E1B38(void* card);
void fn_802E1C10(void* card);
int fn_802DE64C(int status, int* out);
int fn_802DDEDC(int status, int a, int b, float dt);
void fn_802DDE38(int a, int b);
void fn_802DDE9C(void);
}

typedef struct TPLHeader {
    u16 height;
    u16 width;
    int format;
    char* data;
} TPLHeader;

typedef struct TPLDescriptor {
    TPLHeader* textureHeader;
    void* CLUTHeader;
} TPLDescriptor;

typedef struct TPLPalette {
    u32 versionNumber;
    u32 numDescriptors;
    TPLDescriptor* descriptorArray;
} TPLPalette;

int ImageLoadTPL(TPLPalette** pal, const char* name);
void ImageFreeTPL(TPLPalette** pal);
float LoadingTime(void);
void SystemPoll(void);

typedef struct CardData {
    u8 pad[0x24];
    u8* data;
} CardData;

typedef struct CardWork {
    int unk0;
    u8 pad4[0x8];
    int unkC;
    CardData* data;
    int busy;
    u8 pad18[0x90];
    int unkA8;
    u8 padAC[0x198];
} CardWork;

typedef struct CardFile {
    u8 pad[0x28];
    int* done;
    u8 pad2C[0x30];
} CardFile;

static CardFile sCardFile;
static CardWork sCardWork;
static u8 sSaveBuffer[0xA000];
static u8 sCardBanner[0x4000];
static OSThread sStoreThread;
static u8 sStoreStack[0x1000];
static u32 sCrcTable[256];

int sStoreMode = 0;
int sStoreThreadRun = 0;

static int sCardCode;
static int sCardArg;
static u32 sAramBase;
static int sCardDone;
static volatile int sAramBusy;

int StoreAramWrite(int size, u8* src);
int StoreAramRead(int size, u8* dst);
int StoreCardWait(u32 flags);
void StoreAramDone(u32 task);
void* StoreThread(void* arg);

inline void StoreLoadBanner(u8* banner, u8* icons) {
    TPLPalette* pal;
    int i;
    u8* dst;
    TPLHeader* tex;

    if (ImageLoadTPL(&pal, "card.tpl")) {
        tex = pal->descriptorArray[0].textureHeader;
        switch (tex->format) {
        case 5:
            memcpy(banner, tex->data, 0x1800);
            break;
        default:
            OSPanic("Store.c", 217, "Unsupported banner texture formant.");
            break;
        }
        for (dst = icons, i = 1; i < pal->numDescriptors && i < 8; i++) {
            tex = pal->descriptorArray[i].textureHeader;
            switch (tex->format) {
            case 5:
                memcpy(dst, tex->data, 0x800);
                dst += 0x800;
                break;
            default:
                OSPanic("Store.c", 239, "Unsupported icon texture formant.");
                break;
            }
        }
        ImageFreeTPL(&pal);
    }
}

int StoreSave(int mode, int unused, int size, u32 flags, void* src) {
    int result = 0;

    fn_800ECB10(fn_80115A34(0, 1, 0));
    switch (mode) {
    case 1:
        result = StoreAramWrite(size, (u8*)src) != 0;
        break;
    case 2: {
        u8* icons;
        u8* banner;

        fn_8000AA14();
        sStoreThreadRun = 1;
        OSCreateThread(&sStoreThread, StoreThread, 0, sStoreStack + sizeof(sStoreStack), sizeof(sStoreStack), 16, 1);
        OSResumeThread(&sStoreThread);
        icons = sCardBanner + 0x184C;
        banner = sCardBanner + 0x4C;
        StoreLoadBanner(banner, icons);
        fn_802E14B0("Geist", "Geist", "Save File", 0x4000, sCardBanner, sCardCode, icons, banner, &sCardFile);
        memcpy(sCardWork.data->data + 0x204C, src, size);
        fn_802E1BBC(&sCardWork, 3);
        result = StoreCardWait(flags);
        if (result == 2) {
            sCardWork.unkA8 = 0;
        }
        sStoreThreadRun = 0;
        OSYieldThread();
        fn_8000AA08();
        SystemPoll();
        break;
    }
    }
    fn_800ECB10(0);
    return result;
}

int StoreLoad(int mode, int unused, int size, u32 flags, void* dst) {
    int result = 0;

    fn_800ECB10(fn_80115A34(0, 1, 0));
    sStoreMode = mode;
    switch (mode) {
    case 1:
        result = StoreAramRead(size, (u8*)dst) != 0;
        break;
    case 2:
        fn_802E1BBC(&sCardWork, 2);
        result = StoreCardWait(flags);
        if (result == 1) {
            memcpy(dst, sCardWork.data->data + 0x204C, size);
        }
        break;
    }
    fn_800ECB10(0);
    return result;
}

int StoreCheck(int mode, int unused, u32 flags) {
    fn_802E1BBC(&sCardWork, 1);
    switch (StoreCardWait(flags)) {
    case 1:
        return 2;
    case 3:
        return 1;
    }
    return 0;
}

int StoreInit(int code) {
    u8* banner;
    u8* icons;

    icons = sCardBanner + 0x184C;
    banner = sCardBanner + 0x4C;
    sCardCode = code;
    StoreLoadBanner(banner, icons);
    fn_802E14B0("Geist", "Geist", "Save File", 0x4000, sCardBanner, sCardCode, icons, banner, &sCardFile);
    fn_802E1C48(&sCardWork, sCardArg, &sCardFile, sSaveBuffer);
    sCardFile.done = &sCardDone;
    return 1;
}

int StoreCardWait(u32 flags) {
    int status = -1;
    int arg;
    int out;

    while (sCardWork.busy) {
        float dt = LoadingTime();
        fn_80005A64(dt);
        if (sCardWork.busy) {
            arg = 0;
            if ((flags & 0x10) && !(flags & 0x20)) {
                switch (fn_802DE64C(status, &out)) {
                case 1:
                    arg = out;
                    break;
                case 0:
                    break;
                default:
                    flags &= ~0x10;
                    break;
                }
            }
            if (!(flags & 0x10)) {
                arg = fn_802DDEDC(status, sCardWork.unk0, sCardWork.busy, dt);
            }
        }
        if (sCardWork.busy) {
            status = fn_802E1728(&sCardWork, arg);
            if (status < 0) {
                if ((flags & 0x10) && (flags & 0x20) && status < -3) {
                    sCardWork.busy = 0;
                    fn_802E1C1C(&sCardWork);
                } else if (fn_8000A9E0() && (u32)(status + 3) > 2) {
                    fn_802DDE38(0, 0);
                }
            }
        }
        if (status == 4) {
            fn_802DDE38(1, 1);
        }
    }
    fn_802DDE9C();
    return status;
}

int StoreCardDone(void) {
    if (sCardDone) {
        sCardDone = 0;
        if (sCardWork.unkC == 0) {
            return -1;
        }
        return 1;
    }
    return 0;
}

void StoreNop(void) {}

void StoreCardMount(void) { fn_802E1B38(&sCardWork); }

void StoreCardUnmount(void) { fn_802E1C10(&sCardWork); }

int StoreTrue(void) { return 1; }

inline u32 StoreReflect(u32 ref, char ch) {
    u32 value = 0;
    int i;

    for (i = 1; i < ch + 1; i++) {
        if (ref & 1) {
            value |= 1 << (ch - i);
        }
        ref >>= 1;
    }
    return value;
}

inline void StoreInitCrc(void) {
    u32 poly = 0x04C11DB7;
    int i;
    int j;

    for (i = 0; i <= 0xFF; i++) {
        sCrcTable[i] = StoreReflect(i, 8) << 24;
        for (j = 0; j < 8; j++) {
            sCrcTable[i] = (sCrcTable[i] << 1) ^ (sCrcTable[i] & (1 << 31) ? poly : 0);
        }
        sCrcTable[i] = StoreReflect(sCrcTable[i], 32);
    }
}

int StoreAramInit(void) {
    StoreInitCrc();
    sAramBase = ARAlloc(0x4000);
    return 1;
}

int StoreAramWrite(int size, u8* src) {
    u8 buffer[0x1000] ATTRIBUTE_ALIGN(32);
    ARQRequest request;
    u8* p = src;
    u32 aram;
    int offset;
    int left;
    int n;

    if (size >= 0x4000) {
        return 0;
    }

    left = size;
    aram = 0;
    offset = 0;
    while (left > 0) {
        n = 0x1000;
        if (offset > 0 || left < 0x1000) {
            n = (left < 0x1000 - offset) ? left : 0x1000 - offset;
            sAramBusy = 1;
            ARQPostRequest(&request, 1, ARQ_TYPE_ARAM_TO_MRAM, ARQ_PRIORITY_LOW, sAramBase + aram, (u32)buffer, 0x1000,
                           StoreAramDone);
            while (sAramBusy) {
            }
            DCInvalidateRange(buffer, 0x1000);
        }
        memcpy(buffer + offset, p, n);
        DCFlushRange(buffer, 0x1000);
        sAramBusy = 1;
        ARQPostRequest(&request, 1, ARQ_TYPE_MRAM_TO_ARAM, ARQ_PRIORITY_LOW, (u32)buffer, sAramBase + aram, 0x1000,
                       StoreAramDone);
        while (sAramBusy) {
        }
        left -= n;
        p += n;
        offset = 0;
        aram += 0x1000;
    }
    return size;
}

int StoreAramRead(int size, u8* dst) {
    u8 buffer[0x1000] ATTRIBUTE_ALIGN(32);
    ARQRequest request;
    u8* p = dst;
    u32 aram;
    int offset;
    int left;
    int n;

    if (size >= 0x4000) {
        return 0;
    }

    left = size;
    aram = 0;
    offset = 0;
    while (left > 0) {
        n = 0x1000;
        if (offset > 0 || left < 0x1000) {
            n = (left < 0x1000 - offset) ? left : 0x1000 - offset;
        }
        sAramBusy = 1;
        ARQPostRequest(&request, 1, ARQ_TYPE_ARAM_TO_MRAM, ARQ_PRIORITY_LOW, sAramBase + aram, (u32)buffer, 0x1000,
                       StoreAramDone);
        while (sAramBusy) {
        }
        DCInvalidateRange(buffer, 0x1000);
        memcpy(p, buffer + offset, n);
        left -= n;
        p += n;
        offset = 0;
        aram += 0x1000;
    }
    return size;
}

void StoreAramDone(u32 task) { sAramBusy = 0; }

void* StoreThread(void* arg) {
    while (sStoreThreadRun) {
        SystemPoll();
        OSYieldThread();
    }
    return 0;
}

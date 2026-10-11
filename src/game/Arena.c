// Arena.c: ten bump-pointer arenas (0x8007C868-0x8007CC04).
// Each arena is a chain of blocks of at least 0x800 bytes taken from the heap; allocations are
// rounded up to 4 bytes. An unused arena points at a static empty block. Compiled as C++ like the
// rest of the game (MemAlloc/MemFree use C++ names).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

void* MemAlloc(int size, const char* file, int line);
void MemFree(void* p, const char* file, int line);

#define ARENA_COUNT 10

typedef struct ArenaBlock {
    struct ArenaBlock* next;
    u8* end;
    u8* cur;
} ArenaBlock;

static ArenaBlock sArenaEmpty[ARENA_COUNT];
static ArenaBlock* sArenaHead[ARENA_COUNT];

static int sArenaCount;
static u32 sArenaFree;
static u32 sArenaUsed;

void* ArenaAlloc(int arena, int size) {
    u32 n = (size + 3) & ~3;
    int blockSize = n + 0x80C;
    ArenaBlock* b = sArenaHead[arena];

    while (n > (u32)(b->end - b->cur)) {
        b = b->next;
        if (b == 0) {
            b = (ArenaBlock*)MemAlloc(blockSize, "Arena.c", 51);
            if (sArenaHead[arena]->end != 0) {
                b->next = sArenaHead[arena];
            } else {
                b->next = 0;
            }
            b->end = (u8*)b + blockSize;
            b->cur = (u8*)(b + 1);
            sArenaHead[arena] = b;
        }
    }

    b->cur += n;
    return b->cur - n;
}

inline void ArenaFreeBlocks(int arena) {
    ArenaBlock* b = sArenaHead[arena];
    ArenaBlock* next;
    u32 bit;

    while (b != 0) {
        next = b->next;
        if (b != &sArenaEmpty[arena]) {
            MemFree(b, "Arena.c", 84);
        }
        b = next;
    }
    bit = 1 << arena;
    sArenaHead[arena] = 0;
    sArenaUsed &= ~bit;
    sArenaFree |= bit;
}

void ArenaRelease(int arena) {
    if (sArenaUsed & (1 << arena)) {
        ArenaFreeBlocks(arena);
    }
}

int ArenaCreate(void) {
    int i = 0;
    u32 mask;
    u32 bit;

    if (sArenaCount >= ARENA_COUNT) {
        return -1;
    }

    mask = sArenaFree;
    while (!(mask & 1)) {
        mask >>= 1;
        i++;
    }

    bit = 1 << i;
    sArenaFree &= ~bit;
    sArenaUsed |= bit;
    sArenaCount++;
    return i;
}

void ArenaReleaseAll(void) {
    int i = 0;
    u32 mask = sArenaUsed;

    while (mask != 0) {
        if (mask & 1) {
            ArenaFreeBlocks(i);
        }
        mask >>= 1;
        i++;
    }
    sArenaCount = 0;
}

void ArenaInit(void) {
    int i;

    for (i = 0; i < ARENA_COUNT; i++) {
        sArenaEmpty[i].next = 0;
        sArenaEmpty[i].end = 0;
        sArenaEmpty[i].cur = 0;
        sArenaHead[i] = &sArenaEmpty[i];
        sArenaFree |= 1 << i;
    }
}

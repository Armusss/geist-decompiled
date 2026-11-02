// Turf.cpp: "turf" zones, regions that are streamed in from disc and drawn as patches of tiles
// (0x80012E04-0x8001598C). A zone owns a list of 0x44-byte items that are stored on disc as
// offsets and turned into pointers when the zone is streamed in; the callbacks here do the
// reverse when it is released.
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

struct TurfItem {
    int id;                 // 0x00
    u8 unk4[0x0C - 4];
    u32 flags;              // 0x0C
    int stamp;              // 0x10
    u8 unk14[0x2C - 0x14];
    int offA;               // 0x2C
    u8 unk30[0x34 - 0x30];
    int offB;               // 0x34
    int table;              // 0x38
    int offC;               // 0x3C
    int offD;               // 0x40
};                          // size 0x44

struct TurfZone {
    int handleCount;        // 0x00
    int itemCount;          // 0x04
    int countA;             // 0x08
    int countB;             // 0x0C
    int unk10;              // 0x10
    int countC;             // 0x14
    int countD;             // 0x18
    int* handles;           // 0x1C
    u16 state;              // 0x20
    u8 unk22[0x28 - 0x22];
    int size;               // 0x28
    u8 unk2C[0x30 - 0x2C];
    TurfItem* items;        // 0x30
    int baseA;              // 0x34
    int baseB;              // 0x38
    int baseC;              // 0x3C
    int baseD;              // 0x40
    int baseE;              // 0x44
};                          // size 0x48

struct TurfFlag {
    int mode;               // 0 off, 1 on, 2 colored
    u8 r, g, b, a;
};

struct StreamEntry;

extern "C" void MemFree(void* p, const char* file, int line);
extern "C" void* MemAlloc(int size, const char* file, int line);
extern "C" void fn_80008C00(const char* msg);
extern "C" void fn_800050B4(void* p, int value, int size);
extern u8 gTurfTable[];         // 0x803ADFA0
struct TurfDef {
    int w[12];
};
extern TurfDef gTurfDef;        // 0x803DAA40

int StreamRead(int type, StreamEntry* entry, void (*cb)(int, void*, void*), void* arg);   // 0x8017C32C
void StreamFree(void* p);                         // 0x8017C1F4
int GunBlockClear8017BD20(void* p);               // 0x8017BD20
void GunBlockClear8017BCD0(void* p);              // 0x8017BCD0
void GunStateTimerTick8017BAD0(void* p);          // 0x8017BAD0
void GunStateStep8017BBBC(void* p);               // 0x8017BBBC
void GunStateFinish8017BC5C(void* p);             // 0x8017BC5C
extern "C" void fn_8000F60C(int handle);
extern "C" void fn_8000F874(int handle);
int FileAddHandler(int type, int (*handler)(void*, int, void*), void* arg);   // 0x8001950C

struct TurfRegion {
    u8 unk0[0x1E];
    s16 zone;               // 0x1E
};
extern int gTurfPassFlag;       // 0x804D2620
extern "C" void fn_80008B94(int a);
extern int gTurfEnabled;        // 0x804D2630
extern int gTurfDrawEnabled;    // 0x804D2624
extern u32 gTurfKColor;         // 0x804D262C
extern int gTurfHuntArg;        // 0x804D4160
extern int gTurfStamp;          // 0x804D4164
extern int* gTurfHeader;        // 0x804D4168
extern TurfZone* gTurfZones;    // 0x804D416C
extern void* gTurfRegions;      // 0x804D4170
extern int gTurfUnk4174;        // 0x804D4174
extern int gTurfUnk4178;        // 0x804D4178
extern int gTurfZoneCount;      // 0x804D417C
extern int gTurfRegionCount;    // 0x804D4180
extern TurfFlag* gTurfFlags;    // 0x804D4184
extern int gTurfLoadedDirty;    // 0x804D4188
extern int gTurfReleaseDirty;   // 0x804D418C
extern int gTurfMode;           // 0x804D4194

void TurfZoneCallback(int cmd, void* arg, void* zone);   // 0x800130CC
void TurfZoneStart(TurfZone* zone, int mem);             // 0x800145B8
int TurfFileHandler(void* data, int size, void* unused); // 0x800142CC

// Turns a zone's item offsets back into relative values and clears the item list.
static inline void TurfZoneRelease(TurfZone* zone) {
    int i = 0;
    int off = 0;
    if (zone->items != 0) {
        for (; i < zone->itemCount; i++) {
            TurfItem* item = (TurfItem*)((u8*)zone->items + off);
            off += 0x44;
            item->offA -= zone->baseA;
            item->offB = (item->offB - zone->baseB) / 12;
            item->offC = (item->offC - zone->baseC) / 4;
            item->offD = (item->offD - zone->baseD) / 4;
        }
    }
    zone->items = 0;
    zone->baseA = 0;
    zone->baseB = 0;
    zone->baseC = 0;
    zone->baseD = 0;
    zone->baseE = 0;
}

// 0x80012E04 stores one byte into an 8x4-tiled texture buffer.
void TurfPlotPixel(u8* base, int width, int y, int x, u8 value) {
    *(u8*)(x % 8 + (y % 4) * 8 + ((y / 4) * (width / 8) + x / 8) * 32 + (int)base) = value;
}

// 0x80012E64
extern "C" void fn_80012E64(int mode) {   // TurfSetMode; called from other units
    gTurfMode = mode;
}

// 0x80012E6C counts the bytes of resident zones and of all zones.
extern "C" void fn_80012E6C(int* resident, int* total) {   // TurfCountSizes; called from other units
    TurfZone* zone = gTurfZones;
    int resSum = 0;
    int allSum = 0;
    int i;
    for (i = 0; i < gTurfZoneCount; i++) {
        allSum += zone->size;
        if (zone->state == 1 || zone->state == 4) {
            resSum += zone->size;
        }
        zone++;
    }
    if (resident != 0) {
        *resident = resSum;
    }
    if (total == 0) {
        return;
    }
    *total = allSum;
}

// 0x80012ECC re-queues zones that finished loading but were not yet started.
void TurfStartPending(void) {
    int off;
    int i;
    TurfZone* zone;
    if (gTurfLoadedDirty != 0 && gTurfEnabled != 0) {
        off = 0;
        gTurfLoadedDirty = 0;
        for (i = 0; i < gTurfZoneCount; i++) {
            zone = (TurfZone*)((u8*)gTurfZones + off);
            if (zone->state == 2) {
                zone->state = 1;
                StreamRead(0x2A, (StreamEntry*)&zone->state, TurfZoneCallback, 0);
            }
            off += 0x48;
        }
    }
}

// 0x80012F78 releases zones whose streaming finished (state 3).
void TurfReleaseFinished(void) {
    int off;
    int i;
    TurfZone* zone;
    if (gTurfReleaseDirty != 0 && gTurfEnabled != 0) {
        off = 0;
        gTurfReleaseDirty = 0;
        for (i = 0; i < gTurfZoneCount; i++) {
            zone = (TurfZone*)((u8*)gTurfZones + off);
            if (zone->state == 3) {
                if (zone->items != 0) {
                    zone->state = 0;
                    TurfZoneRelease(zone);
                    StreamFree(0);
                } else {
                    gTurfReleaseDirty = 1;
                }
            }
            off += 0x48;
        }
    }
}

// 0x800130CC stream callback for a zone: cmd > 0 means data arrived, cmd < 0 an error/cancel.
void TurfZoneCallback(int cmd, void* arg, void* zonePtr) {
    TurfZone* zone = (TurfZone*)zonePtr;
    int r;
    if (cmd < 1) {
        if (cmd < 0) {
            r = GunBlockClear8017BD20(&zone->state);
            if (r == 2 || r == 1) {
                if (zone->items == 0) {
                    StreamRead(0x2A, (StreamEntry*)&zone->state, TurfZoneCallback, zone);
                }
                zone->state = 1;
            } else {
                TurfZoneRelease(zone);
            }
        } else {
            GunBlockClear8017BCD0(&zone->state);
            TurfZoneRelease(zone);
        }
    } else if (arg == 0) {
        TurfZoneRelease(zone);
    } else {
        TurfZoneStart(zone, (int)arg);
    }
}

// 0x80013394 ticks a zone's timer state and releases its handles.
void TurfZoneTick(int index) {
    TurfZone* zone;
    int i;
    if (index >= 0 && index < gTurfZoneCount) {
        gTurfReleaseDirty = 1;
        zone = &gTurfZones[index];
        GunStateTimerTick8017BAD0(&zone->state);
        for (i = 0; i < zone->handleCount; i++) {
            fn_8000F60C(zone->handles[i]);
        }
    }
}

// 0x80013424 steps a zone's state and releases its handles (the other direction).
void TurfZoneStep(int index) {
    TurfZone* zone;
    int i;
    if (index >= 0 && index < gTurfZoneCount) {
        gTurfLoadedDirty = 1;
        zone = &gTurfZones[index];
        GunStateStep8017BBBC(&zone->state);
        for (i = 0; i < zone->handleCount; i++) {
            fn_8000F874(zone->handles[i]);
        }
    }
}

// 0x800134B4 turns a zone flag off.
void TurfFlagOff(int index) {
    if (index < 0) {
        return;
    }
    if (index >= gTurfZoneCount) {
        return;
    }
    gTurfFlags[index].mode = 0;
}

// 0x800134DC turns a zone flag on.
void TurfFlagOn(int index) {
    if (index < 0) {
        return;
    }
    if (index >= gTurfZoneCount) {
        return;
    }
    gTurfFlags[index].mode = 1;
}

// 0x80013504 gives a zone flag a color.
void TurfFlagColor(int index, u8 r, u8 g, u8 b) {
    if (index < 0) {
        return;
    }
    if (index >= gTurfZoneCount) {
        return;
    }
    gTurfFlags[index].mode = 2;
    gTurfFlags[index].r = r;
    gTurfFlags[index].g = g;
    gTurfFlags[index].b = b;
    gTurfFlags[index].a = 0xFF;
}

// 0x80013560 finishes all zones, releases what is left and frees the flag table.
void TurfShutdown(void) {
    int i;
    int off = 0;
    gTurfReleaseDirty = 1;
    for (i = 0; i < gTurfZoneCount; i++) {
        GunStateFinish8017BC5C((u8*)gTurfZones + off + 0x20);
        off += 0x48;
    }
    TurfReleaseFinished();
    if (gTurfFlags != 0) {
        MemFree(gTurfFlags, "Turf.cpp", 0x466);
        gTurfFlags = 0;
    }
}

// 0x80013710 registers the zone file handler and clears the module state.
void TurfInit(void) {
    FileAddHandler(0x2A, TurfFileHandler, 0);
    gTurfHeader = 0;
    gTurfZones = 0;
    gTurfRegions = 0;
    gTurfUnk4174 = 0;
    gTurfUnk4178 = 0;
    gTurfFlags = 0;
    gTurfZoneCount = 0;
    gTurfRegionCount = 0;
    gTurfLoadedDirty = 0;
    gTurfReleaseDirty = 0;
}

// 0x8001376C stores the hunt argument and a 12-word draw definition.
void TurfSetDef(int huntArg, int* def) {
    gTurfHuntArg = huntArg;
    gTurfDef.w[0] = def[0];
    gTurfDef.w[1] = def[1];
    gTurfDef.w[2] = def[2];
    gTurfDef.w[3] = def[3];
    gTurfDef.w[4] = def[4];
    gTurfDef.w[5] = def[5];
    gTurfDef.w[6] = def[6];
    gTurfDef.w[7] = def[7];
    gTurfDef.w[8] = def[8];
    gTurfDef.w[9] = def[9];
    gTurfDef.w[10] = def[10];
    gTurfDef.w[11] = def[11];
}

// TODO (not drafted yet, 0x700 bytes at 0x800137DC): draws the visible tiles of a region (30 callees)

// 0x80013EDC copies a region's first six words (its bounds) out, if drawing is enabled.
int TurfGetRegionBounds(int index, int* out) {
    int* region;
    if (gTurfDrawEnabled == 0) {
        return 0;
    }
    if (index > -1 && index < gTurfRegionCount) {
        region = (int*)((u8*)gTurfRegions + index * 0x20);
        out[0] = region[0];
        out[1] = region[1];
        out[2] = region[2];
        out[3] = region[3];
        out[4] = region[4];
        out[5] = region[5];
        return 1;
    }
    return 0;
}

// 0x80013F50 starts a new draw pass: bumps the stamp so every item is drawn once.
extern "C" void fn_80013F50(void) {   // TurfBeginPass; called from other units
    gTurfPassFlag = 1;
    gTurfStamp++;
    fn_80008B94(0);
}

// 0x80013F88 returns the item list of the zone a region belongs to (or null).
TurfItem* TurfRegionItems(TurfRegion* region) {
    s16 zone = region->zone;
    if (zone > -1 && zone < gTurfZoneCount) {
        return gTurfZones[zone].items;
    }
    return 0;
}
// TODO (not drafted yet, 0x310 bytes at 0x80013FBC): not read yet

// 0x800142CC file handler for a turf data file: checks the version and carves the tables out of the block.
int TurfFileHandler(void* data, int size, void* unused) {
    bool ok;
    int n;
    int avail;
    int pad;
    int mem;
    int i;
    int off;
    gTurfHeader = (int*)data;
    if (size < 1) {
        gTurfHeader = 0;
    }
    if (*gTurfHeader < 9) {
        fn_80008C00("TRC version mismatch (old code)");
        return 0;
    }
    if (*gTurfHeader < 10) {
        gTurfZoneCount = gTurfHeader[1];
        ok = false;
        n = gTurfZoneCount * 0x48;
        if (size - 0x20 > 0 && n > 0) {
            ok = true;
        }
        gTurfZones = (TurfZone*)(gTurfHeader + 8);
        if (!ok) {
            gTurfZones = 0;
        }
        pad = ((n + 0x1F) & ~0x1F) - gTurfZoneCount * 0x48;
        mem = (int)(gTurfHeader + 8) + pad + n;
        avail = (size - 0x20 - gTurfZoneCount * 0x48) - pad;
        gTurfFlags = (TurfFlag*)MemAlloc(gTurfZoneCount << 3, "Turf.cpp", 0x229);
        fn_800050B4(gTurfFlags, 0, gTurfZoneCount << 3);
        ok = false;
        gTurfRegionCount = gTurfHeader[2];
        if (avail > 0 && gTurfRegionCount * 0x20 > 0) {
            ok = true;
        }
        gTurfRegions = (void*)mem;
        if (!ok) {
            gTurfRegions = 0;
        }
        avail = avail - gTurfRegionCount * 0x20;
        mem = mem + gTurfRegionCount * 0x20;
        n = gTurfHeader[3];
        ok = false;
        if (avail > 0 && n * 2 > 0) {
            ok = true;
        }
        gTurfUnk4174 = mem;
        if (!ok) {
            gTurfUnk4174 = 0;
        }
        pad = ((n * 2 + 0x1F) & ~0x1F) - n * 2;
        ok = false;
        mem = mem + n * 2 + pad;
        if ((avail - n * 2) - pad > 0 && gTurfHeader[4] * 4 > 0) {
            ok = true;
        }
        gTurfUnk4178 = mem;
        if (!ok) {
            gTurfUnk4178 = 0;
        }
        mem = mem + ((gTurfHeader[4] * 4 + 0x1F) & ~0x1F);
        off = 0;
        for (i = 0; i < gTurfZoneCount; i++) {
            int* h = (int*)((u8*)gTurfZones + off + 0x1C);
            off += 0x48;
            *h = gTurfUnk4178 + *h * 4;
        }
        off = 0;
        for (i = 0; i < gTurfRegionCount; i++) {
            int* r = (int*)((u8*)gTurfRegions + off + 0x18);
            off += 0x20;
            *r = gTurfUnk4174 + *r * 2;
        }
        if (gTurfHeader[5] == 0) {
            off = 0;
            for (i = 0; i < gTurfZoneCount; i++) {
                TurfZone* z = (TurfZone*)((u8*)gTurfZones + off);
                z->state = 4;
                TurfZoneStart(z, mem);
                off += 0x48;
                mem += z->size;
            }
        }
        return 1;
    }
    fn_80008C00("TRC version mismatch (old data)");
    return 0;
}

// 0x800145B8 carves a zone's item, offset and table arrays out of its block and turns the stored offsets into pointers.
void TurfZoneStart(TurfZone* zone, int mem) {
    bool ok;
    int count;
    int bytes;
    int avail;
    int pad;
    int p;
    int n;
    int i;
    int off;
    ok = false;
    count = zone->itemCount;
    bytes = count * 0x44;
    if (zone->size > 0 && bytes > 0) {
        ok = true;
    }
    zone->items = (TurfItem*)(ok ? mem : 0);
    pad = ((bytes + 0x1F) & ~0x1F) - bytes;
    avail = (zone->size - bytes) - pad;
    n = zone->countA;
    p = mem + bytes + pad;
    ok = false;
    if (avail > 0 && n > 0) {
        ok = true;
    }
    zone->baseA = ok ? p : 0;
    count = zone->countB;
    pad = ((n + 0x1F) & ~0x1F) - n;
    ok = false;
    avail = (avail - n) - pad;
    bytes = count * 0xC;
    p = p + n + pad;
    if (avail > 0 && bytes > 0) {
        ok = true;
    }
    zone->baseB = ok ? p : 0;
    zone->unk10 = 0x798;
    zone->baseC = (int)gTurfTable;
    pad = ((bytes + 0x1F) & ~0x1F) - bytes;
    n = zone->countC;
    avail = (avail - bytes) - pad;
    p = p + bytes + pad;
    ok = false;
    bytes = n * 4;
    if (avail > 0 && bytes > 0) {
        ok = true;
    }
    zone->baseD = ok ? p : 0;
    pad = ((bytes + 0x1F) & ~0x1F) - bytes;
    p = p + bytes + pad;
    ok = false;
    if ((avail - bytes) - pad > 0 && zone->countD << 2 > 0) {
        ok = true;
    }
    if (!ok) {
        p = 0;
    }
    zone->baseE = p;
    off = 0;
    for (i = 0; i < zone->itemCount; i++) {
        TurfItem* item = (TurfItem*)((u8*)zone->items + off);
        off += 0x44;
        item->offA = zone->baseA + item->offA;
        item->offB = zone->baseB + item->offB * 0xC;
        item->table = zone->baseC;
        item->offC = zone->baseD + item->offC * 4;
        item->offD = zone->baseE + item->offD * 4;
    }
}

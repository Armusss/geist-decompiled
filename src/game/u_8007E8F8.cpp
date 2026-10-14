// Object lookup tables (0x8007E8F8-0x8007EE68): the level's OLT file (version 5) holds a list of
// tables, each a list of {key, id, value} entries. Lookups find an entry by table key (or any table
// for 0xFFFFFFFF) and by id or key; when the file's flag bit 0 is set, values are offsets into the
// data that follows the tables and are relocated on load.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

extern "C" {
// Callees in other files, not named yet.
void fn_80026660(int type, void* func, int arg);
}

void FileAddHandler(int type, int (*func)(void*, int, void*), void* user);
void FileFlushBuffer(void* p, int size);
void FatalError(const char* message);
void* VAlloc(int size);
void VFree(void* p);

struct OLTEntry {
    u32 key;
    int id;
    u32 value;
};

struct OLTTable {
    u32 key;
    u32 base;
    int count;
    OLTEntry* entries;
};

struct OLTHeader {
    int version;
    u32 flags;
    int count;
};

static OLTHeader* sOLT;
static OLTTable** sOLTTables;
static u8* sOLTData;

int OLTMsg(int id, int msg, void* data) { return 0; }

u32 OLTFindValue(u32 table, int id) {
    int i;
    int j;

    if (sOLT == 0 || ((sOLT->flags == 0) & 1)) {
        return 0;
    }
    for (i = 0; i < sOLT->count; i++) {
        if (sOLTTables[i]->key == table || table == 0xFFFFFFFF) {
            for (j = 0; j < sOLTTables[i]->count; j++) {
                if (id == sOLTTables[i]->entries[j].id) {
                    return sOLTTables[i]->entries[j].value;
                }
            }
        }
    }
    return 0;
}

u32 OLTFindValueByKey(u32 table, u32 key) {
    int i;
    int j;

    if (sOLT == 0 || ((sOLT->flags == 0) & 1)) {
        return 0;
    }
    for (i = 0; i < sOLT->count; i++) {
        if (sOLTTables[i]->key == table || table == 0xFFFFFFFF) {
            for (j = 0; j < sOLTTables[i]->count; j++) {
                if (key == sOLTTables[i]->entries[j].key) {
                    return sOLTTables[i]->entries[j].value;
                }
            }
        }
    }
    return 0;
}

int GameEvent(int table, int id) {
    int i;
    int j;

    if (sOLT == 0) {
        return -1;
    }
    for (i = 0; i < sOLT->count; i++) {
        if (sOLTTables[i]->key == table || table == 0xFFFFFFFF) {
            for (j = 0; j < sOLTTables[i]->count; j++) {
                if (id == sOLTTables[i]->entries[j].id) {
                    return sOLTTables[i]->entries[j].key;
                }
            }
        }
    }
    return -1;
}

int AssetFind(u32 table, u32 key) {
    int i;
    int j;

    if (sOLT == 0) {
        return -1;
    }
    for (i = 0; i < sOLT->count; i++) {
        if (sOLTTables[i]->key == table || table == 0xFFFFFFFF) {
            for (j = 0; j < sOLTTables[i]->count; j++) {
                if (key == sOLTTables[i]->entries[j].key) {
                    return sOLTTables[i]->entries[j].id;
                }
            }
        }
    }
    return -1;
}

void OLTFree(void) {
    if (sOLTTables) {
        VFree(sOLTTables);
    }
}

int OLTLoad(void* data, int size, void* user);

void OLTInit(void) {
    FileAddHandler(0x17, OLTLoad, 0);
    fn_80026660(0x31, (void*)OLTMsg, 0);
    sOLT = 0;
    sOLTTables = 0;
    sOLTData = 0;
}

int OLTLoad(void* data, int size, void* user) {
    u8* p = (u8*)data;
    int left;
    int i;
    int j;

    sOLT = size > 0 ? (OLTHeader*)p : 0;
    left = size - sizeof(OLTHeader);
    p += sizeof(OLTHeader);
    FileFlushBuffer(sOLT, sizeof(OLTHeader));
    if (sOLT->version < 5) {
        FatalError("OLTLoad() - Level data is old. Rebuild level");
        return 0;
    }
    if (sOLT->version > 5) {
        FatalError("OLTLoad() - Code is old. Rebuild code");
        return 0;
    }
    sOLTTables = (OLTTable**)VAlloc(sOLT->count * sizeof(OLTTable*));
    for (i = 0; i < sOLT->count; i++) {
        u32 n;
        bool ok;

        sOLTTables[i] = left > 0 ? (OLTTable*)p : 0;
        p += sizeof(OLTTable);
        left -= sizeof(OLTTable);
        FileFlushBuffer(sOLTTables[i], sizeof(OLTTable));
        ok = left > 0 && sOLTTables[i]->count * sizeof(OLTEntry) != 0;
        sOLTTables[i]->entries = ok ? (OLTEntry*)p : 0;
        n = sOLTTables[i]->count * sizeof(OLTEntry);
        p += n;
        left -= n;
        FileFlushBuffer(sOLTTables[i]->entries, n);
    }
    sOLTData = p;
    if (sOLT->flags & 1) {
        for (i = 0; i < sOLT->count; i++) {
            sOLTTables[i]->base = (u32)sOLTData + sOLTTables[i]->base;
            for (j = 0; j < sOLTTables[i]->count; j++) {
                sOLTTables[i]->entries[j].value = (u32)sOLTData + sOLTTables[i]->entries[j].value;
            }
        }
    }
    return 1;
}

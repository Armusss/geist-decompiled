// String table (0x80085E50-0x800866D8): a localized string file loaded whole, with entries looked up
// by an index or by a name hash (31 * h + toupper(c)) through a hash table (0xBAA22E62).
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

extern "C" {
extern unsigned char __upper_map[256];
}

extern "C" void FileFlushBuffer(void* dst, int size);                 // 0x80007088
int FileAddHandler(int type, int (*cb)(void*, int, void*), void* arg); // File.cpp
extern "C" void fn_8000F60C(void* p);
extern "C" void fn_8000F874(void* p);
int AssetFind(u32 table, u32 key);  // 0x8007EB34

inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

struct StrEntry {
    u32 offset;
    u32 extra;
    u32 pad[2];
};

struct StrTable {
    int version;
    u32 numEntries;
    StrEntry* entries;
    u32 numItems;
    void** items;
    u32 stringsSize;
    char* strings;
};

struct StrRef {
    const char* name;
    char* text;
    u32 key;
    u32 extra;
};

StrTable* sStrTable;

inline char* StrGet(int index, u32* extra) {
    u32 offset;

    if (sStrTable == 0) {
        return 0;
    }
    if (index < 0 || index >= sStrTable->numEntries) {
        return 0;
    }
    offset = sStrTable->entries[index].offset;
    if (offset >= sStrTable->stringsSize) {
        return 0;
    }
    if (extra != 0) {
        *extra = sStrTable->entries[index].extra;
    }
    return sStrTable->strings + offset;
}

inline char* StrGetSkip(int index) {
    u32 offset;

    if (sStrTable == 0) {
        return 0;
    }
    if (index < 0 || index >= sStrTable->numEntries) {
        return 0;
    }
    offset = sStrTable->entries[index].offset;
    if (offset >= sStrTable->stringsSize) {
        return 0;
    }
    return sStrTable->strings + offset + 1;
}

inline char* StrFind(u32 key, u32* extra) {
    int index = AssetFind(0xBAA22E62, key);
    if (index == -1) {
        return "ERROR - STRING NOT FOUND";
    }
    return StrGet(index, extra);
}

inline u32 StrHash(const char* s) {
    u32 h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    while (*s != 0) {
        h = h * 31 + (u8)toupper(*s);
        s++;
    }
    return h % 0xFFFFFFFF;
}

inline char* StrFindName(const char* name, u32* extra) {
    if (name == 0) {
        return 0;
    }
    return StrFind(StrHash(name), extra);
}

inline void StrResolve(StrRef* ref) {
    if (ref->key != 0) {
        ref->text = StrFind(ref->key, &ref->extra);
    } else if (ref->name != 0) {
        ref->text = StrFindName(ref->name, &ref->extra);
    }
}

void StrResolveArray(StrRef* refs, int count) {
    int i;

    for (i = 0; i < count; i++) {
        StrResolve(refs);
        refs++;
    }
}

void StrResolveRef(StrRef* ref) { StrResolve(ref); }

char* StrFindKey(u32 key, u32* extra) { return StrFind(key, extra); }

char* StrGetIndex(int index, u32* extra) { return StrGet(index, extra); }

char* StrFindKeySkip(u32 key) {
    int index = AssetFind(0xBAA22E62, key);
    if (index == -1) {
        return "ERROR - STRING NOT FOUND";
    }
    return StrGetSkip(index);
}

char* StrGetIndexSkip(int index) { return StrGetSkip(index); }

void StrTableFree(void) {
    u32 i;

    if (sStrTable != 0) {
        for (i = 0; i < sStrTable->numItems; i++) {
            fn_8000F60C(sStrTable->items[i]);
        }
    }
    sStrTable = 0;
}

void StrTableRelease(void) {
    u32 i;

    if (sStrTable != 0) {
        for (i = 0; i < sStrTable->numItems; i++) {
            fn_8000F874(sStrTable->items[i]);
        }
    }
}

int StrTableLoaded(void* data, int size);

int StrTableLoad(void) {
    FileAddHandler(0x10, (int (*)(void*, int, void*))StrTableLoaded, 0);
    return 1;
}

int StrTableLoaded(void* data, int size) {
    u8* p = (u8*)data;
    int n = size;

    sStrTable = (size > 0) ? (StrTable*)p : 0;
    n -= 0x1C;
    p += 0x1C;
    FileFlushBuffer(sStrTable, 0x1C);

    if (sStrTable->version == 3) {
        sStrTable->entries = (n > 0 && sStrTable->numEntries * sizeof(StrEntry)) ? (StrEntry*)p : 0;
        n -= sStrTable->numEntries * sizeof(StrEntry);
        p += sStrTable->numEntries * sizeof(StrEntry);

        sStrTable->items = (n > 0 && sStrTable->numItems * sizeof(void*)) ? (void**)p : 0;
        n -= sStrTable->numItems * sizeof(void*);
        p += sStrTable->numItems * sizeof(void*);

        sStrTable->strings = (n > 0 && sStrTable->stringsSize) ? (char*)p : 0;
        n -= sStrTable->stringsSize;

        FileFlushBuffer(sStrTable->entries, sStrTable->numEntries * sizeof(StrEntry));
        FileFlushBuffer(sStrTable->items, sStrTable->numItems * sizeof(void*));

        if (n != 0) {
            return 0;
        }
        return 1;
    }
    return 0;
}

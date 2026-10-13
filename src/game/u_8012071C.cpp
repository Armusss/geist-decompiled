// Stat table (0x8012071C-0x80120D98): up to 128 float values keyed by (id, a, b), each with an
// optional change callback. Values can be ranked: all entries with the same (id, a) are sorted by
// value and their b keys returned.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef void (*StatCallback)(void* arg);

extern "C" {
void* memset(void* dst, int val, unsigned long n);
void qsort(void* base, unsigned long n, unsigned long size, int (*cmp)(const void*, const void*));

// Debug print in another file, not named yet.
void fn_800872BC(int channel, const char* format, ...);
}

struct Stat {
    int id;
    float value;
    int a;
    int b;
    char pad10[0x14];
    StatCallback callback;
    void* arg;
    char pad2C[4];
};

struct StatRankEntry {
    int index;
    float value;
};

static Stat sStats[128];
static int sStatCount;

int StatRankDesc(const void* a, const void* b);
int StatRankAsc(const void* a, const void* b);

inline int StatFirst(int id, int a) {
    int i;

    for (i = 0; i < sStatCount; i++) {
        if (sStats[i].id == id && sStats[i].a == a) {
            return i;
        }
    }
    return -1;
}

inline int StatNext(int i) {
    int j;

    if (i < 0 || i >= sStatCount) {
        return -1;
    }
    for (j = i + 1; j < sStatCount; j++) {
        if (sStats[j].id == sStats[i].id && sStats[j].a == sStats[i].a) {
            return j;
        }
    }
    return -1;
}

void StatRank(int id, int a, int* out, int max, int mode) {
    StatRankEntry list[16];
    int n = 0;
    int i;
    int k;
    int m;

    for (i = 0; i < max; i++) {
        out[i] = -1;
    }
    k = StatFirst(id, a);
    if (k == -1) {
        return;
    }
    do {
        i = k;
        list[n].index = i;
        list[n].value = sStats[i].value;
        n++;
        k = StatNext(i);
    } while (k != -1);
    if (mode == 0) {
        qsort(list, n, sizeof(StatRankEntry), StatRankAsc);
    } else if (mode == 1) {
        qsort(list, n, sizeof(StatRankEntry), StatRankDesc);
    } else {
        return;
    }
    m = max < n ? max : n;
    for (i = 0; i < m; i++) {
        out[i] = sStats[list[i].index].b;
    }
}

int StatRankDesc(const void* a, const void* b) {
    return ((StatRankEntry*)b)->value - ((StatRankEntry*)a)->value;
}

int StatRankAsc(const void* a, const void* b) {
    return ((StatRankEntry*)a)->value - ((StatRankEntry*)b)->value;
}

void StatSet(int id, int a, int b, float value) {
    int i;

    for (i = 0; i < sStatCount; i++) {
        if (sStats[i].id == id && sStats[i].a == a && sStats[i].b == b) {
            sStats[i].value = value;
            if (sStats[i].callback) {
                sStats[i].callback(sStats[i].arg);
            }
            return;
        }
    }
}

float StatGetDebug(int id, int a, int b) {
    int i;

    for (i = 0; i < sStatCount; i++) {
        if (sStats[i].id == id && sStats[i].a == a && sStats[i].b == b) {
            fn_800872BC(0x20, "[ Get             ] 0x%08x:%d:%d  %3d", id, a, b, (int)sStats[i].value);
            return sStats[i].value;
        }
    }
    return 0.0f;
}

float StatGet(int id, int a, int b) {
    int i;

    for (i = 0; i < sStatCount; i++) {
        if (sStats[i].id == id && sStats[i].a == a && sStats[i].b == b) {
            return sStats[i].value;
        }
    }
    return 0.0f;
}

void StatAdd(int id, int a, int b, float delta) {
    int i;

    for (i = 0; i < sStatCount; i++) {
        if (sStats[i].id == id && sStats[i].a == a && sStats[i].b == b) {
            sStats[i].value += delta;
            // Retail reads the callback from the entry past the end of the table.
            if (sStats[sStatCount].callback) {
                sStats[sStatCount].callback(sStats[sStatCount].arg);
            }
            return;
        }
    }
}

void StatRegister(int id, int a, int b, float value, StatCallback callback, void* arg) {
    int n = sStatCount++;

    sStats[n].id = id;
    sStats[n].value = value;
    sStats[n].a = a;
    sStats[n].b = b;
    sStats[n].callback = callback;
    sStats[n].arg = arg;
}

void StatReset(void) {
    memset(sStats, 0, sizeof(sStats));
    sStatCount = 0;
}

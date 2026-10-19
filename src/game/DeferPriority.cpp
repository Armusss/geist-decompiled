// DeferPriority.cpp: deferred drawing sorted by distance (0x800F0D24-0x800F14A8).
// A draw is queued as (callback, argument) in one of six priority lists, kept sorted by the squared
// distance from the eye point; lists are flushed back to front. Nodes come from a pool of 0x2000.
// Names are descriptive guesses; the code is matched against the retail DOL.

extern "C" void* MemAlloc(int size, const char* file, int line);
extern "C" void MemFree(void* p, const char* file, int line);
void DebugPrintf(const char* format, ...);

extern "C" {
// Callees in other files, not named yet.
void fn_80008AD8(void* state); // save render state
void fn_8000895C(void* state); // restore render state
}

struct Vec3 {
    float x, y, z;
};

struct DeferNode {
    float dist;
    DeferNode* prev;
    DeferNode* next;
    void (*draw)(void*);
    void* arg;
};

class DeferList {
public:
    DeferList();

    DeferNode* head;
    DeferNode* cur;
};

typedef unsigned long u32;

struct RenderState {
    u32 data[6];
};

int sDeferEnabled = 1;
Vec3 sDeferEye = {0.0f, 0.0f, 0.0f};

DeferList sDeferSpare;
static DeferNode* sDeferPool;
static DeferNode* sDeferFree;
DeferList sDeferLists[6];

void DeferInsert(DeferList* list, DeferNode* node);

inline void DeferRun(DeferList* list) {
    while (list->head) {
        list->cur = list->head;
        list->head = list->head->next;
        list->cur->draw(list->cur->arg);
        list->cur->next = sDeferFree;
        sDeferFree = list->cur;
    }
    list->cur = 0;
}

inline void DeferDiscard(DeferList* list) {
    while (list->head) {
        list->cur = list->head;
        list->head = list->head->next;
        list->cur->next = sDeferFree;
        sDeferFree = list->cur;
    }
    list->cur = 0;
}

void DeferFlush5(void) {
    RenderState state;

    if (sDeferEnabled) {
        fn_80008AD8(&state);
        DeferRun(&sDeferLists[5]);
        fn_8000895C(&state);
    } else {
        DeferDiscard(&sDeferLists[5]);
    }
}

void DeferFlush4(void) {
    RenderState state;

    if (sDeferEnabled) {
        fn_80008AD8(&state);
        DeferRun(&sDeferLists[4]);
        fn_8000895C(&state);
    } else {
        DeferDiscard(&sDeferLists[4]);
    }
}

void DeferFlush3(void) {
    RenderState state;

    if (sDeferEnabled) {
        fn_80008AD8(&state);
        DeferRun(&sDeferLists[3]);
        fn_8000895C(&state);
    } else {
        DeferDiscard(&sDeferLists[3]);
    }
}

void DeferFlush0to2(void) {
    RenderState state;
    int i;

    if (sDeferEnabled) {
        fn_80008AD8(&state);
        for (i = 0; i < 3; i++) {
            DeferRun(&sDeferLists[i]);
        }
        fn_8000895C(&state);
    } else {
        int j;
        for (j = 0; j < 3; j++) {
            DeferDiscard(&sDeferLists[j]);
        }
    }
}

void DeferAdd(int priority, Vec3* pos, void (*draw)(void*), void* arg) {
    DeferNode* node;
    float dx, dy, dz;

    if (priority >= 0 && priority < 6) {
        node = sDeferFree;
        if (node) {
            sDeferFree = node->next;
            dy = sDeferEye.y - pos->y;
            dx = sDeferEye.x - pos->x;
            dz = sDeferEye.z - pos->z;
            node->dist = dx * dx + dy * dy + dz * dz;
            node->draw = draw;
            node->arg = arg;
            DeferInsert(&sDeferLists[priority], node);
        } else {
            DebugPrintf("DeferPriority.cpp: Too many deferred nodes\n");
        }
    }
}

void DeferSetEye(Vec3* eye) { sDeferEye = *eye; }

void DeferNop(void) {}

void DeferShutdown(void) {
    if (sDeferPool) {
        MemFree(sDeferPool, "DeferPriority.cpp", 184);
    }
}

void DeferInit(void) {
    int i;

    sDeferPool = (DeferNode*)MemAlloc(0x2000 * sizeof(DeferNode), "DeferPriority.cpp", 167);
    sDeferFree = 0;
    for (i = 0; i < 0x2000; i++) {
        sDeferPool[i].next = sDeferFree;
        sDeferFree = &sDeferPool[i];
    }
}

void DeferInsert(DeferList* list, DeferNode* node) {
    if (list->head == 0) {
        list->head = node;
        list->cur = node;
        node->prev = 0;
        node->next = 0;
        return;
    }

    if (node->dist > list->cur->dist) {
        while (list->cur->prev) {
            if (list->cur->prev->dist > node->dist) {
                node->prev = list->cur->prev;
                list->cur->prev->next = node;
                node->next = list->cur;
                list->cur->prev = node;
                return;
            }
            list->cur = list->cur->prev;
        }
        list->head = node;
        node->prev = 0;
        node->next = list->cur;
        list->cur->prev = node;
        return;
    }

    while (list->cur->next) {
        if (list->cur->next->dist < node->dist) {
            node->next = list->cur->next;
            list->cur->next->prev = node;
            node->prev = list->cur;
            list->cur->next = node;
            return;
        }
        list->cur = list->cur->next;
    }
    list->cur->next = node;
    node->prev = list->cur;
    node->next = 0;
}

DeferList::DeferList() {
    head = 0;
    cur = 0;
}

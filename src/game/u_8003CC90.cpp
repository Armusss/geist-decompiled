// u_8003CC90.cpp: model table (0x8003CC90-0x800448D0).
// The model counterpart of the texture table in u_8000F2F0.cpp: entries of 0x68 bytes with a list
// of texture ids and a disc streaming slot, load / free / prefetch by id, plus the larger model
// loading, bone and instance code that follows.  Only the small id and bookkeeping functions are
// drafted so far; the rest are declared with TODO comments.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
typedef unsigned short u16;

// Functions called from other units under their placeholder names (C linkage).
struct ModelEntry;
extern "C" void fn_8003CE8C(int* resident, int* total);
extern "C" void fn_8003D710(unsigned long id);
extern "C" void fn_8003D7A0(short* ids, int n);
extern "C" void fn_8003D8F4(unsigned long id);
extern "C" int fn_8003DEFC(ModelEntry* e);
extern "C" void fn_8003E5AC(void);
extern "C" void fn_8003E5B0();
extern "C" void fn_8003F0D4(unsigned long id);

struct StreamEntry;

// Model table entry (0x68 bytes).
struct ModelEntry {
    int texCount;       // 0x00
    char pad04[0x28];
    int data;           // 0x2C: loaded data pointer
    char pad30[4];
    short state;        // 0x34: stream slot state (0 free, 1 queued, 2 requested, 4 resident)
    char prio;          // 0x36
    char pad37;
    int unk38;
    int size;           // 0x3C
    char pad40[4];
    int hasData;        // 0x44
    char pad48[0x68 - 0x48];
};

// The texture id list lives at +0x30 (index 0xC of the word view), the stream entry at +0x34.
struct ModelEntryView {
    int texCount;       // 0x00
    char pad04[0x2C];
    int* texIds;        // 0x30
    char stream[4];     // 0x34
};

void StreamRead(int pri, StreamEntry* e, void (*done)(int, void*, void*), void* arg);
void GunStateTimerTick8017BAD0(void* p);
void GunStateStep8017BBBC(void* p);

extern "C" {
void fn_8000F60C(int id);
void fn_8000F7E4(int id, char prio);
void fn_8000F874(int id);
int fn_80049940(void);
void fn_8009C87C(void);
// TODO (not drafted yet, 0x398 bytes at 0x8004256C): release a model instance
void fn_8004256C(u32 id, int flag);
}

extern ModelEntry* lbl_804D4340;
extern int lbl_804D4350;
extern int lbl_804D4354;
extern int lbl_804D4358;
extern int lbl_804D26D0;
extern int lbl_804D26C4;
extern int lbl_804D4330;
extern int lbl_8040E018[15];
extern int lbl_80410154[12];
extern u32 lbl_8040E054[64];

// TODO (not drafted yet, 0x1F8 bytes at 0x8003CC90): unknown
// (0x8003CE88 below is the empty function.)
void ModelNop(void) {}

// Total and resident streaming memory.
void fn_8003CE8C(int* resident, int* total) {
    int r = 0;
    int t = 0;
    ModelEntry* e = lbl_804D4340;
    int n = lbl_804D4350;

    if (n > 0) {
        do {
            t += e->size;
            if (e->state == 1 || e->state == 4) {
                r += e->size;
            }
            e++;
            n--;
        } while (n != 0);
    }
    if (resident != 0) {
        *resident = r;
    }
    if (total != 0) {
        *total = t;
    }
}

void ModelSlotDone(int result, void* arg, void* entry);

// Queue every model whose load was requested.
void ModelFlushRequests() {
    if (lbl_804D4354 != 0 && lbl_804D26D0 != 0) {
        lbl_804D4354 = 0;
        for (int i = 0; i < lbl_804D4350; i++) {
            ModelEntry* e = &lbl_804D4340[i];
            if (e->state == 2) {
                e->state = 1;
                StreamRead(0x2B, (StreamEntry*)&e->state, ModelSlotDone, e);
            }
        }
    }
}

// TODO (not drafted yet, 0x4CC bytes at 0x8003D1A0): stream completion callback (ModelSlotDone), chains the texture loads
// TODO (not drafted yet, 0x1F8 bytes at 0x8003CF98): unknown

void ModelFreeIds(short* ids, int n) {
    for (int i = 0; i < n; i++) {
        if ((int)(u16)*ids < lbl_804D4350) {
            lbl_804D4358 = 1;
            ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[(u16)*ids];
            GunStateTimerTick8017BAD0(e->stream);
            for (int j = 0; j < e->texCount; j++) {
                fn_8000F60C(e->texIds[j]);
            }
        }
        ids++;
    }
}

void fn_8003D710(u32 id) {
    if ((int)(id & 0xFFFF) < lbl_804D4350) {
        lbl_804D4358 = 1;
        ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[id & 0xFFFF];
        GunStateTimerTick8017BAD0(e->stream);
        for (int j = 0; j < e->texCount; j++) {
            fn_8000F60C(e->texIds[j]);
        }
    }
}

void fn_8003D7A0(short* ids, int n) {
    for (int i = 0; i < n; i++) {
        if ((int)(u16)*ids < lbl_804D4350) {
            lbl_804D4354 = 1;
            ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[(u16)*ids];
            ((ModelEntry*)e)->prio = 1;
            GunStateStep8017BBBC(e->stream);
            for (int j = 0; j < e->texCount; j++) {
                fn_8000F7E4(e->texIds[j], 1);
            }
        }
        ids++;
    }
}

void ModelLoadIds(short* ids, int n) {
    for (int i = 0; i < n; i++) {
        if ((int)(u16)*ids < lbl_804D4350) {
            lbl_804D4354 = 1;
            ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[(u16)*ids];
            GunStateStep8017BBBC(e->stream);
            ((ModelEntry*)e)->prio = 0;
            for (int j = 0; j < e->texCount; j++) {
                fn_8000F874(e->texIds[j]);
            }
        }
        ids++;
    }
}

void fn_8003D8F4(u32 id) {
    if ((int)(id & 0xFFFF) < lbl_804D4350) {
        lbl_804D4354 = 1;
        ModelEntryView* e = (ModelEntryView*)&lbl_804D4340[id & 0xFFFF];
        GunStateStep8017BBBC(e->stream);
        ((ModelEntry*)e)->prio = 0;
        for (int j = 0; j < e->texCount; j++) {
            fn_8000F874(e->texIds[j]);
        }
    }
}

// TODO (not drafted yet, 0x22C bytes at 0x8003D988): unknown

void ModelSaveBlockA(int* src) {
    for (int i = 0; i < 15; i++) {
        lbl_8040E018[i] = src[i];
    }
}

void ModelLoadBlockA(int* dst) {
    for (int i = 0; i < 15; i++) {
        dst[i] = lbl_8040E018[i];
    }
}

// TODO (not drafted yet, 0x240 bytes at 0x8003DCBC): unknown

int fn_8003DEFC(ModelEntry* e) {
    if (e->hasData != 0) {
        return e->data;
    }
    return 0;
}

void ModelSaveBlockB(int* src) {
    for (int i = 0; i < 12; i++) {
        lbl_80410154[i] = src[i];
    }
}

// TODO (not drafted yet, 0x17C bytes at 0x8003DF80): unknown
// TODO (not drafted yet, 0xCC bytes at 0x8003E0FC): unknown
// TODO (not drafted yet, 0x2CC bytes at 0x8003E1C8): unknown
// TODO (not drafted yet, 0x118 bytes at 0x8003E494): unknown
void fn_8003E5AC(void) {}

void fn_8003E5B0() {
    if (lbl_804D26C4 != 0) {
        u32* p = lbl_8040E054;
        for (int i = 0; i < lbl_804D4330; i++) {
            fn_8004256C(*p, 1);
            p++;
        }
        lbl_804D4330 = 0;
    }
}

// TODO (not drafted yet, 0x2BC bytes at 0x8003E620): unknown
// TODO (not drafted yet, 0x7F8 bytes at 0x8003E8DC): unknown

void fn_8003F0D4(u32 id) {
    if (fn_80049940() == 0 && lbl_804D4340 != 0 && (int)(id & 0xFFFF) < lbl_804D4350) {
        ModelEntry* e = &lbl_804D4340[id & 0xFFFF];
        int d;
        if (e->hasData == 0) {
            d = 0;
        } else {
            d = e->data;
        }
        if (d != 0) {
            fn_8009C87C();
        }
    }
}

// TODO (not drafted yet, 0x9FC bytes at 0x8003F15C): unknown
// TODO (not drafted yet, 0x414 bytes at 0x8003FB58): unknown
// TODO (not drafted yet, 0x694 bytes at 0x8003FF6C): unknown
// 0x80040600 (4 bytes) is another empty function; 0x80040604 .. 0x8004393C: not drafted yet:
// TODO (not drafted yet, 0x58 bytes at 0x80040604)
// TODO (not drafted yet, 0x25C bytes at 0x8004065C)
// TODO (not drafted yet, 0x20 bytes at 0x800408B8)
// TODO (not drafted yet, 0x19C bytes at 0x800408D8)
// TODO (not drafted yet, 0x6C bytes at 0x80040A74)
// TODO (not drafted yet, 0xDBC bytes at 0x80040AE0)
// TODO (not drafted yet, 0x2C8 bytes at 0x8004189C)
// TODO (not drafted yet, 0x22C bytes at 0x80041B64)
// TODO (not drafted yet, 0x504 bytes at 0x80041D90)
// TODO (not drafted yet, 0x2D8 bytes at 0x80042294)
// TODO (not drafted yet, 0x398 bytes at 0x8004256C)
// TODO (not drafted yet, 0x8C bytes at 0x80042904)
// TODO (not drafted yet, 0xB8 bytes at 0x80042990)
// TODO (not drafted yet, 0xB8 bytes at 0x80042A48)
// TODO (not drafted yet, 0xB4 bytes at 0x80042B00)
// TODO (not drafted yet, 0x364 bytes at 0x80042BB4)
// TODO (not drafted yet, 0x290 bytes at 0x80042F18)
// TODO (not drafted yet, 0x64 bytes at 0x800431A8)
// TODO (not drafted yet, 0x64 bytes at 0x8004320C)
// TODO (not drafted yet, 0x28C bytes at 0x80043270)
// TODO (not drafted yet, 0x440 bytes at 0x800434FC)
// TODO (not drafted yet, 0xF94 bytes at 0x8004393C)

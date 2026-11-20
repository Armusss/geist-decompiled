// u_8008DCDC.cpp (0x8008DCDC-0x80093BF0): the animation system: a table of 0x50-byte animation entries
// (lbl_804D4718: reference counting, apply/blend, clip lookup), a 300-entry table of 0x28-byte records
// (lbl_804D4724) and the 0x5C-byte record array helpers at the end.
// Boundary evidence (the end cut at 0x80093BF0; the next file is u_80093BF0.cpp, 2D drawing):
// - .sdata2: this file's pool is 0x804D6938-0x804D6960 (0.0f, u32 int-to-float double, 1.0f, -1.0f, 2^-15,
//   s32 int-to-float double); the next file has its own 0.0f/1.0f/doubles (0x804D69BC/0x804D69C0/0x804D69D0/
//   0x804D6A10). fn_80093B3C is the last reader of this pool, fn_80093BF0 the first of the next.
// - .sbss: only 0x804D4718-0x804D4727 is touched here; the next file uses 0x804D4728-0x804D4763.
// - fn_80093BCC (empty destructor loop over 0x5C-byte records, no data) is the partner of fn_80093B3C (the
//   constructor loop, which reads this pool's 0.0f) and is called from fn_80093AC4, so it stays here.
// G4 note: fn_80092E88 has the GC/2.0 pre-stwu load fingerprint (lwz r6,0x24(r3) / lfs before stwu); the file
// stays on GC/1.3.2 (per-file compiler changes need the user's approval).
// Only the small accessors are drafted so far; every larger function is declared with a TODO.
// Names are descriptive guesses; unmatched against retail unless noted.

typedef unsigned char u8;
typedef unsigned short u16;

// One animation table entry (0x50 bytes; only the fields used here).
struct AnimEntry {
    u16 type;                 // 0x00 (1 = loaded clip)
    u8 flag;                  // 0x02
    u8 pad03[0x20 - 0x03];    // 0x03
    void* data;               // 0x20
    u8 pad24[0x40 - 0x24];    // 0x24
    u16 unk40;                // 0x40
    u8 pad42[0x50 - 0x42];    // 0x42
};

struct AnimTable {
    u8 pad00[0x08];           // 0x00
    int count;                // 0x08
    AnimEntry* entries;       // 0x0C
};

extern AnimTable* lbl_804D4718;
extern int lbl_804D4720;
extern int lbl_804D471C;

void GunStateTimerTick8017BAD0(void* p);   // 0x8017BAD0
void GunStateStep8017BBBC(void* p);        // 0x8017BBBC
void GunStateAbort8017BB74(void* p);       // 0x8017BB74
void GunStateAbort8017BB98(void* p);       // 0x8017BB98

// TODO (not drafted yet, 0x4F0 bytes at 0x8008DE4C): animation start helper called by the wrapper below
extern "C" void fn_8008DE4C(int a, int b, int c, int d);

// TODO (not drafted yet, 0x14C bytes at 0x8008DCDC)
extern "C" void fn_8008DCDC();

// Calls fn_8008DE4C with the last argument cleared.
extern "C" void fn_8008DE28(int a, int b, int c);
void fn_8008DE28(int a, int b, int c) {
    fn_8008DE4C(a, b, c, 0);
}

// TODO (not drafted yet, 0x290 bytes at 0x8008E33C): already named Fn8008E33C__Fiif in symbols.txt; real C++ signature still to be written
// Fn8008E33C__Fiif
// TODO (not drafted yet, 0x204 bytes at 0x8008E5CC)
extern "C" void fn_8008E5CC();

// Returns whether animation index i is a loaded clip with data.
extern "C" int fn_8008E7D0(int i);
int fn_8008E7D0(int i) {
    AnimTable* t = lbl_804D4718;
    u8 ok = 0;
    if (t != 0 && i >= 0 && i < t->count) {
        if (t->entries[i].type == 1 && t->entries[i].data != 0) {
            ok = 1;
        }
    }
    return ok;
}

// TODO (not drafted yet, 0x140 bytes at 0x8008E824)
extern "C" void fn_8008E824();
// TODO (not drafted yet, 0x138 bytes at 0x8008E964)
extern "C" void fn_8008E964();
// TODO (not drafted yet, 0x144 bytes at 0x8008EA9C)
extern "C" void fn_8008EA9C();
// TODO (not drafted yet, 0x150 bytes at 0x8008EBE0)
extern "C" void fn_8008EBE0();
// TODO (not drafted yet, 0xEC bytes at 0x8008ED30)
extern "C" void fn_8008ED30();
// TODO (not drafted yet, 0x238 bytes at 0x8008EE1C)
extern "C" void fn_8008EE1C();

// Returns the 16-bit field at +0x40 of animation i, or 0.
extern "C" int fn_8008F054(int i);
int fn_8008F054(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        return t->entries[i].unk40;
    }
    return 0;
}

// TODO (not drafted yet, 0x5C bytes at 0x8008F090)
extern "C" void fn_8008F090();
// TODO (not drafted yet, 0x144 bytes at 0x8008F0EC)
extern "C" void fn_8008F0EC();
// TODO (not drafted yet, 0x148 bytes at 0x8008F230)
extern "C" void fn_8008F230();
// TODO (not drafted yet, 0x170 bytes at 0x8008F378)
extern "C" void fn_8008F378();

// Empty hooks called from many units.
extern "C" void fn_8008F4E8();
void fn_8008F4E8() {}
extern "C" void fn_8008F4EC();
void fn_8008F4EC() {}

// TODO (not drafted yet, 0x140 bytes at 0x8008F4F0)
extern "C" void fn_8008F4F0();
// TODO (not drafted yet, 0x294 bytes at 0x8008F630): already named AnimApply__FiPcii in symbols.txt; real C++ signature still to be written
// AnimApply__FiPcii
// TODO (not drafted yet, 0x314 bytes at 0x8008F8C4)
extern "C" void fn_8008F8C4();
// TODO (not drafted yet, 0x424 bytes at 0x8008FBD8)
extern "C" void fn_8008FBD8();
// TODO (not drafted yet, 0x13C bytes at 0x8008FFFC)
extern "C" void fn_8008FFFC();
// TODO (not drafted yet, 0x74 bytes at 0x80090138)
extern "C" void fn_80090138();
// TODO (not drafted yet, 0x144 bytes at 0x800901AC): already named Fn800901AC__Fii in symbols.txt; real C++ signature still to be written
// Fn800901AC__Fii
// TODO (not drafted yet, 0xA0 bytes at 0x800902F0)
extern "C" void fn_800902F0();
// TODO (not drafted yet, 0x160 bytes at 0x80090390)
extern "C" void fn_80090390();
// TODO (not drafted yet, 0x144 bytes at 0x800904F0)
extern "C" void fn_800904F0();
// TODO (not drafted yet, 0x144 bytes at 0x80090634)
extern "C" void fn_80090634();
// TODO (not drafted yet, 0x1FC bytes at 0x80090778)
extern "C" void fn_80090778();
// TODO (not drafted yet, 0x200 bytes at 0x80090974)
extern "C" void fn_80090974();
// TODO (not drafted yet, 0x1F8 bytes at 0x80090B74)
extern "C" void fn_80090B74();
// TODO (not drafted yet, 0x8C bytes at 0x80090D6C)
extern "C" void fn_80090D6C();
// TODO (not drafted yet, 0xAC bytes at 0x80090DF8)
extern "C" void fn_80090DF8();
// TODO (not drafted yet, 0x258 bytes at 0x80090EA4)
extern "C" void fn_80090EA4();
// TODO (not drafted yet, 0x2F0 bytes at 0x800910FC): already named Fn800910FC__Fiif in symbols.txt; real C++ signature still to be written
// Fn800910FC__Fiif
// TODO (not drafted yet, 0xC0 bytes at 0x800913EC)
extern "C" void fn_800913EC();
// TODO (not drafted yet, 0x2F4 bytes at 0x800914AC)
extern "C" void fn_800914AC();
// TODO (not drafted yet, 0x6C bytes at 0x800917A0)
extern "C" void fn_800917A0();
// TODO (not drafted yet, 0x3E8 bytes at 0x8009180C)
extern "C" void fn_8009180C();
// TODO (not drafted yet, 0x26C bytes at 0x80091BF4)
extern "C" void fn_80091BF4();
// TODO (not drafted yet, 0x5C8 bytes at 0x80091E60)
extern "C" void fn_80091E60();
// TODO (not drafted yet, 0x1A0 bytes at 0x80092428)
extern "C" void fn_80092428();
// TODO (not drafted yet, 0x3F8 bytes at 0x800925C8)
extern "C" void fn_800925C8();
// TODO (not drafted yet, 0x4C8 bytes at 0x800929C0)
extern "C" void fn_800929C0();
// TODO (not drafted yet, 0x1D4 bytes at 0x80092E88)
extern "C" void fn_80092E88();
// TODO (not drafted yet, 0x74 bytes at 0x8009305C)
extern "C" void fn_8009305C();
// TODO (not drafted yet, 0xB0 bytes at 0x800930D0)
extern "C" void fn_800930D0();
// TODO (not drafted yet, 0xD0 bytes at 0x80093180)
extern "C" void fn_80093180();
// TODO (not drafted yet, 0x12C bytes at 0x80093250)
extern "C" void fn_80093250();

// Aborts animation i (variant A).
void AnimAbortA(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D4720 = 1;
        GunStateAbort8017BB74(&t->entries[i]);
    }
}

// Aborts animation i (variant B).
void AnimAbortB(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D471C = 1;
        GunStateAbort8017BB98(&t->entries[i]);
    }
}

// Ticks animation i's timer.
void AnimUnref(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D4720 = 1;
        GunStateTimerTick8017BAD0(&t->entries[i]);
    }
}

// Sets animation i's byte at +2 and steps it.
extern "C" void fn_80093478(int i, u8 flag);
void fn_80093478(int i, u8 flag) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D471C = 1;
        AnimEntry* e = &t->entries[i];
        e->flag = flag;
        GunStateStep8017BBBC(e);
    }
}

// Starts animation i and clears its byte at +2.
void AnimRef(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D471C = 1;
        AnimEntry* e = &t->entries[i];
        e->flag = 0;
        GunStateStep8017BBBC(e);
    }
}

// TODO (not drafted yet, 0x1E4 bytes at 0x8009352C)
extern "C" void fn_8009352C();
// TODO (not drafted yet, 0x1B0 bytes at 0x80093710)
extern "C" void fn_80093710();

// Empty hook.
extern "C" void fn_800938C0();
void fn_800938C0() {}

// TODO (not drafted yet, 0xCC bytes at 0x800938C4)
extern "C" void fn_800938C4();
// TODO (not drafted yet, 0x74 bytes at 0x80093990)
extern "C" void fn_80093990();
// TODO (not drafted yet, 0x68 bytes at 0x80093A04)
extern "C" void fn_80093A04();
// TODO (not drafted yet, 0x58 bytes at 0x80093A6C)
extern "C" void fn_80093A6C();
// TODO (not drafted yet, 0x70 bytes at 0x80093AC4)
extern "C" void fn_80093AC4();

// Size of one 0x5C-byte record.
int AnimRecordSize() {
    return 0x5C;
}

// TODO (not drafted yet, 0x90 bytes at 0x80093B3C)
extern "C" void fn_80093B3C();


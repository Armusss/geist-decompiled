// u_8008DCDC.cpp: the animation system (0x8008DCDC-0x8009C74C): an animation table of 0x50-byte entries
// (reference counting, apply/blend, clip lookup) plus the 2D draw-rect helpers further down.
// Only the small accessors are drafted so far; every larger function is declared with a TODO.
// Names are descriptive guesses; unmatched against retail so far.

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
extern int lbl_804D475C;
extern int lbl_804D4760;
extern float lbl_804D4728;
extern float lbl_804D472C;
extern float lbl_804D2A50;
extern float lbl_804D2A54;
extern float lbl_804D2A58;
extern float lbl_804D2A5C;

void GunStateTimerTick8017BAD0(void* p);   // 0x8017BAD0
void GunStateStep8017BBBC(void* p);        // 0x8017BBBC
void GunStateAbort8017BB74(void* p);       // 0x8017BB74
void GunStateAbort8017BB98(void* p);       // 0x8017BB98

// TODO (not drafted yet, 0x4F0 bytes at 0x8008DE4C): animation start helper called by the wrapper below
extern "C" void fn_8008DE4C(int a, int b, int c, int d);

// Calls fn_8008DE4C with the last argument cleared.
extern "C" void fn_8008DE28(int a, int b, int c);
void fn_8008DE28(int a, int b, int c) {
    fn_8008DE4C(a, b, c, 0);
}

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

// Returns the 16-bit field at +0x40 of animation i, or 0.
extern "C" int fn_8008F054(int i);
int fn_8008F054(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        return t->entries[i].unk40;
    }
    return 0;
}

// Empty hooks called from many units.
extern "C" void fn_8008F4E8();
void fn_8008F4E8() {}
extern "C" void fn_8008F4EC();
void fn_8008F4EC() {}
extern "C" void fn_800938C0();
void fn_800938C0() {}

// Size of one 0x5C-byte record.
int AnimRecordSize() {
    return 0x5C;
}

// Clears the two cached words.
extern "C" void fn_8009425C();
void fn_8009425C() {
    lbl_804D475C = 0;
    lbl_804D4760 = 0;
}

// Stores a rectangle (x0, y0, x1, y1) as origin and size.
void AnimSetRect(float x0, float y0, float x1, float y1) {
    lbl_804D4728 = x0;
    lbl_804D472C = y0;
    lbl_804D2A50 = x1;
    lbl_804D2A54 = y1;
    lbl_804D2A58 = x1 - x0;
    lbl_804D2A5C = y1 - y0;
}


// Ticks animation i's timer.
void AnimUnref(int i) {
    AnimTable* t = lbl_804D4718;
    if (t != 0 && i >= 0 && i < t->count) {
        lbl_804D4720 = 1;
        GunStateTimerTick8017BAD0(&t->entries[i]);
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

// TODO (not drafted yet, 0x14C bytes at 0x8008DCDC)
extern "C" void fn_8008DCDC();
// TODO (not drafted yet, 0x290 bytes at 0x8008E33C): already named Fn8008E33C__Fiif in symbols.txt; real C++ signature still to be written
// Fn8008E33C__Fiif
// TODO (not drafted yet, 0x204 bytes at 0x8008E5CC)
extern "C" void fn_8008E5CC();
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
// TODO (not drafted yet, 0x5C bytes at 0x8008F090)
extern "C" void fn_8008F090();
// TODO (not drafted yet, 0x144 bytes at 0x8008F0EC)
extern "C" void fn_8008F0EC();
// TODO (not drafted yet, 0x148 bytes at 0x8008F230)
extern "C" void fn_8008F230();
// TODO (not drafted yet, 0x170 bytes at 0x8008F378)
extern "C" void fn_8008F378();
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
// TODO (not drafted yet, 0x1E4 bytes at 0x8009352C)
extern "C" void fn_8009352C();
// TODO (not drafted yet, 0x1B0 bytes at 0x80093710)
extern "C" void fn_80093710();
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
// TODO (not drafted yet, 0x90 bytes at 0x80093B3C)
extern "C" void fn_80093B3C();
// TODO (not drafted yet, 0x128 bytes at 0x80093BF0)
extern "C" void fn_80093BF0();
// TODO (not drafted yet, 0xCC bytes at 0x80093D18)
extern "C" void fn_80093D18();
// TODO (not drafted yet, 0x1A4 bytes at 0x80093DE4)
extern "C" void fn_80093DE4();
// TODO (not drafted yet, 0xA8 bytes at 0x80093F88)
extern "C" void fn_80093F88();
// TODO (not drafted yet, 0x120 bytes at 0x80094030)
extern "C" void fn_80094030();
// TODO (not drafted yet, 0x10C bytes at 0x80094150)
extern "C" void fn_80094150();
// TODO (not drafted yet, 0x170 bytes at 0x8009426C)
extern "C" void fn_8009426C();
// TODO (not drafted yet, 0xFC bytes at 0x800943DC)
extern "C" void fn_800943DC();
// TODO (not drafted yet, 0xF4 bytes at 0x800944D8)
extern "C" void fn_800944D8();
// TODO (not drafted yet, 0xFC bytes at 0x800945CC)
extern "C" void fn_800945CC();
// TODO (not drafted yet, 0xFC bytes at 0x800946C8)
extern "C" void fn_800946C8();
// TODO (not drafted yet, 0x2F8 bytes at 0x800947C4)
extern "C" void fn_800947C4();
// TODO (not drafted yet, 0x118 bytes at 0x80094ABC)
extern "C" void fn_80094ABC();
// TODO (not drafted yet, 0x118 bytes at 0x80094BD4)
extern "C" void fn_80094BD4();
// TODO (not drafted yet, 0x134 bytes at 0x80094CEC)
extern "C" void fn_80094CEC();
// TODO (not drafted yet, 0x160 bytes at 0x80094E20)
extern "C" void fn_80094E20();
// TODO (not drafted yet, 0x148 bytes at 0x80094F80)
extern "C" void fn_80094F80();
// TODO (not drafted yet, 0x148 bytes at 0x800950C8)
extern "C" void fn_800950C8();
// TODO (not drafted yet, 0x134 bytes at 0x80095210)
extern "C" void fn_80095210();
// TODO (not drafted yet, 0x14C bytes at 0x80095344)
extern "C" void fn_80095344();
// TODO (not drafted yet, 0xE0 bytes at 0x80095490)
extern "C" void fn_80095490();
// TODO (not drafted yet, 0x134 bytes at 0x80095570)
extern "C" void fn_80095570();
// TODO (not drafted yet, 0x120 bytes at 0x800956A4)
extern "C" void fn_800956A4();
// TODO (not drafted yet, 0x108 bytes at 0x800957C4)
extern "C" void fn_800957C4();
// TODO (not drafted yet, 0xFC bytes at 0x800958CC)
extern "C" void fn_800958CC();
// TODO (not drafted yet, 0x110 bytes at 0x800959C8)
extern "C" void fn_800959C8();
// TODO (not drafted yet, 0x288 bytes at 0x80095AD8)
extern "C" void fn_80095AD8();
// TODO (not drafted yet, 0xC8 bytes at 0x80095D60)
extern "C" void fn_80095D60();
// TODO (not drafted yet, 0x378 bytes at 0x80095E28)
extern "C" void fn_80095E28();
// TODO (not drafted yet, 0xF8 bytes at 0x800961A0)
extern "C" void fn_800961A0();
// TODO (not drafted yet, 0xF8 bytes at 0x80096298)
extern "C" void fn_80096298();
// TODO (not drafted yet, 0x1F4 bytes at 0x80096390)
extern "C" void fn_80096390();
// TODO (not drafted yet, 0x234 bytes at 0x80096584)
extern "C" void fn_80096584();
// TODO (not drafted yet, 0x378 bytes at 0x800967B8)
extern "C" void fn_800967B8();
// TODO (not drafted yet, 0x144 bytes at 0x80096B30)
extern "C" void fn_80096B30();
// TODO (not drafted yet, 0xE8 bytes at 0x80096C74)
extern "C" void fn_80096C74();
// TODO (not drafted yet, 0x74 bytes at 0x80096D5C)
extern "C" void fn_80096D5C();
// TODO (not drafted yet, 0x98C bytes at 0x80096DD0)
extern "C" void fn_80096DD0();
// TODO (not drafted yet, 0x580 bytes at 0x8009775C)
extern "C" void fn_8009775C();
// TODO (not drafted yet, 0xF4 bytes at 0x80097CDC)
extern "C" void fn_80097CDC();
// TODO (not drafted yet, 0x128 bytes at 0x80097DD0)
extern "C" void fn_80097DD0();
// TODO (not drafted yet, 0xF8 bytes at 0x80097EF8)
extern "C" void fn_80097EF8();
// TODO (not drafted yet, 0x108 bytes at 0x80097FF0)
extern "C" void fn_80097FF0();
// TODO (not drafted yet, 0x14C bytes at 0x800980F8)
extern "C" void fn_800980F8();
// TODO (not drafted yet, 0x1B8 bytes at 0x80098244)
extern "C" void fn_80098244();
// TODO (not drafted yet, 0x120 bytes at 0x800983FC)
extern "C" void fn_800983FC();
// TODO (not drafted yet, 0x120 bytes at 0x8009851C)
extern "C" void fn_8009851C();
// TODO (not drafted yet, 0x140 bytes at 0x8009863C)
extern "C" void fn_8009863C();
// TODO (not drafted yet, 0x12C bytes at 0x8009877C)
extern "C" void fn_8009877C();
// TODO (not drafted yet, 0x104 bytes at 0x800988A8)
extern "C" void fn_800988A8();
// TODO (not drafted yet, 0x280 bytes at 0x800989AC)
extern "C" void fn_800989AC();
// TODO (not drafted yet, 0x388 bytes at 0x80098C2C)
extern "C" void fn_80098C2C();
// TODO (not drafted yet, 0x284 bytes at 0x80098FB4)
extern "C" void fn_80098FB4();
// TODO (not drafted yet, 0xB18 bytes at 0x8009925C)
extern "C" void fn_8009925C();
// TODO (not drafted yet, 0x1A4 bytes at 0x80099D74)
extern "C" void fn_80099D74();
// TODO (not drafted yet, 0x668 bytes at 0x80099F18)
extern "C" void fn_80099F18();
// TODO (not drafted yet, 0x848 bytes at 0x8009A580)
extern "C" void fn_8009A580();
// TODO (not drafted yet, 0xB78 bytes at 0x8009ADC8)
extern "C" void fn_8009ADC8();
// TODO (not drafted yet, 0xB64 bytes at 0x8009B940)
extern "C" void fn_8009B940();
// TODO (not drafted yet, 0x228 bytes at 0x8009C4A4)
extern "C" void fn_8009C4A4();
// TODO (not drafted yet, 0x80 bytes at 0x8009C6CC)
extern "C" void fn_8009C6CC();

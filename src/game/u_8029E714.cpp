// u_8029E714.cpp: game-wide world / actor-table services (0x8029E714-0x802A770C).
// A large unit of loosely related code; only the small accessors (frame counters, level-state
// flags, the actor-table lookup and the time-scale values) are drafted so far.  Every larger
// function is declared with a TODO comment.
// Names are descriptive guesses; unmatched against retail so far.

typedef unsigned long u32;
struct Actor;

extern int lbl_804D5728;
extern float lbl_804D5720;
extern int lbl_804D5724;
extern int lbl_804D572C;
extern int lbl_804D5730;
extern int lbl_804D5734;
extern int lbl_804A46F8;
extern int lbl_804A46F4[];
extern int lbl_804D5738;
extern int lbl_804D573C;
extern float lbl_804D3860;
extern float lbl_804D3864;
extern int lbl_804D3868;
extern char lbl_804A4598[];

// Functions called from other units under their placeholder names (C linkage).
extern "C" int fn_802A1D04(void);
extern "C" void fn_802A1D0C(int v);
extern "C" int fn_802A1D9C(void);
extern "C" float fn_802A1DA4(void);
extern "C" int fn_802A1DAC(void);
extern "C" int fn_802A1DB4(void);
extern "C" void fn_802A1D80(void);
extern "C" char* fn_802A1FA0(void);
extern "C" void fn_802A2048(int base, int valid);
extern "C" void fn_802A2054(float v);
extern "C" void fn_802A2078(float v);
extern "C" float fn_802A2080(void);
extern "C" void fn_802A209C(int v);
extern "C" void fn_802A3BEC();


int fn_802A1D04() {
    return lbl_804D5728;
}

void fn_802A1D0C(int v) {
    lbl_804D5728 = v;
}


void FrameCounterAdd(float dt) {
    lbl_804D5724++;
    lbl_804D5720 = (float)((double)lbl_804D5720 + (double)dt);
}

void FrameCounterReset() {
    lbl_804D5724 = 0;
    lbl_804D5720 = 0.0f;
}

void LevelFlagRestore() {
    lbl_804D572C = lbl_804D5730;
}

void LevelFlagsInit() {
    lbl_804D5730 = 1;
    lbl_804A46F8 = 1;
    lbl_804D572C = 1;
    lbl_804D5734 = 1;
}

int FrameCountGet() {
    return lbl_804D5724;
}

float fn_802A1DA4() {
    return lbl_804D5720;
}

int fn_802A1DAC() {
    return lbl_804D5730;
}

int LevelFlagCurrent() {
    return lbl_804D572C;
}

void LevelFlagPop() {
    lbl_804D5730 = lbl_804A46F4[lbl_804D5734];
    if (lbl_804D5734 < 2) {
        return;
    }
    lbl_804D5734--;
}


char* fn_802A1FA0() {
    return lbl_804A4598;
}


struct Actor;
Actor* ActorGet(int index) {
    if (lbl_804D573C == 0) {
        return 0;
    }
    return (Actor*)(lbl_804D5738 + index * 0x2800);
}

void ActorTableSet(int base, int valid) {
    lbl_804D5738 = base;
    lbl_804D573C = valid;
}

void fn_802A2054(float v) {
    lbl_804D3864 = v;
}

void fn_802A2078(float v) {
    lbl_804D3860 = v;
}

float fn_802A2080() {
    if (lbl_804D3868 != 0) {
        return lbl_804D3860;
    }
    return 1.0f;
}

void fn_802A209C(int on) {
    lbl_804D3868 = on;
}


// Empty in retail.
void fn_802A3BEC() {}

// TODO (not drafted yet, 0x6B8 bytes at 0x8029E714): not analysed
// TODO (not drafted yet, 0x260 bytes at 0x8029EDCC): not analysed
// TODO (not drafted yet, 0x90 bytes at 0x8029F02C): not analysed
// TODO (not drafted yet, 0x164 bytes at 0x8029F0BC): not analysed
// TODO (not drafted yet, 0x128 bytes at 0x8029F220): not analysed
// TODO (not drafted yet, 0xA4 bytes at 0x8029F348): not analysed
// TODO (not drafted yet, 0xCC bytes at 0x8029F3EC): not analysed
// TODO (not drafted yet, 0x39C bytes at 0x8029F4B8): not analysed
// TODO (not drafted yet, 0xA4 bytes at 0x8029F854): not analysed
// TODO (not drafted yet, 0xE0 bytes at 0x8029F8F8): not analysed
// TODO (not drafted yet, 0x88 bytes at 0x8029F9D8): not analysed
// TODO (not drafted yet, 0x50 bytes at 0x8029FA60): not analysed
// TODO (not drafted yet, 0xC0 bytes at 0x8029FAB0): not analysed
// TODO (not drafted yet, 0x16C bytes at 0x8029FB70): not analysed
// TODO (not drafted yet, 0x94 bytes at 0x8029FCDC): not analysed
// TODO (not drafted yet, 0xE4 bytes at 0x8029FD70): not analysed
// TODO (not drafted yet, 0x39C bytes at 0x8029FE54): not analysed
// TODO (not drafted yet, 0xA4 bytes at 0x802A01F0): not analysed
// TODO (not drafted yet, 0xE4 bytes at 0x802A0294): not analysed
// TODO (not drafted yet, 0x1C0 bytes at 0x802A0378): not analysed
// TODO (not drafted yet, 0x4C bytes at 0x802A0538): not analysed
// TODO (not drafted yet, 0xBC bytes at 0x802A0584): not analysed
// TODO (not drafted yet, 0x4C bytes at 0x802A0640): not analysed
// TODO (not drafted yet, 0x74 bytes at 0x802A068C): not analysed
// TODO (not drafted yet, 0x170 bytes at 0x802A0700): not analysed
// TODO (not drafted yet, 0x300 bytes at 0x802A0870): not analysed
// TODO (not drafted yet, 0x84 bytes at 0x802A0B70): not analysed
// TODO (not drafted yet, 0x138 bytes at 0x802A0BF4): not analysed
// TODO (not drafted yet, 0x528 bytes at 0x802A0D2C): not analysed
// TODO (not drafted yet, 0x68 bytes at 0x802A1254): not analysed
// TODO (not drafted yet, 0x15C bytes at 0x802A12BC): not analysed
// TODO (not drafted yet, 0xF8 bytes at 0x802A1418): not analysed
// TODO (not drafted yet, 0x58 bytes at 0x802A1510): not analysed
// TODO (not drafted yet, 0xD4 bytes at 0x802A1568): not analysed
// TODO (not drafted yet, 0x6C8 bytes at 0x802A163C): not analysed
// TODO (not drafted yet, 0x30 bytes at 0x802A1D14): not analysed
// TODO (not drafted yet, 0x9C bytes at 0x802A1DEC): not analysed
// TODO (not drafted yet, 0x8 bytes at 0x802A1E88): not analysed
// TODO (not drafted yet, 0x2C bytes at 0x802A1E90): not analysed
// TODO (not drafted yet, 0x4C bytes at 0x802A1EBC): not analysed
// TODO (not drafted yet, 0x98 bytes at 0x802A1F08): not analysed
// TODO (not drafted yet, 0x2C bytes at 0x802A1FAC): not analysed
// TODO (not drafted yet, 0xC bytes at 0x802A1FD8): not analysed
// TODO (not drafted yet, 0x40 bytes at 0x802A1FE4): not analysed
// TODO (not drafted yet, 0x1C bytes at 0x802A205C): not analysed
// TODO (not drafted yet, 0xA8 bytes at 0x802A20A4): not analysed
// TODO (not drafted yet, 0xC4 bytes at 0x802A214C): not analysed
// TODO (not drafted yet, 0x428 bytes at 0x802A2210): not analysed
// TODO (not drafted yet, 0x410 bytes at 0x802A2638): not analysed
// TODO (not drafted yet, 0x34 bytes at 0x802A2A48): not analysed
// TODO (not drafted yet, 0x218 bytes at 0x802A2A7C): not analysed
// TODO (not drafted yet, 0x95C bytes at 0x802A2C94): not analysed
// TODO (not drafted yet, 0x98 bytes at 0x802A35F0): not analysed
// TODO (not drafted yet, 0xF0 bytes at 0x802A3688): not analysed
// TODO (not drafted yet, 0xEC bytes at 0x802A3778): not analysed
// TODO (not drafted yet, 0x254 bytes at 0x802A3864): not analysed
// TODO (not drafted yet, 0xBC bytes at 0x802A3AB8): not analysed
// TODO (not drafted yet, 0x78 bytes at 0x802A3B74): not analysed
// TODO (not drafted yet, 0xF0 bytes at 0x802A3BF0): not analysed
// TODO (not drafted yet, 0x70 bytes at 0x802A3CE0): not analysed
// TODO (not drafted yet, 0x54 bytes at 0x802A3D50): not analysed
// TODO (not drafted yet, 0x68 bytes at 0x802A3DA4): not analysed
// TODO (not drafted yet, 0x48 bytes at 0x802A3E0C): not analysed
// TODO (not drafted yet, 0x60 bytes at 0x802A3E54): not analysed
// TODO (not drafted yet, 0x8 bytes at 0x802A3EB4): not analysed
// TODO (not drafted yet, 0x20 bytes at 0x802A3EBC): not analysed
// TODO (not drafted yet, 0x20 bytes at 0x802A3EDC): not analysed
// TODO (not drafted yet, 0x84 bytes at 0x802A3EFC): not analysed
// TODO (not drafted yet, 0x250 bytes at 0x802A3F80): not analysed
// TODO (not drafted yet, 0x9C bytes at 0x802A41D0): not analysed
// TODO (not drafted yet, 0x58 bytes at 0x802A426C): not analysed
// TODO (not drafted yet, 0x4DC bytes at 0x802A42C4): not analysed
// TODO (not drafted yet, 0x58 bytes at 0x802A47A0): not analysed
// TODO (not drafted yet, 0x9C bytes at 0x802A47F8): not analysed
// TODO (not drafted yet, 0x2C bytes at 0x802A4894): not analysed
// TODO (not drafted yet, 0x80 bytes at 0x802A48C0): not analysed
// TODO (not drafted yet, 0xFC bytes at 0x802A4940): not analysed
// TODO (not drafted yet, 0xBC bytes at 0x802A4A3C): not analysed
// TODO (not drafted yet, 0x170 bytes at 0x802A4AF8): not analysed
// TODO (not drafted yet, 0x5C bytes at 0x802A4C68): not analysed
// TODO (not drafted yet, 0x160 bytes at 0x802A4CC4): not analysed
// TODO (not drafted yet, 0x2C bytes at 0x802A4E24): not analysed
// TODO (not drafted yet, 0x5C bytes at 0x802A4E50): not analysed
// TODO (not drafted yet, 0x15C bytes at 0x802A4EAC): not analysed
// TODO (not drafted yet, 0x10 bytes at 0x802A5008): not analysed
// TODO (not drafted yet, 0x58 bytes at 0x802A5018): not analysed
// TODO (not drafted yet, 0x110 bytes at 0x802A5070): not analysed
// TODO (not drafted yet, 0x6C bytes at 0x802A5180): not analysed
// TODO (not drafted yet, 0x1B8 bytes at 0x802A51EC): not analysed
// TODO (not drafted yet, 0x6C bytes at 0x802A53A4): not analysed
// TODO (not drafted yet, 0x1B8 bytes at 0x802A5410): not analysed
// TODO (not drafted yet, 0x34 bytes at 0x802A55C8): not analysed
// TODO (not drafted yet, 0x6C bytes at 0x802A55FC): not analysed
// TODO (not drafted yet, 0x140 bytes at 0x802A5668): not analysed
// TODO (not drafted yet, 0x6C bytes at 0x802A57A8): not analysed
// TODO (not drafted yet, 0x10C bytes at 0x802A5814): not analysed
// TODO (not drafted yet, 0x288 bytes at 0x802A5920): not analysed
// TODO (not drafted yet, 0xB4 bytes at 0x802A5BA8): not analysed
// TODO (not drafted yet, 0x61C bytes at 0x802A5C5C): not analysed
// TODO (not drafted yet, 0x1F4 bytes at 0x802A6278): not analysed
// TODO (not drafted yet, 0x34 bytes at 0x802A646C): not analysed
// TODO (not drafted yet, 0x6C bytes at 0x802A64A0): not analysed
// TODO (not drafted yet, 0x140 bytes at 0x802A650C): not analysed
// TODO (not drafted yet, 0x520 bytes at 0x802A664C): not analysed
// TODO (not drafted yet, 0xBC bytes at 0x802A6B6C): not analysed
// TODO (not drafted yet, 0x20 bytes at 0x802A6C28): not analysed
// TODO (not drafted yet, 0x510 bytes at 0x802A6C48): not analysed
// TODO (not drafted yet, 0x27C bytes at 0x802A7158): not analysed
// TODO (not drafted yet, 0x338 bytes at 0x802A73D4): not analysed

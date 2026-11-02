// u_802154B0.cpp: front-end menus (0x802154B0-0x80221100): the pause/options/controls/debug menus, menu
// item helpers and the menu input state. Names are descriptive guesses; the code is matched against the
// retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

// A menu entry (button/toggle/slider state shared by the menu screens).
struct MenuItem {
    u32 flags;     // 0x00 (bit 0 enabled, bit 16 pressed)
    int arg0;      // 0x04
    int arg1;      // 0x08
    int arg2;      // 0x0c
    int arg3;      // 0x10
    float hold0;   // 0x14
    float hold;    // 0x18
    void* ops;     // 0x1c
};

// The menu screen that is currently shown.
struct MenuScreen {
    u8 unk0[0x254];  // 0x000
    u32 flag254;     // 0x254
    u8 unk258[0x584 - 0x258];
    u32 flag584;     // 0x584
    int count588;    // 0x588
};

extern "C" void fn_80026660(int kind, void* cb, int arg);

extern int gWideScreen;         // 0x804D4008
extern int lbl_804D345C;        // current menu selection index (also read by u_8021427C)
extern int lbl_804D3434;        // 0x804D3434
extern int lbl_804D3430;        // 0x804D3430
extern int lbl_804D53E0;        // menu active state
extern MenuScreen* lbl_804D53DC; // current screen
extern float lbl_804D53C0;      // 0x804D53C0
extern int lbl_804D53C4;        // 0x804D53C4
extern int lbl_804D53CC;        // 0x804D53CC
extern int lbl_804D53D0;        // 0x804D53D0
extern int lbl_804D53D4;        // 0x804D53D4 (also used by u_8021427C)
extern int lbl_804D53E4;        // 0x804D53E4
extern int lbl_804D53F4;        // 0x804D53F4
extern u8 lbl_803BC970[];       // menu item ops table

// TODO (not drafted yet, 0xDC bytes at 0x80217B60): menu message handler
extern "C" void fn_80217B60();

// MenuIsCurrent: whether a screen is the current one (0: whether any selection is set).
extern "C" int fn_80215C04(MenuScreen* screen) {
    if (lbl_804D53E0 == 0) {
        return 0;
    }
    if (screen != 0) {
        if (screen == lbl_804D53DC && lbl_804D345C > -1) {
            return 1;
        }
        return 0;
    }
    return lbl_804D345C != -1;
}

// MenuReset: clear the menu state and register the menu message handler.
extern "C" void fn_80217AFC() {
    lbl_804D3434 = -1;
    lbl_804D53DC = 0;
    lbl_804D53C0 = 0.0f;
    lbl_804D53C4 = 0;
    lbl_804D53CC = 0;
    lbl_804D53D0 = 0;
    lbl_804D3430 = 1;
    lbl_804D53D4 = 0;
    lbl_804D53E4 = 0;
    fn_80026660(0x20, (void*)fn_80217B60, 0);
}

// WideScreenToggle: flip the wide-screen flag by an increment.
void WideScreenToggle(int delta) {
    gWideScreen = (gWideScreen + delta) & 1;
}

bool WideScreenIsOn() {
    return gWideScreen != 0;
}

u32 MenuScreenFlag254() {
    return lbl_804D53DC->flag254 & 1;
}

u32 MenuScreenFlag584() {
    return lbl_804D53DC->flag584 & 1;
}

int MenuScreenCount() {
    return lbl_804D53DC->count588 - 2;
}

void Fn8021E654(int value) {
    lbl_804D53F4 = value != 0;
}

extern "C" int fn_80220A38() {
    return lbl_804D53E0;
}

void MenuNop1() {
}

// MenuItemTick: count the hold timer down to zero.
void MenuItemTick(float dt, MenuItem* item) {
    item->hold = item->hold - dt;
    if (item->hold < 0.0f) {
        item->hold = 0.0f;
    }
}

void MenuNop2() {
}

void MenuItemRelease(MenuItem* item) {
    item->flags &= ~0x10000;
    item->hold = item->hold0;
}

void MenuItemClearPress(MenuItem* item) {
    item->flags &= ~0x10000;
}

int MenuItemPress(MenuItem* item) {
    if (item->flags & 1) {
        item->flags |= 0x10000;
        return 1;
    }
    return 0;
}

void MenuItemInit(MenuItem* item, int a, int b, int c, int d) {
    item->ops = lbl_803BC970;
    item->flags = 1;
    item->arg0 = a;
    item->arg1 = b;
    item->arg2 = c;
    item->arg3 = d;
    item->hold0 = 0.0f;
    item->hold = 0.0f;
}

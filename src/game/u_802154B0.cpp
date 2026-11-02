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
    int bit58c;      // 0x58c
    u8 unk590[0x14a8 - 0x590];
    int handle14a8;  // 0x14a8
};

// A selectable widget on a menu screen.
struct MenuWidget {
    u8 unk0[0x9c];
    void* callback;  // 0x9c
    int unka0;       // 0xa0
    int state;       // 0xa4
    u8 unka8[4];
    int timer;       // 0xac
};

extern "C" void fn_80026660(int kind, void* cb, int arg);
extern "C" void fn_80054094(int handle, int value);
extern "C" void fn_80053FEC(int handle, int value);
extern "C" void fn_80054104(int handle, int value);
extern "C" void fn_80053FD4(int handle, int value);
extern "C" int fn_802A1D04();
extern "C" void fn_8014FF94(int kind, int handle, int value);
extern "C" u32 fn_800E1BD0();
extern "C" void fn_800C6448();
int HookFalse0();
void HookNop0();
void HookNop1();
extern "C" void fn_8001BC78();
// TODO (not drafted yet, 0x410 bytes at 0x8021E09C): menu screen handler
extern "C" void fn_8021E09C();
// TODO (not drafted yet, 0x188 bytes at 0x8021DCF4): widget callback
extern "C" void fn_8021DCF4();
extern MenuWidget* lbl_8049EE58[];

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

void MenuToggleA(int delta) {
    int v = ((lbl_804D53DC->flag254 & 1) + delta) & 1;
    fn_80054094(lbl_804D53DC->handle14a8, v);
    if (fn_802A1D04() != 0) {
        fn_8014FF94(2, lbl_804D53DC->handle14a8, v);
    }
}

void MenuToggleB(int delta) {
    u32 v = fn_800E1BD0();
    v = v & (1 << lbl_804D53DC->bit58c);
    v = (delta - ((int)(-v | v) >> 31)) & 1;
    fn_80053FEC(lbl_804D53DC->handle14a8, v);
    if (fn_802A1D04() != 0) {
        fn_8014FF94(3, lbl_804D53DC->handle14a8, v);
    }
}

u32 MenuBitState() {
    u32 v = fn_800E1BD0();
    v = v & (1 << lbl_804D53DC->bit58c);
    return (-v | v) >> 31;
}

void MenuToggleC(int delta) {
    int v = ((lbl_804D53DC->flag584 & 1) + delta) & 1;
    fn_80054104(lbl_804D53DC->handle14a8, v);
    if (fn_802A1D04() != 0) {
        fn_8014FF94(1, lbl_804D53DC->handle14a8, v);
    }
}

void MenuStepCount(int delta) {
    lbl_804D53DC->count588 += delta;
    if (lbl_804D53DC->count588 > 3) {
        lbl_804D53DC->count588 = 2;
    } else if (lbl_804D53DC->count588 < 2) {
        lbl_804D53DC->count588 = 3;
    }
    fn_80053FD4(lbl_804D53DC->handle14a8, lbl_804D53DC->count588);
    if (fn_802A1D04() != 0) {
        fn_8014FF94(0, lbl_804D53DC->handle14a8, lbl_804D53DC->count588);
    }
}

void MenuHookToggle() {
    if (HookFalse0() != 0) {
        HookNop1();
        fn_8001BC78();
    } else {
        fn_800C6448();
        HookNop0();
    }
}

int MenuHookState() {
    return HookFalse0();
}

void MenuArmWidget() {
    MenuWidget* w;
    if (lbl_804D345C == -1) {
        return;
    }
    lbl_804D53D4 = 1;
    w = lbl_8049EE58[lbl_804D345C];
    w->callback = (void*)fn_8021DCF4;
    w->unka0 = 0;
    if (w->state != 1) {
        w->timer = 500;
    }
    w->state = 3;
}

void MenuScreenRun() {
    fn_8021E09C();
}

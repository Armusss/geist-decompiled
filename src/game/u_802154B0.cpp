// u_802154B0.cpp: front-end menus (0x802154B0-0x80221100): the pause/options/controls/debug menus, menu
// item helpers and the menu input state. Names are descriptive guesses; the code is matched against the
// retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

struct MenuItem;
struct MenuOps;

// A menu entry (button/toggle/slider state shared by the menu screens).
struct MenuItem {
    u32 flags;     // 0x00 (bit 0 enabled, bit 16 pressed)
    int arg0;      // 0x04
    int arg1;      // 0x08
    int arg2;      // 0x0c
    int arg3;      // 0x10
    float hold0;   // 0x14
    float hold;    // 0x18
    MenuOps* ops;  // 0x1c
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

// Menu item classes: each adds its own callback and argument after the common head.
struct MenuButtonA : MenuItem {
    void (*onPress)(int); // 0x20
    int arg;              // 0x24
};
struct MenuButtonB : MenuItem {
    u8 unk20[8];
    void (*onPress)(int); // 0x28
    int arg;              // 0x2c
};
struct MenuButtonC : MenuItem {
    u8 unk20[0xc];
    void (*onPress)(int); // 0x2c
    int arg;              // 0x30
};
struct MenuButtonD : MenuItem {
    u8 unk20[0x24];
    void (*onPress)(int, int); // 0x44
    int unk48;
    int arg;                   // 0x4c
};
struct MenuButtonE : MenuItem {
    u8 unk20[0xc];
    int arg;                   // 0x2c
    u8 unk30[0x38];
    void (*onPress)(int, int); // 0x68
};
struct MenuButtonF : MenuItem {
    u8 unk20[0x10];
    int arg;                   // 0x30
    u8 unk34[0x3c];
    void (*onPress)(int, int); // 0x70
};
struct MenuOps {
    u8 unk0[0x24];
    void (*activate)(MenuItem* item, int flag); // 0x24
};

// A menu page: owned strings plus up to five (label, id) shortcut pairs.
struct MenuPage {
    void* items[20];  // 0x00
    int itemCount;    // 0x50
    u8 unk54[4];
    struct {
        void* label;
        int id;
    } pairs[5];       // 0x58
    int pairCount;    // 0x80
    int unk84;        // 0x84
    int unk88;
    int unk8c;
    int unk90;
    int unk94;
    int unk98;
    u8 unk9c[8];
    int unka4;        // 0xa4
    int unka8;        // 0xa8
};

// A label item: text plus three user words.
struct MenuLabel : MenuItem {
    const char* text; // 0x20
    u8 kind;          // 0x24
    u8 style;         // 0x25
    u8 pad26[2];
    float scale;      // 0x28
    int arg0;         // 0x2c
    int arg1;         // 0x30
    int arg2;         // 0x34
};

// An item with a list of key hashes.
struct MenuKeyList {
    u8 unk0[0x3c];
    u32 keys[12];  // 0x3c
    int keyCount;  // 0x6c
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
extern int lbl_804D53C8;
extern int lbl_804D53F0;
extern "C" int SendMsg(int to, int msg, void* data);
void TextSysInit();
void GfxFbSave(int n);
void GfxFbCommit();
void GfxSetCopyFilter(int n);
extern "C" int fn_80115850(int a, int b, int c);
extern "C" void fn_801EAEAC();
// TODO (not drafted yet, 0x6EC bytes at 0x8022034C): draw the current menu screen
extern "C" void fn_8022034C(int alpha);
// TODO (not drafted yet, 0x4B4 bytes at 0x8021E9FC): draw one menu page
extern "C" void fn_8021E9FC(MenuWidget* widget, int a, int alpha);

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
extern int lbl_804D344C;        // 0x804D344C
extern int lbl_804D3450;        // 0x804D3450
extern "C" void FxStart(int handle, int a, int b);
extern "C" void fn_8000F60C(int x);
extern "C" void fn_8000F874(int x);
u32 StrFindKeySkip(u32 key);
void VFree(void* p);

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
    item->ops = (MenuOps*)lbl_803BC970;
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
    v = (((-v | v) >> 31) + delta) & 1;
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

// MenuItemActivate: invoke the item class's activate slot when it is not held.
void MenuItemActivate(MenuItem* item) {
    if (0.0f == item->hold) {
        item->ops->activate(item, 1);
    }
}

void MenuItemActivate2(MenuItem* item) {
    if (0.0f == item->hold) {
        item->ops->activate(item, 1);
    }
}

void MenuItemActivate3(MenuItem* item) {
    if (0.0f == item->hold) {
        item->ops->activate(item, 1);
    }
}

void MenuButtonDFire(MenuButtonD* b, int arg) {
    if (0.0f == b->hold && b->onPress != 0) {
        b->onPress(arg, b->arg);
        FxStart(lbl_804D3450, 0, 0);
    }
}

void MenuButtonEFire(MenuButtonE* b, int arg) {
    if (0.0f == b->hold && b->onPress != 0) {
        b->onPress(arg, b->arg);
        FxStart(lbl_804D344C, 0, 0);
    }
}

void MenuButtonFFire(MenuButtonF* b, int arg) {
    if (0.0f == b->hold && b->onPress != 0) {
        b->onPress(arg, b->arg);
        FxStart(lbl_804D344C, 0, 0);
    }
}

void MenuButtonBFire(MenuButtonB* b) {
    if (0.0f == b->hold && b->onPress != 0) {
        b->onPress(b->arg);
        FxStart(lbl_804D344C, 0, 0);
    }
}

void MenuButtonCFire(MenuButtonC* b) {
    if (0.0f == b->hold && b->onPress != 0) {
        b->onPress(b->arg);
        FxStart(lbl_804D344C, 0, 0);
    }
}

void MenuButtonAFire(MenuButtonA* b) {
    if (0.0f == b->hold && b->onPress != 0) {
        b->onPress(b->arg);
        FxStart(lbl_804D344C, 0, 0);
    }
}

// MenuPageAdd: append an owned string (and optionally a shortcut pair) to a page.
void MenuPageAdd(MenuPage* page, void* item, int id) {
    if (page->itemCount >= 20) {
        return;
    }
    page->items[page->itemCount] = item;
    if (id != -1 && page->pairCount < 5) {
        page->pairs[page->pairCount].label = item;
        page->pairs[page->pairCount].id = id;
        page->pairCount++;
    }
    page->itemCount++;
}

// MenuPageClear: free a page's strings and reset it.
void MenuPageClear(MenuPage* page) {
    void** p = page->items;
    int i;
    for (i = 0; i < page->itemCount; i++) {
        VFree(*p);
        p++;
    }
    page->itemCount = 0;
    page->pairCount = 0;
    page->unk88 = 0;
    page->unk8c = 0;
    page->unk90 = 0;
    page->unk94 = 0;
    page->unk98 = 0;
    page->unka4 = 0;
    page->unka8 = 0;
    page->unk84 = 0;
}

// MenuKeyListAdd: add the hash of a key name to the list.
void MenuKeyListAdd(MenuKeyList* list, u32 key) {
    u32 hash = StrFindKeySkip(key);
    if (list->keyCount < 12) {
        list->keys[list->keyCount] = hash;
        list->keyCount++;
    }
}

// MenuLabelInit
void MenuLabelInit(MenuLabel* label, u8 kind, const char* text, int a, int b, int c) {
    label->kind = kind;
    label->style = 2;
    label->scale = 1.0f;
    label->text = text ? text : "<missing string>";
    label->arg0 = a;
    label->arg1 = b;
    label->arg2 = c;
}

// MenuFramePrepare: capture the framebuffer when the menu opens or is resumed.
extern "C" void fn_80215A24() {
    if ((lbl_804D53E0 >= 2 || lbl_804D53E0 < 0) && lbl_804D3430 != 0) {
        if (lbl_804D53C4 != 0) {
            GfxFbSave(0);
            GfxFbCommit();
            GfxSetCopyFilter(2);
            lbl_804D53F0 = fn_80115850(0, 1, 0);
            GfxFbSave(1);
            if (lbl_804D53E0 == 4) {
                lbl_804D53C4 = 0;
                lbl_804D53CC = 1;
            }
        } else if (lbl_804D53D0 == 2) {
            if (!(lbl_804D53C0 < 0.0f)) {
                GfxFbSave(0);
                GfxFbCommit();
                GfxSetCopyFilter(2);
                lbl_804D53F0 = fn_80115850(0, 1, 0);
            }
        } else if (lbl_804D53C8 != 0) {
            GfxFbSave(0);
            GfxFbCommit();
            GfxSetCopyFilter(2);
            lbl_804D53F0 = fn_80115850(0, 1, 0);
            lbl_804D53C8 = 0;
        }
    }
}

// MenuDraw: draw the current screen or widget page; returns 1 if the menu is showing.
extern "C" int fn_80215B40(int widget) {
    if ((u32)lbl_804D53E0 <= 1 || lbl_804D53E0 == 4) {
        return 0;
    } else if (lbl_804D53CC != 0 || lbl_804D53D0 == 1) {
        TextSysInit();
        if (widget != -1) {
            if (lbl_804D345C > -1 && (lbl_804D3430 == 0 || lbl_804D53CC != 0)) {
                fn_8021E9FC(lbl_8049EE58[lbl_804D345C], widget, 0xFF);
            } else {
                fn_8022034C(0xFF);
            }
            fn_801EAEAC();
        }
        return 1;
    } else {
        return 0;
    }
}

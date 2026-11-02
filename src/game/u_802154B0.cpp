// u_802154B0.cpp: front-end menus (0x802154B0-0x80221100): the pause/options/controls/debug menus, menu
// item helpers and the menu input state. Names are descriptive guesses; the code is matched against the
// retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

struct MenuItem;
struct MenuOps;

// A menu entry (button/toggle/slider state shared by the menu screens).
struct MenuItemBase {
    u32 flags;     // 0x00 (bit 0 enabled, bit 16 pressed)
    int arg0;      // 0x04
    int arg1;      // 0x08
    int arg2;      // 0x0c
    int arg3;      // 0x10
    float hold0;   // 0x14
    float hold;    // 0x18
};

struct MenuItem : MenuItemBase {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual int canFocus();          // vtable offset 0x14
    virtual void defocus();          // 0x18
    virtual void layout();           // 0x1c
    virtual void select();           // 0x20
    virtual void activate(int flag); // vtable offset 0x24
    virtual void open();             // 0x28
    virtual void close();            // vtable offset 0x2c
    MenuItem(int a, int b, int c, int d);
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
    u8 unk28[4];
    void (*close)(MenuItem* item);              // 0x2c
};

// Message block of the menu command message (0xC14).
struct MenuMsgData {
    u8 unk0[8];
    u32 target; // 0x08
    int key;    // 0x0c
};

// A menu page: owned strings plus up to five (label, id) shortcut pairs.
struct MenuPage {
    MenuItem* items[20]; // 0x00
    int itemCount;    // 0x50
    int cursor;       // 0x54
    struct {
        void* label;
        int id;
    } pairs[5];       // 0x58
    int pairCount;    // 0x80
    int minFree;      // 0x84
    int unk88;
    int unk8c;
    void (*onOpen)(int);  // 0x90
    void (*onClose)(int); // 0x94
    int unk98;
    void* callback;   // 0x9c
    int unka0;        // 0xa0
    int state;        // 0xa4
    int unka8;        // 0xa8 (page is open)
    int timer;        // 0xac
    int unkb0;        // 0xb0
    int unkb4;        // 0xb4
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
extern "C" void fn_8021E09C(MenuPage* page);
// TODO (not drafted yet, 0x220 bytes at 0x8021DE7C): menu page redraw
extern "C" void fn_8021DE7C();
extern const char* lbl_804D3458; // "MainMenu"
extern "C" int fn_800498D0(MenuScreen* screen);
extern "C" int fn_800C5654(int a, u32 hash, int b);
extern "C" void fn_80142E88();
int GunPoolFreeSize8017BA2C();
extern "C" void fn_8002FD68();
extern int lbl_804D3444;
extern int lbl_804D3448;
extern int lbl_804D3454;
extern int lbl_804D3440;
extern int lbl_804D53EC;
extern int lbl_804D3438;
int PlayerStateValue(int a, int b);
int PadFindPressedPlayer(int a, int b);
extern float lbl_804D53BC;
extern "C" float fn_800071A4();
extern "C" void fn_8000719C(float v);
extern "C" void fn_8014CEDC(int v);
void SaveCommand(int a, int b);
extern "C" int fn_802A1FA0();
extern MenuPage lbl_8049E818;
extern "C" void fn_80201260(float f, int a, int b);
int PadPressed(int pad, int a, int b);
extern "C" void FxStart(int handle, int a, int b);
extern "C" void fn_8021DCF4();
extern MenuPage* lbl_8049EE58[];
extern MenuPage lbl_8049E9A8;
extern MenuPage lbl_8049EB38;
int MenuMsg(int sender, int msg, MenuMsgData* data);
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
extern "C" void fn_8021E9FC(MenuPage* widget, int a, int alpha);

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
    fn_80026660(0x20, (void*)MenuMsg, 0);
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

// MenuItem constructor (the vtable is at lbl_803BC970).
MenuItem::MenuItem(int a, int b, int c, int d) {
    flags = 1;
    arg0 = a;
    arg1 = b;
    arg2 = c;
    arg3 = d;
    hold0 = 0.0f;
    hold = 0.0f;
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
    MenuPage* w;
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

void MenuScreenRun(MenuPage* page) {
    fn_8021E09C(page);
}

// MenuItemActivate: invoke the item class's activate slot when it is not held.
void MenuItemActivate(MenuItem* item) {
    if (0.0f == item->hold) {
        item->activate(1);
    }
}

void MenuItemActivate2(MenuItem* item) {
    if (0.0f == item->hold) {
        item->activate(1);
    }
}

void MenuItemActivate3(MenuItem* item) {
    if (0.0f == item->hold) {
        item->activate(1);
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
void MenuPageAdd(MenuPage* page, void* itemp, int id) {
    MenuItem* item = (MenuItem*)itemp;
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
    MenuItem** p = page->items;
    int i;
    for (i = 0; i < page->itemCount; i++) {
        VFree(*p);
        p++;
    }
    page->itemCount = 0;
    page->pairCount = 0;
    page->unk88 = 0;
    page->unk8c = 0;
    page->onOpen = 0;
    page->onClose = 0;
    page->unk98 = 0;
    page->state = 0;
    page->unka8 = 0;
    page->minFree = 0;
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

// MenuMsg: handle the menu open/close command messages.
int MenuMsg(int sender, int msg, MenuMsgData* data) {
    MenuScreen* screen[10];
    switch (msg) {
    case 0xC14:
        switch (data->key) {
        case -0x79D6369:
            if (lbl_804D53E0 == 4 || lbl_804D53E0 == 3) {
                lbl_804D3434 = -1;
                fn_8021E09C(&lbl_8049EB38);
                return 1;
            }
            break;
        case 0x1C19D386:
            if (lbl_804D53E0 == 2) {
                if ((int)(data->target >> 24) == 2) {
                    SendMsg(data->target, 0x400, screen);
                    lbl_804D53DC = screen[0];
                }
                lbl_804D3434 = -1;
                fn_8021E09C(&lbl_8049E9A8);
                return 1;
            }
            break;
        }
        return 0;
    }
    return 0;
}

// MenuPopPage: close the current page and clear the selection.
void MenuPopPage() {
    MenuPage* page = lbl_8049EE58[lbl_804D345C];
    if (page->unkb4 != 0) {
        MenuPageClear(page);
    }
    lbl_804D345C = -1;
    fn_8021DE7C();
}

// MenuPageClose: close a page (running the item close hooks) and re-arm its widget.
void MenuPageClose(MenuPage* page) {
    MenuItem** p;
    int i;
    if (page->unka8 == 1) {
        if (page->onOpen != 0) {
            page->onClose(0);
        }
        p = page->items;
        for (i = 0; i < page->itemCount; i++) {
            (*p)->close();
            p++;
        }
        page->unka8 = 0;
    }
    MenuPageClear(page);
    MenuArmWidget();
}

struct MenuNameMsg {
    int unk0;
    const char* name;
    u8 unk8[16];
};

// MenuGotoScreen: ask the front end to switch to a named screen.
void MenuGotoScreen(const char* name) {
    MenuNameMsg msg;
    MenuPage* page;
    MenuPage* armed;
    msg.name = name;
    if (name == lbl_804D3458) {
        fn_80142E88();
    }
    SendMsg(0x38000000, 0x5600, &msg);
    if (lbl_804D345C != -1) {
        lbl_804D53D4 = 1;
        page = lbl_8049EE58[lbl_804D345C];
        if (page->unkb4 != 0) {
            MenuPageClear(page);
        }
        armed = lbl_8049EE58[lbl_804D345C];
        armed->callback = (void*)MenuPopPage;
        armed->unka0 = 0;
        if (armed->state != 1) {
            armed->timer = 500;
        }
        armed->state = 3;
    }
}

struct SysObj {
    u8 unk0[0x30];
    void (*onMsg)(int code, void* data); // 0x30
};

struct SysMsgOut {
    SysObj* obj;
    int unk4[3];
};

struct SysCall {
    SysObj* obj;
    int value;
    u8 unk8[16];
};

// MenuSendValue: send a value to the system object, then close the current page.
void MenuSendValue(int value) {
    SysMsgOut out;
    SysCall call;
    MenuPage* page;
    MenuPage* armed;
    SendMsg(0x2000000, 0x5C00, &out);
    call.obj = out.obj;
    call.value = value;
    out.obj->onMsg(0x1000, &call);
    HookNop1();
    if (lbl_804D345C != -1) {
        lbl_804D53D4 = 1;
        page = lbl_8049EE58[lbl_804D345C];
        if (page->unkb4 != 0) {
            MenuPageClear(page);
        }
        armed = lbl_8049EE58[lbl_804D345C];
        armed->callback = (void*)MenuPopPage;
        armed->unka0 = 0;
        if (armed->state != 1) {
            armed->timer = 500;
        }
        armed->state = 3;
    }
    fn_8001BC78();
}

// MenuPageOpen: run the page's hooks the first time it is shown.
static inline void MenuPageOpen(MenuPage* page) {
    MenuItem** p;
    int i;
    if (page->state != 3) {
        page->timer = 0;
    }
    if (page->unka8 == 0) {
        if (lbl_804D53E4 == 0 && page->minFree > GunPoolFreeSize8017BA2C()) {
            fn_8002FD68();
            lbl_804D53E4 = 1;
        }
        if (page->onOpen != 0) {
            page->onOpen(0);
        }
        p = page->items;
        for (i = 0; i < page->itemCount; i++) {
            (*p)->open();
            p++;
        }
        page->unka8 = 1;
    }
    page->state = 1;
}

// MenuPageSetup: lay out every item and focus the first focusable one.
static inline void MenuPageSetup(MenuPage* page) {
    page->cursor = 0;
    while (page->cursor < page->itemCount) {
        page->items[page->cursor]->layout();
        page->cursor++;
    }
    page->cursor = 0;
    for (; page->cursor < page->itemCount; page->cursor++) {
        if (page->items[page->cursor]->canFocus() != 0) {
            break;
        }
    }
    if (page->cursor == page->itemCount) {
        page->cursor = -1;
    }
}

// MenuReturn: pop back to the previous page.
extern "C" void fn_8021DCF4() {
    MenuPage* prev;
    MenuPage* page = lbl_8049EE58[lbl_804D345C];
    if (page->unkb4 != 0) {
        MenuPageClear(page);
    }
    lbl_804D345C--;
    if (lbl_804D345C > -1) {
        prev = lbl_8049EE58[lbl_804D345C];
        MenuPageOpen(prev);
        FxStart(lbl_804D3444, 0, 0);
    } else {
        fn_8021DE7C();
    }
}

// MenuEnter: push a page and focus its first focusable item.
void MenuEnter(MenuPage* page) {
    lbl_804D345C++;
    lbl_8049EE58[lbl_804D345C] = page;
    MenuPageSetup(page);
    MenuPageOpen(page);
    FxStart(lbl_804D3448, 0, 0);
}

// MenuCursorNext: move the cursor to the next focusable item (wrapping).
void MenuCursorNext(MenuPage* page) {
    int i;
    MenuItem** p;
    i = page->cursor + 1;
    p = &page->items[i];
    for (; i < page->itemCount; i++) {
        if ((*p)->canFocus() != 0) {
            page->items[page->cursor]->defocus();
            page->cursor = i;
            FxStart(lbl_804D3454, 0, 0);
            return;
        }
        p++;
    }
    for (i = 0, p = page->items; i < page->cursor; i++, p++) {
        if ((*p)->canFocus() != 0) {
            page->items[page->cursor]->defocus();
            page->cursor = i;
            FxStart(lbl_804D3454, 0, 0);
            return;
        }
    }
}

// MenuCursorPrev: move the cursor to the previous focusable item (wrapping).
void MenuCursorPrev(MenuPage* page) {
    int i;
    MenuItem** p;
    i = page->cursor - 1;
    p = &page->items[i];
    for (; i > -1; i--) {
        if ((*p)->canFocus() != 0) {
            page->items[page->cursor]->defocus();
            page->cursor = i;
            FxStart(lbl_804D3454, 0, 0);
            return;
        }
        p--;
    }
    i = page->itemCount - 1;
    p = &page->items[i];
    for (; i > page->cursor; i--, p--) {
        if ((*p)->canFocus() != 0) {
            page->items[page->cursor]->defocus();
            page->cursor = i;
            FxStart(lbl_804D3454, 0, 0);
            return;
        }
    }
}

// MenuPageShow: (re)open a page, optionally laying its items out first.
void MenuPageShow(MenuPage* page, int setup) {
    if (setup != 0) {
        MenuPageSetup(page);
    }
    MenuPageOpen(page);
}

// MenuReplace: replace the current page.
void MenuReplace(MenuPage* page) {
    int top = lbl_804D345C;
    MenuPage* cur;
    lbl_8049EE58[top] = page;
    cur = lbl_8049EE58[top];
    MenuPageSetup(cur);
    MenuPageOpen(cur);
    FxStart(lbl_804D3440, 0, 0);
}

// MenuSelectPage: play the page's select cue, then open it (or hand it to the current page).
void MenuSelectPage(MenuPage* page) {
    MenuPage* cur;
    switch (page->unkb0) {
    case 1:
        fn_800C5654(fn_800498D0(lbl_804D53DC), 0xCB2B18BE, 0);
        break;
    case 2:
        fn_800C5654(fn_800498D0(lbl_804D53DC), 0x044CEF3E, 0);
        break;
    case 3:
        fn_800C5654(fn_800498D0(lbl_804D53DC), 0x5A7E5498, 0);
        break;
    case 4:
        fn_800C5654(fn_800498D0(lbl_804D53DC), 0x5A7E5498, 0);
        break;
    }
    if (lbl_804D345C > -1) {
        cur = lbl_8049EE58[lbl_804D345C];
        cur->callback = (void*)MenuReplace;
        cur->unka0 = (int)page;
        if (cur->state != 1) {
            cur->timer = 500;
        }
        cur->state = 3;
        lbl_804D53D4 = 1;
    } else {
        fn_8021E09C(page);
    }
}

// MenuAdjustLevel: step the level value by the given amount, clamped to +-0.5.
void MenuAdjustLevel(int steps) {
    float cur = fn_800071A4();
    cur = 0.05f * (float)steps + cur;
    if (cur > 0.5f) {
        cur = 0.5f;
    } else if (cur < -0.5f) {
        cur = -0.5f;
    }
    fn_8000719C(cur);
}

// MenuLevelPercent: current level as a 0-100 value.
int MenuLevelPercent() {
    float cur = fn_800071A4();
    return (int)((0.5f + cur) * 100.0f);
}

// MenuMainMenu: leave to the main menu.
void MenuMainMenu() {
    MenuNameMsg msg;
    MenuPage* page;
    MenuPage* armed;
    fn_8014CEDC(1);
    msg.name = lbl_804D3458;
    if (msg.name == lbl_804D3458) {
        fn_80142E88();
    }
    SendMsg(0x38000000, 0x5600, &msg);
    if (lbl_804D345C != -1) {
        lbl_804D53D4 = 1;
        page = lbl_8049EE58[lbl_804D345C];
        if (page->unkb4 != 0) {
            MenuPageClear(page);
        }
        armed = lbl_8049EE58[lbl_804D345C];
        armed->callback = (void*)MenuPopPage;
        armed->unka0 = 0;
        if (armed->state != 1) {
            armed->timer = 500;
        }
        armed->state = 3;
    }
}

// MenuPageClose2: same as MenuPageClose (a second page type shares the code shape).
void MenuPageClose2(MenuPage* page) {
    MenuItem** p;
    int i;
    if (page->unka8 == 1) {
        if (page->onOpen != 0) {
            page->onClose(0);
        }
        p = page->items;
        for (i = 0; i < page->itemCount; i++) {
            (*p)->close();
            p++;
        }
        page->unka8 = 0;
    }
    MenuPageClear(page);
    MenuArmWidget();
}

// MenuPageCloseResetPad: close hook that also resets the pad modes.
void MenuPageCloseResetPad(MenuPage* arg) {
    MenuPage* page;
    MenuItem** p;
    int i;
    SaveCommand(1, fn_802A1FA0());
    SaveCommand(6, 0);
    page = arg;
    if (page->unka8 == 1) {
        if (page->onOpen != 0) {
            page->onClose(0);
        }
        p = page->items;
        for (i = 0; i < page->itemCount; i++) {
            (*p)->close();
            p++;
        }
        page->unka8 = 0;
    }
    MenuPageClear(page);
    MenuArmWidget();
}

// MenuPageWatch: per-frame update of a page that closes on a button press.
int MenuPageWatch(int pad) {
    if (lbl_8049E818.state == 1) {
        fn_80201260(lbl_804D53BC, 0, 1);
    } else {
        fn_80201260(lbl_804D53BC, 0, 0);
    }
    if (PadPressed(pad, 0, 4) == 1 && lbl_804D53D4 == 0) {
        lbl_804D53D4 = 1;
        MenuArmWidget();
    }
    return 0;
}

static inline int PairHit(int id) {
    int pad;
    u8 r;
    if (lbl_804D53EC != 0 || lbl_804D53D4 != 0) {
        return 0;
    }
    if (lbl_804D3434 > -1 && PlayerStateValue(lbl_804D3434, id) != 0) {
        r = PadPressed(lbl_804D3434, 0, id) == 1;
        return r;
    }
    pad = PadFindPressedPlayer(0, id);
    if (pad > -1 && PlayerStateValue(pad, id) != 0) {
        lbl_804D3438 = pad;
        return 1;
    }
    return 0;
}

// MenuPageInput: trigger the first shortcut pair whose button is pressed; returns 1 if one fired.
int MenuPageInput(MenuPage* page) {
    int id;
    MenuPage* w;
    int i;
    for (i = 0, w = page; i < page->pairCount; w = (MenuPage*)((u8*)w + 8), i++) {
        id = w->pairs[0].id;
        if (PairHit(id)) {
            ((MenuItem*)page->pairs[i].label)->select();
            return 1;
        }
    }
    return 0;
}

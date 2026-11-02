// AppMain.c: application main loop and start-up (0x800866D8-0x800886C8).
// A small file-chunk handler for a table of 0x20-byte entries (registered with the file loader), the
// intrusive circular list helpers shared with the effect code, the per-frame "AppMain" loop with its
// FPS counter and fade, and the start-up sequence that initialises every game system in order.
// Names are descriptive guesses; the code is matched against the retail DOL.

#pragma auto_inline off

typedef unsigned char u8;
typedef unsigned long u32;

// Table entry of the chunk handled by the first functions (0x20 bytes each).
struct ChunkEntry {
    int mId;       // 0x00
    int mArg[7];   // 0x04
};

struct ChunkHeader {
    int mVersion;  // 0x00 1
    int mCount;    // 0x04 number of ChunkEntry records following the header
};

// Intrusive doubly linked list node. The list head points at the first node, whose mPrev is the last node;
// the last node's mNext is null.
struct ListNode {
    ListNode* mPrev;  // 0x00
    ListNode* mNext;  // 0x04
};

// Table walked by the (empty) startup loop: three words per entry, the last one a count.
struct CountEntry {
    int mUnk0;
    int mUnk4;
    int mCount;
};

#ifdef __cplusplus
extern "C" {
#endif
void FileFlushBuffer(void);
void fn_800070C0(void* entry);
void fn_80088C10(int id, void* name, int a, int b, int c, int d, int e, int f);
void fn_80088FB0(int id, int on);
void fn_80088F0C(int id, int on);
void fn_80088E68(int id, int on);
#ifdef __cplusplus
}
#endif

int FileAddHandler(int tag, int (*handler)(void*, int, void*), void* arg);

extern CountEntry lbl_803983C0[];

static ChunkHeader* sChunk;
static ChunkEntry* sEntries;
static int sTableCount;

int ChunkHandler(void* data, int size, void* arg);

// Starts every entry of the loaded chunk.
int ChunkStart(void)
{
    int off;
    ChunkEntry* e;
    int i;
    if (sChunk != 0 && sEntries != 0) {
        off = 0;
        for (i = 0; i < sChunk->mCount; i++) {
            e = (ChunkEntry*)((u8*)sEntries + off);
            fn_80088C10(e->mId, &e->mArg[0], e->mArg[1], e->mArg[2], e->mArg[3], e->mArg[4], e->mArg[5], e->mArg[6]);
            fn_80088FB0(e->mId, 1);
            fn_80088F0C(e->mId, 1);
            fn_80088E68(e->mId, 0);
            off += 0x20;
        }
    }
    return 1;
}

// Registers the chunk handler (tag 0x20) with the file loader.
int ChunkInit(void)
{
    FileAddHandler(0x20, (int (*)(void*, int, void*))ChunkHandler, 0);
    sChunk = 0;
    sEntries = 0;
    return 1;
}

// Chunk handler: remembers the header and the entry table, and prepares every entry.
int ChunkHandler(void* data, int size, void* arg)
{
    int off;
    int i;
    ChunkHeader* chunk;
    FileFlushBuffer();
    chunk = size > 0 ? (ChunkHeader*)data : 0;
    sChunk = chunk;
    if (chunk->mVersion == 1) {
        u8 ok = 0;
        if (size - 8 > 0 && chunk->mCount * 32 != 0) {
            ok = 1;
        }
        if (ok) {
            sEntries = (ChunkEntry*)((u8*)data + 8);
        } else {
            sEntries = 0;
        }
        for (i = 0, off = 0; i < sChunk->mCount; i++) {
            fn_800070C0((u8*)sEntries + off + 4);
            off += 0x20;
        }
        return 1;
    }
    return 0;
}

void ChunkNop(void)
{
}

int ChunkTableWalk(void)
{
    int i;
    int j;
    CountEntry* e;
    sTableCount = 11;
    e = lbl_803983C0;
    for (i = 0; i < 11; i++, e++) {
        for (j = 0; j < e->mCount; j++) {
        }
    }
    return j;
}

// List helpers shared with the effect code (they keep their fn_ names: other files call them).
#ifdef __cplusplus
extern "C" {
#endif
// 0x80086928: first node for which pred(node, key) returns 0
ListNode* fn_80086928(ListNode** list, u32 key, int (*pred)(ListNode*, u32))
{
    ListNode* node = *list;
    while (node != 0) {
        if (pred(node, key) == 0) {
            return node;
        }
        node = node->mNext;
    }
    return 0;
}

// 0x800869A0: node number i
ListNode* fn_800869A0(ListNode** list, int i)
{
    ListNode* node = *list;
    int n = 0;
    while (node != 0) {
        if (n == i) {
            return node;
        }
        node = node->mNext;
        n++;
    }
    return 0;
}

// 0x800869CC: number of nodes
int fn_800869CC(ListNode** list)
{
    int n = 0;
    ListNode* node;
    for (node = *list; node != 0; node = node->mNext) {
        n++;
    }
    return n;
}

// 0x800869EC: unlinks a node
void fn_800869EC(ListNode** list, ListNode* node)
{
    ListNode* head = *list;
    ListNode* last;
    if (head == 0) {
        return;
    }
    if (node != 0) {
    } else {
        return;
    }
    if (node == head) {
        if (head != 0) {
            *list = head->mNext;
            if (*list != 0) {
                (*list)->mPrev = head->mPrev;
            }
            head->mPrev = 0;
            head->mNext = 0;
        }
    } else {
        last = head->mPrev;
        if (node == last) {
            if (head != 0) {
                if (last == head) {
                    *list = 0;
                } else {
                    head->mPrev = last->mPrev;
                    if ((*list)->mPrev != 0) {
                        (*list)->mPrev->mNext = 0;
                    }
                }
                last->mPrev = 0;
                last->mNext = 0;
            }
        } else {
            node->mNext->mPrev = node->mPrev;
            node->mPrev->mNext = node->mNext;
        }
    }
    node->mPrev = 0;
    node->mNext = 0;
}

// 0x80086AC0: takes the last node off the list
ListNode* fn_80086AC0(ListNode** list)
{
    ListNode* head = *list;
    ListNode* last;
    if (head == 0) {
        return 0;
    }
    last = head->mPrev;
    if (last == head) {
        *list = 0;
    } else {
        head->mPrev = last->mPrev;
        if ((*list)->mPrev != 0) {
            (*list)->mPrev->mNext = 0;
        }
    }
    last->mPrev = 0;
    last->mNext = 0;
    return last;
}

// 0x80086B20: adds a node at the end
void fn_80086B20(ListNode** list, ListNode* node)
{
    if (*list != 0) {
        node->mPrev = (*list)->mPrev;
        (*list)->mPrev->mNext = node;
        (*list)->mPrev = node;
        return;
    }
    node->mNext = 0;
    node->mPrev = node;
    *list = node;
}

// 0x80086B60: adds a node at the front
void fn_80086B60(ListNode** list, ListNode* node)
{
    if (*list != 0) {
        node->mNext = *list;
        node->mPrev = (*list)->mPrev;
        (*list)->mPrev = node;
    } else {
        node->mNext = 0;
        node->mPrev = node;
    }
    *list = node;
}

#ifdef __cplusplus
}
#endif

// Callees in other files.
#ifdef __cplusplus
extern "C" {
#endif
void AudioInit(void);
void AudioShutdown(void);
void fn_80005A38(void);
void fn_80005CEC(void);
void fn_8000AE40(void);
void fn_8000AFB0(int arg);
void fn_80020280(void);
void fn_80020694(void);
void fn_80026660(int tag, void* handler, int arg);
void fn_800266A0(void);
void fn_800267EC(void);
void fn_80028140(void);
void fn_800289F8(void);
void fn_80049F70(void);
void fn_8004BD64(void);
void fn_8007CA44(void);
void fn_8007CB14(void);
void fn_800849B8(void);
void fn_80084B34(void);
void fn_80085A98(void);
void fn_80093710(void);
void fn_800938C4(void);
void fn_800BA8A0(void);
void fn_800BA9BC(void);
void fn_800C6DD4(void);
void fn_800C6DD8(void);
void fn_800C843C(void);
void fn_800C8EC8(void);
void fn_800C990C(void);
void fn_800CF2E0(void);
void fn_800CF390(void);
void fn_80157730(void);
void fn_8015860C(void);
void fn_80158744(void);
void fn_8016F818(void);
void fn_80170348(void);
void fn_801D7BC8(void);
void fn_801D7C24(void);
void fn_801D8134(void);
void fn_801D8368(void);
void fn_801E0850(void);
void fn_801E08FC(void);
void fn_801E4830(void);
void fn_801E4A74(void);
void fn_80201F5C(void);
void fn_80202090(void);
void fn_80228608(void);
void fn_80228AE4(void);
void fn_80237690(void);
void fn_802379CC(void);
#ifdef __cplusplus
}
#endif
void ActorAnimTableClear(void);
void ActorFileHandlerInit(void);
void ActorNoOp24518(void);
void ActorPendingMsgsInit(void);
void AnimTrackInit(void);
void AnimTrackReset(void);
void CamTuneInit(void);
void CameraSystemInit(void);
void ClockInitRenderState(void);
void EffectEmitterInit(void);
void EffectEmitterShutdown(void);
void EmitterDestroyAll(void);
void EmitterSystemShutdown(void);
void FileInit(void);
void FileShutdown(void);
void FlickerInit(void);
void FlickerResetCount(void);
void FrameCounterReset(void);
void FxFreeScrollSources(void);
void FxListFree(void);
void FxListInit(void);
void FxResetScrollSources(void);
void HazardSplineClearAll(void);
void HazardZoneBufferInit(void);
void HookNop2(void);
void InitLoadManager(void);
void LightFxFreeAll(void);
void LightFxInit(void);
void MainNop(void);
void MirrorObjInitModule(void);
void ModelPhysicsShutdown(void);
void ModelSystemInit(void);
void OLTFree(void);
void OLTInit(void);
void PadManagerInit(void);
void PadRecordStop(void);
void Q12Noop(void);
void QueueInit(void);
void QueueShutdown(void);
void RegisterAuxMsgHandlers(void);
void ResetLoadManager(void);
void SAMInit(void);
void SAMStop(void);
void SaveInit(void);
void SaveNop(void);
void SaveRegister(void);
void ShotArrayInit(void);
void ShotArrayShutdown(void);
void SltGroupReset(void);
void SltInit(void);
void StoreNop(void);
void StoreTrue(void);
void StrTableFree(void);
void StrTableLoad(void);
void TimerInit(void);
void TimerNop(void);
void VarInit(void);
void VarReset(void);
void fn_8007EE68(void);
void fn_8007F3A4(void);

void LinkObjSysInit(void);
void LinkObjSysShutdown(void);
int fn_80087428(void);

// TODO (not drafted yet, 0x720 bytes at 0x80086B9C): fn_80086B9C, the "AppMain" loop (pre-load, fade in, FPS counter, fade out)
// TODO (not drafted yet, 0x3C8 bytes at 0x80087428): fn_80087428, loading/splash screen state machine
// TODO (not drafted yet, 0x314 bytes at 0x80087BB0): fn_80087BB0
// TODO (not drafted yet, 0x47C bytes at 0x800880F8): fn_800880F8, chunk 0x38 handler
void fn_800880F8(int a, int size, int* out, int b);
void fn_80085490(void);
void PadRecordStart(int pad);
void ActorPlayAnim(int a, u32 b, int c);
static int sPadRecord;

#ifdef __cplusplus
extern "C" {
#endif
// 0x800872BC: debug print, compiled out in the retail build
void fn_800872BC(int channel, const char* format, ...)
{
}
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif
int fn_8000AEF4(void);
void SoundSetsAlloc(void);
void fn_800071AC(void);
void fn_80020F58(void);
void fn_80028060(void);
void fn_80028A38(void);
void fn_8004B2A8(void);
void fn_8004B378(void);
void fn_800848F4(void);
void fn_80085A94(void);
void fn_800938C0(void);
void fn_800BA970(void);
void fn_800BA994(void);
void fn_800C81DC(void);
void fn_800C8944(void);
void fn_800CF33C(void);
void fn_801582AC(void);
void fn_801584F8(void);
void fn_801D7BA8(void);
void fn_801D8168(void);
void fn_801D81F4(void);
void fn_801D9028(void);
void fn_801E08CC(void);
void fn_80202038(void);
#ifdef __cplusplus
}
#endif
void ActorCounterClear(void);
void ActorNoOp24514(void);
void AlwaysTrue(void);
void AnimTrackTimerStart(void);
void CamTuneNop(void);
void EffectTablesLoad(void);
void EmitterNotifyRelease(void);
void EmitterSystemInit(void);
void FlickerSpawnAll(void);
void FxListBroadcast(void);
void FxListSpawnAll(void);
void HazardSplineNoop(void);
void LightFxSpawnAll(void);
void ModelPhysicsStart(void);
void SaveClearBusy(void);
void StrTableRelease(void);
void StreamNop(void);

// Start-up: initialises every game system in order.
int AppSystemsInit(void)
{
    int init[4];
    if (fn_8000AEF4() == 0) {
        return 0;
    }
    {
        StreamNop();
        fn_800848F4();
        ActorCounterClear();
        ModelPhysicsStart();
        ActorNoOp24514();
        AlwaysTrue();
        fn_800938C0();
        fn_80020F58();
        fn_801D7BA8();
        fn_801E08CC();
        StrTableRelease();
        EffectTablesLoad();
        EmitterSystemInit();
        fn_80202038();
        fn_800C8944();
        fn_800BA994();
        FlickerSpawnAll();
        FxListSpawnAll();
        LightFxSpawnAll();
        HazardSplineNoop();
        fn_801D81F4();
        fn_8004B378();
        fn_80028060();
        SoundSetsAlloc();
        fn_801D9028();
        AnimTrackTimerStart();
        ChunkStart();
        fn_801584F8();
        CamTuneNop();
        fn_80085A94();
        SaveClearBusy();
        fn_8004B2A8();
        fn_800C81DC();
        fn_800BA970();
        fn_80028A38();
        FxListBroadcast();
        EmitterNotifyRelease();
        fn_801D8168();
        fn_800CF33C();
        fn_801582AC();
        init[0] = 0;
        init[1] = 0;
        fn_800880F8(0, 0x800, init, 0);
        fn_80085490();
        fn_80087428();
        PadRecordStart(sPadRecord);
        fn_800071AC();
        ActorPlayAnim(0x2000000, 0xFEF24188, 0x2000000);
    }
    return 1;
}

// Shutdown: tears every game system down.
void AppSystemsShutdown(void)
{
    fn_80157730();
    fn_80049F70();
    SaveNop();
    ActorNoOp24518();
    AudioShutdown();
    FlickerResetCount();
    fn_800849B8();
    ActorAnimTableClear();
    fn_800CF2E0();
    fn_801E4830();
    fn_800BA8A0();
    Q12Noop();
    fn_80093710();
    fn_80237690();
    EmitterDestroyAll();
    fn_800C843C();
    LightFxFreeAll();
    fn_800C6DD4();
    TimerNop();
    FxListFree();
    HazardSplineClearAll();
    fn_801D8134();
    fn_800289F8();
    LinkObjSysShutdown();
    SAMStop();
    AnimTrackReset();
    OLTFree();
    SltGroupReset();
    ResetLoadManager();
    QueueInit();
    EffectEmitterShutdown();
    ShotArrayShutdown();
    fn_801D7BC8();
    fn_801E0850();
    PadRecordStop();
    fn_80005A38();
    FxFreeScrollSources();
    fn_80228608();
    fn_8000AE40();
    ModelPhysicsShutdown();
    FileShutdown();
    StoreNop();
    StrTableFree();
    fn_8016F818();
    fn_800266A0();
    fn_8007CA44();
    fn_80201F5C();
    VarReset();
    fn_8007EE68();
    fn_80020280();
}

// Start-up part two: brings the systems up that need the file handlers.
int AppSystemsStart(void)
{
    fn_8007CB14();
    StoreTrue();
    fn_800267EC();
    FxResetScrollSources();
    fn_8000AFB0(0);
    fn_80228AE4();
    ChunkTableWalk();
    fn_80170348();
    ModelSystemInit();
    fn_80005CEC();
    PadManagerInit();
    StrTableLoad();
    fn_80085A98();
    fn_801E08FC();
    fn_8007F3A4();
    fn_801D7C24();
    LinkObjSysInit();
    RegisterAuxMsgHandlers();
    AudioInit();
    fn_800C6DD8();
    fn_800C990C();
    CameraSystemInit();
    HookNop2();
    MirrorObjInitModule();
    ClockInitRenderState();
    fn_80020694();
    fn_800C8EC8();
    LightFxInit();
    EmitterSystemShutdown();
    fn_800938C4();
    fn_802379CC();
    ActorFileHandlerInit();
    fn_8004BD64();
    fn_800BA9BC();
    fn_80084B34();
    ActorPendingMsgsInit();
    fn_800CF390();
    fn_801E4A74();
    FlickerInit();
    TimerInit();
    FxListInit();
    HazardZoneBufferInit();
    fn_801D8368();
    fn_80028140();
    QueueShutdown();
    InitLoadManager();
    SltInit();
    OLTInit();
    SAMInit();
    AnimTrackInit();
    MainNop();
    ChunkInit();
    SaveRegister();
    VarInit();
    fn_80202090();
    EffectEmitterInit();
    CamTuneInit();
    ShotArrayInit();
    fn_80026660(0x38, (void*)fn_800880F8, 0);
    fn_8015860C();
    FileInit();
    return 1;
}

extern "C" void fn_800880D0(void)
{
    FrameCounterReset();
    SaveInit();
    fn_80158744();
}

void GfxFrameEnd(void);
void GfxFrameSwap(void);
void GfxFrameBegin(void);
void TextSetBox(int x, int y, int w, int h);
void TextSetStyle(int r, int g, int b, int a, int font, int c, int d, int e, int f, int g2, int h, int i);
void TextDraw(const char* s, u32 len);
void Draw2DBegin(void);
void Draw2DEnd(void);
void DrawTexQuad(int tex, float x0, float y0, float x1, float y1, u8 r, u8 g, u8 b, u8 a);
void GfxGetViewportRect(short* rect);
void GfxSetCopyFilter(int on);
#ifdef __cplusplus
extern "C" {
#endif
unsigned long strlen(const char* s);
void fn_80009E10(void);
void fn_801EAEAC(void);
void fn_80115A34(int a, int b, short* rect);
void fn_80006D20(void);
void fn_80006558(int a);
#ifdef __cplusplus
}
#endif
extern const char* lbl_804D2A04[2];  // text drawn on the start-up frames
extern int lbl_804D2A00;          // texture of the start-up frames (-1 = none)
extern int gWideScreen;
extern const float lbl_804D686C;
extern const float lbl_804D687C;
extern const float lbl_804D6880;
extern const float lbl_804D6884;
extern const float lbl_804D6888;

// Draws the two start-up frames (text and picture) and restores the viewport.
extern "C" void fn_80088574(void)
{
    const char** text;
    int i;
    short rect[8];
    GfxFrameEnd();
    i = 0;
    text = lbl_804D2A04;
    for (; i != 2; i++) {
        GfxFrameSwap();
        TextSetBox(0x1B8, 0x26C, 0x14, 0x3C);
        TextSetStyle(0xFF, 0xFF, 0xFF, 0xFF, 0xE, 0, 0, 0, 2, 0, 0, 0);
        TextDraw(text[0], strlen(text[0]) + 1);
        Draw2DBegin();
        if (lbl_804D2A00 != -1) {
            if (gWideScreen != 0) {
                DrawTexQuad(lbl_804D2A00, lbl_804D687C, lbl_804D686C, lbl_804D6880, lbl_804D6884, 0xFF, 0xFF, 0xFF, 0xFF);
            } else {
                DrawTexQuad(lbl_804D2A00, lbl_804D686C, lbl_804D686C, lbl_804D6888, lbl_804D6884, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
        Draw2DEnd();
        GfxFrameBegin();
    }
    GfxGetViewportRect(rect);
    fn_80009E10();
    fn_801EAEAC();
    GfxSetCopyFilter(0);
    fn_80115A34(0, 1, rect);
    fn_80006D20();
    fn_80006558(1);
}

// Layout of one split-screen configuration (count of views, one value per view, then the two overlays).
struct ViewSet {
    int mCount;       // 0x00
    int mView[4];     // 0x04
    int mUnk14;       // 0x14
    int mUnk18[5];    // 0x18
    u8 mUnk2C[0x38 - 0x2C];  // 0x2C
    int mUnk38;       // 0x38
    int mUnk3C[5];    // 0x3C
    u8 mUnk50[0x5C - 0x50];  // 0x50
    int mMode;        // 0x5C
};

#ifdef __cplusplus
extern "C" {
#endif
int fn_802A1E90(void);
ViewSet* fn_802A1EBC(int layout);
int fn_800FC3B8(int what);
int fn_800C5E74(int view);
void fn_80009E44(int a, int b);
void fn_80008E5C(int a);
void fn_8015779C(float dt, int pass, int view, int flag);
void fn_8000761C(int a, int* b);
void fn_80007400(int a, int* b);
int fn_80220A38(void);
void fn_80215A24(void);
void fn_80215654(void);
void fn_8000ADD8(void);
#ifdef __cplusplus
}
#endif
ViewSet* LevelSlotCurrent(void);

extern int lbl_804D46D8;  // letterbox borders enabled
extern int lbl_804D29F0;  // border width
extern int lbl_804D29F4;  // border height
extern int lbl_804D29F8;  // border colour
extern int lbl_804D29FC;
extern int lbl_804D46DC;
extern int lbl_804D46E0;

// Renders one frame: every active view of the current layout, then the overlays and the borders.
void AppRenderFrame(float dt)
{
    int* p;
    int i;
    ViewSet* set;
    int flag;
    int layout;
    layout = fn_802A1E90();
    set = LevelSlotCurrent();
    GfxFrameEnd();
    GfxFrameSwap();
    if (layout <= 6) {
        if (set->mCount == 2) {
            if (gWideScreen != 0) {
                set = fn_802A1EBC(5);
            } else {
                set = fn_802A1EBC(1);
            }
        } else if (set->mCount == 3) {
            if (gWideScreen != 0) {
                if (fn_800FC3B8(4) != 0) {
                    set = fn_802A1EBC(6);
                } else {
                    set = fn_802A1EBC(2);
                }
            } else {
                if (fn_800FC3B8(4) != 0) {
                    set = fn_802A1EBC(4);
                } else {
                    set = fn_802A1EBC(2);
                }
            }
        }
    }
    {
        u32 x = set->mCount ^ 1;
        flag = (u32)(((int)x >> 1) - (x & set->mCount)) >> 31;
    }
    for (p = (int*)set, i = 0; i < set->mCount; i++, p++) {
        if (fn_800C5E74(i) != 0) {
            fn_80009E44(p[1], 0);
            fn_80008E5C(set->mMode);
            fn_8015779C(dt, 0, i, flag);
        }
    }
    fn_800C5E74(0);
    fn_80009E44(0, 0);
    fn_80008E5C(0);
    if (set->mUnk14 != 0) {
        fn_8000761C(set->mUnk14, set->mUnk18);
    }
    if (set->mUnk38 != 0) {
        fn_80007400(set->mUnk38, set->mUnk3C);
    }
    fn_8015779C(dt, 1, -1, flag);
    if (fn_80220A38() == 4) {
        fn_80215A24();
    }
    fn_80215654();
    if (lbl_804D46D8 != 0) {
        Draw2DBegin();
        DrawTexQuad(-3, lbl_804D686C, lbl_804D686C, (float)lbl_804D29F0, lbl_804D6884, lbl_804D29F8, lbl_804D46DC, lbl_804D46E0, lbl_804D29FC);
        DrawTexQuad(-3, (float)(640 - lbl_804D29F0), lbl_804D686C, lbl_804D6888, lbl_804D6884, lbl_804D29F8, lbl_804D46DC, lbl_804D46E0, lbl_804D29FC);
        DrawTexQuad(-3, (float)lbl_804D29F0, (float)(480 - lbl_804D29F4), (float)(640 - lbl_804D29F0), lbl_804D6884, lbl_804D29F8, lbl_804D46DC, lbl_804D46E0, lbl_804D29FC);
        DrawTexQuad(-3, (float)lbl_804D29F0, lbl_804D686C, (float)(640 - lbl_804D29F0), (float)lbl_804D29F4, lbl_804D29F8, lbl_804D46DC, lbl_804D46E0, lbl_804D29FC);
        Draw2DEnd();
    }
    GfxFrameBegin();
    fn_8000ADD8();
}

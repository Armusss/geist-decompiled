// u_80080F88.cpp: animation event track system (pool, markers, visibility).

typedef unsigned long u32;

struct AnimTrackTable;
struct AnimEventSlot;

// Unit data (this file's .sdata/.sbss range).
extern int lbl_804D29C4;
extern int lbl_804D29CC;
extern int lbl_804D4624;

extern "C" int fn_800C64E4(void);
extern "C" void fn_80080D88(void);
extern "C" int SendMsg(int to, int msg, void* data);
extern u32 AssetFind(u32 id, u32 asset);
extern int ActorPlayAnim(int actor, u32 anim, int flags);

// More state in this unit's .sdata/.sbss/.bss.
extern int lbl_804D29C0;
extern int lbl_804D29D0;
extern float lbl_804D29D4;
extern float lbl_804D4618;
extern int lbl_804D462C;
extern AnimTrackTable* lbl_804D4630;
extern AnimEventSlot* lbl_804D4634;
extern int lbl_804D4658;
extern int lbl_804D465C;
extern int lbl_804D4660;
extern int lbl_804D4664;
extern int lbl_804D4668;
extern float lbl_804D6818;
extern float lbl_804D681C;
extern int gWideScreen;
extern char lbl_8042DA90;

// Renderer/loading-screen configuration block (this unit's .bss).  Only the
// fade field at +0x34 is touched by this file.
struct RenderConfig {
    int unk00[13];  // +0x00
    float fade;     // +0x34
    int unk38[10];  // +0x38
};
extern RenderConfig lbl_8043CA90;

extern "C" int fn_800C6664(int);
extern "C" void fn_800C6834(int);
extern "C" int fn_800C69B8(void* config, int arg1, int arg2);
extern "C" int fn_800D8BDC(int);
extern "C" int fn_8003B98C(int);
extern "C" int fn_80080864(int, void*, void*, int);
extern "C" void fn_80080B50(int, int, int);
extern "C" void* fn_800807C8(int, int);
extern "C" int fn_80080E24(void* buf, int size, int count, int arg);
extern "C" void fn_8001BB00(void);
extern "C" int fn_80007138(void);
extern "C" void fn_80008BC8(const char* msg);
extern "C" void fn_8001BAA8(int);
extern "C" void fn_80080714(int);
extern "C" int fn_8003C008(int, int, int, int, int);
extern "C" void fn_8004FD70(void*);
extern "C" void fn_8017BDC0(void);
extern "C" void fn_800070C0(void*);
extern "C" void FileFlushBuffer(void*, int);
extern "C" int strcmp(const char*, const char*);

void FontRelease(int font);
void CreditsTipClear(void);
void* GetPlayer(void);
void* VAlloc(int size);
void VFree(void* p);

void DebugPrintf(const char* fmt, ...);
void ScreenPrintf(const char* fmt, ...);
void SystemPoll(void);
void SltUnloadAll(void);
void SltLoadIndex(int index, int value);
void SltRelease(int id);
int FontLoad(int bank, const char* name, int arg2, int arg3, int arg4, int arg5);

typedef int (*FileHandler)(void* owner, int msg, void* data);
void FileAddHandler(int type, FileHandler handler, void* data);

// Animation event track state (this file's .bss/.sbss range).
// One group holds a variable number of timed 16-byte track entries.
struct AnimTrackEntry {
    int time;   // +0x0
    int pad4;   // +0x4
    u32 asset;  // +0x8
    int anim;   // +0xC
};

struct AnimTrackGroup {
    int id;                       // +0x0
    int count;                    // +0x4
    AnimTrackEntry entries[];     // +0x8
};

struct AnimTrackTable {
    int pad0;  // +0x0
    int count; // +0x4
};

// Payload of message 0x3002. The receiver fills unk0.
struct AnimTrackEvent {
    int unk0;
    int time;
    int limit;
    int anim;
};

// Arguments/results block for the actor query helper.
struct QueryArgs {
    int a;  // +0x0
    int b;  // +0x4
    int c;  // +0x8
};

struct AnimTrackSlot;
struct AnimTrackSet;
struct AnimEventSlot;

extern short lbl_804D29C8;
extern int lbl_804D461C;
extern void* lbl_804D4620;
extern AnimTrackSlot* lbl_804D4628;
extern AnimTrackTable* lbl_804D4638;
extern AnimEventSlot* lbl_804D463C;
extern AnimTrackTable* lbl_804D4640;
extern AnimTrackGroup* lbl_804D4644;
extern AnimTrackGroup* lbl_804D4648;
extern AnimTrackEntry* lbl_804D464C;
extern AnimTrackEntry* lbl_804D4650;
extern int lbl_804D4654;

// Matrix / actor types shared with the rest of the game code.
// Actor head: only the position fields touched by this file.
struct Actor {
    char pad00[0x24];  // +0x00
    float posX;        // +0x24
    float posY;        // +0x28
    float posZ;        // +0x2C
};

struct Mtx34 {
    float m[3][4];
};

char* ModelAnimate(int model, char* boneMap, int arg);
void ModelSetMatrix(int model, Mtx34* mtx, char* data);

void ActorGetMatrix(Mtx34* m, Actor* a);
int HookFalse0(void);

extern float lbl_804D6820;
extern double lbl_804D6828;
extern float lbl_804D6830;

extern "C" void fn_8009F76C(Mtx34* m);
extern "C" void fn_800A089C(Mtx34* m, int* rect);
extern "C" void fn_8009F63C(Mtx34* out, Mtx34* in);
extern "C" Actor* fn_800C6510(int id);

// Per-bone pose: three translation floats then four scale floats.
struct AnimBoneData {
    float f[7];
};

// One slot of the per-actor track pool.  The pool base is lbl_804D4628 and
// every slot is 0x1BD8 bytes.
struct AnimTrackSlot {
    int id;                   // +0x0
    short index;              // +0x4
    signed char active;       // +0x6
    signed char prevActive;   // +0x7
    AnimTrackSet* set;        // +0x8
    AnimEventSlot* link;      // +0xC
    AnimBoneData bones[254];  // +0x10
};

// Pose / marker list hung off a track slot (+0x8).  Entries start at +0x1C
// and are variable-size: type 1 is a position (0x10 bytes), type 2 a screen
// rectangle (0xC bytes).
struct AnimTrackSet {
    int unk00;              // +0x0
    int unk04;              // +0x4
    unsigned short flags;   // +0x8
    unsigned short count;   // +0xA
    u32 mask[4];            // +0xC
};

// Scene node walked by AnimTrackBuildList; entries follow at +0x10.
struct AnimSceneEntry {
    int id;                 // +0x0
    unsigned short count;   // +0x4
    unsigned short pad06;   // +0x6
    unsigned short flags;   // +0x8
};

struct AnimScene {
    int unk00;  // +0x0
    int unk04;  // +0x4
    int id;     // +0x8
    int count;  // +0xC
};

// Trigger slot array at lbl_804D463C (stride 0x18 + 4 * count).  `dirty` is
// set when the visibility bits must be pushed out even if nothing changed.
struct AnimEventSlot {
    int id;                    // +0x0
    unsigned char count;       // +0x4
    unsigned char dirty;       // +0x5
    signed char state;         // +0x6
    signed char prevState;     // +0x7
    u32 bits[4];               // +0x8
    int ids[];                 // +0x18
};

// Resource name intern table (lbl_804D4674) and the per-type resource buckets.
struct ResNameEntry {
    char* name;      // +0x0
    char pad04[8];   // +0x4
};

struct ResNameTable {
    int unk00;              // +0x0
    short count;            // +0x4
    short pad06;            // +0x6
    ResNameEntry* entries;  // +0x8
};

struct ResNode {
    int id;              // +0x0
    int type;            // +0x4
    ResNameEntry* name;  // +0x8
    int value;           // +0xC
    ResNode* next;       // +0x10
};

struct ResBucket {
    ResNode* nodes[16];
};

extern ResNameTable* lbl_804D4674;
extern ResNode* lbl_8043EBA0[];
extern ResBucket lbl_8043ED30[];
extern int lbl_804D29D8;
extern "C" ResNode* fn_8007C868(int type, int size);

// Argument of the visibility message 0xC00.
struct AnimVisArg {
    int unk0;  // +0x0
    int on;    // +0x4
};

void AnimTrackApplyPose(void);
void AnimTrackUpdateScene(void);
void AnimTrackSyncVisibility(int model, int target, AnimTrackSet* set, AnimEventSlot* state);
int AnimTrackFileGroups(void* data, int size);
int AnimTrackFileTracks(void* data, int size);
int AnimTrackQueryCallback(void* obj, int* args);

// One marker in an animation track pose set.  Type 1 entries carry a position,
// type 2 entries carry a screen rectangle in their first four halfwords.
struct AnimTrackMarker {
    signed char type;   // +0x0
    char pad01[3];      // +0x1
    float x;            // +0x4
    float y;            // +0x8
    float z;            // +0xC
};

struct AnimTrackRect {
    int x;  // +0x0
    int y;  // +0x4
    int w;  // +0x8
    int h;  // +0xC
};

// Object behind message 0x300A (queried by AnimTrackQueryCallback).
struct AnimTrackQueryObj {
    int unk00;   // +0x0
    int unk04;   // +0x4
    int time;    // +0x8
};

// Visibility payload of message 0xC00.
struct AnimTrackVis {
    int unk0;  // +0x0
    int on;    // +0x4
};

// Returns the current global value (a mode/state id).
int Fn80080F88(void) {
    return lbl_804D29C4;
}

// Message handler: message 1 requests flag bit 1 in the shared sbss word.
void Fn80080F90(void* owner, int msg) {
    if (msg != 1) {
        return;
    }
    lbl_804D4624 |= 2;
}

// True when the time source has reached the recorded target value.
int Fn80080FA8(void) {
    return (u32)__cntlzw((u32)fn_800C64E4() - (u32)lbl_804D29CC) >> 5;
}

// No animation track active: tear the track state down.
void AnimTrackReset(void) {
    if ((u32)lbl_804D29C4 != (u32)-1) {
        fn_80080D88();
    }
}

// Select the animation track group `id` and set the shared time offset to
// `offset`. Any entries already queued in the previous track are flushed
// first by sending message 0x3002.
void AnimTrackSelect(int id, int offset) {
    if (lbl_804D29C8 == -1) {
        return;
    }
    if (lbl_804D4648 != 0) {
        while ((int)(lbl_804D4650 - lbl_804D464C) < lbl_804D4648->count) {
            AnimTrackEvent ev;
            u32 asset;

            ev.time = lbl_804D4650->time + lbl_804D4654;
            ev.limit = lbl_804D461C;
            ev.anim = lbl_804D4650->anim;
            asset = AssetFind((u32)-1, lbl_804D4650->asset);
            SendMsg(asset, 0x3002, &ev);
            lbl_804D4650++;
        }
    }
    if (lbl_804D4644 != 0) {
        AnimTrackGroup* group = lbl_804D4644;
        int count = lbl_804D4640->count;
        int i = 0;

        while (i < count) {
            if (group->id == id) {
                break;
            }
            i++;
            group = (AnimTrackGroup*)&group->entries[group->count];
        }
        if (i == count) {
            lbl_804D4648 = 0;
            lbl_804D4650 = 0;
            lbl_804D464C = 0;
        } else {
            lbl_804D4648 = group;
            lbl_804D464C = group->entries;
            lbl_804D4650 = group->entries;
        }
    }
    lbl_804D4654 = offset;
}

// Play every queued track entry whose timestamp has been reached.
void AnimTrackUpdate(void) {
    AnimTrackEntry* entry;
    AnimTrackEvent ev;
    u32 asset;

    if (lbl_804D29C8 == -1) {
        return;
    }
    if (lbl_804D4648 == 0) {
        return;
    }
    for (;;) {
        if ((int)(lbl_804D4650 - lbl_804D464C) >= lbl_804D4648->count) {
            return;
        }
        entry = lbl_804D4650;
        if (entry->time + lbl_804D4654 > lbl_804D461C) {
            return;
        }
        ev.time = entry->time + lbl_804D4654;
        ev.limit = lbl_804D461C;
        ev.anim = entry->anim;
        asset = AssetFind((u32)-1, entry->asset);
        if (ActorPlayAnim(asset, entry->anim, 0x2C000000) == 0) {
            SendMsg(asset, 0x3002, &ev);
        }
        lbl_804D4650++;
    }
}

// Per-frame update of the animation-event track system plus the level
// preload / loading-screen debug flow.  `dt` is the frame delta in seconds.
void AnimTrackFrameUpdate(float dt) {
    float delta = dt * lbl_804D29D4;
    int found;

    if (fn_800D8BDC(0x5A) == 1) {
        DebugPrintf("Preload: ");
        SltUnloadAll();
        lbl_804D4668 = 1;
        SltLoadIndex(lbl_804D465C, 0x80);
    }
    if (fn_800D8BDC(0x58) == 1) {
        DebugPrintf("Play it..");
        if (lbl_804D4668 == 0) {
            SltUnloadAll();
        } else {
            lbl_804D4668 = 0;
        }
        SltLoadIndex(lbl_804D465C, 0);
        lbl_804D4660 = 1;
    }
    if (lbl_804D29C8 == -1) {
        return;
    }
    if (lbl_804D4658 != 0) {
        ScreenPrintf("RTA Frame: %d", lbl_804D461C);
    }
    if (fn_8003B98C(lbl_804D29C8) == 0) {
        return;
    }
    if (lbl_804D4628 == 0) {
        SltRelease(lbl_804D462C);
        SltRelease(lbl_804D462C);
        lbl_804D29C8 = -1;
        return;
    }

    {
        AnimTrackEvent ev;

        SendMsg(0x2D000000, 0x300A, &ev);
        if ((lbl_804D4624 & 2) == 0 || lbl_804D461C < ev.limit - 1) {
            QueryArgs q = { -1, 0, 1 };

            found = fn_80080864(lbl_804D29C4, &q, (void*)AnimTrackQueryCallback, 8);
            if (found == -1) {
                if (q.a != -1) {
                    found = 0;
                }
            } else if (q.b != q.c) {
                found = (found > q.c) ? found - q.c : 0;
            }
            if (found == -1) {
                if (lbl_804D461C > 0) {
                    SltRelease(lbl_804D462C);
                    SltRelease(lbl_804D462C);
                    lbl_804D29C8 = -1;
                }
                return;
            }
        } else if ((lbl_804D4624 & 4) == 0) {
            if ((lbl_804D4624 & 0x10) == 0) {
                lbl_804D4624 |= 0x10;
                SltRelease(lbl_804D462C);
                lbl_804D29C8 = -1;
                ActorPlayAnim(0x4E000000, 0xDC649061, 0x4E000000);
                return;
            }
        } else {
            lbl_804D4624 &= ~4;
            if (lbl_804D29CC == fn_800C64E4()) {
                fn_800C6664(lbl_804D29CC);
                if (lbl_804D29D0 != 0xFF) {
                    fn_800C6834(lbl_804D29D0);
                }
            }
            found = 0;
        }
    }

    fn_80080B50(lbl_804D29C4, found, 0);
    lbl_804D4620 = fn_800807C8(lbl_804D29C4, 0);
    AnimTrackUpdateScene();
    if (lbl_804D4624 & 1) {
        lbl_804D4624 &= ~1;
    }
    AnimTrackApplyPose();
    lbl_804D4618 = lbl_804D4618 + delta;
    if (lbl_804D4660 != 0) {
        fn_8001BB00();
        while (fn_80007138() != 0) {
            fn_80008BC8("Loading...");
            SystemPoll();
        }
        lbl_804D4660 = 0;
    }

    {
        int level = -1;

        if (lbl_804D29C8 != -1) {
            level = lbl_804D461C;
        }
        if (level >= 0 && gWideScreen != lbl_804D4664) {
            if (gWideScreen == 0) {
                lbl_804D29C0 = FontLoad(0, "LBox On", 5, 0, 0, 0);
                lbl_8043CA90.fade = lbl_804D6818;
            } else {
                lbl_804D29C0 = FontLoad(0, "LBox On W", 5, 0, 0, 0);
                lbl_8043CA90.fade = lbl_804D6818;
            }
            lbl_804D4664 = gWideScreen;
        }
    }
}

// Record the current time-source value and reset the configure fade.
void AnimTrackTimerStart(void) {
    lbl_804D29CC = fn_800C69B8(&lbl_8043CA90, 0, 0);
    lbl_8043CA90.fade = lbl_804D6818;
}

// Set up the animation-track pool, clear the shared state and register the
// two file message handlers.
void AnimTrackInit(void) {
    lbl_804D29C4 = fn_80080E24(&lbl_8042DA90, 0x800, 0x1E, 1);
    lbl_804D4618 = lbl_804D681C;
    lbl_804D4624 = 0;
    lbl_804D461C = 0;
    lbl_804D29C8 = -1;
    lbl_804D462C = -1;
    lbl_804D4628 = 0;
    lbl_804D4630 = 0;
    lbl_804D4634 = 0;
    lbl_804D4638 = 0;
    lbl_804D463C = 0;
    lbl_804D4640 = 0;
    lbl_804D4644 = 0;
    lbl_804D4648 = 0;
    lbl_804D4650 = 0;
    lbl_804D464C = 0;

    FileAddHandler(0x1D, (FileHandler)AnimTrackFileTracks, 0);
    FileAddHandler(0x1E, (FileHandler)AnimTrackFileGroups, 0);
}

// Current track time, or -1 when no track is running.
int Fn800816B0(void) {
    if (lbl_804D29C8 == -1) {
        return -1;
    }
    return lbl_804D461C;
}

// Apply the marker entries of the active slot's pose set to the scene actor:
// type 1 entries record a position, type 2 entries concatenate a screen
// rectangle onto the actor matrix.
void AnimTrackApplyPose(void) {
    float x;
    float y;
    float z;
    Mtx34 mtx;
    AnimTrackSet* set;
    AnimTrackMarker* marker;
    AnimTrackRect* rect;
    AnimTrackRect rc;
    int count;
    int idx;
    int i;
    int off;

    if (lbl_804D4620 == 0) {
        return;
    }
    fn_8009F76C(&mtx);
    x = lbl_804D681C;
    y = x;
    z = x;
    count = lbl_804D4638->count;
    idx = 0;
    while (idx < count) {
        if (lbl_804D4628[idx].id == -1) {
            break;
        }
        if (lbl_804D4628[idx].id == 0x4C281E91) {
            break;
        }
        idx++;
    }
    if (idx >= count || lbl_804D4628[idx].id == -1) {
        return;
    }
    if (lbl_804D4628[idx].active == 0) {
        return;
    }
    set = lbl_804D4628[idx].set;
    off = 0x1C;
    for (i = 0; i < set->count; i++) {
        marker = (AnimTrackMarker*)((char*)lbl_804D4628[idx].set + off);
        switch (marker->type) {
        case 1:
            x = marker->x;
            y = marker->y;
            z = marker->z;
            off += 0x10;
            break;
        case 2:
            rect = (AnimTrackRect*)((char*)marker + 4);
            rc.x = rect->x;
            rc.y = rect->y;
            rc.w = rect->w;
            rc.h = rect->h;
            fn_800A089C(&mtx, (int*)&rc);
            fn_8009F63C(&mtx, &mtx);
            off += 0xC;
            break;
        }
    }
    if ((set->flags & 4) != 0 && lbl_804D4628[idx].index < 7) {
        AnimScene* scene = (AnimScene*)fn_800807C8(lbl_804D29C4, lbl_804D4628[idx].index + 1);
        AnimTrackSet* next;

        if (scene == 0) {
            Actor* actor = fn_800C6510(lbl_804D29CC);

            ActorGetMatrix(&mtx, actor);
            actor->posX = x;
            actor->posY = y;
            actor->posZ = z;
            return;
        }
        next = (AnimTrackSet*)((char*)scene + 0x10);
        off = 0x1C;
        for (i = 0; i < next->count; i++) {
            marker = (AnimTrackMarker*)((char*)next + off);
            switch (marker->type) {
            case 1:
                x = marker->x;
                y = marker->y;
                z = marker->z;
                off += 0x10;
                break;
            case 2:
                rect = (AnimTrackRect*)((char*)marker + 4);
                rc.x = rect->x;
                rc.y = rect->y;
                rc.w = rect->w;
                rc.h = rect->h;
                fn_800A089C(&mtx, (int*)&rc);
                fn_8009F63C(&mtx, &mtx);
                off += 0xC;
                break;
            }
        }
    }
    {
        Actor* actor = fn_800C6510(lbl_804D29CC);

        ActorGetMatrix(&mtx, actor);
        actor->posX = x;
        actor->posY = y;
        actor->posZ = z;
    }
}

// Walk the scene graph behind lbl_804D4620 and mark every active slot of the
// matching node, then fire the trigger transitions recorded in lbl_804D463C.
void AnimTrackUpdateScene(void) {
    AnimScene* node;
    AnimSceneEntry* entry;
    AnimEventSlot* trig;
    AnimTrackSlot* slots;
    AnimTrackVis vis;
    int data[4];
    int nodeId;
    int curId;
    int i;
    int j;
    int k;
    int off;
    int n;
    u32 asset;

    if (lbl_804D4620 == 0) {
        return;
    }
    trig = lbl_804D463C;
    for (i = 0; i < lbl_804D4638->count; i++) {
        trig->state = 0;
        lbl_804D4628[i].active = 0;
        trig = (AnimEventSlot*)((char*)trig + 0x18 + 4 * trig->count);
    }
    node = (AnimScene*)lbl_804D4620;
    nodeId = node->id;
    curId = 0;
    for (i = 0; i < 8; i++) {
        for (j = 0, off = 0; j < node->count; j++) {
            entry = (AnimSceneEntry*)((char*)node + 0x10 + off);
            if (j == 0 && entry->id == curId) {
                if ((entry->flags & 4) == 0) {
                    curId = 0;
                }
            } else {
                if ((entry->flags & 4) != 0) {
                    curId = entry->id;
                }
                if (entry->id == 0x4C281E91) {
                    if (lbl_804D29CC != fn_800C64E4() && HookFalse0() == 0) {
                        lbl_804D29D0 = fn_800C64E4();
                        fn_800C6834(lbl_804D29CC);
                    }
                }
                slots = lbl_804D4628;
                n = lbl_804D4638->count;
                for (k = 0; k < n; k++) {
                    if (slots[k].id == entry->id) {
                        break;
                    }
                }
                if (k != n) {
                    slots[k].active = 1;
                    slots[k].index = i;
                    slots[k].set = (AnimTrackSet*)entry;
                    slots[k].link->state = 1;
                }
            }
            off += 4 * entry->count;
        }
        node = (i == 7) ? 0 : (AnimScene*)fn_800807C8(lbl_804D29C4, i + 1);
        if (node == 0 || node->id != nodeId) {
            break;
        }
    }
    trig = lbl_804D463C;
    for (i = 0; i < lbl_804D4638->count; i++) {
        if (trig->state != trig->prevState && trig->id != 0x4C281E91) {
            switch (trig->state) {
            case 0:
                asset = AssetFind((u32)-1, trig->id);
                if (asset != (u32)-1) {
                    SendMsg(asset, 0x300B, data);
                    vis.on = 0;
                    SendMsg(asset, 0xC00, &vis);
                }
                break;
            case 1:
                asset = AssetFind((u32)-1, trig->id);
                if (asset != (u32)-1) {
                    vis.on = 1;
                    SendMsg(asset, 0xC00, &vis);
                    SendMsg(asset, 0x3000, data);
                }
                break;
            }
            trig->prevState = trig->state;
        }
        trig = (AnimEventSlot*)((char*)trig + 0x18 + 4 * trig->count);
    }
}

// Query callback: reports the first reached marker time and counts how many
// actors share it.
int AnimTrackQueryCallback(void* obj, int* args) {
    AnimTrackQueryObj* q = (AnimTrackQueryObj*)obj;

    if (q == 0) {
        return 0;
    }
    if ((float)q->time * lbl_804D6820 <= lbl_804D4618) {
        if (args[0] == -1) {
            args[0] = q->time;
            args[1] = 1;
            args[2] = q->unk04;
        } else if (args[0] != q->time) {
            lbl_804D461C = args[0];
            return 1;
        } else {
            args[1]++;
        }
    }
    return 0;
}

// Teardown tail of AnimTrackStop.  The retail build repeats it verbatim at
// each of the three exit points, so it is a macro rather than a call.
#define ANIM_TRACK_STOP_CLEANUP()                                      \
    do {                                                               \
        int n;                                                         \
        if (lbl_804D29CC == fn_800C64E4()) {                           \
            fn_800C6664(lbl_804D29CC);                                 \
            if (lbl_804D29D0 != 0xFF) {                                \
                fn_800C6834(lbl_804D29D0);                             \
            }                                                          \
        }                                                              \
        for (n = 0; n < lbl_804D4638->count; n++) {                    \
            lbl_804D4628[n].id = -1;                                   \
        }                                                              \
        if (lbl_804D4628 != 0) {                                       \
            VFree(lbl_804D4628);                                       \
            lbl_804D4628 = 0;                                          \
        }                                                              \
        if (flag) {                                                    \
            CreditsTipClear();                                         \
        }                                                              \
        lbl_804D29C8 = -1;                                             \
    } while (0)

// Stop the running animation track: release the level resources, flush the
// pending track entries and free the slot pool.
void AnimTrackStop(int level, int handle, int arg, int flags) {
    AnimTrackEvent data;
    int flag;
    int i;

    if (lbl_804D29C0 != -1) {
        FontRelease(lbl_804D29C0);
        if (gWideScreen == 0) {
            FontLoad(0, "LBox Off", 5, 0, 0, 0);
        } else {
            FontLoad(0, "LBox Off W", 5, 0, 0, 0);
        }
        lbl_804D4664 = 0;
    }
    if (lbl_804D4628 == 0) {
        return;
    }
    fn_8001BAA8(1);
    flag = flags & 1;
    lbl_804D4624 &= ~2;
    lbl_804D4624 &= ~8;
    if (flag) {
        ActorPlayAnim(0x4E000000, 0xDC649061, 0x4E000000);
    }
    fn_80080714(lbl_804D29C4);
    if (arg == 0 || fn_8003C008(lbl_804D29C4, 0x52, level, handle, arg) != 0) {
        ANIM_TRACK_STOP_CLEANUP();
    } else {
        lbl_804D4620 = fn_800807C8(lbl_804D29C4, 0);
        if (lbl_804D4620 == 0) {
            ANIM_TRACK_STOP_CLEANUP();
        } else {
            AnimTrackUpdateScene();
            if (lbl_804D4624 & 1) {
                lbl_804D4624 &= ~1;
            }
            for (i = 0; i < lbl_804D4638->count; i++) {
                u32 asset = AssetFind((u32)-1, (u32)lbl_804D4628[i].id);

                if (asset != (u32)-1) {
                    SendMsg(asset, 0x3001, &data);
                }
            }
            lbl_804D461C = ((AnimScene*)lbl_804D4620)->id;
            if (lbl_804D29C8 != -1 && lbl_804D4648 != 0) {
                while ((int)(lbl_804D4650 - lbl_804D464C) <
                       lbl_804D4648->count) {
                    AnimTrackEvent ev;
                    u32 asset;

                    ev.time = lbl_804D4650->time + lbl_804D4654;
                    ev.limit = lbl_804D461C;
                    ev.anim = lbl_804D4650->anim;
                    asset = AssetFind((u32)-1, lbl_804D4650->asset);
                    if (ActorPlayAnim(asset, lbl_804D4650->anim,
                                      0x2C000000) == 0) {
                        SendMsg(asset, 0x3002, &ev);
                    }
                    lbl_804D4650++;
                }
            }
            fn_80080714(lbl_804D29C4);
            ANIM_TRACK_STOP_CLEANUP();
        }
    }
}

// Apply the pose set of the slot bound to `actor` and copy `count` bones of
// the resulting pose into `out`.
int AnimTrackAnimate(int actor, int model, int count, void* out) {
    AnimTrackSlot* slot;
    AnimTrackSet* set;
    unsigned char* marker;
    unsigned char* map;
    int i;
    int searching;
    int chained;
    int offset;

    if (lbl_804D4620 == 0) {
        return 0;
    }
    if (lbl_804D4624 & 1) {
        return 0;
    }
    i = 0;
    searching = 1;
    offset = 0;
    while (searching == 1) {
        slot = (AnimTrackSlot*)((char*)lbl_804D4628 + offset);
        if (i >= lbl_804D4638->count) {
            return 0;
        }
        if (slot->id == -1) {
            return 0;
        }
        if (AssetFind((u32)-1, (u32)slot->id) == (u32)actor) {
            if (slot->active == 0) {
                return 0;
            }
            searching = 0;
        }
        offset += 0x1BD8;
        i++;
    }
    set = slot->set;
    map = (unsigned char*)ModelAnimate(model, (char*)slot + 0x10,
                                       slot->prevActive);
    slot->prevActive = 0;
    AnimTrackSyncVisibility(model, actor, set, slot->link);
    chained = 0;
    marker = (unsigned char*)set + 0x1C;
    do {
        for (i = 0; i < set->count; i++) {
            if ((*(unsigned int*)marker & 0x00FF0000) != 0) {
                unsigned char t = marker[1];

                marker[1] = marker[3];
                marker[3] = t;
            }
            switch (*(signed char*)marker) {
            case 1: {
                unsigned int w = *(unsigned int*)marker;
                int idx = (int)(w << 8) >> 8;

                if (idx >= 0) {
                    int bone = map[idx];

                    if (bone != 0xFF) {
                        float* f = slot->bones[bone].f;

                        f[4] = *(float*)(marker + 4);
                        f[5] = *(float*)(marker + 8);
                        f[6] = *(float*)(marker + 0xC);
                    }
                }
                marker += 0x10;
                break;
            }
            case 2: {
                unsigned int w = *(unsigned int*)marker;
                int idx = (int)(w << 8) >> 8;

                if (idx >= 0) {
                    int bone = map[idx];

                    if (bone != 0xFF) {
                        float* f = slot->bones[bone].f;

                        f[0] = (float)*(signed short*)(marker + 4) *
                               lbl_804D6830;
                        f[1] = (float)*(signed short*)(marker + 6) *
                               lbl_804D6830;
                        f[2] = (float)*(signed short*)(marker + 8) *
                               lbl_804D6830;
                        f[3] = (float)*(signed short*)(marker + 0xA) *
                               lbl_804D6830;
                    }
                }
                marker += 0xC;
                break;
            }
            }
        }
        if (chained) {
            chained = 0;
        } else if ((set->flags & 4) != 0 && slot->index < 7) {
            AnimScene* node =
                (AnimScene*)fn_800807C8(lbl_804D29C4, slot->index + 1);

            if (node != 0) {
                set = (AnimTrackSet*)((char*)node + 0x10);
                marker = (unsigned char*)node + 0x2C;
                chained = 1;
            }
        }
    } while (chained);
    for (i = 0; i < count; i++) {
        ((AnimBoneData*)out)[i] = slot->bones[i];
    }
    ModelSetMatrix(model, 0, (char*)out);
    return 1;
}

// Payload of message 0xC02: visibility state of one resource id.
struct AnimVisMsg {
    int unk0;  // +0x0
    int id;    // +0x4
    int bit;   // +0x8
};

// Push the visibility bits of `set` to `target` (message 0xC02), for every id
// when the slot is dirty or only for the bits that changed otherwise.
void AnimTrackSyncVisibility(int model, int target, AnimTrackSet* set,
                             AnimEventSlot* state) {
    AnimVisMsg msg;
    int i = 0;
    int b;
    int n;
    int id;
    int bit;

    if (state == 0) {
        return;
    }
    if (state->dirty & 1) {
        n = 0;
        for (i = 0; i < 4 && n < state->count; i++) {
            for (b = 0; b < 32 && n < state->count; b++, n++) {
                id = state->ids[n];
                bit = (set->mask[i] >> b) & 1;
                if (id != -1) {
                    msg.id = id;
                    msg.bit = bit;
                    SendMsg(target, 0xC02, &msg);
                }
                state->bits[i] = (state->bits[i] & ~(1u << b)) | (bit << b);
            }
        }
        state->dirty &= ~1;
    } else {
        for (i = 0; i < 4; i++) {
            if (set->mask[i] != state->bits[i]) {
                break;
            }
        }
        if (i == 4) {
            return;
        }
        n = 0;
        for (i = 0; i < 4 && n < state->count; i++) {
            for (b = 0; b < 32 && n < state->count; b++, n++) {
                bit = (set->mask[i] >> b) & 1;
                if (((state->bits[i] >> b) & 1) != bit) {
                    id = state->ids[n];
                    if (id != -1) {
                        msg.id = id;
                        msg.bit = bit;
                        SendMsg(target, 0xC02, &msg);
                    }
                    state->bits[i] =
                        (state->bits[i] & ~(1u << b)) | (bit << b);
                }
            }
        }
    }
}

// Start the animation track `id` for the current level: allocate the slot
// pool, bind every trigger slot and queue the matching file group.
int AnimTrackStart(int level, int handle, int trackId) {
    AnimEventSlot* group;
    AnimEventSlot* next;
    AnimTrackGroup* grp;
    AnimTrackEntry* entries;
    int count;
    int i;
    int j;

    group = lbl_804D4634;
    fn_8004FD70(GetPlayer());
    if (lbl_804D4630 == 0 || lbl_804D4634 == 0) {
        return 0;
    }
    next = lbl_804D4634;
    for (i = 0; i < lbl_804D4630->count; i++) {
        if (group->id == trackId) {
            lbl_804D4638 = (AnimTrackTable*)group;
            break;
        }
        next = (AnimEventSlot*)((char*)next + 8);
        for (j = 0; j < group->count; j++) {
            next = (AnimEventSlot*)((char*)next + 4 * next->count + 0x18);
        }
        group = next;
    }
    lbl_804D4628 = (AnimTrackSlot*)VAlloc(group->count * 0x1BD8);
    if (lbl_804D4628 == 0) {
        return 0;
    }
    if (i == lbl_804D4630->count) {
        return 0;
    }
    lbl_804D4638 = (AnimTrackTable*)group;
    lbl_804D463C = next;
    for (j = 0; j < group->count; j++) {
        next->state = 0;
        next->prevState = -1;
        next->dirty = 1;
        lbl_804D4628[j].id = next->id;
        lbl_804D4628[j].link = next;
        lbl_804D4628[j].active = 0;
        lbl_804D4628[j].prevActive = 1;
        next = (AnimEventSlot*)((char*)next + 4 * next->count + 0x18);
    }
    fn_8017BDC0();
    fn_8001BAA8(0);
    lbl_804D4654 = 0;
    if (lbl_804D4644 != 0) {
        grp = lbl_804D4644;
        entries = (AnimTrackEntry*)grp;
        count = lbl_804D4640->count;
        for (i = 0; i < count; i++) {
            entries = (AnimTrackEntry*)((char*)entries + 8);
            if (grp->id == trackId) {
                break;
            }
            entries = (AnimTrackEntry*)((char*)entries + 16 * grp->count);
            grp = (AnimTrackGroup*)entries;
        }
        if (i == count) {
            lbl_804D4648 = 0;
            lbl_804D4650 = 0;
            lbl_804D464C = 0;
        } else {
            lbl_804D4648 = grp;
            lbl_804D464C = entries;
            lbl_804D4650 = entries;
        }
    }
    lbl_804D4624 = 0;
    lbl_804D4624 |= 1;
    lbl_804D29C8 = level;
    lbl_804D462C = handle;
    lbl_804D4618 = lbl_804D681C;
    lbl_804D461C = 0;
    lbl_804D4624 |= 4;
    if (gWideScreen == 0) {
        lbl_804D29C0 = FontLoad(0, "LBox On", 5, 0, 0, 0);
        lbl_8043CA90.fade = lbl_804D6818;
    } else {
        lbl_804D29C0 = FontLoad(0, "LBox On W", 5, 0, 0, 0);
        lbl_8043CA90.fade = lbl_804D6818;
    }
    lbl_804D4664 = gWideScreen;
    ActorPlayAnim(0x4E000000, 0xB1211487, 0x4E000000);
    return 1;
}

// File handler for the animation group table.
int AnimTrackFileGroups(void* data, int size) {
    AnimTrackGroup* group;
    AnimTrackGroup* next;
    int i;

    lbl_804D4640 = (size > 0) ? (AnimTrackTable*)data : 0;
    group = (size - 8 > 0) ? (AnimTrackGroup*)((char*)data + 8) : 0;
    lbl_804D4644 = group;
    FileFlushBuffer(lbl_804D4640, 8);
    next = group;
    for (i = 0; i < lbl_804D4640->count; i++) {
        fn_800070C0(group);
        next = (AnimTrackGroup*)((char*)next + 8 + 16 * group->count);
        group = next;
    }
    if (lbl_804D4640->pad0 != 1) {
        lbl_804D4640 = 0;
    }
    return 1;
}

// File handler for the animation track table.
int AnimTrackFileTracks(void* data, int size) {
    AnimEventSlot* group;
    AnimEventSlot* next;
    int i;
    int j;

    lbl_804D4630 = (size > 0) ? (AnimTrackTable*)data : 0;
    FileFlushBuffer(lbl_804D4630, 8);
    if (lbl_804D4630->pad0 != 3) {
        lbl_804D4630 = 0;
        return 1;
    }
    group = (size - 8 > 0) ? (AnimEventSlot*)((char*)data + 8) : 0;
    lbl_804D4634 = group;
    next = group;
    for (i = 0; i < lbl_804D4630->count; i++) {
        fn_800070C0(group);
        next = (AnimEventSlot*)((char*)next + 8);
        for (j = 0; j < group->count; j++) {
            next = (AnimEventSlot*)((char*)next + 4 * next->count + 0x18);
        }
        group = next;
    }
    return 1;
}

// Bind the resource described by the reader at reader+4 into the three
// resource type buckets and return its name-table entry.
void* AnimTrackBindResource(void* reader) {
    char* header = *(char**)((char*)reader + 4);
    u32 id;
    char* name;
    int value = 0;
    ResNameEntry* entry = 0;
    int i;

    if (((u32)header & 3) == 0) {
        id = *(u32*)header;
    }
    if ((((u32)header + 4) & 3) == 0) {
        name = *(char**)(header + 4);
    }
    if ((((u32)header + 8) & 3) == 0) {
        value = *(int*)(header + 8);
    }
    if ((int)id != -1) {
        if (lbl_804D4674 == 0) {
            entry = 0;
        } else {
            entry = lbl_804D4674->entries;
            i = 0;
            while (i < lbl_804D4674->count) {
                if (strcmp(entry->name, name) == 0) {
                    break;
                }
                entry++;
                i++;
            }
            if (i == lbl_804D4674->count) {
                entry = 0;
            }
        }
    }
    if (entry != 0) {
        if ((int)id >= 0) {
            ResNode* node = lbl_8043ED30[id >> 24].nodes[id & 0xF];

            while (node != 0) {
                if (node->id == (int)id && node->type == 0x7000) {
                    break;
                }
                node = node->next;
            }
            if (node == 0) {
                node = fn_8007C868(lbl_804D29D8, 0x14);
                node->next = lbl_8043ED30[id >> 24].nodes[id & 0xF];
                lbl_8043ED30[id >> 24].nodes[id & 0xF] = node;
            }
            node->id = id;
            node->name = entry;
            node->type = 0x7000;
            node->value = value;
        }
        if ((int)id >= 0) {
            ResNode* node = lbl_8043ED30[id >> 24].nodes[id & 0xF];

            while (node != 0) {
                if (node->id == (int)id && node->type == 0x7002) {
                    break;
                }
                node = node->next;
            }
            if (node == 0) {
                node = fn_8007C868(lbl_804D29D8, 0x14);
                node->next = lbl_8043ED30[id >> 24].nodes[id & 0xF];
                lbl_8043ED30[id >> 24].nodes[id & 0xF] = node;
            }
            node->id = id;
            node->name = entry;
            node->type = 0x7002;
            node->value = value;
        }
        if ((int)id >= 0) {
            ResNode* node = lbl_8043ED30[id >> 24].nodes[id & 0xF];

            while (node != 0) {
                if (node->id == (int)id && node->type == 0x7003) {
                    break;
                }
                node = node->next;
            }
            if (node == 0) {
                node = fn_8007C868(lbl_804D29D8, 0x14);
                node->next = lbl_8043ED30[id >> 24].nodes[id & 0xF];
                lbl_8043ED30[id >> 24].nodes[id & 0xF] = node;
            }
            node->id = id;
            node->name = entry;
            node->type = 0x7003;
            node->value = value;
        }
    }
    *(ResNameEntry**)((char*)reader + 0xC) = entry;
    return entry;
}

// Bind a resource of the type stored in the reader's header.
void* AnimTrackBindResourceId(void* reader) {
    char* header = *(char**)((char*)reader + 4);
    u32 id;
    int type;
    char* name;
    ResNameEntry* entry = 0;
    int i;

    if (((u32)header & 3) == 0) {
        id = *(u32*)header;
    }
    if ((((u32)header + 4) & 3) == 0) {
        type = *(int*)(header + 4);
    }
    if ((((u32)header + 8) & 3) == 0) {
        name = *(char**)(header + 8);
    }
    if ((int)id != -1) {
        if (lbl_804D4674 == 0) {
            entry = 0;
        } else {
            entry = lbl_804D4674->entries;
            i = 0;
            while (i < lbl_804D4674->count) {
                if (strcmp(entry->name, name) == 0) {
                    break;
                }
                entry++;
                i++;
            }
            if (i == lbl_804D4674->count) {
                entry = 0;
            }
        }
    }
    if (entry != 0 && (int)id >= 0) {
        ResNode* node = lbl_8043ED30[id >> 24].nodes[id & 0xF];

        while (node != 0) {
            if (node->id == (int)id && node->type == type) {
                break;
            }
            node = node->next;
        }
        if (node == 0) {
            node = fn_8007C868(lbl_804D29D8, 0x14);
            node->next = lbl_8043ED30[id >> 24].nodes[id & 0xF];
            lbl_8043ED30[id >> 24].nodes[id & 0xF] = node;
        }
        node->id = id;
        node->name = entry;
        node->type = type;
        node->value = 0;
    }
    *(ResNameEntry**)((char*)reader + 0xC) = entry;
    return entry;
}

// Bind a sub-resource: the id indexes the flat sub-resource bucket table.
void* AnimTrackBindResourceSub(void* reader) {
    char* header = *(char**)((char*)reader + 4);
    u32 id;
    int type;
    char* name;
    ResNameEntry* entry;
    int i;

    if (((u32)header & 3) == 0) {
        id = *(u32*)header;
    }
    if ((((u32)header + 4) & 3) == 0) {
        type = *(int*)(header + 4);
    }
    if ((((u32)header + 8) & 3) == 0) {
        name = *(char**)(header + 8);
    }
    if (lbl_804D4674 == 0) {
        entry = 0;
    } else {
        entry = lbl_804D4674->entries;
        i = 0;
        while (i < lbl_804D4674->count) {
            if (strcmp(entry->name, name) == 0) {
                break;
            }
            entry++;
            i++;
        }
        if (i == lbl_804D4674->count) {
            entry = 0;
        }
    }
    if (entry != 0) {
        ResNode* node = lbl_8043EBA0[id];

        while (node != 0) {
            if (node->id == (int)id && node->type == type) {
                break;
            }
            node = node->next;
        }
        if (node == 0) {
            node = fn_8007C868(lbl_804D29D8, 0x14);
            node->next = lbl_8043EBA0[id];
            lbl_8043EBA0[id] = node;
        }
        node->id = id;
        node->name = entry;
        node->type = type;
    }
    *(ResNameEntry**)((char*)reader + 0xC) = entry;
    return entry;
}

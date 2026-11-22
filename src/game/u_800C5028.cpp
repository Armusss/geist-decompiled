// u_800C5028.cpp (0x800C5028-0x800C6E8C): actor animation-state table lookup, the actor play-anim / anim-ref
// helpers, pending actor messages, and the camera slot system (128 slots at .bss 0x8044FE00, the per-view
// slot heads .data 0x8039B760, the camera tunables .sbss 0x804D48A8-0x804D48D0).
// The old range 0x800C5028-0x800D0254 held four files; cuts at 0x800C6E8C, 0x800C99E8 and 0x800CF2E0.
// Boundary evidence (end 0x800C6E8C): the next file's .sdata2 pool starts at 0x804D6F10 with its own
// int-to-float double (this file's is 0x804D6EE8, pool 0x804D6EE0-0x804D6F10), and it calls ActorPlayAnim
// (0x3C bytes, defined here) out of line, which CodeWarrior would auto-inline inside one file. The empty
// fn_800C6E88 (called from u_800F73C8) could sit on either side; kept here.
// Not split further: the anim/message helpers (0x800C5028-0x800C5920, .sbss 0x804D4890-0x804D48A8, .bss
// 0x8044F600) read no floats and nothing here calls them out of line, so there is no evidence for a cut
// before the camera code.
// Names are descriptive guesses; the code is matched against the retail DOL unless noted.

typedef unsigned long u32;
typedef signed long s32;
typedef unsigned char u8;

extern "C" {
void* memset(void* dst, int c, unsigned long n);
void qsort(void* base, unsigned long n, unsigned long size, int (*cmp)(const void*, const void*));
void fn_80026660(int msg, void* handler, int arg);
void fn_800933D0(int anim);
void fn_8009337C(int anim);
int fn_80084540(int to, int msg, void* data, void* result);
}
void AnimUnref(int anim);
void AnimRef(int anim);

// A sorted table entry: an animation key and the list of states that use it.
struct AnimTableEntry {
    u32 key;      // 0x00
    u32* states;  // 0x04
    int count;    // 0x08
};

// A message queued for later delivery (a pooled singly linked list).
struct PendingMsg {
    int arg0;          // 0x00
    int target;        // 0x04
    int arg1;          // 0x08
    PendingMsg* next;  // 0x0c
};

extern AnimTableEntry* lbl_804D4898;  // sorted animation table
extern int lbl_804D489C;              // entries in the table
extern int lbl_804D48A0;
extern PendingMsg* lbl_804D4890;      // queued messages
extern PendingMsg* lbl_804D4894;      // free pool
extern PendingMsg lbl_8044F600[64];   // message pool

// The camera slots: lbl_8039B760 maps a view number to a slot (-1 when unused).
struct CameraObj {
    char pad0[0x34];
    float fov;  // 0x34
};
struct CameraSlot {
    CameraObj* camera;                             // 0x00
    void (*update)(float dt, CameraObj*, int top);  // 0x04
    void (*callback)(CameraObj*);                  // 0x08
    int view;                                      // 0x0c
    int next[4];                                   // 0x10  next slot per view
    float lastTime;                                // 0x20
};
extern int lbl_8039B760[4];
extern int lbl_804D2BA0;  // first free slot
extern "C" float fn_802A1DA4();
void LinkInfoInit(void* p);
extern CameraSlot lbl_8044FE00[];
extern int lbl_804D48C4;  // current view
extern int lbl_804D48C8;  // view count
extern int lbl_804D4B98;
extern float lbl_804D48A8;
extern float lbl_804D48AC;
extern float lbl_804D48B0;
extern float lbl_804D48B4;
extern int lbl_804D48B8;
extern int lbl_804D48BC;
extern int lbl_804D48C0;
CameraObj* CameraTable();
extern "C" void fn_800C5E00(int view);

// Binary search of the animation table: 1 if the state list of `key` holds `state`, 0 if not,
// -1 if the key is missing.
static inline int AnimTableFind(u32 key, u32 state, int unused, AnimTableEntry* table, int lo, int hi,
                                int found)
{
    while (lo <= hi && found == -1) {
        int mid = (lo + hi) / 2;
        u32 k = table[mid].key;

        if (k == key) {
            found = mid;
        } else if (key < k) {
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }
    }
    if (found >= 0) {
        int i;

        table = table + found;
        for (i = 0; i < table->count; i++) {
            if (state == table->states[i]) {
                return 1;
            }
        }
        return 0;
    }
    return -1;
}

int AnimEntryCompare(const u32* a, const u32* b);
int ActorPendingMsgHandler(int self, int msg, int* data);

// 0x800C5028: does an animation key allow a state?
int ActorAnimState(int key, u32 state)
{
    AnimTableEntry* table = lbl_804D4898;
    int lo = 0;
    int hi = lbl_804D489C;
    int found = -1;

    if (table == 0) {
        return -1;
    }
    return AnimTableFind(key, state, 0, table, lo, hi, found);
}

// 0x800C50F0: empty
void ActorAnimNop()
{
}

// 0x800C50F4: installs the animation table, sorts it and drops the unused (-1) entries
void ActorAnimTableSet(int count, AnimTableEntry* table, u32* lists)
{
    int i;
    int dropped = 0;
    int offset = 0;
    int n = count;

    lbl_804D4898 = table;
    lbl_804D489C = count;
    if (count > 0) {
        do {
            AnimTableEntry* e = (AnimTableEntry*)((char*)lbl_804D4898 + offset);

            if (e->key == (u32)-1) {
                dropped++;
            }
            offset += 12;
            e->states = lists + (int)e->states;
            n--;
        } while (n != 0);
    }
    qsort(lbl_804D4898, count, 12, (int (*)(const void*, const void*))AnimEntryCompare);
    lbl_804D489C = count - dropped;
}

// 0x800C5190: qsort comparator for the animation table
int AnimEntryCompare(const u32* a, const u32* b)
{
    return (*a >= *b) ? 1 : -1;
}

// 0x800C51AC: empty
extern "C" void fn_800C51AC()
{
}

// 0x800C51B0: takes or drops an animation reference
void ActorAnimRef(int anim, int release)
{
    if (release != 0) {
        fn_8009337C(anim);
    } else {
        fn_800933D0(anim);
    }
}

// 0x800C51E0: takes or drops an animation reference (other pair)
void ActorAnimRef2(int anim, int release)
{
    if (release != 0) {
        AnimRef(anim);
    } else {
        AnimUnref(anim);
    }
}

// 0x800C5654: tells an actor to play an animation
void ActorPlayAnim(int actor, u32 key, int arg)
{
    int msg[3];
    char result[4];

    msg[0] = 0;
    msg[1] = key;
    msg[2] = arg;
    fn_80084540(actor, 0x7000, msg, result);
}

// 0x800C5690: clears the animation table
void ActorAnimTableClear()
{
    lbl_804D4898 = 0;
}

// 0x800C569C: delivers the queued play-animation messages and recycles them
void ActorPendingMsgsFlush()
{
    PendingMsg* m = lbl_804D4890;
    PendingMsg* n;
    int msg[3];
    char result[4];

    lbl_804D4890 = 0;
    while (m != 0) {
        msg[2] = m->arg1;
        msg[1] = m->arg0;
        msg[0] = 0;
        fn_80084540(m->target, 0x7000, msg, result);
        n = m;
        if (m != 0) {
            n = m->next;
            m->next = lbl_804D4894;
            lbl_804D4894 = m;
        }
        m = n;
    }
}

// 0x800C5724: clears a counter
void ActorCounterClear()
{
    lbl_804D48A0 = 0;
}

// 0x800C5730: builds the free pool of queued messages and registers the handler
void ActorPendingMsgsInit()
{
    int i;
    PendingMsg* head;

    memset(lbl_8044F600, 0, 0x800);
    lbl_804D4894 = 0;
    lbl_804D4890 = 0;
    head = 0;
    for (i = 0; i < 64; i++) {
        lbl_8044F600[i].next = head;
        head = &lbl_8044F600[i];
        lbl_804D4894 = head;
    }
    fn_80026660(0x4D, (void*)ActorPendingMsgHandler, 0);
}

// 0x800C5898: message handler that queues play-animation requests (message 0x7001)
int ActorPendingMsgHandler(int self, int msg, int* data)
{
    if (msg == 0x7001) {
        int arg1 = data[3];
        int arg0 = data[2];
        int target = data[1];
        PendingMsg* m;

        if (lbl_804D4894 == 0) {
            m = 0;
        } else {
            PendingMsg** link = &lbl_804D4890;
            PendingMsg* p;

            for (p = lbl_804D4890; p != 0; p = p->next) {
                link = &p->next;
            }
            *link = lbl_804D4894;
            lbl_804D4894 = lbl_804D4894->next;
            (*link)->next = 0;
            m = *link;
        }
        if (m != 0) {
            m->target = target;
            m->arg0 = arg0;
            m->arg1 = arg1;
        }
        return 1;
    }
    return 0;
}

// 0x800C5DD8: the current view's camera
extern "C" CameraObj* fn_800C5DD8()
{
    return lbl_8044FE00[lbl_8039B760[lbl_804D48C4]].camera;
}

// 0x800C5E00: runs a view's callback
extern "C" void fn_800C5E00(int view)
{
    if (lbl_8039B760[view] != -1) {
        CameraSlot* s = &lbl_8044FE00[lbl_8039B760[view]];

        if (s->callback != 0) {
            s->callback(s->camera);
        }
    }
}

// 0x800C5E5C: counts one more view
extern "C" void fn_800C5E5C()
{
    lbl_804D48C8 = lbl_804D48C8 + 1;
}

// 0x800C5E6C: the current view number
extern "C" int fn_800C5E6C()
{
    return lbl_804D48C4;
}

// 0x800C5E74: selects a view; true when it has a camera
extern "C" int fn_800C5E74(int view)
{
    lbl_804D48C4 = view;
    return lbl_8039B760[view] != -1;
}

// 0x800C63A0: a view's field of view
extern "C" float fn_800C63A0(int view)
{
    if (lbl_8039B760[view] != -1) {
        return lbl_8044FE00[lbl_8039B760[view]].camera->fov;
    }
    return 0.0f;
}

// 0x800C63D8: the current view's field of view
extern "C" float fn_800C63D8()
{
    if (lbl_8039B760[lbl_804D48C4] != -1) {
        return lbl_8044FE00[lbl_8039B760[lbl_804D48C4]].camera->fov;
    }
    return 0.0f;
}

// 0x800C6414: a view's camera or null
extern "C" CameraObj* fn_800C6414(int view)
{
    if (lbl_8039B760[view] != -1) {
        return lbl_8044FE00[lbl_8039B760[view]].camera;
    }
    return 0;
}

// 0x800C6448: the active camera (the shared one when lbl_804D4B98 is set)
extern "C" CameraObj* fn_800C6448()
{
    CameraObj* c;

    if (lbl_804D4B98 != 0) {
        c = CameraTable();
    } else {
        if (lbl_8039B760[lbl_804D48C4] == -1) {
            c = 0;
        } else {
            c = lbl_8044FE00[lbl_8039B760[lbl_804D48C4]].camera;
        }
    }
    return c;
}

// 0x800C64AC: the current view's camera or null
extern "C" CameraObj* fn_800C64AC()
{
    if (lbl_8039B760[lbl_804D48C4] != -1) {
        return lbl_8044FE00[lbl_8039B760[lbl_804D48C4]].camera;
    }
    return 0;
}

// 0x800C64E4: the current view's slot
extern "C" int fn_800C64E4()
{
    return lbl_8039B760[lbl_804D48C4];
}

// 0x800C64FC: a view's slot
extern "C" int fn_800C64FC(int view)
{
    return lbl_8039B760[view];
}

// 0x800C6510: a slot's camera or null
extern "C" CameraObj* fn_800C6510(int slot)
{
    if (slot > -1) {
        return lbl_8044FE00[slot].camera;
    }
    return 0;
}

// 0x800C6534
extern "C" void fn_800C6534(float v)
{
    lbl_804D48AC = v;
}

// 0x800C653C
extern "C" void fn_800C653C(int a, int b, int c)
{
    lbl_804D48B8 = a;
    lbl_804D48BC = b;
    lbl_804D48C0 = c;
}

// 0x800C654C
extern "C" void fn_800C654C(float a, float b)
{
    lbl_804D48B0 = a;
    lbl_804D48B4 = b;
}

// 0x800C6558
extern "C" void fn_800C6558(float v)
{
    lbl_804D48A8 = v;
}

// An actor, as far as the target-tracking code needs it (offsets only).
struct TargetPoint {
    int x;  // 0x24
    int y;  // 0x28
    int z;  // 0x2c
};
struct Actor {
    char pad0[0x84];
    float pos[3];       // 0x84
    char pad90[0x218 - 0x90];
    int unk218;         // 0x218
    int unk21C;         // 0x21c
    int targetMode;     // 0x220
    int unk224;         // 0x224
    int unk228;         // 0x228
    union {
        float point[3]; // 0x22c  mode 1: a point
        TargetPoint* target;  // mode 2: a target object (the point is at +0x24)
    };
    int unk238;         // 0x238
    int unk23C;         // 0x23c
};

// Takes a slot out of its view's list; true when it was found (or the list ran out).
static inline int SlotUnlink(int slot, int view)
{
    int cur = lbl_8039B760[view];
    int found;

    if (cur == -1) {
        found = 0;
    } else if (slot == cur) {
        lbl_8039B760[view] = lbl_8044FE00[slot].next[view];
        found = 1;
    } else {
        int n;

        do {
            n = lbl_8044FE00[cur].next[view];
            if (slot == n) {
                lbl_8044FE00[cur].next[view] = lbl_8044FE00[slot].next[view];
                found = 1;
                goto done;
            }
            cur = n;
        } while (n != -1);
        found = 1;
    }
done:
    return found;
}

// Appends a slot to the end of a view's list.
static inline void SlotAppend(int slot, int view)
{
    int cur = lbl_8039B760[view];

    lbl_8044FE00[slot].next[view] = -1;
    if (cur != -1) {
        int prev;

        do {
            prev = cur;
            cur = lbl_8044FE00[prev].next[view];
        } while (lbl_8044FE00[prev].next[view] != -1);
        lbl_8044FE00[prev].next[view] = slot;
        return;
    }
    lbl_8039B760[view] = slot;
}

// 0x800C6560: moves a slot to the end of a view's list
extern "C" void fn_800C6560(int slot, int view)
{
    if (slot < 0) {
        return;
    }
    if (!SlotUnlink(slot, view)) {
        return;
    }
    SlotAppend(slot, view);
}

// 0x800C6664: moves a slot to the end of its own view's list
extern "C" void fn_800C6664(int slot)
{
    int view;

    if (slot < 0) {
        return;
    }
    view = lbl_8044FE00[slot].view;
    if (slot < 0) {
        return;
    }
    if (!SlotUnlink(slot, view)) {
        return;
    }
    SlotAppend(slot, view);
}

// 0x800C6760: moves a slot to the front of a view's list
extern "C" void fn_800C6760(int slot, int view)
{
    if (slot < 0) {
        return;
    }
    if (SlotUnlink(slot, view)) {
        int head = lbl_8039B760[view];

        lbl_8039B760[view] = slot;
        lbl_8044FE00[slot].next[view] = head;
    }
}

// 0x800C6834: moves a slot to the front of its own view's list
extern "C" void fn_800C6834(int slot)
{
    int view;

    if (slot < 0) {
        return;
    }
    view = lbl_8044FE00[slot].view;
    if (slot < 0) {
        return;
    }
    if (SlotUnlink(slot, view)) {
        int head = lbl_8039B760[view];

        lbl_8039B760[view] = slot;
        lbl_8044FE00[slot].next[view] = head;
    }
}

// 0x800C6900: frees a slot
extern "C" void fn_800C6900(int slot)
{
    int view;
    int cur;

    if (slot < 0) {
        return;
    }
    view = lbl_8044FE00[slot].view;
    cur = lbl_8039B760[view];
    if (cur != -1) {
        if (slot == cur) {
            lbl_8039B760[view] = lbl_8044FE00[slot].next[view];
        } else {
            int n;

            do {
                n = lbl_8044FE00[cur].next[view];
                if (slot == n) {
                    lbl_8044FE00[cur].next[view] = lbl_8044FE00[slot].next[view];
                    break;
                }
                cur = n;
            } while (n != -1);
        }
    }
    lbl_8044FE00[slot].next[0] = lbl_804D2BA0;
    lbl_804D2BA0 = slot;
    lbl_8044FE00[slot].next[1] = -1;
    lbl_8044FE00[slot].next[2] = -1;
    lbl_8044FE00[slot].next[3] = -1;
}

// Fills in a freshly allocated camera slot.
static inline void SlotInit(CameraObj* cam, int slot, void (*update)(float, CameraObj*, int),
                            void (*callback)(CameraObj*))
{
    int* c = (int*)cam;

    LinkInfoInit(cam);
    c[0x30 / 4] = 0;
    *(float*)&c[0x34 / 4] = lbl_804D48A8;
    *(float*)&c[0x40 / 4] = lbl_804D48AC;
    *(float*)&c[0x44 / 4] = lbl_804D48B0;
    *(float*)&c[0x48 / 4] = lbl_804D48B4;
    c[0x4c / 4] = lbl_804D48B8;
    c[0x50 / 4] = lbl_804D48BC;
    c[0x54 / 4] = lbl_804D48C0;
    c[0x58 / 4] = -1;
    lbl_8044FE00[slot].camera = cam;
    lbl_8044FE00[slot].update = update;
    lbl_8044FE00[slot].callback = callback;
}

// 0x800C69B8: allocates a camera slot on view 0
extern "C" int fn_800C69B8(CameraObj* cam, void (*update)(float, CameraObj*, int),
                           void (*callback)(CameraObj*))
{
    int slot = lbl_804D2BA0;
    int head = lbl_8039B760[0];

    if (slot == -1) {
        slot = -1;
    } else {
        int next = lbl_8044FE00[slot].next[0];
        int isEmpty = head == -1;

        lbl_8044FE00[slot].view = 0;
        lbl_804D2BA0 = next;
        lbl_8044FE00[slot].next[0] = -1;
        if (isEmpty) {
            lbl_8039B760[0] = slot;
        } else {
            int prev;

            do {
                prev = head;
                head = lbl_8044FE00[prev].next[0];
            } while (lbl_8044FE00[prev].next[0] != -1);
            lbl_8044FE00[prev].next[0] = slot;
        }
    }
    if (slot != -1) {
        SlotInit(cam, slot, update, callback);
    }
    return slot;
}

// 0x800C6AE8: allocates a camera slot on the given view
extern "C" int fn_800C6AE8(CameraObj* cam, void (*update)(float, CameraObj*, int),
                           void (*callback)(CameraObj*), int view)
{
    int slot = lbl_804D2BA0;

    if (slot == -1) {
        slot = -1;
    } else {
        int head = lbl_8039B760[view];
        int next = lbl_8044FE00[slot].next[0];

        lbl_8044FE00[slot].view = view;
        lbl_804D2BA0 = next;
        lbl_8044FE00[slot].next[view] = -1;
        if (head == -1) {
            lbl_8039B760[view] = slot;
        } else {
            int prev;

            do {
                prev = head;
                head = lbl_8044FE00[prev].next[view];
            } while (lbl_8044FE00[prev].next[view] != -1);
            lbl_8044FE00[prev].next[view] = slot;
        }
    }
    if (slot != -1) {
        SlotInit(cam, slot, update, callback);
    }
    return slot;
}

// 0x800C6C28: runs every view's camera updates
extern "C" void fn_800C6C28(float dt)
{
    float now = fn_802A1DA4();
    int view;

    for (view = 0; view < 4; view++) {
        int cur = lbl_8039B760[view];

        while (cur != -1) {
            CameraSlot* s = &lbl_8044FE00[cur];
            int next = s->next[view];

            if (s->update != 0 && now != s->lastTime) {
                s->update(dt, s->camera, lbl_8039B760[view] == cur);
                s->lastTime = now;
            }
            cur = next;
        }
    }
}

// 0x800C6D08: runs one slot's camera update
extern "C" void fn_800C6D08(float dt, int slot)
{
    float now = fn_802A1DA4();

    if (slot > -1 && slot < 0x80) {
        CameraSlot* s = &lbl_8044FE00[slot];

        if (s->update != 0 && now != s->lastTime) {
            s->update(dt, s->camera, lbl_8039B760[lbl_804D48C4] == slot);
            s->lastTime = now;
        }
    }
}

// 0x800C6DD4: empty
extern "C" void fn_800C6DD4()
{
}

// 0x800C6DD8: resets the camera slots into one free list
extern "C" void fn_800C6DD8()
{
    int i;

    lbl_804D48A8 = 50.0f;
    lbl_804D48AC = 24000.0f;
    lbl_804D48B0 = 20000.0f;
    lbl_804D48B4 = 24000.0f;
    lbl_804D48B8 = 0;
    lbl_804D48BC = 0;
    lbl_804D48C0 = 0;
    lbl_804D48C4 = 0;
    lbl_804D48C8 = 0;
    for (i = 0; i < 0x7f; i++) {
        lbl_8044FE00[i].next[0] = i + 1;
        lbl_8044FE00[i].next[1] = -1;
        lbl_8044FE00[i].next[2] = -1;
        lbl_8044FE00[i].next[3] = -1;
    }
    lbl_8044FE00[i].next[0] = -1;
    lbl_8044FE00[i].next[1] = -1;
    lbl_8039B760[0] = -1;
    lbl_8044FE00[i].next[2] = -1;
    lbl_8044FE00[i].next[3] = -1;
    lbl_804D2BA0 = 0;
    lbl_8039B760[1] = -1;
    lbl_8039B760[2] = -1;
    lbl_8039B760[3] = -1;
}

// TODO (not drafted yet, 0x4 bytes at 0x800C50EC)
// fn_800C50EC
// TODO (not drafted yet, 0x444 bytes at 0x800C5210)
// fn_800C5210
// TODO (not drafted yet, 0x26C bytes at 0x800C5B6C)
// fn_800C5B6C
// TODO (not drafted yet, 0x118 bytes at 0x800C5E9C)
// fn_800C5E9C
// TODO (not drafted yet, 0x110 bytes at 0x800C5FB4)
// fn_800C5FB4
// TODO (not drafted yet, 0x13C bytes at 0x800C60C4)
// fn_800C60C4
// TODO (not drafted yet, 0x1A0 bytes at 0x800C6200)
// fn_800C6200
// TODO (not drafted yet, 0x4 bytes at 0x800C6E88)
// fn_800C6E88

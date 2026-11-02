// u_800C5028.cpp: the actor core: animation-state table lookup, actor message helpers,
// actor rendering, collision hooks and the actor type registry.
// Names are descriptive guesses; the code is matched against the retail DOL.

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

// 0x800C6E8C: the point an actor is aiming at; true when it is a real target
int ActorAimPoint(Actor* a, float* out)
{
    if (a->targetMode == 1) {
        out[0] = a->point[0];
        out[1] = a->point[1];
        out[2] = a->point[2];
        return 1;
    } else if (a->targetMode == 2 && a->target != 0) {
        out[0] = (float)a->target->x;
        out[1] = (float)a->target->y;
        out[2] = (float)a->target->z;
        return 1;
    } else {
        out[0] = a->pos[0];
        out[1] = a->pos[1];
        out[2] = a->pos[2];
        return 0;
    }
}

// 0x800C6F58: clears an actor's aim state
extern "C" int fn_800C6F58(Actor* a)
{
    if (a == 0) {
        return 0;
    }
    a->targetMode = 0;
    a->unk238 = -1;
    a->unk23C = -1;
    return 1;
}

// TODO (not drafted yet, 0x4 bytes at 0x800C50EC): actor/game routine
extern "C" void fn_800C50EC();







// TODO (not drafted yet, 0x444 bytes at 0x800C5210): actor/game routine
extern "C" void fn_800C5210();









// TODO (not drafted yet, 0x26C bytes at 0x800C5B6C): actor/game routine
extern "C" void fn_800C5B6C();






// TODO (not drafted yet, 0x118 bytes at 0x800C5E9C): actor/game routine
extern "C" void fn_800C5E9C();

// TODO (not drafted yet, 0x110 bytes at 0x800C5FB4): actor/game routine
extern "C" void fn_800C5FB4();

// TODO (not drafted yet, 0x13C bytes at 0x800C60C4): actor/game routine
extern "C" void fn_800C60C4();

// TODO (not drafted yet, 0x1A0 bytes at 0x800C6200): actor/game routine
extern "C" void fn_800C6200();
























// TODO (not drafted yet, 0x4 bytes at 0x800C6E88): actor/game routine
extern "C" void fn_800C6E88();



// TODO (not drafted yet, 0xA8 bytes at 0x800C6F84): actor/game routine
extern "C" void fn_800C6F84();

// TODO (not drafted yet, 0x14C bytes at 0x800C702C): actor/game routine
extern "C" void fn_800C702C();

// TODO (not drafted yet, 0xE4 bytes at 0x800C7178): actor/game routine
extern "C" void fn_800C7178();

// TODO (not drafted yet, 0x230 bytes at 0x800C725C): actor/game routine
extern "C" void fn_800C725C();

// TODO (not drafted yet, 0x464 bytes at 0x800C748C): actor/game routine
extern "C" void fn_800C748C();

// TODO (not drafted yet, 0x20 bytes at 0x800C78F0): actor/game routine
extern "C" void fn_800C78F0();

// TODO (not drafted yet, 0x20 bytes at 0x800C7910): actor/game routine
extern "C" void fn_800C7910();

// TODO (not drafted yet, 0x1E4 bytes at 0x800C7930): actor/game routine
extern "C" void fn_800C7930();

// TODO (not drafted yet, 0x48 bytes at 0x800C7B14): actor/game routine
extern "C" void fn_800C7B14();

// TODO (not drafted yet, 0x1C bytes at 0x800C7B5C): actor/game routine
extern "C" void fn_800C7B5C();

// TODO (not drafted yet, 0x170 bytes at 0x800C7B78): actor/game routine
extern "C" void fn_800C7B78();

// TODO (not drafted yet, 0x198 bytes at 0x800C7CE8): actor/game routine
extern "C" void fn_800C7CE8();

// TODO (not drafted yet, 0x318 bytes at 0x800C7E80): actor/game routine
extern "C" void fn_800C7E80();

// TODO (not drafted yet, 0x44 bytes at 0x800C8198): actor/game routine
extern "C" void fn_800C8198();

// TODO (not drafted yet, 0x1B8 bytes at 0x800C81DC): actor/game routine
extern "C" void fn_800C81DC();

// TODO (not drafted yet, 0x5C bytes at 0x800C8394): actor/game routine
extern "C" void fn_800C8394();

// TODO (not drafted yet, 0x4C bytes at 0x800C83F0): actor/game routine
extern "C" void fn_800C83F0();

// TODO (not drafted yet, 0x128 bytes at 0x800C843C): actor/game routine
extern "C" void fn_800C843C();

// TODO (not drafted yet, 0x3E0 bytes at 0x800C8564): actor/game routine
extern "C" void fn_800C8564();

// TODO (not drafted yet, 0x584 bytes at 0x800C8944): actor/game routine
extern "C" void fn_800C8944();

// TODO (not drafted yet, 0xA4 bytes at 0x800C8EC8): actor/game routine
extern "C" void fn_800C8EC8();

// TODO (not drafted yet, 0x4A8 bytes at 0x800C8F6C): actor/game routine
extern "C" void fn_800C8F6C();

// TODO (not drafted yet, 0x6C bytes at 0x800C9414): actor/game routine
extern "C" void fn_800C9414();

// TODO (not drafted yet, 0x64 bytes at 0x800C9480): actor/game routine
extern "C" void fn_800C9480();

// TODO (not drafted yet, 0x308 bytes at 0x800C94E4): actor/game routine
extern "C" void fn_800C94E4();

// TODO (not drafted yet, 0x44 bytes at 0x800C97EC): actor/game routine
extern "C" void fn_800C97EC();

// TODO (not drafted yet, 0x44 bytes at 0x800C9830): actor/game routine
extern "C" void fn_800C9830();

// TODO (not drafted yet, 0x98 bytes at 0x800C9874): actor/game routine
extern "C" void fn_800C9874();

// TODO (not drafted yet, 0x38 bytes at 0x800C990C): actor/game routine
extern "C" void fn_800C990C();

// TODO (not drafted yet, 0x24 bytes at 0x800C9944): actor/game routine
extern "C" void fn_800C9944();

// TODO (not drafted yet, 0x8 bytes at 0x800C9968): actor/game routine
extern "C" void fn_800C9968();

// TODO (not drafted yet, 0x78 bytes at 0x800C9970): actor/game routine
extern "C" void fn_800C9970();

// TODO (not drafted yet, 0x180 bytes at 0x800C99E8): actor/game routine
extern "C" void fn_800C99E8();

// TODO (not drafted yet, 0x88 bytes at 0x800C9B68): actor/game routine
extern "C" void fn_800C9B68();

// TODO (not drafted yet, 0x78 bytes at 0x800C9BF0): actor/game routine
extern "C" void fn_800C9BF0();

// TODO (not drafted yet, 0x6C bytes at 0x800C9C68): actor/game routine
extern "C" void fn_800C9C68();

// TODO (not drafted yet, 0x68 bytes at 0x800C9CD4): actor/game routine
extern "C" void fn_800C9CD4();

// TODO (not drafted yet, 0x194 bytes at 0x800C9D3C): actor/game routine
extern "C" void fn_800C9D3C();

// TODO (not drafted yet, 0x78 bytes at 0x800C9ED0): actor/game routine
extern "C" void fn_800C9ED0();

// TODO (not drafted yet, 0x78 bytes at 0x800C9F48): actor/game routine
extern "C" void fn_800C9F48();

// TODO (not drafted yet, 0xC0 bytes at 0x800C9FC0): actor/game routine
extern "C" void fn_800C9FC0();

// TODO (not drafted yet, 0x130 bytes at 0x800CA080): actor/game routine
extern "C" void fn_800CA080();

// TODO (not drafted yet, 0x998 bytes at 0x800CA1B0): actor/game routine
extern "C" void fn_800CA1B0();

// TODO (not drafted yet, 0xA0 bytes at 0x800CAB48): actor/game routine
extern "C" void fn_800CAB48();

// TODO (not drafted yet, 0x124 bytes at 0x800CABE8): actor/game routine
extern "C" void fn_800CABE8();

// TODO (not drafted yet, 0xD4 bytes at 0x800CAD0C): actor/game routine
extern "C" void fn_800CAD0C();

// TODO (not drafted yet, 0xADC bytes at 0x800CADE0): actor/game routine
extern "C" void fn_800CADE0();

// TODO (not drafted yet, 0x1E4 bytes at 0x800CB8BC): actor/game routine
extern "C" void fn_800CB8BC();

// TODO (not drafted yet, 0x650 bytes at 0x800CBAA0): actor/game routine
extern "C" void fn_800CBAA0();

// TODO (not drafted yet, 0x12C bytes at 0x800CC0F0): actor/game routine
extern "C" void fn_800CC0F0();

// TODO (not drafted yet, 0xB4 bytes at 0x800CC21C): actor/game routine
extern "C" void fn_800CC21C();

// TODO (not drafted yet, 0x38 bytes at 0x800CC2D0): actor/game routine
extern "C" void fn_800CC2D0();

// TODO (not drafted yet, 0x5C4 bytes at 0x800CC308): actor/game routine
extern "C" void fn_800CC308();

// TODO (not drafted yet, 0xB8 bytes at 0x800CC8CC): actor/game routine
extern "C" void fn_800CC8CC();

// TODO (not drafted yet, 0x104 bytes at 0x800CC984): actor/game routine
extern "C" void fn_800CC984();

// TODO (not drafted yet, 0x560 bytes at 0x800CCA88): actor/game routine
extern "C" void fn_800CCA88();

// TODO (not drafted yet, 0x5D0 bytes at 0x800CCFE8): actor/game routine
extern "C" void fn_800CCFE8();

// TODO (not drafted yet, 0x20 bytes at 0x800CD5B8): actor/game routine
extern "C" void fn_800CD5B8();

// TODO (not drafted yet, 0x608 bytes at 0x800CD5D8): actor/game routine
extern "C" void fn_800CD5D8();

// TODO (not drafted yet, 0x130 bytes at 0x800CDBE0): actor/game routine
extern "C" void fn_800CDBE0();

// TODO (not drafted yet, 0x618 bytes at 0x800CDD10): actor/game routine
extern "C" void fn_800CDD10();

// TODO (not drafted yet, 0x210 bytes at 0x800CE328): actor/game routine
extern "C" void fn_800CE328();

// TODO (not drafted yet, 0x224 bytes at 0x800CE538): actor/game routine
extern "C" void fn_800CE538();

// TODO (not drafted yet, 0x210 bytes at 0x800CE75C): actor/game routine
extern "C" void fn_800CE75C();

// TODO (not drafted yet, 0x218 bytes at 0x800CE96C): actor/game routine
extern "C" void fn_800CE96C();

// TODO (not drafted yet, 0x148 bytes at 0x800CEB84): actor/game routine
extern "C" void fn_800CEB84();

// TODO (not drafted yet, 0x13C bytes at 0x800CECCC): actor/game routine
extern "C" void fn_800CECCC();

// TODO (not drafted yet, 0xC0 bytes at 0x800CEE08): actor/game routine
extern "C" void fn_800CEE08();

// TODO (not drafted yet, 0x19C bytes at 0x800CEEC8): actor/game routine
extern "C" void fn_800CEEC8();

// TODO (not drafted yet, 0x18C bytes at 0x800CF064): actor/game routine
extern "C" void fn_800CF064();

// TODO (not drafted yet, 0x48 bytes at 0x800CF1F0): actor/game routine
extern "C" void fn_800CF1F0();

// TODO (not drafted yet, 0x60 bytes at 0x800CF238): actor/game routine
extern "C" void fn_800CF238();

// TODO (not drafted yet, 0x8 bytes at 0x800CF298): actor/game routine
extern "C" void fn_800CF298();

// TODO (not drafted yet, 0x20 bytes at 0x800CF2A0): actor/game routine
extern "C" void fn_800CF2A0();

// TODO (not drafted yet, 0x20 bytes at 0x800CF2C0): actor/game routine
extern "C" void fn_800CF2C0();

// TODO (not drafted yet, 0x4 bytes at 0x800CF2E0): actor/game routine
extern "C" void fn_800CF2E0();

// TODO (not drafted yet, 0x58 bytes at 0x800CF2E4): actor/game routine
extern "C" void fn_800CF2E4();

// TODO (not drafted yet, 0x54 bytes at 0x800CF33C): actor/game routine
extern "C" void fn_800CF33C();

// TODO (not drafted yet, 0x30 bytes at 0x800CF390): actor/game routine
extern "C" void fn_800CF390();

// TODO (not drafted yet, 0xF4 bytes at 0x800CF3C0): actor/game routine
extern "C" void fn_800CF3C0();

// TODO (not drafted yet, 0x460 bytes at 0x800CF4B4): actor/game routine
extern "C" void fn_800CF4B4();

// TODO (not drafted yet, 0x224 bytes at 0x800CF914): actor/game routine
extern "C" void fn_800CF914();

// TODO (not drafted yet, 0x40 bytes at 0x800CFB38): actor/game routine
extern "C" void fn_800CFB38();

// TODO (not drafted yet, 0x100 bytes at 0x800CFB78): actor/game routine
extern "C" void fn_800CFB78();

// TODO (not drafted yet, 0x8 bytes at 0x800CFC78): actor/game routine
extern "C" void fn_800CFC78();

// TODO (not drafted yet, 0x104 bytes at 0x800CFC80): actor/game routine
extern "C" void fn_800CFC80();

// TODO (not drafted yet, 0x120 bytes at 0x800CFD84): actor/game routine
extern "C" void fn_800CFD84();

// TODO (not drafted yet, 0x80 bytes at 0x800CFEA4): actor/game routine
extern "C" void fn_800CFEA4();

// TODO (not drafted yet, 0x8 bytes at 0x800CFF24): actor/game routine
extern "C" void fn_800CFF24();

// TODO (not drafted yet, 0x2C bytes at 0x800CFF2C): actor/game routine
extern "C" void fn_800CFF2C();

// TODO (not drafted yet, 0x140 bytes at 0x800CFF58): actor/game routine
extern "C" void fn_800CFF58();

// TODO (not drafted yet, 0x7C bytes at 0x800D0098): actor/game routine
extern "C" void fn_800D0098();

// TODO (not drafted yet, 0x24 bytes at 0x800D0114): actor/game routine
extern "C" void fn_800D0114();

// TODO (not drafted yet, 0xEC bytes at 0x800D0138): actor/game routine
extern "C" void fn_800D0138();

// TODO (not drafted yet, 0x8 bytes at 0x800D0224): actor/game routine
extern "C" void fn_800D0224();

// TODO (not drafted yet, 0x28 bytes at 0x800D022C): actor/game routine
extern "C" void fn_800D022C();

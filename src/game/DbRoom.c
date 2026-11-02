// DbRoom.c: the room database ("Loaded Rooms" debug page, DbRoom.c asserts) 0x80019A04-0x80022B10.
// Rooms (0x68 bytes each) hold a name, a bounding box, a list of portals and a link chain; the file finds
// rooms by name hash or position, tracks which rooms are loaded and visible, and draws the room debug text.
// Compiled as C++ although the file is called .c: it calls String/Buffer constructors and destructors.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef short s16;

typedef struct Vec3 {
    float x, y, z;
} Vec3;
typedef struct IVec3 {
    int x, y, z;
} IVec3;
typedef struct IBox {
    IVec3 min, max;
} IBox;
typedef struct Mtx34 {
    float rot[9];
    Vec3 pos;
} Mtx34;
typedef struct Box {
    Vec3 min, max;
} Box;
typedef struct Actor Actor;

// A message: id at +4, argument at +0xc.
typedef struct Msg {
    int pad0;
    int id;
    int pad8;
    int arg;
} Msg;

// A portal between rooms (0x70 bytes).

typedef struct Portal {
    u8 pad0[0x1c];
    s16 id;     // 0x1c
    s16 other;  // 0x1e (also read as unsigned flag bits)
    int volCount;  // 0x20
    int* vols;     // 0x24
    u8 pad28[0x58 - 0x28];
    IBox box;      // 0x58
    u8 pad70[0x70 - 0x70];
} Portal;

// A room (0x68 bytes). The name hash is taken from the string at offset 0.
typedef struct Room {
    char name[0x1c];  // 0x00
    u16 state;        // 0x1c
    u8 pad1e[0x20 - 0x1e];
    IBox box;         // 0x20
    float fogNear;    // 0x38
    float fogFar;     // 0x3c
    u8 r, g, b;       // 0x40..0x42
    u8 pad43[0x44 - 0x43];
    int link;         // 0x44
    u8 pad48[0x4c - 0x48];
    u32 flags;        // 0x4c
    u8 pad50[0x5c - 0x50];
    int portalCount;  // 0x5c
    Portal* portals;  // 0x60
    u8 pad64[0x68 - 0x64];
} Room;

// A 0x48-byte table entry that is tested against with a box (e.g. a collision volume).
typedef struct Volume {
    u8 pad0[0x20];
    int box[2];       // 0x20 (a Box source)
    u8 pad28[0x40 - 0x28];
    int value40;      // 0x40
    int result;       // 0x44
} Volume;

// Link chain entry (8 bytes): byte at +4 is the key, bits 8.. of the word at +4 hold the next index.
typedef struct LinkEntry {
    int pad0;
    int key : 8;
    int next : 24;
} LinkEntry;

extern short lbl_804D41F8;
extern int lbl_804D41D0;       // number of rooms
extern Room* lbl_804D41D4;     // room table
extern int lbl_804D41D8;       // number of volumes
extern Volume* lbl_804D41DC;   // volume table
extern LinkEntry* lbl_804D41EC;
extern unsigned char __upper_map[256];
extern IVec3 lbl_80341F8C;     // margin added around each room box when testing a point

extern "C" {
Mtx34* fn_800C64AC(void);                           // camera matrix
void fn_800A0168(Mtx34* out, Mtx34* in);            // invert
void fn_800A0030(Vec3* out, Mtx34* m, Vec3* in);    // transform a vector
void fn_8009FC6C(Mtx34* out, Mtx34* a, Mtx34* b);   // matrix multiply
void fn_8016D864(Box* out, void* bounds);
void fn_8016D920(Vec3* out, Box* box, int corner);  // corner of a box
}
void ActorGetMatrix(Mtx34* out, Actor* actor);
void LinkInfoInit(void* p);
int ModelBoneHitBox(int a, int b, int c, int d);

inline int toupper(int c) { return (c == -1) ? -1 : (int)__upper_map[(unsigned char)c]; }

// Name hash of a room (0 for a null name).
static inline u32 RoomNameHash(const char* s) {
    int i;
    u32 h;

    if (s == 0) {
        return 0;
    }
    h = 0;
    for (i = 0; s[i] != 0; i++) {
        h *= 31;
        h += (unsigned char)toupper(s[i]);
    }
    h %= 0xFFFFFFFF;
    return h;
}

// RoomGetCurrent
extern "C" short fn_80019A04(void) { return lbl_804D41F8; }

// RoomActorLowestY: lowest world Y of the eight corners of an actor's box, seen from the camera.
extern "C" float fn_80019A0C(Actor* actor) {
    Mtx34 cam = *fn_800C64AC();
    Mtx34 m;
    Mtx34 prod;
    Box box;
    Vec3 d;
    Vec3 v;
    float lowest;
    int i;

    ActorGetMatrix(&m, actor);
    fn_800A0168(&m, &m);
    d.x = m.pos.x - cam.pos.x;
    d.y = m.pos.y - cam.pos.y;
    d.z = m.pos.z - cam.pos.z;
    fn_800A0030(&d, &cam, &d);
    fn_8009FC6C(&prod, &cam, &m);
    prod.pos.x = d.x;
    prod.pos.y = d.y;
    prod.pos.z = d.z;
    fn_8016D864(&box, *(void**)((char*)actor + 0x3c));
    lowest = 3.4028235e38f;
    for (i = 0; i != 8; i++) {
        fn_8016D920(&v, &box, i);
        fn_800A0030(&v, &prod, &v);
        v.x += prod.pos.x;
        v.y += prod.pos.y;
        v.z += prod.pos.z;
        if (v.y < lowest) {
            lowest = v.y;
        }
    }
    return lowest;
}

// RoomActorVolume: result of the first volume an actor's box hits (0 if none).
extern "C" int fn_80019BA8(Actor* actor) {
    Mtx34 m;
    Mtx34 id;
    Box box;
    Box box2;
    int i;
    int off;

    ActorGetMatrix(&m, actor);
    LinkInfoInit(&id);
    fn_8016D864(&box, actor);
    for (i = 0, off = 0; i < lbl_804D41D8; i++, off += 0x48) {
        fn_8016D864(&box2, (char*)lbl_804D41DC + off + 0x20);
        if (ModelBoneHitBox((int)&box, (int)&m, (int)&box2, (int)&id)) {
            return lbl_804D41DC[i].result;
        }
    }
    return 0;
}

// Handles the "set room state" messages: rooms whose name hash equals the argument get a new state.
static inline void RoomSetState(u32 hash, int state) {
    int i;
    int off;

    for (i = 0, off = 0; i != lbl_804D41D0; i++, off += sizeof(Room)) {
        Room* r = (Room*)((char*)lbl_804D41D4 + off);
        if (hash == RoomNameHash(r->name)) {
            r->state = state;
        }
    }
}

int RoomMsgState(Msg* m) {
    switch (m->id) {
    case (int)0xCF040EC2:
        RoomSetState(m->arg, 0x1000);
        return 1;
    case 0x117DC8CC:
        RoomSetState(m->arg, 0);
        return 1;
    case 0x1E381056:
        RoomSetState(m->arg, 0x800);
        return 1;
    case 0x6A6B4B34:
        RoomSetState(m->arg, 0x2000);
        return 1;
    }
    return 0;
}

// RoomMsgFlags: messages that set or clear a room flag (0x200 / 0x100).
extern "C" int fn_80019F88(Msg* m, Room* r) {
    switch (m->id) {
    case 0x16A8A76D:
        if (m->arg != 0) {
            r->flags |= 0x100;
        } else {
            r->flags &= ~0x100;
        }
        return 1;
    case 0x5FAE72FE:
        if (m->arg != 0) {
            r->flags |= 0x200;
        } else {
            r->flags &= ~0x200;
        }
        return 1;
    }
    return 0;
}

// The portal id's partner room, searched over all rooms.
int RoomPortalOther(int id) {
    int i;
    int j;

    for (i = 0; i < lbl_804D41D0; i++) {
        Room* r = &lbl_804D41D4[i];
        for (j = 0; j < r->portalCount; j++) {
            if (id == r->portals[j].id) {
                return lbl_804D41D4[i].portals[j].other;
            }
        }
    }
    return 0;
}

// Index of the room with the given name hash (0 if none).
int RoomFindByHash(u32 hash) {
    int i;

    for (i = 0; i < lbl_804D41D0; i++) {
        if (hash == RoomNameHash(lbl_804D41D4[i].name)) {
            return i;
        }
    }
    return 0;
}

// RoomLinkHas: walks a link chain for the byte key.
extern "C" int fn_8001A14C(Room* r, int key) {
    int i = r->link;

    while (i != -1) {
        LinkEntry* e = &lbl_804D41EC[i];
        if (key == e->key) {
            return 1;
        }
        i = e->next;
    }
    return 0;
}

// RoomFromIndex
extern "C" Room* fn_8001A194(int i) {
    if (i == 0) {
        return 0;
    }
    return &lbl_804D41D4[i];
}

static inline void IBoxGrow(IBox* b, IVec3* m) {
    b->min.x -= m->x;
    b->min.y -= m->y;
    b->min.z -= m->z;
    b->max.x += m->x;
    b->max.y += m->y;
    b->max.z += m->z;
}

static inline int IBoxHasXYZ(IBox* b, int x, int y, int z) {
    return ((x - b->min.x | y - b->min.y | z - b->min.z | b->max.x - x | b->max.y - y | b->max.z - z) >> 31) == 0;
}

static inline int IBoxHas(IBox* b, IVec3* p) { return IBoxHasXYZ(b, p->x, p->y, p->z); }

// RoomFindAt: index of the first room (from 1) whose box, grown by a margin, contains the point (0 if none).
extern "C" int fn_8001A1B4(Vec3* pos) {
    int hits[8];
    int* hp = hits;
    int count = 0;
    int i;

    if (pos == 0) {
        hits[0] = 0;
    } else {
        IVec3 p;
        p.x = (int)pos->x;
        p.y = (int)pos->y;
        p.z = (int)pos->z;
        for (i = 1; i < lbl_804D41D0; i++) {
            IBox b = lbl_804D41D4[i].box;
            IBoxGrow(&b, &lbl_80341F8C);
            if (IBoxHas(&b, &p)) {
                count++;
                *hp++ = i;
                if (count >= 8) {
                    break;
                }
            }
        }
        if (count == 0) {
            hits[0] = 0;
        }
    }
    return hits[0];
}

// TODO (not drafted yet, 0x378 bytes at 0x8001A30C): sphere cast against the rooms, collecting sorted hits
// TODO (not drafted yet, 0x434 bytes at 0x8001A684): per-room query of the link chain for a sphere cast
// TODO (not drafted yet, 0x21C bytes at 0x8001AAB8): portal query for a sphere cast
// TODO (not drafted yet, 0x29C bytes at 0x8001ACD4): box cast against the rooms, collecting sorted hits
// TODO (not drafted yet, 0x44C bytes at 0x8001AF70): per-room query of the link chain for a box cast
// TODO (not drafted yet, 0x22C bytes at 0x8001B3BC): portal query for a box cast
// TODO (not drafted yet, 0x448 bytes at 0x8001B5E8): builds the bounds of a box cast

extern int lbl_804D4258;       // nesting counter of room updates
extern int lbl_804D425C;
extern int lbl_804D4260;

extern "C" void fn_8001C4B4(void);

// RoomVolumeSetFlags: set the field at +0x40 of the volume whose result id is given.
extern "C" int fn_8001BA30(int id, int value) {
    Volume* v = lbl_804D41DC;
    int i;

    for (i = 0; i < lbl_804D41D8; i++) {
        if (v->result == id) {
            v->value40 = value;
            return 1;
        }
        v++;
    }
    return 0;
}

// RoomSetLoadMode
extern "C" void fn_8001BA6C(int mode) {
    lbl_804D4260 = mode;
    if (lbl_804D4258 != 0) {
        lbl_804D425C = 0;
    } else {
        fn_8001C4B4();
    }
}

// RoomLock: nesting counter; unlocking clamps at zero.
extern "C" void fn_8001BAA8(int unlock) {
    if (unlock != 0) {
        u32 n = lbl_804D4258 - 1;
        lbl_804D4258 = n & (int)(-n & ~n) >> 31;
        return;
    }
    lbl_804D4258++;
}

// RoomRefresh
extern "C" void fn_8001BAE0(void) { fn_8001C4B4(); }

// RoomRequestRefresh
extern "C" void fn_8001BC78(void) { lbl_804D425C = 1; }

// Per-frame room visibility state: the room the camera is in and its fog/ambient settings.
extern Room* lbl_804D4230;     // current room
extern int lbl_804D4234;
extern int lbl_804D4238;
extern int lbl_804D423C;
extern int lbl_804D4240;
extern int lbl_804D4244;
extern u32 lbl_803DCF7C[0x400 / 32];   // bitset of visible rooms

extern "C" void fn_8001C23C(u32* bits, int n, Vec3* pos);   // marks the room around a position
extern "C" {
void fn_8001C6F4(void* dst, void* src, int n);
int fn_8001D89C(void);
void fn_80012F78(void);
void fn_8003CF98(void);
void fn_8002FB90(void);
void fn_80093180(void);
void* fn_800C6448(void);
}
void BitClear(u32* bits, int n);
void BitsetClear(void* bits, int n);
int BitsetToIndices(int* out, int outMax, u32* bits, int n);
void TextureFlushReleases(void);

// RoomsFindAtInt: indices of the rooms (from 1) whose grown box contains an integer point, at most max.
int RoomsFindAtInt(IVec3* p, int* out, int max) {
    int count = 0;
    int i;
    int* wp = out;
    IVec3 m = lbl_80341F8C;

    for (i = 1; i < lbl_804D41D0; i++) {
        IBox b = lbl_804D41D4[i].box;
        IBoxGrow(&b, &m);
        if (IBoxHas(&b, p)) {
            count++;
            *wp++ = i;
            if (max <= count) {
                break;
            }
        }
    }
    if (count == 0) {
        count = 1;
        *out = 0;
    }
    return count;
}

// RoomsSelectCurrent: picks the room under the camera from the visible set and caches its settings.
void RoomsSelectCurrent(void) {
    u32 bits[0x400 / 32];
    int idx[0x400];

    fn_8001C23C(bits, 0x400, (Vec3*)((char*)fn_800C6448() + 0x24));
    if (BitsetToIndices(idx, 0x400, bits, 0x400) != 0) {
        lbl_804D4230 = &lbl_804D41D4[idx[0]];
        lbl_804D4234 = (int)(lbl_804D4230->fogNear / 8.0f);
        lbl_804D4238 = (int)(lbl_804D4230->fogFar / 8.0f);
        lbl_804D423C = lbl_804D4230->r;
        lbl_804D4240 = lbl_804D4230->g;
        lbl_804D4244 = lbl_804D4230->b;
    } else {
        lbl_804D4230 = lbl_804D41D4;
    }
}

// RoomsFrame: advances the frame counter and refreshes the rooms once a camera exists.
void RoomsFrame(void) {
    lbl_804D41F8++;
    if (lbl_804D41D4 != 0 && fn_800C6448() != 0) {
        fn_8001D89C();
    }
}

// RoomsLoadedReset: clears the loaded-room bitset and lets the dependent subsystems free what they held.
void RoomsLoadedReset(void) {
    u32 bits[0x400 / 32];

    BitClear(bits, 0x400);
    fn_8001C6F4(lbl_803DCF7C, bits, 0x400);
    fn_80012F78();
    fn_8003CF98();
    TextureFlushReleases();
    fn_8002FB90();
    fn_80093180();
    BitsetClear(lbl_803DCF7C, 0x400);
}

static inline void BitSetAt(u32* bits, int i) { bits[i / 32] |= 1 << (i & 31); }

// RoomsMarkAt: sets the bit of the room(s) that contain a world position. A portal box wins over a room
// box; if no portal contains the point, the first room box that does is used.
extern "C" void fn_8001C23C(u32* bits, int n, Vec3* pos) {
    int px = (int)pos->x;
    int py = (int)pos->y;
    int pz = (int)pos->z;
    int found = 0;
    int r;
    int j;

    BitsetClear(bits, n);
    for (r = 1; r < lbl_804D41D0; r++) {
        Room* room = &lbl_804D41D4[r];
        for (j = 0; j < room->portalCount; j++) {
            IBox* b = &room->portals[j].box;
            if (IBoxHasXYZ(b, px, py, pz)) {
                found = 1;
                BitSetAt(bits, r);
                break;
            }
        }
    }
    if (!found) {
        for (r = 1; r < lbl_804D41D0; r++) {
            IBox* b = &lbl_804D41D4[r].box;
            if (IBoxHasXYZ(b, px, py, pz)) {
                found = 1;
                BitSetAt(bits, r);
                break;
            }
        }
    }
    if (!found) {
        for (r = 1; r < lbl_804D41D0; r++) {
            IBox* b = &lbl_804D41D4[r].box;
            if (IBoxHasXYZ(b, px, py, pz)) {
                BitSetAt(bits, r);
                return;
            }
        }
    }
}

// Functions not drafted yet (symbol names are still the placeholders):
// TODO (not drafted yet, 0x178 bytes at 0x8001BB00): fn_8001BB00
// TODO (not drafted yet, 0x1B0 bytes at 0x8001BC84): fn_8001BC84
// TODO (not drafted yet, 0x408 bytes at 0x8001BE34): fn_8001BE34
// TODO (not drafted yet, 0x240 bytes at 0x8001C4B4): fn_8001C4B4
// TODO (not drafted yet, 0x1DC bytes at 0x8001C6F4): fn_8001C6F4
// TODO (not drafted yet, 0x1DC bytes at 0x8001C8D0): fn_8001C8D0
// TODO (not drafted yet, 0x2C0 bytes at 0x8001CAAC): fn_8001CAAC
// TODO (not drafted yet, 0x218 bytes at 0x8001CE28): fn_8001CE28
// TODO (not drafted yet, 0x218 bytes at 0x8001D040): fn_8001D040
// TODO (not drafted yet, 0x384 bytes at 0x8001D258): fn_8001D258
// TODO (not drafted yet, 0x27C bytes at 0x8001D5DC): fn_8001D5DC
// TODO (not drafted yet, 0xAD4 bytes at 0x8001D89C): fn_8001D89C
// TODO (not drafted yet, 0x664 bytes at 0x8001E370): fn_8001E370
// TODO (not drafted yet, 0x2A0 bytes at 0x8001EAF8): fn_8001EAF8
// TODO (not drafted yet, 0x344 bytes at 0x8001ED98): fn_8001ED98
// TODO (not drafted yet, 0x28C bytes at 0x8001F0DC): fn_8001F0DC
// TODO (not drafted yet, 0xF4 bytes at 0x8001F368): fn_8001F368
// TODO (not drafted yet, 0x2BC bytes at 0x8001F45C): fn_8001F45C
// TODO (not drafted yet, 0x210 bytes at 0x8001F718): fn_8001F718
// TODO (not drafted yet, 0x8BC bytes at 0x8001F928): fn_8001F928
// TODO (not drafted yet, 0x9C bytes at 0x800201E4): fn_800201E4
// TODO (not drafted yet, 0x134 bytes at 0x80020280): fn_80020280
// TODO (not drafted yet, 0x280 bytes at 0x80020414): fn_80020414
// TODO (not drafted yet, 0x1FC bytes at 0x80020694): fn_80020694
// TODO (not drafted yet, 0x6C8 bytes at 0x80020890): fn_80020890
// TODO (not drafted yet, 0x32C bytes at 0x80020F58): fn_80020F58
// TODO (not drafted yet, 0x708 bytes at 0x80021284): fn_80021284
// TODO (not drafted yet, 0xCC bytes at 0x8002198C): fn_8002198C
// TODO (not drafted yet, 0x30C bytes at 0x80021A58): fn_80021A58
// TODO (not drafted yet, 0x7F0 bytes at 0x80021D64): fn_80021D64
// TODO (not drafted yet, 0x74 bytes at 0x80022554): fn_80022554
// TODO (not drafted yet, 0x70 bytes at 0x800225C8): fn_800225C8
// TODO (not drafted yet, 0x60 bytes at 0x80022638): fn_80022638
// TODO (not drafted yet, 0x25C bytes at 0x80022698): fn_80022698
// TODO (not drafted yet, 0x21C bytes at 0x800228F4): fn_800228F4

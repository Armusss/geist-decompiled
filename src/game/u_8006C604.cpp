// u_8006C604.cpp: player actor code (0x8006C604-0x800776FC): gun motion, ghost effects, messages.
// Names are descriptive guesses; the code is matched against the retail DOL.

// Returns 0 when the object stored in *slot is `other` or is of kind 0x13,
// 1 otherwise (including when the slot is empty). Name is a guess.
int TargetSlotUsable(int** slot, int* other)
{
    int* p = *slot;

    if (p == other || p == 0 || (int)((unsigned)p[14] >> 24) == 0x13)
    {
        if (p != 0)
            return 0;
    }
    return 1;
}

typedef unsigned int u32;
typedef unsigned char u8;

class String
{
public:
    String();
    ~String();
};

extern "C" float lbl_804D6598;
extern "C" float lbl_804D659C;
extern "C" float lbl_804D65A0;
extern "C" double lbl_804D65A8;

extern "C" void fn_80064EE4(void*);
int PlayerGunVecNonZero(void*);
extern "C" int fn_8020B97C(void*);
extern "C" int fn_80049940(void);
extern "C" int fn_800498D0(void*);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" float sqrtf(float);

// Per-frame update of the stun/freeze timers of a player-like object
// (name is a guess; offsets are transcribed from the retail code).
void PlayerUpdateStun(float dt, void* owner, void* arg)
{
    u8* p = (u8*)owner;

    fn_80064EE4(p + 0x7C);
    fn_80064EE4(p + 0xD4);

    u32 flags = *(u32*)(p + 0x7B4);
    if ((flags & 0x80000) == 0 || (flags & 0x2000000) != 0 || PlayerGunVecNonZero(p) == 0)
    {
        *(float*)(p + 0x804) = lbl_804D65A0;
    }
    else
    {
        *(float*)(p + 0x804) -= dt;
        if (*(float*)(p + 0x804) <= lbl_804D6598)
        {
            String s;
            int data[2];
            data[1] = 0x7FFCFD4F;
            *(int*)&s = 0xE6669026;
            SendMsg(fn_800498D0(p), 0x41D, data);
            *(float*)(p + 0x804) = lbl_804D659C;
        }
    }

    if (*(u32*)(p + 0x7B4) & 1)
    {
        *(u32*)(p + 0x7B4) &= ~0x20u;
        *(u32*)(p + 0x7B4) &= ~0x40u;
        *(u32*)(p + 0x7B4) &= ~0x100u;
        *(u32*)(p + 0x7B4) &= ~0x200u;
    }

    *(float*)(p + 0x7D8) -= dt;
    if (*(float*)(p + 0x7D8) < lbl_804D6598)
        *(float*)(p + 0x7D8) = lbl_804D6598;

    if (fn_8020B97C(p) != 0 || *(int*)(p + 0x960) == 1 || fn_80049940() != 0)
        *(float*)(p + 0x7D8) = lbl_804D659C;

    (*(void (**)(float, void*, void*))(p + 0x810))(dt, p, arg);

    if (*(u32*)(p + 0x7B4) & 0x80000)
        *(float*)(p + 0x64C) = (float)(-*(int*)(p + 0xA14));
}

extern "C" float lbl_804D65B0;
extern "C" float lbl_804D65B4;

extern "C" void fn_8008F230(float value, int slots, int index, int flag);
extern "C" void fn_80090390(int slots, int index);
extern "C" int fn_800904F0(int slots, int index);
extern "C" void fn_80090778(float value, int slots, int index, int a, int b, int c, void* owner);
extern "C" void fn_80090974(float value, int slots, int index, int a, int b, int c, void* owner);

// Refreshes the two groups of three player slots from their pending values
// (name is a guess; the offsets are transcribed from the retail code).
void PlayerUpdateHeldSlots(void* owner)
{
    u8* p = (u8*)owner;
    int i;

    for (i = 0; i < 3; i++)
    {
        u8* q = p + i * 4;

        if (*(int*)(q + 0x80) == *(int*)(q + 0x98))
            fn_8008F230(lbl_804D6598, *(int*)(p + 0x7C), i, 1);
        else
            fn_8008F230(lbl_804D65B4, *(int*)(p + 0x7C), i, 1);

        if (*(int*)(q + 0x98) == -1)
        {
            fn_80090390(*(int*)(p + 0x7C), i);
            if (i != 0)
                *(int*)(p + 0x7AC) = 0;
        }
        else if (*(int*)(q + 0x98) != *(int*)(q + 0x80) ||
                 fn_800904F0(*(int*)(p + 0x7C), i) != *(int*)(q + 0x8C))
        {
            if (*(int*)(q + 0x8C) == 0)
            {
                fn_80090778((float)*(u32*)(q + 0xB0) / lbl_804D65B0,
                            *(int*)(p + 0x7C), i, *(int*)(q + 0x98),
                            *(int*)(q + 0xA4), *(int*)(p + 0x38), p);
            }
            else
            {
                fn_80090974((float)*(u32*)(q + 0xB0) / lbl_804D65B0,
                            *(int*)(p + 0x7C), i, *(int*)(q + 0x98),
                            *(int*)(q + 0xA4), *(int*)(q + 0x8C), p);
            }
        }
        *(int*)(q + 0x80) = *(int*)(q + 0x98);

        if (*(int*)(q + 0xD8) == *(int*)(q + 0xF0))
            fn_8008F230(lbl_804D6598, *(int*)(p + 0xD4), i, 1);
        else
            fn_8008F230(lbl_804D65B4, *(int*)(p + 0xD4), i, 1);

        if (*(int*)(q + 0xF0) == -1)
        {
            fn_80090390(*(int*)(p + 0xD4), i);
        }
        else if (*(int*)(q + 0xF0) != *(int*)(q + 0xD8) ||
                 fn_800904F0(*(int*)(p + 0xD4), i) != *(int*)(q + 0xE4))
        {
            if (*(int*)(q + 0xE4) == 0)
            {
                fn_80090778((float)*(u32*)(q + 0x108) / lbl_804D65B0,
                            *(int*)(p + 0xD4), i, *(int*)(q + 0xF0),
                            *(int*)(q + 0xFC), *(int*)(p + 0x38), p);
            }
            else
            {
                fn_80090974((float)*(u32*)(q + 0x108) / lbl_804D65B0,
                            *(int*)(p + 0xD4), i, *(int*)(q + 0xF0),
                            *(int*)(q + 0xFC), *(int*)(q + 0xE4), p);
            }
        }
        *(int*)(q + 0xD8) = *(int*)(q + 0xF0);
    }
}

// Returns the player's held-slot word at 0x7D0 (name is a guess).
int PlayerGetHeldSlotIndex(void* owner)
{
    return *(int*)((u8*)owner + 0x7D0);
}

// Sets the player's held-slot word at 0x7D0 (name is a guess).
void PlayerSetHeldSlotIndex(void* owner, int index)
{
    *(int*)((u8*)owner + 0x7D0) = index;
}

extern "C" int fn_802A1D04(void);
// Retail calls this as (int handle, float* out) (see 0x8006E66C); the older
// call sites in this draft still pass a leading float and need revisiting.
extern "C" void fn_800913EC(int handle, float scale, void* out);
extern "C" float lbl_804D65B8;

// C++-mangled helper of another unit (Fiif).
void Fn8008E33C(int slot, int hand, float value);

// Clears the player's held slots and resets the action state to 0x15 (name is a guess).
void PlayerResetHeldSlots(void* owner)
{
    u8* p = (u8*)owner;
    int i = 0;
    u8* q = p;

    fn_802A1D04();

    do
    {
        fn_80090390(*(int*)(p + 0x7C), i);
        fn_80090390(*(int*)(p + 0xD4), i);
        *(int*)(q + 0x80) = -1;
        i++;
        *(int*)(q + 0xD8) = -1;
        q += 4;
    } while (i < 3);

    if (*(void (**)(void*))(p + 0x814) != 0)
        (*(void (**)(void*))(p + 0x814))(p);

    *(int*)(p + 0x7A4) = 0x15;
    *(u32*)(p + 0x7B4) |= 1;

    if (*(void (**)(void*))(p + 0x80C) != 0)
        (*(void (**)(void*))(p + 0x80C))(p);

    PlayerUpdateHeldSlots(p);

    fn_800913EC(*(int*)(p + 0x7C), lbl_804D65B8, 0);
    fn_800913EC(*(int*)(p + 0xD4), lbl_804D65B8, 0);

    *(int*)(p + 0x7A8) = 0x15;
}

// Runs the player's reset callback pair, sets the action state to 0x14 and
// refreshes the held slots (name is a guess).
void PlayerEnterIdle(void* owner)
{
    u8* p = (u8*)owner;

    if (*(void (**)(void*))(p + 0x814) != 0)
        (*(void (**)(void*))(p + 0x814))(p);

    *(int*)(p + 0x7A4) = 0x14;
    *(u32*)(p + 0x7B4) |= 1;

    if (*(void (**)(void*))(p + 0x80C) != 0)
        (*(void (**)(void*))(p + 0x80C))(p);

    PlayerUpdateHeldSlots(p);

    fn_800913EC(*(int*)(p + 0x7C), lbl_804D65B8, 0);
    fn_800913EC(*(int*)(p + 0xD4), lbl_804D65B8, 0);
}

// Picks the player's held-slot state from the slot word at 0x7D0
// (name is a guess).
void PlayerSelectHeldState(void* owner)
{
    u8* p = (u8*)owner;

    if (*(int*)(p + 0x7D0) != 0)
        *(int*)(p + 0x7A8) = 3;
    else
        *(int*)(p + 0x7A8) = 1;
}

// Pushes the held-slot state selected by the word at 0x7D0 into the action
// state, unless the object is already in state 1 (name is a guess).
void PlayerApplyHeldSlotState(void* owner)
{
    u8* p = (u8*)owner;

    if (*(int*)(p + 0x7D0) != 0)
    {
        if (*(void (**)(void*))(p + 0x814) != 0)
            (*(void (**)(void*))(p + 0x814))(p);

        *(int*)(p + 0x7A4) = 3;
        *(u32*)(p + 0x7B4) |= 1;

        if (*(void (**)(void*))(p + 0x80C) != 0)
            (*(void (**)(void*))(p + 0x80C))(p);

        PlayerUpdateHeldSlots(p);

        fn_800913EC(*(int*)(p + 0x7C), lbl_804D65B8, 0);
        fn_800913EC(*(int*)(p + 0xD4), lbl_804D65B8, 0);
    }
    else
    {
        if (*(int*)(p + 0x960) == 1)
            return;

        if (*(void (**)(void*))(p + 0x814) != 0)
            (*(void (**)(void*))(p + 0x814))(p);

        *(int*)(p + 0x7A4) = 1;
        *(u32*)(p + 0x7B4) |= 1;

        if (*(void (**)(void*))(p + 0x80C) != 0)
            (*(void (**)(void*))(p + 0x80C))(p);

        PlayerUpdateHeldSlots(p);

        fn_800913EC(*(int*)(p + 0x7C), lbl_804D65B8, 0);
        fn_800913EC(*(int*)(p + 0xD4), lbl_804D65B8, 0);
    }
}

class Player;

// Forwards a float value to both of the player's held-weapon slots when the
// player is in action state 0x16 (name is a guess).
void Fn8006CD84(Player* player, int hand, float value)
{
    u8* p = (u8*)player;

    if (*(int*)(p + 0x7A4) == 0x16)
    {
        Fn8008E33C(*(int*)(p + 0x7C), hand, value);
        Fn8008E33C(*(int*)(p + 0xD4), hand, value);
    }
}

extern "C" void fn_8008FFFC(int slot, int value);
void Fn800901AC(int slot, int value);

// Clears (flag == 0) or sets (flag != 0) the two held-weapon slots of a player
// that is in action state 0x16 (name is a guess; offsets are from retail).
void Fn8006CDE8(Player* player, int value, int flag)
{
    u8* p = (u8*)player;

    if (*(int*)(p + 0x7A4) == 0x16)
    {
        if (flag != 0)
        {
            fn_8008FFFC(*(int*)(p + 0x7C), value);
            fn_8008FFFC(*(int*)(p + 0xD4), value);
        }
        else
        {
            Fn800901AC(*(int*)(p + 0x7C), value);
            Fn800901AC(*(int*)(p + 0xD4), value);
        }
    }
}

// Stores one pending held-slot entry into both slot groups and enters action
// state 0x16 (name is a guess; offsets are from retail).
void Fn8006CE5C(Player* player, int value, int index, int flags, int param5,
                int param6, int param7)
{
    u8* p = (u8*)player;
    u8* q = p + index * 4;

    *(int*)(q + 0x98) = value;
    *(int*)(q + 0xA4) = flags;
    *(int*)(q + 0xB0) = param5;
    *(int*)(p + 0xBC) = param6;
    *(int*)(p + 0xC0) = param7;
    *(int*)(q + 0xF0) = value;
    *(int*)(q + 0xFC) = flags;
    *(int*)(q + 0x108) = param5;
    *(int*)(p + 0x114) = param6;
    *(int*)(p + 0x118) = 0;

    if ((flags & 0x10) == 0)
    {
        *(int*)(p + 0xC4) = value;
        *(int*)(p + 0x11C) = value;
    }
    else
    {
        *(int*)(p + 0xC4) = -1;
        *(int*)(p + 0x11C) = -1;
    }

    if (*(void (**)(void*))(p + 0x814) != 0)
        (*(void (**)(void*))(p + 0x814))(p);

    *(int*)(p + 0x7A4) = 0x16;
    *(u32*)(p + 0x7B4) |= 1;

    if (*(void (**)(void*))(p + 0x80C) != 0)
        (*(void (**)(void*))(p + 0x80C))(p);

    PlayerUpdateHeldSlots(p);

    fn_800913EC(*(int*)(p + 0x7C), lbl_804D65B8, 0);
    fn_800913EC(*(int*)(p + 0xD4), lbl_804D65B8, 0);
}

// Stores one pending held-slot entry into both slot groups and refreshes the
// three stream slots (name is a guess; offsets are from retail).
void fn_8006CF44(void* owner, int value, int index, u32 flags, int param5,
                 int param6, int param7)
{
    u8* p = (u8*)owner;
    u8* q = p + index * 4;
    int i;

    *(int*)(q + 0x148) = value;
    *(u32*)(q + 0x154) = flags;
    *(int*)(q + 0x160) = param5;
    *(int*)(p + 0x16C) = param6;
    *(int*)(p + 0x170) = param7;

    if ((flags & 0x10) == 0)
        *(int*)(p + 0x174) = value;
    else
        *(int*)(p + 0x174) = -1;

    for (i = 0; i < 3; i++)
    {
        u8* e = p + i * 4;

        fn_8008F230(lbl_804D6598, *(int*)(p + 0x12C), i, 1);

        if (*(int*)(e + 0x148) == -1)
        {
            fn_80090390(*(int*)(p + 0x12C), i);
        }
        else
        {
            fn_80090778((float)*(u32*)(e + 0x160) / lbl_804D65B0,
                        *(int*)(p + 0x12C), i, *(int*)(e + 0x148),
                        *(int*)(e + 0x154), *(int*)(p + 0x38), p);
        }
    }
}

// Enters the player's held-slot action state 0x0D and refreshes the slots
// (name is a guess; offsets are from retail).
void fn_8006D038(void* owner)
{
    u8* p = (u8*)owner;

    *(u32*)(p + 0x50) |= 0x1000002;
    *(u32*)(p + 0x7B4) |= 0x10000;

    if (*(void (**)(void*))(p + 0x814) != 0)
        (*(void (**)(void*))(p + 0x814))(p);

    *(int*)(p + 0x7A4) = 0xD;
    *(u32*)(p + 0x7B4) |= 1;

    if (*(void (**)(void*))(p + 0x80C) != 0)
        (*(void (**)(void*))(p + 0x80C))(p);

    PlayerUpdateHeldSlots(p);

    fn_800913EC(*(int*)(p + 0x7C), lbl_804D65B8, 0);
    fn_800913EC(*(int*)(p + 0xD4), lbl_804D65B8, 0);
}

// Leaves the player's held-slot action state 0x0C and refreshes the slots
// (name is a guess; offsets are from retail).
void PlayerExitHeldSlotState0C(void* owner)
{
    u8* p = (u8*)owner;

    if (*(void (**)(void))(p + 0x814) != 0)
        (*(void (**)(void))(p + 0x814))();

    *(int*)(p + 0x7A4) = 0xC;
    *(u32*)(p + 0x7B4) |= 1;

    if (*(void (**)(void*))(p + 0x80C) != 0)
        (*(void (**)(void*))(p + 0x80C))(p);

    PlayerUpdateHeldSlots(p);

    fn_800913EC(*(int*)(p + 0x7C), lbl_804D65B8, 0);
    fn_800913EC(*(int*)(p + 0xD4), lbl_804D65B8, 0);
}

// Stores a value at offset 0x28 of the object (a setter; name is a guess).
void PlayerSetValue28(void* owner, int value)
{
    *(int*)((u8*)owner + 0x28) = value;
}

// Stores an int at offset 4 of the object (a setter; name is a guess).
void PlayerSetValue4(void* owner, int value)
{
    *(int*)((u8*)owner + 4) = value;
}

// Clears the player's held slots and re-enters the given action mode when the
// game state allows it (name is a guess; offsets are transcribed from retail).
void PlayerSetHeldSlotMode(void* owner, int mode)
{
    u8* p = (u8*)owner;
    int i = 0;
    u8* q = p;

    if (fn_802A1D04() != 0 && (mode == 1 || mode == 0))
        return;

    do
    {
        fn_80090390(*(int*)(p + 0x7C), i);
        fn_80090390(*(int*)(p + 0xD4), i);
        *(int*)(q + 0x80) = -1;
        i++;
        *(int*)(q + 0xD8) = -1;
        q += 4;
    } while (i < 3);

    if (*(int*)(p + 0x960) == 1 && (u32)mode <= 1)
        return;

    if (*(void (**)(void*))(p + 0x814) != 0)
        (*(void (**)(void*))(p + 0x814))(p);

    *(int*)(p + 0x7A4) = mode;
    *(u32*)(p + 0x7B4) |= 1;

    if (*(void (**)(void*))(p + 0x80C) != 0)
        (*(void (**)(void*))(p + 0x80C))(p);

    PlayerUpdateHeldSlots(p);

    fn_800913EC(*(int*)(p + 0x7C), lbl_804D65B8, 0);
    fn_800913EC(*(int*)(p + 0xD4), lbl_804D65B8, 0);
}

// Switches the player to the given held-item action mode and refreshes the
// held slots (name is a guess; offsets are transcribed from retail).
void PlayerSetActionMode(void* owner, int mode)
{
    u8* p = (u8*)owner;

    if (*(int*)(p + 0x960) == 1 && (mode == 1 || mode == 0))
        return;

    if (*(void (**)(void*))(p + 0x814) != 0)
        (*(void (**)(void*))(p + 0x814))(p);

    *(int*)(p + 0x7A4) = mode;
    *(u32*)(p + 0x7B4) |= 1;

    if (*(void (**)(void*))(p + 0x80C) != 0)
        (*(void (**)(void*))(p + 0x80C))(p);

    PlayerUpdateHeldSlots(p);

    fn_800913EC(*(int*)(p + 0x7C), lbl_804D65B8, 0);
    fn_800913EC(*(int*)(p + 0xD4), lbl_804D65B8, 0);
}

// Callbacks installed on the player when it holds a given item (offsets are
// transcribed from retail; field names are guesses).
struct PlayerSlotCallbacks
{
    void (*unk80C)();
    void (*unk810)();
    void (*unk814)();
    void (*unk818)();
    int unk81C;
    int unk820;
    int unk824;
    int unk828;
};

// One row of the held-item callback tables in .data (name and id pointers,
// then the callbacks copied into the player). Field names are guesses.
struct PlayerSlotDef
{
    const char* name;
    const void* id;
    PlayerSlotCallbacks callbacks;
};

extern "C" PlayerSlotDef lbl_803948C0[];
extern "C" PlayerSlotDef lbl_80394190[];

// Installs the callback set of the held item `id` for the given slot kind
// (1 = no item, otherwise a weapon). Returns 1 when a row matched, 0 if not.
int PlayerSetSlotCallbacks(void* owner, int kind, const void* id)
{
    PlayerSlotCallbacks* callbacks = (PlayerSlotCallbacks*)((u8*)owner + 0x80C);

    if (kind == 1)
    {
        for (int i = 0; i < 9; i++)
        {
            if (id == lbl_803948C0[i].id)
            {
                *callbacks = lbl_803948C0[i].callbacks;
                return 1;
            }
        }
    }
    else
    {
        for (int i = 0; i < 0x2E; i++)
        {
            if (id == lbl_80394190[i].id)
            {
                *callbacks = lbl_80394190[i].callbacks;
                return 1;
            }
        }
    }

    return 0;
}

extern "C" void* memset(void*, int, unsigned long);
extern "C" u8 __upper_map[256];

extern "C" void fn_8012B9E8(void);
extern "C" void fn_80136DC8(void);
extern "C" void fn_8013A0CC(void);
extern "C" void fn_8018B100(void);
extern "C" void fn_801963E0(void);
extern "C" void fn_801B8080(void);
extern "C" void fn_801C9F50(void);
extern "C" void fn_801D09FC(void);
extern "C" void fn_801EA660(void);
extern "C" void fn_801F157C(void);
extern "C" void fn_80209A14(void);
extern "C" void fn_80210B64(void);
extern "C" void fn_80227B9C(void);
extern "C" void fn_8023704C(void);

void GunLoadAssets();
void GrenadeLoadSounds();
void InitGhostActionAssets();
void GunHostLoadSounds();
void HostLoadSounds();

// Lazily computed name hash used to fill the `id` fields of the held-item
// tables (name is a guess; the shape is transcribed from retail).
static inline u32 PlayerNameHash(const char* name)
{
    u32 hash = 0;
    int count = 0;
    const u8* p = (const u8*)name;

    if (p == 0)
        return 0;

    while (*p != 0)
    {
        u32 c;
        if ((signed char)*p == -1)
            c = 0xFFFFFFFF;
        else
            c = __upper_map[*p];
        hash *= 31;
        hash += c & 0xFF;
        p++;
        count++;
    }

    return hash % 0xFFFFFFFF;
}

// State block at player+0x7A4 reset by PlayerInitGunState (offsets and field
// names are guesses; sizes come from the retail memset).
struct PlayerGunState
{
    int field_00;
    int field_04;
    int field_08;
    int field_0C;
    u32 flags;
    int field_14;
    u8 pad_18[0xC];
    float f24;
    int field_28;
    int field_2C;
    int field_30;
    float f34;
    u8 pad_38[4];
    int field_3C;
    float f40;
    u8 pad_44[0x8];
    int field_4C;
    int field_50;
    int field_54;
    float f58;
    float f5C;
    float f60;
    u8 pad_64[0x24];
};

// Initialises the player's gun/held-item state, hashes the held-item tables
// and loads the assets of every weapon type (name is a guess; offsets and
// calls are transcribed from retail).
void PlayerInitGunState(void* owner)
{
    u8* p = (u8*)owner;
    PlayerGunState* state = (PlayerGunState*)(p + 0x7A4);
    PlayerSlotDef* def;
    int i;

    memset(state, 0, 0x88);

    state->flags = 0;
    state->field_00 = 3;
    state->field_04 = 3;
    state->field_08 = 0;
    state->field_0C = 0;
    state->field_14 = -1;
    state->f24 = lbl_804D6598;
    state->field_28 = 0;
    state->field_2C = 1;
    state->field_30 = 0;
    state->f34 = lbl_804D659C;
    state->field_3C = 0;
    state->f40 = lbl_804D65B4;
    state->field_50 = -1;
    state->field_54 = -1;
    state->field_4C = 0;
    state->f58 = lbl_804D6598;
    state->f5C = lbl_804D6598;
    state->f60 = lbl_804D65A0;

    def = lbl_80394190;
    for (i = 0; i < 0x17; i++)
    {
        def[0].id = (const void*)PlayerNameHash(def[0].name);
        def[1].id = (const void*)PlayerNameHash(def[1].name);
        def += 2;
    }

    for (i = 0; i < 9; i++)
        lbl_803948C0[i].id = (const void*)PlayerNameHash(lbl_803948C0[i].name);

    if (*(int*)(p + 0x26E4) == 0)
        PlayerSetSlotCallbacks(p, 2, *(const void**)(*(int*)(p + 0x26C4) + 0x174));
    else
        PlayerSetSlotCallbacks(p, *(u32*)(*(int*)(p + 0x26E4) + 0x38) >> 24,
                               *(const void**)(*(int*)(p + 0x26C8) + 0x174));

    state->flags &= ~0x20u;
    state->flags &= ~0x40u;
    state->flags &= ~0x100u;
    state->flags &= ~0x200u;

    GunLoadAssets();
    fn_8012B9E8();
    fn_80136DC8();
    fn_8013A0CC();
    GrenadeLoadSounds();
    fn_8018B100();
    fn_801963E0();
    fn_801B8080();
    fn_801C9F50();
    fn_801D09FC();
    fn_801EA660();
    fn_801F157C();
    fn_80209A14();
    fn_80210B64();
    InitGhostActionAssets();
    fn_80227B9C();
    fn_8023704C();
    GunHostLoadSounds();
    HostLoadSounds();
}

// Resets the player's gun timer (player+0x7D8) to its default value. Name is
// a guess; the offset is transcribed from retail.
void PlayerResetGunTimer(void* owner)
{
    *(float*)((u8*)owner + 0x7D8) = lbl_804D659C;
}

// Calls the player's callback at offset 0x824 if one is installed
// (name is a guess; the offset is transcribed from retail).
void PlayerCallCallback824(void* owner)
{
    if (*(void (**)(void))((u8*)owner + 0x824) != 0)
        (*(void (**)(void))((u8*)owner + 0x824))();
}

// Calls the player's callback at offset 0x820 if one is installed
// (name is a guess; the offset is transcribed from retail).
void PlayerCallCallback820(void* owner)
{
    if (*(void (**)(void))((u8*)owner + 0x820) != 0)
        (*(void (**)(void))((u8*)owner + 0x820))();
}

// Calls the player's callback at offset 0x81C if one is installed
// (name is a guess; the offset is transcribed from retail).
void PlayerCallCallback81c(void* owner)
{
    if (*(void (**)(void))((u8*)owner + 0x81C) != 0)
        (*(void (**)(void))((u8*)owner + 0x81C))();
}

extern "C" float lbl_804D65C0;
extern "C" float lbl_804D65C4;
extern "C" float lbl_804D65C8;
extern "C" float lbl_804D65CC;
extern "C" double lbl_804D65D0;

extern "C" int lbl_804D2960;
extern "C" float lbl_804D296C;
extern "C" float lbl_804D2970;
extern "C" float lbl_804D2978;
extern "C" float lbl_804D297C;
extern "C" float lbl_804D2980;
extern "C" float lbl_804D2984;
extern "C" float lbl_804D2988;
extern "C" float lbl_804D298C;

extern "C" void fn_800F47F4(float value);
int ActorPlayAnim(int actor, unsigned long anim, int mask);
extern "C" int rand(void);

// Handles one gun/held-item message (message ids and offsets are transcribed
// from retail; names are guesses). Returns 0 for an unknown message.
int PlayerHandleGunEvent(void* owner, void* data)
{
    u8* p = (u8*)owner;
    u8* d = (u8*)data;
    int msg = *(int*)(d + 4);

    if (msg == 0x3B99A50D)
    {
        *(float*)(p + 0x7A0) = (float)*(u32*)(d + 0xC);
        return 1;
    }

    if (msg < 0x3B99A50D)
    {
        if (msg == (int)0xE65BB939)
        {
            lbl_804D2960 = *(int*)(d + 0xC) != 0;
            return 1;
        }

        if (msg < (int)0xE65BB939)
        {
            if (msg == (int)0xAD6DA4DD)
            {
                if (*(int*)(d + 0xC) < 1)
                {
                    lbl_804D298C = lbl_804D65C8;
                    lbl_804D2988 = lbl_804D298C;
                }
                else
                {
                    lbl_804D2978 = lbl_804D296C;
                    lbl_804D297C = lbl_804D2970;
                    lbl_804D298C = (float)*(u32*)(d + 0xC);
                    lbl_804D2988 = lbl_804D298C;
                }
                return 1;
            }

            if (msg < (int)0xAD6DA4DD && msg == (int)0xAA25D309)
            {
                if (*(int*)(d + 0xC) != 0)
                {
                    *(float*)(p + 0x798) = lbl_804D65C8;
                    *(float*)(p + 0x79C) =
                        lbl_804D65C4 * ((float)(u32)rand() / lbl_804D65C0);
                    *(float*)(p + 0x7A0) = lbl_804D65C8;
                    if (*(int*)(p + 0x794) == 0)
                    {
                        ActorPlayAnim(*(int*)(p + 0x38), 0x7B3A7CF7,
                                      *(int*)(p + 0x38));
                        *(int*)(p + 0x794) = 1;
                    }
                }
                else if (*(int*)(p + 0x794) != 0)
                {
                    fn_800F47F4(lbl_804D65CC);
                    ActorPlayAnim(*(int*)(p + 0x38), 0xEC152137,
                                  *(int*)(p + 0x38));
                    *(int*)(p + 0x794) = 0;
                }
                return 1;
            }
        }
        else if (msg == 0x3A90169B)
        {
            *(float*)(p + 0x79C) = (float)*(u32*)(d + 0xC);
            return 1;
        }
    }
    else
    {
        if (msg == 0x7386CCFC)
        {
            if (lbl_804D2988 > lbl_804D65CC)
                lbl_804D2984 = (float)*(u32*)(d + 0xC);
            else
                lbl_804D2970 = (float)*(u32*)(d + 0xC);
            return 1;
        }

        if (msg < 0x7386CCFC)
        {
            if (msg == 0x61ACC3D8)
            {
                *(float*)(p + 0x798) = (float)*(u32*)(d + 0xC);
                return 1;
            }
        }
        else if (msg == 0x7386CDEA)
        {
            if (lbl_804D2988 > lbl_804D65CC)
                lbl_804D2980 = (float)*(u32*)(d + 0xC);
            else
                lbl_804D296C = (float)*(u32*)(d + 0xC);
            return 1;
        }
    }

    return 0;
}

// Stores a value into a global float (looks like a scale/time setter; the
// global's meaning is a guess).
extern "C" float lbl_804D2948;

void SetGlobalFloat8006DB74(float value)
{
    lbl_804D2948 = value;
}

// Releases the handle stored at +0x708 and marks the slot empty (-1).
// The owner type and the callee's contract are guesses.
extern "C" void fn_8016EEF4(int handle);

void ReleaseHandle8006DB7C(void* owner)
{
    u8* p = (u8*)owner;
    int handle = *(int*)(p + 0x708);

    if (handle != -1)
    {
        fn_8016EEF4(handle);
        *(int*)(p + 0x708) = -1;
    }
}

// --- helpers and data used by PlayerUpdateGunMotion -----------------------

struct Actor;
struct PlaceF;

struct Mtx34
{
    float m[12];
};

// Animation/model helpers defined in other units (the names are their C++
// manglings; the parameter types and meanings are guesses).
int ModelBoneCount(int model);
char* ModelAnimate(int model, char* anim, int flag);
void AnimApply(int slots, char* anim, int animData, int boneCount);
void PlacementToMatrix(Mtx34* mtx, PlaceF* place);
int PlayerInHost(Actor* actor);
void ActorGetMatrix(Mtx34* out, Actor* actor);
void ModelSetMatrix(int model, Mtx34* mtx, char* anim);
void ActorRelinkOwners(Actor* actor);
void SplineQuatRotateRamp(float value, float* anim, int boneA, int boneB,
                          unsigned long owner);
void SplineQuatRotateRange(float value, float* anim, int boneA, int boneB,
                           unsigned long owner);

extern "C" int fn_80082118(int, int, int, int);
extern "C" int fn_8003A1F0(int, int, int);
extern "C" void fn_8009F63C(void*, void*);
extern "C" void fn_8009F44C(void*, void*, void*);
extern "C" float fn_800E1020(float*);
extern "C" void fn_800E1098(float, float*, float*);
extern "C" void fn_8008F4EC(int, int);
extern "C" int fn_800F94C8(int);
extern "C" int fn_800C64E4(void);
extern "C" float fabsf(float);
void PlayerCollisionMove(void*, float*);
void PlayerGunFxUpdate(float, void*);
extern "C" void fn_80075F94(float, void*, float*);
extern "C" void fn_80075560(float, void*);

extern "C" float lbl_804D65D8;
extern "C" float lbl_804D65DC;
extern "C" float lbl_804D65E0;
extern "C" float lbl_804D65E4;
extern "C" float lbl_804D65E8;
extern "C" float lbl_804D65EC;
extern "C" float lbl_804D65F0;
extern "C" float lbl_804D65F4;

// Per-frame motion update of a player holding an item: advances the streamed
// slot offsets, integrates the drift/random walk of the hand, moves the model
// and picks the animation. Message ids and offsets are transcribed from
// retail; the name and the field meanings are guesses.
void PlayerUpdateGunMotion(float dt, void* owner)
{
    u8* p = (u8*)owner;
    float streamPos[3];
    float move[3];
    float delta[3];
    float pred[3];
    float vx;
    float vy;
    float vz;
    Mtx34 mtx;
    int i;

    streamPos[0] = lbl_804D65CC;
    streamPos[1] = lbl_804D65CC;
    streamPos[2] = lbl_804D65CC;
    move[0] = lbl_804D65CC;
    move[1] = lbl_804D65CC;
    move[2] = lbl_804D65CC;

    *(int*)(p + 0x660) = 0;
    *(int*)(p + 0x664) = 0;
    *(int*)(p + 0x668) = 0;
    *(int*)(p + 0x640) = 0;

    if (p != 0 && (*(u32*)(p + 0x4C) & 1) != 0)
    {
        int handle = fn_800498D0(p);
        int boneCount = ModelBoneCount(*(int*)(p + 0x58));
        char* anim;
        int place;

        fn_80082118(handle, *(int*)(p + 0x58), boneCount, *(int*)(p + 0x140C));
        anim = ModelAnimate(*(int*)(p + 0x58), 0, 0);
        place = fn_8003A1F0(*(int*)(p + 0x58), *(int*)(p + 0x140C), (int)anim);
        PlacementToMatrix((Mtx34*)p, (PlaceF*)place);

        *(float*)(p + 0x67C) = (float)*(int*)(p + 0x24);
        *(float*)(p + 0x680) = (float)*(int*)(p + 0x28);
        *(float*)(p + 0x684) = (float)*(int*)(p + 0x2C);
        return;
    }

    fn_800913EC(*(int*)(p + 0xD4), dt, streamPos);
    *(int*)(p + 0x660) += (int)streamPos[0];
    *(int*)(p + 0x664) += (int)streamPos[1];

    if ((*(u32*)(p + 0x50) & 4) == 0)
    {
        fn_8009F63C(&mtx, p);

        vx = *(float*)(p + 0x650);
        vy = *(float*)(p + 0x654);
        vz = *(float*)(p + 0x658);
        if (vx != lbl_804D65CC || vy != lbl_804D65CC || vz != lbl_804D65CC)
        {
            move[0] = vx * dt;
            move[1] = vy * dt;
            move[2] = vz * dt;
        }

        if (*(u32*)(p + 0x7B4) & 0x20)
            *(int*)(p + 0x660) = 0;
        if (*(u32*)(p + 0x7B4) & 0x40)
            *(int*)(p + 0x664) = 0;

        fn_8009F44C((void*)(p + 0x66C), &mtx, (void*)(p + 0x660));

        if (fabsf(move[0]) > lbl_804D65E0 || fabsf(move[1]) > lbl_804D65E0 ||
            fabsf(move[2]) > lbl_804D65D8)
        {
            *(float*)(p + 0x6CC) = move[0];
            *(float*)(p + 0x6D0) = move[1];
            *(float*)(p + 0x6D4) = move[2];
            PlayerCollisionMove(p, move);

            if (!(fabsf(*(float*)(p + 0x650)) > lbl_804D65E0 ||
                  fabsf(*(float*)(p + 0x654)) > lbl_804D65E0 ||
                  fabsf(*(float*)(p + 0x658)) > lbl_804D65E0))
            {
                move[0] = lbl_804D65CC;
                move[1] = lbl_804D65CC;
                move[2] = lbl_804D65CC;
            }
        }
        else if ((PlayerInHost((Actor*)p) == 0 ||
                  (*(u32*)(*(int*)(p + 0x26E4) + 0x38) >> 24) != 1) &&
                 *(int*)(p + 0x7A4) != 0xC)
        {
            delta[0] = *(float*)(p + 0x67C) - *(float*)(p + 0x6D8);
            delta[1] = *(float*)(p + 0x680) - *(float*)(p + 0x6DC);
            delta[2] = *(float*)(p + 0x684) - *(float*)(p + 0x6E0);

            if (fn_800E1020(delta) < lbl_804D65E4)
            {
                delta[0] = *(float*)(p + 0x6CC);
                delta[1] = *(float*)(p + 0x6D0);
                delta[2] = *(float*)(p + 0x6D4);
            }

            if (fn_800E1020(delta) < lbl_804D65E4)
            {
                float randA = (float)rand() / lbl_804D65C0;
                float randB = (float)rand() / lbl_804D65C0;

                delta[2] = lbl_804D65C8;
                delta[1] = randA / lbl_804D65E8 - lbl_804D65EC;
                delta[0] = randB / lbl_804D65E8 - lbl_804D65EC;
            }

            fn_800E1098(lbl_804D65F0, delta, delta);
            PlayerCollisionMove(p, delta);

            if (fn_800E1020(delta) > lbl_804D65D8)
            {
                move[0] = delta[0];
                move[1] = delta[1];
                move[2] = delta[2];
            }
            else
            {
                move[0] = lbl_804D65CC;
                move[1] = lbl_804D65CC;
                move[2] = lbl_804D65CC;
            }
        }

        *(float*)(p + 0x6D8) = *(float*)(p + 0x67C);
        *(float*)(p + 0x6DC) = *(float*)(p + 0x680);
        *(float*)(p + 0x6E0) = *(float*)(p + 0x684);

        if (dt == lbl_804D65CC)
        {
            *(float*)(p + 0x650) = lbl_804D65CC;
            *(float*)(p + 0x654) = lbl_804D65CC;
            *(float*)(p + 0x658) = lbl_804D65CC;
        }
        else
        {
            float scale = lbl_804D65D8 / dt;

            *(float*)(p + 0x650) = move[0] * scale;
            *(float*)(p + 0x654) = move[1] * scale;
            *(float*)(p + 0x658) = move[2] * scale;
        }

        pred[0] = move[0] + (float)*(int*)(p + 0x66C);
        pred[1] = move[1] + (float)*(int*)(p + 0x670);
        pred[2] = move[2] + (float)*(int*)(p + 0x674);

        fn_80075F94(dt, p, pred);

        *(float*)(p + 0x67C) += move[0] + (float)*(int*)(p + 0x66C);
        *(float*)(p + 0x680) += move[1] + (float)*(int*)(p + 0x670);
        if ((*(u32*)(p + 0x7B4) & 0x100) == 0)
            *(float*)(p + 0x684) += move[2];
        else if (move[2] < lbl_804D65CC)
            *(float*)(p + 0x684) += move[2];

        fn_80075560(dt, p);

        if ((*(u32*)(p + 0x50) & 0x20000) == 0)
        {
            *(int*)(p + 0x24) = (int)*(float*)(p + 0x67C);
            *(int*)(p + 0x28) = (int)*(float*)(p + 0x680);
            *(int*)(p + 0x2C) = (int)*(float*)(p + 0x684);
        }
        else
        {
            *(float*)(p + 0x1474) = *(float*)(p + 0x67C);
            *(float*)(p + 0x1478) = *(float*)(p + 0x680);
            *(float*)(p + 0x147C) = *(float*)(p + 0x684);
            *(int*)(p + 0x1444) = (int)*(float*)(p + 0x67C);
            *(int*)(p + 0x1448) = (int)*(float*)(p + 0x680);
            *(int*)(p + 0x144C) = (int)*(float*)(p + 0x684);
        }

        if (*(u32*)(p + 0x678) & 0x20)
        {
            fn_8008F4EC(*(int*)(p + 0x7C), 0);
            *(u32*)(p + 0x678) &= ~0x20u;
        }
    }

    *(float*)(p + 0x65C) = fn_800E1020((float*)(p + 0x650));

    {
        char* anim = *(char**)(p + 0x140C);
        int animData;
        int boneCount;

        *(float*)(anim + 0x10) = *(float*)(p + 0x67C);
        *(float*)(anim + 0x14) = *(float*)(p + 0x680);
        *(float*)(anim + 0x18) = *(float*)(p + 0x684);

        animData = (int)ModelAnimate(*(int*)(p + 0x58), anim, 1);
        boneCount = ModelBoneCount(*(int*)(p + 0x58));
        AnimApply(*(int*)(p + 0xD4), anim, animData, boneCount);

        if ((*(u32*)(p + 0x7B4) & 0x100) == 0)
            *(int*)(p + 0x63C) = (int)streamPos[2];

        i = *(int*)(p + 0x78);
        if (i == 1)
        {
            u8 msgData[0x18];

            *(short*)(msgData + 4) = 0;
            SendMsg(*(int*)(*(int*)(p + 0x1BA0) + 0x38), 0x402, msgData);
            ActorGetMatrix(&mtx, (Actor*)(msgData + 0x10));
            ModelSetMatrix(*(int*)(p + 0x58), &mtx, anim);
        }
        else if (i == 0)
        {
            int host;
            int value;

            *(float*)(anim + 0x10) = lbl_804D65CC;
            *(float*)(anim + 0x14) = lbl_804D65CC;
            *(float*)(anim + 0x18) = lbl_804D65CC;

            if (anim[*(int*)(p + 0x12C4)] != (char)0xFF &&
                anim[*(int*)(p + 0x12B4)] != (char)0xFF)
            {
                if (fn_802A1D04() == 0)
                {
                    value = fn_800F94C8(*(int*)(p + 0x184));
                    if (fn_800C64E4() == value)
                    {
                        host = PlayerInHost((Actor*)p);
                        SplineQuatRotateRamp(
                            *(float*)(p + 0x1F0), (float*)anim,
                            (u8)anim[*(int*)(p + 0x12C4)],
                            (u8)anim[*(int*)(p + 0x12B4)],
                            *(u32*)(*(int*)(p + 0x26C4 + host * 4)));
                        goto matrix_done;
                    }
                }

                if (fn_802A1D04() != 0)
                {
                    float val = *(float*)(p + 0x1F0) * lbl_804D65F4;
                    int bone = (u8)anim[*(int*)(p + 0x12C4)];

                    host = PlayerInHost((Actor*)p);
                    SplineQuatRotateRange(val, (float*)anim, bone, bone,
                                          *(u32*)(*(int*)(p + 0x26C4 + host * 4)));
                    bone = (u8)anim[*(int*)(p + 0x12B8)];
                    host = PlayerInHost((Actor*)p);
                    SplineQuatRotateRange(val, (float*)anim, bone, bone,
                                          *(u32*)(*(int*)(p + 0x26C4 + host * 4)));
                }
            }

        matrix_done:
            ActorGetMatrix(&mtx, (Actor*)p);
            ModelSetMatrix(*(int*)(p + 0x58), &mtx, anim);
        }
    }

    if (PlayerInHost((Actor*)p) != 0 &&
        *(float*)(*(int*)(p + 0x738) + 8) > lbl_804D65CC)
    {
        if (ActorPlayAnim(*(int*)(*(int*)(p + 0x26E4) + 0x38), 0xFB2744AA,
                          *(int*)(p + 0x38)) == 0)
        {
            u32* dst = (u32*)*(int*)(p + 0x26E4);
            const u32* src = (const u32*)p;

            for (i = 0; i < 0xC; i++)
                dst[i] = src[i];
            ActorRelinkOwners((Actor*)p);
        }
    }

    PlayerGunFxUpdate(dt, p);

    if (*(float*)(p + 0x734) < lbl_804D65D8)
    {
        float timer = *(float*)(p + 0x734) + dt / lbl_804D65DC;

        *(float*)(p + 0x734) = timer;
        *(float*)(p + 0x734) = timer < lbl_804D65D8 ? timer : lbl_804D65D8;
    }
}

extern "C" int fn_8021284C(int);
extern "C" int fn_80212814(int, int, int, int, int, int);
extern "C" int fn_802127A0(int, int, int, int*, int, int, int);

// Handles message 0x415 for the audio object referenced by the message data:
// releases the channel when the id is -1, otherwise starts the sound with or
// without the extra parameter block. Message id and callees are transcribed
// from retail; the name and the data layout are guesses.
int PlayerHandleSoundMsg(int unused, int msg, int* data)
{
    int result;

    if (msg == 0x415)
    {
        int id = data[1];
        int obj = data[0];

        if (id == -1)
            result = fn_8021284C(*(int*)(obj + 0x38));
        else if (data[3] == 0)
            result = fn_80212814(*(int*)(obj + 0x38), id, data[2], 1, 0, 0);
        else
            result = fn_802127A0(*(int*)(obj + 0x38), id, data[2], data + 4,
                                 1, 0, 0);
    }
    else
    {
        result = 0;
    }

    return result;
}

struct Player;

int AssetFind(unsigned long, unsigned long);
void PlayerModelUpdate(Player* player);
extern "C" void fn_800C64FC(int);
extern "C" void fn_800C6D08(float);

// Per-frame model/animation update of a player-like object, including the
// host-tracking matrix copies done for state 1 and the per-state model matrix
// setup. Offsets and message ids are transcribed from retail; the name and the
// field meanings are guesses.
void PlayerModelAnimUpdate(float dt, void* owner)
{
    u8* p = (u8*)owner;
    float stream[3];
    Mtx34 mtx;
    int animData;
    int boneCount;
    int state;

    if ((*(u32*)(p + 0x50) & 0x4000) != 0 || (*(u32*)(p + 0x50) & 0x8000) != 0)
    {
        float f0;
        float f1;
        float f2;
        float f3;
        float* anim = (float*)*(int*)(p + 0x1400);

        fn_800913EC(*(int*)(p + 0x7C), dt, stream);
        animData = (int)ModelAnimate(*(int*)(p + 0x54), (char*)anim, 1);
        f0 = anim[0];
        f1 = anim[1];
        f2 = anim[2];
        f3 = anim[3];
        boneCount = ModelBoneCount(*(int*)(p + 0x54));
        AnimApply(*(int*)(p + 0x7C), (char*)anim, animData, boneCount);
        anim[0] = f0;
        anim[1] = f1;
        anim[2] = f2;
        anim[3] = f3;
        goto after_anim;
    }

    if (*(u32*)(p + 0x4C) & 1)
    {
        u8 msg[0x10];

        boneCount = ModelBoneCount(*(int*)(p + 0x54));
        fn_80082118(*(int*)(p + 0x38), *(int*)(p + 0x54), boneCount,
                    *(int*)(p + 0x1400));
        if (PlayerInHost((Actor*)p) == 0)
        {
            *(int*)(msg + 4) = AssetFind(0xD9044CBB, 0x50EC64DA);
            *(int*)(msg + 8) = 1;
            *(int*)(msg + 12) = 1;
            SendMsg(*(int*)(p + 0x38), 0xC02, msg);
        }
        goto finished;
    }

    fn_800913EC(*(int*)(p + 0x7C), dt, stream);
    animData = (int)ModelAnimate(*(int*)(p + 0x54), *(char**)(p + 0x1400), 1);
    if (*(u32*)(p + 0x50) & 0x200)
    {
        float g0;
        float g1;
        float g2;
        float g3;
        float* anim = (float*)*(int*)(p + 0x1400);

        g0 = anim[0];
        g1 = anim[1];
        g2 = anim[2];
        g3 = anim[3];
        boneCount = ModelBoneCount(*(int*)(p + 0x54));
        AnimApply(*(int*)(p + 0x7C), (char*)anim, animData, boneCount);
        anim[0] = g0;
        anim[1] = g1;
        anim[2] = g2;
        anim[3] = g3;
        *(int*)(p + 0x78) = 0;
        *(u32*)(p + 0x50) &= ~0x200u;
    }
    else
    {
        boneCount = ModelBoneCount(*(int*)(p + 0x54));
        AnimApply(*(int*)(p + 0x7C), *(char**)(p + 0x1400), animData, boneCount);
    }

after_anim:
    state = *(int*)(p + 0x78);
    if (state == 1)
    {
        float* anim;

        ActorGetMatrix(&mtx, (Actor*)*(int*)(p + 0x1BA0));
        if (*(u32*)(p + 0x50) & 0x4000)
        {
            anim = (float*)*(int*)(p + 0x1400);
            anim[0] = *(float*)(p + 0x1BF0);
            anim[1] = *(float*)(p + 0x1BF4);
            anim[2] = *(float*)(p + 0x1BF8);
            anim[3] = *(float*)(p + 0x1BFC);
            anim = (float*)*(int*)(p + 0x1400);
            anim[4] = *(float*)(p + 0x1BA8);
            anim[5] = *(float*)(p + 0x1BAC);
            anim[6] = *(float*)(p + 0x1BB0);
        }
        else if (*(u32*)(p + 0x50) & 0x8000)
        {
            *(float*)(p + 0x1474) = *(float*)(p + 0x1BA8);
            *(float*)(p + 0x1478) = *(float*)(p + 0x1BAC);
            *(float*)(p + 0x147C) = *(float*)(p + 0x1BB0);
            *(int*)(p + 0x1444) = (int)*(float*)(p + 0x1BA8);
            *(int*)(p + 0x1448) = (int)*(float*)(p + 0x1BAC);
            *(int*)(p + 0x144C) = (int)*(float*)(p + 0x1BB0);
        }
        ModelSetMatrix(*(int*)(p + 0x54), &mtx, *(char**)(p + 0x1400));
    }
    else if (state >= 0)
    {
        float* anim = (float*)*(int*)(p + 0x1400);

        anim[4] = lbl_804D65CC;
        anim[5] = lbl_804D65CC;
        anim[6] = lbl_804D65CC;
        ModelSetMatrix(*(int*)(p + 0x54), (Mtx34*)(p + 0x1450),
                       *(char**)(p + 0x1400));
    }

    if (*(int*)(p + 0x7A4) == 0xC || (*(u32*)(p + 0x50) & 0x8000) != 0)
    {
        PlayerModelUpdate((Player*)p);
        fn_800C64FC(*(int*)(p + 0x248));
        fn_800C6D08(lbl_804D65D8);
    }

finished:
    ;
}

// Applies the animation of the secondary (0x12C) model when it has one.
// Offsets are transcribed from retail; the name and the field meanings are
// guesses.
void PlayerSubModelAnimUpdate(float dt, void* owner)
{
    u8* p = (u8*)owner;
    float stream[4];

    if (*(int*)(p + 0x12C) != -1)
    {
        int animData;
        int boneCount;

        fn_800913EC(*(int*)(p + 0x12C), dt, stream);
        animData = (int)ModelAnimate(*(int*)(p + 0x64),
                                     *(char**)(p + 0x1418), 1);
        boneCount = ModelBoneCount(*(int*)(p + 0x64));
        AnimApply(*(int*)(p + 0x12C), *(char**)(p + 0x1418), animData,
                  boneCount);
    }
}

// --- the two held-item effect handles --------------------------------------

extern "C" int FxStart(int id, int arg1, int arg2);
extern "C" void fn_8016F018(int handle, int value);
extern "C" int lbl_804D452C;
extern "C" int lbl_804D4530;

extern "C" float lbl_804D65F8;
extern "C" float lbl_804D65FC;
extern "C" float lbl_804D6600;
extern "C" float lbl_804D6604;
extern "C" float lbl_804D6608;

// Ticks the two effect handles a player carries while holding an item (the
// charge sparkle at 0x708 and the pick-up glow at 0x710): starts / stops the
// effects and ramps their intensity toward a speed-dependent cap. Field
// offsets, effect ids and constants are transcribed from retail; the name and
// the field meanings are guesses.
void PlayerGunFxUpdate(float dt, void* owner)
{
    u8* p = (u8*)owner;
    int limit;
    int target;
    int cur;

    if (PlayerInHost((Actor*)owner) == 0)
    {
        if (*(int*)(p + 0x708) == -1 &&
            ActorPlayAnim(0x4E000000, 0xF4CEBF7B, 0x4E000000) == 0 &&
            fn_80049940() == 0)
        {
            *(int*)(p + 0x708) = FxStart(lbl_804D452C, 0, 0);
            *(int*)(p + 0x70C) = 0xC80;
        }

        if (*(int*)(p + 0x710) != -1)
        {
            fn_8016EEF4(*(int*)(p + 0x710));
            *(int*)(p + 0x710) = -1;
        }

        limit = (int)(lbl_804D6604 +
                      (lbl_804D6608 * *(float*)(p + 0x65C)) /
                          *(float*)(*(int*)(p + 0x738) + 8));
        target = 0x1000;
        if (limit < 0x1000)
            target = limit;

        cur = *(int*)(p + 0x70C);
        if (target < cur)
        {
            *(int*)(p + 0x70C) = cur - (int)(lbl_804D6600 * dt);
            if (*(int*)(p + 0x70C) < target)
                *(int*)(p + 0x70C) = target;
        }
        else
        {
            *(int*)(p + 0x70C) = cur + (int)(lbl_804D6600 * dt);
            if (target < *(int*)(p + 0x70C))
                *(int*)(p + 0x70C) = target;
        }

        if (*(int*)(p + 0x708) != -1)
            fn_8016F018(*(int*)(p + 0x708), *(int*)(p + 0x70C));
    }
    else
    {
        if (*(int*)(p + 0x708) != -1)
        {
            fn_8016EEF4(*(int*)(p + 0x708));
            *(int*)(p + 0x708) = -1;
        }

        if (**(int**)(p + 0x26C8) == 0x1091C ||
            **(int**)(p + 0x26C8) == (int)0xA7F71344)
        {
            if (*(int*)(p + 0x710) == -1)
                *(int*)(p + 0x710) = FxStart(lbl_804D4530, 0, 0);

            limit = (int)(lbl_804D65F8 +
                          (lbl_804D65FC * *(float*)(p + 0x65C)) /
                              *(float*)(*(int*)(p + 0x738) + 8));
            target = 0x1000;
            if (limit < 0x1000)
                target = limit;

            cur = *(int*)(p + 0x714);
            if (target < cur)
            {
                *(int*)(p + 0x714) = cur - (int)(lbl_804D6600 * dt);
                if (*(int*)(p + 0x714) < target)
                    *(int*)(p + 0x714) = target;
            }
            else
            {
                *(int*)(p + 0x714) = cur + (int)(lbl_804D6600 * dt);
                if (target < *(int*)(p + 0x714))
                    *(int*)(p + 0x714) = target;
            }

            if (*(int*)(p + 0x314) != 0)
                *(int*)(p + 0x714) = 0;

            if (*(int*)(p + 0x710) != -1)
                fn_8016F018(*(int*)(p + 0x710), *(int*)(p + 0x714));
        }
        else if (*(int*)(p + 0x710) != -1)
        {
            fn_8016EEF4(*(int*)(p + 0x710));
            *(int*)(p + 0x710) = -1;
        }
    }
}

extern "C" void* OwnerGetInfo(u32);
extern "C" void fn_8009F700(int*, void*, int);
extern "C" int fn_8001ACD4(void*, float*, float*, float*, float*, float*, u32,
                           u32 (*)(int*, float*), void*);
extern "C" int fn_80049750(void*);
extern "C" void fn_80045EBC(void*);
extern "C" void fn_8009ED70(void*, void*);
extern "C" void fn_800A0168(void*, void*);
extern "C" void fn_800A0030(void*, void*, void*);
u32 PlayerCollisionCallback(int*, float*);
int PlayerCollisionSlideVector(int, float*);

extern "C" float lbl_80343068;
extern "C" float lbl_8034306c;
extern "C" float lbl_80343070;

extern "C" int lbl_804d451c;
extern "C" int lbl_804d4520;
extern "C" int lbl_804d4524;
extern "C" int lbl_804d4528;

extern "C" float lbl_80423848;
extern "C" float lbl_8042384c;
extern "C" float lbl_80423850;
extern "C" float lbl_80423854;
extern "C" float lbl_80423858;
extern "C" float lbl_8042385c;
extern "C" float lbl_80423860;
extern "C" float lbl_80423864;
extern "C" float lbl_80423868;

extern "C" float lbl_80420048[];
extern "C" float lbl_80421048[];
extern "C" float lbl_80421448[];
extern "C" float lbl_80422448[];
extern "C" float lbl_80422848[];
extern "C" float lbl_80422c48[];

extern "C" float lbl_80394a28;
extern "C" float lbl_80394a2c;
extern "C" float lbl_80394a30;

extern "C" float lbl_804D660C;
extern "C" float lbl_804D6610;
extern "C" float lbl_804D6614;
extern "C" float lbl_804D6618;
extern "C" float lbl_804D661C;
extern "C" float lbl_804D6620;
extern "C" float lbl_804D6624;
extern "C" float lbl_804D6628;
extern "C" float lbl_804D662C;

// Player collision: queries the level collision system with the player's
// motion, slides the motion along the contact planes (iterating a few times
// until it settles) and writes the corrected motion back into `motion`. The
// retail debug output calls it "Player Collision". Offsets and constants are
// transcribed from retail; the helper semantics and the name are guesses.
void PlayerCollisionMove(void* owner, float* motion)
{
    u8* p = (u8*)owner;
    float base[3];
    float ref0[3];
    float ref1[3];
    float ref2[3];
    float pos[3];
    float contact[6];
    float prev[3];
    float nrm[3];
    float normMotion[3];
    float tmpVec[3];
    float corr[3];
    float hit[4];
    float slideBuf[12];
    float rotBuf[12];
    float crossBuf[4];
    float matBuf[12];
    int raw[3];
    u32 flags;
    int i;
    int j;
    int found;
    int iter;
    int saved;
    bool touching;
    bool slide;
    float accX;
    float accY;
    float accZ;

    lbl_804d4520 = *(int*)(p + 0x740);
    touching = false;

    base[0] = lbl_80343068;
    base[1] = lbl_8034306c;
    base[2] = lbl_80343070;

    if (((*(u32*)(p + 0x62c)) >> 24) == 2)
    {
        u8* info = (u8*)OwnerGetInfo(*(u32*)(p + 0x62c));

        if (*(u32*)(info + 0x7b4) & 0x40000)
            *(u32*)(p + 0x7b4) |= 0x40000;
    }

    pos[0] = *(float*)(p + 0x67c);
    pos[1] = *(float*)(p + 0x680);
    pos[2] = *(float*)(p + 0x684) + *(float*)(*(int*)(p + 0x738) + 0x68);

    fn_8009F700(raw, p, 0);
    ref0[0] = (float)(u32)raw[0];
    ref0[1] = (float)(u32)raw[1];
    ref0[2] = (float)(u32)raw[2];
    fn_800E1098(*(float*)(*(int*)(p + 0x738) + 0x6c), ref0, ref0);

    fn_8009F700(raw, p, 1);
    ref1[0] = (float)(u32)raw[0];
    ref1[1] = (float)(u32)raw[1];
    ref1[2] = (float)(u32)raw[2];
    fn_800E1098(*(float*)(*(int*)(p + 0x738) + 0x6c), ref1, ref1);

    fn_8009F700(raw, p, 2);
    ref2[0] = (float)(u32)raw[0];
    ref2[1] = (float)(u32)raw[1];
    ref2[2] = (float)(u32)raw[2];

    if (!(*(u32*)(p + 0x7b4) & 0x80000))
        fn_800E1098(lbl_804D65F4 * *(float*)(*(int*)(p + 0x738) + 0x70) +
                        *(float*)(*(int*)(p + 0x738) + 0x6c),
                    ref2, ref2);
    else
        fn_800E1098(lbl_804D65D8 + *(float*)(*(int*)(p + 0x738) + 0x6c),
                    ref2, ref2);

    lbl_80423848 = motion[0];
    lbl_8042384c = motion[1];
    lbl_80423850 = motion[2];

    accX = 0.0f;
    accY = 0.0f;
    accZ = 0.0f;
    iter = 0;
    slide = false;
    saved = lbl_804d451c;
    prev[0] = 0.0f;
    prev[1] = 0.0f;
    prev[2] = 0.0f;

    for (;;)
    {
        lbl_804d451c = saved;
        lbl_804d4524 = 0;
        *(float*)(p + 0x6bc) = lbl_804D660C;
        *(float*)(p + 0x6c0) = 0.0f;
        *(float*)(p + 0x6c4) = 0.0f;
        *(float*)(p + 0x6c8) = 0.0f;
        contact[0] = 0.0f;
        contact[1] = 0.0f;
        contact[2] = 0.0f;
        contact[3] = 0.0f;
        contact[4] = 0.0f;
        contact[5] = 0.0f;
        *(float*)(p + 0x14e8) = 0.0f;

        lbl_80423854 = lbl_80423848;
        lbl_80423858 = lbl_8042384c;
        lbl_8042385c = lbl_80423850;
        lbl_80423860 = pos[0];
        lbl_80423864 = pos[1];
        lbl_80423868 = pos[2];

        found = fn_8001ACD4(p, pos, ref0, ref1, ref2, &lbl_80423848,
                            0x10000000, PlayerCollisionCallback, contact);

        if (found == 0)
        {
            touching = false;
            accX += lbl_80423848;
            accY += lbl_8042384c;
            accZ += lbl_80423850;
            lbl_80423848 = 0.0f;
            lbl_8042384c = 0.0f;
            lbl_80423850 = 0.0f;

            if (fn_800E1020(motion) < lbl_804D65D8)
            {
                accY = 0.0f;
                accX = 0.0f;
            }
        }
        else
        {
            if (*(void**)(p + 0x828) != 0)
                (*(void (**)(void*))(p + 0x828))(p);

            if (!touching ||
                (touching &&
                 lbl_804D6610 <=
                     (prev[0] - contact[5]) * (prev[0] - contact[5]) +
                         (prev[1] - contact[3]) * (prev[1] - contact[3]) +
                         (prev[2] - contact[4]) * (prev[2] - contact[4])))
            {
                flags = *(u32*)(p + 0x7b4);

                if (fn_800E1020(motion) > lbl_804D6614)
                {
                    if (!(flags & 0x40000) && !(flags & 0x8000000))
                    {
                        accX = contact[3] - pos[0];
                        accZ = 0.0f;
                        accY = contact[4] - pos[1];
                    }
                    else
                    {
                        accX = contact[3] - pos[0];
                        accY = contact[4] - pos[1];
                        accZ = contact[5] - pos[2];
                    }
                }

                if (*(u32*)(p + 0x50) & 0x20000000)
                    fn_80045EBC(p);

                lbl_80423848 -= accX;
                lbl_8042384c -= accY;
                lbl_80423850 -= accZ;
                pos[0] += accX;
                pos[1] += accY;
                pos[2] += accZ;
                prev[1] = contact[3];
                prev[2] = contact[4];
                prev[0] = contact[5];

                if (!(fabsf(lbl_80423848) > lbl_804D65E0 ||
                      fabsf(lbl_8042384c) > lbl_804D65E0 ||
                      fabsf(lbl_80423850) > lbl_804D65E0))
                {
                    nrm[0] = 0.0f;
                    nrm[1] = 0.0f;
                    nrm[2] = 0.0f;
                }
                else
                {
                    float inv =
                        1.0f / sqrtf(lbl_80423848 * lbl_80423848 +
                                     lbl_8042384c * lbl_8042384c +
                                     lbl_80423850 * lbl_80423850);

                    nrm[0] = lbl_80423848 * inv;
                    nrm[1] = lbl_8042384c * inv;
                    nrm[2] = lbl_80423850 * inv;
                }

                for (i = 0; i < lbl_804d4524; i++)
                {
                    float* rec = &lbl_80420048[i * 4];
                    float* point = &lbl_80422c48[i * 3];
                    float depth = lbl_80422848[i];

                    if (!(flags & 0x40000) && !(flags & 0x8000000))
                    {
                        hit[0] = rec[0];
                        hit[1] = rec[1];
                        hit[2] = 0.0f;
                    }
                    else
                    {
                        hit[0] = rec[0];
                        hit[1] = rec[1];
                        hit[2] = rec[2];
                    }
                    hit[3] = 0.0f;

                    if (!(fabsf(hit[0]) > lbl_804D65E0 ||
                          fabsf(hit[1]) > lbl_804D65E0 ||
                          fabsf(hit[2]) > lbl_804D65E0))
                        continue;

                    PlayerCollisionSlideVector((int)slideBuf, hit);
                    fn_800A0168(rotBuf, slideBuf);
                    fn_800A0030(crossBuf, slideBuf, nrm);
                    fn_800A0030(nrm, rotBuf, crossBuf);

                    if (fn_800E1020(nrm) < lbl_804D6618)
                    {
                        nrm[0] = 0.0f;
                        nrm[1] = 0.0f;
                        nrm[2] = 0.0f;
                        break;
                    }

                    if (!(fabsf(nrm[0]) > lbl_804D65E0 ||
                          fabsf(nrm[1]) > lbl_804D65E0 ||
                          fabsf(nrm[2]) > lbl_804D65E0))
                    {
                        nrm[0] = 0.0f;
                        nrm[1] = 0.0f;
                        nrm[2] = 0.0f;
                    }
                    else
                    {
                        float inv = 1.0f / sqrtf(nrm[0] * nrm[0] +
                                                 nrm[1] * nrm[1] +
                                                 nrm[2] * nrm[2]);

                        nrm[0] *= inv;
                        nrm[1] *= inv;
                        nrm[2] *= inv;
                    }

                    {
                        float dot = hit[2] * base[2] +
                                    (hit[0] * base[0] + hit[1] * base[1]);

                        if (((!(flags & 0x40000) && !(flags & 0x8000000)) ||
                             nrm[2] <= 0.0f))
                        {
                            if (dot < lbl_804D661C)
                                slide = true;
                        }
                        else if (dot < lbl_804D65F4)
                        {
                            slide = true;
                        }
                    }

                    if ((fn_80049750(p) == 0 ||
                         ((*(u32*)(*(int*)(p + 0x26e4) + 0x38) >> 24) != 1)) &&
                        *(int*)(p + 0x7a4) != 0xc)
                    {
                        float speed = *(float*)(*(int*)(p + 0x738) + 0x6c);
                        float scaled;
                        float maxSpeed;
                        float amount;
                        float limit;
                        float baseAmt;

                        if (!(flags & 0x40000) && !(flags & 0x8000000))
                            scaled = lbl_804D6620 * speed;
                        else
                            scaled = speed;

                        if (depth < scaled)
                        {
                            for (j = 0; j < i; j++)
                                fn_8009F700(raw, p, 1);

                            corr[0] = pos[0] - point[0];
                            corr[1] = pos[1] - point[1];
                            corr[2] = pos[2] - point[2];

                            if (!(flags & 0x40000) && !(flags & 0x8000000))
                                corr[2] = 0.0f;

                            maxSpeed = speed;
                            if (maxSpeed < scaled)
                                maxSpeed = scaled;
                            amount = lbl_804D6618 * (maxSpeed - depth);

                            limit = lbl_804D6624;
                            if (lbl_804D6618 * speed < limit)
                                limit = lbl_804D6618 * speed;

                            if (!(flags & 0x40000) && !(flags & 0x8000000))
                            {
                                baseAmt = limit;
                                if (baseAmt < amount)
                                    baseAmt = amount;
                            }
                            else
                            {
                                baseAmt = lbl_804D6628 * limit;
                                if (baseAmt < amount)
                                    baseAmt = amount;
                            }

                            if (limit <= lbl_804D662C)
                                baseAmt = 0.0f;

                            if (((flags & 0x40000) || (flags & 0x8000000)) &&
                                point[2] <
                                    -(lbl_804D65F0 * ref2[2] - pos[2]))
                                corr[2] = 0.0f;

                            maxSpeed = speed;
                            if (maxSpeed < scaled)
                                maxSpeed = scaled;
                            amount = lbl_804D65D8 + (maxSpeed - depth);
                            if (baseAmt < amount)
                                amount = baseAmt;

                            fn_800E1098(amount, corr, corr);

                            accX += corr[0];
                            accY += corr[1];
                            accZ += corr[2];
                            *(float*)(p + 0x6c0) = 0.0f;
                            *(float*)(p + 0x6c4) = 0.0f;
                            *(float*)(p + 0x6c8) = 0.0f;
                        }
                    }
                }

                fn_8009ED70(matBuf, p);
                fn_800A0030(tmpVec, matBuf, &lbl_80423848);

                if (!(fabsf(lbl_80423848) > lbl_804D65E0 ||
                      fabsf(lbl_8042384c) > lbl_804D65E0 ||
                      fabsf(lbl_80423850) > lbl_804D65E0))
                {
                    normMotion[0] = 0.0f;
                    normMotion[1] = 0.0f;
                    normMotion[2] = 0.0f;
                }
                else
                {
                    float inv =
                        1.0f / sqrtf(lbl_80423848 * lbl_80423848 +
                                     lbl_8042384c * lbl_8042384c +
                                     lbl_80423850 * lbl_80423850);

                    normMotion[0] = lbl_80423848 * inv;
                    normMotion[1] = lbl_8042384c * inv;
                    normMotion[2] = lbl_80423850 * inv;
                }

                {
                    float dot = nrm[2] * normMotion[2] +
                                nrm[0] * normMotion[0] +
                                nrm[1] * normMotion[1];
                    float len;

                    if (dot == 0.0f)
                        dot = lbl_804D65E4;

                    len = fn_800E1020(tmpVec);
                    lbl_80423848 = dot * (len * nrm[0]);
                    lbl_8042384c = dot * (len * nrm[1]);
                    lbl_80423850 = dot * (len * nrm[2]);
                }
            }
            else
            {
                lbl_80423848 = 0.0f;
                lbl_8042384c = 0.0f;
                lbl_80423850 = 0.0f;
            }

            touching = true;
        }

        for (i = 0; i < lbl_804d4524; i++)
        {
            lbl_80421448[i * 4 + 0] = lbl_80420048[i * 4 + 0];
            lbl_80421448[i * 4 + 1] = lbl_80420048[i * 4 + 1];
            lbl_80421448[i * 4 + 2] = lbl_80420048[i * 4 + 2];
            lbl_80422448[i] = lbl_80421048[i];
            lbl_80421048[i] = 0.0f;
        }

        iter++;
        lbl_804d451c = 0;
        saved = 0;
        lbl_804d4528 = lbl_804d4524;
        lbl_80394a28 = 0.0f;
        lbl_80394a2c = 0.0f;
        lbl_80394a30 = 0.0f;
        lbl_804d451c = 0;

        if ((lbl_80423848 == 0.0f && lbl_8042384c == 0.0f) || iter > 5)
        {
            for (i = 0; i < 0x400; i++)
                lbl_80421448[i] = 0.0f;

            lbl_804d4528 = 0;
            motion[0] = accX;
            motion[1] = accY;
            motion[2] = accZ;
            lbl_80394a30 = 0.0f;
            lbl_80394a2c = 0.0f;
            lbl_80394a28 = 0.0f;
            *(int*)(p + 0x740) = lbl_804d451c;
            lbl_804d451c = 0;
            return;
        }
    }
}

extern "C" void fn_800A02C0(int, float*, int);
extern "C" float sqrtf(float);

// Builds the slide direction of a collision: turns the (x, y, z) part of
// `motion` into a perpendicular direction, crosses `motion` with it and writes
// -motion.w * motion into the +0x24 vector of the owning object. Returns 0 when
// `motion` is numerically zero. The three fn_800A02C0 calls are debug drawing
// passes (1 = motion, 2 = perpendicular, 0 = cross product). Offsets, constants
// and draw ids are transcribed from retail; the name and the field meanings are
// guesses.
int PlayerCollisionSlideVector(int obj, float* motion)
{
    float px;
    float py;
    float pz;
    float x = motion[0];
    float y = motion[1];
    float z = motion[2];

    if (!(fabsf(x) > lbl_804D65E0 || fabsf(y) > lbl_804D65E0 ||
          fabsf(z) > lbl_804D65E0))
        return 0;

    fn_800A02C0(obj, motion, 1);

    if (fabsf(x) < lbl_804D65E0 && fabsf(y) < lbl_804D65E0)
    {
        px = lbl_804D65CC;
        py = z;
        pz = lbl_804D65CC;
    }
    else if (fabsf(x) <= fabsf(y))
    {
        float len = sqrtf(y * y + z * z);

        px = lbl_804D65CC;
        py = z / len;
        pz = -y / len;
    }
    else
    {
        float len = sqrtf(x * x + z * z);

        px = z / len;
        py = lbl_804D65CC;
        pz = -x / len;
    }

    if (!(fabsf(px) > lbl_804D65E0 || fabsf(py) > lbl_804D65E0 ||
          fabsf(pz) > lbl_804D65E0))
    {
        px = lbl_804D65CC;
        py = lbl_804D65CC;
        pz = lbl_804D65CC;
    }
    else
    {
        float inv = 1.0f / sqrtf(px * px + py * py + pz * pz);

        px *= inv;
        py *= inv;
        pz *= inv;
    }

    fn_800A02C0(obj, &px, 2);

    {
        float nx = y * pz - z * py;
        float ny = z * px - x * pz;
        float nz = x * py - y * px;

        px = nx;
        py = ny;
        pz = nz;
    }

    fn_800A02C0(obj, &px, 0);

    {
        float depth = -motion[3];

        *(float*)(obj + 0x24) = x * depth;
        *(float*)(obj + 0x28) = y * depth;
        *(float*)(obj + 0x2C) = z * depth;
    }

    return 1;
}

// --- collision query callback ---------------------------------------------

extern "C" float lbl_804D6630;
extern "C" float lbl_804D6634;
extern "C" float lbl_804D6638;
extern "C" float lbl_804D663C;
extern "C" float lbl_804D6640;
extern "C" float lbl_804D6644;
extern "C" float lbl_804D6648;
extern "C" float lbl_804D664C;

extern "C" float lbl_80394a34;
extern "C" float lbl_80394a38;
extern "C" float lbl_80394a3c;
extern "C" u8 lbl_80393D7C;

extern "C" int fn_80049914(void);
extern "C" int fn_80212730(void);
extern "C" int fn_800A3990(u32 id);
extern "C" void fn_8028C500(void* msg);
extern "C" int fn_800265D4(u32 id, int msg, void* data);
extern "C" void fn_8028B9C0(void* msg, int flag);
extern "C" int fn_800A7AC4(void* obj, int msg);
extern "C" float sqrtf(float);

// Normalises a 3-vector in place; a near-zero vector becomes zero.
static inline void PlayerNormalizeVec3(float* v)
{
    if (!(fabsf(v[0]) > lbl_804D65E0 || fabsf(v[1]) > lbl_804D65E0 ||
          fabsf(v[2]) > lbl_804D65E0))
    {
        v[0] = lbl_804D65CC;
        v[1] = lbl_804D65CC;
        v[2] = lbl_804D65CC;
        return;
    }

    float inv = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    v[0] *= inv;
    v[1] *= inv;
    v[2] *= inv;
}

// Keeps the closest contact point seen so far when the hit object is not a
// player-like object (kind 2).
static inline void PlayerTrackClosest(u8* p, int* contact)
{
    float d[3];
    float len;

    if (*contact != 0 && (*(u32*)(*contact + 0x38) >> 24) == 2)
        return;

    d[0] = (float)contact[4] - lbl_80423860;
    d[1] = (float)contact[5] - lbl_80423864;
    d[2] = lbl_804D65CC;
    len = fn_800E1020(d);
    if (len < *(float*)(p + 0x6BC))
    {
        *(float*)(p + 0x6BC) = len;
        *(int*)(p + 0x6C0) = contact[4];
        *(int*)(p + 0x6C4) = contact[5];
        *(int*)(p + 0x6C8) = contact[6];
    }
}

// Copies the accumulated contact normal and contact point to the callback's
// output block.
static inline void PlayerWriteContact(float* out, const float* n)
{
    out[0] = n[0];
    out[1] = n[1];
    out[2] = n[2];
    out[3] = lbl_80394a34;
    out[4] = lbl_80394a38;
    out[5] = lbl_80394a3c;
}

// Out block of the 0x400 "who am I" query the collision system sends to both
// objects involved in a contact (field meanings are guesses).
struct CollisionQuery
{
    void* owner;
    u32 id;
    int field_08;
    int valid;
    int field_10;
    int field_14;
    u32 mask;
};

typedef void (*CollisionQueryFn)(int, void*);

struct CollisionQueryTarget
{
    u8 pad[0x30];
    CollisionQueryFn handler;
};

// Small stack message used for the kind-0x13 special case.
struct CollisionMessage
{
    u8 data[4];
    u32 hash;
    u32 field_08;
    void* ptr;
};

// Collision callback installed in the level collision query (fn_8001ACD4).
// It receives one candidate contact record (`contact`: [0] hit actor, [4..6]
// contact point, [7..9] stop point, [10..12] normal, [6] depth) and decides
// whether the contact blocks the move; accepted contacts are accumulated in
// the collision globals and the resulting slide normal goes to `out`.
// Returns 0 (no collision), 1 or 2 (collision recorded, 2 = list full).
// Offsets and constants are transcribed from retail; the name and the field
// meanings are guesses.
u32 PlayerCollisionCallback(int* contact, float* out)
{
    u8* p = (u8*)fn_80049914();
    float dir[3];
    float fwd[3];
    float vec[3];
    float acc[3];
    float nrm[3];
    float v[3];
    float contactZ;
    float dot;
    u32 flags = *(u32*)(p + 0x7B4);
    int state;
    int i;
    int count;
    bool blocked;

    dir[0] = lbl_80423854;
    dir[1] = lbl_80423858;
    dir[2] = lbl_8042385c;
    PlayerNormalizeVec3(dir);

    state = *(int*)(p + 0x634);
    if (state == 0 || state == 4 || state == 3)
    {
        fwd[0] = lbl_804D6630 * (float)(*(u32*)(p + 0x604) ^ 0x80000000);
        fwd[1] = lbl_804D6630 * (float)(*(u32*)(p + 0x608) ^ 0x80000000);
        fwd[2] = lbl_804D6630 * (float)(*(u32*)(p + 0x60c) ^ 0x80000000);
    }
    else
    {
        fwd[0] = lbl_804D65CC;
        fwd[1] = lbl_804D65CC;
        fwd[2] = lbl_804D65D8;
    }

    vec[0] = (float)contact[4] - *(float*)(p + 0x67C);
    vec[1] = (float)contact[5] - *(float*)(p + 0x680);
    vec[2] = (float)contact[6] - *(float*)(p + 0x684);
    contactZ = vec[2];
    PlayerNormalizeVec3(vec);

    if (((flags & 0x40000) != 0 || (flags & 0x8000000) != 0) &&
        (lbl_804D65D8 * dir[2] + lbl_804D65CC * dir[0] + lbl_804D65CC * dir[1] <
             lbl_804D6634 &&
         vec[2] * dir[2] + vec[0] * dir[0] + vec[1] * dir[1] < lbl_804D65CC &&
         *contact != 0) &&
        lbl_804D6638 *
                (*(float*)(*(int*)(p + 0x738) + 0x70) +
                 *(float*)(*(int*)(p + 0x738) + 0x6c)) +
            *(float*)(p + 0x684) <
            contactZ)
    {
        return 0;
    }

    if (vec[2] * dir[2] + vec[0] * dir[0] + vec[1] * dir[1] < lbl_804D663C &&
        lbl_804D662C < fn_800E1020(vec))
    {
        return 0;
    }

    vec[0] = (float)contact[7] - *(float*)(p + 0x67C);
    vec[1] = (float)contact[8] - *(float*)(p + 0x680);
    vec[2] = (float)contact[9] - *(float*)(p + 0x684);
    PlayerNormalizeVec3(vec);

    if (vec[2] * dir[2] + vec[0] * dir[0] + vec[1] * dir[1] < lbl_804D65CC &&
        lbl_804D65F4 * *(float*)(*(int*)(p + 0x738) + 0x6c) <
            vec[2] * vec[2] + vec[0] * vec[0] + vec[1] * vec[1])
    {
        return 0;
    }

    acc[0] = lbl_804D65CC;
    acc[1] = lbl_804D65CC;
    acc[2] = lbl_804D65CC;

    if (lbl_804D662C < fn_800E1020(&lbl_80423854) &&
        lbl_804D65D8 * lbl_8042385c + lbl_804D65CC * lbl_80423854 +
                lbl_804D65CC * lbl_80423858 <
            lbl_804D6620 &&
        contactZ < lbl_804D662C)
    {
        contact[6] = contact[9];
    }

    if ((flags & 0x40000) == 0 && (flags & 0x8000000) == 0)
    {
        float f1 = fwd[2] * (float)contact[0xc] +
                   (fwd[0] * (float)contact[10] + fwd[1] * (float)contact[0xb]);
        float f2 = lbl_804D65D8 * (float)contact[0xc] +
                   (lbl_804D65CC * (float)contact[10] +
                    lbl_804D65CC * (float)contact[0xb]);

        if (f2 < f1)
            f2 = f1;

        if ((*(u32*)(p + 0x678) & 0x200) == 0)
        {
            if (lbl_804D65F4 < f2 &&
                (float)contact[6] <
                    -(lbl_804D6640 *
                          (*(float*)(*(int*)(p + 0x738) + 0x70) -
                           *(float*)(*(int*)(p + 0x738) + 0x6c)) -
                      *(float*)(p + 0x684)))
            {
                return 0;
            }

            if (lbl_804D6644 < f2)
                return 0;
        }
    }

    for (i = 0; i < lbl_804d4528; i++)
    {
        float* rec = &lbl_80421448[i * 4];

        if ((fabsf(rec[0] - (float)contact[10]) <= lbl_804D65E0 &&
             fabsf(rec[1] - (float)contact[0xb]) <= lbl_804D65E0 &&
             fabsf(rec[2] - (float)contact[0xc]) <= lbl_804D65E0) ||
            lbl_804D6648 < rec[2] * (float)contact[0xc] +
                               rec[0] * (float)contact[10] +
                               rec[1] * (float)contact[0xb])
        {
            nrm[0] = lbl_80423848;
            nrm[1] = lbl_8042384c;
            nrm[2] = lbl_80423850;
            PlayerNormalizeVec3(nrm);

            v[0] = (float)contact[10];
            v[1] = (float)contact[0xb];
            v[2] = (float)contact[0xc];
            if ((flags & 0x40000) == 0 && (flags & 0x8000000) == 0)
                v[2] = lbl_804D65CC;

            if (lbl_804D65EC <= fn_800E1020(v))
            {
                PlayerNormalizeVec3(v);
                dot = nrm[2] * v[2] + nrm[0] * v[0] + nrm[1] * v[1];

                if (lbl_804D663C <= dot &&
                    (lbl_80422448[i] == lbl_804D65CC || lbl_804D664C <= dot))
                    return 0;

                PlayerWriteContact(out, nrm);
                PlayerTrackClosest(p, contact);
                return 1;
            }

            return 0;
        }
    }

    if (*contact != 0)
    {
        int actor = *contact;
        CollisionQuery hit;
        CollisionQuery player;
        u32 kind;

        hit.owner = (void*)actor;
        ((CollisionQueryTarget*)actor)->handler(0x400, &hit);

        player.owner = (void*)fn_80049914();
        ((CollisionQueryTarget*)player.owner)->handler(0x400, &player);

        if (player.valid == 0)
            return 0;

        if (hit.id == *(u32*)(p + 0x27F4) || hit.id == (u32)fn_80212730())
            return 0;

        kind = hit.id >> 24;

        if (kind == 2)
        {
            if (fn_80049750(player.owner) == 0)
                return 0;
            if (fn_80049750(hit.owner) == 0)
                return 0;
        }
        else if (kind < 2)
        {
            if (kind == 0)
            {
                if ((player.mask & hit.mask) == 0)
                    return 0;
            }
            else
            {
                if (fn_80049750(player.owner) == 0 && hit.mask == 2)
                    return 0;
            }
        }
        else if (kind == 0x13)
        {
            int obj = fn_800A3990(hit.id);

            if ((player.mask & 2) == 0 && (hit.mask & 2) == 0)
            {
                CollisionMessage msg;
                int result;
                void* old;

                fn_8028C500(&msg);
                msg.ptr = 0;
                msg.hash = 0x365F2878;
                msg.field_08 = 0;
                result = fn_800265D4(hit.id, 0x41E, msg.data);
                old = msg.ptr;
                msg.ptr = &lbl_80393D7C;
                fn_8028B9C0(&msg, 0);
                if (result != 0 && old == (void*)1)
                    return 0;
            }
            else
            {
                if (fn_80049750(player.owner) == 0)
                    return 0;
                if ((hit.mask & 2) == 0)
                    return 0;
            }

            if ((player.mask & hit.mask) == 0)
                return 0;
            if (fn_800A7AC4((u8*)obj + 0x270, 0xB) != 0)
                return 0;
            if (fn_800A7AC4((u8*)obj + 0x270, 10) != 0)
                return 0;
        }
        else
        {
            if ((player.mask & hit.mask) == 0)
                return 0;
        }
    }

    count = lbl_804d4524;
    blocked = false;
    for (i = 0; i < count; i++)
    {
        float* rec = &lbl_80420048[i * 4];

        if (lbl_804D6620 < rec[2] * (float)contact[0xc] +
                               rec[0] * (float)contact[10] +
                               rec[1] * (float)contact[0xb])
        {
            blocked = true;
            break;
        }
    }

    if (!blocked && lbl_804d4524 < 0x100)
    {
        int idx = lbl_804d4524;
        float* rec = &lbl_80420048[idx * 4];

        rec[0] = (float)contact[10];
        rec[1] = (float)contact[0xb];
        rec[2] = (float)contact[0xc];
        rec[3] = -((float)contact[0xc] * (float)contact[6] +
                   (float)contact[10] * (float)contact[4] +
                   (float)contact[0xb] * (float)contact[5]);

        lbl_80422c48[idx * 3 + 0] = (float)contact[4];
        lbl_80422c48[idx * 3 + 1] = (float)contact[5];
        lbl_80422c48[idx * 3 + 2] = (float)contact[6];

        vec[0] = (float)contact[4] - lbl_80423860;
        vec[1] = (float)contact[5] - lbl_80423864;
        vec[2] = (float)contact[6] - lbl_80423868;
        lbl_80422848[idx] = fn_800E1020(vec);

        for (i = 0; i < idx; i++)
        {
            rec = &lbl_80420048[i * 4];
            if (rec[2] * (float)contact[0xc] + rec[0] * (float)contact[10] +
                    rec[1] * (float)contact[0xb] <
                lbl_804D664C ||
                lbl_804D65E0 < rec[3] + rec[2] * (float)contact[6] +
                                   rec[0] * (float)contact[4] +
                                   rec[1] * (float)contact[5])
            {
                lbl_80421048[i] = lbl_804D65D8;
                lbl_80421048[idx + i] = lbl_804D65D8;
            }
        }

        lbl_804d4524++;
    }

    if (lbl_804d451c == 0)
    {
        lbl_80394a34 = (float)contact[7];
        lbl_80394a38 = (float)contact[8];
        lbl_80394a3c = (float)contact[9];
        lbl_80394a28 += (float)contact[10];
        lbl_80394a2c += (float)contact[0xb];
        lbl_80394a30 += (float)contact[0xc];
        lbl_804d451c = 1;

        acc[0] = lbl_80394a28;
        acc[1] = lbl_80394a2c;
        acc[2] = lbl_80394a30;
        PlayerNormalizeVec3(acc);
        PlayerWriteContact(out, acc);
        PlayerTrackClosest(p, contact);

        if (lbl_804d4524 < 0x100)
            return 2;
        return 1;
    }

    if (lbl_804D6624 < fabsf(lbl_80394a34 - (float)contact[7]) ||
        lbl_804D6624 < fabsf(lbl_80394a38 - (float)contact[8]))
    {
        acc[0] = lbl_80394a28 + (float)contact[10];
        acc[1] = lbl_80394a2c + (float)contact[0xb];
        acc[2] = lbl_80394a30 + (float)contact[0xc];
        PlayerNormalizeVec3(acc);
        PlayerWriteContact(out, acc);

        lbl_804d451c = 0;
        lbl_80394a28 = lbl_804D65CC;
        lbl_80394a2c = lbl_804D65CC;
        lbl_80394a30 = lbl_804D65CC;

        PlayerTrackClosest(p, contact);
        return 1;
    }

    if (!blocked)
    {
        lbl_804d451c++;
        lbl_80394a28 += (float)contact[10];
        lbl_80394a2c += (float)contact[0xb];
        lbl_80394a30 += (float)contact[0xc];
    }

    acc[0] = lbl_80394a28;
    acc[1] = lbl_80394a2c;
    acc[2] = lbl_80394a30;
    PlayerNormalizeVec3(acc);
    PlayerWriteContact(out, acc);
    PlayerTrackClosest(p, contact);

    if (lbl_804d4524 < 0x100)
        return 2;
    return 1;
}

int PlayerUpdateGunSway(void*, int, float dt);
int PlayerUpdateMotionBrake(void*, int, float dt);
int PlayerUpdateMotionPath(void*, float);
extern "C" void fn_80074018(void*, int);

// Picks the post-collision update path from the movement flags at +0x7B4. A
// blocked / host move resets the state word at +0x634, sets flag 8 and runs the
// matching motion handler; otherwise it falls through to the plain motion
// update. Offsets and flag bits are transcribed from retail; the name and the
// flag meanings are guesses.
void PlayerSelectCollisionUpdate(void* owner, float dt)
{
    u8* p = (u8*)owner;
    u32 flags = *(u32*)(p + 0x7B4);

    if (flags & 0x40000)
    {
        *(int*)(p + 0x634) = 0;
        *(u32*)(p + 0x7B4) |= 8;
        PlayerUpdateGunSway(p, 1, dt);
    }
    else if (flags & 0x4000000)
    {
        *(int*)(p + 0x634) = 0;
        *(u32*)(p + 0x7B4) |= 8;
        PlayerUpdateMotionBrake(p, 1, dt);
    }
    else if (flags & 0x8000000)
    {
        PlayerUpdateMotionPath(p, dt);
    }
    else
    {
        fn_80074018(p, 1);
    }
}

// Picks the post-collision motion update from the movement flags at +0x7B4:
// a blocked / host move resets the state word at +0x634, sets flag 8 and runs
// the matching motion handler, otherwise it falls through to the plain motion
// update. Same dispatch shape as PlayerSelectCollisionUpdate, but forwards the
// caller's mode word instead of a literal 1. Offsets and flag bits are
// transcribed from retail; the name and the flag meanings are guesses.
void PlayerDispatchMotionUpdate(void* owner, int mode, float dt)
{
    u8* p = (u8*)owner;
    u32 flags = *(u32*)(p + 0x7B4);

    if (flags & 0x40000)
    {
        *(int*)(p + 0x634) = 0;
        *(u32*)(p + 0x7B4) |= 8;
        PlayerUpdateGunSway(p, mode, dt);
    }
    else if (flags & 0x4000000)
    {
        *(int*)(p + 0x634) = 0;
        *(u32*)(p + 0x7B4) |= 8;
        PlayerUpdateMotionBrake(p, mode, dt);
    }
    else if (flags & 0x8000000)
    {
        PlayerUpdateMotionPath(p, dt);
    }
    else
    {
        fn_80074018(p, mode);
    }
}

// --- per-frame motion integration -----------------------------------------
// Helpers defined in other units (plain C symbols from retail).
void PlayerResetGunTimer(void* owner);;
void PlayerCalcMotionTarget(float dt, Player* player, float* out);
extern "C" int fn_800F7C64(void*, unsigned long);
extern "C" int fn_800FC3B8(int);
extern "C" void fn_800495BC(void*, int, int);
extern "C" float lbl_804D4548;
extern "C" float lbl_804D6650;
extern "C" float lbl_804D6618;
extern "C" float lbl_804D6654;
int VarFindInt(unsigned long, unsigned long, unsigned long);

// Per-frame motion update of a player-like object: announces the motion
// message (0x41E) and then, depending on the "in host" flag (0x1000), either
// integrates the stick-driven velocity against the tracked position or snaps
// the object to the host player's state. The name and the field meanings are
// guesses; offsets, flag bits and constants are transcribed from retail
// (0x8007215C).
int PlayerUpdateMotionInput(float dt, void* owner)
{
    u8* p = (u8*)owner;
    String s;
    int data[3];
    u8* state;
    float scale;
    float xScale;
    float yScale;
    float zScale;
    Mtx34 mtx;
    Mtx34 inv;
    Mtx34 back;
    float target[3];
    float move[3];
    float pos[3];
    int result;
    float v;

    data[1] = 0x2DA8293C;
    data[2] = 0;
    *(int*)&s = 0;
    SendMsg(*(int*)(p + 0x38), 0x41E, data);

    state = *(u8**)(p + 0x738);
    scale = *(float*)(p + 0x734);
    xScale = *(float*)(state + 0x10) * scale;
    yScale = *(float*)(state + 0x14) * scale;
    zScale = *(float*)(state + 0x2C) * scale;

    *(u32*)(p + 0x7B4) |= 8;
    *(u32*)(p + 0x7B4) |= 0x40000;
    *(u32*)(p + 0x7B4) &= ~0x1000u;

    if (fn_800F7C64(*(void**)&s, 0x3BFF94C2) == 0)
        fn_800FC3B8(0xF);

    if ((*(u32*)(p + 0x7B4) & 0x1000) == 0)
    {
        PlayerResetGunTimer(p);
        ActorGetMatrix(&mtx, (Actor*)p);
        fn_800A0030(pos, &mtx, (float*)(p + 0x650));

        move[0] = lbl_804D65CC;
        move[1] = lbl_804D65CC;
        move[2] = lbl_804D65CC;

        v = *(float*)(p + 0x394);
        if (v > lbl_804D65CC)
        {
            move[1] = fabsf(v) * xScale;
            PlayerResetGunTimer(p);
        }
        else if (v < lbl_804D65CC)
        {
            move[1] = -fabsf(v) * xScale;
            PlayerResetGunTimer(p);
        }

        v = *(float*)(p + 0x38C);
        if (v < lbl_804D65CC)
        {
            move[0] = -fabsf(v) * yScale;
            PlayerResetGunTimer(p);
        }
        else if (v > lbl_804D65CC)
        {
            move[0] = fabsf(v) * yScale;
            PlayerResetGunTimer(p);
        }

        fn_800A0168(&inv, (Mtx34*)(p + 0x1C0));
        move[1] += lbl_804D4548;
        fn_800A0030(move, &inv, move);
        fn_800A0030(move, &mtx, move);

        PlayerCalcMotionTarget(dt, (Player*)p, target);
        fn_800A0168(&inv, (Mtx34*)(p + 0x1C0));
        fn_800A0030(target, &inv, target);
        fn_800A0030(target, &mtx, target);

        move[0] += target[0];
        move[1] += target[1];
        move[2] += target[2];

        if (pos[0] != move[0])
        {
            if (pos[0] < move[0])
            {
                pos[0] = dt * zScale + pos[0];
                if (pos[0] > move[0])
                    pos[0] = move[0];
            }
            if (pos[0] > move[0])
            {
                pos[0] = pos[0] - dt * zScale;
                if (pos[0] < move[0])
                    pos[0] = move[0];
            }
        }
        if (pos[1] != move[1])
        {
            if (pos[1] < move[1])
            {
                pos[1] = dt * zScale + pos[1];
                if (pos[1] > move[1])
                    pos[1] = move[1];
            }
            if (pos[1] > move[1])
            {
                pos[1] = pos[1] - dt * zScale;
                if (pos[1] < move[1])
                    pos[1] = move[1];
            }
        }
        if (pos[2] != move[2])
        {
            if (pos[2] < move[2])
            {
                pos[2] = dt * zScale + pos[2];
                if (pos[2] > move[2])
                    pos[2] = move[2];
            }
            if (pos[2] > move[2])
            {
                pos[2] = pos[2] - dt * zScale;
                if (pos[2] < move[2])
                    pos[2] = move[2];
            }
        }

        if (pos[2] == lbl_804D65CC)
            pos[2] = -(*(float*)(state + 0x7C) * dt);

        fn_800A0168(&back, &mtx);
        fn_800A0030((float*)(p + 0x650), &back, pos);

        if (*(float*)(p + 0x658) <= lbl_804D6650)
            *(float*)(p + 0x658) = lbl_804D6650;
        else if (*(float*)(p + 0x658) < lbl_804D6650)
            *(float*)(p + 0x658) = lbl_804D6650;

        if (*(int*)(p + 0x73C) != 4)
            *(u32*)(p + 0x678) |= 0x80;

        result = 0;
        *(int*)(p + 0x73C) = 4;
    }
    else
    {
        int host = PlayerInHost((Actor*)p);
        int id = VarFindInt(0x7982059Cu,
                            *(u32*)(*(u8**)(p + host * 4 + 0x26C4) + 0x15C),
                            0xCD35FA9Du);
        float zero;

        fn_800495BC(p, id, 0);
        zero = lbl_804D65CC;
        result = 1;
        *(u32*)(p + 0x7B4) &= ~0x40000u;
        *(u32*)(p + 0x7B4) &= ~0x8000000u;
        *(u32*)(p + 0x7B4) &= ~0x4000000u;
        *(u32*)(p + 0x7B4) &= ~8u;
        *(float*)(p + 0x658) = zero;
        *(u32*)(p + 0x610) = *(u32*)(p + 0x614);
        *(float*)(p + 0x618) = zero;
    }

    return result;
}

// Per-frame motion update of a player-like object along its tracked path: like
// PlayerUpdateMotionInput it announces the motion message (0x41E), but the
// non-host path clamps the current velocity towards the next path point
// instead of taking a stick-derived velocity. Name and field meanings are
// guesses; offsets, flag bits and constants are transcribed from retail
// (0x800725F8).
int PlayerUpdateMotionSeek(float dt, void* owner)
{
    u8* p = (u8*)owner;
    String s;
    int data[3];
    u8* state;
    float scale;
    float lengthLimit;
    float speed;
    Mtx34 mtx;
    Mtx34 inv;
    Mtx34 back;
    float target[3];
    float move[3];
    float pos[3];
    int result;

    data[1] = 0x2DA8293C;
    data[2] = 0;
    *(int*)&s = 0;
    SendMsg(*(int*)(p + 0x38), 0x41E, data);

    state = *(u8**)(p + 0x738);
    scale = *(float*)(p + 0x734);
    lengthLimit = *(float*)(state + 0x10) * scale;
    speed = *(float*)(state + 0x2C) * scale;

    *(u32*)(p + 0x7B4) |= 8;
    *(u32*)(p + 0x7B4) |= 0x40000;
    *(u32*)(p + 0x7B4) &= ~0x1000u;

    if (fn_800F7C64(*(void**)&s, 0x3BFF94C2) == 0)
        fn_800FC3B8(0xF);

    if ((*(u32*)(p + 0x7B4) & 0x1000) == 0)
    {
        PlayerResetGunTimer(p);
        ActorGetMatrix(&mtx, (Actor*)p);
        fn_800A0030(pos, &mtx, (float*)(p + 0x650));

        if (fn_800E1020(pos) > lengthLimit)
            fn_800E1098(lengthLimit, pos, pos);

        fn_800A0168(&inv, (Mtx34*)(p + 0x1C0));
        move[0] = lbl_804D65CC;
        move[1] = lbl_804D65CC;
        move[2] = lbl_804D65CC;
        fn_800A0030(move, &inv, move);
        fn_800A0030(move, &mtx, move);

        PlayerCalcMotionTarget(dt, (Player*)p, target);
        fn_800A0168(&inv, (Mtx34*)(p + 0x1C0));
        fn_800A0030(move, &inv, target);
        fn_800A0030(move, &mtx, move);

        if (pos[0] != move[0])
        {
            if (pos[0] < move[0])
            {
                pos[0] = dt * speed + pos[0];
                if (pos[0] > move[0])
                    pos[0] = move[0];
            }
            if (pos[0] > move[0])
            {
                pos[0] = pos[0] - dt * speed;
                if (pos[0] < move[0])
                    pos[0] = move[0];
            }
        }
        if (pos[1] != move[1])
        {
            if (pos[1] < move[1])
            {
                pos[1] = dt * speed + pos[1];
                if (pos[1] > move[1])
                    pos[1] = move[1];
            }
            if (pos[1] > move[1])
            {
                pos[1] = pos[1] - dt * speed;
                if (pos[1] < move[1])
                    pos[1] = move[1];
            }
        }
        if (pos[2] != move[2])
        {
            if (pos[2] < move[2])
            {
                pos[2] = dt * speed + pos[2];
                if (pos[2] > move[2])
                    pos[2] = move[2];
            }
            if (pos[2] > move[2])
            {
                pos[2] = pos[2] - dt * speed;
                if (pos[2] < move[2])
                    pos[2] = move[2];
            }
        }

        if (pos[2] == lbl_804D65CC)
            pos[2] = -(*(float*)(state + 0x7C) * dt);

        fn_800A0168(&back, &mtx);
        fn_800A0030((float*)(p + 0x650), &back, pos);

        if (*(float*)(p + 0x658) <= lbl_804D6650)
            *(float*)(p + 0x658) = lbl_804D6650;
        else if (*(float*)(p + 0x658) < lbl_804D6650)
            *(float*)(p + 0x658) = lbl_804D6650;

        if (*(int*)(p + 0x73C) != 4)
            *(u32*)(p + 0x678) |= 0x80;

        result = 0;
        *(int*)(p + 0x73C) = 4;
    }
    else
    {
        int host = PlayerInHost((Actor*)p);
        int id = VarFindInt(0x7982059Cu,
                            *(u32*)(*(u8**)(p + host * 4 + 0x26C4) + 0x15C),
                            0xCD35FA9Du);
        float zero;

        fn_800495BC(p, id, 0);
        zero = lbl_804D65CC;
        result = 1;
        *(u32*)(p + 0x7B4) &= ~0x40000u;
        *(u32*)(p + 0x7B4) &= ~0x8000000u;
        *(u32*)(p + 0x7B4) &= ~0x4000000u;
        *(u32*)(p + 0x7B4) &= ~8u;
        *(float*)(p + 0x658) = zero;
        *(u32*)(p + 0x610) = *(u32*)(p + 0x614);
        *(float*)(p + 0x618) = zero;
    }

    return result;
}

// Per-frame motion target of a player-like object. Announces the motion
// message (0x41E) when the object has a message target, then either snaps the
// tracked velocity (0x72C) to the host state's limits or eases it towards them.
// The name and the field meanings are guesses; offsets, hashes and constants
// are transcribed from retail (0x800729E0).
void PlayerCalcMotionTarget(float dt, Player* player, float* out)
{
    u8* p = (u8*)player;
    String s;
    int data[2];
    int active = 0;
    u8* state;

    if (*(float*)(*(u8**)(p + 0x738) + 0x80) <= lbl_804D65CC)
    {
        *(float*)(p + 0x72C) = lbl_804D65CC;
    }
    else
    {
        if (*(int*)(p + 0x26E4) != 0)
        {
            data[1] = 0x06AB33CB;
            SendMsg(*(int*)(*(u8**)(p + 0x26E4) + 0x38), 0x41E, data);
            active = *(int*)&s;
        }
        else if (*(int*)(p + 0x74C) == 1)
        {
            active = 1;
        }

        state = *(u8**)(p + 0x738);
        if (*(int*)(p + 0x31C) != 0)
        {
            *(float*)(p + 0x730) = *(float*)(state + 0x84);
        }
        else
        {
            *(float*)(p + 0x730) = *(float*)(state + 0x80);
        }

        if (active == 0)
        {
            *(float*)(p + 0x72C) = lbl_804D65CC;
        }
        else
        {
            state = *(u8**)(p + 0x738);
            if (*(float*)(p + 0x72C) < *(float*)(state + 0x80))
            {
                *(float*)(p + 0x72C) = *(float*)(state + 0x80);
            }
            else if (*(float*)(p + 0x72C) < *(float*)(p + 0x730))
            {
                *(float*)(p + 0x72C) = *(float*)(p + 0x72C) + lbl_804D6618 * dt;
                if (*(float*)(p + 0x72C) > *(float*)(p + 0x730))
                {
                    *(float*)(p + 0x72C) = *(float*)(p + 0x730);
                }
            }
            else if (*(float*)(p + 0x72C) > *(float*)(p + 0x730))
            {
                *(float*)(p + 0x72C) = *(float*)(p + 0x72C) - lbl_804D6654 * dt;
                if (*(float*)(p + 0x72C) < *(float*)(p + 0x730))
                {
                    *(float*)(p + 0x72C) = *(float*)(p + 0x730);
                }
            }
        }
    }

    out[0] = lbl_804D65CC;
    out[1] = *(float*)(p + 0x72C);
    out[2] = lbl_804D65CC;
}

// Per-frame positional offset of a player-like object: takes the host state's
// vertical offset (0x74) in the object's local frame and adds it to the
// tracked position (0x650). Returns 1. Offsets and hashes are transcribed from
// retail (0x80072B90); the name is a guess.
int PlayerApplyFrameOffset(Player* player)
{
    u8* p = (u8*)player;
    float offset[3];
    Mtx34 mtx;
    int host;

    offset[0] = lbl_804D65CC;
    offset[1] = *(float*)(*(u8**)(p + 0x738) + 0x74);
    offset[2] = lbl_804D65CC;

    fn_800A0168(&mtx, (Mtx34*)(p + 0x1C0));
    fn_800A0030(offset, &mtx, offset);

    *(float*)(p + 0x650) = *(float*)(p + 0x650) + offset[0];
    *(float*)(p + 0x654) = *(float*)(p + 0x654) + offset[1];
    *(float*)(p + 0x658) = *(float*)(p + 0x658) + offset[2];
    *(float*)(p + 0x658) = *(float*)(p + 0x658) +
                           *(float*)(*(u8**)(p + 0x738) + 0x78);

    host = PlayerInHost((Actor*)p);
    fn_800495BC(p, VarFindInt(0x7982059C,
                               *(int*)(*(u8**)(p + host * 4 + 0x26C4) + 0x15C),
                               0x22EC2E),
                0);
    return 1;
}

// Per-frame motion update of a player-like object: ticks the three action
// timers at +0x2664/+0x265C/+0x2660, announces the motion message (0x41E),
// then rebuilds the tracked position from the actor matrix and brakes the
// speed at +0x658 by the animation rate. If the "in host" flag (0x1000) was
// set and the speed crossed zero, the move flags are cleared and 1 returned.
// Name and field meanings are guesses; offsets, flag bits and constants are
// transcribed from retail (0x80072C74).
extern "C" float lbl_804D6658;
extern "C" float lbl_804D665C;
extern "C" int lbl_804D2904;
extern "C" float lbl_804D2908;
extern "C" float lbl_804D290C;
extern "C" float lbl_804D2910;
extern "C" float fn_80221100(void*);

int PlayerUpdateMotionBrake(void* owner, int mode, float dt)
{
    u8* p = (u8*)owner;
    float pos[3];
    int data[3];
    String s;
    Mtx34 mtx;
    Mtx34 inv;
    int reversed = 0;
    int stop;
    float speed;

    data[1] = 0x2DA8293C;
    data[2] = 0;
    *(int*)&s = 0;
    SendMsg(*(int*)(p + 0x38), 0x41E, data);

    stop = 0;

    if (*(float*)(p + 0x2664) > lbl_804D65CC)
    {
        *(float*)(p + 0x2664) -= dt;
        if (*(float*)(p + 0x2664) < lbl_804D65CC)
            *(float*)(p + 0x2664) = lbl_804D65CC;
    }
    else if (*(float*)(p + 0x265C) > lbl_804D65CC)
    {
        *(float*)(p + 0x265C) -= dt;
        if (*(float*)(p + 0x265C) < lbl_804D65CC)
            *(float*)(p + 0x265C) = lbl_804D65CC;
    }
    else if (*(float*)(p + 0x2660) > lbl_804D65CC)
    {
        *(float*)(p + 0x2660) -= dt;
        if (*(float*)(p + 0x2660) < lbl_804D65CC)
            *(float*)(p + 0x2660) = lbl_804D65CC;
    }

    if (fn_800F7C64(*(void**)&s, 0x8B267329u) != 0 || fn_800FC3B8(0x10) != 0)
        PlayerInHost((Actor*)p);

    if (fn_800F7C64(*(void**)&s, 0x3BFF94C2u) != 0 || fn_800FC3B8(0xF) != 0)
        stop = 1;

    if (*(u32*)(p + 0x7B4) & 0x1000)
    {
        reversed = 1;
        *(float*)(p + 0x658) = -(lbl_804D65F4 * *(float*)(p + 0x658));
        *(u32*)(p + 0x7B4) &= ~0x1000u;
    }

    PlayerResetGunTimer(p);
    ActorGetMatrix(&mtx, (Actor*)p);
    fn_800A0030(pos, &mtx, (float*)(p + 0x650));
    fn_800A0168(&inv, &mtx);
    fn_800A0030((float*)(p + 0x650), &inv, pos);

    speed = *(float*)(p + 0x658);
    if (speed > lbl_804D6650)
    {
        if (!stop)
            *(float*)(p + 0x658) =
                speed - *(float*)(*(int*)(p + 0x738) + 0x7C) * dt;
        else
            *(float*)(p + 0x658) =
                speed - lbl_804D6638 *
                            (*(float*)(*(int*)(p + 0x738) + 0x7C) * dt);

        if (reversed == 1 && speed > lbl_804D65CC &&
            *(float*)(p + 0x658) < lbl_804D65CC)
        {
            *(u32*)(p + 0x7B4) &= ~0x40000u;
            *(u32*)(p + 0x7B4) &= ~0x8000000u;
            *(u32*)(p + 0x7B4) &= ~0x4000000u;
            *(u32*)(p + 0x7B4) &= ~8u;
            *(float*)(p + 0x658) = lbl_804D65CC;
            *(u32*)(p + 0x610) = *(u32*)(p + 0x614);
            *(float*)(p + 0x618) = lbl_804D65CC;
            return 1;
        }

        if (*(float*)(p + 0x658) < lbl_804D6650)
            *(float*)(p + 0x658) = lbl_804D6650;
    }
    else
    {
        *(float*)(p + 0x658) = lbl_804D6650;
    }

    if (*(int*)(p + 0x73C) != 4)
        *(u32*)(p + 0x678) |= 0x80;

    *(int*)(p + 0x73C) = 4;
    return 0;
}

// Per-frame motion update of a player-like object walking a path: runs the
// small state machine at +0x720, announces the motion message (0x41E), then
// advances the tracked position from the actor matrix with the state's axis
// scales. If the "in host" flag (0x1000) is set and the speed reached zero,
// the move flags are cleared and 1 returned. Name and field meanings are
// guesses; offsets, flag bits and constants are transcribed from retail
// (0x80072F60).
int PlayerUpdateMotionPath(void* owner, float dt)
{
    u8* p = (u8*)owner;

    switch (*(int*)(p + 0x720))
    {
    case 1:
        if (*(float*)(p + 0x658) <= lbl_804D65CC)
        {
            *(int*)(p + 0x720) = 2;
            *(float*)(p + 0x724) = lbl_804D65CC;
        }
        break;

    case 2:
        *(float*)(p + 0x724) += dt;
        if (lbl_804D2904 == 0 || *(float*)(p + 0x724) >= lbl_804D2908 ||
            *(int*)(p + 0x328) == 0)
        {
            *(float*)(p + 0x724) = lbl_804D65CC;
            *(int*)(p + 0x720) = 3;
            *(float*)(p + 0x658) = lbl_804D6658;
        }
        break;
    }

    if ((*(u32*)(p + 0x7B4) & 0x1000) &&
        *(float*)(p + 0x658) <= lbl_804D65CC)
    {
        int host = PlayerInHost((Actor*)p);
        int id = VarFindInt(0x7982059Cu,
                            *(u32*)(*(u8**)(p + host * 4 + 0x26C4) + 0x15C),
                            0xCD35FA9Du);
        float zero;

        fn_800495BC(p, id, 0);
        zero = lbl_804D65CC;
        *(int*)(p + 0x720) = 0;
        *(u32*)(p + 0x7B4) &= ~0x40000u;
        *(u32*)(p + 0x7B4) &= ~0x8000000u;
        *(u32*)(p + 0x7B4) &= ~0x4000000u;
        *(u32*)(p + 0x7B4) &= ~8u;
        *(float*)(p + 0x658) = zero;
        *(u32*)(p + 0x610) = *(u32*)(p + 0x614);
        *(float*)(p + 0x618) = zero;
        return 1;
    }

    float pos[3];
    int data[3];
    String s;
    Mtx34 mtx;
    Mtx34 inv;
    float xScale;
    float yScale;
    float zScale;
    float moveX = lbl_804D65CC;
    float moveZ = lbl_804D65CC;
    float v;
    int scaleDown = 0;

    data[1] = 0x2DA8293C;
    data[2] = 0;
    *(int*)&s = 0;
    SendMsg(*(int*)(p + 0x38), 0x41E, data);

    if (lbl_804D2904 == 1)
    {
        switch (*(int*)(p + 0x720))
        {
        case 3:
            xScale = lbl_804D290C;
            yScale = lbl_804D2910;
            zScale = *(float*)(*(int*)(p + 0x738) + 0x2C);
            break;

        case 0:
        case 1:
        case 2:
            xScale = *(float*)(*(int*)(p + 0x738) + 0x10);
            yScale = *(float*)(*(int*)(p + 0x738) + 0x14);
            zScale = *(float*)(*(int*)(p + 0x738) + 0x2C);
            break;
        }
    }
    else
    {
        xScale = *(float*)(*(int*)(p + 0x738) + 0x10);
        yScale = *(float*)(*(int*)(p + 0x738) + 0x14);
        zScale = *(float*)(*(int*)(p + 0x738) + 0x2C);
    }

    float scale = *(float*)(p + 0x734);
    xScale *= scale;
    yScale *= scale;
    zScale *= scale;

    if (*(float*)(p + 0x2664) > lbl_804D65CC)
    {
        xScale *= lbl_804D65CC;
        yScale *= lbl_804D65CC;
        *(float*)(p + 0x2664) -= dt;
        if (*(float*)(p + 0x2664) < lbl_804D65CC)
            *(float*)(p + 0x2664) = lbl_804D65CC;
    }
    else if (*(float*)(p + 0x265C) > lbl_804D65CC)
    {
        xScale *= lbl_804D665C;
        yScale *= lbl_804D665C;
        *(float*)(p + 0x265C) -= dt;
        if (*(float*)(p + 0x265C) < lbl_804D65CC)
            *(float*)(p + 0x265C) = lbl_804D65CC;
    }
    else if (*(float*)(p + 0x2660) > lbl_804D65CC)
    {
        xScale *= lbl_804D6638;
        yScale *= lbl_804D6638;
        *(float*)(p + 0x2660) -= dt;
        if (*(float*)(p + 0x2660) < lbl_804D65CC)
            *(float*)(p + 0x2660) = lbl_804D65CC;
    }

    if (*(int*)(p + 0x9EC) != 0)
    {
        float k = fn_80221100(p);
        xScale *= k;
        yScale *= k;
    }

    if ((fn_800F7C64(*(void**)&s, 0x8B267329u) != 0 ||
         fn_800FC3B8(0x10) != 0) &&
        PlayerInHost((Actor*)p) != 0)
        scaleDown = 1;

    if (scaleDown)
    {
        xScale *= lbl_804D662C;
        yScale *= lbl_804D662C;
        zScale *= lbl_804D662C;
    }

    PlayerResetGunTimer(p);
    ActorGetMatrix(&mtx, (Actor*)p);
    fn_800A0030(pos, &mtx, (float*)(p + 0x650));

    v = *(float*)(p + 0x394);
    if (v > lbl_804D65CC)
    {
        moveX = fabsf(v) * xScale;
        PlayerResetGunTimer(p);
    }
    else if (v < lbl_804D65CC)
    {
        moveX = -fabsf(v) * xScale;
        PlayerResetGunTimer(p);
    }

    v = *(float*)(p + 0x38C);
    if (v < lbl_804D65CC)
    {
        moveZ = -fabsf(v) * yScale;
        PlayerResetGunTimer(p);
    }
    else if (v > lbl_804D65CC)
    {
        moveZ = fabsf(v) * yScale;
        PlayerResetGunTimer(p);
    }

    if (pos[0] != moveZ)
    {
        if (pos[0] < moveZ)
        {
            pos[0] = dt * zScale + pos[0];
            if (pos[0] > moveZ)
                pos[0] = moveZ;
        }
        if (pos[0] > moveZ)
        {
            pos[0] = pos[0] - dt * zScale;
            if (pos[0] < moveZ)
                pos[0] = moveZ;
        }
    }

    if (pos[1] != moveX)
    {
        if (pos[1] < moveX)
        {
            pos[1] = dt * zScale + pos[1];
            if (pos[1] > moveX)
                pos[1] = moveX;
        }
        if (pos[1] > moveX)
        {
            pos[1] = pos[1] - dt * zScale;
            if (pos[1] < moveX)
                pos[1] = moveX;
        }
    }

    fn_800A0168(&inv, &mtx);
    fn_800A0030((float*)(p + 0x650), &inv, pos);

    int state = *(int*)(p + 0x720);
    if (state == 1 || state == 3)
    {
        if (*(float*)(p + 0x658) > lbl_804D6650)
        {
            *(float*)(p + 0x658) -=
                *(float*)(*(int*)(p + 0x738) + 0x7C) * dt;
            if (*(float*)(p + 0x658) < lbl_804D6650)
                *(float*)(p + 0x658) = lbl_804D6650;
        }
        else
        {
            *(float*)(p + 0x658) = lbl_804D6650;
        }
    }
    else if (state == 2)
    {
        *(float*)(p + 0x658) = lbl_804D65CC;
    }

    if (*(int*)(p + 0x73C) != 4)
        *(u32*)(p + 0x678) |= 0x80;

    *(int*)(p + 0x73C) = 4;
    return 0;
}

int VarFindInt(unsigned long, unsigned long, unsigned long);
extern "C" void fn_800495BC(void*, int, int);

// Starts the player's gun-charge effect once: advances the charge timer, sets
// the charge flag and starts the charge variable (name and fields are guesses;
// offsets are transcribed from retail).
int PlayerStartGunCharge(void* owner)
{
    u8* p = (u8*)owner;

    if (*(int*)(p + 0x634) == 1 || *(int*)(p + 0x720) != 0)
        return 0;

    *(float*)(p + 0x658) =
        *(float*)(p + 0x658) + *(float*)(*(int*)(p + 0x738) + 0x78);
    *(int*)(p + 0x720) = 1;
    *(u32*)(p + 0x7B4) |= 0x8000000;
    *(u32*)(p + 0x7B4) &= ~0x1000u;

    {
        int host = PlayerInHost((Actor*)p);
        int value = VarFindInt(
            0x7982059C, *(u32*)(*(int*)(p + host * 4 + 0x26C4) + 0x15C),
            0x22EC2E);

        fn_800495BC(p, value, 0);
    }
    return 1;
}

// Query payload: the receiver writes its answer over the first word of the
// trailing string (shape taken from the sibling player units; names are
// guesses).
struct PlayerQueryNameMsg
{
    int unk0;
    u32 hash;
    int unk8;
    String result;
};

static inline int PlayerQueryName(int target, int msg, u32 hash)
{
    PlayerQueryNameMsg m;

    m.hash = hash;
    m.unk8 = 0;
    *(int*)&m.result = 0;
    SendMsg(target, msg, &m);
    return *(int*)&m.result;
}

extern "C" float lbl_804D6638;
extern "C" float lbl_804D665C;
extern "C" float lbl_804D6660;
extern "C" float fn_80221100(void*);

// Per-frame update of the player's gun/hand offset: damps the hand velocities
// by the active aim/recoil timers and by the two movement-mode words, runs the
// collision helpers on the hand position and applies the resulting aim sway.
// Message ids, hashes, offsets and constants are transcribed from retail; the
// name and the field meanings are guesses.
int PlayerUpdateGunSway(void* owner, int arg, float dt)
{
    u8* p = (u8*)owner;
    float x = *(float*)(*(int*)(p + 0x738) + 0x10) * *(float*)(p + 0x734);
    float y = *(float*)(*(int*)(p + 0x738) + 0x14) * *(float*)(p + 0x734);
    float z = *(float*)(*(int*)(p + 0x738) + 0x2C) * *(float*)(p + 0x734);
    float zero = lbl_804D65CC;
    int b1 = 0;
    int b2 = 0;
    int q = PlayerQueryName(*(int*)(p + 0x38), 0x41E, 0x2DA8293C);

    if (*(float*)(p + 0x2664) > zero)
    {
        x = x * zero;
        y = y * zero;
        *(float*)(p + 0x2664) = *(float*)(p + 0x2664) - dt;
        if (*(float*)(p + 0x2664) < zero)
            *(float*)(p + 0x2664) = zero;
    }
    else if (*(float*)(p + 0x265C) > zero)
    {
        float k = lbl_804D665C;
        x = x * k;
        y = y * k;
        *(float*)(p + 0x265C) = *(float*)(p + 0x265C) - dt;
        if (*(float*)(p + 0x265C) < zero)
            *(float*)(p + 0x265C) = zero;
    }
    else if (*(float*)(p + 0x2660) > zero)
    {
        float k = lbl_804D6638;
        x = x * k;
        y = y * k;
        *(float*)(p + 0x2660) = *(float*)(p + 0x2660) - dt;
        if (*(float*)(p + 0x2660) < zero)
            *(float*)(p + 0x2660) = zero;
    }

    if (*(int*)(p + 0x9EC) != 0)
    {
        float s = fn_80221100(p);
        x = x * s;
        y = y * s;
    }

    if ((fn_800F7C64((void*)q, 0x8B267329) != 0 || fn_800FC3B8(0x10) != 0) &&
        PlayerInHost((Actor*)p) != 0)
        b1 = 1;

    if (b1)
    {
        float k = lbl_804D662C;
        x = x * k;
        y = y * k;
        z = z * k;
    }

    if (*(u32*)(p + 0x7B4) & 0x200000)
    {
        int mode = *(int*)(p + 0x748);

        if (mode == 1)
        {
            float k = lbl_804D6660;
            x = x * k;
            y = y * k;
            z = z * k;
        }
        else if (mode < 1)
        {
            if (mode >= 0)
            {
                float k = lbl_804D662C;
                x = x * k;
                y = y * k;
                z = z * k;
            }
        }
        else if (mode < 3)
        {
            float k = lbl_804D6628;
            x = x * k;
            y = y * k;
            z = z * k;
        }
    }

    {
        int mode = *(int*)(p + 0x7E0);

        if (mode == 3)
        {
            float k = lbl_804D65F4;
            x = x * k;
            y = y * k;
            z = z * k;
        }
        else if (mode < 3)
        {
            if (mode == 1)
            {
                float k = lbl_804D65F4;
                x = x * k;
                y = y * k;
                z = z * k;
            }
            else if (mode > 0)
            {
                float k = lbl_804D661C;
                x = x * k;
                y = y * k;
                z = z * k;
            }
        }
        else if (mode == 5)
        {
            float k = lbl_804D661C;
            x = x * k;
            y = y * k;
            z = z * k;
        }
        else if (mode < 5)
        {
            float k = lbl_804D65F4;
            x = x * k;
            y = y * k;
            z = z * k;
        }
    }

    if (fn_800F7C64((void*)q, 0x3BFF94C2) != 0 || fn_800FC3B8(0xF) != 0)
        b2 = 1;

    if (*(u32*)(p + 0x7B4) & 0x1000)
    {
        int host = PlayerInHost((Actor*)p);
        int value = VarFindInt(
            0x7982059C, *(u32*)(*(int*)(p + host * 4 + 0x26C4) + 0x15C),
            0xCD35FA9D);

        fn_800495BC(p, value, 0);
        *(u32*)(p + 0x7B4) &= ~0x40000u;
        *(u32*)(p + 0x7B4) &= ~0x8000000u;
        *(u32*)(p + 0x7B4) &= ~0x4000000u;
        *(u32*)(p + 0x7B4) &= ~0x8u;
        *(float*)(p + 0x658) = lbl_804D65CC;
        *(float*)(p + 0x610) = *(float*)(p + 0x614);
        *(float*)(p + 0x618) = lbl_804D65CC;
        return 1;
    }

    {
        Mtx34 mtx;
        Mtx34 rot;
        float vec[3];
        float tx;
        float tz;
        float v;

        PlayerResetGunTimer(p);
        ActorGetMatrix(&mtx, (Actor*)p);
        fn_800A0030(vec, &mtx, (float*)(p + 0x650));

        tz = lbl_804D65CC;
        if (*(float*)(p + 0x394) > lbl_804D65CC)
        {
            tz = fabsf(*(float*)(p + 0x394)) * x;
            PlayerResetGunTimer(p);
        }
        else if (*(float*)(p + 0x394) < lbl_804D65CC)
        {
            tz = -fabsf(*(float*)(p + 0x394)) * x;
            PlayerResetGunTimer(p);
        }

        tx = lbl_804D65CC;
        if (*(float*)(p + 0x38C) < lbl_804D65CC)
        {
            tx = -fabsf(*(float*)(p + 0x38C)) * y;
            PlayerResetGunTimer(p);
        }
        else if (*(float*)(p + 0x38C) > lbl_804D65CC)
        {
            tx = fabsf(*(float*)(p + 0x38C)) * y;
            PlayerResetGunTimer(p);
        }

        if (arg == 0)
        {
            tx = lbl_804D65CC;
            tz = tx;
        }

        v = vec[0];
        if (v != tx)
        {
            if (v < tx)
            {
                v = v + dt * z;
                vec[0] = v;
                if (v > tx)
                    vec[0] = tx;
            }
            v = vec[0];
            if (v > tx)
            {
                v = v - dt * z;
                vec[0] = v;
                if (v < tx)
                    vec[0] = tx;
            }
        }

        v = vec[1];
        if (v != tz)
        {
            if (v < tz)
            {
                v = v + dt * z;
                vec[1] = v;
                if (v > tz)
                    vec[1] = tz;
            }
            v = vec[1];
            if (v > tz)
            {
                v = v - dt * z;
                vec[1] = v;
                if (v < tz)
                    vec[1] = tz;
            }
        }

        fn_800A0168(&rot, &mtx);
        fn_800A0030((float*)(p + 0x650), &rot, vec);
    }

    if (*(u32*)(p + 0x50) & 0x20000000)
    {
        *(float*)(p + 0x27E0) = *(float*)(p + 0x27E0) - dt;

        if (*(float*)(p + 0x27E0) <= lbl_804D65CC ||
            *(int*)(p + 0x964) != 0 || *(int*)(p + 0x960) != 0 ||
            *(int*)(p + 0x9E4) != 0)
        {
            fn_80045EBC(p);
        }
        else
        {
            float t = *(float*)(p + 0x27E0) / *(float*)(p + 0x27DC);
            float a = lbl_804D65F0 * *(float*)(p + 0x27D8);

            fn_800E1098(a * t + a, (float*)(p + 0x650),
                        (float*)(p + 0x27E8));
            *(float*)(p + 0x658) =
                (*(float*)(p + 0x27D8) *
                 (*(float*)(p + 0x27E0) / *(float*)(p + 0x27DC))) /
                *(float*)(p + 0x27E4);
        }
    }

    if (*(float*)(p + 0x658) > lbl_804D6650 &&
        (*(u32*)(p + 0x50) & 0x20000000) == 0)
    {
        if (!b2)
        {
            *(float*)(p + 0x658) =
                *(float*)(p + 0x658) -
                *(float*)(*(int*)(p + 0x738) + 0x7C) * dt;
        }
        else
        {
            *(float*)(p + 0x658) =
                *(float*)(p + 0x658) -
                lbl_804D6638 *
                    (*(float*)(*(int*)(p + 0x738) + 0x7C) * dt);
        }

        if (*(float*)(p + 0x658) < lbl_804D6650)
            *(float*)(p + 0x658) = lbl_804D6650;
    }
    else if ((*(u32*)(p + 0x50) & 0x20000000) == 0)
    {
        *(float*)(p + 0x658) = lbl_804D6650;
    }

    if (*(int*)(p + 0x73C) != 4)
        *(u32*)(p + 0x678) |= 0x80;
    *(int*)(p + 0x73C) = 4;

    return 0;
}

extern "C" float lbl_804D6664;
extern "C" float lbl_804D6668;
extern "C" float lbl_804D6678;

int ActorCount();
Actor* ActorGet(int index);
int Fn800F7DEC(int, int, int, int, void*);

// Looks up two named variables through a 0x41E message, turns their values into
// a 0..1 ratio and applies a per-frame slide to the player's position. The
// names and offsets are guesses transcribed from the retail code.
int PlayerSlideByVar(void* owner)
{
    u8* p = (u8*)owner;
    int nameA;
    int nameB;
    float scale;
    float t;

    {
        String msgName;
        int msg[3];

        msg[1] = 0x2DA8293C;
        msg[2] = 0;
        *(int*)&msgName = 0;
        SendMsg(*(int*)(p + 0x38), 0x41E, msg);
        nameA = *(int*)&msgName;
    }

    {
        String msgName;
        int msg[3];

        msg[1] = 0xDFF3097A;
        msg[2] = 0;
        *(int*)&msgName = 0;
        SendMsg(*(int*)(p + 0x38), 0x41E, msg);
        nameB = *(int*)&msgName;
    }

    scale = lbl_804D65CC;
    t = (float)(u32)Fn800F7DEC(nameA, nameB, (int)0x6FAB23FE,
                              fn_800498D0(p), 0) /
        lbl_804D65F8;
    if (t > scale)
        scale = t;

    if (*(int*)(p + 0x634) == 1)
        return 0;

    if (fn_800F7C64((void*)nameA, 0x3BFF94C2) == 0 && fn_800FC3B8(0xF) == 0)
    {
        float vec[3];
        Mtx34 mtx;

        vec[0] = lbl_804D65CC;
        vec[1] = lbl_804D6664 * scale + lbl_804D6628;
        vec[2] = lbl_804D6668 * scale + lbl_804D662C;
        ActorGetMatrix(&mtx, (Actor*)p);
        fn_800A0168(&mtx, &mtx);
        fn_800A0030(vec, &mtx, vec);
        *(float*)(p + 0x650) = *(float*)(p + 0x650) + vec[0];
        *(float*)(p + 0x654) = *(float*)(p + 0x654) + vec[1];
        *(float*)(p + 0x658) = *(float*)(p + 0x658) + vec[2];
    }

    *(u32*)(p + 0x7B4) |= 0x40000;
    *(u32*)(p + 0x7B4) &= ~0x1000u;

    {
        int host = PlayerInHost((Actor*)p);
        int id = VarFindInt(0x7982059C,
                            *(u32*)(*(int*)(p + host * 4 + 0x26C4) + 0x15C),
                            0x8E17604A);
        fn_800495BC(p, id, 0);
    }

    return 1;
}

// TODO fn_80073E80 (0x198 bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_80073E80(void);

// TODO fn_80074018 (0xD50 bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_80074018(void*, int);

// TODO fn_80074D68 (0x7F8 bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_80074D68(void);

// TODO fn_80075560 (0x384 bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_80075560(float dt, void* owner);

// TODO fn_800758E4 (0x668 bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_800758E4(void);

// Clears the gun-motion flag bits of a player and resets two model offsets
// (name is a guess; offsets and masks are transcribed from retail).
void PlayerResetGunMotionFlags(void* owner)
{
    u8* p = (u8*)owner;
    float zero = lbl_804D65CC;

    *(u32*)(p + 0x7B4) &= ~0x00040000u;
    *(u32*)(p + 0x7B4) &= ~0x08000000u;
    *(u32*)(p + 0x7B4) &= ~0x04000000u;
    *(u32*)(p + 0x7B4) &= ~0x00000008u;
    *(float*)(p + 0x658) = zero;
    *(float*)(p + 0x610) = *(float*)(p + 0x614);
    *(float*)(p + 0x618) = zero;
}

// TODO fn_80075F94 (0xA0C bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_80075F94(float dt, void* owner, float* vec);

// TODO fn_800769A0 (0x6D0 bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_800769A0(void);

// Resets the gun-motion vector and the aim offset of a player
// (name is a guess; offsets are transcribed from retail).
void PlayerResetGunMotion(void* owner)
{
    u8* p = (u8*)owner;

    float zero = 0.0f;

    *(float*)(p + 0x650) = zero;
    *(float*)(p + 0x654) = zero;
    *(float*)(p + 0x658) = zero;
    *(float*)(p + 0x620) = zero;
    *(float*)(p + 0x684) = *(float*)(p + 0x610) -
                           (float)*(int*)(*(int*)(p + 0x40) + 8);
}

// Tells whether the player's gun-motion vector is non-zero
// (name is a guess; offsets are transcribed from retail).
int PlayerGunVecNonZero(void* owner)
{
    u8* p = (u8*)owner;
    float zero = lbl_804D65CC;

    if (zero != *(float*)(p + 0x650) || zero != *(float*)(p + 0x654) ||
        zero != *(float*)(p + 0x658))
        return 1;
    return 0;
}

// TODO fn_800770F8 (0x1E8 bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_800770F8(void);

// Returns the address of the gun-motion vector of a player (name is a guess).
float* PlayerGetGunVec(void* owner)
{
    return (float*)((u8*)owner + 0x650);
}

extern "C" int FxFind(const char* name);
struct GhostFxPair
{
    int speed[2];
};

extern "C" GhostFxPair lbl_804D2990;

// Looks up the two ghost speed effects (names from the retail string pool).
void PlayerInitGhostFx(void)
{
    lbl_804D2990.speed[0] = FxFind("Ghost_Speed_01_L");
    lbl_804D2990.speed[1] = FxFind("Ghost_Speed_02_L");
}

// TODO fn_80077330 (0x3CC bytes): not drafted yet; declared only (an empty body would be inlined into callers) (unit is NonMatching).
extern "C" void fn_80077330(void);

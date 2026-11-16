// u_80175D60.cpp: gun actor callbacks, no-op hooks and the live gun effect records (0x80175D60-0x80176044).
// The gun slot / aim behaviour code that follows is u_80176044.cpp (its own 1000.0f and 0.0f).
// Names are descriptive guesses; the code is matched against the retail DOL.

void TrailDestroy(int id);

// Name is a guess: returns 0 when the top byte of an id is 2 (engine-side subtype?
// network message class?), 1 otherwise.
unsigned long Fn80175D60(unsigned long id) {
    switch (id >> 24) {
    case 2:
        return 0;
    }
    return 1;
}

// Placeholder name kept: called from other units. Empty body in retail (just blr).
extern "C" void fn_80175D80(void) {
}

// No-op callback, retail body is a bare `blr`; address 0x80175D84.
extern "C" void fn_80175D84(void) {}

// No-op helper, retail body is a bare `blr`; address 0x80175D88.
void GunNoOp80175D88(void) {}

// Name is a guess: empty helper, retail body is a bare `blr`; address 0x80175D8C.
// fn_80175D8C
extern "C" void fn_80175D8C(void) {}

// Name is a guess: empty helper, retail body is a bare `blr`; address 0x80175D90.
// fn_80175D90
extern "C" void fn_80175D90(void) {}

// Name is a guess: empty helper, retail body is a bare `blr`; address 0x80175D94.
void GunNoOp80175D94(void) {}

// Name is a guess: empty helper, retail body is a bare `blr`; address 0x80175D98.
void GunNoOp80175D98(void) {}

// Name is a guess: empty helper, retail body is a bare `blr`; address 0x80175D9C.
void GunNoOp80175D9C(void) {}

// Name is a guess: empty helper, retail body is a bare `blr`; address 0x80175DA0.
void GunNoOp80175DA0(void) {}

// Name is a guess: empty helper, retail body is a bare `blr`; address 0x80175DA4.
void GunNoOp80175DA4(void) {}

// Name is a guess (fn_80175DA8 is called from GunWithGrenade.cpp, so the placeholder
// name is kept): releases a trail handle held in a caller-supplied id slot; the id is
// reset to -1 so the release happens only once.
extern "C" void fn_80175DA8(void* p) {
    int* id = (int*)p;
    if (*id != -1) {
        TrailDestroy(*id);
        *id = -1;
    }
}

// A trail handle plus the seconds of life left in it (8 bytes); names are guesses.
struct MuzzleFlash {
    int trail;   // 0x00
    float time;  // 0x04
};

extern "C" unsigned long fn_800397F8(const char* name);
extern "C" int fn_801DE7DC(int actor, unsigned long effect);

// Placeholder name kept: called from GunWithGrenade.cpp. Counts the flash's
// remaining lifetime down by dt and releases the trail once it runs out.
extern "C" void fn_80175DE8(void* p, float dt) {
    MuzzleFlash* flash = (MuzzleFlash*)p;
    if (flash->trail != -1) {
        if (flash->time < dt) {
            TrailDestroy(flash->trail);
            flash->trail = -1;
        } else {
            flash->time -= dt;
        }
    }
}

// Placeholder name kept: called from GunWithGrenade.cpp. Starts the muzzle flash
// trail on the actor (the hit position and hit data are unused here) and reloads
// its lifetime.
extern "C" void fn_80175E40(void* p, float* pos, void* hit, int actor) {
    MuzzleFlash* flash = (MuzzleFlash*)p;
    if (flash->trail == -1) {
        flash->trail = fn_801DE7DC(actor, fn_800397F8("MUZZLEFLASH"));
    }
    flash->time = 1000.0f;
}

// Name is a guess (fn_80175EA4 keeps its placeholder name): releases a trail handle
// held in a caller-supplied id slot and resets the slot to -1.
extern "C" void fn_80175EA4(int* id) {
    if (*id != -1) {
        TrailDestroy(*id);
        *id = -1;
    }
}

// Name is a guess (fn_80175EE4 keeps its placeholder name): resets a muzzle flash
// to "no trail" and loads its default lifetime.
extern "C" void fn_80175EE4(MuzzleFlash* flash) {
    flash->trail = -1;
    flash->time = 0.0f;
}

// Placeholder name kept: called from other units (GunWithGrenade.cpp and others).
// Empty body in retail, a bare `blr`.
extern "C" void fn_80175EF8(void) {}

// 3x3 rotation followed by a translation vector; 0x30 bytes. Used for the actor
// matrix copied into a slot record (field names are guesses).
struct Mtx34 {
    float rot[3][3];   // 0x00
    float trans[3];    // 0x24
};

// One record per gun/actor slot (names and purposes are guesses, the stride of
// 0xe8 and every offset come from retail).
struct GunActiveRecord {
    unsigned char unk00[0x24];          // 0x00
    float field24;                      // 0x24
    float field28;                      // 0x28
    float field2C;                      // 0x2c
    int field30;                        // 0x30
    float field34;                      // 0x34
    unsigned char unk38[0x40 - 0x38];   // 0x38
    float field40;                      // 0x40
    float field44;                      // 0x44
    float field48;                      // 0x48
    int field4C;                        // 0x4c
    int field50;                        // 0x50
    int field54;                        // 0x54
    unsigned char unk58[0x60 - 0x58];   // 0x58
    Mtx34 matrix;                       // 0x60
    float yaw;                          // 0x90
    float pitch;                        // 0x94
    float dist;                         // 0x98
    unsigned char unk9C[0xa0 - 0x9c];   // 0x9c
    float aimYaw;                       // 0xa0
    float aimPitch;                     // 0xa4
    float aimElev;                      // 0xa8
    unsigned char unkAC[0xb0 - 0xac];   // 0xac
    float rateYaw;                      // 0xb0
    float ratePitch;                    // 0xb4
    float rateElev;                     // 0xb8
    unsigned char unkBC[0xc0 - 0xbc];   // 0xbc
    int to;                             // 0xc0
    unsigned long handle;               // 0xc4
    unsigned long value;                // 0xc8
    float fieldCC;                      // 0xcc
    int fieldD0;                        // 0xd0
    int fieldD4;                        // 0xd4
    unsigned char* fieldD8;             // 0xd8
    float fieldDC;                      // 0xdc
    float fieldE0;                      // 0xe0
    float fieldE4;                      // 0xe4
};

unsigned long lbl_804D5040[2];    // 2-word live-id bitmap (.sbss)
GunActiveRecord lbl_80491020[8];

extern "C" void fn_800C6900(unsigned long handle);
extern "C" void fn_800C6664(unsigned long handle);
void BitClear(unsigned long* bits, int id);

// Placeholder name kept: called from other units. Returns the record field for a
// live id, or 0 when the id's bit is clear in the global live-id bitmap.
extern "C" unsigned long fn_80175EFC(unsigned long id) {
    int index = id & 0xffffff;
    if ((lbl_804D5040[index / 32] & (1 << (id & 0x1f))) != 0) {
        return lbl_80491020[index].value;
    }
    return 0;
}

// Releases the effect handle held in a live id's record and clears the id's bit in
// the global live-id bitmap. Name is a guess.
void GunDestroyLiveEffect(unsigned long id) {
    int index = id & 0xffffff;
    if ((lbl_804D5040[index / 32] & (1 << (id & 0x1f))) != 0) {
        BitClear(lbl_804D5040, index);
        fn_800C6900(lbl_80491020[index].handle);
    }
}

// Releases the effect handle of one of the first eight ids. Name is a guess.
void GunDestroyEffectSlot(unsigned long id) {
    int index = id & 0xffffff;
    if (index >= 0 && index < 8) {
        fn_800C6664(lbl_80491020[index].handle);
    }
}

extern "C" void fn_800C6760(unsigned long handle);

// Placeholder name kept: called from other units. Releases the effect handle of one
// of the first eight ids with a different release routine than GunDestroyEffectSlot.
extern "C" void fn_80176000(unsigned long id) {
    int index = id & 0xffffff;
    if (index >= 0 && index < 8) {
        fn_800C6760(lbl_80491020[index].handle);
    }
}

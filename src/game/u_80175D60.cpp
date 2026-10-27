// u_80175D60.cpp: gun actor callbacks, no-op hooks and gun state helpers (0x80175D60-0x8017BE38).
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

extern "C" unsigned long Fn800397F8(const char* name);
extern "C" int Fn801DE7DC(int actor, unsigned long effect);
extern char lbl_80346EE0[];
extern float lbl_804D80E0;
extern float lbl_804D80E4;

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
        flash->trail = Fn801DE7DC(actor, Fn800397F8(lbl_80346EE0));
    }
    flash->time = lbl_804D80E0;
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

extern unsigned long lbl_804D5040[2];    // 2-word live-id bitmap (.sbss)
extern GunActiveRecord lbl_80491020[];

extern "C" void fn_800C6900(unsigned long handle);
extern "C" void fn_800C6664(unsigned long handle);
void BitClear(unsigned long* bits, unsigned long id);

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

struct Actor;

unsigned long BitsetFirstClear(unsigned long* bits, int start, int count);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int fn_8001BAA8(int flag);
extern "C" int fn_800C6AE8(void* record, void (*callback)(void*, float), int flags,
                           void* user);
extern "C" float fn_800A1128(float y, float x);
extern "C" float fn_800A109C(float z);
extern "C" float fn_800A1418(float angle);
extern "C" float fn_800A1394(float angle);
void GunSlotUpdate(void* record, float dt);
void GunRefreshAimBasis(void* record);
extern "C" float sqrtf(float x);
extern "C" float fabsf(float x);
extern "C" void fn_800E1098(float* out, const float* in, float scale);
extern "C" void fn_800A02C0(void* record, float* row, int index);

void ActorGetMatrix(Mtx34* out, Actor* actor);

extern float lbl_804D80E8;
extern float lbl_804D80EC;
extern float lbl_804D80F0;
extern float lbl_804D80F4;
extern float lbl_804D80F8;
extern float lbl_804D80FC;
extern float lbl_804D8100;
extern float lbl_804D8104;
extern float lbl_804D8108;
extern float lbl_804D810C;
extern float lbl_804D8110;
extern float lbl_804D8114;
extern float lbl_804D8118;
extern float lbl_804D811C;
extern float lbl_804D8120;

// Placeholder name kept: called from other units. Allocates one of the first eight
// gun slots, fills its record from the actor, position and angle arguments, works
// out the aim angles towards dir and starts the slot's per-frame update. Returns
// the new handle (id with the 0x18 type tag) or -1 when no slot is free.
extern "C" unsigned long fn_80176044(float radius, float height, float angle,
                                     void* actor, int to, const float* dir,
                                     int arg7, int arg8) {
    unsigned long data[8];
    unsigned char pathBuffer[0x58];
    float nx;
    float ny;
    float nz;
    float dist;

    int id = BitsetFirstClear(lbl_804D5040, 1, 8);
    if (id == -1) {
        return 0xffffffff;
    }
    GunActiveRecord* rec = &lbl_80491020[id];
    int handle = fn_800C6AE8(rec, GunSlotUpdate, 0, actor);
    if (handle == -1) {
        return 0xffffffff;
    }
    lbl_804D5040[id / 32] |= 1 << (id & 0x1f);
    fn_8001BAA8(0);
    rec->to = to;
    rec->handle = handle;
    rec->value = 0;
    rec->fieldCC = lbl_804D80E8;
    rec->fieldE4 = radius;
    rec->fieldD0 = arg7;
    rec->fieldD4 = arg8;
    SendMsg(rec->to, 0x400, data);
    ActorGetMatrix(&rec->matrix, *(Actor**)data);
    rec->fieldE0 = height;
    rec->matrix.trans[2] += rec->fieldE0;

    float dx = dir[0] - rec->matrix.trans[0];
    float dy = dir[1] - rec->matrix.trans[1];
    float dz = dir[2] - rec->matrix.trans[2];
    if (((fabsf(dx) > lbl_804D80E8) + (fabsf(dy) > lbl_804D80E8) +
         (fabsf(dz) > lbl_804D80E8)) == 0) {
        nx = lbl_804D80E8;
        ny = lbl_804D80E8;
        nz = lbl_804D80E8;
        dist = lbl_804D80E8;
    } else {
        dist = sqrtf(dx * dx + dy * dy + dz * dz);
        nx = dx / dist;
        ny = dy / dist;
        nz = dz / dist;
    }
    rec->dist = dist;
    rec->yaw = fn_800A1128(ny, nx);
    rec->pitch = lbl_804D80F0 - fn_800A109C(nz);

    float aimYaw = rec->yaw + lbl_804D80F4;
    float aimPitch = rec->pitch + lbl_804D80E8;
    while (aimYaw > lbl_804D80F4) {
        aimYaw -= lbl_804D80F8;
    }
    while (aimPitch > lbl_804D80F4) {
        aimPitch -= lbl_804D80F8;
    }
    while (aimYaw < lbl_804D80FC) {
        aimYaw += lbl_804D80F8;
    }
    while (aimPitch < lbl_804D80FC) {
        aimPitch += lbl_804D80F8;
    }
    rec->aimYaw = aimYaw;
    rec->aimPitch = lbl_804D8100;
    rec->aimElev = angle;

    float diff = aimYaw - rec->yaw;
    float mag = fabsf(diff);
    float scale = (diff < lbl_804D80E8) ? lbl_804D8104 : lbl_804D8108;
    if (mag < lbl_804D80F4) {
        scale = scale * lbl_804D8104 * lbl_804D810C *
                ((lbl_804D80F8 - mag) / rec->fieldE4);
    } else {
        scale = scale * lbl_804D810C * (mag / rec->fieldE4);
    }
    rec->rateYaw = scale;

    diff = rec->aimPitch - rec->pitch;
    mag = fabsf(diff);
    scale = (diff < lbl_804D80E8) ? lbl_804D8104 : lbl_804D8108;
    if (mag > lbl_804D80F4) {
        scale = scale * lbl_804D8104 * lbl_804D810C *
                ((lbl_804D80F8 - mag) / rec->fieldE4);
    } else {
        scale = scale * lbl_804D810C * (mag / rec->fieldE4);
    }
    rec->ratePitch = lbl_804D8110 * scale;

    diff = rec->aimElev - rec->dist;
    mag = fabsf(diff);
    scale = (diff < lbl_804D80E8) ? lbl_804D8104 : lbl_804D8108;
    rec->rateElev = scale * lbl_804D810C * lbl_804D8110 * (mag / rec->fieldE4);

    float pitchAngle =
        lbl_804D8114 * rec->ratePitch / lbl_804D810C + rec->pitch;
    float yawAngle = lbl_804D8114 * rec->rateYaw / lbl_804D810C + rec->yaw;
    fn_800A1418(pitchAngle);
    fn_800A1418(yawAngle);
    fn_800A1418(pitchAngle);
    fn_800A1394(yawAngle);
    fn_800A1394(pitchAngle);
    rec->fieldD8 = pathBuffer;
    rec->fieldDC = rec->dist;
    GunRefreshAimBasis(rec);
    return (id & 0xffffff) | 0x18000000;
}

extern "C" int fn_80026660(int timer, void (*callback)(void), int arg);

int GunFillSlotInfo(unsigned long id, int msg, void* data);

// Name is a guess: hooks the per-frame update (GunFillSlotInfo) onto timer slot
// 0x18 and clears the live-id bitmap. The timer passes no arguments.
extern "C" void fn_801764F0(void) {
    fn_80026660(0x18, (void (*)(void))GunFillSlotInfo, 0);
    lbl_804D5040[0] = 0;
}

// Name is a guess: per-frame update for one gun slot. Steers the slot's aim angles
// towards the target angles at a rate that slows down near the target and past the
// slot's lifetime, wraps the pitch/yaw into range, then rebuilds the aim angles and
// hands the record to the path update.
void GunSlotUpdate(void* p, float dt) {
    GunActiveRecord* rec = (GunActiveRecord*)p;
    unsigned char pathBuffer[0x58];

    if (rec->value != 0) {
        return;
    }

    float diff = rec->aimPitch - rec->pitch;
    float mag = fabsf(diff);
    float scale = (diff < lbl_804D80E8) ? lbl_804D8104 : lbl_804D8108;
    float range = rec->fieldE4 - rec->fieldCC;
    if (mag <= lbl_804D80F4) {
        scale = scale * (lbl_804D810C * (mag / range));
    } else {
        scale = scale * lbl_804D8104 *
                (lbl_804D810C * ((lbl_804D80F8 - mag) / range));
    }
    rec->ratePitch = lbl_804D8110 * scale;

    if (rec->fieldDC != rec->dist) {
        float d = rec->fieldDC - rec->dist;
        float m = fabsf(d);
        float s = (d < lbl_804D80E8) ? lbl_804D8104 : lbl_804D8108;
        rec->rateElev = lbl_804D810C * (s * (m / (lbl_804D8118 * dt)));
    } else {
        float d = rec->aimElev - rec->dist;
        float m = fabsf(d);
        float s = (d < lbl_804D80E8) ? lbl_804D8104 : lbl_804D8108;
        float r = rec->fieldE4 - rec->fieldCC;
        rec->rateElev = lbl_804D8110 * (lbl_804D810C * (s * (m / r)));
    }

    rec->fieldCC += dt;

    float deltaElev = rec->rateElev * dt / lbl_804D810C;
    float deltaPitch = rec->ratePitch * dt / lbl_804D810C;
    float deltaYaw = rec->rateYaw * dt / lbl_804D810C;
    float newYaw = rec->yaw + deltaYaw;
    float newPitch = rec->pitch + deltaPitch;
    float newDist = rec->dist + deltaElev;

    while (newYaw > lbl_804D80F4) {
        newYaw -= lbl_804D80F8;
    }
    while (newPitch > lbl_804D80F4) {
        newPitch -= lbl_804D80F8;
    }
    while (newYaw < lbl_804D80FC) {
        newYaw += lbl_804D80F8;
    }
    while (newPitch < lbl_804D80FC) {
        newPitch += lbl_804D80F8;
    }

    if (fabsf(rec->aimYaw - newYaw) <= fabsf(deltaYaw)) {
        rec->rateYaw = lbl_804D80E8;
        rec->value = 1;
        fn_8001BAA8(1);
        newYaw = rec->aimYaw;
    }
    if (rec->fieldCC > lbl_804D811C) {
        rec->rateYaw = lbl_804D80E8;
        rec->value = 1;
        fn_8001BAA8(1);
    }
    if (fabsf(rec->aimPitch - newPitch) < fabsf(deltaPitch)) {
        rec->ratePitch = lbl_804D80E8;
        newPitch = rec->aimPitch;
    }
    if (fabsf(rec->aimElev - newDist) < fabsf(deltaElev)) {
        newDist = rec->aimElev;
    }

    rec->yaw = newYaw;
    rec->pitch = newPitch;
    rec->dist = newDist;

    float pitchAngle = rec->ratePitch * dt / lbl_804D810C + rec->pitch;
    float yawAngle = rec->rateYaw * dt / lbl_804D810C + rec->yaw;
    fn_800A1418(pitchAngle);
    fn_800A1418(yawAngle);
    fn_800A1418(pitchAngle);
    fn_800A1394(yawAngle);
    fn_800A1394(pitchAngle);
    rec->fieldD8 = pathBuffer;
    rec->fieldDC = rec->dist;
    GunRefreshAimBasis(rec);
    rec->field34 = lbl_804D8120;
}

// Reply buffer of the "slot info" message (0x1c01): the caller sets `flags` to
// choose which record fields it wants back (field purposes are guesses).
struct GunSlotInfo {
    unsigned char unk00[4];   // 0x00
    unsigned long flags;      // 0x04
    int field08;              // 0x08
    float field0C;            // 0x0c
    float field10;            // 0x10
    float field14;            // 0x14
    float field18;            // 0x18
    int field1C;              // 0x1c
    int field20;              // 0x20
    int field24;              // 0x24
    unsigned long field28;    // 0x28
};

// Name is a guess: message handler for the "slot info" query. Copies the fields
// of the gun slot named by `id` that the request's flag mask asks for into the
// reply. Returns 1 when the message was handled, 0 for any other message.
int GunFillSlotInfo(unsigned long id, int msg, void* data) {
    GunSlotInfo* info = (GunSlotInfo*)data;

    if (msg != 0x1c01) {
        return 0;
    }
    if ((info->flags & 1) != 0) {
        info->field08 = lbl_80491020[id & 0xffffff].field30;
    }
    if ((info->flags & 2) != 0) {
        info->field0C = lbl_80491020[id & 0xffffff].field34;
    }
    if ((info->flags & 4) != 0) {
        info->field10 = lbl_80491020[id & 0xffffff].field40;
    }
    if ((info->flags & 8) != 0) {
        info->field14 = lbl_80491020[id & 0xffffff].field44;
    }
    if ((info->flags & 0x10) != 0) {
        info->field18 = lbl_80491020[id & 0xffffff].field48;
    }
    if ((info->flags & 0x20) != 0) {
        info->field1C = lbl_80491020[id & 0xffffff].field4C;
        info->field20 = lbl_80491020[id & 0xffffff].field50;
        info->field24 = lbl_80491020[id & 0xffffff].field54;
    }
    if ((info->flags & 0x40) != 0) {
        info->field28 = lbl_80491020[id & 0xffffff].handle;
    }
    return 1;
}

// Three floats of a direction/basis vector (names are guesses).
struct Vec3 {
    float x;   // 0x00
    float y;   // 0x04
    float z;   // 0x08
};

// Scales a 3-vector to unit length; a vector shorter than the epsilon is left at
// zero, which the callers read as a degenerate direction.
static inline void GunNormalizeVec(Vec3* v) {
    if (((fabsf(v->x) > lbl_804D80EC) + (fabsf(v->y) > lbl_804D80EC) +
         (fabsf(v->z) > lbl_804D80EC)) != 0) {
        float inv = 1.0f / sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
        v->x *= inv;
        v->y *= inv;
        v->z *= inv;
    } else {
        v->x = lbl_804D80E8;
        v->y = lbl_804D80E8;
        v->z = lbl_804D80E8;
    }
}

// Name is a guess: rebuilds the slot's aim matrix from its yaw, pitch and
// distance. The aim point is the actor matrix translation plus the offset those
// angles give; the basis rows are the offset scaled by -1, its cross product
// with the world Z axis, and the cross product of those two, each normalised.
// The rows are handed to fn_800A02C0, which stores them in the actor.
void GunRefreshAimBasis(void* p) {
    GunActiveRecord* rec = (GunActiveRecord*)p;
    Vec3 offset;
    Vec3 dir;
    Vec3 side;
    Vec3 up;
    float worldAxis[3];

    offset.x = rec->dist * fn_800A1418(rec->yaw) * fn_800A1418(rec->pitch);
    offset.y = rec->dist * fn_800A1394(rec->yaw) * fn_800A1418(rec->pitch);
    offset.z = rec->dist * fn_800A1394(rec->pitch);

    rec->field24 = rec->matrix.trans[0] + offset.x;
    rec->field28 = rec->matrix.trans[1] + offset.y;
    rec->field2C = rec->matrix.trans[2] + offset.z;

    fn_800E1098(&dir.x, &offset.x, lbl_804D8104);

    worldAxis[0] = lbl_804D80E8;
    worldAxis[1] = lbl_804D80E8;
    worldAxis[2] = lbl_804D8108;

    side.x = dir.y * worldAxis[2] - dir.z * worldAxis[1];
    side.y = dir.z * worldAxis[0] - dir.x * worldAxis[2];
    side.z = dir.x * worldAxis[1] - dir.y * worldAxis[0];
    GunNormalizeVec(&side);

    up.x = side.y * dir.z - side.z * dir.y;
    up.y = side.z * dir.x - side.x * dir.z;
    up.z = side.x * dir.y - side.y * dir.x;
    GunNormalizeVec(&up);

    fn_800A02C0(rec, &side.x, 0);
    fn_800A02C0(rec, &dir.x, 1);
    fn_800A02C0(rec, &up.x, 2);
}

// Flags block reached through the behaviour's +0x54 pointer (field names are guesses).
struct GunFlagBlock80176CF4 {
    int field00;         // 0x00
    int field04;         // 0x04
    unsigned int flags;  // 0x08
};

// The behaviour object these callbacks receive; only its +0x54 pointer is used.
struct GunFlagOwner80176CF4 {
    unsigned char unk00[0x54];    // 0x00
    GunFlagBlock80176CF4* flags;  // 0x54
};

// Name is a guess: clears the +0x04 counter once the flag block reports both
// 0x20 and 0x10. Two behaviour tables hold the same body.
void GunFlagStateClear80176CF4(void* owner, void* beh) {
    GunFlagOwner80176CF4* o = (GunFlagOwner80176CF4*)beh;
    GunFlagBlock80176CF4* b;
    if (o == 0) {
        return;
    }
    b = o->flags;
    if ((b->flags & 0x20) == 0) {
        return;
    }
    if ((b->flags & 0x10) == 0) {
        return;
    }
    b->field04 = 0;
}

// Name is a guess: same body as GunFlagStateClear80176CF4, used by another
// behaviour table (address 0x80176D20).
void GunFlagStateClear80176D20(void* owner, void* beh) {
    GunFlagOwner80176CF4* o = (GunFlagOwner80176CF4*)beh;
    GunFlagBlock80176CF4* b;
    if (o == 0) {
        return;
    }
    b = o->flags;
    if ((b->flags & 0x20) == 0) {
        return;
    }
    if ((b->flags & 0x10) == 0) {
        return;
    }
    b->field04 = 0;
}

// State callbacks of the gun behaviour tables (names are guesses). The first
// argument is the owner, the second the behaviour holding the flag block.
struct Owner;
struct Beh;

// Owner-side fields handed to the message send (purposes are guesses).
struct GunAimOwner80176D4C {
    unsigned char unk00[0x8ec];  // 0x00
    int field8EC;                // 0x8ec
    int field8F0;                // 0x8f0
};

int OwnerId(Owner* owner);
int Fn800F7DEC(int a, int b, int c, int d, void* data);
void BehUpdate(Owner* owner, Beh* beh);
void Fn8016295C(Owner* owner, Beh* beh);

// Name is a guess: asks the owner whether the behaviour is still wanted and
// advances (or clears) the flag block's state counter from the reply.
void GunAimStateSwitch80176D4C(Owner* owner, Beh* beh) {
    GunAimOwner80176D4C* o = (GunAimOwner80176D4C*)owner;
    GunFlagBlock80176CF4* b;
    int result;

    if (beh == 0) {
        return;
    }
    b = ((GunFlagOwner80176CF4*)beh)->flags;
    if ((b->flags & 1) == 0) {
        return;
    }
    if ((b->flags & 0x10) == 0) {
        return;
    }
    result = Fn800F7DEC(o->field8EC, o->field8F0, 0xf829db93, OwnerId(owner), 0);
    switch (result) {
    case 0xc0becf7e:
        b->field04 = 7;
        break;
    case 0x04a3e183:
        b->field04 = 0;
        break;
    case 0x3da260f7:
        break;
    case 0x11d2c025:
        break;
    }
}

// Name is a guess: updates the behaviour, then asks the owner whether it is
// still wanted and moves the flag block to the next state from the reply.
void GunAimStateUpdate80176E20(Owner* owner, Beh* beh) {
    GunAimOwner80176D4C* o = (GunAimOwner80176D4C*)owner;
    GunFlagBlock80176CF4* b;
    int result;

    if (beh == 0) {
        return;
    }
    b = ((GunFlagOwner80176CF4*)beh)->flags;
    BehUpdate(owner, beh);
    Fn8016295C(owner, beh);
    if ((b->flags & 1) == 0) {
        return;
    }
    if ((b->flags & 0x10) == 0) {
        return;
    }
    result = Fn800F7DEC(o->field8EC, o->field8F0, 0x20ee36, OwnerId(owner), 0);
    switch (result) {
    case 0xc0bfcf7e:
        b->field04 = 6;
        break;
    case 0x04a3e183:
        b->field04 = 0;
        break;
    case 0x3da260f7:
        break;
    case 0x11d2c025:
        break;
    }
}

// Actor that owns a follow target and the world position the aim is measured
// from (offsets come from retail; names are guesses).
struct GunAimActor80176F14 {
    unsigned char unk00[0x24];          // 0x00
    int posX;                           // 0x24
    int posY;                           // 0x28
    int posZ;                           // 0x2c
    unsigned char unk30[0x270 - 0x30];  // 0x30
    int follow;                         // 0x270
};

// Follow target: its position and the id copied into the behaviour.
struct GunAimTarget80176F14 {
    unsigned char unk00[0x24];          // 0x00
    int posX;                           // 0x24
    int posY;                           // 0x28
    unsigned char unk2C[0x38 - 0x2c];   // 0x2c
    int field38;                        // 0x38
};

// Aim behaviour: view limits at 0x14/0x16 and the aim flags at 0x2c.
struct GunAimBeh80176F14 {
    unsigned char unk00[0x14];          // 0x00
    unsigned short field14;             // 0x14
    unsigned short field16;             // 0x16
    unsigned char unk18[0x24 - 0x18];   // 0x18
    int field24;                        // 0x24
    int field28;                        // 0x28
    unsigned int flags;                 // 0x2c
    int field30;                        // 0x30
    int field34;                        // 0x34
};

void BehDone(void* beh);
void BehReset(void* beh);
void* FollowGetTargetObj(void* follow);
void fn_800A0168(Mtx34* out, const Mtx34* in);
void fn_800A0030(Vec3* out, const Mtx34* m, const Vec3* v);
float Fn800AA050(void* actor, void* target);

extern float lbl_804D8128;
extern float lbl_804D812C;
extern float lbl_804D8130;
extern float lbl_804D8134;
extern float lbl_804D8140;

// Scales a 3-vector to unit length; a vector shorter than the epsilon is set to
// zero, which the caller reads as a degenerate direction.
static void GunNormalizeAimVec80176F14(Vec3* v) {
    if (((fabsf(v->x) > lbl_804D8130) + (fabsf(v->y) > lbl_804D8130) +
         (fabsf(v->z) > lbl_804D8130)) != 0) {
        float inv = 1.0f / sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
        v->x *= inv;
        v->y *= inv;
        v->z *= inv;
    } else {
        v->x = lbl_804D8128;
        v->y = lbl_804D8128;
        v->z = lbl_804D8128;
    }
}

// Name is a guess: aim state update. Ends and restarts the behaviour, picks up
// its follow target (the behaviour is dropped when there is none), copies the
// target id, then works out whether the target is in front of the actor within
// the behaviour's view limits and sets the aim flags from that.
void GunAimTargetUpdate80176F14(void* ownerArg, void* behArg) {
    GunAimActor80176F14* owner = (GunAimActor80176F14*)ownerArg;
    GunAimBeh80176F14* beh;
    GunAimTarget80176F14* target;
    Vec3 delta;
    Vec3 axis;
    Mtx34 mtx;
    float angle;

    if (behArg == 0) {
        return;
    }
    beh = (GunAimBeh80176F14*)behArg;
    BehDone(beh);
    BehReset(beh);
    target = (GunAimTarget80176F14*)FollowGetTargetObj(&owner->follow);
    if (target == 0) {
        return;
    }
    beh->field30 = 2;
    beh->field34 = target->field38;

    delta.x = (float)target->posX - (float)owner->posX;
    delta.y = (float)target->posY - (float)owner->posY;
    delta.z = (float)owner->posZ - (float)owner->posZ;

    axis.x = lbl_804D8128;
    axis.y = lbl_804D812C;
    axis.z = lbl_804D8128;
    ActorGetMatrix(&mtx, (Actor*)owner);
    fn_800A0168(&mtx, &mtx);
    fn_800A0030(&axis, &mtx, &axis);

    GunNormalizeAimVec80176F14(&axis);
    GunNormalizeAimVec80176F14(&delta);

    if (axis.z * delta.z + axis.x * delta.x + axis.y * delta.y <=
        lbl_804D8134) {
        beh->flags |= 0x100;
    } else {
        angle = Fn800AA050(owner, target);
        if (angle <= (float)beh->field16 - lbl_804D8140 &&
            (float)beh->field14 - lbl_804D8140 <= angle) {
            beh->flags |= 1;
            beh->flags |= 4;
        }
        if ((beh->flags & 4) == 0 && (beh->flags & 8) == 0) {
            beh->flags |= 0x80;
        }
    }
}

// Name is a guess: empty state callback, retail body is a bare `blr`; address
// 0x801772C4. It sits next to the other gun-aim state callbacks.
void GunAimNoOp801772C4(void*, void*) {}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x801772C8.
void GunAimNoOp801772C8(void*, void*) {}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x801772CC.
void GunAimNoOp801772CC(void*, void*) {}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x801772D0.
void GunAimNoOp801772D0(void*, void*) {}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x801772D4.
void GunAimNoOp801772D4(void*, void*) {}

// Placeholder name kept: called from a behaviour table. Clears the flag block's
// +0x04 state counter and ends the behaviour (address 0x801772D8).
extern "C" void fn_801772D8(void* owner, void* beh) {
    GunFlagOwner80176CF4* o = (GunFlagOwner80176CF4*)beh;
    GunFlagBlock80176CF4* b;
    if (o == 0) {
        return;
    }
    b = o->flags;
    b->flags = 0;
    BehDone(beh);
}

// Placeholder name kept: called from a behaviour table. Same body as
// fn_801772D8, used by a second behaviour table (address 0x80177310).
extern "C" void fn_80177310(void* owner, void* beh) {
    GunFlagOwner80176CF4* o = (GunFlagOwner80176CF4*)beh;
    GunFlagBlock80176CF4* b;
    if (o == 0) {
        return;
    }
    b = o->flags;
    b->flags = 0;
    BehDone(beh);
}

// Placeholder name kept: called from a behaviour table. Same body as
// fn_801772D8, used by a third behaviour table (address 0x80177348).
extern "C" void fn_80177348(void* owner, void* beh) {
    GunFlagOwner80176CF4* o = (GunFlagOwner80176CF4*)beh;
    GunFlagBlock80176CF4* b;
    if (o == 0) {
        return;
    }
    b = o->flags;
    b->flags = 0;
    BehDone(beh);
}

// Placeholder name kept: called from a behaviour table. Same body as
// fn_801772D8, used by a fourth behaviour table (address 0x80177380).
extern "C" void fn_80177380(void* owner, void* beh) {
    GunFlagOwner80176CF4* o = (GunFlagOwner80176CF4*)beh;
    GunFlagBlock80176CF4* b;
    if (o == 0) {
        return;
    }
    b = o->flags;
    b->flags = 0;
    BehDone(beh);
}

// Aim behaviour of the state callbacks below: its aim flags at +0x2c and the
// flag block pointer at +0x54 (field names are guesses).
struct GunAimBeh801773B8 {
    unsigned char unk00[0x2c];         // 0x00
    unsigned int flags;                // 0x2c
    unsigned char unk30[0x54 - 0x30];  // 0x30
    GunFlagBlock80176CF4* block;       // 0x54
};

// Name is a guess: ends the behaviour's current state, then asks the owner
// whether the aim should stop; on the 0x04a3e183 reply the behaviour's aim
// flags are marked done (address 0x801773B8).
void GunAimStateDone801773B8(void* ownerArg, void* behArg) {
    GunAimOwner80176D4C* owner;
    GunAimBeh801773B8* beh;
    int result;

    if (behArg == 0) {
        return;
    }
    owner = (GunAimOwner80176D4C*)ownerArg;
    beh = (GunAimBeh801773B8*)behArg;
    beh->block->flags = 0;
    BehDone(behArg);
    result = Fn800F7DEC(owner->field8EC, owner->field8F0, 0xe6b71bcc,
                        OwnerId((Owner*)ownerArg), 0);
    if (result == 0x04a3e183) {
        beh->flags |= 0x10;
    }
}

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Empty behaviour-table callback that always answers 0, retail is `li r3,0; blr`
// (address 0x80177444).
extern "C" int fn_80177444(void*, void*) {
    return 0;
}

// Owner-side fields used by the weapon message handlers below (offsets from
// retail; names are guesses).
struct GunMsgOwner8017744C {
    unsigned char unk00[0x138];           // 0x00
    float field138;                       // 0x138
    unsigned char unk13C[0x8ec - 0x13c];  // 0x13c
    int field8EC;                         // 0x8ec
    int field8F0;                         // 0x8f0
};

// Per-behaviour state block of the handlers (field names are guesses).
struct GunMsgBlock8017744C {
    int state;           // 0x00 current state
    int next;            // 0x04 requested state
    unsigned int flags;  // 0x08
    float time;          // 0x0c
};

// The behaviour object; +0x10 receives the new state, +0x54 holds the block.
struct GunMsgBeh8017744C {
    unsigned char unk00[0x10];            // 0x00
    int field10;                          // 0x10
    unsigned char unk14[0x54 - 0x14];     // 0x14
    GunMsgBlock8017744C* block;           // 0x54
};

// Reply/parameter buffer filled by Fn80161FF4 (retail stack slot is 0x6c).
struct GunMsgPayload8017744C {
    unsigned char data[0x6c];
};

int Fn800F7F5C(int a, int b, int c, int d, void* data, float f);
int Fn80161FF4(Owner* owner, void* data, float f);
extern float lbl_804D8148;
extern float lbl_804D814C;

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Weapon behaviour message handler (GunMsgHandler801744C is the descriptive
// name): picks the next state from the message id, fires the weapon when asked
// to and reports whether the behaviour should keep running.
extern "C" int fn_8017744C(void* ownerArg, void* behArg, int msg) {
    GunMsgOwner8017744C* owner;
    GunMsgBlock8017744C* block;
    GunMsgPayload8017744C payload;
    int reply;

    if (behArg == 0) {
        return 0;
    }
    owner = (GunMsgOwner8017744C*)ownerArg;
    block = ((GunMsgBeh8017744C*)behArg)->block;
    switch (msg) {
    case 1:
        if (block->state == 0 && block->time <= lbl_804D8128) {
            block->next = 4;
        }
        break;
    case 2:
        if (block->state == 0 && block->time <= lbl_804D8128) {
            block->next = 5;
        }
        break;
    case 3:
        if (block->state == 0) {
            block->next = 6;
        }
        break;
    case 4:
        if (block->state == 0) {
            block->next = 7;
        }
        break;
    case 0x20ee36:
        if ((block->flags & 1) == 0) {
            reply = Fn800F7DEC(owner->field8EC, owner->field8F0, 0x20ee36,
                               OwnerId((Owner*)ownerArg), 0);
            if (reply == 0x04a3e183) {
                block->flags |= 1;
                if (Fn80161FF4((Owner*)ownerArg, &payload, lbl_804D8148) == 0) {
                    return 0;
                }
                Fn800F7F5C(owner->field8EC, owner->field8F0, 0x20ee36,
                           OwnerId((Owner*)ownerArg), &payload, owner->field138);
                block->time = lbl_804D814C;
            }
        }
        break;
    case 0x1b83833f:
        block->flags |= 0x10;
        break;
    case 0xe6b71bcc:
    case 0x968cbe3d:
        if ((block->flags & 0x20) == 0) {
            block->flags |= 0x20;
            Fn800F7F5C(owner->field8EC, owner->field8F0, msg,
                       OwnerId((Owner*)ownerArg), 0, owner->field138);
        }
        break;
    case 0xf829db93:
        break;
    }
    return 1;
}

// One row of the behaviour state table: enter, change and update callbacks.
struct GunStateEntry80177694 {
    void (*enter)(void* owner, void* beh);   // 0x00
    void (*change)(void* owner, void* beh);  // 0x04
    void (*update)(void* owner);             // 0x08
};

extern GunStateEntry80177694 lbl_803A8140[];

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Per-frame behaviour update (GunStateUpdate80177694 is the descriptive name):
// runs the current state's update callback, then - when a new state was
// requested - the change callback, the state copy and the new state's enter
// callback.
extern "C" int fn_80177694(void* ownerArg, void* behArg) {
    GunMsgOwner8017744C* owner;
    GunMsgBeh8017744C* beh;
    GunMsgBlock8017744C* block;

    if (ownerArg == 0 || behArg == 0) {
        return 0;
    }
    owner = (GunMsgOwner8017744C*)ownerArg;
    beh = (GunMsgBeh8017744C*)behArg;
    block = beh->block;
    if (block->time >= lbl_804D8128) {
        block->time -= owner->field138;
    }
    if (lbl_803A8140[block->state].update != 0) {
        lbl_803A8140[block->state].update(ownerArg);
    }
    if (block->next != block->state) {
        if (lbl_803A8140[block->state].change != 0) {
            lbl_803A8140[block->state].change(ownerArg, behArg);
        }
        block->state = block->next;
        beh->field10 = block->next;
        if (lbl_803A8140[block->state].enter != 0) {
            lbl_803A8140[block->state].enter(ownerArg, behArg);
        }
    }
    return 1;
}

void* VAlloc(int size);
void VFree(void* ptr);

// Name is a guess: frees the behaviour block hung off +0x54 of the behaviour
// object (GunBehBlockFree801777B0).  Returns false when there is no behaviour
// object.  Placeholder name kept: u_80161F40.cpp references it from the
// behaviour table.
extern "C" bool fn_801777B0(void* ownerArg, void* behArg) {
    GunMsgBeh8017744C* beh;

    if (behArg == 0) {
        return false;
    }
    beh = (GunMsgBeh8017744C*)behArg;
    VFree(beh->block);
    return true;
}

// Name is a guess: allocates a fresh 0x10-byte behaviour block, stores it at
// +0x54 of the behaviour object and runs the initial state's enter callback
// (GunBehBlockCreate801777E8).  Returns false when there is no behaviour object
// or the allocation fails.  Placeholder name kept: u_80161F40.cpp references
// it from the behaviour table.
extern "C" bool fn_801777E8(void* ownerArg, void* behArg) {
    GunMsgBeh8017744C* beh;
    GunMsgBlock8017744C* block;

    if (behArg == 0) {
        return false;
    }
    beh = (GunMsgBeh8017744C*)behArg;
    block = (GunMsgBlock8017744C*)VAlloc(0x10);
    if (block == 0) {
        beh->block = 0;
        return false;
    }
    beh->block = block;
    beh->field10 = 0;
    block->state = 0;
    block->next = 0;
    block->flags = 0;
    block->time = lbl_804D8128;
    if (lbl_803A8140[block->state].enter != 0) {
        lbl_803A8140[block->state].enter(ownerArg, beh);
    }
    return true;
}

// Behaviour-parameter flags word plus a value cleared once the flags say the
// parameter is both active (0x20) and ready (0x10); offsets/names are guesses.
struct Ov80177898State {
    unsigned char unk00[0x04];   // 0x00
    unsigned long value;         // 0x04
    unsigned long flags;         // 0x08
};

struct Ov80177898Holder {
    unsigned char unk00[0x54];   // 0x00
    Ov80177898State* state;      // 0x54
};

// Name is a guess: clears the behaviour parameter's value word when its active
// and ready flags are both set. Called through a behaviour table, so the owner
// argument is unused here.
void Ov80177898_Reset(void* owner, void* param) {
    if (param == 0) {
        return;
    }
    Ov80177898Holder* holder = (Ov80177898Holder*)param;
    Ov80177898State* state = holder->state;
    if ((state->flags & 0x20) == 0) {
        return;
    }
    if ((state->flags & 0x10) == 0) {
        return;
    }
    state->value = 0;
}

// Name is a guess: same as Ov80177898_Reset, second behaviour-table entry.
void Ov801778C4_Reset(void* owner, void* param) {
    if (param == 0) {
        return;
    }
    Ov80177898Holder* holder = (Ov80177898Holder*)param;
    Ov80177898State* state = holder->state;
    if ((state->flags & 0x20) == 0) {
        return;
    }
    if ((state->flags & 0x10) == 0) {
        return;
    }
    state->value = 0;
}

// Name is a guess: asks the owner whether the aim behaviour is still wanted (the
// query is repeated while the first reply is not 0xc0becf7e) and settles the flag
// block's state counter from the reply; when the block reports both 0x40 and 0x10
// the reply is asked once more and mapped onto the state counter.
void GunAimStateCheck801778F0(Owner* owner, Beh* beh) {
    GunAimOwner80176D4C* o;
    GunFlagBlock80176CF4* b;
    int result;

    if (beh == 0) {
        return;
    }
    b = ((GunFlagOwner80176CF4*)beh)->flags;
    o = (GunAimOwner80176D4C*)owner;
    result = Fn800F7DEC(o->field8EC, o->field8F0, 0xf829db93, OwnerId(owner), 0);
    if (result == (int)0xc0becf7e) {
        b->field04 = 7;
    } else {
        result = Fn800F7DEC(o->field8EC, o->field8F0, 0xf829db93, OwnerId(owner), 0);
        if (result == 0x11d2c025) {
            b->field04 = 7;
        }
    }
    if ((b->flags & 0x40) != 0 && (b->flags & 0x10) != 0) {
        result = Fn800F7DEC(o->field8EC, o->field8F0, 0xf829db93, OwnerId(owner), 0);
        switch (result) {
        case 0xc0becf7e:
            b->field04 = 7;
            break;
        case 0x04a3e183:
            b->field04 = 0;
            break;
        case 0x3da260f7:
            break;
        case 0x11d2c025:
            break;
        }
    }
}

// Owner-side fields of the effect-spawn behaviour (offsets from retail; names are
// guesses: the model handle, two bone ids, the animation frame and the owner's
// world position).
struct GunFxOwner80177A2C {
    unsigned char unk00[0x84];           // 0x00
    Vec3 pos;                            // 0x84
    unsigned char unk90[0xcc - 0x90];    // 0x90
    int model;                           // 0xcc
    unsigned char unkD0[0xd8 - 0xd0];    // 0xd0
    int boneA;                           // 0xd8
    int boneB;                           // 0xdc
    unsigned char unkE0[0xf0 - 0xe0];    // 0xe0
    int anim;                            // 0xf0
    unsigned char unkF4[0x8ec - 0xf4];   // 0xf4
    int field8EC;                        // 0x8ec
    int field8F0;                        // 0x8f0
};

// Per-behaviour block of the effect-spawn behaviour: the state counter at 0x04,
// the flag word at 0x08 and the live effect handle at 0x0c (names are guesses).
struct GunFxBlock80177A2C {
    int state;           // 0x00
    int next;            // 0x04
    unsigned int flags;  // 0x08
    int effect;          // 0x0c
};

// The behaviour object: aim/effect flags at 0x2c and the block at 0x54.
struct GunFxBeh80177A2C {
    unsigned char unk00[0x2c];           // 0x00
    unsigned int flags;                  // 0x2c
    unsigned char unk30[0x54 - 0x30];    // 0x30
    GunFxBlock80177A2C* block;           // 0x54
};

extern "C" int fn_800BA7FC(int arg);
int ModelFindBone(int model, int bone);
int ModelAnimate(int model, char* name, int frame);
extern "C" void* fn_8003A3F0(int model, int anim, int frame);
extern "C" int rand(void);
extern "C" int fn_80027A58(int type, Vec3* pos, float* params, int delay, int id);
extern "C" void fn_800277B0(int effect, Vec3* pos, float* params, int* delay);
extern "C" void fn_80027690(int effect);

extern float lbl_804D8150;
extern float lbl_804D8154;
extern float lbl_804D8158;
extern float lbl_804D815C;
extern float lbl_804D8160;
extern float lbl_804D8164;

// Name is a guess: effect-spawn behaviour update. Advances the behaviour, and -
// when it reports itself active, no menu is open and its effect has not run yet -
// starts (or, on the second pass, refreshes and then releases) an effect at the
// chosen bone of the owner's model, with a random lifetime and a randomised
// colour/parameter vector. Finally asks the owner whether the behaviour should
// keep running and moves the block's state counter from the reply.
void GunFxSpawnUpdate80177A2C(Owner* ownerArg, Beh* behArg) {
    GunFxOwner80177A2C* owner = (GunFxOwner80177A2C*)ownerArg;
    GunFxBeh80177A2C* beh;
    GunFxBlock80177A2C* block;
    Vec3 pos;
    float params[4];
    Mtx34 mtx;
    int bone;
    int delay;

    if (behArg == 0) {
        return;
    }
    beh = (GunFxBeh80177A2C*)behArg;
    block = beh->block;
    BehUpdate(ownerArg, behArg);
    Fn8016295C(ownerArg, behArg);
    if ((block->flags & 1) == 0) {
        return;
    }
    if (fn_800BA7FC(0x29) == 0 && (block->flags & 0x100) == 0) {
        bone = ModelFindBone(owner->model, owner->boneA);
        if (bone == -1) {
            bone = ModelFindBone(owner->model, owner->boneB);
        }
        if (bone != -1) {
            Mtx34* mats = (Mtx34*)fn_8003A3F0(
                owner->model, owner->anim, ModelAnimate(owner->model, 0, 0));
            ActorGetMatrix(&mtx, (Actor*)&mats[bone]);
            pos.x = mtx.trans[0];
            pos.y = mtx.trans[1];
            pos.z = mtx.trans[2];
        } else {
            pos.x = owner->pos.x;
            pos.y = owner->pos.y;
            pos.z = owner->pos.z;
        }
        delay = (int)(lbl_804D8158 *
                          ((float)(unsigned)rand() / lbl_804D8150) +
                      lbl_804D8154);
        params[0] = lbl_804D815C;
        params[1] = lbl_804D8160 *
                        ((float)(unsigned)rand() / lbl_804D8150) +
                    lbl_804D8160;
        params[2] = lbl_804D8164;
        params[3] = lbl_804D815C;
        if (block->effect == -1) {
            block->effect = fn_80027A58(1, &pos, params, delay, 9);
            beh->flags |= 0x100000;
        } else {
            fn_800277B0(block->effect, &owner->pos, params, &delay);
            beh->flags &= ~0x100000;
            fn_80027690(block->effect);
            block->effect = -1;
            block->flags |= 0x100;
        }
    }
    if ((block->flags & 0x10) != 0) {
        int result = Fn800F7DEC(owner->field8EC, owner->field8F0, 0x20ee36,
                                OwnerId(ownerArg), 0);
        switch (result) {
        case 0xc0becf7e:
            block->next = 6;
            break;
        case 0x04a3e183:
            if ((beh->flags & 4) == 0) {
                block->next = 0;
            }
            break;
        case 0x3da260f7:
            break;
        case 0x11d2c025:
            break;
        }
    }
}

// Name is a guess: behaviour-table callback that updates the behaviour and its
// state machine when a behaviour object is present (address 0x80177CE8).
void GunAimBehUpdate80177CE8(void* owner, void* beh) {
    if (beh != 0) {
        Owner* o = (Owner*)owner;
        Beh* b = (Beh*)beh;

        BehUpdate(o, b);
        Fn8016295C(o, b);
    }
}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x80177D34.
void GunAimNoOp80177D34(void) {}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x80177D38.
void GunAimNoOp80177D38(void) {}

// Placeholder name kept: called from a behaviour table. Releases the live effect
// handle held in the behaviour block and clears it so it is released only once
// (address 0x80177D3C).
extern "C" void fn_80177D3C(void* ownerArg, void* behArg) {
    GunFxBeh80177A2C* beh;
    GunFxBlock80177A2C* block;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        block = beh->block;
        if (block->effect != -1) {
            fn_80027690(block->effect);
            block->effect = -1;
        }
    }
}

// Name is a guess: another gun-aim behaviour callback. Releases the live effect
// handle held in the behaviour block and clears it so it is released only once
// (address 0x80177D84).
void GunAimBehRelease80177D84(void* ownerArg, void* behArg) {
    GunFxBeh80177A2C* beh;
    GunFxBlock80177A2C* block;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        block = beh->block;
        if (block->effect != -1) {
            fn_80027690(block->effect);
            block->effect = -1;
        }
    }
}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x80177DCC.
void GunAimNoOp80177DCC(void) {}

// Name is a guess: clears the flag block's +0x08 flags and ends the behaviour;
// behaviour-table callback (address 0x80177DD0).
void GunFlagStateDone80177DD0(void* owner, void* beh) {
    GunFlagOwner80176CF4* b;
    if (beh == 0) {
        return;
    }
    b = (GunFlagOwner80176CF4*)beh;
    b->flags->flags = 0;
    BehDone(beh);
}

// Name is a guess: same body as GunFlagStateDone80177DD0, used by a second
// behaviour table (address 0x80177E08).
void GunFlagStateDone80177E08(void* owner, void* beh) {
    GunFlagOwner80176CF4* b;
    if (beh == 0) {
        return;
    }
    b = (GunFlagOwner80176CF4*)beh;
    b->flags->flags = 0;
    BehDone(beh);
}

// Name is a guess: same body as GunFlagStateDone80177DD0, used by a further
// behaviour table (address 0x80177E40).
void GunFlagStateDone80177E40(void* owner, void* beh) {
    GunFlagOwner80176CF4* b;

    if (beh == 0) {
        return;
    }
    b = (GunFlagOwner80176CF4*)beh;
    b->flags->flags = 0;
    BehDone(beh);
}

// Name is a guess: same body as GunFlagStateDone80177DD0, used by a further
// behaviour table (address 0x80177E78).
void GunFlagStateDone80177E78(void* owner, void* beh) {
    GunFlagOwner80176CF4* b;

    if (beh == 0) {
        return;
    }
    b = (GunFlagOwner80176CF4*)beh;
    b->flags->flags = 0;
    BehDone(beh);
}

// Name is a guess: ends the behaviour's current state, then asks the owner
// twice (hashes 0xe6b71bcc and 0x968cbe3d); each 0x04a3e183 reply marks a
// different aim flag done (address 0x80177EB0).
void GunAimBothFlagsDone80177EB0(void* ownerArg, void* behArg) {
    GunAimOwner80176D4C* owner;
    GunAimBeh801773B8* beh;
    int result;

    if (behArg == 0) {
        return;
    }
    owner = (GunAimOwner80176D4C*)ownerArg;
    beh = (GunAimBeh801773B8*)behArg;
    beh->block->flags = 0;
    BehDone(behArg);
    result = Fn800F7DEC(owner->field8EC, owner->field8F0, 0xe6b71bcc,
                        OwnerId((Owner*)ownerArg), 0);
    if (result == 0x04a3e183) {
        beh->flags |= 0x10;
    }
    result = Fn800F7DEC(owner->field8EC, owner->field8F0, 0x968cbe3d,
                        OwnerId((Owner*)ownerArg), 0);
    if (result == 0x04a3e183) {
        beh->flags |= 0x20;
    }
}

// Placeholder name kept: u_80161F40.cpp's behaviour table references it.
// Behaviour-table callback that always answers 0, retail is `li r3,0; blr`
// (address 0x80177F78).
extern "C" int fn_80177F78(void* owner, void* beh) {
    return 0;
}

// Per-behaviour state block of the handlers below (offsets from retail; names are
// guesses).
struct GunTrailBlock80177F80 {
    int state;           // 0x00 current state
    int next;            // 0x04 requested state
    unsigned int flags;  // 0x08
    int effect;          // 0x0c effect handle, -1 when none
    float time;          // 0x10 seconds of life left
};

// The behaviour object; +0x10 receives the new state, +0x54 holds the block.
struct GunTrailBeh80177F80 {
    unsigned char unk00[0x10];        // 0x00
    int field10;                      // 0x10
    unsigned char unk14[0x54 - 0x14]; // 0x14
    GunTrailBlock80177F80* block;     // 0x54
};

// Owner-side fields the two handlers touch (offsets from retail; names are guesses).
struct GunTrailOwner80177F80 {
    unsigned char unk00[0xcc];            // 0x00
    int fieldCC;                          // 0xcc
    unsigned char unkD0[0xe4 - 0xd0];     // 0xd0
    int fieldE4;                          // 0xe4
    unsigned char unkE8[0x138 - 0xe8];    // 0xe8
    float field138;                       // 0x138
    unsigned char unk13C[0x270 - 0x13c];  // 0x13c
    unsigned char conditions[4];          // 0x270 condition set
    unsigned char unk274[0x8ec - 0x274];  // 0x274
    int field8EC;                         // 0x8ec
    int field8F0;                         // 0x8f0
};

int HasCondition(void* set, int id);
void ModelNodeSetup(int a, int b, int c, int d);
extern float lbl_804D8170;
extern float lbl_804D8174;

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Message handler of the gun trail behaviour (GunTrailMsg80177F80 is the descriptive
// name): the message id picks the block's next phase, and the two effect messages
// query the owner, fill a payload from the reply and send it on. Returns 0 to
// swallow the message, 1 to let it through.
extern "C" int fn_80177F80(void* ownerArg, void* behArg, int msg) {
    GunTrailOwner80177F80* owner = (GunTrailOwner80177F80*)ownerArg;
    GunTrailBlock80177F80* block;
    unsigned char payload20EE36[0x6c];
    unsigned char payloadF829DB93[0x60];
    int reply;

    if (behArg == 0) {
        return 0;
    }
    block = ((GunTrailBeh80177F80*)behArg)->block;
    switch (msg) {
    case 1:
        if (block->state != 0) {
            return 0;
        }
        block->next = 4;
        break;
    case 2:
        if (block->state != 0 || block->time > lbl_804D8164) {
            return 0;
        }
        block->next = 5;
        break;
    case 3:
        if (block->state != 0) {
            return 0;
        }
        block->next = 6;
        break;
    case 4:
        if (block->state != 0) {
            return 0;
        }
        block->next = 7;
        break;
    case 0x1b83833f:
        block->flags |= 0x10;
        break;
    case 0x20ee36:
        reply = Fn800F7DEC(owner->field8EC, owner->field8F0, 0x20ee36,
                           OwnerId((Owner*)ownerArg), 0);
        if (reply == 0x04a3e183) {
            block->flags |= 1;
            if (Fn80161FF4((Owner*)ownerArg, payload20EE36, lbl_804D8164) == 0) {
                return 0;
            }
            Fn800F7F5C(owner->field8EC, owner->field8F0, 0x20ee36,
                       OwnerId((Owner*)ownerArg), payload20EE36, owner->field138);
            block->flags &= ~0x100;
        }
        break;
    case 0xe6b71bcc:
    case 0x968cbe3d:
        if ((block->flags & 0x20) != 0) {
            return 0;
        }
        block->flags |= 0x20;
        Fn800F7F5C(owner->field8EC, owner->field8F0, msg,
                   OwnerId((Owner*)ownerArg), 0, owner->field138);
        break;
    case 0xf829db93:
        if ((block->flags & 0x40) != 0) {
            return 1;
        }
        reply = Fn800F7DEC(owner->field8EC, owner->field8F0, 0xf829db93,
                           OwnerId((Owner*)ownerArg), 0);
        if (reply != 0x04a3e183) {
            return 1;
        }
        block->flags |= 0x40;
        if (Fn80161FF4((Owner*)ownerArg, payloadF829DB93, lbl_804D8170) == 0) {
            return 0;
        }
        Fn800F7F5C(owner->field8EC, owner->field8F0, 0xf829db93,
                   OwnerId((Owner*)ownerArg), payloadF829DB93, owner->field138);
        block->time = lbl_804D8174;
        break;
    }
    return 1;
}

// One row of this behaviour's state table: enter, change and update callbacks.
struct GunTrailStateEntry80178270 {
    void (*enter)(void* owner, void* beh);   // 0x00
    void (*change)(void* owner, void* beh);  // 0x04
    void (*update)(void* owner, void* beh);  // 0x08
};

extern GunTrailStateEntry80178270 lbl_803A81A0[];

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Per-frame update of the gun trail behaviour (GunTrailUpdate80178270 is the
// descriptive name): drops the trail while one of its stop conditions holds, and
// otherwise runs the current state's update callback and, when a new state was
// requested, the change callback, the state copy and the new state's enter callback.
extern "C" int fn_80178270(void* ownerArg, void* behArg) {
    GunTrailOwner80177F80* owner = (GunTrailOwner80177F80*)ownerArg;
    GunTrailBlock80177F80* block;
    GunTrailStateEntry80178270* entry;

    if (behArg == 0) {
        return 0;
    }
    block = ((GunTrailBeh80177F80*)behArg)->block;
    if (HasCondition(owner->conditions, 9) != 0 ||
        HasCondition(owner->conditions, 10) != 0 ||
        HasCondition(owner->conditions, 11) != 0) {
        if (block->effect != -1) {
            fn_80027690(block->effect);
            block->effect = -1;
        }
        ModelNodeSetup(owner->fieldCC, owner->fieldE4, 0, 1);
        block->flags = 0;
        block->time = lbl_804D8164;
        BehReset(behArg);
        BehDone(behArg);
        return 1;
    }
    if (block->time >= lbl_804D8164) {
        block->time -= owner->field138;
    }
    entry = &lbl_803A81A0[block->state];
    if (entry->update != 0) {
        entry->update(ownerArg, behArg);
    }
    if (block->next != block->state) {
        entry = &lbl_803A81A0[block->state];
        if (entry->change != 0) {
            entry->change(ownerArg, behArg);
        }
        block->state = block->next;
        ((GunTrailBeh80177F80*)behArg)->field10 = block->next;
        entry = &lbl_803A81A0[block->state];
        if (entry->enter != 0) {
            entry->enter(ownerArg, behArg);
        }
    }
    return 1;
}

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Gun trail block release callback (GunTrailBlockFree80178414 is the descriptive
// name): kills the block's effect if one is running and frees the block.
extern "C" int fn_80178414(void* owner, void* beh) {
    GunTrailBlock80177F80* block;

    if (beh == 0) {
        return 0;
    }
    block = ((GunTrailBeh80177F80*)beh)->block;
    if (block->effect != -1) {
        fn_80027690(block->effect);
        block->effect = -1;
    }
    VFree(block);
    return 1;
}

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Gun trail block setup callback (GunTrailBlockSetup80178470 is the descriptive
// name): allocates and initialises the block, then runs the initial state's
// enter callback.
extern "C" int fn_80178470(void* owner, void* beh) {
    GunTrailBeh80177F80* behObj;
    GunTrailBlock80177F80* block;
    GunTrailStateEntry80178270* entry;

    if (beh == 0) {
        return 0;
    }
    behObj = (GunTrailBeh80177F80*)beh;
    block = (GunTrailBlock80177F80*)VAlloc(0x14);
    if (block == 0) {
        behObj->block = 0;
        return 0;
    }
    behObj->block = block;
    behObj->field10 = 0;
    block->state = 0;
    block->next = 0;
    block->effect = -1;
    block->flags = 0;
    block->time = 0.0f;
    entry = &lbl_803A81A0[block->state];
    if (entry->enter != 0) {
        entry->enter(owner, beh);
    }
    return 1;
}

// Name is a guess: clears the flag block's +0x04 value once the block reports
// both 0x20 and 0x10; a behaviour-table callback.
void GunFlagStateClear80178528(void* owner, void* beh) {
    GunFlagOwner80176CF4* o = (GunFlagOwner80176CF4*)beh;
    GunFlagBlock80176CF4* b;

    if (o == 0) {
        return;
    }
    b = o->flags;
    if ((b->flags & 0x20) == 0) {
        return;
    }
    if ((b->flags & 0x10) == 0) {
        return;
    }
    b->field04 = 0;
}

// Name is a guess: same body as GunFlagStateClear80178528, used by another
// behaviour table.
void GunFlagStateClear80178554(void* owner, void* beh) {
    GunFlagOwner80176CF4* o = (GunFlagOwner80176CF4*)beh;
    GunFlagBlock80176CF4* b;

    if (o == 0) {
        return;
    }
    b = o->flags;
    if ((b->flags & 0x20) == 0) {
        return;
    }
    if ((b->flags & 0x10) == 0) {
        return;
    }
    b->field04 = 0;
}

extern float lbl_804D8178;
extern float lbl_804D817C;
extern float lbl_804D8180;
extern float lbl_804D8184;
extern float lbl_804D8188;
extern float lbl_804D818C;

// Name is a guess: effect-spawn behaviour update variant (behaviour-table entry,
// address 0x80178580). Starts or, on the second pass, refreshes and then releases
// an effect at the chosen bone of the owner's model with a randomised lifetime
// and parameter vector; finally asks the owner whether the behaviour should keep
// running and moves the block's state counter from the reply.
void GunFxSpawnState80178580(Owner* ownerArg, Beh* behArg) {
    GunFxOwner80177A2C* owner = (GunFxOwner80177A2C*)ownerArg;
    GunFxBeh80177A2C* beh;
    GunFxBlock80177A2C* block;
    Vec3 pos;
    float params[4];
    Mtx34 mtx;
    int bone;
    int delay;
    int result;

    if (behArg == 0) {
        return;
    }
    beh = (GunFxBeh80177A2C*)behArg;
    block = beh->block;
    BehUpdate(ownerArg, behArg);
    Fn8016295C(ownerArg, behArg);
    if ((block->flags & 1) == 0) {
        return;
    }
    if (fn_800BA7FC(0x29) == 0 && (block->flags & 0x100) == 0) {
        bone = ModelFindBone(owner->model, owner->boneA);
        if (bone == -1) {
            bone = ModelFindBone(owner->model, owner->boneB);
        }
        if (bone != -1) {
            Mtx34* mats = (Mtx34*)fn_8003A3F0(
                owner->model, owner->anim, ModelAnimate(owner->model, 0, 0));
            ActorGetMatrix(&mtx, (Actor*)&mats[bone]);
            pos.x = mtx.trans[0];
            pos.y = mtx.trans[1];
            pos.z = mtx.trans[2];
        } else {
            pos.x = owner->pos.x;
            pos.y = owner->pos.y;
            pos.z = owner->pos.z;
        }
        delay = (int)(lbl_804D8180 *
                          ((float)(unsigned)rand() / lbl_804D8178) +
                      lbl_804D817C);
        params[0] = lbl_804D8184;
        params[1] = lbl_804D8188 *
                        ((float)(unsigned)rand() / lbl_804D8178) +
                    lbl_804D8188;
        params[2] = lbl_804D818C;
        params[3] = lbl_804D8184;
        if (block->effect == -1) {
            block->effect = fn_80027A58(1, &pos, params, delay, 9);
            beh->flags |= 0x100000;
        } else {
            fn_800277B0(block->effect, &owner->pos, params, &delay);
            beh->flags &= ~0x100000;
            fn_80027690(block->effect);
            block->effect = -1;
            block->flags |= 0x100;
        }
    }
    if ((block->flags & 0x10) != 0) {
        result = Fn800F7DEC(owner->field8EC, owner->field8F0, 0xf829db93,
                            OwnerId(ownerArg), 0);
        switch (result) {
        case 0xc0becf7e:
            block->next = 7;
            break;
        case 0x04a3e183:
            if ((beh->flags & 8) == 0) {
                block->next = 0;
            }
            break;
        case 0x11d2c025:
            break;
        }
    }
}

// Name is a guess: effect-spawn behaviour update variant used by the second
// behaviour table (address 0x8017883C). Same body as fn_80178580 but queries the
// owner with 0x20ee36 and moves to state 6 (address 0x8017883C).
void GunFxSpawnState8017883C(Owner* ownerArg, Beh* behArg) {
    GunFxOwner80177A2C* owner = (GunFxOwner80177A2C*)ownerArg;
    GunFxBeh80177A2C* beh;
    GunFxBlock80177A2C* block;
    Vec3 pos;
    float params[4];
    Mtx34 mtx;
    int bone;
    int delay;
    int result;

    if (behArg == 0) {
        return;
    }
    beh = (GunFxBeh80177A2C*)behArg;
    block = beh->block;
    BehUpdate(ownerArg, behArg);
    Fn8016295C(ownerArg, behArg);
    if ((block->flags & 1) == 0) {
        return;
    }
    if (fn_800BA7FC(0x29) == 0 && (block->flags & 0x100) == 0) {
        bone = ModelFindBone(owner->model, owner->boneA);
        if (bone == -1) {
            bone = ModelFindBone(owner->model, owner->boneB);
        }
        if (bone != -1) {
            Mtx34* mats = (Mtx34*)fn_8003A3F0(
                owner->model, owner->anim, ModelAnimate(owner->model, 0, 0));
            ActorGetMatrix(&mtx, (Actor*)&mats[bone]);
            pos.x = mtx.trans[0];
            pos.y = mtx.trans[1];
            pos.z = mtx.trans[2];
        } else {
            pos.x = owner->pos.x;
            pos.y = owner->pos.y;
            pos.z = owner->pos.z;
        }
        delay = (int)(lbl_804D8180 *
                          ((float)(unsigned)rand() / lbl_804D8178) +
                      lbl_804D817C);
        params[0] = lbl_804D8184;
        params[1] = lbl_804D8188 *
                        ((float)(unsigned)rand() / lbl_804D8178) +
                    lbl_804D8188;
        params[2] = lbl_804D818C;
        params[3] = lbl_804D8184;
        if (block->effect == -1) {
            block->effect = fn_80027A58(1, &pos, params, delay, 9);
            beh->flags |= 0x100000;
        } else {
            fn_800277B0(block->effect, &owner->pos, params, &delay);
            beh->flags &= ~0x100000;
            fn_80027690(block->effect);
            block->effect = -1;
            block->flags |= 0x100;
        }
    }
    if ((block->flags & 0x10) != 0) {
        result = Fn800F7DEC(owner->field8EC, owner->field8F0, 0x20ee36,
                            OwnerId(ownerArg), 0);
        switch (result) {
        case 0xc0becf7e:
            block->next = 6;
            break;
        case 0x04a3e183:
            if ((beh->flags & 4) == 0) {
                block->next = 0;
            }
            break;
        case 0x11d2c025:
            break;
        }
    }
}

// Name is a guess: state callback that updates the behaviour and then hands the
// same owner/behaviour pair on to the shared behaviour advance.
void BehPairUpdate80178AF8(Owner* owner, Beh* beh) {
    if (beh != 0) {
        BehUpdate(owner, beh);
        Fn8016295C(owner, beh);
    }
}

// Name is a guess: empty table callback (retail is a bare blr).
void BehNoop80178B44() {
}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x80178B48.
void GunAimNoOp80178B48(void) {}

// Name is a guess: gun-aim behaviour callback, the twin of the 0x80177D3C
// callback above. Releases the live effect handle held in the behaviour block
// and clears it so it is released only once (address 0x80178B4C).
void GunAimBehRelease80178B4C(void* ownerArg, void* behArg) {
    GunFxBeh80177A2C* beh;
    GunFxBlock80177A2C* block;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        block = beh->block;
        if (block->effect != -1) {
            fn_80027690(block->effect);
            block->effect = -1;
        }
    }
}

// Name is a guess: gun-aim behaviour callback, the twin of the 0x80178B4C
// callback above. Releases the live effect handle held in the behaviour block
// and clears it so it is released only once (address 0x80178B94).
void GunAimBehStop80178B94(void* ownerArg, void* behArg) {
    GunFxBeh80177A2C* beh;
    GunFxBlock80177A2C* block;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        block = beh->block;
        if (block->effect != -1) {
            fn_80027690(block->effect);
            block->effect = -1;
        }
    }
}

// Name is a guess: empty gun-aim table callback, retail body is a bare `blr`
// (address 0x80178BDC).
void GunNoop80178BDC(void) {}

// Name is a guess: gun-aim behaviour callback, the twin of the 0x80178B4C
// callback above. Clears the flag word of the behaviour block and then hands
// the behaviour back so it is finished (address 0x80178BE0).
void GunAimBehReset80178BE0(void* ownerArg, void* behArg) {
    GunFxBeh80177A2C* beh;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        beh->block->flags = 0;
        BehDone(beh);
    }
}

// Name is a guess: gun-aim behaviour callback, identical to the 0x80178BE0
// callback above: clears the behaviour block flag and finishes the behaviour
// (address 0x80178C18).
void GunAimBehReset80178C18(void* ownerArg, void* behArg) {
    GunFxBeh80177A2C* beh;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        beh->block->flags = 0;
        BehDone(beh);
    }
}

// Name is a guess: gun-aim behaviour callback, identical to the 0x80178C18
// callback above: clears the behaviour block flag and finishes the behaviour
// (address 0x80178C50).
void GunAimBehReset80178C50(void* ownerArg, void* behArg) {
    GunFxBeh80177A2C* beh;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        beh->block->flags = 0;
        BehDone(beh);
    }
}

// Name is a guess: gun-aim behaviour callback, identical to the 0x80178C50
// callback above: clears the behaviour block flag and finishes the behaviour
// (address 0x80178C88).
void GunAimBehReset80178C88(void* ownerArg, void* behArg) {
    GunFxBeh80177A2C* beh;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        beh->block->flags = 0;
        BehDone(beh);
    }
}

// Name is a guess: gun-aim behaviour callback that clears the block flag word,
// finishes the behaviour, asks the owner and marks the behaviour when the reply
// is the aim hash (address 0x80178CC0).
void GunAimBehCheck80178CC0(void* ownerArg, void* behArg) {
    GunAimOwner80176D4C* owner;
    GunFxBeh80177A2C* beh;
    int result;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        owner = (GunAimOwner80176D4C*)ownerArg;
        beh->block->flags = 0;
        BehDone(beh);
        result = Fn800F7DEC(owner->field8EC, owner->field8F0, 0xe6b71bcc,
                            OwnerId((Owner*)owner), 0);
        if (result == 0x04a3e183) {
            beh->flags |= 0x10;
        }
    }
}

// Name is a guess: gun-aim state query that always reports failure
// (address 0x80178D4C).
// fn_80178D4C
extern "C" int fn_80178D4C(void) { return 0; }

// Per-behaviour state block of the two gun-aim handlers below: the state
// counter, the requested state, the flag word, the live effect handle and the
// block's remaining life (offsets from retail; names are guesses).
struct GunAimBlock80178D54 {
    int state;           // 0x00 current state
    int next;            // 0x04 requested state
    unsigned int flags;  // 0x08
    int effect;          // 0x0c live effect handle, -1 when none
    float time;          // 0x10 seconds of life left
};

// The behaviour object: +0x10 receives the new state, +0x2c holds the aim flags
// and +0x54 holds the block.
struct GunAimBeh80178D54 {
    unsigned char unk00[0x10];           // 0x00
    int field10;                         // 0x10 current state copy
    unsigned char unk14[0x2c - 0x14];    // 0x14
    unsigned int flags;                  // 0x2c
    unsigned char unk30[0x54 - 0x30];    // 0x30
    GunAimBlock80178D54* block;          // 0x54
};

// Owner-side fields the handlers touch (offsets from retail; names are guesses:
// the model handle, two node ids, the frame delta, the condition set and the
// message channel of the owner).
struct GunAimOwner80178D54 {
    unsigned char unk00[0xcc];            // 0x00
    int fieldCC;                          // 0xcc
    unsigned char unkD0[0xe4 - 0xd0];     // 0xd0
    int fieldE4;                          // 0xe4
    unsigned char unkE8[0x138 - 0xe8];    // 0xe8
    float field138;                       // 0x138
    unsigned char unk13C[0x270 - 0x13c];  // 0x13c
    unsigned char conditions[4];          // 0x270 condition set
    unsigned char unk274[0x8ec - 0x274];  // 0x274
    int field8EC;                         // 0x8ec
    int field8F0;                         // 0x8f0
};

// Reply/parameter buffer filled by Fn80161FF4 (retail stack slot is 0x6c bytes).
struct GunAimPayload80178D54 {
    unsigned char data[0x6c];
};

// One row of the gun-aim state table: enter, change and update callbacks.
struct GunAimStateEntry80178D54 {
    void (*enter)(void* owner, void* beh);   // 0x00
    void (*change)(void* owner, void* beh);  // 0x04
    void (*update)(void* owner, void* beh);  // 0x08
};

extern GunAimStateEntry80178D54 lbl_803A8200[];
extern float lbl_804D8198;

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Message handler of the gun-aim behaviour (GunAimMsg80178D54 is the descriptive
// name): the message id picks the block's next phase, the two effect messages
// ask the owner, fill a payload from the reply and send it on, and one message
// raises the block's aim flag. Returns 0 to swallow the message, 1 to let it
// through (address 0x80178D54).
extern "C" int fn_80178D54(void* ownerArg, void* behArg, int msg) {
    GunAimOwner80178D54* owner = (GunAimOwner80178D54*)ownerArg;
    GunAimBlock80178D54* block;
    GunAimPayload80178D54 payload;
    int reply;

    if (behArg == 0) {
        return 0;
    }
    block = ((GunAimBeh80178D54*)behArg)->block;
    switch (msg) {
    case 1:
        if (block->state != 0) {
            return 0;
        }
        block->next = 4;
        break;
    case 2:
        if (block->state != 0) {
            return 0;
        }
        block->next = 5;
        break;
    case 3:
        if (block->state != 0) {
            return 0;
        }
        block->next = 6;
        break;
    case 4:
        if (block->state != 0) {
            return 0;
        }
        block->next = 7;
        break;
    case 0x1b83833f:
        block->flags |= 0x10;
        break;
    case 0xe6b71bcc:
    case 0x968cbe3d:
        if ((block->flags & 0x20) != 0) {
            return 0;
        }
        block->flags |= 0x20;
        Fn800F7F5C(owner->field8EC, owner->field8F0, 0x968cbe3d,
                   OwnerId((Owner*)ownerArg), 0, owner->field138);
        break;
    case 0xf829db93:
    case 0x20ee36:
        reply = Fn800F7DEC(owner->field8EC, owner->field8F0, msg,
                           OwnerId((Owner*)ownerArg), 0);
        if (reply == 0x04a3e183) {
            block->flags |= 1;
            if (Fn80161FF4((Owner*)ownerArg, &payload, lbl_804D818C) == 0) {
                return 0;
            }
            Fn800F7F5C(owner->field8EC, owner->field8F0, msg,
                       OwnerId((Owner*)ownerArg), &payload, owner->field138);
            block->flags &= ~0x100;
            block->time = lbl_804D8198;
        }
        break;
    }
    return 1;
}

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Per-frame update of the gun-aim behaviour (GunAimTick80178FA0 is the
// descriptive name): when one of the aim conditions holds, releases the live
// effect, re-arms the owner's model and resets then finishes the behaviour;
// otherwise runs the current state's update callback, counts the block's life
// down, and - when a new state was requested - runs the change callback, copies
// the state and runs the new state's enter callback. Returns 1
// (address 0x80178FA0).
extern "C" int fn_80178FA0(void* ownerArg, void* behArg) {
    GunAimOwner80178D54* owner = (GunAimOwner80178D54*)ownerArg;
    GunAimBeh80178D54* beh;
    GunAimBlock80178D54* block;
    GunAimStateEntry80178D54* entry;

    if (behArg == 0) {
        return 0;
    }
    beh = (GunAimBeh80178D54*)behArg;
    block = beh->block;
    if (HasCondition(owner->conditions, 9) != 0 ||
        HasCondition(owner->conditions, 10) != 0 ||
        HasCondition(owner->conditions, 11) != 0) {
        if (block->effect != -1) {
            fn_80027690(block->effect);
            block->effect = -1;
        }
        ModelNodeSetup(owner->fieldCC, owner->fieldE4, 0, 1);
        block->flags = 0;
        block->time = lbl_804D818C;
        BehReset(behArg);
        BehDone(behArg);
    } else {
        entry = &lbl_803A8200[block->state];
        if (entry->update != 0) {
            entry->update(ownerArg, behArg);
        }
        if (block->time >= lbl_804D818C) {
            beh->flags &= ~4;
            block->time = block->time - owner->field138;
        }
        if (block->next != block->state) {
            entry = &lbl_803A8200[block->state];
            if (entry->change != 0) {
                entry->change(ownerArg, behArg);
            }
            block->state = block->next;
            beh->field10 = block->next;
            entry = &lbl_803A8200[block->state];
            if (entry->enter != 0) {
                entry->enter(ownerArg, behArg);
            }
        }
    }
    return 1;
}

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Release callback of the gun-aim behaviour (GunAimBehFree80179154 is the
// descriptive name): releases the live effect, frees the state block and
// reports success (address 0x80179154).
extern "C" int fn_80179154(void* ownerArg, void* behArg) {
    GunAimBlock80178D54* block;

    if (behArg == 0) {
        return 0;
    }
    block = ((GunAimBeh80178D54*)behArg)->block;
    if (block->effect != -1) {
        fn_80027690(block->effect);
        block->effect = -1;
    }
    VFree(block);
    return 1;
}

// Placeholder name kept: u_80161F40.cpp references it (behaviour table entry).
// Start callback of the gun-aim behaviour (GunAimBehStart801791B0 is the
// descriptive name): allocates the 0x14-byte state block, initialises it,
// stores it on the behaviour and runs the initial state's enter callback.
// Returns 0 when the behaviour is missing or the block cannot be allocated
// (address 0x801791B0).
extern "C" int fn_801791B0(void* ownerArg, void* behArg) {
    GunAimBeh80178D54* beh = (GunAimBeh80178D54*)behArg;
    GunAimBlock80178D54* block;

    if (beh == 0) {
        return 0;
    }
    block = (GunAimBlock80178D54*)VAlloc(0x14);
    if (block == 0) {
        beh->block = 0;
        return 0;
    }
    beh->block = block;
    beh->field10 = 0;
    block->state = 0;
    block->next = 0;
    block->flags = 0;
    block->effect = -1;
    block->time = 0.0f;
    if (lbl_803A8200[block->state].enter != 0) {
        lbl_803A8200[block->state].enter(ownerArg, behArg);
    }
    return 1;
}

// Owner-side fields the aim-matrix update below touches (offsets from retail;
// names are guesses): the model handle, the two bone ids, the animation, the aim
// mode, the aim range, the behaviour object BehTarget reads at 0x894, the fade
// window at 0x8b8 and the message channel of the owner.
struct GunAimMatOwner80179268 {
    unsigned char unk00[0xcc];            // 0x00
    int model;                            // 0xcc
    unsigned char unkD0[0xd8 - 0xd0];     // 0xd0
    int boneA;                            // 0xd8
    int boneB;                            // 0xdc
    unsigned char unkE0[0xf0 - 0xe0];     // 0xe0
    int anim;                             // 0xf0
    unsigned char unkF4[0x26c - 0xf4];    // 0xf4
    int mode;                             // 0x26c aim mode, picks the aim scale
    unsigned char unk270[0x5c8 - 0x270];  // 0x270
    float range;                          // 0x5c8 aim range
    unsigned char unk5CC[0x894 - 0x5cc];  // 0x5cc
    unsigned char behTarget[0x8b8 - 0x894]; // 0x894 behaviour passed to BehTarget
    unsigned short fadeStart;             // 0x8b8
    unsigned short fadeEnd;               // 0x8ba zero when the blend is off
    unsigned char unk8BC[0x8ec - 0x8bc];  // 0x8bc
    int field8EC;                         // 0x8ec condition/message channel
};

// The target object the first reply points at: the position it reports and the
// message channel the aim query goes to (names are guesses).
struct GunAimMatTarget80179268 {
    unsigned char unk00[0x24];            // 0x00
    int posX;                             // 0x24
    int posY;                             // 0x28
    int posZ;                             // 0x2c
    unsigned char unk30[0x38 - 0x30];     // 0x30
    int msgTo;                            // 0x38 message channel
};

// The behaviour object the update writes: the basis matrix at 0x00, the bone
// translation at 0x30, the aim direction at 0x3c, the target and the aim range /
// blend / scale at 0x48..0x54 and the bone index at 0x5c (names are guesses).
struct GunAimMatBeh80179268 {
    Mtx34 matrix;                         // 0x00
    Vec3 field30;                         // 0x30 bone translation
    Vec3 field3C;                         // 0x3c aim direction
    void* target;                         // 0x48 target object from the reply
    float field4C;                        // 0x4c aim range
    float field50;                        // 0x50 aim blend
    float field54;                        // 0x54 aim scale
    unsigned char unk58[0x5c - 0x58];     // 0x58
    int bone;                             // 0x5c model bone index
};

// Reply to the "who are you aiming at" message (message 0x400).
struct GunAimReply80179268 {
    void* obj;                            // 0x00 target object
};

// Payload of the aim query (message 0x412): the object to query, the id of the
// sender and the position the reply writes at 0x34..0x3c (names are guesses).
struct GunAimQuery80179268 {
    void* obj;                            // 0x00
    int id;                               // 0x04
    unsigned char unk08[0x34 - 0x08];     // 0x08
    int x;                                // 0x34
    int y;                                // 0x38
    int z;                                // 0x3c
};

int BehTarget(Beh* beh);
extern "C" int fn_800F7C64(int channel, unsigned long id);
extern "C" int fn_800FC3B8(int condition);
extern "C" void fn_800A025C(Vec3* out, Mtx34* m, int index);
void Mtx34StoreColumn(Mtx34* m, Vec3* v, int index);
extern "C" int fn_80049940(void);

extern float lbl_804D81A0;
extern float lbl_804D81A4;
extern float lbl_804D81A8;
extern float lbl_804D81AC;
extern float lbl_804D81B0;
extern float lbl_804D81B4;
extern float lbl_804D81B8;
extern float lbl_804D81BC;
extern int lbl_804D4EC0;

// Scales a 3-vector to unit length and returns the inverse of its old length; a
// vector shorter than the epsilon is set to zero and reports zero.
static inline float GunNormalizeAimVec80179268(Vec3* v) {
    float inv;
    if (((fabsf(v->x) > lbl_804D81A8) + (fabsf(v->y) > lbl_804D81A8) +
         (fabsf(v->z) > lbl_804D81A8)) != 0) {
        inv = 1.0f / sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
        v->x *= inv;
        v->y *= inv;
        v->z *= inv;
        return inv;
    }
    v->x = lbl_804D81A4;
    v->y = lbl_804D81A4;
    v->z = lbl_804D81A4;
    return lbl_804D81A4;
}

// Name is a guess: aim-matrix update of the gun behaviour. Asks the behaviour's
// target for the object it aims at (message 0x400), picks the model bone to aim
// from (falling back to a second bone id), stores the target and the range on the
// behaviour and scales the range when the owner is in the scoped state. It then
// takes the target position from the aim query (message 0x412) or from the target
// object itself, adds the frame time to its height, and builds the behaviour's
// basis: the bone matrix row 0 as the reference direction, the normalised
// direction to the target as the middle column, the cross products as the first
// and third columns. Finally it blends the aim scale from the owner's aim mode
// and the fade window. Returns 1 when the basis was rebuilt, 0 with no target.
int GunAimMatrixUpdate80179268(float time, Owner* ownerArg, Beh* behArg) {
    GunAimMatOwner80179268* owner = (GunAimMatOwner80179268*)ownerArg;
    GunAimMatBeh80179268* beh = (GunAimMatBeh80179268*)behArg;
    GunAimReply80179268 reply;
    GunAimQuery80179268 query;
    GunAimMatTarget80179268* target;
    Mtx34* mats;
    Vec3 dir;
    Vec3 up;
    Vec3 delta;
    Vec3 side;
    Mtx34 mtx;
    float invDist;
    float blend;
    int posX;
    int posY;
    int posZ;
    int bone;

    if (SendMsg(BehTarget((Beh*)owner->behTarget), 0x400, &reply) == 0) {
        return 0;
    }
    target = (GunAimMatTarget80179268*)reply.obj;
    bone = ModelFindBone(owner->model, owner->boneA);
    beh->bone = bone;
    if (beh->bone == -1) {
        bone = ModelFindBone(owner->model, owner->boneB);
        beh->bone = bone;
    }
    if (target == 0 || beh->bone == -1) {
        return 0;
    }
    beh->target = target;
    beh->field4C = owner->range;
    if (fn_800F7C64(owner->field8EC, 0x20a5aa68) > 0 || fn_800FC3B8(0x12) != 0) {
        beh->field4C = beh->field4C * lbl_804D81A0;
    }
    query.obj = target;
    query.id = lbl_804D4EC0;
    if (lbl_804D4EC0 == -1) {
        posX = target->posX;
        posY = target->posY;
        posZ = target->posZ;
    } else if (SendMsg(target->msgTo, 0x412, &query) != 0) {
        posX = query.x;
        posY = query.y;
        posZ = query.z;
    }
    posZ += (int)time;
    mats = (Mtx34*)fn_8003A3F0(owner->model, owner->anim,
                               ModelAnimate(owner->model, 0, 0));
    ActorGetMatrix(&mtx, (Actor*)&mats[beh->bone]);
    beh->field30.x = mtx.trans[0];
    beh->field30.y = mtx.trans[1];
    beh->field30.z = mtx.trans[2];
    beh->matrix.trans[0] = mtx.trans[0];
    beh->matrix.trans[1] = mtx.trans[1];
    beh->matrix.trans[2] = mtx.trans[2];
    fn_800A025C(&dir, &mtx, 0);
    dir.z = lbl_804D81A4;
    GunNormalizeAimVec80179268(&dir);
    {
        int* bonePos = (int*)&mats[beh->bone].trans[0];
        delta.x = (float)(posX - bonePos[0]);
        delta.y = (float)(posY - bonePos[1]);
        delta.z = (float)(posZ - bonePos[2]);
    }
    {
        float sq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        invDist = lbl_804D81A4;
        if (sq > lbl_804D81A4) {
            invDist = 1.0f / sqrtf(sq);
        }
    }
    GunNormalizeAimVec80179268(&delta);
    if (delta.z * dir.z + delta.x * dir.x + delta.y * dir.y < lbl_804D81AC) {
        delta.x = dir.x;
        delta.y = dir.y;
        delta.z = dir.z;
    }
    ActorGetMatrix(&mtx, (Actor*)mats);
    fn_800A025C(&side, &mtx, 0);
    fn_800A025C(&up, &mtx, 2);
    up.x = side.y * delta.z - side.z * delta.y;
    up.y = side.z * delta.x - side.x * delta.z;
    up.z = side.x * delta.y - side.y * delta.x;
    side.x = delta.y * up.z - delta.z * up.y;
    side.y = delta.z * up.x - delta.x * up.z;
    side.z = delta.x * up.y - delta.y * up.x;
    Mtx34StoreColumn(&mtx, &side, 0);
    Mtx34StoreColumn(&mtx, &delta, 1);
    Mtx34StoreColumn(&mtx, &up, 2);
    beh->field54 = lbl_804D81B0;
    switch (owner->mode) {
    case 0:
        beh->field50 = lbl_804D81A4;
        break;
    case 1:
        beh->field50 = lbl_804D81B4;
        break;
    case 2:
        beh->field50 = lbl_804D81B8;
        break;
    case 3:
        beh->field50 = lbl_804D81B0;
        break;
    }
    blend = lbl_804D81A4;
    if (owner->fadeEnd != 0) {
        blend = (invDist - (float)owner->fadeStart) /
                (float)(owner->fadeEnd - owner->fadeStart);
        if (blend < lbl_804D81A4) {
            blend = lbl_804D81A4;
        } else if (blend > lbl_804D81B0) {
            blend = lbl_804D81B0;
        }
    }
    beh->field50 = (lbl_804D81B0 - beh->field50) * blend + beh->field50;
    if (fn_80049940() != 0) {
        beh->field50 = beh->field50 * lbl_804D81BC;
    }
    beh->field3C.x = delta.x;
    beh->field3C.y = delta.y;
    beh->field3C.z = delta.z;
    fn_800A0168((Mtx34*)beh, &mtx);
    return 1;
}

// Flags block reached through the behaviour's +0x54 pointer (field names are
// guesses): the state request at 0x04 and the aim flags at 0x08.
struct GunAimFlagBlock801798A4 {
    int field00;         // 0x00
    int field04;         // 0x04 requested state
    unsigned int flags;  // 0x08
};

// The behaviour object; only its +0x54 block pointer is used.
struct GunAimFlagBeh801798A4 {
    unsigned char unk00[0x54];               // 0x00
    GunAimFlagBlock801798A4* block;          // 0x54
};

// Name is a guess: state callback that drops the behaviour's requested state once
// the block has both the released flag (0x20) and the done flag (0x10) set, which
// stops the behaviour. Does nothing while either flag is clear.
void GunAimResetNext801798A4(void* owner, void* behArg) {
    GunAimFlagBeh801798A4* beh;
    GunAimFlagBlock801798A4* block;

    if (behArg == 0) {
        return;
    }
    beh = (GunAimFlagBeh801798A4*)behArg;
    block = beh->block;
    if ((block->flags & 0x20) == 0) {
        return;
    }
    if ((block->flags & 0x10) == 0) {
        return;
    }
    block->field04 = 0;
}

extern float lbl_804D81D0;
extern float lbl_804D81D4;
extern float lbl_804D81D8;
extern float lbl_804D81DC;

// Name is a guess: state callback with the same shape as GunAimResetNext801798A4
// above - it drops the behaviour's requested state once the block has both the
// released flag (0x20) and the done flag (0x10) set. The owner is unused.
void GunAimResetNext801798D0(void* owner, void* behArg) {
    GunAimFlagBeh801798A4* beh;
    GunAimFlagBlock801798A4* block;

    if (behArg == 0) {
        return;
    }
    beh = (GunAimFlagBeh801798A4*)behArg;
    block = beh->block;
    if ((block->flags & 0x20) == 0) {
        return;
    }
    if ((block->flags & 0x10) == 0) {
        return;
    }
    block->field04 = 0;
}

// The effect handle block reached through the behaviour's +0x54 pointer: the
// requested state at 0x04, the flags at 0x08 and the live effect id at 0x0c.
struct GunAimFxBlock801798FC {
    int field00;   // 0x00
    int state;     // 0x04 requested state
    int flags;     // 0x08
    int effect;    // 0x0c live effect id, -1 when none
};

// The behaviour object; only its +0x54 block pointer is used.
struct GunAimFxBeh801798FC {
    unsigned char unk00[0x54];              // 0x00
    GunAimFxBlock801798FC* block;           // 0x54
};

// The owner/actor the behaviour belongs to; only the fields used here are named.
struct GunAimFxOwner801798FC {
    unsigned char unk00[0x84];                  // 0x00
    Vec3 pos;                                   // 0x84
    unsigned char unk90[0xcc - 0x90];           // 0x90
    int model;                                  // 0xcc
    unsigned char unkD0[0xd8 - 0xd0];           // 0xd0
    int boneA;                                  // 0xd8
    int boneB;                                  // 0xdc
    unsigned char unkE0[0xf0 - 0xe0];           // 0xe0
    int anim;                                   // 0xf0
    unsigned char unkF4[0x138 - 0xf4];          // 0xf4
    float field138;                             // 0x138
    unsigned char unk13C[0x8ec - 0x13c];        // 0x13c
    int field8EC;                               // 0x8ec
    int field8F0;                               // 0x8f0
};

// Name is a guess: keeps the live effect of a gun behaviour in sync. While the
// block still holds a live effect id (not -1) it rebuilds the muzzle position
// from the model bone (falling back to the owner position), rolls a fresh delay
// and colour/parameter vector, and re-issues the effect update. When the block's
// 0x40 flag is set it also asks the owner for the effect message and drops the
// requested state on the answers that end it.
void GunAimFxUpdate801798FC(Owner* ownerArg, void* behArg) {
    GunAimFxOwner801798FC* owner = (GunAimFxOwner801798FC*)ownerArg;
    GunAimFxBeh801798FC* beh;
    GunAimFxBlock801798FC* block;
    Mtx34 mtx;
    Vec3 pos;
    float params[4];
    int delay;
    int bone;
    Mtx34* mats;

    if (behArg == 0) {
        return;
    }
    beh = (GunAimFxBeh801798FC*)behArg;
    block = beh->block;
    if (block->effect != -1) {
        bone = ModelFindBone(owner->model, owner->boneA);
        if (bone == -1) {
            bone = ModelFindBone(owner->model, owner->boneB);
        }
        if (bone != -1) {
            mats = (Mtx34*)fn_8003A3F0(owner->model, owner->anim,
                                        ModelAnimate(owner->model, 0, 0));
            ActorGetMatrix(&mtx, (Actor*)&mats[bone]);
            pos.x = mtx.trans[0];
            pos.y = mtx.trans[1];
            pos.z = mtx.trans[2];
        } else {
            pos.x = owner->pos.x;
            pos.y = owner->pos.y;
            pos.z = owner->pos.z;
        }
        delay = (int)(lbl_804D81D8 *
                          ((float)(unsigned)rand() / lbl_804D81D0) +
                      lbl_804D81D4);
        params[0] = lbl_804D81B0;
        params[1] = lbl_804D81DC *
                        ((float)(unsigned)rand() / lbl_804D81D0) +
                    lbl_804D81DC;
        params[2] = lbl_804D81A4;
        params[3] = lbl_804D81B0;
        fn_800277B0(block->effect, &pos, params, &delay);
    }
    if ((block->flags & 0x40) != 0) {
        switch (Fn800F7DEC(owner->field8EC, owner->field8F0, 0xf829db93,
                           OwnerId(ownerArg), 0)) {
        case 0xc0becf7e:
            Fn800F7F5C(owner->field8EC, owner->field8F0, 0x968cbe3d,
                       OwnerId(ownerArg), 0, owner->field138);
            block->state = 0;
            break;
        case 0x04a3e183:
            block->state = 0;
            break;
        case 0x3da260f7:
            break;
        case 0x11d2c025:
            break;
        }
    }
}

// Name is a guess: sibling of GunAimFxUpdate801798FC with its own message ids.
// Keeps the live effect of a gun behaviour in sync: while the block still holds a
// live effect id it rebuilds the muzzle position from the model bone (falling back
// to the owner position), rolls a fresh delay and parameter vector, and re-issues
// the effect update. When the block's 0x02 flag is set it asks the owner for the
// effect message and moves the requested state from the reply.
void GunAimFxUpdate80179B40(Owner* ownerArg, void* behArg) {
    GunAimFxOwner801798FC* owner = (GunAimFxOwner801798FC*)ownerArg;
    GunAimFxBeh801798FC* beh;
    GunAimFxBlock801798FC* block;
    Mtx34 mtx;
    Vec3 pos;
    float params[4];
    int delay;
    int bone;
    Mtx34* mats;

    if (behArg == 0) {
        return;
    }
    beh = (GunAimFxBeh801798FC*)behArg;
    block = beh->block;
    if (block->effect != -1) {
        bone = ModelFindBone(owner->model, owner->boneA);
        if (bone == -1) {
            bone = ModelFindBone(owner->model, owner->boneB);
        }
        if (bone != -1) {
            mats = (Mtx34*)fn_8003A3F0(owner->model, owner->anim,
                                        ModelAnimate(owner->model, 0, 0));
            ActorGetMatrix(&mtx, (Actor*)&mats[bone]);
            pos.x = mtx.trans[0];
            pos.y = mtx.trans[1];
            pos.z = mtx.trans[2];
        } else {
            pos.x = owner->pos.x;
            pos.y = owner->pos.y;
            pos.z = owner->pos.z;
        }
        delay = (int)(lbl_804D81D8 *
                          ((float)(unsigned)rand() / lbl_804D81D0) +
                      lbl_804D81D4);
        params[0] = lbl_804D81B0;
        params[1] = lbl_804D81DC *
                        ((float)(unsigned)rand() / lbl_804D81D0) +
                    lbl_804D81DC;
        params[2] = lbl_804D81A4;
        params[3] = lbl_804D81B0;
        fn_800277B0(block->effect, &pos, params, &delay);
    }
    if ((block->flags & 2) != 0) {
        switch (Fn800F7DEC(owner->field8EC, owner->field8F0, 0x20ee36,
                           OwnerId(ownerArg), 0)) {
        case 0xc0becf7e:
            block->state = 6;
            break;
        case 0x04a3e183:
            block->state = 0;
            break;
        case 0x3da260f7:
            break;
        case 0x11d2c025:
            break;
        }
    }
}

// The effect-timer block reached through the behaviour's +0x54 pointer: the
// requested state at 0x04 and the remaining life time at 0x18 (names are guesses).
struct GunAimFxTimerBlock80179D5C {
    unsigned char unk00[0x04];         // 0x00
    int state;                         // 0x04 requested state
    unsigned char unk08[0x18 - 0x08];  // 0x08
    float time;                        // 0x18 seconds of life left
};

// The behaviour object; only its +0x54 block pointer is used.
struct GunAimFxTimerBeh80179D5C {
    unsigned char unk00[0x54];                     // 0x00
    GunAimFxTimerBlock80179D5C* block;             // 0x54
};

// Name is a guess: counts the behaviour's life time down. A first effect message
// sends the owner's current payload on, the second one asks the owner whether the
// timer should tick (or has run out). When it has run out, a third message either
// asks for the payload and moves the requested state to 4, or drops the state.
void GunAimFxTimerUpdate80179D5C(Owner* ownerArg, void* behArg) {
    GunAimFxOwner801798FC* owner = (GunAimFxOwner801798FC*)ownerArg;
    GunAimFxTimerBeh80179D5C* beh;
    GunAimFxTimerBlock80179D5C* block;
    unsigned char payload315A1A7F[0x6c];
    unsigned char payloadD19E338[0x60];
    int expired = 0;
    int result;

    if (behArg == 0) {
        return;
    }
    beh = (GunAimFxTimerBeh80179D5C*)behArg;
    block = beh->block;
    if (Fn80161FF4(ownerArg, payload315A1A7F, lbl_804D81A4) != 0) {
        Fn800F7F5C(owner->field8EC, owner->field8F0, 0x315a1a7f,
                   OwnerId(ownerArg), payload315A1A7F, owner->field138);
    }
    result = Fn800F7DEC(owner->field8EC, owner->field8F0, 0x6fab23fe,
                        OwnerId(ownerArg), 0);
    switch (result) {
    case 0x7183f329:
    case 0x7183f5a1:
        if (lbl_804D81A4 <= block->time) {
            block->time -= owner->field138;
        } else {
            expired = 1;
        }
        break;
    case 0xbef8884d:
        expired = 1;
        break;
    case 0x3da260f7:
    default:
        block->state = 0;
        break;
    }
    if (expired) {
        result = Fn800F7DEC(owner->field8EC, owner->field8F0, 0xd19e338,
                            OwnerId(ownerArg), 0);
        if (result == 0x04a3e183) {
            block->state = 4;
            if (Fn80161FF4(ownerArg, payloadD19E338, lbl_804D81A4) != 0) {
                Fn800F7F5C(owner->field8EC, owner->field8F0, 0xd19e338,
                           OwnerId(ownerArg), payloadD19E338,
                           owner->field138);
            }
        }
    }
}

// Owner, follow target and behaviour of the second aim-target update family
// (offsets come from retail; names are guesses).
struct GunAimActor80179F08 {
    unsigned char unk00[0x24];          // 0x00
    int posX;                           // 0x24
    int posY;                           // 0x28
    int posZ;                           // 0x2c
    unsigned char unk30[0x270 - 0x30];  // 0x30
    int follow;                         // 0x270
};

// Follow target: its position and the id copied into the behaviour.
struct GunAimTarget80179F08 {
    unsigned char unk00[0x24];          // 0x00
    int posX;                           // 0x24
    int posY;                           // 0x28
    unsigned char unk2C[0x38 - 0x2c];   // 0x2c
    int field38;                        // 0x38
};

// Per-behaviour flag block; +0x1c gates the wide-angle aim test.
struct GunAimBlock80179F08 {
    unsigned char unk00[0x1c];  // 0x00
    float field1C;              // 0x1c
};

// Aim behaviour: view limits and counters at 0x14-0x22, aim flags at 0x2c and
// the flag block at 0x54.
struct GunAimBeh80179F08 {
    unsigned char unk00[0x14];          // 0x00
    unsigned short field14;             // 0x14
    unsigned short field16;             // 0x16
    unsigned short field18;             // 0x18
    unsigned short field1A;             // 0x1a
    unsigned short field1C;             // 0x1c
    unsigned char unk1E[0x22 - 0x1e];   // 0x1e
    unsigned short field22;             // 0x22
    unsigned char unk24[0x2c - 0x24];   // 0x24
    unsigned int flags;                 // 0x2c
    int field30;                        // 0x30
    int field34;                        // 0x34
    unsigned char unk38[0x54 - 0x38];   // 0x38
    GunAimBlock80179F08* block;         // 0x54
};

extern float lbl_804D81E0;

float OwnerValue(Owner* owner, void* target);

// Name is a guess: second aim-target update of the gun behaviour tables. Ends and
// restarts the behaviour, picks up its follow target and copies the target id;
// then checks whether the target lies inside the behaviour's view limits and
// sets the aim flags from the result.
void GunAimTargetUpdate80179F08(void* ownerArg, void* behArg) {
    GunAimActor80179F08* owner = (GunAimActor80179F08*)ownerArg;
    GunAimBeh80179F08* beh;
    GunAimTarget80179F08* target;
    GunAimBlock80179F08* block;
    Vec3 delta;
    Vec3 axis;
    Mtx34 mtx;
    float angle;

    if (behArg == 0) {
        return;
    }
    beh = (GunAimBeh80179F08*)behArg;
    block = beh->block;
    BehDone(beh);
    BehReset(beh);
    target = (GunAimTarget80179F08*)FollowGetTargetObj(&owner->follow);
    beh->flags |= 1;
    beh->flags |= 2;
    if (target == 0) {
        return;
    }
    beh->field30 = 2;
    beh->field34 = target->field38;

    delta.x = (float)target->posX - (float)owner->posX;
    delta.y = (float)target->posY - (float)owner->posY;
    delta.z = (float)owner->posZ - (float)owner->posZ;

    axis.x = lbl_804D81A4;
    axis.y = lbl_804D81B0;
    axis.z = lbl_804D81A4;
    ActorGetMatrix(&mtx, (Actor*)owner);
    fn_800A0168(&mtx, &mtx);
    fn_800A0030(&axis, &mtx, &axis);
    GunNormalizeAimVec80179268(&axis);
    GunNormalizeAimVec80179268(&delta);

    if (axis.z * delta.z + axis.x * delta.x + axis.y * delta.y >
        lbl_804D81E0) {
        angle = OwnerValue((Owner*)owner, target);
        if (angle <= (float)beh->field16 && (float)beh->field14 <= angle) {
            beh->flags |= 4;
        }
        if (block->field1C <= lbl_804D81A4 &&
            angle <= (float)beh->field1A &&
            (float)beh->field18 <= angle) {
            beh->flags |= 8;
        }
        if ((beh->flags & 4) == 0 && (beh->flags & 8) == 0) {
            beh->flags |= 0x80;
        }
        if ((float)beh->field1C <= angle) {
            if (angle > (float)beh->field22) {
                beh->flags |= 0x800;
            }
        } else {
            beh->flags |= 0x400;
        }
    }
}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x8017A384.
void GunAimNoOp8017A384(void) {}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x8017A388.
void GunAimNoOp8017A388(void) {}

// Name is a guess: another gun-aim behaviour callback. Releases the live effect
// handle held in the behaviour block and clears it so it is released only once
// (address 0x8017A38C).
void GunAimBehRelease8017A38C(void* ownerArg, void* behArg) {
    GunFxBeh80177A2C* beh;
    GunFxBlock80177A2C* block;

    if (behArg != 0) {
        beh = (GunFxBeh80177A2C*)behArg;
        block = beh->block;
        if (block->effect != -1) {
            fn_80027690(block->effect);
            block->effect = -1;
        }
    }
}

// Name is a guess: same shape as the other gun-aim behaviour release callbacks.
// Releases the live effect handle held in the behaviour block and clears it so it
// is released only once (address 0x8017A3D4).
void GunAimBehRelease8017A3D4(void* ownerArg, void* behArg) {
    GunAimFxBeh801798FC* beh;
    GunAimFxBlock801798FC* block;

    if (behArg != 0) {
        beh = (GunAimFxBeh801798FC*)behArg;
        block = beh->block;
        if (block->effect != -1) {
            fn_80027690(block->effect);
            block->effect = -1;
        }
    }
}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x8017A41C.
void GunAimNoOp8017A41C(void) {}

// Name is a guess: empty gun-aim state callback, retail body is a bare `blr`;
// address 0x8017A420.
void GunAimNoOp8017A420(void) {}

// Name is a guess: clears the aim block's 0x08 word and ends the behaviour;
// address 0x8017A424.
void GunAimStateDone8017A424(void* owner, void* beh) {
    GunFlagOwner80176CF4* o = (GunFlagOwner80176CF4*)beh;
    GunFlagBlock80176CF4* b;

    if (o == 0) {
        return;
    }
    b = o->flags;
    b->flags = 0;
    BehDone(beh);
}

// Per-behaviour block: the state/flag word at 0x08 and the live effect handle
// at 0x0c (names are guesses).
struct GunSpawnBlock8017A45C {
    unsigned char unk00[0x08];  // 0x00
    int flags;                  // 0x08
    int effect;                 // 0x0c
};

// The behaviour object; only its +0x54 block pointer is used.
struct GunSpawnBeh8017A45C {
    unsigned char unk00[0x54];               // 0x00
    GunSpawnBlock8017A45C* block;            // 0x54
};

// Name is a guess: clears the behaviour block's flag word and ends the
// behaviour; address 0x8017A45C.
void GunSpawnStateDone8017A45C(void* ownerArg, void* behArg) {
    GunSpawnBeh8017A45C* beh;
    GunSpawnBlock8017A45C* block;

    if (behArg != 0) {
        beh = (GunSpawnBeh8017A45C*)behArg;
        block = beh->block;
        block->flags = 0;
        BehDone(beh);
    }
}

// Actor-side message target shared by the owner and the player: a callback at
// +0x30 receives the message id and the payload block (names are guesses).
struct GunMsgTarget8017A494 {
    unsigned char unk00[0x30];             // 0x00
    void (*send)(int msg, void* payload);  // 0x30
};

// The gun actor that owns the behaviour: the message callback at 0x30, the
// position at 0x84, the model at 0xcc, the muzzle bones at 0xd8/0xdc and the
// animation at 0xf0 (names are guesses).
struct GunSpawnOwner8017A494 {
    GunMsgTarget8017A494 target;         // 0x00
    unsigned char unk34[0x84 - 0x34];    // 0x34
    Vec3 pos;                            // 0x84
    unsigned char unk90[0xcc - 0x90];    // 0x90
    int model;                           // 0xcc
    unsigned char unkD0[0xd8 - 0xd0];    // 0xd0
    int boneA;                           // 0xd8
    int boneB;                           // 0xdc
    unsigned char unkE0[0xf0 - 0xe0];    // 0xe0
    int anim;                            // 0xf0
};

// Message payload: the target object at 0x00 and the reply flags at 0x18.
struct GunMsgPayload8017A494 {
    GunMsgTarget8017A494* target;        // 0x00
    unsigned char unk04[0x18 - 0x04];    // 0x04
    unsigned int flags;                  // 0x18
    unsigned char unk1C[0x20 - 0x1C];    // 0x1c
};

GunMsgTarget8017A494* GetPlayer();

// Name is a guess: spawn callback of the gun effect behaviour. Clears the
// block's flag word, ends the behaviour and then - when no menu is open and no
// effect is running - asks the owner and the player for their reply flags; when
// they overlap it starts a muzzle effect at the model bone (or, for an actor
// without that bone, at the owner position) with a random lifetime and
// randomised colour/parameter vector. The effect handle is kept in the block.
void GunSpawnStart8017A494(void* ownerArg, void* behArg) {
    GunSpawnOwner8017A494* owner = (GunSpawnOwner8017A494*)ownerArg;
    GunSpawnBeh8017A45C* beh;
    GunSpawnBlock8017A45C* block;
    GunMsgPayload8017A494 ownerMsg;
    GunMsgPayload8017A494 playerMsg;
    Vec3 pos;
    float params[4];
    Mtx34 mtx;
    int bone;
    int delay;

    if (behArg != 0) {
        beh = (GunSpawnBeh8017A45C*)behArg;
        block = beh->block;
        block->flags = 0;
        BehDone(beh);
        if (fn_800BA7FC(0x29) == 0 && block->effect == -1) {
            ownerMsg.target = &owner->target;
            playerMsg.target = GetPlayer();
            ownerMsg.target->send(0x400, &ownerMsg);
            playerMsg.target->send(0x400, &playerMsg);
            if ((ownerMsg.flags & playerMsg.flags) != 0) {
                bone = ModelFindBone(owner->model, owner->boneA);
                if (bone == -1) {
                    bone = ModelFindBone(owner->model, owner->boneB);
                }
                if (bone != -1) {
                    Mtx34* mats = (Mtx34*)fn_8003A3F0(
                        owner->model, owner->anim,
                        ModelAnimate(owner->model, 0, 0));
                    ActorGetMatrix(&mtx, (Actor*)&mats[bone]);
                    pos.x = mtx.trans[0];
                    pos.y = mtx.trans[1];
                    pos.z = mtx.trans[2];
                } else {
                    pos.x = owner->pos.x;
                    pos.y = owner->pos.y;
                    pos.z = owner->pos.z;
                }
                params[0] = lbl_804D81B0;
                params[1] = lbl_804D81DC *
                                ((float)(unsigned)rand() / lbl_804D81D0) +
                            lbl_804D81DC;
                params[2] = lbl_804D81A4;
                params[3] = lbl_804D81B0;
                delay = (int)(lbl_804D81D8 *
                                  ((float)(unsigned)rand() / lbl_804D81D0) +
                              lbl_804D81D4);
                block->effect = fn_80027A58(1, &pos, params, delay, 9);
            }
        }
    }
}

// The gun actor shared by the two spawn callbacks below: the message target at
// 0x00, the position at 0x84, the model at 0xcc, the muzzle bones at 0xd8/0xdc,
// the animation at 0xf0, the frame time at 0x138 and the message-handler pair at
// 0x8ec/0x8f0 (names are guesses).
struct GunSpawnOwner8017A698 {
    GunMsgTarget8017A494 target;         // 0x00
    unsigned char unk34[0x84 - 0x34];    // 0x34
    Vec3 pos;                            // 0x84
    unsigned char unk90[0xcc - 0x90];    // 0x90
    int model;                           // 0xcc
    unsigned char unkD0[0xd8 - 0xd0];    // 0xd0
    int boneA;                           // 0xd8
    int boneB;                           // 0xdc
    unsigned char unkE0[0xf0 - 0xe0];    // 0xe0
    int anim;                            // 0xf0
    unsigned char unkF4[0x138 - 0xf4];   // 0xf4
    float field138;                      // 0x138
    unsigned char unk13C[0x8ec - 0x13c]; // 0x13c
    int field8EC;                        // 0x8ec
    int field8F0;                        // 0x8f0
};

// Name is a guess: second spawn callback of the gun effect behaviour. Like the
// other spawn callback it clears the block's flag word, ends the behaviour and -
// when no menu is open and no effect is running - asks owner and player for their
// reply flags and starts a muzzle effect at the model bone (or the owner position)
// with a random colour parameter and delay. It then sends the aim-matrix payload
// on when the spawn message answers with the start reply.
void GunSpawnStart8017A698(Owner* ownerArg, void* behArg) {
    GunSpawnOwner8017A698* owner = (GunSpawnOwner8017A698*)ownerArg;
    GunSpawnBeh8017A45C* beh;
    GunSpawnBlock8017A45C* block;
    GunMsgPayload8017A494 ownerMsg;
    GunMsgPayload8017A494 playerMsg;
    unsigned char payload[0x60];
    Vec3 pos;
    float params[4];
    Mtx34 mtx;
    Mtx34* mats;
    float ratio;
    int bone;
    int delay;

    if (behArg == 0) {
        return;
    }
    beh = (GunSpawnBeh8017A45C*)behArg;
    block = beh->block;
    block->flags = 0;
    BehDone(beh);
    if (fn_800BA7FC(0x29) == 0 && block->effect != -1) {
        ownerMsg.target = &owner->target;
        playerMsg.target = GetPlayer();
        ownerMsg.target->send(0x400, &ownerMsg);
        playerMsg.target->send(0x400, &playerMsg);
        if ((ownerMsg.flags & playerMsg.flags) != 0) {
            bone = ModelFindBone(owner->model, owner->boneA);
            if (bone == -1) {
                bone = ModelFindBone(owner->model, owner->boneB);
            }
            if (bone != -1) {
                mats = (Mtx34*)fn_8003A3F0(owner->model, owner->anim,
                                            ModelAnimate(owner->model, 0, 0));
                ActorGetMatrix(&mtx, (Actor*)&mats[bone]);
                pos.x = mtx.trans[0];
                pos.y = mtx.trans[1];
                pos.z = mtx.trans[2];
            } else {
                pos.x = owner->pos.x;
                pos.y = owner->pos.y;
                pos.z = owner->pos.z;
            }
            ratio = (float)(unsigned)rand() / lbl_804D81D0;
            params[0] = lbl_804D81B0;
            params[1] = lbl_804D81DC * ratio + lbl_804D81DC;
            params[2] = lbl_804D81A4;
            params[3] = lbl_804D81B0;
            delay = (int)(lbl_804D81D8 *
                              ((float)(unsigned)rand() / lbl_804D81D0) +
                          lbl_804D81D4);
            block->effect = fn_80027A58(1, &pos, params, delay, 9);
        }
    }
    if (Fn800F7DEC(owner->field8EC, owner->field8F0, 0x20ee36, OwnerId(ownerArg),
                   0) == 0x04a3e183) {
        block->flags |= 2;
        if (GunAimMatrixUpdate80179268(lbl_804D81A4, ownerArg, (Beh*)payload) !=
            0) {
            Fn800F7F5C(owner->field8EC, owner->field8F0, 0x20ee36,
                       OwnerId(ownerArg), payload, owner->field138);
        }
    }
}

// The block of the random spawn timer below: the flag word at 0x08, the spawn
// range at 0x10/0x14 and the rolled spawn time at 0x18 (names are guesses).
struct GunSpawnTimerBlock8017A918 {
    unsigned char unk00[0x08];  // 0x00
    int flags;                  // 0x08
    int field0C;                // 0x0c
    int min;                    // 0x10
    int max;                    // 0x14
    float time;                 // 0x18
};

// The behaviour object; only its +0x54 block pointer is used.
struct GunSpawnTimerBeh8017A918 {
    unsigned char unk00[0x54];                    // 0x00
    GunSpawnTimerBlock8017A918* block;            // 0x54
};

// Name is a guess: spawn-timer callback of the gun effect behaviour. Clears the
// block's flag word, ends the behaviour and rolls a spawn time between the
// block's range limits; when the spawn message answers with the start reply it
// sends the aim payload on.
void GunSpawnTimer8017A918(Owner* ownerArg, void* behArg) {
    GunSpawnOwner8017A698* owner = (GunSpawnOwner8017A698*)ownerArg;
    GunSpawnTimerBeh8017A918* beh;
    GunSpawnTimerBlock8017A918* block;
    unsigned char payload[0x60];
    float ratio;

    if (behArg == 0) {
        return;
    }
    beh = (GunSpawnTimerBeh8017A918*)behArg;
    block = beh->block;
    block->flags = 0;
    BehDone(beh);
    ratio = (float)(unsigned)rand() / lbl_804D81D0;
    block->time = (float)(block->max - block->min) * ratio;
    block->time += (float)block->min;
    if (Fn800F7DEC(owner->field8EC, owner->field8F0, 0x96224fcc, OwnerId(ownerArg),
                   0) == 0x04a3e183 &&
        Fn80161FF4(ownerArg, payload, lbl_804D81A4) != 0) {
        Fn800F7F5C(owner->field8EC, owner->field8F0, 0x96224fcc,
                   OwnerId(ownerArg), payload, owner->field138);
    }
}

// The aim behaviour of the two callbacks below: the aim flags at 0x2c and the
// flag block pointer at 0x54 (offsets come from retail; names are guesses).
struct GunAimBeh8017AA48 {
    unsigned char unk00[0x2c];          // 0x00
    unsigned int flags;                 // 0x2c
    unsigned char unk30[0x54 - 0x30];   // 0x30
    GunFlagBlock80176CF4* block;        // 0x54
};

// Name is a guess: ends the aim behaviour and sets its 0x10/0x20 aim flags when
// the owner answers the two aim messages with the done reply.
void GunAimFlagsDone8017AA48(void* ownerArg, void* behArg) {
    GunSpawnOwner8017A698* owner;
    GunAimBeh8017AA48* beh;

    if (behArg == 0) {
        return;
    }
    owner = (GunSpawnOwner8017A698*)ownerArg;
    beh = (GunAimBeh8017AA48*)behArg;
    beh->block->flags = 0;
    BehDone(beh);
    if (Fn800F7DEC(owner->field8EC, owner->field8F0, 0xe6b71bcc,
                   OwnerId((Owner*)ownerArg), 0) == 0x04a3e183) {
        beh->flags |= 0x10;
    }
    if (Fn800F7DEC(owner->field8EC, owner->field8F0, 0x968cbe3d,
                   OwnerId((Owner*)ownerArg), 0) == 0x04a3e183) {
        beh->flags |= 0x20;
    }
}

// Name is a guess: no-op callback of an aim behaviour table.
// fn_8017AB10
extern "C" int fn_8017AB10(void) {
    return 0;
}

extern float lbl_804D81E4;
extern float lbl_804D81E8;

// Placeholder name kept until other units are checked: weapon behaviour message
// handler of the second gun behaviour table (GunAimMsg8017AB18 is the
// descriptive name). Picks the next state from the message id and handles the
// aimed-shot and release messages; returns 0 only when the behaviour object is
// missing or the payload request failed.
extern "C" int fn_8017AB18(void* ownerArg, void* behArg, int msg) {
    GunMsgOwner8017744C* owner;
    GunMsgBlock8017744C* block;
    GunMsgPayload8017744C payload;
    int reply;

    if (behArg == 0) {
        return 0;
    }
    owner = (GunMsgOwner8017744C*)ownerArg;
    block = ((GunMsgBeh8017744C*)behArg)->block;
    switch (msg) {
    case 1:
        if (block->state == 0) {
            block->next = 1;
        }
        break;
    case 2:
        if (block->state == 0) {
            block->next = 5;
        }
        break;
    case 3:
        if (block->state == 0) {
            block->next = 6;
        }
        break;
    case 4:
        if (block->state == 0) {
            block->next = 7;
        }
        break;
    case 0x20ee36:
        break;
    case 0x1b83833f:
        block->flags |= 0x10;
        break;
    case 0xe6b71bcc:
    case 0x968cbe3d:
        if ((block->flags & 0x20) == 0) {
            block->flags |= 0x20;
            Fn800F7F5C(owner->field8EC, owner->field8F0, msg,
                       OwnerId((Owner*)ownerArg), 0, owner->field138);
        }
        break;
    case 0xf829db93:
        if ((block->flags & 0x40) == 0) {
            reply = Fn800F7DEC(owner->field8EC, owner->field8F0, 0xf829db93,
                               OwnerId((Owner*)ownerArg), 0);
            if (reply == 0x04a3e183) {
                block->flags |= 0x40;
                if (Fn80161FF4((Owner*)ownerArg, &payload, lbl_804D81E4) == 0) {
                    return 0;
                }
                Fn800F7F5C(owner->field8EC, owner->field8F0, 0xf829db93,
                           OwnerId((Owner*)ownerArg), &payload,
                           owner->field138);
                block->time = lbl_804D81E8;
            }
        }
        break;
    }
    return 1;
}

// Per-behaviour state block of the update below (time field at 0x1c here).
struct GunStateBlock8017AD38 {
    int state;                           // 0x00 current state
    int next;                            // 0x04 requested state
    unsigned int flags;                  // 0x08
    int field0C;                         // 0x0c effect handle, -1 when idle
    int field10;                         // 0x10
    int field14;                         // 0x14
    float field18;                       // 0x18
    float time;                          // 0x1c
};

// The behaviour object; +0x10 receives the new state, +0x54 holds the block.
struct GunStateBeh8017AD38 {
    unsigned char unk00[0x10];           // 0x00
    int field10;                         // 0x10
    unsigned char unk14[0x54 - 0x14];    // 0x14
    GunStateBlock8017AD38* block;        // 0x54
};

extern GunStateEntry80177694 lbl_803A8260[];
extern float lbl_804D81A4;

// Placeholder name kept until other units are checked: per-frame behaviour
// update of the second gun behaviour table (GunAimStateUpdate8017AD38 is the
// descriptive name). Runs the current state's update callback, then - when a
// new state was requested - the change callback, the state copy and the new
// state's enter callback.
extern "C" int fn_8017AD38(void* ownerArg, void* behArg) {
    GunMsgOwner8017744C* owner;
    GunStateBeh8017AD38* beh;
    GunStateBlock8017AD38* block;

    if (behArg == 0) {
        return 0;
    }
    owner = (GunMsgOwner8017744C*)ownerArg;
    beh = (GunStateBeh8017AD38*)behArg;
    block = beh->block;
    if (block->time >= lbl_804D81A4) {
        block->time -= owner->field138;
    }
    if (lbl_803A8260[block->state].update != 0) {
        lbl_803A8260[block->state].update(ownerArg);
    }
    if (block->next != block->state) {
        if (lbl_803A8260[block->state].change != 0) {
            lbl_803A8260[block->state].change(ownerArg, behArg);
        }
        block->state = block->next;
        beh->field10 = block->next;
        if (lbl_803A8260[block->state].enter != 0) {
            lbl_803A8260[block->state].enter(ownerArg, behArg);
        }
    }
    return 1;
}

// Name is a guess: stops the effect recorded in the state block and frees the
// block hung off +0x54 of the behaviour object (GunStateBlockFree8017AE50).
// Placeholder name kept: u_80161F40.cpp's behaviour table references it.
extern "C" int fn_8017AE50(void* ownerArg, void* behArg) {
    GunStateBeh8017AD38* beh;
    GunStateBlock8017AD38* block;

    if (behArg == 0) {
        return 0;
    }
    beh = (GunStateBeh8017AD38*)behArg;
    block = beh->block;
    if (block->field0C != -1) {
        fn_80027690(block->field0C);
        block->field0C = -1;
    }
    VFree(block);
    return 1;
}

// Name is a guess: allocates the 0x20-byte state block for the second gun
// behaviour table, stores it at +0x54 of the behaviour object and runs the
// initial state's enter callback (GunStateBlockCreate8017AEAC).  Placeholder
// name kept: u_80161F40.cpp's behaviour table references it.
extern "C" int fn_8017AEAC(void* ownerArg, void* behArg) {
    GunStateBeh8017AD38* beh;
    GunStateBlock8017AD38* block;

    if (behArg == 0) {
        return 0;
    }
    beh = (GunStateBeh8017AD38*)behArg;
    block = (GunStateBlock8017AD38*)VAlloc(0x20);
    if (block == 0) {
        beh->block = 0;
        return 0;
    }
    beh->block = block;
    beh->field10 = 0;
    block->state = 0;
    block->next = 0;
    block->field0C = -1;
    block->flags = 0;
    block->field10 = 400;
    block->field14 = 600;
    block->field18 = lbl_804D81A4;
    block->time = lbl_804D81A4;
    if (lbl_803A8260[block->state].enter != 0) {
        lbl_803A8260[block->state].enter(ownerArg, behArg);
    }
    return 1;
}

// Trail sample copied into a live trail entry (offsets from retail; the meaning of
// each vector is a guess).
struct GunTrailData8017AFA4 {
    Vec3 vec0;  // 0x00
    Vec3 vec1;  // 0x0c
    Vec3 vec2;  // 0x18
    Vec3 vec3;  // 0x24 position handed to the trail buffer
};

struct TrailBuffer;

void TrailBufferPush(TrailBuffer* buffer, float* position);

// Live trail entry owned by the gun effect state (offsets from retail; names are
// guesses).
struct GunTrailEntry8017AFA4 {
    int state;                          // 0x00
    int next;                           // 0x04
    unsigned char unk08[0x0c - 0x08];   // 0x08
    GunTrailData8017AFA4 data;          // 0x0c
    TrailBuffer* trail;                 // 0x3c
};

extern float lbl_804D81F0;

// Advances the entry from state 1 to state 2 (address 0x8017AF78).
void GunTrailStateStart8017AF78(void* block) {
    GunTrailEntry8017AFA4* entry;
    if (!block) {
        return;
    }
    entry = (GunTrailEntry8017AFA4*)block;
    if (!block) {
        return;
    }
    if (entry->state != 1) {
        return;
    }
    entry->state = 2;
    entry->next = 0;
}

// Copies one trail sample into the entry and pushes it to the trail buffer
// (address 0x8017AFA4).
void GunTrailEntryInit8017AFA4(void* block, void* sample) {
    GunTrailEntry8017AFA4* entry;
    GunTrailData8017AFA4* data;
    if (!block) {
        return;
    }
    if (!sample) {
        return;
    }
    entry = (GunTrailEntry8017AFA4*)block;
    if (!block) {
        return;
    }
    if (entry->state != 1) {
        return;
    }
    data = (GunTrailData8017AFA4*)sample;
    entry->data = *data;
    entry->data.vec3.z -= lbl_804D81F0;
    TrailBufferPush(entry->trail, &entry->data.vec3.x);
}

// Effect preset (0x3c bytes each) of the trail effects below: two line-fan
// passes with their own colour ramp, then a ring pass (offsets from retail).
// One draw pass of a trail preset (0x14 bytes).
struct GunTrailPass {
    int mode;                           // 0x00 texture/mode
    float width;                        // 0x04
    unsigned char unk08[4];             // 0x08
    unsigned char color[8];             // 0x0c ring buffer colours (end + base)
};

struct GunTrailFx8017B060 {
    int field00;                        // 0x00 texture/mode of the first fan
    float field04;                      // 0x04 width of the first fan
    float scaleA;                       // 0x08 vertex scale of the first fan
    unsigned char colorA[8];            // 0x0c colour ramp of the first fan
    int field14;                        // 0x14 texture/mode of the second fan
    float field18;                      // 0x18 width of the second fan
    float scaleB;                       // 0x1c vertex scale of the second fan
    unsigned char colorB[8];            // 0x20 colour ramp of the second fan
    GunTrailPass ring;                  // 0x28 ring pass
};

// One live trail effect (0xf0 bytes): state, lifetime counter, effect preset
// index, the 0x30-byte draw link, the ring buffer and its 36 vertices.
struct GunTrailBlock8017B060 {
    int state;                          // 0x00
    int counter;                        // 0x04
    int effect;                         // 0x08
    unsigned char link[0x30];           // 0x0c
    unsigned char trail[0x24];          // 0x3c ring buffer (TrailBuffer)
    float points[36];                   // 0x60
};

// Growing array of live trail effects; the size at 0x08 is in bytes and the
// entries are 4-byte pointers.
struct GunTrailList8017B060 {
    virtual ~GunTrailList8017B060();    // 0x00: vtable
    unsigned int unk04;                 // 0x04
    unsigned int size;                  // 0x08
    void* data;                         // 0x0c
};

extern GunTrailList8017B060* lbl_804D5048;
extern GunTrailFx8017B060 lbl_803A82C0[];
// Shared trail geometry table: 12 base points, 12 angle offsets and the
// per-frame scale factors derived from them.
struct GunTrailGeometry8017B488 {
    float points[36];                   // 0x00
    int angleOffsets[12];               // 0x90
    float scaleY[12];                   // 0xc0
    float scaleXZ[12];                  // 0xf0
};

extern GunTrailGeometry8017B488 lbl_80491760;
extern float lbl_804D81F4;
extern float lbl_804D81F8;
extern float lbl_804D81FC;
extern float lbl_804D8200;

void LinkInfoInit(void* info);
void TrailBufferInit(TrailBuffer* buffer, float* points, unsigned long max,
                     unsigned long rEnd, unsigned long gEnd, unsigned long bEnd,
                     unsigned long aEnd, unsigned char rBase, unsigned char gBase,
                     unsigned char bBase, unsigned char aBase);
void Draw3DBeginLine(int texture, void* link, float width);
void DrawLine(float* from, float* to, unsigned char c0, unsigned char c1,
              unsigned char c2, unsigned char c3, unsigned char c4,
              unsigned char c5, unsigned char c6, unsigned char c7);
void DrawDisableTexOffsets(void);
void DrawTrailRing(TrailBuffer* buffer, unsigned char r, unsigned char g,
                   unsigned char b, unsigned char a);
extern "C" void fn_8028B35C(void* list, void* item, int size);
extern "C" void* fn_8028B3BC(void* list, void* end, int size);

// Name is a guess: allocates a trail effect for the given effect preset and
// registers it in the live list, dropping the oldest effects once 32 are live.
GunTrailBlock8017B060* GunTrailSpawn8017B060(int effect) {
    GunTrailBlock8017B060* block;
    GunTrailFx8017B060* fx;

    while ((lbl_804D5048->size >> 2) >= 0x20) {
        VFree(*(void**)lbl_804D5048->data);
        fn_8028B35C(lbl_804D5048, lbl_804D5048->data, 4);
    }
    block = (GunTrailBlock8017B060*)VAlloc(0xf0);
    if (block != 0) {
        *(GunTrailBlock8017B060**)fn_8028B3BC(
            lbl_804D5048,
            (unsigned char*)lbl_804D5048->data + lbl_804D5048->size, 4) = block;
        block->state = 1;
        block->counter = 0;
        block->effect = effect;
        LinkInfoInit(block->link);
        fx = &lbl_803A82C0[block->effect];
        GunTrailPass* ring = &fx->ring;
        TrailBufferInit((TrailBuffer*)block->trail, block->points, 12,
                        ring->color[0], ring->color[1], ring->color[2],
                        ring->color[3], ring->color[4], ring->color[5],
                        ring->color[6], ring->color[7]);
    }
    return block;
}

// Placeholder name kept: u_80157730.cpp calls it. Per-frame update of the live
// trail effects: draws the live ones from their effect preset, fades the dying
// ones out and frees the effects whose 300 frames are over.
extern "C" void fn_8017B164(void) {
    GunTrailBlock8017B060* block;
    GunTrailFx8017B060* fx;
    void** slot;
    float value;
    float pos[3];
    float base[3];
    int dead;
    int i;
    unsigned char alpha;

    value = lbl_804D81F4;
    slot = (void**)lbl_804D5048->data;
    while (slot != (void**)((unsigned char*)lbl_804D5048->data +
                            lbl_804D5048->size)) {
        block = (GunTrailBlock8017B060*)*slot;
        dead = 0;
        base[0] = value;
        base[1] = value;
        base[2] = value;
        if (block->state == 2) {
            if (block->counter < 300) {
                alpha = (unsigned char)(int)(lbl_804D81F8 *
                        (lbl_804D81FC - (float)block->counter / lbl_804D8200));
                fx = &lbl_803A82C0[block->effect];
                Draw3DBeginLine(fx->ring.mode, 0, fx->ring.width);
                DrawTrailRing((TrailBuffer*)block->trail, alpha, alpha, alpha,
                              alpha);
                DrawDisableTexOffsets();
            } else {
                dead = 1;
            }
        } else if (block->state == 1) {
            fx = &lbl_803A82C0[block->effect];
            Draw3DBeginLine(fx->field00, block->link, fx->field04);
            for (i = 0; i < 12; i++) {
                pos[0] = lbl_80491760.points[i * 3 + 0] * lbl_80491760.scaleXZ[i] *
                         fx->scaleA;
                pos[2] = lbl_80491760.points[i * 3 + 2] * lbl_80491760.scaleXZ[i] *
                         fx->scaleA;
                pos[1] = lbl_80491760.points[i * 3 + 1] * lbl_80491760.scaleY[i] *
                         fx->scaleA;
                DrawLine(base, pos, fx->colorA[0], fx->colorA[1], fx->colorA[2],
                         fx->colorA[3], fx->colorA[4], fx->colorA[5],
                         fx->colorA[6], fx->colorA[7]);
            }
            DrawDisableTexOffsets();
            fx = &lbl_803A82C0[block->effect];
            Draw3DBeginLine(fx->field14, block->link, fx->field18);
            for (i = 0; i < 12; i++) {
                pos[0] = lbl_80491760.points[i * 3 + 0] * lbl_80491760.scaleXZ[i] *
                         fx->scaleB;
                pos[2] = lbl_80491760.points[i * 3 + 2] * lbl_80491760.scaleXZ[i] *
                         fx->scaleB;
                pos[1] = lbl_80491760.points[i * 3 + 1] * lbl_80491760.scaleY[i] *
                         fx->scaleB;
                DrawLine(base, pos, fx->colorB[0], fx->colorB[1], fx->colorB[2],
                         fx->colorB[3], fx->colorB[4], fx->colorB[5],
                         fx->colorB[6], fx->colorB[7]);
            }
            DrawDisableTexOffsets();
            fx = &lbl_803A82C0[block->effect];
            Draw3DBeginLine(fx->ring.mode, 0, fx->ring.width);
            DrawTrailRing((TrailBuffer*)block->trail, 0xFF, 0xFF, 0xFF, 0xFF);
            DrawDisableTexOffsets();
        }
        if (dead) {
            VFree(block);
            fn_8028B35C(lbl_804D5048, slot, 4);
        } else {
            slot++;
        }
    }
}

extern int lbl_804D504C;
extern signed char lbl_804D5050;
extern float lbl_804D8210;
extern float lbl_804D8214;
extern float lbl_804D8218;
extern float lbl_804D821C;
extern float lbl_804D8220;
extern float lbl_804D8224;

// Per-frame update of the live trail effects: advances the shared angle table
// and the effect lifetimes (address 0x8017B488). Name is a guess.
extern "C" void fn_8017B488(float dt) {
    GunTrailBlock8017B060** slot;
    GunTrailBlock8017B060* block;
    float value;
    float sinValue;
    float cosValue;
    int counter;
    int angle;
    int frames;
    int i;

    slot = (GunTrailBlock8017B060**)lbl_804D5048->data;
    if (lbl_804D5050 == 0) {
        lbl_804D504C = 0;
        lbl_804D5050 = 1;
    }
    lbl_804D504C += (int)(lbl_804D8210 * dt);
    for (i = 0; i < 12; i++) {
        counter = lbl_804D504C + lbl_80491760.angleOffsets[i];
        angle = counter % 360 - 180;
        value = lbl_804D8214 * (float)angle / lbl_804D8218;
        if (value < lbl_804D81F4) {
            value = value * lbl_804D821C;
        } else {
            value = value * lbl_804D8220;
        }
        lbl_80491760.scaleY[i] = value;
        sinValue = fn_800A1394(lbl_804D8220 * (float)counter);
        cosValue = fn_800A1394(lbl_804D8224 * (float)counter);
        lbl_80491760.scaleXZ[i] = cosValue * sinValue;
    }
    frames = (int)dt;
    while (slot != (GunTrailBlock8017B060**)((unsigned char*)lbl_804D5048->data +
                                             lbl_804D5048->size)) {
        block = *slot;
        if (block != 0) {
            block->counter += frames;
        }
        slot++;
    }
}

// Frees every live trail effect and destroys their container
// (address 0x8017B6C0). Name is a guess.
extern "C" void fn_8017B6C0(void) {
    while ((lbl_804D5048->size >> 2) != 0) {
        VFree(*(void**)lbl_804D5048->data);
        fn_8028B35C(lbl_804D5048, lbl_804D5048->data, 4);
    }
    delete lbl_804D5048;
}

// Shared byte-buffer base class: its destructor is out of line (0x8028B9C0) and the
// class frees its storage through VFree (declared above).
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    static void operator delete(void* p) { VFree(p); }
};

// Address 0x8028B960: per-object cleanup run from the intermediate base destructor.
// Placeholder name kept (the symbol is unnamed in retail); the call takes only `this`.
extern "C" void fn_8028B960(void* self);

// Base class of the trail-list container: its destructor is defined inline, which is
// why the derived deleting destructor below carries an extra vtable store before it
// reaches Buffer's destructor.
class GunTrailNodeBase : public Buffer {
public:
    virtual ~GunTrailNodeBase() {
        fn_8028B960(this);
    }
};

// Trail-list container; retail deleting destructor at 0x8017B728.
// The `if (flag > 0) VFree(this)` tail is the compiler-generated delete path through
// Buffer::operator delete; source is just an empty destructor body.
class GunTrailNode : public GunTrailNodeBase {
public:
    virtual ~GunTrailNode();
};

GunTrailNode::~GunTrailNode() {
}

// Name is a guess: empty helper, retail body is a bare `blr`; address 0x8017B7A0.
// fn_8017B7A0
extern "C" void fn_8017B7A0(void) {
}

// Live-effect list container. The intermediate base's deleting destructor is out of
// line at 0x8017B8B8: vptr reset, per-object cleanup, the Buffer destructor and the
// compiler-generated VFree delete path. Its vtable is 0x803A8374.
class GunTrailListBase : public Buffer {
public:
    virtual ~GunTrailListBase();

    unsigned int unk04;                 // 0x04
    unsigned int size;                  // 0x08 bytes in `data`
    void* data;                         // 0x0c
    unsigned int unk10;                 // 0x10
};

GunTrailListBase::~GunTrailListBase() {
    fn_8028B960(this);
}

// Most-derived container; its inline constructor chain (Buffer's out-of-line
// constructor followed by the two vptr stores) is what 0x8017B7A4 expands, and its
// allocation goes through VAlloc.
class GunTrailList : public GunTrailListBase {
public:
    static void* operator new(unsigned long size) { return VAlloc((int)size); }
};

// Builds the shared trail geometry table (12 base vectors at 30-degree steps as
// (sin, 1, cos), plus their offsets into the source table) and allocates the
// live-effect list container (address 0x8017B7A4). Name is a guess.
// fn_8017B7A4
extern "C" void fn_8017B7A4(void) {
    GunTrailList* list;
    int angle;
    int i;

    list = new GunTrailList;
    lbl_804D5048 = (GunTrailList8017B060*)list;
    angle = 0;
    for (i = 0; i < 12; i++) {
        lbl_80491760.points[i * 3 + 0] = fn_800A1418((float)angle);
        lbl_80491760.points[i * 3 + 1] = lbl_804D81FC;
        lbl_80491760.points[i * 3 + 2] = fn_800A1394((float)angle);
        lbl_80491760.angleOffsets[i] = (i % 3) * 120 + angle;
        angle += 30;
    }
}

// Shared callback lock flag (plain C global, no mangling).
extern int sCbLock;

// External helpers used by the range wrappers below (still placeholder names).
extern "C" void fn_8028BC2C(int ctx);
extern "C" void fn_8028BC28(int ctx, int ptr, unsigned long size);
void GunRangeWalk8017B9F0(unsigned long start, unsigned long end);

void GunRangeNoOp8017B9D0(unsigned long start, unsigned long end);

// 8017B920 - descriptive name (guess): runs a context prologue on `ctx`, then
// walks the word-aligned range [ptr, ptr + (size & ~3)).
void GunRangeOp8017B920(int ctx, int ptr, unsigned long size) {
    fn_8028BC2C(ctx);
    GunRangeNoOp8017B9D0(ptr, ptr + (size & ~3));
}

// 8017B968 - descriptive name (guess): walks the word-aligned range
// [ptr, ptr + (size & ~3)) first, then hands (ctx, ptr, size) to the runtime.
void GunRangeOp8017B968(int ctx, int ptr, unsigned long size) {
    GunRangeWalk8017B9F0(ptr, ptr + (size & ~3));
    fn_8028BC28(ctx, ptr, size);
}

// 8017B9C8 - descriptive name (guess): returns the element size (4 bytes) of
// the word range walked by the helpers below.
unsigned long GunRangeElemSize8017B9C8(void) {
    return 4;
}

// 8017B9D0 - descriptive name (guess): walks the word-aligned range from
// `start` to `end` doing no per-element work (a no-op range walk).
void GunRangeNoOp8017B9D0(unsigned long start, unsigned long end) {
    for (; start < end; start += 4) {
    }
}

// 8017B9F0 - descriptive name (guess): walks the word-aligned range from
// `start` to `end` doing no per-element work (the `unsigned` twin of 8017B9D0).
void GunRangeWalk8017B9F0(unsigned long start, unsigned long end) {
    for (; start < end; start += 4) {
    }
}

// 8017BA10 - descriptive name (guess): takes the shared callback lock (sets it
// on first use) and reports success.
// fn_8017BA10
extern "C" int fn_8017BA10(void) {
    if (sCbLock == 0) {
        sCbLock = 1;
    }
    return 1;
}

extern "C" int fn_80012E6C(void* out, int value);
extern "C" int fn_8003CE8C(void* out, int value);
extern "C" int fn_8000F2F0(void* out, int value);
extern "C" int fn_8002FA84(void* out, int value);
extern "C" int fn_8009305C(void* out, int value);
int FileOlcSize();
extern int sLastPool;

// Name is a guess: bytes still free in the 0xa80000-byte gun pool. Returns 0
// while the pool is still being set up (sLastPool == -1). The file OLC size and
// four tracked block sizes are queried first (each helper writes its size
// through an out-parameter); the fifth helper's block is not charged here.
int GunPoolFreeSize8017BA2C(void) {
    int fileSize;
    int a;
    int b;
    int c;
    int d;
    int e;

    if (sLastPool == -1) {
        fileSize = FileOlcSize();
        fn_80012E6C(&a, 0);
        fn_8003CE8C(&b, 0);
        fn_8000F2F0(&c, 0);
        fn_8002FA84(&d, 0);
        fn_8009305C(&e, 0);
        return (0xA80000 - fileSize) - (a + b + c + d);
    }
    return 0;
}

// Name is a guess: state word plus a countdown timer, the layout reached
// through a void* by the gun behaviour callbacks.
struct GunStateTimer8017BAD0 {
    unsigned short state;               // 0x00
    unsigned char unk02[0x0c - 0x02];   // 0x02
    int timer;                          // 0x0c
};

// Name is a guess: a two-state timer tick. State 1 counts down and switches to
// state 3 when it runs out; state 2 counts down and switches to state 0. A
// negative timer is clamped to 0, and the sentinel values -9999998 and
// -9999999 freeze the tick (they are the "inactive" markers).
void GunStateTimerTick8017BAD0(void* p) {
    GunStateTimer8017BAD0* t = (GunStateTimer8017BAD0*)p;

    if (t->timer == -9999998) {
        return;
    }
    if (t->timer == -9999999) {
        return;
    }
    if (t->timer < 0) {
        t->timer = 0;
    }
    switch (t->state) {
    case 1:
        t->timer--;
        if (t->timer <= 0) {
            t->state = 3;
            t->timer = 0;
        }
        break;
    case 2:
        t->timer--;
        if (t->timer <= 0) {
            t->state = 0;
            t->timer = 0;
        }
        break;
    }
}

// Name is a guess: the behaviour block reached through void* by the two abort
// callbacks below: active state at 0x00, armed behaviour id at 0x0c.
struct GunStateAbortBlk8017BB74 {
    unsigned short state;               // 0x00
    unsigned char unk02[0x0c - 0x02];   // 0x02
    unsigned int id;                    // 0x0c
};

// Name is a guess: arms behaviour id 0xff676981 and, unless the block is idle
// (state 0), forces it to state 3.
void GunStateAbort8017BB74(void* p) {
    GunStateAbortBlk8017BB74* b = (GunStateAbortBlk8017BB74*)p;

    b->id = 0xff676981;
    if (b->state == 0) {
        return;
    }
    b->state = 3;
}

// Name is a guess: arms behaviour id 0xff676982 and, unless the block is
// already in state 1, forces it to state 2.
void GunStateAbort8017BB98(void* p) {
    GunStateAbortBlk8017BB74* b = (GunStateAbortBlk8017BB74*)p;

    b->id = 0xff676982;
    if (b->state == 1) {
        return;
    }
    b->state = 2;
}

// Name is a guess: the behaviour block this callback steps: active state at
// 0x00 and a timer/behaviour id at 0x0c.
struct GunStateStepBlk8017BBBC {
    unsigned short state;               // 0x00
    unsigned char unk02[0x0c - 0x02];   // 0x02
    int id;                             // 0x0c
};

// Name is a guess: advances the state machine of the block whose id was armed
// by GunStateAbort8017BB74/GunStateAbort8017BB98. States 0 and 3 lead into the
// busy state 2, state 1 backs out to state 2 after a tick.
void GunStateStep8017BBBC(void* p) {
    GunStateStepBlk8017BBBC* b = (GunStateStepBlk8017BBBC*)p;

    if (b->id == (int)0xff676981) {
        return;
    }
    if (b->id == (int)0xff676982) {
        return;
    }
    if (b->id < 0) {
        b->id = 0;
    }
    switch (b->state) {
    case 0:
        b->state = 2;
        b->id = 1;
        break;
    case 1:
        b->id++;
        break;
    case 2:
        b->id++;
        break;
    case 3:
        b->state = 1;
        b->id = 1;
        break;
    }
}

// Name is a guess: winds the same behaviour block down; the busy state 2 drops
// back to idle and state 1 moves to the finishing state 3.
void GunStateFinish8017BC5C(void* p) {
    GunStateStepBlk8017BBBC* b = (GunStateStepBlk8017BBBC*)p;

    if (b->id == (int)0xff676981) {
        return;
    }
    if (b->id == (int)0xff676982) {
        return;
    }
    if (b->id < 0) {
        b->id = 0;
    }
    switch (b->state) {
    case 1:
        b->state = 3;
        b->id = 0;
        break;
    case 2:
        b->state = 0;
        b->id = 0;
        break;
    }
}

// Name is a guess: the behaviour block shared by the two state-clear helpers
// below: current state at 0x00 and armed behaviour id at 0x0c.
struct GunBlockClearBlk8017BCD0 {
    unsigned short state;               // 0x00
    unsigned char unk02[0x0c - 0x02];   // 0x02
    unsigned int id;                    // 0x0c
};

// Name is a guess: returns the block's state, dropping any state below 4 back
// to idle (state 0, id 0) unless the block carries an armed behaviour id.
unsigned short GunBlockClear8017BCD0(void* p) {
    GunBlockClearBlk8017BCD0* b = (GunBlockClearBlk8017BCD0*)p;

    if (b->id == 0xff676982 || b->id == 0xff676981) {
        return b->state;
    }
    switch (b->state) {
    case 0:
    case 1:
    case 2:
    case 3:
        b->state = 0;
        b->id = 0;
        break;
    case 4:
        break;
    }
    return b->state;
}

// Name is a guess: same as above, but only the idle state 0 and the finishing
// state 3 are cleared.
unsigned short GunBlockClear8017BD20(void* p) {
    GunBlockClearBlk8017BCD0* b = (GunBlockClearBlk8017BCD0*)p;

    if (b->id == 0xff676982 || b->id == 0xff676981) {
        return b->state;
    }
    switch (b->state) {
    case 0:
    case 3:
        b->state = 0;
        b->id = 0;
        break;
    case 4:
        break;
    }
    return b->state;
}

// No-op gun hook, retail body is a bare `blr`; address 0x8017BD74.
void GunNoOp8017BD74(void) {}

// File-system load state, owned by the file unit.
extern int sFileOpen;
extern int sPendingBytes;

extern void SystemIdle(void);
extern void SystemPoll(void);

// Name is a guess: pumps the system for one frame and reports whether the file
// system still has a file open or pending bytes (a load in progress).
unsigned char SystemPumpBusy(void) {
    int busy;

    SystemIdle();
    SystemPoll();
    busy = 0;
    if (sFileOpen != 0 || sPendingBytes != 0) {
        busy = 1;
    }
    return busy;
}

extern void StreamUpdate(void);
extern void GfxFrameSwap(void);
extern void GfxFrameBegin(void);
extern void fn_8007F064(int a, int b);
extern "C" int fn_80007060(int a);

// Name is a guess: runs the stream update once, then pumps frames while a file
// load is still in progress (see SystemPumpBusy above).
// fn_8017BDC0
extern "C" void fn_8017BDC0(void) {
    StreamUpdate();
    while (SystemPumpBusy()) {
        if (fn_80007060(4) != 0) {
            GfxFrameSwap();
            fn_8007F064(0, -1);
            GfxFrameBegin();
        }
    }
}

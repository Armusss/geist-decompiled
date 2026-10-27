// u_80175D60.cpp: gun actor callbacks, no-op hooks and gun state helpers (0x80175D60-0x8017BE38).
// Names are descriptive guesses; the code is matched against the retail DOL.

void TrailDestroy(int id);

// Name is a guess: returns 0 when the top byte of an id is 2 (engine-side subtype?
// network message class?), 1 otherwise.
unsigned long Fn80175D60(unsigned long id) {
    if ((id >> 24) != 2) {
        return 1;
    }
    return 0;
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
    flash->time = lbl_804D80E4;
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

extern unsigned long lbl_804D5040[];    // 2-word live-id bitmap (.sbss)
extern GunActiveRecord lbl_80491020[];

extern "C" void fn_800C6900(unsigned long handle);
extern "C" void fn_800C6664(unsigned long handle);
void BitClear(unsigned long* bits, unsigned long id);

// Placeholder name kept: called from other units. Returns the record field for a
// live id, or 0 when the id's bit is clear in the global live-id bitmap.
extern "C" unsigned long fn_80175EFC(unsigned long id) {
    unsigned long index = id & 0xffffff;
    if ((lbl_804D5040[index >> 5] & (1 << (id & 0x1f))) != 0) {
        return lbl_80491020[index].value;
    }
    return 0;
}

// Releases the effect handle held in a live id's record and clears the id's bit in
// the global live-id bitmap. Name is a guess.
void GunDestroyLiveEffect(unsigned long id) {
    unsigned long index = id & 0xffffff;
    if ((lbl_804D5040[(int)index >> 5] & (1 << (id & 0x1f))) != 0) {
        BitClear(lbl_804D5040, index);
        fn_800C6900(lbl_80491020[index].handle);
    }
}

// Releases the effect handle of one of the first eight ids. Name is a guess.
void GunDestroyEffectSlot(unsigned long id) {
    unsigned long index = id & 0xffffff;
    if (index < 8) {
        fn_800C6664(lbl_80491020[index].handle);
    }
}

extern "C" void fn_800C6760(unsigned long handle);

// Placeholder name kept: called from other units. Releases the effect handle of one
// of the first eight ids with a different release routine than GunDestroyEffectSlot.
extern "C" void fn_80176000(unsigned long id) {
    unsigned long index = id & 0xffffff;
    if (index < 8) {
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
    case 0xc0bfcf7e:
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
        BehUpdate((Owner*)owner, (Beh*)beh);
        Fn8016295C((Owner*)owner, (Beh*)beh);
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
    block->time = lbl_804D8164;
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

// TODO fn_80178CC0 (0x8C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80178CC0(void) {}

// TODO fn_80178D4C (0x8 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80178D4C(void) {}

// TODO fn_80178D54 (0x24C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80178D54(void) {}

// TODO fn_80178FA0 (0x1B4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80178FA0(void) {}

// TODO fn_80179154 (0x5C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80179154(void) {}

// TODO fn_801791B0 (0xB8 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_801791B0(void) {}

// TODO fn_80179268 (0x63C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80179268(void) {}

// TODO fn_801798A4 (0x2C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_801798A4(void) {}

// TODO fn_801798D0 (0x2C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_801798D0(void) {}

// TODO fn_801798FC (0x244 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_801798FC(void) {}

// TODO fn_80179B40 (0x21C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80179B40(void) {}

// TODO fn_80179D5C (0x1AC bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80179D5C(void) {}

// TODO fn_80179F08 (0x47C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_80179F08(void) {}

// TODO fn_8017A384 (0x4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A384(void) {}

// TODO fn_8017A388 (0x4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A388(void) {}

// TODO fn_8017A38C (0x48 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A38C(void) {}

// TODO fn_8017A3D4 (0x48 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A3D4(void) {}

// TODO fn_8017A41C (0x4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A41C(void) {}

// TODO fn_8017A420 (0x4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A420(void) {}

// TODO fn_8017A424 (0x38 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A424(void) {}

// TODO fn_8017A45C (0x38 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A45C(void) {}

// TODO fn_8017A494 (0x204 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A494(void) {}

// TODO fn_8017A698 (0x280 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A698(void) {}

// TODO fn_8017A918 (0x130 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017A918(void) {}

// TODO fn_8017AA48 (0xC8 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017AA48(void) {}

// TODO fn_8017AB10 (0x8 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017AB10(void) {}

// TODO fn_8017AB18 (0x220 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017AB18(void) {}

// TODO fn_8017AD38 (0x118 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017AD38(void) {}

// TODO fn_8017AE50 (0x5C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017AE50(void) {}

// TODO fn_8017AEAC (0xCC bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017AEAC(void) {}

// TODO fn_8017AF78 (0x2C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017AF78(void) {}

// TODO fn_8017AFA4 (0xBC bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017AFA4(void) {}

// TODO fn_8017B060 (0x104 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B060(void) {}

// TODO fn_8017B164 (0x324 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B164(void) {}

// TODO fn_8017B488 (0x238 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B488(void) {}

// TODO fn_8017B6C0 (0x68 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B6C0(void) {}

// TODO fn_8017B728 (0x78 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B728(void) {}

// TODO fn_8017B7A0 (0x4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B7A0(void) {}

// TODO fn_8017B7A4 (0x114 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B7A4(void) {}

// TODO fn_8017B8B8 (0x68 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B8B8(void) {}

// TODO fn_8017B920 (0x48 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B920(void) {}

// TODO fn_8017B968 (0x60 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B968(void) {}

// TODO fn_8017B9C8 (0x8 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B9C8(void) {}

// TODO fn_8017B9D0 (0x20 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B9D0(void) {}

// TODO fn_8017B9F0 (0x20 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017B9F0(void) {}

// TODO fn_8017BA10 (0x1C bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BA10(void) {}

// TODO fn_8017BA2C (0xA4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BA2C(void) {}

// TODO fn_8017BAD0 (0xA4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BAD0(void) {}

// TODO fn_8017BB74 (0x24 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BB74(void) {}

// TODO fn_8017BB98 (0x24 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BB98(void) {}

// TODO fn_8017BBBC (0xA0 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BBBC(void) {}

// TODO fn_8017BC5C (0x74 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BC5C(void) {}

// TODO fn_8017BCD0 (0x50 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BCD0(void) {}

// TODO fn_8017BD20 (0x54 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BD20(void) {}

// TODO fn_8017BD74 (0x4 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BD74(void) {}

// TODO fn_8017BD78 (0x48 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BD78(void) {}

// TODO fn_8017BDC0 (0x78 bytes): not drafted yet; the empty body keeps the file building (unit is NonMatching).
extern "C" void fn_8017BDC0(void) {}

// GrenadeFx.cpp: grenade effects (0x801608A4-0x80160DB0).
// Loads the grenade sounds and texture, and keeps up to 8 "GRENADE" particle emitters, one per owner.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

struct Vec3 {
    float x, y, z;
};

// Particle emitter description (0xEC bytes) passed to EmitterCreate.
struct EmitterDesc {
    Vec3 pos;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    int unk1C;
    char pad20[4];
    float unk24;
    int unk28;
    int texture;
    unsigned char unk30;
    char pad31[0x3B];
    float unk6C;
    float unk70;
    float unk74;
    float unk78;
    int unk7C;
    float unk80;
    char pad84[4];
    int unk88;
    char pad8C[4];
    float unk90;
    float unk94;
    int unk98;
    int unk9C;
    int unkA0;
    int unkA4;
    int unkA8;
    float unkAC;
    float unkB0;
    float unkB4;
    float unkB8;
    float unkBC;
    float unkC0;
    float unkC4;
    float unkC8;
    unsigned char color[4];
    char padD0[4];
    int unkD4;
    int unkD8;
    char padDC[0x10];
};

struct EmitterSlot {
    int active;
    int handle;
    int owner;
};

extern "C" void* memset(void* dst, int val, u32 n);
int AssetFind(u32 bank, u32 name);                 // 0x8007EB34
void TextureLoad(short* id, int a);                // 0x8000F738
void EmitterDestroy(int handle);                   // 0x800D3694
int EmitterCreate(EmitterDesc* desc);              // 0x800D3D88
void EmitterAttach(int handle, int owner, const char* name); // 0x800D35B0

int gGrenadeSounds[16];
EmitterSlot gGrenadeEmitters[8];
short gGrenadeTexture = -1;

void GrenadeLoadSounds(void) {
    gGrenadeSounds[0] = AssetFind(0xF1754BF2, 0x0021C5E0);
    gGrenadeSounds[1] = AssetFind(0xF1754BF2, 0x0024805F);
    gGrenadeSounds[2] = AssetFind(0xF1754BF2, 0x101844CC);
    gGrenadeSounds[3] = AssetFind(0xF1754BF2, 0x48874F6E);
    gGrenadeSounds[4] = AssetFind(0xF1754BF2, 0x04A53FC1);
    gGrenadeSounds[5] = AssetFind(0xF1754BF2, 0x967DAAC6);
    gGrenadeSounds[6] = AssetFind(0xF1754BF2, 0xD9404234);
    gGrenadeSounds[7] = AssetFind(0xF1754BF2, 0x0450B2BB);
    gGrenadeSounds[8] = AssetFind(0xF1754BF2, 0x922BF04E);
    gGrenadeSounds[9] = AssetFind(0xF1754BF2, 0x8C73DFA9);
    gGrenadeSounds[10] = AssetFind(0xF1754BF2, 0x90AEEEE4);
    gGrenadeSounds[11] = AssetFind(0xF1754BF2, 0x04A2F9AA);
    gGrenadeSounds[12] = AssetFind(0xF1754BF2, 0x04A48BC0);
    gGrenadeSounds[13] = AssetFind(0xF1754BF2, 0x8671DB2A);
    gGrenadeSounds[14] = AssetFind(0xF1754BF2, 0x044E6CA4);
    gGrenadeSounds[15] = AssetFind(0xF1754BF2, 0x044FFEBA);
}

// Destroys every live emitter in a slot table.
static inline void EmitterSlotsClear(EmitterSlot* slots, int count) {
    int i;

    for (i = 0; i < count; i++) {
        EmitterSlot* s = &slots[i];
        if (s->active) {
            EmitterDestroy(s->handle);
            s->handle = -1;
            s->active = 0;
        }
    }
}

void GrenadeRemoveAll(void) {
    EmitterSlotsClear(gGrenadeEmitters, 8);
}

void GrenadeNop(void) {}

void GrenadeLoadTexture(void) {
    gGrenadeTexture = AssetFind(0xF2E2BEB8, 0xCD678095);
    TextureLoad(&gGrenadeTexture, 1);
}

void GrenadeInit(void) {
    gGrenadeEmitters[0].handle = -1;
    gGrenadeEmitters[1].handle = -1;
    gGrenadeEmitters[2].handle = -1;
    gGrenadeEmitters[3].handle = -1;
    gGrenadeEmitters[4].handle = -1;
    gGrenadeEmitters[5].handle = -1;
    gGrenadeEmitters[6].handle = -1;
    gGrenadeEmitters[7].handle = -1;
}

int GrenadeRemove(int owner) {
    int i;

    for (i = 0; i < 8; i++) {
        EmitterSlot* s = &gGrenadeEmitters[i];
        if (s->active && s->owner == owner) {
            EmitterDestroy(s->handle);
            s->handle = -1;
            s->active = 0;
            return 1;
        }
    }
    return 0;
}

int GrenadeAdd(int owner, Vec3* pos) {
    int i;

    for (i = 0; i < 8; i++) {
        EmitterSlot* s = &gGrenadeEmitters[i];
        if (s->active == 0) {
            EmitterDesc d;
            memset(&d, 0, sizeof(d));
            d.unk94 = 0.0f;
            d.color[0] = 160;
            d.color[1] = 160;
            d.color[2] = 160;
            d.color[3] = 255;
            d.unk98 = 1;
            d.unk7C = 100;
            d.unkA8 = 0;
            d.unkA4 = 0;
            d.unk88 = 0;
            d.unkA0 = 0;
            d.unk9C = 0;
            d.unk6C = 5.0f;
            d.unkD8 = -1;
            d.unkD4 = -1;
            d.unk90 = 1.0f;
            d.unk28 = 1;
            d.texture = gGrenadeTexture;
            d.unk30 = 255;
            d.unkBC = 0.0f;
            d.unkC4 = 1.0f;
            d.unkC0 = 0.0f;
            d.unkC8 = 0.767f;
            d.pos = *pos;
            d.unkC = 0.0f;
            d.unk10 = 0.5f;
            d.unk14 = 0.0f;
            d.unk18 = 0.0f;
            d.unk24 = 0.0f;
            d.unk80 = 5.0f;
            d.unkAC = 5.0f;
            d.unkB4 = 0.167f;
            d.unkB0 = 0.0f;
            d.unkB8 = 0.0f;
            d.unk78 = 0.0f;
            d.unk1C = 1;
            d.unk74 = 0.0f;
            d.unk70 = 5.0f;
            s->handle = EmitterCreate(&d);
            s->active = 1;
            EmitterAttach(s->handle, owner, "GRENADE");
            s->owner = owner;
            return 1;
        }
    }
    return 0;
}

// u_8024E0D4.cpp (0x8024E0D4-0x8024E374): the scannable beam's tunables (message 0x41E setter and the
// variant-driven init), split from the old u_8024CAA0 range.
// Boundary evidence (the cut at 0x8024E0D4): this file's .sdata2 pool 0x804D9D48-0x804D9D68 starts at 0.01f
// and holds its own int-to-float double (0x804D9D50) and 0.0f (0x804D9D58); the previous file's pool
// (0x804D9C90-0x804D9D48) has its own copies at 0x804D9CA8 and 0x804D9C94. CodeWarrior merges identical
// literals within one translation unit, so these are two files. Scannable.cpp follows at 0x8024E374
// (.sdata2 0x804D9D68).
// Names of structures and functions are descriptive guesses.

typedef unsigned char u8;
typedef signed int s32;
typedef unsigned long u32;

// Payload of message 0x41e: the actor's id, a request hash and a value the
// receiver fills in.
struct RailMsg41E {
    u32 unk00;          // 0x00
    u32 hash;           // 0x04
    u32 unk08;          // 0x08
    u32 value;          // 0x0c
};

// Variant object the beam reads its per-variant id from.
struct BeamOwner {
    u8 pad00[0x170];
    u32 id;             // 0x170
};

// Beam object of the scannable rail effect. The three tunables at 0x5b8/0x5c4/
// 0x5c8 are set from message 0x41e; the rest is filled in when the effect is
// (re)started.
struct ScannableBeam {
    u8 pad00[0xc4];
    BeamOwner* owner;   // 0xc4
    u8 padc8[0x270 - 0xc8];
    u8 cond;            // 0x270
    u8 pad271[0x5b4 - 0x271];
    float f5b4;         // 0x5b4
    float f5b8;         // 0x5b8
    float f5bc;         // 0x5bc
    float f5c0;         // 0x5c0
    float f5c4;         // 0x5c4
    float f5c8;         // 0x5c8
    float f5cc;         // 0x5cc
    s32 state;          // 0x5d0
    float f5d4;         // 0x5d4
    float f5d8;         // 0x5d8
    float f5dc;         // 0x5dc
    s32 i5e0;           // 0x5e0
    float f5e4;         // 0x5e4
    s32 i5e8;           // 0x5e8
    u8 pad5ec[0x910 - 0x5ec];
    u32 f910;           // 0x910
};

// Sets one of the beam's three tunables from the message value; the hash picks
// which one. 0x8024E0D4.
int ScannableBeamSetParam(ScannableBeam* beam, RailMsg41E* msg) {
    switch ((s32)msg->hash) {
    case (s32)0x51979712:
        beam->f5c8 = 0.01f * (float)(s32)msg->value;
        return 1;
    case (s32)0xb9af66bf:
        beam->f5b8 = 0.01f * (float)(s32)msg->value;
        return 1;
    case (s32)0xba683b26:
        beam->f5c4 = 0.01f * (float)(s32)msg->value;
        return 1;
    }
    return 0;
}

float VarFindFloat(u32 set, u32 id, u32 key);
void ConditionUpdateBit(void* cond, int bit, int value);

// Initialises the scannable rail beam from variant 0x6249ec5a: when the object
// has not been given a variant id yet it resets its state and takes the id from
// its owner, then reads the seven float tunables and three fixed floats and
// flags condition 0x26 when the variant's 0x5ede8673 value is set. 0x8024E1C0.
int ScannableBeamInit(ScannableBeam* beam) {
    u32 variant;
    if (beam->f910 == 0) {
        beam->state = 4;
        beam->i5e0 = -1;
        beam->f5e4 = 0.0f;
        beam->i5e8 = -1;
        variant = beam->owner->id;
    } else {
        variant = beam->f910;
    }

    beam->f5b4 = 0.01f * VarFindFloat(0x6249ec5a, variant, 0x9782a85b);
    beam->f5b8 = 0.01f * VarFindFloat(0x6249ec5a, variant, 0xb9af66bf);
    beam->f5bc = 0.01f * VarFindFloat(0x6249ec5a, variant, 0x03ff5cb8);
    beam->f5c0 = 0.01f * VarFindFloat(0x6249ec5a, variant, 0x1d9b4d25);
    beam->f5c4 = 0.01f * VarFindFloat(0x6249ec5a, variant, 0xba683b26);
    beam->f5c8 = 0.01f * VarFindFloat(0x6249ec5a, variant, 0x51979712);
    beam->f5cc = 0.01f * VarFindFloat(0x6249ec5a, variant, 0xc3cabadf);
    beam->f5d4 = 2000.0f;
    beam->f5dc = 1280.0f;
    beam->f5d8 = 4000.0f;
    if (VarFindFloat(0x6249ec5a, variant, 0x5ede8673)) {
        ConditionUpdateBit(&beam->cond, 0x26, 1);
    }
    return 1;
}

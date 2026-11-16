#ifndef GAME_OWNER_H
#define GAME_OWNER_H

#include "game/types.h"

// Owner: a character (host, enemy, NPC) as seen by its behaviours, and Beh, the behaviour block inside it.
// Names are descriptive guesses.
//
// The same object is called `Actor` in about 25 units (u_80251CB8, u_80252F2C, u_8016C244, ...): see the note
// in game/actor.h. Those units keep their local definition until the Lead decides on the symbol renames.
//
// Evidence: every field is used at that offset by a Matching unit (named in the comment); fields only seen in
// NonMatching units are left as pads. The object is at least 0x920 bytes (u_802DB9D0 reads a float at +0x91C).
// The first 0x3C bytes (placement, message handler, id) are laid out like the world-object Actor.
//
// Beh (at +0x894) is the running behaviour: `state` selects the BehState, `data` points to the behaviour's
// own state (untyped here, each behaviour casts it). ThermoBeh, Thermostat and NoBot receive it as a
// separate pointer argument next to the owner.

struct Owner;
struct Beh;

typedef void (*BehCallback)(void* owner, void* beh, int arg);

struct Beh {
    char pad0[0x10];                           // 0x00
    int state;                                 // 0x10: ThermoBeh, NoBot, u_80160DB0, u_80252F2C
    unsigned short lo1;                        // 0x14: ThermoBeh, Thermostat
    unsigned short hi1;                        // 0x16
    unsigned short lo2;                        // 0x18
    unsigned short hi2;                        // 0x1A
    char pad1C[0xC];                           // 0x1C
    unsigned short delay;                      // 0x28: NoBot, u_80160DB0
    unsigned short unk2A;                      // 0x2A: u_80160DB0
    unsigned long flags;                       // 0x2C: ThermoBeh, NoBot, u_80160DB0
    int targetKind;                            // 0x30: u_80252F2C (1 = position, 2 = actor id)
    int target;                                // 0x34: u_80252F2C
    char pad38[0x14];                          // 0x38
    BehCallback callback;                      // 0x4C: ThermoBeh, u_80252F2C (called when the state changes)
    char pad50[4];                             // 0x50
    void* data;                                // 0x54: the behaviour's own state
    int unk58;                                 // 0x58: ThermoBeh; +0x8EC of the owner (NoBot, u_80160DB0)
    int unk5C;                                 // 0x5C: ThermoBeh; +0x8F0 of the owner
};

struct Owner {
    Placement place;                           // 0x000: u_8016C244 (fixed-point position at +0x24)
    int (*handler)(int msg, void* data);       // 0x030: u_80251CB8, u_8016C244
    char pad34[4];                             // 0x034
    int id;                                    // 0x038: u_80251CB8, u_80160DB0, NoBot
    char pad3C[4];                             // 0x03C
    void* unk40;                               // 0x040: u_80251CB8
    char pad44[8];                             // 0x044
    unsigned long flags4C;                     // 0x04C: u_802DB9D0
    char pad50[0x34];                          // 0x050
    Vec3 pos;                                  // 0x084: u_80251CB8, u_80160DB0 (float world position)
    char pad90[0x34];                          // 0x090
    void* info;                                // 0x0C4: type data (u_8016C244, u_8023E7E0, u_802DB9D0)
    char padC8[4];                             // 0x0C8
    int model;                                 // 0x0CC: u_80160DB0, u_8016C244
    int anim;                                  // 0x0D0: u_8016C244
    int unkD4;                                 // 0x0D4: u_80251CB8
    int boneId1;                               // 0x0D8: u_80160DB0
    int boneId2;                               // 0x0DC: u_80160DB0
    char padE0[4];                             // 0x0E0
    int nodeE4;                                // 0x0E4: u_80160DB0
    char padE8[8];                             // 0x0E8
    int nodeF0;                                // 0x0F0: u_80160DB0, u_8016C244
    char padF4[8];                             // 0x0F4
    void* unkFC;                               // 0x0FC: a callback (u_80251CB8, u_8016C244, u_8023E7E0)
    char pad100[4];                            // 0x100
    int unk104;                                // 0x104: u_80251CB8, u_8016C244
    char pad108[4];                            // 0x108
    void* unk10C;                              // 0x10C: a callback (u_80251CB8, u_8023E7E0)
    char pad110[0x18];                         // 0x110
    unsigned long type;                        // 0x128: u_8016C244
    char pad12C[4];                            // 0x12C
    void* state;                               // 0x130: behaviour state (u_80251CB8, u_8016C244, u_802DB9D0)
    char pad134[4];                            // 0x134
    float dt;                                  // 0x138: frame time (u_80251CB8, u_8023E7E0, NoBot)
    float unk13C;                              // 0x13C: u_8016C244
    char pad140[0xD4];                         // 0x140
    int unk214;                                // 0x214: u_80251CB8
    char pad218[4];                            // 0x218
    int unk21C;                                // 0x21C: u_80251CB8
    int unk220;                                // 0x220: u_80251CB8, u_8023E7E0
    char pad224[8];                            // 0x224
    void* target;                              // 0x22C: u_80251CB8, u_8023E7E0
    char pad230[0x38];                         // 0x230
    int unk268;                                // 0x268: u_802DB9D0
    int unk26C;                                // 0x26C: u_802DB9D0
    char conditions[0x18];                     // 0x270: condition bits, passed by address (ThermoBeh, NoBot)
    int unk288;                                // 0x288: u_80251CB8, u_8023E7E0
    char pad28C[0x38];                         // 0x28C
    int unk2C4;                                // 0x2C4: u_80251CB8, u_8016C244
    char pad2C8[8];                            // 0x2C8
    int unk2D0;                                // 0x2D0: u_8016C244
    char pad2D4[0x318];                        // 0x2D4
    void* obj;                                 // 0x5EC: u_8016C244
    char pad5F0[0xC];                          // 0x5F0
    void* unk5FC;                              // 0x5FC: callbacks (u_80251CB8)
    void* unk600;                              // 0x600
    void* unk604;                              // 0x604: u_80251CB8, u_8023E7E0
    char pad608[0x28C];                        // 0x608
    Beh beh;                                   // 0x894: ThermoBeh, Thermostat
    char pad8F4[4];                            // 0x8F4
    int unk8F8;                                // 0x8F8: u_8016C244
    char pad8FC[4];                            // 0x8FC
    float unk900;                              // 0x900: u_80251CB8, u_8016C244
    float health;                              // 0x904: u_80251CB8
    char pad908[0x14];                         // 0x908
    float unk91C;                              // 0x91C: u_802DB9D0
};

#endif

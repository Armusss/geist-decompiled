#ifndef GAME_ACTOR_H
#define GAME_ACTOR_H

#include "game/types.h"

// Actor: a placed world object (clock, hazard, mirror, pickup, grenade, ...) and its type descriptor.
// Names are descriptive guesses.
//
// IMPORTANT: the name "Actor" (P5Actor in the symbols) is used by the game sources for three different
// retail objects that only share their first 0x3C bytes (placement, message handler, handle):
//   1. the world-object actor defined here (def/info/type at +0x80..+0x88, per-type data at +0xA4);
//   2. the character ("Owner", see game/owner.h): float position at +0x84, type data at +0xC4,
//      behaviour state at +0x130, conditions at +0x270, about 0x90C bytes;
//   3. the player/host object: flags at +0x50, model at +0x58, message handler at +0x8C, about 0x27A0 bytes.
// Units of kinds 2 and 3 keep their own local `struct Actor` and must not include this header. Giving the
// three their own names changes mangled symbol names and is a decision for the Lead.
//
// Evidence for this layout: every field below is used at that offset by at least one Matching unit
// (named in the comment); no Matching unit of kind 1 contradicts another. Unknown regions are pads.
// The object is at least 0x188 bytes (u_80285ACC reads +0x184); most types use only the first 0x120.
//
// def, info and data point to per-type structures, so they are untyped here and each unit casts them.
// The slot at +0xA4 is per-type storage: most types keep a data pointer there, a few an integer handle
// (Hazard, Streamer) or a float (Clock).

struct Actor;

typedef int (*ActorMsgFunc)(Actor* actor, int msg, void* data, int arg);

// Actor type descriptor, 0x30 bytes, one per actor file in .data: {0, name, msg, update, init, 0, destroy, ...}
// (docs/matching-tricks.md, helper-game-b). u_801A1D24 shows that +0x1C/+0x20 are save/load callbacks.
struct ActorType {
    int unk0;                                  // 0x00
    const char* name;                          // 0x04
    ActorMsgFunc msg;                          // 0x08
    void (*update)(Actor* actor, float dt);    // 0x0C
    int (*init)(Actor* actor);                 // 0x10
    void* unk14;                               // 0x14
    void (*destroy)(Actor* actor);             // 0x18
    void* unk1C[5];                            // 0x1C
};

struct Actor {
    Placement place;                           // 0x00: Hazard, EvacPoint, u_801A1D24, u_80247EB0
    int (*handler)(int msg, void* data);       // 0x30: EvacPoint, u_801A1D24 (called as handler(0x401, &q))
    char pad34[4];                             // 0x34
    unsigned long handle;                      // 0x38: unsigned, the top byte is the object kind: Repel,
                                               //       CollectionHost and u_801A1D24 need `(int)(handle >> 24) == 2`
                                               //       to shift logically (srwi)
    char pad3C[0x10];                          // 0x3C
    unsigned long flags4C;                     // 0x4C: u_80247EB0
    char unk50[0x18];                          // 0x50: Hazard, EvacPoint (passed by address)
    char unk68[0x18];                          // 0x68: Hazard, EvacPoint (passed by address)
    void* def;                                 // 0x80: placement definition (Hazard, EvacPoint, Skybox)
    void* info;                                // 0x84: type parameters (Hazard, Repel, Streamer, ThermoBeh)
    ActorType* type;                           // 0x88: Scannable, ThermoBeh, Thermostat
    int id;                                    // 0x8C: owner/id (Hazard, EvacPoint, CollectionHost)
    int model;                                 // 0x90: Clock, Mirror
    int boneCount;                             // 0x94: Clock
    int unk98;                                 // 0x98: EvacPoint
    void* bones;                               // 0x9C: Clock, Mirror
    char unkA0[4];                             // 0xA0: Hazard, EvacPoint (passed by address)
    void* data;                                // 0xA4: per-type data (LaserScope, Mirror, Repel, ThermoBeh, ...)
    int unkA8;                                 // 0xA8: sound handle in Hazard and EvacPoint
    unsigned long flags;                       // 0xAC: LaserScope, Mirror, ThermoBeh, EvacPoint
    char unkB0[0x30];                          // 0xB0
    Mtx34 matrix;                              // 0xE0: u_80247EB0, u_802B3430
    int anim;                                  // 0x110: Clock, ThermoBeh, Thermostat
    char pad114[4];                            // 0x114
    float unk118;                              // 0x118: u_801A1D24
    void* effect;                              // 0x11C: u_80247EB0
    char pad120[0xC];                          // 0x120
    float scanRange;                           // 0x12C: CollectionHost, CollectionGhost
    char pad130[0x54];                         // 0x130
    int unk184;                                // 0x184: u_80285ACC
};

#endif

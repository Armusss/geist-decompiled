#ifndef GAME_TYPES_H
#define GAME_TYPES_H

// Shared math and placement types of the game code (0x80005800-0x802E4858).
// Names are descriptive guesses; the layouts are the ones the Matching units compile with.
// Only built-in types are used here, so a unit's own u32/s32 typedefs cannot clash.
//
// Evidence, per type (survey of src/game on 2026-11-16, "units" = units that declared the same layout):
//   Vec3      0x0C  three floats; 105 units, all with the same layout.
//   IVec3     0x0C  three ints; world positions are 20.12 fixed point (actor +0x24).
//   Vec4/Quat 0x10  four floats.
//   Mtx33     0x24  3x3 rotation.
//   Mtx34     0x30  declared as m[3][4] by 43 units and as a flat m[12] by 29; both compile to the same code.
//                   The row form is kept; a flat index k is m[k / 4][k % 4].
//   PlaceF    0x30  rotation + float position: the argument of the actor spawn helpers.
//   Placement 0x30  rotation + fixed-point position: the first 0x30 bytes of every actor
//                   (Hazard, EvacPoint, u_801A1D24, u_80247EB0 match with it).

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Vec4 {
    float x, y, z, w;
};

struct Quat {
    float x, y, z, w;
};

struct Mtx33 {
    float m[3][3];
};

struct Mtx34 {
    float m[3][4];
};

// Orientation and float position, as used to place a new actor.
struct PlaceF {
    Mtx33 rot;                                 // 0x00
    Vec3 pos;                                  // 0x24
};

// Orientation and position, as stored at the start of every actor.
struct Placement {
    Mtx33 rot;                                 // 0x00
    IVec3 pos;                                 // 0x24
};

#endif

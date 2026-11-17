#ifndef GAME_VECNORM_H
#define GAME_VECNORM_H

// Shared vector normalize used by the original game code (243 retail sites in 57 units, see docs/asm-normalize.md).
// Retail does the length math in double precision and uses the paired-single estimate instructions
// (ps_rsqrte / ps_res), which GC/1.3.2 never emits from C, so the original was this inline asm block.
// USER-APPROVED EXCEPTION (2026-11-17), like FastSqrt: use it verbatim, and only keep it in a unit that then
// fully matches (flipped to Matching, AUDIT OK). Do not edit this helper to fit one caller.
//
// Notes from the investigation (scratch-verified with GC/1.3.2):
// - `lwz s, src` / `lwz d, dst` give retail's pointer stores at 8/12(r1); parameter order must be (dst, src).
// - the zero test is three __fabsf locals declared z, y, x with the sum compared to 0.
// - callers with an untouched source pointer declare it `const Vec3*`; in-place calls pass the same vector twice.

#include "game/types.h"

float __fabsf(float x);

inline float VecNormalize(Vec3* dst, const Vec3* src) {
    float len;
    float az = __fabsf(src->z);
    float ay = __fabsf(src->y);
    float ax = __fabsf(src->x);
    register float x, y, z, sq, t, w;
    register const Vec3* s;
    register Vec3* d;
    if ((ax > 1.1920929e-7f) + (ay > 1.1920929e-7f) + (az > 1.1920929e-7f) == 0) {
        dst->x = 0.0f;
        dst->y = 0.0f;
        dst->z = 0.0f;
    } else {
        asm {
            lwz s, src
            lwz d, dst
            lfs x, 0(s)
            lfs y, 4(s)
            lfs z, 8(s)
            fmul sq, x, x
            fmul t, y, y
            fmul w, z, z
            fadd sq, sq, t
            fadd sq, sq, w
            ps_rsqrte sq, sq
            fmul x, x, sq
            fmul y, y, sq
            fmul z, z, sq
            ps_res t, sq
            stfs x, 0(d)
            stfs y, 4(d)
            stfs z, 8(d)
            stfs t, len
        }
    }
    return len;
}

#endif

#ifndef _global_h_
#define _global_h_
/* Minimal stand-in for zeldaret/tp's global.h, as used by its MSL/Runtime/TRK sources (GameCube only). */
#define PLATFORM_GCN 1
#define PLATFORM_WII 0
#define PLATFORM_SHIELD 0
#ifndef DEBUG
#define DEBUG 0
#endif
#define MSL_INLINE inline
#define DECL_WEAK __declspec(weak)
#define ARRAY_SIZE(o) (s32)(sizeof(o) / sizeof(o[0]))
#define ARRAY_SIZEU(o) (sizeof(o) / sizeof(o[0]))
#endif

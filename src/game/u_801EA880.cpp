// u_801EA880.cpp (0x801EA880-0x801EAACC): debug text helpers (text packet draw, debug-text state word,
// ScreenPrintf). Split off the old u_801E7DE0 range, whose host states belong to u_801E7534.
// Boundary to report (not applied, it moves u_801EAACC's start): this is the start of the text file. fn_801EA880
// writes the .data 0x803B4284 text buffer through the .sdata 0x804D3318 cursor, which u_801EAACC's
// TextDebugFlush / TextSysInit / TextDraw also use; the debug-text word .sbss 0x804D52A4 sits inside
// u_801EAACC's .sbss (0x804D5290 on).
// Names are descriptive guesses; matched against retail unless noted.

#include "game/msg.h"
#include <dolphin/gx/GXStruct.h>

// Debug display-list buffer and its write cursor. The buffer is a 16 KB
// scratch block the debug text renderer appends packets to.
extern u8 lbl_803B4284[];
extern u8* lbl_804D3318;

// Builds one 9-byte text packet from the colour/rectangle arguments and then
// draws the formatted string. (Descriptive name: DebugTextDraw. Kept as
// fn_801EA880 because other units already call it by that placeholder.)
void TextSetStyle(int, int, int, int, int, int, int, int, int, int, int, int);
void DebugText(const char* fmt, ...);

// Debug text packet writers: each stores one field at the write cursor and
// advances it.
static inline void DebugPut8(u8 v) { u8* p = lbl_804D3318; *p = v; lbl_804D3318 = p + 1; }
static inline void DebugPut16(unsigned short v) { u8* p = lbl_804D3318; *(unsigned short*)p = v; lbl_804D3318 = p + 2; }

extern "C" void fn_801EA880(const char* fmt, GXColor color, int x, int y, int w,
                            int h, int flags)
{
    if ((int)lbl_804D3318 - (int)lbl_803B4284 + 9 < 0x4000) {
        DebugPut8(9);
        DebugPut16(x < 0 ? 0 : (x > 640 ? 640 : x));
        DebugPut16(640);
        DebugPut16(0);
        DebugPut16(y < 0 ? 0 : (y > 480 ? 480 : y));
        *lbl_804D3318 = 0;
    }

    if (flags != 0) {
        TextSetStyle(color.r, color.g, color.b, color.a, w, 2,
                     h / 16 - w, 0, 0, 0, 0, 0);
    } else {
        TextSetStyle(color.r, color.g, color.b, color.a, w, 0, 0, 0,
                     0, 0, 0, 0);
    }
    DebugText(fmt);
}

// Debug-text state word shared with the screen-printf renderer. It lives in
// another unit's small-data section, so it keeps its placeholder name here.
extern int lbl_804D52A4;

// Reads the debug-text state word.
// (Descriptive name: GetDebugTextState. Was fn_801EA9DC.)
int GetDebugTextState(void)
{
    return lbl_804D52A4;
}

// Clears the debug-text state word.
// (Descriptive name: ResetDebugTextState. Was fn_801EA9E4.)
void ResetDebugTextState(void)
{
    lbl_804D52A4 = 0;
}

#include <stdarg.h>

extern "C" {
extern int vsprintf(char* buf, const char* fmt, va_list args);
extern char* strcat(char* dst, const char* src);
extern char lbl_803B8284[];
}

void* ScratchAAlloc(int size);

// Formats a debug message into a scratch block and appends it to the debug
// text buffer when the combined text still fits in the 0x1000-byte block.
void ScreenPrintf(const char* fmt, ...)
{
    va_list args;

    if (lbl_804D52A4 != 0) {
        char* buf = (char*)ScratchAAlloc(0x40000);
        va_start(args, fmt);
        int len = vsprintf(buf, fmt, args);

        if ((u32)(len + strlen(lbl_803B8284)) < 0x1000) {
            strcat(lbl_803B8284, buf);
        }
    }
}

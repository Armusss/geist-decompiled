// u_801EAACC.cpp: text rendering and layout, and player mode/gun action helpers (0x801EAACC-0x801EDE74).
// Names are descriptive guesses; the code is matched against the retail DOL.
//
// This unit holds the game's text drawing code (TextDraw/DebugText, the
// TextSet* setters, style/rect handling and the layout measure pass).
// All names are descriptive guesses.

#include <stdarg.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef float f32;
typedef double f64;

// GX vertex colour: four bytes in memory order.
struct GXColor {
    u8 r, g, b, a;
};

extern "C" char* strcpy(char* dst, const char* src);
extern "C" void* memcpy(void* dst, const void* src, u32 len);

typedef void (*TextCallback)(int, int, int, int, int, int, int, int);

// Shared text state, defined by another unit.
struct TextSavedState {
    u32 w[23];                  // 0x00-0x5b: text state words
    TextCallback callback;      // 0x5c: run callback
    u32 w24;                    // 0x60
    u32 w25;                    // 0x64
    u32 w26;                    // 0x68
};

struct TextState {
    TextSavedState cur;         // 0x000: current text state
    TextSavedState saved[32];   // 0x06c: one saved copy per nesting level
};

extern TextState lbl_8049C228;
extern u8 lbl_803B4284[];       // text command buffer
extern u8* lbl_804D3318;        // command buffer cursor
extern char lbl_803B8284[];     // debug string buffer
extern s32 lbl_804D331C;
extern s32 lbl_804D3320;
extern s32 lbl_804D5298;        // current save depth
extern s32 lbl_804D52A0;
extern u8* lbl_804D5290;
extern f32 lbl_804D8F30;
extern f64 lbl_804D8F38;
extern f32 lbl_804D8F44;
extern f32 lbl_804D8F48;
extern f32 lbl_804D8F4C;
extern f32 lbl_804D8F50;
extern f64 lbl_804D8F58;
extern f32 lbl_804D2520;
extern f32 gAspectScale;
extern f32 lbl_803B9284[];      // six outline-offset directions, xy pairs
extern u32 lbl_804DBB18;        // fixed shadow RGB
extern s32 lbl_804D3314;        // text debug flag

extern f32 lbl_804D8F60;       // projection scale numerator

void TextStateInit(void);
void TextDrawBeginFrame(void);
extern "C" void fn_8010EC10(f32 scaleX, f32 scaleY, int arg);
extern "C" void fn_8010C880(void);
void GfxSetBlendMode(int a, int b, int c, int d);
extern "C" void GXSetZMode(int compareEnable, int func, int updateEnable);
extern "C" void GXLoadTexObj(void* texObj, int mapId);
extern "C" char* OSGetFontTexture(const char* str, void** image, int* x, int* y,
                                  int* width);
extern "C" int fn_80010448(int a, int b);
extern "C" void fn_80010330(int a, int b);
extern "C" void GXBegin(int primitive, int vtxFmt, int nVerts);

void TextDrawFlatQuad(u32 obj, short x, short advance, int y, short height);
void TextDrawGlyphQuad(short x, short advance, short y, short height,
                       int texX, int texY, u32* color);

void TextSetStyle(int, int, int, int, int, int, int, int, int, int, int, int);
void DebugText(const char* fmt, ...);
void TextDrawRun(short x, int y, char* text);
char* TextMeasureRun(char* str, int width, int* widthOut, int* advanceOut,
                     int* flag, int* lineEnd);

// Appends the debug-line command block, white style, then renders and clears
// the debug string buffer (guessed: TextDebugFlush).
void TextDebugFlush(void)
{
    if (lbl_804D3318 + 9 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 9;                   // 0x0: command id
        *(u16*)(p + 1) = 0x14;      // 0x1
        *(u16*)(p + 3) = 0x280;     // 0x3
        *(u16*)(p + 5) = 0;         // 0x5
        *(u16*)(p + 7) = 0x1CC;     // 0x7
        lbl_804D3318 = p + 9;
        p[9] = 0;                   // 0x9: terminator
    }
    TextSetStyle(0xFF, 0xFF, 0xFF, 0xFF, lbl_804D331C, 2, lbl_804D3320,
                 0, 0, 0, 0, 0);
    DebugText(lbl_803B8284);
    lbl_803B8284[0] = 0;
}

// Measures one string in the current font and returns its width in pixels
// (guessed: TextLayoutMeasure).
int TextLayoutMeasure(const char* str, int id)
{
    int unk8 = 0;
    int unkC = 0;
    int unk10 = 0;
    int width = 0;
    u8 flag = 1;
    char buf[128];

    strcpy(buf, str);

    if (lbl_804D5298 < 0x20) {
        lbl_8049C228.saved[lbl_804D5298] = lbl_8049C228.cur;
        lbl_804D5298++;
    }
    lbl_8049C228.cur.w[9] = 0;
    lbl_8049C228.cur.w[10] = 0;
    lbl_8049C228.cur.w[16] = id;
    lbl_8049C228.cur.w[8] = id * (u32)*(u16*)(lbl_804D5290 + 0x10);
    lbl_8049C228.cur.w[7] =
        (int)((f32)((f64)lbl_8049C228.cur.w[8] - lbl_804D8F38) * lbl_804D8F30
              / lbl_804D2520);

    TextMeasureRun((char*)&flag, 0x2800, &width, &unk10, &unkC, &unk8);

    if (1 < lbl_804D5298) {
        lbl_804D5298--;
        lbl_8049C228.cur = lbl_8049C228.saved[lbl_804D5298];
        lbl_804D52A0 = 0;
    }
    return width / 16;
}

// Puts the text command buffer back to its empty state and points the
// cursor at it (guessed: TextSysInit).
void TextSysInit(void)
{
    lbl_803B4284[0] = 0;
    lbl_804D3318 = lbl_803B4284;
}

void GfxSetVtxDesc(int desc);
void GfxSetMaterial(void* material);
void GfxSetCullMode(int mode);
extern "C" void GXSetAlphaUpdate(int enable);
extern "C" void GXSetColorUpdate(int enable);
extern "C" void fn_80008CA4(void);
extern "C" void fn_8000FF7C(void);
extern "C" void fn_8010F7EC(void);
extern "C" void fn_801D8FC8(void);
void TextProcessCommands(u8* buffer);

// Restores the default render states, flushes the pending text and empties
// the command buffer (guessed: fn_801EAEAC).
// fn_801EAEAC
extern "C" void fn_801EAEAC(void)
{
    if (lbl_803B4284[0] != 0) {
        GfxSetVtxDesc(3);
        fn_80008CA4();
        GfxSetMaterial(0);
        GfxSetCullMode(0);
        GXSetAlphaUpdate(1);
        GXSetColorUpdate(1);
        lbl_804D52A0 = 0;
        TextProcessCommands(lbl_803B4284);
        fn_8000FF7C();
        fn_8010F7EC();
        fn_801D8FC8();
    }
    lbl_803B4284[0] = 0;
    lbl_804D3318 = lbl_803B4284;
}

// ---- Font arena ----------------------------------------------------------
extern "C" void* OSGetArenaLo(void);
extern "C" void* OSInitFont(void* font);
extern "C" void OSSetArenaLo(void* arenaLo);
extern "C" int sprintf(char* buf, const char* fmt, ...);
extern "C" void GXInitTexObj(void* texObj, void* image, u16 width, u16 height,
                             u8 format, u8 wrapS, u8 wrapT, u8 mipmap);
extern "C" void GXInitTexObjLOD(void* texObj, int minFilt, int magFilt,
                                f32 minLod, f32 maxLod, f32 lodBias,
                                u8 biasClamp, u8 edgeLod, int maxAniso);

extern u8 lbl_8049D014[];       // GXTexObj table, 0x20 bytes per object
extern u8* lbl_804D5294;        // start of the font image data
extern u32 lbl_804D529C;        // number of font pages
extern s32 lbl_804D3310;        // selected font index
extern char* lbl_803B4268[];    // font file names
extern char lbl_80349038[];     // string pool; +0x3c is "fonts/%s.bin"
extern f32 lbl_804D8F40;

// Header of the block OSGetArenaLo hands back for the font system.
struct FontArena {
    u8 unk00[0x08];             // 0x000
    u16 ascent;                 // 0x008: font ascent in texels
    u16 descent;                // 0x00a: font descent in texels
    u16 shear;                  // 0x00c: oblique shear numerator
    u8 unk0e[0x10 - 0x0e];      // 0x00e
    u16 maxCharWidth;           // 0x010: character-cell width
    u16 cellWidth;              // 0x012
    u32 lineStride;             // 0x014: bytes per glyph row
    u16 format;                 // 0x018: texture format
    u8 unk1a[0x1e - 0x1a];      // 0x01a
    u16 texWidth;               // 0x01e
    u16 texHeight;              // 0x020
    u32 headerSize;             // 0x024
    u32 dataSize;               // 0x028
};

// Carves the font arena out of the OS arena and builds one texture object per
// font page (guessed: TextFontInit).
void TextFontInit(void)
{
    char name[128];
    u8* arena;
    u8* tex;
    int i;

    arena = (u8*)OSGetArenaLo();
    lbl_804D5290 = arena;
    OSInitFont(arena);
    sprintf(name, &lbl_80349038[0x3c], lbl_803B4268[lbl_804D3310]);
    OSInitFont(lbl_804D5290);
    ((FontArena*)lbl_804D5290)->headerSize = 0x120;
    lbl_804D5294 = lbl_804D5290 +
        (((FontArena*)lbl_804D5290)->headerSize + 0x1f & ~0x1f);
    lbl_804D529C = ((FontArena*)lbl_804D5290)->dataSize /
                   ((FontArena*)lbl_804D5290)->lineStride;
    OSSetArenaLo(lbl_804D5290 + 0x20120);

    tex = lbl_8049D014;
    for (i = 0; i < (int)lbl_804D529C; i++) {
        GXInitTexObj(tex,
                     lbl_804D5294 + ((FontArena*)lbl_804D5290)->lineStride * i,
                     ((FontArena*)lbl_804D5290)->texWidth,
                     ((FontArena*)lbl_804D5290)->texHeight,
                     ((FontArena*)lbl_804D5290)->format, 0, 0, 0);
        GXInitTexObjLOD(tex, 1, 1, lbl_804D8F40, lbl_804D8F40,
                        lbl_804D8F40, 0, 0, 0);
        tex += 0x20;
    }
}

int FileLoadInto(const char* name, void* buf, u32 size);

// Reloads the text system from fonts/%s.bin, falling back to the built-in font
// when the file cannot be read (guessed: TextFontLoad). main.cpp still calls
// this through its fn_ placeholder, so it keeps C linkage.
extern "C" void fn_801EB060(int index)
{
    char name[128];

    lbl_804D3310 = index;
    sprintf(name, &lbl_80349038[0x3c], lbl_803B4268[index]);
    if (FileLoadInto(name, lbl_804D5290, 0x20120) == 0) {
        OSInitFont(lbl_804D5290);
    }
    ((FontArena*)lbl_804D5290)->headerSize = 0x120;
}

// Appends a one-glyph draw command (id 8, glyph id) to the text command buffer
// (guessed: TextDrawChar).
void TextDrawChar(int ch)
{
    if ((int)(lbl_804D3318 - lbl_803B4284 + 5) < 0x4000) {
        u8* p = lbl_804D3318;

        *p = 8;                     // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u32*)lbl_804D3318 = ch;   // 0x1: glyph id
        lbl_804D3318 += 4;
        *lbl_804D3318 = 0;          // 0x5: terminator
    }
}

// Appends `len` bytes of a string to the text command buffer (guessed:
// TextDraw).
void TextDraw(const char* str, unsigned long len)
{
    if ((int)(lbl_804D3318 - lbl_803B4284 + len + 1) < 0x4000) {
        memcpy(lbl_804D3318, str, len);
        lbl_804D3318 += len;
        *lbl_804D3318 = 0;
    }
}

extern "C" int vsprintf(char* buf, const char* fmt, va_list args);
void* ScratchAAlloc(int size);

// Formats a debug line into a scratch buffer and appends it to the text
// command buffer as command 1 (guessed: TextDebugPrint).
void DebugText(const char* fmt, ...)
{
    char* buf;
    va_list args;
    int len;

    if (*fmt != '\0') {
        buf = (char*)ScratchAAlloc(0x2000);
        va_start(args, fmt);
        len = vsprintf(buf, fmt, args);
        va_end(args);
        if ((int)(lbl_804D3318 + len + 3 - lbl_803B4284) < 0x4000) {
            *lbl_804D3318 = 1;      // 0x0: command id
            lbl_804D3318++;
            strcpy((char*)lbl_804D3318, buf);
            lbl_804D3318 += len + 1;
            *lbl_804D3318 = 0;
        }
    }
}

// Walks the text command stream, updating the current text state and emitting
// runs of glyphs (guessed: TextProcessCommands).
void TextProcessCommands(u8* buffer)
{
    TextSavedState* cur = &lbl_8049C228.cur;
    u8* next;
    char text[0x400];
    int width;
    int advance;
    int lineEnd = 0;
    int alignOfs;

    TextStateInit();

    for (;;) {
        switch ((s32)(s8)*buffer) {
        case 0:
            // end of the command stream
            if (cur->callback != 0) {
                cur->callback((int)cur->w[17], (int)cur->w[18], (int)cur->w[19],
                              (int)cur->w[20], (int)cur->w[21], (int)cur->w[22],
                              (int)cur->w[0] / 16, (int)cur->w24);
            }
            return;

        case 1:
            buffer++;
            if (lbl_804D5298 < 0x20) {
                lbl_8049C228.saved[lbl_804D5298] = *cur;
                lbl_804D5298++;
            }
            break;

        case 2:
            buffer++;
            if (1 < lbl_804D5298) {
                lbl_804D5298--;
                *cur = lbl_8049C228.saved[lbl_804D5298];
                lbl_804D52A0 = 0;
            }
            break;

        case 3:
            if (cur->callback != 0) {
                cur->callback((int)cur->w[17], (int)cur->w[18], (int)cur->w[19],
                              (int)cur->w[20], (int)cur->w[21], (int)cur->w[22],
                              (int)cur->w[0] / 16, (int)cur->w24);
            }
            *cur = lbl_8049C228.saved[0];
            lbl_804D5298 = 1;
            lbl_804D52A0 = 0;
            break;

        case 4:
            cur->w[2] = *(u32*)(buffer + 1);
            buffer += 5;
            break;

        case 5:
            cur->w[9] = (s32)(s8)buffer[1];
            buffer += 2;
            break;

        case 6: {
            s32 v = (s16)*(s16*)(buffer + 1);

            buffer += 3;
            cur->w[16] = v;
            cur->w[8] = v * *(u16*)(lbl_804D5290 + 0x10);
            cur->w[7] = (int)((f32)((f64)cur->w[8] - lbl_804D8F38) * lbl_804D8F30
                              / gAspectScale);
            break;
        }

        case 7:
            cur->w[10] = (s32)(s16)*(s16*)(buffer + 1) * *(u16*)(lbl_804D5290 + 0x10);
            cur->w[11] = (s32)(s16)*(s16*)(buffer + 3) * *(u16*)(lbl_804D5290 + 0x10);
            buffer += 5;
            break;

        case 8:
            lbl_804D52A0 = 0;
            cur->w25 = *(u32*)(buffer + 1);
            cur->w26 = *(u32*)(buffer + 5);
            buffer += 9;
            break;

        case 9:
            if (cur->callback != 0) {
                cur->callback((int)cur->w[17], (int)cur->w[18], (int)cur->w[19],
                              (int)cur->w[20], (int)cur->w[21], (int)cur->w[22],
                              (int)cur->w[0] / 16, (int)cur->w24);
            }
            cur->w[3] = (s32)(s16)*(s16*)(buffer + 1);
            cur->w[4] = (s32)(s16)*(s16*)(buffer + 3);
            cur->w[5] = (s32)(s16)*(s16*)(buffer + 5);
            cur->w[6] = (s32)(s16)*(s16*)(buffer + 7);
            buffer += 9;
            cur->w[3] *= 16;
            cur->w[4] *= 16;
            cur->w[5] *= 16;
            cur->w[6] *= 16;
            cur->w[17] = 0;
            cur->w[18] = 0;
            cur->w[19] = 0;
            cur->w[20] = 0x7FFFFFFF;
            cur->w[21] = 0;
            cur->w[22] = 0x7FFFFFFF;
            cur->callback = 0;
            cur->w24 = 0;
            cur->w[0] = cur->w[6];
            cur->w[1] = 1;
            break;

        case 10:
            if (cur->callback != 0) {
                cur->callback((int)cur->w[17], (int)cur->w[18], (int)cur->w[19],
                              (int)cur->w[20], (int)cur->w[21], (int)cur->w[22],
                              (int)cur->w[0] / 16, (int)cur->w24);
            }
            cur->w[3] = (s32)(s16)*(s16*)(buffer + 1);
            cur->w[4] = (s32)(s16)*(s16*)(buffer + 3);
            cur->w[5] = (s32)(s16)*(s16*)(buffer + 5);
            cur->w[6] = (s32)(s16)*(s16*)(buffer + 7);
            cur->w[3] *= 16;
            cur->w[4] *= 16;
            cur->w[5] *= 16;
            cur->w[6] *= 16;
            cur->w[17] = (s32)(s16)*(s16*)(buffer + 9);
            cur->w[22] = (s32)(s16)*(s16*)(buffer + 0xB);
            cur->callback = (TextCallback)*(u32*)(buffer + 0xD);
            cur->w24 = *(u32*)(buffer + 0x11);
            buffer += 0x15;
            cur->w[18] = cur->w[17];
            cur->w[19] = 0;
            cur->w[20] = 0x7FFFFFFF;
            cur->w[21] = 0;
            cur->w[0] = cur->w[6];
            cur->w[1] = 1;
            break;

        case 11:
            cur->w[1] = 1;
            cur->w[12] = (s32)(s8)buffer[1];
            buffer += 2;
            break;

        case 12:
            cur->w[13] = (s32)(s8)buffer[1];
            cur->w[14] = (s32)(s16)*(s16*)(buffer + 2) * *(u16*)(lbl_804D5290 + 0x10);
            cur->w[15] = (s32)(s16)*(s16*)(buffer + 4) * *(u16*)(lbl_804D5290 + 0x10);
            cur->w[1] = 1;
            buffer += 6;
            break;

        case 13:
            if (lbl_804D5298 < 0x20) {
                lbl_8049C228.saved[lbl_804D5298] = *cur;
                lbl_804D5298++;
            }
            // fall through
        case 14: {
            if (cur->w[12] == 0) {
                if ((int)cur->w[1] != 0) {
                    if ((int)cur->w[13] == 1) {
                        width = (int)cur->w[4] - (int)cur->w[3] - (int)cur->w[15];
                        alignOfs = (int)cur->w[15];
                    } else if ((int)cur->w[13] == 3) {
                        width -= (int)cur->w[15] * 2;
                        alignOfs = (int)cur->w[15];
                    } else {
                        alignOfs = 0;
                        width = (int)cur->w[4] - (int)cur->w[3];
                    }
                } else {
                    if ((int)cur->w[13] == 2) {
                        width = (int)cur->w[4] - (int)cur->w[3] - (int)cur->w[15];
                        alignOfs = (int)cur->w[15];
                    } else if ((int)cur->w[13] == 3) {
                        width -= (int)cur->w[15] * 2;
                        alignOfs = (int)cur->w[15];
                    } else {
                        alignOfs = 0;
                        width = (int)cur->w[4] - (int)cur->w[3];
                    }
                }
            }

            next = (u8*)TextMeasureRun((char*)buffer, width, &width, &advance,
                                       (int*)&cur->w[1], &lineEnd);

            if ((int)cur->w[0] - advance > (int)cur->w[20]) {
                // keep the run on the current line
            } else {
                // past the right edge or the line limit: drop this run
                if (1 < lbl_804D5298) {
                    lbl_804D5298--;
                    lbl_804D52A0 = 0;
                }
                cur->w[19]++;
                buffer = next;
                break;
            }
            if ((int)cur->w[17] > (int)cur->w[19]) {
                if (1 < lbl_804D5298) {
                    lbl_804D5298--;
                    lbl_804D52A0 = 0;
                }
                cur->w[19]++;
                buffer = next;
                break;
            }
            if (width != 0) {
                int x;
                int i;

                if (1 < lbl_804D5298) {
                    lbl_804D5298--;
                    lbl_804D52A0 = 0;
                    *cur = lbl_8049C228.saved[lbl_804D5298];
                }
                i = (int)(next - buffer);
                memcpy(text, buffer, i);
                text[i] = 0;
                text[i + 1] = 0;
                switch ((int)cur->w[12]) {
                case 0:
                    x = (int)cur->w[3] + alignOfs;
                    break;
                case 1:
                    x = (int)cur->w[4] - advance;
                    break;
                case 2:
                    x = ((int)cur->w[3] + (int)cur->w[4] - advance) / 2;
                    break;
                default:
                    x = 0;
                    break;
                }
                TextDrawRun((s16)x, (s16)((int)cur->w[0] - advance), text);
            } else {
                if (1 < lbl_804D5298) {
                    lbl_804D5298--;
                    lbl_804D52A0 = 0;
                }
            }

            if ((int)cur->w[14] != -(int)*(u16*)(lbl_804D5290 + 0x12)) {
                cur->w[0] -= (int)cur->w[11] +
                             width * *(u16*)(lbl_804D5290 + 0xE) /
                             *(u16*)(lbl_804D5290 + 0x10);
            }
            if (lineEnd != 0) {
                lineEnd = 0;
                cur->w[0] = cur->w[20];
            } else if ((int)cur->w[1] != 0) {
                if ((int)cur->w[14] != -(int)*(u16*)(lbl_804D5290 + 0x12)) {
                    cur->w[0] -= (int)cur->w[14];
                }
            }
            cur->w[18]++;
            cur->w[19]++;
            buffer = next;
            break;
        }

        default:
            return;
        }
    }
}

// Measures one run of the text command stream (guessed: TextMeasureRun).
// Walks the layout commands from `str`, accumulating the advance of the glyphs
// it meets, and returns the pointer where rendering should resume.
char* TextMeasureRun(char* str, int width, int* widthOut, int* advanceOut,
                     int* flag, int* lineEnd)
{
    TextSavedState* cur = &lbl_8049C228.cur;
    FontArena* font = (FontArena*)lbl_804D5290;
    void* image;
    int glyphWidth;
    int texY;
    int texX;
    int advance;
    int count;
    int on;
    char* start;

restart:
    start = str;
    *widthOut = 0;
    advance = 0;
    count = 0;
    *advanceOut = 0;
    on = 0;
    *flag = 0;
    *lineEnd = 0;

    for (;;) {
        switch (*str) {
        default:
            *widthOut += advance;
            return str;

        case 11:
            str++;
            if (lbl_804D5298 < 0x20) {
                lbl_8049C228.saved[lbl_804D5298] = *cur;
                lbl_804D5298++;
            }
            break;

        case 12:
            str++;
            if (1 < lbl_804D5298) {
                lbl_804D5298--;
                *cur = lbl_8049C228.saved[lbl_804D5298];
                lbl_804D52A0 = 0;
            }
            break;

        case 2:
            cur->w[2] = *(u32*)(str + 1);
            str += 5;
            break;

        case 5:
            cur->w[9] = (s32)(s8)str[1];
            str += 2;
            break;

        case 3: {
            s16 size = *(s16*)(str + 1);

            str += 3;
            cur->w[16] = size;
            cur->w[8] = (s32)size * font->maxCharWidth;
            cur->w[7] = (int)((f32)cur->w[8] * lbl_804D8F30 / gAspectScale);
            break;
        }

        case 13:
            cur->w[16] += (s32)(s8)str[1];
            str += 2;
            cur->w[8] = (s32)cur->w[16] * font->maxCharWidth;
            cur->w[7] = (int)((f32)cur->w[8] * lbl_804D8F30 / gAspectScale);
            break;

        case 4: {
            s16 a = *(s16*)(str + 1);
            s16 b = *(s16*)(str + 3);

            cur->w[10] = a;
            cur->w[11] = b;
            str += 5;
            cur->w[10] = (s32)a * font->maxCharWidth;
            cur->w[11] = (s32)b * font->maxCharWidth;
            break;
        }

        case 14:
            lbl_804D52A0 = 0;
            cur->w25 = *(u32*)(str + 1);
            cur->w26 = *(u32*)(str + 5);
            str += 9;
            break;

        case 8:
            str += 5;
            advance += (s32)cur->w[10];
            count++;
            advance += (s32)cur->w[7];
            if (on && *widthOut + advance > width) {
                *start = 1;
                return start;
            }
            break;

        case 16:
            if (lbl_804D3314 == 1) {
                *widthOut += advance;
                str++;
                *lineEnd = 1;
                return str;
            }
            str[-1] = ' ';
            *str = 0;
            do {
                str--;
            } while (*str != 1);
            goto restart;

        case 1:
            str++;
            if (*advanceOut < (s32)cur->w[8])
                *advanceOut = (s32)cur->w[8];
            goto scan;
        }
        continue;

    scan:
        while (*str != 0) {
            char* ch = str;
            int glyphAdvance;

            str = OSGetFontTexture(str, &image, &texX, &texY, &glyphWidth);
            if (cur->w[9] & 2)
                glyphAdvance = (s32)cur->w[7] + (s32)cur->w[10];
            else
                glyphAdvance = (s32)cur->w[10] +
                               (s32)cur->w[7] * glyphWidth / font->maxCharWidth;

            if (*ch == ' ') {
                if ((s32)cur->w[21] < (s32)cur->w[20])
                    cur->w[21] += count;
                *widthOut += advance;
                start = ch;
                count = 1;
                on = 1;
                advance = glyphAdvance;
            } else if (*ch == '\n') {
                if ((s32)cur->w[21] < (s32)cur->w[20])
                    cur->w[21] += count;
                *widthOut += advance;
                *flag = 1;
                *ch = 1;
                return ch;
            } else {
                advance += glyphAdvance;
                count++;
                if (on && *widthOut + advance > width) {
                    *start = 1;
                    return start;
                }
            }
        }
        str++;
    }
}

// Draws one text run (guessed: TextDrawRun).  Walks the glyph string that
// follows the run command and emits a quad per glyph.  When the outline flag
// is set it first emits six offset copies in the fixed shadow colour.
void TextDrawRun(short x, int y, char* text)
{
    TextSavedState* cur = &lbl_8049C228.cur;
    FontArena* font = (FontArena*)lbl_804D5290;
    char* str = text;
    void* image;
    int texX;
    int texY;
    int glyphWidth;
    short penX = x;
    short sy = (short)y;
    short height;
    short advance;
    short xoff;
    short px;
    short py;
    int count;
    int i;

    for (;;) {
        switch (*str) {
        default:
            return;

        case 11:
            str++;
            if (lbl_804D5298 < 0x20) {
                lbl_8049C228.saved[lbl_804D5298] = *cur;
                lbl_804D5298++;
            }
            break;

        case 12:
            str++;
            if (1 < lbl_804D5298) {
                lbl_804D5298--;
                *cur = lbl_8049C228.saved[lbl_804D5298];
                lbl_804D52A0 = 0;
            }
            break;

        case 2:
            cur->w[2] = *(u32*)(str + 1);
            str += 5;
            break;

        case 5:
            cur->w[9] = (s32)(s8)str[1];
            str += 2;
            break;

        case 3: {
            s16 size = *(s16*)(str + 1);

            str += 3;
            cur->w[16] = size;
            cur->w[8] = (s32)size * font->maxCharWidth;
            cur->w[7] = (int)((f32)cur->w[8] * lbl_804D8F30 / gAspectScale);
            break;
        }

        case 13:
            cur->w[16] += (s32)(s8)str[1];
            str += 2;
            cur->w[8] = (s32)cur->w[16] * font->maxCharWidth;
            cur->w[7] = (int)((f32)cur->w[8] * lbl_804D8F30 / gAspectScale);
            break;

        case 4: {
            s16 a = *(s16*)(str + 1);
            s16 b = *(s16*)(str + 3);

            cur->w[10] = a;
            cur->w[11] = b;
            str += 5;
            cur->w[10] = (s32)a * font->maxCharWidth;
            cur->w[11] = (s32)b * font->maxCharWidth;
            break;
        }

        case 14:
            lbl_804D52A0 = 0;
            cur->w25 = *(u32*)(str + 1);
            cur->w26 = *(u32*)(str + 5);
            str += 9;
            break;

        case 8: {
            u32 obj = *(u32*)(str + 1);

            str += 5;
            height = (short)((s32)cur->w[8] * (font->ascent + font->descent) /
                             font->maxCharWidth);
            if ((s32)cur->w[21] <= (s32)cur->w[22]) {
                lbl_804D52A0 = 0;
                fn_8000FF7C();
                switch (fn_80010448((int)obj, 1)) {
                case 2:
                    fn_80010330((int)obj, 1);
                    break;
                case 0:
                case 8:
                    fn_80010448(-3, 1);
                    break;
                }
                GXSetZMode(0, 7, 0);
                TextDrawFlatQuad(obj, penX, (short)cur->w[7], y, height);
            }
            penX = (short)(penX + (s32)cur->w[10] + (s32)cur->w[7]);
            if ((s32)cur->w[20] > (s32)cur->w[21])
                cur->w[20] = cur->w[21];
            cur->w[21]++;
            break;
        }

        case 1: {
            str++;
            if (cur->w[9] & 8) {
                u32 shadow;
                f32 scale = lbl_804D8F44 -
                    lbl_804D8F48 * ((f32)cur->w[8] / (f32)font->cellWidth -
                                    lbl_804D8F4C);

                if (scale < lbl_804D8F50)
                    scale = lbl_804D8F50;
                else if (scale > lbl_804D8F44)
                    scale = lbl_804D8F44;

                shadow = (lbl_804DBB18 & 0xFFFFFF00) | (cur->w[2] & 0xFF);

                for (i = 0; i < 6; i++) {
                    char* cp;

                    px = (short)(penX + (int)(scale * (f32)cur->w[8] *
                                              lbl_803B9284[i * 2]));
                    py = (short)(sy + (int)(scale * (f32)cur->w[7] *
                                            lbl_803B9284[i * 2 + 1]));
                    height = (short)((s32)cur->w[8] *
                                     (font->ascent + font->descent) /
                                     font->maxCharWidth);
                    count = cur->w[21];
                    cp = str;
                    while (*cp != 0) {
                        cp = OSGetFontTexture(cp, &image, &texX, &texY,
                                              &glyphWidth);
                        if (cur->w[9] & 2) {
                            advance = (short)((s32)cur->w[7] + (s32)cur->w[10]);
                            xoff = (short)(((s32)cur->w[7] -
                                            (s32)cur->w[7] * glyphWidth /
                                            font->maxCharWidth) / 2);
                        } else {
                            xoff = 0;
                            advance = (short)((s32)cur->w[10] +
                                              (s32)cur->w[7] * glyphWidth /
                                              font->maxCharWidth);
                        }
                        if (count <= (s32)cur->w[22]) {
                            if (lbl_804D52A0 == 0)
                                ((void (*)(int))cur->w25)((int)cur->w26);
                            if (lbl_804D52A0 != (s32)image) {
                                lbl_804D52A0 = (s32)image;
                                GXLoadTexObj(&lbl_8049D014[
                                    ((u8*)image - lbl_804D5294) /
                                    font->lineStride * 0x20], 0);
                            }
                            TextDrawGlyphQuad((short)(px + xoff),
                                              (short)cur->w[7], py, height,
                                              texX, texY, &shadow);
                        }
                        px = (short)(px + advance);
                        count++;
                    }
                }
            }
            {
                char* cp;

                px = penX;
                height = (short)((s32)cur->w[8] *
                                 (font->ascent + font->descent) /
                                 font->maxCharWidth);
                count = cur->w[21];
                cp = str;
                while (*cp != 0) {
                    cp = OSGetFontTexture(cp, &image, &texX, &texY,
                                          &glyphWidth);
                    if (cur->w[9] & 2) {
                        advance = (short)((s32)cur->w[7] + (s32)cur->w[10]);
                        xoff = (short)(((s32)cur->w[7] -
                                        (s32)cur->w[7] * glyphWidth /
                                        font->maxCharWidth) / 2);
                    } else {
                        xoff = 0;
                        advance = (short)((s32)cur->w[10] +
                                          (s32)cur->w[7] * glyphWidth /
                                          font->maxCharWidth);
                    }
                    if (count <= (s32)cur->w[22]) {
                        if (lbl_804D52A0 == 0)
                            ((void (*)(int))cur->w25)((int)cur->w26);
                        if (lbl_804D52A0 != (s32)image) {
                            lbl_804D52A0 = (s32)image;
                            GXLoadTexObj(&lbl_8049D014[
                                ((u8*)image - lbl_804D5294) /
                                font->lineStride * 0x20], 0);
                        }
                        TextDrawGlyphQuad((short)(px + xoff),
                                          (short)cur->w[7], (short)y, height,
                                          texX, texY, &cur->w[2]);
                    }
                    px = (short)(px + advance);
                    count++;
                }
                cur->w[21] = count;
                if ((s32)cur->w[20] > count - 1)
                    cur->w[20] = count - 1;
                penX = px;
                str = cp + 1;
            }
            break;
        }
        }
    }
}

// Emits the flat-colour cell quad used for inline image objects: the whole
// glyph cell is tiled with the current colour (guessed: TextDrawFlatQuad).
// The first (object) argument is dead in the retail code.
void TextDrawFlatQuad(u32 obj, short x, short advance, int y, short height)
{
    GXColor color = *(GXColor*)&lbl_8049C228.cur.w[2];
    u16 top = (u16)(y + height);
    u16 right = (u16)(x + advance);

    (void)obj;
    GXBegin(0x80, 3, 4);

    // top-left
    *(volatile u16*)0xCC008000 = (u16)x;
    *(volatile u16*)0xCC008000 = top;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u8*)0xCC008000 = color.r;
    *(volatile u8*)0xCC008000 = color.g;
    *(volatile u8*)0xCC008000 = color.b;
    *(volatile u8*)0xCC008000 = color.a;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u16*)0xCC008000 = 0;

    // top-right
    *(volatile u16*)0xCC008000 = right;
    *(volatile u16*)0xCC008000 = top;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u8*)0xCC008000 = color.r;
    *(volatile u8*)0xCC008000 = color.g;
    *(volatile u8*)0xCC008000 = color.b;
    *(volatile u8*)0xCC008000 = color.a;
    *(volatile u16*)0xCC008000 = 1;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u16*)0xCC008000 = 0;

    // bottom-right
    *(volatile u16*)0xCC008000 = right;
    *(volatile u16*)0xCC008000 = (u16)y;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u8*)0xCC008000 = color.r;
    *(volatile u8*)0xCC008000 = color.g;
    *(volatile u8*)0xCC008000 = color.b;
    *(volatile u8*)0xCC008000 = color.a;
    *(volatile u16*)0xCC008000 = 1;
    *(volatile u16*)0xCC008000 = 1;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u16*)0xCC008000 = 0;

    // bottom-left
    *(volatile u16*)0xCC008000 = (u16)x;
    *(volatile u16*)0xCC008000 = (u16)y;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u8*)0xCC008000 = color.r;
    *(volatile u8*)0xCC008000 = color.g;
    *(volatile u8*)0xCC008000 = color.b;
    *(volatile u8*)0xCC008000 = color.a;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u16*)0xCC008000 = 1;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u16*)0xCC008000 = 0;
}

// Emits one glyph quad: the glyph rectangle in the font texture, sheared when
// the italic flag is set, plus an inset copy of the texture rectangle for the
// second texture stage (guessed: TextDrawGlyphQuad).
void TextDrawGlyphQuad(short x, short advance, short y, short height,
                       int texX, int texY, u32* color)
{
    FontArena* font = (FontArena*)lbl_804D5290;
    const u8* c = (const u8*)color;
    short x0 = x;
    short x1 = (short)(x0 + advance);
    short y1 = (short)(y + height);
    short u0 = (short)texX;
    short v0 = (short)texY;
    short u1 = (short)(texX + font->maxCharWidth);
    short v1 = (short)(texY + font->cellWidth);
    short slant;
    short insetX;
    short insetY;

    if (lbl_8049C228.cur.w[9] & 1)
        slant = (short)((s32)lbl_8049C228.cur.w[7] * font->shear /
                        (font->maxCharWidth * 4));
    else
        slant = 0;

    if (lbl_8049C228.cur.w[9] & 4) {
        insetX = 4;
        insetY = 3;
    } else {
        insetX = 0;
        insetY = 0;
    }

    GXBegin(0x80, 3, 4);

    // top-left
    *(volatile u16*)0xCC008000 = (u16)(x0 + slant);
    *(volatile u16*)0xCC008000 = (u16)y1;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u8*)0xCC008000 = c[0];
    *(volatile u8*)0xCC008000 = c[1];
    *(volatile u8*)0xCC008000 = c[2];
    *(volatile u8*)0xCC008000 = c[3];
    *(volatile u16*)0xCC008000 = (u16)u0;
    *(volatile u16*)0xCC008000 = (u16)v0;
    *(volatile u16*)0xCC008000 = (u16)u0;
    *(volatile u16*)0xCC008000 = (u16)v0;

    // top-right
    *(volatile u16*)0xCC008000 = (u16)(x1 + slant);
    *(volatile u16*)0xCC008000 = (u16)y1;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u8*)0xCC008000 = c[0];
    *(volatile u8*)0xCC008000 = c[1];
    *(volatile u8*)0xCC008000 = c[2];
    *(volatile u8*)0xCC008000 = c[3];
    *(volatile u16*)0xCC008000 = (u16)u1;
    *(volatile u16*)0xCC008000 = (u16)v0;
    *(volatile u16*)0xCC008000 = (u16)(u1 - insetX);
    *(volatile u16*)0xCC008000 = (u16)v0;

    // bottom-right
    *(volatile u16*)0xCC008000 = (u16)x1;
    *(volatile u16*)0xCC008000 = (u16)y;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u8*)0xCC008000 = c[0];
    *(volatile u8*)0xCC008000 = c[1];
    *(volatile u8*)0xCC008000 = c[2];
    *(volatile u8*)0xCC008000 = c[3];
    *(volatile u16*)0xCC008000 = (u16)u1;
    *(volatile u16*)0xCC008000 = (u16)v1;
    *(volatile u16*)0xCC008000 = (u16)(u1 - insetX);
    *(volatile u16*)0xCC008000 = (u16)(v1 - insetY);

    // bottom-left
    *(volatile u16*)0xCC008000 = (u16)x;
    *(volatile u16*)0xCC008000 = (u16)y;
    *(volatile u16*)0xCC008000 = 0;
    *(volatile u8*)0xCC008000 = c[0];
    *(volatile u8*)0xCC008000 = c[1];
    *(volatile u8*)0xCC008000 = c[2];
    *(volatile u8*)0xCC008000 = c[3];
    *(volatile u16*)0xCC008000 = (u16)u0;
    *(volatile u16*)0xCC008000 = (u16)v1;
    *(volatile u16*)0xCC008000 = (u16)u0;
    *(volatile u16*)0xCC008000 = (u16)(v1 - insetY);
}

// Begins a text command block: writes the 0xf command id and returns the
// buffer to empty (guessed: TextBegin).
void TextBegin(void)
{
    if (lbl_804D3318 + 1 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 0xF;                 // 0x0: command id
        lbl_804D3318 = p + 1;
        p[1] = 0;                   // 0x1: terminator
    }
}

// Appends the origin/placement command (id 7): one byte and two 16-bit
// values (guessed: TextSetOrigin).
void TextSetOrigin(u8 mode, u16 x, u16 y)
{
    if (lbl_804D3318 + 6 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 7;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u8*)(p + 1) = mode;       // 0x1
        lbl_804D3318 = p + 2;
        *(u16*)(p + 2) = x;         // 0x2
        lbl_804D3318 = p + 4;
        *(u16*)(p + 4) = y;         // 0x4
        lbl_804D3318 = p + 6;
        p[6] = 0;                   // 0x6: terminator
    }
}

// Appends the horizontal-alignment command (id 5) with one mode byte
// (guessed: TextSetAlign).
void TextSetAlign(int align)
{
    if (lbl_804D3318 + 2 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 5;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u8*)(p + 1) = align;      // 0x1: alignment
        lbl_804D3318 = p + 2;
        p[2] = 0;                   // 0x2: terminator
    }
}

// Appends the vertical-alignment command (id 6) with one mode byte
// (guessed: TextSetMode).
void TextSetMode(int mode)
{
    if (lbl_804D3318 + 2 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 6;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u8*)(p + 1) = mode;       // 0x1: mode
        lbl_804D3318 = p + 2;
        p[2] = 0;                   // 0x2: terminator
    }
}

// Appends the colour command (id 2) with four bytes, stored in memory order
// red, green, blue, alpha (guessed: TextSetColor).
void TextSetColor(int r, int g, int b, int a)
{
    if (lbl_804D3318 + 5 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 2;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u8*)(p + 1) = r;          // 0x1: red
        lbl_804D3318 = p + 2;
        *(u8*)(p + 2) = g;          // 0x2: green
        lbl_804D3318 = p + 3;
        *(u8*)(p + 3) = b;          // 0x3: blue
        lbl_804D3318 = p + 4;
        *(u8*)(p + 4) = a;          // 0x4: alpha
        lbl_804D3318 = p + 5;
        p[5] = 0;                   // 0x5: terminator
    }
}

// Appends the position command (id 4) with two 16-bit values (guessed:
// TextSetPosition).
void TextSetPosition(u16 x, u16 y)
{
    if (lbl_804D3318 + 5 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 4;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u16*)(p + 1) = x;         // 0x1: x offset
        lbl_804D3318 = p + 3;
        *(u16*)(p + 3) = y;         // 0x3: y offset
        lbl_804D3318 = p + 5;
        p[5] = 0;                   // 0x5: terminator
    }
}

// Text flag setter.  Empty in the retail build: the flags are carried in the
// command stream by the other setters (guessed: TextSetFlags).
void TextSetFlags(int flags, int value)
{
    (void)flags;
    (void)value;
}

// Appends the font-size command (id 3) with one 16-bit size (guessed:
// TextSetSize).
void TextSetSize(int size)
{
    if (lbl_804D3318 + 3 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 3;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u16*)(p + 1) = size;      // 0x1: size
        lbl_804D3318 = p + 3;
        p[3] = 0;                   // 0x3: terminator
    }
}

// Clamps one text coordinate into [0, max] (guessed: TextClampCoord).
static inline s16 TextClampCoord(int value, int max)
{
    if (value < 0)
        return 0;
    if (value > max)
        return max;
    return value;
}

// Appends the rectangle command (id 10): four clamped screen coordinates and
// four extra values (guessed: TextSetRect).
void TextSetRect(int x, int y, int width, int height, int u, int v, int a,
                 int b)
{
    if (lbl_804D3318 + 0x13 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 10;                              // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u16*)(p + 1) = TextClampCoord(x, 0x280);      // 0x1: x
        lbl_804D3318 = p + 3;
        *(u16*)(p + 3) = TextClampCoord(y, 0x280);      // 0x3: y
        lbl_804D3318 = p + 5;
        *(u16*)(p + 5) = TextClampCoord(width, 0x1E0);  // 0x5: width
        lbl_804D3318 = p + 7;
        *(u16*)(p + 7) = TextClampCoord(height, 0x1E0); // 0x7: height
        lbl_804D3318 = p + 9;
        *(u16*)(p + 9) = u;                     // 0x9
        lbl_804D3318 = p + 0xB;
        *(u16*)(p + 0xB) = v;                   // 0xB
        lbl_804D3318 = p + 0xD;
        *(u32*)(p + 0xD) = a;                   // 0xD
        lbl_804D3318 = p + 0x11;
        *(u32*)(p + 0x11) = b;                  // 0x11
        lbl_804D3318 = p + 0x15;
        p[0x15] = 0;                            // 0x15: terminator
    }
}

// Appends the text-box command (id 9): four clamped screen coordinates
// (guessed: TextSetBox).
void TextSetBox(int x, int y, int width, int height)
{
    if (lbl_804D3318 + 9 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 9;                               // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u16*)(p + 1) = TextClampCoord(x, 0x280);      // 0x1: x
        lbl_804D3318 = p + 3;
        *(u16*)(p + 3) = TextClampCoord(y, 0x280);      // 0x3: y
        lbl_804D3318 = p + 5;
        *(u16*)(p + 5) = TextClampCoord(width, 0x1E0);  // 0x5: width
        lbl_804D3318 = p + 7;
        *(u16*)(p + 7) = TextClampCoord(height, 0x1E0); // 0x7: height
        lbl_804D3318 = p + 9;
        p[9] = 0;                               // 0x9: terminator
    }
}

// Appends the whole text style to the command buffer in one block: mode,
// colour, size, position, alignment and origin (guessed: TextSetStyle).
void TextSetStyle(int red, int green, int blue, int alpha, int size, int align,
                  int x, int y, int textMode, int originMode, int originX,
                  int originY)
{
    if (lbl_804D3318 + 2 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 6;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u8*)(p + 1) = textMode;   // 0x1: vertical mode
        lbl_804D3318 = p + 2;
        p[2] = 0;                   // 0x2: terminator
    }

    if (lbl_804D3318 + 5 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 2;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u8*)(p + 1) = red;        // 0x1
        lbl_804D3318 = p + 2;
        *(u8*)(p + 2) = green;      // 0x2
        lbl_804D3318 = p + 3;
        *(u8*)(p + 3) = blue;       // 0x3
        lbl_804D3318 = p + 4;
        *(u8*)(p + 4) = alpha;      // 0x4
        lbl_804D3318 = p + 5;
        p[5] = 0;                   // 0x5: terminator
    }

    if (lbl_804D3318 + 3 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 3;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u16*)(p + 1) = size;      // 0x1: font size
        lbl_804D3318 = p + 3;
        p[3] = 0;                   // 0x3: terminator
    }

    if (lbl_804D3318 + 5 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 4;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u16*)(p + 1) = x;         // 0x1: x offset
        lbl_804D3318 = p + 3;
        *(u16*)(p + 3) = y;         // 0x3: y offset
        lbl_804D3318 = p + 5;
        p[5] = 0;                   // 0x5: terminator
    }

    if (lbl_804D3318 + 2 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 5;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u8*)(p + 1) = align;      // 0x1: alignment
        lbl_804D3318 = p + 2;
        p[2] = 0;                   // 0x2: terminator
    }

    if (lbl_804D3318 + 6 - lbl_803B4284 < 0x4000) {
        u8* p = lbl_804D3318;

        p[0] = 7;                   // 0x0: command id
        lbl_804D3318 = p + 1;
        *(u8*)(p + 1) = originMode; // 0x1: origin mode
        lbl_804D3318 = p + 2;
        *(u16*)(p + 2) = originX;   // 0x2: origin x
        lbl_804D3318 = p + 4;
        *(u16*)(p + 4) = originY;   // 0x4: origin y
        lbl_804D3318 = p + 6;
        p[6] = 0;                   // 0x6: terminator
    }
}

// Puts the current text state and the first saved state back to the default
// layout and marks the save depth as one (guessed: TextStateInit).
void TextStateInit(void)
{
    TextSavedState* cur = &lbl_8049C228.cur;
    TextSavedState* base = &lbl_8049C228.saved[0];
    GXColor* color = (GXColor*)&cur->w[2];
    u32 size;
    s32 scaled;

    color->r = 0xFF;
    color->g = 0xFF;
    color->b = 0xFF;
    color->a = 0xFF;
    cur->w[3] = 0;
    cur->w[4] = 0x280;
    cur->w[5] = 0;
    cur->w[6] = 0x1E0;
    size = (u32)*(u16*)(lbl_804D5290 + 0x10) * 12;
    cur->w[8] = size;
    scaled = (s32)((f32)((f64)size - lbl_804D8F38) * lbl_804D8F30 /
                   gAspectScale);
    cur->w[9] = 0;
    cur->w[10] = 0;
    cur->w[11] = 0;
    cur->w[12] = 0;
    cur->w[13] = 0;
    cur->w[14] = 0;
    cur->w[15] = 0;
    cur->w[17] = 0;
    cur->w[18] = 0;
    cur->w[19] = 0;
    cur->w[20] = 0;
    cur->w[21] = 0;
    cur->w[22] = 0;
    cur->callback = 0;
    cur->w24 = 0;
    cur->w[7] = scaled;
    cur->w[0] = 0x1E0;
    cur->w[1] = 1;
    cur->w25 = (u32)&TextDrawBeginFrame;
    cur->w26 = 0;

    base->w[0] = 0x1E0;
    base->w[1] = 1;
    base->w[2] = cur->w[2];
    base->w[3] = 0;
    base->w[4] = 0x280;
    base->w[5] = 0;
    base->w[6] = 0x1E0;
    base->w[7] = scaled;
    base->w[8] = size;
    base->w[9] = 0;
    base->w[10] = 0;
    base->w[11] = 0;
    base->w[12] = 0;
    base->w[13] = 0;
    base->w[14] = 0;
    base->w[15] = 0;
    base->w[16] = cur->w[16];
    base->w[17] = 0;
    base->w[18] = 0;
    base->w[19] = 0;
    base->w[20] = 0;
    base->w[21] = 0;
    base->w[22] = 0;
    base->callback = 0;
    base->w24 = 0;
    base->w25 = (u32)&TextDrawBeginFrame;
    base->w26 = 0;

    lbl_804D5298 = 1;
}

// TextSetDebugFlag: enable/disable the text debug overlay.
// Kept under its placeholder name because another unit calls it.
extern "C" void fn_801ED838(int enable)
{
    lbl_804D3314 = enable;
}

// TextDrawBeginFrame: set up the ortho projection for the text pass, then
// the blend mode and z-mode the text quads are drawn with.
void TextDrawBeginFrame(void)
{
    f32 width = (f32)*(u16*)(lbl_804D5290 + 0x1e);
    f32 height = (f32)*(u16*)(lbl_804D5290 + 0x20);
    fn_8010EC10(lbl_804D8F60 / width, lbl_804D8F60 / height, 0);
    fn_8010C880();
    GfxSetBlendMode(1, 4, 5, 0);
    GXSetZMode(0, 7, 0);
}

// ---- Hit effect placement -------------------------------------------------
struct HitObject {
    u8 unk00[0x24];             // 0x00
    s32 pos[3];                 // 0x24: world position
    void (*callback)(int, void*); // 0x30: model/part lookup callback
    u8 unk34[0x38 - 0x34];      // 0x34
    u32 kind;                   // 0x38: object type in the top byte
};

struct HitTarget {
    HitObject* obj;             // 0x00: object the query hit
    u8 unk04[0x0c - 0x04];      // 0x04
    u32 flags;                  // 0x0c
    u8 unk10[0x14 - 0x10];      // 0x10
    s32 bone;                   // 0x14: hit bone index
    u8 unk18[0x30 - 0x18];      // 0x18
    s32 from[3];                // 0x30: query start
    s32 to[3];                  // 0x3c: query end
};

struct HitFilterCtx {
    HitTarget* target;          // 0x00
    s32 bone;                   // 0x04
};

struct HitQueryArg {
    HitObject* obj;             // 0x00
    s32 bone;                   // 0x04: filled in by the callback
};

typedef int (*HitFilter)(void* ctx, void* hit);

extern "C" int fn_800463F0(void);
extern "C" void fn_800E0E9C(s32* out, void* delta, int scale);
extern "C" int fn_8001F928(int a, s32* pos, u32 mask, HitFilter filter,
                           void* ctx);
extern "C" int fn_80183DD0(void* ctx, void* hit);
extern "C" void fn_8018424C(s32* from, s32* to, int value, int type);
extern "C" int rand(void);
void ModelBoneName(int model, int bone);
void AssetFind(u32 hash, u32 id);
f32 Distance3D(f32* a, f32* b);

// Places the impact effect for a hit object between the query endpoints,
// scaled by how far the object moved during the ground query
// (guessed: HitEffectSpawn). main-side units call it through its placeholder
// name, so it keeps C linkage.
extern "C" int fn_801ED8CC(int a, void* hitRaw, int b, int c, void* delta, int d,
                           int e, float f1, float f2)
{
    HitTarget* hit = (HitTarget*)hitRaw;
    HitFilterCtx ctx;
    HitQueryArg arg;
    f32 end[3];
    f32 start[3];
    s32 offset[3];
    s32 base[3];
    s32 pos[3];
    int count;
    int value;

    if (hit == 0)
        return 0;
    if (fn_800463F0() == 0)
        return 0;
    if (hit->obj == 0)
        return 0;

    if (((hit->obj->kind >> 24) & 0xFF) != 2 &&
        ((hit->obj->kind >> 24) & 0xFF) != 0x13)
        return 1;

    ctx.target = hit;
    ctx.bone = -1;

    if (c != 0) {
        arg.obj = hit->obj;
        hit->obj->callback(0x410, &arg);
        ModelBoneName(arg.bone, hit->bone);
        AssetFind(0xF1754BF2, 0x21C5E0);
    }

    if (delta != 0) {
        fn_800E0E9C(offset, delta, b);
        if (d != 0) {
            base[0] = hit->obj->pos[0];
            base[1] = hit->obj->pos[1];
            base[2] = hit->obj->pos[2];
        } else {
            base[0] = hit->from[0];
            base[1] = hit->from[1];
            base[2] = hit->from[2];
        }
    } else {
        offset[0] = 0;
        offset[1] = 0;
        offset[2] = -0x1000;
        fn_800E0E9C(offset, offset, b);
        base[0] = hit->from[0];
        base[1] = hit->from[1];
        base[2] = hit->from[2];
    }
    pos[0] = base[0] + offset[0];
    pos[1] = base[1] + offset[1];
    pos[2] = base[2] + offset[2];

    start[0] = (f32)hit->from[0];
    start[1] = (f32)hit->from[1];
    start[2] = (f32)hit->from[2];

    if (fn_8001F928(0, pos, 0x82000000, fn_80183DD0, &ctx) != 0 &&
        (hit->flags & 0x100) == 0) {
        end[0] = (f32)hit->from[0];
        end[1] = (f32)hit->from[1];
        end[2] = (f32)hit->from[2];
        value = (int)(Distance3D(start, end) / (f32)b * (f1 - f2) + f2);
        if (e != 0)
            count = 2 + (rand() & 1);
        else
            count = 1;
        for (; count != 0; count--)
            fn_8018424C(hit->from, hit->to, value, a);
    }
    return 1;
}

// ---- Player mode advance --------------------------------------------------
struct PlayerModeState {
    u8 unk000[0x8c];            // 0x000
    u32 field8c;                // 0x08c: cleared on mode reset
    u8 unk90[0x98 - 0x90];      // 0x090
    s32 field98;                // 0x098: copy of mode
    u8 unk9c[0xc4 - 0x9c];      // 0x09c
    s32 mode;                   // 0x0c4: -1 while the character is live
    u8 unk0c8[0xf0 - 0xc8];     // 0x0c8
    s32 fieldf0;                // 0x0f0: copy of field11c
    u8 unkf4[0x11c - 0xf4];     // 0x0f4
    s32 field11c;               // 0x11c: -1 when no alternate mode
    u8 unk120[0x638 - 0x120];   // 0x120
    u32 field638;               // 0x638
    u8 unk63c[0x7a4 - 0x63c];   // 0x63c
    s32 action;                 // 0x7a4: PlayerSetValue4 target
    u8 unk7a8[0x7b4 - 0x7a8];   // 0x7a8
    u32 flags;                  // 0x7b4
    u8 unk7b8[0x7d0 - 0x7b8];   // 0x7b8
    s32 field7d0;               // 0x7d0: alternate live mode
    u8 unk7d4[0x26e0 - 0x7d4];  // 0x7d4
    u8 sub26e0[0x26e8 - 0x26e0];// 0x26e0: sub-state passed to fn_80155A84
    u32 field26e8;              // 0x26e8
    u8 unk26ec[0x279c - 0x26ec];// 0x26ec
    s32 field279c;              // 0x279c: -1 when no gun animation
};

extern "C" void fn_800769A0(void);
void PlayerSelectCollisionUpdate(void* self, f32 dt);
void PlayerSetValue4(int* value, int mode);

// Clears the per-frame state, updates selection collision and selects the
// next player action (guessed: PlayerModeUpdate).
void PlayerModeUpdate(void* self, f32 dt)
{
    PlayerModeState* p = (PlayerModeState*)self;

    p->field638 = 0;
    p->flags &= ~1u;
    fn_800769A0();
    PlayerSelectCollisionUpdate(self, dt);

    if (p->mode == -1) {
        if (p->field7d0 == 0)
            PlayerSetValue4(&p->action, 1);
        else
            PlayerSetValue4(&p->action, 3);
    } else {
        PlayerSetValue4(&p->action, 0x16);
    }
}

extern "C" void fn_80155A84(void* state, u32 value, int flags);
extern "C" int fn_80175EFC(void);
void PlayerResetGunTimer(void* self);

// Clears the per-frame mode fields and restores the pending mode values
// (guessed: PlayerRefreshMode).
void PlayerRefreshMode(void* self)
{
    PlayerModeState* p = (PlayerModeState*)self;

    p->field8c = 0;
    if (p->mode != -1)
        p->field98 = p->mode;
    if (p->field11c == -1)
        return;
    p->fieldf0 = p->field11c;
}

// Resets the gun timer and picks the gun action when the character has a gun
// animation (guessed: PlayerSetupGunAction).
void PlayerSetupGunAction(void* self)
{
    PlayerModeState* p = (PlayerModeState*)self;

    PlayerResetGunTimer(p);
    p->field638 = 1;
    p->flags &= ~1u;
    PlayerSetValue4(&p->action, 0x15);
    if (p->field279c == -1 || fn_80175EFC() != 0) {
        PlayerSetValue4(&p->action, 3);
        fn_80155A84(p->sub26e0, p->field26e8, 1);
    }
}

// ---- Player item carry state ----------------------------------------------
struct PlayerItemHost {
    u8 unk000[0x38];             // 0x000
    u32 type;                    // 0x038: object type in the high byte (0x13 = item)
    u8 unk03c[0x270 - 0x3c];     // 0x03c
    u8 conditions[4];            // 0x270: condition set tested by HasCondition
};

struct PlayerItemAnim {
    u8 unk00[0x14];              // 0x00
    s32 field14;                 // 0x14
    s32 field18;                 // 0x18
    s32 field1c;                 // 0x1c
};

struct PlayerItemState {
    u8 unk000[0x8c];                 // 0x000
    int (*handler)(int, void*);      // 0x08c: message handler
    u8 unk090[0x98 - 0x90];          // 0x090
    s32 field98;                     // 0x098
    u8 unk09c[0xa4 - 0x9c];          // 0x09c
    s32 fielda4;                     // 0x0a4
    u8 unka8[0xf0 - 0xa8];           // 0x0a8
    s32 fieldf0;                     // 0x0f0
    u8 unkf4[0xfc - 0xf4];           // 0x0f4
    s32 fieldfc;                     // 0x0fc
    u8 unk100[0x73c - 0x100];        // 0x100
    s32 field73c;                    // 0x73c
    u8 unk740[0x7a4 - 0x740];        // 0x740
    s32 action;                      // 0x7a4: PlayerSetValue4 target
    u8 unk7a8[0x26e8 - 0x7a8];       // 0x7a8
    PlayerItemHost* host;            // 0x26e8: possessed object
};

struct Actor;

extern "C" void* fn_800A384C(void* obj);
extern "C" s32 fn_800A1BC8(s32 a, s32 b, s32 c, s32 d, s32 e);
extern "C" u32 lbl_803B9378[];
void PlayerInHost(Actor* actor);
int HasCondition(void* set, int condition);
int PlayerItemMessageHandler(int msg, void* p);
void PlayerSetValue4(void* value, int mode);

// Starts the item carry state for the possessed object: picks the two item
// animations and installs the item message handler (guessed: PlayerItemBegin).
void PlayerItemBegin(void* self)
{
    PlayerItemState* p = (PlayerItemState*)self;
    PlayerItemHost* host = p->host;
    PlayerItemAnim* anim;
    s32* slot;
    u32 flag = 0;

    if ((host->type >> 24) == 0x13) {
        anim = (PlayerItemAnim*)fn_800A384C(host);
        if (HasCondition(host->conditions, 0xb))
            flag = 1;
    }
    PlayerInHost((Actor*)self);
    slot = (s32*)&lbl_803B9378[0x17D + p->field73c];
    p->field98 = fn_800A1BC8(anim->field14, anim->field18, slot[0], flag, 0);
    p->handler = PlayerItemMessageHandler;
    p->fielda4 = 0x19;
    p->fieldf0 = fn_800A1BC8(anim->field14, anim->field1c, slot[0], flag, 0);
    p->fieldfc = 0x19;
    PlayerSetValue4((void*)&p->action, 0x15);
}

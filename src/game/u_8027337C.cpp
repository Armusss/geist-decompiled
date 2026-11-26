// u_8027337C.cpp (0x8027337C-0x80273650): ODE error reporting (message/debug formatting and the
// handler setters; ODE's error.cpp). Split from u_8026C200.
// Evidence: only these functions use .sbss 0x804D5520-0x804D5528, the ODE message strings at .rodata
// 0x8034AE38 and __files; no .sdata2.
// Names are descriptive guesses; the code is matched against the retail DOL.



#include <stdarg.h>

// The MSL stream table (stdin, stdout, stderr; 0x50 bytes each).
struct MslFile {
    unsigned char data[0x50];
};

extern "C" {
extern MslFile __files[];
int fflush(MslFile* f);
int fprintf(MslFile* f, const char* format, ...);
int vfprintf(MslFile* f, const char* format, va_list ap);
void abort();
void exit(int status);
}

#define stdout (&__files[1])
#define stderr (&__files[2])

typedef void (*OdeErrorHandler)(int num, const char* msg, void* ap);

static OdeErrorHandler sErrorHandler;    // 0x804D5520 (dError's handler)
static OdeErrorHandler sDebugHandler;    // 0x804D5524 (dDebug's handler)
static OdeErrorHandler sMessageHandler;  // 0x804D5528 (dMessage's handler; its setter is dead-stripped)

// Prints "<msg1> <num>: <formatted msg2>" to stderr (ODE's printMessage, expanded in place).
#define PRINT_MESSAGE(num, msg1, msg2, ap)               \
    do {                                                 \
        fflush(stderr);                                  \
        fflush(stdout);                                  \
        if (num) {                                       \
            fprintf(stderr, "\n%s %d: ", msg1, num);     \
        } else {                                         \
            fprintf(stderr, "\n%s: ", msg1);             \
        }                                                \
        vfprintf(stderr, msg2, ap);                      \
        fprintf(stderr, "\n");                           \
        fflush(stderr);                                  \
    } while (0)

// 0x8027337C: ODE message (dMessage)
extern "C" void fn_8027337C(int num, const char* msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    if (sMessageHandler) {
        sMessageHandler(num, msg, ap);
    } else {
        PRINT_MESSAGE(num, "ODE Message", msg, ap);
    }
}

// 0x802734DC: ODE internal error (dDebug): report, then abort
void dDebug(int num, const char* msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    if (sDebugHandler) {
        sDebugHandler(num, msg, ap);
    } else {
        PRINT_MESSAGE(num, "ODE INTERNAL ERROR", msg, ap);
    }
    abort();
}

// 0x80273640: sets dDebug's handler (named SetErrorHandler by its caller u_801D4738)
void SetErrorHandler(OdeErrorHandler fn)
{
    sDebugHandler = fn;
}

// 0x80273648: sets dError's handler (named SetMessageHandler by its caller u_801D4738)
void SetMessageHandler(OdeErrorHandler fn)
{
    sErrorHandler = fn;
}

// ODE fatal error (dError): report, then exit. Nothing in the game calls it, so the linker drops the
// code, but its "ODE Error" string stays at the end of this file's string pool (.rodata 0x8034AE68).
void dError(int num, const char* msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    if (sErrorHandler) {
        sErrorHandler(num, msg, ap);
    } else {
        PRINT_MESSAGE(num, "ODE Error", msg, ap);
    }
    exit(1);
}

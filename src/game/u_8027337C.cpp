// u_8027337C.cpp (0x8027337C-0x80273650): ODE error reporting (message/debug formatting and the
// handler setters; ODE's error.cpp). Split from u_8026C200.
// Evidence: only these functions use .sbss 0x804D5520-0x804D5528, the ODE message strings at .rodata
// 0x8034AE38 and __files; no .sdata2.
// Names are descriptive guesses; the code is matched against the retail DOL.



// TODO (not drafted yet, 0x160 bytes at 0x8027337C): ODE message (varargs formatting through the debug console)
extern "C" void fn_8027337C(int num, const char* msg, ...);

// TODO (not drafted yet, 0x164 bytes at 0x802734DC): ODE internal error (varargs formatting, then aborts)
extern "C" void fn_802734DC(int num, const char* msg, ...);

typedef void (*OdeErrorHandler)(int num, const char* msg, void* ap);

static OdeErrorHandler sErrorHandler;    // 0x804D5524
static OdeErrorHandler sMessageHandler;  // 0x804D5520

// 0x80273640: sets the error handler (dSetErrorHandler)
void SetErrorHandler(OdeErrorHandler fn)
{
    sErrorHandler = fn;
}

// 0x80273648: sets the message handler (dSetMessageHandler)
void SetMessageHandler(OdeErrorHandler fn)
{
    sMessageHandler = fn;
}

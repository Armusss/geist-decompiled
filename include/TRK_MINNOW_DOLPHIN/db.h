#ifndef _TRK_DB_H_
#define _TRK_DB_H_
#include <os.h>
typedef struct DBInterface {
    u32 bPresent;
    u32 exceptionMask;
    void (*ExceptionDestination)(void);
    void* exceptionReturn;
} DBInterface;
extern DBInterface* __DBInterface;
#endif

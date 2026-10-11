#include "abort_exit.h"
#include "critical_regions.h"
#include <cstddef>

extern void (*_dtors[])(void);

static void (*__atexit_funcs[64])(void);

static void (*__console_exit)(void);

void (*__stdio_exit)(void);

static int __atexit_curr_func;

static int __aborting;

// Geist's (older) MSL: abort() goes through exit(), and exit() hands over to __exit().
void __destroy_global_chain(void);
void _ExitProcess(void);
void __exit(int status);

void abort(void) {
    raise(1);
    __aborting = 1;
    exit(1);
}

void exit(int status) {
    void (**dtor)(void);

    if (!__aborting) {
        __begin_critical_region(atexit_funcs_access);
        __end_critical_region(atexit_funcs_access);
        __destroy_global_chain();

        dtor = _dtors;
        while (*dtor != NULL) {
            (*dtor)();
            dtor++;
        }

        if (__stdio_exit != NULL) {
            __stdio_exit();
            __stdio_exit = NULL;
        }
    }

    __exit(status);
}

void __exit(int status) {
    __begin_critical_region(atexit_funcs_access);

    while (__atexit_curr_func > 0)
        __atexit_funcs[--__atexit_curr_func]();

    __end_critical_region(atexit_funcs_access);
    __kill_critical_regions();

    if (__console_exit != NULL) {
        __console_exit();
        __console_exit = NULL;
    }

    _ExitProcess();
}

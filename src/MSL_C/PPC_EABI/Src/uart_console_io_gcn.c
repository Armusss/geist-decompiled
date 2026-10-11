#include "uart_console_io_gcn.h"
#include <os.h>

int InitializeUART(size_t);
int WriteUARTN(unsigned char*, size_t);
int __TRK_write_console(__file_handle handle, unsigned char* buffer, size_t* count,
                        __idle_proc idle_fn);

// Geist: UART setup is split into __init_uart_console; __close_console lives in MetroTRK's mslsupp.c.
static int initialized;

int __init_uart_console(void);

int __write_console(__file_handle handle, unsigned char* buffer, size_t* count,
                    __idle_proc idle_fn) {
    if ((OSGetConsoleType() & 0x20000000) == 0) {
        if (__init_uart_console() != 0) {
            return 1;
        }

        if (WriteUARTN(buffer, *count) != 0) {
            *count = 0;
            return 1;
        }
    }

    __TRK_write_console(handle, buffer, count, idle_fn);
    return 0;
}

int __init_uart_console(void) {
    int err = 0;

    if (initialized == FALSE) {
        err = InitializeUART(0xE100);

        if (err == 0) {
            initialized = 1;
        }
    }

    return err;
}

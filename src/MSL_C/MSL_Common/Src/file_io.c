#include "alloc.h"
#include "critical_regions.h"
#include "buffer_io.h"
#include "file_io.h"
#include "FILE_POS.h"
#include <cctype>

int fclose(FILE* file) {
    int flush_result, close_result;

    if (file == NULL)
        return (-1);
    if (file->file_mode.file_kind == __closed_file)
        return (0);

    flush_result = fflush(file);

    close_result = (*file->close_fn)(file->handle);

    file->file_mode.file_kind = __closed_file;
    file->handle = NULL;

    if (file->file_state.free_buffer)
        free((FILE*)file->buffer);
    return ((flush_result || close_result) ? -1 : 0);
}

int fflush(FILE* file) {
    long pos;
    int ioresult;

    if (file == NULL) {
        return __flush_all();
    }

    if (file->file_state.error != 0 || file->file_mode.file_kind == __closed_file) {
        return -1;
    }

    if (file->file_mode.io_mode == __read) {
        return 0;
    }

    if (file->file_state.io_state >= __rereading) {
        file->file_state.io_state = __reading;
    }

    if (file->file_state.io_state == __reading) {
        file->buffer_length = 0;
    }

    if (file->file_state.io_state != __writing) {
        file->file_state.io_state = __neutral;
        return 0;
    }

    if (file->file_mode.file_kind == __disk_file) {
        pos = ftell(file);
    } else {
        pos = 0;
    }

    ioresult = __flush_buffer(file, 0);

    // Status: the only difference left in this unit; retail reserves r29 here but compares r3 directly.
    if (ioresult) {
        file->file_state.error = 1;
        file->buffer_length = 0;
        return -1;
    }

    file->file_state.io_state = __neutral;
    file->position = pos;
    file->buffer_length = 0;
    return 0;
}

// Geist (older MSL) also has fopen, __handle_reopen and __get_file_modes (emitted before fflush).
void __stdio_atexit(void);
void clearerr(FILE* file);
FILE* __find_unopened_file(void);
void __init_file(FILE* file, file_modes mode, char* buff, size_t size);
int __open_file(const char* name, file_modes mode, __file_handle* handle);
int fseek(FILE* file, unsigned long offset, int mode);
int __get_file_modes(const char* mode, file_modes* modes);
FILE* __handle_reopen(const char* name, const char* mode, FILE* file);

FILE* fopen(const char* name, const char* mode) {
    FILE* file;

    __begin_critical_region(stdin_access);
    file = __handle_reopen(name, mode, __find_unopened_file());
    __end_critical_region(stdin_access);

    return file;
}

FILE* __handle_reopen(const char* name, const char* mode, FILE* file) {
    file_modes modes;

    __stdio_atexit();

    if (!file)
        return NULL;

    fclose(file);
    clearerr(file);

    if (!__get_file_modes(mode, &modes))
        return NULL;

    __init_file(file, modes, 0, 1024);

    if (__open_file(name, modes, &file->handle)) {
        file->file_mode.file_kind = __closed_file;
        if (file->file_state.free_buffer)
            free(file->buffer);
        return NULL;
    }

    if (modes.io_mode & __append)
        fseek(file, 0, 2);

    return file;
}

int __get_file_modes(const char* mode, file_modes* modes) {
    const char* mode_ptr = mode;
    int mode_str;
    unsigned char io_mode;
    unsigned char open_mode;

    modes->file_kind = __disk_file;
    modes->file_orientation = 0;
    modes->binary_io = 0;

    mode_str = *mode_ptr++;

    switch (mode_str) {
    case 'r':
        open_mode = 0;
        break;
    case 'w':
        open_mode = 2;
        break;
    case 'a':
        open_mode = 1;
        break;
    default:
        return 0;
    }

    modes->open_mode = open_mode;

    switch (*mode_ptr++) {
    case 'b':
        modes->binary_io = 1;
        if (*mode_ptr == '+')
            mode_str = (mode_str << 8) | '+';
        break;
    case '+':
        mode_str = (mode_str << 8) | '+';
        if (*mode_ptr == 'b')
            modes->binary_io = 1;
        break;
    }

    switch (mode_str) {
    case 'r':
        io_mode = __read;
        break;
    case 'w':
        io_mode = __write;
        break;
    case 'a':
        io_mode = __write | __append;
        break;
    case 'r+':
        io_mode = __read_write;
        break;
    case 'w+':
        io_mode = __read_write;
        break;
    case 'a+':
        io_mode = __read_write | __append;
        break;
    }

    modes->io_mode = io_mode;
    return 1;
}

int __msl_strnicmp(const char* str1, const char* str2, int n) {
    int i;
    char c1, c2;

    for (i = 0; i < n; i++) {
        c1 = tolower(*str1++);
        c2 = tolower(*str2++);

        if (c1 < c2) {
            return -1;
        }

        if (c1 > c2) {
            return 1;
        }

        if (c1 == '\0') {
            return 0;
        }
    }

    return 0;
}

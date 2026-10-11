#include "buffer_io.h"
#include "critical_regions.h"
#include "direct_io.h"
#include "FILE_POS.h"
#include "misc_io.h"
#include "wchar_io.h"
#include <cstring>

int __load_buffer(FILE* file, size_t* bytes_loaded, int alignment);
size_t __fread(void* buffer, size_t size, size_t count, FILE* file);

#define set_eof(file)                                                                              \
    do {                                                                                           \
        (file)->file_state.io_state = __neutral;                                                   \
        (file)->file_state.eof = 1;                                                                \
        (file)->buffer_length = 0;                                                                 \
    } while (0)
int __flush_line_buffered_output_files(void);

// Geist keeps fread/__fread (MSL reference algorithm, matched against the DOL).
size_t fread(void* buffer, size_t size, size_t count, FILE* stream) {
    size_t retval;

    __begin_critical_region(stdin_access);
    retval = __fread(buffer, size, count, stream);
    __end_critical_region(stdin_access);

    return (retval);
}

size_t __fread(void* buffer, size_t size, size_t count, FILE* file) {
    unsigned char* read_ptr;
    size_t num_bytes, bytes_to_go, bytes_read;
    int ioresult, always_buffer;

    if (fwide(file, 0) == 0) {
        fwide(file, -1);
    }

    bytes_to_go = size * count;

    if (!bytes_to_go || file->file_state.error || file->file_mode.file_kind == __closed_file) {
        return 0;
    }

    always_buffer = !file->file_mode.binary_io || file->file_mode.buffer_mode == _IOFBF;

    if (file->file_state.io_state == __neutral) {
        if (file->file_mode.io_mode & __read) {
            file->file_state.io_state = __reading;
            file->buffer_length = 0;
        }
    }

    if (file->file_state.io_state < __reading) {
        set_error(file);
        return 0;
    }

    if (file->file_mode.buffer_mode & _IOLBF) {
        if (__flush_line_buffered_output_files()) {
            set_error(file);
            return 0;
        }
    }

    read_ptr = (unsigned char*)buffer;
    bytes_read = 0;

    if (bytes_to_go && file->file_state.io_state >= __rereading) {
        do {
            if (fwide(file, 0) == 1) {
                *(wchar_t*)read_ptr = file->ungetc_wide_buffer[file->file_state.io_state - __rereading];
                read_ptr += sizeof(wchar_t);
                bytes_read += sizeof(wchar_t);
                bytes_to_go -= sizeof(wchar_t);
            } else {
                *read_ptr++ = ((unsigned char*)file->ungetc_buffer)[file->file_state.io_state - __rereading];
                bytes_read++;
                bytes_to_go--;
            }

            file->file_state.io_state--;
        } while (bytes_to_go && file->file_state.io_state >= __rereading);

        if (file->file_state.io_state == __reading) {
            file->buffer_length = file->save_buffer_length;
        }
    }

    if (bytes_to_go && (file->buffer_length || always_buffer)) {
        do {
            if (!file->buffer_length) {
                ioresult = __load_buffer(file, NULL, __align_buffer);

                if (ioresult) {
                    if (ioresult == __io_error) {
                        set_error(file);
                    } else {
                        set_eof(file);
                    }

                    bytes_to_go = 0;
                    break;
                }
            }

            num_bytes = file->buffer_length;

            if (num_bytes > bytes_to_go) {
                num_bytes = bytes_to_go;
            }

            memcpy(read_ptr, file->buffer_ptr, num_bytes);

            read_ptr += num_bytes;
            bytes_read += num_bytes;
            bytes_to_go -= num_bytes;

            file->buffer_ptr += num_bytes;
            file->buffer_length -= num_bytes;
        } while (bytes_to_go && always_buffer);
    }

    if (bytes_to_go && !always_buffer) {
        unsigned char* save_buffer = file->buffer;
        size_t save_size = file->buffer_size;

        file->buffer = read_ptr;
        file->buffer_size = bytes_to_go;

        ioresult = __load_buffer(file, &num_bytes, __dont_align_buffer);

        if (ioresult) {
            if (ioresult == __io_error) {
                set_error(file);
            } else {
                set_eof(file);
            }
        }

        bytes_read += num_bytes;

        file->buffer = save_buffer;
        file->buffer_size = save_size;

        __prep_buffer(file);

        file->buffer_length = 0;
    }

    return (bytes_read / size);
}

size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream) {
    size_t retval;

    __begin_critical_region(stdin_access);
    retval = __fwrite(buffer, size, count, stream);
    __end_critical_region(stdin_access);

    return (retval);
}

size_t __fwrite(const void* buffer, size_t size, size_t count, FILE* stream) {
    unsigned char* write_ptr;
    size_t num_bytes, bytes_to_go, bytes_written;
    int ioresult, always_buffer;

#ifndef __NO_WIDE_CHAR
    if (fwide(stream, 0) == 0)
        fwide(stream, -1);
#endif

    bytes_to_go = size * count;

    if (!bytes_to_go || stream->file_state.error || stream->file_mode.file_kind == __closed_file)
        return 0;

    if (stream->file_mode.file_kind == __console_file)
        __stdio_atexit();

    always_buffer = !stream->file_mode.binary_io || stream->file_mode.buffer_mode == _IOFBF ||
                    stream->file_mode.buffer_mode == _IOLBF;

    if (stream->file_state.io_state == __neutral) {
        if (stream->file_mode.io_mode & __write) {
            if (stream->file_mode.io_mode & __append) {
                #if PLATFORM_GCN
                if (fseek(stream, 0, SEEK_END))
                #else
                if (_fseek(stream, 0, SEEK_END))
                #endif
                    return 0;
            }
            stream->file_state.io_state = __writing;

            __prep_buffer(stream);
        }
    }

    if (stream->file_state.io_state != __writing) {
        set_error(stream);
        return 0;
    }

    write_ptr = (unsigned char*)buffer;
    bytes_written = 0;

    if (bytes_to_go && (stream->buffer_ptr != stream->buffer || always_buffer)) {
        stream->buffer_length = stream->buffer_size - (stream->buffer_ptr - stream->buffer);

        do {
            unsigned char* newline = NULL;

            num_bytes = stream->buffer_length;

            if (num_bytes > bytes_to_go)
                num_bytes = bytes_to_go;

            if (stream->file_mode.buffer_mode == _IOLBF && num_bytes)
                if ((newline = (unsigned char*)__memrchr(write_ptr, '\n', num_bytes)) != NULL)
                    num_bytes = newline + 1 - write_ptr;

            if (num_bytes) {
                memcpy(stream->buffer_ptr, write_ptr, num_bytes);

                write_ptr += num_bytes;
                #if PLATFORM_GCN
                bytes_written += num_bytes;
                #endif
                bytes_to_go -= num_bytes;

                stream->buffer_ptr += num_bytes;
                stream->buffer_length -= num_bytes;
            }

            if (!stream->buffer_length || newline != NULL ||
                (stream->file_mode.buffer_mode == _IONBF))
            {
                ioresult = __flush_buffer(stream, NULL);

                if (ioresult) {
                    set_error(stream);
                    bytes_to_go = 0;
                    break;
                }
            }
            #if !PLATFORM_GCN
            bytes_written += num_bytes;
            #endif
        } while (bytes_to_go && always_buffer);
    }

    if (bytes_to_go && !always_buffer) {
        unsigned char* save_buffer = stream->buffer;
        size_t save_size = stream->buffer_size;

        stream->buffer = write_ptr;
        stream->buffer_size = bytes_to_go;
        stream->buffer_ptr = write_ptr + bytes_to_go;

    #if PLATFORM_GCN
        if (__flush_buffer(stream, &num_bytes) != __no_io_error)
            set_error(stream);

        bytes_written += num_bytes;
    #else
        if (__flush_buffer(stream, &num_bytes) != __no_io_error)
            set_error(stream);
        else
            bytes_written += num_bytes;
    #endif

        stream->buffer = save_buffer;
        stream->buffer_size = save_size;

        __prep_buffer(stream);

        stream->buffer_length = 0;
    }

    if (stream->file_mode.buffer_mode != _IOFBF)
        stream->buffer_length = 0;

    #if PLATFORM_GCN
    return ((bytes_written + size - 1) / size);
    #else
    return bytes_written / size;
    #endif
}

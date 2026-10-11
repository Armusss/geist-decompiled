#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msghndlr.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/target_options.h"
#include "TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.h"

// MSL's __file_modes (ansi_files.h), repeated here because that header's console prototypes differ.
typedef struct {
    unsigned int open_mode : 2;
    unsigned int io_mode : 3;
    unsigned int buffer_mode : 2;
    unsigned int file_kind : 3;
    unsigned int file_orientation : 2;
    unsigned int binary_io : 1;
} file_modes;

// Geist links MetroTRK v2.0's MSL support layer: console and file I/O over the TRK connection,
// including open/close/position. Built with -inline deferred, so the DOL holds these functions in
// reverse order and __access_file/__close_file are inlined into their callers.

static DSIOResult __access_file(u32 handle, u8* buffer, size_t* count, void* ref_con,
                                MessageCommandID cmd);
DSIOResult __read_file(u32 handle, u8* buffer, size_t* count, void* ref_con);
DSIOResult __write_file(u32 handle, u8* buffer, size_t* count, void* ref_con);
DSIOResult __close_file(u32 handle);

DSIOResult __read_console(u32 handle, u8* buffer, size_t* count, void* ref_con) {
    if (GetUseSerialIO() == 0) {
        return DS_IOError;
    }
    return __read_file(DS_Stdin, buffer, count, ref_con);
}

DSIOResult __TRK_write_console(u32 handle, u8* buffer, size_t* count, void* ref_con) {
    if (GetUseSerialIO() == 0) {
        return DS_IOError;
    }
    return __write_file(DS_Stdout, buffer, count, ref_con);
}

DSIOResult __close_console(u32 handle) {
    return __close_file(handle);
}

DSIOResult __read_file(u32 handle, u8* buffer, size_t* count, void* ref_con) {
    return __access_file(handle, buffer, count, ref_con, DSMSG_ReadFile);
}

DSIOResult __write_file(u32 handle, u8* buffer, size_t* count, void* ref_con) {
    return __access_file(handle, buffer, count, ref_con, DSMSG_WriteFile);
}

static DSIOResult __access_file(u32 handle, u8* buffer, size_t* count, void* ref_con,
                                MessageCommandID cmd) {
    size_t countTemp;
    u32 r0;

    if (GetTRKConnected() == DS_NoError) {
        return DS_IOError;
    }

    countTemp = *count;
    r0 = TRKAccessFile(cmd, handle, &countTemp, buffer);
    *count = countTemp;

    switch ((u8)r0) {
    case DS_IONoError:
        return DS_IONoError;
    case DS_IOEOF:
        return DS_IOEOF;
    }

    return DS_IOError;
}

DSIOResult __open_file(const char* name, file_modes mode, u32* handle) {
    u8 trk_mode;
    u32 r0;
    int open_mode;
    int io_mode;
    u32 binary_io;

    if (GetTRKConnected() == DS_NoError) {
        return DS_IOError;
    }

    open_mode = mode.open_mode;
    trk_mode = 0;
    io_mode = mode.io_mode;
    binary_io = mode.binary_io;

    switch (open_mode) {
    case 0: // must exist
        trk_mode |= 0x01;
        break;
    case 2: // create or truncate
        trk_mode |= 0x02;
        break;
    case 1: // create if necessary
        trk_mode |= 0x04;
        break;
    }

    switch (io_mode) {
    case 1: // read
        trk_mode |= 0x01;
        break;
    case 2: // write
        trk_mode |= 0x02;
        break;
    case 6:
        trk_mode |= 0x04;
        break;
    case 3: // read+write
        trk_mode |= 0x12;
        break;
    case 7:
        trk_mode |= 0x07;
        break;
    }

    if (binary_io == 1) {
        trk_mode |= 0x08;
    }

    r0 = TRKOpenFile(DSMSG_OpenFile, name, trk_mode, handle);

    switch ((u8)r0) {
    case DS_IONoError:
        return DS_IONoError;
    case DS_IOEOF:
        return DS_IOEOF;
    }

    return DS_IOError;
}

DSIOResult __close_file(u32 handle) {
    u32 r0;

    if (GetTRKConnected() == DS_NoError) {
        return DS_IOError;
    }

    r0 = TRKCloseFile(DSMSG_CloseFile, handle);

    switch ((u8)r0) {
    case DS_IONoError:
        return DS_IONoError;
    case DS_IOEOF:
        return DS_IOEOF;
    }

    return DS_IOError;
}

DSIOResult __position_file(u32 handle, u32* position, int mode) {
    int trk_mode = 0;
    u32 r0;

    if (GetTRKConnected() == DS_NoError) {
        return DS_IOError;
    }

    if (mode == 0) {
        trk_mode = 0;
    } else if (mode == 1) {
        trk_mode = 1;
    } else if (mode == 2) {
        trk_mode = 2;
    }

    r0 = TRKPositionFile(DSMSG_PositionFile, handle, position, trk_mode);

    switch ((u8)r0) {
    case DS_IONoError:
        return DS_IONoError;
    case DS_IOEOF:
        return DS_IOEOF;
    }

    return DS_IOError;
}

#include "FILE_POS.h"
#include "critical_regions.h"
#include "errno.h"

// Geist (older MSL): _ftell/_fseek follow PrimeDecomp/prime's matched FILE_POS.c (field names adapted);
// ftell from zeldaret/tp; fseek copies the offset to an fpos_t first.
int __flush_buffer(FILE* file, size_t* bytes_flushed);

fpos_t _ftell(FILE* file) {
  int charsInUndoBuffer = 0;
  fpos_t position;

  unsigned char tmp_kind = file->file_mode.file_kind;
  if (!(tmp_kind == __disk_file || tmp_kind == __console_file) || file->file_state.error) {
    errno = 0x28;
    return (-1L);
  }

  if (file->file_state.io_state == __neutral)
    return (file->position);

  position = file->buffer_position + (file->buffer_ptr - file->buffer);

  if (file->file_state.io_state >= __rereading) {
    charsInUndoBuffer = file->file_state.io_state - __rereading + 1;
    position -= charsInUndoBuffer;
  }

  return (position);
}

long ftell(FILE* file) {
    int retval;

    __begin_critical_region(stdin_access);
    retval = (long)_ftell(file);
    __end_critical_region(stdin_access);
    return retval;
}

int _fseek(FILE* file, fpos_t offset, int mode) {
  fpos_t position;
  __pos_proc pos_proc;

  unsigned char tmp_kind = file->file_mode.file_kind;
  if (!(tmp_kind == __disk_file) || file->file_state.error) {
    errno = 0x28;
    return (-1);
  }

  if (file->file_state.io_state == __writing) {
    if (__flush_buffer(file, NULL) != __no_io_error) {
      set_error(file);
      errno = 0x28;
      return (-1);
    }
  }

  if (mode == SEEK_CUR) {

    mode = SEEK_SET;

    if ((position = _ftell(file)) < 0)
      position = 0;

    offset += position;
  }

  if ((mode != SEEK_END) && (file->file_mode.io_mode != __read_write) &&
      ((file->file_state.io_state == __reading) || (file->file_state.io_state == __rereading))) {
    if ((offset >= file->position) || offset < file->buffer_position) {
      file->file_state.io_state = __neutral;
    } else {
      file->buffer_ptr = file->buffer + (offset - file->buffer_position);
      file->buffer_length = file->position - offset;
      file->file_state.io_state = __reading;
    }
  } else {
    file->file_state.io_state = __neutral;
  }

  if (file->file_state.io_state == __neutral) {
    if ((pos_proc = file->position_fn) != 0 &&
        (*pos_proc)(file->handle, &offset, mode, file->idle_fn)) {
      set_error(file);
      errno = 0x28;
      return (-1);
    }

    file->file_state.eof = 0;
    file->position = offset;
    file->buffer_length = 0;
  }

  return 0;
}

int fseek(FILE* file, unsigned long offset, int mode) {
    fpos_t real_offset = (fpos_t)offset;
    int retval;

    __begin_critical_region(stdin_access);
    retval = _fseek(file, real_offset, mode);
    __end_critical_region(stdin_access);

    return retval;
}

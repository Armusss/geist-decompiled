#ifndef PPC_EXPORT_TARGSUPP_H
#define PPC_EXPORT_TARGSUPP_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 TRKAccessFile(u32, u32, u32*, u8*);
u32 TRKOpenFile(u32 cmd, const char* name, u32 mode, u32* handle);
u32 TRKCloseFile(u32 cmd, u32 handle);
u32 TRKPositionFile(u32 cmd, u32 handle, u32* position, u8 mode);

#ifdef __cplusplus
}
#endif

#endif /* PPC_EXPORT_TARGSUPP_H */

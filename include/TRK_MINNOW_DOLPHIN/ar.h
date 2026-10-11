#ifndef _TRK_AR_H_
#define _TRK_AR_H_
#include <types.h>
#define ARAM_DIR_MRAM_TO_ARAM 0
#define ARAM_DIR_ARAM_TO_MRAM 1
u32 ARGetDMAStatus(void);
void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length);
#endif

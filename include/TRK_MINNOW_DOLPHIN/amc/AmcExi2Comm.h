#ifndef _TRK_AMCEXI2COMM_H_
#define _TRK_AMCEXI2COMM_H_
#include <os.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { AMC_EXI_NO_ERROR = 0, AMC_EXI_UNSELECTED } AmcExiError;
void EXI2_Init(volatile unsigned char** inputPendingPtrRef, EXICallback monitorCallback);
void EXI2_EnableInterrupts(void);
int EXI2_Poll(void);
AmcExiError EXI2_ReadN(void* bytes, unsigned long length);
AmcExiError EXI2_WriteN(const void* bytes, unsigned long length);
void EXI2_Reserve(void);
void EXI2_Unreserve(void);
int AMC_IsStub(void);
#ifdef __cplusplus
}
#endif
#endif

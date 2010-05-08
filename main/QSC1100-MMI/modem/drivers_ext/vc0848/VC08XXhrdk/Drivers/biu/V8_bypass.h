#ifndef _V8_TRM_BYPASS_H_
#define _V8_TRM_BYPASS_H_
#include "mmd_common.h"
#ifdef __cplusplus
extern "C" {
#endif

void                Vm_SetRSLow(MMD_U16 val);
void                Vm_SetRSHigh(MMD_U16 val);
MMD_U16      Vm_GetRSLow(void);
MMD_U16     Vm_GetRSHigh(void);

    
#ifdef __cplusplus
}
#endif

#endif


#ifndef _V8_RDK_BIU_H_
#define _V8_RDK_BIU_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "..\Drivers\biu\v8_baserw.h"
#include "..\Drivers\biu\v8_reg.h"

#ifdef MMD_BIU_DATABUS16BIT
#define Vm_SetReg          		V8_Mul16SetReg  
#define Vm_GetReg     		V8_Mul16GetReg
#define Vm_WrSram			V8_Mul16WrSram
#define Vm_RdSram		  	V8_Mul16RdSram
#define Vm_SetBiuHdFlag		V8_Mul16SetBiuHdFlag

#define Vm_SetReg_NEX			V8_Mul16SetReg_NEX
#define Vm_GetReg_NEX	          	V8_Mul16GetReg_NEX
#define Vm_WrSram_NEX			V8_Mul16WrSram_NEX
#define Vm_RdSram_NEX			V8_Mul16RdSram_NEX
#define Vm_SetBiuHdFlag_NEX       	V8_Mul16SetBiuHdFlag_NEX
#else
#define Vm_SetReg			V8_Mul8SetReg  
#define Vm_GetReg         		V8_Mul8GetReg
#define Vm_WrSram     		V8_Mul8WrSram
#define Vm_RdSram			V8_Mul8RdSram
#define Vm_SetBiuHdFlag		V8_Mul8SetBiuHdFlag

#define Vm_SetReg_NEX			V8_Mul8SetReg_NEX
#define Vm_GetReg_NEX			V8_Mul8GetReg_NEX
#define Vm_WrSram_NEX			V8_Mul8WrSram_NEX
#define Vm_RdSram_NEX			V8_Mul8RdSram_NEX
#define Vm_SetBiuHdFlag_NEX		V8_Mul8SetBiuHdFlag_NEX
#endif

#define HAL_READ_UINT32( _register_, _value_ ) \
        ((_value_) = Vm_GetReg(_register_))

#define HAL_WRITE_UINT32( _register_, _value_ ) \
        (Vm_SetReg(_register_, _value_))

void Biu_init(void);

MMD_U32 MMD_BIU_IRQGetStatusAll_NEX(void);
MMD_U32 MMD_BIU_IRQGetEnableAll_NEX(void);
MMD_U32 MMD_BIU_IRQGetStatus_NEX(void);
void    MMD_BIU_AssertIRQ_NEX(void);

// temp use, remove later
void Host_HIF_SetReg8(MMD_U32 adr, MMD_U08 val);
MMD_U08 Host_HIF_GetReg8(MMD_U32 adr);       
void Host_HIF_SetReg16(MMD_U32 adr, MMD_U16 val);       
void Host_HIF_SetReg32(MMD_U32 adr, MMD_U32 val);       
MMD_U32 Host_HIF_GetReg32(MMD_U32 adr);       

#ifdef __cplusplus
}
#endif

#endif //_V8_RDK_BIU_H_


#ifndef _V8_TRM_BASERW_H_
#define _V8_TRM_BASERW_H_

/* RDK header */
#include "mmp_rdk.h"

#ifdef __cplusplus
extern "C" {
#endif


#define REG_BIU_BASE                     0x60000000

#define REG_BIU_CMD						 (REG_BIU_BASE+0x000)
#define REG_BIU_SET						 (REG_BIU_BASE+0x004)
#define REG_BIU_STATUS					 (REG_BIU_BASE+0x008)
#define REG_BIU_BURST_LEN				 (REG_BIU_BASE+0x00c)
#define REG_BIU_BASE_ADDR				 (REG_BIU_BASE+0x010)
#define REG_BIU_LOCOSTO_CTRL			 (REG_BIU_BASE+0x014)
#define REG_BIU_BREAK_ON				 (REG_BIU_BASE+0x0b0)
#define REG_BIU_BREAK_OFF				 (REG_BIU_BASE+0x0b4)
#define REG_BIU_BREAK_RST				 (REG_BIU_BASE+0x0bc)


void MMD_BIU_Init(MMD_U32 data_width);

 void    V8_Mul8SetReg(MMD_U32 addr, MMD_U32 dat);
 MMD_U32  V8_Mul8GetReg(MMD_U32 addr);
 MMD_U32  V8_Mul8WrSram(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 MMD_U32  V8_Mul8RdSram(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 void    V8_Mul8SetBiuHdFlag(MMD_U32 flag);

 void    V8_Mul16SetReg(MMD_U32 addr, MMD_U32 dat);
 MMD_U32  V8_Mul16GetReg(MMD_U32 addr);
 MMD_U32  V8_Mul16WrSram(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 MMD_U32  V8_Mul16WrSramOnWord(MMD_U32 addr, MMD_U16 data, MMD_U32 size);
 MMD_U32  V8_Mul16RdSram(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 //MMD_U32  V8_Mul16RdSramForCap(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 void    V8_Mul16SetBiuHdFlag(MMD_U32 flag);

 void    V8_Mul8SetReg_NEX(MMD_U32 addr, MMD_U32 dat);
 MMD_U32  V8_Mul8GetReg_NEX(MMD_U32 addr);
 MMD_U32  V8_Mul8WrSram_NEX(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 MMD_U32  V8_Mul16WrSram_NEX_OnWord(MMD_U32 addr, MMD_U16 data, MMD_U32 size);
 MMD_U32  V8_Mul8RdSram_NEX(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 void    V8_Mul8SetBiuHdFlag_NEX(MMD_U32 flag);

 void    V8_Mul16SetReg_NEX(MMD_U32 addr, MMD_U32 dat);
 MMD_U32  V8_Mul16GetReg_NEX(MMD_U32 addr);
 MMD_U32  V8_Mul16WrSram_NEX(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 MMD_U32  V8_Mul16RdSram_NEX(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 //MMD_U32  V8_Mul16RdSram_NEXForCap(MMD_U32 addr, MMD_S08 *pbuf, MMD_U32 size);
 void    V8_Mul16SetBiuHdFlag_NEX(MMD_U32 flag);

 void Host_HIF_SetReg8(MMD_U32 adr, MMD_U08 val);
 MMD_U08 Host_HIF_GetReg8(MMD_U32 adr);
 void Host_HIF_SetReg16(MMD_U32 adr, MMD_U16 val) ;
 MMD_U16 Host_HIF_GetReg16(MMD_U32 adr);
 void Host_HIF_SetReg32(MMD_U32 adr, MMD_U32 val) ;
 MMD_U32 Host_HIF_GetReg32(MMD_U32 adr);

#ifdef __cplusplus
}
#endif

#endif


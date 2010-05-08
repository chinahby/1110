/*--------------------------------------------------------------------

			COPYRIGHT (C) 2008, Vimicro Corporation
                        ALL RIGHTS RESERVED

 This source code has been made available to you by VIMICRO on an
 AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 copyrights to use it in any way he or she deems fit, including copying it,
 modifying it, compiling it, and redistributing it either with or without
 modifications. Any person who transfers this source code or any derivative
 work must include the VIMICRO copyright notice and this paragraph in
 the transferred software.

 Module:		mmd_sd_driver.h.
 
 Description:	VC0848 SDPIO Driver header file.
 
 Update History:
	1.0			Initial version					2008.04.08		Chen Jing'en

-----------------------------------------------------------------------*/
#ifndef _MMD_SD_DRIVER_H_
#define _MMD_SD_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include "mmp_rdk.h"
#include "..\drivers\sd\VIM_SD_Driver.h"
#include "mmd_data_type.h"

#define SD_MULTI_MAX_ITEM_SIZE 		20 	// n sectors = 512*n bytes

#define MMD_SD_Malloc 	MMD_Malloc
#define MMD_SD_Free 	MMD_Free
#define MMD_SD_MemCpy 	MMD_MemCpy

MMD_U32 MMD_SD_Reset(void);
MMD_U32	MMD_SD_ReadSector(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr);
MMD_U32	MMD_SD_WriteSector(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr);
MMD_U32 MMD_SD_ReadMultiSector(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr, MMD_U32 dNum);
MMD_U32	MMD_SD_WriteMultiSector(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr, MMD_U32 dNum);
MMD_U32	MMD_SD_GetInfo(SDIO_CARD_INFORMATION *psInfo);
MMD_U32	MMD_SD_GetCapacity(void);
MMD_U32	MMD_SD_GetMinErasableSize(void);
MMD_U32	MMD_SD_Erase(MMD_U32 dSecAddr, MMD_U32 dNum);

#ifdef __cplusplus
}
#endif

#endif /* _MMD_SD_DRIVER_H_ */


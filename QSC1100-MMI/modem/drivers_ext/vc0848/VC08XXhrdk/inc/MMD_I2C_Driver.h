/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED   2007                                                   
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : MMD_I2C_Driver.h
 * [Description]          : I2C Driver Header
 * [Author]                 : An Xinfang
 * [Date Of Creation]  : 2007-04-11 
 * [Platform]              : ARM
 * [Note]                   :  None   
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date                      Author         Modifications
 * ------------------------------------------------------------------------------
 * 2007-04-11         An Xinfang       Created
 *******************************************************************************/
 /*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/
 #ifndef _MMD_I2C_DRIVER_H_
 #define _MMD_I2C_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * INCLUDE FILE                                              
 */
#include "mmp_rdk.h"

//#include "..\common_ae\MMD_COMMON_TypeDef.h"

#define I2C_DBG                     /*I2C Driver can't work in DEBUG MODE,because this causes time-out*/
#ifdef I2C_DBG
	
       #define I2CPRINTF  MMD_PRINTF
#else
	#define I2CPRINTF(_x_)
  
#endif

/*
 * MACRO DEFINITION
 */
 
/* I2C Transfer Mode */
#define MMD_I2C_MODE_STANDARD     0                 /* Transfer Mode: Standard(100kbps) */
#define MMD_I2C_MODE_FAST              1                  /* Transfer Mode: Fast    (400kbps) */

/*I2C Return Value */
#define MMD_I2C_RET_NOERR               0x00000000 /* I2C No Error  */
#define MMD_I2C_RET_NACK_DATA      0x00000001 /* I2C No ACK for data      */
#define MMD_I2C_RET_NACK_ADDR      0x00000002 /* I2C No ACK for Addr      */
#define MMD_I2C_RET_ARBIT_LOST      0x00000003 /* Arbitration occurs       */
#define MMD_I2C_RET_INVALIDPARAM 0x00000004 /* I2C Invalid Parameter set  */
#define MMD_I2C_RET_PROCESSING     0x00000005 /* I2C is processing  */
#define MMD_I2C_RET_TIMEOUT           0x00000006 /* Communication time-out    */


#define MMD_I2C_RET_ERR                   0xffffffff   /* */

#ifndef NULL
#define NULL ((void*)(0))
#endif 

/*
 * PROTOTYPE DECLARATION                                     
 */


/* Initialization of I2C */
void MMD_I2C_Initialize(void);
/* I2C Processing Start */
MMD_U32 MMD_I2C_Start(void);
/* I2C Processing End */
MMD_U32 MMD_I2C_Stop(void);
/* I2C Read command issue */
MMD_U32 MMD_I2C_Read(MMD_U08 sladdr, MMD_U08 *buf, MMD_U32 bytes);
/* I2C Write command issue */
MMD_U32 MMD_I2C_Write(MMD_U08 sladdr, MMD_U08 *buf, MMD_U32 bytes);
/* I2C Transfer mode set */
MMD_U32 MMD_I2C_ModeSet(MMD_U08 mode);
#ifdef __cplusplus
	}
#endif

#endif/*_MMD_I2C_DRIVER_H_*/



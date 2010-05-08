/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_driveric_driver.h
 * [Description]          : VC0820 lcdc, lcdif module relevant API.
 * [Author]               : zhangxf
 * [Date Of Creation]     : 2007-03-13
 * [Platform]             : Platform independent
 * [Note]                 : None
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        Author     Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-13  zhangxf    Created
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

#ifndef _MMD_DRIVERIC_H_
#define _MMD_DRIVERIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * INCLUDE FILE                                              
 */
 #include "mmp_rdk.h"

typedef	enum PANEL_INDEX
{
	PNL_MAIN = 1,   // main panel
	PNL_SUB  = 2,     // the second panel connect by cs2
	PNL_RESERVED=0x7fffffff
} pnl_idx;

//lcdif head config
typedef enum _MMD_LCDIF_SEL_CS
{
	MMD_LCDIF_CS1=0,
	MMD_LCDIF_CS2,
	MMD_LCDIF_SEL_CS_ERROR=0x7fffffff
}MMD_LCDIF_SEL_CS;

extern volatile MMD_U32 g_BIU_Count;
#define BIU_INC()		{g_BIU_Count++;}
#define BIU_DEC()		{g_BIU_Count--;}

/***********************************************************************************/
#define  MMD_DELAY_USE_INFO		1			//wendy guo modifyied from 0 to 1, 2/16/2007

#define 	MMD_REG_LCDIF_BASE						0x60090000

/***********************************************************************************/
/***********************************************************************************/
/*! LCDIF registers */
#define MMD_REG_LCDIF_CFG						(MMD_REG_LCDIF_BASE+0x000)
#define MMD_REG_LCDIF_LCD_RESET					(MMD_REG_LCDIF_BASE+0x004)
#define MMD_REG_LCDIF_DBI_TIM					(MMD_REG_LCDIF_BASE+0x008)
#define MMD_REG_LCDIF_DPI_HTIM0					(MMD_REG_LCDIF_BASE+0x00c)
#define MMD_REG_LCDIF_DPI_HTIM1					(MMD_REG_LCDIF_BASE+0x010)
#define MMD_REG_LCDIF_DPI_VTIM0					(MMD_REG_LCDIF_BASE+0x014)
#define MMD_REG_LCDIF_DPI_VTIM1					(MMD_REG_LCDIF_BASE+0x018)
#define MMD_REG_LCDIF_DAT_FMT					(MMD_REG_LCDIF_BASE+0x01c)
#define MMD_REG_LCDIF_EXRW_COMM_DATA			(MMD_REG_LCDIF_BASE+0x020)
#define MMD_REG_LCDIF_EXRW_EN					(MMD_REG_LCDIF_BASE+0x024)
#define MMD_REG_LCDIF_EXR_DATA					(MMD_REG_LCDIF_BASE+0x028)
#define MMD_REG_LCDIF_HEAD_CONFIG				(MMD_REG_LCDIF_BASE+0x02c)
#define MMD_REG_LCDIF_HEAD_RS_FLAG				(MMD_REG_LCDIF_BASE+0x030)
#define MMD_REG_LCDIF_HEAD_CS_FLAG				(MMD_REG_LCDIF_BASE+0x034)
#define MMD_REG_LCDIF_START_POS				(MMD_REG_LCDIF_BASE+0x038)
#define MMD_REG_LCDIF_END_POS					(MMD_REG_LCDIF_BASE+0x03c)
#define MMD_REG_LCDIF_BIAS						(MMD_REG_LCDIF_BASE+0x040)
#define MMD_REG_LCDIF_HEAD_PROCESS_FLAG		(MMD_REG_LCDIF_BASE+0x044)
#define MMD_REG_LCDIF_START_COMM_CODE			(MMD_REG_LCDIF_BASE+0x048)
#define MMD_REG_LCDIF_END_COMM_CODE			(MMD_REG_LCDIF_BASE+0x04c)
#define MMD_REG_LCDIF_REPEAT_START_POS		(MMD_REG_LCDIF_BASE+0x050)
#define MMD_REG_LCDIF_REPEAT_END_POS			(MMD_REG_LCDIF_BASE+0x054)
#define MMD_REG_LCDIF_HEAD_REPEAT_RS_FLAG	(MMD_REG_LCDIF_BASE+0x058)
#define MMD_REG_LCDIF_HEAD_REPEAT_CS_FLAG	(MMD_REG_LCDIF_BASE+0x05c)
#define MMD_REG_LCDIF_HEAD_DATA01				(MMD_REG_LCDIF_BASE+0x060)
#define MMD_REG_LCDIF_HEAD_DATA23				(MMD_REG_LCDIF_BASE+0x064)
#define MMD_REG_LCDIF_HEAD_DATA45				(MMD_REG_LCDIF_BASE+0x068)
#define MMD_REG_LCDIF_HEAD_DATA67				(MMD_REG_LCDIF_BASE+0x06c)
#define MMD_REG_LCDIF_HEAD_DATA89				(MMD_REG_LCDIF_BASE+0x070)
#define MMD_REG_LCDIF_HEAD_DATA1011			(MMD_REG_LCDIF_BASE+0x074)
#define MMD_REG_LCDIF_HEAD_DATA1213			(MMD_REG_LCDIF_BASE+0x078)
#define MMD_REG_LCDIF_HEAD_DATA1415			(MMD_REG_LCDIF_BASE+0x07c)
#define MMD_REG_LCDIF_HEAD_DATA1617			(MMD_REG_LCDIF_BASE+0x080)
#define MMD_REG_LCDIF_HEAD_DATA1819			(MMD_REG_LCDIF_BASE+0x084)
#define MMD_REG_LCDIF_HEAD_DATA2021			(MMD_REG_LCDIF_BASE+0x088)
#define MMD_REG_LCDIF_HEAD_DATA2223			(MMD_REG_LCDIF_BASE+0x08c)
#define MMD_REG_LCDIF_HEAD_DATA2425			(MMD_REG_LCDIF_BASE+0x090)
#define MMD_REG_LCDIF_HEAD_DATA2627			(MMD_REG_LCDIF_BASE+0x094)
#define MMD_REG_LCDIF_HEAD_DATA2829			(MMD_REG_LCDIF_BASE+0x098)
#define MMD_REG_LCDIF_HEAD_DATA3031			(MMD_REG_LCDIF_BASE+0x09c)
#define MMD_REG_LCDIF_SERIAL_SEND				(MMD_REG_LCDIF_BASE+0x0a0)
#define MMD_REG_LCDIF_SERIAL_COMM_DATA		(MMD_REG_LCDIF_BASE+0x0ac)
/***********************************************************************************/

/*
 * PROTOTYPE DECLARATION                                     
 * 
 */
void LCDIF_SetRSHigh_normal(MMD_U16 value);
void LCDIF_SetRSLow_normal(MMD_U16 value);
void LCDIF_InitLcdIFParm(PTLcdifParm parm);	
void LCDIF_SetLCDIFParm(MMD_U32 bPanel);

#ifdef __cplusplus                              
}                                               
#endif   

#endif /*_MMD_DRIVERIC_H_*/


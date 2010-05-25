/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2005                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : VIM_LCDIF_Driver.h
 * [Description]          : 5X 's LCDIF moudle sub driver:578b 
 * [Author]                 : WendyGuo	
 * [Date Of Creation]  : 2007-03-06 
 * [Platform]              : ARM7
 * [Note]                    : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        		Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2005-11-10  	Liuhd			The first version
 * 2006-06-09  	Angela			Update for 528
 * 2006-03-06  	WendyGuo          	Created 578b
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

#ifndef _VIM_5XRDK_LCDIF_H_
#define _VIM_5XRDK_LCDIF_H_


#ifdef __cplusplus
extern "C" {
#endif


#define  DELAY_USE_INFO		0



//lcdif head config
typedef enum _VIM_LCDIF_SEL_CS
{
	VIM_LCDIF_CS1=0,
	VIM_LCDIF_CS2,
	VIM_LCDIF_SEL_CS_ERROR=0x7fffffff
}VIM_LCDIF_SEL_CS;

void VIM_LCDIF_InitLcdIFParm(PTLcdifParm parm);
void VIM_LCDIF_ResetPanel(void);
void VIM_LCDIF_RsLExW(UINT16 uVal);
void VIM_LCDIF_RsHExW(UINT16 uVal);
UINT32 VIM_LCDIF_RsLExR(void);
UINT32 VIM_LCDIF_RsHExR(void);
void VIM_LCDIF_SetBiasSpt(TPoint pt);


#ifdef __cplusplus
}
#endif


#endif

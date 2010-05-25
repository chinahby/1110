/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_COMMON.h
 * [Description]        : 公共文件，所有的RDK都包含这一个文件
 *                       	   
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-10 16:01
 * [Platform]               : 平台无关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-10  		angela      		 Created
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

#ifndef _VIM_5XRDK_COMMON_H_
#define _VIM_5XRDK_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "compile_macro.h"

#include "mmd_data_type.h"
#include "VIM_Drivers.h" //vc820 driver
#include "VIM_Config.h" //vc820 driver

#include"VIM_COMMON_Typedef.h"
#include"VIM_COMMON_Debug.h"
#include"VIM_COMMON_Regdef.h"

#include"VIM_DRIVERIC_Info.h"
//#include "VIM_SENSOR_Info.h"
#include "SENSOR_Info.h"
#include "VIM_SDRAM_Info.h"

//---------------------------------------------------------------
// Define Typed
#define VIM_MULTI8 0
#define VIM_MULTI16 1
#define VIM_LOCOSTO 2
#define VIM_HIFAMR9 3
#define VIM_VC0820_MULTI32 	10

#define VIM_LITTLEENDIAN	0
#define VIM_BIGENDIAN	1


typedef enum _VIM_BYPASSTYPE
{
	VIM_BYPASS_USE_GPIO = 0x0,
	VIM_BYPASS_USE_REG = 0x1
}VIM_BYPASSTYPE;

typedef enum _VIM_BYPASS_SUBPANELCTL
{
	VIM_BYPASS_USEREGISTER_SUBPANEL=BIT1,
	VIM_BYPASS_USECS2_SUBPANEL=0,
	VIM_BYPASS_USEGPIO_SUBPANEL=(BIT6)

}VIM_BYPASS_SUBPANELCTL;
//---------------------------------------------------------------

#include"VIM_USER_Info.h"
#include "VIM_HIF_Driver.h"
#include "VIM_SIF_Driver.h"
#include "VIM_SYS.h"

#include "VIM_DISP_Driver.h"
#include "VIM_LCDIF_Driver.h"
#include "VIM_IPP_Driver.h"
#include "VIM_JPEG_Driver.h"
#include "VIM_MARB_Driver.h"
#include "VIM_TVDC_Driver.h"
#include "VIM_MPEG4_Driver.h"
#include "VIM_SDRAM_Driver.h"
#include "VIM_HIGH_API.h"
#include "VIM_MID_API.h"
#include "mmd_sys_driver.h"
#include "..\drivers_ext\vc0848\VC08XXhrdk\Drivers\audio\VIM_Audio_Driver.h"

#ifdef __cplusplus
}
#endif

#endif 

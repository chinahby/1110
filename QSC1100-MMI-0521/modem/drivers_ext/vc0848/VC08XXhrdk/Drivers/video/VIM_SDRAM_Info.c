/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_SDRAM_Info.c
 * [Description]        :   ”√”⁄578b.
 *				    LCD panel parameter setting structure.
 *				    				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-06-06 16:01
 * [Platform]               : not be limited by platform
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-06-06  		angela     		 Created
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

#include "VIM_COMMON.h"

#if HYB39S128160CT_7
const TSdraminfo g_SdramInfo={
	
	2,    // SdramSize;//mbyte

	VM_SDRC_BANKADDR_24_23,//BankAddress;
	VM_SDRC_ROWADDR_23_11,// RowAddress; 
	VM_SDRC_PAGESIZE_512,/// PageSize; 
	VM_SDRC_BANKNUMBER_4,//BankNum; 
	VM_SDRC_DATAWIDTH_32BIT,// DataWidth; 
	VM_SDRC_SDRAMDENSITY_1M_16,//UINT32 Density; 
	VM_SDRC_CASLATENCY_3,//UINT32 Cas;
	0x2e,
	{
		0,/*!< auto self refresh mode entry time : (1~16)*16 cycles */
		2,/*!< load mode register command to active or refresh command : (1~4)cycles */
		1,/*!< data-in to precharge command, in fact max of{(tras-trcd},(trc-trcd-trp)} : (1~16)cycles */
		4,/*!< auto refresh period : (1~16)cycles */
		1,/*!< row pre-charge time(precharge command period) : (1~4)cycles */
		1,/*!< ras to cas delay(active to read/write delay) : (1~4)cycles */
		4,/*!< exit self refresh tp active command : (1~16)cycles */
		15,/*!< row cycle time(active to active command) : (1~16)cycles */
		15,/*!< row active time(active to precharge command) : (1~16)cycles */
	}


};

#endif

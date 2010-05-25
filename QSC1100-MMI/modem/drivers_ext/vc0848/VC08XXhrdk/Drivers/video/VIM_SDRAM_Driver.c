
/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_SDRAM_Driver.c
 * [Description]        : 适用于578b.定义了所有跟sdram 有关的
 *				     函数。 
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-05-20 16:01
 * [Platform]               : 平台无关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-06-6  		angela      		 Created
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

#if VIM_USER_SUPPORT_SDRAM

/***********************************************************************************
Description:
		set sdrc commands.
		
Parameters:
		command.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
VIM_RESULT VIM_SDRAM_SetCommand(VIM_SDRC_COMMAND command)
{
	if(command > VIM_SDRC_COMMAND_PWDNX){
		return VIM_ERR_SDRC_COMMAND_UNKNOWN;
	}
	
	VIM_HIF_SetReg32(V5_REG_SDRC_COMMAND,	(UINT32)command);
	
	return VIM_SUCCEED;
}


/***********************************************************************************
Description:
		initialize sdram parameters.
		
Parameters:
		void.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.
		
Remarks:
		if there have sdram in vc0578b, should call this function to intialize sdram.
		
************************************************************************************/
VIM_RESULT VIM_SDRAM_Initialize(PTSdraminfo Info)
{
	UINT32	regconfig=Info->SdramSize, regmode=0, regtiming=0, regrefresh=0;
	UINT32 	regstatus = 0;
	SINT32	i = VIM_SDRAM_STATUSDETECT_MAXTIME;
	VIM_RESULT	result ;

	while(regconfig>1)
	{
		regconfig/=2;
		regmode++;
	}
		
	VIM_HIF_SetReg32(V5_REG_PAD_MEM_PC, 	0x00000000);		/*!all sdame pin work*/
	VIM_HIF_SetReg16(V5_REG_MARB_MEMORY_ADDR+2,0X8|regmode);		/*!<  */
	
	regconfig	= Info->BankAddress| Info->RowAddress| VM_SDRC_SAFEROWMODE |VM_SDRC_CACHEMODE | 
				VM_SDRC_APDE | VM_SDRC_SDRAMCLOCK | VM_SDRC_ASRE | VM_SDRC_EMR | Info->PageSize| 
				Info->BankNum| Info->DataWidth| Info->Density;
	regmode		=  VM_SDRC_DRIVERSTRENGTH | VM_SDRC_TCSR | VM_SDRC_PASR | VM_SDRC_WRITEBURSTMODE | 
				VM_SDRC_OPMODE | Info->Cas| VM_SDRC_BURSTTYPE | VM_SDRC_BURSTLENGTH;
	regtiming	= (Info->Timing.Tase&0xf)<<28| (Info->Timing.Tmrd&3)<<24| (Info->Timing.Tdpl&0xf)<<20 |( Info->Timing.Trfc&0xf)<<16| ( Info->Timing.Trp&0x3)<<14 |
				(Info->Timing.Trcd&0x3)<<12 | (Info->Timing.Txsr&0xf)<<8 | (Info->Timing.Trc&0xf)<<4 | (Info->Timing.Tras&0xf);
	regrefresh	= Info->RefreshInt;
	
	VIM_HIF_SetReg32(V5_REG_SDRC_CFG,			regconfig);
	VIM_HIF_SetReg32(V5_REG_SDRC_MODE,			regmode);
	VIM_HIF_SetReg32(V5_REG_SDRC_TIMING,		regtiming);
	VIM_HIF_SetReg32(V5_REG_SDRC_REFRESH,		regrefresh);
	result = VIM_SDRAM_SetCommand(VIM_SDRC_COMMAND_INIT);
	if(result > 0){
		return result;
	}	
	
	do{
		regstatus = VIM_HIF_GetReg32(V5_REG_SDRC_STATUS)&0xfff0;
		i--;
	}while((regstatus!=0) && (i!=0));
	
	if((0==i) && (regstatus!=0)){
		return VIM_ERR_SDRC_INIT_OUTOFTIME;
	}else{
		return VIM_SUCCEED;
	}
}

/***********************************************************************************
Description:
		get sdram size
		
Parameters:
		command.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
UINT32 VIM_SDRAM_GetSize(void)
{
	UINT32 Size=1,i=0;
	i=VIM_HIF_GetReg16(V5_REG_MARB_MEMORY_ADDR+2)&0x7;
	while(i--)
	{
		Size=Size*2;
	}
	Size=Size*1024*1024;
	return Size;
}
#endif



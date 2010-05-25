/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_driveric_driver.c
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
 
/*
 * INCLUDE FILE                                              
 */

#include "mmp_rdk.h"

#define MMD_DRIVERIC_DEBUG_INFO
#define MMD_DRIVERIC_DEBUG_ERR

#ifdef MMD_DRIVERIC_DEBUG_INFO
    #define PRINTF(WriteVal)        MMD_PRINTF(WriteVal) //general print out
#else
    #define PRINTF(WriteVal)
#endif

#ifdef MMD_DRIVERIC_DEBUG_ERR
    #define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal) //note
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
    #define PRINTFS(WriteVal)   MMD_PRINTFS(WriteVal) //success
    #define PRINTFW(WriteVal)   MMD_PRINTFW(WriteVal) //warning
#else
    #define PRINTF(WriteVal)
    #define PRINTFN(WriteVal) 
    #define PRINTFE(WriteVal)
    #define PRINTFS(WriteVal)
#endif


volatile MMD_U32 g_BIU_Count;


#ifndef ENABLE
#define ENABLE	1
#endif

#ifndef DISABLE
#define  DISABLE	0
#endif

void LCDIF_SetRSHigh_normal(MMD_U16 value)
{ 
	Vm_SetReg(MMD_REG_LCDIF_EXRW_COMM_DATA, (MMD_U32)(value<<16));
	Vm_SetReg(MMD_REG_LCDIF_EXRW_EN, 0x00000002);	//EXW_COM_EN
}
void LCDIF_SetRSLow_normal(MMD_U16 value)
{
	Vm_SetReg(MMD_REG_LCDIF_EXRW_COMM_DATA, (MMD_U32)value);
	Vm_SetReg(MMD_REG_LCDIF_EXRW_EN, 0x00000001);	//EXW_DAT_EN
}

/***************************************************************
Description:
		switch panel cs
		
Parameters:
		Panel: select CS1 or CS2
Returns:
		void
****************************************************************/
static void LCDIF_SwitchPanelCs(MMD_U08 Panel)
{
#if 0
	MMD_U32 bValue;
	bValue = Vm_GetReg(V5_REG_LCDIF_CFG);
		if(Panel==VIM_LCDIF_CS1)
			{
				bValue &= ~(BIT0);
			}
		else
			{
				bValue |= (BIT0);
			}
		Vm_SetReg(V5_REG_LCDIF_CFG, bValue);
#endif
}

#if MMD_DELAY_USE_INFO==0
/********************************************************************************
  Description:
	  tools for caculate the delay inter value
  Parameters:
	MMD_U16 wcy
  Returns:
  	MMD_U08 len:	The value of delay 
  Remarks:
*********************************************************************************/

static MMD_U08 LCDIF_GetDeInterVal(
	MMD_U16 wcy				/*(IN)	write pulse width	*/
)
{
	MMD_U08 len;

	len = (MMD_U08)(wcy & 0x1f);
	wcy >>= 5;
	len = (MMD_U08)(len +(wcy & 0x1f) + 1);
	wcy >>= 5;
	len = (MMD_U08)(len + (wcy & 0x1f));
	return len;
}


/********************************************************************************
  Description:
	  set write cycle and 3 delay value
  Parameters:
	MMD_U16 x,MMD_U08 HeadNum
  Returns:
  	void
  Remarks:
*********************************************************************************/
static const MMD_U08 gLcdcDelay[] = {	0x0e, 0xff,			//DEDLY  value   : 15 ~ 256
							0x01, 0x3e,			//DEINTV value   :  2 ~ 63
							0x0e, 0xff };			//LINEINTV value : 15 ~ 256

static void LCDIF_DispSetWrTiming(
 	MMD_U16 x,				/*(IN)	The write/read access timing parameter for DBI mode LCD panel	*/
 	MMD_U08 HeadNum			/*(IN)	The head data number in the config structure	*/
 )
{
	MMD_U08 i, j, delay[3];
	MMD_U32 temp;

	
	Vm_SetReg(MMD_REG_LCDIF_DBI_TIM, x);
	delay[1] = (MMD_U08)(LCDIF_GetDeInterVal(x) + 1);
	delay[0] = (MMD_U08)(delay[1] * HeadNum);
	delay[2] = delay[1];

#ifdef MMD_BIU_DATABUS16BIT	
#else
		delay[1] <<= 1;
		delay[2] <<= 1;
#endif

	for(i = 0; i < 3; i++)
	{
		j = i << 1;
		delay[i] -= 1;
		delay[i] = delay[i] < gLcdcDelay[j] ? gLcdcDelay[j] : delay[i];
		delay[i] = delay[i] > gLcdcDelay[j+1] ? gLcdcDelay[j+1] : delay[i];
	}

	temp=delay[2]<<	16|delay[1]<<8|delay[0];
	Vm_SetReg(MMD_REG_LCDC_DEDLY, temp);
	
	
}

#endif
/*-----------------------------------------------------------------------------
Description:
		 SET dbi dpi mode
		
Parameters:
		VIM_LCDIF_DBI_B=BIT1,		
		VIM_LCDIF_DBI_A=BIT2,
		VIM_LCDIF_DPI=BIT3
Returns:

Remarks:
--------------------------------------------------------------------------------*/
static void LCDIF_SetDbiDpi(VIM_LCDIF_TYPE Mode)
{
	MMD_U32 value32 = 0;
	value32 = Vm_GetReg(MMD_REG_LCDC_CTRL);
	value32 &= ~(BIT20|BIT21);			//regval &= (~((MMD_U32)3<<2));

	if(Mode==VIM_LCDIF_DPI)
		value32|=BIT21;
	else
		value32|=BIT20;
		
	Vm_SetReg(MMD_REG_LCDC_CTRL,value32);
	Vm_SetReg(MMD_REG_LCDIF_CFG, Mode);
}

 /********************************************************************************
Description:
	init lcdif parm 
Parameters:
	 PTLcdifParm  parm
	 parm->rsflag:	when send head data,set RS signal value
	 parm->headnum:	when send head data,set head number
	 parm->stxp:		set position of the 1th start x address value in the HEAD_DATA register group. 		
	 parm->styp:		set position of the 1th start y address value in the HEAD_DATA register group. 
	 parm->endxp:	set position of the 1th end x address value in the HEAD_DATA register group. 
	 parm->endyp:	set position of the 1th end y address value in the HEAD_DATA register group. 

	 //if the start x, end x, start y, end y address value only use 1 register to store, then set Repeat_stxp, _styp, _endxp, _endyp=0.
	 parm->Repeat_stxp:	set position of the 2th start x address value in the HEAD_DATA register group.	//new parameter
	 parm->Repeat_styp:	set position of the 2th start y address value in the HEAD_DATA register group.
 	 parm->Repeat_endxp:	set position of the 2th end x address value in the HEAD_DATA register group.
	 parm->Repeat_endyp:	set position of the 2th end y address value in the HEAD_DATA register group.

	 parm->Head_Process_flag: if start x, end x, start y, end y address value need 2 registers to store then set this flag=0x03.
	 parm->Start_x_comm_code: set the address of register which stroes start x value.
	 parm->End_x_comm_code: set the address of register which stroes end x value.
	 parm->Start_y_comm_code: set the address of register which stroes start y value.
	 parm->End_y_comm_code: set the address of register which stroes end y value.
	  
	 parm->fmt:         Set display format and mapping of the image data
	 parm->wcy:		Set the write access parameter for built-on SRAM type LCD access
	 parm->dedelay:	The number of clock cycles from address enable assertion to data enable assertion. The min value is 15 and the max value is 255
	 parm->deinteval:	The minimum number of clock cycles between the two data enable signals. The min value is 2 and the max value is 63. 
	 parm->lineinteval:  he number of clock cycles from the last data enable assertion to the address enable assertion in the next line. The min value is 15 and the max value is 255.
	 parm->rcy: 	 	Set the read access parameter for built-on SRAM type LCD access
  Returns:
  	void
  Remarks:
  *********************************************************************************/

void LCDIF_InitLcdIFParm(PTLcdifParm parm)			/*(IN)	The structure information*/
{
	MMD_U32 temp;
	MMD_U08 i;

	if(parm->DBI_Parm==NULL)					//DPI mode
	{
		temp=parm->DPI_Parm->SerialCOMMD_BIT_NUM|((parm->DPI_Parm->SerialDATA_BIT_NUM)<<8)|((parm->DPI_Parm->SCL_WIDTH)<<16);
		Vm_SetReg(MMD_REG_LCDIF_SERIAL_SEND, temp);

		temp=parm->DPI_Parm->PixClock_Polarization|parm->DPI_Parm->DE_Polarization|parm->DPI_Parm->HS_Polarization|parm->DPI_Parm->VS_Polarization;
		Vm_SetReg(MMD_REG_LCDIF_SERIAL_SEND, temp|parm->LCDIF_TYPE);

		temp=parm->DPI_Parm->HS_END_X|(parm->DPI_Parm->HBP_END_X)<<16;
		Vm_SetReg(MMD_REG_LCDIF_DPI_HTIM0, temp);

		temp=parm->DPI_Parm->AR_END_X|(parm->DPI_Parm->HFP_END_X)<<16;
		Vm_SetReg(MMD_REG_LCDIF_DPI_HTIM1, temp);

		temp=parm->DPI_Parm->VS_END_Y|(parm->DPI_Parm->VBP_END_Y)<<16;
		Vm_SetReg(MMD_REG_LCDIF_DPI_VTIM0, temp);

		temp=parm->DPI_Parm->AR_END_Y|(parm->DPI_Parm->VFP_END_Y)<<16;
		Vm_SetReg(MMD_REG_LCDIF_DPI_VTIM1, temp);
	}
	else										//DBI mode, same as 528 LCDIF 
	{
		Vm_SetReg(MMD_REG_LCDIF_HEAD_RS_FLAG, parm->DBI_Parm->rsflag);
		temp=Vm_GetReg(MMD_REG_LCDIF_HEAD_CONFIG);
		Vm_SetReg(MMD_REG_LCDIF_HEAD_CONFIG,  temp|((parm->DBI_Parm->headnum)<<8) );	//must OR other bit!!!!!
		Vm_SetReg(MMD_REG_LCDIF_START_POS, ((parm->DBI_Parm->stxp)<<16)|(parm->DBI_Parm->styp));
		Vm_SetReg(MMD_REG_LCDIF_END_POS, ((parm->DBI_Parm->endxp)<<16)|(parm->DBI_Parm->endyp));
		
		if((parm->DBI_Parm->SpecialLCD_flag)==1)											//wendy guo 1/24/2007
		{
			Vm_SetReg(MMD_REG_LCDIF_REPEAT_START_POS, ((parm->DBI_Parm->Repeat_stxp)<<16)|(parm->DBI_Parm->Repeat_styp));
			Vm_SetReg(MMD_REG_LCDIF_REPEAT_END_POS, ((parm->DBI_Parm->Repeat_endxp)<<16)|(parm->DBI_Parm->Repeat_endyp));
		}
		if( parm->DBI_Parm->config) //& V5B_LCDIF_CS1_SEL)  //angela changed it 
		{
			LCDIF_SwitchPanelCs(MMD_LCDIF_CS2);	//cs2 enable	Sub panel
		}
		else
		{
			LCDIF_SwitchPanelCs(MMD_LCDIF_CS1);	//cs1 enable	Main panel
		}
		Vm_SetReg(MMD_REG_LCDIF_DAT_FMT, (parm->DBI_Parm->fmt));
		#if MMD_DELAY_USE_INFO==0  // not use the lcdifo value
			LCDIF_DispSetWrTiming(parm->DBI_Parm->wcy,parm->DBI_Parm->headnum);
		#else
			Vm_SetReg(MMD_REG_LCDIF_DBI_TIM, parm->DBI_Parm->wcy);
			temp=(parm->DBI_Parm->lineinteval)<<	16|(parm->DBI_Parm->deinteval)<<8|parm->DBI_Parm->dedelay;
			Vm_SetReg(MMD_REG_LCDC_DEDLY, temp);				
		#endif

		temp=Vm_GetReg(MMD_REG_LCDIF_DBI_TIM);
		Vm_SetReg(MMD_REG_LCDIF_DBI_TIM, temp|((parm->DBI_Parm->rcy)<<16));
  
		//set the head value according to the head request.
		for(i=0;i<parm->DBI_Parm->headnum;i++)
		{
			Host_HIF_SetReg16(MMD_REG_LCDIF_HEAD_DATA01 + (i<<1) , (parm->DBI_Parm->head[i]) );
		}
		
		if((parm->DBI_Parm->SpecialLCD_flag)==1)																//wendy guo 1/24/2007
		{
			Vm_SetReg(MMD_REG_LCDIF_HEAD_PROCESS_FLAG, parm->DBI_Parm->Head_Process_flag);				//0x03
			Vm_SetReg(MMD_REG_LCDIF_START_COMM_CODE, ((parm->DBI_Parm->Start_x_comm_code)<<16)|(parm->DBI_Parm->Start_y_comm_code));//parm->DBI_Parm->Start_x_comm_code);			//00000000,0001,0000
			Vm_SetReg(MMD_REG_LCDIF_END_COMM_CODE, ((parm->DBI_Parm->End_x_comm_code)<<16)|(parm->DBI_Parm->End_y_comm_code));
		}

		// head enable when set 1
		temp=Vm_GetReg(MMD_REG_LCDIF_HEAD_CONFIG);
		Host_HIF_SetReg8(MMD_REG_LCDIF_HEAD_CONFIG, temp|ENABLE);

		//update the all position of x and y to the register. It will set 1 after setting all other register
		temp=Vm_GetReg(MMD_REG_LCDIF_HEAD_CONFIG);
		Host_HIF_SetReg8(MMD_REG_LCDIF_HEAD_CONFIG, temp|0x9);

		//V5B_LcdifSetBiasSpt(parm->biaspt);	//angela changed it
	
	}
	LCDIF_SetDbiDpi(parm->LCDIF_TYPE);

}

void LCDIF_SetLCDIFParm(MMD_U32 bPanel)
{
	if(bPanel==MMD_HAPI_LCDPANEL_SUB)	//sub
	{
		LCDIF_InitLcdIFParm(gDriverIcInfo.slave);
	}
	else
	{
		LCDIF_InitLcdIFParm(gDriverIcInfo.master);
	}
}



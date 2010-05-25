/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2005                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : VIM_LCDIF_Driver.c
 * [Description]          : 5X 's LCDIF moudle sub driver:578b 
 * [Author]                 : WendyGuo	
 * [Date Of Creation]  : 2007-03-06 
 * [Platform]              : not limited by platform
 * [Note]                    : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        		Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2005-11-16  	Liuhd			The first version
 * 2006-06-09  	Angela			Update for 528
 * 2006-03-06  	WendyGuo         	Created 578b
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

#include"VIM_COMMON.h"
const UINT8 gLcdcDelay[] = {	0x0e, 0xff,			//DEDLY  value   : 15 ~ 256
							0x01, 0x3e,			//DEINTV value   :  2 ~ 63
							0x0e, 0xff };			//LINEINTV value : 15 ~ 256

/********************************************************************************
  Description:
	  tools for caculate the delay inter value
  Parameters:
	UINT16 wcy
  Returns:
  	UINT8 len:	The value of delay 
  Remarks:
*********************************************************************************/

static UINT8 VIM_LCDIF_GetDeInterVal(
	UINT16 wcy				/*(IN)	write pulse width	*/
)
{
	UINT8 len;

	len = (UINT8)(wcy & 0x1f);
	wcy >>= 5;
	len = (UINT8)(len +(wcy & 0x1f) + 1);
	wcy >>= 5;
	len = (UINT8)(len + (wcy & 0x1f));
	return len;
}
/********************************************************************************
  Description:
	  set write cycle and 3 delay value
  Parameters:
	UINT16 x,UINT8 HeadNum
  Returns:
  	void
  Remarks:
*********************************************************************************/

 void VIM_LCDIF_DispSetWrTiming(
 	UINT16 x,				/*(IN)	The write/read access timing parameter for DBI mode LCD panel	*/
 	UINT8 HeadNum			/*(IN)	The head data number in the config structure	*/
 )
{
	UINT8 i, j, delay[3];
	UINT32 temp;

	
	VIM_HIF_SetReg32(V5_REG_LCDIF_DBI_TIM, x);
	delay[1] = (UINT8)(VIM_LCDIF_GetDeInterVal(x) + 1);
	delay[0] = (UINT8)(delay[1] * HeadNum);
	delay[2] = delay[1];
	if(VIM_BUSTYPE!=VIM_MULTI16)	//for 8-bit bus panel
	{
		delay[1] <<= 1;
		delay[2] <<= 1;
	}

	for(i = 0; i < 3; i++)
	{
		j = i << 1;
		delay[i] -= 1;
		delay[i] = delay[i] < gLcdcDelay[j] ? gLcdcDelay[j] : delay[i];
		delay[i] = delay[i] > gLcdcDelay[j+1] ? gLcdcDelay[j+1] : delay[i];
	}

	temp=delay[2]<<	16|delay[1]<<8|delay[0];
	VIM_HIF_SetReg32(V5_REG_LCDC_DEDLY, temp);
	
	
}
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
void VIM_LCDIF_SetDbiDpi(VIM_LCDIF_TYPE Mode)
{
	UINT32 value32 = 0;
	value32 = VIM_HIF_GetReg32(V5_REG_LCDC_CTRL);
	value32 &= ~(BIT20|BIT21);			//regval &= (~((UINT32)3<<2));

	if(Mode==VIM_LCDIF_DPI)
		value32|=BIT21;
	else
		value32|=BIT20;
		
	VIM_HIF_SetReg32(V5_REG_LCDC_CTRL,value32);
    value32 = VIM_HIF_GetReg32(V5_REG_LCDIF_CFG);		
    value32 &= ~(VIM_LCDIF_DPI|VIM_LCDIF_DBI_A|VIM_LCDIF_DBI_B);
	value32 |=Mode;
	VIM_HIF_SetReg32(V5_REG_LCDIF_CFG, value32);


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
void VIM_LCDIF_InitLcdIFParm(
	PTLcdifParm parm			/*(IN)	The structure information*/
)
{
	UINT32 temp;
	UINT8 i;

	if(parm->DBI_Parm==NULL)					//DPI mode
		{
		
		    VIM_HIF_SetLcdPclock(parm->DPI_Parm->PixClock);
			temp=parm->DPI_Parm->SerialCOMMD_BIT_NUM|((parm->DPI_Parm->SerialDATA_BIT_NUM)<<8)|((parm->DPI_Parm->SCL_WIDTH)<<16);
			VIM_HIF_SetReg32(V5_REG_LCDIF_SERIAL_SEND, temp);

			temp=parm->DPI_Parm->PixClock_Polarization|parm->DPI_Parm->DE_Polarization|parm->DPI_Parm->HS_Polarization|parm->DPI_Parm->VS_Polarization;
			VIM_HIF_SetReg32(V5_REG_LCDIF_CFG, temp|parm->LCDIF_TYPE);

			temp=parm->DPI_Parm->HS_END_X|(parm->DPI_Parm->HBP_END_X)<<16;
			VIM_HIF_SetReg32(V5_REG_LCDIF_DPI_HTIM0, temp);

			temp=parm->DPI_Parm->AR_END_X|(parm->DPI_Parm->HFP_END_X)<<16;
			VIM_HIF_SetReg32(V5_REG_LCDIF_DPI_HTIM1, temp);

			temp=parm->DPI_Parm->VS_END_Y|(parm->DPI_Parm->VBP_END_Y)<<16;
			VIM_HIF_SetReg32(V5_REG_LCDIF_DPI_VTIM0, temp);

			temp=parm->DPI_Parm->AR_END_Y|(parm->DPI_Parm->VFP_END_Y)<<16;
			VIM_HIF_SetReg32(V5_REG_LCDIF_DPI_VTIM1, temp);


			
		}
	else										//DBI mode, same as 528 LCDIF 
		{

			VIM_HIF_SetReg32(V5_REG_LCDIF_HEAD_RS_FLAG, parm->DBI_Parm->rsflag);
			temp=VIM_HIF_GetReg32(V5_REG_LCDIF_HEAD_CONFIG);
			VIM_HIF_SetReg32(V5_REG_LCDIF_HEAD_CONFIG,  temp|((parm->DBI_Parm->headnum)<<8) );	//must OR other bit!!!!!
			VIM_HIF_SetReg32(V5_REG_LCDIF_START_POS, ((parm->DBI_Parm->stxp)<<16)|(parm->DBI_Parm->styp));
			VIM_HIF_SetReg32(V5_REG_LCDIF_END_POS, ((parm->DBI_Parm->endxp)<<16)|(parm->DBI_Parm->endyp));
			
			if((parm->DBI_Parm->SpecialLCD_flag)==1)											//wendy guo 1/24/2007
			{
				VIM_HIF_SetReg32(V5_REG_LCDIF_REPEAT_START_POS, ((parm->DBI_Parm->Repeat_stxp)<<16)|(parm->DBI_Parm->Repeat_styp));
				VIM_HIF_SetReg32(V5_REG_LCDIF_REPEAT_END_POS, ((parm->DBI_Parm->Repeat_endxp)<<16)|(parm->DBI_Parm->Repeat_endyp));

			}


			if( parm->DBI_Parm->config) //& V5B_LCDIF_CS1_SEL)  //angela changed it 
				VIM_HIF_SwitchPanelCs(VIM_LCDIF_CS2);	//cs2 enable	Sub panel
			else
				VIM_HIF_SwitchPanelCs(VIM_LCDIF_CS1);	//cs1 enable	Main panel

			VIM_HIF_SetReg32(V5_REG_LCDIF_DAT_FMT, (parm->DBI_Parm->fmt));
			#if DELAY_USE_INFO==0  // not use the lcdifo value
				VIM_LCDIF_DispSetWrTiming(parm->DBI_Parm->wcy,parm->DBI_Parm->headnum);
			#else
				VIM_HIF_SetReg32(V5_REG_LCDIF_DBI_TIM, parm->DBI_Parm->wcy);

	
				temp=(parm->DBI_Parm->lineinteval)<<	16|(parm->DBI_Parm->deinteval)<<8|parm->DBI_Parm->dedelay;
				VIM_HIF_SetReg32(V5_REG_LCDC_DEDLY, temp);				
				
			#endif

			temp=VIM_HIF_GetReg32(V5_REG_LCDIF_DBI_TIM);
			VIM_HIF_SetReg32(V5_REG_LCDIF_DBI_TIM, temp|((parm->DBI_Parm->rcy)<<16));
	  
			//set the head value according to the head request.
			for(i=0;i<parm->DBI_Parm->headnum;i++)
			{
		
					VIM_HIF_SetReg16(V5_REG_LCDIF_HEAD_DATA01 + (i<<1) , (parm->DBI_Parm->head[i]) );
			}
			
				

			if((parm->DBI_Parm->SpecialLCD_flag)==1)																//wendy guo 1/24/2007
			{

				VIM_HIF_SetReg32(V5_REG_LCDIF_HEAD_PROCESS_FLAG, parm->DBI_Parm->Head_Process_flag);				//0x03
				
				VIM_HIF_SetReg32(V5_REG_LCDIF_START_COMM_CODE, ((parm->DBI_Parm->Start_x_comm_code)<<16)|(parm->DBI_Parm->Start_y_comm_code));//parm->DBI_Parm->Start_x_comm_code);			//00000000,0001,0000
				VIM_HIF_SetReg32(V5_REG_LCDIF_END_COMM_CODE, ((parm->DBI_Parm->End_x_comm_code)<<16)|(parm->DBI_Parm->End_y_comm_code));
				
			}
	
			// head enable when set 1
			temp=VIM_HIF_GetReg32(V5_REG_LCDIF_HEAD_CONFIG);
			VIM_HIF_SetReg8(V5_REG_LCDIF_HEAD_CONFIG, temp|ENABLE);
	
			//update the all position of x and y to the register. It will set 1 after setting all other register
			temp=VIM_HIF_GetReg32(V5_REG_LCDIF_HEAD_CONFIG);
			VIM_HIF_SetReg8(V5_REG_LCDIF_HEAD_CONFIG, temp|0x9);

			//V5B_LcdifSetBiasSpt(parm->biaspt);	//angela changed it
		
		}
	VIM_LCDIF_SetDbiDpi(parm->LCDIF_TYPE);

}
/********************************************************************************
  Description:
	  lcd reset signal. Low active
  Parameters:
	void
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_LCDIF_ResetPanel(void)
{

       VIM_HIF_SetReg8(V5_REG_LCDIF_LCD_RESET, 0x1);
//#ifndef NCSC
#if (PROJECT_NU820_VERSION >= NU820_MP4)
	//delaycycle(100);
	VIM_USER_DelayMs(10);//add by guoyuankai for mp4 DBI LCD reset signal 2008,3,27
#else
	//VIM_USER_DelayMs(100);
#endif


	VIM_HIF_SetReg8(V5_REG_LCDIF_LCD_RESET, 0x0);
//#ifndef NCSC
#if (PROJECT_NU820_VERSION >= NU820_MP4)
	//delaycycle(100);
	VIM_USER_DelayMs(10);//add by guoyuankai for mp4 DBI LCD reset signal 2008,3,27
#else
	VIM_USER_DelayMs(100);
#endif

//#endif
	VIM_HIF_SetReg8(V5_REG_LCDIF_LCD_RESET, 1);
//#ifndef NCSC
#if (PROJECT_NU820_VERSION >= NU820_MP4)
	//delaycycle(20);
	VIM_USER_DelayMs(20);//add by guoyuankai for mp4 DBI LCD reset signal 2008,3,27

#else
	VIM_USER_DelayMs(20);	//wait signal stable to avoid writing reg fail.
#endif
//#endif
}


/********************************************************************************
  Description:
	 Set output data to this register.  the data put in this register is output through LCD_D[15-0].  
	 During this process LCD_RS signal is kept in Low.
  Parameters:
	uVal:	 the data put in this register
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_LCDIF_RsLExW(
	UINT16 uVal			/*(IN)	The value low 16bit is outputing command	*/				
)
{
	
	VIM_HIF_SetReg16(V5_REG_LCDIF_EXRW_COMM_DATA, uVal);			//low 16bit is Command
	VIM_HIF_SetReg32(V5_REG_LCDIF_EXRW_EN, 0x1);
}


/********************************************************************************
  Description:
	 Set output data to this register.  the data put in this register is output through LCD_D[15-0].  
	 During this process LCD_RS signal is kept in High
	 Parameters:
	uVal:	 the data put in this register
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_LCDIF_RsHExW(
	UINT16 uVal			/*(IN)	The value high 16bit  is outputing data	*/	
)
{

	VIM_HIF_SetReg16(V5_REG_LCDIF_EXRW_COMM_DATA+2, uVal);		//high 16bit is Data
	VIM_HIF_SetReg32(V5_REG_LCDIF_EXRW_EN, 0x2);
}

/********************************************************************************
  Description:
	 Read current data on DIS_D[15-0] from this register,LCD_RS signal is kept in Low
 Parameters:
	void
  Returns:
  	(UINT32)VIM_HIF_GetReg32(V5_REG_LCDIF_EXR_DATA):	The data  in this register
  	
  Remarks:
*********************************************************************************/

UINT32 VIM_LCDIF_RsLExR(void)
{
	
	UINT32 temp;
	temp=VIM_HIF_GetReg32(V5_REG_LCDIF_EXRW_EN);
	VIM_HIF_SetReg32(V5_REG_LCDIF_EXR_DATA, temp&0xfffffff7);	//bit3=0
	return (UINT32)VIM_HIF_GetReg32(V5_REG_LCDIF_EXR_DATA);
}

/********************************************************************************
  Description:
	 Read current data on DIS_D[15-0] from this register,LCD_RS signal is kept in High
 Parameters:
	void
  Returns:
  	(UINT32)VIM_HIF_GetReg32(V5_REG_LCDIF_EXR_DATA):	The data  in this register
  Remarks:
*********************************************************************************/
UINT32 VIM_LCDIF_RsHExR(void)
{
	
	UINT32 temp;
	temp=VIM_HIF_GetReg32(V5_REG_LCDIF_EXRW_EN);
	VIM_HIF_SetReg32(V5_REG_LCDIF_EXR_DATA, temp|0x00000008);	//bit3=1
	return (UINT32)VIM_HIF_GetReg32(V5_REG_LCDIF_EXR_DATA);
}



/********************************************************************************
  Description:
	set the x,y bias value
 Parameters:
	TPoint pt:  set the x/y bias value
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_LCDIF_SetBiasSpt(
	TPoint pt				/*(IN)	The information of x/y bias	*/
)
{
	
	UINT16 PtTemp;
	PtTemp=((UINT8)pt.x)|(pt.y<<8);
	VIM_HIF_SetReg16(V5_REG_LCDIF_BIAS, PtTemp);
	
}



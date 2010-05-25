/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_IPP_Driver.c
 * [Description]        : 适用于578b.和ipp相关的函数   
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-19 16:01
 * [Platform]               : 平台相关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-24 		angela      		 Created
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
///////////////////////////////tools for ipp caculate start/////////////////////////////

static UINT16	VIM_IPP_MaxDivisor(UINT16 a, UINT16 b)
{
	while(a && b)
	{
		if(a > b)
			a %= b;
		else
			b %= a;
	}
	if(a)
		return a;
	return b;
}
static void VIM_IPP_CalcSizerDownFactor(UINT16 src, UINT16 dst, PVIM_IPP_SzDnFct pfct)
{
	UINT32 tmps = 0, tmpd = 0, maxdiv = 0;

	maxdiv = VIM_IPP_MaxDivisor(src, dst);
	tmps = src / maxdiv;
	tmpd = dst / maxdiv;
	pfct->ifmaxdiv=0;
	
	if(tmps > 0x80)
	{
//#if (BOARD_TYPE==VC0820_FPGA)
//		tmpd = (UINT32)(tmpd *128 / tmps + 0.5);
//#else		
		tmpd = (tmpd << 7) / tmps ;
//#endif
		tmps = 0x80;
		pfct->ifmaxdiv=1;
	}
	pfct->maxdiv=maxdiv;
	pfct->w1 = (UINT8)(tmps & 0x7f);
	pfct->w2 = (UINT8)(tmpd & 0x7f);
	pfct->s1 = (UINT16)(tmps / tmpd);
	pfct->s2 = (UINT16)(pfct->s1 + ((tmps % tmpd) ? 1 : 0));
	pfct->s1 = (UINT16)(0x10000 / pfct->s1);
	pfct->s2 = (UINT16)(0x10000 / pfct->s2);
	pfct->c  = (UINT8)(tmpd - (tmps % tmpd));
}

static void VIM_IPP_CalcSizerUpFactor(UINT16 src, UINT16 dst, PVIM_IPP_SzUpFct pfct)
{
	UINT32 tmps = 0, tmpd = 0, maxdiv = 0;

	maxdiv = VIM_IPP_MaxDivisor(src, dst);
	tmps = src / maxdiv;
	tmpd = dst / maxdiv;
	if(tmpd > 0x100)
	{
//#if (BOARD_TYPE==VC0820_FPGA)	
//		tmps = (UINT32)(tmps*128 / tmpd+0.5);
//#else
		tmps = (tmps << 8) / tmpd;
//#endif
		tmpd = 0x100;
	}
	//if(tmps == tmpd)
		//tmps --;
	pfct->w1 = (UINT8)tmps;
	pfct->w2 = (UINT8)tmpd;
	pfct->s  = (UINT16)(0x10000 / tmpd);
}
///////////////////////////////tools for ipp caculate end/////////////////////////////

/********************************************************************************
	Description:
		IPP reset 
	Parameters:
		byEffect:	special control setting
	Note:
	Remarks:
*********************************************************************************/
void VIM_IPP_Reset(void)
{
UINT8 Temp;
	//VIM_USER_DelayMs(VIM_IPP_DOWNTOUP_DELAY);
	Temp = VIM_HIF_GetReg8(V5_REG_IPP_CTRL+3);
	// Reset all the internal registers except control registers
	VIM_HIF_SetReg8(V5_REG_IPP_CTRL+3, Temp | BIT7);
	VIM_USER_DelayMs(1);
	// Restore reset bit in V5_REG_IPP_SIZCTRL register.
	VIM_HIF_SetReg8(V5_REG_IPP_CTRL+3, Temp & (~BIT7));
}

/********************************************************************************
	Description:
		set ipp and line buf work mode
	Parameters:
		byMode:	work mode
			VIM_JPEG_MODE_PREVIEW=0,
			VIM_JPEG_MODE_CAPTURE,
			VIM_JPEG_MODE_DISPLAY,
			VIM_JPEG_MODE_DECODE,
			VIM_JPEG_MODE_ENCODE
		HaveFrame:
			VIM_IPP_HAVEFRAME = 0x1,
			VIM_IPP_HAVE_NOFRAME=0X0;
	Returns:
	Remarks:
*********************************************************************************/
void VIM_IPP_SetMode(VIM_IPP_MODE bMode,VIM_IPP_HAVEFRAM bHaveFrame)
{
	UINT8 bTempIpp,bLineMode;
	UINT16 bSizerCtrl;

	//VIM_IPP_Reset();//angela discard it for zoom out problem ,ipp reset must be used in one frame stream have passed ipp
	VIM_SIF_StartCaptureEnable(DISABLE);

	//ready set lbuf working mode, bit0-2 is working mode 
	bLineMode=0;	
	bLineMode|=(bMode);	//Set line buf mode

	//ready set ipp working mode, bit5-7 is working mode
	bTempIpp=VIM_HIF_GetReg8(V5_REG_IPP_CTRL+3);
	bTempIpp&=(~BIT4);//bit 4 display  sizer input select   clear 0 to select special effect output.
	
	//selete ipp image data from lbuf/sif/isp bit 5-6 is data select
	bSizerCtrl=VIM_HIF_GetReg16(V5_REG_IPP_CTRL);
	//bSizerCtrl&=0;	
	switch(bMode)
		{
		case VIM_IPP_MODE_CAPTURE:
		case VIM_IPP_MODE_PREVIEW:
            		bSizerCtrl&=(~(BIT12|BIT13|BIT11|BIT8));
			bSizerCtrl|=(1<<12);		//zoom input select from sif 
			if(bHaveFrame==VIM_IPP_HAVEFRAME)
			{
				bLineMode|=BIT3;	//	line buf get data from lcdc
		              bSizerCtrl&=(~BIT10);//disable path from capture sizer to linebuffer
                		bSizerCtrl&=(~BIT9);//disable path from display sizer to linebuffer
			}
			else
			{
				bLineMode&=(~BIT3);	//	
				bSizerCtrl|=BIT10;			// capture sizer to lbuf enable
				bSizerCtrl&=(~BIT9); //disable path from display to linebuffer
			}
			bSizerCtrl|=BIT8;					// display sizer to lcdc
			bSizerCtrl|=BIT0;					// display sizer enable
			bSizerCtrl|=BIT1;					// capture down sizer enable
			bSizerCtrl&=(~BIT6); //thumb nail sizer pingpong mode
			/*if((gVc0578b_Info.CaptureStatus.ThumbSize.cx*gVc0578b_Info.CaptureStatus.ThumbSize.cy)>0)
			{
				bSizerCtrl|=BIT3;			// thumb sizer enable
				bSizerCtrl&=(~BIT6); //thumb nail sizer pingpong mode
			}*/
			break;								
		case VIM_IPP_MODE_ENCODE_MP4:
#if (BOARD_TYPE==VC0820_FPGA)
            bSizerCtrl&=(~(BIT12|BIT13|BIT11|BIT8));
#endif
			bSizerCtrl|=(1<<12);		//zoom input select from sif 
			if(bHaveFrame==VIM_IPP_HAVEFRAME)
			{
				//bSizerCtrl|=BIT9;			// display sizer to line bufeer output
				bLineMode|=BIT3;	//	line buf get data from lcdc
#if (BOARD_TYPE==VC0820_FPGA)
                bSizerCtrl&=(~BIT10);//disable path from display sizer to linebuffer
                bSizerCtrl&=(~BIT9);//disable path from capture sizer to linebuffer
#else
#endif
			}
			else
			{
				bSizerCtrl|=BIT10;			// capture sizer to lbuf enable
#if (BOARD_TYPE==VC0820_FPGA)
				bLineMode&=(~BIT3);	//	
			    bSizerCtrl&=(~BIT9); //disable path from display to linebuffer
#else
#endif		
			}
			bSizerCtrl&=(~(BIT11));
			bSizerCtrl|=BIT8;					// display sizer to lcdc
			bSizerCtrl|=BIT0;			// display sizer enable
//revised by youhai_2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)			
#else
			bSizerCtrl|=BIT1;			// capture down sizer enable
#endif			
			break;
		case VIM_IPP_MODE_DISPLAY:
#if(BOARD_TYPE==VC0820_FPGA)//add by zhoujian
			bSizerCtrl&=(~(BIT12|BIT13|BIT11|BIT8));		//zoom input select from line buf
			bSizerCtrl|=BIT11;			// capture sizer to lcdc
			bSizerCtrl&=(~(BIT8)); 	
			bSizerCtrl|=BIT1;			// capture down sizer enable
			break;
#endif
		case VIM_IPP_MODE_DECODE_MP4:
#if 0//(BOARD_TYPE==VC0820_FPGA)
			bSizerCtrl|=(BIT16|BIT17);		
			bSizerCtrl|=BIT8;			// Display sizer to LCDC output enable
			bSizerCtrl|=BIT0;			// Display sizer enable
#else
			bSizerCtrl&=(~(BIT12|BIT13|BIT11|BIT8));		//zoom input select from line buf
			bSizerCtrl|=BIT11;			// capture sizer to lcdc
			bSizerCtrl&=(~(BIT8)); 		
			bSizerCtrl|=BIT1;			// capture down sizer enable
#endif			
			break;

		case VIM_IPP_MODE_VIDEO_MP4:
			bTempIpp|=BIT4;		//display sizer from line buf
			if(VIM_SIF_GetSensorType()==VIM_SIF_SENSOR_TYPE_YUV)
				bSizerCtrl|=(1<<12);		//zoom input select from sif 
			else
				bSizerCtrl|=(2<<12);		//zoom input select from isp 
			bSizerCtrl|=BIT8;			// display sizer to lcdc
			bSizerCtrl|=BIT10;	//capture sizer to line buf
			bSizerCtrl|=BIT0;			// display sizer enalbe
			bSizerCtrl|=BIT1;			// capture down sizer enable
			bSizerCtrl|=BIT3;			// thumbnail sizer enable
			break;
		case VIM_IPP_MODE_DECODE:
			bSizerCtrl&=(~(BIT12|BIT13));	//zoom input select from line buf
			bSizerCtrl|=BIT0;			// display  sizer enable
	        if (bHaveFrame)
			{
				bSizerCtrl&=(~(BIT11)); 				// display sizer to lcdc
				bSizerCtrl|=(1<<8);		
				bLineMode|=0x2<<4;	//data pass to ipp,lcdc, marb read data from line buffer 1
				bLineMode|=BIT3;	//	line buf get data from lcdc

			}
			else
			{
				bSizerCtrl&=(~(BIT11|BIT8)); 		
				bSizerCtrl|=BIT9;				//display sizer to line buf
				bLineMode|=0x1<<4;	//data pass to ipp unit,marb read data from line buffer 1
			}
				
			break;
		case VIM_IPP_MODE_CAPTURE_WITHTHUMB:
			if(VIM_SIF_GetSensorType()==VIM_SIF_SENSOR_TYPE_YUV)
				bSizerCtrl|=(1<<12);		//zoom input select from sif 
			else
				bSizerCtrl|=(2<<12);		//zoom input select from isp 
			if(bHaveFrame==VIM_IPP_HAVEFRAME)
			{
				bSizerCtrl|=BIT9;			// display sizer to line bufeer output		
			}
			else
			{
				bSizerCtrl|=BIT10;			// capture sizer to lbuf enable
			}
			bSizerCtrl|=BIT8;					// display sizer to lcdc
			//bSizerCtrl|=BIT7;			// jpeg capture enable
#if(BOARD_TYPE==VC0820_FPGA) //add by zhoujian
#else
			bSizerCtrl|=BIT6;			// thumb nail sizer fifo mode
#endif
			bSizerCtrl|=BIT0;			// display sizer enalbe
			bSizerCtrl|=BIT1;			// capture down sizer enable
			bSizerCtrl|=BIT3;			// thumb sizer enable
#if(BOARD_TYPE==VC0820_FPGA) //add by zhoujian
			bLineMode=0;
			bLineMode|=VIM_IPP_MODE_CAPTURE;	//Set line buf mode  JPEG capture mode
#endif
			break;
		default:
			break;
		}
	//set IPP working mode
	VIM_HIF_SetReg8(V5_REG_IPP_CTRL+3,bTempIpp);
	//set data from, and with frame or not
	VIM_HIF_SetReg16(V5_REG_IPP_CTRL,bSizerCtrl);
	// set line buf mode
	VIM_HIF_SetReg8(V5_REG_LBUF_MODE+1,bLineMode);
	
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */

}

/********************************************************************************
	Description:
		set special control 
	Parameters:
		Effect:	special control setting
			VIM_IPP_EFFECT_NORMAL = 0x8,
			VIM_IPP_EFFECT_SEPHIA = 0x0,
			VIM_IPP_EFFECT_SPECIALCOLOR = 0x1,
			VIM_IPP_EFFECT_NEGATIVE = 0x2,
			VIM_IPP_EFFECT_SKETCH = 0x3,
			VIM_IPP_EFFECT_MONO=0X4,
			VIM_IPP_EFFECT_FOUR_COLOR=0X5,
			VIM_IPP_EFFECT_TV = 0x6,
			VIM_IPP_EFFECT_GRID=0X7
	Note:

	Remarks:
*********************************************************************************/
void VIM_IPP_SetEffect(VIM_IPP_EFFECT bEffect)
{
UINT8 bTemp;
	bTemp=(VIM_HIF_GetReg8(V5_REG_IPP_CTRL+3)&0xf0);
	bTemp|=bEffect;
	VIM_HIF_SetReg8((V5_REG_IPP_CTRL+3),bTemp);
	
	bTemp=(VIM_HIF_GetReg8(V5_REG_IPP_CTRL));	
	if(bEffect==VIM_IPP_EFFECT_NORMAL)
		bTemp&=(~BIT5);
	else
		bTemp|=BIT5;
	VIM_HIF_SetReg8((V5_REG_IPP_CTRL),bTemp);
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */

}

/********************************************************************************
	Description:
		select caputre down size or caputre up size
	Parameters:
		bMode:		
			VIM_IPP_CAPMODE_DOWN= 0x2,
			VIM_IPP_CAPMODE_UP= 0x4
	Note:
	Remarks:
*********************************************************************************/

void VIM_IPP_CaputreSizeSelect(VIM_IPP_CAPSIZE_MODE bMode)
{
//revised by youhai_2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
UINT16 Temp;
	Temp = VIM_HIF_GetReg16(V5_REG_IPP_CTRL);
	Temp&=0xfbf9;
if(bMode==VIM_IPP_CAPMODE_DOWN||bMode==VIM_IPP_CAPMODE_UP)
{
	Temp|=(0x400|bMode);
}
else	if(bMode==VIM_IPP_DISP_CAPMODE_DOWN)
{
      Temp|=(0x200);
}
	VIM_HIF_SetReg16(V5_REG_IPP_CTRL,Temp);
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
#else
UINT8 Temp;
	Temp = VIM_HIF_GetReg8(V5_REG_IPP_CTRL);
	Temp&=0xf9;
	Temp|=bMode;
	VIM_HIF_SetReg8(V5_REG_IPP_CTRL,Temp);
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
#endif	
}
/********************************************************************************
	Description:
		select caputre down size or caputre up size
	Parameters:
		byEffect:	special control setting
	Note:
	Remarks:
*********************************************************************************/

void VIM_IPP_SetMpeg4BufMode(VIM_IPP_BUFFERMODER bMode)
{
UINT8 Temp;
	Temp = VIM_HIF_GetReg8(V5_REG_LBUF_MODE+3);
	Temp&=(~(BIT2|BIT3));
	Temp|=(bMode<<2);	// mpeg decode
	Temp|=(bMode<<3);	//mpeg encode
	VIM_HIF_SetReg8(V5_REG_LBUF_MODE+3,Temp);
}

/********************************************************************************
	Description:
		select caputre down size or caputre up size
	Parameters:
		byEffect:	special control setting
	Note:
	Remarks:
*********************************************************************************/

VIM_IPP_CAPSIZE_MODE VIM_IPP_GetCaputreSizeSelect(void)
{
UINT8 Temp;
	Temp = VIM_HIF_GetReg8(V5_REG_IPP_CTRL);
	Temp&=0x06;
	return (VIM_IPP_CAPSIZE_MODE)(Temp);
}
/***********************************************************************************
Description:
		 enable/disable sizer
		
Parameters:
		sizer	: One of the following <U>VM_IPP_BYPASSSIZER</U>
 				enumeration values that specify bypass sizer type.
		enable	: 1 for enable bypass, 0 for no disbale bypass
		
Returns:
		void.
		
Remarks:
		
************************************************************************************/
void VIM_IPP_SetSizeToLcdEnable(UINT8 enable)
{
	UINT32	regval = 0,Loop=0x3000;
	regval = VIM_HIF_GetReg32(V5_REG_IPP_CTRL);
	regval&=(~(BIT8|BIT11));
	if(enable)	{		/*!< enable */
		if(regval&BIT0)//preview
			regval |=BIT8;
		else
			regval |=BIT11;
	}
	VIM_HIF_SetReg32(V5_REG_IPP_CTRL,	regval);
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
	//if(enable==DISABLE)
	{
		while(Loop--)
		{
			if(!VIM_HIF_GetReg32(V5_REG_IPP_WINDOW_UPDATE))
				break;
		}
	}
	if(!Loop)
	{
#if(VIM_JPEG_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("VC0578B][JPEG]VIM_IPP_SetSizeToLcdEnable disable over time[ERROR]");
#endif	
	}
}
/***********************************************************************************
Description:
		 enable/disable bypass specified sizer.
		
Parameters:
		sizer	: One of the following <U>VM_IPP_BYPASSSIZER</U>
 				enumeration values that specify bypass sizer type.
		enable	: 1 for enable bypass, 0 for no disbale bypass
		
Returns:
		void.
		
Remarks:
		
************************************************************************************/
void VIM_IPP_SetSizeBypassEnable(VIM_IPP_SIZER sizer, UINT8 enable)
{
	UINT32	regval = 0;
	regval = VIM_HIF_GetReg32(V5_REG_IPP_CTRL);
	if(enable)	{		/*!< enable */
		regval |= (UINT32)sizer;
	}else{				/*!< disbale */
		regval &= (~(UINT32)sizer);
	}
	VIM_HIF_SetReg32(V5_REG_IPP_CTRL,	regval);
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
}

/********************************************************************************
	Description:
		Config input image size in IPP module
	Parameters:
		wWidth:		input image width to IPP module
		wHeight:		input image height to IPP module
	Remarks:
*********************************************************************************/
void VIM_IPP_SetImageSize(UINT16 wWidth, UINT16 wHeight)
{
	VIM_HIF_SetReg16(V5_REG_IPP_IMGSIZE, wWidth);
	VIM_HIF_SetReg16(V5_REG_IPP_IMGSIZE+2, wHeight);
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
}


void VIM_IPP_GetImageSize(UINT16 *wWidth, UINT16 *wHeight)
{
	*wWidth = VIM_HIF_GetReg16(V5_REG_IPP_IMGSIZE);
	*wHeight = VIM_HIF_GetReg16(V5_REG_IPP_IMGSIZE+2);
}

/********************************************************************************
	Description:
		Config sizer window size in IPP module
	Parameters:
		wWidth:		sizer window width to IPP module
		wHeight:	sizer window height to IPP module
	Remarks:
*********************************************************************************/
static void VIM_IPP_SetWindowSize(UINT16 wWidth, UINT16 wHeight)
{
	VIM_HIF_SetReg16(V5_REG_IPP_WINSIZE, wWidth);
	VIM_HIF_SetReg16(V5_REG_IPP_WINSIZE+2, wHeight);
}


void VIM_IPP_GetWindowSize(UINT16 *wWidth, UINT16 *wHeight)
{
	*wWidth = VIM_HIF_GetReg16(V5_REG_IPP_WINSIZE);
	*wHeight = VIM_HIF_GetReg16(V5_REG_IPP_WINSIZE+2);
}

/********************************************************************************
	Description:
		Config the X, Y coordinate of sizer's start pixel in IPP module
	Parameters:
		wStartx:	X coordinate of sizer's start pixel
		wStarty:	Y coordinate of sizer's start pixel
	Remarks:
*********************************************************************************/
static void VIM_IPP_SetWindowPos(UINT16 wStart_X, UINT16 wStart_Y)
{
	VIM_HIF_SetReg16(V5_REG_IPP_WINST_POS, wStart_X);
	VIM_HIF_SetReg16(V5_REG_IPP_WINST_POS+2, wStart_Y);
}


 void VIM_IPP_GetWindowPos(UINT16 *wStart_X, UINT16 *wStart_Y)
{
	*wStart_X = VIM_HIF_GetReg16(V5_REG_IPP_WINST_POS);
	*wStart_Y = VIM_HIF_GetReg16(V5_REG_IPP_WINST_POS+2);
}

/********************************************************************************
	Description:
		Config sizer window after display  size in IPP module
	Parameters:
		wWidth:	sizer window width 
		wHeight:	sizer window height
	Remarks:
*********************************************************************************/
static void VIM_IPP_SetDispalyWindowSize(UINT16 wWidth, UINT16 wHeight)
{
	VIM_HIF_SetReg16(V5_REG_IPP_DISPTGT_SIZE, wWidth);
	VIM_HIF_SetReg16(V5_REG_IPP_DISPTGT_SIZE+2, wHeight);
}


 void VIM_IPP_GetDispalyWindowSize(UINT16 *wWidth, UINT16 *wHeight)
{
	*wWidth = VIM_HIF_GetReg16(V5_REG_IPP_DISPTGT_SIZE);
	*wHeight = VIM_HIF_GetReg16(V5_REG_IPP_DISPTGT_SIZE+2);
}

/********************************************************************************
	Description:
		Set the down factor of down width  display sizer 
	Parameters:
		factor: the factor w1 ;w2 ;s1; s2;c 
	Remarks:
*********************************************************************************/

static void VIM_IPP_SetDisplaySizerWFactor(PVIM_IPP_SzDnFct factor)
{
	UINT32 w=0,s=0;
	w = (UINT32)(factor->w2&0x7f);
	w <<= 16;
	w |= (UINT32)(factor->w1&0x7f);
	
	s = (UINT32)(factor->s2&0xffff);
	s <<= 16;
	s |= (UINT32)(factor->s1&0xffff);
	
	VIM_HIF_SetReg32(V5_REG_IPP_DISH_W, w);
	VIM_HIF_SetReg32(V5_REG_IPP_DISH_S, s);
	VIM_HIF_SetReg32(V5_REG_IPP_DISH_C, (factor->c&0x7f));
}

/********************************************************************************
	Description:
		Set the down factor of down height display sizer 
	Parameters:
		factor: the factor w1 ;w2 ;s1; s2;c 
	Remarks:
*********************************************************************************/

static void VIM_IPP_SetDisplaySizerHFactor(PVIM_IPP_SzDnFct factor)
{
	UINT32 w=0,s=0;
	w = (UINT32)(factor->w2&0x7f);
	w <<= 16;
	w |= (UINT32)(factor->w1&0x7f);
	
	s = (UINT32)(factor->s2&0xffff);
	s <<= 16;
	s |= (UINT32)(factor->s1&0xffff);
	
	VIM_HIF_SetReg32(V5_REG_IPP_DISV_H, w);
	VIM_HIF_SetReg32(V5_REG_IPP_DISV_S, s);
	VIM_HIF_SetReg32(V5_REG_IPP_DISV_C, (factor->c&0x7f));
}


/********************************************************************************
	Description:
		Set Sourouse Window and display Window
	Parameters:
		pt: the start point of source window
		winSize: the width and height of source window
		dispSize:the width and height of display window
	Remarks:
*********************************************************************************/
VIM_RESULT VIM_IPP_SetDispalySize(TPoint pt, TSize winSize, TSize dispSize)
{
VIM_IPP_SzDnFct DownFc;
//revised by youhai_2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
UINT8 Temp;
#endif
UINT16 width,Height;
// check error
	if((dispSize.cx>VIM_IPP_DISPLAY_MAX_W)||(dispSize.cy>VIM_IPP_DISPLAY_MAX_H))
		return VIM_ERROR_DISPLAY_WINDOW;

	if((VIM_HIF_GetReg8(V5_REG_LBUF_MODE+1)&0x7)!=0x7)	//not video confrence mod
	{
		VIM_IPP_GetImageSize(&width,&Height);
		winSize.cx = (winSize.cx >> 1) << 1;
		winSize.cy = (winSize.cy >> 1) << 1;
		if((width<winSize.cx)||(Height<winSize.cy))
			return VIM_ERROR_SOURCE_WINDOW;
		//set source window start position
		VIM_IPP_SetWindowPos(pt.x,pt.y);
		//set source window size
		VIM_IPP_SetWindowSize(winSize.cx,winSize.cy);
		//get display factor and set display factor	
	}
	 if(winSize.cx>=dispSize.cx)
	{
		VIM_IPP_CalcSizerDownFactor(winSize.cx,dispSize.cx,&DownFc);
		if(DownFc.w1==DownFc.w2)
			   VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_DISP_W,ENABLE);
		else
		{
			VIM_IPP_SetDisplaySizerWFactor(&DownFc);
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_DISP_W,DISABLE);
		}
	 }
	else
		return VIM_ERROR_DISPLAY_WINDOW;


	 if(winSize.cy>=dispSize.cy)
	{
		VIM_IPP_CalcSizerDownFactor(winSize.cy,dispSize.cy,&DownFc);
#if(VIM_IPP_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578B][IPP]DownFc:-W1  ",DownFc.w1);
		VIM_USER_PrintDec("[VC0578B][IPP] DownFc:-W2  ",DownFc.w2);
		VIM_USER_PrintDec("[VC0578B][IPP] DownFc:-s1  ",DownFc.s1);
		VIM_USER_PrintDec("[VC0578B][IPP]DownFc:-s2  ",DownFc.s2);
		VIM_USER_PrintDec("[VC0578B][IPP] DownFc:-c  ",DownFc.c);
#endif	
		if(DownFc.w1==DownFc.w2)
		{
#if(VIM_IPP_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintString("[VC0578B][IPP] ***********enable bypass ipp Y**********");

#endif	
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_DISP_H,ENABLE);
		}
		else
		{
			VIM_IPP_SetDisplaySizerHFactor(&DownFc);
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_DISP_H,DISABLE);
		}
	}
	else
		return VIM_ERROR_DISPLAY_WINDOW;

	
	//set display target size
//#if(BOARD_TYPE==VC0820_FPGA)
//	VIM_IPP_SetWindowSize(winSize.cx,winSize.cy);
//#endif
//revised by youhai_2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
	Temp = VIM_HIF_GetReg8(V5_REG_IPP_CTRL);
	Temp|=1;
	VIM_HIF_SetReg8(V5_REG_IPP_CTRL,Temp);
#endif
	VIM_IPP_SetDispalyWindowSize(dispSize.cx,dispSize.cy);
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
	return VIM_SUCCEED;
}


/********************************************************************************
	Description:
		Get display size
	Parameters:
		Width: the width of thumbnail size
		height: the height of thumbnail size
		0 means 256
	Remarks:
*********************************************************************************/
void VIM_IPP_GetDispSize(PTSize Size)
{
	Size->cx=VIM_HIF_GetReg16(V5_REG_IPP_DISPTGT_SIZE);
	Size->cy=VIM_HIF_GetReg16(V5_REG_IPP_DISPTGT_SIZE+2);

}

/********************************************************************************
	Description:
		Config sizer window after capture  size in IPP module
	Parameters:
		wWidth:		sizer window width
		wHeight:	sizer window height 
	Remarks:
*********************************************************************************/
static void VIM_IPP_SetCaptureWindowSize(UINT16 wWidth, UINT16 wHeight)
{
	VIM_HIF_SetReg16(V5_REG_IPP_CAPTGT_SIZE, wWidth);
	VIM_HIF_SetReg16(V5_REG_IPP_CAPTGT_SIZE+2, wHeight);
}
void VIM_IPP_GetCaptureWindowSize(PTSize Size)
{
	Size->cx = VIM_HIF_GetReg16(V5_REG_IPP_CAPTGT_SIZE);
	Size->cy = VIM_HIF_GetReg16(V5_REG_IPP_CAPTGT_SIZE+2);
}


/********************************************************************************
	Description:
		Set the down factor of down width capture sizer 
	Parameters:
		factor: the factor w1 ;w2 ;s1; s2;c 
	Remarks:
*********************************************************************************/

static void VIM_IPP_SetCaptureSizerWFactor(PVIM_IPP_SzDnFct factor)
{
	VIM_HIF_SetReg8(V5_REG_IPP_CAPDH_W,factor->w1);
	VIM_HIF_SetReg8(V5_REG_IPP_CAPDH_W+2,factor->w2);
	VIM_HIF_SetReg16(V5_REG_IPP_CAPDH_S,factor->s1);
	VIM_HIF_SetReg16(V5_REG_IPP_CAPDH_S+2,factor->s2);
	VIM_HIF_SetReg8(V5_REG_IPP_CAPDH_C,factor->c);
}
/********************************************************************************
	Description:
		Set the down factor of down heigth  capture sizer 
	Parameters:
		factor: the factor w1 ;w2 ;s1; s2;c 
	Remarks:
*********************************************************************************/

static void VIM_IPP_SetCaptureSizerHFactor(PVIM_IPP_SzDnFct factor)
{
	VIM_HIF_SetReg8(V5_REG_IPP_CAPDV_H,factor->w1);
	VIM_HIF_SetReg8(V5_REG_IPP_CAPDV_H+2,factor->w2);
	VIM_HIF_SetReg16(V5_REG_IPP_CAPDV_S,factor->s1);
	VIM_HIF_SetReg16(V5_REG_IPP_CAPDV_S+2,factor->s2);
	VIM_HIF_SetReg8(V5_REG_IPP_CAPDV_C,factor->c);
}

/********************************************************************************
	Description:
		Set the down factor of up width capture sizer 
	Parameters:
		factor: the factor w1 ;w2 ;s1; s2;c 
	Remarks:
*********************************************************************************/

static void VIM_IPP_SetCaptureUpSizerWFactor(PVIM_IPP_SzUpFct factor)
{
	VIM_HIF_SetReg8(V5_REG_IPP_CAPUH_W,factor->w1);
	VIM_HIF_SetReg8(V5_REG_IPP_CAPUH_W+2,factor->w2);
	VIM_HIF_SetReg16(V5_REG_IPP_CAPUH_S,factor->s);
}
/********************************************************************************
	Description:
		Set the down factor of up heigth  capture sizer 
	Parameters:
		factor: the factor w1 ;w2 ;s1; s2;c 
	Remarks:
*********************************************************************************/

static void VIM_IPP_SetCaptureUpSizerHFactor(PVIM_IPP_SzUpFct factor)
{
	VIM_HIF_SetReg8(V5_REG_IPP_CAPUV_H,factor->w1);
	VIM_HIF_SetReg8(V5_REG_IPP_CAPUV_H+2,factor->w2);
	VIM_HIF_SetReg16(V5_REG_IPP_CAPUV_S,factor->s);
}

/********************************************************************************
	Description:
		Set the down factor of down width thumb sizer 
	Parameters:
		factor: the factor w1 ;w2 ;s1; s2;c 
	Remarks:
*********************************************************************************/

static void VIM_IPP_SetThumbSizerWFactor(PVIM_IPP_SzDnFct factor)
{
	VIM_HIF_SetReg8(V5_REG_IPP_THUMBH_W,factor->w1);
	VIM_HIF_SetReg8(V5_REG_IPP_THUMBH_W+2,factor->w2);
	VIM_HIF_SetReg16(V5_REG_IPP_THUMBH_S,factor->s1);
	VIM_HIF_SetReg16(V5_REG_IPP_THUMBH_S+2,factor->s2);
	VIM_HIF_SetReg8(V5_REG_IPP_THUMBH_C,factor->c);
}
/********************************************************************************
	Description:
		Set the down factor of down heigth  thumb sizer 
	Parameters:
		factor: the factor w1 ;w2 ;s1; s2;c 
	Remarks:
*********************************************************************************/

void VIM_IPP_SetThumbSizerHFactor(PVIM_IPP_SzDnFct factor)
{
	VIM_HIF_SetReg8(V5_REG_IPP_THUMBV_H,factor->w1);
	VIM_HIF_SetReg8(V5_REG_IPP_THUMBV_H+2,factor->w2);
	VIM_HIF_SetReg16(V5_REG_IPP_THUMBV_S,factor->s1);
	VIM_HIF_SetReg16(V5_REG_IPP_THUMBV_S+2,factor->s2);
	VIM_HIF_SetReg8(V5_REG_IPP_THUMBV_C,factor->c);
}
/********************************************************************************
	Description:
		Set thumbnail size
	Parameters:
		Width: the width of thumbnail size
		height: the height of thumbnail size
		0 means 256
	Remarks:
*********************************************************************************/
VIM_RESULT VIM_IPP_SetThumbSize(UINT8 bWidth, UINT8 bheight,VIM_IPP_THUMB_SELECT ThumbSelect)
{
VIM_IPP_SzDnFct DownFc;
TSize Size;
UINT16 wHeightCal;
UINT8 wTemp;
	if((!bWidth)||(!bheight))
		return VIM_SUCCEED;
	if(ThumbSelect==VIM_IPP_THUMB_FROM_CAPTURE)
		VIM_IPP_GetCaptureWindowSize(&Size);
	else if(ThumbSelect==VIM_IPP_THUMB_FROM_SPECIAL)
	{
		VIM_IPP_GetWindowSize(&Size.cx,&Size.cy);
	}
	else
		VIM_IPP_GetDispalyWindowSize(&Size.cx,&Size.cy);
	VIM_HIF_SetReg8(V5_REG_IPP_THUMBTGT_SIZE,bWidth);
	VIM_HIF_SetReg8(V5_REG_IPP_THUMBTGT_SIZE+2,bheight);
	
	wTemp=(VIM_HIF_GetReg8(V5_REG_IPP_CTRL+1)&0x3f);
	VIM_HIF_SetReg8(V5_REG_IPP_CTRL+1,(wTemp|(ThumbSelect<<6))); //thumb from 
	
	wHeightCal=(UINT16)(((Size.cy* bWidth / Size.cx) >> 1) << 1);
	if((bWidth>Size.cx)||(bheight>Size.cy))
		return VIM_ERROR_THUMB_WINDOW;
	if(bheight>wHeightCal)
		return VIM_ERROR_THUMB_HEIGHT_MORE;
	VIM_IPP_CalcSizerDownFactor(Size.cx,bWidth,&DownFc);
	VIM_IPP_SetThumbSizerWFactor(&DownFc);
	VIM_IPP_CalcSizerDownFactor(Size.cy,wHeightCal,&DownFc);
	VIM_IPP_SetThumbSizerHFactor(&DownFc);
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
	return VIM_SUCCEED;
}

/********************************************************************************
	Description:
		Get thumbnail size
	Parameters:
		Width: the width of thumbnail size
		height: the height of thumbnail size
		0 means 256
	Remarks:
*********************************************************************************/
void VIM_IPP_GetThumbSize(PTSize Size)
{
	Size->cx=VIM_HIF_GetReg8(V5_REG_IPP_THUMBTGT_SIZE);
	Size->cy=VIM_HIF_GetReg8(V5_REG_IPP_THUMBTGT_SIZE+2);

}
/********************************************************************************
  Description:
	set thumbnail buf memary 
  Parameters:
	StartAdd: the start address of sram
	Size: the size of sram
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_IPP_SetThumbbufMem(UINT32 StartAdd,UINT32 Size)
{
     //if(Size==0) Size=4;//youhai add for save use 2007.9.4
#if 0
	VIM_HIF_SetReg32(V5_REG_IPP_THUMBNAIL_START_ADDR0,	StartAdd+V5_1TSRAM_BASE);
	VIM_HIF_SetReg32(V5_REG_IPP_THUMBNAIL_END_ADDR0,	StartAdd+Size-4+V5_1TSRAM_BASE);//?why -4??
	
	VIM_HIF_SetReg32(V5_REG_IPP_THUMBNAIL_START_ADDR1,	StartAdd+V5_1TSRAM_BASE);
	VIM_HIF_SetReg32(V5_REG_IPP_THUMBNAIL_END_ADDR1,	StartAdd+Size-4+V5_1TSRAM_BASE);
#else
	VIM_HIF_SetReg32(V5_REG_IPP_THUMBNAIL_START_ADDR0,	StartAdd);
	VIM_HIF_SetReg32(V5_REG_IPP_THUMBNAIL_END_ADDR0,	StartAdd+Size);//?why -4??
	
	VIM_HIF_SetReg32(V5_REG_IPP_THUMBNAIL_START_ADDR1,	StartAdd);
	VIM_HIF_SetReg32(V5_REG_IPP_THUMBNAIL_END_ADDR1,	StartAdd+Size);
#endif


}

/********************************************************************************
	Description:
		Set the UV offset of uv sephia effect only
	Parameters:
		bUoffset: the offset of U
		bVoffset:the offset of V
	Remarks:
*********************************************************************************/

void VIM_IPP_SetEffectUVoffset(UINT8 bUoffset,UINT8 bVoffset)
{
	VIM_HIF_SetReg8(V5_REG_IPP_UV_OFFSET,bUoffset);
	VIM_HIF_SetReg8(V5_REG_IPP_UV_OFFSET+2,bVoffset);
}
/********************************************************************************
	Description:
		Set the UV offset of uv sephia effect only
	Parameters:
		bUoffset: the offset of U
		bVoffset:the offset of V
	Remarks:
*********************************************************************************/

void VIM_IPP_SetEffectUVThreshold(UINT8 bU_up,UINT8 bU_down,UINT8 bV_up,UINT8 bV_down)
{
	VIM_HIF_SetReg8(V5_REG_IPP_UV_THRESHOLD+1,bU_up);
	VIM_HIF_SetReg8(V5_REG_IPP_UV_THRESHOLD,bU_down);
	VIM_HIF_SetReg8(V5_REG_IPP_UV_THRESHOLD+3,bV_up);
	VIM_HIF_SetReg8(V5_REG_IPP_UV_THRESHOLD+2,bV_down);
}
/********************************************************************************
	Description:
		set the drop frame register in display sizer
	Parameters:
		 wDropFrame: 0xffff: meas all will be kept
					 0x01: meas one will be kept
					 every bit means every frames  
	Remarks:
*********************************************************************************/

void VIM_IPP_SetDisplayDropFrame(UINT16 wDropFrame)
{
	VIM_HIF_SetReg16(V5_REG_IPP_DISFRMDP,wDropFrame);//  5bit ??
}

/********************************************************************************
	Description:
		set the drop frame register in capture sizer
	Parameters:
		 wDropFrame: 0xffff: meas all will be kept
					 0x01: meas one will be kept
					 every bit means every frames  continuous
	Remarks:
*********************************************************************************/

void VIM_IPP_SetCaptureDropFrame(UINT16 wDropFrame)
{
	VIM_HIF_SetReg16(V5_REG_IPP_CAPFRMDP,wDropFrame);
}

//revised by youhai_mp4enc 2007.8.30	
#if (BOARD_TYPE==VC0820_FPGA)
//ipp close
UINT32 VIM_IPP_Close(void)
{
UINT32 Temp,Loop=0x3000;
	Temp=VIM_HIF_GetReg32(V5_REG_IPP_CTRL);
//	VIM_HIF_SetReg32(V5_REG_IPP_CTRL, 	(Temp|0x80000000)&0xffffffc0);		/*!< update window info */
	VIM_HIF_SetReg32(V5_REG_IPP_CTRL, 	Temp&0xfffffff9);		/*!< update window info */
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
	while(Loop--)
	{
		if(!VIM_HIF_GetReg32(V5_REG_IPP_WINDOW_UPDATE))
			break;
	}
	if(!Loop)
	{
#if(VIM_JPEG_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("VC0578B][JPEG]VIM_JPEG_StartEncode disable over time[ERROR]");
#endif	
	}
     return Temp;
}
void VIM_IPP_Open(UINT32 value)
{
UINT32 Loop=0x3000;

	VIM_HIF_SetReg32(V5_REG_IPP_CTRL, value);		/*!< update window info */
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
	while(Loop--)
	{
		if(!VIM_HIF_GetReg32(V5_REG_IPP_WINDOW_UPDATE))
			break;
	}
	if(!Loop)
	{
#if(VIM_JPEG_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("VC0578B][JPEG]VIM_JPEG_StartEncode disable over time[ERROR]");
#endif	
	}
}
#endif
/********************************************************************************
	Description:
		设置ipp captuer sizer 开流，关流
	Parameters:

	Remarks:
*********************************************************************************/

void VIM_IPP_SetCapSyncGen(BOOL Enable)
{
UINT32 Temp,Loop=0x3000;
	Temp=VIM_HIF_GetReg32(V5_REG_IPP_CTRL);
	if(Enable==ENABLE)
		VIM_HIF_SetReg32(V5_REG_IPP_CTRL, Temp|BIT0);
	else
		VIM_HIF_SetReg32(V5_REG_IPP_CTRL, Temp&(~BIT0));
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
	if(Enable==DISABLE)
	{
		while(Loop--)
		{
			if(!VIM_HIF_GetReg32(V5_REG_IPP_WINDOW_UPDATE))
				break;
		}
	}
	if(!Loop)
	{
#if(VIM_JPEG_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("VC0578B][JPEG]VIM_JPEG_StartEncode disable over time[ERROR]");
#endif	
	}

}
/********************************************************************************
	Description:
		Set Sourouse Window and capture Window
	Parameters:
		pt: the start point of source window
		winSize: the width and height of source window
		dispSize:the width and height of display window
	Remarks:
*********************************************************************************/
VIM_RESULT VIM_IPP_SetCaptureSize(TSnrInfo *pSensorInfo,TPoint pt, TSize winSize, TSize CaptureSize)
{
VIM_IPP_SzDnFct DownFc;
VIM_IPP_SzUpFct UpFc;
UINT16 width,Height;
VIM_IPP_CAPSIZE_MODE SourceSelect;
/*UINT32 sensor_target;
//UINT32 lcd_target=24;
UINT32 CDKLS;
UINT8 CON_L,CON_H;
UINT16 ML,MH;
UINT32 PLL_CLK = 432/2;//432M have predivider*/
       // check error
     //  if((CaptureSize.cx>(2560))||(CaptureSize.cy>2048))//angela 2006-3-10
       if((CaptureSize.cx>(2592))||(CaptureSize.cy>2048))//angela 2006-7-5
	//if((CaptureSize.cx>(2048-32))||(CaptureSize.cy>2048))
		return VIM_ERROR_CAPTURE_WINDOW;
	VIM_IPP_GetImageSize(&width,&Height);
	if((width<winSize.cx)||(Height<winSize.cy))
		return VIM_ERROR_SOURCE_WINDOW;
	
	// normal sensor clock
	//if(pSensorInfo)
		//VIM_SIF_SetSensorClk(pSensorInfo,VIM_SIF_SENSOR_CLKNORMAL);
	//youhai don't think it is necessary 2008.1.4
      //get factor and set factor	
	SourceSelect=VIM_IPP_GetCaputreSizeSelect();
	if(winSize.cx>=CaptureSize.cx)
	{
		if(winSize.cy<CaptureSize.cy)
#if(BOARD_TYPE==VC0820_FPGA)//add by zhoujian
		winSize.cy=(UINT32)winSize.cx*(UINT32)CaptureSize.cy/(UINT32)CaptureSize.cx;
#else
		return VIM_ERROR_CAPTURE_WINDOW;
#endif
		if(SourceSelect!=VIM_IPP_CAPMODE_DOWN)
			      VIM_IPP_Reset();
		VIM_IPP_CaputreSizeSelect(VIM_IPP_CAPMODE_DOWN);
		VIM_IPP_CalcSizerDownFactor(winSize.cx,CaptureSize.cx,&DownFc);
		VIM_IPP_SetCaptureSizerWFactor(&DownFc);
		if(DownFc.w1==DownFc.w2)
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_W,ENABLE);
		else
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_W,DISABLE);
		VIM_IPP_CalcSizerDownFactor(winSize.cy,CaptureSize.cy,&DownFc);
		VIM_IPP_SetCaptureSizerHFactor(&DownFc);
		if(DownFc.w1==DownFc.w2)
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_H,ENABLE);
		else
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_H,DISABLE);
	}
	else
	{
		if(winSize.cy>=CaptureSize.cy)
#if(BOARD_TYPE==VC0820_FPGA)//add by zhoujian
		winSize.cy=(UINT32)winSize.cx*(UINT32)CaptureSize.cy/(UINT32)CaptureSize.cx;
#else
		return VIM_ERROR_CAPTURE_WINDOW;
#endif
		if((winSize.cx>VIM_IPP_CSIZER_HIN_MAX)||(winSize.cy>VIM_IPP_CSIZER_VIN_MAX))
#if(BOARD_TYPE==VC0820_FPGA)//add by zhoujian
		{
			if(winSize.cx>VIM_IPP_CSIZER_HIN_MAX)
				winSize.cx=VIM_IPP_CSIZER_HIN_MAX;
			if(winSize.cy>VIM_IPP_CSIZER_VIN_MAX)
				winSize.cy=VIM_IPP_CSIZER_VIN_MAX;
		}
#else
			return VIM_ERROR_UP_SOURCE_WINDOW;
#endif
		if((CaptureSize.cx>VIM_IPP_CSIZER_HOUT_MAX)||(CaptureSize.cy>VIM_IPP_CSIZER_VOUT_MAX))
#if(BOARD_TYPE==VC0820_FPGA)//add by zhoujian
		{
			if(CaptureSize.cx>VIM_IPP_CSIZER_HOUT_MAX)
				CaptureSize.cx=VIM_IPP_CSIZER_HOUT_MAX;
			if(CaptureSize.cy>VIM_IPP_CSIZER_VOUT_MAX)
				CaptureSize.cy=VIM_IPP_CSIZER_VOUT_MAX;
		}
#else
		return VIM_ERROR_UP_SOURCE_WINDOW;
#endif

#if(BOARD_TYPE==VC0820_FPGA)//add by zhoujian
		if((CaptureSize.cx>(winSize.cx<<1))||(CaptureSize.cy>(winSize.cy<<1)))
		{
			CaptureSize.cx=winSize.cx<<1;
			CaptureSize.cy=winSize.cy<<1;
		}		
#else
		if(CaptureSize.cx>(winSize.cx<<1))
			return VIM_ERROR_UP_RESIZE_OVER;	
#endif
#if(BOARD_TYPE==VC0820_FPGA)//add by zhoujian
		if(gVc0578b_Info.LcdStatus.ARotationMode==0)
		{
			if(SourceSelect!=VIM_IPP_CAPMODE_UP)
				VIM_IPP_Reset();
			VIM_IPP_CaputreSizeSelect(VIM_IPP_CAPMODE_UP);
									
			VIM_IPP_CalcSizerUpFactor(winSize.cx,CaptureSize.cx,&UpFc);
			VIM_IPP_SetCaptureUpSizerWFactor(&UpFc);
			if(UpFc.w1==UpFc.w2)
				VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_W,ENABLE);
			else
				VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_W,DISABLE);
			VIM_IPP_CalcSizerUpFactor(winSize.cy,CaptureSize.cy,&UpFc);
			VIM_IPP_SetCaptureUpSizerHFactor(&UpFc);
					
			if(UpFc.w1==UpFc.w2)
				VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_H,ENABLE);
			else
			{
				VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_H,DISABLE);
				if(pSensorInfo)
        			{
					gVc0578b_Info.CaptureStatus.DownSensorClock=1;  
					
					/*HAL_READ_UINT32(V5_REG_SYS_CKDLSCON,CDKLS);
        				sensor_target=18;
        				CON_L=PLL_CLK/sensor_target/2-1;
        				CON_H=PLL_CLK/sensor_target-CON_L-2;
        				if((CON_L+CON_H+2)*sensor_target<PLL_CLK)
        					CON_H++;
        				ML =(CON_H<<8)+CON_L;
        				CDKLS&=0xffff0000;
        				CDKLS|=ML;
        				HAL_WRITE_UINT32(V5_REG_SYS_CKDLSCON,CDKLS);*/
        			}
        				//VIM_SIF_SetSensorClk(pSensorInfo,VIM_SIF_SENSOR_HALF);
			}
		}
		else
		{
			winSize.cx=CaptureSize.cx;
			winSize.cy=CaptureSize.cy;
			if(SourceSelect!=VIM_IPP_CAPMODE_DOWN)
					VIM_IPP_Reset();
			VIM_IPP_CaputreSizeSelect(VIM_IPP_CAPMODE_DOWN);
			VIM_IPP_CalcSizerDownFactor(winSize.cx,CaptureSize.cx,&DownFc);
			VIM_IPP_SetCaptureSizerWFactor(&DownFc);
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_W,ENABLE);
		
			VIM_IPP_CalcSizerDownFactor(winSize.cy,CaptureSize.cy,&DownFc);
			VIM_IPP_SetCaptureSizerHFactor(&DownFc);
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_H,ENABLE);
		}
#else
		if(SourceSelect!=VIM_IPP_CAPMODE_UP)
			     VIM_IPP_Reset();
		VIM_IPP_CaputreSizeSelect(VIM_IPP_CAPMODE_UP);
						
		VIM_IPP_CalcSizerUpFactor(winSize.cx,CaptureSize.cx,&UpFc);
		VIM_IPP_SetCaptureUpSizerWFactor(&UpFc);
		if(UpFc.w1==UpFc.w2)
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_W,ENABLE);
		else
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_W,DISABLE);
		VIM_IPP_CalcSizerUpFactor(winSize.cy,CaptureSize.cy,&UpFc);
		VIM_IPP_SetCaptureUpSizerHFactor(&UpFc);
		
		if(UpFc.w1==UpFc.w2)
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_H,ENABLE);
		else
		{
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_H,DISABLE);
			if(pSensorInfo)
				VIM_SIF_SetSensorClk(pSensorInfo,VIM_SIF_SENSOR_HALF);
		}
		// clk should changed 
#endif
	}
      // set value
	VIM_IPP_SetWindowPos(pt.x,pt.y);
	VIM_IPP_SetWindowSize(winSize.cx,winSize.cy);
	VIM_IPP_SetCaptureWindowSize(CaptureSize.cx,CaptureSize.cy);
	//VIM_IPP_SetCaptureWindowSize(winSize.cx,CaptureSize.cy);
	//UPDATA
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
	return VIM_SUCCEED;
}
/********************************************************************************
	Description:
		Get display size
	Parameters:
		Width: the width of thumbnail size
		height: the height of thumbnail size
		0 means 256
	Remarks:
*********************************************************************************/
void VIM_IPP_GetCapSize(PTSize Size)
{
	Size->cx=VIM_HIF_GetReg16(V5_REG_IPP_CAPTGT_SIZE);
	Size->cy=VIM_HIF_GetReg16(V5_REG_IPP_CAPTGT_SIZE+2);

}
/***********************************************************************************
Description:
		set the burst number of mpeg4 decoder.
		
Parameters:
		framemode	: One of the following <U>VM_MP4_FRAMEMODE</U>
 				enumeration values that specify mpeg4 frame mode.
 				</VM_MP4_FRMMODE_ONEFRAME>
 				</VM_MP4_FRMMODE_MBL>
 		size		: mpeg4 image size.
		
Returns:
		success	: VM_SUCCEED.
		failure	: other error value.
		
Remarks:
		
************************************************************************************/
VIM_RESULT VIM_IPP_SetMp4BurstNum(TSize size)
{
	UINT32	regburst0=0, regburst12=0;
	UINT8 framemode;
	if((size.cx>4096) || (size.cy>4096)){
		return VIM_ERROR_LBUF_PARAM_OUTOFRANGE;
	}
	framemode = (VIM_HIF_GetReg8(V5_REG_LBUF_MODE+3)>>2)&0x1;
	if(framemode == VIM_IPP_MPG4_ONEFRAME){	/*!< one frame mode */
		regburst0	= (size.cx*size.cy)>>6;
		VIM_HIF_SetReg32(V5_REG_LBUF_BURSTNUMBER0,	regburst0);
	}else {	/*!< mbl mode */
		regburst0	= size.cx>>2;
		regburst12	= ((size.cx*3)>>3)<<16;
		regburst12	|= size.cx>>3;
		VIM_HIF_SetReg32(V5_REG_LBUF_BURSTNUMBER0,	regburst0);
		VIM_HIF_SetReg32(V5_REG_LBUF_BURSTNUMBER12,regburst12);
	}
	
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	for caculate window
parameters: 
	src:         preview off
	dst:         preview on without frame
	pwin:       preview on with frame
	
Return:
		VIM_SUCCEED: cacaulte ok
		VIM_ERROR_CACAULATELE_SOURCE_WINDOW: cacaulte less source window error
Remarks:
*********************************************************************************/
static unsigned int  gcd(unsigned int m, unsigned int n)
{
  unsigned  int temp;
   if(m<n)
   {
   temp=m;
    m=n;
   n=temp;
   }
   if(!n)
   	return 0;//error
   while(n>0)
   {
     temp=m%n;
      m=n;
      n=temp;
    }
   return m;
}
VIM_RESULT VIM_IPP_ToolCaculateLessSrcWindow(TSize src, TSize dst,TSize *win)
{
	UINT16 div,temp=0;
	if((dst.cx>src.cx)||(dst.cy>src.cy))
		temp=1;
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
     div=gcd(dst.cx,dst.cy);
    if(!div)
		return VIM_ERROR_CACAULATELE_SOURCE_WINDOW;
	
#else	

	if(dst.cx>dst.cy)
		div=dst.cy;
	else
		div=dst.cx;
	if(!div)
		return VIM_ERROR_CACAULATELE_SOURCE_WINDOW;
	while((dst.cx%div)||(dst.cy%div))
	{
		div--;
		if(!div)
			return VIM_ERROR_CACAULATELE_SOURCE_WINDOW;
	}
#endif
	dst.cx/=div;
	dst.cy/=div;
RECAPSOURSE:	


//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
	if(src.cy*dst.cx<=src.cx*dst.cy)
		{
		win->cy=(src.cy)&0xfff8;
		win->cx=(src.cy*dst.cx/dst.cy)&0xfff8;
		}
	else
		{
		win->cx=(src.cx)&0xfff8;
		win->cy=(src.cx*dst.cy/dst.cx)&0xfff8;
		}

#else
       div=1;
	while((dst.cy*div<=src.cy)&&(dst.cx*div<=src.cx))
	{
		div++;
		if((div>(src.cy+1))||(div>(src.cx+1)))
			return VIM_ERROR_CACAULATELE_SOURCE_WINDOW;
	}
	win->cy=(dst.cy*(div-1))&0xfffe;
	win->cx=(dst.cx*(div-1))&0xfffe;
#endif
	if(temp)
	{
		if((win->cx>VIM_IPP_CSIZER_HIN_MAX)||(win->cy>VIM_IPP_CSIZER_VIN_MAX))
		{
			src.cx=VIM_IPP_CSIZER_HIN_MAX;
			src.cy=VIM_IPP_CSIZER_VIN_MAX;
			goto RECAPSOURSE;
		}
	}
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	for caculate window
parameters: 
	src:         preview off
	dst:         preview on without frame
	pwin:       preview on with frame
	
Return:
		VIM_SUCCEED: cacaulte ok
		VIM_ERROR_CACAULATELE_SOURCE_WINDOW: cacaulte big source window error
Remarks:
*********************************************************************************/
#define VIM_IPP_ADJUST_NUM	0
VIM_RESULT VIM_IPP_ToolCaculateBigDstWindow(TSize src, TSize dst,TSize *win)
{
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)	
TSize winsize;
#else
	TSize winsize,winsize1;
#endif
	if(dst.cx>(src.cx))			//2006-11-17 angela 
		dst.cx=src.cx;
	if(dst.cy>(src.cy))			//2006-11-17 angela 
		dst.cy=src.cy;
	
REPEATCACULATE:	
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)	
//revise by youhai for mp4 display ,we need keep all the video info
if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_DISPLAYMPG4_MODE)
  {
 	 if ((src.cy*dst.cx)>=(dst.cy*src.cx))
	{

		winsize.cx=(dst.cx)&0xfff8;
		winsize.cy=(((UINT32)dst.cx*(UINT32)src.cy/(UINT32)src.cx))&0xfff8;
	}
	else
	{

		winsize.cy=(dst.cy)&0xfff8;
		winsize.cx=(((UINT32)dst.cy*(UINT32)src.cx/(UINT32)src.cy))&0xfff8;
	}
 }
else
{
 	 if ((src.cy*dst.cx)<=(dst.cy*src.cx))
	{

		winsize.cx=(dst.cx)&0xfff8;
		winsize.cy=(((UINT32)dst.cx*(UINT32)src.cy/(UINT32)src.cx))&0xfff8;
	}
	else
	{

		winsize.cy=(dst.cy)&0xfff8;
		winsize.cx=(((UINT32)dst.cy*(UINT32)src.cx/(UINT32)src.cy))&0xfff8;
	}
}
	win->cx=winsize.cx;
	win->cy=winsize.cy;	
#else

 	 if ((src.cy/src.cx)>(dst.cy/dst.cx))
	{

		winsize.cx=(dst.cx)&0xfffc;
		winsize.cy=(((UINT32)winsize.cx*(UINT32)src.cy/(UINT32)src.cx))&0xfffc;
	}
	else if ((src.cy/src.cx)<(dst.cy/dst.cx))
	{

		winsize.cy=(dst.cy)&0xfffc;
		winsize.cx=(((UINT32)winsize.cy*(UINT32)src.cx/(UINT32)src.cy))&0xfffc;
	}
	else
	{
		winsize.cy=(dst.cy)&0xfffc;
		winsize.cx=(((UINT32)winsize.cy*(UINT32)src.cx/(UINT32)src.cy))&0xfffc;
		winsize1.cx=(dst.cx)&0xfffc;
		winsize1.cy=(((UINT32)winsize1.cx*(UINT32)src.cy/(UINT32)src.cx))&0xfffc;
		if (winsize1.cx>=winsize.cx)
		{
			winsize.cx=winsize1.cx;
			winsize.cy=winsize1.cy;
		}
	}
	win->cx=winsize.cx;
	win->cy=winsize.cy;	
#endif	
	if((win->cx>VIM_IPP_DISPLAY_MAX_W)||(win->cy>VIM_IPP_DISPLAY_MAX_H))
		return VIM_ERROR_DISPLAY_WINDOW;

	if(win->cx>AM_MAXPIXEL)
	{
		dst.cy-=4;
		goto	REPEATCACULATE;
	}
	if(win->cy>AM_MAXPIXEL)
	{
		dst.cx-=4;
		goto	REPEATCACULATE;
	}
/*#if (BOARD_TYPE==VC0820_FPGA)//add by zhoujian

	if(win->cx>dst.cx)
	{
		win->cx=dst.cx;
	}
	if(win->cy>dst.cy)
	{
		win->cy=dst.cy;
	}
#endif*/
	/*if(((VIM_HIF_GetReg8(V5_REG_IPP_SPECTRL)>>5)&0x3)==3)//display mode
	{
		if((((win->cx-dst.cx)>>1)+dst.cx)>VIM_IPP_DISPLAY_MAX_W)	//angela 2007-3-5
		{
	#if(VIM_IPP_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintString("[VC0578B][IPP]DISPLAY IS MORE THAN 320");
	#endif	
			dst.cy-=4;
			goto	REPEATCACULATE;
		}
	}*/
	return VIM_SUCCEED;	
}

/********************************************************************************
Description:
	for caculate window
parameters: 
	src:         preview off
	dst:         preview on without frame
	pwin:       preview on with frame
	
Return:
		VIM_SUCCEED: cacaulte ok
		VIM_ERROR_CACAULATELE_SOURCE_WINDOW: cacaulte less source window error
Remarks:
*********************************************************************************/
VIM_RESULT VIM_IPP_ToolCaculateLessDisplayWindow(TSize src, TSize dst,TSize *dis)
{
	TSize destsize,destsize1;
 	//dst.cx=(dst.cx<src.cx)?dst.cx : src.cx;
	//dst.cy=(dst.cy<src.cy)?dst.cy : src.cy;
	#if 0 //(now can up size, so cancel this case, Amanda Deng)
	if((src.cx<dst.cx)&&(src.cy<dst.cy))  //2006-8-4 angela 
	{
		destsize.cy=src.cy&0xfffc;
		destsize.cx=src.cx&0xfffc;
	}
 	else 
	#endif	
	if ((src.cy/src.cx)>(dst.cy/dst.cx))
	{
		destsize.cy=dst.cy&0xfffe;
		destsize.cx=(destsize.cy*src.cx/src.cy)&0xfffe;
	}
	else if ((src.cy/src.cx)<(dst.cy/dst.cx))
	{
		destsize.cx=dst.cx&0xfffe;
		destsize.cy=(destsize.cx*src.cy/src.cx)&0xfffe;
	}
	else
	{
		destsize.cy=dst.cy&0xfffe;

		destsize.cx=(destsize.cy*src.cx/src.cy)&0xfffe;
		destsize1.cx=dst.cx&0xfffe;
		destsize1.cy=(destsize1.cx*src.cy/src.cx)&0xfffe;
		if (destsize1.cx<destsize.cx)
		{
			destsize.cx=destsize1.cx;
			destsize.cy=destsize1.cy;
		}
	}
	dis->cx=destsize.cx;
	dis->cy=destsize.cy;	
	if((dis->cx>VIM_IPP_DISPLAY_MAX_W)||(dis->cy>VIM_IPP_DISPLAY_MAX_H))
		return VIM_ERROR_DISPLAY_WINDOW;
	return VIM_SUCCEED;
}
#if(BOARD_TYPE==VC0820_FPGA) //add by zhoujian
void VIM_IPP_SetThumbnailEn(BOOL Enable)
{
	UINT32 Loop=0x3000;
    UINT16 bSizerCtrl;

	bSizerCtrl=VIM_HIF_GetReg16(V5_REG_IPP_CTRL);
	bSizerCtrl&=(~BIT3);
	VIM_HIF_SetReg16(V5_REG_IPP_CTRL,bSizerCtrl);
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);	

	while(Loop--)
	{
		if(!VIM_HIF_GetReg32(V5_REG_IPP_WINDOW_UPDATE))
			break;
	}
	
	if(Enable==ENABLE)
	{
		bSizerCtrl=VIM_HIF_GetReg16(V5_REG_IPP_CTRL);
		bSizerCtrl|=BIT3;
		VIM_HIF_SetReg16(V5_REG_IPP_CTRL,bSizerCtrl);
	}
	
	return;
}
#endif

VIM_RESULT VIM_IPP_SetDisplaySizerSize(TPoint winpt, TSize winSize, TSize DisplaySize, TSize* DestSize)
{
	VIM_IPP_SzDnFct DownFc;
	VIM_IPP_SzUpFct UpFc;
	VIM_IPP_CAPSIZE_MODE SourceSelect;
	UINT16 width,height;

    // check error
	VIM_IPP_GetImageSize(&width,&height);
	if((width<winSize.cx)||(height<winSize.cy))
		return VIM_ERROR_SOURCE_WINDOW;
	//get factor and set factor	
	SourceSelect=VIM_IPP_GetCaputreSizeSelect();
	if(winSize.cx>=DisplaySize.cx)
	{
		//source size(width) >display width, sizer down
		if(winSize.cy<DisplaySize.cy)
			winSize.cy=(UINT32)winSize.cx*(UINT32)DisplaySize.cy/(UINT32)DisplaySize.cx;
		if(SourceSelect!=VIM_IPP_CAPMODE_DOWN)
			VIM_IPP_Reset();
		VIM_IPP_CaputreSizeSelect(VIM_IPP_CAPMODE_DOWN);
		VIM_IPP_CalcSizerDownFactor(winSize.cx,DisplaySize.cx,&DownFc);		
		VIM_IPP_SetCaptureSizerWFactor(&DownFc);
		//if(DownFc.w1==0x0)
		if(DownFc.ifmaxdiv)
		{
			DisplaySize.cx=winSize.cx*(DownFc.w2)/128;
			DisplaySize.cx&=0xfffe;
		}
		if(DownFc.w1==DownFc.w2)
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_W,ENABLE);
		else
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_W,DISABLE);
		VIM_IPP_CalcSizerDownFactor(winSize.cy,DisplaySize.cy,&DownFc);
		VIM_IPP_SetCaptureSizerHFactor(&DownFc);
		//if(DownFc.w1==0x0)
		if(DownFc.ifmaxdiv)
		{
			DisplaySize.cy=winSize.cy*(DownFc.w2)/128;
			DisplaySize.cy&=0xfffe;
		}
		if(DownFc.w1==DownFc.w2)
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_H,ENABLE);
		else
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_H,DISABLE);
	}
	else
	{
		//source size(width) <display width, sizer up
		if(winSize.cy>DisplaySize.cy)
			winSize.cy=(UINT32)winSize.cx*(UINT32)DisplaySize.cy/(UINT32)DisplaySize.cx;
		//display size >= window size *2
		if((DisplaySize.cx>(winSize.cx<<1))||(DisplaySize.cy>(winSize.cy<<1)))
		{
			DisplaySize.cx=winSize.cx<<1;
			DisplaySize.cy=winSize.cy<<1;
		}	
				
		if(gVc0578b_Info.LcdStatus.ARotationMode==0)
		{
			// when sizer up can not rotation A layer
			if(SourceSelect!=VIM_IPP_CAPMODE_UP)
				VIM_IPP_Reset();
			VIM_IPP_CaputreSizeSelect(VIM_IPP_CAPMODE_UP);
									
			VIM_IPP_CalcSizerUpFactor(winSize.cx,DisplaySize.cx,&UpFc);
			VIM_IPP_SetCaptureUpSizerWFactor(&UpFc);
			if(UpFc.w1==UpFc.w2)
				VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_W,ENABLE);
			else
				VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_W,DISABLE);
			VIM_IPP_CalcSizerUpFactor(winSize.cy,DisplaySize.cy,&UpFc);
			VIM_IPP_SetCaptureUpSizerHFactor(&UpFc);
			if(UpFc.w1==UpFc.w2)
				VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_H,ENABLE);
			else
			{
				VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_UP_H,DISABLE);
			}
		}
		else
		{
			// rotation 90/270  can not sizer up
			DisplaySize.cx=winSize.cx;
			DisplaySize.cy=winSize.cx;
			if(SourceSelect!=VIM_IPP_CAPMODE_DOWN)
				VIM_IPP_Reset();
			VIM_IPP_CaputreSizeSelect(VIM_IPP_CAPMODE_DOWN);
			VIM_IPP_CalcSizerDownFactor(winSize.cx,DisplaySize.cx,&DownFc);
			VIM_IPP_SetCaptureSizerWFactor(&DownFc);
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_W,ENABLE);
			VIM_IPP_CalcSizerDownFactor(winSize.cy,DisplaySize.cy,&DownFc);
			VIM_IPP_SetCaptureSizerHFactor(&DownFc);
			VIM_IPP_SetSizeBypassEnable(VIM_IPP_BPSIZER_CAP_DOWN_H,ENABLE);
		}
	}
    // set value
	VIM_IPP_SetWindowPos(winpt.x,winpt.y);
	VIM_IPP_SetWindowSize(winSize.cx,winSize.cy);
	VIM_IPP_SetCaptureWindowSize(DisplaySize.cx,DisplaySize.cy);
	DestSize->cx = DisplaySize.cx;
	DestSize->cy = DisplaySize.cy;
	#if(VIM_IPP_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintDec("[VC0578][IPP]real src window position xpos:  ",winpt.x);
		VIM_USER_PrintDec("[VC0578][IPP]real src window position ypos:	",winpt.y);
		VIM_USER_PrintDec("[VC0578][IPP]real src window size width:  	",winSize.cx);
		VIM_USER_PrintDec("[VC0578][IPP]real src window size height:	",winSize.cy);
		VIM_USER_PrintDec("[VC0578][IPP]real dest display size width:  	",DisplaySize.cx);
		VIM_USER_PrintDec("[VC0578][IPP]real dest display size height:	",DisplaySize.cy);
	#endif	
	//UPDATA
	VIM_HIF_SetReg32(V5_REG_IPP_WINDOW_UPDATE, 0x01);		/*!< update window info */
	return VIM_SUCCEED;
}



/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2005                                                     
 * ------------------------------------------------------------------------------
 * [File Name]            : VIM_HIGH_API.h
 * [Description]          : 适用于578b.和High API 相关的函数 
 * [Author]                 : WendyGuo	
 * [Date Of Creation]  : 2007-04-02 
 * [Platform]              : 平台无关
 * [Note]                    : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        		Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2006-04-02  	WendyGuo         	Created 
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
#include "msg.h"
#include "VIM_COMMON.h"
#include "AEESTDLIB.h"

#define DRVINIT 1
#define LCDDRV 1
#define JPEGCAP 1
#define JPEGDIS 1
#define VIDEOCAP 1
#define VIDEDISP 1
#define OTHERDRV 0

//static char g_Code_Version[]={" VC0578b V1.0 2007-8-30 "};

void *g_first_available_memory = NULL; 

#if DRVINIT
#if 0
/********************************************************************************
video driver initial related
*******************************************************************************/

/********************************************************************************
Description:
Initialize 5X(global parameters, 5X modules)
	Return
	if return V5H_SUCCEED initialize succeed,
	Return 1:  read and write register wrong 
	Return 2:  read and write register wrong (MCLK domain)
	Return 3:  read and write sram wrong	
	Return 4:  Clk 's parameter  wrong
Note:
	This function should be called before V5x can work, before Initialize, 5x chip should be reset.
	This function only can be called one time.
	After this function, the 528 will be the bypass mode
*******************************************************************************/
UINT16 VIM_HAPI_InitVc05x(void)
{
	VIM_RESULT Result;
	VIM_USER_PrintString((char *)g_Code_Version);
	VIM_MAPI_InitIfo();
	Result=VIM_MAPI_InitHif();
	
	if(Result)
	return (UINT16)Result;

#if 1//( PROJECT_NU820_VERSION!=4) //NU820_MOBILE_RELEASE

	//Init sensor
#if 1//(PROJECT_NU820_VERSION >= NU820_MP4)
#else
	Result=VIM_MAPI_AutoFindSensor();
#endif

#if 1//(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	if(Result)
		MMDPrintf("have not find sensor\n");
	else
		MMDPrintf("%s\n",gVc0578b_Info.pSensorInfo->desc);
		//MMDPrintf("have find sensor\n");
#endif	
	//Init lcd
	//VIM_LCDIF_ResetPanel();
	//VIM_MAPI_SwitchPanel(VIM_HAPI_LCDPANEL_MAIN);
	VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_16BIT);		//wendy guo 4/5/2007 add
	gVc0578b_Info.CaptureStatus.SensorClock=36;
	// init interrupt
	VIM_HIF_ClearIntModule(INT_ALL);
	//VIM_INT_Init();
#if	VC0848SRAM
#else
	VIM_MARB_RamInfo_Init((UINT32)g_first_available_memory,
							0x20060000 - (UINT32)g_first_available_memory,
							V5_1TSRAM_BASE,
							V5_1TSRAM_TOTALSIZE);
#endif
	//VIM_HAPI_SetWorkMode(VIM_HAPI_MODE_POWERDOWN);
#else
	gVc0578b_Info.pSensorInfo= &gSensorInfo_DUMMY;
#endif

	return VIM_SUCCEED;
}
#endif
/********************************************************************************
Description:
	Set 5x chip work mode
Parameters:
	WorkMode:
		V5H_MODE_CAMERAON
		V5H_MODE_BYPASS
		V5H_MODE_PHOTOPROCESS
		V5H_MODE_DIRECTDISPLAY
	Return:
		V5H_SUCCEED: set work mode ok
		V5H_FAILED:     error work mode,or no this work mode
	Note:
		If the original state and the des state is the same 
		,the function will return V5H_SUCCEED and doing nothing.
********************************************************************************/
UINT16 VIM_HAPI_SetWorkMode(VIM_HAPI_WORKMODE WorkMode)
{
//VIM_RESULT result;

	if(WorkMode==gVc0578b_Info.ChipWorkMode)
		return VIM_SUCCEED;
#if 0
	if((gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS)||(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_POWERDOWN))
		{
			if((gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_POWERDOWN))
				VIM_SET_XCLKON();
			VIM_HIF_SetBypassOrNormal(VIM_HIF_NORMALTYPE);
			//if((gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_POWERDOWN))  //temp delete for bypass bug
			{
				if(VIM_BYPASS_RESETCHIP)
					VIM_USER_Reset();
				result=VIM_MAPI_InitHif();
				if(result)
				{
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintDec("******** VIM_HAPI_SetWorkMode error*********** ,result =",result);
#endif	
					return (UINT16)result;
				}
				VIM_SIF_SifInitI2c(gVc0578b_Info.pSensorInfo,
				VIM_HIF_GetMclock()); // Init i2c
				result = VIM_SIF_SifSensorInit(gVc0578b_Info.pSensorInfo,&gVc0578b_Info.VIM_SIF_AeAdjust);
				if (result) 
				{
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
					VIM_USER_PrintDec("\n sensor init error,result =",result);
#endif	
			 		 // return	result;
				}
			}
			VIM_DISP_SetGbufFormat((VIM_DISP_BFORMAT)gVc0578b_Info.LcdStatus.ColorDep); //angela 2006-8-3 for changed value
			VIM_MAPI_SwitchPanel(gVc0578b_Info.LcdStatus.NowPanel);
		}
	VIM_HIF_SetModClkOnOff(VIM_HIF_CLKGATE_ALL,ENABLE);
	switch(WorkMode)
	{
		case VIM_HAPI_MODE_CAMERAON:
			VIM_MARB_Set1TSramMode(VIM_MARB_1TSRAM_ALL,VIM_MARB_1TSRAM_POWERON);
			VIM_SIF_SetSensorState(gVc0578b_Info.pSensorInfo,VIM_SIF_SENSOR_POWERON);
			break;
		case VIM_HAPI_MODE_POWERDOWN:
			VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_OFF,VIM_HAPI_MAXPIXEL);
			//VIM_MAPI_SetChipMode(VIM_MARB_BYPASS_MODE,VIM_IPP_HAVE_NOFRAME);
			VIM_SIF_SetSensorState(gVc0578b_Info.pSensorInfo,VIM_SIF_SENSOR_STANDBY);
			VIM_MARB_Set1TSramMode(VIM_MARB_1TSRAM_ALL,VIM_MARB_1TSRAM_OFF);
//revised by youhai_mp4enc 2007.8.30	
#if (BOARD_TYPE==VC0820_FPGA)
//to be done by youhai
#else
			VIM_HIF_SetReg32(V5_REG_PAD_MEM_PC, 1);		/*!all sdame pin input*/
#endif
			VIM_HIF_SetModClkOnOff(VIM_HIF_CLKGATE_ALL,DISABLE);
			VIM_HIF_SetLdoStatus(VIM_LDO_OFF);
			VIM_HIF_SetBypassOrNormal(VIM_HIF_BYPASSTYE);
			VIM_SET_XCLKOFF();
			break;
		case VIM_HAPI_MODE_PHOTOPROCESS:		
			VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_OFF,VIM_HAPI_MAXPIXEL);
			VIM_SIF_SetSensorState(gVc0578b_Info.pSensorInfo,VIM_SIF_SENSOR_STANDBY);
			VIM_MARB_Set1TSramMode(VIM_MARB_1TSRAM_ALL,VIM_MARB_1TSRAM_POWERON);
			VIM_HIF_SetModClkOnOff((VIM_HIF_CLKGATE_ISP|VIM_HIF_CLKGATE_SIF),DISABLE);
			break;
		case VIM_HAPI_MODE_BYPASS:
			VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_OFF,VIM_HAPI_MAXPIXEL);
			VIM_MARB_Set1TSramMode(VIM_MARB_1TSRAM_ALL,VIM_MARB_1TSRAM_STANDBY);
			//VIM_MAPI_SetChipMode(VIM_MARB_BYPASS_MODE,VIM_IPP_HAVE_NOFRAME);
			VIM_HIF_SetModClkOnOff(VIM_HIF_CLKGATE_ALL,DISABLE);
			VIM_HIF_SetBypassOrNormal(VIM_HIF_BYPASSTYE);
			break;
		default:
			return VIM_ERROR_PARAMETER;
	}
#endif
	gVc0578b_Info.ChipWorkMode=WorkMode;
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	Get 5x chip work mode
Parameters:
	WorkMode:
		V5H_MODE_CAMERAON
		V5H_MODE_BYPASS
		V5H_MODE_PHOTOPROCESS
		V5H_MODE_DIRECTDISPLAY
	Return:
		V5H_SUCCEED: set work mode ok
		V5H_FAILED:     error work mode,or no this work mode
	Note:
		If the original state and the des state is the same 
		,the function will return V5H_SUCCEED and doing nothing.
********************************************************************************/
UINT16 VIM_HAPI_GetWorkMode(void)
{
   return gVc0578b_Info.ChipWorkMode;
}

#endif

#if 0
#define V5_REG_LCDIF_START_X				(V5_REG_LCDIF_BASE+0x03a)
#define V5_REG_LCDIF_START_Y				(V5_REG_LCDIF_BASE+0x038)

#define V5_REG_LCDIF_END_X				(V5_REG_LCDIF_BASE+0x03e)
#define V5_REG_LCDIF_END_Y				(V5_REG_LCDIF_BASE+0x03c)


#define VIM_DISP_BySetRSlow(adr)    *(volatile UINT16 *)0x8000000=adr
#define VIM_DISP_BySetRShigh(val)   *(volatile UINT16 *)0x8000200=val
void LcdRotate_S1D19320(UINT8 degree, UINT8 WorkMode)
{
	if (VIM_HAPI_MODE_BYPASS == WorkMode )
	{
		switch(degree)
		{
			case VIM_HAPI_ROTATE_0:
				VIM_DISP_BySetRSlow((UINT16)0xbc<<8);
				VIM_DISP_BySetRShigh((UINT16)0x00<<8);
				break;
				
			case VIM_HAPI_ROTATE_90:
				VIM_DISP_BySetRSlow((UINT16)0xbc<<8);
				VIM_DISP_BySetRShigh((UINT16)0x09<<8);				
				break;
				
			case VIM_HAPI_ROTATE_180:
				VIM_DISP_BySetRSlow((UINT16)0xbc<<8);
				VIM_DISP_BySetRShigh((UINT16)0x03<<8);				
				break;
				
			case VIM_HAPI_ROTATE_270:
				VIM_DISP_BySetRSlow((UINT16)0xbc<<8);
				VIM_DISP_BySetRShigh((UINT16)0x0a<<8);
				break;
				
			case VIM_HAPI_MIRROR_0:
				VIM_DISP_BySetRSlow((UINT16)0xbc<<8);
				VIM_DISP_BySetRShigh((UINT16)0x02<<8);				
				break;
				
			case VIM_HAPI_MIRROR_90:
				VIM_DISP_BySetRSlow((UINT16)0xbc<<8);
				VIM_DISP_BySetRShigh((UINT16)0x08<<8);				
				break;
				
			case VIM_HAPI_MIRROR_180:	
				VIM_DISP_BySetRSlow((UINT16)0xbc<<8);
				VIM_DISP_BySetRShigh((UINT16)0x01<<8);				
				break;
				
			case VIM_HAPI_MIRROR_270:
				VIM_DISP_BySetRSlow((UINT16)0xbc<<8);
				VIM_DISP_BySetRShigh((UINT16)0x0b<<8);				
				break;
				
			
		}

	
	}
	else
	{
		switch(degree)
		{
			case VIM_HAPI_ROTATE_0:
//#ifdef RV_ENABLE
				VIM_LCDIF_RsLExW((UINT16)0x03);
				VIM_LCDIF_RsHExW((UINT16)0x1030);
				VIM_LCDIF_RsLExW((UINT16)0x60);
				VIM_LCDIF_RsHExW((UINT16)0x2700);// for 320x240 set 0xa700, for 240x320 set 0x2700, by lizg
		
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 3);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 0x0807);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 0);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 0);
/*#else
				VIM_LCDIF_RsLExW((UINT16)0x03);
				VIM_LCDIF_RsHExW((UINT16)0x1030);

				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 3);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 0x0807);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 0);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 0);
#endif*/
				break;
				
			case VIM_HAPI_ROTATE_90:
//#ifdef RV_ENABLE
				VIM_LCDIF_RsLExW((UINT16)0x03);
				VIM_LCDIF_RsHExW((UINT16)0x1028);
				
				VIM_LCDIF_RsLExW((UINT16)0x60);
				VIM_LCDIF_RsHExW((UINT16)0xa700);// for 320x240 set 0xa700, for 240x320 set 0x2700, by lizg

				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 0x0807);				//exchange  parm->styp and  parm->stxp
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 0x3);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 0);				//exchange
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 0);

/*#else
				VIM_LCDIF_RsLExW((UINT16)0x03);
				VIM_LCDIF_RsHExW((UINT16)0x1018);

				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 0x0807);				//exchange  parm->styp and  parm->stxp
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 0x3);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 0);				//exchange
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 0);
#endif*/
		
				break;
				
			case VIM_HAPI_ROTATE_180:
				VIM_LCDIF_RsLExW((UINT16)0xbc<<8);
				VIM_LCDIF_RsHExW((UINT16)0x03<<8);
				
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 4);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 0x0608);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 12);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 0x0e10);
				
				break;
			case VIM_HAPI_MIRROR_180:
				VIM_LCDIF_RsLExW((UINT16)0xbc<<8);
				VIM_LCDIF_RsHExW((UINT16)0x01<<8);
		
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 4);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 0x0608);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 12);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 0x0e10);		
				break;
			case VIM_HAPI_ROTATE_270:
				VIM_LCDIF_RsLExW((UINT16)0xbc<<8);
				VIM_LCDIF_RsHExW((UINT16)0x0a<<8);

				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 0x0608);				//exchange  parm->styp and  parm->stxp
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 4);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 0x0e10);				//exchange
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 12);
				
				break;
				
			case VIM_HAPI_MIRROR_0:
				VIM_LCDIF_RsLExW((UINT16)0xbc<<8);
				VIM_LCDIF_RsHExW((UINT16)0x02<<8);
		
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 4);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 0x0608);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 12);				
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 0x0e10);
				break;
				
			case VIM_HAPI_MIRROR_90:
				VIM_LCDIF_RsLExW((UINT16)0xbc<<8);
				VIM_LCDIF_RsHExW((UINT16)0x08<<8);

				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 0x0608);				//exchange  parm->styp and  parm->stxp
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 4);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 0x0e10);				//exchange
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 12);
				
				break;
				
			
				
			case VIM_HAPI_MIRROR_270:
				VIM_LCDIF_RsLExW((UINT16)0xbc<<8);
				VIM_LCDIF_RsHExW((UINT16)0x0b<<8);

				VIM_HIF_SetReg16(V5_REG_LCDIF_START_X, 0x0608);				//exchange  parm->styp and  parm->stxp
				VIM_HIF_SetReg16(V5_REG_LCDIF_START_Y, 4);
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_X, 0x0e10);				//exchange
				VIM_HIF_SetReg16(V5_REG_LCDIF_END_Y, 12);
				break;
				
			default:
				break;
		}

	
	}
	
return;	
}

void VIM_HAPI_LCDRotate(UINT8 degree)
{	
#if HX_PANEL

#elif LPFD5420A 
	
#else
	LcdRotate_S1D19320(degree,VIM_HAPI_MODE_CAMERAON);
#endif
}
#endif

#if LCDDRV

/********************************************************************************
LCD related
*******************************************************************************/

volatile unsigned int g_b0LayerUpdateFinished=1;
volatile unsigned int g_b1LayerUpdateFinished=1;
TSize gb0lcdsize ={0,0};
TSize gb1lcdsize ={0,0};

/*************************************************************
Description:
	This function will choose a mode about relationship of a layer and b layer .
	and overlay keycolor(blending value)
Parameters:
	Mode: LCD work mode
			V5H_LCDMODE_AFIRST,
			V5H_LCDMODE_BLONLY,
			V5H_LCDMODE_OVERLAY,
			V5H_LCDMODE_BLEND,
	Value:
		In V5H_ V5H_LCDMODE_AFIRST,V5H_ LCDMODE_BLONLY, mode
			This value has no effect.
		Else in V5H_ LCDMODE _OVERLAY, mode
			The value is the key color which color will be transparent
		in	V5H_ LCDMODE _BLEND, mode
			The value is the Blending value(0-255)
	Return:
		V5H_SUCCEED: set ok
		V5H_ERROR_WORKMODE: error work mode
		V5H_FAILED: error parameter
	Note
		In V5H_ LCDMODE _BLEND mode, the image should be the 15bit colordep.
	This function can be used in Camera open mode or Photo process mode
	or direct display mode
*********************************************************************************/
UINT16 VIM_HAPI_SetLCDWorkMode(VIM_HAPI_LCDWORKMODE byABLayerMode, UINT16 wValue)
{
	UINT32 dwKeyColor = 0;
	if ((VIM_HAPI_MODE_BYPASS == gVc0578b_Info.ChipWorkMode) || (VIM_HAPI_MODE_POWERDOWN ==gVc0578b_Info.ChipWorkMode))
	{
		return VIM_ERROR_WORKMODE;
	}

	switch(byABLayerMode)
	{
		case VIM_HAPI_LCDMODE_AFIRST:
			// Enable A Layer, disable overlay and blending
			VIM_DISP_SetWorkMode(VIM_DISP_AFIRST);
			break;
		case VIM_HAPI_LCDMODE_BFIRST:
			// Enable A Layer, disable overlay and blending
			VIM_DISP_SetWorkMode(VIM_DISP_BFIRST);
			break;
		case VIM_HAPI_LCDMODE_BLONLY:
			// Disable A Layer, disable overlay and blending
			VIM_SIF_EnableSyncGen(DISABLE);
 			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);		//wendy guo 3/27/2007
 			VIM_DISP_SetLayerEnable(VIM_DISP_B0LAYER,ENABLE);		//wendy guo 4/6/2007 eanble B0 layer
			VIM_DISP_SetWorkMode(VIM_DISP_BFIRST);
			break;
		case VIM_HAPI_LCDMODE_OVERLAY:
			// Enable A Layer, enable overlay, disable blending
			if(gVc0578b_Info.LcdStatus.ColorDep==VIM_HAPI_COLORDEP_A16BIT)
			{
				dwKeyColor = (((UINT32)wValue) & 0x1f) << 3; // B 
				dwKeyColor |= (((UINT32)wValue) & 0x7e0) << 6; // G
				dwKeyColor |= (((UINT32)wValue) & 0xf800) << 9; // R 
				
			}
				
			else
			{
				dwKeyColor = (((UINT32)wValue) & 0x1f) << 3; // B 
				dwKeyColor |= (((UINT32)wValue) & 0x7e0) << 5; // G
				dwKeyColor |= (((UINT32)wValue) & 0xf800) << 8; // R 
			}
			
			VIM_DISP_SetOverlayKeyColor(VIM_DISP_ALLBLAYER,dwKeyColor);
			VIM_DISP_SetWorkMode(VIM_DISP_ALL_OVERLAY);		//wendy guo test 4/16/2007
						
			VIM_DISP_SetLayerEnable(VIM_DISP_ALLBLAYER,ENABLE);	//must set _ALLBLAYER parameter
			
			//VIM_DISP_SetOverlayMode(VIM_DISP_ALLBLAYER, VM_LCDC_OVERLAY_INVERT);		//wendy guo 4/16/2007
			VIM_DISP_SetOverlayMode(VIM_DISP_ALLBLAYER, VM_LCDC_OVERLAY_TRANSPARENT);
			break;
		case VIM_HAPI_LCDMODE_BLEND:
			// Enable A Layer, disable overlay, enable blending
			VIM_DISP_SetBlendRatio(VIM_DISP_ALLBLAYER, wValue);
			VIM_DISP_SetWorkMode(VIM_DISP_ALL_BLEND);			//wendy guo test 4/16/2007
			VIM_DISP_SetLayerEnable(VIM_DISP_ALLBLAYER,ENABLE);
			break;
		default:
			return VIM_ERROR_PARAMETER;
	}
	gVc0578b_Info.LcdStatus.WorkMode=byABLayerMode;
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	Draw a Rectangle  and update to lcd use a layer mem
Parameters:
	startx: the x coordinate of the Rectangle start point(pixel, start from 0)
	starty: the y coordinate of the Rectangle start point(pixel, start from 0)
	Width: the width of the Rectangle(pixel)
	Height: the height of the Rectangle(pixel)
	uspImage: the point of memory which to fill the Rectangle
	Return:
		V5H_SUCCEED: set ok
		V5H_ERROR_WORKMODE: error work mode
		V5H_FAILED: error parameter
	Note：
		 write to the 5x internal ram, and update to LCD.
		 use this function in display video (no mamery case)
	This function can be used in Camera open mode or Photo process mode 
Remarks:
	valid
*********************************************************************************/
UINT16 VIM_HAPI_UpdateLcdDirectly(UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height, HUGE  UINT8 *pwImage)
{
	TSize BSize,NowBSize;

	TPoint startPoint,point,Newpoint;
	VIM_RESULT result=VIM_SUCCEED;
	if ((VIM_HAPI_MODE_BYPASS == gVc0578b_Info.ChipWorkMode) || (VIM_HAPI_MODE_POWERDOWN ==gVc0578b_Info.ChipWorkMode))
	{
		return VIM_ERROR_WORKMODE;
	}

	if ((NULL == pwImage) || (0 ==Width) || (0 == Height))
	{
		return VIM_ERROR_PARAMETER;
	}

	if(gVc0578b_Info.DisplayStatus.outputtype == VIM_OUTPUTTV)
	{
		return VIM_ERROR_WORKMODE;
	}
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
	VIM_IPP_SetSizeToLcdEnable(DISABLE);
	point.x=startx;
	point.y=starty;
	NowBSize.cx=Width;
	NowBSize.cy=Height;	
	
	VIM_HIF_SetReg32(V5_REG_LCDC_GBBA0,gVc0578b_Info.MarbStatus.MapList.layerA0start+V5_1TSRAM_BASE);

	// Get B Layer star point and size
	////b start point  is always 0,0 VIM_DISP_GetBStartPoint
	VIM_DISP_GetBSize(VIM_DISP_B0LAYER,&BSize);
	// When LCD rotate,change BSize width and height
	VIM_DISP_GetBStartPoint(VIM_DISP_B0LAYER,&startPoint);			//wendy guo 4/12/2007

	result=VIM_DISP_SetB0_DisplayPanel(point,NowBSize);
	if(result)
		return result;
	VIM_MAPI_GetNewPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.BRotationMode,&point,NowBSize,&Newpoint);
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS+2,Newpoint.x);
	VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS,Newpoint.y);

	if(gVc0578b_Info.LcdStatus.BRotationMode%2)
		BSize.cx=BSize.cy;
	
	VIM_HIF_WriteSram(gVc0578b_Info.MarbStatus.MapList.layerA0start ,
		(UINT8 *)(pwImage),Width * 2 *Height, VIM_HIF_SRAM_MODE);

	point.x=0;
	point.y=0;
	VIM_DISP_SetB0_RefreshBlock(point,NowBSize);


	//set update control register
	VIM_DISP_Update(VIM_DISP_B0LAYER);
	VIM_HIF_SetReg32(V5_REG_LCDC_GBBA0,gVc0578b_Info.MarbStatus.MapList.layerB0start+V5_1TSRAM_BASE);

	VIM_IPP_SetSizeToLcdEnable(ENABLE);
	result=VIM_DISP_SetB0_DisplayPanel(startPoint,BSize);	
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);	
	return result;
}

UINT32 VIM_GetBlayerBuff(VIM_HAPI_LAYER BLayer)
{
	switch(BLayer)
	{
		case VIM_DISP_B0LAYER:
			return gVc0578b_Info.MarbStatus.MapList.layerB0start;
		case VIM_DISP_B1LAYER:
			return gVc0578b_Info.MarbStatus.MapList.layerB1start;
	}
	return 0;
			
}

extern int rotateStauts;
void VIM_SetRT(UINT32 rtFlag)
{
	
	
	// Set Memory Width
	if(rtFlag)
	{
	       gVc0578b_Info.LcdStatus.Size.cx = MMD_LCD_HEIGHT;
              gVc0578b_Info.LcdStatus.Size.cy = MMD_LCD_WIDTH;
	       rotateStauts= 1 ;
		VIM_HIF_SetReg16(V5_REG_LCDC_B0_SIZE+2, MMD_LCD_HEIGHT-1);
		VIM_HIF_SetReg16(V5_REG_LCDC_B0_SIZE,  MMD_LCD_WIDTH-1);
		VIM_HIF_SetReg16(V5_REG_LCDC_GBUF_WIDTH, MMD_LCD_HEIGHT);	
	}
	else
	{
	       gVc0578b_Info.LcdStatus.Size.cx = MMD_LCD_WIDTH;
              gVc0578b_Info.LcdStatus.Size.cy = MMD_LCD_HEIGHT;
		rotateStauts= 0 ;
		VIM_HIF_SetReg16(V5_REG_LCDC_B0_SIZE+2, MMD_LCD_WIDTH-1);
		VIM_HIF_SetReg16(V5_REG_LCDC_B0_SIZE,  MMD_LCD_HEIGHT-1);
		VIM_HIF_SetReg16(V5_REG_LCDC_GBUF_WIDTH, MMD_LCD_WIDTH);	
	}
}



/********************************************************************************
Description:
	Draw a Rectangle to 5x chip's  B Layer memory
Parameters:
	startx: the x coordinate of the Rectangle start point(pixel, start from 0)
	starty: the y coordinate of the Rectangle start point(pixel, start from 0)
	Width: the width of the Rectangle(pixel)
	Height: the height of the Rectangle(pixel)
	uspImage: the point of memory which to fill the Rectangle
	Return:
		V5H_SUCCEED: set ok
		V5H_ERROR_WORKMODE: error work mode
		V5H_FAILED: error parameter
	Note：
		Only write to the 568 internal ram, not update to LCD.
	This function can be used in Camera open mode or Photo process mode 
Remarks:
	valid
*********************************************************************************/
//extern int rotateStauts;
UINT16 VIM_HAPI_DrawLCDRctngl(VIM_HAPI_LAYER BLayer,UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height, HUGE  UINT8 *pwImage)
{
	TSize BSize;
	UINT32 offset = 0,i = 0,index=0;
	TPoint startPoint,point;
	UINT32 ulColorDep = 0;//,tmpx;	//unit is bit.Zhang Xuecheng,2007.10.23

	/* added by Zhang Xuecheng,2007.10.23 */
	switch(gVc0578b_Info.LcdStatus.ColorDep)
	{
		case VIM_HAPI_COLORDEP_12BIT:
			ulColorDep = 12;
			break;
		case VIM_HAPI_COLORDEP_A16BIT:
			ulColorDep = 16;
			break;
		case VIM_HAPI_COLORDEP_16BIT:
			ulColorDep = 16;
			break;
		case VIM_HAPI_COLORDEP_18BIT:
			ulColorDep = 18;
			break;
		case VIM_HAPI_COLORDEP_24BIT:
			ulColorDep = 24;
			break;
		case VIM_HAPI_COLORDEP_A24BIT:
			ulColorDep = 24;
			break;
		default:
			ulColorDep = 16;;
	}
	
	if ((VIM_HAPI_MODE_BYPASS == gVc0578b_Info.ChipWorkMode) || (VIM_HAPI_MODE_POWERDOWN ==gVc0578b_Info.ChipWorkMode))
	{
		return VIM_ERROR_WORKMODE;
	}

	if(BLayer > VIM_DISP_ALLLAYER || BLayer < VIM_DISP_B0LAYER)
		return VIM_ERROR_PARAMETER;

	if ((NULL == pwImage) || (0 ==Width) || (0 == Height))
	{
		return VIM_ERROR_PARAMETER;
	}

	if(gVc0578b_Info.DisplayStatus.outputtype == VIM_OUTPUTTV)
	{
		return VIM_ERROR_WORKMODE;
	}
	
	if(VIM_USER_MPEG4_USEUPDATEDIRECTLY)
	{
		VIM_DISP_GetBSize((VIM_DISP_LAYER)BLayer,&BSize);
		if(BSize.cx==0)
		{
			VIM_HAPI_UpdateLcdDirectly(startx,starty,Width,Height,pwImage);
			return VIM_SUCCEED;
		}
	}

	point.x=startx;
	point.y=starty;
	// Get B Layer star point and size
	////b start point  is always 0,0 VIM_DISP_GetBStartPoint
	if(BLayer & VIM_DISP_B0LAYER)
	{
		VIM_DISP_GetBSize(VIM_DISP_B0LAYER,&BSize);

		// When LCD rotate,change BSize width and height
		VIM_DISP_GetBStartPoint(VIM_DISP_B0LAYER,&startPoint);			//wendy guo 4/12/2007
		if((point.x<startPoint.x)||(point.y<startPoint.y))
			return VIM_ERROR_LCD_DRAWOVERRANGE;

		if(gVc0578b_Info.LcdStatus.BRotationMode%2)
			BSize.cx=BSize.cy;

		offset = (point.y - startPoint.y) * BSize.cx * 2 + (point.x - startPoint.x) * 2;//startx*2;//
		index = point.y  * BSize.cx * 2 + point.x * 2;
		if(BSize.cx/*MMD_GetLCDWidth()*/==Width)
		{
			VIM_HIF_WriteSram(gVc0578b_Info.MarbStatus.MapList.layerB0start + offset,
				(UINT8 *)&(pwImage)[index],Width * 2 *Height, VIM_HIF_SRAM_MODE);
		}
		else
		{
			for(i = starty; i < Height+starty; i++)
			{
				VIM_HIF_WriteSram(gVc0578b_Info.MarbStatus.MapList.layerB0start + offset,
					(UINT8 *)(pwImage + (i * BSize.cx * 2) + startx*2),Width * 2, VIM_HIF_SRAM_MODE);

				offset+=BSize.cx*2;//angela changed it
			}
		}
	}
	if(BLayer & VIM_DISP_B1LAYER)
	{
		VIM_DISP_GetBSize(VIM_DISP_B1LAYER,&BSize);
		// When LCD rotate,change BSize width and height
		VIM_DISP_GetBStartPoint(VIM_DISP_B1LAYER,&startPoint);			//wendy guo 4/12/2007

		if((point.x<startPoint.x)||(point.y<startPoint.y))
			return VIM_ERROR_LCD_DRAWOVERRANGE;

		if(gVc0578b_Info.LcdStatus.BRotationMode%2)
			BSize.cx=BSize.cy;

		offset = (point.y - startPoint.y) * BSize.cx * 2 + (point.x- startPoint.x) * 2;
		index = point.y  * BSize.cx * 2 + point.x * 2;
		if(BSize.cx==Width)
		{
			VIM_HIF_WriteSram(gVc0578b_Info.MarbStatus.MapList.layerB1start + offset,
				(UINT8 *)&(pwImage)[index],Width * 2 *Height, VIM_HIF_SRAM_MODE);
		}
		else
		{
			for(i = starty; i < Height+starty; i++)
			{
				VIM_HIF_WriteSram(gVc0578b_Info.MarbStatus.MapList.layerB1start + offset,
					(UINT8 *)(pwImage + (i * BSize.cx * 2) + startx*2),Width * 2, VIM_HIF_SRAM_MODE);

				offset+=BSize.cx*2;//angela changed it
			}
		}
	}

	return VIM_SUCCEED;
}


/********************************************************************************
Description:
	Update a region from B layer buffer to lcd panel
Parameters:
	startx: the start x coordinate of region(it must be a whole number multiple by 4)
	starty: the start y coordinate of region
	Width: the width of the Rectangle(pixel)
	Height: the height of the Rectangle(pixel)
	Return:
		V5H_SUCCEED: update ok
		V5H_ERROR_WORKMODE: error work mode
		V5H_FAILED: error parameter(update erea overflow)
	Note:
	This function can be used in Camera open mode or Photo process mode 
Remarks:
State: valid
*********************************************************************************/

UINT16 VIM_HAPI_UpdateLCD(VIM_HAPI_LAYER BLayer,UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height)
{

	TPoint refreshPoint;
	TSize size = {0,0}, BSize;
	TPoint startPoint,point,Newpoint;
       unsigned int time_begin,time_end;//,tmpx;

	if ((VIM_HAPI_MODE_BYPASS == gVc0578b_Info.ChipWorkMode) || (VIM_HAPI_MODE_POWERDOWN ==gVc0578b_Info.ChipWorkMode))
	{
		return VIM_ERROR_WORKMODE;
	}
    if(gVc0578b_Info.PreviewStatus.Mode!=VIM_HAPI_PREVIEW_OFF)//zhangxiang add
    {
        
        //ERR("VIM_HAPI_UpdateLCD",0,0,0);
        if(((startx>=gVc0578b_Info.PreviewStatus.Point.x)&&(starty>=gVc0578b_Info.PreviewStatus.Point.y))&&
            ((Width<=gVc0578b_Info.PreviewStatus.Size.cx)&&((Height+starty)<=gVc0578b_Info.PreviewStatus.Size.cy))) 
        {
            
			    //ERR("VIM_HAPI_UpdateLCD111",0,0,0);
                return VIM_SUCCEED;
        }
    }
	if(BLayer > VIM_HAPI_B1_LAYER || BLayer < VIM_HAPI_B0_LAYER)
		return VIM_ERROR_PARAMETER;
	
	if ((0 == Width) || (0 == Height))
	{
		return VIM_ERROR_PARAMETER;
	}


	point.x=startx;
	point.y=starty;
	size.cx=Width;
	size.cy=Height;

	if((VIM_MAPI_FindUpdateRange(size,point)==4)&&(gVc0578b_Info.PreviewStatus.Mode!=VIM_HAPI_PREVIEW_OFF)
		&&(gVc0578b_Info.LcdStatus.WorkMode>=VIM_HAPI_LCDMODE_OVERLAY))
		return VIM_SUCCEED;

	VIM_DISP_GetBSize((VIM_DISP_LAYER)BLayer,&BSize);

	VIM_MAPI_GetNewPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.BRotationMode,&point,
		size,&Newpoint);


	VIM_DISP_GetBStartPoint((VIM_DISP_LAYER)BLayer,&startPoint);			//wendy guo 4/12/2007
	if((Newpoint.y<startPoint.y)||(Newpoint.y>startPoint.y+BSize.cy)||(Newpoint.x<startPoint.x)||(Newpoint.x>startPoint.x+BSize.cx))
	{
		#if((VIM_5XRDK_ERROR)&&(VIM_5XRDK_DEBUG))
			VIM_USER_PrintString("[VC0578B][HAPI]:LCD START POSITION parameter is set error when UPDATE LCD image on Layer B![ERROR]");
		#endif
			
		return VIM_ERROR_LCD_UPDATERANGE;

	}

	if(((Newpoint.y+(Height-point.y))>(startPoint.y+BSize.cy))||((Newpoint.x+(Width-point.x))>(startPoint.x+BSize.cx)))
	{
		#if((VIM_5XRDK_ERROR)&&(VIM_5XRDK_DEBUG))
			VIM_USER_PrintString("[VC0578B][HAPI]:LCD SIZE parameter is set error when draw UPDATE LCD image on Layer B![ERROR]");
		#endif
			
		return VIM_ERROR_LCD_UPDATERANGE;
		
	}	
	//add by lrx 
	if(Newpoint.x>255)
	{
		size.cx += Newpoint.x -254;
		Newpoint.x = 254;
	}
	//add by lrx end
	refreshPoint.x =Newpoint.x-startPoint.x;
	refreshPoint.y =Newpoint.y-startPoint.y;	

	if(gVc0578b_Info.LcdStatus.BRotationMode % 2)
	{
		Width = size.cx;
		size.cx = size.cy;
		size.cy = Width;
	}

//#define VIM_HAPI_B_LAYER_DELAY 400000	
#define VIM_HAPI_B_LAYER_DELAY 400	
       if(BLayer&VIM_HAPI_B0_LAYER)
	{
	time_begin=VIM_TIM2_GetValue();
	   while(!g_b0LayerUpdateFinished)	
	   	{
	   	time_end=VIM_TIM2_GetValue();
		if(((time_end>VIM_HAPI_B_LAYER_DELAY)&&((time_end-VIM_HAPI_B_LAYER_DELAY)>time_begin))||((time_end<time_begin)&&((0xffffffff-time_begin+time_end)>VIM_HAPI_B_LAYER_DELAY)))
			break;
		}
	}
       if(BLayer&VIM_HAPI_B1_LAYER)
	{
	time_begin=VIM_TIM2_GetValue();
	   while(!g_b1LayerUpdateFinished)	
	   	{
	   	time_end=VIM_TIM2_GetValue();
		if(((time_end>VIM_HAPI_B_LAYER_DELAY)&&((time_end-VIM_HAPI_B_LAYER_DELAY)>time_begin))||((time_end<time_begin)&&((0xffffffff-time_begin+time_end)>VIM_HAPI_B_LAYER_DELAY)))
			break;
		}
	}
	//set B refreshed position and size
	if(BLayer & VIM_HAPI_B0_LAYER)
		{
	       g_b0LayerUpdateFinished=0;		
		VIM_DISP_SetB0_RefreshBlock(refreshPoint,size);
		}
	if(BLayer & VIM_HAPI_B1_LAYER)
		{
		g_b1LayerUpdateFinished=0;
		VIM_DISP_SetB1_RefreshBlock(refreshPoint,size);
	}
	VIM_DISP_Update((VIM_DISP_LAYER)BLayer);	//set update control register
	return VIM_SUCCEED;

}

UINT16 VIM_HAPI_UpdateBG(UINT32 color,UINT16 x, UINT16 y,UINT16 width,UINT16 height)
{
	/* the _default_ background color format is rgb24 */
	return VIM_DISP_UpdateBackgroud(VM_LCDC_BGCOLOR_RGB24,color,x,y,width,height);
}
	
/********************************************************************************
* Description:
*	Select main panel or sub panel
* Parameters:
*	byPanel:	Panel Index.
*				VIM_LCDPANEL_MAIN
*				VIM_LCDPANEL_SUB
* Return:
*	VIM_SUCCEED:				Select the LCD panel success.
*	VIM_ERROR_PARAMETER:		byPanel is unknown.
* Note:
*	1. After init 578, call it to power on panel
*	2. When use high api functions, first call it to switch panel to init LCDC and LCDIF.
State: valid
*********************************************************************************/
UINT16 VIM_HAPI_SelectLCDPanel(VIM_HAPI_LCDPANEL byPanel)
{
	if(gVc0578b_Info.LcdStatus.NowPanel == byPanel)
		return VIM_SUCCEED;

	if(byPanel > VIM_HAPI_LCDPANEL_SUB)
		return VIM_ERROR_PARAMETER; 
	
	if ((VIM_HAPI_MODE_BYPASS == gVc0578b_Info.ChipWorkMode) || (VIM_HAPI_MODE_POWERDOWN ==gVc0578b_Info.ChipWorkMode))
	{

		if(VIM_BYPASS_SUBPANEL==VIM_BYPASS_USEREGISTER_SUBPANEL)		//if use write register to select the Main panel or Sub panel 
		{	

			VIM_HIF_SetBypassOrNormal(VIM_HIF_NORMALTYPE);			//only in Normal type it can use write register to select the panel
			
			VIM_HIF_SwitchPanelCs(byPanel);			
			
			VIM_HIF_SetBypassOrNormal(VIM_HIF_BYPASSTYE);				//set back the Bypass type because the original type is Bypass Type
					
		}
		else 															//if use GPIO to select Main panel or Sub panel
		{
			VIM_HIF_SwitchPanelCs(byPanel);
		}
	}
	else
	{
		// Set LCDIF
		VIM_MAPI_SwitchPanel(byPanel);
	}
	gVc0578b_Info.LcdStatus.NowPanel = (VIM_HAPI_LCDPANEL)byPanel;


	return VIM_SUCCEED;
}


/********************************************************************************
* Description:
*	select a window in the lcd panel ,can draw rgb data in b layer 
* Parameters:
*	wStartx:		The left coordinate of the Rectangle(pixel)
*	wStarty:		The up coordinate of the Rectangle(pixel)
*	wWidth:			The width of the Rectangle(pixel)
*	wHeight:		The height of the Rectangle(pixel)
*	wRGB565Color:	The color in RGB565 format.
* Return:
*	VIM_SUCCEED:			Copy pure color data to B layer success.
*	VIM_ERROR_PARAMETER:		Rectangle area is out of B win area
*							or wWidth is 0 or wHeight is 0
*	VIM_ERROR_WORKMODE:		Current mode is V5H_MODE_BYPASS or V5H_MODE_DIRECTDISPLAY
* Note:
*	1. Only write data to B layer, but does not update it.
*	2. The data format in B layer is RGB565.
*	3 if lcd mirror have been changed ,please recall this function to effect the change.
State: valid
*********************************************************************************/
////wendy guo 3/27/2007
//add by lrx 


UINT16 VIM_HAPI_SetLCDSize(VIM_HAPI_LAYER BLayer,UINT16 wStartx,UINT16 wStarty,UINT16 Width,UINT16 Height)
{
	TPoint StartPoint,Newpoint;
	TSize size,resize;
	VIM_RESULT result;
	StartPoint.x = wStartx;
	StartPoint.y = wStarty;
	size.cx = Width;
	size.cy = Height;
	if ((VIM_HAPI_MODE_BYPASS == gVc0578b_Info.ChipWorkMode) || (VIM_HAPI_MODE_POWERDOWN ==gVc0578b_Info.ChipWorkMode))
	{
		return VIM_ERROR_WORKMODE;
	}			

//	if(gVc0578b_Info.PreviewStatus.Mode!=VIM_HAPI_PREVIEW_OFF)
//	{
//		VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	//angela 2006-12-12
//		VIM_MAPI_Delay_Frame(1);
//	}

	if(gVc0578b_Info.LcdStatus.BRotationMode%2)	//wendy guo 4/17/2007, when rotate, the width and height will exchange
	{
		if((Width>gVc0578b_Info.LcdStatus.Size.cx)||(Height>gVc0578b_Info.LcdStatus.Size.cy))	
		{
			#if((VIM_5XRDK_ERROR)&&(VIM_HIGH_API_DEBUG))
				VIM_USER_PrintString("[VC0578B][HAPI]:LCD size is set error![ERROR]");
			#endif
			
			return VIM_ERROR_LCD_SIZE;
		}

		if((wStartx+Width>gVc0578b_Info.LcdStatus.Size.cx)||(wStarty+Height>gVc0578b_Info.LcdStatus.Size.cy))	
		{
			#if((VIM_5XRDK_ERROR)&&VIM_HIGH_API_DEBUG)
				VIM_USER_PrintString("[VC0578B][HAPI]: Layer B LCD start position is set error![ERROR]");
			#endif
					
			return VIM_ERROR_LCD_POSITION;
		}
	

	}

	else
	{
		if((Width>gVc0578b_Info.LcdStatus.Size.cx)||(Height>gVc0578b_Info.LcdStatus.Size.cy))	//wendy guo 4/12/2007
		{
			#if((VIM_5XRDK_ERROR)&&(VIM_HIGH_API_DEBUG))
				VIM_USER_PrintString("[VC0578B][HAPI]:LCD size is set error![ERROR]");
			#endif
			
			return VIM_ERROR_LCD_SIZE;
		}

		if((wStartx+Width>gVc0578b_Info.LcdStatus.Size.cx)||(wStarty+Height>gVc0578b_Info.LcdStatus.Size.cy))	//wendy guo 4/12/2007
		{
			#if((VIM_5XRDK_ERROR)&&(VIM_HIGH_API_DEBUG))
				VIM_USER_PrintString("[VC0578B][HAPI]: Layer B LCD start position is set error![ERROR]");
			#endif
					
			return VIM_ERROR_LCD_POSITION;
		}

	}
	
	switch(BLayer)
	{
		case VIM_HAPI_B0_LAYER:
		{
			VIM_DISP_GetBSize(VIM_DISP_B0LAYER,&resize);
			result=VIM_DISP_SetB0_DisplayPanel(StartPoint,size);
			VIM_MAPI_GetNewPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.BRotationMode,&StartPoint,size,&Newpoint);
			VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS+2,Newpoint.x);
			VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS,Newpoint.y);

		}
		//add by lrx 
		if(result)
		return result;
        //ERR("gb0lcdsize.cx=%d,gb0lcdsize.cy=%d",gb0lcdsize.cx,gb0lcdsize.cy,0);
		if(((gb0lcdsize.cx*gb0lcdsize.cy)<(size.cx*size.cy)))	//zhang xuecheng,2008-1-2 21:39:35
		{
			//gb0lcdsize = size;
			//ERR("11gb0lcdsize.cx=%d,gb0lcdsize.cy=%d",gb0lcdsize.cx,gb0lcdsize.cy,0);
			gb0lcdsize.cx = size.cx;
			gb0lcdsize.cy = size.cy;
			
			result=VIM_MARB_SetLCDMap(	(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
            //ERR("result=%d,",result,0,0);
			
		}
		break;
		case VIM_HAPI_B1_LAYER:
		{
			VIM_DISP_GetBSize(VIM_DISP_B1LAYER,&resize);
			result=VIM_DISP_SetB1_DisplayPanel(StartPoint,size);
			VIM_MAPI_GetNewPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.BRotationMode,&StartPoint,size,&Newpoint);
			VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS+2,Newpoint.x);
			VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS,Newpoint.y);
		}
		//add by lrx 
		if(result)
		return result;
		if(((gb1lcdsize.cx*gb1lcdsize.cy) <(size.cx*size.cy)))	//zhang xuecheng,2008-1-2 21:39:35
		{
			//gb1lcdsize = (size);
			gb1lcdsize.cx = size.cx;
			gb1lcdsize.cy = size.cy;
			result=VIM_MARB_SetLCDMap(	(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
			
		}
		break;
		default:
			return VIM_ERROR_PARAMETER;
	}
	#if 0 //delete by lrx 
	if(result)
		return result;
	if(((resize.cx*resize.cy) <(size.cx*size.cy)))	//zhang xuecheng,2008-1-2 21:39:35
		result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
			(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
	#endif
	if(result)
	{
		#if((VIM_5XRDK_ERROR)&&(VIM_HIGH_API_DEBUG))
			VIM_USER_PrintString("[VC0578B][HAPI]: Layer map is distributed error![ERROR]");
		#endif
				
		return VIM_ERROR_MARB_MAP_DISTRIBUTE;
	}	
	
	//if(gVc0578b_Info.PreviewStatus.Mode!=VIM_HAPI_PREVIEW_OFF)
	//{
	//	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);	//wendy guo 3/27/2007 angela 2006-12-12
//	}
	return result;
}

UINT16 VIM_HAPI_GetLCDSize(VIM_HAPI_LAYER layer,UINT16 *width,UINT16 *height)
{
	UINT32	regval = 0;
	
	switch(layer)
	{
	case VIM_HAPI_A_LAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_A0_SIZE_H);
		break;
	case VIM_HAPI_B0_LAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_B0_SIZE);
		break;
	case VIM_HAPI_B1_LAYER:
		regval = VIM_HIF_GetReg32(V5_REG_LCDC_B1_SIZE);
	default:
		return VM_ERR_LCDC_LAYER_UNKNOWN;
	}
	*height = (UINT16)((regval&0x3ff)+1);
	*width = (UINT16)(((regval>>16)&0x3ff)+1);
	
	return VIM_SUCCEED;
}

/********************************************************************************
* Description:
*	Copy pure color into a Rectangle area on LCD( B Layer)
* Parameters:
*	wStartx:		The left coordinate of the Rectangle(pixel)
*	wStarty:		The up coordinate of the Rectangle(pixel)
*	wWidth:		The width of the Rectangle(pixel)
*	wHeight:		The height of the Rectangle(pixel)
*	wRGB565Color:	The color in RGB565 format.
* Return:
*	VIM_SUCCEED:			Copy pure color data to B layer success.
*	VIM_ERROR_PARAMETER:		Rectangle area is out of B win area
*							or wWidth is 0 or wHeight is 0
*	VIM_ERROR_WORKMODE:		Current mode is V5H_MODE_BYPASS or V5H_MODE_DIRECTDISPLAY
* Note:
*	1. Only write data to B layer, but does not update it.
*	2. The data format in B layer is RGB565.
State: valid
*********************************************************************************/
UINT16 VIM_HAPI_DrawLCDPureColor(VIM_HAPI_LAYER BLayer,UINT16 startx, UINT16 starty,UINT16 Width,UINT16 Height,UINT16 wRGBColor)
{
	TSize BSize;
	UINT32 offset,i;
	TPoint startPoint;
	UINT16 dwColor = 0;
	if ((VIM_HAPI_MODE_BYPASS == gVc0578b_Info.ChipWorkMode) || (VIM_HAPI_MODE_POWERDOWN ==gVc0578b_Info.ChipWorkMode))
	{
		return VIM_ERROR_WORKMODE;
	}

	if(BLayer > VIM_DISP_ALLLAYER || BLayer < VIM_DISP_B0LAYER)
		return VIM_ERROR_PARAMETER;

	if ( (0 ==Width) || (0 == Height))
	{
		return VIM_ERROR_PARAMETER;
	}

	if(gVc0578b_Info.DisplayStatus.outputtype == VIM_OUTPUTTV)
	{
		return VIM_ERROR_WORKMODE;
	}
	
	//dwColor = (((UINT32)wRGBColor) << 16) | wRGBColor;
	dwColor = wRGBColor;
	// Get B Layer star point and size
	////b start point  is always 0,0 VIM_DISP_GetBStartPoint
	if(BLayer & VIM_DISP_B0LAYER)
	{
		VIM_DISP_GetBSize(VIM_DISP_B0LAYER,&BSize);
		// When LCD rotate,change BSize width and height
		VIM_DISP_GetBStartPoint(VIM_DISP_B0LAYER,&startPoint);			//wendy guo 4/12/2007
		if((starty<startPoint.y)||(starty>startPoint.y+BSize.cy)||(startx<startPoint.x)||(startx>startPoint.x+BSize.cx))
		{
			#if((VIM_5XRDK_ERROR)&&(VIM_HIGH_API_DEBUG))
				VIM_USER_PrintString("[VC0578B][HAPI]:LCD START POSITION parameter is set error when draw LCD pure color on Layer B0![ERROR]");
			#endif
			
			return VIM_ERROR_LCD_POSITION;

		}

		if(((starty+Height)>(startPoint.y+BSize.cy))||((startx+Width)>(startPoint.x+BSize.cx)))
		{
			#if((VIM_5XRDK_ERROR)&&(VIM_HIGH_API_DEBUG))
				VIM_USER_PrintString("[VC0578B][HAPI]:LCD SIZE parameter is set error when draw LCD pure color on Layer B0![ERROR]");
			#endif
			
			return VIM_ERROR_LCD_SIZE;
		
		}

		if(gVc0578b_Info.LcdStatus.BRotationMode%2)
			BSize.cx=BSize.cy;

			
		offset = (starty - startPoint.y) * BSize.cx * 2 + (startx- startPoint.x) * 2;
        FARF(MSG,("offset=%d,%d",offset, gVc0578b_Info.MarbStatus.MapList.layerB0start));
		if(BSize.cx==Width)
		{
			VIM_HIF_WriteSramOnWord(gVc0578b_Info.MarbStatus.MapList.layerB0start + offset,
				dwColor,Width * 2 *Height);
		}
		else
		{
			for(i = 0; i < Height; i++)
			{
				VIM_HIF_WriteSramOnWord(gVc0578b_Info.MarbStatus.MapList.layerB0start + offset,
					dwColor,Width * 2);
				offset+=BSize.cx*2;//angela changed it
			}
		}
	}
	if(BLayer & VIM_DISP_B1LAYER)
	{
		VIM_DISP_GetBSize(VIM_DISP_B1LAYER,&BSize);


		VIM_DISP_GetBStartPoint(VIM_DISP_B1LAYER,&startPoint);			//wendy guo 4/12/2007
		if((starty<startPoint.y)||(starty>startPoint.y+BSize.cy)||(startx<startPoint.x)||(startx>startPoint.x+BSize.cx))
		{
			#if((VIM_5XRDK_ERROR)&&(VIM_HIGH_API_DEBUG))
				VIM_USER_PrintString("[VC0578B][HAPI]:LCD START POSITION parameter is set error when draw LCD pure color on Layer B1![ERROR]");
			#endif
			
			return VIM_ERROR_LCD_POSITION;

		}

		if(((starty+Height)>(startPoint.y+BSize.cy))||((startx+Width)>(startPoint.x+BSize.cx)))
		{
			#if((VIM_5XRDK_ERROR)&&(VIM_HIGH_API_DEBUG))
				VIM_USER_PrintString("[VC0578B][HAPI]:LCD SIZE parameter is set error when draw LCD pure color on Layer B1![ERROR]");
			#endif
			
			return VIM_ERROR_LCD_SIZE;
		
		}


		if(gVc0578b_Info.LcdStatus.BRotationMode%2)
			BSize.cx=BSize.cy;

			
		offset = (starty - startPoint.y) * BSize.cx * 2 + (startx- startPoint.x) * 2;
		if(BSize.cx==Width)
		{
			VIM_HIF_WriteSramOnWord(gVc0578b_Info.MarbStatus.MapList.layerB1start + offset,
				dwColor,Width * 2 *Height);
		}
		else
		{
			for(i = 0; i < Height; i++)
			{
				VIM_HIF_WriteSramOnWord(gVc0578b_Info.MarbStatus.MapList.layerB1start + offset ,
					dwColor,(Width * 2));
				offset+=BSize.cx*2;//angela changed it
			}
		}
	}
	return VIM_SUCCEED;
}


/********************************************************************************
* Description:
*	Set B Layer color depth
* Parameters:
*	byColorDepth:	
		VIM_HAPI_COLORDEP_16BIT=6,
		VIM_HAPI_COLORDEP_15BIT=5
		VIM_HAPI_COLORDEP_12BIT
 * Return:
*	VIM_SUCCEED:				Set color depth success.
*	VIM_ERROR_PARAMETER:		byColorDepth is unknown.
*	VIM_ERROR_WORKMODE:		Current mode is V5H_MODE_BYPASS or V5H_MODE_DIRECTDISPLAY
* Note: 
*	 
State: valid
*********************************************************************************/
UINT16 VIM_HAPI_SetLCDColordep(VIM_HAPI_COLORDEP_MODE byColorDepth)
{
	if ((VIM_HAPI_MODE_BYPASS == gVc0578b_Info.ChipWorkMode) || (VIM_HAPI_MODE_POWERDOWN ==gVc0578b_Info.ChipWorkMode))
	{
		return VIM_ERROR_WORKMODE;
	}
	if(byColorDepth>VIM_HAPI_COLORDEP_A24BIT||byColorDepth<VIM_HAPI_COLORDEP_12BIT)
		return VIM_ERROR_PARAMETER;
	
	gVc0578b_Info.LcdStatus.ColorDep=byColorDepth;
	VIM_DISP_SetGbufFormat((VIM_DISP_BFORMAT)byColorDepth);
	return VIM_SUCCEED;
}

/********************************************************************************
* Description:
*	Mirror or flip the display image on LCD, includes A layer and B layer.
*	Note: A layer display by hardware, but B layer display by software!
*
* Parameters:
*	byMirrorFlipMode:
	Rotate_Degree_0,
	Rotate_Degree_90,
	Rotate_Degree_180,
	Rotate_Degree_270,
	Rotate_Mirror_0,
	Rotate_Mirror_90,
	Rotate_Mirror_180,
	Rotate_Mirror_270,

*	byABLayer:
	VIM_HAPI_B0_Layer = 1,
	VIM_HAPI_B1_Layer,
	VIM_HAPI_ALL_BLayer,
	VIM_HAPI_A_Layer,
	VIM_HAPI_All_Layer
* Return:
*	VIM_SUCCEED:			Mirror or flip success.
*	VIM_ERROR_PARAMETER:		byMirrorFlipMode is unknown  
*							or byABLayer is unknown  
*	VIM_ERROR_WORKMODE:		Current mode is V5H_MODE_BYPASS or V5H_MODE_DIRECTDISPLAY
* Note:
*	1. Only write data to B layer, but does not update it.
*	2. The data format in B layer is RGB565.
State: valid
*********************************************************************************/
UINT16 VIM_HAPI_SetLCDMirror(VIM_HAPI_ROTATEMODE MirrorFlipMode, VIM_HAPI_LAYER Layer)
{
	if ((VIM_HAPI_MODE_BYPASS == gVc0578b_Info.ChipWorkMode) || (VIM_HAPI_MODE_POWERDOWN ==gVc0578b_Info.ChipWorkMode))
	{
		return VIM_ERROR_WORKMODE;
	}

	if(MirrorFlipMode > VIM_DISP_MIRROR_270)
		return VIM_ERROR_PARAMETER;		

	switch(Layer)
	{
	case VIM_HAPI_ALLB_LAYER:	
		//gVc0578b_Info.LcdStatus.BRotationMode=(VIM_DISP_ROTATEMODE)MirrorFlipMode;


		
		break;
	case VIM_HAPI_A_LAYER:
		gVc0578b_Info.LcdStatus.ARotationMode =(VIM_DISP_ROTATEMODE)MirrorFlipMode;
		break;
	case VIM_HAPI_ALL_LAYER:
		gVc0578b_Info.LcdStatus.BRotationMode=(VIM_DISP_ROTATEMODE)MirrorFlipMode;
		gVc0578b_Info.LcdStatus.ARotationMode =(VIM_DISP_ROTATEMODE)MirrorFlipMode;
		break;
	default:
		return VIM_ERROR_PARAMETER;
	}

	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	Set special effect
Parameters:
	Mode:
		 VIM_HAPI_SPECIAL_NORMAL.
		 VIM_HAPI_SPECIAL_MONOCHROME,	 //黑白
		 VIM_HAPI_SPECIAL_SEPIA,		//怀旧
		 VIM_HAPI_SPECIAL_NEGATIVE,	//负片
		 VIM_HAPI_SPECIAL_REDONLY,	 //红色突出
	Y = ( 77R + 150G + 29B)/256
	U = (-43R - 85G + 128B)/256 + 128
	V = (128R - 107G - 21B)/256 + 128
	Return:
		VIM_SUCCEED: set effect ok
		VIM_ERROR_WORKMODE: error work mode
	Note:
		This function must be used in Camera open mode
Remarks:
	state: 	valid
*********************************************************************************/
UINT16 VIM_HAPI_SetEffect(VIM_HAPI_SPECIAL_EFFECT Mode)
{
	VIM_IPP_EFFECT Effect;
	if(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS)
		return VIM_ERROR_WORKMODE;
	Effect=(VIM_IPP_EFFECT)Mode;
	switch(Mode)
		{
		case  VIM_HAPI_SPECIAL_MONOCHROME: 
			Effect=VIM_IPP_EFFECT_SPECIALCOLOR;
			VIM_IPP_SetEffectUVThreshold(0,0,0,0);
			break;
		case  VIM_HAPI_SPECIAL_REDONLY:
         //modify by xinggan.xu 20090112 start 					
         Effect=VIM_IPP_EFFECT_SEPHIA;         
         VIM_IPP_SetEffectUVoffset(100,200);         
			break;
        //add by xinggang.xu 20081217 start
      case  VIM_HAPI_SPECIAL_GREENONLY: 						
         Effect=VIM_IPP_EFFECT_SEPHIA;      //modify by xinggang.xu 20090112    
         VIM_IPP_SetEffectUVoffset(60,60);  //modify by xinggang.xu 20090112 
			break;        
		case  VIM_HAPI_SPECIAL_SEPIA:
			VIM_IPP_SetEffectUVoffset(85,128);
			break;
		default:
			break;
		}
	VIM_IPP_SetEffect(Effect);
	gVc0578b_Info.PreviewStatus.SpecialEffect=(VIM_HAPI_SPECIAL_EFFECT)Mode;
	return VIM_SUCCEED;
}

void VIM_HAPI_LCDCBuffer_Release(void)
{
	VIM_MAPI_FreeLCDCBuffer();
	gb0lcdsize.cx=0;
	gb0lcdsize.cy=0;
	gb1lcdsize.cx=0;
	gb1lcdsize.cy=0;
}

void VIM_HAPI_LCDCBuffer_Cast(void)
{
	gb0lcdsize.cx=0;
	gb0lcdsize.cy=0;
	gb1lcdsize.cx=0;
	gb1lcdsize.cy=0;
	gVc0578b_Info.MarbStatus.MapList.layerB0start=0;
	gVc0578b_Info.MarbStatus.MapList.layerB1start=0;
}

#endif


/********************************************************************************
Description:
	interrupt service routine
Parameters:

Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_WORKMODE: error work mode
Note:
	This function should be called by the user's CPU's external interrupt ISR, or a 50ms timer when 
	start preview or start capture.
		
Remarks:
state: 
	valid
*********************************************************************************/
#if 0

void VIM_HAPI_Timer(void)
{
	UINT32 uWaitlen=VIM_WAITMAX;

		
	if(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS ||gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_POWERDOWN)
       		return ;
	//interrupt protect
	VIM_HIF_SetReg32(V5_REG_BIU_BREAK_OCCUR, 0x01);



#if (VIM_EXTERN_INTERRUPT==0)

	 	_ISR_HIF_IntHandle();
#endif

	 if(gVc0578b_Info.aeaf.Baeafen)
	 {
#if VIM_SENSOR_AUTO_FOCUS
		VIM_MAPI_DoAeAF();
#endif
	 }


	 //interrupt protect
	while(uWaitlen--){
		if(VIM_HIF_GetReg32(V5_REG_BIU_BREAK_FINISH))
			break;
	}
	if(!uWaitlen)
	{
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][HAPI] interrupt break off over time [ERROR] ");
#endif	

	}
	 return ;
}
#endif


#if	JPEGCAP

/********************************************************************************
			JPEG capture related
*******************************************************************************/

/********************************************************************************
Description:
	Set Capture parameter(still, multishot, video)
Parameters:
	CapWidth:  the width of captured image.
	CapHeight: the height of captured image.
	ThumWidth: Thumbnail image width(when capture with thumbnail) , and it's heith is decided by
	     CapWidth: CapHeight(same ration).ThumWidth should no larger than CapWidth.
	     The real width and height of thumbnail can get by V5H_Capture_GetThumbRealSize().
	Return:
		V5H_SUCCEED: set ok
		V5H_ERROR_WORKMODE: error work mode
	Note:
		This function must be used before first preview or change the resolution of display.
		This function must be used in Camera open mode
	Remarks:
	State:Valid.
*********************************************************************************/
UINT16 VIM_HAPI_SetCaptureParameter(UINT16 CapWidth, UINT16 CapHeight)
{

	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;
	gVc0578b_Info.CaptureStatus.Size.cx=(CapWidth>>2)<<2;
	gVc0578b_Info.CaptureStatus.Size.cy=(CapHeight>>1)<<1;
	gVc0578b_Info.CaptureStatus.ReCapSize=0;
	VIM_MPEG4_SetVolSize(gVc0578b_Info.CaptureStatus.Size.cx,gVc0578b_Info.CaptureStatus.Size.cy);
	//if(CapWidth%16||CapHeight%8)		//angela 2007-3-12
		//return VIM_ERROR_CAPTURE_PARAMETER;
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	Set thumbnail size(still, multishot, video)
Parameters:
	Width:  the width of thumbnail.
	Height: the height of thumbnail.	
Return:
	V5H_SUCCEED: set ok
	V5H_ERROR_WORKMODE: error work mode
Note:
	This function must be used before first preview or change the resolution of display.
	This function must be used in Camera open mode
Remarks:
	State:Valid.
*********************************************************************************/
UINT16 VIM_HAPI_SetThumbnailSize(UINT16 Width, UINT16 Height)
{

	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;
	gVc0578b_Info.CaptureStatus.ThumbSize.cx=Width;
	gVc0578b_Info.CaptureStatus.ThumbSize.cy=Height;
	if(gVc0578b_Info.CaptureStatus.ThumbSize.cx>254)
		gVc0578b_Info.CaptureStatus.ThumbSize.cx=254;
	if(gVc0578b_Info.CaptureStatus.ThumbSize.cy>254)
		gVc0578b_Info.CaptureStatus.ThumbSize.cy=254;
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	Set Capture parameter(still, multishot, video)
Parameters:
	CapWidth:  the width of captured image.
	CapHeight: the height of captured image.
	ThumWidth: Thumbnail image width(when capture with thumbnail) , and it's heith is decided by
	     CapWidth: CapHeight(same ration).ThumWidth should no larger than CapWidth.
	     The real width and height of thumbnail can get by V5H_Capture_GetThumbRealSize().
	Return:
		V5H_SUCCEED: set ok
		V5H_ERROR_WORKMODE: error work mode
	Note:
		This function must be used before first preview or change the resolution of display.
		This function must be used in Camera open mode
	Remarks:
	State:Valid.
*********************************************************************************/
UINT16 VIM_HAPI_SetCaptureParameterChangeFlag()
{
	gVc0578b_Info.CaptureStatus.SizeChange=1;
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	set preview parameters
Parameters:
	Width:  the width of preview area on lcd.
	height: the height of preview area on lcd.
	Lcd_OffsetW: offset x of start point on lcd.
	Lcd_OffsetH: offset y of start point  on lcd.
	Return:
		VIM_ERROR_WORKMODE:  work error mode
		VIM_SUCCEED:  set parameter ok
		VIM_ERROR_PREVIEW_PARAMETER:  error parameter
Note:
	Preview parameters should be set before start preview, 
	and these parameters only can come to valid when start preview 
Remarks:
	state: Valid
*********************************************************************************/
UINT16 VIM_HAPI_SetPreviewParameter(UINT16 Lcd_OffsetW,UINT16 Lcd_OffsetH,UINT16 Width, UINT16 Height)
{
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;
	gVc0578b_Info.PreviewStatus.Size.cx=Width&0xfffe;
	gVc0578b_Info.PreviewStatus.Size.cy=Height;
	gVc0578b_Info.PreviewStatus.Point.x=Lcd_OffsetW&0xfffc;
	gVc0578b_Info.PreviewStatus.Point.y=Lcd_OffsetH;
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	SET preview MODE
Parameters:
	MODE: preview mode
		VIM_PREVIEW_OFF: preview off
		VIM_HAPI_PREVIEW_ON: preview on without frame
		VIM_HAPI_PREVIEW_FRAMEON: preview on with frame
	bIndex: is the index of resolution (see the sensorinfo.c)
	Return:
		VIM_SUCCEED: set mode ok
		VIM_ERROR_WORKMODE: error work mode
		VIM_ERROR_????????: Return from middle API function
	Note:
		This function must be used in Camera open mode
	Remarks:
		state: valid
*********************************************************************************/
UINT16 VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_MODE MODE,VIM_HAPI_RESOLUTION bIndex)
{
VIM_RESULT result;
TSize Amem,InputSize,CaptureSize,SourceWin,ThumbnailSize;
TPoint pt;
//UINT32 sensor_target;
//UINT32 lcd_target=24;
/*UINT32 CDKLS;
UINT8 CON_L,CON_H;
UINT16 ML,MH;
UINT32 PLL_CLK = 432/2;//432M have predivider*/
	
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;

       switch(MODE)
	{
		case VIM_HAPI_PREVIEW_ON:
		case VIM_HAPI_PREVIEW_FRAMEON:		
			VIM_SIF_EnableSyncGen(DISABLE);
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
			gVc0578b_Info.CaptureStatus.SizeChange=0;
			gVc0578b_Info.CaptureStatus.DownSensorClock=0;
			gVc0578b_Info.LcdStatus.ARotationMode=(VIM_DISP_ROTATEMODE)0;
			ThumbnailSize.cx=gVc0578b_Info.CaptureStatus.ThumbSize.cx;
			ThumbnailSize.cy=gVc0578b_Info.CaptureStatus.ThumbSize.cy;

			if(MODE!=VIM_HAPI_PREVIEW_FRAMEON)
			{
		 		VIM_MAPI_SetChipMode(VIM_MARB_PREVIEW_MODE,VIM_IPP_HAVE_NOFRAME);	//actually capture mode
			}
			else
			{
				VIM_MAPI_SetChipMode(VIM_MARB_PREVIEW_MODE,VIM_IPP_HAVEFRAME);
			}
         FARF(MSG,("VIM_HAPI_SetPreviewMode"));
			gVc0578b_Info.PreviewStatus.bOutputIndex=bIndex;
            		result = VIM_SIF_SetSensorResolution(gVc0578b_Info.pSensorInfo,&gVc0578b_Info.VIM_SIF_AeAdjust,(VIM_SIF_RESOLUTION)bIndex,VIM_SIF_SNSOR_FRAMERATE_FORCAPIMAGE);//*diferrence
			if(result)
		       return (UINT16)result;
			//get input size
			result = VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, (VIM_SIF_RESOLUTION)bIndex, &InputSize);
			if(result)
		           return (UINT16)result;

			VIM_IPP_SetImageSize(InputSize.cx, InputSize.cy);
			CaptureSize.cx=gVc0578b_Info.CaptureStatus.Size.cx;
			CaptureSize.cy=gVc0578b_Info.CaptureStatus.Size.cy;

			//change capture size
			if((CaptureSize.cx>InputSize.cx)||(CaptureSize.cy>InputSize.cy))		//* difference, added
			{
				if(VIM_USER_FAST_PREVIEW)
				{
					CaptureSize.cx/=2;
					CaptureSize.cy/=2;
					if((CaptureSize.cx>InputSize.cx)||(CaptureSize.cy>InputSize.cy))		//* difference, added
					{
						CaptureSize.cx/=2;
						CaptureSize.cy/=2;
					}
				}
			}
			//get source window size
#if 1
			result = VIM_IPP_ToolCaculateLessSrcWindow(InputSize,CaptureSize,&SourceWin);
			if(result)
				return (UINT16)result;
#else
			SourceWin.cx = InputSize.cx;
			SourceWin.cy = InputSize.cy;
#endif
			//get source window start position	
			pt.x=((InputSize.cx-SourceWin.cx)/2)&0xfffe;
			pt.y=((InputSize.cy-SourceWin.cy)/2)&0xfffe;
			result = VIM_IPP_SetCaptureSize(gVc0578b_Info.pSensorInfo,pt, SourceWin, CaptureSize); 
			if(result)
				return (UINT16)result;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("[VC0578B][HAPI] sensor output width: ",InputSize.cx);
			VIM_USER_PrintDec("[VC0578B][HAPI] sensor output height: ",InputSize.cy);
			VIM_USER_PrintDec("[VC0578B][HAPI] Source window width: ",SourceWin.cx);
			VIM_USER_PrintDec("[VC0578B][HAPI] Source window height: ",SourceWin.cy);
			VIM_USER_PrintDec("[VC0578B][HAPI] Capture window width:  ",CaptureSize.cx);
			VIM_USER_PrintDec("[VC0578B][HAPI]Capture window height:  ",CaptureSize.cy);
#endif	
			//get part size of source win will display in LCD
			result = VIM_IPP_ToolCaculateBigDstWindow(SourceWin, gVc0578b_Info.PreviewStatus.Size,&InputSize);
			if(result)
				return (UINT16)result;
			
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("[VC0578B][HAPI]  display InputSize window width:",InputSize.cx);
			VIM_USER_PrintDec("[VC0578B][HAPI]  display InputSize window height:  ",InputSize.cy);
#endif	

			if(InputSize.cx<gVc0578b_Info.PreviewStatus.Size.cx)	//angela 2007-3-7		//*difference, added
				InputSize.cx=gVc0578b_Info.PreviewStatus.Size.cx;
			if(InputSize.cy<gVc0578b_Info.PreviewStatus.Size.cy)
				InputSize.cy=gVc0578b_Info.PreviewStatus.Size.cy;			
			
			result = VIM_IPP_SetDispalySize( pt, SourceWin, InputSize);
	 		if(result)
				return (UINT16)result;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("[VC0578B][HAPI] new display InputSize window width:",InputSize.cx);
			VIM_USER_PrintDec("[VC0578B][HAPI] new display InputSize window height:  ",InputSize.cy);
			VIM_USER_PrintDec("[VC0578B][HAPI]preivew  window width:",gVc0578b_Info.PreviewStatus.Size.cx);
			VIM_USER_PrintDec("[VC0578B][HAPI]preivew window height:  ",gVc0578b_Info.PreviewStatus.Size.cy);			
#endif	
			
			if(ThumbnailSize.cx*ThumbnailSize.cy!=0)
			{
				result = VIM_IPP_SetThumbSize((UINT8)ThumbnailSize.cx, (UINT8)ThumbnailSize.cy,VIM_IPP_THUMB_FROM_SPECIAL);
				if(result)
					return (UINT16)result;
			}			
			gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize.cx=SourceWin.cx;
			gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize.cy=SourceWin.cy;
	 		gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cx= InputSize.cx;
			gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cy= InputSize.cy;
	 		pt.x=(((InputSize.cx-gVc0578b_Info.PreviewStatus.Size.cx)>>1)>>1)<<1;
			pt.y=(InputSize.cy-gVc0578b_Info.PreviewStatus.Size.cy)>>1;
			
			VIM_DISP_SetA_Memory(pt,InputSize);
			VIM_DISP_SetA_DisplayPanel(gVc0578b_Info.PreviewStatus.Point,gVc0578b_Info.PreviewStatus.Size);

			result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
					VIM_DISP_NOTCHANGE);			//wendy guo 4/18/2007, add this function to test A0 layer rotation!
			
			if(result)    
				return (UINT16)result;
			VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
			VIM_JPEG_SetSize(JPEG_422,CaptureSize);
					
			// angela added it for set capture ratio before preview on
			result=VIM_MAPI_SetCaptureRatio(gVc0578b_Info.CaptureStatus.QualityMode);

			if(result)
				return (UINT16)result;
			if(gVc0578b_Info.MarbStatus.ALayerMode != VIM_DISP_TVOUT_ONEFRAME &&gVc0578b_Info.MarbStatus.ALayerMode!= VIM_DISP_TVOUT_TWOFRAME)
				gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_PREVIEW_ALAYER_MODE;//ALAYER_1FRAMEBUF;
CHAGNEAMODE:
			result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
		                  (PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
			if(result)
			{
				if(gVc0578b_Info.MarbStatus.ALayerMode == VIM_DISP_LINEBUF)
					return (UINT16)result;
				else if(gVc0578b_Info.MarbStatus.ALayerMode == VIM_DISP_TWOFRAME)
					{
						gVc0578b_Info.MarbStatus.ALayerMode =VIM_DISP_ONEFRAME;
						goto CHAGNEAMODE;
					}
				else
					{
						gVc0578b_Info.MarbStatus.ALayerMode =VIM_DISP_LINEBUF;
						goto CHAGNEAMODE;
					}
			}
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("[VC0578B][HAPI] user a layter mode :",gVc0578b_Info.MarbStatus.ALayerMode);
#endif	
			////////////zoom ///////////////////
			if ( gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize.cx >= 
				(CaptureSize.cx/2 + VIM_USER_MIN_PIXEL_ONESTEP * VIM_USER_MAX_STEP))
			{
				result=VIM_MAPI_Preview_CaptureZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
			}
			else
			{
				result=VIM_MAPI_Preview_DisplayZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
			}
			gVc0578b_Info.CaptureStatus.ErrorImageRecaptureTimes=0;
			gVc0578b_Info.CaptureStatus.RecaptureTimes=0;

			VIM_JPEG_SetBitRateControlEn(ENABLE);//angela   
			VIM_IPP_SetDisplayDropFrame(0xffff);
			VIM_SIF_EnableSyncGen(ENABLE);
            
			// VIM_MAPI_Delay_Frame(1);   //* difference from 528 code: _Frame(2) //Delay for the stable sensor data output

			VIM_DISP_ResetState();
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);

			if(!gVc0578b_Info.pSensorInfo->snrtype)	//rgb type
			{
#if VIM_SENSOR_HAVE_BAYER
#if VIM_SENSOR_AUTO_FOCUS
				VIM_USER_SetAutoFocusStep(gVc0578b_Info.aeaf.rightstep);
#endif
				VIM_MAPI_AEIsrInit();
#endif
			}
			break;

		case VIM_HAPI_MP4_PREVIEW_ON:
			VIM_SIF_EnableSyncGen(DISABLE);
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
			//set chip mode
			gVc0578b_Info.LcdStatus.ARotationMode=(VIM_DISP_ROTATEMODE)0;
	 		VIM_MAPI_SetChipMode(VIM_MARB_CAPTUREMPG4_MODE,VIM_IPP_HAVE_NOFRAME);	//actually capture mode
			// set sensor
			gVc0578b_Info.PreviewStatus.bOutputIndex=bIndex;
            result = VIM_SIF_SetSensorResolution(gVc0578b_Info.pSensorInfo,&gVc0578b_Info.VIM_SIF_AeAdjust,(VIM_SIF_RESOLUTION)bIndex,VIM_SIF_SNSOR_FRAMERATE_FORCAPVIDEO);//*diferrence

			if(result)
		    	return (UINT16)result;
			//get input size
			result = VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, (VIM_SIF_RESOLUTION)bIndex, &InputSize);
			if(result)
		           return (UINT16)result;
			VIM_IPP_SetImageSize(InputSize.cx, InputSize.cy);
			CaptureSize=gVc0578b_Info.CaptureStatus.Size;
			//change capture size
			if((CaptureSize.cx>InputSize.cx)||(CaptureSize.cy>InputSize.cy))		//* difference, added
			{
				if(VIM_USER_FAST_PREVIEW)
				{
					CaptureSize.cx/=2;
					CaptureSize.cy/=2;
					if((CaptureSize.cx>InputSize.cx)||(CaptureSize.cy>InputSize.cy))		//* difference, added
					{
						CaptureSize.cx/=2;
						CaptureSize.cy/=2;
					}
				}
			}
			//get source window size
			result = VIM_IPP_ToolCaculateLessSrcWindow(InputSize,CaptureSize,&SourceWin);
			if(result)
				return (UINT16)result;
			//get source window start position	
			pt.x=((InputSize.cx-SourceWin.cx)/2)&0xfffe;
			pt.y=((InputSize.cy-SourceWin.cy)/2)&0xfffe;
			result = VIM_IPP_SetCaptureSize(gVc0578b_Info.pSensorInfo,pt, SourceWin, CaptureSize); 
			if(result)
				return (UINT16)result;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
				VIM_USER_PrintDec("[VC0578B][HAPI] sensor output width: ",InputSize.cx);
				VIM_USER_PrintDec("[VC0578B][HAPI] sensor output height: ",InputSize.cy);
				VIM_USER_PrintDec("[VC0578B][HAPI] Source window width: ",SourceWin.cx);
				VIM_USER_PrintDec("[VC0578B][HAPI] Source window height: ",SourceWin.cy);
				VIM_USER_PrintDec("[VC0578B][HAPI] Capture window width:  ",CaptureSize.cx);
				VIM_USER_PrintDec("[VC0578B][HAPI]Capture window height:  ",CaptureSize.cy);
#endif	
			//get part size of source win will display in LCD
			
			result = VIM_IPP_ToolCaculateBigDstWindow(SourceWin, gVc0578b_Info.PreviewStatus.Size,&InputSize);
			if(result)
				return (UINT16)result;
			
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("[VC0578B][HAPI]  display InputSize window width:",InputSize.cx);
			VIM_USER_PrintDec("[VC0578B][HAPI]  display InputSize window height:  ",InputSize.cy);
#endif	

			if(InputSize.cx<gVc0578b_Info.PreviewStatus.Size.cx)	//angela 2007-3-7		//*difference, added
				InputSize.cx=gVc0578b_Info.PreviewStatus.Size.cx;
			if(InputSize.cy<gVc0578b_Info.PreviewStatus.Size.cy)
				InputSize.cy=gVc0578b_Info.PreviewStatus.Size.cy;			
			
			result = VIM_IPP_SetDispalySize( pt, SourceWin, InputSize);
	 		if(result)
				return (UINT16)result;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("[VC0578B][HAPI] new display InputSize window width:",InputSize.cx);
			VIM_USER_PrintDec("[VC0578B][HAPI] new display InputSize window height:  ",InputSize.cy);
			VIM_USER_PrintDec("[VC0578B][HAPI]preivew  window width:",gVc0578b_Info.PreviewStatus.Size.cx);
			VIM_USER_PrintDec("[VC0578B][HAPI]preivew window height:  ",gVc0578b_Info.PreviewStatus.Size.cy);			
#endif	
	 		gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize=SourceWin;
	 		gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize = InputSize;
	 		pt.x=(((InputSize.cx-gVc0578b_Info.PreviewStatus.Size.cx)>>1)>>1)<<1;
			pt.y=(InputSize.cy-gVc0578b_Info.PreviewStatus.Size.cy)>>1;
			
			VIM_DISP_SetA_Memory(pt,InputSize);
			VIM_DISP_SetA_DisplayPanel(gVc0578b_Info.PreviewStatus.Point,gVc0578b_Info.PreviewStatus.Size);

			result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
					VIM_DISP_NOTCHANGE);			//wendy guo 4/18/2007, add this function to test A0 layer rotation!
			
			if(result)
				return (UINT16)result;
			VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
#if 1
			//VIM_JPEG_SetSize(JPEG_422,CaptureSize);
			VIM_HIF_SetReg8(V5_REG_LBUF_MODE,(UINT8)JPEG_420);
			//Set line buf image width
			VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE,(UINT16)CaptureSize.cx);
			VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE+2,(UINT16)(CaptureSize.cy));
#endif
			if(gVc0578b_Info.MarbStatus.ALayerMode != VIM_DISP_TVOUT_ONEFRAME &&gVc0578b_Info.MarbStatus.ALayerMode!= VIM_DISP_TVOUT_TWOFRAME)
				gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_MP4_PREVIEW_ALAYER_MODE;//ALAYER_1FRAMEBUF;

CHAGNEAMODE1:
			result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
		                  (PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
			if(result)
			{
				if(gVc0578b_Info.MarbStatus.ALayerMode == VIM_DISP_LINEBUF)
					return (UINT16)result;
				else if(gVc0578b_Info.MarbStatus.ALayerMode == VIM_DISP_TWOFRAME)
					{
						gVc0578b_Info.MarbStatus.ALayerMode =VIM_DISP_ONEFRAME;
						goto CHAGNEAMODE1;
					}
				else
					{
						gVc0578b_Info.MarbStatus.ALayerMode =VIM_DISP_LINEBUF;
						goto CHAGNEAMODE1;
					}
			}

				
		//	result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
		    //              (PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
			////////////zoom ///////////////////
			if ( gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize.cx >= 
				(CaptureSize.cx/2 + VIM_USER_MIN_PIXEL_ONESTEP * VIM_USER_MAX_STEP))
			{
				result=VIM_MAPI_Preview_CaptureZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
			}
			else
			{
				result=VIM_MAPI_Preview_DisplayZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
			}
			VIM_IPP_SetDisplayDropFrame(0xffff);
			VIM_SIF_EnableSyncGen(ENABLE);	
			VIM_DISP_ResetState();
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
			break;
		case VIM_HAPI_PREVIEW_OFF:
		default:
			//VIM_USER_StopTimer();
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);//angela 2007-1-25	
			VIM_MAPI_Delay_Frame(1);
		 	VIM_SIF_EnableSyncGen(DISABLE);//next frame
			VIM_USER_DelayMs(30);/*added by shiyong for bug10586*/
		 	pt.x=pt.y=Amem.cx=Amem.cy=0;
		 	VIM_DISP_SetA_Memory(pt,Amem);
			//VIM_IPP_Reset();
			if((gVc0578b_Info.MarbStatus.WorkMode==VIM_MARB_PREVIEW_MODE))//||(gVc0578b_Info.MarbStatus.WorkMode==VIM_MARB_CAPTURESTILL_MODE))
			{
				VIM_MAPI_FreeCaptureJpegBuffer();
			}
			else if((gVc0578b_Info.MarbStatus.WorkMode==VIM_MARB_CAPTUREMPG4_MODE))
			{
				VIM_MAPI_FreeLCDCA0Buffer();
				VIM_MAPI_FreeCaptureVideoBuffer();
			}
			
		}
	gVc0578b_Info.PreviewStatus.Mode=MODE;
	return VIM_SUCCEED;
}
/********************************************************************************
Description:
	zoom in or zoom out
Parameters:
	Mode:
		VIM_HAPI_CHANGE_NORMAL=0,
		 	auto set normal status(preview mode).
		 	in this mode the step has no meaning.
		VIM_HAPI_CHANGE_ADD,
			when in add mode.
			the step 's meaning is that 
			source window's width  will zoom in one step
		VIM_HAPI_CHANGE_DEC,
			when in decrease mode.
			the step 's meaning is that 
			source window's width  will zoom out one step
		VIM_HAPI_SET_STEP,
		       set the direct zoom step.
	Return:
		VIM_HAPI_SUCCEED: zoom ok
	Note:
		If preview is on, zoom will take effect immediately.
		Otherwise, this function only 	save the zoom step. The zoom has effect when turn on preview.
	Remarks:
		state: valid
*********************************************************************************/
UINT16 VIM_HAPI_SetPreviewZoom(VIM_HAPI_CHANGE_MODE Mode,UINT8 step)
{
	VIM_RESULT Result;
	TSize CaptureSize=gVc0578b_Info.CaptureStatus.Size,InputSize;
	if(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS)
		return VIM_ERROR_WORKMODE;

	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
	VIM_MAPI_Delay_Frame(1);   //Delay for the stable sensor data output
	switch(Mode)
	{
		case VIM_HAPI_CHANGE_ADD:
			if (++gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep >VIM_USER_MAX_STEP)
				gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep = VIM_USER_MAX_STEP;
			break;
		case VIM_HAPI_CHANGE_DEC:
			if(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep)
				gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep--;
			break;
		case VIM_HAPI_SET_STEP:
			gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep = step;
			break;
		case VIM_HAPI_CHANGE_NORMAL:
			gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep=0;
		default:
			break;
	}

	if (gVc0578b_Info.PreviewStatus.Mode == VIM_HAPI_PREVIEW_OFF)
		return VIM_SUCCEED;
//change capture size
	VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, (VIM_SIF_RESOLUTION)gVc0578b_Info.PreviewStatus.bOutputIndex, &InputSize);
	if((CaptureSize.cx>InputSize.cx)||(CaptureSize.cy>InputSize.cy))
	{
		if(VIM_USER_FAST_PREVIEW)
		{
			CaptureSize.cx/=2;
			CaptureSize.cy/=2;
		}
	}

	if ( gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize.cx >= 
		(CaptureSize.cx/2 + VIM_USER_MIN_PIXEL_ONESTEP * VIM_USER_MAX_STEP))
	{
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintString("[VC0578B][HAPI] capture zoom:");
#endif	
		Result=VIM_MAPI_Preview_CaptureZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
	}
	else
	{
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintString("[VC0578B][HAPI] display zoom:");
#endif	
		Result=VIM_MAPI_Preview_DisplayZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
	}

	VIM_MAPI_Delay_Frame(2);   //Delay for the stable sensor data output
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);	
	return (UINT16)Result;
}
UINT16 VIM_HAPI_SetPreviewZoom_1(VIM_HAPI_CHANGE_MODE Mode,UINT8 step,INT32 framecx)
{
	VIM_RESULT Result;
    UINT8 MaxZoomstet;  //add by xinggang.xu for zoom 20081201
	TSize CaptureSize=gVc0578b_Info.CaptureStatus.Size,InputSize;
	if(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS)
		return VIM_ERROR_WORKMODE;  

	//VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	  //zhangxiang del 
	//VIM_MAPI_Delay_Frame(1);   //Delay for the stable sensor data output  //zhangxiang del 
	switch(Mode)
	{
		case VIM_HAPI_CHANGE_ADD:
            //add by xinggang.xu for zoom 20081201 start
            switch(framecx)
            {    
                case 176 : MaxZoomstet=4; break;        
                case 320 : MaxZoomstet=3; break;
                case 640 : MaxZoomstet=1; break;
                case 1280: MaxZoomstet=1; break;
                case 1600: MaxZoomstet=0; break;
                default  : MaxZoomstet=0; 
            }
			if (++gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep >MaxZoomstet)//VIM_USER_MAX_STEP)//modify by xinggang.xu for zoom 20081201
				gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep =MaxZoomstet;// VIM_USER_MAX_STEP;//modify by xinggang.xu for zoom 20081201
			break;
		case VIM_HAPI_CHANGE_DEC:
			if(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep)
				gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep--;
			break;
		case VIM_HAPI_SET_STEP:
			gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep = step;
			break;
		case VIM_HAPI_CHANGE_NORMAL:
			gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep=0;
		default:
			break;
	}

	if (gVc0578b_Info.PreviewStatus.Mode == VIM_HAPI_PREVIEW_OFF)
		return VIM_SUCCEED;
//change capture size
	VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, (VIM_SIF_RESOLUTION)gVc0578b_Info.PreviewStatus.bOutputIndex, &InputSize);
	if((CaptureSize.cx>InputSize.cx)||(CaptureSize.cy>InputSize.cy))
	{
		if(VIM_USER_FAST_PREVIEW)
		{
			CaptureSize.cx/=2;
			CaptureSize.cy/=2;
		}
	}

	if ( gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize.cx >= 
		(CaptureSize.cx/2 + VIM_USER_MIN_PIXEL_ONESTEP * VIM_USER_MAX_STEP))
	{
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintString("[VC0578B][HAPI] capture zoom:");
#endif	
		Result=VIM_MAPI_Preview_CaptureZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
	}
	else
	{
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintString("[VC0578B][HAPI] display zoom:");
#endif	
		Result=VIM_MAPI_Preview_DisplayZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
	}

	//VIM_MAPI_Delay_Frame(2);   //Delay for the stable sensor data output  //zhangxiang del 
	//VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);	 //zhangxiang del 
	return (UINT16)Result;
}
/********************************************************************************
Description:
	set capture quitlity 
Parameters:
	ratio: the Compression ratio
		
	Return:
		VIM_SUCCEED: set mode ok
		VIM_ERROR_PARAMETER: can not support this value
	Note:
		after calling this funcion. it will take few seconds to make effect.
	Remarks:
		state: valid
*********************************************************************************/
UINT16 VIM_HAPI_SetCaptureQuality(VIM_HAPI_CPTURE_QUALITY Quality)
{
  VIM_RESULT Result;
       if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;
       gVc0578b_Info.CaptureStatus.QualityMode=Quality;
       //gVc0578b_Info.CaptureStatus.QualityMode=15;
       //if (gVc0578b_Info.PreviewStatus.Mode == VIM_HAPI_PREVIEW_OFF)
		//return VIM_SUCCEED;
	Result=VIM_MAPI_SetCaptureRatio(Quality);
	return (UINT16)Result;		 
}
/********************************************************************************
Description:
	set capture ratio 
Parameters:
	ratio: the Compression ratio
		
	Return:
		VIM_SUCCEED: set mode ok
		VIM_ERROR_PARAMETER: can not support this value
	Note:
		after calling this funcion. it will take few seconds to make effect.
	Remarks:
		state: valid
*********************************************************************************/
UINT16 VIM_HAPI_SetCaptureRatio(UINT8 ratio)
{
  VIM_RESULT Result;
       if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;
       gVc0578b_Info.CaptureStatus.QualityMode=ratio;
	Result=VIM_MAPI_SetCaptureRatio(ratio);
	return (UINT16)Result;		 
}
/********************************************************************************
Description:
	Capture a still(jpeg)
Parameters:
      SaveMode:
      				VIM_HAPI_RAM_SAVE,   save in the still buf (UINT8)
				VIM_HAPI_ROM_SAVE  stillbuf is the file id  of file 
      StillBuf: the buffer to store the jpeg or the file id
      BUF_Length: the buffer length
      pUCallBack: callback function
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_WORKMODE(V5H_ERROR_PREVIEWMODE): error work mode
	VIM_ERROR_PREVIEWMODE: preview error
Note:
	if(pUCallBack==NULL) this function will return after finish capturing,
	else it will return before  finish capturing, users should judge the end in callback function.
Remarks:
	This function should be called when preview on.
       pUCallBack return status:
 	V5H_CAPTURE_END: capture end
	V5H_BUF_ERROR:     buffer length is not enough for JPEG     
state: 
	valid
*********************************************************************************/
UINT16 VIM_HAPI_CaptureStill(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *StillBuf,UINT32 BUF_Length,VIM_HAPI_CallBack pUCallBack)
{
	UINT32 dwTwc;
    VIM_RESULT result;
	UINT32 et,offset;
	UINT32 Ggain;
	UINT8 awb[3];

	//check the chip working mode 
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON) 
		return VIM_ERROR_WORKMODE;
	//check the preview  mode 
    if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_OFF)
       	return VIM_ERROR_PREVIEWMODE;

//	gVc0578b_Info.MarbStatus.WorkMode=VIM_MARB_CAPTURESTILL_MODE;
//	gVc0578b_Info.CaptureStatus.SensorOutputChange=0;

#if VIM_SENSOR_AUTO_FOCUS	
	VIM_MAPI_IspDoAFAtOnetime();
#endif
	
	//check if capture still or frame
    if(gVc0578b_Info.PreviewStatus.Mode!=VIM_HAPI_PREVIEW_FRAMEON)
    {
		gVc0578b_Info.MarbStatus.WorkMode=VIM_MARB_CAPTURESTILL_MODE;
		gVc0578b_Info.CaptureStatus.SensorOutputChange=0;
		offset=VIM_USER_RECAPTURE_OFFSET;
		VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
		//VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_YMEAN,DISABLE);		
		//gVc0578b_Info.pSensorInfo->snrGetEt(gVc0578b_Info.PreviewStatus.bOutputIndex,&et,&Ggain,awb);		
		VIM_MAPI_Delay_Frame(1);
		gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;
		result=VIM_MAPI_FastPreview();
		if(result)
			return (UINT16)result;
		
		if(gVc0578b_Info.CaptureStatus.DownSensorClock==1)
		{
			gVc0578b_Info.pSensorInfo->snrGetEt(gVc0578b_Info.PreviewStatus.bOutputIndex,&et,&Ggain,awb);		
			VIM_SYS_SetSensorClk(VIM_SYS_SENSOR_HALF);
			gVc0578b_Info.pSensorInfo->snrSetEt(gVc0578b_Info.PreviewStatus.bOutputIndex,et,Ggain,awb);
			VIM_MAPI_Delay_Frame(3);
		}
		/*if(gVc0578b_Info.CaptureStatus.SensorOutputChange==0)
		{
			VIM_MAPI_Delay_Frame(1);
		
			result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_NODISP,
				(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
			
			if(result)
				return result;			
		}*/
    	}
	else
	{
		gVc0578b_Info.MarbStatus.WorkMode=VIM_MARB_CAPTURESTILLWITHFRAME_MODE;
		gVc0578b_Info.CaptureStatus.SensorOutputChange=0;
		offset=VIM_USER_RECAPTUREWITHFRAME_OFFSET;
			VIM_MAPI_Delay_Frame(1);
		
			result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_NODISP,
				(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
			
			if(result)
				return result;			
		}
    // init parameter
	gVc0578b_Info.CaptureStatus.SaveMode=SaveMode;
	if(SaveMode==VIM_HAPI_RAM_SAVE)
	{
		gVc0578b_Info.CaptureStatus.BufPoint=StillBuf;
	}
	else
	{
		gVc0578b_Info.CaptureStatus.MallocPr=gVc0578b_Info.CaptureStatus.BufPoint=NULL;
	}
#if (BOARD_TYPE==VC0820_FPGA)//add by zhouj
	if((gVc0578b_Info.CaptureStatus.BufPoint==NULL)&&(SaveMode==VIM_HAPI_RAM_SAVE))
		return VIM_ERROR_BUFPOINT;
#else
	if(gVc0578b_Info.CaptureStatus.BufPoint==NULL)
		return VIM_ERROR_BUFPOINT;
#endif

	gVc0578b_Info.CaptureStatus.RecaptureTimes=0;
	gVc0578b_Info.CaptureStatus.ThumbBuf=0;
	gVc0578b_Info.CaptureStatus.CapFileLength=0;
	//gVc0578b_Info.CaptureStatus.CapCallback=pUCallBack;
#if (BOARD_TYPE==VC0820_FPGA)//add by zhouj
	if(SaveMode==VIM_HAPI_RAM_SAVE)
	{
		gVc0578b_Info.CaptureStatus.BufLength=BUF_Length;	
	}
	else
	{
		gVc0578b_Info.CaptureStatus.BufLength=0;	
	}
#else
	gVc0578b_Info.CaptureStatus.BufLength=BUF_Length;	
#endif
	gVc0578b_Info.CaptureStatus.CaptureError=VIM_SUCCEED;
	dwTwc=VIM_JPEG_GetTargetWordCount();

	VIM_MARB_SetCaptureNum(VIM_MARB_FIFO_0,1);
	// can enable recapture function
	if((dwTwc+offset)<=gVc0578b_Info.MarbStatus.MapList.jbufsize)
	{
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578B][HAPI] start restart capture ,dwTwc =",dwTwc);
		VIM_USER_PrintDec("[VC0578B][HAPI] jbuf size =",gVc0578b_Info.MarbStatus.MapList.jbufsize);
#endif	
		gVc0578b_Info.CaptureStatus.Mode=VIM_CAPTURECAPTURE;
		//VIM_MARB_SetCaptureMaxLen(VIM_MARB_FIFO_0,(dwTwc+VIM_USER_RECAPTURE_OFFSET));
		//VIM_MARB_SetRecaptureEn(VIM_MARB_FIFO_0,DISABLE);
		VIM_MARB_SetCaptureMaxLen(VIM_MARB_FIFO_0,(dwTwc+offset));
		VIM_MARB_SetRecaptureEn(VIM_MARB_FIFO_0,ENABLE);
	}
	else
	{
		// set buffer gapratio
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintString("[VC0578B][HAPI] start one capture ");
		VIM_USER_PrintDec("[VC0578B][HAPI] jbuf size =",gVc0578b_Info.MarbStatus.MapList.jbufsize);
#endif	
#if(BOARD_TYPE==VC0820_FPGA)
		VIM_MARB_SetJpegbufGapratio(9,VIM_MARB_FIFO_0);  
#else
		VIM_MARB_SetJpegbufGapratio(1,VIM_MARB_FIFO_0);
#endif
		VIM_MARB_SetRecaptureEn(VIM_MARB_FIFO_0,DISABLE);
		gVc0578b_Info.CaptureStatus.Mode=VIM_CAPTURESTILL;
	}

	// change timer if call back is not zero
	
	// init intrrupt
	VIM_MAPI_InitCapInterrupt();

	// enable capture
    	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);

	if((gVc0578b_Info.CaptureStatus.ThumbSize.cx*gVc0578b_Info.CaptureStatus.ThumbSize.cy)>0)
	{
		VIM_IPP_SetThumbnailEn(ENABLE);
		//VIM_HIF_SetIntEnableSec(VM_HIF_INT_IPP,1);
		//VIM_HIF_SetIntModuleEn(VM_HIF_INT_IPP,ENABLE);
		
	}
    VIM_JPEG_StartEncode(ENABLE);
	//-----------------------------------------------------------
	/*if(gVc0578b_Info.CaptureStatus.CapCallback==NULL)
	{
		while((gVc0578b_Info.CaptureStatus.Mode!=VIM_CAPTUREDONE)&&(dwCount--))
		{
			VIM_USER_DelayMs(1); 
			_ISR_HIF_IntHandle();
			if(gVc0578b_Info.CaptureStatus.CaptureError!=VIM_SUCCEED)
				return (UINT16)gVc0578b_Info.CaptureStatus.CaptureError;
		}
		if(gVc0578b_Info.CaptureStatus.Mode!=VIM_CAPTUREDONE)
			return VIM_ERROR_NOINITERRUPT;
	}*/
	return VIM_SUCCEED;
}

UINT16 VIM_HAPI_SetSensorReg(UINT32 regaddr,UINT16 regvalue)
{
	return VIM_SIF_SetSensorReg(gVc0578b_Info.pSensorInfo,regaddr,regvalue);

}

UINT16 VIM_HAPI_GetSensorReg(UINT32 regaddr,UINT16 *regvalue)
{
	return VIM_SIF_GetSensorReg(gVc0578b_Info.pSensorInfo,regaddr,regvalue);

}


/********************************************************************************
Description:
	Capture a still(jpeg) with thumbnail
Parameters:
      SaveMode:
      				VIM_HAPI_RAM_SAVE,   save in the still buf (UINT8)
				VIM_HAPI_ROM_SAVE  stillbuf is the file id  of file 
      StillBuf: the buffer to store the jpeg or the file id
      BUF_Length: the buffer length
      pUCallBack: callback function
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_WORKMODE(V5H_ERROR_PREVIEWMODE): error work mode
	VIM_ERROR_PREVIEWMODE: preview error
Note:
	if(pUCallBack==NULL) this function will return after finish capturing,
	else it will return before  finish capturing, users should judge the end in callback function.
Remarks:
	This function should be called when preview on.
       pUCallBack return status:
 	V5H_CAPTURE_END: capture end
	V5H_BUF_ERROR:     buffer length is not enough for JPEG     
state: 
	valid
*********************************************************************************/

UINT16 VIM_HAPI_CaptureStillThumbnail(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *StillBuf,UINT32 BUF_Length,VIM_HAPI_CallBack pUCallBack)
{
	UINT32 dwCount=500000,dwTwc;
	VIM_RESULT result;
	UINT8 * thumbaddr;

	//check the chip working mode 
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON) 
		return VIM_ERROR_WORKMODE;
	//check the preview  mode 
    if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_OFF)
       	return VIM_ERROR_PREVIEWMODE;

#if VIM_SENSOR_AUTO_FOCUS	
	VIM_SIF_IspDoAFAtOnetime();
#endif

	//check if capture still or frame
    if(gVc0578b_Info.PreviewStatus.Mode!=VIM_HAPI_PREVIEW_FRAMEON)
    {
		VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
		VIM_MAPI_Delay_Frame(1);
		gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;
		result=VIM_MAPI_FastPreview();
		if(result)
			return (UINT16)result;
		//VIM_MAPI_SetChipMode(VIM_MARB_CAPTURESTILLTHUMB_MODE,VIM_IPP_HAVE_NOFRAME);
		VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_NODISP,
			(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
	}
	thumbaddr=(UINT8 *)(gVc0578b_Info.MarbStatus.MapList.thumbstart+V5_1TSRAM_BASE);
	VIM_USER_PrintHex("\n thumbnail addr =",gVc0578b_Info.MarbStatus.MapList.thumbstart+V5_1TSRAM_BASE);	   
	VIM_USER_PrintDec("\n thumbnail size =",gVc0578b_Info.MarbStatus.MapList.thumbsize);
	VIM_USER_MemSet(thumbaddr, 0, gVc0578b_Info.MarbStatus.MapList.thumbsize);
	gVc0578b_Info.CaptureStatus.ThumbBuf=thumbaddr;
/*#if(BOARD_TYPE==VC0820_FPGA)	   //add by zhoujian
	pThumbInfo->ThumbnailAddr=gVc0578b_Info.CaptureStatus.ThumbBuf=thumbaddr;
	pThumbInfo->length=gVc0578b_Info.MarbStatus.MapList.thumbsize-4;
	pThumbInfo->IsThumbnail=1;
	pThumbInfo->ThumbnailSize=gVc0578b_Info.CaptureStatus.ThumbSize;
#endif*/

    dwTwc=VIM_JPEG_GetTargetWordCount();
       // init parameter
	gVc0578b_Info.CaptureStatus.SaveMode=SaveMode;
	if(SaveMode==VIM_HAPI_RAM_SAVE)
	{
		gVc0578b_Info.CaptureStatus.BufPoint=StillBuf;
	}
	else
	{
		gVc0578b_Info.CaptureStatus.MallocPr=gVc0578b_Info.CaptureStatus.BufPoint=NULL;
	}
	if(gVc0578b_Info.CaptureStatus.BufPoint==NULL)
		return VIM_ERROR_BUFPOINT;

	gVc0578b_Info.CaptureStatus.RecaptureTimes=0;
	gVc0578b_Info.CaptureStatus.CapFileLength=0;
	gVc0578b_Info.CaptureStatus.CapCallback=pUCallBack;
	gVc0578b_Info.CaptureStatus.BufLength=BUF_Length;	
	gVc0578b_Info.CaptureStatus.CaptureError=VIM_SUCCEED;
	//gVc0578b_Info.MarbStatus.Jpgpoint=gVc0578b_Info.MarbStatus.MapList.jbufstart;

	VIM_MARB_SetCaptureNum(VIM_MARB_FIFO_0,1);
	// can enable recapture function
	if((dwTwc+VIM_USER_RECAPTURE_OFFSET)<=gVc0578b_Info.MarbStatus.MapList.jbufsize)
	{
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("\n start restart capture ,dwTwc =",dwTwc);
		VIM_USER_PrintDec("\n jbuf size =",gVc0578b_Info.MarbStatus.MapList.jbufsize);
#endif	
		gVc0578b_Info.CaptureStatus.Mode=VIM_CAPTURECAPTURE;
		VIM_MARB_SetCaptureMaxLen(VIM_MARB_FIFO_0,(dwTwc+VIM_USER_RECAPTURE_OFFSET));
		VIM_MARB_SetRecaptureEn(VIM_MARB_FIFO_0,ENABLE);
	}
	else
	{
		// set buffer gapratio
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintString("\n start one capture ");
		VIM_USER_PrintDec("\n jbuf size =",gVc0578b_Info.MarbStatus.MapList.jbufsize);
#endif	
#if(BOARD_TYPE==VC0820_FPGA)
		VIM_MARB_SetJpegbufGapratio(5,VIM_MARB_FIFO_0);
#else
		VIM_MARB_SetJpegbufGapratio(1,VIM_MARB_FIFO_0);
#endif
		VIM_MARB_SetRecaptureEn(VIM_MARB_FIFO_0,DISABLE);
		gVc0578b_Info.CaptureStatus.Mode=VIM_CAPTURESTILL;
	}

	// change timer if call back is not zero
	//VIM_USER_StopTimer();
	/*if(pUCallBack)
	{
		result=VIM_USER_StartTimer(VIM_USER_CAPTURE_TIMER);
		if(result)
			return (UINT16)result;
	}*/
	
	// init intrrupt
	VIM_MAPI_InitCapInterrupt();

	// enable capture
    VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);

	if(gVc0578b_Info.MarbStatus.WorkMode==VIM_MARB_CAPTURESTILLTHUMB_MODE)
	{
		VIM_IPP_SetThumbnailEn(ENABLE);

	}

    VIM_JPEG_StartEncode(ENABLE);
	//-----------------------------------------------------------
	if(gVc0578b_Info.CaptureStatus.CapCallback==NULL)
	{
		while((gVc0578b_Info.CaptureStatus.Mode!=VIM_CAPTUREDONE)&&(dwCount--))
		{
			VIM_USER_DelayMs(1); 
			_ISR_HIF_IntHandle();
			if(gVc0578b_Info.CaptureStatus.CaptureError!=VIM_SUCCEED)
				return (UINT16)gVc0578b_Info.CaptureStatus.CaptureError;
		}
		if(gVc0578b_Info.CaptureStatus.Mode!=VIM_CAPTUREDONE)
			return VIM_ERROR_NOINITERRUPT;
	}
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	SET preview MODE(this mode can capture thumbnail)
Parameters:
	MODE: preview mode
		VIM_PREVIEW_OFF: preview off
		VIM_HAPI_PREVIEW_ON: preview on without frame
		VIM_HAPI_PREVIEW_FRAMEON: preview on with frame
	bIndex: is the index of resolution (see the sensorinfo.c)
	Return:
		VIM_SUCCEED: set mode ok
		VIM_ERROR_WORKMODE: error work mode
		VIM_ERROR_????????: Return from middle API function
	Note:
		This function must be used in Camera open mode
	Remarks:
		state: valid
*********************************************************************************/
UINT16 VIM_HAPI_SetCaptureThumbnailPreviewMode(VIM_HAPI_PREVIEW_MODE MODE,VIM_HAPI_RESOLUTION bIndex,TSize ThumbSize)
{
	VIM_RESULT result;
	TSize Amem,InputSize,CaptureSize,SourceWin;
	TPoint pt;
		
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;
	
	switch(MODE)
	{
		case VIM_HAPI_PREVIEW_ON:
		case VIM_HAPI_PREVIEW_FRAMEON:		
			//VIM_USER_StopTimer();
			//VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_YMEAN,DISABLE);
				
			VIM_SIF_EnableSyncGen(DISABLE);
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else			
			VIM_HIF_SetReg32(V5_REG_ISP_RESET, 1);
#endif				
			//set chip work mode
			if(MODE!=VIM_HAPI_PREVIEW_FRAMEON)
				VIM_MAPI_SetChipMode(VIM_MARB_CAPTURESTILLTHUMB_MODE,VIM_IPP_HAVE_NOFRAME); //actually capture mode
			else
			{
				VIM_MAPI_SetChipMode(VIM_MARB_CAPTURESTILLTHUMB_MODE,VIM_IPP_HAVEFRAME);
			}
			VIM_HIF_SetReg32(0x60010118,0x00000018);

			gVc0578b_Info.PreviewStatus.bOutputIndex=bIndex;
                  	result = VIM_SIF_SetSensorResolution(gVc0578b_Info.pSensorInfo,&gVc0578b_Info.VIM_SIF_AeAdjust,(VIM_SIF_RESOLUTION)bIndex,VIM_SIF_SNSOR_FRAMERATE_FORCAPIMAGE);//*diferrence
			if(result)
		           return (UINT16)result;
			//get input size
			result = VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, (VIM_SIF_RESOLUTION)bIndex, &InputSize);
			if(result)
		           return (UINT16)result;
	
			VIM_IPP_SetImageSize(InputSize.cx, InputSize.cy);
			CaptureSize=gVc0578b_Info.CaptureStatus.Size;
	
			//change capture size
			if((CaptureSize.cx>InputSize.cx)||(CaptureSize.cy>InputSize.cy))		//* difference, added
			{
				if(VIM_USER_FAST_PREVIEW)
				{
					CaptureSize.cx/=2;
					CaptureSize.cy/=2;
					if((CaptureSize.cx>InputSize.cx)||(CaptureSize.cy>InputSize.cy))		//* difference, added
					{
						CaptureSize.cx/=2;
						CaptureSize.cy/=2;
					}
				}
			}

			//get source window size
			result = VIM_IPP_ToolCaculateLessSrcWindow(InputSize,CaptureSize,&SourceWin);
			if(result)
				return (UINT16)result;

			//get source window start position	
			pt.x=((InputSize.cx-SourceWin.cx)/2)&0xfffe;
			pt.y=((InputSize.cy-SourceWin.cy)/2)&0xfffe;

			result = VIM_IPP_SetCaptureSize(gVc0578b_Info.pSensorInfo,pt, SourceWin, CaptureSize); 
			if(result)
				return (UINT16)result;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("\n sensor output width: ",InputSize.cx);
			VIM_USER_PrintDec("\n sensor output height: ",InputSize.cy);
			VIM_USER_PrintDec("\n Source window width: ",SourceWin.cx);
			VIM_USER_PrintDec("\n Source window height: ",SourceWin.cy);
			VIM_USER_PrintDec("\n Capture window width:  ",CaptureSize.cx);
			VIM_USER_PrintDec("\n Capture window height:  ",CaptureSize.cy);
#endif	
				//get part size of source win will display in LCD
			result = VIM_IPP_ToolCaculateBigDstWindow(SourceWin, gVc0578b_Info.PreviewStatus.Size,&InputSize);
			if(result)
				return (UINT16)result;
				
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("\n  display InputSize window width:",InputSize.cx);
			VIM_USER_PrintDec("\n  display InputSize window height:  ",InputSize.cy);
#endif	
	
			if(InputSize.cx<gVc0578b_Info.PreviewStatus.Size.cx)	//angela 2007-3-7		//*difference, added
				InputSize.cx=gVc0578b_Info.PreviewStatus.Size.cx;
			if(InputSize.cy<gVc0578b_Info.PreviewStatus.Size.cy)
				InputSize.cy=gVc0578b_Info.PreviewStatus.Size.cy;			
				
			result = VIM_IPP_SetDispalySize( pt, SourceWin, InputSize);
			if(result)
				return (UINT16)result;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("\n new display InputSize window width:",InputSize.cx);
			VIM_USER_PrintDec("\n new display InputSize window height:	",InputSize.cy);
			VIM_USER_PrintDec("preivew	window width:",gVc0578b_Info.PreviewStatus.Size.cx);
			VIM_USER_PrintDec("preivew window height:  ",gVc0578b_Info.PreviewStatus.Size.cy);			
#endif	
	
			gVc0578b_Info.CaptureStatus.ThumbSize.cx=ThumbSize.cx;
			gVc0578b_Info.CaptureStatus.ThumbSize.cy=ThumbSize.cy;
			if(gVc0578b_Info.CaptureStatus.ThumbSize.cx>254)
				gVc0578b_Info.CaptureStatus.ThumbSize.cx=254;
			if(gVc0578b_Info.CaptureStatus.ThumbSize.cy>254)
				gVc0578b_Info.CaptureStatus.ThumbSize.cy=254;
			result = VIM_IPP_SetThumbSize((UINT8)gVc0578b_Info.CaptureStatus.ThumbSize.cx, (UINT8)gVc0578b_Info.CaptureStatus.ThumbSize.cy,VIM_IPP_THUMB_FROM_SPECIAL);
			if(result)
				return (UINT16)result;
				
			gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize=SourceWin;
			gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize = InputSize;
			pt.x=(((InputSize.cx-gVc0578b_Info.PreviewStatus.Size.cx)>>1)>>1)<<1;
			pt.y=(InputSize.cy-gVc0578b_Info.PreviewStatus.Size.cy)>>1;
				
			VIM_DISP_SetA_Memory(pt,InputSize);
			VIM_DISP_SetA_DisplayPanel(gVc0578b_Info.PreviewStatus.Point,gVc0578b_Info.PreviewStatus.Size);
	
			result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
						VIM_DISP_NOTCHANGE);			//wendy guo 4/18/2007, add this function to test A0 layer rotation!
			if(result)
			return (UINT16)result;

			VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
				
			VIM_JPEG_SetSize(JPEG_422,CaptureSize);
				// angela added it for set capture ratio before preview on
			result=VIM_MAPI_SetCaptureRatio(gVc0578b_Info.CaptureStatus.QualityMode);
			if(result)
				return (UINT16)result;
			if(gVc0578b_Info.MarbStatus.ALayerMode != VIM_DISP_TVOUT_ONEFRAME &&gVc0578b_Info.MarbStatus.ALayerMode!= VIM_DISP_TVOUT_TWOFRAME)
				gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_PREVIEW_ALAYER_MODE;//ALAYER_1FRAMEBUF;
CHAGNEAMODE:
	
			result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
							  (PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
			if(result)
			{
				if(gVc0578b_Info.MarbStatus.ALayerMode == VIM_DISP_LINEBUF)
					return (UINT16)result;
				else if(gVc0578b_Info.MarbStatus.ALayerMode == VIM_DISP_TWOFRAME)
					{
						gVc0578b_Info.MarbStatus.ALayerMode =VIM_DISP_ONEFRAME;
						goto CHAGNEAMODE;
					}
				else
					{
						gVc0578b_Info.MarbStatus.ALayerMode =VIM_DISP_LINEBUF;
						goto CHAGNEAMODE;
					}
			}
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec(" user a layter mode :",gVc0578b_Info.MarbStatus.ALayerMode);
#endif	
			////////////zoom ///////////////////
			if ( gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize.cx >= 
					(CaptureSize.cx/2 + VIM_USER_MIN_PIXEL_ONESTEP * VIM_USER_MAX_STEP))
			{
				result=VIM_MAPI_Preview_CaptureZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
			}
			else
			{
				result=VIM_MAPI_Preview_DisplayZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
			}

			VIM_JPEG_SetBitRateControlEn(ENABLE);//angela	
			VIM_IPP_SetDisplayDropFrame(0xffff);
			VIM_SIF_EnableSyncGen(ENABLE);	
	
			VIM_MAPI_Delay_Frame(1);   //* difference from 528 code: _Frame(2) //Delay for the stable sensor data output
			VIM_DISP_ResetState();
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);				
			if(!gVc0578b_Info.pSensorInfo->snrtype) //rgb type
			{
#if VIM_SENSOR_HAVE_BAYER
#if VIM_SENSOR_AUTO_FOCUS
				VIM_USER_SetAutoFocusStep(gVc0578b_Info.aeaf.rightstep);
#endif
				VIM_MAPI_AEIsrInit();
#endif
			}
			//VIM_USER_StartTimer(100);		//*difference from 528 code, this function have not finished!
				//VIM_IPP_SetEffect(VIM_IPP_EFFECT_FOUR_COLOR);
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec(" %%%%%%% :",VIM_HIF_GetReg32(V5_REG_IPP_WINDOW_UPDATE));
#endif
				
			break;
		case VIM_HAPI_PREVIEW_OFF:
		default:
			//VIM_USER_StopTimer();
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);//angela 2007-1-25 
			VIM_MAPI_Delay_Frame(1);
			VIM_SIF_EnableSyncGen(DISABLE);//next frame
			VIM_USER_DelayMs(50);/*added by shiyong for bug10586*/
			pt.x=pt.y=Amem.cx=Amem.cy=0;
			VIM_DISP_SetA_Memory(pt,Amem);
		}
	gVc0578b_Info.PreviewStatus.Mode=MODE;
	return VIM_SUCCEED;

}


/********************************************************************************
Description:
	Get the  captured length(byte count).
Parameters:

Return:
		length
Note:
	The function can be used to get the length of capture still, mutishot, MJPEG	
Remarks:
state: 
	valid
*********************************************************************************/
UINT32 VIM_HAPI_GetCaptureLength(void)
{
	//check the chip working mode 
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON) 
		return 0;
	return gVc0578b_Info.CaptureStatus.CapFileLength;
}

/********************************************************************************
Description:
	Get one frame from Multishot buffer
Parameters:
      framebuf: the buffer to store the muti frame.
      buflength: the total length of multishot buffer
      framenumber: the frame number to get( first is 1)      
     *pframelength: the frame(jpeg) length. if 0, no this frame in the buffer 
Return:
	the buffer point of this frame
Note:

Remarks:
state: 
	valid
*********************************************************************************/
HUGE UINT8* VIM_HAPI_GetOneFrame(HUGE UINT8 *framebuf, UINT32 buflength, UINT8 framenumber, UINT32 *pframelength)
{
	UINT8 i=0, *ptmp;
	UINT32 length=0;
	buflength/=2;
	while(buflength--)
	{
		if( (*framebuf)==0xff&&(*(framebuf+1))==0xd8)
			ptmp=framebuf;
		else if( (*framebuf)==0xff&&(*(framebuf+1))==0xd9)
		{
			i++;
			if(i==framenumber)
			{
				if(framebuf>ptmp)
					length=framebuf+2-ptmp;
				break;
			}
		}
		framebuf+=2;
	}
	*pframelength=length;
	return ptmp;
}

/***************************************************************************************************************************************
Description:
	read raw jpeg data to marb virtual FIFO, called by user
Parameters:
       addr: 	the buffer point of raw jpeg data
       rawdatalen: 	the length of raw jpeg data that user want to write
Return:
	data count that practially writed
Note:
	none
Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_ReadJpegRawData(HUGE UINT8 *addr,UINT32 rawdatalen)
{
	UINT32 fifocount;

	fifocount=VIM_MARB_GetJbufRWSize();
			
	if(fifocount>rawdatalen)
	{
		return VIM_MARB_ReadJpegData(addr,rawdatalen);
	}
	else
	{
		return VIM_MARB_ReadJpegData(addr,fifocount);
	}

}

/***************************************************************************************************************************************
Description:
	read raw jpeg data to marb virtual FIFO, called by user
Parameters:
       addr: 	the buffer point of raw jpeg data
       rawdatalen: 	the length of raw jpeg data that user want to write
Return:
	data count that practially writed
Note:
	none
Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_ReadThumbnailData(HUGE UINT8 *addr,UINT32 buflen)
{
	UINT32 thumbnailaddr,thumbnaillen;
	thumbnailaddr = gVc0578b_Info.MarbStatus.MapList.thumbstart;
	thumbnaillen  = gVc0578b_Info.MarbStatus.MapList.thumbsize;
	gVc0578b_Info.CaptureStatus.ThumbBuf=addr;
	
	if((addr==NULL)||(thumbnailaddr==0)||(thumbnaillen>buflen))
		return 0;
	
	return VIM_HIF_ReadSram(thumbnailaddr,addr, thumbnaillen,VIM_HIF_SRAM_MODE);

}
UINT16 VIM_HAPI_JPEG_MakeExif(UINT8* pbuf,UINT32 DataLen,UINT32 BufLen,UINT8* pthumb,UINT32 ThumbLen,UINT32 *jpglen)
{
	//VIM_RESULT result;
	UINT8 *psrc,*pdst;
	UINT32 i,templen;
	UINT8 Appn[24] = {0xff,0xd8,0xff,0xe0,0x00,0x10,0x4a,0x46,0x49,0x46,0x00,0x01,0x02,\
					0x01,0x00,0x60,0x00,0x60,0x00,0x00,0xff,0xed,0x00,0x00};

	
	//if(pbuf[0] != 0xff || pbuf[1] != 0xd8)
		//return result;

	Appn[22] = (UINT8)(((ThumbLen+2)>>8)&0x00ff);	
	Appn[23] = (UINT8)((ThumbLen+2)&0x00ff);

	if(DataLen+ThumbLen+sizeof(Appn)-2 > BufLen)
		return VIM_ERROR_CAPTURE_BUFFULL;

	templen = sizeof(Appn)+ThumbLen-2;
	psrc = pdst = pbuf;

	for(i=DataLen-1+templen;i-templen>1;i--)
	{
		pdst[i] = psrc[i-templen];
	}
	VIM_USER_MemCpy(pbuf,Appn,sizeof(Appn));
		
	VIM_USER_MemCpy(pbuf+sizeof(Appn),pthumb,ThumbLen);

	//gVc0578b_Info.CaptureStatus.CapFileLength = DataLen+ThumbLen+sizeof(Appn)-2;
	*jpglen = DataLen+ThumbLen+sizeof(Appn)-2;
#if(VIM_JPEG_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("\n jpg filelen = ",*jpglen);
#endif
	
	return VIM_SUCCEED;
}
void VIM_HAPI_CaptureJpeg_Release(void)
{
	VIM_MAPI_ClearCapInterrupt();
    	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
    	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x01);
    	VIM_USER_DelayMs(1);
    	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x0);
  	VIM_JPEG_ResetState();
	VIM_MAPI_FreeCaptureJpegBuffer();
	gVc0578b_Info.MarbStatus.WorkMode=VIM_MARB_NONE_MODE;
}

void VIM_HAPI_EncodeJpeg_Release(void)
{
    	VIM_MAPI_ClearCapInterrupt();
    	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
    	VIM_JPEG_ResetState();
	VIM_MAPI_FreeEncodeJpegBuffer();
	gVc0578b_Info.MarbStatus.WorkMode=VIM_MARB_NONE_MODE;
}

UINT32 VIM_HAPI_GetJPEGCaptureStatus(void)
{
	UINT32 marbflag=0;
	//VIM_INT_Mask(VM_HIF_INT_MARB);
	
	//读取二级中断状态寄存器,用于判断
	//marbflag = VIM_HIF_GetIntFlagSec(VM_HIF_INT_MARB); 
	
	//清除中断标志	
	//VIM_INT_Acknowledge(VM_HIF_INT_MARB);	
	//VIM_INT_Unmask(VM_HIF_INT_MARB);


	marbflag=VIM_HIF_GetReg8(V5_REG_IPP_Q_HISTOGRAM);
	return marbflag;
}

void VIM_HAPI_SetJPEGCaptureStatus(UINT32 marbflag)
{
	
	//VIM_INT_Mask(VM_HIF_INT_MARB);
	
	//读取二级中断状态寄存器,用于判断
	//marbflag = VIM_HIF_GetIntFlagSec(VM_HIF_INT_MARB); 
	
	//清除中断标志	
	//VIM_INT_Acknowledge(VM_HIF_INT_MARB);	
	//VIM_INT_Unmask(VM_HIF_INT_MARB);

	VIM_HIF_SetReg8(V5_REG_IPP_Q_HISTOGRAM,(UINT8)marbflag);
	//MMD_SYS_SetMem(V5_REG_IPP_Q_HISTOGRAM,marbflag);

}


/********************************************************************************
Description:
	the one frame have been encode call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_HAPI_Cap_Recapture(void)
{
UINT8 qf;
//UINT32 Length;
	//Length=VIM_MARB_GetJbufRWSize();
	//revised by zhoujian_2008.01.29
#if (BOARD_TYPE==VC0820_FPGA) //add by zhoujian 
	VIM_JPEG_StartEncode(DISABLE);
	//VIM_MARB_SetFifoStop(VIM_MARB_FIFO_0);
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);	
#endif	
/*#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID] fifo length is =",Length);
	VIM_USER_PrintDec("[VC0578B][MID] qf is =",VIM_JPEG_GetQFactor());
	VIM_USER_PrintString("[VC0578B][MID]VIM_MAPI_ISR_Recapture");
	VIM_USER_PrintDec("[VC0578B][MID] recapture time is=",gVc0578b_Info.CaptureStatus.RecaptureTimes);
#endif*/
	if(gVc0578b_Info.CaptureStatus.RecaptureTimes>=VIM_USER_RECAPTUREMAXTIMES)
		VIM_MAPI_ISR_JbufError();
	else
	{
		VIM_JPEG_SetBitRateControlEn(DISABLE);		//??没有效果
		qf=VIM_JPEG_GetQFactor();
		if((qf+VIM_USER_QFONESTEP)<127)
			VIM_JPEG_SetBrcQF(qf+VIM_USER_QFONESTEP);
		else
			VIM_JPEG_SetBrcQF(127);
		gVc0578b_Info.CaptureStatus.RecaptureTimes++;
		if(gVc0578b_Info.CaptureStatus.SaveMode==VIM_HAPI_ROM_SAVE)
		{
		}
		else
		{
			gVc0578b_Info.CaptureStatus.BufPoint-=gVc0578b_Info.CaptureStatus.CapFileLength;
			gVc0578b_Info.CaptureStatus.BufLength+=gVc0578b_Info.CaptureStatus.CapFileLength;
			gVc0578b_Info.CaptureStatus.CapFileLength=0;
		}
		gVc0578b_Info.CaptureStatus.CaptureError=VIM_SUCCEED;	//angela add it 2007-6-21
		//VIM_JPEG_StartEncode(DISABLE);
	 	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
	 	VIM_JPEG_StartEncode(ENABLE);
	}
}


/********************************************************************************
Description:
	fifo count  call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_HAPI_Cap_FifoCont(void)
{
	UINT32 length;//,OneFifocount=VIM_MARB_GetJbufUpThreshold();
	//UINT32 stime,etime;

	length=VIM_MARB_GetJbufRWSize();
	/*if(!length)		//angela 2007-5-8
		return;*/
	//switch(gVc0578b_Info.CaptureStatus.Mode)
	//{
		//case VIM_CAPTURESTILL:
		//case VIM_CAPTUREMPE4:
//FIFOREAD:
//#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			//MMDPrintf("[VC0578B][MID] VIM_MAPI_ISR_FifoCont length=%d\n",length);

			//VIM_USER_PrintDec("[VC0578B][MID] VIM_MAPI_ISR_FifoCont length=",length);
//#endif
			//if(gVc0578b_Info.CaptureStatus.SaveMode==VIM_HAPI_RAM_SAVE)
			//{
				if(length<=gVc0578b_Info.CaptureStatus.BufLength)
				{
					//stime=VIM_USER_GetSystemTime_1us();
					VIM_MARB_ReadJpegData(gVc0578b_Info.CaptureStatus.BufPoint,length);
					//etime=VIM_USER_GetSystemTime_1us();
//#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
					//MMDPrintf("\n[VC0578B][MID] read %d count,use %d us\n",length,etime-stime);
//#endif
					gVc0578b_Info.CaptureStatus.BufPoint+=length;
					gVc0578b_Info.CaptureStatus.BufLength-=length;
					gVc0578b_Info.CaptureStatus.CapFileLength+=length;
					//gVc0578b_Info.VideoStatus.CapVideoLength+=length;
					
					/*length=VIM_MARB_GetJbufRWSize();
					if(!length)
					{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
					VIM_USER_PrintString("[VC0578B][MID] length==0");
#endif	
						break;
					}
					//if(length>=(OneFifocount-2*VIM_MARB_JBUF_GAPINTERVAL))
					//if(length>0)
					if(length>=VIM_MARB_JBUF_GAPINTERVAL)
					{
						goto FIFOREAD;
					}*/
					
				}
				else
				{
					gVc0578b_Info.CaptureStatus.CaptureError=VIM_ERROR_CAPTURE_BUFFULL;
					//goto FIFOERROR;		
				}
			//}
			//else
			//{
				//gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_FIFO_UP,length);
			//}
			//break;
			
	//}
	
//FIFOERROR:
/*#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
	VIM_USER_PrintDec("[VC0578B][MID] VIM_MAPI_ISR_FifoCont [error]",gVc0578b_Info.CaptureStatus.CaptureError);
#endif	
	VIM_MAPI_ISR_JbufError();*/
	
	return;

}


/********************************************************************************
Description:
	the capture done call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

UINT32 VIM_HAPI_Cap_JbufDone(void)
{
UINT32 length,dwSavelen=0;
UINT8 qf;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID] qf3 times is =",VIM_HIF_GetReg8(V5_REG_JPEG_BRC));
#endif
//revised by zhoujian_2008.01.29
#if (BOARD_TYPE==VC0820_FPGA) //add by zhoujian 
	VIM_JPEG_StartEncode(DISABLE);
	if((gVc0578b_Info.CaptureStatus.ThumbSize.cx*gVc0578b_Info.CaptureStatus.ThumbSize.cy)>0)
	{
		VIM_IPP_SetThumbnailEn(DISABLE);
	}
	//VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	VIM_MARB_SetFifoStop(VIM_MARB_FIFO_0);
	VIM_JPEG_SetBitRateControlEn(DISABLE);	
#else
	VIM_MARB_SetFifoStop(VIM_MARB_FIFO_0);
#endif	
	//if(gVc0578b_Info.CaptureStatus.CaptureError)
		//goto CAPUTREERROR;	
//GETLENGTH:
	length=dwSavelen=VIM_MARB_GetJbufRWSize();
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID] VIM_MAPI_ISR_JbufDone,length is ",length);
#endif
	switch(gVc0578b_Info.CaptureStatus.Mode)
	{
		case VIM_CAPTURESTILL:
		case VIM_CAPTURECAPTURE:

			if(gVc0578b_Info.CaptureStatus.SaveMode==VIM_HAPI_RAM_SAVE)
			{
				if(length<=gVc0578b_Info.CaptureStatus.BufLength)
				{
					VIM_MARB_ReadJpegData(gVc0578b_Info.CaptureStatus.BufPoint,length);
					gVc0578b_Info.CaptureStatus.BufLength-=length;
					gVc0578b_Info.CaptureStatus.BufPoint+=length;
					gVc0578b_Info.CaptureStatus.CapFileLength+=length;
					gVc0578b_Info.CaptureStatus.BufPoint-=gVc0578b_Info.CaptureStatus.CapFileLength;
					if((gVc0578b_Info.CaptureStatus.BufPoint[0]==0xff)&&(gVc0578b_Info.CaptureStatus.BufPoint[1]==0xd8))
					{
						gVc0578b_Info.CaptureStatus.BufPoint+=gVc0578b_Info.CaptureStatus.CapFileLength;
					}
					else
					{
						gVc0578b_Info.CaptureStatus.BufLength+=gVc0578b_Info.CaptureStatus.CapFileLength;
						gVc0578b_Info.CaptureStatus.CapFileLength=0;
						//gVc0578b_Info.CaptureStatus.ErrorImageRecaptureTimes++;
						MMDPrintf("\ncapture image error!\n");
						goto RECAPUTRE;
					}
				}
				else
				{
					gVc0578b_Info.CaptureStatus.CaptureError=VIM_ERROR_CAPTURE_BUFFULL;
					goto CAPUTREERROR;	
				}
			}
			else
			{
				//VIM_USER_StopTimer();
				//gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_CAPTURE_END,length);
			}
			//gVc0578b_Info.CaptureStatus.Mode=VIM_CAPTUREDONE;
			goto CAPUTREOK;
			break;
	}
RECAPUTRE:
	VIM_JPEG_StartEncode(DISABLE);
	//VIM_MARB_SetFifoStop(VIM_MARB_FIFO_0);
	VIM_MARB_SetFifoDisable(VIM_MARB_FIFO_0);	
	if(gVc0578b_Info.CaptureStatus.RecaptureTimes>=VIM_USER_RECAPTUREMAXTIMES)
		VIM_MAPI_ISR_JbufError();
	else
	{
		VIM_JPEG_SetBitRateControlEn(DISABLE);		//??没有效果
		qf=VIM_JPEG_GetQFactor();
		if((qf+VIM_USER_QFONESTEP)<127)
			VIM_JPEG_SetBrcQF(qf+VIM_USER_QFONESTEP);
		else
			VIM_JPEG_SetBrcQF(127);
		gVc0578b_Info.CaptureStatus.RecaptureTimes++;
	
		gVc0578b_Info.CaptureStatus.CaptureError=VIM_SUCCEED;	//angela add it 2007-6-21
		//VIM_JPEG_StartEncode(DISABLE);
	 	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
	 	VIM_JPEG_StartEncode(ENABLE);
	
	}
	return 0;

CAPUTREERROR:
	//VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	VIM_MARB_SetFifoDisable(VIM_MARB_FIFO_0);

	/*if(gVc0578b_Info.CaptureStatus.CapCallback)
	{
		VIM_USER_StopTimer();
		VIM_MAPI_ClearCapInterrupt();
		gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_BUF_ERROR,gVc0578b_Info.CaptureStatus.CaptureError);
	}*/
	return 0;
CAPUTREOK:
//revised by zhoujian_2008.01.29

	//VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	VIM_MARB_SetFifoDisable(VIM_MARB_FIFO_0);

	if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_FRAMEON) //zhangxiang mod 
	{
		VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
		gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;
	}
	
	if(gVc0578b_Info.CaptureStatus.DownSensorClock==1)
	{
		gVc0578b_Info.CaptureStatus.DownSensorClock=0;
		VIM_SYS_SetSensorClk(VIM_SYS_SENSOR_CLKNORMAL);
	}
	
	gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep=0;
	VIM_MAPI_ClearCapInterrupt();
	//VIM_HIF_SetIntEnableSec(VM_HIF_INT_IPP,0);
	//VIM_HIF_SetIntModuleEn(VM_HIF_INT_IPP,DISABLE);
	if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_OFF)
		VIM_MAPI_EnableSyncGen(DISABLE);	//angela 2006-9-4

	return gVc0578b_Info.CaptureStatus.CapFileLength;
}

/********************************************************************************
Description:
	the capture done call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_HAPI_Cap_JbufError(void)
{

	VIM_MARB_SetFifoStop(VIM_MARB_FIFO_0);
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	if(gVc0578b_Info.CaptureStatus.RecaptureTimes<VIM_USER_RECAPTUREMAXTIMES)
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][MID] VIM_MAPI_ISR_JbufError [ERROR]");
#endif	
		VIM_HIF_ClearIntModule(VM_HIF_INT_MARB);
		VIM_MAPI_InitCapInterrupt();
		VIM_JPEG_ResetState();
		VIM_MAPI_ISR_Recapture();
		
       	return;
	}

}

/***************************************************************************************************************************************
Description:
	set encode mode and parameter, encode one frame 
Parameters:
      jyuvdataBuf: 	the buffer point of YUV block data
      ImageSize: 	YUV image size
      YUVmode	:      the YUV mode of image
      VIM_HAPI_CPTURE_QUALITY	: the encode qulity
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_?????????: return form mid api
Note:
	
Remarks:
state: 
	valid
****************************************************************************************************************************************/

UINT16 VIM_HAPI_EncodeJpeg(HUGE void *StillBuf,UINT32 BUF_Length,UINT32 *pjpglen)
{
	VIM_RESULT result;
	UINT8 status;
	UINT8 *yuvdatabuf;
	TSize JpegSize;
	VIM_JPEG_YUVMODE YUVmode;
	UINT32 marbflag=0,timecount=0xf000;
	
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON&&gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_PHOTOPROCESS) 
		return VIM_ERROR_WORKMODE;

	VIM_SIF_EnableSyncGen(DISABLE); //angela 2006-8-15	
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);
	
	gVc0578b_Info.EncodeStatus.YUVBufPoint=gVc0578b_Info.CaptureStatus.ThumbBuf;
	if(gVc0578b_Info.EncodeStatus.YUVBufPoint==NULL)
		return VIM_ERROR_BUFPOINT;	
	VIM_USER_PrintHex(" [VC0578B][MARB]YUVBufPoint    ",(UINT32)gVc0578b_Info.EncodeStatus.YUVBufPoint);
	
	gVc0578b_Info.EncodeStatus.JpegSize.cx=gVc0578b_Info.CaptureStatus.ThumbSize.cx;		
	gVc0578b_Info.EncodeStatus.JpegSize.cy=gVc0578b_Info.CaptureStatus.ThumbSize.cy;	
	gVc0578b_Info.EncodeStatus.YUVmode=JPEG_422;
	gVc0578b_Info.EncodeStatus.QualityMode=gVc0578b_Info.CaptureStatus.QualityMode=VIM_HAPI_CPTURE_QUALITY_HIGH;
	gVc0578b_Info.CaptureStatus.Mode=VIM_CAPTURESTILL;
	
	VIM_MAPI_SetChipMode(VIM_MARB_ENCODE_MODE,VIM_IPP_HAVE_NOFRAME);
	
	VIM_JPEG_SetSize(gVc0578b_Info.EncodeStatus.YUVmode,gVc0578b_Info.EncodeStatus.JpegSize);
	// angela added it for set capture ratio before preview on
	result=VIM_MAPI_SetCaptureRatio(gVc0578b_Info.CaptureStatus.QualityMode);
	if(result)
		return (UINT16)result;

	result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_NODISP,
			(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
	if(result)
		return (UINT16)result;
	
	VIM_MAPI_InitCapInterrupt();
	VIM_MARB_SetCaptureNum(VIM_MARB_FIFO_0,1);		
	VIM_MARB_SetJpegbufGapratio(9,VIM_MARB_FIFO_0);
	VIM_MARB_SetRecaptureEn(VIM_MARB_FIFO_0,DISABLE);

	gVc0578b_Info.CaptureStatus.BufPoint=gVc0578b_Info.EncodeStatus.BufPoint=(UINT8 *)StillBuf;
	gVc0578b_Info.CaptureStatus.CapFileLength=0;
	gVc0578b_Info.CaptureStatus.CapCallback=NULL;
	gVc0578b_Info.CaptureStatus.BufLength=gVc0578b_Info.EncodeStatus.BufLength=BUF_Length;	
	gVc0578b_Info.CaptureStatus.CaptureError=VIM_SUCCEED;
	
	yuvdatabuf=gVc0578b_Info.EncodeStatus.YUVBufPoint;
	JpegSize=gVc0578b_Info.EncodeStatus.JpegSize;
	YUVmode=gVc0578b_Info.EncodeStatus.YUVmode;

	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
	VIM_JPEG_StartEncodeYUV(ENABLE);

	status=VIM_JPEG_GetStatus();
	while(!(status&0x08))//header done
	{
		status=VIM_JPEG_GetStatus();
	}
	VIM_MAPI_SendYUVData(yuvdatabuf,JpegSize,YUVmode);

	while(timecount)
	{
		marbflag=VIM_HIF_GetReg8(V5_REG_IPP_Q_HISTOGRAM);
		if(marbflag)
		{
			VIM_HIF_SetReg8(V5_REG_IPP_Q_HISTOGRAM,0);
			if(marbflag&VIM_MARB_FIFOFRMCNT)
       		{
       			printf("Encode Done!\n");
               		break;
               	}			
		}
	}
	
	if(timecount==0)
	{
		printf("Encode Timeout!\n");
		return VIM_ERROR_TIME_OUT;					
	}
	
	result=VIM_MAPI_Encode_JbufDone();
	if(result)
	{
		*pjpglen=result;
		return VIM_SUCCEED;	
	}
	else
	{
		*pjpglen=0;
		return VIM_ERROR_CAPTURE_BUFFULL;		
	}
}

#endif


#if JPEGDIS
/********************************************************************************
JPEG display related
*******************************************************************************/

/***************************************************************************************************************************************
Description:
	write raw jpeg data to marb virtual FIFO, called by user
Parameters:
       addr: 	the buffer point of raw jpeg data
       rawdatalen: 	the length of raw jpeg data that user want to write
Return:
	data count that practially writed
Note:
	none
Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_WriteJpegRawData(HUGE UINT8 *addr,UINT32 rawdatalen)
{
	UINT32 fifolen;

	fifolen=VIM_MARB_GetJbufRWSize();
		
	if(fifolen>rawdatalen)
	{
		return VIM_MAPI_WriteJpegDataToFIFO(addr,rawdatalen,VIM_HIF_FIFO_0_MODE);
	}
	else
	{
		return VIM_MAPI_WriteJpegDataToFIFO(addr,fifolen,VIM_HIF_FIFO_0_MODE);
	}

}

/***************************************************************************************************************************************
Description:
	parser the jpeg file information
Parameters:
      Info: 	the struct that used to input user infomation
      OutInfo: 	the struct that used to record jpeg file infomation
    
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_YUV_UNKNOW:jpeg file yuv mode error, can not display
Note:
Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_ParserJpegFile(PVIM_INJPEGDISPLAYINFO Info,PVIM_FILEINFO OutInfo)
{
	VIM_RESULT result;

	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON&&gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_PHOTOPROCESS) 
		return VIM_ERROR_WORKMODE;

	gVc0578b_Info.DisplayStatus.BufPoint=Info->filebuffer;
	gVc0578b_Info.DisplayStatus.BufLength=Info->fileLength;

	result = VIM_MAPI_GetJpegInformation(gVc0578b_Info.DisplayStatus.BufPoint,gVc0578b_Info.DisplayStatus.BufLength);
	
	if(result)
	{
		return (UINT16)result;
	}

	VIM_USER_MemCpy((UINT8 *)&(OutInfo->ImageInfo),(UINT8 *)&gVc0578b_JpegInfo,sizeof(gVc0578b_JpegInfo));
	OutInfo->DisplayMode=Info->DisplayMode;

	switch(OutInfo->DisplayMode)
	{
		case VIM_HAPI_DISPLAY_JPEG:
			OutInfo->RawDataPosition=gVc0578b_Info.DisplayStatus.BufPoint+gVc0578b_JpegInfo.offset;
			OutInfo->RawDataLength=gVc0578b_Info.DisplayStatus.BufLength-gVc0578b_JpegInfo.offset;
			if(OutInfo->RawDataLength &0x7f)
			{
				OutInfo->RawDataLength >>=7;
				OutInfo->RawDataLength +=1;
				OutInfo->RawDataLength <<=7;		
			}
			break;
		case VIM_HAPI_DISPLAY_MOTIONJPEG:	
			OutInfo->RawDataPosition=gVc0578b_Info.DisplayStatus.BufPoint;
			OutInfo->RawDataLength=gVc0578b_Info.DisplayStatus.BufLength;
			if(OutInfo->RawDataLength &0x7f)
			{
				OutInfo->RawDataLength >>=7;
				OutInfo->RawDataLength +=1;
				OutInfo->RawDataLength <<=7;		
			}
			break;
		default:
			break;
	}
	
	return VIM_SUCCEED;	
}

/***************************************************************************************************************************************
Description:
      initial jpeg decode information
Parameters:
      Info: 	the struct that used to input user infomation
      FileInfo: 	the struct that used to record jpeg file infomation
      DecodeInfo:	the struct that used to record jpeg decode infomation
Return:
	VIM_SUCCEED:  ok
Note:
	Info.LcdWantPt.x+Info.LcdWantSize.cx<=LCD width
	Info.LcdWantPt.y+Info.LcdWantSize.cy<=LCD height
Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_InitDisplayInfo(PVIM_INJPEGDISPLAYINFO Info,PVIM_FILEINFO FileInfo,PVIM_JPEG_DECODE_INFO DecodeInfo)
{

	DecodeInfo->LcdWantPt=Info->LcdWantPt;
	DecodeInfo->LcdWantSize=Info->LcdWantSize;
	DecodeInfo->JpegCallBack=Info->JpegCallBack;

	VIM_USER_MemCpy((UINT8 *)&(DecodeInfo->ImageInfo),(UINT8 *)&(FileInfo->ImageInfo),sizeof(FileInfo->ImageInfo));
	DecodeInfo->DisplayMode=FileInfo->DisplayMode;
	return VIM_SUCCEED;	
}
#if 1
/***************************************************************************************************************************************
Description:
	set display mode and parameter, display one frame or more frame jpeg picture on LCD
Parameters:
      jpegBuf: 	the buffer point of jpeg
      length: 	the length of jpeg (=>jpeg file length)
      LcdOffsetX,LcdOffsetY		: the display coordinate on LCD panel.
      DisplayWidth,DisplayHeigth	: the display width and height.
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_WORKMODE: error work mode
	VIM_ERROR_YUV_UNKNOW:jpeg file yuv mode error, can not display
	VIM_ERROR_PARAMETER: marb jbuf size less than capture size, so data is not good
	VIM_ERROR_?????????: return form mid api
Note:
	LcdOffsetX+DisplayWidth<=LCD width
	LcdOffsetY+DisplayHeigth<=LCD height
	length=file length(this is important)
Remarks:
state: 
	valid
****************************************************************************************************************************************/

// this function add by Amanda Deng 2008/01/30
UINT32 VIM_HAPI_ReadyToDisplayJpeg(PVIM_JPEG_DECODE_INFO DecodeInfo)
{
	VIM_RESULT result;
	TSize DestDisplaySize,DestDisplaySizeA1;
	TSize WindowSize,DisplaySize,ImageSize;
	TPoint pt = {0, 0};
	TPoint WindowPt = {0, 0};
	TPoint DisplayPt = {0, 0};
						
	// 1, disable syncgen
	VIM_SIF_EnableSyncGen(DISABLE); //angela 2006-8-15
	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;
	// 2. check chip workmode
	if(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS)
		return VIM_ERROR_WORKMODE;
	// 3. get image information
	VIM_USER_MemCpy((UINT8 *)&gVc0578b_JpegInfo,(UINT8 *)&(DecodeInfo->ImageInfo),sizeof(gVc0578b_JpegInfo));	
	//gVc0578b_Info.DisplayStatus.JpegCallBack=DecodeInfo->JpegCallBack;
	/*gVc0578b_Info.PreviewStatus.LCDCA0FrameStartTime=0;
	gVc0578b_Info.PreviewStatus.LCDCA0FrameEndTime=0;
	gVc0578b_Info.PreviewStatus.LCDCA0FrameTimes=0;
	gVc0578b_Info.PreviewStatus.LCDCA0FrameStartTime=VIM_USER_GetSystemTime_10ms()*10;*/
	ImageSize.cx = DecodeInfo->ImageInfo.JpgSize.cx;
	ImageSize.cy = DecodeInfo->ImageInfo.JpgSize.cy;
	// 4. get source window size and position(window size<=image size)
	WindowSize.cx = DecodeInfo->JpegWinSize.cx;
	WindowSize.cy = DecodeInfo->JpegWinSize.cy;
	WindowPt.x = DecodeInfo->JpegOffsetPt.x;
	WindowPt.y = DecodeInfo->JpegOffsetPt.y;
    if((WindowPt.x+WindowSize.cx)>(DecodeInfo->ImageInfo.ImageSize.cx))
		WindowPt.x = (DecodeInfo->ImageInfo.ImageSize.cx) - WindowSize.cx;
	if((WindowPt.y+WindowSize.cy)>(DecodeInfo->ImageInfo.ImageSize.cy))
		WindowPt.y = (DecodeInfo->ImageInfo.ImageSize.cy) - WindowSize.cy;
	WindowSize.cx =(WindowSize.cx&0xfffe);
	WindowSize.cy =(WindowSize.cy&0xfffe);
	WindowPt.x=(WindowPt.x&0xfffe);	
	WindowPt.y=(WindowPt.y&0xfffe);	
	#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]image width: ",ImageSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]image height:",ImageSize.cy);
	VIM_USER_PrintDec("[VC0578][HAPI]source window width: ",WindowSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]source window height:",WindowSize.cy);
	VIM_USER_PrintDec("[VC0578][HAPI]source window xpos:	",WindowPt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]source window ypos: ",WindowPt.y);
	#endif	
	// 5. get display size and position(display size<=lcd size)
	DisplaySize.cx = DecodeInfo->LcdWantSize.cx;
	DisplaySize.cy = DecodeInfo->LcdWantSize.cy;
	DisplayPt.x = DecodeInfo->LcdWantPt.x;
	DisplayPt.y = DecodeInfo->LcdWantPt.y;
	DisplaySize.cx=(DisplaySize.cx&0xfffe);	
	DisplaySize.cy=(DisplaySize.cy&0xfffe);
	DisplayPt.x=(DisplayPt.x&0xfffe);	
	DisplayPt.y=(DisplayPt.y&0xfffe);	
	//check display size>{4,4},must less than (LCD size),get display size
	// 6. set display effect mode
	VIM_HAPI_SetEffect((VIM_IPP_EFFECT)DecodeInfo->EffectMode);
	// 7. set a layer rotation mode
	VIM_HAPI_SetLCDMirror((VIM_DISP_ROTATEMODE)DecodeInfo->RotationMode, VIM_HAPI_A_LAYER);
	
	result = VIM_MAPI_CheckDisplaySize(DisplayPt.x,DisplayPt.y,DisplaySize.cx,DisplaySize.cy,&DestDisplaySize);
	if(result)
		{goto DISPLAYERROR;}
	DisplaySize.cx = gVc0578b_Info.DisplayStatus.LcdWantSize.cx;
	DisplaySize.cy = gVc0578b_Info.DisplayStatus.LcdWantSize.cy;
	DisplayPt.x = gVc0578b_Info.DisplayStatus.LcdWantPt.x;
	DisplayPt.y = gVc0578b_Info.DisplayStatus.LcdWantPt.y;
	#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]display dest width: ",DisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]display dest height:",DisplaySize.cy);
	VIM_USER_PrintDec("[VC0578][HAPI]display dest xpos:	",DisplayPt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]display dest ypos: ",DisplayPt.y);
	#endif	
	
	// 8. set jpeg display mode
	switch(DecodeInfo->DisplayMode)
	{
		case VIM_HAPI_DISPLAY_JPEG:
		case VIM_HAPI_DISPLAY_MOTIONJPEG:
			VIM_MAPI_SetDisplayJpegMode();
			break;
		case VIM_HAPI_DISPLAY_YUV:
			VIM_MAPI_SetDisplayYUVMode();
			break;			
	}	
	// 9. get real display size(display image in user define display size, maybe not full display in LCD, only part of user define display size)

	//result = VIM_IPP_ToolCaculateLessDisplayWindow(WindowSize,DisplaySize,&DestDisplaySize);
	if(VIM_USER_DISPLAY_FULLSCREEN)
		result = VIM_IPP_ToolCaculateBigDstWindow(WindowSize,DisplaySize,&DestDisplaySize);
	else
		result = VIM_IPP_ToolCaculateLessDisplayWindow(WindowSize,DisplaySize,&DestDisplaySize);//VIM_IPP_ToolCaculateLessDisplayWindow(ImageSize,DisplaySize,&DestDisplaySize);//
	if(result)
		{goto DISPLAYERROR;}
	
	DisplaySize.cx = DestDisplaySize.cx;
	DisplaySize.cy = DestDisplaySize.cy;
	#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]caculate dest display size width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]caculate dest display size height:	",DestDisplaySize.cy);
	#endif	
	// 10.set image size(ipp module)
	VIM_IPP_SetImageSize(ImageSize.cx,ImageSize.cy);
	// 11.set source window size, display size(ipp module)
	result = VIM_IPP_SetDisplaySizerSize(WindowPt,WindowSize,DisplaySize,&DestDisplaySize);
	//result = VIM_IPP_SetCaptureSize(NULL,WindowPt,WindowSize,DestDisplaySize);
	if(result)
		{goto DISPLAYERROR;}
	//if(VIM_USER_DISPLAY_FULLSCREEN)//zhangxiang del
	{
		DisplaySize.cx = gVc0578b_Info.DisplayStatus.LcdWantSize.cx;
		DisplaySize.cy = gVc0578b_Info.DisplayStatus.LcdWantSize.cy;
	}
	//else//zhangxiang del
	//{
	//	DisplaySize.cx = DestDisplaySize.cx;
	//	DisplaySize.cy = DestDisplaySize.cy;
	//}
	
	#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]real dest display size width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]real dest display size height:	",DestDisplaySize.cy);
	#endif	
	// 12.set LCDC A layer memory size and window size
	pt.x=pt.y=0;
	if(DestDisplaySize.cx>DisplaySize.cx)	
		pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
	if(DestDisplaySize.cy>DisplaySize.cy)	
		pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;
	#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
	#endif		

    	result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
	if(result)
		goto DISPLAYERROR;
			
	pt.x=(gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	pt.y=(gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;

	if(DestDisplaySize.cx<=DisplaySize.cx)	
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	else
		DestDisplaySize.cx=DisplaySize.cx;
	if(DestDisplaySize.cy<=DisplaySize.cy)	
		pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;
	else
		DestDisplaySize.cy=DisplaySize.cy;
	#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset y:  ",pt.y);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize height:  ",DestDisplaySize.cy);
	#endif	
	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);
	DestDisplaySizeA1.cx=0;
	DestDisplaySizeA1.cy=0;
	VIM_DISP_SetA1_DisplayPanel(pt,DestDisplaySizeA1);
	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,VIM_DISP_NOTCHANGE);
	if(result)
		goto DISPLAYERROR;
	VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
	//enable a layer
	VIM_DISP_ResetState();
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
	//set layer A mode, normal display use ALAYER_LINEBUF mode
	gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_DISPLAY_USELINEBUF;
	result = VIM_MAPI_SetDisplayJpegParameter();
	if(result)
		goto DISPLAYERROR;
	
	/*switch(DecodeInfo->DisplayMode)
	{
		case VIM_HAPI_DISPLAY_JPEG:
		case VIM_HAPI_DISPLAY_MOTIONJPEG:	
			VIM_MAPI_InitDispInterrupt();;
			break;
		case VIM_HAPI_DISPLAY_YUV:	
			VIM_MAPI_ClearDispInterrupt();
			break;
		default:
			break;
	}
	VIM_INT_Acknowledge(VM_HIF_INT_LCDC);
	VIM_DISP_RegisterISR(VIM_INT_DISP_ALL,NULL);
		
	VIM_DISP_RegisterISR(VIM_INT_DISP_A0_DONE,VIM_MAPI_ISR_LCDCA0DoneDebug);
	VIM_DISP_SetIntEnable(VIM_INT_DISP_A0_DONE,ENABLE);	*/

DISPLAYERROR:
	if(result)
	{
		return (UINT16)result;
	}
	return VIM_SUCCEED;
}


#else
UINT32 VIM_HAPI_ReadyToDisplayJpeg(PVIM_JPEG_DECODE_INFO DecodeInfo)
{
	VIM_RESULT result;
	TSize DisplaySize,DestDisplaySize,WindowSize,DestDisplaySizeA1;
	VIM_IPP_EFFECT Effect;
	TPoint pt = {0, 0};
						
	//disable syncgen
	VIM_SIF_EnableSyncGen(DISABLE); //angela 2006-8-15
	if(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS)
		return VIM_ERROR_WORKMODE;

	WindowSize=DecodeInfo->JpegWinSize;
	pt=DecodeInfo->JpegOffsetPt;

	Effect=(VIM_IPP_EFFECT)DecodeInfo->EffectMode;
	switch(DecodeInfo->EffectMode)
	{
		case  VIM_HAPI_SPECIAL_MONOCHROME: 
			Effect=VIM_IPP_EFFECT_SPECIALCOLOR;
			VIM_IPP_SetEffectUVThreshold(0,0,0,0);
			break;
		case  VIM_HAPI_SPECIAL_REDONLY: 
			Effect=VIM_IPP_EFFECT_SPECIALCOLOR;
			VIM_IPP_SetEffectUVThreshold(128,85,255,128);
			break;
		case  VIM_HAPI_SPECIAL_SEPIA:
			VIM_IPP_SetEffectUVoffset(85,128);
			break;
		default:
			break;
	}
	VIM_IPP_SetEffect(Effect);
	gVc0578b_Info.PreviewStatus.SpecialEffect=(VIM_HAPI_SPECIAL_EFFECT)DecodeInfo->EffectMode;
	gVc0578b_Info.LcdStatus.ARotationMode=(VIM_DISP_ROTATEMODE)DecodeInfo->RotationMode;
	// set all module to display jpeg mode
	switch(DecodeInfo->DisplayMode)
	{
		case VIM_HAPI_DISPLAY_JPEG:
		case VIM_HAPI_DISPLAY_MOTIONJPEG:
			VIM_MAPI_SetDisplayJpegMode();
			break;
		case VIM_HAPI_DISPLAY_YUV:
			VIM_MAPI_SetDisplayYUVMode();
			break;			
	}	
	//VIM_HIF_SetReg32(0x60010118,0x00000018);
	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;

	VIM_USER_MemCpy((UINT8 *)&gVc0578b_JpegInfo,(UINT8 *)&(DecodeInfo->ImageInfo),sizeof(gVc0578b_JpegInfo));	
		
	gVc0578b_Info.DisplayStatus.LcdWantPt=DecodeInfo->LcdWantPt;
	gVc0578b_Info.DisplayStatus.LcdWantSize=DecodeInfo->LcdWantSize;
	gVc0578b_Info.DisplayStatus.JpegCallBack=DecodeInfo->JpegCallBack;
		
	//check display size>{4,4},must less than (LCD size),get display size
	/*if(gVc0578b_Info.LcdStatus.ARotationMode%2)
	{
		result = VIM_MAPI_CheckDisplaySize(gVc0578b_Info.DisplayStatus.LcdWantPt.x,gVc0578b_Info.DisplayStatus.LcdWantPt.y,gVc0578b_Info.DisplayStatus.LcdWantSize.cy,gVc0578b_Info.DisplayStatus.LcdWantSize.cx,&DisplaySize);
	}
	else
	{*/
		result = VIM_MAPI_CheckDisplaySize(gVc0578b_Info.DisplayStatus.LcdWantPt.x,gVc0578b_Info.DisplayStatus.LcdWantPt.y,gVc0578b_Info.DisplayStatus.LcdWantSize.cx,gVc0578b_Info.DisplayStatus.LcdWantSize.cy,&DisplaySize);
	//}	
	if(result)
		goto DISPLAYERROR;
		
	//from version 0.2 need display whole pic in LCD, so source size =jpeg image size
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]jpeg Image width:	",gVc0578b_JpegInfo.ImageSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]jpeg Image height:  ",gVc0578b_JpegInfo.ImageSize.cy);
	VIM_USER_PrintDec("[VC0578][HAPI]yuv is:  %d",gVc0578b_JpegInfo.YUVType);
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image width:	",gVc0578b_JpegInfo.JpgSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image height:  ",gVc0578b_JpegInfo.JpgSize.cy);
#endif	
	//get real display size(display image in user define display size, maybe not full display in LCD, only part of user define display size)
		
	if(VIM_USER_DISPLAY_FULLSCREEN)
		result = VIM_IPP_ToolCaculateBigDstWindow(gVc0578b_JpegInfo.JpgSize,DisplaySize,&DestDisplaySize);
	else
		result = VIM_IPP_ToolCaculateLessDisplayWindow(gVc0578b_JpegInfo.JpgSize,DisplaySize,&DestDisplaySize);
	
	if(result)
		goto DISPLAYERROR;
			
			//set ipp module image size
#if(BOARD_TYPE==VC0820_FPGA)
	VIM_IPP_SetImageSize(gVc0578b_JpegInfo.JpgSize.cx,gVc0578b_JpegInfo.JpgSize.cy);
#else
	VIM_IPP_SetImageSize(gVc0578b_JpegInfo.ImageSize.cx,gVc0578b_JpegInfo.ImageSize.cy);
#endif
		
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize height:	",DestDisplaySize.cy);
#endif	
	
//set ipp module source window size, display size
	result = VIM_IPP_SetCaptureSize(NULL,pt,WindowSize, DestDisplaySize);
	if(result)
	goto DISPLAYERROR;
				
	//set LCDC A layer memory size and window size
	pt.x=pt.y=0;
	if(DestDisplaySize.cx>DisplaySize.cx)	
		pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
	if(DestDisplaySize.cy>DisplaySize.cy)	
		pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
#endif		

    result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
	if(result)
		goto DISPLAYERROR;
			
	pt.x=(gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	pt.y=(gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;

	if(DestDisplaySize.cx<=DisplaySize.cx)	
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	else
		DestDisplaySize.cx=DisplaySize.cx;
	if(DestDisplaySize.cy<=DisplaySize.cy)	
		pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;
	else
		DestDisplaySize.cy=DisplaySize.cy;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset y:  ",pt.y);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize height:  ",DestDisplaySize.cy);
#endif	
		
	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);
	DestDisplaySizeA1.cx=0;
	DestDisplaySizeA1.cy=0;
	VIM_DISP_SetA1_DisplayPanel(pt,DestDisplaySizeA1);
	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
							VIM_DISP_NOTCHANGE);
	if(result)
		goto DISPLAYERROR;
	
	VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
		
	//enable a layer
	VIM_DISP_ResetState();
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
			
	//set layer A mode, normal display use ALAYER_LINEBUF mode
	gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_DISPLAY_USELINEBUF;

	result = VIM_MAPI_SetDisplayJpegParameter();
	if(result)
		goto DISPLAYERROR;
	switch(DecodeInfo->DisplayMode)
	{
		case VIM_HAPI_DISPLAY_JPEG:
		case VIM_HAPI_DISPLAY_MOTIONJPEG:	
			VIM_MAPI_InitDispInterrupt();;
			break;
		case VIM_HAPI_DISPLAY_YUV:	
			VIM_MAPI_ClearDispInterrupt();
			break;
		default:
			break;
	}
	

DISPLAYERROR:
	if(result)
	{
		return (UINT16)result;
	}
	return VIM_SUCCEED;
}
#endif

#if 0
static __align(4) unsigned char mp4d_data[536] = {
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x20, 0x00, 0xC8, 0x88, 0x80, 0x0F, 0x50, 0x20, 0x40, 
	0x81, 0x44, 0x60, 0x00, 0x00, 0x01, 0xB6, 0x10, 0x61, 0x56, 0x17, 0x19, 0x57, 0xF9, 0x4A, 0xB9, 
	0xC8, 0x15, 0xAC, 0xBB, 0x74, 0xB7, 0x6F, 0x49, 0x09, 0x96, 0x0C, 0xC1, 0x74, 0xB4, 0xA5, 0x94, 
	0x36, 0xA2, 0xE1, 0x57, 0xFA, 0x7B, 0x92, 0xBA, 0x91, 0xD3, 0xD4, 0x1D, 0xC7, 0xA8, 0x66, 0x76, 
	0x64, 0x40, 0x8A, 0x7A, 0x82, 0x2E, 0x11, 0x27, 0x37, 0x94, 0xD5, 0x46, 0x12, 0x16, 0xE9, 0x54, 
	0x25, 0x3A, 0x69, 0x4D, 0x5F, 0x4D, 0xDB, 0xC2, 0x84, 0x4E, 0x93, 0xC5, 0xF4, 0x3C, 0xFE, 0x02, 
	0xC1, 0x77, 0x8E, 0xD8, 0x80, 0x6F, 0x3F, 0x20, 0x0E, 0x07, 0x50, 0x94, 0x60, 0x06, 0xF1, 0xBF, 
	0x59, 0x97, 0xCB, 0xF5, 0x7E, 0xA2, 0x3F, 0x51, 0x49, 0x2F, 0x1D, 0x63, 0x76, 0x5B, 0x9F, 0x0F, 
	0x72, 0xF5, 0x75, 0xC9, 0x2B, 0x93, 0xDC, 0x53, 0x0A, 0x51, 0x94, 0x9B, 0x13, 0x9C, 0xF2, 0xAF, 
	0xB0, 0xAF, 0x7F, 0xDA, 0x59, 0xB2, 0x4B, 0xE4, 0x12, 0x9B, 0x70, 0xB7, 0x2C, 0x7A, 0xDE, 0x10, 
	0x57, 0x5A, 0xFB, 0x2E, 0xE5, 0x88, 0x71, 0x11, 0x29, 0x39, 0x9E, 0xDB, 0x78, 0x6E, 0x9E, 0x27, 
	0x7D, 0x3A, 0x16, 0x9D, 0xAA, 0x94, 0x17, 0xFA, 0x71, 0x6D, 0xAA, 0x60, 0xC9, 0x71, 0x71, 0xE6, 
	0x54, 0xC5, 0xB6, 0x2F, 0x61, 0xA9, 0xC7, 0x4B, 0x71, 0x47, 0x0D, 0x40, 0x99, 0x09, 0x08, 0xD3, 
	0x1F, 0xCE, 0x2A, 0x66, 0xC5, 0xA9, 0x65, 0x90, 0xA7, 0x8B, 0x05, 0x6B, 0x17, 0x7E, 0xD8, 0xDC, 
	0x20, 0x36, 0x0D, 0xD5, 0x69, 0xD9, 0xD8, 0x55, 0x3E, 0xA3, 0xC6, 0xAF, 0x01, 0xD2, 0x3C, 0x8A, 
	0x5B, 0x2F, 0xE7, 0x03, 0x8E, 0x8D, 0x50, 0x1F, 0x31, 0x83, 0xE4, 0xCC, 0x31, 0x56, 0xDD, 0xCE, 
	0xF4, 0x96, 0xA3, 0xA1, 0x59, 0x9D, 0xEC, 0xDB, 0x2D, 0xED, 0x72, 0x6D, 0xED, 0x28, 0x39, 0x5C, 
	0x98, 0xAF, 0xC1, 0xEF, 0x4D, 0x70, 0xE1, 0x9E, 0x2F, 0x54, 0x2D, 0xB8, 0x8D, 0xC9, 0x26, 0xB3, 
	0x67, 0x10, 0xC2, 0x32, 0xC9, 0x58, 0x2B, 0x8C, 0xB4, 0x37, 0xF2, 0xDB, 0x03, 0x9C, 0x01, 0xF0, 
	0x82, 0xC3, 0xBD, 0x97, 0x69, 0xB5, 0x25, 0x5A, 0xB3, 0xC5, 0x15, 0xA9, 0x66, 0x7D, 0x0E, 0x77, 
	0x83, 0x39, 0xC7, 0xD7, 0xDA, 0xEA, 0xF1, 0x1A, 0x37, 0x76, 0x9B, 0x53, 0xEB, 0x2A, 0x38, 0x47, 
	0x6A, 0x75, 0x30, 0xE2, 0x09, 0x71, 0x48, 0xE1, 0xA0, 0xE7, 0xF2, 0xD0, 0xD3, 0x17, 0x7C, 0x10, 
	0x65, 0x7B, 0x33, 0xF3, 0x50, 0xCC, 0xB3, 0x06, 0x75, 0x70, 0xA3, 0xCB, 0xCB, 0xD2, 0x6E, 0xC4, 
	0xEA, 0xD5, 0x66, 0xE5, 0xBB, 0x11, 0x40, 0xA0, 0x6E, 0xDE, 0x28, 0xFD, 0x85, 0x03, 0x1E, 0x1D, 
	0x31, 0xFF, 0xFF, 0xFA, 0xD5, 0xBB, 0x14, 0x7B, 0xD7, 0xED, 0xC9, 0x57, 0xBD, 0x1B, 0x76, 0x1A, 
	0x5A, 0x89, 0xEA, 0xDE, 0xDB, 0x6A, 0x3A, 0x74, 0xD5, 0xBA, 0x9E, 0x2C, 0x1B, 0x73, 0xA8, 0x6D, 
	0x87, 0xCB, 0xCB, 0xA6, 0xE5, 0x19, 0x04, 0x6C, 0x2D, 0x50, 0xB4, 0xF3, 0xCB, 0xE8, 0xE2, 0xFD, 
	0x71, 0x84, 0x55, 0x4A, 0x23, 0x6E, 0x1C, 0x08, 0x62, 0x4F, 0xD3, 0x2A, 0xDB, 0x9C, 0x9B, 0xB6, 
	0x67, 0x97, 0xE9, 0x5D, 0x42, 0x32, 0x23, 0x91, 0xF5, 0x4F, 0xB6, 0x6F, 0xF5, 0x4E, 0x6F, 0xA7, 
	0x50, 0x72, 0x9E, 0x95, 0x72, 0xC6, 0x9E, 0xBE, 0x4D, 0x9F, 0xB4, 0xAE, 0x40, 0xC1, 0xC5, 0x99, 
	0x62, 0x5F, 0x28, 0x0C, 0xA9, 0x10, 0xC2, 0x83, 0xC1, 0x7F, 0xC3, 0xE6, 0x94, 0x0C, 0x76, 0xED, 
	0xC8, 0x13, 0xBE, 0xBA, 0x2E, 0xA5, 0xCB, 0x2E, 0x5E, 0x2E, 0x4B, 0xAD, 0xEF, 0x01, 0x1A, 0x72, 
	0x49, 0xC1, 0x9C, 0x71, 0x7B, 0x22, 0x69, 0x40, 0x7D, 0x8E, 0x35, 0x7E, 0xA9, 0x74, 0x76, 0x22, 
	0x7F, 0xB8, 0xDF, 0x17, 0x98, 0xF8, 0xEA, 0xff
};

/***************************************************************************************************************************************
Description:
	display one frame YUV data on LCD,not uesd jpeg module ,but used mpeg4 dec module
Parameters:
      PVIM_JPEG_DECODE_INFO DecodeInfo:recode input information
Return:
	VIM_SUCCEED:  ok
	
	VIM_ERROR_?????????: return form mid api
Note:
	
Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_DisplayYUVML(PVIM_JPEG_DECODE_INFO DecodeInfo)
{
	VIM_RESULT result;
	TSize DisplaySize,DestDisplaySize,WindowSize,DestDisplaySizeA1;
	VIM_IPP_EFFECT Effect;
	TPoint pt = {0, 0};
	UINT8 *mp4bit;
	UINT32 BitStreamAddr;
	UINT32 BitLength;
	UINT32 JpegYUVAddr,temp;
	UINT8  pixelrate;
	UINT16 blank;
	
	mp4bit=mp4d_data;
	BitStreamAddr=(UINT32)mp4bit;
	BitLength=536;
						
	//disable syncgen
	VIM_SIF_EnableSyncGen(DISABLE); //angela 2006-8-15
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON&&gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_PHOTOPROCESS) 
		return VIM_ERROR_WORKMODE;

	WindowSize=DecodeInfo->JpegWinSize;
	pt=DecodeInfo->JpegOffsetPt;
	JpegYUVAddr=DecodeInfo->JpegLbufAddr;

	Effect=(VIM_IPP_EFFECT)DecodeInfo->EffectMode;
	switch(DecodeInfo->EffectMode)
	{
		case  VIM_HAPI_SPECIAL_MONOCHROME: 
			Effect=VIM_IPP_EFFECT_SPECIALCOLOR;
			VIM_IPP_SetEffectUVThreshold(0,0,0,0);
			break;
		case  VIM_HAPI_SPECIAL_REDONLY: 
			Effect=VIM_IPP_EFFECT_SPECIALCOLOR;
			VIM_IPP_SetEffectUVThreshold(128,85,255,128);
			break;
		case  VIM_HAPI_SPECIAL_SEPIA:
			VIM_IPP_SetEffectUVoffset(85,128);
			break;
		default:
			break;
	}
	VIM_IPP_SetEffect(Effect);
	gVc0578b_Info.PreviewStatus.SpecialEffect=(VIM_HAPI_SPECIAL_EFFECT)DecodeInfo->EffectMode;
	gVc0578b_Info.LcdStatus.ARotationMode=(VIM_DISP_ROTATEMODE)DecodeInfo->RotationMode;
	// set all module to display jpeg mode
		
	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;
	
	VIM_USER_MemCpy((UINT8 *)&gVc0578b_JpegInfo,(UINT8 *)&(DecodeInfo->ImageInfo),sizeof(gVc0578b_JpegInfo));	
	gVc0578b_Info.DisplayStatus.LcdWantPt=DecodeInfo->LcdWantPt;
	gVc0578b_Info.DisplayStatus.LcdWantSize=DecodeInfo->LcdWantSize;
	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;
	//set mode
	VIM_MAPI_SetChipMode(VIM_MARB_DISPLAYDEBUG_MODE,VIM_IPP_HAVE_NOFRAME);

	//check display size>{4,4},must less than (LCD size),get display size
	if(gVc0578b_Info.LcdStatus.ARotationMode%2)
	{
		result = VIM_MAPI_CheckDisplaySize(gVc0578b_Info.DisplayStatus.LcdWantPt.x,gVc0578b_Info.DisplayStatus.LcdWantPt.y,gVc0578b_Info.DisplayStatus.LcdWantSize.cy,gVc0578b_Info.DisplayStatus.LcdWantSize.cx,&DisplaySize);
	}
	else
	{
		result = VIM_MAPI_CheckDisplaySize(gVc0578b_Info.DisplayStatus.LcdWantPt.x,gVc0578b_Info.DisplayStatus.LcdWantPt.y,gVc0578b_Info.DisplayStatus.LcdWantSize.cx,gVc0578b_Info.DisplayStatus.LcdWantSize.cy,&DisplaySize);
	}	
	if(result)
		goto DISPLAYERROR;
		
	//from version 0.2 need display whole pic in LCD, so source size =jpeg image size
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]jpeg Image width:	",gVc0578b_JpegInfo.ImageSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]jpeg Image height:  ",gVc0578b_JpegInfo.ImageSize.cy);
	VIM_USER_PrintDec("[VC0578][HAPI]yuv is:  %d",gVc0578b_JpegInfo.YUVType);
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image width:	",gVc0578b_JpegInfo.JpgSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image height:  ",gVc0578b_JpegInfo.JpgSize.cy);
#endif	
	//get real display size(display image in user define display size, maybe not full display in LCD, only part of user define display size)
		
	if(VIM_USER_DISPLAY_FULLSCREEN)
		result = VIM_IPP_ToolCaculateBigDstWindow(gVc0578b_JpegInfo.ImageSize,DisplaySize,&DestDisplaySize);
	else
		result = VIM_IPP_ToolCaculateLessDisplayWindow(gVc0578b_JpegInfo.ImageSize,DisplaySize,&DestDisplaySize);
	
	if(result)
		goto DISPLAYERROR;
			
			//set ipp module image size
#if(BOARD_TYPE==VC0820_FPGA)
	VIM_IPP_SetImageSize(gVc0578b_JpegInfo.JpgSize.cx,gVc0578b_JpegInfo.JpgSize.cy);
#else
	VIM_IPP_SetImageSize(gVc0578b_JpegInfo.ImageSize.cx,gVc0578b_JpegInfo.ImageSize.cy);
#endif
		
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize height:	",DestDisplaySize.cy);
#endif	
	
//set ipp module source window size, display size
#if(BOARD_TYPE==VC0820_FPGA)
	result = VIM_IPP_SetCaptureSize(NULL,pt,WindowSize, DestDisplaySize);
#else
	result = VIM_IPP_SetCaptureSize(NULL,pt,WindowSize, DestDisplaySize);
#endif
	if(result)
	goto DISPLAYERROR;
				
	//set LCDC A layer memory size and window size
	pt.x=pt.y=0;
	if(DestDisplaySize.cx>DisplaySize.cx)	
		pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
	if(DestDisplaySize.cy>DisplaySize.cy)	
		pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
#endif		

    result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
	if(result)
		goto DISPLAYERROR;
			
	pt.x=(gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	pt.y=(gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;

	if(DestDisplaySize.cx<=DisplaySize.cx)	
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	else
		DestDisplaySize.cx=DisplaySize.cx;
	if(DestDisplaySize.cy<=DisplaySize.cy)	
		pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;
	else
		DestDisplaySize.cy=DisplaySize.cy;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset y:  ",pt.y);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize height:  ",DestDisplaySize.cy);
#endif	
		
	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);
	DestDisplaySizeA1.cx=0;
	DestDisplaySizeA1.cy=0;
	VIM_DISP_SetA1_DisplayPanel(pt,DestDisplaySizeA1);
	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
							VIM_DISP_NOTCHANGE);
	if(result)
		goto DISPLAYERROR;
	
	VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
		
	//enable a layer
	VIM_DISP_ResetState();
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
			
	//set layer A mode, normal display use ALAYER_LINEBUF mode
	gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_DISPLAY_USELINEBUF;
	
	VIM_HIF_SetReg32(V5_REG_MP4DEC_RESET, 0);	
	
	
	//(12 )set mp4 dec module image size
	
	VIM_HIF_SetReg8(V5_REG_MP4DEC_PINGPANG_MBL, (64>>4));
	VIM_HIF_SetReg8(V5_REG_MP4DEC_PINGPANG_MBL+1, (64>>4));
	
	VIM_HIF_SetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE, gVc0578b_JpegInfo.JpgSize.cx);
	VIM_HIF_SetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE+2, gVc0578b_JpegInfo.JpgSize.cy);
	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE,gVc0578b_JpegInfo.JpgSize.cx);
	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE+2,gVc0578b_JpegInfo.JpgSize.cy);

	temp=((gVc0578b_JpegInfo.JpgSize.cx*gVc0578b_JpegInfo.JpgSize.cy+63)>>6);
 	VIM_HIF_SetReg32(V5_REG_LBUF_BURSTNUMBER0, temp);

 	//(14) marb map memory
	result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
			(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
	if(result)
		return result;
	VIM_HIF_SetReg32(V5_REG_LBUF_Y_INI0_DEC,JpegYUVAddr);
	VIM_HIF_SetReg32(V5_REG_LBUF_Y_INI1_DEC,0);
		// y1
		
	//(15) set interrupt
	VIM_HIF_ClearIntModule(VM_HIF_INT_VDEC);
	
	VIM_MPEG4_RegisterISR(VIM_MPEG4_INT_ALL,NULL);
		
	//set mpeg 
	VIM_MPEG4_RegisterISR(VIM_INT_MPEG4_DEC_DONE,VIM_MAPI_ISR_MpegDecDoneDebug);
	VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_DEC_DONE,ENABLE);
	
	//(16) pisel rate

	pixelrate =7;
	VIM_JPEG_SetPixelRate(pixelrate);
	blank = 2;
	VIM_JPEG_SetLineBlank(blank);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_INI_ADD,BitStreamAddr);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_SIZE,BitLength);
	VIM_MPEG4_StartDec(0x0f);
	
DISPLAYERROR:
	if(result)
	{
		return (UINT16)result;
	}
	return VIM_SUCCEED;
}

/***************************************************************************************************************************************
Description:
	display one frame YUV data on LCD,not use jpeg module ,but use mpeg4 dec module, for RMVB application
Parameters:
      PVIM_JPEG_DECODE_INFO DecodeInfo:recode input information
Return:
	VIM_SUCCEED:  ok
		VIM_ERROR_?????????: return form mid api
Note:
	
Remarks:
state: 
	valid
****************************************************************************************************************************************/
void VIM_HAPI_InitDisplayRM(void)
{
	gVc0578b_Info.DisplayStatus.RMFrameTimes=0;
	gVc0578b_Info.DisplayStatus.LCDCA0done=0;

	return;
}



/***************************************************************************************************************************************
Description:
	display one frame YUV data on LCD,not use jpeg module ,but use mpeg4 dec module, for RMVB application
Parameters:
      PVIM_JPEG_DECODE_INFO DecodeInfo:recode input information
Return:
	VIM_SUCCEED:  ok
		VIM_ERROR_?????????: return form mid api
Note:
	
Remarks:
state: 
	valid
****************************************************************************************************************************************/

UINT32 VIM_HAPI_DisplayYUVMLForRM(PVIM_JPEG_DECODE_INFO DecodeInfo)
{
#if 1
	VIM_RESULT result;
	TSize DisplaySize,DestDisplaySize,ImageSize;
	TPoint pt = {0, 0};
	UINT32 JpegYUVAddr,temp;
	UINT8 *mp4bit;
	UINT32 BitStreamAddr;
	UINT32 BitLength;
	UINT8  pixelrate;
	UINT16 blank;
	
	ImageSize=DecodeInfo->ImageInfo.JpgSize;					
	JpegYUVAddr=DecodeInfo->JpegLbufAddr;
	mp4bit=mp4d_data;
//modify by zhoujian for SDRAM Remap,2008.02.14
	//BitStreamAddr=(UINT32)mp4bit;
	BitStreamAddr=(UINT32)((UINT32)mp4bit|0x40000000);
	BitLength=536;
	
	gVc0578b_Info.LcdStatus.ARotationMode=0;
	gVc0578b_Info.DisplayStatus.LcdWantPt=DecodeInfo->LcdWantPt;
	gVc0578b_Info.DisplayStatus.LcdWantSize=DecodeInfo->LcdWantSize;

	//set mode
	VIM_MAPI_SetChipMode(VIM_MARB_DISPLAYDEBUG_MODE,VIM_IPP_HAVE_NOFRAME);
	
	result = VIM_MAPI_CheckDisplaySize(gVc0578b_Info.DisplayStatus.LcdWantPt.x,gVc0578b_Info.DisplayStatus.LcdWantPt.y,gVc0578b_Info.DisplayStatus.LcdWantSize.cx,gVc0578b_Info.DisplayStatus.LcdWantSize.cy,&DisplaySize);
	if(result)
		goto DISPLAYERROR;
		
	//from version 0.2 need display whole pic in LCD, so source size =jpeg image size
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image width:	",ImageSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image height:  ",ImageSize.cy);
#endif	
	//get real display size(display image in user define display size, maybe not full display in LCD, only part of user define display size)
	result = VIM_IPP_ToolCaculateBigDstWindow(ImageSize,DisplaySize,&DestDisplaySize);
	if(result)
		goto DISPLAYERROR;
		
	//set ipp module image size
	VIM_IPP_SetImageSize(ImageSize.cx,ImageSize.cy);		
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize height:	",DestDisplaySize.cy);
#endif	

	result = VIM_IPP_SetCaptureSize(NULL,pt,ImageSize, DestDisplaySize);
	if(result)
	goto DISPLAYERROR;
				
	//set LCDC A layer memory size and window size
	pt.x=pt.y=0;
	if(DestDisplaySize.cx>DisplaySize.cx)	
		pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
	if(DestDisplaySize.cy>DisplaySize.cy)	
		pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
#endif		

    result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
	if(result)
		goto DISPLAYERROR;
			
	pt.x=(gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	pt.y=(gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;

	if(DestDisplaySize.cx<=DisplaySize.cx)	
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	else
		DestDisplaySize.cx=DisplaySize.cx;
	if(DestDisplaySize.cy<=DisplaySize.cy)	
		pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;
	else
		DestDisplaySize.cy=DisplaySize.cy;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset y:  ",pt.y);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize height:  ",DestDisplaySize.cy);
#endif	
		
	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);
	
	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
							VIM_DISP_NOTCHANGE);
	if(result)
		goto DISPLAYERROR;
	
	VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
		
	//enable a layer
	VIM_DISP_ResetState();
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
			
	//set layer A mode, normal display use ALAYER_LINEBUF mode
	gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_DISPLAY_USELINEBUF;
	VIM_HIF_SetReg32(V5_REG_MP4DEC_RESET, 0);	
	//(12 )set mp4 dec module image size
	VIM_HIF_SetReg8(V5_REG_MP4DEC_PINGPANG_MBL, (64>>4));
	VIM_HIF_SetReg8(V5_REG_MP4DEC_PINGPANG_MBL+1, (64>>4));
	VIM_HIF_SetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE, ImageSize.cx);
	VIM_HIF_SetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE+2, ImageSize.cy);
	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE,ImageSize.cx);
	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE+2,ImageSize.cy);

	temp=((ImageSize.cx*ImageSize.cy+63)>>6);
 	VIM_HIF_SetReg32(V5_REG_LBUF_BURSTNUMBER0, temp);

 	//(14) marb map memory
	result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
			(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
	if(result)
		return result;
	
	VIM_HIF_SetReg32(V5_REG_LBUF_Y_INI0_DEC,JpegYUVAddr);
	VIM_HIF_SetReg32(V5_REG_LBUF_Y_INI1_DEC,0);
		
	//(15) set interrupt
	
	VIM_HIF_ClearIntModule(VM_HIF_INT_VDEC);
	VIM_HIF_ClearIntModule(VM_HIF_INT_LCDC);
	VIM_MPEG4_RegisterISR(VIM_MPEG4_INT_ALL,NULL);
	//VIM_DISP_RegisterISR(VIM_INT_DISP_ALL,NULL);
		
	//set mpeg 
	VIM_MPEG4_RegisterISR(VIM_INT_MPEG4_DEC_DONE,VIM_MAPI_ISR_MpegDecDoneDebug);
	VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_DEC_DONE,ENABLE);
	VIM_DISP_RegisterISR(VIM_INT_DISP_A0_DONE,VIM_MAPI_ISR_LCDCA0DoneDebug);
	VIM_DISP_SetIntEnable(VIM_INT_DISP_A0_DONE,ENABLE);
	
	//(16) pisel rate
	pixelrate =2;
	VIM_JPEG_SetPixelRate(pixelrate);
	blank = 0;
	VIM_JPEG_SetLineBlank(blank);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_INI_ADD,BitStreamAddr);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_SIZE,BitLength);
	VIM_MPEG4_StartDec(0x0f);
	
DISPLAYERROR:
	if(result)
	{
		return (UINT16)result;
	}
	return VIM_SUCCEED;

#else
	VIM_RESULT result;
	UINT32 JpegYUVAddr,temp,BitStreamAddr,BitLength;
	UINT8 *mp4bit;
	UINT8  pixelrate;
	UINT16 blank;
	TSize DisplaySize,DestDisplaySize,ImageSize;
	TPoint pt = {0, 0};
	TPoint WindowPt = {0, 0};
	TPoint DisplayPt = {0, 0};
	
	// 1. disable syncgen
	VIM_SIF_EnableSyncGen(DISABLE); //angela 2006-8-15
	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;

	// 2. check frame times
	gVc0578b_Info.DisplayStatus.RMFrameTimes++;

	if((gVc0578b_Info.DisplayStatus.RMFrameTimes==1)||((gVc0578b_Info.DisplayStatus.RMFrameTimes>1)&&(gVc0578b_Info.DisplayStatus.LCDCA0done==1)))
	{
      	gVc0578b_Info.DisplayStatus.LCDCA0done=0;
      	// 3. get image information
      	ImageSize.cx=DecodeInfo->ImageInfo.JpgSize.cx;	
      	ImageSize.cy=DecodeInfo->ImageInfo.JpgSize.cy; 
      	JpegYUVAddr=DecodeInfo->JpegLbufAddr;
      
      	// 4. get mpeg4 bitstream
      	mp4bit=mp4d_data;
      	//modify by zhoujian for SDRAM Remap,2008.02.14
      	//BitStreamAddr=(UINT32)mp4bit;
      	BitStreamAddr=(UINT32)((UINT32)mp4bit|0x40000000);
      	BitLength=536;
      
      	// 5. set a layer rotation mode
      	gVc0578b_Info.LcdStatus.ARotationMode=0;
      
      	// 6. get display size and position(display size<=lcd size)
      	DisplaySize.cx = DecodeInfo->LcdWantSize.cx;
      	DisplaySize.cy = DecodeInfo->LcdWantSize.cy;
      	DisplayPt.x = DecodeInfo->LcdWantPt.x;
      	DisplayPt.y = DecodeInfo->LcdWantPt.y;
      	/*DisplaySize.cx=(DisplaySize.cx&0xfffc);	
      		DisplaySize.cy=(DisplaySize.cy&0xfffc);
     	 	DisplayPt.x=(DisplayPt.x&0xfffc);	
      		DisplayPt.y=(DisplayPt.y&0xfffc);*/
        
      	result = VIM_MAPI_CheckDisplaySize(DisplayPt.x,DisplayPt.y,DisplaySize.cx,DisplaySize.cy,&DestDisplaySize);
      	if(result)
      		goto DISPLAYERROR;
      	DisplaySize.cx = gVc0578b_Info.DisplayStatus.LcdWantSize.cx;
      	DisplaySize.cy = gVc0578b_Info.DisplayStatus.LcdWantSize.cy;
      	DisplayPt.x = gVc0578b_Info.DisplayStatus.LcdWantPt.x;
      	DisplayPt.y = gVc0578b_Info.DisplayStatus.LcdWantPt.y;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
      	VIM_USER_PrintDec("[VC0578][HAPI]display dest width: ",DisplaySize.cx);
      	VIM_USER_PrintDec("[VC0578][HAPI]display dest height:",DisplaySize.cy);
      	VIM_USER_PrintDec("[VC0578][HAPI]display dest xpos: ",DisplayPt.x);
      	VIM_USER_PrintDec("[VC0578][HAPI]display dest ypos: ",DisplayPt.y);
#endif	
      	
      	// 8. set RM display mode
      	VIM_MAPI_SetChipMode(VIM_MARB_DISPLAYDEBUG_MODE,VIM_IPP_HAVE_NOFRAME);
      	
      	// 9. get real display size(display image in user define display size, maybe not full display in LCD, only part of user define display size)
      	result = VIM_IPP_ToolCaculateBigDstWindow(ImageSize,DisplaySize,&DestDisplaySize);
      	if(result)
      	goto DISPLAYERROR;
      	
      	if(ImageSize.cx<DestDisplaySize.cx)
      	{
      		pixelrate =5;
      	}
      	else
      	{
      		pixelrate =2;
      	}
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
      	VIM_USER_PrintDec("[VC0578][HAPI]caculate dest display size width:	",DestDisplaySize.cx);
      	VIM_USER_PrintDec("[VC0578][HAPI]caculate dest display size height: ",DestDisplaySize.cy);
#endif	
      	
      	//10. set ipp module image size
      	VIM_IPP_SetImageSize(ImageSize.cx,ImageSize.cy);		
         
      	//11.set source window size, display size(ipp module)
      	result = VIM_IPP_SetCaptureSize(NULL,pt,ImageSize, DestDisplaySize);
      	if(result)
      	goto DISPLAYERROR;
      		
      	// 12.set LCDC A layer memory size and window size
      	pt.x=pt.y=0;
      	if(DestDisplaySize.cx>DisplaySize.cx)	
      	pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
      	if(DestDisplaySize.cy>DisplaySize.cy)	
      	pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;
      	
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
      	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
      	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
#endif		
      	result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
      	if(result)
      	goto DISPLAYERROR;
      	
      	pt.x=(gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
      	pt.y=(gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;
      	
      	if(DestDisplaySize.cx<=DisplaySize.cx)	
      	pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
      	else
      	DestDisplaySize.cx=DisplaySize.cx;
      	if(DestDisplaySize.cy<=DisplaySize.cy)	
      	pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;
      	else
      	DestDisplaySize.cy=DisplaySize.cy;
      	
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
      	VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
      	VIM_USER_PrintDec("[VC0578][HAPI]a display offset y:  ",pt.y);
      	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize width:  ",DestDisplaySize.cx);
      	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize height:  ",DestDisplaySize.cy);
#endif	
      	
      	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);
      	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
      					VIM_DISP_NOTCHANGE);
      	if(result)
      	goto DISPLAYERROR;
      	
      	VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
      	
      	//enable a layer
      	VIM_DISP_ResetState();
      	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
      	
      	//set layer A mode, normal display use ALAYER_LINEBUF mode
      	gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_DISPLAY_USELINEBUF;
      
      	//13.set mp4 dec module and lbuf module
      	VIM_HIF_SetReg32(V5_REG_MP4DEC_RESET, 0);	
      	VIM_HIF_SetReg8(V5_REG_MP4DEC_PINGPANG_MBL, (64>>4));
      	VIM_HIF_SetReg8(V5_REG_MP4DEC_PINGPANG_MBL+1, (64>>4));
      	VIM_HIF_SetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE, ImageSize.cx);
      	VIM_HIF_SetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE+2, ImageSize.cy);
      	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE,ImageSize.cx);
      	VIM_HIF_SetReg16(V5_REG_LBUF_IMAGESIZE+2,ImageSize.cy);
      	temp=((ImageSize.cx*ImageSize.cy+63)>>6);
      	VIM_HIF_SetReg32(V5_REG_LBUF_BURSTNUMBER0, temp);
      	
      	//14.marb map memory
      	result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
      	(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
      	if(result)
      	return result;
      	VIM_HIF_SetReg32(V5_REG_LBUF_Y_INI0_DEC,JpegYUVAddr);
      	VIM_HIF_SetReg32(V5_REG_LBUF_Y_INI1_DEC,0);
      	
      	//15. set interrupt
      	VIM_HIF_ClearIntModule(VM_HIF_INT_VDEC);
      	VIM_HIF_ClearIntModule(VM_HIF_INT_LCDC);
      	VIM_MPEG4_RegisterISR(VIM_MPEG4_INT_ALL,NULL);
      	VIM_DISP_RegisterISR(VIM_INT_DISP_ALL,NULL);
      	
      	//set mpeg 
      	VIM_MPEG4_RegisterISR(VIM_INT_MPEG4_DEC_DONE,VIM_MAPI_ISR_MpegDecDoneDebug);
      	VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_DEC_DONE,ENABLE);
      	VIM_DISP_RegisterISR(VIM_INT_DISP_A0_DONE,VIM_MAPI_ISR_LCDCA0DoneDebug);
      	VIM_DISP_SetIntEnable(VIM_INT_DISP_A0_DONE,ENABLE);
      	
      	//16. set pisel rate
      	VIM_JPEG_SetPixelRate(pixelrate);
      	blank = 0;
      	VIM_JPEG_SetLineBlank(blank);
      	
      	//17. set mpg4 module bitstream addr
      	VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_INI_ADD,BitStreamAddr);
      	VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_SIZE,BitLength);
      	VIM_MPEG4_StartDec(0x0f);
DISPLAYERROR:
      	if(result)
      	{
      		return (UINT16)result;
      	}
      	return VIM_SUCCEED;
	}
	else
	{
		return VIM_SUCCEED;
	}

#endif
}


//add by youhai 2008.1.30 begin	
UINT32 VIM_HAPI_GetRMVBDecInfo(PVIM_VIDEO_OUTINFO pDecInfo)
{
	TSize DispalySize,ImageSize;
	TPoint DispalyPoint;
	
	if(pDecInfo==NULL)
		return VIM_ERROR_DECODE_ERROR;
	VIM_DISP_GetAWinsize(&DispalySize);
	DispalyPoint.x=VIM_HIF_GetReg16(V5_REG_LCDC_A0_LCD_POS+2);
	DispalyPoint.y=VIM_HIF_GetReg16(V5_REG_LCDC_A0_LCD_POS);
	VIM_IPP_GetImageSize(&ImageSize.cx,&ImageSize.cy);

	pDecInfo->v_outeffect=8;
	pDecInfo->v_outrotation=0;
	pDecInfo->v_outheight=DispalySize.cy;
	pDecInfo->v_outwidth=DispalySize.cx;
	pDecInfo->v_outxpos=DispalyPoint.x;
	pDecInfo->v_outypos=DispalyPoint.y;

	pDecInfo->v_srcheight=ImageSize.cy;
	pDecInfo->v_srcwidth=ImageSize.cx;
	pDecInfo->v_srcxpos=0;
	pDecInfo->v_srcypos=0;
	return VIM_SUCCEED;
}


/***************************************************************************************************************************************
Description:
	display one frame YUV data on LCD,not use jpeg module ,but use mpeg4 dec module, for RMVB application
Parameters:
      PVIM_JPEG_DECODE_INFO DecodeInfo:recode input information
Return:
	VIM_SUCCEED:  ok
		VIM_ERROR_?????????: return form mid api
Note:
	
Remarks:
state: 
	valid
****************************************************************************************************************************************/

UINT32 VIM_HAPI_DisplayYUVForRM(PVIM_JPEG_DECODE_INFO DecodeInfo)
{
	VIM_RESULT result;
	TSize DisplaySize,DestDisplaySize;

	TPoint pt = {0, 0};
	
	gVc0578b_Info.LcdStatus.ARotationMode=0;
	gVc0578b_Info.DisplayStatus.LcdWantPt=DecodeInfo->LcdWantPt;
	gVc0578b_Info.DisplayStatus.LcdWantSize=DecodeInfo->LcdWantSize;
	gVc0578b_JpegInfo.ImageSize=DecodeInfo->ImageInfo.ImageSize;
	gVc0578b_JpegInfo.JpgSize=DecodeInfo->ImageInfo.JpgSize;
	gVc0578b_JpegInfo.YUVType=DecodeInfo->ImageInfo.YUVType;
	//set mode
	VIM_MAPI_SetDisplayYUVMode();
	//VIM_HIF_SetReg32(0x60010118,0x00000018);
		
	result = VIM_MAPI_CheckDisplaySize(gVc0578b_Info.DisplayStatus.LcdWantPt.x,gVc0578b_Info.DisplayStatus.LcdWantPt.y,gVc0578b_Info.DisplayStatus.LcdWantSize.cx,gVc0578b_Info.DisplayStatus.LcdWantSize.cy,&DisplaySize);
	if(result)
		goto DISPLAYERROR;
	
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]jpeg Image width:	",gVc0578b_JpegInfo.ImageSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]jpeg Image height:  ",gVc0578b_JpegInfo.ImageSize.cy);
	VIM_USER_PrintDec("[VC0578][HAPI]yuv is:  %d",gVc0578b_JpegInfo.YUVType);
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image width:	",gVc0578b_JpegInfo.JpgSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image height:  ",gVc0578b_JpegInfo.JpgSize.cy);
#endif		
	
	//get real display size(display image in user define display size, maybe not full display in LCD, only part of user define display size)
	result = VIM_IPP_ToolCaculateBigDstWindow(gVc0578b_JpegInfo.JpgSize,DisplaySize,&DestDisplaySize);
	if(result)
		goto DISPLAYERROR;
	//set ipp module image size
	VIM_IPP_SetImageSize(gVc0578b_JpegInfo.JpgSize.cx,gVc0578b_JpegInfo.JpgSize.cy);		

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize height:	",DestDisplaySize.cy);
#endif	

	//set ipp module source window size, display size
	result = VIM_IPP_SetCaptureSize(NULL,pt,gVc0578b_JpegInfo.JpgSize, DestDisplaySize);
	if(result)
		goto DISPLAYERROR;
				
	//set LCDC A layer memory size and window size
	pt.x=pt.y=0;
	if(DestDisplaySize.cx>DisplaySize.cx)	
		pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
	if(DestDisplaySize.cy>DisplaySize.cy)	
		pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
#endif		
    result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
	if(result)
		goto DISPLAYERROR;
			
	pt.x=(gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	pt.y=(gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;

	if(DestDisplaySize.cx<=DisplaySize.cx)	
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	else
		DestDisplaySize.cx=DisplaySize.cx;
	if(DestDisplaySize.cy<=DisplaySize.cy)	
		pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;
	else
		DestDisplaySize.cy=DisplaySize.cy;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset y:  ",pt.y);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize height:  ",DestDisplaySize.cy);
#endif	
		
	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);
	
	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
						VIM_DISP_NOTCHANGE);
	if(result)
		goto DISPLAYERROR;
	VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
	//enable a layer
	VIM_DISP_ResetState();
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
	//set layer A mode, normal display use ALAYER_LINEBUF mode
	gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_DISPLAY_USELINEBUF;
	result = VIM_MAPI_SetDisplayJpegParameter();
	if(result)
		goto DISPLAYERROR;

	VIM_MAPI_ClearDispInterrupt();
	VIM_MAPI_StartDecodeYUVBlock();
DISPLAYERROR:
	if(result)
	{
		return (UINT16)result;
	}
	return VIM_SUCCEED;
}
#endif



/***************************************************************************************************************************************
Description:
	set display mode and parameter, display one frame or more frame jpeg picture on LCD
Parameters:
      Info: 	the struct that used to transform the output infomation
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_WORKMODE: error work mode
	VIM_ERROR_YUV_UNKNOW:jpeg file yuv mode error, can not display
	VIM_ERROR_PARAMETER: marb jbuf size less than capture size, so data is not good
	VIM_ERROR_?????????: return form mid api
Note:
	none
	Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_StartToDisplayJpeg(PVIM_JPEG_DECODE_INFO DecodeInfo)
{
	//UINT32 fifolen;
	
	switch(DecodeInfo->DisplayMode)
	{
		case VIM_HAPI_DISPLAY_JPEG:
			VIM_MAPI_StartDecode();
			break;
		case VIM_HAPI_DISPLAY_MOTIONJPEG:	
			VIM_MAPI_StartDecodeMJpeg();
			break;
		case VIM_HAPI_DISPLAY_YUV:
			VIM_MAPI_StartDecodeYUVBlock();
			break;
		default:
			break;
	}
	return 0;
}

UINT32 VIM_HAPI_DecodeJpeg(HUGE UINT8 *addr,UINT32 rawdatalen)
{
	UINT32 fifolen;
	
	fifolen=VIM_MARB_GetJbufRWSize();
		
	if(fifolen>rawdatalen)
	{
		//return VIM_MAPI_WriteJpegDataToFIFO(addr,rawdatalen,VIM_HIF_FIFO_0_MODE);VIM_HIF_SRAM_MODE
		return VIM_MAPI_WriteJpegDataToFIFO(addr,rawdatalen,VIM_HIF_SRAM_MODE);
	}
	else
	{
		//return VIM_MAPI_WriteJpegDataToFIFO(addr,fifolen,VIM_HIF_FIFO_0_MODE);
		return VIM_MAPI_WriteJpegDataToFIFO(addr,fifolen,VIM_HIF_SRAM_MODE);
		
	}
}

UINT32 VIM_HAPI_GetJpegDecStatus(void)
{
	return VIM_JPEG_GetStatus();
}


void VIM_HAPI_DisplayJpeg_Release(void)
{
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	VIM_JPEG_ResetState();
	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x01);
	VIM_USER_DelayMs(1);
	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x0);
	VIM_IPP_Reset();
	VIM_MAPI_FreeDisplayJpegBuffer();
	gVc0578b_Info.MarbStatus.WorkMode=VIM_MARB_NONE_MODE;
}

#endif


#if VIDEDISP
/********************************************************************************
   		video display related
*******************************************************************************/
/********************************************************************************
Description:
	set video decoder information()

Parameters:
	Info
		video info
Return:
	VIM_SUCCEED:  ok
Note:
Remarks:

state: 
	valid
*********************************************************************************/
UINT32 VIM_HAPI_ChangeDisplaySize(UINT32 width,UINT32 height)
{
	TSize lcdsize;
	lcdsize.cx=width;
	lcdsize.cy=height;
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
	VIM_DISP_SetA_DisplaySize(lcdsize);
	//VIM_DISP_ResetState();
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);	
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	set video decoder information()

Parameters:
	Info
		video info
Return:
	VIM_SUCCEED:  ok
Note:
Remarks:

state: 
	valid
*********************************************************************************/
UINT32 VIM_HAPI_GetDisplaySize(UINT32 * width,UINT32 * height)
{
	TSize lcdsize;
	VIM_DISP_GetAWinsize(&lcdsize);
	
	*width	= lcdsize.cx;
	*height 	= lcdsize.cy;
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	set video decoder information()

Parameters:
	Info
		video info
Return:
	VIM_SUCCEED:  ok
Note:
Remarks:

state: 
	valid
*********************************************************************************/
UINT32 VIM_HAPI_VideoDecInit(PVIM_MPEG4INFO Info)
{
#if 0 // call 578 rdk
	VIM_HAPI_SetDisplayVideoInfo(Info,VIM_HAPI_RAM_SAVE);
	VIM_HAPI_StartDisplayVideo((char*)0x40c00000,0x100000,NULL);
	return VIM_SUCCEED;
#else
	VIM_RESULT result;
       TSize DisplaySize,DestDisplaySize,SourceSize;
	TPoint pt = {0, 0};
	TSize MaxSize= {0, 0};

	MaxSize.cx=(gVc0578b_Info.MainPanel.Size.cx>=gVc0578b_Info.MainPanel.Size.cy)?gVc0578b_Info.MainPanel.Size.cx:gVc0578b_Info.MainPanel.Size.cy;
	MaxSize.cy=(gVc0578b_Info.MainPanel.Size.cx<gVc0578b_Info.MainPanel.Size.cy)?gVc0578b_Info.MainPanel.Size.cx:gVc0578b_Info.MainPanel.Size.cy;
	
	//(1) check the chip working mode 
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON&&gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_PHOTOPROCESS) 
		return VIM_ERROR_WORKMODE;
	
	VIM_USER_MemCpy((UINT8 *)&gVc0578b_Info.Mpeg4Status.DisInfo,(UINT8 *)Info,sizeof(VIM_MPEG4INFO));

	//(2) check user defined display size>{4,4},must less than (LCD size),get display size
	result = VIM_MAPI_CheckDisplaySize(gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_x,
	gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_y,gVc0578b_Info.Mpeg4Status.DisInfo.DecWidth,
	gVc0578b_Info.Mpeg4Status.DisInfo.DecHeigth,&DisplaySize);
	if(result)
		return result;

	//(3) set chip work mode 
	//disable syncgen
	VIM_SIF_EnableSyncGen(DISABLE);	//angela 2006-8-15
	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;
	//set mode
	VIM_MAPI_SetChipMode(VIM_MARB_DISPLAYMPG4_MODE,VIM_IPP_HAVE_NOFRAME);

	//(4) get real display size according to image source size (display image in user define display size, maybe not full display in LCD, only part of user define display size)
	SourceSize.cx=gVc0578b_Info.Mpeg4Status.DisInfo.SorWidth;
	SourceSize.cy=gVc0578b_Info.Mpeg4Status.DisInfo.SorHeigth;
	if(VIM_USER_DISPLAY_FULLSCREEN)
		result = VIM_IPP_ToolCaculateBigDstWindow(SourceSize,DisplaySize,&DestDisplaySize);
	else
		result = VIM_IPP_ToolCaculateLessDisplayWindow(SourceSize,DisplaySize,&DestDisplaySize);
	if(result)
		return result;

	//(11 )set ipp module image size
	VIM_IPP_SetImageSize((SourceSize.cx+15)&0xfff0,( SourceSize.cy+15)&0xfff0);//revised by youhai 2008. 1.29

	//(13) set ipp module sizer, source window size, display size
#if USERDISPLAYSIZE
	DestDisplaySize.cx=DisplaySize.cx;
	DestDisplaySize.cy=DisplaySize.cy;
#endif
	pt.x = pt.y = 0;
	result = VIM_IPP_SetCaptureSize(NULL,pt, SourceSize, DestDisplaySize);
	if(result)
		return result;

	//(10) reset 
	VIM_HIF_SetReg32(V5_REG_MP4DEC_RESET, 0);	

	//(12 )set mp4 dec module image size
	VIM_MPEG4_SetDecVolSize(SourceSize.cx, SourceSize.cy);

	//(5) set LCDC A layer start point in video frame
	pt.x=pt.y=0;
	if(DestDisplaySize.cx>DisplaySize.cx)	
		pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffe;
	if(DestDisplaySize.cy>DisplaySize.cy)	
		pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffe;

	result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
	if(result)
		return result;

	//(6) set A layer start point and window size on LCD panel
	pt.x=(gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_x)&0xfffe;
	pt.y=(gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_y)&0xfffe;
	if(DestDisplaySize.cx<=DisplaySize.cx)	
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_x)&0xfffe;
	else
		DestDisplaySize.cx=DisplaySize.cx;
	if(DestDisplaySize.cy<=DisplaySize.cy)
		pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_y)&0xfffe;
	else
		DestDisplaySize.cy=DisplaySize.cy;
//add by youhai 2008.1.30 begin	
		gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outxpos=pt.x;
		gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outypos=pt.y;
		gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outwidth=DestDisplaySize.cx;
		gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outheight=DestDisplaySize.cy;
//add by youhai 2008.1.30 end
	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);

	//(7) keep LCD size in A1 layer window size register, it used in MarbSetMap function
#if VIM_USER_MPEG4_USE_MAXALAYER

#if LCDCA0SDRAM
	gVc0578b_Info.MarbStatus.ALayerMode = VIM_DISP_ONEFRAME;
#else
	gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_DISPLAY_USELINEBUF;
#endif
	VIM_DISP_SetA1_DisplayPanel(gVc0578b_Info.MainPanel.Point,
							MaxSize);
	/*use this register to save the value of max size*/

#endif

	// (8) rotation process
	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
					VIM_DISP_NOTCHANGE);
	if(result)
		return result;
	VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);

	//(9) reset and enable a layer
	VIM_DISP_ResetState();
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);


	//(14) marb map memory
	result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
		(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
	if(result)
		return result;
	
	//(15) set interrupt
	//VIM_MAPI_InitDisMpeg4Interrupt();
	VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_DEC_DONE,ENABLE);

       //(16) pisel rate
//       VIM_JPEG_AdjustClkBlank();revised by youhai 2008.1.25
      VIM_MPEG4_AdjustClkBlank();

	return VIM_SUCCEED;
#endif	
}

/********************************************************************************
Description:
	Start new block 

Parameters:
	addr
		video bitstream address
	len	
		video bitstream len
	mode
		VIM_MPEG4_FIRSTBLOCK,VIM_MPEG4_NEXTBLOCK,VIM_MPEG4_LASTBLOCK
Return:
	VIM_SUCCEED:  ok
Note:
Remarks:

state: 
	valid
*********************************************************************************/
UINT32 VIM_HAPI_StartNewBlockVop(UINT32 addr, UINT32 len, UINT32 mode)
{
#if VIM_USER_MPEG4_VOPMODE 
	VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_INI_ADD,addr);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_SIZE,len);
#else
	if(mode&VIM_MPEG4_BLOCK_START)
	{
		VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_INI_ADD,addr);
		VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_SIZE,len);
	}
#endif
	VIM_MPEG4_StartDec(mode);	
	return VIM_SUCCEED;
}

UINT32 VIM_HAPI_GetMPEGDecIntStatus(void)
{
	UINT32 temp;

	temp=VIM_HIF_GetReg32(V5_REG_IPP_Q_HISTOGRAM);
	return (VIM_MPEG4_DECINTFLAG)temp;
	
}

void VIM_HAPI_SetMPEGDecIntStatus(UINT32 marbflag)
{
	
	//VIM_HIF_SetReg32(V5_REG_IPP_Q_HISTOGRAM,marbflag);
	MMD_SYS_SetMem(V5_REG_IPP_Q_HISTOGRAM,marbflag);

}

UINT32 VIM_HAPI_WriteVideoRawData(UINT32 addr,HUGE UINT8 *buf,UINT32 len)
{
	return VIM_HIF_WriteSram(addr,buf,len,VIM_HIF_SRAM_MODE);
}


UINT32 VIM_HAPI_GetMp4DecStatus(void)
{
	return VIM_MPEG4_GetDecIntFlag();
}

//add by youhai 2008.1.30 begin	
UINT32 VIM_HAPI_GetDecInfo(PVIM_VIDEO_OUTINFO pDecInfo)
{
if(pDecInfo==NULL)
	return VIM_ERROR_DECODE_ERROR;

pDecInfo->v_outeffect=gVc0578b_Info.PreviewStatus.SpecialEffect;
pDecInfo->v_outrotation=gVc0578b_Info.LcdStatus.ARotationMode;
pDecInfo->v_outheight=gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outheight;
pDecInfo->v_outwidth=gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outwidth;
pDecInfo->v_outxpos=gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outxpos;
pDecInfo->v_outypos=gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outypos;

pDecInfo->v_srcheight=gVc0578b_Info.Mpeg4Status.DisInfo.SorHeigth;
pDecInfo->v_srcwidth=gVc0578b_Info.Mpeg4Status.DisInfo.SorWidth;
pDecInfo->v_srcxpos=0;
pDecInfo->v_srcypos=0;
return VIM_SUCCEED;
}
//add by youhai 2008.1.30 end

/********************************************************************************
Description:
	set video decoder information()

Parameters:
	Info
		video info
Return:
	VIM_SUCCEED:  ok
Note:
Remarks:

state: 
	valid
*********************************************************************************/
UINT32 VIM_HAPI_VideoResize(PVIM_MPEG4INFO Info)
{
#if 0 // call 578 rdk
	VIM_HAPI_SetDisplayVideoInfo(Info,VIM_HAPI_RAM_SAVE);
	VIM_HAPI_ChangeVideoSize();
#else
	VIM_RESULT result;
       TSize DisplaySize,DestDisplaySize,SoucseSize;//,AbufSize;
	TPoint pt = {0, 0};
	UINT8 bZoomOut=1;

	//(1) check the chip working mode 
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON&&gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_PHOTOPROCESS) 
		return VIM_ERROR_WORKMODE;

	if(gVc0578b_Info.MarbStatus.WorkMode==VIM_MARB_VIDEOCONFERENCE_MODE)
		return VIM_ERROR_CANNOTSUPPORT_CHANGESIZE;

	//(2) copy mp4 info
	VIM_USER_MemCpy((UINT8 *)&gVc0578b_Info.Mpeg4Status.DisInfo,(UINT8 *)Info,sizeof(VIM_MPEG4INFO));

	//(3) Disable A layer
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
	VIM_IPP_Reset();
	//(4) get real display size according to image source size (display image in user define display size, maybe not full display in LCD, only part of user define display size)
	DisplaySize.cx=gVc0578b_Info.Mpeg4Status.DisInfo.DecWidth;
	DisplaySize.cy=gVc0578b_Info.Mpeg4Status.DisInfo.DecHeigth;
	SoucseSize.cx=gVc0578b_Info.Mpeg4Status.DisInfo.SorWidth;
	SoucseSize.cy=gVc0578b_Info.Mpeg4Status.DisInfo.SorHeigth;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]mpeg Image width:  ",gVc0578b_Info.Mpeg4Status.DisInfo.SorWidth);
	VIM_USER_PrintDec("[VC0578][HAPI]mpeg Image height:  ",gVc0578b_Info.Mpeg4Status.DisInfo.SorHeigth);
#endif	
	if(VIM_USER_DISPLAY_FULLSCREEN)
		result = VIM_IPP_ToolCaculateBigDstWindow(SoucseSize,DisplaySize,&DestDisplaySize);
	else
		result = VIM_IPP_ToolCaculateLessDisplayWindow(SoucseSize,DisplaySize,&DestDisplaySize);
	if(result)
		return result;

	//(5) Caculate zoom size
       if(SoucseSize.cx*DisplaySize.cy>=SoucseSize.cy*DisplaySize.cx)
       {
		if((DisplaySize.cx/2<DestDisplaySize.cx)&&(DestDisplaySize.cx<=DisplaySize.cx))
			DestDisplaySize.cx=DisplaySize.cx;
		else	 if (DestDisplaySize.cx<=DisplaySize.cx/2)
			DestDisplaySize.cx=DestDisplaySize.cx*2;
		else
			bZoomOut=0;
	    //  if(bZoomOut&&DestDisplaySize.cx<SoucseSize.cx)/*to avoid the situation of diffrent sizer selection of width and height*/
             //   DestDisplaySize.cx=SoucseSize.cx;
		
              DestDisplaySize.cy=DestDisplaySize.cx*SoucseSize.cy/SoucseSize.cx;
//       	
       	}
       else
	   	{
		if((DisplaySize.cy/2<DestDisplaySize.cy)&&(DestDisplaySize.cy<=DisplaySize.cy))
			DestDisplaySize.cy=DisplaySize.cy;
		else	 if (DestDisplaySize.cy<=DisplaySize.cy/2)
			DestDisplaySize.cy=DestDisplaySize.cy*2;	
		else
			bZoomOut=0;
	    //  if(bZoomOut&&DestDisplaySize.cy<SoucseSize.cy)/*to avoid the situation of diffrent sizer selection of width and height*/
               // DestDisplaySize.cy=SoucseSize.cy;

              DestDisplaySize.cx=DestDisplaySize.cy*SoucseSize.cx/SoucseSize.cy;
	}
	DestDisplaySize.cx&=0xfff8;
	DestDisplaySize.cy&=0xfff8;
#if USERDISPLAYSIZE
	DestDisplaySize.cx=DisplaySize.cx;
	DestDisplaySize.cy=DisplaySize.cy;
#endif
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI] Dest DisplaySize height:  ",DestDisplaySize.cy);
#endif	

	//(6) Set IPP mode capture sizer 
	pt.x = pt.y = 0;
	DisplaySize.cx=DestDisplaySize.cx;
	DisplaySize.cy=DestDisplaySize.cy;
	if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_VIDEOCONFERENCE_MODE)
		//result = VIM_IPP_SetCaptureSize(NULL,pt, SoucseSize, DestDisplaySize);
		result = VIM_IPP_SetDisplaySizerSize(pt,SoucseSize,DisplaySize,&DestDisplaySize);
	else
		result = VIM_IPP_SetDispalySize(pt, SoucseSize, DestDisplaySize);
	if(result)
		return result;
	DisplaySize.cx=gVc0578b_Info.Mpeg4Status.DisInfo.DecWidth;
	DisplaySize.cy=gVc0578b_Info.Mpeg4Status.DisInfo.DecHeigth;
	//(7) set LCDC A layer memory size and window size
	pt.x=pt.y=0;
	if(DestDisplaySize.cx>DisplaySize.cx)	
		pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffe;
	if(DestDisplaySize.cy>DisplaySize.cy)	
		pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffe;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
		VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
#endif		
	result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
	if(result)
		return result;
	
	pt.x=(gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_x)&0xfffe;
	pt.y=(gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_y)&0xfffe;
	if(DestDisplaySize.cx<DisplaySize.cx)	
		//{}
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_x)&0xfffe;
		//pt.x=gVc0578b_Info.Mpeg4Status.DisInfo.DecWidth-(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_x)&0xfffc;
	else if(DestDisplaySize.cx==DisplaySize.cx)
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_x)&0xfffe;
	else
		DestDisplaySize.cx=DisplaySize.cx;
	if(DestDisplaySize.cy<=DisplaySize.cy)	
		//{}
		pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_y)&0xfffe;
	else
		DestDisplaySize.cy=DisplaySize.cy;
//add by youhai 2008.1.30 begin	
		gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outxpos=pt.x;
		gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outypos=pt.y;
		gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outwidth=DestDisplaySize.cx;
		gVc0578b_Info.Mpeg4Status.cur_DisInfo.v_outheight=DestDisplaySize.cy;
//add by youhai 2008.1.30 end
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
		VIM_USER_PrintDec("[VC0578][HAPI] a display offset y:  ",pt.y);
		VIM_USER_PrintDec("[VC0578][HAPI] new DisplaySize width:  ",DestDisplaySize.cx);
		VIM_USER_PrintDec("[VC0578][HAPI] new DisplaySize height:  ",DestDisplaySize.cy);
#endif	

	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);
	//(8) rotation
	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
					VIM_DISP_NOTCHANGE);
	if(result)
		return result;
	VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
	//(9) enable a layer
	VIM_DISP_ResetState();
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);

	//(10)pisel rate
       //VIM_JPEG_AdjustClkBlank();revised by youhai 2008.1.25
       VIM_MPEG4_AdjustClkBlank();
#endif
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	Disable mp4 stuck error interrupt count increment, this function is used in pause mode to 
	to avoid stuck error occured
Parameters:
	NULL
Return:
	VIM_SUCCEED:  ok
Note:
Remarks:

state: 
	valid
*********************************************************************************/
UINT32 VIM_HAPI_DisableStuckError(void)
{
	UINT32 temp;
	temp = VIM_HIF_GetReg32(V5_REG_MP4DEC_PINGPANG_MBL);
	temp |= (0x1<<29);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_PINGPANG_MBL, temp);
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	Enable mp4 stuck error interrupt count increment, this function is used when resume decode
	operation
Parameters:
	NULL
Return:
	VIM_SUCCEED:  ok
Note:
Remarks:

state: 
	valid
*********************************************************************************/
UINT32 VIM_HAPI_EnableStuckError(void)
{
	UINT32 temp;
	temp = VIM_HIF_GetReg32(V5_REG_MP4DEC_PINGPANG_MBL);
	temp &= ~(0x1<<29);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_PINGPANG_MBL, temp);
	return VIM_SUCCEED;
}

//this function add by Amanda Deng 2008/01/30
UINT32 VIM_HAPI_VideoStopDec(void)
{
	UINT32 temp;
	temp = VIM_HIF_GetReg32(V5_REG_MP4DEC_PINGPANG_MBL);
	temp &= ~(0x1<<29);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_PINGPANG_MBL, temp);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_RESET, 0);	
	VIM_HIF_SetReg32(V5_REG_MP4DEC_RESET, 0);
	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x01);
	VIM_USER_DelayMs(1);
	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x0);
	VIM_IPP_Reset();
	
	VIM_MAPI_FreeDisplayVideoBuffer();
	gVc0578b_Info.MarbStatus.WorkMode=VIM_MARB_NONE_MODE;
	return VIM_SUCCEED;
}
#endif 

#if 0


/********************************************************************************
Description:
	get JPG picture size 
Parameters:
	picWidth:  the width of captured image.
	picHeight: the height of captured image.
	Return:
		V5H_SUCCEED: set ok
	Note:
		This function must be used before first preview or change the resolution of display.
		This function must be used in Camera open mode
	Remarks:
	State:Valid.
*********************************************************************************/
UINT16 VIM_HAPI_GetPictureSize(UINT16 *picWidth, UINT16 *picHeight)
{
	*picWidth =gVc0578b_JpegInfo.ImageSize.cx; 
	*picHeight =	gVc0578b_JpegInfo.ImageSize.cy;
	return VIM_SUCCEED;
}



/********************************************************************************
Description:
	sensor work mode 
Parameters:
	mode:
		VIM_HAPI_SENSOR_OUT = 0x0,		out door condition
		VIM_HAPI_SENSOR_50HZ=0x1,		in door 50hz
		VIM_HAPI_SENSOR_60HZ=2,		in door	60hz
		VIM_HAPI_SENSOR_NIGHT=3,		night
	Return:
		VIM_SUCCEED: set mirror ok
		VIM_ERROR_WORKMODE: error work mode
	Note:
		This function must be used in Camera open mode
Remarks:
	state: valid
*********************************************************************************/
UINT16 VIM_HAPI_SetSensorMode(VIM_HAPI_SENSORMODE mode)
{

	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;
	
	if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_OFF)
		{
			gVc0578b_Info.PreviewStatus.SensorMode=(VIM_HAPI_SENSORMODE)mode;
			return VIM_SUCCEED;  //angela changed for set preview mirror in preview on
		}
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
	//V5M_PreviewMirror(mode);
	VIM_USER_DelayMs(100);
	if(gVc0578b_Info.pSensorInfo->snrModeCall!=NULL)
	{
		gVc0578b_Info.pSensorInfo->snrModeCall(mode);
		VIM_DISP_ResetState();
	}
	VIM_USER_DelayMs(100);// one frame end
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);	
	gVc0578b_Info.PreviewStatus.SensorMode=(VIM_HAPI_SENSORMODE)mode;

return VIM_SUCCEED;
}


//do af or not when preview
/********************************************************************************
Description:
	select weather do af in preview mode
Parameters:
	able: ENABLE, Yes,Do af during preview
	      DISABLE, No,Do not do af during preview
	      default is DISABLE
	Return:
		NULL
Remarks:
		this function can be called before VIM_HAPI_SetPreviewMode
*********************************************************************************/
void VIM_HAPI_SetAFPreviewEnable(	UINT8 Able)
{
	gVc0578b_Info.aeaf.Baeafen = Able;
}



/********************************************************************************
Description:
	Adjust the focus when preview or before capture if not in focus
Parameters:
	Parameters:
		NULL
	Return:
		succeed
		or failed
Remarks:
	Called by User
*********************************************************************************/
UINT16 VIM_HAPI_DoAutoFocus(void)
{
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;

	if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_OFF)
		return VIM_ERROR_PREVIEWMODE;  //angela changed for set preview mirror in preview on
#if VIM_SENSOR_AUTO_FOCUS	
	VIM_MAPI_IspDoAFAtOnetime();
#endif
	return VIM_SUCCEED;
}




/********************************************************************************
Description:
	Change Brightness
Parameters:
	Parameters:
	Mode:
		VIM_HAPI_CHANGE_NORMAL=0,
		 	auto set normal status(preview mode).
		 	in this mode the step has no meaning.
		VIM_HAPI_CHANGE_ADD,
			when in add mode.
			the step 's meaning is that 
		VIM_HAPI_CHANGE_DEC,
			when in decrease mode.
			the step 's meaning is that 
			source window's width  will zoom out one step
		VIM_HAPI_SET_STEP,
		       set the direct brightness.
	Step:
		0-5
	Return:
		succeed
		or failed
Remarks:
	Called by User
*********************************************************************************/
UINT16 VIM_HAPI_SetBrightness(VIM_HAPI_CHANGE_MODE Mode,UINT8 Step)
{
	UINT8 newbright;
	//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else	
	SINT8 temp;
#endif
	//#define BRIGHTNESS_STEP 2
	
	if(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS)
		return VIM_ERROR_WORKMODE;

	newbright = gVc0578b_Info.VIM_SIF_AeAdjust.brightness;

	if(Step > VIM_USER_MAX_STEP )
		Step = VIM_USER_MAX_STEP;

//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else	
	if(! gVc0578b_Info.pSensorInfo->snrtype)//RGB sensor
	{
		temp =gVc0578b_Info.VIM_SIF_AeAdjust.brightness - 32;	
		temp /= BRIGHTNESS_STEP;
	
	
		switch(Mode)
		{
			case VIM_HAPI_CHANGE_ADD:
				if (temp < (VIM_USER_MAX_STEP/2) )
					newbright += BRIGHTNESS_STEP;
				break;
			case VIM_HAPI_CHANGE_DEC:
				if(temp >  -(VIM_USER_MAX_STEP/2) )
					newbright -= BRIGHTNESS_STEP;
				break;
			case VIM_HAPI_SET_STEP:
				if(Step > VIM_USER_MAX_STEP/2)
					newbright=32 + (Step-VIM_USER_MAX_STEP/2)*BRIGHTNESS_STEP;
				else
					newbright=32 - (VIM_USER_MAX_STEP/2 - Step)*BRIGHTNESS_STEP;				
				break;
			case VIM_HAPI_CHANGE_NORMAL:
				newbright=32;
			default:
				break;
		}

		VIM_SIF_IspSetBrightness(&gVc0578b_Info.VIM_SIF_AeAdjust,gVc0578b_Info.pSensorInfo,newbright);
	}		
	else
#endif
	{
		if(gVc0578b_Info.pSensorInfo->snrBrightnessCall)
		{
			gVc0578b_Info.pSensorInfo->snrBrightnessCall(Step);
		}
	}
	return VIM_SUCCEED;
}




/********************************************************************************
Description:
	Change Brightness
Parameters:
	Parameters:
	Mode:
		VIM_HAPI_CHANGE_NORMAL=0,
		 	auto set normal status(preview mode).
		 	in this mode the step has no meaning.
		VIM_HAPI_CHANGE_ADD,
			when in add mode.
			the step 's meaning is that 
		VIM_HAPI_CHANGE_DEC,
			when in decrease mode.
			the step 's meaning is that 
			source window's width  will zoom out one step
		VIM_HAPI_SET_STEP,
		       set the direct brightness.
	Return:
		succeed
		or failed
Remarks:
	Called by User
*********************************************************************************/
UINT16 VIM_HAPI_SetContrast(VIM_HAPI_CHANGE_MODE Mode,UINT8 Step)
{

	UINT8 newcontrast;
	//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else	
	SINT8 temp;
#endif
	//#define CONTRAST_STEP 5
	
	if(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS)
		return VIM_ERROR_WORKMODE;

	newcontrast = gVc0578b_Info.VIM_SIF_AeAdjust.contrast;

	if(Step > VIM_USER_MAX_STEP )
		Step = VIM_USER_MAX_STEP;
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else	
	if(!gVc0578b_Info.pSensorInfo->snrtype)
	{
		temp =gVc0578b_Info.VIM_SIF_AeAdjust.contrast - 32;
		temp /= CONTRAST_STEP;

		switch(Mode)
		{
			case VIM_HAPI_CHANGE_ADD:
				if (temp < (VIM_USER_MAX_STEP/2))
					newcontrast += CONTRAST_STEP;
				break;
			case VIM_HAPI_CHANGE_DEC:
				if(temp > -(VIM_USER_MAX_STEP/2 ))
					newcontrast -= CONTRAST_STEP;
				break;
			case VIM_HAPI_SET_STEP:
				if(Step > VIM_USER_MAX_STEP/2)
					newcontrast=32 + (Step-VIM_USER_MAX_STEP/2)*CONTRAST_STEP;
				else
					newcontrast=32 - (VIM_USER_MAX_STEP/2-Step)*CONTRAST_STEP;
				break;
			case VIM_HAPI_CHANGE_NORMAL:
					newcontrast=32;
			default:
				break;
		}
		VIM_SIF_IspSetContrast(&gVc0578b_Info.VIM_SIF_AeAdjust,gVc0578b_Info.pSensorInfo,newcontrast);

		return VIM_SUCCEED;
	}
	else
#endif
	{
		if(gVc0578b_Info.pSensorInfo->snrContrastCall)
		{
			gVc0578b_Info.pSensorInfo->snrContrastCall(Step);
		}	
	}
	return VIM_SUCCEED;
}


/********************************************************************************
Description:
	Change Sensor Output type
Parameters:
	Parameters:
	Mode:
		VIM_HAPI_SENSOR_NORMAL = 0x3,
		VIM_HAPI_SENSOR_FLIP=0x2,
		VIM_HAPI_SENSOR_MIRROR=1
	Return:
		succeed
		or failed
Remarks:
	Called by User
*********************************************************************************/
UINT16 VIM_HAPI_SetPreviewMirror(VIM_HAPI_SENSORMIRROR mode)
{
	if(gVc0578b_Info.ChipWorkMode==VIM_HAPI_MODE_BYPASS)
		return VIM_ERROR_WORKMODE;


	if(gVc0578b_Info.pSensorInfo->snrMirrorFlipCall)
	{
		gVc0578b_Info.pSensorInfo->snrMirrorFlipCall(mode);
		return VIM_SUCCEED;
	}

	return VIM_ERROR_MIRROR_NOTSUPPORT;
	
}
#endif

#if VIDEOCAP
/********************************************************************************
   		video capture related
*******************************************************************************/

/********************************************************************************
Description:
	set capture video information()
	UINT16 VIM_HAPI_StartCaptureVideo(void *StillBuf,UINT32 BUF_Length,VIM_HAPI_CallBack pUCallBack)

Parameters:
      SaveMode:
      				VIM_HAPI_RAM_SAVE,   save in the still buf (UINT8)
				VIM_HAPI_ROM_SAVE  stillbuf is the file id  of file 
      bFramRate: frame rate (per second) if 0.do not start timer to capture (customer will shot jpeg themsleves)
	dwMaxFrameCount: the max frame count  the user wanted
					if it is zero ,will not stop the video automatically
Return:
	VIM_SUCCEED:  ok
Note:
Remarks:

state: 
	valid
*********************************************************************************/

UINT16 VIM_HAPI_SetCaptureVideoInfo(VIM_HAPI_SAVE_MODE SaveMode,UINT8 bFramRate,UINT32 dwMaxFrameCount ,UINT32 BitRate)
{
	//check the chip working mode 
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON) 
		return VIM_ERROR_WORKMODE;
	
	gVc0578b_Info.CaptureStatus.SaveMode=SaveMode;
	gVc0578b_Info.VideoStatus.CapVideoFrameRate=bFramRate;
	gVc0578b_Info.VideoStatus.MaxFrame=dwMaxFrameCount;
//revised by youhai_mp4enc 2007.8.30	
#if (BOARD_TYPE==VC0820_FPGA)
#else
	gVc0578b_Info.Mpeg4Status.BitRate=BitRate;
#endif
	
	VIM_MPEG4_SetVolSize(gVc0578b_Info.CaptureStatus.Size.cx,gVc0578b_Info.CaptureStatus.Size.cy);

	return VIM_SUCCEED;
}

UINT32 VIM_HAPI_GetMp4EncStatus(void)
{
	return VIM_MPEG4_GetEncIntFlag();	
}

UINT32 VIM_HAPI_GetMPEGEncIntStatus(void)
{
	UINT32 temp;

	temp=VIM_HIF_GetReg32(V5_REG_IPP_Q_HISTOGRAM);
	return (VIM_MPEG4_ENCINTFLAG)temp;
	
}

void VIM_HAPI_SetMPEGEncIntStatus(UINT32 marbflag)
{
	
	//VIM_HIF_SetReg32(V5_REG_IPP_Q_HISTOGRAM,marbflag);
	MMD_SYS_SetMem(V5_REG_IPP_Q_HISTOGRAM,marbflag);

}


UINT32 VIM_HAPI_GetMp4EncVopLength(void)
{
	return VIM_MPEG4_GetLength(0,VIM_USER_MPEG4_FRAMS);	
}

UINT32 VIM_HAPI_ReadVideoRawData(UINT32 addr,HUGE UINT8 *buf,UINT32 len)
{
	return VIM_HIF_ReadSram(addr,buf,len,VIM_HIF_SRAM_MODE);
}


//revised by youhai_mp4enc 2007.8.30	

UINT32 VIM_HAPI_Init_CaptureMp4Parameter(PVIM_HAPI_MP4_ENC_INIT_IN_INFO pMp4InInfo)
{

	pMp4InInfo->pCallBack=NULL;//for further; 
	
	pMp4InInfo->Enc_Stuff=0x1ff;// bit8: 0->8bit 1:->32bit and the lowest 8bits reprensent the stuff  data
 //capture quality
	pMp4InInfo->Enc_Quality_Level=20; //  1-31 the higher, the better image quality.
	pMp4InInfo->Enc_Half_Pixsel_Estimation_Enable=1;//
#if 1
//for not short header
	pMp4InInfo->Enc_Short_HeaderSurpport=0;//0:not 1: yes def:0
#if 1	
	pMp4InInfo->Enc_Mpeg4_ResyncMarker_Disable=1;//0:not 1: yes
#else
	pMp4InInfo->Enc_Mpeg4_ResyncMarker_Disable=0;//0:not 1: yes
	pMp4InInfo->Enc_Mpeg4_HeaderExt=1;// 1: enable 0:disable  
	pMp4InInfo->Enc_Mpeg4_HeaderExtMaxPacketWordCnt=0xffff;//
#endif
       pMp4InInfo->Enc_Mpeg4_QuantType=0;//0:H.263 1:mpeg-2
	pMp4InInfo->Enc_Mpeg4_4mv_Disable=0;//
	pMp4InInfo->Enc_Mpeg4_Rounding_Enable=1;//
       pMp4InInfo->Enc_Mpeg4_intra_dc_vlc=0;	
	pMp4InInfo->time_increment=2;//7.5frame
	pMp4InInfo->time_resolution=30;//29.9
#else
//for short header
      pMp4InInfo->Enc_Short_HeaderSurpport=1;//0:not 1: yes def:0
      pMp4InInfo->Enc_Mpeg4_ResyncMarker_Disable=1;//0:not 1: yes
      pMp4InInfo->time_increment=4;//7.5frame/s
      pMp4InInfo->time_resolution=30;
#endif
//-----------        
#if 1
pMp4InInfo->Enc_Out_Bitrate_Ctrl_En=0;
#else
//-----------
//out bitrate control ---------------------------------------------------------------      
      pMp4InInfo->Enc_Out_Bitrate_Ctrl_En=1;
      pMp4InInfo->RC_no_frame_skipping=1;
      pMp4InInfo->RC_fixed_Qp=0;
      pMp4InInfo->RC_out_bitrate=128;
#endif	  
	return VIM_SUCCEED;
}

//revised by youhai_mp4enc 2007.8.30	

/*
software provide
vos->visual object->  
{0x00,0x00,0x01,0xB0,0x03, 0x00, 0x00, 0x01,0xB5,0x0E,0xA0,0x20,0x20,0x2F}14 bytes 
hardware provide
(video object+vol header)=8+11=19bytes
vol header
random_accessible_vol=0;                        1bit
video_object_type_indication=00000001;          8bits //Simple Object Type 
is_object_layer_identifier=1                    1bit
video_object_layer_verid=0010                   4bits
video_object_layer_priority=001                 3bits
aspect_ratio_info=0001                          4bits
vol_control_parameters=0                        1bits
video_object_layer_shape=00                     2bits//rectangle
marker_bit= 1                                   1bit
vop_time_increment_resolution=00000000 00011110 16bits //30
marker_bit=1                                    1bit
fixed_vop_rate=0                                1bit
marker_bit= 1                                   1bit
video_object_layer_width =00000 11110000        13bit //240
marker_bit= 1                                   1bit  
video_object_layer_height=00001 01000000        13bit //320
marker_bit= 1                                   1bit
interlaced=0                                    1bit//progressive format.
obmc_disable=1                                  1bit//disables overlapped block motion compensation.
sprite_enable=00                                2bits//sprite not used
not_8_bit=0                                     1bit
quant_type=0                                    1bit
quarter_sample=0                                1bit
complexity_estimation_disable=1                 1bit
resync_marker_disable=1                         1bit
data_partitioned=0                              1bit
newpred_enable=0
reduced_resolution_vop_enable=0
scalability=0
011
*/
UINT32 VIM_HAPI_CaptureMp4_Init(PVIM_HAPI_MP4_ENC_INIT_IN_INFO pMp4InInfo,PVIM_HAPI_MP4_ENC_INIT_OUT_INFO pMp4OutInfo)
{
	UINT32 result;
	TSize InputSize;

	if(!pMp4InInfo)
		return VIM_ERROR_CANNOT_CAPVIDEO;

	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON) 
		return VIM_ERROR_WORKMODE;
	
	//check the preview  mode 
    if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_OFF)
       	return VIM_ERROR_PREVIEWMODE;

    gVc0578b_Info.Mpeg4Status.m_Enc_I_Frame_Estimat_Size=VIM_MPEG4_Estimate_I_Size(pMp4InInfo->capture_width, pMp4InInfo->capture_height, pMp4InInfo->RC_out_bitrate);
	pMp4OutInfo->I_Frame_Estimat_Size=gVc0578b_Info.Mpeg4Status.m_Enc_I_Frame_Estimat_Size;
	pMp4OutInfo->OneBufferSize=gVc0578b_Info.Mpeg4Status.m_Enc_I_Frame_Estimat_Size*VIM_USER_MPEG4_ENC_OUTBUF_FRAMES_NUM;//0x3580;//
	//check the chip working mode 
	
	result = VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, (VIM_SIF_RESOLUTION)gVc0578b_Info.PreviewStatus.bOutputIndex, &InputSize);
// revised by youhai 2007.9.20
	if((gVc0578b_Info.CaptureStatus.Size.cx>VIM_MPEG4_CAP_MAX_WIDTH)||(gVc0578b_Info.CaptureStatus.Size.cy>VIM_MPEG4_CAP_MAX_HEIGHT))
		return VIM_ERROR_CANNOT_CAPVIDEO;

	if((gVc0578b_Info.CaptureStatus.Size.cx>(InputSize.cx*2))||(gVc0578b_Info.CaptureStatus.Size.cy>(2*InputSize.cy)))
		return VIM_ERROR_CANNOT_CAPVIDEO;

	if(pMp4InInfo->osd_en&&((gVc0578b_Info.CaptureStatus.Size.cx>pMp4InInfo->preview_width)||(gVc0578b_Info.CaptureStatus.Size.cy>pMp4InInfo->preview_height)))
		return VIM_ERROR_CANNOT_CAPVIDEO;
	
	gVc0578b_Info.CaptureStatus.CaptureError=VIM_SUCCEED;
	gVc0578b_Info.CaptureStatus.Mode=VIM_CAPTUREMPE4;
	
//keep as reserved	   	
	gVc0578b_Info.Mpeg4Status.m_Enc_OutBitStreamOrder=1;//big endian
	gVc0578b_Info.Mpeg4Status.m_Enc_SrcBitStreamOrder=1;//big endian
	gVc0578b_Info.Mpeg4Status.m4Rc_MultiFrames=1;

//---------------------------------------------------------------------------------	
//for h.263/mp4v
	gVc0578b_Info.Mpeg4Status.m_Enc_Stuff=pMp4InInfo->Enc_Stuff;//enable
	gVc0578b_Info.Mpeg4Status.m_Enc_ResyncMarker_Disable=pMp4InInfo->Enc_Mpeg4_ResyncMarker_Disable;//disable
	gVc0578b_Info.Mpeg4Status.m_Enc_Short_HeaderSurpport=pMp4InInfo->Enc_Short_HeaderSurpport;
	gVc0578b_Info.Mpeg4Status.m_Enc_QuantType=pMp4InInfo->Enc_Mpeg4_QuantType;//H.263
	gVc0578b_Info.Mpeg4Status.m_Enc_4mv_Disable=pMp4InInfo->Enc_Mpeg4_4mv_Disable;//enable
	gVc0578b_Info.Mpeg4Status.m_Enc_Half_Pixsel_Estimation_Enable=pMp4InInfo->Enc_Half_Pixsel_Estimation_Enable;//enalbe
	gVc0578b_Info.Mpeg4Status.m_Enc_intra_dc_vlc=pMp4InInfo->Enc_Mpeg4_intra_dc_vlc;//enalbe
	gVc0578b_Info.Mpeg4Status.m_Enc_Rounding_Enable=pMp4InInfo->Enc_Mpeg4_Rounding_Enable;//enable
	gVc0578b_Info.Mpeg4Status.m_Enc_HeaderExt=pMp4InInfo->Enc_Mpeg4_HeaderExt;//enable
	gVc0578b_Info.Mpeg4Status.m_Enc_HeaderExtMaxPacketWordCnt=pMp4InInfo->Enc_Mpeg4_HeaderExtMaxPacketWordCnt;//enable
//-----------------------------------------------------------------------------------	
//for outbit control
    gVc0578b_Info.Mpeg4Status.m_Enc_Out_Bitrate_Ctrl_En=pMp4InInfo->Enc_Out_Bitrate_Ctrl_En;
      //gVc0578b_Info.Mpeg4Status.RC.no_frame_skipping=pMp4InInfo->RC_no_frame_skipping;//no skipping
      //gVc0578b_Info.Mpeg4Status.RC.rc_fixed_Qp=pMp4InInfo->RC_fixed_Qp;//
	//if(!pMp4InInfo->Enc_Quality_Level)
		//pMp4InInfo->Enc_Quality_Level=1;
	gVc0578b_Info.Mpeg4Status.m4Qp_g=pMp4InInfo->Enc_Quality_Level&0x1f;//32-(pMp4InInfo->Enc_Quality_Level&0x1f);	  
    gVc0578b_Info.Mpeg4Status.m4Coded_g=1;	
	//check if capture still or frame
	if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_VIDEOCONFERENCE_MODE)
	{
		if(gVc0578b_Info.PreviewStatus.Mode!=VIM_HAPI_PREVIEW_FRAMEON)
			VIM_MAPI_SetChipMode(VIM_MARB_CAPTUREMPG4_MODE,VIM_IPP_HAVE_NOFRAME);
		else //frame on
	       	VIM_MAPI_SetChipMode(VIM_MARB_CAPTUREMPG4_MODE,VIM_IPP_HAVEFRAME);
	}
 //	if(pMp4InInfo->osd_en)
		//VIM_MAPI_Osd_Lcdc_Init_ForDemo();

	//reset 
	VIM_HIF_SetReg32(V5_REG_MP4ENC_RESET, 0);
	VIM_MPEG4_SetVolSize(gVc0578b_Info.CaptureStatus.Size.cx,gVc0578b_Info.CaptureStatus.Size.cy);
	
	/*if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_VIDEOCONFERENCE_MODE)
	{
		result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
			(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
		if(result)
			return (UINT16)result;
	}*/
  //  for(i=0;i<VIM_USER_MPEG4_ENC_OUTBUF_NUM;i++)
	//   	pMp4OutInfo->m_Enc_Out_BufferPtr[i]=gVc0578b_Info.Mpeg4Status.m_Enc_Out_BufferPtr[i];
	
	VIM_HIF_SetReg16(V5_REG_MP4ENC_TIMERESO, pMp4InInfo->time_resolution);
	//设置超时时间
	// init bir rate contol 
//#if VIM_USER_MPEG4_BITRATE
	if(gVc0578b_Info.Mpeg4Status.m_Enc_Out_Bitrate_Ctrl_En)
	VIM_MPEG4_initrc(&gVc0578b_Info.Mpeg4Status.RC,pMp4InInfo->RC_out_bitrate,pMp4InInfo->RC_no_frame_skipping,pMp4InInfo->RC_fixed_Qp, pMp4InInfo->time_increment, 
					pMp4InInfo->time_resolution, gVc0578b_Info.Mpeg4Status.m4Qp_g,&gVc0578b_Info.Mpeg4Status.m4Qp_g,
					&gVc0578b_Info.Mpeg4Status.m4Coded_g,gVc0578b_Info.Mpeg4Status.m4Rc_MultiFrames);
//#endif
	VIM_MPEG4_SetVopType(VIM_MPEG4_VOP_I);
	VIM_MPEG4_SetVopCoded(gVc0578b_Info.Mpeg4Status.m4Coded_g);
	VIM_MPEG4_SetVopQt(gVc0578b_Info.Mpeg4Status.m4Qp_g);
	
	return VIM_SUCCEED;
}
void VIM_HAPI_CaptureMp4_Release(void)
{
//disabel interrupt
	VIM_MAPI_FreeCaptureVideoBuffer();
}
UINT32 VIM_HAPI_StopCaptureMp4(void)
{
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON) 
		return VIM_ERROR_WORKMODE;
	//check the preview  mode 
      	if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_OFF)
       	return VIM_ERROR_PREVIEWMODE;
	if(gVc0578b_Info.VideoStatus.CapMode!=VIM_VIDEO_STARTCAPTURE)
       	return VIM_ERROR_VIDEO_MODE;
	gVc0578b_Info.VideoStatus.CapMode=VIM_VIDEO_WANTSTOP;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintString("**************VIM_HAPI_StopCapture");
#endif
//disabel interrupt
	VIM_INT_Acknowledge(VM_HIF_INT_VENC);
	//VIM_MARB_RegisterISR(MARB_INT_ADDERROR,NULL);	
	//VIM_MARB_SetIntEnable(MARB_INT_ADDERROR,DISABLE);	
//	VIM_MPEG4_RegisterISR(VIM_INT_MPEG4_ENC_DONE,NULL);
	VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_ENC_DONE,DISABLE);
	VIM_HIF_SetReg32(V5_REG_MP4ENC_RESET, 0);
		return VIM_SUCCEED;
}

/********************************************************************************
Description:
*********************************************************************************/
void VIM_HAPI_CaptureMp4_StartNextVop(UINT32 nextpos,UINT32 voptype,UINT32 lastframelen,PVIM_HAPI_MP4_ENC_TIME enc_time)
{
	UINT32 tempreg;
    VIM_HIF_SetReg32(V5_REG_MP4ENC_OUTPUT_ADDR,nextpos);
	
	if(gVc0578b_Info.Mpeg4Status.m_Enc_Out_Bitrate_Ctrl_En)
//#if VIM_USER_MPEG4_BITRATE
	{
		VIM_MPEG4_takeactiveonce(&gVc0578b_Info.Mpeg4Status.RC,lastframelen*8,
								voptype,&gVc0578b_Info.Mpeg4Status.m4Coded_g,
								&gVc0578b_Info.Mpeg4Status.m4Qp_g);
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578B][MID] New Qf  is =",gVc0578b_Info.Mpeg4Status.m4Qp_g);
#endif
		VIM_MPEG4_SetVopCoded(gVc0578b_Info.Mpeg4Status.m4Coded_g);
		VIM_MPEG4_SetVopQt(gVc0578b_Info.Mpeg4Status.m4Qp_g);
    }
//#endif
	VIM_MPEG4_SetVopType(voptype);

	VIM_HIF_SetReg16(V5_REG_MP4ENC_TIME_INC,enc_time->nTime_Inc);
	tempreg=VIM_HIF_GetReg32(V5_REG_MP4ENC_VOP_CTRL2);
	tempreg &= 0xfffff1ff;
	tempreg|=(enc_time->bMod_Time_Base<<9); //0x8;
	VIM_HIF_SetReg16(V5_REG_MP4ENC_VOP_CTRL2,tempreg);
	
	if(gVc0578b_Info.VideoStatus.CapMode==VIM_VIDEO_WANTSTOP)
		VIM_MPEG4_StartEnc(VIM_MPEG4_LASTVOP,VIM_USER_MPEG4_FRAMS);
	else
		VIM_MPEG4_StartEnc(VIM_MPEG4_NEXTVOP,VIM_USER_MPEG4_FRAMS);

}

UINT32 VIM_HAPI_GetCaptureMp4_Mode(void)
{
 return (UINT32)gVc0578b_Info.VideoStatus.CapMode;
}
void VIM_HAPI_SetCaptureMp4_Mode(UINT32 mode)
{
  gVc0578b_Info.VideoStatus.CapMode=(VIM_VIDEO_MODE)mode;
}
/********************************************************************************
  Description:
	set marb mode
  Parameters:
	mode:
		// Preview modes
	VIM_MARB_PREVIEW_MODE=0, // Auto Bit Rate Control enable

	// Capture modes
	VIM_MARB_CAPTURESTILL_MODE,
	VIM_MARB_CAPTURESTILLTHUMB_MODE,
	VIM_MARB_CAPTUREMPG4_MODE,

	// Encode modes
	VIM_MARB_ENCODE_MODE,

	// Display modes
	VIM_MARB_DISPLAYSTILL_MODE,
	VIM_MARB_DISPLAYSTILLBYPASS_MODE,
	VIM_MARB_DISPLAYMPG4_MODE,

	// Decode modes
	VIM_MARB_DECODEYUV_MODE, // Without IPP
	VIM_MARB_DECODERGB_MODE,
	
	// Special modes
	VIM_MARB_DIRECTDISPLAY_MODE,
	VIM_MARB_BYPASS_MODE
  Returns:
  	void
  Remarks:
*********************************************************************************/
VIM_MARB_WORKMODE VIM_HAPI_Get_Marb_Mode(void)
{
return gVc0578b_Info.MarbStatus.WorkMode;
}

/********************************************************************************
Description:
	start Capture a stream still(jpeg)
Parameters:
      StillBuf: the buffer to store the jpeg or the file id
      BUF_Length: the buffer length
      pUCallBack: callback function (must set it)
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_WORKMODE(V5H_ERROR_PREVIEWMODE): error work mode
	VIM_ERROR_PREVIEWMODE: preview error
Note:
	if(pUCallBack==NULL) this function will return after finish capturing,
	else it will return before  finish capturing, users should judge the end in callback function.
Remarks:
	This function should be called when preview on.
       pUCallBack return status:
 	V5H_CAPTURE_END: capture end
	V5H_BUF_ERROR:     buffer length is not enough for JPEG     
state: 
	valid
*********************************************************************************/
//revised by youhai_mp4enc 2007.8.30	

UINT32 VIM_HAPI_StartCaptureMp4(UINT32 startpos)
{
      UINT32 ippvalue;
 	gVc0578b_Info.VideoStatus.CapMode=VIM_VIDEO_STARTCAPTURE;
	VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_ENC_DONE,ENABLE);

//revised by youhai_mp4enc 2007.8.30	
	// init intrrupt
	ippvalue=VIM_IPP_Close();
       VIM_HIF_SetReg32(V5_REG_LBUF_CNT_RESET,1);//RESET LINE BUF
	VIM_HIF_SetReg32(V5_REG_MP4ENC_OUTPUT_ADDR,startpos);       
       VIM_MPEG4_SetExternTime(90);
//	if(gVc0578b_Info.Mpeg4Status.m_Enc_Osd_En)
		//VIM_MAPI_Osd_Lcdc_Init_ForDemo();
 	// enable capture
    VIM_MPEG4_StartEnc(1,VIM_USER_MPEG4_FRAMS);
 	VIM_HIF_SetReg32(V5_REG_LBUF_CNT_RESET,0);//RESET LINE BUF
	VIM_IPP_Open(ippvalue);
#if VIM_USER_MPEG4_FRAMEMODE
	//VIM_USER_DelayMs(300);
#endif
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	Stop capture video(mpeg4)
Parameters:

Return:
	V5H_SUCCEED:  ok
	V5H_ERROR_WORKMODE(V5H_ERROR_PREVIEWMODE): error work mode
Note:

Remarks:
state: 
	valid
*********************************************************************************/
UINT16 VIM_HAPI_StopCapture(void)
{
	//check the chip working mode 
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON) 
		return VIM_ERROR_WORKMODE;
	//check the preview  mode 
      	if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_OFF)
       	return VIM_ERROR_PREVIEWMODE;
	if(gVc0578b_Info.VideoStatus.CapMode!=VIM_VIDEO_STARTCAPTURE)
       	return VIM_ERROR_VIDEO_MODE;
	gVc0578b_Info.VideoStatus.CapMode=VIM_VIDEO_WANTSTOP;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintString("**************VIM_HAPI_StopCapture");
#endif
	return VIM_SUCCEED;
}




/***************************************************************************************************************************************
Description:
	change display Video size on LCD
Parameters:

Return:
Note:

Remarks:
state: 
	valid
Modify History:
	 
****************************************************************************************************************************************/
UINT16 VIM_HAPI_ChangeVideoSize(void)
{
	VIM_RESULT result;
       TSize DisplaySize,DestDisplaySize,SoucseSize,AbufSize;
	TPoint pt = {0, 0};

	UINT8 bZoomOut=1;
	
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON&&gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_PHOTOPROCESS) 
		return VIM_ERROR_WORKMODE;

	if(gVc0578b_Info.MarbStatus.WorkMode==VIM_MARB_VIDEOCONFERENCE_MODE)
		return VIM_ERROR_CANNOTSUPPORT_CHANGESIZE;
		
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	

	DisplaySize.cx=gVc0578b_Info.Mpeg4Status.DisInfo.DecWidth;
	DisplaySize.cy=gVc0578b_Info.Mpeg4Status.DisInfo.DecHeigth;
	//from version 0.2 need display whole pic in LCD, so source size =jpeg image size
	SoucseSize.cx=gVc0578b_Info.Mpeg4Status.DisInfo.SorWidth;
	SoucseSize.cy=gVc0578b_Info.Mpeg4Status.DisInfo.SorHeigth;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578][HAPI]mpeg Image width:  ",gVc0578b_Info.Mpeg4Status.DisInfo.SorWidth);
		VIM_USER_PrintDec("[VC0578][HAPI]mpeg Image height:  ",gVc0578b_Info.Mpeg4Status.DisInfo.SorHeigth);
#endif	
	//get real display size(display image in user define display size, maybe not full display in LCD, only part of user define display size)
	if(VIM_USER_DISPLAY_FULLSCREEN)
		result = VIM_IPP_ToolCaculateBigDstWindow(SoucseSize,DisplaySize,&DestDisplaySize);
	else
		result = VIM_IPP_ToolCaculateLessDisplayWindow(SoucseSize,DisplaySize,&DestDisplaySize);
	if(result)
		goto CHANGEDISPLAYMPEG4ERROR;

	if((DisplaySize.cx/2<DestDisplaySize.cx)&&(DestDisplaySize.cx<=DisplaySize.cx))
		DestDisplaySize.cx=DisplaySize.cx;
	else	 if (DestDisplaySize.cx<=DisplaySize.cx/2)
		DestDisplaySize.cx=DestDisplaySize.cx*2;
	else
		bZoomOut=0;
	if((DisplaySize.cy/2<DestDisplaySize.cy)&&(DestDisplaySize.cy<=DisplaySize.cy))
		DestDisplaySize.cy=DisplaySize.cy;
	else	 if (DestDisplaySize.cy<=DisplaySize.cy/2)
		DestDisplaySize.cy=DestDisplaySize.cy*2;	

	if(bZoomOut&&DestDisplaySize.cy<SoucseSize.cy)/*to avoid the situation of diffrent sizer selection of width and height*/
		DestDisplaySize.cy=SoucseSize.cy;
	else if(!bZoomOut&&DestDisplaySize.cy>SoucseSize.cy)
		DestDisplaySize.cy=SoucseSize.cy;		
	

	//DestDisplaySize.cx=DestDisplaySize.cx&0xfff8;
	//DestDisplaySize.cy=DestDisplaySize.cy&0xfff8;
	//set ipp module image size
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578][HAPI]Dest DisplaySize width:  ",DestDisplaySize.cx);
		VIM_USER_PrintDec("[VC0578][HAPI] Dest DisplaySize height:  ",DestDisplaySize.cy);
#endif	
	//set ipp module source window size, display size
	pt.x=pt.y=0;
	if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_VIDEOCONFERENCE_MODE)
		result = VIM_IPP_SetCaptureSize(NULL,pt, SoucseSize, DestDisplaySize);
	else
		result = VIM_IPP_SetDispalySize(pt, SoucseSize, DestDisplaySize);
		
	if(result)
		goto CHANGEDISPLAYMPEG4ERROR;

#if VIM_USER_MPEG4_USE_MAXALAYER
		{
			result =VIM_MAPI_GetAmodeInMpeg4(SoucseSize,DestDisplaySize,(PTSize)&AbufSize,(VIM_DISP_BUFFERMODER *)&gVc0578b_Info.MarbStatus.ALayerMode);
			if(result)
				goto CHANGEDISPLAYMPEG4ERROR;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("[VC0578][HAPI]new DestDisplaySize x:  ",AbufSize.cx);
			VIM_USER_PrintDec("[VC0578][HAPI]new DestDisplaySize y:  ",AbufSize.cy);
#endif		
			result = VIM_IPP_SetCaptureSize(NULL,pt, SoucseSize, AbufSize);
			if(result)
				goto CHANGEDISPLAYMPEG4ERROR;		
			DestDisplaySize=AbufSize;
			VIM_DISP_SetA1_DisplayPanel(gVc0578b_Info.MainPanel.Point,AbufSize);
			/*use this register to save the value of max size*/
			if(gVc0578b_Info.LcdStatus.ARotationMode%2)
				VIM_DISP_SetBufferMode(gVc0578b_Info.MarbStatus.ALayerMode);
			else
				VIM_DISP_SetBufferMode(VIM_DISP_ONEFRAME);
		}
#endif


			
	//set LCDC A layer memory size and window size
	pt.x=pt.y=0;
	if(DestDisplaySize.cx>DisplaySize.cx)	
		pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
	if(DestDisplaySize.cy>DisplaySize.cy)	
		pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;


#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
		VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
#endif		
	result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
	if(result)
		goto CHANGEDISPLAYMPEG4ERROR;
	
	pt.x=(gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_x)&0xfffc;
	pt.y=(gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_y)&0xfffc;
	if(DestDisplaySize.cx<=DisplaySize.cx)	
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_x)&0xfffc;
	else
		DestDisplaySize.cx=DisplaySize.cx;
	if(DestDisplaySize.cy<=DisplaySize.cy)	
		pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.Mpeg4Status.DisInfo.DecPoint_y)&0xfffc;
	else
		DestDisplaySize.cy=DisplaySize.cy;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
		VIM_USER_PrintDec("[VC0578][HAPI] a display offset y:  ",pt.y);
		VIM_USER_PrintDec("[VC0578][HAPI] new DisplaySize width:  ",DestDisplaySize.cx);
		VIM_USER_PrintDec("[VC0578][HAPI] new DisplaySize height:  ",DestDisplaySize.cy);
#endif	

	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);
	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
					VIM_DISP_NOTCHANGE);
	if(result)
		return result;
	VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
	//enable a layer
	VIM_DISP_ResetState();
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);

	    //pisel rate
       VIM_JPEG_AdjustClkBlank();

	return VIM_SUCCEED;
CHANGEDISPLAYMPEG4ERROR:
	if(gVc0578b_Info.DisplayStatus.MallocPr)
	{
		VIM_USER_FreeMemory(gVc0578b_Info.DisplayStatus.MallocPr);
		gVc0578b_Info.CaptureStatus.MallocPr=0;
	}
	if(gVc0578b_Info.VideoStatus.DecCallback)
		gVc0578b_Info.VideoStatus.DecCallback(VIM_HAPI_MPE4_ERROR,0,0);
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_1,DISABLE);
	VIM_HIF_ClearIntModule(VM_HIF_INT_VDEC);
	if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_VIDEOCONFERENCE_MODE)
	{
		VIM_HIF_ClearIntModule(VM_HIF_INT_MARB);
		//VIM_USER_StopTimer();
	}	
	return result;
}

#endif

#if VIM_USER_SUPPORT_TV


/***********************************************************************************
Description:
		Output the image from sensor To TV
Parameters:
		MODE:
			VIM_HAPI_TVPREVIEW_ON
			VIM_HAPI_TVPREVIEW_OFF
			
Returns:
		success	:error message or VM_SUCCEED.

Remarks:
		Before call the function,You Must call VIM_HAPI_EnableTV
************************************************************************************/
UINT16 VIM_HAPI_SetTVPreviewMode(VIM_HAPI_TVPREVIEW_MODE MODE)
{
	VIM_RESULT result;
	
	VIM_TVDC_EnableAdpcm(DISABLE);
	if(MODE == VIM_HAPI_TVPREVIEW_OFF)
		return VIM_SUCCEED;

	gVc0578b_Info.PreviewStatus.Size.cx >>= 4;
	gVc0578b_Info.PreviewStatus.Size.cx <<= 4;
	
	gVc0578b_Info.PreviewStatus.Size.cy >>= 3;
	gVc0578b_Info.PreviewStatus.Size.cy <<= 3;

	gVc0578b_Info.PreviewStatus.Point.x >>= 1;
	gVc0578b_Info.PreviewStatus.Point.x <<= 1;

	gVc0578b_Info.PreviewStatus.Point.y >>= 1;
	gVc0578b_Info.PreviewStatus.Point.y <<=  1;

	result = VIM_TVDC_SetActiveFrameStartPoint(gVc0578b_Info.PreviewStatus.Point);
	if(result)
	{
		return result;
	}

	result = VIM_TVDC_SetAdpcmSize(gVc0578b_Info.PreviewStatus.Size);
	if(result)
	{
		return result;
	}

		
	VIM_HAPI_SetCaptureParameter(gVc0578b_Info.PreviewStatus.Size.cx , gVc0578b_Info.PreviewStatus.Size.cy);
	gVc0578b_Info.MarbStatus.ALayerMode = VIM_DISP_TVOUT_TWOFRAME;
	VIM_HAPI_SetPreviewMode(VIM_HAPI_PREVIEW_FRAMEON, gVc0578b_Info.PreviewStatus.bOutputIndex);
	result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_TVOUT_TWOFRAME,(PVIM_MARB_Map)( &gVc0578b_Info.MarbStatus.MapList));
	if(result)
	{
		return result;
	}	
	VIM_USER_DelayMs(2);


	VIM_TVDC_EnableCaptureSize(DISABLE);
	
	VIM_TVDC_OpenTV(VIM_TVDC_MODE_NORMAL) ;
	return VIM_SUCCEED;

	return 0;
}


/***********************************************************************************
Description:
		Output the image from sensor To TV
Parameters:
		format:
			VIM_HAPI_TVDC_TVFMT_PAL
			VIM_HAPI_TVDC_TVFMT_NTSC
		enable:
			ENALBE or DISABLE
		backcolor:
			backcolor is one 32bit data,
			bit0-7: 	B
			bit8-15	G
			bit16-24	R
Returns:
		success	:error message or VM_SUCCEED.

Remarks:
		if you want do tv operation,you must call this VIM_HAPI_EnableTV use ENABLE parameter.
		if you don't want do tv operation,call it use DISABLE parameter.
************************************************************************************/
void VIM_HAPI_EnableTV(VIM_HAPI_TVDC_TVFORMAT format,UINT8 enable,UINT32 backcolor)
{
	if(enable == ENABLE)
	{
		gVc0578b_Info.DisplayStatus.outputtype = VIM_OUTPUTTV;
	
		VIM_TVDC_SetTvFormat((VIM_TVDC_TVFORMAT)format);
		VIM_TVDC_SetBackColor(backcolor);
		VIM_TVDC_EnableOsd(DISABLE);
		VIM_TVDC_EnableVdecSync(TRUE);
		
	}
	else
	{
		gVc0578b_Info.DisplayStatus.outputtype = VIM_OUTPUTLCD;
		VIM_TVDC_EnableVdecSync(FALSE);
		VIM_TVDC_CloseTV();
	}
	return;	
}



/***********************************************************************************
Description:
		set osd on tv
Parameters:
		startx,starty: 	the position of the left and top corner.
		alph:		limpid parameter.
		enable:		ENABLE,show the osd
					DISABLE:hide the osd
		bmp:		one pointer of bmp
Returns:
		success	:error message or VM_SUCCEED.
Remarks:
		if you want do tv operation,you must call this VIM_HAPI_EnableTV use ENABLE parameter.
************************************************************************************/
UINT16 VIM_HAPI_SetTVOSD(UINT16 startx,UINT16 starty,UINT8 alph,UINT8 enable,UINT8 *bmp)
{
	VIM_RESULT result;
	TPoint pt;
	TSize size;
	UINT32 palettelen;
	UINT16 biBitCount;
	UINT16 biWidth,biHeight;
	UINT32 bfOffBits,biSizeImage;

	
	if(VIM_MAPI_BMP_TESTOK(bmp) == FALSE)
	{
		return VIM_ERROR_TVDC_BMP_FORMAT;
	}

	biBitCount = (bmp[28]|(bmp[29]<<8));
	switch(biBitCount)
	{
		case 2:
			palettelen = 4;
			VIM_TVDC_SetOsdbppFormat(VIM_TVDC_OSDFMT_2BPP);
			break;
		case 4:
			palettelen = 16;
			VIM_TVDC_SetOsdbppFormat(VIM_TVDC_OSDFMT_4BPP);			
			break;
		case 8:
			palettelen = 256;
			VIM_TVDC_SetOsdbppFormat(VIM_TVDC_OSDFMT_8BPP);
			break;
	}
	VIM_TVDC_SetOsdAlphablending(alph);

	pt.x=startx;
	pt.y=starty;

	biWidth = (bmp[18] | (bmp[19]<<8) | (bmp[20]<<16) |(bmp[21]<<24));
	biHeight = (bmp[22] |(bmp[23]<<8) | (bmp[24]<<16) |(bmp[25]<<24));
	size.cx = biWidth;
	size.cy = biHeight;

	result = VIM_TVDC_SetOsdPanel(pt,size);	
	if(result)
		return result;


	//VIM_MAPI_Palette_RGB2YUV(bmp+54,bmp+54,palettelen);
	result = VIM_TVDC_SetRGBPalette(palettelen,(bmp+54));
	if(result)
		return result;	
	
	result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_TVOUT_TWOFRAME,(PVIM_MARB_Map)( &gVc0578b_Info.MarbStatus.MapList));
	if(result)
	{
		return result;
	}	

	bfOffBits=(bmp[10]|(bmp[11]<<8)|(bmp[12]<<16)|(bmp[13]<<24));
	biSizeImage=(bmp[34]|(bmp[35]<<8)|(bmp[36]<<16)|(bmp[37]<<24));
	VIM_HIF_WriteSram(gVc0578b_Info.MarbStatus.MapList.layerB1start, bmp+bfOffBits,biSizeImage, VIM_HIF_SRAM_MODE);

	//VIM_TVDC_Enable_Capture_Size(DISABLE);

	if(enable)
		VIM_TVDC_EnableOsd(ENABLE);
	else
		VIM_TVDC_EnableOsd(DISABLE);
	return VIM_SUCCEED;
}
#endif
/***************************************************************************************************************************************
Description:
	set display mode and parameter, display one frame or more frame jpeg picture on LCD
Parameters:
      jpegBuf: 	the buffer point of jpeg
      length: 	the length of jpeg (=>jpeg file length)
      LcdOffsetX,LcdOffsetY		: the display coordinate on LCD panel.
      DisplayWidth,DisplayHeigth	: the display width and height.
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_WORKMODE: error work mode
	VIM_ERROR_YUV_UNKNOW:jpeg file yuv mode error, can not display
	VIM_ERROR_PARAMETER: marb jbuf size less than capture size, so data is not good
	VIM_ERROR_?????????: return form mid api
Note:
	LcdOffsetX+DisplayWidth<=LCD width
	LcdOffsetY+DisplayHeigth<=LCD height
	length=file length(this is important)
Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_ReadyToDecodeJpeg(PVIM_JPEG_DECODE_INFO DecodeInfo)
{
	VIM_RESULT result;
	TSize DestDisplaySize;
	TSize WindowSize,DisplaySize,ImageSize;
	TPoint pt = {0, 0};
	TPoint WindowPt = {0, 0};
	TPoint DisplayPt = {0, 0};

	//disable syncgen
	VIM_SIF_EnableSyncGen(DISABLE); //angela 2006-8-15	
	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;		

	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON&&gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_PHOTOPROCESS) 
		return VIM_ERROR_WORKMODE;
		
	VIM_USER_MemCpy((UINT8 *)&gVc0578b_JpegInfo,(UINT8 *)&(DecodeInfo->ImageInfo),sizeof(gVc0578b_JpegInfo));	
	ImageSize.cx = DecodeInfo->ImageInfo.JpgSize.cx;
	ImageSize.cy = DecodeInfo->ImageInfo.JpgSize.cy;
	// 4. get source window size and position(window size<=image size)
	WindowSize.cx = DecodeInfo->JpegWinSize.cx=gVc0578b_JpegInfo.ImageSize.cx;
	WindowSize.cy = DecodeInfo->JpegWinSize.cy=gVc0578b_JpegInfo.ImageSize.cy;
	WindowPt.x = DecodeInfo->JpegOffsetPt.x=0;
	WindowPt.y = DecodeInfo->JpegOffsetPt.y=0;
    	if((WindowPt.x+WindowSize.cx)>(DecodeInfo->ImageInfo.ImageSize.cx))
		WindowPt.x = (DecodeInfo->ImageInfo.ImageSize.cx) - WindowSize.cx;
	if((WindowPt.y+WindowSize.cy)>(DecodeInfo->ImageInfo.ImageSize.cy))
		WindowPt.y = (DecodeInfo->ImageInfo.ImageSize.cy) - WindowSize.cy;
	WindowSize.cx =(WindowSize.cx&0xfffe);
	WindowSize.cy =(WindowSize.cy&0xfffe);
	WindowPt.x=(WindowPt.x&0xfffe);	
	WindowPt.y=(WindowPt.y&0xfffe);
	
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]image width: ",ImageSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]image height:",ImageSize.cy);
	VIM_USER_PrintDec("[VC0578][HAPI]source window width: ",WindowSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]source window height:",WindowSize.cy);
	VIM_USER_PrintDec("[VC0578][HAPI]source window xpos:	",WindowPt.x);
	VIM_USER_PrintDec("[VC0578][HAPI]source window ypos: ",WindowPt.y);
#endif	

	// 5. get display size and position(display size<=lcd size)
	DisplaySize.cx = DecodeInfo->LcdWantSize.cx;
	DisplaySize.cy = DecodeInfo->LcdWantSize.cy;
	DisplayPt.x = DecodeInfo->LcdWantPt.x;
	DisplayPt.y = DecodeInfo->LcdWantPt.y;
	DisplaySize.cx=(DisplaySize.cx&0xfffe);	
	DisplaySize.cy=(DisplaySize.cy&0xfffe);
	DisplayPt.x=(DisplayPt.x&0xfffe);	
	DisplayPt.y=(DisplayPt.y&0xfffe);		

	gVc0578b_Info.DecodeStatus.DecodeMode=DecodeInfo->DecodeMode;
	gVc0578b_Info.LcdStatus.ARotationMode=(VIM_DISP_ROTATEMODE)0;
	gVc0578b_Info.DecodeStatus.TargetSize.cx=DisplaySize.cx;
	gVc0578b_Info.DecodeStatus.TargetSize.cy=DisplaySize.cy;
	gVc0578b_Info.DecodeStatus.YUVmode=gVc0578b_JpegInfo.YUVType;
	gVc0578b_Info.DecodeStatus.JpegSize.cx=gVc0578b_JpegInfo.JpgSize.cx;
	gVc0578b_Info.DecodeStatus.JpegSize.cy=gVc0578b_JpegInfo.JpgSize.cy;
	
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]jpeg Image width:	",gVc0578b_JpegInfo.ImageSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]jpeg Image height:  ",gVc0578b_JpegInfo.ImageSize.cy);
	VIM_USER_PrintDec("[VC0578][HAPI]yuv is:  %d",gVc0578b_JpegInfo.YUVType);
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image width:	",gVc0578b_JpegInfo.JpgSize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI]adj jpeg Image height:  ",gVc0578b_JpegInfo.JpgSize.cy);
#endif
	VIM_IPP_Reset();
	VIM_MAPI_SetDecodeJpegMode(DecodeInfo->DecodeMode);	
	
	if(DecodeInfo->DecodeMode==VIM_DECODE_IPP)
	{
		/*result = VIM_IPP_ToolCaculateLessDisplayWindow(ImageSize,DisplaySize,&DestDisplaySize);
		if(result)
		{goto DECODEERROR;}		
		
		gVc0578b_Info.DecodeStatus.TargetSize.cx=DestDisplaySize.cx;
		gVc0578b_Info.DecodeStatus.TargetSize.cy=DestDisplaySize.cy;*/
		VIM_IPP_SetImageSize(ImageSize.cx,ImageSize.cy);
		//set ipp module source window size, display size
		pt.x=0;//(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
		pt.y=0;//(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;	
		DestDisplaySize.cx=gVc0578b_Info.DecodeStatus.TargetSize.cx;
		DestDisplaySize.cy=gVc0578b_Info.DecodeStatus.TargetSize.cy;
		result = VIM_IPP_SetDispalySize(pt, WindowSize, DestDisplaySize);//*different from Display Jpeg mode
		if(result)
		goto DECODEERROR;
		VIM_JPEG_SetLbuf1Line(gVc0578b_Info.DecodeStatus.YUVmode);
		gVc0578b_Info.MarbStatus.ALayerMode = VIM_DISP_NODISP;
	}
	else if(DecodeInfo->DecodeMode==VIM_DECODE_BYPASS_IPP)
	{
		/*result = VIM_IPP_ToolCaculateLessDisplayWindow(ImageSize,DisplaySize,&DestDisplaySize);
		if(result)
		{goto DECODEERROR;}		
		
		gVc0578b_Info.DecodeStatus.TargetSize.cx=DestDisplaySize.cx;
		gVc0578b_Info.DecodeStatus.TargetSize.cy=DestDisplaySize.cy;*/
		VIM_IPP_SetImageSize(ImageSize.cx,ImageSize.cy);
		//set ipp module source window size, display size
		pt.x=0;//(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
		pt.y=0;//(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;	
		DestDisplaySize.cx=gVc0578b_Info.DecodeStatus.TargetSize.cx;
		DestDisplaySize.cy=gVc0578b_Info.DecodeStatus.TargetSize.cy;
		result = VIM_IPP_SetDispalySize(pt, WindowSize, DestDisplaySize);//*different from Display Jpeg mode
		if(result)
		goto DECODEERROR;
		VIM_JPEG_SetLbuf1Line(gVc0578b_Info.DecodeStatus.YUVmode);
		gVc0578b_Info.MarbStatus.ALayerMode = VIM_DISP_NODISP;
	}
	else if(DecodeInfo->DecodeMode==VIM_DECODE_FRAME)
	{
		/*result = VIM_IPP_ToolCaculateLessDisplayWindow(ImageSize,DisplaySize,&DestDisplaySize);
		if(result)
		{goto DECODEERROR;}	

		gVc0578b_Info.DecodeStatus.TargetSize.cx=DestDisplaySize.cx;
		gVc0578b_Info.DecodeStatus.TargetSize.cy=DestDisplaySize.cy;*/
		VIM_IPP_SetImageSize(ImageSize.cx,ImageSize.cy);
		//set ipp module source window size, display size
		pt.x=0;//(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
		pt.y=0;//(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc; 
		DestDisplaySize.cx=gVc0578b_Info.DecodeStatus.TargetSize.cx;
		DestDisplaySize.cy=gVc0578b_Info.DecodeStatus.TargetSize.cy;
		result = VIM_IPP_SetDispalySize(pt, WindowSize, DestDisplaySize);//*different from Display Jpeg mode
		if(result)
		goto DECODEERROR;		

		//set LCDC A layer memory size and window size
		pt.x=pt.y=0;
		if(DestDisplaySize.cx>DisplaySize.cx)	
			pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
		if(DestDisplaySize.cy>DisplaySize.cy)	
			pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
		VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
#endif
		result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
		if(result)
			goto DECODEERROR;
	
		pt.x=(gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
		pt.y=(gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;

		if(DestDisplaySize.cx<=DisplaySize.cx)	
			pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
		else
			DestDisplaySize.cx=DisplaySize.cx;
		if(DestDisplaySize.cy<=DisplaySize.cy)	
			pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;
		else
			DestDisplaySize.cy=DisplaySize.cy;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
		VIM_USER_PrintDec("[VC0578][HAPI]a display offset y:  ",pt.y);
		VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize width:  ",DestDisplaySize.cx);
		VIM_USER_PrintDec("[VC0578][HAPI]new DisplaySize height:  ",DestDisplaySize.cy);
#endif	
		VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
		//enable a layer
		VIM_DISP_ResetState();
		//VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
		VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);		
		VIM_JPEG_SetLbuf1Line(gVc0578b_Info.DecodeStatus.YUVmode);
		gVc0578b_Info.MarbStatus.ALayerMode = VIM_DISP_LINEBUF;		
	}
	else
	{
		gVc0578b_Info.MarbStatus.ALayerMode = VIM_DISP_NODISP;
	}
	//set layer A mode, normal display use ALAYER_LINEBUF mode

	result = VIM_MAPI_SetDisplayJpegParameter();
	if(result)
		goto DECODEERROR;
	VIM_MAPI_InitDecoInterrupt();
		
	gVc0578b_Info.DecodeStatus.DecodePoint=(UINT8 *)DecodeInfo->JpegDecodeBufAddr;
	gVc0578b_Info.DecodeStatus.DecodeBufLength=DecodeInfo->JpegDecodeBufLen;
	
	VIM_MAPI_DecodelbufEn(gVc0578b_Info.MarbStatus.WorkMode);

DECODEERROR:
	if(result)
	{
		return (UINT16)result;
	}
	return VIM_SUCCEED;
}

UINT32 VIM_HAPI_LbufRead(void)
{
	UINT32 *lbuf1Addr;
	UINT32 *decbuf;
	UINT32 image_width,image_height,filelen=0;
	UINT32 yuv_mode,decode_mode;
	UINT32 buflen;
	TSize image_size;

	yuv_mode=gVc0578b_Info.DecodeStatus.YUVmode;
	decode_mode=gVc0578b_Info.DecodeStatus.DecodeMode;
	decbuf=(UINT32 *)gVc0578b_Info.DecodeStatus.DecodePoint;
	VIM_USER_PrintString("lbuf_cpm_read_int\n");
	
	switch(decode_mode)
	{
		case VIM_DECODE_IPP:
		case VIM_DECODE_FRAME:
		case VIM_DECODE_BYPASS_IPP:
		case VIM_DECODE_BYPASS_FRAME:
			lbuf1Addr = (UINT32 *)VIM_HIF_GetReg32(V5_REG_LBUF_Y_INI1_DEC);
			VIM_IPP_GetDispSize(&image_size);
			image_width = image_size.cx;
			image_height=VIM_HIF_GetReg8(V5_REG_LBUF_STATUS+1);
			image_height&=0xff;
			//printf("get lines is %d",image_height);
			buflen=image_width*2*image_height;
			if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
			{
				//printf("decode buffer overflow!\n");
				return VIM_ERROR_DECODE;
			}
			get_video_data((UINT8 *)decbuf,(UINT8 *)lbuf1Addr, buflen);
			gVc0578b_Info.DecodeStatus.DecodePoint+=buflen;
			gVc0578b_Info.DecodeStatus.DecodeBufLength-=buflen;
			gVc0578b_Info.DecodeStatus.DecFileLength+=buflen;
			gVc0578b_Info.DecodeStatus.DecodeLine+=image_height;
			VIM_MAPI_DecodelbufEn(gVc0578b_Info.MarbStatus.WorkMode);
			if(gVc0578b_Info.DecodeStatus.DecodeLine>= image_size.cy)
			{
				//gVc0578b_Info.DecodeStatus.DecCallback(VIM_HAPI_LBUF_END,gVc0578b_Info.DecodeStatus.DecFileLength);
				filelen=gVc0578b_Info.DecodeStatus.DecFileLength;
				gVc0578b_Info.DecodeStatus.DecFileLength=0;
				gVc0578b_Info.DecodeStatus.DecodeLine=0;
				return filelen;
			}
			break;
		default:
			break;
	}
	VIM_JPEG_RestartDecode();
	return 0;
}

UINT32 VIM_HAPI_GetLbufReadStatus(void)
{
	UINT32 marbflag=0;	
	marbflag=VIM_HIF_GetReg8(V5_REG_IPP_Q_HISTOGRAM);
	return marbflag;
}

void VIM_HAPI_SetLbufReadStatus(UINT32 marbflag)
{
	VIM_HIF_SetReg8(V5_REG_IPP_Q_HISTOGRAM,marbflag);
}

/***************************************************************************************************************************************
Description:
	set display mode and parameter, display one frame or more frame jpeg picture on LCD
Parameters:
      Info: 	the struct that used to transform the output infomation
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_WORKMODE: error work mode
	VIM_ERROR_YUV_UNKNOW:jpeg file yuv mode error, can not display
	VIM_ERROR_PARAMETER: marb jbuf size less than capture size, so data is not good
	VIM_ERROR_?????????: return form mid api
Note:
	none
	Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_StartDecodeJpeg(void)
{

	gVc0578b_Info.DecodeStatus.DecFileLength=0;
	gVc0578b_Info.DecodeStatus.DecodeLine=0;
	VIM_MAPI_StartDecode();
			
	return 0;
}

void VIM_HAPI_DecodeJpeg_Release(void)
{
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	VIM_JPEG_ResetState();
	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x01);
	VIM_USER_DelayMs(1);
	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x0);
	VIM_IPP_Reset();

	VIM_MAPI_FreeDecodeJpegBuffer();
	gVc0578b_Info.MarbStatus.WorkMode=VIM_MARB_NONE_MODE;

}
#if OTHERDRV 

/********************************************************************************
   		other driver
*******************************************************************************/

/***************************************************************************************************************************************
Description:
	Set freq acording the rules in the place 
Parameters:
	freq:
	VIM_HAPI_FREQ_50HZ =		0x0,
	VIM_HAPI_FREQ_60HZ =		0x1,
Return:
Note:

Remarks:
state: 
	valid
Modify History:
	 
****************************************************************************************************************************************/
void VIM_HAPI_SetFreq(VIM_HAPI_FREQ freq)
{
	gVc0578b_Info.VIM_SIF_AeAdjust.env = freq;
	gVc0578b_Info.VIM_SIF_AeAdjust.freq = (VIM_SIF_FREQ_60HZ == freq) ? EPFREQ_60HZ : EPFREQ_50HZ;
}

/********************************************************************************
Description:
	SET preview MODE
Parameters:
	MODE: preview mode
		VIM_PREVIEW_OFF: preview off
		VIM_HAPI_PREVIEW_ON: preview on without frame
		VIM_HAPI_PREVIEW_FRAMEON: preview on with frame
	bIndex: is the index of resolution (see the sensorinfo.c)
	Return:
		VIM_SUCCEED: set mode ok
		VIM_ERROR_WORKMODE: error work mode
		VIM_ERROR_????????: Return from middle API function
	Note:
		This function must be used in Camera open mode
	Remarks:
		state: valid
*********************************************************************************/
UINT16 VIM_HAPI_SetVideoConferencePreviewMode(VIM_HAPI_PREVIEW_MODE MODE)
{
VIM_RESULT result;
TSize Amem,InputSize,CaptureSize,SourceWin;
TPoint pt;
	
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON)
		return VIM_ERROR_WORKMODE;

       switch(MODE)
	{
		case VIM_HAPI_PREVIEW_ON:
		case VIM_HAPI_PREVIEW_FRAMEON:		
			//VIM_USER_StopTimer();
			//VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_YMEAN,DISABLE);
			
			VIM_SIF_EnableSyncGen(DISABLE);
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else			
			VIM_HIF_SetReg32(V5_REG_ISP_RESET, 1);
#endif			
			
			//set chip work mode
                    if(MODE!=VIM_HAPI_PREVIEW_FRAMEON)
		 		VIM_MAPI_SetChipMode(VIM_MARB_VIDEOCONFERENCE_MODE,VIM_IPP_HAVE_NOFRAME);	//actually capture mode
			else
			{
				VIM_MAPI_SetChipMode(VIM_MARB_VIDEOCONFERENCE_MODE,VIM_IPP_HAVEFRAME);
			}
			gVc0578b_Info.PreviewStatus.bOutputIndex=VIM_HAPI_HALFPIXEL;
                  	result = VIM_SIF_SetSensorResolution(gVc0578b_Info.pSensorInfo,&gVc0578b_Info.VIM_SIF_AeAdjust,(VIM_SIF_RESOLUTION)VIM_HAPI_HALFPIXEL,VIM_SIF_SNSOR_FRAMERATE_FORCAPVIDEO);//*diferrence
			if(result)
		           return (UINT16)result;
			//get input size
			result = VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, (VIM_SIF_RESOLUTION)VIM_HAPI_HALFPIXEL, &InputSize);
			if(result)
		           return (UINT16)result;

			VIM_IPP_SetImageSize(InputSize.cx, InputSize.cy);
			CaptureSize=gVc0578b_Info.CaptureStatus.Size;

			//change capture size
			if((CaptureSize.cx>InputSize.cx)||(CaptureSize.cy>InputSize.cy))		//* difference, added
			{
				return VIM_ERROR_OVERSIZE_VIDEOCON;
			}
			//get source window size
			result = VIM_IPP_ToolCaculateLessSrcWindow(InputSize,CaptureSize,&SourceWin);
			if(result)
				return (UINT16)result;
			//get source window start position	
			pt.x=((InputSize.cx-SourceWin.cx)/2)&0xfffe;
			pt.y=((InputSize.cy-SourceWin.cy)/2)&0xfffe;
			result = VIM_IPP_SetCaptureSize(gVc0578b_Info.pSensorInfo,pt, SourceWin, CaptureSize); 
			if(result)
				return (UINT16)result;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
				VIM_USER_PrintDec("\n sensor output width: ",InputSize.cx);
				VIM_USER_PrintDec("\n sensor output height: ",InputSize.cy);
				VIM_USER_PrintDec("\n Source window width: ",SourceWin.cx);
				VIM_USER_PrintDec("\n Source window height: ",SourceWin.cy);
				VIM_USER_PrintDec("\n Capture window width:  ",CaptureSize.cx);
				VIM_USER_PrintDec("\n Capture window height:  ",CaptureSize.cy);
#endif	
			//get part size of source win will display in LCD
			result = VIM_IPP_ToolCaculateBigDstWindow(SourceWin, gVc0578b_Info.PreviewStatus.Size,&InputSize);
			if(result)
				return (UINT16)result;
			
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("\n  display InputSize window width:",InputSize.cx);
			VIM_USER_PrintDec("\n  display InputSize window height:  ",InputSize.cy);
#endif	
			InputSize=gVc0578b_Info.PreviewStatus.Size;
			result = VIM_IPP_SetThumbSize((UINT8)InputSize.cx, (UINT8)InputSize.cy,VIM_IPP_THUMB_FROM_CAPTURE);
			if(result)
				return (UINT16)result;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("\n new display InputSize window width:",InputSize.cx);
			VIM_USER_PrintDec("\n new display InputSize window height:  ",InputSize.cy);
			VIM_USER_PrintDec("preivew  window width:",gVc0578b_Info.PreviewStatus.Size.cx);
			VIM_USER_PrintDec("preivew window height:  ",gVc0578b_Info.PreviewStatus.Size.cy);			
#endif	
			
	 		gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize=SourceWin;
	 		gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize = InputSize;
			VIM_DISP_SetA1_DisplayPanel(gVc0578b_Info.PreviewStatus.Point,gVc0578b_Info.PreviewStatus.Size);

			result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
					VIM_DISP_NOTCHANGE);			//wendy guo 4/18/2007, add this function to test A0 layer rotation!
			
			if(result)
				return (UINT16)result;
			VIM_DISP_SetRotateMode((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode);
			
			VIM_JPEG_SetSize(JPEG_420,CaptureSize);

			gVc0578b_Info.MarbStatus.ALayerMode =VIM_DISP_ONEFRAME;

			result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
		             (PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
			if(result)
				return result;
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
				VIM_USER_PrintDec(" user a layter mode :",gVc0578b_Info.MarbStatus.ALayerMode);
#endif	
			////////////zoom ///////////////////
			if ( gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize.cx >= 
				(CaptureSize.cx/2 + VIM_USER_MIN_PIXEL_ONESTEP * VIM_USER_MAX_STEP))
			{
				result=VIM_MAPI_Preview_CaptureZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
			}
			else
			{
				result=VIM_MAPI_Preview_DisplayZoom(gVc0578b_Info.PreviewStatus.ZoomPara.CurrentZoomStep);
			}
			VIM_IPP_SetDisplayDropFrame(0xffff);
			VIM_SIF_EnableSyncGen(ENABLE);	

			VIM_MAPI_Delay_Frame(1);   //* difference from 528 code: _Frame(2) //Delay for the stable sensor data output
			VIM_DISP_ResetState();
			VIM_DISP_SetLayerEnable(VIM_DISP_A1LAYER,(ENABLE));				
			if(!gVc0578b_Info.pSensorInfo->snrtype)	//rgb type
			{
#if VIM_SENSOR_HAVE_BAYER
#if VIM_SENSOR_AUTO_FOCUS
				VIM_USER_SetAutoFocusStep(gVc0578b_Info.aeaf.rightstep);
#endif
				VIM_MAPI_AEIsrInit();
#endif
			}
			//VIM_USER_StartTimer(100);		//*difference from 528 code, this function have not finished!
			//VIM_IPP_SetEffect(VIM_IPP_EFFECT_FOUR_COLOR);
#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
				VIM_USER_PrintDec(" %%%%%%% :",VIM_HIF_GetReg32(V5_REG_IPP_WINDOW_UPDATE));
#endif
			break;
		case VIM_HAPI_PREVIEW_OFF:
		default:
			//VIM_USER_StopTimer();
			VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);//angela 2007-1-25	
			VIM_MAPI_Delay_Frame(1);
		 	VIM_SIF_EnableSyncGen(DISABLE);//next frame
			VIM_USER_DelayMs(50);/*added by shiyong for bug10586*/
		 	pt.x=pt.y=Amem.cx=Amem.cy=0;
		 	VIM_DISP_SetA_Memory(pt,Amem);
		}
	gVc0578b_Info.PreviewStatus.Mode=MODE;
	return VIM_SUCCEED;
}
#if (BOARD_TYPE==VC0820_FPGA)//add by zhoujian

/***************************************************************************************************************************************
Description:
	set encode mode and parameter, encode one frame 
Parameters:
      jyuvdataBuf: 	the buffer point of YUV block data
      ImageSize: 	YUV image size
      YUVmode	:      the YUV mode of image
      VIM_HAPI_CPTURE_QUALITY	: the encode qulity
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_?????????: return form mid api
Note:
	
Remarks:
state: 
	valid
****************************************************************************************************************************************/

UINT16 VIM_HAPI_ReadyToEncodeJpeg(HUGE void *yuvdataBuf,TSize ImageSize,VIM_JPEG_YUVMODE YUVmode,VIM_HAPI_CPTURE_QUALITY Quality)
{
	VIM_RESULT result;
	
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON&&gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_PHOTOPROCESS) 
		return VIM_ERROR_WORKMODE;

	VIM_SIF_EnableSyncGen(DISABLE); //angela 2006-8-15	
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);
	
	
	if(gVc0578b_Info.MarbStatus.WorkMode==VIM_MARB_CAPTURESTILLTHUMB_MODE)
	{
	
		gVc0578b_Info.EncodeStatus.YUVBufPoint=gVc0578b_Info.CaptureStatus.ThumbBuf;
		gVc0578b_Info.EncodeStatus.JpegSize=gVc0578b_Info.CaptureStatus.ThumbSize;
	}
	else 
	{
		gVc0578b_Info.EncodeStatus.YUVBufPoint=(UINT8 *)yuvdataBuf;
		gVc0578b_Info.EncodeStatus.JpegSize=ImageSize;
	}
	
	if(gVc0578b_Info.EncodeStatus.YUVBufPoint==NULL)
			return VIM_ERROR_BUFPOINT;
	
	
	VIM_USER_PrintHex(" [VC0578B][MARB]YUVBufPoint    ",(UINT32)gVc0578b_Info.EncodeStatus.YUVBufPoint);
	gVc0578b_Info.EncodeStatus.YUVmode=YUVmode;
	gVc0578b_Info.EncodeStatus.QualityMode=Quality;
	
	gVc0578b_Info.CaptureStatus.Mode=VIM_CAPTURESTILL;
	gVc0578b_Info.CaptureStatus.QualityMode=gVc0578b_Info.EncodeStatus.QualityMode;
	
	VIM_MAPI_SetChipMode(VIM_MARB_ENCODE_MODE,VIM_IPP_HAVE_NOFRAME);
	//VIM_HIF_SetReg32(0x60010118,0x00000018);
	
	VIM_JPEG_SetSize(YUVmode,gVc0578b_Info.EncodeStatus.JpegSize);
	// angela added it for set capture ratio before preview on
	result=VIM_MAPI_SetCaptureRatio(gVc0578b_Info.CaptureStatus.QualityMode);
	if(result)
		return (UINT16)result;

	if(gVc0578b_Info.CaptureStatus.ThumbBuf==0)
	{
		result=VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_NODISP,
			(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
		
		if(result)
			return (UINT16)result;
	}

	VIM_MARB_SetCaptureNum(VIM_MARB_FIFO_0,1);
		
	VIM_MARB_SetJpegbufGapratio(5,VIM_MARB_FIFO_0);
	VIM_MARB_SetRecaptureEn(VIM_MARB_FIFO_0,DISABLE);

	
	VIM_MAPI_InitCapInterrupt();

	return VIM_SUCCEED;	
}
/***************************************************************************************************************************************
Description:
	encode one jpeg image
Parameters:
      StillBuf: the buffer to store the jpeg 
      BUF_Length: the buffer length
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_?????????: return form mid api
Note:
	none
	Remarks:
state: 
	valid
****************************************************************************************************************************************/

UINT16 VIM_HAPI_StartEncodeJpeg(HUGE void *StillBuf,UINT32 BUF_Length,VIM_HAPI_CallBack pUCallBack)
{
	UINT8 status;
	UINT8 *yuvdatabuf;
	TSize JpegSize;
	VIM_JPEG_YUVMODE YUVmode;
		
	gVc0578b_Info.CaptureStatus.BufPoint=gVc0578b_Info.EncodeStatus.BufPoint=(UINT8 *)StillBuf;
	gVc0578b_Info.CaptureStatus.CapFileLength=0;
	gVc0578b_Info.CaptureStatus.CapCallback=gVc0578b_Info.EncodeStatus.CapCallback=pUCallBack;
	gVc0578b_Info.CaptureStatus.BufLength=gVc0578b_Info.EncodeStatus.BufLength=BUF_Length;	
	gVc0578b_Info.CaptureStatus.CaptureError=VIM_SUCCEED;
	yuvdatabuf=gVc0578b_Info.EncodeStatus.YUVBufPoint;
	JpegSize=gVc0578b_Info.EncodeStatus.JpegSize;
	YUVmode=gVc0578b_Info.EncodeStatus.YUVmode;

	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
	VIM_JPEG_StartEncodeYUV(ENABLE);

	status=VIM_JPEG_GetStatus();
	while(!(status&0x08))//header done
	{
		status=VIM_JPEG_GetStatus();
	}

	VIM_MAPI_SendYUVData(yuvdatabuf,JpegSize,YUVmode);

	return VIM_SUCCEED;	
}

/***************************************************************************************************************************************
Description:
	set display mode and parameter, display one frame or more frame jpeg picture on LCD
Parameters:
      jpegBuf: 	the buffer point of jpeg
      length: 	the length of jpeg (=>jpeg file length)
      LcdOffsetX,LcdOffsetY		: the display coordinate on LCD panel.
      DisplayWidth,DisplayHeigth	: the display width and height.
Return:
	VIM_SUCCEED:  ok
	VIM_ERROR_WORKMODE: error work mode
	VIM_ERROR_YUV_UNKNOW:jpeg file yuv mode error, can not display
	VIM_ERROR_PARAMETER: marb jbuf size less than capture size, so data is not good
	VIM_ERROR_?????????: return form mid api
Note:
	LcdOffsetX+DisplayWidth<=LCD width
	LcdOffsetY+DisplayHeigth<=LCD height
	length=file length(this is important)
Remarks:
state: 
	valid
****************************************************************************************************************************************/
UINT32 VIM_HAPI_DirectDisplay(TSize ImageSize,TSize LCDsize,VIM_JPEG_YUVMODE YUVmode,VIM_HAPI_JPEGCallBack pUCallBack)
{
	VIM_RESULT result;
	TSize JpegSize,DisplaySize,DestDisplaySize;
	TPoint pt = {0, 0};
	
	if(gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_CAMERAON&&gVc0578b_Info.ChipWorkMode!=VIM_HAPI_MODE_PHOTOPROCESS) 
		return VIM_ERROR_WORKMODE;					
	//disable syncgen
	LCDsize.cx&=0xfffe;
	LCDsize.cy&=0xfffe;
	
	VIM_SIF_EnableSyncGen(DISABLE); //angela 2006-8-15	
	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;	
	
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
	VIM_DISP_SetWorkMode(VIM_DISP_DECODE);
	VIM_MAPI_SetChipMode(VIM_MARB_DISPLAYSTILLBYPASS_MODE,VIM_IPP_HAVEFRAME);
	VIM_IPP_SetDisplayDropFrame(0xffff);
	
	gVc0578b_Info.DisplayStatus.JpegCallBack=pUCallBack;
	gVc0578b_Info.DisplayStatus.LcdWantPt=pt;
	gVc0578b_Info.DisplayStatus.LcdWantSize=LCDsize;
	//gVc0578b_Info.DecodeStatus.DecodeMode=VIM_DISPALY_BYPASS;
	//gVc0578b_Info.DecodeStatus.TargetSize=gVc0578b_Info.DisplayStatus.LcdWantSize=LCDsize;
	//gVc0578b_Info.DecodeStatus.YUVmode=gVc0578b_JpegInfo.YUVType=yuvmode;
	gVc0578b_JpegInfo.YUVType=YUVmode;
	gVc0578b_JpegInfo.ImageSize=ImageSize;	
	VIM_JPEG_GetJpegSize(YUVmode,ImageSize,&JpegSize);
	gVc0578b_JpegInfo.JpgSize=JpegSize;
	//gVc0578b_Info.DecodeStatus.JpegSize=gVc0578b_JpegInfo.JpgSize=JpegSize;
	
	result = VIM_MAPI_CheckDisplaySize(gVc0578b_Info.DisplayStatus.LcdWantPt.x,gVc0578b_Info.DisplayStatus.LcdWantPt.y,gVc0578b_Info.DisplayStatus.LcdWantSize.cx,gVc0578b_Info.DisplayStatus.LcdWantSize.cy,&DisplaySize);
	if(result)
		goto DECODEERROR;
	
	//get real display size(display image in user define display size, maybe not full display in LCD, only part of user define display size)
	result = VIM_IPP_ToolCaculateBigDstWindow(gVc0578b_JpegInfo.ImageSize,DisplaySize,&DestDisplaySize);
	//DestDisplaySize.cy=220;
	if(result)
		goto DECODEERROR;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("\n DestDisplaySize width:  ",DestDisplaySize.cx);
		VIM_USER_PrintDec("\n DestDisplaySize height:  ",DestDisplaySize.cy);
#endif	
	//set ipp module image size
	VIM_IPP_SetImageSize(gVc0578b_JpegInfo.JpgSize.cx, gVc0578b_JpegInfo.JpgSize.cy);
	//VIM_IPP_SetImageSize(gVc0578b_JpegInfo.ImageSize.cx, gVc0578b_JpegInfo.ImageSize.cy);//7/23/2007 guoying

	//set ipp module source window size, display size
	pt.x=0;//(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
	pt.y=0;//(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;

	result = VIM_IPP_SetDispalySize(pt,gVc0578b_JpegInfo.JpgSize, DestDisplaySize);//*different from Display Jpeg mode
	if(result)
		goto DECODEERROR;


		//set LCDC A layer memory size and window size
	pt.x=pt.y=0;
	if(DestDisplaySize.cx>DisplaySize.cx)	
		pt.x=(((DestDisplaySize.cx-DisplaySize.cx)>>1))&0xfffc;
	if(DestDisplaySize.cy>DisplaySize.cy)	
		pt.y=(((DestDisplaySize.cy-DisplaySize.cy)>>1))&0xfffc;
	#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
			VIM_USER_PrintDec("[VC0578][HAPI]a mem offset x:  ",pt.x);
			VIM_USER_PrintDec("[VC0578][HAPI]a mem offset y:  ",pt.y);
	#endif
	
	result =VIM_DISP_SetA_Memory(pt,DestDisplaySize);
	if(result)
	goto DECODEERROR;
	
	pt.x=(gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	pt.y=(gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;

	if(DestDisplaySize.cx<=DisplaySize.cx)	
		pt.x=(((DisplaySize.cx-DestDisplaySize.cx)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.x)&0xfffc;
	else
		DestDisplaySize.cx=DisplaySize.cx;
	if(DestDisplaySize.cy<=DisplaySize.cy)	
		pt.y=(((DisplaySize.cy-DestDisplaySize.cy)>>1)+gVc0578b_Info.DisplayStatus.LcdWantPt.y)&0xfffc;
	else
		DestDisplaySize.cy=DisplaySize.cy;

#if(VIM_HIGH_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578][HAPI]a display offset x:  ",pt.x);
	VIM_USER_PrintDec("[VC0578][HAPI] a display offset y:  ",pt.y);
	VIM_USER_PrintDec("[VC0578][HAPI] new DisplaySize width:  ",DestDisplaySize.cx);
	VIM_USER_PrintDec("[VC0578][HAPI] new DisplaySize height:  ",DestDisplaySize.cy);
#endif	

	VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);

	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
					VIM_DISP_NOTCHANGE);
	if(result)
			goto DECODEERROR;	
	gVc0578b_Info.MarbStatus.ALayerMode = VIM_USER_PREVIEW_ALAYER_MODE;

	result = VIM_MAPI_SetDisplayJpegParameter();
	if(result)
		goto DECODEERROR;
	
	VIM_MAPI_InitDispInterrupt();
	VIM_MARB_ResetSubModule(VIM_MARB_SUB_RESET_JPEG);
	VIM_MARB_SetJpegbufGapratio(5,VIM_MARB_FIFO_0);
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
	//set jpeg each table from jpeg file

	VIM_JPEG_StartDecode();	

DECODEERROR:
	if(result)
	{
		return (UINT16)result;
	}
	return VIM_SUCCEED;
}

#endif

#endif

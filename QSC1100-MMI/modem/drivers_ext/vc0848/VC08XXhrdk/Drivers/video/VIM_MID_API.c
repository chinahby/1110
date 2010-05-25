/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_MID_Driver.c
 * [Description]        : 适用于578b.
 *				     这个模块的目的是为了high层做准备函数，拼凑一些底层
 *                       	     函数。另外还申请了几个全局变量
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-26 16:01
 * [Platform]               : 平台无关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-26  		angela      		 Created
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
#include "AEESTDLIB.h"


#define QUALITY_EXTERNAL_MIN 1
#define QUALITY_EXTERNAL_MAX 100
#define QUALITY_NATIVE_MAX   30 //modify the min quality value of camera is 30 2009.02.10

extern UINT8 osd_6060[10800];


VIM_STATUS gVc0578b_Info;			//578 all work mode information define
VIM_JPEG_Index gVc0578b_JpegInfo;	//jepg information define
VIM_HIF_TISR  gVc0578b_Isr;//VIM_HIF_ISR;

//revised by youhai_mp4enc 2007.9.17	
#if (BOARD_TYPE==VC0820_FPGA)
extern  UINT32 gSensorNum;
#endif



/********************************************************************************
  Description:
  	init context
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_MAPI_InitIfo(void)
{
	VIM_USER_MemSet(( UINT8 *)&gVc0578b_Info,0,sizeof(gVc0578b_Info));
	gVc0578b_Info.pDriverIcInfo=(PTPanelInfo)&gDriverIcInfo;
	gVc0578b_Info.pSensorInfo=(PTSnrInfo)&gPSensorInfo[0];
	//gVc0578b_Info.LcdStatus.Size=gVc0578b_Info.pUserInfo->MainPanel.Size;
	gVc0578b_Info.SubPanel.Point.x=0;
	gVc0578b_Info.SubPanel.Point.y=0;
	gVc0578b_Info.SubPanel.Size.cx=MMD_LCD_WIDTH;
	gVc0578b_Info.SubPanel.Size.cy=MMD_LCD_HEIGHT;
	gVc0578b_Info.MainPanel.Point.x=0;
	gVc0578b_Info.MainPanel.Point.y=0;
	gVc0578b_Info.MainPanel.Size.cx=MMD_LCD_WIDTH;
	gVc0578b_Info.MainPanel.Size.cy=MMD_LCD_HEIGHT;
	gVc0578b_Info.LcdStatus.Size.cx=gVc0578b_Info.MainPanel.Size.cx;
	gVc0578b_Info.LcdStatus.Size.cy=gVc0578b_Info.MainPanel.Size.cy;
	gVc0578b_Info.LcdStatus.NowPanel=VIM_HAPI_LCDPANEL_MAIN;
	gVc0578b_Info.MarbStatus.ALayerMode=VIM_DISP_LINEBUF;
	gVc0578b_Info.MarbStatus.MapList.jbufnowblock_int=5;
	gVc0578b_Info.MarbStatus.WorkMode=VIM_MARB_BYPASS_MODE;
	gVc0578b_Info.CaptureStatus.QualityMode=VIM_HAPI_CPTURE_QUALITY_HIGH;
	gVc0578b_Info.VideoStatus.CapMode=VIM_VIDEO_STOP;
	gVc0578b_Info.VideoStatus.DisMode=VIM_VIDEO_STOP;
	gVc0578b_Info.CaptureStatus.ThumbSize.cx=0;
	gVc0578b_Info.CaptureStatus.ThumbSize.cy=0;
	gVc0578b_Info.CaptureStatus.Size.cx=640;
	gVc0578b_Info.CaptureStatus.Size.cy=480;
	gVc0578b_Info.PreviewStatus.Size.cx=128;
	gVc0578b_Info.PreviewStatus.Size.cy=160;
	gVc0578b_Info.LcdStatus.ColorDep=VIM_HAPI_COLORDEP_16BIT;
	gVc0578b_Info.aeaf.Baeafen=0;
	gVc0578b_Info.aeaf.Baeafsel=1;//进入ae 状态
	gVc0578b_Info.ChipWorkMode=VIM_HAPI_MODE_CAMERAON;
	gVc0578b_Info.aeaf.rightstep = VIM_SENSOR_AF_MAXSTEP/4;
	gVc0578b_Info.CaptureStatus.SensorClock=24;
}
/********************************************************************************
* Description:
*	Auto to find sensor from the list
*	
* Parameters:
*	void
* Return:
*	VIM_SUCCEED: 		find the fit sensor
*	VIM_ERRO_SENSOR_AUTOFIND: not find sensor.
**********************************************************************************/
VIM_RESULT VIM_MAPI_AutoFindSensor(void)
{
       VIM_RESULT Result;
	UINT8 i = 0;
	
//revised by youhai_mp4enc 2007.9.17	
#if (BOARD_TYPE==VC0820_FPGA)
	for(i=0; i<gSensorNum; i++)
#else
	for(i=0; i<100; i++)
#endif		
	{
		if(gPSensorInfo[i]==NULL)
		{
			gVc0578b_Info.pSensorInfo = gPSensorInfo[i];
			return VIM_ERROR_SENSOR_AUTOFIND; //sensor error;
		}
		VIM_HIF_ResetSubModule(VIM_HIF_RESET_SIF);				//Reset the SIF module by software 
		gVc0578b_Info.pSensorInfo = gPSensorInfo[i];
		Result=VIM_SIF_SifInitI2c(gVc0578b_Info.pSensorInfo,
#if (BOARD_TYPE==FPGA)
			6000);
#else
			VIM_HIF_GetMclock()); // Init i2c
#endif
			
		//reset sensor here
		Result=VIM_SIF_SetSensorPowerOn(gVc0578b_Info.pSensorInfo);//having reset sensor
 		if(gVc0578b_Info.pSensorInfo->snrIdChkCall!= NULL)
		{

			VIM_USER_DelayMs(5); //zhangxiang mod  09/01/14  100
         //VIM_USER_DelayMs(100);   
		 	if(SensorIdChk_OV2650_YUV()/*gVc0578b_Info.pSensorInfo->snrIdChkCall()*/ == SUCCEED)
		 	{
		 		VIM_USER_DelayMs(5); //zhangxiang mod  09/01/14  100
		 		//VIM_USER_DelayMs(100);
		 		Result = VIM_SIF_SifSensorInit(gVc0578b_Info.pSensorInfo,&gVc0578b_Info.VIM_SIF_AeAdjust);
		 		if (Result) 
		 		   return	Result;
		 		return VIM_SUCCEED;
		 	}
		}
	}

	return VIM_ERROR_SENSOR_AUTOFIND; //sensor error;
}
/********************************************************************************
  Description:
	Switch panel 
 Parameters:
	switch cs and point and info
  Returns:
  	void
  Remarks:
*********************************************************************************/

void VIM_MAPI_SwitchPanel(VIM_HAPI_LCDPANEL bPanel)
{
TPoint point={0,0};
TSize size={0,0};
	VIM_HIF_SetLcdDataBus(V5_BYPASS_DATABUS);
	if(bPanel==VIM_HAPI_LCDPANEL_SUB)	//sub
	{
		VIM_LCDIF_SetBiasSpt(gVc0578b_Info.SubPanel.Point);
		gVc0578b_Info.LcdStatus.Size=gVc0578b_Info.SubPanel.Size;	//x=96,y=64
		gVc0578b_Info.LcdStatus.NowPanel=bPanel;		//==1
		VIM_LCDIF_InitLcdIFParm(gVc0578b_Info.pDriverIcInfo->slave);
		VIM_DISP_SetB0_DisplayPanel(point,size);
		VIM_DISP_SetB1_DisplayPanel(point,size);
	}
	else
	{
		VIM_LCDIF_SetBiasSpt(gVc0578b_Info.MainPanel.Point);
		gVc0578b_Info.LcdStatus.Size=gVc0578b_Info.MainPanel.Size;	//x=240,y=320
		gVc0578b_Info.LcdStatus.NowPanel=bPanel;		//==0;
		VIM_LCDIF_InitLcdIFParm(gVc0578b_Info.pDriverIcInfo->master);
		VIM_DISP_SetB0_DisplayPanel(point,size);
		VIM_DISP_SetB1_DisplayPanel(point,size);
	}
}

/********************************************************************************

	Description:
		Test the read and write register 

	Parameters:
		Return:
		return 0:The function succeed
			Return 1:  read and write register wrong 
			Return 2:  read and write register wrong (MCLK domain)
			Return 3:  read and write Sram wrong (MCLK domain)

Note：
	This function can be used in Camera open mode ;
						or Photo process mode; 
						or direct display mode ;
Remarks:
		state: valid

*********************************************************************************/
#define TESTSRAMNUM 48
VIM_RESULT VIM_MAPI_TestRegister (void)
{
	UINT8 Test8,TestSram[TESTSRAMNUM],Sram[TESTSRAMNUM];
	UINT16 Test16[2],i;
	UINT32 Test32;
	UINT32 addr;

	//测试8位寄存器读写
#if (BOARD_TYPE==VC0820_FPGA)
    //register  accesse test, GPIO_A_PE = 0x6017_0014
	addr = 0x60170014;
#else
	addr = V5_REG_CPM_ABI_TEST;
#endif	
	Test8=0x55;
	VIM_HIF_SetReg8(addr,Test8);
	Test8=0xAA;
	VIM_HIF_SetReg8(addr+1,Test8);
	Test8=0x11;
	VIM_HIF_SetReg8(addr+2,Test8);
	Test8=0xbb;
	VIM_HIF_SetReg8(addr+3,Test8);
	Test32=VIM_HIF_GetReg32(addr);

	if(Test32!=0xbb11aa55)
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
			VIM_USER_PrintString("[VC0578B][MID] VIM_ERROR_HIF_READWRITE_REG8 [ERROR]  ");
#endif
		return VIM_ERROR_HIF_READWRITE_REG8;
	}
	
	//测试bit endian
	Test8=VIM_USER_CheckBigendian();
	if(VIM_ENDIANTYPE!=Test8)
		return VIM_ERROR_DEFINE_ENDIAN;
	
	//测试16位寄存器读写
	Test16[0]=0x1155;
	Test16[1]=0xaabb;
	VIM_HIF_SetReg16(addr,Test16[0]);
	VIM_HIF_SetReg16(addr+2,Test16[1]);
	
	Test32=VIM_HIF_GetReg32(addr);
	if(Test32!=0xaabb1155)
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
			VIM_USER_PrintString("[VC0578B][MID] VIM_ERROR_HIF_READWRITE_REG16 [ERROR]  ");
#endif
		return VIM_ERROR_HIF_READWRITE_REG16;
	}

	//测试32位寄存器读写
	VIM_HIF_SetReg32(addr,0x1155aabb);
	Test32=VIM_HIF_GetReg32(addr);
	if(Test32!=0x1155aabb)
		return VIM_ERROR_HIF_READWRITE_REG32;
	
	//测试ipp模块读写
	Test8=VIM_HIF_GetReg8(V5_REG_IPP_IMGSIZE);
	Test8=120;
	VIM_HIF_SetReg8(V5_REG_IPP_IMGSIZE,Test8);
	Test8=VIM_HIF_GetReg8(V5_REG_IPP_IMGSIZE);
	if(Test8!=120)
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
			VIM_USER_PrintString("[VC0578B][MID] VIM_ERROR_IPP_READWRITE_REG8 [ERROR]  ");
#endif
		return VIM_ERROR_IPP_READWRITE_REG8;
	}


	//测试SIF模块读写

	Test8=VIM_HIF_GetReg16(V5_REG_SIF_I2C_CLKCNT);
	Test8=32;
	VIM_HIF_SetReg16(V5_REG_SIF_I2C_CLKCNT,Test8);
	Test8=VIM_HIF_GetReg16(V5_REG_SIF_I2C_CLKCNT);
	if(Test8!=32)
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
			VIM_USER_PrintString("[VC0578B][MID] VIM_ERROR_SIF_READWRITE_REG8 [ERROR]  ");
#endif
		return VIM_ERROR_IPP_READWRITE_REG8;
	}

#if (BOARD_TYPE==VC0820_FPGA)
    //sram accesse test
	addr = (UINT32)Sram;
#else
	addr = 0;
#endif	
	
	//测试sram读写
	for(i=0;i<TESTSRAMNUM;i++)
		TestSram[i]=i;
	VIM_HIF_WriteSram(addr, TestSram, TESTSRAMNUM,VIM_HIF_SRAM_MODE);
	VIM_USER_MemSet(TestSram,0,TESTSRAMNUM);
	VIM_HIF_ReadSram(addr, TestSram, TESTSRAMNUM,VIM_HIF_SRAM_MODE);
	for(i=0;i<TESTSRAMNUM;i++)
	{
		if(TestSram[i]!=i)
		{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
			VIM_USER_PrintString("[VC0578B][MID] VIM_ERROR_READWRITE_SRAM [ERROR]  ");
#endif
			return VIM_ERROR_READWRITE_SRAM;
		}
	}
#if(BOARD_TYPE==VC0820_FPGA)
	//测试fifo读写, 在ARM9使用实际地址访问
	
#else
	//测试fifo读写
	VIM_MARB_SetFifoChannel(VIM_MARB_PRI_BIU,VIM_MARB_PRI_NULL,VIM_MARB_FIFO_0);
	VIM_HIF_SetReg32(V5_REG_MARB_VFIFO_STARTADDR_0,V5_1TSRAM_BASE+0x100);
	VIM_HIF_SetReg32(V5_REG_MARB_VFIFO_LENGTH_0,TESTSRAMNUM);
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
	for(i=0;i<TESTSRAMNUM;i++)
		TestSram[i]=i;

	VIM_HIF_WriteSram(0x100, TestSram, TESTSRAMNUM+4,VIM_HIF_FIFO_MODE);

	VIM_USER_MemSet(TestSram,0,TESTSRAMNUM);
	
	VIM_MARB_SetFifoChannel(VIM_MARB_PRI_NULL,VIM_MARB_PRI_BIU,VIM_MARB_FIFO_0);
	VIM_HIF_ReadSram(0x100, TestSram, TESTSRAMNUM,VIM_HIF_FIFO_MODE);
	
	for(i=0;i<TESTSRAMNUM;i++)
	{
		if(TestSram[i]!=i)
		{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
			VIM_USER_PrintString("[VC0578B][MID] VIM_ERROR_READWRITE_FIFO [ERROR]  ");
#endif

			//return VIM_ERROR_READWRITE_FIFO;
		}
	}
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);

#endif
	//测试sdram读写
#if VIM_USER_SUPPORT_SDRAM

	for(i=0;i<TESTSRAMNUM;i++)
		TestSram[i]=i;
	VIM_HIF_WriteSdram(0, TestSram, TESTSRAMNUM,VIM_HIF_SRAM_MODE);
	VIM_USER_MemSet(TestSram,0,TESTSRAMNUM);
	VIM_HIF_ReadSdram(0, TestSram, TESTSRAMNUM,VIM_HIF_SRAM_MODE);
	for(i=0;i<TESTSRAMNUM;i++)
	{
		if(TestSram[i]!=i)
		{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
			VIM_USER_PrintString("[VC0578B][MID] VIM_ERROR_READWRITE_SDRAM [ERROR]  ");
#endif
			return VIM_ERROR_READWRITE_SDRAM;
		}
	}	
#endif

	return VIM_SUCCEED;
}

/********************************************************************************
  Description:
  	init control
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/
VIM_RESULT VIM_MAPI_InitHif(void)
{
	return VIM_SUCCEED;
}



/********************************************************************************
  Description:
  	set chip work mode
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_MAPI_SetChipMode(VIM_MARB_WORKMODE bMode,VIM_IPP_HAVEFRAM bHaveFrame)
{
	//if(gVc0578b_Info.MarbStatus.WorkMode==bMode)
	//	return ;
#if(BOARD_TYPE==VC0820_FPGA) //add by zhoujian
	UINT8 bLineMode=0;
#endif
	gVc0578b_Info.MarbStatus.WorkMode=bMode;
	VIM_MARB_SetMode(bMode);
	
#if(BOARD_TYPE==VC0820_FPGA) //add by zhoujian
	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x01);
	VIM_USER_DelayMs(1);
	VIM_HIF_SetReg8(V5_REG_LBUF_CNT_RESET,0x0);
#endif


	switch(bMode)
	{
		case VIM_MARB_PREVIEW_MODE: 
			VIM_IPP_SetMode(VIM_IPP_MODE_CAPTURE,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_PREVIEW);
			break;
		case VIM_MARB_MP4_PREVIEW_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_PREVIEW,bHaveFrame);
			break;
		case VIM_MARB_CAPTURESTILL_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_CAPTURE,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_CAPTURE_MJPEG);
			break;
		case VIM_MARB_CAPTUREMPG4_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_ENCODE_MP4,bHaveFrame);
//revised by youhai_2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
#else			
			VIM_JPEG_SetMode(VIM_JPEG_MODE_CAPTURE_MPG4);
#endif
			VIM_MPEG4_SetMode(VIM_MPEG4_ENCODE);
			break;
		case VIM_MARB_DISPLAYMPG4_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_DECODE_MP4,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_DISPLAY_MPG4);
			VIM_MPEG4_SetMode(VIM_MPEG4_DECODE);
			break;
#if(BOARD_TYPE==VC0820_FPGA) //add by zhoujian
		case VIM_MARB_DISPLAYDEBUG_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_DECODE_MP4,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_DISPLAY_MPG4);
			VIM_MPEG4_SetMode(VIM_MPEG4_DECODE_LBUF);
			break;
#endif
		case VIM_MARB_CAPTURESTILLTHUMB_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_CAPTURE_WITHTHUMB,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_CAPTURE_MJPEG);
			break;

		case VIM_MARB_ENCODE_MODE:
#if(BOARD_TYPE==VC0820_FPGA) //add by zhoujian
#else
			VIM_IPP_SetMode(VIM_IPP_MODE_CAPTURE,bHaveFrame);
#endif
			VIM_JPEG_SetMode(VIM_JPEG_MODE_ENCODE);
			break;
		case VIM_MARB_DISPLAYSTILL_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_DISPLAY,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_DISPLAY_JPEG);
			break;
		case VIM_MARB_DISPLAYSTILLBYPASS_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_DISPLAY,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_DISPLAY_YUV);
			break;
#if(BOARD_TYPE==VC0820_FPGA) //add by zhoujian
		case VIM_MARB_DECODEONLY_MODE:
			VIM_JPEG_SetMode(VIM_JPEG_MODE_DECODE_JPEGTOYUV);
			bLineMode|=(VIM_IPP_MODE_DECODE);
			VIM_HIF_SetReg8(V5_REG_LBUF_MODE+1,bLineMode);
			break;
#endif
		case VIM_MARB_DECODEYUV_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_DECODE,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_DECODE_JPEGTOYUV);
			break;
		case VIM_MARB_DECODERGB_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_DECODE,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_DECODE_JPEGTORGB);
			break;
#if(BOARD_TYPE==VC0820_FPGA) //add by zhoujian
		case VIM_MARB_DECODERGBBYPASS_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_DECODE,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_DECODE_YUV);
			break;
#endif
		case VIM_MARB_VIDEOCONFERENCE_MODE:
			VIM_IPP_SetMode(VIM_IPP_MODE_VIDEO_MP4,bHaveFrame);
			VIM_JPEG_SetMode(VIM_JPEG_MODE_VIDEO_MPG4);
			VIM_MPEG4_SetMode(VIM_MPEG4_VIDEOCON);
			
			break;
		default:
			break;
	}
	
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
		this funcion should be called after caculte correct VWC
	Remarks:
		state: valid
*********************************************************************************/
VIM_RESULT VIM_MAPI_SetCaptureRatio(UINT8 ratio)
{
  UINT32 dwVwc;
	 if((ratio < QUALITY_EXTERNAL_MIN)||(ratio > QUALITY_EXTERNAL_MAX))
		return VIM_ERROR_PARAMETER;
    
	 // convert the ratio from QC 1~100 to VC0848 10~30
	 ratio = (ratio * QUALITY_NATIVE_MAX / QUALITY_EXTERNAL_MAX);
	 if(ratio <= 15) 
        ratio = 15;   //zhangxiang modify 20090205
     FARF(MSG,("VIM_MAPI_SetCaptureRatio=%d",ratio));
	 dwVwc=VIM_JPEG_GetVideoWordCount();
	 dwVwc=dwVwc/ratio;
	 VIM_JPEG_SetTargetWordCount(dwVwc);
        VIM_JPEG_SetTargetCompressRatio(ratio);
        VIM_JPEG_SetBitRateControlEn(ENABLE);
	 return VIM_SUCCEED;		 
}

#if (BOARD_TYPE==VC0820_FPGA)//add by zhoujian

/********************************************************************************
Description:
	jpeg decode done callback 
Parameters:
	Note:
		none
	Remarks:
		state: valid
*********************************************************************************/
void VIM_MAPI_ISR_JpegDecDone(void)
{
	UINT32 fifocount;

	if(gVc0578b_Info.DisplayStatus.JpegCallBack)
	{
		fifocount=VIM_MARB_GetFifoSize(VIM_MARB_FIFO_0);
		if(gVc0578b_Info.DisplayStatus.JpegCallBack(VIM_HAPI_DECODE_END,fifocount))
		{
			VIM_USER_PrintString("Decode  end \n");
		}
	}
	else
	{
		VIM_USER_PrintString("no jpeg decode done callback\n");
	}
	
}

/********************************************************************************
Description:
	jpeg threshlow callback 
Parameters:
	Note:
		none
	Remarks:
		state: valid
*********************************************************************************/
void VIM_MAPI_ISR_ThreshLow(void)
{
	UINT32 fifosize;
	
	if(gVc0578b_Info.DisplayStatus.JpegCallBack)
	{
		fifosize=VIM_MARB_GetJbufRWSize();
		gVc0578b_Info.DisplayStatus.JpegCallBack(VIM_HAPI_FIFO_THRLOW,fifosize);
	}
	else
	{
		VIM_USER_PrintString("no jpeg threshlow callback\n");
	}
	
	//VIM_USER_PrintString("vfifo_0_thresh_low\n");	
}
/********************************************************************************
Description:
	marb fifo emptycallback 
Parameters:
	Note:
		none
	Remarks:
		state: valid
*********************************************************************************/
void VIM_MAPI_ISR_FIFOEmpty(void)
{
	UINT32 fifosize;
	
	if(gVc0578b_Info.DisplayStatus.JpegCallBack)
	{
		fifosize=VIM_MARB_GetJbufRWSize();
		gVc0578b_Info.DisplayStatus.JpegCallBack(VIM_HAPI_FIFO_EMPTY,fifosize);
	}
	else
	{
		VIM_USER_PrintString("no jpeg empty callback\n");
	}
	
	//VIM_USER_PrintString("vfifo_0_thresh_low\n");	
}

void VIM_MAPI_ISR_LbufRead(void)
{
	UINT32 *lbuf0AddrY,*lbuf0AddrU,*lbuf0AddrV;
	UINT32 *lbuf1Addr;
	UINT32 *decbuf;
	UINT32 image_width,image_height;
	UINT32 yuv_mode,decode_mode,write_mode;
	UINT32 Y1,Y2,U,V,data,i,j,m,uint;
	UINT32 buflen,bufpoint=0;
	TSize image_size;

	lbuf0AddrY=(UINT32 *)VIM_HIF_GetReg32(V5_REG_LBUF_Y_INI0_DEC);
	lbuf0AddrU=(UINT32 *)VIM_HIF_GetReg32(V5_REG_LBUF_CB_INI0_DEC);
	lbuf0AddrV=(UINT32 *)VIM_HIF_GetReg32(V5_REG_LBUF_CR_INI0_DEC);

	yuv_mode=gVc0578b_Info.DecodeStatus.YUVmode;
	decode_mode=gVc0578b_Info.DecodeStatus.DecodeMode;
	write_mode=gVc0578b_Info.DecodeStatus.WriteBlock;
	decbuf=(UINT32 *)gVc0578b_Info.DecodeStatus.DecodePoint;
	VIM_USER_PrintString("lbuf_cpm_read_int\n");
	
	switch(decode_mode)
	{
		case VIM_DECODE_ONLY:
			if(write_mode==1)
			{
				image_width =gVc0578b_Info.DecodeStatus.JpegSize.cx;
				
				if(yuv_mode == JPEG_422 || yuv_mode == JPEG_444)//422
				{
					uint=image_width/16;
					buflen=image_width*2*8;
					if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
					{
						VIM_USER_PrintString("decode buffer overflow!\n");
						return;
					}
					for(i=0;i<uint;i++)
					{
						for(j=0;j<8;j++)	//y0 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*4+j*image_width/4+m];
							}
						for(j=0;j<8;j++)	//y1 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*4+2+j*image_width/4+m];
							}
						for(j=0;j<8;j++)	//u block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrU[i*2+j*image_width/8+m];
							}
						for(j=0;j<8;j++)	//v block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrV[i*2+j*image_width/8+m];
							}
					}
					gVc0578b_Info.DecodeStatus.DecodeLine+=8;
				}
				else if(yuv_mode == JPEG_420 )//420
				{
					uint=image_width/16;
					buflen=image_width*(16+8);
					if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
					{
						VIM_USER_PrintString("decode buffer overflow!\n");
						return;
					}
					for(i=0;i<uint;i++)
					{
						for(j=0;j<8;j++)	//y0 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*4+j*image_width/4+m];
							}
						for(j=0;j<8;j++)	//y1 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*4+2+j*image_width/4+m];
							}
						for(j=8;j<16;j++)	//y2 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*4+j*image_width/4+m];
							}
						for(j=8;j<16;j++)	//y3 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*4+2+j*image_width/4+m];
							}
						for(j=0;j<8;j++)	//u block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrU[i*2+j*image_width/8+m];
							}
						for(j=0;j<8;j++)	//v block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrV[i*2+j*image_width/8+m];
							}
					}
					gVc0578b_Info.DecodeStatus.DecodeLine+=16;
				}
				else if(yuv_mode == JPEG_411)//411
				{
					uint=image_width/32;
					buflen=image_width*(8+4);
					if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
					{
						VIM_USER_PrintString("decode buffer overflow!\n");
						return;
					}
					for(i=0;i<uint;i++)
					{
						for(j=0;j<8;j++)	//y0 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*8+j*image_width/4+m];
							}
						for(j=0;j<8;j++)	//y1 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*8+2+j*image_width/4+m];
							}
						for(j=0;j<8;j++)	//y2 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*8+4+j*image_width/4+m];
							}
						for(j=0;j<8;j++)	//y3 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*8+6+j*image_width/4+m];
							}
						for(j=0;j<8;j++)	//u block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrU[i*2+j*image_width/16+m];
							}
						for(j=0;j<8;j++)	//v block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrV[i*2+j*image_width/16+m];
							}
					}
					gVc0578b_Info.DecodeStatus.DecodeLine+=8;
				}
				else if(yuv_mode == JPEG_400)//400
				{
					uint=image_width/8;
					buflen=image_width*8;
					if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
					{
						VIM_USER_PrintString("decode buffer overflow!\n");
						return;
					}
					for(i=0;i<uint;i++)
					{
						for(j=0;j<8;j++)	//y0 block
							for(m=0;m<2;m++)
							{
								*(decbuf + bufpoint++)=lbuf0AddrY[i*2+j*image_width/4+m];
							}
					}
					gVc0578b_Info.DecodeStatus.DecodeLine+=8;
				}
			}
			else
			{
				image_width =gVc0578b_Info.DecodeStatus.JpegSize.cx;
				image_height = image_width;
				if(yuv_mode == JPEG_422 || yuv_mode == JPEG_444)//422
				{
					buflen=image_width*2*8;
					if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
					{
						VIM_USER_PrintString("decode buffer overflow!\n");
						return;
					}
#if 0
					for(i=0; i<8; i++)
					{
						image_width=image_height;
						for(;image_width>0;image_width-=8)
						{
							Y1=*(lbuf0AddrY++);
							Y2=*(lbuf0AddrY++);
							U=*(lbuf0AddrU++);
							V=*(lbuf0AddrV++);
							data = (U&0xff) | ((Y1&0xff00)) | ((V&0xff)<<16) | ((Y1&0xff)<<24);
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff00)>>8) | ((Y1&0xff000000)>>16) | ((V&0xff00)<<8) |((Y1&0xff0000)<<8);
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff0000)>>16) | ((Y2&0xff00)) | (V&0xff0000) |((Y2&0xff)<<24) ;
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff000000)>>24) | ((Y2&0xff000000)>>16) | ((V&0xff000000)>>8) |((Y2&0xff0000)<<8);
							*(decbuf + bufpoint++) = data;
						}
					}
#else
					for(i=0; i<8; i++)
					{
						image_width=image_height;
						for(;image_width>0;image_width-=8)
						{
							Y1=*(lbuf0AddrY++);
							Y2=*(lbuf0AddrY++);
							U=*(lbuf0AddrU++);
							V=*(lbuf0AddrV++);
							data = (U&0xff) | ((Y1&0xff)<<8) | ((V&0xff)<<16) | ((Y1&0xff00)<<16);
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff00)>>8) | ((Y1&0xff0000)>>8) | ((V&0xff00)<<8) |(Y1&0xff000000);
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff0000)>>16) | ((Y2&0xff)<<8) | (V&0xff0000) |((Y2&0xff00)<<16) ;
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff000000)>>24) | ((Y2&0xff0000)>>8) | ((V&0xff000000)>>8) |((Y2&0xff000000));
							*(decbuf + bufpoint++) = data;
						}
					}

#endif
					gVc0578b_Info.DecodeStatus.DecodeLine+=8;
				}
				else if(yuv_mode == JPEG_420 )//420
				{
					buflen=image_width*16*2;
					if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
					{
						VIM_USER_PrintString("decode buffer overflow!\n");
						return;
					}
					for(i=3; i<19; i++)
					{
						image_width=image_height;
						if(i%2 == 0)
						{
							lbuf0AddrU-=image_width/8;
							lbuf0AddrV-=image_width/8;
						}	
						for(;image_width>0;image_width-=8)
						{
							Y1=*(lbuf0AddrY++);
							Y2=*(lbuf0AddrY++);
							U=*(lbuf0AddrU++);
							V=*(lbuf0AddrV++);
							data = (U&0xff) | ((Y1&0xff)<<8) | ((V&0xff)<<16) | ((Y1&0xff00)<<16);
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff00)>>8) | ((Y1&0xff0000)>>8) | ((V&0xff00)<<8) |(Y1&0xff000000);
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff0000)>>16) | ((Y2&0xff)<<8) | (V&0xff0000) |((Y2&0xff00)<<16) ;
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff000000)>>24) | ((Y2&0xff0000)>>8) | ((V&0xff000000)>>8) |((Y2&0xff000000));
							*(decbuf + bufpoint++) = data;
						}
					}
					gVc0578b_Info.DecodeStatus.DecodeLine+=16;
				}
				else if(yuv_mode == JPEG_411)//411
				{
					buflen=image_width*8*2;
					if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
					{
						VIM_USER_PrintString("decode buffer overflow!\n");
						return;
					}
					for(i=0; i<16; i++)
					{
						image_width=image_height;
						for(;image_width>0;image_width-=16)
						{
							Y1=*(lbuf0AddrY++);
							Y2=*(lbuf0AddrY++);
							U=*(lbuf0AddrU++);
							V=*(lbuf0AddrV++);
							data = (U&0xff) | ((Y1&0xff)<<8) | ((V&0xff)<<16) | ((Y1&0xff00)<<16);
							*(decbuf + bufpoint++) = data;
							data = (U&0xff) | ((Y1&0xff0000)>>8) | ((V&0xff)<<16) |(Y1&0xff000000);
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff00)>>8) | ((Y2&0xff)<<8) | (V&0xff00) |((Y2&0xff00)<<16) ;
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff00)>>8) | ((Y2&0xff0000)>>8) | (V&0xff00)|((Y2&0xff000000));
							*(decbuf + bufpoint++) = data;
							Y1=*(lbuf0AddrY++);
							Y2=*(lbuf0AddrY++);
							data = ((U&0xff0000)>>16) | ((Y1&0xff)<<8) | (V&0xff0000) | ((Y1&0xff00)<<16);
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff0000)>>16) | ((Y1&0xff0000)>>8) | (V&0xff0000) |(Y1&0xff000000);
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff000000)>>24) | ((Y2&0xff)<<8) | ((V&&0xff000000)>>8) |((Y2&0xff00)<<16) ;
							*(decbuf + bufpoint++) = data;
							data = ((U&0xff000000)>>24) | ((Y2&0xff0000)>>8) | ((V&0xff000000)>>8) |((Y2&0xff000000));
							*(decbuf + bufpoint++) = data;
						}
					}				
					gVc0578b_Info.DecodeStatus.DecodeLine+=16;
				}
				else if(yuv_mode == JPEG_400)//400
				{
					buflen=image_width*8;
					if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
					{
						VIM_USER_PrintString("decode buffer overflow!\n");
						return;
					}
					for(i=0; i<8; i++)
					{
						image_width=image_height;
						for(;image_width>0;image_width-=8)
						{
							*(decbuf + bufpoint++) = *(lbuf0AddrY++);
							*(decbuf + bufpoint++) = *(lbuf0AddrY++);
						}
					}
					gVc0578b_Info.DecodeStatus.DecodeLine+=8;
				}
			}
			gVc0578b_Info.DecodeStatus.DecodePoint+=buflen;
			gVc0578b_Info.DecodeStatus.DecodeBufLength-=buflen;
			gVc0578b_Info.DecodeStatus.DecFileLength+=buflen;
			VIM_MAPI_DecodelbufEn(gVc0578b_Info.MarbStatus.WorkMode);
			if(gVc0578b_Info.DecodeStatus.DecodeLine>=gVc0578b_Info.DecodeStatus.JpegSize.cy)
			{
				gVc0578b_Info.DecodeStatus.DecCallback(VIM_HAPI_LBUF_END,gVc0578b_Info.DecodeStatus.DecFileLength);
				gVc0578b_Info.DecodeStatus.DecFileLength=0;
				gVc0578b_Info.DecodeStatus.DecodeLine=0;
				return;
			}
	
			break;	
		case VIM_DECODE_IPP:
		case VIM_DECODE_FRAME:
		case VIM_DECODE_BYPASS_IPP:
		case VIM_DECODE_BYPASS_FRAME:
			lbuf1Addr = (UINT32 *)VIM_HIF_GetReg32(V5_REG_LBUF_Y_INI1_DEC);
			VIM_IPP_GetDispSize(&image_size);
			image_width = image_size.cx;
			image_height=VIM_HIF_GetReg8(V5_REG_LBUF_STATUS+1);
			image_height&=0xff;
			VIM_USER_PrintDec("get lines is",image_height);
			buflen=image_width*2*image_height;
			if(buflen>gVc0578b_Info.DecodeStatus.DecodeBufLength)
			{
				VIM_USER_PrintString("decode buffer overflow!\n");
				return;
			}
			get_video_data((UINT8 *)decbuf,(UINT8 *)lbuf1Addr, buflen);
			gVc0578b_Info.DecodeStatus.DecodePoint+=buflen;
			gVc0578b_Info.DecodeStatus.DecodeBufLength-=buflen;
			gVc0578b_Info.DecodeStatus.DecFileLength+=buflen;
			gVc0578b_Info.DecodeStatus.DecodeLine+=image_height;
			VIM_MAPI_DecodelbufEn(gVc0578b_Info.MarbStatus.WorkMode);
			if(gVc0578b_Info.DecodeStatus.DecodeLine>= image_size.cy)
			{
				gVc0578b_Info.DecodeStatus.DecCallback(VIM_HAPI_LBUF_END,gVc0578b_Info.DecodeStatus.DecFileLength);
				gVc0578b_Info.DecodeStatus.DecFileLength=0;
				gVc0578b_Info.DecodeStatus.DecodeLine=0;
				return;
			}
			break;
		default:
			break;
	}
	VIM_JPEG_RestartDecode();
	return;
}
/********************************************************************************
Description:
	init interrupt of display function
Parameters:
	Note:
		this funcion should be called in the capture function high api
	Remarks:
		state: valid 
*********************************************************************************/
void VIM_MAPI_InitDispInterrupt(void)
{
	
		//clear interrupt
		//VIM_INT_Acknowledge(VM_HIF_INT_MARB);
		//VIM_INT_Acknowledge(VM_HIF_INT_JPEG);
		
		VIM_MARB_RegisterISR(MARB_INT_ALL,NULL);
		VIM_JPEG_RegisterISR(VIM_JPEG_JPEG_INT_ALL,NULL);
		
		//enalbe jbuf error interrrupt
		/*VIM_MARB_RegisterISR(MARB_INT_FIFO0_FULL,VIM_MAPI_ISR_JbufError);
		VIM_MARB_RegisterISR(MARB_INT_FIFO1_FULL,VIM_MAPI_ISR_JbufError);*/
		//VIM_MARB_RegisterISR(MARB_INT_ADDERROR,VIM_MAPI_ISR_DispJbufError);
		/*VIM_MARB_SetIntEnable(MARB_INT_FIFO0_FULL,ENABLE);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO1_FULL,ENABLE);*/
		//VIM_MARB_SetIntEnable(MARB_INT_ADDERROR,ENABLE);	
	
		VIM_MARB_RegisterISR(MARB_INT_FIFO0_THRESH_LOW, VIM_MAPI_ISR_ThreshLow);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_THRESH_LOW,ENABLE);
		VIM_MARB_RegisterISR(MARB_INT_FIFO0_EMPTY,VIM_MAPI_ISR_FIFOEmpty);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_EMPTY,ENABLE);
		// set jpeg done 
		VIM_JPEG_RegisterISR(VIM_INT_JPEG_DEC_DONE,VIM_MAPI_ISR_JpegDecDone);
		VIM_JPEG_SetIntEnable(VIM_INT_JPEG_DEC_DONE,ENABLE);
		
}
void VIM_MAPI_ClearDispInterrupt(void)
{
		//VIM_INT_Acknowledge(VM_HIF_INT_MARB);
		//VIM_INT_Acknowledge(VM_HIF_INT_JPEG);
		
		VIM_MARB_RegisterISR(MARB_INT_ALL,NULL);
		VIM_JPEG_RegisterISR(VIM_JPEG_JPEG_INT_ALL,NULL);
		
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_THRESH_LOW,DISABLE);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_EMPTY,DISABLE);
		VIM_JPEG_SetIntEnable(VIM_INT_JPEG_DEC_DONE,DISABLE);
}
void VIM_MAPI_ClearCapInterrupt(void)
{
		VIM_INT_Acknowledge(VM_HIF_INT_MARB);
		//VIM_MARB_RegisterISR(MARB_INT_ALL,NULL);
	VIM_MARB_SetIntEnable(MARB_INT_FIFO0_FULL,DISABLE);
	switch(gVc0578b_Info.CaptureStatus.Mode)
	{
		case VIM_MULTISHOT:
			VIM_MARB_SetIntEnable(MARB_INT_FIFO0_DONE,DISABLE);
			VIM_MARB_SetIntEnable(MARB_INT_FIFO0_THRESH_UP,DISABLE);
			VIM_MARB_SetIntEnable(MARB_INT_FIFO0_FRAME_END,DISABLE);
			break;
		case VIM_CAPTUREVIDEO:
		case VIM_CAPTURESTILL:
			VIM_MARB_SetIntEnable(MARB_INT_FIFO0_DONE,DISABLE);
			VIM_MARB_SetIntEnable(MARB_INT_FIFO0_THRESH_UP,DISABLE);
			break;
		case VIM_CAPTURECAPTURE:
			VIM_MARB_SetIntEnable(MARB_INT_FIFO0_DONE,DISABLE);
			VIM_MARB_SetIntEnable(MARB_INT_FIFO0_LARGE,DISABLE);	
			break;
		case VIM_CAPTUREMPE4:
			VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_ENC_DONE,DISABLE);
			break;
		default:
			break;
	}
}


/********************************************************************************
Description:
	init interrupt of display function
Parameters:
	Note:
		this funcion should be called in the capture function high api
	Remarks:
		state: valid 
*********************************************************************************/
void VIM_MAPI_InitDecoInterrupt(void)
{	
	//clear interrupt
	//VIM_INT_Acknowledge(VM_HIF_INT_MARB);
	////VIM_INT_Acknowledge(VM_HIF_INT_JPEG);
	//VIM_INT_Acknowledge(VM_HIF_INT_LBUF);
		
	//VIM_MARB_RegisterISR(MARB_INT_ALL,NULL);
	//VIM_JPEG_RegisterISR(VIM_JPEG_JPEG_INT_ALL,NULL);
	//VIM_LBUF_RegisterISR(VIM_INT_LBUF_INT_ALL,NULL);
		
	//enalbe jbuf error interrrupt
	/*VIM_MARB_RegisterISR(MARB_INT_FIFO0_FULL,VIM_MAPI_ISR_JbufError);
	VIM_MARB_RegisterISR(MARB_INT_FIFO1_FULL,VIM_MAPI_ISR_JbufError);*/
	//VIM_MARB_RegisterISR(MARB_INT_ADDERROR,VIM_MAPI_ISR_DispJbufError);
	/*VIM_MARB_SetIntEnable(MARB_INT_FIFO0_FULL,ENABLE);
	VIM_MARB_SetIntEnable(MARB_INT_FIFO1_FULL,ENABLE);*/
	//VIM_MARB_SetIntEnable(MARB_INT_ADDERROR,ENABLE);	
	
	//VIM_MARB_RegisterISR(MARB_INT_FIFO0_THRESH_LOW, VIM_MAPI_ISR_ThreshLow);
	//VIM_MARB_SetIntEnable(MARB_INT_FIFO0_THRESH_LOW,ENABLE);
	//VIM_MARB_RegisterISR(MARB_INT_FIFO0_EMPTY,VIM_MAPI_ISR_FIFOEmpty);
	//VIM_MARB_SetIntEnable(MARB_INT_FIFO0_EMPTY,ENABLE);
	// set jpeg done 
	//VIM_JPEG_RegisterISR(VIM_INT_JPEG_DEC_DONE,VIM_MAPI_ISR_JpegDecDone);
	//VIM_JPEG_SetIntEnable(VIM_INT_JPEG_DEC_DONE,ENABLE);
	//VIM_LBUF_RegisterISR(VIM_INT_LBUF_READ_INT,VIM_MAPI_ISR_LbufRead);
	VIM_LBUF_SetIntEnable(VIM_INT_LBUF_READ_INT,ENABLE);
		
}

void VIM_MAPI_DecodelbufEn(VIM_MARB_WORKMODE bMode)
{
	//UINT8 LineStatus=0;

	//LineStatus=VIM_HIF_GetReg8(V5_REG_LBUF_STATUS+2);

	switch(bMode)
	{
		case VIM_MARB_DECODEONLY_MODE:
			 VIM_HIF_SetReg8(V5_REG_LBUF_STATUS+2,BIT0);
			 break;
		case VIM_MARB_DECODEYUV_MODE:
		case VIM_MARB_DECODERGB_MODE:
		case VIM_MARB_DECODERGBBYPASS_MODE:
			 VIM_HIF_SetReg8(V5_REG_LBUF_STATUS+2,BIT1);
			 break;
		default:
			break;
	}
	VIM_HIF_SetReg8(V5_REG_LBUF_STATUS+2,BIT2);
	return;	
}
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
UINT32 VIM_MAPI_WriteJpegDataToFIFO(HUGE UINT8 *addr,UINT32 rawdatalen,VIM_HIF_SRAMMODE Mode)
{
	//return VIM_HIF_WriteSram(0,addr,rawdatalen,Mode);
	return VIM_HIF_WriteSram(0x80000000,addr,rawdatalen,Mode);
}
/********************************************************************************
Description:
	start decode motion jpeg do
parameters: 
	void
Return:
     	void
Remarks:
*********************************************************************************/
void VIM_MAPI_StartDecodeMJpeg(void)
{
	VIM_MARB_ResetSubModule(VIM_MARB_SUB_RESET_JPEG);
	VIM_MARB_SetJpegbufGapratio(5,VIM_MARB_FIFO_0);
    VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
	//set jpeg each table from jpeg file
    VIM_JPEG_SetMotionJpegInfo(&gVc0578b_JpegInfo);
	VIM_JPEG_StartDecode();	
}
void VIM_MAPI_StartDecodeYUVBlock(void)
{
	VIM_MARB_ResetSubModule(VIM_MARB_SUB_RESET_JPEG);
	//VIM_MARB_SetJpegbufGapratio(5,VIM_MARB_FIFO_0);
    VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
	//set jpeg each table from jpeg file
 	VIM_JPEG_StartDecode();	
}
void  VIM_MAPI_WriteUYVYEncodeDataLine(UINT8 *pbuf,UINT16 Width,UINT16 Height)
{
	UINT16 uint,i,j,m,leavex,uint1;
	
	uint=uint1=Width/16;
	leavex=Width%16;
	if(leavex)
		uint1++;
	if(!Height)
		return;
	
	for(i=0;i<uint1;i++)
	{
		for(j=0;j<8;j++)	//y0 block
			for(m=0;m<8;m++)
				{
					if(((m>leavex)&&(i==uint))||(Height==j))
						VIM_JPEG_WriteYUVData(0);
					else
						VIM_JPEG_WriteYUVData(pbuf[i*32+j*Width*2+m*2+1]+128);
				}
		for(j=0;j<8;j++)	//y1 block
			for(m=0;m<8;m++)
				{
					if((m>(leavex+8))&&(i==uint)||(Height==j))
						VIM_JPEG_WriteYUVData(0);
					else
						VIM_JPEG_WriteYUVData(pbuf[i*32+16+2*j*Width+2*m+1]+128);
				}
		for(j=0;j<8;j++)	//u block
			for(m=0;m<8;m++)
				{
					if((m>(leavex*2))&&(i==uint)||(Height==j))
						VIM_JPEG_WriteYUVData(0);
					else
						VIM_JPEG_WriteYUVData(pbuf[i*32+j*Width*2+4*m]+128);
				}
		for(j=0;j<8;j++)	//v block
			for(m=0;m<8;m++)
				{
					if((m>(leavex*2))&&(i==uint)||(Height==j))
						VIM_JPEG_WriteYUVData(0);
					else
					VIM_JPEG_WriteYUVData(pbuf[i*32+j*Width*2+4*m+2]+128);
				}
	}

	return;
}

void VIM_MAPI_SendYUVData(UINT8 *psrcbuf,TSize Size,VIM_JPEG_YUVMODE YUVmode)
{
	UINT32 i = 0;

	switch(YUVmode)
	{
		case JPEG_422:
			for(i=0;i<(Size.cy/8);i++)
				VIM_MAPI_WriteUYVYEncodeDataLine(psrcbuf+i*8*(Size.cx<<1),Size.cx,8);
			VIM_MAPI_WriteUYVYEncodeDataLine(psrcbuf+i*8*(Size.cx<<1),Size.cx,(Size.cy%8));
			break;
		default:
			break;
	}

	return;
}

void VIM_MAPI_SetDisplayYUVMode(void)
{
	VIM_SIF_EnableSyncGen(DISABLE);
    VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
    //  VIM_USER_DelayMs(100); 
    VIM_MAPI_SetChipMode(VIM_MARB_DISPLAYSTILLBYPASS_MODE,VIM_IPP_HAVE_NOFRAME);
    // 	VIM_USER_DelayMs(100); 
    VIM_IPP_SetDisplayDropFrame(0xffff);
}


void VIM_MAPI_ISR_SIFVSYNC(void)
{

	VIM_USER_PrintString("Sif Vsysc int\n");

}
void VIM_MAPI_ISR_SIFFRAMEEND(void)
{
	VIM_USER_PrintString("Sif FrameEnd int\n");
}
void VIM_MAPI_FreeLCDCBuffer(void)
{
	if(gVc0578b_Info.MarbStatus.MapList.layerB0start)
	{
		if(gVc0578b_Info.MarbStatus.MapList.layerB0start < 0x40000000 ||  MMD_SYS_Mode_Get() >2)	
		{
			//printf("Free (0x%x) \n",gVc0578b_Info.MarbStatus.MapList.layerB0start );
			MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.layerB0start);
		}
		gVc0578b_Info.MarbStatus.MapList.layerB0start=0;
	}
		
	if(gVc0578b_Info.MarbStatus.MapList.layerB1start)
	{
		if(gVc0578b_Info.MarbStatus.MapList.layerB1start < 0x40000000 ||  MMD_SYS_Mode_Get() >2)	
		{
			MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.layerB1start);
		}
		gVc0578b_Info.MarbStatus.MapList.layerB1start=0;
	}
		
}
void VIM_MAPI_FreeLCDCA0Buffer(void)
{
	if(gVc0578b_Info.MarbStatus.MapList.layerA0start)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.layerA0start);
		gVc0578b_Info.MarbStatus.MapList.layerA0start=0;
	}
		
}


void VIM_MAPI_FreeDisplayJpegBuffer(void)
{
	if(gVc0578b_Info.MarbStatus.MapList.layerA0start)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.layerA0start);
		gVc0578b_Info.MarbStatus.MapList.layerA0start=0;
	}
		
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart=0;
	}
		
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart=0;
	}
		
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart=0;
	}
		
	if(gVc0578b_Info.MarbStatus.MapList.jbufstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.jbufstart);
		gVc0578b_Info.MarbStatus.MapList.jbufstart=0;
	}
		
	return;
}
void VIM_MAPI_FreeDecodeJpegBuffer(void)
{
	if(gVc0578b_Info.MarbStatus.MapList.layerA0start)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.layerA0start);
		gVc0578b_Info.MarbStatus.MapList.layerA0start=0;
	}
		
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart=0;
	}
		
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart=0;
	}
		
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart=0;
	}
	
	/*if(gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart);
		gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart=0;
	}
		
	if(gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart);
		gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart=0;
	}*/
		
	if(gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart);
		gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart=0;
	}
	
	if(gVc0578b_Info.MarbStatus.MapList.jbufstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.jbufstart);
		gVc0578b_Info.MarbStatus.MapList.jbufstart=0;
	}
		
	return;
}
void VIM_MAPI_FreeDisplayVideoBuffer(void)
{
#if LCDCA0SDRAM//LCDCTEST
	if(gVc0578b_Info.MarbStatus.MapList.layerA0start)
	{
		gVc0578b_Info.MarbStatus.MapList.layerA0start=0;
	}
#else
	if(gVc0578b_Info.MarbStatus.MapList.layerA0start)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.layerA0start);
		gVc0578b_Info.MarbStatus.MapList.layerA0start=0;
	}
#endif

	if(gVc0578b_Info.MarbStatus.MapList.layerA1start)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.layerA1start);
		gVc0578b_Info.MarbStatus.MapList.layerA1start=0;
	}
	
	if(gVc0578b_Info.MarbStatus.MapList.Total_start)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.Total_start);
		gVc0578b_Info.MarbStatus.MapList.Total_start=0;
	}
	return;
}
void VIM_MAPI_FreeCaptureVideoBuffer(void)
{
#if VIM_USER_MPEG4_FRAMEMODE

	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart=0;
	}
	
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart=0;
	}

	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart=0;
	}

	if(gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart);
		gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart=0;
	}

	if(gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart);
		gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart=0;
	}

	if(gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart);
		gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart=0;
	}
#else
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart=0;
		gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart=0;
	}
	
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart=0;
		gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart=0;
	}

	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart=0;
		gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart=0;
	}
#endif
	if(gVc0578b_Info.MarbStatus.MapList.Total_start)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.Total_start);
		gVc0578b_Info.MarbStatus.MapList.Total_start=0;
	}
	
	return;
}

void VIM_MAPI_FreeCaptureJpegBuffer(void)
{
	if(gVc0578b_Info.MarbStatus.MapList.layerA0start)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.layerA0start);
		gVc0578b_Info.MarbStatus.MapList.layerA0start=0;
	}

	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart=0;
	}
			
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart=0;
	}
			
	if(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart);
		gVc0578b_Info.MarbStatus.MapList.lbuf0Ystart=0;
	}

	if(gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart);
		gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart=0;
	}
				
	if(gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart);
		gVc0578b_Info.MarbStatus.MapList.lbuf1Vstart=0;
	}
				
	if(gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart);
		gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart=0;
	}

	if(gVc0578b_Info.MarbStatus.MapList.jbufstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.jbufstart);
		gVc0578b_Info.MarbStatus.MapList.jbufstart=0;
	}

	if(gVc0578b_Info.MarbStatus.MapList.thumbstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.thumbstart);
		gVc0578b_Info.MarbStatus.MapList.thumbstart=0;
	}
	return;
}

void VIM_MAPI_FreeEncodeJpegBuffer(void)
{

	if(gVc0578b_Info.MarbStatus.MapList.jbufstart)
	{
		MMD_SYS_Free(gVc0578b_Info.MarbStatus.MapList.jbufstart);
		gVc0578b_Info.MarbStatus.MapList.jbufstart=0;
	}

	return;
}

#endif

/********************************************************************************
Description:
	the capture done call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

UINT32 VIM_MAPI_Encode_JbufDone(void)
{
	UINT32 length;

	VIM_JPEG_StartEncodeYUV(DISABLE);
	//VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	VIM_MARB_SetFifoStop(VIM_MARB_FIFO_0);

	length=VIM_MARB_GetJbufRWSize();
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID] VIM_MAPI_ISR_JbufDone,length is ",length);
#endif
	if(length<=gVc0578b_Info.CaptureStatus.BufLength)
	{
		VIM_MARB_ReadJpegData(gVc0578b_Info.CaptureStatus.BufPoint,length);
              gVc0578b_Info.CaptureStatus.BufLength-=length;
              gVc0578b_Info.CaptureStatus.BufPoint+=length;
              gVc0578b_Info.CaptureStatus.CapFileLength+=length;
       }
       else
       {
              gVc0578b_Info.CaptureStatus.CaptureError=VIM_ERROR_CAPTURE_BUFFULL;
		VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
              return 0;	
	}
                
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	VIM_MAPI_ClearCapInterrupt();
	return length;
}

/********************************************************************************
Description:
	the capture done call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_JbufDone(void)
{
UINT32 length,dwSavelen=0;
UINT8 qf;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID] qf3 times is =",VIM_HIF_GetReg8(V5_REG_JPEG_BRC));
#endif
//revised by zhoujian_2008.01.29
#if (BOARD_TYPE==VC0820_FPGA) //add by zhoujian 
	VIM_JPEG_StartEncode(DISABLE);
	//VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	VIM_MARB_SetFifoStop(VIM_MARB_FIFO_0);
	VIM_JPEG_SetBitRateControlEn(DISABLE);	
#else
	VIM_MARB_SetFifoStop(VIM_MARB_FIFO_0);
#endif	
	if(gVc0578b_Info.CaptureStatus.CaptureError)
		goto CAPUTREERROR;	
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
						gVc0578b_Info.CaptureStatus.ErrorImageRecaptureTimes++;
						//MMDPrintf("\ncapture image error!\n");
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
				gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_CAPTURE_END,length);
			}
			gVc0578b_Info.CaptureStatus.Mode=VIM_CAPTUREDONE;
			goto CAPUTREOK;
		break;
	}
RECAPUTRE:
	VIM_JPEG_SetBitRateControlEn(DISABLE);
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	qf=VIM_JPEG_GetQFactor();
	VIM_JPEG_SetBrcQF(qf);

	if(gVc0578b_Info.CaptureStatus.ErrorImageRecaptureTimes<VIM_USER_RECAPTUREMAXTIMES)
	{
		VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
		VIM_JPEG_StartEncode(ENABLE);
	}
	else
	{
		VIM_HIF_ClearIntModule(VM_HIF_INT_MARB);
		VIM_HIF_ClearIntModule(VM_HIF_INT_JPEG);
		gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_BUF_ERROR,gVc0578b_Info.CaptureStatus.CaptureError);
	}
	return;

CAPUTREERROR:
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);

	if(gVc0578b_Info.CaptureStatus.CapCallback)
	{
		//VIM_USER_StopTimer();
		VIM_HIF_ClearIntModule(VM_HIF_INT_MARB);
		VIM_HIF_ClearIntModule(VM_HIF_INT_JPEG);
		gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_BUF_ERROR,gVc0578b_Info.CaptureStatus.CaptureError);
	}
	if(gVc0578b_Info.CaptureStatus.SaveMode==VIM_HAPI_ROM_SAVE)//angela2007-1-24
	{
#if (BOARD_TYPE==VC0820_FPGA) //add by zhoujian 
#else
		gVc0578b_Info.CaptureStatus.CaptureError=VIM_USER_FreeMemory(gVc0578b_Info.CaptureStatus.MallocPr);
		#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
			VIM_USER_PrintString("[VC0578B][MID] free memory because capture error[ERROR]  ");
		#endif
		gVc0578b_Info.CaptureStatus.MallocPr=0;
		gVc0578b_Info.CaptureStatus.BufPoint=0;
#endif
	}
	return;
CAPUTREOK:
//revised by zhoujian_2008.01.29
/*#if (BOARD_TYPE==VC0820_FPGA) //add by zhoujian 
#else*/
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
//#endif

	if((gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_FRAMEON)&&(gVc0578b_Info.VideoStatus.CapMode==VIM_VIDEO_STOP))
	{
		VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
		gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;
	}

	//VIM_HIF_ClearIntModule(VM_HIF_INT_MARB);//angela 2007-5-14
	VIM_MAPI_ClearCapInterrupt();
	if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_OFF)
		VIM_MAPI_EnableSyncGen(DISABLE);	//angela 2006-9-4
	if(gVc0578b_Info.CaptureStatus.SaveMode==VIM_HAPI_RAM_SAVE)
	{
		if(gVc0578b_Info.CaptureStatus.CapCallback)
			{
				//VIM_USER_StopTimer();
				gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_CAPTURE_END,gVc0578b_Info.CaptureStatus.CapFileLength);
			}
	}

	return;
}
/********************************************************************************
Description:
	the one frame have been encode call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_OneFrameDone(void)
{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	MMDPrintf("\n[VC0578B][MID] VIM_MAPI_ISR_OneFrameDone,rw=%d\n",VIM_MARB_GetJbufRWSize());
	VIM_USER_PrintDec("[VC0578B][MID] VIM_MAPI_ISR_OneFrameDone,rw=",VIM_MARB_GetJbufRWSize());
#endif
}

void VIM_MAPI_ISR_OneFrameEND(void)
{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	MMDPrintf("\n[VC0578B][MID] VIM_MAPI_ISR_marbOneFrameend!!");
	//VIM_USER_PrintDec("[VC0578B][MID] VIM_MAPI_ISR_OneFrameDone,rw=",VIM_MARB_GetJbufRWSize());
#endif
}


/********************************************************************************
Description:
	fifo count  call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_FifoCont(void)
{
UINT32 length,OneFifocount=VIM_MARB_GetJbufUpThreshold();

	length=VIM_MARB_GetJbufRWSize();
	if(!length)		//angela 2007-5-8
		return;
	switch(gVc0578b_Info.CaptureStatus.Mode)
	{
		case VIM_CAPTURESTILL:
		case VIM_CAPTUREMPE4:
FIFOREAD:
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID] VIM_MAPI_ISR_FifoCont length=",length);
	//VIM_USER_PrintDec("[VC0578B][MID] fifo count=",OneFifocount);
	//VIM_USER_PrintDec("[VC0578B][MID] JBUF SIZE=",gVc0578b_Info.MarbStatus.MapList.jbufsize);
#endif

		if(gVc0578b_Info.CaptureStatus.SaveMode==VIM_HAPI_RAM_SAVE)
		{
			if(length<=gVc0578b_Info.CaptureStatus.BufLength)
			{
				VIM_MARB_ReadJpegData(gVc0578b_Info.CaptureStatus.BufPoint,length);
				gVc0578b_Info.CaptureStatus.BufPoint+=length;
				gVc0578b_Info.CaptureStatus.BufLength-=length;
				gVc0578b_Info.CaptureStatus.CapFileLength+=length;
				gVc0578b_Info.VideoStatus.CapVideoLength+=length;
				length=VIM_MARB_GetJbufRWSize();
				if(!length)
				{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
					VIM_USER_PrintString("[VC0578B][MID] length==0");
#endif	
					goto FIFOOK;
				}
				if(length>=(OneFifocount-2*VIM_MARB_JBUF_GAPINTERVAL))
					goto FIFOREAD;
				else
					goto FIFOOK;
			}
			else
			{
				gVc0578b_Info.CaptureStatus.CaptureError=VIM_ERROR_CAPTURE_BUFFULL;
				goto FIFOERROR;		
			}
		}
		else
		{
				gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_FIFO_UP,length);
		}
		goto FIFOOK;

		break;
			

	}
FIFOERROR:
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
	VIM_USER_PrintDec("[VC0578B][MID] VIM_MAPI_ISR_FifoCont [error]",gVc0578b_Info.CaptureStatus.CaptureError);
#endif	
	VIM_MAPI_ISR_JbufError();
	//VIM_USER_StopTimer();
	//VIM_HIF_ClearIntModule(VM_HIF_INT_MARB);
	//VIM_HIF_ClearIntModule(VM_HIF_INT_JPEG);
	//if(gVc0578b_Info.CaptureStatus.CapCallback)
		//gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_BUF_ERROR,gVc0578b_Info.CaptureStatus.CaptureError);
	return;
FIFOOK:
#if (BOARD_TYPE==VC0820_FPGA) //add by zhoujian 
	if(gVc0578b_Info.CaptureStatus.SaveMode==VIM_HAPI_RAM_SAVE)
	{
		if(gVc0578b_Info.CaptureStatus.CapCallback)
			gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_FIFO_FULL,gVc0578b_Info.CaptureStatus.CapFileLength);
	}
#else
	if(gVc0578b_Info.CaptureStatus.CapCallback)
		gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_FIFO_FULL,gVc0578b_Info.CaptureStatus.CapFileLength);
#endif
	return;

}
/********************************************************************************
Description:
	the capture done call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_JbufError(void)
{
//UINT16 bIntetype,i,addflag=0;
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
		gVc0578b_Info.CaptureStatus.BufPoint-=gVc0578b_Info.CaptureStatus.CapFileLength;
		gVc0578b_Info.CaptureStatus.BufLength+=gVc0578b_Info.CaptureStatus.CapFileLength;
		gVc0578b_Info.CaptureStatus.CapFileLength=0;

		if(gVc0578b_Info.CaptureStatus.SaveMode==VIM_HAPI_ROM_SAVE)
		{

		}
	       //VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
       	return;
	}

/*	for(i=0;i<8;i++)
	{
		bIntetype=gVc0578b_Isr.byFirstLevelInt[i];
		if(bIntetype==VM_HIF_INT_MARB)
			addflag=gVc0578b_Isr.bSecondLevelIntFlag[i];
	}


	if(addflag&BIT15)
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][MID] MARB ADDRERROR [ERROR]");
#endif	
		gVc0578b_Info.CaptureStatus.CaptureError=VIM_ERROR_CAPTURE_MARBADDRESS;
	}
	else if(addflag&BIT0)
	{
		gVc0578b_Info.CaptureStatus.CaptureError=VIM_ERROR_CAPTURE_OVERFLOW;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][MID]  jbuf overflow error[ERROR]");
#endif		
	}
	else if(addflag&BIT8)
	{
		gVc0578b_Info.CaptureStatus.CaptureError=VIM_ERROR_CAPTURE_THUMB_OVERFLOW;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][MID]  THUMB BUF overflow error[ERROR]");
#endif		
	}
	else
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintString("[VC0578B][MID] error unknow[ERROR]");
		VIM_USER_PrintHex("[VC0578B][MID] marbflag=",addflag);
#endif		
		gVc0578b_Info.CaptureStatus.CaptureError=VIM_ERROR_CAPTURE_OVERFLOW;
	}
	if(gVc0578b_Info.CaptureStatus.CapCallback)
	{
		VIM_USER_StopTimer();
		VIM_MARB_RegisterISR(MARB_INT_FIFO0_THRESH_UP,NULL);//0307
		VIM_HIF_ClearIntModule(VM_HIF_INT_MARB);
		VIM_HIF_ClearIntModule(VM_HIF_INT_JPEG);
		gVc0578b_Info.CaptureStatus.CapCallback(VIM_HAPI_BUF_ERROR,gVc0578b_Info.CaptureStatus.CaptureError);
	}*/
	
}

/********************************************************************************
Description:
	fake address error interrupt 
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_AddressError(void)
{
#if (BOARD_TYPE==VC0820_FPGA)
	UINT32 uValue;

#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
	VIM_USER_PrintString("[VC0578B][MID] VIM_MAPI_ISR_AddressError [ERROR]");
#endif	
	uValue=	VIM_HIF_GetReg32(V5_REG_MARB_ADDRERROR);
	VIM_HIF_SetReg32(V5_REG_MARB_ADDRERROR,uValue);
	VIM_USER_PrintHex("[VC0578B][MID] error address=",uValue);
#if 0
	VIM_MARB_SetFifoStop(VIM_MARB_FIFO_0);
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
#endif	
#else
	VIM_MAPI_ISR_JbufError();
#endif
	
}
/********************************************************************************
Description:
	the one frame have been encode call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_Recapture(void)
{
UINT8 qf;
UINT32 Length;
	Length=VIM_MARB_GetJbufRWSize();
	//revised by zhoujian_2008.01.29
#if (BOARD_TYPE==VC0820_FPGA) //add by zhoujian 
	VIM_JPEG_StartEncode(DISABLE);
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);	
#endif	
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID] fifo length is =",Length);
	VIM_USER_PrintDec("[VC0578B][MID] qf is =",VIM_JPEG_GetQFactor());
	VIM_USER_PrintString("[VC0578B][MID]VIM_MAPI_ISR_Recapture");
	VIM_USER_PrintDec("[VC0578B][MID] recapture time is=",gVc0578b_Info.CaptureStatus.RecaptureTimes);
#endif
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
*********************************************************************************/

//revised by youhai_mp4enc 2007.8.30	
#if (BOARD_TYPE==VC0820_FPGA)
void VIM_MAPI_Osd_Lcdc_Init_ForDemo(void)
{
TPoint uppoint;
TSize  upsize;
//int i;
UINT8 *ptr;
uppoint.x=0x10;
uppoint.y=0;
upsize.cx=0x3c;
upsize.cy=0x3c;

                     //VIM_DISP_SetB0_DisplayPanel();
			//VIM_HIF_SetReg32(0x6008001c, 0x20050000);//LCDC address
			ptr=(UINT8 *)VIM_HIF_GetReg32(V5_REG_LCDC_GBBA0);
#if DRV_578RDK_TEST_ENA
			for(i=0;i<10800;)
				{
				*ptr++=osd_6060[i++];
				}
#endif			
			//mpeg4_wr_sram(0x20050000, osd_6060, 10800);	
				//VIM_HIF_SetReg32(0x60080024, 0x00000000);//a0_lcd_pos
			//tempvalue = ((width-1) << 16) | (height-1);
			//VIM_HIF_SetReg32(0x60080028, tempvalue);
			//VIM_HIF_SetReg32(0x6008002c, 0x00000000);
			VIM_DISP_ResetState();//VIM_HIF_SetReg32(V5_REG_LCDC_RESET, 0x00000001);
			//VIM_HIF_SetReg32(0x60080000,0x081deb00);//0x08158904);//V5_REG_LCDC_CTRL 
			VIM_DISP_SetB0_RefreshBlock(uppoint,upsize);
			//VIM_HIF_SetReg32(0x6008003c, 0x00100030);
			//VIM_HIF_SetReg32(0x60080040, 0x003b003b);
			//VIM_HIF_SetReg32(0x6008004c, 0x00000000);
			//VIM_HIF_SetReg32(0x60080044, 0x00010000);
			//VIM_HIF_SetReg32(0x60080048, 0x003b003b);
			//VIM_HIF_SetReg32(0x600800b8, 0x0000ffa0);
			//VIM_HIF_SetReg32(0x60080064, 0x0000003c);	

}

#endif
/********************************************************************************
Description:
	fifo count  call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_MpegEncDone(void)
{
UINT32 MpegFlag,length;
	MpegFlag=VIM_MPEG4_GetEncIntFlag();
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintHex("[VC0578B][MID] VIM_MPEG4_GetIntFlag  is =",MpegFlag);
#endif
	length=VIM_MPEG4_GetLength(0,VIM_USER_MPEG4_FRAMS);
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintHex("len  is =",length);
#endif
	if(gVc0578b_Info.VideoStatus.Mp4EncCallBack)
		gVc0578b_Info.VideoStatus.Mp4EncCallBack(MpegFlag,length);

 }

/********************************************************************************
Description:
	fifo count  call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_Mpeg4Vsyc(void)
{
//revised by youhai_mp4enc 2007.8.30	
#if (BOARD_TYPE==VC0820_FPGA)
#else
	gVc0578b_Info.Mpeg4Status.m4Ticks++;
#endif
}



/********************************************************************************
Description:
	fifo count  call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_MpegTIMER(void)
{
UINT8 TestSram[128];
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	//VIM_USER_PrintString("[VC0578B][MID] VIM_MAPI_ISR_MpegTIMER  is =");
#endif


	VIM_HIF_ReadSram(0, TestSram, 128,VIM_HIF_SRAM_MODE);
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	//VIM_USER_PrintHex("[VC0578B][MID]buf[0]=",TestSram[0]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[1]=",TestSram[1]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[0]=",TestSram[64]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[1]=",TestSram[65]);
	/*VIM_HIF_ReadSram(gVc0578b_Info.MarbStatus.MapList.lbuf1Ustart, TestSram, 20,VIM_HIF_SRAM_MODE);
	VIM_USER_PrintHex("[VC0578B][MID]buf[0]=",TestSram[0]);
	VIM_USER_PrintHex("[VC0578B][MID]buf[1]=",TestSram[1]);
	VIM_HIF_ReadSram(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart, TestSram, 20,VIM_HIF_SRAM_MODE);
	VIM_USER_PrintHex("[VC0578B][MID]buf[0]=",TestSram[0]);
	VIM_USER_PrintHex("[VC0578B][MID]buf[1]=",TestSram[1]);*/
	
	//VIM_USER_PrintHex("[VC0578B][MID]buf[2]=",TestSram[2]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[3]=",TestSram[3]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[4]=",TestSram[4]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[5]=",TestSram[5]);
#endif

}
/********************************************************************************
Description:
	fifo error
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_MpegError(void)
{
UINT32 length;
	length=VIM_MARB_GetJbufRWSize();
	if(!length)		
		return;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_ERROR)
	VIM_USER_PrintString("[VC0578B][MID] VIM_MAPI_ISR_MpegError");
#endif
	gVc0578b_Info.VideoStatus.CapMode=VIM_VIDEO_STOP;
	gVc0578b_Info.VideoStatus.DisMode=VIM_VIDEO_STOP;
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;
	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,DISABLE);
	VIM_HIF_ClearIntModule(VM_HIF_INT_MARB);
	VIM_HIF_ClearIntModule(VM_HIF_INT_VENC);
	VIM_HIF_ClearIntModule(VM_HIF_INT_VDEC);
	VIM_HIF_ClearIntModule(VM_HIF_INT_HWT);
	//VIM_USER_StopTimer();
	VIM_MAPI_EnableSyncGen(DISABLE);	//angela 2006-9-4
	gVc0578b_Info.CaptureStatus.CaptureError=VIM_ERROR_CAPTURE_ROMOVERFLOW;
	if(gVc0578b_Info.VideoStatus.Callback)
		gVc0578b_Info.VideoStatus.Callback(VIM_HAPI_MPE4_ERROR,0,gVc0578b_Info.CaptureStatus.CaptureError);

}
/********************************************************************************
Description:
	decode call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_MpegDecDone(void)
{
	UINT32 temp;
	temp =VIM_MPEG4_GetDecIntFlag();
	gVc0578b_Info.Mpeg4Status.DisInfo.Mp4DecCallback(temp);
	return;
}
#if (BOARD_TYPE==VC0820_FPGA)

void VIM_MAPI_ISR_MpegDecDoneDebug(void)
{
	VIM_MPEG4_DECINTFLAG MpegFlag;

	MpegFlag=VIM_MPEG4_GetDecIntFlag();
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintHex("[VC0578B][MID] MpegFlag  is =",MpegFlag);
#endif
	/*gVc0578b_Info.PreviewStatus.LCDCA0FrameEndTime=VIM_USER_GetSystemTime_1us();
	gVc0578b_Info.PreviewStatus.LCDCA0FrameTimes=gVc0578b_Info.PreviewStatus.LCDCA0FrameEndTime-gVc0578b_Info.PreviewStatus.LCDCA0FrameStartTime;
	MMDPrintf("mp4 decode use %dus \n",gVc0578b_Info.PreviewStatus.LCDCA0FrameTimes);
	gVc0578b_Info.PreviewStatus.LCDCA0FrameStartTime=VIM_USER_GetSystemTime_1us();*/
}
void VIM_MAPI_ISR_LCDCA0DoneDebug(void)
{

	//MMDPrintf("LCDCA0 done int \n");
	//VIM_DISP_SetIntEnable(VIM_INT_DISP_A0_DONE,DISABLE);
	//gVc0578b_Info.DisplayStatus.LCDCA0done=1;
	/*gVc0578b_Info.PreviewStatus.LCDCA0FrameEndTime=VIM_USER_GetSystemTime_10ms()*10;
	gVc0578b_Info.PreviewStatus.LCDCA0FrameTimes=gVc0578b_Info.PreviewStatus.LCDCA0FrameEndTime-gVc0578b_Info.PreviewStatus.LCDCA0FrameStartTime;
	MMDPrintf("one frame use %dms \n",gVc0578b_Info.PreviewStatus.LCDCA0FrameTimes);*/

}

#endif

/********************************************************************************
Description:
	fifo count  call back function
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_DecFifo(void)
{
UINT32 length;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID] VIM_MAPI_ISR_DecFifo  is ",gVc0578b_Info.DisplayStatus.BufLength);
#endif
	length=VIM_MAPI_WriteOneBlockData(gVc0578b_Info.DisplayStatus.BufLength,gVc0578b_Info.DisplayStatus.BufPoint);
	gVc0578b_Info.DisplayStatus.BufLength-=length;
	gVc0578b_Info.DisplayStatus.BufPoint+=length;
}
/********************************************************************************
Description:
	fifo empty
Parameters:
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_ISR_DecEmpty(void)
{
UINT8 Temp[64];
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintString("[VC0578B][MID] VIM_MAPI_ISR_DecEmpty  is =");
#endif
	VIM_USER_MemSet(Temp, 0, 64);
	VIM_MAPI_WriteOneBlockData(64,Temp);

}
void VIM_MAPI_DebugFrame(void)
{
UINT8 TestSram[128];
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintString("[VC0578B][MID]VIM_INT_SIF_VSYNC_INT");
#endif
	VIM_HIF_ReadSram(0, TestSram, 128,VIM_HIF_SRAM_MODE);
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintHex("[VC0578B][MID]buf[0]=",TestSram[0]);
	VIM_USER_PrintHex("[VC0578B][MID]buf[1]=",TestSram[1]);
	VIM_USER_PrintHex("[VC0578B][MID]buf[0]=",TestSram[64]);
	VIM_USER_PrintHex("[VC0578B][MID]buf[1]=",TestSram[65]);
	/*VIM_HIF_ReadSram(gVc0578b_Info.MarbStatus.MapList.lbuf0Ustart, TestSram, 20,VIM_HIF_SRAM_MODE);
	VIM_USER_PrintHex("[VC0578B][MID]buf[0]=",TestSram[0]);
	VIM_USER_PrintHex("[VC0578B][MID]buf[1]=",TestSram[1]);
	VIM_HIF_ReadSram(gVc0578b_Info.MarbStatus.MapList.lbuf0Vstart, TestSram, 20,VIM_HIF_SRAM_MODE);
	VIM_USER_PrintHex("[VC0578B][MID]buf[0]=",TestSram[0]);
	VIM_USER_PrintHex("[VC0578B][MID]buf[1]=",TestSram[1]);*/
	
	//VIM_USER_PrintHex("[VC0578B][MID]buf[2]=",TestSram[2]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[3]=",TestSram[3]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[4]=",TestSram[4]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[5]=",TestSram[5]);
#endif

}
/********************************************************************************
Description:
	init interrupt of capture function
Parameters:
	Note:
		this funcion should be called in the capture function high api
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_InitCapInterrupt(void)
{
	//clear interrupt
	VIM_INT_Acknowledge(VM_HIF_INT_MARB);
	/*//VIM_INT_Acknowledge(VM_HIF_INT_JPEG);
	//VIM_INT_Acknowledge(VM_HIF_INT_VENC);

	//VIM_MARB_RegisterISR(MARB_INT_ALL,NULL);
	//VIM_JPEG_RegisterISR(VIM_JPEG_JPEG_INT_ALL,NULL);
	//VIM_MPEG4_RegisterISR(VIM_MPEG4_INT_ALL,NULL);

	//enalbe jbuf error interrrupt
	VIM_MARB_RegisterISR(MARB_INT_FIFO0_FULL,VIM_MAPI_ISR_JbufError);
	//VIM_MARB_RegisterISR(MARB_INT_FIFO1_FULL,VIM_MAPI_ISR_JbufError);
	//VIM_MARB_RegisterISR(MARB_INT_ADDERROR,VIM_MAPI_ISR_JbufError);
	VIM_MARB_RegisterISR(MARB_INT_ADDERROR,VIM_MAPI_ISR_AddressError);	
	
	VIM_MARB_SetIntEnable(MARB_INT_FIFO1_FULL,ENABLE);
	//VIM_MARB_SetIntEnable(MARB_INT_ADDERROR,ENABLE);*/	
	VIM_MARB_SetIntEnable(MARB_INT_ADDERROR,DISABLE);	
	VIM_MARB_SetIntEnable(MARB_INT_FIFO0_FULL,ENABLE);

	switch(gVc0578b_Info.CaptureStatus.Mode)
	{
	case VIM_MULTISHOT:
		// enable  interrupt of jpeg done
		//VIM_MARB_RegisterISR(MARB_INT_FIFO0_DONE, VIM_MAPI_ISR_JbufDone);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_DONE,ENABLE);
		// set marb fifocnt 
		//VIM_MARB_RegisterISR(MARB_INT_FIFO0_THRESH_UP, VIM_MAPI_ISR_JbufDone);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_THRESH_UP,ENABLE);
		
		// set one frame done
		//VIM_MARB_RegisterISR(MARB_INT_FIFO0_FRAME_END, VIM_MAPI_ISR_OneFrameDone);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_FRAME_END,ENABLE);
		// set jpeg done 
		//VIM_JPEG_RegisterISR(VIM_INT_JPEG_ENC_DONE,VIM_MAPI_ISR_OneFrameDone);
		//VIM_JPEG_SetIntEnable(VIM_INT_JPEG_ENC_DONE,ENABLE);
		break;
	case VIM_CAPTUREVIDEO:
	case VIM_CAPTURESTILL:
		// enable  interrupt of jpeg done
		//VIM_MARB_RegisterISR(MARB_INT_FIFO0_DONE, VIM_MAPI_ISR_JbufDone);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_DONE,ENABLE);
		// set marb fifocnt 
		//VIM_MARB_RegisterISR(MARB_INT_FIFO0_THRESH_UP, VIM_MAPI_ISR_FifoCont);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_THRESH_UP,ENABLE);
		// set jpeg done 
		//VIM_JPEG_RegisterISR(VIM_INT_JPEG_ENC_DONE,VIM_MAPI_ISR_OneFrameDone);
		//VIM_JPEG_SetIntEnable(VIM_INT_JPEG_ENC_DONE,ENABLE);
		break;
	case VIM_CAPTURECAPTURE:
		//VIM_HIF_SetIntEnableSec(VM_HIF_INT_MARB,0x50);
		//VIM_INT_Enable(VM_HIF_INT_MARB);
		//VIM_INT_Unmask(VM_HIF_INT_MARB);
		// enable  interrupt of jpeg done
		//VIM_MARB_RegisterISR(MARB_INT_FIFO0_DONE, VIM_MAPI_ISR_JbufDone);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_THRESH_UP,DISABLE);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_DONE,ENABLE);
		
		//VIM_MARB_RegisterISR(MARB_INT_FIFO0_LARGE,VIM_MAPI_ISR_Recapture);
		VIM_MARB_SetIntEnable(MARB_INT_FIFO0_LARGE,ENABLE);	
		// set jpeg done 
		//VIM_JPEG_RegisterISR(VIM_INT_JPEG_ENC_DONE,VIM_MAPI_ISR_OneFrameDone);
		//VIM_JPEG_SetIntEnable(VIM_INT_JPEG_ENC_DONE,ENABLE);
		break;
#if 0
	case VIM_CAPTUREMPE4:
//revised by youhai_mp4enc 2007.8.31	

		//marb full
		//VIM_MARB_RegisterISR(MARB_INT_FIFO0_FULL,	VIM_MAPI_ISR_MpegError);
		// set marb fifocnt 
		//VIM_MARB_RegisterISR(MARB_INT_FIFO0_THRESH_UP, VIM_MAPI_ISR_FifoCont);
		//VIM_MARB_SetIntEnable(MARB_INT_FIFO0_THRESH_UP,ENABLE);
		//set mpeg 
		VIM_MPEG4_RegisterISR(VIM_INT_MPEG4_ENC_DONE,VIM_MAPI_ISR_MpegEncDone);
		VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_ENC_DONE,ENABLE);

		//set mpeg 
		//VIM_MPEG4_RegisterISR(VIM_INT_MPEG4_HW_TIMER,VIM_MAPI_ISR_MpegTIMER);
		//VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_HW_TIMER,ENABLE);

		//set mpeg 
		/*VIM_MPEG4_RegisterISR(VIM_INT_MPEG4_HW_VSYN,VIM_MAPI_ISR_Mpeg4Vsyc);
		VIM_MARB_RegisterISR(MARB_INT_FIFO0_FULL,VIM_MAPI_ISR_MpegError);
		VIM_MARB_RegisterISR(MARB_INT_FIFO1_FULL,VIM_MAPI_ISR_MpegError);*/
		VIM_MARB_RegisterISR(MARB_INT_ADDERROR,VIM_MAPI_ISR_AddressError);
//for further revised youhai		2007.9.1
		break;
#endif
	default:
		break;
	}

}
/********************************************************************************
Description:
	init interrupt of capture function
Parameters:
	Note:
		this funcion should be called in the capture function high api
	Remarks:
		state: valid
*********************************************************************************/

void VIM_MAPI_InitDisMpeg4Interrupt(void)
{
	//clear interrupt
	if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_VIDEOCONFERENCE_MODE)
		VIM_HIF_ClearIntModule(VM_HIF_INT_MARB);
	VIM_HIF_ClearIntModule(VM_HIF_INT_VDEC);
	if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_VIDEOCONFERENCE_MODE)
	{
		VIM_MARB_RegisterISR(MARB_INT_ALL,NULL);
		VIM_MPEG4_RegisterISR(VIM_MPEG4_INT_ALL,NULL);
		//enalbe marb error interrrupt
		VIM_MARB_RegisterISR(MARB_INT_ADDERROR,VIM_MAPI_ISR_MpegError);
		VIM_MARB_SetIntEnable(MARB_INT_ADDERROR,ENABLE);	
	}


	// set marb fifocnt 
	//VIM_MARB_RegisterISR(MARB_INT_FIFO1_THRESH_LOW, VIM_MAPI_ISR_DecFifo);
	//VIM_MARB_SetIntEnable(MARB_INT_FIFO1_THRESH_LOW,ENABLE);
	// set marb empty 
	//VIM_MARB_RegisterISR(MARB_INT_FIFO0_EMPTY, VIM_MAPI_ISR_DecEmpty);
	//VIM_MARB_SetIntEnable(MARB_INT_FIFO0_EMPTY,ENABLE);
	//set mpeg 
	VIM_MPEG4_RegisterISR(VIM_INT_MPEG4_DEC_DONE,VIM_MAPI_ISR_MpegDecDone);
	VIM_MPEG4_SetIntEnable(VIM_INT_MPEG4_DEC_DONE,ENABLE);
}
void VIM_MAPI_DebugJpegFrame(void)
{
UINT8 TestSram[20];
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintString("[VC0578B][MID]VIM_INT_JPEG_ENC_DONE");
#endif
	VIM_HIF_ReadSram(gVc0578b_Info.MarbStatus.MapList.jbufstart, TestSram, 20,VIM_HIF_FIFO_MODE);

		//VIM_JPEG_SetBitRateControlEn(DISABLE);		//??没有效果
		//VIM_JPEG_SetBrcQF(50);
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID] qf is =",VIM_JPEG_GetQFactor());
	VIM_USER_PrintHex("[VC0578B][MID]buf[0]=",TestSram[0]);
	VIM_USER_PrintHex("[VC0578B][MID]buf[1]=",TestSram[1]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[2]=",TestSram[2]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[3]=",TestSram[3]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[4]=",TestSram[4]);
	//VIM_USER_PrintHex("[VC0578B][MID]buf[5]=",TestSram[5]);
#endif

}

void VIM_MAPI_DebugDispFrame(void)
{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintString("[VC0578B][MID]VIM_INT_DISP_ALL");
#endif

}
/********************************************************************************
Description:
	for caculate window
parameters: 
	FrameCount:        need delay frame counter
Return:
     	void
Remarks:
*********************************************************************************/
void VIM_MAPI_Delay_Frame(UINT8 FrameCount)
{
UINT16 iStartFram,iCurrentFrame,x;
UINT32 i=350*FrameCount;
	if(i==0) i=350;

	iStartFram=VIM_JPEG_GetFrameCount();
	while(i--)
	{
		iCurrentFrame=VIM_JPEG_GetFrameCount();
		x=iCurrentFrame-iStartFram;
		if(x>FrameCount)
			break;
		VIM_USER_DelayMs(1);
	}
	//VIM_HIF_SetIntModuleEn(VM_HIF_INT_SIF,ENABLE);
	//VIM_SIF_ISP_RegisterISR(VIM_INT_SIF_FRAMEEND,VIM_MAPI_DebugFrame);
	//VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_FRAMEEND,ENABLE);
	//VIM_HIF_SetIntModuleEn(VM_HIF_INT_JPEG,ENABLE);
	//VIM_JPEG_RegisterISR(VIM_INT_JPEG_ENC_DONE,VIM_MAPI_DebugJpegFrame);
	//VIM_JPEG_SetIntEnable(VIM_INT_JPEG_ENC_DONE,ENABLE);

	//VIM_HIF_SetIntModuleEn(VM_HIF_INT_LCDC,ENABLE);
	//VIM_DISP_RegisterISR(VIM_INT_DISP_ALL,VIM_MAPI_DebugDispFrame);
	//VIM_DISP_SetIntEnable(VIM_INT_DISP_ALL,ENABLE);
}
/********************************************************************************
Description:
	for display or decode function to start marb and jpeg module
parameters: 
	void
Return:
     	void
Remarks:
*********************************************************************************/
void VIM_MAPI_SetDisplayJpegMode(void)
{
	VIM_SIF_EnableSyncGen(DISABLE);
    VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
    //  VIM_USER_DelayMs(100); 
    VIM_MAPI_SetChipMode(VIM_MARB_DISPLAYSTILL_MODE,VIM_IPP_HAVE_NOFRAME);
    // 	VIM_USER_DelayMs(100); 
    VIM_IPP_SetDisplayDropFrame(0xffff);
}

VIM_RESULT VIM_MAPI_SetDecodeJpegMode(VIM_DECODE_MODE decodemode)
{	
	VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,DISABLE);	
	switch(decodemode) 
	{
		case VIM_DECODE_ONLY:
    			VIM_MAPI_SetChipMode(VIM_MARB_DECODEONLY_MODE,VIM_IPP_HAVE_NOFRAME);
   			break;
		case VIM_DECODE_IPP:
			VIM_MAPI_SetChipMode(VIM_MARB_DECODEYUV_MODE,VIM_IPP_HAVE_NOFRAME);
   			break;
		case VIM_DECODE_FRAME:
			VIM_DISP_SetWorkMode(VIM_DISP_DECODE);
			VIM_MAPI_SetChipMode(VIM_MARB_DECODERGB_MODE,VIM_IPP_HAVEFRAME);
			break;
		case VIM_DECODE_BYPASS_IPP:
			break;
		case VIM_DECODE_BYPASS_FRAME:
			break;
		default:
			break;
	}
	VIM_IPP_SetDisplayDropFrame(0xffff);
	return VIM_SUCCEED;		
}

/********************************************************************************
Description:
	start decode do
parameters: 
	void
Return:
     	void
Remarks:
*********************************************************************************/

void VIM_MAPI_StartDecode(void)
{

       VIM_MARB_ResetSubModule(VIM_MARB_SUB_RESET_JPEG);
	   
#if(BOARD_TYPE==VC0820_FPGA)//add by zhoujian
		VIM_MARB_SetJpegbufGapratio(5,VIM_MARB_FIFO_0);
#endif
     	VIM_MARB_SetFifoEn(VIM_MARB_FIFO_0,ENABLE);
	//set jpeg each table from jpeg file
       VIM_JPEG_SetJpegInfo(&gVc0578b_JpegInfo);

	VIM_JPEG_StartDecode();	
}
/********************************************************************************
Description:
	set jpeg information to jpeg module and marb map memory
parameters: 
	void
Return:
	VIM_SUCCEED:set parameter ok and map memory ok
Remarks:
*********************************************************************************/
VIM_RESULT VIM_MAPI_SetDisplayJpegParameter(void)
{
	VIM_RESULT result;
	UINT8  pixelrate;
	UINT16 blank;
	

	//set line buffer YUV mode, jpeg module image size, video word counter ect.
	
//#if(BOARD_TYPE==VC0820_FPGA)//add by zhoujian
	//	result = VIM_JPEG_SetSize((VIM_JPEG_YUVMODE)gVc0578b_JpegInfo.YUVType, gVc0578b_JpegInfo.ImageSize);
//#else
	result = VIM_JPEG_SetSize((VIM_JPEG_YUVMODE)gVc0578b_JpegInfo.YUVType, gVc0578b_JpegInfo.JpgSize);
//#endif

	if(result)
			return result;
DISPLAYCHAGNEAMODE:
	result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,gVc0578b_Info.MarbStatus.ALayerMode,
		                  (PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
	if(result)
	{
		if(gVc0578b_Info.MarbStatus.ALayerMode == VIM_DISP_LINEBUF)
			return result;
		else
		{
			gVc0578b_Info.MarbStatus.ALayerMode =VIM_DISP_LINEBUF; 
			goto DISPLAYCHAGNEAMODE;
		}
	}

	//angela added it 2006-8-22 for auto just a mode
	//VIM_JPEG_AdjustClkBlank();
	pixelrate =VIM_JPEG_GetPixRate();
	VIM_JPEG_SetPixelRate(pixelrate);
	blank = VIM_JPEG_GetHbanlk(pixelrate);
	VIM_JPEG_SetLineBlank(blank);

	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	write current frame data to jbuf to decode by jpeg module
parameters: 
	Truelen: the true data length of this frame  
	start	    : the start address for jpeg data write to        
Return:
	void
Remarks:
*********************************************************************************/
void VIM_MAPI_WriteOneFrameData(UINT32 Truelen,HUGE UINT8 *start)
{
 UINT32 marblen, offset=0,Waittimes=0xfff,templen;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
 UINT8 JpegStatus=0;
#endif

	VIM_MARB_SetJpegbufGapratio(8,VIM_MARB_FIFO_0);
	marblen=gVc0578b_Info.MarbStatus.MapList.jbufsize;	
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("[VC0578B][MID]  JbufSize 	 : ", marblen);
		VIM_USER_PrintHex("[VC0578B][MID]  JpegFileSize : ", Truelen);
#endif

	while(Waittimes--)
	{
		if((VIM_MARB_GetDeStatus()&&Truelen))	//have decode more than 9/10 jbuf clean by less than ?
		{
			templen=VIM_MARB_GetJbufRWSize();
			if(templen>=Truelen)
			{
				if(!gVc0578b_JpegInfo.frmEnd)	//agnela 2007-2-25
					VIM_MARB_WriteJpegData(start+offset, Truelen,0);
				else
					VIM_MARB_WriteJpegData(start+offset,Truelen,1);
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
				VIM_USER_PrintHex("[VC0578B][MID] end of write : ", Truelen);
#endif
				Truelen=0;	
			}
			else
			{
				VIM_MARB_WriteJpegData(start+offset, templen,0);
				offset+=templen;
				Truelen-=templen;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
				//VIM_USER_PrintHex("\n middle of write : ", templen);
#endif
			}
		}
		VIM_USER_DelayMs(5);
		//file is not end then break waite next time call for jpeg display
		if((!gVc0578b_JpegInfo.frmEnd)&&(!Truelen))
		  	break;
		//not check LCDC frame end because dislay size is not 16 
		if((gVc0578b_JpegInfo.frmEnd)&&(!Truelen))
		{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
				JpegStatus=VIM_JPEG_GetStatus();
				VIM_USER_PrintHex("\n Jpegstatus : ", JpegStatus);
				if(JpegStatus&0x2)
			   		break;	
#else
				if(VIM_JPEG_GetStatus()&0x2)
			   		break;	
#endif
		}
	}
}

/********************************************************************************
Description:
	write current frame data to jbuf to decode by jpeg module
parameters: 
	Truelen: the true data length of this frame  
	start	    : the start address for jpeg data write to        
Return:
	void
Remarks:
*********************************************************************************/
UINT32 VIM_MAPI_WriteOneBlockData(UINT32 Truelen,HUGE UINT8 *start)
{
 UINT32 marblen,templen;

	marblen=gVc0578b_Info.MarbStatus.MapList.jbufsize/2;	

	templen=VIM_MARB_GetMp4RWSize();
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		//VIM_USER_PrintHex("[VC0578B][MID]  JbufSize 	 : ", marblen);
		//VIM_USER_PrintHex("[VC0578B][MID]  templen : ", templen);
#endif
	
	if(!templen||!Truelen)
		return 0;

	if(templen>=Truelen)
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintHex("[VC0578B][MID] Truelen  is =",Truelen);
#endif
		VIM_MARB_WriteMp4Data(start, Truelen,0);
	}
	else
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	//VIM_USER_PrintHex("[VC0578B][MID] Truelen  is =",templen);
#endif
		VIM_MARB_WriteMp4Data(start, templen,0);
		Truelen=templen;
	}
	gVc0578b_Info.DisplayStatus.FileLength+=Truelen;
	return Truelen;
}
/********************************************************************************
  Description:
	check user define display size is good or not 
  Parameters  :
	LcdOffsetX,LcdOffsetY:user define start position on LCD
	DisplayWidth,DisplayHeight:user define display size on LCD
	DisplaySize:get the real display size (if a have rotation)
  Return:
  	VIM_SUCCEED	: ok
  	VIM_ERROR_PARAMETER:startposition+displaysize>LCD size
  Remarks: 
*********************************************************************************/
VIM_RESULT VIM_MAPI_CheckDisplaySize(UINT16 LcdOffsetX, UINT16 LcdOffsetY, UINT16 DisplayWidth, UINT16 DisplayHeight, TSize *DisplaySize)
{

	if(gVc0578b_Info.DisplayStatus.outputtype == VIM_OUTPUTLCD)
	{	
		TSize Size,lcddis;
		TPoint pt = {0, 0};
		//check display size>{4,4},must less than (LCD size)
	    Size.cx=(DisplayWidth&0xfffe);	
		Size.cy=(DisplayHeight&0xfffe);	
		if((Size.cx<4)||(Size.cy<4))
		   	return VIM_ERROR_PARAMETER;
		//check display if large than lcd size
		pt.x=(LcdOffsetX&0xfffc);
		pt.y=LcdOffsetY;	

		lcddis=gVc0578b_Info.LcdStatus.Size;
		if (gVc0578b_Info.LcdStatus.ARotationMode%2)
		{
			lcddis.cx=gVc0578b_Info.LcdStatus.Size.cy;	
			lcddis.cy=gVc0578b_Info.LcdStatus.Size.cx;	
		}
		if(((pt.x+Size.cx)>lcddis.cx)||((pt.y+Size.cy)>lcddis.cy))
		   	return VIM_ERROR_PARAMETER;
		DisplaySize->cx=Size.cx;	
		DisplaySize->cy=Size.cy;	
		gVc0578b_Info.DisplayStatus.LcdWantPt=pt;
		gVc0578b_Info.DisplayStatus.LcdWantSize=Size;
		return VIM_SUCCEED;
	}
	
	else//TV
	{
#if VIM_USER_SUPPORT_TV
		DisplaySize->cx = (DisplayWidth>>1);
		DisplaySize->cx <<= 1;

		DisplaySize->cy = (DisplayHeight>>1);
		DisplaySize->cy <<= 1;

		
		return VIM_TVDC_VerifyJpegSize(DisplayWidth,DisplayHeight);
#endif
	}
	return VIM_SUCCEED;
}
/********************************************************************************
  Description:
	check display size if overflow by downsize
  Parameters  :
	bYUVmode:jpeg YUV mode
	SourceSize:jpeg source size
	DisplaySize:decode target size
  Return:
  	VIM_SUCCEED	: ok
  	VIM_ERROR_DECODE_DOWNSIZEOVERFLOW:420 DisplaySize.cy/SourceSize.cy<1/16
  	                                                                   422 DisplaySize.cy/SourceSize.cy<1/8
  	                                                                   444 DisplaySize.cy/SourceSize.cy<1/8
  	                                                                   410 DisplaySize.cy/SourceSize.cy<1/8
  	                                                                   400 DisplaySize.cy/SourceSize.cy<1/8
  Remarks: 
        this function add in 2006/2/24, because 578 hardware hace bug so need add this check
  Modify History:
	Version			Modifyby				DataTime					modified
	   0.1			Amanda Deng			2006.02.24				create first version

*********************************************************************************/
VIM_RESULT VIM_MAPI_CheckDecodeTargetSize(VIM_JPEG_YUVMODE bYUVmode,TSize SourceSize, TSize DisplaySize)
{
	if(bYUVmode==JPEG_420)
	{
		if (DisplaySize.cy*16<SourceSize.cy)
			return VIM_ERROR_DECODE_DOWNSIZEOVERFLOW;
	}
	else
	{
		if (DisplaySize.cy*8<SourceSize.cy)
			return VIM_ERROR_DECODE_DOWNSIZEOVERFLOW;
	}
	return VIM_SUCCEED;
}
/********************************************************************************
   Description:
	check user define display size is good or not 
  Parameters  :
	JpegOffsetX,JpegOffsetY:user define start position on jpeg imagesize
	DisplaySize:user define display size on LCD
	DestDisplaySize:get the real display size (if displaysize+startposition>jpeg imagesize)
  Return:
  	VIM_SUCCEED	: ok
  	VIM_ERROR_PARAMETER:startposition>jpeg imagesize
  Remarks: 
*********************************************************************************/
VIM_RESULT VIM_MAPI_CheckDisplayPanSize(UINT16 JpegOffsetX, UINT16 JpegOffsetY, TSize DisplaySize,TSize *DestDisplaySize,TPoint* pt )
{
	TSize SourceWin;

	//get source window size and source start position
	SourceWin.cx=DestDisplaySize->cx=DisplaySize.cx;
	SourceWin.cy=DestDisplaySize->cy=DisplaySize.cy;	
	pt->x=JpegOffsetX&0xfffe;
	pt->y=JpegOffsetY&0xfffe;
	if((pt->x>=gVc0578b_JpegInfo.JpgSize.cx)||(pt->y>=gVc0578b_JpegInfo.JpgSize.cy))
	{
		pt->x=pt->y=0;
		return VIM_ERROR_PARAMETER;
	}
	if((pt->x+SourceWin.cx)>gVc0578b_JpegInfo.JpgSize.cx)
	{
		DestDisplaySize->cx=gVc0578b_JpegInfo.JpgSize.cx-pt->x;
	      //return VIM_ERROR_PARAMETER;
	}
	if((pt->y+SourceWin.cy)>gVc0578b_JpegInfo.JpgSize.cy)
	{
		DestDisplaySize->cy=gVc0578b_JpegInfo.JpgSize.cy-pt->y;
	      //return VIM_ERROR_PARAMETER;
	}
	return VIM_SUCCEED;
}
/********************************************************************************
  Description:
	check user define display size is good or not 
  Parameters  :
  	mPt:a layer memory start position
  	MemorySize:a layer memory size
  	DisplaySize:user define display size on LCD
	LcdOffsetX,LcdOffsetY:user define start position on LCD
  Return:
  	VIM_SUCCEED	: ok
  	VIM_ERROR_????????: return from disp module
  Remarks: 
*********************************************************************************/
VIM_RESULT VIM_MAPI_SetALayerDisplaySize(TPoint mPt,TSize MemorySize, TSize DisplaySize, UINT16 LcdOffsetX, UINT16 LcdOffsetY)
{
	VIM_RESULT result;
	TPoint pt = {0, 0};
	TSize lcddis,winsize;

	#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578B][MID]  layer a memory x start : ", mPt.x);
		VIM_USER_PrintDec("[VC0578B][MID]  layer a memory y start : ", mPt.y);
		VIM_USER_PrintDec("[VC0578B][MID]  layer a memory width : ",  MemorySize.cx);
		VIM_USER_PrintDec("[VC0578B][MID]  layer a memory height : ", MemorySize.cy);
	#endif
       //get display size
	result = VIM_DISP_SetA_Memory(mPt,MemorySize);
	if(result)
		return result;
	//set A layer window size and start position in memory panel (window size = display size)
	pt.x=LcdOffsetX;
	pt.y=LcdOffsetY;	
	//because user define display size is after rotation
	winsize=MemorySize;
	lcddis=DisplaySize;
	if (gVc0578b_Info.LcdStatus.ARotationMode%2)
	{
      		winsize.cx=MemorySize.cy;
		winsize.cy=MemorySize.cx;
		lcddis.cx=DisplaySize.cy;
		lcddis.cy=DisplaySize.cx;
	}
	if(winsize.cx<DisplaySize.cx)
	{
		pt.x=(lcddis.cx-winsize.cx)>>1;
		pt.x+=LcdOffsetX;
	}
	if(winsize.cy<lcddis.cy)
	{
		pt.y=(lcddis.cy-winsize.cy)>>1;
		pt.y+=LcdOffsetY;
	}	
	pt.x&=0xfffc;
	pt.y&=0xfffc;
	#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578B][MID]  layer a on lcd x start : ", pt.x);
		VIM_USER_PrintDec("[VC0578B][MID]  layer a on lcd y start : ", pt.y);
		VIM_USER_PrintDec("[VC0578B][MID] layer a window width : ",  winsize.cx);
		VIM_USER_PrintDec("[VC0578B][MID]  layer a window height : ", winsize.cy);
	#endif
	result = VIM_DISP_SetA_DisplayPanel(pt,winsize);
	if(result)
		return result;
	return VIM_SUCCEED;
}

/********************************************************************************
  Description:
	get jpeg information 
  Parameters  :
	jpegBuf:	jpeg file data start address
	length: jpeg file length
  Return:
  	VIM_SUCCEED	: ok, can auto decode
  	VIM_ERROR_PARAMETER:capture file length more than jbuf size
  	VIM_ERROR_YUV_UNKNOW: jpeg file yuv format error
  	VIM_ERROR_NOPIC_INBUF: jbuffer have no pic end flag
  Remarks: Jpeg data must be captured/encoded by v578 chip
*********************************************************************************/
VIM_RESULT VIM_MAPI_GetJpegInformation(HUGE UINT8 *jpegBuf,UINT32 length)
{

	if(jpegBuf)	
	{
		VIM_JPEG_ToolJpegParse(jpegBuf, length, &gVc0578b_JpegInfo,1);
		if((VIM_JPEG_YUVMODE)gVc0578b_JpegInfo.YUVType == JPEG_UNKNOWN) 
			return VIM_ERROR_YUV_UNKNOW;
		if(gVc0578b_JpegInfo.frmEnd ==TRUE)
			gVc0578b_Info.DisplayStatus.FileLength+=gVc0578b_JpegInfo.eop;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("[VC0578B][MID]  gVc0578b_Info.DisplayStatus.FileLength=",gVc0578b_Info.DisplayStatus.FileLength);
#endif
	}
	else
	{
		return VIM_ERROR_DISPLAY_NULL;
	}
	return VIM_SUCCEED;
}
/********************************************************************************
  Description:
	判断想要更新的区域是否在ram屏幕范围内
	
  Parameters  :
	size:	大小
	Point: 起始点
  Return:
  返回几个点在范围内
*********************************************************************************/


UINT8 VIM_MAPI_FindUpdateRange(TSize size, TPoint Point)
{
UINT8 InCon=0;
	if((Point.x>=gVc0578b_Info.PreviewStatus.Point.x)&&
		(Point.x<=(gVc0578b_Info.PreviewStatus.Point.x+
		gVc0578b_Info.PreviewStatus.Size.cx)))
	{
		if((Point.y>=gVc0578b_Info.PreviewStatus.Point.y)&&
		(Point.y<=(gVc0578b_Info.PreviewStatus.Point.y+
		gVc0578b_Info.PreviewStatus.Size.cy)))
			InCon++;
	}
	if(((Point.x+size.cx)>=gVc0578b_Info.PreviewStatus.Point.x)&&
		((Point.x+size.cx)<=(gVc0578b_Info.PreviewStatus.Point.x+
		gVc0578b_Info.PreviewStatus.Size.cx)))
	{
		if((Point.y>=gVc0578b_Info.PreviewStatus.Point.y)&&
		(Point.y<=(gVc0578b_Info.PreviewStatus.Point.y+
		gVc0578b_Info.PreviewStatus.Size.cy)))
			InCon++;
	}
	if(((Point.x+size.cx)>=gVc0578b_Info.PreviewStatus.Point.x)&&
		((Point.x+size.cx)<=(gVc0578b_Info.PreviewStatus.Point.x+
		gVc0578b_Info.PreviewStatus.Size.cx)))
	{
		if(((Point.y+size.cy)>=gVc0578b_Info.PreviewStatus.Point.y)&&
		((Point.y+size.cy)<=(gVc0578b_Info.PreviewStatus.Point.y+
		gVc0578b_Info.PreviewStatus.Size.cy)))
			InCon++;
	}
	if((Point.x>=gVc0578b_Info.PreviewStatus.Point.x)&&
		(Point.x<=(gVc0578b_Info.PreviewStatus.Point.x+
		gVc0578b_Info.PreviewStatus.Size.cx)))
	{
		if(((Point.y+size.cy)>=gVc0578b_Info.PreviewStatus.Point.y)&&
		((Point.y+size.cy)<=(gVc0578b_Info.PreviewStatus.Point.y+
		gVc0578b_Info.PreviewStatus.Size.cy)))
			InCon++;
	}
	return InCon;
}

/********************************************************************************
Description:
	adjust point because of rotaion of lcd 
Parameters:
	UINT8 step:
		the step 

Remarks:
	state: 	valid
*********************************************************************************/

VIM_RESULT VIM_MAPI_AdjustPoint(VIM_DISP_ROTATEMODE byARotateMode,VIM_DISP_ROTATEMODE byBRotateMode)
{
TPoint	Point,Point1;
TSize Size,Size1;

	if(byARotateMode > VIM_DISP_NOTCHANGE || byBRotateMode > VIM_DISP_NOTCHANGE)
		return VIM_ERROR_DISP_PARAM;

	if(VIM_DISP_NOTCHANGE!=byARotateMode)
		{
			Point.x=VIM_HIF_GetReg16(V5_REG_LCDC_A0_LCD_POS+2);
			Point.y=VIM_HIF_GetReg16(V5_REG_LCDC_A0_LCD_POS);
			VIM_DISP_GetAWinsize(&Size);
			if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_DISPLAYMPG4_MODE||!VIM_USER_MPEG4_USE_MAXALAYER)
			{
				Point1.x=VIM_HIF_GetReg16(V5_REG_LCDC_A1_LCD_POS+2);
				Point1.y=VIM_HIF_GetReg16(V5_REG_LCDC_A1_LCD_POS);			
				VIM_DISP_GetA1Winsize(&Size1);
			}
			switch(byARotateMode)
			{
				case VIM_DISP_ROTATE_90:
				case VIM_DISP_MIRROR_90:
					VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS+2,Point.y);
					Point.y=gVc0578b_Info.LcdStatus.Size.cy-Point.x-Size.cx;
					if(Point.y>gVc0578b_Info.LcdStatus.Size.cy)
						return VIM_ERROR_ADJUST_OFFSET;
					VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS,Point.y);

					if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_DISPLAYMPG4_MODE||!VIM_USER_MPEG4_USE_MAXALAYER)
					{
						VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS+2,Point1.y);
						Point1.y=gVc0578b_Info.LcdStatus.Size.cy-Point1.x-Size1.cx;
						if(Point1.y>gVc0578b_Info.LcdStatus.Size.cy)
							return VIM_ERROR_ADJUST_OFFSET;
						VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS,Point1.y);					
					}
					break;
				case VIM_DISP_ROTATE_180:
				case VIM_DISP_MIRROR_180:
					Point.x=gVc0578b_Info.LcdStatus.Size.cx-Point.x-Size.cx;
					Point.y=gVc0578b_Info.LcdStatus.Size.cy-Point.y-Size.cy;
					if(Point.x>gVc0578b_Info.LcdStatus.Size.cx)
						return VIM_ERROR_ADJUST_OFFSET;
					if(Point.y>gVc0578b_Info.LcdStatus.Size.cy)
						return VIM_ERROR_ADJUST_OFFSET;
					VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS+2,Point.x);
					VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS,Point.y);


					if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_DISPLAYMPG4_MODE||!VIM_USER_MPEG4_USE_MAXALAYER)
					{
						Point1.x=gVc0578b_Info.LcdStatus.Size.cx-Point1.x-Size1.cx;
						Point1.y=gVc0578b_Info.LcdStatus.Size.cy-Point1.y-Size1.cy;
						if(Point1.x>gVc0578b_Info.LcdStatus.Size.cx)
							return VIM_ERROR_ADJUST_OFFSET;
						if(Point1.y>gVc0578b_Info.LcdStatus.Size.cy)
							return VIM_ERROR_ADJUST_OFFSET;
						VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS+2,Point1.x);
						VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS,Point1.y);					
					}
					break;
				case VIM_DISP_ROTATE_270:
				case VIM_DISP_MIRROR_270:
					VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS,Point.x);
					Point.x=gVc0578b_Info.LcdStatus.Size.cx-Point.y-Size.cy;
					if(Point.x>gVc0578b_Info.LcdStatus.Size.cx)
						return VIM_ERROR_ADJUST_OFFSET;
					VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS+2,Point.x);
	
					if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_DISPLAYMPG4_MODE||!VIM_USER_MPEG4_USE_MAXALAYER)
					{
						VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS,Point1.x);
						Point1.x=gVc0578b_Info.LcdStatus.Size.cx-Point1.y-Size1.cy;
						if(Point1.x>gVc0578b_Info.LcdStatus.Size.cx)
							return VIM_ERROR_ADJUST_OFFSET;
						VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS+2,Point1.x);				
					}
					break;
				default:
					break;
			}
		}
	Point.x=VIM_HIF_GetReg16(V5_REG_LCDC_A0_LCD_POS+2)&0xfffe;  //angela 2006-8-10
	Point.y=VIM_HIF_GetReg16(V5_REG_LCDC_A0_LCD_POS)&0xfffe;
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS+2,Point.x);
	VIM_HIF_SetReg16(V5_REG_LCDC_A0_LCD_POS,Point.y);

	if(gVc0578b_Info.MarbStatus.WorkMode!=VIM_MARB_DISPLAYMPG4_MODE||!VIM_USER_MPEG4_USE_MAXALAYER)
	{
		Point1.x=VIM_HIF_GetReg16(V5_REG_LCDC_A1_LCD_POS+2)&0xfffe;  //angela 2006-8-10
		Point1.y=VIM_HIF_GetReg16(V5_REG_LCDC_A1_LCD_POS)&0xfffe;
		VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS+2,Point1.x);
		VIM_HIF_SetReg16(V5_REG_LCDC_A1_LCD_POS,Point1.y);
	}

	if(VIM_DISP_NOTCHANGE==byBRotateMode)
		return VIM_SUCCEED;
	Point.x=VIM_HIF_GetReg16(V5_REG_LCDC_B0_LCD_POS+2);
	Point.y=VIM_HIF_GetReg16(V5_REG_LCDC_B0_LCD_POS);
	VIM_DISP_GetBSize(VIM_DISP_B0LAYER,&Size);

	
	Point1.x=VIM_HIF_GetReg16(V5_REG_LCDC_B1_LCD_POS+2);
	Point1.y=VIM_HIF_GetReg16(V5_REG_LCDC_B1_LCD_POS);
	VIM_DISP_GetBSize(VIM_DISP_B1LAYER,&Size1);


	switch(byBRotateMode)
	{
		case VIM_DISP_ROTATE_90:
		case VIM_DISP_MIRROR_90:
			VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS+2,Point.y);
			VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS+2,Point1.y);
			Point.y=gVc0578b_Info.LcdStatus.Size.cy-Point.x-Size.cx;
			Point1.y=gVc0578b_Info.LcdStatus.Size.cy-Point1.x-Size1.cx;
			if(Point.y>gVc0578b_Info.LcdStatus.Size.cy)
				return VIM_ERROR_ADJUST_OFFSET;
			if(Point1.y>gVc0578b_Info.LcdStatus.Size.cy)
				return VIM_ERROR_ADJUST_OFFSET;
			VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS,Point.y);
			VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS,Point1.y);
			break;
		case VIM_DISP_ROTATE_180:
		case VIM_DISP_MIRROR_180:
			Point.x=gVc0578b_Info.LcdStatus.Size.cx-Point.x-Size.cx;
			Point1.x=gVc0578b_Info.LcdStatus.Size.cx-Point1.x-Size1.cx;
			Point.y=gVc0578b_Info.LcdStatus.Size.cy-Point.y-Size.cy;
			Point1.y=gVc0578b_Info.LcdStatus.Size.cy-Point1.y-Size1.cy;
			if(Point.x>gVc0578b_Info.LcdStatus.Size.cx)
				return VIM_ERROR_ADJUST_OFFSET;
			if(Point1.x>gVc0578b_Info.LcdStatus.Size.cx)
				return VIM_ERROR_ADJUST_OFFSET;
			if(Point.y>gVc0578b_Info.LcdStatus.Size.cy)
				return VIM_ERROR_ADJUST_OFFSET;
			if(Point1.y>gVc0578b_Info.LcdStatus.Size.cy)
				return VIM_ERROR_ADJUST_OFFSET;
			VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS+2,Point.x);
			VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS+2,Point1.x);
			VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS,Point.y);
			VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS,Point1.y);
			break;
		case VIM_DISP_ROTATE_270:
		case VIM_DISP_MIRROR_270:
			VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS,Point.x);
			VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS,Point1.x);
			Point.x=gVc0578b_Info.LcdStatus.Size.cx-Point.y-Size.cy;
			Point1.x=gVc0578b_Info.LcdStatus.Size.cx-Point1.y-Size1.cy;
			if(Point.x>gVc0578b_Info.LcdStatus.Size.cx)
				return VIM_ERROR_ADJUST_OFFSET;
			if(Point1.x>gVc0578b_Info.LcdStatus.Size.cx)
				return VIM_ERROR_ADJUST_OFFSET;
			VIM_HIF_SetReg16(V5_REG_LCDC_B0_LCD_POS+2,Point.x);
			VIM_HIF_SetReg16(V5_REG_LCDC_B1_LCD_POS+2,Point1.x);
			break;
		default:
			break;
	}
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	adjust point because of rotaion of lcd 
Parameters:
	UINT8 step:
		the step 

Remarks:
	state: 	valid
*********************************************************************************/

VIM_RESULT VIM_MAPI_GetNewPoint(VIM_DISP_ROTATEMODE byRotateMode,PTPoint OldPoint,TSize OldSize,PTPoint NewPoint)
{
	switch(byRotateMode)
	{
		case VIM_DISP_ROTATE_90:
		case VIM_DISP_MIRROR_90:
			NewPoint->x=OldPoint->y;
			NewPoint->y=gVc0578b_Info.LcdStatus.Size.cy-OldPoint->x-OldSize.cx;
			if(NewPoint->y>gVc0578b_Info.LcdStatus.Size.cy)
				return VIM_ERROR_ADJUST_OFFSET;
			break;
		case VIM_DISP_ROTATE_180:
		case VIM_DISP_MIRROR_180:
			NewPoint->x=gVc0578b_Info.LcdStatus.Size.cx-OldPoint->x-OldSize.cx;
			NewPoint->y=gVc0578b_Info.LcdStatus.Size.cy-OldPoint->y-OldSize.cy;
			if(NewPoint->x>gVc0578b_Info.LcdStatus.Size.cx)
				return VIM_ERROR_ADJUST_OFFSET;
			if(NewPoint->y>gVc0578b_Info.LcdStatus.Size.cy)
				return VIM_ERROR_ADJUST_OFFSET;
			break;
		case VIM_DISP_ROTATE_270:
		case VIM_DISP_MIRROR_270:
			NewPoint->y=OldPoint->x;
			NewPoint->x=gVc0578b_Info.LcdStatus.Size.cx-OldPoint->y-OldSize.cy;
			break;
		default:
			NewPoint->x=OldPoint->x;
			NewPoint->y=OldPoint->y;
			break;

	}
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	disable or enable syncgen ,and delay one frame.
Parameters:

Remarks:
	state: 	valid
*********************************************************************************/

void VIM_MAPI_EnableSyncGen(UINT8 bEnable)
{
UINT32 i=50;
UINT16 iCurrentFrame,iStartFram;
	iStartFram=VIM_JPEG_GetFrameCount();
	VIM_SIF_EnableSyncGen(bEnable);
	if(bEnable==DISABLE)
	{
		while(i--)
		{
			iCurrentFrame=VIM_JPEG_GetFrameCount();
			if((iCurrentFrame-iStartFram)>1)
				break;
			VIM_USER_DelayMs(1);
		}
	}
}

/********************************************************************************
Description:
	set zoom (can capture)
Parameters:
	UINT8 step:
		the step 

Remarks:
	state: 	valid
*********************************************************************************/
VIM_RESULT VIM_MAPI_Preview_CaptureZoom(UINT8 bstep)
{
	UINT32 dwZoomW,dwZoomH;
	TSize CaptureSize,ZoomSourcSize = gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize,ZoomBaseSize,InputSize;
	TPoint pt;
	VIM_RESULT result;


	CaptureSize.cx=gVc0578b_Info.CaptureStatus.Size.cx;
	CaptureSize.cy=gVc0578b_Info.CaptureStatus.Size.cy;
	ZoomBaseSize.cx=ZoomSourcSize.cx;
	ZoomBaseSize.cy=ZoomSourcSize.cy;
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
	if(bstep)
	{
		if((CaptureSize.cx/2) > gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cx)
		{
			ZoomBaseSize.cx = CaptureSize.cx/2;
			ZoomBaseSize.cy = CaptureSize.cy/2;
		}
		else
			ZoomBaseSize = gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize;
#if (VIM_USER_ZOOM_MIN_WIDHT)
			{
				if (ZoomBaseSize.cx < VIM_USER_ZOOM_MIN_WIDHT)
				{
					ZoomBaseSize.cy = VIM_USER_ZOOM_MIN_WIDHT *  ZoomBaseSize.cy / ZoomBaseSize.cx;
					ZoomBaseSize.cx = VIM_USER_ZOOM_MIN_WIDHT;
				}
			}
#endif
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
		if((ZoomSourcSize.cx*ZoomBaseSize.cy)>=(ZoomBaseSize.cx*ZoomSourcSize.cy))
		{
			dwZoomW =(UINT32) (bstep *(ZoomSourcSize.cx -ZoomBaseSize.cx)/VIM_USER_MAX_STEP+0.5);
			dwZoomH =(UINT32)(bstep *(ZoomSourcSize.cx -ZoomBaseSize.cx)* ZoomSourcSize.cy/(VIM_USER_MAX_STEP*ZoomSourcSize.cx)+0.5);

		}
		else
		{
			dwZoomH = (bstep * (ZoomSourcSize.cy-ZoomBaseSize.cy) / VIM_USER_MAX_STEP)&0xfffe;
			dwZoomW = dwZoomH* ZoomSourcSize.cx /ZoomSourcSize.cy;
		}
		ZoomSourcSize.cx -= dwZoomW;
		ZoomSourcSize.cy -= dwZoomH;
		//ZoomSourcSize.cx &= 0xfffe;
		//ZoomSourcSize.cy &= 0xfffe;
#else	
		if(((ZoomSourcSize.cx/ZoomSourcSize.cy)>(ZoomBaseSize.cx/ZoomBaseSize.cy))||(((ZoomSourcSize.cx/ZoomSourcSize.cy)
			==(ZoomBaseSize.cx/ZoomBaseSize.cy))&&((ZoomSourcSize.cx%ZoomSourcSize.cy)>=(ZoomBaseSize.cx%ZoomBaseSize.cy))))
		{
			dwZoomW = (bstep * (ZoomSourcSize.cx -ZoomBaseSize.cx))  / VIM_USER_MAX_STEP;
			dwZoomH = (dwZoomW * ZoomSourcSize.cy)  / ZoomSourcSize.cx;

		}
		else if(((ZoomSourcSize.cx/ZoomSourcSize.cy)<(ZoomBaseSize.cx/ZoomBaseSize.cy))||(((ZoomSourcSize.cx/ZoomSourcSize.cy)
			==(ZoomBaseSize.cx/ZoomBaseSize.cy))&&((ZoomSourcSize.cx%ZoomSourcSize.cy)<(ZoomBaseSize.cx%ZoomBaseSize.cy))))
		{
			dwZoomH = (bstep * (ZoomSourcSize.cy-ZoomBaseSize.cy))  / VIM_USER_MAX_STEP;
			dwZoomW = (dwZoomH * ZoomSourcSize.cx)  / ZoomSourcSize.cy;
		}
		else
			return VIM_ERROR_PREVIEW_BASE_ZOOM_PARA;
		ZoomSourcSize.cx -= dwZoomW;
		ZoomSourcSize.cy -= dwZoomH;
		ZoomSourcSize.cx &= 0xfffe;
		ZoomSourcSize.cy &= 0xfffe;
#endif		
		if ((ZoomSourcSize.cx < CaptureSize.cx/2)||(ZoomSourcSize.cx < gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cx)
			||((ZoomSourcSize.cy < CaptureSize.cy/2)
			   ||(ZoomSourcSize.cy < gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cy)))
		{
			ZoomSourcSize.cx=ZoomBaseSize.cx;
		}

		
		result = VIM_IPP_ToolCaculateLessSrcWindow(ZoomSourcSize,CaptureSize,&ZoomBaseSize);
		if(result)
			return result;
		if((ZoomBaseSize.cx<gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cx)||
			(ZoomBaseSize.cy<gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cy))
		{
			ZoomBaseSize=gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize;
		}
	}
	VIM_SIF_EnableSyncGen(DISABLE);
	
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID]  zoom capture ,the source x is",ZoomBaseSize.cx);
	VIM_USER_PrintDec("[VC0578B][MID]  zoom capture ,the source y is",ZoomBaseSize.cy);
#endif
	VIM_IPP_GetImageSize(&InputSize.cx,&InputSize.cy);
	pt.x=((InputSize.cx-ZoomBaseSize.cx)/2)&0xfffe;
	pt.y=((InputSize.cy-ZoomBaseSize.cy)/2)&0xfffe;
	

	result=VIM_IPP_SetCaptureSize(gVc0578b_Info.pSensorInfo,pt, ZoomBaseSize, CaptureSize);
	if(result)
		return result;
	result=VIM_IPP_SetDispalySize(pt,ZoomBaseSize, gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize);
	if(result)
		return result;
	pt.x=(((gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cx-gVc0578b_Info.PreviewStatus.Size.cx)>>1)>>1)<<1;
	pt.y=(gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cy-gVc0578b_Info.PreviewStatus.Size.cy)>>1;
	VIM_DISP_SetA_Memory(pt, gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize);	
	VIM_SIF_EnableSyncGen(ENABLE);

	return VIM_SUCCEED;

}

/********************************************************************************
Description:
	set zoom (can not capture)
Parameters:
	UINT8 step:
		the step 

Remarks:
	state: 	valid
*********************************************************************************/

VIM_RESULT VIM_MAPI_Preview_DisplayZoom(UINT8 bstep)
{
#if VIM_USER_SURPPORT_DISPLAYZOOM
UINT32 dwZoomW,dwZoomH;
TSize ZoomSourcSize = gVc0578b_Info.PreviewStatus.ZoomPara.InitSourceSize,ZoomBaseSize;
TSize AmSize=gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize;
TPoint pt;
VIM_RESULT result;

	if(bstep)
	{
#if (VIM_USER_MAX_AMEM_WIDTH)
		{
			if ((ZoomSourcSize.cx > VIM_USER_MAX_AMEM_WIDTH)&&
				(gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cx<VIM_USER_MAX_AMEM_WIDTH)) 
			{
				ZoomBaseSize.cx = VIM_USER_MAX_AMEM_WIDTH;
				ZoomBaseSize.cy = ZoomBaseSize.cx * ZoomSourcSize.cy / ZoomSourcSize.cx;
			}
			else
				ZoomBaseSize=ZoomSourcSize;
		}
#else
		{
			if (ZoomSourcSize.cx > AM_MAXPIXEL) 
			{
				ZoomBaseSize.cx = AM_MAXPIXEL;
				ZoomBaseSize.cy = ZoomBaseSize.cx * ZoomSourcSize.cy / ZoomSourcSize.cx;
			}
			else
				ZoomBaseSize=ZoomSourcSize;
		}
#endif
//revised by youhai 2008.1.4	
#if (BOARD_TYPE==VC0820_FPGA)
		dwZoomW =(UINT32)(bstep * (ZoomBaseSize.cx - gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cx)/ VIM_USER_MAX_STEP+0.5);
		dwZoomH =(UINT32)(bstep * (ZoomBaseSize.cx - gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cx) * ZoomSourcSize.cy/(VIM_USER_MAX_STEP*ZoomSourcSize.cx)+0.5);

		AmSize.cx += dwZoomW;
		AmSize.cy += dwZoomH;
		AmSize.cx &= 0xfff0;
		AmSize.cy &= 0xfff0;
		if ((ZoomSourcSize.cx < AmSize.cx) ||(ZoomSourcSize.cy < AmSize.cy)
		    || (AmSize.cx > AM_MAXPIXEL) ||(AmSize.cy > AM_MAXPIXEL))
		{
			return VIM_ERROR_PREVIEW_DISPLAY_ZOOM_PARA;
		}

#else
		dwZoomW = (bstep * (ZoomBaseSize.cx - gVc0578b_Info.PreviewStatus.ZoomPara.InitAMemSize.cx))  / VIM_USER_MAX_STEP;
		dwZoomH = (dwZoomW * ZoomSourcSize.cy)  / ZoomSourcSize.cx;

		AmSize.cx += dwZoomW;
		AmSize.cy += dwZoomH;
		AmSize.cx &= 0xfff0;
		AmSize.cy &= 0xfff0;
		if ((ZoomSourcSize.cx < AmSize.cx) ||(ZoomSourcSize.cy < AmSize.cy)
		    || (AmSize.cx > AM_MAXPIXEL) ||(AmSize.cy > AM_MAXPIXEL))
		{
			return VIM_ERROR_PREVIEW_DISPLAY_ZOOM_PARA;
		}
#endif		
	}

	VIM_SIF_EnableSyncGen(DISABLE);
	VIM_USER_DelayMs(10);
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
	VIM_USER_PrintDec("[VC0578B][MID]  zoom display  ,the AmSize x is",AmSize.cx);
	VIM_USER_PrintDec("[VC0578B][MID]  zoom display ,the AmSize y is",AmSize.cy);
#endif
	VIM_IPP_GetWindowPos(&pt.x,&pt.y);
	result=VIM_IPP_SetDispalySize(pt,ZoomSourcSize, AmSize);
	if(result)
		return result;
	pt.x=(((AmSize.cx-gVc0578b_Info.PreviewStatus.Size.cx)>>1)>>1)<<1;
	pt.y=(AmSize.cy-gVc0578b_Info.PreviewStatus.Size.cy)>>1;
	VIM_DISP_SetA_Memory(pt,AmSize);
	VIM_SIF_EnableSyncGen(ENABLE);
#endif
	return VIM_SUCCEED;
}


//#endif

/********************************************************************************
Description:
	change the input size of 
Parameters:
	UINT8 step:
		the step 

Remarks:
	state: 	valid
*********************************************************************************/
extern UINT8 last_environment_mode;

VIM_RESULT VIM_MAPI_FastPreview(void)
{

      VIM_RESULT result;
	TSize InputSize,CaptureSize,SourceWin;
	TPoint pt;
	//UINT32 stime,etime;

	

	//UINT8 qf=0xff;
	/*UINT32 et;
	UINT8 Ggain;
	UINT8 awb[3];*/
	
	VIM_SIF_RESOLUTION NowIndex=(VIM_SIF_RESOLUTION)gVc0578b_Info.PreviewStatus.bOutputIndex;
	
	if( VIM_USER_FAST_PREVIEW)
	{
		/*if(gVc0578b_Info.pSensorInfo->snrGetEt )
		{
			gVc0578b_Info.pSensorInfo->snrGetEt(gVc0578b_Info.PreviewStatus.bOutputIndex,&et,&Ggain,awb);
		}*/
		result = VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, NowIndex, &InputSize);
		if(((gVc0578b_Info.CaptureStatus.Size.cx)>InputSize.cx)||((gVc0578b_Info.CaptureStatus.Size.cy)>InputSize.cy))
		{
    			while(((gVc0578b_Info.CaptureStatus.Size.cx)>InputSize.cx)||((gVc0578b_Info.CaptureStatus.Size.cy)>InputSize.cy))
    			{
				//stime=MMD_SYS_GetTimer2Value();
				VIM_JPEG_SetBitRateControlEn(DISABLE);
    				//gVc0578b_Info.CaptureStatus.SensorOutputChange=1;
    				//??没有效果
    				//qf=VIM_JPEG_GetQFactor();
    				 if((NowIndex)==VIM_SIF_HALFHALFPIXEL)
    				{
    					NowIndex=VIM_SIF_HALFPIXEL;
    					result = VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, NowIndex, &InputSize);
    				}
    				else if((NowIndex)==VIM_SIF_HALFPIXEL)
    				{
    					NowIndex=VIM_SIF_MAXPIXEL;
    					result = VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, NowIndex, &InputSize);
    				}
    				else
    					break;
    				
    				 if(gVc0578b_Info.PreviewStatus.Mode==VIM_HAPI_PREVIEW_FRAMEON)
    					return VIM_ERROR_FASET_PREVIEW;
    
    
    				VIM_IPP_GetCaptureWindowSize(&CaptureSize);
    				VIM_IPP_GetWindowSize(&SourceWin.cx,&SourceWin.cy);
    				
    			//	VIM_IPP_SetCapSyncGen(DISABLE);
    				VIM_SIF_EnableSyncGen(DISABLE);
    				
    				result = VIM_SIF_SetSensorResolution(gVc0578b_Info.pSensorInfo,&gVc0578b_Info.VIM_SIF_AeAdjust,NowIndex,VIM_SIF_SNSOR_FRAMERATE_FORCAPIMAGE);
    				if(result)
    				       return result;
                    if(last_environment_mode==ENVIRONMENT_NIGHT)
                    {
                        if(SensorIdChk_OV2650_YUV()==SUCCEED)
                            SensorSetReg_OV2650_YUV(0x3011,0x02);//0x05
                    }
    				result = VIM_SIF_GetSensorResolution(gVc0578b_Info.pSensorInfo, NowIndex, &InputSize);
    				if(result)
    				       return result;
    				VIM_IPP_SetImageSize(InputSize.cx, InputSize.cy);
    				
    				//get source window size
    				SourceWin.cx<<=1;
    				SourceWin.cy<<=1;
    				CaptureSize.cx=gVc0578b_Info.CaptureStatus.Size.cx;
				CaptureSize.cy=gVc0578b_Info.CaptureStatus.Size.cy;
    				// 1280x960->1280x1024  angela 2006-8-8 for can not capture
    	
    				pt.x=((InputSize.cx-SourceWin.cx)/2)&0xfffe;
    				pt.y=((InputSize.cy-SourceWin.cy)/2)&0xfffe;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
    				VIM_USER_PrintDec("[VC0578B][MID]  fast preview ipp resize ,the SourceWin x is",SourceWin.cx);
    				VIM_USER_PrintDec("[VC0578B][MID]  fast preview ipp resize ,the SourceWin y is",SourceWin.cy);
#endif
    				result = VIM_IPP_SetCaptureSize(gVc0578b_Info.pSensorInfo,pt, SourceWin, CaptureSize); 
    				if(result)
    					return result;
    				VIM_JPEG_SetSize(JPEG_422,CaptureSize);
    				VIM_MAPI_SetCaptureRatio(gVc0578b_Info.CaptureStatus.QualityMode);

				//etime=MMD_SYS_GetTimer2Value();
				//printf("VIM_MAPI_FastPreview()  1 use %dms\n",(etime-stime)/1000);
    				// angela added it for set capture ratio before preview on
    				/*result = VIM_IPP_SetThumbSize((UINT8)gVc0578b_Info.CaptureStatus.ThumbSize.cx, (UINT8)gVc0578b_Info.CaptureStatus.ThumbSize.cy,VIM_IPP_THUMB_FROM_CAPTURE);
    				if(result)
    					return result;*/
    				if((gVc0578b_Info.CaptureStatus.ThumbSize.cx*gVc0578b_Info.CaptureStatus.ThumbSize.cy)>0)
    				{
    					result = VIM_IPP_SetThumbSize((UINT8)gVc0578b_Info.CaptureStatus.ThumbSize.cx, (UINT8)gVc0578b_Info.CaptureStatus.ThumbSize.cy,VIM_IPP_THUMB_FROM_CAPTURE);
					if(result)
						return result;
    				}
    				
				result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_NODISP,
				                  (PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
    				if(result)
    					return result;
    				VIM_JPEG_SetBitRateControlEn(ENABLE);
    				VIM_IPP_SetDisplayDropFrame(0xffff);
    				VIM_SIF_EnableSyncGen(ENABLE);	
    				/*if(gVc0578b_Info.pSensorInfo->snrSetEt && gVc0578b_Info.PreviewStatus.bOutputIndex != VIM_SIF_MAXPIXEL)
    				{
    					gVc0578b_Info.pSensorInfo->snrSetEt(NowIndex,et,Ggain,awb);
    				}*/
    				//VIM_JPEG_SetBrcQF(qf);
    				//stime=MMD_SYS_GetTimer2Value();
    				VIM_MAPI_Delay_Frame(1);   //Delay for the stable sensor data output
    				//etime=MMD_SYS_GetTimer2Value();
				//printf("VIM_MAPI_FastPreview()  2 use %dms\n",(etime-stime)/1000);
    			}
		}
		else
		{
			VIM_MAPI_Delay_Frame(1); //zhangxiang add 090313
			if(gVc0578b_Info.CaptureStatus.SizeChange==1)
			{
				//gVc0578b_Info.CaptureStatus.SensorOutputChange=1;
				gVc0578b_Info.CaptureStatus.SizeChange=0;
				VIM_JPEG_SetBitRateControlEn(DISABLE);
				//VIM_IPP_GetWindowSize(&SourceWin.cx,&SourceWin.cy);
                     		
                     	//	VIM_IPP_SetCapSyncGen(DISABLE);
                     	VIM_SIF_EnableSyncGen(DISABLE);
                     		
                     	CaptureSize.cx=gVc0578b_Info.CaptureStatus.Size.cx;
				CaptureSize.cy=gVc0578b_Info.CaptureStatus.Size.cy;
                     	// 1280x960->1280x1024  angela 2006-8-8 for can not capture
                     	//get source window size
#if 0
				result = VIM_IPP_ToolCaculateLessSrcWindow(InputSize,CaptureSize,&SourceWin);
				if(result)
					return (UINT16)result;
#else
				SourceWin.cx = InputSize.cx;
				SourceWin.cy = InputSize.cy;
#endif
                     	result = VIM_IPP_ToolCaculateLessSrcWindow(InputSize,CaptureSize,&SourceWin);
				if(result)
					return (UINT16)result;
                     
                     	pt.x=((InputSize.cx-SourceWin.cx)/2)&0xfffe;
                     	pt.y=((InputSize.cy-SourceWin.cy)/2)&0xfffe;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
                     	VIM_USER_PrintDec("[VC0578B][MID]  fast preview ipp resize ,the SourceWin x is",SourceWin.cx);
                     	VIM_USER_PrintDec("[VC0578B][MID]  fast preview ipp resize ,the SourceWin y is",SourceWin.cy);
#endif
                     	result = VIM_IPP_SetCaptureSize(gVc0578b_Info.pSensorInfo,pt, SourceWin, CaptureSize); 
                     	if(result)
                     		return result;
                     	VIM_JPEG_SetSize(JPEG_422,CaptureSize);
                     	VIM_MAPI_SetCaptureRatio(gVc0578b_Info.CaptureStatus.QualityMode);
				if((gVc0578b_Info.CaptureStatus.ThumbSize.cx*gVc0578b_Info.CaptureStatus.ThumbSize.cy)>0)
    				{
    					result = VIM_IPP_SetThumbSize((UINT8)gVc0578b_Info.CaptureStatus.ThumbSize.cx, (UINT8)gVc0578b_Info.CaptureStatus.ThumbSize.cy,VIM_IPP_THUMB_FROM_CAPTURE);
					if(result)
						return result;
    				}
                     		                		
                     	result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_NODISP,
                     		                  (PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
                     	if(result)
                     		return result;
                     	VIM_JPEG_SetBitRateControlEn(ENABLE);
                     	VIM_IPP_SetDisplayDropFrame(0xffff);
                     	VIM_SIF_EnableSyncGen(ENABLE);	
                     		
                     	VIM_MAPI_Delay_Frame(1);   //Delay for the stable sensor data output
			}
			else
			{
                VIM_JPEG_SetBitRateControlEn(ENABLE); //zhangxiang add 090313
				VIM_SIF_EnableSyncGen(DISABLE);
		
				result =VIM_MARB_SetMap(gVc0578b_Info.MarbStatus.WorkMode,VIM_DISP_NODISP,
						(PVIM_MARB_Map)&gVc0578b_Info.MarbStatus.MapList);
			
				if(result)
					return result;	
				VIM_SIF_EnableSyncGen(ENABLE);	
				VIM_MAPI_Delay_Frame(1); 				
			}
			
		}
	}
	return VIM_SUCCEED;
}





/********************************************************************************
Description:
	AE isr
Parameters:
	NULL
Remarks:
	NULL
*********************************************************************************/
void VIM_SIF_IspAutoExposeISR(void)
{
 	static UINT8 valid = 0;
 	// If the position that setting et is not in ae window,
 	// the value should be set at next frame beginning,
 	// so gain delay should add 1 to assure gain and et effect in same frame	
 	if(valid < (gVc0578b_Info.pSensorInfo->aeparm.gdelay+1))	
 	{
 		valid++;
		gVc0578b_Info.aeaf.count=0;
 		return;
 	}
	//revised by youhai_mp4enc 2007.8.30	
#if (BOARD_TYPE==VC0820_FPGA)
#else
	VIM_SIF_IspAutoExpose(&gVc0578b_Info.VIM_SIF_AeAdjust,gVc0578b_Info.pSensorInfo); 	
#endif
 	valid = 0;
	gVc0578b_Info.aeaf.count=0;
}



#if VIM_SENSOR_AUTO_FOCUS
/********************************************************************************
  Description:
	adjust focus before capture
  Parameters:
	NULL
  Remarks:
	this fun will not return except af finished or timeout
*********************************************************************************/
void VIM_MAPI_IspDoAFAtOnetime(void)
{
	UINT32 Waittme=0x1000;
	UINT8 temp;
	gVc0578b_Info.aeaf.Bafover = FALSE;

	//过程：开AF中断，关AE中断
	VIM_ISP_SetAutoIntFreq(0);//计算公式为2^n+1;
	VIM_SIF_ISP_RegisterISR(VIM_INT_SIF_YMEAN,VIM_SIF_IspAutoExposeISR);
	VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_YMEAN,DISABLE);

	//enable auto focus.
	temp=VIM_HIF_GetReg8(V5_REG_ISP_AUTOCTRL+1);
	VIM_HIF_SetReg8(V5_REG_ISP_AUTOCTRL+1, temp|(1<<5));

//	VIM_USER_SetAutoFocusStep(1);
	VIM_MAPI_Delay_Frame(4);
	
	VIM_SIF_ISP_RegisterISR(VIM_INT_SIF_AUTOFOC,VIM_MAPI_AutoFocusISR);
	VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_AUTOFOC,ENABLE);

	
	//等AF做完
	while(Waittme--)
	{
		if(gVc0578b_Info.aeaf.Bafover == TRUE)
			break;
		VIM_USER_DelayMs(2);
			
	}	
	//关AF,开AE
	//VIM_ISP_SetAutoIntFreq(0);//计算公式为2^n+1;
	VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_AUTOFOC,DISABLE);

	//enable ae.
	//temp=VIM_HIF_GetReg8(V5_REG_ISP_AUTOCTRL+1);
	//VIM_HIF_SetReg8(V5_REG_ISP_AUTOCTRL+1, temp&(~(1<<5)));

	//VIM_SIF_ISP_RegisterISR(VIM_INT_SIF_YMEAN,VIM_SIF_IspAutoExposeISR);		
	//VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_YMEAN,ENABLE);
	
	VIM_MAPI_Delay_Frame(3);		//angela disable it 2007-9-10
	return;
}

/********************************************************************************
Description:
	AF isr
Parameters:
	NULL
Remarks:
	This af isr can be used both in the state machine fun(do af during preview) 
	and Do VIM_SIF_IspDoAFAtOnetime before capture
*********************************************************************************/
void VIM_MAPI_AutoFocusISR(void)
{
	UINT8 rightstep;

	static UINT8 count=0;

	count ++;	
	if(count<2)
	{
		return;
	}
	count=0;

	
//>>>>>>>>>>>
	//rightstep = VIM_SIF_IspAutoFocus();
//<<<<<<<<<<<
	rightstep = VIM_SIF_IspAutoFocus(& gVc0578b_Info.aeaf.rightstep);
	if(rightstep!= 0)//AF finished
	{
		gVc0578b_Info.aeaf.Bafover = TRUE;
		gVc0578b_Info.aeaf.Baeafsel=4;

		gVc0578b_Info.aeaf.rightstep = rightstep;

		VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_AUTOFOC,DISABLE);
	}
	return;

}



//使用状态机在AE,AF,IDLE状态切换.
/********************************************************************************
Description:
	state machine fun,The main idea is that  after ae finished,wait for a while,then do af,
	after af finished,wait for a while,then do ae....................
Parameters:
	NULL
Remarks:
	this fun is called in a timer
	if gVc0578b_Info.Baeafen is set,it willed called,otherwise It can't be called
*********************************************************************************/

void VIM_MAPI_DoAeAF(void)
{
	UINT8 ret=0,temp;
	static UINT8 count=0,count1=0;

	switch(gVc0578b_Info.aeaf.Baeafsel)
	{
		case 0://idle
	//revised by youhai_mp4enc 2007.8.30	
#if (BOARD_TYPE==VC0820_FPGA)
#else		
			ret = VIM_SIF_IspAutoExpose(&gVc0578b_Info.VIM_SIF_AeAdjust,gVc0578b_Info.pSensorInfo);
#endif
			if(ret == FALSE)
				gVc0578b_Info.aeaf.Baeafsel=1;
		
			break;
			
		case 1://doing AE
			VIM_SIF_ISP_RegisterISR(VIM_INT_SIF_YMEAN,VIM_SIF_IspAutoExposeISR);		
			VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_YMEAN,ENABLE);

			count1++;
			gVc0578b_Info.aeaf.count++;
			if(gVc0578b_Info.aeaf.count >= 15
				||count1>=30//很长时间没有调整过来。
				)
			{
				gVc0578b_Info.aeaf.Baeafsel=3;
				gVc0578b_Info.aeaf.count=0;

				count1=0;
			}
			break;
			
		case 2://doing AF 
			VIM_SIF_ISP_RegisterISR(VIM_INT_SIF_AUTOFOC,VIM_MAPI_AutoFocusISR);
			VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_AUTOFOC,ENABLE);
			break;
			
		case 3:
			if(count==0)
			{
				//stop ae int
				VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_YMEAN,DISABLE);

				//enable auto focus.
				temp=VIM_HIF_GetReg8(V5_REG_ISP_AUTOCTRL+1);
				VIM_HIF_SetReg8(V5_REG_ISP_AUTOCTRL+1, temp|(1<<5));	

//				VIM_USER_SetAutoFocusStep(1);
			}
			count++;
			
			if(count>=5)
			{
				gVc0578b_Info.aeaf.Baeafsel=2;
				count = 0;
			}

			break;
		case 4:
			if(count==0)
			{
				//stop af int
				VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_AUTOFOC,DISABLE);

				//enable ae.
				temp=VIM_HIF_GetReg8(V5_REG_ISP_AUTOCTRL+1);
				VIM_HIF_SetReg8(V5_REG_ISP_AUTOCTRL+1, temp&(~(1<<5)));				
			}
			count++;			
			if(count>=5)
			{
				gVc0578b_Info.aeaf.Baeafsel=0;
				count =0;
			}
			
			break; 

		default:
			break;
	}
	
}

#endif


#if VIM_SENSOR_HAVE_BAYER
/********************************************************************************
Description:
	open ae enable module,register isr fun,and open ae int
Parameters:
	NULL
Return:
	NULL
Remarks:
	this function will be called in VIM_HAPI_SetPreviewMode if HAVE_BAYER defined
*********************************************************************************/
void VIM_MAPI_AEIsrInit(void)
{
	VIM_HIF_ClearIntModule(VM_HIF_INT_ISP);
	VIM_ISP_SetAutoIntFreq(0);//计算公式为2^n+1;
	//enable ae.
	VIM_SIF_IspEnableFocus(DISABLE);
		
	VIM_SIF_ISP_RegisterISR(VIM_INT_SIF_YMEAN,VIM_SIF_IspAutoExposeISR);
	VIM_SIF_ISP_SetIntEnable(VIM_INT_SIF_YMEAN,ENABLE);				
}
#endif


#if VIM_USER_SUPPORT_TV
BOOL VIM_MAPI_BMP_TESTOK(UINT8 *bmp)
{
	UINT16 bfType;
	UINT16 biBitCount;

	bfType=( bmp[0]|(bmp[1]<<8) );
	if(bfType != 0x4d42)
		return FALSE;

	biBitCount = (bmp[28]|(bmp[29]<<8));
	if(biBitCount != 0x08 &&biBitCount != 0x04 && biBitCount != 0x02)
		return FALSE;
		
	return TRUE;
}




/*
裁减，TV输出时先裁减后缩放
*/
TSize VIM_MAPI_CutoffIPP(TSize Size_Jiang,TSize DisplaySize,TPoint *pt1)
{

	TPoint pt={0,0};

	if(Size_Jiang.cx > DisplaySize.cx && Size_Jiang.cy > DisplaySize.cy)
	{
		if(Size_Jiang.cy*1.0/Size_Jiang.cx > DisplaySize.cy*1.0/DisplaySize.cx)
		{
			pt.x = 0;
			pt.y = (Size_Jiang.cy-Size_Jiang.cx*DisplaySize.cy/DisplaySize.cx)/2;

			pt.y >>= 1;
			pt.y <<= 1;

			Size_Jiang.cy = Size_Jiang.cx*DisplaySize.cy/DisplaySize.cx;
		}
		else
		{
			pt.y = 0;
			pt.x = (Size_Jiang.cx - Size_Jiang.cy*DisplaySize.cx/DisplaySize.cy)/2;

			pt.x >>= 1;
			pt.x <<= 1;

			Size_Jiang.cx = Size_Jiang.cy*DisplaySize.cx/DisplaySize.cy;
		}
	}

	pt1->x = pt.x;
	pt1->y = pt.y;
	
	return Size_Jiang;
	
}
#endif

/********************************************************************************
Description:
	line buffer 1 data read call back function
Parameters:
Remarks:
state: valid
*********************************************************************************/
#if 0
void VIM_MAPI_ISR_LBufRead(void)
{
	UINT32 vwc;
	TSize newsize,displaysize;

	if(!( VIM_HIF_GetReg8(V5_REG_LBUF_STATUS) & 0x02))
		return;
	
	VIM_DISP_GetAWinsize(&displaysize);
	newsize.cx=displaysize.cx;
	#if 1// for get the regiset value to debug 7/17/2007
	//newsize.cy=VIM_HIF_GetReg16(V5_REG_LBUF_STATUS);
	//newsize.cy>>= 8;
	newsize.cy=VIM_HIF_GetReg8(V5_REG_LBUF_STATUS+1);
	newsize.cy&=0xff;
	#endif

#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("\n ** new  decode line counter : ", newsize.cy);
#endif
	if (!newsize.cy) 
		return;	 
	
	gVc0578b_Info.DisplayStatus.DecodeHeight+=newsize.cy;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("DecodeHeight height:",gVc0578b_Info.DisplayStatus.DecodeHeight); 
		VIM_USER_PrintDec("DecLength :",gVc0578b_Info.DisplayStatus.DecLength);
#endif
	vwc = VIM_JPEG_GetYuvSize(JPEG_422, newsize);

	if(gVc0578b_Info.DisplayStatus.DecLength<vwc)	
	{
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("\n decode buffer length : ", gVc0578b_Info.DisplayStatus.BufLength);
		VIM_USER_PrintHex("\n decode data length   : ", vwc);
		VIM_USER_PrintString("\n decode buf error!");
#endif
		gVc0578b_Info.DisplayStatus.ReadOk=VIM_DECODE_BUFERROR;
		return;
	}
	VIM_HIF_ReadSram(gVc0578b_Info.MarbStatus.MapList.lbuf1Ystart, gVc0578b_Info.DisplayStatus.DecPoint, vwc, VIM_HIF_SRAM_MODE);//7/11/2007 guoying add
	
	gVc0578b_Info.DisplayStatus.DecPoint+=vwc;
	gVc0578b_Info.DisplayStatus.DecLength-=vwc;
	
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("\n decode Height : ", gVc0578b_Info.DisplayStatus.DecodeHeight);
		VIM_USER_PrintHex("\n displaysize.cy : ", displaysize.cy);
#endif


	if (gVc0578b_Info.DisplayStatus.DecodeHeight < displaysize.cy)
		VIM_JPEG_RestartDecode();
	else 
		gVc0578b_Info.DisplayStatus.ReadOk=VIM_DECODE_DONE;
	
}



/********************************************************************************
Description:
	just decode one frame! at romsave mode,this function don't responsible for read file,This is controled by hig_api function
parameters: 
	Truelen: the true data length for decode
Return:
	VIM_SUCCEED:
	VIM_ERROR_TIME_OUT:
	VIM_ERROR_BUFFERLENGTHS:
Remarks:
*********************************************************************************/
VIM_RESULT VIM_MAPI_DecodeOneFrame(UINT32 Truelen,HUGE UINT8 *start)
{
	UINT32 marblen, offset=0;
	UINT32 Waittimes=0x3ff,Waittimes1=0x3fff;
	UINT32 Lenwrite;
	
	VIM_MARB_SetJpegbufGapratio(8,VIM_MARB_FIFO_0);	//7/16/2007, guoying add
	marblen=gVc0578b_Info.MarbStatus.MapList.jbufsize;	
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintHex("\n JbufSize 	 : ", marblen);
		VIM_USER_PrintHex("\n JpegFileSize : ", Truelen);
		VIM_USER_PrintDec("\n current file length 	 : ", gVc0578b_Info.DisplayStatus.FileLength);
#endif

	while(Waittimes--)
	{
		gVc0578b_Info.DisplayStatus.ReadOk=VIM_DECODE_ING;
		if(Truelen < marblen)
			Lenwrite = Truelen;
		else
			Lenwrite = marblen;
		
		if(marblen>Truelen)
		{
			if(gVc0578b_JpegInfo.frmEnd)
				VIM_MARB_WriteJpegData(start+offset,Lenwrite,1);
			else
				VIM_MARB_WriteJpegData(start+offset,Lenwrite,0);
		}
		else
		{
			VIM_MARB_WriteJpegData(start+offset,Lenwrite,0);
		}
		
		Truelen -= Lenwrite;
		offset+=marblen;

		VIM_USER_DelayMs(5);


		while((Waittimes1--)&&(gVc0578b_Info.DisplayStatus.ReadOk==VIM_DECODE_ING))
		{
			VIM_MAPI_ISR_LBufRead();
			VIM_USER_DelayMs(10); 
			
			if((VIM_MARB_GetJbufRWSize()==marblen)&&(Truelen))
				gVc0578b_Info.DisplayStatus.ReadOk=VIM_DECODE_SHOULDREWRITE;

			if((! gVc0578b_JpegInfo.frmEnd) && Truelen == 0)//not last block in file
			{
				if(VIM_MARB_GetJbufRWSize()==marblen //all data in jpeg buffer has been send to ipp
					/*&& VIM_HIF_GetReg8(V5_REG_LBUF_STATUS+1)==0*/)//can not read any data in LBUF buffer1
					gVc0578b_Info.DisplayStatus.ReadOk=VIM_DECODE_READFILE;
			}
		}
		
		if((Waittimes1==0)&&(!Truelen))
			return VIM_ERROR_TIME_OUT;
		Waittimes1=0x3fff;
		if(gVc0578b_Info.DisplayStatus.ReadOk==VIM_DECODE_DONE)
			break;
		if(gVc0578b_Info.DisplayStatus.ReadOk==VIM_DECODE_READFILE)
			break;
		if(gVc0578b_Info.DisplayStatus.ReadOk==VIM_DECODE_BUFERROR)
			return VIM_ERROR_BUFFERLENGTHS;
		
		VIM_USER_DelayMs(1);
	}
	return VIM_SUCCEED;
}

/********************************************************************************
Description:
	ready to decode
Parameters:

Remarks:
	state: 	valid
*********************************************************************************/
VIM_RESULT VIM_MAPI_ReadyToDecode(VIM_HAPI_SAVE_MODE SaveMode,HUGE void *jpegBuf, UINT32 length,UINT16 DisplayWidth, UINT16 DisplayHeight)
{
	VIM_RESULT result;
	UINT32 readCnt;
	TSize DisplaySize,DestDisplaySize;
	TPoint pt;
	DisplayWidth=(DisplayWidth&0xfffe);	
	DisplayHeight=(DisplayHeight&0xfffe);

	gVc0578b_Info.PreviewStatus.Mode=VIM_HAPI_PREVIEW_OFF;	//*

	gVc0578b_Info.DisplayStatus.SaveMode=SaveMode;
	gVc0578b_Info.DisplayStatus.FileLength=0;
	gVc0578b_Info.DisplayStatus.DecodeHeight=0;		//*
	if(SaveMode==VIM_HAPI_RAM_SAVE)
	{
		gVc0578b_Info.DisplayStatus.BufPoint=jpegBuf;
		gVc0578b_Info.DisplayStatus.BufLength=length;
		readCnt=length;
	}
	else
	{
		gVc0578b_Info.DisplayStatus.pFileHandle=jpegBuf;
		gVc0578b_Info.DisplayStatus.BufOffset=0;
		gVc0578b_Info.DisplayStatus.BufLength=VIM_USER_BUFLENGTH;
		length=VIM_USER_BUFLENGTH;
		gVc0578b_Info.DisplayStatus.MallocPr=gVc0578b_Info.DisplayStatus.BufPoint=VIM_USER_MallocMemory(VIM_USER_BUFLENGTH);
		if(gVc0578b_Info.DisplayStatus.BufPoint==NULL)
			return VIM_ERROR_BUFPOINT;
		//read file from ROM to user define RAM
		result = VIM_USER_SeekFile(gVc0578b_Info.DisplayStatus.pFileHandle,0);
		if(result)
			goto DECODEERROR;
		readCnt = VIM_USER_ReadFile(gVc0578b_Info.DisplayStatus.pFileHandle,  gVc0578b_Info.DisplayStatus.BufPoint, VIM_USER_BUFLENGTH);
	}


	//disable syncgen
	VIM_SIF_EnableSyncGen(DISABLE);		//angela 2006-8-15
	
	//get jpeg information( YVU mode, image size and other table parameters
	result = VIM_MAPI_GetJpegInformation(gVc0578b_Info.DisplayStatus.BufPoint,readCnt);
	if(result)
		goto DECODEERROR;
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)
		VIM_USER_PrintDec("\n jpeg Image width:  ",gVc0578b_JpegInfo.ImageSize.cx);
		VIM_USER_PrintDec("\n jpeg Image height:  ",gVc0578b_JpegInfo.ImageSize.cy);
		VIM_USER_PrintDec("\n yuv is:  ",gVc0578b_JpegInfo.YUVType);
#endif		
	// set decode jpeg mode
	result = VIM_MAPI_SetDecodeJpegMode();
	if(result)
		goto DECODEERROR;	
	// caculate the target
	DisplaySize.cx=DisplayWidth;
	DisplaySize.cy=DisplayHeight;	
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




//result = VIM_IPP_SetCaptureSize(0,pt, gVc0578b_JpegInfo.ImageSize, DestDisplaySize);
	
	result = VIM_IPP_SetDispalySize(pt, gVc0578b_JpegInfo.JpgSize, DestDisplaySize);//*different from Display Jpeg mode
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
		

		if(DestDisplaySize.cx>DisplaySize.cx)	
			DestDisplaySize.cx=DisplaySize.cx;
		if(DestDisplaySize.cy>DisplaySize.cy)	
			DestDisplaySize.cy=DisplaySize.cy;

		VIM_DISP_SetA_DisplayPanel(pt,DestDisplaySize);

	
	result = VIM_MAPI_AdjustPoint((VIM_DISP_ROTATEMODE)gVc0578b_Info.LcdStatus.ARotationMode,
					VIM_DISP_NOTCHANGE);
	if(result)
			goto DECODEERROR;

	gVc0578b_Info.MarbStatus.ALayerMode = VIM_DISP_LINEBUF;
	//set jpeg module parameter and marb map memory
	if(gVc0578b_Info.DisplayStatus.BufPoint)
	{
		result = VIM_MAPI_SetDisplayJpegParameter();
	}
	else
		result=VIM_ERROR_DECODE_ERROR;


	//ready start do decode
	VIM_MAPI_StartDecode();			//7/12/2007, guoying add
DECODEERROR:
	if(result)
	{
		if(SaveMode==VIM_HAPI_ROM_SAVE)
		{
			VIM_USER_FreeMemory(gVc0578b_Info.DisplayStatus.MallocPr);
			gVc0578b_Info.CaptureStatus.MallocPr=0;
		}
		return result;
	}
	return VIM_SUCCEED;
}
#endif
/********************************************************************************
Description:
	caculate the max a size and the a layer mode
Parameters:
	SourceSize: the mpeg4 size
	TargetSize: the target size
	*Size: get the wanted size
	 *Amode :get the  the wanted a mode
	Return:
		VIM_SUCCEED: set mode ok
		VIM_ERROR_PARAMETER: can not support this value
	Remarks:
		state: valid
*********************************************************************************/
VIM_RESULT VIM_MAPI_GetAmodeInMpeg4(TSize SourceSize,TSize TargetSize,PTSize Size,VIM_DISP_BUFFERMODER *Amode)
{
	UINT32 AmemLen,div;
	UINT32 AneedLen;
	TSize divsize;
		AmemLen=SourceSize.cx*SourceSize.cy*3;//mpeg dec needed it
		AmemLen+=SourceSize.cx*16*2;// two y line buf 
		AmemLen+=SourceSize.cx*SourceSize.cy/2;// two u v  line buf 		
		
		AmemLen=V5_1TSRAM_TOTALSIZE-AmemLen;

		AneedLen=TargetSize.cx*TargetSize.cy*2;
		if(AmemLen>(AneedLen*2))
		{
			Size->cx=TargetSize.cx;
			Size->cy=TargetSize.cy;	
			*Amode=VIM_DISP_TWOFRAME;
			return VIM_SUCCEED;
		}
		else
		{
			if(TargetSize.cx>TargetSize.cy)
				div=TargetSize.cy;
			else
				div=TargetSize.cx;
			if(!div)
				return VIM_ERROR_CACAULATELE_SOURCE_WINDOW;
			while((TargetSize.cx%div)||(TargetSize.cy%div))
			{
				div--;
				if(!div)
					return VIM_ERROR_CACAULATELE_SOURCE_WINDOW;
			}
			Size->cx=divsize.cx=TargetSize.cx/div;
			Size->cy=divsize.cy=TargetSize.cy/div;
			div=1;
			while((Size->cx*Size->cy*2*2)<AmemLen)
			{
				Size->cx+=divsize.cx;
				Size->cy+=divsize.cy;
			}
			if(VIM_IPP_GetCaputreSizeSelect()==VIM_IPP_CAPMODE_UP)	
			{
				*Amode=VIM_DISP_TWOFRAME;
				return VIM_SUCCEED;
			}
			else
			{
				//Size->cx=SourceSize.cx;
				//Size->cy=SourceSize.cy;		
				Size->cx=TargetSize.cx;
				Size->cy=TargetSize.cy;		
				*Amode=VIM_DISP_ONEFRAME;
				return VIM_SUCCEED;				
			}
		}
}


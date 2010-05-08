/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_USER_Info.h
 * [Description]        : 适用于578b.所有客户需要定义的参数
 *				    和平台相关的东西都在这里定义。
 *                       	   
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-10 16:01
 * [Platform]               : 平台相关
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

#ifndef _VIM_5XRDK_USER_INFO_H_
#define _VIM_5XRDK_USER_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __NEW_SYS__ // 820 MP chip macro, by lizg
#include <stdio.h>
#include <string.h>
#endif


#define SVBOARD			1
#define FPGA				2
#define DEMO			3
#define SV762			4
#define NEWDEMO		5
#define ARM9DEMO		6
#define ARM9OLDDEMO	7
#define VC0820_FPGA		10	// by lizg

#ifdef __NEW_SYS__ // 820 MP chip macro, by lizg, VC0820_FPGA		
#define BOARD_TYPE	VC0820_FPGA 
#else
#define BOARD_TYPE	ARM9OLDDEMO
#endif
#define DRV_USE_CACHEBUF	
/********************************************************************************
*     定义了稳定的工作时钟
* 	有tv的时候大约在378m.没有tv可以跑到405m
**********************************************************************************/
#if ((BOARD_TYPE==SVBOARD)||(BOARD_TYPE==NEWDEMO)||(BOARD_TYPE==ARM9DEMO))
#define	VIM_PLLOUT		378000//378000//405000	//k hz 378000
#else
#define	VIM_PLLOUT		100000//405000	//k hz 378000
#endif
/********************************************************************************
*     this define select the bus type multi 16 or multi8  or locosto
* 	VIM_MULTI8    VIM_MULTI16 VIM_LOCOSTO
**********************************************************************************/

#if (BOARD_TYPE==ARM9DEMO||(BOARD_TYPE==ARM9OLDDEMO))
#define VIM_BUSTYPE		VIM_HIFAMR9
#elif(BOARD_TYPE==VC0820_FPGA)
#define VIM_BUSTYPE		VIM_MULTI16	
#else
#define VIM_BUSTYPE		VIM_MULTI16	
#endif

/********************************************************************************
*    定义了一些bypass里面用的参数
* 	VIM_MULTI8    VIM_MULTI16 VIM_LOCOSTO
**********************************************************************************/
/*切换bypass的方法
VIM_BYPASS_USE_REG:用寄存器方式切换
VIM_BYPASS_USE_GPIO 用GPIO方式切换*/
#define VIM_BYPASSMODE		VIM_BYPASS_USE_GPIO	//wendy guo 4/5/2007 VIM_BYPASS_USE_REG	

/*在bypass的模式下，切换主副屏幕的方法
VIM_BYPASS_USEREGISTER_SUBPANEL:用寄存器方式切换主副屏幕
VIM_BYPASS_USECS2_SUBPANEL 用两个片选切换两个屏幕
VIM_BYPASS_USEGPIO_SUBPANEL:用gpio切换两个屏幕*/
#define VIM_BYPASS_SUBPANEL	VIM_BYPASS_USEGPIO_SUBPANEL

/*在寄存器切换bypass的方式下，为了不影响屏幕，
这里定义了设置bypass的寄存器地址*/
#define VIM_BYPASS_ADDR	V5_REG_CPM_GLOBAL_POWDN

/*在寄存器切换bypass的方式下，为了不影响屏幕，
这里定义了设置bypass的寄存器值，最后三位必须为0 
且必须保证最后三位为任意值都不会影响屏幕*/
#define VIM_BYPASS_VALUE	0

/*是否使用中断腿来做外部中断处理中断处理
1 是使用。0是不使用，仅仅使用timer来做*/
#define VIM_EXTERN_INTERRUPT	0

/********************************************************************************
*     this define select the endian
* 	VIM_LITTLEENDIAN    VIM_BIGENDIAN
**********************************************************************************/
#define VIM_ENDIANTYPE		VIM_LITTLEENDIAN  

/********************************************************************************
*     this define for special infineon platform
* 	if other platform ,please do not define it
**********************************************************************************/
#ifndef __INFINEON_PLATFORM__
#define __INFINEON_PLATFORM__		0
#endif
/********************************************************************************
*    when access to v5x ,please define this address
**********************************************************************************/
//  CS address
#define VIM_REG_INDEX		0x8000000	// vc0528 a8(rs) is 0,means address 
#define VIM_REG_VALUE		0x8000300    // vc0528 a8(rs) is 1,means value 

/********************************************************************************
*    Write read IO control by GPIO
*    If user uses Address bus to switch them, need not implement them
**********************************************************************************/
#define VIM_SET_RSLOW_BY_GPIO() //(rBASEBAND_PDATG &= ~BIT2)
#define VIM_SET_RSHIGH_BY_GPIO() //(rBASEBAND_PDATG |= BIT2)


/********************************************************************************
*    Used for GPIO switching bypass and normal modes
*   If user does not use GPIO to switch bypass and normal, does not implement them.
*	High to bypass, low to normal
**********************************************************************************/
#if ((BOARD_TYPE==DEMO)||(BOARD_TYPE==NEWDEMO))
#define VIM_SET_BYPASS_BY_GPIO()		((*(volatile unsigned *)0x1d2002c) |=0x01)	//((*(volatile unsigned *)0x1d20044) |=0x40)			//b7=1 Bypass mode
#define VIM_SET_NORMAL_BY_GPIO()		((*(volatile unsigned *)0x1d2002c) &=0xFE)			//b7=0 Normal mode	//((*(volatile unsigned *)0x1d20044) &=0xbf)			//b7=0 Normal mode
#elif (BOARD_TYPE==ARM9DEMO||(BOARD_TYPE==ARM9OLDDEMO))
#define VIM_SET_BYPASS_BY_GPIO()		((*(volatile unsigned *)0x60170010) |=0xf0)			//b7=1 Bypass mode
#define VIM_SET_NORMAL_BY_GPIO()		((*(volatile unsigned *)0x60170010) &=0xFFFFFF0F)				//b7=0 Normal mode	//((*(volatile unsigned *)0x1d20044) &=0xbf)			//b7=0 Normal mode
#elif(BOARD_TYPE==VC0820_FPGA)
#define VIM_SET_BYPASS_BY_GPIO()		
#define VIM_SET_NORMAL_BY_GPIO()		
#else
#define VIM_SET_BYPASS_BY_GPIO()		((*(volatile unsigned *)0x1d20044) |=0x40)			//b7=1 Bypass mode
#define VIM_SET_NORMAL_BY_GPIO()		((*(volatile unsigned *)0x1d20044) &=0xbf)				//b7=0 Normal mode	//((*(volatile unsigned *)0x1d20044) &=0xbf)			//b7=0 Normal mode

#endif

#if BOARD_TYPE==DEMO
#define VIM_READREG_DELAYFOR		50		
#define VIM_READRAM_DELAYFOR		100		
#else
#define VIM_READREG_DELAYFOR		20	
#define VIM_READRAM_DELAYFOR		20
#endif
/********************************************************************************
*     When switch to bypass mode, disable core LDO for saving power
**********************************************************************************/
#define VIM_DISABLE_LDO_FORCORE()
#define VIM_ENABLE_LDO_FORCORE()


/********************************************************************************
*     当gpio选择主副屏幕的时候选择主屏和副屏
**********************************************************************************/
#define VIM_CHANGETO_MAINPANEL()
#define VIM_CHANGETO_SUBPANEL()


/********************************************************************************
*     When switch to bypass mode, disable base band to vimicro 5X's clock for saving power
**********************************************************************************/
#define VIM_SET_XCLKOFF()
#define VIM_SET_XCLKON()

/********************************************************************************
*    Select driver ic
**********************************************************************************/
#if(CHIP_ID==1)
	#define V5_MAINDRIVERIC_TYPE 	V5_DEF_LCD_ILI9320_16BIT_320x240
#else
	#define V5_MAINDRIVERIC_TYPE 	V5_DEF_LCD_ILI9320_16BIT_240x320
#endif

#define V5_SUBDRIVERIC_TYPE 	NOSUB

#define V5_BYPASS_DATABUS		VIM_HIF_LCD_0_15


/********************************************************************************
*  select SDRAM	
**********************************************************************************/
#define 	HYB39S128160CT_7				1



/********************************************************************************
*  use for software define
**********************************************************************************/



//select	sensor output size   
extern UINT8 FastPreview;
/*-----------------for preview ---------------*/
#define VIM_USER_PREVIEW_ALAYER_MODE  VIM_DISP_ONEFRAME //VIM_DISP_LINEBUF// To make view size almost the same to different capture size
#define VIM_USER_MP4_PREVIEW_ALAYER_MODE  VIM_DISP_LINEBUF//VIM_DISP_ONEFRAME // To make view size almost the same to different capture size
#define VIM_USER_RGBSENSOR_TIMER		200

#define VIM_USER_FAST_PREVIEW 1  // To make view size almost the same to different capture size

#define VIM_USER_ZOOM_MIN_WIDHT 0  // To make view size almost the same to different capture size
							 // If don't limit, set it to 0
							 // 320 is the zoom source size widht of 640x480
#define VIM_USER_MAX_AMEM_WIDTH	0	// To make zoom has the same view space
#define VIM_USER_SURPPORT_DISPLAYZOOM	0	// To make zoom has the same view space
#define VIM_USER_MAX_STEP		5		//means the max step of zoom
#define VIM_USER_MIN_PIXEL_ONESTEP	5	//meas thi limit of min pixel per step
#define	VIM_SENSOR_HAVE_BAYER		0// 0:ONLY SUPPORT YUV  SENSOR		1:SUPPORT YUV &BAYER SENSOR
#define VIM_SENSOR_INIT_FREQ			VIM_EXP_50HZ //VIM_EXP_50HZ,VIM_EXP_60HZ,VIM_EXP_AUTO

#define VIM_SENSOR_AUTO_FOCUS			0 //是否支持auto focus功能，编译开关
#define VIM_SENSOR_AF_MAXSTEP			20 //AUTO FOCUS的最大步数。


/*-----------------for capture ---------------*/
#define VIM_USER_RECAPTURE_OFFSET	0x4000//BYTE ,TWC+VIM_USER_RECAPTURE_OFFSET <JBUF ,THEN RECAPTURE 7
#define VIM_USER_RECAPTUREWITHFRAME_OFFSET	0x2000//BYTE ,TWC+VIM_USER_RECAPTURE_OFFSET <JBUF ,THEN RECAPTURE 7
#define VIM_USER_BUFLENGTH	0x50000//0x1ffff//0x20000  
#define VIM_USER_RECAPTUREMAXTIMES	8
#define VIM_USER_QFONESTEP	20
#define VIM_USER_CAPTURE_TIMER   5 //(MS ) means when capture ,timer interval check done interrupt
/*--------------for display-----------------*/
#define VIM_USER_DISPLAY_USELINEBUF	VIM_DISP_LINEBUF//VIM_DISP_ONEFRAME////	// 0 means use one frame  1 mean s use line buf when display
#define VIM_USER_DISPLAY_FULLSCREEN	0	// 0 means auto find position  1 means will  full fill the area

#define VIM_USER_DISPLAY_ZOOMDIVISOR	20	// display zoom的时候先除的一个值
#define VIM_USER_DISPLAY_ZOOMOFFSETMAXSTEP	5	// display zoom的时候先除的一个值
#define VIM_USER_DISPLAY_ZOOMMINPIXEL	10	// display zoom的时候先除的一个值




/*-----------------for capture mpeg4---------------*/
#define VIM_USER_MPEG4_FRAMEMODE 	0// 表示是否选用line buf的frame 模式,用的话会比较浪费内存
#define VIM_USER_MPEG4_DEC_FRAMEMODE  0// 表示是否选用line buf的frame 模式,用的话会比较浪费内存
#define VIM_USER_MPEG4_VOPMODE 0// 表示是否选用line buf的frame 模式,用的话会比较浪费内存
#define VIDEONORMAL_VOPIN848 0
#define VIDEONORMAL_BLOCKIN848 1
#define VIDEORECORDERIN848 1

#if (BOARD_TYPE==VC0820_FPGA)
#define VIM_USER_MPEG4_BITRATE	0//表示可以调整波特率
#define VIM_USER_MPEG4_FRAMS	1//must be 1 when for 820 一次启动拍多少帧?
#define VIM_USER_MPEG4_GOVNUM	3//一个i帧跟VIM_USER_MPEG4_GOVNUM－1个p帧当VIM_USER_MPEG4_FRAMS>1的时候无效
#else
#define VIM_USER_MPEG4_BITRATE	1// 表示可以调整波特率
#define VIM_USER_MPEG4_FRAMS	1//一次启动拍多少帧?
#define VIM_USER_MPEG4_GOVNUM	2//一个i帧跟VIM_USER_MPEG4_GOVNUM－1个p帧当VIM_USER_MPEG4_FRAMS>1的时候无效
#endif
#define VIM_USER_MPEG4_VYSN	1 // 1 表示用vsyn同步拍摄，这些只在多真拍摄的时候有效
#define VIM_USER_MPEG4_MAXQF	15 // 最大的压缩率
#if (BOARD_TYPE==VC0820_FPGA)
#define VIM_USER_MPEG4_ENC_OUTBUF_NUM	2 //output buffer using pingpong structure
#define VIM_USER_MPEG4_ENC_OUTBUF_FRAMES_NUM	3 //output buffer support how many frames
#else
#endif
#define 	VIM_USER_VIDEOCON_SYN	1	//video conference 的时候。为了同步，解压和压缩放在一起做

/*-----------------for display mpeg4---------------*/
#define VIM_USER_MPEG4_USE_MAXALAYER	1	/*用最大的内存来放MP4只有这样可以再播放的过程中修改显示的大小,也可以旋转*/
#define VIM_USER_MPEG4_USEUPDATEDIRECTLY 1


/*-----------------for tv support ---------------*/
#define VIM_USER_SUPPORT_TV		0

/*-----------------for 578b---------------*/
#if ((BOARD_TYPE==SVBOARD)||(BOARD_TYPE==NEWDEMO)||(BOARD_TYPE==ARM9DEMO))
	#define VIM_BYPASS_RESETCHIP		0
#else
	#define VIM_BYPASS_RESETCHIP		1
#endif


#define 	VIM_USER_SUPPORT_SDRAM		0
#define 	VIM_USER_762_ONLY				0

/*******************end for user modify*****************************************/


//------Infineon------//
#if __INFINEON_PLATFORM__

#define 	HUGE		huge
#define	SHUGE		shuge
#define 	_FAR		_far	

#else

#define 	HUGE
#define	SHUGE
#define	_FAR

#endif





//--------------------------------------------------------------
// change Big Endian and Little Endian
#if VIM_ENDIANTYPE == VIM_LITTLEENDIAN

#define VIM_ENDIAN16(Value16)  (Value16)
#define VIM_ENDIAN32(Value32)  (Value32)

#else /*VIM_BIGENDIAN*/

#define VIM_ENDIAN16(Value16)  ((((Value16)&0xFF00)>>8)|\
                                (((Value16)&0x00FF)<<8))

#define VIM_ENDIAN32(Value32)  ((((Value32)&0x000000FF)<<24)|\
                                (((Value32)&0x0000FF00)<< 8)|\
                                (((Value32)&0x00FF0000)>> 8)|\
                                (((Value32)&0xFF000000)>>24))

#endif /*VIM_LITTLEENDIAN*/

// Set Bypass or Normal Type

typedef enum _VIM_LDOMODE
{
	VIM_LDO_ON = 0x0,
	VIM_LDO_OFF = 0x1
}VIM_LDOMODE;
typedef struct tag_VIM_CLK
{
	UINT32 ClockIn;	
	UINT8 Mclk;	//8 bit	
	UINT8 Hclk;	//8 bit	
	UINT8 TVLowClk;		//8 bit	
	UINT8 TVHighClk;		//8 bit		
} VIM_CLK,*PVIM_CLK;

typedef struct tag_VIM_PANEINIT
{
	TPoint Point;
	TSize Size;
} VIM_PANEINIT,*PVIM_PANEINIT;

typedef enum _VIM_INTERRUPT_TYPE
{
    	VIM_INT_LEVER_LOW=1,
       VIM_INT_LEVER_HIGH,
       VIM_INT_ADGE_DOWN,
       VIM_INT_ADGE_UP
}VIM_INTERRUPT_TYPE;
typedef struct tag_VIM_GPIO
{
	UINT8 GpioValue;   //if out ,the value 
} VIM_GPIO,*PVIM_GPIO;


typedef struct tag_VIM_INTERRUPT
{
	VIM_INTERRUPT_TYPE Int_Type; 
	UINT8 LeverWidth;  //if type is lever 
} VIM_INTERRUPT,* PVIM_INTERRUPT;
typedef struct tag_VIM_TIMING
{
	UINT8 AbiDelay;
	UINT8 WrDelay;
	UINT8 CsDelay;
} VIM_TIMING,* PVIM_TIMING;

  typedef enum _VIM_EXPPIN_TYPE
{
	VIM_BYPASS_INPUT_NORES=1,
	VIM_BYPASS_INPUT_RES=3,
	VIM_BYPASS_OUTPUT=0
}VIM_EXPPIN_TYPE;
typedef struct tag_VIM_EXTERNPIN_INFO
{
	VIM_EXPPIN_TYPE Type;
	UINT8 OutValue;
} VIM_EXTERNPIN_INFO,* PVIM_EXTERNPIN_INFO;


typedef enum _VIM_EXP_MODE
{
	VIM_EXP_AUTO=0,
	VIM_EXP_50HZ=1,
	VIM_EXP_60HZ
} VIM_EXP_MODE;

typedef struct tag_VIM_EXTERNPINS_INFO
{
	VIM_EXTERNPIN_INFO	Interrupt;
	VIM_EXTERNPIN_INFO	LcdReset;
	VIM_EXTERNPIN_INFO	SensorClk;
	VIM_EXTERNPIN_INFO	SensorReset;
	VIM_EXTERNPIN_INFO	SensorPowerDown;
	VIM_EXTERNPIN_INFO	SensorEnable;
	VIM_EXTERNPIN_INFO	SensorSck;
	VIM_EXTERNPIN_INFO	SensorSda;
	VIM_EXTERNPIN_INFO	SensorVsyn;
	VIM_EXTERNPIN_INFO	SensorHsyn;
	VIM_EXTERNPIN_INFO	GPIO0;
	VIM_EXTERNPIN_INFO	GPIO1;
	VIM_EXTERNPIN_INFO	GPIO2;
	VIM_EXTERNPIN_INFO	GPIO3;
	VIM_EXTERNPIN_INFO	GPIO4;
	VIM_EXTERNPIN_INFO	GPIO5;
	VIM_EXTERNPIN_INFO	GPIO6;
	VIM_EXTERNPIN_INFO	GPIO7;
	VIM_EXTERNPIN_INFO	GPIO8;
} VIM_EXTERNPINS_INFO,* PVIM_EXTERNPINS_INFO;



 typedef enum _VIM_EXP_TYPE
{
	VIM_EXP_UNKOWN=0,
	VIM_EXP_INDOOR=1,
	VIM_EXP_OUTDOOR,
	VIM_EXP_NIGHT
}VIM_EXP_TYPE;

typedef struct tag_VIM_USER_INITINFO
{
	VIM_PANEINIT MainPanel;
	VIM_PANEINIT SubPanel;
	VIM_INTERRUPT InitInterrupt;
	VIM_GPIO InitGpio;
	VIM_CLK WorkMClk;
	VIM_TIMING WorkTiming;
	VIM_EXTERNPINS_INFO PinsMode;
}VIM_USER_INITINFO,*PVIM_USER_INITINFO;

//VIM_RESULT VIM_USER_SeekFile(HUGE void* pFile_Ptr, UINT32 dOffset);
//VIM_RESULT VIM_USER_WriteFile(HUGE void* pFile_Ptr, const HUGE  UINT8* pbBuffer, UINT32 dFile_Size);

HUGE void* VIM_USER_MallocMemory(UINT32 dwbyte);
VIM_RESULT VIM_USER_FreeMemory(HUGE void* mem);
void	VIM_USER_MemSet(HUGE UINT8 *pbDest,  UINT8 bValue, UINT32 dCount);
void	VIM_USER_MemCpy(HUGE UINT8 *pbDest, const HUGE UINT8 *pbSrc, UINT32 dCount);
void VIM_USER_PrintHex(char *string,UINT32 data);
void VIM_USER_PrintDec(char *string,UINT32 data);
void VIM_USER_PrintString(char *string);
void VIM_USER_DelayMs(UINT32 ms);
void VIM_Delay(int time); 
UINT8 VIM_USER_CheckBigendian(void);
//VIM_RESULT VIM_USER_StartTimer(UINT32 Intervel);
//VIM_RESULT VIM_USER_StopTimer(void);

UINT32 VIM_USER_GetSystemTime_1us(void);
UINT32 VIM_USER_GetSystemTime_10ms(void);

void VIM_USER_Reset(void);

//void VIM_USER_AutoFocusInit(void);
void VIM_USER_SetAutoFocusStep(UINT8 Step);
UINT32 VIM_USER_Init_IRQ(void);
#define VIM_USER_Print  MMDPrintf// added by lizg
#ifdef __cplusplus
}
#endif

#endif 

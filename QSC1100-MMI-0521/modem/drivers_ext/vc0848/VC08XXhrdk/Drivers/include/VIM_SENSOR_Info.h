
/*************************************************************************
*                                                                       
*             Copyright (C) 2005 Vimicro CO.,LTD     		 
*
* FILE NAME                                  VERSION                
*                                                                       
* VIM_SENSOR_Info.h				0.1                    
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*    VC0578B  sensor information head file                      
*
*	Version		Author			Date		Description		
*  ---------------------------------------------------------------						 
*   	0.1		yaoweiquan  		2007-04-18	The first version. 
*  ---------------------------------------------------------------
*                                                                       
*************************************************************************/
/****************************************************************************
This source code has been made available to you by VIMICRO on an
AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
copyrights to use it in any way he or she deems fit, including copying it,
modifying it, compiling it, and redistributing it either with or without
modifications. Any person who transfers this source code or any derivative
work must include the VIMICRO copyright notice and this paragraph in
the transferred software.
****************************************************************************/
#ifndef _VIM_5XRDK_SENSORINFO_H_
#define _VIM_5XRDK_SENSORINFO_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void	(*PSensorSetReg)(UINT32 uAddr, UINT16 uVal);
typedef void	(*PSensorGetReg)(UINT32 uAddr, UINT16 *uVal);
typedef UINT8	(*PSnrIdChk)(void);
typedef void	(*PSnrMirrorFlip)(UINT8 val);
typedef void	(*PSnrContrast)(UINT8 val);
typedef void	(*PSnrBrightness)(UINT8 val);
typedef void	(*PSnrMode)(UINT8 val);
typedef void	(*PVispIsrCallback)(void);

typedef void	(*PSensorSetET)(UINT8 freqtype, UINT32 t, TSize size, UINT32 snrclock);
//typedef void	(*PSnrSwitchSize)(UINT8 index, UINT32 et, UINT8 gain);  //不用了，要删除掉



//============== sif type =======================
//I2c Structure
typedef struct tag_TI2cBatch
{
	UINT8 RegBytes;//bit0-bit1 :byte number of regval used
	UINT32 RegAddr;
	UINT8 RegVal[3];
} TI2cBatch, *PTI2cBatch;

typedef struct tag_TAeParm
{
	UINT32	ytarget;
	UINT32	ythreshold;
	UINT32	ytop;
	UINT32	gtarget;						//gain target
	UINT32	gthreshold;						//gain threshold
	UINT32	gdelay;
	UINT32	gain;
	UINT32	minet;							//if minet=0,et can decrease as it shoule be.if minet=1,it means 
	UINT32	maxgain;
	UINT32	mingain;
	UINT32	speed;							//adjust ae speed 0,1,2 
} TAeParm, *PTAeParm;
typedef struct tag_TFlashParm
{
	UINT32 yflashthd;
	UINT32 flashstep;

	UINT32 FlashClockNum;						//Auto Focus function:flash clock number=MCLK/Charge frq
	UINT32 DutyCount;						//Auto Focus function:duty count based on FlashClcokNum and duty 
} TFlashParm, *PTFlashParm;

typedef struct tag_THistParm
{
	UINT32	ratio;							//percent of whole frame pixel, 1 is recommend.
} THistParm, *PTHistParm;

typedef struct tag_TSnrSizeCfg
{
	TRegGroup	snr;
	TRegGroup	visp;
	TSize		size;
	UINT32		maxet;
	UINT32		snrclkratio;		//SNR CLK = MCLK / ratio
	UINT32		snrpixelratio;		//SNR Pixel freq = SNR CLK / pixel ratio
} TSnrSizeCfg, *PTSnrSizeCfg;

typedef struct tag_TTSnrSizeCfgGroup
{
	UINT32 len;
	TSnrSizeCfg *pSizeCfg;
} TSnrSizeCfgGroup, *PTSnrSizeCfgGroup;

typedef void	(*PSnrGetEt)(UINT8 index,UINT32 *et,UINT32 *GLgain,UINT8 *AWB);
typedef void	(*PSnrSetEt) (UINT8 index,UINT32 et, UINT32 Glgain,UINT8*AWB);
typedef struct tag_TSensorInfo
{
	char				desc[40];	

	UINT32				snrtype;			//sensor type(RGB or YUV)
	UINT32				pclk;				//use PCLK of sensor
	UINT32				clkcfg;				//[0]: VSync delay enable; [1]: Fifo VSync delay enable; [2]: Use PCLK Negtive Edge.
	UINT32				bustype;			//[0]: 0 -- I2C, 1 -- Serial bus;
	UINT32				i2ctype;			//[0]: 1 -- OV I2C bus
	UINT32				i2crate;			//I2C rate : KHz				
	UINT32				i2caddress;			//I2C address	if bit 15 is1  is 10 bit len
	UINT32				i2cispaddress;			//ISP I2C address
	UINT32				pwrcfg;				//sensor power initial configure(SIF REG801 BIT[0]~BIT[2])
	UINT32				snrrst;				//Reset sensor enable

	UINT32				brightness;			//brightness
	UINT32				contrast;			//contrast
	UINT32				saturation;			//saturation
	UINT32				gammaRGB[17];			// RGB gamma value array
	
	TRegGroup			sifpwronseq;			//Sif config sequence(Reg.800 bit0~2) when sensor power on
	TRegGroup			standby;			//sensor standby register
	TRegGroup			sifstdbyseq;			//Sif config sequence(Reg.800 bit0~2) when sensor standby
	
	TRegGroup			dsif[4];				//SIF initial configure
	TRegGroup			disp;				//ISP initial configure	

	TRegGroup			Initpoweron;			//SENSOR ISP initial configure
	TSnrSizeCfgGroup		snrSizeCfg;			//sensor size configure information
	TSnrSizeCfgGroup		snrSizeCfgforVideoRecorder;			//sensor size configure information

	PSensorSetReg			snrSetRegCall;			//set reg callback
	PSensorGetReg			snrGetRegCall;
	PSnrIdChk			snrIdChkCall;
	PSnrMirrorFlip			snrMirrorFlipCall;
	PSensorSetET			snrSetETCall;			//set et callback
	
//	PSnrSwitchSize			snrSwitchSizeCall;		//不用了，要删除掉
	
	PSnrContrast			snrContrastCall;
	PSnrBrightness			snrBrightnessCall;
	PSnrMode			snrModeCall;
	PSnrGetEt			snrGetEt;
	PSnrSetEt			snrSetEt;

	TAeParm				aeparm;
	TFlashParm			flashparm;
	THistParm			histparm;
} TSnrInfo, *PTSnrInfo;

extern const PTSnrInfo gPSensorInfo[];


#define FAILED FALSE
#define SUCCEED TRUE

/********************************************************************************
*  select sensor
**********************************************************************************/
// 0.3M
#define V5_DEF_SNR_MT9V011_BAYER		0	//MI360


// 1.3M
#define V5_DEF_SNR_MT9M111_YUV		0	//MI1310SOC

#if(CHIP_ID!=ID_FPAG)
#define V5_DEF_SNR_MT9M112_YUV		0	//MI1320SOC ,big sensor for FPGA board
#define V5_DEF_SNR_MT9M112_YUV_5XX	0 	//MI1320SOC 5XX/8XX, small sensor for demo board,add by lizg 
#define V5_DEF_SNR_MT9M112_YUV_5XX_NEW	0	//MI1320SOC 5XX/8XX, small sensor for demo board,add by lizg 
#define V5_DEF_SNR_OV9650_YUV       0   //ov9650soc 824 add by guoyuankai
#else
#define V5_DEF_SNR_MT9M112_YUV		0	//MI1320SOC
#define V5_DEF_SNR_MT9M112_YUV_5XX	0 	//MI1320SOC 5XX/8XX, add by lizg
#define V5_DEF_SNR_OV9650_YUV       0   //ov9650soc 824 add by guoyuankai
#endif

#define V5_DEF_SNR_MT9M112_BAYER		0

// 2M
#define V5_DEF_SNR_OV2640_YUV		       0
#define V5_DEF_SNR_OV2650_YUV		       1

#define V5_DEF_SNR_OV2640_BAYER		0
// 3M
#define V5_DEF_SNR_MT9T012_BAYER		0
#define V5_DEF_SNR_OV3630_BAYER		0  

#if ( PROJECT_NU820_VERSION==4) //NU820_MOBILE_RELEASE
#define V5_DEF_SNR_DUMMY				1
#else
#define V5_DEF_SNR_DUMMY				0
#endif

extern TSnrInfo gSensorInfo_DUMMY; 

#ifdef __cplusplus
}
#endif

#endif 

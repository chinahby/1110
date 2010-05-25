/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2005                                                     
 * ------------------------------------------------------------------------------
 * [File Name]            : VIM_SIF_Driver.h
 * [Description]          : 5X 's LCDIF moudle sub driver:578b 
 * [Author]                 : WendyGuo	
 * [Date Of Creation]  : 2007-03-15 
 * [Platform]              : ARM7
 * [Note]                    : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        		Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2005-11-16  	Liuhd			The first version
 * 2006-06-08  	Angela			Update for 528
 * 2006-03-15  	WendyGuo         	Created 578b
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

#ifndef _VIM_0528RDK_SIF_H_
#define _VIM_0528RDK_SIF_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define I2C_WRDELAY 0x7f00
#define I2C_WRDELAY 0x80
#define SIF_SYNC_MODE    VIM_SIF_SNSOR_NORMAL_SYNC  
typedef enum tag_VIM_SIF_RESOLUTION
{
	VIM_SIF_MAXPIXEL = 0,
	VIM_SIF_HALFPIXEL,
	VIM_SIF_HALFHALFPIXEL,
	VIM_SIF_RESERVED=0x7FFFFFFF
}VIM_SIF_RESOLUTION;

typedef enum  tag_VIM_SIF_OUTPUT_SYNC
{
	VIM_SIF_SNSOR_NORMAL_SYNC    =0,
	VIM_SIF_SNSOR_CCIR656_SYNC,    
	VIM_SIF_SNSOR_RESERVED=0x7FFFFFFF
}VIM_SIF_OUTPUT_SYNC;

typedef enum  tag_VIM_SIF_FRAMERATE
{
	VIM_SIF_SNSOR_FRAMERATE_FORCAPIMAGE    =0,
	VIM_SIF_SNSOR_FRAMERATE_FORCAPVIDEO,    
	VIM_SIF_SNSOR_FRAMERATE_RESERVED=0x7FFFFFFF
}VIM_SIF_FRAMERATE;

typedef enum tag_VIM_SIF_SENSORSTATE
{
	VIM_SIF_SENSOR_POWERON, /* Sensor is on power on state */
//	VIM_SIF_SENSOR_POWEROFF, /* Sensor is on power off state */
	VIM_SIF_SENSOR_STANDBY, /* Sensor is on standby state */
//	VIM_SIF_SENSOR_SLEEP /* Sensor is on sleep state  */
	VIM_SIF_SENSOR_UNKOWNSTATE,
	
	VIM_SIF_SENSOR_RESERVED=0x7FFFFFFF
}VIM_SIF_SENSORSTATE;

typedef enum _VIM_SIF_CLKTYPE
{
	VIM_SIF_SENSOR_CLKNORMAL=0, /* Sensor normal clk */
	VIM_SIF_SENSOR_HALF,
	VIM_SIF_CLKTYPE_UNKOWNED=0x7FFFFFFF /* Sensor half clk ,yuv is 1/4*/
}VIM_SIF_CLKTYPE;

//Define the parameter value of the Auto focus function;	Written by Wendy Guo 3/16/2007 
typedef enum tag_VIM_SIF_FLASH_CLOCKNUM		//MCLK/charge frequence
{
	VIM_SIF_SNSOR_CHARGE_FRQ_9K=0x0A6B,				//24Mhz/9Khz=0x0A6B
	VIM_SIF_SNSOR_CHARGE_FRQ_10K=0x0960,			//24Mhz/10Khz
	VIM_SIF_SNSOR_CHARGE_FRQ_112500=0x0855,			//24Mhz/11.25Khz
	VIM_SIF_SNSOR_CHARGE_FRQ_128500=0x074C,			//24Mhz/12.85Khz
	VIM_SIF_SNSOR_CHARGE_FRQ_15K=0x0640,			//24Mhz/15Khz
	VIM_SIF_SNSOR_CHARGE_FRQ_18K=0x0535,			//24Mhz/18Khz
	VIM_SIF_SNSOR_CHARGE_FRQ_22K=0x0443,			//24Mhz/22Khz
	VIM_SIF_SNSOR_CHARGE_FRQ_30K=0x0320	,			//24Mhz/30Khz
	VIM_SIF_SNSOR_CHARGE_FRQ_UNKOWN=0x7FFFFFFF
	
}VIM_SIF_FLASH_CLOCKNUM;

typedef enum tag_VIM_SIF_DUTY_COUNT		//VIM_SIF_SNSOR_CHARGE_FRQ*(1-charge duty)
{
	VIM_SIF_PWM_DUTY_DIV2=2,					//0x0320*(1-1/3)
	VIM_SIF_PWM_DUTY_DIV3=3,					//0x0443*(1-1/4)
	VIM_SIF_PWM_DUTY_DIV4=4,					//0x0535*(1-1/5)
	VIM_SIF_PWM_DUTY_DIV5=5,					//0x0640*(1-1/6)
	VIM_SIF_PWM_DUTY_DIV6=6,					//0x0855*(1-1/8)
	VIM_SIF_PWM_DUTY_DIV7,
	VIM_SIF_PWM_DUTY_DIV8,
	VIM_SIF_PWM_DUTY_DIV9,
	VIM_SIF_PWM_DUTY_DIV10,
	VIM_SIF_PWM_DUTY_DIV11,
	VIM_SIF_PWM_DUTY_DIV12,
	VIM_SIF_PWM_DUTY_DIV13,
	VIM_SIF_PWM_DUTY_DIV14,
	VIM_SIF_PWM_DUTY_DIV15,
	VIM_SIF_PWM_DUTY_DIV16,
	VIM_SIF_PWM_DUTY_UNKOWN=0x7FFFFFFF
}VIM_SIF_DUTY_COUNT;




typedef enum tag_VIM_SIF_PWM_NUM		//VIM_SIF_SNSOR_CHARGE_FRQ*(1-charge duty)
{
	VIM_SIF_PWM_NUM0=BIT0,
	VIM_SIF_PWM_NUM1=BIT1,
	VIM_SIF_PWM_UNKOWN_NUM=0x7FFFFFFF
}VIM_SIF_PWM_NUM;

typedef enum tag_VIM_SIF_PWM_POL		//VIM_SIF_SNSOR_CHARGE_FRQ*(1-charge duty)
{
	VIM_SIF_PWM_LOWACTIVE=0,
	VIM_SIF_PWM_HIGHACTIVE=1,
	VIM_SIF_PWM_POL_UNKOWN=0x7FFFFFFF
}VIM_SIF_PWM_POL;
typedef enum tag_VIM_SENSOR_TYPE		//VIM_SIF_SNSOR_CHARGE_FRQ*(1-charge duty)
{
	VIM_SIF_SENSOR_TYPE_YUV=1,
	VIM_SIF_SENSOR_TYPE_RGB=0,
	VIM_SIF_SENSOR_TYPE_UNKOWN=0x7FFFFFFF
}VIM_SIF_SENSOR_TYPE;

#if (BOARD_TYPE==VC0820_FPGA)
typedef enum _VIM_SIF_INT_TYPE
{
	VIM_INT_SIF_VSYNC_INT		= 0,
	VIM_INT_SIF_FRAMEEND		= 1,
	VIM_SIF_INT_ALL=VIM_SIF_SIF_INT_NUM,
	VIM_SIF_INT_UNKOWN =0x7FFFFFFF
}VIM_SIF_INT_TYPE;
#else
typedef enum _VIM_SIF_INT_TYPE
{
	VIM_INT_SIF_VSYNC_INT		= 0,
	VIM_INT_SIF_FRAMEEND		= 1,
	VIM_INT_SIF_YMEAN		=2,
	VIM_INT_SIF_ANTIF		=3,
	VIM_INT_SIF_AUTOFOC	=4,
	VIM_SIF_INT_ALL		= (VIM_SIF_SIF_INT_NUM+VIM_SIF_ISP_INT_NUM),
	VIM_SIF_INT__UNKOWN=0x7FFFFFFF
}VIM_SIF_INT_TYPE;
#endif

typedef struct _VIM_ISP_TAEADJUST {
	UINT8	ymean; /* Current Y Mean value */
	UINT8	ytarget; /* Y Mean target value */
	UINT8	ythd; /* Y Mean changes threadshold */
	UINT8	ytop; /* Y Mean max value */
	UINT8	gain; /* Digital gain value */
	UINT8	gtarget;	/* Gain target value */
	UINT8	gthd; /* Gain threshold */
	UINT8	gstep; /* Gain step */
	UINT32	et; /* Expose time */
	UINT32	newet; /* New Expose time, when auto expose, need to recalculate et */
	UINT32	snrclock; /* Sensor clock */
	UINT32	freq; /* Light frequence, 50Hz or 60Hz */
	UINT8	maxet; /* Max expose time */
	UINT8	minet; /* Min expose time */
	UINT8	maxgain; /* Max digital gain value */
	UINT8	mingain; /* Min digital gain value */
	UINT8	speed; /* Adjust ae speed 0,1,2 */
	UINT8	env; /* Indoor, out door or night */
	UINT8	brightness; /* Sensor default brightness */ 
	UINT8	contrast; /* Sensor default brightness */
	UINT8	saturation; /* Sensor default saturation */
} VIM_ISP_TAEADJUST, *VIM_ISP_PTAEADJUST;



VIM_RESULT VIM_SIF_SifInitI2c(const TSnrInfo *pSensorInfo, UINT32 dwMclk);
VIM_RESULT VIM_SIF_SifSensorInit(const TSnrInfo *pSensorInfo,VIM_ISP_TAEADJUST *paeadjust);
VIM_RESULT VIM_SIF_SetSensorResolution(const TSnrInfo *pSensorInfo,	VIM_ISP_TAEADJUST *taeadjust,	VIM_SIF_RESOLUTION byResolution,VIM_SIF_FRAMERATE frmrate);
VIM_RESULT VIM_SIF_SetSensorState(const TSnrInfo *pSensorInfo, VIM_SIF_SENSORSTATE bySensorState);
VIM_RESULT VIM_SIF_SetSensorPowerOn(const TSnrInfo *pSensorInfo);
VIM_RESULT VIM_SIF_GetSensorResolution(const TSnrInfo *pSensorInfo, VIM_SIF_RESOLUTION bySensorResolution, TSize *InputSize);
VIM_RESULT VIM_SIF_SetSensorClk(const TSnrInfo *pSensorInfo,VIM_SIF_CLKTYPE Type);
VIM_SIF_SENSOR_TYPE VIM_SIF_GetSensorType(void);


void 	VIM_SIF_SensorReset(void);
void 	VIM_SIF_EnableSyncGen(UINT8 bEnable);
void        VIM_SIF_StartCaptureEnable(UINT8 bEnable);
void 	VIM_SIF_SetI2cDevAddr(UINT16 adr);
UINT16 	VIM_SIF_I2cWriteByte(UINT32 byAddr, UINT8 byVal,UINT8 num);			//wendy guo 3/29/2007,change UINT8 byVal to UINT32
UINT16 	VIM_SIF_I2cWriteWord(UINT32 uAddr, UINT16 uVal,UINT8 num);			//wendy guo 3/29/2007,change UINT8 byVal to UINT32
UINT16 	VIM_SIF_I2cReadByte(UINT32 byAddr, UINT8 *pbyVa,UINT8 num);		//wendy guo 3/29/2007,change UINT8 byVal to UINT32
UINT16 	VIM_SIF_I2cReadWord(UINT32 uAddr, UINT16 *uVal,UINT8 num);			//wendy guo 3/29/2007
void 	VIM_SIF_I2cAeBatch(UINT8 uNum, TI2cBatch *AeData,UINT8 addrbyte);
void		VIM_SIF_AutoFocus(const TSnrInfo *pSensorInfo);					//new function written by WendyGuo 3/16/2007
void VIM_SIF_SetPwmInit(VIM_SIF_PWM_NUM Num,VIM_SIF_PWM_POL Pro);
void VIM_SIF_SetPwmEn(BOOL Enable);
void VIM_SIF_SetPwmDuty(UINT8 Duty,UINT8 Max);
void VIM_SIF_SetPwmClock(UINT32 Clock);

void _ISR_SifSifIntHandle(UINT8 wIntFlag);
#if (BOARD_TYPE==VC0820_FPGA)
#else
 void _ISR_SifIspIntHandle(UINT8 wIntFlag);
#endif
 void VIM_SIF_ISP_RegisterISR(VIM_SIF_INT_TYPE bySecondLevelInt, PVIM_Isr  Isr);
 void VIM_SIF_ISP_SetIntEnable(VIM_SIF_INT_TYPE bySecondLevelInt,BOOL Eanble);

void VIM_SIF_IspDoAFAtOnetime(void);

VIM_RESULT VIM_SIF_InitI2c(
	UINT32 i2caddress,			/*(IN)	The pointer to TSensorInfo structure	*/
	UINT32 Clk	);
/*
  FLASH FUNCTION
*/


/*
  FLASH FUNCTION
*/


#if 1
/*******************************************************/

/*******************ISP Module****************************/ 

/*******************************************************/
//  define sensor type
#define SENSOR_TYPE_YUV	1
#define SENSOR_TYPE_RGB	0

//light frequence type
typedef enum _FREQ_TYPEenum
{
	VIM_SIF_FREQ_50HZ =		0x0,
	VIM_SIF_FREQ_60HZ =		0x1,
	VIM_SIF_FREQ_NONE =		0x2,		//outdoor
	VIM_SIF_FREQ_NOTSURE =	0x3
}FREQ_TYPEenum;
#define EPFREQ_50HZ			100000
#define EPFREQ_60HZ			120000
enum VIM_AEADJUSTRET
{
	GAIN_ONLY = 0x0,
	GAIN_ET = 0x1,
	UN_ET_GAIN = 0x2
};





#define ET_ONE_STEP		1000
#define CLOCK_MUL			1000
#define AE_M			256
enum AE_FOMULA_FLAG
{
	AE_GET_ET,
	AE_GET_GAIN
};
typedef enum tag_VIM_SIF_ISP_EXP_MODE
{
	VIM_SIF_EXP_AUTO=0, /* Light Frequence when expose: auto select */
	VIM_SIF_EXP_50HZ=1, /* Light Frequence when expose: 50Hz */
	VIM_SIF_EXP_60HZ /* Light Frequence when expose: 60Hz */
}VIM_SIF_ISP_EXP_MODE;


typedef enum tag_VIM_SIF_ISP_CTRL_UPDATE
{
	VIM_SIF_ISP_GLBGAIN_UPDATE	=BIT0, 	/*After update [22H]~[23H] Global gain register,  you must enable this bit. */
	VIM_SIF_ISP_DGAIN_UPDATE	  	=BIT1, 	/*After update [1eH]~[20H] Digital RGBgain register,  you must enable this bit.  */
	VIM_SIF_ISP_BGAM_UPDATE	 	=BIT2, 	/*After update [6cH]~[7cH] Bgma registers,  you must enable this bit. */
	VIM_SIF_ISP_GGAM_UPDATE	  	=BIT3, 	/*After update [5bH]~[6bH] Ggma registers,  you must enable this bit. */
	VIM_SIF_ISP_RGAM_UPDATE	  	=BIT4, 	/*After update [4aH]~[5aH] Rgma registers,  you must enable this bit */
	VIM_SIF_ISP_COLMAT_UPDATE	=BIT5, 	/*After update [28H]~[37H] Color matrix registers,  you must enable this bit. */
	VIM_SIF_ISP_GRAY_UPDATE	  	=BIT6, 	/*After update [26H]~[27H] CbCrOfs,  you must enable this bit. */
	VIM_SIF_ISP_FEM_UPDATE	  	=BIT7	/*After update  fem_m fem_p, fem_x1, fem_x2, fem_max, fem_min[0aH]~[0fH],  you must enable this bit.*/	
}VIM_SIF_ISP_CTRL_UPDATE;
///////////////////////////////////////


#define VIM_SIF_ET_GAINRANGE_STEP			4
#define VIM_SIF_ET_GAINRANGE_OVERLAY		2
 #define VIM_ISP_MAX_BRIGHT_CONTRAST 64		//algorithmic require
#define VIM_ISP_MIN_BRIGHT_CONTRAST 1


#define BRIGHTNESS_STEP	0x02
#define CONTRAST_STEP		0x05
		

typedef struct tag_TAeStep {
	UINT8 step;				//if et change<step,et only change 1
	UINT8 div;				//if et change > setp, et change newet-et
} TAeStep, *PTAeStep;


#endif

#if (BOARD_TYPE==VC0820_FPGA)
#else
void VIM_SIF_IspCtrlUpate(UINT8  sel);
void VIM_SIF_IspSetContrast(VIM_ISP_TAEADJUST *paeadjust,const TSnrInfo *pSensorInfo,UINT8 byValue);
void VIM_SIF_IspSetSaturation(VIM_ISP_TAEADJUST *paeadjust,UINT8 byValue);
void VIM_SIF_IspSetBrightness(VIM_ISP_TAEADJUST *paeadjust,const TSnrInfo *pSensorInfo,UINT8 byValue);
void VIM_SIF_IspSetImageSize(UINT16 wWidth, UINT16 wHeight);
void VIM_SIF_IspAutoExposeISR(void);
void VIM_SIF_IspAutoFocusISR(void);
void VIM_ISP_SetAutoIntFreq(UINT8 val);
UINT8 VIM_SIF_IspAutoExpose(VIM_ISP_PTAEADJUST ae,TSnrInfo *pSensorInfo);
#endif
//<<<<<<<<<<<<
//UINT8 VIM_SIF_IspAutoFocus(void);
//>>>>>>>>>
UINT8 VIM_SIF_IspAutoFocus(UINT8 *position);
VIM_RESULT VIM_SIF_SetSensorReg(const TSnrInfo *pSensorInfo,UINT32 regaddr,UINT16 regvalue);
VIM_RESULT VIM_SIF_GetSensorReg(const TSnrInfo *pSensorInfo,UINT32 regaddr,UINT16 *regvalue);	

void VIM_SIF_IspEnableFocus(BOOL enable);
void VIM_SIF_IspSelEMMarix(UINT8 index);
#ifdef __cplusplus
}
#endif

#endif




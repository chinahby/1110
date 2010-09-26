/*******************************************************************
 * OV7670 VGA for C108 Sensor
 * William 20080922
 *
 *
 *******************************************************************/ 
#include "a8_common.h"
#include "a8_sensor.h"
#include "a8_vif.h"
#include "tlg1120api.h"
#include "tlg1120app.h"

#include "AIT700_EBIbus.h"
#include "ait_interface.h"
#include "platform.h"
//extern  void AITS_I2C_Init();
#ifdef AIT_ATV_SUPPORT

#ifdef SENSOR_VIF
extern u_char	gbSensorInited;
#endif

extern uint32           tlg_i2c_addr;
#define SENSOR_ID (0x5555)
#define	TLG_I2C_TYPE			  (A8_I2C_2A2D)

#if 1	//A-TV
#define TLG_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)

#else	//D-TV
#define TLG_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS)
#endif

#define TLG_FULL_VH_POLARITY     (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define TLG_PREVIEW_LATCH_COUNT  (A8_PHASE_COUNTER_NUM_4) //Wind@20091224 (A8_PHASE_COUNTER_NUM_4)
#define TLG_FULL_LATCH_COUNT     (A8_PHASE_COUNTER_NUM_4)
#define TLG_PREVIEW_YUV_FORMAT   (A8_SENSOR_CRYCBY)
#define TLG_FULL_YUV_FORMAT      (A8_SENSOR_CBYCRY)
#define TLG_LATCH_EDGE        	 (A8_SENSOR_POS_EDGE)	//Wind@20091224 (A8_SENSOR_POS_EDGE)


#define	TLG_I2C_ID				(0x2c<<1)

#define	TLG_PREVIEW_WIDTH  	    (360)
#define	TLG_PREVIEW_HEIGHT	   	(288)



__align(4)static u_short  Sensor_Init[] = 
{
0xffff,0xffff   
};

__align(4)static u_short    Sensor_Video_Preview[]=
{
0xffff,0xffff   
};
__align(4)static u_short	Sensor_DSC_Preview[] =
{
0xffff,0xffff
};
__align(4)static u_short	Sensor_Normal_Capture[] = 
{
0xffff,0xffff
};
__align(4)static u_short  Sensor_Full_Capture[] = 
{
0xffff,0xffff
};
__align(4)static u_short Sensor_Full_Capture_ScaleUp[] =
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EFFECT_NORMAL[] = 
{	// normal 0 
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EFFECT_1[] = 
{	// black and write 
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EFFECT_2[] = 
{	// huaijiu
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EFFECT_3[] = 
{	// sepia	//B&W negative //fupian
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EFFECT_4[] = 
{	// Negative
0xffff,0xffff
};

__align(4)static u_short  SENSOR_NIGHT_MODE_ON[] = 
{ 
0xffff,0xffff
};
__align(4)static u_short  SENSOR_NIGHT_MODE_OFF[] = 
{
0xffff,0xffff  
};
__align(4)static u_short  SENSOR_WB_AUTO[] = 
{ 
0xffff,0xffff
};
__align(4)static u_short  SENSOR_WB_DAYLIGHT[] = 
{   // Sunny 
0xffff,0xffff
};
__align(4)static u_short  SENSOR_WB_INCANDESCENT[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_WB_FLUORSCENT[] = 
{   // Indoor Home 
0xffff,0xffff
};
__align(4)static u_short  SENSOR_WB_CLOUDY[] = 
{   // Cloud 
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_0[] = 
{	// EV -2
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_1[] = 
{	// EV -1.5
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_2[] = 
{	// EV -1
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_3[] = 
{	// EV -0.5
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_4[] = 
{	// EV 0
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_5[] = 
{	// EV +0.5
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_6[] = 
{	// EV +1
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_7[] = 
{	// EV +1.5
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_8[] = 
{	// EV +2
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_9[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_10[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_11[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_12[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_13[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_14[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_15[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_16[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_17[] = 
{
0xffff,0xffff
};
__align(4)static u_short  SENSOR_EV_18[] = 
{
0xffff,0xffff
};
__align(4)static u_short	SENSOR_PHOTOMETRY_NORMAL[] =
{
0xffff,0xffff
};
__align(4)static u_short	SENSOR_PHOTOMETRY_CENTER[] =
{
0xffff,0xffff
};

static u_short	Sensor_PowerOn_Sequence(void)
{
    return A8_NO_ERROR;
}

static u_short	Sensor_PowerOff_Sequence(void)
{	
    return A8_NO_ERROR;
}

static u_short	Sensor_Effect(u_short EffectMode)
{
    return A8_NO_ERROR;
}

static u_short	Sensor_EV(u_short EvStep)
{
    return A8_NO_ERROR;
}


static u_short	Sensor_WhiteBalance(u_short WbMode)
{
    return A8_NO_ERROR;


}


static u_short	Sensor_NightMode(mmp_sensor_night_mode NightMode)
{
    return A8_NO_ERROR;
}




static u_short	Sensor_DeBand(mmp_sensor_deband DeBandMode)
{
    return A8_NO_ERROR;	
}

static u_short	Sensor_Rotate(mmp_sensor_rotate Rotate)
{
    return A8_NO_ERROR;
}

static u_short 	Sensor_FrameRate(u_short FrameRate)
{
    return A8_NO_ERROR;
}


static t_sensor_mode_attr	preview_mode =
{
	TLG_PREVIEW_VH_POLARITY,
	TLG_PREVIEW_LATCH_COUNT,
	TLG_PREVIEW_YUV_FORMAT,
	TLG_LATCH_EDGE,
	1,
	1,
	TLG_PREVIEW_WIDTH,
	TLG_PREVIEW_HEIGHT,
	0,
	TLG_PREVIEW_LATCH_COUNT
};
static t_sensor_mode_attr	full_mode =
{
/*temp. the same as preview mode*/
	TLG_PREVIEW_VH_POLARITY,
	TLG_PREVIEW_LATCH_COUNT,
	TLG_PREVIEW_YUV_FORMAT,
	TLG_LATCH_EDGE,
	1,
	1,
	TLG_PREVIEW_WIDTH,
	TLG_PREVIEW_HEIGHT,
	0,
	TLG_PREVIEW_LATCH_COUNT
};

#if 1
static void	Sensor_Enable(u_short enable)
{

extern kal_int8 init_ATV(void);
extern void power_off_ATV(void);
extern void PowerOffFM(void);
extern	u_char	g_ATV_Flag;

	if(enable) 
	{	
		A8L_VIF_Init(&preview_mode);	//
		
		if (g_ATV_Flag == 0){
			AIT_Message_P0("===TLG===ATV turn Off, Sensor turn on \n\r");

			//ATV off
			PowerOffFM();

			//sensor power LDO
			A8L_SetSensorPowerPin(1);
			sys_IF_ait_delay1ms(10);
			//sensor PWDN
			A8L_SetSensorEnablePin(0);
			sys_IF_ait_delay1ms(10);       
			//Reset
			A8L_SetSensorResetPin(1); 
			sys_IF_ait_delay1ms(10);
			A8L_SetSensorResetPin(0);
			sys_IF_ait_delay1ms(10); 
			A8L_SetSensorResetPin(1); 
			sys_IF_ait_delay1ms(10);

			A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
			A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
			sys_IF_ait_delay1ms(1);
		}
		else
		{
			AIT_Message_P0("===TLG===ATV turn ON, Sensor Sleep \n\r");
			//SetA8RegB(0x69D4,(GetA8RegB(0x69D4)|8)& (~7));
			//SetA8RegB(0x69CD,(GetA8RegB(0x69CD)|4)&(~2));
			//SetA8RegB(0x69CE,(GetA8RegB(0x69CE)|4)&(~2));
			//SetA8RegB(0x69a0,(GetA8RegB(0x69a0))&(~0x18));

			init_ATV();
			sys_IF_ait_delay1ms(1);
			
			A8L_SetSensorPowerPin(1); //Turn on			
    		A8L_SetSensorEnablePin(0); //Enable
			A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
			A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
			//Reset
			A8L_SetSensorResetPin(1); 
				sys_IF_ait_delay1ms(10);
			A8L_SetSensorResetPin(0);
				sys_IF_ait_delay1ms(10); 
			A8L_SetSensorResetPin(1); 
				sys_IF_ait_delay1ms(10);
    		A8L_SetSensorEnablePin(1); //Disable
    			sys_IF_ait_delay1ms(10);
		}

	        		
	}
	else 
	{
	AIT_Message_P0("===TLG===ATV / Sensor all turn off \n\r");

	//ATV off
	PowerOffFM();
	
	sys_IF_ait_delay1ms(1); 
	//Sensor off
	A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_OFF);
	A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_OFF);	
	SetA8RegB(VIF_SEN_CLK_CTL,0);
	A8L_SetSensorResetPin(0);
	A8L_SetSensorEnablePin(0); //Disable sensor
	A8L_SetSensorPowerPin(0); //Turn off sensor
	}
}
#else
// this sensor is low enable
//can not put sensor setting here
static void	Sensor_Enable(u_short enable)
{

	AIT_Message_P1("Sensor_Enable: %d",enable);

	if(enable) {
		SetA8RegB(0x69D4,(GetA8RegB(0x69D4)|8)& (~7));
		SetA8RegB(0x69CD,(GetA8RegB(0x69CD)|4)&(~2));
		SetA8RegB(0x69CE,(GetA8RegB(0x69CE)|4)&(~2));
		SetA8RegB(0x69a0,(GetA8RegB(0x69a0))&(~0x18));

		A8L_VIF_Init(&preview_mode);

		init_ATV();


		sys_IF_ait_delay1ms(1);
	} else {


		PowerOffFM();
		SetA8RegB(VIF_SEN_CLK_CTL,0);
		sys_IF_ait_delay1ms(1);  	

		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);		
		gbSensorInited = 0;	

	}	
}

#endif
static u_short	Get_Sensor_ID(u_short*	SensorID)
{
	AIT_Message_P0("Get_Sensor_ID");

    *SensorID = 0;
#if 1//test only
    *SensorID = SENSOR_ID;
#endif

    return 0;


}
static u_short	ChangeSensorAE(void)
{
    return  0;
}      
int vhpolarity = 0;
int yuvformat = 0;
static u_short SetSensorResolution(mmp_sensor_mode sensor_mode)
{
	u_short	*tab_addr,tab_len;
	u_short fRet = 0;
	uint16 reg0;

	extern char TLG1120_TestPatternOn;
	extern kal_uint8	atv_cur_region;
	extern TLG_TV_SCANLINES gTvScanLines;

	AIT_Message_P1("SetSensorResolution:%d",sensor_mode);
	switch(sensor_mode)
	{
		case SENSOR_INIT:
#ifdef CHECK_POLARITY
	switch(vhpolarity)
	{
	case 0:
		preview_mode.hv_polarity = (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG);
		break;

	case 1:
		preview_mode.hv_polarity = (A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS);
		break;
	case 2:
		preview_mode.hv_polarity = (A8_SENSOR_HOR_NEG|A8_SENSOR_VER_POS);
		break;
	case 3:
		preview_mode.hv_polarity = (A8_SENSOR_HOR_NEG|A8_SENSOR_VER_NEG);
		break;

    	}			
	vhpolarity++;
	if(vhpolarity==4)	
		vhpolarity = 0;
#endif
#ifdef CHECK_YUV

		preview_mode.yuv_format  = vhpolarity;	
		vhpolarity++;
		if(vhpolarity==4)	
			vhpolarity = 0;
#endif


		A8L_VIF_Init(&preview_mode); 	//pass

		AIT_Message_P1("vhpolarity:%d",preview_mode.yuv_format);
			
		A8L_SetSensorAttr(&preview_mode);       //pass     
		TLGAPP_Restart(); 	// pass
		TLGAPP_Init(tlg_i2c_addr); 

//By case		TLG_InitRegionChannelMap(atv_cur_region);

		if(gTvScanLines==TLG_TV_525_LINES)
		{
			cam_IF_ait_TV_SrcRes(AIT_TV_SRC_NTSC);
		}
		else
		{
			cam_IF_ait_TV_SrcRes(AIT_TV_SRC_PALD);
		}

		TLG_ReadReg(tlg_i2c_addr, 0,&reg0);
		TLG_WriteReg(tlg_i2c_addr, 0, reg0|0x300);   

	//	TLGAPP_TurnOnTestPattern();      

                      
		gbSensorInited = 1;
	
		break;	
        case	SENSOR_DSC_PREVIEW:
            tab_addr = (u_short *)Sensor_DSC_Preview;
            tab_len = sizeof(Sensor_DSC_Preview);				
            break;
        case    SENSOR_VIDEO_PREVIEW:
            tab_addr = (u_short *)Sensor_Video_Preview;
            tab_len = sizeof(Sensor_Video_Preview);				
            break;            
        case	SENSOR_NORMAL_CAP_MODE:
            tab_addr = (u_short *)Sensor_Normal_Capture;
            tab_len = sizeof(Sensor_Normal_Capture);				
            break;
        case	SENSOR_FULL_RESOL_MODE:
            A8L_VIF_Init(&full_mode); 
            A8L_SetSensorAttr(&full_mode);            
            tab_addr = (u_short *)Sensor_Full_Capture;
            tab_len = sizeof(Sensor_Full_Capture);				
            break;
        case	SENSOR_SCALE_UP_MODE:
            fRet = ChangeSensorAE();
            tab_addr = (u_short *)Sensor_Full_Capture_ScaleUp;
            tab_len = sizeof(Sensor_Full_Capture_ScaleUp);				
            break;			
    }
		AIT_Message_P2("DSC TEST %s, %d",__FILE__,__LINE__);

        return  fRet;
}


t_sensor_manager sensor_tlg1120 = 
{
	SENSOR_ID,
	TLG_I2C_ID,
	TLG_I2C_TYPE,
	&preview_mode,
	&full_mode,
	SetSensorResolution,
	Sensor_Enable,
	Get_Sensor_ID,
	Sensor_Effect,
	Sensor_EV,
	Sensor_WhiteBalance,
	Sensor_NightMode,
	Sensor_DeBand,
	Sensor_Rotate,
	Sensor_FrameRate
};
#endif

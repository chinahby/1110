#include	"A8_common.h"
#include	"AIT700_EBIbus.h"
#include	"A8_sensor.h"
#include 	"ait_interface.h"

#define	SENSOR_I2C_TYPE						(A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  		(A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_FULL_VH_POLARITY     		(A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_PREVIEW_LATCH_COUNT  		(A8_PHASE_COUNTER_NUM_2)
#define SENSOR_FULL_LATCH_COUNT     		(A8_PHASE_COUNTER_NUM_2)
#define SENSOR_PREVIEW_YUV_FORMAT   		(A8_SENSOR_YCBYCR) //(A8_SENSOR_CBYCRY) //
#define SENSOR_FULL_YUV_FORMAT     	 		(A8_SENSOR_YCBYCR) //(A8_SENSOR_CBYCRY) //
#define SENSOR_LATCH_EDGE           		(A8_SENSOR_POS_EDGE)

#define	SENSOR_I2C_ID  			    		(0xFF)
#define	SENSOR_PREVIEW_WIDTH  	    		(320)
#define	SENSOR_PREVIEW_HEIGHT  	    		(240)
#define	SENSOR_FULL_WIDTH  		    		(320)
#define	SENSOR_FULL_HEIGHT  	    		(240)

extern boolean camsensor_tlg1120_init(void);


static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;


__align(2)const static u_char	Sensor_Init[]=
{
NULL,
};

__align(2)const static u_char	Sensor_DSC_Preview[] =
{
NULL,
};
__align(2)const static u_char	Sensor_Video_Preview[]=
{
NULL,
};
__align(2)const static u_char	Sensor_Normal_Capture[] = 
{
NULL,
};
__align(2)const static u_char  Sensor_Full_Capture[] = 
{
NULL,
};
__align(2)const static u_char Sensor_Full_Capture_ScaleUp[] =
{
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_NORMAL[] = 
{	//Normal
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_1[] = 
{	//BW
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_2[] = 
{	// Bluish 2
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_3[] = 
{	// Negative 3
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_4[] = 
{	// Reddish 4
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_5[] = 
{	// Antique 5
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_6[] = 
{	// Greenish 6
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_7[] = 
{
NULL,
};

__align(2)const static u_char  SENSOR_EV_0[] = 
{	// EV -2
NULL,
};
__align(2)const static u_char  SENSOR_EV_1[] = 
{	// EV -1.5
NULL,
};
__align(2)const static u_char  SENSOR_EV_2[] = 
{	// EV -1
NULL,
};
__align(2)const static u_char  SENSOR_EV_3[] = 
{	// EV -0.5
NULL,
};
__align(2)const static u_char  SENSOR_EV_4[] = 
{	// EV 0
NULL,
};
__align(2)const static u_char  SENSOR_EV_5[] = 
{	// EV +0.5
NULL,
};
__align(2)const static u_char  SENSOR_EV_6[] = 
{	// EV +1
NULL,
};
__align(2)const static u_char  SENSOR_EV_7[] = 
{	// EV +1.5
NULL,
};
__align(2)const static u_char  SENSOR_EV_8[] = 
{	// EV +2
NULL,
};

__align(2)const static u_char  SENSOR_WB_AUTO[] = 
{	// Auto 0
NULL,
};	
__align(2)const static u_char  SENSOR_WB_1[] = 
{	// Sunny 1
NULL,
};	
__align(2)const static u_char  SENSOR_WB_2[] = 
{
NULL,
};	
__align(2)const static u_char  SENSOR_WB_3[] = 
{	// Indoor home 3
NULL,
};	
__align(2)const static u_char  SENSOR_WB_4[] = 
{	// Indoor office 4
NULL,
};	
__align(2)const static u_char  SENSOR_WB_5[] = 
{
NULL
};
__align(2)const static u_char  SENSOR_WB_6[] = 
{
NULL
};

__align(2)const static u_char  SENSOR_NIGHT_MODE_ON[] = 
{ 
NULL,
};
__align(2)const static u_char  SENSOR_NIGHT_MODE_OFF[] = 
{
NULL,
};

__align(2)const static u_char  SENSOR_DEBAND_50[] = 
{
NULL,
};
__align(2)const static u_char  SENSOR_DEBAND_60[] = 
{
NULL,
};

__align(2)const static u_char  SENSOR_ROTATE_NOMAL[] = 
{
NULL,
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR[] = 
{
NULL,
};
__align(2)const static u_char  SENSOR_ROTATE_FLIP[] = 
{
NULL,
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_FLIP[] = 
{
NULL,
};

static u_short	Sensor_Effect(u_short EffectMode)
{
	return 0;
}

static u_short	Sensor_EV(u_short EvStep)
{
	return 0;
}


static u_short	Sensor_WhiteBalance(u_short WbMode)
{
	return 0;		
}

static u_short	Sensor_NightMode(mmp_sensor_night_mode NightMode)
{
	return 0;
}

static u_short	Sensor_DeBand(mmp_sensor_deband DeBandMode)
{
	return 0;		
}

static u_short	Sensor_Rotate(mmp_sensor_rotate Rotate)
{
	return 0;
}

static u_short 	Sensor_FrameRate(u_short FrameRate)
{
	return 0;
}

static void	Sensor_Enable(u_short enable)
{
	u_char retVal = 0;
	int i=50;
	AIT_Message_P1("GC0307 enable=%d\r\n", enable);
	
	if(enable)
	{
		A8L_VIF_Init(&preview_mode);
		sys_IF_ait_delay1ms(30);
	    //gbSensorInited = 1;    		
	}
	else
	{
		A8L_SetSensorResetPin(0);
		sys_IF_ait_delay1ms(1);

		//Turn off Sensor MCLK
		A8L_SensorClockOutput(A8_OFF);

		A8L_SetSensorEnablePin(0);	
		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_OFF);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_OFF);	
		A8L_SetSensorPowerPin(0);		    			        	
		//A800_ClearGPIO(AIT_GPIO_SENSOR_POWER_CTL);    
		gbSensorInited = 0;	
	}	
	
	return;
}
static u_short	Get_Sensor_ID(u_short*	SensorID)
{
	return 0;
}

static u_short ChangeSensorAE(void)
{
	return A8_NO_ERROR;
}

static u_short SetSensorResolution(mmp_sensor_mode sensor_mode)
{
	u_short	*tab_addr,tab_len;
    u_short fRet = 0;
	
	switch(sensor_mode)
	{
		case	SENSOR_INIT:
                A8L_VIF_Init(&preview_mode); 
                A8L_SetSensorAttr(&preview_mode);            
				tab_addr = (u_short *)Sensor_Init;
				tab_len = sizeof(Sensor_Init);	
				camsensor_tlg1120_init();
                gbSensorInited = 1;
                return fRet;
			break;	
		case	SENSOR_DSC_PREVIEW:
				tab_addr = (u_short *)Sensor_Init;
				tab_len = sizeof(Sensor_Init);				
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
    if(!fRet)
    	return	A8L_SendSensorSettingTable(tab_addr,tab_len);
    else
        return  fRet;
}

static t_sensor_mode_attr	preview_mode =
{
	SENSOR_PREVIEW_VH_POLARITY,
	SENSOR_PREVIEW_LATCH_COUNT,
	SENSOR_PREVIEW_YUV_FORMAT,
	SENSOR_LATCH_EDGE,
	1,
	1,
	SENSOR_PREVIEW_WIDTH,
	SENSOR_PREVIEW_HEIGHT,
	0,
	SENSOR_PREVIEW_LATCH_COUNT
};

static t_sensor_mode_attr	full_mode =
{
	SENSOR_FULL_VH_POLARITY,
	SENSOR_FULL_LATCH_COUNT,
	SENSOR_FULL_YUV_FORMAT,
	SENSOR_LATCH_EDGE,
	1,
	1,
	SENSOR_FULL_WIDTH,
	SENSOR_FULL_HEIGHT,
	0,
	SENSOR_FULL_LATCH_COUNT 
};

t_sensor_manager sensor_tlg1120 = 
{
    0xFF,
	0xFF,
	0xFF,
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


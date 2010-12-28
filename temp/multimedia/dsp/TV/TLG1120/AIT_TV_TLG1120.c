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
#define	SENSOR_PREVIEW_WIDTH  	    		360
#define	SENSOR_PREVIEW_HEIGHT  	    		288
#define	SENSOR_FULL_WIDTH  		    		360
#define	SENSOR_FULL_HEIGHT  	    		288

extern boolean camsensor_tlg1120_init(void);


static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;


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
	if(enable)
	{
		A8L_VIF_Init(&preview_mode);
		sys_IF_ait_delay1ms(30);   		
	}
	else
	{  
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
    u_short fRet = 0;
	
	switch(sensor_mode)
	{
		case SENSOR_INIT:
		{
            A8L_VIF_Init(&preview_mode); 
            A8L_SetSensorAttr(&preview_mode);     
			MSG_FATAL(" SetSensorResolution WIDTH=%d,HEIGHT=%d",preview_mode.image_width,preview_mode.image_height,0);
			camsensor_tlg1120_init();
            gbSensorInited = 1;
            return fRet;
			break;	
		}
		
		default:				
            break;			
	}
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

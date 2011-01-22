#include "A8_common.h"
#include "AIT700_EBIbus.h"
#include "A8_sensor.h"
#include "a8_vif.h"
#include "cam_module.h"
#include "AIT_Interface.h"

#define	SENSOR_ID					(0x3703)

#define	SENSOR_I2C_TYPE				(A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_FULL_VH_POLARITY     (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_PREVIEW_LATCH_COUNT  (A8_PHASE_COUNTER_NUM_3)
#define SENSOR_FULL_LATCH_COUNT     (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_PREVIEW_YUV_FORMAT   (A8_SENSOR_CRYCBY)//(A8_SENSOR_CBYCRY) //(A8_SENSOR_YCBYCR) // check BYD
#define SENSOR_FULL_YUV_FORMAT      (A8_SENSOR_CRYCBY)//(A8_SENSOR_CBYCRY) //(A8_SENSOR_YCBYCR) // check BYD
#define SENSOR_LATCH_EDGE           (A8_SENSOR_POS_EDGE)

#define	SENSOR_I2C_ID  			    (0xDC)
#define	SENSOR_PREVIEW_WIDTH  	    (640)
#define	SENSOR_PREVIEW_HEIGHT  	    (480)
#define	SENSOR_FULL_WIDTH  		    (640)
#define	SENSOR_FULL_HEIGHT  	    (480)

static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;

__align(2)const static u_char Sensor_Init[]=
{
	0x11,0x80,
	0x09,0x00,
	0x13,0x00,
	0x01,0x15,
	0x02,0x24,	//0x22
	0x8c,0x02, //01 :devided by 2  02 :devided by 1
	0x8d,0x64, //0x7c//32: devided by 2  64 :devided by 1
	0x87,0x18,
	0x13,0x07,
          
	0x15,0x00,
	0x3a,0x03,	//YUV 
	//black level ,对上电偏绿有改善
	/*
	0x05,0x1f,
	0x06,0x60,
	0x14,0x1f,
	0x06,0xe0,
	*/
	//lens shading
	0x35,0x50,
	0x65,0x53,
	0x66,0x50,
	0x36,0x05,
	0x37,0xf6,
	0x38,0x46,
	0x9b,0xff,
	0x9c,0x46,
	0xbc,0x01,
	0xbd,0xff,
	0xbe,0x46,

	//AE
	0x82,0x14,
	0x83,0x23,
	0x9a,0x23,//the same as 0x83
	0x84,0x20,
	0x85,0x26,
	0x89,0x02,//01 :devided by 2  02 :devided by 1
	0x8a,0x64,//32: devided by 2  64 :devided by 1
	0x86,0x28,
	0x96,0xa6,//AE speed
	0x97,0x0c,//AE speed
	0x98,0x18,//AE speed
	//AE target
	0x24,0x78,
	0x25,0x88,
	0x94,0x0a,//INT_OPEN  
	0x80,0x55,

	//denoise 
	0x70,0x6f,//denoise
	0x72,0x4f,//denoise
	0x73,0x2f,//denoise
	0x74,0x27,//denoise
	0x7a,0x4e,//denoise in  low light,0x8e\0x4e\0x0e
	0x7b,0x28,//the same as 0x86

	//black level
	0X1F,0x20,//G target
	0X22,0x20,//R target
	0X26,0x20,//B target
	//模拟部分参数
	0X16,0x00,//如果觉得黑色物体不够黑，有点偏红，将0x16写为0x03会有点改善	
	0xbb,0x20,  // deglitch
	0xeb,0x30,
	0xf5,0x21,
	0xe1,0x3c,
	0xbb,0x20,
	0X2f,0X66,
	0x06,0xe0,	

	//anti black sun spot
	0x61,0xd3,//0x61[3]=0 black sun disable
	0x79,0x48,//0x79[7]=0 black sun disable

	//Gamma

	0x3b,0x60,//auto gamma offset adjust in  low light  
	0x3c,0x10,//auto gamma offset adjust in  low light  
	0x56,0x40,
	0x39,0x80,  
	//gamma1
	0x3f,0xb0,
	0X40,0X88,
	0X41,0X74,
	0X42,0X5E,
	0X43,0X4c,
	0X44,0X44,
	0X45,0X3E,
	0X46,0X39,
	0X47,0X35,
	0X48,0X31,
	0X49,0X2E,
	0X4b,0X2B,
	0X4c,0X29,
	0X4e,0X25,
	0X4f,0X22,
	0X50,0X1F,

	/* gamma2 高亮
	0x3f,0xa8,
	0X40,0X9b,
	0X41,0X88,
	0X42,0X6e,
	0X43,0X59,
	0X44,0X4d,
	0X45,0X45,
	0X46,0X3e,
	0X47,0X39,
	0X48,0X35,
	0X49,0X31,
	0X4b,0X2e,
	0X4c,0X2b,
	0X4e,0X26,
	0X4f,0X23,
	0X50,0X1F,

	*/
	/*//gamma3 清晰亮丽
	0X3f,0Xa0,
	0X40,0X60,
	0X41,0X60,
	0X42,0X66,
	0X43,0X57,
	0X44,0X4c,
	0X45,0X43,
	0X46,0X3c,
	0X47,0X37,
	0X48,0X32,
	0X49,0X2f,
	0X4b,0X2c,
	0X4c,0X29,
	0X4e,0X25,
	0X4f,0X22,
	0X50,0X20,

	//gamma 4	low noise	
	0X3f,0Xa0,
	0X40,0X48,
	0X41,0X54,
	0X42,0X4E,
	0X43,0X44,
	0X44,0X3E,
	0X45,0X39,
	0X46,0X34,
	0X47,0X30,
	0X48,0X2D,
	0X49,0X2A,
	0X4b,0X28,
	0X4c,0X26,
	0X4e,0X22,
	0X4f,0X20,
	0X50,0X1E,
	*/		

	//color matrix
	//艳丽
	/*
	0x51,0x08,
	0x52,0x0E,
	0x53,0x06,
	0x54,0x12,
	0x57,0x82,
	0x58,0x70,
	0x59,0x5C,
	0x5a,0x77,
	0x5b,0x1B,
	0x5c,0x0e,//0x5c[3:0] low light color coefficient，smaller ,lower noise
	0x5d,0x95,
	*/
	//适中
	0x51,0x06,
	0x52,0x16,
	0x53,0x10,
	0x54,0x11,
	0x57,0x62,
	0x58,0x51,
	0x59,0x49,
	0x5a,0x65,
	0x5b,0x1c,
	0x5c,0x0e,//0x5c[3:0] low light color coefficient，smaller ,lower noise
	0x5d,0x95,
	

	/* //淡
	0x51,0x03,
	0x52,0x0d,
	0x53,0x0b,
	0x54,0x14,
	0x57,0x59,
	0x58,0x45,
	0x59,0x41,
	0x5a,0x5f,
	0x5b,0x1e,
	0x5c,0x0e,//0x5c[3:0] low light color coefficient，smaller ,lower noise
	0x5d,0x95,
	*/

	0x60,0x20,//color open in low light 
	//AWB
	0x6a,0x81,
	0x23,0x66,//Green gain
	0xa0,0x03,

	0xa1,0X31,
	0xa2,0X0e,
	0xa3,0X26,
	0xa4,0X0d,
	0xa5,0x23,
	0xa6,0x06,
	0xa7,0x81,//BLUE Target
	0xa8,0x7f,//RED Target
	0xa9,0x1e,
	0xaa,0x23,
	0xab,0x1e,
	0xac,0x3c,
	0xad,0xf0,
	0xc8,0x19,
	0xc9,0x20,
	0xca,0x18,
	0xcb,0x1f,
	0xaf,0x00,		
	0xc5,0x19,	
	0xc6,0x00,
	0xc7,0x20,	
	0xae,0x81,
	0xcd,0x90,
	0xee,0x4c,// P_TH

	// color saturation
	0xb0,0xd0,
	0xb1,0xc8,
	0xb2,0xb8,
	0xb3,0x8a,

	//anti webcamera banding 
 	0x9d,0x4c,	//0x69
	//switch direction
	0x1e,0x00,//00:normal  10:IMAGE_V_MIRROR   20:IMAGE_H_MIRROR  30:IMAGE_HV_MIRROR
};

static u_short	Sensor_Effect(u_short EffectMode)
{
	u_short result=0;

	switch(EffectMode)
	{
		case DSC_GRAYSCALE:   
		{
			A8L_SetSenReg(0x80,0x45);
			A8L_SetSenReg(0x76,0x00);
			A8L_SetSenReg(0x69,0x20);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		case DSC_SEPIA:
		{
			A8L_SetSenReg(0x80,0x45);
			A8L_SetSenReg(0x76,0x00);
			A8L_SetSenReg(0x69,0x20);
			A8L_SetSenReg(0x67,0x60);
			A8L_SetSenReg(0x68,0x98);
			break;
		}
		
		case DSC_SEPIA_GREEN:
		{
			A8L_SetSenReg(0x80,0x45);
			A8L_SetSenReg(0x76,0x00);
			A8L_SetSenReg(0x69,0x20);
			A8L_SetSenReg(0x67,0x40);
			A8L_SetSenReg(0x68,0x40);
			break;
		}
		
		case DSC_SEPIA_BLUE:
		{
			A8L_SetSenReg(0x80,0x45);
			A8L_SetSenReg(0x76,0x00);
			A8L_SetSenReg(0x69,0x20);
			A8L_SetSenReg(0x67,0xA0);
			A8L_SetSenReg(0x68,0x40);
			break;
		}
		
		case DSC_COLOR_INV:
		{
			A8L_SetSenReg(0x80,0x45);
			A8L_SetSenReg(0x76,0x00);
			A8L_SetSenReg(0x69,0x40);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		case DSC_GRAY_INV:
		{
			A8L_SetSenReg(0x80,0x45);
			A8L_SetSenReg(0x76,0x00);
			A8L_SetSenReg(0x69,0x20);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		case DSC_BLACKBOARD:
		{
			A8L_SetSenReg(0x69,0x00);
			A8L_SetSenReg(0x80,0xc5);
			A8L_SetSenReg(0x76,0xe0);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		case DSC_WHITEBOARD:	
		{
			A8L_SetSenReg(0x69,0x00);
			A8L_SetSenReg(0x80,0xc5);
			A8L_SetSenReg(0x76,0xf0);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		case DSC_COPPER_CARVING:
		{
			A8L_SetSenReg(0x69,0x00);
			A8L_SetSenReg(0x80,0xc5);
			A8L_SetSenReg(0x76,0xc0);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		case DSC_EMBOSSMENT:
		{
			A8L_SetSenReg(0x69,0x00);
			A8L_SetSenReg(0x80,0xc5);
			A8L_SetSenReg(0x77,0x80);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		case DSC_BLUE_CARVING:
		{
			A8L_SetSenReg(0x69,0x00);
			A8L_SetSenReg(0x80,0xc5);
			A8L_SetSenReg(0x76,0xd0);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		case DSC_CONTRAST:
		{
			A8L_SetSenReg(0x80,0x45);
			A8L_SetSenReg(0x76,0x00);
			A8L_SetSenReg(0x69,0x00);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		case DSC_SKETCH:
		{
			A8L_SetSenReg(0x69,0x00);
			A8L_SetSenReg(0x80,0xc5);
			A8L_SetSenReg(0x76,0xb0);
			A8L_SetSenReg(0x67,0x80);
			A8L_SetSenReg(0x68,0x80);
			break;
		}
		
		default:		
			break;				
	}

    return result;
}

static u_short	Sensor_EV(u_short EvStep)
{
	switch(EvStep)
	{
		case	DSC_EV_VALUE_NEG_4:
				A8L_SetSenReg(0x55, 0xF8);
				break;
				
		case	DSC_EV_VALUE_NEG_3:
				A8L_SetSenReg(0x55, 0xD8);
				break;
				
		case	DSC_EV_VALUE_NEG_2:
				A8L_SetSenReg(0x55, 0xB8);	
				break;
				
		case	DSC_EV_VALUE_NEG_1:
				A8L_SetSenReg(0x55, 0x98);	
				break;
				
		case	DSC_EV_VALUE_ZERO:
				A8L_SetSenReg(0x55, 0x00);	
				break;
				
		case	DSC_EV_VALUE_POS_1:
				A8L_SetSenReg(0x55, 0x18);		
				break;
				
		case	DSC_EV_VALUE_POS_2:
				A8L_SetSenReg(0x55, 0x38);	
				break;
				
		case	DSC_EV_VALUE_POS_3:
				A8L_SetSenReg(0x55, 0x58);	
				break;
				
		case	DSC_EV_VALUE_POS_4:
				A8L_SetSenReg(0x55, 0x78);	
				break;																																																		
		default:
				break;
	}
	
   	return	A8_NO_ERROR;
}


static u_short	Sensor_WhiteBalance(u_short WbMode)
{
    u_short fRet = 0;
    u_short  temp_reg;	

	A8L_GetSenReg(0x13, &temp_reg);
	
	switch(WbMode)
	{
		case	DSC_WB_AUTO:	//WB_AUTO
		{
			A8L_SetSenReg(0x01,0x15);
			A8L_SetSenReg(0x02,0x24);
			A8L_SetSenReg(0x6a,0x81);
			A8L_SetSenReg(0x23,0x66);
			A8L_SetSenReg(0x13,temp_reg|0x2);   // Enable AWB
			break;
		}
		
		case	DSC_WB_CLOUD:	//WB_CLOUD
		{
			A8L_SetSenReg(0x13,temp_reg&~0x2);  // Disable AWB				
			A8L_SetSenReg(0x01,0x10);
			A8L_SetSenReg(0x02,0x28);
			A8L_SetSenReg(0x6a,0x81);
			A8L_SetSenReg(0x23,0x66);
			break;
		}					

		case DSC_WB_DAYLIGHT:	//WB_DAYLIGHT
		{
			A8L_SetSenReg(0x13,temp_reg&~0x2);  // Disable AWB				
			A8L_SetSenReg(0x01,0x13);
			A8L_SetSenReg(0x02,0x26);
			A8L_SetSenReg(0x6a,0x81);
			A8L_SetSenReg(0x23,0x66);
			break;
		}
		
		case	DSC_WB_INCANDESCENCE:	//WB_INCANDESCENCE
		{
			A8L_SetSenReg(0x13,temp_reg&~0x2);  // Disable AWB				
			A8L_SetSenReg(0x01,0x1f);
			A8L_SetSenReg(0x02,0x15);
			A8L_SetSenReg(0x6a,0x81);
			A8L_SetSenReg(0x23,0x66);
			break;
		}
		
		case	DSC_WB_FLUORESCENT:	//WB_FLUORESCENT
		{
			A8L_SetSenReg(0x13,temp_reg&~0x2);  // Disable AWB				
			A8L_SetSenReg(0x01,0x1a);
			A8L_SetSenReg(0x02,0x1e);
			A8L_SetSenReg(0x6a,0x81);
			A8L_SetSenReg(0x23,0x66);
			break;
		}

		case	DSC_WB_TUNGSTEN:	//WB_TUNGSTEN
		{
			A8L_SetSenReg(0x13,temp_reg&~0x2);  // Disable AWB				
			A8L_SetSenReg(0x01,0x1a);
			A8L_SetSenReg(0x02,0x0d);
			A8L_SetSenReg(0x6a,0x81);
			A8L_SetSenReg(0x23,0x66);
			break;
		}
		
		case	DSC_WB_MANUAL:	//
		{
			break;
		}
		
		default:
				break;
	}	
	return	fRet;
}

static u_short	Sensor_NightMode(mmp_sensor_night_mode NightMode)
{
	switch(NightMode)
	{
		case	A8_SENSOR_NIGHT_MODE_OFF:		
				break;
		default:
				break;	
	}				
	
   	return	A8_NO_ERROR;	
}

static u_short	Sensor_DeBand(mmp_sensor_deband DeBandMode)
{
	switch(DeBandMode)
	{
		case	A8_SENSOR_DEBAND_60:
				break;
		default:
				break;		
	}
	
   	return	A8_NO_ERROR;		
}

static u_short	Sensor_Rotate(mmp_sensor_rotate Rotate)
{
    {
		switch(Rotate)
		{
			case	A8_SENSOR_ROTATE_MIRROR:
					break;
			case	A8_SENSOR_ROTATE_FILP:
					break;
			case	A8_SENSOR_ROTATE_MIRROR_FLIP:
					break;
			default:
					break;	
		}				
    }
		
   	return	A8_NO_ERROR;		
}

static u_short 	Sensor_FrameRate(u_short FrameRate)
{
}

static void	Sensor_Enable(u_short enable)
{
	if(enable) 
	{	
		A8L_VIF_Init(&preview_mode);	//

		AIT_Message_P0("===BF3603===BF3603 turn on \n\r");

		//sensor power LDO
		A8L_SetSensorPowerPin(1);
		A8L_SetSensorResetPin(1);
		sys_IF_ait_delay1ms(10);

		//sensor PWDN
		A8L_SetSensorEnablePin(0);
		sys_IF_ait_delay1ms(10);
		//Reset
		A8L_SetSensorResetPin(1); 
		sys_IF_ait_delay1ms(10);
		A8L_SetSensorResetPin(0);
		sys_IF_ait_delay1ms(20); 
		A8L_SetSensorResetPin(1); 
		sys_IF_ait_delay1ms(10);


		//Initial I2C I/F
		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
		sys_IF_ait_delay1ms(10);
	        		
	} 
	else
	{
		AIT_Message_P0("===BF3603===BF3603 turn off \n\r");	
		//ATV off

		A8L_SetSensorResetPin(0);
		sys_IF_ait_delay1ms(1);

		//Turn off Sensor MCLK
		A8L_SensorClockOutput(A8_OFF);
		SetA8RegB(VIF_SEN_CLK_CTL,0);

		A8L_SetSensorEnablePin(1);
		//AIT701_SUBSNESOR_DISABLE
		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_OFF);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_OFF);	
		A8L_SetSensorPowerPin(0);		    			        	
		//A800_ClearGPIO(AIT_GPIO_SENSOR_POWER_CTL);
		//A8L_SetSensorPowerPin(0); //Turn off sensor

		gbSensorInited=0;
	}
}

static u_short	Get_Sensor_ID(u_short*	SensorID)
{
	u_short	fRet ,i;
	if(!gbSensorInited)
	{
		Sensor_Enable(1);
	}

	for(i=0;i<3;i++)
	{
		A8L_GetSenReg(0xFC,&fRet);
		*SensorID = fRet << 8;
		A8L_GetSenReg(0xFD,&fRet);
		*SensorID += fRet;

		MSG_FATAL("SensorID = 0x%x",*SensorID,0,0);
		if (SENSOR_ID == *SensorID )
		{
			break;
		}
	}

	if(!gbSensorInited)
	{
		Sensor_Enable(0);
	}
	
	AIT_Message_P1(" BF3703 Get_Sensor_ID:0x%x\r\n",*SensorID);
	
	return	fRet;
}

static u_short SetSensorResolution(mmp_sensor_mode sensor_mode)
{
	u_short	*tab_addr,tab_len;
    u_short fRet = 0;
	switch(sensor_mode)
	{
		case	SENSOR_INIT:
        {
        	A8L_VIF_Init(&preview_mode); 
            A8L_SetSensorAttr(&preview_mode);
			tab_addr = (u_short *)Sensor_Init;
			tab_len = sizeof(Sensor_Init);
            gbSensorInited = 1;
            return	A8L_SendSensorSettingTable(tab_addr,tab_len);
            break;
        }
        
		case	SENSOR_DSC_PREVIEW:				
				break;
        case    SENSOR_VIDEO_PREVIEW:			
				break;            
		case	SENSOR_NORMAL_CAP_MODE:				
				break;
		case	SENSOR_FULL_RESOL_MODE:				
				break;
		case	SENSOR_SCALE_UP_MODE:				
                break;			
	}

	return  fRet;
	//return fRet;
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

t_sensor_manager sensor_bf3703 = 
{
    SENSOR_ID,
	SENSOR_I2C_ID,
	SENSOR_I2C_TYPE,
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

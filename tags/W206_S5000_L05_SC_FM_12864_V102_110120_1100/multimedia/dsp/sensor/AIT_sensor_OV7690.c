#include	"A8_common.h"
#include	"AIT700_EBIbus.h"
#include	"A8_sensor.h"
#include "A8_vif.h"
#if 1//defined(SENSOR_OV7670)
#define	SENSOR_ID					(0x7691)//	(0x7691)
#define SENSOR_ID2					(0x7690)
#define	SENSOR_I2C_TYPE				(A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_FULL_VH_POLARITY     (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_PREVIEW_LATCH_COUNT  (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_FULL_LATCH_COUNT     (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_PREVIEW_YUV_FORMAT   (A8_SENSOR_YCBYCR) //(A8_SENSOR_CBYCRY)
#define SENSOR_FULL_YUV_FORMAT      (A8_SENSOR_YCBYCR) //(A8_SENSOR_CBYCRY)
#define SENSOR_LATCH_EDGE           (A8_SENSOR_POS_EDGE)

#define	SENSOR_I2C_ID  			    (0x42)
#define	SENSOR_PREVIEW_WIDTH  	    (640)
#define	SENSOR_PREVIEW_HEIGHT  	    (480)
#define	SENSOR_FULL_WIDTH  		    (640)
#define	SENSOR_FULL_HEIGHT  	    (480)

static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;


#define	Sunny_F28
__align(2)const static u_char	Sensor_Init[]=
{
0x0c,0x16,

0x48,0x44,
0x49,0x0d,//;releated IO voltage.

0x41,0x43,
0x81,0xff,
0x21,0x57,//BD step	
0x16,0x03,
0x39,0x80,

//;;===Format===;;
0x11,0x00,//13M pclk
0x12,0x00,
0x82,0x03,
0xd0,0x48,
0x80,0x7f,
0x3e,0x30,
0x22,0x00,


0x2a,0x30,
0x2b,0x0c,                          
0x2c,0x2b, 
0x50,0x53,//50hz banding filter value
0x51,0x45,//60hz banding filter value
0x21,0x57,//step
0x15,0x04,//auto down to 1/3 frame rate, 15fps~5fps 

//;;===Resolution===;;

0x17,0x69,
0x18,0xa4,
0x19,0x0c,
0x1a,0xf6,
                           
0xc8,0x02,
0xc9,0x80,//ISP input hsize (640)
0xca,0x01,
0xcb,0xe0,//ISP input vsize (480)
                           
0xcc,0x02,
0xcd,0x80,//ISP output hsize (640)
0xce,0x01,
0xcf,0xe0,//ISP output vsize (480)
                           
//;;===Lens Correction==;
/*
0x85,0x90,  //sunny
0x86,0x18,
0x87,0xb0,
0x88,0xA0,
0x89,0x24,
0x8a,0x1f,
0x8b,0x21,
*/
0x85,0x10,  //TRULLY AND BYD
0x86,0x00,
0x87,0x1b,
0x88,0xAF,
0x89,0x21,
0x8a,0x20,
0x8b,0x20,
                           
//;;====Color Matrix======
                           
0xbb,0xac,
0xbc,0xae,
0xbd,0x02,
0xbe,0x1f,
0xbf,0x93,
0xc0,0xb1,		
0xc1,0x1A,
                           
//;;===Edge + Denoise====;
                           
0xb4,0x06,
0xb7,0x07,
0xb8,0x06,
0xb9,0x00,
0xba,0x40,

//;;====AEC/AGC target===
 
0x24,0x90,
0x25,0x80,
0x26,0xB4,
                           
//;;=====UV adjust======; 
0x81,0xff,
0x5A,0x10,
0x5B,0xA1,
0x5C,0x3a,
0x5d,0x20,

//;;====Gamma====;;

0xa3,0x04,
0xa4,0x09,
0xa5,0x18,
0xa6,0x38,
0xa7,0x47,
0xa8,0x56,
0xa9,0x66,
0xaa,0x74,
0xab,0x7f,
0xac,0x89,
0xad,0x9a,
0xae,0xa9,
0xaf,0xc4,
0xb0,0xdb,
0xb1,0xee,
0xb2,0x18,

//;;==Advance AWB==;;

0x8c, 0x5c,
0x8d, 0x11,
0x8e, 0x12,
0x8f, 0x19,
0x90, 0x50,
0x91, 0x20,
0x92, 0x99,
0x93, 0x91,
0x94, 0x0f,
0x95, 0x13,
0x96, 0xff,
0x97, 0x00,
0x98, 0x38,
0x99, 0x33,
0x9a, 0x4f,
0x9b, 0x43,
0x9c, 0xf0,
0x9d, 0xf0,
0x9e, 0xf0,
0x9f, 0xff,
0xa0, 0x60,
0xa1, 0x5a,
0xa2, 0x10,

                           
0x14,0x20,//8x gain ceiling, PPChrg off

0xd8,0x40,//saturation
0xd9,0x40,
0xd2,0x00,

0x13,0xf7,

};

__align(2)const static u_char	Sensor_DSC_Preview[] =
{
NULL
};
__align(2)const static u_char	Sensor_Video_Preview[]=
{
NULL
};
__align(2)const static u_char	Sensor_Normal_Capture[] = 
{
NULL
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
	0x28, 0x00,
	0xD2, 0x00,
	0xDA, 0x80,
	0xDB, 0x80,
};
__align(2)const static u_char  SENSOR_EFFECT_1[] = 
{	//BW
	0x28, 0x00,
	0xD2, 0x18,
	0xDA, 0x80,
	0xDB, 0x80,
};
__align(2)const static u_char  SENSOR_EFFECT_2[] = 
{	// Bluish 2
	0x28, 0x00,
	0xD2, 0x18,
	0xDA, 0x40,
	0xDB, 0xA0,
};
__align(2)const static u_char  SENSOR_EFFECT_3[] = 
{	// Negative 3
		0x28, 0x00,
		0xD2, 0x18,
		0xDA, 0x60,
		0xDB, 0x60,
};
__align(2)const static u_char  SENSOR_EFFECT_4[] = 
{	// Reddish 4
		0x28, 0x00,
		0xD2, 0x18,
		0xDA, 0xA0,
		0xDB, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_5[] = 
{	// Antique 5
		0x28, 0x80,
		0xD2, 0x00,
		0xDA, 0x80,
		0xDB, 0x80,
};
__align(2)const static u_char  SENSOR_EFFECT_6[] = 
{	// Greenish 6
		0x28, 0x00,
		0xD2, 0x18,
		0xDA, 0x80,
		0xDB, 0xC0,
};
__align(2)const static u_char  SENSOR_EFFECT_7[] = 
{
NULL
};

__align(2)const static u_char  SENSOR_EV_0[] = 
{	// EV -2
	0x24, 0x50,
	0x25, 0x40,
	0x26, 0x73,
};
__align(2)const static u_char  SENSOR_EV_1[] = 
{	// EV -1.5
	0x24, 0x60,
	0x25, 0x50,
	0x26, 0x83,
};
__align(2)const static u_char  SENSOR_EV_2[] = 
{	// EV -1
	0x24, 0x70,
	0x25, 0x60,
	0x26, 0x94,
};
__align(2)const static u_char  SENSOR_EV_3[] = 
{	// EV -0.5
	0x24, 0x80,
	0x25, 0x70,
	0x26, 0xa5,
};
__align(2)const static u_char  SENSOR_EV_4[] = 
{	// EV 0
	0x24, 0x90,
	0x25, 0x88,
	0x26, 0xb5,
};
__align(2)const static u_char  SENSOR_EV_5[] = 
{	// EV +0.5
	0x24, 0xa0,
	0x25, 0x90,
	0x26, 0xc6,
};
__align(2)const static u_char  SENSOR_EV_6[] = 
{	// EV +1
	0x24, 0xb0,
	0x25, 0xa0,
	0x26, 0xd6,
};
__align(2)const static u_char  SENSOR_EV_7[] = 
{	// EV +1.5
	0x24, 0xc0,
	0x25, 0xb0,
	0x55, 0xe6,
};
__align(2)const static u_char  SENSOR_EV_8[] = 
{	// EV +2
	0x24, 0xd0,
	0x25, 0xc0,
	0x26, 0xf8,
};

__align(2)const static u_char  SENSOR_WB_AUTO[] = 
{	// Auto 0
	0x13, 0xf7,
};	
__align(2)const static u_char  SENSOR_WB_1[] = 
{	// Sunny 1
	0x13, 0xf5,
	0x01, 0x43,
	0x02, 0x5d,
};	
__align(2)const static u_char  SENSOR_WB_2[] = 
{	// Cloudy 2
	0x13, 0xf5,
	0x01, 0x43,
	0x02, 0x5d,
};	
__align(2)const static u_char  SENSOR_WB_3[] = 
{	// Indoor home 3
	0x13, 0xf5,
	0x01, 0x66,
	0x02, 0x40,

};	
__align(2)const static u_char  SENSOR_WB_4[] = 
{	// Indoor office 4
	0x13, 0xf5,
	0x01, 0x56,
	0x02, 0x50,

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
  0x15, 0x04,
};
__align(2)const static u_char  SENSOR_NIGHT_MODE_OFF[] = 
{
  0x15, 0x04,
};

__align(2)const static u_char  SENSOR_DEBAND_50[] = 
{
 0x14,0x21,
};
__align(2)const static u_char  SENSOR_DEBAND_60[] = 
{
 0x14,0x20,
};

__align(2)const static u_char  SENSOR_ROTATE_NOMAL[] = 
{
0x0c,0x16,
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR[] = 
{
0x0c,0x56,

};
__align(2)const static u_char  SENSOR_ROTATE_FLIP[] = 
{
0x0c,0x96,

};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_FLIP[] = 
{
0x0c,0xd6,

};

static u_short	Sensor_Effect(u_short EffectMode)
{
	u_short	*tab_addr,tab_len;
	u_char fRet = 0;
    A8L_CheckSensorFrame(VIF_FRAME_END); 
    SetA8RegB(VIF_DATA_OUT_EN,0);	
	switch(EffectMode)
	{
		case	1:
				tab_addr = (u_short*)SENSOR_EFFECT_1;
				tab_len = sizeof(SENSOR_EFFECT_1);
				break;
		case	2:
				tab_addr = (u_short*)SENSOR_EFFECT_2;
				tab_len = sizeof(SENSOR_EFFECT_2);
				break;
		case	3:
				tab_addr = (u_short*)SENSOR_EFFECT_3;
				tab_len = sizeof(SENSOR_EFFECT_3);
				break;
		case	4:
				tab_addr = (u_short*)SENSOR_EFFECT_4;
				tab_len = sizeof(SENSOR_EFFECT_4);
				break;
		case	5:
				tab_addr = (u_short*)SENSOR_EFFECT_5;
				tab_len = sizeof(SENSOR_EFFECT_5);
				break;	
		case	6:
				tab_addr = (u_short*)SENSOR_EFFECT_6;
				tab_len = sizeof(SENSOR_EFFECT_6);
				break;																			
		default:
				tab_addr = (u_short*)SENSOR_EFFECT_NORMAL;
				tab_len = sizeof(SENSOR_EFFECT_NORMAL);
				break;				
	}

   		fRet = 	A8L_SendSensorSettingTable(tab_addr,tab_len);
		A8L_CheckFrameStart();
		SetA8RegB(VIF_DATA_OUT_EN,VIF_EN_DATA_OUT);	
		return fRet;
}

static u_short	Sensor_EV(u_short EvStep)
{
	u_short	*tab_addr,tab_len;
   // u_short fRet = 0;
	switch(EvStep)
	{
		case	0:
				tab_addr = (u_short*)SENSOR_EV_0;
				tab_len = sizeof(SENSOR_EV_0);		
				break;	
		case	1:
				tab_addr = (u_short*)SENSOR_EV_1;
				tab_len = sizeof(SENSOR_EV_1);		
				break;	
		case	2:
				tab_addr = (u_short*)SENSOR_EV_2;
				tab_len = sizeof(SENSOR_EV_2);		
				break;	
		case	3:
				tab_addr = (u_short*)SENSOR_EV_3;
				tab_len = sizeof(SENSOR_EV_3);		
				break;	
		case	4:
				tab_addr = (u_short*)SENSOR_EV_4;
				tab_len = sizeof(SENSOR_EV_4);		
				break;	
		case	5:
				tab_addr = (u_short*)SENSOR_EV_5;
				tab_len = sizeof(SENSOR_EV_5);		
				break;	
		case	6:
				tab_addr = (u_short*)SENSOR_EV_6;
				tab_len = sizeof(SENSOR_EV_6);		
				break;	
		case	7:
				tab_addr = (u_short*)SENSOR_EV_7;
				tab_len = sizeof(SENSOR_EV_7);		
				break;	
		case	8:
				tab_addr = (u_short*)SENSOR_EV_8;
				tab_len = sizeof(SENSOR_EV_8);		
				break;																																																			
		default:
				tab_addr = (u_short*)SENSOR_EV_4;
				tab_len = sizeof(SENSOR_EV_4);
				break;	
	}	
	
   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);					
}


static u_short	Sensor_WhiteBalance(u_short WbMode)
{
	u_short	*tab_addr,tab_len;
   // u_short fRet = 0;
	switch(WbMode)
	{
		case	0:
				tab_addr = (u_short*)SENSOR_WB_AUTO;
				tab_len = sizeof(SENSOR_WB_AUTO);		
				break;	
		case	1:
				tab_addr = (u_short*)SENSOR_WB_1;
				tab_len = sizeof(SENSOR_WB_1);		
				break;	
		case	2:
				tab_addr = (u_short*)SENSOR_WB_2;
				tab_len = sizeof(SENSOR_WB_2);		
				break;	
		case	3:
				tab_addr = (u_short*)SENSOR_WB_3;
				tab_len = sizeof(SENSOR_WB_3);		
				break;	
		case	4:
				tab_addr = (u_short*)SENSOR_WB_4;
				tab_len = sizeof(SENSOR_WB_4);		
				break;	
		case	5:
				tab_addr = (u_short*)SENSOR_WB_5;
				tab_len = sizeof(SENSOR_WB_5);		
				break;	
		case	6:
				tab_addr = (u_short*)SENSOR_WB_6;
				tab_len = sizeof(SENSOR_WB_6);		
				break;	;																																																			
		default:
				tab_addr = (u_short*)SENSOR_WB_AUTO;
				tab_len = sizeof(SENSOR_WB_AUTO);
				break;	
	}	
	
   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);		
}

static u_short	Sensor_NightMode(mmp_sensor_night_mode NightMode)
{
	u_short	*tab_addr,tab_len;
	switch(NightMode)
	{
		case	A8_SENSOR_NIGHT_MODE_OFF:
				tab_addr = (u_short*)SENSOR_NIGHT_MODE_OFF;
				tab_len = sizeof(SENSOR_NIGHT_MODE_OFF);		
				break;	;																																																			
		default:
				tab_addr = (u_short*)SENSOR_NIGHT_MODE_ON;
				tab_len = sizeof(SENSOR_NIGHT_MODE_ON);
				break;	
	}				
	
   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);	
}

static u_short	Sensor_DeBand(mmp_sensor_deband DeBandMode)
{
	u_short	*tab_addr,tab_len;

	switch(DeBandMode)
	{
		case	A8_SENSOR_DEBAND_60:
				tab_addr = (u_short*)SENSOR_DEBAND_60;
				tab_len = sizeof(SENSOR_DEBAND_60);		
				break;	;																																																			
		default:
				tab_addr = (u_short*)SENSOR_DEBAND_50;
				tab_len = sizeof(SENSOR_DEBAND_50);
				break;		
	}
	
   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);		
}

static u_short	Sensor_Rotate(mmp_sensor_rotate Rotate)
{
	u_short	*tab_addr,tab_len;
    return A8_NO_ERROR; 
	switch(Rotate)
	{
		case	A8_SENSOR_ROTATE_MIRROR:
				tab_addr = (u_short*)SENSOR_ROTATE_MIRROR;
				tab_len = sizeof(SENSOR_ROTATE_MIRROR);		
				break;	
		case	A8_SENSOR_ROTATE_FILP:
				tab_addr = (u_short*)SENSOR_ROTATE_FLIP;
				tab_len = sizeof(SENSOR_ROTATE_FLIP);		
				break;	
		case	A8_SENSOR_ROTATE_MIRROR_FLIP:
				tab_addr = (u_short*)SENSOR_ROTATE_MIRROR_FLIP;
				tab_len = sizeof(SENSOR_ROTATE_MIRROR_FLIP);		
				break;																																																											
		default:
				tab_addr = (u_short*)SENSOR_ROTATE_NOMAL;
				tab_len = sizeof(SENSOR_ROTATE_NOMAL);
				break;	
	}				
	
   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);		
}

static u_short 	Sensor_FrameRate(u_short FrameRate)
{return 0;}

static void	Sensor_Enable(u_short enable)
{
	if(enable) {
		A8L_VIF_Init(&preview_mode);
	    //sensor power LDO
	    A8L_SetSensorPowerPin(1);
	    sys_IF_ait_delay1ms(1);

	    //sensor PWDN
	    A8L_SetSensorEnablePin(0);
	    sys_IF_ait_delay1ms(1);  
		
//		A810H_PresetSensor(0, SENSOR_I2C_ID, SENSOR_I2C_TYPE, 3);
//		A810H_SetPinPull(0x69D1, PIN_PULL_HIGH);	//PSEN pull up
//		A810H_SetPinPull(0x69D2, PIN_PULL_HIGH);	//PSDA pull up
//		A810H_SetPinPull(0x69D3, PIN_PULL_HIGH);	//PSCK pull up
//		AITS_I2C_Init();

        A8L_SetSensorResetPin(1); 
        sys_IF_ait_delay1ms(1);
        A8L_SetSensorResetPin(0);
        sys_IF_ait_delay1ms(1); 
        A8L_SetSensorResetPin(1); 
	//Initial I2C I/F
	A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
	A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
	sys_IF_ait_delay1ms(1);

	/// Set OV7670's COM7: Reset all register and set to default value
	if (A8L_SetSenReg(0x12, 0x80))
	{
//		return 1;
	}
	sys_IF_ait_delay1ms(10);
	} else {
#if 0	
		SetA8RegB(0x7150,GetA8RegB(0x7150)|0x07);
		sys_IF_ait_delay1ms(100);	
		//Set to low first
		//If hardware sensor power can not disable, it is better to set SCK and SDA to high or floating
		SetA8RegB(0x7151,GetA8RegB(0x7151)&(~0x07));
		
		//A810H_SetRegB(0x6050,A810H_GetRegB(0x6050)|0x01);
		//A810H_SetRegB(0x6051,A810H_GetRegB(0x6051)&0xFE);
		sys_IF_ait_delay1ms(1);
		SetA8RegB(0x7151,GetA8RegB(0x7151)|0x01);
		sys_IF_ait_delay1ms(1);
	    //sensor PWDN
    	A8L_SetSensorEnablePin(1);
        sys_IF_ait_delay1ms(10);
	    //sensor power LDO
	    A8L_SetSensorPowerPin(0);
	    sys_IF_ait_delay1ms(10);        
		gbSensorInited = 0;	
#else
        A8L_SetSensorResetPin(0);
	    sys_IF_ait_delay1ms(1);  
	SetA8RegB(VIF_SEN_CLK_CTL,0);		
	    //sensor PWDN
	    A8L_SetSensorEnablePin(1);
        
	    sys_IF_ait_delay1ms(1);  	
		//For power consumption issue
		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);		
		gbSensorInited = 0;	
#endif
	}	
}
static u_short	Get_Sensor_ID(u_short*	SensorID)
{
	u_short	fRet;
	if(!gbSensorInited)
		Sensor_Enable(1);
	A8L_GetSenReg(0x1c,&fRet);
	*SensorID = fRet << 8;
	A8L_GetSenReg(0x1d,&fRet);
	*SensorID += fRet;
	if(!gbSensorInited)
		Sensor_Enable(0);
	return	fRet;
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


t_sensor_manager sensor_ov7690 = 
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
#endif

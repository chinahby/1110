#include	"A8_common.h"
#include	"AIT700_EBIbus.h"
#include	"A8_sensor.h"
#include "A8_vif.h"

#if 1//defined(SENSOR_OV7680)
#define	SENSOR_ID					(0x7680)
#define	SENSOR_I2C_TYPE				(A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)

#define SENSOR_FULL_VH_POLARITY     (A8_SENSOR_HOR_NEG|A8_SENSOR_VER_POS)
#define SENSOR_PREVIEW_LATCH_COUNT  (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_FULL_LATCH_COUNT     (A8_PHASE_COUNTER_NUM_2)
//#define SENSOR_PREVIEW_YUV_FORMAT   (A8_SENSOR_YCBYCR) //(A8_SENSOR_CBYCRY)
#define SENSOR_PREVIEW_YUV_FORMAT   (A8_SENSOR_CBYCRY)
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
__align(2)const static u_char	Sensor_Init[]=
{

	0x29, 0xA2,		// 20080529, test clock setting.
	0x1e, 0x91,
	0x14, 0xA2,
	0x15, 0xC8,
	0x63, 0x0b,
	0x64, 0x0f,	
	0x65, 0x07,
	0x12, 0x01,
	0x0c, 0x06, //0xC6 -> 0x46 // b7:v-flip,b6:h-mirror,b4:y0<->y1,
	0x16, 0x08,
	0x82, 0x9a,
	0x17, 0x1a, // HStart MSB:b2~b9
	0x18, 0xA4, // HSize reg*4
	0x19, 0x03, // VStart MSB:b2~b9
	0x1a, 0xF2, // VSize reg*2
	0x22, 0x40,
	0x23, 0x20,
	0x31, 0x11,
	0x40, 0x20,
	0x4a, 0x42,
	0x4b, 0x42,
	0x67, 0x50,
	0x6b, 0x00,
	0x6c, 0x00,
	0x6d, 0x00,
	0x6e, 0x00,
	0x42, 0x4a,
	0x45, 0x40,
	0x3f, 0x46,
	0x48, 0x20,
	0x21, 0x76,
	0x5a, 0x10,
	0x5b, 0xdf,
	0x5c, 0xe0,
	0x27, 0x33,
	0x4d, 0x2d,
	0x42, 0x4a,
	0x65, 0x87,		// 20080528, mirror option need to sync with reg_0x0c
	0x66, 0x02,
	0x67, 0x5c,
	0x31, 0x05,
	0x32, 0x8f,
	0x33, 0x9f,
	0x34, 0x23,
	0x35, 0x00,
	0x36, 0x23,
	0x37, 0x23,
	0x2f, 0x0c,
	0x30, 0x10,
	0x38, 0x84,
	0x27, 0xb3,
	0x40, 0x23,
	0x4d, 0x2d,
	0x4e, 0x24,
	0x55, 0x86,
	0x58, 0x83,
	0x13, 0xff,
	0x38, 0x84,
	0x4f, 0x4c,
	0x59, 0x0e,
	0x5f, 0x20,
	0x56, 0x40,
	0x51, 0x00,
	0x57, 0x22,
	0x59, 0x07,
	0x5f, 0xa3,
	0x59, 0x08,
	0x5f, 0x54,
	0x52, 0x30,
	0x56, 0x29,
	0x53, 0x20,
	0x54, 0x30,
	0x59, 0x00,
	0x5f, 0xf0,
	0x59, 0x01,
	0x5f, 0xf0,
	0x59, 0x0f,
	0x5f, 0x20,
	0x5f, 0x00,
	0x59, 0x10,
	0x5f, 0x7e,
	0x59, 0x0a,
	0x5f, 0x80,
	0x59, 0x0b,
	0x5f, 0x01,
	0x59, 0x0c,
	0x5f, 0x07,
	0x5f, 0x0f,
	0x59, 0x0d,
	0x5f, 0x20,
	0x59, 0x09,
	0x5f, 0x30,
	0x59, 0x02,
	0x5f, 0x80,
	0x59, 0x03,
	0x5f, 0x60,
	0x59, 0x04,
	0x5f, 0xf0,
	0x59, 0x05,
	0x5f, 0x80,
	0x59, 0x06,
	0x5f, 0x04,
	0x59, 0x26,
	0x59, 0x0b,
	0x5f, 0x31,
	0x55, 0xa8,
	0x11, 0x00,//0x11, 0x01, // pll
	0x81, 0x06,
	0x12, 0x00,
	0x82, 0x98,
	0x3e, 0x30,
	0xb7, 0xb0,
	0xb8, 0x9d,
	0xb9, 0x13,
	0xba, 0x16,
	0xbb, 0x7b,
	0xbc, 0x91,
	0xbd, 0x1e,
	0xbe, 0x98,
	0xbf, 0x98,
	0xc0, 0x00,
	0xc1, 0x28,
	0xc2, 0x70,
	0xc3, 0x98,
	0xc4, 0x5a,
	0xc5, 0x9d,
	0xc6, 0x9a,
	0xc7, 0x03,
	0xc8, 0x2e,
	0xc9, 0x91,
	0xca, 0xbf,
	0xcb, 0x1e,
	0xa0, 0x0e,
	0xa1, 0x1a,
	0xa2, 0x31,
	0xa3, 0x5a,
	0xa4, 0x69,
	0xa5, 0x75,
	0xa6, 0x82,//7e
	0xa7, 0x8d,//88
	0xa8, 0x98,//8f
	0xa9, 0xa0,//96
	0xaa, 0xae,//a3
	0xab, 0xba,//af
	0xac, 0xcd,//c4
	0xad, 0xdd,//d7
	0xae, 0xec,//e8
	0xaf, 0x20,
	0x89, 0x9c,
	0x8a, 0x11,
	0x8b, 0x12,
	0x8c, 0x11,
	0x8d, 0x52,
	0x96, 0xf0,
	0x97, 0x00,
	0x9c, 0xf0,
	0x9d, 0xf0,
	0x9e, 0xf0,
	0x8f, 0x5c,
	0x90, 0x5c,
	0x91, 0x16,
	0x98, 0x2d,
	0x9a, 0x4c,
	0x92, 0xe0,
	0x95, 0x1f,
	0x99, 0x28,
	0x9b, 0x48,
	0x93, 0xc8,
	0x94, 0x1f,
	0xb2, 0x06,
	0xb3, 0x03,
	0xb4, 0x05,
	0xb5, 0x04,
	0xb6, 0x03,
	0xd5, 0x02,
	0xdb, 0x40,
	0xdc, 0x40,
	0x24, 0x90,
	0x25, 0x80,
	0x26, 0xb2,		// b3-> b2 Jessica
	0x2a, 0xB0,
	0x2b, 0x0B,
	0x2c, 0x19,//14.28fps
	0x5a, 0x14,
	0x5b, 0xe3,
	0x5c, 0x70,
	0x5d, 0x30,
	0x80, 0x7f,
	0x81, 0x07,
	0x83, 0x08, // b2: cb<->cr
	
	0xFF, 0xFF,
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
NULL
};

__align(2)const static u_char  SENSOR_EFFECT_1[] = 
{	//BW
NULL
};
__align(2)const static u_char  SENSOR_EFFECT_2[] = 
{	// Bluish 2
NULL
};
__align(2)const static u_char  SENSOR_EFFECT_3[] = 
{	// Negative 3
NULL
};
__align(2)const static u_char  SENSOR_EFFECT_4[] = 
{	// Reddish 4
NULL
};
__align(2)const static u_char  SENSOR_EFFECT_5[] = 
{	// Antique 5
NULL
};
__align(2)const static u_char  SENSOR_EFFECT_6[] = 
{	// Greenish 6
NULL
};
__align(2)const static u_char  SENSOR_EFFECT_7[] = 
{
NULL
};

__align(2)const static u_char  SENSOR_EV_0[] = 
{	// EV -2
NULL
};
__align(2)const static u_char  SENSOR_EV_1[] = 
{	// EV -1.5
NULL
};
__align(2)const static u_char  SENSOR_EV_2[] = 
{	// EV -1
NULL
};
__align(2)const static u_char  SENSOR_EV_3[] = 
{	// EV -0.5
NULL
};
__align(2)const static u_char  SENSOR_EV_4[] = 
{	// EV 0
NULL
};
__align(2)const static u_char  SENSOR_EV_5[] = 
{	// EV +0.5
NULL
};

__align(2)const static u_char  SENSOR_EV_6[] = 
{	// EV +1
NULL
};

__align(2)const static u_char  SENSOR_EV_7[] = 
{	// EV +1.5
NULL
};

__align(2)const static u_char  SENSOR_EV_8[] = 
{	// EV +2
NULL
};

__align(2)const static u_char  SENSOR_WB_AUTO[] = 
{	// Auto 0
NULL
};	
__align(2)const static u_char  SENSOR_WB_1[] = 
{	// Sunny 1
NULL
};	
__align(2)const static u_char  SENSOR_WB_2[] = 
{	// Cloudy 2
NULL
};	
__align(2)const static u_char  SENSOR_WB_3[] = 
{	// Indoor home 3
NULL
};	
__align(2)const static u_char  SENSOR_WB_4[] = 
{	// Indoor office 4
NULL
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
NULL
};
__align(2)const static u_char  SENSOR_NIGHT_MODE_OFF[] = 
{
NULL
};

__align(2)const static u_char  SENSOR_DEBAND_50[] = 
{
NULL
};
__align(2)const static u_char  SENSOR_DEBAND_60[] = 
{
NULL
};

__align(2)const static u_char  SENSOR_ROTATE_NOMAL[] = 
{
NULL
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR[] = 
{
NULL
};
__align(2)const static u_char  SENSOR_ROTATE_FLIP[] = 
{
NULL
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_FLIP[] = 
{
NULL
};

static u_short	Sensor_Effect(u_short EffectMode)
{
	extern u_char	A8L_SendSensorSettingTable(u_short* tbl,u_short table_length);
	u_short	*tab_addr,tab_len;
    	return 0;

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

   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);
}

static u_short	Sensor_EV(u_short EvStep)
{
	u_short	*tab_addr,tab_len;
//    u_short fRet = 0;
    	return 0;

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
//    u_short fRet = 0;
//    	return 0;

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
				break;																																																	
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
    	return 0;

	switch(NightMode)
	{
		case	A8_SENSOR_NIGHT_MODE_OFF:
				tab_addr = (u_short*)SENSOR_NIGHT_MODE_OFF;
				tab_len = sizeof(SENSOR_NIGHT_MODE_OFF);		
				break;																																																	
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
    	return 0;

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
    	return 0;

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
#if 1
static void	Sensor_Enable(u_short enable)
{
#ifdef AIT_ATV_SUPPORT

extern kal_int8 init_ATV(void);
extern void power_off_ATV(void);
extern void PowerOffFM(void);
extern	u_char	g_ATV_Flag;
#endif
	if(enable) 
	{	
		A8L_VIF_Init(&preview_mode);	//
#ifdef AIT_ATV_SUPPORT		
		if (g_ATV_Flag == 0)
#endif			
		{
			AIT_Message_P0("===OV7680===ATV turn Off, OV7680 turn on \n\r");
#ifdef AIT_ATV_SUPPORT		

			//ATV off
			PowerOffFM();
#endif
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
#ifdef AIT_ATV_SUPPORT
		else{
			AIT_Message_P0("===OV7680===ATV turn ON, OV7680 active \n\r");
			//SetA8RegB(0x69D4,(GetA8RegB(0x69D4)|8)& (~7));
			//SetA8RegB(0x69CD,(GetA8RegB(0x69CD)|4)&(~2));
			//SetA8RegB(0x69CE,(GetA8RegB(0x69CE)|4)&(~2));
			//SetA8RegB(0x69a0,(GetA8RegB(0x69a0))&(~0x18));

			init_ATV();
			sys_IF_ait_delay1ms(1);

			A8L_SetSensorPowerPin(1); //Turn on			
	    		A8L_SetSensorEnablePin(1); //Enable
	    		A8L_SetSensorResetPin(1); 
			A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
			A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
			sys_IF_ait_delay1ms(1);

		}
#endif
	        		
	} else {
	AIT_Message_P0("===OV7680===ATV / OV7680 all turn off \n\r");
#ifdef AIT_ATV_SUPPORT		

	//ATV off
	PowerOffFM();
#endif	
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

		A8L_SetSensorResetPin(1); 
		sys_IF_ait_delay1ms(1);
		A8L_SetSensorResetPin(0);
		sys_IF_ait_delay1ms(1); 
		A8L_SetSensorResetPin(1); 
		//Initial I2C I/F
		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
		sys_IF_ait_delay1ms(1);
/*
		/// Set OV7680's COM7: Reset all register and set to default value
		if (A8L_SetSenReg(0x12, 0x80))
		{
		//		return 1;
		}
*/		
		sys_IF_ait_delay1ms(10);
	} else {

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

	}	
}
#endif

static u_short	Get_Sensor_ID(u_short*	SensorID)
{
	u_short	fRet;
	if(!gbSensorInited)
		Sensor_Enable(1);
	A8L_GetSenReg(0x0a,&fRet);
	*SensorID = fRet << 8;
	A8L_GetSenReg(0x0b,&fRet);
	*SensorID += fRet;
	if(!gbSensorInited)
		Sensor_Enable(0);
	return	fRet;
}

static u_short ChangeSensorAE(void)
{}

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
t_sensor_manager sensor_ov7680 = 
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

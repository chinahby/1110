#include	"A8_common.h"
#include	"AIT700_EBIbus.h"
#include	"A8_sensor.h"
#include "a8_vif.h"
 
#if 1//defined(SENSOR)
#define	SENSOR_ID					(0x7fa2)
#define	SENSOR_I2C_TYPE				(A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_FULL_VH_POLARITY     (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_PREVIEW_LATCH_COUNT  (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_FULL_LATCH_COUNT     (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_PREVIEW_YUV_FORMAT   (A8_SENSOR_CBYCRY) //(A8_SENSOR_YCBYCR)
#define SENSOR_FULL_YUV_FORMAT      (A8_SENSOR_CBYCRY) //(A8_SENSOR_YCBYCR)
#define SENSOR_LATCH_EDGE           (A8_SENSOR_POS_EDGE)

#define	SENSOR_I2C_ID  			    (0xDC)
#define	SENSOR_PREVIEW_WIDTH  	    (640)
#define	SENSOR_PREVIEW_HEIGHT  	    (480)
#define	SENSOR_FULL_WIDTH  		    (640)
#define	SENSOR_FULL_HEIGHT  	    (480)

static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;


__align(2)const static u_char	Sensor_Init[]=
{	0xf0,0x01,
	0xa0,0x01,
	0x13,0x00,
	0x01,0x50,
	0x02,0x60,
	0x13,0x07,
	0x1e,0x00,
	0x15,0x40,
	0x3a,0x0C,
	0x3d,0xc3,
	         
	0x60,0x1b,
	0x63,0x4f,
	0x62,0x10,
	0x66,0xfd,
	0x65,0x80,
	0x98,0x14,
	0x35,0x40,
	0x3B,0xC2,
	0x1F,0x20,
	0x22,0x20,
	0x3f,0xa0,
	0x39,0x80,
	0x3B,0xC2,
	0x83,0x0e,
	0x84,0x02,
	0x85,0x06,
	0x86,0x08,
	0x88,0x01,
	0x89,0x55,

	0x40,0x20,
	0x41,0x20,
	0x42,0x28,
	0x43,0x22,
	0x44,0x1e,
	0x45,0x18,
	0x46,0x12,
	0x47,0x0e,
	0x48,0x0c,
	0x49,0x0c,
	0x4b,0x0a,
	0x4c,0x0a,
	0x4e,0x08,
	0x4f,0x07,
	0x50,0x07,

	0x51,0x38,
	0x52,0x98,
	0x53,0x01,
	0x54,0x8D,
	0x57,0x3b,
	0x58,0x8D,
	0x59,0x87,
	0x5a,0x9C,
	0x5b,0x44,
	         
	0x24,0x7a,
	0x25,0x6a,
	0xb1,0xb0,
	0xb2,0xa0,

	0x16,0x2a,
	0xa2,0x80,
	0xf0,0x00,
	
};

__align(2)const static u_char Sensor_DSC_Preview[] =
{
NULL
};
__align(2)const static u_char Sensor_Video_Preview[]=
{
NULL
};
__align(2)const static u_char Sensor_Normal_Capture[] = 
{
NULL
};
__align(2)const static u_char Sensor_Full_Capture[] = 
{
NULL,
};
__align(2)const static u_char Sensor_Full_Capture_ScaleUp[] =
{
NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_NORMAL[] = 
{	//Normal
	0x3A, 0x0c,
	0x67, 0x80,
	0x68, 0x80,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_1[] = 
{	// GrayScale
	0x3A, 0x1c,
	0x67, 0x80,
	0x68, 0x80,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_2[] = 
{	// Sepia
	0x3A, 0x1c,
	0x67, 0x80,
	0x68, 0xc0,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_3[] = 
{	// Color Inv 
	0x3A, 0x2c,
	0x67, 0x80,
	0x68, 0x80,
	0x56, 0x40
};
__align(2)const static u_char  SENSOR_EFFECT_4[] = 
{	// SepiaGreen
	0x3A, 0x1c,
	0x67, 0x40,
	0x68, 0x40,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_5[] = 
{	// SepiaBlue
	0x3A, 0x1c,
	0x67, 0xA0,
	0x68, 0x40,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_6[] = 
{	// GrayInv
	0x3A, 0x1c,
	0x67, 0x80,
	0x68, 0x80,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_7[] = 
{	// CopperCarving
	0x3A, 0x1c,
	0x67, 0x80,
	0x68, 0xf0,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_8[] = 
{	// BlueCarving
	0x3A, 0x1c,
	0x67, 0xb0,
	0x68, 0x60,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_9[] = 
{	// Contrast
	0x3A, 0x0c,
	0x67, 0x80,
	0x68, 0x80,
	0x56, 0x80,
};
__align(2)const static u_char  SENSOR_EFFECT_A[] = 
{	// Embossment
	0x3A, 0x14,
	0x67, 0x80,
	0x68, 0x80,
	0x56, 0x80,
};
__align(2)const static u_char  SENSOR_EFFECT_B[] = 
{	// Sketch
	0x3A, 0x24,
	0x67, 0x80,
	0x68, 0x00,
	0x56, 0xa0,
};


__align(2)const static u_char  SENSOR_EV_0[] = 
{	// EV -2
	0xff, 0xf8,
};
__align(2)const static u_char  SENSOR_EV_1[] = 
{	// EV -1.5
	0x55, 0xd8,
};
__align(2)const static u_char  SENSOR_EV_2[] = 
{	// EV -1
	0x55, 0xb8,
};
__align(2)const static u_char  SENSOR_EV_3[] = 
{	// EV -0.5
	0x55, 0x98,
};
__align(2)const static u_char  SENSOR_EV_4[] = 
{	// EV 0
	0x55,0x00,
};
__align(2)const static u_char  SENSOR_EV_5[] = 
{	// EV +0.5
	0x55, 0x18,
};
__align(2)const static u_char  SENSOR_EV_6[] = 
{	// EV +1
	0x55, 0x38,
};
__align(2)const static u_char  SENSOR_EV_7[] = 
{	// EV +1.5
	0x55, 0x58,
};
__align(2)const static u_char  SENSOR_EV_8[] = 
{	// EV +2
	0x55, 0x78,
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
{	// Cloudy 2
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
NULL
};
__align(2)const static u_char  SENSOR_NIGHT_MODE_OFF[] = 
{
NULL
};

__align(2)const static u_char  SENSOR_DEBAND_50[] = 
{
	0xf0, 0x01,
	0x80, 0x01,
	0xf0, 0x00,
	0x80, 0x5b,
	0x9d, 0x4c,
};
__align(2)const static u_char  SENSOR_DEBAND_60[] = 
{
	0xf0, 0x01,
	0x80, 0x00,
	0xf0, 0x00,
	0x80, 0x5b,
	0x9e, 0x3f,
};

__align(2)const static u_char  SENSOR_ROTATE_NOMAL[] = 
{
	0x03, 0x00,
	0x11, 0x80,
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR[] = 
{
	0x03, 0x00,
	0x11, 0x81,
};
__align(2)const static u_char  SENSOR_ROTATE_FLIP[] = 
{
	0x03, 0x00,
	0x11, 0x82,
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_FLIP[] = 
{
	0x03, 0x00,
	0x11, 0x83,
};

static u_short	Sensor_Effect(u_short EffectMode)
{
	extern u_char	A8L_SendSensorSettingTable(u_short* tbl,u_short table_length);
	u_short	*tab_addr,tab_len;
    
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
 //   u_short fRet = 0;
    
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
//	u_short	*tab_addr,tab_len=0;
    u_short fRet = 0;
    u_short	tmp_val;
    
    fRet = A8L_GetSenReg(0x013,&tmp_val);
    
	switch(WbMode)
	{
		case	0:	//WB_AUTO
				A8L_SetSenReg(0x01,0x66);
				A8L_SetSenReg(0x02,0x6c);
				A8L_SetSenReg(0x13,tmp_val | 0x2);	//enable awb
//				tab_addr = (u_short*)SENSOR_WB_AUTO;
//				tab_len = sizeof(SENSOR_WB_AUTO);		
				break;	
		case	1:	//WB_CLOUD
				A8L_SetSenReg(0x13,tmp_val & (~0x2));	//enable awb			
				A8L_SetSenReg(0x01,0x52);
				A8L_SetSenReg(0x02,0x6c);
//				tab_addr = (u_short*)SENSOR_WB_1;
//				tab_len = sizeof(SENSOR_WB_1);		
				break;	
		case	2:	//WB_DAYLIGHT
				A8L_SetSenReg(0x13,tmp_val & (~0x2));	//enable awb			
				A8L_SetSenReg(0x01,0x52);
				A8L_SetSenReg(0x02,0x66);		
//				tab_addr = (u_short*)SENSOR_WB_2;
//				tab_len = sizeof(SENSOR_WB_2);		
				break;	
		case	3:	//WB_INCANDESCENCE
				A8L_SetSenReg(0x13,tmp_val & (~0x2));	//enable awb			
				A8L_SetSenReg(0x01,0x8c);
				A8L_SetSenReg(0x02,0x59);		
//				tab_addr = (u_short*)SENSOR_WB_3;
//				tab_len = sizeof(SENSOR_WB_3);		
				break;	
		case	4:	//WB_FLUORESCENT
				A8L_SetSenReg(0x13,tmp_val & (~0x2));	//enable awb			
				A8L_SetSenReg(0x01,0x7e);
				A8L_SetSenReg(0x02,0x49);		
//				tab_addr = (u_short*)SENSOR_WB_4;
//				tab_len = sizeof(SENSOR_WB_4);		
				break;	
		case	5:	//WB_TUNGSTEN
				A8L_SetSenReg(0x13,tmp_val & (~0x2));	//enable awb			
				A8L_SetSenReg(0x01,0x90);
				A8L_SetSenReg(0x02,0x3d);		
//				tab_addr = (u_short*)SENSOR_WB_5;
//				tab_len = sizeof(SENSOR_WB_5);		
				break;	
		case	6:	//
//				tab_addr = (u_short*)SENSOR_WB_6;
//				tab_len = sizeof(SENSOR_WB_6);		
				break;	;																																																			
		default:
				A8L_SetSenReg(0x01,0x66);
				A8L_SetSenReg(0x02,0x6c);
				A8L_SetSenReg(0x13,tmp_val | 0x2);	//enable awb		
//				tab_addr = (u_short*)SENSOR_WB_AUTO;
//				tab_len = sizeof(SENSOR_WB_AUTO);
				break;	
	}	
		return	fRet;
//   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);		
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
    u_short fRet,tmp_val;
    fRet = A8L_GetSenReg(0x3b,&tmp_val);
	switch(DeBandMode)
	{
		case	A8_SENSOR_DEBAND_60:
				A8L_SetSenReg(0x3b, tmp_val & 0xf7);
				tab_addr = (u_short*)SENSOR_DEBAND_60;
				tab_len = sizeof(SENSOR_DEBAND_60);		
				break;	;																																																			
		default:
				A8L_SetSenReg(0x3b,tmp_val | 0x08);
				tab_addr = (u_short*)SENSOR_DEBAND_50;
				tab_len = sizeof(SENSOR_DEBAND_50);
				break;		
	}
	
   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);		
}

static u_short	Sensor_Rotate(mmp_sensor_rotate Rotate)
{
	u_short	*tab_addr,tab_len;
    
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
	extern void A8L_VIF_Init(t_sensor_mode_attr* sensor_mode);
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
			//AIT_Message_P0("===BF3503===ATV turn Off, BF3503 turn on\r\n");
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
			//AIT_Message_P0("===BF3503===ATV turn ON, BF3503 Semi-active \n\r");
			//SetA8RegB(0x69D4,(GetA8RegB(0x69D4)|8)& (~7));
			//SetA8RegB(0x69CD,(GetA8RegB(0x69CD)|4)&(~2));
			//SetA8RegB(0x69CE,(GetA8RegB(0x69CE)|4)&(~2));
			//SetA8RegB(0x69a0,(GetA8RegB(0x69a0))&(~0x18));

			init_ATV();
			sys_IF_ait_delay1ms(1);
			
	    		A8L_SetSensorEnablePin(1); //Enable
			A8L_SetSensorPowerPin(0); //Turn on
		}
#endif
	        		
	} else {
	//AIT_Message_P0("===BF3503===ATV / BF3503 all turn off \n\r");
#if AIT_ATV_SUPPORT==1

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
{return 0;}

static u_short SetSensorResolution(mmp_sensor_mode sensor_mode)
{
	extern u_short	A8L_SetSensorAttr(t_sensor_mode_attr* sensor_mode);
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
	65,
	16,
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
t_sensor_manager sensor_bf3503 = 
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










#if 0
static void	Sensor_Enable(u_short enable)
{
	if(enable) 
	{	
		A8L_VIF_Init(&preview_mode);	//
		
		if (g_ATV_Flag == 0){
			AIT_Message_P0("======ATV turn Off, BF3503 turn on \n\r");

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

			//Initial I2C I/F
			A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
			A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
			sys_IF_ait_delay1ms(1);
    		
		}else{
			AIT_Message_P0("======ATV turn ON, BF3503 turn off \n\r");
			//SetA8RegB(0x69D4,(GetA8RegB(0x69D4)|8)& (~7));
			//SetA8RegB(0x69CD,(GetA8RegB(0x69CD)|4)&(~2));
			//SetA8RegB(0x69CE,(GetA8RegB(0x69CE)|4)&(~2));
			//SetA8RegB(0x69a0,(GetA8RegB(0x69a0))&(~0x18));

			init_ATV();
			sys_IF_ait_delay1ms(1);
			
	    		A8L_SetSensorEnablePin(0); //Disable
			A8L_SetSensorPowerPin(0); //Turn off
			SetA8RegB(VIF_SEN_CLK_CTL,0);
			A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_OFF);
			A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_OFF);
		}

	        		
	} else {
	AIT_Message_P0("======ATV / BF3503 all turn off \n\r");

	//ATV off
	PowerOffFM();
	sys_IF_ait_delay1ms(1); 
	//Sensor off
	A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_OFF);
	A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_OFF);	
	SetA8RegB(VIF_SEN_CLK_CTL,0);
	A8L_SetSensorEnablePin(0); //Disable sensor
	A8L_SetSensorPowerPin(0); //Turn off sensor
		}
}



#endif

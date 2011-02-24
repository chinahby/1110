#include "a810h_sensor.h"
#include "a810h_common.h"
#include "a810h_system.h"
#include "aits_init.h"
#include "aits_dsc.h"

#if HOST_SENSOR_INIT==1
#define	SENSOR_I2C_TYPE				(A8_I2C_2A1D)
#define SENSOR_PREVIEW_VH_POLARITY	(A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)//(A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS)
#define SENSOR_FULL_VH_POLARITY		(A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS)
#define SENSOR_PREVIEW_LATCH_COUNT	(A8_PHASE_COUNTER_NUM_4)
#define SENSOR_FULL_LATCH_COUNT		(A8_PHASE_COUNTER_NUM_4)
#define SENSOR_PREVIEW_YUV_FORMAT	(A8_SENSOR_YCBYCR)
#define SENSOR_FULL_YUV_FORMAT		(A8_SENSOR_YCBYCR)
#define SENSOR_LATCH_EDGE				(A8_SENSOR_POS_EDGE)

#define	SENSOR_I2C_ID					(0x60)
#define	SENSOR_PREVIEW_WIDTH			(640)
#define	SENSOR_PREVIEW_HEIGHT		(480)
#define	SENSOR_FULL_WIDTH				(2048)
#define	SENSOR_FULL_HEIGHT			(1536)
 // 2K size, share with storage DMA address , the old address 0x40000 may conflict with OSD 
#define SENSOR_TABLE_LOAD_ADDR		(0x47800)


// dsc/videor may run under different CPU clk, so, using different phase count
u_short gsVideoPreviewSensorPhaseCount = A8_PHASE_COUNTER_NUM_4;
u_short gsDSCPreviewSensorPhaseCount = SENSOR_PREVIEW_LATCH_COUNT;

u_short gsSensorModeWidth[A8_SENSOR_RESOLUTION_NUM] =  {SENSOR_FULL_WIDTH , SENSOR_PREVIEW_WIDTH};
u_short gsSensorModeHeight[A8_SENSOR_RESOLUTION_NUM] = {SENSOR_FULL_HEIGHT, SENSOR_PREVIEW_HEIGHT};


u_short SENSOR_Init[] = 
{
//IO & Clock & Analog Setup
 0x308c,0x80, //TMC12: DIS_MIPI_RW
 0x308d,0x0e, //TMC13: MIPI disable
 0x360b,0x00,
 0x30b0,0xff, //IO_CTRL0: Cy[7:0]
 0x30b1,0xff, //IO_CTRL1: C_VSYNC,C_STROBE,C_PCLK,C_HREF,Cy[9:8]
 0x30b2,0x24, //IO_CTRL2: R_PAD[3:0] 

 0x300f,0xa6,
 0x3010,0x81,
 0x3082,0x01,
 0x30f4,0x01,
 0x3090,0x33,
 0x3091,0xc0,
 0x30ac,0x42,

 0x30d1,0x08,
 0x30a8,0x56,
 0x3015,0x02, //AUTO_3: AGC ceiling = 4x, 5dummy frame
 0x3093,0x00,
 0x307e,0xe5, //TMC8: AGC[7:6]=b'11
 0x3079,0x00,
 0x30aa,0x42,
 0x3017,0x40, //AUTO_5: disable data drop, manual banding counter=0
 0x30f3,0x82,
 0x306a,0x0f, //0x0c->0x0f Joe 0814 : BLC
 0x306d,0x00,
 0x336a,0x3c,
 0x3076,0x6a, //TMC0: VSYNC drop option: drop
 0x30d9,0x95,
 0x3016,0xc2, //Joe=0xc2 0808 //org=0x82//AUTO_4: max exposure adjust option=2
 0x3601,0x30,
 0x304e,0x88,
 0x30f1,0x82,
 0x306f,0x14,
 0x302a,0x02,
 0x302b,0x6a,

 0x3012,0x10,
 0x3011,0x01,

 //AEC/AGC
 0x3013,0xf7, //AUTO_1: banding filter, AGC auto, AEC auto
 0x301c,0x03, //AECG_MAX50: 50Hz smooth banding max step=0x13
 0x301d,0x03, //AECG_MAX60: 60Hz smooth banding max step=0x17
 0x3070,0x3e, //BD50:LSB
 0x3072,0x4d, //BD60:LSB

 0x3018,0x82, //high limit //AE target¢Q?¯Ù£V?
 0x3019,0x72, //low limit 
 0x301a,0xa3, //0xa3 by Joe 0805 //AE step 

 //D5060
 0x30af,0x00,
 0x3048,0x1f,
 0x3049,0x4e,
 0x304a,0x20,
 0x304f,0x20,
 0x304b,0x02,
 0x304c,0x00,
 0x304d,0x02,
 0x304f,0x20,
 0x30a3,0x10,
 0x3013,0xf7, 
 0x3014,0x25, //R1D bit6 always = 0 , bit[5]=1, bit[0]=1
 0x3071,0x00,
 0x3070,0xba, //50hz banding
 0x3073,0x00,
 0x3072,0x4d, //60hz banding
 0x301c,0x03, //50hz banding
 0x301d,0x03, //60hz banding
 0x304d,0x42,
 0x304a,0x00, //Disable 50/60 auto detection function, due to ov2650 no this function
 0x304f,0x00, //Disable 50/60 auto detection function, due to ov2650 no this function
 0x3095,0x07,
 0x3096,0x16,
 0x3097,0x1d,

 //Window Setup
 0x3020,0x01, //HS
 0x3021,0x18, //HS 0x18 0813
 0x3022,0x00,
 0x3023,0x06,
 0x3024,0x06,
 0x3025,0x58,
 0x3026,0x02,
 0x3027,0x5e,
 0x3088,0x03,
 0x3089,0x20,
 0x308a,0x02,
 0x308b,0x58,
 0x3316,0x64,
 0x3317,0x25,
 0x3318,0x80,
 0x3319,0x08,
 0x331a,0x64,
 0x331b,0x4b,
 0x331c,0x00,
 0x331d,0x38,
 0x3100,0x00,

    //AWB
 0x3320,0x9a,   
 0x3321,0x11,   
 0x3322,0x92,   
 0x3323,0x01,   
 0x3324,0x92,   
 0x3325,0x02,   
 0x3326,0xff,   
 0x3327,0x0f,   
 0x3328,0x0F,   
 0x3329,0x14,   
 0x332a,0x66,   
 0x332b,0x5c,   //5f -> 5c
 0x332c,0x89,   //a5 -> 89
 0x332d,0x96,   //ac -> 96
 0x332e,0x3d,   //35 -> 3d
 0x332f,0x2f,   
 0x3330,0x57,   
 0x3331,0x3d,   
 0x3332,0xf0,   
 0x3333,0x10,   
 0x3334,0xf0,   
 0x3335,0xf0,   
 0x3336,0xf0,   
 0x3337,0x40,   
 0x3338,0x40,   
 0x3339,0x40,   
 0x333a,0x00,   
 0x333b,0x00,   

 //Color Matrix
 0x3380,0x2d, //28->2d
 0x3381,0x4d, //48->4d
 0x3382,0x13, //10->13 0819 Joe for ?úö
 0x3383,0x2f,  
 0x3384,0x8c,  
 0x3385,0xbb,  
 0x3386,0x9c,  
 0x3387,0x94,  
 0x3388,0x05,  
 0x3389,0x98,
 0x338a,0x01,

 //Gamma
 0x3340,0x05,  
 0x3341,0x10,  
 0x3342,0x25,  
 0x3343,0x46,
 0x3344,0x57,  
 0x3345,0x64,  
 0x3346,0x70,  
 0x3347,0x7c,  
 0x3348,0x87,  
 0x3349,0x90,
 0x334a,0x9f,
 0x334b,0xac,
 0x334c,0xc1,
 0x334d,0xd5,
 0x334e,0xe8,
       0x334f,0x20,

 0x30d9, 0x95,


 //Lens correction
 //R
 0x3350,0x32,
 0x3351,0x25,
 0x3352,0x80,
 0x3353,0x1e,
 0x3354,0x00,
 0x3355,0x85,
 //G
 0x3356,0x32,
 0x3357,0x25,
 0x3358,0x80,
 0x3359,0x1b,
 0x335a,0x00,
 0x335b,0x85,
 //B
 0x335c,0x32,
 0x335d,0x25,
 0x335e,0x80,
 0x335f,0x1b,
 0x3360,0x00,
 0x3361,0x85,

 0x3363,0x70,
 0x3364,0x7f,
 0x3365,0x00,
 0x3366,0x00,

    0x3362,0x90,

 //UVadjust
 0x3301,0xff,
 0x338B,0x0e,
 0x338c,0x1f,
 0x338d,0x40,

 //Sharpness/De-noise
 0x3370,0xd0,
 0x3371,0x00,
 0x3372,0x00,
 0x3373,0x40,
 0x3374,0x10,
 0x3375,0x10,
 0x3376,0x05,
 0x3377,0x02,
 0x3378,0x04,
 0x3379,0x80,

 //BLC
 0x3069,0x84,
 0x307c,0x10,
 0x3087,0x02,

 //Other functions
 0x3300,0xfc,
 0x3302,0x11,
 0x3400,0x00,
 0x3606,0x20,
 0x3601,0x30,
 0x30f3,0x83,
 0x304e,0x88,
 
 0x3086,0x0f,
 0x3086,0x00,

     0x3391,0x02,
 0x3394,0x4c,
     0x3395,0x4c,

 0x302d, 0x00, 
 0x302e, 0x00,

0xffff,0xffff
};

u_short SENSOR_640x480_Preview_Video[]=
{

0xffff,0xffff
};

u_short SENSOR_640x480_Preview[] =
{
0xffff,0xffff
};

u_short SENSOR_1600x1200_Capture[] = 
{

0xffff,0xffff
};

u_short  SENSOR_2048x1536_Capture[] = 
{
0xffff,0xffff
};

u_short SENSOR_EFFECT_NORMAL[] = 
{

0xffff,0xffff
};
u_short SENSOR_EFFECT_1[] = 
{

0xffff,0xffff
};
u_short SENSOR_EFFECT_2[] = 
{

0xffff,0xffff
};
u_short SENSOR_EFFECT_3[] = 
{

0xffff,0xffff
};
u_short SENSOR_NIGHT_MODE_ON[] = 
{

0xffff,0xffff
};
u_short SENSOR_NIGHT_MODE_OFF[] = 
{

0xffff,0xffff
};
u_short SENSOR_WB_AUTO[] = 
{

0xffff,0xffff
};
u_short SENSOR_WB_DAYLIGHT[] = 
{

0xffff,0xffff
};
u_short SENSOR_WB_INCANDESCENT[] = 
{

0xffff,0xffff
};
u_short SENSOR_WB_FLUORSCENT[] = 
{

0xffff,0xffff
};
u_short SENSOR_WB_CLOUDY[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_0[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_1[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_2[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_3[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_4[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_5[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_6[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_7[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_8[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_9[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_10[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_11[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_12[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_13[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_14[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_15[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_16[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_17[] = 
{

0xffff,0xffff
};
u_short SENSOR_EV_18[] = 
{

0xffff,0xffff
};
u_short	SENSOR_PHOTOMETRY_NORMAL[] =
{

0xffff,0xffff
};
u_short	SENSOR_PHOTOMETRY_CENTER[] =
{

0xffff,0xffff
};

static const AITS_Sensor_Parameter_Entry m_master_table[] = {
                  // type,                                   | pointer to                    | special function
                  //                                         | table arrary                  | if any
    ADD_PARAM_ENTRY(AITS_SENSOR_PARAM_MAX/*end of table*/,     NULL,                           NULL)
};

u_short AITS_INIT_Sensor(void)
{
	u_short	fRet;
	gbSensorInited = 0;
	A810H_PresetSensor(0, SENSOR_I2C_ID, SENSOR_I2C_TYPE, 3);
	A810H_PresetSensorResol(A8_SENSOR_FULL_RESOL, 0, SENSOR_LATCH_EDGE, SENSOR_FULL_VH_POLARITY, SENSOR_FULL_YUV_FORMAT,
	                         SENSOR_FULL_WIDTH, SENSOR_FULL_HEIGHT,SENSOR_FULL_LATCH_COUNT);
	A810H_PresetSensorResol(A8_SENSOR_PREVIEW_RESOL, 0, SENSOR_LATCH_EDGE, SENSOR_PREVIEW_VH_POLARITY, SENSOR_PREVIEW_YUV_FORMAT,
	                         SENSOR_PREVIEW_WIDTH, SENSOR_PREVIEW_HEIGHT,SENSOR_PREVIEW_LATCH_COUNT);
    //don't send this table to fw, use api send setting to sensor at mmp_dsc_preview_start()
    //if want to use this table in fw, then, send this table down and need to configure enough table size in fw
    #if 0        
	A810H_PresetSensorResolutionTable(SENSOR_PREVIEW_TAB, SENSOR_TABLE_LOAD_ADDR, (u_short *)SENSOR_640x480_Preview, sizeof(SENSOR_640x480_Preview));
	#endif
	
	A810H_PresetSensorResolutionTable(SENSOR_NORMAL_TAB, SENSOR_TABLE_LOAD_ADDR, (u_short *)SENSOR_640x480_Preview, sizeof(SENSOR_640x480_Preview));	
	A810H_PresetSensorResolutionTable(SENSOR_FULL_TAB, SENSOR_TABLE_LOAD_ADDR, (u_short *)SENSOR_2048x1536_Capture, sizeof(SENSOR_2048x1536_Capture));
	A810H_PresetSensorPreviewTable(SENSOR_640x480_Preview,SENSOR_640x480_Preview_Video);

	//PSEN pull up
	A810H_SetPinPull(0x69D1, PIN_PULL_HIGH);
	//PSDA pull up
	A810H_SetPinPull(0x69D2, PIN_PULL_HIGH);
	//PSCK pull up
	A810H_SetPinPull(0x69D3, PIN_PULL_HIGH);

	//sensor PSEN
	A810H_SetRegB(0x6050,A810H_GetRegB(0x6050)|0x01);
	
	A810H_SetRegB(0x6051,A810H_GetRegB(0x6051)|0x01);	
	
	A810H_SetRegB(0x6051,A810H_GetRegB(0x6051)&0xFE);		
	AITC_SYS_BusyWaitMS(20);

        // Start MCLK (Init TG)
        A810H_Init_TG();

	A810H_SetRegB(0x6051,A810H_GetRegB(0x6051)&0xFE);
	AITC_SYS_BusyWaitMS(20);
    
	//sensor reset
	AITS_SYS_SetGPIO(9, AITS_SYSTEM_OUTPUT, 1); 
	AITC_SYS_BusyWaitMS(20);
	AITS_SYS_SetGPIO(9, AITS_SYSTEM_OUTPUT, 0);
	AITC_SYS_BusyWaitMS(20); 
	AITS_SYS_SetGPIO(9, AITS_SYSTEM_OUTPUT, 1); 
	AITC_SYS_BusyWaitMS(20);

    A810H_Sensor_ID(SENSOR_I2C_ID);

        {
            u_char pid_h,pid_l,sccb_id,test_temp;
            pid_h=AITS_Sensor_GetReg(0x300A);
            pid_l=AITS_Sensor_GetReg(0x300B);
            sccb_id=AITS_Sensor_GetReg(0x300C);
            AIT_Message_P1("PIDH = 0x%x   should be 0x26\n", pid_h);
            AIT_Message_P1("PIDL = 0x%x   should be 0x56\n", pid_l);
            AIT_Message_P1("SCCB = 0x%x   should be 0x60\n", sccb_id);
            
            
            AITS_Sensor_SetReg(0x3004,0x55);
        	test_temp = AITS_Sensor_GetReg(0x3004);
        	AIT_Message_P1("test_temp = 0x%x   should be 0x55\n", test_temp);
        	
            AITS_Sensor_SetReg(0x3004,0xAA);
        	test_temp = AITS_Sensor_GetReg(0x3004);
        	AIT_Message_P1("test_temp = 0x%x   should be 0xAA\n", test_temp);       	
        
        
        
        }
    
	fRet = A810H_SetSensorRegister(0x3012, 0x0080);
	if (fRet)
	{
		AIT_Message_P0("ov2655, Software reset failed\n");
		return fRet;
	}
	AITC_SYS_BusyWaitMS(20);
	
	fRet = A810H_InitSensor_1(SENSOR_TABLE_LOAD_ADDR, (u_short *)SENSOR_Init, sizeof(SENSOR_Init)); 
    	if(fRet==DSC_NO_ERROR) {
    		gbSensorInited = 1;
	}
	
    	return fRet;
}
u_short	AITS_Get_Sensor_ID(u_short*	SensorID)
{
	u_short	fRet;
	fRet = AITS_Sensor_SetReg(0x03, 0x00);
	*SensorID = AITS_Sensor_GetReg(0x04);
	return	fRet;
}
u_short	AITS_Sensor_Effect(u_short EffectMode)
{
	u_short *Tmp;
	//u_int	i,j;
	u_short	Saddr, Sdata;
	switch(EffectMode)
	{
		case	1:
			Tmp = (u_short *)SENSOR_EFFECT_1;
			break;
		case	2:	
			Tmp = (u_short *)SENSOR_EFFECT_2;
			break;
		case	3:
			Tmp = (u_short *)SENSOR_EFFECT_3;
			break;
		default:	
			Tmp = (u_short *)SENSOR_EFFECT_NORMAL;
			break;
	}

	while(1)
	{
		Saddr = *Tmp++;	
		if(SENSOR_I2C_TYPE) {
			Sdata = *Tmp++;
			if((Saddr == 0xffff)&&(Sdata == 0xffff))
				break;
		} else {
			Sdata = Saddr >> 8;
			if(Saddr == 0xffff)
				break;
		}
		AITS_Sensor_SetReg(Saddr, Sdata);			
	}
	return 0;

}

u_short	AITS_Sensor_Disable(void)
{
    return 0;
}

u_short AITS_Sensor_VideoMode(u_char VideoOn)
{
    return 0;    
}

u_short	AITS_Sensor_EV(u_short EvStep)
{
    return 0;
}

u_short	AITS_Sensor_WhiteBalance(u_short WbMode)
{
    return 0;
}

u_short	AITS_Sensor_NightMode(u_short NightMode)
{
    return 0;
}

u_short	AITS_Sensor_ExposureMode(u_short ExposureMode)
{
    return 0;
}

u_short	AITS_Sensor_BandingFreq(u_short BandingFreq)
{
    return DSC_NO_ERROR;
}

u_short	AITS_Sensor_Contrast(u_short Contrast)
{
    return DSC_NO_ERROR;
}
u_short	AITS_Sensor_Capture_SetWB(void)
{
    return 0;
}

u_short AITS_Sensor_FullMode(u_char FullModeOn)
{
    return 0;
}


u_short	AITS_Sensor_PowerOff_Sequence(void)
{	
	//power off sequency.....


	//////	
		
    return 0;
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



t_sensor_manager sensor_ov2655 = 
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

#endif //#if HOST_SENSOR_INIT==1

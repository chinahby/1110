#include	"A8_common.h"
#include	"AIT700_EBIbus.h"
#include	"A8_sensor.h"
#include 	"ait_interface.h"

// sensor's chip ID and version
#define SIV121A_SENSOR_ID       		    (0x92)
#define SIV121A_SENSOR_VERSION  			(0x11)

#define SIV121A_I2C_WRITE_ID   	            (0x66)
#define SIV121A_I2C_READ_ID		            (0x67)

/* SENSOR REGISTER DEFINE */
#define SIV121A_ID_REG           	        (0x01)
#define SIV121A_INFO_REG                	(0x02)

#define	SENSOR_I2C_TYPE						(A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  		(A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS)
#define SENSOR_FULL_VH_POLARITY     		(A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS)
#define SENSOR_PREVIEW_LATCH_COUNT  		(A8_PHASE_COUNTER_NUM_3)
#define SENSOR_FULL_LATCH_COUNT     		(A8_PHASE_COUNTER_NUM_2)
#define SENSOR_PREVIEW_YUV_FORMAT   		(A8_SENSOR_CBYCRY)
#define SENSOR_FULL_YUV_FORMAT     	 		(A8_SENSOR_CBYCRY)
#define SENSOR_LATCH_EDGE           		(A8_SENSOR_POS_EDGE)

#define	SENSOR_I2C_ID  			    		(0x66)
#define	SENSOR_PREVIEW_WIDTH  	    		(640)
#define	SENSOR_PREVIEW_HEIGHT  	    		(480)
#define	SENSOR_FULL_WIDTH  		    		(640)
#define	SENSOR_FULL_HEIGHT  	    		(480)

u_short siv121a_read_cmos_sensor(u_short Reg)
{
	u_short ret;
	A8L_GetSenReg(Reg,&ret);
	return ret;
}

u_short siv121a_write_cmos_sensor(u_short Reg, u_short Value)
{

	return A8L_SetSenReg(Reg,Value);

}

static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;


static u_short	Get_Sensor_ID(u_short*	SensorID);

__align(2)const static u_char	Sensor_Init[]=
{
	// SNR Block [Vendor recommended value ##Don't change##]
	0x00,0x00,
	0x03,0xF4, //0x04
	0x10,0x83, 
	0x13,0x17,
	0x15,0x22, 

	0x00,0x01,
	0x04,0x00,
	0x05,0x02, 
	0x11,0x25,
	0x12,0x21, 
	0x13,0x17,
	0x15,0x22,
	 
	0x20,0x10, //0x00
	0x21,0xc5, //0x03
    0x23,0xad, //0x01
    
    0x00,0x02,
	0x34,0x60,   //42 
    //50HZ 12MHZ 12.5FPS
 
    
    0x00,0x02,
    0x11,0x08,
    0x12,0x66,
    0x13,0x66,
    0x14,0x66,
    0x1e,0x00,
	
	0x40,0x30,
	0x41,0x20,
	0x42,0x20,
	0x43,0x00,
	0x44,0x00,
	0x45,0x00, 
	0x46,0x0A,
	0x47,0x10,
	0x48,0x13,
	0x49,0x15,
	0x4A,0x18,
	0x4B,0x1A,
	0x4C,0x1D,
	0x4D,0x20,
	0x4E,0x10,
	0x4F,0x0A,
	0x50,0x08,
	0x51,0x06,
	0x52,0x05,
	0x53,0x04,
	0x54,0x02,
	0x55,0x01,

    //AWB
	0x00,0x03,
    0x10,0xc3,//d0
	0x11,0xC0,
	0x12,0x80,
	0x13,0x7f,
	0x14,0x7f,
	0x15,0xFE, 
	0x16,0x87,
	0x17,0xc8,
	0x18,0x70,
	0x19,0x94,
	0x1A,0x6c,
	0x1B,0x94,
	0x1C,0x6c,
	0x1D,0x94,
	0x1E,0x6c,
	0x20,0xE8,
	0x21,0x30,
	0x22,0xA4,
	0x23,0x20,
	0x25,0x20,
	0x26,0x0F,
	0x27,0x10,
	0x28,0x20,
	0x29,0xa9,
	0x2A,0x8c,

	0x30,0x00,
	0x31,0x10,
	0x32,0x00,
	0x33,0x10,
	0x34,0x02,
	0x35,0x76,
	0x36,0x01,
	0x37,0xD6,
	0x40,0x01,
	0x41,0x04,
	0x42,0x08,
	0x43,0x10,
	0x44,0x12,
	0x45,0x35,
	0x46,0x64,
	0x50,0x33,
	0x51,0x20,
	0x52,0xE5,
	0x53,0xFB,
	0x54,0x13,
	0x55,0x26,
	0x56,0x07,
	0x57,0xF5,
	0x58,0xEA,
	0x59,0x21,

	//CMA change  -D65~A
	0x63,0x98, //D30 to D20 for R
	0x64,0xa8, //D30 to D20 for B
	0x65,0x98, //D20 to D30 for R
	0x66,0xa8, //D20 to D30 for B
	0x67,0xc8, //D65 to D30 for R
	0x68,0x9f, //D65 to D30 for B
	0x69,0xc8, //D30 to D65 for R
	0x6A,0x9f, //D30 to D65 for B

	// IDP
	0x00,0x04,
	0x10,0xFF, // IDP function enable
	0x11,0x19, //0x1D // PCLK polarity  0x09/0x19
	//0x12,0x9D, // Y,Cb,Cr order sequence
    0x12,0x9D,// Y,Cb,Cr order sequence
	// DPCNR
	0x17,0x98,
	0x18,0x00,
	0x19,0x80, //0X40  20091130
	0x1A,0x40,
	0x1B,0x12,
	0x1C,0x00,
	0x1D,0x4f,
	0x1E,0x4f,
	0x1F,0x2F,
	0x20,0x04, // Normal illumiinfo start
	0x21,0x0F, // Dark illumiinfo start

	// Gamma
	0x30,0x00,
	0x31,0x02,
	0x32,0x06,
	0x33,0x1c,
	0x34,0x42,
	0x35,0x60,
	0x36,0x79,
	0x37,0x8c,
	0x38,0x9B,
	0x39,0xAA,
	0x3A,0xb6,
	0x3B,0xca,
	0x3C,0xdc,
	0x3D,0xef,
	0x3E,0xf8,
	0x3F,0xFF,

	// Shading
    0x40,0x11,
    0x41,0x21,
    0x42,0x32,
    0x43,0x43,
    0x44,0x55,
    0x45,0x55,                                                 
    0x46,0x22,  // left R gain[7:4], right R gain[3:0] 0x22        
    0x47,0x23,   // top R gain[7:4], bottom R gain[3:0]             
    0x48,0x11,  // left Gr gain[7:4], right Gr gain[3:0]           
    0x49,0x12,   // top Gr gain[7:4], bottom Gr gain[3:0]           
    0x4a,0x01,   // left Gb gain[7:4], right Gb gain[3:0]           
    0x4b,0x12,  // top Gb gain[7:4], bottom Gb gain[3:0]           
    0x4c,0x01,   // left B gain[7:4], right B gain[3:0]             
    0x4d,0x11,   // top B gain[7:4], bottom B gain[3:0]             
    0x4e,0x00,  // X-axis center high[3:2], Y-axis center high[1:0]
    0x4f,0xa0,  // X-axis center low[7:0]                          
    0x50,0x80,  // Y-axis center low[7:0]                          
    0x51,0x80,  // Shading Center Gain                             
    0x52,0x00,   // Shading R Offset                                
    0x53,0x00,   // Shading Gr Offset                               
    0x54,0x00, // Shading Gb Offset                               
    0x55,0x00,   // Shading B Offset  

	// Lowlux Shading
	0x56,0x10,
	0x57,0x92,
	0x58,0x00,

	//Filter
	0x61,0xB4,
	0x62,0xB4,
	0x63,0xB4,

	// Color Matrix (D65) - Daylight
	0x71,0x42,
	0x72,0xbf,
	0x73,0x00,
	0x74,0x0F,
	0x75,0x31,
	0x76,0x00,
	0x77,0x00,
	0x78,0xbc,
	0x79,0x44,

	// Color Matrix (D30) - CWF
	0x7A,0x39,
	0x7B,0xCd,
	0x7C,0xFa,
	0x7D,0x13,
	0x7E,0x2c,
	0x7F,0x02,
	0x80,0xF9,
	0x81,0xC6,
	0x82,0x41,

	// Color Matrix (D20) - A
	0x83,0x38,
	0x84,0xbe,
	0x85,0x0a,
	0x86,0x13,
	0x87,0x1e,
	0x88,0x0F,
	0x89,0xF3,
	0x8A,0xcc,
	0x8B,0x41,
	0x8c,0x10,
	                      
	// Edge - Green
	0x90,0x12, //Up Gain
	0x91,0x12, //Down Gain
	0x92,0x00,
	0x96,0x00,

	// Edge - Luminance
	0x9F,0x18,
	0xA0,0x18,
	0xA1,0x02,
	0xA2,0x02,
	0xA3,0x0a,
  	0xA4,0x0a,
  
	0xA8,0x10,
	0xA9,0x0f,
	0xAA,0x12,
	0xAB,0x82,
	0xAe,0x40,
	0xAf,0x86,

	// Color suppress
	0xB9,0x10, //Ilimininfo Start
	0xBA,0x30, //Slope

    0xc0,0x24,
	0xc1,0x00,
	0xc2,0x80,
	0xc3,0x00,
	0xc4,0xe0,

	0xDD,0x0F, // ENHCTRL
	0xDE,0xfA, // NOIZCTRL

	0xDF,0x1F,
	0xE0,0x60,
	0xE1,0x90,
	0xE2,0x08,
	0xE3,0x08,

	// Memory speed control
	0xE5,0x15,
	0xE6,0x28,
	0xE7,0x04,

	// AE on
	0x00,0x02,
	0x10,0x80,

	0x70,0xd4,	//RBNCTRL
	0x74,0x07,
	0x79,0x6f,

	// Sensor on
	0x00,0x01,
	0x03,0x01,	
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

		A8L_SetSensorPowerPin(1);
		sys_IF_ait_delay1ms(5);
		A8L_SetSensorEnablePin(1);
		sys_IF_ait_delay1ms(10);
		
		
		A8L_SetSensorResetPin(1); 
		sys_IF_ait_delay1ms(10);
		A8L_SetSensorResetPin(0);
		sys_IF_ait_delay1ms(50); //0x15_1021
		A8L_SetSensorResetPin(1); 
		sys_IF_ait_delay1ms(10);
		//Initial I2C I/F
		
		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
		sys_IF_ait_delay1ms(30);		
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
		gbSensorInited = 0;	
	}	
	
	return;
}
static u_short	Get_Sensor_ID(u_short*	SensorID)
{
	u_short	fRet,sensor_id=0;
	u_short i= 40;
	if(!gbSensorInited)
		Sensor_Enable(1);

	A8L_SetSenReg(0x00,0x00);
	
   	A8L_GetSenReg(SIV121A_ID_REG,&sensor_id); 
    
    MSG_FATAL("SIV121A_ID_REG = %x",sensor_id,0,0);
    
	*SensorID = sensor_id;
	
	A8L_GetSenReg(SIV121A_INFO_REG,&sensor_id);
    MSG_FATAL("SIV121A_INFO_REG = %x",sensor_id,0,0);

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
    {
    	return	A8L_SendSensorSettingTable(tab_addr,tab_len);
    }
    else
    {
    	return  fRet;
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

t_sensor_manager sensor_siv121a = 
{
    SIV121A_SENSOR_ID,
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


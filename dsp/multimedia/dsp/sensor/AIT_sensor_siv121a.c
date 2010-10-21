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

#define	SENSOR_I2C_TYPE				(A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS)
#define SENSOR_FULL_VH_POLARITY     (A8_SENSOR_HOR_POS|A8_SENSOR_VER_POS)
#define SENSOR_PREVIEW_LATCH_COUNT  (A8_PHASE_COUNTER_NUM_3)
#define SENSOR_FULL_LATCH_COUNT     (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_PREVIEW_YUV_FORMAT   (A8_SENSOR_YCBYCR) //(A8_SENSOR_CBYCRY) //
#define SENSOR_FULL_YUV_FORMAT      (A8_SENSOR_YCRYCB) //(A8_SENSOR_CBYCRY) //
#define SENSOR_LATCH_EDGE           (A8_SENSOR_POS_EDGE)

#define	SENSOR_I2C_ID  			    (0x66)
#define	SENSOR_PREVIEW_WIDTH  	    (640)
#define	SENSOR_PREVIEW_HEIGHT  	    (480)
#define	SENSOR_FULL_WIDTH  		    (640)
#define	SENSOR_FULL_HEIGHT  	    (480)

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
typedef unsigned char kal_uint8;
//typedef unsigned char kal_bool; 
typedef unsigned short kal_uint16;
static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;
unsigned char gc_effect_on;
unsigned char gc_reinit = 1;

uint8  siv121a_start_grab_x_offset=0, siv121a_start_grab_y_offset=0;
boolean  siv121a_gVGAmode=KAL_TRUE,siv121a_sensor_night_mode = KAL_FALSE;
boolean siv121a_MPEG4_encode_mode=KAL_FALSE;


static u_short	Get_Sensor_ID(u_short*	SensorID);

uint8  siv121a_normal_gain=0;//, siv121a_night_gain=GC_0307_SENSOR_NIGHT_MODE_GAIN;
uint8  siv121a_preview_pclk_division=0, siv121a_capture_pclk_division=0;
uint16 siv121a_dummy_pixels=0, siv121a_dummy_lines=0, siv121a_extra_exposure_lines=0;
uint16 siv121a_exposure_lines=0,siv121a_w_blank=0,siv121a_h_blank=0;
//kal_uint16 siv121a_sensor_global_gain=BASEGAIN, 
uint16 	siv121a_sensor_gain_base=0x0;
boolean siv121a_g_bMJPEG_mode;





__align(2)const static u_char	Sensor_Init[]=
{
//addr, data
// Initail Sequence Write In.

	//========= close output
		//0x18 ,0xf0,
	 	0x43  ,0x00, 
	 	0x44  ,0xa2, 
	
	//========= close some functions
	// open them after configure their parmameters
	 	0x40  ,0x10, 
	 	0x41  ,0x00, 			
	 	0x42  ,0x10,					  	
	 	0x47  ,0x00, 			  	
	 	0x48  ,0xc3, 
	 	0x49  ,0x00,	
	 	0x4a  ,0x00,
	 	0x4b  ,0x00, 
	 	0x4E  ,0x23,
	 	0x4F  ,0x01, 
	//========= frame timing
		0x01  ,0x6a, //HB
		0x02  ,0x0c, //VB
		0x1C  ,0x00, //Vs_st
		0x1D  ,0x00, //Vs_et
		0x10  ,0x00, //high 4 bits of VB, HB
		0x11  ,0x05, //row_tail,  AD_pipe_number
	
	
	//========= windowing
		0x05  ,0x00, //row_start
		0x06  ,0x00,
		0x07  ,0x00, //col start
		0x08  ,0x00, 
		0x09  ,0x01, //win height
		0x0A  ,0xE8,
		0x0B  ,0x02, //win width, pixel array only 640
		0x0C  ,0x80,
	
	//========= analog
		0x0D  ,0x22, //rsh_width
					  
		0x0E  ,0x02, //CISCTL mode2,  

			  
		0x12  ,0x70, //7 hrst, 6_4 darsg,
		0x13  ,0x00, //7 CISCTL_restart, 0 apwd
		0x14  ,0x00, //NA
		0x15  ,0xba, //7_4 vref
		0x16  ,0x13, //5to4 _coln_r,  __1to0__da18
		0x17  ,0x52, //opa_r, ref_r, sRef_r
	//write_cmos_sensor(	0x18  ,0xc0, //analog_mode, best case for left band.
	
		0x1E  ,0x0d, //tsp_width 		   
		0x1F  ,0x32, //sh_delay
	
	//========= offset
		0x47  ,0x00,  //7__test_image, __6__fixed_pga, __5__auto_DN, __4__CbCr_fix, 
				//__3to2__dark_sequence, __1__allow_pclk_vcync, __0__LSC_test_image
		0x19  ,0x06,  //pga_o			 
		0x1a  ,0x06,  //pga_e			 
	
		0x31  ,0x00,  //4	//pga_oFFset ,	 high 8bits of 11bits
		0x3B  ,0x00,  //global_oFFset, low 8bits of 11bits
	
		0x59  ,0x0f,  //offset_mode 	
		0x58  ,0x88,  //DARK_VALUE_RATIO_G,  DARK_VALUE_RATIO_RB
		0x57  ,0x08,  //DARK_CURRENT_RATE
		0x56  ,0x77,  //PGA_OFFSET_EVEN_RATIO, PGA_OFFSET_ODD_RATIO
	
	//========= blk
		0x35  ,0xd8,  //blk_mode

		0x36  ,0x40,  
	
		0x3C  ,0x00, 
		0x3D  ,0x00, 
		0x3E  ,0x00, 
		0x3F  ,0x00, 
	
		0xb5  ,0x70, 
		0xb6  ,0x40, 
		0xb7  ,0x00, 
		0xb8  ,0x38, 
		0xb9  ,0xc3, 		  
		0xba  ,0x0f, 
	
		0x7e  ,0x35, 
		0x7f  ,0x86, 
	
		0x5c  ,0x68, //78
		0x5d  ,0x78, //88
	
	
	//========= manual_gain 
		0x61  ,0x80, //manual_gain_g1	
		0x63  ,0x80, //manual_gain_r
		0x65  ,0x98, //manual_gai_b, 0xa0=1.25, 0x98=1.1875
		0x67  ,0x80, //manual_gain_g2
		0x68  ,0x18, //global_manual_gain	 2.4bits
	
	//=========CC _R
		0x69  ,0x58,  //54
		0x6A  ,0xf6,  //ff
		0x6B  ,0xfb,  //fe
		0x6C  ,0xf4,  //ff
		0x6D  ,0x5a,  //5f
		0x6E  ,0xe6,  //e1

		0x6f  ,0x00, 	
	
	//=========lsc							  
		0x70  ,0x14, 
		0x71  ,0x1c, 
		0x72  ,0x20, 
	
		0x73  ,0x10, 	
		0x74  ,0x3c, 
		0x75  ,0x52, 
	
	//=========dn																			 
		0x7d  ,0x2f,  //dn_mode   	
		0x80  ,0x0c, //when auto_dn, check 7e,7f
		0x81  ,0x0c,
		0x82  ,0x44,
																						
	//dd																		   
		0x83  ,0x18,  //DD_TH1 					  
		0x84  ,0x18,  //DD_TH2 					  
		0x85  ,0x04,  //DD_TH3 																							  
		0x87  ,0x34,  //32 b DNDD_low_range X16,  DNDD_low_range_C_weight_center					
	
	   
	//=========intp-ee																		   
		0x88  ,0x04,  													   
		0x89  ,0x01,  										  
		0x8a  ,0x50,//60  										   
		0x8b  ,0x50,//60  										   
		0x8c  ,0x07,  												 				  
																					  
		0x50  ,0x0c,   						   		
		0x5f  ,0x3c, 																					 
																					 
		0x8e  ,0x02,  															  
		0x86  ,0x02,  																  
																					
		0x51  ,0x20,  																
		0x52  ,0x08,  
		0x53  ,0x00, 
	
	
	//========= YCP 
	//contrast_center																			  
		0x77  ,0x80, //contrast_center 																  
		0x78  ,0x00, //fixed_Cb																		  
		0x79  ,0x00, //fixed_Cr																		  
		0x7a  ,0x00, //luma_offset 																																							
		0x7b  ,0x40, //hue_cos 																		  
		0x7c  ,0x00, //hue_sin 																		  
																							 
	//saturation																				  
		0xa0  ,0x40, //global_saturation
		0xa1  ,0x40, //luma_contrast																	  
		0xa2  ,0x34, //saturation_Cb																	  
		0xa3  ,0x34, //saturation_Cr
																				
		0xa4  ,0xc8, 																  
		0xa5  ,0x02, 
		0xa6  ,0x28, 																			  
		0xa7  ,0x02, 
	
	//skin																								  
		0xa8  ,0xee, 															  
		0xa9  ,0x12, 															  
		0xaa  ,0x01, 														  
		0xab  ,0x20, 													  
		0xac  ,0xf0, 														  
		0xad  ,0x10, 															  
		
	//========= ABS
		0xae  ,0x18, 
		0xaf  ,0x74, 
		0xb0  ,0xe0, 	  
		0xb1  ,0x20, 
		0xb2  ,0x6c, 
		0xb3  ,0x40, 
		0xb4  ,0x04, 
		
	//========= AWB 
		0xbb  ,0x42, 
		0xbc  ,0x60, 
		0xbd  ,0x50, 
		0xbe  ,0x50, 
	
		0xbf  ,0x0c, 
		0xc0  ,0x06, 
		0xc1  ,0x60, 
		0xc2  ,0xf1,  //f4
		0xc3  ,0x40, 
		0xc4  ,0x1c, //18
		0xc5  ,0x56, 
		0xc6  ,0x1d,   

		0xca  ,0x70, 
		0xcb  ,0x70, 
		0xcc  ,0x78,
	
		0xcd  ,0x80, //R_ratio 									 
		0xce  ,0x80, //G_ratio  , cold_white white 								   
		0xcf  ,0x80, //B_ratio  	
	
	//=========  aecT  
		0x20  ,0x06, 
		0x21  ,0xc0, 
		0x22  ,0x60,    
		0x23  ,0x88, 
		0x24  ,0x96, 
		0x25  ,0x30, 
		0x26  ,0xd0, 
		0x27  ,0x00, 
	
		0x28  ,0x01, //AEC_exp_level_1bit11to8   
		0x29  ,0xf4, //AEC_exp_level_1bit7to0	  
		0x2a  ,0x02, //AEC_exp_level_2bit11to8   
		0x2b  ,0xbc, //AEC_exp_level_2bit7to0			 
		0x2c  ,0x03, //AEC_exp_level_3bit11to8   659 - 8FPS,  8ca - 6FPS  //	 
		0x2d  ,0xe8, //AEC_exp_level_3bit7to0			 
		0x2e  ,0x09, //AEC_exp_level_4bit11to8   4FPS 
		0x2f  ,0xc4, //AEC_exp_level_4bit7to0	 
	
		0x30  ,0x20, 						  
		0x31  ,0x00, 					   
		0x32  ,0x1c, 
		0x33  ,0x90, 			  
		0x34  ,0x10,	

	0xFF, 0xFF,
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
	0x01,0x8c,
	0x02,0x28,
	0xd6,0x58,
	0xff,0xff
};
__align(2)const static u_char  SENSOR_DEBAND_60[] = 
{
	0x01,0x9d,
	0x02,0x1e,
	0xd6,0x48,
	0xff,0xff
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
		
		//A800_SetGPIO(AIT_GPIO_SENSOR_POWER_CTL);		    
		A8L_SetSensorPowerPin(1);
		sys_IF_ait_delay1ms(5);
		A8L_SetSensorEnablePin(1);
		
		sys_IF_ait_delay1ms(10);
		
		
		A8L_SetSensorResetPin(1); 
		sys_IF_ait_delay1ms(10);
		A8L_SetSensorResetPin(0);
		sys_IF_ait_delay1ms(15); 
		A8L_SetSensorResetPin(1); 
		sys_IF_ait_delay1ms(10);
		//Initial I2C I/F

		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
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
	u_short	fRet,sensor_id=0;
	u_short i= 40;
	if(!gbSensorInited)
		Sensor_Enable(1);

	A8L_SetSenReg(0x00,0x00);
	
   	A8L_GetSenReg(SIV121A_ID_REG,&sensor_id); 
    
    MSG_FATAL("SIV121A_ID_REG = %x",sensor_id,0,0);
    

	A8L_GetSenReg(SIV121A_INFO_REG,&sensor_id);
    MSG_FATAL("SIV121A_INFO_REG = %x",sensor_id,0,0);

	if(!gbSensorInited)
		Sensor_Enable(0);
		
	return	fRet;
}

static u_short ChangeSensorAE(void)
{
	return 0;
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
				fRet = A8L_SendSensorSettingTable(tab_addr,tab_len);
				sys_IF_ait_delay1ms(20);
				A8L_SetSenReg(0x43  ,0x40);
				A8L_SetSenReg(0x44  ,0xE2);				
                gbSensorInited = 1;
                return fRet;
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


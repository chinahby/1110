
#if defined(__AIT701_DSP__)
#include	"A8_common.h"
#include	"AIT700_EBIbus.h"
#include	"A8_sensor.h"
#include "med_api.h"
#include "AIT_interface.h"

#if 1//defined(SENSOR_HYNIX)
#define SENSOR_ID                   (0x81)
#define	SENSOR_I2C_TYPE  		    (A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_FULL_VH_POLARITY     (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_PREVIEW_LATCH_COUNT  (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_FULL_LATCH_COUNT     (A8_PHASE_COUNTER_NUM_2)
#define SENSOR_PREVIEW_YUV_FORMAT   (A8_SENSOR_YCBYCR) //(A8_SENSOR_CBYCRY)
#define SENSOR_FULL_YUV_FORMAT      (A8_SENSOR_YCBYCR) //(A8_SENSOR_CBYCRY)
#define SENSOR_LATCH_EDGE           (A8_SENSOR_NEG_EDGE)

#define	SENSOR_I2C_ID  			    (0x60)
#define	SENSOR_PREVIEW_WIDTH  	    (640)
#define	SENSOR_PREVIEW_HEIGHT  	    (480)
#define	SENSOR_FULL_WIDTH  		    (640)
#define	SENSOR_FULL_HEIGHT  	    (480)

typedef unsigned char kal_uint8;
//typedef unsigned char kal_bool; 
typedef unsigned short kal_uint16;
static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;

u_short hynix_read_cmos_sensor(u_short Reg)
{
	u_short ret;
	A8L_GetSenReg(Reg,&ret);
	return ret;
}

u_short hynix_write_cmos_sensor(u_short Reg, u_short Value)
{

	return A8L_SetSenReg(Reg,Value);

}

__align(2)const static u_char	Sensor_Init[]=
{
/*
	0x03, 0x00,
	0x11, 0x93,
	0x12, 0x05,  ////[B:2] pclk inversion(0x04)
	
	0x03, 0x02,
	0x1a, 0x21,
	
	0x03, 0x10,
	0x10, 0x03,
	
	0x03, 0x00,
	0x01, 0xf0,
	
	0xff, 0xff,
	*/
	
#if 1
	
	0x03, 0x20,  //page 3
	0x10, 0x0c,  //ae off
	0x03, 0x22,  //page 4
	0x10, 0x69,  //awb off 
	          
	0x03, 0x00,
	0x11, 0x93,
	0x12, 0x05,  ////[B:2] pclk inversion(0x04) 0x05,13M         
	0x80, 0x0e,
	0x90, 0x0a,
	0x91, 0x0a,
	0x92, 0x60,
	          
	0xa0, 0x46,
	0xa1, 0x46,
	0xa2, 0x45,
	0xa3, 0x45,
	0xa4, 0x48,
	0xa5, 0x48,
	0xa6, 0x44,
	0xa7, 0x44,
	
	0xa8, 0x45,
	0xa9, 0x45,
	0xaa, 0x45,
	0xab, 0x45,
	0xac, 0x45,
	0xad, 0x45,
	0xae, 0x45,
	0xaf, 0x45,
	          
	0x03, 0x02,
	0x1a, 0x21,
	          
	0x1c, 0x00,
	0x1d, 0x03,
	          
	0x20, 0x33,
	0x21, 0x77,
	0x22, 0xad,
	0x34, 0xff,
	          
	0x62, 0x78,
	0x63, 0x7a,
	0x64, 0x7d,
	0x65, 0x88,
	          
	0x72, 0x78,
	0x73, 0x8b,
	0x74, 0x78,
	0x75, 0x8b,
	          
	0xa0, 0x03,
	0xa8, 0x03,
	0xaa, 0x03,
	          
	0x03, 0x10,
	0x10, 0x03,  ////OutputFormat data (0x00:VYUY or 0x03:YUYV)
	0x12, 0x30,
	0x40, 0x00,
	0x41, 0x20,
	0x50, 0xc0,
	          
	0x60, 0x1f,
	0x61, 0xa8,
	0x62, 0xa8,
	0x63, 0x60,
	0x64, 0xa0,
	          
	0x03, 0x11,
	0x10, 0x1d,
	0x11, 0x0a,
	0x60, 0x08,
	0x61, 0x98,
	0x62, 0x98,
	0x63, 0x63,
	          
	0x03, 0x12,
	0x40, 0x21,
	0x41, 0x07,
	0x50, 0x03,
	0x70, 0x1d,
	0x74, 0x04,
	0x75, 0x06,
	0x90, 0x01,
	0x91, 0x10,
	          
	0x03, 0x13,
	0x10, 0x29,
	          
	0x11, 0x07,
	0x12, 0x01,
	0x13, 0x02,
	          
	0x20, 0x05,
	0x21, 0x04,  //edge
	0x23, 0x18,
	0x24, 0x03,
	0x29, 0xd0,          
	0x80, 0x0d,
	0x81, 0x01,
	          
	0x83, 0x51,
	0x83, 0x55,
	0x83, 0x5d,
	          
	0x90, 0x05,
	0x91, 0x04,
	0x93, 0x19,
	0x94, 0x03,
	0x95, 0x00,
	          
	0x03, 0x14,
	0x10, 0x07,
	0x22, 0x80,
	0x23, 0x62,
	0x24, 0x52,
	          
	0x03, 0x15,
	0x10, 0x0f,
	0x14, 0x36,
	0x16, 0x28,
	0x17, 0x3f,
	          
	0x30, 0x73,
	0x31, 0x36,
	0x32, 0x02,
	0x33, 0x12,
	0x34, 0x6d,
	0x35, 0x1a,
	0x36, 0x0c,
	0x37, 0x27,
	0x38, 0x73,
	          
	0x40, 0x00,
	0x41, 0x00,
	0x42, 0x00,
	0x43, 0x8b,
	0x44, 0x07,
	0x45, 0x04,
	0x46, 0x84,
	0x47, 0xa1,
	0x48, 0x25,
	          
	0x80, 0x00,
	0x84, 0x80,
	0x85, 0x80,
	0x86, 0x80,
	          
	0x03, 0x16,
	0x10, 0x01,
	0x30, 0x00,
	0x31, 0x0b,
	0x32, 0x20,
	0x33, 0x36,
	0x34, 0x5b,
	0x35, 0x75,
	0x36, 0x8c,
	0x37, 0x9f,
	0x38, 0xaf,
	0x39, 0xbd,
	0x3a, 0xca,
	0x3b, 0xdd,
	0x3c, 0xec,
	0x3d, 0xf7,
	0x3e, 0xff,
	          
	0x60, 0x01,
	0x61, 0x03,
	0x62, 0x08,
	0x63, 0x17,
	0x64, 0x2c,
	0x65, 0x59,
	0x66, 0x74,
	0x67, 0x8c,
	0x68, 0xa0,
	0x69, 0xb1,
	0x6A, 0xc0,
	0x6B, 0xcd,
	0x6C, 0xe1,
	0x6D, 0xf1,
	0x6E, 0xfc,
	0x6F, 0xff,
	          
	0x03, 0x20,
	0x11, 0x00,
	0x15, 0xd1,
	0x1c, 0x50,
	0x1d, 0x24,
	0x1e, 0x28,
	0x20, 0x00,
	0x28, 0x0b,
	0x29, 0xaa,
	0x2a, 0xf0,
	0x2b, 0x34,
	0x2d, 0x07,
	0x30, 0xf8,
	0x33, 0x6c,
	0x34, 0x30,
	0x35, 0xd4,
	0x36, 0x00,
	0x37, 0x50,
	          
	0x38, 0x02,
	0x39, 0x49,
	0x3a, 0xF0,
	0x3b, 0x27,
	0x3c, 0x10,
	0x3d, 0x30,
	0x3e, 0xd4,
	          
	0x50, 0x28,
	0x51, 0x10,
	0x52, 0xd0,
	0x53, 0x10,
	0x54, 0x10,
	0x55, 0x28,
	0x57, 0x60,
	0x58, 0x60,
	          
	0x60, 0x00,
	0x70, 0x42,
    
	0x83, 0x00,
	0x84, 0xbe,  //30fps
	0x85, 0x6e,

/*
test
	0x83, 0x04,
	0x84, 0xf5,  //5fps
	0x85, 0x88,

	0x88, 0x04,
	0x89, 0xf5,  //5fps
	0x8a, 0x88,

*/	
	0x8b, 0x3f,
	0x8c, 0x7a,
	0x8d, 0x34,
	0x8e, 0xe5,
	          
	0x8f, 0xc4,
	0x90, 0x68,
	0x98, 0xac,

	0x9c, 0x0a,
	0x9d, 0x41,
	
	0x9e, 0x00,
	0x9f, 0xfa,
	          
	0xb0, 0x18,
	0xb1, 0x18,
	0xb2, 0x60,
	0xb3, 0x18,
	0xb4, 0x18,
	          
	0xb5, 0x41,
	0xb6, 0x2c,
	0xb7, 0x25,
	0xb8, 0x22,
	0xb9, 0x20,
	0xba, 0x1e,
	0xbb, 0x1d,
	0xbc, 0x1d,
	0xbd, 0x1c,
	0xc0, 0x1a,
	
	0xc3, 0xd0,
	0xc4, 0xd0,
	
	0x03, 0x22,// page 22
	0x11, 0x2c,
	0x21, 0x01,
	0x38, 0x12,
	0x40, 0xe3,
	0x41, 0x88,
	0x42, 0x44,
	          
	0x43, 0xe3,
	0x44, 0xee,
	0x45, 0x99,
	0x46, 0x01,
	          
	0x83, 0x70,
	0x84, 0x1a,
	0x85, 0x70,
	0x86, 0x22,
	0x87, 0x50,
	0x88, 0x20,
	0x89, 0x60,
	0x8a, 0x20,
	0x8f, 0x68,
	0x90, 0x67,
	0x91, 0x65,
	0x92, 0x61,
	0x93, 0x5c,
	0x94, 0x55,
	0x95, 0x4f,
	0x96, 0x48,
	0x97, 0x42,
	0x98, 0x3c,
	0x99, 0x35,
	0x9a, 0x2d,
	0x9b, 0x07,
	0x10, 0xea, ////AWB ON
	          
	0x03, 0x20,
	0x10, 0x9c, ////AE ON
	0x01, 0xf0,

	#endif
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
#if 0
0x03, 0x00,
0x11, 0x80,
#else
NULL,
#endif
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR[] = 
{
#if 0
0x03, 0x00,
0x11, 0x81,
#else
NULL,
#endif
};
__align(2)const static u_char  SENSOR_ROTATE_FLIP[] = 
{
#if 0
0x03, 0x00,
0x11, 0x82,
#else
NULL,
#endif
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_FLIP[] = 
{
#if 0
0x03, 0x00,
0x11, 0x83,
#else
NULL,
#endif
};

static u_short	Sensor_Effect(u_short EffectMode)
{
	u_short	*tab_addr,tab_len;
	u_char  temp_reg;
	u_short ret = 0;
#if 0
	switch(EffectMode)
	{
		case CAM_EFFECT_ENC_NORMAL:		
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x11, 0x03);
			hynix_write_cmos_sensor(0x12, 0x30);
			hynix_write_cmos_sensor(0x13, 0x00);
			hynix_write_cmos_sensor(0x44, 0x80);
			hynix_write_cmos_sensor(0x45, 0x80);
			hynix_write_cmos_sensor(0x47, 0x7f);
			hynix_write_cmos_sensor(0x20, 0x07);
			hynix_write_cmos_sensor(0x21, 0x03);
			break;	
		
		case CAM_EFFECT_ENC_GRAYSCALE:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x11, 0x03);
			hynix_write_cmos_sensor(0x12, 0x23);
			hynix_write_cmos_sensor(0x13, 0x00);
			hynix_write_cmos_sensor(0x44, 0x80);
			hynix_write_cmos_sensor(0x45, 0x80);
			hynix_write_cmos_sensor(0x47, 0x7f);
			hynix_write_cmos_sensor(0x20, 0x07);
			hynix_write_cmos_sensor(0x21, 0x03);
			break;	
			
		case CAM_EFFECT_ENC_SEPIA:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x11, 0x03);
			hynix_write_cmos_sensor(0x12, 0x23);
			hynix_write_cmos_sensor(0x13, 0x00);
			hynix_write_cmos_sensor(0x44, 0x70);
			hynix_write_cmos_sensor(0x45, 0x98);
			hynix_write_cmos_sensor(0x47, 0x7f);
			hynix_write_cmos_sensor(0x20, 0x07);
			hynix_write_cmos_sensor(0x21, 0x03);
			break;		
			
		case CAM_EFFECT_ENC_COLORINV:		
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x11, 0x03);
			hynix_write_cmos_sensor(0x12, 0x28);
			hynix_write_cmos_sensor(0x13, 0x00);
			hynix_write_cmos_sensor(0x44, 0x80);
			hynix_write_cmos_sensor(0x45, 0x80);
			hynix_write_cmos_sensor(0x47, 0x7f);
			hynix_write_cmos_sensor(0x20, 0x07);
			hynix_write_cmos_sensor(0x21, 0x03);
			break;		
			
		case CAM_EFFECT_ENC_SEPIAGREEN:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x11, 0x03);
			hynix_write_cmos_sensor(0x12, 0x33);
			hynix_write_cmos_sensor(0x13, 0x00);
			hynix_write_cmos_sensor(0x44, 0x30);
			hynix_write_cmos_sensor(0x45, 0x50);
			hynix_write_cmos_sensor(0x47, 0x7f);
			hynix_write_cmos_sensor(0x20, 0x07);
			hynix_write_cmos_sensor(0x21, 0x03);
			break;	
			
		case CAM_EFFECT_ENC_SEPIABLUE:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x11, 0x03);
			hynix_write_cmos_sensor(0x12, 0x33);
			hynix_write_cmos_sensor(0x13, 0x00);
			hynix_write_cmos_sensor(0x44, 0xb0);
			hynix_write_cmos_sensor(0x45, 0x40);
			hynix_write_cmos_sensor(0x47, 0x7f);
			hynix_write_cmos_sensor(0x20, 0x07);
			hynix_write_cmos_sensor(0x21, 0x03);
			break;		
			
		case CAM_EFFECT_ENC_GRAYINV:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x11, 0x03);
			hynix_write_cmos_sensor(0x12, 0x28);
			hynix_write_cmos_sensor(0x13, 0x00);
			hynix_write_cmos_sensor(0x44, 0x80);
			hynix_write_cmos_sensor(0x45, 0x80);
			hynix_write_cmos_sensor(0x47, 0x7f);
			hynix_write_cmos_sensor(0x20, 0x07);
			hynix_write_cmos_sensor(0x21, 0x03);
			break;
			
		case CAM_EFFECT_ENC_COPPERCARVING:
			break;								
		case CAM_EFFECT_ENC_BLUECARVING:
			break;								
		case CAM_EFFECT_ENC_CONTRAST:
			break;		
			
		case CAM_EFFECT_ENC_EMBOSSMENT:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x11, 0x23);
			hynix_write_cmos_sensor(0x12, 0x33);
			hynix_write_cmos_sensor(0x13, 0x02);
			hynix_write_cmos_sensor(0x44, 0x80);
			hynix_write_cmos_sensor(0x45, 0x80);
			hynix_write_cmos_sensor(0x47, 0x7f);
			hynix_write_cmos_sensor(0x20, 0x07);
			hynix_write_cmos_sensor(0x21, 0x07);
			break;		
			
		case CAM_EFFECT_ENC_SKETCH:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x11, 0x13);
			hynix_write_cmos_sensor(0x12, 0x38);
			hynix_write_cmos_sensor(0x13, 0x02);
			hynix_write_cmos_sensor(0x44, 0x80);
			hynix_write_cmos_sensor(0x45, 0x80);
			hynix_write_cmos_sensor(0x47, 0x7f);
			hynix_write_cmos_sensor(0x20, 0x07);
			hynix_write_cmos_sensor(0x21, 0x07);
			break;		
			
		case CAM_EFFECT_ENC_BLACKBOARD:
			break;	
			
		case CAM_EFFECT_ENC_JEAN:
		case CAM_EFFECT_ENC_OIL:			
		case CAM_EFFECT_ENC_WHITEBOARD:
			break;			

		default:
		ret = 0;
	}
#endif	
	return 0;
}

static u_short	Sensor_EV(u_short EvStep)
{
	u_short	*tab_addr,tab_len;
	u_short fRet = 0;
	return 0;
	switch(EvStep)
	{
		case CAM_EV_NEG_4_3: 
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x40, 0xc0);//0xb0
			break;		
			
		case CAM_EV_NEG_3_3:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x40, 0xb0);//0xb0
			break;	
			
		case CAM_EV_NEG_2_3:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x40, 0xa0);//0xb0
			break;	
			
		case CAM_EV_NEG_1_3:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x40, 0x90);//0xb0
			break;	
			
		case CAM_EV_ZERO:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x40, 0x00);//0xb0
			break;		
			
		case CAM_EV_POS_1_3:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x40, 0x10);//0xb0
			break;		
			
		case CAM_EV_POS_2_3:

			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x40, 0x20);//0xb0
			break;	
			
		case CAM_EV_POS_3_3:
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x40, 0x30);//0xb0
			break;	
			
		case CAM_EV_POS_4_3:	
			hynix_write_cmos_sensor(0x03, 0x10);
			hynix_write_cmos_sensor(0x40, 0x40);//0xb0
			break;
			
		default:
			return 0;    
	}			

	return 0;
}


static u_short	Sensor_WhiteBalance(u_short WbMode)
{
	u_short	*tab_addr,tab_len;
	u_short fRet = 0;
#if 0
	
	switch(WbMode)
	{
		case CAM_WB_AUTO:
			hynix_write_cmos_sensor(0x03,0x22);  // AUTO 3000K~7000K                                     
			hynix_write_cmos_sensor(0x10,0x6a);                                                          
			hynix_write_cmos_sensor(0x80,0x38);                                                          
			hynix_write_cmos_sensor(0x81,0x20);		                                                       
			hynix_write_cmos_sensor(0x82,0x30);		                                                       
			hynix_write_cmos_sensor(0x83,0x65);		                                                       
			hynix_write_cmos_sensor(0x84,0x1a);		                                                       
			hynix_write_cmos_sensor(0x85,0x58);		                                                       
			hynix_write_cmos_sensor(0x86,0x22);                                                          
			hynix_write_cmos_sensor(0x03,0x22);                                                          
			hynix_write_cmos_sensor(0x10,0xea);                                                         
			break;	
	  
		case CAM_WB_CLOUD:
			hynix_write_cmos_sensor(0x03,0x22);   //7000K                                     
			hynix_write_cmos_sensor(0x10,0x6a);                                                          
			hynix_write_cmos_sensor(0x80,0x50);                                                          
			hynix_write_cmos_sensor(0x81,0x20);		                                                       
			hynix_write_cmos_sensor(0x82,0x24);		                                                       
			hynix_write_cmos_sensor(0x83,0x6d);		                                                       
			hynix_write_cmos_sensor(0x84,0x65);		                                                       
			hynix_write_cmos_sensor(0x85,0x24);		                                                       
			hynix_write_cmos_sensor(0x86,0x1c);                                                          
			break;		

		case CAM_WB_DAYLIGHT:   // tai yang guang
			hynix_write_cmos_sensor(0x03,0x22);  //6500K                                     
			hynix_write_cmos_sensor(0x10,0x6a);                                                          
			hynix_write_cmos_sensor(0x80,0x40);                                                          
			hynix_write_cmos_sensor(0x81,0x20);		                                                       
			hynix_write_cmos_sensor(0x82,0x28);		                                                       
			hynix_write_cmos_sensor(0x83,0x45);		                                                       
			hynix_write_cmos_sensor(0x84,0x35);		                                                       
			hynix_write_cmos_sensor(0x85,0x2d);		                                                       
			hynix_write_cmos_sensor(0x86,0x1c);                                                          
			break;		

		case CAM_WB_INCANDESCENCE:   // bai re guang
			hynix_write_cmos_sensor(0x03,0x22);  //2800K~3000K                                     
			hynix_write_cmos_sensor(0x10,0x69);                                                          
			hynix_write_cmos_sensor(0x80,0x25);                                                          
			hynix_write_cmos_sensor(0x81,0x20);		                                                       
			hynix_write_cmos_sensor(0x82,0x44);		                                                       
			hynix_write_cmos_sensor(0x83,0x24);		                                                       
			hynix_write_cmos_sensor(0x84,0x1e);		                                                       
			hynix_write_cmos_sensor(0x85,0x50);		                                                       
			hynix_write_cmos_sensor(0x86,0x45);                                                          
			break;		

		case CAM_WB_FLUORESCENT:   //ri guang deng
			hynix_write_cmos_sensor(0x03,0x22);  //4200K~5000K                                     
			hynix_write_cmos_sensor(0x10,0x69);                                                          
			hynix_write_cmos_sensor(0x80,0x35);                                                          
			hynix_write_cmos_sensor(0x81,0x20);		                                                       
			hynix_write_cmos_sensor(0x82,0x32);		                                                       
			hynix_write_cmos_sensor(0x83,0x3c);		                                                       
			hynix_write_cmos_sensor(0x84,0x2c);		                                                       
			hynix_write_cmos_sensor(0x85,0x45);		                                                       
			hynix_write_cmos_sensor(0x86,0x35);                                                          
			break;		

		case CAM_WB_TUNGSTEN:   // wu si deng
			hynix_write_cmos_sensor(0x03,0x22);  //4000K                                   
			hynix_write_cmos_sensor(0x10,0x69);                                                          
			hynix_write_cmos_sensor(0x80,0x33);                                                          
			hynix_write_cmos_sensor(0x81,0x20);		                                                       
			hynix_write_cmos_sensor(0x82,0x3d);		                                                       
			hynix_write_cmos_sensor(0x83,0x2e);		                                                       
			hynix_write_cmos_sensor(0x84,0x24);		                                                       
			hynix_write_cmos_sensor(0x85,0x43);		                                                       
			hynix_write_cmos_sensor(0x86,0x3d);                                                          
			break;

		case CAM_WB_MANUAL: 	
			// TODO
			break;		

		default:
			return 0;			
	}	
#endif	
	return 0;		
}


static u_short	Sensor_NightMode(mmp_sensor_night_mode NightMode)
{
	u_short	*tab_addr,tab_len;
	u_short temp=0;
#if 0
	switch(NightMode)
	{
		case	A8_SENSOR_NIGHT_MODE_OFF:
			// MCLK=26M
			hynix_write_cmos_sensor(0x03, 0x20);
			hynix_write_cmos_sensor(0x10, 0x1c); ////AE Off
			hynix_write_cmos_sensor(0x88, 0x02);//EXP_M -> 10FPS
			hynix_write_cmos_sensor(0x89, 0xca);
			hynix_write_cmos_sensor(0x8a, 0x74);

			/*
			test
			hynix_write_cmos_sensor(0x83, 0x04);//EXP_M -> 5FPS  
			hynix_write_cmos_sensor(0x84, 0xf5);
			hynix_write_cmos_sensor(0x85, 0x88);

			hynix_write_cmos_sensor(0x88, 0x04);//EXP_M -> 5FPS  
			hynix_write_cmos_sensor(0x89, 0xf5);
			hynix_write_cmos_sensor(0x8a, 0x88);
			*/

			hynix_write_cmos_sensor(0x70, 0x48);
			hynix_write_cmos_sensor(0xb2, 0x80);
			hynix_write_cmos_sensor(0x11, 0x00);
			hynix_write_cmos_sensor(0x10, 0x9c);//AE On
			break;		
			
		default:
			hynix_write_cmos_sensor(0x03, 0x20);
			hynix_write_cmos_sensor(0x10, 0x1c);//AE Off
			hynix_write_cmos_sensor(0x88, 0x04);//EXP_M -> 5FPS  
			hynix_write_cmos_sensor(0x89, 0xf5);
			hynix_write_cmos_sensor(0x8a, 0x88);
			hynix_write_cmos_sensor(0x70, 0x70);
			hynix_write_cmos_sensor(0xb2, 0xd0);
			hynix_write_cmos_sensor(0x11, 0x00);
			hynix_write_cmos_sensor(0x10, 0x9c);//AE On	
			break;	
	}		
#endif	
   	return A8_NO_ERROR;//	A8L_SendSensorSettingTable(tab_addr,tab_len);	
}

static u_short	Sensor_DeBand(mmp_sensor_deband DeBandMode)
{
	kal_uint8 banding;
#if 0
	switch(DeBandMode)
	{
		case CAM_BANDING_50HZ:
			hynix_write_cmos_sensor(0x03, 0x20);
			hynix_write_cmos_sensor(0x10, 0x9c);
			break;   
			
		case CAM_BANDING_60HZ:
			hynix_write_cmos_sensor(0x03, 0x20);
			hynix_write_cmos_sensor(0x10, 0x8c);
			break;
			
		default:
			return 0;		
	}	
#endif	
	return 0;		
}


static u_short	Sensor_Rotate(mmp_sensor_rotate Rotate)
{

	u_short	*tab_addr,tab_len;
#if 0   
	switch (Rotate)
	{
		case A8_SENSOR_ROTATE_NORMAL:
			break;
			
		case A8_SENSOR_ROTATE_MIRROR:
			break;
			
		case A8_SENSOR_ROTATE_FILP:			
			break;
			
		case A8_SENSOR_ROTATE_MIRROR_FLIP:      // 180
			break;
	}	
#endif	
   	return	0;		
}

static u_short 	Sensor_FrameRate(u_short FrameRate)
{
}

#if 1 //Antonio modify
extern ePREVIEW_SRC_MODE	gsMasterSlaveSensor;
static void	Sensor_Enable(u_short enable)
{
	if(enable) 
	{	
		//A8L_VIF_Init(&preview_mode);	//
		
		if (1/* g_ATV_Flag == 0*/){
			AIT_Message_P0("===Hynix===ATV turn Off, Hynix turn on \n\r");
			//sensor power LDO
			A8L_SetSensorPowerPin(1);
			//AIT701_SENSOR_LDO_HIGH;
			sys_IF_ait_delay1ms(10);
			SetA8RegB(0x7126 /*VIF_PXL_CLK_DELAY*/,0x02); // Pixel clcok latch delay for Hynix sensor
			A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
			A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
			A8L_SetSensorResetPin(1);
			sys_IF_ait_delay1ms(10);

			//switch master and slave PWDN
			if(gsMasterSlaveSensor==AIT_MASTER_SENSOR_SRC /*AIT_SLAVE_SENSOR_SRC*/)
			{
				A8L_SetSensorEnablePin(1);//for master sensor, Hynix high enable
				A800_ClearGPIO(AIT_GPIO_SUB_SENSOR_ENABLE);
				
			}
#if AIT_SUPPORT_DUAL_SENSOR			
			else
			{
				A800_SetGPIO(AIT_GPIO_SUB_SENSOR_ENABLE);//for slave sensor, high enable
				A8L_SetSensorEnablePin(0);
		
			}
#endif			
			sys_IF_ait_delay1ms(10);
			
			A8L_VIF_Init(&preview_mode);
			sys_IF_ait_delay1ms(8);
			
			//Reset
			A8L_SetSensorResetPin(1); 
			sys_IF_ait_delay1ms(10);
			A8L_SetSensorResetPin(0);
			sys_IF_ait_delay1ms(10); 
			A8L_SetSensorResetPin(1); 
			sys_IF_ait_delay1ms(10);

	              //SW reset sensro??
			//A8L_SetSenReg(0x12, 0x80);
			//sys_IF_ait_delay1ms(8);
    		
		}

	        		
	} else {
	AIT_Message_P0("===Hynix===ATV / Hynix all turn off \n\r");
        A8L_SetSensorResetPin(0);
	sys_IF_ait_delay1ms(1);

	//Turn off Sensor MCLK
	A8L_SensorClockOutput(A8_OFF);
	//Sensor off
	A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_OFF);
	A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_OFF);	
	//A8L_SetSensorResetPin(0);
	A8L_SetSensorEnablePin(0); //low disable sensor
#if AIT_SUPPORT_DUAL_SENSOR
	A800_ClearGPIO(AIT_GPIO_SUB_SENSOR_ENABLE); //low disable sensor
#endif
	
	AIT701_SENSOR_LDO_LOW;
	sys_IF_ait_delay1ms(5); 

	}
}

#else

static void	Sensor_Enable(u_short enable)
{
	if(enable) {
		//		A810H_PresetSensor(0, SENSOR_I2C_ID, SENSOR_I2C_TYPE, 3);
		//		A810H_SetPinPull(0x69D1, PIN_PULL_HIGH);	//PSEN pull up
		//		A810H_SetPinPull(0x69D2, PIN_PULL_HIGH);	//PSDA pull up
		//		A810H_SetPinPull(0x69D3, PIN_PULL_HIGH);	//PSCK pull up
		A8L_VIF_Init(&preview_mode);
		//		AITS_I2C_Init();

		AIT701_SENSOR_LDO_HIGH;
		AIT701_SENSOR_RESET_HIGH;
		sys_IF_ait_delay1ms(10);

		//sensor PWDN
		#ifdef __SWITCH_CAMERA_SUPPORT__
		if(get_current_camera() == SECONDARY_CAMERA){
			AIT701_MAIN_SEN_EN_LOW;
			AIT701_SUB_SEN_EN_HIGH;
		}else{
			AIT701_SUB_SEN_EN_LOW;
			AIT701_MAIN_SEN_EN_HIGH;		
		}
		#else
			AIT701_MAIN_SEN_EN_HIGH;
		#endif
		sys_IF_ait_delay1ms(10);       

		AIT701_SENSOR_RESET_LOW;
		sys_IF_ait_delay1ms(10); 
		AIT701_SENSOR_RESET_HIGH;

		//Initial I2C I/F
		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
		sys_IF_ait_delay1ms(1);

		A8L_SetSenReg(0x03, 0x00);
		A8L_SetSenReg(0x01, 0xC1);
		A8L_SetSenReg(0x01, 0xC3);
		A8L_SetSenReg(0x01, 0xC1);
		sys_IF_ait_delay1ms(100);
		A8L_SetSenReg(0x03, 0x00);
		A8L_SetSenReg(0x01, 0xC0);
	        		
	} else {
		AIT701_SENSOR_RESET_LOW;
		sys_IF_ait_delay1ms(1);

		//Turn off Sensor MCLK
		A8L_SensorClockOutput(A8_OFF);

		A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_OFF);
		A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_OFF);	
		AIT701_SENSOR_LDO_LOW;
		AIT701_MAIN_SEN_EN_LOW;	
		#if defined(__SWITCH_CAMERA_SUPPORT__)
		AIT701_SUB_SEN_EN_LOW;	
		#endif
		gbSensorInited = 0;	
	}	
	
	return;
}
#endif
static u_short	Get_Sensor_ID(u_short*	SensorID)
{
	u_short	fRet,sensor_id;	
	
	if(!gbSensorInited)
		Sensor_Enable(1);
	
	A8L_SetSenReg(0x03,0x00);
	fRet = A8L_GetSenReg(0x04,&sensor_id);
	*SensorID = sensor_id;
	
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

t_sensor_manager sensor_hynix_vga = 
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
#endif

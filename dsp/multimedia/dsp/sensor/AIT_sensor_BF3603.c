#include	"A8_common.h"
#include	"AIT700_EBIbus.h"
#include	"A8_sensor.h"
#include "a8_vif.h"
#include "cam_module.h"
#include "AIT_Interface.h"

#if 1//#ifdef  RGT_SENSOR_BF3603
#define	SENSOR_ID					(0x3603) //(0x7fa2)

#define	SENSOR_I2C_TYPE				(A8_I2C_1A1D)
#define SENSOR_PREVIEW_VH_POLARITY  (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_FULL_VH_POLARITY     (A8_SENSOR_HOR_POS|A8_SENSOR_VER_NEG)
#define SENSOR_PREVIEW_LATCH_COUNT  (A8_PHASE_COUNTER_NUM_2)//(A8_PHASE_COUNTER_NUM_3)
#define SENSOR_FULL_LATCH_COUNT     (A8_PHASE_COUNTER_NUM_2)//(A8_PHASE_COUNTER_NUM_3)
#define SENSOR_PREVIEW_YUV_FORMAT   (A8_SENSOR_CRYCBY)//(A8_SENSOR_CBYCRY) //(A8_SENSOR_YCBYCR) // check BYD
#define SENSOR_FULL_YUV_FORMAT      (A8_SENSOR_CRYCBY)//(A8_SENSOR_CBYCRY) //(A8_SENSOR_YCBYCR) // check BYD
#define SENSOR_LATCH_EDGE           (A8_SENSOR_POS_EDGE)

#define	SENSOR_I2C_ID  			   (0xDC)// (0x42) //(0x6e)//(0xDC) //(0x42) // check BYD
#define	SENSOR_PREVIEW_WIDTH  	    (640)
#define	SENSOR_PREVIEW_HEIGHT  	    (480)
#define	SENSOR_FULL_WIDTH  		    (640)
#define	SENSOR_FULL_HEIGHT  	    (480)

static t_sensor_mode_attr	full_mode;
static t_sensor_mode_attr	preview_mode;

extern	u_char	gbSensorInited;
extern char gAitSensorSelect;

__align(2)const static u_char Sensor_Init[]=
{

    // check BYD
    // BYD provide BF3603
 0x12,0x80,
 0xff,0xff,
 0x09,0x00, //0x01  on other project
 0x11,0x80,
 0x13,0x00,
 0x01,0x15,
 0x02,0x22,
 0x87,0x18,
 0x8c,0x01,
 0x8d,0x7c,
 0x13,0x07,
 0x3a,0x00, //no on other project
          
 0x28,0x03,
 0x2c,0x03,
 0x00,0x20,
 0x0d,0x20,
 0x0e,0x20,
 0x0f,0x20,
 0x05,0x16,
 0x14,0x16,
 0x06,0x19,
 0x08,0x19,
 0x26,0x08,
 0x27,0x08,
 0x1F,0xa0,
 0x22,0xa0,
          
 0x28,0x00,
 0x2c,0x00,
 0x1e,0x20,  
 //0x1e,0x30, //invert
 0x15,0x00, ///40
 0x3A,0x03,
 0x2f,0x00,
 0x16,0x20,
 0x29,0x04,
 0x56,0x40,
          
          
 0x21,0x00, //remove on other project
 0x04,0xbb, //remove on other project
          
          
          
 0x35,0x60,
 0x65,0x58,
 0x66,0x58,
          
 0x82,0x16,
 0x83,0x25,
 0x84,0x1a,
 0x85,0x26,
 0x86,0x30,  //0x34 on other project
          
 0x96,0x26,
 0x97,0x0c,
 0x2b,0x06,
 0x70,0x6f,  //0x2f on other project
 0x72,0x4f, 
 0x73,0x2f,
 0x74,0x27,
 0x75,0x0e,
 0x69,0x00, 
 0x76,0xff,
 0x80,0x55,
 0x89,0x02,
 0x8a,0xf8, 
          
 0x90,0x20,
 0x91,0x1c,
          
 0x1F,0x20,  //no on other project
 0x22,0x20, //no on other project
 0x39,0x80,
 0x3f,0xC0, //0xa0 on other project
          
 0x3b,0x60,
 0x3c,0x10, 
 /*                
 //gamma µÕ‘Î        
 0x40,0x32, 
 0x41,0x2c,
 0x42,0x30,
 0x43,0x1d,
 0x44,0x1a,
 0x45,0x14,
 0x46,0x11,
 0x47,0x0e,
 0x48,0x0d,
 0x49,0x0c,
 0x4b,0x0B,
 0x4c,0x09,
 0x4e,0x09,
 0x4f,0x08,
 0x50,0x07,
 //gamma 1                
 0x40,0x20,
 0x41,0x30,
 0x42,0x28,
 0x43,0x28,
 0x44,0x1d,
 0x45,0x15,
 0x46,0x13,
 0x47,0x10,
 0x48,0x0E, 
 0x49,0x0B,
 0x4b,0x0B,
 0x4c,0x09,
 0x4e,0x07,
 0x4f,0x06,
 0x50,0x05,        
 //gamma 2 «ÂŒ˙       
 0x40,0x25,
 0x41,0x2a,
 0x42,0x28,
 0x43,0x28,
 0x44,0x20,
 0x45,0x1d,
 0x46,0x17,
 0x47,0x15, 
 0x48,0x0f,
 0x49,0x0e,
 0x4b,0x0a,
 0x4c,0x06,
 0x4e,0x05,
 0x4f,0x04,  
 0x50,0x02, 
 */       
 //gamma 3 ¡¡¿ˆ        
 0x40,0x42,
 0x41,0x3b,
 0x42,0x32,
 0x43,0x24,
 0x44,0x1c,
 0x45,0x15,
 0x46,0x11,
 0x47,0x0d,
 0x48,0x0d,
 0x49,0x0B,
 0x4b,0x09,
 0x4c,0x08,
 0x4e,0x08,
 0x4f,0x07,
 0x50,0x06,        
 //color matrix        
 0x51,0x30,
 0x52,0x92,
 0x53,0x02,
 0x54,0x86,
 0x57,0x30,
 0x58,0x8a,
 0x59,0x80,
 0x5a,0x92,
 0x5b,0x32,
 /*         
 // the other one       
 0x51,0x2e,
 0x52,0x8e,
 0x53,0x80,
 0x54,0x88,
 0x57,0x30,
 0x58,0x88,
 0x59,0x80,
 0x5a,0x92,
 0x5b,0x32,
 */        
 0x5c,0x2e,
 0x5d,0x17,
          
 0x6a,0x01,
 0x23,0x66,
 0xa0,0x9f,
 0xa1,0x51,
 0xa2,0x10,
 0xa3,0x26,
 0xa4,0x0b,
 0xa5,0x26,
 0xa6,0x06,
 0xa7,0x98,
 0xa8,0x84, //ox80 on other project
 0xa9,0x3e, //0x42  on other project
 0xaa,0x4b,
 0xab,0x3e,
 0xac,0x42,
 0xad,0x43,
 0xae,0x48,
 0xaf,0x2b,
 0xc5,0x32,
 0xc6,0x34,
 0xc7,0x39,
 0xc8,0x2f,
 0xc9,0x36,
 0xca,0x3f,
 0xcb,0x41,
 0xcc,0x45,
 0xcd,0x48,
 0xce,0x44,
 0xcf,0x4C,
 0xd0,0x4B,
 0xd1,0x55,
        
 0xb0,0xe4,
 0xb1,0xc0,
 0xb2,0xb0,
 0xb3,0x86,
          
 0x24,0x8a,
 0x25,0x7a,
 //banding   
 0x9d,0x69,
 
 0x92,0xF6,
 0x93,0x00,
 0x2a,0x10, 
 0x2b,0xC0,
 
}; 



__align(2)const static u_char Sensor_DSC_Preview[] =
{
NULL
};
__align(2)const static u_char Sensor_Video_Preview[]=
{
#if 1  // fix 15pfs
	0x13,0x02,
	0x8c,0x01,
	0x8d,0x7c,
	0x92,0x8b,
	0x93,0x00,
#else  //vary pfs

#endif

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
	0x3A,0x03,
	0x80,0x45,
	0x77,0x00, 
	0x69,0x00,
	0x67,0x80,
	0x68,0x80,
	0x56,0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_GRAYSCALE[] = 
{	// GrayScale
	0x3A,0x03,
	0x80,0x45,
	0x77,0x00,
	0x69,0x20,
	0x67,0x80,
	0x68,0x80,
	0x56,0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_SEPIA[] = 
{	// Sepia
  0x3A,0x03,					
  0x80,0x45,					
  0x77,0x00,					
  0x69,0x20,					
  0x67,0x66,					
  0x68,0x98,					
  0x56,0x40,					
};
__align(2)const static u_char  SENSOR_EFFECT_SEPIAGREEN[] = 
{	// SepiaGreen
	0x3A,0x03,
	0x80,0x45,
	0x77,0x00,
	0x69,0x20,
	0x67,0x70,
	0x68,0x70,
	0x56,0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_SEPIABLUE[] = 
{	// SepiaBlue : lack, check BYD
	0x3A, 0x1c,
	0x67, 0xA0,
	0x68, 0x40,
	0x56, 0x40,
};

__align(2)const static u_char  SENSOR_EFFECT_COLORINV[] = 
{	// Color Inv : lack, check BYD
	0x3A, 0x2c,
	0x67, 0x80,
	0x68, 0x80,
	0x56, 0x40
};
__align(2)const static u_char  SENSOR_EFFECT_GRAYINV[] = 
{	// GrayInv : lack, check BYD
	0x3A, 0x1c,
	0x67, 0x80,
	0x68, 0x80,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_BLACKBOARD[] = 
{	
  NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_WHITEBOARD[] = 
{
   NULL,
};
__align(2)const static u_char  SENSOR_EFFECT_COPPER_CARVING[] = 
{
   // CopperCarving : lack, check BYD
	0x3A, 0x1c,
	0x67, 0x80,
	0x68, 0xf0,
	0x56, 0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_EMBOSSMENT[] = 
{	// Embossment
	0x3A,0x03,
	0x69,0x00,
	0x80,0xc5,
	0x77,0x80,
	0x67,0x80,
	0x68,0x80,
	0x56,0x40,
};
__align(2)const static u_char  SENSOR_EFFECT_BLUECARVING[] = 
{
// BlueCarving : lack, check BYD
	0x3A, 0x1c,
	0x67, 0xb0,
	0x68, 0x60,
	0x56, 0x40,
};

__align(2)const static u_char  SENSOR_EFFECT_CONTRAST[] = 
{
   // Contrast : lack, check BYD
	0x3A, 0x0c,
	0x67, 0x80,
	0x68, 0x80,
	0x56, 0x80,
};
__align(2)const static u_char  SENSOR_EFFECT_SKETCH[] = 
{	// Sketch
	0x3A,0x03,
	0x69,0x00,
	0x80,0xc5,
	0x77,0xb0,
	0x67,0x80,
	0x68,0x80,
	0x56,0x40,
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


__align(2)const static u_char  SENSOR_NIGHT_MODE_ON[] = 
{ 
	0x85, 0x2a,
	0x86, 0x36,
	0x8e, 0x07,
	0x8f, 0x79,
};
__align(2)const static u_char  SENSOR_NIGHT_MODE_OFF[] = 
{
	0x8e, 0x03,
	0x8f, 0xbc,
};

#if 1
__align(2)const static u_char  SENSOR_DEBAND_50[] = 
{  // : lack, check BYD

 0x80, 0x45,
 0x9d, 0x69,
};
__align(2)const static u_char  SENSOR_DEBAND_60[] = 
{  // : lack, check BYD
 0x80, 0x44,
 0x9e, 0x57,
};
#else
__align(2)const static u_char  SENSOR_DEBAND_50[] = 
{  // : lack, check BYD
	0xf0, 0x01,
	0x80, 0x01,
	0xf0, 0x00,
	0x80, 0x5b,
	0x9d, 0x4c,
};
__align(2)const static u_char  SENSOR_DEBAND_60[] = 
{  // : lack, check BYD
	0xf0, 0x01,
	0x80, 0x00,
	0xf0, 0x00,
	0x80, 0x5b,
	0x9e, 0x3f,
};
#endif


__align(2)const static u_char  SENSOR_ROTATE_NOMAL_BACK[] = 
{ // : lack, check BYD
	0x1e, 0x00,
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_BACK[] = 
{ // : lack, check BYD
	0x1e, 0x20,
};
__align(2)const static u_char  SENSOR_ROTATE_FLIP_BACK[] = 
{ // : lack, check BYD
	0x1e, 0x10,

};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_FLIP_BACK[] = 
{ // : lack, check BYD
	0x1e, 0x30,
};
__align(2)const static u_char  SENSOR_ROTATE_NOMAL_FRONT[] = 
{ // : lack, check BYD
	0x1e, 0x20,
};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_FRONT[] = 
{ // : lack, check BYD
	0x1e, 0x00,
};
__align(2)const static u_char  SENSOR_ROTATE_FLIP_FRONT[] = 
{ // : lack, check BYD
	0x1e, 0x30,

};
__align(2)const static u_char  SENSOR_ROTATE_MIRROR_FLIP_FRONT[] = 
{ // : lack, check BYD
	0x1e, 0x10,
};

static u_short	Sensor_Effect(u_short EffectMode)
{
	//u_short	*tab_addr,tab_len;
	u_short result=0;
	u_short  temp_reg;
        A8L_GetSenReg(0x13, &temp_reg);
//        kal_prompt_trace(MOD_MED,"( temp_reg----set_BF3603_param_effect=%x\n",temp_reg);

	switch(EffectMode)
	{
		case DSC_GRAYSCALE:
                                 A8L_SetSenReg(0x3A,0x03);
                                 A8L_SetSenReg(0x13,temp_reg|0x2);		
                                 A8L_SetSenReg(0x80,0x45);		
                                 A8L_SetSenReg(0x77,0x00);
                                 A8L_SetSenReg(0x69,0x20);		
                                 A8L_SetSenReg(0x67,0x80);		
                                 A8L_SetSenReg(0x68,0x80);
                                 A8L_SetSenReg(0x56,0x40);	
                                  //tab_addr = (u_short*)SENSOR_EFFECT_GRAYSCALE;
                                  //tab_len = sizeof(SENSOR_EFFECT_GRAYSCALE);                              
                                 break;
		case DSC_SEPIA:
                                 A8L_SetSenReg(0x3A,0x03);
                                 A8L_SetSenReg(0x13,temp_reg|0x2);		
                                 A8L_SetSenReg(0x80,0x45);		
                                 A8L_SetSenReg(0x77,0x00);
                                 A8L_SetSenReg(0x69,0x20);		
                                 A8L_SetSenReg(0x67,0x66);		
                                 A8L_SetSenReg(0x68,0x98);
                                 A8L_SetSenReg(0x56,0x40);	
				//tab_addr = (u_short*)SENSOR_EFFECT_SEPIA;
				//tab_len = sizeof(SENSOR_EFFECT_SEPIA);
				break;
		case DSC_SEPIA_GREEN:
                                 A8L_SetSenReg(0x3A,0x03);
                                 A8L_SetSenReg(0x13,temp_reg|0x2);		
                                 A8L_SetSenReg(0x80,0x45);		
                                 A8L_SetSenReg(0x77,0x00);
                                 A8L_SetSenReg(0x69,0x20);		
                                 A8L_SetSenReg(0x67,0x70);		
                                 A8L_SetSenReg(0x68,0x70);
                                 A8L_SetSenReg(0x56,0x40);	
				//tab_addr = (u_short*)SENSOR_EFFECT_SEPIAGREEN;
				//tab_len = sizeof(SENSOR_EFFECT_SEPIAGREEN);
				break;
		case  DSC_SEPIA_BLUE:
                                A8L_SetSenReg(0x3A,0x03);
                                A8L_SetSenReg(0x13,temp_reg|0x2);		
                                A8L_SetSenReg(0x80,0x45);		
                                A8L_SetSenReg(0x77,0x00);
                                A8L_SetSenReg(0x69,0x20);		
                                A8L_SetSenReg(0x67,0xA0);		
                                A8L_SetSenReg(0x68,0x60);
                                 A8L_SetSenReg(0x56,0x40);	
				//tab_addr = (u_short*)SENSOR_EFFECT_SEPIABLUE;
				//tab_len = sizeof(SENSOR_EFFECT_SEPIABLUE);
				break;
		case	 DSC_COLOR_INV:
                               A8L_SetSenReg(0x3A,0x03);
                               A8L_SetSenReg(0x13,temp_reg|0x2);		
                               A8L_SetSenReg(0x80,0x45);		
                               A8L_SetSenReg(0x77,0x00);
                               A8L_SetSenReg(0x69,0x40);		
                               A8L_SetSenReg(0x67,0x80);		
                               A8L_SetSenReg(0x68,0x80);
                               A8L_SetSenReg(0x56,0x40);	
				//tab_addr = (u_short*)SENSOR_EFFECT_COLORINV;
				//tab_len = sizeof(SENSOR_EFFECT_COLORINV);
				break;	
		case DSC_GRAY_INV:
                                A8L_SetSenReg(0x3A,0x03);
                                A8L_SetSenReg(0x13,temp_reg|0x2);		
                                A8L_SetSenReg(0x80,0x45);		
                                A8L_SetSenReg(0x77,0x00);
                                A8L_SetSenReg(0x69,0x60);		
                                A8L_SetSenReg(0x67,0x80);		
                                A8L_SetSenReg(0x68,0x80);
                                A8L_SetSenReg(0x56,0x40);		
				//tab_addr = (u_short*)SENSOR_EFFECT_GRAYINV;
				//tab_len = sizeof(SENSOR_EFFECT_GRAYINV);
				break;
		case DSC_BLACKBOARD:
                                A8L_SetSenReg(0x3A,0x03);
                                A8L_SetSenReg(0x69,0x00);
                                A8L_SetSenReg(0x13,temp_reg&~0x2);
                                A8L_SetSenReg(0x80,0xc5);		
                                A8L_SetSenReg(0x77,0xe0);
                                A8L_SetSenReg(0x67,0x80);		
                                A8L_SetSenReg(0x68,0x80);
                                A8L_SetSenReg(0x56,0x40);
				//tab_addr = (u_short*)SENSOR_EFFECT_BLACKBOARD;
				//tab_len = sizeof(SENSOR_EFFECT_BLACKBOARD);
				break;
		case DSC_WHITEBOARD :	
                                A8L_SetSenReg(0x3A,0x03);
                                A8L_SetSenReg(0x69,0x00);
                                A8L_SetSenReg(0x13,temp_reg&~0x2);
                                A8L_SetSenReg(0x80,0xc5);		
                                A8L_SetSenReg(0x77,0xf0);
                                A8L_SetSenReg(0x67,0x80);		
                                A8L_SetSenReg(0x68,0x80);
                                A8L_SetSenReg(0x56,0x40);
			       //tab_addr = (u_short*)SENSOR_EFFECT_WHITEBOARD;
				//tab_len = sizeof(SENSOR_EFFECT_WHITEBOARD);
				break;	
		case DSC_COPPER_CARVING:
                                A8L_SetSenReg(0x3A,0x03);
                                A8L_SetSenReg(0x69,0x00);
                                A8L_SetSenReg(0x13,temp_reg&~0x2);
                                A8L_SetSenReg(0x80,0xc5);
                                A8L_SetSenReg(0x77,0xc0);
                                A8L_SetSenReg(0x67,0x80);		
                                A8L_SetSenReg(0x68,0x80);
                                A8L_SetSenReg(0x56,0x40);
			        //tab_addr = (u_short*)SENSOR_EFFECT_COPPER_CARVING;
				//tab_len = sizeof(SENSOR_EFFECT_COPPER_CARVING);
				break;
		case DSC_EMBOSSMENT:
                                A8L_SetSenReg(0x3A,0x03);
                                A8L_SetSenReg(0x69,0x00);
                                A8L_SetSenReg(0x13,temp_reg&~0x2);
                                A8L_SetSenReg(0x80,0xc5);	
                                A8L_SetSenReg(0x77,0x80);
                                A8L_SetSenReg(0x67,0x80);		
                                A8L_SetSenReg(0x68,0x80);
                                A8L_SetSenReg(0x56,0x40);
                                //tab_addr = (u_short*)SENSOR_EFFECT_EMBOSSMENT;
				//tab_len = sizeof(SENSOR_EFFECT_EMBOSSMENT);
				break;
		case DSC_BLUE_CARVING:
                                A8L_SetSenReg(0x3A,0x03);
                                A8L_SetSenReg(0x69,0x00);
                                A8L_SetSenReg(0x13,temp_reg&~0x2);
                                A8L_SetSenReg(0x80,0xc5);	
                                A8L_SetSenReg(0x77,0xd0);
                                A8L_SetSenReg(0x67,0x80);		
                                A8L_SetSenReg(0x68,0x80);
                                A8L_SetSenReg(0x56,0x40);		
			        //tab_addr = (u_short*)SENSOR_EFFECT_BLUECARVING;
				//tab_len = sizeof(SENSOR_EFFECT_BLUECARVING);
				break;  
		case DSC_CONTRAST:
                               A8L_SetSenReg(0x3A,0x03);
                               A8L_SetSenReg(0x13,temp_reg|0x2);		
                               A8L_SetSenReg(0x80,0x45);		
                               A8L_SetSenReg(0x77,0x00);
                               A8L_SetSenReg(0x69,0x00);		
                               A8L_SetSenReg(0x67,0x80);		
                               A8L_SetSenReg(0x68,0x80);
                               A8L_SetSenReg(0x56,0x80);
			        //tab_addr = (u_short*)SENSOR_EFFECT_CONTRAST;
				//tab_len = sizeof(SENSOR_EFFECT_CONTRAST);
				break; 
		case DSC_SKETCH:
                                A8L_SetSenReg(0x3A,0x03);
                                A8L_SetSenReg(0x69,0x00);
                                A8L_SetSenReg(0x13,temp_reg&~0x2);
                                A8L_SetSenReg(0x80,0xc5);		
                                A8L_SetSenReg(0x77,0xb0);
                                A8L_SetSenReg(0x67,0x80);		
                                A8L_SetSenReg(0x68,0x80);
                                A8L_SetSenReg(0x56,0x40);
                               // tab_addr = (u_short*)SENSOR_EFFECT_SKETCH;
				//tab_len = sizeof(SENSOR_EFFECT_SKETCH);
				break;		
		default:		
                                  A8L_SetSenReg(0x3A,0x03);
                                  A8L_SetSenReg(0x13,temp_reg|0x2);		
                                  A8L_SetSenReg(0x80,0x45);		
                                  A8L_SetSenReg(0x77,0x00);
                                  A8L_SetSenReg(0x69,0x00);		
                                  A8L_SetSenReg(0x67,0x80);		
                                  A8L_SetSenReg(0x68,0x80);
                                  A8L_SetSenReg(0x56,0x40);			
				//tab_addr = (u_short*)SENSOR_EFFECT_NORMAL;
				//tab_len = sizeof(SENSOR_EFFECT_NORMAL);
          
				break;				
	}
            return result;
   	//return	A8L_SendSensorSettingTable(tab_addr,tab_len);
}

static u_short	Sensor_EV(u_short EvStep)
{
	u_short	*tab_addr,tab_len;
    
	switch(EvStep)
	{
		case	DSC_EV_VALUE_NEG_4:
				tab_addr = (u_short*)SENSOR_EV_0;
				tab_len = sizeof(SENSOR_EV_0);		
				break;	
		case	DSC_EV_VALUE_NEG_3:
				tab_addr = (u_short*)SENSOR_EV_1;
				tab_len = sizeof(SENSOR_EV_1);		
				break;	
		case	DSC_EV_VALUE_NEG_2:
				tab_addr = (u_short*)SENSOR_EV_2;
				tab_len = sizeof(SENSOR_EV_2);		
				break;	
		case	DSC_EV_VALUE_NEG_1:
				tab_addr = (u_short*)SENSOR_EV_3;
				tab_len = sizeof(SENSOR_EV_3);		
				break;	
		case	DSC_EV_VALUE_ZERO:
				tab_addr = (u_short*)SENSOR_EV_4;
				tab_len = sizeof(SENSOR_EV_4);		
				break;	
		case	DSC_EV_VALUE_POS_1:
				tab_addr = (u_short*)SENSOR_EV_5;
				tab_len = sizeof(SENSOR_EV_5);		
				break;	
		case	DSC_EV_VALUE_POS_2:
				tab_addr = (u_short*)SENSOR_EV_6;
				tab_len = sizeof(SENSOR_EV_6);		
				break;	
		case	DSC_EV_VALUE_POS_3:
				tab_addr = (u_short*)SENSOR_EV_7;
				tab_len = sizeof(SENSOR_EV_7);		
				break;	
		case	DSC_EV_VALUE_POS_4:
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
	//u_short	*tab_addr,tab_len=0;
    u_short fRet = 0;
    u_short	tmp_val;
    
    fRet = A8L_GetSenReg(0x013,&tmp_val);
    
	switch(WbMode)
	{
		case	DSC_WB_AUTO:	//WB_AUTO
				A8L_SetSenReg(0x01,0x15);
				A8L_SetSenReg(0x02,0x22);
				//A8L_SetSenReg(0x13,0x07);	//enable awb
				A8L_SetSenReg(0x13,(tmp_val|0x2)); 
	
				break;	
		case	DSC_WB_CLOUD:	//WB_CLOUD
				//A8L_SetSenReg(0x13,0x05);	//enable awb
				A8L_SetSenReg(0x13,(tmp_val&~0x2));
				A8L_SetSenReg(0x01,0x16);
				A8L_SetSenReg(0x02,0x24);
	
				break;	
		case DSC_WB_DAYLIGHT:	//WB_DAYLIGHT
				//A8L_SetSenReg(0x13,0x05);	//enable awb
				A8L_SetSenReg(0x13,(tmp_val&~0x2));			
				A8L_SetSenReg(0x01,0x19);
				A8L_SetSenReg(0x02,0x27);		
				break;	
		case	DSC_WB_INCANDESCENCE:	//WB_INCANDESCENCE
				//A8L_SetSenReg(0x13,0x05);	//enable awb
				A8L_SetSenReg(0x13,(tmp_val&~0x2));	
				A8L_SetSenReg(0x01,0x2D);
				A8L_SetSenReg(0x02,0x0c);	
				break;	
		case	DSC_WB_FLUORESCENT:	//WB_FLUORESCENT
				//A8L_SetSenReg(0x13,0x05);	//enable awb
				A8L_SetSenReg(0x13,(tmp_val&~0x2));  // Disable AWB
				A8L_SetSenReg(0x01,0x22);
				A8L_SetSenReg(0x02,0x1a);
				break;	
		case	DSC_WB_TUNGSTEN:	//WB_TUNGSTEN
				//A8L_SetSenReg(0x13,0x05);	//enable awb
				A8L_SetSenReg(0x13,(tmp_val&~0x2));	
				A8L_SetSenReg(0x01,0x28);
				A8L_SetSenReg(0x02,0x0f);	
				break;	
		case	DSC_WB_MANUAL:	//	
				break;	;																																																			
		default:
				A8L_SetSenReg(0x01,0x15);
				A8L_SetSenReg(0x02,0x22);
				A8L_SetSenReg(0x13,0x07);	//enable awb	
				break;	
	}	
	return	fRet;
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
    {
	switch(Rotate)
	{
		case	A8_SENSOR_ROTATE_MIRROR:
				tab_addr = (u_short*)SENSOR_ROTATE_MIRROR_FRONT;
				tab_len = sizeof(SENSOR_ROTATE_MIRROR_FRONT);		
				break;	
		case	A8_SENSOR_ROTATE_FILP:
				tab_addr = (u_short*)SENSOR_ROTATE_FLIP_FRONT;
				tab_len = sizeof(SENSOR_ROTATE_FLIP_FRONT);		
				break;	
		case	A8_SENSOR_ROTATE_MIRROR_FLIP:
				tab_addr = (u_short*)SENSOR_ROTATE_MIRROR_FLIP_FRONT;
				tab_len = sizeof(SENSOR_ROTATE_MIRROR_FLIP_FRONT);		
				break;																																																											
		default:
				tab_addr = (u_short*)SENSOR_ROTATE_NOMAL_FRONT;
				tab_len = sizeof(SENSOR_ROTATE_NOMAL_FRONT);
				break;	
	}				
    }
		
   	return	A8L_SendSensorSettingTable(tab_addr,tab_len);		
}

static u_short 	Sensor_FrameRate(u_short FrameRate)
{}



//need to check and change  !!!!!!!
u_short g_sensor_side=1;
static void	Sensor_Enable(u_short enable)
{
extern unsigned char init_ATV(void);
extern void power_off_ATV(void);
extern void PowerOffFM(void);
extern	u_char	g_ATV_Flag;


	if(enable) 
	{	
		A8L_VIF_Init(&preview_mode);	//
		
			AIT_Message_P0("===BF3603===BF3603 turn on \n\r");
			//ATV off
			PowerOffFM();

			//sensor power LDO
			A8L_SetSensorPowerPin(1);
			sys_IF_ait_delay1ms(5);
			
			//sensor PWDN
			if(gAitSensorSelect==AIT_MASTER_SENSOR_SRC)//master
			{
			A8L_SetSensorEnablePin(0);
				//AIT701_SUBSNESOR_DISABLE
			}
			else if(gAitSensorSelect==AIT_SLAVE_SENSOR_SRC)//sub-sensor
			{
				A8L_SetSensorEnablePin(1);
				//AIT701_SUBSNESOR_ENABLE
			}

			//Reset
			A8L_SetSensorResetPin(1); 
			sys_IF_ait_delay1ms(5);
			A8L_SetSensorResetPin(0);
			sys_IF_ait_delay1ms(5); 
			A8L_SetSensorResetPin(1); 
			sys_IF_ait_delay1ms(5);


		//Initial I2C I/F
			A8L_SetSensorIF(A8_SENSOR_SCK_PIN, A8_ON);
			A8L_SetSensorIF(A8_SENSOR_SDA_PIN, A8_ON);
			sys_IF_ait_delay1ms(1);
	        		
	} else {
		AIT_Message_P0("===BF3603===BF3603 turn off \n\r");	
		//ATV off
		PowerOffFM();

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
		Sensor_Enable(1);
	for(i=0;i<3;i++)
	{
		A8L_GetSenReg(0xFC,&fRet);
		*SensorID = fRet << 8;
		A8L_GetSenReg(0xFD,&fRet);
		*SensorID += fRet;
		if (SENSOR_ID == *SensorID )
		{
			break;
		}
	}
	if(!gbSensorInited)
		Sensor_Enable(0);
	
	AIT_Message_P1(" BF3603 Get_Sensor_ID:0x%x\r\n",*SensorID);
	
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
t_sensor_manager sensor_bf3603 = 
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

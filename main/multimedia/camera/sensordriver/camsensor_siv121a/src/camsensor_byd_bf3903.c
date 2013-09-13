/*============================================================================

                 Camera Interface Device Driver Source File for 
                    BF3903 VGA CMOS sensor
                    added by yangty LONGSHANG

   DESCRIPTION
     This file contains the definitions needed for the camera interface
     device driver.

   Copyright (c) 2006 by LCT Driver Team.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msm6550/drivers/camsensor/camsensor_ SIV121A_ycbcr.c#3 $ $DateTime: 2006/08/11 14:02:49 $ $Author: klee $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
  09/30/06  lee    Initial release.
 
============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "camsensor.h"
#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_BF3903)
#include "camerai.h"

#include "camsensor_byd_bf3903.h"
#include "clk.h"


/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

#define CAMSENSOR_BF3903_I2C_SLAVE_ID              0xdc ///SIV121A //62

#define CAMSENSOR_BF3903_SENSOR_ID              0x38 ///SIV121A
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_K212_12832
#define CAMSENSOR_BF3903_RESET_PIN         GPIO_OUTPUT_10
#endif
#endif
/* 
 * Maximum number of trials before aborting a register write operation
 *
 */
#define MAX_REGISTER_WRITE_TRIALS   3
#define MAX_REGISTER_READ_TRIALS    3
/*begin\bug6192\yangtingyu\20090226\camera驱动更新，实现亮度调节*/
#define BF3903_MAX_BRIGHTNESS  9
#define BF3903_MIN_BRIGHTNESS  0
/*end\bug6192\yangtingyu\20090226\camera驱动更新，实现亮度调节*/

#define BF3903_MAX_CONTRAST   4
#define BF3903_MIN_CONTRAST   0

/* From the logic analyzer measurements */
#define OV_BF3903_YCBCR_FULL_SIZE_WIDTH           640//640
#define OV_BF3903_YCBCR_FULL_SIZE_HEIGHT          480

#define OV_BF3903_YCBCR_QTR_SIZE_WIDTH   		640//640   
#define OV_BF3903_YCBCR_QTR_SIZE_HEIGHT  		480//240//   

/* Strobe Flash Epoch Interrupt time before the end of line count */
#define OV_SIV121A_YCBCR_EPOCH_LINES_DELAY       1

/*===========================================================================
                          MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                        STATIC VARIABLES 
============================================================================*/
// modified Longcheer3G_liudan_20070403.........................................
const char camsensor_bf3903_ycbcr_sensor_name[]  = "BF3903 VGA";

// modified end.................................................................
/*============================================================================
                          INTERNAL API PROTOTYPES
============================================================================*/
static boolean camsensor_bf3903_ycbcr_i2c_write_byte(uint8 reg, uint8 data);
static void camsensor_bf3903_ycbcr_register(camsensor_function_table_type *camsensor_function_table_ptr);
static boolean camsensor_bf3903_ycbcr_i2c_read_byte(uint8 reg, uint8 *data); 
static boolean camsensor_bf3903_ycbcr_start(camsensor_static_params_type *camsensor_params );
static boolean camsensor_bf3903_ycbcr_video_config( camsensor_static_params_type *camsensor_params /* Other config params */ );
static void camsensor_bf3903_ycbcr_power_up(void);
static boolean camsensor_bf3903_ycbcr_snapshot_config( 	camsensor_static_params_type *camsensor_params /* Other config params */);
static camera_ret_code_type camsensor_bf3903_set_effect(int8 effect);
static camera_ret_code_type camsensor_bf3903_set_wb(int8 wb);
static camera_ret_code_type camsensor_bf3903_set_contrast(int8 contrast);
static camera_ret_code_type camsensor_bf3903_set_antibanding(int8 antibanding);
static camera_ret_code_type camsensor_bf3903_set_brightness(int8 value);
static void    camsensor_bf3903_0m3_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
/*============================================================================
                          EXTERNAL API DEFINITIONS
============================================================================*/
static  boolean camsensor_bf3903_sensor_init(void)
{
	MSG_FATAL("-----camsensor_bf3903_sensor_init-----enter",0,0,0);
	
    camsensor_bf3903_ycbcr_i2c_write_byte(0x11,0x80);//0X90
	camsensor_bf3903_ycbcr_i2c_write_byte(0x09,0x01);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x15,0x02);//0x00);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x3a,0x02);//0x02);
	 
	//initial awb and ae setting 
	camsensor_bf3903_ycbcr_i2c_write_byte(0x13,0x00);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x01,0x11);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x02,0x22);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x8c,0x02);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x8d,0x64);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x9d,0x2f); //manual global gain
	camsensor_bf3903_ycbcr_i2c_write_byte(0x13,0x07);
	 
	// analog setting
	camsensor_bf3903_ycbcr_i2c_write_byte(0x20,0x89);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x2f,0xc5);  
	camsensor_bf3903_ycbcr_i2c_write_byte(0x06,0x68);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x08,0x10);
	 
	//lens shading   
	camsensor_bf3903_ycbcr_i2c_write_byte(0x35,0x46);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x65,0x46);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x66,0x46);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xbe,0x44);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xbd,0xf4);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xbc,0x0d);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x9c,0x44);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x9b,0xf4);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x36,0x45);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x37,0xf4);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x38,0x44);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x6e,0x10);
	
	//denoise
	camsensor_bf3903_ycbcr_i2c_write_byte(0x72,0x2f);// 0x72[7:4] denoise:the bigger,the smaller noise 
	camsensor_bf3903_ycbcr_i2c_write_byte(0x73,0x2f);// 0x73[7:4] denoise:the bigger,the smaller noise 
	camsensor_bf3903_ycbcr_i2c_write_byte(0x74,0x27);// 0x74[3:0] denoise:the smaller,the smaller noise 
	 
	//edge enhancement
	camsensor_bf3903_ycbcr_i2c_write_byte(0x75,0x24);//0x75[6:4]:bright edge enhancement.0x75[2:0]:dark edge enhancement.
	 
	//denoise in low light
	camsensor_bf3903_ycbcr_i2c_write_byte(0x79,0x2d);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x7a,0x2d);//oí0x86?D??ó|μ?êy×?gain ò??ù
	camsensor_bf3903_ycbcr_i2c_write_byte(0x7e,0x1a);
	 
	//denoise for outdoor
	camsensor_bf3903_ycbcr_i2c_write_byte(0x7c,0x88);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x7d,0xba);
	 
	//color fringe correction
	camsensor_bf3903_ycbcr_i2c_write_byte(0x5b,0xc2);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x76,0x90);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x7b,0x55);
	 
	//ae
	camsensor_bf3903_ycbcr_i2c_write_byte(0x25,0x88);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x80,0x86);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x81,0xa0);//ae speed
	camsensor_bf3903_ycbcr_i2c_write_byte(0x82,0x2d);//minimum global gain
	camsensor_bf3903_ycbcr_i2c_write_byte(0x83,0x5a);   
	camsensor_bf3903_ycbcr_i2c_write_byte(0x84,0x30);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x85,0x40);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x86,0x50);//maximum global gain
	camsensor_bf3903_ycbcr_i2c_write_byte(0x89,0xa3);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x8a,0x99);//50HZ
	camsensor_bf3903_ycbcr_i2c_write_byte(0x8b,0x7f);//60HZ
	camsensor_bf3903_ycbcr_i2c_write_byte(0x8e,0x2c);   
	camsensor_bf3903_ycbcr_i2c_write_byte(0x8f,0x82);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x97,0x78);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x98,0x12);//ae window
	camsensor_bf3903_ycbcr_i2c_write_byte(0x24,0xb0);//ae target
		 
	//used to modify Y_aver
	camsensor_bf3903_ycbcr_i2c_write_byte(0x95,0x80);//the smaller value  ,the bigger brightness in gray region. í???0x80ò?é??￡
	camsensor_bf3903_ycbcr_i2c_write_byte(0x9a,0xa0);
	 
	//outdoor detection
	camsensor_bf3903_ycbcr_i2c_write_byte(0x9e,0x50);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x9f,0x50);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xd2,0x78);
	 
	//contrast
	camsensor_bf3903_ycbcr_i2c_write_byte(0x56,0x40);
	 
	//auto offset in high light scence
	camsensor_bf3903_ycbcr_i2c_write_byte(0x90,0x20);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x91,0x1c);
	 
	//auto offset in low light scence
	camsensor_bf3903_ycbcr_i2c_write_byte(0x3b,0x60);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x3c,0x10);//the bigger  ,the brighter in dark scence.
	 
	//gamma offset
	camsensor_bf3903_ycbcr_i2c_write_byte(0x39,0xa0);//??áDoffset,ó?0x3fμ??μòaD′ò??ù
	camsensor_bf3903_ycbcr_i2c_write_byte(0x3f,0xa0);// ??áDoffset￡?ó?0x39μ??μòaD′3éò??ù?￡
	 
	//gamma1 1y??1y?éo?
	camsensor_bf3903_ycbcr_i2c_write_byte(0x40,0x80);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x41,0x80);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x42,0x68);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x43,0x53);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x44,0x48);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x45,0x41);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x46,0x3b);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x47,0x36);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x48,0x32);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x49,0x2f);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4b,0x2c);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4c,0x2a);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4e,0x25);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4f,0x22);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x50,0x20);
	 
	/*//gamma2 ??é?D?
	camsensor_bf3903_ycbcr_i2c_write_byte(0x40,0x68);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x41,0x60);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x42,0x54);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x43,0x46);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x44,0x3e);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x45,0x39);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x46,0x35);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x47,0x31);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x48,0x2e);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x49,0x2b);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4b,0x29);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4c,0x27);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4e,0x24);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4f,0x22);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x50,0x20);
	 
	//gamma3 ??éùD?
	camsensor_bf3903_ycbcr_i2c_write_byte(0x40,0x64);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x41,0x5c);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x42,0x50);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x43,0x42);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x44,0x3a);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x45,0x35);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x46,0x31);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x47,0x2d);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x48,0x2a);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x49,0x28);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4b,0x26);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4c,0x25);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4e,0x23);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4f,0x21);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x50,0x20);
	 
	//gamma4 ???úááà?
	camsensor_bf3903_ycbcr_i2c_write_byte(0x40,0x78);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x41,0x70);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x42,0x68);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x43,0x55);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x44,0x4A);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x45,0x43);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x46,0x3C);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x47,0x37);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x48,0x33);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x49,0x2F);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4b,0x2C);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4c,0x29);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4e,0x25);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x4f,0x22);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x50,0x20);*/
	 
	//color coefficient for outdoor scene     
	camsensor_bf3903_ycbcr_i2c_write_byte(0x5a,0x16);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x51,0x08);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x52,0x1a);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x53,0xb4);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x54,0x8d);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x57,0xab);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x58,0x40);
	 
	//color coefficient 1 for indoor scene 
	camsensor_bf3903_ycbcr_i2c_write_byte(0x5a,0x96); 
	camsensor_bf3903_ycbcr_i2c_write_byte(0x51,0x05);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x52,0x05);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x53,0x30);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x54,0x18);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x57,0x0f);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x58,0x04);
	 
	/*
	//·?é?o?
	camsensor_bf3903_ycbcr_i2c_write_byte(0x5a,0x96);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x51,0x03);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x52,0x0d);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x53,0x77);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x54,0x66);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x57,0x8b);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x58,0x37);
	 
	//é?2ê?Tà?
	camsensor_bf3903_ycbcr_i2c_write_byte(0x5a,0x96);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x51,0x1f);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x52,0x13);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x53,0xab);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x54,0x92);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x57,0xbb);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x58,0x3b);
	*/
	 
	//color saturation
	camsensor_bf3903_ycbcr_i2c_write_byte(0x5c,0x28);//the smaller  ,the smaller color noise in low light scene; 
	camsensor_bf3903_ycbcr_i2c_write_byte(0xb0,0xe0);//when more than 0x80  ,the smaller  ,the smaller color noise in low light scene.              
	camsensor_bf3903_ycbcr_i2c_write_byte(0xb1,0xd0);//blue coefficience for saturation 
	camsensor_bf3903_ycbcr_i2c_write_byte(0xb2,0xc8);//red coefficience for saturation
	camsensor_bf3903_ycbcr_i2c_write_byte(0xb3,0x7c);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xb4,0x60);//?òé???óò?μ2êé???éù?￡0xb4=0x20,??éù×?D??￡
	 
	//awb
	camsensor_bf3903_ycbcr_i2c_write_byte(0x6a,0x81);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x23,0x66);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa0,0xd0);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa1,0x31);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa2,0x0b);//the low limit of blue gain 
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa3,0x26);//the upper limit of blue gain
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa4,0x09);//the low limit of red gain
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa5,0x2c);//the upper limit of red gain
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa6,0x06);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa7,0x97);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa8,0x16);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xa9,0x50);   
	camsensor_bf3903_ycbcr_i2c_write_byte(0xaa,0x50);  
	camsensor_bf3903_ycbcr_i2c_write_byte(0xab,0x1a);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xac,0x3c);  
	camsensor_bf3903_ycbcr_i2c_write_byte(0xad,0xf0);                            
	camsensor_bf3903_ycbcr_i2c_write_byte(0xae,0x57);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xc5,0xaa);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xc6,0x77);//pure color detection 
	camsensor_bf3903_ycbcr_i2c_write_byte(0xd0,0x00);
	/*****************************************************
	0xd1[7:4]: ?μ′óóú8￡??μ??′ó￡?à?é???ò???′ó￡??μD?óú8￡??μ??′ó￡?à?é???ò???D?￡?
	0xd1[3:0]: ?μ′óóú8￡??μ??′ó￡?oìé???ò???′ó￡??μD?óú8￡??μ??′ó￡?oìé???ò???D?￡?
	*******************************************************/	
	camsensor_bf3903_ycbcr_i2c_write_byte(0xd1,0xaa);
	
	
	camsensor_bf3903_ycbcr_i2c_write_byte(0xc8,0x0d);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xc9,0x0f);
	camsensor_bf3903_ycbcr_i2c_write_byte(0xd3,0x09);             
	camsensor_bf3903_ycbcr_i2c_write_byte(0xd4,0x24);
	 /*
	camsensor_bf3903_ycbcr_i2c_write_byte(0x69,0x20); //out only Y
	camsensor_bf3903_ycbcr_i2c_write_byte(0x67,0x80); //out only Y
	camsensor_bf3903_ycbcr_i2c_write_byte(0x68,0x80); //out only Y
	*/
	//black sun  correction  
	camsensor_bf3903_ycbcr_i2c_write_byte(0x61,0xc8);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x74,0x27);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x78,0xc7);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x16,0xa1);

	camsensor_bf3903_ycbcr_i2c_write_byte(0x6f,0x5f);
	camsensor_bf3903_ycbcr_i2c_write_byte(0x69,0x1E); 
	camsensor_bf3903_ycbcr_i2c_write_byte(0x89,0x8D);
	//switch direction
	camsensor_bf3903_ycbcr_i2c_write_byte(0x1e,0x00);//00:normal  10:IMAGE_V_MIRROR   20:IMAGE_H_MIRROR  30:IMAGE_HV_MIRROR
	

	return  TRUE;
} /* camsensor_bf3903_sensor_init */


uint8 camsensor_bf3903_ycbcr_active(void)
{
	return 0;
}

uint8 camsensor_bf3903_ycbcr_unactive(void)
{
	return 1;
}

/*===========================================================================

FUNCTION      camsensor_bf3903_init

DESCRIPTION
              Initialize the camera sensor;

DEPENDENCIES
  None

RETURN VALUE
  if successful
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
static void bf3903_0p1mp_H_V_Switch(uint8 direction)
{
    switch(direction) 
    {
        case 1:  // normal
            //camsensor_bf3903_ycbcr_i2c_write_byte(0x17, 0x14);    
            break;
        case 2:  // IMAGE_H_MIRROR
            //camsensor_bf3903_ycbcr_i2c_write_byte(0x17, 0x15);    
            break;        
        case 3:  // IMAGE_V_MIRROR
            // camsensor_bf3903_ycbcr_i2c_write_byte(0x17, 0x16);    
            break;  
        case 4:  // IMAGE_HV_MIRROR
            //camsensor_bf3903_ycbcr_i2c_write_byte(0x17, 0x17);    
            break;  
        default:
            break;
    } 
}

void bf3903_0p1mp_write_more_registers(uint8 direction)
{
	//  TODO: FAE Modify the Init Regs here!!!
	/*Customer can adjust GAMMA, MIRROR & UPSIDEDOWN here!*/
	//bf3903_0p1mp_gamma_select(2);
	if(direction)
  	bf3903_0p1mp_H_V_Switch(1);
    else
    bf3903_0p1mp_H_V_Switch(2);
}

boolean camsensor_byd_bf3903_init(camsensor_function_table_type *camsensor_function_table_ptr, camctrl_tbl_type *camctrl_tbl_ptr)
{
	uint8 sensor_id = 0;
	uint8 sensor_id_low = 0;
	MSG_FATAL("camsensor_bf3903_init()\n",0,0,0);///yty 


	camsensor_camclk_po_hz =24000000;
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_K212_12832

	CAMERA_CONFIG_GPIO(CAMSENSOR_BF3903_RESET_PIN);
    
    gpio_out(CAMSENSOR_BF3903_RESET_PIN,1);
    clk_busy_wait(2*1000);
    gpio_out(CAMSENSOR_BF3903_RESET_PIN,0);
    clk_busy_wait(50*1000);
    gpio_out(CAMSENSOR_BF3903_RESET_PIN,1);
    clk_busy_wait(2*1000);
#endif
#endif
	camsensor_preview_resolution  = CAMSENSOR_FULL_SIZE;
	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;


	/* 1. Initiate I2C:  */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = CAMSENSOR_BF3903_I2C_SLAVE_ID;
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 

	//camsensor_bf3903_ycbcr_i2c_write_byte(0xfc, 0x16);

	clk_busy_wait(50*1000);
	camsensor_bf3903_ycbcr_i2c_read_byte(0xfc,&sensor_id);
	camsensor_bf3903_ycbcr_i2c_read_byte(0xfd,&sensor_id_low);
	MSG_FATAL("Sensor ID = 0x%x",sensor_id,0,0);
	MSG_FATAL("Sensor ID LOW = 0x%x",sensor_id_low,0,0);
	if ( sensor_id != CAMSENSOR_BF3903_SENSOR_ID ) 
	{
		return FALSE;
	}
	/* Initialize the sensor_function_table */
	camsensor_bf3903_ycbcr_register (camsensor_function_table_ptr);

	camsensor_bf3903_0m3_setup_camctrl_tbl(camctrl_tbl_ptr);
	return TRUE;
} /* camsensor_bf3903_init */


/*===========================================================================

FUNCTION      camsensor_bf3903_ycbcr_start

DESCRIPTION
              Initialize the camsensor parameters.

DEPENDENCIES
  None

RETURN VALUE
  if successful
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean camsensor_bf3903_ycbcr_start( camsensor_static_params_type *camsensor_params)
{

	MSG_FATAL("camsensor_bf3903_ycbcr_start begin ", 0,0,0);///yty 

	/* Initialize CAMIF operation mode */
	camsensor_params->camif_config.SyncMode         = CAMIF_APS;
	camsensor_params->camif_config.HSyncEdge        = CAMIF_High;
	camsensor_params->camif_config.VSyncEdge        = CAMIF_High;
	camsensor_params->camif_efs_config.EFS_EOL      = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOL      = 0x0000;
	camsensor_params->camif_efs_config.EFS_EOF      = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOF      = 0x0000;

	/* ------------------  Sensor-specific Config -------------- */
	/* Make/model of sensor */
	camsensor_params->sensor_model = CAMSENSOR_ID_BF3903;

	/* CCD or CMOS */
	camsensor_params->sensor_type = CAMSENSOR_CMOS;

	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;
	/* BAYER or YCbCr */
	camsensor_params->output_format = CAMSENSOR_YCBCR;

	/* A pointer to the sensor name for EXIF tags                */
	camsensor_params->sensor_name = camsensor_bf3903_ycbcr_sensor_name;

	/* What is the maximum FPS that can be supported by this sensor
	 in video mode */
	camsensor_params->max_video_fps = 15 * Q8;

	/* Application assigned FPS in video mode */
	camsensor_params->video_fps = 15 * Q8;

	/* Snapshot mode operation */
	camsensor_params->max_preview_fps = 15 * Q8;
	camsensor_params->nightshot_fps = 15 * Q8;

	/* May be assigned with max_preview_fps or nightshot_fps. */
	camsensor_params->preview_fps = 15 * Q8;

	/* PCLK Invert */
	// camsensor_params->pclk_invert = TRUE;

	/* VFE's perception of Sensor output capability */

	/* Snapshot dimensions */
	camsensor_params->full_size_width  = OV_BF3903_YCBCR_FULL_SIZE_WIDTH;
	camsensor_params->full_size_height = OV_BF3903_YCBCR_FULL_SIZE_HEIGHT;

	/* Preview dimensions */
	camsensor_params->qtr_size_width = OV_BF3903_YCBCR_QTR_SIZE_WIDTH;
	camsensor_params->qtr_size_height = OV_BF3903_YCBCR_QTR_SIZE_HEIGHT;

	switch (camsensor_preview_resolution)
	{
		case CAMSENSOR_QTR_SIZE:
			camsensor_params->preview_dx_decimation = camsensor_params->full_size_width * Q12 / camsensor_params->qtr_size_width;
			camsensor_params->preview_dy_decimation = camsensor_params->full_size_height * Q12 / camsensor_params->qtr_size_height;

			/* Set the current dimensions */
			camsensor_params->camsensor_width  = camsensor_params->qtr_size_width;
			camsensor_params->camsensor_height = camsensor_params->qtr_size_height;
			break;

		case CAMSENSOR_FULL_SIZE:
			camsensor_params->preview_dx_decimation = Q12;
			camsensor_params->preview_dy_decimation = Q12;

			/* Set the current dimensions */
			camsensor_params->camsensor_width  = camsensor_params->full_size_width;
			camsensor_params->camsensor_height = camsensor_params->full_size_height;
			break;

		default:
			camsensor_params->preview_dx_decimation = Q12;
			camsensor_params->preview_dy_decimation = Q12;
			break;
	}

	/* This tells camera service the minimum decimation that is supported
	* by the sensor. 
	* Ex: if preview is in quarter size mode, then there is a 
	* sensor decimation of 2, so the minimum is 2 
	*/
	camsensor_params->preview_dx_decimation = (camsensor_params->preview_dx_decimation < Q12) ? Q12 : camsensor_params->preview_dx_decimation;
	camsensor_params->preview_dy_decimation = (camsensor_params->preview_dy_decimation < Q12) ? Q12 : camsensor_params->preview_dy_decimation;

	/* ------------  Auto Exposure Control Config -------------- */
	camsensor_params->aec_enable = FALSE;

	/* ------------  Auto White Balance Config ----------------- */
	/* AWB -Auto White Balance Parameters */
	camsensor_params->awb_enable = FALSE;

	/* ------------  Auto Focus Config ------------------------- */
	/* AF -Auto Focus Parameters */
	camsensor_params->af_enable = FALSE; //TRUE;

	camsensor_params->num_possible_frame_rates = 1;
	/* Define these frame rates */
	/* By convention, the highest frame rate will be first in the
	array (zeroth index) and the lowest last (in order). */
	camsensor_params->frame_rate_array[0].fps = (uint16) (15*256.0); /* Q8 */
	camsensor_params->frame_rate_array[0].use_in_auto_frame_rate = TRUE;

	camsensor_params->frame_rate_array[1].fps = (uint16) (15*256.0); /* Q8 */
	camsensor_params->frame_rate_array[1].use_in_auto_frame_rate = FALSE;

	camsensor_params->pclk_invert = camsensor_info.pclk_invert;
	/* ------------  Default Misc Parmas Config ---------------- */
	/* Does the sensor need/use a flash  */
#if defined FEATURE_WHITE_LED_FLASH || defined FEATURE_STROBE_FLASH
	camsensor_params->support_auto_flash = FALSE;//TRUE;   ///lee test 1004
#else
	camsensor_params->support_auto_flash = FALSE;
#endif

	return TRUE;
} /* camsensor_SIV121A_ycbcr_start */


/*===========================================================================

FUNCTION      camsensor_bf3903_ycbcr_snapshot_config

DESCRIPTION
              Configure the camera sensor and the VFE_CAMIF for
              snapshot mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

static boolean camsensor_bf3903_ycbcr_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	/* Sensor output data format */
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;

	switch (camsensor_snapshot_resolution)
	{
		case CAMSENSOR_QTR_SIZE:
			/* Set the current dimensions */
			camsensor_params->camsensor_width = camsensor_params->qtr_size_width;
			camsensor_params->camsensor_height = camsensor_params->qtr_size_height;
			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = camsensor_params->qtr_size_width*2;
			camsensor_params->camif_frame_config.linesPerFrame = camsensor_params->qtr_size_height;
			break;

		case CAMSENSOR_FULL_SIZE:
			/* Set the current dimensions */
			camsensor_params->camsensor_width = camsensor_params->full_size_width;
			camsensor_params->camsensor_height = camsensor_params->full_size_height;
			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = camsensor_params->full_size_width*2;
			camsensor_params->camif_frame_config.linesPerFrame = camsensor_params->full_size_height;
			break;

		default:
			return FALSE;
	} /* camsensor_preview_resolution */

	/* CAMIF window */
	camsensor_params->camif_window_width_config.firstPixel = 0;
	camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel + camsensor_params->camsensor_width*2 - 1;
	camsensor_params->camif_window_height_config.firstLine = 0;
	camsensor_params->camif_window_height_config.lastLine = camsensor_params->camif_window_height_config.firstLine + camsensor_params->camsensor_height - 1;

	clk_busy_wait(100*1000);
	camsensor_current_resolution = camsensor_snapshot_resolution;
	return TRUE;
} /* camsensor_bf3903_ycbcr_snapshot_config */

/*===========================================================================

FUNCTION      camsensor_bf3903_ycbcr_raw_snapshot_config

DESCRIPTION
              Configure the camera sensor and the VFE_CAMIF for
              raw snapshot mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

static boolean camsensor_bf3903_ycbcr_raw_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	return TRUE;
} /* camsensor_bf3903_ycbcr_raw_snapshot_config */


/*===========================================================================

FUNCTION      camsensor_bf3903_ycbcr_video_config

DESCRIPTION
              Configure the camera sensor and the camera interface
              for Preview mode. Implements all 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

static boolean camsensor_bf3903_ycbcr_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
)
{
	camsensor_bf3903_sensor_init();
	/* Sensor output data format */
	camsensor_params->discardFirstFrame = TRUE;
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;

	switch (camsensor_preview_resolution)
	{
		case CAMSENSOR_QTR_SIZE:
			/* Set the current dimensions */
			camsensor_params->camsensor_width = camsensor_params->qtr_size_width;
			camsensor_params->camsensor_height = camsensor_params->qtr_size_height;
			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = camsensor_params->qtr_size_width*2;
			camsensor_params->camif_frame_config.linesPerFrame = camsensor_params->qtr_size_height;
			break;

		case CAMSENSOR_FULL_SIZE:
			/* Set the current dimensions */
			camsensor_params->camsensor_width = camsensor_params->full_size_width;
			camsensor_params->camsensor_height = camsensor_params->full_size_height;
			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = camsensor_params->full_size_width*2;
			camsensor_params->camif_frame_config.linesPerFrame = camsensor_params->full_size_height;
			break;

		default:
			return FALSE;
	} /* camsensor_preview_resolution */

	/* CAMIF window */
	camsensor_params->camif_window_width_config.firstPixel = 0;
	camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel + camsensor_params->camsensor_width*2 - 1;
	camsensor_params->camif_window_height_config.firstLine = 0;
	camsensor_params->camif_window_height_config.lastLine = camsensor_params->camif_window_height_config.firstLine + camsensor_params->camsensor_height - 1;

	camsensor_current_resolution = camsensor_preview_resolution;

	return TRUE;
} /* camsensor_bf3903_ycbcr_video_config */

/*===========================================================================

FUNCTION      camsensor_bf3903_ycbcr_power_up

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_bf3903_ycbcr_power_up(void)
{
}


/*===========================================================================

FUNCTION      camsensor_bf3903_ycbcr_power_down

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_bf3903_ycbcr_power_down(void)
{
    MSG_FATAL ("camsensor_bf3903_ycbcr_power_down begin", 0,0,0);
}


/*===========================================================================

FUNCTION      camsensor_bf3903_ycbcr_i2c_read_byte

DESCRIPTION
              8-bit I2C read.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - I2C read successful
  FALSE - I2C read failed


SIDE EFFECTS
  None

===========================================================================*/
static boolean camsensor_bf3903_ycbcr_i2c_read_byte(uint8 reg, uint8 *data) 
{
    static uint8 readbyte; 
	uint8  i;

	if (data == NULL)
	{
		return FALSE;
	}

	camsensor_i2c_command.addr.reg = reg;
	camsensor_i2c_command.buf_ptr  = (byte *)(&readbyte);
	camsensor_i2c_command.len      = 1;

	for (i =0; i < 3; ++i)
	{
		if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
		{
			*data  = readbyte;
			MSG_FATAL("camsensor_bf3903_ycbcr_i2c_read_byte: OK %x",reg,0,0);
			return TRUE;
		}
	}

	MSG_FATAL("camsensor_bf3903_ycbcr_i2c_read_byte: false %x",reg,0,0);
	return FALSE;
}

/*===========================================================================

FUNCTION      camsensor_bf3903_ycbcr_i2c_write_byte

DESCRIPTION
              8-bit I2C write.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - I2C write successful
  FALSE - I2C write failed


SIDE EFFECTS
  None

===========================================================================*/
static boolean camsensor_bf3903_ycbcr_i2c_write_byte(uint8 reg, uint8 data) 
{
    static uint8 writebyte;
	uint8 i;

	writebyte  = data;

	camsensor_i2c_command.addr.reg = reg;
	camsensor_i2c_command.buf_ptr  = (byte *)(&writebyte);
	camsensor_i2c_command.len      = 1;

	for (i = 0; i < 3; ++i)
	{
		if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
		{
			MSG_FATAL("camsensor_bf3903_ycbcr_i2c_write_byte: OK %x,%x",reg,data,0);
			return TRUE;
		}
	}
	MSG_FATAL("camsensor_bf3903_ycbcr_i2c_write_byte: false %x,%x",reg,data,0);
	return FALSE;
} /* camsensor_bf3903_ycbcr_i2c_write_byte */


/* The following are stub functions, (unsupported sensor functions) */
static camera_ret_code_type camsensor_bf3903_ycbcr_write_exposure_gain(uint16 gain, uint32 line_count)
{
	return(CAMERA_SUCCESS);
} /* camsensor_bf3903_ycbcr_write_exposure_gain */

//wlr 1114 start
static camera_ret_code_type camsensor_bf3903_set_effect(int8 effect) 
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;

	MSG_FATAL ("+++++ camsensor_bf3903_set_effect effect = %d",effect,0,0);
	
	switch(effect)
	{
		case CAMERA_EFFECT_OFF://normal
			break;
			
		case CAMERA_EFFECT_MONO:
			break;
			
		case CAMERA_EFFECT_NEGATIVE://负片
			break;
			
		case CAMERA_EFFECT_SEPIA://棕褐色
			break;

		default:
			ret_val = CAMERA_INVALID_PARM;
			break;
	}

  	return ret_val;
}/* camsensor_bf3903_set_effect */

static camera_ret_code_type camsensor_bf3903_set_wb(int8 wb) 
{
  	camera_ret_code_type ret_val = CAMERA_SUCCESS;

	MSG_FATAL ("+++++ camsensor_bf3903_set_wb wb = %d",wb,0,0);

  	switch (wb)
  	{
		case  CAMERA_WB_AUTO://自动
		    camsensor_bf3903_ycbcr_i2c_write_byte(0x13, 0x07);
	  		break;

		case CAMERA_WB_CLOUDY_DAYLIGHT://多云
            camsensor_bf3903_ycbcr_i2c_write_byte(0x13, 0x05); //WB_manual_gain 
            camsensor_bf3903_ycbcr_i2c_write_byte(0x01, 0x10); //WB_manual_gain 
            camsensor_bf3903_ycbcr_i2c_write_byte(0x02, 0x28);
            camsensor_bf3903_ycbcr_i2c_write_byte(0x6a, 0x01);
	  		break;
	
		case CAMERA_WB_INCANDESCENT://白炽灯
   		
           camsensor_bf3903_ycbcr_i2c_write_byte(0x13, 0x05); 
           camsensor_bf3903_ycbcr_i2c_write_byte(0x01, 0x1f);
           camsensor_bf3903_ycbcr_i2c_write_byte(0x02, 0x15);
           camsensor_bf3903_ycbcr_i2c_write_byte(0x6a, 0x01);
           
	  		break;

		case CAMERA_WB_FLUORESCENT: //荧光
		
            camsensor_bf3903_ycbcr_i2c_write_byte(0x13, 0x05); 
            camsensor_bf3903_ycbcr_i2c_write_byte(0x01, 0x1a);
            camsensor_bf3903_ycbcr_i2c_write_byte(0x02, 0x1e);
            camsensor_bf3903_ycbcr_i2c_write_byte(0x6a, 0x01);
            
	  		break;	
	  
		case CAMERA_WB_DAYLIGHT:  //日光
            camsensor_bf3903_ycbcr_i2c_write_byte(0x13, 0x05); 
            camsensor_bf3903_ycbcr_i2c_write_byte(0x01, 0x13); 
            camsensor_bf3903_ycbcr_i2c_write_byte(0x02, 0x26);
            camsensor_bf3903_ycbcr_i2c_write_byte(0x6a, 0x01); 
			break;

		default:
	  		ret_val = CAMERA_INVALID_PARM;
	  		break;
  	}

  	return ret_val;
}/* camsensor_bf3903_set_wb */
//wlr 1114 end

/*===========================================================================

FUNCTION	  camsensor_bf3903_set_brightness

DESCRIPTION   
  Set brightness.

DEPENDENCIES
  None

INPUT
  positive or negative value to change birghtness.

RETURN VALUE
  CAMERA_SUCCESS - if we changed brightness successfully
  CAMERA_FAILED  - if brightness change failed

SIDE EFFECTS
  This will change the output brgihtness.

===========================================================================*/
static camera_ret_code_type camsensor_bf3903_set_brightness (int8 value)
{
  	return CAMERA_SUCCESS;
} /* camsensor_bf3903_set_brightness */

/*===========================================================================

FUNCTION	  camsensor_bf3903_set_contrast

DESCRIPTION   
  Set contrast.

DEPENDENCIES
  None

INPUT
  positive or negative value to change contrast.

RETURN VALUE
  CAMERA_SUCCESS - if we changed contrast successfully
  CAMERA_FAILED  - if contrast change failed

SIDE EFFECTS
  This will change the output contrast.

===========================================================================*/
static camera_ret_code_type camsensor_bf3903_set_contrast (int8 contrast)
{
  	return CAMERA_SUCCESS;
} /* camsensor_bf3903_set_contrast */

static camera_ret_code_type camsensor_bf3903_set_frame_rate(uint16 fps)
{
  	return CAMERA_SUCCESS;
} /* camsensor_bf3903_set_frame_rate */

static camera_ret_code_type camsensor_bf3903_set_antibanding(int8 antibanding)
{
	return CAMERA_SUCCESS;
}

static uint16 camsensor_bf3903_0m3_get_snapshot_fps(uint16 fps)
{
	/* In the current driver, the snapshot fps is always 11 */
	fps = 15 * Q8;

	return fps;
}

static void    camsensor_bf3903_0m3_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
	camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 
	camctrl_tbl_ptr->la_is_supported              = FALSE;
	camctrl_tbl_ptr->cs_is_supported              = FALSE;
	camctrl_tbl_ptr->iso_is_supported             = FALSE;
	camctrl_tbl_ptr->enable_rolloff_correction    = FALSE;
	camctrl_tbl_ptr->hjr_bayer_filtering_enable   = FALSE;
	camctrl_tbl_ptr->nightshot_is_supported       = FALSE;

	camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_bf3903_0m3_get_snapshot_fps;
}
/*===========================================================================

FUNCTION   camsensor_bf3903_ycbcr_register 

DESCRIPTION  
  Exports the sensor specific functions to CAMSENSOR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_bf3903_ycbcr_register (camsensor_function_table_type *camsensor_function_table_ptr)
{
	camsensor_function_table_ptr->camsensor_start               	= camsensor_bf3903_ycbcr_start;
	camsensor_function_table_ptr->camsensor_video_config        	= camsensor_bf3903_ycbcr_video_config;
	camsensor_function_table_ptr->camsensor_raw_snapshot_config 	= camsensor_bf3903_ycbcr_raw_snapshot_config;
	camsensor_function_table_ptr->camsensor_snapshot_config     	= camsensor_bf3903_ycbcr_snapshot_config;
	camsensor_function_table_ptr->camsensor_power_up            	= camsensor_bf3903_ycbcr_power_up;
	camsensor_function_table_ptr->camsensor_power_down          	= camsensor_bf3903_ycbcr_power_down;
	camsensor_function_table_ptr->camsensor_write_exposure_gain 	= camsensor_bf3903_ycbcr_write_exposure_gain;
	camsensor_function_table_ptr->camsensor_set_frame_rate        	= camsensor_bf3903_set_frame_rate;
	camsensor_function_table_ptr->camsensor_set_effect				= camsensor_bf3903_set_effect; 
	camsensor_function_table_ptr->camsensor_set_wb					= camsensor_bf3903_set_wb;
	camsensor_function_table_ptr->camsensor_set_brightness 			= camsensor_bf3903_set_brightness;	 
	camsensor_function_table_ptr->camsensor_set_contrast 			= camsensor_bf3903_set_contrast;
	camsensor_function_table_ptr->camsensor_set_antibanding 		= camsensor_bf3903_set_antibanding; 
} /* camsensor_SIV121A_ycbcr_register */
#endif /* FEATURE_CAMERA */


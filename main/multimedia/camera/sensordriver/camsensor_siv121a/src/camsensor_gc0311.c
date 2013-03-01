/*============================================================================

                 Camera Interface Device Driver Source File for 
                    GC0311 VGA CMOS sensor
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
#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_GC0311)
#include "camerai.h"

#include "camsensor_GC0311.h"
#include "clk.h"


/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

#define CAMSENSOR_GC0311_I2C_SLAVE_ID              0x66 ///SIV121A

#define CAMSENSOR_GC0311_SENSOR_ID              0xbb ///SIV121A

#define CAMSENSOR_GC0311_RESET_PIN         GPIO_OUTPUT_10

/* 
 * Maximum number of trials before aborting a register write operation
 *
 */
#define MAX_REGISTER_WRITE_TRIALS   3
#define MAX_REGISTER_READ_TRIALS    3
/*begin\bug6192\yangtingyu\20090226\camera驱动更新，实现亮度调节*/
#define GC0311_MAX_BRIGHTNESS  9
#define GC0311_MIN_BRIGHTNESS  0
/*end\bug6192\yangtingyu\20090226\camera驱动更新，实现亮度调节*/

#define GC0311_MAX_CONTRAST   4
#define GC0311_MIN_CONTRAST   0

/* From the logic analyzer measurements */
#define OV_GC0311_YCBCR_FULL_SIZE_WIDTH           640//640
#define OV_GC0311_YCBCR_FULL_SIZE_HEIGHT          480

#define OV_GC0311_YCBCR_QTR_SIZE_WIDTH   		640//640   
#define OV_GC0311_YCBCR_QTR_SIZE_HEIGHT  		480//240//   

/* Strobe Flash Epoch Interrupt time before the end of line count */
#define OV_SIV121A_YCBCR_EPOCH_LINES_DELAY       1

#define GC0311_SET_PAGE0 	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x00)
#define GC0311_SET_PAGE1 	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x01)

/*===========================================================================
                          MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                        STATIC VARIABLES 
============================================================================*/
// modified Longcheer3G_liudan_20070403.........................................
const char camsensor_GC0311_ycbcr_sensor_name[]  = "GC0311 VGA";

// modified end.................................................................
/*============================================================================
                          INTERNAL API PROTOTYPES
============================================================================*/
static boolean camsensor_GC0311_ycbcr_i2c_write_byte(uint8 reg, uint8 data);
static void camsensor_GC0311_ycbcr_register(camsensor_function_table_type *camsensor_function_table_ptr);
static boolean camsensor_GC0311_ycbcr_i2c_read_byte(uint8 reg, uint8 *data); 
static boolean camsensor_GC0311_ycbcr_start(camsensor_static_params_type *camsensor_params );
static boolean camsensor_GC0311_ycbcr_video_config( camsensor_static_params_type *camsensor_params /* Other config params */ );
static void camsensor_GC0311_ycbcr_power_up(void);
static boolean camsensor_GC0311_ycbcr_snapshot_config( 	camsensor_static_params_type *camsensor_params /* Other config params */);
static camera_ret_code_type camsensor_GC0311_set_effect(int8 effect);
static camera_ret_code_type camsensor_GC0311_set_wb(int8 wb);
static camera_ret_code_type camsensor_GC0311_set_contrast(int8 contrast);
static camera_ret_code_type camsensor_GC0311_set_antibanding(int8 antibanding);
static camera_ret_code_type camsensor_GC0311_set_brightness(int8 value);
static void    camsensor_GC0311_0m3_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
/*============================================================================
                          EXTERNAL API DEFINITIONS
============================================================================*/
static  boolean camsensor_GC0311_sensor_init(void)
{
	MSG_FATAL("-----camsensor_GC0311_sensor_init-----enter",0,0,0);

	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0xf0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0xf0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0xf0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x42, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x4f, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x03, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x04, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfc, 0x16);
	//camsensor_GC0311_ycbcr_i2c_write_byte(0x77, 0x77);

	///////////////////////////////////////////////
	/////////// system reg ////////////////////////
	///////////////////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0xf1, 0x07);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xf2, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfc, 0x16);
	///////////////////////////////////////////////
	/////////// CISCTL////////////////////////
	///////////////////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x00);
	//////window setting/////
	camsensor_GC0311_ycbcr_i2c_write_byte(0x0d, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x0e, 0xe8);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x0f, 0x02);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x10, 0x88);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x09, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x0a, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x0b, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x0c, 0x04);
							
	camsensor_GC0311_ycbcr_i2c_write_byte(0x05, 0x02); 	
	camsensor_GC0311_ycbcr_i2c_write_byte(0x06, 0x2c); 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x07, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x08, 0xb8);
	///////////20120703////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0x77, 0x7c);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x78, 0x40);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x79, 0x56);

	GC0311_SET_PAGE1;
	camsensor_GC0311_ycbcr_i2c_write_byte(0x29, 0x00);   //anti-flicker step [11:8]
	camsensor_GC0311_ycbcr_i2c_write_byte(0x2a, 0x60);   //anti-flicker step [7:0]

	camsensor_GC0311_ycbcr_i2c_write_byte(0x2b, 0x02);   //exp level 0  14.28fps
	camsensor_GC0311_ycbcr_i2c_write_byte(0x2c, 0xa0); 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x2d, 0x03);   //exp level 1  12.50fps
	camsensor_GC0311_ycbcr_i2c_write_byte(0x2e, 0x00); 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x2f, 0x03);   //exp level 2  10.00fps
	camsensor_GC0311_ycbcr_i2c_write_byte(0x30, 0xc0); 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x31, 0x07);   //exp level 3  5.00fps
	camsensor_GC0311_ycbcr_i2c_write_byte(0x32, 0x80); 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x33, 0x20);
	GC0311_SET_PAGE0;
						  
	camsensor_GC0311_ycbcr_i2c_write_byte(0x17, 0x14);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x19, 0x04);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x1f, 0x08);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x20, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x21, 0x48);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x1b, 0x48);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x22, 0xba);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x23, 0x07);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x24, 0x16);
						   				   
	//global gain for range 	
	camsensor_GC0311_ycbcr_i2c_write_byte(0x70, 0x54);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x73, 0x80);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x76, 0x80);
	////////////////////////////////////////////////
	///////////////////////BLK//////////////////////
	////////////////////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0x26, 0xf7);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x28, 0x7f);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x29, 0x40);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x33, 0x18);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x34, 0x18);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x35, 0x18);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x36, 0x18);

	////////////////////////////////////////////////
	//////////////block enable/////////////////////
	////////////////////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0x40, 0xdf); 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x41, 0x2e); 

	camsensor_GC0311_ycbcr_i2c_write_byte(0x42, 0xff);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x44, 0xa2);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x46, 0x03);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x4d, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x4f, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x7e, 0x08);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x7f, 0xc3);
							
	//DN & EEINTP				
	camsensor_GC0311_ycbcr_i2c_write_byte(0x80, 0xe7);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x82, 0x30);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x84, 0x02);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x89, 0x22);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x90, 0xbc);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x92, 0x08);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x94, 0x08);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x95, 0x64);
							 
	/////////////////////ASDE/////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0x9a, 0x15);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x9c, 0x46);

	///////////////////////////////////////
	////////////////Y gamma ///////////////////
	////////////////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x63, 0x00); 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x64, 0x06); 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x65, 0x0c); 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x66, 0x18);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x67, 0x2A);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x68, 0x3D);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x69, 0x50);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x6A, 0x60);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x6B, 0x80);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x6C, 0xA0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x6D, 0xC0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x6E, 0xE0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x6F, 0xFF);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x00);

	///////////////////////////////////////
	////////////////RGB gamma //////////////
	///////////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0xBF, 0x0E);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc0, 0x1C);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc1, 0x34);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc2, 0x48);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc3, 0x5A);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc4, 0x6B);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc5, 0x7B);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc6, 0x95);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc7, 0xAB);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc8, 0xBF);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc9, 0xCE);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xcA, 0xD9);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xcB, 0xE4);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xcC, 0xEC);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xcD, 0xF7);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xcE, 0xFD);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xcF, 0xFF);

	////////////////////////////
	/////////////YCP//////////////
	////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0xd1, 0x36);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xd2, 0x36);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xdd, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xed, 0x00);

	camsensor_GC0311_ycbcr_i2c_write_byte(0xde, 0x38);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xe4, 0x88);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xe5, 0x40);

	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x18, 0x22);

	//////////////////////////////////
	///////////MEANSURE WINDOW////////
	/////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0x08, 0xa4);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x09, 0xf0);

	///////////////////////////////////////////////
	/////////////// AEC ////////////////////////
	///////////////////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x01);

	camsensor_GC0311_ycbcr_i2c_write_byte(0x10, 0x08);
			 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x11, 0x11);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x12, 0x14);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x13, 0x40);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x16, 0xd8);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x17, 0x98);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x18, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x21, 0xc0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x22, 0x40);

	//////////////////////////////
	/////////////AWB///////////////
	////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0x06, 0x10);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x08, 0xa0);
							
	camsensor_GC0311_ycbcr_i2c_write_byte(0x50, 0xfe);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x51, 0x05);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x52, 0x28);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x53, 0x05);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x54, 0x10);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x55, 0x20);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x56, 0x16);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x57, 0x10);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x58, 0xf0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x59, 0x10);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x5a, 0x10);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x5b, 0xf0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x5e, 0xe8);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x5f, 0x20);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x60, 0x20);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x61, 0xe0);
							
	camsensor_GC0311_ycbcr_i2c_write_byte(0x62, 0x03);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x63, 0x30);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x64, 0xc0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x65, 0xd0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x66, 0x20);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x67, 0x00);

	camsensor_GC0311_ycbcr_i2c_write_byte(0x6d, 0x40);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x6e, 0x08);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x6f, 0x08);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x70, 0x10);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x71, 0x62);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x72, 0x2e);//26 fast mode
	camsensor_GC0311_ycbcr_i2c_write_byte(0x73, 0x71);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x74, 0x23);
							
	camsensor_GC0311_ycbcr_i2c_write_byte(0x75, 0x40);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x76, 0x48);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x77, 0xc2);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x78, 0xa5);
							 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x79, 0x18);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x7a, 0x40);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x7b, 0xb0);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x7c, 0xf5);
							 
	camsensor_GC0311_ycbcr_i2c_write_byte(0x81, 0x80);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x82, 0x60);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x83, 0xa0);
							
	camsensor_GC0311_ycbcr_i2c_write_byte(0x8a, 0xf8);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x8b, 0xf4);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x8c, 0x0a);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x8d, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x8e, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x8f, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x90, 0x12);
							
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x00);

	///////////////////////////////////////////////
	/////////// SPI reciver////////////////////////
	///////////////////////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0xad, 0x00);

	/////////////////////////////
	///////////LSC///////////////
	/////////////////////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa0, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa1, 0x3c);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa2, 0x50);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa3, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa8, 0x09);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa9, 0x04);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xaa, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xab, 0x0c);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xac, 0x02);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xad, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xae, 0x15);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xaf, 0x05);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb0, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb1, 0x0f);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb2, 0x06);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb3, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb4, 0x36);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb5, 0x2a);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb6, 0x25);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xba, 0x36);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xbb, 0x25);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xbc, 0x22);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc0, 0x1e);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc1, 0x18);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc2, 0x17);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc6, 0x1c);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc7, 0x18);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc8, 0x17);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb7, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb8, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb9, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xbd, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xbe, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xbf, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc3, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc4, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc5, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xc9, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xca, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xcb, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa4, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa5, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa6, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xa7, 0x00);
	 ////////////20120613 start////////////
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x01);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x74, 0x13);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x15, 0xfe);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x21, 0xe0);

	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x41, 0x6e);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x83, 0x03);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x7e, 0x08);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x9c, 0x64);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x95, 0x65);
	
    	camsensor_GC0311_ycbcr_i2c_write_byte(0xd1, 0x2d);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xd2, 0x2d);

	camsensor_GC0311_ycbcr_i2c_write_byte(0xb0, 0x13);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb1, 0x26);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb2, 0x07);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb3, 0xf5);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb4, 0xea);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb5, 0x21);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb6, 0x21);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb7, 0xe4);
	camsensor_GC0311_ycbcr_i2c_write_byte(0xb8, 0xfb);
  ////////////20120613 end/////////////					  
	camsensor_GC0311_ycbcr_i2c_write_byte(0xfe, 0x00);
	camsensor_GC0311_ycbcr_i2c_write_byte(0x50, 0x01);  //crop
	camsensor_GC0311_ycbcr_i2c_write_byte(0x44, 0xa0);
	return  TRUE;
} /* camsensor_GC0311_sensor_init */


uint8 camsensor_GC0311_ycbcr_active(void)
{
	return 0;
}

uint8 camsensor_GC0311_ycbcr_unactive(void)
{
	return 1;
}

/*===========================================================================

FUNCTION      camsensor_GC0311_init

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

boolean camsensor_GC0311_init(camsensor_function_table_type *camsensor_function_table_ptr, camctrl_tbl_type *camctrl_tbl_ptr)
{
	uint8 sensor_id = 0;
    int j=3;
	MSG_FATAL("camsensor_GC0311_init()\n",0,0,0);///yty 

	camsensor_camclk_po_hz =12000000;

	//CAMERA_CONFIG_GPIO(CAMSENSOR_GC0311_RESET_PIN);
    
    gpio_out(CAMSENSOR1_POWER_PIN,0);
    clk_busy_wait(20*1000);
    gpio_out(CAMSENSOR1_POWER_PIN,1);
    clk_busy_wait(50*1000);
    gpio_out(CAMSENSOR1_POWER_PIN,0);
    clk_busy_wait(20*1000);

	camsensor_preview_resolution  = CAMSENSOR_FULL_SIZE;
	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;


	/* 1. Initiate I2C:  */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = CAMSENSOR_GC0311_I2C_SLAVE_ID;
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 

	///camsensor_GC0311_ycbcr_i2c_write_byte(0xfc, 0x16);

	clk_busy_wait(50*1000);
    // 3 times try
    while(j>0)
    {        
    	camsensor_GC0311_ycbcr_i2c_read_byte(0xf0,&sensor_id);
    	MSG_FATAL("Sensor ID = 0x%x",sensor_id,0,0);
    	if ( sensor_id == CAMSENSOR_GC0311_SENSOR_ID ) 
    	{
    		break;
    	}
        j--;
        i2c_init();
        gpio_out(CAMSENSOR1_POWER_PIN,0);
        clk_busy_wait(20*1000);
        gpio_out(CAMSENSOR1_POWER_PIN,1);
        clk_busy_wait(50*1000);
        gpio_out(CAMSENSOR1_POWER_PIN,0);
        clk_busy_wait(20*1000);
        
    }
    if(j<=0)
    {
      return FALSE;  
    }
	/* Initialize the sensor_function_table */
	camsensor_GC0311_ycbcr_register (camsensor_function_table_ptr);

	camsensor_GC0311_0m3_setup_camctrl_tbl(camctrl_tbl_ptr);
	return TRUE;
} /* camsensor_GC0311_init */


/*===========================================================================

FUNCTION      camsensor_GC0311_ycbcr_start

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
static boolean camsensor_GC0311_ycbcr_start( camsensor_static_params_type *camsensor_params)
{

	MSG_FATAL("camsensor_GC0311_ycbcr_start begin ", 0,0,0);///yty 

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
	camsensor_params->sensor_model = CAMSENSOR_ID_GC0311;

	/* CCD or CMOS */
	camsensor_params->sensor_type = CAMSENSOR_CMOS;

	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;
	/* BAYER or YCbCr */
	camsensor_params->output_format = CAMSENSOR_YCBCR;

	/* A pointer to the sensor name for EXIF tags                */
	camsensor_params->sensor_name = camsensor_GC0311_ycbcr_sensor_name;

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
	camsensor_params->full_size_width  = OV_GC0311_YCBCR_FULL_SIZE_WIDTH;
	camsensor_params->full_size_height = OV_GC0311_YCBCR_FULL_SIZE_HEIGHT;

	/* Preview dimensions */
	camsensor_params->qtr_size_width = OV_GC0311_YCBCR_QTR_SIZE_WIDTH;
	camsensor_params->qtr_size_height = OV_GC0311_YCBCR_QTR_SIZE_HEIGHT;

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

FUNCTION      camsensor_GC0311_ycbcr_snapshot_config

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

static boolean camsensor_GC0311_ycbcr_snapshot_config
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
} /* camsensor_GC0311_ycbcr_snapshot_config */

/*===========================================================================

FUNCTION      camsensor_GC0311_ycbcr_raw_snapshot_config

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

static boolean camsensor_GC0311_ycbcr_raw_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	return TRUE;
} /* camsensor_GC0311_ycbcr_raw_snapshot_config */


/*===========================================================================

FUNCTION      camsensor_GC0311_ycbcr_video_config

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

static boolean camsensor_GC0311_ycbcr_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
)
{
	camsensor_GC0311_sensor_init();
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
} /* camsensor_GC0311_ycbcr_video_config */

/*===========================================================================

FUNCTION      camsensor_GC0311_ycbcr_power_up

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_GC0311_ycbcr_power_up(void)
{
}


/*===========================================================================

FUNCTION      camsensor_GC0311_ycbcr_power_down

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_GC0311_ycbcr_power_down(void)
{
    MSG_FATAL ("camsensor_GC0311_ycbcr_power_down begin", 0,0,0);
}


/*===========================================================================

FUNCTION      camsensor_GC0311_ycbcr_i2c_read_byte

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
static boolean camsensor_GC0311_ycbcr_i2c_read_byte(uint8 reg, uint8 *data) 
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
			MSG_FATAL("camsensor_GC0311_ycbcr_i2c_read_byte: OK %x",reg,0,0);
			return TRUE;
		}
	}

	MSG_FATAL("camsensor_GC0311_ycbcr_i2c_read_byte: false %x",reg,0,0);
	return FALSE;
}

/*===========================================================================

FUNCTION      camsensor_GC0311_ycbcr_i2c_write_byte

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
static boolean camsensor_GC0311_ycbcr_i2c_write_byte(uint8 reg, uint8 data) 
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
			MSG_FATAL("camsensor_GC0311_ycbcr_i2c_write_byte: OK %x,%x",reg,data,0);
			return TRUE;
		}
	}
	MSG_FATAL("camsensor_GC0311_ycbcr_i2c_write_byte: false %x,%x",reg,data,0);
	return FALSE;
} /* camsensor_GC0311_ycbcr_i2c_write_byte */


/* The following are stub functions, (unsupported sensor functions) */
static camera_ret_code_type camsensor_GC0311_ycbcr_write_exposure_gain(uint16 gain, uint32 line_count)
{
	return(CAMERA_SUCCESS);
} /* camsensor_GC0311_ycbcr_write_exposure_gain */

//wlr 1114 start
static camera_ret_code_type camsensor_GC0311_set_effect(int8 effect) 
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;

	MSG_FATAL ("+++++ camsensor_GC0311_set_effect effect = %d",effect,0,0);
	
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
}/* camsensor_GC0311_set_effect */

static camera_ret_code_type camsensor_GC0311_set_wb(int8 wb) 
{
  	camera_ret_code_type ret_val = CAMERA_SUCCESS;

	MSG_FATAL ("+++++ camsensor_GC0311_set_wb wb = %d",wb,0,0);

  	switch ((camera_wb_type)wb)
  	{
		case  CAMERA_WB_AUTO://自动
		    camsensor_GC0311_ycbcr_i2c_write_byte(0x42, 0xfe);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x77, 0x8c); //WB_manual_gain 
			camsensor_GC0311_ycbcr_i2c_write_byte(0x78, 0x50);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x79, 0x40);
	  		break;

		case CAMERA_WB_CLOUDY_DAYLIGHT://多云
		    camsensor_GC0311_ycbcr_i2c_write_byte(0x42, 0xfd);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x77, 0x8c); //WB_manual_gain 
			camsensor_GC0311_ycbcr_i2c_write_byte(0x78, 0x50);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x79, 0x40);
	  		break;
	
		case CAMERA_WB_INCANDESCENT://白炽灯
		    camsensor_GC0311_ycbcr_i2c_write_byte(0x42, 0xfd);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x77, 0x48);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x78, 0x40);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x79, 0x5c);
	  		break;

		case CAMERA_WB_FLUORESCENT: //荧光
		    camsensor_GC0311_ycbcr_i2c_write_byte(0x42, 0xfd);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x77, 0x40);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x78, 0x42);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x79, 0x50);
	  		break;	
	  
		case CAMERA_WB_DAYLIGHT:  //日光
		    camsensor_GC0311_ycbcr_i2c_write_byte(0x42, 0xfd);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x77, 0x74); 
			camsensor_GC0311_ycbcr_i2c_write_byte(0x78, 0x52);
			camsensor_GC0311_ycbcr_i2c_write_byte(0x79, 0x40);
			break;

		default:
	  		ret_val = CAMERA_INVALID_PARM;
	  		break;
  	}

  	return ret_val;
}/* camsensor_GC0311_set_wb */
//wlr 1114 end

/*===========================================================================

FUNCTION	  camsensor_GC0311_set_brightness

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
static camera_ret_code_type camsensor_GC0311_set_brightness (int8 value)
{
  	return CAMERA_SUCCESS;
} /* camsensor_GC0311_set_brightness */

/*===========================================================================

FUNCTION	  camsensor_GC0311_set_contrast

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
static camera_ret_code_type camsensor_GC0311_set_contrast (int8 contrast)
{
  	return CAMERA_SUCCESS;
} /* camsensor_GC0311_set_contrast */

static camera_ret_code_type camsensor_GC0311_set_frame_rate(uint16 fps)
{
  	return CAMERA_SUCCESS;
} /* camsensor_GC0311_set_frame_rate */

static camera_ret_code_type camsensor_GC0311_set_antibanding(int8 antibanding)
{
	return CAMERA_SUCCESS;
}

static uint16 camsensor_GC0311_0m3_get_snapshot_fps(uint16 fps)
{
	/* In the current driver, the snapshot fps is always 11 */
	fps = 15 * Q8;

	return fps;
}

static void    camsensor_GC0311_0m3_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
	camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 
	camctrl_tbl_ptr->la_is_supported              = FALSE;
	camctrl_tbl_ptr->cs_is_supported              = FALSE;
	camctrl_tbl_ptr->iso_is_supported             = FALSE;
	camctrl_tbl_ptr->enable_rolloff_correction    = FALSE;
	camctrl_tbl_ptr->hjr_bayer_filtering_enable   = FALSE;
	camctrl_tbl_ptr->nightshot_is_supported       = FALSE;

	camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_GC0311_0m3_get_snapshot_fps;
}
/*===========================================================================

FUNCTION   camsensor_GC0311_ycbcr_register 

DESCRIPTION  
  Exports the sensor specific functions to CAMSENSOR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_GC0311_ycbcr_register (camsensor_function_table_type *camsensor_function_table_ptr)
{
	camsensor_function_table_ptr->camsensor_start               	= camsensor_GC0311_ycbcr_start;
	camsensor_function_table_ptr->camsensor_video_config        	= camsensor_GC0311_ycbcr_video_config;
	camsensor_function_table_ptr->camsensor_raw_snapshot_config 	= camsensor_GC0311_ycbcr_raw_snapshot_config;
	camsensor_function_table_ptr->camsensor_snapshot_config     	= camsensor_GC0311_ycbcr_snapshot_config;
	camsensor_function_table_ptr->camsensor_power_up            	= camsensor_GC0311_ycbcr_power_up;
	camsensor_function_table_ptr->camsensor_power_down          	= camsensor_GC0311_ycbcr_power_down;
	camsensor_function_table_ptr->camsensor_write_exposure_gain 	= camsensor_GC0311_ycbcr_write_exposure_gain;
	camsensor_function_table_ptr->camsensor_set_frame_rate        	= camsensor_GC0311_set_frame_rate;
	camsensor_function_table_ptr->camsensor_set_effect				= camsensor_GC0311_set_effect;
	camsensor_function_table_ptr->camsensor_set_wb					= camsensor_GC0311_set_wb; 
	camsensor_function_table_ptr->camsensor_set_brightness 			= camsensor_GC0311_set_brightness;	 
	camsensor_function_table_ptr->camsensor_set_contrast 			= camsensor_GC0311_set_contrast;
	camsensor_function_table_ptr->camsensor_set_antibanding 		= camsensor_GC0311_set_antibanding; 
} /* camsensor_SIV121A_ycbcr_register */
#endif /* FEATURE_CAMERA */

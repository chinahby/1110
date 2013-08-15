/*============================================================================

                 Camera Interface Device Driver Source File for 
                    DB8V63A VGA CMOS sensor
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

$Header: //depot/asic/msm6550/drivers/camsensor/camsensor_ DB8V63A_ycbcr.c#3 $ $DateTime: 2006/08/11 14:02:49 $ $Author: klee $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
  09/30/06  lee    Initial release.
 
============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "camsensor.h"
#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_DB8B63A)
#include "camerai.h"

#include "camsensor_DB8V63A_yuv.h"
#include "clk.h"

#ifdef LOCAL
#undef LOCAL
#endif
#define LOCAL static

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

#define CAMSENSOR_DB8V63A_I2C_SLAVE_ID              0xA8 ///DB8V63A

/* 
 * Maximum number of trials before aborting a register write operation
 *
 */
#define MAX_REGISTER_WRITE_TRIALS   3
#define MAX_REGISTER_READ_TRIALS    3
/*camera驱动更新，实现亮度调节*/
#define DB8V63A_MAX_BRIGHTNESS  9
#define DB8V63A_MIN_BRIGHTNESS  0
/*end\bug6192\20090226\camera驱动更新，实现亮度调节*/

#define DB8V63A_MAX_CONTRAST   4
#define DB8V63A_MIN_CONTRAST   0

#define RETURN_ON_FALSE(rc) if ((rc)==FALSE) return FALSE  ///lee

#define OV_DB8V63A_YCBCR_TOTAL_STEPS_NEAR_TO_FAR   26
#define OV_DB8V63A_YCBCR_STEPS_NEAR_TO_CLOSEST_INF 20

/* From the logic analyzer measurements */
#define OV_DB8V63A_YCBCR_FULL_SIZE_WIDTH           640
#define OV_DB8V63A_YCBCR_FULL_SIZE_HEIGHT          480

#define OV_DB8V63A_YCBCR_QTR_SIZE_WIDTH   		640      
#define OV_DB8V63A_YCBCR_QTR_SIZE_HEIGHT  		480///yty     

/* Strobe Flash Epoch Interrupt time before the end of line count */
#define OV_DB8V63A_YCBCR_EPOCH_LINES_DELAY       4

#define CAMSENSOR_DB8V63A_ENABLE_PIN GPIO_OUTP_27
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_K212_12832
#define CAMSENSOR_DB8V63A_RESET_PIN  GPIO_OUTPUT_10
#endif
#endif
/*===========================================================================
                          MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                        STATIC VARIABLES 
============================================================================*/

// modified Longcheer3G_liudan_20070403.........................................
const char camsensor_DB8V63A_ycbcr_sensor_name[]  = "DB8V63A VGA";

// modified end.................................................................
/*============================================================================
                          INTERNAL API PROTOTYPES
============================================================================*/
static boolean camsensor_DB8V63A_ycbcr_write_sensor(camsensor_sensor_resolution_type);
static void camsensor_DB8V63A_ycbcr_register(camsensor_function_table_type *camsensor_function_table_ptr);
boolean camsensor_DB8V63A_ycbcr_i2c_write_byte(uint8 reg, uint8 data);
static boolean camsensor_DB8V63A_ycbcr_i2c_read_byte(uint8 reg, uint8 *data); 


/*============================================================================
                          EXTERNAL API DEFINITIONS
============================================================================*/
LOCAL boolean camsensor_DB8V63A_sensor_init(void)
{

	// Sensor		: DB8V63A
	// Copyright		: 
	// Date     		: 2010/01/25 (Analog 01/25)
	// Comment		: MCLK : 24MHz
	//====================================================================


	//==================================
	// preview command
	//==================================
	 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0xA0);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x10,	0x01);		// Idle
	clk_busy_wait(50);

	//==================================
	// SensorCon
	//==================================

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0xB0);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x02,	0x01);		// Sensor Enable
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x03,	0x00);		// Y Flip 

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x12,	0x44);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x14,	0x80);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x17,	0xe3);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x1e,	0x01);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x4e,	0x01);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x4f,	0x9a);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x67,	0x0d);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x68,	0x15);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x69,	0x1f);		// 

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x61);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xd4,	0x01);		// Fgain : for B99A01A

	//==================================
	// ISP Global Control #1
	//==================================

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x80);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x00,	0xFF); 		// Bayer Function
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x01,	0xDF); 		// RgbYc Function
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x02,	0x00); 		// Test Mode


	//----------------------------------
	// LSC
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x46,	0xa0); 		// RRp0Gain
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x47,	0x30); 		// RRp1Gain
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x48,	0x00); 		// RRp2Gain
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x49,	0x80); 		// GGp0Gain
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x4A,	0x30); 		// GGp1Gain
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x4B,	0x00); 		// GGp2Gain
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x4C,	0x88); 		// BBp0Gain
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x4D,	0x28); 		// BBp1Gain
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x4E,	0x00); 		// BBp2Gain
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x4F,	0xFF); 		// ShdGGain

	//CDC
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x6b,	0x00);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x6c,	0x00);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x6d,	0x00);		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x6e,	0x00);		// 

	//BCD
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xd1,	0x20);		// 

	//==================================
	// ISP Global Control #2
	//==================================

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x81);	// Page Mode

	//GAMMA
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x00,	0x0a); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x01,	0x11); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x02,	0x30); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x03,	0x46); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x04,	0x5c); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x05,	0x7a); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x06,	0x8f); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x07,	0x9d); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x08,	0xac); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x09,	0xc1); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x0a,	0xd3); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x0b,	0xe3); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x0c,	0xeb); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x0d,	0xf2); 		//
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x0e,	0xf9); 		//

	//----------------------------------
	// Edge
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x30,	0x36); 		// Edge Option
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x31,	0x00);		// Edge Luminance adaptive
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x32,	0x04); 		// Edge PreCoring point
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x37,	0x12); 		// Edge Transfer slope 1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x38,	0x0e); 		// Edge Transfer slope 2


	//==================================
	// AE
	//==================================

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xff,	0x81);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x91,	0x00);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x92,	0x00);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x93,	0x35);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x94,	0x18);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x95,	0x6b);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x96,	0x60);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x97,	0xa0);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x98,	0x78);		// 

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x61);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xE4,	0x01); 		// AE Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xE5,	0x19); 		// AE Ctrl
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xE8,	0xee); 		// AE Weight (Left / Top)
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xE9,	0xed); 		// AE Weight (Right / Center)
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xEA,	0xee); 		// AE weight (Factor / Bottom)
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xEB,	0x06); 		// AE Speed

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFA,	0x34); 		// Outdoor AE Target
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFB,	0x38); 		// Indoor AE Target
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x62);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x0D,	0x40);		//0x2c// Peak AE Target min
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x0F,	0x02);		// Peak Y Threshold
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x11,	0x10); 		// LuxGainTB_0
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x12,	0x30); 		// LuxGainTB_1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x13,	0x60); 		// LuxGainTB_2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x14,	0x03); 		// LuxTimeTB_0
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x15,	0x00); 		// LuxTimeTB_1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x16,	0x04); 		// LuxTimeTB_2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x17,	0x00); 		// LuxTimeTB_3
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x18,	0x05); 		// LuxTimeTB_4
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x19,	0x1F); 		// LuxTimeTB_5
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x1A,	0x40); 		// LuxUpperIndex
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x1B,	0x08); 		// LuxLowerIndex	

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x31,	0x04);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x32,	0xb0);		// 


	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x36,	0x00); 		// AE Gain Max High
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x37,	0x80); 		// AE Gain Max Low
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x38,	0x30); 		// Gain2Lut
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x39,	0x20); 		// Gain1Lut
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x3A,	0x14); 		// GainMin
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x3B,	0x01); 		// Min Frame	// 7.5 fps
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x3C,	0xf5); 		// Min Frame
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x3D,	0x18); 		// 60Hz Max Time
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x3E,	0x0C); 		// Time2Lut60hz
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x3F,	0x08); 		// Time1Lut60hz
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x40,	0x14); 		// 50Hz Max Time
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x41,	0x0A); 		// Time2Lut50hz
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x42,	0x06); 		// Time1Lut50hz
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x67,	0x01); 		// 60Hz Min Time High
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x68,	0xf5); 		// 60Hz Min Time Low
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x69,	0x01); 		// 50Hz Min Time High
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x6A,	0xe9); 		// 50Hz Min Time Low
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x61);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xE5,	0x39); 		// AE Ctrl

	//==================================
	// Vsync mode
	//==================================
	 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x81);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x71,	0x1d);		//

	//==================================
	// preview command
	//==================================
	 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0xA0);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x10,	0x02);		// Idle

	//----------------------------------
	// AWB STE
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x81);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x99,	0xC8);		// AE Histogram Threshold
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x9F,	0x21);		// AWBZone0LTx		// Flash
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA0,	0x6E);		// AWBZone0LTy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA1,	0x35);		// AWBZone0RBx
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA2,	0x5A);		// AWBZone0RBy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA3,	0x87);		// AWBZone1LTx		// Cloudy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA4,	0x50);		// AWBZone1LTy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA5,	0x96);		// AWBZone1RBx
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA6,	0x41);		// AWBZone1RBy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA7,	0x71);		// AWBZone2LTx		// DayLight
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA8,	0x5A);		// AWBZone2LTy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xA9,	0x85);		// AWBZone2RBx
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xAA,	0x3C);		// AWBZone2RBy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xAB,	0x54);		// AWBZone3LTx		// Fluorescent
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xAC,	0x5F);		// AWBZone3LTy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xAD,	0x6E);		// AWBZone3RBx
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xAE,	0x46);		// AWBZone3RBy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xAF,	0x3B);		// AWBZone4LTx		// Coolwhite
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB0,	0x69);		// AWBZone4LTy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB1,	0x52);		// AWBZone4RBx
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB2,	0x4E);		// AWBZone4RBy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB3,	0x46);		// AWBZone5LTx		// TL84
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB4,	0x6E);		// AWBZone5LTy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB5,	0x58);		// AWBZone5RBx
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB6,	0x64);		// AWBZone5RBy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB7,	0x3E);		// AWBZone6LTx		// A
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB8,	0x88);		// AWBZone6LTy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xB9,	0x48);		// AWBZone6RBx
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xBA,	0x7E);		// AWBZone6RBy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xBB,	0x37);		// AWBZone7LTx		// Horizon
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xBC,	0xA1);		// AWBZone7LTy
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xBD,	0x43);		// AWBZone7RBx
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xBE,	0x95);		// AWBZone7RBy



	//==================================
	// AWB
	//==================================

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x62);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x90,	0x10);	// HENRY 白平衡容易变化
	//camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x90,	0x30);	// Min Gray Count High
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x91,	0x00);	// Min Gray Count Low

	//==================================
	// CCM
	//==================================
	

	//==================================
	// ADF
	//==================================

	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x63);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x70,	0xeF); 		// ISP Adaptive Enable
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x71,	0x03); 		// Analog Adaptive Enable

	// ADF Threshold
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x77,	0xF4); 		// Th_LSC
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x78,	0x04); 		// Th_CDC
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x79,	0x42); 		// Th_NSF
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x7A,	0x84); 		// Th_CNF
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x7B,	0x32); 		// Th_EDE
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x7C,	0x84); 		// Th_GDC
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x7D,	0x42); 		// Th_Gamma
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x7E,	0x32); 		// Th_Suppression

	// ADF Min Value	// Dark
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x7F,	0x80); 		// Min_LSC
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x80,	0x20); 		// Min_NSFTh1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x81,	0x40); 		//0x60// Min_NSFTh2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x82,	0x14); 		// Min_CNFTh1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x83,	0x40); 		// Min_CNFTh2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x84,	0x10); 		// Min_EdgeCo
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x85,	0x04); 		// Min_EdgeSlp1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x86,	0x04); 		// Min_EdgeSlp2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x87,	0x40); 		// Min_GDCTh1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x88,	0x80); 		// Min_GDCTh2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x89,	0x78); 		// Min_Suppression

	// ADF Max Value	// Brightness
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x8A,	0xA0);		// Max_LSC
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x8B,	0x10); 		// Max_NSFTh1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x8C,	0x80); 		// Max_NSFTh2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x8D,	0x0A); 		// Max_CNFTh1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x8E,	0x34); 		// Max_CNFTh2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x8F,	0x02); 		// Max_EdgeCo
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x90,	0x0a); 		// Max_EdgeSlp1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x91,	0x10); 		// Max_EdgeSlp2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x92,	0x20); 		// Max_GDCTh1
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x93,	0x40); 		// Max_GDCTh2
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x94,	0x80); 		// Max_Suppression

	
	//==================================
	// ADF Analog
	//==================================
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xff,	0x63);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xcd,	0x23);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xd5,	0x0a);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xd7,	0x23);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xde,	0x60);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x74,	0x20); 		// ADF_PreLux	

	//==================================
	// Brightness/Contrast
	//==================================
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xff,	0x61);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xb7,	0x00);		//0x08// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xb8,	0x80);		//0x88//

	//==================================
	// Saturation
	//==================================
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xff,	0x63);		// 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xc6,	0x40);		//0x30 
	/*
	//==================================
	// Vsync mode
	//==================================
	 

	//==================================
	// preview command
	//==================================

	*/

	//==================================
	// 3A
	//==================================
	 
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0xFF,	0x61);	// Page Mode
	camsensor_DB8V63A_ycbcr_i2c_write_byte( 0x8C,	0x2F);		// AFC Disable
	return	TRUE;
} /* camsensor_ov7690_write_sensor */





/*===========================================================================

FUNCTION      CAMSENSOR_DB8V63A_YCBCR_INIT

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
boolean camsensor_DB8V63A_ycbcr_init (camsensor_function_table_type *camsensor_function_table_ptr,camctrl_tbl_type              *camctrl_tbl_ptr)
{
                                                                                    

  uint8  manufacturer_id_msb;
  uint8  manufacturer_id_lsb;


	MSG_HIGH("camsensor_DB8V63A_ycbcr_init()\n",0,0,0);///yty 


///firste power enable, then reset yty
    camsensor_camclk_po_hz =24000000;
 
    camsensor_camclk_po_hz = camsensor_config_camclk_po(camsensor_camclk_po_hz);
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_K212_12832

  	CAMERA_CONFIG_GPIO(CAMSENSOR_DB8V63A_RESET_PIN);
  	gpio_out(CAMSENSOR_DB8V63A_RESET_PIN,1);
  	clk_busy_wait(2*100);
  	gpio_out(CAMSENSOR_DB8V63A_RESET_PIN,0);
  	clk_busy_wait(100*100);
  	gpio_out(CAMSENSOR_DB8V63A_RESET_PIN,1);
  	clk_busy_wait(2*100);
#endif
#endif
    camsensor_preview_resolution  = CAMSENSOR_FULL_SIZE;
    camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;
    //camsensor_current_resolution  = CAMSENSOR_INVALID_SIZE;

    /* 1. Initiate I2C:  */
    camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
    camsensor_i2c_command.slave_addr = CAMSENSOR_DB8V63A_I2C_SLAVE_ID;
    camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 

    /* Auto Detect the sensor */

  
    /* 3. Read sensor manufacturer ID MSB:       */
  
  camsensor_DB8V63A_ycbcr_i2c_write_byte(0xff,0xB0);
  
  if (camsensor_DB8V63A_ycbcr_i2c_read_byte(0x00, &manufacturer_id_msb) == FALSE) ///0x92
  {
  	MSG_HIGH("camsensor_DB8V63A_read REG0x1 fail %x ",manufacturer_id_msb,0,0);///yty 

    return FALSE;
  }
  MSG_FATAL("manufacturer_id_msb===%d",0,0,0);
  if (camsensor_DB8V63A_ycbcr_i2c_read_byte(0x01, &manufacturer_id_lsb) == FALSE) ///0x11
  {
  	MSG_HIGH("camsensor_DB8V63A_read REG0x2 fail %x ",manufacturer_id_lsb,0,0);///yty 
    return FALSE;
  }

  MSG_FATAL("Sensor_id = %02x, version_id = %02x",manufacturer_id_msb,manufacturer_id_lsb,0);
  if ( (manufacturer_id_msb != 0x50) || (manufacturer_id_lsb != 0x00) ) 
      
  {
    return FALSE;
  }

  /* Always initialize the sensor with preview resolution */
  camsensor_DB8V63A_sensor_init();
	MSG_FATAL("camsensor_DB8V63A_ycbcr_register........",0,0,0);
   

  /* Initialize the sensor_function_table */
  camsensor_DB8V63A_ycbcr_register (camsensor_function_table_ptr);

  return TRUE;
} /* camsensor_DB8V63A_ycbcr_init */


/*===========================================================================

FUNCTION      CAMSENSOR_DB8V63A_YCBCR_START

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
boolean camsensor_DB8V63A_ycbcr_start( camsensor_static_params_type *camsensor_params)
{
	//SC_DBG(("camsensor_DB8V63A_ycbcr_start()\n"));///yty 

  /* Initialize CAMIF operation mode */
  camsensor_params->camif_config.SyncMode         = CAMIF_APS;
  camsensor_params->camif_config.HSyncEdge        = CAMIF_High;//CAMIF_High
  camsensor_params->camif_config.VSyncEdge        = CAMIF_High;
  camsensor_params->camif_efs_config.EFS_EOL      = 0x0000;
  camsensor_params->camif_efs_config.EFS_SOL      = 0x0000;
  camsensor_params->camif_efs_config.EFS_EOF      = 0x0000;
  camsensor_params->camif_efs_config.EFS_SOF      = 0x0000;

  /* ------------------  Sensor-specific Config -------------- */
  /* Make/model of sensor */
  camsensor_params->sensor_model = CAMSENSOR_DB8V63A_ID;

  /* CCD or CMOS */
  camsensor_params->sensor_type = CAMSENSOR_CMOS;

  camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;
  /* BAYER or YCbCr */
    camsensor_params->output_format = CAMSENSOR_YCBCR;

  /* A pointer to the sensor name for EXIF tags                */
  camsensor_params->sensor_name = camsensor_DB8V63A_ycbcr_sensor_name;

  /* What is the maximum FPS that can be supported by this sensor
     in video mode */
  camsensor_params->max_video_fps = 15 * Q8;

  /* Application assigned FPS in video mode */
  camsensor_params->video_fps = 15 * Q8;

  /* Snapshot mode operation */
  camsensor_params->max_preview_fps = 15 * Q8;
  camsensor_params->nightshot_fps = 15 * Q8 /2;

  /* May be assigned with max_preview_fps or nightshot_fps. */
  camsensor_params->preview_fps = 15 * Q8;

   /* PCLK Invert */
 // camsensor_params->pclk_invert = TRUE;

  /* VFE's perception of Sensor output capability */

  /* Snapshot dimensions */
  camsensor_params->full_size_width  = OV_DB8V63A_YCBCR_FULL_SIZE_WIDTH;
  camsensor_params->full_size_height = OV_DB8V63A_YCBCR_FULL_SIZE_HEIGHT;

  /* Preview dimensions */
  camsensor_params->qtr_size_width = OV_DB8V63A_YCBCR_QTR_SIZE_WIDTH;
  camsensor_params->qtr_size_height = OV_DB8V63A_YCBCR_QTR_SIZE_HEIGHT;

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
  /* Num steps to go across whole range */
  camsensor_params->num_steps_near_to_far = OV_DB8V63A_YCBCR_TOTAL_STEPS_NEAR_TO_FAR;  ///lee test 1004 //wlr 070413


  /* Default position at optimum general focus: nearest infinity
     point.  This is in terms of number of steps from near side.
     This, with the number above, defines the search space. */
  camsensor_params->num_steps_near_to_closest_infinity = OV_DB8V63A_YCBCR_STEPS_NEAR_TO_CLOSEST_INF; ///lee test 1004 //wlr070413

  /* Num steps for coarse first phase sweep search */
  camsensor_params->num_gross_steps_between_stat_points = 5; ///lee test 1004  //wlr070413

  /* Num steps for fine second phase sweep search */
  camsensor_params->num_fine_steps_between_stat_points = 1; ///lee test 1004  //wlr070413

  /* Num search points to gather when doing 2nd pass fine search */
  camsensor_params->num_fine_search_points = 5; ///lee test 1004  //wlr070413

  /* ------------  Default Misc Parmas Config ---------------- */
  /* Does the sensor need/use a flash  */
#if defined FEATURE_WHITE_LED_FLASH || defined FEATURE_STROBE_FLASH
  camsensor_params->support_auto_flash = FALSE;//TRUE;   ///lee test 1004
#else
  camsensor_params->support_auto_flash = FALSE;
#endif

  return TRUE;
} /* camsensor_DB8V63A_ycbcr_start */


/*===========================================================================

FUNCTION      CAMSENSOR_DB8V63A_YCBCR_SNAPSHOT_CONFIG

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

boolean camsensor_DB8V63A_ycbcr_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
 /* Sensor output data format */
  camsensor_params->format = CAMIF_YCbCr_Cb_Y_Cr_Y;

  /* Discard the first frame? */
  camsensor_params->discardFirstFrame =  TRUE;

  /* CAMIF frame */
  camsensor_params->camif_frame_config.pixelsPerLine = OV_DB8V63A_YCBCR_FULL_SIZE_WIDTH * 2;
  camsensor_params->camif_frame_config.linesPerFrame = OV_DB8V63A_YCBCR_FULL_SIZE_HEIGHT;

  /* CAMIF window */
  camsensor_params->camif_window_width_config.firstPixel = 0;//yty
  camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel + (camsensor_params->full_size_width * 2) - 1;
  camsensor_params->camif_window_height_config.firstLine = 0;//yty
  camsensor_params->camif_window_height_config.lastLine = camsensor_params->camif_window_height_config.firstLine + camsensor_params->full_size_height - 1;

  /* Set the current dimensions */
  camsensor_params->camsensor_width  = camsensor_params->full_size_width;
  camsensor_params->camsensor_height = camsensor_params->full_size_height;

#ifdef FEATURE_STROBE_FLASH
  camsensor_params->epoch_line = 1192;
#else
  /* Disable the Epoch Interrupt by setting more
   * number of lines than in a frame */
  camsensor_params->epoch_line = 10000;
#endif /* FEATURE_STROBE_FLASH */

  /* Update sensor resolution */ 
  if ( (camsensor_current_resolution != camsensor_snapshot_resolution) &&
       (camsensor_DB8V63A_sensor_init () == FALSE) ) //
  {
    return FALSE;
  }
  else {
    camsensor_current_resolution = camsensor_snapshot_resolution;
  }
  // camera_timed_wait(40);//yty
  return TRUE;
} /* camsensor_DB8V63A_ycbcr_snapshot_config */

/*===========================================================================

FUNCTION      CAMSENSOR_DB8V63A_YCBCR_RAW_SNAPSHOT_CONFIG

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

boolean camsensor_DB8V63A_ycbcr_raw_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
  return TRUE;
} /* camsensor_DB8V63A_ycbcr_raw_snapshot_config */


/*===========================================================================

FUNCTION      CAMSENSOR_DB8V63A_YCBCR_VIDEO_CONFIG

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

boolean camsensor_DB8V63A_ycbcr_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
)
{
  /* Sensor output data format */
  camsensor_params->format = CAMIF_YCbCr_Cb_Y_Cr_Y;

  switch (camsensor_preview_resolution)
  {
    case CAMSENSOR_QTR_SIZE:
      /* Set the current dimensions */
      camsensor_params->camsensor_width = camsensor_params->qtr_size_width;
      camsensor_params->camsensor_height = camsensor_params->qtr_size_height;
      /* CAMIF frame */
      camsensor_params->camif_frame_config.pixelsPerLine = OV_DB8V63A_YCBCR_QTR_SIZE_WIDTH * 2;
      camsensor_params->camif_frame_config.linesPerFrame = OV_DB8V63A_YCBCR_QTR_SIZE_HEIGHT;
      break;

    case CAMSENSOR_FULL_SIZE:
      /* Set the current dimensions */
      camsensor_params->camsensor_width = camsensor_params->full_size_width;
      camsensor_params->camsensor_height = camsensor_params->full_size_height;
      /* CAMIF frame */
      camsensor_params->camif_frame_config.pixelsPerLine = OV_DB8V63A_YCBCR_FULL_SIZE_WIDTH * 2;
      camsensor_params->camif_frame_config.linesPerFrame = OV_DB8V63A_YCBCR_FULL_SIZE_HEIGHT;
      break;

    default:
      return FALSE;
  } /* camsensor_preview_resolution */

  /* CAMIF window */
  camsensor_params->camif_window_width_config.firstPixel = 0;
  camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel + camsensor_params->camsensor_width*2 - 1;
  camsensor_params->camif_window_height_config.firstLine = 0;
  camsensor_params->camif_window_height_config.lastLine = camsensor_params->camif_window_height_config.firstLine + camsensor_params->camsensor_height - 1;

  /* Disable the Epoch Interrupt by setting more
   * number of lines than in a frame */
  camsensor_params->epoch_line = 10000; ///lee test 1004
  
  //if(camsensor_DB8V63A_sensor_init()==FALSE)//YTY
  //{
    //return FALSE;   
  //}
  
  camsensor_current_resolution = camsensor_preview_resolution;
 //  camera_timed_wait(40);//yty

  return TRUE;
} /* camsensor_DB8V63A_ycbcr_video_config */

/*===========================================================================

FUNCTION      CAMSENSOR_DB8V63A_YCBCR_POWER_UP

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_DB8V63A_ycbcr_power_up(void)
{
	//gpio_out(CAMSENSOR_DB8V63A_ENABLE_PIN,0);
}


/*===========================================================================

FUNCTION      camsensor_DB8V63A_ycbcr_POWER_DOWN

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_DB8V63A_ycbcr_power_down(void)
{
  // CAMERA_CONFIG_GPIO (CAMIF_EN_N);
  //gpio_out(CAMSENSOR_DB8V63A_ENABLE_PIN, GPIO_HIGH_VALUE);
  //gpio_out(CAMIF_RST_N, GPIO_LOW_VALUE);

}


/*===========================================================================

FUNCTION      camsensor_DB8V63A_ycbcr_i2c_read_byte

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
static boolean camsensor_DB8V63A_ycbcr_i2c_read_byte(uint8 reg, uint8 *data) 
{
  uint8 trials_count;

  if (data == NULL) 
  {
    return FALSE;
  }

  camsensor_i2c_command.addr.reg = reg;
  camsensor_i2c_command.buf_ptr  = data;
  camsensor_i2c_command.len      = 1;

  for (trials_count = 0; trials_count < MAX_REGISTER_READ_TRIALS; ++trials_count) 
  {
    if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS) 
    {
    MSG_FATAL("DB8V63A_i2c_read_byte OK! offset=0x%x,data=0x%x",0,data,0);
      return TRUE;
    }
  } /* end of for loop. */

  return FALSE;

}

/*===========================================================================

FUNCTION      camsensor_DB8V63A_ycbcr_i2c_write_byte

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
boolean camsensor_DB8V63A_ycbcr_i2c_write_byte (uint8 reg, uint8 data) 
{
	static uint8 writebyte;
	uint8 i;

	writebyte  = data;

	camsensor_i2c_command.addr.reg = reg;
	camsensor_i2c_command.buf_ptr  = (byte *)(&writebyte);
	camsensor_i2c_command.len	   = 1;

	for (i = 0; i < 3; ++i)
	{
		if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
		{
			//ERR("camsensor_SIV121A_ycbcr_i2c_read_byte: OK %x,%x",reg,data,0);
			MSG_FATAL("DB8V63A_i2c_write_byte OK! offset=0x%x,data=0x%x",0,data,0);
			return TRUE;
		}
	}
	
	return FALSE;

} /* camsensor_DB8V63A_ycbcr_i2c_write_byte */

/* The following are stub functions, (unsupported sensor functions) */
camera_ret_code_type camsensor_DB8V63A_ycbcr_write_exposure_gain(uint16 gain, uint32 line_count)
{
  return(CAMERA_NOT_SUPPORTED);
} /* camsensor_DB8V63A_ycbcr_write_exposure_gain */

camera_ret_code_type camsensor_DB8V63A_ycbcr_move_focus(camsensor_move_focus_enum_type direction, int32 num_steps)
{
  return(CAMERA_NOT_SUPPORTED);
} /* camsensor_DB8V63A_ycbcr_move_focus */

//wlr 1114 start
camera_ret_code_type camsensor_DB8V63A_set_effect(int8 effect) 
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;

	MSG_HIGH ("+++++ camsensor_DB8V63A_set_effect effect = %d",effect,0,0);
			camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF,0x61);
	switch(effect)
	{
		case CAMERA_EFFECT_OFF://normal
			MSG_HIGH("yujun enter here CAMERA_EFFECT_OFF",0,0,0);
	
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC6, 0x00)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC7, 0x00)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC8, 0x00)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC9, 0x00)) {return FALSE;}
		
			ret_val = CAMERA_SUCCESS;
			break;
		case CAMERA_EFFECT_MONO:
			MSG_HIGH("yujun enter here CAMERA_EFFECT_MONO",0,0,0);
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC6, 0x08)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC7, 0x00)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC8, 0x00)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC9, 0x00)) {return FALSE;}
			ret_val = CAMERA_SUCCESS;
			break;
		case CAMERA_EFFECT_NEGATIVE://负片
			MSG_HIGH("yujun enter here CAMERA_EFFECT_NEGATIVE",0,0,0);
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC6, 0x20)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC7, 0x00)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC8, 0x00)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC9, 0x00)) {return FALSE;}
			ret_val = CAMERA_SUCCESS;
			break;
		case CAMERA_EFFECT_SEPIA://棕褐色
			MSG_HIGH("yujun enter here CAMERA_EFFECT_SEPIA",0,0,0);
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC6, 0x18)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC7, 0x00)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC8, 0x61)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xC9, 0x9A)) {return FALSE;}
			ret_val = CAMERA_SUCCESS;
			break;

		default:
			ret_val = CAMERA_INVALID_PARM;
			break;
	}

  return ret_val;
}/* camsensor_DB8V63A_set_effect */

camera_ret_code_type camsensor_DB8V63A_set_wb(int8 wb) 
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;
	uint16 iRGain = 0x80, iGGain = 0x80, iBGain = 0x80;
	MSG_HIGH ("+++++ camsensor_DB8V63A_set_wb wb = %d",wb,0,0);
			
  switch (wb)
  {
		case  CAMERA_WB_AUTO://自动
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xff, 0x61)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x8C, 0x2D)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x62)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9A, 0x05)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9B, 0x80)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9E, 0x05)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9F, 0x80)) {return FALSE;}
	  ret_val = CAMERA_SUCCESS;
	  break;

		case CAMERA_WB_CLOUDY_DAYLIGHT://多云
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xff, 0x61)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x8C, 0x25)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x62)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9A, 0x04)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9B, 0x73)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9E, 0x06)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9F, 0x06)) {return FALSE;}
	  ret_val = CAMERA_SUCCESS;
	  break;
	
		case CAMERA_WB_INCANDESCENT://白炽灯
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xff, 0x61)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x8C, 0x25)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x62)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9A, 0x04)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9B, 0xB2)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9E, 0x06)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9F, 0x7B)) {return FALSE;}
			ret_val = CAMERA_SUCCESS;
	  break;

		case CAMERA_WB_FLUORESCENT: //荧光
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xff, 0x61)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x8C, 0x25)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x62)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9A, 0x05)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9B, 0x90)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9E, 0x06)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9F, 0x52)) {return FALSE;}
			ret_val = CAMERA_SUCCESS;
	  break;	
	  
		case CAMERA_WB_DAYLIGHT:  //日光
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xff, 0x61)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x8C, 0x25)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x62)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9A, 0x04)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9B, 0xF8)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9E, 0x05)) {return FALSE;}
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x9F, 0xA0)) {return FALSE;}
			ret_val = CAMERA_SUCCESS;
			break;

	default:
	  ret_val = CAMERA_INVALID_PARM;
	  break;
  }
	
  return ret_val;
}/* camsensor_DB8V63A_set_wb */
//wlr 1114 end

/*===========================================================================

FUNCTION	  CAMSENSOR_DB8V63A_YCBCR_2MP_SET_BRIGHTNESS

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
camera_ret_code_type camsensor_DB8V63A_set_brightness (int8 value)
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;
	uint8  temp = 0;

	MSG_HIGH ("camsensor_DB8V63A_set_brightness value = %d",value,0,0);
			camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF,0x61);
	switch(value)
	{
		case 0:
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xB7,0xC0)) {return FALSE;}//-4
			ret_val = CAMERA_SUCCESS;

			break;
		case 1:
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xB7,0xD0)) {return FALSE;}//-3
			ret_val = CAMERA_SUCCESS;

			break;
		case 2:
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xB7,0xE0)) {return FALSE;}//-2
			ret_val = CAMERA_SUCCESS;

			break;
		case 3:
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xB7,0xF0)) {return FALSE;}//-1
			ret_val = CAMERA_SUCCESS;

			break;
		case 4:
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xB7,0x00)) {return FALSE;}//0
			ret_val = CAMERA_SUCCESS;

			break;
		case 5:
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xB7,0x10)) {return FALSE;}//+1
			ret_val = CAMERA_SUCCESS;

			break;
		case 6:
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xB7,0x20)) {return FALSE;}//+2
			ret_val = CAMERA_SUCCESS;
			break;
		case 7:
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xB7,0x30)) {return FALSE;}//+3
			ret_val = CAMERA_SUCCESS;	
			break;
		case 8:
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xB7,0x40)) {return FALSE;}//+4
			ret_val = CAMERA_SUCCESS;
			break;
		default:
			ret_val = CAMERA_INVALID_PARM;
			break;
	}
  return ret_val;
} /* camsensor_ov2640_set_brightness */

/*===========================================================================

FUNCTION	  CAMSENSOR_DB8V63A_YCBCR_2MP_SET_CONTRAST

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
camera_ret_code_type camsensor_DB8V63A_set_contrast (int8 contrast)
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;
  /*bug6701/begin/zwp/2009.05.20/实现深安星和的AT命令*/
  MSG_HIGH("camsensor_DB8V63A_set_contrast contrast = %d", contrast, 0, 0);

  if ((contrast > DB8V63A_MAX_CONTRAST) || 
	  (contrast < DB8V63A_MIN_CONTRAST))
  {
	return CAMERA_FAILED;
  }
  
  /*bug6701/end/zwp/2009.05.20/实现深安星和的AT命令*/
  return ret_val;
} /* camsensor_ov2640_set_contrast */

// added Longcheer3G_liudan_20070411............................................
/*******************************************************************************/
//	Function:		camsensor_DB8V63A_set_antibanding
//	Description:	设置抗条纹干扰闪烁频率
//	Dependencies:	None
//	Input:			50Hz or 60Hz
//	Author: 		刘丹 2007.04.11
//	Return: 		CAMERA_SUCCESS ----> 设置成功
//					CAMERA_FAILED  ----> 设置失败
//	Note:			由OEMCamera层映射到camsensor_function_table_ptr虚表来调用
/*******************************************************************************/
camera_ret_code_type camsensor_DB8V63A_set_antibanding (int8 antibanding)
{
  	camera_ret_code_type ret_val = CAMERA_SUCCESS;
  
	MSG_HIGH("camsensor_DB8V63A_set_antibanding: antibanding %d", antibanding,0,0);
	#if 1//yty ls.yanfb open 2010.01.11 
  	switch (antibanding)
  	{
	case CAMERA_ANTIBANDING_50HZ:		// 50赫兹			
		{
			if (!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x61)) { return CAMERA_FAILED; }
			if (!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xE5, 0x19)) { return CAMERA_FAILED; }
			
			MSG_HIGH("camsensor_DB8V63A_set_antibanding: CAMERA_ANTIBANDING_50HZ",0,0,0);
		}
		break;																					   
																																																   
	case CAMERA_ANTIBANDING_60HZ:		// 60赫兹
		{
			if (!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x61)) { return CAMERA_FAILED; }
			if (!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xE5, 0x1D)) { return CAMERA_FAILED; }
		
			
			MSG_HIGH("camsensor_DB8V63A_set_antibanding: CAMERA_ANTIBANDING_60HZ",0,0,0);
		}
		break;

	default:
		{
			ret_val = CAMERA_INVALID_PARM;
		}
		break;
   	}
	#endif 
  	return ret_val;
}

/*******************************************************************************/
//	Function:		camsensor_DB8V63A_set_nightmode
//	Description:	设置夜光模式
//	Dependencies:	None
//	Input:			ON or OFF
//	Author: 		刘丹 2007.04.26
//	Return: 		CAMERA_SUCCESS ----> 设置成功
//					CAMERA_FAILED  ----> 设置失败
//	Note:			由OEMCamera层映射到camsensor_function_table_ptr虚表来调用
/*******************************************************************************/
camera_ret_code_type camsensor_DB8V63A_set_nightmode(int8 nightmode) 
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;
       uint8  itemp ;
	MSG_HIGH ("camsensor_DB8V63A_set_nightmode nightmode = %d",nightmode,0,0);
	
	switch (nightmode)
	{
	case CAMERA_NIGHTSHOT_MODE_ON:		// 开启 
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x62)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3B, 0x01)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3C, 0x91)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3D, 0x1E)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3E, 0x0C)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3F, 0x08)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x40, 0x19)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x41, 0x0A)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x42, 0x06)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x67, 0x01)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x68, 0xF4)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x69, 0x01)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x6A, 0xF4)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x61)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xE5, 0x19)) {return FALSE;};
		
		break;																					   
																																																   
	case CAMERA_NIGHTSHOT_MODE_OFF: 	// 关闭
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x62)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3B, 0x03)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3C, 0x21)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3D, 0x0F)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3E, 0x0D)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x3F, 0x08)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x40, 0x0C)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x41, 0x0A)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x42, 0x06)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x67, 0x01)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x68, 0xF4)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x69, 0x01)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0x6A, 0xF4)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xFF, 0x61)) {return FALSE;};
			if(!camsensor_DB8V63A_ycbcr_i2c_write_byte(0xE5, 0x19)) {return FALSE;};
		break;

	default:
			ret_val = CAMERA_INVALID_PARM;
		break;
	}
  return ret_val;
}
// added end....................................................................

/*===========================================================================

FUNCTION   CAMSENSOR_DB8V63A_YCBCR_SET_DEFAULT_FOCUS 

DESCRIPTION  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camsensor_DB8V63A_ycbcr_set_default_focus(void)
{
  return CAMERA_NOT_SUPPORTED;  /* Not supported on this sensor */
} /* camsensor_DB8V63A_ycbcr_set_default_focus */


///< ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///lee       test
///
LOCAL float camsensor_DB8V63A_register_to_real_gain(uint16 reg_gain)
{
  float gain;
  uint16 multiply_factor; 
#if 0
  if (reg_gain > 0x3FF)
  {
     reg_gain = 0x3FF;
  }

  multiply_factor = (((reg_gain >> 4) & 1) + 1) *
                    (((reg_gain >> 5) & 1) + 1) *
                    (((reg_gain >> 6) & 1) + 1) *
                    (((reg_gain >> 7) & 1) + 1) *
                    (((reg_gain >> 8) & 1) + 1) *
                    (((reg_gain >> 9) & 1) + 1);

  /*lint -save -e736 intended conversion from int to float */
  gain = (1.0 + (float)((reg_gain % 16) / 16.0)) * multiply_factor;
  /*lint -restore */
#endif
  return gain;
} /* camsensor_ov9650_register_to_real_gain */


LOCAL uint16 camsensor_DB8V63A_real_to_register_gain(float gain)
{
  uint16 reg_gain;
  uint16  multiply_factor;

  return reg_gain;
} /* camsensor_ov9650_real_to_register_gain */


LOCAL camera_ret_code_type camsensor_DB8V63A_set_frame_rate(uint16 fps)
{

  return CAMERA_SUCCESS;
} /* camsensor_ov9650_set_frame_rate */

///> ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*===========================================================================

FUNCTION   CAMSENSOR_DB8V63A_YCBCR_REGISTER 

DESCRIPTION  
  Exports the sensor specific functions to CAMSENSOR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_DB8V63A_ycbcr_register (camsensor_function_table_type *camsensor_function_table_ptr)
{
  camsensor_function_table_ptr->camsensor_start               	= camsensor_DB8V63A_ycbcr_start;
  camsensor_function_table_ptr->camsensor_video_config        	= camsensor_DB8V63A_ycbcr_video_config;
  camsensor_function_table_ptr->camsensor_raw_snapshot_config 	= camsensor_DB8V63A_ycbcr_raw_snapshot_config;
  camsensor_function_table_ptr->camsensor_snapshot_config     	= camsensor_DB8V63A_ycbcr_snapshot_config;
  camsensor_function_table_ptr->camsensor_power_up            	= camsensor_DB8V63A_ycbcr_power_up;
  camsensor_function_table_ptr->camsensor_power_down          	= camsensor_DB8V63A_ycbcr_power_down;
  camsensor_function_table_ptr->camsensor_write_exposure_gain 	= camsensor_DB8V63A_ycbcr_write_exposure_gain;
  camsensor_function_table_ptr->camsensor_set_default_focus   	= camsensor_DB8V63A_ycbcr_set_default_focus;
  camsensor_function_table_ptr->camsensor_move_focus          	= camsensor_DB8V63A_ycbcr_move_focus;
  camsensor_function_table_ptr->camsensor_register_to_real_gain = camsensor_DB8V63A_register_to_real_gain;///lee test
  camsensor_function_table_ptr->camsensor_real_to_register_gain = camsensor_DB8V63A_real_to_register_gain;///lee test
  camsensor_function_table_ptr->camsensor_set_frame_rate        = camsensor_DB8V63A_set_frame_rate;///lee test
  camsensor_function_table_ptr->camsensor_set_effect			= camsensor_DB8V63A_set_effect; //wlr1114	 
  camsensor_function_table_ptr->camsensor_set_wb				= camsensor_DB8V63A_set_wb;//wlr 1114
  camsensor_function_table_ptr->camsensor_set_brightness 		= camsensor_DB8V63A_set_brightness; //wlr1114	 
  camsensor_function_table_ptr->camsensor_set_contrast 			= camsensor_DB8V63A_set_contrast;//wlr 1114
/*bug7427/begin/zwp/20090925/添加设置reflect的接口*/
#ifdef LS_FEATURE_SETREFLECT_BY_DRRIVER
  camsensor_function_table_ptr->camsensor_set_reflect           = camsensor_DB8V63A_set_reflect;
#endif
/*bug7427/begin/zwp/20090925/添加设置reflect的接口*/
  
// added Longcheer3G_liudan_20070426............................................
  camsensor_function_table_ptr->camsensor_set_antibanding 		= camsensor_DB8V63A_set_antibanding; 
//yty//  camsensor_function_table_ptr->camsensor_set_nightmode		  	= camsensor_DB8V63A_set_nightmode;
// added end....................................................................

} /* camsensor_DB8V63A_ycbcr_register */
#endif /* FEATURE_CAMERA */


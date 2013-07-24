/*============================================================================

                 Camera Interface Device Driver Source File for 
                    SIV121A VGA CMOS sensor
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
#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_SIV121A)
#include "camerai.h"

#include "camsensor_SIV121A_yuv.h"
#include "clk.h"

#ifdef LOCAL
#undef LOCAL
#endif
#define LOCAL static

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

#define CAMSENSOR_SIV121A_I2C_SLAVE_ID              0x66 ///SIV121A
#ifndef FEATURE_VERSION_K212
#define CAMSENSOR_SIV121A_RESET_PIN        GPIO_OUTPUT_10
#endif
/* 
 * Maximum number of trials before aborting a register write operation
 *
 */
#define MAX_REGISTER_WRITE_TRIALS   3
#define MAX_REGISTER_READ_TRIALS    3
/*begin\bug6192\yangtingyu\20090226\camera驱动更新，实现亮度调节*/
#define SIV121A_MAX_BRIGHTNESS  9
#define SIV121A_MIN_BRIGHTNESS  0
/*end\bug6192\yangtingyu\20090226\camera驱动更新，实现亮度调节*/

#define SIV121A_MAX_CONTRAST   4
#define SIV121A_MIN_CONTRAST   0

/* From the logic analyzer measurements */
#define OV_SIV121A_YCBCR_FULL_SIZE_WIDTH           640//640
#define OV_SIV121A_YCBCR_FULL_SIZE_HEIGHT          480

#ifdef FEATURE_PROJECT_W455
#define OV_SIV121A_YCBCR_QTR_SIZE_WIDTH   		240//640   
#define OV_SIV121A_YCBCR_QTR_SIZE_HEIGHT  		320//240//   
#else
#define OV_SIV121A_YCBCR_QTR_SIZE_WIDTH   		640//640   
#define OV_SIV121A_YCBCR_QTR_SIZE_HEIGHT  		480//240//   
#endif
/* Strobe Flash Epoch Interrupt time before the end of line count */
#define OV_SIV121A_YCBCR_EPOCH_LINES_DELAY       1

#define PV_MODE_HBLANK_50HZ_NORMAL 		(0x1C3)
#define PV_MODE_VBLANK_50HZ_NORMAL 		(0x4D)
#define PV_MODE_STSTN_50HZ_NORMAL  		(0x60)
#define PV_MODE_FRCNT_50HZ_NORMAL  		(0x08)
#define PV_MODE_FRCNT_50HZ_NIGHT   		(0x21)

#define PV_MODE_HBLANK_60HZ_NORMAL 		(0x1C3)
#define PV_MODE_VBLANK_60HZ_NORMAL 		(0x3D)
#define PV_MODE_STSTN_60HZ_NORMAL  		(0x50)
#define PV_MODE_FRCNT_60HZ_NORMAL  		(0x0A) 
#define PV_MODE_FRCNT_60HZ_NIGHT   		(0x28)

#define VIDEO_MODE_HBLANK_50HZ_NORMAL 	(0x1C3)
#define VIDEO_MODE_VBLANK_50HZ_NORMAL 	(0x4D)
#define VIDEO_MODE_STSTN_50HZ_NORMAL  	(0x30)
#define VIDEO_MODE_FRCNT_50HZ_NORMAL  	(0x0C)
#define VIDEO_MODE_FRCNT_50HZ_NIGHT   	(0x10)

#define VIDEO_MODE_HBLANK_60HZ_NORMAL 	(0x1C3)
#define VIDEO_MODE_VBLANK_60HZ_NORMAL 	(0x3D)
#define VIDEO_MODE_STSTN_60HZ_NORMAL  	(0x28)
#define VIDEO_MODE_FRCNT_60HZ_NORMAL  	(0x0D) 
#define VIDEO_MODE_FRCNT_60HZ_NIGHT   	(0x10)

#define VIDEO_CAPTURE_CUT_LINE			(0x3)
static camera_antibanding_type g_iBanding = CAMERA_ANTIBANDING_OFF;
static camera_reflect_type g_reflect = CAMERA_MAX_REFLECT;
static int8 g_effect = CAMERA_EFFECT_MIN_MINUS_1;

/*===========================================================================
                          MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                        STATIC VARIABLES 
============================================================================*/
// modified Longcheer3G_liudan_20070403.........................................
const char camsensor_SIV121A_ycbcr_sensor_name[]  = "SIV121A VGA";

// modified end.................................................................
/*============================================================================
                          INTERNAL API PROTOTYPES
============================================================================*/
static boolean camsensor_SIV121A_ycbcr_i2c_write_byte(uint8 reg, uint8 data);
static void camsensor_SIV121A_ycbcr_register(camsensor_function_table_type *camsensor_function_table_ptr);
static boolean camsensor_SIV121A_ycbcr_i2c_read_byte(uint8 reg, uint8 *data); 
static  camera_nightshot_mode_type night_mode = CAMERA_MAX_NIGHTSHOT_MODE;

static void SIV121A_config_window(uint16 startx,uint16 starty,uint16 width, uint16 height);


static void SIV121A_config_window(uint16 startx,uint16 starty,uint16 width, uint16 height)
{
	camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x04);//page 0

	camsensor_SIV121A_ycbcr_i2c_write_byte(0xc0,(byte)((startx>>8)<<6)|((width>>8)<<4)|((starty>>8)<<3)|((height>>8)<<2));//10 40 F0 QVGA WINDOW 00 A0 80 160*128
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xc1,(byte)startx);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xc2,(byte)width);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xc3,(byte)starty);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xc4,(byte)height);
}

/*============================================================================
                          EXTERNAL API DEFINITIONS
============================================================================*/
LOCAL boolean camsensor_SIV121A_sensor_init(void)
{
	MSG_FATAL("-----camsensor_SIV121A_sensor_init-----enter",0,0,0);
		
  // SNR Block [Vendor recommended value ##Don't change##]
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x03,0xf4); //0x04
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x10,0x83); //LDO on
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x13,0x17);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x15,0x22); 
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x01);
	camsensor_SIV121A_ycbcr_i2c_write_byte(0x04,0x00); //switch

    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x05,0x02); 
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x11,0x25);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x12,0x21); 
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x13,0x17);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x15,0x22); 
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x20,0x00); 
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x21,0x03);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x22,0x01);
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x02);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x10,0x00);
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x11,0x14); 
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x12,0x88); 
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x13,0x88);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x14,0x88);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1e,0x70);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x34,0x96);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x40,0x50);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x41,0x20);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x42,0x20);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x43,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x44,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x45,0x00); 
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x46,0x0A);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x47,0x10);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x48,0x13);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x49,0x15);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4A,0x18);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4B,0x1A);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4C,0x1D);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4D,0x20);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4E,0x10);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4F,0x0A);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x50,0x08);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x51,0x06);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x52,0x05);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x53,0x04);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x54,0x02);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x55,0x01);
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x03);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x10,0xD0);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x11,0xC0);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x12,0x80);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x13,0x80);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x14,0x80);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x15,0xFE); 
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x16,0x87);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x17,0xc8);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x18,0x70);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x19,0x94);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1A,0x6c);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1B,0x94);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1C,0x6c);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1D,0x94);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1E,0x6c);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x20,0xE8);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x21,0x30);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x22,0xA4);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x23,0x20);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x25,0x20);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x26,0x0F);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x27,0x10);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x28,0x20);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x29,0xa9);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x2A,0x8c);
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x30,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x31,0x10);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x32,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x33,0x10);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x34,0x02);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x35,0x76);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x36,0x01);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x37,0xD6);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x40,0x01);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x41,0x04);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x42,0x08);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x43,0x10);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x44,0x12);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x45,0x35);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x46,0x64);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x50,0x33);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x51,0x20);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x52,0xE5);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x53,0xFB);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x54,0x13);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x55,0x26);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x56,0x07);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x57,0xF5);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x58,0xEA);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x59,0x21);
    
    //CMA change  -D65~A
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x63,0x98); //D30 to D20 for R
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x64,0xa8); //D30 to D20 for B
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x65,0x98); //D20 to D30 for R
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x66,0xa8); //D20 to D30 for B
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x67,0xc8); //D65 to D30 for R
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x68,0x9f); //D65 to D30 for B
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x69,0xc8); //D30 to D65 for R
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x6A,0x9f); //D30 to D65 for B
    
    // IDP
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x04);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x10,0xFF); // IDP function enable
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x11,0x09); //0x1D // PCLK polarity
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x12,0x9D); // Y,Cb,Cr order sequence
    // DPCNR
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x17,0x98);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x18,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x19,0x40);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1A,0x40);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1B,0x12);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1C,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1D,0x4f);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1E,0x4f);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x1F,0x2F);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x20,0x04); // Normal illumiinfo start
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x21,0x0F); // Dark illumiinfo start
    
    // Gamma
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x30,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x31,0x04);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x32,0x0b);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x33,0x24);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x34,0x49);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x35,0x66);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x36,0x7c);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x37,0x8d);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x38,0x9b);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x39,0xaa);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x3A,0xb6);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x3B,0xca);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x3C,0xdc);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x3D,0xef);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x3E,0xF8);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x3F,0xFF);
    
    // Shading
    #if 0
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x40,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x41,0x21);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x42,0x32);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x43,0x43);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x44,0x55);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x45,0x55);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x46,0x11); //R left # right
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x47,0x23); //R top # bottom
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x48,0x10); //Gr left # right
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x49,0x12); //Gr top # bottom
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4A,0x01); //Gb left # right
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4B,0x12); //Gb top # bottom
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4C,0x00); //B left # right
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4D,0x11); //Br top # bottom
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4E,0x04);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4F,0x50);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x50,0xF6);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x51,0x80);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x52,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x53,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x54,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x55,0x00);
    #else
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x40,0x0a);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x41,0xba);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x42,0x32);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x43,0x32);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x44,0x21);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x45,0x00);                                                  
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x46,0x24);	// left R gain[7:4], right R gain[3:0] 0x22        
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x47,0x22);	// top R gain[7:4], bottom R gain[3:0]             
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x48,0x23);	// left Gr gain[7:4], right Gr gain[3:0]           
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x49,0x50);	// top Gr gain[7:4], bottom Gr gain[3:0]           
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4a,0x14);	// left Gb gain[7:4], right Gb gain[3:0]           
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4b,0x12);	// top Gb gain[7:4], bottom Gb gain[3:0]           
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4c,0x03);	// left B gain[7:4], right B gain[3:0]             
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4d,0x21);	// top B gain[7:4], bottom B gain[3:0]             
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4e,0x04);	// X-axis center high[3:2], Y-axis center high[1:0]
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x4f,0x46);	// X-axis center low[7:0]                          
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x50,0xf6);	// Y-axis center low[7:0]                          
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x51,0x84);	// Shading Center Gain                             
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x52,0x00);	// Shading R Offset                                
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x53,0x00);	// Shading Gr Offset                               
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x54,0x00);	// Shading Gb Offset                               
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x55,0x00);	// Shading B Offset  
    #endif
    
    // Lowlux Shading
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x56,0x10);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x57,0x92);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x58,0x00);
    
    //Filter
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x61,0xB4);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x62,0xB4);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x63,0xB4);
    
    // Color Matrix (D65) - Daylight
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x71,0x42);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x72,0xbf);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x73,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x74,0x0F);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x75,0x31);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x76,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x77,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x78,0xbc);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x79,0x44);
    
    // Color Matrix (D30) - CWF
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x7A,0x39);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x7B,0xCd);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x7C,0xFa);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x7D,0x13);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x7E,0x2c);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x7F,0x02);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x80,0xF9);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x81,0xC6);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x82,0x41);
    
    // Color Matrix (D20) - A
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x83,0x38);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x84,0xbe);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x85,0x0a);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x86,0x13);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x87,0x1e);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x88,0x0F);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x89,0xF3);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x8A,0xcc);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x8B,0x41);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x8c,0x10);
                          
    // Edge - Green
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x90,0x14); //Up Gain
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x91,0x14); //Down Gain
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x92,0x02);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x96,0x02);
    
    // Edge - Luminance
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x9F,0x10);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xA0,0x10);
    
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xA9,0x10);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xAA,0x12);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xAB,0x82);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xAe,0x40);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xAf,0x86);
    
    // Color suppress
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xB9,0x10); //Ilimininfo Start
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xBA,0x30); //Slope
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xc0,0x24);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xc1,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xc2,0x58);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xc3,0x00);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xc4,0xe0);
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xDD,0x0F); // ENHCTRL
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xDE,0xfA); // NOIZCTRL
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xDF,0x1F);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xE0,0x90);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xE1,0x90);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xE2,0x08);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xE3,0x0c);
    
    // Memory speed control
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xE5,0x15);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xE6,0x28);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0xE7,0x04);
    
    // AE on
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x02);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x10,0x80);
    
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x70,0xd4);	//RBNCTRL
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x74,0x07);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x79,0x6f);
    
    // Sensor on
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x01);
    camsensor_SIV121A_ycbcr_i2c_write_byte(0x03,0x01);
    
	return  TRUE;
} /* camsensor_ov7690_write_sensor */


uint8 camsensor_SIV121A_ycbcr_active(void)
{
	return 0;
}

uint8 camsensor_SIV121A_ycbcr_unactive(void)
{
	return 1;
}

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_YCBCR_INIT

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

boolean camsensor_siv121a_init(camsensor_function_table_type *camsensor_function_table_ptr, camctrl_tbl_type *camctrl_tbl_ptr)
{
    uint8  manufacturer_id_msb;
    uint8  manufacturer_id_lsb;

	MSG_FATAL("camsensor_SIV121A_ycbcr_init()\n",0,0,0);///yty 


	camsensor_camclk_po_hz =24000000;
#ifndef FEATURE_VERSION_K212
	CAMERA_CONFIG_GPIO(CAMSENSOR_SIV121A_RESET_PIN);
    
    gpio_out(CAMSENSOR_SIV121A_RESET_PIN,1);
    clk_busy_wait(2*1000);
    gpio_out(CAMSENSOR_SIV121A_RESET_PIN,0);
    clk_busy_wait(100*1000);
    gpio_out(CAMSENSOR_SIV121A_RESET_PIN,1);
    clk_busy_wait(2*1000);
#endif
	camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;


	/* 1. Initiate I2C:  */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = CAMSENSOR_SIV121A_I2C_SLAVE_ID;
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 


	/* 3. Read sensor manufacturer ID MSB:       */
  
	camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x00);

	if (camsensor_SIV121A_ycbcr_i2c_read_byte(0x01, &manufacturer_id_msb) == FALSE) ///0x92
	{
		MSG_FATAL("camsensor_SIV121A_read REG0x1 fail %x ",manufacturer_id_msb,0,0);///yty 
		return FALSE;
	}

	if (camsensor_SIV121A_ycbcr_i2c_read_byte(0x02, &manufacturer_id_lsb) == FALSE) ///0x11
	{
		MSG_FATAL("camsensor_SIV121A_read REG0x2 fail %x ",manufacturer_id_lsb,0,0);///yty 
		return FALSE;
	}

	if ( (manufacturer_id_msb != 0x92) || (manufacturer_id_lsb != 0x11) )
	{
		MSG_FATAL("manufacturer_id_msb:0x%x,manufacturer_id_lsb:0x%x",manufacturer_id_msb,manufacturer_id_lsb,0);///yty 
		return FALSE;
	} 

	/* Initialize the sensor_function_table */
	camsensor_SIV121A_ycbcr_register (camsensor_function_table_ptr);

	return TRUE;
} /* camsensor_SIV121A_ycbcr_init */


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_YCBCR_START

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
boolean camsensor_SIV121A_ycbcr_start( camsensor_static_params_type *camsensor_params)
{

	MSG_FATAL("camsensor_SIV121A_ycbcr_start begin ", 0,0,0);///yty 

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
	camsensor_params->sensor_model = CAMSENSOR_SIV121A_ID;//CAMSENSOR_OMNI_VISION_7670;

	/* CCD or CMOS */
	camsensor_params->sensor_type = CAMSENSOR_CMOS;

	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;
	/* BAYER or YCbCr */
	camsensor_params->output_format = CAMSENSOR_YCBCR;

	/* A pointer to the sensor name for EXIF tags                */
	camsensor_params->sensor_name = camsensor_SIV121A_ycbcr_sensor_name;

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
	camsensor_params->full_size_width  = OV_SIV121A_YCBCR_FULL_SIZE_WIDTH;
	camsensor_params->full_size_height = OV_SIV121A_YCBCR_FULL_SIZE_HEIGHT;

	/* Preview dimensions */
	camsensor_params->qtr_size_width = OV_SIV121A_YCBCR_QTR_SIZE_WIDTH;
	camsensor_params->qtr_size_height = OV_SIV121A_YCBCR_QTR_SIZE_HEIGHT;

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

FUNCTION      CAMSENSOR_SIV121A_YCBCR_SNAPSHOT_CONFIG

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

boolean camsensor_SIV121A_ycbcr_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
#ifdef FEATURE_PROJECT_W455
	SIV121A_config_window(0,0,OV_SIV121A_YCBCR_FULL_SIZE_WIDTH,OV_SIV121A_YCBCR_FULL_SIZE_HEIGHT);
#endif
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
} /* camsensor_SIV121A_ycbcr_snapshot_config */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_YCBCR_RAW_SNAPSHOT_CONFIG

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

boolean camsensor_SIV121A_ycbcr_raw_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	return TRUE;
} /* camsensor_SIV121A_ycbcr_raw_snapshot_config */


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_YCBCR_VIDEO_CONFIG

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

boolean camsensor_SIV121A_ycbcr_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
)
{
	camsensor_SIV121A_sensor_init();

#ifdef FEATURE_PROJECT_W455
	SIV121A_config_window(0,0,OV_SIV121A_YCBCR_QTR_SIZE_WIDTH,OV_SIV121A_YCBCR_QTR_SIZE_HEIGHT);
#endif
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

	//	camsensor_SIV121A_ycbcr_write_sensor (camsensor_preview_resolution);//yty add
	camsensor_current_resolution = camsensor_preview_resolution;

	return TRUE;
} /* camsensor_SIV121A_ycbcr_video_config */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_YCBCR_POWER_UP

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_SIV121A_ycbcr_power_up(void)
{
}


/*===========================================================================

FUNCTION      camsensor_SIV121A_ycbcr_POWER_DOWN

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_SIV121A_ycbcr_power_down(void)
{
    MSG_FATAL ("camsensor_SIV121A_ycbcr_power_down begin", 0,0,0);
	g_reflect = CAMERA_MAX_REFLECT;
	g_effect = CAMERA_EFFECT_MIN_MINUS_1;
    // Sensor off
    if( camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x00) == FALSE)
    {
        MSG_FATAL("camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x00) return fail", 0, 0, 0);
    }
    if( camsensor_SIV121A_ycbcr_i2c_write_byte(0x03,0x02) == FALSE)
    {
        MSG_FATAL("camsensor_SIV121A_ycbcr_i2c_write_byte(0x03,0x02) return fail", 0, 0, 0);
    }
}


/*===========================================================================

FUNCTION      camsensor_SIV121A_ycbcr_i2c_read_byte

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
static boolean camsensor_SIV121A_ycbcr_i2c_read_byte(uint8 reg, uint8 *data) 
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
			MSG_FATAL("camsensor_SIV121A_ycbcr_i2c_read_byte: OK %x",reg,0,0);
			return TRUE;
		}
	}

	MSG_FATAL("camsensor_SIV121A_ycbcr_i2c_read_byte: false %x",reg,0,0);
	return FALSE;
}

/*===========================================================================

FUNCTION      camsensor_SIV121A_ycbcr_i2c_write_byte

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
static boolean camsensor_SIV121A_ycbcr_i2c_write_byte(uint8 reg, uint8 data) 
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
			MSG_FATAL("camsensor_SIV121A_ycbcr_i2c_read_byte: OK %x,%x",reg,data,0);
			return TRUE;
		}
	}
	MSG_FATAL("camsensor_SIV121A_ycbcr_i2c_read_byte: false %x,%x",reg,data,0);
	return FALSE;
} /* camsensor_SIV121A_ycbcr_i2c_write_byte */


/* The following are stub functions, (unsupported sensor functions) */
camera_ret_code_type camsensor_SIV121A_ycbcr_write_exposure_gain(uint16 gain, uint32 line_count)
{
	return(CAMERA_SUCCESS);
} /* camsensor_SIV121A_ycbcr_write_exposure_gain */

//wlr 1114 start
camera_ret_code_type camsensor_SIV121A_set_effect(int8 effect) 
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;

	MSG_FATAL ("+++++ camsensor_siv121a_set_effect effect = %d",effect,0,0);

	if ( g_effect == effect )
	{
		return CAMERA_SUCCESS;
	}
	
	camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x04);
	switch(effect)
	{
		case CAMERA_EFFECT_OFF://normal
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xB6, 0x00);
			break;
			
		case CAMERA_EFFECT_MONO:
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xB6, 0x40);
			break;
			
		case CAMERA_EFFECT_NEGATIVE://负片
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xB6, 0x10);
			break;
			
		case CAMERA_EFFECT_SEPIA://棕褐色
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xB6, 0x80);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xB7, 0x60);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xB8, 0xA0);
			break;

		default:
			ret_val = CAMERA_INVALID_PARM;
			break;
	}

	g_effect = effect;
  	return ret_val;
}/* camsensor_SIV121A_set_effect */

camera_ret_code_type camsensor_SIV121A_set_wb(int8 wb) 
{
  	camera_ret_code_type ret_val = CAMERA_SUCCESS;
	static int8 m_wb = CAMERA_WB_MIN_MINUS_1;

	if ( wb == m_wb )
	{
		return CAMERA_SUCCESS;
	}
	
	MSG_FATAL ("+++++ camsensor_SIV121A_set_wb wb = %d",wb,0,0);
	camsensor_SIV121A_ycbcr_i2c_write_byte(0x00, 0x03);

  	switch (wb)
  	{
		case  CAMERA_WB_AUTO://自动
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x10, 0xD3);
	  		break;

		case CAMERA_WB_CLOUDY_DAYLIGHT://多云
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x10, 0x00);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x60, 0xC2);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x61, 0x9E);
	  		break;
	
		case CAMERA_WB_INCANDESCENT://白炽灯
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x10, 0x00);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x60, 0x98);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x61, 0xC8);
	  		break;

		case CAMERA_WB_FLUORESCENT: //荧光
		    camsensor_SIV121A_ycbcr_i2c_write_byte(0x10, 0x00);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x60, 0xAA);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x61, 0xBE);
			ret_val = CAMERA_SUCCESS;
	  		break;	
	  
		case CAMERA_WB_DAYLIGHT:  //日光
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x10, 0x00);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x60, 0x90);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x61, 0xC0);
			break;

		default:
	  		ret_val = CAMERA_INVALID_PARM;
	  		break;
  	}

	m_wb = wb;
  	return ret_val;
}/* camsensor_SIV121A_set_wb */
//wlr 1114 end

/*===========================================================================

FUNCTION	  CAMSENSOR_SIV121A_YCBCR_2MP_SET_BRIGHTNESS

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
camera_ret_code_type camsensor_SIV121A_set_brightness (int8 value)
{
  	return CAMERA_SUCCESS;
} /* camsensor_ov2640_set_brightness */

/*===========================================================================

FUNCTION	  CAMSENSOR_SIV121A_YCBCR_2MP_SET_CONTRAST

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
camera_ret_code_type camsensor_SIV121A_set_contrast (int8 contrast)
{
  	return CAMERA_SUCCESS;
} /* camsensor_ov2640_set_contrast */

camera_ret_code_type camsensor_SIV121A_set_nightmode (camera_nightshot_mode_type mode)
{
	if ( night_mode == mode )
	{
		return CAMERA_SUCCESS;
	}
	
	night_mode = mode;

	camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x02); //bank 2

    if (mode == CAMERA_NIGHTSHOT_MODE_ON)
    {  
    	/* Night Mode */
        if (g_iBanding == CAMERA_ANTIBANDING_50HZ)
        {
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x11, VIDEO_MODE_FRCNT_50HZ_NIGHT);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x00, 0x04);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xAB, 0x18);			
        }
        else
        {
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x11, VIDEO_MODE_FRCNT_60HZ_NIGHT);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x00, 0x04);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xAB, 0x18);
        }
    }
    else
    { 
    	/* Normal Mode */
        if (g_iBanding == CAMERA_ANTIBANDING_50HZ)
    	{
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x11, VIDEO_MODE_FRCNT_50HZ_NORMAL);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x00, 0x04);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xAB, 0x00);
    	}
   	 	else
    	{
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x11, VIDEO_MODE_FRCNT_60HZ_NORMAL);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x00, 0x04);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0xAB, 0x00);
    	}
    }
  	return CAMERA_SUCCESS;
}


LOCAL camera_ret_code_type camsensor_SIV121A_set_frame_rate(uint16 fps)
{
  	return CAMERA_SUCCESS;
} /* camsensor_ov9650_set_frame_rate */

camera_ret_code_type camsensor_SIV121A_set_antibanding(int8 antibanding)
{
	if ( g_iBanding == antibanding )
	{
		return CAMERA_SUCCESS;
	}

	g_iBanding = antibanding;
	
	switch ((camera_antibanding_type)antibanding)
	{
		case CAMERA_ANTIBANDING_OFF:
			break;

		case CAMERA_ANTIBANDING_60HZ:
			// all 50HZ blank register cannot be used
           	camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x01);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x20,0x10);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x21,0xC3);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x23,0x4D);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x02);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x34,0x30);
			break;

		case CAMERA_ANTIBANDING_50HZ:
			
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x01);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x20,0x10);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x21,0xC3);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x23,0x3D);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x00,0x02);
			camsensor_SIV121A_ycbcr_i2c_write_byte(0x34,0x28);
			break;

		default:
			return CAMERA_NOT_SUPPORTED;
	}
	
	return CAMERA_SUCCESS;
}
/*===========================================================================

FUNCTION   CAMSENSOR_SIV121A_YCBCR_REGISTER 

DESCRIPTION  
  Exports the sensor specific functions to CAMSENSOR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_SIV121A_ycbcr_register (camsensor_function_table_type *camsensor_function_table_ptr)
{
	camsensor_function_table_ptr->camsensor_start               	= camsensor_SIV121A_ycbcr_start;
	camsensor_function_table_ptr->camsensor_video_config        	= camsensor_SIV121A_ycbcr_video_config;
	camsensor_function_table_ptr->camsensor_raw_snapshot_config 	= camsensor_SIV121A_ycbcr_raw_snapshot_config;
	camsensor_function_table_ptr->camsensor_snapshot_config     	= camsensor_SIV121A_ycbcr_snapshot_config;
	camsensor_function_table_ptr->camsensor_power_up            	= camsensor_SIV121A_ycbcr_power_up;
	camsensor_function_table_ptr->camsensor_power_down          	= camsensor_SIV121A_ycbcr_power_down;
	camsensor_function_table_ptr->camsensor_write_exposure_gain 	= camsensor_SIV121A_ycbcr_write_exposure_gain;
	camsensor_function_table_ptr->camsensor_set_frame_rate        	= camsensor_SIV121A_set_frame_rate;///lee test
	camsensor_function_table_ptr->camsensor_set_effect				= camsensor_SIV121A_set_effect; //wlr1114	 
	camsensor_function_table_ptr->camsensor_set_wb					= camsensor_SIV121A_set_wb;//wlr 1114
	camsensor_function_table_ptr->camsensor_set_brightness 			= camsensor_SIV121A_set_brightness; //wlr1114	 
	camsensor_function_table_ptr->camsensor_set_contrast 			= camsensor_SIV121A_set_contrast;//wlr 1114
	camsensor_function_table_ptr->camsensor_set_antibanding 		= camsensor_SIV121A_set_antibanding; 
} /* camsensor_SIV121A_ycbcr_register */
#endif /* FEATURE_CAMERA */

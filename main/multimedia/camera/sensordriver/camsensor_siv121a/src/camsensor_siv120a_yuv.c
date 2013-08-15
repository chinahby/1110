/*============================================================================

                 Camera Interface Device Driver Source File for 
                    SIV120A VGA CMOS sensor
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

$Header: //depot/asic/msm6550/drivers/camsensor/camsensor_ SIV120A_ycbcr.c#3 $ $DateTime: 2006/08/11 14:02:49 $ $Author: klee $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
  09/30/06  lee    Initial release.
 
============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "camsensor.h"
#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_SIV120A)
#include "camerai.h"

#include "camsensor_SIV120A_yuv.h"
#include "clk.h"

#ifdef LOCAL
#undef LOCAL
#endif
#define LOCAL static

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

#define CAMSENSOR_SIV120A_I2C_SLAVE_ID              0x66 ///SIV120A
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_K212_12832

#define CAMSENSOR_SIV120A_RESET_PIN         GPIO_OUTPUT_10
#endif
#endif
/* From the logic analyzer measurements */
#ifdef	FEATURE_VERSION_X3
#define OV_SIV120A_YCBCR_FULL_SIZE_WIDTH           360//640
#define OV_SIV120A_YCBCR_FULL_SIZE_HEIGHT          480
#define OV_SIV120A_YCBCR_QTR_SIZE_WIDTH   		   360//640   
#define OV_SIV120A_YCBCR_QTR_SIZE_HEIGHT  		   480//240//   
#else
#define OV_SIV120A_YCBCR_FULL_SIZE_WIDTH           640//640
#define OV_SIV120A_YCBCR_FULL_SIZE_HEIGHT          480
#define OV_SIV120A_YCBCR_QTR_SIZE_WIDTH   		   640   
#define OV_SIV120A_YCBCR_QTR_SIZE_HEIGHT  		   480 //   
#endif
#define SIV120A_SENSOR_ID						(0x12)
#define SIV120A_SENSOR_VERSION					(0x10)
/*===========================================================================
                          MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                        STATIC VARIABLES 
============================================================================*/
// modified Longcheer3G_liudan_20070403.........................................
const char camsensor_SIV120A_ycbcr_sensor_name[]  = "SIV120A VGA";

// modified end.................................................................
/*============================================================================
                          INTERNAL API PROTOTYPES
============================================================================*/
static boolean camsensor_SIV120A_ycbcr_i2c_write_byte(uint8 reg, uint8 data);
static void camsensor_SIV120A_ycbcr_register(camsensor_function_table_type *camsensor_function_table_ptr);
static boolean camsensor_SIV120A_ycbcr_i2c_read_byte(uint8 reg, uint8 *data); 
static void SIV120A_config_window(uint16 startx,uint16 starty,uint16 width, uint16 height);


static void SIV120A_config_window(uint16 startx,uint16 starty,uint16 width, uint16 height)
{
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x00,0x03);

	camsensor_SIV120A_ycbcr_i2c_write_byte(0xa0,(byte)((startx>>8)<<6)|((width>>8)<<4)|((starty>>8)<<3)|((height>>8)<<2));
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xa1,(byte)startx);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xa2,(byte)width);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xa3,(byte)starty);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xa4,(byte)height);
}
/*============================================================================
                          EXTERNAL API DEFINITIONS
============================================================================*/
LOCAL boolean camsensor_SIV120A_sensor_init(void)
{
	MSG_FATAL("-----camsensor_SIV120A_sensor_init-----enter",0,0,0);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x00, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x04, 0x00);//0x07上下颠倒

	//Sensor Control Block
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x00, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x04, 0x01);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x10, 0x37);//2010/11/10
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x12, 0x11);	//BLC _0605
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x13, 0x00);	//0x1A_Analog //0x17, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x42, 0x32);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x43, 0x80);

	//SIV120A 24MHz Setting For 50Hz
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x20, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x21, 0xC9);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x23, 0x65);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x00, 0x01);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x34, 0x78);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x11, 0x10);

	//AE Block
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x00, 0x01);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x10, 0x84);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x12, 0x80);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x13, 0x78);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x40, 0x5F);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x41, 0x08);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x42, 0x3F);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x45, 0x20);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x46, 0x29);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x47, 0x30);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x48, 0x33);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x49, 0x36);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x4A, 0x39);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x4B, 0x3B);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x4C, 0x3D);

	//AWB Block
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x00, 0x02);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x10, 0xD3);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x11, 0xC0);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x13, 0x80);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x14, 0x80);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x15, 0xFE);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x16, 0x80);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x17, 0xEA);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x18, 0x80);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x19, 0xA0);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x1A, 0x70);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x1B, 0xA0);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x1C, 0x60);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x1D, 0xA0);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x1E, 0x70);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x20, 0xE8);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x21, 0x20);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x22, 0xA4);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x23, 0x20);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x25, 0x20);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x26, 0x0F);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x27, 0x0D);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x28, 0x90);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x29, 0xD0);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x2A, 0x90);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x30, 0x05);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x46, 0x64);

	//RGB to YCbCr (CSC) no used
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x50, 0x33);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x51, 0x20);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x52, 0xE5);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x53, 0xFB);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x54, 0x13);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x55, 0x26);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x56, 0x07);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x57, 0xF5);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x58, 0xEA);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x59, 0x21);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x63, 0x90);	


	//R For D30 to D20
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x64, 0xB8);	//G For D30 to D20    
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x65, 0x90); //R For D20 to D30
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x66, 0xB8); //G For D20 to D30
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x67, 0xC8); //R For D65 to D30
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x68, 0x9F); //G For D65 to D30
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x69, 0xC8); //R For D30 to D65
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x6A, 0x9F);	//G For D30 to D65//IDP   
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x00, 0x03);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x10, 0xEF);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x11, 0x1D);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x12, 0x1D);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x14, 0xF5);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x44, 0xB9);//Shading   
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x2C, 0x42); //R x-Axis
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x2D, 0x00); //R y-Axis
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x2E, 0x20);	//G x-Axis
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x2F, 0x00); //G y-Axis
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x30, 0x20);	//B x-Axis
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x31, 0x00); //B y-Axis
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x32, 0xA1);	//Center x-Axis
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x33, 0x81);	//Center y-Axis//Gamma
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x34, 0x00);                     
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x35, 0x0B);                   
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x36, 0x15);          
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x37, 0x29);         
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x38, 0x4A);         
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x39, 0x66);        
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x3A, 0x7A);        
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x3B, 0x8D);         
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x3C, 0x9D);        
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x3D, 0xAA);         
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x3E, 0xB6);        
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x3F, 0xCB);         
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x40, 0xDE);        
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x41, 0xF0);        
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x42, 0xF8);         
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x43, 0xFF); 
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x51, 0x40);		

	// Dark Start Analog Gain//CMA-D65
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x54, 0x39);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x55, 0xC3);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x56, 0x05);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x57, 0x14);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x58, 0x23);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x59, 0x09);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x5A, 0xF4);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x5B, 0xCD);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x5C, 0x41);

	//CMA-D30
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x5D, 0x46);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x5E, 0xBE);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x5F, 0xFD);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x60, 0x02);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x61, 0x29);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x62, 0x15);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x63, 0xFA);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x64, 0xBC);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x65, 0x4C);//CMA-D20
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x66, 0x3D);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x67, 0xC2);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x68, 0x02);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x69, 0xFD);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x6A, 0x3B);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x6B, 0x09);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x6C, 0xFA);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x6D, 0x9E);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x6E, 0x6A);        
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x6F, 0xC0);	//CMA D65, D30,D20//Edge
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x76, 0x40);	//Green Edge Suppress Start Analog Gain
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x7F, 0x40);	//Luminance Edge Suppress Start Analog Gain
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x85, 0x10);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x86, 0x12);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x87, 0x12);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x88, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x89, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x8C, 0xFF);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x8D, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x8E, 0xFF);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x8F, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x90, 0xFF);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x91, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x92, 0x44);	//Color Saturation Suppress Start Analog Gain//Windowing
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xA0, 0x24);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xA1, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xA2, 0x80);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xA3, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xA4, 0xE0);

	//Sensor On
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x00, 0x00);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x03, 0x05);
	camsensor_SIV120A_ycbcr_i2c_write_byte(0xff, 0xff);	
  	// SNR Block [Vendor recommended value ##Don't change##]
	return  TRUE;
} /* camsensor_ov7690_write_sensor */


uint8 camsensor_SIV120A_ycbcr_active(void)
{
	return 1;
}

uint8 camsensor_SIV120A_ycbcr_unactive(void)
{
	return 0;
}

/*===========================================================================

FUNCTION      CAMSENSOR_SIV120A_YCBCR_INIT

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

boolean camsensor_siv120a_init(camsensor_function_table_type *camsensor_function_table_ptr, camctrl_tbl_type *camctrl_tbl_ptr)
{
	uint8  product_id_msb=0;
	uint8  product_id_lsb=0;
	
	MSG_FATAL("camsensor_SIV120A_ycbcr_init()\n",0,0,0);///yty 


	camsensor_camclk_po_hz =24000000;
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_K212_12832

	CAMERA_CONFIG_GPIO(CAMSENSOR_SIV120A_RESET_PIN);
    
    gpio_out(CAMSENSOR_SIV120A_RESET_PIN,1);
    clk_busy_wait(2*1000);
    gpio_out(CAMSENSOR_SIV120A_RESET_PIN,0);
    clk_busy_wait(100*1000);
    gpio_out(CAMSENSOR_SIV120A_RESET_PIN,1);
    clk_busy_wait(2*1000);
#endif
#endif
	camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;


	/* 1. Initiate I2C:  */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = CAMSENSOR_SIV120A_I2C_SLAVE_ID;
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 


	/* 3. Read sensor manufacturer ID MSB:       */
	/* Initialize the sensor_function_table */
	camsensor_SIV120A_ycbcr_i2c_write_byte(0x00, 0x00);
	camsensor_SIV120A_ycbcr_i2c_read_byte(0x01, &product_id_msb);
	clk_busy_wait(10);
	MSG_FATAL ("camsensor_siv120a_ycbcr_init_product_id_msb=0x%x",product_id_msb,0,0);
	camsensor_SIV120A_ycbcr_i2c_read_byte(0x02, &product_id_lsb);
	clk_busy_wait(10);
	MSG_FATAL ("camsensor_siv120a_ycbcr_init_product_id_msb=0x%x",product_id_lsb,0,0);

	if ( product_id_msb != SIV120A_SENSOR_ID || product_id_lsb != SIV120A_SENSOR_VERSION )
	{
		return FALSE;
	}
	
	camsensor_SIV120A_ycbcr_register (camsensor_function_table_ptr);

	return TRUE;
} /* camsensor_SIV120A_ycbcr_init */


/*===========================================================================

FUNCTION      CAMSENSOR_SIV120A_YCBCR_START

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
boolean camsensor_SIV120A_ycbcr_start( camsensor_static_params_type *camsensor_params)
{

	MSG_FATAL("camsensor_SIV120A_ycbcr_start begin ", 0,0,0);///yty 

	/* Initialize CAMIF operation mode */
	camsensor_params->camif_config.SyncMode         = CAMIF_APS;
	camsensor_params->camif_config.HSyncEdge        = CAMIF_High;
	camsensor_params->camif_config.VSyncEdge        = CAMIF_Low;
	camsensor_params->camif_efs_config.EFS_EOL      = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOL      = 0x0000;
	camsensor_params->camif_efs_config.EFS_EOF      = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOF      = 0x0000;

	/* ------------------  Sensor-specific Config -------------- */
	/* Make/model of sensor */
	camsensor_params->sensor_model = CAMSENSOR_ID_SIV120A;//CAMSENSOR_OMNI_VISION_7670;

	/* CCD or CMOS */
	camsensor_params->sensor_type = CAMSENSOR_CMOS;

	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;
	/* BAYER or YCbCr */
	camsensor_params->output_format = CAMSENSOR_YCBCR;

	/* A pointer to the sensor name for EXIF tags                */
	camsensor_params->sensor_name = camsensor_SIV120A_ycbcr_sensor_name;

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
	camsensor_params->full_size_width  = OV_SIV120A_YCBCR_FULL_SIZE_WIDTH;
	camsensor_params->full_size_height = OV_SIV120A_YCBCR_FULL_SIZE_HEIGHT;

	/* Preview dimensions */
	camsensor_params->qtr_size_width = OV_SIV120A_YCBCR_QTR_SIZE_WIDTH;
	camsensor_params->qtr_size_height = OV_SIV120A_YCBCR_QTR_SIZE_HEIGHT;

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
} /* camsensor_SIV120A_ycbcr_start */


/*===========================================================================

FUNCTION      CAMSENSOR_SIV120A_YCBCR_SNAPSHOT_CONFIG

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

boolean camsensor_SIV120A_ycbcr_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	#ifdef	FEATURE_VERSION_X3
	SIV120A_config_window(140,0,OV_SIV120A_YCBCR_FULL_SIZE_WIDTH,OV_SIV120A_YCBCR_FULL_SIZE_HEIGHT);
	#endif
	/* Discard the first frame.*/
	camsensor_params->discardFirstFrame = FALSE;
	/* Sensor output data format */
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;

	/* Set the current dimensions */
	camsensor_params->camsensor_width = camsensor_params->full_size_width;
	camsensor_params->camsensor_height = camsensor_params->full_size_height;
	/* CAMIF frame */
	camsensor_params->camif_frame_config.pixelsPerLine = OV_SIV120A_YCBCR_FULL_SIZE_WIDTH*2;
	camsensor_params->camif_frame_config.linesPerFrame = OV_SIV120A_YCBCR_FULL_SIZE_HEIGHT;
	
	/* CAMIF window */
	camsensor_params->camif_window_width_config.firstPixel = 0;
	camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camsensor_width*2 - 1;
	camsensor_params->camif_window_height_config.firstLine = 0;
	camsensor_params->camif_window_height_config.lastLine = camsensor_params->camsensor_height - 1;

	clk_busy_wait(100*1000);
	camsensor_current_resolution = camsensor_snapshot_resolution;
	return TRUE;
} /* camsensor_SIV120A_ycbcr_snapshot_config */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV120A_YCBCR_RAW_SNAPSHOT_CONFIG

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

boolean camsensor_SIV120A_ycbcr_raw_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	return TRUE;
} /* camsensor_SIV120A_ycbcr_raw_snapshot_config */


/*===========================================================================

FUNCTION      CAMSENSOR_SIV120A_YCBCR_VIDEO_CONFIG

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

boolean camsensor_SIV120A_ycbcr_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
)
{
	camsensor_SIV120A_sensor_init();
#ifdef	FEATURE_VERSION_X3
	SIV120A_config_window(140,0,OV_SIV120A_YCBCR_QTR_SIZE_WIDTH,OV_SIV120A_YCBCR_QTR_SIZE_HEIGHT);
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

	camsensor_current_resolution = camsensor_preview_resolution;
	
	return TRUE;
} /* camsensor_SIV120A_ycbcr_video_config */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV120A_YCBCR_POWER_UP

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_SIV120A_ycbcr_power_up(void)
{
}


/*===========================================================================

FUNCTION      camsensor_SIV120A_ycbcr_POWER_DOWN

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_SIV120A_ycbcr_power_down(void)
{
    MSG_FATAL ("camsensor_SIV120A_ycbcr_power_down begin", 0,0,0);
}


/*===========================================================================

FUNCTION      camsensor_SIV120A_ycbcr_i2c_read_byte

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
static boolean camsensor_SIV120A_ycbcr_i2c_read_byte(uint8 reg, uint8 *data) 
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
			MSG_FATAL("camsensor_SIV120A_ycbcr_i2c_read_byte: OK %x",reg,0,0);
			return TRUE;
		}
	}

	MSG_FATAL("camsensor_SIV120A_ycbcr_i2c_read_byte: false %x",reg,0,0);
	return FALSE;
}

/*===========================================================================

FUNCTION      camsensor_SIV120A_ycbcr_i2c_write_byte

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
static boolean camsensor_SIV120A_ycbcr_i2c_write_byte(uint8 reg, uint8 data) 
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
			MSG_FATAL("camsensor_SIV120A_ycbcr_i2c_write_byte: OK %x,%x",reg,data,0);
			return TRUE;
		}
	}
	MSG_FATAL("camsensor_SIV120A_ycbcr_i2c_write_byte: false %x,%x",reg,data,0);
	return FALSE;
} /* camsensor_SIV120A_ycbcr_i2c_write_byte */


/* The following are stub functions, (unsupported sensor functions) */
camera_ret_code_type camsensor_SIV120A_ycbcr_write_exposure_gain(uint16 gain, uint32 line_count)
{
	return(CAMERA_SUCCESS);
} /* camsensor_SIV120A_ycbcr_write_exposure_gain */

//wlr 1114 start
camera_ret_code_type camsensor_SIV120A_set_effect(int8 effect) 
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;

	MSG_FATAL ("+++++ camsensor_siv120a_set_effect effect = %d",effect,0,0);
	
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
}/* camsensor_SIV120A_set_effect */

camera_ret_code_type camsensor_SIV120A_set_wb(int8 wb) 
{
  	camera_ret_code_type ret_val = CAMERA_SUCCESS;
	
	MSG_FATAL ("+++++ camsensor_SIV120A_set_wb wb = %d",wb,0,0);

  	switch (wb)
  	{
		case  CAMERA_WB_AUTO://自动
	  		break;

		case CAMERA_WB_CLOUDY_DAYLIGHT://多云
	  		break;
	
		case CAMERA_WB_INCANDESCENT://白炽灯
	  		break;

		case CAMERA_WB_FLUORESCENT: //荧光
			ret_val = CAMERA_SUCCESS;
	  		break;	
	  
		case CAMERA_WB_DAYLIGHT:  //日光
			break;

		default:
	  		ret_val = CAMERA_INVALID_PARM;
	  		break;
  	}

  	return ret_val;
}/* camsensor_SIV120A_set_wb */
//wlr 1114 end

/*===========================================================================

FUNCTION	  CAMSENSOR_SIV120A_YCBCR_2MP_SET_BRIGHTNESS

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
camera_ret_code_type camsensor_SIV120A_set_brightness (int8 value)
{
  	return CAMERA_SUCCESS;
} /* camsensor_ov2640_set_brightness */

/*===========================================================================

FUNCTION	  CAMSENSOR_SIV120A_YCBCR_2MP_SET_CONTRAST

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
camera_ret_code_type camsensor_SIV120A_set_contrast (int8 contrast)
{
  	return CAMERA_SUCCESS;
} /* camsensor_ov2640_set_contrast */

camera_ret_code_type camsensor_SIV120A_set_nightmode (camera_nightshot_mode_type mode)
{
  	return CAMERA_SUCCESS;
}


LOCAL camera_ret_code_type camsensor_SIV120A_set_frame_rate(uint16 fps)
{
  	return CAMERA_SUCCESS;
} /* camsensor_ov9650_set_frame_rate */

camera_ret_code_type camsensor_SIV120A_set_antibanding(int8 antibanding)
{
	return CAMERA_SUCCESS;
}
/*===========================================================================

FUNCTION   CAMSENSOR_SIV120A_YCBCR_REGISTER 

DESCRIPTION  
  Exports the sensor specific functions to CAMSENSOR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_SIV120A_ycbcr_register (camsensor_function_table_type *camsensor_function_table_ptr)
{
	camsensor_function_table_ptr->camsensor_start               	= camsensor_SIV120A_ycbcr_start;
	camsensor_function_table_ptr->camsensor_video_config        	= camsensor_SIV120A_ycbcr_video_config;
	camsensor_function_table_ptr->camsensor_raw_snapshot_config 	= camsensor_SIV120A_ycbcr_raw_snapshot_config;
	camsensor_function_table_ptr->camsensor_snapshot_config     	= camsensor_SIV120A_ycbcr_snapshot_config;
	camsensor_function_table_ptr->camsensor_power_up            	= camsensor_SIV120A_ycbcr_power_up;
	camsensor_function_table_ptr->camsensor_power_down          	= camsensor_SIV120A_ycbcr_power_down;
	camsensor_function_table_ptr->camsensor_write_exposure_gain 	= camsensor_SIV120A_ycbcr_write_exposure_gain;
	camsensor_function_table_ptr->camsensor_set_frame_rate        	= camsensor_SIV120A_set_frame_rate;///lee test
	camsensor_function_table_ptr->camsensor_set_effect				= camsensor_SIV120A_set_effect; //wlr1114	 
	camsensor_function_table_ptr->camsensor_set_wb					= camsensor_SIV120A_set_wb;//wlr 1114
	camsensor_function_table_ptr->camsensor_set_brightness 			= camsensor_SIV120A_set_brightness; //wlr1114	 
	camsensor_function_table_ptr->camsensor_set_contrast 			= camsensor_SIV120A_set_contrast;//wlr 1114
	camsensor_function_table_ptr->camsensor_set_antibanding 		= camsensor_SIV120A_set_antibanding; 
} /* camsensor_SIV120A_ycbcr_register */

#endif /* FEATURE_CAMERA */

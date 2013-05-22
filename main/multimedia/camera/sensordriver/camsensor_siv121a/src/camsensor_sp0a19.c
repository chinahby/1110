/*============================================================================

                 Camera Interface Device Driver Source File for 
                    SP0A18 VGA CMOS sensor
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

$Header: //depot/asic/msm6550/drivers/camsensor/camsensor_ SP0A18_ycbcr.c#3 $ $DateTime: 2006/08/11 14:02:49 $ $Author: klee $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
  09/30/06  lee    Initial release.
 
============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "camsensor.h"
#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_SP0A19)
#include "camerai.h"

#include "camsensor_sp0a19.h"
#include "clk.h"

#ifdef LOCAL
#undef LOCAL
#endif
#define LOCAL static

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

#define CAMSENSOR_SP0A19_I2C_SLAVE_ID              0x42 ///SP0A18

#define CAMSENSOR_SP0A19_RESET_PIN         GPIO_OUTPUT_10

/* 
 * Maximum number of trials before aborting a register write operation
 *
 */
#define MAX_REGISTER_WRITE_TRIALS   3
#define MAX_REGISTER_READ_TRIALS    3
/*begin\bug6192\yangtingyu\20090226\camera驱动更新，实现亮度调节*/
#define SP0A19_MAX_BRIGHTNESS  9
#define SP0A19_MIN_BRIGHTNESS  0
/*end\bug6192\yangtingyu\20090226\camera驱动更新，实现亮度调节*/

#define SP0A19_MAX_CONTRAST   4
#define SP0A19_MIN_CONTRAST   0

/* From the logic analyzer measurements */
#define OV_SP0A19_YCBCR_FULL_SIZE_WIDTH           360//640
#define OV_SP0A19_YCBCR_FULL_SIZE_HEIGHT          480

#define OV_SP0A19_YCBCR_QTR_SIZE_WIDTH   		360//640   
#define OV_SP0A19_YCBCR_QTR_SIZE_HEIGHT  		480//240//   

/* Strobe Flash Epoch Interrupt time before the end of line count */

static camera_antibanding_type g_iBanding = CAMERA_ANTIBANDING_OFF;

/*===========================================================================
                          MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                        STATIC VARIABLES 
============================================================================*/
// modified Longcheer3G_liudan_20070403.........................................
const char camsensor_sp0a19_ycbcr_sensor_name[]  = "SP0A19 VGA";

// modified end.................................................................
/*============================================================================
                          INTERNAL API PROTOTYPES
============================================================================*/
static boolean camsensor_SP0A19_ycbcr_i2c_write_byte(uint8 reg, uint8 data);
static void camsensor_sp0a19_ycbcr_register(camsensor_function_table_type *camsensor_function_table_ptr);
static boolean camsensor_SP0A19_ycbcr_i2c_read_byte(uint8 reg, uint8 *data); 
static void    camsensor_sp0a19_0m3_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
static void SP0A19_config_window(uint16 startx,uint16 starty,uint16 width, uint16 height);


static void SP0A19_config_window(uint16 startx,uint16 starty,uint16 width, uint16 height)
{
	camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);//page 0
	// Horizontal
	camsensor_SP0A19_ycbcr_i2c_write_byte(0x4b,(startx&0x0300)>>8);	//2msb
	camsensor_SP0A19_ycbcr_i2c_write_byte(0x4c,startx&0x00FF);			// 8lsb
	camsensor_SP0A19_ycbcr_i2c_write_byte(0x4d,(width&0x0300)>>8);			//2msb
	camsensor_SP0A19_ycbcr_i2c_write_byte(0x4e,width&0x00FF);			// 8lsb
	// Vertical
	camsensor_SP0A19_ycbcr_i2c_write_byte(0x47,(starty&0x0100)>>8);	//1msb
	camsensor_SP0A19_ycbcr_i2c_write_byte(0x48,starty&0x00FF);   			// 8lsb
	camsensor_SP0A19_ycbcr_i2c_write_byte(0x49,(height&0x0100)>>8);		   	// 1msb
	camsensor_SP0A19_ycbcr_i2c_write_byte(0x4a,height&0x00FF);		   	// 8lsb

}	/* config_SP0A18_window */
/*============================================================================
                          EXTERNAL API DEFINITIONS
============================================================================*/
LOCAL boolean camsensor_sp0a19_sensor_init(void)
{
		MSG_FATAL("-----camsensor_SP0A19_sensor_init-----enter",0,0,0);

      camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1C,0x28);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x0f,0x2f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x10,0x2e);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x11,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x12,0x18);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x13,0x2f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x14,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x15,0x3f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x16,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x17,0x18);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x25,0x40);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1a,0x0b);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1b,0xc );
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1e,0xb );
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x20,0x3f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x21,0x13);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x22,0x19);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x26,0x1a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x27,0xab);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x28,0xfd);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x30,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfb,0x33); //0x31 PZT 2013-5-22
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1f,0x08);     
  	//Blacklevel
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x65,0x00);//06
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x66,0x00);//06
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x67,0x00);//06
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x68,0x00);//06
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x45,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x46,0x0f);

#if 1	
         /*//ae setting  24M 50hz 20-8FPS maxgain:0x70 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x03,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x04,0x32);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x06,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x09,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x0a,0x46);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf0,0x66);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf1,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x90,0x0c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x92,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x98,0x66);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x99,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9a,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9b,0x00);
  	  
  	//Status
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xce,0xc8);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcf,0x04);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd0,0xc8);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd1,0x04);  */
  	  
  //ae setting  24M 50hz 18-6FPS maxgain:0x70 
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x03,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x04,0x14);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x06,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x09,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x0a,0xc6);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf0,0x5c);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf1,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x90,0x10);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x92,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x98,0x5c);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x99,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9a,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9b,0x00);
  //Status                                      
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xce,0xc0);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcf,0x05);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd0,0xc0);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd1,0x05);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
#else
//;ae setting      SI13_SP0A19 24M 1分频 50Hz 20-20fps AE_Parameters_20130503155954                 
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd , 0x00);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x03 , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x04 , 0x32);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x06 , 0x00);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x09 , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x0a , 0x46);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf0 , 0x66);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf1 , 0x00);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x90 , 0x05);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x92 , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x98 , 0x66);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x99 , 0x00);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9a , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9b , 0x00);                                               
//Status                                                                                            
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xce , 0xfe);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcf , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd0 , 0xfe);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd1 , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd , 0x00);                                               


#endif
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xc4,0x56);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xc5,0x8f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xca,0x30);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcb,0x45);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcc,0x70);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcd,0x48);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  
  	 //lsc  for st
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x35,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x36,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x37,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x38,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x39,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3a,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3b,0x13);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3c,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3d,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3e,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3f,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x40,0x18);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x41,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x42,0x04);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x43,0x04);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x44,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x45,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x46,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x47,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x48,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x49,0xfd);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4a,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4b,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4c,0xfd);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);  
  	//awb 1
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x28,0xc5);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x29,0x9b);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x2e,0x02);	
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x2f,0x16);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x17,0x17);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x18,0x19);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x19,0x45);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x2a,0xef);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x2b,0x15);  
  	  //awb2
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x73,0x80);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1a,0x80);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1b,0x80); 
  	//d65
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x65,0xd5);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x66,0xfa);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x67,0x72);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x68,0x8a);
  	//indoor
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x69,0xc6);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x6a,0xee);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x6b,0x94);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x6c,0xab);
  	//f
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x61,0x7a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x62,0x98);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x63,0xc5);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x64,0xe6);
  	//cwf
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x6d,0xb9);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x6e,0xde);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x6f,0xb2);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x70,0xd5);
  	 
  	//////////////////skin detect//////
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x08,0x15);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x09,0x04);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0a,0x20);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0b,0x12);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0c,0x27);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0d,0x06);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0f,0x63);  
  	//BPC_grad
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x79,0xf0);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x7a,0x80);  //f0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x7b,0x80);   //f0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x7c,0x20);     //f0
  	  
  	//smooth
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  		//单通道间平滑阈值	
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x57,0x06);	//raw_dif_thr_outdoor
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x58,0x16); //raw_dif_thr_normal //0x0d
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x56,0x20); //raw_dif_thr_dummy //0x10
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x59,0x20); //raw_dif_thr_lowlight //0x10
  		//GrGb平滑阈值
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x89,0x06);	//raw_grgb_thr_outdoor 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x8a,0x16); //raw_grgb_thr_normal   //0x0d
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9c,0x20); //raw_grgb_thr_dummy    //0x10
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9d,0x20); //raw_grgb_thr_lowlight //0x10
  
  		//Gr\Gb之间平滑强度
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x81,0xe0);    //raw_gflt_fac_outdoor
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x82,0xd2); //80//raw_gflt_fac_normal //0xe0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x83,0x70);    //raw_gflt_fac_dummy //0x80 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x84,0x28);    //raw_gflt_fac_lowlight //0x40
  		//Gr、Gb单通道内平滑强度  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x85,0xe0); //raw_gf_fac_outdoor  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x86,0xb2); //raw_gf_fac_normal  //0xc0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x87,0x70); //raw_gf_fac_dummy    //0x80 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x88,0x28); //raw_gf_fac_lowlight //0x40
  		//R、B平滑强度  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5a,0xff);		 //raw_rb_fac_outdoor
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5b,0xd2); //40//raw_rb_fac_normal //0xe0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5c,0x70); 	 //raw_rb_fac_dummy //0x80
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5d,0x00); 	 //raw_rb_fac_lowlight
  	  
  	//sharpen 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe2,0x30);	//sharpen_y_base
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe4,0xa0);	//sharpen_y_max
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe5,0x04); //rangek_neg_outdoor //0x04
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd3,0x04); //rangek_pos_outdoor   //0x04 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd7,0x04); //range_base_outdoor    //0x04
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe6,0x04); //rangek_neg_normal //0x04
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd4,0x04); //rangek_pos_normal  //0x04
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd8,0x04); //range_base_normal   //0x04
  	    
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe7,0x0a); //rangek_neg_dummy //0x07
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd5,0x0a); //rangek_pos_dummy //0x08
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd9,0x0a); //range_base_dummy  	    //0x08
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd2,0x10); //rangek_neg_lowlight
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd6,0x10); //rangek_pos_lowlight
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xda,0x10); //range_base_lowlight
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe8,0x20);//sharp_fac_pos_outdoor
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xec,0x35);//sharp_fac_neg_outdoor
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe9,0x20);//sharp_fac_pos_nr
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xed,0x35);//sharp_fac_neg_nr
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xea,0x20);//sharp_fac_pos_dummy
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xef,0x30);//sharp_fac_neg_dummy
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xeb,0x10);//sharp_fac_pos_low
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf0,0x20);//sharp_fac_neg_low 
  	  #if 0 //default PZT 2013-5-22
  	//CCM
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa0,0x80);//80(红色接近，肤色不理想)
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa1,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa2,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa3,0xf3);//f0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa4,0x8e);//a6
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa5,0x0 );//ea
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa6,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa7,0xe6);//e6
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa8,0x9a);//9a
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa9,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xaa,0x3 );//33
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xab,0xc );//c 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  #else
	    	//CCM
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa0,0x80);//80(红色接近，肤色不理想)
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa1,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa2,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa3,0xf4);//f0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa4,0xa6);//a6
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa5,0xe7);//ea
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa6,0xf4);//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa7,0xe7);//e6
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa8,0xa6);//9a
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa9,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xaa,0x33);//33
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xab,0x0f);//c 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
         #endif
  	//gamma  
  	  
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8b,0x0 );//0 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8c,0xC );//11
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8d,0x19);//19 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8e,0x2C);//28 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8f,0x49);//46 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x90,0x61);//61 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x91,0x77);//78 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x92,0x8A);//8A 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x93,0x9B);//9B 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x94,0xA9);//A9 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x95,0xB5);//B5 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x96,0xC0);//C0 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x97,0xCA);//CA 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x98,0xD4);//D4 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x99,0xDD);//DD 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x9a,0xE6);//E6 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x9b,0xEF);//EF 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);//01 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8d,0xF7);//F7 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8e,0xFF);//FF 
  	             
  	//rpc
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe0,0x4c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe1,0x3c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe2,0x34);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe3,0x2e);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe4,0x2e);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe5,0x2c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe6,0x2c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe8,0x2a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe9,0x2a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xea,0x2a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xeb,0x28);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf5,0x28);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf6,0x28);
  	//ae min gain  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x94,0xa0);//rpc_max_indr
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x95,0x28);//1e//rpc_min_indr 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9c,0xa0);//rpc_max_outdr
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9d,0x28);//rpc_min_outdr    
	#if 0  
  	//ae target
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00); 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xed,0x8c);//80 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf7,0x88);//7c 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf8,0x80);//70 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xec,0x7c);//6c  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xef,0x74);//99
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf9,0x70);//90
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfa,0x68);//80
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xee,0x64);//78    
	 #else
	 //ae target modify 20130503
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00); 
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xed,0x80);//80 
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xf7,0x7c);//7c 
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xf8,0x70);//70 
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xec,0x6c);//6c  
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xef,0x74);//99
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xf9,0x70);//90
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xfa,0x68);//80
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xee,0x64);//78   

	 #endif
  	//gray detect
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x30,0x40);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x31,0x70);//
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x40);//80
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x33,0xef);//
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x34,0x05);//04
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4d,0x2f);//40
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4e,0x20);//
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4f,0x16);//13      
  	  //lowlight lum
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00); 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xb2,0x40);//lum_limit //0x20 PZT 2013-5-22
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xb3,0x1f);//lum_set
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xb4,0x30);//black_vt
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xb5,0x45);//white_vt
  	//saturation
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xbe,0xff);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xbf,0x01); 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xc0,0xff);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xc1,0xd8);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd3,0x80);//0x78 //0x88
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd4,0x78);//0x78 //0x80 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd6,0x70);//0x78 //0x78
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd7,0x60);//0x78 //0x60
  	//HEQ
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xdc,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xdd,0x78); //0x80 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xde,0xa4);//80
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xdf,0x80);    
  	//func enable
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x31,0x00);//10
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x15);//d
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x34,0x76);//16
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x33,0xef);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5f,0x51);          
	return  TRUE;
} /* camsensor_ov7690_write_sensor */



uint8 camsensor_sp0a19_ycbcr_active(void)
{
	return 0;
}

uint8 camsensor_sp0a19_ycbcr_unactive(void)
{
	return 1;
}
/*===========================================================================

FUNCTION      CAMSENSOR_SP0A18_YCBCR_INIT

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

boolean camsensor_sp0a19_init(camsensor_function_table_type *camsensor_function_table_ptr, camctrl_tbl_type *camctrl_tbl_ptr)
{
    uint8  manufacturer_id_msb;
    int j = 3;

	MSG_FATAL("camsensor_SP0A19_ycbcr_init()\n",0,0,0);///yty 

#if 0
	camsensor_camclk_po_hz =12000000; //24000000 
#else
	camsensor_camclk_po_hz =24000000; //24000000 
#endif
	camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;


	/* 1. Initiate I2C:  */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = CAMSENSOR_SP0A19_I2C_SLAVE_ID;
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_REG_DEV | I2C_START_BEFORE_READ); 


	/* 3. Read sensor manufacturer ID MSB:       */
	camsensor_SP0A19_ycbcr_i2c_write_byte(0xFD,0x00);
   
    /*
	if (camsensor_SP0A19_ycbcr_i2c_read_byte(0x02, &manufacturer_id_msb) == FALSE) ///0x92
	{
		MSG_FATAL("camsensor_SP0A18_read REG0x1 fail %x ",manufacturer_id_msb,0,0);///yty 
		return FALSE;
	}
    */

    //Add By zzg 2013_03_25
    // 3 times try
    while(j>0)
    {
        if (camsensor_SP0A19_ycbcr_i2c_read_byte(0x02, &manufacturer_id_msb) == TRUE) ///0x92
    	{
    	    break;
    	}
        else
        {
    		MSG_FATAL("camsensor_SP0A18_read REG0x1 fail %x ",manufacturer_id_msb,0,0);///yty     		
        }
        
        j--;
        i2c_init();
    }

    
    if (j<=0)
    {
      return FALSE;  
    }
    //Add End

	if ( manufacturer_id_msb != 0xA6 )
	{
		MSG_FATAL("manufacturer_id_msb:0x%x,manufacturer_id_lsb:0x%x",manufacturer_id_msb,0,0);///yty 
		return FALSE;
	} 
	
	/* Initialize the sensor_function_table */
	camsensor_sp0a19_ycbcr_register (camsensor_function_table_ptr);

	camsensor_sp0a19_0m3_setup_camctrl_tbl(camctrl_tbl_ptr);
	return TRUE;
} /* camsensor_SP0A18_ycbcr_init */

void camsensor_sp0a19_0m3_init_camsensor_info()
{    
	MSG_FATAL("    camsensor_sp0a19_0m3_init_camsensor_info     ",0,0,0);
	camsensor_info.pclk_invert	=FALSE;
	camsensor_info.preview_dummy_pixels = 0;

	camsensor_camclk_po_hz	=12000000;//24000000

	switch (camsensor_info.msm_id)
	{
		case MSM6246:
		case MSM6260:
		case QSC6270:
			camsensor_info.pclk_invert = FALSE;
			break;

		case MSM6280:
		case MSM6290:
			camsensor_info.pclk_invert = FALSE;
			break;

		default:
			break;  
	}
}/* camsensor_sp0a18_0m3_init_camsensor_info */
/*===========================================================================

FUNCTION      CAMSENSOR_SP0A18_YCBCR_START

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
boolean camsensor_sp0a19_ycbcr_start( camsensor_static_params_type *camsensor_params)
{

	MSG_FATAL("camsensor_SP0A19_ycbcr_start begin ", 0,0,0);///yty 
	camsensor_sp0a19_0m3_init_camsensor_info();
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
	camsensor_params->sensor_model = CAMSENSOR_ID_SP0A18;

	/* CCD or CMOS */
	camsensor_params->sensor_type = CAMSENSOR_CMOS;

	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;
	/* BAYER or YCbCr */
	camsensor_params->output_format = CAMSENSOR_YCBCR;

	/* A pointer to the sensor name for EXIF tags                */
	camsensor_params->sensor_name = camsensor_sp0a19_ycbcr_sensor_name;

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
	camsensor_params->full_size_width  = OV_SP0A19_YCBCR_FULL_SIZE_WIDTH;
	camsensor_params->full_size_height = OV_SP0A19_YCBCR_FULL_SIZE_HEIGHT;

	/* Preview dimensions */
	camsensor_params->qtr_size_width = OV_SP0A19_YCBCR_QTR_SIZE_WIDTH;
	camsensor_params->qtr_size_height = OV_SP0A19_YCBCR_QTR_SIZE_HEIGHT;

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
} /* camsensor_SP0A18_ycbcr_start */


/*===========================================================================

FUNCTION      CAMSENSOR_SP0A18_YCBCR_SNAPSHOT_CONFIG

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

boolean camsensor_sp0a19_ycbcr_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	SP0A19_config_window(0,0,OV_SP0A19_YCBCR_FULL_SIZE_WIDTH,OV_SP0A19_YCBCR_FULL_SIZE_HEIGHT);
	
	/* Sensor output data format */
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;

	/* Set the current dimensions */
	camsensor_params->camsensor_width = camsensor_params->full_size_width;
	camsensor_params->camsensor_height = camsensor_params->full_size_height;
	/* CAMIF frame */
	camsensor_params->camif_frame_config.pixelsPerLine = OV_SP0A19_YCBCR_FULL_SIZE_WIDTH*2;
	camsensor_params->camif_frame_config.linesPerFrame = OV_SP0A19_YCBCR_FULL_SIZE_HEIGHT;
	
	/* CAMIF window */
	camsensor_params->camif_window_width_config.firstPixel = 0;
	camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camsensor_width*2 - 1;
	camsensor_params->camif_window_height_config.firstLine = 0;
	camsensor_params->camif_window_height_config.lastLine = camsensor_params->camsensor_height - 1;
	
	camsensor_current_resolution = camsensor_snapshot_resolution;
	return TRUE;
} /* camsensor_SP0A18_ycbcr_snapshot_config */

/*===========================================================================

FUNCTION      CAMSENSOR_SP0A18_YCBCR_RAW_SNAPSHOT_CONFIG

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

boolean camsensor_sp0a19_ycbcr_raw_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	return TRUE;
} /* camsensor_SP0A19_ycbcr_raw_snapshot_config */


/*===========================================================================

FUNCTION      CAMSENSOR_SP0A19_YCBCR_VIDEO_CONFIG

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

boolean camsensor_sp0a19_ycbcr_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
)
{
	camsensor_sp0a19_sensor_init();

	SP0A19_config_window(0,0,OV_SP0A19_YCBCR_QTR_SIZE_WIDTH,OV_SP0A19_YCBCR_QTR_SIZE_HEIGHT);
	/* Sensor output data format */
	camsensor_params->discardFirstFrame = TRUE;
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;

	switch ( camsensor_preview_resolution )
    {
    	case CAMSENSOR_QTR_SIZE:

			/* Set the current dimensions */
			camsensor_params->camsensor_width  = \
			            camsensor_params->qtr_size_width;
			camsensor_params->camsensor_height = \
			            camsensor_params->qtr_size_height;

			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = \
			            camsensor_params->qtr_size_width*2;

			camsensor_params->camif_frame_config.linesPerFrame = \
			            camsensor_params->qtr_size_height;

			/* CAMIF Window */
			camsensor_params->camif_window_width_config.firstPixel = \
			                 camsensor_info.preview_dummy_pixels*2;

			camsensor_params->camif_window_width_config.lastPixel  = \
			          (camsensor_info.preview_dummy_pixels) \
			      +(camsensor_params->qtr_size_width*2)-1;

			camsensor_params->camif_window_height_config.firstLine = 0;

			camsensor_params->camif_window_height_config.lastLine  = \
			            camsensor_params->qtr_size_height-1;
      		break;


		case CAMSENSOR_FULL_SIZE:
			/* Set the current dimensions */
			camsensor_params->camsensor_width  = \
			            camsensor_params->full_size_width;
			camsensor_params->camsensor_height = \
			            camsensor_params->full_size_height;

			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = \
			            camsensor_params->full_size_width*2;

			camsensor_params->camif_frame_config.linesPerFrame = \
			            camsensor_params->full_size_width;

			/* CAMIF window */
			camsensor_params->camif_window_width_config.firstPixel = 0;

			camsensor_params->camif_window_width_config.lastPixel  = \
			           (camsensor_params->full_size_width*2)-1;

			camsensor_params->camif_window_height_config.firstLine = 0;

			camsensor_params->camif_window_height_config.lastLine  = \
			           camsensor_params->full_size_height-1;

			break;

    	case CAMSENSOR_INVALID_SIZE:

       		return FALSE;

  	}

	//	camsensor_SP0A19_ycbcr_write_sensor (camsensor_preview_resolution);//yty add
	camsensor_current_resolution = camsensor_preview_resolution;

	return TRUE;
} /* camsensor_SP0A19_ycbcr_video_config */

/*===========================================================================

FUNCTION      CAMSENSOR_SP0A19_YCBCR_POWER_UP

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_sp0a19_ycbcr_power_up(void)
{
}


/*===========================================================================

FUNCTION      camsensor_SP0A19_ycbcr_POWER_DOWN

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_sp0a19_ycbcr_power_down(void)
{
    MSG_FATAL ("camsensor_SP0A18_ycbcr_power_down begin", 0,0,0);
}


/*===========================================================================

FUNCTION      camsensor_SP0A19_ycbcr_i2c_read_byte

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
static boolean camsensor_SP0A19_ycbcr_i2c_read_byte(uint8 reg, uint8 *data) 
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
			MSG_FATAL("camsensor_SP0A19_ycbcr_i2c_read_byte: OK %x",reg,0,0);
			return TRUE;
		}
	}

	MSG_FATAL("camsensor_SP0A19_ycbcr_i2c_read_byte: false %x",reg,0,0);
	return FALSE;
}

/*===========================================================================

FUNCTION      camsensor_SP0A19_ycbcr_i2c_write_byte

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
static boolean camsensor_SP0A19_ycbcr_i2c_write_byte(uint8 reg, uint8 data) 
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
			MSG_FATAL("camsensor_SP0A19_ycbcr_i2c_read_byte: OK %x,%x",reg,data,0);
			return TRUE;
		}
	}
	MSG_FATAL("camsensor_SP0A18_ycbcr_i2c_read_byte: false %x,%x",reg,data,0);
	return FALSE;
} /* camsensor_SP0A19_ycbcr_i2c_write_byte */


/* The following are stub functions, (unsupported sensor functions) */
camera_ret_code_type camsensor_sp0a19_ycbcr_write_exposure_gain(uint16 gain, uint32 line_count)
{
	return(CAMERA_SUCCESS);
} /* camsensor_SP0A18_ycbcr_write_exposure_gain */

//wlr 1114 start
camera_ret_code_type camsensor_sp0a19_set_effect(int8 effect) 
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;
	
	MSG_FATAL ("+++++ camsensor_SP0A18_set_effect effect = %d",effect,0,0);
	

	switch(effect)
	{
		case CAMERA_EFFECT_OFF://normal
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd, 0x00);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x62, 0x00);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x63, 0x80);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x64, 0x80);
			break;
			
		case CAMERA_EFFECT_MONO:
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd, 0x00);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x62, 0x20);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x63, 0x80);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x64, 0x80);
			break;
			
		case CAMERA_EFFECT_NEGATIVE://负片
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd, 0x00);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x62, 0x04);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x63, 0x80);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x64, 0x80);
			break;
			
		case CAMERA_EFFECT_SEPIA://棕褐色
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd, 0x00);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x62, 0x10);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x63, 0xc0);			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x64, 0x20);
			break;

		default:
			ret_val = CAMERA_INVALID_PARM;
			break;
	}

  	return ret_val;
}/* camsensor_SP0A19_set_effect */

camera_ret_code_type camsensor_sp0a19_set_wb(int8 wb) 
{
  	camera_ret_code_type ret_val = CAMERA_SUCCESS;
	
	MSG_FATAL ("+++++ camsensor_SP0A18_set_wb wb = %d",wb,0,0);

  	switch (wb)
  	{
		case  CAMERA_WB_AUTO://自动
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x28,0xc5);
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x29,0x9b);
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);  // AUTO 3000K~7000K   	  			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x15); 
	  		break;

		case CAMERA_WB_CLOUDY_DAYLIGHT://多云
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);   //7000K                                     			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x05);                                                          			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);                                                          			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x28,0xbf);		                                                       			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x29,0x89);		                                                       			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);   
	  		break;
	
		case CAMERA_WB_INCANDESCENT://白炽灯
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);  //2800K~3000K                                     			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x05);                                                          			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);                                                          			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x28,0x89);		                                                       			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x29,0xb8);		                                                       			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00); 
	  		break;

		case CAMERA_WB_FLUORESCENT: //荧光
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);  //4200K~5000K                                     			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x05);                                                          			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);                                                          			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x28,0xaf);		                                                       			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x29,0x99);		                                                       			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00); 
			ret_val = CAMERA_SUCCESS;
	  		break;	
	  
		case CAMERA_WB_DAYLIGHT:  //日光
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);  //6500K                                     			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x05);                                                          			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);                                                          			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x28,0xbc);		                                                       			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0x29,0x5d);		                                                       			
			camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00); 
			break;

		default:
	  		ret_val = CAMERA_INVALID_PARM;
	  		break;
  	}

  	return ret_val;
}/* camsensor_SP0A19_set_wb */
//wlr 1114 end

/*===========================================================================

FUNCTION	  CAMSENSOR_SP0A19_YCBCR_2MP_SET_BRIGHTNESS

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
camera_ret_code_type camsensor_sp0a19_set_brightness (int8 value)
{
  	return CAMERA_SUCCESS;
} /* camsensor_ov2640_set_brightness */

/*===========================================================================

FUNCTION	  CAMSENSOR_SP0A19_YCBCR_2MP_SET_CONTRAST

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
camera_ret_code_type camsensor_sp0a19_set_contrast (int8 contrast)
{
  	return CAMERA_SUCCESS;
} /* camsensor_ov2640_set_contrast */

camera_ret_code_type camsensor_sp0a19_set_nightmode (camera_nightshot_mode_type mode)
{
  	return CAMERA_SUCCESS;
}


LOCAL camera_ret_code_type camsensor_sp0a19_set_frame_rate(uint16 fps)
{
  	return CAMERA_SUCCESS;
} /* camsensor_ov9650_set_frame_rate */

camera_ret_code_type camsensor_sp0a19_set_antibanding(int8 antibanding)
{
	if ( g_iBanding == antibanding )
	{
		return CAMERA_SUCCESS;
	}

	return CAMERA_SUCCESS;
	
	g_iBanding = antibanding;
	
	switch ((camera_antibanding_type)antibanding)
	{
		case CAMERA_ANTIBANDING_OFF:
			break;

		case CAMERA_ANTIBANDING_60HZ:
			break;

		case CAMERA_ANTIBANDING_50HZ:
			break;

		default:
			return CAMERA_NOT_SUPPORTED;
	}
	
	return CAMERA_SUCCESS;
}

static uint16 camsensor_sp0a19_0m3_get_snapshot_fps(uint16 fps)
{
	/* In the current driver, the snapshot fps is always 11 */
	fps = 15 * Q8;

	return fps;
}

static void    camsensor_sp0a19_0m3_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
	camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 
	camctrl_tbl_ptr->la_is_supported              = FALSE;
	camctrl_tbl_ptr->cs_is_supported              = FALSE;
	camctrl_tbl_ptr->iso_is_supported             = FALSE;
	camctrl_tbl_ptr->enable_rolloff_correction    = FALSE;
	camctrl_tbl_ptr->hjr_bayer_filtering_enable   = FALSE;
	camctrl_tbl_ptr->nightshot_is_supported       = FALSE;

	camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_sp0a19_0m3_get_snapshot_fps;
}
/*===========================================================================

FUNCTION   CAMSENSOR_SP0A19_YCBCR_REGISTER 

DESCRIPTION  
  Exports the sensor specific functions to CAMSENSOR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_sp0a19_ycbcr_register (camsensor_function_table_type *camsensor_function_table_ptr)
{
	camsensor_function_table_ptr->camsensor_start               	= camsensor_sp0a19_ycbcr_start;
	camsensor_function_table_ptr->camsensor_video_config        	= camsensor_sp0a19_ycbcr_video_config;
	camsensor_function_table_ptr->camsensor_raw_snapshot_config 	= camsensor_sp0a19_ycbcr_raw_snapshot_config;
	camsensor_function_table_ptr->camsensor_snapshot_config     	= camsensor_sp0a19_ycbcr_snapshot_config;
	camsensor_function_table_ptr->camsensor_power_up            	= camsensor_sp0a19_ycbcr_power_up;
	camsensor_function_table_ptr->camsensor_power_down          	= camsensor_sp0a19_ycbcr_power_down;
	camsensor_function_table_ptr->camsensor_write_exposure_gain 	= camsensor_sp0a19_ycbcr_write_exposure_gain;
	camsensor_function_table_ptr->camsensor_set_frame_rate        	= camsensor_sp0a19_set_frame_rate;///lee test
	camsensor_function_table_ptr->camsensor_set_effect				= camsensor_sp0a19_set_effect; //wlr1114	 
	camsensor_function_table_ptr->camsensor_set_wb					= camsensor_sp0a19_set_wb;//wlr 1114
	camsensor_function_table_ptr->camsensor_set_brightness 			= camsensor_sp0a19_set_brightness; //wlr1114	 
	camsensor_function_table_ptr->camsensor_set_contrast 			= camsensor_sp0a19_set_contrast;//wlr 1114
	camsensor_function_table_ptr->camsensor_set_antibanding 		= camsensor_sp0a19_set_antibanding; 
} /* camsensor_SP0A18_ycbcr_register */
#endif /* FEATURE_CAMERA */


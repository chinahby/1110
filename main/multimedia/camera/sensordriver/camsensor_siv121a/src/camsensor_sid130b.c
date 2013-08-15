/*============================================================================

Camera Interface Device Driver Source File for 
Micron MT9D112 2 Mega Pixel CMOS sensor

DESCRIPTION
This file contains the definitions needed for the camera interface
device driver.

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

  EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when      who    what, where, why
--------  -----  ----------------------------------------------------------
10/14/08  chai   Fixed Rvct warnings on ULC
08/28/08  rs     Resolved the side effect of the previous fix(qcamera app displayed white screen,no preview)
07/29/08  cg     Resolved the issue with SVGA( 800 x 600 ) Snapshot.( Needs to be looked by firmware later on). 
08/09/08  cg     Added Support for MWB( Manual White Balance) for DAYLIGHT,FLUORESCENT,INCANDESCENT,CLOUDY_DAYLIGHT
07/03/08  rs     Made the file RVCT compiler compliant
06/25/08  rs     Resolved compiler warnings
06/25/08  rs     Implemented the set_wb api for adjusting the whitebalance
05/05/08  rs     Corrected AHB overflow in ULC while taking snapshot.
                 snapshot fps 7.5 for ULC
04/05/08  rs     Corrected the maximum snapshot size to 1600X1200
                 Added brightness, contrast, ev compensation,exposure mode
                 Modified the Set_frame rate logic 

05/12/08  cg     Increased the Slew rate for PCLK and DATA[0:7].
10/2/07   cg    a) Fixed the bad Preview issue.
b) Fixed the issue with the snapshot at lower frame rate.
c) Fixed the issue of Snapshot config taking more than 700ms.

05/16/07 khk    Filled in the frame_rate_array[]. Added get_snapshot_fps
and set_frame_rate functions.
05/15/07  khk   Removed code to fix fps at 30fps. AFR from sensor is 
running now which decides the current fps.
01/08/07  khk   Updated the driver to work at 30fps
11/23/06  khk   Initial Version

============================================================================*/

/*============================================================================
INCLUDE FILES
============================================================================*/

#include "camsensor.h"

#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_SID130B)
#include "camsensor_sid130b.h"
#include "camerai.h"
#include "ipl.h"
#include "msg.h"
#include "i2c.h"
#include "clk.h"
#include "chromatix_6550.h"

#include "camera_svcs.h"
#include "camsensor.h"

#define CAMERA_MIN_CONTRAST    0
#define CAMERA_DEF_CONTRAST    5
#define CAMERA_MAX_CONTRAST    10


#define CAMERA_MIN_BRIGHTNESS 0
#define CAMERA_DEF_BRIGHTNESS 5
#define CAMERA_MAX_BRIGHTNESS 10

#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_K212_12832

#define CAMSENSOR_SIV130B_RESET_PIN        GPIO_OUTPUT_10
#endif
#endif
/*============================================================================
       TYPE DEFINITIONS 
============================================================================*/
typedef enum {
	OV_SENSOR_PREVIEW_MODE,
	OV_SENSOR_SNAPSHOT_MODE
} sensor_mode_type;

typedef enum
{
	SENSOR_INVALID_MIN_FPS,
	SENSOR_14_FPS = 14,
	SENSOR_15_FPS = 15,
	SENSOR_25_FPS = 25,
	SENSOR_30_FPS = 30,
	SENSOR_INVALID_MAX_FPS 
} sensor_fps_type;

typedef struct register_address_value_pair_struct
{
	uint8 register_address;
	uint8 register_value;
} register_address_value_pair;

/*============================================================================
       DATA DECLARATIONS
============================================================================*/
static const char camsensor_SETi_SID130B_sensor_name[] = "SETi SID130B";
static const char camsensor_SETi_SID130B_sensor_suffix[]= "SID130B_mu2myu";

/****************SLEEP MODE ***************************/
/*****************************************************/

/*****************************************************************************
*                          RUN TIME VARIABLES
****************************************************************************/

/*============================================================================
	  MACRO DEFINITIONS
============================================================================*/
/*  I2C Address of the Sensor  */
#define  SETi_SID130B_I2C_SLAVE_ID  	  		0x6e//0x37	// 0x60

/* Micron MT9D112 Registers and their values */
/* Sensor Core Registers */
#define  REG_SETi_SID130B_PIDH            		0x01
#define  REG_SETi_SID130B_PIDL            		0x02

#define  SETi_SID130B_PIDH                    	0x1b
#define  SETi_SID130B_PIDL                    	0xb1


/***************************************************************
* The following defines are used as UI parameters
***************************************************************/
#define CAMSENSOR_SID130B_FULL_SIZE_WIDTH  900//480//600
#define CAMSENSOR_SID130B_FULL_SIZE_HEIGHT 1200//640//800

#define CAMSENSOR_SID130B_QTR_SIZE_WIDTH   600
#define CAMSENSOR_SID130B_QTR_SIZE_HEIGHT  800

#define CAMSENSOR_MIN_BRIGHTNESS 0

#define CAMSENSOR_MAX_BRIGHTNESS 10

#define CAMSENSOR_MIN_CONTRAST 0

#define CAMSENSOR_MAX_CONTRAST 10

#define MIN_TARGET_LUMA 0x2

#define MAX_TARGET_LUMA 0x66 

#define MAX_EV_COMP 25


/*============================================================================
	  FORWARD DECLARATIONS 
============================================================================*/
static void    camsensor_SETi_SID130B_register(camsensor_function_table_type *camsensor_function_table_ptr);
static void    camsensor_SETi_SID130B_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
static boolean SETi_SID130B_i2c_write_byte(uint8 offset, byte data);
static boolean SETi_SID130B_i2c_read_byte(uint8 offset, byte *data);
static camera_ret_code_type camsensor_SETi_SID130B_set_frame_rate(uint16 fps);
static camera_ret_code_type camsensor_SETi_SID130B_set_effect(int8 effect);

static uint16 camsensor_SETi_SID130B_get_snapshot_fps(uint16);
camera_ret_code_type camsensor_SETi_SID130B_set_wb(int8 wb);
LOCAL uint16 camsensor_SETi_SID130B_get_snapshot_fps(uint16 fps);
static camera_ret_code_type camsensor_SETi_SID130B_set_frame_rate(uint16 fps);
camera_ret_code_type camsensor_SETi_SID130B_set_wb(int8 wb);
camera_ret_code_type camsensor_SETi_SID130B_set_nightmode (int8 mode);
camera_ret_code_type camsensor_SETi_SID130B_set_brightness(int8 value);

static camera_effect_type camsensor_SETi_SID130B_effect = CAMERA_EFFECT_OFF;

static CAMIF_InputFormatType format_preview,format_snapshot;
/*****************************************/
/*****************************************/

/*============================================================================
	  EXPORTED FUNCTIONS
============================================================================*/

/*===========================================================================

FUNCTION      CAMSENSOR_SETi_SID130BYU_INIT

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
init
===========================================================================*/
static register_address_value_pair camsensor_SETi_SID130B_reg_settings_array[]=
{  
	{0x00, 0x00},
	{0x04, 0x00},
	{0x05, 0x0f},
	{0x06, 0x8f},
	{0x07, 0x88},
	{0x08, 0x22},
	{0x09, 0x11},
	{0x0a, 0x11},
	{0x10, 0x13},	
	{0x11, 0x01},
	{0x12, 0x8A},
	{0x13, 0x1a},
	{0x14, 0x27},
	{0x15, 0x22},
	{0x18, 0x38},
	{0x17, 0xcb},
	{0x18, 0x38},
	//GA-STiLL 
	{0x20, 0x00},
	{0x21, 0x02},
	{0x23, 0x51},
	//GB-Preview 
	/*
	{0x24, 0x00},
	{0x25, 0x10},
	{0x27, 0xaf},
	*/

	{0x24, 0x00},
	{0x25, 0x02},
	{0x27, 0x51},

    {0x40, 0x0f},
	{0x41, 0x17},
	{0x42, 0x52},
    {0x43, 0x80},
	{0x44, 0x00},
	{0x45, 0x00},

	// AE 	     
	{0x00, 0x01},

	{0x10, 0x00},
	{0x11, 0x10},

	{0x12, 0x8c},//0x85
	{0x13, 0x8c},//0x85
	{0x14, 0x85},
    {0x17, 0xC4},
    {0x1c, 0x0c},

	//{0x34, 0x64},   //100
	{0x34, 0x3c},
	{0x35, 0x3c},   //
	{0x36, 0x40}, //0X35/0X34=X/64
	{0x40, 0x40},

	{0x70, 0x04},
	{0x73, 0x22},
	{0x74, 0x07},
	{0x77, 0xd0},
	{0x78, 0xd8},
	{0x79, 0x68},

	// AWB
	{0x00, 0x02},  
	{0x10, 0x00},

	{0x11, 0x11},
	{0x13, 0x7f},
	{0x14, 0x80},
	{0x15, 0xee},
	{0x16, 0x80},
	{0x17, 0xd0},
	{0x18, 0x80},
	{0x19, 0x98},
	{0x1a, 0x68},
	{0x1b, 0x98},
	{0x1c, 0x68},
	{0x1d, 0xa8},
	{0x1e, 0x70},
	{0x20, 0xf0},
	{0x21, 0x98},
	{0x22, 0xb4},
	{0x23, 0x20},
	{0x25, 0x20},
	{0x26, 0x05},
	{0x27, 0x08},
	{0x28, 0x08},

	{0x29, 0xb9},//  outdoor RGAIN # Default : c8
	{0x2a, 0x9d},//  outdoor BGAIN # Default : 9d

	{0x30, 0x00},
	{0x31, 0x10},
	{0x32, 0x00},
	{0x33, 0x10},
	{0x34, 0x06},
	{0x35, 0x30},
	{0x36, 0x04},
	{0x37, 0xa0},
	{0x40, 0x01},
	{0x41, 0x04},
	{0x42, 0x08},
	{0x43, 0x10},
	{0x44, 0x13},
	{0x45, 0x6b},
	{0x46, 0x82},
    {0x52, 0x88},
	{0x53, 0xa3},
	{0x54, 0xb6},
	{0x55, 0xa3},
	{0x56, 0xb6},
	{0x57, 0xc5},
	{0x58, 0x99},
	{0x59, 0xc5},
	{0x5A, 0x99},

	{0xb4, 0x05},
	{0xb5, 0x0f},
	{0xb6, 0x06},
	{0xb7, 0x06},
	{0xb8, 0x40},
	{0xb9, 0x10},
	{0xba, 0x06},

	{0x64, 0x00},
	{0x65, 0x00},
	{0x66, 0x00},
	{0x67, 0x00},
	{0x68, 0xa5},
	{0x69, 0xb4},
	{0x6a, 0xb3},
	{0x6b, 0xac},
	{0x6c, 0xb7},
	{0x6d, 0x98},
	{0x6e, 0xba},
	{0x6f, 0x90},
	{0x70, 0xbf},
	{0x71, 0x9b},
	{0x72, 0xce},
	{0x73, 0x8c},


	{0x74, 0xbf},
	{0x75, 0x9b},
	{0x76, 0xad},
	{0x77, 0xba},

	{0x78, 0x8f},
	{0x79, 0x9a},
	{0x7a, 0xa3},
	{0x7b, 0xac},

	{0x7c, 0xa0},
	{0x7d, 0xa9},
	{0x7e, 0x95},
	{0x7f, 0xac},

	{0x80, 0xad},
	{0x81, 0xbc},
	{0x82, 0x98},
	{0x83, 0xa4},

	{0x84, 0x00},
	{0x85, 0x00},
	{0x86, 0x00},
	{0x87, 0x00},

	{0x88, 0xc9},
	{0x89, 0xd5},
	{0x8a, 0x70},
	{0x8b, 0x7b},

	{0x8C, 0xD0},
	{0x8D, 0xE5},
	{0x8E, 0x58},
	{0x8F, 0x70},

	//IDP
	{0x00, 0x03},

	{0x10, 0xff},
	{0x11, 0x1d}, // 1d
	{0x12, 0x9d},// 3d
	{0x13, 0xff},
	{0x14, 0x00},

	{0x30, 0x88},
	{0x31, 0x14},
	{0x32, 0x0c},
	{0x33, 0x08},
	{0x34, 0x04},
	{0x35, 0x04},
	{0x36, 0x44},
	{0x37, 0x66},
	{0x38, 0x00},//0x00
	{0x39, 0x08},//0x40
	{0x3A, 0x08},//0x60
	{0x3B, 0x20},//0x80
	{0x3C, 0x20},//0xa0
	{0x3D, 0x00},
	{0x3E, 0x40},
	{0x3F, 0x60},
	{0x40, 0x80},
	{0x41, 0xa0},
	{0x42, 0x30},
	{0x43, 0x40},
	{0x44, 0x40},
	{0x45, 0x0f},
	{0x46, 0xa0},
	{0x47, 0x30},
	//shading
	{0x50, 0x10},
	{0x51, 0x28},
	{0x52, 0x24},
	{0x53, 0x24},

	{0x54, 0x10},
	{0x55, 0x20},
	{0x56, 0x10},
	{0x57, 0x20},

	{0x58, 0x10},
	{0x59, 0x20},
	{0x5A, 0x10},
	{0x5B, 0x20},

	{0x5C, 0x10},
	{0x5D, 0x34},
	{0x5E, 0x25},
	{0x5F, 0x20},
        {0x60, 0x32},
        {0x61, 0x20},
        {0x62, 0x58},

	{0x6B, 0x00},
	{0x6C, 0x01},
	{0x6D, 0x23},
	{0x6E, 0x55},
	{0x6F, 0x55},
	{0x70, 0x55},
	{0x71, 0x00},
	{0x72, 0x01},
	{0x73, 0x23},
	{0x74, 0x44},
	{0x75, 0x45},
	{0x76, 0x66},
	{0x77, 0x00},
	{0x78, 0x01},
	{0x79, 0x23},
	{0x7A, 0x44},
	{0x7B, 0x45},
	{0x7C, 0x66},
	{0x7D, 0x00},
	{0x7E, 0x00},
	{0x7F, 0x12},
	{0x80, 0x33},
	{0x81, 0x44},
	{0x82, 0x55},
	{0x83, 0x14},
	{0x84, 0x0f},

	{0x90, 0x01}, //win size 800x600
	{0x91, 0xf4},
	{0x92, 0x00},
	{0x93, 0xc8},
	
	{0x94, 0x02}, //win size 800x600
	{0x95, 0x58},
	{0x96, 0x03},
	{0x97, 0x20},

	//Interpolation   
	{0xA0, 0x3f},
	{0xA1, 0x05},
	{0xA2, 0xB7},
	{0xA3, 0xB7},
	{0xA4, 0x08},
	{0xA5, 0xFF},
	{0xA6, 0x08},
	{0xA7, 0xFF},
	{0xA8, 0x00},
	{0xA9, 0x00},
	{0xAA, 0x00},
	{0xAB, 0x00},
	{0xAC, 0x60},
	{0xAD, 0x18},
	{0xAE, 0x10},
	{0xAF, 0x20},
	{0xB0, 0x08},
	{0xB1, 0x00},
	//CMA
	{0xC0, 0xaF},
	{0xC1, 0x66},
	{0xC2, 0xd4},
	{0xC3, 0x06},
	{0xC4, 0xed},
	{0xC5, 0x5A},
	{0xC6, 0xF9},
	{0xC7, 0xF9},
	{0xC8, 0xBF},
	{0xC9, 0x88},

	{0xCA, 0x00},
	{0xCB, 0x00},
	{0xCC, 0x00},
	{0xCD, 0x00},
	{0xCE, 0x00},

	{0xD0, 0x2F},
	{0xD1, 0x71},
	{0xD2, 0xb6},
	{0xD3, 0x19},
	{0xD4, 0xE5},
	{0xD5, 0x5a},
	{0xD6, 0xFb},
	{0xD7, 0xe8},
	{0xD8, 0xae},
	{0xD9, 0xaa},

	{0xDA, 0x00},
	{0xDB, 0x00},
	{0xDC, 0x00},
	{0xDD, 0x00},
	{0xDE, 0x00},

	{0xE0, 0x2F},
	{0xE1, 0x5C},
	{0xE2, 0xDD},
	{0xE3, 0x06},
	{0xE4, 0xE0},
	{0xE5, 0x69},
	{0xE6, 0xF6},
	{0xE7, 0xE5},
	{0xE8, 0xAB},
	{0xE9, 0xAE},
	{0xEA, 0x6A},
	{0xEB, 0x01},
	{0xEC, 0x2D},
	{0xED, 0xEE},
	{0xEE, 0x04},

	//IDP
	{0x00, 0x04},

	{0x10, 0x00},
	{0x11, 0x04},
	{0x12, 0x08},
	{0x13, 0x17},
	{0x14, 0x35},
	{0x15, 0x53},
	{0x16, 0x6c},
	{0x17, 0x81},
	{0x18, 0x93},
	{0x19, 0xa3},
	{0x1A, 0xb1},
	{0x1B, 0xca},
	{0x1C, 0xdd},
	{0x1D, 0xf0},
	{0x1E, 0xf8},
	{0x1F, 0xFF},

	{0x20, 0x00},
	{0x21, 0x04},
	{0x22, 0x08},
	{0x23, 0x17},
	{0x24, 0x35},
	{0x25, 0x53},
	{0x26, 0x6c},
	{0x27, 0x81},
	{0x28, 0x93},
	{0x29, 0xa3},
	{0x2A, 0xb1},
	{0x2B, 0xca},
	{0x2C, 0xdd},
	{0x2D, 0xf0},
	{0x2E, 0xf8},
	{0x2F, 0xFF},

	{0x30, 0x00},
	{0x31, 0x04},
	{0x32, 0x08},
	{0x33, 0x17},
	{0x34, 0x35},
	{0x35, 0x53},
	{0x36, 0x6c},
	{0x37, 0x81},
	{0x38, 0x93},
	{0x39, 0xa3},
	{0x3A, 0xb1},
	{0x3B, 0xca},
	{0x3C, 0xdd},
	{0x3D, 0xf0},
	{0x3E, 0xf8},
	{0x3F, 0xFF},


	{0x60, 0x33},
	{0x61, 0x20},
	{0x62, 0xE4},
	{0x63, 0xFA},
	{0x64, 0x13},
	{0x65, 0x25},
	{0x66, 0x07},
	{0x67, 0xF5},
	{0x68, 0xEA},
	{0x69, 0x20},
	{0x6A, 0xC8},
	{0x6B, 0xC4},
	{0x6C, 0x84},
	{0x6D, 0x04},
	{0x6E, 0x0C},
	{0x6F, 0x00},

	{0x80, 0xa2},
	{0x81, 0x10},//0x10
	{0x82, 0x18},//0x14
	{0x83, 0x04},//0x08
	{0x85, 0x08},
	{0x87, 0x04},//0x08
	{0x89, 0x08},
	{0x90, 0x16},
	{0x91, 0x03},
	{0x93, 0xE0},
	{0x94, 0x14},
	{0x95, 0x14},
	{0x96, 0x4C},
	{0x97, 0x76},
	{0x9a, 0xf5},
	{0xa1, 0x08},
	{0xa2, 0x10},
	{0xa3, 0x16},
	{0xa4, 0x20},
	{0xa5, 0x30},
	{0xa6, 0xa0},
	{0xac, 0xff},
	{0xad, 0x09},
	{0xae, 0x96},
	{0xaf, 0x18},
	{0xbc, 0x14},
	{0xbd, 0x14},
	{0xbe, 0x14},
	{0xbf, 0x14},
	{0xc0, 0x10},
	{0xc1, 0x10},
	{0xc2, 0x14},
	{0xc3, 0x14},
	{0xc4, 0x14},
	{0xc5, 0x14},
	{0xc6, 0x01},
	{0xc7, 0x01},
	{0xc8, 0x01},
	{0xc9, 0x01},
	{0xca, 0x01},
	{0xcb, 0x01},
	{0xcc, 0x04},
	{0xcd, 0x3f},
	{0xce, 0x01},
	//IDP
	{0x00, 0x05},
	{0x40, 0x15},
	{0x41, 0x28},
	{0x42, 0x04},
	{0x43, 0x15},
	{0x44, 0x28},
	{0x45, 0x04},
	{0x46, 0x15},
	{0x47, 0x28},
	{0x48, 0x04},
	{0x90, 0xca},
	{0x91, 0x81},
	{0x93, 0x50},
	{0x94, 0x41},
	{0x95, 0x7e},
	{0x99, 0xc0},
	{0xa0, 0x10},
	{0xa1, 0x22},
	{0xa2, 0x36},
	{0xa3, 0x49},
	{0xa4, 0x5d},
	{0xa5, 0x70},
	{0xa6, 0x82},
	{0xa7, 0x94},
	{0xa8, 0xa5},
	{0xa9, 0xb5},
	{0xaa, 0xc3},
	{0xab, 0xd1},
	{0xac, 0xde},
	{0xad, 0xea},
	{0xae, 0xf5},
	{0xaf, 0xff},
	{0xb0, 0xc0},
	{0xb1, 0x04},
	{0xb8, 0x0f},
	{0xb9, 0x10},
	{0xc0, 0x03},
	{0xc1, 0x0e},
	{0xc2, 0x16},
	{0xc3, 0x24},
	{0xc4, 0x3f},
	{0xc5, 0x56},
	{0xc6, 0x6a},
	{0xc7, 0x7c},
	{0xc8, 0x8C},
	{0xc9, 0x98},
	{0xca, 0xA2},
	{0xcb, 0xB8},
	{0xcc, 0xCD},
	{0xcd, 0xE2},
	{0xce, 0xF0},
	{0xcf, 0xFF},
	{0xe0, 0x04},
	{0xe1, 0x88},
	{0xe2, 0x09},
	{0xe3, 0x0c},
	{0xe4, 0x11},
	{0xe5, 0x16},
	{0xe6, 0x1b},
	{0xe7, 0x24},
	{0xe8, 0x30},
	{0xe9, 0x3e},
	{0x00, 0x00},
	{0x03, 0x55}, // 05
	{0x00, 0x01},
	{0x10, 0x80},
	{0x00, 0x02},
	{0x10, 0xd3}
};

uint8 camsensor_SID130B_active(void)
{
	return 1;
}

uint8 camsensor_SID130B_unactive(void)
{
	return 0;
}

boolean camsensor_SID130B_init(camsensor_function_table_type *camsensor_function_table_ptr,camctrl_tbl_type *camctrl_tbl_ptr)                                                                                       
{
	uint8 pidh; 

	MSG_FATAL("camsensor_SID130B_init!",0,0,0);

  	/* Input MCLK = 24MHz */

  	camsensor_camclk_po_hz = 24000000;
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_VERSION_K212_12832

  	gpio_tlmm_config(CAMSENSOR_SIV130B_RESET_PIN);
  	gpio_out(CAMSENSOR_SIV130B_RESET_PIN, GPIO_HIGH_VALUE);
  	clk_busy_wait(1000);
  	gpio_out(CAMSENSOR_SIV130B_RESET_PIN, GPIO_LOW_VALUE);
  	clk_busy_wait(20*1000);
  	gpio_out(CAMSENSOR_SIV130B_RESET_PIN, GPIO_HIGH_VALUE);
  	clk_busy_wait(1000);
#endif
#endif
  	camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
  	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;

  	/* Configure I2C parameters */
  	camsensor_i2c_command.bus_id       = I2C_BUS_HW_CTRL;
  	camsensor_i2c_command.slave_addr   = SETi_SID130B_I2C_SLAVE_ID;
  	/*lint -save -e655 */
  	camsensor_i2c_command.options      =  (i2c_options_type)(I2C_REG_DEV | I2C_START_BEFORE_READ); 

	/***********add   sensor type  judgement***********************/
  	SETi_SID130B_i2c_write_byte(0x00, 0x00);

  	if (!SETi_SID130B_i2c_read_byte(REG_SETi_SID130B_PIDH,  &pidh))
    {
    	MSG_FATAL("SETi_SID130B_i2c_read_byte failed!",0,0,0);
    	return FALSE;
  	}

	MSG_FATAL("REG_SETi_SID130B_PIDH = 0x%x",pidh,0,0);

	if ( pidh != SETi_SID130B_PIDH)
	{
		MSG_FATAL("SETi_SID130B_PIDH != pidh",0,0,0);
    	return FALSE;
	}

	/* Register function table: */
	camsensor_SETi_SID130B_register(camsensor_function_table_ptr);

	/* Setup camctrl_tbl */
	camsensor_SETi_SID130B_setup_camctrl_tbl(camctrl_tbl_ptr);

	return TRUE;
} /* camsensor_mt9d112_mu2m0yu_init */

/*===========================================================================
FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_INIT_CAMSENSOR_INFO

DESCRIPTION
Initialize the camera sensor info;

DEPENDENCIES
msm_id must be set to a vaild id before calling this function.

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/

void camsensor_SETi_SID130B_init_camsensor_info()
{
	camsensor_info.pclk_invert	=FALSE;
	camsensor_info.preview_dummy_pixels = 0;
	format_preview	= CAMIF_YCbCr_Cr_Y_Cb_Y;
	format_snapshot	= CAMIF_YCbCr_Cr_Y_Cb_Y;
	camsensor_camclk_po_hz	=24000000;

	switch(camsensor_info.msm_id)
	{
		MSG_FATAL("camsensor_SETi_SID130B_init_camsensor_info : %d",camsensor_info.msm_id,0,0);
		case MSM6280:
		case MSM6290:
		   camsensor_info.pclk_invert=FALSE;
		   break;


		case MSM6800B:
		    camsensor_info.pclk_invert=FALSE;
		    camsensor_camclk_po_hz=49000000;
			break;

		case MSM6246:
		case MSM6260:
			camsensor_info.preview_dummy_pixels = 1;
		   	format_preview=CAMIF_YCbCr_Cb_Y_Cr_Y;
		   	format_snapshot=CAMIF_YCbCr_Cb_Y_Cr_Y;
		   	//fmt_reg_val= 0x0200;
		   	break;

		case SC2X:
		  	camsensor_info.pclk_invert=FALSE;
		  	camsensor_info.preview_dummy_pixels = 1;
		  	format_preview = CAMIF_YCbCr_Y_Cb_Y_Cr;
		  	format_snapshot= CAMIF_YCbCr_Y_Cb_Y_Cr;
		  	//fmt_reg_val=0x0200;
			camsensor_camclk_po_hz=54000000;
			break;

		default:
			break;
	}
}/* camsensor_mt9d112_mu2m0yu_init_camsensor_info */

/*===========================================================================

FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_START 

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

boolean camsensor_SETi_SID130B_start(camsensor_static_params_type *camsensor_params)
{

	MSG_FATAL("camsensor_SETi_SID130B_start!",0,0,0);

	camsensor_SETi_SID130B_init_camsensor_info();
	
	/* Initialize CAMIF operation mode */
	camsensor_params->camif_config.SyncMode    = CAMIF_APS;
	camsensor_params->camif_config.HSyncEdge   = CAMIF_High;
	camsensor_params->camif_config.VSyncEdge   = CAMIF_Low; //CAMIF_High;
	camsensor_params->camif_efs_config.EFS_EOL = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOL = 0x0000;
	camsensor_params->camif_efs_config.EFS_EOF = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOF = 0x0000;

	/* ------------------------------------------- */
	/* Configure the extra Camera Layer Parameters */
	/* ------------------  Sensor-specific Config -------------- */
	/* Make/model of sensor */
	camsensor_params->sensor_model  = CAMSENSOR_ID_SID130B;

	/* CCD or CMOS */
	camsensor_params->sensor_type   = CAMSENSOR_CMOS;

	/* Sensor output data format */
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;

	/* BAYER or YCbCr */
	camsensor_params->output_format = CAMSENSOR_YCBCR;

	/* A pointer to the sensor name for EXIF tags */
	camsensor_params->sensor_name = camsensor_SETi_SID130B_sensor_name;

	/* Pointer to sensor suffix */
	camsensor_params->sensor_suffix =camsensor_SETi_SID130B_sensor_suffix;
	
	/* What is the maximum FPS that can be supported by this sensor in video mode? */
	camsensor_params->max_video_fps   = 30 * Q8;

	/* Application assigned FPS in video mode */
	camsensor_params->video_fps 	  = 30 * Q8;

	/* Snapshot mode operation */
	camsensor_params->max_preview_fps = 15 * Q8;

	/* May be assigned with max_preview_fps or nightshot_fps. */
	camsensor_params->preview_fps     = camsensor_params->max_preview_fps; 

	/* VFE's perception of Sensor output capability */

	/* Full size dimensions - 1600x1200 */
	camsensor_params->full_size_width  = CAMSENSOR_SID130B_FULL_SIZE_WIDTH;
	camsensor_params->full_size_height = CAMSENSOR_SID130B_FULL_SIZE_HEIGHT; /* 2 Extra Lines for VFE padding */

	/**********</PR:3150>}**********************/
	/* Quarter size dimensions - The same size. */
	camsensor_params->qtr_size_width  = CAMSENSOR_SID130B_QTR_SIZE_WIDTH;      //CAMSENSOR_SID130B_FULL_SIZE_WIDTH;
	camsensor_params->qtr_size_height = CAMSENSOR_SID130B_QTR_SIZE_HEIGHT ;     // CAMSENSOR_SID130B_FULL_SIZE_HEIGHT;

	switch(camsensor_preview_resolution)
	{
		case CAMSENSOR_QTR_SIZE:

			camsensor_params->preview_dx_decimation = \
			            camsensor_params->full_size_width * Q12 / \
			          camsensor_params->qtr_size_width;
			camsensor_params->preview_dy_decimation = \
			          camsensor_params->full_size_height * Q12 / \
			          camsensor_params->qtr_size_height;

			camsensor_params->camsensor_width  = \
			          camsensor_params->qtr_size_width;
			camsensor_params->camsensor_height = \
			          camsensor_params->qtr_size_height;
			camsensor_params->format = CAMIF_YCbCr_Cb_Y_Cr_Y;

			break;

		case CAMSENSOR_FULL_SIZE:

			camsensor_params->preview_dx_decimation = Q12;
			camsensor_params->preview_dy_decimation = Q12;
			camsensor_params->camsensor_width  = \
			          camsensor_params->full_size_width;
			camsensor_params->camsensor_height = \
			          camsensor_params->full_size_height;
			camsensor_params->format = CAMIF_YCbCr_Cb_Y_Cr_Y;

			break;

		default:

			MSG_ERROR("Unrecognized preview resolution, assuming full size",0,0,0);

			camsensor_params->camsensor_width  = \
			      camsensor_params->full_size_width;
			camsensor_params->camsensor_height = \
			      camsensor_params->full_size_height;

			break;
    }
	MSG_FATAL("camsensor_params->preview_dx_decimation=%d,y=%d",camsensor_params->preview_dx_decimation,camsensor_params->preview_dy_decimation,0);
	/* Pixel Clock Should be inverted for this Sensor */
	MSG_FATAL("camsensor_params->camsensor_width=%d,heiht==%d",camsensor_params->camsensor_width,camsensor_params->camsensor_height,0);
	/* ------------  Auto Exposure Control Config -------------- */
	camsensor_params->aec_enable = FALSE;

	/* ------------  Auto White Balance Config ----------------- */
	/* AWB -Auto White Balance Parameters */
	camsensor_params->awb_enable = FALSE;

	/* ------------  Auto Focus Config ------------------------- */
	/* AF -Auto Focus Parameters */
	camsensor_params->af_enable = FALSE;

	camsensor_params->num_possible_frame_rates = 3; 
	/* Define these frame rates */
	/* By convention, the highest frame rate will be first in the
	array (zeroth index) and the lowest last (in order). */

	camsensor_params->frame_rate_array[0].fps = (uint16) (30 * Q8);  /* Q8 */
	camsensor_params->frame_rate_array[0].use_in_auto_frame_rate = TRUE;

	camsensor_params->frame_rate_array[1].fps = (uint16) (15 * Q8);  /* Q8 */
	camsensor_params->frame_rate_array[1].use_in_auto_frame_rate = TRUE;

	camsensor_params->frame_rate_array[2].fps = (uint16) (7.5 * Q8); /* Q8 */
	camsensor_params->frame_rate_array[2].use_in_auto_frame_rate = TRUE;

	/* ------------  Default Misc Parmas Config ---------------- */
	/* Does the sensor need/use a flash  */
#if defined FEATURE_WHITE_LED_FLASH || defined FEATURE_STROBE_FLASH
	camsensor_params->support_auto_flash = FALSE;
#else
	camsensor_params->support_auto_flash = FALSE;
#endif  

	/* PCLK Invert */
	camsensor_params->pclk_invert = camsensor_info.pclk_invert;

	return TRUE;
} /* camsensor_mt9d112_mu2m0yu_start */

/*===========================================================================

FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_SNAPSHOT_CONFIG

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
// 1600x1200,UXGA YUV output
static register_address_value_pair SID130B_mode_full[] = 
{
	{0x00, 0x00},
	{0x04, 0x13},
	{0x05, 0x8f},
	{0x06, 0x8e},
	//GA-STiLL 
	{0x20, 0x00},
	{0x21, 0x02},
	{0x23, 0x51},
	//GB-Preview 
	/*
	{0x24, 0x00},
	{0x25, 0x10},
	{0x27, 0xaf},
		*/
	{0x24, 0x00},
	{0x25, 0x02},
	{0x27, 0x51},
	
	{0x41, 0x17},
	{0x42, 0x52},

	// AE		 
	{0x00, 0x01},
	   
	{0x34, 0x3c},	//100
	{0x35, 0x3c},	//
	{0x36, 0x40}, //0X35/0X34=X/64


//IDP
	{0x00, 0x03},

	{0x90, 0x00}, //win size 1600x12600
    {0x91, 0x00},
    {0x92, 0x00},
    {0x93, 0x00},
	{0x94, 0x06}, //win size 1600x12600
    {0x95, 0x40},
    {0x96, 0x04},
    {0x97, 0xb0},
};

boolean camsensor_SETi_SID130B_snapshot_config
(
	camsensor_static_params_type *camsensor_params
) 
{
 	int32 i, array_length;
	MSG_FATAL("camsensor_SETi_SID130B_snapshot_config!",0,0,0);

	camsensor_params->format = format_snapshot;
 	
	camsensor_params->camsensor_width  = \
			camsensor_params->full_size_width;
	camsensor_params->camsensor_height = \
			camsensor_params->full_size_height;

	/* CAMIF frame */
	camsensor_params->camif_frame_config.pixelsPerLine = \
			camsensor_params->full_size_width*2;
	camsensor_params->camif_frame_config.linesPerFrame = \
			camsensor_params->full_size_height;

	/* CAMIF window */
	camsensor_params->camif_window_width_config.firstPixel = 640;
	camsensor_params->camif_window_width_config.lastPixel  = 640 + (camsensor_params->full_size_width*2)-1;

	camsensor_params->camif_window_height_config.firstLine = 0;
	camsensor_params->camif_window_height_config.lastLine  = 0 + camsensor_params->full_size_height-1;
			
	array_length = sizeof(SID130B_mode_full) / sizeof(SID130B_mode_full[0]);
	/* Configure sensor for Preview mode  */
	for (i = 0; i < array_length; i++)
	{
		if (SETi_SID130B_i2c_write_byte(SID130B_mode_full[i].register_address, SID130B_mode_full[i].register_value) == FALSE) 
		{
			return(FALSE);
		}
	}

	clk_busy_wait(100*1000);
	camsensor_current_resolution = camsensor_snapshot_resolution;
	return TRUE;
} /* camsensor_mt9d112_mu2m0yu_snapshot_config */

/**********</PR:1673>}**********************/




/*===========================================================================

FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_RAW_SNAPSHOT_CONFIG

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

boolean camsensor_SETi_SID130B_raw_snapshot_config
(
	camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	return TRUE;
} /* camsensor_mt9d112_mu2m0yu_raw_snapshot_config */



/*===========================================================================

FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_VIDEO_CONFIG

DESCRIPTION
Configure the camera sensor and the camera interface
for Preview mode.

DEPENDENCIES
None

RETURN VALUE
TRUE if successful
FALSE otherwise

SIDE EFFECTS
None

===========================================================================*/

boolean camsensor_SETi_SID130B_video_config
(
	camsensor_static_params_type  *camsensor_params      /* Camera sensor config params */
)
{
	int32 i, array_length;

	
	MSG_FATAL("camsensor_SETi_SID130B_video_config!",0,0,0);

	array_length = sizeof(camsensor_SETi_SID130B_reg_settings_array) / sizeof(camsensor_SETi_SID130B_reg_settings_array[0]);
	/* Configure sensor for Preview mode and Snapshot mode */
	for (i = 0; i < array_length; i++)
	{
		if (SETi_SID130B_i2c_write_byte(camsensor_SETi_SID130B_reg_settings_array[i].register_address, 
	    		camsensor_SETi_SID130B_reg_settings_array[i].register_value) == FALSE) 
	 	{
	    	return(FALSE);
	 	}
	}

	camera_timed_wait(10); 
      
	camsensor_params->format 			= format_preview;
 	camsensor_params->discardFirstFrame =  TRUE;

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
			camsensor_params->camif_window_width_config.firstPixel = 0;

			camsensor_params->camif_window_width_config.lastPixel  = 0 +(camsensor_params->qtr_size_width*2)-1;

			camsensor_params->camif_window_height_config.firstLine = 0;

			camsensor_params->camif_window_height_config.lastLine  = 0 + camsensor_params->qtr_size_height-1;
      		break;


		case CAMSENSOR_FULL_SIZE:
			camsensor_params->camsensor_width  = camsensor_params->full_size_width;
			camsensor_params->camsensor_height = camsensor_params->full_size_height;

			/* CAMIF frame */
			camsensor_params->camif_frame_config.pixelsPerLine = \
					camsensor_params->full_size_width*2;
			camsensor_params->camif_frame_config.linesPerFrame = \
					camsensor_params->full_size_height;

			/* CAMIF window */
			camsensor_params->camif_window_width_config.firstPixel = 0;
			camsensor_params->camif_window_width_config.lastPixel  = 0 + (camsensor_params->full_size_width*2)-1;

			camsensor_params->camif_window_height_config.firstLine = 0;
			camsensor_params->camif_window_height_config.lastLine  = 0 + camsensor_params->full_size_height-1;

			break;

    	case CAMSENSOR_INVALID_SIZE:

       		return FALSE;

  	}

	camsensor_current_resolution = camsensor_preview_resolution;
	//(void)camsensor_config_camclk_po(camsensor_camclk_po_hz);
	return TRUE;
} /* camsensor_mt9d112_mu2m0yu_video_config */

/*===========================================================================

FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_POWER_UP

DESCRIPTION
Power up the sensor.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/

void camsensor_SETi_SID130B_power_up(void)
{
} /* camsensor_SID130B_mu2m0yu_power_up */

/*===========================================================================

FUNCTION      camsensor_mt9d112_mu2m0yu_POWER_DOWN

DESCRIPTION
Power down the sensor, using I2C driver.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/

void camsensor_SETi_SID130B_power_down(void)
{
 	SETi_SID130B_i2c_write_byte(0x00, 0x00); 
 	SETi_SID130B_i2c_write_byte(0x03, 0x02); 
} /* camsensor_SID130B_mu2m0yu_power_down */

/*==========================================================================

FUNCTION   CAMSENSOR_MT9D112_MU2M0YU_SET_EFFECT

DESCRIPTION
Applies special effects(solarize,sepia,negative,monochrome)

DEPENDENCIES
None

RETURN VALUE
CAMERA_SUCCESS        if successful
CAMERA_NOT_SUPPORTED  if sensor does not support function;
CAMERA_FAILED         if failed

SIDE EFFECTS
None 

===============================================================================*/

camera_ret_code_type camsensor_SETi_SID130B_set_effect(int8 effect)
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;

	if ( camsensor_SETi_SID130B_effect == effect )
	{
		return CAMERA_SUCCESS;
	}
	
	if ( (effect<CAMERA_EFFECT_MIN_MINUS_1) && (effect>CAMERA_EFFECT_MAX_PLUS_1) )
	{
		return CAMERA_INVALID_PARM;
	}
	switch(effect)
	{
		case CAMERA_EFFECT_OFF: 
			if(!(SETi_SID130B_i2c_write_byte(0x00,0x04)))
			{
				ret_val = CAMERA_FAILED;
			}
			if(!(SETi_SID130B_i2c_write_byte(0xD9,0x00)))
			{
				ret_val = CAMERA_FAILED;
			}
			break;

		case CAMERA_EFFECT_NEGATIVE: 
			if(!(SETi_SID130B_i2c_write_byte(0x00,0x04)))
			{
				ret_val = CAMERA_FAILED;
			}
			if(!(SETi_SID130B_i2c_write_byte(0xD9,0x10)))
			{
				ret_val = CAMERA_FAILED;
			}
			break;

		case CAMERA_EFFECT_MONO:   
			if(!(SETi_SID130B_i2c_write_byte(0x00,0x04)))
			{
				ret_val = CAMERA_FAILED;
			}
			if(!(SETi_SID130B_i2c_write_byte(0xD9,0x40)))
			{
				ret_val = CAMERA_FAILED;
			}
			break;

		case CAMERA_EFFECT_SEPIA:
			if(!(SETi_SID130B_i2c_write_byte(0x00,0x04)))
			{
				ret_val = CAMERA_FAILED;
			}
			if(!(SETi_SID130B_i2c_write_byte(0xD9,0x80)))
			{
				ret_val = CAMERA_FAILED;
			}
			break;

		default:
	  		ret_val = CAMERA_NOT_SUPPORTED;
	  		break;
	}

	camsensor_SETi_SID130B_effect = effect;

	return ret_val;
} /* camsensor_mt9d112_mu2m0yu_set_effect() */

/*==================================================================================== 
FUNCTION   CAMSENSOR_MT9D112_MU2M0YU_SET_ANTIBANDING

DESCRIPTION
Changes the antibanding

DEPENDENCIES
None

RETURN VALUE
CAMERA_SUCCESS        if successful
CAMERA_NOT_SUPPORTED  if sensor does not support function;
CAMERA_FAILED         if failed

SIDE EFFECTS
======================================================================================*/
camera_ret_code_type camsensor_SETi_SID130B_set_antibanding(int8 antibanding)
{
	MSG_FATAL("camsensor_SETi_SID130B_set_antibanding",0,0,0);
  	return CAMERA_SUCCESS;
}
/*============================================================================

FUNCTION      CAMSENSOR_SID130B_MU2M0YU_SET_NIGHTMODE

DESCRIPTION
set the nightmode on the sensor.

DEPENDENCIES
None

RETURN VALUE:
CAMERA_SUCCESS        if successful
CAMERA_NOT_SUPPORTED  if sensor does not support function;
CAMERA_FAILED         if failed

SIDE EFFECTS:
None

============================================================================*/
camera_ret_code_type camsensor_SETi_SID130B_set_nightmode (int8 mode)
{
	MSG_FATAL("camsensor_SETi_SID130B_set_nightmode",0,0,0);
	return (CAMERA_SUCCESS);
} /* camsensor_mt9d112_mu2m0yu_set_brightness() */
/*============================================================================

FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_SET_BRIGHTNESS

DESCRIPTION
Changes the brightness on the sensor.

DEPENDENCIES
None

RETURN VALUE:
CAMERA_SUCCESS        if successful
CAMERA_NOT_SUPPORTED  if sensor does not support function;
CAMERA_FAILED         if failed

SIDE EFFECTS:
None

============================================================================*/
/** {<PR:5023>  --  F3060366   2009/09/25
* brightness  mode change (from Ybrightness->EV )  
**/
camera_ret_code_type camsensor_SETi_SID130B_set_brightness(int8 value)
{
	MSG_FATAL("camsensor_SETi_SID130B_set_brightness",0,0,0);
	return (CAMERA_SUCCESS);
} /* camsensor_mt9d112_mu2m0yu_set_brightness() */
/**********</PR:5023>}**********************/

/*===========================================================================


FUNCTION   CAMSENSOR_MT9D112_MU2M0YU_SET_EXPOSURE_MODE

DESCRIPTION
Changes the Exposure Control Algorithm (Frame Average ,Spot Metering , Center Weighted)

DEPENDENCIES
None

RETURN VALUE
CAMERA_SUCCESS        if successful
CAMERA_NOT_SUPPORTED  if sensor does not support function;
CAMERA_FAILED         if failed

SIDE EFFECTS


================================================================================*/

camera_ret_code_type camsensor_SETi_SID130B_set_exposure_mode(int8 mode)
{
	MSG_FATAL("camsensor_SETi_SID130B_set_exposure_mode",0,0,0);
	return CAMERA_SUCCESS;
}


/*===========================================================================


FUNCTION   CAMSENSOR_MT9D112_MU2M0YU_SET_EV_COMPENSATION

DESCRIPTION
Apply EV Compensation

DEPENDENCIES
None

RETURN VALUE
CAMERA_SUCCESS        if successful
CAMERA_NOT_SUPPORTED  if sensor does not support function;
CAMERA_FAILED         if failed

SIDE EFFECTS


================================================================================*/

camera_ret_code_type camsensor_SETi_SID130B_set_ev_compensation(int32 compensation)
{
	camera_ret_code_type ret_val = CAMERA_SUCCESS;
	int16  parmCurNum = (int16)(compensation>>16);
	int32 max_parmVal = 12;
	int32 min_parmVal =-12;
	int32  parmVal= parmCurNum;
	uint32 new_luma_target;

	MSG_FATAL("camsensor_SETi_SID130B_set_ev_compensation!",0,0,0);
	if ( parmVal > max_parmVal )
	{
		ret_val =  CAMERA_INVALID_PARM;
	}
	else if ( parmVal < min_parmVal )
	{
		ret_val =  CAMERA_INVALID_PARM;
	}

	new_luma_target = (uint32) parmVal + max_parmVal;

	if( new_luma_target >= MAX_EV_COMP)
	{
		ret_val=CAMERA_INVALID_PARM;
	}
	else
	{
		switch(new_luma_target)
		{
			case 8:    // -4 EV
        		SETi_SID130B_i2c_write_byte(0x12, 0x34);
        		SETi_SID130B_i2c_write_byte(0x13, 0x34);
        		SETi_SID130B_i2c_write_byte(0x14, 0x34);
       			break;

		    case 9:    // -3 EV
		        SETi_SID130B_i2c_write_byte(0x12, 0x44);
		        SETi_SID130B_i2c_write_byte(0x13, 0x44);
		        SETi_SID130B_i2c_write_byte(0x14, 0x44);
		        break;

		    case 10:    // -2 EV
		        SETi_SID130B_i2c_write_byte(0x12, 0x54);
		        SETi_SID130B_i2c_write_byte(0x13, 0x54);
		        SETi_SID130B_i2c_write_byte(0x14, 0x54);
		        break;

		    case 11:    // -1 EV
		        SETi_SID130B_i2c_write_byte(0x12, 0x64);
		        SETi_SID130B_i2c_write_byte(0x13, 0x64);
		        SETi_SID130B_i2c_write_byte(0x14, 0x64);
		        break;

		    case 12:   // +0 EV
		       /* SETi_SID130B_i2c_write_byte(0x12, 0x8c);//0x74
		        SETi_SID130B_i2c_write_byte(0x13, 0x8c);
		        SETi_SID130B_i2c_write_byte(0x14, 0x8c);
		        */
		        SETi_SID130B_i2c_write_byte(0x12, 0x7c);//0x74
		        SETi_SID130B_i2c_write_byte(0x13, 0x7c);
		        SETi_SID130B_i2c_write_byte(0x14, 0x7c);
		        break;

		    case 13:    // +1 EV
		        SETi_SID130B_i2c_write_byte(0x12, 0xa0);//0x84
		        SETi_SID130B_i2c_write_byte(0x13, 0xa0);
		        SETi_SID130B_i2c_write_byte(0x14, 0xa0);
		        break;

		    case 14:    // +2 EV
		        SETi_SID130B_i2c_write_byte(0x12, 0xb0);//0x94
		        SETi_SID130B_i2c_write_byte(0x13, 0xb0);
		        SETi_SID130B_i2c_write_byte(0x14, 0xb0);
		        break;

		    case 15:    // +3 EV
		        SETi_SID130B_i2c_write_byte(0x12, 0xc0);//0xa4
		        SETi_SID130B_i2c_write_byte(0x13, 0xc0);
		        SETi_SID130B_i2c_write_byte(0x14, 0xc0);
		        break;

		    case 16:    // +4 EV
		        SETi_SID130B_i2c_write_byte(0x12, 0xd0);//0xb4
		        SETi_SID130B_i2c_write_byte(0x13, 0xd0);
		        SETi_SID130B_i2c_write_byte(0x14, 0xd0);
		        break;

		    default:
		    	break;
	    }
	}
	
	return ret_val;
}

/*
===========================================================================
FUNCTION      CAMSENSOR_MT9M112_SET_CONTRAST

DESCRIPTION
Changes the contrast on the sensor.

DEPENDENCIES
None

RETURN VALUE
CAMERA_SUCCESS        if successful
CAMERA_NOT_SUPPORTED  if sensor does not support function;
CAMERA_FAILED         if failed

SIDE EFFECTS
None

============================================================================*/
camera_ret_code_type camsensor_SETi_SID130B_set_contrast(int8 contrast)
{
	MSG_FATAL("camsensor_SETi_SID130B_set_contrast",0,0,0);
	return CAMERA_SUCCESS;
} /* camsensor_SID130B_set_contrast() */
/**********</PR:5679>}**********************/
/*===========================================================================

FUNCTION   CAMSENSOR_MT9D112_MU2M0YU_REGISTER 

DESCRIPTION  
Exports the sensor specific functions to SENSOR

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void camsensor_SETi_SID130B_register(camsensor_function_table_type *camsensor_function_table_ptr)
{
	camsensor_function_table_ptr->camsensor_start                 = camsensor_SETi_SID130B_start;
	camsensor_function_table_ptr->camsensor_video_config          = camsensor_SETi_SID130B_video_config;
	camsensor_function_table_ptr->camsensor_raw_snapshot_config   = camsensor_SETi_SID130B_raw_snapshot_config;
	camsensor_function_table_ptr->camsensor_snapshot_config       = camsensor_SETi_SID130B_snapshot_config;
	camsensor_function_table_ptr->camsensor_power_up              = camsensor_SETi_SID130B_power_up;
	camsensor_function_table_ptr->camsensor_power_down            = camsensor_SETi_SID130B_power_down;
	camsensor_function_table_ptr->camsensor_set_frame_rate        = camsensor_SETi_SID130B_set_frame_rate;  

	camsensor_function_table_ptr->camsensor_set_effect            = camsensor_SETi_SID130B_set_effect;
	camsensor_function_table_ptr->camsensor_set_antibanding       = camsensor_SETi_SID130B_set_antibanding; 

	camsensor_function_table_ptr->camsensor_set_brightness        = camsensor_SETi_SID130B_set_brightness;
	camsensor_function_table_ptr->camsensor_set_contrast          = camsensor_SETi_SID130B_set_contrast;

	camsensor_function_table_ptr->camsensor_set_exposure_mode     = camsensor_SETi_SID130B_set_exposure_mode;
	camsensor_function_table_ptr->camsensor_set_ev_compensation   = camsensor_SETi_SID130B_set_ev_compensation; 
	camsensor_function_table_ptr->camsensor_set_wb                = camsensor_SETi_SID130B_set_wb;
	//camsensor_function_table_ptr->camsensor_set_nightmode            = camsensor_SETi_SID130B_set_nightmode;
} /* camsensor_mt9d112_mu2m0yu_register */

/*===========================================================================

FUNCTION   CAMSENSOR_MT9D112_MU2M0YU_SETUP_CAMCTRL_TBL 

DESCRIPTION  
Initialize sensor specific function pointers and static params in camctrl_tbl

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void camsensor_SETi_SID130B_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
	camctrl_tbl_ptr->asf_5x5_is_supported       	= FALSE; 

	camctrl_tbl_ptr->get_snapshot_frame_per_sec 	= camsensor_SETi_SID130B_get_snapshot_fps;
}

/*===========================================================================

FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_GET_SNAPSHOT_FPS

DESCRIPTION   Get Snapshot FPS compared to input preview fps.

DEPENDENCIES
None

INPUT
uint16 fps - Frames Per Second rate in Q8 format for preview.

RETURN VALUE
uint16       FPS for snapshot

SIDE EFFECTS
None

===========================================================================*/

LOCAL uint16 camsensor_SETi_SID130B_get_snapshot_fps(uint16 fps)
{
	/* In the current driver, the snapshot fps is always 11 */

	MSG_FATAL("camsensor_SETi_SID130B_get_snapshot_fps",0,0,0);
	fps = 15 * Q8;

	return fps;
}

/*===========================================================================

FUNCTION      I2C_WRITE_WORD

DESCRIPTION
16-bit I2C write.

DEPENDENCIES
None

RETURN VALUE
TRUE  - I2C write successful
FALSE - I2C write failed


SIDE EFFECTS
None

===========================================================================*/

static boolean SETi_SID130B_i2c_write_byte(uint8 reg, byte data) 
{
	static uint8 writebyte;
	uint8 i;

	writebyte  = data;

	camsensor_i2c_command.addr.reg = reg;
	camsensor_i2c_command.buf_ptr = &writebyte;
	camsensor_i2c_command.len = 1;
  
	for (i = 0; i < 3; ++i)
	{
		if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
		{
			MSG_FATAL("SETi_SID130B_i2c_write_byte: OK %x,%x",reg,data,0);
			return TRUE;
		}
	}
  
  	MSG_FATAL("SETi_SID130B_i2c_write_byte: false %x,%x",reg,data,0);
	return FALSE;
} /* camif_i2c_write_byte */

/*===========================================================================

FUNCTION      mt9d112_mu2m0yu_I2C_READ_WORD

DESCRIPTION
16-bit I2C read.

DEPENDENCIES
None

RETURN VALUE
TRUE - I2C read successful
FALSE - I2C read failed


SIDE EFFECTS
None

===========================================================================*/
static boolean SETi_SID130B_i2c_read_byte( uint8 reg, byte *data) 
{
	static byte readbyte; 
	uint8 i;
	
	if (data == NULL)
	{
		MSG_FATAL("data is null pointer",0,0,0);
		return FALSE;
	}

	camsensor_i2c_command.addr.reg = reg;
	camsensor_i2c_command.buf_ptr = &readbyte;
	camsensor_i2c_command.len = 1;

	for (i = 0; i < 3; ++i)
	{
		if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
		{
			MSG_FATAL("SETi_SID130B_i2c_read_byte: OK %x",reg,0,0);
			*data = readbyte;
			return TRUE;
		}
	}
  
  	MSG_FATAL("SETi_SID130B_i2c_read_byte: false %x",reg,0,0);
	return FALSE;  
}


/* =================================================================================

FUNCTION   CAMSENSOR_MT9D112_MU2MOYU_SET_WB

DESCRIPTION
  Changes the White balance acording to the lighting conditions .

DEPENDENCIES
  None

RETURN VALUE
  CAMERA_SUCCESS        if successful
  CAMERA_NOT_SUPPORTED  if sensor does not support function;
  CAMERA_FAILED         if failed

SIDE EFFECTS

=====================================================================================*/

camera_ret_code_type camsensor_SETi_SID130B_set_wb(int8 wb)
{
  	camera_ret_code_type ret_val = CAMERA_SUCCESS;
  	static int8 m_wb = CAMERA_WB_MIN_MINUS_1;

  	MSG_FATAL("camsensor_SETi_SID130B_set_wb!",0,0,0);

  	if ( m_wb == wb )
  	{
		return ret_val;
  	}

  
	switch(wb)
	{
		case CAMERA_WB_AUTO:  //AUTO
			break;

		case CAMERA_WB_DAYLIGHT:  //sunny
			break;

		case CAMERA_WB_CLOUDY_DAYLIGHT:  //cloudy
			break;

		case   CAMERA_WB_INCANDESCENT:  //indoor office
			break;

		case   CAMERA_WB_FLUORESCENT:  // indoor home
			break;

		case   CAMERA_WB_TWILIGHT:  // 
			break;

		default:
			ret_val = CAMERA_NOT_SUPPORTED;
			break;
	}

  	m_wb = wb;

  	return ret_val;
}


/*===========================================================================

FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_SET_FRAME_RATE

DESCRIPTION   Set the Frames Per Second Rate in Preview mode.

DEPENDENCIES
None

INPUT
uint16 fps - New Frames Per Second rate in Q8 format

RETURN VALUE
camera_ret_code_type
CAMERA_SUCCESS       - if we changed frame rate successfully
CAMERA_NOT_SUPPORTED - function not implemented
CAMERA_FAILED        - if frame rate change failed

SIDE EFFECTS
Changes the sensor input clock rate.

LIMTATIONS
The sensor master clock rate must be between MIN_PIXEL_CLOCK
and MAX_PIXEL_CLOCK. Therefore, the frame rates are limited
accordingly, and CAMERA_FAILED will be returned if these limits
are passed.

===========================================================================*/
static camera_ret_code_type camsensor_SETi_SID130B_set_frame_rate(uint16 fps)
{
	return CAMERA_SUCCESS;
}

/*===========================================================================

FUNCTION      mt9d112_mu2m0yu_SET_SENSOR_MODE

DESCRIPTION   Set the camera sensor to preview mode or snapshot mode.
In each mode, the camera sensor runs in a different resolution
and different register values are used.

DEPENDENCIES
None

INPUT
sensor_mode_type sensor_mode - Specifies wheter the sensor should now be in
			 preview mode or in snapshot mode.

RETURN VALUE
TRUE  - if the camera mode has changed successfully.
FALSE - if the camera mode has not changed successfully.

SIDE EFFECTS
Changes the sensor image resolution.
Changes the sensor context.
Changes the sensor power consumtion.
Changes the sensor data rate.

LIMTATIONS

===========================================================================*/

#endif /* FEATURE_CAMERA,USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU */

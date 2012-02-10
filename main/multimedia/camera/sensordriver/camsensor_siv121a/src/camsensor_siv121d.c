/*============================================================================

Camera Interface Device Driver Source File for 
Micron MT9D113 2 Mega Pixel CMOS sensor

DESCRIPTION
This file contains the definitions needed for the camera interface
device driver.

Copyright (c) 2006 bySIMCOM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================
INCLUDE FILES
============================================================================*/

#include "camsensor.h"

#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_MICRON_SIV121D_0M3)
#include "camsensor_siv121d.h"
#include "camerai.h"
#include "ipl.h"
#include "msg.h"
#include "i2c.h"
#include "clk.h"
#include "chromatix_6550.h"

#define CAMSENSOR_SIV121D_RESET_PIN         GPIO_OUTPUT_10
/*============================================================================
	TYPE DEFINITIONS 
============================================================================*/
typedef enum {
SENSOR_PREVIEW_MODE,
SENSOR_SNAPSHOT_MODE
} sensor_mode_type;

typedef struct register_address_value_pair_struct {
uint16 register_address;
uint16 register_value;
} register_address_value_pair;

/*============================================================================
	DATA DECLARATIONS
============================================================================*/
static const char camsensor_siv121d_0m3_sensor_name[] = "SIV121D VGA";
static int8 camsensor_siv121d_0m3_brightness;
static int8 camsensor_siv121d_0m3_exposure_mode;
/* static int8 camsensor_siv121d_0m3_contrast; */


/*****************************************************************************
*                          RUN TIME VARIABLES
****************************************************************************/

/*============================================================================
	  MACRO DEFINITIONS
============================================================================*/
/*  I2C Address of the Sensor  */
#define  SIV121D_0M3_I2C_SLAVE_ID  0x66

/* Sensor Core Registers */
#define  SIV121D_0M3_MODEL_ID     (0xDE)

/***************************************************************
* The following defines are used as UI parameters
***************************************************************/


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
static void    camsensor_siv121d_0m3_register(camsensor_function_table_type *camsensor_function_table_ptr);
static void    camsensor_siv121d_0m3_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
static camera_ret_code_type camsensor_siv121d_0m3_set_frame_rate(uint16 fps);
static camera_ret_code_type camsensor_siv121d_0m3_set_effect(int8 effect);
static boolean initialize_siv121d_0m3_registers(void);
static boolean siv121d_0m3_set_lens_roll_off(void);
static uint16 camsensor_siv121d_0m3_get_snapshot_fps(uint16);
camera_ret_code_type camsensor_siv121d_0m3_set_wb(int8 wb);

static uint16 max_fps = 25 * Q8;
static camera_effect_type 		camsensor_siv121d_0m3_effect = CAMERA_EFFECT_OFF;
static sensor_mode_type 		siv121d_0m3_sensor_mode;
static CAMIF_InputFormatType 	format_preview,format_snapshot;

static boolean camsensor_SIV121D_ycbcr_i2c_write_byte(uint8 reg, uint8 data);
static boolean camsensor_SIV121D_ycbcr_i2c_read_byte(uint8 reg, uint8 *data);
#if 1
static void SIV121D_config_window(uint16 startx,uint16 starty,uint16 width, uint16 height)
{
	camsensor_SIV121D_ycbcr_i2c_write_byte(0x00,0x04);//page 0

	camsensor_SIV121D_ycbcr_i2c_write_byte(0xc0,(byte)((startx>>8)<<6)|((width>>8)<<4)|((starty>>8)<<3)|((height>>8)<<2));//10 40 F0 QVGA WINDOW 00 A0 80 160*128
    camsensor_SIV121D_ycbcr_i2c_write_byte(0xc1,(byte)startx);
    camsensor_SIV121D_ycbcr_i2c_write_byte(0xc2,(byte)width);
    camsensor_SIV121D_ycbcr_i2c_write_byte(0xc3,(byte)starty);
    camsensor_SIV121D_ycbcr_i2c_write_byte(0xc4,(byte)height);
}
#endif
static register_address_value_pair siv121d_reg_settings_array[] =
{
	//Sensor On  
	//PMU

	{0x00, 0x01},
	{0x03, 0x0a},

	{0x00, 0x01},
	{0x03, 0x0a},
	
	
	{0x00, 0x00},
	{0x03, 0x04},
	{0x10, 0x85},
	{0x11, 0x11},

	//Sensor On 
	{0x00, 0x01},
	{0x04, 0x00}, 
	{0x06, 0x04}, 

	{0x10,	0x46}, 
	{0x11,	0x23}, 
	{0x12,	0x21},

	{0x17,	0x86},
	{0x18,	0x00},

	{0x20,	0x00},
	{0x21,	0x05},
	{0x22,	0x01},
	{0x23,	0x69},

	{0x40,	0x0F}, 
	{0x41,	0x90},
	{0x42,	0xd2}, 
	{0x43,	0x00},

	//AE
	{0x00, 0x02 },
	{0x10, 0x80 },
	{0x11, 0x0d },      
	{0x12, 0x78 },// D65 target 
	{0x14, 0x70 },// A target
	{0x34, 0x96 },
	{0x40, 0x40 },// Max x6
	{0x44, 0x08 },

	//AWB   
	{0x00, 0x03 },
	{0x10, 0xc0 },
	{0x11, 0xc0 },
	{0x12, 0x80 },
	{0x13, 0x81 },//Cr target
	{0x14, 0x7e },//Cb target
	{0x15, 0xff },// R gain Top
	{0x16, 0x80 },// R gain bottom 
	{0x17, 0xff },// B gain Top
	{0x18, 0x70 },// B gain bottom 0x80
	{0x19, 0x94 },// Cr top value 0x90
	{0x1a, 0x6c },// Cr bottom value 0x70
	{0x1b, 0x94 },// Cb top value 0x90
	{0x1c, 0x6c },// Cb bottom value 0x70
	{0x1d, 0x94 },// 0xa0
	{0x1e, 0x6c },// 0x60
	{0x20, 0xe8 },// AWB luminous top value
	{0x21, 0x30 },// AWB luminous bottom value 0x20
	{0x22, 0xa4 },
	{0x23, 0x20 },
	{0x25, 0x20 },
	{0x26, 0x0f },
	{0x27, 0x08 },//BRTSRT 
	{0x28, 0x0f },//BRTEND
	{0x29, 0xba },//BRTRGNBOT 
	{0x2a, 0xa0 },//BRTBGNTOP

	{0x40, 0x01 },
	{0x41, 0x04 },
	{0x42, 0x08 },
	{0x43, 0x10 },
	{0x44, 0x12 },
	{0x45, 0x35 },
	{0x46, 0x64 },

	{0x63, 0xa0 },//R D30 to D20
	{0x64, 0xc4 },//B D30 to D20
	{0x65, 0xa0 },//R D20 to D30
	{0x66, 0xc4 },//B D20 to D30

	//IDP   
	{0x00, 	0x04},
	{0x10,	0xff},
	{0x11,	0x0D},
	{0x12,	0x9D},
	{0x14,	0x00},

	//DPCBNR
	{0x18,	0xfe},	//DPCNRCTRL
	{0x19,	0x00},	//DPCTHV
	{0x1A,	0x00},	//DPCTHVSLP
	{0x1B,	0x00},	//DPCTHVDIFSRT
	{0x1C,	0x0f},	//DPCTHVDIFSLP
	{0x1d,	0xFF},	//DPCTHVMAX
	               
	{0x1E,	0x06},	//BNRTHV  0c
	{0x1F,	0x06},	//BNRTHVSLPN 10
	{0x20,	0x20},	//BNRTHVSLPD
	{0x21,	0x00},	//BNRNEICNT
	{0x22,	0x10},	//STRTNOR
	{0x23,	0x40},	//STRTDRK
    {0x24,	0x00},
	//Gamma
	{0x31, 0x04 },//0x08
	{0x32, 0x0b },//0x10
	{0x33, 0x24 },//0x1B
	{0x34, 0x49 },//0x37
	{0x35, 0x66 },//0x4D
	{0x36, 0x7C },//0x60
	{0x37, 0x8D },//0x72
	{0x38, 0x9B },//0x82
	{0x39, 0xAA },//0x91
	{0x3a, 0xb6 },//0xA0
	{0x3b, 0xca },//0xBA
	{0x3c, 0xdc },//0xD3
	{0x3d, 0xef },//0xEA

	//Shading Register Setting                  
	{0x40, 0x0a },                         
	{0x41, 0x77 },                         
	{0x42, 0x66 },                         
	{0x43, 0x55 },                         
	{0x44, 0x22 },//left R gain[7:4], right R gain[3:0]                         
	{0x45, 0x11 },//top R gain[7:4], bottom R gain[3:0]                          
	{0x46, 0x00 },//left G gain[7:4], right G gain[3:0]       
	{0x47, 0x00 },//top G gain[7:4], bottom G gain[3:0]       
	{0x48, 0x00 },//left B gain[7:4], right B gain[3:0]       
	{0x49, 0x10 },//top B gain[7:4], bottom B gain[3:0]      
	{0x4a, 0x04 },//X-axis center high[3:2], Y-axis center high[1:0]     
	{0x4b, 0x50 },//X-axis center low[7:0]                      
	{0x4c, 0xe0 },//Y-axis center low[7:0]                     
	{0x4d, 0x80 },//Shading Center Gain
	{0x4e, 0x00 },//Shading R Offset   
	{0x4f, 0x00 },//Shading Gr Offset  
	{0x50, 0x00 },//Shading B Offset  

	//Interpolation
	{0x60,  0x7f},
	{0x61,  0x08},	//INTCTRL outdoor

	//Color matrix (D65) - Daylight
	{0x71, 0x3B },   
	{0x72, 0xCE },   
	{0x73, 0xF7 },   
	{0x74, 0x13 },   
	{0x75, 0x25 },   
	{0x76, 0x08 },   
	{0x77, 0xF2 },  
	{0x78, 0xC7 },   
	{0x79, 0x47 },   

	//Color matrix (D20) - A
	{0x83, 0x3c },//0x3c     
	{0x84, 0xce },//0xc6     
	{0x85, 0xf7 },//0xff   
	{0x86, 0x12 },//0x12    
	{0x87, 0x25 },//0x24     
	{0x88, 0x09 },//0x0a     
	{0x89, 0xed },//0xed   
	{0x8a, 0xb6 },//0xc2     
	{0x8b, 0x5d },//0x51

	{0x8c, 0x10 },//CMA select     

	//G Edge 
	{0x90, 0x18 },//Upper gain
	{0x91, 0x18 },//down gain
	{0x92, 0x22 },//[7:4] upper coring [3:0] down coring
	{0x9c, 0x38 },//edge suppress start
	{0x9d, 0x50 },//edge suppress slope

	{0xa9, 0x11 },
	{0xaa, 0x11 },

	{0xb9, 0x30 },//nightmode 38 at gain 0x48 5fps
	{0xba, 0x44 },//nightmode 80 at gain 0x48 5fps

	{0xde, 0x80 },

	{0xe5, 0x15 },//MEMSPDA
	{0xe6, 0x02 },//MEMSPDB
	{0xe7, 0x04 },//MEMSPDC

	//Sensor On
	{0x00,	0x01},
	{0x03,	0x01}, //SNR Enable
};

/*============================================================================
	  EXPORTED FUNCTIONS
============================================================================*/

uint8 camsensor_SIV121D_ycbcr_active(void)
{
	return 0;
}

uint8 camsensor_SIV121D_ycbcr_unactive(void)
{
	return 1;
}

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121D_0M3_INIT

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
boolean camsensor_siv121d_0m3_init(camsensor_function_table_type *camsensor_function_table_ptr,
				   camctrl_tbl_type              *camctrl_tbl_ptr)
{
	uint8 gc_model_id;
	
	MSG_FATAL("    camsensor_siv121d_0m3_init     ",0,0,0);
	/* Input MCLK = 24MHz */
	camsensor_camclk_po_hz = 24000000;
	//camsensor_camclk_po_hz = camsensor_config_camclk_po(camsensor_camclk_po_hz);

	//gpio_tlmm_config(CAMSENSOR_SIV121D_RESET_PIN);
    
   // gpio_out(CAMSENSOR_SIV121D_RESET_PIN,1);
   // clk_busy_wait(2*1000);
   // gpio_out(CAMSENSOR_SIV121D_RESET_PIN,0);
   // clk_busy_wait(100*1000);
   // gpio_out(CAMSENSOR_SIV121D_RESET_PIN,1);

	camsensor_SIV121D_ycbcr_i2c_write_byte(0x00,0x01);
	camsensor_SIV121D_ycbcr_i2c_write_byte(0x03,0x0a);
    clk_busy_wait(2*1000);

	/* Configure I2C parameters */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;//I2C_BUS_HW_CTRLtao.bu20110414CHANGE
	camsensor_i2c_command.slave_addr = SIV121D_0M3_I2C_SLAVE_ID;
	/*lint -save -e655 */
	camsensor_i2c_command.options    = (i2c_options_type) I2C_REG_DEV | I2C_STOP_START_BEFORE_READ;
	
	siv121d_0m3_sensor_mode = SENSOR_PREVIEW_MODE;

	camsensor_preview_resolution  = CAMSENSOR_QTR_SIZE;
	camsensor_snapshot_resolution = CAMSENSOR_FULL_SIZE;

	
	/* The Sensor is indeed Micron MT9D112 */
	/* Initialize Sensor registers */
	camsensor_SIV121D_ycbcr_i2c_write_byte(0x00,0x00);
	
	camsensor_SIV121D_ycbcr_i2c_read_byte(0x01,&gc_model_id);
	MSG_FATAL("camsensor  id is    0x%x",gc_model_id,0,0);
	if(gc_model_id != SIV121D_0M3_MODEL_ID)
	{
        MSG_FATAL("camsensor  id is    0x%x",gc_model_id,0,0);
        return FALSE;
	}

	/* Register function table: */
	camsensor_siv121d_0m3_register(camsensor_function_table_ptr);

	/* Setup camctrl_tbl */
	camsensor_siv121d_0m3_setup_camctrl_tbl(camctrl_tbl_ptr);

	MSG_FATAL("Camsensor siv121d Init Success!",0, 0, 0);

	return TRUE;
} /* camsensor_siv121d_0m3_init */

/*===========================================================================
FUNCTION      CAMSENSOR_SIV121D_0M3_INIT_CAMSENSOR_INFO

DESCRIPTION
Initialize the camera sensor info;

DEPENDENCIES
msm_id must be set to a vaild id before calling this function.

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/

void camsensor_siv121d_0m3_init_camsensor_info()
{    
	MSG_FATAL("    camsensor_siv121d_0m3_init_camsensor_info     ",0,0,0);
	camsensor_info.pclk_invert	=FALSE;
	camsensor_info.preview_dummy_pixels = 0;
	format_preview	= CAMIF_YCbCr_Cr_Y_Cb_Y;
	format_snapshot	= CAMIF_YCbCr_Cr_Y_Cb_Y;
	camsensor_camclk_po_hz	=24000000;

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
}/* camsensor_siv121d_0m3_init_camsensor_info */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121D_0M3_START 

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

boolean camsensor_siv121d_0m3_start(camsensor_static_params_type *camsensor_params)
{
 	MSG_FATAL("    camsensor_siv121d_0m3_start     ",0,0,0);
	camsensor_siv121d_0m3_init_camsensor_info();

	if ( camsensor_params == NULL)
	{
		return(FALSE);
	}

	/* Initialize CAMIF operation mode */
	camsensor_params->camif_config.SyncMode    = CAMIF_APS;
	camsensor_params->camif_config.HSyncEdge   = CAMIF_High;
	camsensor_params->camif_config.VSyncEdge   = CAMIF_Low;//CAMIF_High;
	camsensor_params->camif_efs_config.EFS_EOL = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOL = 0x0000;
	camsensor_params->camif_efs_config.EFS_EOF = 0x0000;
	camsensor_params->camif_efs_config.EFS_SOF = 0x0000;

	/* ------------------  Sensor-specific Config -------------- */
	/* Make/model of sensor */
	camsensor_params->sensor_model  = CAMSENSOR_MICRON_SIV121D_0M3;

	/* CCD or CMOS */
	camsensor_params->sensor_type   = CAMSENSOR_CMOS;

	/* Sensor output data format */
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;

	/* BAYER or YCbCr */
	camsensor_params->output_format = CAMSENSOR_YCBCR;

	/* What is the maximum FPS that can be supported by this sensor in video mode? */
	camsensor_params->max_video_fps   = 20 * Q8;

	/* Application assigned FPS in video mode */
	camsensor_params->video_fps       = 20 * Q8;

	/* Snapshot mode operation */
	camsensor_params->max_preview_fps = 25 * Q8;
	camsensor_params->nightshot_fps   = 10 * Q8;

	/* May be assigned with max_preview_fps or nightshot_fps. */
	camsensor_params->preview_fps     = camsensor_params->max_preview_fps; 

	/* VFE's perception of Sensor output capability */

	/* Full size dimensions - 1600x1200 */
	camsensor_params->full_size_width  = 360;//480;//640;
	camsensor_params->full_size_height = 480;//640;//480; /* 2 Extra Lines for VFE padding */


	/* Quarter size dimensions - The same size. */
	camsensor_params->qtr_size_width  = 360;
	camsensor_params->qtr_size_height = 480;

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
	camsensor_params->frame_rate_array[0].fps = (uint16) (25*256.0); /* Q8 */
	camsensor_params->frame_rate_array[0].use_in_auto_frame_rate = TRUE;

	camsensor_params->frame_rate_array[1].fps = (uint16) (25*256.0); /* Q8 */
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
} /* camsensor_mt9d112_mu2m0yu_start */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121D_0M3_SNAPSHOT_CONFIG

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

boolean camsensor_siv121d_0m3_snapshot_config
(
camsensor_static_params_type *camsensor_params
) 
{
	MSG_FATAL("    camsensor_siv121d_0m3_snapshot_config     ",0,0,0);

	/* Sensor output data format */
	camsensor_params->format = CAMIF_YCbCr_Cr_Y_Cb_Y;

	/* Set the current dimensions */
	camsensor_params->camsensor_width = camsensor_params->full_size_width;
	camsensor_params->camsensor_height = camsensor_params->full_size_height;
	/* CAMIF frame */
	camsensor_params->camif_frame_config.pixelsPerLine = camsensor_params->full_size_width*2;
	camsensor_params->camif_frame_config.linesPerFrame = camsensor_params->full_size_height;
	
	/* CAMIF window */
	camsensor_params->camif_window_width_config.firstPixel = 0;
	camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camsensor_width*2 - 1;
	camsensor_params->camif_window_height_config.firstLine = 0;
	camsensor_params->camif_window_height_config.lastLine = camsensor_params->camsensor_height - 1;
	//SIV121D_config_window(0,0,camsensor_params->camsensor_width,camsensor_params->camsensor_height);
	//rex_sleep(5);
	camsensor_current_resolution = camsensor_snapshot_resolution;
	return TRUE;
} /* camsensor_siv121d_0m3_snapshot_config */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121D_0M3_RAW_SNAPSHOT_CONFIG

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

boolean camsensor_siv121d_0m3_raw_snapshot_config
(
camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	MSG_FATAL("camsensor_siv121d_0m3_raw_snapshot_config",0,0,0);
	return TRUE;
} /* camsensor_siv121d_0m3_raw_snapshot_config */


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121D_0M3_VIDEO_CONFIG

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

boolean camsensor_siv121d_0m3_video_config
(
camsensor_static_params_type  *camsensor_params		 /* Camera sensor config params */
)
{

	if (!initialize_siv121d_0m3_registers())
	{
	    return FALSE;
	}
	
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

	MSG_FATAL("camsensor_siv121d_0m3_video_config  camsensor_params->camsensor_width %d",camsensor_params->camsensor_width,0,0);
	/* CAMIF window */
	camsensor_params->camif_window_width_config.firstPixel = 0;
	camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel + (camsensor_params->camsensor_width)*2 - 1;
	camsensor_params->camif_window_height_config.firstLine = 0;
	camsensor_params->camif_window_height_config.lastLine = camsensor_params->camif_window_height_config.firstLine + camsensor_params->camsensor_height - 1;

	SIV121D_config_window(100,0,camsensor_params->camsensor_width,camsensor_params->camsensor_height);
	rex_sleep(5);
	//SIV121D_config_window();
	//	camsensor_SIV121A_ycbcr_write_sensor (camsensor_preview_resolution);//yty add
	camsensor_current_resolution = camsensor_preview_resolution;
	return TRUE;
} /* camsensor_siv121d_0m3_video_config */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121D_0M3_POWER_UP

DESCRIPTION
Power up the sensor.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/

void camsensor_siv121d_0m3_power_up(void)
{
} /* camsensor_siv121d_0m3_power_up */

/*===========================================================================

FUNCTION      camsensor_siv121d_0m3_POWER_DOWN

DESCRIPTION
Power down the sensor, using I2C driver.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/

void camsensor_siv121d_0m3_power_down(void)
{
	return;
} /* camsensor_siv121d_0m3_power_down */

/*==========================================================================

FUNCTION   CAMSENSOR_SIV121D_0M3_SET_EFFECT

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
camera_ret_code_type camsensor_siv121d_0m3_set_effect(int8 effect)
{
	MSG_FATAL("    camsensor_siv121d_0m3_set_effect   effect %d ",effect,0,0);
	
	switch ((camera_effect_type)effect)
	{
		case CAMERA_EFFECT_OFF:
		{
			/*camsensor_SIV121D_ycbcr_i2c_write_byte(0x23,0x00);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x2d,0x0a); // 0x08
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x20,0xff);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xd2,0x90);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x73,0x00);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x77,0x54);// 54-110307
			
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb3,0x40);// 38 36-110307
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb4,0x80);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xba,0x00);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xbb,0x00); */
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb6, 0x00); 
			break;
		}
		
		case CAMERA_EFFECT_MONO:
		{
			/*camsensor_SIV121D_ycbcr_i2c_write_byte(0x23,0x02);	
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x2d,0x0a);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x20,0xff);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xd2,0x90);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x73,0x00);

			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb3,0x40);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb4,0x80);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xba,0x00);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xbb,0x00);*/
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb6, 0x40); 
			break;
		}
		
		case CAMERA_EFFECT_NEGATIVE:
		{
			/*camsensor_SIV121D_ycbcr_i2c_write_byte(0x23,0x01);		
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x2d,0x0a);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x20,0xff);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xd2,0x90);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x73,0x00);

			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb3,0x40);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb4,0x80);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xba,0x00);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xbb,0x00);*/
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
		    camsensor_SIV121D_ycbcr_i2c_write_byte(0xb6, 0x10); 
			break;
		}
		
		case CAMERA_EFFECT_SEPIA:
		{
			/*camsensor_SIV121D_ycbcr_i2c_write_byte(0x23,0x02);		
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x2d,0x0a);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x20,0xff);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xd2,0x90);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x73,0x00);

			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb3,0x40);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb4,0x80);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xba,0xd0);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xbb,0x28);*/
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb6, 0x80); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb7, 0x60); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xb8, 0xcc); 
			break;
		}
		
		case CAMERA_EFFECT_SOLARIZE:
		case CAMERA_EFFECT_PASTEL:
		case CAMERA_EFFECT_MOSAIC:
		case CAMERA_EFFECT_RESIZE:
		default:
			return CAMERA_NOT_SUPPORTED;
	}

	camsensor_siv121d_0m3_effect = (camera_effect_type)effect;

	camera_timed_wait(20);

	return CAMERA_SUCCESS;
}
/*==================================================================================== 
FUNCTION   CAMSENSOR_SIV121D_0M3_SET_ANTIBANDING

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
camera_ret_code_type camsensor_siv121d_0m3_set_antibanding(int8 antibanding)
{

	MSG_FATAL("    camsensor_siv121d_0m3_set_antibanding   antibanding %d ",antibanding,0,0);

	switch ((camera_antibanding_type)antibanding)
	{
		case CAMERA_ANTIBANDING_60HZ:
		
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00  ,0x01);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x20  ,0x00);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x21  ,0x05);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x23  ,0x69);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00  ,0x02);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x34  ,0x96);

			break;

		case CAMERA_ANTIBANDING_50HZ:
			
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00  ,0x01);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x20  ,0x00);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x21  ,0x05);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x23  ,0x69);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00  ,0x02);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x34  ,0x96);
			break;

		default:
			return CAMERA_NOT_SUPPORTED;
	}
	return CAMERA_SUCCESS;
}


/*============================================================================

FUNCTION      CAMSENSOR_SIV121D_0M3_SET_BRIGHTNESS

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
camera_ret_code_type camsensor_siv121d_0m3_set_brightness(int8 brightness)
{

	MSG_FATAL("    camsensor_siv121d_0m3_set_brightness   brightness %d ",brightness,0,0);

	if (brightness < CAMSENSOR_MIN_BRIGHTNESS)
	{
		return CAMERA_INVALID_PARM;
	}
	else if (brightness > CAMSENSOR_MAX_BRIGHTNESS)
	{
		return CAMERA_INVALID_PARM;
	}
	else
	{
		switch(brightness)
		{
		    case 1:
	        {
				camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
				camsensor_SIV121D_ycbcr_i2c_write_byte(0xab, 0xa0); 
				break;
	        }
	        
		    case 2:
			{
				camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
				camsensor_SIV121D_ycbcr_i2c_write_byte(0xab, 0x90); 
				break;
			}
		         
		    case 3:
			{
				camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
				camsensor_SIV121D_ycbcr_i2c_write_byte(0xab, 0x00); 
			    break;
			}

		    case 4:
			{
				camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
				camsensor_SIV121D_ycbcr_i2c_write_byte(0xab, 0x10); 
				break;
			}  
		         
		    case 5:
	        {
			    camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			    camsensor_SIV121D_ycbcr_i2c_write_byte(0xab, 0x20); 
				break;
			}
			
		    default:
		            break;
		}
		camera_timed_wait(10);

	}
	camsensor_siv121d_0m3_brightness = brightness;

	return CAMERA_SUCCESS;

} /* camsensor_siv121d_0m3_set_brightness() */


/*===========================================================================


FUNCTION   CAMSENSOR_siv121d_0m3_SET_EXPOSURE_MODE

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

camera_ret_code_type camsensor_siv121d_0m3_set_exposure_mode(int8 mode)
{
	MSG_FATAL("    camsensor_siv121d_0m3_set_exposure_mode   mode %d ",mode,0,0);
	
	return CAMERA_SUCCESS;
}


/*===========================================================================


FUNCTION   CAMSENSOR_siv121d_0m3_SET_EV_COMPENSATION

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

camera_ret_code_type camsensor_siv121d_0m3_set_ev_compensation(int32 compensation)
{
	MSG_FATAL("    camsensor_siv121d_0m3_set_ev_compensation   compensation %d ",compensation,0,0);
	return CAMERA_NOT_SUPPORTED;
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
camera_ret_code_type camsensor_siv121d_0m3_set_contrast(int8 contrast)
{
	MSG_FATAL("    camsensor_siv121d_0m3_set_contrast   contrast %d ",contrast,0,0);

	switch(contrast)
	{
		case 1:
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xa8, 0x0e);  
			break;
			
		case 2:
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xa8, 0x10); 
			break;
			
		case 3:
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xa8, 0x12); 
			break;
			
		case 4:
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xa8, 0x14); 
			break;
			
		default:
			break;
	}
	
	return CAMERA_SUCCESS;
} /* camsensor_mt9m112_set_contrast() */

/*===========================================================================

FUNCTION     CAMSENSOR_SIV121D_0M3_nightmode

DESCRIPTION
              
PARAMETERS
              
DEPENDENCIES

RETURN VALUE
              

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camsensor_siv121d_0m3_set_nightmode(int8 nightmode)
{
	uint8 regval = 0;
	MSG_FATAL("~~~~~~~~camsensor_siv121d_0m3_set_nightmode~~~~~~~~~~~~,nightmode=%d", nightmode,0,0);
	camsensor_SIV121D_ycbcr_i2c_read_byte(0x20, &regval);
	
	switch(nightmode)
	{
		case 1:   //indoor
		{
			
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x02); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x11, 0x0d);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xab, 0x00); 
		}
		break;
		
		case 2: //outdoor
		{
			
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x02); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x11, 0x0d);
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xab, 0x00); 

		}
		break;
		
		case 3: //nightmode
		{
			
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x02); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x11, 0x10); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x04); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0xab, 0x10); 
		}
		break;
		
		default:
			break;
	}
	
	camera_timed_wait(20);
	return CAMERA_SUCCESS;
}
/*===========================================================================

FUNCTION   CAMSENSOR_SIV121D_0M3_REGISTER 

DESCRIPTION  
Exports the sensor specific functions to SENSOR

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void camsensor_siv121d_0m3_register(camsensor_function_table_type *camsensor_function_table_ptr)
{
	camsensor_function_table_ptr->camsensor_start                 = camsensor_siv121d_0m3_start;
	camsensor_function_table_ptr->camsensor_video_config          = camsensor_siv121d_0m3_video_config;
	camsensor_function_table_ptr->camsensor_raw_snapshot_config   = camsensor_siv121d_0m3_raw_snapshot_config;
	camsensor_function_table_ptr->camsensor_snapshot_config       = camsensor_siv121d_0m3_snapshot_config;
	camsensor_function_table_ptr->camsensor_power_up              = camsensor_siv121d_0m3_power_up;
	camsensor_function_table_ptr->camsensor_power_down            = camsensor_siv121d_0m3_power_down;
	camsensor_function_table_ptr->camsensor_set_frame_rate        = camsensor_siv121d_0m3_set_frame_rate;  

	camsensor_function_table_ptr->camsensor_set_effect            = camsensor_siv121d_0m3_set_effect;
	camsensor_function_table_ptr->camsensor_set_antibanding       = camsensor_siv121d_0m3_set_antibanding; 

	camsensor_function_table_ptr->camsensor_set_brightness        = \
	  camsensor_siv121d_0m3_set_brightness;
	 camsensor_function_table_ptr->camsensor_set_contrast          = \
	  camsensor_siv121d_0m3_set_contrast;

	camsensor_function_table_ptr->camsensor_set_exposure_mode = \
	  camsensor_siv121d_0m3_set_exposure_mode;
	camsensor_function_table_ptr->camsensor_set_ev_compensation=\
	  camsensor_siv121d_0m3_set_ev_compensation; 
	camsensor_function_table_ptr->camsensor_set_wb= \
	                              camsensor_siv121d_0m3_set_wb;

} /* camsensor_siv121d_0m3_register */

/*===========================================================================

FUNCTION   CAMSENSOR_SIV121D_0M3_SETUP_CAMCTRL_TBL 

DESCRIPTION  
Initialize sensor specific function pointers and static params in camctrl_tbl

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void camsensor_siv121d_0m3_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
	camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 
	camctrl_tbl_ptr->la_is_supported              = FALSE;
	camctrl_tbl_ptr->cs_is_supported              = FALSE;
	camctrl_tbl_ptr->iso_is_supported             = FALSE;
	camctrl_tbl_ptr->enable_rolloff_correction    = FALSE;
	camctrl_tbl_ptr->hjr_bayer_filtering_enable   = FALSE;
	camctrl_tbl_ptr->nightshot_is_supported       = FALSE;

	camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_siv121d_0m3_get_snapshot_fps;
}

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121D_0M3_GET_SNAPSHOT_FPS

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

static uint16 camsensor_siv121d_0m3_get_snapshot_fps(uint16 fps)
{
	/* In the current driver, the snapshot fps is always 11 */
	fps = 15 * Q8;

	return fps;
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
===========================================================================================*/

camera_ret_code_type camsensor_siv121d_0m3_set_wb(int8 wb)
{
	byte temp_data;
	MSG_FATAL("   camsensor_siv121d_0m3_set_wb  wb  %d  ",wb,0,0);
	camsensor_SIV121D_ycbcr_i2c_read_byte(0x22, &temp_data);
	
	switch((camera_wb_type)wb)
	{
		case CAMERA_WB_AUTO:
		{
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x03); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x10, 0xd0); 
			break;
		}
		case CAMERA_WB_INCANDESCENT:
		{
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x03); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x10, 0xd5); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x72, 0x80); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x73, 0x90); 
			break;
		}
		case CAMERA_WB_FLUORESCENT:
		{
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x03); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x10, 0xd5); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x72, 0xb8); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x73, 0xcc); 
			break;
		}
		case CAMERA_WB_DAYLIGHT:
		{
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x03); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x10, 0xd5); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x72, 0xd8); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x73, 0x90); 
			break;
		}
		case CAMERA_WB_CLOUDY_DAYLIGHT:
		{
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x00, 0x03); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x10, 0xd5); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x72, 0xb4); 
			camsensor_SIV121D_ycbcr_i2c_write_byte(0x73, 0x74); 
			break;
		}
		
		case CAMERA_WB_TWILIGHT:
		case CAMERA_WB_SHADE :
		case CAMERA_WB_CUSTOM:
		default:
			return CAMERA_NOT_SUPPORTED;
	}//end of switch
	
	return CAMERA_SUCCESS;
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
static boolean camsensor_SIV121D_ycbcr_i2c_read_byte(uint8 reg, uint8 *data) 
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
			//ERR("camsensor_SIV121A_ycbcr_i2c_read_byte: OK %x",reg,0,0);
			return TRUE;
		}
	}

	MSG_FATAL("camsensor_SIV121D_ycbcr_i2c_read_byte: false %x",reg,0,0);
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
static boolean camsensor_SIV121D_ycbcr_i2c_write_byte(uint8 reg, uint8 data) 
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
			MSG_FATAL("camsensor_SIV121D_ycbcr_i2c_write_byte: OK %x,%x",reg,data,0);
			return TRUE;
		}
	}
	MSG_FATAL("camsensor_SIV121D_ycbcr_i2c_write_byte: false %x,%x",reg,data,0);
	return FALSE;
} /* camsensor_SIV121A_ycbcr_i2c_write_byte */

/*===========================================================================

FUNCTION      INITIALIZE_mt9d113_mu2m0_REGISTERS

DESCRIPTION
16-bit I2C read.

DEPENDENCIES
None

RETURN VALUE
TRUE  - Register initialization successful.
FALSE - Register initialization failed.


SIDE EFFECTS
None

if(!(mt9d113_mu2m0_i2c_write_word( , )))
{return FALSE;}

===========================================================================*/
static boolean initialize_siv121d_0m3_registers(void)
{
	int32 i, array_length;
	MSG_FATAL("    initialize_siv121d_0m3_registers   ",0,0,0);
	
	camera_timed_wait(20);

	/* PLL Setup End   */
	array_length = sizeof (siv121d_reg_settings_array) / sizeof (siv121d_reg_settings_array[0]);
	/* Configure sensor for Preview mode and Snapshot mode */
	for (i = 0; i < array_length; i++) 
	{
		if (camsensor_SIV121D_ycbcr_i2c_write_byte(siv121d_reg_settings_array[i].register_address, siv121d_reg_settings_array[i].register_value) == FALSE)
		{
			return(FALSE);
		}
	}

	camera_timed_wait(100);

	return TRUE;
} /* end of initialize_siv121d_0m3_registers. */

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121D_0M3_SET_FRAME_RATE

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
static camera_ret_code_type camsensor_siv121d_0m3_set_frame_rate(uint16 fps)
{
	MSG_FATAL("    initialize_siv121d_0m3_registers   fps %d",fps,0,0);
	if (fps > max_fps) 
	{
		return CAMERA_FAILED;
	}

	return CAMERA_SUCCESS;
}

#endif /* FEATURE_CAMERA,USE_CAMSENSOR_MICRON_siv121d_0m3 */

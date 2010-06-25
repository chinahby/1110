
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
03/18/09  chai   Fixed Rvct compiler warning
01/16/08  SV     Fix for CR # 168242
12/18/08  rs     Waiting for 1 frame time in set_effect api for changes in effect requested to take place. 
11/27/08  cg     Fix for CR # 164535
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

#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_GC0309)
#include "camsensor_gc0309.h"
#include "camerai.h"
#include "ipl.h"
#include "msg.h"
#include "i2c.h"
#include "clk.h"
#include "chromatix_6550.h"

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
static const char camsensor_gc0309_sensor_name[] = "GC0309 2.0 MP YUV";
static int8 camsensor_gc0309_brightness;
static int8 camsensor_gc0309_exposure_mode;
/* static int8 camsensor_gc0309_contrast; */


/*****************************************************************************
*                          RUN TIME VARIABLES
****************************************************************************/

/*============================================================================
	  MACRO DEFINITIONS
============================================================================*/
	/* SENSOR READ/WRITE ID */
#define GC0309_WRITE_ID 							(0x42) 
#define GC0309_READ_ID								(0x43)

/* SENSOR CHIP VERSION */
#define GC0309_SENSOR_ID							 (0xa0)


#define GC0309_SET_PAGE0 	camsensor_gc0309_i2c_write_byte(0xfe, 0x00)
#define GC0309_SET_PAGE1 	camsensor_gc0309_i2c_write_byte(0xfe, 0x01)


/* Micron MT9D112 Registers and their values */
/* Sensor Core Registers */
#define  REG_MT9D112_MU2M0YU_MODEL_ID 0x3000
#define  MT9D112_MU2M0YU_MODEL_ID     0x1580

/*  SOC Registers Page 1  */
#define  REG_MT9D112_MU2M0YU_SENSOR_RESET     0x301A
#define  REG_MT9D112_MU2M0YU_STANDBY_CONTROL  0x3202
#define  REG_MT9D112_MU2M0YU_MCU_BOOT         0x3386

boolean   sensor_night_mode=FALSE, MPEG4_encode_mode=FALSE, g_bMJPEG_mode = FALSE;
boolean sensor_cap_state = FALSE;
boolean gc_effect_on = FALSE;
boolean g_banding_is_60Hz = FALSE;     

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
static void    camsensor_gc0309_register(camsensor_function_table_type *camsensor_function_table_ptr);
static void    camsensor_gc0309_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
static boolean camsensor_gc0309_i2c_write_byte(uint8 offset, uint8 data);
static boolean camsensor_gc0309_i2c_read_byte(uint8 offset, uint8 *data);
static camera_ret_code_type camsensor_gc0309_set_frame_rate(uint16 fps);
static camera_ret_code_type camsensor_gc0309_set_effect(int8 effect);
static boolean camsensor_gc0309_set_sensor_mode(sensor_mode_type sensor_mode);
static boolean initialize_gc0309_registers(void);
static boolean camsensor_set_lens_roll_off(void);
static uint16 camsensor_gc0309_get_snapshot_fps(uint16);
camera_ret_code_type camsensor_gc0309_set_wb(int8 wb);

static uint16 current_fps=30*Q8;
static boolean initial_effect=FALSE;
static uint16 max_fps = 30 * Q8;
static camera_effect_type camsensor_gc0309_effect = CAMERA_EFFECT_OFF;
static sensor_mode_type mt9d112_mu2m0yu_sensor_mode;
static CAMIF_InputFormatType format_preview,format_snapshot;

#ifdef T_FFA
#error code not present
#endif

float camsensor_gc0309_ev_compensation_table[] =
{
0.2500,    /* 2^EV_Comp = 2^-12/6 */
0.2806,    /* 2^EV_Comp = 2^-11/6 */
0.3150,    /* 2^EV_Comp = 2^-10/6 */
0.3536,    /* 2^EV_Comp = 2^-9/6  */
0.3969,    /* 2^EV_Comp = 2^-8/6  */
0.4454,    /* 2^EV_Comp = 2^-7/6  */
0.5000,    /* 2^EV_Comp = 2^-6/6  */
0.5612,    /* 2^EV_Comp = 2^-5/6  */
0.6299,    /* 2^EV_Comp = 2^-4/6  */
0.7071,    /* 2^EV_Comp = 2^-3/6  */
0.7937,    /* 2^EV_Comp = 2^-2/6  */
0.8909,    /* 2^EV_Comp = 2^-1/6  */
1.0000,    /* 2^EV_Comp = 2^0     */
1.1225,    /* 2^EV_Comp = 2^1/6   */
1.2599,    /* 2^EV_Comp = 2^2/6   */
1.4142,    /* 2^EV_Comp = 2^3/6   */
1.5874,    /* 2^EV_Comp = 2^4/6   */
1.7818,    /* 2^EV_Comp = 2^5/6   */
2.0000,    /* 2^EV_Comp = 2^6/6   */
2.2449,    /* 2^EV_Comp = 2^7/6   */
2.5198,    /* 2^EV_Comp = 2^8/6   */
2.8284,    /* 2^EV_Comp = 2^9/6   */
3.1748,    /* 2^EV_Comp = 2^10/6  */
3.5636,    /* 2^EV_Comp = 2^11/6  */
4.0000     /* 2^EV_Comp = 2^12/6  */
};

LOCAL register_address_value_pair gc0309_preview_snapshot_mode_reg_settings_array[] =
{
{0x338C, 0x2703}, 
{0x3390, 800},	  /* Output Width (P) = 640 */
{0x338C, 0x2705}, 
{0x3390, 600},	  /* Output Height (P) = 480 */
{0x338C, 0x2707}, 
{0x3390, 0x0640}, /* Output Width (S) = 1600 */
{0x338C, 0x2709}, 
{0x3390, 0x04C0}, /* Output Height (S) = 1216 */
{0x338C, 0x270D}, 
{0x3390, 0x0000}, /* Row Start (P) = 0 */ 
{0x338C, 0x270F}, 
{0x3390, 0x0000}, /* Column Start (P) = 0*/
{0x338C, 0x2711}, 
{0x3390, 0x04BD}, /* Row End (P) = 1213 */
{0x338C, 0x2713}, 
{0x3390, 0x064D}, /* Column End (P) = 1613 */
{0x338C, 0x2715}, 
{0x3390, 0x0000}, /* Extra Delay (P) = 0 */
{0x338C, 0x2717}, 
{0x3390, 0x2111}, /* Row Speed (P) = 8465 */
{0x338C, 0x2719}, 
{0x3390, 0x046C}, /* Read Mode (P) = 1132 */
{0x338C, 0x271B}, 
{0x3390, 0x024F}, /* Sensor_Sample_Time_pck(P) = 591 */
{0x338C, 0x271D}, 
{0x3390, 0x0102}, /* Sensor_Fine_Correction(P) = 258 */
{0x338C, 0x271F}, 
{0x3390, 0x0279}, /* Sensor_Fine_IT_min(P) = 633 */
{0x338C, 0x2721}, 
{0x3390, 0x0155}, /* Sensor_Fine_IT_max_margin(P) = 341 */
{0x338C, 0x2723}, 
{0x3390, 650},	  /* Frame Lines (P) = 650 */
{0x338C, 0x2725}, 
{0x3390, 0x0824}, /* Line Length (P) = 2084 */
{0x338C, 0x2727}, 
{0x3390, 0x2020}, 
{0x338C, 0x2729}, 
{0x3390, 0x2020}, 
{0x338C, 0x272B}, 
{0x3390, 0x1020}, 
{0x338C, 0x272D}, 
{0x3390, 0x2007}, 
{0x338C, 0x272F}, 
{0x3390, 0x0000}, /* Row Start(S) = 0 */
{0x338C, 0x2731}, 
{0x3390, 0x0004}, /* Column Start(S) = 4 */
{0x338C, 0x2733}, 
{0x3390, 0x04BF}, /* Row End(S) = 1215 */
{0x338C, 0x2735}, 
{0x3390, 0x064B}, /* Column End(S) = 1611 */
{0x338C, 0x2737}, 
{0x3390, 0x04CE}, /* Extra Delay(S) = 1230 */ 
{0x338C, 0x2739}, 
{0x3390, 0x2111}, /* Row Speed(S) = 8465*/
{0x338C, 0x273B}, 
{0x3390, 0x0024}, /* Read Mode(S) = 36*/
{0x338C, 0x273D}, 
{0x3390, 0x0120}, /* Sensor sample time pck(S) = 288*/
{0x338C, 0xA206},
{0x3390, 0x0032}, /* Luma Target */

{0x338C, 0x2741}, 
{0x3390, 0x0169}, /* Sensor_Fine_IT_min(P) = 361 */

{0x338C, 0x2745}, 
{0x3390, 0x04FF}, /* Frame Lines(S) = 1279*/
{0x338C, 0x2747}, 
{0x3390, 0x0824}, /* Line Length(S) = 2084*/
{0x338C, 0x2751}, 
{0x3390, 0x0000}, /* Crop_X0(P) = 0*/
{0x338C, 0x2753}, 
{0x3390, 0x0320}, /* Crop_X1(P) = 800*/
{0x338C, 0x2755}, 
{0x3390, 0x0000}, /* Crop_Y0(P) = 0*/
{0x338C, 0x2757}, 
{0x3390, 0x0258}, /* Crop_Y1(P) = 600*/
{0x338C, 0x275F},
{0x3390, 0x0000}, /* Crop_X0(S) = 0*/
{0x338C, 0x2761}, 
{0x3390, 0x0640}, /* Crop_X1(S) = 1600*/
{0x338C, 0x2763}, 
{0x3390, 0x0000}, /* Crop_Y0(S) = 0*/
{0x338C, 0x2765}, 
{0x3390, 0x04C0}, /* Crop_Y1(S) = 1216*/
{0x338C, 0x222E}, 
{0x3390, 0x00A0}, /* R9 Step = 160*/
{0x338C, 0xA408}, 
{0x3390, 0x001F}, 
{0x338C, 0xA409}, 
{0x3390, 0x0021}, 
{0x338C, 0xA40A}, 
{0x3390, 0x0025}, 
{0x338C, 0xA40B}, 
{0x3390, 0x0027}, 
{0x338C, 0x2411}, 
{0x3390, 0x00A0}, 
{0x338C, 0x2413}, 
{0x3390, 0x00C0}, 
{0x338C, 0x2415}, 
{0x3390, 0x00A0}, 
{0x338C, 0x2417}, 
{0x3390, 0x00C0}, 
{0x338C, 0x2799}, 
{0x3390, 0x6408}, /* MODE_SPEC_EFFECTS(P) */
{0x338C, 0x279B}, 
{0x3390, 0x6408}, /* MODE_SPEC_EFFECTS(S) */
{0x3214, 0x0F87}, /* SLEW RATE PCLK and DATA[0..7] */
};
LOCAL register_address_value_pair gc0309_noise_reduction_reg_settings_array[] =
{
{0x338C, 0xA76D}, 
{0x3390, 0x0003},
{0x338C, 0xA76E}, 
{0x3390, 0x0003}, 
{0x338C, 0xA76F}, 
{0x3390, 0}, 
{0x338C, 0xA770}, 
{0x3390, 21}, 
{0x338C, 0xA771}, 
{0x3390, 37}, 
{0x338C, 0xA772}, 
{0x3390, 63}, 
{0x338C, 0xA773}, 
{0x3390, 100}, 
{0x338C, 0xA774}, 
{0x3390, 128}, 
{0x338C, 0xA775}, 
{0x3390, 151}, 
{0x338C, 0xA776}, 
{0x3390, 169}, 
{0x338C, 0xA777}, 
{0x3390, 186}, 
{0x338C, 0xA778}, 
{0x3390, 199}, 
{0x338C, 0xA779}, 
{0x3390, 210}, 
{0x338C, 0xA77A}, 
{0x3390, 220}, 
{0x338C, 0xA77B}, 
{0x3390, 228}, 
{0x338C, 0xA77C}, 
{0x3390, 234}, 
{0x338C, 0xA77D}, 
{0x3390, 240}, 
{0x338C, 0xA77E}, 
{0x3390, 244}, 
{0x338C, 0xA77F}, 
{0x3390, 248}, 
{0x338C, 0xA780}, 
{0x3390, 252}, 
{0x338C, 0xA781}, 
{0x3390, 255}, 
{0x338C, 0xA782}, 
{0x3390, 0}, 
{0x338C, 0xA783}, 
{0x3390, 21}, 
{0x338C, 0xA784}, 
{0x3390, 37}, 
{0x338C, 0xA785}, 
{0x3390, 63}, 
{0x338C, 0xA786}, 
{0x3390, 100}, 
{0x338C, 0xA787}, 
{0x3390, 128}, 
{0x338C, 0xA788}, 
{0x3390, 151}, 
{0x338C, 0xA789}, 
{0x3390, 169}, 
{0x338C, 0xA78A}, 
{0x3390, 186}, 
{0x338C, 0xA78B}, 
{0x3390, 199}, 
{0x338C, 0xA78C}, 
{0x3390, 210}, 
{0x338C, 0xA78D}, 
{0x3390, 220}, 
{0x338C, 0xA78E}, 
{0x3390, 228}, 
{0x338C, 0xA78F}, 
{0x3390, 234}, 
{0x338C, 0xA790}, 
{0x3390, 240}, 
{0x338C, 0xA791}, 
{0x3390, 244}, 
{0x338C, 0xA793}, 
{0x3390, 252}, 
{0x338C, 0xA794}, 
{0x3390, 255}, 
{0x338C, 0xA103}, 
{0x3390, 6}, 
};



void camsensor_gc0309_i2c_init(void)
{
	/* Configure I2C parameters */
	camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
	camsensor_i2c_command.slave_addr = GC0309_WRITE_ID;
	/*lint -save -e655 */
	camsensor_i2c_command.options    = (i2c_options_type) (I2C_MEM_DEV | I2C_START_BEFORE_READ); 
}


static boolean camsensor_gc0309_i2c_write_byte(uint8 offset, uint8 data) 
{
	uint8 i;

	camsensor_i2c_command.addr.offset = (word)offset;
	camsensor_i2c_command.buf_ptr  = (byte *)(&data);
	camsensor_i2c_command.len      = 1;

	for (i = 0; i < 3; ++i) 
	{
		if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS) 
		{
			return TRUE;
		}
	}

	return FALSE;
}

static boolean camsensor_gc0309_i2c_read_byte(uint8 offset, uint8* data) 
{
	uint8  i;

	if (data == NULL) 
	{
		return FALSE;
	}

	camsensor_i2c_command.addr.offset = offset;
	camsensor_i2c_command.buf_ptr  = (byte *)(data);
	camsensor_i2c_command.len      = 1;

	for (i =0; i < 3; ++i) 
	{
		if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS) 
		{	
			return TRUE;
		}
	}
	return FALSE;
} 

/*============================================================================
	  EXPORTED FUNCTIONS
============================================================================*/

/*===========================================================================

FUNCTION      CAMSENSOR_MT9D112_MU2M0YU_INIT

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
boolean camsensor_gc0309_init(camsensor_function_table_type *camsensor_function_table_ptr,
				   camctrl_tbl_type              *camctrl_tbl_ptr)
{

#ifdef T_FFA
#error code not present
#endif

	/* Input MCLK = 24MHz */
	camsensor_camclk_po_hz = 24000000;
	camsensor_camclk_po_hz = camsensor_config_camclk_po(camsensor_camclk_po_hz);

	camsensor_gc0309_i2c_init();

	initialize_gc0309_registers();

	/* Register function table: */
	camsensor_gc0309_register(camsensor_function_table_ptr);

	/* Setup camctrl_tbl */
	camsensor_gc0309_setup_camctrl_tbl(camctrl_tbl_ptr);

	return TRUE;
} /* camsensor_gc0309_init */

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

void camsensor_gc0309_init_camsensor_info()
{

	camsensor_info.pclk_invert = TRUE;
	format_preview  = CAMIF_YCbCr_Cb_Y_Cr_Y;
	format_snapshot = CAMIF_YCbCr_Cb_Y_Cr_Y;

	camsensor_info.snapshot_width        = 1600; 
	camsensor_info.snapshot_height       = 1216;
	camsensor_info.snapshot_dummy_pixels =  0;
	camsensor_info.snapshot_dummy_lines  =  0;

	camsensor_info.preview_width         = 800;
	camsensor_info.preview_height        = 600;
	camsensor_info.preview_dummy_pixels  = 0;
	camsensor_info.preview_dummy_lines   = 0;

	switch (camsensor_info.msm_id) 
	{
		case MSM6246:
		case MSM6260:
#ifdef FEATURE_CAMIF_INTERNAL_TEST
		case QSC6085:
#endif
		case QSC6270:
		camsensor_info.pclk_invert = FALSE;
		break;
		case QSC1110:
		format_preview  = CAMIF_YCbCr_Cr_Y_Cb_Y;
		format_snapshot = CAMIF_YCbCr_Cr_Y_Cb_Y;
		break;
		case MSM6280:
		case MSM6290:
		camsensor_info.pclk_invert = FALSE;
		break;
		default:
		break;  
	}
}/* camsensor_gc0309_init_camsensor_info */

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

boolean camsensor_gc0309_start(camsensor_static_params_type *camsensor_params)
{
camsensor_gc0309_init_camsensor_info();

if ( camsensor_params == NULL) {
return(FALSE);
}

/* Initialize CAMIF operation mode */
camsensor_params->camif_config.SyncMode    = CAMIF_APS;
camsensor_params->camif_config.HSyncEdge   = CAMIF_High;
camsensor_params->camif_config.VSyncEdge   = CAMIF_High;
camsensor_params->camif_efs_config.EFS_EOL = 0x0000;
camsensor_params->camif_efs_config.EFS_SOL = 0x0000;
camsensor_params->camif_efs_config.EFS_EOF = 0x0000;
camsensor_params->camif_efs_config.EFS_SOF = 0x0000;

/* ------------------  Sensor-specific Config -------------- */
/* Make/model of sensor */
camsensor_params->sensor_model  = CAMSENSOR_GC0309_ID;

/* CCD or CMOS */
camsensor_params->sensor_type   = CAMSENSOR_CMOS;

/* Sensor output data format */
camsensor_params->format = CAMIF_YCbCr_Cb_Y_Cr_Y;

/* BAYER or YCbCr */
camsensor_params->output_format = CAMSENSOR_YCBCR;

/* What is the maximum FPS that can be supported by this sensor in video mode? */
camsensor_params->max_video_fps   = 30 * Q8;

/* Application assigned FPS in video mode */
camsensor_params->video_fps       = 30 * Q8;

/* Snapshot mode operation */
camsensor_params->max_preview_fps = 30 * Q8;
camsensor_params->nightshot_fps   = 15 * Q8;

/* May be assigned with max_preview_fps or nightshot_fps. */
camsensor_params->preview_fps     = camsensor_params->max_preview_fps; 

/* VFE's perception of Sensor output capability */

/* Full size dimensions - 1600x1200 */
camsensor_params->full_size_width  = 1600;
camsensor_params->full_size_height = 1202; /* 2 Extra Lines for VFE padding */


/* Quarter size dimensions - The same size. */
camsensor_params->qtr_size_width  = 800;
camsensor_params->qtr_size_height = 600;

camsensor_params->preview_dx_decimation = camsensor_params->full_size_width * Q12 / camsensor_params->qtr_size_width;
camsensor_params->preview_dy_decimation = camsensor_params->full_size_height * Q12 / camsensor_params->qtr_size_height;

camsensor_params->camsensor_width  = camsensor_params->qtr_size_width;
camsensor_params->camsensor_height = camsensor_params->qtr_size_height;

/* A pointer to the sensor name for EXIF tags */
camsensor_params->sensor_name = camsensor_gc0309_sensor_name;

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
camsensor_params->frame_rate_array[0].fps = (uint16) (30 * Q8);	/* Q8 */
camsensor_params->frame_rate_array[0].use_in_auto_frame_rate = FALSE;

camsensor_params->frame_rate_array[1].fps = (uint16) (15 * Q8);	/* Q8 */
camsensor_params->frame_rate_array[1].use_in_auto_frame_rate = FALSE;

camsensor_params->frame_rate_array[2].fps = (uint16) (7.5 * Q8); /* Q8 */
camsensor_params->frame_rate_array[2].use_in_auto_frame_rate = FALSE;

/* ------------  Default Misc Parmas Config ---------------- */
/* Does the sensor need/use a flash  */
#if defined FEATURE_WHITE_LED_FLASH || defined FEATURE_STROBE_FLASH
camsensor_params->support_auto_flash = TRUE;
#else
camsensor_params->support_auto_flash = FALSE;
#endif  

/* Sensor driver determines if the camif error needs to be
* ignored. Set the default to FALSE
*/
camsensor_params->ignore_camif_error = TRUE;

/* PCLK Invert */
camsensor_params->pclk_invert = camsensor_info.pclk_invert;

return TRUE;
} /* camsensor_gc0309_start */

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

boolean camsensor_gc0309_snapshot_config
(
camsensor_static_params_type *camsensor_params
) 
{
if ( camsensor_params == NULL) {
return(FALSE);
}

mt9d112_mu2m0yu_sensor_mode = SENSOR_SNAPSHOT_MODE;

/* Set special effects if any */
if (camsensor_gc0309_set_effect((uint16) camsensor_gc0309_effect) != CAMERA_SUCCESS) {
return FALSE;
}

if (camsensor_gc0309_set_sensor_mode(SENSOR_SNAPSHOT_MODE) == FALSE) {
return FALSE;
}

/* Sensor output fromat */
camsensor_params->format = format_snapshot;

/* Discard the first frame.*/
camsensor_params->discardFirstFrame = FALSE;

/* Set the current dimensions */
camsensor_params->camsensor_width  = camsensor_params->full_size_width;
camsensor_params->camsensor_height = camsensor_params->full_size_height;

/* VFE_CAMIF frame parameters */
camsensor_params->camif_frame_config.linesPerFrame = camsensor_info.snapshot_height;
camsensor_params->camif_frame_config.pixelsPerLine = camsensor_info.snapshot_width * 2;

/* Configure the VFE_CAMIF window parameters. */
camsensor_params->camif_window_width_config.firstPixel = camsensor_info.snapshot_dummy_pixels;
camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel +
 (camsensor_params->full_size_width * 2) - 1;

camsensor_params->camif_window_height_config.firstLine = camsensor_info.snapshot_dummy_lines;
camsensor_params->camif_window_height_config.lastLine  = camsensor_params->camif_window_height_config.firstLine +
 camsensor_params->full_size_height - 1;

#ifdef FEATURE_STROBE_FLASH
camsensor_params->epoch_line = camsensor_params->camif_window_height_config.lastLine;
#else
/* Disable the Epoch Interrupt by setting more number of lines than in a frame
*/
camsensor_params->epoch_line = 0xFFFF;
#endif

return TRUE;
} /* camsensor_gc0309_snapshot_config */

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

boolean camsensor_gc0309_raw_snapshot_config
(
camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
	
	return TRUE;
} /* camsensor_gc0309_raw_snapshot_config */


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

boolean camsensor_gc0309_video_config
(
camsensor_static_params_type  *camsensor_params		 /* Camera sensor config params */
)
{
	if ( camsensor_params == NULL) {
	return(FALSE);
	}

	mt9d112_mu2m0yu_sensor_mode = SENSOR_PREVIEW_MODE;

	camsensor_gc0309_set_effect(camsensor_gc0309_effect);

	if(camsensor_gc0309_set_frame_rate(max_fps)!=CAMERA_SUCCESS) 
	{
		return FALSE;    
	}

	if (camsensor_gc0309_set_sensor_mode(SENSOR_PREVIEW_MODE) == FALSE) {
	return FALSE;
	}
	/* Sensor output fromat */
	camsensor_params->format = format_preview;

	/* Set the current dimensions */  
	camsensor_params->camsensor_width  = camsensor_params->qtr_size_width;
	camsensor_params->camsensor_height = camsensor_params->qtr_size_height;   

	/* VFE_CAMIF frame parameters */
	camsensor_params->camif_frame_config.linesPerFrame = camsensor_info.preview_height;
	camsensor_params->camif_frame_config.pixelsPerLine = camsensor_info.preview_width * 2;

	/* Configure the VFE_CAMIF window parameters. */
	camsensor_params->camif_window_width_config.firstPixel = camsensor_info.preview_dummy_pixels;
	camsensor_params->camif_window_width_config.lastPixel  = camsensor_params->camif_window_width_config.firstPixel + 
	 (camsensor_params->camsensor_width * 2) - 1;

	camsensor_params->camif_window_height_config.firstLine = camsensor_info.preview_dummy_lines;
	camsensor_params->camif_window_height_config.lastLine  = camsensor_params->camif_window_height_config.firstLine +
	camsensor_params->camsensor_height - 1;

	/* Disable the Epoch Interrupt by setting more number of lines than 
	* in a frame 
	*/
	camsensor_params->epoch_line = 0xFFFF;

	return TRUE;
} /* camsensor_gc0309_video_config */

#ifdef T_FFA
#error code not present
#endif

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

void camsensor_gc0309_power_up(void)
{
	initial_effect= TRUE; //Apply special effects for the first time while entering camera
} /* camsensor_gc0309_power_up */

/*===========================================================================

FUNCTION      camsensor_gc0309_POWER_DOWN

DESCRIPTION
Power down the sensor, using I2C driver.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/

void camsensor_gc0309_power_down(void)
{

} /* camsensor_gc0309_power_down */


void camsensor_gc0309_gamma_Select(uint32 GammaLvl)
{
	switch(GammaLvl)
	{
		case 1:                                             //smallest gamma curve
			camsensor_gc0309_i2c_write_byte( 0x9F, 0x0B ); 
			camsensor_gc0309_i2c_write_byte( 0xA0, 0x16 ); 
			camsensor_gc0309_i2c_write_byte( 0xA1, 0x29 ); 
			camsensor_gc0309_i2c_write_byte( 0xA2, 0x3C ); 
			camsensor_gc0309_i2c_write_byte( 0xA3, 0x4F ); 
			camsensor_gc0309_i2c_write_byte( 0xA4, 0x5F ); 
			camsensor_gc0309_i2c_write_byte( 0xA5, 0x6F ); 
			camsensor_gc0309_i2c_write_byte( 0xA6, 0x8A ); 
			camsensor_gc0309_i2c_write_byte( 0xA7, 0x9F ); 
			camsensor_gc0309_i2c_write_byte( 0xA8, 0xB4 ); 
			camsensor_gc0309_i2c_write_byte( 0xA9, 0xC6 ); 
			camsensor_gc0309_i2c_write_byte( 0xAA, 0xD3 ); 
			camsensor_gc0309_i2c_write_byte( 0xAB, 0xDD );  
			camsensor_gc0309_i2c_write_byte( 0xAC, 0xE5 );  
			camsensor_gc0309_i2c_write_byte( 0xAD, 0xF1 ); 
			camsensor_gc0309_i2c_write_byte( 0xAE, 0xFA ); 
			camsensor_gc0309_i2c_write_byte( 0xAF, 0xFF ); 	
			break;
		case 2:			
			camsensor_gc0309_i2c_write_byte( 0x9F, 0x0E ); 
			camsensor_gc0309_i2c_write_byte( 0xA0, 0x1C ); 
			camsensor_gc0309_i2c_write_byte( 0xA1, 0x34 ); 
			camsensor_gc0309_i2c_write_byte( 0xA2, 0x48 ); 
			camsensor_gc0309_i2c_write_byte( 0xA3, 0x5A ); 
			camsensor_gc0309_i2c_write_byte( 0xA4, 0x6B ); 
			camsensor_gc0309_i2c_write_byte( 0xA5, 0x7B ); 
			camsensor_gc0309_i2c_write_byte( 0xA6, 0x95 ); 
			camsensor_gc0309_i2c_write_byte( 0xA7, 0xAB ); 
			camsensor_gc0309_i2c_write_byte( 0xA8, 0xBF );
			camsensor_gc0309_i2c_write_byte( 0xA9, 0xCE ); 
			camsensor_gc0309_i2c_write_byte( 0xAA, 0xD9 ); 
			camsensor_gc0309_i2c_write_byte( 0xAB, 0xE4 );  
			camsensor_gc0309_i2c_write_byte( 0xAC, 0xEC ); 
			camsensor_gc0309_i2c_write_byte( 0xAD, 0xF7 ); 
			camsensor_gc0309_i2c_write_byte( 0xAE, 0xFD ); 
			camsensor_gc0309_i2c_write_byte( 0xAF, 0xFF ); 
		break;
		case 3:
			camsensor_gc0309_i2c_write_byte( 0x9F, 0x10 ); 
			camsensor_gc0309_i2c_write_byte( 0xA0, 0x20 ); 
			camsensor_gc0309_i2c_write_byte( 0xA1, 0x38 ); 
			camsensor_gc0309_i2c_write_byte( 0xA2, 0x4E ); 
			camsensor_gc0309_i2c_write_byte( 0xA3, 0x63 ); 
			camsensor_gc0309_i2c_write_byte( 0xA4, 0x76 ); 
			camsensor_gc0309_i2c_write_byte( 0xA5, 0x87 ); 
			camsensor_gc0309_i2c_write_byte( 0xA6, 0xA2 ); 
			camsensor_gc0309_i2c_write_byte( 0xA7, 0xB8 ); 
			camsensor_gc0309_i2c_write_byte( 0xA8, 0xCA ); 
			camsensor_gc0309_i2c_write_byte( 0xA9, 0xD8 ); 
			camsensor_gc0309_i2c_write_byte( 0xAA, 0xE3 ); 
			camsensor_gc0309_i2c_write_byte( 0xAB, 0xEB ); 
			camsensor_gc0309_i2c_write_byte( 0xAC, 0xF0 ); 
			camsensor_gc0309_i2c_write_byte( 0xAD, 0xF8 ); 
			camsensor_gc0309_i2c_write_byte( 0xAE, 0xFD ); 
			camsensor_gc0309_i2c_write_byte( 0xAF, 0xFF ); 

			break;
		case 4:
			camsensor_gc0309_i2c_write_byte( 0x9F, 0x14 ); 
			camsensor_gc0309_i2c_write_byte( 0xA0, 0x28 ); 
			camsensor_gc0309_i2c_write_byte( 0xA1, 0x44 ); 
			camsensor_gc0309_i2c_write_byte( 0xA2, 0x5D ); 
			camsensor_gc0309_i2c_write_byte( 0xA3, 0x72 ); 
			camsensor_gc0309_i2c_write_byte( 0xA4, 0x86 ); 
			camsensor_gc0309_i2c_write_byte( 0xA5, 0x95 ); 
			camsensor_gc0309_i2c_write_byte( 0xA6, 0xB1 ); 
			camsensor_gc0309_i2c_write_byte( 0xA7, 0xC6 ); 
			camsensor_gc0309_i2c_write_byte( 0xA8, 0xD5 ); 
			camsensor_gc0309_i2c_write_byte( 0xA9, 0xE1 ); 
			camsensor_gc0309_i2c_write_byte( 0xAA, 0xEA ); 
			camsensor_gc0309_i2c_write_byte( 0xAB, 0xF1 ); 
			camsensor_gc0309_i2c_write_byte( 0xAC, 0xF5 ); 
			camsensor_gc0309_i2c_write_byte( 0xAD, 0xFB ); 
			camsensor_gc0309_i2c_write_byte( 0xAE, 0xFE ); 
			camsensor_gc0309_i2c_write_byte( 0xAF, 0xFF );
		break;
		case 5:								// largest gamma curve
			camsensor_gc0309_i2c_write_byte( 0x9F, 0x15 ); 
			camsensor_gc0309_i2c_write_byte( 0xA0, 0x2A ); 
			camsensor_gc0309_i2c_write_byte( 0xA1, 0x4A ); 
			camsensor_gc0309_i2c_write_byte( 0xA2, 0x67 ); 
			camsensor_gc0309_i2c_write_byte( 0xA3, 0x79 ); 
			camsensor_gc0309_i2c_write_byte( 0xA4, 0x8C ); 
			camsensor_gc0309_i2c_write_byte( 0xA5, 0x9A ); 
			camsensor_gc0309_i2c_write_byte( 0xA6, 0xB3 ); 
			camsensor_gc0309_i2c_write_byte( 0xA7, 0xC5 ); 
			camsensor_gc0309_i2c_write_byte( 0xA8, 0xD5 ); 
			camsensor_gc0309_i2c_write_byte( 0xA9, 0xDF ); 
			camsensor_gc0309_i2c_write_byte( 0xAA, 0xE8 ); 
			camsensor_gc0309_i2c_write_byte( 0xAB, 0xEE ); 
			camsensor_gc0309_i2c_write_byte( 0xAC, 0xF3 ); 
			camsensor_gc0309_i2c_write_byte( 0xAD, 0xFA ); 
			camsensor_gc0309_i2c_write_byte( 0xAE, 0xFD ); 
			camsensor_gc0309_i2c_write_byte( 0xAF, 0xFF );
			break;
		default:
		break;
	}
}

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
typedef enum
{
	CAM_EFFECT_ENC_NORMAL,
	CAM_EFFECT_ENC_GRAYSCALE,
	CAM_EFFECT_ENC_SEPIA,
	CAM_EFFECT_ENC_COLORINV,
	CAM_EFFECT_ENC_SEPIAGREEN,
	CAM_EFFECT_ENC_SEPIABLUE,
	CAM_EFFECT_ENC_COPPERCARVING,
	CAM_EFFECT_ENC_BLUECARVING,
	CAM_EFFECT_ENC_CONTRAST,
	CAM_EFFECT_ENC_EMBOSSMENT,
	CAM_EFFECT_ENC_SKETCH,
	CAM_EFFECT_ENC_GRAYINV,
	CAM_EFFECT_ENC_BLACKBOARD,
	CAM_EFFECT_ENC_JEAN,
	CAM_EFFECT_ENC_OIL,
	CAM_EFFECT_ENC_WHITEBOARD
}GC0309_EFFECT_TYPE;   //跟camif的类型不对应

camera_ret_code_type camsensor_gc0309_set_effect(int8 effect)
{
	
		uint32 ret = TRUE; 

	
		if(TRUE == sensor_cap_state)
			return TRUE;

		
		switch (effect)
		{	
			case CAM_EFFECT_ENC_NORMAL:
				camsensor_gc0309_i2c_write_byte(0x23,0x00);
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a); 
				camsensor_gc0309_i2c_write_byte(0x20,0xff);
				camsensor_gc0309_i2c_write_byte(0xd2,0x90);
				camsensor_gc0309_i2c_write_byte(0x73,0x00);
				camsensor_gc0309_i2c_write_byte(0x77,0x54);
				
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);
				camsensor_gc0309_i2c_write_byte(0xba,0x00);
				camsensor_gc0309_i2c_write_byte(0xbb,0x00);
		
				gc_effect_on = FALSE;
				break;		
			case CAM_EFFECT_ENC_GRAYSCALE:
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xff);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x90);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x00);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0x00);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x00);//fixed_Cr 	
				gc_effect_on = TRUE;	
				break;		
			case CAM_EFFECT_ENC_SEPIA:
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xff);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x90);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x00);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0xd0);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x28);//fixed_Cr 
	
				gc_effect_on = TRUE;
				break;		
			case CAM_EFFECT_ENC_COLORINV:	
				camsensor_gc0309_i2c_write_byte(0x23,0x01);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xff);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x90);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x00);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0x00);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x00);//fixed_Cr 
				gc_effect_on = TRUE;	
				break;		
			case CAM_EFFECT_ENC_SEPIAGREEN:
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xff);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x90);//close AEC
				camsensor_gc0309_i2c_write_byte(0x77,0x88);//promote the edge strength.
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0xc0);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0xc0);//fixed_Cr 
				gc_effect_on = TRUE;
				break;					
			case CAM_EFFECT_ENC_SEPIABLUE:
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xff);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x90);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x00);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0x50);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0xe0);//fixed_Cr 
	
				gc_effect_on = TRUE;
				break;								
	
			case CAM_EFFECT_ENC_COPPERCARVING:
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xbf);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x10);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x01);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0x51,0x40);//auto_pre_gain
				camsensor_gc0309_i2c_write_byte(0x52,0x40);//auto_post_gain
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0xc0);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x20);//fixed_Cr 
	
				gc_effect_on = TRUE;
				break;								
			case CAM_EFFECT_ENC_BLUECARVING:
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xbf);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x10);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x01);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0x51,0x40);//auto_pre_gain
				camsensor_gc0309_i2c_write_byte(0x52,0x40);//auto_post_gain
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0x60);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x00);//fixed_Cr 
	
				gc_effect_on = TRUE;
				break;								
			case CAM_EFFECT_ENC_CONTRAST://not yet
	
				gc_effect_on = TRUE;
				break;						
			case CAM_EFFECT_ENC_EMBOSSMENT:
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xbf);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x10);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x01);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0x51,0x40);//auto_pre_gain
				camsensor_gc0309_i2c_write_byte(0x52,0x40);//auto_post_gain
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0x00);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x00);//fixed_Cr 
				gc_effect_on = TRUE;
				break;			
			case CAM_EFFECT_ENC_SKETCH:
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xbf);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x10);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x01);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0x51,0x40);//auto_pre_gain
				camsensor_gc0309_i2c_write_byte(0x52,0x40);//auto_post_gain
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x98);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x06);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0x00);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x00);//fixed_Cr 
	
				gc_effect_on = TRUE;
				break;	
			case CAM_EFFECT_ENC_GRAYINV:
				camsensor_gc0309_i2c_write_byte(0x23,0x03);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xff);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x90);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x00);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x40);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0x80);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0x00);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x00);//fixed_Cr 		
	
				gc_effect_on = TRUE;
				break;										
			case CAM_EFFECT_ENC_BLACKBOARD:
				clk_busy_wait(300*1000);
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xbf);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x10);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x01);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0x51,0x40);//auto_pre_gain
				camsensor_gc0309_i2c_write_byte(0x52,0x40);//auto_post_gain
	
				camsensor_gc0309_i2c_write_byte(0xb3,0x98);//contrast 
				camsensor_gc0309_i2c_write_byte(0xb4,0xb0);//contrast center
				camsensor_gc0309_i2c_write_byte(0xba,0x00);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x00);//fixed_Cr 			
				gc_effect_on = TRUE;
				break;	
			case CAM_EFFECT_ENC_JEAN:
			case CAM_EFFECT_ENC_OIL:			
			case CAM_EFFECT_ENC_WHITEBOARD:
				camsensor_gc0309_i2c_write_byte(0x23,0x02);//special effect		
				camsensor_gc0309_i2c_write_byte(0x2d,0x0a);//advance the awb gain.
				camsensor_gc0309_i2c_write_byte(0x20,0xbf);//close gamma
				camsensor_gc0309_i2c_write_byte(0xd2,0x10);//close AEC
				camsensor_gc0309_i2c_write_byte(0x73,0x01);//edge_map
	
				camsensor_gc0309_i2c_write_byte(0x51,0x40);//auto_pre_gain
				camsensor_gc0309_i2c_write_byte(0x52,0x40);//auto_post_gain
				camsensor_gc0309_i2c_write_byte(0xb3,0x60);
				camsensor_gc0309_i2c_write_byte(0xb4,0x40);
				camsensor_gc0309_i2c_write_byte(0xba,0x00);//fixed_Cb
				camsensor_gc0309_i2c_write_byte(0xbb,0x00);//fixed_Cr 		
				gc_effect_on = TRUE;
				break;			
				
			default:
				ret = FALSE;
		}
		
		return ret;
	
} /* camsensor_gc0309_set_effect() */

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
camera_ret_code_type camsensor_gc0309_set_antibanding(int8 antibanding)
{
	if(TRUE == sensor_cap_state)
	return TRUE;
	camsensor_gc0309_i2c_write_byte(0xd2  ,0x10); 	

	switch (antibanding)
	{
		case CAMERA_ANTIBANDING_50HZ:
			if(g_bMJPEG_mode)   // MPEG mode , 12M
			{
				camsensor_gc0309_i2c_write_byte(0x01  ,0x32); 	
				camsensor_gc0309_i2c_write_byte(0x02  ,0x70); 
				camsensor_gc0309_i2c_write_byte(0x0f  ,0x01);

				camsensor_gc0309_i2c_write_byte(0x03  ,0x01); 	
				camsensor_gc0309_i2c_write_byte(0x04  ,0x2c); 	
		 
				camsensor_gc0309_i2c_write_byte(0xe2  ,0x00); 	//anti-flicker step [11:8]
				camsensor_gc0309_i2c_write_byte(0xe3  ,0x3c);   //anti-flicker step [7:0]
					
				camsensor_gc0309_i2c_write_byte(0xe4  ,0x02);   //exp level 1
				camsensor_gc0309_i2c_write_byte(0xe5  ,0x58); 
				camsensor_gc0309_i2c_write_byte(0xe6  ,0x02);   //exp level 2
				camsensor_gc0309_i2c_write_byte(0xe7  ,0x58); 
				camsensor_gc0309_i2c_write_byte(0xe8  ,0x02);   //exp level 3
				camsensor_gc0309_i2c_write_byte(0xe9  ,0x58); 
				camsensor_gc0309_i2c_write_byte(0xea  ,0x05);   //exp level 4
				camsensor_gc0309_i2c_write_byte(0xeb  ,0xa0); 	
			}
			else   // Preview             MCLK:24MHz
			{
				camsensor_gc0309_i2c_write_byte(0x01  ,0xfa); 	
				camsensor_gc0309_i2c_write_byte(0x02  ,0x70); 
				camsensor_gc0309_i2c_write_byte(0x0f  ,0x01);

				camsensor_gc0309_i2c_write_byte(0x03  ,0x01); 	
				camsensor_gc0309_i2c_write_byte(0x04  ,0x2c); 	
		 
				camsensor_gc0309_i2c_write_byte(0xe2  ,0x00); 	//anti-flicker step [11:8]
				camsensor_gc0309_i2c_write_byte(0xe3  ,0x64);   //anti-flicker step [7:0]
					
				camsensor_gc0309_i2c_write_byte(0xe4  ,0x02);   //exp level 1  16.67fps
				camsensor_gc0309_i2c_write_byte(0xe5  ,0x58); 
				camsensor_gc0309_i2c_write_byte(0xe6  ,0x03);   //exp level 2  12.5fps
				camsensor_gc0309_i2c_write_byte(0xe7  ,0x20); 
				camsensor_gc0309_i2c_write_byte(0xe8  ,0x04);   //exp level 3  8.33fps
				camsensor_gc0309_i2c_write_byte(0xe9  ,0xb0); 
				camsensor_gc0309_i2c_write_byte(0xea  ,0x09);   //exp level 4  4.00fps
				camsensor_gc0309_i2c_write_byte(0xeb  ,0xc4); 
				
			//below is HB\VB setting MCLK@12MHz; 50Hz Banding
			/*
				camsensor_gc0309_i2c_write_byte(0x01  ,0x6a); 	
				camsensor_gc0309_i2c_write_byte(0x02  ,0x25); 
				camsensor_gc0309_i2c_write_byte(0x0f  ,0x00);
		 
				camsensor_gc0309_i2c_write_byte(0xe2  ,0x00); 	//anti-flicker step [11:8]
				camsensor_gc0309_i2c_write_byte(0xe3  ,0x4b);   //anti-flicker step [7:0]
					
				camsensor_gc0309_i2c_write_byte(0xe4  ,0x02);   //exp level 1
				camsensor_gc0309_i2c_write_byte(0xe5  ,0x0d); 
				camsensor_gc0309_i2c_write_byte(0xe6  ,0x02);   //exp level 2
				camsensor_gc0309_i2c_write_byte(0xe7  ,0x0d); 
				camsensor_gc0309_i2c_write_byte(0xe8  ,0x02);   //exp level 3
				camsensor_gc0309_i2c_write_byte(0xe9  ,0x0d); 
				camsensor_gc0309_i2c_write_byte(0xea  ,0x05);   //exp level 4
				camsensor_gc0309_i2c_write_byte(0xeb  ,0xdc); 
			*/
			}
			g_banding_is_60Hz = FALSE;
			break;                        
		case CAMERA_ANTIBANDING_60HZ:
			if(g_bMJPEG_mode)   // MPEG mode , 12M
			{
				camsensor_gc0309_i2c_write_byte(0x01  ,0x32); 	
				camsensor_gc0309_i2c_write_byte(0x02  ,0x70); 
				camsensor_gc0309_i2c_write_byte(0x0f  ,0x01); 	

				camsensor_gc0309_i2c_write_byte(0x03  ,0x01); 	
				camsensor_gc0309_i2c_write_byte(0x04  ,0x2c); 	

				camsensor_gc0309_i2c_write_byte(0xe2  ,0x00); 	//anti-flicker step [11:8]
				camsensor_gc0309_i2c_write_byte(0xe3  ,0x32); 
				
				camsensor_gc0309_i2c_write_byte(0xe4  ,0x02); 
				camsensor_gc0309_i2c_write_byte(0xe5  ,0x58); 
				camsensor_gc0309_i2c_write_byte(0xe6  ,0x02); 
				camsensor_gc0309_i2c_write_byte(0xe7  ,0x58); 
				camsensor_gc0309_i2c_write_byte(0xe8  ,0x02); 
				camsensor_gc0309_i2c_write_byte(0xe9  ,0x58); 
				camsensor_gc0309_i2c_write_byte(0xea  ,0x04); 
				camsensor_gc0309_i2c_write_byte(0xeb  ,0xb0); 			
			}
			else   // Preview          MCLK:24MHz
			{
				camsensor_gc0309_i2c_write_byte(0x01  ,0x2c); 	
				camsensor_gc0309_i2c_write_byte(0x02  ,0x98); 
				camsensor_gc0309_i2c_write_byte(0x0f  ,0x02);

 				camsensor_gc0309_i2c_write_byte(0x03  ,0x01); 	
				camsensor_gc0309_i2c_write_byte(0x04  ,0x40); 	

				camsensor_gc0309_i2c_write_byte(0xe2  ,0x00); 	//anti-flicker step [11:8]
				camsensor_gc0309_i2c_write_byte(0xe3  ,0x50);   //anti-flicker step [7:0]
					
				camsensor_gc0309_i2c_write_byte(0xe4  ,0x02);   //exp level 1  15.00fps
				camsensor_gc0309_i2c_write_byte(0xe5  ,0x80); 
				camsensor_gc0309_i2c_write_byte(0xe6  ,0x03);   //exp level 2  10.00fps
				camsensor_gc0309_i2c_write_byte(0xe7  ,0xc0); 
				camsensor_gc0309_i2c_write_byte(0xe8  ,0x05);   //exp level 3  7.50fps
				camsensor_gc0309_i2c_write_byte(0xe9  ,0x00); 
				camsensor_gc0309_i2c_write_byte(0xea  ,0x09);   //exp level 4  4.00fps
				camsensor_gc0309_i2c_write_byte(0xeb  ,0x60); 

			//below is HB\VB setting MCLK@12MHz; 60Hz Banding
			/*
				camsensor_gc0309_i2c_write_byte(0x01  ,0x32); 	
				camsensor_gc0309_i2c_write_byte(0x02  ,0x3e); 
				camsensor_gc0309_i2c_write_byte(0x0f  ,0x01); 
				
				camsensor_gc0309_i2c_write_byte(0xe2  ,0x00); 	//anti-flicker step [11:8]
				camsensor_gc0309_i2c_write_byte(0xe3  ,0x32); 
				
				camsensor_gc0309_i2c_write_byte(0xe4  ,0x02); 
				camsensor_gc0309_i2c_write_byte(0xe5  ,0x26); 
				camsensor_gc0309_i2c_write_byte(0xe6  ,0x02); 
				camsensor_gc0309_i2c_write_byte(0xe7  ,0x26); 
				camsensor_gc0309_i2c_write_byte(0xe8  ,0x02); 
				camsensor_gc0309_i2c_write_byte(0xe9  ,0x26); 
				camsensor_gc0309_i2c_write_byte(0xea  ,0x04); 
				camsensor_gc0309_i2c_write_byte(0xeb  ,0x4c);  
			*/
			}
			g_banding_is_60Hz = TRUE;
			break;
		default:
			return CAMERA_SUCCESS;		
	}	

	camsensor_gc0309_i2c_write_byte(0xd2  ,0x90); 	

	return CAMERA_SUCCESS;

}


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
camera_ret_code_type camsensor_gc0309_set_brightness(int8 brightness)
{
uint16 reg_val;
uint16 target_step;

if (brightness < CAMSENSOR_MIN_BRIGHTNESS)
{
return CAMERA_INVALID_PARM;
}
else if (brightness > CAMSENSOR_MAX_BRIGHTNESS)
{
return CAMERA_INVALID_PARM;
}
else if (camsensor_gc0309_brightness == brightness)
{
return CAMERA_SUCCESS;
}
else
{

target_step =
(MAX_TARGET_LUMA - MIN_TARGET_LUMA) / CAMSENSOR_MAX_BRIGHTNESS;

reg_val = (uint16)(MIN_TARGET_LUMA + (target_step * brightness));
/* Set the Value of new Target Luma */
/* AE Driver , Offset 6 */

//add code here



}

camsensor_gc0309_brightness = brightness;

return CAMERA_SUCCESS;

} /* camsensor_gc0309_set_brightness() */


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

camera_ret_code_type camsensor_gc0309_set_exposure_mode(int8 mode)

{


if (camsensor_gc0309_exposure_mode == mode)
{
return CAMERA_SUCCESS;
}


//add code here


camsensor_gc0309_exposure_mode=mode;
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

camera_ret_code_type camsensor_gc0309_set_ev_compensation(int32 compensation)

{
camera_ret_code_type ret_val = CAMERA_SUCCESS;
uint8 camsensor_gc0309_default_luma_target=50;
int16  parmCurNum = (int16)(compensation>>16);
int32 max_parmVal = 12;
int32 min_parmVal =-12;
int32  parmVal= parmCurNum;
uint32 new_luma_target;

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
new_luma_target = (uint32) (( camsensor_gc0309_default_luma_target *
camsensor_gc0309_ev_compensation_table[new_luma_target] ) + 0.5);
}

/* Apply the new Luma Target to the AE Driver */



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
camera_ret_code_type camsensor_gc0309_set_contrast(int8 contrast)
{

#if 0 
uint16 reg;

MSG_MED("camsensor_mt9m112_set_contrast()", 0, 0, 0);
 /* Contrast is not tuned */
if (contrast > CAMSENSOR_MAX_CONTRAST)
{
return CAMERA_INVALID_PARM;
}
else if (contrast < CAMSENSOR_MIN_CONTRAST)
{
return CAMERA_INVALID_PARM;
}
else if (camsensor_gc0309_contrast == contrast)
{
return CAMERA_SUCCESS;
}

/* Load the contrast table */
for (reg = 0; reg < 10; reg++)
{
if (!mt9d112_mu2m0yu_i2c_write_word(
camsensor_gc0309_contrast_reg_tbl[contrast][reg].register_address,
camsensor_gc0309_contrast_reg_tbl[contrast][reg].register_value))
return CAMERA_FAILED;
}

camsensor_gc0309_contrast = contrast;
#endif
return CAMERA_SUCCESS;
} /* camsensor_mt9m112_set_contrast() */


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
static void camsensor_gc0309_register(camsensor_function_table_type *camsensor_function_table_ptr)
{
camsensor_function_table_ptr->camsensor_start                 = camsensor_gc0309_start;
camsensor_function_table_ptr->camsensor_video_config          = camsensor_gc0309_video_config;
camsensor_function_table_ptr->camsensor_raw_snapshot_config   = camsensor_gc0309_raw_snapshot_config;
camsensor_function_table_ptr->camsensor_snapshot_config       = camsensor_gc0309_snapshot_config;
camsensor_function_table_ptr->camsensor_power_up              = camsensor_gc0309_power_up;
camsensor_function_table_ptr->camsensor_power_down            = camsensor_gc0309_power_down;
camsensor_function_table_ptr->camsensor_set_frame_rate        = camsensor_gc0309_set_frame_rate;  

camsensor_function_table_ptr->camsensor_set_effect            = camsensor_gc0309_set_effect;
camsensor_function_table_ptr->camsensor_set_antibanding       = camsensor_gc0309_set_antibanding; 

camsensor_function_table_ptr->camsensor_set_brightness        = \
  camsensor_gc0309_set_brightness;
 camsensor_function_table_ptr->camsensor_set_contrast          = \
  camsensor_gc0309_set_contrast;

camsensor_function_table_ptr->camsensor_set_exposure_mode = \
  camsensor_gc0309_set_exposure_mode;
camsensor_function_table_ptr->camsensor_set_ev_compensation=\
  camsensor_gc0309_set_ev_compensation; 
camsensor_function_table_ptr->camsensor_set_wb= \
                              camsensor_gc0309_set_wb;



} /* camsensor_gc0309_register */

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
static void camsensor_gc0309_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 
camctrl_tbl_ptr->la_is_supported              = FALSE;
camctrl_tbl_ptr->cs_is_supported              = FALSE;
camctrl_tbl_ptr->iso_is_supported             = FALSE;
camctrl_tbl_ptr->enable_rolloff_correction    = FALSE;
camctrl_tbl_ptr->hjr_bayer_filtering_enable   = FALSE;
camctrl_tbl_ptr->nightshot_is_supported       = FALSE;

camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_gc0309_get_snapshot_fps;
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

LOCAL uint16 camsensor_gc0309_get_snapshot_fps(uint16 fps)
{
/* In the current driver, the snapshot fps is always 11 */
fps = 11 * Q8;

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

camera_ret_code_type camsensor_gc0309_set_wb(int8 wb)

{
  
  uint16 reg_val;  

  switch((camera_wb_type)wb)

   {

  case CAMERA_WB_AUTO:
   {
 /* Enable the Auto White Balance */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA102))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val|0x0004))) {
    return CAMERA_FAILED;
}

/* Let awb.CCMPosition set Automatically*/

if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA353))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val&0xFFDF))) {
    return CAMERA_FAILED;
}

/* Refresh the Sequencer */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA103))) {
    return CAMERA_FAILED;
       }
if (!(mt9d112_mu2m0yu_i2c_write_word(0x339C,0x0005))) {
    return CAMERA_FAILED;
    }
camera_timed_wait(5);
    break;
}

case CAMERA_WB_INCANDESCENT:
{

 /* Disable Auto White Balance */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA102))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val&0xFFFB))) {
    return CAMERA_FAILED;
}

/* force AWB Digital Gains to unit; program value into
   awb.CCMPosition to manually set matrix position */

if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA353))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val|0x0020))) {
    return CAMERA_FAILED;
}

/* Program Pre-calibrated awb.CCMPosition for Incandescent */

if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA351))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,0x0))) {
    return CAMERA_FAILED;
}
/* Refresh the Sequencer */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA103))) {
    return CAMERA_FAILED;
   }
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,0x0005))) {
    return CAMERA_FAILED;
}
camera_timed_wait(5);
break;

}


case CAMERA_WB_FLUORESCENT:
{
/* Disable Auto White Balance */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA102))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val&0xFFFB))) {
    return CAMERA_FAILED;
}

/* force AWB Digital Gains to unit; program value into
    awb.CCMPosition to manually set matrix position */


if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA353))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val|0x0020))) {
    return CAMERA_FAILED;
}

/* Program Pre-calibrated awb.CCMPosition for Incandescent */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA351))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,0x1C))) {
    return CAMERA_FAILED;
   }
/* Refresh the Sequencer */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA103))) {
    return CAMERA_FAILED;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,0x0005))) {
    return CAMERA_FAILED;}
camera_timed_wait(5);
break;
}

case CAMERA_WB_DAYLIGHT:
{
/* Disable Auto White Balance */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA102))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val&0xFFFB))) {
    return CAMERA_FAILED;
}

/* force AWB Digital Gains to unit; program value into
    awb.CCMPosition to manually set matrix position */

if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA353))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val|0x0020))) {
    return CAMERA_FAILED;
}

/* Program Pre-calibrated awb.CCMPosition for Incandescent */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA351))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,0x7F))) {
    return CAMERA_FAILED;
   }
/* Refresh the Sequencer */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA103))) {
    return CAMERA_FAILED;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,0x0005))) {
    return CAMERA_FAILED;}
camera_timed_wait(5);
break;
}

case CAMERA_WB_CLOUDY_DAYLIGHT:
{
/* Disable Auto White Balance */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA102))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val&0xFFFB))) {
    return CAMERA_FAILED;
}

/* force AWB Digital Gains to unit; program value into
    awb.CCMPosition to manually set matrix position */

if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA353))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390,&reg_val))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,reg_val|0x0020))) {
    return CAMERA_FAILED;
}

/* Program Pre-calibrated awb.CCMPosition for Incandescent */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA351))) {
    return CAMERA_FAILED;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,0x30))) {
    return CAMERA_FAILED;
   }
/* Refresh the Sequencer */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C,0xA103))) {
    return CAMERA_FAILED;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390,0x0005))) {
    return CAMERA_FAILED;}
camera_timed_wait(5);
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

FUNCTION      INITIALIZE_mt9d112_mu2m0yu_REGISTERS

DESCRIPTION
16-bit I2C read.

DEPENDENCIES
None

RETURN VALUE
TRUE  - Register initialization successful.
FALSE - Register initialization failed.


SIDE EFFECTS
None

if(!(mt9d112_mu2m0yu_i2c_write_word( , )))
{return FALSE;}

===========================================================================*/
static boolean initialize_gc0309_registers(void)
{
	uint8 sensor_id = 0;

	//gc_effect_on = KAL_FALSE;
#ifdef GC_DEBUG	
	_dbg_print("Enter init_GC0309!\r\n");
#endif
	//cis_module_power_on(KAL_TRUE);	  // Power On CIS Power 
	clk_busy_wait(2000);
// SCCB high, output
	//GPIO_ModeSetup(SCCB_SERIAL_CLK_PIN,0);
	//GPIO_ModeSetup(SCCB_SERIAL_DATA_PIN,0);		
	//GPIO_WriteIO(1, SCCB_SERIAL_CLK_PIN); 	
	//GPIO_WriteIO(1, SCCB_SERIAL_DATA_PIN);			  
	//GPIO_InitIO(OUTPUT, SCCB_SERIAL_CLK_PIN); 	
	//GPIO_InitIO(OUTPUT, SCCB_SERIAL_DATA_PIN);	
	
	
	clk_busy_wait(2000);				// To wait for Stable Power

	//REG_ISP_CMOS_SENSOR_MODE_CONFIG |= REG_CMOS_SENSOR_RESET_BIT;
	//kal_sleep_task(2);	
	//REG_ISP_CMOS_SENSOR_MODE_CONFIG &= ~REG_CMOS_SENSOR_RESET_BIT;//low		
	//kal_sleep_task(2);
	//REG_ISP_CMOS_SENSOR_MODE_CONFIG |= REG_CMOS_SENSOR_RESET_BIT;
			
	//SET_CMOS_CLOCK_POLARITY_LOW; 
	//SET_VSYNC_POLARITY_HIGH;	 
	//SET_VSYNC_POLARITY_LOW;
	//SET_HSYNC_POLARITY_LOW;  // Hsync is high actived when set to LOW here  
	
	//ENABLE_CAMERA_INDATA_FORMAT;
	//SET_CAMERA_INPUT_TYPE(INPUT_YUV422);
	//SET_CAMERA_INPUT_ORDER(INPUT_ORDER_YCbY1Cr);
		
	//ENABLE_CAMERA_TG_CLK_48M;
	//UPLL_Enable(UPLL_OWNER_ISP);	  
	//set_isp_driving_current(camera_para.SENSOR.reg[CMMCLK_CURRENT_INDEX].para);
	//set_isp_driving_current(ISP_DRIVING_8MA);

	clk_busy_wait(10000);	
		
	camsensor_gc0309_i2c_read_byte(0x00,&sensor_id);			
		
	if(sensor_id != GC0309_SENSOR_ID)
	{
		return FALSE;  
	}

		/*init registers code.*/
	camsensor_gc0309_i2c_write_byte(0xfe,0x80);		
		
	GC0309_SET_PAGE0;		// set page0
	
	camsensor_gc0309_i2c_write_byte(0x1a,0x16);		
	camsensor_gc0309_i2c_write_byte(0xd2,0x10);	// close AEC
	camsensor_gc0309_i2c_write_byte(0x22,0x55);	// close AWB

	camsensor_gc0309_i2c_write_byte(0x5a,0x56); 
	camsensor_gc0309_i2c_write_byte(0x5b,0x40);
	camsensor_gc0309_i2c_write_byte(0x5c,0x4a);		
	
	camsensor_gc0309_i2c_write_byte(0x22,0x57);  // Open AWB
		
	camsensor_gc0309_i2c_write_byte(0x01,0xfa); 
	camsensor_gc0309_i2c_write_byte(0x02,0x70); 
	camsensor_gc0309_i2c_write_byte(0x0f,0x01); 

	camsensor_gc0309_i2c_write_byte(0xe2,0x00); 
	camsensor_gc0309_i2c_write_byte(0xe3,0x64); 

	camsensor_gc0309_i2c_write_byte(0x03,0x01); 
	camsensor_gc0309_i2c_write_byte(0x04,0x2c); 

	camsensor_gc0309_i2c_write_byte(0xd2,0x90);  

	/*
	camsensor_gc0309_i2c_write_byte(0x01,0x6a); 
	camsensor_gc0309_i2c_write_byte(0x02,0x25); 
	camsensor_gc0309_i2c_write_byte(0x0f,0x00);

	camsensor_gc0309_i2c_write_byte(0xe2,0x00); 
	camsensor_gc0309_i2c_write_byte(0xe3,0x4b); 
		
	camsensor_gc0309_i2c_write_byte(0xe4,0x02); 
	camsensor_gc0309_i2c_write_byte(0xe5,0x0d); 
	camsensor_gc0309_i2c_write_byte(0xe6,0x02); 
	camsensor_gc0309_i2c_write_byte(0xe7,0x0d); 
	camsensor_gc0309_i2c_write_byte(0xe8,0x02); 
	camsensor_gc0309_i2c_write_byte(0xe9,0x0d); 
	camsensor_gc0309_i2c_write_byte(0xea,0x05); 
	camsensor_gc0309_i2c_write_byte(0xeb,0xdc); 
	*/

	camsensor_gc0309_i2c_write_byte(0x05,0x00);
	camsensor_gc0309_i2c_write_byte(0x06,0x00);
	camsensor_gc0309_i2c_write_byte(0x07,0x00); 
	camsensor_gc0309_i2c_write_byte(0x08,0x00); 
	camsensor_gc0309_i2c_write_byte(0x09,0x01); 
	camsensor_gc0309_i2c_write_byte(0x0a,0xe8); 
	camsensor_gc0309_i2c_write_byte(0x0b,0x02); 
	camsensor_gc0309_i2c_write_byte(0x0c,0x88); 
	camsensor_gc0309_i2c_write_byte(0x0d,0x02); 
	camsensor_gc0309_i2c_write_byte(0x0e,0x02); 
	camsensor_gc0309_i2c_write_byte(0x10,0x22); 
	camsensor_gc0309_i2c_write_byte(0x11,0x0d); 
	camsensor_gc0309_i2c_write_byte(0x12,0x2a); 
	camsensor_gc0309_i2c_write_byte(0x13,0x00); 
	camsensor_gc0309_i2c_write_byte(0x15,0x0a); 
	camsensor_gc0309_i2c_write_byte(0x16,0x05); 
	camsensor_gc0309_i2c_write_byte(0x17,0x01); 

	camsensor_gc0309_i2c_write_byte(0x1b,0x00); 
	camsensor_gc0309_i2c_write_byte(0x1c,0xc1); 
	camsensor_gc0309_i2c_write_byte(0x1d,0x08); 
	camsensor_gc0309_i2c_write_byte(0x1e,0x20); 
	camsensor_gc0309_i2c_write_byte(0x1f,0x16); 

	camsensor_gc0309_i2c_write_byte(0x20,0xff); 
	camsensor_gc0309_i2c_write_byte(0x21,0xf8); 
	camsensor_gc0309_i2c_write_byte(0x24,0xa2); 
	camsensor_gc0309_i2c_write_byte(0x25,0x0f);
	//output sync_mode
	camsensor_gc0309_i2c_write_byte(0x26,0x03); 
	camsensor_gc0309_i2c_write_byte(0x2f,0x01); 
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// grab_t ////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0x30,0xf7); 
	camsensor_gc0309_i2c_write_byte(0x31,0x40);
	camsensor_gc0309_i2c_write_byte(0x32,0x00); 
	camsensor_gc0309_i2c_write_byte(0x39,0x04); 
	camsensor_gc0309_i2c_write_byte(0x3a,0x20); 
	camsensor_gc0309_i2c_write_byte(0x3b,0x20); 
	camsensor_gc0309_i2c_write_byte(0x3c,0x02); 
	camsensor_gc0309_i2c_write_byte(0x3d,0x02); 
	camsensor_gc0309_i2c_write_byte(0x3e,0x02);
	camsensor_gc0309_i2c_write_byte(0x3f,0x02); 
	
	//gain
	camsensor_gc0309_i2c_write_byte(0x50,0x24); 
	
	camsensor_gc0309_i2c_write_byte(0x53,0x82); 
	camsensor_gc0309_i2c_write_byte(0x54,0x80); 
	camsensor_gc0309_i2c_write_byte(0x55,0x80); 
	camsensor_gc0309_i2c_write_byte(0x56,0x82); 
	
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// LSC_t  ////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0x8b,0x20); 
	camsensor_gc0309_i2c_write_byte(0x8c,0x20); 
	camsensor_gc0309_i2c_write_byte(0x8d,0x20); 
	camsensor_gc0309_i2c_write_byte(0x8e,0x10); 
	camsensor_gc0309_i2c_write_byte(0x8f,0x10); 
	camsensor_gc0309_i2c_write_byte(0x90,0x10); 
	camsensor_gc0309_i2c_write_byte(0x91,0x3c); 
	camsensor_gc0309_i2c_write_byte(0x92,0x50); 
	camsensor_gc0309_i2c_write_byte(0x5d,0x12); 
	camsensor_gc0309_i2c_write_byte(0x5e,0x1a); 
	camsensor_gc0309_i2c_write_byte(0x5f,0x24); 
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// DNDD_t	///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0x60,0x07); 
	camsensor_gc0309_i2c_write_byte(0x61,0x0e); 
	camsensor_gc0309_i2c_write_byte(0x62,0x0c); 
	camsensor_gc0309_i2c_write_byte(0x64,0x03); 
	camsensor_gc0309_i2c_write_byte(0x66,0xe8); 
	camsensor_gc0309_i2c_write_byte(0x67,0x86); 
	camsensor_gc0309_i2c_write_byte(0x68,0xa2); 
	
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// asde_t ///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0x69,0x20); 
	camsensor_gc0309_i2c_write_byte(0x6a,0x0f); 
	camsensor_gc0309_i2c_write_byte(0x6b,0x00); 
	camsensor_gc0309_i2c_write_byte(0x6c,0x53); 
	camsensor_gc0309_i2c_write_byte(0x6d,0x83); 
	camsensor_gc0309_i2c_write_byte(0x6e,0xac); 
	camsensor_gc0309_i2c_write_byte(0x6f,0xac); 
	camsensor_gc0309_i2c_write_byte(0x70,0x15); 
	camsensor_gc0309_i2c_write_byte(0x71,0x33); 
	/////////////////////////////////////////////////////////////////////
	/////////////////////////// eeintp_t///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0x72,0xdc);  
	camsensor_gc0309_i2c_write_byte(0x73,0x80);  
	//for high resolution in light scene
	camsensor_gc0309_i2c_write_byte(0x74,0x02); 
	camsensor_gc0309_i2c_write_byte(0x75,0x3f); 
	camsensor_gc0309_i2c_write_byte(0x76,0x02); 
	camsensor_gc0309_i2c_write_byte(0x77,0x54); 
	camsensor_gc0309_i2c_write_byte(0x78,0x88); 
	camsensor_gc0309_i2c_write_byte(0x79,0x81); 
	camsensor_gc0309_i2c_write_byte(0x7a,0x81); 
	camsensor_gc0309_i2c_write_byte(0x7b,0x22); 
	camsensor_gc0309_i2c_write_byte(0x7c,0xff);
	
	
	/////////////////////////////////////////////////////////////////////
	///////////////////////////CC_t///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0x93,0x45); 
	camsensor_gc0309_i2c_write_byte(0x94,0x00); 
	camsensor_gc0309_i2c_write_byte(0x95,0x00); 
	camsensor_gc0309_i2c_write_byte(0x96,0x00); 
	camsensor_gc0309_i2c_write_byte(0x97,0x45); 
	camsensor_gc0309_i2c_write_byte(0x98,0xf0); 
	camsensor_gc0309_i2c_write_byte(0x9c,0x00); 
	camsensor_gc0309_i2c_write_byte(0x9d,0x03); 
	camsensor_gc0309_i2c_write_byte(0x9e,0x00); 
	
	
	/////////////////////////////////////////////////////////////////////
	///////////////////////////YCP_t///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0xb1,0x40); 
	camsensor_gc0309_i2c_write_byte(0xb2,0x40); 
	camsensor_gc0309_i2c_write_byte(0xb8,0x20); 
	camsensor_gc0309_i2c_write_byte(0xbe,0x36); 
	camsensor_gc0309_i2c_write_byte(0xbf,0x00); 
	/////////////////////////////////////////////////////////////////////
	///////////////////////////AEC_t///////////////////////////////
	/////////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0xd0,0xc9);  
	camsensor_gc0309_i2c_write_byte(0xd1,0x10);  
//	camsensor_gc0309_i2c_write_byte(0xd2,0x90);  
	camsensor_gc0309_i2c_write_byte(0xd3,0x80);  
	camsensor_gc0309_i2c_write_byte(0xd5,0xf2); 
	camsensor_gc0309_i2c_write_byte(0xd6,0x16);  
	camsensor_gc0309_i2c_write_byte(0xdb,0x92); 
	camsensor_gc0309_i2c_write_byte(0xdc,0xa5);  
	camsensor_gc0309_i2c_write_byte(0xdf,0x23);	
	camsensor_gc0309_i2c_write_byte(0xd9,0x00);  
	camsensor_gc0309_i2c_write_byte(0xda,0x00);  
	camsensor_gc0309_i2c_write_byte(0xe0,0x09);

	camsensor_gc0309_i2c_write_byte(0xec,0x20);  
	camsensor_gc0309_i2c_write_byte(0xed,0x04);  
	camsensor_gc0309_i2c_write_byte(0xee,0xa0);  
	camsensor_gc0309_i2c_write_byte(0xef,0x40);  
	///////////////////////////////////////////////////////////////////
	///////////////////////////GAMMA//////////////////////////////////
	///////////////////////////////////////////////////////////////////
#if 0	
	camsensor_gc0309_i2c_write_byte(0x9F,0x0F);			
	camsensor_gc0309_i2c_write_byte(0xA0,0x1D);  
	camsensor_gc0309_i2c_write_byte(0xA1,0x2D);
	camsensor_gc0309_i2c_write_byte(0xA2,0x3B);
	camsensor_gc0309_i2c_write_byte(0xA3,0x46);
	camsensor_gc0309_i2c_write_byte(0xA4,0x50);
	camsensor_gc0309_i2c_write_byte(0xA5,0x5A);
	camsensor_gc0309_i2c_write_byte(0xA6,0x6B);
	camsensor_gc0309_i2c_write_byte(0xA7,0x7B);
	camsensor_gc0309_i2c_write_byte(0xA8,0x8A);
	camsensor_gc0309_i2c_write_byte(0xA9,0x98);
	camsensor_gc0309_i2c_write_byte(0xAA,0xA5);
	camsensor_gc0309_i2c_write_byte(0xAB,0xB2);
	camsensor_gc0309_i2c_write_byte(0xAC,0xBE);
	camsensor_gc0309_i2c_write_byte(0xAD,0xD5);
	camsensor_gc0309_i2c_write_byte(0xAE,0xEB);
	camsensor_gc0309_i2c_write_byte(0xAF,0xFE);
#endif
	//Y_gamma
	camsensor_gc0309_i2c_write_byte(0xc0,0x00);
	camsensor_gc0309_i2c_write_byte(0xc1,0x0B);
	camsensor_gc0309_i2c_write_byte(0xc2,0x15);
	camsensor_gc0309_i2c_write_byte(0xc3,0x27);
	camsensor_gc0309_i2c_write_byte(0xc4,0x39);
	camsensor_gc0309_i2c_write_byte(0xc5,0x49);
	camsensor_gc0309_i2c_write_byte(0xc6,0x5A);
	camsensor_gc0309_i2c_write_byte(0xc7,0x6A);
	camsensor_gc0309_i2c_write_byte(0xc8,0x89);
	camsensor_gc0309_i2c_write_byte(0xc9,0xA8);
	camsensor_gc0309_i2c_write_byte(0xca,0xC6);
	camsensor_gc0309_i2c_write_byte(0xcb,0xE3);
	camsensor_gc0309_i2c_write_byte(0xcc,0xFF);

	/////////////////////////////////////////////////////////////////
	/////////////////////////// ABS_t ///////////////////////////////
	/////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0xf0,0x02);
	camsensor_gc0309_i2c_write_byte(0xf1,0x01);
	camsensor_gc0309_i2c_write_byte(0xf2,0x00);  
	camsensor_gc0309_i2c_write_byte(0xf3,0x30); 
	
	/////////////////////////////////////////////////////////////////
	/////////////////////////// Measure Window ///////////////////////
	/////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0xf7,0x04); 
	camsensor_gc0309_i2c_write_byte(0xf8,0x02); 
	camsensor_gc0309_i2c_write_byte(0xf9,0x9f);
	camsensor_gc0309_i2c_write_byte(0xfa,0x78);

	//---------------------------------------------------------------
	GC0309_SET_PAGE1;

	/////////////////////////////////////////////////////////////////
	///////////////////////////AWB_p/////////////////////////////////
	/////////////////////////////////////////////////////////////////
	camsensor_gc0309_i2c_write_byte(0x00,0xf5); 
	camsensor_gc0309_i2c_write_byte(0x01,0x0a);  
	camsensor_gc0309_i2c_write_byte(0x02,0x1a); 
	camsensor_gc0309_i2c_write_byte(0x0a,0xa0); 
	camsensor_gc0309_i2c_write_byte(0x0b,0x64); 
	camsensor_gc0309_i2c_write_byte(0x0c,0x08);
	camsensor_gc0309_i2c_write_byte(0x0e,0x4c); 
	camsensor_gc0309_i2c_write_byte(0x0f,0x39); 
	camsensor_gc0309_i2c_write_byte(0x11,0x3f); 
	camsensor_gc0309_i2c_write_byte(0x13,0x11); 
	camsensor_gc0309_i2c_write_byte(0x14,0x40);  
	camsensor_gc0309_i2c_write_byte(0x15,0x40); 
	camsensor_gc0309_i2c_write_byte(0x16,0xc2); 
	camsensor_gc0309_i2c_write_byte(0x17,0xa8); 
	camsensor_gc0309_i2c_write_byte(0x18,0x18);  
	camsensor_gc0309_i2c_write_byte(0x19,0x40);  
	camsensor_gc0309_i2c_write_byte(0x1a,0xd0); 
	camsensor_gc0309_i2c_write_byte(0x1b,0xf5);  

	camsensor_gc0309_i2c_write_byte(0x70,0x43); 
	camsensor_gc0309_i2c_write_byte(0x71,0x58);  
	camsensor_gc0309_i2c_write_byte(0x72,0x30);  
	camsensor_gc0309_i2c_write_byte(0x73,0x48);  
	camsensor_gc0309_i2c_write_byte(0x74,0x20);  
	camsensor_gc0309_i2c_write_byte(0x75,0x60);  
	
	GC0309_SET_PAGE0;

	//GC0309_Write_More_Registers();

	/*Customer can adjust GAMMA, MIRROR & UPSIDEDOWN here!*/
	//GC0309_GAMMA_Select(2);
	//GC0309_H_V_Switch(1);
	
	return TRUE;
} /* end of initialize_mt9d112_mu2m0yu_registers. */

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
static camera_ret_code_type camsensor_gc0309_set_frame_rate(uint16 fps)
{
	uint16  n_m;
	if(current_fps==fps)
	{
		return CAMERA_SUCCESS;
	}
	current_fps=fps; 
	if (fps > max_fps) 
	{
		return CAMERA_FAILED;
	}
	else if (fps == max_fps)
	{
		n_m=0x250;
	}
	else if (fps>= max_fps/2)
	{
		n_m=0x228;
	}

	else 
	{
		return CAMERA_FAILED;
	}

	//add code here
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
static boolean camsensor_gc0309_set_sensor_mode(sensor_mode_type sensor_mode)
{

/* Declare the local Variables */

	return TRUE;
}
/* mt9d112_mu2m0yu_set_sensor_mode */

LOCAL boolean mt9d112_mu2m0yu_set_lens_roll_off(void)
{
	return TRUE;
} /* mt9d112_mu2m0yu_set_lens_roll_off */

#endif /* FEATURE_CAMERA,USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU */

#ifdef NEW_CODE





#endif



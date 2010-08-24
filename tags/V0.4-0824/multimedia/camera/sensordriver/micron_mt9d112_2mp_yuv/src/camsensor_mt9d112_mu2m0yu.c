
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

#if defined (FEATURE_CAMERA) && defined (USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU)
#include "camsensor_mt9d112_mu2m0yu.h"
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
static const char camsensor_mt9d112_mu2m0yu_sensor_name[] = "Micron MT9D112 2.0 MP YUV";
static int8 camsensor_mt9d112_mu2m0yu_brightness;
static int8 camsensor_mt9d112_mu2m0yu_exposure_mode;
/* static int8 camsensor_mt9d112_mu2m0yu_contrast; */


/*****************************************************************************
*                          RUN TIME VARIABLES
****************************************************************************/

/*============================================================================
	  MACRO DEFINITIONS
============================================================================*/
/*  I2C Address of the Sensor  */
#define  MT9D112_MU2M0YU_I2C_SLAVE_ID  0x78

/* Micron MT9D112 Registers and their values */
/* Sensor Core Registers */
#define  REG_MT9D112_MU2M0YU_MODEL_ID 0x3000
#define  MT9D112_MU2M0YU_MODEL_ID     0x1580

/*  SOC Registers Page 1  */
#define  REG_MT9D112_MU2M0YU_SENSOR_RESET     0x301A
#define  REG_MT9D112_MU2M0YU_STANDBY_CONTROL  0x3202
#define  REG_MT9D112_MU2M0YU_MCU_BOOT         0x3386


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
static void    camsensor_mt9d112_mu2m0yu_register(camsensor_function_table_type *camsensor_function_table_ptr);
static void    camsensor_mt9d112_mu2m0yu_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr);
static boolean mt9d112_mu2m0yu_i2c_write_word(uint16 offset, uint16 data);
static boolean mt9d112_mu2m0yu_i2c_read_word(uint16 offset, uint16 *data);
static camera_ret_code_type camsensor_mt9d112_mu2m0yu_set_frame_rate(uint16 fps);
static camera_ret_code_type camsensor_mt9d112_mu2m0yu_set_effect(int8 effect);
static boolean mt9d112_mu2m0yu_set_sensor_mode(sensor_mode_type);
static boolean initialize_mt9d112_mu2m0yu_registers(void);
static boolean mt9d112_mu2m0yu_set_lens_roll_off(void);
static uint16 camsensor_mt9d112_mu2m0yu_get_snapshot_fps(uint16);
camera_ret_code_type camsensor_mt9d112_mu2moyu_set_wb(int8 wb);

static uint16 current_fps=30*Q8;
static boolean initial_effect=FALSE;
static uint16 max_fps = 30 * Q8;
static camera_effect_type camsensor_mt9d112_mu2m0yu_effect = CAMERA_EFFECT_OFF;
static sensor_mode_type mt9d112_mu2m0yu_sensor_mode;
static CAMIF_InputFormatType format_preview,format_snapshot;

#ifdef T_FFA
#error code not present
#endif

float camsensor_mt9d112_mu2m0yu_ev_compensation_table[] =
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

LOCAL register_address_value_pair preview_snapshot_mode_reg_settings_array[] =
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
LOCAL register_address_value_pair noise_reduction_reg_settings_array[] =
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
boolean camsensor_mt9d112_mu2m0yu_init(camsensor_function_table_type *camsensor_function_table_ptr,
				   camctrl_tbl_type              *camctrl_tbl_ptr)
{
uint16 model_id;

#ifdef T_FFA
#error code not present
#endif

/* Input MCLK = 24MHz */
camsensor_camclk_po_hz = 24000000;
camsensor_camclk_po_hz = camsensor_config_camclk_po(camsensor_camclk_po_hz);

/* Configure I2C parameters */
camsensor_i2c_command.bus_id     = I2C_BUS_HW_CTRL;
camsensor_i2c_command.slave_addr = MT9D112_MU2M0YU_I2C_SLAVE_ID;
/*lint -save -e655 */
camsensor_i2c_command.options    = (i2c_options_type) (I2C_MEM_DEV | I2C_START_BEFORE_READ); 

/* Micron suggested Power up block Start */
/* Put MCU into Reset - Stop MCU */
if (!(mt9d112_mu2m0yu_i2c_write_word(REG_MT9D112_MU2M0YU_MCU_BOOT, 0x0501))) {
return FALSE;
}

/* Pull MCU from Reset - Start MCU */
if (!(mt9d112_mu2m0yu_i2c_write_word(REG_MT9D112_MU2M0YU_MCU_BOOT, 0x0500))) {
return FALSE;
}

/* Delay */
camera_timed_wait(5);

/* Micron Suggested - Power up block */
if (!(mt9d112_mu2m0yu_i2c_write_word(REG_MT9D112_MU2M0YU_SENSOR_RESET, 0x0ACC))) {
return FALSE;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(REG_MT9D112_MU2M0YU_STANDBY_CONTROL, 0x0008))) {
return FALSE;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x33F4, 0x031D))) {// FUSED_DEFECT_CORRECTION
return FALSE;
}

/* Delay */
camera_timed_wait(5);

/* Micron suggested Power up block End */
/* Read the Model ID of the sensor */
if (!(mt9d112_mu2m0yu_i2c_read_word(REG_MT9D112_MU2M0YU_MODEL_ID, &model_id))) {
return FALSE;
}
/* Check if it matches it with the value in Datasheet */
if ( model_id != MT9D112_MU2M0YU_MODEL_ID ) {
return FALSE;
}
mt9d112_mu2m0yu_sensor_mode = SENSOR_PREVIEW_MODE;
/* The Sensor is indeed Micron MT9D112 */
/* Initialize Sensor registers */
if (!initialize_mt9d112_mu2m0yu_registers()) {
return FALSE;
}
/* Register function table: */
camsensor_mt9d112_mu2m0yu_register(camsensor_function_table_ptr);

/* Setup camctrl_tbl */
camsensor_mt9d112_mu2m0yu_setup_camctrl_tbl(camctrl_tbl_ptr);

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

void camsensor_mt9d112_mu2m0yu_init_camsensor_info()
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

switch (camsensor_info.msm_id) {
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

boolean camsensor_mt9d112_mu2m0yu_start(camsensor_static_params_type *camsensor_params)
{
camsensor_mt9d112_mu2m0yu_init_camsensor_info();

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
camsensor_params->sensor_model  = CAMSENSOR_MICRON_MT9D112_MU2M0YU;

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
camsensor_params->sensor_name = camsensor_mt9d112_mu2m0yu_sensor_name;

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

boolean camsensor_mt9d112_mu2m0yu_snapshot_config
(
camsensor_static_params_type *camsensor_params
) 
{
if ( camsensor_params == NULL) {
return(FALSE);
}

mt9d112_mu2m0yu_sensor_mode = SENSOR_SNAPSHOT_MODE;

/* Set special effects if any */
if (camsensor_mt9d112_mu2m0yu_set_effect((uint16) camsensor_mt9d112_mu2m0yu_effect) != CAMERA_SUCCESS) {
return FALSE;
}

if (mt9d112_mu2m0yu_set_sensor_mode(SENSOR_SNAPSHOT_MODE) == FALSE) {
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
} /* camsensor_mt9d112_mu2m0yu_snapshot_config */

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

boolean camsensor_mt9d112_mu2m0yu_raw_snapshot_config
(
camsensor_static_params_type *camsensor_params /* Other config params */
) 
{

/* Reducing fps in case of raw snapshot due to AHB overflows */
/* Total data wil be x*y*2 MB which is causing AHB overflows */
/* No real time requirement for Raw Snapshot so reducing the fps */

if(camsensor_mt9d112_mu2m0yu_set_frame_rate((max_fps/2))!=CAMERA_SUCCESS) {
return FALSE;    
}

if (mt9d112_mu2m0yu_set_sensor_mode(SENSOR_SNAPSHOT_MODE) == FALSE) {
return FALSE;
}
mt9d112_mu2m0yu_sensor_mode = SENSOR_SNAPSHOT_MODE;




/* Set special effects if any */
//  if(camsensor_mt9d112_mu2m0yu_set_effect(camsensor_mt9d112_mu2m0yu_effect) != CAMERA_SUCCESS)
//  {
//    return FALSE;
//  }

/* Sensor output format */
camsensor_params->format = format_snapshot;

/* Select discard first frame. */
camsensor_params->discardFirstFrame =  FALSE;

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
/* Disable the Epoch Interrupt by setting more number of lines than in a frame */
camsensor_params->epoch_line = 0xFFFF;
#endif

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

boolean camsensor_mt9d112_mu2m0yu_video_config
(
camsensor_static_params_type  *camsensor_params		 /* Camera sensor config params */
)
{
if ( camsensor_params == NULL) {
return(FALSE);
}

mt9d112_mu2m0yu_sensor_mode = SENSOR_PREVIEW_MODE;

camsensor_mt9d112_mu2m0yu_set_effect(camsensor_mt9d112_mu2m0yu_effect);

if(camsensor_mt9d112_mu2m0yu_set_frame_rate(max_fps)!=CAMERA_SUCCESS) 
{
	return FALSE;    
}

if (mt9d112_mu2m0yu_set_sensor_mode(SENSOR_PREVIEW_MODE) == FALSE) {
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
} /* camsensor_mt9d112_mu2m0yu_video_config */

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

void camsensor_mt9d112_mu2m0yu_power_up(void)
{
	initial_effect= TRUE; //Apply special effects for the first time while entering camera
} /* camsensor_mt9d112_mu2m0yu_power_up */

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

void camsensor_mt9d112_mu2m0yu_power_down(void)
{
(void) mt9d112_mu2m0yu_i2c_write_word(REG_MT9D112_MU2M0YU_MCU_BOOT, 0x0500);

(void) mt9d112_mu2m0yu_i2c_write_word(REG_MT9D112_MU2M0YU_STANDBY_CONTROL, 0x0008);

(void) mt9d112_mu2m0yu_i2c_write_word(REG_MT9D112_MU2M0YU_STANDBY_CONTROL, 0x0009);

/* Wait for Camera to enter standby. */
camera_timed_wait(10); 
} /* camsensor_mt9d112_mu2m0yu_power_down */

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

camera_ret_code_type camsensor_mt9d112_mu2m0yu_set_effect(int8 effect)
{
uint16 reg_addr;
uint16 reg_val;
uint16 seteffect_delay=1;
uint16 seteffect_zone=1;
static camera_effect_type prev_effect= CAMERA_EFFECT_MIN_MINUS_1;
static sensor_mode_type prev_camstate = SENSOR_SNAPSHOT_MODE;

if(prev_effect==effect && prev_camstate==mt9d112_mu2m0yu_sensor_mode && !initial_effect)
{
    return CAMERA_SUCCESS;
}
if(initial_effect==TRUE)
{
    initial_effect=FALSE;
}

prev_effect=(camera_effect_type)effect;
prev_camstate=mt9d112_mu2m0yu_sensor_mode;

switch (mt9d112_mu2m0yu_sensor_mode) {
case SENSOR_PREVIEW_MODE:
/* Context A Special Effects */
reg_addr = 0x2799;
break;
case SENSOR_SNAPSHOT_MODE:
/* Context B Special Effects */
reg_addr = 0x279B;
break;
default :
reg_addr = 0x2799;
break;
}
switch ((camera_effect_type)effect) {
case CAMERA_EFFECT_OFF:
{
reg_val = 0x6440;
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, reg_addr)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}
break;
}
case CAMERA_EFFECT_MONO:
{
reg_val = 0x6441;
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, reg_addr)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}
break;
}
case CAMERA_EFFECT_NEGATIVE:
{
reg_val = 0x6443;
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, reg_addr)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}
break;
}
case CAMERA_EFFECT_SOLARIZE:
{
reg_val = 0x6445;
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, reg_addr)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}
break;
}
case CAMERA_EFFECT_SEPIA:
{
reg_val = 0x6442;
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, reg_addr)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}
break;
}
case CAMERA_EFFECT_PASTEL:
case CAMERA_EFFECT_MOSAIC:
case CAMERA_EFFECT_RESIZE:
default:
{
reg_val = 0x6440;
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, reg_addr)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}

return CAMERA_NOT_SUPPORTED;
}
}

camsensor_mt9d112_mu2m0yu_effect = (camera_effect_type)effect;

/* Refresh Sequencer */
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA103)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x0005)) {
return CAMERA_FAILED;
}

if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA21C)) {
return CAMERA_FAILED;
}

if (!mt9d112_mu2m0yu_i2c_read_word(0x3390, &seteffect_zone)) {
return CAMERA_FAILED;
}
if(seteffect_zone> 0 && seteffect_zone< 25)
{
  seteffect_delay=(double)(250*seteffect_zone)/(double)(current_fps/Q8);

  camera_timed_wait(seteffect_delay);
}
return CAMERA_SUCCESS;
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
camera_ret_code_type camsensor_mt9d112_mu2m0yu_set_antibanding(int8 antibanding)
{
uint16 reg_val;
switch ((camera_antibanding_type)antibanding) {
case CAMERA_ANTIBANDING_OFF:
reg_val = 0x0000;
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0x2404)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}
break;

case CAMERA_ANTIBANDING_60HZ:
reg_val = 0x0080;
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0x2404)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}
break;

case CAMERA_ANTIBANDING_50HZ:
reg_val = 0x00C0;
if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0x2404)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}
break;

default:
return CAMERA_NOT_SUPPORTED;
}
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
camera_ret_code_type camsensor_mt9d112_mu2m0yu_set_brightness(int8 brightness)
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
else if (camsensor_mt9d112_mu2m0yu_brightness == brightness)
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

if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA206)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, reg_val)) {
return CAMERA_FAILED;
}



}

camsensor_mt9d112_mu2m0yu_brightness = brightness;

return CAMERA_SUCCESS;

} /* camsensor_mt9d112_mu2m0yu_set_brightness() */


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

camera_ret_code_type camsensor_mt9d112_mu2m0yu_set_exposure_mode(int8 mode)

{


if (camsensor_mt9d112_mu2m0yu_exposure_mode == mode)
{
return CAMERA_SUCCESS;
}

switch((camera_auto_exposure_mode_type)mode)

{

case CAMERA_AEC_FRAME_AVERAGE :

{
/* Whole Frame is considered for measuring the average luma, so both the windows 
should weight equally */
/* AE Driver Offset 17 */

if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA211)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x00FF)) {
return CAMERA_FAILED;
}
break;

}


case CAMERA_AEC_CENTER_WEIGHTED :

{
/* Center Window Should be Weighed more than Peripheral Window */
/* Center Window Weighs 3 times more than Peripheral Window */

if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA211)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x005F)) {
return CAMERA_FAILED;
}
break;


}

case CAMERA_AEC_SPOT_METERING :

{

/* Only Center Window is used for Calculating the average Luma */

if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA211)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x000F)) {
return CAMERA_FAILED;
}    
break;
}

default:

return CAMERA_NOT_SUPPORTED;


}
camsensor_mt9d112_mu2m0yu_exposure_mode=mode;
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

camera_ret_code_type camsensor_mt9d112_mu2m0yu_set_ev_compensation(int32 compensation)

{
camera_ret_code_type ret_val = CAMERA_SUCCESS;
uint8 camsensor_mt9d112_mu2m0yu_default_luma_target=50;
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
new_luma_target = (uint32) (( camsensor_mt9d112_mu2m0yu_default_luma_target *
camsensor_mt9d112_mu2m0yu_ev_compensation_table[new_luma_target] ) + 0.5);
}

/* Apply the new Luma Target to the AE Driver */

if (!mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA206)) {
return CAMERA_FAILED;
}
if (!mt9d112_mu2m0yu_i2c_write_word(0x3390, new_luma_target)) {
return CAMERA_FAILED;
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
camera_ret_code_type camsensor_mt9d112_mu2m0yu_set_contrast(int8 contrast)
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
else if (camsensor_mt9d112_mu2m0yu_contrast == contrast)
{
return CAMERA_SUCCESS;
}

/* Load the contrast table */
for (reg = 0; reg < 10; reg++)
{
if (!mt9d112_mu2m0yu_i2c_write_word(
camsensor_mt9d112_mu2m0yu_contrast_reg_tbl[contrast][reg].register_address,
camsensor_mt9d112_mu2m0yu_contrast_reg_tbl[contrast][reg].register_value))
return CAMERA_FAILED;
}

camsensor_mt9d112_mu2m0yu_contrast = contrast;
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
static void camsensor_mt9d112_mu2m0yu_register(camsensor_function_table_type *camsensor_function_table_ptr)
{
camsensor_function_table_ptr->camsensor_start                 = camsensor_mt9d112_mu2m0yu_start;
camsensor_function_table_ptr->camsensor_video_config          = camsensor_mt9d112_mu2m0yu_video_config;
camsensor_function_table_ptr->camsensor_raw_snapshot_config   = camsensor_mt9d112_mu2m0yu_raw_snapshot_config;
camsensor_function_table_ptr->camsensor_snapshot_config       = camsensor_mt9d112_mu2m0yu_snapshot_config;
camsensor_function_table_ptr->camsensor_power_up              = camsensor_mt9d112_mu2m0yu_power_up;
camsensor_function_table_ptr->camsensor_power_down            = camsensor_mt9d112_mu2m0yu_power_down;
camsensor_function_table_ptr->camsensor_set_frame_rate        = camsensor_mt9d112_mu2m0yu_set_frame_rate;  

camsensor_function_table_ptr->camsensor_set_effect            = camsensor_mt9d112_mu2m0yu_set_effect;
camsensor_function_table_ptr->camsensor_set_antibanding       = camsensor_mt9d112_mu2m0yu_set_antibanding; 

camsensor_function_table_ptr->camsensor_set_brightness        = \
  camsensor_mt9d112_mu2m0yu_set_brightness;
 camsensor_function_table_ptr->camsensor_set_contrast          = \
  camsensor_mt9d112_mu2m0yu_set_contrast;

camsensor_function_table_ptr->camsensor_set_exposure_mode = \
  camsensor_mt9d112_mu2m0yu_set_exposure_mode;
camsensor_function_table_ptr->camsensor_set_ev_compensation=\
  camsensor_mt9d112_mu2m0yu_set_ev_compensation; 
camsensor_function_table_ptr->camsensor_set_wb= \
                              camsensor_mt9d112_mu2moyu_set_wb;



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
static void camsensor_mt9d112_mu2m0yu_setup_camctrl_tbl(camctrl_tbl_type *camctrl_tbl_ptr)
{
camctrl_tbl_ptr->asf_5x5_is_supported         = FALSE; 
camctrl_tbl_ptr->la_is_supported              = FALSE;
camctrl_tbl_ptr->cs_is_supported              = FALSE;
camctrl_tbl_ptr->iso_is_supported             = FALSE;
camctrl_tbl_ptr->enable_rolloff_correction    = FALSE;
camctrl_tbl_ptr->hjr_bayer_filtering_enable   = FALSE;
camctrl_tbl_ptr->nightshot_is_supported       = FALSE;

camctrl_tbl_ptr->get_snapshot_frame_per_sec = camsensor_mt9d112_mu2m0yu_get_snapshot_fps;
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

LOCAL uint16 camsensor_mt9d112_mu2m0yu_get_snapshot_fps(uint16 fps)
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

static boolean mt9d112_mu2m0yu_i2c_write_word(uint16 offset, uint16 data) 
{
static uint16 swapbytes;
uint8 i;

swapbytes  = (data & 0xFF00) >> 8;
swapbytes |= (data & 0x00FF) << 8;

camsensor_i2c_command.addr.offset = offset;
camsensor_i2c_command.buf_ptr  = (byte *)(&swapbytes);
camsensor_i2c_command.len      = 2;

for (i = 0; i < 3; ++i) {
if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS) {
return TRUE;
}
}

return FALSE;
}

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
static boolean mt9d112_mu2m0yu_i2c_read_word(uint16 offset, uint16 *data) 
{
static uint16 swapbytes; 
uint8  i;

if (data == NULL) {
return FALSE;
}

camsensor_i2c_command.addr.offset = offset;
camsensor_i2c_command.buf_ptr  = (byte *)(&swapbytes);
camsensor_i2c_command.len      = 2;

for (i =0; i < 3; ++i) {
if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS) {
*data  = (swapbytes & 0x00FF) << 8;
*data |= (swapbytes & 0xFF00) >> 8;
return TRUE;
}
}
return FALSE;
} /* end of mt9d112_mu2m0yu_i2c_read_word. */



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

camera_ret_code_type camsensor_mt9d112_mu2moyu_set_wb(int8 wb)

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
static boolean initialize_mt9d112_mu2m0yu_registers(void)
{
int32 i, array_length;

/* PLL Setup Start */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x341E, 0x8F09))) {
return FALSE;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x341C, 0x0250))) {
return FALSE;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x341E, 0x8F09))) {
return FALSE;
}
camera_timed_wait(5);
if (!(mt9d112_mu2m0yu_i2c_write_word(0x341E, 0x8F08))) {
return FALSE;
}
/* PLL Setup End   */

array_length = sizeof (preview_snapshot_mode_reg_settings_array) / 
sizeof (preview_snapshot_mode_reg_settings_array[0]);
/* Configure sensor for Preview mode and Snapshot mode */
for (i = 0; i < array_length; i++) {
if (mt9d112_mu2m0yu_i2c_write_word(preview_snapshot_mode_reg_settings_array[i].register_address, 
    preview_snapshot_mode_reg_settings_array[i].register_value) == FALSE) {
return(FALSE);
}
}
/* Configure for Noise Reduction, Saturation and Aperture Correction */
array_length = sizeof (noise_reduction_reg_settings_array) / 
sizeof (noise_reduction_reg_settings_array[0]);
for (i = 0; i < array_length; i++) {
if (mt9d112_mu2m0yu_i2c_write_word(noise_reduction_reg_settings_array[i].register_address, 
   noise_reduction_reg_settings_array[i].register_value) == FALSE) {
return(FALSE);
}
}

/* Set Color Kill Saturation point to optimum value*/
if (!(mt9d112_mu2m0yu_i2c_write_word(0x35A4, 0x0593))) {
return FALSE;
}

/* Refresh Sequencer */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA103))) {
return FALSE;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x0005))) {
return FALSE;
}
camera_timed_wait(5);
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA103))) {
return FALSE;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x0006))) {
return FALSE;
}

if (!mt9d112_mu2m0yu_set_lens_roll_off()) {
return FALSE;
}

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
static camera_ret_code_type camsensor_mt9d112_mu2m0yu_set_frame_rate(uint16 fps)
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

if (!(mt9d112_mu2m0yu_i2c_write_word(0x341E, 0x8F09))) 
{
return CAMERA_FAILED;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x341E, 0x8F0B))) 
{
return CAMERA_FAILED;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x341C,n_m))) 
{
return CAMERA_FAILED;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x341E, 0x8F09))) 
{
return CAMERA_FAILED;
}
camera_timed_wait(5);
if (!(mt9d112_mu2m0yu_i2c_write_word(0x341E, 0x8F08))) 
{
return CAMERA_FAILED;
}
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
static boolean mt9d112_mu2m0yu_set_sensor_mode(sensor_mode_type sensor_mode)
{
uint16 mode;
uint16 loop_count = 100;
/* Declare the local Variables */
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA104))) 
{
return FALSE;
}
if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390, &mode))) 
{
return FALSE;
}
camera_timed_wait(20); /* delay ms */

if(mode==3 && sensor_mode==SENSOR_PREVIEW_MODE)
{
return TRUE;
}
else if(mode==7 && sensor_mode==SENSOR_SNAPSHOT_MODE)
{
return TRUE;
}


switch (sensor_mode) {

case SENSOR_PREVIEW_MODE:

if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA103))) {
return FALSE;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x0001))) { /* Do Preview */
return FALSE;
}
loop_count=100;

while (loop_count>0) 
{
    if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA104))) 
    {
        return FALSE;
    }
    if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390, &mode))) 
    {
        return FALSE;
    }
    camera_timed_wait(20); /* delay ms */
    if (mode==3)
        break;
    else
        --loop_count;
}
if(loop_count==0)
{
  return FALSE;
}
break;

case SENSOR_SNAPSHOT_MODE:



if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA121))) {  //no of frames to capture 
return FALSE;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x0005))) { 
return FALSE;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA120))) {
return FALSE;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x0002))) { /* Capture Video */
return FALSE;
}

if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA103))) {
return FALSE;
}
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3390, 0x0002))) { /* Capture Mode */
return FALSE;
}

camera_timed_wait(5); /* delay ms */

/* Wait for the sensor to be in Snapshot mode */
loop_count = 100;

while (loop_count > 0) 
{
if (!(mt9d112_mu2m0yu_i2c_write_word(0x338C, 0xA104))) 
{
return FALSE;
}
if (!(mt9d112_mu2m0yu_i2c_read_word(0x3390, &mode))) 
{
return FALSE;
}
camera_timed_wait(20); /* delay ms */
if (mode==7)
{
	break;
}
else
{
  loop_count--;
}
}

if(loop_count==0)
{
  return FALSE;
}
break;

default:
return FALSE;
}
return TRUE;
}
/* mt9d112_mu2m0yu_set_sensor_mode */

LOCAL boolean mt9d112_mu2m0yu_set_lens_roll_off(void)
{
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34CE, 0x81A0 ))) {
return FALSE;
}// LENS_CORRECTION_CONTROL
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34D0, 0x6331 ))) {
return FALSE;
}// ZONE_BOUNDS_X1_X2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34D2, 0x3394 ))) {
return FALSE;
}// ZONE_BOUNDS_X0_X3
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34D4, 0x9966 ))) {
return FALSE;
}// ZONE_BOUNDS_X4_X5
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34D6, 0x4B25 ))) {
return FALSE;
}// ZONE_BOUNDS_Y1_Y2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34D8, 0x2670 ))) {
return FALSE;
}// ZONE_BOUNDS_Y0_Y3
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34DA, 0x724C ))) {
return FALSE;
}// ZONE_BOUNDS_Y4_Y5
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34DC, 0xFFFD ))) {
return FALSE;
}// CENTER_OFFSET
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34DE, 0x00CA ))) {
return FALSE;
}// FX_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34E6, 0x00AC ))) {
return FALSE;
}// FY_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34EE, 0x0EE1 ))) {
return FALSE;
}// DF_DX_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34F6, 0x0D87 ))) {
return FALSE;
}// DF_DY_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3500, 0xE1F7 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_0_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3508, 0x1CF4 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_1_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3510, 0x1D28 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_2_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3518, 0x1F26 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_3_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3520, 0x2220 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_4_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3528, 0x333D ))) {
return FALSE;
}// SECOND_DERIV_ZONE_5_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3530, 0x15D9 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_6_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3538, 0xCFB8 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_7_RED
if (!(mt9d112_mu2m0yu_i2c_write_word(0x354C, 0x05FE ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_R_TL
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3544, 0x05F8 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_R_TR
if (!(mt9d112_mu2m0yu_i2c_write_word(0x355C, 0x0596 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_R_BL
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3554, 0x0611 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_R_BR
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34E0, 0x00F2 ))) {
return FALSE;
}// FX_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34E8, 0x00A8 ))) {
return FALSE;
}// FY_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34F0, 0x0F7B ))) {
return FALSE;
}// DF_DX_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34F8, 0x0CD7 ))) {
return FALSE;
}// DF_DY_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3502, 0xFEDB ))) {
return FALSE;
}// SECOND_DERIV_ZONE_0_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x350A, 0x13E4 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_1_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3512, 0x1F2C ))) {
return FALSE;
}// SECOND_DERIV_ZONE_2_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x351A, 0x1D20 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_3_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3522, 0x2422 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_4_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x352A, 0x2925 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_5_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3532, 0x1D04 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_6_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x353A, 0xFBF2 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_7_GREEN
if (!(mt9d112_mu2m0yu_i2c_write_word(0x354E, 0x0616 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_G1_TL
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3546, 0x0597 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_G1_TR
if (!(mt9d112_mu2m0yu_i2c_write_word(0x355E, 0x05CD ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_G1_BL
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3556, 0x0529 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_G1_BR
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34E4, 0x00B2 ))) {
return FALSE;
}// FX_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34EC, 0x005E ))) {
return FALSE;
}// FY_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34F4, 0x0F43 ))) {
return FALSE;
}// DF_DX_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34FC, 0x0E2F ))) {
return FALSE;
}// DF_DY_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3506, 0xF9FC ))) {
return FALSE;
}// SECOND_DERIV_ZONE_0_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x350E, 0x0CE4 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_1_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3516, 0x1E1E ))) {
return FALSE;
}// SECOND_DERIV_ZONE_2_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x351E, 0x1B19 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_3_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3526, 0x151B ))) {
return FALSE;
}// SECOND_DERIV_ZONE_4_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x352E, 0x1416 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_5_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3536, 0x10FC ))) {
return FALSE;
}// SECOND_DERIV_ZONE_6_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x353E, 0xC018 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_7_BLUE
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3552, 0x06B4 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_B_TL
if (!(mt9d112_mu2m0yu_i2c_write_word(0x354A, 0x0506 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_B_TR
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3562, 0x06AB ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_B_BL
if (!(mt9d112_mu2m0yu_i2c_write_word(0x355A, 0x063A ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_B_BR
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34E2, 0x00E5 ))) {
return FALSE;
}// FX_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34EA, 0x008B ))) {
return FALSE;
}// FY_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34F2, 0x0E4C ))) {
return FALSE;
}// DF_DX_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x34FA, 0x0CA3 ))) {
return FALSE;
}// DF_DY_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3504, 0x0907 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_0_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x350C, 0x1DFD ))) {
return FALSE;
}// SECOND_DERIV_ZONE_1_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3514, 0x1E24 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_2_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x351C, 0x2529 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_3_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3524, 0x1D20 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_4_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x352C, 0x2332 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_5_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3534, 0x10E9 ))) {
return FALSE;
}// SECOND_DERIV_ZONE_6_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x353C, 0x0BCB ))) {
return FALSE;
}// SECOND_DERIV_ZONE_7_GREEN2
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3550, 0x04EF ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_G2_TL
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3548, 0x0609 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_G2_TR
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3560, 0x0580 ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_G2_BL
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3558, 0x05DD ))) {
return FALSE;
}// K_FACTOR_IN_K_FX_FY_G2_BR
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3540, 0x0000 ))) {
return FALSE;
}// X2_FACTORS
if (!(mt9d112_mu2m0yu_i2c_write_word(0x3542, 0x0000 ))) {
return FALSE;
}// GLOBAL_OFFSET_FXY_FUNCTION
return TRUE;
} /* mt9d112_mu2m0yu_set_lens_roll_off */

#endif /* FEATURE_CAMERA,USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU */

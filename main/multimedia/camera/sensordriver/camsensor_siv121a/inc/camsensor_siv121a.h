#ifndef CAMSENSOR_SIV121A_H
#define CAMSENSOR_SIV121A_H

/*============================================================================

                 Camera Interface Device Driver Source File

   DESCRIPTION
     This file contains the definitions needed for the camera interface
     device driver.

============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 10/06/29 wangliang    Initial Version


============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "camsensor.h"
#include "camctrl.h"

// sensor's chip ID and version
#define SIV121A_SENSOR_ID       (0xa0)

#define SIV121A_I2C_SLAVE_ID (0x42)

#define IMAGE_SENSOR_VGA_WIDTH  (640)
#define IMAGE_SENSOR_VGA_HEIGHT (480)

/* CAMERA PREVIEW FRAME RATE DEFINITION */
#define CAM_PREVIEW_15FPS

#define SYSTEM_CLK                           (48*1000*1000)

/* PIXEL CLOCK USED BY BANDING FILTER CACULATION*/
#define PIXEL_CLK							         (SYSTEM_CLK/8)		// 52/8 MHz

/* SENSOR GLOBAL GAIN AT NIGHT MODE */
#define SENSOR_NIGHT_MODE_GAIN					0x08	// Please refer to OV7660 Implementation Guide for suitable value.

/* SENSOR PIXEL/LINE NUMBERS IN ONE PERIOD */
#define VGA_PERIOD_PIXEL_NUMS						916//784
#define VGA_PERIOD_LINE_NUMS						524//510

/* SENSOR EXPOSURE LINE LIMITATION */
#define VGA_EXPOSURE_LIMITATION					524//510 //509//508

/* 1M RESOLUTION SIZE */
#define IMAGE_SENSOR_1M_WIDTH					1280
#define IMAGE_SENSOR_1M_HEIGHT					960

#define IMAGE_SENSOR_2M_WIDTH					1600
#define IMAGE_SENSOR_2M_HEIGHT					1200

/* SENSOR VGA SIZE */
#define IMAGE_SENSOR_VGA_WIDTH					(640)
#define IMAGE_SENSOR_VGA_HEIGHT					(480)

/* SETUP TIME NEED TO BE INSERTED */
#define IMAGE_SENSOR_VGA_INSERTED_PIXELS		72//128//56//(0)   
#define IMAGE_SENSOR_VGA_INSERTED_LINES		    34//17//(1)

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_INIT

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

boolean camsensor_siv121a_init(camsensor_function_table_type *camsensor_function_table_ptr,
                                     camctrl_tbl_type              *camctrl_tbl_ptr);


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_START

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

boolean camsensor_siv121a_start(camsensor_static_params_type *camsensor_params);


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_SNAPSHOT_CONFIG

DESCRIPTION
              Configure the camera sensor for Snapshot 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean camsensor_siv121a_snapshot_config
(
  camsensor_static_params_type  *camsensor_params /* Other config params */
);

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_RAW_SNAPSHOT_CONFIG

DESCRIPTION
              Configure the camera sensor for RAW Snapshot

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean camsensor_siv121a_raw_snapshot_config
(
   camsensor_static_params_type  *camsensor_params /* Other config params */
);

/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_VIDEO_CONFIG

DESCRIPTION
              Configure the camera sensor for Preview

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean camsensor_siv121a_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
);


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_POWER_UP

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camsensor_siv121a_power_up(void);


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_POWER_DOWN

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camsensor_siv121a_power_down(void);


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_WRITE_EXPOSURE_GAIN

DESCRIPTION   UPdate exposure gain on sensor

DEPENDENCIES
  None

INPUT
  uint16 gain - value for new gain param
  uint32 line_count - num lines in whole gain table
  
RETURN VALUE
  camera_ret_code_type
  CAMERA_SUCCESS - if no error comes back from I2C
  CAMERA_FAILED  - if I2C reported an error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camsensor_siv121a_write_exposure_gain(uint16 gain, uint32 line);


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_MOVE_FOCUS

DESCRIPTION
  Move focus either near or toward infinity the number of steps
  specified.

DEPENDENCIES
  None

INPUT
  camsensor_move_focus_enum_type: - either of the two following
    CAMSENSOR_MOVE_FOCUS_NEAR - moves to near end for closer objects
    CAMSENSOR_MOVE_FOCUS_FAR - moves to the far end for more distant objs
  int32 num_steps - num steps to move focal motor

RETURN VALUE
  camera_ret_code_type
  CAMERA_SUCCESS - if no error comes back from I2C
  CAMERA_FOCUS_SENSOR_FAILED  - if I2C reported an error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camsensor_siv121a_move_focus(camsensor_move_focus_enum_type  direction,
                                                  int32                           num_steps);


/*===========================================================================

FUNCTION      CAMSENSOR_SIV121A_SET_DEFAULT_FOCUS

DESCRIPTION
  Move focus to location best suited to any subject: at the nearest 
  point where infinity is still in focus.
  specified.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type
  CAMERA_SUCCESS - if no error comes back from I2C
  CAMERA_FOCUS_SENSOR_FAILED  - if I2C reported an error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camsensor_siv121a_set_default_focus(void);

/*============================================================================

FUNCTION      

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
camera_ret_code_type camsensor_siv121a_set_brightness(int8 brightness);


/*============================================================================

FUNCTION      

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
camera_ret_code_type camsensor_siv121a_set_contrast(int8 contrast);




/*===========================================================================

FUNCTION      

DESCRIPTION
  Set the frames per second rate for preview and snapshot mode.

DEPENDENCIES
  None

RETURN VALUE
  CAMERA_SUCCESS        if successful
  CAMERA_NOT_SUPPORTED  if sensor does not support function;
  CAMERA_INVALID_PARM   if invalid fps

SIDE EFFECTS
  None

============================================================================*/
camera_ret_code_type camsensor_siv121a_set_frame_rate
(
  uint16 fps
);

#endif /* CAMSENSOR_SIV100B_H */



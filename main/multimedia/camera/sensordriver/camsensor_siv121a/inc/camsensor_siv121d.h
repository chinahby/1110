#ifndef CAMSENSOR_SIV121D_0M3_H
#define CAMSENSOR_SIV121D_0M3_H

/*============================================================================

                 Camera Interface Device Driver Source File for 
                    Micron MT9D113 2 Mega Pixel CMOS sensor

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
 09/10/06  khk    Initial Version


============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "camsensor.h"
#include "camctrl.h"

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

/*============================================================================
                        DATA DECLARATIONS
============================================================================*/

/*============================================================================
                          MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                          EXPORTED FUNCTIONS
============================================================================*/


uint8 camsensor_SIV121D_ycbcr_active(void);
uint8 camsensor_SIV121D_ycbcr_unactive(void);
/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_INIT

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
                                     camctrl_tbl_type              *camctrl_tbl_ptr);


/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_START

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

boolean camsensor_siv121d_0m3_start(camsensor_static_params_type *camsensor_params);


/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_SNAPSHOT_CONFIG

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

boolean camsensor_siv121d_0m3_snapshot_config
(
  camsensor_static_params_type  *camsensor_params /* Other config params */
);

/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_RAW_SNAPSHOT_CONFIG

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

boolean camsensor_siv121d_0m3_raw_snapshot_config
(
   camsensor_static_params_type  *camsensor_params /* Other config params */
);

/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_VIDEO_CONFIG

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

boolean camsensor_siv121d_0m3_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
);


/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_POWER_UP

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camsensor_siv121d_0m3_power_up(void);


/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_POWER_DOWN

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camsensor_siv121d_0m3_power_down(void);


/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_WRITE_EXPOSURE_GAIN

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
camera_ret_code_type camsensor_siv121d_0m3_write_exposure_gain(uint16 gain, uint32 line);


/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_MOVE_FOCUS

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
camera_ret_code_type camsensor_siv121d_0m3_move_focus(camsensor_move_focus_enum_type  direction,
                                                  int32                           num_steps);


/*===========================================================================

FUNCTION      CAMSENSOR_OV7675_OV0M3_SET_DEFAULT_FOCUS

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
camera_ret_code_type camsensor_siv121d_0m3_set_default_focus(void);


#endif /* CAMSENSOR_OV7675_OV0M3_H */





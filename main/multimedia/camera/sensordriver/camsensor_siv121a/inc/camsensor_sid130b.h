#ifndef CAMSENSOR_SID130B_H
#define CAMSENSOR_SID130B_H

/*============================================================================

                 Camera Interface Device Driver Source File
                   for OmniVision 2630 2.0 Mega Pixel sensor

   DESCRIPTION
     This file contains the definitions needed for the camera interface
     device driver.

   Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header:   //Pvcs-srv/NJ_VM_1/archives/Walle/Walle_V1.2/multimedia/camera/sensordriver/omnivision2650_2mp/inc/camsensor_SID130B_2m0.h-arc   1.0   Jul 25 2009 16:51:28   F3060335  $ $DateTime: 2008/05/09 16:34:47 $ $Author:   F3060335  $

 when      who    what, where, why
 --------  ----  ----------------------------------------------------------
 12/01/05  yl    Adopted from camsensor_OV2650 

============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "camsensor.h"
#include "camctrl.h"
/*============================================================================
                          EXTERNAL API PROTOTYPES
============================================================================*/

/*===========================================================================

FUNCTION      CAMSENSOR_OV2650_OV2M0_INIT

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

boolean camsensor_SID130B_init(camsensor_function_table_type *camsensor_function_table_ptr,
                                    camctrl_tbl_type              *camctrl_tbl_ptr);


uint8 camsensor_SID130B_active(void);
uint8 camsensor_SID130B_unactive(void);

/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_START

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

boolean camsensor_SID130B_2m0_start( camsensor_static_params_type *camsensor_params);


/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_SNAPSHOT_CONFIG

DESCRIPTION
              Configure the camera sensor for snapshot 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean camsensor_SID130B_2m0_snapshot_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
);

/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_RAW_SNAPSHOT_CONFIG

DESCRIPTION
              Configure the camera sensor and the camera interface

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean camsensor_SID130B_2m0_raw_snapshot_config
(
   camsensor_static_params_type *camsensor_params /* Other config params */
);


/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_VIDEO_CONFIG

DESCRIPTION
              Configure the camera sensor and the camera interface

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean camsensor_SID130B_2m0_video_config
(
  camsensor_static_params_type *camsensor_params /* Other config params */
);


/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_POWER_UP

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camsensor_SID130B_2m0_power_up(void);


/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_POWER_DOWN

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camsensor_SID130B_2m0_power_down(void);


/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_WRITE_EXPOSURE_GAIN

DESCRIPTION   Update exposure gain on sensor

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
camera_ret_code_type camsensor_SID130B_2m0_write_exposure_gain(uint16 gain, uint32 line_count);


/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_EOF_I2C_UPDATE

DESCRIPTION
              Update I2C at End of Frame.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/

void camsensor_SID130B_2m0_eof_i2c_update (void);


/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_REGISTER_TO_REAL_GAIN

DESCRIPTION   Converts Exposure Table gain to a real gain value.

DEPENDENCIES
  None

INPUT
  uint16 reg_gain - value of register gain to convert

RETURN VALUE
  float gain - real gain in linear scale

SIDE EFFECTS
  None

===========================================================================*/
float    camsensor_SID130B_2m0_register_to_real_gain (uint16 reg_gain);


/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_REAL_TO_REGISTER_GAIN

DESCRIPTION   Converts real sensor gain to a valid register value to write
              directly to sensor.  Used to generate antibanding tables.

DEPENDENCIES
  None

INPUT
  float gain - real gain in linear scale

RETURN VALUE
  uint16 reg_gain - value of register gain to convert

SIDE EFFECTS
  None

===========================================================================*/
uint16    camsensor_SID130B_2m0_real_to_register_gain (float gain);


/*===========================================================================

FUNCTION      CAMSENSOR_SID130B_OV2M0_SET_FRAME_RATE

DESCRIPTION   Set the Frames Per Second Rate in Preview mode.

DEPENDENCIES
  None

INPUT
  uint16 fps - New Frames Per Second rate in Q8 format

RETURN VALUE
  camera_ret_code_type
  CAMERA_SUCCESS - if we changed frame rate successfully
  CAMERA_NOT_SUPPORTED - function not implemented
  CAMERA_FAILED  - if frame rate change failed

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type    camsensor_SID130B_2m0_set_frame_rate (uint16 fps);


GPIO_ValueType   camsensor_SID130B_get_active_valule(void);

#endif /* CAMSENSOR_SID130B_H */
